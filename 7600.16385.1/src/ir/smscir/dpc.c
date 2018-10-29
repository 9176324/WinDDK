//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Copyright 2005 OSR, Open Systems Resources, Inc. All rights Reserved.
// 
// Module Name:
// 
//     dpc.c
// 
// Abstract:
//
//     Contains routines for  DPC processing and RLC coding
// 
// Author:
//
// Revision History:
//      
#include "smscir.h"

#ifdef WPP_TRACING
#include "dpc.tmh"
#endif


//
// Forward declarations
//
VOID
SmscirEncodeAndEnqueueRLC(
    IN PSMSCIR_RLC_RECEIVER RLCReceiver,
    __in_bcount(DataLen) PCHAR Buffer,
    IN ULONG DataLen,
    IN BOOLEAN DataEnd
    );


NTSTATUS 
IrSetupCurrentReceive(
    IN PSMSCIR_RLC_RECEIVER RLCReceiver,
    IN WDFREQUEST ReceiveRequest
    );

NTSTATUS 
IrSetupCurrentPriorityReceive(
    IN PSMSCIR_RLC_RECEIVER PriorityRLCReceiver,
    IN WDFREQUEST PriorityReceiveRequest
    );

VOID
SmscIrEvtInterruptDpc(
    IN WDFINTERRUPT Interrupt,
    IN WDFOBJECT    Object
    ) {
/*++

Routine Description.

    The DpcForIsr. This routine's job is to take the 
    FIFO data that was enqueued in the ISR, RLC it,
    then attempt to complete any pending IR receive
    requests.

Arguments:

    Interrupt       - Our interrupt object

    MessageID       - For MSI interrupts. Not used.

--*/

    PSMSCIR_DATA         deviceData;
    UCHAR                rxBuffer[SMSCIR_MAX_LOCAL_BUFFER_SIZE];
    ULONG                numLoops = 0;
    BOOLEAN              dataEnd;
    ULONG                amountRxData;
    KIRQL                oldIrql;
    PSMSCIR_RLC_RECEIVER rlcReceiver;
    WDFREQUEST           successfulRequest;
    WDFREQUEST           failedRequest;
    NTSTATUS             failureStatus;
    ULONG                remainingRxBytes;
    ULONG                maxLoops;
    WDFREQUEST           txRequest = NULL;
    NTSTATUS             txRequestStatus = STATUS_SUCCESS;
    BOOLEAN              txInterrupt;

    UNREFERENCED_PARAMETER(Object);
    
    SmscIrTracePrint(
        TRACE_LEVEL_VERBOSE,
        SMSCDBG_RECEIVE_INFO,
        ("SmscIrEvtInterruptDpc: Entered"));

   
    deviceData = (PSMSCIR_DATA)GetIrData(WdfInterruptGetDevice(Interrupt));

    //
    // Get a pointer to the appropriate receiver
    //
    rlcReceiver = deviceData->InPriorityMode ? 
                    &deviceData->PriorityReceiver : &deviceData->Receiver;

    //
    // We have to get hold of the lock immediately because 
    //  we may be racing with other DPCs on other processors.
    //  If we don't hold the lock across the FIFO dequeue and 
    //  RLC encoding/enqueueing we could end up enqueueing 
    //  the RLC data out of order
    //
    KeAcquireSpinLock(&rlcReceiver->ListSpinLock, &oldIrql);

    //
    // Acquire the TX lock as we need to check if we're blasting
    //
    KeAcquireSpinLockAtDpcLevel(&deviceData->TxFifoDataLock);

    //
    // Loop some reasonable amount of times pulling FIFO data
    //  off of the FIFO queue and then RLC'ing it and putting
    //  it on the RLC queue. 
    //
    maxLoops = 2;

    do {

        numLoops++;


        //
        // The ISR enqueues the data, so we need to serialize against it.
        //  We'll also check for a TX interrupt while we have the 
        //  lock (we keep a shadow copy of the TX interrupt register)
        //
        WdfInterruptAcquireLock(deviceData->Interrupt);


        amountRxData = SmscIrDequeueFifoData(deviceData,
                                             (PUCHAR)&rxBuffer[0],
                                             sizeof(rxBuffer),
                                             &dataEnd);

        //
        // Because DPCs are not 1:1 with ISRs (multiple calls to
        //  WdfInterruptQueueDpcForIsr while the DPC is queued
        //  are no-ops). we need to watch for the case where 
        //  the last ISR fires, the DPC is already queued, and
        //  our loops here leave some number of remaining bytes
        //  on the FIFO queue**. Because we won't get another 
        //  DPC until the next ISR, we may leave some orphaned
        //  bytes here if we don't watch out for it. So, basically,
        //  we might make an extra loop to clean up some spare bytes
        //  sitting at the end of the queue. This may be unnecessary,
        //  we have know way of knowing if this DPC is going to
        //  fire again OR sitting on another proc, but better to be safe 
        //  than sorry.
        //
        // **Note that due to the numbers chosen (2 * FIFO_SIZE for
        //  the local buffer and 2 loops default) the odds of this
        //  happening are slim. The ISR would have to fire 5 times
        //  before the first DPC is satisfied (each ISR enqueues a 
        //  max of FIFO_SIZE bytes, this DPC can dequeue 4 * FIFO_SIZE 
        //  bytes per run). But, just cuz it isn't likely doesn't
        //  mean that it won't happen - with possibly hideous user
        //  experience results (imagine missing some bytes that 
        //  make up a data end, it will look like the user is 
        //  holding down the button even though they have let it go). 
        //
        remainingRxBytes = SmscIrFifoBytesRemaining(deviceData);

        if ((remainingRxBytes && 
             remainingRxBytes < SMSCIR_FIFO_SIZE)) {

            //
            // We won't go overboard here, just make one more pass. 
            //  The beauty is that if on the next loop we don't 
            //  capture all of the remaining data we know that 
            //  the ISR has fired and added more data to the FIFO, 
            //  in which case another DPC has been requested while our DPC
            //  is running and we'll be back in here again shortly...
            //
            maxLoops = 3;
        }

        //
        // Check for a TX interrupt in the shadow copy.
        //
        if (deviceData->TxIntId.Fifo) {

            //
            // Clear it in our copy.
            //
            deviceData->TxIntId.Fifo = FALSE;

            txInterrupt = TRUE;

        } else {

            txInterrupt = FALSE;

        }

        WdfInterruptReleaseLock(deviceData->Interrupt);


        //
        // Check for Tx processing to do  and skip if 
        //  we've already gotten a TX request
        //  to complete on a previous loop (if there's
        //  another request to be blasted we'll get it at 
        //  a later time)
        //
        if (txInterrupt && !txRequest) {

            //
            // TX processing to do
            //
            SmscIrContinueBlasting(deviceData,
                                   &txRequest,
                                   &txRequestStatus);
        }

        if (amountRxData) {

            //
            // RX processing to do
            //
            SmscirEncodeAndEnqueueRLC(rlcReceiver,
                                      (PCHAR)rxBuffer,
                                      amountRxData,
                                      dataEnd);

        }


    } while (numLoops < maxLoops);

    KeReleaseSpinLockFromDpcLevel(&deviceData->TxFifoDataLock);

    
    //
    // Have the helper routine grab the next pending
    //  receive, copy data into it, and then return
    //  it to us if we need to complete it. 
    //
    // This routine also dequeues the next request
    //  and tries to set it up as the current request.
    //  If that operation fails for some reason (invalid
    //  parameters in the next request, e.g.) we may
    //  also be passed a request to fail...
    //
    SmscIrProcessPendingReceives(rlcReceiver,
                                 &successfulRequest,
                                 &failedRequest,
                                 &failureStatus);


    KeReleaseSpinLock(&rlcReceiver->ListSpinLock, oldIrql);


    if (successfulRequest) {

        SmscIrTracePrint(
            TRACE_LEVEL_INFORMATION,
            SMSCDBG_RECEIVE_INFO,
            ("SmscIrEvtInterruptDpc: Completing RX request 0x%p",
                successfulRequest));

        WdfRequestCompleteWithPriorityBoost(successfulRequest, 
                                            STATUS_SUCCESS,
                                            IO_KEYBOARD_INCREMENT);
                
    }

    if (failedRequest) {
        
        SmscIrTracePrint(
            TRACE_LEVEL_INFORMATION,
            SMSCDBG_RECEIVE_INFO,
            ("SmscIrEvtInterruptDpc: Failing RX request 0x%p",
                successfulRequest));

        WdfRequestComplete(failedRequest, failureStatus);

    }

    if (txRequest) {

        //
        // Stop the dead man timer
        //
        SmscIrTracePrint(
            TRACE_LEVEL_INFORMATION,
            SMSCDBG_TRANSMIT_INFO,
            ("SmscIrEvtInterruptDpc: Completing TX request 0x%p",
                successfulRequest));
        WdfTimerStop(deviceData->DeadManTimer, FALSE);
        WdfRequestComplete(txRequest, txRequestStatus);

    }
   
    SmscIrTracePrint(
        TRACE_LEVEL_VERBOSE,
        SMSCDBG_RECEIVE_INFO,
        ("SmscIrEvtInterruptDpc: Exited"));

    return;

}


