/*++

Copyright (c) 1997-1998 Microsoft Corporation

Module Name:

DISPAUD.C

Abstract:

This source file contains routines which update the edit control
to display information about USB Audio descriptors.

Environment:

user mode

Revision History:

03-07-1998 : created

--*/

//*****************************************************************************
// I N C L U D E S
//*****************************************************************************

#include <windows.h>
#include <basetyps.h>
#include <tchar.h>
#include "usbview.h"


#if _MSC_VER >= 1200
#pragma warning(push)
#endif
#pragma warning(disable:4200) // named type definition in parentheses
#pragma warning(disable:4213) // named type definition in parentheses
#pragma warning(disable:4701) // named type definition in parentheses


//*****************************************************************************
// D E F I N E S
//*****************************************************************************

//*****************************************************************************
// T Y P E D E F S
//*****************************************************************************

typedef struct _TERMTYPE
{
    USHORT TermTypeCode;
    PCTSTR  TermTypeName;
} TERMTYPE, *PTERMTYPE;

typedef struct _FORMATTYPE
{
    USHORT FormatTypeCode;
    PCTSTR  FormatTypeName;
} FORMATTYPE, *PFORMATTYPE;

//*****************************************************************************
// G L O B A L S    P R I V A T E    T O    T H I S    F I L E
//*****************************************************************************

//
// USB Device Class Definition for Terminal Types  0.9 Draft Revision
//
TERMTYPE TermTypes[] =
{
    //
    // 2.1 USB Terminal Types
    //
    {0x0100, _T("USB Undefined")},
    {0x0101, _T("USB streaming")},
    {0x01FF, _T("USB vendor specific")},
    //
    // 2.2 Input Terminal Types
    //
    {0x0200, _T("Input Undefined")},
    {0x0201, _T("Microphone")},
    {0x0202, _T("Desktop microphone")},
    {0x0203, _T("Personal microphone")},
    {0x0204, _T("Omni-directional microphone")},
    {0x0205, _T("Microphone array")},
    {0x0206, _T("Processing microphone array")},
    //
    // 2.3 Output Terminal Types
    //
    {0x0300, _T("Output Undefined")},
    {0x0301, _T("Speaker")},
    {0x0302, _T("Headphones")},
    {0x0303, _T("Head Mounted Display Audio")},
    {0x0304, _T("Desktop speaker")},
    {0x0305, _T("Room speaker")},
    {0x0306, _T("Communication speaker")},
    {0x0307, _T("Low frequency effects speaker")},
    //
    // 2.4 Bi-directional Terminal Types
    //
    {0x0400, _T("Bi-directional Undefined")},
    {0x0401, _T("Handset")},
    {0x0402, _T("Headset")},
    {0x0403, _T("Speakerphone, no echo reduction")},
    {0x0404, _T("Echo-suppressing speakerphone")},
    {0x0405, _T("Echo-canceling speakerphone")},
    //
    // 2.5 Telephony Terminal Types
    //
    {0x0500, _T("Telephony Undefined")},
    {0x0501, _T("Phone line")},
    {0x0502, _T("Telephone")},
    {0x0503, _T("Down Line Phone")},
    //
    // 2.6 External Terminal Types
    //
    {0x0600, _T("External Undefined")},
    {0x0601, _T("Analog connector")},
    {0x0602, _T("Digital audio interface")},
    {0x0603, _T("Line connector")},
    {0x0604, _T("Legacy audio connector")},
    {0x0605, _T("S/PDIF interface")},
    {0x0606, _T("1394 DA stream")},
    {0x0607, _T("1394 DV stream soundtrack")},
    //
    // Embedded Function Terminal Types
    //
    {0x0700, _T("Embedded Undefined")},
    {0x0701, _T("Level Calibration Noise Source")},
    {0x0702, _T("Equalization Noise")},
    {0x0703, _T("CD player")},
    {0x0704, _T("DAT")},
    {0x0705, _T("DCC")},
    {0x0706, _T("MiniDisk")},
    {0x0707, _T("Analog Tape")},
    {0x0708, _T("Phonograph")},
    {0x0709, _T("VCR Audio")},
    {0x070A, _T("Video Disc Audio")},
    {0x070B, _T("DVD Audio")},
    {0x070C, _T("TV Tuner Audio")},
    {0x070D, _T("Satellite Receiver Audio")},
    {0x070E, _T("Cable Tuner Audio")},
    {0x070F, _T("DSS Audio")},
    {0x0710, _T("Radio Receiver")},
    {0x0711, _T("Radio Transmitter")},
    {0x0712, _T("Multi-track Recorder")},
    {0x0713, _T("Synthesizer")},
    //
    // Terminate List
    //
    {0xFFFF, NULL}
};

