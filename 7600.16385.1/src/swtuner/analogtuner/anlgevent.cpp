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
/**************************************************************************
File    : anlgevent.cpp 
Abstract: Contains devent handling mechanisms and worker/system thread implemntations.
**************************************************************************/

#include "common.h"
PEventHandlerData EventHandler = NULL;
extern BOOLEAN g_bHardwareAssistedScanning;

VOID TunerScanWorkItem(IN PDEVICE_OBJECT DeviceObject,IN PVOID Context);
VOID ScanThread(IN PVOID Context);
VOID StopPollingThread();
void TimerDpcInterrupt(IN PKDPC Dpc, IN PEventHandlerData EventBlock,IN PVOID SystemArg1,IN PVOID SystemArg2);

PCWSTR MpegFileName= L"\\??\\C:\\mpeg\\rio.mpg";

DWORD DetermineIfFineTuning(IN DWORD dwFreq)
{
    UNREFERENCED_PARAMETER(dwFreq); 
    return FALSE;
}


NTSTATUS 
GetStreamLocationFromFrequency(IN CEncoderDevice* EncDevice, IN DWORD dwFreq, OUT PUNICODE_STRING FileName)
{
	// 1) Convert dword frequency to string.
	// 2) find matching key in registry
	// 3) read StreamLocation value from registry.
	// 4) pass key back to caller.

	NTSTATUS Status = STATUS_SUCCESS;
	UNICODE_STRING KeyName;
		
    if (!EncDevice)
	{
        return STATUS_UNSUCCESSFUL;
	}
	
    dwFreq /= 1000; //registry frequencies are in khz

    WCHAR *pczDeviceInstanceID = EncDevice->GetDeviceInstanceID();
    
    KeyName.Buffer = (LPWSTR)ExAllocatePoolWithTag(NonPagedPool,
        MAX_PATH+1, 
        MS_SAMPLE_ANALOG_POOL_TAG
        );
	KeyName.Length = MAX_PATH;
	KeyName.MaximumLength = MAX_PATH;

	UNICODE_STRING StubName;
    RtlInitUnicodeString(&StubName, L"\\Registry\\Machine\\System\\PSWTuner\\");
    UNICODE_STRING Seperator;
    RtlInitUnicodeString(&Seperator, L"\\");
    UNICODE_STRING Parameters;
    RtlInitUnicodeString(&Parameters, L"Device Parameters\\");
    UNICODE_STRING DevInstanceID;
    RtlInitUnicodeString(&DevInstanceID, pczDeviceInstanceID);


    RtlCopyUnicodeString(&KeyName,&StubName);
    RtlAppendUnicodeStringToString(&KeyName, &DevInstanceID);
    RtlAppendUnicodeStringToString(&KeyName, &Seperator);
    RtlAppendUnicodeStringToString(&KeyName, &Parameters);

	UNICODE_STRING Freq;
	Freq.Buffer = (LPWSTR)ExAllocatePoolWithTag(NonPagedPool,
		MAX_PATH+1, 
		MS_SAMPLE_ANALOG_POOL_TAG
		);

	if (NULL == Freq.Buffer)
	{
        ExFreePoolWithTag(KeyName.Buffer, MS_SAMPLE_ANALOG_POOL_TAG);
        return STATUS_UNSUCCESSFUL;
	}

	Freq.Length = MAX_PATH;
	Freq.MaximumLength = MAX_PATH;

	RtlZeroMemory(Freq.Buffer,  MAX_PATH+1);

	if (STATUS_SUCCESS != RtlIntegerToUnicodeString(dwFreq, 10, &Freq))
	{
        ExFreePoolWithTag(KeyName.Buffer, MS_SAMPLE_ANALOG_POOL_TAG);
        ExFreePoolWithTag(Freq.Buffer, MS_SAMPLE_ANALOG_POOL_TAG);
        
        return STATUS_UNSUCCESSFUL;
	}

    DbgPrint("Converted Frequency value %S\n", Freq.Buffer);

	if (STATUS_SUCCESS != RtlAppendUnicodeStringToString(&KeyName, &Freq))
	{
        ExFreePoolWithTag(KeyName.Buffer, MS_SAMPLE_ANALOG_POOL_TAG);
        ExFreePoolWithTag(Freq.Buffer, MS_SAMPLE_ANALOG_POOL_TAG);
        
        return STATUS_UNSUCCESSFUL;
	}


	//    UNICODE_STRING StreamLocation;
	RtlZeroMemory(&(EncDevice->StreamLocation), sizeof(EncDevice->StreamLocation));

	Status = PptRegGetSz(RTL_REGISTRY_ABSOLUTE, 
                        KeyName.Buffer,
                        L"StreamLocation", 
                        &(EncDevice->StreamLocation));

    if (!NT_SUCCESS(Status))
    {
        DetermineIfFineTuning(dwFreq);

        if (Freq.Buffer)
        {
            ExFreePoolWithTag(Freq.Buffer, MS_SAMPLE_ANALOG_POOL_TAG);
        }
    
        if (KeyName.Buffer)
        {
            ExFreePoolWithTag(KeyName.Buffer, MS_SAMPLE_ANALOG_POOL_TAG);
        }
        return Status;
    }

	RtlInitUnicodeString(FileName, EncDevice->StreamLocation.Buffer);

	if (Freq.Buffer)
	{
        ExFreePoolWithTag(Freq.Buffer, MS_SAMPLE_ANALOG_POOL_TAG);
	}
	
	if (KeyName.Buffer)
	{
        ExFreePoolWithTag(KeyName.Buffer, MS_SAMPLE_ANALOG_POOL_TAG);
	}

	return Status;
}

