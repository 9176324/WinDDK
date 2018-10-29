/*++
    Copyright (c) Microsoft Corporation

    Module Name:
        serial.c

    Abstract:
        Functions to talk to the serial port.

    Environment:
        Kernel mode

--*/

#include "pch.h"

#define MODULE_ID                       3

#ifdef ALLOC_PRAGMA
  #pragma alloc_text(PAGE, SerialSyncSendIoctl)
  #pragma alloc_text(PAGE, SerialSyncReadWritePort)
  #pragma alloc_text(PAGE, SerialAsyncReadWritePort)
#endif /* ALLOC_PRAGMA */

/*++
    @doc    INTERNAL

    @func   NTSTATUS | SerialSyncSendIoctl |
            Performs a synchronous ioctl request to the serial port.

    @parm   IN ULONG | IoctlCode | ioctl code.
    @parm   IN PDEVICE_OBJECT | DevObj | Points to the device object.
    @parm   IN PVOID | InBuffer OPTIONAL | Points to the input buffer.
    @parm   IN ULONG | InBufferLen | Specifies the size of the input buffer.
    @parm   OUT PVOID | OutBuffer OPTIONAL | Points to the output buffer.
    @parm   IN ULONG | OutBufferLen | Specifies the size of the output buffer.
    @parm   IN BOOLEAN | fInternal | If TRUE, an internal ioctl is sent.
    @parm   OUT PIO_STATUS_BLOCK | Iosb | Points to the io status block.

    @rvalue SUCCESS | returns STATUS_SUCCESS
    @rvalue FAILURE | returns NT status code
--*/

NTSTATUS INTERNAL
SerialSyncSendIoctl(
    __in ULONG          IoctlCode,
    __in PDEVICE_OBJECT DevObj,
    __in_bcount_opt(InBufferLen) PVOID          InBuffer OPTIONAL,
    __in ULONG          InBufferLen,
    __out_bcount_opt(OutBufferLen) PVOID         OutBuffer,
    __in ULONG          OutBufferLen,
    __in BOOLEAN        fInternal,
    __out PIO_STATUS_BLOCK Iosb
    )
{
    NTSTATUS status = STATUS_SUCCESS;
    KEVENT event;
    PIRP irp;
    LARGE_INTEGER Timeout;
    
    Timeout.QuadPart = Int32x32To64(1000, -10000);

    PAGED_CODE();

    TEnter(Func, ("(Ioctl=%s,DevObj=%p,InBuff=%p,InLen=%d,OutBuff=%p,OutLen=%d,fInternal=%x,Iosb=%p)",
                  LookupName(IoctlCode,
                             fInternal? SerialInternalIoctlNames: SerialIoctlNames),
                  DevObj, InBuffer, InBufferLen, OutBuffer, OutBufferLen,
                  fInternal, Iosb));

    KeInitializeEvent(&event, NotificationEvent, FALSE);
    irp = IoBuildDeviceIoControlRequest(IoctlCode,
                                        DevObj,
                                        InBuffer,
                                        InBufferLen,
                                        OutBuffer,
                                        OutBufferLen,
                                        fInternal,
                                        &event,
                                        Iosb);

    __analysis_assume(Iosb->Status != STATUS_PENDING);

    if (irp != NULL)
    {
        status = IoCallDriver(DevObj, irp);
        if (status == STATUS_PENDING)
        {
            status = KeWaitForSingleObject(&event,
                                           Executive,
                                           KernelMode,
                                           FALSE,
                                           &Timeout);
            if (status == STATUS_TIMEOUT)
            {
                LogError(ERRLOG_SERIAL_ACCESS_FAILED,
                         status,
                         UNIQUE_ERRID(0x40),
                         NULL,
                         NULL);
                TWarn(("Failed accessing COM (status=%x).", status));
                //
                // STATUS_TIMEOUT is not an error so NT_SUCCESS will still
                // succeed, so map it to STATUS_IO_TIMEOUT which is an
                // error.
                //
                status = STATUS_IO_TIMEOUT;
            }
        }

        if (status == STATUS_SUCCESS)
        {
            status = Iosb->Status;
        }
    }
    else
    {
        status = STATUS_INSUFFICIENT_RESOURCES;
        LogError(ERRLOG_INSUFFICIENT_RESOURCES,
                 status,
                 UNIQUE_ERRID(0x10),
                 NULL,
                 NULL);
        TErr(("Failed to build ioctl IRP."));
    }

    TExit(Func, ("=%x", status));
    return status;
}       //SerialSyncSendIoctl