//
// USB Device Class Definition for Audio Data Formats  0.9 Draft Revision
//
FORMATTYPE FormatTypes[] =
{
    //
    // A.1.1 Audio Data Format Type I Codes
    //
    {0x0000, _T("TYPE_I_UNDEFINED")},
    {0x0001, _T("PCM")},
    {0x0002, _T("PCM8")},
    {0x0003, _T("IEEE_FLOAT")},
    {0x0004, _T("ALAW")},
    {0x0005, _T("MULAW")},
    //
    // A.1.2 Audio Data Format Type II Codes
    //
    {0x1000, _T("TYPE_II_UNDEFINED")},
    {0x1001, _T("MPEG")},
    {0x1002, _T("AC-3")},
    //
    // A.1.3 Audio Data Format Type III Codes
    //
    {0x2000, _T("TYPE_III_UNDEFINED")},
    {0x2001, _T("IEC1937_AC-3")},
    {0x2002, _T("IEC1937_MPEG-1_Layer1")},
    {0x2003, _T("IEC1937_MPEG-1_Layer2/3 or IEC1937_MPEG-2_NOEXT")},
    {0x2004, _T("IEC1937_MPEG-2_EXT")},
    {0x2005, _T("IEC1937_MPEG-2_Layer1_LS")},
    {0x2006, _T("IEC1937_MPEG-2_Layer2/3_LS")},
    //
    // Terminate List
    //
    {0xFFFF, NULL}
};

//*****************************************************************************
// L O C A L    F U N C T I O N    P R O T O T Y P E S
//*****************************************************************************

BOOL
DisplayACHeader (
    PUSB_AUDIO_AC_INTERFACE_HEADER_DESCRIPTOR HeaderDesc
);

BOOL
DisplayACInputTerminal (
    PUSB_AUDIO_INPUT_TERMINAL_DESCRIPTOR ITDesc
);

BOOL
DisplayACOutputTerminal (
    PUSB_AUDIO_OUTPUT_TERMINAL_DESCRIPTOR OTDesc
);

BOOL
DisplayACMixerUnit (
    PUSB_AUDIO_MIXER_UNIT_DESCRIPTOR MixerDesc
);

BOOL
DisplayACSelectorUnit (
    PUSB_AUDIO_SELECTOR_UNIT_DESCRIPTOR SelectorDesc
);

BOOL
DisplayACFeatureUnit (
    PUSB_AUDIO_FEATURE_UNIT_DESCRIPTOR FeatureDesc
);

BOOL
DisplayACProcessingUnit (
    PUSB_AUDIO_PROCESSING_UNIT_DESCRIPTOR ProcessingDesc
);

BOOL
DisplayACExtensionUnit (
    PUSB_AUDIO_EXTENSION_UNIT_DESCRIPTOR ExtensionDesc
);

BOOL
DisplayASGeneral (
    PUSB_AUDIO_GENERAL_DESCRIPTOR GeneralDesc
);

BOOL
DisplayCSEndpoint (
    PUSB_AUDIO_ENDPOINT_DESCRIPTOR EndpointDesc
);

BOOL
DisplayASFormatType (
    PUSB_AUDIO_COMMON_FORMAT_DESCRIPTOR FormatDesc
);

BOOL
DisplayASFormatSpecific (
    PUSB_AUDIO_COMMON_DESCRIPTOR CommonDesc
);

VOID
DisplayBytes (
    PUCHAR Data,
    USHORT Len
);

PCTSTR
TermTypeCodeToName (
    USHORT TermTypeCode
);

PCTSTR
FormatTypeCodeToName (
    USHORT FormatTypeCode
);


//*****************************************************************************
// L O C A L    F U N C T I O N S
//*****************************************************************************



//*****************************************************************************
//
// DisplayAudioDescriptor()
//
// CommonDesc - An Audio Class Descriptor
//
// bInterfaceSubClass - The SubClass of the Interface containing the descriptor
//
//*****************************************************************************

