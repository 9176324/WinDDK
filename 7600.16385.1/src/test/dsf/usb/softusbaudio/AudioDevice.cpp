/*++ BUILD Version 0000

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    AudioDevice.cpp

Abstract:

    Implements the interface IAudioDevice,
    configures the audio device to be a valid USB device,
    responds to audio device requests from the host, and
    loops dequeuing isoch OUT data received from host
    
--*/

#include "stdafx.h"
#include <conio.h>
#include <stdio.h>
#include <USBProtocolDefs.h>
#include <dsfif.h>
#include <softusbif.h>
#include "AudioDevice.h"
#include "SoftUSBAudio_i.c"

// These are the indexes of the string descriptors. They are used both
// as the indexes of the strings with SoftUSBDevice.Strings and as the
// string descriptor index property values on the various objects (e.g.
// SoftUSBDevice.Manufacturer = STRING_IDX_MANUFACTURER).

#define STRING_IDX_MANUFACTURER                 1
#define STRING_IDX_PRODUCT_DESC                 2
#define STRING_IDX_SERIAL_NO                    0
#define STRING_IDX_CONFIG                       0
#define STRING_IDX_AUDIO_CTRL_INTERFACE         0
#define STRING_IDX_AUDIO_STREAMING_INTERFACE    0

// Audio class-specific device request types (from USB audio class spec
// table A-9):

#define REQUEST_CODE_UNDEFINED  0x00
#define SET_CUR                 0x01
#define GET_CUR                 0x81
#define SET_MIN                 0x02
#define GET_MIN                 0x82
#define SET_MAX                 0x03
#define GET_MAX                 0x83
#define SET_RES                 0x04
#define GET_RES                 0x84
#define SET_MEM                 0x05
#define GET_MEM                 0x85
#define GET_STAT                0xFF

// Audio control entity identifiers
#define USB_STREAMING_INPUT_TERMINAL    1
#define UPDOWN_MIX_PROCESSING_UNIT_2    2
#define UPDOWN_MIX_PROCESSING_UNIT_3    3
#define GRAPHIC_EQUALIZER_UNIT          4
#define LINE_CONNECTOR_INPUT_TERMINAL   5
#define LINE_CONNECTOR_INPUT_VOLUME     6
#define LINE_IN_AND_USB_MIXER_UNIT      7
#define COMBINED_OUTPUT_FEATURE_UNIT    8
#define DESKTOP_SPEAKERS                9
#define LOW_FREQ_MIXER_UNIT             10
#define LOW_FREQ_VOLUME_UNIT            11
#define LOW_FREQ_SPEAKERS               12

// Up/Down-mix Processing Unit Control Selectors
#define UD_CONTROL_UNDEFINED    0x00
#define UD_ENABLE_CONTROL       0x01
#define UD_MODE_SELECT_CONTROL  0x02


// Feature Unit Control IDS
#define FU_CONTROL_UNDEFINED        0x00
#define MUTE_CONTROL                0x01
#define VOLUME_CONTROL              0x02
#define BASS_CONTROL                0x03
#define MID_CONTROL                 0x04
#define TREBLE_CONTROL              0x05
#define GRAPHIC_EQUALIZER_CONTROL   0x06
#define AUTOMATIC_GAIN_CONTROL      0x07
#define DELAY_CONTROL               0x08
#define BASS_BOOST_CONTROL          0x09
#define LOUDNESS_CONTROL            0x0A

// Endpoint control selector
#define SAMPLING_FREQ_CONTROL       0x01
#define PITCH_CONTROL               0x02


// Feature Unit Control Selectors
#define FU_CONTROL_SELECTOR_UNDEFINED        0x000
#define MUTE_CONTROL_SELECTOR                0x001
#define VOLUME_CONTROL_SELECTOR              0x002
#define BASS_CONTROL_SELECTOR                0x004
#define MID_CONTROL_SELECTOR                 0x008
#define TREBLE_CONTROL_SELECTOR              0x010
#define GRAPHIC_EQUALIZER_CONTROL_SELECTOR   0x020
#define AUTOMATIC_GAIN_CONTROL_SELECTOR      0x040
#define DELAY_CONTROL_SELECTOR               0x080
#define BASS_BOOST_CONTROL_SELECTOR          0x100
#define LOUDNESS_CONTROL_SELECTOR            0x200

/* ***************************************************************************

The following is a dump of the audio control interface descriptor from
a real Microsoft DSS 80 device. The data structures that follow it
define the binary form of the descriptor that we'll use to configure
the SoftUSBDevice object.

Interface Descriptor:
bInterfaceNumber:     0x00
bAlternateSetting:    0x00
bNumEndpoints:        0x00
bInterfaceClass:      0x01 (Audio)
bInterfaceSubClass:   0x01 (Audio Control)
bInterfaceProtocol:   0x00
iInterface:           0x00

Audio Control Interface Header Descriptor:
bLength:              0x09
bDescriptorType:      0x24
bDescriptorSubtype:   0x01
bcdADC:             0x0100
wTotalLength:       0x00A2
bInCollection:        0x01
baInterfaceNr[1]:     0x01

Audio Control Input Terminal Descriptor:
bLength:              0x0C
bDescriptorType:      0x24
bDescriptorSubtype:   0x02
bTerminalID:          0x01
wTerminalType:      0x0101 (USB streaming)
bAssocTerminal:       0x00
bNrChannels:          0x02
wChannelConfig:     0x0003
iChannelNames:        0x00
iTerminal:            0x00

Audio Control Input Terminal Descriptor:
bLength:              0x0C
bDescriptorType:      0x24
bDescriptorSubtype:   0x02
bTerminalID:          0x05
wTerminalType:      0x0603 (Line connector)
bAssocTerminal:       0x00
bNrChannels:          0x02
wChannelConfig:     0x0003
iChannelNames:        0x00
iTerminal:            0x00

Audio Control Processing Unit Descriptor:
bLength:              0x12
bDescriptorType:      0x24
bDescriptorSubtype:   0x07
bUnitID:              0x02
wProcessType:       0x0001(Up / Down Mix Process)
bNrInPins:            0x01
baSourceID[1]:        0x01
bNrChannels:          0x04
wChannelConfig:     0x0107
iChannelNames:        0x00
bControlSize:         0x01
bmControls:
01 
iProcessing:          0x00
Process Specific:
01 07 01 

Audio Control Processing Unit Descriptor:
bLength:              0x12
bDescriptorType:      0x24
bDescriptorSubtype:   0x07
bUnitID:              0x03
wProcessType:       0x0001(Up / Down Mix Process)
bNrInPins:            0x01
baSourceID[1]:        0x02
bNrChannels:          0x02
wChannelConfig:     0x0003
iChannelNames:        0x00
bControlSize:         0x01
bmControls:
00 
iProcessing:          0x00
Process Specific:
01 03 00 

Audio Control Feature Unit Descriptor:
bLength:              0x0D
bDescriptorType:      0x24
bDescriptorSubtype:   0x06
bUnitID:              0x04
bSourceID:            0x03
bControlSize:         0x02
bmaControls[0]:
20 00 
bmaControls[1]:
00 00 
bmaControls[2]:
00 00 
iFeature:             0x00

Audio Control Feature Unit Descriptor:
bLength:              0x0D
bDescriptorType:      0x24
bDescriptorSubtype:   0x06
bUnitID:              0x06
bSourceID:            0x05
bControlSize:         0x02
bmaControls[0]:
02 00 
bmaControls[1]:
00 00 
bmaControls[2]:
00 00 
iFeature:             0x00

Audio Control Mixer Unit Descriptor:
bLength:              0x0D
bDescriptorType:      0x24
bDescriptorSubtype:   0x04
bUnitID:              0x07
bNrInPins:            0x02
baSourceID[1]:        0x04
baSourceID[2]:        0x06
bNrChannels:          0x02
wChannelConfig:     0x0003
iChannelNames:        0x00
bmControls:
00 
iMixer:               0x00

Audio Control Feature Unit Descriptor:
bLength:              0x0D
bDescriptorType:      0x24
bDescriptorSubtype:   0x06
bUnitID:              0x08
bSourceID:            0x07
bControlSize:         0x02
bmaControls[0]:
15 00 
bmaControls[1]:
02 00 
bmaControls[2]:
02 00 
iFeature:             0x00

Audio Control Mixer Unit Descriptor:
bLength:              0x0C
bDescriptorType:      0x24
bDescriptorSubtype:   0x04
bUnitID:              0x0A
bNrInPins:            0x01
baSourceID[1]:        0x08
bNrChannels:          0x01
wChannelConfig:     0x0008
iChannelNames:        0x00
bmControls:
00 
iMixer:               0x00

Audio Control Feature Unit Descriptor:
bLength:              0x0B
bDescriptorType:      0x24
bDescriptorSubtype:   0x06
bUnitID:              0x0B
bSourceID:            0x0A
bControlSize:         0x02
bmaControls[0]:
00 00 
bmaControls[1]:
02 00 
iFeature:             0x00

Audio Control Output Terminal Descriptor:
bLength:              0x09
bDescriptorType:      0x24
bDescriptorSubtype:   0x03
bTerminalID:          0x09
wTerminalType:      0x0304 (Desktop speaker)
bAssocTerminal:       0x00
bSourceID:            0x08
iTerminal:            0x00

Audio Control Output Terminal Descriptor:
bLength:              0x09
bDescriptorType:      0x24
bDescriptorSubtype:   0x03
bTerminalID:          0x0C
wTerminalType:      0x0307 (Low frequency effects speaker)
bAssocTerminal:       0x00
bSourceID:            0x0B
iTerminal:            0x00


*/

// Need to use 1 byte alignment for this data
#include <PSHPACK1.h>

// These structs are defined according to the USB audio device class
// specification found at
// http://www.usb.org/developers/devclass_docs/audio10.pdf.

// From Table 4-2: Class-Specific AC Interface Header Descriptor
typedef struct _AudioControlInterfaceHeaderDescriptor
{
    BYTE bLength;
    BYTE bDescriptorType;
    BYTE bDescriptorSubtype;
    WORD bcdADC;
    WORD wTotalLength;
    BYTE bInCollection;
    BYTE baInterfaceNr[1];
} AudioControlInterfaceHeaderDescriptor;

C_ASSERT((sizeof(AudioControlInterfaceHeaderDescriptor) == 9));


// From Table 4-3: Input Terminal Descriptor
typedef struct _AudioControlInputTerminalDescriptor
{
    BYTE bLength;
    BYTE bDescriptorType;
    BYTE bDescriptorSubtype;
    BYTE bTerminalID;
    WORD wTerminalType;
    BYTE bAssocTerminal;
    BYTE bNrChannels;
    WORD wChannelConfig;
    BYTE iChannelNames;
    BYTE iTerminal;
} AudioControlInputTerminalDescriptor;

C_ASSERT((sizeof(AudioControlInputTerminalDescriptor) == 12));

// From Table 4-9: Up/Down-mix Processing Unit Descriptor
typedef struct _AudioControlProcessingUnitDescriptor
{
    BYTE bLength;
    BYTE bDescriptorType;
    BYTE bDescriptorSubtype;
    BYTE bUnitID;
    WORD wProcessType;
    BYTE bNrInPins;
    BYTE baSourceID[1];
    BYTE bNrChannels;
    WORD wChannelConfig;
    BYTE iChannelNames;
    BYTE bControlSize;
    BYTE bmControls;
    BYTE iProcessing;
    BYTE bNrModes;
    WORD waModes[1];
} AudioControlProcessingUnitDescriptor;

C_ASSERT((sizeof(AudioControlProcessingUnitDescriptor) == 18));



// From Table 4-7: Feature Unit Descriptor
// This version of the structure holds controls for 2 channels
typedef struct _AudioControlFeatureUnitDescriptor2ch
{
    BYTE bLength;
    BYTE bDescriptorType;
    BYTE bDescriptorSubtype;
    BYTE bUnitID;
    BYTE bSourceID;
    BYTE bControlSize;
    WORD bmaControls[2];
    BYTE iFeature;
} AudioControlFeatureUnitDescriptor2ch;

C_ASSERT((sizeof(AudioControlFeatureUnitDescriptor2ch) == 11));



// From Table 4-7: Feature Unit Descriptor
// This version of the structure holds controls for 3 channels
typedef struct _AudioControlFeatureUnitDescriptor3ch
{
    BYTE bLength;
    BYTE bDescriptorType;
    BYTE bDescriptorSubtype;
    BYTE bUnitID;
    BYTE bSourceID;
    BYTE bControlSize;
    WORD bmaControls[3];
    BYTE iFeature;
} AudioControlFeatureUnitDescriptor3ch;

C_ASSERT((sizeof(AudioControlFeatureUnitDescriptor3ch) == 13));

typedef struct _AudioControlOutputTerminalDescriptor
{
    BYTE bLength;
    BYTE bDescriptorType;
    BYTE bDescriptorSubtype;
    BYTE bTerminalID;
    WORD wTerminalType;
    BYTE bAssocTerminal;
    BYTE bSourceID;
    BYTE iTerminal;

} AudioControlOutputTerminalDescriptor;

C_ASSERT((sizeof(AudioControlOutputTerminalDescriptor) == 9));

// From Table 4-5: Mixer Unit Descriptor
// This version of the structure holds source IDs for 1 input pin 
typedef struct _AudioControlMixerUnitDescriptor1Pin
{
    BYTE bLength;
    BYTE bDescriptorType;
    BYTE bDescriptorSubtype;
    BYTE bUnitID;
    BYTE bNrInPins;
    BYTE baSourceID[1];
    BYTE bNrChannels;
    WORD wChannelConfig;
    BYTE iChannelNames;
    BYTE bmControls;
    BYTE iMixer;
} AudioControlMixerUnitDescriptor1Pin;

C_ASSERT((sizeof(AudioControlMixerUnitDescriptor1Pin) == 12));


// From Table 4-5: Mixer Unit Descriptor
// This version of the structure holds source IDs for 2 input pins
typedef struct _AudioControlMixerUnitDescriptor2Pin
{
    BYTE bLength;
    BYTE bDescriptorType;
    BYTE bDescriptorSubtype;
    BYTE bUnitID;
    BYTE bNrInPins;
    BYTE baSourceID[2];
    BYTE bNrChannels;
    WORD wChannelConfig;
    BYTE iChannelNames;
    BYTE bmControls;
    BYTE iMixer;
} AudioControlMixerUnitDescriptor2Pin;

C_ASSERT((sizeof(AudioControlMixerUnitDescriptor2Pin) == 13));



// This struct describes the audio control interface descriptor
// Contained descriptors are named with suffixes containing their unit
// IDs


typedef struct _DSS80AudioControlInterfaceDescriptor
{
    AudioControlInterfaceHeaderDescriptor   Header;
    AudioControlInputTerminalDescriptor     USBStreamingITD01;
    AudioControlInputTerminalDescriptor     LineConnectorITD05;
    AudioControlProcessingUnitDescriptor    UpDownMixPUD02; // source=USBStreamingITD01
    AudioControlProcessingUnitDescriptor    UpDownMixPUD03; // source=UpDownMixPUD02
    AudioControlFeatureUnitDescriptor3ch    FUD04; // source=UpDownMixPUD03 / ch0: graphic equalizer / ch1: none / ch2: none
    AudioControlFeatureUnitDescriptor3ch    FUD06; // source=LineConnectorITD05 / ch1: volume / ch2: none / ch3: none
    AudioControlMixerUnitDescriptor2Pin     MUD07; // source=FUD04, FUD06
    AudioControlFeatureUnitDescriptor3ch    FUD08; // source=MUD07 / ch0: mute, treble, automatic gain / ch1: volume / ch2: volume
    AudioControlMixerUnitDescriptor1Pin     MUD0A; // source=FUD08
    AudioControlFeatureUnitDescriptor2ch    FUD0B; // source=MUD0A / ch0: none / ch1: volume
    AudioControlOutputTerminalDescriptor    DesktopSpeakerOT09; // source=FUD08
    AudioControlOutputTerminalDescriptor    LFEffectsSpeakerOT0C; // source=FUD0B

} DSS80AudioControlInterfaceDescriptor;

