/*++

Copyright (c) 1997-1998 Microsoft Corporation

Module Name:

DISPLAY.C

Abstract:

This source file contains the routines which update the edit control
to display information about the selected USB device.

Environment:

user mode

Revision History:

04-25-97 : created

--*/

//*****************************************************************************
// I N C L U D E S
//*****************************************************************************

#include <windows.h>
#include <basetyps.h>
#include <stdio.h>
#include <stddef.h>
#include <tchar.h>
#include "vndrlist.h"
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

#define BUFFERALLOCINCREMENT        8192
#define BUFFERMINFREESPACE          1024

//*****************************************************************************
// T Y P E D E F S
//*****************************************************************************

//*****************************************************************************
// G L O B A L S    P R I V A T E    T O    T H I S    F I L E
//*****************************************************************************

// Workspace for text info which is used to update the edit control
//
PTSTR TextBuffer = NULL;
int   TextBufferLen = 0;
int   TextBufferPos = 0;

//*****************************************************************************
// L O C A L    F U N C T I O N    P R O T O T Y P E S
//*****************************************************************************

VOID
DisplayHubInfo (
    PUSB_HUB_INFORMATION HubInfo
);

VOID
DisplayHubCaps (
    PUSB_HUB_CAPABILITIES_EX HubCapsEx, 
    PUSB_HUB_CAPABILITIES HubCaps 
);

VOID
DisplayConnectionInfo (
    PUSB_NODE_CONNECTION_INFORMATION_EX    ConnectInfo,
    PSTRING_DESCRIPTOR_NODE             StringDescs
);

VOID
DisplayPipeInfo (
    ULONG           NumPipes,
    USB_PIPE_INFO  *PipeInfo
);

VOID
DisplayConfigDesc (
    PUSB_CONFIGURATION_DESCRIPTOR   ConfigDesc,
    PSTRING_DESCRIPTOR_NODE         StringDescs
);

VOID
DisplayConfigurationDescriptor (
    PUSB_CONFIGURATION_DESCRIPTOR   ConfigDesc,
    PSTRING_DESCRIPTOR_NODE         StringDescs
);

VOID
DisplayInterfaceDescriptor (
    PUSB_INTERFACE_DESCRIPTOR   InterfaceDesc,
    PSTRING_DESCRIPTOR_NODE     StringDescs
);

VOID
DisplayEndpointDescriptor (
    PUSB_ENDPOINT_DESCRIPTOR    EndpointDesc
);

VOID
DisplayHidDescriptor (
    PUSB_HID_DESCRIPTOR         HidDesc
);

VOID
DisplayStringDescriptor (
    UCHAR                       Index,
    PSTRING_DESCRIPTOR_NODE     StringDescs
);

VOID
DisplayUnknownDescriptor (
    PUSB_COMMON_DESCRIPTOR      CommonDesc
);

PTSTR
GetVendorString (
    USHORT     idVendor
);

//*****************************************************************************
// L O C A L    F U N C T I O N S
//*****************************************************************************


//*****************************************************************************
//
// CreateTextBuffer()
//
//*****************************************************************************

BOOL
CreateTextBuffer (
)
{
    // Allocate the buffer
    //
    TextBuffer = ALLOC(BUFFERALLOCINCREMENT * sizeof(TCHAR));

    if (TextBuffer == NULL)
    {
        OOPS();

        return FALSE;
    }

    TextBufferLen = BUFFERALLOCINCREMENT;

    // Reset the buffer position and terminate the buffer
    //
    *TextBuffer = 0;
    TextBufferPos = 0;

    return TRUE;
}


//*****************************************************************************
//
// DestroyTextBuffer()
//
//*****************************************************************************

VOID
DestroyTextBuffer (
)
{
    if (TextBuffer != NULL)
    {
        FREE(TextBuffer);

        TextBuffer = NULL;
    }
}


//*****************************************************************************
//
// ResetTextBuffer()
//
//*****************************************************************************

BOOL
ResetTextBuffer (
)
{
    // Fail if the text buffer has not been allocated
    //
    if (TextBuffer == NULL)
    {
        OOPS();

        return FALSE;
    }

    // Reset the buffer position and terminate the buffer
    //
    *TextBuffer = 0;
    TextBufferPos = 0;

    return TRUE;
}

//*****************************************************************************
//
// AppendTextBuffer()
//
//*****************************************************************************

VOID __cdecl
AppendTextBuffer (
    LPCTSTR lpFormat,
    ...
)
{
    va_list arglist;

    va_start(arglist, lpFormat);

    // Make sure we have a healthy amount of space free in the buffer,
    // reallocating the buffer if necessary.
    //
    if (TextBufferLen - TextBufferPos < BUFFERMINFREESPACE)
    {
        PTSTR TextBufferTmp;

        TextBufferTmp = REALLOC(TextBuffer, (TextBufferLen+BUFFERALLOCINCREMENT) * sizeof(TCHAR));

        if (TextBufferTmp != NULL)
        {
            TextBuffer = TextBufferTmp;
            TextBufferLen += BUFFERALLOCINCREMENT;
        }
        else
        {
            // If GlobalReAlloc fails, the original memory is not freed,
            // and the original handle and pointer are still valid.
            //
            OOPS();

            return;
        }
    }

    // Add the text to the end of the buffer, updating the buffer position.
    //
    TextBufferPos += _vstprintf_s(TextBuffer + TextBufferPos,
                               TextBufferLen - TextBufferPos,
                               lpFormat,
                               arglist);
}


