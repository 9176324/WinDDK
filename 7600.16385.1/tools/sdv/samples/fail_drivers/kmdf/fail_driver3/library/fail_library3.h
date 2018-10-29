/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    fail_library3.h

Environment:

    Kernel mode

--*/


#ifdef __cplusplus
extern "C" {
#endif
#include <NTDDK.h>  
#include <wdf.h>
#ifdef __cplusplus
}
#endif

VOID
SDVTest_wdf_FDOPowerPolicyOwnerAPI(
    PWDFDEVICE_INIT  DeviceInit
    );

VOID
SDVTest_wdf_RequestCompleted(
    WDFREQUEST  Request
    );

VOID
SDVTest_wdf_ReqNotCanceled(
    WDFREQUEST  Request
    );

VOID
SDVTest_wdf_NotPowerPolicyOwner();

VOID
SDVTest_wdf_MarkCancOnCancReq(
    WDFREQUEST  Request
    );

VOID
SDVTest_EvtRequestCancel(
    IN WDFREQUEST  Request
    );
