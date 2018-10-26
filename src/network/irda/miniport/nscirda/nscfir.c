/*
 ************************************************************************
 *
 *  NSCFIR.C
 *
 *
 * Portions Copyright (C) 1996-2001 National Semiconductor Corp.
 * All rights reserved.
 * Copyright (C) 1996-2001 Microsoft Corporation. All Rights Reserved.
 *
 *
 *
 *************************************************************************
 */


#include "nsc.h"
#include "nscfir.tmh"
#include "nsctypes.h"

VOID NSC_FIR_ISR(IrDevice *thisDev, BOOLEAN *claimingInterrupt,
                 BOOLEAN *requireDeferredCallback)
{
    LOG_FIR("==> NSC_FIR_ISR");
    DEBUGFIR(DBG_ISR|DBG_OUT, ("NSC: ==> NSC_FIR_ISR(0x%x)\n", thisDev));

    thisDev->InterruptMask   = NSC_ReadBankReg(thisDev->portInfo.ioBase, 0, 1);

    thisDev->InterruptStatus = NSC_ReadBankReg(thisDev->portInfo.ioBase, 0, 2) & thisDev->FirIntMask;

    thisDev->AuxStatus       = NSC_ReadBankReg(thisDev->portInfo.ioBase, 0, 7);

    thisDev->LineStatus      = NSC_ReadBankReg(thisDev->portInfo.ioBase, 0, 5);


    LOG_FIR("InterruptMask: %02x, InterruptStatus: %02x", thisDev->InterruptMask,thisDev->InterruptStatus);
    LOG_FIR("AuxStatus:     %02x, LineStatus:      %02x", thisDev->AuxStatus,thisDev->LineStatus);

    DEBUGFIR(DBG_ISR|DBG_OUT, ("NSC: InterruptMask = 0x%x\n", thisDev->InterruptMask));
    DEBUGFIR(DBG_ISR|DBG_OUT, ("NSC: InterruptStatus = 0x%x\n", thisDev->InterruptStatus));
    DEBUGFIR(DBG_ISR|DBG_OUT, ("NSC: AuxStatus = 0x%x\n", thisDev->AuxStatus));
    DEBUGFIR(DBG_ISR|DBG_OUT, ("NSC: LineStatus = 0x%x\n", thisDev->LineStatus));

    if (thisDev->InterruptStatus) {
        //
        // After seeing the first packet switch the interrupt
        // to timer and use the DMA counter to decide if receptions
        // have stopped.
        //
        if (thisDev->InterruptStatus & LS_EV) {
            //
            //  Got a link status interrupt
            //
            if (thisDev->LineStatus & LSR_FR_END) {
                //
                //  The frame end status is set
                //
                if (!thisDev->FirTransmitPending) {
                    //
                    //  we weren't tansmitting
                    //
                }
            }
            if (thisDev->LineStatus & LSR_OE) {

                LOG_ERROR("NSC_FIR_ISR: rx overflow");
            }
        }

        if (thisDev->InterruptStatus & TMR_EV){
            //
            //  Disable Timer during call to DPC bit.
            //
            NSC_WriteBankReg(thisDev->portInfo.ioBase, 4, 2, 0x00);
            NSC_WriteBankReg(thisDev->portInfo.ioBase, 0, 7, 0x80);
        }

        *claimingInterrupt = TRUE;
        *requireDeferredCallback = TRUE;

        SetCOMInterrupts(thisDev, FALSE);

        if (thisDev->UIR_ModuleId >= 0x16 )
        {
            // This is for the Frame stop mode when the ISR is interrupted
            // after every Frame Tx
            //

            if ((thisDev->AuxStatus & 0x08)
             && (thisDev->InterruptStatus & 0x04)
             && (thisDev->FirTransmitPending == TRUE))
            {
                if (thisDev->LineStatus&0x40)
                {
                    NSC_WriteBankReg(thisDev->portInfo.ioBase, 0, 7, 0x40);
                    DBGERR(("FIR: Transmit underrun\n"));
                }
            }
        }
    }
    else {
        *claimingInterrupt = FALSE;
        *requireDeferredCallback = FALSE;
    }

    LOG_FIR("<== NSC_FIR_ISR claimingInterrupt = %x, requireDeferredCallback = %x ", *claimingInterrupt,*requireDeferredCallback);

    DEBUGFIR(DBG_ISR|DBG_OUT, ("NSC: <== NSC_FIR_ISR\n"));

}

