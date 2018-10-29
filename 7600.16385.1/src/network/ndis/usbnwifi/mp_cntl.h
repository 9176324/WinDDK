/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    mpcontrol.h

Abstract:

    Constants and types to access the NDISPROT driver.
    Users must also include ntddndis.h

Environment:

    User/Kernel mode.

Revision History:

    
--*/

#ifndef __MPCONTROL__H
#define __MPCONTROL__H

//
// Simple Mutual Exclusion constructs used in preference to
// using KeXXX calls since we don't have Mutex calls in NDIS.
// These can only be called at passive IRQL.
//

typedef struct _NIC_MUTEX
{
    ULONG                   Counter;
    ULONG                   ModuleAndLine;  // useful for debugging

} NIC_MUTEX, *PNIC_MUTEX;

#define NIC_INIT_MUTEX(_pMutex)                                 \
{                                                               \
    (_pMutex)->Counter = 0;                                     \
    (_pMutex)->ModuleAndLine = 0;                               \
}

#define NIC_ACQUIRE_MUTEX(_pMutex)                              \
{                                                               \
    while (NdisInterlockedIncrement((PLONG)&((_pMutex)->Counter)) != 1)\
    {                                                           \
        NdisInterlockedDecrement((PLONG)&((_pMutex)->Counter));        \
        NdisMSleep(10000);                                      \
    }                                                           \
    (_pMutex)->ModuleAndLine = ('I' << 16) | __LINE__;\
}

#define NIC_RELEASE_MUTEX(_pMutex)                              \
{                                                               \
    (_pMutex)->ModuleAndLine = 0;                               \
    NdisInterlockedDecrement((PLONG)&(_pMutex)->Counter);              \
}

typedef struct _CONTROL_DEVICE_EXTENSION {
    PADAPTER    Adapter;
}   CONTROL_DEVICE_EXTENSION, *PCONTROL_DEVICE_EXTENSION ;

NDIS_STATUS
NICRegisterDevice(
    NDIS_HANDLE NdisMpHandle ,
    PADAPTER AdapterParam
    );

VOID
NICDeregisterDevice(
    PADAPTER AdapterParam
    );

DRIVER_DISPATCH NICDispatch;
 
#endif // __NUIOUSER__H