//
// Hardcoded information about specific EHCI controllers
//
typedef struct _EHCI_CONTROLLER_DATA
{
    USHORT  VendorID;
    USHORT  DeviceID;
    UCHAR   DebugPortNumber;
} EHCI_CONTROLLER_DATA, *PEHCI_CONTROLLER_DATA;

EHCI_CONTROLLER_DATA EhciControllerData[] =
{
    {0x8086, 0x24CD, 1},    // INTEL ICH4
    {0x8086, 0x24DD, 1},    // INTEL ICH5
    {0x8086, 0x265C, 1},    // INTEL ICH6
    {0x8086, 0x27CC, 1},    // INTEL ICH7
    {0x10DE, 0x00D8, 1},
    {0,0,0},
};


//*****************************************************************************
//
// UpdateEditControl()
//
// hTreeItem - Handle of selected TreeView item for which information should
// be displayed in the edit control.
//
//*****************************************************************************

VOID
UpdateEditControl (
    HWND      hEditWnd,
    HWND      hTreeWnd,
    HTREEITEM hTreeItem
)
{
    TV_ITEM tvi;
    PVOID   info;
    ULONG   i;

    // Start with an empty text buffer.
    //
    if (!ResetTextBuffer())
    {
        return;
    }

    //
    // Get the name of the TreeView item, along with the a pointer to the
    // info we stored about the item in the item's lParam.
    //

    tvi.mask = TVIF_HANDLE | TVIF_TEXT | TVIF_PARAM;
    tvi.hItem = hTreeItem;
    tvi.pszText = TextBuffer;
    tvi.cchTextMax = TextBufferLen-2;  // leave space for "\r\n'

    TreeView_GetItem(hTreeWnd,
                     &tvi);

    info = (PVOID)tvi.lParam;

    //
    // If we didn't store any info for the item, just display the item's
    // name, else display the info we stored for the item.
    //
    if (info)
    {
        PUSB_NODE_INFORMATION               HubInfo = NULL;
        PCTSTR                              HubName = NULL;
        PUSB_HUB_CAPABILITIES               HubCaps = NULL;
        PUSB_HUB_CAPABILITIES_EX            HubCapsEx = NULL;
        PUSB_NODE_CONNECTION_INFORMATION_EX ConnectionInfo = NULL;
        PUSB_DESCRIPTOR_REQUEST             ConfigDesc = NULL;
        PSTRING_DESCRIPTOR_NODE             StringDescs = NULL;

        switch (*(PUSBDEVICEINFOTYPE)info)
        {
            case HostControllerInfo:
                AppendTextBuffer(_T("DriverKey: %s\r\n"),
                                 ((PUSBHOSTCONTROLLERINFO)info)->DriverKey);

                AppendTextBuffer(_T("VendorID: %04X\r\n"),
                                 ((PUSBHOSTCONTROLLERINFO)info)->VendorID);

                AppendTextBuffer(_T("DeviceID: %04X\r\n"),
                                 ((PUSBHOSTCONTROLLERINFO)info)->DeviceID);

                AppendTextBuffer(_T("SubSysID: %08X\r\n"),
                                 ((PUSBHOSTCONTROLLERINFO)info)->SubSysID);

                AppendTextBuffer(_T("Revision: %02X\r\n"),
                                 ((PUSBHOSTCONTROLLERINFO)info)->Revision);

                for (i = 0; EhciControllerData[i].VendorID; i++)
                {
                    if (((PUSBHOSTCONTROLLERINFO)info)->VendorID ==
                          EhciControllerData[i].VendorID &&
                        ((PUSBHOSTCONTROLLERINFO)info)->DeviceID ==
                          EhciControllerData[i].DeviceID)
                    {
                        AppendTextBuffer(_T("DebugPort: %d\r\n"),
                                         EhciControllerData[i].DebugPortNumber);
                    }
                }

                break;

            case RootHubInfo:
                HubInfo = ((PUSBROOTHUBINFO)info)->HubInfo;
                HubName = ((PUSBROOTHUBINFO)info)->HubName;
                HubCaps = ((PUSBROOTHUBINFO)info)->HubCaps;
                HubCapsEx = ((PUSBROOTHUBINFO)info)->HubCapsEx;

                AppendTextBuffer(_T("Root Hub: %s\r\n"),
                                 HubName);

                break;

            case ExternalHubInfo:
                HubInfo = ((PUSBEXTERNALHUBINFO)info)->HubInfo;
                HubName = ((PUSBEXTERNALHUBINFO)info)->HubName;
                HubCaps = ((PUSBEXTERNALHUBINFO)info)->HubCaps;
                HubCapsEx = ((PUSBEXTERNALHUBINFO)info)->HubCapsEx;
                ConnectionInfo = ((PUSBEXTERNALHUBINFO)info)->ConnectionInfo;
                ConfigDesc = ((PUSBEXTERNALHUBINFO)info)->ConfigDesc;
                StringDescs = ((PUSBEXTERNALHUBINFO)info)->StringDescs;

                AppendTextBuffer(_T("External Hub: %s\r\n"),
                                 HubName);

                break;

            case DeviceInfo:
                ConnectionInfo = ((PUSBDEVICEINFO)info)->ConnectionInfo;
                ConfigDesc = ((PUSBDEVICEINFO)info)->ConfigDesc;
                StringDescs = ((PUSBDEVICEINFO)info)->StringDescs;
                break;
        }

        if (HubInfo)
        {
            DisplayHubInfo(&HubInfo->u.HubInformation);
            DisplayHubCaps(HubCapsEx, HubCaps);
        }

        

        if (ConnectionInfo)
        {
            DisplayConnectionInfo(ConnectionInfo,
                                  StringDescs);
        }

        if (ConfigDesc)
        {
            DisplayConfigDesc((PUSB_CONFIGURATION_DESCRIPTOR)(ConfigDesc + 1),
                              StringDescs);
        }
    }

    // All done formatting text buffer with info, now update the edit
    // control with the contents of the text buffer
    //
    SetWindowText(hEditWnd, TextBuffer);
}