/*++
    @doc    INTERNAL

    @func   NTSTATUS | SerialAsyncReadWriteData |
            Read/Write data from/to the Serial Port asynchornously.

    @parm   IN BOOLEAN | fRead | If TRUE, the access is a Read.
    @parm   IN PDEVICE_EXTENSION | DevExt | Points to the device extension.
    @parm   IN PIRP | Irp | Points to an I/O Request Packet.
    @parm   IN PUCHAR | Buffer | Points to the data buffer.
    @parm   IN ULONG | BuffLen | Specifies the data buffer length.
    @parm   IN PIO_COMPLETION_ROUTINE | CompletionRoutine |
            Points to the completion callback routine.
    @parm   IN PVOID | Context | Callback context.

    @rvalue SUCCESS | returns STATUS_SUCCESS
    @rvalue FAILURE | returns NT status code
--*/

NTSTATUS INTERNAL
SerialAsyncReadWriteData(
    __in BOOLEAN                fRead,
    __in PDEVICE_EXTENSION      DevExt,
    __in PIRP                   Irp,
    __in PUCHAR                 Buffer,
    __in ULONG                  BuffLen,
    __in PIO_COMPLETION_ROUTINE CompletionRoutine,
    __in PVOID                  Context
    )
{
    NTSTATUS status;
    PIO_STACK_LOCATION irpsp;

    TEnter(Func, ("(fRead=%x,DevExt=%p,Irp=%p,Buff=%p,BuffLen=%d,Context=%p)",
                  fRead, DevExt, Irp, Buffer, BuffLen,
                  Context));

    TAssert(Buffer != NULL);
    TAssert(BuffLen > 0);

    Irp->AssociatedIrp.SystemBuffer = Buffer;
    irpsp = IoGetNextIrpStackLocation(Irp);
    RtlZeroMemory(irpsp, sizeof(*irpsp));
    irpsp->Parameters.Read.Length = BuffLen;
    irpsp->Parameters.Read.ByteOffset.QuadPart = 0;
    irpsp->MajorFunction = fRead? IRP_MJ_READ: IRP_MJ_WRITE;
    IoSetCompletionRoutine(Irp,
                           CompletionRoutine,
                           Context,
                           TRUE,
                           TRUE,
                           TRUE);

    status = IoCallDriver(DevExt->LowerDevObj, Irp);

    TExit(Func, ("=%x", status));
    return status;
}       //SerialAsyncReadWriteData

/*++
    @doc    INTERNAL

    @func   NTSTATUS | SerialAsyncReadWritePort | Read/Write data from/to
            the Serial Port asynchronously.

    @parm   IN BOOLEAN | fRead | If TRUE, the access is a Read.
    @parm   IN PDEVICE_EXTENSION | DevExt | Points to the device extension.
    @parm   IN PUCHAR | Buffer | Points to the data buffer.
    @parm   IN ULONG | BuffLen | Specifies the data buffer length.

    @rvalue SUCCESS | returns STATUS_SUCCESS
    @rvalue FAILURE | returns NT status code
--*/

