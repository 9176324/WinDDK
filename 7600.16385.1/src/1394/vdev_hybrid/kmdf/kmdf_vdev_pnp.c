/*++

Copyright (c) Microsoft Corporation

Module Name:

    kmdf_vdev_pnp.c

Abstract

--*/

//#define INITGUID
#include "kmdf_vdev.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, kmdf1394_EvtDeviceAdd)
#pragma alloc_text (PAGE, kmdf1394_EvtPrepareHardware)
#pragma alloc_text (PAGE, kmdf1394_EvtReleaseHardware)
#pragma alloc_text (PAGE, kmdf1394_EvtDeviceD0Exit)
#pragma alloc_text (PAGE, DbgDevicePowerString)
#endif

NTSTATUS
kmdf1394_EvtDeviceAdd (
                       IN WDFDRIVER        Driver,
                       IN PWDFDEVICE_INIT  DeviceInit)
/*++
Routine Description:

    EvtDeviceAdd is called by the framework in response to AddDevice
    call from the PnP manager.

Arguments:

    Driver - Handle to a framework driver object created in DriverEntry

    DeviceInit - Pointer to a framework-allocated WDFDEVICE_INIT structure.

Return Value:

    NTSTATUS

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION deviceExtension;
    WDF_PNPPOWER_EVENT_CALLBACKS  pnpPowerCallbacks;
    WDF_OBJECT_ATTRIBUTES fdoAttributes,lockAttributes;
    WDFDEVICE device;
    WDF_DEVICE_PNP_CAPABILITIES pnpCaps;
    WDF_IO_QUEUE_CONFIG ioQueueConfig;

    UNREFERENCED_PARAMETER (Driver);

    PAGED_CODE();

    ENTER("kmdf1394_PnpAddDevice");

    //
    // Zero out the PnpPowerCallbacks structure.
    //
    WDF_PNPPOWER_EVENT_CALLBACKS_INIT(&pnpPowerCallbacks);

    //
    // Set Callbacks for any of the functions we are interested in.
    // If no callback is set, Framework will take the default action
    // by itself.

    //
    // These two callbacks set up and tear down hardware state,
    // specifically that which only has to be done once.
    //

    pnpPowerCallbacks.EvtDevicePrepareHardware = kmdf1394_EvtPrepareHardware;
    pnpPowerCallbacks.EvtDeviceReleaseHardware = kmdf1394_EvtReleaseHardware;

    pnpPowerCallbacks.EvtDeviceSelfManagedIoCleanup = \
        kmdf1394_EvtDeviceSelfManagedIoCleanup;

    pnpPowerCallbacks.EvtDeviceD0Entry = kmdf1394_EvtDeviceD0Entry;
    pnpPowerCallbacks.EvtDeviceD0Exit  = kmdf1394_EvtDeviceD0Exit;

    //
    // Register the PnP and power callbacks. Power policy related callbacks
    // will be registered// later in SotwareInit.
    //
    WdfDeviceInitSetPnpPowerEventCallbacks (DeviceInit, &pnpPowerCallbacks);
    if (!NT_SUCCESS (ntStatus))
    {
        TRACE (TL_ERROR, ("WdfDeviceInitSetPnpPowerEventCallbacks failed %x\n",
                         ntStatus));
        return ntStatus;
    }

    //
    // We'll allow multiple handles to be opened to this device driver
    // so we'll set exclusivity to FALSE.
    //
    WdfDeviceInitSetExclusive (DeviceInit, FALSE);

    //
    // Specify the size and type of device context.
    //
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE (&fdoAttributes, DEVICE_EXTENSION);

    ntStatus = WdfDeviceCreate (&DeviceInit, &fdoAttributes, &device);

    if (!NT_SUCCESS (ntStatus)) 
    {
        TRACE(TL_ERROR, ("WdfDeviceInitialize failed %x\n", ntStatus));
        return ntStatus;
    }


    deviceExtension = GetDeviceContext (device);
    deviceExtension->WdfDevice = device;

    TRACE (
        TL_TRACE, 
        ("PDO(0x%p) FDO(0x%p), Lower(0x%p) DevExt (0x%p)\n",
        WdfDeviceWdmGetPhysicalDevice (device),
        WdfDeviceWdmGetDeviceObject (device),
        WdfDeviceWdmGetAttachedDevice(device),
        deviceExtension));

    //
    // Tell the Framework that this device will need an interface so that
    // application can interact with it.
    //

    ntStatus = WdfDeviceCreateDeviceInterface (
        device,
        (LPGUID) &GUID_KMDF_VDEV,
        NULL);
    if (!NT_SUCCESS (ntStatus)) 
    {
        TRACE(TL_ERROR, ("WdfDeviceCreateDeviceInterface failed %x\n", ntStatus));
        return ntStatus;
    }

    //
    // Tell the framework to set the SurpriseRemovalOK in the DeviceCaps so
    // that you don't get the popup in usermode (on Win2K) when you surprise
    // remove the device.
    //
    WDF_DEVICE_PNP_CAPABILITIES_INIT (&pnpCaps);
    pnpCaps.SurpriseRemovalOK = WdfTrue;

    WdfDeviceSetPnpCapabilities (device, &pnpCaps);

    //
    // save the device object we created as our physical device object
    //
    deviceExtension->PhysicalDeviceObject = \
        WdfDeviceWdmGetPhysicalDevice (device);

    if (NULL == deviceExtension->PhysicalDeviceObject) 
    {
        TRACE(TL_ERROR, ("WdfDeviceWdmGetPhysicalDevice: NULL DeviceObject\n"));
        return STATUS_UNSUCCESSFUL;
    }

    //
    // This is our default IoTarget representing the deviceobject
    // we are attached to.
    //
    deviceExtension->StackIoTarget = WdfDeviceGetIoTarget(device);
    deviceExtension->StackDeviceObject = WdfDeviceWdmGetAttachedDevice(device);

    if (NULL == deviceExtension->StackDeviceObject) 
    {
        TRACE(TL_ERROR, ("WdfDeviceWdmGetAttachedDevice: NULL DeviceObject\n"));
        return STATUS_UNSUCCESSFUL;
    }


    //
    // Create a automanaged queue for dispatching ioctl requests.
    // All other requests are automatically failed by the framework.
    // By creating an automanaged queue we don't have to worry about
    // PNP/Power synchronization.
    // A default queue gets all the requests that are not
    // configure-fowarded using WdfDeviceConfigureRequestDispatching.
    //
    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE (
        &ioQueueConfig,
        WdfIoQueueDispatchParallel);

    ioQueueConfig.EvtIoDeviceControl = kmdf1394_EvtIoDeviceControl;

    ntStatus = WdfIoQueueCreate (
        deviceExtension->WdfDevice,
        &ioQueueConfig,
        WDF_NO_OBJECT_ATTRIBUTES,
        &deviceExtension->IoctlQueue);

    if (!NT_SUCCESS (ntStatus)) 
    {
         TRACE(TL_ERROR, ("WdfIoQueueCreate failed 0x%x\n", ntStatus));
        return ntStatus;
    }

    //
    // Create an additional queue to hold bus reset requests.
    //
    WDF_IO_QUEUE_CONFIG_INIT (&ioQueueConfig, WdfIoQueueDispatchManual);

    ntStatus = WdfIoQueueCreate (
        deviceExtension->WdfDevice,
        &ioQueueConfig,
        WDF_NO_OBJECT_ATTRIBUTES,
        &deviceExtension->BusResetRequestsQueue);
    if(!NT_SUCCESS (ntStatus))
    {
        TRACE (TL_ERROR, ("Error Creating Reset Request Queue 0x%x\n",
                         ntStatus));
        return ntStatus;
    }


    WDF_OBJECT_ATTRIBUTES_INIT (&lockAttributes);
    lockAttributes.ParentObject = device;

    ntStatus = WdfSpinLockCreate (&lockAttributes,&deviceExtension->CromSpinLock);
    if(!NT_SUCCESS (ntStatus))
    {
        TRACE (
            TL_ERROR, 
            ("WdfSpinLockCreate CromSpinLock "
            "failed 0x%x\n", ntStatus));
        return ntStatus;
    }


    WDF_OBJECT_ATTRIBUTES_INIT (&lockAttributes);
    lockAttributes.ParentObject = device;

    ntStatus = WdfSpinLockCreate (
        &lockAttributes,
        &deviceExtension->AsyncSpinLock);
    if(!NT_SUCCESS (ntStatus))
    {
        TRACE (TL_ERROR, ("WdfSpinLockCreate AsyncSpinLock "
            "failed 0x%x\n", ntStatus));
        return ntStatus;
    }

    WDF_OBJECT_ATTRIBUTES_INIT (&lockAttributes);
    lockAttributes.ParentObject = device;

    ntStatus = WdfSpinLockCreate (
        &lockAttributes,
        &deviceExtension->IsochSpinLock );
    if(!NT_SUCCESS (ntStatus))
    {
        TRACE (
            TL_ERROR, 
            ("WdfSpinLockCreate IsochSpinLock failed 0x%x\n", 
            ntStatus));
        return ntStatus;
    }

    WDF_OBJECT_ATTRIBUTES_INIT (&lockAttributes);
    lockAttributes.ParentObject = device;

    ntStatus = WdfSpinLockCreate (
        &lockAttributes,
        &deviceExtension->IsochResourceSpinLock);
    if(!NT_SUCCESS (ntStatus))
    {
        TRACE(
            TL_ERROR, 
            ("WdfSpinLockCreate IsochResourceSpinLock failed 0x%x\n", 
            ntStatus));
        return ntStatus;
    }

    InitializeListHead (&deviceExtension->CromData);
    InitializeListHead (&deviceExtension->AsyncAddressData);
    InitializeListHead (&deviceExtension->IsochDetachData);
    InitializeListHead (&deviceExtension->IsochResourceData);

    EXIT("kmdf1394_PnpAddDevice", ntStatus);

    return(ntStatus);
} // kmdf1394_PnpAddDevice

NTSTATUS
kmdf1394_EvtPrepareHardware (
                             WDFDEVICE      Device,
                             WDFCMRESLIST   Resources,
                             WDFCMRESLIST   ResourcesTranslated)
/*++

Routine Description:

    EvtDeviceStart event callback performs operations that are necessary
    to make the driver's device operational. The framework calls the driver's
    EvtDeviceStart callback when the PnP manager sends an IRP_MN_START_DEVICE
    request to the driver stack.

Arguments:

    Device - Handle to a framework device object.

Return Value:

    WDF status code

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;

    UNREFERENCED_PARAMETER (Resources);
    UNREFERENCED_PARAMETER (ResourcesTranslated);

    PAGED_CODE();

    TRACE (TL_TRACE, ( "--> kmdf1394_EvtPrepareHardware\n"));

    ntStatus = kmdf1394_BusResetNotification ( 
        Device,
        NULL,
        REGISTER_NOTIFICATION_ROUTINE );

    TRACE (TL_TRACE, ( "<-- kmdf1394_EvtPrepareHardware\n"));

    return ntStatus;
}

NTSTATUS
kmdf1394_EvtReleaseHardware (
                         IN  WDFDEVICE Device,
                         IN  WDFCMRESLIST ResourcesTranslated)
/*++

Routine Description:

    EvtDeviceReleaseHardware is called by the framework whenever the PnP manager
    is revoking ownership of our resources.  This may be in response to either
    IRP_MN_STOP_DEVICE or IRP_MN_REMOVE_DEVICE.  The callback is made before
    passing down the IRP to the lower driver.

    In this callback, do anything necessary to free those resources.

Arguments:

    Device - Handle to a framework device object.

Return Value:

    NTSTATUS - Failures will be logged, but not acted on.

--*/
{
    NTSTATUS ntStatus;

    UNREFERENCED_PARAMETER(ResourcesTranslated);

    PAGED_CODE();

    TRACE(TL_TRACE, ( "--> kmdf1394_EvtReleaseHardware\n"));

    ntStatus = kmdf1394_BusResetNotification (
        Device,
        NULL,
        DEREGISTER_NOTIFICATION_ROUTINE );

    TRACE (TL_TRACE, ( "<-- kmdf1394_EvtReleaseHardware\n"));

    return ntStatus;
}

