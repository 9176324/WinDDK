/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    fail_library5.c

Abstract:

    Template library that can be used together with a driver for testing the 
    seamless build feature of SDV via defect injection. This library is not 
    functional and not intended as a sample for real software development 
    projects. It only provides a skeleton for building a defective library.

Environment:

    Kernel mode

--*/

#include "fail_library5.h"


VOID
SDVTest_wdf_BufferAfterReqCompletionIntIoctl(
    IN  PVOID   Buffer,
    IN  size_t  BufferLength
    )
{
    //
    // Zero out the buffer that was obtained from the Request object. This 
    // Request was completed before calling RtlZeroMemory.
    //
    RtlZeroMemory(Buffer, BufferLength);
}

VOID
SDVTest_wdf_MemoryAfterReqCompletionWrite(
    IN  WDFMEMORY   Memory
    )
{
    PVOID   pBuffer;

    //
    // Retrieve the the buffer from the memory object. The memory was 
    // retrieved from the Request object. The Request was completed 
    // before the call to WdfMemoryGetBuffer DDI
    //
    pBuffer = WdfMemoryGetBuffer(Memory, NULL);
}

VOID
SDVTest_wdf_NonFDONotPowerPolicyOwnerAPI(
    IN  WDFDEVICE   Device
    )
{
    NTSTATUS                                status;
    WDF_DEVICE_POWER_POLICY_IDLE_SETTINGS   powerPolicyIdleSettings;

    WDF_DEVICE_POWER_POLICY_IDLE_SETTINGS_INIT(&powerPolicyIdleSettings, IdleCanWakeFromS0);

    //
    // This driver is a filter driver. It is not the default Power Policy 
    // Owner. Due to which it is not allowed to call WdfDeviceAssignS0IdleSettings 
    // DDI
    status = WdfDeviceAssignS0IdleSettings(Device, &powerPolicyIdleSettings);
}

VOID
SDVTest_wdf_DeferredRequestCompletedNoAlias(
    IN  WDFREQUEST  Request
    )
{
    UNREFERENCED_PARAMETER(Request);

    //
    // Do not complete the deferred request inside this routine. This will 
    // induce an error when the unload routine is called
    //
}

PWDFDEVICE_INIT	
SDVTest_wdf_Cleanup4ControlDeviceRegistered(
    IN  WDFDRIVER   Driver
    )
{
    PWDFDEVICE_INIT         pControlDevice  = NULL;
    UNICODE_STRING          securityDesc;

    RtlInitUnicodeString(&securityDesc, L"D:P");

    //
    // Do not register EvtCleanupCallback, EvtDestroyCallback or EvtDeviceSelfManagedIoCleanup
    // for Control device object. The call to WdfControlDeviceInitAllocate will be detected as 
    // a defect
    //
    pControlDevice = WdfControlDeviceInitAllocate(Driver, &securityDesc);

    return pControlDevice;
}

VOID
SDVTest_wdf_ReqIsCancOnCancReq(
    IN  WDFREQUEST  Request
    )
{
    BOOLEAN     isCanceled;
    
    //
    // Check it the request is canceled. The Request has previously been 
    // marked for cancellation
    //
    isCanceled = WdfRequestIsCanceled(
                    Request
                    );
}

VOID
SDVTest_InitFreeDeviceCreateType2(
    IN  PWDFDEVICE_INIT     pControlDeviceInit
    )
{
    //
    // Free the device initialization structure.
    //
    WdfDeviceInitFree(pControlDeviceInit);
}
