/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    fail_driver6.c

Abstract:

    This is a sample driver that contains intentionally placed code defects 
    in order to illustrate how Static Driver Verifier works. This driver is
    not functional and not intended as a sample for real driver development
    projects.

Environment:

    Kernel mode

--*/

#include "fail_driver6.h"

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
    WDF_TIMER_CONFIG       timerConfig;
    WDF_OBJECT_ATTRIBUTES  timerAttributes;
    WDFQUEUE queue;
    WDFTIMER       Timer;


    UNREFERENCED_PARAMETER(Driver);

    status = WdfDeviceCreate(&DeviceInit, WDF_NO_OBJECT_ATTRIBUTES, &device);

    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&queueConfig, WdfIoQueueDispatchSequential);

    queueConfig.EvtIoRead           = EvtIoRead;
    queueConfig.EvtIoWrite          = EvtIoWrite;
    queueConfig.EvtIoDeviceControl  = EvtIoDeviceControl;

    status = WdfIoQueueCreate(device, &queueConfig, WDF_NO_OBJECT_ATTRIBUTES, &queue);


    //
    // Create a WDFTIMER object
    //
    WDF_TIMER_CONFIG_INIT_PERIODIC(&timerConfig, EvtTimerFunc, 100);

    WDF_OBJECT_ATTRIBUTES_INIT(&timerAttributes);
    timerAttributes.ParentObject = queue; // Synchronize with the I/O Queue

    status = WdfTimerCreate(
                             &timerConfig,
                             &timerAttributes,
                             &Timer     // Output handle
                             );


    return status;
}

VOID
EvtIoRead(
    IN WDFQUEUE     Queue,
    IN WDFREQUEST   Request,
    IN size_t       Length
    )
{
	NTSTATUS	status;
	PVOID		pBuffer;
	WDFMEMORY	memory;
	PMDL		pMdl;

        UNREFERENCED_PARAMETER(Queue);

	status = WdfRequestRetrieveOutputBuffer(Request, Length, &pBuffer, NULL);

	status = WdfRequestRetrieveOutputMemory(Request, &memory);

	status = WdfRequestRetrieveOutputWdmMdl(Request, &pMdl);

	WdfRequestComplete(Request, status);
	
        /*
        This defect is injected for the "BufAfterReqCompletedRead" rule.
        */
        SDVTest_wdf_BufferAfterReqCompletionRead(Request, Length, pBuffer);

       	/*
       	This defect is injected for the "MemAfterReqCompletedRead" rule.
       	*/
	SDVTest_wdf_MemoryAfterReqCompletionRead(Request, memory);

       	/*
       	This defect is injected for the "MdlAfterReqCompletedRead" rule.
       	*/
    	SDVTest_wdf_MdlAfterReqCompletionRead(Request, pMdl);
}

VOID
EvtIoWrite(
    IN WDFQUEUE     Queue,
    IN WDFREQUEST   Request,
    IN size_t       Length
    )
{
	NTSTATUS	status;
	PVOID		pBuffer;
	PMDL		pMdl;
        
        UNREFERENCED_PARAMETER(Queue);
	
	status = WdfRequestRetrieveInputBuffer(Request, Length, &pBuffer, NULL);

	status = WdfRequestRetrieveOutputWdmMdl(Request, &pMdl);

	WdfRequestComplete(Request, status);
	
       	/*
       	This defect is injected for the "BufAfterReqCompletedWrite" rule.
       	*/
       	SDVTest_wdf_BufferAfterReqCompletionWrite(Request, Length, pBuffer);

	/*
      	This defect is injected for the "MdlAfterReqCompletedWrite" rule.
	*/
	SDVTest_wdf_MdlAfterReqCompletionWrite(Request, pMdl);


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
	NTSTATUS	status;
	WDFMEMORY	memory;

        UNREFERENCED_PARAMETER(Queue);
        UNREFERENCED_PARAMETER(OutputBufferLength);
        UNREFERENCED_PARAMETER(InputBufferLength);
        UNREFERENCED_PARAMETER(IoControlCode);
	
	status = WdfRequestRetrieveOutputMemory(Request, &memory);

	WdfRequestComplete(Request, status);
	
    	/*
      	This defect is injected for the "MemAfterReqCompletedIoctl" rule.
    	*/
    	SDVTest_wdf_MemoryAfterReqCompletionIoctl(Request, memory);
}


VOID
EvtIoIntDeviceControl(
    IN WDFQUEUE     Queue,
    IN WDFREQUEST   Request,
    IN size_t       OutputBufferLength,
    IN size_t       InputBufferLength,
    IN ULONG        IoControlCode
    )
{
        PQUEUE_CONTEXT queueContext ;

        UNREFERENCED_PARAMETER(OutputBufferLength);
        UNREFERENCED_PARAMETER(InputBufferLength);
        UNREFERENCED_PARAMETER(IoControlCode);

	queueContext = QueueGetContext(Queue);
	queueContext->CurrentRequest = Request;
        __sdv_save_request(Request);

       return;
}

VOID
EvtTimerFunc(
    IN WDFTIMER  Timer
    )
{
    WDFQUEUE queue;
    PQUEUE_CONTEXT queueContext ;

    queue = (WDFQUEUE)WdfTimerGetParentObject(Timer);
    queueContext = QueueGetContext(queue);

    UNREFERENCED_PARAMETER(Timer);   
    __sdv_retrieve_request(queueContext->CurrentRequest); 
 
    /*
    This defect is injected for DeferredRequestComplete rule.
    */
    SDVTest_wdf_DeferredRequestCompletedNoAlias(queueContext->CurrentRequest);
}

VOID
DeviceContextCleanUp(
    IN WDFOBJECT Object
    )
{
    UNREFERENCED_PARAMETER(Object);
}

VOID
DeviceContextDestroy(
    IN WDFOBJECT Object
    )
{
    UNREFERENCED_PARAMETER(Object);
}

VOID
QueueCleanup(
    IN WDFOBJECT Object
    )
{
    UNREFERENCED_PARAMETER(Object);	
}

VOID
QueueDestroy(
    IN WDFOBJECT Object
    )
{
    UNREFERENCED_PARAMETER(Object);	
}

VOID
FileContextCleanup(
    IN WDFOBJECT Object
    )
{
    UNREFERENCED_PARAMETER(Object);
}

VOID
FileContextDestroy(
    IN WDFOBJECT Object
    )
{
    UNREFERENCED_PARAMETER(Object);
}
