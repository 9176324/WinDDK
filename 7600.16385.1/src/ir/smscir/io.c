//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Copyright 2005 OSR, Open Systems Resources, Inc. All rights Reserved.
// 
// Module Name:
// 
//     io.c
// 
// Abstract:
//
//     Contains routines to handle opens, closes, and all necessary IOCTLs
// 
// Author:
//
// Revision History:
//      
#include "smscir.h"

#ifdef WPP_TRACING
#include "io.tmh"
#endif

#pragma warning(disable:4127) // constant conditional expression

NTSTATUS
IrTransmitBuildFifoData(
    PIR_TRANSMIT_CHUNK TransmitChunk,
    ULONG CapturedByteCount,
    PSMSCIR_TX_FIFO_DATA *TxFifoData
    );


PCCHAR 
IoctlToString(
    ULONG IoControlCode
    );


VOID
SmscIrEvtDeviceFileCreate(
    IN WDFDEVICE            Device,
    IN WDFREQUEST           Request,
    IN WDFFILEOBJECT        FileObject
    ) {
/*++

Routine Description:

  This routine is called when IRCLASS is opening a handle to our
  device.

Arguments:

    Device       - Handle to a framework device object.
    FileObject   - Pointer to fileobject that represents the open handle.
    CreateParams - Parameters of IO_STACK_LOCATION for create


--*/
    PSMSCIR_DATA deviceData;
    KIRQL        oldIrql;
    BOOLEAN      createOK;

    UNREFERENCED_PARAMETER(FileObject);
    
    deviceData = (PSMSCIR_DATA)GetIrData(Device);

    SmscIrTracePrint(
        TRACE_LEVEL_VERBOSE,
        SMSCDBG_CREATE_CLOSE_INFO,
        ("SmscIrEvtDeviceFileCreate: Entered"));

    //
    // The initial open indicates an open of the normal receiver.
    //  We switch to the priority receiver in response to 
    //  an ENTER_PRIORITY_RECEIVE IOCTL
    //
    KeAcquireSpinLock(&deviceData->Receiver.ListSpinLock, &oldIrql);

    if (deviceData->Receiver.OpenCount == 0) {

        SmscIrTracePrint(
            TRACE_LEVEL_INFORMATION,
            SMSCDBG_CREATE_CLOSE_INFO,
            ("SmscIrEvtDeviceFileCreate: Allowing open of FDO"));

        createOK = TRUE;
        deviceData->Receiver.OpenCount = 1;

    } else {

        SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_CREATE_CLOSE_INFO,
            ("SmscIrEvtDeviceFileCreate: Failing request to open "\
             "FDO, device already open"));

        createOK = FALSE;

    }
    
    KeReleaseSpinLock(&deviceData->Receiver.ListSpinLock, oldIrql);

    if (createOK) {

        WdfRequestComplete(Request, STATUS_SUCCESS);

    } else {

        WdfRequestComplete(Request, STATUS_DEVICE_BUSY);

    }

    SmscIrTracePrint(
        TRACE_LEVEL_VERBOSE,
        SMSCDBG_CREATE_CLOSE_INFO,
        ("SmscIrEvtDeviceFileCreate: Exit"));
    
    return;
}


VOID
SmscIrEvtFileClose(
    IN WDFFILEOBJECT FileObject
    ) {
/*++

Routine Description:

   EvtFileClose is called when all the handles represented by the FileObject
   is closed and all the references to FileObject is removed. 

Arguments:

    FileObject - Pointer to fileobject that represents the open handle.

Return Value:

   VOID

--*/
    PSMSCIR_DATA deviceData;
    KIRQL        oldIrql;

    deviceData = (PSMSCIR_DATA)GetIrData(WdfFileObjectGetDevice(FileObject));

    SmscIrTracePrint(
            TRACE_LEVEL_INFORMATION,
            SMSCDBG_CREATE_CLOSE_INFO,
            ("SmscIrEvtFileClose: Closing FDO"));

    KeAcquireSpinLock(&deviceData->Receiver.ListSpinLock, &oldIrql);

    deviceData->Receiver.OpenCount = 0;
    
    KeReleaseSpinLock(&deviceData->Receiver.ListSpinLock, oldIrql);

    return;
}


VOID
SmscIrEvtIoDeviceControl(
    IN WDFQUEUE  Queue,
    IN WDFREQUEST  Request,
    IN size_t  OutputBufferLength,
    IN size_t  InputBufferLength,
    IN ULONG  IoControlCode
    ) {
/*++
Routine Description.

  This routine handles IOCTL requests to the FDO.
  Note that this is the callback for the DEFAULT
  PARALLEL QUEUE. This routine can be called 
  concurrently by multiple threads submitting IOCTL
  requests

  For the receive paths, we're OK with this. We're
  usually swarmed with requests for receive I/O and
  we serialize that ourselves with a spinlock. This 
  provides greater concurrency and usually guarantees
  that once I/O is done we have a new I/O waiting to
  pick up the slack.

  However, for several of the other IOCTLs we really only
  want to support them one at a time. WDF provides the
  concept of a sequential queue, which will present the 
  I/Os within it to our driver one at a time. We've created
  one of those queues and we will forward all requests
  that we want to serialize against each other to it. So,
  we get requests here and decide if we want to handle
  the request in parallel with other requests. If we do,
  we handle it and provide synchronization. If not, we
  forward it to our serial queue and let WDF hand the
  requests off to us one by one.

Arguments:

  Queue              - Our FDO's default WDF queue
  
  Request            - The IOCTL request

  OutputBufferLength - Size of the output buffer

  InputBufferLength  - Size of the input buffer

  IoControlCode      = The IOCTL to process


--*/

    PSMSCIR_DATA deviceData; 
    NTSTATUS     status;

    UNREFERENCED_PARAMETER(InputBufferLength);

    deviceData = (PSMSCIR_DATA)GetIrData(WdfIoQueueGetDevice(Queue));

    SmscIrTracePrint(
            TRACE_LEVEL_INFORMATION,
            SMSCDBG_IOCTL_INFO,
            ("SmscIrEvtIoDeviceControl: Processing IOCTL 0x%x (%s)", 
                IoControlCode,
                IoctlToString(IoControlCode)));


    switch (IoControlCode) {

        case IOCTL_IR_RECEIVE: {

            IrReceive(deviceData,
                      Request, 
                      OutputBufferLength);
            return;

        }

        case IOCTL_IR_PRIORITY_RECEIVE: {

            IrPriorityReceive(deviceData,
                         Request, 
                         OutputBufferLength);
            return;

        }

        default: {

            break;


        }

    }
                                                                               
    //
    // If we didn't handle it here, forward it to
    //  the SerialQueue for serial processing...This
    //  will serialize this IOCTL with all other IOCTLs
    //  that are forwarded to this queue.
    //
    status = WdfRequestForwardToIoQueue(Request, 
                                        deviceData->SerialQueue);


    if (!NT_SUCCESS(status)) {

        SmscIrTracePrint(
                TRACE_LEVEL_ERROR,
                SMSCDBG_IOCTL_INFO,
                ("WdfRequestForwardToIoQueue failed with Status code 0x%x", 
                    status));
        
            
        WdfRequestComplete(Request, status);
        return;
    }

    return;
   
}


VOID
SmscIrEvtIoDeviceControlSerial(
    IN WDFQUEUE  Queue,
    IN WDFREQUEST  Request,
    IN size_t  OutputBufferLength,
    IN size_t  InputBufferLength,
    IN ULONG  IoControlCode
    ) {
/*++
Routine Description.

  This routine handles all IOCTL requests that have
  been forwarded to the SerialQueue, Note that only
  once instance of this routine will ever be running,
  so we have implicit synchronization within all of the
  IOCTL processing routines.

Arguments:

  Queue              - Our FDO's default WDF queue
  
  Request            - The IOCTL request

  OutputBufferLength - Size of the output buffer

  InputBufferLength  - Size of the input buffer

  IoControlCode      = The IOCTL to process


--*/

    PSMSCIR_DATA deviceData 
            = (PSMSCIR_DATA)GetIrData(WdfIoQueueGetDevice(Queue));

    SmscIrTracePrint(
            TRACE_LEVEL_INFORMATION,
            SMSCDBG_IOCTL_INFO,
            ("SmscIrEvtIoDeviceControlSerial: Processing serial IOCTL: %s", 
                IoctlToString(IoControlCode)));


    //
    // Remember - we're called here SERIALLY, so all of these
    //  routines are synchronized against each other. This is 
    //  fine, because none of these are performance sensitive
    //
    switch (IoControlCode) {

        case IOCTL_IR_HANDSHAKE: {

            IrHandshake(deviceData, 
                        Request);
            return;

        }

        case IOCTL_IR_ENTER_PRIORITY_RECEIVE: {

            IrEnterPriorityMode(deviceData,
                           Request, 
                           InputBufferLength);
            return;

        }

        case IOCTL_IR_EXIT_PRIORITY_RECEIVE: {

            IrExitPriorityMode(deviceData,
                          Request);
            return;

        }

        case IOCTL_IR_USER_OPEN: {

            IrUserOpen(deviceData,
                       Request);
            return;

        }

        case IOCTL_IR_USER_CLOSE: {

            IrUserClose(deviceData,
                        Request);
            return;

        }

        case IOCTL_IR_TRANSMIT: {

            IrTransmit(deviceData,
                       Request, 
                       OutputBufferLength, 
                       InputBufferLength);
            return;

        }

        case IOCTL_IR_GET_DEV_CAPS: {

            IrGetDevCaps(deviceData,
                         Request, 
                         OutputBufferLength);
            return;

        }

        case IOCTL_IR_GET_EMITTERS: {

            IrGetEmitters(deviceData,
                          Request, 
                          OutputBufferLength);
            return;

        }

        case IOCTL_IR_FLASH_RECEIVER: {

            IrFlashReceiver(deviceData,
                            Request, 
                            InputBufferLength);
            return;

        }

        case IOCTL_IR_RESET_DEVICE: {

            IrResetDevice(deviceData,
                          Request);
            return;

        }

        default: {

            break;


        }

    }


    SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_IOCTL_INFO,
            ("SmscIrEvtIoDeviceControlSerial: Failing unknown IOCTL"));
    WdfRequestComplete(Request, STATUS_INVALID_DEVICE_REQUEST);
   
}


VOID
IrHandshake(
    PSMSCIR_DATA DeviceData,
    WDFREQUEST Request
    ) {
/*++
Routine Description.

    Called here by IRCLASS before the creation of the 
    HID PDO. A rejection of this message prevents the
    HID stack from being created.

    Request is to completed synchronously.

Arguments:

    DeviceData - Our device extension

    Request    - An IOCTL_IR_HANDSHAKE

--*/
    //
    // Just flag that we've seen it and complete the 
    //  request
    //
    DeviceData->HandshakeAccepted = TRUE;

    SmscIrTracePrint(
            TRACE_LEVEL_INFORMATION,
            SMSCDBG_IOCTL_INFO,
            ("IrHandshake: Handshake from class arrived"));

    WdfRequestComplete(Request, STATUS_SUCCESS);
    return;

}

