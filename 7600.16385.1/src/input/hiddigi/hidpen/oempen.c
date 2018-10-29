/*++
    Copyright (c) Microsoft Corporation

    Module Name:
        oempen.c

    Abstract:
        Contains OEM common functions.

    Environment:
        Kernel mode

--*/

#include "pch.h"

#define MODULE_ID                       4

//
// Type definitions
//


typedef struct _READ_WORKITEM
{
    PIO_WORKITEM      WorkItem;
    PIRP              Irp;
    PHID_INPUT_REPORT HidReport;
} READ_WORKITEM, *PREAD_WORKITEM;

//
// Local function prototype.
//
IO_COMPLETION_ROUTINE ReadReportCompletion;


NTSTATUS INTERNAL
ProcessResyncBuffer(
    __in PDEVICE_EXTENSION DevExt,
    __in PIRP              Irp
    );

NTSTATUS INTERNAL
ProcessInputData(
    __in PDEVICE_EXTENSION DevExt,
    __in PIRP              Irp,
    __in PHID_INPUT_REPORT HidReport
    );

IO_WORKITEM_ROUTINE ReadMoreBytes;

VOID INTERNAL
ReadMoreBytes(
    IN PDEVICE_OBJECT DevObj,
    IN PVOID  Context
    );

#ifdef ALLOC_PRAGMA
  #pragma alloc_text(PAGE, OemAddDevice)
  #pragma alloc_text(PAGE, OemStartDevice)
  #pragma alloc_text(PAGE, OemStopDevice)
#endif  //ifdef ALLOC_PRAGMA

/*++
    @doc    INTERNAL

    @func   NTSTATUS | OemAddDevice | OEM specific AddDevice code.

    @parm   IN PDEVICE_EXTENSION | DevExt | Points to the device extension.

    @rvalue SUCCESS | Returns STATUS_SUCCESS.
    @rvalue FAILURE | Returns NT status code.
--*/

NTSTATUS INTERNAL
OemAddDevice(
    __in PDEVICE_EXTENSION DevExt
    )
{
    NTSTATUS status;

    PAGED_CODE ();
#ifdef _DBG
    TEnter(Func, ("(DevExt=%p)", DevExt));
#else
    UNREFERENCED_PARAMETER(DevExt);
#endif

    status = STATUS_SUCCESS;
#ifdef _DBG
    TExit(Func, ("=%x", status));
#endif
    return status;
}       //OemAddDevice

/*++
    @doc    INTERNAL

    @func   NTSTATUS | OemStartDevice | Start pen tablet device.

    @parm   IN PDEVICE_EXTENSION | DevExt | Points to the device extension.
    @parm   IN PIRP | Irp | Points to an I/O request packet.

    @rvalue SUCCESS | returns STATUS_SUCCESS
    @rvalue FAILURE | returns NT status code
--*/

