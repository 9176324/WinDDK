/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    fail_library2.h

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
SDVTest_wdf_ControlFinishInitializingInDriverEntry();

VOID
SDVTest_wdf_DeviceInitAPI(
    IN PWDFDEVICE_INIT DeviceInit
    );

VOID
SDVTest_wdf_ControlDeviceDeleted();

VOID
SDVTest_wdf_FDODriver(
    IN  PWDFDEVICE_INIT     DeviceInit
    );

VOID
SDVTest_wdf_DoubleCompletion(
    IN WDFREQUEST Request
    );

VOID
SDVTest_InitFreeDeviceCreateType2(
    IN  PWDFDEVICE_INIT     pControlDeviceInit
    );