void TimerDpcInterrupt(IN PKDPC Dpc, IN PEventHandlerData EventBlock,IN PVOID SystemArg1,IN PVOID SystemArg2)
{

    UNREFERENCED_PARAMETER(Dpc); 
    UNREFERENCED_PARAMETER(SystemArg1); 
    UNREFERENCED_PARAMETER(SystemArg2); 
    if (!EventBlock) return;

	if (!EventBlock->bStopScan) {
		//      CEncoderDevice* EncDevice = EventBlock->pDevice;
		//
		// Reschedule the timer for the next interrupt time.
		//
		if(EventBlock->CurrentFrequency <= EventBlock->ScanData.EndFrequency)
		{
			KeSetEvent(&(EventBlock->InitEvent), 0,FALSE);
		}
		else
		{
			KeAcquireSpinLockAtDpcLevel(&(EventHandler->LockAccess));
			EventBlock->bStopScan = TRUE;
			EventBlock->CurrentFrequency = EventBlock->ScanData.EndFrequency;
			KeReleaseSpinLockFromDpcLevel(&(EventHandler->LockAccess));
			KeSetEvent(&(EventBlock->ThreadEvent), 0,FALSE);            
		}

	} else {
		//
		// If someone is waiting on the hardware to stop, raise the stop
		// event and clear the flag.
		//
		KeSetEvent(&(EventBlock->ThreadEvent), 0,FALSE);
	}
}

NTSTATUS InitializeEventing(CEncoderDevice* pEncDevice)
{
	//  First, allocate buffers
	NTSTATUS ntStatus = STATUS_SUCCESS;
	if(!EventHandler)
	{
		EventHandler = (PEventHandlerData)ExAllocatePoolWithTag(NonPagedPool,
			sizeof(EventHandlerData), MS_SAMPLE_ANALOG_POOL_TAG);
		if (!EventHandler) {
			ntStatus = STATUS_UNSUCCESSFUL;
			return ntStatus;
		}
		pEncDevice->EventData = reinterpret_cast<PVOID>(EventHandler);
		KeInitializeEvent(&(EventHandler->InitEvent),
			SynchronizationEvent,
			FALSE);

		KeInitializeEvent(&(EventHandler->TuneEvent),
			SynchronizationEvent,
			FALSE);

		KeInitializeEvent(&(EventHandler->ThreadEvent),
			SynchronizationEvent,
			FALSE);
		KeInitializeSpinLock(&(EventHandler->LockAccess));
		KeInitializeDpc (&(EventHandler->DPCObject), reinterpret_cast <PKDEFERRED_ROUTINE>(TimerDpcInterrupt), EventHandler);
		KeInitializeTimerEx(&(EventHandler->timer), SynchronizationTimer);
	}
	KeClearEvent(&(EventHandler->InitEvent));
	KeClearEvent(&(EventHandler->TuneEvent));
	KeClearEvent(&(EventHandler->ThreadEvent));
	return ntStatus;
}

