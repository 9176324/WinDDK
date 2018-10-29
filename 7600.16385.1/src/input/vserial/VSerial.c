/*++
    Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

    Module Name:

        VSerial.c

    Abstract:
        Driver entry and support functions

    Environment:
        Kernel mode


--*/

#include "VSerial.h"


NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    )
/*++

Routine Description:

    This routine initializes the mouse class driver.

Arguments:

    DriverObject - Pointer to driver object created by system.

    RegistryPath - Pointer to the Unicode name of the registry path
        for this driver.

Return Value:

    The function value is the final status from the initialization operation.

--*/
{
    NTSTATUS            status = STATUS_SUCCESS;
    WDF_DRIVER_CONFIG   config = {0};

    KdPrint(("Entering DriverEntry Built %s %s\n", __DATE__, __TIME__));

    WDF_DRIVER_CONFIG_INIT(&config, VSerialAddDevice);

    //
    // Create a framework driver object to represent our driver.
    //
    status = WdfDriverCreate(
                            DriverObject,
                            RegistryPath,
                            WDF_NO_OBJECT_ATTRIBUTES, // Driver Attributes
                            &config,          // Driver Config Info
                            WDF_NO_HANDLE
                            );

    if (!NT_SUCCESS(status)) {
        KdPrint( ("WdfDriverCreate failed with status 0x%x\n", status));
    }

    return status;
}

NTSTATUS
VSerialAddDevice(
    IN WDFDRIVER        Driver,
    IN PWDFDEVICE_INIT  DeviceInit
    )
{
    NTSTATUS                            status = STATUS_SUCCESS;
    WDFDEVICE                           device = {0};
    WDF_OBJECT_ATTRIBUTES               attributes = {0};
    WDF_PNPPOWER_EVENT_CALLBACKS        pnpPowerCallbacks = {0};
    PSERIAL_DEVICE_EXTENSION            devExt = NULL;
    WDF_IO_QUEUE_CONFIG                 ioQueueConfig = {0};
    UCHAR minorFunction[] = {IRP_MN_QUERY_ID};

    UNREFERENCED_PARAMETER(Driver);

    WdfFdoInitSetFilter(DeviceInit);

    //
    // Register pnp callback so that we can create a polling thread when the
    // device is started.
    //
    WDF_PNPPOWER_EVENT_CALLBACKS_INIT(&pnpPowerCallbacks);
    pnpPowerCallbacks.EvtDeviceSelfManagedIoInit = VSerialEvtDeviceSelfManagedIoInit;
    WdfDeviceInitSetPnpPowerEventCallbacks(DeviceInit, &pnpPowerCallbacks);

    //
    // Register preprocess callback to handle IRP_MN_QUERY_ID. This is an hack
    // to make a virtual hid device to enumerate.
    //
    status = WdfDeviceInitAssignWdmIrpPreprocessCallback(
                                            DeviceInit,
                                            VSerialPnpIrpPreprocessCallback,
                                            IRP_MJ_PNP,
                                            minorFunction, // pointer minor function table
                                            sizeof(minorFunction)/sizeof(minorFunction[0])
                                            ); // number of entries in the table

    if (!NT_SUCCESS(status)) {
        KdPrint(("WdfDeviceInitAssignWdmIrpPreprocessCallback failed 0x%x\n", status));
        return status;
    }

    // 
   // We are registering WDM preprocess callback to handle create, cleanup and close 
   // requests. You should use WdfDeviceInitSetFileObjectConfig to register create, 
   // cleanup and close events under normal circumstances. We register these WDM 
   // escape callbacks so that we can fail cleanup and close IRPs and verify the error code path.
   // A real driver should use framework interfaces to handle these requests.
   //

    status = WdfDeviceInitAssignWdmIrpPreprocessCallback(
                                            DeviceInit,
                                            VSerialWdmFileCreate,
                                            IRP_MJ_CREATE,
                                            NULL,
                                            0); 
    if (!NT_SUCCESS(status)) {
        KdPrint(("WdfDeviceInitAssignWdmIrpPreprocessCallback for IRP_MJ_CREATE failed 0x%x\n", status));
        return status;
    }

    status = WdfDeviceInitAssignWdmIrpPreprocessCallback(
                                            DeviceInit,
                                            VSerialWdmFileClose,
                                            IRP_MJ_CLOSE,
                                            NULL,
                                            0); 

    if (!NT_SUCCESS(status)) {
        KdPrint(("WdfDeviceInitAssignWdmIrpPreprocessCallback for failed IRP_MJ_CLOSE 0x%x\n", status));
        return status;
    }

    status = WdfDeviceInitAssignWdmIrpPreprocessCallback(
                                            DeviceInit,
                                            VSerialWdmFileCleanup,
                                            IRP_MJ_CLEANUP,
                                            NULL,
                                            0); 

    if (!NT_SUCCESS(status)) {
        KdPrint(("WdfDeviceInitAssignWdmIrpPreprocessCallback for IRP_MJ_CLEANUP failed 0x%x\n", status));
        return status;
    }


    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes, SERIAL_DEVICE_EXTENSION);

    //
    // Registerd context cleanup so that we can terminate the thread when
    // device is deleted.
    //
    attributes.EvtCleanupCallback = VSerialDeviceContextCleanup;

    //
    // Set execution level passive so that queue callbacks are invoked at
    // passive-level even if the request is sent to this device at DISPATCH_LEVEL.
    //
    attributes.ExecutionLevel = WdfExecutionLevelPassive;

    status = WdfDeviceCreate(&DeviceInit, &attributes, &device);
    if (!NT_SUCCESS(status)) {
        KdPrint(("WdfDeviceCreate failed 0x%x\n", status));
        return status;
    }

    devExt = GetDeviceContext(device);

    //
    // Initialize an event to notify the polling thread when the device is removed.
    //
    KeInitializeEvent(&devExt->TerminateWorkerThread, NotificationEvent, FALSE);

    //
    // Create a toplevel default queue to receive read, write and ioctl requests.
    //
    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&ioQueueConfig,
                                           WdfIoQueueDispatchParallel);

    ioQueueConfig.EvtIoDeviceControl = VSerialEvtIoDeviceControl;
    ioQueueConfig.EvtIoInternalDeviceControl = VSerialEvtIoInternalDeviceControl;
    ioQueueConfig.EvtIoWrite = VSerialEvtIoWrite;
    ioQueueConfig.EvtIoRead = VSerialEvtIoRead;
    
    status = WdfIoQueueCreate(device,
                         &ioQueueConfig,
                         WDF_NO_OBJECT_ATTRIBUTES,
                         WDF_NO_HANDLE);
    if (!NT_SUCCESS(status)) {
        KdPrint(("WdfIoQueueCreate failed  %x\n", status));
        return status;
    }

    //
    // Create a manual queue for holding read request.
    //
    WDF_IO_QUEUE_CONFIG_INIT(&ioQueueConfig,
                                           WdfIoQueueDispatchManual);
    status = WdfIoQueueCreate(device,
                         &ioQueueConfig,
                         WDF_NO_OBJECT_ATTRIBUTES,
                         &devExt->ReadQueue);
    if (!NT_SUCCESS(status)) {
        KdPrint(("WdfIoQueueCreate failed  %x\n", status));
        return status;
    }

    status = RegOpenDeviceKey(devExt);

    return status;
}

