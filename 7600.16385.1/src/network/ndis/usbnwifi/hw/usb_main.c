/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Hw_usb_main.c

Abstract:
    Bus dependant layer USB specific function
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    01-01-2006    Created

Notes:

--*/

#include "hw_pcomp.h"

#include <stdio.h>
#pragma warning(disable:4200)  // nameless struct/union
#pragma warning(disable:4201)  // nameless struct/union
#pragma warning(disable:4214)  // bit field types other than int

#include "hw_main.h"
#include "hw_nic.h"
#include "usb_recv.h"

#pragma warning(default:4200)
#pragma warning(default:4201)
#pragma warning(default:4214)
#pragma warning(disable:4748)

#include "usb_main.h"

VOID
FreeControlRequests(
    PNIC    Nic
    );

NTSTATUS
AllocateControlRequests(
    PNIC    Nic,
    BYTE    MaxOutstanding
    );


NDIS_STATUS
HwUSBSpecificInit(
    __in PNIC         Nic
    )
{
    NTSTATUS ntStatus;
    NDIS_STATUS ndisStatus;

    ntStatus =  HwUsbCreateInitializeUsbDevice(Nic);
    NT_STATUS_TO_NDIS_STATUS(ntStatus, &ndisStatus);
    //
    // Set the hardware id here before HwInitializeVariable as this gets used there. 
    //
    HwSetHardwareID(Nic);
    return    ndisStatus;
}

VOID
HwUsbDeInit(
    __in PNIC Nic
    )
/*++

Routine Description:

    Restricted to PASSIVE_LEVEL due to WdfIoTargetCancelSentIo action
    This can be called from MPHalt.

Arguments:

    Nic - Pointer to the HW NIC structure. 

Return Value:

    NT status value

--*/

{
    //
    // WdfIoTargetStop will cancel all the outstanding I/O and wait
    // for them to complete before returning. WdfIoTargetStop  with the
    //  action type WdfIoTargetCancelSentIo can be called at IRQL PASSIVE_LEVEL only.
    //
    FreeControlRequests(Nic);
}


NTSTATUS
HwUsbCreateInitializeUsbDevice(
    __in PNIC         Nic
    )
