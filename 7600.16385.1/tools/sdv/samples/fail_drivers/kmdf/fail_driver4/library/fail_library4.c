/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    fail_library4.c

Abstract:

    Template library that can be used together with a driver for testing the 
    seamless build feature of SDV via defect injection. This library is not 
    functional and not intended as a sample for real software development 
    projects. It only provides a skeleton for building a defective library.

Environment:

    Kernel mode

--*/

#include "fail_library4.h"

VOID
SDVTest_wdf_FileObjectConfigured(
    IN  WDFREQUEST  Request
    )
{
    WDFFILEOBJECT   fileObject;

    //
    // Call WdfRequestGetFileObject API without a previous call to WdfDeviceInitSetFileObjectConfig
    //
    fileObject = WdfRequestGetFileObject(Request);
}

VOID
SDVTest_wdf_InputBufferAPI(
    IN  WDFREQUEST  Request
    )
{
    NTSTATUS    status;
    WDFMEMORY   memory;

    //
    // Call WdfRequestRetrieveInputMemory API for a Read request
    //
    status = WdfRequestRetrieveInputMemory(Request, &memory);

    WdfRequestComplete(Request, status);
}

VOID
SDVTest_wdf_OutputBufferAPI(
    IN  WDFREQUEST  Request
    )
{
    NTSTATUS    status;
    WDFMEMORY   memory;

    //
    // Call WdfRequestRetrieveOutputMemory API for a Write request
    //
    status = WdfRequestRetrieveOutputMemory(Request, &memory);

    WdfRequestComplete(Request, status);
}

VOID
SDVTest_ReqNotCanceledLocal(
    IN  WDFREQUEST              Request, 
    IN  PFN_WDF_REQUEST_CANCEL  EvtRequestCancel
    )
{
    //
    // Call WdfRequestMarkCancelable API for a request that is completed
    // in the same Dispatch routine
    //
    WdfRequestMarkCancelable(Request, EvtRequestCancel);
}

VOID
SDVTest_wdf_ReqIsNotCancelable(
    IN  WDFREQUEST  Request
    )
{
    NTSTATUS    status = STATUS_SUCCESS;

    //
    // Unmark the request for cancellation. This request is not previously marked for 
    // cancellation nor is previously completed
    //
    status = WdfRequestUnmarkCancelable(Request);

    WdfRequestComplete(Request, status);
}

VOID
SDVTest_wdf_NonPnpDriverPowerPolicyOwnerAPI(
    IN  WDFDEVICE   Device
    )
{
    NTSTATUS                                status;
    WDF_DEVICE_POWER_POLICY_IDLE_SETTINGS    powerPolicyWakeSettings;

    //WDF_DEVICE_POWER_POLICY_IDLE_SETTINGS_INIT(&powerPolicyWakeSettings);

    //
    // This driver is a filter driver. It is not the default Power Policy 
    // Owner. Due to which it is not allowed to call WdfDeviceAssignSxWakeSettings
    // DDI
    status = WdfDeviceAssignS0IdleSettings(Device, &powerPolicyWakeSettings);
}
