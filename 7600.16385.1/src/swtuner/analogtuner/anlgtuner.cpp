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
File    : anlgtuner.cpp 
Abstract: Contains dispatch routines, property handlers and intersect 
handlers for the tuner filter
**************************************************************************/

#include "anlgtuner.h"

BOOLEAN g_bHardwareAssistedScanning = FALSE;

NTSTATUS 
GetDWORDAttribute(IN WCHAR* pczDeviceInstanceID, IN __nullterminated LPWSTR AttributeName,  OUT DWORD& AttributeValue)
{
    // 1) Convert dword frequency to string.
    // 2) find matching key in registry
    // 3) read SignalQuality value from registry.
    // 4) pass key back to caller.

    NTSTATUS Status = STATUS_SUCCESS;
    UNICODE_STRING KeyName;

    UNREFERENCED_PARAMETER(pczDeviceInstanceID);

    KeyName.Buffer = (LPWSTR)ExAllocatePoolWithTag(NonPagedPool,
    	MAX_PATH+1, 
    	MS_SAMPLE_ANALOG_POOL_TAG
    	);

    if (NULL == KeyName.Buffer)
    {
        return STATUS_UNSUCCESSFUL;
    }

    KeyName.Length = MAX_PATH;
    KeyName.MaximumLength = MAX_PATH;

    UNICODE_STRING StubName;
    RtlInitUnicodeString(&StubName, L"\\Registry\\Machine\\System\\PSWTuner\\");
    UNICODE_STRING Seperator;
    RtlInitUnicodeString(&Seperator, L"\\");
    UNICODE_STRING Parameters;
    RtlInitUnicodeString(&Parameters, L"Device Parameters\\");

    RtlCopyUnicodeString(&KeyName, &StubName);
	
    if (!StubName.Length)
    {
        ExFreePoolWithTag(KeyName.Buffer, MS_SAMPLE_ANALOG_POOL_TAG);
        return STATUS_UNSUCCESSFUL;
    }

    UNICODE_STRING DevInstanceID;
    RtlInitUnicodeString(&DevInstanceID, pczDeviceInstanceID);

    RtlAppendUnicodeStringToString(&KeyName, &DevInstanceID);
    RtlAppendUnicodeStringToString(&KeyName, &Seperator);
    RtlAppendUnicodeStringToString(&KeyName, &Parameters);
	
	Status = PptRegGetDword(RTL_REGISTRY_ABSOLUTE, 
		KeyName.Buffer,
		AttributeName, 
		&AttributeValue);

    if (!NT_SUCCESS(Status))
    {
        DbgPrint( "%s  Error reading the registry 0x%08x\n", __FUNCTION__, Status); 
        // There was an error, use the default filename.
        AttributeValue= 0;
    }

    if (KeyName.Buffer)
    {
        ExFreePoolWithTag(KeyName.Buffer, MS_SAMPLE_ANALOG_POOL_TAG);
    }

    return Status;
}

DWORD 
GetSensingRanges( IN WCHAR* pczDeviceInstanceID, OUT DWORD& FineTuneSensingRange, OUT DWORD& ScanSensingRange)
{
	// 1) Convert dword frequency to string.
	// 2) find matching key in registry
	// 3) read SignalQuality value from registry.
	// 4) pass key back to caller.

	NTSTATUS Status = STATUS_SUCCESS;

	Status = GetDWORDAttribute(pczDeviceInstanceID,  L"FineTuneSensingRange", FineTuneSensingRange);

	if (!NT_SUCCESS(Status))
	{
        DbgPrint( "%s  Error reading the registry 0x%08x\n", __FUNCTION__, Status); 
		// There was an error, use the default filename.
		FineTuneSensingRange = 0;
	}
    DbgPrint("FineTuneSensingRange %ld\n", FineTuneSensingRange);

	Status = GetDWORDAttribute(pczDeviceInstanceID,  L"ScanSensingRange", ScanSensingRange);

	if (!NT_SUCCESS(Status))
	{
        DbgPrint( "%s  Error reading the registry 0x%08x\n", __FUNCTION__, Status); 
		// There was an error, use the default filename.
		ScanSensingRange = 0;
	}
      DbgPrint("ScanSensingRange %ld\n", ScanSensingRange);

	return Status;
}

