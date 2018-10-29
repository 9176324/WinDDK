//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Copyright 2005 OSR, Open Systems Resources, Inc. All rights Reserved.
// 
// Module Name:
// 
//     isr.c
// 
// Abstract:
//
//     Contains routines for interrupts
// 
// Author:
//
// Revision History:
//      
#include "smscir.h"

#ifdef WPP_TRACING
#include "isr.tmh"
#endif


//
// Forward declarations
//

VOID
SmscIrEnableReceiveInterrupts(
    PSMSCIR_DATA DeviceData
    );

VOID
SmscIrEnableTransmitInterrupts(
    PSMSCIR_DATA DeviceData
    );

VOID 
SmscIrEnqueueFifoByte(
    IN PSMSCIR_DATA FdoData,
    UCHAR Byte,
    IN BOOLEAN DataEnd
    );

BOOLEAN
SmscIrProcessRxInterrupt(
    PSMSCIR_DATA DeviceData,
    PINTERRUPT_ID_OR_ENABLE RxIntId
    );

BOOLEAN
SmscIrProcessTxInterrupt(
    PSMSCIR_DATA DeviceData,
    PBIRCC2_INTERRUPT_ID_OR_ENABLE TxIntId
    );


NTSTATUS
SmscIrEvtInterruptEnable(
    IN WDFINTERRUPT Interrupt,
    IN WDFDEVICE    Device
    ) {
/*++

Routine Description.

    Called here to enable interrupts on the device. 

Arguments:

    Interrupt  - Our interrupt object

    Device     - The device to enable interrupts on

--*/
    PSMSCIR_DATA           deviceData;

    UNREFERENCED_PARAMETER(Interrupt);

    SmscIrTracePrint(TRACE_LEVEL_VERBOSE, 
                     SMSCDBG_INIT_INFO, 
                     ("SmscIrEvtInterruptEnable: Entered"));
    
    deviceData = (PSMSCIR_DATA)GetIrData(Device);

    //
    // Setup the device for RECEIVE interrupts
    //
    SmscIrEnableReceiveInterrupts(deviceData);

    //
    // Setup the device for TRANSMIT interrupts
    //
    SmscIrEnableTransmitInterrupts(deviceData);

    SmscIrTracePrint(TRACE_LEVEL_VERBOSE, 
                     SMSCDBG_INIT_INFO, 
                     ("SmscIrEvtInterruptEnable: Exited"));

    return STATUS_SUCCESS;

}


VOID
SmscIrEnableReceiveInterrupts(
    PSMSCIR_DATA DeviceData
    ) {
/*++

Routine Description.

    Helper routine to enable receive interrupts

Arguments:

    DeviceData     - The device extension

--*/
    MASTER_BLOCK_CONTROL   mbc;
    INTERRUPT_ID_OR_ENABLE intEnable;
    LINE_CONTROL_B         lcb;
    LINE_CONTROL_A         lca;
    CONSUMER_IR_CTRL       irCtrl;


    //
    // First thing is first, reset the device...
    //
    mbc.AsUChar = 0;
    mbc.MasterReset = TRUE;
    mbc.ErrorReset = TRUE;
    WRITE_HARDWARE_UCHAR(DeviceData,
                         CIRCC2_MASTER_BLOCK_CONTROL,
                         mbc.AsUChar);


    mbc.AsUChar = 0;
    mbc.RegisterBlockSelect = 1;
    WRITE_HARDWARE_UCHAR(DeviceData,
                         CIRCC2_MASTER_BLOCK_CONTROL,
                         mbc.AsUChar);

    //
    // Setup the receive threshold. We'll get an interrupt
    //  when FIFO_THRESHOLD_FOR_RECEIVE bytes are in the 
    //  FIFO
    //
    WRITE_HARDWARE_UCHAR(DeviceData,
                         CIRCC2_FIFO_THRESHOLD,
                         FIFO_THRESHOLD_FOR_RECEIVE);

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
    
    //
    // RC6 uses a 36kHz carrier, which is the standard
    //  that the class driver uses. So, not matter
    //  what device the IR is coming from the class driver
    //  wants the data modulated at 36kHz
    //
    WRITE_HARDWARE_UCHAR(DeviceData,
                         CIRCC2_CONSUMER_IR_CARRIER,
                         SMSCIR_RC6_CARRIER_DIVIDER);

    WRITE_HARDWARE_UCHAR(DeviceData,
                         CIRCC2_CONSUMER_IR_BIT_RATE,
                         SMSCIR_BIT_RATE_DIVIDER);


    //
    // Back to register block 0...
    //
    mbc.AsUChar = 0;
    mbc.RegisterBlockSelect = 0;
    WRITE_HARDWARE_UCHAR(DeviceData,
                         CIRCC2_MASTER_BLOCK_CONTROL,
                         mbc.AsUChar);

    //
    // It appears that in some cases the master reset
    //  doesn't dump out FIFO garbage, do that now...
    //
    lca.AsUChar = 0;
    lca.FifoReset = TRUE;

    WRITE_HARDWARE_UCHAR(DeviceData,
                         CIRCC2_LINE_CONTROL_A,
                         lca.AsUChar);

    //
    // Enable interrupt support on the device
    //
    mbc.AsUChar = 0;
    mbc.MasterInterruptEnable = TRUE;
    WRITE_HARDWARE_UCHAR(DeviceData,
                         CIRCC2_MASTER_BLOCK_CONTROL,
                         mbc.AsUChar);


    //
    // Enable FIFO interrupts.
    //
    intEnable.AsUChar = 0;
    intEnable.Fifo    = TRUE;
    WRITE_HARDWARE_UCHAR(DeviceData,
                         CIRCC2_INTERRUPT_ENABLE,
                         intEnable.AsUChar);


    //
    // Set the device to receive.
    //
    lcb.AsUChar = 0;
    lcb.SCEModeBits = SCE_MODE_RECEIVE;

    WRITE_HARDWARE_UCHAR(DeviceData,
                         CIRCC2_LINE_CONTROL_B,
                         lcb.AsUChar);

    return;

}

