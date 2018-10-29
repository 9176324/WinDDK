/*++
    Copyright (c) 2000,2001 Microsoft Corporation

    Module Name:
        HidBtn.c

    Abstract:
        Tablet PC Buttons HID Driver.

    Environment:
        Kernel mode

--*/

#include "pch.h"
#define MODULE_ID                       0

#ifdef ALLOC_PRAGMA
  #pragma alloc_text(INIT, DriverEntry)
  #pragma alloc_text(PAGE, HbtnCreateClose)
  #pragma alloc_text(PAGE, HbtnAddDevice)
  #pragma alloc_text(PAGE, HbtnUnload)
#endif  //ifdef ALLOC_PRAGMA

/*++
    @doc    EXTERNAL

    @func   NTSTATUS | DriverEntry |
            Installable driver initialization entry point.
            <nl>This entry point is called directly by the I/O system.

    @parm   IN PDRIVER_OBJECT | DrvObj | Points to the driver object.
    @parm   IN PUNICODE_STRINT | RegPath | Points to the registry path.

    @rvalue SUCCESS | returns STATUS_SUCCESS.
    @rvalue FAILURE | returns NT status code.
--*/

NTSTATUS EXTERNAL
DriverEntry(
    __in PDRIVER_OBJECT  DrvObj,
    __in PUNICODE_STRING RegPath
    )
{
    
    NTSTATUS                    status = STATUS_SUCCESS;
    HID_MINIDRIVER_REGISTRATION hidMinidriverRegistration = {0};

    TEnter(Func,("(DrvObj=%p,RegPath=%p)\n", DrvObj, RegPath));

    gDriverObj = DrvObj;
    DrvObj->MajorFunction[IRP_MJ_CREATE] =
    DrvObj->MajorFunction[IRP_MJ_CLOSE] = HbtnCreateClose;

    DrvObj->MajorFunction[IRP_MJ_INTERNAL_DEVICE_CONTROL] = HbtnInternalIoctl;

    DrvObj->MajorFunction[IRP_MJ_PNP]   = HbtnPnp;
    DrvObj->MajorFunction[IRP_MJ_POWER] = HbtnPower;
    DrvObj->DriverUnload                = HbtnUnload;
    DrvObj->DriverExtension->AddDevice  = HbtnAddDevice;

    //
    // Register with HIDCLASS.SYS module
    //
    RtlZeroMemory(&hidMinidriverRegistration,
                  sizeof(hidMinidriverRegistration));

    hidMinidriverRegistration.Revision            = HID_REVISION;
    hidMinidriverRegistration.DriverObject        = DrvObj;
    hidMinidriverRegistration.RegistryPath        = RegPath;
    hidMinidriverRegistration.DeviceExtensionSize = sizeof(DEVICE_EXTENSION);
    hidMinidriverRegistration.DevicesArePolled    = FALSE;

    status = HidRegisterMinidriver(&hidMinidriverRegistration);

    if (!NT_SUCCESS(status))
    {
        LogError(ERRLOG_MINIDRV_REG_FAILED,
                 status,
                 UNIQUE_ERRID(0x10), NULL, NULL);
         TErr(("failed to register mini driver.\n"));
    }

    TExit(Func,("=%x\n", status));
    return status;
}       //DriverEntry

/*++
    @doc    EXTERNAL

    @func   NTSTATUS | HbtnCreateClose |
            Process the create and close IRPs sent to this device.

    @parm   IN PDEVICE_OBJECT | DevObj | Points to the device object.
    @parm   IN PIRP | Irp | Points to an I/O Request Packet.

    @rvalue SUCCESS | returns STATUS_SUCCESS.
    @rvalue FAILURE | returns NT status code.
--*/

NTSTATUS EXTERNAL
HbtnCreateClose(
    IN PDEVICE_OBJECT DevObject,
    IN PIRP Irp
    )
{
    NTSTATUS            status = STATUS_SUCCESS;
    PIO_STACK_LOCATION  irpsp = NULL;

    PAGED_CODE ();

    UNREFERENCED_PARAMETER(DevObject);
    irpsp = IoGetCurrentIrpStackLocation(Irp);

    TEnter(Func,("(DevObject=%p,Irp=%p,IrpStack=%p,Major=%s)\n",
                DevObject, Irp, irpsp,
                LookupName(irpsp->MajorFunction, MajorIrpNames)));

    switch(irpsp->MajorFunction)
    {
        case IRP_MJ_CREATE:
        case IRP_MJ_CLOSE:
            Irp->IoStatus.Information = 0;
            break;

        default:
            status = STATUS_INVALID_PARAMETER;
            break;
    }

    Irp->IoStatus.Status = status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    TExit(Func,("=%x\n", status));
    return status;
}       //HbtnCreateClose