DWORD 
GetMinMaxFrequency(IN WCHAR* pczDeviceInstanceID, OUT DWORD& MinFreq, OUT DWORD& MaxFreq )
{
	NTSTATUS Status = STATUS_SUCCESS;
	Status = GetDWORDAttribute(pczDeviceInstanceID,  L"MinFrequency", MinFreq);

	if (!NT_SUCCESS(Status))
	{
        DbgPrint( "%s  Error reading the registry 0x%08x\n", __FUNCTION__, Status); 
		// There was an error, use the default filename.
		MinFreq = 0;
	}
    DbgPrint("MinFreq %ld\n", MinFreq);

	Status = GetDWORDAttribute(pczDeviceInstanceID, L"MaxFrequency", MaxFreq);

	if (!NT_SUCCESS(Status))
	{
        DbgPrint( "%s  Error reading the registry 0x%08x\n", __FUNCTION__, Status); 
		// There was an error, use the default filename.
		MaxFreq = 0;
	}
    DbgPrint("MaxFreq %ld\n", MaxFreq);

	return Status;
}

DWORD 
GetSettlingTimeStandardSupported(IN WCHAR* pczDeviceInstanceID, OUT DWORD& SettlingTime, OUT DWORD& Standards)
{
	NTSTATUS Status = STATUS_SUCCESS;
	Status = GetDWORDAttribute(pczDeviceInstanceID, L"SettlingTime", SettlingTime);

	if (!NT_SUCCESS(Status))
	{
        DbgPrint( "%s  Error reading the registry 0x%08x\n", __FUNCTION__, Status); 
		// There was an error, use the default filename.
		SettlingTime = 0;
	}
    DbgPrint("SettlingTime %ld\n", SettlingTime);

	Status = GetDWORDAttribute(pczDeviceInstanceID, L"TvStandardSupported", Standards);

	if (!NT_SUCCESS(Status))
	{
        DbgPrint( "%s  Error reading the registry 0x%08x\n", __FUNCTION__, Status); 
		// There was an error, use the default filename.
		Standards = 0;
	}
    DbgPrint("TvStandardSupported %ld\n", Standards);

	return Status;
}

DWORD 
GetHardwareScanning( IN WCHAR* pczDeviceInstanceID, OUT BOOLEAN& HardwareAssistedScanning)
{
	// 1) Convert dword frequency to string.
	// 2) find matching key in registry
	// 3) read SignalPresent value from registry.
	// 4) pass key back to caller.
	HardwareAssistedScanning = FALSE;

	NTSTATUS Status = STATUS_SUCCESS;
	DWORD HardwareScanning = 0;
	Status = GetDWORDAttribute(pczDeviceInstanceID, L"HardwareScanning", HardwareScanning);

	if (!NT_SUCCESS(Status))
	{
        DbgPrint( "%s  Error reading the registry 0x%08x\n", __FUNCTION__, Status); 
	}

	if (HardwareScanning == 0)
	{
		HardwareAssistedScanning = FALSE;
        g_bHardwareAssistedScanning = FALSE;
	}
	else
	{
		HardwareAssistedScanning = TRUE;
        g_bHardwareAssistedScanning = TRUE;
	}

    DbgPrint("HardwareAssistedScanning %ld\n", HardwareAssistedScanning);

	return Status;
}

