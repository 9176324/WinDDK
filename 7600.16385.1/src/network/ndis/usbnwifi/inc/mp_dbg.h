/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Mp_Dbg.h

Abstract:
    Debug functions and macros
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/

#ifndef _NATIVE_WIFI__MP_DBG_

#define _NATIVE_WIFI__MP_DBG_

//
// TODO: REMOVE THESE ONCE NDIS DEFINES THEM
//
#define         NDIS_STATUS_BAD_ALIGNMENT           0xC0009999
#define         NDIS_STATUS_CAPABILITY_UNAVAILABLE 0xC0001114 

//
// Value from updated 1.67 spec
//
#define         DOT11_OPERATION_MODE_NETWORK_MONITOR    0x80000000
//#endif


//
// Turn tracing on for this driver if requested. When tracing is on
// debug messages will not be posted to the debugger. ASSERT etc. will
// still work so attaching a debugger is advised.
//
#if DOT11_TRACE_ENABLED


#include "mp_trace.h"


#endif  // DOT11_TRACE_ENABLED


#if DBG



//
//    ==============================
//    MEMORY MANAGEMENT DEBUG APIS
//    ==============================
//

#define  MP_HEADER_PATTERN          0x76028412
#define  MP_TRAILER_PATTERN         0x83395627
#define  MP_FREED_PATTERN           0x82962503

//
// Note: it's important that the size of MP_MEMORY_BLOCK structure
//       be multiple of 16 bytes.
//
typedef struct _MP_MEMORY_BLOCK
{
  /** Pointer to next and previous blocks */
  LIST_ENTRY  ListEntry;
  /** name of the file which is doing the allocation */
  PCHAR       pstrFile;
  /** pad to make the size of whole structure multiple of 16 bytes */
  PVOID       pad;
  /** line number where allocated */
  ULONG       ulLine;
  /** ulong index of trailer (=(length/4)-1 relative to data start */
  ULONG       ulLength;
  /** Flags that can be used in future to do special allocations */
  ULONG       ulFlags;
  /** To help detect underflows. A trailer is also added to find overflows */
  ULONG       ulHeaderPattern;
} MP_MEMORY_BLOCK, *PMP_MEMORY_BLOCK;

PVOID 
MpAllocateMemory (
    NDIS_HANDLE         AllocateHandle,
    ULONG               ulSize,
    ULONG               ulTag,
    EX_POOL_PRIORITY    Priority,
    PCSTR               pstrFileName,
    ULONG               ulLineNumber,
    ULONG               ulFlags
    );

VOID
MpFreeMemory (
    PVOID    pMemory
    );

VOID 
MpFreeAllocatedBlocks();


/**
* Use this macro to allocate memory whenever needed. For debug versions of the driver,
* this macro expands to MpAllocateMemory and tracks all memory allocated with detection
* of overflow and underflow. For retail builds, this macro expands to a direct call to Ndis for
* memory allocation.
* 
* \param _ppMemory        Pointer to the memory allocated will be placed here
* \param _ulSizeInBytes   The number of bytes to allocate
* \param _Priority        The priority of the allocation
* \sa MP_FREE_MEMORY, MpAllocateMemory, NdisAllocateMemoryWithTag
*/
#define  MP_ALLOCATE_MEMORY_WITH_PRIORITY(_NdisHandle, _ppMemory, _ulSizeInBytes, _ulTag, _Priority)        \
    *_ppMemory = MpAllocateMemory(_NdisHandle, _ulSizeInBytes, _ulTag, _Priority, __FILE__, __LINE__, 0)


#define MP_ALLOCATE_MEMORY(_NdisHandle, _ppMemory, _ulSizeInBytes, _ulTag)       \
    MP_ALLOCATE_MEMORY_WITH_PRIORITY(_NdisHandle, _ppMemory, _ulSizeInBytes, _ulTag, NormalPoolPriority)

/**
* Use this macro to free memory previously allocated with MP_ALLOCATE_MEMORY. For debug
* versions of the driver, this macro expands to MpFreeMemory which checks for memory corruption,
* bad frees and other special memory operations. For retail builds, it directly macros to NDIS API for
* freeing memory.
* 
* \param _pMemory      Memory to free. Must have been allocated using MP_ALLOCATE_MEMORY
* \sa MP_ALLOCATE_MEMORY, MpFreeMemory, NdisFreeMemory
*/
#define MP_FREE_MEMORY(_pMemory)                                     \
    MpFreeMemory(_pMemory);

#define MP_DUMP_LEAKING_BLOCKS()        MpFreeAllocatedBlocks()

//
//    =========================
//         DEBUGGER BREAK/ASSERT APIS
//    =========================
//
#define MPASSERT(_Exp)                           ASSERT(_Exp)
#define MPASSERTMSG(_Msg, _Exp)                 ASSERTMSG(__FUNCTION__ ": " _Msg, _Exp)
#define MPVERIFY(_Exp)              \
    ASSERTMSG(__FUNCTION__ ": This is probably an issue with the miniport driver.\n", _Exp)