NTSTATUS INTERNAL
OemStartDevice(
     __in PDEVICE_EXTENSION DevExt,
     __in PIRP              Irp
    )
{
    NTSTATUS status;
    PIO_STACK_LOCATION irpspNext = IoGetNextIrpStackLocation(Irp);
    NTSTATUS PrevStatus = Irp->IoStatus.Status;
    ULONG_PTR PrevInfo = Irp->IoStatus.Information;

    PAGED_CODE();
    TEnter(Func, ("(DevExt=%p,Irp=%p)", DevExt, Irp));
    TAssert(!(DevExt->dwfHPen & HPENF_DEVICE_STARTED));

    RtlZeroMemory(irpspNext, sizeof(*irpspNext));
    irpspNext->MajorFunction = IRP_MJ_CREATE;
    status = SendSyncIrp(DevExt->LowerDevObj, Irp, FALSE);
    if (NT_SUCCESS(status))
    {
        IO_STATUS_BLOCK iosb;

        DevExt->dwfHPen |= HPENF_SERIAL_OPENED;
        Irp->IoStatus.Status = PrevStatus;
        Irp->IoStatus.Information = PrevInfo;
        status = SerialSyncSendIoctl(IOCTL_SERIAL_INTERNAL_BASIC_SETTINGS,
                                     DevExt->LowerDevObj,
                                     NULL,
                                     0,
                                     &DevExt->OemData.PrevSerialSettings,
                                     sizeof(DevExt->OemData.PrevSerialSettings),
                                     TRUE,
                                     &iosb);
        if (NT_SUCCESS(status))
        {
            DevExt->dwfHPen |= HPENF_SERIAL_INITED;
            status = OemInitSerialPort(DevExt);
            if (NT_SUCCESS(status))
            {
                status = OemInitDevice(DevExt);
            }
        }
        else
        {
            LogError(ERRLOG_INIT_SERIAL_FAILED,
                     status,
                     UNIQUE_ERRID(0x08),
                     NULL,
                     NULL);
            TErr(("Failed to set basic settings of COM (status=%x).", status));
        }
    }
    else
    {
        LogError(ERRLOG_OPEN_SERIAL_FAILED,
                 status,
                 UNIQUE_ERRID(0x10),
                 NULL,
                 NULL);
        TErr(("Failed to send Create IRP to serial port (status=%x).", status));
    }

    if (!NT_SUCCESS(status))
    {
        OemStopDevice(DevExt, Irp);
    }

    TExit(Func, ("=%x", status));
    return status;
}       //OemStartDevice

/*++
    @doc    INTERNAL

    @func   NTSTATUS | OemStopDevice | Uninitialize the device.

    @parm   IN PDEVICE_EXTENSION | DevExt | Points to the device extension.
    @parm   IN PIRP | Irp | Points to an I/O request packet.

    @rvalue SUCCESS | returns STATUS_SUCCESS
    @rvalue FAILURE | returns NT status code
--*/

NTSTATUS INTERNAL
OemStopDevice(
    __in PDEVICE_EXTENSION DevExt,
    __in PIRP              Irp
    )
{
    NTSTATUS status;
    IO_STATUS_BLOCK iosb;
    BOOLEAN fFailed = FALSE;

    PAGED_CODE();
    TEnter(Func, ("(DevExt=%p,Irp=%p)", DevExt, Irp));
    TAssert(!(DevExt->dwfHPen & HPENF_DEVICE_STARTED));

    if (DevExt->dwfHPen & HPENF_SERIAL_INITED)
    {
        DevExt->dwfHPen &= ~HPENF_SERIAL_INITED;
        status = SerialSyncSendIoctl(IOCTL_SERIAL_INTERNAL_RESTORE_SETTINGS,
                                     DevExt->LowerDevObj,
                                     &DevExt->OemData.PrevSerialSettings,
                                     sizeof(DevExt->OemData.PrevSerialSettings),
                                     NULL,
                                     0,
                                     TRUE,
                                     &iosb);
        if (!NT_SUCCESS(status))
        {
            fFailed = TRUE;
            LogError(ERRLOG_RESTORE_SERIAL_FAILED,
                     status,
                     UNIQUE_ERRID(0x20),
                     NULL,
                     NULL);
            TWarn(("Failed to restore COM settings (status=%x).", status));
        }
    }

    if (DevExt->dwfHPen & HPENF_SERIAL_OPENED)
    {
        PIO_STACK_LOCATION irpspNext;

        DevExt->dwfHPen &= ~HPENF_SERIAL_OPENED;
        irpspNext = IoGetNextIrpStackLocation(Irp);
        RtlZeroMemory(irpspNext, sizeof(*irpspNext));
        irpspNext->MajorFunction = IRP_MJ_CLEANUP;
        status = SendSyncIrp(DevExt->LowerDevObj, Irp, FALSE);
        if (!NT_SUCCESS(status))
        {
            fFailed = TRUE;
            TErr(("Failed to send a Cleanup IRP to the serial port (status=%x).",
                  status));
        }

        irpspNext = IoGetNextIrpStackLocation(Irp);
        RtlZeroMemory(irpspNext, sizeof(*irpspNext));
        irpspNext->MajorFunction = IRP_MJ_CLOSE;
        status = SendSyncIrp(DevExt->LowerDevObj, Irp, FALSE);
        if (!NT_SUCCESS(status))
        {
            fFailed = TRUE;
            TErr(("Failed to send a Close IRP to the serial port (status=%x).",
                  status));
        }
    }

    status = fFailed? STATUS_UNSUCCESSFUL: STATUS_SUCCESS;

    TExit(Func, ("=%x", status));
    return status;
}       //OemStopDevice