VOID
IrReceive(
    PSMSCIR_DATA DeviceData,
    WDFREQUEST Request, 
    SIZE_T OutputBufferLength
    ) {
/*++
Routine Description.

    This request is sent exclusively from IRCLASS and receives Run Length Coded
    (RLC) IR data when the device is not running in priority mode. When running
    in priority mode, these requests remain queued but receive no data. 

    An IOCTL_IR_RECEIVE request remains pending until one of two events occurs:

    1)  The data buffer provided in the request has been completely filled 
        with RLC IR data.

    2)  An IR timeout occurs. In the case of an IR timeout, the DataEnd 
        member of the output structure is set to TRUE. 

Arguments:

    DeviceData - Our device extension

    Request    - An IOCTL_IR_RECEIVE request

    OutputBufferLength - Size of a variable length IR_RECEIVE_PARAMS structure

--*/
    NTSTATUS           status = STATUS_SUCCESS;
    KIRQL              oldIrql;
    ULONG              amountData;
    PUCHAR             callersBuffer;
    PIR_RECEIVE_PARAMS receiveParams;
    BOOLEAN            dataEnd;
    ULONG              capturedByteCount;
    PIO_STATUS_BLOCK   ioStatus;

    //
    // Make sure the user is supplying a big enough buffer...
    //
    if (OutputBufferLength < sizeof(IR_RECEIVE_PARAMS)) {

        SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_RECEIVE_INFO,
            ("IrReceive: Buffer size must be at least 0x%x bytes",
                    sizeof(IR_RECEIVE_PARAMS)));

               
        WdfRequestComplete(Request, STATUS_INVALID_BUFFER_SIZE);
        return;
    }


    status = WdfRequestRetrieveOutputBuffer(Request, 
                                            OutputBufferLength,
                                            &receiveParams,
                                            NULL);

    if (!NT_SUCCESS(status)) {
        
        SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_RECEIVE_INFO,
            ("IrReceive: WdfRequestRetrieveOutputBuffer failed - 0x%x", 
                    status));
           
        WdfRequestComplete(Request, status);
        return;
        
    }

    ASSERT(receiveParams);

    //
    // Because the receiveParams are mapped via MDL, the user
    //  still has the ability to modify the buffer contents
    //  from underneath us. This means that we need to capture
    //  the input values and only use the captured value from
    //  then on.
    //
    //
    // Case to ULONG is OK, max transfer size in Windows
    //  is ULONG bytes, the ULONG_PTR is only for alignment
    //  purposes.
    // 
    capturedByteCount = (ULONG)receiveParams->ByteCount;

    //
    // Make sure we check the byte count against the overall
    //  buffer length first to avoid getting tricked into 
    //  an overflow (i.e. 0xFFFFFFFF + sizeof())
    //
    if (capturedByteCount >
         (OutputBufferLength - sizeof(IR_RECEIVE_PARAMS))) {


        //
        // Tricky user...
        //
        SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_RECEIVE_INFO,
            ("IrReceive: Buffer size specified in structure is too large. "\
             "Buffer Size: 0x%x - Overall buffer size: 0x%Ix", 
            capturedByteCount, OutputBufferLength));
        WdfRequestComplete(Request, STATUS_INVALID_BUFFER_SIZE);
        return;
        
    }


    //
    // Setup the caller's buffer...
    //
    callersBuffer = (PUCHAR)&receiveParams->Data[0];

    KeAcquireSpinLock(&DeviceData->Receiver.ListSpinLock, &oldIrql);
   
    // 
    // If we don't have a current IR receive, set this up 
    //  to be it...
    //
    if (DeviceData->Receiver.CurrentIrReceiveRequest == NULL) {

        //
        // If we're not in PRIORITY mode, we can see 
        //  if there is some data to process. If we are in
        //  priority mode we need to be hands off...
        //
        amountData = 0;

        if (!DeviceData->InPriorityMode) {

            //
            // Any data out there?
            //
            amountData = SmscIrDequeueRLCData(&DeviceData->Receiver, 
                                              callersBuffer, 
                                              capturedByteCount,
                                              &dataEnd);
        
            if (dataEnd || 
                (amountData == capturedByteCount)){

                //
                // Filled the user's buffer already or we copied up to 
                //  a data end event. Complete the request.
                //
                SmscIrTracePrint(
                    TRACE_LEVEL_VERBOSE,
                    SMSCDBG_RECEIVE_INFO,
                    ("IrReceive: Data was waiting. Completing request."));

                KeReleaseSpinLock(&DeviceData->Receiver.ListSpinLock, oldIrql);
            
                //
                // Get the IO_STATUS_BLOCK for the underlying IRP. This is 
                //  ehere we'll be filling in the number of bytes 
                //  transferred.
                //
                ioStatus = &WdfRequestWdmGetIrp(Request)->IoStatus;

                ioStatus->Information
                     = FIELD_OFFSET(IR_RECEIVE_PARAMS, Data) + amountData;
                
                receiveParams->DataEnd = dataEnd;
            
                WdfRequestComplete(Request, STATUS_SUCCESS);
                return;
            
            }

        }
        
        //
        // Otherwise we're out of here, we'll complete the 
        //  request when we see all of the data.
        //
        DeviceData->Receiver.CurrentIrReceiveRequest = Request;
        DeviceData->Receiver.ReceiveBuffer           = callersBuffer;
        DeviceData->Receiver.ReceiveBufferSize       = capturedByteCount;
        DeviceData->Receiver.ReceiveCurrentOffset    = amountData;
        DeviceData->Receiver.ReceiveParams           = receiveParams;

        //
        // This request is going to hang out there, and may stay
        //  there indefinitely. We need to be able to cancel this
        //  request if the user wants to disable the device, 
        //  so we need a cancel routine...
        //
        WdfRequestMarkCancelable(Request, IrReceiveInProgressCancel);

            
    } else {

        //
        // Put the request in the queue.
        //
        status = WdfRequestForwardToIoQueue(
                                     Request, 
                                     DeviceData->Receiver.PendingReceiveQueue);

        if(!NT_SUCCESS(status)) {
            
            KeReleaseSpinLock(&DeviceData->Receiver.ListSpinLock, oldIrql);
     
            SmscIrTracePrint(
                TRACE_LEVEL_ERROR,
                SMSCDBG_RECEIVE_INFO,
                ("IrReceive: WdfRequestForwardToIoQueue failed - 0x%x", 
                        status));
            
            WdfRequestComplete(Request, status);
            return;
        }

    }

    KeReleaseSpinLock(&DeviceData->Receiver.ListSpinLock, oldIrql);
    return;

}


VOID
IrReceiveInProgressCancel(
    IN WDFREQUEST  Request
    ) {
/*++
Routine Description.

    Cancel routine set for the in progress 
    receive request.

Arguments:

    Request - An IOCTL_IR_RECEIVE request

--*/
    PSMSCIR_DATA deviceData;
    WDFQUEUE     ioQueue;
    KIRQL        oldIrql;
    WDFREQUEST   failedRequest = NULL;
    NTSTATUS     status;
    NTSTATUS     failureStatus = STATUS_SUCCESS;

    ioQueue = WdfRequestGetIoQueue(Request);

    SmscIrTracePrint(
            TRACE_LEVEL_VERBOSE,
            SMSCDBG_IOCTL_INFO,
            ("IrReceiveInProgressCancel: Entered"));


    deviceData = (PSMSCIR_DATA)GetIrData(WdfIoQueueGetDevice(ioQueue));

    KeAcquireSpinLock(&deviceData->Receiver.ListSpinLock, &oldIrql);


    
    //
    // We're here first. NULL out the current request
    //  in the extension  - the completion side will 
    //  never see this request.
    //
    deviceData->Receiver.CurrentIrReceiveRequest = NULL;


    SmscIrTracePrint(
        TRACE_LEVEL_VERBOSE,
        SMSCDBG_RECEIVE_INFO,
        ("IrReceiveInProgressCancel: Request 0x%p is the currently "\
         "active receive request.",
             Request));

    //
    // Need to get the next request active.
    //
    status = IrSetupNextRequest(&deviceData->Receiver,
                                &failedRequest,
                                &failureStatus);
    if (!NT_SUCCESS(status)) {
    
        SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_RECEIVE_INFO,
            ("IrReceiveInProgressCancel: IrSetupNextRequest failed - 0x%x",
                status));

    }


    KeReleaseSpinLock(&deviceData->Receiver.ListSpinLock, oldIrql);

    WdfRequestComplete(Request, STATUS_CANCELLED);
    
    if (failedRequest) {
        WdfRequestComplete(failedRequest, failureStatus);
    }

    SmscIrTracePrint(
            TRACE_LEVEL_VERBOSE,
            SMSCDBG_IOCTL_INFO,
            ("IrReceiveInProgressCancel: Exited"));


}

