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
    File    : tvaudio.cpp
    Abstract: Contains dispatch routines, property handlers and intersect
              handlers for the TV Audio filter
**************************************************************************/

#include "tvaudio.h"
NTSTATUS TVAudioFilterCreate(IN PKSFILTER Filter, IN PIRP Irp) 
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

    BOOL pPinDirection[2] = {FALSE, TRUE};
    KSPIN_MEDIUM pMediumList[2] = 
    {
	{
	    GUID_AnlgTunerPinMediumAudioOut, 0, 0
	}, 
	{
	    GUID_TVAudioPinMediumAudioOut, 0, 0
	}
    };

    Status = KsRegisterFilterWithNoKSPins(
		 pKsDevice->PhysicalDeviceObject,
		 &KSCATEGORY_TVAUDIO,
		 2,
		 pPinDirection,
		 pMediumList,
		 NULL 
		 );
    
    return Status;
}

NTSTATUS TVAudioFilterClose(IN PKSFILTER Filter, IN PIRP Irp)
{
    UNREFERENCED_PARAMETER(Filter);
    UNREFERENCED_PARAMETER(Irp);
    return STATUS_SUCCESS;
}


NTSTATUS TVAudioCapsGetHandler
(
    IN PIRP Irp,
    IN PKSPROPERTY_TVAUDIO_CAPS_S pRequest,
    IN OUT PKSPROPERTY_TVAUDIO_CAPS_S pData
) {

    UNREFERENCED_PARAMETER(Irp);
    UNREFERENCED_PARAMETER(pRequest);

    pData->Capabilities = KS_TVAUDIO_MODE_MONO | KS_TVAUDIO_MODE_STEREO | KS_TVAUDIO_MODE_LANG_A | KS_TVAUDIO_MODE_LANG_B;
    pData->InputMedium = PinMediumAudioIn;
    pData->OutputMedium = PinMediumAudioOut;
    return STATUS_SUCCESS;

}

NTSTATUS TVAudioModeGetHandler
(
    IN PIRP Irp,
    IN PKSPROPERTY_TVAUDIO_S pRequest,
    IN OUT PKSPROPERTY_TVAUDIO_S pData
) {

    UNREFERENCED_PARAMETER(Irp);
    UNREFERENCED_PARAMETER(pRequest);
    
    pData->Mode = KS_TVAUDIO_MODE_STEREO;
    return STATUS_SUCCESS;

}