VOID
SmscirEncodeAndEnqueueRLC(
    IN PSMSCIR_RLC_RECEIVER RLCReceiver,
    __in_bcount(DataLen) PCHAR Buffer,
    IN ULONG DataLen,
    IN BOOLEAN DataEnd
    ) {

/*++

Routine Description:

    This routine takes the raw data pulled from the 
    device's FIFO and turns it into an array of
    on/off samples in 50usec units. It then 
    puts this data into a queue to be picked up
    by the class driver.
    
    RLCReceiver is *LOCKED* by caller on entry 

Arguments:

    RLCReceiver - The holder for either regular IR 
                    receivers or priority IR receivers

    Buffer        - The FIFO data teken from the device
             
    MaxLen        - Size of the Buffer parameter, in bytes

    DataEnd       - Is this a data end?

Return Value:

--*/

    ULONG       resultLen;
    PLONG       resultPtr;
    LONG        lastRLCPiece;
    ULONG       samples = 0;
    ULONG       i;
    LONG        bitsConsumed;
    PSAMPLE_RUN curSample;
    BOOLEAN     lastSampleOn;
    
    //
    // Make sure we're not going to overrun
    //  our scratch buffer (which is created to accomodate
    //  a worst case scenario of SMSCIR_MAX_LOCAL_BUFFER_SIZE
    //  samples...)
    //
    ASSERT(DataLen <= SMSCIR_MAX_LOCAL_BUFFER_SIZE);

    //
    // We're going to do a bit of a trick here. 
    //
    // We always save the last piece of the RLC calculation, 
    //  because the next piece that we get may be a continuation of it.
    //  When we get the next RLC calculation, we look at the last piece 
    //  of the previous calculation and decide if it's a continuation 
    //  or a new piece. If it's a continuation, we'll add it to the 
    //  first piece of the new calculation and go on as usual. However, 
    //  if it not a continuation, we'll stick the last piece of the 
    //  last calculation in RLCResultBuffer[0] and give that to the class
    //  driver to process. 
    //
    // We're called here under lock and we copy the data out of this
    //  scratch buffer into a real location, so we can safely use this
    //  one place for calcs without worrying about contention
    //
    resultPtr = &RLCReceiver->DeviceData->RLCResultBuffer[1];
    
    //
    // Get the state of the first sample and invert it. This will
    //  set things up for the first pass in the loop
    //
    lastSampleOn = !SmscIrSampleTable[(UCHAR)Buffer[0]]->On;

    for (i = 0; i < DataLen; i++) {

        //
        // For a long, overly winded description of what we're
        //  doing here, check out runtables.c
        //
        curSample = (PSAMPLE_RUN)&SmscIrSampleTable[(UCHAR)Buffer[i]];


        //
        // Every sample is 8 bits wide, i.e. one UCHAR. 
        //
        bitsConsumed = 8;

        //
        // While we haven't processed all of the bits in this
        //  sample...
        //
        while (bitsConsumed > 0) {

            if (lastSampleOn == curSample->On) {

                //
                // This is the previous on or off sample
                //  overflowing into the next byte.
                //
                // Add the current sample value into the
                //  previous result. Note that this will ONLY
                //  happen when bitsConsumed == 8. What it means
                //  is that we've overflown from the last bits
                //  of the previous byte into the first bits
                //  of this byte.
                //
                ASSERT(bitsConsumed == 8);
                resultPtr[samples-1] += curSample->Result;

            } else {

                resultPtr[samples++] = curSample->Result;

            }

            //
            // Subtract the number of bits that this run
            //  consumed from the sample...
            //
            bitsConsumed -= curSample->BitsConsumed;
            lastSampleOn = curSample->On;
            curSample++;

        }

    }


    //
    // Store away the last piece of this
    //  report...
    //
    lastRLCPiece = resultPtr[samples-1];
    
    //
    // We possibly won't be passing that to the class driver...
    //
    resultLen = ((samples-1) * sizeof(LONG));
    

    //
    // Was the last piece of the previous calculation
    //  the same sign as the first piece of this 
    //  report?
    //
    if ((RLCReceiver->LastRLCPieceOfPreviousPacket < 0 && resultPtr[0] < 0) ||
        (RLCReceiver->LastRLCPieceOfPreviousPacket > 0 && resultPtr[0] > 0)) {
        
        //
        // join pieces
        //
        resultPtr[0] += RLCReceiver->LastRLCPieceOfPreviousPacket;

        //
        // Watch out for the case where we only had one sample. If
        //  we did then the last RLC piece needs to be modified
        //
        if (samples == 1) {
            lastRLCPiece = resultPtr[0];
        }

    } else if (RLCReceiver->LastRLCPieceOfPreviousPacket) {
        
        //
        // Nope, it was different. We need to 
        //  adjust the result pointer to be the 
        //  beginning of the buffer, give the last
        //  piece of the previous calculation to the user,
        //  and bump the amount of bytes we'll be sending...
        //
        // Note that this does not take into account the
        //  "lastRLCPiece" that we've saved off above, that's
        //  the last piece of THIS packet (which we may add 
        //  back in if this is a data end)
        //
        resultPtr = &RLCReceiver->DeviceData->RLCResultBuffer[0];
        resultPtr[0] = RLCReceiver->LastRLCPieceOfPreviousPacket;
        resultLen += sizeof(LONG);
        
    }

    //
    // For some unknown reason, we get a 50 uSec period of
    // silence from the SIO1049 the first time we turn on the 
    // learning receiver.  If we hit this, skip it.
    //
    if ( RLCReceiver->IgnoreFirstSilence &&
            resultLen >= sizeof(LONG) ) {

        
        if ( resultPtr[0] < 0 ) {
            resultPtr++;
            resultLen -= sizeof(LONG);
            }
        //
        // Turn off this flag even if we don't get the silence.
        // We're trying to skip the _first_ period of silence, 
        // not every period of silence.
        //
        RLCReceiver->IgnoreFirstSilence = FALSE;
        
        }
    
    if (!DataEnd) {

        //
        // Not done yet? Store away the last piece, we'll get to it in the 
        //  next pass.
        //
        RLCReceiver->LastRLCPieceOfPreviousPacket = lastRLCPiece;

    } else {

        //
        // DONE! Up the count of samples taken, this causes it to 
        //  include the lastRLCPiece in our return...
        //
        resultLen += sizeof(LONG);

        //
        // Start over...
        //
        RLCReceiver->LastRLCPieceOfPreviousPacket = 0;



    }


    SmscIrEnqueueRLCData(RLCReceiver,
                         (PUCHAR)resultPtr,
                         resultLen,
                         DataEnd);


}

