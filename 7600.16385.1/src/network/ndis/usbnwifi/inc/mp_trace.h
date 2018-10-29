/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Mp_Tracing.h

Abstract:
    WPP tracing defines
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
#ifndef _NATIVE_802_11_SAMPLE_DRIVER_
#define _NATIVE_802_11_SAMPLE_DRIVER_


#define DBG_SERIOUS            1        // TRACE_LEVEL_CRITICAL/FATAL
#define DBG_NORMAL             4        // TRACE_LEVEL_INFORMATION
#define DBG_LOUD               5        // TRACE_LEVEL_VERBOSE
#define DBG_TRACE              6        // TRACE_LEVEL_RESERVED6

//
// These are the various tracing components we can have. The max number is 31
// so always keep that in mind.
//
// WARNING! THIS GUID IS BEING USED BY THE REALTEK DRIVER.
// PLEASE GENERATE YOUR OWN UNIQUE GUID AND PLACE BELOW.
//
// Use COMP_TESTING when you want to add messages trace through a specific
// piece of code and not want to see trace from all other components
//
#define WPP_CONTROL_GUIDS                                                   \
    WPP_DEFINE_CONTROL_GUID(CtlGuid,(e49b27dd,b2f0,4571,8c6a,3271a3a3a6b9), \
        WPP_DEFINE_BIT(COMP_MISC)                                          \
        WPP_DEFINE_BIT(COMP_INIT_PNP)                                      \
        WPP_DEFINE_BIT(COMP_OID)                                           \
        WPP_DEFINE_BIT(COMP_RECV)                                          \
        WPP_DEFINE_BIT(COMP_SEND)                                          \
        WPP_DEFINE_BIT(COMP_PS_PACKETS)                                    \
        WPP_DEFINE_BIT(COMP_EVENTS)                                        \
        WPP_DEFINE_BIT(COMP_POWER)                                         \
        WPP_DEFINE_BIT(COMP_SCAN)                                          \
        WPP_DEFINE_BIT(COMP_ASSOC)                                         \
        WPP_DEFINE_BIT(COMP_DBG)                                           \
        WPP_DEFINE_BIT(COMP_TESTING)                                       \
    )

#ifdef WPP_COMPID_LEVEL_ENABLED
#undef WPP_COMPID_LEVEL_ENABLED
#endif

#define WPP_COMPID_LEVEL_ENABLED(CTL,LEVEL)             \
    ((WPP_CONTROL(WPP_BIT_ ## CTL).Level >= LEVEL) &&   \
    (WPP_CONTROL(WPP_BIT_ ## CTL).Flags[WPP_FLAG_NO(WPP_BIT_ ## CTL)] & WPP_MASK(WPP_BIT_ ## CTL)))

#ifndef WPP_COMPID_LEVEL_LOGGER
#  define WPP_COMPID_LEVEL_LOGGER(CTL,LEVEL)      (WPP_CONTROL(WPP_BIT_ ## CTL).Logger),
#endif


#endif // _NATIVE_802_11_SAMPLE_DRIVER_