//*****************************************************************************
//
// DisplayHubInfo()
//
// HubInfo - Info about the hub.
//
//*****************************************************************************

VOID
DisplayHubInfo (
    PUSB_HUB_INFORMATION    HubInfo
)
{

    USHORT wHubChar;

    AppendTextBuffer(_T("Hub Power:               %s\r\n"),
                     HubInfo->HubIsBusPowered ?
                     _T("Bus Power") : _T("Self Power"));

    AppendTextBuffer(_T("Number of Ports:         %d\r\n"),
                     HubInfo->HubDescriptor.bNumberOfPorts);

    wHubChar = HubInfo->HubDescriptor.wHubCharacteristics;

    switch (wHubChar & 0x0003)
    {
        case 0x0000:
            AppendTextBuffer(_T("Power switching:         Ganged\r\n"));
            break;

        case 0x0001:
            AppendTextBuffer(_T("Power switching:         Individual\r\n"));
            break;

        case 0x0002:
        case 0x0003:
            AppendTextBuffer(_T("Power switching:         None\r\n"));
            break;
    }

    switch (wHubChar & 0x0004)
    {
        case 0x0000:
            AppendTextBuffer(_T("Compound device:         No\r\n"));
            break;

        case 0x0004:
            AppendTextBuffer(_T("Compound device:         Yes\r\n"));
            break;
    }

    switch (wHubChar & 0x0018)
    {
        case 0x0000:
            AppendTextBuffer(_T("Over-current Protection: Global\r\n"));
            break;

        case 0x0008:
            AppendTextBuffer(_T("Over-current Protection: Individual\r\n"));
            break;

        case 0x0010:
        case 0x0018:
            AppendTextBuffer(_T("No Over-current Protection (Bus Power Only)\r\n"));
            break;
    }

    AppendTextBuffer(_T("\r\n"));

}


//*****************************************************************************
//
// DisplayHubCaps()
//
// HubCapsEx - Extended Capabilities
// HubCaps - Basic capabilities
//
//*****************************************************************************

VOID
DisplayHubCaps (
    PUSB_HUB_CAPABILITIES_EX HubCapsEx, 
    PUSB_HUB_CAPABILITIES HubCaps 
)
{
    if (HubCapsEx) {
#if (_WIN32_WINNT >= 0x0600) 
        // Only available on Vista + 
        PUSB_HUB_CAP_FLAGS HubCapFlags = (PUSB_HUB_CAP_FLAGS) &(HubCapsEx->CapabilityFlags);
        AppendTextBuffer(_T("Extended Hub Capability Flags:  %0#8lx\r\n"), HubCapFlags->ul);
        AppendTextBuffer(_T("    High speed Capable:  %s\r\n"), (HubCapFlags->HubIsHighSpeedCapable ? _T("Yes") : _T("No")));
        AppendTextBuffer(_T("    High speed:  %s\r\n"), (HubCapFlags->HubIsHighSpeed ? _T("Yes") : _T("No")));
        AppendTextBuffer(_T("    Mulit-transaction Capable:  %s\r\n"), (HubCapFlags->HubIsMultiTtCapable ? _T("Yes") : _T("No")));
        AppendTextBuffer(_T("    Mulit-transaction ON:  %s\r\n"), (HubCapFlags->HubIsMultiTt ? _T("Yes") : _T("No")));
        AppendTextBuffer(_T("    Root hub:  %s\r\n"), (HubCapFlags->HubIsRoot? _T("Yes") : _T("No")));
        AppendTextBuffer(_T("    Armed for wake on connect:  %s\r\n"), (HubCapFlags->HubIsArmedWakeOnConnect ? _T("Yes") : _T("No")));
        AppendTextBuffer(_T("    Reserved (26 bits):  %0#6lx\r\n"), HubCapFlags->ReservedMBZ);

        // Don't display un-extended caps if extended caps are available, they don't appear to be correct.
#endif
    } else {
        AppendTextBuffer(_T("Extended Hub Capabilities UNAVAILABLE\r\n"));
        // Pre-Vista this is all we've got
        if (HubCaps) {
            AppendTextBuffer(_T("Hub Capabilities:  %0#8lx (%s)\r\n"), HubCaps->HubIs2xCapable, (HubCaps->HubIs2xCapable? _T("High speed") : _T("Not high speed")));
        } else {
            AppendTextBuffer(_T("Hub Capabilities UNAVAILABLE\r\n"));
        }
    }
    AppendTextBuffer(_T("\r\n"));
}