/*++
    @doc    EXTERNAL

    @func   NTSTATUS | HbtnAddDevice |
            Called by hidclass, allows us to initialize our device extensions.

    @parm   IN PDRIVER_OBJECT | DrvObj | Points to the driver object.
    @parm   IN PDEVICE_OBJECT | DevObj |
            Points to a functional device object created by hidclass.

    @rvalue SUCCESS | Returns STATUS_SUCCESS.
    @rvalue FAILURE | Returns NT status code.
--*/

NTSTATUS EXTERNAL
HbtnAddDevice(
    __in PDRIVER_OBJECT DrvObj,
    __in PDEVICE_OBJECT DevObj
    )
{
    NTSTATUS            status = STATUS_SUCCESS;
    PDEVICE_EXTENSION   devext = NULL;

    PAGED_CODE ();
    TEnter(Func,("(DrvObj=%p,DevObj=%p)\n", DrvObj, DevObj));
    
    UNREFERENCED_PARAMETER(DrvObj);

    devext = GET_MINIDRIVER_DEVICE_EXTENSION(DevObj);
    RtlZeroMemory(devext, sizeof(*devext));
    devext->self = DevObj;
    devext->LowerDevObj = GET_NEXT_DEVICE_OBJECT(DevObj);
    IoInitializeRemoveLock(&devext->RemoveLock, HBTN_POOL_TAG, 0, 10);
    KeInitializeSpinLock(&devext->QueueLock);
    KeInitializeSpinLock(&devext->DataLock);
    InitializeListHead(&devext->PendingIrpList);
    IoCsqInitialize(&devext->IrpQueue, HbtnInsertIrp, HbtnRemoveIrp, 
        HbtnPeekNextIrp, HbtnAcquireLock, HbtnReleaseLock, HbtnCompleteCancelledIrp);

    status = OemAddDevice(devext);
    if (NT_SUCCESS(status))
    {
        DevObj->Flags &= ~DO_DEVICE_INITIALIZING;
        DevObj->Flags |= DO_POWER_PAGABLE;
    }

    TExit(Func,("=%x\n", status));
    return status;
}       //HbtnAddDevice

/*++
    @doc    EXTERNAL

    @func   void | HbtnUnload | Free all the allocated resources, etc.

    @parm   IN PDRIVER_OBJECT | DrvObj | Points to the driver object.

    @rvalue None.
--*/

VOID EXTERNAL
HbtnUnload(
    __in PDRIVER_OBJECT DrvObj
    )
{
    
    PAGED_CODE();

    TEnter(Func,("(DrvObj=%p)\n", DrvObj));

    TAssert(DrvObj->DeviceObject == NULL);
    UNREFERENCED_PARAMETER(DrvObj);

    TExit(Func,("!\n"));
    return;
}       //HbtnUnload


/*++
    @doc    INTERNAL

    @func   VOID | AcquireLock | Cancel safe queue callback for locking

    @parm   IN PIO_CSQ | csq | Points to cancel safe queue.
    @parm   IN PKIRQL | Irql | Points to KIRQL for lock.
    
--*/
__drv_raisesIRQL(DISPATCH_LEVEL)
__drv_maxIRQL(DISPATCH_LEVEL)
VOID 
HbtnAcquireLock(
   __in                                   PIO_CSQ csq, 
   __out __drv_out_deref(__drv_savesIRQL) PKIRQL Irql
   )
{
    PDEVICE_EXTENSION DevExt =  GET_DEV_EXT(csq);
    //
    // Suppressing because the address below csq is valid since it's
    // part of DEVICE_EXTENSION structure.
    //
#pragma prefast(suppress: __WARNING_BUFFER_UNDERFLOW, "Underflow using expression 'DevExt->QueueLock'")
    KeAcquireSpinLock(&DevExt->QueueLock, Irql);
} //HbtnAcquireLock


/*++
    @doc    INTERNAL

    @func   VOID | ReleaseLock | Cancel safe queue callback for lock release

    @parm   IN PIO_CSQ | csq | Points to cancel safe queue.
    @parm   IN PKIRQL | Irql | Points to KIRQL for lock.
    
--*/
__drv_requiresIRQL(DISPATCH_LEVEL)
VOID 
HbtnReleaseLock(
   __in                                PIO_CSQ csq,
   __in __drv_in(__drv_restoresIRQL)   KIRQL   Irql
   )
{
    PDEVICE_EXTENSION DevExt =  GET_DEV_EXT(csq);
    //
    // Suppressing because the address below csq is valid since it's
    // part of DEVICE_EXTENSION structure.
    //
#pragma prefast(suppress: __WARNING_BUFFER_UNDERFLOW, "Underflow using expression 'DevExt->QueueLock'")
    KeReleaseSpinLock(&DevExt->QueueLock, Irql);
} // HbtnReleaseLock