VOID
IrPriorityReceive(
    PSMSCIR_DATA DeviceData,
    WDFREQUEST Request, 
    SIZE_T OutputBufferLength
    ) {
/*++
Routine Description.

    This request is sent from user mode to receive RLC IR data for the 
    purpose of learning. If the device is not already in priority mode, 
    initiated by an IOCTL_ENTER_PRIORITY_RECEIVE, the request is failed 
    immediately. If in priority mode, the request will remain pending until 
    one of two events occurs:

        1)  The data buffer provided in the request has been completely 
            filled with priority IR data.

        2)  An IR timeout occurs. The length of time required for the IR 
            timeout was specified when entering priority mode.

    While in priority mode and processing IOCTL_IR_PRIORITY_RECEIVE requests, 
    IOCTL_IR_RECEIVE requests remain pending and are not filled with IR data.

Arguments:

    DeviceData         - Our device extension

    Request            - An IOCTL_IR_PRIORITY_RECEIVE request

    OutputBufferLength - Size of a variable length IR_PRIORITY_RECEIVE_PARAMS 
                         structure

--*/
    NTSTATUS                    status = STATUS_SUCCESS;
    KIRQL                       oldIrql;
    ULONG                       amountData;
    PUCHAR                      callersBuffer;
    PIR_PRIORITY_RECEIVE_PARAMS receiveParams = NULL;
    BOOLEAN                     dataEnd;
    ULONG                       capturedByteCount;
    PIO_STATUS_BLOCK            ioStatus;

    //
    // If we're not in priority mode, there's nothing to do here.
    //
    if (!DeviceData->InPriorityMode) {

        SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_RECEIVE_INFO,
                ("IrPriorityReceive: Device is NOT in priority mode. "\
                 "Failing PRIORITY receive"));

        WdfRequestComplete(Request, STATUS_INVALID_DEVICE_STATE);
        return;

    }

    if (OutputBufferLength < sizeof(IR_PRIORITY_RECEIVE_PARAMS)) {
        
        SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_RECEIVE_INFO,
                ("IrPriorityReceive: Buffer size must be at least 0x%x",
                sizeof(IR_PRIORITY_RECEIVE_PARAMS)));

        //
        // Work around for the current version of WDF Verifier, which
        //  checks the amount of data to be returned before setting
        //  the status in the IRP (the default status in the IRP is
        //  SUCCESS, so if we don't do this it believes we're going to
        //  copy sizeof(IR_PRIORITY_RECEIVE_PARAMS) into a user buffer
        //  that is < sizeof(IR_PRIORITY_RECEIVE_PARAMS)
        //
        WdfRequestWdmGetIrp(Request)->IoStatus.Status   
                                        = STATUS_BUFFER_TOO_SMALL;
        WdfRequestCompleteWithInformation(Request, STATUS_BUFFER_TOO_SMALL, 
                                          sizeof(IR_PRIORITY_RECEIVE_PARAMS));
        return;
    }


    status = WdfRequestRetrieveOutputBuffer(Request, 
                                            OutputBufferLength,
                                            &receiveParams,
                                            NULL);

    if (!NT_SUCCESS(status)) {
        
        SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_RECEIVE_INFO,
            ("IrPriorityReceive: "\
             "WdfRequestRetrieveOutputBuffer failed - 0x%x", 
                status));
        WdfRequestComplete(Request, status);
        return;
        
    }

    //
    // See IrReceive for the reasoning behind capturedByteCount
    //
    capturedByteCount = (ULONG)receiveParams->ByteCount;
    
    //
    // Watch out for a malformed ByteCount...
    //
    if (capturedByteCount >
         (OutputBufferLength - sizeof(IR_PRIORITY_RECEIVE_PARAMS))) {

        //
        // Tricky user...
        //
        SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_RECEIVE_INFO,
            ("IrPriorityReceive: Buffer size specified in structure is too "\
             "large. Buffer Size: 0x%x - Overall buffer size: 0x%Ix", 
            capturedByteCount, OutputBufferLength));
        WdfRequestComplete(Request, STATUS_INVALID_BUFFER_SIZE);
        return;
        
    }

    ASSERT(receiveParams);

    //
    // Setup the caller's buffer...
    //
    callersBuffer = (PUCHAR)&receiveParams->Data[0];

    KeAcquireSpinLock(&DeviceData->PriorityReceiver.ListSpinLock, &oldIrql);

    // 
    // If we don't have a current PRIORITY IR receive, set this up 
    //  to be it...
    //
    if (DeviceData->PriorityReceiver.CurrentIrReceiveRequest == NULL) {

        //
        // Any data out there?
        //
        amountData = SmscIrDequeueRLCData(&DeviceData->PriorityReceiver, 
                                          callersBuffer, 
                                          capturedByteCount,
                                          &dataEnd);
        
        if (dataEnd || 
            (amountData == capturedByteCount)){

            //
            // Filled the user's buffer already or we copied up to 
            //  a data end event. Complete the request.
            //
            SmscIrTracePrint(
                TRACE_LEVEL_VERBOSE,
                SMSCDBG_RECEIVE_INFO,
                ("IrPriorityReceive: Data was waiting. Completing request."));

            KeReleaseSpinLock(&DeviceData->PriorityReceiver.ListSpinLock, 
                              oldIrql);

            //
            // Get the IO_STATUS_BLOCK for the underlying IRP. This is 
            //  ehere we'll be filling in the number of bytes 
            //  transferred.
            //
            ioStatus = &WdfRequestWdmGetIrp(Request)->IoStatus;

            ioStatus->Information
                 = FIELD_OFFSET(IR_PRIORITY_RECEIVE_PARAMS, Data) + amountData;
                            
            receiveParams->DataEnd = dataEnd;
            
            WdfRequestComplete(Request, STATUS_SUCCESS);
            return;
            
        }
        
        //
        // Otherwise we're out of here, we'll complete the 
        //  request when we see all of the data.
        //
        DeviceData->PriorityReceiver.CurrentIrReceiveRequest = Request;
        DeviceData->PriorityReceiver.ReceiveBuffer         = callersBuffer;
        DeviceData->PriorityReceiver.ReceiveBufferSize     = capturedByteCount;
        DeviceData->PriorityReceiver.ReceiveCurrentOffset  = amountData;
        DeviceData->PriorityReceiver.PriorityReceiveParams = receiveParams;

        //
        // This request is going to hang out there, and may stay
        //  there indefinitely. We need to be able to cancel this
        //  request if the user wants to disable the device, 
        //  so we need a cancel routine...
        //
        WdfRequestMarkCancelable(Request, IrPriorityReceiveInProgressCancel);
            
    } else {

        //
        // Put the request in the queue.
        //
        status = WdfRequestForwardToIoQueue(
                             Request, 
                             DeviceData->PriorityReceiver.PendingReceiveQueue);

        if(!NT_SUCCESS(status)) {
            
            KeReleaseSpinLock(&DeviceData->PriorityReceiver.ListSpinLock, 
                              oldIrql);
     
            SmscIrTracePrint(
                TRACE_LEVEL_ERROR,
                SMSCDBG_RECEIVE_INFO,
                ("IrPriorityReceive: WdfRequestForwardToIoQueue failed - 0x%x",
                    status));
            
            WdfRequestComplete(Request, status);
            return;
        }

    }

    KeReleaseSpinLock(&DeviceData->PriorityReceiver.ListSpinLock, oldIrql);
    return;

}




VOID
IrPriorityReceiveInProgressCancel(
    IN WDFREQUEST  Request
    ) {
/*++
Routine Description.

    Cancel routine set for the in progress 
    PRIORITY receive request.

Arguments:

    Request - An IOCTL_IR_PRIORITY_RECEIVE request

--*/
    PSMSCIR_DATA deviceData;
    WDFQUEUE     ioQueue;
    KIRQL        oldIrql;
    WDFREQUEST   failedRequest = NULL;
    NTSTATUS     status;
    NTSTATUS     failureStatus = STATUS_SUCCESS;

    ioQueue = WdfRequestGetIoQueue(Request);

    SmscIrTracePrint(
            TRACE_LEVEL_VERBOSE,
            SMSCDBG_IOCTL_INFO,
            ("IrReceiveInProgressCancel: Entered"));


    deviceData = (PSMSCIR_DATA)GetIrData(WdfIoQueueGetDevice(ioQueue));

    KeAcquireSpinLock(&deviceData->PriorityReceiver.ListSpinLock, &oldIrql);


    //
    // We're here first. NULL out the current request
    //  in the extension - the completion side will never 
    //  see this request.
    //
    deviceData->PriorityReceiver.CurrentIrReceiveRequest = NULL;


    SmscIrTracePrint(
        TRACE_LEVEL_VERBOSE,
        SMSCDBG_RECEIVE_INFO,
        ("IrPriorityReceiveInProgressCancel: Priority Request 0x%p is "\
        "the currently active receive request.", 
        Request));
    
    //
    // Need to get the next request active.
    //
    status = IrSetupNextRequest(&deviceData->PriorityReceiver,
                                &failedRequest,
                                &failureStatus);
    if (!NT_SUCCESS(status)) {
    
        SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_RECEIVE_INFO,
            ("IrPriorityReceiveInProgressCancel: IrSetupNextRequest "\
             "failed - 0x%x", status));

    }

    KeReleaseSpinLock(&deviceData->PriorityReceiver.ListSpinLock, oldIrql);

    WdfRequestComplete(Request, STATUS_CANCELLED);

    if (failedRequest) {
        WdfRequestComplete(failedRequest, failureStatus);
    }

    SmscIrTracePrint(
            TRACE_LEVEL_VERBOSE,
            SMSCDBG_IOCTL_INFO,
            ("IrReceiveInProgressCancel: Exited"));



}


