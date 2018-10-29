/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Mp_Events.c

Abstract:
    Hooking functions for modifying MP Layer functionality
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/

///////////////////////////////////////////////////////////////////
//
// This file contains all interfaces that vendor needs
// to add to the miniport. This file among all others is
// least likely to change from one revision of the sample
// to the next. By localising all MP portion changes to
// this file, vendor can add code, value adds and handle
// hardware specific issues with little or no changes to
// other Mp portion files. Syncing with the next revision
// of the driver (to get bug fixes and/or new features will,
// hopefully, be a much easier process.
//
//////////////////////////////////////////////////////////////////


#include "Precomp.h"

#ifdef USE_SAMPLE_PACKET_BUFFERING_IMPLEMENTATION
#include "Mp_Main.h"
#include "Nwf_Mp_PSPacketsManager.h"
#include "Mp_Send.h"

/** Determines if the packet is a PS poll packet type or not */
#define MP_IS_PS_POLL_PACKET(_FragmentHdr)  (((PUCHAR)&_FragmentHdr->FrameControl)[0]  == 0xA4)

#endif  // USE_SAMPLE_PACKET_BUFFERING_IMPLEMENTATION

#include "mp_evnts.h"
#if DOT11_TRACE_ENABLED
#include "Mp_Events.tmh"
#endif



/*
 * Allocate any resources required by CustomInterfaces.
 * All initializations go here as well.
 * 
 * \param pAdapter  The adapter context for this miniport
 * \return NDIS_STATUS_SUCCESS if all goes well, else the
 * appropriate failure
 * \sa MpEventTerminate
 */ 
NDIS_STATUS
MpEventInitialize(
    __in  PADAPTER    pAdapter
    )
{
    NDIS_STATUS     ndisStatus = NDIS_STATUS_SUCCESS;

    UNREFERENCED_PARAMETER(pAdapter);
    
#ifdef  USE_SAMPLE_PACKET_BUFFERING_IMPLEMENTATION
    //
    // Allocate the PS Packets Manager for buffering packets for
    // sleeping stations.
    //
    ndisStatus = MpPSPacketsManagerInitialize(&pAdapter->PSPacketManager, pAdapter);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        MpTrace(COMP_EVENTS, DBG_SERIOUS,  ("Failed to initialize PS Packets Manager. Status = 0x%x\n", ndisStatus));
        goto error;
    }
#endif  // #ifdef  USE_SAMPLE_PACKET_BUFFERING_IMPLEMENTATION

    goto error;

error:
    return ndisStatus;
}


/*
 * Free all resources allocated by CustomInterfaces
 * 
 * \param pAdapter  The adapter context for this miniport
 * \sa MpEventInitialize
 */
VOID
MpEventTerminate(
    __in  PADAPTER    pAdapter
    )
{
    UNREFERENCED_PARAMETER(pAdapter);
    
#ifdef  USE_SAMPLE_PACKET_BUFFERING_IMPLEMENTATION

    //
    // Free the PS Packets Managers Queue.
    //
    MpPSPacketsManagerTerminate(&pAdapter->PSPacketManager);

#endif  // USE_SAMPLE_PACKET_BUFFERING_IMPLEMENTATION
}


/**
 * Handle and private OIDs or intercept other RequestQuery in this function.
 * This way, OID handling changes are localised to this file.
 * 
 * \return The appropriate NDIS_STATUS error code.
 * \sa Please refer to MiniportRequest function in the DDK for
 * full description of this function and its handling.
 */