/*++

Routine Description:

    This helper routine selects the configuration, interface and
    creates a context for every pipe (end point) in that interface.

Arguments:

    Nic - Pointer to the HW NIC structure. 

Return Value:

    NT status value

--*/
{
    WDF_USB_DEVICE_SELECT_CONFIG_PARAMS configParams;
    NTSTATUS                            ntStatus;
    WDFUSBPIPE                          pipe;
    WDF_USB_PIPE_INFORMATION            pipeInfo;
    UCHAR                               index;
    UCHAR                               numberConfiguredPipes;
    PUSB_DEVICE_CONTEXT                 usbDeviceContext;
    WDF_OBJECT_ATTRIBUTES               attributes;
    WDFDEVICE                           WdfDevice;

    //
    // Create a USB device handle so that we can communicate with the
    // underlying USB stack. The WDFUSBDEVICE handle is used to query,
    // configure, and manage all aspects of the USB device.
    // These aspects include device properties, bus properties,
    // and I/O creation and synchronization.
    //
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes, USB_DEVICE_CONTEXT);
    WdfDevice = Mp11GetWdfDevice( Nic->Adapter );
    ntStatus = WdfUsbTargetDeviceCreate(WdfDevice,
                                &attributes,
                                &Nic->UsbDevice);
    
    if (!NT_SUCCESS(ntStatus)) {
        MpTrace(COMP_INIT_PNP ,DBG_SERIOUS ,
             ("WdfUsbTargetDeviceCreate failed with Status code %!STATUS!\n", ntStatus));
        return ntStatus;
    }

    WDF_USB_DEVICE_SELECT_CONFIG_PARAMS_INIT_SINGLE_INTERFACE( &configParams);
    ntStatus = WdfUsbTargetDeviceSelectConfig(Nic->UsbDevice,
                                        WDF_NO_OBJECT_ATTRIBUTES,
                                        &configParams);
    
    if(!NT_SUCCESS(ntStatus)) {
        MpTrace(COMP_INIT_PNP ,DBG_SERIOUS ,
                ("WdfUsbTargetDeviceSelectConfig failed %!STATUS!\n",
                ntStatus));
        return ntStatus;
    }
    
    usbDeviceContext = GetUsbDeviceContext(Nic->UsbDevice);
    usbDeviceContext->UsbInterface =
                configParams.Types.SingleInterface.ConfiguredUsbInterface;
    numberConfiguredPipes = configParams.Types.SingleInterface.NumberConfiguredPipes;

    if (numberConfiguredPipes > MAX_PIPES) {
        numberConfiguredPipes = MAX_PIPES;
    }
    
    usbDeviceContext->NumberOfPipes = numberConfiguredPipes;
    //
    // If we didn't find the 3 pipes, fail the start.
    //

    if(numberConfiguredPipes < 3) {
        ntStatus = STATUS_INVALID_DEVICE_STATE;
        MpTrace(COMP_INIT_PNP ,DBG_SERIOUS ,
                            ("Device is not configured properly %!STATUS!\n",
                            ntStatus));
        return ntStatus;

    }

    //
    // Get pipe handles
    //
    for(index=0; index < numberConfiguredPipes; index++) {

        WDF_USB_PIPE_INFORMATION_INIT(&pipeInfo);

        pipe = WdfUsbInterfaceGetConfiguredPipe(usbDeviceContext->UsbInterface,
                                                index, //PipeIndex,
                                                &pipeInfo
                                                );
        //
        // Tell the framework that it's okay to read less than
        // MaximumPacketSize
        //
        WdfUsbTargetPipeSetNoMaximumPacketSizeCheck(pipe);

        if(WdfUsbPipeTypeBulk == pipeInfo.PipeType ) {
            MpTrace(COMP_OID , DBG_LOUD,
                   ("BulkInput Pipe is 0x%p direction : %s\n", 
                    pipe,
                    WdfUsbTargetPipeIsInEndpoint(pipe) ? "__in" : "__out"));
            
            if (WdfUsbTargetPipeIsInEndpoint(pipe)) {
                usbDeviceContext->InputPipe = pipe;
            } 
            else {
                //
                // The out pipe normal is the first out end point so if we haven't found any then 
                //  it is the first one. 
                // 
                if (usbDeviceContext->OutputPipeNormal == NULL) {
                   usbDeviceContext->OutputPipeNormal = pipe;
                }
                else {
                    usbDeviceContext->OutputPipeHigh = pipe;
                }
            }

        }
        else{
           MpTrace(COMP_OID, DBG_LOUD ,
                   (" Pipe is 0x%p type %x\n", pipe, pipeInfo.PipeType));
        }

    }
   
    WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
    attributes.ParentObject = Mp11GetWdfDevice( Nic->Adapter );

    ntStatus = WdfWaitLockCreate(&attributes, &usbDeviceContext->PipeStateLock);
    if (!NT_SUCCESS(ntStatus)) {
        MpTrace(COMP_INIT_PNP ,DBG_SERIOUS ,
                            ("Couldn't create PipeStateLock %!STATUS!\n",
                            ntStatus));
        return ntStatus;

    }
    ntStatus = AllocateControlRequests(Nic,  MAX_CONTROL_REQUESTS );
    return ntStatus;
}


VOID
HwUsbStopAllPipes(
    __in PNIC         Nic
    )
{
    PUSB_DEVICE_CONTEXT     pUsbDeviceContext;

    pUsbDeviceContext = GetUsbDeviceContext(Nic->UsbDevice);
    //
    // stop the in endpoint first
    ///
    HwUsbRecvStop(Nic);

    WdfWaitLockAcquire(pUsbDeviceContext->PipeStateLock, NULL);
    WdfIoTargetStop(WdfUsbTargetPipeGetIoTarget(pUsbDeviceContext->OutputPipeNormal),
                                                    WdfIoTargetCancelSentIo);
    WdfIoTargetStop(WdfUsbTargetPipeGetIoTarget(pUsbDeviceContext->OutputPipeHigh),
                                                WdfIoTargetCancelSentIo);

    WdfWaitLockRelease(pUsbDeviceContext->PipeStateLock);
    
}

VOID
HwSetHardwareID(
    __in  PNIC Nic
    )
{
    Nic->CardInfo.HardwareID = HW_ID_8187;
}




