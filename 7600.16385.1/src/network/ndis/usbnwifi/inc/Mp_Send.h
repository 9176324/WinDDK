/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Mp_Send.h

Abstract:
    Mp layer send defines (accessible to all layers)
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
#ifndef _MP_SEND_H_
#define _MP_SEND_H_

/**
 * \defgroup TX_MSDU_Macros Various macros to access information within MP_TX_MSDU.
 */
/*@{*/   // TX_MSDU_Macros

/** The number of Physical Buffers in the packet described by MP_TX_MSDU */
#define     MP_TX_MSDU_PHYSICAL_BUFFER_COUNT(MpTxd)     (MpTxd)->PhysicalBufCount

/** The number of NDIS_BUFFERs in the packet described by MP_TX_MSDU */
#define     MP_TX_MSDU_BUFFER_COUNT(MpTxd)                (MpTxd)->BufferCount

/** Pointer to the first buffer in the packet described by MP_TX_MSDU */
#define     MP_TX_MSDU_FIRST_BUFFER(MpTxd)                (MpTxd)->FirstBuffer

/** Total length in bytes of the packet described by MP_TX_MSDU */
#define     MP_TX_MSDU_LENGTH(MpTxd)                       (MpTxd)->PacketLength

/** The 802.11 specific Send Information for this packet placed by Native WiFi IM driver */
#define     MP_TX_MSDU_SEND_SEND_CONTEXT(MpTxd)            (MpTxd)->Dot11SendContext

/** Length in bytes of the Dot11SendContext data structure returned by MP_TX_MSDU_SEND_SEND_CONTEXT macro*/
#define     MP_TX_MSDU_SEND_SEND_CONTEXT_SIZE(MpTxd)       (MpTxd)->SendContextSize

/*@}*/   // TX_MSDU_Macros


//
// Various flags used by the miniport
//

/** The Tx MSDU is free and can used to transmit a packet */
#define     MP_TX_MSDU_IS_FREE                  0x00000000
/** This Tx MSDU is reserved for a packet and should not be used for any other packet */
#define     MP_TX_MSDU_IS_RESERVED              0x00000001
/** The Tx MSDU is ready for sending. All required info like Scatter Gather etc. is done */
#define     MP_TX_MSDU_IS_READY_FOR_SEND       0x00000002
/** This Tx MSDU has been passed to the hardware for transmission */
#define     MP_TX_MSDU_IS_IN_SEND               0x00000004
/** This Tx MSDU has a thread owner that will transmit it when the turn comes */
#define     MP_TX_MSDU_HAS_SENDER               0x00000008

/** This Tx MSDU was buffered by an AP for a sleeping station */
#define     MP_TX_MSDU_AP_BUFFERED_PACKET            0x00000001
/** This Tx MSDU was buffered by a STA for another sleeping STA */
#define     MP_TX_MSDU_STA_BUFFERED_PACKET           0x00000002
/** Scatter Gather has been successfully requested for all the NBs in the NBL */
#define     MP_TX_MSDU_SCATTER_GATHER_REQUESTED     0x00000004
/** Scatter Gather has run out of resources for one of the NBs in the NBL */
#define     MP_TX_MSDU_SCATTER_GATHER_LOW_RESOURCES        0x00000008
/** Scatter Gather has failed due to sends being stopped (reset, pause, surprise remove) */
#define     MP_TX_MSDU_SCATTER_GATHER_SEND_ABANDONED       0x00000010


/**
 * This structure describes a Transmit MSDU (TX_MSDU) that is used
 * to send a packet out on the wire. MP_TX_MSDU refers to the miniport specific
 * structure that setups the packet for the hardware to DMA and send.
 * 
 * \sa MP_COALESCE_BUFFER
 */
typedef struct _MP_TX_MSDU
{
    /** Pointer to the next TX_MSDU in the circular list */
    struct _MP_TX_MSDU*             Next;

    /** Special send/ownership flags. Careful when messing with these */
    UCHAR                       SendFlags;
    
    /** Some status information like TX_MSDU is in use etc. */
    USHORT                      Flags;

    /** Reference to the ADAPTER object for this miniport */
    PADAPTER                    Adapter;

    /** The associated TX_MSDU that contains hardware specific information */
    PNIC_TX_MSDU                NicTxd;

    /*
     * Tis number is also the total number
     * of NetBuffers (MPDU) in this NetBufferList (MSDU)
     */
    USHORT                      NetBufferCount;


#ifdef  USE_SAMPLE_PACKET_BUFFERING_IMPLEMENTATION

    /** The PSPacketQueue this packet came from. Needed for 802.11 Power Mgmt */
    PMP_PS_PACKET_QUEUE         PSPacketQueue;

#endif  // USE_SAMPLE_PACKET_BUFFERING_IMPLEMENTATION

    /** The NBL associated with this TX_MSDU */
    PNET_BUFFER_LIST            NetBufferList;

    /** The Send Context Information for this TX_MSDU */
    PDOT11_EXTSTA_SEND_CONTEXT  Dot11SendContext;    
} MP_TX_MSDU, *PMP_TX_MSDU;

#endif // _MP_SEND_H_

