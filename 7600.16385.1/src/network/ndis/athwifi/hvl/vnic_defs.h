/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    vnic_defs.h

Abstract:
    Contains VNIC specific defines
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    09-04-2007    Created

Notes:

--*/

#pragma once

typedef struct _HVL_CONTEXT HVL_CONTEXT, *PHVL_CONTEXT;

typedef enum _VNIC_CTXS_REF_TYPE
{
    REF_CTXS_BARRIER = 0,
    REF_JOIN,
    REF_PENDING_OP_WORKITEM,
    REF_SEND_PKTS,
    REF_CH_SW,
    REF_HW_OP,
    REF_START_BSS,
    REF_STOP_BSS,
    REF_DEF_KEY,
    REF_DEF_KEY_ID,
    REF_OPERATING_PHY_ID,
    REF_DESIRED_PHY_ID_LIST,
    REF_SCAN,
    REF_MAX
}VNIC_CTXS_REF_TYPE;

typedef struct _VNIC_COMPLETION_CTX
{
    PORT11_GENERIC_CALLBACK_FUNC CompletionFn;
} VNIC_COMPLETION_CTX, *PVNIC_COMPLETION_CTX;

typedef struct _VNIC
{
    /* 
        link for the next VNIC in the HVL context - this field is manipulated by the HVL - against
        the principles of data hiding but simplifies our code
        */
    LIST_ENTRY CtxLink;

    /*
        link for the next VNIC in the HVL's VNIC list. this field is manipulated by the HVL - against
        the principles of data hiding but simplifies our code 
        */
    LIST_ENTRY VNicLink;

    PHVL_CONTEXT pHvlCtx;
    
    /** Lock used to protect the VNIC data */
    NDIS_SPIN_LOCK              Lock;

    // The following boolean is used for debugging purposes and tracks whether the VNic is locked
    BOOLEAN                     fLocked;

    /** Refcount that blocks the VNIC from getting context switched out */
    LONG                        CtxtSRefCount;
    LONG                        CtxSRefCountTracker[REF_MAX];   

    /**
     * The handle by which NDIS recognizes this adapter. This handle needs to be passed
     * in for many of the calls made to NDIS
     */
    NDIS_HANDLE                 MiniportAdapterHandle;

    /**Is the VNIC terminating*/
    BOOLEAN fTerminating;

    /**Is the VNIC active right now */
    BOOLEAN fActive;

    /**Does the VNIC have exclusve access right now */
    BOOLEAN fExAccess;

    BOOLEAN fResetInProgress;
    
    MP_PORT_TYPE                PortType;
    NDIS_PORT_NUMBER            PortNumber;
    
    
    PVOID                       pvPort;

    PVOID                       pvHvl;

    PVOID                       pvHw;

    /**
     * This is MAC context in the HW layer that is specific to this VNIC. 
     */
    PVOID                       pvHwContext;

    /**
     *Queue for pending hardware operations. These operations will be processed once the 
     VNIC become active
     */
    LIST_ENTRY                  PendingOpQueue;

    /*
     * Data structure to queue NBL to be transmitted in. We will queue NBL
     * if we are not active or if the hardware cannot transmit right now (e.g. it is low on
      resources)
     */
    PKT_QUEUE                   TxQueue;

    /*
        Number of sends that have been submitted to the hardware but haven't yet been
        completed by it
        */
    LONG                       lOutstandingSends;
    
    /** handle for the work item that processes pending operations*/    
    NDIS_HANDLE                 PendingOpsWorkItemHandle;

    // flag that tells whether the pending operations work item is currently running
    BOOLEAN fPendingOpsRunning;
    NDIS_EVENT PendingOpWorkItemDoneEvent;

    BOOLEAN fVNicReadyToCtxS;

    /*
        The channel number that the VNIC prefers. This is useful in the following cases
        1. An AP might decide to move to the other channel in order to avoid context switches
        2. A station might decide to prefer an AP on the same channel over an AP  on some other 
          channel, possibly with a higher signal strength

        A value of zero means there is no preferred channel yet
          */
    ULONG ulPreferredChannel;
    PVNIC pPreferredChannelSrcVNic;

    /*
        pPnpOpExReq points to the pre-allocated exclusive access request structure used for 
        PnP related exclusive access operations. It needs to be pre-allocated because we cannot 
        fail exclusive access request for certain operations e.g. MiniportPause
        */
    PVNIC_EX_ACCESS_REQ  pPnpOpExReq;

    /*
        PnpPendingOp is the pending operation structure used for PnP related exclusive access 
        operations. The pvOpData field corresponds to pPnpOpExReq
        */
    PENDING_OP           PnpPendingOp;

    BOOLEAN LastLinkStateNotified;   // TRUE = MediaConnected, FALSE = Disconnected
} VNIC, *PVNIC;

#define VNIC_PORT_NO (pVNic->PortNumber)

// VNIC lock manipulation macros

#ifndef HIDE_ANNOTATION
__drv_maxIRQL(DISPATCH_LEVEL)
__drv_setsIRQL(DISPATCH_LEVEL)
__drv_at((pVNic->Lock).OldIrql, __drv_savesIRQL)
__drv_at((pVNic->Lock).SpinLock, __maybenull __drv_acquiresExclusiveResource(KeSpinLockType))
#endif
VOID
VNicLock(
    __in  PVNIC      pVNic
    );

VOID
VNicLockAtDispatch(
    __in  PVNIC      pVNic,
    __in  BOOLEAN    fDispatch
    );

