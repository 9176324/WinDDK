/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    hw_send.h

Abstract:
    Contains defines used for send functionality 
    in the HW layer
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    09-04-2007    Created

Notes:

--*/
#pragma once

/*
 * The maximum number of Dot11 fragments we expect to practically see in a NBL 
 * Worst case, this value can go up to 9 and theoretically go up to 16
 */
#define HW_MAX_NUM_OF_FRAGMENTS                 MIN(MP_TX_FRAGMENTS_MAX_COUNT, 4)

#define HW_TX_COALESCE_THRESHOLD                8

/** Number of buffers for beacons */
#define HW_BEACON_QUEUE_BUFFER_COUNT            1
/** Number of buffers for Probe response, etc */
#define HW_INTERNAL_SEND_QUEUE_BUFFER_COUNT     16
#define HW_UNUSED_SEND_QUEUE_BUFFER_COUNT       2


#define sAckCtsLng		112		// bits in ACK and CTS frames
#define sCrcLng			4		// octets for crc32 (FCS, ICV)
#define aSifsTime		10
#define sMacHdrLng		24		// octets in data header, no WEP


/** We can have per queue-lock, but we currently dont use them */
#define HW_TX_ACQUIRE_QUEUE_LOCK(_Hw, _HwQueue, _Dispatch)  \
    HW_ACQUIRE_HARDWARE_LOCK(_Hw, _Dispatch)
#define HW_TX_RELEASE_QUEUE_LOCK(_Hw, _HwQueue, _Dispatch)  \
    HW_RELEASE_HARDWARE_LOCK(_Hw, _Dispatch)


typedef enum _HW_TX_CIPHER_SETTING{
    HW_TX_CAN_ENCRYPT,
    HW_TX_ENCRYPT_IF_KEY_MAPPING_KEY_AVAILABLE,
    HW_TX_NEVER_ENCRYPT
} HW_TX_CIPHER_SETTING, *PHW_TX_CIPHER_SETTING;

#define HW_INCREMENT_AVAILABLE_TX_MSDU(_HwQueue)        \
    NdisInterlockedIncrement((PLONG)&((_HwQueue)->NumMSDUAvailable))
#define HW_DECREMENT_AVAILABLE_TX_MSDU(_HwQueue)        \
    NdisInterlockedDecrement((PLONG)&((_HwQueue)->NumMSDUAvailable))

#define HW_INCREMENT_PENDING_TX_MSDU_SG_OP(_Txd)    \
    ((UCHAR)NdisInterlockedIncrement((PLONG)&((_Txd)->OutstandingSGAllocationCount)))
#define HW_DECREMENT_PENDING_TX_MSDU_SG_OP(_Txd)    \
    ((UCHAR)NdisInterlockedDecrement((PLONG)&((_Txd)->OutstandingSGAllocationCount)))


#define HW_INCREMENT_HAL_PENDING_TX(_HwQueue)       \
    NdisInterlockedIncrement((PLONG)&((_HwQueue)->NumPending))

#define HW_DECREMENT_HAL_PENDING_TX(_HwQueue)       \
    NdisInterlockedDecrement((PLONG)&((_HwQueue)->NumPending))

//
// Various flags used for the synchronization of the TX_MSDU list
//
// DO NOT ADD ANY OTHER BITS TO THESE FLAGS (ie. do not add any other flags)
/** The Tx MSDU is free and can used to transmit a packet */
#define     HW_TX_MSDU_IS_FREE                  0x00000000
/** This Tx MSDU is reserved for a packet and should not be used for any other packet */
#define     HW_TX_MSDU_IS_RESERVED              0x00000001
/** The Tx MSDU is setup for send */
#define     HW_TX_MSDU_IS_READY_FOR_SEND        0x00000002
/** This Tx MSDU has a thread owner that will transmit it when the turn comes */
#define     HW_TX_MSDU_HAS_SENDER               0x00000004
/** This Tx Desc has been passed to the hardware for transmission */
#define     HW_TX_MSDU_IS_IN_SEND               0x00000008
// DO NOT ADD ANY OTHER BITS TO THESE FLAGS (ie. do not add any other flags)