BOOL
DisplayAudioDescriptor (
    PUSB_AUDIO_COMMON_DESCRIPTOR CommonDesc,
    UCHAR                        bInterfaceSubClass
)
{
    switch (CommonDesc->bDescriptorType)
    {
        case USB_AUDIO_CS_INTERFACE:
            switch (bInterfaceSubClass)
            {
                case USB_AUDIO_SUBCLASS_AUDIOCONTROL:
                    switch (CommonDesc->bDescriptorSubtype)
                    {
                        case USB_AUDIO_AC_HEADER:
                            return DisplayACHeader((PUSB_AUDIO_AC_INTERFACE_HEADER_DESCRIPTOR)CommonDesc);

                        case USB_AUDIO_AC_INPUT_TERMINAL:
                            return DisplayACInputTerminal((PUSB_AUDIO_INPUT_TERMINAL_DESCRIPTOR)CommonDesc);

                        case USB_AUDIO_AC_OUTPUT_TERMINAL:
                            return DisplayACOutputTerminal((PUSB_AUDIO_OUTPUT_TERMINAL_DESCRIPTOR)CommonDesc);

                        case USB_AUDIO_AC_MIXER_UNIT:
                            return DisplayACMixerUnit((PUSB_AUDIO_MIXER_UNIT_DESCRIPTOR)CommonDesc);

                        case USB_AUDIO_AC_SELECTOR_UNIT:
                            return DisplayACSelectorUnit((PUSB_AUDIO_SELECTOR_UNIT_DESCRIPTOR)CommonDesc);

                        case USB_AUDIO_AC_FEATURE_UNIT:
                            return DisplayACFeatureUnit((PUSB_AUDIO_FEATURE_UNIT_DESCRIPTOR)CommonDesc);

                        case USB_AUDIO_AC_PROCESSING_UNIT:
                            return DisplayACProcessingUnit((PUSB_AUDIO_PROCESSING_UNIT_DESCRIPTOR)CommonDesc);

                        case USB_AUDIO_AC_EXTENSION_UNIT:
                            return DisplayACExtensionUnit((PUSB_AUDIO_EXTENSION_UNIT_DESCRIPTOR)CommonDesc);

                        default:
                            break;
                    }
                    break;

                case USB_AUDIO_SUBCLASS_AUDIOSTREAMING:
                    switch (CommonDesc->bDescriptorSubtype)
                    {
                        case USB_AUDIO_AS_GENERAL:
                            return DisplayASGeneral((PUSB_AUDIO_GENERAL_DESCRIPTOR)CommonDesc);

                        case USB_AUDIO_AS_FORMAT_TYPE:
                            return DisplayASFormatType((PUSB_AUDIO_COMMON_FORMAT_DESCRIPTOR)CommonDesc);
                            break;

                        case USB_AUDIO_AS_FORMAT_SPECIFIC:
                            return DisplayASFormatSpecific(CommonDesc);

                        default:
                            break;
                    }
                    break;

                default:
                    break;
            }
            break;

        case USB_AUDIO_CS_ENDPOINT:
            return DisplayCSEndpoint((PUSB_AUDIO_ENDPOINT_DESCRIPTOR)CommonDesc);

        default:
            break;
    }

    return FALSE;
}


//*****************************************************************************
//
// DisplayACHeader()
//
//*****************************************************************************

BOOL
DisplayACHeader (
    PUSB_AUDIO_AC_INTERFACE_HEADER_DESCRIPTOR HeaderDesc
)
{
    UINT i;

    if (HeaderDesc->bLength < sizeof(USB_AUDIO_AC_INTERFACE_HEADER_DESCRIPTOR))
    {
        OOPS();
        return FALSE;
    }

    AppendTextBuffer(_T("\r\nAudio Control Interface Header Descriptor:\r\n"));

    AppendTextBuffer(_T("bLength:              0x%02X\r\n"),
                     HeaderDesc->bLength);

    AppendTextBuffer(_T("bDescriptorType:      0x%02X\r\n"),
                     HeaderDesc->bDescriptorType);

    AppendTextBuffer(_T("bDescriptorSubtype:   0x%02X\r\n"),
                     HeaderDesc->bDescriptorSubtype);

    AppendTextBuffer(_T("bcdADC:             0x%04X\r\n"),
                     HeaderDesc->bcdADC);

    AppendTextBuffer(_T("wTotalLength:       0x%04X\r\n"),
                     HeaderDesc->wTotalLength);

    AppendTextBuffer(_T("bInCollection:        0x%02X\r\n"),
                     HeaderDesc->bInCollection);

    for (i=0; i<HeaderDesc->bInCollection; i++)
    {
        AppendTextBuffer(_T("baInterfaceNr[%d]:     0x%02X\r\n"),
                         i+1,
                         HeaderDesc->baInterfaceNr[i]);
    }

    return TRUE;
}


//*****************************************************************************
//
// DisplayACInputTerminal()
//
//*****************************************************************************