//*****************************************************************************
//
// DisplayConnectionInfo()
//
// ConnectInfo - Info about the connection.
//
//*****************************************************************************

VOID
DisplayConnectionInfo (
    PUSB_NODE_CONNECTION_INFORMATION_EX    ConnectInfo,
    PSTRING_DESCRIPTOR_NODE             StringDescs
)
{

    if (ConnectInfo->ConnectionStatus == NoDeviceConnected)
    {
        AppendTextBuffer(_T("ConnectionStatus: NoDeviceConnected\r\n"));
    }
    else
    {
        PCTSTR VendorString;

        AppendTextBuffer(_T("Device Descriptor:\r\n"));

        AppendTextBuffer(_T("bcdUSB:             0x%04X\r\n"),
                         ConnectInfo->DeviceDescriptor.bcdUSB);

        AppendTextBuffer(_T("bDeviceClass:         0x%02X\r\n"),
                         ConnectInfo->DeviceDescriptor.bDeviceClass);

        AppendTextBuffer(_T("bDeviceSubClass:      0x%02X\r\n"),
                         ConnectInfo->DeviceDescriptor.bDeviceSubClass);

        AppendTextBuffer(_T("bDeviceProtocol:      0x%02X\r\n"),
                         ConnectInfo->DeviceDescriptor.bDeviceProtocol);

        AppendTextBuffer(_T("bMaxPacketSize0:      0x%02X (%d)\r\n"),
                         ConnectInfo->DeviceDescriptor.bMaxPacketSize0,
                         ConnectInfo->DeviceDescriptor.bMaxPacketSize0);

        VendorString = GetVendorString(ConnectInfo->DeviceDescriptor.idVendor);

        if (VendorString != NULL)
        {
            AppendTextBuffer(_T("idVendor:           0x%04X (%s)\r\n"),
                             ConnectInfo->DeviceDescriptor.idVendor,
                             VendorString);
        }
        else
        {
            AppendTextBuffer(_T("idVendor:           0x%04X\r\n"),
                             ConnectInfo->DeviceDescriptor.idVendor);
        }

        AppendTextBuffer(_T("idProduct:          0x%04X\r\n"),
                         ConnectInfo->DeviceDescriptor.idProduct);

        AppendTextBuffer(_T("bcdDevice:          0x%04X\r\n"),
                         ConnectInfo->DeviceDescriptor.bcdDevice);

        AppendTextBuffer(_T("iManufacturer:        0x%02X\r\n"),
                         ConnectInfo->DeviceDescriptor.iManufacturer);

        if (ConnectInfo->DeviceDescriptor.iManufacturer)
        {
            DisplayStringDescriptor(ConnectInfo->DeviceDescriptor.iManufacturer,
                                    StringDescs);
        }

        AppendTextBuffer(_T("iProduct:             0x%02X\r\n"),
                         ConnectInfo->DeviceDescriptor.iProduct);

        if (ConnectInfo->DeviceDescriptor.iProduct)
        {
            DisplayStringDescriptor(ConnectInfo->DeviceDescriptor.iProduct,
                                    StringDescs);
        }

        AppendTextBuffer(_T("iSerialNumber:        0x%02X\r\n"),
                         ConnectInfo->DeviceDescriptor.iSerialNumber);

        if (ConnectInfo->DeviceDescriptor.iSerialNumber)
        {
            DisplayStringDescriptor(ConnectInfo->DeviceDescriptor.iSerialNumber,
                                    StringDescs);
        }

        AppendTextBuffer(_T("bNumConfigurations:   0x%02X\r\n"),
                         ConnectInfo->DeviceDescriptor.bNumConfigurations);

        AppendTextBuffer(_T("\r\nConnectionStatus: %s\r\n"),
                         ConnectionStatuses[ConnectInfo->ConnectionStatus]);

        AppendTextBuffer(_T("Current Config Value: 0x%02X\r\n"),
                         ConnectInfo->CurrentConfigurationValue);

	switch	(ConnectInfo->Speed){
		case UsbLowSpeed:
			AppendTextBuffer(_T("Device Bus Speed:     Low\r\n"));
			break;
		case UsbFullSpeed:
			AppendTextBuffer(_T("Device Bus Speed:     Full\r\n"));
			break;
		case UsbHighSpeed:
			AppendTextBuffer(_T("Device Bus Speed:     High\r\n"));
			break;
		default:
			AppendTextBuffer(_T("Device Bus Speed:     Unknown\r\n"));

	}

        AppendTextBuffer(_T("Device Address:       0x%02X\r\n"),
                         ConnectInfo->DeviceAddress);

        AppendTextBuffer(_T("Open Pipes:             %2d\r\n"),
                         ConnectInfo->NumberOfOpenPipes);

        if (ConnectInfo->NumberOfOpenPipes)
        {
            DisplayPipeInfo(ConnectInfo->NumberOfOpenPipes,
                            ConnectInfo->PipeList);
        }
    }
}