VOID
IrTransmit(
    PSMSCIR_DATA DeviceData,
    WDFREQUEST Request, 
    SIZE_T OutputBufferLength, 
    SIZE_T InputBufferLength
    ) {
/*++
Routine Description.

    Transmits the given IR stream on the given port(s) at the given 
    carrier frequency.
    
    On legacy devices, this maintains the pre-existing carrier 
    frequency, port masks, and sample period values.  (ie. it gets the old 
    values, changes them, transmits, and then changes them back)

    This IOCTL is synchronous.  It does not return until the IR has actually 
    been transmitted.

Arguments:

    DeviceData - Our device extension

    Request    - An IOCTL_IR_TRANSMIT request

    OutputBufferLength - Size of a variable length IR_TRANSMIT_CHUNK structure

    InputBufferLength  - Size of an IR_TRANSMIT_PARAMS structure

--*/    

    NTSTATUS                    status = STATUS_SUCCESS;
    KIRQL                       oldIrql;
    PIR_TRANSMIT_PARAMS         transmitParams;
    PIR_TRANSMIT_CHUNK          transmitChunk;
    ULONG_PTR                   transmitMaxAddress;
    ULONG                       frequencyKHz;
    ULONG                       carrierDivider = 0;
    ULONG                       capturedCarrierPeriod;
    ULONG                       capturedRepeatCount;
    ULONG_PTR                   capturedNextOffset;
    ULONG                       capturedByteCount;
    UCHAR                       capturedTxPorts;
    BIRCC2_TX_CHANNEL_ENABLES   txEnables;
    BIRCC2_LINE_CONTROL         lineControl;
    BIRCC2_MASTER_BLOCK_CONTROL mbc;
    PSMSCIR_TX_FIFO_DATA        txFifoData = NULL;
    WDFREQUEST                  requestToComplete;
    NTSTATUS                    requestStatus = STATUS_SUCCESS;


    UNREFERENCED_PARAMETER(InputBufferLength);

    if (OutputBufferLength < sizeof(IR_TRANSMIT_CHUNK)) {

        SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_TRANSMIT_INFO,
            ("IrTransmit: Output buffer too small, must be at least "\
             "0x%x bytes", sizeof(IR_TRANSMIT_CHUNK)));
        WdfRequestWdmGetIrp(Request)->IoStatus.Status 
                                        = STATUS_BUFFER_TOO_SMALL;
        WdfRequestCompleteWithInformation(Request, STATUS_BUFFER_TOO_SMALL,
                                          sizeof(IR_TRANSMIT_CHUNK));
        return;

    }

    status = WdfRequestRetrieveOutputBuffer(Request, 
                                            OutputBufferLength,
                                            &transmitChunk,
                                            NULL);

    if (!NT_SUCCESS(status)) {
        
        SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_TRANSMIT_INFO,
            ("IrTransmit: WdfRequestRetrieveOutputBuffer failed - 0x%x", 
                    status));
        WdfRequestComplete(Request, status);
        return;
        
    }


    //
    // The data structure contains byte counts and offsets.
    //  Calculate the largest possible address that we can 
    //  use when walking this data structure to ensure
    //  we don't walk all over memory we don't have access
    //  too 
    //
    transmitMaxAddress = ((ULONG_PTR)transmitChunk + OutputBufferLength);


    status = WdfRequestRetrieveInputBuffer(Request, 
                                           sizeof(IR_TRANSMIT_PARAMS),
                                           &transmitParams,
                                           NULL);

    if (!NT_SUCCESS(status)) {
        
        SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_TRANSMIT_INFO,
            ("IrTransmit: WdfRequestRetrieveInputBuffer failed - 0x%x", 
                status));
        WdfRequestComplete(Request, status);
        return;
        
    }


    capturedCarrierPeriod = (ULONG)transmitParams->CarrierPeriod;
    if (capturedCarrierPeriod == 0) {

        if (transmitParams->Flags & TRANSMIT_FLAGS_DC_MODE) {
        
            SmscIrTracePrint(
                TRACE_LEVEL_ERROR,
                SMSCDBG_TRANSMIT_INFO,
                ("IrTransmit: Device does not support DC mode"));
            WdfRequestComplete(Request, STATUS_INVALID_DEVICE_REQUEST);
            return;

        }

        ASSERT(transmitParams->Flags & TRANSMIT_FLAGS_PULSE_MODE);

        SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_TRANSMIT_INFO,
            ("IrTransmit: Pulse mode support not available"));
        WdfRequestComplete(Request, STATUS_NOT_IMPLEMENTED);
        return;

    } else {

        //
        // We've been given the carrier period for the device
        //  in usecs.
        //

        //
        // Calculate the frequency in Hz to keep things integral
        //
        frequencyKHz 
            = SMSCIR_USEC_P_TO_KHZ_F(capturedCarrierPeriod);

        //
        // And turn the value into something for our CFD register (see
        //  section 2.1.1 in the CIRCC2 data book)
        //

        //
        // Beware an obscenely large period from an unsavory user.
        //
        if (frequencyKHz != 0) {

            carrierDivider = SMSCIR_KHZ_TO_CFD(frequencyKHz);

        }
        
        SmscIrTracePrint(
            TRACE_LEVEL_INFORMATION,
            SMSCDBG_IOCTL_INFO,
                ("IrTransmit: Blasting will occur at %dKHz", 
                    frequencyKHz));

        if (carrierDivider == 0 ||
            carrierDivider > UCHAR_MAX) {

            SmscIrTracePrint(
                TRACE_LEVEL_ERROR,
                SMSCDBG_TRANSMIT_INFO,
                    ("Frequency divider result out of range (%d), "\
                     "Defaulting to %d", 
                        carrierDivider, SMSCIR_RC6_CARRIER_DIVIDER));

        }

    }



    //
    // Figure out which emitters this request is going
    //  out on. 
    //
    // Note that we only have 4 emitter jacks, so only
    //  look at the botton four bits 
    //
    capturedTxPorts = (UCHAR)(transmitParams->TransmitPortMask & 0xF);

    //
    // Write out the emitters we're using
    //
    SmscIrTracePrint(
        TRACE_LEVEL_INFORMATION,
        SMSCDBG_IOCTL_INFO,
        ("IrTransmit: Blasting on port mask 0x%x", capturedTxPorts));


    //
    // Need to modify registers, so we need the lock.
    //  The registers on this device use a windowing
    //  scheme, so we always need the lock to avoid
    //  banging into someone else.
    //
    WdfInterruptAcquireLock(DeviceData->Interrupt);

    //
    // Disable interrupts.
    //
    mbc.AsUChar = 0;
    WRITE_TRANSMIT_UCHAR(DeviceData,
                         BIRCC2_MASTER_BLOCK_CONTROL_ADDR,
                         mbc.AsUChar);

    //
    // Reset the fifo
    //
    lineControl.AsUChar   = 0;
    lineControl.FifoReset = 1;
    WRITE_TRANSMIT_UCHAR(DeviceData,
                         BIRCC2_LINE_CONTROL_ADDR,
                         lineControl.AsUChar);

    //
    // Write it out to the blaster mask
    //
    txEnables.AsUChar = capturedTxPorts;
    WRITE_TRANSMIT_UCHAR(DeviceData,
                         BIRCC2_TX_CHANNEL_ENABLES_ADDR,
                         txEnables.AsUChar);

    


    //
    //  Figure out if we need to modify the carrier settings
    //
    if (SMSCIR_RC6_CARRIER_DIVIDER != carrierDivider) {

        //
        // Indeed we do. Update it...
        //
        mbc.AsUChar = 0;
        mbc.RegisterBlockSelect = 1;
        WRITE_TRANSMIT_UCHAR(DeviceData,
                             BIRCC2_MASTER_BLOCK_CONTROL_ADDR,
                             mbc.AsUChar);

        WRITE_TRANSMIT_UCHAR(DeviceData,
                             BIRCC2_CONSUMER_IR_CARRIER,
                             (UCHAR)carrierDivider);

    }

    //
    // Re-enable interrupts on the device (this also puts
    //  us back to register block 0).
    //
    mbc.AsUChar = 0;
    mbc.RegisterBlockSelect = 0;
    mbc.MasterInterruptEnable = TRUE;
    WRITE_TRANSMIT_UCHAR(DeviceData,
                         CIRCC2_MASTER_BLOCK_CONTROL,
                         mbc.AsUChar);

    WdfInterruptReleaseLock(DeviceData->Interrupt);


    //
    // The user has given us a data buffer in RLC format:
    //
    //      500,      -1000,     200,       -500
    //   ON for 10  OFF for 20 ON for 5  OFF for 10
    //
    // Our job is to now convert this into something that
    //  is appropriate for our device, which in our case
    //  is one bit per sample, and transmit it
    //
    // We're going to do this by looping over all of the
    //  user data, converting it to our native format,
    //  then begin blasting. Each time a piece is done our
    //  device will interrupt, we'll queue a DPC, and then
    //  continue the blasting from within the DPC. Once the
    //  blasting is complete we'll complete this transmit 
    //  request
    //

    //
    // We shouldn't have anything outstanding
    //
    ASSERT(IsListEmpty(&DeviceData->TxFifoDataList));

    while (TRUE) {

        //
        // transmitChunk is mapped via an MDL. Make sure we
        //  capture anything important before validating,
        //  the user has the ability to modifiy the contents
        //  while we work on it.
        //
        capturedByteCount   = (ULONG)transmitChunk->ByteCount;
        capturedNextOffset  = transmitChunk->OffsetToNextChunk;
        capturedRepeatCount = (ULONG)transmitChunk->RepeatCount;

        SmscIrTracePrint(
            TRACE_LEVEL_VERBOSE,
            SMSCDBG_TRANSMIT_INFO,
                  ("IrTransmit: Transmit chunk 0x%p: Byte Count 0x%x, "\
                   "Repeat Count 0x%x",
                     transmitChunk, capturedByteCount,
                     capturedRepeatCount));

        //
        // Watch out for a malformed ByteCount...
        //
        if (capturedByteCount >
            (transmitMaxAddress - (ULONG_PTR)transmitChunk->Data)) {

            SmscIrTracePrint(
                TRACE_LEVEL_ERROR,
                SMSCDBG_TRANSMIT_INFO,
               ("IrTransmit: ByteCount of 0x%x is invalid - passes end "\
                "of allocation",
                capturedByteCount));
            status = STATUS_INVALID_BUFFER_SIZE;
            break;

        }


        //
        // Convert this transmit chunk into data that works for
        //  our device.
        //
        status = IrTransmitBuildFifoData(transmitChunk,
                                         capturedByteCount,
                                         &txFifoData);

        if (!NT_SUCCESS(status)) {
            SmscIrTracePrint(
                TRACE_LEVEL_ERROR,
                SMSCDBG_TRANSMIT_INFO,
               ("IrTransmit: IrTransmitBuildFifoData failed - 0x%x", status));
            break;

        }

        //
        // Insert this (now converted) chunk onto the tail of
        //  our "to be blasted" list. No need to lock, we'll only
        //  ever have one TX request active
        //
        InsertTailList(&DeviceData->TxFifoDataList,
                       &txFifoData->ListEntry);


        if (capturedNextOffset == 0) {

            //
            // We ran out of things to process...Get out...
            //
            break;

        }

        //
        // Watch out for a malformed Offset...Life would be easy
        //  if it wasn't for users...
        //
        if (capturedNextOffset >
                (transmitMaxAddress - (ULONG_PTR)transmitChunk->Data)) {

            SmscIrTracePrint(
                TRACE_LEVEL_ERROR,
                SMSCDBG_TRANSMIT_INFO,
                ("IrTransmit: OffsetToNextChunk of 0x%Ix is invalid - passes "\
                 "end of allocation",
                   capturedNextOffset));
            status = STATUS_INVALID_USER_BUFFER;
            break;

        }

        if (capturedNextOffset < 
                (sizeof(IR_TRANSMIT_CHUNK) + capturedByteCount)) {
            SmscIrTracePrint(
                TRACE_LEVEL_ERROR,
                SMSCDBG_TRANSMIT_INFO,
               ("IrTransmit: OffsetToNextChunk of 0x%Ix is invalid - too "\
                "small based on prev chunk",
                 capturedNextOffset));
            status = STATUS_INVALID_USER_BUFFER;
            break;

        }

        //
        // Next!
        //
        transmitChunk 
            = (PIR_TRANSMIT_CHUNK)((ULONG_PTR)transmitChunk 
                                                + capturedNextOffset);


        //
        // Make sure that the user followed best practices
        //  and aligned the next offset properly...
        //
        ASSERT(((ULONG_PTR)transmitChunk % sizeof(ULONG_PTR)) == 0);


    }

    //
    // If we encountered an error it's time to cleanup
    //
    if (!NT_SUCCESS(status)) {
        
        while (!IsListEmpty(&DeviceData->TxFifoDataList)) {

            txFifoData 
                = (PSMSCIR_TX_FIFO_DATA)
                        RemoveHeadList(&DeviceData->TxFifoDataList);

            ExFreePool(txFifoData);

        }

        WdfRequestComplete(Request, status);
        return;

    }


    //
    // We've preprocessed the data, let's get the transfer going...
    //
    KeAcquireSpinLock(&DeviceData->TxFifoDataLock, &oldIrql);

    txFifoData = (PSMSCIR_TX_FIFO_DATA)DeviceData->TxFifoDataList.Flink;

    DeviceData->CurrentTxFifoData = txFifoData;
    DeviceData->CurrentTxRequest  = Request;
    DeviceData->BytesTransmitted  = 0;

    SmscIrTracePrint(
        TRACE_LEVEL_INFORMATION,
        SMSCDBG_TRANSMIT_INFO,
        ("IrTransmit: Beginning blasting"));

    //
    // Call the helper to kick off the blasting (even though
    //  it's called "Continue Blasting" it will do the right thing).
    //
    SmscIrContinueBlasting(DeviceData,
                           &requestToComplete,
                           &requestStatus);

    KeReleaseSpinLock(&DeviceData->TxFifoDataLock, oldIrql);

    if (requestToComplete) {

        //
        // This would only happen on incredibly small transfers (like,
        //  < 32 bytes) or requests that were cancelled while we were
        //  preprocessing the data.
        //
        WdfRequestComplete(requestToComplete, requestStatus);

    } else {

        
        //
        // Start the deadman. If the system is under much stress
        //  there's a chance for the TX FIFO to underrun, so we
        //  need to keep a deadman out there on the watch (we won't
        //  necessarily get an interrupt for the underrun, and
        //  the TX FIFO interrupt stopping also stops the RX FIFO
        //  interrupt)
        //
        WdfTimerStart(DeviceData->DeadManTimer, 
                      WDF_REL_TIMEOUT_IN_MS(SMSCIR_DEADMAN_TIMER_PERIOD));

    }



    return;


}