NTSTATUS AnlgTunerFilterCreate(IN PKSFILTER Filter, IN PIRP Irp) 
{
	NTSTATUS Status = STATUS_SUCCESS;

	if(!Filter)
	{
		return STATUS_UNSUCCESSFUL;
	}
	if(!Irp)
	{
		return STATUS_UNSUCCESSFUL;
	}

	PKSDEVICE pKsDevice = KsFilterGetDevice(Filter);
	if(!pKsDevice) 
	{
		return Status;
	}

	BOOL pPinDirection[2] = {TRUE, TRUE};
	KSPIN_MEDIUM pMediumList[2] = 
	{
		{
			GUID_AnlgTunerPinMediumVideoOut, 0, 0
		},
		{
        		GUID_AnlgTunerPinMediumAudioOut, 0, 0
		}
	};

	Status = KsRegisterFilterWithNoKSPins(
		pKsDevice->PhysicalDeviceObject,
		&KSCATEGORY_TVTUNER,
		2,
		pPinDirection,
		pMediumList,
		NULL 
		);

	return Status;
}

NTSTATUS AnlgTunerFilterClose(IN PKSFILTER Filter, IN PIRP Irp)
{
    UNREFERENCED_PARAMETER(Filter);
    UNREFERENCED_PARAMETER(Irp);
    
    return STATUS_SUCCESS;
}

NTSTATUS AnlgTunerFrequencyGetHandler
(
 IN PIRP Irp,
 IN PKSPROPERTY_TUNER_FREQUENCY_S pRequest,
 IN OUT PKSPROPERTY_TUNER_FREQUENCY_S pData
 )
{	

    UNREFERENCED_PARAMETER(pRequest); 
       
	ASSERT(Irp);
	PKSFILTER pFilter = KsGetFilterFromIrp(Irp);
	if(!pFilter)
		return STATUS_UNSUCCESSFUL;
	
	PKSDEVICE pDevice = KsFilterGetDevice(pFilter);
	if(!pDevice)
		return STATUS_UNSUCCESSFUL;

	CEncoderDevice* pEncDevice = (CEncoderDevice *)(pDevice->Context);
	if (!pEncDevice)
		return STATUS_UNSUCCESSFUL;

	pEncDevice->GetFrequency(&pData->Frequency);
	pData->LastFrequency = pData->Frequency;
	pData->TuningFlags = KS_TUNER_TUNING_EXACT;
	pData->Channel = 4;

	
    NTSTATUS Status;
	WCHAR strHardwareId[MAX_PATH+1]={0};
	 //Get HardwareIDs
	ULONG lLenResult = 0;
	Status = IoGetDeviceProperty(pDevice->PhysicalDeviceObject
								,DevicePropertyHardwareID 
								,MAX_PATH
								,strHardwareId
								,&lLenResult);
	if(!NT_SUCCESS(Status))
	{
	    DbgPrint("IoGetDeviceProperty failed!\n");
	    return Status;
	}
	
      if(0 == wcscmp(strHardwareId , L"ms_swtNTSC"))//NTSC
      {
		pData->Country = 1;
              DbgPrint("set analog NTSC country!\n");
      }
      else if(0 == wcscmp(strHardwareId, L"ms_swtPAL"))//PAL
      {
 		pData->Country = 49;
              DbgPrint("set analog PAL country!\n");
      }
      else
      {
           DbgPrint("unkown analog Device!\n");
           return STATUS_ABANDONED;
      }
	return STATUS_SUCCESS;
}

NTSTATUS AnlgTunerFrequencySetHandler
(
 IN PIRP Irp,
 IN PKSPROPERTY_TUNER_FREQUENCY_S pRequest,
 IN OUT PKSPROPERTY_TUNER_FREQUENCY_S pData
 )
{ 
    ASSERT(Irp);
    PKSFILTER pFilter = KsGetFilterFromIrp(Irp);
    if (!pFilter)
        return STATUS_UNSUCCESSFUL;

    PKSDEVICE pDevice = KsFilterGetDevice(pFilter);
    if (!pDevice)
        return STATUS_UNSUCCESSFUL;

    UNREFERENCED_PARAMETER(pData);

    CEncoderDevice* pEncDevice = (CEncoderDevice *)(pDevice->Context);
    if (!pEncDevice)
        return STATUS_UNSUCCESSFUL;

    if (pEncDevice->CurrentInput == 0)//only set the frequency if we are at the tuner, a real device should tune 
    {
        pEncDevice->SetFrequency(pRequest->Frequency);
    }

    DbgPrint("SWTANALOG-FrequencySetHandler %d,country is %d\n", pRequest->Frequency,pRequest->Country);

    return STATUS_SUCCESS;
}