NTSTATUS
AllocateControlRequests(
    PNIC    Nic,
    BYTE    MaxOutstanding
    )

/*++

Routine Description

     Do this for low/meduim q and for high q
    Create write requests,
    Allocate         URB Memory
    Allocate         WDFMEMORY objects
    Setup the assocition between them in the Request context.

Arguments:
    Nic - Pointer to the HW NIC structure. 
    MaxOutstanding -- Number of Control requests to allocate.
    
Return Value:

    
--*/

{
    WDF_OBJECT_ATTRIBUTES   requestAttributes;
    UCHAR                   requestIndex ;
    NTSTATUS                ntStatus;
    WDF_OBJECT_ATTRIBUTES   objectAttribs;
    PUSB_DEVICE_CONTEXT     pUsbDeviceContext;
    PUSB_CONTROL_RESOURCES  controlResources;
    WDFIOTARGET             ioTarget;

    ntStatus = STATUS_SUCCESS;
    pUsbDeviceContext = GetUsbDeviceContext(Nic->UsbDevice);

    ioTarget = WdfUsbTargetDeviceGetIoTarget(Nic->UsbDevice);
    controlResources = &pUsbDeviceContext->ControlResources;

    WDF_OBJECT_ATTRIBUTES_INIT(&objectAttribs);  
    objectAttribs.ParentObject = Mp11GetWdfDevice( Nic->Adapter );
    ntStatus = WdfSpinLockCreate(&objectAttribs, &controlResources->RequestArrayLock);

    if (!NT_SUCCESS(ntStatus)) {
        MpTrace(COMP_SEND, DBG_SERIOUS,("Could not create request lock: status(0x%08X)", ntStatus));
        return ntStatus;
    }

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&requestAttributes, USB_CONTROL_REQ_CONTEXT);
    requestAttributes.ParentObject = Mp11GetWdfDevice( Nic->Adapter );

    for (requestIndex = 0; requestIndex < MaxOutstanding; requestIndex++)
    {
        WDFREQUEST* pNextRequest;
        PUSB_CONTROL_REQ_CONTEXT vendorRequestContext;
        WDF_USB_CONTROL_SETUP_PACKET  setupPacket;

        pNextRequest = &controlResources->RequestArray[requestIndex];

        ntStatus = WdfRequestCreate(&requestAttributes, ioTarget, pNextRequest);
        
        if (!NT_SUCCESS(ntStatus)) {
            MpTrace(COMP_SEND, DBG_SERIOUS,("Could not create request: status(0x%08X)", ntStatus));
            controlResources->MaxOutstandingRequests = requestIndex;
            controlResources->NextAvailableRequestIndex = requestIndex;
            return ntStatus;
        }
        //
        // By doing this we allocate resources like the IRP upfront eventhough 
        // we may not have all the data yer. 
        //
        ntStatus = WdfUsbTargetDeviceFormatRequestForControlTransfer(
                                                                     Nic->UsbDevice,
                                                                     *pNextRequest,
                                                                     &setupPacket,
                                                                     NULL,
                                                                     NULL);
        
        if(!NT_SUCCESS(ntStatus)) {
            MpTrace (COMP_SEND, DBG_SERIOUS, (
                     "WdfUsbTargetDeviceFormatRequestForControlTransfer: Failed - 0x%x \n",
                     ntStatus));
            return ntStatus;
        }

        //
        // set REQUEST_CONTEXT  parameters.
        //
        vendorRequestContext = GetControlRequestContext(*pNextRequest);
        vendorRequestContext->NdisContext = Nic->Adapter;
        vendorRequestContext->Nic = Nic;

    }

    controlResources->MaxOutstandingRequests = MaxOutstanding;
    controlResources->NextAvailableRequestIndex = MaxOutstanding;

    return ntStatus;
}

