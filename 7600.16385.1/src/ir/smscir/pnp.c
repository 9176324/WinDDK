//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Copyright 2005 OSR, Open Systems Resources, Inc. All rights Reserved.
// 
// Module Name:
// 
//     pnp.c
// 
// Abstract:
//
//     Contains routines to handle PnP and power transitions
// 
// Author:
//
// Revision History:
//      
#include "smscir.h"

#ifdef WPP_TRACING
#include "pnp.tmh"
#endif

EVT_WDF_IO_QUEUE_IO_STOP SmscIrEvtIoQueueStop;
/*
VOID
SmscIrEvtIoQueueStop(
    IN WDFQUEUE Queue,
    IN WDFREQUEST Request,
    IN ULONG  ActionFlags
    );
*/


NTSTATUS
SmscIrEvtDeviceAdd(
    IN WDFDRIVER Driver,
    IN PWDFDEVICE_INIT DeviceInit
    ) {
/*++
Routine Description.

  This routine is called when the FDO for the SMSC
  IR device needs to be created

Arguments:

  Driver     - Our WDF Driver object (not used)
  
  DeviceInit - The WDFDEVICE_INIT structure that we will use to
                indicate the characteristics of our device

--*/
    NTSTATUS                         status;
    PSMSCIR_DATA                     deviceData;
    WDF_PNPPOWER_EVENT_CALLBACKS     pnpPowerCallbacks;
    WDF_OBJECT_ATTRIBUTES            fdoAttributes;
    WDFDEVICE                        device;
    WDF_IO_QUEUE_CONFIG              ioQueueConfig;
    WDF_FILEOBJECT_CONFIG            fileConfig;
    WDF_INTERRUPT_CONFIG             interruptConfig;
    WDF_POWER_POLICY_EVENT_CALLBACKS powerPolicyCallbacks;
    WDF_TIMER_CONFIG                 timerConfig;
    WDF_OBJECT_ATTRIBUTES            timerAttributes;

    SmscIrTracePrint(
            TRACE_LEVEL_VERBOSE,
            SMSCDBG_PNP_INFO,
            ("SmscIrEvtDeviceAdd: Entered"));


    //
    // Initialize all the properties specific to the device.
    // Framework has default values for the one that are not
    // set explicitly here.
    //
    WdfDeviceInitSetDeviceType(DeviceInit, FILE_DEVICE_SMSCIR);

    //
    // We want MDLs...
    //
    WdfDeviceInitSetIoType(DeviceInit, WdfDeviceIoDirect);

    //
    // Initialize the pnpPowerCallbacks structure.  Callback events for PNP
    // and Power are specified here.  If you don't supply any callbacks,    
    // the Framework will take appropriate default actions for an FDO
    //
    WDF_PNPPOWER_EVENT_CALLBACKS_INIT(&pnpPowerCallbacks);

    // 
    // We'll need to get our port resource in the prepare
    //  callback and return it in the release.
    //
    pnpPowerCallbacks.EvtDevicePrepareHardware 
                                        = SmscIrEvtDevicePrepareHardware;
    pnpPowerCallbacks.EvtDeviceReleaseHardware 
                                        = SmscIrEvtDeviceReleaseHardware;
    
    //
    // These two callbacks are called as we go in and out of the D0-working 
    //  state.
    //
    pnpPowerCallbacks.EvtDeviceD0Entry = SmscIrEvtDeviceD0Entry;
    pnpPowerCallbacks.EvtDeviceD0Exit  = SmscIrEvtDeviceD0Exit;    

    WdfDeviceInitSetPnpPowerEventCallbacks(DeviceInit, &pnpPowerCallbacks);


    WDF_POWER_POLICY_EVENT_CALLBACKS_INIT(&powerPolicyCallbacks);

    //
    // We might need to arm the device for wake, so provide
    //  an "arming for wake" callback
    //
    powerPolicyCallbacks.EvtDeviceArmWakeFromSx = SmscIrEvtDeviceWakeArm;


    //
    // Register the power policy callbacks.
    //
    WdfDeviceInitSetPowerPolicyEventCallbacks(DeviceInit, &powerPolicyCallbacks);

    WDF_OBJECT_ATTRIBUTES_INIT(&fdoAttributes);

    //
    // We deal with our synchronization
    //
    fdoAttributes.SynchronizationScope = WdfSynchronizationScopeNone;

    //
    // Specify size and accessor function for storing device context.
    //
    WDF_OBJECT_ATTRIBUTES_SET_CONTEXT_TYPE(&fdoAttributes, SMSCIR_DATA);

    //
    // We want to specify an open and close routine so we know 
    //  when IRCLASS is trying to open our device...
    //
    WDF_FILEOBJECT_CONFIG_INIT(&fileConfig,
                               SmscIrEvtDeviceFileCreate,
                               SmscIrEvtFileClose,
                               NULL);

    WdfDeviceInitSetFileObjectConfig(DeviceInit,
                                     &fileConfig,
                                     NULL);


    //
    // Create a framework device object. In response to this call, framework
    // creates a WDM deviceobject and attaches it to the PDO.
    //
    status = WdfDeviceCreate(&DeviceInit, 
                             &fdoAttributes, 
                             &device);

    if (!NT_SUCCESS(status)) {
        SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_PNP_INFO,
            ("SmscIrEvtDeviceAdd: WdfDeviceCreate failed - 0x%x", status));

        return status;
    }


    //
    // Initialize our context area....
    //
    deviceData = (PSMSCIR_DATA)GetIrData(device);

    //
    // Keep a pointer to the driver
    //
    deviceData->WdfDriverObject = Driver;

    //
    // Set the default number of FFs we need to see to signal
    //  a data end
    //
    deviceData->NumFFsForDataEnd = SMSCIR_NUM_FFS_FOR_DATA_END;


    //
    // Initialize the basic portions of our receivers
    //
    SMSCIR_INITIALIZE_RECEIVER(&deviceData->Receiver, 
                               deviceData, 
                               FALSE);
    SMSCIR_INITIALIZE_RECEIVER(&deviceData->PriorityReceiver, 
                               deviceData, 
                               TRUE);

    KeInitializeSpinLock(&deviceData->TxFifoDataLock);
    InitializeListHead(&deviceData->TxFifoDataList);

    //
    // Create our deadman timer
    //
    WDF_TIMER_CONFIG_INIT_PERIODIC(&timerConfig, 
                                   SmscIrEvtDeadManTimerForTransmit,
                                   SMSCIR_DEADMAN_TIMER_PERIOD);

    WDF_OBJECT_ATTRIBUTES_INIT(&timerAttributes);

    //
    // Set the device as the parent of the timer so
    //  that the timer will go away when the 
    //  device does
    //
    timerAttributes.ParentObject = device;

    status = WdfTimerCreate(&timerConfig,
                            &timerAttributes,
                            &deviceData->DeadManTimer);

    if (!NT_SUCCESS(status)) {
        SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_PNP_INFO,
            ("SmscIrEvtDeviceAdd: Error creating timer - 0x%x", status));
        return status;
    }                            


    //
    // Create our interrupt object.
    //
    WDF_INTERRUPT_CONFIG_INIT(&interruptConfig,
                              SmscIrEvtInterruptIsr,
                              SmscIrEvtInterruptDpc);

    interruptConfig.EvtInterruptEnable  = SmscIrEvtInterruptEnable;
    interruptConfig.EvtInterruptDisable = SmscIrEvtInterruptDisable;

    status = WdfInterruptCreate(device,
                                &interruptConfig,
                                WDF_NO_OBJECT_ATTRIBUTES,
                                &deviceData->Interrupt);

    if( !NT_SUCCESS(status) ) {
        SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_PNP_INFO,
            ("SmscIrEvtDeviceAdd: Error creating interrupt - 0x%x", status));
        return status;
    }

    //
    // Create a parallel default queue
    //
    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&ioQueueConfig,
                                           WdfIoQueueDispatchParallel);

    ioQueueConfig.EvtIoDeviceControl = SmscIrEvtIoDeviceControl;

    //
    // And we need a stop callback so that we can kill the current
    //  IR receive request
    //
    ioQueueConfig.EvtIoStop          = SmscIrEvtIoQueueStop;

    status = WdfIoQueueCreate(device,
                              &ioQueueConfig,
                              WDF_NO_OBJECT_ATTRIBUTES,
                              NULL);

    if (!NT_SUCCESS(status)) {
        SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_PNP_INFO,
            ("SmscIrEvtDeviceAdd: WdfIoQueueCreate failed - 0x%x", status));
        return status;
    }


    //
    // Serial queue for things that we want to process 
    //  exclusive to each other.
    //
    WDF_IO_QUEUE_CONFIG_INIT(&ioQueueConfig,
                             WdfIoQueueDispatchSequential);

    ioQueueConfig.EvtIoDeviceControl = SmscIrEvtIoDeviceControlSerial;
    ioQueueConfig.EvtIoStop          = SmscIrEvtIoQueueStop;
    status = WdfIoQueueCreate(device,
                              &ioQueueConfig,
                              WDF_NO_OBJECT_ATTRIBUTES,
                              &deviceData->SerialQueue);

    if(!NT_SUCCESS (status)){
        SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_PNP_INFO,
            ("SmscIrEvtDeviceAdd: Creating IOCTL queue - 0x%x", status));
        return status;
    }


    //
    // Manual internal queue for regular receives reqeusts. 
    //  This will be used to queue the recieve requests presented 
    //  to us from the parallel default queue
    //
    WDF_IO_QUEUE_CONFIG_INIT(&ioQueueConfig,
                             WdfIoQueueDispatchManual);


    ioQueueConfig.EvtIoStop = SmscIrEvtIoQueueStop;

    status = WdfIoQueueCreate(device,
                              &ioQueueConfig,
                              WDF_NO_OBJECT_ATTRIBUTES,
                              &deviceData->Receiver.PendingReceiveQueue);

    if(!NT_SUCCESS (status)){
        SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_PNP_INFO,
            ("SmscIrEvtDeviceAdd: Error creating read queue - 0x%x", status));
        return status;
    }


    //
    // One more queue...We need a place to put the priority receive 
    //  requests that we get from the parallel queue.
    //
    WDF_IO_QUEUE_CONFIG_INIT(&ioQueueConfig,
                             WdfIoQueueDispatchManual);

    ioQueueConfig.EvtIoStop = SmscIrEvtIoQueueStop;

    status = WdfIoQueueCreate(
                            device,
                            &ioQueueConfig,
                            WDF_NO_OBJECT_ATTRIBUTES,
                            &deviceData->PriorityReceiver.PendingReceiveQueue);

    if(!NT_SUCCESS (status)){
        SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_PNP_INFO,
            ("SmscIrEvtDeviceAdd: Error creating priority queue - 0x%x", 
                status));
        return status;
    }

    //
    // Register a device interface. IRCLASS will be notified of our
    //  presence when WDF enables the interface and will then create
    //  a HID PDO for our device.
    //
    status = WdfDeviceCreateDeviceInterface(device,
                                            (LPGUID)&GUID_DEVINTERFACE_IRPORT,
                                            NULL);

    if (!NT_SUCCESS(status)) {

        SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_PNP_INFO,
            ("SmscIrEvtDeviceAdd: WdfDeviceCreateDeviceInterface "\
             "failed - 0x%x", status));

        return status;
    }

    //
    // Setup the wake key
    //
    SmscIrSetWakeKeyInfo(deviceData);

    SmscIrTracePrint(
            TRACE_LEVEL_VERBOSE,
            SMSCDBG_PNP_INFO,
            ("SmscIrEvtDeviceAdd: Exit"));

    return STATUS_SUCCESS;
}

