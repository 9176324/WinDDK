/*++

Copyright (C) Microsoft Corporation, 1991 - 1999

Module Name:

    retry.c

Abstract:

    Packet retry routines for CLASSPNP

Environment:

    kernel mode only

Notes:


Revision History:

--*/

#include "classp.h"
#include "debug.h"

#ifdef DEBUG_USE_WPP
#include "retry.tmh"
#endif


/*
 *  InterpretTransferPacketError
 *
 *      Interpret the SRB error into a meaningful IRP status.
 *      ClassInterpretSenseInfo also may modify the SRB for the retry.
 *
 *      Return TRUE iff packet should be retried.
 */
BOOLEAN InterpretTransferPacketError(PTRANSFER_PACKET Pkt)
{
    PFUNCTIONAL_DEVICE_EXTENSION fdoExtension = Pkt->Fdo->DeviceExtension;
    PCLASS_PRIVATE_FDO_DATA fdoData = fdoExtension->PrivateFdoData;
    ULONG timesAlreadyRetried;
    BOOLEAN shouldRetry = FALSE;
    PCDB pCdb = (PCDB)Pkt->Srb.Cdb;

    /*
     *  Interpret the error using the returned sense info first.
     */
    Pkt->RetryIn100nsUnits = 0;

    /*
     *  Pre-calculate the number of times the IO has already been
     *  retried, so that all InterpretSenseInfo routines get the right value.
     */
    if (pCdb->MEDIA_REMOVAL.OperationCode == SCSIOP_MEDIUM_REMOVAL)
    {
        timesAlreadyRetried = NUM_LOCKMEDIAREMOVAL_RETRIES - Pkt->NumRetries;
    }
    else if ((pCdb->MODE_SENSE.OperationCode == SCSIOP_MODE_SENSE) ||
             (pCdb->MODE_SENSE.OperationCode == SCSIOP_MODE_SENSE10))
    {
        timesAlreadyRetried = NUM_MODESENSE_RETRIES - Pkt->NumRetries;
    }
    else if ((pCdb->CDB10.OperationCode == SCSIOP_READ_CAPACITY) ||
             (pCdb->CDB16.OperationCode == SCSIOP_READ_CAPACITY16))
    {
        timesAlreadyRetried = NUM_DRIVECAPACITY_RETRIES - Pkt->NumRetries;
    }
    else if (IS_SCSIOP_READWRITE(pCdb->CDB10.OperationCode))
    {
        timesAlreadyRetried = NUM_IO_RETRIES - Pkt->NumRetries;
    }
    else
    {
        TracePrint((TRACE_LEVEL_ERROR, TRACE_FLAG_GENERAL, "Unhandled SRB Function %xh in error path for packet %p (did miniport change Srb.Cdb.OperationCode ?)", (ULONG)pCdb->CDB10.OperationCode, Pkt));
        timesAlreadyRetried = 0;
    }


    if (fdoData->InterpretSenseInfo != NULL) {

        // SAL annotation and ClassInitializeEx() both validate this
        ASSERT(fdoData->InterpretSenseInfo->Interpret != NULL);
        
        shouldRetry = fdoData->InterpretSenseInfo->Interpret(Pkt->Fdo,
                                                             Pkt->OriginalIrp,
                                                             &Pkt->Srb,
                                                             IRP_MJ_SCSI,
                                                             0,
                                                             timesAlreadyRetried,
                                                             Pkt->RetryHistory,
                                                             &Pkt->Irp->IoStatus.Status,
                                                             &Pkt->RetryIn100nsUnits);
    }
    else if (pCdb->MEDIA_REMOVAL.OperationCode == SCSIOP_MEDIUM_REMOVAL){

        ULONG retryIntervalSeconds = 0;
        /*
         *  This is an Ejection Control SRB.  Interpret its sense info specially.
         */
        shouldRetry = ClassInterpretSenseInfo(
                            Pkt->Fdo,
                            &Pkt->Srb,
                            IRP_MJ_SCSI,
                            0,
                            timesAlreadyRetried,
                            &Pkt->Irp->IoStatus.Status,
                            &retryIntervalSeconds);
        if (shouldRetry){
            /*
             *  If the device is not ready, wait at least 2 seconds before retrying.
             */
            PSENSE_DATA senseInfoBuffer = Pkt->Srb.SenseInfoBuffer;
            ASSERT(senseInfoBuffer);
            if (((Pkt->Irp->IoStatus.Status == STATUS_DEVICE_NOT_READY) &&
                 (senseInfoBuffer->AdditionalSenseCode == SCSI_ADSENSE_LUN_NOT_READY)) ||
                (SRB_STATUS(Pkt->Srb.SrbStatus) == SRB_STATUS_SELECTION_TIMEOUT)){

                retryIntervalSeconds = MAX(retryIntervalSeconds, 2);
            }
        }


        if (shouldRetry)
        {
            Pkt->RetryIn100nsUnits = retryIntervalSeconds;
            Pkt->RetryIn100nsUnits *= 1000 * 1000 * 10;
        }
    }
    else if ((pCdb->MODE_SENSE.OperationCode == SCSIOP_MODE_SENSE) ||
            (pCdb->MODE_SENSE.OperationCode == SCSIOP_MODE_SENSE10)){

        ULONG retryIntervalSeconds = 0;
        /*
         *  This is an Mode Sense SRB.  Interpret its sense info specially.
         */
        shouldRetry = ClassInterpretSenseInfo(
                            Pkt->Fdo,
                            &Pkt->Srb,
                            IRP_MJ_SCSI,
                            0,
                            timesAlreadyRetried,
                            &Pkt->Irp->IoStatus.Status,
                            &retryIntervalSeconds);
        if (shouldRetry){
            /*
             *  If the device is not ready, wait at least 2 seconds before retrying.
             */
            PSENSE_DATA senseInfoBuffer = Pkt->Srb.SenseInfoBuffer;
            ASSERT(senseInfoBuffer);
            if (((Pkt->Irp->IoStatus.Status == STATUS_DEVICE_NOT_READY) &&
                (senseInfoBuffer->AdditionalSenseCode == SCSI_ADSENSE_LUN_NOT_READY)) ||
                    (SRB_STATUS(Pkt->Srb.SrbStatus) == SRB_STATUS_SELECTION_TIMEOUT)){

                retryIntervalSeconds = MAX(retryIntervalSeconds, 2);
            }
        }

        /*
         *  Some special cases for mode sense.
         */
        if (Pkt->Irp->IoStatus.Status == STATUS_VERIFY_REQUIRED){
            shouldRetry = TRUE;
        }
        else if (SRB_STATUS(Pkt->Srb.SrbStatus) == SRB_STATUS_DATA_OVERRUN){
            /*
             *  This is a HACK.
             *  Atapi returns SRB_STATUS_DATA_OVERRUN when it really means
             *  underrun (i.e. success, and the buffer is longer than needed).
             *  So treat this as a success.
             *  When the caller of this function sees that the status was changed to success,
             *  it will add the transferred length to the original irp.
             */
            Pkt->Irp->IoStatus.Status = STATUS_SUCCESS;
            shouldRetry = FALSE;
        }

        if (shouldRetry)
        {
            Pkt->RetryIn100nsUnits = retryIntervalSeconds;
            Pkt->RetryIn100nsUnits *= 1000 * 1000 * 10;
        }
    }
    else if ((pCdb->CDB10.OperationCode == SCSIOP_READ_CAPACITY) ||
             (pCdb->CDB16.OperationCode == SCSIOP_READ_CAPACITY16)) {

        ULONG retryIntervalSeconds = 0;

        /*
         *  This is a Drive Capacity SRB.  Interpret its sense info specially.
         */
        shouldRetry = ClassInterpretSenseInfo(
                            Pkt->Fdo,
                            &Pkt->Srb,
                            IRP_MJ_SCSI,
                            0,
                            timesAlreadyRetried,
                            &Pkt->Irp->IoStatus.Status,
                            &retryIntervalSeconds);
        if (Pkt->Irp->IoStatus.Status == STATUS_VERIFY_REQUIRED){
            shouldRetry = TRUE;
        }

        if (shouldRetry)
        {
            Pkt->RetryIn100nsUnits = retryIntervalSeconds;
            Pkt->RetryIn100nsUnits *= 1000 * 1000 * 10;
        }
    }
    else if (IS_SCSIOP_READWRITE(pCdb->CDB10.OperationCode)) {

        ULONG retryIntervalSeconds = 0;
        /*
         *  This is a Read/Write Data packet.
         */
        PIO_STACK_LOCATION origCurrentSp = IoGetCurrentIrpStackLocation(Pkt->OriginalIrp);

        shouldRetry = ClassInterpretSenseInfo(
                            Pkt->Fdo,
                            &Pkt->Srb,
                            origCurrentSp->MajorFunction,
                            0,
                            timesAlreadyRetried,
                            &Pkt->Irp->IoStatus.Status,
                            &retryIntervalSeconds);
        /*
         *  Deal with some special cases.
         */
        if (Pkt->Irp->IoStatus.Status == STATUS_INSUFFICIENT_RESOURCES){
            /*
             *  We are in extreme low-memory stress.
             *  We will retry in smaller chunks.
             */
            shouldRetry = TRUE;
        }
        else if (TEST_FLAG(origCurrentSp->Flags, SL_OVERRIDE_VERIFY_VOLUME) &&
                (Pkt->Irp->IoStatus.Status == STATUS_VERIFY_REQUIRED)){
            /*
             *  We are still verifying a (possibly) reloaded disk/cdrom.
             *  So retry the request.
             */
            Pkt->Irp->IoStatus.Status = STATUS_IO_DEVICE_ERROR;
            shouldRetry = TRUE;
        }

        if (shouldRetry)
        {
            Pkt->RetryIn100nsUnits = retryIntervalSeconds;
            Pkt->RetryIn100nsUnits *= 1000 * 1000 * 10;
        }
    }
    else {
        TracePrint((TRACE_LEVEL_ERROR, TRACE_FLAG_GENERAL, "Unhandled SRB Function %xh in error path for packet %p (did miniport change Srb.Cdb.OperationCode ?)", (ULONG)pCdb->CDB10.OperationCode, Pkt));
    }

    return shouldRetry;
}


