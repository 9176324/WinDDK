/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    fail_driver4.c

Abstract:

    This is a sample driver that contains intentionally placed code defects 
    in order to illustrate how Static Driver Verifier works. This driver is
    not functional and not intended as a sample for real driver development
    projects.

Environment:

    Kernel mode

--*/

#include "fail_driver4.h"

NTSTATUS 
DriverEntry( 
    IN PDRIVER_OBJECT   DriverObject, 
    IN PUNICODE_STRING  RegistryPath
    )
{
    NTSTATUS            status          = STATUS_SUCCESS;
    WDF_DRIVER_CONFIG   driverConfig;
    PWDFDEVICE_INIT     pControlDevice  = NULL;
    UNICODE_STRING      securityDesc;
    WDFDRIVER           driver;
    WDFDEVICE           device          = NULL;
    WDF_IO_QUEUE_CONFIG ioQueueConfig;

    RtlInitUnicodeString(&securityDesc, L"D:P");

    WDF_DRIVER_CONFIG_INIT(&driverConfig, NULL);

    driverConfig.DriverInitFlags = WdfDriverInitNonPnpDriver;

    status = WdfDriverCreate(DriverObject, RegistryPath, WDF_NO_OBJECT_ATTRIBUTES, &driverConfig, &driver);

    pControlDevice = WdfControlDeviceInitAllocate(driver, &securityDesc);

    status = WdfDeviceCreate(&pControlDevice, WDF_NO_OBJECT_ATTRIBUTES, &device);

    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&ioQueueConfig, WdfIoQueueDispatchSequential);

    ioQueueConfig.EvtIoRead             = EvtIoRead;
    ioQueueConfig.EvtIoWrite            = EvtIoWrite;
    ioQueueConfig.EvtIoDeviceControl    = EvtIoDeviceControl;

    WdfIoQueueCreate(device, &ioQueueConfig, WDF_NO_OBJECT_ATTRIBUTES, WDF_NO_HANDLE);

    /*
      This defect is injected for the "NonPnpDrvPowerPolicyOwnerAPI" rule.
    */
    SDVTest_wdf_NonPnpDriverPowerPolicyOwnerAPI(device);

    return status;
}

VOID
EvtIoRead(
    IN WDFQUEUE     Queue,
    IN WDFREQUEST   Request,
    IN size_t       Length
    )
{

    UNREFERENCED_PARAMETER(Length);
    UNREFERENCED_PARAMETER(Queue);

    /*
      This defect is injected for the "FileObjectConfigured" rule.
    */
    SDVTest_wdf_FileObjectConfigured(Request);

    /*
      This defect is injected for the "InputBufferAPI" rule.
    */
    SDVTest_wdf_InputBufferAPI(Request);
}

VOID
EvtIoWrite(
    IN WDFQUEUE     Queue,
    IN WDFREQUEST   Request,
    IN size_t       Length
    )
{

    UNREFERENCED_PARAMETER(Length);
    UNREFERENCED_PARAMETER(Queue);

    /*
      This defect is injected for the "OutputBufferAPI" rule.
    */
    SDVTest_wdf_OutputBufferAPI(Request);

    /*
      This defect is injected for the "ReqNotCanceledLocal" rule.
    */
    SDVTest_ReqNotCanceledLocal(Request, EvtRequestCancel);

    WdfRequestComplete(Request, STATUS_SUCCESS);
}

VOID
EvtRequestCancel(
    IN WDFREQUEST  Request
    )
{
    WdfRequestComplete(Request, STATUS_CANCELLED);
}

VOID
EvtIoDeviceControl(
    IN WDFQUEUE     Queue,
    IN WDFREQUEST   Request,
    IN size_t       OutputBufferLength,
    IN size_t       InputBufferLength,
    IN ULONG        IoControlCode
    )
{

    UNREFERENCED_PARAMETER(Queue);
    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(InputBufferLength);
    UNREFERENCED_PARAMETER(IoControlCode);

    SDVTest_wdf_ReqIsNotCancelable(Request);
}