NTSTATUS AnlgTunerInputGetHandler
(
 PIRP Irp,
 IN PKSPROPERTY_TUNER_INPUT_S pRequest,
 IN OUT PKSPROPERTY_TUNER_INPUT_S pData
 ) 
{
    UNREFERENCED_PARAMETER(Irp); 
    UNREFERENCED_PARAMETER(pRequest); 
    pData->InputIndex = 0;
    return STATUS_SUCCESS;
}

NTSTATUS AnlgTunerCapsGetHandler
(
 IN PIRP Irp,
 IN PKSPROPERTY_TUNER_CAPS_S pRequest,
 IN OUT PKSPROPERTY_TUNER_CAPS_S pData
 ) {
    UNREFERENCED_PARAMETER(Irp); 
    UNREFERENCED_PARAMETER(pRequest); 
        
    pData->ModesSupported = KSPROPERTY_TUNER_MODE_TV;
    pData->VideoMedium = PinMediumVideoOut;
    pData->TVAudioMedium = PinMediumAudioOut;
    DbgPrint("SWTANALOG-CapsGetHandler");
    return STATUS_SUCCESS;
}

NTSTATUS AnlgTunerModeGetHandler
(
 IN PIRP Irp,
 IN PKSPROPERTY_TUNER_MODE_S pRequest,
 IN OUT PKSPROPERTY_TUNER_MODE_S pData

 ) 
{ 
    UNREFERENCED_PARAMETER(Irp); 
    UNREFERENCED_PARAMETER(pRequest); 

    pData->Mode = KSPROPERTY_TUNER_MODE_TV;
    DbgPrint("SWTANALOG-ModeGetHandler");
    return STATUS_SUCCESS;
}

NTSTATUS AnlgTunerModeCapsGetHandler
(
 IN PIRP Irp,
 IN PKSPROPERTY_TUNER_MODE_CAPS_S pRequest,
 IN OUT PKSPROPERTY_TUNER_MODE_CAPS_S pData

 ) 
{

    UNREFERENCED_PARAMETER(pRequest); 
       
    PKSFILTER pFilter = KsGetFilterFromIrp(Irp);
    if(!pFilter)
        return STATUS_UNSUCCESSFUL;
    PKSDEVICE pDevice = KsFilterGetDevice(pFilter);
    if(!pDevice)
        return STATUS_UNSUCCESSFUL;

    NTSTATUS Status;
    WCHAR strHardwareId[MAX_PATH+1]={0};
    //Get HardwareIDs
    ULONG lLenResult = 0;
    Status = IoGetDeviceProperty(pDevice->PhysicalDeviceObject
                                    ,DevicePropertyHardwareID 
                                    ,MAX_PATH
                                    ,strHardwareId
                                    ,&lLenResult);
    if(!NT_SUCCESS(Status))
    {
        DbgPrint("IoGetDeviceProperty failed!\n");
        return Status;
    }

    DbgPrint("Get Hardware ID is %S SUCCEEDED\n!",strHardwareId);	 
    ULONG ulTvStandard=0; 
    Status = CEncoderDevice::GetTVStandard(strHardwareId,ulTvStandard);
    if(!NT_SUCCESS(Status))
    {
        DbgPrint("AnlgTunerModeCapsGetHandler-->GetTVStandard failed\n");
        return Status;
    }

    pData->StandardsSupported = ulTvStandard;
    pData->NumberOfInputs = 1;
    pData->Strategy = KS_TUNER_STRATEGY_DRIVER_TUNES;
    // This is sleeping time for vidctl
    pData->SettlingTime = 0;

    // Reasonable values required. Otherwise kstvtune will decide no channels fit.
    pData->MinFrequency = 48000000;
    pData->TuningGranularity = 50000;

    pData->MaxFrequency = 862000000;
    // end

    return STATUS_SUCCESS;

}

