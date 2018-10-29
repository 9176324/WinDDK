/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Mp_PacketQueue.h

Abstract:
    Packet queuing macros
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
#ifndef _NATIVE_WIFI_NET_BUFFER_QUEUE

#define _NATIVE_WIFI_NET_BUFFER_QUEUE

typedef struct _QUEUE_ENTRY
{
    struct _QUEUE_ENTRY *Next;
} QUEUE_ENTRY, *PQUEUE_ENTRY;

typedef struct _MP_NBL_QUEUE
{
    PQUEUE_ENTRY        Head;
    PQUEUE_ENTRY        Tail;
    ULONG               Count;
} MP_NBL_QUEUE, *PMP_NBL_QUEUE;


#define MP_VERIFY_QUEUE(pNBLQueue)           \
    MPVERIFY(!(pNBLQueue->Head == NULL && pNBLQueue->Count != 0));   \
    MPVERIFY(!(pNBLQueue->Head != NULL && pNBLQueue->Count == 0));   \
    MPVERIFY(!(pNBLQueue->Head == NULL && pNBLQueue->Tail != NULL)); \
    MPVERIFY(!(pNBLQueue->Head != NULL && pNBLQueue->Tail == NULL));

//
// If Next is no longer the first link in the NBL, change these macros
//
#define MP_GET_QUEUE_ENTRY_FROM_NBL(_NBL)      \
    (PQUEUE_ENTRY)_NBL
#define MP_GET_NBL_FROM_QUEUE_ENTRY(_QueueEntry)      \
    (PNET_BUFFER_LIST)_QueueEntry;


INLINE BOOL
MpNBLQueueIsEmpty(
    PMP_NBL_QUEUE pNBLQueue
    )
{
    MP_VERIFY_QUEUE(pNBLQueue);
    return (pNBLQueue->Head == NULL);
}


INLINE ULONG
MpNBLQueueDepth(
    PMP_NBL_QUEUE    pNBLQueue
    )
{
    MP_VERIFY_QUEUE(pNBLQueue);
    return pNBLQueue->Count;
}


INLINE VOID
MpInitNBLQueue(
    PMP_NBL_QUEUE       pNBLQueue
    )
{
    NdisZeroMemory(pNBLQueue, sizeof(MP_NBL_QUEUE));
}

INLINE VOID
MpDeinitNBLQueue(
    PMP_NBL_QUEUE       pNBLQueue
    )
{
    //
    // For debug versions of the driver, assert if a leak is detected.
    //
    if (!MpNBLQueueIsEmpty(pNBLQueue))
    {
        MPASSERTMSG("Attempt to deinitialize a NBL queue that is not empty\n", FALSE);
    }
}

INLINE VOID
MpQueueNBL(
    PMP_NBL_QUEUE       pNBLQueue,
    PNET_BUFFER_LIST    NetBufferList
    )
{
    PQUEUE_ENTRY pQueueEntry;
    MP_VERIFY_QUEUE(pNBLQueue);

    pQueueEntry = MP_GET_QUEUE_ENTRY_FROM_NBL(NetBufferList);
    pQueueEntry->Next = NULL;
    
    //
    // If queue is empty, the head is NULL. New head should be this entry
    //
    if (MpNBLQueueIsEmpty(pNBLQueue))
    {
        pNBLQueue->Head = pQueueEntry;
    }
    else
    {
        //
        // If not empty, then there must be a tail. Add the NBL to next of Tail
        //
        pNBLQueue->Tail->Next = pQueueEntry;
    }
    
    //
    // Make the NBL the new tail
    //
    pNBLQueue->Tail = pQueueEntry;
    pNBLQueue->Count++;
}


INLINE VOID
MpQueueNBLPriority(
    PMP_NBL_QUEUE       pNBLQueue,
    PNET_BUFFER_LIST    NetBufferList
    )
{
    PQUEUE_ENTRY pQueueEntry;
    MP_VERIFY_QUEUE(pNBLQueue);

    pQueueEntry = MP_GET_QUEUE_ENTRY_FROM_NBL(NetBufferList);
    pQueueEntry->Next = pNBLQueue->Head;
    pNBLQueue->Head = pQueueEntry;
    if (pNBLQueue->Tail == NULL)
        pNBLQueue->Tail = pQueueEntry;
    pNBLQueue->Count++;
}


/**
 * Removes a NBL from the head of the Queue. Call to make sure that NBLQueue is
 * not empty before attempting to dequeue a NBL.
 * 
 * \param pNBLQueue         The Queue to insert the NBL in
 * \return The NBL at the head.
 */
INLINE PNET_BUFFER_LIST
MpDequeueNBL(
    IN  PMP_NBL_QUEUE   pNBLQueue
    )
{
    PQUEUE_ENTRY pNext, pHead;
    MP_VERIFY_QUEUE(pNBLQueue);
    
    MPASSERTMSG("Attempt to DeQueue from an Empty Packet Queue!\n", pNBLQueue->Head);
    
    pHead = pNBLQueue->Head;
    pNext = pNBLQueue->Head->Next;
    pNBLQueue->Head = pNext;
    if (pNext == NULL)
    {
        //
        // The queue is now emty as a result of dequeue.
        // Reset some variables
        //
        pNBLQueue->Tail = NULL;
    }
    
    pNBLQueue->Count--;

    //
    // We want to return only One NBL to the caller. Since NBL are chained
    // together by the Next Pointer, we should set it to NULL.
    //
    pHead->Next = NULL;

    return MP_GET_NBL_FROM_QUEUE_ENTRY(pHead);
}

#endif  // _NATIVE_WIFI_PACKET_QUEUE

