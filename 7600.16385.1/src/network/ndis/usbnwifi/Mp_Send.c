/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Mp_Send.c

Abstract:
    MP layer send functions
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
#include "Precomp.h"

#include "Mp_Main.h"
#include "Mp_Send.h"
#if DOT11_TRACE_ENABLED
#include "Mp_Send.tmh"
#endif

#if DBG

/** Used to verify that mutual exclusion is not being violated during Send for checked builds */
MUTUAL_EXCLUSION_VERIFIER       SendMutexVerifier = 0;

/** Used to verify that mutual exclusion is not being violated during Scatter Gather Complete */
MUTUAL_EXCLUSION_VERIFIER       SGCompletionVerifier = 0;

/** Used to verify that mutual exclusion is not being violated during SendComplete for checked builds */
MUTUAL_EXCLUSION_VERIFIER       SendCompleteMutexVerifier = 0;

#endif  // DBG

#define MP_SET_SEND_FLAG(_M, _F)            ((_M)->SendFlags |= (_F))   
#define MP_CLEAR_SEND_FLAG(_M, _F)          ((_M)->SendFlags &= ~(_F))
#define MP_CLEAR_ALL_SEND_FLAGS(_M)         ((_M)->SendFlags = 0)
#define MP_TEST_SEND_FLAG(_M, _F)           (((_M)->SendFlags & (_F)) != 0)
#define MP_TEST_SEND_FLAGS(_M, _F)          (((_M)->SendFlags & (_F)) == (_F))


#define MP_REINITIALIZE_TX_MSDU(_Txd)       \
{                                           \
    MP_CLEAR_ALL_SEND_FLAGS(_Txd);          \
    MP_CLEAR_ALL_FLAGS(_Txd);               \
}

/*
 * The maximum number of Dot11 fragments we expect to practically see in a NBL 
 * Worst case, this value can go up to 9 and theoretically go up to 16
 */
#define MP_MAX_NUM_OF_FRAGMENTS            4

#define MP_MAX_PENDING_TX_PACKETS             5


/**
 * This Macro tries to obtain ownership of sending a Tx MSDU for the current thread.
 * If the ownership is successfullly obtained, the macro returns true. Else a
 * false value is returned. This macro is internal to the send path only.
 */
#define     MP_ACQUIRE_TX_MSDU_SEND_OWNERSHIP(_Txd)                             \
    (InterlockedCompareExchange(                                            \
                (PLONG) &(_Txd->SendFlags),                                 \
                (MP_TX_MSDU_IS_RESERVED | MP_TX_MSDU_IS_READY_FOR_SEND | MP_TX_MSDU_HAS_SENDER),    \
                (MP_TX_MSDU_IS_RESERVED | MP_TX_MSDU_IS_READY_FOR_SEND)                         \
                ) == (MP_TX_MSDU_IS_RESERVED | MP_TX_MSDU_IS_READY_FOR_SEND))



/**
 * This function allocates and initializes all the memory and variables needed
 * for sends to occur. This includes preallocation of both Miniport and Nic Tx MSDUs
 * as well as the association that is established between them.
 * 
 * \param pAdapter  The adapter context
 * \return NDIS_STATUS_SUCCESS on success or appropriate failure (like NDIS_STATUS_RESOURCES)
 * \sa MpReinitializeSendEngine, MpTerminateSendEngine, Hw11InitializeSend
 */