//*****************************************************************************
//
// DisplayPipeInfo()
//
// NumPipes - Number of pipe for we info should be displayed.
//
// PipeInfo - Info about the pipes.
//
//*****************************************************************************

VOID
DisplayPipeInfo (
    ULONG           NumPipes,
    USB_PIPE_INFO  *PipeInfo
)
{
    ULONG i;

    for (i=0; i<NumPipes; i++)
    {
        DisplayEndpointDescriptor(&PipeInfo[i].EndpointDescriptor);
    }

}


//*****************************************************************************
//
// DisplayConfigDesc()
//
// ConfigDesc - The Configuration Descriptor, and associated Interface and
// EndpointDescriptors
//
//*****************************************************************************

VOID
DisplayConfigDesc (
    PUSB_CONFIGURATION_DESCRIPTOR   ConfigDesc,
    PSTRING_DESCRIPTOR_NODE         StringDescs
)
{
    PUCHAR                  descEnd;
    PUSB_COMMON_DESCRIPTOR  commonDesc;
    UCHAR                   bInterfaceClass;
    UCHAR                   bInterfaceSubClass;
    BOOL                    displayUnknown;

    bInterfaceClass = 0;

    descEnd = (PUCHAR)ConfigDesc + ConfigDesc->wTotalLength;

    commonDesc = (PUSB_COMMON_DESCRIPTOR)ConfigDesc;

    while ((PUCHAR)commonDesc + sizeof(USB_COMMON_DESCRIPTOR) < descEnd &&
           (PUCHAR)commonDesc + commonDesc->bLength <= descEnd)
    {
        displayUnknown = FALSE;

        switch (commonDesc->bDescriptorType)
        {
            case USB_CONFIGURATION_DESCRIPTOR_TYPE:
                if (commonDesc->bLength != sizeof(USB_CONFIGURATION_DESCRIPTOR))
                {
                    OOPS();
                    displayUnknown = TRUE;
                    break;
                }
                DisplayConfigurationDescriptor((PUSB_CONFIGURATION_DESCRIPTOR)commonDesc,
                                               StringDescs);
                break;

            case USB_INTERFACE_DESCRIPTOR_TYPE:
                if ((commonDesc->bLength != sizeof(USB_INTERFACE_DESCRIPTOR)) &&
                    (commonDesc->bLength != sizeof(USB_INTERFACE_DESCRIPTOR2)))
                {
                    OOPS();
                    displayUnknown = TRUE;
                    break;
                }
                bInterfaceClass = ((PUSB_INTERFACE_DESCRIPTOR)commonDesc)->bInterfaceClass;
                bInterfaceSubClass = ((PUSB_INTERFACE_DESCRIPTOR)commonDesc)->bInterfaceSubClass;

                DisplayInterfaceDescriptor((PUSB_INTERFACE_DESCRIPTOR)commonDesc,
                                           StringDescs);
                break;

            case USB_ENDPOINT_DESCRIPTOR_TYPE:
                if ((commonDesc->bLength != sizeof(USB_ENDPOINT_DESCRIPTOR)) &&
                    (commonDesc->bLength != sizeof(USB_ENDPOINT_DESCRIPTOR2)))
                {
                    OOPS();
                    displayUnknown = TRUE;
                    break;
                }
                DisplayEndpointDescriptor((PUSB_ENDPOINT_DESCRIPTOR)commonDesc);
                break;

            case USB_HID_DESCRIPTOR_TYPE:
                if (commonDesc->bLength < sizeof(USB_HID_DESCRIPTOR))
                {
                    OOPS();
                    displayUnknown = TRUE;
                    break;
                }
                DisplayHidDescriptor((PUSB_HID_DESCRIPTOR)commonDesc);
                break;

            default:
                switch (bInterfaceClass)
                {
                    case USB_DEVICE_CLASS_AUDIO:
                        displayUnknown = !DisplayAudioDescriptor(
                                              (PUSB_AUDIO_COMMON_DESCRIPTOR)commonDesc,
                                              bInterfaceSubClass);
                        break;

                    default:
                        displayUnknown = TRUE;
                        break;
                }
                break;
        }

        if (displayUnknown)
        {
            DisplayUnknownDescriptor(commonDesc);
        }

        (PUCHAR)commonDesc += commonDesc->bLength;
    }
}