/**
 * This Macro tries to obtain ownership of sending a tx MSDU for the current thread.
 * If the ownership is successfullly obtained, the macro returns true. Else a
 * false value is returned. This macro is internal to the send path only.
 * 
 * FOR THIS TO WORK, DO NOT ADD ANY OTHER BITS TO THE FLAGS
 */
#define HW_ACQUIRE_TX_MSDU_SEND_OWNERSHIP(_TxDesc)                          \
    (InterlockedCompareExchange(											\
                (PLONG)&(_TxDesc->Flags),									        \
                (HW_TX_MSDU_IS_RESERVED | HW_TX_MSDU_IS_READY_FOR_SEND | HW_TX_MSDU_HAS_SENDER),    \
                (HW_TX_MSDU_IS_RESERVED | HW_TX_MSDU_IS_READY_FOR_SEND)	    \
                ) == (HW_TX_MSDU_IS_RESERVED | HW_TX_MSDU_IS_READY_FOR_SEND))

#define HW_RELEASE_TX_MSDU_SEND_OWNERSHIP(_TxDesc)                          \
    InterlockedExchange((PLONG)&(_TxDesc->Flags), (HW_TX_MSDU_IS_RESERVED | HW_TX_MSDU_IS_READY_FOR_SEND))

#define HW_TX_MSDU_SET_FLAG(_Txd, _Flag)    MP_INTERLOCKED_SET_FLAG(&(_Txd->Flags), _Flag)
#define HW_TX_MSDU_TEST_FLAG(_Txd, _Flag)   MP_TEST_FLAG((_Txd->Flags), _Flag)
#define HW_TX_MSDU_CLEAR_FLAG(_Txd, _Flag)  MP_INTERLOCKED_CLEAR_FLAG(&(_Txd->Flags), _Flag)


/**
 * Initializes a newly reserved HW_TX_MPDU
 */
#define HW_RESERVE_TX_MSDU(_Msdu, _Packet)                  \
{                                                           \
    MPASSERT(_Msdu->Flags == 0);                            \
    HW_TX_MSDU_SET_FLAG(_Msdu, HW_TX_MSDU_IS_RESERVED);     \
    _Msdu->MpMsdu = _Packet;                                \
    _Msdu->SGElementListCount = 0;                          \
    _Msdu->WaitForSendToComplete = TRUE;                    \
    _Msdu->FailedDuringSend = FALSE;                        \
    _Msdu->TxSucceeded = FALSE;                             \
    _Msdu->ScatterGatherRequested = FALSE;                  \
}


/**
 * Release a previously reserved HW_TX_MPDU
 */
#define HW_RELEASE_TX_MSDU(_Msdu)                           \
{                                                           \
    _Msdu->MpduCount = 0;                                   \
    _Msdu->Flags = 0;                                       \
    _Msdu->MpMsdu = NULL;                                   \
    _Msdu->PeerNode = NULL;                                 \
    _Msdu->PhyId = DOT11_PHY_ID_ANY;                        \
    _Msdu->SGElementListCount = 0;                          \
    _Msdu->Key = NULL;                                      \
}

#define HW_TIMESTAMP_TX_MSDU(_Msdu, _TxQueue)               \
{                                                           \
    NdisGetCurrentSystemTime(&_Msdu->SnapshotTime);         \
}

#define HW_CHECK_TX_MSDU_TIME(_Msdu, _BreakTime)            \
{                                                           \
    LARGE_INTEGER               _currentTime;               \
    NdisGetCurrentSystemTime(&_currentTime);   \
    DbgPrint("# %4d %s %d\n", _Msdu->TotalMSDULength, (_Msdu->TxSucceeded ? "OK " : "ERR"), \
        (_currentTime.QuadPart - _Msdu->SnapshotTime.QuadPart));                \
}

/**
 * This function is called once the Hw has made any and all changes that 
 * were needed to the 802.11 frame to be transmitted. After the scatter gather 
 * operation has been performed, the NIC must NOT make any changes to the 
 * packet as they may not get reflected in the SG Elements.
 *
 */
