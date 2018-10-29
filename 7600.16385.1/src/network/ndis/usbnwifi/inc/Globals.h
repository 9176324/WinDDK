/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Globals.h

Abstract:
    Global defines
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
#ifndef _NATIVE_WIFI_GLOBALS_H

#define _NATIVE_WIFI_GLOBALS_H

//
// Some macros to make code more readable
//


#ifndef INLINE
    #define INLINE      __inline
#endif


#ifndef EXTERN
    #define EXTERN      extern
#endif


#ifndef BOOL
typedef int BOOL;
#endif

#ifndef DWORD
typedef unsigned long       DWORD;
#endif

#ifndef MIN
    #define MIN(a, b)                  ((a) < (b)? a : b)
#endif  // MIN
#ifndef MAX
    #define MAX(a, b)                  ((a) > (b)? a : b)
#endif  // MAX
//
// Interlocked bit manipulation interfaces
//

#ifndef RtkInterlockedSetBits
#define RtkInterlockedSetBits(Flags, Flag) \
    InterlockedOr((PLONG)(Flags), Flag)
#endif

#ifndef RtkInterlockedAndBits 
#define RtkInterlockedAndBits(Flags, Flag) \
    InterlockedAnd((PLONG)(Flags), Flag)
#endif

#ifndef RtkInterlockedClearBits 
#define RtkInterlockedClearBits(Flags, Flag) \
    RtlInterlockedAndBits(Flags, ~(Flag))
#endif

#ifndef RtkInterlockedXorBits 
#define RtkInterlockedXorBits(Flags, Flag) \
    InterlockedXor(Flags, Flag)
#endif

#ifndef RtkInterlockedSetBitsDiscardReturn 
#define RtkInterlockedSetBitsDiscardReturn(Flags, Flag) \
    (VOID) RtlInterlockedSetBits(Flags, Flag)
#endif

#ifndef RtkInterlockedAndBitsDiscardReturn 
#define RtkInterlockedAndBitsDiscardReturn(Flags, Flag) \
    (VOID) RtlInterlockedAndBits(Flags, Flag)
#endif

#ifndef RtkInterlockedClearBitsDiscardReturn 
#define RtkInterlockedClearBitsDiscardReturn(Flags, Flag) \
    RtlInterlockedAndBitsDiscardReturn(Flags, ~(Flag))
#endif


#define NDIS_STATUS_TO_NT_STATUS(_NdisStatus, _pNtStatus)                           \
{                                                                                   \
    /*                                                                              \
     *  The following NDIS status codes map directly to NT status codes.            \
     */                                                                             \
    if (((NDIS_STATUS_SUCCESS == (_NdisStatus)) ||                                  \
        (NDIS_STATUS_PENDING == (_NdisStatus)) ||                                   \
        (NDIS_STATUS_BUFFER_OVERFLOW == (_NdisStatus)) ||                           \
        (NDIS_STATUS_FAILURE == (_NdisStatus)) ||                                   \
        (NDIS_STATUS_RESOURCES == (_NdisStatus)) ||                                 \
        (NDIS_STATUS_NOT_SUPPORTED == (_NdisStatus))))                              \
    {                                                                               \
        *(_pNtStatus) = (NTSTATUS)(_NdisStatus);                                    \
    }                                                                               \
    else if (NDIS_STATUS_BUFFER_TOO_SHORT == (_NdisStatus))                         \
    {                                                                               \
        /*                                                                          \
         *  The above NDIS status codes require a little special casing.            \
         */                                                                         \
        *(_pNtStatus) = STATUS_BUFFER_TOO_SMALL;                                    \
    }                                                                               \
    else if (NDIS_STATUS_INVALID_LENGTH == (_NdisStatus))                           \
    {                                                                               \
        *(_pNtStatus) = STATUS_INVALID_BUFFER_SIZE;                                 \
    }                                                                               \
    else if (NDIS_STATUS_INVALID_DATA == (_NdisStatus))                             \
    {                                                                               \
        *(_pNtStatus) = STATUS_INVALID_PARAMETER;                                   \
    }                                                                               \
    else if (NDIS_STATUS_ADAPTER_NOT_FOUND == (_NdisStatus))                        \
    {                                                                               \
        *(_pNtStatus) = STATUS_NO_MORE_ENTRIES;                                     \
    }                                                                               \
    else if (NDIS_STATUS_ADAPTER_NOT_READY == (_NdisStatus))                        \
    {                                                                               \
        *(_pNtStatus) = STATUS_DEVICE_NOT_READY;                                    \
    }                                                                               \
    else                                                                            \
    {                                                                               \
        *(_pNtStatus) = STATUS_UNSUCCESSFUL;                                        \
    }                                                                               \
}


#define NT_STATUS_TO_NDIS_STATUS(_NtStatus, _pNdisStatus)                           \
{                                                                                   \
    /*                                                                              \
     *  The following NDIS status codes map directly to NT status codes.            \
     */                                                                             \
    if (((STATUS_SUCCESS == (_NtStatus)) ||                                  \
        (STATUS_PENDING == (_NtStatus)) ||                                   \
        (STATUS_BUFFER_OVERFLOW == (_NtStatus)) ||                           \
        (STATUS_UNSUCCESSFUL == (_NtStatus)) ||                                   \
        (STATUS_INSUFFICIENT_RESOURCES == (_NtStatus)) ||                                 \
        (STATUS_NOT_SUPPORTED == (_NtStatus))))                              \
    {                                                                               \
        *(_pNdisStatus) = (NDIS_STATUS)(_NtStatus);                                    \
    }                                                                               \
    else if (STATUS_BUFFER_TOO_SMALL == (_NtStatus))                         \
    {                                                                               \
        /*                                                                          \
         *  The above NDIS status codes require a little special casing.            \
         */                                                                         \
        *(_pNdisStatus) = NDIS_STATUS_BUFFER_TOO_SHORT;                            \
    }                                                                               \
    else if (STATUS_INVALID_BUFFER_SIZE == (_NtStatus))                           \
    {                                                                               \
        *(_pNdisStatus) = NDIS_STATUS_INVALID_LENGTH;                                 \
    }                                                                               \
    else if (STATUS_INVALID_PARAMETER == (_NtStatus))                             \
    {                                                                               \
        *(_pNdisStatus) = NDIS_STATUS_INVALID_DATA;                                   \
    }                                                                               \
    else if (STATUS_NO_MORE_ENTRIES == (_NtStatus))                        \
    {                                                                               \
        *(_pNdisStatus) = NDIS_STATUS_ADAPTER_NOT_FOUND;                                     \
    }                                                                               \
    else if (STATUS_DEVICE_NOT_READY == (_NtStatus))                        \
    {                                                                               \
        *(_pNdisStatus) = NDIS_STATUS_ADAPTER_NOT_READY;                                    \
    }                                                                               \
    else                                                                            \
    {                                                                               \
        *(_pNdisStatus) = NDIS_STATUS_FAILURE;                                        \
    }                                                                               \
}


//-----------------------------------------------------------------------------
// 4127 -- Conditional Expression is Constant warning
//-----------------------------------------------------------------------------
#define WHILE(a) \
while(__pragma(warning(disable:4127)) a __pragma(warning(disable:4127)))


#endif  // _NATIVE_WIFI_GLOBALS_H