/*++
    @doc    INTERNAL

    @func   NTSTATUS | OemReadReport | Send the read request down to
            serial.sys,

    @parm   IN PDEVICE_EXTENSION | DevExt | Points to the device extension.
    @parm   IN PIRP | Irp | Points to an I/O Request Packet.
    @parm   IN PIRP | fSentDown | Points to BOOLEAN for checking if the IRP was sent down the stack

    @rvalue SUCCESS | returns STATUS_SUCCESS.
    @rvalue FAILURE | returns NT status code.
--*/

NTSTATUS INTERNAL
OemReadReport(
    __in  PDEVICE_EXTENSION DevExt,
    __in  PIRP              Irp,
    __out BOOLEAN           *fSentDown
    )
{


    NTSTATUS status;
    PHID_INPUT_REPORT HidReport = (PHID_INPUT_REPORT)Irp->UserBuffer;
    KIRQL OldIrql;

    TEnter(Func, ("(DevExt=%p,Irp=%p)", DevExt, Irp));

    KeAcquireSpinLock(&DevExt->SpinLock, &OldIrql);
    status = ProcessResyncBuffer(DevExt, Irp);
    KeReleaseSpinLock(&DevExt->SpinLock, OldIrql);


    if (!NT_SUCCESS(status))
    {
        //
        // If we don't have enough bytes in the resync buffer or the packet
        // in the resync buffer is invalid, send an IRP down to read some
        // more.
        //

        status = SerialAsyncReadWriteData(TRUE,
                                          DevExt,
                                          Irp,
                                          HidReport->Report.RawInput,
                                          sizeof(OEM_INPUT_REPORT),
                                          ReadReportCompletion,
                                          HidReport);
       //if the IRP is sent down the stack it should not be touched
       //anymore.
       if(NULL != fSentDown)
       {
          *fSentDown = TRUE;
       }
       else
       {
         TWarn(("fSentDown is NULL!"));
       }
    }
    TExit(Func, ("=%x", status));
    return status;
}       //OemReadReport



/*++
    @doc    INTERNAL

    @func   NTSTATUS | OemWriteReport | Handle write report requests.

    @parm   IN PDEVICE_EXTENSION | DevExt | Points to the device extension.
    @parm   IN PIRP | Irp | Points to an I/O request packet.

    @rvalue Returns STATUS_NOT_SUPPORTED.
--*/


NTSTATUS INTERNAL
OemWriteReport(
    __in PDEVICE_EXTENSION DevExt,
    __in PIRP Irp
    )
{

    NTSTATUS status = STATUS_NOT_SUPPORTED;
#ifdef _DBG
    TEnter(Func, ("(DevExt=%p,Irp=%p)", DevExt, Irp));
#else
    UNREFERENCED_PARAMETER(DevExt);
    UNREFERENCED_PARAMETER(Irp);
#endif

    LogError(ERRLOG_NOT_SUPPORTED,
             status,
             UNIQUE_ERRID(0x30),
             NULL,
             NULL);
    TWarn(("WriteReport is unsupported."));
#ifdef _DBG
    TExit(Func, ("=%x", status));
#endif
    return status;
}       //OemWriteReport


