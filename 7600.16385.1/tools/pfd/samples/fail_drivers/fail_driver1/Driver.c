/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    Driver.c

Abstract:

    This is a sample driver that contains intentionally placed
    code defects in order to illustrate how PREfast for Drivers (PFD)
    works. This driver is not functional and not intended as a 
    sample for real driver development projects. 

Environment:

    Kernel mode

--*/

#include "Driver.h"

#define _DRIVER_NAME_ "Driver"

#ifdef __cplusplus
extern " C "
#endif
ULONG _fltused=1;

//
// The following lines declare DriverEntry as an INIT segment, and the others as residing in Pageable memory.
// PFD will issue warnings for any functions that do not use the corresponding PAGED_CODE(); for functions in pageable memory.
// DriverDispatchControl is not declared here, to display the effects of using the PAGED_CODE(); macro without first declaring the function in a pageable segment.
//
#ifdef ALLOC_PRAGMA
#pragma alloc_text (INIT, DriverEntry)
#pragma alloc_text (PAGE, DriverAddDevice)
#pragma alloc_text (PAGE, DispatchPnp)
#pragma alloc_text (PAGE, DispatchPower)
#pragma alloc_text (PAGE, DriverUnload)
#endif



NTSTATUS
DriverEntry(
    __in PDRIVER_OBJECT  DriverObject,
    __in PUNICODE_STRING RegistryPath
    )
{
    //
    // PFD Warning 28101: Informational message inferring that this is the DriverEntry function
    //

    DriverObject->MajorFunction[IRP_MJ_PNP]            = DispatchPnp;           // PFD Warning 28155 and 28168 (See declaration in driver.h)
    DriverObject->MajorFunction[IRP_MJ_POWER]          = DispatchPower;         // PFD Warning 28168 (See declaration in driver.h)
    DriverObject->MajorFunction[IRP_MJ_SYSTEM_CONTROL] = DispatchSystemControl;
    DriverObject->DriverExtension->AddDevice           = DriverAddDevice;
    DriverObject->DriverUnload                         = DriverUnload;   

    return STATUS_SUCCESS;
}

NTSTATUS
DriverAddDevice(
    __in PDRIVER_OBJECT DriverObject,
    __in PDEVICE_OBJECT PhysicalDeviceObject
    )
{
    //
    // This function will register warning 28152:
    //  AddDevice did not clear DO_DEVICE_INITIALIZING bit
    //  See declaration in Driver.h
    //

    PAGED_CODE();
     

    return STATUS_SUCCESS;
}

NTSTATUS
DispatchPnp (
    __in PDEVICE_OBJECT DeviceObject,
    __in PIRP           Irp
    )
{  
    //
    // Missing PAGED_CODE(); macro results in:
    //   Warning 28170: Function declared in Paged Segment, but PAGED_CODE(); not found.  
    // A memory leaking operation that causes PFD to register two of them:
    //   Warning 6014: Leaking Memory 'buffer'
    //   Warning 28193: 'buffer' holds a value that must be examined.
    //

    char *buffer = ExAllocatePoolWithTag(NonPagedPool, sizeof(char), 'Fail');
    
    
    return STATUS_SUCCESS;
}

NTSTATUS
DispatchPower (
    __in PDEVICE_OBJECT DeviceObject,
    __in PIRP           Irp
    )
{
    double a, b, c;

    PAGED_CODE();

    //
    // A floating point operation that causes PFD to register the following warning:
    // Warning 28110: Drivers should always protect the floating point state.
    //

    a = 2.85;
    b = 7.87;
    c = (b/a);

    return STATUS_SUCCESS;
}

NTSTATUS
DispatchSystemControl (
    __in PDEVICE_OBJECT DeviceObject,
    __in PIRP           Irp
    )
{   
    //
    // This function will register PFD Warning 28172:
    //  DispatchSystemControl has a PAGED_CODE(); macro, but has not been declared to be in a paged segment.
    // 

    PAGED_CODE();
    return STATUS_SUCCESS;
}

VOID
DriverUnload(
    __in PDRIVER_OBJECT DriverObject
    )
{
    //
    // This function should not produce any PFD Warnings as written in fail_driver1
    //
   
    PAGED_CODE();
    return;
}