NTSTATUS INTERNAL
SerialAsyncReadWritePort(
    __in BOOLEAN           fRead,
    __in PDEVICE_EXTENSION DevExt,
    __in_bcount_opt(BuffLen) PUCHAR            Buffer,
    __in ULONG             BuffLen
    )
{
    NTSTATUS status;
    PIRP irp;
    LARGE_INTEGER StartingOffset = {0};

    PAGED_CODE();

    TEnter(Func, ("(fRead=%x,DevExt=%p,Buff=%p,BuffLen=%d)",
                  fRead, DevExt, Buffer, BuffLen));

    TAssert(Buffer != NULL);
    TAssert(BuffLen > 0);

    irp = IoBuildAsynchronousFsdRequest(fRead? IRP_MJ_READ: IRP_MJ_WRITE,
                                        DevExt->LowerDevObj,
                                        Buffer,
                                        BuffLen,
                                        &StartingOffset,
                                        NULL);
    if (irp != NULL)
    {
        status = IoAcquireRemoveLock(&DevExt->RemoveLock, irp);
        if (NT_SUCCESS(status))
        {
            IoSetCompletionRoutine(irp,
                                   AsyncReadWriteCompletion,
                                   DevExt,
                                   TRUE,
                                   TRUE,
                                   TRUE);
            TEnter(Func, (".IoCallDriver(DevObj=%p,Irp=%p)",
                          DevExt->LowerDevObj, irp));
            status = IoCallDriver(DevExt->LowerDevObj, irp);
            TExit(Func, (".IoCallDriver=%x", status));

            if (!NT_SUCCESS(status))
            {
                LogError(ERRLOG_SERIAL_ACCESS_FAILED,
                         status,
                         UNIQUE_ERRID(0x20),
                         NULL,
                         NULL);
                TErr(("Failed to send async read/write irp to COM (status=%x).",
                      status));
            }
        }
        else
        {
            IoFreeIrp(irp);
            TWarn(("Failed to acquire remove lock (status=%x).", status));
        }
    }
    else
    {
        status = STATUS_INSUFFICIENT_RESOURCES;
        LogError(ERRLOG_INSUFFICIENT_RESOURCES,
                 status,
                 UNIQUE_ERRID(0x30),
                 NULL,
                 NULL);
        TErr(("Failed to allocate asynchronous IRP."));
    }

    TExit(Func, ("=%x", status));
    return status;
}       //SerialAsyncReadWritePort

/*++
    @doc    INTERNAL

    @func   NTSTATUS | AsyncReadWriteCompletion | Completion routine for
            AsyncReadWritePort.

    @parm   IN PDEVICE_OBJECT | DevObj | Points to the device object.
    @parm   IN PIRP | Irp | Points to an I/O request packet.
    @parm   IN PDEVICE_EXTENSION | DevExt | Points to the device extension.

    @rvalue SUCCESS | Returns STATUS_SUCCESS.
    @rvalue FAILURE | Returns NT status code.
--*/

NTSTATUS INTERNAL
AsyncReadWriteCompletion(
    __in PDEVICE_OBJECT    DevObj,
    __in PIRP              Irp,
    __in_xcount_opt("varies") PVOID             Context
    )
{
    NTSTATUS status = Irp->IoStatus.Status;
    PDEVICE_EXTENSION DevExt = (PDEVICE_EXTENSION)Context;

#ifdef _DBG
    TEnter(Func, ("(DevObj=%p,Irp=%p,devext=%p,Status=%x)",
                  DevObj, Irp, DevExt, status));
#else
    UNREFERENCED_PARAMETER(DevObj);
#endif
    
    

    if (status == STATUS_CANCELLED)
    {
        TWarn(("AsyncReadWrite IRP was cancelled."));
    }
    else if (!NT_SUCCESS(status))
    {
        TWarn(("AsyncReadWrite irp failed (status=%x).", status));
    }

    if(NULL != DevExt)
    {
        IoReleaseRemoveLock(&DevExt->RemoveLock, Irp);
    }
    IoFreeIrp(Irp);
    status = STATUS_MORE_PROCESSING_REQUIRED;
#ifdef _DBG
    TExit(Func, ("=%x", status));
#endif
    return status;
}       //AsyncReadWriteCompletion