NTSTATUS
kmdf1394_EvtDeviceD0Entry (
                       IN  WDFDEVICE Device,
                       IN  WDF_POWER_DEVICE_STATE PreviousState)
/*++

Routine Description:

   EvtDeviceD0Entry event callback must perform any operations that are
   necessary before the specified device is used.  It will be called every
   time the hardware needs to be (re-)initialized.  This includes after
   IRP_MN_START_DEVICE, IRP_MN_CANCEL_STOP_DEVICE, IRP_MN_CANCEL_REMOVE_DEVICE,
   IRP_MN_SET_POWER-D0.

   This function runs at PASSIVE_LEVEL, though it is generally not paged.  A
   driver can optionally make this function pageable if DO_POWER_PAGABLE is set.

   Even if DO_POWER_PAGABLE isn't set, this function still runs at
   PASSIVE_LEVEL.  In this case, though, the function absolutely must not do
   anything that will cause a page fault.

Arguments:

    Device - Handle to a framework device object.

    PreviousState - Device power state which the device was in most recently.
        If the device is being newly started, this will be
        PowerDeviceUnspecified.

Return Value:

    NTSTATUS

--*/
{
    NTSTATUS   ntStatus = STATUS_SUCCESS;

    UNREFERENCED_PARAMETER(PreviousState);

    TRACE (
        TL_TRACE, 
        ("-->kmdf1394_EvtDeviceD0Entry - coming from %s\n",
        DbgDevicePowerString(PreviousState)));

    ntStatus = kmdf1394_UpdateGenerationCount (Device);

    TRACE (TL_TRACE, ( "<--kmdf1394_EvtDeviceD0Entry\n"));

    return ntStatus;
}