NTSTATUS AnlgTunerStandardGetHandler
(
 IN PIRP Irp,
 IN PKSPROPERTY_TUNER_STANDARD_S pRequest,
 IN OUT PKSPROPERTY_TUNER_STANDARD_S pData
 ) 
{
    UNREFERENCED_PARAMETER(pRequest); 

	PKSFILTER pFilter = KsGetFilterFromIrp(Irp);
	if(!pFilter)
		return STATUS_UNSUCCESSFUL;
	
	PKSDEVICE pDevice = KsFilterGetDevice(pFilter);
	if(!pDevice)
		return STATUS_UNSUCCESSFUL;
	
    NTSTATUS Status;
	WCHAR strHardwareId[MAX_PATH+1]={0};
    //Get HardwareIDs
    ULONG lLenResult = 0;
	Status = IoGetDeviceProperty(pDevice->PhysicalDeviceObject
								,DevicePropertyHardwareID 
								,MAX_PATH
								,strHardwareId
								,&lLenResult);
	if(!NT_SUCCESS(Status))
	{
	    DbgPrint("AnlgTunerStandardGetHandler -- IoGetDeviceProperty failed!\n");
	    return Status;
	}

	ULONG ulTvStandard=0; 
	Status = CEncoderDevice::GetTVStandard(strHardwareId,ulTvStandard);
	if(!NT_SUCCESS(Status))
	{
	   DbgPrint("AnlgTunerModeCapsGetHandler-->GetTVStandard failed\n");
	   return Status;
	}
	 
    pData->Standard = ulTvStandard;
    return STATUS_SUCCESS;

}
NTSTATUS AnlgTunerStatusGetHandler
(
 IN PIRP Irp,
 IN PKSPROPERTY_TUNER_STATUS_S pRequest,
 IN OUT PKSPROPERTY_TUNER_STATUS_S pData
 ) 
{

    UNREFERENCED_PARAMETER(pRequest); 
	PKSFILTER pFilter = KsGetFilterFromIrp(Irp);
	if (!pFilter)
		return STATUS_UNSUCCESSFUL;
       
	PKSDEVICE pDevice = KsFilterGetDevice(pFilter);
	if (!pDevice)
		return STATUS_UNSUCCESSFUL;

	CEncoderDevice* pEncDevice = reinterpret_cast<CEncoderDevice *>(pDevice->Context);
	if (!pEncDevice )
		return STATUS_UNSUCCESSFUL;

	extern bool g_bFrequencySignal;
	if(g_bFrequencySignal)
	{
		pData->CurrentFrequency = pEncDevice->m_TsSynth->m_fileReader.GetFrequency();
		pData->SignalStrength = 100;
		pData->Busy = FALSE;
		pData->PLLOffset = 0;		
	}
	else
	{
		pData->CurrentFrequency = 500000000;
		pData->SignalStrength = 0;
		pData->Busy = FALSE;
		pData->PLLOffset = 0;
	}
	return STATUS_SUCCESS;
}