NTSTATUS 
AnlgTunerScanAddEvent(
					  PIRP pIrp,
					  PKSEVENTDATA EventData,
					  PKSEVENT_ENTRY pEventEntry
					  )
{
	KIRQL OldIrql;
	NTSTATUS ntStatus = STATUS_SUCCESS;
	PKSEVENT_TUNER_INITIATE_SCAN_S ScanRequest = reinterpret_cast<PKSEVENT_TUNER_INITIATE_SCAN_S>(EventData);
	PEventHandlerData EventBlock;

	ASSERT(pIrp);
	PKSFILTER pFilter = KsGetFilterFromIrp(pIrp);
	if (!pFilter)
	{
		ntStatus = STATUS_UNSUCCESSFUL;
		goto error;
	}

	PKSDEVICE pDevice = KsFilterGetDevice(pFilter);
	if (!pDevice)
	{
		ntStatus = STATUS_UNSUCCESSFUL;
		goto error;
	}
    if (g_bHardwareAssistedScanning == FALSE)
    {
        return STATUS_UNSUCCESSFUL; 
    }

	CEncoderDevice* pEncDevice = reinterpret_cast<CEncoderDevice *>(pDevice->Context);
	if (!pEncDevice )
	{
		ntStatus = STATUS_UNSUCCESSFUL;
		goto error;
	}

	DbgPrint("AnlgTunerScanAddEvent enter!begin frenquency is %d,end frenquency is %d\n",ScanRequest -> StartFrequency,ScanRequest -> EndFrequency);
	if(ScanRequest -> StartFrequency == ScanRequest -> EndFrequency )
	{
		if(pEncDevice->ScanInitialization)
		{
			ntStatus = STATUS_UNSUCCESSFUL;
			goto error;
		}
		// go ahead and tune to the frequency;
		ntStatus = InitializeEventing(pEncDevice);
		if (!NT_SUCCESS(ntStatus))
			goto error;
		EventBlock = EventHandler;

		ntStatus = GetStreamLocationFromFrequency(pEncDevice, ScanRequest -> StartFrequency, &pEncDevice->FileName);
		if(ntStatus != STATUS_SUCCESS)                // Found the frequency in the registry...so Just tune to it.
		{
			ULONG dwFreq = ( ScanRequest -> StartFrequency / (1000 * 1000) ); // fine tune algorithm ..
			dwFreq *= 1000000;

			ntStatus = GetStreamLocationFromFrequency(pEncDevice, dwFreq, &pEncDevice->FileName);
			if(ntStatus != STATUS_SUCCESS)
			{
				pEncDevice->CurrentFrequency = EventBlock->CurrentFrequency = ScanRequest -> StartFrequency;
				pEncDevice->ScanStatusCode = EventBlock->StatusCode = Tuner_LockType_None;
				KsGenerateEvent(pEventEntry);
				ntStatus = STATUS_SUCCESS;
				goto error;
			}
		}
		KeClearEvent(&(EventBlock->TuneEvent));
		EventBlock->IdleTimeout.QuadPart = pEncDevice->SettlingTime / 2;
		EventBlock->IdleTimeout.QuadPart = EventBlock->IdleTimeout.QuadPart * 1000 * 10; // in 100 nanasecond units

		KeWaitForSingleObject(&(EventBlock->TuneEvent),
			Executive,
			KernelMode,
			FALSE,
			&(EventBlock->IdleTimeout));     // in 100 nano second units

		pEncDevice->CurrentFrequency = EventBlock->CurrentFrequency = ScanRequest -> StartFrequency;
		pEncDevice->SetFrequency(pEncDevice->CurrentFrequency);

		pEncDevice->ScanStatusCode = EventBlock->StatusCode = Tuner_LockType_Locked;
		KsGenerateEvent(pEventEntry);
		ntStatus = STATUS_SUCCESS;
		goto error;

	}
	else
	{
		if (pEncDevice->pScanEvent)
		{
			// only one scan operation allowed on a tuner at a time
			if(EventHandler->ScanData.EventData.EventHandle.Event != ScanRequest->EventData.EventHandle.Event)
			{
			    DbgPrint("error:EventHandler->ScanData.EventData.EventHandle.Event != ScanRequest->EventData.EventHandle.Event\n");
				ntStatus = STATUS_UNSUCCESSFUL;
				goto error;
			}
			else
			{
				if(pEncDevice->ScanInitialization)
				{
					KeAcquireSpinLock(&pEncDevice->EventHandlerSpinLock, &OldIrql);

					pEncDevice->pScanEvent = pEventEntry;
					EventHandler->EventEntry = pEventEntry; // Need to remove one of the references here.
					memcpy(&(EventHandler->ScanData),ScanRequest, sizeof(KSEVENT_TUNER_INITIATE_SCAN_S));
					pEncDevice->CurrentFrequency = EventHandler->CurrentFrequency = ScanRequest->StartFrequency;
					pEncDevice->ScanStatusCode = EventHandler->StatusCode = Tuner_LockType_None;
					KeReleaseSpinLock(&pEncDevice->EventHandlerSpinLock, OldIrql);
					LARGE_INTEGER   Timeout;
					Timeout.QuadPart = (-1) * EventHandler->IdleTimeout.QuadPart;
					KeSetTimerEx (&(EventHandler->timer), Timeout, NULL, &(EventHandler->DPCObject));
					DbgPrint("AnlgTunerScanAddEvent restart scan at Frequency %d",EventHandler->CurrentFrequency);
					ntStatus = STATUS_SUCCESS;
					goto error;
				}
			}
		}
		else
		{
			KeAcquireSpinLock(&pEncDevice->EventHandlerSpinLock, &OldIrql);
			pEncDevice->pScanEvent = pEventEntry;
			KeReleaseSpinLock(&pEncDevice->EventHandlerSpinLock, OldIrql);
			ULONG ulTimeout=0;
			LARGE_INTEGER   WaitForThread;
			WaitForThread.QuadPart = -10000;
			while (EventHandler != NULL)
			{
			    DbgPrint("AnlgTunerScanAddEvent waiting for previous scan to end");
			    ulTimeout++;
			    KeWaitForSingleObject(pEncDevice->pScanEvent, Executive, KernelMode, FALSE, &WaitForThread);//wait for the thread to end
			    if (ulTimeout > 1000)//one second
			    {
			        ntStatus = STATUS_UNSUCCESSFUL;
			        goto error;
			    }
			}
		}
        if (!NT_SUCCESS(ntStatus)) goto error;
            
        // use a separate thread to do the actual scan.
        PEventHandlerData EventBlock;
        NTSTATUS     WaitStatus;

        ntStatus = InitializeEventing(pEncDevice);
        if ((!NT_SUCCESS(ntStatus)) || (EventHandler == NULL))
            goto error;

        EventBlock = EventHandler;

        KeAcquireSpinLock(&(EventBlock->LockAccess), &OldIrql);
        memcpy(&(EventBlock->ScanData),ScanRequest, sizeof(KSEVENT_TUNER_INITIATE_SCAN_S));
        pEncDevice->CurrentFrequency = EventHandler->CurrentFrequency = ScanRequest->StartFrequency;
        if(EventBlock->ScanData.EndFrequency < EventBlock->ScanData.StartFrequency)
        {
            ULONG temp;
            temp = EventBlock->ScanData.EndFrequency;
            EventBlock->ScanData.EndFrequency= EventBlock->ScanData.StartFrequency;
            EventBlock->ScanData.StartFrequency = temp;
        }
        pEncDevice->FreqRange = ScanRequest->EndFrequency - ScanRequest->StartFrequency;
        EventBlock->EventEntry = pEventEntry;
        EventBlock->IdleTimeout.QuadPart = pEncDevice->SettlingTime * 1000 * 10; // in 100 nanosecond units;
        EventBlock->bStopScan = FALSE;
        EventBlock->pDevice = pEncDevice;
        pEncDevice->ScanStatusCode = EventBlock->StatusCode = Tuner_LockType_None;
        KeReleaseSpinLock(&(EventBlock->LockAccess), OldIrql);

        // worker thread item for reads
        EventBlock->ThreadItem=IoAllocateWorkItem(pDevice->PhysicalDeviceObject);

        IoQueueWorkItem(EventBlock->ThreadItem,
            TunerScanWorkItem,
            CriticalWorkQueue,
            EventBlock);

        pEncDevice->ScanInitialization = TRUE;

        WaitStatus = KeWaitForSingleObject(&(EventBlock->InitEvent),
            Executive,
            KernelMode,
            FALSE,
            NULL);

        if (!NT_SUCCESS(WaitStatus)) {
            ntStatus = STATUS_UNSUCCESSFUL;
            goto error;
        }
            
    }
	// remove handler wont be called for this particular pEventEntry
	// till KsDefaultAddEventHandler returns. So pEventEntry is "safe"
	ntStatus = KsDefaultAddEventHandler(pIrp, reinterpret_cast<PKSEVENTDATA>(ScanRequest), pEventEntry);

error:
	DbgPrint("AnlgTunerScanAddEvent return -- code is %d",ntStatus);
	pIrp->IoStatus.Status = ntStatus;
	return ntStatus;
}

