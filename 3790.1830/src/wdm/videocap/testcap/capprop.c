//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992 - 1996  Microsoft Corporation.  All Rights Reserved.
//
//==========================================================================;

//
// This file handles all adapter property sets
//


#include "strmini.h"
#include "ksmedia.h"
#include "capmain.h"
#include "capdebug.h"
#include "capxfer.h"
#define DEFINE_MEDIUMS
#include "mediums.h"

// -------------------------------------------------------------------
// A few notes about property set handling
//  
// Property sets used in Testcap are of two varieties, those that have
// default values, ranges, and stepping, such as VideoProcAmp and CameraControl,
// and those which don't have defaults and ranges, such as TVTuner and
// Crossbar.
// 
// Default values and stepping are established by tables in capprop.h,
// no code is required to implement this other than initally creating the tables.
// 
// Many of the property sets require the ability to modify a number
// of input parameters.  Since KS doesn't allow this inherently, you'll
// note that some property sets require copying the provided input parameters
// to the ouput parameter list, effectively creating a "read, modify, write"
// capability.  For this reason, the input and output parameter lists
// use identical structures.
//
// On an SRB_GET_DEVICE_PROPERTY, read-only input data to the driver is provided as:
//      pSrb->CommandData.PropertyInfo
//
// ... while the output data pointer is:
//      pSrb->CommandData.PropertyInfo.PropertyInfo
// 
// -------------------------------------------------------------------


// -------------------------------------------------------------------
// XBar pin definitions
// -------------------------------------------------------------------

typedef struct _XBAR_PIN_DESCRIPTION {
    ULONG       PinType;
    ULONG       SynthImageCommand;    // This driver simulates different inputs by synthesizing images
    ULONG       RelatedPinIndex;
    const KSPIN_MEDIUM *Medium;               // Describes hardware connectivity
} XBAR_PIN_DESCRIPTION, *PXBAR_PIN_DESCRIPTION;


XBAR_PIN_DESCRIPTION XBarInputPins[] = {

    // First list the video input pins, then the audio inputs, then the output pins
    // Note that audio pin index 6 is shared between two video inputs (index 1 and index 2)
    

    //    PinType                       SynthImageCommand                     RelatedPinIndex   Medium
    /*0*/ KS_PhysConn_Video_Tuner,         IMAGE_XFER_NTSC_EIA_100AMP_100SAT,    5,             &CrossbarMediums[0],
    /*1*/ KS_PhysConn_Video_Composite,     IMAGE_XFER_NTSC_EIA_75AMP_100SAT,     6,             &CrossbarMediums[1],
    /*2*/ KS_PhysConn_Video_SVideo,        IMAGE_XFER_BLACK,                     6,             &CrossbarMediums[2],
    /*3*/ KS_PhysConn_Video_Tuner,         IMAGE_XFER_WHITE,                     7,             &CrossbarMediums[3],
    /*4*/ KS_PhysConn_Video_Composite,     IMAGE_XFER_GRAY_INCREASING,           8,             &CrossbarMediums[4],
    
    /*5*/ KS_PhysConn_Audio_Tuner,         0,                                    0,             &CrossbarMediums[5],
    /*6*/ KS_PhysConn_Audio_Line,          0,                                    1,             &CrossbarMediums[6],
    /*7*/ KS_PhysConn_Audio_Tuner,         0,                                    3,             &CrossbarMediums[7],
    /*8*/ KS_PhysConn_Audio_Line,          0,                                    4,             &CrossbarMediums[8],

};
#define NUMBER_OF_XBAR_INPUTS       (SIZEOF_ARRAY (XBarInputPins))


XBAR_PIN_DESCRIPTION XBarOutputPins[] = {

    //    PinType                       SynthImageCommand                     RelatedPinIndex

    /*0*/ KS_PhysConn_Video_VideoDecoder,  0,                                    1,             &CrossbarMediums[9],
    /*1*/ KS_PhysConn_Audio_AudioDecoder,  0,                                    0,             &CrossbarMediums[10],
};
#define NUMBER_OF_XBAR_OUTPUTS      (SIZEOF_ARRAY (XBarOutputPins))

#define NUMBER_OF_XBAR_PINS_TOTAL   (NUMBER_OF_XBAR_INPUTS + NUMBER_OF_XBAR_OUTPUTS)


// -------------------------------------------------------------------
// XBar Property Set functions
// -------------------------------------------------------------------

/*
** AdapterSetCrossbarProperty ()
**
**    Handles Set operations on the Crossbar property set.
**      Testcap uses this to select an image to synthesize.
**
** Arguments:
**
**      pSRB -
**          Pointer to the HW_STREAM_REQUEST_BLOCK 
**
** Returns:
**
** Side Effects:  none
*/

VOID 
STREAMAPI
AdapterSetCrossbarProperty(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PHW_DEVICE_EXTENSION pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
    ULONG Id  = pSPD->Property->Id;              // index of the property
    ULONG nS  = pSPD->PropertyOutputSize;        // size of data supplied

    switch (Id) {
    case KSPROPERTY_CROSSBAR_ROUTE:                       //  W 
    {
        PKSPROPERTY_CROSSBAR_ROUTE_S  pRoute = 
            (PKSPROPERTY_CROSSBAR_ROUTE_S)pSPD->PropertyInfo;   

        ASSERT (nS >= sizeof (KSPROPERTY_CROSSBAR_ROUTE_S));

        // Copy the input property info to the output property info
        RtlCopyMemory(  pRoute, 
                        pSPD->Property, 
                        sizeof (KSPROPERTY_CROSSBAR_ROUTE_S));


        // Default to failure
        pRoute->CanRoute = 0;

        // if video
        if (pRoute->IndexOutputPin == 0) {
            if (pRoute->IndexInputPin <= 4) {
                pHwDevExt->VideoInputConnected = pRoute->IndexInputPin;
                pRoute->CanRoute = 1;
            }
        }
        // if audio
        else if (pRoute->IndexOutputPin == 1) {
            // Special case!  Audio Routing of (-1) means mute!!!
            if (pRoute->IndexInputPin == -1) {
                pHwDevExt->AudioInputConnected = pRoute->IndexInputPin;
                pRoute->CanRoute = 1;
            }
            else if (pRoute->IndexInputPin > 4 && pRoute->IndexInputPin <= 8) {
                pHwDevExt->AudioInputConnected = pRoute->IndexInputPin;
                pRoute->CanRoute = 1;
            }
        }

        // Somebody passed bogus data
        if (pRoute->CanRoute == 0) {
            pSrb->Status = STATUS_INVALID_PARAMETER;
        }
    }
    break;


    default:
        TRAP;
        break;
    }
}

/*
** AdapterGetCrossbarProperty ()
**
**    Handles Get operations on the Crossbar property set.
**      Testcap uses this to select an image to synthesize.
**
** Arguments:
**
**      pSRB -
**          Pointer to the HW_STREAM_REQUEST_BLOCK 
**
** Returns:
**
** Side Effects:  none
*/