BOOL
DisplayACInputTerminal (
    PUSB_AUDIO_INPUT_TERMINAL_DESCRIPTOR ITDesc
)
{
    PCTSTR pStr;

    if (ITDesc->bLength != sizeof(USB_AUDIO_INPUT_TERMINAL_DESCRIPTOR))
    {
        OOPS();
        return FALSE;
    }

    AppendTextBuffer(_T("\r\nAudio Control Input Terminal Descriptor:\r\n"));

    AppendTextBuffer(_T("bLength:              0x%02X\r\n"),
                     ITDesc->bLength);

    AppendTextBuffer(_T("bDescriptorType:      0x%02X\r\n"),
                     ITDesc->bDescriptorType);

    AppendTextBuffer(_T("bDescriptorSubtype:   0x%02X\r\n"),
                     ITDesc->bDescriptorSubtype);

    AppendTextBuffer(_T("bTerminalID:          0x%02X\r\n"),
                     ITDesc->bTerminalID);

    AppendTextBuffer(_T("wTerminalType:      0x%04X"),
                     ITDesc->wTerminalType);

    pStr = TermTypeCodeToName(ITDesc->wTerminalType);

    if (pStr)
    {
        AppendTextBuffer(_T(" (%s)\r\n"),
                         pStr);
    }
    else
    {
        AppendTextBuffer(_T("\r\n"));
    }

    AppendTextBuffer(_T("bAssocTerminal:       0x%02X\r\n"),
                     ITDesc->bAssocTerminal);

    AppendTextBuffer(_T("bNrChannels:          0x%02X\r\n"),
                     ITDesc->bNrChannels);

    AppendTextBuffer(_T("wChannelConfig:     0x%04X\r\n"),
                     ITDesc->wChannelConfig);

    AppendTextBuffer(_T("iChannelNames:        0x%02X\r\n"),
                     ITDesc->iChannelNames);

    AppendTextBuffer(_T("iTerminal:            0x%02X\r\n"),
                     ITDesc->iTerminal);


    return TRUE;
}


//*****************************************************************************
//
// DisplayACOutputTerminal()
//
//*****************************************************************************

BOOL
DisplayACOutputTerminal (
    PUSB_AUDIO_OUTPUT_TERMINAL_DESCRIPTOR OTDesc
)
{
    PCTSTR pStr;

    if (OTDesc->bLength != sizeof(USB_AUDIO_OUTPUT_TERMINAL_DESCRIPTOR))
    {
        OOPS();
        return FALSE;
    }

    AppendTextBuffer(_T("\r\nAudio Control Output Terminal Descriptor:\r\n"));

    AppendTextBuffer(_T("bLength:              0x%02X\r\n"),
                     OTDesc->bLength);

    AppendTextBuffer(_T("bDescriptorType:      0x%02X\r\n"),
                     OTDesc->bDescriptorType);

    AppendTextBuffer(_T("bDescriptorSubtype:   0x%02X\r\n"),
                     OTDesc->bDescriptorSubtype);

    AppendTextBuffer(_T("bTerminalID:          0x%02X\r\n"),
                     OTDesc->bTerminalID);

    AppendTextBuffer(_T("wTerminalType:      0x%04X"),
                     OTDesc->wTerminalType);

    pStr = TermTypeCodeToName(OTDesc->wTerminalType);

    if (pStr)
    {
        AppendTextBuffer(_T(" (%s)\r\n"),
                         pStr);
    }
    else
    {
        AppendTextBuffer(_T("\r\n"));
    }

    AppendTextBuffer(_T("bAssocTerminal:       0x%02X\r\n"),
                     OTDesc->bAssocTerminal);

    AppendTextBuffer(_T("bSoruceID:            0x%02X\r\n"),
                     OTDesc->bSoruceID);

    AppendTextBuffer(_T("iTerminal:            0x%02X\r\n"),
                     OTDesc->iTerminal);


    return TRUE;
}


//*****************************************************************************
//
// DisplayACMixerUnit()
//
//*****************************************************************************

BOOL
DisplayACMixerUnit (
    PUSB_AUDIO_MIXER_UNIT_DESCRIPTOR MixerDesc
)
{
    UCHAR  i;
    PUCHAR data;

    if (MixerDesc->bLength < 10)
    {
        OOPS();
        return FALSE;
    }

    AppendTextBuffer(_T("\r\nAudio Control Mixer Unit Descriptor:\r\n"));

    AppendTextBuffer(_T("bLength:              0x%02X\r\n"),
                     MixerDesc->bLength);

    AppendTextBuffer(_T("bDescriptorType:      0x%02X\r\n"),
                     MixerDesc->bDescriptorType);

    AppendTextBuffer(_T("bDescriptorSubtype:   0x%02X\r\n"),
                     MixerDesc->bDescriptorSubtype);

    AppendTextBuffer(_T("bUnitID:              0x%02X\r\n"),
                     MixerDesc->bUnitID);

    AppendTextBuffer(_T("bNrInPins:            0x%02X\r\n"),
                     MixerDesc->bNrInPins);

    for (i=0; i<MixerDesc->bNrInPins; i++)
    {
        AppendTextBuffer(_T("baSourceID[%d]:        0x%02X\r\n"),
                        i+1,
                        MixerDesc->baSourceID[i]);
    }

    data = &MixerDesc->baSourceID[MixerDesc->bNrInPins];

    AppendTextBuffer(_T("bNrChannels:          0x%02X\r\n"),
                     *data++);

    AppendTextBuffer(_T("wChannelConfig:     0x%04X\r\n"),
                     *((PUSHORT)data)++);

    AppendTextBuffer(_T("iChannelNames:        0x%02X\r\n"),
                     *data++);

    AppendTextBuffer(_T("bmControls:\r\n"));

    i = MixerDesc->bLength - 10 - MixerDesc->bNrInPins;

    DisplayBytes(data, i);

    data += i;

    AppendTextBuffer(_T("iMixer:               0x%02X\r\n"),
                     *data);

    return TRUE;
}