VOID 
AnlgTunerScanRemoveEvent(
                        PFILE_OBJECT  FileObject,
                        struct _KSEVENT_ENTRY*  EventEntry
	)
{
    KIRQL OldIrql;
    CEncoderDevice* pEncDevice = NULL;

    DbgPrint("AnlgTunerScanRemoveEvent enter!\n");
    PKSFILTER Filter = KsGetFilterFromFileObject(FileObject);
    ASSERT(Filter);
    if (!Filter) return;
    ASSERT(EventEntry);

    PKSDEVICE pDevice = KsFilterGetDevice(Filter);
    pEncDevice = reinterpret_cast<CEncoderDevice *>(pDevice->Context);

    KeAcquireSpinLock(&pEncDevice->EventHandlerSpinLock, &OldIrql);

    if(pEncDevice->ScanInitialization)
		RemoveEntryList(&EventEntry->ListEntry);
    if (pEncDevice->pScanEvent) 
    {

        pEncDevice->pScanEvent = NULL;
        pEncDevice->EventData = NULL;
        pEncDevice->ScanInitialization = FALSE;
        if (EventHandler)//if not already stopped due to the end of the range stop it
        {
            EventHandler->bStopScan = TRUE;
            KeSetEvent(&(EventHandler->TuneEvent), 0,FALSE);
        }
    }
    KeReleaseSpinLock(&pEncDevice->EventHandlerSpinLock, OldIrql);
	return;

}

