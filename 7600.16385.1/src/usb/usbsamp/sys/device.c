/*++

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    Device.c

Abstract:

    Bulk USB device driver for Intel 82930 USB test board
    Plug and Play module. This file contains routines to handle pnp requests.

Environment:

    Kernel mode

--*/

#include "private.h"

#ifdef ALLOC_PRAGMA

#pragma alloc_text(PAGE, UsbSamp_EvtDeviceAdd)
#pragma alloc_text(PAGE, UsbSamp_EvtDevicePrepareHardware)
#pragma alloc_text(PAGE, ReadAndSelectDescriptors)
#pragma alloc_text(PAGE, ConfigureDevice)
#pragma alloc_text(PAGE, SelectInterfaces)
#pragma alloc_text(PAGE, UsbSampSetPowerPolicy)
#pragma alloc_text(PAGE, AbortPipes)
#pragma alloc_text(PAGE, UsbSamp_ReadFdoRegistryKeyValue)
#endif

NTSTATUS
UsbSamp_EvtDeviceAdd(
    IN WDFDRIVER        Driver,
    IN PWDFDEVICE_INIT  DeviceInit
    )
/*++
Routine Description:

    EvtDeviceAdd is called by the framework in response to AddDevice
    call from the PnP manager. We create and initialize a device object to
    represent a new instance of the device. All the software resources
    should be allocated in this callback.

Arguments:

    Driver - Handle to a framework driver object created in DriverEntry

    DeviceInit - Pointer to a framework-allocated WDFDEVICE_INIT structure.

Return Value:

    NTSTATUS

--*/
{
    WDF_FILEOBJECT_CONFIG     fileConfig;
    WDF_PNPPOWER_EVENT_CALLBACKS        pnpPowerCallbacks;
    WDF_OBJECT_ATTRIBUTES               fdoAttributes;
    WDF_OBJECT_ATTRIBUTES               fileObjectAttributes;
    WDF_OBJECT_ATTRIBUTES               requestAttributes;
    WDF_OBJECT_ATTRIBUTES               queueAttributes;
    NTSTATUS                            status;
    WDFDEVICE                           device;
    WDF_DEVICE_PNP_CAPABILITIES         pnpCaps;
    WDF_IO_QUEUE_CONFIG                 ioQueueConfig;
    PDEVICE_CONTEXT                     pDevContext;
    WDFQUEUE                            queue;
    ULONG                               maximumTransferSize;

    UNREFERENCED_PARAMETER(Driver);

    UsbSamp_DbgPrint (3, ("UsbSamp_EvtDeviceAdd routine\n"));

    PAGED_CODE();

    //
    // Initialize the pnpPowerCallbacks structure.  Callback events for PNP
    // and Power are specified here.  If you don't supply any callbacks,
    // the Framework will take appropriate default actions based on whether
    // DeviceInit is initialized to be an FDO, a PDO or a filter device
    // object.
    //

    WDF_PNPPOWER_EVENT_CALLBACKS_INIT(&pnpPowerCallbacks);

    pnpPowerCallbacks.EvtDevicePrepareHardware = UsbSamp_EvtDevicePrepareHardware;

    WdfDeviceInitSetPnpPowerEventCallbacks(DeviceInit, &pnpPowerCallbacks);

    //
    // Initialize the request attributes to specify the context size and type
    // for every request created by framework for this device.
    //
    WDF_OBJECT_ATTRIBUTES_INIT(&requestAttributes);
    WDF_OBJECT_ATTRIBUTES_SET_CONTEXT_TYPE(&requestAttributes, REQUEST_CONTEXT);

    WdfDeviceInitSetRequestAttributes(DeviceInit, &requestAttributes);

    //
    // Initialize WDF_FILEOBJECT_CONFIG_INIT struct to tell the
    // framework whether you are interested in handle Create, Close and
    // Cleanup requests that gets genereate when an application or another
    // kernel component opens an handle to the device. If you don't register
    // the framework default behaviour would be complete these requests
    // with STATUS_SUCCESS. A driver might be interested in registering these
    // events if it wants to do security validation and also wants to maintain
    // per handle (fileobject) context.
    //

    WDF_FILEOBJECT_CONFIG_INIT(
        &fileConfig,
        UsbSamp_EvtDeviceFileCreate,
        WDF_NO_EVENT_CALLBACK,
        WDF_NO_EVENT_CALLBACK
        );

    //
    // Specify a context for FileObject. If you register FILE_EVENT callbacks,
    // the framework by default creates a framework FILEOBJECT corresponding
    // to the WDM fileobject. If you want to track any per handle context,
    // use the context for FileObject. Driver that typically use FsContext
    // field should instead use Framework FileObject context.
    //
    WDF_OBJECT_ATTRIBUTES_INIT(&fileObjectAttributes);
    WDF_OBJECT_ATTRIBUTES_SET_CONTEXT_TYPE(&fileObjectAttributes, FILE_CONTEXT);

    WdfDeviceInitSetFileObjectConfig(DeviceInit,
                                       &fileConfig,
                                       &fileObjectAttributes);

#if !defined(BUFFERED_READ_WRITE)
    //
    // I/O type is Buffered by default. We want to do direct I/O for Reads
    // and Writes so set it explicitly. Please note that this sample
    // can do isoch transfer only if the io type is directio.
    //
    WdfDeviceInitSetIoType(DeviceInit, WdfDeviceIoDirect);

#endif

    //
    // Now specify the size of device extension where we track per device
    // context.DeviceInit is completely initialized. So call the framework
    // to create the device and attach it to the lower stack.
    //
    WDF_OBJECT_ATTRIBUTES_INIT(&fdoAttributes);
    WDF_OBJECT_ATTRIBUTES_SET_CONTEXT_TYPE(&fdoAttributes, DEVICE_CONTEXT);

    status = WdfDeviceCreate(&DeviceInit, &fdoAttributes, &device);
    if (!NT_SUCCESS(status)) {
        UsbSamp_DbgPrint(1, ("WdfDeviceCreate failed with Status code %!STATUS!\n", status));
        return status;
    }

    //
    // Get the DeviceObject context by using accessor function specified in
    // the WDF_DECLARE_CONTEXT_TYPE_WITH_NAME macro for DEVICE_CONTEXT.
    //

    pDevContext = GetDeviceContext(device);

    //
    //Get MaximumTransferSize from registry
    //
    maximumTransferSize=0;

    status = UsbSamp_ReadFdoRegistryKeyValue(Driver,
                                  L"MaximumTransferSize",
                                  &maximumTransferSize);

    if (!NT_SUCCESS(status)) {
        UsbSamp_DbgPrint(1, ("UsbSamp_ReadFdoRegistryKeyValue failed with Status code %!STATUS!\n", status));
        return status;
    }
    if(maximumTransferSize){
        pDevContext->MaximumTransferSize = maximumTransferSize;
    }
    else {
        pDevContext->MaximumTransferSize=DEFAULT_REGISTRY_TRANSFER_SIZE;
    }
    //
    // Tell the framework to set the SurpriseRemovalOK in the DeviceCaps so
    // that you don't get the popup in usermode (on Win2K) when you surprise
    // remove the device.
    //
    WDF_DEVICE_PNP_CAPABILITIES_INIT(&pnpCaps);
    pnpCaps.SurpriseRemovalOK = WdfTrue;

    WdfDeviceSetPnpCapabilities(device, &pnpCaps);

    //
    // Register I/O callbacks to tell the framework that you are interested
    // in handling WdfRequestTypeRead, WdfRequestTypeWrite, and IRP_MJ_DEVICE_CONTROL requests.
    // WdfIoQueueDispatchParallel means that we are capable of handling
    // all the I/O request simultaneously and we are responsible for protecting
    // data that could be accessed by these callbacks simultaneously.
    // This queue will be,  by default,  automanaged by the framework with
    // respect to PNP and Power events. That is, framework will take care
    // of queuing, failing, dispatching incoming requests based on the current
    // pnp/power state of the device.
    //

    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&ioQueueConfig,
                                           WdfIoQueueDispatchParallel);

    ioQueueConfig.EvtIoRead = UsbSamp_EvtIoRead;
    ioQueueConfig.EvtIoWrite = UsbSamp_EvtIoWrite;
    ioQueueConfig.EvtIoDeviceControl = UsbSamp_EvtIoDeviceControl;
    ioQueueConfig.EvtIoStop = UsbSampEvtIoStop;
    ioQueueConfig.EvtIoResume = UsbSampEvtIoResume;

    status = WdfIoQueueCreate(device,
                              &ioQueueConfig,
                              WDF_NO_OBJECT_ATTRIBUTES,
                              &queue);// pointer to default queue
    if (!NT_SUCCESS(status)) {
        UsbSamp_DbgPrint(1, ("WdfIoQueueCreate failed  for Default Queue %!STATUS!\n", status));
        return status;
    }

    WDF_IO_QUEUE_CONFIG_INIT(&ioQueueConfig,
                             WdfIoQueueDispatchParallel);

    WDF_OBJECT_ATTRIBUTES_INIT(&queueAttributes);
    queueAttributes.SynchronizationScope=WdfSynchronizationScopeQueue;
    

    ioQueueConfig.EvtIoRead = UsbSamp_EvtIsochRead;

    status = WdfIoQueueCreate(device,
                              &ioQueueConfig,
                              &queueAttributes,
                              &pDevContext->IsochReadQ);// pointer to IsochRead queue
    if (!NT_SUCCESS(status)) {
        UsbSamp_DbgPrint(1, ("WdfIoQueueCreate failed  for IsochRead Queue %!STATUS!\n", status));
        return status;
    }

    WDF_IO_QUEUE_CONFIG_INIT(&ioQueueConfig,
                             WdfIoQueueDispatchParallel);

    WDF_OBJECT_ATTRIBUTES_INIT(&queueAttributes);
    queueAttributes.SynchronizationScope=WdfSynchronizationScopeQueue;
    

    ioQueueConfig.EvtIoWrite = UsbSamp_EvtIsochWrite;

    status = WdfIoQueueCreate(device,
                              &ioQueueConfig,
                              &queueAttributes,
                              &pDevContext->IsochWriteQ);// pointer to IsochWrite queue
    if (!NT_SUCCESS(status)) {
        UsbSamp_DbgPrint(1, ("WdfIoQueueCreate failed  for IsochWrite Queue %!STATUS!\n", status));
        return status;
    }
    
    //
    // Register a device interface so that app can find our device and talk to it.
    //
    status = WdfDeviceCreateDeviceInterface(device,
                        (LPGUID) &GUID_CLASS_USBSAMP_USB,
                        NULL);// Reference String
    if (!NT_SUCCESS(status)) {
        UsbSamp_DbgPrint(1, ("WdfDeviceCreateDeviceInterface failed  %!STATUS!\n", status));
        return status;
    }

    UsbSamp_DbgPrint(3, ("EvtDriverDeviceAdd - ends\n"));

    return status;
}