VOID
SmscIrEnableTransmitInterrupts(
    PSMSCIR_DATA DeviceData
    ) {
/*++

Routine Description.

    Helper routine to enable transmit interrupts

Arguments:

    DeviceData     - The device extension

--*/

    BIRCC2_MASTER_BLOCK_CONTROL   txMbc;
    BIRCC2_MODE                   txMode;


    //
    // First thing is first, reset the device...
    //
    txMbc.AsUChar = 0;
    txMbc.MasterReset = TRUE;
    WRITE_TRANSMIT_UCHAR(DeviceData,
                         BIRCC2_MASTER_BLOCK_CONTROL_ADDR,
                         txMbc.AsUChar);

    //
    // Now setup the FIFO threshold, which happens in register 1
    //
    txMbc.AsUChar = 0;
    txMbc.RegisterBlockSelect = 1;
    WRITE_TRANSMIT_UCHAR(DeviceData,
                         BIRCC2_MASTER_BLOCK_CONTROL_ADDR,
                         txMbc.AsUChar);

    //
    // Setup the Tx FIFO threshold
    //
    WRITE_TRANSMIT_UCHAR(DeviceData,
                         BIRCC2_FIFO_THRESHOLD_ADDR,
                         FIFO_THRESHOLD_FOR_TRANSMIT);

    //
    // Default to the RC6 carrier
    //
    WRITE_TRANSMIT_UCHAR(DeviceData,
                         BIRCC2_CONSUMER_IR_CARRIER,
                         SMSCIR_RC6_CARRIER_DIVIDER);

    //
    // And the standard bit rate divider (to make 50usec
    //  samples)
    //
    WRITE_TRANSMIT_UCHAR(DeviceData,
                         BIRCC2_CONSUMER_IR_BIT_RATE,
                         SMSCIR_BIT_RATE_DIVIDER);

    txMode.AsUChar = 0;
    WRITE_TRANSMIT_UCHAR(DeviceData,
                         BIRCC2_MODE_ADDR,
                         txMode.AsUChar);

    //
    // Back to block zero to set the appropriate interrupt
    //  enable bits
    //
    txMbc.AsUChar = 0;
    txMbc.RegisterBlockSelect = 0;
    txMbc.MasterInterruptEnable = TRUE;
    WRITE_TRANSMIT_UCHAR(DeviceData,
                         BIRCC2_MASTER_BLOCK_CONTROL_ADDR,
                         txMbc.AsUChar);


}