/*++
    @doc    INTERNAL

    @func   VOID | InsertIrp | Cancel safe queue callback for irp insertion

    @parm   IN PIO_CSQ | csq | Points to cancel safe queue.
    @parm   IN PIRP | Irp | Points to Irp to insert.
    
--*/
VOID 
HbtnInsertIrp(
   __in PIO_CSQ csq, 
   __in PIRP Irp
   )
{
    PDEVICE_EXTENSION DevExt =  GET_DEV_EXT(csq);
    //
    // Suppressing because the address below csq is valid since it's
    // part of DEVICE_EXTENSION structure.
    //
#pragma prefast(suppress: __WARNING_BUFFER_UNDERFLOW, "Underflow using expression 'DevExt->PendingIrpList'")
    InsertTailList(&DevExt->PendingIrpList, &Irp->Tail.Overlay.ListEntry);
} // HbtnInsertIrp

/*++
    @doc    INTERNAL

    @func   PIRP | PeekNextIrp | Cancel safe queue callback for peeking at irp in CSQ

    @parm   IN PIO_CSQ | csq | Points to cancel safe queue.
    @parm   IN PIRP | Irp | Points to Irp 
     @parm  IN PVOID | PeekContext | Points to context.
    
--*/
PIRP 
HbtnPeekNextIrp(
   __in PIO_CSQ csq, 
   __in PIRP Irp, 
   __in PVOID PeekContext
   )
{
    PDEVICE_EXTENSION DevExt =  GET_DEV_EXT(csq);
    PIRP                    nextIrp = NULL;
    PLIST_ENTRY             nextEntry;
    PLIST_ENTRY             listHead;
    PIO_STACK_LOCATION     irpStack;    
   
    listHead = &DevExt->PendingIrpList;
    // 
    // If the IRP is NULL, we will start peeking from the listhead, else
    // we will start from that IRP onwards. This is done under the
    // assumption that new IRPs are always inserted at the tail.
    //
        
    if(Irp == NULL) {       
    //
    // Suppressing because the address below csq is valid since it's
    // part of DEVICE_EXTENSION structure.
    //
#pragma prefast(suppress: __WARNING_BUFFER_UNDERFLOW, "Underflow using expression 'DevExt->PendingIrpList'")
        nextEntry = listHead->Flink;
    } else {
        nextEntry = Irp->Tail.Overlay.ListEntry.Flink;
    }
    
    while(nextEntry != listHead) {
        
        nextIrp = CONTAINING_RECORD(nextEntry, IRP, Tail.Overlay.ListEntry);

        irpStack = IoGetCurrentIrpStackLocation(nextIrp);
        
        //
        // If context is present, continue until you find a matching one.
        // Else you break out as you got next one.
        //
        
        if(PeekContext) {
            if(irpStack->FileObject == (PFILE_OBJECT) PeekContext) {       
                break;
            }
        } else {
            break;
        }
        nextIrp = NULL;
        nextEntry = nextEntry->Flink;
    }

    return nextIrp;
} //HbtnPeekNextIrp

/*++
    @doc    INTERNAL

    @func   VOID | RemoveIrp | Cancel safe queue callback for irp removal

    @parm   IN PIO_CSQ | csq | Points to cancel safe queue.
    @parm   IN PIRP | Irp | Points to Irp to remove.
    
--*/
VOID 
HbtnRemoveIrp(
   __in PIO_CSQ csq, 
   __in PIRP Irp
   )
{
    UNREFERENCED_PARAMETER(csq);
    RemoveEntryList(&Irp->Tail.Overlay.ListEntry);
}//HbtnRemoveIrp

/*++
    @doc    INTERNAL

    @func   VOID | CompleteCancelledIrp | Cancel safe queue callback for irp completion 

    @parm   IN PIO_CSQ | csq | Points to cancel safe queue.
    @parm   IN PIRP | Irp | Points to Irp to complete.
    
--*/

VOID 
HbtnCompleteCancelledIrp(
   __in PIO_CSQ csq, 
   __in PIRP Irp
   )
{
    UNREFERENCED_PARAMETER(csq);
    Irp->IoStatus.Status = STATUS_CANCELLED;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
} //HbtnCompleteCancelledIrp