VOID TunerScanWorkItem(IN PDEVICE_OBJECT DeviceObject,IN PVOID Context)
{
    HANDLE           handle;
    NTSTATUS          WaitStatus;

    UNREFERENCED_PARAMETER(DeviceObject); 

    PEventHandlerData EventBlock= static_cast<PEventHandlerData>( Context);

    DbgPrint("MY-Event::TunerScanWorkItem");

    OBJECT_ATTRIBUTES oa;
    InitializeObjectAttributes(&oa, NULL, OBJ_KERNEL_HANDLE, NULL, NULL);

    if (STATUS_SUCCESS != PsCreateSystemThread(&handle, THREAD_ALL_ACCESS, &oa, NULL, NULL, ScanThread, EventBlock))
    {
        return;
    }

    ObReferenceObjectByHandle(handle, THREAD_ALL_ACCESS, NULL,
        KernelMode, (PVOID*) &EventBlock->thread, NULL);

    KeSetEvent(&(EventBlock->InitEvent), 0,FALSE);
    ZwClose(handle);
    do
    {
        WaitStatus = KeWaitForSingleObject(&(EventBlock->TuneEvent),
	        Executive,
	        KernelMode,
	        FALSE,
	        NULL);
        if (!NT_SUCCESS(WaitStatus)) {
	        break;
        }
        if(EventBlock->bStopScan)
        {
	        StopPollingThread();
	        KsGenerateEvent(EventBlock->EventEntry);//fire an Event to the app that we are stopped
	        break;
        }
        CEncoderDevice* EncDevice = EventBlock->pDevice;
        EncDevice->CurrentFrequency = EventBlock->CurrentFrequency;

        KeClearEvent(&(EventBlock->TuneEvent));
        DbgPrint( "MY-TunerScanWorkItem : Firing an event from driver to the app\n");
        KsGenerateEvent(EventBlock->EventEntry);

#pragma warning(push)
#pragma warning(disable:4127)
    }while(1);
#pragma warning (pop)
    DbgPrint( "MY-TunerScanWorkItem terminating\n");
    IoFreeWorkItem(EventHandler->ThreadItem); // Release Work Item
    EventHandler->EventEntry = NULL;
    ExFreePoolWithTag(EventHandler, MS_SAMPLE_ANALOG_POOL_TAG);
    EventHandler = NULL;
}