BOOLEAN
SmscIrEvtInterruptIsr(
    IN WDFINTERRUPT Interrupt,
    IN ULONG        MessageID
    ) {
/*++

Routine Description.

    The interrupt service routine for the 
    SMSC CIRCC2 device. This ISR fires every
    time there is a FIFO interrupt, which means
    that there is data to receive in Rx mode
    or that more data needs to be moved into the
    FIFO in Tx mode

Arguments:

    Interrupt       - Our interrupt object

    MessageID       - For MSI interrupts. Not used.

--*/

    PSMSCIR_DATA           deviceData;
    INTERRUPT_ID_OR_ENABLE rxIntId;
    BOOLEAN                requestDpc = FALSE;
    BIRCC2_INTERRUPT_ID_OR_ENABLE txIntId;

    UNREFERENCED_PARAMETER(MessageID);
    
    deviceData = (PSMSCIR_DATA)GetIrData(WdfInterruptGetDevice(Interrupt));


    //
    // Check for Rx interrupt
    //
    rxIntId.AsUChar = READ_HARDWARE_UCHAR(deviceData,
                                          CIRCC2_INTERRUPT_ID);

    //
    // Receive interrupt?
    // 
    if (SmscIrProcessRxInterrupt(deviceData, &rxIntId)) {
        
        //
        // We'll need a DPC to complete any outstanding
        //  receive requests that his might fulfill...
        //
        requestDpc = TRUE;
        
    }
    
    //
    // And check for Tx interrupt
    //
    txIntId.AsUChar = READ_TRANSMIT_UCHAR(deviceData,
                                          BIRCC2_INTERRUPT_ID_ADDR);

    if (SmscIrProcessTxInterrupt(deviceData, &txIntId)) {

        //
        // We'll need a DPC to continue the blasting...
        //
        requestDpc = TRUE;

    }


    if (requestDpc) {

        WdfInterruptQueueDpcForIsr(Interrupt);

    }



    //
    // We're an ISA interrupt and therefore we are NOT
    //  shared. If we're interrupting here then it
    //  was definitely our interrupt...
    //
    return TRUE;

}



