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
    File    : xbar.cpp
    Abstract: Contains dispatch routines, property handlers and intersect
              handlers for the Crossbar Filter
**************************************************************************/

#include "xbar.h"
NTSTATUS XBarFilterCreate(IN PKSFILTER Filter, IN PIRP Irp) 
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
    BOOL pPinDirection[8] = {FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE};
    KSPIN_MEDIUM pMediumList[8] = 
    {
	{
	    GUID_AnlgTunerPinMediumVideoOut, 0, 0
	},
	{
	    GUID_TVAudioPinMediumAudioOut, 0, 0
	},	
	{
            GUID_XBarPinMediumVideoAUX, 0, 0 //Composite IN
        },
        {
            GUID_XBarPinMediumAudioAUX, 0, 0
        },	
        {
            GUID_XBarPinMediumVideoAUX, 0, 0 //SVIDEO IN
        },
        {
            GUID_XBarPinMediumAudioAUX, 0, 0
        },	
        {
	    GUID_XBarPinMediumVideoOut, 0, 0
	},
	{
	    GUID_XBarPinMediumAudioOut, 0, 0
	}
    };
    
    Status = KsRegisterFilterWithNoKSPins(
                 pKsDevice->PhysicalDeviceObject,
		 &KSCATEGORY_CROSSBAR,
		 8,
		 pPinDirection,
		 pMediumList,
		 NULL
		 );

    return Status;

}

NTSTATUS XBarFilterClose(IN PKSFILTER Filter, IN PIRP Irp)
{
    UNREFERENCED_PARAMETER(Filter);
    UNREFERENCED_PARAMETER(Irp);
    
    return STATUS_SUCCESS;
}


NTSTATUS XBarPinInfoGetHandler
(
    IN PIRP Irp,
    IN PKSPROPERTY_CROSSBAR_PININFO_S pRequest,
    IN OUT PKSPROPERTY_CROSSBAR_PININFO_S pData
) {
    UNREFERENCED_PARAMETER(Irp);
    if (!pData)
    {
        return STATUS_UNSUCCESSFUL;
    }

    if (pRequest->Direction == KSPIN_DATAFLOW_IN) {
	if (pRequest->Index == 1) {
	    pData->Medium = PinMediumAudioIn;
	    pData->PinType = KS_PhysConn_Audio_Tuner;
            pData->RelatedPinIndex = 0;
	}
	if (pRequest->Index == 0) {
	    pData->Medium = PinMediumVideoIn;
	    pData->PinType = KS_PhysConn_Video_Tuner;
            pData->RelatedPinIndex = 1;
        }
        if (pRequest->Index == 3) {
            pData->Medium = PinMediumAudioAUXIn;
            pData->PinType = KS_PhysConn_Audio_Line;
            pData->RelatedPinIndex = 2;
        }
        if (pRequest->Index == 2) {
            pData->Medium = PinMediumVideoAUXIn;
            pData->PinType = KS_PhysConn_Video_Composite;
            pData->RelatedPinIndex = 3;
        }
        if (pRequest->Index == 5) {
            pData->Medium = PinMediumAudioAUXIn;
            pData->PinType = KS_PhysConn_Audio_Line;
            pData->RelatedPinIndex = 4;
        }
        if (pRequest->Index == 4) {
            pData->Medium = PinMediumVideoAUXIn;
            pData->PinType = KS_PhysConn_Video_SVideo;
            pData->RelatedPinIndex = 5;
	}
    }
    if (pRequest->Direction == KSPIN_DATAFLOW_OUT) {
	if (pRequest->Index == 1) {
	    pData->Medium = PinMediumAudioOut;
            pData->PinType = KS_PhysConn_Audio_AudioDecoder;
            pData->RelatedPinIndex = 0;
	}
	if (pRequest->Index == 0) {
	    pData->Medium = PinMediumVideoOut;
            pData->PinType = KS_PhysConn_Video_VideoDecoder;
            pData->RelatedPinIndex = 1;
	}
    }
    return STATUS_SUCCESS;

}

NTSTATUS XBarCapsGetHandler
(
    IN PIRP Irp,
    IN PKSPROPERTY_CROSSBAR_CAPS_S pRequest,
    IN OUT PKSPROPERTY_CROSSBAR_CAPS_S pData
 ) {

    UNREFERENCED_PARAMETER(pRequest);
    UNREFERENCED_PARAMETER(Irp);

    if (!pData)
    {
        return STATUS_UNSUCCESSFUL;
    }

    pData->NumberOfInputs = 6;
    pData->NumberOfOutputs = 2;

    return STATUS_SUCCESS;
}