/*++
    @doc    INTERNAL

    @func   NTSTATUS | SerialSyncReadWritePort |
            Read/Write data from/to the Serial Port.

    @parm   IN BOOLEAN | fRead | If TRUE, the access is a Read.
    @parm   IN PDEVICE_EXTENSION | DevExt | Points to the device extension.
    @parm   IN PUCHAR | Buffer | Points to the data buffer.
    @parm   IN ULONG | BuffLen | Specifies the data buffer length.
    @parm   IN PLARGE_INTEGER | Timeout | Points to an optional timeout value. if NULL is passed in we default to 1 sec.
    @parm   OUT PULONG | BytesAccessed | Optionally returns number of bytes
            accessed.

    @rvalue SUCCESS | returns STATUS_SUCCESS
    @rvalue FAILURE | returns NT status code
--*/

NTSTATUS INTERNAL
SerialSyncReadWritePort(
    __in BOOLEAN           fRead,
    __in PDEVICE_EXTENSION DevExt,
    __in_bcount_opt(BuffLen) PUCHAR            Buffer,
    __in ULONG             BuffLen,
    __in PIRPLOCK          lock,
    __in_opt PLARGE_INTEGER    Timeout,
    __out_opt PULONG           BytesAccessed
    )
{
    NTSTATUS status;
    KEVENT event;
    PIRP irp;
    LARGE_INTEGER StartingOffset = {0};
    IO_STATUS_BLOCK iosb;
    LARGE_INTEGER DefaultTimeout;
    PLARGE_INTEGER    WaitTimeout = Timeout;   

    PAGED_CODE();

    TEnter(Func, ("(fRead=%x,DevExt=%p,Buff=%p,BuffLen=%d,pTimeout=%p,pBytesAccessed=%p)",
                  fRead, DevExt, Buffer, BuffLen, Timeout, BytesAccessed));

    TAssert(Buffer != NULL);
    TAssert(BuffLen > 0);

    // we have a default timeout of one second. If the caller 
    // requests an endless wait, we will change the request to 
    // a 1 sec wait. This way we won't wait infinitely if an
    //irp we send down runs into problems.
    DefaultTimeout.QuadPart = Int32x32To64(1000, -10000);
    if(WaitTimeout == NULL)
    {
        WaitTimeout =  &DefaultTimeout;
    }

    if (BytesAccessed != NULL)
    {
        *BytesAccessed = 0;
    }
    KeInitializeEvent(&event, NotificationEvent, FALSE);
    irp = IoBuildSynchronousFsdRequest(fRead? IRP_MJ_READ: IRP_MJ_WRITE,
                                       DevExt->LowerDevObj,
                                       Buffer,
                                       BuffLen,
                                       &StartingOffset,
                                       &event,
                                       &iosb);
    
    __analysis_assume(iosb.Status != STATUS_PENDING);

    if (irp != NULL)
    {
        IoSetCompletionRoutine(irp,
                                SyncReadWriteCompletion,
                                lock,
                                TRUE,
                                TRUE,
                                TRUE
                                );

        TEnter(Func, (".IoCallDriver(DevObj=%p,Irp=%p)",
                      DevExt->LowerDevObj, irp));
        status = IoCallDriver(DevExt->LowerDevObj, irp);
        TExit(Func, (".IoCallDriver=%x", status));

        if (status == STATUS_PENDING)
        {
            status = KeWaitForSingleObject(&event,
                                           Executive,
                                           KernelMode,
                                           FALSE,
                                           WaitTimeout);
            if (status == STATUS_TIMEOUT)
            {
                
                if (InterlockedExchange((PLONG)lock, IRPLOCK_CANCEL_STARTED) == IRPLOCK_CANCELABLE) 
                {
                    IoCancelIrp(irp);
                    if(InterlockedExchange((PLONG)lock, IRPLOCK_CANCEL_COMPLETE) == IRPLOCK_COMPLETED) 
                    {
                        //we need to reset the lock state in preparation for the 
                        //next call to SerialSyncReadWritePort
                        *lock = IRPLOCK_CANCELABLE; 
                        IoCompleteRequest(irp, IO_NO_INCREMENT);
                    }
                }
                iosb.Status = status;
                status = KeWaitForSingleObject(&event,
		                              Executive,
		                              KernelMode,
		                              FALSE,
		                              NULL);
                if(STATUS_SUCCESS != status)
                {
                    TErr(("KeWait returned: status=%x", status));
                }
                LogError(ERRLOG_SERIAL_ACCESS_FAILED,
                         status,
                         UNIQUE_ERRID(0x40),
                         NULL,
                         NULL);
                TWarn(("Failed accessing COM (status=%x).", iosb.Status));
                //
                // STATUS_TIMEOUT is not an error so NT_SUCCESS will still
                // succeed, so map it to STATUS_IO_TIMEOUT which is an
                // error.
                //
                status = STATUS_IO_TIMEOUT;
            }
            else
            {
                //
                // Irp was completed at the same time we timed out, so
                // we don't really have a timeout.
                //
                status = STATUS_SUCCESS;
            }
        }
        if (status == STATUS_SUCCESS)
        {
            status = iosb.Status;
            if(NT_SUCCESS(status) &&
	            (BytesAccessed != NULL))
            {
	            *BytesAccessed = (ULONG)iosb.Information;
            }			
        }
        else
        {
            LogError(ERRLOG_SERIAL_ACCESS_FAILED,
                     status,
                     UNIQUE_ERRID(0x40),
                     NULL,
                     NULL);
            TWarn(("Failed accessing COM (status=%x).", status));
        }
        
       
    }
    else
    {
        status = STATUS_INSUFFICIENT_RESOURCES;
        LogError(ERRLOG_INSUFFICIENT_RESOURCES,
                 status,
                 UNIQUE_ERRID(0x50),
                 NULL,
                 NULL);
        TErr(("Failed to allocate synchronous IRP."));
    }

    TExit(Func, ("=%x (BytesAccessed=%Id)", status, iosb.Information));
    return status;
}       //SerialSyncReadWritePort