NDIS_STATUS
MpInitializeSendEngine(
    __in PADAPTER pAdapter
    )
{
    NDIS_STATUS         ndisStatus = NDIS_STATUS_SUCCESS;
    PMP_TX_MSDU         pMpTxd = NULL, pCurrentMpTxd;
    PNIC_TX_MSDU        pNicTxd;
    ULONG               i, fHw11Initialized = 0, ErrorValue = 0;
    NDIS_ERROR_CODE ErrorCode = NDIS_STATUS_SUCCESS;

    MPVERIFY(pAdapter);
    
    //
    // Here TotalTxMSDUs just tell us how many TX_MSDU to make available
    // The TX_MSDU are not yet allocated.
    //
    MPVERIFY(pAdapter->TotalTxMSDUs > 0);

    do
    {
        //
        // Allocate the TX_MSDU's for both Miniport and NIC
        //
        MP_ALLOCATE_MEMORY(pAdapter->MiniportAdapterHandle, &pMpTxd, (pAdapter->TotalTxMSDUs * sizeof(MP_TX_MSDU)), MP_MEMORY_TAG);
        if (pMpTxd == NULL)
        {
            ErrorCode = NDIS_ERROR_CODE_OUT_OF_RESOURCES;
            ndisStatus = NDIS_STATUS_RESOURCES;
            MpTrace(COMP_SEND, DBG_SERIOUS,  ("Failed to allocate the requested number (%d) of TX_MSDUs.\n", pAdapter->TotalTxMSDUs));
            break;
        }
        NdisZeroMemory(pMpTxd, (pAdapter->TotalTxMSDUs * sizeof(MP_TX_MSDU)));
        
        //
        // Let the NIC allocate the resources it needs for sending.
        // These would include shared memory etc.
        //
        ndisStatus = Hw11InitializeSend(
            pAdapter->Nic,
            pAdapter->TotalTxMSDUs
            );
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            break;
        }
        else
        {
            fHw11Initialized = 1;
        }
        
        //
        // Initialize the Tx MSDU list as a circular one.
        //
        pCurrentMpTxd = pMpTxd;
        for (i=0; i < pAdapter->TotalTxMSDUs; i++)
        {
            if (i < pAdapter->TotalTxMSDUs - 1)
            {
                pCurrentMpTxd->Next = pCurrentMpTxd + 1;
            }
            else
            {
                // Boundary condition. The last node needs to point to the first
                // to make list circular
                pCurrentMpTxd->Next = pMpTxd;
            }

            pCurrentMpTxd->Adapter = pAdapter;
            
            //
            // Associate this MP_TX_MSDU with the corresponding NIC_TX_MSDU
            //
            pNicTxd = NULL;
            ndisStatus = Hw11AssociateTxMSDU(
                pAdapter->Nic,
                pCurrentMpTxd,
                &pNicTxd
                );
            
            if (ndisStatus != NDIS_STATUS_SUCCESS)
                break;
            else
            {
                MPVERIFY(pNicTxd);
                pCurrentMpTxd->NicTxd = pNicTxd;
                MP_INCREMENT_AVAILABLE_TX_MSDU(pAdapter);
            }
            
            pCurrentMpTxd = pCurrentMpTxd->Next;
        }
        
        // If any error occurred in the loop, bail out
        if (ndisStatus != NDIS_STATUS_SUCCESS)
            break;
        
        //
        // Set the TxMSDUList pointer to first TX_MSDU. This pointer will remain unchanged.
        // Set the FreeTxMSDU pointer to the first TX_MSDU. Next available TX_MSDU is the first one.
        // Set NextTxMSDUToSend pointer to first TX_MSDU. Next Tx MSDU we will send is the first one.
        // Set NextTxMSDUToComplete to first TX_MSDU. Again ordering ensures first Tx MSDU gets completed first.
        //
        pAdapter->TxMSDUList = pMpTxd;
        pAdapter->FreeTxMSDU = pMpTxd;
        pAdapter->NextTxMSDUToSend = pMpTxd;
        pAdapter->NextTxMSDUToComplete = pMpTxd;
    } while (FALSE);
    
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        if (fHw11Initialized)
            MpTerminateSendEngine(pAdapter);
    
        if (pMpTxd)
            MP_FREE_MEMORY(pMpTxd);
        
        if (ErrorCode != NDIS_STATUS_SUCCESS)
        {
            NdisWriteErrorLogEntry(
                pAdapter->MiniportAdapterHandle,
                ErrorCode,
                ErrorValue
                );
        }
    }
    
    return ndisStatus;
}


/**
 * This function reinitializes the send engine. This is usually called
 * when a reset operation occurs.
 * 
 * \param pAdapter      The Adapter context for this miniport
 * \sa MPReset, MpInitializeSendEngine
 */
VOID
MpReinitializeSendEngine(
    __in PADAPTER     pAdapter
    )
{
    ULONG   i;
    PMP_TX_MSDU  pTxd;
    
    //
    // Reset the SendEngine.
    // 1. Reinitialize all TX_MSDUs in the list
    // 2. Reset some state variables
    //
    pTxd = pAdapter->TxMSDUList;
    for (i=0; i<pAdapter->TotalTxMSDUs; i++)
    {
        MP_REINITIALIZE_TX_MSDU(pTxd);
        Hw11ReinitializeTxMSDU(pTxd->NicTxd);
    }

    pAdapter->FreeTxMSDU = pAdapter->TxMSDUList;
    pAdapter->NextTxMSDUToSend = pAdapter->TxMSDUList;
    pAdapter->NextTxMSDUToComplete = pAdapter->TxMSDUList;
    pAdapter->NumAvailableTxMSDU = pAdapter->TotalTxMSDUs;
}


/**
 * Will release any resources allocated during MpInitializeSendEngine
 * 
 * \param pAdapter  The adapter context
 * \sa MpInitializeSendEngine
 */
VOID
MpTerminateSendEngine(
    __in PADAPTER pAdapter
    )
{
    ULONG i;
    MPVERIFY(pAdapter);

    //
    // Disassocaite each TX_MSDU that was previously associated in Initialize
    //
    for (i=0; i<pAdapter->NumAvailableTxMSDU; i++)
    {
        Hw11DisassociateTxMSDU(
            pAdapter->Nic,
            pAdapter->TxMSDUList[i].NicTxd
            );
    }
    
    //
    // Ask the NIC to release any resources allocated during
    // Hw11IntiializeSend
    //
    Hw11TerminateSend(pAdapter->Nic);
    
    //
    // Release the memory we allocated
    //
    if (pAdapter->TxMSDUList) {
        MP_FREE_MEMORY(pAdapter->TxMSDUList);
    }
    
}

/**
 * A helper method that determines whether the miniport driver will be
 * able to transmit the packet immediately. The conditions that need to
 * be satisfied are:
 *
 * 1. There is atleast one Tx MSDU available to be used for sending
 * 2. No scan operation is currently executing on the miniport
 *
 * \param pAdapter      The adapter context
 * \return TRUE if miniport can transmit immediately, else false
 * \sa MpSendTxMSDUs
 */
INLINE BOOL
MPCanTransmit(
    __in PADAPTER pAdapter
    )
{
    //
    // The requirements to be able to transmit are:
    // 1. NIC has available TX_MSDUs for sending
    // 2. A scan operation is not currently occuring
    //
    return (
        (pAdapter->NumAvailableTxMSDU > 0)   &&          // Has transmit buffers
        !MP_TEST_STATUS_FLAG(pAdapter, MP_ADAPTER_HW_IS_SCANNING)   // scan is in progress
        );
}


/**
 * Internal helper routine to be called to check if there are packets
 * ready for sending in the Tx MSDU list. This function will send any packets
 * that are ready for sending maintaining the send order requirement.
 * This function is thread safe
 * 
 * \param pAdapter      The adapter context for this miniport
 * \param DispatchLevel True if the IRQL is dispatch else false
 * \sa MpSendTxMSDUs
 */
