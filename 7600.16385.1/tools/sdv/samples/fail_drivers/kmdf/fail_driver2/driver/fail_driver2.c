/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    fail_driver2.c

Abstract:

    This is a sample driver that contains intentionally placed code defects 
    in order to illustrate how Static Driver Verifier works. This driver is
    not functional and not intended as a sample for real driver development
    projects.

Environment:

    Kernel mode

--*/

#include "fail_driver2.h"

#ifndef __cplusplus
#pragma alloc_text (INIT, DriverEntry)
#pragma alloc_text (PAGE, EvtDriverDeviceAdd)
#pragma alloc_text (PAGE, EvtDeviceContextCleanup)
#pragma alloc_text (PAGE, EvtIoRead)
#pragma alloc_text (PAGE, EvtIoWrite)
#endif

NTSTATUS 
DriverEntry( 
    IN PDRIVER_OBJECT   DriverObject, 
    IN PUNICODE_STRING  RegistryPath
    )
{
    WDFDRIVER               driver                      = NULL;
    NTSTATUS                status                      = STATUS_SUCCESS;
    PWDFDEVICE_INIT         pControlDeviceInit          = NULL;
    WDFDEVICE               controlDevice               = NULL;
    WDF_OBJECT_ATTRIBUTES   controlDevObjectAttributes;  
    WDF_DRIVER_CONFIG       driverConfig;
    UNICODE_STRING	        securityDescriptor;	
    
    RtlInitUnicodeString(&securityDescriptor, L"D:P"); 

    WDF_DRIVER_CONFIG_INIT(&driverConfig, EvtDriverDeviceAdd);

    status = WdfDriverCreate(DriverObject, RegistryPath, WDF_NO_OBJECT_ATTRIBUTES, &driverConfig, &driver);
    if(!NT_SUCCESS(status)){
        return status;
    }
    
    pControlDeviceInit = WdfControlDeviceInitAllocate(driver, &securityDescriptor);
    if (pControlDeviceInit == NULL) {
        return STATUS_UNSUCCESSFUL;
    }

    WDF_OBJECT_ATTRIBUTES_INIT(&controlDevObjectAttributes);

    controlDevObjectAttributes.EvtCleanupCallback = EvtDeviceContextCleanup;

    status = WdfDeviceCreate(&pControlDeviceInit, &controlDevObjectAttributes, &controlDevice);
    if(!NT_SUCCESS(status)){
        return status;
    }

    /*
      This defect is injected for the "ControlDeviceInitAPI" rule.
    */
    SDVTest_wdf_DeviceInitAPI(pControlDeviceInit);

    /*
      This defect is injected for the "CtlDeviceFinishInitDrEntry" rule.
    */
    SDVTest_wdf_ControlFinishInitializingInDriverEntry();
    return status;    
}

NTSTATUS
EvtDriverDeviceAdd(
    IN WDFDRIVER        Driver,
    IN PWDFDEVICE_INIT  DeviceInit
    )
{
    NTSTATUS    status = STATUS_SUCCESS;
    WDFDEVICE   device = NULL;
    WDF_IO_QUEUE_CONFIG     ioQueueConfig;
    WDF_OBJECT_ATTRIBUTES   queueAttributes;
 
    UNREFERENCED_PARAMETER(Driver);	

    /*
      This defect is injected for the "FDODriver" rule.
    */
    SDVTest_wdf_FDODriver(DeviceInit);

    status = WdfDeviceCreate(&DeviceInit, WDF_NO_OBJECT_ATTRIBUTES, &device);

    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE (
        &ioQueueConfig,
        WdfIoQueueDispatchSequential
        );

    ioQueueConfig.EvtIoWrite = EvtIoWrite;
    ioQueueConfig.EvtIoRead          = EvtIoRead;
    WDF_OBJECT_ATTRIBUTES_INIT(&queueAttributes);

    status = WdfIoQueueCreate( device,
                               &ioQueueConfig,
                               &queueAttributes,
                               WDF_NO_HANDLE );

    return status;
}

VOID
EvtDeviceContextCleanup(
    IN WDFOBJECT  Object
    )
{
    UNREFERENCED_PARAMETER(Object);
    /*
      This defect is injected for the "ControlDeviceDeleted" rule.
    */
    SDVTest_wdf_ControlDeviceDeleted();
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

    WdfRequestCompleteWithInformation(Request, STATUS_SUCCESS, 0L);
    
    /*
      This defect is injected for the "DoubleCompletion" rule.
    */
    SDVTest_wdf_DoubleCompletion(Request);
    
    return;
}

VOID
EvtIoWrite(
    IN WDFQUEUE     Queue,
    IN WDFREQUEST   Request,
    IN size_t       Length
    )
{
    NTSTATUS            status              = STATUS_SUCCESS;
    WDFDEVICE           childDevice         = NULL;
    PWDFDEVICE_INIT     pChildDeviceInit    = NULL;

    UNREFERENCED_PARAMETER(Length);

    pChildDeviceInit = WdfPdoInitAllocate(WdfIoQueueGetDevice(Queue));
    if(pChildDeviceInit == NULL){
        status = STATUS_UNSUCCESSFUL;
        goto Cleanup;
    }

    /*
      This defect is injected for the "InitFreeDeviceCreateType2" rule.
    */
    SDVTest_InitFreeDeviceCreateType2(pChildDeviceInit);

    status = WdfDeviceCreate(&pChildDeviceInit, WDF_NO_OBJECT_ATTRIBUTES, &childDevice);
    if(!NT_SUCCESS(status)){
        goto Cleanup;
    }

    /*
      This defect is injected for the "PdoDeviceInitAPI" rule.
    */
    SDVTest_wdf_DeviceInitAPI(pChildDeviceInit);

Cleanup:

    WdfRequestComplete(Request, status);
    return;
}