VOID
HwPerformScatterGather(
    __in PHW                      Hw,
    __in PHW_TX_MSDU              Msdu
    );



/**
 * This function is called when Scatter Gather operation for a TX_MSDU has
 * been completed, either successfully or with failures. This function
 * determines whether SG operations were successful or not and then calls
 * the hardware with appropriate status.
 * 
 * \param pTxd              The Tx MSDU which was being scatter gathered
 * \param DispatchLevel     TRUE if IRQL is at dispatch level
 */
VOID
HwSGComplete(
    __in  PHW_TX_MSDU         Msdu,
    __in  BOOLEAN             DispatchLevel
    );

__inline BOOLEAN
HwCanTransmit(
    __in  PHW_TX_QUEUE            TxQueue
    );

NDIS_STATUS
HwPrepareTxMSDUForSend(
    __in  PHW                     Hw,
    __in  PHW_TX_QUEUE            TxQueue,
    __in  PHW_TX_MSDU             Msdu
    );


// Lock held
__inline NDIS_STATUS
HwReserveTxResources(
    __in  PHW                     Hw,
    __in  PHW_TX_QUEUE            TxQueue,
    __in  PMP_TX_MSDU             Packet
    );


VOID
HwProcessReservedTxPackets(
    __in  PHW                     Hw,
    __in  PHW_TX_QUEUE            TxQueue,
    __in  LONG                    StartIndex,
    __in  ULONG                   NumTxReady,
    __in  ULONG                   SendFlags
    );


__inline USHORT
HwComputeTxTime(
    __in  PHW_TX_MSDU             Msdu,
    __in  DOT11_FRAME_TYPE        FrameType,
    __in  ULONG                   FragmentLength,
    __in  BOOLEAN                 LastFragment
    );



__inline HW_TX_CIPHER_SETTING
HwDetermineCipherSettings(
    __in  PHW_MAC_CONTEXT         MacContext,
    __in  PHW_PEER_NODE           PeerNode,
    __in  PHW_TX_MSDU             Msdu,
    __in  PDOT11_MAC_HEADER       FragmentHeader
    );


PHW_KEY_ENTRY
HwFindEncryptionKey(
    __in  PHW_MAC_CONTEXT         MacContext,
    __in  PHW_PEER_NODE           PeerNode,
    __in  PHW_TX_MSDU             Msdu,
    __in  PDOT11_MAC_HEADER       FragmentHeader,
    __in  HW_TX_CIPHER_SETTING    CipherSettings
    );

NDIS_STATUS
HwSetupTxCipher(
    __in  PHW                     Hw,
    __in  PHW_MAC_CONTEXT         MacContext,
    __in  PHW_TX_MSDU             Msdu,
    __in  PHW_TX_MPDU             Mpdu,
    __in  PHW_KEY_ENTRY           Key
    );

VOID
HwSubmitReadyMSDUs(
    __in  PHW                     Hw,
    __in  PHW_TX_QUEUE            TxQueue,
    __in  BOOLEAN                 DispatchLevel
    );


ULONG
HwGetTxFreeDescNum(
    __in  PHW                     Hw,
    __in  PHW_TX_QUEUE            TxQueue
    );


//
// Return values:
//  NDIS_STATUS_SUCCESS: The packet was handed to hardware
//  NDIS_STATUS_RESOURCES: The packet could not be handed
//          to hardware due to lack of resources
//  NDIS_STATUS_FAILURE: Send failed due to reasons other
//          than low Hw resources
//
NDIS_STATUS
HwSubmitMSDU(
    __in  PHW                     Hw,
    __in  PHW_TX_QUEUE            TxQueue,
    __in  PHW_TX_MSDU             Msdu
    );




ULONG
HwCountTxResourceNeeded(
    __in  PHW_TX_MSDU         Msdu,
    __out BOOLEAN             *UseCoalesce
    );



NDIS_STATUS 
HwTransmitMSDU(
    __in  PHW                     Hw,
    __in  PHW_TX_QUEUE            TxQueue,
    __in  PHW_TX_MSDU             Msdu,
    __in  USHORT                  NumTxDescNeeded
    );