/*++
    @doc    INTERNAL


    @func   NTSTATUS | ReadReportCompletion | Completion routine for ReadReport.

    @parm   IN PDEVICE_OBJECT | DevObj | Points to the device object.
    @parm   IN PIRP | Irp | Points to an I/O request packet.
    @parm   IN PHID_INPUT_REPORT | HidReport | Points to input data packet.

    @rvalue SUCCESS | Returns STATUS_SUCCESS.
    @rvalue FAILURE | Returns NT status code.
--*/


NTSTATUS INTERNAL
ReadReportCompletion(
    IN PDEVICE_OBJECT    DevObj,
    IN PIRP              Irp,
    IN PVOID             Context
    )
{
    NTSTATUS status = Irp->IoStatus.Status;
    PHID_INPUT_REPORT HidReport = (PHID_INPUT_REPORT)Context;

    PDEVICE_EXTENSION devext = GET_MINIDRIVER_DEVICE_EXTENSION(DevObj);

    TEnter(Func, ("(DevObj=%p,Irp=%p,HidReport=%p,Status=%x)",
                  DevObj, Irp, HidReport, status));

    if (status == STATUS_CANCELLED)
    {
        TWarn(("ReadReport IRP was cancelled."));
        status = STATUS_SUCCESS;
    }
    else if (!NT_SUCCESS(status))
    {
        TWarn(("Failed to read input data packet (status=%x).", status));
        status = STATUS_SUCCESS;
    }
    else
    {
        status = ProcessInputData(devext, Irp, HidReport);

    }
    if (status != STATUS_MORE_PROCESSING_REQUIRED)
    {
        if (Irp->PendingReturned)
        {
            IoMarkIrpPending(Irp);
        }
        IoReleaseRemoveLock(&devext->RemoveLock, Irp);
    }    
    TExit(Func, ("=%x", status));
    return status;
}       //ReadReportCompletion


/*++
    @doc    INTERNAL

    @func   NTSTATUS | ProcessResyncBuffer |
            Process input data from the resync buffer.
            Note that this function must be called at IRQL==DISPATCH_LEVEL

    @parm   IN PDEVICE_EXTENSION | DevExt | Points to the device extension.
    @parm   IN PIRP | Irp | Points to an I/O request packet.

    @rvalue SUCCESS | Returns STATUS_SUCCESS.
    @rvalue FAILURE | Returns STATUS_MORE_PROCESSING_REQUIRED
                      (We want the IRP back).
--*/


NTSTATUS INTERNAL
ProcessResyncBuffer(
    __in PDEVICE_EXTENSION DevExt,
    __in PIRP              Irp
    )
{
    NTSTATUS status = STATUS_DATA_ERROR;
    PHID_INPUT_REPORT HidReport = (PHID_INPUT_REPORT)Irp->UserBuffer;

    TEnter(Func, ("(DevExt=%p,Irp=%p,Len=%d,status=%x,xData=%x,yData=%x)",
                  DevExt, Irp, DevExt->OemData.BytesInBuff,
                  DevExt->OemData.ResyncData[0].InputReport.bStatus,
                  DevExt->OemData.ResyncData[0].InputReport.wXData,
                  DevExt->OemData.ResyncData[0].InputReport.wYData));

    TAssert(KeGetCurrentIrql() == DISPATCH_LEVEL);
    while (DevExt->OemData.BytesInBuff >= sizeof(OEM_INPUT_REPORT))
    {
        if (OemIsResyncDataValid(DevExt))
        {
            status = OemNormalizeInputData(DevExt,
                                           &DevExt->OemData.ResyncData[0]);
            if (NT_SUCCESS(status))
            {
                HidReport->ReportID = REPORTID_PEN;
#ifdef _TIMESTAMP_
                HidReport->TimeStamp = DevExt->OemData.LastCounter;
#endif
                RtlCopyMemory(HidReport->Report.RawInput,
                              &DevExt->OemData.ResyncData[0],
                              sizeof(OEM_INPUT_REPORT));
                Irp->IoStatus.Information = sizeof(HID_INPUT_REPORT);
            }

            DevExt->OemData.BytesInBuff -= sizeof(OEM_INPUT_REPORT);
            if (DevExt->OemData.BytesInBuff > 0)
            {
                RtlMoveMemory(&DevExt->OemData.ResyncData[0],
                              &DevExt->OemData.ResyncData[1],
                              DevExt->OemData.BytesInBuff);
            }

            if (NT_SUCCESS(status))
            {
                break;
            }
        }
    }

    TExit(Func, ("=%x (status=%x,xData=%x,yData=%x)",
                 status,
                 HidReport->Report.InputReport.bStatus,
                 HidReport->Report.InputReport.wXData,
                 HidReport->Report.InputReport.wYData));
    return status;
}//ProcessResyncBuffer