typedef struct {
    IrDevice *thisDev;
    ULONG_PTR Offset;
    ULONG_PTR Length;
} DMASPACE;

void SkipNonDmaBuffers(PLIST_ENTRY Head, PLIST_ENTRY *Entry)
{
    while (Head!=*Entry)
    {
        rcvBuffer *rcvBuf = CONTAINING_RECORD(*Entry,
                                              rcvBuffer,
                                              listEntry);

        if (rcvBuf->isDmaBuf)
        {
            break;
        }
        else
        {
            *Entry = (*Entry)->Flink;
        }
    }
}

//
// We have two lists of buffers which occupy our DMA space.  We
// want to walk this list and find the largest space for putting
// new packets.
//
rcvBuffer *GetNextPacket(DMASPACE *Space,
                         PLIST_ENTRY *CurrFull,
                         PLIST_ENTRY *CurrPend)
{
    rcvBuffer *Result = NULL;

    SkipNonDmaBuffers(&Space->thisDev->rcvBufFull, CurrFull);
    SkipNonDmaBuffers(&Space->thisDev->rcvBufPend, CurrPend);

    if (*CurrFull==&Space->thisDev->rcvBufFull)
    {
        // Full list is finished.
        if (*CurrPend!=&Space->thisDev->rcvBufPend)
        {
            // Any entry on the pend list is valid.  Take the
            // next one and advance the pointer.

            Result = CONTAINING_RECORD(*CurrPend,
                                       rcvBuffer,
                                       listEntry);

            *CurrPend = (*CurrPend)->Flink;
        }
        else
        {
            // Both lists are finished.  We will return NULL.
        }
    }
    else
    {
        if (*CurrPend==&Space->thisDev->rcvBufPend)
        {
            // Pend list is finished.  Take anything from the
            // Full list, advance the pointer.
            Result = CONTAINING_RECORD(*CurrFull,
                                       rcvBuffer,
                                       listEntry);

            *CurrFull = (*CurrFull)->Flink;
        }
        else
        {
            // Both list have valid entries.  Compare the two and take the
            // one that appears in the buffer first.
            rcvBuffer *Full, *Pend;

            Full = CONTAINING_RECORD(*CurrFull,
                                     rcvBuffer,
                                     listEntry);
            Pend = CONTAINING_RECORD(*CurrPend,
                                     rcvBuffer,
                                     listEntry);

            if (Full->dataBuf < Pend->dataBuf)
            {
                // Full is the winner.  Take it.

                Result = Full;
                *CurrFull = (*CurrFull)->Flink;
            }
            else
            {
                // Pend is the winner.  Take it.

                Result = Pend;
                *CurrPend = (*CurrPend)->Flink;
            }
        }
    }

    if (Result)
    {
        ASSERT(Result->isDmaBuf);
    }

    return Result;
}