INLINE VOID
MpSendReadyTxMSDUs(
    __in  PADAPTER    pAdapter,
    __in BOOLEAN      DispatchLevel
    )
{
    PMP_TX_MSDU pTxd;
    
    //
    // Starting from NextTxMSDUToSend, we will try to send, in a thread-safe manner,
    // as many TX_MSDUs as are ready and possible to send.
    //
    pTxd = pAdapter->NextTxMSDUToSend;
    while (MP_TEST_SEND_FLAG(pTxd, MP_TX_MSDU_IS_READY_FOR_SEND))
    {
        //
        // Ofcourse, multiple threads can be here simultaneouly. They all think this
        // Tx MSDU is the next one to be transmitted and is ready for sending. However, we
        // want only one thread to do the transmission (to avoid multiple sends of same
        // packet). All threads will try to become the sender of this packet through the
        // following Interlocked operation but only one will succeed.
        //
        if (MP_ACQUIRE_TX_MSDU_SEND_OWNERSHIP(pTxd))
        {
            //
            // This TX_MSDU is ready for sending and this thread has won the contention
            // for ownership of sending. Any thread that fails to win contention will
            // abondon trying to acquire it and will leave immediately
            //            
            if (MpTransmitTxMSDU(pAdapter, pTxd, DispatchLevel) != NDIS_STATUS_SUCCESS)
            {
                //
                // Transmission of this Tx MSDU did not succeed. The only reason could
                // be NDIS_STATUS_RESOURCES. In order to guarantee order of delivery
                // we will not submit any more packets and bail out. Try again later.
                //
                break;
            }
            
            //
            // Move onto next TX_MSDU to see if it is ready for sending
            //
            pTxd = pAdapter->NextTxMSDUToSend;
        }
        else
        {
            break;
        }
    }
}


/**
 * This function is called to transmit a bunch of Tx MSDUs each of which has
 * a NDIS Packet associated with it. This function goes through the Tx MSDU 
 * list sequentially until the specified number of Tx MSDUs have been passed to
 * the hardware to be transmitted.
 * 
 * \param pAdapter      The adapter context assoicated with the NIC
 * \param pMpTxd        A pointer to the first Tx MSDU to be transmitted
 * \param NumTxd        The number of Tx MSDUs to transmit starting from the first one
 * \sa MpSendSingleTxMSDU, MPSendNetBufferLists
 */
VOID
MpSendTxMSDUs(
    __in PADAPTER        pAdapter,
    __in PMP_TX_MSDU     pMpTxd,
    __in ULONG           NumTxd,
    __in BOOLEAN         DispatchLevel
    )
{
    ULONG           i;

    //
    // Transmit all the Tx MSDUs submitted by the caller. If the send operation
    // fails, we will still continue. The Tx MSDU will simply get queued up and
    // get retried later
    //
    for(i=0; i<NumTxd; i++)
    {
        MpSendSingleTxMSDU(pAdapter, pMpTxd, DispatchLevel);
        pMpTxd = pMpTxd->Next;
    }

    //
    // Flush all TX_MSDUs that are ready for sending. If some Tx MSDU was holding up
    // the queue, it will get sent.
    //
    MpSendReadyTxMSDUs(pAdapter, DispatchLevel);
}


/**
 * This function sends a single Tx MSDU to the hardware for asynchronous transmission.
 * After the Tx MSDU is ready, a call is made to Hw11TransmitPacket in a synchronous,
 * sequential manner during which tranmission of the packet can be initiaited.
 * If Tx MSDU is ready but cannot be sent because of send order restrictions, we will
 * mark the Tx MSDU as ready for send and return immediately. This Tx MSDU will be transmitted
 * when its turn arrives in the Tx MSDU queue.
 *
 * \param pAdapter      The context associated with this adapter.
 * \param pMpTxd        The Tx MSDU that needs to be prepared for tranmission
 * \return NDIS_STATUS_SUCCESS if Tx MSDU immediately given to hardware, else NDIS_STATUS_PENDING.
 * In either case, the associated packet will not be send completed synchronously but will
 * happen through the interrupt generated by the NIC.
 * \sa MpSendTxMSDUs, MpTransmitTxMSDU
 */
INLINE NDIS_STATUS
MpSendSingleTxMSDU(
    __in PADAPTER        pAdapter,
    __in PMP_TX_MSDU     pMpTxd,
    __in BOOLEAN         DispatchLevel
    )
{
    NDIS_STATUS                 ndisStatus;
    
    //
    // Verify args and also verify that the packet being sent is the same
    // as the packet for which TX_MSDU was reserved
    //
    MPVERIFY(pMpTxd);
    MPVERIFY(MP_TEST_SEND_FLAG(pMpTxd, MP_TX_MSDU_IS_RESERVED)); // TX_MSDU must be reserved
    MPVERIFY(pMpTxd->NetBufferList);       // There is an associated MSDU
    MPVERIFY(pMpTxd->Dot11SendContext);   // There is Dot11 send context Info

    //
    // Check if we can send this TX_MSDU. Reason for not sending would be
    // to avoid out-of-order delivery
    //
    if(pMpTxd == pAdapter->NextTxMSDUToSend)
    {
        //
        // This Tx MSDU is the next in line for transmission. Send it out.
        //
        ndisStatus = MpTransmitTxMSDU(pAdapter, pMpTxd, DispatchLevel);
    }
    else
    {
        //
        // Although prepared, the turn for transmission of this Tx MSDU has not yet
        // come. Mark the Tx MSDU as ready. It will be sent when its turn comes
        //
        MP_SET_SEND_FLAG(pMpTxd, MP_TX_MSDU_IS_READY_FOR_SEND);
        ndisStatus = NDIS_STATUS_PENDING;
    }
    
    return ndisStatus;
}