VOID 
STREAMAPI
AdapterGetCrossbarProperty(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PHW_DEVICE_EXTENSION pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
    ULONG Id  = pSPD->Property->Id;              // index of the property
    ULONG nS  = pSPD->PropertyOutputSize;        // size of data supplied

    switch (Id) {

    case KSPROPERTY_CROSSBAR_CAPS:                  // R 
    {
        PKSPROPERTY_CROSSBAR_CAPS_S  pCaps = 
            (PKSPROPERTY_CROSSBAR_CAPS_S)pSPD->PropertyInfo;   

        if (nS < sizeof (KSPROPERTY_CROSSBAR_CAPS_S))
            break;

        // Copy the input property info to the output property info
        RtlCopyMemory(  pCaps, 
                        pSPD->Property, 
                        sizeof (KSPROPERTY_CROSSBAR_CAPS_S));

        pCaps->NumberOfInputs  = NUMBER_OF_XBAR_INPUTS;
        pCaps->NumberOfOutputs = NUMBER_OF_XBAR_OUTPUTS;

        pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_CROSSBAR_CAPS_S);
    }
    break;


    case KSPROPERTY_CROSSBAR_CAN_ROUTE:                   // R 
    {
        PKSPROPERTY_CROSSBAR_ROUTE_S  pRoute = 
            (PKSPROPERTY_CROSSBAR_ROUTE_S)pSPD->PropertyInfo;   

        if (nS < sizeof (KSPROPERTY_CROSSBAR_ROUTE_S))
            break;

        // Copy the input property info to the output property info
        RtlCopyMemory(  pRoute, 
                        pSPD->Property, 
                        sizeof (KSPROPERTY_CROSSBAR_ROUTE_S));

        // Special case, audio output routed to (-1) means mute
        if (pRoute->IndexOutputPin == 1 && pRoute->IndexInputPin == -1) {
            pRoute->CanRoute = TRUE;
        }
        else if ((pRoute->IndexInputPin  >= NUMBER_OF_XBAR_INPUTS) ||
            (pRoute->IndexOutputPin >= NUMBER_OF_XBAR_OUTPUTS)) {

            pRoute->CanRoute = FALSE;
        }
        else if ((pRoute->IndexInputPin <= 4) &&
            (pRoute->IndexOutputPin == 0) ||
            (pRoute->IndexInputPin >= 5) &&
            (pRoute->IndexOutputPin == 1)) {

            // This driver allows any video input to connect to any video output
            // and any audio input to connect to any audio output
            pRoute->CanRoute = TRUE;
        }
        else {
            pRoute->CanRoute = FALSE;
        }
        pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_CROSSBAR_ROUTE_S);
    }
    break;


    case KSPROPERTY_CROSSBAR_PININFO:                     // R
    { 
        PKSPROPERTY_CROSSBAR_PININFO_S  pPinInfo = 
            (PKSPROPERTY_CROSSBAR_PININFO_S)pSPD->PropertyInfo;   

        if (nS < sizeof (KSPROPERTY_CROSSBAR_PININFO_S))
            break;

        // Copy the input property info to the output property info
        RtlCopyMemory(  pPinInfo, 
                        pSPD->Property, 
                        sizeof (KSPROPERTY_CROSSBAR_PININFO_S));

        if (pPinInfo->Direction == KSPIN_DATAFLOW_IN) {

            ASSERT (pPinInfo->Index < NUMBER_OF_XBAR_INPUTS);
            if (pPinInfo->Index >= NUMBER_OF_XBAR_INPUTS) {
                pSrb->Status = STATUS_INVALID_PARAMETER;
                break;
            }

            pPinInfo->PinType          = XBarInputPins[pPinInfo->Index].PinType;
            pPinInfo->RelatedPinIndex  = XBarInputPins[pPinInfo->Index].RelatedPinIndex;
            pPinInfo->Medium           = *XBarInputPins[pPinInfo->Index].Medium;
        }
        else {

            ASSERT (pPinInfo->Index < NUMBER_OF_XBAR_OUTPUTS);
            if (pPinInfo->Index >= NUMBER_OF_XBAR_OUTPUTS) {
                pSrb->Status = STATUS_INVALID_PARAMETER;
                break;
            }

            pPinInfo->PinType          = XBarOutputPins[pPinInfo->Index].PinType;
            pPinInfo->RelatedPinIndex  = XBarOutputPins[pPinInfo->Index].RelatedPinIndex;
            pPinInfo->Medium           = *XBarOutputPins[pPinInfo->Index].Medium;
        }

        pPinInfo->Medium.Id = pHwDevExt->DriverMediumInstanceCount;  // Multiple instance support

        pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_CROSSBAR_PININFO_S);
    }
    break;


    case KSPROPERTY_CROSSBAR_ROUTE:                   // R 
    {
        PKSPROPERTY_CROSSBAR_ROUTE_S  pRoute = 
            (PKSPROPERTY_CROSSBAR_ROUTE_S)pSPD->PropertyInfo;   

        if (nS < sizeof (KSPROPERTY_CROSSBAR_ROUTE_S))
            break;

        // Copy the input property info to the output property info
        RtlCopyMemory(  pRoute, 
                        pSPD->Property, 
                        sizeof (KSPROPERTY_CROSSBAR_ROUTE_S));

        // Sanity check
        if (pRoute->IndexOutputPin >= NUMBER_OF_XBAR_OUTPUTS) {
            pRoute->CanRoute = FALSE;
        }
        // querying the the video output pin
        else if (pRoute->IndexOutputPin == 0) {
            pRoute->IndexInputPin = pHwDevExt->VideoInputConnected;
            pRoute->CanRoute = TRUE;
        }
        // querying the the audio output pin
        else if (pRoute->IndexOutputPin == 1) {
            pRoute->IndexInputPin = pHwDevExt->AudioInputConnected;
            pRoute->CanRoute = TRUE;
        }
        pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_CROSSBAR_ROUTE_S);
    }
    break;


    default:
        TRAP;
        break;
    }
}

// -------------------------------------------------------------------
// TVTuner Property Set functions
// -------------------------------------------------------------------

/*
** AdapterSetTunerProperty ()
**
**    Handles Set operations on the TvTuner property set.
**      Testcap uses this for demo purposes only.
**
** Arguments:
**
**      pSRB -
**          Pointer to the HW_STREAM_REQUEST_BLOCK 
**
** Returns:
**
** Side Effects:  none
*/

VOID 
STREAMAPI
AdapterSetTunerProperty(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PHW_DEVICE_EXTENSION pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
    ULONG Id = pSPD->Property->Id;              // index of the property
    ULONG nS = pSPD->PropertyOutputSize;        // size of data supplied

    switch (Id) {

    case KSPROPERTY_TUNER_MODE:
    {
         PKSPROPERTY_TUNER_MODE_S pMode =
            (PKSPROPERTY_TUNER_MODE_S)pSPD->Property;
         ASSERT (pMode->Mode & (KSPROPERTY_TUNER_MODE_TV       |
                                KSPROPERTY_TUNER_MODE_AM_RADIO |
                                KSPROPERTY_TUNER_MODE_FM_RADIO |
                                KSPROPERTY_TUNER_MODE_ATSC));
         pHwDevExt->TunerMode = pMode->Mode;
    }
    break;

    case KSPROPERTY_TUNER_STANDARD:
    {
        PKSPROPERTY_TUNER_STANDARD_S pStandard_S = 
            (PKSPROPERTY_TUNER_STANDARD_S) pSPD->Property;
        pHwDevExt->VideoStandard = pStandard_S->Standard;
    }
    break;

    case KSPROPERTY_TUNER_FREQUENCY:
    {
        PKSPROPERTY_TUNER_FREQUENCY_S pFreq_S = 
            (PKSPROPERTY_TUNER_FREQUENCY_S) pSPD->Property;
        pHwDevExt->Frequency = pFreq_S->Frequency;
        pHwDevExt->Country = pFreq_S->Country;
        pHwDevExt->Channel = pFreq_S->Channel;
    }
    break;

    case KSPROPERTY_TUNER_INPUT:
    {
        PKSPROPERTY_TUNER_INPUT_S pInput_S = 
            (PKSPROPERTY_TUNER_INPUT_S) pSPD->Property;
        pHwDevExt->TunerInput = pInput_S->InputIndex;
    }
    break;

    default:
        TRAP;
        break;
    }
}

/*
** AdapterGetTunerProperty ()
**
**    Handles Get operations on the TvTuner property set.
**      Testcap uses this for demo purposes only.
**
** Arguments:
**
**      pSRB -
**          Pointer to the HW_STREAM_REQUEST_BLOCK 
**
** Returns:
**
** Side Effects:  none
*/