//*****************************************************************************
//
// DisplayConfigurationDescriptor()
//
//*****************************************************************************

VOID
DisplayConfigurationDescriptor (
    PUSB_CONFIGURATION_DESCRIPTOR   ConfigDesc,
    PSTRING_DESCRIPTOR_NODE         StringDescs
)
{

    AppendTextBuffer(_T("\r\nConfiguration Descriptor:\r\n"));

    AppendTextBuffer(_T("wTotalLength:       0x%04X\r\n"),
                     ConfigDesc->wTotalLength);

    AppendTextBuffer(_T("bNumInterfaces:       0x%02X\r\n"),
                     ConfigDesc->bNumInterfaces);

    AppendTextBuffer(_T("bConfigurationValue:  0x%02X\r\n"),
                     ConfigDesc->bConfigurationValue);

    AppendTextBuffer(_T("iConfiguration:       0x%02X\r\n"),
                     ConfigDesc->iConfiguration);

    if (ConfigDesc->iConfiguration)
    {
        DisplayStringDescriptor(ConfigDesc->iConfiguration,
                                StringDescs);
    }

    AppendTextBuffer(_T("bmAttributes:         0x%02X ("),
                     ConfigDesc->bmAttributes);

    if (ConfigDesc->bmAttributes & 0x80)
    {
        AppendTextBuffer(_T("Bus Powered "));
    }

    if (ConfigDesc->bmAttributes & 0x40)
    {
        AppendTextBuffer(_T("Self Powered "));
    }

    if (ConfigDesc->bmAttributes & 0x20)
    {
        AppendTextBuffer(_T("Remote Wakeup"));
    }

    AppendTextBuffer(_T(")\r\n"));

    AppendTextBuffer(_T("MaxPower:             0x%02X (%d mA)\r\n"),
                     ConfigDesc->MaxPower,
                     ConfigDesc->MaxPower * 2);

}

//*****************************************************************************
//
// DisplayInterfaceDescriptor()
//
//*****************************************************************************

VOID
DisplayInterfaceDescriptor (
    PUSB_INTERFACE_DESCRIPTOR   InterfaceDesc,
    PSTRING_DESCRIPTOR_NODE     StringDescs
)
{
    PCTSTR pStr;

    AppendTextBuffer(_T("\r\nInterface Descriptor:\r\n"));

    AppendTextBuffer(_T("bInterfaceNumber:     0x%02X\r\n"),
                     InterfaceDesc->bInterfaceNumber);

    AppendTextBuffer(_T("bAlternateSetting:    0x%02X\r\n"),
                     InterfaceDesc->bAlternateSetting);

    AppendTextBuffer(_T("bNumEndpoints:        0x%02X\r\n"),
                     InterfaceDesc->bNumEndpoints);

    AppendTextBuffer(_T("bInterfaceClass:      0x%02X"),
                     InterfaceDesc->bInterfaceClass);

    pStr = _T("\r\n");

    switch (InterfaceDesc->bInterfaceClass)
    {
        case USB_DEVICE_CLASS_AUDIO:
            pStr = _T(" (Audio)\r\n");
            break;

        case USB_DEVICE_CLASS_HUMAN_INTERFACE:
            pStr = _T(" (HID)\r\n");
            break;

        case USB_DEVICE_CLASS_HUB:
            pStr = _T(" (Hub)\r\n");
            break;

        default:
            break;
    }

    AppendTextBuffer(pStr);

    AppendTextBuffer(_T("bInterfaceSubClass:   0x%02X"),
                     InterfaceDesc->bInterfaceSubClass);

    pStr = _T("\r\n");

    switch (InterfaceDesc->bInterfaceClass)
    {
        case USB_DEVICE_CLASS_AUDIO:
            switch (InterfaceDesc->bInterfaceSubClass)
            {
                case USB_AUDIO_SUBCLASS_AUDIOCONTROL:
                    pStr = _T(" (Audio Control)\r\n");
                    break;

                case USB_AUDIO_SUBCLASS_AUDIOSTREAMING:
                    pStr = _T(" (Audio Streaming)\r\n");
                    break;

                case USB_AUDIO_SUBCLASS_MIDISTREAMING:
                    pStr = _T(" (MIDI Streaming)\r\n");
                    break;

                default:
                    break;
            }
            break;

        default:
            break;
    }

    AppendTextBuffer(pStr);

    AppendTextBuffer(_T("bInterfaceProtocol:   0x%02X\r\n"),
                     InterfaceDesc->bInterfaceProtocol);

    AppendTextBuffer(_T("iInterface:           0x%02X\r\n"),
                     InterfaceDesc->iInterface);

    if (InterfaceDesc->iInterface)
    {
        DisplayStringDescriptor(InterfaceDesc->iInterface,
                                StringDescs);
    }

    if (InterfaceDesc->bLength == sizeof(USB_INTERFACE_DESCRIPTOR2))
    {
        PUSB_INTERFACE_DESCRIPTOR2 interfaceDesc2;

        interfaceDesc2 = (PUSB_INTERFACE_DESCRIPTOR2)InterfaceDesc;

        AppendTextBuffer(_T("wNumClasses:        0x%04X\r\n"),
                         interfaceDesc2->wNumClasses);
    }

}

