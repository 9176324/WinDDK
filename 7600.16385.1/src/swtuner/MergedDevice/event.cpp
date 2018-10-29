//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.
//
//==========================================================================;

#include "BDATuner.h"

#define MS_SAMPLE_TUNER_EVENT_POOL_TAG 'EadB'

VOID
EventManager::ReleaseAllEventsWithoutLock()
{
    CFilter* pFilter = reinterpret_cast<CFilter *>(m_pContext);
    KIRQL Irql;

    KeAcquireSpinLock(&pFilter->m_Lock, &Irql);
    ReleaseAllEvents();
    KeReleaseSpinLock(&pFilter->m_Lock, Irql);


}


VOID
EventManager::ReleaseAllEvents()
{
    PBDATUNER_EVENT pTunerEvent;
    PLIST_ENTRY pEntry;
    
    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);
    //remove all pending events
    ReleaseAllUnsignalledEvents();

    //remove all Signalled Events
    while (!IsListEmpty(&m_EventListSignalledHead))
    {
        pEntry = RemoveHeadList(&m_EventListSignalledHead);
        pTunerEvent = CONTAINING_RECORD(pEntry,BDATUNER_EVENT, ListEntry);
        ReleaseEvent(pTunerEvent, STATUS_SUCCESS);
    }

}


VOID
EventManager::Enable()
{
    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);
    m_bEnabled = TRUE;

}

VOID
EventManager::Disable()
{
    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);
    m_bEnabled = FALSE;
}


BOOL
EventManager::IsEnabled()
{
    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);
    return (m_bEnabled == TRUE);
}

VOID
EventManager::ReleaseAllUnsignalledEvents()
{
    LIST_ENTRY FreeListHead;
    PBDATUNER_EVENT pTunerEvent;
    PLIST_ENTRY pEntry;

    InitializeListHead(&FreeListHead);
    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);
    ASSERT(m_bEnabled == FALSE);
    if (m_bEnabled == TRUE)
    {
        Disable();
    }

    while (!IsListEmpty(&m_EventListPendingHead))
    {
        pEntry = RemoveHeadList(&m_EventListPendingHead);
        pTunerEvent = CONTAINING_RECORD(pEntry,BDATUNER_EVENT, ListEntry);
        InsertTailList(&FreeListHead, pEntry);
    }    
    //
    // Now free all the events in the free list. 
    //

    while (!IsListEmpty(&FreeListHead))
    {
        pEntry = RemoveHeadList(&FreeListHead);
        pTunerEvent = CONTAINING_RECORD(pEntry,BDATUNER_EVENT, ListEntry);
        ReleaseEvent(pTunerEvent, STATUS_SUCCESS);
    }

}

PLIST_ENTRY
EventManager::NextEvent(PLIST_ENTRY pEntry)
{
    PLIST_ENTRY pNextEntry;
    pNextEntry = pEntry->Flink;
    if (pNextEntry == &m_EventListPendingHead)
    {
        pNextEntry = NULL;
    }
    if (pNextEntry == &m_EventListSignalledHead)
    {
        pNextEntry = NULL;
    }

    return pNextEntry;
}

NTSTATUS 
EventManager::ReadCurrentEvent(
    PBDATUNER_EVENT *ppEvent
    )
{
    NTSTATUS Status = STATUS_SUCCESS;
    PBDATUNER_EVENT pEvent = NULL;
    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);
    if (IsListEmpty(&m_EventListPendingHead))
    {       
        Status = STATUS_NO_MORE_ENTRIES;
        goto exit;
    }

    PLIST_ENTRY pEntry = NULL;

    pEntry = m_EventListPendingHead.Flink;

    pEvent = CONTAINING_RECORD(pEntry,BDATUNER_EVENT,ListEntry);
    
  exit:
    *ppEvent = pEvent;
    return Status;
}