VOID 
STREAMAPI
AdapterGetTunerProperty(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PHW_DEVICE_EXTENSION pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
    ULONG Id = pSPD->Property->Id;              // index of the property
    ULONG nS = pSPD->PropertyOutputSize;        // size of data supplied
    PVOID pV = pSPD->PropertyInfo;              // pointer to the output data

    ASSERT (nS >= sizeof (LONG));

    switch (Id) {

    case KSPROPERTY_TUNER_CAPS:
    {
         PKSPROPERTY_TUNER_CAPS_S pCaps =
            (PKSPROPERTY_TUNER_CAPS_S)pSPD->Property;
         ASSERT (nS >= sizeof( KSPROPERTY_TUNER_CAPS_S ) );

         // now work with the output buffer
         pCaps =(PKSPROPERTY_TUNER_CAPS_S)pV;

        // Copy the input property info to the output property info
        RtlCopyMemory(  pCaps, 
                        pSPD->Property, 
                        sizeof (KSPROPERTY_TUNER_CAPS_S));

         pCaps->ModesSupported = 
             KSPROPERTY_TUNER_MODE_TV       |
             KSPROPERTY_TUNER_MODE_FM_RADIO |
             KSPROPERTY_TUNER_MODE_AM_RADIO |
             KSPROPERTY_TUNER_MODE_ATSC;

         pCaps->VideoMedium = TVTunerMediums[0];
         pCaps->VideoMedium.Id = pHwDevExt->DriverMediumInstanceCount;  // Multiple instance support

         pCaps->TVAudioMedium = TVTunerMediums[1];
         pCaps->TVAudioMedium.Id = pHwDevExt->DriverMediumInstanceCount;  // Multiple instance support

         pCaps->RadioAudioMedium = TVTunerMediums[2];   // No separate radio audio pin?
         pCaps->RadioAudioMedium.Id = pHwDevExt->DriverMediumInstanceCount;  // Multiple instance support

         pSrb->ActualBytesTransferred = sizeof( KSPROPERTY_TUNER_CAPS_S );
    }
    break;

    case KSPROPERTY_TUNER_MODE:
    {
        PKSPROPERTY_TUNER_MODE_S pMode =
            (PKSPROPERTY_TUNER_MODE_S)pSPD->Property;
        ASSERT (nS >= sizeof( KSPROPERTY_TUNER_MODE_S ) );

        // now work with the output buffer
        pMode =(PKSPROPERTY_TUNER_MODE_S)pV;

        // Copy the input property info to the output property info
        RtlCopyMemory(  pMode, 
                        pSPD->Property, 
                        sizeof (KSPROPERTY_TUNER_MODE_S));

        pMode->Mode = pHwDevExt->TunerMode;

        pSrb->ActualBytesTransferred = sizeof( KSPROPERTY_TUNER_MODE_S);
    }
    break;

    case KSPROPERTY_TUNER_MODE_CAPS:
    {
        PKSPROPERTY_TUNER_MODE_CAPS_S pCaps = 
                (PKSPROPERTY_TUNER_MODE_CAPS_S) pSPD->Property;

        ASSERT (nS >= sizeof (KSPROPERTY_TUNER_MODE_CAPS_S));

        // now work with the output buffer
        pCaps = (PKSPROPERTY_TUNER_MODE_CAPS_S) pV;

        // Copy the input property info to the output property info
        RtlCopyMemory(  pCaps, 
                        pSPD->Property, 
                        sizeof (KSPROPERTY_TUNER_MODE_CAPS_S));

        pCaps->Mode = ((PKSPROPERTY_TUNER_MODE_CAPS_S) pSPD->Property)->Mode;
        pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_TUNER_MODE_CAPS_S);

        switch (pCaps->Mode) {
        
        case KSPROPERTY_TUNER_MODE_TV:
        case KSPROPERTY_TUNER_MODE_ATSC:
            // List the formats actually supported by the tuner in this mode
            pCaps->StandardsSupported = 
                (pHwDevExt->TunerMode == KSPROPERTY_TUNER_MODE_ATSC) ?
                       KS_AnalogVideo_NTSC_M 
                :
                       KS_AnalogVideo_NTSC_M
    
                    |  KS_AnalogVideo_PAL_B    
                    |  KS_AnalogVideo_PAL_D    
                //  |  KS_AnalogVideo_PAL_H    
                //  |  KS_AnalogVideo_PAL_I    
                    |  KS_AnalogVideo_PAL_M    
                    |  KS_AnalogVideo_PAL_N    
                    |  KS_AnalogVideo_PAL_N_COMBO
    
                //  |  KS_AnalogVideo_SECAM_B  
                //  |  KS_AnalogVideo_SECAM_D  
                //  |  KS_AnalogVideo_SECAM_G  
                //  |  KS_AnalogVideo_SECAM_H  
                //  |  KS_AnalogVideo_SECAM_K  
                //  |  KS_AnalogVideo_SECAM_K1 
                //  |  KS_AnalogVideo_SECAM_L  
                    ;
    
            //
            // Get the min and max frequencies supported
            //
    
            pCaps->MinFrequency =  55250000L;
            pCaps->MaxFrequency = 997250000L;
    
            // What is the frequency step size?
            pCaps->TuningGranularity =  62500L;
    
            // How many inputs are on the tuner?
            pCaps->NumberOfInputs = 1;
    
            // What is the maximum settling time in milliseconds?
            pCaps->SettlingTime = 100;
        
            //
            // Strategy defines how the tuner knows when it is in tune:
            // 
            // KS_TUNER_STRATEGY_PLL (Has PLL offset information)
            // KS_TUNER_STRATEGY_SIGNAL_STRENGTH (has signal strength info)
            // KS_TUNER_STRATEGY_DRIVER_TUNES (driver handles all fine tuning)
            //
            pCaps->Strategy = KS_TUNER_STRATEGY_PLL;
            break;

        case KSPROPERTY_TUNER_MODE_FM_RADIO:
            pCaps->StandardsSupported = 0;
            pCaps->MinFrequency =  88100000L;
            pCaps->MaxFrequency = 107900000L;

            // What is the frequency step size?
            pCaps->TuningGranularity =  200000L;
    
            // How many inputs are on the tuner?
            pCaps->NumberOfInputs = 1;
    
            // What is the maximum settling time in milliseconds?
            pCaps->SettlingTime = 100;
            // Strategy defines how the tuner knows when it is in tune:
            pCaps->Strategy = KS_TUNER_STRATEGY_DRIVER_TUNES;
            break;

        case KSPROPERTY_TUNER_MODE_AM_RADIO:
            pCaps->StandardsSupported = 0;
            pCaps->MinFrequency =  540000L;
            pCaps->MaxFrequency = 1700000L;

            // What is the frequency step size?
            pCaps->TuningGranularity =  1000L;
    
            // How many inputs are on the tuner?
            pCaps->NumberOfInputs = 1;
    
            // What is the maximum settling time in milliseconds?
            pCaps->SettlingTime = 100;

            // Strategy defines how the tuner knows when it is in tune:
            pCaps->Strategy = KS_TUNER_STRATEGY_DRIVER_TUNES;
            break;

        default:
            ASSERT (FALSE);
            break;
        }
    }
    break;
        
    case KSPROPERTY_TUNER_STANDARD:
    {
        // What is the currently selected video standard?

        // Copy the input property info to the output property info
        RtlCopyMemory(  pSPD->PropertyInfo, 
                        pSPD->Property, 
                        sizeof (KSPROPERTY_TUNER_STANDARD_S));

        ((PKSPROPERTY_TUNER_STANDARD_S) pSPD->PropertyInfo)->Standard =
                pHwDevExt->VideoStandard;

        pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_TUNER_STANDARD_S);
    }
    break;

    case KSPROPERTY_TUNER_INPUT:
    {
        // What is the currently selected input?

        // Copy the input property info to the output property info
        RtlCopyMemory(  pSPD->PropertyInfo,
                        pSPD->Property, 
                        sizeof (KSPROPERTY_TUNER_INPUT_S));

        ((PKSPROPERTY_TUNER_INPUT_S) pSPD->PropertyInfo)->InputIndex = 
                pHwDevExt->TunerInput;

        pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_TUNER_INPUT_S);
    }
    break;


    case KSPROPERTY_TUNER_STATUS:

        // Return the status of the tuner

        // PLLOffset is in units of TuningGranularity 
        // SignalStrength is 0 to 100
        // Set Busy to 1 if tuning is still in process

        {
            PKSPROPERTY_TUNER_STATUS_S pStatus = 
                        (PKSPROPERTY_TUNER_STATUS_S) pSPD->PropertyInfo;

            ASSERT (nS >= sizeof (KSPROPERTY_TUNER_STATUS_S));

            // Copy the input property info to the output property info
            RtlCopyMemory(  pStatus, 
                            pSPD->Property, 
                            sizeof (KSPROPERTY_TUNER_STATUS_S));

            pStatus->CurrentFrequency = pHwDevExt->Frequency;
            pStatus->PLLOffset = 0;
            pStatus->SignalStrength = 100;
            pStatus->Busy = pHwDevExt->Busy;

            pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_TUNER_STATUS_S);
        }
        break;

    case KSPROPERTY_TUNER_IF_MEDIUM:
    {
        // Only Digital TV tuners should support this property
        PKSPROPERTY_TUNER_IF_MEDIUM_S pMedium =
           (PKSPROPERTY_TUNER_IF_MEDIUM_S)pSPD->Property;
        ASSERT (nS >= sizeof( KSPROPERTY_TUNER_IF_MEDIUM_S) );

        // now work with the output buffer
        pMedium =(PKSPROPERTY_TUNER_IF_MEDIUM_S)pV;

        // Copy the input property info to the output property info
        RtlCopyMemory(  pMedium, 
                        pSPD->Property, 
                        sizeof (KSPROPERTY_TUNER_IF_MEDIUM_S));

        pMedium->IFMedium = TVTunerMediums[3];
        pMedium->IFMedium.Id = pHwDevExt->DriverMediumInstanceCount;  // Multiple instance support

        pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_TUNER_IF_MEDIUM_S);
    }
    break;

    default:
        TRAP;
        break;
    }
}