C_ASSERT((sizeof(DSS80AudioControlInterfaceDescriptor) == 0x00A2));



DSS80AudioControlInterfaceDescriptor ACIDescriptor =
{
    // Audio Control Interface Header Descriptor:
    {
      0x09, // bLength
      0x24, // bDescriptorType:
      0x01, // bDescriptorSubtype:
      0x0100, // bcdADC:
      0x00A2, // wTotalLength:
      0x01, // bInCollection:
      { 0x01 } // baInterfaceNr[1]
    },
    // Audio Control Input Terminal Descriptor:
    {
      0x0C, // bLength:
      0x24, // bDescriptorType:
      0x02, // bDescriptorSubtype:
      0x01, // bTerminalID:
      0x0101 , // wTerminalType: (USB streaming)
      0x00, // bAssocTerminal:
      0x02, // bNrChannels:
      0x0003, // wChannelConfig:
      0x00, // iChannelNames:
      0x00  // iTerminal:
    },
    // Audio Control Input Terminal Descriptor:
    {
      0x0C, // bLength:
      0x24, // bDescriptorType:
      0x02, // bDescriptorSubtype:
      0x05, // bTerminalID:
      0x0603, // wTerminalType:  (Line connector)
      0x00, // bAssocTerminal:
      0x02, // bNrChannels:
      0x0003, // wChannelConfig:
      0x00, // iChannelNames:
      0x00 // iTerminal:
    },
    // Audio Control Processing Unit Descriptor:
    {
      0x12, // bLength:
      0x24, // bDescriptorType:
      0x07, // bDescriptorSubtype:
      0x02, // bUnitID:
      0x0001,// wProcessType: (Up / Down Mix Process), 
      0x01, // bNrInPins:
      { 0x01 }, // baSourceID[1]:        
      0x04, // bNrChannels:
      0x0107, // wChannelConfig:
      0x00, // iChannelNames:
      0x01, // bControlSize:
      0x01, // bmControls:
      0x00, // iProcessing:
      0x01, // bNrModes
      { 0x0107 } // waModes[1]      
    },
    // Audio Control Processing Unit Descriptor:
    {
      0x12, // bLength:
      0x24, // bDescriptorType:
      0x07, // bDescriptorSubtype:
      0x03, // bUnitID:
      0x0001,// wProcessType: (Up / Down Mix Process), 
      0x01, // bNrInPins:
      { 0x02 }, // baSourceID[1]:        
      0x02, // bNrChannels:
      0x0003, // wChannelConfig:
      0x00, // iChannelNames:
      0x01, // bControlSize:
      0x00, // bmControls:
      0x00, // iProcessing:
      0x01, // bNrModes
      { 0x0003 } // waModes[1]      
    },
        
    // Audio Control Feature Unit Descriptor:
    {
        0x0D, // bLength
        0x24, // bDescriptorType
        0x06, // bDescriptorSubtype
        0x04, // bUnitID
        0x03, // bSourceID
        0x02, // bControlSize
        { 0x0020, 0x0000, 0x0000 }, // bmaControls
        0x00 // iFeature
    },
    
    // Audio Control Feature Unit Descriptor:
    {
        0x0D, // bLength
        0x24, // bDescriptorType
        0x06, // bDescriptorSubtype
        0x06, // bUnitID
        0x05, // bSourceID
        0x02, // bControlSize
        { 0x0002, 0x0000, 0x0000 }, // bmaControls
        0x00 // iFeature
    },

    // Audio Control Mixer Unit Descriptor:
    {
        0x0D, // bLength
        0x24, // bDescriptorType
        0x04, // bDescriptorSubtype
        0x07, // bUnitID
        0x02, // bNrInPins
        { 0x04, 0x06 }, // baSourceID
        0x02, // bNrChannels
        0x0003, // wChannelConfig
        0x00, // iChannelNames
        0x00, // bmControls
        0x00 // iMixer
    },

    // Audio Control Feature Unit Descriptor:
    {
        0x0D, // bLength
        0x24, // bDescriptorType
        0x06, // bDescriptorSubtype
        0x08, // bUnitID
        0x07, // bSourceID
        0x02, // bControlSize
        { 0x0015, 0x0002, 0x0002 }, // bmaControls
        0x00 // iFeature
    },

    // Audio Control Mixer Unit Descriptor:
    {
        0x0C, // bLength
        0x24, // bDescriptorType
        0x04, // bDescriptorSubtype
        0x0A, // bUnitID
        0x01, // bNrInPins
        { 0x08 }, // baSourceID[1]
        0x01, // bNrChannels
        0x0008, // wChannelConfig
        0x00, // iChannelNames
        0x00, // bmControls
        0x00 // iMixer
    },

    // Audio Control Feature Unit Descriptor:
    {
        0x0B, // bLength
        0x24, // bDescriptorType
        0x06, // bDescriptorSubtype
        0x0B, // bUnitID
        0x0A, // bSourceID
        0x02, // bControlSize
        { 0x0000, 0x0002 }, // bmaControls
        0x00 // iFeature
    },

    // Audio Control Output Terminal Descriptor:
    {
        0x09, // bLength
        0x24, // bDescriptorType
        0x03, // bDescriptorSubtype
        0x09, // bTerminalID
        0x0304, // wTerminalType: (Desktop speaker)
        0x00, // bAssocTerminal
        0x08, // bSourceID
        0x00 // iTerminal
    },

    // Audio Control Output Terminal Descriptor:
    {
        0x09, // bLength
        0x24, // bDescriptorType
        0x03, // bDescriptorSubtype
        0x0C, // bTerminalID
        0x0307, // wTerminalType: (Low frequency effects speaker)
        0x00, // bAssocTerminal
        0x0B, // bSourceID
        0x00  // iTerminal
    }
};




typedef struct _AudioStreamingClassSpecificInterfaceDescriptor
{
    BYTE bLength;
    BYTE bDescriptorType;
    BYTE bDescriptorSubtype;
    BYTE bTerminalLink;
    BYTE bDelay;
    WORD wFormatTag;

} AudioStreamingClassSpecificInterfaceDescriptor;

C_ASSERT((sizeof(AudioStreamingClassSpecificInterfaceDescriptor) == 7));

typedef struct _AudioStreamingFormatTypeDescriptor
{
    BYTE bLength;
    BYTE bDescriptorType;
    BYTE bDescriptorSubtype;
    BYTE bFormatType;
    BYTE bNrChannels;
    BYTE bSubframeSize;
    BYTE bBitResolution;
    BYTE bSamFreqType;
    BYTE abLowerSamFreq[3];
    BYTE abUpperSamFreq[3];

} AudioStreamingFormatTypeDescriptor;

C_ASSERT((sizeof(AudioStreamingFormatTypeDescriptor) == 14));

typedef struct _AudioStreamingClassSpecificInterfaceDescriptorData
{
    AudioStreamingClassSpecificInterfaceDescriptor Interface;
    AudioStreamingFormatTypeDescriptor             Format;
} AudioStreamingClassSpecificInterfaceDescriptorData;

C_ASSERT((sizeof(AudioStreamingClassSpecificInterfaceDescriptorData) == 21));


typedef struct _AudioStreamingClassSpecificAudioDataEndpointDescriptor
{
    BYTE bLength;
    BYTE bDescriptorType;
    BYTE bDescriptorSubtype;
    BYTE bmAttributes;
    BYTE bLockDelayUnits;
    WORD wLockDelay;
} AudioStreamingClassSpecificAudioDataEndpointDescriptor;

C_ASSERT((sizeof(AudioStreamingFormatTypeDescriptor) == 14));


AudioStreamingClassSpecificInterfaceDescriptorData StreamIntfDescAlt1 = 
{
    // Audio Streaming Class Specific Interface Descriptor
    {
        0x07, // bLength
        0x24, // bDescriptorType
        0x01, // bDescriptorSubtype
        0x01, // bTerminalLink
        0x00, // bDelay
        0x0001 // wFormatTag (PCM)
    },

    // Audio Streaming Format Type Descriptor
    {
        0x0E, // bLength
        0x24, // bDescriptorType
        0x02, // bDescriptorSubtype
        0x01, // bFormatType
        0x01, // sbNrChannels
        0x01, // bSubframeSize
        0x08, // bBitResolution
        0x00, // bSamFreqType
        { 0x7E, 0x13, 0x00 }, // abLowerSamFreq 0x00137E (4990 Hz)
        { 0xE2, 0xD6, 0x00 }  // abUpperSamFreq 0x00D6E2 (55010 Hz)
    }
};

AudioStreamingClassSpecificAudioDataEndpointDescriptor StreamEPDesc =
{
    0x07, // bLength
    0x25, // bDescriptorType
    0x01, // bDescriptorSubtype
    0x01, // bmAttributes
    0x02, // bLockDelayUnits
    0x0200 // wLockDelay
};


AudioStreamingClassSpecificInterfaceDescriptorData StreamIntfDescAlt2 = 
{
    // Audio Streaming Class Specific Interface Descriptor
    {
        0x07, // bLength
        0x24, // bDescriptorType
        0x01, // bDescriptorSubtype
        0x01, // bTerminalLink
        0x00, // bDelay
        0x0001 // wFormatTag (PCM)
    },

    // Audio Streaming Format Type Descriptor
    {
        0x0E, // bLength
        0x24, // bDescriptorType
        0x02, // bDescriptorSubtype
        0x01, // bFormatType
        0x02, // sbNrChannels
        0x01, // bSubframeSize
        0x08, // bBitResolution
        0x00, // bSamFreqType
        { 0x7E, 0x13, 0x00 }, // abLowerSamFreq 0x00137E (4990 Hz)
        { 0xE2, 0xD6, 0x00 }  // abUpperSamFreq 0x00D6E2 (55010 Hz)
    }
};

AudioStreamingClassSpecificInterfaceDescriptorData StreamIntfDescAlt3 = 
{
    // Audio Streaming Class Specific Interface Descriptor
    {
        0x07, // bLength
        0x24, // bDescriptorType
        0x01, // bDescriptorSubtype
        0x01, // bTerminalLink
        0x00, // bDelay
        0x0001 // wFormatTag (PCM)
    },

    // Audio Streaming Format Type Descriptor
    {
        0x0E, // bLength
        0x24, // bDescriptorType
        0x02, // bDescriptorSubtype
        0x01, // bFormatType
        0x01, // sbNrChannels
        0x02, // bSubframeSize
        0x10, // bBitResolution
        0x00, // bSamFreqType
        { 0x7E, 0x13, 0x00 }, // abLowerSamFreq 0x00137E (4990 Hz)
        { 0xE2, 0xD6, 0x00 }  // abUpperSamFreq 0x00D6E2 (55010 Hz)
    }
};

AudioStreamingClassSpecificInterfaceDescriptorData StreamIntfDescAlt4 = 
{
    // Audio Streaming Class Specific Interface Descriptor
    {
        0x07, // bLength
        0x24, // bDescriptorType
        0x01, // bDescriptorSubtype
        0x01, // bTerminalLink
        0x00, // bDelay
        0x0001 // wFormatTag (PCM)
    },

    // Audio Streaming Format Type Descriptor
    {
        0x0E, // bLength
        0x24, // bDescriptorType
        0x02, // bDescriptorSubtype
        0x01, // bFormatType
        0x02, // sbNrChannels
        0x02, // bSubframeSize
        0x10, // bBitResolution
        0x00, // bSamFreqType
        { 0x7E, 0x13, 0x00 }, // abLowerSamFreq 0x00137E (4990 Hz)
        { 0xE2, 0xD6, 0x00 }  // abUpperSamFreq 0x00D6E2 (55010 Hz)
    }
};

AudioStreamingClassSpecificInterfaceDescriptorData StreamIntfDescAlt5 = 
{
    // Audio Streaming Class Specific Interface Descriptor
    {
        0x07, // bLength
        0x24, // bDescriptorType
        0x01, // bDescriptorSubtype
        0x01, // bTerminalLink
        0x00, // bDelay
        0x0001 // wFormatTag (PCM)
    },

    // Audio Streaming Format Type Descriptor
    {
        0x0E, // bLength
        0x24, // bDescriptorType
        0x02, // bDescriptorSubtype
        0x01, // bFormatType
        0x01, // sbNrChannels
        0x03, // bSubframeSize
        0x14, // bBitResolution
        0x00, // bSamFreqType
        { 0x7E, 0x13, 0x00 }, // abLowerSamFreq 0x00137E (4990 Hz)
        { 0xE2, 0xD6, 0x00 }  // abUpperSamFreq 0x00D6E2 (55010 Hz)
    }
};

AudioStreamingClassSpecificInterfaceDescriptorData StreamIntfDescAlt6 = 
{
    // Audio Streaming Class Specific Interface Descriptor
    {
        0x07, // bLength
        0x24, // bDescriptorType
        0x01, // bDescriptorSubtype
        0x01, // bTerminalLink
        0x00, // bDelay
        0x0001 // wFormatTag (PCM)
    },

    // Audio Streaming Format Type Descriptor
    {
        0x0E, // bLength
        0x24, // bDescriptorType
        0x02, // bDescriptorSubtype
        0x01, // bFormatType
        0x02, // sbNrChannels
        0x03, // bSubframeSize
        0x14, // bBitResolution
        0x00, // bSamFreqType
        { 0x7E, 0x13, 0x00 }, // abLowerSamFreq 0x00137E (4990 Hz)
        { 0xE2, 0xD6, 0x00 }  // abUpperSamFreq 0x00D6E2 (55010 Hz)
    }
};



#include <POPPACK.h>



CAudioDevice::CAudioDevice()
/*++
Routine Description:
    Constructor for CAudioDevice. 

Arguments:
    None

Return value:
    None
--*/
{
    InitMemberVariables();
}

CAudioDevice::~CAudioDevice()
/*++
Routine Description:
    Destructor for CAudioDevice. 

Arguments:
    None

Return value:
    None
--*/
{
    // Remove our event sink on the control endpoint
    ReleaseConnectionPoint();
    
    // Release any intefaces that we hold
    RELEASE(m_piUSBDevice);
    RELEASE(m_piEndpoint0);
    RELEASE(m_piConnectionPoint);
    RELEASE(m_piConfig);
    RELEASE(m_piCurrentInterface);
    RELEASE(m_piEndpoint);

    InitMemberVariables();
}