NTSTATUS 
EventManager::GetCurrentEvent(
    PBDATUNER_EVENT *ppEvent
    )
{
    NTSTATUS Status = STATUS_NO_MORE_ENTRIES;
    PBDATUNER_EVENT pEvent = NULL;
    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);

    if (IsListEmpty(&m_EventListSignalledHead))
    {       
        Status = STATUS_NO_MORE_ENTRIES;
        goto exit;
    }

    PLIST_ENTRY pEntry = NULL;

    pEntry = m_EventListSignalledHead.Flink;

    while (pEntry != NULL) 
    {
        pEvent = CONTAINING_RECORD(pEntry,BDATUNER_EVENT, ListEntry);                
        if (pEvent->ulEventId == INVALID_TUNER_EVENT_ID)
        {
            Status = STATUS_SUCCESS;
            break;
        }
        pEntry = NextEvent(pEntry);
    }
    
  exit:
    *ppEvent = pEvent;
    return Status;
}


NTSTATUS 
EventManager::QueueEvent(
        PBDATUNER_EVENT pEvent,
        BOOL fNotifyIfPossible //if Event only queued set false, if target to be signaled TRUE
        )
{
    NTSTATUS Status = STATUS_SUCCESS;
    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);

    if((m_pContext ==  NULL) || (m_bEnabled == FALSE))
    {
        //
        // The event has not been enabled yet. Simply fail queueing the event.
        // The caller must complete it internally.
        //
        Status = STATUS_UNSUCCESSFUL;
        DbgPrint("EventManager::QueueEvent  this %p failed\n", this);
        goto exit;
    }

    InsertTailList(&m_EventListPendingHead, &pEvent->ListEntry);
    DbgPrint("EventManager::QueueEvent  Queued this %p\n", this);

    if (fNotifyIfPossible == TRUE)
    {
        //
        // All events in the queue have already been signalled and are waiting for completion.
        // Therefore we must notify the owning object (CFilter) to fire a pending event.
        //
        DbgPrint("EventManager::QueueEvent Firing Event this %p\n", this);
        Status = NotifyEvent();
        m_fEventSignalled = TRUE;
    }
    if (!NT_SUCCESS(Status))
    {
        RemoveEntryList(&pEvent->ListEntry);
    }
  exit:        
    return Status;
}


VOID
EventManager::MoveEventToSignalledList(
    PBDATUNER_EVENT pEvent
    )
{
    RemoveEntryList(&pEvent->ListEntry);
    InsertTailList(&m_EventListSignalledHead, &pEvent->ListEntry);
}


VOID
EventManager::ReleaseEvent(
    PBDATUNER_EVENT pTunerEvent,
    ULONG CompletionStatus
    )
{
    UNREFERENCED_PARAMETER(CompletionStatus);
    FreeTunerEvent(pTunerEvent);
}


NTSTATUS 
EventManager::Initialize(
    PVOID pContext
    )
{
    m_pContext = pContext;
    return STATUS_SUCCESS;
}

VOID
EventManager::UnInitialize()
{
    
    ReleaseAllEventsWithoutLock();
    m_pContext = NULL;
}


VOID
EventManager::CompleteEvent(
    PBDATUNER_EVENT pEventToComplete, 
    ULONG ulEventIdToComplete,
    HRESULT hrComplete
    )
{
    UNREFERENCED_PARAMETER(pEventToComplete);
    UNREFERENCED_PARAMETER(ulEventIdToComplete);

    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);
    PLIST_ENTRY  pEntry = NULL;
    PBDATUNER_EVENT pMatchingEvent = NULL;
    PBDATUNER_EVENT pTunerEvent = NULL;
    BOOL bEventMatch = FALSE;

    if (!IsListEmpty(&m_EventListSignalledHead))
    {    
        pEntry = m_EventListSignalledHead.Flink;

        while (pEntry != NULL) 
        {
            pTunerEvent = CONTAINING_RECORD(pEntry,BDATUNER_EVENT, ListEntry);                
            bEventMatch = AreEventsEqual(pTunerEvent, pEventToComplete, ulEventIdToComplete);
            if (bEventMatch == TRUE)
            {
                RemoveEntryList(pEntry);
                pMatchingEvent = pTunerEvent;
                break;
            }
            pEntry = NextEvent(pEntry);
        }
        if (pMatchingEvent != NULL)
        {
            ReleaseEvent(pMatchingEvent, hrComplete);
            return;
        } 
    }

    // if the Event cannot be found in the Signalled List we complete it from the Pending list
    //derived classes typically do not signal the event and just keep a internal list
    if (!IsListEmpty(&m_EventListPendingHead))
    {    
        pEntry = m_EventListPendingHead.Flink;

        while (pEntry != NULL) 
        {
            pTunerEvent = CONTAINING_RECORD(pEntry,BDATUNER_EVENT, ListEntry);                
            bEventMatch = AreEventsEqual(pTunerEvent, pEventToComplete, ulEventIdToComplete);
            if (bEventMatch == TRUE)
            {
                RemoveEntryList(pEntry);
                pMatchingEvent = pTunerEvent;
                break;
            }
            pEntry = NextEvent(pEntry);
        }
        if (pMatchingEvent != NULL)
        {
            ReleaseEvent(pMatchingEvent, hrComplete);
        } 
    }

    if (IsListEmpty(&m_EventListPendingHead))
    {
        m_fEventSignalled = FALSE;
    }
}