NTSTATUS
IrTransmitBuildFifoData(
    PIR_TRANSMIT_CHUNK TransmitChunk,
    ULONG CapturedByteCount,
    PSMSCIR_TX_FIFO_DATA *TxFifoData
    ) {
/*++
Routine Description.

    This routine takes the RLC data that is sent from the user
    mode app and turns it into data that can be pushed out to
    our TX FIFO

Arguments:

    DeviceData - Our device extension

    TxFifoData - On successful return contains a PSMSCIR_TX_FIFO_DATA
                 that describes the RLC data in TransmitChunk in terms
                 of the CIR device

--*/    
    ULONG_PTR            i;
    ULONG_PTR            rlcPackets;
    LONG                 numSamplesThisPacket;
    LONG                 rlcSample;
    UCHAR                fifoByte = 0;
    ULONG                bitPos = 0;
    ULONG                bytesInFifo = 0;
    UCHAR                on;
    PLONG                rlcDataPtr;
    ULONG                fifoBufferSize;
    PSMSCIR_TX_FIFO_DATA txFifoData;
    ULONG                paddingRequired;

    if (CapturedByteCount < sizeof(LONG)) {

        //
        // Bad buffer.
        //
        SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_TRANSMIT_INFO,
            ("IrTransmitBuildFifoData: Invalid byte count."));
        return STATUS_BUFFER_TOO_SMALL;

    }

    //
    // RLC samples are stored in LONGs by the caller.
    //
    rlcPackets = CapturedByteCount / sizeof(LONG);
    
    //
    // Need a LONG pointer to the data...
    //
    rlcDataPtr = (PLONG)&TransmitChunk->Data[0];

    //
    // First loop through and compute the required
    //  FIFO buffer size...This is suboptimal, but
    //  is rquired in order to figure out how many
    //  FIFO bytes this transfer is going to require
    //  (and we're choosing to convert the entire buffer
    //  up front instead of in pieces as we process in
    //  order to spend less time at IRQL > PASSIVE_LEVEL -
    //  if we didn't do this now we'd be doing it in our
    //  DPC or ISR as we blast)
    //
    fifoBufferSize = 0;

    for (i = 0; i < rlcPackets; i++) {
        
        //
        // Get the RLC sample...
        //
        rlcSample = rlcDataPtr[i];
        
        if (rlcSample < 0) {

            //
            //  Get the sign right.
            //
            rlcSample = -rlcSample;

        }
        
        //
        // Now add it to the overall buffer size. We'll
        //  div this total by IR_SAMPLE_PERIOD when we're done
        //
        fifoBufferSize += rlcSample;

    }

    if (fifoBufferSize < IR_SAMPLE_PERIOD) {

        //
        // Bad data buffer...
        //
        SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_TRANSMIT_INFO,
            ("IrTransmitBuildFifoData: User buffer contained no samples."));

        return STATUS_INVALID_USER_BUFFER;

    }


    //
    // fifoBufferSize now contains the number of individual samples
    //  * IR_SAMPLE_PERIOD, so div it out. Note that we won't 
    //  need all of this buffer since we'll be packing the bits in
    //  as we go. However, we're going to take the hit of overallocating
    //  instead of the perf hit of calculating the exact size twice (once
    //  to figure out how big it needs to be, then again to build it)
    //
    fifoBufferSize = (fifoBufferSize / IR_SAMPLE_PERIOD);

    //
    // Due to the way we configure the device, We always 
    //  need to blast in multiples of SMSCIR_FIFO_SIZE 
    //  pieces. Pad this buffer so we can round up later
    //
    txFifoData = (PSMSCIR_TX_FIFO_DATA)
                ExAllocatePoolWithTag(
                   NonPagedPool,
                   fifoBufferSize+sizeof(SMSCIR_TX_FIFO_DATA)+SMSCIR_FIFO_SIZE,
                   'DFxT');

    if (!txFifoData) {

        SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_TRANSMIT_INFO,
            ("IrTransmitBuildFifoData: Cannot allocate %d bytes of memory.",
            fifoBufferSize+sizeof(SMSCIR_TX_FIFO_DATA)+SMSCIR_FIFO_SIZE));
        return STATUS_INSUFFICIENT_RESOURCES;

    }

    txFifoData->CurrentOffset = 0;
    txFifoData->RepeatCount = (ULONG)TransmitChunk->RepeatCount;
    txFifoData->TimesRepeated = 0;

    //
    // *****NOTE*****
    //
    //  We have to be extra careful here because the user
    //  could have already changed the data buffer supplied
    //  with the transfer (the TransmitChunk is described
    //  via DIRECT_IO).
    //
    //  All we're going to do is build the FIFO buffer
    //  with the data as it is at this moment up to
    //  fifoBufferSize bytes. 
    //
    // *****NOTE*****
    //

    //
    // Now go through and REALLY build the buffer. We'll need
    //  one bit per sample, and the LSBit of one byte is the 
    //  continuation of the MSBit of the previous byte
    //
    for (i = 0; i < rlcPackets; i++) {
        
        //
        // Get the number of samples in the RLC packet. 
        //
        numSamplesThisPacket = rlcDataPtr[i];
        
        if (numSamplesThisPacket < 0) {

            //
            // Indicates an off, or "dark", period in the sample,
            //

            //
            //  Get the sign right.
            //
            numSamplesThisPacket = -numSamplesThisPacket;
            on = FALSE;
        } else {

            //
            // Indicates an on, or "light", period in the sample,
            //
            on = TRUE;
        }
        
        //
        // We now have a positive number of 50usec RLC samples.
        //  We div by 50 to get the number of samples, 
        //  and then we need to build UCHARs containing
        //  one bit per 50usc sample to send to the transmit
        //  FIFO on the device
        //
        numSamplesThisPacket /= IR_SAMPLE_PERIOD;

        while (numSamplesThisPacket > 0) {
            
            //
            // Fill in as many of the high bits as possible,
            //  starting at whatever position we are in the
            //  overall byte
            //
            if (on) {
                
                fifoByte &= ~(0xFF << bitPos);
                
            } else {
                
                fifoByte |= (0xFF << bitPos);
                
            }
                
            if ((numSamplesThisPacket + bitPos) < 8) {
                
                //
                // We didn't completely fill this
                //  byte yet but we're done with the
                //  sample (e.g. this was the first piece
                //  of the sample and it was ON for 1, so 
                //  we have 7 bits remaining in the byte)
                //
                // Remember our position in the byte so we 
                //  can properly process the next sample
                //
                bitPos += numSamplesThisPacket;
                break;
                
            } else {
                
                //
                // We're finishing off a byte. May be leftover
                //  bits from a previous pass, so subtract our
                //  position from the total number of possible
                //  bits.
                //
                numSamplesThisPacket -= (8-bitPos);

                //
                // Put this byte in the FIFO.
                //
                txFifoData->FifoBuffer[bytesInFifo++] = fifoByte;
                
                //
                // Used all bits. Restart the position,
                //
                bitPos = 0;

                if (bytesInFifo > fifoBufferSize) {

                    //
                    // This shouldn't happen: we've exhausted our 
                    //  previous overallocation of the user buffer.
                    //
                    //  The only reason for this would be the user 
                    //  data buffer changing out from underneath us,
                    //  so this is a user mode app problem.
                    //
                    ExFreePool(txFifoData);
                    SmscIrTracePrint(
                        TRACE_LEVEL_ERROR,
                        SMSCDBG_TRANSMIT_INFO,
                        ("IrTransmitBuildFifoData: User data changed while "
                         "processing. Aborting TX"));
                    return STATUS_DATA_NOT_ACCEPTED;

                }

            }

        }

    }


    if (bitPos) {

        //
        // If bit position isn't zero, then the last packet
        //  was short on filling up the byte. imagine ON for 8,
        //  OFF for 2 being the last two samples...We now have
        //  a partially empty packet that needs to have the upper
        //  bits stuffed.
        //
        // We'll deal with that here by padding the last byte with 
        //  silence
        //
        fifoByte |= (0xFF << bitPos);
        txFifoData->FifoBuffer[bytesInFifo++] = fifoByte;
    }

    //
    // Add whatever silence we need at the end to make
    //  this SMSCIR_FIFO_SIZE aligned
    //
    paddingRequired 
            = (SMSCIR_FIFO_SIZE - (bytesInFifo & (SMSCIR_FIFO_SIZE-1)));

    for (i = 0; i < paddingRequired; i++) {

        txFifoData->FifoBuffer[bytesInFifo++] = 0xFF;

    }

    ASSERT((bytesInFifo % SMSCIR_FIFO_SIZE) == 0);

    txFifoData->FifoBufferLength = bytesInFifo;

    //
    // Done building the buffer.
    //
    *TxFifoData = txFifoData;
    return STATUS_SUCCESS;

}



VOID
IrGetDevCaps(
    PSMSCIR_DATA DeviceData,
    WDFREQUEST Request, 
    SIZE_T OutputBufferLength
    ) {
/*++
Routine Description.

    Returns device capabilities.  

Arguments:

    DeviceData - Our device extension

    Request    - An IOCTL_IR_GET_DEV_CAPS request

    OutputBufferLength - sizeof(IR_DEV_CAPS)

--*/
    PIR_DEV_CAPS devCaps;
    NTSTATUS status;

    UNREFERENCED_PARAMETER(DeviceData);

    if (OutputBufferLength < sizeof(IR_DEV_CAPS)) {
        SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_IOCTL_INFO,
                ("IrGetDevCaps: Buffer size must be at least 0x%x",
                sizeof(IR_DEV_CAPS)));
        WdfRequestWdmGetIrp(Request)->IoStatus.Status 
                                        = STATUS_BUFFER_TOO_SMALL;
        WdfRequestCompleteWithInformation(Request, STATUS_BUFFER_TOO_SMALL, 
                                          sizeof(IR_DEV_CAPS));
        return;
    }

    status = WdfRequestRetrieveOutputBuffer(Request,
                                            sizeof(IR_DEV_CAPS),
                                            &devCaps,
                                            NULL);

    if (!NT_SUCCESS(status)) {

        SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_IOCTL_INFO,
            ("WdfRequestRetrieveOutputBuffer failed with Status code 0x%x", 
                status));
        WdfRequestComplete(Request, status);
        return;

    }

    //
    // Fill in the capabilities based on what we just happen to  "know" 
    //  about out device
    //
    devCaps->ProtocolVersion      = DEV_CAPS_PROTOCOL_VERSION;
    devCaps->DevCapsFlags         = 0;
    devCaps->NumReceivePorts      = DEVCAPS_NUM_RECEIVE_PORTS;
    devCaps->LearningReceiverMask = DEVCAPS_LEARNING_MASK;
    devCaps->NumTransmitPorts     = DEVCAPS_TRANSMIT_PORTS;

    SmscIrTracePrint(
        TRACE_LEVEL_INFORMATION,
        SMSCDBG_IOCTL_INFO,
        ("IrGetDevCaps: Returning capabilities:"));
    SmscIrTracePrint(
        TRACE_LEVEL_INFORMATION,
        SMSCDBG_IOCTL_INFO,
        ("\tProtocolVersion = %Id", devCaps->ProtocolVersion));
    SmscIrTracePrint(
        TRACE_LEVEL_INFORMATION,
        SMSCDBG_IOCTL_INFO,
        ("\tDevCapsFlags = 0x%Ix", devCaps->DevCapsFlags));
    SmscIrTracePrint(
        TRACE_LEVEL_INFORMATION,
        SMSCDBG_IOCTL_INFO,
        ("\tNumReceivePorts = %Id", devCaps->NumReceivePorts));
    SmscIrTracePrint(
        TRACE_LEVEL_INFORMATION,
        SMSCDBG_IOCTL_INFO,
        ("\tLearningReceiverMask = 0x%Ix", devCaps->LearningReceiverMask));
    SmscIrTracePrint(
        TRACE_LEVEL_INFORMATION,
        SMSCDBG_IOCTL_INFO,
        ("\tNumTransmitPorts = %Id", devCaps->NumTransmitPorts));

    
    WdfRequestCompleteWithInformation(Request, STATUS_SUCCESS,
                                      sizeof(IR_DEV_CAPS));

}


