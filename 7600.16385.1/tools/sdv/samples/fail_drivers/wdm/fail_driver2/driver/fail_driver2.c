/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    fail_driver2.c

Abstract:

    This is a sample driver that contains intentionally placed
    code defects in order to illustrate how Static Driver Verifier
    works. All of the injected defects can be found only using the
    seamless build feature, in other words, by processing the library
    before verifying this driver. This driver is not functional and
    not intended as a sample for real driver development projects.

Environment:

    Kernel mode

--*/

#include "fail_driver2.h"

#define _DRIVER_NAME_ "Driver"

#ifndef __cplusplus
#pragma alloc_text (INIT, DriverEntry)
#pragma alloc_text (PAGE, DriverAddDevice)
#pragma alloc_text (PAGE, DispatchCreate)
#pragma alloc_text (PAGE, DispatchRead)
#pragma alloc_text (PAGE, DispatchPower)
#pragma alloc_text (PAGE, DispatchSystemControl)
#pragma alloc_text (PAGE, DriverUnload)
#endif

NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT  DriverObject,
    IN PUNICODE_STRING RegistryPath
    )
{
    UNREFERENCED_PARAMETER(RegistryPath);
    DriverObject->MajorFunction[IRP_MJ_CREATE]         = DispatchCreate;
    DriverObject->MajorFunction[IRP_MJ_READ]           = DispatchRead;
    DriverObject->MajorFunction[IRP_MJ_WRITE]          = DispatchWrite;
    DriverObject->MajorFunction[IRP_MJ_POWER]          = DispatchPower;
    DriverObject->MajorFunction[IRP_MJ_SYSTEM_CONTROL] = DispatchSystemControl;
    DriverObject->DriverExtension->AddDevice           = DriverAddDevice;
    DriverObject->DriverUnload                         = DriverUnload;

    return STATUS_SUCCESS;
}

NTSTATUS
DriverAddDevice (
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT PhysicalDeviceObject
    )
{

    PDEVICE_OBJECT device;
    PDRIVER_DEVICE_EXTENSION extension ;
    NTSTATUS status;
    PVOID Context = NULL;
    
    UNREFERENCED_PARAMETER(DriverObject);
    UNREFERENCED_PARAMETER(PhysicalDeviceObject);
       

    status = IoCreateDevice(DriverObject,                 
                            sizeof(DRIVER_DEVICE_EXTENSION), 
                            NULL,                   
                            FILE_DEVICE_DISK,  
                            0,                     
                            FALSE,                 
                            &device                
                            );
  
   extension = (PDRIVER_DEVICE_EXTENSION)(device->DeviceExtension);

   KeInitializeDpc(&extension->dpc1,CustomDpcRoutine, Context);

   return status;
}

NTSTATUS
DispatchCreate (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    )
{
    PVOID *badPointer = NULL;

    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);

    LibraryRoutine2(badPointer);    
    
    return STATUS_SUCCESS;
}

NTSTATUS
DispatchRead (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    )
{
    KSPIN_LOCK queueLock;

    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);

    /*
       This defect is injected for the "SpinLock" rule.
    */
    LibraryRoutine3(&queueLock);
    
    return STATUS_SUCCESS;
}

NTSTATUS
DispatchWrite (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    )
{

    
    KAFFINITY ProcessorMask;
    PDRIVER_DEVICE_EXTENSION extension ;
    
    
    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);
    
    extension = (PDRIVER_DEVICE_EXTENSION)(DeviceObject->DeviceExtension);

    ProcessorMask   =  (KAFFINITY)1;
    
    IoConnectInterrupt( &extension->InterruptObject,
                         InterruptServiceRoutine,
                         extension,
                         NULL,
                         extension->ControllerVector,
                         PASSIVE_LEVEL,
                         PASSIVE_LEVEL,
                         LevelSensitive,
                         TRUE,
                         ProcessorMask,
                         TRUE );


    return STATUS_SUCCESS;
}

NTSTATUS
DispatchPower (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    )
{
    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);

    /*
       This defect is injected for the "CancelSpinLock" rule.
    */
    LibraryRoutine5();
    
    return STATUS_SUCCESS;

}

NTSTATUS
DispatchSystemControl (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    )
{   
    /*
       This defect is injected for "LowerDriverReturn" rule.
    */
    NTSTATUS status = LibraryRoutine6(DeviceObject, Irp);

    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);
    
    status = STATUS_SUCCESS;
    
    return status;
}

BOOLEAN
InterruptServiceRoutine (
    IN PKINTERRUPT Interrupt,
    IN PVOID DeviceExtensionIn
    )
{     
    PDRIVER_DEVICE_EXTENSION DeviceExtension = (PDRIVER_DEVICE_EXTENSION)DeviceExtensionIn;
    PVOID Context2 = NULL;        
     
    UNREFERENCED_PARAMETER(Interrupt); 
    KeInsertQueueDpc(&DeviceExtension->dpc1, DeviceExtension, Context2);
    
    return TRUE;
}

VOID
CustomDpcRoutine(
IN struct _KDPC  *Dpc,    
IN PVOID  DeferredContext,    
IN PVOID  SystemArgument1,    
IN PVOID  SystemArgument2    
)
{  
    UNREFERENCED_PARAMETER(SystemArgument1);
    UNREFERENCED_PARAMETER(SystemArgument2);
    UNREFERENCED_PARAMETER(Dpc);
    UNREFERENCED_PARAMETER(DeferredContext);
     /*
       This defect is injected for "PagedCode" rule.
    */
    PAGED_CODE();   
}



VOID
DriverUnload (
    IN PDRIVER_OBJECT DriverObject
    )
{

    UNREFERENCED_PARAMETER(DriverObject);

    return;
}