/*++
    @doc    INTERNAL

    @func   NTSTATUS | ProcessInputData |
            OEM specific code to process input data.

    @parm   IN PDEVICE_EXTENSION | DevExt | Points to the device extension.
    @parm   IN PIRP | Irp | Points to an I/O request packet.
    @parm   IN PHID_INPUT_REPORT | HidReport | Points to hid report packet.

    @rvalue SUCCESS | Returns STATUS_SUCCESS.
    @rvalue FAILURE | Returns STATUS_MORE_PROCESSING_REQUIRED
                     (We want the IRP back).
--*/


NTSTATUS INTERNAL
ProcessInputData(
    __in PDEVICE_EXTENSION DevExt,
    __in PIRP              Irp,
    __in PHID_INPUT_REPORT HidReport
    )
{
    NTSTATUS status;
    KIRQL OldIrql;

    TEnter(Func,
           ("(DevExt=%p,Irp=%p,HidReport=%p,Len=%Id,status=%x,xData=%x,yData=%x)",
            DevExt, Irp, HidReport, Irp->IoStatus.Information,
            HidReport->Report.InputReport.bStatus,
            HidReport->Report.InputReport.wXData,
            HidReport->Report.InputReport.wYData));

    KeAcquireSpinLock(&DevExt->SpinLock, &OldIrql);

    if ((DevExt->OemData.BytesInBuff == 0) &&
        (Irp->IoStatus.Information == sizeof(OEM_INPUT_REPORT)) &&
        IsValidPacket(&HidReport->Report))
    {
        status = OemNormalizeInputData(DevExt, &HidReport->Report);
        if (NT_SUCCESS(status))
        {
            HidReport->ReportID = REPORTID_PEN;
#ifdef _TIMESTAMP_
            HidReport->TimeStamp = DevExt->OemData.LastCounter;
#endif
            Irp->IoStatus.Information = sizeof(HID_INPUT_REPORT);
        }
    }
    else
    {
        //
        // Either resync buffer already has something in it or packet is
        // partial or invalid, so append data to resync buffer and process
        // it again.
        //

        RtlMoveMemory((PUCHAR)&DevExt->OemData.ResyncData[0] +
                      DevExt->OemData.BytesInBuff,
                      &HidReport->Report,
                      Irp->IoStatus.Information);
        DevExt->OemData.BytesInBuff += (ULONG)Irp->IoStatus.Information;
        TAssert(DevExt->OemData.BytesInBuff <= sizeof(DevExt->OemData.ResyncData));
        status = ProcessResyncBuffer(DevExt, Irp);
    }

    if (!NT_SUCCESS(status))
    {
        status = IoAcquireRemoveLock(&DevExt->RemoveLock, Irp);
        if (!NT_SUCCESS(status))
        {
            LogError(ERRLOG_DEVICE_REMOVED,
                     status,
                     UNIQUE_ERRID(0x40),
                     NULL,
                     NULL);
            TErr(("Device was removed (status=%x).", status));
        }
        else
        {
            PREAD_WORKITEM ReadWorkItem;

            //
            // No valid data packet, send another IRP down to read more.
            //
            ReadWorkItem = ExAllocatePoolWithTag(NonPagedPool,
                                                 sizeof(READ_WORKITEM),
                                                 HPEN_POOL_TAG);
            if (NULL != ReadWorkItem)
            {
                ReadWorkItem->WorkItem = IoAllocateWorkItem(DevExt->DevObj);
                if (NULL != ReadWorkItem->WorkItem )
                {
                    ReadWorkItem->Irp = Irp;
                    ReadWorkItem->HidReport = HidReport;
                    IoQueueWorkItem(ReadWorkItem->WorkItem,
                                    ReadMoreBytes,
                                    DelayedWorkQueue,
                                    ReadWorkItem);

                    status = STATUS_MORE_PROCESSING_REQUIRED;
                }
                else
                {
                    status = STATUS_INSUFFICIENT_RESOURCES;
                    LogError(ERRLOG_INSUFFICIENT_RESOURCES,
                             status,
                             UNIQUE_ERRID(0x50),
                             NULL,
                             NULL);
                    TErr(("Failed to allocate read work item."));
                }
            }
            else
            {
                status = STATUS_INSUFFICIENT_RESOURCES;
                LogError(ERRLOG_INSUFFICIENT_RESOURCES,
                         status,
                         UNIQUE_ERRID(0x60),
                         NULL,
                         NULL);
                TErr(("Failed to allocate read work item data."));
            }
        }
        if(status != STATUS_MORE_PROCESSING_REQUIRED)
        {
            //if we were unable to queue the work item then we need to release the remove lock.
            IoReleaseRemoveLock(&DevExt->RemoveLock, Irp);
        }
    }
    KeReleaseSpinLock(&DevExt->SpinLock, OldIrql);

    TExit(Func, ("=%x (status=%x,xData=%x,yData=%x)",
                 status, HidReport->Report.InputReport.bStatus,
                 HidReport->Report.InputReport.wXData,
                 HidReport->Report.InputReport.wYData));
    return status;
}       //ProcessInputData