#ifndef HIDE_ANNOTATION
__drv_maxIRQL(DISPATCH_LEVEL)
__drv_minIRQL(DISPATCH_LEVEL)
__drv_at((pVNic->Lock).OldIrql, __drv_restoresIRQL)
__drv_at((pVNic->Lock).SpinLock, __maybenull __drv_releasesExclusiveResource(KeSpinLockType))
#endif
VOID
VNicUnlock(
    __in  PVNIC      pVNic
    );

VOID
VNicUnlockAtDispatch(
    __in  PVNIC      pVNic,
    __in  BOOLEAN    fDispatch
    );

BOOLEAN
VNicIsLocked(    
    __in  PVNIC      PVNIC
    );
    
VOID
VNicAddCtxSRef(
    __in  PVNIC      pVNic,
    __in  VNIC_CTXS_REF_TYPE RefType
    );

VOID
VNicRemoveCtxSRef(
    __in  PVNIC      pVNic,
    __in  VNIC_CTXS_REF_TYPE RefType
    );

VOID
VNicIncCtxSRef(
    __in  PVNIC      pVNic,
    __in  ULONG      ulNumRef,
    __in  VNIC_CTXS_REF_TYPE RefType
    );

VOID
VNicDecCtxSRef(
    __in  PVNIC      pVNic,
    __in  ULONG      ulNumRef,
    __in  VNIC_CTXS_REF_TYPE RefType
    );

BOOLEAN
VNicIsActive(    
    __in  PVNIC      pVNic
    );

NDIS_STATUS
VNicPreHwSyncCallActions(PVNIC pVNic, BOOLEAN *pfProgramHw);

VOID
VNicPostHwSyncCallActions(PVNIC pVNic, BOOLEAN fProgramHw);

NDIS_STATUS 
VNicCanProcessReqNow(PVNIC pVNic, BOOLEAN *pfProcessNow);

BOOLEAN
VNicIsInReset(
    __in PVNIC pVNic
    );

#ifndef HIDE_ANNOTATION
__drv_mustHold(KeSpinLockType)
__drv_requiresIRQL(DISPATCH_LEVEL)
#endif
NDIS_STATUS
VNicSwChHelper(
    __in  PVNIC                   pVNic,
    __in  ULONG                   ulChannel,
    __in  ULONG                   ulPhyId,
    __in  BOOLEAN                 fSwitchPhy,
    __in  PVNIC_COMPLETION_CTX    pCtx
    );

NDIS_STATUS 
VNicReleaseExAccess(
    PVNIC pVNic
    );

#ifndef HIDE_ANNOTATION
__drv_mustHold(KeSpinLockType)
__drv_requiresIRQL(DISPATCH_LEVEL)
#endif
NDIS_STATUS
VNicSetDefaultKeyHelper(
    __in  PVNIC                   pVNic,
    __in_bcount(DOT11_ADDRESS_SIZE)  DOT11_MAC_ADDRESS       MacAddr,
    __in  ULONG                   KeyID,
    __in  BOOLEAN                 Persistent,
    __in  DOT11_CIPHER_ALGORITHM  AlgoId,
    __in  ULONG                   KeyLength,
    __in_bcount(KeyLength)  PUCHAR                  KeyValue    
    );

#ifndef HIDE_ANNOTATION
__drv_mustHold(KeSpinLockType)
__drv_requiresIRQL(DISPATCH_LEVEL)
#endif
NDIS_STATUS
VNicSetOperatingPhyIdHelper(
    __in  PVNIC                   pVNic,
    __in  ULONG                   PhyId
    );

#ifndef HIDE_ANNOTATION
__drv_mustHold(KeSpinLockType)
__drv_requiresIRQL(DISPATCH_LEVEL)
#endif
NDIS_STATUS
VNicCompleteExclusiveAccessOp(
    PVNIC pVNic,
    PENDING_OP_TYPE opType,
    PVOID pvOpData,
    BOOLEAN fCompleteReq
    );

#ifndef HIDE_ANNOTATION
__drv_mustHold(KeSpinLockType)
__drv_requiresIRQL(DISPATCH_LEVEL)
#endif
NDIS_STATUS 
VNicChSwComplete(
    __in  PVNIC                   pVNic,
    __in  BOOLEAN                 fReferenced,
    __in  PORT11_GENERIC_CALLBACK_FUNC ChSwCompleteHandler,
    __in  PVOID                   Data
    );

BOOLEAN
VNicHasExAccess(    
    __in  PVNIC      pVNic
    );

#ifndef HIDE_ANNOTATION
__drv_mustHold(KeSpinLockType)
__drv_requiresIRQL(DISPATCH_LEVEL)
#endif
NDIS_STATUS
VNicSetDesiredPhyIdListHelper(
    __in  PVNIC                   pVNic,
    __in  PULONG                  PhyIDList,
    __in  ULONG                   PhyIDCount
    );

VOID
VNicSetReadyToCtxS(
    PVNIC pVNic,
    BOOLEAN fReadyToCtxS
    );

BOOLEAN
VNicIsPreallocatedRequest(
    PVNIC pVNic,
    PVNIC_EX_ACCESS_REQ pExReq
    );

BOOLEAN
VNicIsPreallocatedOperation(
    PVNIC pVNic,
    PPENDING_OP pPendingOp
    );

VOID
VNicSendChannelNotification(
    PVNIC pVNic,
    ULONG ulChannel
    );

VOID
VNicSendLinkStateNotification(
    IN  PVNIC                   pVNic,
    IN  BOOLEAN                 MediaConnected
    );