VOID 
__forceinline
SmscIrEnqueueRLCData(
    PSMSCIR_RLC_RECEIVER RLCReceiver,
    __in_bcount(Length) PUCHAR RLCData,
    ULONG Length,
    IN BOOLEAN DataEnd
    ) {

/*++

Routine Description:

    This routine puts the given RLC data into the 
    circular buffer that the class driver requests
    are satisfied from

    RLCReceiver is *LOCKED* by caller on entry 

Arguments:

    RLCReceiver - The holder for either regular IR 
                    receivers or priority IR receivers

    RLCData        - The RLC data
             
    ReportLength  - Number of bytes to enqueue
    
    DataEnd       - Is this a data end?

--*/
    ULONG amountToCopy;

    ASSERT(Length < RLC_RECEIVER_BUFFER_LENGTH);

    if (!RLCReceiver->OpenCount) {
        return;
    }

    RLCReceiver->CurrentBufferSize += Length;
    
    if (RLCReceiver->NextIndex+Length > RLC_RECEIVER_BUFFER_LENGTH) {
        // Wrap past end of buffer
        RtlCopyMemory(RLCReceiver->RLCBuffer + RLCReceiver->NextIndex,
                      RLCData,
                      RLC_RECEIVER_BUFFER_LENGTH - RLCReceiver->NextIndex);
        //
        // Prefast will complain about this calculation, but it is 
        //  correct.
        //
        amountToCopy = (RLC_RECEIVER_BUFFER_LENGTH - RLCReceiver->NextIndex);
#pragma prefast (suppress:412, "Calculation is correct and as designed")
        RtlCopyMemory(RLCReceiver->RLCBuffer,
                      RLCData+amountToCopy,
                      Length-amountToCopy);
        RLCReceiver->NextIndex = (Length - amountToCopy);
    } else {
        RtlCopyMemory(RLCReceiver->RLCBuffer + RLCReceiver->NextIndex, 
                      RLCData,
                      Length);
        RLCReceiver->NextIndex = 
            (RLCReceiver->NextIndex + Length == RLC_RECEIVER_BUFFER_LENGTH) ?
                                             0 : RLCReceiver->NextIndex+Length;
    }

    if (RLCReceiver->CurrentBufferSize > RLC_RECEIVER_BUFFER_LENGTH) {
        RLCReceiver->CurrentBufferSize = RLC_RECEIVER_BUFFER_LENGTH;
        RLCReceiver->CurrentIndex = (RLCReceiver->NextIndex != 0) ?
                                              RLCReceiver->NextIndex-1 :
                                                  RLC_RECEIVER_BUFFER_LENGTH-1;
    }

    //
    // If this is a data end, then we'll set the watermark
    //  for the data end event. When the dequeue code comes
    //  in it will use this to know how to mark the 
    //  DataEnd member of the RECEIVE IOCTL.
    //
    if (DataEnd) {

        if (RLCReceiver->BytesToDataEnd) {
            SmscIrTracePrint(
                    TRACE_LEVEL_INFORMATION,
                    SMSCDBG_RECEIVE_INFO,
                    ("SmscIrEnqueueRLCData: DataEnd was already present. "
                     "Previous KeyUp missed"));
        }

        RLCReceiver->BytesToDataEnd = RLCReceiver->CurrentBufferSize;

    }

}