NDIS_STATUS
MpEventQueryInformation(
    __in    PADAPTER                  pAdapter,
    __in    NDIS_OID                  Oid,
    __in    PVOID                     InformationBuffer,
    __in    ULONG                     InformationBufferLength,
    __out   PULONG                    BytesWritten,
    __out   PULONG                    BytesNeeded
    )
{
    NDIS_STATUS                 ndisStatus = NDIS_STATUS_SUCCESS;
    ULONG                       uInfo = 0, uInfoLen = 0;
    PVOID                       pInfo = (PVOID)&uInfo;
    
    UNREFERENCED_PARAMETER(pAdapter);
    UNREFERENCED_PARAMETER(InformationBuffer);
    UNREFERENCED_PARAMETER(InformationBufferLength);
    UNREFERENCED_PARAMETER(BytesWritten);
    UNREFERENCED_PARAMETER(BytesNeeded);

    switch(Oid)
    {
#ifdef  USE_SAMPLE_PACKET_BUFFERING_IMPLEMENTATION
    case OID_GEN_TRANSMIT_QUEUE_LENGTH:
        uInfo = pAdapter->PendingSends +                 // packet submitted to hardware
        MpNBLQueueDepth(&pAdapter->TxQueue) +       // packets in TxQueue
        MpGetBufferedPSPacketCount(pAdapter);
        
        uInfoLen = sizeof(uInfo);
        break;
#endif  // USE_SAMPLE_PACKET_BUFFERING_IMPLEMENTATION

        default:
            ndisStatus = NDIS_STATUS_INVALID_OID;
            break;
    }

    if (ndisStatus == NDIS_STATUS_SUCCESS)
    {
        if (uInfoLen <= InformationBufferLength)
        {
            if(uInfoLen)
            {
                // Copy result into InformationBuffer
                *BytesWritten = uInfoLen;
                NdisMoveMemory(InformationBuffer, pInfo, uInfoLen);
            }
            else 
            {
                //
                // the result is either zero length of has been copied
                // by the OID specific handler
                //
                // Dont overwrite the BytesWritten and BytesNeeded
                // as they are also appropriately set
                //
            }
        }
        else
        {
            //
            // The buffer length is not sufficient
            //
            *BytesNeeded = uInfoLen;
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
        }
    }

    return ndisStatus;
}


/**
 * Handle and private OIDs or intercept other RequestSet in this function.
 * This way, OID handling changes are localised to this file.
 * 
 * \return The appropriate NDIS_STATUS error code.
 * \sa Please refer to MiniportRequest function in the DDK for
 * full description of this function and its handling.
 */
NDIS_STATUS
MpEventSetInformation(
    __in    PADAPTER                  pAdapter,
    __in    NDIS_OID                  Oid,
    __in    PVOID                     InformationBuffer,
    __in    ULONG                     InformationBufferLength,
    __out   PULONG                    BytesRead,
    __out   PULONG                    BytesNeeded
    )
{
    NDIS_STATUS         ndisStatus = NDIS_STATUS_SUCCESS;

    UNREFERENCED_PARAMETER(pAdapter);
    UNREFERENCED_PARAMETER(InformationBuffer);
    UNREFERENCED_PARAMETER(InformationBufferLength);
    UNREFERENCED_PARAMETER(BytesRead);
    UNREFERENCED_PARAMETER(BytesNeeded);

    switch(Oid)
    {
        default:
            ndisStatus = NDIS_STATUS_INVALID_OID;
            break;
    }
    
    return ndisStatus;
}


/**
 * Handle and private OIDs or intercept other RequestMethods in this function.
 * This way, OID handling changes are localised to this file.
 * 
 * \return The appropriate NDIS_STATUS error code.
 * \sa Please refer to MiniportRequest function in the DDK for
 * full description of this function and its handling.
 */
NDIS_STATUS
MpEventQuerySetInformation(
    __in    PADAPTER                  pAdapter,
    __in    NDIS_OID                  Oid,
    __in    PVOID                     InformationBuffer,
    __in    ULONG                     InputBufferLength,
    __in    ULONG                     OutputBufferLength,
    __in    ULONG                     MethodId,
    __out   PULONG                    BytesWritten,
    __out   PULONG                    BytesRead,
    __out   PULONG                    BytesNeeded
    )
{
    NDIS_STATUS         ndisStatus = NDIS_STATUS_SUCCESS;

    UNREFERENCED_PARAMETER(pAdapter);
    UNREFERENCED_PARAMETER(InformationBuffer);
    UNREFERENCED_PARAMETER(InputBufferLength);
    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(MethodId);
    UNREFERENCED_PARAMETER(BytesWritten);
    UNREFERENCED_PARAMETER(BytesRead);
    UNREFERENCED_PARAMETER(BytesNeeded);

    switch(Oid)
    {
        default:
            ndisStatus = NDIS_STATUS_INVALID_OID;
            break;
    }

    return ndisStatus;    
}