// -------------------------------------------------------------------
// VideoProcAmp functions
// -------------------------------------------------------------------

/*
** AdapterSetVideoProcAmpProperty ()
**
**    Handles Set operations on the VideoProcAmp property set.
**      Testcap uses this for demo purposes only.
**
** Arguments:
**
**      pSRB -
**          Pointer to the HW_STREAM_REQUEST_BLOCK 
**
** Returns:
**
** Side Effects:  none
*/

VOID 
STREAMAPI
AdapterSetVideoProcAmpProperty(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PHW_DEVICE_EXTENSION pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
    ULONG Id = pSPD->Property->Id;              // index of the property
    PKSPROPERTY_VIDEOPROCAMP_S pS = (PKSPROPERTY_VIDEOPROCAMP_S) pSPD->PropertyInfo;

    ASSERT (pSPD->PropertyInputSize >= sizeof (KSPROPERTY_VIDEOPROCAMP_S));

    switch (Id) {

    case KSPROPERTY_VIDEOPROCAMP_BRIGHTNESS:
        pHwDevExt->Brightness = pS->Value;
        pHwDevExt->BrightnessFlags = pS->Flags;
        break;
        
    case KSPROPERTY_VIDEOPROCAMP_CONTRAST:
        pHwDevExt->Contrast = pS->Value;
        pHwDevExt->ContrastFlags = pS->Flags;
        break;

    case KSPROPERTY_VIDEOPROCAMP_COLORENABLE:
        pHwDevExt->ColorEnable = pS->Value;
        pHwDevExt->ColorEnableFlags = pS->Flags;
        break;

    default:
        TRAP;
        break;
    }
}

/*
** AdapterGetVideoProcAmpProperty ()
**
**    Handles Get operations on the VideoProcAmp property set.
**      Testcap uses this for demo purposes only.
**
** Arguments:
**
**      pSRB -
**          Pointer to the HW_STREAM_REQUEST_BLOCK 
**
** Returns:
**
** Side Effects:  none
*/

VOID 
STREAMAPI
AdapterGetVideoProcAmpProperty(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PHW_DEVICE_EXTENSION pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
    ULONG Id = pSPD->Property->Id;              // index of the property
    PKSPROPERTY_VIDEOPROCAMP_S pS = (PKSPROPERTY_VIDEOPROCAMP_S) pSPD->PropertyInfo;  

    ASSERT (pSPD->PropertyOutputSize >= sizeof (KSPROPERTY_VIDEOPROCAMP_S));

    // Copy the input property info to the output property info
    RtlCopyMemory(  pS,
                    pSPD->Property, 
                    sizeof (KSPROPERTY_VIDEOPROCAMP_S));

    switch (Id) {

    case KSPROPERTY_VIDEOPROCAMP_BRIGHTNESS:
        pS->Value = pHwDevExt->Brightness;
        pS->Flags = pHwDevExt->BrightnessFlags;
        pS->Capabilities = KSPROPERTY_VIDEOPROCAMP_FLAGS_MANUAL | 
                           KSPROPERTY_VIDEOPROCAMP_FLAGS_AUTO;
        break;
        
    case KSPROPERTY_VIDEOPROCAMP_CONTRAST:
        pS->Value = pHwDevExt->Contrast;
        pS->Flags = pHwDevExt->ContrastFlags;
        pS->Capabilities = KSPROPERTY_VIDEOPROCAMP_FLAGS_MANUAL | 
                           KSPROPERTY_VIDEOPROCAMP_FLAGS_AUTO;
        break;

    case KSPROPERTY_VIDEOPROCAMP_COLORENABLE:
        pS->Value = pHwDevExt->ColorEnable;
        pS->Flags = pHwDevExt->ColorEnableFlags;
        pS->Capabilities = KSPROPERTY_VIDEOPROCAMP_FLAGS_MANUAL;
        break;

    default:
        TRAP;
        break;
    }
    pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_VIDEOPROCAMP_S);
}

// -------------------------------------------------------------------
// CameraControl functions
// -------------------------------------------------------------------

/*
** AdapterSetCameraControlProperty ()
**
**    Handles Set operations on the CameraControl property set.
**      Testcap uses this for demo purposes only.
**
** Arguments:
**
**      pSRB -
**          Pointer to the HW_STREAM_REQUEST_BLOCK 
**
** Returns:
**
** Side Effects:  none
*/

VOID 
STREAMAPI
AdapterSetCameraControlProperty(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PHW_DEVICE_EXTENSION pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
    ULONG Id = pSPD->Property->Id;              // index of the property
    PKSPROPERTY_CAMERACONTROL_S pS = (PKSPROPERTY_CAMERACONTROL_S) pSPD->PropertyInfo;

    ASSERT (pSPD->PropertyInputSize >= sizeof (KSPROPERTY_CAMERACONTROL_S));

    switch (Id) {

    case KSPROPERTY_CAMERACONTROL_ZOOM:
        pHwDevExt->Zoom = pS->Value;
        pHwDevExt->ZoomFlags = pS->Flags;
        break;
        
    case KSPROPERTY_CAMERACONTROL_FOCUS:
        pHwDevExt->Focus = pS->Value;
        pHwDevExt->FocusFlags = pS->Flags;
        break;

    default:
        TRAP;
        break;
    }
}

/*
** AdapterGetCameraControlProperty ()
**
**    Handles Get operations on the CameraControl property set.
**      Testcap uses this for demo purposes only.
**
** Arguments:
**
**      pSRB -
**          Pointer to the HW_STREAM_REQUEST_BLOCK 
**
** Returns:
**
** Side Effects:  none
*/

VOID 
STREAMAPI
AdapterGetCameraControlProperty(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PHW_DEVICE_EXTENSION pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
    ULONG Id = pSPD->Property->Id;              // index of the property
    PKSPROPERTY_CAMERACONTROL_S pS = (PKSPROPERTY_CAMERACONTROL_S) pSPD->PropertyInfo;    // pointer to the output data

    ASSERT (pSPD->PropertyOutputSize >= sizeof (KSPROPERTY_CAMERACONTROL_S));

    // Copy the input property info to the output property info
    RtlCopyMemory(  pS,
                    pSPD->Property, 
                    sizeof (KSPROPERTY_CAMERACONTROL_S));

    switch (Id) {

    case KSPROPERTY_CAMERACONTROL_ZOOM:
        pS->Value = pHwDevExt->Zoom;
        pS->Flags = pHwDevExt->ZoomFlags;
        pS->Capabilities = KSPROPERTY_CAMERACONTROL_FLAGS_MANUAL | 
                           KSPROPERTY_CAMERACONTROL_FLAGS_AUTO;
        break;
        
    case KSPROPERTY_CAMERACONTROL_FOCUS:
        pS->Value = pHwDevExt->Focus;
        pS->Flags = pHwDevExt->FocusFlags;
        pS->Capabilities = KSPROPERTY_CAMERACONTROL_FLAGS_MANUAL | 
                           KSPROPERTY_CAMERACONTROL_FLAGS_AUTO;
        break;

    default:
        TRAP;
        break;
    }
    pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_CAMERACONTROL_S);
}

// -------------------------------------------------------------------
// TVAudio functions
// -------------------------------------------------------------------