void CAudioDevice::InitMemberVariables()
/*++
Routine Description:
   Initialize member variables for the class CAudioDevice  

Arguments:
    None

Return value:
    None
--*/
{
    m_piUSBDevice = NULL;
    m_piEndpoint0 = NULL;
    m_piConfig    = NULL;
    m_piCurrentInterface = NULL;
    m_piEndpoint = NULL;
    m_bStreamInterface = 1;
    m_bAlternateInterface = 0xff;
    m_fDisplayDeviceRequests = FALSE;
    m_piConnectionPoint = NULL;
    m_dwConnectionCookie = 0;
    m_wVol = 0;
    m_bBass = 0;
    m_bTreble = 0;
    m_wLineVol = 0;
    m_bLineBass = 0;
    m_bLineTreble = 0;
    m_wLowFreqVol = 0;
    m_bLowFreqBass = 0;
    m_bLowFreqTreble = 0;
    m_wCombineCh1Vol = 0;
    m_bCombineCh1Bass = 0;
    m_bCombineCh1Treble = 0;
    m_wCombineCh2Vol = 0;
    m_bCombineCh2Bass = 0;
    m_bCombineCh2Treble = 0;
    m_fMuted = FALSE;
    m_fPitchControlEnabled = FALSE;
    m_lPacketCount = 0;

    ZeroMemory(m_ProcessingUnitMode2, sizeof(m_ProcessingUnitMode2));
    ZeroMemory(m_ProcessingUnitMode3, sizeof(m_ProcessingUnitMode3));

    ZeroMemory(m_abCurIsochEndpointSampFreq, sizeof(m_abCurIsochEndpointSampFreq));
    // Set to 0x00137E (4990 Hz)
    m_abCurIsochEndpointSampFreq[0] = 0x7E;
    m_abCurIsochEndpointSampFreq[1] = 0x13;
    m_abCurIsochEndpointSampFreq[2] = 0x00;

    ZeroMemory(m_abMinIsochEndpointSampFreq, sizeof(m_abMinIsochEndpointSampFreq));
    // Set to 0x00137E (4990 Hz)
    m_abMinIsochEndpointSampFreq[0] = 0x7E;
    m_abMinIsochEndpointSampFreq[1] = 0x13;
    m_abMinIsochEndpointSampFreq[2] = 0x00;

    ZeroMemory(m_abMaxIsochEndpointSampFreq, sizeof(m_abMaxIsochEndpointSampFreq));
    // Set to 0x00D6E2 (55010 Hz)
    m_abMaxIsochEndpointSampFreq[0] = 0xE2;
    m_abMaxIsochEndpointSampFreq[1] = 0xD6;
    m_abMaxIsochEndpointSampFreq[2] = 0x00;

    ZeroMemory(m_abResIsochEndpointSampFreq, sizeof(m_abResIsochEndpointSampFreq));
    // Set to 0x000001 (1 Hz)
    m_abResIsochEndpointSampFreq[0] = 0x01;
    m_abResIsochEndpointSampFreq[1] = 0x00;
    m_abResIsochEndpointSampFreq[2] = 0x00;

    // Initialize graphic equalizer settings
    EQDATA *apEqData[] =
    {
        &m_EqDataCur,
        &m_EqDataMin,
        &m_EqDataMax,
        &m_EqDataRes
    };

    for (int i = 0; i < sizeof(apEqData) / sizeof(apEqData[0]); i++)
    {
        ZeroMemory(apEqData[i], sizeof(EQDATA));
        apEqData[i]->bmBandsPresent.fBand15 = 1;
        apEqData[i]->bmBandsPresent.fBand18 = 1;
        apEqData[i]->bmBandsPresent.fBand21 = 1;
        apEqData[i]->bmBandsPresent.fBand24 = 1;
        apEqData[i]->bmBandsPresent.fBand27 = 1;
        apEqData[i]->bmBandsPresent.fBand30 = 1;
        apEqData[i]->bmBandsPresent.fBand33 = 1;
        apEqData[i]->bmBandsPresent.fBand36 = 1;
        apEqData[i]->bmBandsPresent.fBand39 = 1;
        apEqData[i]->bmBandsPresent.fBand42 = 1;
    }

    // Set all current levels to 0db
    m_EqDataCur.bBand15 = 0;
    m_EqDataCur.bBand18 = 0;
    m_EqDataCur.bBand21 = 0;
    m_EqDataCur.bBand24 = 0;
    m_EqDataCur.bBand27 = 0;
    m_EqDataCur.bBand30 = 0;
    m_EqDataCur.bBand33 = 0;
    m_EqDataCur.bBand36 = 0;
    m_EqDataCur.bBand39 = 0;
    m_EqDataCur.bBand42 = 0;

    // Set all min levels to -32 db
    m_EqDataMin.bBand15 = 0x80;
    m_EqDataMin.bBand18 = 0x80;
    m_EqDataMin.bBand21 = 0x80;
    m_EqDataMin.bBand24 = 0x80;
    m_EqDataMin.bBand27 = 0x80;
    m_EqDataMin.bBand30 = 0x80;
    m_EqDataMin.bBand33 = 0x80;
    m_EqDataMin.bBand36 = 0x80;
    m_EqDataMin.bBand39 = 0x80;
    m_EqDataMin.bBand42 = 0x80;

    // Set all max levels to +31.75 db
    m_EqDataMax.bBand15 = 0x7F;
    m_EqDataMax.bBand18 = 0x7F;
    m_EqDataMax.bBand21 = 0x7F;
    m_EqDataMax.bBand24 = 0x7F;
    m_EqDataMax.bBand27 = 0x7F;
    m_EqDataMax.bBand30 = 0x7F;
    m_EqDataMax.bBand33 = 0x7F;
    m_EqDataMax.bBand36 = 0x7F;
    m_EqDataMax.bBand39 = 0x7F;
    m_EqDataMax.bBand42 = 0x7F;

    // Set all res to 2 db
    m_EqDataRes.bBand15 = 1;
    m_EqDataRes.bBand18 = 1;
    m_EqDataRes.bBand21 = 1;
    m_EqDataRes.bBand24 = 1;
    m_EqDataRes.bBand27 = 1;
    m_EqDataRes.bBand30 = 1;
    m_EqDataRes.bBand33 = 1;
    m_EqDataRes.bBand36 = 1;
    m_EqDataRes.bBand39 = 1;
    m_EqDataRes.bBand42 = 1;
}



HRESULT CAudioDevice::FinalConstruct()
/*++
Routine Description:
   Perform tasks which may fail when the class CAudioDevice
   is finally constructed. This involves creating the USB device 
   object and initializing the device so that it is recognized
   as a valid USB device by the controller

Arguments:
    None

Return value:
    From called functions.
--*/
{
    HRESULT hr = S_OK;

    IfFailHrGo(::CoInitialize(NULL));

    IfFailHrGo(CreateUSBDevice());

Exit:
    return hr;
}



HRESULT CAudioDevice::CreateUSBDevice()
/*++
Routine Description:
   Creatres the USB device and initailizes the device
   member variables and creates and initializes the 
   device qualifier. The device qualifier is required
   for USB2.0 devices.

Arguments:
    None

Return value:
    From called functions
--*/
{
    HRESULT                 hr = S_OK;
    ISoftUSBConfigurations *piConfigs = NULL;
    ISoftUSBInterfaces     *piInterfaces = NULL;
    ISoftUSBInterface      *piAudioCtrlIntf = NULL;
    USBCONFIGATTRIBS        ConfigAttribs; ConfigAttribs.Byte = 0;
    VARIANT                 varIndex; VariantInit(&varIndex);

    // All collection items will be added at the default locations
    // so set up the index to indicate it is an optional parameter that
    // has not been specified.
    varIndex.vt = VT_ERROR;
    varIndex.scode = DISP_E_PARAMNOTFOUND;

    IfFailHrGo(::CoCreateInstance(CLSID_SoftUSBDevice, 
                                  NULL,
                                  CLSCTX_INPROC_SERVER,
                                  __uuidof(ISoftUSBDevice),     
                                  reinterpret_cast<void**>(&m_piUSBDevice)));

    // Set the device properties 
    IfFailHrGo(m_piUSBDevice->put_USB(0x0110));                             // binary coded decimal USB version 1.1
    IfFailHrGo(m_piUSBDevice->put_DeviceClass(0x00));                       // 00=audio is defined by interface class
    IfFailHrGo(m_piUSBDevice->put_DeviceSubClass(0x00));                    // 00=audio is defined by interface subclass
    IfFailHrGo(m_piUSBDevice->put_DeviceProtocol(0x00));                    // 00=audio is defined by interface protocol
    IfFailHrGo(m_piUSBDevice->put_MaxPacketSize0(64));                      // max packet size endpoint 0
    IfFailHrGo(m_piUSBDevice->put_Vendor(0x045E));                          // Vendor ID - Microsoft
    IfFailHrGo(m_piUSBDevice->put_Product(0x0014));                         // product id - Microsoft Digital Sound System 80
    IfFailHrGo(m_piUSBDevice->put_Device(0x0100));                          // Binary decimal coded version 1.0
    IfFailHrGo(m_piUSBDevice->put_RemoteWakeup(VARIANT_FALSE));             // Device does not suppport remote wake up
    IfFailHrGo(m_piUSBDevice->put_Manufacturer(STRING_IDX_MANUFACTURER));   // Index of the manufacturer string descriptor
    IfFailHrGo(m_piUSBDevice->put_ProductDesc(STRING_IDX_PRODUCT_DESC));    // Index of the product descripton string descriptor
    IfFailHrGo(m_piUSBDevice->put_SerialNumber(STRING_IDX_SERIAL_NO));      // Index of the serial number string descriptor
    IfFailHrGo(m_piUSBDevice->put_HasExternalPower(VARIANT_TRUE));          // Indicate that the device has power

    // Store an interface on the control endpoint so we can handle
    // class-specific device requests later on
    IfFailHrGo(m_piUSBDevice->get_Endpoint0(&m_piEndpoint0));

    // Create the configuation and set its properties
    IfFailHrGo(::CoCreateInstance(CLSID_SoftUSBConfiguration, 
                                  NULL,
                                  CLSCTX_INPROC_SERVER,
                                  __uuidof(ISoftUSBConfiguration),     
                                  reinterpret_cast<void**>(&m_piConfig)));

    IfFailHrGo(m_piConfig->put_ConfigurationValue(1));            // Config number passed by host in SET_CONFIGURATION request
    IfFailHrGo(m_piConfig->put_Configuration(STRING_IDX_CONFIG)); // Index of string descriptor
    IfFailHrGo(m_piConfig->put_MaxPower(0));                      // max bus power consumed in 2mA units

    ConfigAttribs.Bits.bRemoteWakeup = 0;                       // Device does not do remote wakeup
    ConfigAttribs.Bits.bSelfPowered = 1;                        // Device is self-powered
    IfFailHrGo(m_piConfig->put_Attributes(ConfigAttribs.Byte));

    // Get SoftUSBConfiguration.Interfaces
    IfFailHrGo(m_piConfig->get_Interfaces(&piInterfaces));

    // Create the audio control interface and add it to the
    // configuration
    IfFailHrGo(CreateAudioControlInterface(piAudioCtrlIntf));
    IfFailHrGo(piInterfaces->Add(reinterpret_cast<SoftUSBInterface *>(piAudioCtrlIntf), varIndex));

    // Create and add the audio streaming interfaces to the
    // configuration
    IfFailHrGo(CreateAudioStreamingInterfaces(m_piUSBDevice, piInterfaces));

    // Add the configuration to SoftUSBDevice.Configurations
    IfFailHrGo(m_piUSBDevice->get_Configurations(&piConfigs));
    IfFailHrGo(piConfigs->Add(reinterpret_cast<SoftUSBConfiguration *>(m_piConfig), varIndex));

    // Create the string descriptors and add them to the device
    IfFailHrGo(CreateStrings());

Exit:
    RELEASE(piConfigs);
    RELEASE(piAudioCtrlIntf);
    RELEASE(piInterfaces);
    return hr;
}


HRESULT CAudioDevice::CreateAudioControlInterface(ISoftUSBInterface *&rpiAudioCtrlIntf)
/*++
Routine Description:
   Creates a SoftUSBInterface object, configures it with the audio
   control interface, and returns it to the caller. 

Arguments:
    rpiAudioCtrlIntf - ISoftUSBInterface on audio control interface
    returned here. Caller must release.

Return value:
    E_OUTOFMEMORY - failed to create SAFEARRAY to set SoftUSBInterface.DeviceSpecificDescriptor

    other - from called functions
--*/
{
    HRESULT            hr = S_OK;
    SAFEARRAY         *psa = NULL;
    ISoftUSBInterface *piInterface = NULL;

    rpiAudioCtrlIntf = NULL;

    // Create the SoftUSBInterface object and get an ISoftUSBInterface
    // interface on it
    IfFailHrGo(::CoCreateInstance(CLSID_SoftUSBInterface, 
                                  NULL,
                                  CLSCTX_INPROC_SERVER,
                                  __uuidof(ISoftUSBInterface),     
                                  reinterpret_cast<void **>(&piInterface)));

    // Configure the interface properties
    IfFailHrGo(piInterface->put_InterfaceNumber(0));
    IfFailHrGo(piInterface->put_AlternateSetting(0));
    IfFailHrGo(piInterface->put_InterfaceClass(0x01));    // audio class
    IfFailHrGo(piInterface->put_InterfaceSubClass(0x01)); // audio control
    IfFailHrGo(piInterface->put_InterfaceProtocol(0));
    IfFailHrGo(piInterface->put_Interface(STRING_IDX_AUDIO_CTRL_INTERFACE));

    // Create a SAFEARRAY containing the audio control class-specific
    // descriptor data and set
    // SoftUSBInterface.DeviceSpecificDescriptor with it

    IfFailHrGo(ByteArrayToSafeArray(reinterpret_cast<BYTE *>(&ACIDescriptor), sizeof(ACIDescriptor), psa));
    IfFailHrGo(piInterface->put_DeviceSpecificDescriptor(psa));

    // Return the interface to the caller
    piInterface->AddRef();
    rpiAudioCtrlIntf = piInterface;

Exit:
    if (NULL != psa)
    {
        (void)::SafeArrayDestroy(psa);
    }
    RELEASE(piInterface);
    return hr;
}