/**
 * This function is called every time MiniportCheckForHang is
 * called. Return TRUE if the miniport needs to be reset.
 * 
 * \param pAdapter  The adapter context for this miniport
 * \return TRUE if miniport should be reset. Else false
 * \sa MpEventMiniportReset
 */
BOOLEAN
MpEventCheckForHang(
    __in  PADAPTER    pAdapter
    )
{
    UNREFERENCED_PARAMETER(pAdapter);
    
#ifdef  USE_SAMPLE_PACKET_BUFFERING_IMPLEMENTATION

    //
    // Drop expired buffered frames if in AP or adhoc mode
    // Doing this periodically helps clean up expired packets.
    //
    if(pAdapter->StartRequestCalled)
    {
        MpPSPacketsManagerDropExpiredPackets(&pAdapter->PSPacketManager, TRUE);
    }
    
#endif  // USE_SAMPLE_PACKET_BUFFERING_IMPLEMENTATION

    return FALSE;
}


/**
 * When a Dot11 Reset is issued (OID_DOT11_RESET_REQUEST), before the reset
 * of miniport and hardware begins, this function is called. If there is
 * any state that requires to be reset, should be done here. Please note that
 * MpEventMiniportReset will -also- be called so only Dot11 Reset specific work
 * be placed here. All reinitializations that always take place during a reset
 * can be placed in MpEventMiniportReset.
 * 
 * \param pAdapter      The miniport Adapter context
 * \param ResetType     The type of reset this miniport is undergoing.
 * \sa MpEventMiniportReset, OID_DOT11_RESET_REQUEST
 */
VOID
MpEventDot11Reset(
    __in  PADAPTER            pAdapter,
    __in  DOT11_RESET_TYPE    ResetType
    )
{
    UNREFERENCED_PARAMETER(pAdapter);
    UNREFERENCED_PARAMETER(ResetType);
}


/**
 * This function is called when miniport is being reset.
 * Interrupts are disabled on the device and send/receive
 * units have been stopped.
 * 
 * \param pAdapter  The adapter context for this miniport 
 * \sa MpEventCheckForHang, MpEventDot11Reset
 */
VOID
MpEventMiniportReset(
    __in  PADAPTER    pAdapter
    )
{
    UNREFERENCED_PARAMETER(pAdapter);
    
#ifdef USE_SAMPLE_PACKET_BUFFERING_IMPLEMENTATION

    //
    // Free the packets present in the PSPacketManager
    //
    MpPSPacketsManagerFlushAllQueues(&pAdapter->PSPacketManager);

    //
    // Clear virtual bitmap in beacon
    //
    NdisZeroMemory(pAdapter->ElementTIM.VirtualBitmap, sizeof(pAdapter->ElementTIM.VirtualBitmap));
    pAdapter->LowestElementTIMAssocationID = 0;
    pAdapter->HighestElementTIMAssocationID = 0;

#endif  // USE_SAMPLE_PACKET_BUFFERING_IMPLEMENTATION

}


/**
 * This function is called when a packet to be sent is submitted to
 * the miniport driver. This function can perform a number of operations
 * on the packet being sent. Please be careful about major operations
 * like subsuming packets etc. as they may affect the operation of the
 * rest of the miniport driver.
 * 
 * \param pAdapter                  The Adapter context for this miniport
 * \param NetBufferList             The BetBufferList submitted to the driver
 * \param pDot11SendContext         The send context for this packet
 * \param SendContextSize           Size of send context Info
 * \return NDIS_STATUS_SUCCESS if this packet has been subsumed by this function.
 * Responsibility for sending and send completing this packet is on this function.
 * NDIS_STATUS_NOT_RECOGNIZED be returned if this packet is to process normally.
 * Any other status will be treated as failure and packet failed back to the OS.
 * \sa MPSendNetBufferLists, MpEventPacketReceived
 */