NTSTATUS
UsbSamp_EvtDevicePrepareHardware(
    IN WDFDEVICE Device,
    IN WDFCMRESLIST ResourceList,
    IN WDFCMRESLIST ResourceListTranslated
    )
/*++

Routine Description:

    In this callback, the driver does whatever is necessary to make the
    hardware ready to use.  In the case of a USB device, this involves
    reading and selecting descriptors.

    //TODO:

Arguments:

    Device - handle to a device

Return Value:

    NT status value

--*/
{
    NTSTATUS                    status;
    PDEVICE_CONTEXT             pDeviceContext;
    WDF_USB_DEVICE_INFORMATION  info;

    UNREFERENCED_PARAMETER(ResourceList);
    UNREFERENCED_PARAMETER(ResourceListTranslated);

    UsbSamp_DbgPrint(3, ("EvtDevicePrepareHardware - begins\n"));

    PAGED_CODE();

    pDeviceContext = GetDeviceContext(Device);

    //
    // Read the device descriptor, configuration descriptor
    // and select the interface descriptors
    //
    status = ReadAndSelectDescriptors(Device);

    if (!NT_SUCCESS(status)) {
        UsbSamp_DbgPrint(1, ("ReadandSelectDescriptors failed\n"));
        return status;
    }

    WDF_USB_DEVICE_INFORMATION_INIT(&info);

    //
    // Retrieve USBD version information, port driver capabilites and device
    // capabilites such as speed, power, etc.
    //
    status = WdfUsbTargetDeviceRetrieveInformation(pDeviceContext->WdfUsbTargetDevice,
                                                   &info);
    if (NT_SUCCESS(status)) {
        pDeviceContext->IsDeviceHighSpeed =
            (info.Traits & WDF_USB_DEVICE_TRAIT_AT_HIGH_SPEED) ? TRUE : FALSE;

        UsbSamp_DbgPrint(3, ("DeviceIsHighSpeed: %s\n",
                     pDeviceContext->IsDeviceHighSpeed ? "TRUE" : "FALSE"));
    } else {
        pDeviceContext->IsDeviceHighSpeed = FALSE;
    }

    UsbSamp_DbgPrint(3, ("IsDeviceSelfPowered: %s\n",
        (info.Traits & WDF_USB_DEVICE_TRAIT_SELF_POWERED) ? "TRUE" : "FALSE"));

    pDeviceContext->WaitWakeEnable =
                        info.Traits & WDF_USB_DEVICE_TRAIT_REMOTE_WAKE_CAPABLE;

    UsbSamp_DbgPrint(3, ("IsDeviceRemoteWakeable: %s\n",
        (info.Traits & WDF_USB_DEVICE_TRAIT_REMOTE_WAKE_CAPABLE) ? "TRUE" : "FALSE"));

    //
    // Enable wait-wake and idle timeout if the device supports it
    //
    if(pDeviceContext->WaitWakeEnable){
        status = UsbSampSetPowerPolicy(Device);
        if (!NT_SUCCESS (status)) {
            UsbSamp_DbgPrint(3, ("UsbSampSetPowerPolicy failed\n"));
            return status;
        }
    }

    UsbSamp_DbgPrint(3, ("EvtDevicePrepareHardware - ends\n"));

    return status;
}