BOOLEAN SynchronizedFindLargestSpace(IN PVOID Context)
{
    DMASPACE *Space = Context;
    IrDevice *thisDev = Space->thisDev;
    BOOLEAN Result;
    PLIST_ENTRY Full, Pend;
    rcvBuffer *Current = NULL;

    ASSERT(sizeof(ULONG)==sizeof(PVOID));


    if (IsListEmpty(&thisDev->rcvBufFull) && IsListEmpty(&thisDev->rcvBufPend))
    {
        Space->Offset = (ULONG_PTR)thisDev->dmaReadBuf;
        Space->Length = RCV_DMA_SIZE;
    }
    else
    {
        ULONG_PTR EndOfLast;
        ULONG_PTR ThisSpace;

        Full = thisDev->rcvBufFull.Flink;
        Pend = thisDev->rcvBufPend.Flink;

        Space->Length = 0;

        EndOfLast = Space->Offset = (ULONG_PTR)thisDev->dmaReadBuf;

        Current = GetNextPacket(Space, &Full, &Pend);
        while (Current)
        {
            // It's possible to get a packet that is from SIR.  If so, skip it.

            if (Current->isDmaBuf)
            {
                ASSERT((ULONG_PTR)Current->dataBuf >= EndOfLast);

                ThisSpace = (ULONG_PTR)Current->dataBuf - EndOfLast;

                // ASSERT the pointer is actually in our DMA buffer.
                ASSERT(Current->dataBuf >= thisDev->dmaReadBuf ||
                       Current->dataBuf < thisDev->dmaReadBuf+RCV_DMA_SIZE);

                if (ThisSpace > Space->Length)
                {
                    Space->Offset = EndOfLast;
                    Space->Length = ThisSpace;
                }

                EndOfLast = (ULONG_PTR)Current->dataBuf + Current->dataLen;
            }


            Current = GetNextPacket(Space, &Full, &Pend);
        }

        // Now we do one more calculation for the space after the end of the list.

        ThisSpace = (ULONG_PTR)thisDev->dmaReadBuf + RCV_DMA_SIZE - (ULONG_PTR)EndOfLast;

        if (ThisSpace > Space->Length)
        {
            Space->Offset = EndOfLast;
            Space->Length = ThisSpace;
        }

        // Round off to start DMA on 4 byte boundary
        Space->Length -= 4 - (Space->Offset & 3);
        Space->Offset = (Space->Offset+3) & (~3);
    }

    // We want space relative to start of buffer.
    Space->Offset -= (ULONG_PTR)thisDev->dmaReadBuf;

    Result = (Space->Length >= MAX_RCV_DATA_SIZE + FAST_IR_FCS_SIZE);

    if (!Result)
    {
        DEBUGFIR(DBG_ERR, ("NSC: ERROR: Not enough space to DMA full packet %x\n", thisDev));
    }

    return Result;
}

BOOLEAN FindLargestSpace(IN IrDevice *thisDev,
                         OUT PULONG_PTR pOffset,
                         OUT PULONG_PTR pLength)
{
    DMASPACE Space;
    BOOLEAN Result;

    Space.Offset = 0;
    Space.Length = 0;
    Space.thisDev = thisDev;

    Result = NdisMSynchronizeWithInterrupt(
                                           &thisDev->interruptObj,
                                           SynchronizedFindLargestSpace,
                                           &Space
                                           );


    *pOffset = Space.Offset;
    *pLength = Space.Length;

    return Result;
}