NTSTATUS
kmdf1394_EvtDeviceD0Exit (
                      IN  WDFDEVICE Device,
                      IN  WDF_POWER_DEVICE_STATE TargetState)
/*++

Routine Description:

   EvtDeviceD0Exit event callback must perform any operations that are
   necessary before the specified device is moved out of the D0 state.  If the
   driver needs to save hardware state before the device is powered down, then
   that should be done here.

   This function runs at PASSIVE_LEVEL, though it is generally not paged.  A
   driver can optionally make this function pageable if DO_POWER_PAGABLE is set.

   Even if DO_POWER_PAGABLE isn't set, this function still runs at
   PASSIVE_LEVEL.  In this case, though, the function absolutely must not do
   anything that will cause a page fault.

Arguments:

    Device - Handle to a framework device object.

    TargetState - Device power state which the device will be put in once this
        callback is complete.

Return Value:

    NTSTATUS

--*/
{
    UNREFERENCED_PARAMETER(Device);
    UNREFERENCED_PARAMETER(TargetState);

    TRACE(
        TL_TRACE, 
        ("-->kmdf1394_EvtDeviceD0Exit - moving to %s\n",
        DbgDevicePowerString(TargetState)));

    PAGED_CODE();

    TRACE(TL_TRACE, ( "<--kmdf1394_EvtDeviceD0Exit\n"));

    return STATUS_SUCCESS;
}