ULONG 
SmscIrDequeueRLCData(
    PSMSCIR_RLC_RECEIVER RLCReceiver,
    __out_bcount(MaxLen) PUCHAR Buffer,
    ULONG MaxLen,
    __out PBOOLEAN DataEnd
    ) {
/*++

Routine Description:

    This routine pulls RLC data out of the given receiver
    and stores it in the given buffer

    RLCReceiver is *LOCKED* by caller on entry 

Arguments:

   
    RLCReceiver - The holder for either regular IR 
                    receivers or priority IR receivers

    Buffer - Buffer to copy the data into
             
    MaxLen - Size of the Buffer parameter, in bytes

    DataEnd - Is this a data end?

Return Value:

    Actual number of bytes copied

--*/
    ULONG bufferSize;
    ULONG amountToCopy;
#if DBG
    ULONG prevValue;
#endif

    *DataEnd = FALSE;

    if (0 == RLCReceiver->CurrentBufferSize){
        return 0;
    } else {


        if (RLCReceiver->BytesToDataEnd) {
            bufferSize = RLCReceiver->BytesToDataEnd;
        } else {
            bufferSize = RLCReceiver->CurrentBufferSize;
        }

        amountToCopy = bufferSize < MaxLen ? bufferSize : MaxLen;

        if ((RLCReceiver->CurrentIndex + amountToCopy) 
                                              > RLC_RECEIVER_BUFFER_LENGTH) {
            ULONG amountToCopyFirst 
                    = RLC_RECEIVER_BUFFER_LENGTH - RLCReceiver->CurrentIndex;
            
            RtlCopyMemory(Buffer, 
                          RLCReceiver->RLCBuffer + RLCReceiver->CurrentIndex,
                          amountToCopyFirst);
            //
            // Prefast will complain about this calculation, but it is 
            //  correct.
            //
#pragma prefast (suppress:411, "Calculation is correct and as designed")
            RtlCopyMemory(Buffer + amountToCopyFirst, 
                          RLCReceiver->RLCBuffer,
                          amountToCopy - amountToCopyFirst);
            RLCReceiver->CurrentIndex = amountToCopy - amountToCopyFirst;
        } else {
            RtlCopyMemory(Buffer, 
                          RLCReceiver->RLCBuffer + RLCReceiver->CurrentIndex,
                          amountToCopy);
            RLCReceiver->CurrentIndex = 
                ((RLCReceiver->CurrentIndex + amountToCopy) 
                              == RLC_RECEIVER_BUFFER_LENGTH) ? 0 : 
                                      RLCReceiver->CurrentIndex + amountToCopy;
        }

        if (RLCReceiver->BytesToDataEnd) {
            
#if DBG
            prevValue = RLCReceiver->BytesToDataEnd;
#endif

            RLCReceiver->BytesToDataEnd -= amountToCopy;

#if DBG
            //
            // Make sure we didn't wrap under...
            //
            ASSERT(RLCReceiver->BytesToDataEnd < prevValue);
#endif

            *DataEnd = (BOOLEAN)(RLCReceiver->BytesToDataEnd == 0);

        }

#if DBG
        prevValue = RLCReceiver->CurrentBufferSize;
#endif
        RLCReceiver->CurrentBufferSize -= amountToCopy;

#if DBG
        //
        // Make sure we didn't wrap under...
        //
        ASSERT(RLCReceiver->CurrentBufferSize < prevValue);
#endif

        return amountToCopy;
    }
}

NTSTATUS
IrSetupNextRequest(
    IN PSMSCIR_RLC_RECEIVER RLCReceiver,
    OUT WDFREQUEST *CompleteWithFailure,
    OUT PNTSTATUS FailureStatus
    ) {
/*++

Routine Description:

    This routine dequeues the next pending request from the receiver
    queue and makes it the active/current request

    RLCReceiver is *LOCKED* by caller on entry 

Arguments:

    RLCReceiver         - Circular buffer for this IR data

    CompleteWithFailure - In case of error, the request to complete 
                          with *FailureStatus by the caller once the 
                          lock is dropped

    FailureStatus       - If *CompleteWithFailure != NULL, failure
                          status of request.

--*/
    NTSTATUS   status;
    WDFREQUEST nextRequest;

    status = WdfIoQueueRetrieveNextRequest(RLCReceiver->PendingReceiveQueue,
                                           &nextRequest);

    if (NT_SUCCESS(status)) {
                
        //
        // We have to get slightly different parameters
        //  based on this being a priority IR data buffer or not
        //
        if (!RLCReceiver->IsPriorityReceiver) {
                
            status = IrSetupCurrentReceive(RLCReceiver,
                                           nextRequest);
        } else {

            status = IrSetupCurrentPriorityReceive(RLCReceiver,
                                                   nextRequest);

        }


        if (!NT_SUCCESS(status)) {
                    
            SmscIrTracePrint(
                    TRACE_LEVEL_ERROR,
                    SMSCDBG_RECEIVE_INFO,
                    ("IrSetupNextRequest: Could not setup 0x%p as "\
                     "the next request - 0x%x", 
                        nextRequest, status));

            *CompleteWithFailure = nextRequest;
            *FailureStatus = status;
                
            SmscIrTracePrint(
                    TRACE_LEVEL_ERROR,
                    SMSCDBG_RECEIVE_INFO,
                    ("IrSetupNextRequest: Failing request 0x%p", 
                        nextRequest));
                    
        }
            
    } else {

        SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_RECEIVE_INFO,
            ("WdfIoQueueRetrieveNextRequest failed - 0x%x", status));

    }

    return status;

}


