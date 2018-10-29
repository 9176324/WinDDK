/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Hw_Precomp.h

Abstract:
    Hw layer precompiled header
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
//
// Globally disabled warnings
// These are superfluous errors at warning level 4.
//
#pragma warning(disable:4200)  // nameless struct/union
#pragma warning(disable:4201)  // nameless struct/union
#pragma warning(disable:4214)  // bit field types other than int

#include <ntddk.h>
#include "usbdi.h"
#include "usbdlib.h"

//
// For making calls into NDIS
//
#include <ndis.h>


#include <wdf.h>
#include <wdfusb.h>
#include <wdfworkitem.h>
#include <wdfdevice.h>
#include <Wdfcore.h>
#include <wdfrequest.h>

#define NTSTRSAFE_LIB
#include <ntstrsafe.h>


                   
//
// Miscellanous globals to improve code readability etc.
//
#include "Globals.h"

//
// Various functions and data structures provided by WiFi
//
#include "Windot11.h"
#pragma warning(default:4200)
#pragma warning(default:4201)
#pragma warning(default:4214)



#pragma warning(disable:4200)  // nameless struct/union
#pragma warning(disable:4201)  // nameless struct/union
#pragma warning(disable:4214)  // bit field types other than int

#include "80211hdr.h"



//
// Definitions made by the Hw files needed in *Interface.h come here
//
#include "hw_def.h"

//
// Definitions necessary to interact with the station functionality
//
#include "st_def.h"

//
// Definitions necessary to interact with the miniport functionality
//
#include "mp_def.h"

//
// Calls made by the miniport/station portion into the vendor defined (hardware specific)
// functions
//
#include "hw_intf.h"

//
// Calls made by the hardware/station portion into the miniport funtions
//
#include "mp_intf.h"

//
// Calls made by the miniport/hardware portion into the station functions
// (for managing MAC state)
//
#include "st_intf.h"

//
// To provide debugging macros and functions to everyone
//
#include "mp_dbg.h"

//
// various files could use the ability to wrk with INFO_ELEMENTS
//
#include "mp_elem.h"
#include "r8180equ.h"
#include "hw_80211.h"
#include "hw_main.h"
#include "hw_ethnt.h"
#include "8187_reg.h"
#include "usb_reg.h"
#include "hw_eprom.h"

#pragma warning(default:4200)
#pragma warning(default:4201)
#pragma warning(default:4214)


