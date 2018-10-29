/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    fail_driver3.c

Abstract:

    This is a sample driver that contains intentionally placed
    code defects in order to illustrate how Static Driver Verifier
    works. This driver is not functional and not intended as a 
    sample for real driver development projects.

Environment:

    Kernel mode

--*/

#include "fail_driver3.h"

#define _DRIVER_NAME_ "fail_driver3"

#ifndef __cplusplus
#pragma alloc_text (INIT, DriverEntry)
#pragma alloc_text (PAGE, DriverAddDevice)
#pragma alloc_text (PAGE, DispatchCreate)
#pragma alloc_text (PAGE, DispatchClose)
#pragma alloc_text (PAGE, DispatchCleanup)
#pragma alloc_text (PAGE, DispatchRead)
#pragma alloc_text (PAGE, DispatchWrite)
#pragma alloc_text (PAGE, DispatchDeviceControl)
#pragma alloc_text (PAGE, DispatchInternalDeviceControl)
#pragma alloc_text (PAGE, DispatchPower)
#pragma alloc_text (PAGE, DispatchSystemControl)
#pragma alloc_text (PAGE, DispatchPnp)
#pragma alloc_text (PAGE, DriverUnload)
#endif

NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT  DriverObject,
    IN PUNICODE_STRING RegistryPath
    )
{
    UNREFERENCED_PARAMETER(RegistryPath);

    DriverObject->MajorFunction[IRP_MJ_CREATE]                  = DispatchCreate;
    DriverObject->MajorFunction[IRP_MJ_CLOSE]                   = DispatchClose;
    DriverObject->MajorFunction[IRP_MJ_CLEANUP]                 = DispatchCleanup;
    DriverObject->MajorFunction[IRP_MJ_READ]                    = DispatchRead;
    DriverObject->MajorFunction[IRP_MJ_WRITE]                   = DispatchWrite;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL]          = DispatchDeviceControl;
    DriverObject->MajorFunction[IRP_MJ_INTERNAL_DEVICE_CONTROL] = DispatchInternalDeviceControl;
    DriverObject->MajorFunction[IRP_MJ_POWER]                   = DispatchPower;
    DriverObject->MajorFunction[IRP_MJ_SYSTEM_CONTROL]          = DispatchSystemControl;
    DriverObject->MajorFunction[IRP_MJ_PNP]                     = DispatchPnp;

    DriverObject->DriverExtension->AddDevice = DriverAddDevice;
    DriverObject->DriverUnload               = DriverUnload;

    return STATUS_SUCCESS;
}

NTSTATUS
DriverAddDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT PhysicalDeviceObject
    )
{
    NTSTATUS       status = STATUS_SUCCESS;
    PDEVICE_OBJECT deviceObject = NULL;
    PFDO_DATA      fdoData;

    status = IoCreateDevice (DriverObject,
                             sizeof (FDO_DATA),
                             NULL,
                             FILE_DEVICE_UNKNOWN,
                             FILE_DEVICE_SECURE_OPEN,
                             FALSE,
                             &deviceObject);

    if(!NT_SUCCESS (status))
    {
        return status;
    }

    fdoData = (PFDO_DATA) deviceObject->DeviceExtension;

    fdoData->UnderlyingPDO = PhysicalDeviceObject;

    fdoData->Self = deviceObject;

    fdoData->NextLowerDriver = IoAttachDeviceToDeviceStack (deviceObject,
                                                            fdoData->UnderlyingPDO);
    if(NULL == fdoData->NextLowerDriver)
    {
        IoDeleteDevice(deviceObject);

        return STATUS_NO_SUCH_DEVICE;
    }

    return status;
}

NTSTATUS
DispatchCreate (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{   
    //
    // This defect is injected for the "DanglingDeviceObjectReference"
    // rule only for the x64 code path.
    //

    PDEVICE_OBJECT deviceObject = NULL;

    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);

    deviceObject = IoGetAttachedDeviceReference(DeviceObject);

    #if defined (_X86_)

    ObDereferenceObject(deviceObject);

    #endif
	
    return STATUS_SUCCESS;
}