HRESULT CAudioDevice::CreateAudioStreamingInterfaces
(
    ISoftUSBDevice        *piSoftUSBDevice,
    ISoftUSBInterfaces    *piInterfaces
)
/*++
Routine Description:
   Creates SoftUSBInterface objects for the audio streaming interfaces,
   configures them, and adds them to the specified interface collection.
   

Arguments:
    piSoftUSBDevice - the device that owns the config

    piInterfaces - SoftUSBInterface collection to which audio streaming
    interfaces will be added

Return value:
    other - from called functions
--*/
{
    HRESULT               hr = S_OK;
    ISoftUSBInterface    *piInterface = NULL;
    ISoftUSBEndpoints    *piEndpoints = NULL;
    ISoftUSBEndpoint     *piEndpoint = NULL;
    BYTE                  i = 0;
    SAFEARRAY            *psa = NULL;
    VARIANT               varIndex; VariantInit(&varIndex);
    VARIANT               varEndpointIndex; VariantInit(&varEndpointIndex);

    UNREFERENCED_PARAMETER(piSoftUSBDevice);

    // All interface collection items will be added at the default
    // locations so set up the index to indicate it is an optional
    // parameter that has not been specified.
    varIndex.vt = VT_ERROR;
    varIndex.scode = DISP_E_PARAMNOTFOUND;

    // All endpoints will be saves in the 1st location in the endpoint
    // collection so that they can be retrived when the data is
    // drained from the endpoint. Note there is only one endpoint 
    // per interface.
    varEndpointIndex.vt = VT_I4;
    varEndpointIndex.lVal = 1;

    // Array of addresses of audio streaming class-specific interface
    // descriptors for each interface alternate setting 1 to 6
    static AudioStreamingClassSpecificInterfaceDescriptorData *apStreamIntfDescs[] =
    {
        &StreamIntfDescAlt1,
        &StreamIntfDescAlt2,
        &StreamIntfDescAlt3,
        &StreamIntfDescAlt4,
        &StreamIntfDescAlt5,
        &StreamIntfDescAlt6
    };

    // Isoch out endpoint max packet sizes for alternates 1 - 6
    static WORD MaxPacketSizes[] = { 0x0038, 0x0070, 0x0070, 0x00E0, 0x00A8, 0x0150 };

    static USBENDPOINTATTRIBS EndpointAttribs;
    EndpointAttribs.Bits.TransferType = USB_TRANSFER_TYPE_ISOCH;
    EndpointAttribs.Bits.SyncType = USB_SYNC_TYPE_SYNC;
    EndpointAttribs.Bits.UsageType = USB_USAGE_TYPE_DATA;

    for (i = 0; i <= 6; i++)
    {
        // Create a SoftUSBInterface object and get an ISoftUSBInterface
        // interface on it
        IfFailHrGo(::CoCreateInstance(CLSID_SoftUSBInterface, 
                                      NULL,
                                      CLSCTX_INPROC_SERVER,
                                      __uuidof(ISoftUSBInterface),     
                                      reinterpret_cast<void **>(&piInterface)));

        // Configure the interface properties
        IfFailHrGo(piInterface->put_InterfaceNumber(m_bStreamInterface));
        IfFailHrGo(piInterface->put_AlternateSetting(i));
        IfFailHrGo(piInterface->put_InterfaceClass(1));    // audio class
        IfFailHrGo(piInterface->put_InterfaceSubClass(2)); // audio streaming
        IfFailHrGo(piInterface->put_InterfaceProtocol(0));
        IfFailHrGo(piInterface->put_Interface(STRING_IDX_AUDIO_STREAMING_INTERFACE));

        if (i > 0) // alternate setting 0 does not define endpoints
        {
            // Create a SAFEARRAY containing the audio streaming
            // class-specific interface descriptor data and set
            // SoftUSBInterface.DeviceSpecificDescriptor with it
            IfFailHrGo(ByteArrayToSafeArray(reinterpret_cast<BYTE *>(apStreamIntfDescs[i - 1]), sizeof(*(apStreamIntfDescs[i - 1])), psa));
            IfFailHrGo(piInterface->put_DeviceSpecificDescriptor(psa));
            (void)::SafeArrayDestroy(psa);
            psa = NULL;

            // Create a SoftUSBEndpoint object and get an
            // ISoftUSBEndpoint interface on it
            IfFailHrGo(::CoCreateInstance(CLSID_SoftUSBEndpoint, 
                                          NULL,
                                          CLSCTX_INPROC_SERVER,
                                          __uuidof(ISoftUSBEndpoint),     
                                          reinterpret_cast<void **>(&piEndpoint)));

            // Configure the endpoint properties
            IfFailHrGo(piEndpoint->put_EndpointAddress(4));
            IfFailHrGo(piEndpoint->put_Attributes(EndpointAttribs.Byte));
            IfFailHrGo(piEndpoint->put_MaxPacketSize(MaxPacketSizes[i - 1]));
            IfFailHrGo(piEndpoint->put_Interval(1));
            IfFailHrGo(piEndpoint->put_HandleStdDeviceRequests(VARIANT_TRUE));

            // Create a SAFEARRAY containing the audio streaming
            // class-specific endpoint descriptor data and set
            // SoftUSBEndpoint.DeviceSpecificDescriptor with it
            IfFailHrGo(ByteArrayToSafeArray(reinterpret_cast<BYTE *>(&StreamEPDesc), sizeof(StreamEPDesc), psa));
            IfFailHrGo(piEndpoint->put_DeviceSpecificDescriptor(psa));
            (void)::SafeArrayDestroy(psa);
            psa = NULL;

            // Add the endpoint to SoftUSBInterface.Endpoints
            IfFailHrGo(piInterface->get_Endpoints(&piEndpoints));
            IfFailHrGo(piEndpoints->Add(reinterpret_cast<SoftUSBEndpoint *>(piEndpoint), varEndpointIndex));

            RELEASE(piEndpoints);
            RELEASE(piEndpoint);
        }

        // Add the interface to the specified SoftUSBInterface collection
        IfFailHrGo(piInterfaces->Add(reinterpret_cast<SoftUSBInterface *>(piInterface), varIndex));
        RELEASE(piInterface);
    }


Exit:
    if (NULL != psa)
    {
        (void)::SafeArrayDestroy(psa);
        psa = NULL;
    }
    RELEASE(piEndpoint);
    RELEASE(piEndpoints);
    RELEASE(piInterface);
    return hr;
}


HRESULT CAudioDevice::ByteArrayToSafeArray
(
    BYTE       *pbData,      
    ULONG      cbData,
    SAFEARRAY *&rpsa
)
/*++
Routine Description:
   Converts a BYTE array to a SAFEARRAY of VARIANT where each element
   is VT_UI1.

Arguments:
    pbData - pointer to array of bytes
    cbData - length of array in bytes
    rpsa - SAFEARRAY returned here. Caller must call SafeArrayDestroy.

Return value:
    E_OUTOFMEMORY - could not allocate memory for SAFEARRAY
    other - from called functions
--*/
{
    HRESULT        hr = S_OK;
    SAFEARRAY     *psa = NULL;
    VARIANT HUGEP *pvarData = NULL;
    size_t         i = 0;

    rpsa = NULL;

    // Create the SAFEARRAY of cbData VARIANTs
    psa = ::SafeArrayCreateVector(VT_VARIANT, 0, cbData);
    IfFalseHrGo(NULL != psa, E_OUTOFMEMORY);

    // Get a pointer to the raw array data
    IfFailHrGo(::SafeArrayAccessData(psa, reinterpret_cast<void HUGEP **>(&pvarData)));

    // Initialize each VARIANT in the array and set its value to the
    // corresponding byte in the caller's buffer
    for (i = 0; i < cbData; i++)
    {
        ::VariantInit(&pvarData[i]);
        pvarData[i].vt = VT_UI1;
        pvarData[i].bVal = pbData[i];
    }

    // Return the SAFEARRAY to the caller
    rpsa = psa;
    psa = NULL;

Exit:
    if (NULL != pvarData)
    {
        ::SafeArrayUnaccessData(psa);
        pvarData = NULL;
    }
    if (FAILED(hr) && (NULL != psa))
    {
        (void)::SafeArrayDestroy(psa);
        psa = NULL;
    }
    return hr;
}





HRESULT CAudioDevice::CreateStrings()
/*++
Routine Description:
   Creates the string descriptors for the device and adds them to
   SoftUSBDevice.Strings.

Arguments:
    None

Return value:
    E_OUTOFMEMORY - could no allocate string memory
    other - from called functions
--*/
{
    HRESULT             hr = S_OK;
    ISoftUSBStrings    *piStrings = NULL;
    ISoftUSBString     *piString = NULL;
    size_t              i = 0;
    BSTR                bstr = NULL;
    VARIANT             varIndex; VariantInit(&varIndex);

    static struct StringSourceData
    {
        LPCWSTR pwszString;
        long    lIndex;
    } SourceStrings[] =
    {
        { (L"Microsoft"),                         STRING_IDX_MANUFACTURER },
        { (L"Microsoft Digital Sound System 80"), STRING_IDX_PRODUCT_DESC }
    };

    static size_t cStrings = sizeof(SourceStrings) / sizeof(SourceStrings[0]);

    // Get SoftUSBDevice.Strings
    IfFailHrGo(m_piUSBDevice->get_Strings(&piStrings));

    // Set up the variant used as the index
    varIndex.vt = VT_I4;

    // Create string descriptors and add them to SoftUSBDevice.Strings.
    // Also create a string descriptor index for each. This index is
    // used both to set the string descriptor's position in
    // SoftUSBUSBDevice.Strings and is the index value sent in a
    // GET_DESCRIPTOR request from the host. Note that we don't use
    // string descriptor index zero because that is a reserved value for
    // a device's language ID descriptor.

    for (i = 0; i < cStrings; i++)
    {
        IfFailHrGo(::CoCreateInstance(CLSID_SoftUSBString,
                                      NULL,
                                      CLSCTX_INPROC_SERVER,
                                      __uuidof(ISoftUSBString),     
                                      reinterpret_cast<void **>(&piString)));

        bstr = ::SysAllocString(SourceStrings[i].pwszString);
        IfFalseHrGo(0 != ::SysStringLen(bstr), E_OUTOFMEMORY);

        IfFailHrGo(piString->put_Value(bstr));
        ::SysFreeString(bstr);
        bstr = NULL;

        varIndex.lVal = SourceStrings[i].lIndex;
        IfFailHrGo(piStrings->Add(reinterpret_cast<SoftUSBString *>(piString), varIndex));

        RELEASE(piString);
    }
    

Exit:
    RELEASE(piStrings);
    RELEASE(piString);
    ::SysFreeString(bstr);

    return hr;
}

HRESULT CAudioDevice::ReleaseConnectionPoint()
/*++
Routine Description:
   Release the connection point to the OUT Endpoint
   if one has begin established.

Arguments:
    None

Return value:
    S_OK
--*/
{
    HRESULT hr = S_OK;
    
    if (NULL != m_piConnectionPoint)
    {
        m_piConnectionPoint->Unadvise(m_dwConnectionCookie);
        m_dwConnectionCookie = 0;
    }

    RELEASE(m_piConnectionPoint);

    return hr;
}


HRESULT CAudioDevice::SetupConnectionPoint
(
    IUnknown *punkObject,
    REFIID    iidConnectionPoint
)
/*++
Routine Description:
   Setup the connection point to the OUT Endpoint.
   It validates that the punkObject support IConnectionPointContainter
   then finds the correct connection point and attach this object
   as the event sink.

Arguments:
    punkObject - Object which implement IConnectionPointContainer

    iidConnectionPoint - IID of the connection point

Return value:
    S_OK if the connection point is correctly set up
    E_UNEXPECTED if the IUknown of this can not be obtained
    Otherwise from called functions.
--*/
{
    HRESULT hr = S_OK;
    IConnectionPointContainer *piConnectionPointContainer = NULL;
    IUnknown                  *punkSink = NULL;

    //If there is already connection point enabled disable it
    if(NULL != m_piConnectionPoint)
    {
        IfFailHrGo(ReleaseConnectionPoint());
    }
        
    IfFailHrGo(punkObject->QueryInterface(IID_IConnectionPointContainer,
                                          reinterpret_cast<void **>(&piConnectionPointContainer)));

    IfFailHrGo(piConnectionPointContainer->FindConnectionPoint(iidConnectionPoint,
                                                               &m_piConnectionPoint));

    //Get the IUnknown of this object as this is the event sink
    punkSink = (this)->GetUnknown(); 

    if(NULL == punkSink)
    {
        IfFailHrGo(E_UNEXPECTED);
    }

    IfFailHrGo(m_piConnectionPoint->Advise(punkSink, &m_dwConnectionCookie));


Exit:
    RELEASE(piConnectionPointContainer);

    return hr;
}



STDMETHODIMP CAudioDevice::get_DSFDevice(DSFDevice** ppDSFDevice)
/*++
Routine Description:
   Implement the get property DSFDevice for the interface
   IAudioDevice.

Arguments:
    ppDSFDevice - called allocated space to hold the DSF device 
                  associated with this USB device.

Return value:
    E_FAIL - the USB device does not exist
    E_POINTER - ppDSFDevice is not a valid pointer
    other - from called functions
--*/
{
    HRESULT     hr = S_OK;

    IfFalseHrGo(NULL != ppDSFDevice, E_POINTER);
    *ppDSFDevice = NULL;

    // Validate the the USB device exists else this is an internal error
    IfFalseHrGo(NULL != m_piUSBDevice, E_FAIL);

    IfFailHrGo(m_piUSBDevice->get_DSFDevice(ppDSFDevice));

Exit:

    return hr;
}

STDMETHODIMP CAudioDevice::get_PacketsReceived(LONG* plPacketCount)
/*++
Routine Description:
   Implement the get property for packets received for the IDSFAudioDevice

Arguments:
    plPacketCount - caller allocated space to hold the number of packets
    received by audio device

Return value:
    E_POINTER - plPacketCount is not a valid pointer
    other - from called functions
--*/
{
    HRESULT     hr = S_OK;

    IfFalseHrGo(NULL != plPacketCount, E_POINTER);
    *plPacketCount = m_lPacketCount;

Exit:
    return hr;
}





STDMETHODIMP CAudioDevice::StartSimulation()
/*++
Routine Description:
    Called from the test application to run the main simulation loop.
    During the loop the audio simulator fires an event into the test
    app asking it whether to continue the simulation.

Arguments:
    None

Return value:
    E_UNEXPECTED - SoftUSBDevice not configured
    
    other - from called functions
--*/
{
    HRESULT      hr           = S_OK;
    VARIANT_BOOL fvarContinue = VARIANT_TRUE;
    
    IfFalseHrGo(NULL != m_piEndpoint0, E_UNEXPECTED);

    // Set up event sink on the control endpoint
    IfFailHrGo(SetupConnectionPoint(m_piEndpoint0, __uuidof(ISoftUSBEndpointEvents)));

    ::printf("AudioDevice.StartSimulation starting simulation\n");

    // Loop waiting for events to be fired (this will happen in another
    // thread created by the USB simulation runtime).
    while (VARIANT_TRUE == fvarContinue)
    {
        // Context switch to allow other threads to process
        ::Sleep(125);

        // Fire event to check if the caller wants to continue processing
        IfFailHrGo(Fire_ContinueSimulating(&fvarContinue));

        if (VARIANT_FALSE == fvarContinue)
        {
            ::printf("AudioDevice.StartSimulation received False from ContinueSimulating event handler\n");
        }
        else
        {
            DrainIsochEndpoints();
        }
    }

    ::printf("AudioDevice.StartSimulation terminating simulation\n");

    // Remove the event sink from the control endpoint
    IfFailHrGo(ReleaseConnectionPoint());
    
Exit:
    return hr;
}

VOID CAudioDevice::DrainIsochEndpoints()
/*++
Routine Description:
   Iterates through all isochronous endpoints and drains their OUT
   queues. For now data is discarded. In future this would be the place
   to add data integrity checking.


Arguments:
    None

Return value:
    None
--*/
{
    ULONG                 cTransfersRemaining = 0;
    HRESULT               hr                  = S_OK;
    SOFTUSB_OUT_TRANSFER *pTransfers          = NULL;
    SOFTUSB_OUT_TRANSFER *pTransfersNext      = NULL;       // the next element in the list
    ISoftUSBInterface    *piCurrentAlternate  = NULL;
    ISoftUSBEndpoints    *piEndpoints         = NULL;
    BYTE                  bAlternateInterface = 0;
    VARIANT               varIndex; ::VariantInit(&varIndex);

    //Get the current alternate for the streaming interface
    IfFailHrGo(m_piConfig->get_AlternateForInterface(m_bStreamInterface, &piCurrentAlternate));


    IfFailHrGo(piCurrentAlternate->get_AlternateSetting (&bAlternateInterface));

    //Check if we have the same alternate interface
    if (m_bAlternateInterface != bAlternateInterface)
    {   
        //Set the variant to retrieve the 
        varIndex.vt = VT_I4;
        varIndex.lVal = 1;
        
        // A new alternate interface has been selected update the current interface
        RELEASE(m_piCurrentInterface);
        piCurrentAlternate->AddRef();
        m_piCurrentInterface = piCurrentAlternate;
        m_bAlternateInterface = bAlternateInterface;

        // Need to update the current endpoint
        // First release the current endpoint
        RELEASE(m_piEndpoint);
        
        //Need to get the endpoint from the interface to drain it
        IfFailHrGo(m_piCurrentInterface->get_Endpoints(&piEndpoints));

        //Read the endpoint from the collection
        IfFailHrGo(piEndpoints->get_Item(varIndex,
                                         reinterpret_cast<SoftUSBEndpoint**>(&m_piEndpoint)));
    
    }
       
    if (NULL != m_piEndpoint)
    {
        if (SUCCEEDED(m_piEndpoint->DrainOUTQueue(16, &cTransfersRemaining, &pTransfers)))
        {
            
            // iterate through the pTransfers and count elements in the list
            if (NULL != pTransfers)
            {
                pTransfersNext = pTransfers;                    // save the head of the transfers
                do
                {
                    // Macro will point to next structure by pointing to the end of the struct 
                    // that contains the field NextTransfer, hence pointing to the beginning of the next struct
                    pTransfersNext = (SOFTUSB_OUT_TRANSFER*)CONTAINING_RECORD(pTransfersNext->NextTransfer.Flink,
                                                                              SOFTUSB_OUT_TRANSFER,
                                                                              NextTransfer);
                    m_lPacketCount++;                          // there will always be one element in the list
                }while(pTransfersNext != pTransfers);           // if the elements are at the same address then finish 
            }
            if (0 != cTransfersRemaining)
            {
                ::printf("Received isoch OUT transfers. %lu transfers remaining to be dequeued. %lu packets received.\n", cTransfersRemaining, m_lPacketCount);
            }
            (void)m_piEndpoint->FreeOUTQueue(pTransfers);
        }
    }
    
Exit:
    //Release any reference that have been taken
    RELEASE(piCurrentAlternate);
    RELEASE(piEndpoints);
}