VOID
FreeControlRequests(
    PNIC    Nic
    )
{
    PUSB_DEVICE_CONTEXT     pUsbDeviceContext;
    PUSB_CONTROL_RESOURCES    controlResources;
    USHORT requestIndex;

    pUsbDeviceContext = GetUsbDeviceContext(Nic->UsbDevice);
    controlResources = &pUsbDeviceContext->ControlResources;

    //
    // Don't need a lock here because nobody can be making control request by the time this routine 
    // is called
    //
    for (requestIndex = 0; requestIndex < controlResources->MaxOutstandingRequests; requestIndex++) {
        if (controlResources->RequestArray[requestIndex] != NULL) {
            WdfObjectDelete(controlResources->RequestArray[requestIndex]);
            controlResources->RequestArray[requestIndex] = NULL;
        }
    }
    
    if (controlResources->RequestArrayLock != NULL) {
        WdfObjectDelete(controlResources->RequestArrayLock);
        controlResources->RequestArrayLock = NULL;
    }

}


WDFREQUEST
GetControlRequest(
    PNIC    Nic
    )
{
    WDFREQUEST controlRequest;
    PUSB_DEVICE_CONTEXT     pUsbDeviceContext;
    PUSB_CONTROL_RESOURCES    controlResources;

    controlRequest = NULL;
    pUsbDeviceContext = GetUsbDeviceContext(Nic->UsbDevice);
    controlResources = &pUsbDeviceContext->ControlResources;
    WdfSpinLockAcquire(controlResources->RequestArrayLock);

    if (controlResources->NextAvailableRequestIndex != 0) {   // Request is available
        --(controlResources->NextAvailableRequestIndex);
        controlRequest = controlResources->RequestArray[controlResources->NextAvailableRequestIndex];
        controlResources->RequestArray[controlResources->NextAvailableRequestIndex] = NULL;
    }

    WdfSpinLockRelease(controlResources->RequestArrayLock);

    //
    // TODO : If we run out of control requests then allocate on the fly 
    // instead of returning NULL
    //
    return controlRequest;
}

VOID
ReturnControlRequest(
    __in PNIC     Nic,
    WDFREQUEST Request
    )

/*++

Routine Description

    Returns a request back to the available array

Arguments:
    Nic - Pointer to the HW NIC structure. 
    Request - Request to be freed.
    
Return Value:
    VOID
    
--*/
{
    PUSB_DEVICE_CONTEXT     pUsbDeviceContext;
    PUSB_CONTROL_RESOURCES    controlResources;
    WDF_REQUEST_REUSE_PARAMS    params;

    pUsbDeviceContext = GetUsbDeviceContext(Nic->UsbDevice);
    controlResources = &pUsbDeviceContext->ControlResources;
    WDF_REQUEST_REUSE_PARAMS_INIT(&params, WDF_REQUEST_REUSE_NO_FLAGS, STATUS_SUCCESS);
    WdfRequestReuse(Request, &params);
    WdfSpinLockAcquire(controlResources->RequestArrayLock);
    ASSERTMSG("Control Request list is already full!",
              controlResources->NextAvailableRequestIndex < controlResources->MaxOutstandingRequests
               );
    controlResources->RequestArray[controlResources->NextAvailableRequestIndex++] = Request;
    WdfSpinLockRelease(controlResources->RequestArrayLock);

}

EVT_WDF_REQUEST_COMPLETION_ROUTINE VendorControlCompletionRoutine;


void
VendorControlCompletionRoutine(
    __in WDFREQUEST  Request,
    __in WDFIOTARGET  Target,
    __in PWDF_REQUEST_COMPLETION_PARAMS  CompletionParams,
    __in WDFCONTEXT  Context
    )
/*++

Routine Description

    Returns a request back to the available array

Arguments:
    Request - The request which completed
    Target - The USB target to which the request was sent. In this case
             it is the Control EP.
    CompletionParams - Has the status and othe information about the 
                        USB request.
                        
    Context - The ciontext which set when the Request was sent. 
    
Return Value:

    
--*/

{
    PUSB_CONTROL_REQ_CONTEXT vendorRequestContext;
    PWDF_USB_REQUEST_COMPLETION_PARAMS usbCompletionParams;
    NTSTATUS                    status;

    UNREFERENCED_PARAMETER(Target);

    status = CompletionParams->IoStatus.Status;

    //
    // For usb devices, we should look at the Usb.Completion param.
    //
    usbCompletionParams = CompletionParams->Parameters.Usb.Completion;

    if (!NT_SUCCESS(status)) {
           MpTrace( COMP_SEND,DBG_SERIOUS,
            ("VendorRequest failed: request Status 0x%x UsbdStatus 0x%x\n",
             status, usbCompletionParams->UsbdStatus));
    }

    vendorRequestContext = (PUSB_CONTROL_REQ_CONTEXT)Context;
    //
    // inform NDIS
    //
    //3 Check Normal Queue(This should be transparent for upper layer)

    ReturnControlRequest(vendorRequestContext->Nic, Request);

}


