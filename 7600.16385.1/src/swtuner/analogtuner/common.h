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
    File    : common.h
    Abstract: Contains definitions common to several files
**************************************************************************/
#pragma warning (disable : 20051 26035 28101 28155 28170 28172 28718)

/*************************************************

    Standard Includes

*************************************************/

extern "C" {
#include <wdm.h>
}

#include <windef.h>
#include <stdio.h>
#include <stdlib.h>
#include <windef.h>
#define NOBITMAP
#include <mmreg.h>
#undef NOBITMAP
#include <unknown.h>
#include <ks.h>
#include <ksmedia.h>
#include <bdatypes.h>
#include <bdamedia.h>
#include <kcom.h>
#include "guids.h"
/*************************************************

    Misc Definitions

*************************************************/

#define ABS(x) ((x) < 0 ? (-(x)) : (x))
#define MS_SAMPLE_ANALOG_POOL_TAG 'AadB'

#define PS_BANDWIDTH 10 //Bandwidth in mega bits per second
#define FRAME_RATE 20

#define PS_PACKET_SIZE 2048 
#define PS_PACKETS_PER_SAMPLE 64
#define PS_SAMPLE_SIZE PS_PACKET_SIZE * PS_PACKETS_PER_SAMPLE
#define NANO_SECONDS 1000000000

#ifndef mmioFOURCC    
#define mmioFOURCC( ch0, ch1, ch2, ch3 )                \
        ( (DWORD)(BYTE)(ch0) | ( (DWORD)(BYTE)(ch1) << 8 ) |    \
        ( (DWORD)(BYTE)(ch2) << 16 ) | ( (DWORD)(BYTE)(ch3) << 24 ) )
#endif

#define FOURCC_YUV422       mmioFOURCC('U', 'Y', 'V', 'Y')

//
// ENCODER_OUT_PIN_DATA_RANGE_COUNT:
//
// The number of ranges supported on the encoder output pin.
//
#define ENCODER_OUT_PIN_DATA_RANGE_COUNT 1

//
// ENCODER_IN_PIN_DATA_RANGE_COUNT:
//
// The number of ranges supported on the encoder input pin.
//
#define ENCODER_IN_PIN_DATA_RANGE_COUNT 1

//
// ENCODER_FILTER_PIN_COUNT:
//
// The number of pins on the capture filter.
//
#define ENCODER_FILTER_PIN_COUNT 3

//
// ENCODER_FILTER_CATEGORIES_COUNT:
//
// The number of categories for the capture filter.
//
#define ENCODER_FILTER_CATEGORIES_COUNT 2

/*************************************************

    Enums / Typedefs

*************************************************/

typedef enum _HARDWARE_STATE {

    HardwareStopped = 0,
    HardwarePaused,
    HardwareRunning

} HARDWARE_STATE, *PHARDWARE_STATE;

/*************************************************

    Class Definitions

*************************************************/

//
// IHardwareSink:
//
// This interface is used by the hardware simulation to fake interrupt
// service routines.  The Interrupt method is called at DPC as a fake
// interrupt.
//
class IHardwareSink {

public:

    virtual
    void
    Interrupt (
        ) = 0;

};

//
// IEncoderSink:
//
// This is a capture sink interface.  The device level calls back the
// CompleteMappings method passing the number of completed mappings for
// the capture pin.  This method is called during the device DPC.
//
class IEncoderSink {

public:

    virtual
    void
    CompleteMappings (
        IN ULONG NumMappings
        ) = 0;

};

/*************************************************

    Internal Includes

*************************************************/
#include "commonx.h"
#include "TStream.h"
#include "hwsim.h"
#include "device.h"
#include "encoder.h"
#include "encoderpin.h"
#include "anlgevent.h"