VOID
IrUserOpen(
    PSMSCIR_DATA DeviceData,
    WDFREQUEST Request
    ) {
/*++
Routine Description.

    Called here by IRCLASS when our port FDO is indirectly
    opened by a user opening the IRCLASS FDO. 

    This IOCTL is simply informational, IRCLASS will not
    reject the open if this is failed.

Arguments:

    DeviceData - Our device extension

    Request    - An IOCTL_IR_USER_OPEN request

--*/

    UNREFERENCED_PARAMETER(DeviceData);

    SmscIrTracePrint(
            TRACE_LEVEL_VERBOSE,
            SMSCDBG_IOCTL_INFO,
            ("IrUserOpen: Entered"));

    //
    // Just complete the request.
    //
    WdfRequestComplete(Request, STATUS_SUCCESS);

    SmscIrTracePrint(
            TRACE_LEVEL_VERBOSE,
            SMSCDBG_IOCTL_INFO,
            ("IrUserOpen: Exited"));

    return;

}

VOID
IrUserClose(
    PSMSCIR_DATA DeviceData,
    WDFREQUEST Request
    ) {
/*++
Routine Description.

    Called here by IRCLASS when our port FDO is indirectly
    closed by a user closing its handle to the IRCLASS FDO. 

Arguments:

    DeviceData - Our device extension

    Request    - An IOCTL_IR_USER_CLOSE request


--*/

    SmscIrTracePrint(
            TRACE_LEVEL_VERBOSE,
            SMSCDBG_IOCTL_INFO,
            ("IrUserClose: Entered"));

    //
    // For closes, make sure that the user did us
    //  right and exited priority mode if he entered it.
    //  If he didn't, fix it for him...
    //
    if (DeviceData->InPriorityMode) {

        SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_IOCTL_INFO,
            ("IrUserClose: The application exited without exiting priority "\
             "mode! Cleaning up"));
        IrExitPriorityMode(DeviceData,
                      Request);
        return;

    }

    //
    // Otherwise just complete it...
    //
    WdfRequestComplete(Request, STATUS_SUCCESS);

    SmscIrTracePrint(
            TRACE_LEVEL_VERBOSE,
            SMSCDBG_IOCTL_INFO,
            ("IrUserOpen: Exited"));

    return;

}


VOID
IrEnterPriorityMode(
    PSMSCIR_DATA DeviceData,
    WDFREQUEST Request, 
    SIZE_T InputBufferLength
    ) {

/*++
Routine Description.

    This request is sent from a user mode application to prepare the port to 
    return priority IR data. While the device is in priority mode, all 
    IOCTL_IR_RECEIVE requests should be starved and IOCTL_IR_PRIORITY_RECEIVE 
    requests should be completed with RLC IR data.

Arguments:

    DeviceData - Our device extension

    Request    - An IOCTL_IR_ENTER_PRIORITY_RECEIVE request

    InputBufferLength  - sizeof(IOCTL_IR_ENTER_PRIORITY_RECEIVE_PARAMS)

--*/
    NTSTATUS                status = STATUS_SUCCESS;
    PIOCTL_IR_ENTER_PRIORITY_RECEIVE_PARAMS priorityModeParams = NULL;
    ULONG                   numFFs;
    ULONG                   localTimeoutInUSec;
    MASTER_BLOCK_CONTROL    mbc;
    SCE_CONFIG_B            configB;
    LINE_CONTROL_B          lcb;
    LINE_CONTROL_A          lca;
    CONSUMER_IR_CTRL        irCtrl;
    KIRQL                   oldIrql;
    WDFREQUEST              successfulRequest = NULL;
    WDFREQUEST              failedRequest = NULL;
    NTSTATUS                failureStatus;
    ULONG                   minusOne;
    CARRIER_CAPTURE_CONTROL captureControl;
    ULONG                   receiver;


    SmscIrTracePrint(
            TRACE_LEVEL_VERBOSE,
            SMSCDBG_IOCTL_INFO,
            ("IrEnterPriorityMode: Entered"));

    //
    // Sorry buddy, can't help you if we're already in priority mode...
    //
    if (DeviceData->InPriorityMode) {
        SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_IOCTL_INFO,
                ("IrEnterPriorityMode: Device is already in priority mode. "\
                 "Failing ENTER_PRIORITY_RECEIVE"));
        WdfRequestComplete(Request, STATUS_INVALID_DEVICE_STATE);
        return;

    }

    if (InputBufferLength < sizeof(IOCTL_IR_ENTER_PRIORITY_RECEIVE_PARAMS)) {
        SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_IOCTL_INFO,
                ("IrEnterPriorityMode: Buffer size must be at least 0x%x",
                sizeof(IOCTL_IR_ENTER_PRIORITY_RECEIVE_PARAMS)));
        WdfRequestWdmGetIrp(Request)->IoStatus.Status 
                                        = STATUS_BUFFER_TOO_SMALL;
        WdfRequestCompleteWithInformation(
                               Request, 
                               STATUS_BUFFER_TOO_SMALL, 
                               sizeof(IOCTL_IR_ENTER_PRIORITY_RECEIVE_PARAMS));
        return;
    }


    status = WdfRequestRetrieveInputBuffer(Request, 
                                           InputBufferLength,
                                           &priorityModeParams,
                                           NULL);

    if (!NT_SUCCESS(status)) {
        
        SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_IOCTL_INFO,
            ("IrEnterPriorityMode: WdfRequestRetrieveInputBuffer failed "\
             "with Status code 0x%x",
                status));
        WdfRequestComplete(Request, status);
        return;
        
    }

    ASSERT(priorityModeParams);

    //
    // Get the receiver the caller wants to use
    //
    receiver = (ULONG)priorityModeParams->Receiver;

    if (receiver > DEVCAPS_NUM_RECEIVE_PORTS) {

        SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_IOCTL_INFO,
            ("IrEnterPriorityMode: Receiver #%d invalid. Device only has %d "\
             "receivers ", receiver, DEVCAPS_NUM_RECEIVE_PORTS));
        WdfRequestComplete(Request, STATUS_INVALID_PARAMETER);
        return;

    }


    //
    // Figure out the number of FFs we need to see for a timeout.
    //  Any timeout less than 8 * 50usecs will be rounded up, but it
    //  prevents us from having to calculate RLC from within the ISR
    //  (which isn't an option anyway)
    //
    //
    // Timeout is given in milliseconds....
    //
    localTimeoutInUSec = (ULONG)priorityModeParams->TimeOut * 1000;


    if (!localTimeoutInUSec) {

        //
        // Invalid to not give some sort of timeout
        //
        SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_IOCTL_INFO,
            ("IrEnterPriorityMode: Zero timeout supplied. Invalid."));
        WdfRequestComplete(Request, STATUS_INVALID_PARAMETER);
        return;
    }
    //
    // Round it up to the next 50usec unit...
    //
    numFFs = (localTimeoutInUSec + (IR_SAMPLE_PERIOD-1)) & ~(IR_SAMPLE_PERIOD-1);
    //
    // And divide by 50usec...
    //
    numFFs = numFFs / IR_SAMPLE_PERIOD;

    //
    // Divide into number of FF bytes required for a data end...
    //
    numFFs = (numFFs/8);

    if (numFFs != DeviceData->NumFFsForDataEnd) {

        SmscIrTracePrint(
            TRACE_LEVEL_INFORMATION,
            SMSCDBG_IOCTL_INFO,
                ("IrEnterPriorityMode: Setting new timeout FFs value: 0x%x", 
                    numFFs));
        
        DeviceData->NumFFsForDataEnd = numFFs;    
    }


    //
    // We're in priority mode.
    //
    DeviceData->PriorityReceiver.OpenCount = 1;
    DeviceData->InPriorityMode = TRUE;


    //
    // Abort the current normal receive. This is to
    //  deal with the case of the user pressing and 
    //  holding a button that causes the device to 
    //  go into priority mode. If the user holds
    //  the button for too long then the button UP
    //  will go to the priority reader, which will
    //  result in the appearance of a stuck keypress
    //
    KeAcquireSpinLock(&DeviceData->Receiver.ListSpinLock, &oldIrql);


    //
    // Put a DataEnd in the receiver  However, before we do
    //  that reset the BytesToDataEnd field. This will ensure
    //  that is actually gets picked up.
    //
    DeviceData->Receiver.BytesToDataEnd = 0;

    minusOne = 0xFF;

    //
    // TRUE for DataEnd
    //
    SmscIrEnqueueRLCData(&DeviceData->Receiver,
                         (PUCHAR)&minusOne,
                         sizeof(ULONG),
                         TRUE);

    //
    // And call the helper to get the request to complete.
    //
    SmscIrProcessPendingReceives(&DeviceData->Receiver,
                                 &successfulRequest,
                                 &failedRequest,
                                 &failureStatus);


    KeReleaseSpinLock(&DeviceData->Receiver.ListSpinLock, oldIrql);


    if (successfulRequest) {

        SmscIrTracePrint(
                TRACE_LEVEL_INFORMATION,
                SMSCDBG_IOCTL_INFO,
                ("IrEnterPriorityMode: Aborting normal read 0x%p before "\
                 "entering priority mode", successfulRequest));

        WdfRequestCompleteWithPriorityBoost(successfulRequest, 
                                            STATUS_SUCCESS,
                                            IO_KEYBOARD_INCREMENT);
                
    }

    if (failedRequest) {
        
        SmscIrTracePrint(
                TRACE_LEVEL_ERROR,
                SMSCDBG_IOCTL_INFO,
                ("IrEnterPriorityMode: SmscIrProcessPendingReceives failed "\
                 "request 0x%p", failedRequest));
        WdfRequestComplete(failedRequest, failureStatus);

    }


    //
    // Time to move the device into learning mode
    //
    WdfInterruptAcquireLock(DeviceData->Interrupt);

    //
    // Reset the number of consecutive FFs we've seen
    //
    DeviceData->ConsecutiveFFs = 0;

    //
    // Shut the receiver off
    //
    lcb.AsUChar = 0;
    lcb.SCEModeBits = 0;
    WRITE_HARDWARE_UCHAR(DeviceData,
                         CIRCC2_LINE_CONTROL_B,
                         lcb.AsUChar);

    //
    // Reset the FIFO
    //
    lca.AsUChar = 0;
    lca.FifoReset = TRUE;
    WRITE_HARDWARE_UCHAR(DeviceData,
                         CIRCC2_LINE_CONTROL_A,
                         lca.AsUChar);


    if (receiver == DEVCAPS_LEARNING_INDEX) {

        SmscIrTracePrint(
            TRACE_LEVEL_INFORMATION,
            SMSCDBG_IOCTL_INFO,
                ("IrEnterPriorityMode: Priority read on learning receiver"));

        //
        // Switch to the learning receiver.
        //
        mbc.AsUChar = 0;
        mbc.RegisterBlockSelect = 1;
        WRITE_HARDWARE_UCHAR(DeviceData,
                             CIRCC2_MASTER_BLOCK_CONTROL,
                             mbc.AsUChar);

        configB.AsUChar = 0;
        configB.OutputMux = SCE_CONFIB_B_LEARN_MUX;
        WRITE_HARDWARE_UCHAR(DeviceData,
                             CIRCC2_SCE_CONFIG_B,
                             configB.AsUChar);

        mbc.AsUChar = 0;
        mbc.RegisterBlockSelect = 2;
        WRITE_HARDWARE_UCHAR(DeviceData,
                             CIRCC2_MASTER_BLOCK_CONTROL,
                             mbc.AsUChar);

        //
        // No carrier demodulation for learn mode
        // 
        irCtrl.AsUChar = 0;
        irCtrl.CarrierOff  = FALSE;
        irCtrl.ReceiveSync = TRUE;
        WRITE_HARDWARE_UCHAR(DeviceData,
                             CIRCC2_CONSUMER_IR_CTRL,
                             irCtrl.AsUChar);

        //
        // We see a 50uSec period of silence when we
        // switch to the learning receiver on the SIO1049.
        // Ignore this.
        //
        DeviceData->PriorityReceiver.IgnoreFirstSilence = TRUE;
        

    } else {

        SmscIrTracePrint(
            TRACE_LEVEL_INFORMATION,
            SMSCDBG_IOCTL_INFO,
                ("IrEnterPriorityMode: Priority read on normal receiver"));

        mbc.AsUChar = 0;
        mbc.RegisterBlockSelect = 2;
        WRITE_HARDWARE_UCHAR(DeviceData,
                             CIRCC2_MASTER_BLOCK_CONTROL,
                             mbc.AsUChar);

        irCtrl.AsUChar = 0;
        irCtrl.CarrierOff  = TRUE;
        irCtrl.ReceiveSync = TRUE;
        WRITE_HARDWARE_UCHAR(DeviceData,
                             CIRCC2_CONSUMER_IR_CTRL,
                             irCtrl.AsUChar);


    }



    //
    // Start the carrier measurement
    //
    mbc.AsUChar = 0;
    mbc.RegisterBlockSelect = 5;
    WRITE_HARDWARE_UCHAR(DeviceData,
                         CIRCC2_MASTER_BLOCK_CONTROL,
                         mbc.AsUChar);

    captureControl.AsUChar = 0;    
    captureControl.CaptureReset = TRUE;
    WRITE_HARDWARE_UCHAR(DeviceData,
                         CIRCC2_CARRIER_CAPTURE_CONTROL,
                         captureControl.AsUChar);

    captureControl.AsUChar = 0;
    captureControl.CaptureMeasurement = TRUE;
    WRITE_HARDWARE_UCHAR(DeviceData,
                         CIRCC2_CARRIER_CAPTURE_CONTROL,
                         captureControl.AsUChar);

    //
    // Re-enable interrupts 
    //
    mbc.AsUChar = 0;
    mbc.MasterInterruptEnable = TRUE;
    WRITE_HARDWARE_UCHAR(DeviceData,
                         CIRCC2_MASTER_BLOCK_CONTROL,
                         mbc.AsUChar);

    //
    // Re-enable the receiver
    //
    lcb.AsUChar = 0;
    lcb.SCEModeBits = SCE_MODE_RECEIVE;
    WRITE_HARDWARE_UCHAR(DeviceData,
                         CIRCC2_LINE_CONTROL_B,
                         lcb.AsUChar);

    WdfInterruptReleaseLock(DeviceData->Interrupt);


    SmscIrTracePrint(
            TRACE_LEVEL_VERBOSE,
            SMSCDBG_IOCTL_INFO,
            ("IrEnterPriorityMode: Exited"));

    WdfRequestComplete(Request, status);
    return;

}