VOID
SmscIrEvtIoQueueStop(
    IN WDFQUEUE Queue,
    IN WDFREQUEST Request,
    IN ULONG  ActionFlags
    ) {
/*++
Routine Description.

  This routine is called when a queue of ours needs to 
  pause, notably due to power down.

Arguments:

  Queue   - The WDFQUEUE that is stopping

  Request - We're called here multiple times, once 
            for each request on the queue. This is one
            such request

  ActionFlags - More detailed info about the stop.
  
--*/

    PSMSCIR_DATA  deviceData;
    KIRQL         oldIrql;

    SmscIrTracePrint(
            TRACE_LEVEL_VERBOSE,
            SMSCDBG_PNP_INFO,
            ("SmscIrEvtIoQueueStop: Stopping request 0x%p - 0x%x", 
                Request, ActionFlags));

    deviceData = (PSMSCIR_DATA)GetIrData(WdfIoQueueGetDevice(Queue));

    KeAcquireSpinLock(&deviceData->Receiver.ListSpinLock, &oldIrql);

    if (deviceData->Receiver.CurrentIrReceiveRequest == Request) {

        //
        // Do not requeue the request, just ACK and leave it where it is
        //
        WdfRequestStopAcknowledge(Request, FALSE);

    }
   
    KeReleaseSpinLock(&deviceData->Receiver.ListSpinLock, oldIrql);

    KeAcquireSpinLock(&deviceData->PriorityReceiver.ListSpinLock, &oldIrql);

    if (deviceData->PriorityReceiver.CurrentIrReceiveRequest == Request) {

        //
        // Do not requeue the request, just ACK and leave it where it is
        //
        WdfRequestStopAcknowledge(Request, FALSE);

    }
    
    KeReleaseSpinLock(&deviceData->PriorityReceiver.ListSpinLock, oldIrql);
    return;
}