//*****************************************************************************
//
// DisplayACSelectorUnit()
//
//*****************************************************************************

BOOL
DisplayACSelectorUnit (
    PUSB_AUDIO_SELECTOR_UNIT_DESCRIPTOR SelectorDesc
)
{
    UCHAR  i;
    PUCHAR data;

    if (SelectorDesc->bLength < 6)
    {
        OOPS();
        return FALSE;
    }

    AppendTextBuffer(_T("\r\nAudio Control Selector Unit Descriptor:\r\n"));

    AppendTextBuffer(_T("bLength:              0x%02X\r\n"),
                     SelectorDesc->bLength);

    AppendTextBuffer(_T("bDescriptorType:      0x%02X\r\n"),
                     SelectorDesc->bDescriptorType);

    AppendTextBuffer(_T("bDescriptorSubtype:   0x%02X\r\n"),
                     SelectorDesc->bDescriptorSubtype);

    AppendTextBuffer(_T("bUnitID:              0x%02X\r\n"),
                     SelectorDesc->bUnitID);

    AppendTextBuffer(_T("bNrInPins:            0x%02X\r\n"),
                     SelectorDesc->bNrInPins);

    for (i=0; i<SelectorDesc->bNrInPins; i++)
    {
        AppendTextBuffer(_T("baSourceID[%d]:        0x%02X\r\n"),
                        i+1,
                        SelectorDesc->baSourceID[i]);
    }

    data = &SelectorDesc->baSourceID[SelectorDesc->bNrInPins];

    AppendTextBuffer(_T("iSelector:            0x%02X\r\n"),
                     *data);

    return TRUE;
}


//*****************************************************************************
//
// DisplayACFeatureUnit()
//
//*****************************************************************************

BOOL
DisplayACFeatureUnit (
    PUSB_AUDIO_FEATURE_UNIT_DESCRIPTOR FeatureDesc
)
{
    UCHAR  i, n, ch;
    PUCHAR data;

    if (FeatureDesc->bLength < 7)
    {
        OOPS();
        return FALSE;
    }

    n = FeatureDesc->bControlSize;

    ch = ((FeatureDesc->bLength - 7) / n) - 1;

    if (FeatureDesc->bLength != (7 + (ch + 1) * n))
    {
        OOPS();
        return FALSE;
    }

    AppendTextBuffer(_T("\r\nAudio Control Feature Unit Descriptor:\r\n"));

    AppendTextBuffer(_T("bLength:              0x%02X\r\n"),
                     FeatureDesc->bLength);

    AppendTextBuffer(_T("bDescriptorType:      0x%02X\r\n"),
                     FeatureDesc->bDescriptorType);

    AppendTextBuffer(_T("bDescriptorSubtype:   0x%02X\r\n"),
                     FeatureDesc->bDescriptorSubtype);

    AppendTextBuffer(_T("bUnitID:              0x%02X\r\n"),
                     FeatureDesc->bUnitID);

    AppendTextBuffer(_T("bSourceID:            0x%02X\r\n"),
                     FeatureDesc->bSourceID);

    AppendTextBuffer(_T("bControlSize:         0x%02X\r\n"),
                     FeatureDesc->bControlSize);

    data = &FeatureDesc->bmaControls[0];

    for (i=0; i<=ch; i++)
    {
        AppendTextBuffer(_T("bmaControls[%d]:\r\n"),
                         i);

        DisplayBytes(data, n);

        data += n;
    }

    AppendTextBuffer(_T("iFeature:             0x%02X\r\n"),
                     *data);

    return TRUE;
}


//*****************************************************************************
//
// DisplayACProcessingUnit()
//
//*****************************************************************************

