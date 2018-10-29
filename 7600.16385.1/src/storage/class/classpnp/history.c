/*++

Copyright (C) Microsoft Corporation, 1991 - 1999

Module Name:

    history.c

Abstract:

    Packet history routines for CLASSPNP

Environment:

    kernel mode only

Notes:


Revision History:

--*/

#include "classp.h"
#include "debug.h"

#ifdef DEBUG_USE_WPP
#include "history.tmh"
#endif

//#ifdef ALLOC_PRAGMA
//    #pragma alloc_text(PAGE, InitializeTransferPackets)
//#endif

VOID HistoryInitializeRetryLogs(__out PSRB_HISTORY History, ULONG HistoryCount) {
    ULONG tmpSize = HistoryCount * sizeof(SRB_HISTORY_ITEM);
    tmpSize += sizeof(SRB_HISTORY) - sizeof(SRB_HISTORY_ITEM);
    RtlZeroMemory(History, tmpSize);
    History->TotalHistoryCount = HistoryCount;
    return;
}


VOID HistoryLogSendPacket(TRANSFER_PACKET * Pkt) {

    PSRB_HISTORY history;
    PSRB_HISTORY_ITEM item;

    ASSERT( Pkt->RetryHistory != NULL );
    history = Pkt->RetryHistory;
    // sending a packet implies a new history unit is to be used.
    ASSERT( history->UsedHistoryCount <= history->TotalHistoryCount );

    // if already all used up, request class driver to remove at least one history unit
    if (history->UsedHistoryCount == history->TotalHistoryCount )
    {
        PFUNCTIONAL_DEVICE_EXTENSION fdoExtension = Pkt->Fdo->DeviceExtension;
        PCLASS_PRIVATE_FDO_DATA fdoData = fdoExtension->PrivateFdoData;
        ASSERT( fdoData->InterpretSenseInfo != NULL );
        ASSERT( fdoData->InterpretSenseInfo->Compress != NULL );
        fdoData->InterpretSenseInfo->Compress( fdoExtension->DeviceObject, history );
        ASSERT( history->UsedHistoryCount < history->TotalHistoryCount );
    }

    // thus, since we are about to increment the count, it must now be less...
    ASSERT( history->UsedHistoryCount < history->TotalHistoryCount );

    // increment the number of history units in use
    history->UsedHistoryCount++;

    // determine index to use
    item = &( history->History[ history->UsedHistoryCount-1 ] );

    // zero out the history item
    RtlZeroMemory(item, sizeof(SRB_HISTORY_ITEM));

    // Query the tick count and store in the history
    KeQueryTickCount(&item->TickCountSent);
    return;
}

VOID HistoryLogReturnedPacket(TRANSFER_PACKET *Pkt) {

    PSRB_HISTORY history;
    PSRB_HISTORY_ITEM item;
    ULONG senseSize;

    ASSERT( Pkt->RetryHistory != NULL );
    history = Pkt->RetryHistory;
    ASSERT( history->UsedHistoryCount <= history->TotalHistoryCount );
    item = &( history->History[ history->UsedHistoryCount-1 ] );

    // Query the tick count and store in the history
    KeQueryTickCount(&item->TickCountCompleted);

    // Copy the SRB Status...
    item->SrbStatus = Pkt->Srb.SrbStatus;

    // Determine the amount of valid sense data
    if (Pkt->Srb.SenseInfoBufferLength >= RTL_SIZEOF_THROUGH_FIELD(SENSE_DATA, AdditionalSenseLength))
    {
        PSENSE_DATA sense = (PSENSE_DATA)Pkt->Srb.SenseInfoBuffer;
        senseSize =
            RTL_SIZEOF_THROUGH_FIELD(SENSE_DATA, AdditionalSenseLength) +
            sense->AdditionalSenseLength;
        senseSize = min(senseSize, sizeof(SENSE_DATA));
    }
    else
    {
        senseSize = Pkt->Srb.SenseInfoBufferLength;
    }
    // Normalize the sense data copy in the history
    RtlZeroMemory(&(item->NormalizedSenseData), sizeof(SENSE_DATA));
    RtlCopyMemory(&(item->NormalizedSenseData),
                  Pkt->Srb.SenseInfoBuffer,
                  senseSize
                  );
    return;
}


