/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    bulkusb.c

Abstract:

    Bulk USB device driver for Intel 82930 USB test board
	Main module

Author:

Environment:

    kernel mode only

Notes:

    Copyright (c) 2000 Microsoft Corporation.  
    All Rights Reserved.

--*/

#include "bulkusb.h"
#include "bulkpnp.h"
#include "bulkpwr.h"
#include "bulkdev.h"
#include "bulkwmi.h"
#include "bulkusr.h"
#include "bulkrwr.h"

//
// Globals
//

GLOBALS Globals;
ULONG   DebugLevel = 1;

NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT  DriverObject,
    IN PUNICODE_STRING UniRegistryPath
    );

VOID
BulkUsb_DriverUnload(
    IN PDRIVER_OBJECT DriverObject
    );

NTSTATUS
BulkUsb_AddDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT PhysicalDeviceObject
    );

#ifdef PAGE_CODE
#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, BulkUsb_DriverUnload)
#endif
#endif

NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT  DriverObject,
    IN PUNICODE_STRING UniRegistryPath
    )
/*++ 

Routine Description:

    Installable driver initialization entry point.
    This entry point is called directly by the I/O system.    

Arguments:
    
    DriverObject - pointer to driver object 

    RegistryPath - pointer to a unicode string representing the path to driver 
                   specific key in the registry.

Return Values:

    NT status value
    
--*/
{

    NTSTATUS        ntStatus;
    PUNICODE_STRING registryPath;
    
    //
    // initialization of variables
    //

    registryPath = &Globals.BulkUsb_RegistryPath;

    //
    // Allocate pool to hold a null-terminated copy of the path.
    // Safe in paged pool since all registry routines execute at
    // PASSIVE_LEVEL.
    //

    registryPath->MaximumLength = UniRegistryPath->Length + sizeof(UNICODE_NULL);
    registryPath->Length        = UniRegistryPath->Length;
    registryPath->Buffer        = ExAllocatePool(PagedPool,
                                                 registryPath->MaximumLength);

    if (!registryPath->Buffer) {

        BulkUsb_DbgPrint(1, ("Failed to allocate memory for registryPath\n"));
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        goto DriverEntry_Exit;
    } 


    RtlZeroMemory (registryPath->Buffer, 
                   registryPath->MaximumLength);
    RtlMoveMemory (registryPath->Buffer, 
                   UniRegistryPath->Buffer, 
                   UniRegistryPath->Length);

    ntStatus = STATUS_SUCCESS;

    //
    // Initialize the driver object with this driver's entry points.
    //
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = BulkUsb_DispatchDevCtrl;
    DriverObject->MajorFunction[IRP_MJ_POWER]          = BulkUsb_DispatchPower;
    DriverObject->MajorFunction[IRP_MJ_PNP]            = BulkUsb_DispatchPnP;
    DriverObject->MajorFunction[IRP_MJ_CREATE]         = BulkUsb_DispatchCreate;
    DriverObject->MajorFunction[IRP_MJ_CLOSE]          = BulkUsb_DispatchClose;
    DriverObject->MajorFunction[IRP_MJ_CLEANUP]        = BulkUsb_DispatchClean;
    DriverObject->MajorFunction[IRP_MJ_READ]           =
    DriverObject->MajorFunction[IRP_MJ_WRITE]          = BulkUsb_DispatchReadWrite;
    DriverObject->MajorFunction[IRP_MJ_SYSTEM_CONTROL] = BulkUsb_DispatchSysCtrl;
    DriverObject->DriverUnload                         = BulkUsb_DriverUnload;
    DriverObject->DriverExtension->AddDevice           = (PDRIVER_ADD_DEVICE)
                                                         BulkUsb_AddDevice;
DriverEntry_Exit:

    return ntStatus;
}

VOID
BulkUsb_DriverUnload(
    IN PDRIVER_OBJECT DriverObject
    )