/*
 *  RetryTransferPacket
 *
 *      Retry sending a TRANSFER_PACKET.
 *
 *      Return TRUE iff the packet is complete.
 *          (if so the status in pkt->irp is the final status).
 */
BOOLEAN RetryTransferPacket(PTRANSFER_PACKET Pkt)
{
    BOOLEAN packetDone;

    TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_GENERAL, "retrying failed transfer (pkt=%ph, op=%s)", Pkt, DBGGETSCSIOPSTR(&Pkt->Srb)));

    ASSERT(Pkt->NumRetries > 0 || Pkt->RetryHistory);
    Pkt->NumRetries--;

    //
    // If this is the last retry, then turn off disconnect, sync transfer,
    // and tagged queuing.  On all other retries, leave the original settings.
    //

    if ( 0 == Pkt->NumRetries ) {
        /*
         *  Tone down performance on the retry.
         *  This increases the chance for success on the retry.
         *  We've seen instances of drives that fail consistently but then start working
         *  once this scale-down is applied.
         */
        SET_FLAG(Pkt->Srb.SrbFlags, SRB_FLAGS_DISABLE_DISCONNECT);
        SET_FLAG(Pkt->Srb.SrbFlags, SRB_FLAGS_DISABLE_SYNCH_TRANSFER);
        CLEAR_FLAG(Pkt->Srb.SrbFlags, SRB_FLAGS_QUEUE_ACTION_ENABLE);
        Pkt->Srb.QueueTag = SP_UNTAGGED;
    }

    if (Pkt->Irp->IoStatus.Status == STATUS_INSUFFICIENT_RESOURCES) {

        PCDB pCdb = (PCDB)Pkt->Srb.Cdb;
        BOOLEAN isReadWrite = IS_SCSIOP_READWRITE(pCdb->CDB10.OperationCode);

        PFUNCTIONAL_DEVICE_EXTENSION fdoExtension = Pkt->Fdo->DeviceExtension;
        PCLASS_PRIVATE_FDO_DATA fdoData = fdoExtension->PrivateFdoData;

        if ((Pkt->DriverUsesStartIO) &&
            ( (pCdb->AsByte[0] == SCSIOP_WRITE6 ) ||
              (pCdb->AsByte[0] == SCSIOP_WRITE  ) ||
              (pCdb->AsByte[0] == SCSIOP_WRITE12) ||
              (pCdb->AsByte[0] == SCSIOP_WRITE16) )) {

            /* don't retry writes in super-low-memory conditions if the
             * driver must serialize against StartIO.  This is because
             * some write methods used in such drivers cannot accept
             * random-sized writes. (i.e CD-RW in packet writing mode)
             * Reads, however, are always safe to split up.
             */
            SET_FLAG(fdoData->TrackingFlags, TRACKING_FORWARD_PROGRESS_PATH1);
            packetDone = TRUE;
        }
        else if (Pkt->InLowMemRetry || !isReadWrite){
            /*
             *  This should never happen.
             *  The memory manager guarantees that at least four pages will
             *  be available to allow forward progress in the port driver.
             *  So a one-page transfer should never fail with insufficient resources.
             */
            SET_FLAG(fdoData->TrackingFlags, TRACKING_FORWARD_PROGRESS_PATH2);
            ASSERT(isReadWrite && !Pkt->InLowMemRetry);
            packetDone = TRUE;
        }
        else {
            /*
             *  We are in low-memory stress.
             *  Start the low-memory retry state machine, which tries to
             *  resend the packet in little one-page chunks.
             */
            SET_FLAG(fdoData->TrackingFlags, TRACKING_FORWARD_PROGRESS_PATH3);
            InitLowMemRetry(Pkt,
                            Pkt->BufPtrCopy,
                            Pkt->BufLenCopy,
                            Pkt->TargetLocationCopy);
            StepLowMemRetry(Pkt);
            packetDone = FALSE;
        }
    }
    else {
        /*
         *  Retry the packet by simply resending it after a delay.
         *  Put the packet back in the pending queue and
         *  schedule a timer to retry the transfer.
         *
         *  Do not call SetupReadWriteTransferPacket again because:
         *  (1)  The minidriver may have set some bits
         *       in the SRB that it needs again and
         *  (2)  doing so would reset numRetries.
         *
         *  BECAUSE we do not call SetupReadWriteTransferPacket again,
         *  we have to reset a couple fields in the SRB that
         *  some miniports overwrite when they fail an SRB.
         */

        Pkt->Srb.DataBuffer = Pkt->BufPtrCopy;
        Pkt->Srb.DataTransferLength = Pkt->BufLenCopy;

        TransferPacketQueueRetryDpc(Pkt);

        packetDone = FALSE;
    }

    return packetDone;
}