VOID
kmdf1394_EvtDeviceSelfManagedIoCleanup (
                                        IN  WDFDEVICE Device)
/*++

Routine Description:

    EvtDeviceSelfManagedIoCleanup is called by the Framework when the device is
    being torn down, either in response to IRP_MN_REMOVE_DEVICE or
    IRP_MN_SURPRISE_REMOVE_DEVICE.  It will be called only once.  Its job is to
    stop all outstanding I/O in the driver that the Framework is not managing.

Arguments:

    Device - Handle to a framework device object.

Return Value:

    None

--*/
{
    PDEVICE_EXTENSION deviceExtension = NULL;
    PLIST_ENTRY listEntry = NULL;

    ENTER("kmdf1394_EvtDeviceSelfManagedIoCleanup");

    deviceExtension = GetDeviceContext(Device);

    TRACE(TL_WARNING, ("Removing KMDF1394VDEV.SYS.\n"));

    //
    // lets free up any crom data structs we've allocated...
    //
    WdfSpinLockAcquire (deviceExtension->CromSpinLock);

    while (!IsListEmpty (&deviceExtension->CromData))
    {
        PCROM_DATA      CromData;

        //
        // get struct off list
        //
        listEntry = RemoveHeadList (&deviceExtension->CromData);
        CromData = CONTAINING_RECORD (listEntry, CROM_DATA, CromList);

        //
        // need to free up everything associated with this allocate...
        //
        if (CromData)
        {
            if (CromData->Buffer)
            {
                ExFreePoolWithTag (CromData->Buffer, POOLTAG_KMDF_VDEV);
            }

            if (CromData->pMdl)
            {
                IoFreeMdl (CromData->pMdl);
            }
            //
            // we already checked CromData
            //
            ExFreePoolWithTag (CromData, POOLTAG_KMDF_VDEV);
        }
    }

    WdfSpinLockRelease (deviceExtension->CromSpinLock);

    //
    // lets free up any allocated addresses and deallocate all
    // memory associated with them...
    //
    WdfSpinLockAcquire (deviceExtension->AsyncSpinLock);

    while (!IsListEmpty (&deviceExtension->AsyncAddressData)) 
    {
        PASYNC_ADDRESS_DATA     AsyncAddressData;

        //
        // get struct off list
        //
        listEntry = RemoveHeadList (&deviceExtension->AsyncAddressData);

        AsyncAddressData = CONTAINING_RECORD (
            listEntry, 
            ASYNC_ADDRESS_DATA,
            AsyncAddressList);

        // 
        // need to free up everything associated with this allocate...
        //
        if (AsyncAddressData->pMdl)
        {
            IoFreeMdl (AsyncAddressData->pMdl);
        }

        if (AsyncAddressData->Buffer)
        {
            ExFreePoolWithTag(AsyncAddressData->Buffer, POOLTAG_KMDF_VDEV);
        }

        if (AsyncAddressData->AddressRange)
        {
            ExFreePoolWithTag(AsyncAddressData->AddressRange, POOLTAG_KMDF_VDEV);
        }

        ExFreePoolWithTag(AsyncAddressData, POOLTAG_KMDF_VDEV);
    }


    WdfSpinLockRelease(deviceExtension->AsyncSpinLock);

    //
    // TODO: Free up any attached isoch buffers when
    // we get the attach / detach code inserted again.
    //

    //
    // Remove any isoch resource data
    //
    WHILE (TRUE) 
    {
        WdfSpinLockAcquire(deviceExtension->IsochResourceSpinLock);

        if (!IsListEmpty(&deviceExtension->IsochResourceData)) 
        {
            PISOCH_RESOURCE_DATA    IsochResourceData = NULL;

            listEntry = RemoveHeadList(&deviceExtension->CromData);

            IsochResourceData = CONTAINING_RECORD (
                listEntry,
                ISOCH_RESOURCE_DATA,
                IsochResourceList);

            WdfSpinLockRelease(deviceExtension->IsochResourceSpinLock);

            TRACE(
                TL_TRACE, 
                ("Surprise Removal: IsochResourceData = 0x%x\n",
                IsochResourceData));

            if (IsochResourceData) 
            {
                PIRB pIrb;
                WDFREQUEST request;
                NTSTATUS ntStatus;

                TRACE(
                    TL_TRACE, 
                    ("Surprise Removal: Freeing hResource = 0x%x\n",
                    IsochResourceData->hResource));

                ntStatus = WdfRequestCreate (
                    WDF_NO_OBJECT_ATTRIBUTES,
                    deviceExtension->StackIoTarget,
                    &request);
                if (!NT_SUCCESS (ntStatus)) 
                {
                    TRACE (
                        TL_ERROR, 
                        ("Failed to allocate request %x\n", 
                        ntStatus));
                }
                else 
                {
                    pIrb = ExAllocatePoolWithTag(
                        NonPagedPool, 
                        sizeof(IRB), 
                        POOLTAG_KMDF_VDEV);
                    if (!pIrb) 
                    {
                        WdfObjectDelete(request);
                        TRACE(TL_ERROR, ("Failed to allocate pIrb!\n"));
                    }
                    else 
                    {
                        RtlZeroMemory (pIrb, sizeof (IRB));
                        pIrb->FunctionNumber = REQUEST_ISOCH_FREE_RESOURCES;
                        pIrb->Flags = 0;
                        pIrb->u.IsochFreeResources.hResource = \
                            IsochResourceData->hResource;

                        ntStatus = kmdf1394_SubmitIrpSynch (
                            deviceExtension->StackIoTarget, 
                            request, 
                            pIrb);
                        if (!NT_SUCCESS (ntStatus)) 
                        {
                            TRACE (
                                TL_ERROR, 
                                ("SubmitIrpSync failed = 0x%x\n", 
                                ntStatus));
                        }

                        ExFreePoolWithTag (pIrb, POOLTAG_KMDF_VDEV);
                        WdfObjectDelete (request);
                    } // else
                } // else
            } // if (IsochResourceData) 
        } //  if (!IsListEmpty(&deviceExtension->IsochResourceData)) 
        else 
        {
            WdfSpinLockRelease (deviceExtension->IsochResourceSpinLock);
            break;
        }
    }

    EXIT("kmdf1394_EvtDeviceSelfManagedIoCleanup", STATUS_SUCCESS);

} // kmdf1394_EvtDeviceSelfManagedIoCleanup

PCHAR
DbgDevicePowerString (
                     IN WDF_POWER_DEVICE_STATE Type)
/*++

Updated Routine Description:
    DbgDevicePowerString does not change in this stage of the function driver.

--*/
{
    PAGED_CODE();

    switch (Type) 
    {
    case WdfPowerDeviceInvalid:
        return "WdfPowerDeviceInvalid";
    case WdfPowerDeviceD0:
        return "WdfPowerDeviceD0";
    case PowerDeviceD1:
        return "WdfPowerDeviceD1";
    case WdfPowerDeviceD2:
        return "WdfPowerDeviceD2";
    case WdfPowerDeviceD3:
        return "WdfPowerDeviceD3";
    case WdfPowerDeviceD3Final:
        return "WdfPowerDeviceD3Final";
    case WdfPowerDevicePrepareForHibernation:
        return "WdfPowerDevicePrepareForHibernation";
    case WdfPowerDeviceMaximum:
        return "PowerDeviceMaximum";
    default:
        return "UnKnown Device Power State";
    }
}