NTSTATUS
HwUsbSendVendorControlPacketAsync (
    __in PNIC Nic,
    __in BYTE Request,
    __in USHORT Value,
    __in ULONG Length,
    __in PVOID Buffer,
    __in USHORT Index
    )
/*++

Routine Description

    Send an asynchronous request on the pipe

Arguments:
    Nic - Pointer to the HW NIC structure. 
    Request - Request to be used to send the Vendor COntrol request.
    Value - Value to be used in the Control Transfer Setup packet.
    Length - Amount of data to be sent.
    Buffer - Buffer which has the data to be sent.
    Index - Index to be used in the Control Transfer Setup packet.
    
Return Value:
    NTSTATUS
    
--*/
{
    WDFREQUEST              controlRequest;
    WDFIOTARGET             ioTarget ;
    NTSTATUS                ntStatus;
    PUSB_CONTROL_REQ_CONTEXT vendorRequestContext;
    WDF_USB_CONTROL_SETUP_PACKET    controlSetupPacket;
    WDFMEMORY  memoryObject;

    MpTrace (COMP_OID, DBG_TRACE, ( "--> HwUsbSendVendorControlPacketAsync\n"));
    controlRequest = GetControlRequest(Nic);
    
    if (controlRequest == NULL) {   // No requests left
        MpTrace (COMP_SEND, DBG_SERIOUS, (
                        "HwUsbSendVendorControlPacketAsync Out of control request \n"));
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    vendorRequestContext = GetControlRequestContext(controlRequest);
    //
    // Copy the buffer which is allocated on the stack  to the heap allocated 
    // Request context because we will be sending an async request. 
    //
    NdisMoveMemory(vendorRequestContext->Data, (PUCHAR)Buffer, Length); 

    WDF_USB_CONTROL_SETUP_PACKET_INIT_VENDOR(&controlSetupPacket,
                                        BmRequestHostToDevice,
                                        BmRequestToDevice,
                                        Request, // Request
                                        Value, // Value
                                        Index); // Index
    //
    // Create a valid handle to memory object
    //
    ntStatus = WdfMemoryCreatePreallocated(
                        WDF_NO_OBJECT_ATTRIBUTES,   // Attributes 
                        (PVOID)vendorRequestContext->Data,                      
                        Length,                 // BufferSize 
                        &memoryObject                      // Memory 
                        );

    if(!NT_SUCCESS(ntStatus)) {
        MpTrace (COMP_SEND, DBG_SERIOUS, (
                 "WdfMemoryCreatePreallocated: Failed - 0x%x \n",
                 ntStatus));
        return ntStatus;
    }
    ntStatus = WdfUsbTargetDeviceFormatRequestForControlTransfer(
                                                                 Nic->UsbDevice,
                                                                 controlRequest,
                                                                 &controlSetupPacket,
                                                                 memoryObject,
                                                                 NULL
                                                                  );

    if(!NT_SUCCESS(ntStatus)) {
        MpTrace (COMP_SEND, DBG_SERIOUS, (
                 "WdfUsbTargetDeviceFormatRequestForControlTransfer: Failed - 0x%x \n",
                 ntStatus));
        return ntStatus;
    }

    WdfRequestSetCompletionRoutine(controlRequest, VendorControlCompletionRoutine, vendorRequestContext);

    ioTarget = WdfUsbTargetDeviceGetIoTarget(Nic->UsbDevice);

    if (WdfRequestSend(controlRequest, ioTarget, 0) == FALSE) {   // Failure - Return request
        MpTrace (COMP_SEND, DBG_SERIOUS, (
                 "WdfRequestSend: Failed - 0x%x \n", ntStatus));
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        goto error;
    }

    return ntStatus;

    error:
        ReturnControlRequest(Nic, controlRequest);
        MpTrace( COMP_SEND, DBG_TRACE, ("<-- HwUsbSendVendorControlPacketAsync\n"));


    return ntStatus;
}