/*++
    @doc    INTERNAL

    @func   NTSTATUS | ReadMoreBytes | Read more bytes to resync packet.

    @parm   IN PDEVICE_OBJECT | DevObj | Points to the device object.
    @parm   IN PREAD_WORKITEM | ReadWorkItem | Points to the read work item.
    @parm   IN PIRP | Irp | Points to an I/O request packet.

    @rvalue None.
--*/


VOID INTERNAL
ReadMoreBytes(
    __in PDEVICE_OBJECT DevObj,
    __in_opt PVOID Context
    )
{
    PDEVICE_EXTENSION devext;
    ULONG BytesToRead;
    KIRQL OldIrql;
    PREAD_WORKITEM ReadWorkItem = (PREAD_WORKITEM)Context;

    TEnter(Func, ("(DevObj=%p,ReadWorkItem=%p)", DevObj, ReadWorkItem));

    if(NULL != ReadWorkItem)
    {
        devext = GET_MINIDRIVER_DEVICE_EXTENSION(DevObj);

        KeAcquireSpinLock(&devext->SpinLock, &OldIrql);
        IoReleaseRemoveLock(&devext->RemoveLock, ReadWorkItem->Irp);
        BytesToRead = sizeof(OEM_INPUT_REPORT) -
                      devext->OemData.BytesInBuff%sizeof(OEM_INPUT_REPORT);
        KeReleaseSpinLock(&devext->SpinLock, OldIrql);

        TInfo(("Read %d more bytes", BytesToRead));
        SerialAsyncReadWriteData(TRUE,
                                 devext,
                                 ReadWorkItem->Irp,
                                 ReadWorkItem->HidReport->Report.RawInput,
                                 BytesToRead,
                                 ReadReportCompletion,
                                 ReadWorkItem->HidReport);
        IoFreeWorkItem(ReadWorkItem->WorkItem);
        ExFreePool(ReadWorkItem);
    }

    TAssert(KeGetCurrentIrql() == PASSIVE_LEVEL);

    TExit(Func, ("!"));
    return;
}       //ReadMoreBytes