NTSTATUS
VSerialEvtDeviceSelfManagedIoInit(
    IN WDFDEVICE  Device
    )
{
    PSERIAL_DEVICE_EXTENSION devExt = GetDeviceContext(Device);
    NTSTATUS            status = STATUS_SUCCESS;
    HANDLE                hThread = 0;
    OBJECT_ATTRIBUTES    oa;

    if(NULL == devExt->Thread)
    {
        status = RegOpenDeviceKey(devExt);
        if(!NT_SUCCESS(status))
        {
            //the driver can still function so we don't fail.
            KdPrint(("Cannot open sim data Key!\n"));
        }
        InitializeObjectAttributes(&oa, NULL, 
            OBJ_KERNEL_HANDLE, NULL, NULL);
        //log file must be available when the driver is loaded!
        status = OpenDeviceDataFile(devExt);
        if(NT_SUCCESS(status))
        {
            //create worker thread
            status = PsCreateSystemThread(&hThread, THREAD_ALL_ACCESS, &oa, NULL, NULL,
                                          ReadIrpCompletionRoutine, devExt);
            if(!NT_SUCCESS(status))
            {
                KdPrint(("failed to create worker thread"));
                return status;
            }

            ObReferenceObjectByHandle(hThread, THREAD_ALL_ACCESS, NULL,
                                      KernelMode, (PVOID*)&devExt->Thread, NULL);
            ZwClose(hThread);

        }
        else
        {
            KdPrint(("Unable to open device data file!\n"));
            status = STATUS_UNSUCCESSFUL;
        }
    }
    return status;
}

