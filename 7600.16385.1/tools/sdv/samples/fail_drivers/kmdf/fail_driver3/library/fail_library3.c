/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    fail_library3.c

Abstract:

    Template library that can be used together with a driver for testing the 
    seamless build feature of SDV via defect injection. This library is not 
    functional and not intended as a sample for real software development 
    projects. It only provides a skeleton for building a defective library.

Environment:

    Kernel mode

--*/
#include "fail_library3.h"

VOID
SDVTest_wdf_FDOPowerPolicyOwnerAPI(
    PWDFDEVICE_INIT  DeviceInit
    )
{
    WDF_POWER_POLICY_EVENT_CALLBACKS    powerPolicyEventCallbacks;

    WdfDeviceInitSetPowerPolicyOwnership(DeviceInit, FALSE);

    WDF_POWER_POLICY_EVENT_CALLBACKS_INIT(&powerPolicyEventCallbacks);

    WdfDeviceInitSetPowerPolicyEventCallbacks(DeviceInit, &powerPolicyEventCallbacks);

    return;
}

VOID
SDVTest_wdf_RequestCompleted(
    WDFREQUEST  Request
    )
{
    WDF_REQUEST_PARAMETERS  requestParameters;

    WDF_REQUEST_PARAMETERS_INIT(&requestParameters);

    WdfRequestGetParameters(Request, &requestParameters);

    if(requestParameters.Type == WdfRequestTypeRead){
        return;
    }
    else{
        WdfRequestComplete(Request, STATUS_SUCCESS);
    }
}

VOID
SDVTest_wdf_ReqNotCanceled(
    WDFREQUEST  Request
    )
{
    WdfRequestComplete(Request, STATUS_SUCCESS);
}

VOID
SDVTest_wdf_NotPowerPolicyOwner()
{
    //
    // As this a function driver, it is the default Power Policy Owner
    //
}

VOID
SDVTest_wdf_MarkCancOnCancReq(
    WDFREQUEST  Request
    )
{
    WdfRequestMarkCancelable(Request, SDVTest_EvtRequestCancel);
}

VOID
SDVTest_EvtRequestCancel(
    IN WDFREQUEST  Request
    )
{
    WdfRequestComplete(Request, STATUS_CANCELLED);
}
