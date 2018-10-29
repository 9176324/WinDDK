/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    precomp.h

Abstract:
    Extensible station/MP layer precompile header
    
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
#pragma warning(disable:4214)   // bit field types other than int
#pragma warning(disable:4200)   // non-standard extension used
#pragma warning(disable:4201)   // nameless struct/union
#pragma warning(disable:4115)   // named type definition in parentheses
#pragma warning(disable:4127)   // conditional expression is constant
#pragma warning(disable:4054)   // cast of function pointer to PVOID
#pragma warning(disable:4206)   // translation unit is empty


//
// For making calls into NDIS
//
#include <ndis.h>
#include <wdf.h>
#include <WdfMiniport.h>
#include <wdftimer.h>


//
// Miscellanous globals to improve code readability etc.
//
#include "Globals.h"

//
// Various functions and data structures provided by WiFi
//
#include "Windot11.h"
#include "80211hdr.h"

//
// Definitions made by the Hw files needed in *Interface.h come here
//
#include "hw_Def.h"

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
#include "hW_intf.h"

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
// Interface functions that are special to our hardware
//
#include "Rtl_intf.h"

//
// To provide debugging macros and functions to everyone
//
#include "Mp_dbg.h"


//
// various files could use the ability to wrk with INFO_ELEMENTS
//
#include "mp_elem.h"

//
// Station includes
//
#include "st_main.h"
#include "st_misc.h"

#include "mp_def.h"

//
// Macros, queues and other defination used by the miniport driver
//
#include "mp_pktq.h"

#include "mp_dot11.h"
   

//
// Make the custom interfaces available globally
//
#include "mp_evnts.h"