BOOLEAN
__forceinline
SmscIrProcessRxInterrupt(
    PSMSCIR_DATA DeviceData,
    PINTERRUPT_ID_OR_ENABLE RxIntId
    ) {
/*++

Routine Description.

    Helper routine to check for and process receive interrupts
    one the device.


    returns TRUE if the RX interrupt is active and a DPC is 
    required.

Arguments:

    DeviceData      - Our device extension

    RxIntId         - The contents of the CIRCC2_INTERRUPT_ID register

--*/

    LINE_CONTROL_B          lcb;
    MASTER_BLOCK_CONTROL    mbc;
    INTERRUPT_ID_OR_ENABLE  intEnable;
    LINE_CONTROL_A          lca;
    LINE_STATUS_READ        lsr;
    UCHAR                   byte;
    BOOLEAN                 bytesDequeued;
    UCHAR                   carrierMeasure;
    CARRIER_CAPTURE_CONTROL captureControl;

    if (RxIntId->Fifo) {

        //
        // Clear the FIFO interrupt by disabling FIFO
        //  interrupts. We'll enable it again before we
        //  leave
        //
        intEnable.AsUChar = 0;
        intEnable.Fifo    = FALSE;
        WRITE_HARDWARE_UCHAR(DeviceData,
                             CIRCC2_INTERRUPT_ENABLE,
                             intEnable.AsUChar);

        //
        // We set the trigger to fire when there is SIZE-1 bytes in the
        //  FIFO (cuz that's the highest watermark allowed). Copy all
        //  of those bytes out.
        //
        for (bytesDequeued = 0; 
                    bytesDequeued < (SMSCIR_FIFO_SIZE-1); 
                                            bytesDequeued++) {


            byte = READ_HARDWARE_UCHAR(DeviceData,
                                       CIRCC2_DATA);

            
            //
            // Enqueue the byte to be picked up by the DPC. FALSE
            //  because this is not a data end 
            //
            SmscIrEnqueueFifoByte(DeviceData,
                                  byte,
                                  FALSE); 

            //
            // Do we have an FF?
            //
            if (byte == 0xFF) {
            
                //
                // Update the number that we've seen so far...
                //
                DeviceData->ConsecutiveFFs++;
            
            } else {
            
                //
                // Reset!
                //
                DeviceData->ConsecutiveFFs = 0;
            }
        
            bytesDequeued++;

        }
        
    }

    lsr.AsUChar = READ_HARDWARE_UCHAR(DeviceData,
                                      CIRCC2_LINE_STATUS_R);


    //
    // End of IR data or have we trashed the FIFO? If we 
    //  trashed the FIFO then the safest thing to do is
    //  just abort the keypress and start over, which we'll
    //  do...
    //
    if ((DeviceData->ConsecutiveFFs >= DeviceData->NumFFsForDataEnd) ||
        lsr.OverRun) {

            
        //
        // Indeed. Let's start this over...
        //
        DeviceData->ConsecutiveFFs = 0;
        
        //
        // Enqueue one more byte, this time indicate that
        //  we are indeed at the end fo the line with TRUE 
        //  for DataEnd...
        //
        SmscIrEnqueueFifoByte(DeviceData,
                              0xFF,
                              TRUE); 

            
        //
        // Once the receiver starts to collect IR
        //  data, it collects the samples from the actual
        //  IR stream and then continues to fill the FIFO 
        //  with FFs to indicate silence until we shut it 
        //  up. How do we shut it up, you ask? By disabling
        //  the IR receiver and then enabling it again, of 
        //  course...
        //
        // We'll reenable the receiver before we get out of here...
        //
        lcb.AsUChar = 0;
        WRITE_HARDWARE_UCHAR(DeviceData,
                             CIRCC2_LINE_CONTROL_B,
                             lcb.AsUChar);

        //
        // Dump any potential leftovers from the FIFO. 
        //
        lca.AsUChar = 0;
        lca.FifoReset = TRUE;
        
        WRITE_HARDWARE_UCHAR(DeviceData,
                             CIRCC2_LINE_CONTROL_A,
                             lca.AsUChar);

        //
        // If we're in priority mode we'll need to read
        //  the carrier measure register
        //
        if (DeviceData->InPriorityMode) {

            mbc.AsUChar = 0;
            mbc.RegisterBlockSelect = 5;
            WRITE_HARDWARE_UCHAR(DeviceData,
                                 CIRCC2_MASTER_BLOCK_CONTROL,
                                 mbc.AsUChar);

            carrierMeasure 
                    = READ_HARDWARE_UCHAR(DeviceData,
                                          CIRCC2_CARRIER_CAPTURE_MEASURE);

            if (carrierMeasure != 0) {
                
                DeviceData->CarrierMeasure = carrierMeasure;
                
            }

            //
            // Reset the carrier capture control
            //
            captureControl.AsUChar = 0;    
            captureControl.CaptureReset = TRUE;
            WRITE_HARDWARE_UCHAR(DeviceData,
                                 CIRCC2_CARRIER_CAPTURE_CONTROL,
                                 captureControl.AsUChar);

        }

        mbc.AsUChar = 0;
        mbc.ErrorReset = TRUE;
        WRITE_HARDWARE_UCHAR(DeviceData,
                             CIRCC2_MASTER_BLOCK_CONTROL,
                             mbc.AsUChar);
        mbc.AsUChar = 0;
        mbc.MasterInterruptEnable = TRUE;
        WRITE_HARDWARE_UCHAR(DeviceData,
                             CIRCC2_MASTER_BLOCK_CONTROL,
                             mbc.AsUChar);

             
    }

    //
    // Re-enable the FIFO interrupt
    //
    intEnable.AsUChar = 0;
    intEnable.Fifo    = TRUE;
    WRITE_HARDWARE_UCHAR(DeviceData,
                         CIRCC2_INTERRUPT_ENABLE,
                         intEnable.AsUChar);

    //
    // Set the device to receive.
    //
    lcb.AsUChar = 0;
    lcb.SCEModeBits = SCE_MODE_RECEIVE;
    WRITE_HARDWARE_UCHAR(DeviceData,
                         CIRCC2_LINE_CONTROL_B,
                         lcb.AsUChar);

    //
    // DPC required.
    //
    return TRUE;

}