VOID TransferPacketQueueRetryDpc(PTRANSFER_PACKET Pkt)
{
    KeInitializeDpc(&Pkt->RetryTimerDPC, TransferPacketRetryTimerDpc, Pkt);

    if (Pkt->RetryIn100nsUnits == 0){
        KeInsertQueueDpc(&Pkt->RetryTimerDPC, NULL, NULL);
    }
    else {
        LARGE_INTEGER timerPeriod;

        ASSERT(Pkt->RetryIn100nsUnits < 100 * 1000 * 1000 * 10); // sanity check -- 100 seconds is normally too long
        timerPeriod.QuadPart = -(Pkt->RetryIn100nsUnits);
        KeInitializeTimer(&Pkt->RetryTimer);
        KeSetTimer(&Pkt->RetryTimer, timerPeriod, &Pkt->RetryTimerDPC);
    }
}


VOID TransferPacketRetryTimerDpc(   IN PKDPC Dpc,
                                    IN PVOID DeferredContext,
                                    IN PVOID SystemArgument1,
                                    IN PVOID SystemArgument2)
{
    PTRANSFER_PACKET pkt = (PTRANSFER_PACKET)DeferredContext;
    SubmitTransferPacket(pkt);
}


VOID InitLowMemRetry(PTRANSFER_PACKET Pkt, PVOID BufPtr, ULONG Len, LARGE_INTEGER TargetLocation)
{
    ASSERT(Len > 0);
    ASSERT(!Pkt->InLowMemRetry);

    if (Pkt->DriverUsesStartIO)
    {
        /*
         * special case: StartIO-based writing must stay serialized for performance
         * and proper operations (i.e. sequential writing mode).  If need more than
         * one transfer to perform this operation, and it's a StartIO-based driver
         * (such as CDROM), then just use a single packet and use the retry logic
         * that's already built-in to the packet engine.  Note that low-mem retry
         * cannot be used directly because some write methods do not work if the
         * writes are only PAGE_SIZE (i.e. packet writing may corrupt data).
         */
        Pkt->InLowMemRetry = FALSE;
    }
    else
    {
        Pkt->InLowMemRetry = TRUE;
    }
    Pkt->LowMemRetry_remainingBufPtr = BufPtr;
    Pkt->LowMemRetry_remainingBufLen = Len;
    Pkt->LowMemRetry_nextChunkTargetLocation = TargetLocation;
}