VOID
ScanThread(
		   IN PVOID Context
		   )
{
	PEventHandlerData EventBlock = static_cast<PEventHandlerData>( Context);
	LARGE_INTEGER   Timeout;
	NTSTATUS        WaitStatus, Status;
	PVOID           eventArray[2];
    KIRQL           OldIrql;
	CEncoderDevice* EncDevice = EventBlock->pDevice;

	eventArray[0] = &(EventBlock->InitEvent);
	eventArray[1] = &(EventBlock->ThreadEvent);

	Timeout.QuadPart = (-1) * EventBlock->IdleTimeout.QuadPart;
	KeSetTimerEx (&(EventBlock->timer), Timeout,NULL, &(EventBlock->DPCObject));
	do
	{
		WaitStatus = KeWaitForMultipleObjects( 2, 
			eventArray,
			WaitAny,
			Executive,
			KernelMode,
			FALSE, 
			NULL,
			NULL);
		if(WaitStatus == STATUS_TIMEOUT )
		{
			KeAcquireSpinLock(&(EventBlock->LockAccess), &OldIrql);
			EncDevice->ScanStatusCode = EventBlock->StatusCode = Tuner_LockType_None;
			KeReleaseSpinLock(&(EventBlock->LockAccess), OldIrql);
			KeSetEvent(&(EventBlock->TuneEvent), 0,FALSE);
			continue;
		}
		else if(WaitStatus == STATUS_WAIT_0 )
		{
			Status = GetStreamLocationFromFrequency(EncDevice, EventBlock->CurrentFrequency, &EncDevice->FileName);
			KeClearEvent((PKEVENT)eventArray[0]);
			if(Status == STATUS_SUCCESS)        
			{
				KeAcquireSpinLock(&(EventBlock->LockAccess), &OldIrql);
				EncDevice->ScanStatusCode = EventBlock->StatusCode = Tuner_LockType_Locked;
				KeReleaseSpinLock(&(EventBlock->LockAccess), OldIrql);
				if (KeCancelTimer(&(EventBlock->timer)) == FALSE)
				{
				    DbgPrint( "MY-Tuner ScanThread no timer active\n");
				}
				KeSetEvent(&(EventBlock->TuneEvent), 0,FALSE);  
			}
			else
			{
				KeAcquireSpinLock(&(EventBlock->LockAccess), &OldIrql);
				EventBlock->CurrentFrequency +=  EncDevice->ScanSensingRange;
				EncDevice->CurrentFrequency = EventBlock->CurrentFrequency;
				EncDevice->ScanStatusCode = EventBlock->StatusCode = Tuner_LockType_None;
				KeReleaseSpinLock(&(EventBlock->LockAccess), OldIrql);
				LARGE_INTEGER NextTime;
				NextTime.QuadPart = (-1) * EventBlock->IdleTimeout.QuadPart;
				KeSetTimerEx(&(EventBlock->timer), NextTime, NULL, &(EventBlock->DPCObject));
			}
			continue;
		}
		else if(WaitStatus == STATUS_WAIT_1)
		{
			KeAcquireSpinLock(&(EventBlock->LockAccess), &OldIrql);
			if (KeCancelTimer(&(EventBlock->timer)) == FALSE)
			{
			    DbgPrint( "MY-Tuner ScanThread no timer active at terminate\n");
			}
			EncDevice->ScanStatusCode = EventBlock->StatusCode = Tuner_LockType_None;
			DbgPrint( "MY-Tuner terminate ScanThread \n");
			KeReleaseSpinLock(&(EventBlock->LockAccess), OldIrql);

			KeClearEvent((PKEVENT)eventArray[1]);
			KeSetEvent(&(EventBlock->TuneEvent), 0,FALSE);  
			PsTerminateSystemThread(STATUS_SUCCESS);
		}
#pragma warning( push )
#pragma warning(disable:4127)
       }while(1);
#pragma warning (pop)
}

VOID StopPollingThread()
{                           
	if(EventHandler)      // need to check..it is possible and seen that this gets called even after the cleanup is done and RemoveEvent has been called.
	{
		if(EventHandler->thread)
		{
			KeSetEvent(&EventHandler->ThreadEvent, 0, FALSE);
			KeWaitForSingleObject(EventHandler->thread, Executive, KernelMode, FALSE, NULL);
			ObDereferenceObject(EventHandler->thread);
			EventHandler->thread = NULL;
		}
	}
}

