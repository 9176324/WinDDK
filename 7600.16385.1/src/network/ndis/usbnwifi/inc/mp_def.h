/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Mp_Defines.h

Abstract:
    Hardware defines
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
#ifndef _MP_802_11_COMMON_DEFINES_
#define _MP_802_11_COMMON_DEFINES_

/** Defines Major version for the OID_DOT11_OPERATION_MODE_CAPABILITY */
#define MP_OPERATION_MODE_CAPABILITY_MAJOR_VERSION              2

/** Defines Minor version for the OID_DOT11_OPERATION_MODE_CAPABILITY */
#define MP_OPERATION_MODE_CAPABILITY_MINOR_VERSION              0


typedef struct _MP_TIM_ELEMENT {
    DOT11_INFO_ELEMENT;
    UCHAR       DTIMCount;
    UCHAR       DTIMPeriod;
    UCHAR       BitmapControl;
    UCHAR       VirtualBitmap[251];
} MP_TIM_ELEMENT, *PMP_TIM_ELEMENT;

/** The Max Frame size does not include the FCS */
#define MP_802_11_MAX_FRAME_SIZE                (DOT11_MAX_PDU_SIZE - 4)
#define MP_802_11_MIN_FRAME_SIZE                DOT11_MIN_PDU_SIZE
#define MP_802_11_SHORT_HEADER_SIZE             DOT11_DATA_SHORT_HEADER_SIZE
#define MP_802_11_LONG_HEADER_SIZE              DOT11_DATA_LONG_HEADER_SIZE
#define MP_802_11_MAX_FRAGMENTS_PER_MSDU        DOT11_MAX_NUM_OF_FRAGMENTS
#define MP_802_11_MAXIMUM_LOOKAHEAD                                             \
                (DOT11_MIN_PDU_SIZE - (                                     \
                            sizeof(DOT11_DATA_SHORT_HEADER) + 12))

/**
 * Tracks the type of reset
 */
typedef enum _MP_RESET_TYPE {
    MP_RESET_TYPE_DOT11_PHY = dot11_reset_type_phy,
    MP_RESET_TYPE_DOT11_MAC = dot11_reset_type_mac,
    MP_RESET_TYPE_DOT11_PHY_MAC = dot11_reset_type_phy_and_mac,
    MP_RESET_TYPE_NDIS_RESET = 0x1000,
    MP_RESET_TYPE_INTERNAL_RESET = 0x2000
} MP_RESET_TYPE, *PMP_RESET_TYPE;

/**
 * Common header structure for management packets and short data packets
 */
typedef struct DOT11_DATA_SHORT_HEADER MP_DOT11_MGMT_DATA_MAC_HEADER, *PMP_DOT11_MGMT_DATA_MAC_HEADER;


#define MP_SET_FLAG(_M, _F)         ((_M)->Flags |= (_F))   
#define MP_CLEAR_FLAG(_M, _F)       ((_M)->Flags &= ~(_F))
#define MP_CLEAR_ALL_FLAGS(_M)      ((_M)->Flags = 0)
#define MP_TEST_FLAG(_M, _F)        (((_M)->Flags & (_F)) != 0)
#define MP_TEST_FLAGS(_M, _F)       (((_M)->Flags & (_F)) == (_F))


#define MP_ADDRESS_IS_MULTICAST(_Dot11MacAddress)       (_Dot11MacAddress[0] & 0x1)

/** This macro converts the time provided in TU units to Milliseconds */
#define MP_TU_TO_MS(_TimeInTU_)     ((_TimeInTU_ * 1024) / 1000)


//
// BAIL Macros.
//
#define BAIL_ON_NDIS_ERROR(ndisStatus)          \
    if (ndisStatus != NDIS_STATUS_SUCCESS) {    \
        goto error;                             \
    }

//
// MACROS to make code more readable
//
#define MP_ACQUIRE_SPIN_LOCK(_Lock, _bDispatchLevel)        \
{                                                           \
    if (_bDispatchLevel)            \
    {                                   \
        NdisDprAcquireSpinLock(_Lock);      \
    }                                       \
    else                                    \
    {                                        \
        NdisAcquireSpinLock(_Lock);         \
    }                                       \
}

#define MP_RELEASE_SPIN_LOCK(_Lock, _bDispatchLevel)        \
{                                                           \
    if (_bDispatchLevel)            \
    {                                   \
        NdisDprReleaseSpinLock(_Lock);      \
    }                                       \
    else                                    \
    {                                        \
        NdisReleaseSpinLock(_Lock);         \
    }                                       \
}

/** Returns success of the MAC addresses are equal, else failure */
#define MP_COMPARE_MAC_ADDRESS(_MacAddr1, _MacAddr2)    \
    (RtlCompareMemory(_MacAddr1, _MacAddr2, sizeof(DOT11_MAC_ADDRESS)) == sizeof(DOT11_MAC_ADDRESS))

#define MP_ACQUIRE_READ_LOCK(pLock, pLockState)     \
    NdisAcquireReadWriteLock(pLock, FALSE, pLockState)

#define MP_ACQUIRE_WRITE_LOCK(pLock, pLockState)    \
    NdisAcquireReadWriteLock(pLock, TRUE, pLockState)
                
#define MP_RELEASE_READ_LOCK(pLock, pLockState)     \
    NdisReleaseReadWriteLock(pLock, pLockState)
                
#define MP_RELEASE_WRITE_LOCK(pLock, pLockState)    \
    NdisReleaseReadWriteLock(pLock, pLockState)

//
//  Handy macros for doing pointer arithmetic
//
#ifndef Add2Ptr
#define Add2Ptr(P,I) ((PVOID)((PUCHAR)(P) + (I)))
#endif

#ifndef PtrOffset
#define PtrOffset(B,O) ((ULONG)((ULONG_PTR)(O) - (ULONG_PTR)(B)))
#endif

//
// Unicast address
//

#define ETH_IS_UNICAST(Address) (!ETH_IS_MULTICAST(Address) && !ETH_IS_BROADCAST(Address))

//
// Macros for assigning and verifying NDIS_OBJECT_HEADER
//
#define MP_ASSIGN_NDIS_OBJECT_HEADER(_header, _type, _revision, _size) \
    (_header).Type = _type; \
    (_header).Revision = _revision; \
    (_header).Size = _size; 

#define MP_VERIFY_NDIS_OBJECT_HEADER(_header, _type, _revision, _size) \
    (((_header).Type == _type) && \
     ((_header).Revision == _revision) && \
     ((_header).Size == _size))

// 
// With NDIS 6.0 header versioning, the driver should allow higher versions
// of structures to be set. This macro verifies that for sets the version is atleast
// the expected one and size is greater or equal to required
//
#define MP_VERIFY_NDIS_OBJECT_HEADER_DEFAULT(_header, _type, _revision, _size) \
    (((_header).Type == _type) && \
     ((_header).Revision == _revision) && \
     ((_header).Size == _size))


#endif // _MP_802_11_COMMON_DEFINES_