/**
 * This is an internal helper function. It is used to reserve a Tx MSDU for a particular
 * packet. This function assumes that SendLock is acquired and that the caller has
 * already verified that there is a Tx MSDU available for this NBL.
 * 
 * \param pAdapter          The adapter context for this miniport
 * \param NetBufferList     The NBL for which a TX_MSDU is to be reserved.
 * \return A pointer to the reserved TX_MSDU
 * \sa MPSendNetBufferLists, MpSendTxMSDUs
 */
PMP_TX_MSDU
MPReserveTxResources(
    __in PADAPTER                 pAdapter,
    __in PNET_BUFFER_LIST         NetBufferList
    )
{
    PMP_TX_MSDU  pReservedTxd;
    MPVERIFY(pAdapter->NumAvailableTxMSDU > 0);
    MPVERIFY(pAdapter->FreeTxMSDU->SendFlags == MP_TX_MSDU_IS_FREE);
    
    //
    // One more TX_MSDU will now be in use. Decrement available count and 
    // update TX_MSDU fields.
    //
    MP_DECREMENT_AVAILABLE_TX_MSDU(pAdapter);
    pReservedTxd = pAdapter->FreeTxMSDU;
    MP_SET_SEND_FLAG(pReservedTxd, MP_TX_MSDU_IS_RESERVED);
    pReservedTxd->NetBufferList = NetBufferList;

    //
    // The next free TX_MSDU is ahead
    // Note that the next free TX_MSDU may not actually be free
    // (will happen under heavy load), but it is the TX_MSDU
    // that "will next become" free.
    //
    pAdapter->FreeTxMSDU = pAdapter->FreeTxMSDU->Next;
    
    return pReservedTxd;
}


/**
 * This function used to hand off a prepared TX_MSDU to the underlying hardware for 
 * transmission. The design of the send engine is such that this function is
 * called in a synchronous manner i.e. no two threads will simultaneously be present
 * in this function. For checked build of the driver, there are mechanisms in place
 * to catch such violations. The synchronous calls help ensure that the hardware is
 * presented a TX_MSDU one-at-a-time, making it easier to implement.
 * 
 * \param pAdapter      The adapter context for this miniport driver
 * \param pTxd          The prepared TX_MSDU that needs to be given to the hardware
 * \param DispatchLevel Set to TRUE if the IRQL is DISPATCH
 * \sa MpSendTxMSDUs, MpReserveTxResources
 */
INLINE NDIS_STATUS
MpTransmitTxMSDU(
    __in PADAPTER         pAdapter,
    __in PMP_TX_MSDU      pTxd,
    __in BOOLEAN          DispatchLevel
    )
{
    NDIS_STATUS     status;
    //
    // For checked builds verify that calls to this function are seralized
    //
    MP_ENTER_MUTEX_REGION(SendMutexVerifier);
    
    MPVERIFY(pTxd == pAdapter->NextTxMSDUToSend);

    MP_ACQUIRE_SEND_LOCK(pAdapter, FALSE);


    //
    // Mark the TX_MSDU as going in send stage
    //
    MP_SET_SEND_FLAG(pTxd, MP_TX_MSDU_IS_IN_SEND);

    MP_INCREMENT_PENDING_SENDS(pAdapter);

    //
    // The next TX_MSDU to send pointer needs to be incremented
    //
    pAdapter->NextTxMSDUToSend = pTxd->Next;

    status = Hw11PrepareTxMSDUForSend(pAdapter->Nic, pTxd);
    if (status != NDIS_STATUS_SUCCESS)
    {
        MP_RELEASE_SEND_LOCK(pAdapter, FALSE);
        Mp11SendCompleteNBL(pAdapter);
        return status;
    }

    MP_RELEASE_SEND_LOCK(pAdapter, FALSE);
    
    //
    // Submit the Tx MSDU to the hardware
    // This function actually sends data out on the wire
    //
    status = Hw11TransmitTxMSDU(pAdapter->Nic, pTxd, DispatchLevel);
    if (status != NDIS_STATUS_SUCCESS && status != NDIS_STATUS_RESOURCES)
    {
        Mp11SendCompleteNBL(pAdapter);
    }
   
    MP_LEAVE_MUTEX_REGION(SendMutexVerifier);

    return status;
}