STDMETHODIMP CAudioDevice::DisplayDeviceRequests(VARIANT_BOOL fvarDisplay)
/*++
Routine Description:
   Implements ISoftUSBAudio::DisplayDeviceRequests. Simply sets
   m_fDisplayDeviceRequests from the parameter fvarDisplay
   
Arguments:
    fvarDisplay - True=display audio class-specific device requests in
    console window. False=don't display requests.

Return value:
    S_OK
--*/
{
    if (VARIANT_TRUE == fvarDisplay)
    {
        m_fDisplayDeviceRequests = TRUE;
    }
    else
    {
        m_fDisplayDeviceRequests = VARIANT_FALSE;
    }

    return S_OK;
}


STDMETHODIMP CAudioDevice::AreKeystrokesWaiting(VARIANT_BOOL *pfvarKeyWaiting)
/*++
Routine Description:

   Implements ISoftUSBAudio::AreKeystrokesWaiting. It calls the low
   level IO function _kbhit to see if the use has pressed any keys. If
   so then the function returns VARIANT_TRUE otherwise it returns
   VARIANT_FALSE
   
Arguments:
    pfvarKeyWaiting - VARIANT_TRUE returned here if keys have been
    pressed

Return value:
    S_OK if the result is returned successfully

    E_POINTER if pfvarKeyWaiting is not valid
    
--*/
{
    HRESULT hr = S_OK;

    if (NULL == pfvarKeyWaiting)
    {
        IfFailHrGo(E_POINTER);
    }
    
    if (0 != _kbhit())
    {
        *pfvarKeyWaiting = VARIANT_TRUE;
    }
    else
    {
        *pfvarKeyWaiting = VARIANT_FALSE;
    }

Exit:
    return hr;
}



//ISoftUSBEndpointEvents

STDMETHODIMP CAudioDevice::OnSetupTransfer
(
    BYTE   DataToggle,
    BYTE  *pbDataBuffer,
    ULONG  cbDataBuffer,
    BYTE  *pbStatus
)
/*++
Routine Description:
   Implements the OnSetupTransfer for the interface ISoftUSBEndpointEvent

Arguments:
    DataToggle - DataToggle value
    pbDataBuffer - pointer to the buffer which contains the data
    cbDataBuffer - size of the data buffer
    pbStatus - pointer to transfer status value

Return value:
    E_NOTIMPL as this event is not handled by the device
--*/
{
    UNREFERENCED_PARAMETER(DataToggle);
    UNREFERENCED_PARAMETER(pbDataBuffer);
    UNREFERENCED_PARAMETER(cbDataBuffer);
    UNREFERENCED_PARAMETER(pbStatus);

    return E_NOTIMPL;
}

STDMETHODIMP CAudioDevice::OnWriteTransfer
(
    BYTE   DataToggle,
    BYTE  *pbDataBuffer,
    ULONG  cbDataBuffer,
    BYTE  *pbStatus
)
/*++
Routine Description:
   Implements the OnWriteTransfer for the interface ISoftUSBEndpointEvent

Arguments:
    DataToggle - DataToggle value
    pbDataBuffer - pointer to the buffer which contains the data
    cbDataBuffer - size of the data buffer
    pbStatus - pointer to transfer status value

Return value:
    E_UNEXPECTED if the IN endpoint is not valit
    Otherwise form call function
--*/
{
    UNREFERENCED_PARAMETER(DataToggle);
    UNREFERENCED_PARAMETER(pbDataBuffer);
    UNREFERENCED_PARAMETER(cbDataBuffer);
    UNREFERENCED_PARAMETER(pbStatus);

    return E_NOTIMPL;
}

STDMETHODIMP CAudioDevice::OnReadTransfer
(
    BYTE   DataToggle,
    BYTE  *pbDataBuffer,
    ULONG  cbDataBuffer,
    ULONG *cbDataWritten,
    BYTE  *pbStatus
)       
/*++
Routine Description:
   Implements the OnReadTransfer for the interface ISoftUSBEndpointEvent

Arguments:
    DataToggle - DataToggle value
    pbDataBuffer - pointer to the data beffer
    cbDataBuffer - size of the data buffer
    cbDataWritten - amount of data actually written to the buffer
    pbStatus - pointer to transfer status value

Return value:
    E_NOTIMPL as this event is not handled by the device
--*/
{
    UNREFERENCED_PARAMETER(DataToggle);
    UNREFERENCED_PARAMETER(pbDataBuffer);
    UNREFERENCED_PARAMETER(cbDataBuffer);
    UNREFERENCED_PARAMETER(cbDataWritten);
    UNREFERENCED_PARAMETER(pbStatus);

    return E_NOTIMPL;
}

STDMETHODIMP CAudioDevice::OnDeviceRequest
(
    USBSETUPREQUEST  *pSetupRequest,
    ULONG            *RequestHandle,
    BYTE             *pbHostData,
    ULONG            cbHostData,
    BYTE            **ppbResponseData,
    ULONG            *pcbResponseData,
    BYTE             *pbSetupStatus
)
/*++
Routine Description:
   Implements the OnDeviceRequest method of the ISoftUSBEndpointEvent
   interface

Arguments:
    pSetupRequest - pointer to the setup request begin handled
    RequestHandle - Handle fro the request
    pbHostData - pointer to a buffer which contain data sebt from the
                 host to the device for an OUT setup transfer
    cbHostData - amount of data in the HostData buffer
    ppbReponseData - pointer to data buffer to hold the response data
    pcbResponseData - pointer to the size of the data buffer
    pbSetpStatus - pointer to the setup status

Return value:
    E_NOTIMPL as this event is not handled by the device
--*/
{
    HRESULT hr = E_NOTIMPL;

    UNREFERENCED_PARAMETER(RequestHandle);
    UNREFERENCED_PARAMETER(pbHostData);
    UNREFERENCED_PARAMETER(cbHostData);
    UNREFERENCED_PARAMETER(ppbResponseData);
    UNREFERENCED_PARAMETER(pcbResponseData);
    UNREFERENCED_PARAMETER(pbSetupStatus);

    if (m_fDisplayDeviceRequests)
    {
        DisplayDeviceRequest(pSetupRequest);
    }

    if (TYPE_CLASS != pSetupRequest->bmRequestType.Bits.Type)
    {
        // Received non-class specific device request
        if (m_fDisplayDeviceRequests)
        {
            ::printf("Received non-class specific device request. Ignoring.\n");
        }
        goto Exit;
    }

    if (RCPT_INTERFACE == pSetupRequest->bmRequestType.Bits.Recipient)
    {
        if (0 == LOBYTE(pSetupRequest->sSetupIndex))
        {
            // Request to audio control interface
            IfFailHrGo(ProcessAudioControlInterfaceRequest(pSetupRequest,
                                                           pbHostData,
                                                           cbHostData,
                                                           ppbResponseData,
                                                           pcbResponseData,
                                                           pbSetupStatus));
        }
    }
    else if (RCPT_ENDPOINT == pSetupRequest->bmRequestType.Bits.Recipient)
    {
        // Request to endpoint
        IfFailHrGo(ProcessEndpointRequest(pSetupRequest,
                                          pbHostData,
                                          cbHostData,
                                          ppbResponseData,
                                          pcbResponseData,
                                          pbSetupStatus));
    }

Exit:
    return hr;
}

HRESULT CAudioDevice::ProcessEndpointRequest
(
    USBSETUPREQUEST  *pSetupRequest,
    BYTE             *pbHostData,
    ULONG             cbHostData,
    BYTE            **ppbResponseData,
    ULONG            *pcbResponseData,
    BYTE             *pbSetupStatus
)
/*++
Routine Description:
   Process class-specific device requests sent to an endpoint

Arguments:
    pSetupRequest - pointer to the request

Return value:
    
--*/
{
    HRESULT hr = S_OK;

    ::printf("ProcessEndpointRequest ");

    // Determine which endpoint is the target of the request
    switch (pSetupRequest->sSetupIndex)
    {
        case 4: // isoch endpoint
            // Determine which control selector is the target
            switch (HIBYTE(pSetupRequest->sSetupValue))
            {
                case SAMPLING_FREQ_CONTROL:
                    if (GET_CUR == pSetupRequest->bRequest)
                    {
                        if (m_fDisplayDeviceRequests)
                        {
                            ::printf("isoch endpoint 4, GET_CUR SAMPLING_FREQ_CONTROL %02.2X %02.2X %02.2X\n", m_abCurIsochEndpointSampFreq[0], m_abCurIsochEndpointSampFreq[1], m_abCurIsochEndpointSampFreq[2]);
                        }
                        *ppbResponseData = (BYTE *)::CoTaskMemAlloc(sizeof(m_abCurIsochEndpointSampFreq));
                        IfFalseHrGo(NULL != *ppbResponseData, E_OUTOFMEMORY);
                        ::memcpy(*ppbResponseData, m_abCurIsochEndpointSampFreq, sizeof(m_abCurIsochEndpointSampFreq));
                        *pcbResponseData = sizeof(m_abCurIsochEndpointSampFreq);
                        *pbSetupStatus = USB_ACK;
                        hr = S_OK;
                    }
                    else if (SET_CUR == pSetupRequest->bRequest)
                    {
                        if (cbHostData == sizeof(m_abCurIsochEndpointSampFreq))
                        {
                            ::memcpy(m_abCurIsochEndpointSampFreq, pbHostData, sizeof(m_abCurIsochEndpointSampFreq));
                        }
                        if (m_fDisplayDeviceRequests)
                        {
                            ::printf("isoch endpoint 4, SET_CUR SAMPLING_FREQ_CONTROL %02.2X %02.2X %02.2X\n", m_abCurIsochEndpointSampFreq[0], m_abCurIsochEndpointSampFreq[1], m_abCurIsochEndpointSampFreq[2]);
                        }
                        *pbSetupStatus = USB_ACK;
                        hr = S_OK;
                    }
                    else if (GET_MIN == pSetupRequest->bRequest)
                    {
                        if (m_fDisplayDeviceRequests)
                        {
                            ::printf("isoch endpoint 4, GET_MIN SAMPLING_FREQ_CONTROL %02.2X %02.2X %02.2X\n", m_abMinIsochEndpointSampFreq[0], m_abMinIsochEndpointSampFreq[1], m_abMinIsochEndpointSampFreq[2]);
                        }
                        *ppbResponseData = (BYTE *)::CoTaskMemAlloc(sizeof(m_abMinIsochEndpointSampFreq));
                        IfFalseHrGo(NULL != *ppbResponseData, E_OUTOFMEMORY);
                        ::memcpy(*ppbResponseData, m_abMinIsochEndpointSampFreq, sizeof(m_abMinIsochEndpointSampFreq));
                        *pcbResponseData = sizeof(m_abMinIsochEndpointSampFreq);
                        *pbSetupStatus = USB_ACK;
                        hr = S_OK;
                    }
                    else if (GET_MAX == pSetupRequest->bRequest)
                    {
                        if (m_fDisplayDeviceRequests)
                        {
                            ::printf("isoch endpoint 4, GET_MAX SAMPLING_FREQ_CONTROL %02.2X %02.2X %02.2X\n", m_abMaxIsochEndpointSampFreq[0], m_abMaxIsochEndpointSampFreq[1], m_abMaxIsochEndpointSampFreq[2]);
                        }
                        *ppbResponseData = (BYTE *)::CoTaskMemAlloc(sizeof(m_abMaxIsochEndpointSampFreq));
                        IfFalseHrGo(NULL != *ppbResponseData, E_OUTOFMEMORY);
                        ::memcpy(*ppbResponseData, m_abMaxIsochEndpointSampFreq, sizeof(m_abMaxIsochEndpointSampFreq));
                        *pcbResponseData = sizeof(m_abMaxIsochEndpointSampFreq);
                        *pbSetupStatus = USB_ACK;
                        hr = S_OK;
                    }
                    else if (GET_RES == pSetupRequest->bRequest)
                    {
                        if (m_fDisplayDeviceRequests)
                        {
                            ::printf("isoch endpoint 4, GET_RES SAMPLING_FREQ_CONTROL %02.2X %02.2X %02.2X\n", m_abResIsochEndpointSampFreq[0], m_abResIsochEndpointSampFreq[1], m_abResIsochEndpointSampFreq[2]);
                        }
                        *ppbResponseData = (BYTE *)::CoTaskMemAlloc(sizeof(m_abResIsochEndpointSampFreq));
                        IfFalseHrGo(NULL != *ppbResponseData, E_OUTOFMEMORY);
                        ::memcpy(*ppbResponseData, m_abResIsochEndpointSampFreq, sizeof(m_abResIsochEndpointSampFreq));
                        *pcbResponseData = sizeof(m_abResIsochEndpointSampFreq);
                        *pbSetupStatus = USB_ACK;
                        hr = S_OK;
                    }
                    break; // SAMPLING_FREQ_CONTROL

                case PITCH_CONTROL:
                    if (SET_CUR == pSetupRequest->bRequest)
                    {
                        *pbSetupStatus = USB_ACK;
                        ::printf("cbHostData = %lu ", cbHostData);
                        IfFalseHrGo(cbHostData > 0, S_OK);
                        if (m_fDisplayDeviceRequests)
                        {
                            ::printf("PITCH_CONTROL enabled=%s ", (1 == *pbHostData) ? "true" : "false");
                        }
                        if (1 == *pbHostData)
                        {
                            m_fPitchControlEnabled = TRUE;
                        }
                        else
                        {
                            m_fPitchControlEnabled = FALSE;
                        }
                        hr = S_OK;
                    }
                    else if (GET_CUR == pSetupRequest->bRequest)
                    {
                        if (m_fDisplayDeviceRequests)
                        {
                            ::printf("PITCH_CONTROL returning enabled=%s ", m_fPitchControlEnabled ? "true" : "false");
                        }
                        *ppbResponseData = (BYTE *)::CoTaskMemAlloc(sizeof(m_fPitchControlEnabled));
                        IfFalseHrGo(NULL != *ppbResponseData, E_OUTOFMEMORY);
                        **ppbResponseData = m_fPitchControlEnabled;
                        *pcbResponseData = sizeof(m_fPitchControlEnabled);
                        *pbSetupStatus = USB_ACK;
                        hr = S_OK;
                    }
                    break; // PITCH_CONTROL

                default:
                {
                    ::printf("***************Received isoch endpoint request for unknown control selector HIBYTE(pSetupRequest->sSetupValue)==%u\r\n", HIBYTE(pSetupRequest->sSetupValue));
                    hr = E_NOTIMPL;
                }
                break;
            }
            break; // switch (HIBYTE(pSetupRequest->sSetupValue)) for endpoint 4

        default:
        {
            ::printf("***************Received endpoint request for unknown endpoint pSetupRequest->sSetupIndex==%u\r\n", pSetupRequest->sSetupIndex);
            hr = E_NOTIMPL;
        }
        break;
    } // switch endpoint number

Exit:
    return hr;
}



