/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Mp_Recv.h

Abstract:
    Mp layer receive structures (accessible to all layers)
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
#ifndef _MP_RECEIVE_H_
#define _MP_RECEIVE_H_


/**
 * \defgroup RX_MSDU_Macros Various macros to access information within an RX_MSDU.
 */
/*@{*/   // RX_MSDU_Macros

/** Maximum size that an RX_MSDU structure can be */
#define MP_RX_MSDU_MAX_SIZE     sizeof(MP_RX_MSDU)

/** Provides access to the Receive Context Info for this Rx MSDU */
#define MP_RX_MSDU_RECV_CONTEXT(_Rxd)        (&_Rxd->Dot11RecvContext)


/*@}*/   // RX_MSDU_Macros

/**
 * \defgroup RxdFlags These are the various values that the Flags field in MP_RX_MSDU can take.
 */
/*@{*/   // RxdFlags

/** The Rx MSDU is free and unused. It is available for indication of packets */
#define     MP_RX_MSDU_IS_FREE                  0x00000000
/** This Packet associated with this Rx MSDU has been marked with NDIS_STATUS_RESOURCES */
#define     MP_RX_MSDU_LOW_RESOURCES           0x00000001
/** The NBL associated with this Rx MSDU has been indicated to NDIS */
#define     MP_RX_MSDU_NBL_INDICATED        0x00000002

/*@}*/   // RxdFlags


typedef struct _MP_RX_MSDU
{
    union _Link
    {
        /** Link to place the Rx MSDU in an SList */
        SLIST_ENTRY                 SListLink;

        /** Link to place the Rx MSDU in an LIST_ENTRY */
        LIST_ENTRY                  ListEntryLink;
    } Link;

    /** The NDIS_PACKET that will be indicated up for an 802.11 frame received */
    //PNDIS_PACKET                NdisPacket;

    /** 802.1 MSDU for this Rx MSDU will be indicated using this NBL */
    PNET_BUFFER_LIST                NetBufferList;

    /** The array of NetBuffers each of which represents an MPDU */
    //PNET_BUFFER                     NetBufferArray[MP_802_11_MAX_FRAGMENTS_PER_MSDU];
    
    /*
     * Flags describing different state values of this RX_MSDU 
     * \warning Any fields that should be zeroed out when a packet is returned
     * should be placed after this field. The two fields above remain unchanged.
     * All the rest of the fields are zeroed out in MpReInitializeRxMSDU.
     */
    ULONG                       Flags;

    /** The timestamp at which reassembly started */
    ULONGLONG                   TimeStamp;

    /** Index in the Reassembly line where this Rx MSDU is present */
    USHORT                      ReassemblyLinePosition;

    /** The sequence number for this 802.11 frame */
    USHORT                      SequenceNumber;

    /** The total number of fragments that should be in this packet */
    USHORT                      TotalFragmentCount;

    /** The number of fragments currently received for this Rx MSDU */
    USHORT                      FragmentCount;

    /** The NIC_RX_FRAGMENTs to hold each fragment for this Rx MSDU in */
    PNIC_RX_FRAGMENT            Fragments[MP_802_11_MAX_FRAGMENTS_PER_MSDU];

    /** Sender MAC address that we use for fragmentat reassembly */
    UCHAR                       PeerMacAddress[6];

    /** The Recv Context Info Data Structure for this MSDU */
    DOT11_EXTSTA_RECV_CONTEXT   Dot11RecvContext;
} MP_RX_MSDU, *PMP_RX_MSDU;

#endif // _MP_RECEIVE_H_