BOOL
DisplayACProcessingUnit (
    PUSB_AUDIO_PROCESSING_UNIT_DESCRIPTOR ProcessingDesc
)
{
    UCHAR  i;
    PUCHAR data;

    if (ProcessingDesc->bLength < sizeof(USB_AUDIO_PROCESSING_UNIT_DESCRIPTOR))
    {
        OOPS();
        return FALSE;
    }

    AppendTextBuffer(_T("\r\nAudio Control Processing Unit Descriptor:\r\n"));

    AppendTextBuffer(_T("bLength:              0x%02X\r\n"),
                     ProcessingDesc->bLength);

    AppendTextBuffer(_T("bDescriptorType:      0x%02X\r\n"),
                     ProcessingDesc->bDescriptorType);

    AppendTextBuffer(_T("bDescriptorSubtype:   0x%02X\r\n"),
                     ProcessingDesc->bDescriptorSubtype);

    AppendTextBuffer(_T("bUnitID:              0x%02X\r\n"),
                     ProcessingDesc->bUnitID);

    AppendTextBuffer(_T("wProcessType:       0x%04X"),
                     ProcessingDesc->wProcessType);

    switch (ProcessingDesc->wProcessType)
    {
        case USB_AUDIO_PROCESS_UNDEFINED:
            AppendTextBuffer(_T("(Undefined Process)\r\n"));
            break;

        case USB_AUDIO_PROCESS_UPDOWNMIX:
            AppendTextBuffer(_T("(Up / Down Mix Process)\r\n"));
            break;

        case USB_AUDIO_PROCESS_DOLBYPROLOGIC:
            AppendTextBuffer(_T("(Dolby Prologic Process)\r\n"));
            break;

        case USB_AUDIO_PROCESS_3DSTEREOEXTENDER:
            AppendTextBuffer(_T("(3D-Stereo Extender Process)\r\n"));
            break;

        case USB_AUDIO_PROCESS_REVERBERATION:
            AppendTextBuffer(_T("(Reverberation Process)\r\n"));
            break;

        case USB_AUDIO_PROCESS_CHORUS:
            AppendTextBuffer(_T("(Chorus Process)\r\n"));
            break;

        case USB_AUDIO_PROCESS_DYNRANGECOMP:
            AppendTextBuffer(_T("(Dynamic Range Compressor Process)\r\n"));
            break;

        default:
            AppendTextBuffer(_T("\r\n"));
            break;
    }

    AppendTextBuffer(_T("bNrInPins:            0x%02X\r\n"),
                     ProcessingDesc->bNrInPins);

    for (i=0; i<ProcessingDesc->bNrInPins; i++)
    {
        AppendTextBuffer(_T("baSourceID[%d]:        0x%02X\r\n"),
                        i+1,
                        ProcessingDesc->baSourceID[i]);
    }

    data = &ProcessingDesc->baSourceID[ProcessingDesc->bNrInPins];

    AppendTextBuffer(_T("bNrChannels:          0x%02X\r\n"),
                     *data++);

    AppendTextBuffer(_T("wChannelConfig:     0x%04X\r\n"),
                     *((PUSHORT)data)++);

    AppendTextBuffer(_T("iChannelNames:        0x%02X\r\n"),
                     *data++);

    i = *data++;

    AppendTextBuffer(_T("bControlSize:         0x%02X\r\n"),
                     i);

    AppendTextBuffer(_T("bmControls:\r\n"));

    DisplayBytes(data, i);

    data += i;

    AppendTextBuffer(_T("iProcessing:          0x%02X\r\n"),
                     *data++);


    i = ProcessingDesc->bLength - 13 - ProcessingDesc->bNrInPins - i;

    if (i)
    {
        AppendTextBuffer(_T("Process Specific:\r\n"));

        DisplayBytes(data, i);
    }

    return TRUE;
}


//*****************************************************************************
//
// DisplayACExtensionUnit()
//
//*****************************************************************************

BOOL
DisplayACExtensionUnit (
    PUSB_AUDIO_EXTENSION_UNIT_DESCRIPTOR ExtensionDesc
)
{
    UCHAR  i;
    PUCHAR data;

    if (ExtensionDesc->bLength < 13)
    {
        OOPS();
        return FALSE;
    }

    AppendTextBuffer(_T("\r\nAudio Control Extension Unit Descriptor:\r\n"));

    AppendTextBuffer(_T("bLength:              0x%02X\r\n"),
                     ExtensionDesc->bLength);

    AppendTextBuffer(_T("bDescriptorType:      0x%02X\r\n"),
                     ExtensionDesc->bDescriptorType);

    AppendTextBuffer(_T("bDescriptorSubtype:   0x%02X\r\n"),
                     ExtensionDesc->bDescriptorSubtype);

    AppendTextBuffer(_T("bUnitID:              0x%02X\r\n"),
                     ExtensionDesc->bUnitID);

    AppendTextBuffer(_T("wExtensionCode:     0x%04X\r\n"),
                     ExtensionDesc->wExtensionCode);


    AppendTextBuffer(_T("bNrInPins:            0x%02X\r\n"),
                     ExtensionDesc->bNrInPins);

    for (i=0; i<ExtensionDesc->bNrInPins; i++)
    {
        AppendTextBuffer(_T("baSourceID[%d]:        0x%02X\r\n"),
                        i+1,
                        ExtensionDesc->baSourceID[i]);
    }

    data = &ExtensionDesc->baSourceID[ExtensionDesc->bNrInPins];

    AppendTextBuffer(_T("bNrChannels:          0x%02X\r\n"),
                     *data++);

    AppendTextBuffer(_T("wChannelConfig:     0x%04X\r\n"),
                     *((PUSHORT)data)++);

    AppendTextBuffer(_T("iChannelNames:        0x%02X\r\n"),
                     *data++);

    i = *data++;

    AppendTextBuffer(_T("bControlSize:         0x%02X\r\n"),
                     i);

    AppendTextBuffer(_T("bmControls:\r\n"));

    DisplayBytes(data, i);

    data += i;

    AppendTextBuffer(_T("iExtension:           0x%02X\r\n"),
                     *data);
    return TRUE;
}


