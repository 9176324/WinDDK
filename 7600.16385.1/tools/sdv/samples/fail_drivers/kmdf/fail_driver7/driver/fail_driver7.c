/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    fail_driver7.c

Abstract:

    This is a sample driver that contains intentionally placed code defects 
    in order to illustrate how Static Driver Verifier works. This driver is
    not functional and not intended as a sample for real driver development
    projects.

Environment:

    Kernel mode

--*/

#include "fail_driver7.h"

WDFREQUEST  g_Request;

NTSTATUS 
DriverEntry( 
    IN PDRIVER_OBJECT   DriverObject, 
    IN PUNICODE_STRING  RegistryPath
    )
{
    WDFDRIVER               driver                      = NULL;
    NTSTATUS                status                      = STATUS_SUCCESS;
    WDF_DRIVER_CONFIG       driverConfig;
    
    WDF_DRIVER_CONFIG_INIT(&driverConfig, EvtDriverDeviceAdd);

    status = WdfDriverCreate(DriverObject, RegistryPath, WDF_NO_OBJECT_ATTRIBUTES, &driverConfig, &driver);

    return status;    
}

NTSTATUS
EvtDriverDeviceAdd(
    IN WDFDRIVER        Driver,
    IN PWDFDEVICE_INIT  DeviceInit
    )
{
    NTSTATUS            status          = STATUS_SUCCESS;
    WDFDEVICE           device          = NULL;
    WDF_IO_QUEUE_CONFIG queueConfig;

    UNREFERENCED_PARAMETER(Driver);

    status = WdfDeviceCreate(&DeviceInit, WDF_NO_OBJECT_ATTRIBUTES, &device);

    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&queueConfig, WdfIoQueueDispatchSequential);

    queueConfig.EvtIoRead           = EvtIoRead;
    queueConfig.EvtIoWrite          = EvtIoWrite;
    queueConfig.EvtIoDeviceControl  = EvtIoDeviceControl;
    queueConfig.EvtIoInternalDeviceControl  = EvtIoInternalDeviceControl;

    status = WdfIoQueueCreate(device, &queueConfig, WDF_NO_OBJECT_ATTRIBUTES, WDF_NO_HANDLE);

    return status;
}

VOID
EvtIoRead(
    IN WDFQUEUE     Queue,
    IN WDFREQUEST   Request,
    IN size_t       Length
    )
{
    NTSTATUS status;
    PVOID pBuffer;
    PMDL pMdl;

    UNREFERENCED_PARAMETER(Queue);

    
    status = WdfRequestRetrieveOutputBuffer(Request, Length, &pBuffer, NULL);

    status = WdfRequestRetrieveOutputWdmMdl(Request, &pMdl);

    WdfRequestComplete(Request, status);

    /*
      This defect is injected for the "BufAfterReqCompletedReadA" rule.
    */
    SDVTest_wdf_BufferAfterReqCompletionReadAdd(pBuffer, Length);

  	/*
      This defect is injected for the "MdlAfterReqCompletedRead" rule.
    */
    SDVTest_wdf_MdlAfterReqCompletionReadAdd(pMdl);
}

VOID
EvtIoWrite(
    IN WDFQUEUE     Queue,
    IN WDFREQUEST   Request,
    IN size_t       Length
    )
{
    NTSTATUS status;
    WDFMEMORY memory;
    PMDL pMdl;

    UNREFERENCED_PARAMETER(Queue);

    status = WdfRequestRetrieveInputMemory(Request, &memory);

    status = WdfRequestRetrieveInputWdmMdl(Request, &pMdl);

    WdfRequestComplete(Request, status);

    /*
      This defect is injected for the "MemAfterReqCompletedWriteA" rule.
    */
    SDVTest_wdf_MemoryAfterReqCompletionWriteAdd(memory, Length);

    /*
      This defect is injected for the "MdlAfterReqCompletedWriteA" rule.
    */
    SDVTest_wdf_MdlAfterReqCompletionWriteAdd(pMdl);
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
    NTSTATUS    status;
    PVOID       pBuffer;


    UNREFERENCED_PARAMETER(Queue);
    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(InputBufferLength);
    UNREFERENCED_PARAMETER(IoControlCode);

    status = WdfRequestRetrieveInputBuffer(Request, InputBufferLength, &pBuffer, NULL);

    WdfRequestComplete(Request, status);

    /*
      This defect is injected for the "BufAfterReqCompletedIoctl" rule.
    */
    SDVTest_wdf_BufferAfterReqCompletionIoctl(Request, InputBufferLength, pBuffer);
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
    NTSTATUS    status;
    PVOID       pBuffer;


    UNREFERENCED_PARAMETER(Queue);
    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(InputBufferLength);
    UNREFERENCED_PARAMETER(IoControlCode);

    status = WdfRequestRetrieveInputBuffer(Request, OutputBufferLength, &pBuffer, NULL);

    WdfRequestComplete(Request, status);

    /*
      This defect is injected for the "BufAfterReqCompletedIntIoctlA" rule.
    */
    SDVTest_wdf_BufferAfterReqCompletionIntIoctlAdd(OutputBufferLength, pBuffer);
}