NTSTATUS AnlgTunerScanCapsGetHandler
(
 IN PIRP Irp,
 IN PKSPROPERTY_TUNER_SCAN_CAPS_S pRequest,
 IN OUT PKSPROPERTY_TUNER_SCAN_CAPS_S pData
 )
{
	DbgPrint("SWTANALOG-ScanCapsGetHandler");
	BOOLEAN bScanning = TRUE;
	NTSTATUS Status = STATUS_SUCCESS;

	ASSERT(Irp);
	PKSFILTER pFilter = KsGetFilterFromIrp(Irp);
	if (!pFilter)
		return STATUS_UNSUCCESSFUL;

	PKSDEVICE pDevice = KsFilterGetDevice(pFilter);
	if (!pDevice)
		return STATUS_UNSUCCESSFUL;

	CEncoderDevice* pEncDevice = reinterpret_cast<CEncoderDevice *>(pDevice->Context);
	if (!pEncDevice)
		return STATUS_UNSUCCESSFUL;
	
	RtlCopyMemory(pData, pRequest, sizeof(KSPROPERTY_TUNER_SCAN_CAPS_S));

	WCHAR* pczDeviceInstanceID = pEncDevice->GetDeviceInstanceID();
	GetHardwareScanning(pczDeviceInstanceID, bScanning);

	pData->fSupportsHardwareAssistedScanning = bScanning;

	if(pData->SupportedBroadcastStandards == 0)
	{
		pData->SupportedBroadcastStandards = 1;
	}
	else
	{
		__try
		{
			ProbeForWrite(pData->GUIDBucket, pData->lengthofBucket,sizeof(UCHAR));
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			Status = GetExceptionCode();
			return Status;
		}

		if (pData->lengthofBucket < (sizeof(GUID)))
		{
			Status = STATUS_BUFFER_TOO_SMALL;
			return Status;
		}

		GUID* guid = reinterpret_cast<GUID*>(pData->GUIDBucket);
		RtlCopyMemory(guid, &ANALOG_TV_NETWORK_TYPE, sizeof(GUID));
		//RtlCopyMemory(guid++, &DIGITAL_CABLE_NETWORK_TYPE, sizeof(GUID));
	}
	return STATUS_SUCCESS;
}

NTSTATUS AnlgTunerAnalogCapsGetHandler
(
 IN PIRP Irp,
 IN PKSPROPERTY_TUNER_NETWORKTYPE_SCAN_CAPS_S pRequest,
 IN OUT PKSPROPERTY_TUNER_NETWORKTYPE_SCAN_CAPS_S pData
 )
{
	ASSERT(Irp);
	PKSFILTER pFilter = KsGetFilterFromIrp(Irp);
	if(!pFilter)
		return STATUS_UNSUCCESSFUL;

	PKSDEVICE pDevice = KsFilterGetDevice(pFilter);
	if(!pDevice)
		return STATUS_UNSUCCESSFUL;

	CEncoderDevice* pEncDevice = reinterpret_cast<CEncoderDevice *>(pDevice->Context);
	if (!pEncDevice)
		return STATUS_UNSUCCESSFUL;
		
	NTSTATUS Status = STATUS_SUCCESS;

	DbgPrint("SWTANALOG-AnalogCapsGetHandler");

	RtlCopyMemory(pData, pRequest, sizeof(KSPROPERTY_TUNER_NETWORKTYPE_SCAN_CAPS_S));

	if(pData->NetworkTunerCapabilities && pData->BufferSize)
	{
		__try
		{
			ProbeForWrite(pData->NetworkTunerCapabilities, pData->BufferSize , sizeof(UCHAR));
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			Status = GetExceptionCode();
			return Status;
		}
		if(pData->NetworkType == ANALOG_TV_NETWORK_TYPE)
		{
			__try
			{
				ProbeForWrite(pData->NetworkTunerCapabilities, sizeof(TUNER_ANALOG_CAPS_S),sizeof(UCHAR));
			}
			__except (EXCEPTION_EXECUTE_HANDLER) {
				Status = GetExceptionCode();
				return Status;
			}

			PTUNER_ANALOG_CAPS_S pAnalog_Caps = reinterpret_cast<PTUNER_ANALOG_CAPS_S>(pData->NetworkTunerCapabilities);

			pAnalog_Caps->StandardsSupported = pEncDevice->StandardsSupported;
			pAnalog_Caps->SettlingTime = pEncDevice->SettlingTime;
			pAnalog_Caps->MinFrequency = pEncDevice->MinFrequency;
			pAnalog_Caps->MaxFrequency = pEncDevice->MaxFrequency;
			pAnalog_Caps->Mode = KSPROPERTY_TUNER_MODE_TV;
			pAnalog_Caps->ScanSensingRange = pEncDevice->ScanSensingRange;
			pAnalog_Caps->FineTuneSensingRange = pEncDevice->FineTuneSensingRange;
		}
	}
	else
	{
		return STATUS_UNSUCCESSFUL;
	}
	return STATUS_SUCCESS;
}



