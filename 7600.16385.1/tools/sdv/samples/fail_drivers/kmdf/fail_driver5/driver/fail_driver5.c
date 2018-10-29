/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    fail_driver5.c

Abstract:

    This is a sample driver that contains intentionally placed code defects 
    in order to illustrate how Static Driver Verifier works. This driver is
    not functional and not intended as a sample for real driver development
    projects.

Environment:

    Kernel mode

--*/

#include "fail_driver5.h"

WDFREQUEST     g_Request;

NTSTATUS 
DriverEntry( 
    IN PDRIVER_OBJECT   DriverObject, 
    IN PUNICODE_STRING  RegistryPath
    )
{
    NTSTATUS            status          = STATUS_SUCCESS;
    WDF_DRIVER_CONFIG   driverConfig;
    WDFDRIVER Driver;
    PWDFDEVICE_INIT  ControlDeviceInit;
    WDFDEVICE device;

    WDF_DRIVER_CONFIG_INIT(&driverConfig, EvtDriverDeviceAdd);
	
    status = WdfDriverCreate(DriverObject, RegistryPath, WDF_NO_OBJECT_ATTRIBUTES, &driverConfig, &Driver);
    
    /*
      This defect is injected for the "Cleanup4CtlDeviceRegistered" rule.
    */
    ControlDeviceInit = SDVTest_wdf_Cleanup4ControlDeviceRegistered(Driver);

    /*
      This defect is injected for the "InitFreeDeviceCreateType2" rule.
    */
    SDVTest_InitFreeDeviceCreateType2(ControlDeviceInit);

    status = WdfDeviceCreate(&ControlDeviceInit, WDF_NO_OBJECT_ATTRIBUTES, &device);
    
    return status;
}

NTSTATUS
EvtDriverDeviceAdd(
    IN WDFDRIVER        Driver,
    IN PWDFDEVICE_INIT  DeviceInit
    )
{
    NTSTATUS                status          = STATUS_SUCCESS;
    WDFDEVICE               device          = NULL;
    WDF_IO_QUEUE_CONFIG     ioQueueConfig;
    
    UNREFERENCED_PARAMETER(Driver);
	
    WdfFdoInitSetFilter(DeviceInit);

    status = WdfDeviceCreate(&DeviceInit, WDF_NO_OBJECT_ATTRIBUTES, &device);

    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&ioQueueConfig, WdfIoQueueDispatchSequential);

    ioQueueConfig.EvtIoRead                     = EvtIoRead;
    ioQueueConfig.EvtIoWrite                    = EvtIoWrite;
    ioQueueConfig.EvtIoDeviceControl            = EvtIoDeviceControl;
    ioQueueConfig.EvtIoInternalDeviceControl    = EvtIoInternalDeviceControl;
    ioQueueConfig.EvtIoStop                    	= EvtIoStop;

    WdfIoQueueCreate(device, &ioQueueConfig, WDF_NO_OBJECT_ATTRIBUTES, WDF_NO_HANDLE);

    /*
      This defect is injected for the "NonFDONotPowerPolicyOwnerAPI" rule.
    */
    SDVTest_wdf_NonFDONotPowerPolicyOwnerAPI(device);

    return status;
}

VOID
EvtIoInternalDeviceControl(
    IN WDFQUEUE     Queue,
    IN WDFREQUEST   Request,
    IN size_t       OutputBufferLength,
    IN size_t       InputBufferLength,
    IN ULONG        IoControlCode
    )
{
    NTSTATUS    status  = STATUS_SUCCESS;
    PVOID       pBuffer;

    UNREFERENCED_PARAMETER(Queue);
    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(IoControlCode);

    status = WdfRequestRetrieveInputBuffer(Request, InputBufferLength, &pBuffer, NULL);

    WdfRequestCompleteWithInformation(Request, status, 0L);

    /*
      This defect is injected for the "BufAfterReqCompletedIntIoctlA" rule.
    */
    SDVTest_wdf_BufferAfterReqCompletionIntIoctl(pBuffer, InputBufferLength);
}

VOID
EvtIoWrite(
    IN WDFQUEUE     Queue,
    IN WDFREQUEST   Request,
    IN size_t       Length
    )
{
    NTSTATUS    status  = STATUS_SUCCESS;
    WDFMEMORY   memory;

    UNREFERENCED_PARAMETER(Queue);
    UNREFERENCED_PARAMETER(Length);

    status = WdfRequestRetrieveInputMemory(Request, &memory);

    WdfRequestComplete(Request, status);

    /*
      This defect is injected for the "MemAfterReqCompletedWriteA" rule.
    */
    SDVTest_wdf_MemoryAfterReqCompletionWrite(memory);
}

VOID
EvtIoRead(
    IN WDFQUEUE     Queue,
    IN WDFREQUEST   Request,
    IN size_t       Length
    )
{
    UNREFERENCED_PARAMETER(Queue);
    UNREFERENCED_PARAMETER(Length);

    WdfRequestMarkCancelable(Request, EvtRequestCancel);
    
    g_Request = Request;
}

VOID
EvtIoStop(
    IN WDFQUEUE     Queue,
    IN WDFREQUEST   Request,
    IN ULONG        ActionFlags
    )
{
    UNREFERENCED_PARAMETER(Queue);
    UNREFERENCED_PARAMETER(ActionFlags);
    /*
      This defect is injected for the "EvtIoStopCancel" rule.
    */
    WdfRequestStopAcknowledge(Request, TRUE);
    
}

VOID
EvtIoDeviceControl(
    IN WDFQUEUE     Queue,
    IN WDFREQUEST   Request,
    IN size_t       OutputBufferLength,
    IN size_t       InputBufferLength,
    IN ULONG        IoControlCode
    )
{
    UNREFERENCED_PARAMETER(Queue);
    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(InputBufferLength);
    UNREFERENCED_PARAMETER(IoControlCode);

    WdfRequestMarkCancelable(Request, EvtRequestCancel);

    /*
      This defect is injected for the "ReqIsCancOnCancReq" rule.
    */
    SDVTest_wdf_ReqIsCancOnCancReq(Request);
}

VOID
EvtRequestCancel(
    IN WDFREQUEST  Request
    )
{
    WdfRequestComplete(Request, STATUS_CANCELLED);
}

VOID
EvtDpcFunc(
    IN WDFDPC  Dpc
    )
{
    UNREFERENCED_PARAMETER(Dpc);    
    SDVTest_wdf_DeferredRequestCompletedNoAlias(g_Request);
}

VOID
EvtDriverUnload(
    IN WDFDRIVER  Driver
    )
{
    UNREFERENCED_PARAMETER(Driver);
}