VOID
MpPrepareTxMSDUForCompletion(
    __in PADAPTER     pAdapter,
    __in PMP_TX_MSDU  pTxd,
    __in NDIS_STATUS  MSDUStatus
    )
{
    //PNET_BUFFER     CurrentNetBuffer;
   // USHORT          i, SGElemCount;

    //
    // For checked builds verify that calls to this function are seralized
    //
    MP_ENTER_MUTEX_REGION(SendCompleteMutexVerifier);
    MPVERIFY(pTxd == pAdapter->NextTxMSDUToComplete);
    MPVERIFY(pTxd->SendFlags & MP_TX_MSDU_IS_IN_SEND);   
    MpTrace(COMP_TESTING, DBG_LOUD, ("Send Complete Tx MSDU %p\n", pTxd));
    
    //
    // Set the completion status of this NBL
    //
    NET_BUFFER_LIST_STATUS(pTxd->NetBufferList) = MSDUStatus;
    
    //
    // Increment the appropriate statistics
    //
    if (MSDUStatus == NDIS_STATUS_SUCCESS)
        MP_INCREMENT_TOTAL_GOOD_TRANSMITS(pAdapter);
    else
        MP_INCREMENT_TOTAL_BAD_TRANSMITS(pAdapter);
    
    //
    // Note down send-completion.
    //
    MP_DECREMENT_PENDING_SENDS(pAdapter);
    MP_INCREMENT_TOTAL_TRANSMITS(pAdapter);
    
    //
    // The pointer to next TX_MSDU to be completed needs to be incremented
    //
    pAdapter->NextTxMSDUToComplete = pTxd->Next;

    //
    // Invoke the station to notify about this packet completion before we complete it
    //
    Sta11ProcessSendComplete(
        pAdapter->Station,
        pTxd,
        MSDUStatus
        );

    //
    // Invoke MpEvents to notify about this packet completion before we do.
    //
    MpEventSendComplete(pAdapter, pTxd, MSDUStatus, TRUE);

    //
    // Clear out the Tx MSDU flags. This reinitializes the TX_MSDU
    //
    MP_REINITIALIZE_TX_MSDU(pTxd);
        
    MP_LEAVE_MUTEX_REGION(SendCompleteMutexVerifier);
}


__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
MpCompleteQueuedTxMSDUs(
    __in PADAPTER     pAdapter
    )
{
    PMP_TX_MSDU         pTxd;
    ULONG               ulTotalTxMSDUs;
    NDIS_STATUS         ndisStatus = MpGetAdapterStatus(pAdapter);
    PNET_BUFFER_LIST    NetBufferList = NULL;
    ULONG               index;

    MpEntry;
    
    //
    // Start from the next TX_MSDU which is to be send completed
    // go through all TX_MSDUs that are currently reserved.
    //
    pTxd = pAdapter->NextTxMSDUToComplete;
    ulTotalTxMSDUs = pAdapter->TotalTxMSDUs - pAdapter->NumAvailableTxMSDU;
    
    for (index = 0; index < ulTotalTxMSDUs; index++)
    {
        MPVERIFY(pTxd);
        MpTrace (COMP_SEND, DBG_NORMAL, ("Cancelling TX_MSDU %p associated with NBL %p\n", pTxd, pTxd->NetBufferList));
        
        //
        // This TX_MSDU should be failed irrespective of whether it was sent
        // or not. Hardware is possibly in a bad state so no assumptions can
        // be made about information provided by it.
        //
        MpPrepareTxMSDUForCompletion(pAdapter, pTxd, ndisStatus);

        //
        // Add this NBL to the list that will be completed back to the OS
        //
        NET_BUFFER_LIST_NEXT_NBL(pTxd->NetBufferList) = NetBufferList;
        NetBufferList = pTxd->NetBufferList;

        //
        // Go to the next Tx MSDU and check if it needs to be completed
        //
        pTxd = pTxd->Next;
        MPVERIFY(pTxd == pAdapter->NextTxMSDUToComplete);
    }
    
    //
    // Send complete all the failed NBL to NDIS
    //
    if (NetBufferList)
    {
        MP_RELEASE_SEND_LOCK(pAdapter, FALSE);
        MP_SEND_COMPLETE_NBLS(pAdapter, NetBufferList, FALSE);
        MP_ACQUIRE_SEND_LOCK(pAdapter, FALSE);
        MP_ADD_AVAILABLE_TX_MSDU(pAdapter, ulTotalTxMSDUs);
    }
    
    MPVERIFY(pAdapter->TotalTxMSDUs == pAdapter->NumAvailableTxMSDU);

    MpExit;
}


__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
MpCompleteQueuedNBL(
    __in PADAPTER    pAdapter
    )
{
    PNET_BUFFER_LIST    NetBufferList, CurrentNBL;
    NDIS_STATUS         ndisStatus = MpGetAdapterStatus(pAdapter);

    MpEntry;

    //
    // Since we also use the Next field to link NBL in the TxQueue, we
    // can simply complete the entire lot in one shot.
    //
    if (!MpNBLQueueIsEmpty(&pAdapter->TxQueue))
    {
        //
        // Save a reference to the first NBL and then reset the NBL Queue
        //
        NetBufferList = MP_GET_NBL_FROM_QUEUE_ENTRY(pAdapter->TxQueue.Head);
        MPVERIFY(NetBufferList);

        if (NetBufferList != NULL)
        {
            //
            // Set the NDIS_STATUS for each NBL being failed
            //
            for(CurrentNBL = NetBufferList; CurrentNBL != NULL; CurrentNBL = NET_BUFFER_LIST_NEXT_NBL(CurrentNBL))
            {
                MP_SET_NBL_STATUS(CurrentNBL, ndisStatus);
            }

            //
            // Fix the TX_QUEUE so it will be empty.
            //
            pAdapter->TxQueue.Head = NULL;
            pAdapter->TxQueue.Tail = NULL;
            pAdapter->TxQueue.Count = 0;
            
            MP_RELEASE_SEND_LOCK(pAdapter, FALSE);
            MP_SEND_COMPLETE_NBLS(pAdapter, NetBufferList, FALSE);
            MP_ACQUIRE_SEND_LOCK(pAdapter, FALSE);
        }
    }
    
    MpExit;
}


/**
 * This function is called by NDIS to cancel any NBL that have
 * not yet been sent and which correspond to CancelId passed in.
 * 
 * \param MiniportAdapterContext    The context for this miniport
 * \param CancelId                  Packets with this Id will be cancelled
 * \sa MPSendPackets
 */