HRESULT CAudioDevice::ProcessAudioControlInterfaceRequest
(
    USBSETUPREQUEST  *pSetupRequest,
    BYTE             *pbHostData,
    ULONG            cbHostData,
    BYTE            **ppbResponseData,
    ULONG            *pcbResponseData,
    BYTE             *pbSetupStatus
)
/*++
Routine Description:
   Process class-specific device requests sent to the audio control
   interface.

Arguments:
    pSetupRequest - pointer to the request

Return value:
    
--*/
{
    HRESULT hr = S_OK;

    // Determine which entity is the target of the request and call
    // corresponding routine 
    switch (HIBYTE(pSetupRequest->sSetupIndex))
    {
        case UPDOWN_MIX_PROCESSING_UNIT_2:
        case UPDOWN_MIX_PROCESSING_UNIT_3:
            hr = ProcessUpDownMixProcessingRequest(pSetupRequest,
                                                   pbHostData,
                                                   cbHostData,
                                                   ppbResponseData,
                                                   pcbResponseData,
                                                   pbSetupStatus);
            break;

        case GRAPHIC_EQUALIZER_UNIT :
        case LINE_CONNECTOR_INPUT_VOLUME:
        case COMBINED_OUTPUT_FEATURE_UNIT :
        case LOW_FREQ_VOLUME_UNIT:
            hr = ProcessFeatureUnitRequest(pSetupRequest,
                                           pbHostData,
                                           cbHostData,
                                           ppbResponseData,
                                           pcbResponseData,
                                           pbSetupStatus);
            break;

        case LINE_IN_AND_USB_MIXER_UNIT:
        case LOW_FREQ_MIXER_UNIT:
            hr = ProcessMixerUnitRequest(pSetupRequest,
                                         pbHostData,
                                         cbHostData,
                                         ppbResponseData,
                                         pcbResponseData,
                                         pbSetupStatus);
            break;

        default:
        {
            ::printf("***************Received Audio Control Interface Request request for unknown entity HIBYTE(pSetupRequest->sSetupIndex)==%u\r\n", HIBYTE(pSetupRequest->sSetupIndex));
            hr = E_NOTIMPL;
        }
        break;
    }


    return hr;
}


HRESULT CAudioDevice::ProcessUpDownMixProcessingRequest
(
    USBSETUPREQUEST  *pSetupRequest,
    BYTE             *pbHostData,
    ULONG             cbHostData,
    BYTE            **ppbResponseData,
    ULONG            *pcbResponseData,
    BYTE             *pbSetupStatus
)
/*++
Routine Description:
   Process class-specific device requests sent to the up/down mix processing units

Arguments:
    pSetupRequest - pointer to the request

Return value:

--*/
{
    HRESULT hr = E_NOTIMPL;

    UNREFERENCED_PARAMETER(pbHostData);
    UNREFERENCED_PARAMETER(cbHostData);

    switch (HIBYTE(pSetupRequest->sSetupValue))
    {
        case UD_ENABLE_CONTROL:
        {
            if (GET_CUR == pSetupRequest->bRequest)
            {
                if (m_fDisplayDeviceRequests)
                {
                    ::printf("UD_ENABLE_CONTROL returning 1 ");
                }
                *ppbResponseData = (BYTE *)::CoTaskMemAlloc(1UL);
                IfFalseHrGo(NULL != *ppbResponseData, E_OUTOFMEMORY);
                **ppbResponseData = 1; // enabled
                *pcbResponseData = 1;
                *pbSetupStatus = USB_ACK;
                hr = S_OK;
            }
        }
        break;

        case UD_MODE_SELECT_CONTROL:
        {
            if (GET_CUR == pSetupRequest->bRequest)
            {
                *ppbResponseData = (BYTE *)::CoTaskMemAlloc(sizeof(m_ProcessingUnitMode2[0]));
                IfFalseHrGo(NULL != *ppbResponseData, E_OUTOFMEMORY);
                *pbSetupStatus = USB_ACK;
                hr = S_OK;
                if (UPDOWN_MIX_PROCESSING_UNIT_2 == HIBYTE(pSetupRequest->sSetupIndex))
                {
                    if (1 == LOBYTE(pSetupRequest->sSetupValue))
                    {
                        ::memcpy(*ppbResponseData, &m_ProcessingUnitMode2[0], sizeof(m_ProcessingUnitMode2[0]));
                        *pcbResponseData = sizeof(m_ProcessingUnitMode2[0]);
                        if (m_fDisplayDeviceRequests)
                        {
                            ::printf("UD_MODE_SELECT_CONTROL Unit 2 Channel 1 returning %u ", m_ProcessingUnitMode2[0]);
                        }
                    }
                    else if (2 == LOBYTE(pSetupRequest->sSetupValue))
                    {
                        ::memcpy(*ppbResponseData, &m_ProcessingUnitMode2[1], sizeof(m_ProcessingUnitMode2[1]));
                        *pcbResponseData = sizeof(m_ProcessingUnitMode2[1]);
                        if (m_fDisplayDeviceRequests)
                        {
                            ::printf("UD_MODE_SELECT_CONTROL Unit 2 Channel 2 returning %u ", m_ProcessingUnitMode2[1]);
                        }
                    }
                    else if (3 == LOBYTE(pSetupRequest->sSetupValue))
                    {
                        ::memcpy(*ppbResponseData, &m_ProcessingUnitMode2[2], sizeof(m_ProcessingUnitMode2[2]));
                        *pcbResponseData = sizeof(m_ProcessingUnitMode2[2]);
                        if (m_fDisplayDeviceRequests)
                        {
                            ::printf("UD_MODE_SELECT_CONTROL Unit 2 Channel 3 returning %u ", m_ProcessingUnitMode2[2]);
                        }
                    }
                    else if (4 == LOBYTE(pSetupRequest->sSetupValue))
                    {
                        ::memcpy(*ppbResponseData, &m_ProcessingUnitMode2[3], sizeof(m_ProcessingUnitMode2[3]));
                        *pcbResponseData = sizeof(m_ProcessingUnitMode2[3]);
                        if (m_fDisplayDeviceRequests)
                        {
                            ::printf("UD_MODE_SELECT_CONTROL Unit 2 Channel 4 returning %u ", m_ProcessingUnitMode2[3]);
                        }
                    }
                }
                if (UPDOWN_MIX_PROCESSING_UNIT_3 == HIBYTE(pSetupRequest->sSetupIndex))
                {
                    if (1 == LOBYTE(pSetupRequest->sSetupValue))
                    {
                        ::memcpy(*ppbResponseData, &m_ProcessingUnitMode3[0], sizeof(m_ProcessingUnitMode3[0]));
                        *pcbResponseData = sizeof(m_ProcessingUnitMode3[0]);
                        if (m_fDisplayDeviceRequests)
                        {
                            ::printf("UD_MODE_SELECT_CONTROL Unit 3 Channel 1 returning %u ", m_ProcessingUnitMode3[0]);
                        }
                    }
                    else if (2 == LOBYTE(pSetupRequest->sSetupValue))
                    {
                        ::memcpy(*ppbResponseData, &m_ProcessingUnitMode3[1], sizeof(m_ProcessingUnitMode3[1]));
                        *pcbResponseData = sizeof(m_ProcessingUnitMode3[1]);
                        if (m_fDisplayDeviceRequests)
                        {
                            ::printf("UD_MODE_SELECT_CONTROL Unit 3 Channel 2 returning %u ", m_ProcessingUnitMode3[1]);
                        }
                    }
                }
                
            }
            else if (SET_CUR == pSetupRequest->bRequest)
            {
                if (m_fDisplayDeviceRequests)
                {
                    ::printf("UD_MODE_SELECT_CONTROL received %u ", pbHostData[0]);
                }

                if (UPDOWN_MIX_PROCESSING_UNIT_2 == HIBYTE(pSetupRequest->sSetupIndex))
                {
                    if (0 == LOBYTE(pSetupRequest->sSetupValue))
                    {
                        if (cbHostData >= sizeof(m_ProcessingUnitMode2[0]))
                        {
                            ::memcpy(&m_ProcessingUnitMode2[0], pbHostData, sizeof(m_ProcessingUnitMode2[0]));
                            ::memcpy(&m_ProcessingUnitMode2[1], pbHostData, sizeof(m_ProcessingUnitMode2[1]));
                            ::memcpy(&m_ProcessingUnitMode2[2], pbHostData, sizeof(m_ProcessingUnitMode2[2]));
                            ::memcpy(&m_ProcessingUnitMode2[3], pbHostData, sizeof(m_ProcessingUnitMode2[3]));
                        }
                    }
                    else if (1 == LOBYTE(pSetupRequest->sSetupValue))
                    {
                        if (cbHostData >= sizeof(m_ProcessingUnitMode2[0]))
                        {
                            ::memcpy(&m_ProcessingUnitMode2[0], pbHostData, sizeof(m_ProcessingUnitMode2[0]));
                        }
                    }
                    else if (2 == LOBYTE(pSetupRequest->sSetupValue))
                    {
                        if (cbHostData >= sizeof(m_ProcessingUnitMode2[1]))
                        {
                            ::memcpy(&m_ProcessingUnitMode2[1], pbHostData, sizeof(m_ProcessingUnitMode2[1]));
                        }
                    }
                    else if (3 == LOBYTE(pSetupRequest->sSetupValue))
                    {
                        if (cbHostData >= sizeof(m_ProcessingUnitMode2[2]))
                        {
                            ::memcpy(&m_ProcessingUnitMode2[2], pbHostData, sizeof(m_ProcessingUnitMode2[2]));
                        }
                    }
                    else if (4 == LOBYTE(pSetupRequest->sSetupValue))
                    {
                        if (cbHostData >= sizeof(m_ProcessingUnitMode2[3]))
                        {
                            ::memcpy(&m_ProcessingUnitMode2[3], pbHostData, sizeof(m_ProcessingUnitMode2[3]));
                        }
                    }
                }
                if (UPDOWN_MIX_PROCESSING_UNIT_3 == HIBYTE(pSetupRequest->sSetupIndex))
                {
                    if (0 == LOBYTE(pSetupRequest->sSetupValue))
                    {
                        if (cbHostData >= sizeof(m_ProcessingUnitMode3[0]))
                        {
                            ::memcpy(&m_ProcessingUnitMode3[0], pbHostData, sizeof(m_ProcessingUnitMode3[0]));
                            ::memcpy(&m_ProcessingUnitMode3[1], pbHostData, sizeof(m_ProcessingUnitMode3[1]));
                        }
                    }
                    else if (1 == LOBYTE(pSetupRequest->sSetupValue))
                    {
                        if (cbHostData >= sizeof(m_ProcessingUnitMode3[0]))
                        {
                            ::memcpy(&m_ProcessingUnitMode3[0], pbHostData, sizeof(m_ProcessingUnitMode3[0]));
                        }
                    }
                    else if (2 == LOBYTE(pSetupRequest->sSetupValue))
                    {
                        if (cbHostData >= sizeof(m_ProcessingUnitMode3[1]))
                        {
                            ::memcpy(&m_ProcessingUnitMode3[1], pbHostData, sizeof(m_ProcessingUnitMode3[1]));
                        }
                    }
                }
                *pbSetupStatus = USB_ACK;
                hr = S_OK;
            }
            else if (GET_MIN == pSetupRequest->bRequest)
            {
                if (m_fDisplayDeviceRequests)
                {
                    ::printf("UD_MODE_SELECT_CONTROL returning 1");
                }
                *ppbResponseData = (BYTE *)::CoTaskMemAlloc(sizeof(BYTE));
                IfFalseHrGo(NULL != *ppbResponseData, E_OUTOFMEMORY);
                **ppbResponseData = (BYTE)1;
                *pcbResponseData = sizeof(BYTE);
                *pbSetupStatus = USB_ACK;
                hr = S_OK;
            }
            else if (GET_MAX == pSetupRequest->bRequest)
            {
                if (m_fDisplayDeviceRequests)
                {
                    ::printf("UD_MODE_SELECT_CONTROL returning 1");
                }
                *ppbResponseData = (BYTE *)::CoTaskMemAlloc(sizeof(BYTE));
                IfFalseHrGo(NULL != *ppbResponseData, E_OUTOFMEMORY);
                **ppbResponseData = (BYTE)1;
                *pcbResponseData = sizeof(BYTE);
                *pbSetupStatus = USB_ACK;
                hr = S_OK;
            }
            else if (GET_RES == pSetupRequest->bRequest)
            {
                if (m_fDisplayDeviceRequests)
                {
                    ::printf("UD_MODE_SELECT_CONTROL returning 1");
                }
                *ppbResponseData = (BYTE *)::CoTaskMemAlloc(sizeof(BYTE));
                IfFalseHrGo(NULL != *ppbResponseData, E_OUTOFMEMORY);
                **ppbResponseData = (BYTE)1;
                *pcbResponseData = sizeof(BYTE);
                *pbSetupStatus = USB_ACK;
                hr = S_OK;
            }
        }
        break;

        default:
        {
            ::printf("***************ProcessUpDownMixProcessingRequest received unknown request HIBYTE(pSetupRequest->sSetupValue)==%u\r\n", HIBYTE(pSetupRequest->sSetupValue));
            hr = E_NOTIMPL;
        }
        break;
    }
            

Exit:
    return hr;
}