/*
** AdapterSetTVAudioProperty ()
**
**    Handles Set operations on the TVAudio property set.
**      Testcap uses this for demo purposes only.
**
** Arguments:
**
**      pSRB -
**          Pointer to the HW_STREAM_REQUEST_BLOCK 
**
** Returns:
**
** Side Effects:  none
*/

VOID 
STREAMAPI
AdapterSetTVAudioProperty(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PHW_DEVICE_EXTENSION pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
    ULONG Id = pSPD->Property->Id;              // index of the property


    switch (Id) {

    case KSPROPERTY_TVAUDIO_MODE:
    {
        PKSPROPERTY_TVAUDIO_S pS = (PKSPROPERTY_TVAUDIO_S) pSPD->PropertyInfo;    

        pHwDevExt->TVAudioMode = pS->Mode;
    }
    break;

    default:
        TRAP;
        break;
    }
}

/*
** AdapterGetTVAudioProperty ()
**
**    Handles Get operations on the TVAudio property set.
**      Testcap uses this for demo purposes only.
**
** Arguments:
**
**      pSRB -
**          Pointer to the HW_STREAM_REQUEST_BLOCK 
**
** Returns:
**
** Side Effects:  none
*/

VOID 
STREAMAPI
AdapterGetTVAudioProperty(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PHW_DEVICE_EXTENSION pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
    ULONG Id = pSPD->Property->Id;              // index of the property

    switch (Id) {

    case KSPROPERTY_TVAUDIO_CAPS:
    {
        PKSPROPERTY_TVAUDIO_CAPS_S pS = (PKSPROPERTY_TVAUDIO_CAPS_S) pSPD->PropertyInfo;    // pointer to the data

        ASSERT (pSPD->PropertyOutputSize >= sizeof (KSPROPERTY_TVAUDIO_CAPS_S));

        // Copy the input property info to the output property info
        RtlCopyMemory(  pS, 
                        pSPD->Property, 
                        sizeof (KSPROPERTY_TVAUDIO_CAPS_S));
        
        pS->InputMedium  = TVAudioMediums[0];
        pS->InputMedium.Id = pHwDevExt->DriverMediumInstanceCount;  // Multiple instance support
        pS->OutputMedium = TVAudioMediums[1];
        pS->OutputMedium.Id = pHwDevExt->DriverMediumInstanceCount;  // Multiple instance support

        // Report all of the possible audio decoding modes the hardware is capabable of
        pS->Capabilities = KS_TVAUDIO_MODE_MONO   |
                           KS_TVAUDIO_MODE_STEREO |
                           KS_TVAUDIO_MODE_LANG_A |
                           KS_TVAUDIO_MODE_LANG_B ;

        pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_TVAUDIO_CAPS_S);
    }
    break;
        
    case KSPROPERTY_TVAUDIO_MODE:
    {
        PKSPROPERTY_TVAUDIO_S pS = (PKSPROPERTY_TVAUDIO_S) pSPD->PropertyInfo;    // pointer to the data

        ASSERT (pSPD->PropertyOutputSize >= sizeof (KSPROPERTY_TVAUDIO_S));

        // Copy the input property info to the output property info
        RtlCopyMemory(  pS, 
                        pSPD->Property, 
                        sizeof (KSPROPERTY_TVAUDIO_S));

        // Report the currently selected mode
        pS->Mode = pHwDevExt->TVAudioMode;

        pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_TVAUDIO_S);
    }
    break;

    case KSPROPERTY_TVAUDIO_CURRENTLY_AVAILABLE_MODES:
    {
        PKSPROPERTY_TVAUDIO_S pS = (PKSPROPERTY_TVAUDIO_S) pSPD->PropertyInfo;    // pointer to the data

        ASSERT (pSPD->PropertyOutputSize >= sizeof (KSPROPERTY_TVAUDIO_S));

        // Copy the input property info to the output property info
        RtlCopyMemory(  pS,
                        pSPD->Property, 
                        sizeof (KSPROPERTY_TVAUDIO_S));

        // Report which audio modes could potentially be selected right now
        pS->Mode = KS_TVAUDIO_MODE_MONO   |
                   KS_TVAUDIO_MODE_STEREO |
                   KS_TVAUDIO_MODE_LANG_A ;

        pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_TVAUDIO_S);
    }
    break;
    
    default:
        TRAP;
        break;
    }
}

// -------------------------------------------------------------------
// AnalogVideoDecoder functions
// -------------------------------------------------------------------

/*
** AdapterSetAnalogVideoDecoderProperty ()
**
**    Handles Set operations on the AnalogVideoDecoder property set.
**      Testcap uses this for demo purposes only.
**
** Arguments:
**
**      pSRB -
**          Pointer to the HW_STREAM_REQUEST_BLOCK 
**
** Returns:
**
** Side Effects:  none
*/

VOID 
STREAMAPI
AdapterSetAnalogVideoDecoderProperty(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PHW_DEVICE_EXTENSION pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
    ULONG Id = pSPD->Property->Id;              // index of the property
    PKSPROPERTY_VIDEODECODER_S pS = (PKSPROPERTY_VIDEODECODER_S) pSPD->PropertyInfo;

    ASSERT (pSPD->PropertyInputSize >= sizeof (KSPROPERTY_VIDEODECODER_S));
    
    switch (Id) {

    case KSPROPERTY_VIDEODECODER_STANDARD:
    {
        pHwDevExt->VideoDecoderVideoStandard = pS->Value;
    }
    break;

    case KSPROPERTY_VIDEODECODER_OUTPUT_ENABLE:
    {
        pHwDevExt->VideoDecoderOutputEnable = pS->Value;
    }
    break;

    case KSPROPERTY_VIDEODECODER_VCR_TIMING:
    {
        pHwDevExt->VideoDecoderVCRTiming = pS->Value;
    }
    break;

    default:
        TRAP;
        break;
    }
}

/*
** AdapterGetAnalogVideoDecoderProperty ()
**
**    Handles Get operations on the AnalogVideoDecoder property set.
**      Testcap uses this for demo purposes only.
**
** Arguments:
**
**      pSRB -
**          Pointer to the HW_STREAM_REQUEST_BLOCK 
**
** Returns:
**
** Side Effects:  none
*/