VOID
MPCancelSendNetBufferLists(
    __in    NDIS_HANDLE              MiniportAdapterContext,
    __in    PVOID                    CancelId
    )
{
    PADAPTER pAdapter = (PADAPTER) MiniportAdapterContext;
    MP_NBL_QUEUE        CancelNBLQueue;
    PQUEUE_ENTRY        CurrentEntry, PrevEntry;
    PNET_BUFFER_LIST    NetBufferList;
    PVOID               NBLCancelId = NULL;

    MpInitNBLQueue(&CancelNBLQueue);

    MP_ACQUIRE_SEND_LOCK(pAdapter, FALSE);

    //
    // Walk through the send list looking for NBL with matching cancel id
    //
    CurrentEntry = pAdapter->TxQueue.Head;
    PrevEntry = pAdapter->TxQueue.Head; // To avoid Prefast warning, intialize to head

    while(CurrentEntry != NULL)
    {
        //
        // Get the NBL from the Queue Entry
        //
        NetBufferList = MP_GET_NBL_FROM_QUEUE_ENTRY(CurrentEntry);

        //
        // If the cancel ID mactches, remove the NBL from TxQueue and
        // place the NBL on a local Cancel Queue
        //
        
        NBLCancelId = NET_BUFFER_LIST_INFO(NetBufferList, NetBufferListCancelId);
        if (NBLCancelId == CancelId)
        {
            if (CurrentEntry == pAdapter->TxQueue.Head)
            {
                pAdapter->TxQueue.Head = CurrentEntry->Next;
                if (CurrentEntry->Next == NULL)
                {
                    pAdapter->TxQueue.Tail = NULL;
                }
            }
            else
            {
                PrevEntry->Next = CurrentEntry->Next;
                if (CurrentEntry->Next == NULL)
                {
                    pAdapter->TxQueue.Tail = PrevEntry;
                }
            }

            pAdapter->TxQueue.Count--;
            MpQueueNBL(&CancelNBLQueue, NetBufferList);
        }

        PrevEntry = CurrentEntry;
        CurrentEntry = CurrentEntry->Next;
    }
    
    MP_RELEASE_SEND_LOCK(pAdapter, FALSE);
    
    //
    // If there were any packets successfully cancelled, fail them
    //
    while (MpNBLQueueDepth(&CancelNBLQueue) > 0)
    {
        NetBufferList = MpDequeueNBL(&CancelNBLQueue);
        MPVERIFY(NetBufferList);
        
        MP_SEND_COMPLETE_ONE_NBL(
            pAdapter,
            NetBufferList,
            NDIS_STATUS_REQUEST_ABORTED,
            FALSE
            );
    }
    
    MpDeinitNBLQueue(&CancelNBLQueue);
}


/**
 * The Hw11 portion can call this function if it needs to flush out
 * and Tx MSDUs that are ready for sending but have not yet been sent due
 * to restrictions on send order.
 * 
 * \param pAdapter          The adapter context for this miniport
 * \param DispatchLevel     True if current IRQL is dispatch
 * \sa MpSendReadyTxMSDUs
 */
VOID
Mp11PollForTransmission(
    __in  PADAPTER    pAdapter,
    __in  BOOLEAN     DispatchLevel
    )
{
    MpSendReadyTxMSDUs(pAdapter, DispatchLevel);
}


/**
 * This function is called by NDIS when the OS submits a NBL for transmission to
 * this miniport. The NBL will be transmitted immediately if possible, or otherwise
 * is queued up for later trannmission by the miniport.
 * 
 * \param MiniportAdapterContext        The context for this miniport
 * \param NetBufferList                 The chain of NBL to be transmitted
 * \param PortNumber                    Port number for this send packet
 * \param SendFlags                     Flags such as dispatch level, etc for send
 *
 * \sa MPHandleSendCompleteInterrupt. MpTransmitTxMSDU
 */