HRESULT CAudioDevice::ProcessFeatureUnitRequest
(
    USBSETUPREQUEST  *pSetupRequest,
    BYTE             *pbHostData,
    ULONG             cbHostData, 
    BYTE            **ppbResponseData,
    ULONG            *pcbResponseData,
    BYTE             *pbSetupStatus
)
/*++
Routine Description:
   Process class-specific device requests sent to the feature units

Arguments:
    pSetupRequest - pointer to the request

Return value:

--*/
{
    HRESULT hr       = E_NOTIMPL;
    BYTE    bChannel = LOBYTE(pSetupRequest->sSetupValue);

    UNREFERENCED_PARAMETER(pbHostData);
    UNREFERENCED_PARAMETER(cbHostData);

    // high byte of value is control selector
    // low byte of value is channel number
    
    if (m_fDisplayDeviceRequests)
    {
        ::printf("channel number=%u ",bChannel );
    }

    switch (HIBYTE(pSetupRequest->sSetupValue))
    {
        case VOLUME_CONTROL:
        {
            if (GET_MIN == pSetupRequest->bRequest)
            {
                if (m_fDisplayDeviceRequests)
                {
                    ::printf("VOLUME_CONTROL returning 0x0x9CFF= - 90dB ");
                }
                *ppbResponseData = (BYTE *)::CoTaskMemAlloc(sizeof(WORD));
                IfFalseHrGo(NULL != *ppbResponseData, E_OUTOFMEMORY);
                *((WORD *)*ppbResponseData) = 0x9CFF; // -90 dB
                *pcbResponseData = sizeof(WORD);
                *pbSetupStatus = USB_ACK;
                hr = S_OK;
            }
            else if (GET_MAX == pSetupRequest->bRequest)
            {
                if (m_fDisplayDeviceRequests)
                {
                    ::printf("VOLUME_CONTROL returning 0x0000=0 dB ");
                }
                *ppbResponseData = (BYTE *)::CoTaskMemAlloc(sizeof(WORD));
                IfFalseHrGo(NULL != *ppbResponseData, E_OUTOFMEMORY);
                *((WORD *)*ppbResponseData) = 0x0000; // 0 dB
                *pcbResponseData = sizeof(WORD);
                *pbSetupStatus = USB_ACK;
                hr = S_OK;
            }
            else if (GET_RES == pSetupRequest->bRequest)
            {
                if (m_fDisplayDeviceRequests)
                {
                    ::printf("VOLUME_CONTROL returning 0x0200=2 dB ");
                }
                *ppbResponseData = (BYTE *)::CoTaskMemAlloc(sizeof(WORD));
                IfFalseHrGo(NULL != *ppbResponseData, E_OUTOFMEMORY);
                *((WORD *)*ppbResponseData) = 0x0200; // 2 dB
                *pcbResponseData = sizeof(WORD);
                *pbSetupStatus = USB_ACK;
                hr = S_OK;
            }
            else if (GET_CUR == pSetupRequest->bRequest)
            {
                *ppbResponseData = (BYTE *)::CoTaskMemAlloc(sizeof(WORD));
                IfFalseHrGo(NULL != *ppbResponseData, E_OUTOFMEMORY);
                *pcbResponseData = sizeof(WORD);
                *pbSetupStatus = USB_ACK;
                hr = S_OK;
                switch (HIBYTE(pSetupRequest->sSetupIndex))
                {
                    case LINE_CONNECTOR_INPUT_VOLUME:
                        if (m_fDisplayDeviceRequests)
                        {
                            ::printf("VOLUME_CONTROL returning %u ", m_wLineVol);
                        }
                        *((WORD *)*ppbResponseData) = m_wLineVol;
                        break;
                    case LOW_FREQ_VOLUME_UNIT:
                        if (m_fDisplayDeviceRequests)
                        {
                            ::printf("VOLUME_CONTROL returning %u ", m_wLowFreqVol);
                        }
                        *((WORD *)*ppbResponseData) = m_wLowFreqVol;
                        break;
                    case COMBINED_OUTPUT_FEATURE_UNIT:               
                        if (0 == bChannel || 1 == bChannel)
                        {
                            if (m_fDisplayDeviceRequests)
                            {
                                ::printf("VOLUME_CONTROL returning Ch 1 %u ", m_wCombineCh1Vol);
                            }
                            *((WORD *)*ppbResponseData) = m_wCombineCh1Vol;
                        }
                        
                        if (0 == bChannel || 2 == bChannel)
                        {
                            if (m_fDisplayDeviceRequests)
                            {
                                ::printf("VOLUME_CONTROL returning Ch 2 %u ", m_wCombineCh2Vol);
                            }
                            *((WORD *)*ppbResponseData) = m_wCombineCh2Vol;
                        }
                        break;
                    case GRAPHIC_EQUALIZER_UNIT :
                    default:
                        if (m_fDisplayDeviceRequests)
                        {
                            ::printf("VOLUME_CONTROL returning %u ", m_wVol);
                        }
                        *((WORD *)*ppbResponseData) = m_wVol;
                }
            }
            else if (SET_CUR == pSetupRequest->bRequest)
            {
                *pbSetupStatus = USB_ACK;
                ::printf("cbHostData = %lu ", cbHostData);
                IfFalseHrGo(cbHostData >= sizeof(m_wVol), S_OK);
                hr = S_OK;
                switch (HIBYTE(pSetupRequest->sSetupIndex))
                {
                    case LINE_CONNECTOR_INPUT_VOLUME:
                        m_wLineVol = *(WORD *)pbHostData;
                        if (m_fDisplayDeviceRequests)
                        {
                            ::printf("VOLUME_CONTROL %u ", m_wLineVol);
                        }
                        break;
                    case LOW_FREQ_VOLUME_UNIT:
                        m_wLowFreqVol = *(WORD *)pbHostData;
                        if (m_fDisplayDeviceRequests)
                        {
                            ::printf("VOLUME_CONTROL %u ", m_wLowFreqVol);
                        }
                        break;
                    case COMBINED_OUTPUT_FEATURE_UNIT :
                        if (0 == bChannel || 1 == bChannel)
                        {
                            m_wCombineCh1Vol = *(WORD *)pbHostData;
                            if (m_fDisplayDeviceRequests)
                            {
                                ::printf("VOLUME_CONTROL Ch 1%u ", m_wCombineCh1Vol);
                            }
                        }
                        
                        if (0 == bChannel || 2 == bChannel)
                        {
                            m_wCombineCh2Vol = *(WORD *)pbHostData;
                            if (m_fDisplayDeviceRequests)
                            {
                                ::printf("VOLUME_CONTROL Ch2  %u ", m_wCombineCh2Vol);
                            }
                        }
                        break;
                    case GRAPHIC_EQUALIZER_UNIT :
                    default:
                        m_wVol = *(WORD *)pbHostData;
                        if (m_fDisplayDeviceRequests)
                        {
                            ::printf("VOLUME_CONTROL %u ", m_wVol);
                        }
                }
            }
        }
        break;

        case BASS_CONTROL:
        {
            if (GET_MIN == pSetupRequest->bRequest)
            {
                if (m_fDisplayDeviceRequests)
                {
                    ::printf("BASS_CONTROL returning 0xD0=-12.00 dB ");
                }
                *ppbResponseData = (BYTE *)::CoTaskMemAlloc(sizeof(BYTE));
                IfFalseHrGo(NULL != *ppbResponseData, E_OUTOFMEMORY);
                **ppbResponseData = 0xD0; // -12.00 dB
                *pcbResponseData = sizeof(BYTE);
                *pbSetupStatus = USB_ACK;
                hr = S_OK;
            }
            else if (GET_MAX == pSetupRequest->bRequest)
            {
                if (m_fDisplayDeviceRequests)
                {
                    ::printf("BASS_CONTROL returning 0x30=12.00 dB ");
                }
                *ppbResponseData = (BYTE *)::CoTaskMemAlloc(sizeof(BYTE));
                IfFalseHrGo(NULL != *ppbResponseData, E_OUTOFMEMORY);
                **ppbResponseData = 0x30; // 12.00 dB
                *pcbResponseData = sizeof(BYTE);
                *pbSetupStatus = USB_ACK;
                hr = S_OK;
            }
            else if (GET_RES == pSetupRequest->bRequest)
            {
                if (m_fDisplayDeviceRequests)
                {
                    ::printf("BASS_CONTROL returning 0x08=2.00 dB ");
                }
                *ppbResponseData = (BYTE *)::CoTaskMemAlloc(sizeof(BYTE));
                IfFalseHrGo(NULL != *ppbResponseData, E_OUTOFMEMORY);
                **ppbResponseData = 0x08; // 2.00 dB
                *pcbResponseData = sizeof(BYTE);
                *pbSetupStatus = USB_ACK;
                hr = S_OK;
            }
            else if (GET_CUR == pSetupRequest->bRequest)
            {
                *ppbResponseData = (BYTE *)::CoTaskMemAlloc(sizeof(BYTE));
                IfFalseHrGo(NULL != *ppbResponseData, E_OUTOFMEMORY);
                *pcbResponseData = sizeof(BYTE);
                *pbSetupStatus = USB_ACK;
                hr = S_OK;
                switch (HIBYTE(pSetupRequest->sSetupIndex))
                {
                    case LINE_CONNECTOR_INPUT_VOLUME:
                        if (m_fDisplayDeviceRequests)
                        {
                            ::printf("BASS_CONTROL returning %u ", m_bLineBass);
                        }
                        **ppbResponseData = m_bLineBass;
                        break;
                    case LOW_FREQ_VOLUME_UNIT:
                        if (m_fDisplayDeviceRequests)
                        {
                            ::printf("BASS_CONTROL returning %u ", m_bLowFreqBass);
                        }
                        **ppbResponseData = m_bLowFreqBass;
                        break;
                    case COMBINED_OUTPUT_FEATURE_UNIT :
                        if (0 == bChannel || 1 == bChannel)
                        {
                            if (m_fDisplayDeviceRequests)
                            {
                                ::printf("BASS_CONTROL Ch 1 returning %u ", m_bCombineCh1Bass);
                            }
                            **ppbResponseData = m_bCombineCh1Bass;
                        }
                        
                        if (0 == bChannel || 2 == bChannel)
                        {
                            if (m_fDisplayDeviceRequests)
                            {
                                ::printf("BASS_CONTROL Ch 2 returning %u ", m_bCombineCh2Bass);
                            }
                            **ppbResponseData = m_bCombineCh2Bass;
                        }
                        break;
                    case GRAPHIC_EQUALIZER_UNIT :
                    default:
                        if (m_fDisplayDeviceRequests)
                        {
                            ::printf("BASS_CONTROL returning %u ", m_bBass);
                        }
                        **ppbResponseData = m_bBass;
                }            
            }
            else if (SET_CUR == pSetupRequest->bRequest)
            {
                *pbSetupStatus = USB_ACK;
                ::printf("cbHostData = %lu ", cbHostData);
                IfFalseHrGo(cbHostData >= sizeof(m_bBass), S_OK);
                hr = S_OK;
                switch (HIBYTE(pSetupRequest->sSetupIndex))
                {
                    case LINE_CONNECTOR_INPUT_VOLUME:
                        m_bLineBass = *pbHostData;
                        if (m_fDisplayDeviceRequests)
                        {
                            ::printf("BASS_CONTROL %u ", m_bLineBass);
                        }
                        break;
                    case LOW_FREQ_VOLUME_UNIT:
                        m_bLowFreqBass = *pbHostData;
                        if (m_fDisplayDeviceRequests)
                        {
                            ::printf("BASS_CONTROL %u ", m_bLowFreqBass);
                        }
                        break;
                    case COMBINED_OUTPUT_FEATURE_UNIT :
                        if (0 == bChannel || 1 == bChannel)
                        {
                            m_bCombineCh1Bass = *pbHostData;
                            if (m_fDisplayDeviceRequests)
                            {
                                ::printf("BASS_CONTROL Ch 1 %u ", m_bCombineCh1Bass);
                            }
                        }
                        
                        if (0 == bChannel || 2 == bChannel)
                        {
                            m_bCombineCh2Bass = *pbHostData;
                            if (m_fDisplayDeviceRequests)
                            {
                                ::printf("BASS_CONTROL Ch 2 %u ", m_bCombineCh2Bass);
                            }
                        }

                        break;
                    case GRAPHIC_EQUALIZER_UNIT :
                    default:
                        m_bBass = *pbHostData;
                        if (m_fDisplayDeviceRequests)
                        {
                            ::printf("BASS_CONTROL %u ", m_bBass);
                        }
                }            
            }
        }
        break;

        case TREBLE_CONTROL:
        {
            if (GET_MIN == pSetupRequest->bRequest)
            {
                if (m_fDisplayDeviceRequests)
                {
                    ::printf("TREBLE_CONTROL returning 0xD0=-12.00 dB ");
                }
                *ppbResponseData = (BYTE *)::CoTaskMemAlloc(sizeof(BYTE));
                IfFalseHrGo(NULL != *ppbResponseData, E_OUTOFMEMORY);
                **ppbResponseData = 0xD0; // -12.00 dB
                *pcbResponseData = sizeof(BYTE);
                *pbSetupStatus = USB_ACK;
                hr = S_OK;
            }
            else if (GET_MAX == pSetupRequest->bRequest)
            {
                if (m_fDisplayDeviceRequests)
                {
                    ::printf("TREBLE_CONTROL returning 0x30=12.00 dB ");
                }
                *ppbResponseData = (BYTE *)::CoTaskMemAlloc(sizeof(BYTE));
                IfFalseHrGo(NULL != *ppbResponseData, E_OUTOFMEMORY);
                **ppbResponseData = 0x30; // 12.00 dB
                *pcbResponseData = sizeof(BYTE);
                *pbSetupStatus = USB_ACK;
                hr = S_OK;
            }
            else if (GET_RES == pSetupRequest->bRequest)
            {
                if (m_fDisplayDeviceRequests)
                {
                    ::printf("TREBLE_CONTROL returning 0x08=2.00 dB ");
                }
                *ppbResponseData = (BYTE *)::CoTaskMemAlloc(sizeof(BYTE));
                IfFalseHrGo(NULL != *ppbResponseData, E_OUTOFMEMORY);
                **ppbResponseData = 0x08; // 2.00 dB
                *pcbResponseData = sizeof(BYTE);
                *pbSetupStatus = USB_ACK;
                hr = S_OK;
            }
            else if (GET_CUR == pSetupRequest->bRequest)
            {
                *ppbResponseData = (BYTE *)::CoTaskMemAlloc(sizeof(BYTE));
                IfFalseHrGo(NULL != *ppbResponseData, E_OUTOFMEMORY);
                *pcbResponseData = sizeof(BYTE);
                *pbSetupStatus = USB_ACK;
                hr = S_OK;
                switch (HIBYTE(pSetupRequest->sSetupIndex))
                {
                    case LINE_CONNECTOR_INPUT_VOLUME:
                        if (m_fDisplayDeviceRequests)
                        {
                            ::printf("TREBLE_CONTROL returning %u ", m_bLineTreble);
                        }
                        **ppbResponseData = m_bLineTreble;
                        break;
                    case LOW_FREQ_VOLUME_UNIT:
                        if (m_fDisplayDeviceRequests)
                        {
                            ::printf("TREBLE_CONTROL returning %u ", m_bLowFreqTreble);
                        }
                        **ppbResponseData = m_bLowFreqTreble;
                        break;
                    case COMBINED_OUTPUT_FEATURE_UNIT :
                        if (0 == bChannel || 1 == bChannel)
                        {
                            if (m_fDisplayDeviceRequests)
                            {
                                ::printf("TREBLE_CONTROL Ch 1 returning %u ", m_bCombineCh1Treble);
                            }
                            **ppbResponseData = m_bCombineCh1Treble;
                        }
                        
                        if (0 == bChannel || 2 == bChannel)
                        {
                            if (m_fDisplayDeviceRequests)
                            {
                                ::printf("TREBLE_CONTROL Ch 2 returning %u ", m_bCombineCh2Treble);
                            }
                            **ppbResponseData = m_bCombineCh2Treble;
                        }
                        break;
                    case GRAPHIC_EQUALIZER_UNIT :
                    default:
                        if (m_fDisplayDeviceRequests)
                        {
                            ::printf("TREBLE_CONTROL returning %u ", m_bTreble);
                        }
                        **ppbResponseData = m_bTreble;
                }            
            }
            else if (SET_CUR == pSetupRequest->bRequest)
            {
                *pbSetupStatus = USB_ACK;
                ::printf("cbHostData = %lu ", cbHostData);
                IfFalseHrGo(cbHostData >= sizeof(m_bTreble), S_OK);
                hr = S_OK;
                switch (HIBYTE(pSetupRequest->sSetupIndex))
                {
                    case LINE_CONNECTOR_INPUT_VOLUME:
                        if (m_fDisplayDeviceRequests)
                        {
                            ::printf("TREBLE_CONTROL %u ", m_bLineTreble);
                        }
                        m_bLineTreble = *pbHostData;
                        break;
                    case LOW_FREQ_VOLUME_UNIT:
                        if (m_fDisplayDeviceRequests)
                        {
                            ::printf("TREBLE_CONTROL %u ", m_bLowFreqTreble);
                        }
                        m_bLowFreqTreble = *pbHostData;
                        break;
                    case COMBINED_OUTPUT_FEATURE_UNIT :
                        if (0 == bChannel || 1 == bChannel)
                        {
                            if (m_fDisplayDeviceRequests)
                            {
                                ::printf("TREBLE_CONTROL CH 1 %u ", m_bCombineCh1Treble);
                            }
                            m_bCombineCh1Treble = *pbHostData;
                        }
                        
                        if (0 == bChannel || 2 == bChannel)
                        {
                            if (m_fDisplayDeviceRequests)
                            {
                                ::printf("TREBLE_CONTROL CH 2 %u ", m_bCombineCh2Treble);
                            }
                            m_bCombineCh2Treble = *pbHostData;
                        }
                        break;
                    case GRAPHIC_EQUALIZER_UNIT :
                    default:
                        if (m_fDisplayDeviceRequests)
                        {
                            ::printf("TREBLE_CONTROL %u ", m_bTreble);
                        }
                        m_bTreble = *pbHostData;
                }
            }
        }
        break;

        case GRAPHIC_EQUALIZER_CONTROL:
        {
            if (GET_MIN == pSetupRequest->bRequest)
            {
                if (m_fDisplayDeviceRequests)
                {
                    ::printf("GRAPHIC_EQUALIZER_CONTROL returning 0x80=-32.00 dB ");
                }
                *ppbResponseData = (BYTE *)::CoTaskMemAlloc(sizeof(m_EqDataMin));
                IfFalseHrGo(NULL != *ppbResponseData, E_OUTOFMEMORY);
                ::memcpy(*ppbResponseData, &m_EqDataMin, sizeof(m_EqDataMin));
                *pcbResponseData = sizeof(m_EqDataMin);
                *pbSetupStatus = USB_ACK;
                hr = S_OK;
            }
            else if (GET_MAX == pSetupRequest->bRequest)
            {
                if (m_fDisplayDeviceRequests)
                {
                    ::printf("GRAPHIC_EQUALIZER_CONTROL returning 0x7F=31.75 dB ");
                }
                *ppbResponseData = (BYTE *)::CoTaskMemAlloc(sizeof(m_EqDataMax));
                IfFalseHrGo(NULL != *ppbResponseData, E_OUTOFMEMORY);
                ::memcpy(*ppbResponseData, &m_EqDataMax, sizeof(m_EqDataMax));
                *pcbResponseData = sizeof(m_EqDataMax);
                *pbSetupStatus = USB_ACK;
                hr = S_OK;
            }
            else if (GET_RES == pSetupRequest->bRequest)
            {
                if (m_fDisplayDeviceRequests)
                {
                    ::printf("GRAPHIC_EQUALIZER_CONTROL returning 1=2.00 dB ");
                }
                *ppbResponseData = (BYTE *)::CoTaskMemAlloc(sizeof(m_EqDataRes));
                IfFalseHrGo(NULL != *ppbResponseData, E_OUTOFMEMORY);
                ::memcpy(*ppbResponseData, &m_EqDataRes, sizeof(m_EqDataRes));
                *pcbResponseData = sizeof(m_EqDataRes);
                *pbSetupStatus = USB_ACK;
                hr = S_OK;
            }
            else if (GET_CUR == pSetupRequest->bRequest)
            {
                *ppbResponseData = (BYTE *)::CoTaskMemAlloc(sizeof(m_EqDataCur));
                IfFalseHrGo(NULL != *ppbResponseData, E_OUTOFMEMORY);
                ::memcpy(*ppbResponseData, &m_EqDataCur, sizeof(m_EqDataCur));
                *pcbResponseData = sizeof(m_EqDataCur);
                *pbSetupStatus = USB_ACK;
                hr = S_OK;
            }
            else if (SET_CUR == pSetupRequest->bRequest)
            {
                *pbSetupStatus = USB_ACK;
                IfFalseHrGo(cbHostData >= sizeof(m_EqDataCur), S_OK);
                ::memcpy(&m_EqDataCur, pbHostData, sizeof(m_EqDataCur));
                hr = S_OK;
            }
        }
        break;



        case MUTE_CONTROL:
        {
            if (SET_CUR == pSetupRequest->bRequest)
            {
                *pbSetupStatus = USB_ACK;
                ::printf("cbHostData = %lu ", cbHostData);
                IfFalseHrGo(cbHostData > 0, S_OK);
                if (m_fDisplayDeviceRequests)
                {
                    ::printf("MUTE_CONTROL muted=%s ", (1 == *pbHostData) ? "true" : "false");
                }
                if (1 == *pbHostData)
                {
                    m_fMuted = TRUE;
                }
                else
                {
                    m_fMuted = FALSE;
                }
                hr = S_OK;
            }
            else if (GET_CUR == pSetupRequest->bRequest)
            {
                if (m_fDisplayDeviceRequests)
                {
                    ::printf("MUTE_CONTROL returning muted=%s ", m_fMuted ? "true" : "false");
                }
                *ppbResponseData = (BYTE *)::CoTaskMemAlloc(sizeof(m_fMuted));
                IfFalseHrGo(NULL != *ppbResponseData, E_OUTOFMEMORY);
                **ppbResponseData = m_fMuted;
                *pcbResponseData = sizeof(m_fMuted);
                *pbSetupStatus = USB_ACK;
                hr = S_OK;
            }
        }
        break;

        default:
        {
            ::printf("***************ProcessFeatureUnitRequest request for unknown control selector HIBYTE(pSetupRequest->sSetupValue)==%u\r\n", HIBYTE(pSetupRequest->sSetupValue));
            hr = E_NOTIMPL;
        }
        break;
    }
            
Exit:
    return hr;
}