VOID
SmscIrProcessPendingReceives(
    IN PSMSCIR_RLC_RECEIVER RLCReceiver,
    OUT WDFREQUEST *CompleteWithSuccess,
    OUT WDFREQUEST *CompleteWithFailure,
    __out PNTSTATUS FailureStatus
    ) {
/*++

Routine Description:

    This routine copies the IR report given into any requests
    that are waiting for IR data in the given FILE_IR_DATA 
    structure.

    The current reader stays active until its data buffer is
    exhausted or DataEnd is TRUE, in which case it is completed
    immediately

    RLCReceiver is *LOCKED* by caller on entry 

Arguments:

    RLCReceiver - Circular buffer for this IR data

    CompleteWithSuccess - Request to complete with STATUS_SUCCESS 
                          by the caller once the lock is dropped
    
    CompleteWithFailure - Request to complete with *FailureStatus
                          by the caller once the lock is dropped

    FailureStatus       - If *CompleteWithFailure != NULL, failure
                          status of request.

--*/
    WDFREQUEST             successfulRequest = NULL;
    WDFREQUEST             failedRequest = NULL;
    NTSTATUS               failureStatus = STATUS_UNSUCCESSFUL;
    ULONG                  amountCopied;
    NTSTATUS               status;
    ULONG                  userBufferRemaining;
    BOOLEAN                dataEnd;
    PIO_STATUS_BLOCK       ioStatus;

    *CompleteWithSuccess = NULL;
    *CompleteWithFailure = NULL;

    if (!RLCReceiver->CurrentIrReceiveRequest) {
        
        //
        // We have IR data, but no active IR request. 
        //
        SmscIrTracePrint(
                TRACE_LEVEL_ERROR,
                SMSCDBG_RECEIVE_INFO,
                ("SmscIrProcessPendingReceives: Received data on the %s "\
                 "receiver, but no IR requests",
                 RLCReceiver->IsPriorityReceiver ? "PRIORITY" : "NORMAL"));
        return;
        
    }
    
    //
    // Try to complete some data...
    //
    userBufferRemaining = RLCReceiver->ReceiveBufferSize 
                                    - RLCReceiver->ReceiveCurrentOffset;

    amountCopied = 
        SmscIrDequeueRLCData(
            RLCReceiver,
            &RLCReceiver->ReceiveBuffer[RLCReceiver->ReceiveCurrentOffset],
            userBufferRemaining,
            &dataEnd);

    RLCReceiver->ReceiveCurrentOffset += amountCopied;

    //
    // Something causes a 50 uSec silence to be inserted here when
    // using the learning receiver (cirtest test 7).
    // suppress it.
    // 
    if ( dataEnd && 
            RLCReceiver->IsPriorityReceiver ) {
   
        RLCReceiver->IgnoreFirstSilence = TRUE;
        }

    if (dataEnd ||
        (RLCReceiver->ReceiveCurrentOffset 
                              >= RLCReceiver->ReceiveBufferSize)) {

        //
        // Data end.or the user's buffer is done...
        //
        successfulRequest = RLCReceiver->CurrentIrReceiveRequest;

        status = WdfRequestUnmarkCancelable(successfulRequest);

        if (status == STATUS_CANCELLED) {

            //
            // The cancel routine is spinning waiting to complete this
            //  request then setup the next request...Don't go anything here,
            //  just get out.
            //
            SmscIrTracePrint(
                TRACE_LEVEL_ERROR,
                SMSCDBG_RECEIVE_INFO,
                ("SmscIrProcessPendingReceives: In progress request was "\
                 "cancelled."));
            return;
            
        }

        //
        // Get the IO_STATUS_BLOCK for the underlying IRP. This is 
        //  ehere we'll be filling in the number of bytes 
        //  transferred.
        //
        ioStatus = &WdfRequestWdmGetIrp(successfulRequest)->IoStatus;

        //
        // We need to break the abstraction of this routine 
        //  here since the data structures are a bit different
        //  for priority vs normal receive...
        //
        if (RLCReceiver->IsPriorityReceiver == FALSE) {

            RLCReceiver->ReceiveParams->DataEnd = dataEnd;

            //
            // The number of bytes returned is the size of the
            //  head of the structure plus the actual amount of
            //  data copied.
            //
            ioStatus->Information 
                    = FIELD_OFFSET(IR_RECEIVE_PARAMS, Data) +
                                    RLCReceiver->ReceiveCurrentOffset;
                            

        } else {
            
            RLCReceiver->PriorityReceiveParams->DataEnd = dataEnd;

            //
            // The number of bytes returned is the size of the
            //  head of the structure plus the actual amount of
            //  data copied.
            //
            ioStatus->Information 
                    = FIELD_OFFSET(IR_PRIORITY_RECEIVE_PARAMS, Data) +
                                        RLCReceiver->ReceiveCurrentOffset;

            //
            // Report the carrier frequency of the measurement.
            //
            if (RLCReceiver->DeviceData->CarrierMeasure) {

                //
                // Convert the carrier measure to a frequency
                //
                RLCReceiver->PriorityReceiveParams->CarrierFrequency = 
                    SMSCIR_C_C_MEASURE_TO_KHZ_F(
                            RLCReceiver->DeviceData->CarrierMeasure);

                SmscIrTracePrint(
                  TRACE_LEVEL_VERBOSE,
                  SMSCDBG_RECEIVE_INFO,
                 ("SmscIrProcessPendingReceives: Carrier for measurement "\
                  "was %dKHz",
                 (ULONG)RLCReceiver->PriorityReceiveParams->CarrierFrequency));

            } else {
                //
                // Oops, device didn't calculate it properly...
                //
                RLCReceiver->PriorityReceiveParams->CarrierFrequency = 0;

                SmscIrTracePrint(
                    TRACE_LEVEL_ERROR,
                    SMSCDBG_RECEIVE_INFO,
                    ("SmscIrProcessPendingReceives: No carrier measurement"));

            }

        }


        //
        // Get the next request setup
        //
        RLCReceiver->CurrentIrReceiveRequest = NULL;
        RLCReceiver->ReceiveBuffer           = NULL;
        RLCReceiver->ReceiveBufferSize       = 0;
        RLCReceiver->ReceiveCurrentOffset    = 0;

        status = IrSetupNextRequest(RLCReceiver,
                                    CompleteWithFailure,
                                    FailureStatus);

        if (!NT_SUCCESS(status)) {
    
            SmscIrTracePrint(
                TRACE_LEVEL_ERROR,
                SMSCDBG_RECEIVE_INFO,
                ("SmscIrProcessPendingReceives: IrSetupNextRequest failed "\
                 "- 0x%x", status));

        }
        
    }
    
    *CompleteWithSuccess = successfulRequest;
    *CompleteWithFailure = failedRequest;
    *FailureStatus = failureStatus;
    return;

}