NTSTATUS XBarRouteCanGetHandler
(
    IN PIRP Irp,
    IN PKSPROPERTY_CROSSBAR_ROUTE_S pRequest,
    IN OUT PKSPROPERTY_CROSSBAR_ROUTE_S pData
)
{
    UNREFERENCED_PARAMETER(Irp);
    if ((!pData) || (!pRequest))
    {
        return STATUS_UNSUCCESSFUL;
    }

    pData->CanRoute = 0;
    pData->IndexInputPin = pRequest->IndexInputPin;
    pData->IndexOutputPin = pRequest->IndexOutputPin;

    if (0 == pRequest->IndexOutputPin)//video out
    {
        if ((pRequest->IndexInputPin == 0) || (pRequest->IndexInputPin == 2) || (pRequest->IndexInputPin == 4))
        {
            pData->CanRoute = 1;
        }
    }
    if (1 == pRequest->IndexOutputPin)//audio out
    {
        if ((pRequest->IndexInputPin == 1) || (pRequest->IndexInputPin == 3) || (pRequest->IndexInputPin == 5))
        {
            pData->CanRoute = 1;
        }
    }
    
    //DbgPrint( "MY-XBarRouteCanGetHandler return Input=%d Output=%d CanRoute=%d\n", pData->IndexInputPin,pData->IndexOutputPin,pData->CanRoute );

    return STATUS_SUCCESS;
}

NTSTATUS XBarRouteHandler
(
    IN PIRP pIrp,
    IN PKSPROPERTY_CROSSBAR_ROUTE_S pRequest,
    IN OUT PKSPROPERTY_CROSSBAR_ROUTE_S pData
)
{
    NTSTATUS Status = STATUS_UNSUCCESSFUL;
    if ((!pData) || (!pRequest) || (!pIrp))
    {
        return Status;
    }
    PKSFILTER pFilter = KsGetFilterFromIrp(pIrp);
    PKSDEVICE pDevice = KsFilterGetDevice(pFilter);
    if (!pDevice)
    {
        return Status;
    }
    CEncoderDevice* pEncDevice = reinterpret_cast<CEncoderDevice *>(pDevice->Context);
    if(!pEncDevice)
        return Status;

    if(pRequest->Property.Flags == KSPROPERTY_TYPE_SET)
    {
        Status = STATUS_SUCCESS;
        if ((0 == pRequest->IndexOutputPin) && (pRequest->IndexInputPin == 0)) //switch to tuner
        {
            DbgPrint( "MY-XBarRouteHandler switch Video to Tuner\n");
            if ((pEncDevice->CurrentFrequency) && (pEncDevice->CurrentInput != 0))
            {
                pEncDevice->SetFrequency(pEncDevice->CurrentFrequency);//restore the previously used frequency
            }
            pEncDevice->CurrentInput = 0;
        }
        else if ((1 == pRequest->IndexOutputPin) && (pRequest->IndexInputPin == 1)) //switch to tuner
        {
            DbgPrint( "MY-XBarRouteHandler switch Audio to Tuner\n");
        }
        else if ((0 == pRequest->IndexOutputPin) && (pRequest->IndexInputPin == 2)) //switch to Composite
        {
            DbgPrint( "MY-XBarRouteHandler switch Video to Composite\n");
            if (pEncDevice->CurrentInput == 0)
            {
                pEncDevice->GetFrequency(&pEncDevice->CurrentFrequency);
            }
            pEncDevice->CurrentInput = 1;
            pEncDevice->SetFrequency(pEncDevice->CurrentInput * 1000);//map to 1 khz
        }
        else if ((1 == pRequest->IndexOutputPin) && (pRequest->IndexInputPin == 3)) //switch to Composite
        {
            DbgPrint( "MY-XBarRouteHandler switch Audio to Composite\n");
        }
        else if ((0 == pRequest->IndexOutputPin) && (pRequest->IndexInputPin == 4)) //switch to S-Video
        {
            DbgPrint( "MY-XBarRouteHandler switch Video to S-Video\n");
            if (pEncDevice->CurrentInput == 0)
            {
                pEncDevice->GetFrequency(&pEncDevice->CurrentFrequency);
            }
            pEncDevice->CurrentInput = 2;
            pEncDevice->SetFrequency(pEncDevice->CurrentInput * 1000);//map to 2 khz
        }
        else if ((1 == pRequest->IndexOutputPin) && (pRequest->IndexInputPin == 5)) //switch to S-Video
        {
            DbgPrint( "MY-XBarRouteHandler switch Audio to S-Video\n");
        }
        else
        {
            Status = STATUS_UNSUCCESSFUL;
        }
    }
    else //get the routing information
    {
        pData->CanRoute = 0;

        if (0 == pRequest->IndexOutputPin)//video out
        {
            pData->IndexInputPin = pEncDevice->CurrentInput * 2;//return the current Video input
            pData->IndexOutputPin = 0;
            pData->CanRoute = 1;
            Status = STATUS_SUCCESS;
        }
        else if (1 == pRequest->IndexOutputPin)//audio out
        {
            pData->IndexInputPin = pEncDevice->CurrentInput * 2 + 1;//return the current Audio input
            pData->IndexOutputPin = 1;
            pData->CanRoute = 1;
            Status = STATUS_SUCCESS;
        }

        //DbgPrint( "MY-XBarRouteHandler Get return Input=%d Output=%d CanRoute=%d\n", pData->IndexInputPin,pData->IndexOutputPin,pData->CanRoute );
    }
    

    return Status;
}