VOID 
STREAMAPI
AdapterGetAnalogVideoDecoderProperty(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PHW_DEVICE_EXTENSION pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
    ULONG Id = pSPD->Property->Id;              // index of the property

    switch (Id) {

    case KSPROPERTY_VIDEODECODER_CAPS:
    {
        PKSPROPERTY_VIDEODECODER_CAPS_S pS = (PKSPROPERTY_VIDEODECODER_CAPS_S) pSPD->PropertyInfo;    // pointer to the data

        ASSERT (pSPD->PropertyOutputSize >= sizeof (KSPROPERTY_VIDEODECODER_CAPS_S));

        // Copy the input property info to the output property info
        RtlCopyMemory(  pS, 
                        pSPD->Property, 
                        sizeof (KSPROPERTY_VIDEODECODER_CAPS_S));
        
        pS->StandardsSupported =         
                   KS_AnalogVideo_NTSC_M

                |  KS_AnalogVideo_PAL_B    
                |  KS_AnalogVideo_PAL_D    
            //  |  KS_AnalogVideo_PAL_H    
            //  |  KS_AnalogVideo_PAL_I    
                |  KS_AnalogVideo_PAL_M    
                |  KS_AnalogVideo_PAL_N    

            //  |  KS_AnalogVideo_SECAM_B  
            //  |  KS_AnalogVideo_SECAM_D  
            //  |  KS_AnalogVideo_SECAM_G  
            //  |  KS_AnalogVideo_SECAM_H  
            //  |  KS_AnalogVideo_SECAM_K  
            //  |  KS_AnalogVideo_SECAM_K1 
            //  |  KS_AnalogVideo_SECAM_L  
                   ;

        pS->Capabilities = KS_VIDEODECODER_FLAGS_CAN_DISABLE_OUTPUT  |
                           KS_VIDEODECODER_FLAGS_CAN_USE_VCR_LOCKING |
                           KS_VIDEODECODER_FLAGS_CAN_INDICATE_LOCKED ;


        pS->SettlingTime = 10;          // How long to delay after tuning before 
                                        // Locked indicator is valid
                                        
        pS->HSyncPerVSync = 6;          // HSync per VSync

        pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_VIDEODECODER_CAPS_S);
    }
    break;
        
    case KSPROPERTY_VIDEODECODER_STANDARD:
    {
        PKSPROPERTY_VIDEODECODER_S pS = (PKSPROPERTY_VIDEODECODER_S) pSPD->PropertyInfo;    // pointer to the data

        ASSERT (pSPD->PropertyOutputSize >= sizeof (KSPROPERTY_VIDEODECODER_S));

        // Copy the input property info to the output property info
        RtlCopyMemory(  pS, 
                        pSPD->Property, 
                        sizeof (KSPROPERTY_VIDEODECODER_S));

        pS->Value = pHwDevExt->VideoDecoderVideoStandard;

        pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_VIDEODECODER_S);
    }
    break;

    case KSPROPERTY_VIDEODECODER_STATUS:
    {
        PKSPROPERTY_VIDEODECODER_STATUS_S pS = (PKSPROPERTY_VIDEODECODER_STATUS_S) pSPD->PropertyInfo;    // pointer to the data

        ASSERT (pSPD->PropertyOutputSize >= sizeof (KSPROPERTY_VIDEODECODER_STATUS_S));

        // Copy the input property info to the output property info
        RtlCopyMemory(  pS, 
                        pSPD->Property, 
                        sizeof (KSPROPERTY_VIDEODECODER_STATUS_S));

        pS->NumberOfLines = (pHwDevExt->VideoDecoderVideoStandard & KS_AnalogVideo_NTSC_Mask)
                             ? 525 : 625;

        // Just to make things interesting, simulate that some channels aren't locked
        // In the US, these are channels 54 through 70
        pS->SignalLocked = (pHwDevExt->Frequency < 400000000 || pHwDevExt->Frequency > 500000000);

        pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_VIDEODECODER_S);
    }
    break;

    case KSPROPERTY_VIDEODECODER_OUTPUT_ENABLE:
    {
        PKSPROPERTY_VIDEODECODER_S pS = (PKSPROPERTY_VIDEODECODER_S) pSPD->PropertyInfo;    // pointer to the data

        ASSERT (pSPD->PropertyOutputSize >= sizeof (KSPROPERTY_VIDEODECODER_S));

        // Copy the input property info to the output property info
        RtlCopyMemory(  pS, 
                        pSPD->Property, 
                        sizeof (KSPROPERTY_VIDEODECODER_S));

        pS->Value = pHwDevExt->VideoDecoderOutputEnable;

        pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_VIDEODECODER_S);
    }
    break;

    case KSPROPERTY_VIDEODECODER_VCR_TIMING:
    {
        PKSPROPERTY_VIDEODECODER_S pS = (PKSPROPERTY_VIDEODECODER_S) pSPD->PropertyInfo;    // pointer to the data

        ASSERT (pSPD->PropertyOutputSize >= sizeof (KSPROPERTY_VIDEODECODER_S));

        // Copy the input property info to the output property info
        RtlCopyMemory(  pS, 
                        pSPD->Property, 
                        sizeof (KSPROPERTY_VIDEODECODER_S));

        pS->Value = pHwDevExt->VideoDecoderVCRTiming;

        pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_VIDEODECODER_S);
    }
    break;

    default:
        TRAP;
        break;
    }
}

// -------------------------------------------------------------------
// VideoControl functions
// -------------------------------------------------------------------

/*
** AdapterSetVideoControlProperty ()
**
**    Handles Set operations on the VideoControl property set.
**      Testcap uses this for demo purposes only.
**
** Arguments:
**
**      pSRB -
**          Pointer to the HW_STREAM_REQUEST_BLOCK 
**
** Returns:
**
** Side Effects:  none
*/

VOID 
STREAMAPI
AdapterSetVideoControlProperty(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PHW_DEVICE_EXTENSION pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
    ULONG Id = pSPD->Property->Id;              // index of the property
    PKSPROPERTY_VIDEOCONTROL_MODE_S pS = (PKSPROPERTY_VIDEOCONTROL_MODE_S) pSPD->PropertyInfo;
    PSTREAMEX pStrmEx;
    ULONG StreamIndex;
    ULONG *pVideoControlMode;

    // For this property set, the StreamIndex is always in the same place
    // for each property
    StreamIndex = ((PKSPROPERTY_VIDEOCONTROL_CAPS_S) pSPD->Property)->StreamIndex;

    ASSERT (StreamIndex < MAX_TESTCAP_STREAMS);

    // Verify the stream index is valid
    if (StreamIndex >= MAX_TESTCAP_STREAMS) {
        pSrb->Status = STATUS_INVALID_PARAMETER;
        return;
    }

    pStrmEx = (PSTREAMEX) pHwDevExt->pStrmEx[StreamIndex];

    // If the stream is not opened when this property set is used,
    // store the values in the HwDevExt

    if (pStrmEx) {
        pVideoControlMode = &pStrmEx->VideoControlMode;
    }
    else {
        pVideoControlMode = &pHwDevExt->VideoControlMode;
    }

    ASSERT (pSPD->PropertyInputSize >= sizeof (KSPROPERTY_VIDEOCONTROL_MODE_S));
    
    switch (Id) {

    case KSPROPERTY_VIDEOCONTROL_MODE:
    {
        *pVideoControlMode = pS->Mode;
    }
    break;

    default:
        TRAP;
        break;
    }
}

/*
** AdapterGetVideoControlProperty ()
**
**    Handles Get operations on the VideoControl property set.
**      Testcap uses this for demo purposes only.
**
** Arguments:
**
**      pSRB -
**          Pointer to the HW_STREAM_REQUEST_BLOCK 
**
** Returns:
**
** Side Effects:  none
*/

VOID 
STREAMAPI
AdapterGetVideoControlProperty(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PHW_DEVICE_EXTENSION pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
    ULONG Id = pSPD->Property->Id;              // index of the property
    PSTREAMEX pStrmEx;
    ULONG StreamIndex;
    ULONG *pVideoControlMode;

    // For this property set, the StreamIndex is always in the same place
    // for each property
    StreamIndex = ((PKSPROPERTY_VIDEOCONTROL_CAPS_S) pSPD->Property)->StreamIndex;

    ASSERT (StreamIndex < MAX_TESTCAP_STREAMS);

    // Verify the stream index is valid
    if (StreamIndex >= MAX_TESTCAP_STREAMS) {
        pSrb->Status = STATUS_INVALID_PARAMETER;
        return;
    }

    pStrmEx = (PSTREAMEX) pHwDevExt->pStrmEx[StreamIndex];

    // If the stream is not opened when this property set is used,
    // store the values in the HwDevExt

    if (pStrmEx) {
        pVideoControlMode = &pStrmEx->VideoControlMode;
    }
    else {
        pVideoControlMode = &pHwDevExt->VideoControlMode;
    }

    switch (Id) {

    case KSPROPERTY_VIDEOCONTROL_CAPS:
    {
        PKSPROPERTY_VIDEOCONTROL_CAPS_S pS = (PKSPROPERTY_VIDEOCONTROL_CAPS_S) pSPD->PropertyInfo;    // pointer to the data

        ASSERT (pSPD->PropertyOutputSize >= sizeof (KSPROPERTY_VIDEOCONTROL_CAPS_S));

        // Copy the input property info to the output property info
        RtlCopyMemory(  pS, 
                        pSPD->Property, 
                        sizeof (KSPROPERTY_VIDEOCONTROL_CAPS_S));
        
        pS->VideoControlCaps =    
              KS_VideoControlFlag_FlipHorizontal       
//            | KS_VideoControlFlag_FlipVertical         
//            | KS_VideoControlFlag_ExternalTriggerEnable
//            | KS_VideoControlFlag_Trigger              
            ;

        pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_VIDEOCONTROL_CAPS_S);
    }
    break;
        
    case KSPROPERTY_VIDEOCONTROL_ACTUAL_FRAME_RATE:
    {
        PKSPROPERTY_VIDEOCONTROL_ACTUAL_FRAME_RATE_S pS = 
            (PKSPROPERTY_VIDEOCONTROL_ACTUAL_FRAME_RATE_S) pSPD->PropertyInfo;    // pointer to the data

        ASSERT (pSPD->PropertyOutputSize >= sizeof (KSPROPERTY_VIDEOCONTROL_ACTUAL_FRAME_RATE_S));

        // Copy the input property info to the output property info
        RtlCopyMemory(  pS, 
                        pSPD->Property, 
                        sizeof (KSPROPERTY_VIDEOCONTROL_ACTUAL_FRAME_RATE_S));

        pS->CurrentActualFrameRate = 15;        // TODO: Implement the right rates in shipping drivers.
        pS->CurrentMaxAvailableFrameRate = 15;  // TODO: Implement the right rates in shipping drivers.
        

        pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_VIDEOCONTROL_ACTUAL_FRAME_RATE_S);
    }
    break;

    case KSPROPERTY_VIDEOCONTROL_FRAME_RATES:
    {
        // todo
    }
    break;

    case KSPROPERTY_VIDEOCONTROL_MODE:
    {
        PKSPROPERTY_VIDEOCONTROL_MODE_S pS = (PKSPROPERTY_VIDEOCONTROL_MODE_S) pSPD->PropertyInfo;    // pointer to the data

        ASSERT (pSPD->PropertyOutputSize >= sizeof (KSPROPERTY_VIDEOCONTROL_MODE_S));

        // Copy the input property info to the output property info
        RtlCopyMemory(  pS, 
                        pSPD->Property, 
                        sizeof (KSPROPERTY_VIDEOCONTROL_MODE_S));

        pS->Mode = *pVideoControlMode;

        pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_VIDEOCONTROL_MODE_S);
    }
    break;

    default:
        TRAP;
        break;
    }
}