NTSTATUS
SmscIrEvtDevicePrepareHardware(
    IN WDFDEVICE    Device,
    IN WDFCMRESLIST ResourceList,
    IN WDFCMRESLIST ResourceListTranslated
    ) {
/*++

Routine Description:

   This entry point is called when hardware resources are assigned to one of
   our devices.

  Arguments:

      Device               -  Handle to our WDFDEVICE object

      Resources            -  WDFCMRESLIST of hardware resources assigned
                              to our device.

      ResourcesTranslated  -  A WDFCMRESLIST of hardware resources
                              assigned to our device, made usable.
Return Value:

    NTSTATUS

--*/    
    WDF_DEVICE_POWER_POLICY_WAKE_SETTINGS wakeSettings;
    PSMSCIR_DATA                          deviceData;
    NTSTATUS                              status;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR       resourceTrans;
    ULONG                                 i;

    UNREFERENCED_PARAMETER(ResourceList);

    deviceData = (PSMSCIR_DATA)GetIrData(Device);

    SmscIrTracePrint(
        TRACE_LEVEL_VERBOSE,
        SMSCDBG_PNP_INFO,
        ("SmscIrEvtDevicePrepareHardware: Entered"));

    for (i=0; i < WdfCmResourceListGetCount(ResourceListTranslated); i++) {
               
        //
        // Get the i'th partial resource descriptor from the list
        //
        resourceTrans = WdfCmResourceListGetDescriptor(ResourceListTranslated, i);

        if(!resourceTrans){
            SmscIrTracePrint(
                TRACE_LEVEL_ERROR,
                SMSCDBG_PNP_INFO,
                ("SmscIrEvtDevicePrepareHardware: NULL resource returned??"));
            return(STATUS_DEVICE_CONFIGURATION_ERROR);
        }

        //
        // Let's examine and store the resources, based on their type
        //
        // We expect to get a port (that we'll use to talk to the device)
        // and an interrupt (for, ah, interrupts).
        //
        switch (resourceTrans->Type) {

        case CmResourceTypePort:
            
            SmscIrTracePrint(
                TRACE_LEVEL_ERROR,
                SMSCDBG_PNP_INFO,
                ("SmscIrEvtDevicePrepareHardware: "\
                 "Resource %d: Port 0x%0x, Length %d.",
                    i,
                    resourceTrans->u.Port.Start.LowPart,
                    resourceTrans->u.Port.Length));
            //
            // We have two port or memory resources, one for Rx and 
            //  one for Tx
            //
            if (!deviceData->BaseAddress) {

                //
                // First time through, gotta be the Rx port
                //

                //
                // Should be 8 bytes long...
                //
                ASSERT(resourceTrans->u.Port.Length == 8);

                deviceData->BaseAddress = 
                      (PUCHAR)UlongToPtr(resourceTrans->u.Port.Start.LowPart); 

                //
                // Indicate that the port is, indeed, in "port space" on this
                // machine. PortIsMapped is used by our READ_HARDWARE_UCHAR and
                // WRITE_HARDWARE_UCHAR functions to determine which HAL
                // function to call to do talk to the device.
                //
                deviceData->PortIsMapped = FALSE;

            } else {

                //
                // Second time through...Gotta be the Tx port
                //

                ASSERT(resourceTrans->u.Port.Length == 8);

                deviceData->TransmitBaseAddress =
                        (PUCHAR)UlongToPtr(resourceTrans->u.Port.Start.LowPart); 

                deviceData->TransmitPortIsMapped = FALSE;

            }
            break;

        case CmResourceTypeMemory:

            SmscIrTracePrint(
                TRACE_LEVEL_ERROR,
                SMSCDBG_PNP_INFO,
                ("SmscIrEvtDevicePrepareHardware: "\
                 "Resource %d: Register 0x%0x:%0x Length %d.",
                                i,
                                resourceTrans->u.Memory.Start.HighPart, 
                                resourceTrans->u.Memory.Start.LowPart, 
                                resourceTrans->u.Memory.Length));
            //
            // We're running on a system where our PORTS appear in memory 
            //  space! It's rare, but it's certainly possible.
            //
            // What this means is that we have to (a) Map the memory
            // range into kernel virtual address space, (b) remember to
            // unmap this range later, and (c) write to these registers
            // using the HAL "register" functions and not the "port"
            // functions.
            //
            //
            if (!deviceData->BaseAddress) {

                ASSERT(resourceTrans->u.Port.Length == 8);

                //
                // Map the memory-mapped "port" into kernel virtual address
                // space...
                //
                deviceData->BaseAddress =  
                          (PUCHAR)MmMapIoSpace(resourceTrans->u.Memory.Start,
                                               resourceTrans->u.Memory.Length,
                                               MmNonCached);

                //
                // Indicate that the port is in "memory space" on this
                // machine. PortIsMapped is used by our READ_HARDWARE_UCHAR
                // and WRITE_HARDWARE_UCHAR functions to determine which HAL
                // function to call to do talk to the device.
                //
                deviceData->PortIsMapped = TRUE;


            } else {

                deviceData->TransmitBaseAddress =  
                          (PUCHAR)MmMapIoSpace(resourceTrans->u.Memory.Start,
                                               resourceTrans->u.Memory.Length,
                                               MmNonCached);

                deviceData->TransmitPortIsMapped = TRUE;
            }

            break;

        //
        // We'll need to get an interrupt resource, but note that we don't
        // have to DO anything with it.  The Framework will actually remember
        // what our interrupt resource is, and connect it for us using the
        // WDFINTERRUPT that we created in EvtDeviceAdd.
        //
        case CmResourceTypeInterrupt:

            SmscIrTracePrint(
                TRACE_LEVEL_ERROR,
                SMSCDBG_PNP_INFO,
                ("SmscIrEvtDevicePrepareHardware: "\
                 "Resource %d: Interrupt level: 0x%0x, Vector: 0x%0x",
                    i,
                    resourceTrans->u.Interrupt.Level,
                    resourceTrans->u.Interrupt.Vector));
            break;

        default:

            //
            // This could be device-private type added by the PCI bus driver. 
            //  We shouldn't filter this or change the information contained 
            //  in it.
            //
            SmscIrTracePrint(
                TRACE_LEVEL_ERROR,
                SMSCDBG_PNP_INFO,
                ("SmscIrEvtDevicePrepareHardware: "\
                 "Resource %d: Unhandled resource type 0x%0x", 
                     i,
                     resourceTrans->Type));
            break;

        }   // end of switch

    }  // end of for


    
    ASSERT(deviceData->BaseAddress);
    ASSERT(deviceData->TransmitBaseAddress);

    //
    // Init wait-wake policy structure. We assign default
    //  waking capability for the device.
    //
    WDF_DEVICE_POWER_POLICY_WAKE_SETTINGS_INIT(&wakeSettings);

    status = WdfDeviceAssignSxWakeSettings(Device, &wakeSettings);

    if (!NT_SUCCESS(status)) {
        SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
             SMSCDBG_PNP_INFO,
            ("SmscIrEvtDevicePrepareHardware: "\
             "WdfDeviceAssignSxWakeSettings failed with Status code 0x%x", 
                status));
        return status;
    }

    SmscIrTracePrint(
        TRACE_LEVEL_VERBOSE,
        SMSCDBG_PNP_INFO,
        ("SmscIrEvtDevicePrepareHardware: Exit"));

    return STATUS_SUCCESS;

}