VOID
MPSendNetBufferLists(
    __in  NDIS_HANDLE         MiniportAdapterContext,
    __in  PNET_BUFFER_LIST    NetBufferList,
    __in  NDIS_PORT_NUMBER    PortNumber,
    __in  ULONG               SendFlags
    )
{
    PADAPTER            pAdapter = (PADAPTER)MiniportAdapterContext;
    NDIS_STATUS         ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_EXTSTA_SEND_CONTEXT pDot11SendContext;
    PNET_BUFFER_LIST    CurrentNetBufferList, NextNetBufferList, FailedNBLs = NULL;
    PMP_TX_MSDU         pFirstTxd, pTxd;
    UINT                NumTxdPrepared = 0;
    BOOLEAN             DispatchLevel = SendFlags & NDIS_SEND_FLAGS_DISPATCH_LEVEL ? TRUE : FALSE;

    UNREFERENCED_PARAMETER(PortNumber);
    
    do
    {
        //
        // Acquire the lock. The lock should be acquired before checking if we can
        // send packets in order to synchronize with MpReset.
        //
        MP_ACQUIRE_SEND_LOCK(pAdapter, DispatchLevel);
        
        //
        // If the adapter cannot send due to reasons like no cable,
        // middle of reset, halt or pause, or just hardware errors
        // we will fail all the sends
        //
        if (MP_ADAPTER_CANNOT_SEND_PACKETS(pAdapter))
        {
            NDIS_STATUS     failStatus = MpGetAdapterStatus(pAdapter);
            MP_RELEASE_SEND_LOCK(pAdapter, DispatchLevel);
            
            for(CurrentNetBufferList = NetBufferList;
                CurrentNetBufferList != NULL;
                CurrentNetBufferList = NET_BUFFER_LIST_NEXT_NBL(CurrentNetBufferList))
            {
                MP_SET_NBL_STATUS(CurrentNetBufferList, failStatus);
            }

            if (NetBufferList != NULL)
                MP_SEND_COMPLETE_NBLS(pAdapter, NetBufferList, DispatchLevel);
            break;
        }
        
        //
        // There are no pended NetBuffers. Lets try to send these packet.
        // First, store a local reference to the first free Tx MSDU in the list
        //
        pFirstTxd = pAdapter->FreeTxMSDU;
        
        for(CurrentNetBufferList = NetBufferList;
            CurrentNetBufferList != NULL;
            CurrentNetBufferList = NextNetBufferList)
        {
            MPASSERT(NET_BUFFER_LIST_FIRST_NB(CurrentNetBufferList) != NULL);
            
            //
            // Cache a reference to the next NBL right away. We will process one NBL
            // at a time so the CurrentNBL next pointer will be set to NULL.
            //
            NextNetBufferList = NET_BUFFER_LIST_NEXT_NBL(CurrentNetBufferList);
            NET_BUFFER_LIST_NEXT_NBL(CurrentNetBufferList) = NULL;
            
            //
            // Retrieve the 802.11 Send Context Information placed by Native Wifi
            //
            pDot11SendContext = MP_GET_SEND_CONTEXT(CurrentNetBufferList);            
            MPASSERT(pDot11SendContext != NULL);

            ndisStatus = MpEventSendNetBufferList(
                            pAdapter,
                            CurrentNetBufferList,
                            SendFlags,
                            pDot11SendContext,
                            TRUE        // Lock is acquired
                            );
            if (ndisStatus == NDIS_STATUS_SUCCESS)
            {
                // NBL has been subsumed.
                continue;
            }
            else if(ndisStatus == NDIS_STATUS_NOT_RECOGNIZED)
            {
                // Continue processing this NBL
            }
            else
            {
                // NBL has been failed. Add it to the list of failed NBLs
                MP_SET_NBL_STATUS(CurrentNetBufferList, ndisStatus);
                NET_BUFFER_LIST_NEXT_NBL(CurrentNetBufferList) = FailedNBLs;
                FailedNBLs = CurrentNetBufferList;
                continue;
            }
            
            ndisStatus = Sta11SendNetBufferList(
                            pAdapter->Station,
                            CurrentNetBufferList,
                            SendFlags,
                            pDot11SendContext
                            );
            if (ndisStatus == NDIS_STATUS_SUCCESS)
            {
                // NBL has been subsumed.
                continue;
            }
            else if(ndisStatus == NDIS_STATUS_NOT_RECOGNIZED)
            {
                // Continue processing this NBL
            }
            else
            {
                // NBL has been failed. Add it to the list of failed NBLs
                MP_SET_NBL_STATUS(CurrentNetBufferList, ndisStatus);
                NET_BUFFER_LIST_NEXT_NBL(CurrentNetBufferList) = FailedNBLs;
                FailedNBLs = CurrentNetBufferList;
                continue;
            }

            //
            // First, check if we have any packets pended from previous sends and Mp
            // can currently transmit packet.
            //
            if (MpNBLQueueIsEmpty(&pAdapter->TxQueue) &&
                MPCanTransmit(pAdapter) &&
                Sta11CanTransmit(pAdapter->Station) &&
                Hw11CanTransmitNBL(pAdapter->Nic, CurrentNetBufferList, pDot11SendContext))
            {
                //
                // Lets reserve a TX_MSDU for this packet. Reservation guarantees that no
                // one else will use the reserved TX_MSDU and avoid synchronisation issues.
                //
                pTxd = MPReserveTxResources(pAdapter, CurrentNetBufferList);

                pTxd->Dot11SendContext = pDot11SendContext;

                Sta11ProcessSend(
                    pAdapter->Station,
                    pTxd
                    );
                // Cannot fail, etc
                NumTxdPrepared++;
            }
            else
            {
                //
                // There are NB present to be sent before this or there are
                // resource limitations. Queue this NBL.
                //
                MpQueueNBL(&pAdapter->TxQueue, CurrentNetBufferList);
            }
        }
        
        //
        // If we were able to reserve at least one TX_MSDU, we can transmit them
        //
        if (NumTxdPrepared > 0)
        {
            //
            // Increment the number of active threads doing sends. Must be protected
            // by send lock. The release operation need not be protected.
            //
            MP_INCREMENT_ACTIVE_SENDERS(pAdapter);
            
            //
            // Release the send lock for other threads and SendCompleteDpc to acquire.
            // Transmission can occur without the lock aacquired as TX_MSDU are reserved
            //
            MP_RELEASE_SEND_LOCK(pAdapter, DispatchLevel);
            MpSendTxMSDUs(pAdapter, pFirstTxd, NumTxdPrepared, DispatchLevel);
            
            MP_DECREMENT_ACTIVE_SENDERS(pAdapter);
        }
        else
        {
            MP_RELEASE_SEND_LOCK(pAdapter, DispatchLevel);
        }

        //
        // Now that spinlock has been released, return failed NBL if any to the OS
        //
        if (FailedNBLs != NULL)
        {
            MP_SEND_COMPLETE_NBLS(
                pAdapter,
                FailedNBLs,
                DispatchLevel
                );
        }

    }while(FALSE);
}