NTSTATUS
UsbSampSetPowerPolicy(
    __in WDFDEVICE Device
    )
{
    WDF_DEVICE_POWER_POLICY_IDLE_SETTINGS idleSettings;
    WDF_DEVICE_POWER_POLICY_WAKE_SETTINGS wakeSettings;
    NTSTATUS    status = STATUS_SUCCESS;

    PAGED_CODE();

    //
    // Init the idle policy structure.
    //
    WDF_DEVICE_POWER_POLICY_IDLE_SETTINGS_INIT(&idleSettings, IDLE_CAPS_TYPE);
    idleSettings.IdleTimeout = 10000; // 10-sec

    status = WdfDeviceAssignS0IdleSettings(Device, &idleSettings);
    if ( !NT_SUCCESS(status)) {
        UsbSamp_DbgPrint(3, ("WdfDeviceSetPowerPolicyS0IdlePolicy failed  %!STATUS!\n", status));
        return status;
    }

    //
    // Init wait-wake policy structure.
    //
    WDF_DEVICE_POWER_POLICY_WAKE_SETTINGS_INIT(&wakeSettings);

    status = WdfDeviceAssignSxWakeSettings(Device, &wakeSettings);
    if (!NT_SUCCESS(status)) {
        UsbSamp_DbgPrint(3, ("WdfDeviceAssignSxWakeSettings failed  %!STATUS!\n", status));
        return status;
    }

    return status;
}