VOID
IrExitPriorityMode(
    PSMSCIR_DATA DeviceData,
    WDFREQUEST Request
    ) {
/*++
Routine Description.
    
    This request is sent from a user mode application when it is done reading 
    priority IR data. Upon receipt of the request, the port should abort any 
    outstanding IOCTL_IR_PRIORITY_RECEIVE requests and fail any future 
    IOCTL_IR_PRIORITY_RECEIVE requests. This IOCTL is also responsible for 
    restoring the device to the state that it was in before receipt of the 
    IOCT_IR_ENTER_PRIORITY_RECEIVE.

Arguments:

    DeviceData - Our device extension

    Request    - An IOCTL_IR_EXIT_PRIORITY_RECEIVE request

--*/
    KIRQL                  oldIrql;
    WDFREQUEST             activeRequest;
    MASTER_BLOCK_CONTROL   mbc;
    SCE_CONFIG_B           configB;
    LINE_CONTROL_A         lca;
    LINE_CONTROL_B         lcb;
    CONSUMER_IR_CTRL       irCtrl;
    NTSTATUS               status;


    SmscIrTracePrint(
            TRACE_LEVEL_VERBOSE,
            SMSCDBG_IOCTL_INFO,
            ("IrExitPriorityMode: Entered"));

    //
    // Note that because this IOCTL is in the 
    //  same serial queue as the enter priority mode
    //  IOCTL, there is no race between entering and
    //  exiting priority mode.
    //
    //
    // Sorry buddy, can't help you if we're not in priority mode...
    //
    if (!DeviceData->InPriorityMode) {
        SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_IOCTL_INFO,
                ("Device is not in priority mode. " \
                 "Failing EXIT_PRIORITY_RECEIVE"));
        WdfRequestComplete(Request, STATUS_INVALID_DEVICE_STATE);
        return;

    }

    //
    // We're no longer in priority mode.
    //
    DeviceData->InPriorityMode = FALSE;


    WdfInterruptAcquireLock(DeviceData->Interrupt);


    //
    // Shut the receiver off
    //
    lcb.AsUChar = 0;
    lcb.SCEModeBits = 0;
    WRITE_HARDWARE_UCHAR(DeviceData,
                         CIRCC2_LINE_CONTROL_B,
                         lcb.AsUChar);

    //
    // Dump anything in the FIFO
    //
    lca.AsUChar = 0;
    lca.FifoReset = TRUE;
    WRITE_HARDWARE_UCHAR(DeviceData,
                         CIRCC2_LINE_CONTROL_A,
                         lca.AsUChar);

    mbc.AsUChar = 0;
    mbc.RegisterBlockSelect = 2;
    WRITE_HARDWARE_UCHAR(DeviceData,
                         CIRCC2_MASTER_BLOCK_CONTROL,
                         mbc.AsUChar);

    //
    // Carrier demodulation on for normal receive
    //
    irCtrl.AsUChar = 0;
    irCtrl.CarrierOff  = TRUE;
    irCtrl.ReceiveSync = TRUE;
    WRITE_HARDWARE_UCHAR(DeviceData,
                         CIRCC2_CONSUMER_IR_CTRL,
                         irCtrl.AsUChar);

    //
    // Switch back to the normal receiver.
    //
    mbc.AsUChar = 0;
    mbc.RegisterBlockSelect = 1;
    WRITE_HARDWARE_UCHAR(DeviceData,
                         CIRCC2_MASTER_BLOCK_CONTROL,
                         mbc.AsUChar);

    configB.AsUChar = 0;
    configB.OutputMux = SCE_CONFIB_B_NORMAL_MUX;
    WRITE_HARDWARE_UCHAR(DeviceData,
                         CIRCC2_SCE_CONFIG_B,
                         configB.AsUChar);

    //
    // Re-enable interrupts
    //
    mbc.AsUChar = 0;
    mbc.MasterInterruptEnable = TRUE;
    WRITE_HARDWARE_UCHAR(DeviceData,
                         CIRCC2_MASTER_BLOCK_CONTROL,
                         mbc.AsUChar);

    
    //
    // Turn on the receiver
    //
    lcb.AsUChar = 0;
    lcb.SCEModeBits = SCE_MODE_RECEIVE;
    WRITE_HARDWARE_UCHAR(DeviceData,
                         CIRCC2_LINE_CONTROL_B,
                         lcb.AsUChar);


    WdfInterruptReleaseLock(DeviceData->Interrupt);


    //
    // Kill the pending receive queue...
    //
    SmscIrTracePrint(
            TRACE_LEVEL_VERBOSE,
            SMSCDBG_IOCTL_INFO,
            ("IrExitPriorityMode: Purging priority queue"));

    WdfIoQueuePurge(DeviceData->PriorityReceiver.PendingReceiveQueue,
                    NULL,
                    NULL);


    KeAcquireSpinLock(&DeviceData->PriorityReceiver.ListSpinLock, &oldIrql);

    activeRequest = DeviceData->PriorityReceiver.CurrentIrReceiveRequest;

    DeviceData->PriorityReceiver.CurrentIrReceiveRequest = NULL;

    if (activeRequest) {
    
        status = WdfRequestUnmarkCancelable(activeRequest);

        if (status == STATUS_CANCELLED) {
            //
            // Cancel got here first...
            //
            activeRequest = NULL;
        
        }

    }

    //
    // Reset the circular buffer in the receiver
    //
    SMSCIR_RESET_RECEIVER(&DeviceData->PriorityReceiver);

    KeReleaseSpinLock(&DeviceData->PriorityReceiver.ListSpinLock, oldIrql);

    if (activeRequest) {

        SmscIrTracePrint(
                TRACE_LEVEL_VERBOSE,
                SMSCDBG_IOCTL_INFO,
                ("IrExitPriorityMode: Completing active request 0x%p",
                    activeRequest));

        WdfRequestComplete(activeRequest, STATUS_CANCELLED);

    }

    //
    // Re-enable the manual priority receive queue. 
    //
    WdfIoQueueStart(DeviceData->PriorityReceiver.PendingReceiveQueue);

    //
    // Restore the timeout...
    //
    DeviceData->NumFFsForDataEnd = SMSCIR_NUM_FFS_FOR_DATA_END;

    WdfRequestComplete(Request, STATUS_SUCCESS);


    SmscIrTracePrint(
            TRACE_LEVEL_VERBOSE,
            SMSCDBG_IOCTL_INFO,
            ("IrExitPriorityMode: Exited"));

    return;

}