VOID
Mp11SendCompleteNBL(
    PADAPTER            pAdapter
    )
{
    PMP_TX_MSDU         pTxd;
    PNET_BUFFER_LIST    NetBufferList = NULL;
    UINT                NumPacketsCompleted = 0;
    ULONG               pendingSends = 0;
    
    
    MP_ACQUIRE_SEND_LOCK(pAdapter, FALSE);

    pendingSends = pAdapter->PendingSends;
    pTxd = pAdapter->NextTxMSDUToComplete;
    if (pendingSends > 0 && Hw11TxMSDUIsComplete(pAdapter->Nic, pTxd->NicTxd))
    {
        do
        {
            //
            // Send Complete this Tx MSDU.
            //
            MpPrepareTxMSDUForCompletion(
                pAdapter,
                pTxd,
                Hw11TxMSDUTransmissionStatus(pAdapter->Nic, pTxd->NicTxd)
                );
            
            //
            // Add this NBL to the list of NBLs that we will Send Complete
            //
            NET_BUFFER_LIST_NEXT_NBL(pTxd->NetBufferList) = NetBufferList;
            NetBufferList = pTxd->NetBufferList;
            
            //
            // Move to the next TX_MSDU to check if it has been completed
            //
            pTxd = pAdapter->NextTxMSDUToComplete;

            NumPacketsCompleted++;
            MP_INCREMENT_AVAILABLE_TX_MSDU(pAdapter);
            pendingSends--;
        } while (pendingSends > 0  && Hw11TxMSDUIsComplete(pAdapter->Nic, pTxd->NicTxd));

        //
        // Increment the available Tx MSDU count
        //
    }

    MP_RELEASE_SEND_LOCK(pAdapter, FALSE);

    if (NetBufferList != NULL)
    {
        MP_SEND_COMPLETE_NBLS(pAdapter, NetBufferList, TRUE);
    }
}

VOID
Mp11HandleSendCompleteInterrupt(
    PADAPTER            pAdapter
    )
{
    PMP_TX_MSDU         pTxd, pFirstTxd;
    PNET_BUFFER_LIST    NetBufferList = NULL;
#ifdef SIMPLE_NATIVE_MODEL
    PDOT11_SEND_CONTEXT Dot11SendContext;
#else
    PDOT11_EXTSTA_SEND_CONTEXT Dot11SendContext;
#endif
    UINT                NumPacketsToTransmit;

    //
    // Note: Usb completion can happen at IRQL < DISPATCH_LEVEL so don't use
    // MP_ACQUIRE_SEND_LOCK at DISPATCH_LEVEL here.
    //

    //
    // Let the hardware process any sends it has pending
    //
    Hw11ProcessPendingSend(pAdapter->Nic);

    //
    // Flush any Tx MSDUs to the hardware that are ready for sending
    //
    MpSendReadyTxMSDUs(pAdapter, TRUE);
    
    MP_ACQUIRE_SEND_LOCK(pAdapter, FALSE);
    //
    // Send out any packets queued up if possible. Do this only if the 
    // HwUsbPendRequestsTimer fires and not for ordinary completions
    //
    if (!MpNBLQueueIsEmpty(&pAdapter->TxQueue) &&
         Sta11CanTransmit(pAdapter->Station) &&
         MP_ADAPTER_CAN_SEND_PACKETS(pAdapter))
    {
        //
        // The adapter can transmit packets. Send out as many as possible.
        // First, store a local reference to the first free Tx MSDU in the list
        // Next figure out how many packets can be sent
        //
        pFirstTxd = pAdapter->FreeTxMSDU;
        NumPacketsToTransmit = 0;
        
        while((!MpNBLQueueIsEmpty(&pAdapter->TxQueue)) &&
                MPCanTransmit(pAdapter) && 
                NumPacketsToTransmit < MP_MAX_PENDING_TX_PACKETS )
        {
            NetBufferList = MpDequeueNBL(&pAdapter->TxQueue);
            MPVERIFY(NetBufferList);
            
            Dot11SendContext = MP_GET_SEND_CONTEXT(NetBufferList);
            
            if (Hw11CanTransmitNBL(pAdapter->Nic, NetBufferList, Dot11SendContext))
            {
                //
                // Lets reserve a TX_MSDU for this packet. Reservation guarantees that no
                // one else will use the reserved TX_MSDU and avoid synchronisation issues.
                //
                pTxd = MPReserveTxResources(pAdapter, NetBufferList);
                pTxd->Dot11SendContext = Dot11SendContext;

                Sta11ProcessSend(
                    pAdapter->Station,
                    pTxd
                    );
                // Cannot fail, cannot subsume packets
                NumPacketsToTransmit++;
            }
            else
            {
                //
                // Hardware is unable to send this packet. Return it to the queue
                // This Queueing is priority so that this is the next packet which
                // will be sent out by the miniport. This will preserve order of sends
                //
                MpQueueNBLPriority(&pAdapter->TxQueue, NetBufferList);
                break;
            }
        }

        if (NumPacketsToTransmit > 0)
        {
            //
            // Increment the number of active threads doing sends. Must be protected
            // by send lock. The release operation need not be protected.
            //
            MP_INCREMENT_ACTIVE_SENDERS(pAdapter);
            
            //
            // Release the send lock for other threads and SendCompleteDpc to acquire.
            // Transmission can occur without the lock aacquired as TX_MSDU are reserved
            //            
            MP_RELEASE_SEND_LOCK(pAdapter, FALSE);
            MpSendTxMSDUs(pAdapter, pFirstTxd, NumPacketsToTransmit, TRUE);
            
            MP_DECREMENT_ACTIVE_SENDERS(pAdapter);
        }
        else
        {
            //
            // No packets need sending. Release lock
            //
            MP_RELEASE_SEND_LOCK(pAdapter, FALSE);
        }
    }
    else
    {
        //
        // We can't send packet. Release the lock
        //
        MP_RELEASE_SEND_LOCK(pAdapter, FALSE);
    }
}