//*****************************************************************************
//
// DisplayASGeneral()
//
//*****************************************************************************

BOOL
DisplayASGeneral (
    PUSB_AUDIO_GENERAL_DESCRIPTOR GeneralDesc
)
{
    PCTSTR pStr;

    if (GeneralDesc->bLength != sizeof(USB_AUDIO_GENERAL_DESCRIPTOR))
    {
        OOPS();
        return FALSE;
    }

    AppendTextBuffer(_T("\r\nAudio Streaming Class Specific Interface Descriptor:\r\n"));

    AppendTextBuffer(_T("bLength:              0x%02X\r\n"),
                     GeneralDesc->bLength);

    AppendTextBuffer(_T("bDescriptorType:      0x%02X\r\n"),
                     GeneralDesc->bDescriptorType);

    AppendTextBuffer(_T("bDescriptorSubtype:   0x%02X\r\n"),
                     GeneralDesc->bDescriptorSubtype);

    AppendTextBuffer(_T("bTerminalLink:        0x%02X\r\n"),
                     GeneralDesc->bTerminalLink);

    AppendTextBuffer(_T("bDelay:               0x%02X\r\n"),
                     GeneralDesc->bDelay);

    AppendTextBuffer(_T("wFormatTag:         0x%04X"),
                     GeneralDesc->wFormatTag);

    pStr = FormatTypeCodeToName(GeneralDesc->wFormatTag);

    if (pStr)
    {
        AppendTextBuffer(_T(" (%s)\r\n"),
                         pStr);
    }
    else
    {
        AppendTextBuffer(_T("\r\n"));
    }

    return TRUE;
}


//*****************************************************************************
//
// DisplayCSEndpoint()
//
//*****************************************************************************

BOOL
DisplayCSEndpoint (
    PUSB_AUDIO_ENDPOINT_DESCRIPTOR EndpointDesc
)
{
    if (EndpointDesc->bLength != sizeof(USB_AUDIO_ENDPOINT_DESCRIPTOR))
    {
        OOPS();
        return FALSE;
    }

    AppendTextBuffer(_T("\r\nAudio Streaming Class Specific Audio Data Endpoint Descriptor:\r\n"));

    AppendTextBuffer(_T("bLength:              0x%02X\r\n"),
                     EndpointDesc->bLength);

    AppendTextBuffer(_T("bDescriptorType:      0x%02X\r\n"),
                     EndpointDesc->bDescriptorType);

    AppendTextBuffer(_T("bDescriptorSubtype:   0x%02X\r\n"),
                     EndpointDesc->bDescriptorSubtype);

    AppendTextBuffer(_T("bmAttributes:         0x%02X\r\n"),
                     EndpointDesc->bmAttributes);

    AppendTextBuffer(_T("bLockDelayUnits:      0x%02X\r\n"),
                     EndpointDesc->bLockDelayUnits);

    AppendTextBuffer(_T("wLockDelay:         0x%04X\r\n"),
                     EndpointDesc->wLockDelay);

    return TRUE;
}


//*****************************************************************************
//
// DisplayASFormatType()
//
//*****************************************************************************