HRESULT CAudioDevice::ProcessMixerUnitRequest
(
    USBSETUPREQUEST  *pSetupRequest,
    BYTE             *pbHostData,
    ULONG             cbHostData,
    BYTE            **ppbResponseData,
    ULONG            *pcbResponseData,
    BYTE             *pbSetupStatus
)
/*++
Routine Description:
   Process class-specific device requests sent to the feature units

Arguments:
    pSetupRequest - pointer to the request

Return value:

--*/
{
    HRESULT hr = E_NOTIMPL;
    WORD    i = 0;

    UNREFERENCED_PARAMETER(pbHostData);
    UNREFERENCED_PARAMETER(cbHostData);

    // high byte of value is control selector
    // low byte of value is channel number
    
    if (m_fDisplayDeviceRequests)
    {
        ::printf("mixer unit %u input channel number=%u output channel number=%u ",
                 HIBYTE(pSetupRequest->sSetupIndex),
                 HIBYTE(pSetupRequest->sSetupValue),
                 LOBYTE(pSetupRequest->sSetupValue));
    }

    if (GET_MIN == pSetupRequest->bRequest)
    {
        if (m_fDisplayDeviceRequests)
        {
            ::printf("GET_MIN returning 0x0000=0 dB ");
        }
        if (pSetupRequest->sSetupIndex > 0)
        {
           *ppbResponseData = (BYTE *)::CoTaskMemAlloc(pSetupRequest->sSetupIndex);
            IfFalseHrGo(NULL != *ppbResponseData, E_OUTOFMEMORY);
            ZeroMemory(*ppbResponseData, pSetupRequest->sSetupIndex);
            for (i = 0; i < pSetupRequest->sSetupIndex / sizeof(WORD); i++)
            {
                ((WORD *)*ppbResponseData)[i] = 0x9CFF; // set all to -90 dB
            }
            *pcbResponseData = pSetupRequest->sSetupIndex;
        }
        else
        {
            *pcbResponseData = 0;
        }
        *pbSetupStatus = USB_ACK;
        hr = S_OK;
    }
    else if (GET_MAX == pSetupRequest->bRequest)
    {
        if (m_fDisplayDeviceRequests)
        {
            ::printf("GET_MAX returning 0x4600=70 dB ");
        }
        if (pSetupRequest->sSetupIndex > 0)
        {
            *ppbResponseData = (BYTE *)::CoTaskMemAlloc(pSetupRequest->sSetupIndex);
            IfFalseHrGo(NULL != *ppbResponseData, E_OUTOFMEMORY);
            ZeroMemory(*ppbResponseData, pSetupRequest->sSetupIndex);
            for (i = 0; i < pSetupRequest->sSetupIndex / sizeof(WORD); i++)
            {
                ((WORD *)*ppbResponseData)[i] = 0x0000; // set all to 00 dB
            }
            *pcbResponseData = pSetupRequest->sSetupIndex;
        }
        else
        {
            *pcbResponseData = 0;
        }
        *pbSetupStatus = USB_ACK;
        hr = S_OK;
    }
    else if (GET_RES == pSetupRequest->bRequest)
    {
        if (m_fDisplayDeviceRequests)
        {
            ::printf("GET_RES returning 0x0200=2 dB ");
        }
        if (pSetupRequest->sSetupIndex > 0)
        {
            *ppbResponseData = (BYTE *)::CoTaskMemAlloc(pSetupRequest->sSetupIndex);
            IfFalseHrGo(NULL != *ppbResponseData, E_OUTOFMEMORY);
            ZeroMemory(*ppbResponseData, pSetupRequest->sSetupIndex);
            for (i = 0; i < pSetupRequest->sSetupIndex / sizeof(WORD); i++)
            {
                ((WORD *)*ppbResponseData)[i] = 0x0200; // set all to 2 dB
            }
            *pcbResponseData = pSetupRequest->sSetupIndex;
        }
        else
        {
            *pcbResponseData = 0;
        }
        *pbSetupStatus = USB_ACK;
        hr = S_OK;
    }
    else if (GET_CUR == pSetupRequest->bRequest)
    {
        if (m_fDisplayDeviceRequests)
        {
            ::printf("GET_CUR returning 0x0000=0 dB ");
        }
        if (pSetupRequest->sSetupIndex > 0)
        {
            *ppbResponseData = (BYTE *)::CoTaskMemAlloc(pSetupRequest->sSetupIndex);
            IfFalseHrGo(NULL != *ppbResponseData, E_OUTOFMEMORY);
            ZeroMemory(*ppbResponseData, pSetupRequest->sSetupIndex); // set all to zero dB
            *pcbResponseData = pSetupRequest->sSetupIndex;
        }
        else
        {
            *pcbResponseData = 0;
        }
        *pbSetupStatus = USB_ACK;
        hr = S_OK;
    }
    else if (SET_CUR == pSetupRequest->bRequest)
    {
        if (m_fDisplayDeviceRequests)
        {
            ::printf("SET_CUR ");
        }
        *pbSetupStatus = USB_ACK;
        ::printf("cbHostData = %lu ", cbHostData);
        hr = S_OK;
    }
    else
    {
        ::printf("ProcessMixerUnitRequest received unknown request pSetupRequest->bRequest=%u\r\n", pSetupRequest->bRequest);
        hr = E_NOTIMPL;
    }

            
Exit:
    return hr;
}



VOID CAudioDevice::DisplayDeviceRequest
(
    USBSETUPREQUEST *pSetupRequest
)
/*++
Routine Description:
   Displays details of audio class-specific device requests in the
   console window

Arguments:
    pSetupRequest - pointer to the request

Return value:
    None
--*/
{
    if (TYPE_CLASS != pSetupRequest->bmRequestType.Bits.Type)
    {
        ::printf("\nReceived non-class specific device request\n");
        goto Exit;
    }

    ::printf("\nDevice request: Recipient=");
    switch (pSetupRequest->bmRequestType.Bits.Recipient)
    {
        case RCPT_DEVICE:
            ::printf("device ");
            break;

        case RCPT_INTERFACE:
            ::printf("interface %u entity ID=%u %s ", LOBYTE(pSetupRequest->sSetupIndex), HIBYTE(pSetupRequest->sSetupIndex), EntityIDToString(HIBYTE(pSetupRequest->sSetupIndex)));
            break;

        case RCPT_ENDPOINT:
            ::printf("endpoint %u entity ID=%u", LOBYTE(pSetupRequest->sSetupIndex), HIBYTE(pSetupRequest->sSetupIndex));
            break;

        case RCPT_OTHER:
            ::printf("other ");
            break;

        default:
            ::printf("unknown type %u ", pSetupRequest->bmRequestType.Bits.Recipient);
            break;
    }

    if (DIR_DEVICE_TO_HOST == pSetupRequest->bmRequestType.Bits.Direction)
    {
        ::printf(" direction=to host ");
    }
    else
    {
        ::printf(" direction=to device ");
    }

    ::printf("request type=");
    switch (pSetupRequest->bRequest)
    {
        case REQUEST_CODE_UNDEFINED:
            ::printf("REQUEST_CODE_UNDEFINED ");
            break;

        case SET_CUR:
            ::printf("SET_CUR ");
            break;

        case GET_CUR:
            ::printf("GET_CUR ");
            break;

        case SET_MIN:
            ::printf("SET_MIN ");
            break;

        case GET_MIN:
            ::printf("GET_MIN ");
            break;

        case SET_MAX:
            ::printf("SET_MAX ");
            break;

        case GET_MAX:
            ::printf("GET_MAX ");
            break;

        case SET_RES:
            ::printf("SET_RES ");
            break;

        case GET_RES:
            ::printf("GET_RES ");
            break;

        case SET_MEM:
            ::printf("SET_MEM ");
            break;

        case GET_MEM:
            ::printf("GET_MEM ");
            break;

        case GET_STAT:
            ::printf("GET_STAT ");
            break;

        default:
            ::printf("unknown type %u ", pSetupRequest->bRequest);
            break;
    }

    ::printf("wValue=%04.4X wIndex=%04.4X wLength==%04.4X ", pSetupRequest->sSetupValue, pSetupRequest->sSetupIndex, pSetupRequest->sSetupLength);

    ::printf("\n");

Exit:
    return;
}



LPCSTR CAudioDevice::EntityIDToString(BYTE bEntityID)
{
    LPCSTR pwszID = "unknown";

    switch (bEntityID)
    {
        case 0x01:
            pwszID = "USB streaming input terminal";
            break;

        case 0x02:
            pwszID = "up / down mix processing unit 2";
            break;

        case 0x03:
            pwszID = "up / down mix processing unit 3";
            break;

        case 0x04:
            pwszID = "feature unit 4";
            break;

        case 0x05:
            pwszID = "line connector input terminal";
            break;

        case 0x06:
            pwszID = "feature unit 6";
            break;

        case 0x07:
            pwszID = "mixer unit 7";
            break;

        case 0x08:
            pwszID = "feature unit 8";
            break;

        case 0x09:
            pwszID = "desktop speaker output terminal";
            break;

        case 0x0A:
            pwszID = "mixer unit 10";
            break;

        case 0x0B:
            pwszID = "feature unit 11";
            break;

        case 0x0C:
            pwszID = "low frequency effects speaker output terminal";
            break;

    }
    return pwszID;
}

STDMETHODIMP CAudioDevice::OnDeviceRequestComplete(ULONG RequestHandle,
                                       BYTE *pbFinalRequestStatus)

/*++
Routine Description:
   Implements the OnDeviceRequestComplete for the interface ISoftUSBEndpointEvent

Arguments:
    RequestHandle - Handle to the request which has just completed
    pbFinalStatus - Pointer to the final status that is to be returned.
    
Return value:
    E_NOTIMPL as this event is not handled by the device
--*/
{
    HRESULT hr = E_NOTIMPL;

    UNREFERENCED_PARAMETER(RequestHandle);
    UNREFERENCED_PARAMETER(pbFinalRequestStatus);

    return hr;
}