/*
 *  StepLowMemRetry
 *
 *      During extreme low-memory stress, this function retries
 *      a packet in small one-page chunks, sent serially.
 *
 *      Returns TRUE iff the packet is done.
 */
BOOLEAN StepLowMemRetry(PTRANSFER_PACKET Pkt)
{
    BOOLEAN packetDone;

    if (Pkt->LowMemRetry_remainingBufLen == 0){
        packetDone = TRUE;
    }
    else {
        ULONG thisChunkLen;
        if (Pkt->DriverUsesStartIO)
        {
            /*
             * Need the fdoData for the HwMaxXferLen
             */
            PFUNCTIONAL_DEVICE_EXTENSION fdoExt = Pkt->Fdo->DeviceExtension;
            PCLASS_PRIVATE_FDO_DATA fdoData = fdoExt->PrivateFdoData;

            /*
             * Need the adapterDesc to limit transfers based on byte count
             */
            PCOMMON_DEVICE_EXTENSION commonExtension = Pkt->Fdo->DeviceExtension;
            PSTORAGE_ADAPTER_DESCRIPTOR adapterDesc = commonExtension->PartitionZeroExtension->AdapterDescriptor;

            ULONG hwMaxXferLen;

            /*
             * special case: StartIO-based writing must stay serialized for performance
             * and proper operations (i.e. sequential writing mode).  If need more than
             * one transfer to perform this operation, and it's a StartIO-based driver
             * (such as CDROM), then just use a single packet and use the retry logic
             * that's already built-in to the packet engine.  Note that low-mem retry
             * cannot be used directly because some write methods do not work if the
             * writes are only PAGE_SIZE (i.e. packet writing may corrupt data).
             */
            ASSERT(!Pkt->InLowMemRetry);

            /*
             *  We precomputed fdoData->HwMaxXferLen using (MaximumPhysicalPages-1).
             *  If the buffer is page-aligned, that's one less page crossing so we can add the page back in.
             *  Note: adapters that return MaximumPhysicalPages=0x10 depend on this to
             *           transfer aligned 64K requests in one piece.
             *  Also note:  make sure adding PAGE_SIZE back in doesn't wrap to zero.
             */

            if (((ULONG_PTR)(Pkt->LowMemRetry_remainingBufPtr) & (PAGE_SIZE-1)) || (fdoData->HwMaxXferLen > 0xffffffff-PAGE_SIZE)){
                hwMaxXferLen = fdoData->HwMaxXferLen;
            }
            else {
                ASSERT((PAGE_SIZE%fdoExt->DiskGeometry.BytesPerSector) == 0);
                hwMaxXferLen = min(fdoData->HwMaxXferLen+PAGE_SIZE, adapterDesc->MaximumTransferLength);
            }
            thisChunkLen = MIN(Pkt->LowMemRetry_remainingBufLen, hwMaxXferLen);
        }
        else {
            /*
             *  Make sure the little chunk we send is <= a page length
             *  AND that it does not cross any page boundaries.
             */
            ULONG bytesToNextPageBoundary;
            bytesToNextPageBoundary = PAGE_SIZE-(ULONG)((ULONG_PTR)Pkt->LowMemRetry_remainingBufPtr%PAGE_SIZE);
            thisChunkLen = MIN(Pkt->LowMemRetry_remainingBufLen, bytesToNextPageBoundary);
            ASSERT(Pkt->InLowMemRetry);
        }


        /*
         *  Set up the transfer packet for the new little chunk.
         *  This will reset numRetries so that we retry each chunk as required.
         */
        SetupReadWriteTransferPacket(Pkt,
                                     Pkt->LowMemRetry_remainingBufPtr,
                                     thisChunkLen,
                                     Pkt->LowMemRetry_nextChunkTargetLocation,
                                     Pkt->OriginalIrp);

        Pkt->LowMemRetry_remainingBufPtr += thisChunkLen;
        Pkt->LowMemRetry_remainingBufLen -= thisChunkLen;
        Pkt->LowMemRetry_nextChunkTargetLocation.QuadPart += thisChunkLen;

        //
        // When running in low-memory stress, always use a partial MDL.
        // This allows lower drivers to potentially map a smaller buffer.
        //
        Pkt->UsePartialMdl = TRUE;

        TransferPacketQueueRetryDpc(Pkt);

        packetDone = FALSE;
    }

    return packetDone;
}

