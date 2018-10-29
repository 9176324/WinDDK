/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    fail_library1.h

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
SDVTest_wdf_DriverCreate();

VOID
SDVTest_wdf_DeviceInitAPI(
    IN PWDFDEVICE_INIT DeviceInit
    );

VOID
SDVTest_wdf_MdlAfterReqCompletionIoctl(
    IN  WDFREQUEST  Request,
    IN  PMDL        Mdl
    );

VOID
SDVTest_wdf_MemoryAfterReqCompletionIntIoctlAdd(
    IN  WDFMEMORY   Memory
    );

VOID
SDVTest_wdf_MdlAfterReqCompletionIntIoctlAdd(
    IN  PMDL        Mdl
    );

VOID
SDVTest_wdf_MarkCancOnCancReqDispatch(
    IN  WDFREQUEST  Request
    );

VOID
SDVTest_EvtRequestCancel(
    IN WDFREQUEST  Request
    );