NTSTATUS
SmscIrEvtDeviceReleaseHardware(
    IN WDFDEVICE    Device,
    IN WDFCMRESLIST ResourceListTranslated
    ) {
/*++

Routine Description:

    This function is called any time we need to release our hardware resources.
    In WDM terms

  Arguments:

    Device              - The usual handle to our WDFDEVICE

    ResourcesTranslated - The resources we're giving up, in case we don't
                          remember what they are, I suppose.

Return Value:

    NTSTATUS

--*/    
    
    PSMSCIR_DATA deviceData;

    UNREFERENCED_PARAMETER(ResourceListTranslated);

    SmscIrTracePrint(
        TRACE_LEVEL_VERBOSE,
        SMSCDBG_PNP_INFO,
        ("SmscIrEvtDevicePrepareHardware: Entered"));
    
    deviceData = (PSMSCIR_DATA)GetIrData(Device);

    //
    // If we had mapped any shared-memory into kernel virtual address space
    // during EvtPrepareHardware, we'd unmap it here. The only time for THIS
    // hardware that this would be necessary is in the unlikely event that our 
    // PORTs were located in memory space on a machine.
    //
    if (deviceData->PortIsMapped)  {
        MmUnmapIoSpace(deviceData->BaseAddress, 8);
    }
    if (deviceData->TransmitPortIsMapped)  {
        MmUnmapIoSpace(deviceData->TransmitBaseAddress, 8);
    }


    //
    // Reset the context locations we use to track info about our device's
    // register.
    //
    deviceData->BaseAddress = NULL;
    deviceData->PortIsMapped = FALSE;

    SmscIrTracePrint(
        TRACE_LEVEL_VERBOSE,
        SMSCDBG_PNP_INFO,
        ("SmscIrEvtDevicePrepareHardware: Exit"));

    return STATUS_SUCCESS;
}