VOID
VSerialDeviceContextCleanup(
    IN WDFDEVICE  Device
    )
{
    PSERIAL_DEVICE_EXTENSION devExt = GetDeviceContext(Device);
    NTSTATUS                 status = STATUS_SUCCESS;

    // detach device and delete resources
    KdPrint(("setting thread terminate event!\n"));
    if(NULL != devExt->Thread)
    {
        KeSetEvent(&devExt->TerminateWorkerThread, 0, FALSE);
        status = KeWaitForSingleObject(devExt->Thread, Executive, KernelMode, FALSE, NULL);
        if(!NT_SUCCESS(status))
        {
            KdPrint(("KeWaitForSingleObject didn't succeed status:%d!\n", status));
        }
        ObDereferenceObject(devExt->Thread);
    }
    if(NULL != devExt->SimDevKey)
    {
        WdfRegistryClose(devExt->SimDevKey);
    }
    return;
}

NTSTATUS
VSerialPnpIrpPreprocessCallback (
    IN WDFDEVICE Device,
    IN PIRP Irp
    )
{
    NTSTATUS status = STATUS_SUCCESS;
    PIO_STACK_LOCATION  irpsp = NULL;
    PWCHAR  buffer = NULL;
    PIO_STACK_LOCATION minidriverFilterSp = NULL;
    PIO_STACK_LOCATION hidclassSp = NULL;

    UNREFERENCED_PARAMETER(Device);

    KdPrint(("Enter VSerialPnpIrpPreprocessCallback for IRP_MN_QUERY_ID\n"));

    //
    // This check is required to filter out QUERY_IDs forwarded
    // by the HIDCLASS for the parent FDO. These IDs are sent
    // by PNP manager for the parent FDO if you root-enumerate this driver.
    //
    irpsp = IoGetCurrentIrpStackLocation(Irp);
    minidriverFilterSp = irpsp + 1;
    hidclassSp = minidriverFilterSp + 1;

    if (hidclassSp->DeviceObject == minidriverFilterSp->DeviceObject) {
        //
        // Filtering out this prevents the Found New Hardware
        // popup on pre-Vista OS for the root-enumerated vserial on 
        // reboot. On Vista, if not filtered, the enumeration of HID 
        // minidriver fails upon reboot
        // 
        status = Irp->IoStatus.Status;
        IoCompleteRequest (Irp, IO_NO_INCREMENT);
        return status;
    }

    switch (irpsp->Parameters.QueryId.IdType) {
    case BusQueryDeviceID:
    case BusQueryHardwareIDs:
        //
        // HIDClass is asking for child deviceid & hardwareids.
        // Let us just make up some  id for our child device.
        //
        buffer = (PWCHAR)ExAllocatePoolWithTag(
                    NonPagedPool,
                    HPEN_HARDWARE_IDS_LENGTH,
                    VSERIAL_TAG
                    );

        if (buffer) {
            //
            // Do the copy, store the buffer in the Irp
            //
            RtlCopyMemory(buffer,
                        HPEN_HARDWARE_IDS,
                        HPEN_HARDWARE_IDS_LENGTH
                        );

            Irp->IoStatus.Information = (ULONG_PTR)buffer;
            status = STATUS_SUCCESS;
        }
        else {
            //
            //  No memory
            //
            status = STATUS_INSUFFICIENT_RESOURCES;
        }

        break;
    default:
        status = Irp->IoStatus.Status;
        break;
    } //switch (irpsp->Parameters.QueryId.IdType)

    //
    // We don't need to forward this to our bus. This query
    // is for our child so we should complete it right here.
    //
    Irp->IoStatus.Status = status;
    IoCompleteRequest (Irp, IO_NO_INCREMENT);
    return status;
}

VOID
VSerialEvtIoWrite(
    IN WDFQUEUE         Queue,
    IN WDFREQUEST       Request,
    IN size_t           Length
    )
{
    NTSTATUS status = STATUS_SUCCESS;
    PSERIAL_DEVICE_EXTENSION devExt = NULL;

    devExt = GetDeviceContext(WdfIoQueueGetDevice(Queue));

    KdPrint(("Write count: %d\n", ++devExt->WriteCount));

    status = QueryDesiredReadWriteResponse(IRP_MJ_WRITE, devExt);
    if (NT_SUCCESS(status)) {
        WdfRequestSetInformation(Request, Length);
    }

    WdfRequestComplete(Request, status);
}

