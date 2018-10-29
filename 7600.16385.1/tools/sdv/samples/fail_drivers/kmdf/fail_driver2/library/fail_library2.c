/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    fail_library2.c

Abstract:

    Template library that can be used together with a driver for testing the 
    seamless build feature of SDV via defect injection. This library is not 
    functional and not intended as a sample for real software development 
    projects. It only provides a skeleton for building a defective library.

Environment:

    Kernel mode

--*/
#include "fail_library2.h"

VOID
SDVTest_wdf_ControlFinishInitializingInDriverEntry()
{
    return;
}

VOID
SDVTest_wdf_DeviceInitAPI(
    IN PWDFDEVICE_INIT DeviceInit
    )
{
    WdfDeviceInitSetIoType(DeviceInit, WdfDeviceIoBuffered);
    return;
}

VOID
SDVTest_wdf_ControlDeviceDeleted()
{
    return;
}

VOID
SDVTest_wdf_FDODriver(
    IN  PWDFDEVICE_INIT     DeviceInit
    )
{
    WdfFdoInitSetFilter(DeviceInit);
    return;
}

VOID
SDVTest_wdf_DoubleCompletion(
    IN WDFREQUEST Request
    )
{
    WdfRequestComplete(Request, STATUS_SUCCESS);
    return;
}

VOID
SDVTest_InitFreeDeviceCreateType2(
    IN  PWDFDEVICE_INIT     pControlDeviceInit
    )
{
    WdfDeviceInitFree(pControlDeviceInit);
}