NTSTATUS AnlgTunerScanStatusGetHandler
(
 IN PIRP Irp,
 IN PKSPROPERTY_TUNER_SCAN_STATUS_S pRequest,
 IN OUT PKSPROPERTY_TUNER_SCAN_STATUS_S pData
 )
{
    ASSERT(Irp);
    KIRQL     OldIrql;
    KIRQL     OldIrqlEvent=NULL;
    NTSTATUS  Status  = STATUS_SUCCESS;

    PKSFILTER pFilter = KsGetFilterFromIrp(Irp);
    if (!pFilter)
        return STATUS_UNSUCCESSFUL;

    PKSDEVICE pDevice = KsFilterGetDevice(pFilter);
    if (!pDevice)
        return STATUS_UNSUCCESSFUL;

    CEncoderDevice* pEncDevice = reinterpret_cast<CEncoderDevice *>(pDevice->Context);
    if (!pEncDevice )
        return STATUS_UNSUCCESSFUL;

    UNREFERENCED_PARAMETER(pRequest); 

    KeAcquireSpinLock(&pEncDevice->EventHandlerSpinLock, &OldIrql); //protect EventData
    PEventHandlerData EventData = reinterpret_cast<PEventHandlerData>(pEncDevice->EventData);

    if (EventData)
    {
        KeAcquireSpinLock(&(EventData->LockAccess), &OldIrqlEvent);//protect ScanStatusCode and CurrentFrequency
    }

    pData->LockStatus = pEncDevice->ScanStatusCode;

    pData->CurrentFrequency = pEncDevice->CurrentFrequency;

    if (pEncDevice->ScanStatusCode == Tuner_LockType_Locked)
    {
        DbgPrint("MY-ScanStatusGetHandler locked at Frequency %d",pEncDevice->CurrentFrequency);
    }
    else
    {
        DbgPrint("MY-ScanStatusGetHandler not locked at Frequency %d",pEncDevice->CurrentFrequency);
    }

    if (EventData)
    {
        KeReleaseSpinLock(&(EventData->LockAccess), OldIrqlEvent);
    }

    KeReleaseSpinLock(&pEncDevice->EventHandlerSpinLock, OldIrql);
    return Status;
}

NTSTATUS AnlgTunerStandardModeGetHandler
(
 IN PIRP Irp,
 IN PKSPROPERTY_TUNER_STANDARD_MODE_S pRequest,
 IN OUT PKSPROPERTY_TUNER_STANDARD_MODE_S pData
 )
{

	if(!pData || !pRequest || !Irp)
		return STATUS_UNSUCCESSFUL;
	pData->AutoDetect = FALSE;
	return STATUS_SUCCESS;
}

NTSTATUS AnlgTunerStandardModeSetHandler
(
 IN PIRP Irp,
 IN PKSPROPERTY_TUNER_STANDARD_MODE_S pRequest,
 IN OUT PKSPROPERTY_TUNER_STANDARD_MODE_S pData
 )
{
	if(!pData || !pRequest || !Irp)
		return STATUS_UNSUCCESSFUL;
	pData->AutoDetect = FALSE;
	return STATUS_SUCCESS;
}