VOID
VSerialEvtIoRead(
    IN WDFQUEUE         Queue,
    IN WDFREQUEST       Request,
    IN size_t           Length
    )
{
    NTSTATUS                    status = STATUS_SUCCESS;
    PSERIAL_DEVICE_EXTENSION    devExt = NULL;
    
    UNREFERENCED_PARAMETER(Length);

    devExt = GetDeviceContext(WdfIoQueueGetDevice(Queue));

    KdPrint(("Read count: %d\n", ++devExt->ReadCount));

    status = QueryDesiredReadWriteResponse(IRP_MJ_READ, devExt);
    if (NT_SUCCESS(status)) {
        status = WdfRequestForwardToIoQueue(Request, devExt->ReadQueue);
        if (NT_SUCCESS(status)) {
            return;
        } else {
            KdPrint(("WdfRequestForwardToIoQueue failed: %x\n", status));
        }
    }

    WdfRequestComplete(Request, status);
    return;
}

NTSTATUS
VSerialWdmFileCreate(
    IN WDFDEVICE Device,
    IN PIRP Irp
    )
{
    PSERIAL_DEVICE_EXTENSION    devExt = NULL;
    ULONG                       dwData = 0;
    NTSTATUS                    status = STATUS_SUCCESS;
    NTSTATUS                    statusReg = STATUS_SUCCESS;
    
   devExt = GetDeviceContext(Device);
    
    KdPrint((" - IRP_MJ_CREATE\n"));

    if(NULL != devExt->SimDevKey)
    {
        statusReg = RegQueryValue(devExt->SimDevKey,
                                   CREATE_RESPONSE,
                                   REG_DWORD,
                                   (PUCHAR)&dwData,
                                   sizeof(dwData),
                                   NULL);
        if(NT_SUCCESS(statusReg))
        {
            KdPrint(("registry request to fail IRP_MJ_CREATE\n"));
            status = STATUS_INVALID_DEVICE_STATE;
        }        
    }
    Irp->IoStatus.Status = status;
    IoCompleteRequest (Irp, IO_NO_INCREMENT);
    return status;
}

NTSTATUS
VSerialWdmFileCleanup(
    IN WDFDEVICE Device,
    IN PIRP Irp
    )
{
    PSERIAL_DEVICE_EXTENSION    devExt = NULL;
    ULONG                       dwData = 0;
    NTSTATUS                    status = STATUS_SUCCESS;
    NTSTATUS                    statusReg = STATUS_SUCCESS;
    
     devExt = GetDeviceContext(Device);
    
    KdPrint((" - IRP_MJ_CLEANUP\n"));

    if(NULL != devExt->SimDevKey)
    {
        statusReg = RegQueryValue(devExt->SimDevKey,
                                   CREATE_RESPONSE,
                                   REG_DWORD,
                                   (PUCHAR)&dwData,
                                   sizeof(dwData),
                                   NULL);
        if(NT_SUCCESS(statusReg))
        {
            KdPrint(("registry request to fail IRP_MJ_CLEANUP\n"));
            status = STATUS_INVALID_DEVICE_STATE;
        }        
    }
    Irp->IoStatus.Status = status;
    IoCompleteRequest (Irp, IO_NO_INCREMENT);
    return status;
}

NTSTATUS
VSerialWdmFileClose(
    IN WDFDEVICE Device,
    IN PIRP Irp
    )
{
    PSERIAL_DEVICE_EXTENSION    devExt = NULL;
    ULONG                       dwData = 0;
    NTSTATUS                    status = STATUS_SUCCESS;
    NTSTATUS                    statusReg = STATUS_SUCCESS;
    
    devExt = GetDeviceContext(Device);
    
    KdPrint((" - IRP_MJ_CLOSE\n"));

    if(NULL != devExt->SimDevKey)
    {
        statusReg = RegQueryValue(devExt->SimDevKey,
                                   CREATE_RESPONSE,
                                   REG_DWORD,
                                   (PUCHAR)&dwData,
                                   sizeof(dwData),
                                   NULL);
        if(NT_SUCCESS(statusReg))
        {
            KdPrint(("registry request to fail IRP_MJ_CLOSE\n"));
            status = STATUS_INVALID_DEVICE_STATE;
        }        
    }
    Irp->IoStatus.Status = status;
    IoCompleteRequest (Irp, IO_NO_INCREMENT);
    return status;
}