void FIR_DeliverFrames(IrDevice *thisDev)
{
    UCHAR frameStat;
    NDIS_STATUS stat;
    ULONG rcvFrameSize;
    PUCHAR NewFrame;

    UCHAR   BytesInFifo=0;
    ULONG_PTR LastReadDMACount, EndOfData;
    BOOLEAN resetDma = FALSE, OverflowOccurred = FALSE;
    BOOLEAN Discarding = thisDev->DiscardNextPacketSet;
    const    UINT    fcsSize    =    (   thisDev->currentSpeed    >=    MIN_FIR_SPEED)    ?
                                     FAST_IR_FCS_SIZE : MEDIUM_IR_FCS_SIZE;

    LOG("==> FIR_DeliverFrames, prev dma=%d",(ULONG)thisDev->LastReadDMACount);
    DEBUGFIR(DBG_RX|DBG_OUT, ("NSC: ==> FIR_DeliverFrames(0x%x)\n", thisDev));

    LastReadDMACount = NdisMReadDmaCounter(thisDev->DmaHandle);

    // Check for data received since the last time we were here.
    // We also check for data in fifo.  If there is some, we wait, as long
    // as the DMA still has room to capture data.

    if (LastReadDMACount > 0) {
        //
        //  we have not transfered all of the data possible into the recieve area.
        //  See if we have read more since the last time we were here
        //
        if (((LastReadDMACount < thisDev->LastReadDMACount) || (BytesInFifo=SyncReadBankReg(&thisDev->interruptObj,thisDev->portInfo.ioBase, 2, 7) & 0x3f))) {
            //
            //  we transfered something since
            //  the last interrupt or there are still bytes in the fifo, then set a timer
            //

            thisDev->LastReadDMACount = LastReadDMACount;

            //
            //  Set Timer Enable bit for another Timeout.
            //
            thisDev->FirIntMask = 0x90;
            SyncWriteBankReg(&thisDev->interruptObj,thisDev->portInfo.ioBase, 4, 2, 0x01);
            LOG_FIR("<== FIR_DeliverFrames- Enable timer, fifo=%02x, LastDma=%d",BytesInFifo,(ULONG)LastReadDMACount);
            DEBUGFIR(DBG_RX|DBG_OUT, ("NSC: <== FIR_DeliverFrames\n"));
            return;
        }
    } else {
        //
        //  The dma count has gone to zero so we have transfered all we possibly can,
        //  see if any thing is left in the fifo
        //
        BytesInFifo=SyncReadBankReg(&thisDev->interruptObj,thisDev->portInfo.ioBase, 2, 7) & 0x3f;

        LOG("Receive: dma transfer complete, bytes in fifo=%d",BytesInFifo);

        //
        //  we are stopping because the dma buffer filled up, not because the
        //  receiver was idle
        //
        thisDev->ForceTurnAroundTimeout=TRUE;

    }

    RegStats.RxDPC_Window++;


    //
    //  stop the dma transfer, so the data in the buffer will be valid
    //
    stat=CompleteDmaTransferFromDevice(&thisDev->DmaUtil);

    //
    //  see how was transfer now that we have stopped the dma
    //
    LastReadDMACount = NdisMReadDmaCounter(thisDev->DmaHandle);

    if (stat != NDIS_STATUS_SUCCESS) {
        DBGERR(("NdisMCompleteDmaTransfer failed: %d\n", stat));
        ASSERT(0);
        //
        //  could not complete the DMA, make it appear that zero bytes were transfered
        //
        LastReadDMACount=thisDev->rcvDmaSize;
    }

    thisDev->FirReceiveDmaActive=FALSE;

    thisDev->DiscardNextPacketSet = FALSE;

    EndOfData = thisDev->rcvDmaOffset + (thisDev->rcvDmaSize - LastReadDMACount);

    LOG("Recieve: Total data transfered %d",(ULONG)(thisDev->rcvDmaSize - LastReadDMACount));

    SyncGetFifoStatus(
        &thisDev->interruptObj,
        thisDev->portInfo.ioBase,
        &frameStat,
        &rcvFrameSize
        );

    if (frameStat == 0) LOG_ERROR("Receive: no frames in fifo");

    LOG("frameStat: %x, size=%d ", (UINT) frameStat,rcvFrameSize);
    DEBUGFIR(DBG_RX|DBG_OUT, ("NSC: frameStat = %xh\n", (UINT) frameStat));

    while ((frameStat & 0x80) && thisDev->rcvPktOffset < EndOfData) {

        /*
         *  This status byte is valid; see what else it says...
         *  Also mask off indeterminate bit.
         */
        frameStat &= ~0xA0;

        /*
         * Whether the frame is good or bad, we must read the counter
         * FIFO to synchronize it with the frame status FIFO.
         */
        if (Discarding)
        {
            LOG_ERROR("receive error: disc stat=%02x, lost=%d\n",frameStat,rcvFrameSize);
             // Do nothing
        }
        else if (frameStat != 0) {
            /*
             *  Some rcv error occurred. Reset DMA.
             */

            LOG_ERROR("Receive error: stat=%02x, lost=%d\n",frameStat,rcvFrameSize);

            DEBUGFIR(DBG_RX|DBG_ERR, ("NSC: RCV ERR: frameStat=%xh FrameSize=%d \n",
                                      (UINT)frameStat,rcvFrameSize));

            if (frameStat & 0x40) {
                if (frameStat & 0x01) {
                    RegStats.StatusFIFOOverflows++;
                }
                if (frameStat & 0x02) {
                    RegStats.ReceiveFIFOOverflows++;
                }
                RegStats.MissedPackets += rcvFrameSize;
            }
            else{
                if (frameStat & 0x01) {
                    RegStats.StatusFIFOOverflows++;
                }
                if (frameStat & 0x02) {
                    RegStats.ReceiveFIFOOverflows++;
                }
                if (frameStat & 0x04) {
                    RegStats.ReceiveCRCErrors++;
                }
                if (frameStat & 0x08) {
                }
                if (frameStat & 0x10) {
                }
                LOG("Bytes Lost: %d",rcvFrameSize);
                ASSERT((thisDev->rcvPktOffset + rcvFrameSize)<= RCV_DMA_SIZE);

                /* Advance pointer past bad packet.  */
                thisDev->rcvPktOffset += rcvFrameSize;
            }
        }

        else if (thisDev->rcvPktOffset + rcvFrameSize > EndOfData )
        {

            LOG_ERROR("Receive: Frame extends beyond where dma control wrote: offset=%x, Frame size=%x, EndOfData=%x",(ULONG)thisDev->rcvPktOffset,(ULONG)rcvFrameSize,(ULONG)EndOfData);

            DBGERR(("Packet won't fit in received data!\n"));
            DBGERR(("rcvPktOffset:%x rcvFrameSize:%x LastReadDmaCount:%x\n",
                     thisDev->rcvPktOffset,
                     rcvFrameSize,
                     LastReadDMACount));
            DBGERR(("rcvDmaOffset:%x rcvDmaSize:%x EndOfData:%x\n",
                     thisDev->rcvDmaOffset, thisDev->rcvDmaSize, EndOfData));

            //
            //  The frame seems to have extended past the end of where we dma the data.
            //  This should only happen if the dma space just less than the size of
            //  the fifo too small. The remaining data is still sitting in the fifo
            //  Attempt to read it out so it will be empty when we got read more frames
            //
            while (BytesInFifo > 0) {

                SyncReadBankReg(&thisDev->interruptObj,thisDev->portInfo.ioBase, BANK_0, TXD_RXD_OFFSET);
                BytesInFifo--;
            }


            BytesInFifo=SyncReadBankReg(&thisDev->interruptObj,thisDev->portInfo.ioBase, 2, 7) & 0x3f;

            if (BytesInFifo > 0) {
                //
                //  still bytes in the fifo after attmpting to read them out.
                //  Another frame has probably started showing up, Can trust the data collected
                //  so mark these frames to be discarded
                //
                LOG_ERROR("Receive: Still have bytes in the fifo after attempting to flush, %d bytes remaining",BytesInFifo);

                BytesInFifo=0;

                thisDev->DiscardNextPacketSet = TRUE;
            }
            //
            //  this should be the last frame to be received with out error, advance this pointer
            //  anyway.
            //
            thisDev->rcvPktOffset += rcvFrameSize;

        }

        else {

            DEBUGFIR(DBG_RX|DBG_OUT, ("NSC:  *** >>> FIR_DeliverFrames DMA offset 0x%x:\n",
                                      thisDev->rcvDmaOffset));

            //
            //  this is where the new frame starts
            //
#pragma prefast(suppress:12008, "This summation is guarenteed to be valid");
            NewFrame = thisDev->dmaReadBuf + thisDev->rcvPktOffset;

            //
            //  the next frame will start after the end of this frame
            //
            thisDev->rcvPktOffset += rcvFrameSize;

            ASSERT(thisDev->rcvPktOffset < RCV_DMA_SIZE);

            //
            //  the FCS is included in the length of the frame, remove that from the length
            //  we send to the protocol
            //
            rcvFrameSize -= fcsSize;

            if (rcvFrameSize <= MAX_NDIS_DATA_SIZE &&
                rcvFrameSize >= IR_ADDR_SIZE + IR_CONTROL_SIZE)
            {
                //
                // Queue this rcv packet.  Move Newframe pointer
                // into RxDMA buffer.
                //
                RegStats.ReceivedPackets++;
                RegStats.RxWindow++;
                QueueReceivePacket(thisDev, NewFrame, rcvFrameSize, TRUE);
            }
            else {
                LOG("Error: invalid packet size in FIR_DeliverFrames %d", rcvFrameSize);

                DEBUGFIR(DBG_RX|DBG_ERR, ("NSC: invalid packet size in FIR_DeliverFrames; %xh > %xh\n", rcvFrameSize, MAX_RCV_DATA_SIZE));
                //
                // Discard the rest of the packets.
                //
                while (SyncReadBankReg(&thisDev->interruptObj,thisDev->portInfo.ioBase, 5, 5)&0x80)
                {
                    SyncReadBankReg(&thisDev->interruptObj,thisDev->portInfo.ioBase, 5, 6);
                    SyncReadBankReg(&thisDev->interruptObj,thisDev->portInfo.ioBase, 5, 7);
                }
                thisDev->DiscardNextPacketSet = TRUE;
            }
        }

        SyncGetFifoStatus(
            &thisDev->interruptObj,
            thisDev->portInfo.ioBase,
            &frameStat,
            &rcvFrameSize
            );

        LOG("frameStat: %x, size=%d ", (UINT) frameStat,rcvFrameSize);
        DEBUGFIR(DBG_RX|DBG_OUT, ("NSC: frameStat = %xh\n", (UINT) frameStat));

        //
        // Clear the line status register, of any past events.
        //
        thisDev->LineStatus = SyncReadBankReg(&thisDev->interruptObj,thisDev->portInfo.ioBase, 0, 5);

    }

    thisDev->FirIntMask = 0x04;

    SetupRecv(thisDev);

    LOG("<== FIR_DeliverFrames");
    DEBUGFIR(DBG_RX|DBG_OUT, ("NSC: <== FIR_DeliverFrames\n"));
}