//*****************************************************************************
//
// DisplayEndpointDescriptor()
//
//*****************************************************************************

VOID
DisplayEndpointDescriptor (
    PUSB_ENDPOINT_DESCRIPTOR    EndpointDesc
)
{

    AppendTextBuffer(_T("\r\nEndpoint Descriptor:\r\n"));

    if (USB_ENDPOINT_DIRECTION_IN(EndpointDesc->bEndpointAddress))
    {
        AppendTextBuffer(_T("bEndpointAddress:     0x%02X  IN\r\n"),
               EndpointDesc->bEndpointAddress);
    }
    else
    {
        AppendTextBuffer(_T("bEndpointAddress:     0x%02X  OUT\r\n"),
               EndpointDesc->bEndpointAddress);
    }

    switch (EndpointDesc->bmAttributes & 0x03)
    {
        case 0x00:
            AppendTextBuffer(_T("Transfer Type:     Control\r\n"));
            break;

        case 0x01:
            AppendTextBuffer(_T("Transfer Type: Isochronous\r\n"));
            break;

        case 0x02:
            AppendTextBuffer(_T("Transfer Type:        Bulk\r\n"));
            break;

        case 0x03:
            AppendTextBuffer(_T("Transfer Type:   Interrupt\r\n"));
            break;

    }

    AppendTextBuffer(_T("wMaxPacketSize:     0x%04X (%d)\r\n"),
                     EndpointDesc->wMaxPacketSize,
                     EndpointDesc->wMaxPacketSize);

    if (EndpointDesc->bLength == sizeof(USB_ENDPOINT_DESCRIPTOR))
    {
        AppendTextBuffer(_T("bInterval:            0x%02X\r\n"),
                         EndpointDesc->bInterval);
    }
    else
    {
        PUSB_ENDPOINT_DESCRIPTOR2 endpointDesc2;

        endpointDesc2 = (PUSB_ENDPOINT_DESCRIPTOR2)EndpointDesc;

        AppendTextBuffer(_T("wInterval:          0x%04X\r\n"),
                         endpointDesc2->wInterval);

        AppendTextBuffer(_T("bSyncAddress:         0x%02X\r\n"),
                         endpointDesc2->bSyncAddress);
    }

}


//*****************************************************************************
//
// DisplayHidDescriptor()
//
//*****************************************************************************

VOID
DisplayHidDescriptor (
    PUSB_HID_DESCRIPTOR         HidDesc
)
{
    UCHAR i;

    AppendTextBuffer(_T("\r\nHID Descriptor:\r\n"));

    AppendTextBuffer(_T("bcdHID:             0x%04X\r\n"),
    HidDesc->bcdHID);

    AppendTextBuffer(_T("bCountryCode:         0x%02X\r\n"),
                     HidDesc->bCountryCode);

    AppendTextBuffer(_T("bNumDescriptors:      0x%02X\r\n"),
                     HidDesc->bNumDescriptors);

    for (i=0; i<HidDesc->bNumDescriptors; i++)
    {
        AppendTextBuffer(_T("bDescriptorType:      0x%02X\r\n"),
                         HidDesc->OptionalDescriptors[i].bDescriptorType);

        AppendTextBuffer(_T("wDescriptorLength:  0x%04X\r\n"),
                         HidDesc->OptionalDescriptors[i].wDescriptorLength);
    }

}


#if 0
//*****************************************************************************
//
// DisplayPowerDescriptor()
//
//*****************************************************************************

PCTSTR PowerUnits[] =
{
    _T("0.001 mW"),
    _T("0.01 mW"),
    _T("0.1 mW"),
    _T("1 mW"),
    _T("10 mW"),
    _T("100 mW"),
    _T("1 W")
};