BOOLEAN
__forceinline
SmscIrProcessTxInterrupt(
    PSMSCIR_DATA DeviceData,
    PBIRCC2_INTERRUPT_ID_OR_ENABLE TxIntId
    ) {

    BIRCC2_LINE_CONTROL    txLc;
    BIRCC2_INTERRUPT_ID_OR_ENABLE txIntEnable;


    //
    // Store away the interrupt mask
    //
    DeviceData->TxIntId.AsUChar |= TxIntId->AsUChar;

    if (TxIntId->Fifo) {
     
        //
        // Shut off the FIFO interrupt.
        //
        txIntEnable.AsUChar = 0;
        WRITE_TRANSMIT_UCHAR(DeviceData,
                             BIRCC2_INTERRUPT_ENABLE_ADDR,
                             txIntEnable.AsUChar);
        //
        // Need a DPC
        //
        return TRUE;

    }

    //
    // No FIFO interrupt...Check for an underrun
    //
    if (TxIntId->TxComplete) {
 
        txLc.AsUChar = READ_TRANSMIT_UCHAR(DeviceData,
                                           BIRCC2_LINE_CONTROL_ADDR);
        if (txLc.FifoNotEmpty &&
            !txLc.TxEnable) {
                
            //
            // Underrun...Provoke the DPC into continuing the
            //  processing...
            //
            DeviceData->TxIntId.Fifo = TRUE;

            
            //
            // And reset the FIFO
            //
            txLc.FifoReset = TRUE;
            WRITE_TRANSMIT_UCHAR(DeviceData,
                                 BIRCC2_LINE_CONTROL_ADDR,
                                 txLc.AsUChar);

        }

        return TRUE;

    }
    return FALSE;


}


__forceinline
VOID 
SmscIrEnqueueFifoByte(
    IN PSMSCIR_DATA FdoData,
    UCHAR Byte,
    IN BOOLEAN DataEnd
    ) {
/*++

Routine Description:

    Called from the ISR to move the byte taken from
    the FIFO into the circular buffer

Arguments:

    FdoData     - The device extension

    Byte        - The byte to store

    DataEnd     - Is this a data end?

--*/

    FdoData->CurrentFifoBufferSize++;
    
    FdoData->FifoBuffer[FdoData->NextFifoIndex++] = Byte;

    if (FdoData->NextFifoIndex == SMSCIR_FIFO_BUFFER_SIZE) {
        FdoData->NextFifoIndex = 0;
    }

    if (FdoData->CurrentFifoBufferSize > SMSCIR_FIFO_BUFFER_SIZE) {
        FdoData->CurrentFifoBufferSize = SMSCIR_FIFO_BUFFER_SIZE;
        FdoData->CurrentFifoIndex = (FdoData->NextFifoIndex != 0) 
                                           ? FdoData->NextFifoIndex-1  
                                                   : SMSCIR_FIFO_BUFFER_SIZE-1;
    }

    //
    // If this is a data end, then we'll set the watermark
    //  for the data end event. When the dequeue code comes
    //  in it will use this to know how to mark the 
    //  DataEnd member of the RECEIVE IOCTL.
    //
    if (DataEnd) {

        if (FdoData->BytesToDataEnd) {
            SmscIrTracePrint(
                    TRACE_LEVEL_INFORMATION,
                    SMSCDBG_RECEIVE_INFO,
                    ("SmscIrEnqueueFifoByte: DataEnd was already present. "\
                     "Previous KeyUp missed\n"));
        }

        FdoData->BytesToDataEnd = FdoData->CurrentFifoBufferSize;

    }

}


ULONG 
SmscIrFifoBytesRemaining(
    IN PSMSCIR_DATA DeviceData
    ) {
/*++

Routine Description:

    Returns the number of bytes remaining in the FIFO buffer


    *Interrupt Lock* held by caller on entry

Arguments:

    DeviceData     - The device extension

--*/

    if (DeviceData->BytesToDataEnd) {
        return DeviceData->BytesToDataEnd;
    } else {
        return DeviceData->CurrentFifoBufferSize;
    }

}