NTSTATUS 
IrSetupCurrentReceive(
    IN PSMSCIR_RLC_RECEIVER RLCReceiver,
    IN WDFREQUEST ReceiveRequest
    ) {
/*++

Routine Description.

    Helper routine to take a request and correctly set it
    up as the current receive in the SMSCIR_RLC_RECEIVER structure

    RLCReceiver is *LOCKED* by caller on entry 

Arguments:

    RLCReceiver       - The IR data receiver

    ReceiveRequest    - An IR receive request

--*/

    PIR_RECEIVE_PARAMS receiveParams;
    NTSTATUS           status;
    size_t             actualBufferLength;
    ULONG              capturedByteCount;

    ASSERT(!RLCReceiver->IsPriorityReceiver);


    //
    // Length of the structure is variable, so give it
    //  a minimum size and get back the real size
    // 
    status = WdfRequestRetrieveOutputBuffer(ReceiveRequest, 
                                            sizeof(IR_RECEIVE_PARAMS),
                                            &receiveParams,
                                            &actualBufferLength);

    if (NT_SUCCESS(status)) {

        //
        // See IrReceive for the reasoning behind capturedByteCount
        //
        capturedByteCount = (ULONG)receiveParams->ByteCount;

        //
        // Watch out for a malformed ByteCount...
        //
        if (capturedByteCount >
             (actualBufferLength - sizeof(IR_RECEIVE_PARAMS))) {

            //
            // Tricky user...
            //
            SmscIrTracePrint(
                TRACE_LEVEL_ERROR,
                SMSCDBG_RECEIVE_INFO,
                ("IrSetupCurrentReceive: Buffer size specified in structure "\
                 "is too large. Buffer Size: 0x%x - Overall buffer size: 0x%x", 
                capturedByteCount, (ULONG)actualBufferLength));
            return STATUS_INVALID_BUFFER_SIZE;
        
        }

        RLCReceiver->CurrentIrReceiveRequest = ReceiveRequest;
        RLCReceiver->ReceiveBuffer = (PUCHAR)receiveParams->Data;
        RLCReceiver->ReceiveParams = receiveParams;
        RLCReceiver->ReceiveBufferSize = capturedByteCount;

        //
        // We'll be hanging this out there for a while,
        //  so need to set a cancel routine.
        //
        WdfRequestMarkCancelable(ReceiveRequest, 
                                 IrReceiveInProgressCancel);

        SmscIrTracePrint(
            TRACE_LEVEL_INFORMATION,
            SMSCDBG_RECEIVE_INFO,
            ("IrSetupCurrentReceive: Request 0x%p now current request",
                ReceiveRequest));
        return STATUS_SUCCESS;

    }

    SmscIrTracePrint(
        TRACE_LEVEL_ERROR,
        SMSCDBG_RECEIVE_INFO,
        ("IrSetupCurrentReceive: WdfRequestRetrieveOutputBuffer "\
         "failed - 0x%x", status));
    return status;

}

NTSTATUS 
IrSetupCurrentPriorityReceive(
    IN PSMSCIR_RLC_RECEIVER PriorityRLCReceiver,
    IN WDFREQUEST PriorityReceiveRequest
    ) {
/*++

Routine Description.

    Helper routine to take a request and correctly set it
    up as the current priority receive in the FILE_IR_DATA structure

    RLCReceiver is *LOCKED* by caller on entry 

Arguments:

    PriorityRLCReceiver       - The PRIORITY IR data receiver

    PriorityReceiveRequest - A priority receive request

--*/

    PIR_PRIORITY_RECEIVE_PARAMS priorityReceiveParams;
    NTSTATUS                    status;
    size_t                      actualBufferLength;
    ULONG                       capturedByteCount;


    ASSERT(PriorityRLCReceiver->IsPriorityReceiver);

    //
    // Length of the structure is variable, so give it
    //  a minimum size and get back the real size
    // 
    status = WdfRequestRetrieveOutputBuffer(PriorityReceiveRequest, 
                                            sizeof(IR_PRIORITY_RECEIVE_PARAMS),
                                            &priorityReceiveParams,
                                            &actualBufferLength);

    if (NT_SUCCESS(status)) {

        //
        // See IrReceive for the reasoning behind capturedByteCount
        //
        capturedByteCount = (ULONG)priorityReceiveParams->ByteCount;

        //
        // Watch out for a malformed ByteCount...
        //
        if (capturedByteCount >
             (actualBufferLength - sizeof(IR_PRIORITY_RECEIVE_PARAMS))) {

            //
            // Tricky user...
            //
            SmscIrTracePrint(
                TRACE_LEVEL_ERROR,
                SMSCDBG_RECEIVE_INFO,
                ("IrSetupCurrentPriorityReceive: Buffer size specified in "\
                 "structure is too large. Buffer Size: 0x%x - Overall "\
                 "buffer size: 0x%x", 
                capturedByteCount, (ULONG)actualBufferLength));
            return STATUS_INVALID_BUFFER_SIZE;
        
        }

        PriorityRLCReceiver->CurrentIrReceiveRequest = PriorityReceiveRequest;
        PriorityRLCReceiver->ReceiveBuffer 
                                = (PUCHAR)priorityReceiveParams->Data;
        PriorityRLCReceiver->PriorityReceiveParams = priorityReceiveParams;
        PriorityRLCReceiver->ReceiveBufferSize = capturedByteCount;

        //
        // We'll be hanging this out there for a while,
        //  so need to set a cancel routine.
        //
        WdfRequestMarkCancelable(PriorityReceiveRequest, 
                                 IrPriorityReceiveInProgressCancel);

        SmscIrTracePrint(
            TRACE_LEVEL_INFORMATION,
            SMSCDBG_RECEIVE_INFO,
            ("IrSetupCurrentPriorityReceive: Request 0x%p now current request",
                PriorityReceiveRequest));
        return STATUS_SUCCESS;

    }

    SmscIrTracePrint(
        TRACE_LEVEL_ERROR,
        SMSCDBG_RECEIVE_INFO,
        ("IrSetupCurrentPriorityReceive: WdfRequestRetrieveOutputBuffer "\
         "failed - 0x%x", status));
    return status;

}