NDIS_STATUS
MpEventSendNetBufferList(
    __in  PADAPTER                        pAdapter,
    __in  PNET_BUFFER_LIST                NetBufferList,
    __in  ULONG                           SendFlags,
    __in  PDOT11_EXTSTA_SEND_CONTEXT      pDot11SendContext,
    __in  BOOLEAN                         DispatchLevel
    )
{
    NDIS_STATUS     ndisStatus;
    
    UNREFERENCED_PARAMETER(pAdapter);
    UNREFERENCED_PARAMETER(SendFlags);
    UNREFERENCED_PARAMETER(NetBufferList);
    UNREFERENCED_PARAMETER(pDot11SendContext);
    UNREFERENCED_PARAMETER(DispatchLevel);

    // Currently for extensible model, we dont do anything
    ndisStatus = NDIS_STATUS_NOT_RECOGNIZED;

    return ndisStatus;
}


BOOL
MpEventShouldBufferPacket(
    __in  PADAPTER                        pAdapter,
    __in  PDOT11_EXTSTA_SEND_CONTEXT      pDot11SendContext
    )
{
    BOOL    bufferPacket = FALSE;

    UNREFERENCED_PARAMETER(pAdapter);
    UNREFERENCED_PARAMETER(pDot11SendContext);
    
    // add this packet to PS broadcast queue if
    // 1. DOT11_FLAGS_PS_ON is set (by soft ap) in send context.
    // 2. DOT11_FLAGS_PS_ON is clear but PS broadcast queue is not empty.
    //    This is to prevent packets recieved out of order on receive side. A STA
    //    in PS mode can choose to send out a NULL packet with power save off when
    //    it wakes up to receive broadcast data from AP. This causes problem because 
    //    AP will not set DOT11_FLAGS_PS_ON for subsequent outgoing packets and 
    //    these packets will be sent immediately instead of queued up, and STA 
    //    receives packets out of order as a result.

    return bufferPacket;
}
    
/*
 * This function is called to notify when a send complete has occured and
 * is about to relayed to NDIS. Please note that this function is only called
 * for Tx MSDU that were actually handed to the hardware. Packets failed because
 * of issues that prevented us from ever handing the packet to the hardware
 * may not be provided to this API.
 *
 * \param   pAdapter        Adapter context
 * \param   pTxd            The Tx MSDU completing
 * \param   ndisStatus      Completion status for this packet
 */
VOID
MpEventSendComplete(
    __in  PADAPTER        pAdapter,
    __in  PMP_TX_MSDU     pTxd,
    __in  NDIS_STATUS     ndisStatus,
    __in  BOOLEAN         DispatchLevel
    )
{
    UNREFERENCED_PARAMETER(pAdapter);
    UNREFERENCED_PARAMETER(pTxd);
    UNREFERENCED_PARAMETER(ndisStatus);
    UNREFERENCED_PARAMETER(DispatchLevel);
    
#ifdef USE_SAMPLE_PACKET_BUFFERING_IMPLEMENTATION
    if (MP_TEST_FLAG(pTxd, (MP_TX_MSDU_AP_BUFFERED_PACKET|MP_TX_MSDU_STA_BUFFERED_PACKET)))
    {
        //
        // This packet was sent by the PS Packet manager.
        // Let the PS Packet Manager Send Complete it.
        //
        MpPSPacketsManagerSendComplete(
            pAdapter,
            pTxd,
            ndisStatus,
            DispatchLevel
            );
    }
#endif  // USE_SAMPLE_PACKET_BUFFERING_IMPLEMENTATION
}


/**
 * This function is invoked when the miniport receives a 
 * packet from the underlying hardware. This packet has been
 * checked for correct CRC, but not filtered compeletly. 
 * The default takes no action.
 * 
 * \param pAdapter      The adapter context for this miniport
 * \param pMpFragment   The MP_RX_FRAGMENT for this MPDU
 * \param pNicFragment  The NIC_RX_FRAGMENT for this MPDU
 * \param pFragmentHdr  Buffer pointing to the header of this MPDU
 * \param uFragmentSize The size of this fragment in bytes
 * \return return NDIS_STATUS_SUCCESS if the packet has been
 * processed successfully. Any failure code will cause this
 * fragment to be dropped.
 * \sa MpEventSendPacket
 */