//
// These macros help verify that calls to a function is mutually exclusive
// We need these to verify that we are serializing calls to Hw11TransmitPacket
// as guaranteed. Since a lock is not protecting the region, this mechanism will
// help verify the behavior and catch potential synchronization issues
// Please note, these macros DO NOT guarantee to catch if mutual exclusion is
// violated but could potentially catch the issue if it occurs
//
typedef     LONG   MUTUAL_EXCLUSION_VERIFIER;
#define MP_ENTER_MUTEX_REGION(MutexVerifier)
#define MP_LEAVE_MUTEX_REGION(MutexVerifier)

/**
* This set of macros are used to print messages to the debugger
* depending on the current level of debug output. These macros
* are enabled only if tracing is disabled
*/
#if !DOT11_TRACE_ENABLED

    //
    // Tracing is off, so using Debug Prints instead.
    // These are slower but some people may still prefer
    // them to tracing thorugh initial debugging of code
    //

    EXTERN ULONG GlobalDebugLevel;
    EXTERN ULONG GlobalDebugComponents;

    //
    // Define the tracing levels
    //
    #define     DBG_OFF                 100 // Never used in a call to MpTrace
    #define     DBG_SEC                 11
    #define     DBG_SERIOUS             10
    #define     DBG_NORMAL              45
    #define     DBG_LOUD                90
    #define     DBG_FUNCTION            95  // MpEntry/Exit
    #define     DBG_TRACE               99  // Never Set GlobalDebugLevel to this

    //
    // Define the tracing components
    //
    #define COMP_MISC                   0x00000001
    #define COMP_INIT_PNP               0x00000002
    #define COMP_OID                    0x00000004
    #define COMP_RECV                   0x00000008
    #define COMP_SEND                   0x00000010
    #define COMP_PS_PACKETS             0x00000020
    #define COMP_EVENTS                 0x00000040
    #define COMP_POWER                  0x00000080
    #define COMP_SCAN                   0x00000100
    #define COMP_SEC                    0x00000200  // For Security
    #define COMP_DBG                    0x00000400
    #define COMP_TESTING                0x00000800
    #define COMP_ASSOC                  0x00001000
    
    #define MpTrace(_Comp, _Level, Fmt)                     \
        if((_Comp & GlobalDebugComponents) && (_Level <= GlobalDebugLevel))  \
            DbgPrint Fmt;

    #define MpEntry                  \
        if (DBG_TRACE <= GlobalDebugLevel)   \
            DbgPrint(("==> " __FUNCTION__ "\n"))

    #define MpExit                  \
        if (DBG_TRACE <= GlobalDebugLevel)   \
            DbgPrint(("<== " __FUNCTION__ "\n"))

    //
    // Function that reads DebugLevel and Components information from
    // registry
    //
    VOID MpReadGlobalDebugMask(
        NDIS_HANDLE  NdisMiniportDriverHandle
        );

#else   // !DOT11_TRACE_ENABLED

    //
    // When tracing is enabled MpEntry and MpExit have to be
    // turned off. Tracing cannot understand Preprocessor
    // directives.
    //

#define MpTrace(_Comp, _Level, Fmt)
#define MpEntry
#define MpExit

#endif  // !DOT11_TRACE_ENABLED

#define MP_VERIFY_PASSIVE_IRQL()      \
    MPASSERTMSG("Driver expected IRQL to be passive. Use !irql to check current Irql\n", (NDIS_CURRENT_IRQL() == 0))

#else     // DBG

#define MP_ALLOCATE_MEMORY_WITH_PRIORITY(_NdisHandle, _ppMemory, _ulSizeInBytes, _ulTag, _Priority)  \
    *_ppMemory = NdisAllocateMemoryWithTagPriority (                                \
     _NdisHandle,                                                 \
     _ulSizeInBytes,                                                              \
     _ulTag,                                                                       \
     _Priority                                                                     \
     )

#define  MP_ALLOCATE_MEMORY(_NdisHandle, _ppMemory, _ulSizeInBytes, _ulTag)                \
    MP_ALLOCATE_MEMORY_WITH_PRIORITY(_NdisHandle, _ppMemory, _ulSizeInBytes, _ulTag, NormalPoolPriority)


#define MP_FREE_MEMORY(_pMemory)                    \
    NdisFreeMemory(_pMemory, 0, 0)

#define MP_DUMP_LEAKING_BLOCKS()

#define MPASSERT(_Exp)
#define MPASSERTMSG(_Msg, _Exp)
#define MPVERIFY(_Exp)

#if !DOT11_TRACE_ENABLED

#define DBG_OFF
#define DBG_TRACE
#define DBG_SERIOUS
#define DBG_NORMAL
#define DBG_LOUD
#define MpTrace(_Comp, _Level, Fmt)

#endif // DOT11_TRACE_ENABLED

#define MpEntry
#define MpExit

#define MP_VERIFY_PASSIVE_IRQL()

#define MP_ENTER_MUTEX_REGION(MutexVerifier)
#define MP_LEAVE_MUTEX_REGION(MutexVerifier)

#endif   // DBG

#endif  // _NATIVE_WIFI__MP_DBG_