VOID
SmscIrContinueBlasting(
    IN PSMSCIR_DATA DeviceData,
    OUT WDFREQUEST *TxRequestToComplete,
    OUT PNTSTATUS CompletionStatus
    ) {
/*++

Routine Description.

    This routine continues the current blasting operation.
    It will move bytes into the FIFO and start the transmit
    or it will return a request to complete
    
    Must be called with the *TxFifoDataLock* held

Arguments:

    DeviceData          - Our device context

    TxRequestToComplete - Request to complete with STATUS_SUCCESS 
                          by the caller once the lock is dropped


--*/

    BIRCC2_LINE_CONTROL           lc;
    BIRCC2_INTERRUPT_ID_OR_ENABLE txIntEnable;
    PSMSCIR_TX_FIFO_DATA          txFifoData = NULL;

    *TxRequestToComplete = NULL;
    
    if (!DeviceData->CurrentTxFifoData) {

        //
        // No current request...Someone beat us to it
        //
        SmscIrTracePrint(
            TRACE_LEVEL_VERBOSE,
            SMSCDBG_TRANSMIT_INFO,
            ("SmscIrContinueBlasting: No more data to blast"));
        return;
    }

    //
    // Check to make sure the user doesn't want this request
    //  cancelled. If she does, we'll abort.
    //
    if (WdfRequestIsCanceled(DeviceData->CurrentTxRequest)) {

        //
        // User aborted...Fail request
        //
        *TxRequestToComplete = DeviceData->CurrentTxRequest;
        *CompletionStatus    = STATUS_CANCELLED;

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

        return;
    }
    
    //
    // Tx processing to do...Continue blasting.
    //
    txFifoData = DeviceData->CurrentTxFifoData;

    //
    // Done with this portion?
    //
    if (txFifoData->CurrentOffset == txFifoData->FifoBufferLength) {
            
        SmscIrTracePrint(
            TRACE_LEVEL_VERBOSE,
            SMSCDBG_TRANSMIT_INFO,
            ("SmscIrContinueBlasting: Finished blasting sample"));

        txFifoData->TimesRepeated++;
        
        //
        // Repeated it as many times as necessary?
        //
        if (txFifoData->TimesRepeated < txFifoData->RepeatCount) {
                    
            SmscIrTracePrint(
                TRACE_LEVEL_VERBOSE,
                SMSCDBG_TRANSMIT_INFO,
                ("SmscIrContinueBlasting: Repeating previous sample"));

            //
            // Nope, we'll do it again.
            // 
            txFifoData->CurrentOffset = 0;
                    
        } else {
                    
            RemoveEntryList(&txFifoData->ListEntry);
            ExFreePool(txFifoData);
                    
            //
            // More to blast?
            // 
            if (!IsListEmpty(&DeviceData->TxFifoDataList)) {
                        
                SmscIrTracePrint(
                    TRACE_LEVEL_VERBOSE,
                    SMSCDBG_TRANSMIT_INFO,
                    ("SmscIrContinueBlasting: Blasting next sample"));

                //
                // Yup, move on to the next one.
                //
                txFifoData = (PSMSCIR_TX_FIFO_DATA)
                                RemoveHeadList(&DeviceData->TxFifoDataList);

                DeviceData->CurrentTxFifoData = txFifoData;
                
            } else {
                
                SmscIrTracePrint(
                    TRACE_LEVEL_VERBOSE,
                    SMSCDBG_TRANSMIT_INFO,
                    ("SmscIrContinueBlasting: Blasted final piece"));

                //
                // Done! Return the request to the caller
                //
                *TxRequestToComplete = DeviceData->CurrentTxRequest;
                *CompletionStatus    = STATUS_SUCCESS;

                DeviceData->CurrentTxFifoData = NULL;
                DeviceData->CurrentTxRequest  = NULL;
                return;
                
            }
            
        }

    }


    //
    // If we're here, we're not done..
    //
    SmscIrTracePrint(
        TRACE_LEVEL_VERBOSE,
        SMSCDBG_TRANSMIT_INFO,
        ("SmscIrContinueBlasting: Writing %d bytes starting at 0x%x", 
            SMSCIR_FIFO_SIZE, txFifoData->CurrentOffset));
    
    //
    // The data is guaranteed to always be a multiple of SMSCIR_FIFO_SIZE,
    //  so we can just blindly assume that there's at least that
    //  many bytes remaining
    //
    WRITE_TRANSMIT_BUFFER_UCHAR(
                            DeviceData,
                            BIRCC2_DATA_ADDR,
                            &txFifoData->FifoBuffer[txFifoData->CurrentOffset],
                            SMSCIR_FIFO_SIZE);
    
    txFifoData->CurrentOffset += SMSCIR_FIFO_SIZE;


    //
    // Enable FIFO interrupts...
    //
    txIntEnable.AsUChar = 0;
    txIntEnable.Fifo    = TRUE;
    WRITE_TRANSMIT_UCHAR(DeviceData,
                         BIRCC2_INTERRUPT_ENABLE_ADDR,
                         txIntEnable.AsUChar);

    //
    // Enable the transmitter
    //
    lc.AsUChar = 0;
    lc.TxEnable = 1;
    WRITE_TRANSMIT_UCHAR(DeviceData,
                         BIRCC2_LINE_CONTROL_ADDR,
                         lc.AsUChar);

    //
    // Record when we sent this. Our deadman might need to come along
    //  and poke the device if we don't hear back
    //
    KeQuerySystemTime(&DeviceData->LastBlastSentTime);
        

}