/*++

Description:

    This function will free the memory allocations in DriverEntry.

Arguments:

    DriverObject - pointer to driver object 

Return:
	
    None

--*/
{
    PUNICODE_STRING registryPath;

    BulkUsb_DbgPrint(3, ("BulkUsb_DriverUnload - begins\n"));

    registryPath = &Globals.BulkUsb_RegistryPath;

    if(registryPath->Buffer) {

        ExFreePool(registryPath->Buffer);
        registryPath->Buffer = NULL;
    }

    BulkUsb_DbgPrint(3, ("BulkUsb_DriverUnload - ends\n"));

    return;
}

NTSTATUS
BulkUsb_AddDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT PhysicalDeviceObject
    )
/*++

Description:

Arguments:

    DriverObject - Store the pointer to the object representing us.

    PhysicalDeviceObject - Pointer to the device object created by the
                           undelying bus driver.

Return:
	
    STATUS_SUCCESS - if successful 
    STATUS_UNSUCCESSFUL - otherwise

--*/
{
    NTSTATUS          ntStatus;
    PDEVICE_OBJECT    deviceObject;
    PDEVICE_EXTENSION deviceExtension;
    POWER_STATE       state;
    KIRQL             oldIrql;

    BulkUsb_DbgPrint(3, ("BulkUsb_AddDevice - begins\n"));

    deviceObject = NULL;

    ntStatus = IoCreateDevice(
                    DriverObject,                   // our driver object
                    sizeof(DEVICE_EXTENSION),       // extension size for us
                    NULL,                           // name for this device
                    FILE_DEVICE_UNKNOWN,
                    FILE_AUTOGENERATED_DEVICE_NAME, // device characteristics
                    FALSE,                          // Not exclusive
                    &deviceObject);                 // Our device object

    if(!NT_SUCCESS(ntStatus)) {
        //
        // returning failure here prevents the entire stack from functioning,
        // but most likely the rest of the stack will not be able to create
        // device objects either, so it is still OK.
        //                
        BulkUsb_DbgPrint(1, ("Failed to create device object\n"));
        return ntStatus;
    }

    //
    // Initialize the device extension
    //

    deviceExtension = (PDEVICE_EXTENSION) deviceObject->DeviceExtension;
    deviceExtension->FunctionalDeviceObject = deviceObject;
    deviceExtension->PhysicalDeviceObject = PhysicalDeviceObject;
    deviceObject->Flags |= DO_DIRECT_IO;

    //
    // initialize the device state lock and set the device state
    //

    KeInitializeSpinLock(&deviceExtension->DevStateLock);
    INITIALIZE_PNP_STATE(deviceExtension);

    //
    //initialize OpenHandleCount
    //
    deviceExtension->OpenHandleCount = 0;

    //
    // Initialize the selective suspend variables
    //
    KeInitializeSpinLock(&deviceExtension->IdleReqStateLock);
    deviceExtension->IdleReqPend = 0;
    deviceExtension->PendingIdleIrp = NULL;

    //
    // Hold requests until the device is started
    //

    deviceExtension->QueueState = HoldRequests;

    //
    // Initialize the queue and the queue spin lock
    //

    InitializeListHead(&deviceExtension->NewRequestsQueue);
    KeInitializeSpinLock(&deviceExtension->QueueLock);

    //
    // Initialize the remove event to not-signaled.
    //

    KeInitializeEvent(&deviceExtension->RemoveEvent, 
                      SynchronizationEvent, 
                      FALSE);

    //
    // Initialize the stop event to signaled.
    // This event is signaled when the OutstandingIO becomes 1
    //

    KeInitializeEvent(&deviceExtension->StopEvent, 
                      SynchronizationEvent, 
                      TRUE);

    //
    // OutstandingIo count biased to 1.
    // Transition to 0 during remove device means IO is finished.
    // Transition to 1 means the device can be stopped
    //

    deviceExtension->OutStandingIO = 1;
    KeInitializeSpinLock(&deviceExtension->IOCountLock);

    //
    // Delegating to WMILIB
    //
    ntStatus = BulkUsb_WmiRegistration(deviceExtension);

    if(!NT_SUCCESS(ntStatus)) {

        BulkUsb_DbgPrint(1, ("BulkUsb_WmiRegistration failed with %X\n", ntStatus));
        IoDeleteDevice(deviceObject);
        return ntStatus;
    }

    //
    // set the flags as underlying PDO
    //

    if(PhysicalDeviceObject->Flags & DO_POWER_PAGABLE) {

        deviceObject->Flags |= DO_POWER_PAGABLE;
    }

    //
    // Typically, the function driver for a device is its 
    // power policy owner, although for some devices another 
    // driver or system component may assume this role. 
    // Set the initial power state of the device, if known, by calling 
    // PoSetPowerState.
    // 

    deviceExtension->DevPower = PowerDeviceD0;
    deviceExtension->SysPower = PowerSystemWorking;

    state.DeviceState = PowerDeviceD0;
    PoSetPowerState(deviceObject, DevicePowerState, state);

    //
    // attach our driver to device stack
    // The return value of IoAttachDeviceToDeviceStack is the top of the
    // attachment chain.  This is where all the IRPs should be routed.
    //

    deviceExtension->TopOfStackDeviceObject = 
                IoAttachDeviceToDeviceStack(deviceObject,
                                            PhysicalDeviceObject);

    if(NULL == deviceExtension->TopOfStackDeviceObject) {

        BulkUsb_WmiDeRegistration(deviceExtension);
        IoDeleteDevice(deviceObject);
        return STATUS_NO_SUCH_DEVICE;
    }
        
    //
    // Register device interfaces
    //

    ntStatus = IoRegisterDeviceInterface(deviceExtension->PhysicalDeviceObject, 
                                         &GUID_CLASS_I82930_BULK, 
                                         NULL, 
                                         &deviceExtension->InterfaceName);

    if(!NT_SUCCESS(ntStatus)) {

        BulkUsb_WmiDeRegistration(deviceExtension);
        IoDetachDevice(deviceExtension->TopOfStackDeviceObject);
        IoDeleteDevice(deviceObject);
        return ntStatus;
    }

    if(IoIsWdmVersionAvailable(1, 0x20)) {

        deviceExtension->WdmVersion = WinXpOrBetter;
    }
    else if(IoIsWdmVersionAvailable(1, 0x10)) {

        deviceExtension->WdmVersion = Win2kOrBetter;
    }
    else if(IoIsWdmVersionAvailable(1, 0x5)) {

        deviceExtension->WdmVersion = WinMeOrBetter;
    }
    else if(IoIsWdmVersionAvailable(1, 0x0)) {

        deviceExtension->WdmVersion = Win98OrBetter;
    }

    deviceExtension->SSRegistryEnable = 0;
    deviceExtension->SSEnable = 0;

    //
    // WinXP only
    // check the registry flag -
    // whether the device should selectively
    // suspend when idle
    //

    if(WinXpOrBetter == deviceExtension->WdmVersion) {

        BulkUsb_GetRegistryDword(BULKUSB_REGISTRY_PARAMETERS_PATH,
                                 L"BulkUsbEnable",
                                 &deviceExtension->SSRegistryEnable);

        if(deviceExtension->SSRegistryEnable) {

            //
            // initialize DPC
            //
            KeInitializeDpc(&deviceExtension->DeferredProcCall, 
                            DpcRoutine, 
                            deviceObject);

            //
            // initialize the timer.
            // the DPC and the timer in conjunction, 
            // monitor the state of the device to 
            // selectively suspend the device.
            //
            KeInitializeTimerEx(&deviceExtension->Timer,
                                NotificationTimer);

            //
            // Initialize the NoDpcWorkItemPendingEvent to signaled state.
            // This event is cleared when a Dpc is fired and signaled
            // on completion of the work-item.
            //
            KeInitializeEvent(&deviceExtension->NoDpcWorkItemPendingEvent, 
                              NotificationEvent, 
                              TRUE);

            //
            // Initialize the NoIdleReqPendEvent to ensure that the idle request
            // is indeed complete before we unload the drivers.
            //
            KeInitializeEvent(&deviceExtension->NoIdleReqPendEvent,
                              NotificationEvent,
                              TRUE);
        }
    }

    //
    // Clear the DO_DEVICE_INITIALIZING flag.
    // Note: Do not clear this flag until the driver has set the
    // device power state and the power DO flags. 
    //

    deviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

    BulkUsb_DbgPrint(3, ("BulkUsb_AddDevice - ends\n"));

    return ntStatus;
}