/*
** AdapterGetVideoCompressionProperty()
**
**    Gets compressor settings
**
** Arguments:
**
**    pSrb - pointer to the stream request block for properties
**
** Returns:
**
** Side Effects:  none
*/

VOID
STREAMAPI
AdapterGetVideoCompressionProperty(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PHW_DEVICE_EXTENSION pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    PSTREAMEX pStrmEx;
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
    ULONG Id = pSPD->Property->Id;              // index of the property
    ULONG StreamIndex;
    PCOMPRESSION_SETTINGS pCompressionSettings;

    // For this property set, the StreamIndex is always in the same place
    // for each property
    StreamIndex = ((PKSPROPERTY_VIDEOCOMPRESSION_S) pSPD->Property)->StreamIndex;

    ASSERT (StreamIndex < MAX_TESTCAP_STREAMS);

    // Verify the stream index is valid
    if (StreamIndex >= MAX_TESTCAP_STREAMS) {
        pSrb->Status = STATUS_INVALID_PARAMETER;
        return;
    }

    pStrmEx = (PSTREAMEX) pHwDevExt->pStrmEx[StreamIndex];

    // If the stream is not opened when this property set is used,
    // store the values in the HwDevExt

    if (pStrmEx) {
        pCompressionSettings = &pStrmEx->CompressionSettings;
    }
    else {
        pCompressionSettings = &pHwDevExt->CompressionSettings;
    }


    switch (Id) {

    case KSPROPERTY_VIDEOCOMPRESSION_GETINFO:
        {
            PKSPROPERTY_VIDEOCOMPRESSION_GETINFO_S pS = 
                (PKSPROPERTY_VIDEOCOMPRESSION_GETINFO_S) pSPD->PropertyInfo;

            // Copy the input property info to the output property info
            RtlCopyMemory(  pS, 
                            pSPD->Property, 
                            sizeof (KSPROPERTY_VIDEOCOMPRESSION_GETINFO_S));

            pS->DefaultKeyFrameRate = 15;    // Key frame rate
            pS->DefaultPFrameRate = 3;       // Predeicted frames per Key frame
            pS->DefaultQuality = 5000;       // 0 to 10000
            pS->Capabilities = 
                       KS_CompressionCaps_CanQuality  |
                       KS_CompressionCaps_CanKeyFrame |
                       KS_CompressionCaps_CanBFrame   ;
            
            pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_VIDEOCOMPRESSION_GETINFO_S);
        }
        break;

    case KSPROPERTY_VIDEOCOMPRESSION_KEYFRAME_RATE:
        {
            PKSPROPERTY_VIDEOCOMPRESSION_S pS = 
                (PKSPROPERTY_VIDEOCOMPRESSION_S) pSPD->PropertyInfo;

            // Copy the input property info to the output property info
            RtlCopyMemory(  pS, 
                            pSPD->Property, 
                            sizeof (KSPROPERTY_VIDEOCOMPRESSION_S));

            pS->Value = pCompressionSettings->CompressionKeyFrameRate;
                
            pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_VIDEOCOMPRESSION_S);
        }
        break;
    
    case KSPROPERTY_VIDEOCOMPRESSION_PFRAMES_PER_KEYFRAME:
        {
            PKSPROPERTY_VIDEOCOMPRESSION_S pS = 
                (PKSPROPERTY_VIDEOCOMPRESSION_S) pSPD->PropertyInfo;

            // Copy the input property info to the output property info
            RtlCopyMemory(  pS, 
                            pSPD->Property, 
                            sizeof (KSPROPERTY_VIDEOCOMPRESSION_S));

            pS->Value = pCompressionSettings->CompressionPFramesPerKeyFrame;
                
            pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_VIDEOCOMPRESSION_S);
        }
        break;
    
    case KSPROPERTY_VIDEOCOMPRESSION_QUALITY:
        {
            PKSPROPERTY_VIDEOCOMPRESSION_S pS = 
                (PKSPROPERTY_VIDEOCOMPRESSION_S) pSPD->PropertyInfo;

            // Copy the input property info to the output property info
            RtlCopyMemory(  pS, 
                            pSPD->Property, 
                            sizeof (KSPROPERTY_VIDEOCOMPRESSION_S));

            pS->Value = pCompressionSettings->CompressionQuality;
                
            pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_VIDEOCOMPRESSION_S);
        }
        break;
    
    default:
        TRAP;
        break;
    }
}

/*
** AdapterSetVideoCompressionProperty()
**
**    Sets compressor settings
**
** Arguments:
**
**    pSrb - pointer to the stream request block for properties
**
** Returns:
**
** Side Effects:  none
*/

VOID
STREAMAPI
AdapterSetVideoCompressionProperty(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PHW_DEVICE_EXTENSION pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    PSTREAMEX pStrmEx;
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
    PKSPROPERTY_VIDEOCOMPRESSION_S pS = (PKSPROPERTY_VIDEOCOMPRESSION_S) pSPD->Property;
    ULONG Id = pSPD->Property->Id;              // index of the property
    ULONG StreamIndex;
    PCOMPRESSION_SETTINGS pCompressionSettings;

    // For this property set, the StreamIndex is always in the same place
    // for each property
    StreamIndex = ((PKSPROPERTY_VIDEOCOMPRESSION_S) pSPD->Property)->StreamIndex;

    ASSERT (StreamIndex < MAX_TESTCAP_STREAMS);

    // Verify the stream index is valid
    if (StreamIndex >= MAX_TESTCAP_STREAMS) {
        pSrb->Status = STATUS_INVALID_PARAMETER;
        return;
    }

    pStrmEx = (PSTREAMEX) pHwDevExt->pStrmEx[StreamIndex];

    // If the stream is not opened when this property set is used,
    // store the values in the HwDevExt

    if (pStrmEx) {
        pCompressionSettings = &pStrmEx->CompressionSettings;
    }
    else {
        pCompressionSettings = &pHwDevExt->CompressionSettings;
    }

    switch (Id) {

    case KSPROPERTY_VIDEOCOMPRESSION_KEYFRAME_RATE:
        {
            pCompressionSettings->CompressionKeyFrameRate = pS->Value;
        }
        break;
    
    case KSPROPERTY_VIDEOCOMPRESSION_PFRAMES_PER_KEYFRAME:
        {
            pCompressionSettings->CompressionPFramesPerKeyFrame = pS->Value;
        }
        break;
    
    case KSPROPERTY_VIDEOCOMPRESSION_QUALITY:
        {
            pCompressionSettings->CompressionQuality = pS->Value;
        }
        break;

    default:
        TRAP;
        break;
    }
}