NTSTATUS
ReadAndSelectDescriptors(
    IN WDFDEVICE Device
    )
/*++

Routine Description:

    This routine configures the USB device.
    In this routines we get the device descriptor,
    the configuration descriptor and select the
    configuration.

Arguments:

    Device - Handle to a framework device

Return Value:

    NTSTATUS - NT status value.

--*/
{
    NTSTATUS               status;
    PDEVICE_CONTEXT        pDeviceContext;

    PAGED_CODE();

    //
    // initialize variables
    //
    pDeviceContext = GetDeviceContext(Device);

    //
    // Create a USB device handle so that we can communicate with the
    // underlying USB stack. The WDFUSBDEVICE handle is used to query,
    // configure, and manage all aspects of the USB device.
    // These aspects include device properties, bus properties,
    // and I/O creation and synchronization. We only create device the first
    // the PrepareHardware is called. If the device is restarted by pnp manager
    // for resource rebalance, we will use the same device handle but then select
    // the interfaces again because the USB stack could reconfigure the device on
    // restart.
    //
    if (pDeviceContext->WdfUsbTargetDevice == NULL) {
        status = WdfUsbTargetDeviceCreate(Device,
                                    WDF_NO_OBJECT_ATTRIBUTES,
                                    &pDeviceContext->WdfUsbTargetDevice);
        if (!NT_SUCCESS(status)) {
            return status;
        }
    }
    
    WdfUsbTargetDeviceGetDeviceDescriptor(pDeviceContext->WdfUsbTargetDevice,
                                    &pDeviceContext->UsbDeviceDescriptor);

    ASSERT(pDeviceContext->UsbDeviceDescriptor.bNumConfigurations);

    status = ConfigureDevice(Device);

    return status;
}

