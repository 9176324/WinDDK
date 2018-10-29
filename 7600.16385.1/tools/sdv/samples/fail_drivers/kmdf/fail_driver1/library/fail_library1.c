/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    fail_library1.c

Abstract:

    Template library that can be used together with a driver for testing the 
    seamless build feature of SDV via defect injection. This library is not 
    functional and not intended as a sample for real software development 
    projects. It only provides a skeleton for building a defective library.

Environment:

    Kernel mode

--*/
#include "fail_library1.h"

VOID
SDVTest_wdf_DriverCreate()
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
SDVTest_wdf_MdlAfterReqCompletionIoctl(
    IN  WDFREQUEST  Request,
    IN  PMDL        Mdl
    )
{
    NTSTATUS    status;

    status = WdfRequestRetrieveInputWdmMdl(Request, &Mdl);

    return;
}

VOID
SDVTest_wdf_MemoryAfterReqCompletionIntIoctlAdd(
    IN  WDFMEMORY   Memory
    )
{
    WDF_MEMORY_DESCRIPTOR   memoryDescriptor;

    WDF_MEMORY_DESCRIPTOR_INIT_HANDLE(&memoryDescriptor, Memory, NULL);

    return;
}

VOID
SDVTest_wdf_MdlAfterReqCompletionIntIoctlAdd(
    IN  PMDL        Mdl
    )
{
    ULONG   byteOffset;
    
    byteOffset = MmGetMdlByteOffset(Mdl);

    return;
}

VOID
SDVTest_wdf_MarkCancOnCancReqDispatch(
    IN  WDFREQUEST  Request
    )
{
    WdfRequestMarkCancelable(Request, SDVTest_EvtRequestCancel);

    return;
}

VOID
SDVTest_EvtRequestCancel(
    IN WDFREQUEST  Request
    )
{
    WdfRequestComplete(Request, STATUS_CANCELLED);
}