VOID
IrGetEmitters(
    PSMSCIR_DATA DeviceData,
    WDFREQUEST Request, 
    SIZE_T OutputBufferLength
    ) {

    /*++
Routine Description.

    Gets attached emitters and returns the information in a bitmask.  

Arguments:

    DeviceData - Our device extension

    Request    - An IOCTL_IR_GET_EMITTERS request

    OutputBufferLength - sizeof(ULONG)

--*/
    NTSTATUS status;
    PULONG   emitterMask;
    BIRCC2_TX_CHANNEL_ENABLES txChannelEnables;

    SmscIrTracePrint(
            TRACE_LEVEL_VERBOSE,
            SMSCDBG_IOCTL_INFO,
            ("IrGetEmitters: Entered"));


    if (OutputBufferLength < sizeof(ULONG)) {

        SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_IOCTL_INFO,
                    ("Invalid buffer size passed. Must be >= sizeof(ULONG)"));
        WdfRequestWdmGetIrp(Request)->IoStatus.Status 
                                        = STATUS_BUFFER_TOO_SMALL;
        WdfRequestCompleteWithInformation(Request,
                                          STATUS_BUFFER_TOO_SMALL,
                                          sizeof(ULONG));
        return;

    }

    status = WdfRequestRetrieveOutputBuffer(Request,
                                            sizeof(ULONG),
                                            &emitterMask,
                                            NULL);

    if (!NT_SUCCESS(status)) {

        SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_IOCTL_INFO,
            ("WdfRequestRetrieveOutputBuffer failed with Status code 0x%x", status));
        WdfRequestComplete(Request, status);
        return;

    }

    //
    // We need to read some registers, so we need to lock against
    //  anyone else touching the registers.
    //
    WdfInterruptAcquireLock(DeviceData->Interrupt);

    //
    // Find the attached emitters
    //
    txChannelEnables.AsUChar 
        = READ_TRANSMIT_UCHAR(DeviceData,
                              BIRCC2_TX_CHANNEL_ENABLES_ADDR);

    WdfInterruptReleaseLock(DeviceData->Interrupt);
        

    //
    // The high four bits are the emitter mask
    //
    *emitterMask = (txChannelEnables.AsUChar >> 4);

    SmscIrTracePrint(
            TRACE_LEVEL_INFORMATION,
            SMSCDBG_IOCTL_INFO,
            ("IrGetEmitters: Emitter mask is 0x%x", *emitterMask));


    WdfRequestCompleteWithInformation(Request, status,
                                      sizeof(ULONG));

    SmscIrTracePrint(
            TRACE_LEVEL_VERBOSE,
            SMSCDBG_IOCTL_INFO,
            ("IrGetEmitters: Exited"));


    return;
}

VOID
IrFlashReceiver(
    PSMSCIR_DATA DeviceData,
    WDFREQUEST Request, 
    SIZE_T InputBufferLength
    ) {
/*++
Routine Description.

    Flash an LED on the given receiver.  Used to tell the user where to point 
    their remote, so a given "receiver box" with multiple receiver parts only 
    needs one LED to flash.

Arguments:

    DeviceData - Our device extension

    Request    - An IOCTL_IR_FLASH_EMITTERS request

    InputBufferLength  - sizeof(ULONG)

--*/

    UNREFERENCED_PARAMETER(DeviceData);
    UNREFERENCED_PARAMETER(InputBufferLength);

    //
    // Not supported by our devices
    //
    SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_IOCTL_INFO,
            ("Flashing the receiver not supported on hardware"));
    WdfRequestComplete(Request, STATUS_NOT_SUPPORTED);

}


VOID
IrResetDevice(
    PSMSCIR_DATA DeviceData,
    WDFREQUEST Request
    ) {
/*++
Routine Description.

    Resets the given device.  When a device is reset, all pending transmit 
    and receive IOCTLs are cancelled by the port driver.

Arguments:

    DeviceData - Our device extension

    Request    - An IOCTL_IR_RESET_DEVICE request

--*/
    MASTER_BLOCK_CONTROL mbc;
    LINE_CONTROL_B       lcb;
    LINE_CONTROL_A         lca;
    WDFREQUEST           activePriorityRx;
    WDFREQUEST           activeRx;
    WDFREQUEST           activeTx;
    KIRQL                oldIrql;
    PSMSCIR_TX_FIFO_DATA txFifoData = NULL;
    NTSTATUS             status;

    SmscIrTracePrint(
            TRACE_LEVEL_VERBOSE,
            SMSCDBG_IOCTL_INFO,
            ("IrResetDevice: Entered"));



    //
    // For a reset, we'll purge all of our pending receive
    //  queues, reset any errors, and toggle the receiver.
    //

    //
    // Purging will drain the queue and cancel any incoming 
    //  I/O automatically
    //
    WdfIoQueuePurge(DeviceData->PriorityReceiver.PendingReceiveQueue, 
                    NULL, 
                    NULL);
    WdfIoQueuePurge(DeviceData->Receiver.PendingReceiveQueue, 
                    NULL, 
                    NULL);

    //
    // Reset the device...
    //
    SmscIrTracePrint(
            TRACE_LEVEL_VERBOSE,
            SMSCDBG_IOCTL_INFO,
              ("IrResetDevice: Toggling master reset"));


    //
    // Synchronize against the ISR
    //
    WdfInterruptAcquireLock(DeviceData->Interrupt);

    mbc.AsUChar = 0;

    //
    // Put it into reset mode in order to clear any FIFO overrun
    //  messages we might have accumulated...Turn off interrupts
    //  while we're at it...
    //
    mbc.ErrorReset = 1;
    mbc.MasterInterruptEnable = 0;
    WRITE_HARDWARE_UCHAR(DeviceData,
                         CIRCC2_MASTER_BLOCK_CONTROL,
                         mbc.AsUChar);

    //
    // Dump anything in the FIFO
    //
    lca.AsUChar = 0;
    lca.FifoReset = TRUE;
    WRITE_HARDWARE_UCHAR(DeviceData,
                         CIRCC2_LINE_CONTROL_A,
                         lca.AsUChar);

    //
    // Turn off the receiver...
    //
    lcb.AsUChar = 0;
    WRITE_HARDWARE_UCHAR(DeviceData,
                         CIRCC2_LINE_CONTROL_B,
                         lcb.AsUChar);


    //
    // Clear the reset and re-enable interrupts.
    //
    mbc.AsUChar = 0;
    mbc.MasterInterruptEnable = 1;
    WRITE_HARDWARE_UCHAR(DeviceData,
                         CIRCC2_MASTER_BLOCK_CONTROL,
                         mbc.AsUChar);

    //
    // Turn it back on...
    //
    lcb.AsUChar = SCE_MODE_RECEIVE;
    WRITE_HARDWARE_UCHAR(DeviceData,
                         CIRCC2_LINE_CONTROL_B,
                         lcb.AsUChar);


    WdfInterruptReleaseLock(DeviceData->Interrupt);

    // 
    // Abort any in progress I/O
    // 
    KeAcquireSpinLock(&DeviceData->Receiver.ListSpinLock, &oldIrql);
    KeAcquireSpinLockAtDpcLevel(&DeviceData->PriorityReceiver.ListSpinLock);
    KeAcquireSpinLockAtDpcLevel(&DeviceData->TxFifoDataLock);

    activePriorityRx = DeviceData->PriorityReceiver.CurrentIrReceiveRequest;

    DeviceData->PriorityReceiver.CurrentIrReceiveRequest = NULL;

    if (activePriorityRx) {
        status = WdfRequestUnmarkCancelable(activePriorityRx);

        if (status == STATUS_CANCELLED) {

            //
            // Too late, already cancelled...
            //
            activePriorityRx = NULL;
        
        }

    }

    activeRx = DeviceData->Receiver.CurrentIrReceiveRequest;

    DeviceData->Receiver.CurrentIrReceiveRequest = NULL;

    if (activeRx) {
        status = WdfRequestUnmarkCancelable(activeRx);
        if (status == STATUS_CANCELLED) {

            //
            // Too late, already cancelled...
            //
            activeRx = NULL;

        }
    }


    //
    // We don't mark our TX as cancellable, so no need to
    //  unmark...
    //
    activeTx = DeviceData->CurrentTxRequest;

    DeviceData->CurrentTxFifoData = NULL;
    DeviceData->CurrentTxRequest  = NULL;

    //
    // Need to clean out the list of remaing FIFO 
    //  data.
    //
    while (!IsListEmpty(&DeviceData->TxFifoDataList)) {

        txFifoData 
                = (PSMSCIR_TX_FIFO_DATA)
                        RemoveHeadList(&DeviceData->TxFifoDataList);

        ExFreePool(txFifoData);

    }


    KeReleaseSpinLockFromDpcLevel(&DeviceData->TxFifoDataLock);
    KeReleaseSpinLockFromDpcLevel(&DeviceData->PriorityReceiver.ListSpinLock);
    KeReleaseSpinLock(&DeviceData->Receiver.ListSpinLock, oldIrql);

    if (activePriorityRx) {

        SmscIrTracePrint(
                TRACE_LEVEL_VERBOSE,
                SMSCDBG_IOCTL_INFO,
                ("IrResetDevice: Cancelling active priority request 0x%p",
                    activePriorityRx));

        WdfRequestComplete(activePriorityRx, STATUS_CANCELLED);

    }

    if (activeRx) {

        SmscIrTracePrint(
                TRACE_LEVEL_VERBOSE,
                SMSCDBG_IOCTL_INFO,
                ("IrResetDevice: Cancelling active receive request 0x%p",
                    activeRx));

        WdfRequestComplete(activeRx, STATUS_CANCELLED);

    }

    if (activeTx) {

        SmscIrTracePrint(
                TRACE_LEVEL_VERBOSE,
                SMSCDBG_IOCTL_INFO,
                ("IrResetDevice: Cancelling active transmit request 0x%p",
                    activeTx));

        WdfRequestComplete(activeTx, STATUS_CANCELLED);

    }


    //
    // Restart the queues.
    //
    WdfIoQueueStart(DeviceData->PriorityReceiver.PendingReceiveQueue);
    WdfIoQueueStart(DeviceData->Receiver.PendingReceiveQueue);

    WdfRequestComplete(Request, STATUS_SUCCESS);

    SmscIrTracePrint(
            TRACE_LEVEL_VERBOSE,
            SMSCDBG_IOCTL_INFO,
            ("IrResetDevice: Exited"));


}


PCCHAR 
IoctlToString(
    ULONG IoControlCode
    ) {

    switch (IoControlCode) {

        case IOCTL_IR_HANDSHAKE: {

            return "IOCTL_IR_HANDSHAKE";

        }

        case IOCTL_IR_TRANSMIT: {

            return "IOCTL_IR_TRANSMIT";

        }

        case IOCTL_IR_RECEIVE: {

            return "IOCTL_IR_RECEIVE";

        }

        case IOCTL_IR_PRIORITY_RECEIVE: {

            return "IOCTL_IR_PRIORITY_RECEIVE";

        }

        case IOCTL_IR_GET_DEV_CAPS: {

            return "IOCTL_IR_GET_DEV_CAPS";

        }

        case IOCTL_IR_FLASH_RECEIVER: {

            return "IOCTL_IR_FLASH_RECEIVER";

        }

        case IOCTL_IR_RESET_DEVICE: {

            return "IOCTL_IR_RESET_DEVICE";

        }

        case IOCTL_IR_ENTER_PRIORITY_RECEIVE: {

            return "IOCTL_IR_ENTER_PRIORITY_RECEIVE";

        }
        case IOCTL_IR_EXIT_PRIORITY_RECEIVE: {

            return "IOCTL_IR_EXIT_PRIORITY_RECEIVE";

        }
        case IOCTL_IR_USER_OPEN: {

            return "IOCTL_IR_USER_OPEN";

        }
        case IOCTL_IR_USER_CLOSE: {

            return "IOCTL_IR_USER_CLOSE";

        }
        case IOCTL_IR_GET_EMITTERS: {

            return "IOCTL_IR_GET_EMITTERS";

        }
        default: {

            break;


        }

    }

    return "UNKNOWN IOCTL";

}