NTSTATUS
ConfigureDevice(
    IN WDFDEVICE Device
    )
/*++

Routine Description:

    This helper routine reads the configuration descriptor
    for the device in couple of steps.

Arguments:

    Device - Handle to a framework device

Return Value:

    NTSTATUS - NT status value

--*/
{
    USHORT                        size = 0;
    NTSTATUS                      status;
    PDEVICE_CONTEXT               pDeviceContext;
    PUSB_CONFIGURATION_DESCRIPTOR configurationDescriptor;
    WDF_OBJECT_ATTRIBUTES attributes;
    WDFMEMORY   memory;

    PAGED_CODE();

    //
    // initialize the variables
    //
    configurationDescriptor = NULL;
    pDeviceContext = GetDeviceContext(Device);

    //
    // Read the first configuration descriptor
    // This requires two steps:
    // 1. Ask the WDFUSBDEVICE how big it is
    // 2. Allocate it and get it from the WDFUSBDEVICE
    //
    status = WdfUsbTargetDeviceRetrieveConfigDescriptor(pDeviceContext->WdfUsbTargetDevice,
                                               NULL,
                                               &size);

    if (status != STATUS_BUFFER_TOO_SMALL || size == 0) {
        return status;
    }

    //
    // Create a memory object and specify usbdevice as the parent so that
    // it will be freed automatically.
    //
    WDF_OBJECT_ATTRIBUTES_INIT(&attributes);

    attributes.ParentObject = pDeviceContext->WdfUsbTargetDevice;

    status = WdfMemoryCreate(&attributes,
                             NonPagedPool,
                             POOL_TAG,
                             size,
                             &memory,
                             &configurationDescriptor);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    status = WdfUsbTargetDeviceRetrieveConfigDescriptor(pDeviceContext->WdfUsbTargetDevice,
                                               configurationDescriptor,
                                               &size);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    pDeviceContext->UsbConfigurationDescriptor = configurationDescriptor;

    status = SelectInterfaces(Device);

    return status;
}

NTSTATUS
SelectInterfaces(
    IN WDFDEVICE Device
    )