/*++
    @doc    INTERNAL

    @func   NTSTATUS | SyncReadWriteCompletion | Completion routine for
            SyncReadWritePort.

    @parm   IN PDEVICE_OBJECT | DevObj | Points to the device object.
    @parm   IN PIRP | Irp | Points to an I/O request packet.
    @parm   IN PVOID | Context | Points to lock state.

    @rvalue CONTINUE | STATUS_CONTINUE_COMPLETION.
    @rvalue STOP | STATUS_MORE_PROCESSING_REQUIRED.
--*/
NTSTATUS
SyncReadWriteCompletion(
    __in PDEVICE_OBJECT   DevObj,
    __in PIRP             Irp,
    __in_xcount_opt("varies") PVOID            Context
    )
{
    
#ifdef DGB
    NTSTATUS status = Irp->IoStatus.Status;
    TEnter(Func, ("(DevObj=%p,Irp=%p,Contextt=%p,Status=%x)",
                  DevObj, Irp, Context, status));
#else
    UNREFERENCED_PARAMETER(DevObj);
    UNREFERENCED_PARAMETER(Irp);
    UNREFERENCED_PARAMETER(Context);    
#endif

    if(NULL != Context)
    {
        if (InterlockedExchange((PLONG)Context, IRPLOCK_COMPLETED) == IRPLOCK_CANCEL_STARTED) {
            //
            // Main line code has got the control of the IRP. It will
            // now take the responsibility of completing the IRP.
            // Therefore...
            return STATUS_MORE_PROCESSING_REQUIRED;
        }
        //we need to reset the lock state in preparation for the 
        //next call to SerialSyncReadWritePort
        *(PLONG)Context = IRPLOCK_CANCELABLE;
    }
    return STATUS_CONTINUE_COMPLETION ;
}   //SyncReadWriteCompletion

