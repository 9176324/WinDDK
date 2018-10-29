/*++

Copyright (c) 1990-2000  Microsoft Corporation

Module Name:

    driver.c

Abstract:

    This driver demonstrates use of a default I/O Queue, its
    request start events, cancellation event, and a synchronized DPC.

    To demonstrate asynchronous operation, the I/O requests are not completed
    immediately, but stored in the drivers private data structure, and a timer
    DPC will complete it next time the DPC runs.

    During the time the request is waiting for the DPC to run, it is
    made cancellable by the call WdfRequestMarkCancelable. This
    allows the test program to cancel the cancel the request and exit instantly.

    This rather complicated set of events is designed to demonstrate
    the driver frameworks synchronization of access to a device driver
    data structure, and a pointer which can be a proxy for device hardware
    registers or resources.

    This common data structure, or resource is accessed by new request
    events arriving, the DPC that completes it, and cancel processing.

    Notice the lack of specific lock/unlock operations.

    Even though this example utilizes a serial queue, a parallel queue
    would not need any additional explicit synchronization, just a
    strategy for managing multiple requests outstanding.

--*/

#include "driver.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (INIT, DriverEntry)
#pragma alloc_text (PAGE, PortIOEvtDeviceAdd)
#endif


NTSTATUS
DriverEntry(
    __in PDRIVER_OBJECT  DriverObject,
    __in PUNICODE_STRING RegistryPath
    )
/*++

Routine Description:
    DriverEntry initializes the driver and is the first routine called by the
    system after the driver is loaded. DriverEntry specifies the other entry
    points in the function driver, such as EvtDevice and DriverUnload.

Parameters Description:

    DriverObject - represents the instance of the function driver that is loaded
    into memory. DriverEntry must initialize members of DriverObject before it
    returns to the caller. DriverObject is allocated by the system before the
    driver is loaded, and it is released by the system after the system unloads
    the function driver from memory.

    RegistryPath - represents the driver specific path in the Registry.
    The function driver can use the path to store driver related data between
    reboots. The path does not store hardware instance specific data.

Return Value:

    STATUS_SUCCESS if successful,
    STATUS_UNSUCCESSFUL otherwise.

--*/
{
    WDF_DRIVER_CONFIG config;
    NTSTATUS status;

    WDF_DRIVER_CONFIG_INIT(&config,
                        PortIOEvtDeviceAdd
                        );

    status = WdfDriverCreate(DriverObject,
                            RegistryPath,
                            WDF_NO_OBJECT_ATTRIBUTES,
                            &config,
                            WDF_NO_HANDLE);
    if (!NT_SUCCESS(status)) {
        KdPrint(("Error: WdfDriverCreate failed 0x%x\n", status));
        return status;
    }

    return status;
}

NTSTATUS
PortIOEvtDeviceAdd(
    __in WDFDRIVER       Driver,
    __inout PWDFDEVICE_INIT DeviceInit
    )
/*++
Routine Description:

    EvtDeviceAdd is called by the framework in response to AddDevice
    call from the PnP manager. We create and initialize a device object to
    represent a new instance of the device.

Arguments:

    Driver - Handle to a framework driver object created in DriverEntry

    DeviceInit - Pointer to a framework-allocated WDFDEVICE_INIT structure.

Return Value:

    NTSTATUS

--*/
{
    NTSTATUS status;

    UNREFERENCED_PARAMETER(Driver);

    PAGED_CODE();

    KdPrint(("Enter PortIoDeviceAdd\n"));

    status = PortIODeviceCreate(DeviceInit);

    return status;
}