/*
** AdapterGetVBIProperty()
**
**    Gets VBI settings
**
** Arguments:
**
**    pSrb - pointer to the stream request block for properties
**
** Returns:
**
** Side Effects:  none
*/

VOID
STREAMAPI
AdapterGetVBIProperty(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PHW_DEVICE_EXTENSION pHwDevExt = pSrb->HwDeviceExtension;
    PSTREAMEX pStrmEx;
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
    ULONG Id = pSPD->Property->Id;              // index of the property
    ULONG StreamIndex;
    PVBICAP_PROPERTIES_PROTECTION_S pS = 
        (PVBICAP_PROPERTIES_PROTECTION_S) pSPD->PropertyInfo;

    StreamIndex = pS->StreamIndex;

    ASSERT (StreamIndex < MAX_TESTCAP_STREAMS);

    // Verify the stream index is valid
    if (StreamIndex >= MAX_TESTCAP_STREAMS) {
        pSrb->Status = STATUS_INVALID_PARAMETER;
        return;
    }

    // Copy the input property info to the output property info
    RtlCopyMemory(  pS, 
                    pSPD->Property, 
                    sizeof (*pS));

    pStrmEx = (PSTREAMEX) pHwDevExt->pStrmEx[StreamIndex];

    pS->Status = 0;
    if (pHwDevExt->ProtectionStatus & KS_VBI_FLAG_MV_PRESENT)
        pS->Status |= KS_VBICAP_PROTECTION_MV_PRESENT;
    if (pHwDevExt->ProtectionStatus & KS_VBI_FLAG_MV_HARDWARE)
        pS->Status |= KS_VBICAP_PROTECTION_MV_HARDWARE;
    if (pHwDevExt->ProtectionStatus & KS_VBI_FLAG_MV_DETECTED)
        pS->Status |= KS_VBICAP_PROTECTION_MV_DETECTED;
        
    pSrb->ActualBytesTransferred = sizeof (*pS);
}

#if DBG
/*
** AdapterSetVBIProperty()
**
**    Sets VBI settings
**
** Arguments:
**
**    pSrb - pointer to the stream request block for properties
**
** Returns:
**
** Side Effects:  none
*/

VOID
STREAMAPI
AdapterSetVBIProperty(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PHW_DEVICE_EXTENSION pHwDevExt = pSrb->HwDeviceExtension;
    PSTREAMEX pStrmEx;
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
    ULONG Id = pSPD->Property->Id;              // index of the property
    ULONG StreamIndex;
    PVBICAP_PROPERTIES_PROTECTION_S pS = 
        (PVBICAP_PROPERTIES_PROTECTION_S) pSPD->PropertyInfo;

    StreamIndex = pS->StreamIndex;

    ASSERT (StreamIndex < MAX_TESTCAP_STREAMS);

    // Verify the stream index is valid
    if (StreamIndex >= MAX_TESTCAP_STREAMS) {
        pSrb->Status = STATUS_INVALID_PARAMETER;
        return;
    }

    pStrmEx = (PSTREAMEX) pHwDevExt->pStrmEx[StreamIndex];

    pHwDevExt->ProtectionStatus = 0;
    if (pS->Status & KS_VBICAP_PROTECTION_MV_PRESENT)
        pHwDevExt->ProtectionStatus |= KS_VBI_FLAG_MV_PRESENT;
    if (pS->Status & KS_VBICAP_PROTECTION_MV_HARDWARE)
        pHwDevExt->ProtectionStatus |= KS_VBI_FLAG_MV_HARDWARE;
    if (pS->Status & KS_VBICAP_PROTECTION_MV_DETECTED)
        pHwDevExt->ProtectionStatus |= KS_VBI_FLAG_MV_DETECTED;
}
#endif //DBG


// -------------------------------------------------------------------
// General entry point for all get/set adapter properties
// -------------------------------------------------------------------

/*
** AdapterSetProperty ()
**
**    Handles Set operations for all adapter properties.
**
** Arguments:
**
**      pSRB -
**          Pointer to the HW_STREAM_REQUEST_BLOCK 
**
** Returns:
**
** Side Effects:  none
*/

VOID
STREAMAPI 
AdapterSetProperty(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )

{
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;

    if (IsEqualGUID(&PROPSETID_VIDCAP_CROSSBAR, &pSPD->Property->Set)) {
        AdapterSetCrossbarProperty (pSrb);
    }
    else if (IsEqualGUID(&PROPSETID_TUNER, &pSPD->Property->Set)) {
        AdapterSetTunerProperty (pSrb);
    }
    else if (IsEqualGUID(&PROPSETID_VIDCAP_VIDEOPROCAMP, &pSPD->Property->Set)) {
        AdapterSetVideoProcAmpProperty (pSrb);
    }
    else if (IsEqualGUID(&PROPSETID_VIDCAP_CAMERACONTROL, &pSPD->Property->Set)) {
        AdapterSetCameraControlProperty (pSrb);
    }
    else if (IsEqualGUID(&PROPSETID_VIDCAP_TVAUDIO, &pSPD->Property->Set)) {
        AdapterSetTVAudioProperty (pSrb);
    }
    else if (IsEqualGUID(&PROPSETID_VIDCAP_VIDEODECODER, &pSPD->Property->Set)) {
        AdapterSetAnalogVideoDecoderProperty (pSrb);
    }
    else if (IsEqualGUID(&PROPSETID_VIDCAP_VIDEOCONTROL, &pSPD->Property->Set)) {
        AdapterSetVideoControlProperty (pSrb);
    }
    else if (IsEqualGUID (&PROPSETID_VIDCAP_VIDEOCOMPRESSION, &pSPD->Property->Set)) {
        AdapterSetVideoCompressionProperty (pSrb);
    }
#if DBG
    // Can't normally set protection status; only allow this for DEBUGGING
    else if (IsEqualGUID (&KSPROPSETID_VBICAP_PROPERTIES, &pSPD->Property->Set)) {
        AdapterSetVBIProperty (pSrb);
    }
#endif //DBG
    else {
        //
        // We should never get here
        //

        TRAP;
        pSrb->Status = STATUS_NOT_IMPLEMENTED;
    }
}

/*
** AdapterGetProperty ()
**
**    Handles Get operations for all adapter properties.
**
** Arguments:
**
**      pSRB -
**          Pointer to the HW_STREAM_REQUEST_BLOCK 
**
** Returns:
**
** Side Effects:  none
*/

VOID
STREAMAPI 
AdapterGetProperty(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )

{
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;

    if (IsEqualGUID (&PROPSETID_VIDCAP_CROSSBAR, &pSPD->Property->Set)) {
        AdapterGetCrossbarProperty (pSrb);
    }
    else if (IsEqualGUID (&PROPSETID_TUNER, &pSPD->Property->Set)) {
        AdapterGetTunerProperty (pSrb);
    }
    else if (IsEqualGUID(&PROPSETID_VIDCAP_VIDEOPROCAMP, &pSPD->Property->Set)) {
        AdapterGetVideoProcAmpProperty (pSrb);
    }
    else if (IsEqualGUID(&PROPSETID_VIDCAP_CAMERACONTROL, &pSPD->Property->Set)) {
        AdapterGetCameraControlProperty (pSrb);
    }
    else if (IsEqualGUID(&PROPSETID_VIDCAP_TVAUDIO, &pSPD->Property->Set)) {
        AdapterGetTVAudioProperty (pSrb);
    }
    else if (IsEqualGUID(&PROPSETID_VIDCAP_VIDEODECODER, &pSPD->Property->Set)) {
        AdapterGetAnalogVideoDecoderProperty (pSrb);
    }
    else if (IsEqualGUID(&PROPSETID_VIDCAP_VIDEOCONTROL, &pSPD->Property->Set)) {
        AdapterGetVideoControlProperty (pSrb);
    }
    else if (IsEqualGUID (&PROPSETID_VIDCAP_VIDEOCOMPRESSION, &pSPD->Property->Set)) {
        AdapterGetVideoCompressionProperty (pSrb);
    }
    else if (IsEqualGUID (&KSPROPSETID_VBICAP_PROPERTIES, &pSPD->Property->Set)) {
        AdapterGetVBIProperty (pSrb);
    }
    else {
        //
        // We should never get here
        //

        TRAP;
        pSrb->Status = STATUS_NOT_IMPLEMENTED;
    }
}