BOOLEAN NSC_Setup(IrDevice *thisDev)
{
    NDIS_DMA_DESCRIPTION DMAChannelDcr;
    NDIS_STATUS stat;

    /*
     *  Initialize rcv DMA channel
     */
    RtlZeroMemory(&DMAChannelDcr,sizeof(DMAChannelDcr));

    DMAChannelDcr.DemandMode = TRUE;
    DMAChannelDcr.AutoInitialize = FALSE;
    DMAChannelDcr.DmaChannelSpecified = FALSE;
    DMAChannelDcr.DmaWidth = Width8Bits;
    DMAChannelDcr.DmaSpeed = Compatible;
    DMAChannelDcr.DmaPort = 0;
    DMAChannelDcr.DmaChannel = thisDev->portInfo.DMAChannel; // 0;

    stat = NdisMRegisterDmaChannel(&thisDev->DmaHandle,
                                   thisDev->ndisAdapterHandle,
                                   thisDev->portInfo.DMAChannel,
                                   FALSE, &DMAChannelDcr, RCV_DMA_SIZE);

    if (stat != NDIS_STATUS_SUCCESS) {

        DEBUGFIR(DBG_ERR, ("NSC: NdisMRegisterDmaChannel failed\n"));
        return FALSE;
    }

    InitializeDmaUtil(
        &thisDev->DmaUtil,
        thisDev->DmaHandle
        );


    thisDev->rcvDmaOffset = 0;

    /*
     *  Because we enable rcv DMA while SIR receives may still be
     *  going on, we need to keep a separate receive buffer for DMA.
     *  This buffer gets swapped with the rcvBuffer data pointer
     *  and must be the same size.
     */
    thisDev->dmaReadBuf=NscAllocateDmaBuffer(
        thisDev->ndisAdapterHandle,
        RCV_DMA_SIZE,
        &thisDev->ReceiveDmaBufferInfo
        );

    if (thisDev->dmaReadBuf == NULL) {

        return FALSE;
    }

    thisDev->TransmitDmaBuffer=NscAllocateDmaBuffer(
        thisDev->ndisAdapterHandle,
        MAX_IRDA_DATA_SIZE,
        &thisDev->TransmitDmaBufferInfo
        );


    NdisAllocateBufferPool(&stat, &thisDev->dmaBufferPoolHandle, 2);

    if (stat != NDIS_STATUS_SUCCESS){
        LOG("Error: NdisAllocateBufferPool failed in NSC_Setup");
        DEBUGFIR(DBG_ERR, ("NSC: NdisAllocateBufferPool failed in NSC_Setup\n"));
        return FALSE;
    }

    NdisAllocateBuffer(&stat, &thisDev->rcvDmaBuffer,
                       thisDev->dmaBufferPoolHandle,
                       thisDev->dmaReadBuf,
                       RCV_DMA_SIZE
                       );

    if (stat != NDIS_STATUS_SUCCESS) {
        LOG("Error: NdisAllocateBuffer failed (rcv) in NSC_Setup");
        DEBUGFIR(DBG_ERR, ("NSC: NdisAllocateBuffer failed (rcv) in NSC_Setup\n"));
        return FALSE;
    }

    NdisAllocateBuffer(&stat, &thisDev->xmitDmaBuffer,
                       thisDev->dmaBufferPoolHandle,
                       thisDev->TransmitDmaBuffer,
                       MAX_IRDA_DATA_SIZE
                       );

    if (stat != NDIS_STATUS_SUCCESS) {
        LOG("NdisAllocateBuffer failed (xmit) in NSC_Setup");
        DEBUGFIR(DBG_ERR, ("NSC: NdisAllocateBuffer failed (xmit) in NSC_Setup\n"));
        return FALSE;
    }




    return TRUE;
}