ULONG 
SmscIrDequeueFifoData(
    IN PSMSCIR_DATA DeviceData,
    __out_bcount(MaxLen) PUCHAR Buffer,
    ULONG MaxLen,
    __out PBOOLEAN DataEnd
    ) {
/*++

Routine Description:

    Called from the DPC to remove multiple bytes from
    the FIFO circular buffer

    *Interrupt Lock* held by caller on entry
    
Arguments:

    DeviceData - The device extension

    Buffer - Buffer to copy the data into
             
    MaxLen - Size of the Buffer parameter, in bytes

    DataEnd - Is this a data end?

Return Value:

    Actual number of bytes copied

--*/
    ULONG bufferSize;
    ULONG amountToCopy;
    ULONG nextIndex;
#if DBG
    ULONG prevValue;
#endif

    *DataEnd = FALSE;

    if (0 == DeviceData->CurrentFifoBufferSize){
        return 0;
    } else {

        if (DeviceData->BytesToDataEnd) {
            bufferSize = DeviceData->BytesToDataEnd;
        } else {
            bufferSize = DeviceData->CurrentFifoBufferSize;
        }

        amountToCopy = bufferSize < MaxLen ? bufferSize : MaxLen;

        if ((DeviceData->CurrentFifoIndex + amountToCopy) 
                                            > SMSCIR_FIFO_BUFFER_SIZE) {
            ULONG amountToCopyFirst 
                     = SMSCIR_FIFO_BUFFER_SIZE - DeviceData->CurrentFifoIndex;
            RtlCopyMemory(
                    Buffer, 
                    DeviceData->FifoBuffer + DeviceData->CurrentFifoIndex,
                    amountToCopyFirst);
            //
            // Prefast will complain about this calculation, but it is 
            //  correct.
            //
#pragma prefast (suppress:411, "Calculation is correct and as designed")
            RtlCopyMemory(Buffer + amountToCopyFirst, 
                          DeviceData->FifoBuffer,
                          amountToCopy - amountToCopyFirst);
            DeviceData->CurrentFifoIndex = amountToCopy - amountToCopyFirst;
        } else {
            RtlCopyMemory(
                    Buffer, 
                    DeviceData->FifoBuffer + DeviceData->CurrentFifoIndex,
                    amountToCopy);

            nextIndex = DeviceData->CurrentFifoIndex + amountToCopy;

            if (nextIndex == SMSCIR_FIFO_BUFFER_SIZE) {

                DeviceData->CurrentFifoIndex = 0;

            } else {

                DeviceData->CurrentFifoIndex = nextIndex;

            }

        }

        if (DeviceData->BytesToDataEnd) {
            
#if DBG
            prevValue = DeviceData->BytesToDataEnd;
#endif

            DeviceData->BytesToDataEnd -= amountToCopy;

#if DBG
            //
            // Make sure we didn't wrap under...
            //
            ASSERT(DeviceData->BytesToDataEnd < prevValue);
#endif

            *DataEnd = (BOOLEAN)(DeviceData->BytesToDataEnd == 0);

        }

#if DBG
        prevValue = DeviceData->CurrentFifoBufferSize;
#endif
        DeviceData->CurrentFifoBufferSize -= amountToCopy;

#if DBG
        //
        // Make sure we didn't wrap under...
        //
        ASSERT(DeviceData->CurrentFifoBufferSize < prevValue);
#endif


        return amountToCopy;
    }
}





NTSTATUS
SmscIrEvtInterruptDisable(
    IN WDFINTERRUPT Interrupt,
    IN WDFDEVICE    Device
    ) {
/*++

Routine Description.

    Called here to disable interrupts on the device. 

Arguments:

    Interrupt  - Our interrupt object

    Device     - The device to enable interrupts on

--*/
    MASTER_BLOCK_CONTROL mbc;
    PSMSCIR_DATA         deviceData;
    
    UNREFERENCED_PARAMETER(Interrupt);

    SmscIrTracePrint(TRACE_LEVEL_VERBOSE, 
                     SMSCDBG_INIT_INFO, 
                     ("SmscIrEvtInterruptDisable: Entered"));

    deviceData = (PSMSCIR_DATA)GetIrData(Device);

    //
    // Just disabling master interrupts is enough
    //  to quiesce the device and allow our driver
    //  to go away safely.
    //
    mbc.AsUChar = 0;
    WRITE_HARDWARE_UCHAR(deviceData,
                         CIRCC2_MASTER_BLOCK_CONTROL,
                         mbc.AsUChar);

    SmscIrTracePrint(TRACE_LEVEL_VERBOSE, 
                     SMSCDBG_INIT_INFO, 
                     ("SmscIrEvtInterruptDisable: Exited"));
    return STATUS_SUCCESS;

}