VOID
DisplayPowerDescriptor (
    PUSB_POWER_DESCRIPTOR       PowerDesc
)
{
    UCHAR i;

    AppendTextBuffer(_T("\r\nPower Descriptor:\r\n"));

    AppendTextBuffer(_T("bCapabilitiesFlags:   0x%02X ("),
                     PowerDesc->bCapabilitiesFlags);

    if (PowerDesc->bCapabilitiesFlags & USB_SUPPORT_D2_WAKEUP)
    {
        AppendTextBuffer(_T("WakeD2 "));
    }
    if (PowerDesc->bCapabilitiesFlags & USB_SUPPORT_D1_WAKEUP)
    {
        AppendTextBuffer(_T("WakeD1 "));
    }
    if (PowerDesc->bCapabilitiesFlags & USB_SUPPORT_D3_COMMAND)
    {
        AppendTextBuffer(_T("D3 "));
    }
    if (PowerDesc->bCapabilitiesFlags & USB_SUPPORT_D2_COMMAND)
    {
        AppendTextBuffer(_T("D2 "));
    }
    if (PowerDesc->bCapabilitiesFlags & USB_SUPPORT_D1_COMMAND)
    {
        AppendTextBuffer(_T("D1 "));
    }
    if (PowerDesc->bCapabilitiesFlags & USB_SUPPORT_D0_COMMAND)
    {
        AppendTextBuffer(_T("D0 "));
    }
    AppendTextBuffer(_T(")\r\n"));

    AppendTextBuffer(_T("EventNotification:  0x%04X\r\n"),
                     PowerDesc->EventNotification);

    AppendTextBuffer(_T("D1LatencyTime:      0x%04X\r\n"),
                     PowerDesc->D1LatencyTime);

    AppendTextBuffer(_T("D2LatencyTime:      0x%04X\r\n"),
                     PowerDesc->D2LatencyTime);

    AppendTextBuffer(_T("D3LatencyTime:      0x%04X\r\n"),
                     PowerDesc->D3LatencyTime);

    AppendTextBuffer(_T("PowerUnit:            0x%02X (%s)\r\n"),
                     PowerDesc->PowerUnit,
                     PowerDesc->PowerUnit < sizeof(PowerUnits)/sizeof(PowerUnits[0])
                     ? PowerUnits[PowerDesc->PowerUnit]
                     : _T("?"));

    AppendTextBuffer(_T("D0PowerConsumption: 0x%04X (%5d)\r\n"),
                     PowerDesc->D0PowerConsumption,
                     PowerDesc->D0PowerConsumption);

    AppendTextBuffer(_T("D1PowerConsumption: 0x%04X (%5d)\r\n"),
                     PowerDesc->D1PowerConsumption,
                     PowerDesc->D1PowerConsumption);

    AppendTextBuffer(_T("D2PowerConsumption: 0x%04X (%5d)\r\n"),
                     PowerDesc->D2PowerConsumption,
                     PowerDesc->D2PowerConsumption);

}
#endif

//*****************************************************************************
//
// DisplayStringDescriptor()
//
//*****************************************************************************


VOID
DisplayStringDescriptor (
    UCHAR                       Index,
    PSTRING_DESCRIPTOR_NODE     StringDescs
)
{
    // Use an actual "int" here because it's passed as a printf * precision
    int descChars;  

    while (StringDescs)
    {
        if (StringDescs->DescriptorIndex == Index)
        {

            //
            // bString from USB_STRING_DESCRIPTOR isn't NULL-terminated, so 
            // calculate the number of characters.  
            // 
            // bLength is the length of the whole structure, not just the string.  
            // 
            // bLength is bytes, bString is WCHARs
            // 
            descChars = 
                ( (int) StringDescs->StringDescriptor->bLength - 
                offsetof(USB_STRING_DESCRIPTOR, bString) ) /
                sizeof(WCHAR);
            //
            // Use the * precision and pass the number of characters just caculated.
            // bString is always WCHAR so specify widestring regardless of what TCHAR resolves to
            // 
            AppendTextBuffer(_T("0x%04X: \"%.*ws\"\r\n"),
                StringDescs->LanguageID,
                descChars,
                StringDescs->StringDescriptor->bString);
        }

        StringDescs = StringDescs->Next;
    }

}


//*****************************************************************************
//
// DisplayUnknownDescriptor()
//
//*****************************************************************************

VOID
DisplayUnknownDescriptor (
    PUSB_COMMON_DESCRIPTOR      CommonDesc
)
{
    UCHAR   i;

    AppendTextBuffer(_T("\r\nUnknown Descriptor:\r\n"));

    AppendTextBuffer(_T("bDescriptorType:      0x%02X\r\n"),
                     CommonDesc->bDescriptorType);

    AppendTextBuffer(_T("bLength:              0x%02X\r\n"),
                     CommonDesc->bLength);

    for (i = 0; i < CommonDesc->bLength; i++)
    {
        AppendTextBuffer(_T("%02X "),
                         ((PUCHAR)CommonDesc)[i]);

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
// GetVendorString()
//
// idVendor - USB Vendor ID
//
// Return Value - Vendor name string associated with idVendor, or NULL if
// no vendor name string is found which is associated with idVendor.
//
//*****************************************************************************

PTSTR
GetVendorString (
    USHORT     idVendor
)
{
    PUSBVENDORID vendorID;

    if (idVendor != 0x0000)
    {
        vendorID = USBVendorIDs;

        while (vendorID->usVendorID != 0x0000)
        {
            if (vendorID->usVendorID == idVendor)
            {
                return (vendorID->szVendor);
            }
            vendorID++;
        }
    }

    return NULL;
}

#if _MSC_VER >= 1200
#pragma warning(pop)
#endif