void NSC_Shutdown(IrDevice *thisDev)
{

    if (thisDev->xmitDmaBuffer){
        NdisFreeBuffer(   thisDev->xmitDmaBuffer);
        thisDev->xmitDmaBuffer = NULL;
    }

    if (thisDev->rcvDmaBuffer){
        NdisFreeBuffer(thisDev->rcvDmaBuffer);
        thisDev->rcvDmaBuffer = NULL;
    }

    if (thisDev->dmaBufferPoolHandle){
        NdisFreeBufferPool(thisDev->dmaBufferPoolHandle);
        thisDev->dmaBufferPoolHandle = NULL;
    }

    if (thisDev->dmaReadBuf){

        NscFreeDmaBuffer(&thisDev->ReceiveDmaBufferInfo);
        thisDev->dmaReadBuf = NULL;
    }

    if (thisDev->TransmitDmaBuffer){

        NscFreeDmaBuffer(&thisDev->TransmitDmaBufferInfo);
        thisDev->TransmitDmaBuffer = NULL;
    }


    if (thisDev->DmaHandle){
        NdisMDeregisterDmaChannel(thisDev->DmaHandle);
        thisDev->DmaHandle = NULL;
    }

}


BOOLEAN
NdisToFirPacket(
    PNDIS_PACKET   Packet,
    UCHAR         *irPacketBuf,
    UINT           TotalDmaBufferLength,
    UINT          *ActualTransferLength
    )
{
    PNDIS_BUFFER ndisBuf;
    UINT ndisPacketBytes = 0;
    UINT ndisPacketLen;

    LOG("==> NdisToFirPacket");
    DEBUGFIR(DBG_OUT, ("NSC: ==> NdisToFirPacket\n"));

    /*
     *  Get the packet's entire length and its first NDIS buffer
     */
    NdisQueryPacket(Packet, NULL, NULL, &ndisBuf, &ndisPacketLen);

    LOG("NdisToFirPacket, number of bytes: %d", ndisPacketLen);
    DEBUGFIR(DBG_OUT, ("NSC: NdisToFirPacket, number of bytes: %d\n", ndisPacketLen));

    /*
     *  Make sure that the packet is big enough to be legal.
     *  It consists of an A, C, and variable-length I field.
     */
    if (ndisPacketLen < IR_ADDR_SIZE + IR_CONTROL_SIZE){
        LOG("Error: packet too short in %d", ndisPacketLen);
        DEBUGFIR(DBG_ERR, ("NSC: packet too short in NdisToFirPacket (%d bytes)\n", ndisPacketLen));
        return FALSE;
    }

    /*
     *  Make sure that we won't overwrite our contiguous buffer.
     */
    if (ndisPacketLen > TotalDmaBufferLength){
        /*
         *  The packet is too large
         *  Tell the caller to retry with a packet size large
         *  enough to get past this stage next time.
         */
        LOG("Error: packet too large in %d ", ndisPacketLen);
        DEBUGFIR(DBG_ERR, ("NSC: Packet too large in NdisToIrPacket (%d=%xh bytes), MAX_IRDA_DATA_SIZE=%d, TotalDmaBufferLength=%d.\n",
                           ndisPacketLen, ndisPacketLen, MAX_IRDA_DATA_SIZE, TotalDmaBufferLength));
        *ActualTransferLength = ndisPacketLen;

        return FALSE;
    }


    /*
     *  Read the NDIS packet into a contiguous buffer.
     *  We have to do this in two steps so that we can compute the
     *  FCS BEFORE applying escape-byte transparency.
     */
    while (ndisBuf) {
        UCHAR *bufData;
        UINT bufLen;

        NdisQueryBufferSafe(ndisBuf, (PVOID *)&bufData, &bufLen,NormalPagePriority);

        // Validate the packet size, test for possible integer overflow
        //
        if (bufData==NULL ||
            ((ULONGLONG)ndisPacketBytes + (ULONGLONG)bufLen > (ULONGLONG)ndisPacketLen)){
            /*
             *  Packet was corrupt -- it misreported its size.
             */
            *ActualTransferLength = 0;
            ASSERT(0);
            return FALSE;
        }

        NdisMoveMemory((PVOID)(irPacketBuf+ndisPacketBytes),
                       (PVOID)bufData, (ULONG)bufLen);
        ndisPacketBytes += bufLen;

        NdisGetNextBuffer(ndisBuf, &ndisBuf);
    }

    if (WPP_LEVEL_ENABLED(DBG_LOG_INFO)) {

        UCHAR   CommandByte=*(irPacketBuf+1);
        UCHAR   Nr=CommandByte >> 5;
        UCHAR   Ns=(CommandByte >> 1) & 0x7;
        UCHAR   Pf=(CommandByte >> 4) & 0x1;

        if ((CommandByte & 1) == 0) {

            LOG("Sending - I frame, Nr=%d, Ns=%d p/f=%d",Nr,Ns,Pf);

        } else {

            if ((CommandByte & 0x3) == 0x1) {

                LOG("Sending - S frame, Nr=%d, xx=%d, p/f=%d",Nr, (CommandByte > 2) & 0x3, Pf);

            } else {

                LOG("Sending - U frame, p/f=%d",Pf);
            }
        }
    }

    /*
     *  Do a sanity check on the length of the packet.
     */
    if (ndisPacketBytes != ndisPacketLen){
        /*
         *  Packet was corrupt -- it misreported its size.
         */
        LOG("Error: Packet corrupt in NdisToIrPacket "
            "(buffer lengths don't add up to packet length)");
        DEBUGFIR(DBG_ERR, ("NSC: Packet corrupt in NdisToIrPacket (buffer lengths don't add up to packet length).\n"));
        *ActualTransferLength = 0;
        return FALSE;
    }

#ifdef DBG_ADD_PKT_ID
    if (addPktIdOn){
        static USHORT uniqueId = 0;
        DEBUGFIR(DBG_OUT, ("NSC:  *** --> SEND PKT ID: %xh\n", (UINT)uniqueId));
        LOG("ID: Send (FIR) Pkt id: %x", uniqueId);
        *(USHORT *)(irPacketBuf+ndisPacketBytes) = uniqueId++;
        ndisPacketBytes += sizeof(USHORT);
    }
#endif

    *ActualTransferLength = ndisPacketBytes;

    LOG("<== NdisToFirPacket");
    DEBUGFIR(DBG_OUT, ("NSC: <== NdisToFirPacket\n"));

    return TRUE;
}