FilterEventManager::FilterEventManager()
{
    InitializeListHead(&m_EventListPendingHead);
    InitializeListHead(&m_EventListSignalledHead);
    m_pContext = NULL;
    m_fEventSignalled = FALSE;
}


FilterEventManager::~FilterEventManager()
{
    UnInitialize();
}

NTSTATUS
FilterEventManager::NotifyEvent()
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PBDATUNER_EVENT pTunerEvent = NULL;
    CFilter* pFilter = reinterpret_cast<CFilter *>(m_pContext);
    ASSERT (IsEnabled() == TRUE);
    //
    // The Filter Event Manager must send a pending event to the bda 
    // plugin to start draining events. Do it here
    //
    ntStatus = ReadCurrentEvent(&pTunerEvent);
    if (ntStatus == STATUS_SUCCESS)
    {
        MoveEventToSignalledList(pTunerEvent);
        pFilter->SignalPendingEvent();
    }
    return STATUS_SUCCESS;
}

BOOL
FilterEventManager::AreEventsEqual(
    PBDATUNER_EVENT pEvent,
    PBDATUNER_EVENT pEventToComplete,
    ULONG ulEventIdToComplete
    )
{
    UNREFERENCED_PARAMETER(pEvent);
    UNREFERENCED_PARAMETER(pEventToComplete);
    
    //
    // The Filter Events only need to be compared on the basis of their event ids.
    //
    return (pEvent->ulEventId == ulEventIdToComplete);

}







FilterDebugEventsManager::FilterDebugEventsManager()
{
    InitializeListHead(&m_EventListPendingHead);
    InitializeListHead(&m_EventListSignalledHead);
    m_pContext = NULL;
    m_ucDebugLevel = 0;
    m_fEventSignalled = FALSE;
}



FilterDebugEventsManager::~FilterDebugEventsManager()
{
    UnInitialize();
}



NTSTATUS
FilterDebugEventsManager::NotifyEvent()
{
    NTSTATUS Status = STATUS_UNSUCCESSFUL;
    CFilter* pFilter = reinterpret_cast<CFilter *>(m_pContext);

    PBDATUNER_EVENT pFilterEvent = (PBDATUNER_EVENT) AllocateTunerEvent(&BDA_DEBUG_DATA_AVAILABLE, 0, NULL);
    if ( pFilterEvent == NULL)
    {
        Status = STATUS_NO_MEMORY;
        goto errExit;
    }

    // next queue the Event into the Filter Event Manager and signal if possible
    if (m_fEventSignalled == FALSE)
    {
        Status = pFilter->m_FilterEventsManager.QueueEvent(pFilterEvent, true);
    }
    if (!NT_SUCCESS(Status))
    {
        goto errExit;
    }
    return Status;
  errExit:
  
    if (pFilterEvent)
    {
        FreeTunerEvent(pFilterEvent);
    }
    return Status;
}


BOOL
FilterDebugEventsManager::AreEventsEqual(
    PBDATUNER_EVENT pEvent,
    PBDATUNER_EVENT pEventToComplete,
    ULONG ulEventIdToComplete
    )
{
    ;
    //
    // The Debugs Events only need to be compared on the basis of their Event pointers.
    //
    UNREFERENCED_PARAMETER(ulEventIdToComplete);
    return (pEvent == pEventToComplete);

}