VOID
VSerialEvtIoDeviceControl(
    IN WDFQUEUE   Queue,
    IN WDFREQUEST Request,
    IN size_t     OutputBufferLength,
    IN size_t     InputBufferLength,
    IN ULONG      IoControlCode
    )
{
    NTSTATUS status = STATUS_SUCCESS;
    PSERIAL_DEVICE_EXTENSION devExt = NULL;
    
    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(InputBufferLength);

    devExt = GetDeviceContext(WdfIoQueueGetDevice(Queue));

    status = QueryDesiredControlResponse(IoControlCode, devExt);

    WdfRequestComplete(Request, status);
}


VOID
ReadIrpCompletionRoutine(
    IN PVOID pContext
    )
/*++

Routine Description:

    This routine does most of the work in the virtual driver. It waits for a data file to become
    available and then reads the data and uses the contents to complete queued IRPs


Arguments:

    pContext        - Device extension

Return Value:

   None.

--*/
{
    PSERIAL_DEVICE_EXTENSION        DevExt =  (PSERIAL_DEVICE_EXTENSION)pContext;
    NTSTATUS                        status = STATUS_SUCCESS;
    NTSTATUS                        status_io = STATUS_SUCCESS;
    PCHAR                            buffer = NULL;
    size_t                            bufferLength = 0;
    LARGE_INTEGER                    Timeout = {0};
    BOOLEAN                         Opened = FALSE;
    WDFREQUEST request;

    KdPrint(("Polling thread to complete read request started....\n"));

    while(FALSE == Opened)
    {
        //first we check if the thread should be aborted.
        KdPrint(("It is ok to proceed with check for pending IRP\n"));
        Timeout.QuadPart = Int32x32To64(80, -10000);
        status = KeWaitForSingleObject(&DevExt->TerminateWorkerThread, Executive,
                                       KernelMode, FALSE, &Timeout);
        if(STATUS_WAIT_0 == status)
        {
            //the driver is telling this thread to die
            KdPrint(("Exiting Thread!\n"));
            break;
        }
        //complete the IRP with data from file
        if(STATUS_TIMEOUT == status)
        {
            Timeout.QuadPart = Int32x32To64(80, -10000);

            KeDelayExecutionThread(KernelMode, FALSE, &Timeout);

            status = WdfIoQueueRetrieveNextRequest(DevExt->ReadQueue, &request);

            if (NT_SUCCESS(status))
            {
                KdPrint(("got available request\n"));

                status = WdfRequestRetrieveOutputBuffer(request, 0, &buffer, &bufferLength);
                if (NT_SUCCESS(status)) {
                    status = ReadDeviceBuffer(buffer, (ULONG)bufferLength, DevExt);
                    if(NT_SUCCESS(status))
                    {
                        // if we read data and the length of the read is the expected amount
                        KdPrint(("Successfully read %d bytes of data\n", bufferLength));
                        WdfRequestCompleteWithInformation(request, status, bufferLength);
                    }
                }

                if (!NT_SUCCESS(status))
                {
                    // if we get here then the file has no more data so we fail the current IRP and
                    // wait for another data file to become available.
                    KdPrint(("failed to read data!!\n"));
                    WdfRequestComplete(request, STATUS_UNSUCCESSFUL);

                    //loop while waiting for new file
                    do
                    {
                        status_io = OpenDeviceDataFile(DevExt);
                        if(!NT_SUCCESS(status_io))
                        {
                            KdPrint(("Failed to open data file\n"));
                            //check to see if the driver wants this thread to die.
                            Timeout.QuadPart = Int32x32To64(5000, -10000);
                            status_io = KeWaitForSingleObject(&DevExt->TerminateWorkerThread,
                                                Executive,
                                                KernelMode, 
                                                TRUE, 
                                                &Timeout);

                            if(STATUS_WAIT_0 == status_io)
                            {
                                Opened = TRUE;
                                KdPrint(("Aborting Thread!\n"));
                                break;
                            }
                        }
                        else
                        {
                            //new file is available. We can resume read irp completions.
                            break;
                        }
                    }while(!NT_SUCCESS(status));
                }
            }
        }
    }

    if(NULL != DevExt->hDataFile)
    {
        DeleteDeviceDataFile(DevExt);
    }

    KdPrint(("Thread about to exit...\n"));

    PsTerminateSystemThread(STATUS_SUCCESS);
}

VOID
VSerialEvtIoInternalDeviceControl(
    IN WDFQUEUE   Queue,
    IN WDFREQUEST Request,
    IN size_t     OutputBufferLength,
    IN size_t     InputBufferLength,
    IN ULONG      IoControlCode
    )
{
    VSerialEvtIoDeviceControl(Queue, Request, OutputBufferLength, InputBufferLength, IoControlCode);
}