NDIS_STATUS
HwCopyNBLToBuffer(
    __in  PNET_BUFFER_LIST    NetBufferList,
    __out_bcount(MAX_TX_RX_PACKET_SIZE) PUCHAR              pDest,
    __out PULONG              BytesCopied
    );


NDIS_STATUS
HwTransmitMSDUCoalesce(
    __in  PHW                     Hw,
    __in  PHW_TX_QUEUE            TxQueue,
    __in  PHW_TX_MSDU             Msdu,
    __in  USHORT                  NumTxDescNeeded
    );

NDIS_STATUS
HwTransmitBeacon(
    __in  PHW                     Hw,
    __in  PHW_TX_MSDU             Msdu
    );

BOOLEAN
HwTxMSDUIsComplete(
    __in  PHW                     Hw,
    __in  PHW_TX_MSDU             Msdu
    );

VOID
HwFreeTxResources(
    __in  PHW                     Hw,
    __in  PHW_TX_QUEUE            TxQueue,
    __in  PHW_TX_MSDU             Msdu
    );

VOID
HwCompleteTxMSDU(
    __in  PHW                     Hw,
    __in  PHW_TX_QUEUE            TxQueue,
    __in  PHW_TX_MSDU             Msdu,
    __in  BOOLEAN                 Success
    );


VOID
HwCheckSendQueueForCompletion(
    __in  PHW                     Hw,
    __in  PHW_TX_QUEUE            TxQueue
    );


VOID
HwReinitializeSendQueue(
    __in  PHW                     Hw,
    __in  PHW_TX_QUEUE            TxQueue,
    __in  BOOLEAN                 DispatchLevel
    );

VOID 
HwSendCompletePackets(
    __in  PHW                     Hw,
    __in  PMP_TX_MSDU             PacketList,
    __in  ULONG                   NumberOfPackets,
    __in  ULONG                   SendCompleteFlags
    );

#ifndef HIDE_ANNOTATION
__drv_mustHold(KeSpinLockType)
__drv_requiresIRQL(DISPATCH_LEVEL)
#endif
VOID
HwFlushQueuedTxMSDUs(
    __in  PHW                     Hw,
    __in  PHW_TX_QUEUE            TxQueue,
    __in  BOOLEAN                 DispatchLevel
    );

#ifndef HIDE_ANNOTATION
__drv_mustHold(KeSpinLockType)
__drv_requiresIRQL(DISPATCH_LEVEL)
#endif
VOID
HwFlushReservedTxMSDUs(
    __in  PHW                     Hw,
    __in  PHW_TX_QUEUE            TxQueue,
    __in  BOOLEAN                 DispatchLevel
    );


VOID
HwFlushSendQueue(
    __in  PHW                     Hw,
    __in  PHW_TX_QUEUE            TxQueue,
    __in  BOOLEAN                 DispatchLevel
    );

VOID
HwFlushSendEngine(
    __in  PHW                     Hw,
    __in  BOOLEAN                 DispatchLevel
    );

VOID
HwResetSendEngine(
    __in  PHW                     Hw,
    __in  BOOLEAN                 DispatchLevel
    );

BOOLEAN
HwCheckForSendHang(
    __in  PHW                     Hw
    );


PMP_TX_MSDU
HwAllocatePrivatePacket(
    __in  PHW                     Hw,
    __in  USHORT                  PacketSize
    );

VOID
HwFreePrivatePacket(
    __in  PHW                     Hw,
    __in  PMP_TX_MSDU             Packet
    );    

VOID
HwSendPrivatePackets(
    __in  PHW_MAC_CONTEXT         MacContext,
    __in  PMP_TX_MSDU             PacketList,
    __in  ULONG                   SendFlags
    );

VOID 
HwSendCompletePrivatePackets(
    __in  PHW_MAC_CONTEXT         MacContext,
    __in  PMP_TX_MSDU             PacketList,
    __in  ULONG                   NumberOfPackets,
    __in  ULONG                   SendCompleteFlags
    );

VOID
HwHandleSendCompleteInterrupt(
    __in  PHW                     Hw
    );



