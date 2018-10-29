/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    mp_intf.h

Abstract:
    Contains interfaces into the miniport layer
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    09-04-2007    Created

Notes:

--*/
#pragma once


// Maximum number of ports that can be created on this adapter (excludes the helper port)
#define MP_MAX_NUMBER_OF_PORT   2


//
// Forward declaration
//
typedef struct _ADAPTER         ADAPTER, *PADAPTER;
typedef struct _HVL             HVL, *PHVL;
typedef struct _HW              HW, *PHW;
typedef struct _MP_PORT         MP_PORT, *PMP_PORT;
typedef struct _VNIC            VNIC, *PVNIC;
typedef struct _MP_BSS_LIST     MP_BSS_LIST, *PMP_BSS_LIST;
typedef struct _MP_RW_LOCK_STATE    MP_RW_LOCK_STATE, *PMP_RW_LOCK_STATE;


NDIS_STATUS
Mp11CtxSStart(   
    __in  PADAPTER                Adapter
    );

VOID
Mp11CtxSComplete(   
    __in  PADAPTER                Adapter
    );

NDIS_STATUS 
Mp11PausePort(
    __in  PADAPTER                Adapter,
    __in  PMP_PORT                Port
    );

NDIS_STATUS 
Mp11RestartPort(
    __in  PADAPTER                Adapter,
    __in  PMP_PORT                Port
    );

NDIS_STATUS
Mp11Scan(
    __in  PADAPTER                Adapter,
    __in  PMP_PORT                Port,
    __in  PMP_SCAN_REQUEST        ScanRequest,
    __in  PORT11_GENERIC_CALLBACK_FUNC    CompletionHandler
    );

VOID
Mp11CancelScan(
    __in  PADAPTER                Adapter,
    __in  PMP_PORT                Port
    );

VOID
Mp11FlushBSSList(
    __in  PADAPTER                Adapter,
    __in  PMP_PORT                Port
    );

/** Returns a read-only reference to the BSS list. Individual entries in the list
 * can be modified */
PMP_BSS_LIST
Mp11QueryAndRefBSSList(
    __in  PADAPTER                Adapter,
    __in  PMP_PORT                Port,
    __out PMP_RW_LOCK_STATE       LockState
    );

/** Release read-only access to the BSS list */
VOID
Mp11ReleaseBSSListRef(
    __in  PADAPTER                Adapter,
    __in  PMP_BSS_LIST            BSSList,
    __in  PMP_RW_LOCK_STATE       LockState
    );

/** Complete an OID request */
VOID
Mp11CompleteOidRequest(
    __in  PADAPTER                Adapter,
    __in_opt  PMP_PORT                CompletingPort,
    __in  PNDIS_OID_REQUEST       NdisOidRequest,
    __in  NDIS_STATUS             CompletionStatus
    );


