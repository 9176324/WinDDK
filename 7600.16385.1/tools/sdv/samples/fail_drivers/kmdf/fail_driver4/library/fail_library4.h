/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    fail_library4.h

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
SDVTest_wdf_FileObjectConfigured(
    IN  WDFREQUEST  Request
    );

VOID
SDVTest_wdf_InputBufferAPI(
    IN  WDFREQUEST  Request
    );

VOID
SDVTest_wdf_OutputBufferAPI(
    IN  WDFREQUEST  Request
    );

VOID
SDVTest_ReqNotCanceledLocal(
    IN  WDFREQUEST              Request, 
    IN  PFN_WDF_REQUEST_CANCEL  EvtRequestCancel
    );

VOID
SDVTest_wdf_ReqIsNotCancelable(
    IN  WDFREQUEST  Request
    );

VOID
SDVTest_wdf_NonPnpDriverPowerPolicyOwnerAPI(
    IN  WDFDEVICE   Device
    );