NTSTATUS
DispatchClose (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{   
    //
    // This defect is injected for the "ForwardedAtBadIrql" rule.
    //

    NTSTATUS status;
    KIRQL    oldIrql;   

    KeRaiseIrql(DISPATCH_LEVEL, &oldIrql);

    Irp->IoStatus.Status = STATUS_SUCCESS;

    IoSkipCurrentIrpStackLocation (Irp);

    status = IoCallDriver (((PFDO_DATA)DeviceObject->DeviceExtension)->NextLowerDriver, Irp);

    return status;
}

NTSTATUS
DispatchCleanup (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{   
    //
    // This defect is injected for the "IrqlDispatch" 
    // rule only for the x64 code path.
    //

    KSPIN_LOCK spinLock;
    KIRQL      oldIrql;

    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);

    #if defined (_X86_)

    KeRaiseIrql(DISPATCH_LEVEL, &oldIrql);

    #elif defined (_AMD64_)

    KeRaiseIrql(APC_LEVEL, &oldIrql);    

    #endif

    KeAcquireSpinLockAtDpcLevel(&spinLock);   

    KeReleaseSpinLockFromDpcLevel(&spinLock);
	
    return STATUS_SUCCESS;
}

NTSTATUS
DispatchRead (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{  
    //
    // This defect is injected for the "IrqlExAllocatePool" rule.
    //

    PVOID memory = NULL;
    KIRQL oldIrql;

    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);
    

    #if DBG==1

    KeRaiseIrql(APC_LEVEL, &oldIrql);
 
    #else
   
    KeRaiseIrql(DISPATCH_LEVEL, &oldIrql);		
  
    #endif    
   
    memory = ExAllocatePoolWithTag(PagedPool,
                                   sizeof(FDO_DATA),
                                   'abcd'
                                   );

    if (NULL == memory) {

        return STATUS_INSUFFICIENT_RESOURCES;

    }

    return STATUS_SUCCESS;
}

NTSTATUS
DispatchWrite (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{  
    //
    // This defect is injected for the "IrqlExApcLte3" rule.
    //

    KIRQL     oldIrql;
    ERESOURCE resource;

    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);

    KeRaiseIrql(HIGH_LEVEL, &oldIrql);

    if(!ExAcquireSharedStarveExclusive(&resource,
                                       FALSE
                                       )) {

        return STATUS_UNSUCCESSFUL;

    }

    ExReleaseResourceLite(&resource);

    return STATUS_SUCCESS;
}

NTSTATUS
DispatchDeviceControl (
    IN  PDEVICE_OBJECT  DeviceObject,
    IN  PIRP            Irp
    )
{   
    //
    // This defect is injected for the "IrqlExPassive" 
    // rule only for the x64 code path.
    //

    FAST_MUTEX mutex;

    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);

    ExAcquireFastMutex(&mutex);
    IoSetCancelRoutine(Irp,CancelRoutine);

    #if defined (_AMD64_)

    if (!ExIsProcessorFeaturePresent(PF_FLOATING_POINT_PRECISION_ERRATA)) {
    
        return STATUS_UNSUCCESSFUL;
        
    }

    #endif

    ExReleaseFastMutex(&mutex);
	
    return STATUS_SUCCESS;
}

NTSTATUS
DispatchInternalDeviceControl (
    IN  PDEVICE_OBJECT  DeviceObject,
    IN  PIRP            Irp
    )
{   
    //
    // This defect is injected for the "IrqlIoApcLte"
    // rule only for the x64 code path.
    //

    PVOID stackAddress = NULL;  
    #if defined (_AMD64_)

    KIRQL oldIrql;  

    #endif

    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);

    #if defined (_AMD64_)

    KeRaiseIrql(DISPATCH_LEVEL, &oldIrql);

    #endif

    stackAddress = IoGetInitialStack();
	
    return STATUS_SUCCESS;
}

VOID
CancelRoutine(
IN PDEVICE_OBJECT  DeviceObject,
IN PIRP  Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    //
    // This defect is injected for the CancelSpinLock rule. 
    // IoReleaseCancelSpinLock is not called within CancelROutine.
    //
    Irp->IoStatus.Status = STATUS_CANCELLED;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest (Irp, IO_NO_INCREMENT);
}

VOID
DriverUnload(
    IN PDRIVER_OBJECT DriverObject
    )
{
    UNREFERENCED_PARAMETER(DriverObject);

    return;
}

