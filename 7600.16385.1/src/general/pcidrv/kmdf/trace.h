/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    TRACE.h

Abstract:

    Header file for the debug tracing related function defintions and macros.
    HEXDUMP is not defined by default. So we have to provide a localwpp.ini file
    that defines type.

Environment:

    Kernel mode

--*/

#include <evntrace.h> // For TRACE_LEVEL definitions

#if !defined(EVENT_TRACING)

//
// TODO: These defines are missing in evntrace.h
// in some DDK build environments (XP).
//
#if !defined(TRACE_LEVEL_NONE)
  #define TRACE_LEVEL_NONE        0
  #define TRACE_LEVEL_CRITICAL    1
  #define TRACE_LEVEL_FATAL       1
  #define TRACE_LEVEL_ERROR       2
  #define TRACE_LEVEL_WARNING     3
  #define TRACE_LEVEL_INFORMATION 4
  #define TRACE_LEVEL_VERBOSE     5
  #define TRACE_LEVEL_RESERVED6   6
  #define TRACE_LEVEL_RESERVED7   7
  #define TRACE_LEVEL_RESERVED8   8
  #define TRACE_LEVEL_RESERVED9   9
#endif


//
// Define Debug Flags
//
#define DBG_INIT                0x00000001
#define DBG_PNP                 0x00000002
#define DBG_POWER               0x00000004
#define DBG_WMI                 0x00000008
#define DBG_CREATE_CLOSE        0x00000010
#define DBG_IOCTLS              0x00000020
#define DBG_WRITE               0x00000040
#define DBG_READ                0x00000080
#define DBG_DPC                 0x00000100
#define DBG_INTERRUPT           0x00000200
#define DBG_LOCKS               0x00000400
#define DBG_QUEUEING            0x00000800
#define DBG_HW_ACCESS           0x00001000

VOID
TraceEvents    (
    IN ULONG   DebugPrintLevel,
    IN ULONG   DebugPrintFlag,
    IN PCCHAR  DebugMessage,
    ...
    );

#define Hexdump(x) // Used for HEXDUMP in case tracing is enabled
#define WPP_INIT_TRACING(DriverObject, RegistryPath)
#define WPP_CLEANUP(DriverObject)

#else
//
// If software tracing is defined in the sources file..
// WPP_DEFINE_CONTROL_GUID specifies the GUID used for this driver.
// *** REPLACE THE GUID WITH YOUR OWN UNIQUE ID ***
// WPP_DEFINE_BIT allows setting debug bit masks to selectively print.
// The names defined in the WPP_DEFINE_BIT call define the actual names
// that are used to control the level of tracing for the control guid
// specified.
//
// Name of the logger is PciDrv and the guid is
//   {BC6C9364-FC67-42c5-ACF7-ABED3B12ECC6}
//   (0xbc6c9364, 0xfc67, 0x42c5, 0xac, 0xf7, 0xab, 0xed, 0x3b, 0x12, 0xec, 0xc6);
//

#define WPP_CHECK_FOR_NULL_STRING  //to prevent exceptions due to NULL strings

#define WPP_CONTROL_GUIDS \
    WPP_DEFINE_CONTROL_GUID(PciDrvTraceGuid,(bc6c9364,fc67,42c5,acf7,abed3b12ecc6), \
        WPP_DEFINE_BIT(DBG_INIT)             /* bit  0 = 0x00000001 */ \
        WPP_DEFINE_BIT(DBG_PNP)              /* bit  1 = 0x00000002 */ \
        WPP_DEFINE_BIT(DBG_POWER)            /* bit  2 = 0x00000004 */ \
        WPP_DEFINE_BIT(DBG_WMI)              /* bit  3 = 0x00000008 */ \
        WPP_DEFINE_BIT(DBG_CREATE_CLOSE)     /* bit  4 = 0x00000010 */ \
        WPP_DEFINE_BIT(DBG_IOCTLS)           /* bit  5 = 0x00000020 */ \
        WPP_DEFINE_BIT(DBG_WRITE)            /* bit  6 = 0x00000040 */ \
        WPP_DEFINE_BIT(DBG_READ)             /* bit  7 = 0x00000080 */ \
        WPP_DEFINE_BIT(DBG_DPC)              /* bit  8 = 0x00000100 */ \
        WPP_DEFINE_BIT(DBG_INTERRUPT)        /* bit  9 = 0x00000200 */ \
        WPP_DEFINE_BIT(DBG_LOCKS)            /* bit 10 = 0x00000400 */ \
        WPP_DEFINE_BIT(DBG_QUEUEING)         /* bit 11 = 0x00000800 */ \
        WPP_DEFINE_BIT(DBG_HW_ACCESS)        /* bit 12 = 0x00001000 */ \
        /* You can have up to 32 defines. If you want more than that,\
           you have to provide another trace control GUID */\
        )


#define WPP_LEVEL_FLAGS_LOGGER(lvl,flags) WPP_LEVEL_LOGGER(flags)
#define WPP_LEVEL_FLAGS_ENABLED(lvl, flags) (WPP_LEVEL_ENABLED(flags) && WPP_CONTROL(WPP_BIT_ ## flags).Level  >= lvl)

#pragma warning(disable:4204) // C4204 nonstandard extension used : non-constant aggregate initializer

//
// Define the 'xstr' structure for logging buffer and length pairs
// and the 'log_xstr' function which returns it to create one in-place.
// this enables logging of complex data types.
//
typedef struct xstr { char * _buf; short  _len; } xstr_t;
__inline xstr_t log_xstr(void * p, short l) { xstr_t xs = {(char*)p,l}; return xs; }

#pragma warning(default:4204)

//
// Define the macro required for a hexdump use as:
//
//   DebugTraceEx((LEVEL, FLAG,"%!HEXDUMP!\n", log_xstr(buffersize,(char *)buffer) ));
//
//
#define WPP_LOGHEXDUMP(x) WPP_LOGPAIR(2, &((x)._len)) WPP_LOGPAIR((x)._len, (x)._buf)

#endif