BOOL
DisplayASFormatType (
    PUSB_AUDIO_COMMON_FORMAT_DESCRIPTOR FormatDesc
)
{
    UCHAR  i, n;
    ULONG  freq;
    PUCHAR data;

    if (FormatDesc->bLength < sizeof(USB_AUDIO_COMMON_FORMAT_DESCRIPTOR))
    {
        OOPS();
        return FALSE;
    }

    AppendTextBuffer(_T("\r\nAudio Streaming Format Type Descriptor:\r\n"));

    AppendTextBuffer(_T("bLength:              0x%02X\r\n"),
                     FormatDesc->bLength);

    AppendTextBuffer(_T("bDescriptorType:      0x%02X\r\n"),
                     FormatDesc->bDescriptorType);

    AppendTextBuffer(_T("bDescriptorSubtype:   0x%02X\r\n"),
                     FormatDesc->bDescriptorSubtype);

    AppendTextBuffer(_T("bFormatType:          0x%02X\r\n"),
                     FormatDesc->bFormatType);


    if (FormatDesc->bFormatType == 0x01 ||
        FormatDesc->bFormatType == 0x03)
    {
        PUSB_AUDIO_TYPE_I_OR_III_FORMAT_DESCRIPTOR FormatI_IIIDesc;

        FormatI_IIIDesc = (PUSB_AUDIO_TYPE_I_OR_III_FORMAT_DESCRIPTOR)FormatDesc;

        AppendTextBuffer(_T("bNrChannels:          0x%02X\r\n"),
                         FormatI_IIIDesc->bNrChannels);

        AppendTextBuffer(_T("bSubframeSize:        0x%02X\r\n"),
                         FormatI_IIIDesc->bSubframeSize);

        AppendTextBuffer(_T("bBitResolution:       0x%02X\r\n"),
                         FormatI_IIIDesc->bBitResolution);

        AppendTextBuffer(_T("bSamFreqType:         0x%02X\r\n"),
                         FormatI_IIIDesc->bSamFreqType);

        data = (PUCHAR)(FormatI_IIIDesc + 1);

        n = FormatI_IIIDesc->bSamFreqType;

    }
    else if (FormatDesc->bFormatType == 0x02)
    {
        PUSB_AUDIO_TYPE_II_FORMAT_DESCRIPTOR FormatIIDesc;

        FormatIIDesc = (PUSB_AUDIO_TYPE_II_FORMAT_DESCRIPTOR)FormatDesc;

        AppendTextBuffer(_T("wMaxBitRate:        0x%04X\r\n"),
                         FormatIIDesc->wMaxBitRate);

        AppendTextBuffer(_T("wSamplesPerFrame:   0x%04X\r\n"),
                         FormatIIDesc->wSamplesPerFrame);

        AppendTextBuffer(_T("bSamFreqType:         0x%02X\r\n"),
                         FormatIIDesc->bSamFreqType);

        data = (PUCHAR)(FormatIIDesc + 1);

        n = FormatIIDesc->bSamFreqType;

    }
    else
    {
        data = NULL;
    }

    if (data != NULL)
    {
        if (n == 0)
        {
            freq = (data[0]) + (data[1] << 8) + (data[2] << 16);
            data += 3;

            AppendTextBuffer(_T("tLowerSamFreq:    0x%06X (%d Hz)\r\n"),
                             freq,
                             freq);

            freq = (data[0]) + (data[1] << 8) + (data[2] << 16);
            data += 3;

            AppendTextBuffer(_T("tUpperSamFreq:    0x%06X (%d Hz)\r\n"),
                             freq,
                             freq);
        }
        else
        {
            for (i=0; i<n; i++)
            {
                freq = (data[0]) + (data[1] << 8) + (data[2] << 16);
                data += 3;

                AppendTextBuffer(_T("tSamFreq[%d]:      0x%06X (%d Hz)\r\n"),
                                 i+1,
                                 freq,
                                 freq);
            }
        }
    }

    return TRUE;
}


//*****************************************************************************
//
// DisplayASFormatSpecific()
//
//*****************************************************************************

BOOL
DisplayASFormatSpecific (
    PUSB_AUDIO_COMMON_DESCRIPTOR CommonDesc
)
{
    AppendTextBuffer(_T("\r\nAudio Streaming Format Specific Descriptor:\r\n"));

    AppendTextBuffer(_T("bLength:              0x%02X\r\n"),
                     CommonDesc->bLength);

    AppendTextBuffer(_T("bDescriptorType:      0x%02X\r\n"),
                     CommonDesc->bDescriptorType);

    AppendTextBuffer(_T("bDescriptorSubtype:   0x%02X\r\n"),
                     CommonDesc->bDescriptorSubtype);

    DisplayBytes((PUCHAR)(CommonDesc + 1),
                 CommonDesc->bLength);

    return TRUE;
}

//*****************************************************************************
//
// DisplayBytes()
//
//*****************************************************************************

VOID
DisplayBytes (
    PUCHAR Data,
    USHORT Len
)
{
    USHORT i;

    for (i = 0; i < Len; i++)
    {
        AppendTextBuffer(_T("%02X "),
                         Data[i]);

        if (i % 16 == 15)
        {
            AppendTextBuffer(_T("\r\n"));
        }
    }

    if (i % 16 != 0)
    {
        AppendTextBuffer(_T("\r\n"));
    }
}


//*****************************************************************************
//
// TermTypeCodeToName()
//
//*****************************************************************************

PCTSTR
TermTypeCodeToName (
    USHORT TermTypeCode
)
{
    PTERMTYPE termType;

    for (termType=TermTypes; termType->TermTypeName; termType++)
    {
        if (termType->TermTypeCode == TermTypeCode)
        {
            break;
        }
    }

    return termType->TermTypeName;
}


//*****************************************************************************
//
// FormatTypeCodeToName()
//
//*****************************************************************************

PCTSTR
FormatTypeCodeToName (
    USHORT FormatTypeCode
)
{
    PFORMATTYPE formatType;

    for (formatType=FormatTypes; formatType->FormatTypeName; formatType++)
    {
        if (formatType->FormatTypeCode == FormatTypeCode)
        {
            break;
        }
    }

    return formatType->FormatTypeName;
}

#if _MSC_VER >= 1200
#pragma warning(pop)
#endif

