/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    fail_library5.h

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
SDVTest_wdf_BufferAfterReqCompletionIntIoctl(
    IN  PVOID   Buffer,
    IN  size_t  BufferLength
    );

VOID
SDVTest_wdf_MemoryAfterReqCompletionWrite(
    IN  WDFMEMORY   Memory
    );

VOID
SDVTest_wdf_NonFDONotPowerPolicyOwnerAPI(
    IN  WDFDEVICE   Device
    );

VOID
SDVTest_wdf_DeferredRequestCompletedNoAlias(
    IN  WDFREQUEST  Request
    );

PWDFDEVICE_INIT	
SDVTest_wdf_Cleanup4ControlDeviceRegistered(
    IN  WDFDRIVER   Driver
    );

VOID
SDVTest_wdf_ReqIsCancOnCancReq(
    IN  WDFREQUEST  Request
    );

VOID
SDVTest_InitFreeDeviceCreateType2(
    IN  PWDFDEVICE_INIT     pControlDeviceInit
    );