NTSTATUS
SmscIrEvtDeviceD0Entry(
    IN  WDFDEVICE Device,
    IN  WDF_POWER_DEVICE_STATE PreviousState
    )
/*++

Routine Description:

   EvtDeviceD0Entry event callback must perform any operations that are
   necessary before the specified device is used.  It will be called every
   time the hardware needs to be (re-)initialized. 

Arguments:

    Device - Handle to a framework device object.

    PreviousState - Device power state which the device was in most recently.
        If the device is being newly started, this will be
        PowerDeviceUnspecified.

Return Value:

    NTSTATUS

--*/
{
    PSMSCIR_DATA deviceData;

    deviceData = GetIrData(Device);

    SmscIrTracePrint(
        TRACE_LEVEL_VERBOSE,
        SMSCDBG_PNP_INFO,
        ("SmscIrEvtDeviceD0Entry: Device is powering up from %d",
            PreviousState));


    if (SmscIrDeviceWokeSystem(deviceData)) {

        SmscIrTracePrint(
            TRACE_LEVEL_VERBOSE,
            SMSCDBG_PNP_INFO,
            ("SmscIrEvtDeviceD0Entry: Device used to wake system. Marking "\
             "user as present"));

        PoSetSystemState(ES_USER_PRESENT);

    } else {

        SmscIrTracePrint(
            TRACE_LEVEL_VERBOSE,
            SMSCDBG_PNP_INFO,
            ("SmscIrEvtDeviceD0Entry: Device was NOT used to wake system"));

    }

    //
    // Restart the queues.
    //
    WdfIoQueueStart(deviceData->PriorityReceiver.PendingReceiveQueue);
    WdfIoQueueStart(deviceData->Receiver.PendingReceiveQueue);

    return STATUS_SUCCESS;

}


NTSTATUS
SmscIrEvtDeviceD0Exit(
    IN  WDFDEVICE Device,
    IN  WDF_POWER_DEVICE_STATE TargetState
    )
/*++

Routine Description:

    This routine undoes anything done in EvtDeviceD0Entry.  It is called
    whenever the device leaves the D0 state, which happens when the device is
    stopped, when it is removed, and when it is powered off.

    The device is still in D0 when this callback is invoked, which means that
    the driver can still touch hardware in this routine.


    EvtDeviceD0Exit event callback must perform any operations that are
    necessary before the specified device is moved out of the D0 state.  If the
    driver needs to save hardware state before the device is powered down, then
    that should be done here.

Arguments:

    Device - Handle to a framework device object.

    TargetState - Device power state which the device will be put in once this
        callback is complete.

Return Value:

    Success implies that the device can be used.  Failure will result in the
    device stack being torn down.

--*/
{

    UNREFERENCED_PARAMETER(Device);

    SmscIrTracePrint(
        TRACE_LEVEL_VERBOSE,
        SMSCDBG_PNP_INFO,
        ("SmscIrEvtDeviceD0Exit: Device is powering down to 0x%x",
           TargetState));

    return STATUS_SUCCESS;
}