NDIS_STATUS
MpEventPacketReceived(
    __in  PADAPTER                        pAdapter,
    __in  PMP_RX_FRAGMENT                 pMpFragment,
    __in  PNIC_RX_FRAGMENT                pNicFragment,
    __in  PDOT11_MAC_HEADER               pFragmentHdr,
    __in  USHORT                          uFragmentSize
    )
{
    NDIS_STATUS     ndisStatus = NDIS_STATUS_SUCCESS;

    UNREFERENCED_PARAMETER(pAdapter);
    UNREFERENCED_PARAMETER(pMpFragment);
    UNREFERENCED_PARAMETER(pNicFragment);
    UNREFERENCED_PARAMETER(pFragmentHdr);
    UNREFERENCED_PARAMETER(uFragmentSize);
    
#ifdef USE_SAMPLE_PACKET_BUFFERING_IMPLEMENTATION

    //
    // If packet buffering is being done by the sample for sleeping
    // stations, we must check if this packet is a PS Poll and take
    // appropriate action. This assumes that Hardware is letting all
    // PS-Poll packets up irrespective of the packet filter set by OS
    // The Rtl8180 operates in this manner.
    //
    if (pFragmentHdr->FrameControl.PwrMgt)
    {
        if (MP_IS_PS_POLL_PACKET(pFragmentHdr) &&
            MP_COMPARE_MAC_ADDRESS(&pFragmentHdr->Address1, &pAdapter->CurrentAddress))
        {
            //
            // If this is a PS Poll packet for us, we need to check for buffered packets
            //
            ndisStatus = MpPSPacketsManagerHandlePSPollPacket(
                            pAdapter,
                            pFragmentHdr,
                            pFragmentHdr->DurationID,
                            TRUE        // Receive always occurs at dispatch level
                            );
            //
            // If we are required to indicate CTRL frames up, don't subsume this packet.
            // The Rtl8180, lets PSPoll pkts up even if filter asks to drop Ctrl frames.
            // Also, if we don't have a queue for this station, then the OS may not
            // be aware of this station being in low power state. We should let this
            // packet up.
            //
            if (pAdapter->PacketFilter & NDIS_PACKET_TYPE_802_11_DIRECTED_CTRL ||
                ndisStatus == NDIS_STATUS_FILE_NOT_FOUND)
            {
                ndisStatus = NDIS_STATUS_SUCCESS;
            }
            else
            {
                ndisStatus = NDIS_STATUS_FAILURE;
            }
        }
    }
    else
    {
        //
        // We need to potentially check for PS state change here.
        // The sample driver has a lazy implementation for cleaning up
        // any packet queues not in use. Hence, we need not keep an
        // active track to release those resources.
        // 
    }

#else   // USE_SAMPLE_PACKET_BUFFERING_IMPLEMENTATION

    
 
#endif  // USE_SAMPLE_PACKET_BUFFERING_IMPLEMENTATION

    return ndisStatus;

}

VOID
MpEventHandleInterrupt(
    __in  PADAPTER        pAdapter
    )
{    
#ifdef  USE_SAMPLE_PACKET_BUFFERING_IMPLEMENTATION 

    //
    // If we have to deliver queued DTIM packet, do it before any other
    // task. This is a time bound operation and takes priority over normal
    // sends.
    //
    if (pAdapter->HandleDTIMDeferred)
    {
        MpPSPacketsManagerDeliverDTIMPackets(pAdapter);
        pAdapter->HandleDTIMDeferred = FALSE;
    }

    //
    // Check if any ATIM packet has been send completed.
    //
    if (MP_RUNNING_IN_STA_ADHOC_MODE(pAdapter))
    {
        HwCheckForATIMPacketSendCompletion(pAdapter->Nic);
    }
#else   // USE_SAMPLE_PACKET_BUFFERING_IMPLEMENTATION

    UNREFERENCED_PARAMETER(pAdapter);

#endif  // USE_SAMPLE_PACKET_BUFFERING_IMPLEMENTATION
}

