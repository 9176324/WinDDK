/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    fail_driver3.c

Abstract:

    This is a sample driver that contains intentionally placed code defects 
    in order to illustrate how Static Driver Verifier works. This driver is
    not functional and not intended as a sample for real driver development
    projects.

Environment:

    Kernel mode

--*/

#include "fail_driver3.h"

WDFREQUEST  g_Request;

NTSTATUS 
DriverEntry( 
    IN PDRIVER_OBJECT   DriverObject, 
    IN PUNICODE_STRING  RegistryPath
    )
{
    WDFDRIVER               driver                      = NULL;
    NTSTATUS                status                      = STATUS_SUCCESS;
    WDF_DRIVER_CONFIG       driverConfig;
    
    WDF_DRIVER_CONFIG_INIT(&driverConfig, EvtDriverDeviceAdd);

    driverConfig.EvtDriverUnload = EvtDriverUnload;

    status = WdfDriverCreate(DriverObject, RegistryPath, WDF_NO_OBJECT_ATTRIBUTES, &driverConfig, &driver);

    return status;    
}

NTSTATUS
EvtDriverDeviceAdd(
    IN WDFDRIVER        Driver,
    IN PWDFDEVICE_INIT  DeviceInit
    )
{
    NTSTATUS            status          = STATUS_SUCCESS;
    WDFDEVICE           device          = NULL;
    WDF_IO_QUEUE_CONFIG queueConfig;

    UNREFERENCED_PARAMETER(Driver);

    /*
      This defect is injected for the "FDOPowerPolicyOwnerAPI" rule.
    */
    SDVTest_wdf_FDOPowerPolicyOwnerAPI(DeviceInit);

    status = WdfDeviceCreate(&DeviceInit, WDF_NO_OBJECT_ATTRIBUTES, &device);

    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&queueConfig, WdfIoQueueDispatchSequential);

    queueConfig.EvtIoRead           = EvtIoRead;
    queueConfig.EvtIoWrite          = EvtIoWrite;
    queueConfig.EvtIoDeviceControl  = EvtIoDeviceControl;
    queueConfig.EvtIoStop  = EvtIoStop;

    status = WdfIoQueueCreate(device, &queueConfig, WDF_NO_OBJECT_ATTRIBUTES, WDF_NO_HANDLE);

    /*
      This defect is injected for the "NotPowerPolicyOwner" rule.
    */
    SDVTest_wdf_NotPowerPolicyOwner();

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
      This defect is injected for the "RequestCompletedLocal" rule.
    */
    SDVTest_wdf_RequestCompleted(Request);
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

    WdfRequestMarkCancelable(Request, EvtRequestCancel);

    g_Request = Request;
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

    WdfRequestMarkCancelable(Request, EvtRequestCancel);

    SDVTest_wdf_MarkCancOnCancReq(Request);
}

VOID
EvtIoStop(
    IN WDFQUEUE     Queue,
    IN WDFREQUEST   Request,
    IN ULONG        ActionFlags
    )
{  
    UNREFERENCED_PARAMETER(ActionFlags);
    UNREFERENCED_PARAMETER(Queue);
    /*
      This defect is injected for the "EvtIoStopResume" rule.
    */
    WdfRequestStopAcknowledge(Request, FALSE);
}

VOID
EvtDriverUnload(
    IN WDFDRIVER  Driver
    )
{

    UNREFERENCED_PARAMETER(Driver);

    /*
      This defect is injected for the "RequestCompleted" rule.
    */
    SDVTest_wdf_RequestCompleted(g_Request);
}

VOID
EvtTimerFunc(
    IN WDFTIMER  Timer
    )
{
    UNREFERENCED_PARAMETER(Timer);

    SDVTest_wdf_ReqNotCanceled(g_Request);
}

VOID
EvtRequestCancel(
    IN WDFREQUEST  Request
    )
{
    WdfRequestComplete(Request, STATUS_SUCCESS);
}

NTSTATUS
DeviceD0Entry(
    IN WDFDEVICE  Device,
    IN WDF_POWER_DEVICE_STATE  PreviousState
    )
{
   UNREFERENCED_PARAMETER(Device);
   UNREFERENCED_PARAMETER(PreviousState);
   /* 
     This defect is injected for the "PagedCodeAtD0" rule.
   */   
   PAGED_CODE();
   return STATUS_SUCCESS;
}