NTSTATUS
FilterDebugEventsManager::QueueDebugMessage(
    UCHAR ucLevel,
    ULONG ulLength,
    UCHAR* pucData
    )
{
    NTSTATUS Status = STATUS_UNSUCCESSFUL;

    if (ucLevel > m_ucDebugLevel)
    {
        Status = STATUS_UNSUCCESSFUL;
        goto exit;
    }
        
    
    PBDATUNER_EVENT Event = AllocateTunerEvent(
        &BDA_DEBUG_DATA_AVAILABLE, ulLength,pucData);

    if (Event == NULL)
    {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto exit;
    }
    
    QueueEvent(Event,true);
    Status = STATUS_SUCCESS;
exit:
    return Status;
}


//======================================================================================
//
//  FilterGPNVEventsManager handling the Event queue and signalling
//
//======================================================================================
FilterGPNVEventsManager::FilterGPNVEventsManager()
{
    InitializeListHead(&m_EventListPendingHead);
    InitializeListHead(&m_EventListSignalledHead);
    m_pContext = NULL;
    m_fEventSignalled = FALSE;
}



FilterGPNVEventsManager::~FilterGPNVEventsManager()
{
    UnInitialize();
}



NTSTATUS
FilterGPNVEventsManager::NotifyEvent()
{
    NTSTATUS Status = STATUS_UNSUCCESSFUL;
    CFilter* pFilter = reinterpret_cast<CFilter *>(m_pContext);

    PBDATUNER_EVENT pFilterEvent = (PBDATUNER_EVENT) AllocateTunerEvent(&EVENTID_BDA_GPNVValueUpdate, 0, NULL);
    if ( pFilterEvent == NULL)
    {
        Status = STATUS_NO_MEMORY;
        goto errExit;
    }

    // next queue the Event as copy into the Filter Event Manager and signal if possible
    if (m_fEventSignalled == FALSE)
    {
        Status = pFilter->m_FilterEventsManager.QueueEvent(pFilterEvent, true);
    }
    else
    {
        Status = STATUS_SUCCESS;
    }
    if (!NT_SUCCESS(Status))
    {
        goto errExit;
    }
    return Status;
errExit:

    if (pFilterEvent)
    {
        FreeTunerEvent(pFilterEvent);
    }
    return Status;
}


BOOL
FilterGPNVEventsManager::AreEventsEqual(
    PBDATUNER_EVENT pEvent,
    PBDATUNER_EVENT pEventToComplete,
    ULONG ulEventIdToComplete
    )
{
    ;
    //
    // The GPNV Events only need to be compared on the basis of their Event pointers.
    //
    UNREFERENCED_PARAMETER(ulEventIdToComplete);

    return (pEvent == pEventToComplete);

}


NTSTATUS
FilterGPNVEventsManager::QueueNameValue(
    ULONG ulLength,
    UCHAR* pubData,
    BOOL  fSignalEvent
    )
{
    NTSTATUS Status = STATUS_UNSUCCESSFUL;

    PBDATUNER_EVENT Event = AllocateTunerEvent(
        &EVENTID_BDA_GPNVValueUpdate, ulLength, pubData);

    if (Event == NULL)
    {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto exit;
    }

    QueueEvent(Event,fSignalEvent);
    Status = STATUS_SUCCESS;
exit:
    return Status;
}

//======================================================================================

PBDATUNER_EVENT
AllocateTunerEvent(
    const GUID* Guid,
    ULONG ulLength,
    UCHAR* pucData
    )
{
    PBDATUNER_EVENT pTunerEvent = 
        (PBDATUNER_EVENT) ExAllocatePoolWithTag(
        NonPagedPool, 
        sizeof(BDATUNER_EVENT)+ulLength, 
        MS_SAMPLE_TUNER_POOL_TAG);

    if (pTunerEvent == NULL)
    {
        return NULL;         
    }
    RtlZeroMemory(pTunerEvent, sizeof(BDATUNER_EVENT)+ulLength);

    pTunerEvent->uuidEvent = *Guid;
    pTunerEvent->ulEventId = INVALID_TUNER_EVENT_ID;
    memcpy(pTunerEvent->argbData, pucData, ulLength);
    pTunerEvent->ulDataLength = ulLength;
    return pTunerEvent;
}


VOID
FreeTunerEvent(
    PBDATUNER_EVENT pTunerEvent
    )
{
    ExFreePoolWithTag(pTunerEvent, MS_SAMPLE_TUNER_POOL_TAG);

}