/*++

Routine Description:

    This helper routine selects the configuration, interface and
    creates a context for every pipe (end point) in that interface.

Arguments:

    Device - Handle to a framework device

Return Value:

    NT status value

--*/
{
    WDF_USB_DEVICE_SELECT_CONFIG_PARAMS configParams;
    NTSTATUS                            status;
    PDEVICE_CONTEXT                     pDeviceContext;

    PAGED_CODE();

    pDeviceContext = GetDeviceContext(Device);

    //
    // The device has only 1 interface.
    //
    WDF_USB_DEVICE_SELECT_CONFIG_PARAMS_INIT_SINGLE_INTERFACE( &configParams);

    status = WdfUsbTargetDeviceSelectConfig(pDeviceContext->WdfUsbTargetDevice,
                                        WDF_NO_OBJECT_ATTRIBUTES,
                                        &configParams);


    if (NT_SUCCESS(status) &&
        WdfUsbTargetDeviceGetNumInterfaces(pDeviceContext->WdfUsbTargetDevice) > 0) {

        pDeviceContext->UsbInterface =
            configParams.Types.SingleInterface.ConfiguredUsbInterface;

        pDeviceContext->NumberConfiguredPipes =
            configParams.Types.SingleInterface.NumberConfiguredPipes;

    }

    return status;
}


NTSTATUS
AbortPipes(
    IN WDFDEVICE Device
    )
/*++

Routine Description

    sends an abort pipe request on all open pipes.

Arguments:

    Device - Handle to a framework device

Return Value:

    NT status value

--*/
{
    UCHAR              i;
    ULONG              count;
    NTSTATUS           status;
    PDEVICE_CONTEXT    pDevContext;

    PAGED_CODE();

    //
    // initialize variables
    //
    pDevContext = GetDeviceContext(Device);

    UsbSamp_DbgPrint(3, ("AbortPipes - begins\n"));

    count = pDevContext->NumberConfiguredPipes;

    for (i = 0; i < count; i++) {
        WDFUSBPIPE pipe;
        pipe = WdfUsbInterfaceGetConfiguredPipe(pDevContext->UsbInterface,
                                                i, //PipeIndex,
                                                NULL
                                                );

        UsbSamp_DbgPrint(3, ("Aborting open pipe %d\n", i));

        status = WdfUsbTargetPipeAbortSynchronously(pipe,
                                    WDF_NO_HANDLE, // WDFREQUEST
                                    NULL);//PWDF_REQUEST_SEND_OPTIONS

        if (!NT_SUCCESS(status)) {
            UsbSamp_DbgPrint(1, ("WdfUsbTargetPipeAbortSynchronously failed %x\n", status));
            break;
        }
    }

    UsbSamp_DbgPrint(3, ("AbortPipes - ends\n"));

    return STATUS_SUCCESS;
}

NTSTATUS
UsbSamp_ReadFdoRegistryKeyValue(
    __in  WDFDRIVER   Driver,
    __in LPWSTR      Name,
    __out PULONG      Value
    )
/*++

Routine Description:

    Can be used to read any REG_DWORD registry value stored
    under Device Parameter.

Arguments:

    Driver - pointer to the device object
    Name - Name of the registry value
    Value -


Return Value:

    NTSTATUS 

--*/
{
    WDFKEY      hKey = NULL;
    NTSTATUS    status;
    UNICODE_STRING  valueName;

    UNREFERENCED_PARAMETER(Driver);

    PAGED_CODE();

    *Value = 0;

    status = WdfDriverOpenParametersRegistryKey(WdfGetDriver(),
                                                KEY_READ,
                                                WDF_NO_OBJECT_ATTRIBUTES,
                                                &hKey);

    if (NT_SUCCESS (status)) {

        RtlInitUnicodeString(&valueName,Name);

        status = WdfRegistryQueryULong (hKey,
                                  &valueName,
                                  Value);

        WdfRegistryClose(hKey);
    }

    return status;
}