VOID
SmscIrEvtDeadManTimerForTransmit(
    IN WDFTIMER Timer
    ) {
/*++

Routine Description.

    This is our timer routine that fires once every SMSCIR_DEADMAN_TIMER_PERIOD
    milliseconds to check if the transmitter has stopped. This should happen
    very infrequently, but the possibility is there when the system is
    under heavy load

Arguments:

    Timer          - Our deadman timer

--*/

    PSMSCIR_DATA           deviceData;
    LINE_STATUS_READ       lsr;
    BIRCC2_LINE_CONTROL    txLc;
    LARGE_INTEGER          currentTime;
    LARGE_INTEGER          lastBlast;
    LINE_CONTROL_B         lcb;
    LINE_CONTROL_A         lca;
    MASTER_BLOCK_CONTROL   mbc;
    INTERRUPT_ID_OR_ENABLE intEnable;

    deviceData = (PSMSCIR_DATA)GetIrData(WdfTimerGetParentObject(Timer));

    lastBlast = deviceData->LastBlastSentTime;

    KeQuerySystemTime(&currentTime);

    SmscIrTracePrint(
        TRACE_LEVEL_VERBOSE,
        SMSCDBG_TRANSMIT_INFO,
        ("SmscIrEvtDeadManTimerForTransmit: Entered"));
    
    //
    // It should NOT take a second to send data. If it's been a second
    //  since we last saw a FIFO blast complete reset the device
    //
    if ((currentTime.QuadPart - lastBlast.QuadPart) >= SECONDS(1)) {

        //
        // We're blasting and it's been a while since we last heard from the
        //  device...Check for errors.
        //
        WdfInterruptAcquireLock(deviceData->Interrupt);

        //
        // A bum blast can throw off our receiver. Check for any
        //  error conditions there
        //
        lsr.AsUChar = READ_HARDWARE_UCHAR(deviceData,
                                          CIRCC2_LINE_STATUS_R);
    
        if (lsr.AsUChar) {


            //
            // Some sort of receiver error...
            //
            SmscIrTracePrint(
                TRACE_LEVEL_ERROR,
                SMSCDBG_TRANSMIT_INFO,
                ("SmscIrEvtDeadManTimerForTransmit: Errors on receive fifo"));

            //
            // Shut the receiver off.
            //
            lcb.AsUChar = 0;
            WRITE_HARDWARE_UCHAR(deviceData,
                                 CIRCC2_LINE_CONTROL_B,
                                 lcb.AsUChar);

            //
            // Dump any potential leftovers from the FIFO. 
            //
            lca.AsUChar = 0;
            lca.FifoReset = TRUE;
            
            WRITE_HARDWARE_UCHAR(deviceData,
                                 CIRCC2_LINE_CONTROL_A,
                                 lca.AsUChar);

            mbc.AsUChar = 0;
            mbc.ErrorReset = TRUE;
            WRITE_HARDWARE_UCHAR(deviceData,
                                 CIRCC2_MASTER_BLOCK_CONTROL,
                                 mbc.AsUChar);
            mbc.AsUChar = 0;
            mbc.MasterInterruptEnable = TRUE;
            WRITE_HARDWARE_UCHAR(deviceData,
                                 CIRCC2_MASTER_BLOCK_CONTROL,
                                 mbc.AsUChar);

            intEnable.AsUChar = 0;
            intEnable.Fifo    = TRUE;
            WRITE_HARDWARE_UCHAR(deviceData,
                                 CIRCC2_INTERRUPT_ENABLE,
                                 intEnable.AsUChar);

            //
            // Set the device to receive.
            //
            lcb.AsUChar = 0;
            lcb.SCEModeBits = SCE_MODE_RECEIVE;
            WRITE_HARDWARE_UCHAR(deviceData,
                                 CIRCC2_LINE_CONTROL_B,
                                 lcb.AsUChar);

        }


        txLc.AsUChar = READ_TRANSMIT_UCHAR(deviceData,
                                           BIRCC2_LINE_CONTROL_ADDR);

        if (txLc.FifoNotEmpty && !txLc.TxEnable) {

            //
            // Errors on the device.
            //
            //
            // Reset the FIFO, this will generate a FIFO interrupt
            //  and we'll continue blasting
            //
            SmscIrTracePrint(
                TRACE_LEVEL_ERROR,
                SMSCDBG_TRANSMIT_INFO,
                ("SmscIrEvtDeadManTimerForTransmit: Errors on transmit fifo"));

            txLc.FifoReset = TRUE;
            WRITE_TRANSMIT_UCHAR(deviceData,
                                 BIRCC2_LINE_CONTROL_ADDR,
                                 txLc.AsUChar);
        }
    
        WdfInterruptReleaseLock(deviceData->Interrupt);

    }

    SmscIrTracePrint(
        TRACE_LEVEL_VERBOSE,
        SMSCDBG_TRANSMIT_INFO,
        ("SmscIrEvtDeadManTimerForTransmit: Exited"));


}

