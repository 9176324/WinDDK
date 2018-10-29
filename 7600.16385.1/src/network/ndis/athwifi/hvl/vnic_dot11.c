#include "precomp.h"

#if DOT11_TRACE_ENABLED
#include "vnic_dot11.tmh"
#endif

PSTR
VNicGetStatusTypeString(
    __in  ULONG StatusType
    )
{
    PSTR pszString = NULL;
    
    switch (StatusType)
    {
        case NDIS_STATUS_DOT11_SCAN_CONFIRM:
            pszString = "Scan Confirm";
            break;
        case NDIS_STATUS_DOT11_MPDU_MAX_LENGTH_CHANGED:
            pszString = "MPDU max length changed";
            break;
        case NDIS_STATUS_DOT11_ASSOCIATION_START:
            pszString = "Association start";
            break;
        case NDIS_STATUS_DOT11_ASSOCIATION_COMPLETION:
            pszString = "Association complete";
            break;
        case NDIS_STATUS_DOT11_CONNECTION_START:
            pszString = "connection start";
            break;
        case NDIS_STATUS_DOT11_CONNECTION_COMPLETION:
            pszString = "connection complete";
            break;
        case NDIS_STATUS_DOT11_ROAMING_START:
            pszString = "roaming start";
            break;
        case NDIS_STATUS_DOT11_ROAMING_COMPLETION:
            pszString = "roaming complete";
            break;
        case NDIS_STATUS_DOT11_DISASSOCIATION:
            pszString = "disassociation";
            break;
        case NDIS_STATUS_DOT11_TKIPMIC_FAILURE:
            pszString = "TKIP MIC failure";
            break;
        case NDIS_STATUS_DOT11_PMKID_CANDIDATE_LIST:
            pszString = "PMKID candidate list";
            break;
        case NDIS_STATUS_DOT11_PHY_STATE_CHANGED:
            pszString = "Phy state changed";
            break;
        case NDIS_STATUS_DOT11_LINK_QUALITY:
            pszString = "link quality";
            break;
        case NDIS_STATUS_DOT11_INCOMING_ASSOC_STARTED:
            pszString = "incoming association started";
            break;
        case NDIS_STATUS_DOT11_INCOMING_ASSOC_REQUEST_RECEIVED:
            pszString = "incoming association request received";
            break;
        case NDIS_STATUS_DOT11_INCOMING_ASSOC_COMPLETION:
            pszString = "incoming association completed";
            break;
        case NDIS_STATUS_DOT11_STOP_AP:
            pszString = "Stop AP";
            break;
        case NDIS_STATUS_DOT11_PHY_FREQUENCY_ADOPTED:
            pszString = "Phy frequency adopted";
            break;
    };

    return pszString;
}

#ifndef HIDE_ANNOTATION
__drv_mustHold(KeSpinLockType)
__drv_requiresIRQL(DISPATCH_LEVEL)
#endif
NDIS_STATUS
VNicStopBSSHelper(
    __in  PVNIC                   pVNic
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    ASSERT(VNicIsLocked(pVNic));
    ASSERT(VNicIsActive(pVNic) && !VNicIsInReset(pVNic));

    do
    {                
        /*
            We call the hardware for starting the BSS. We need to give up our lock before
            calling the hardware. The context switch ref count will be decremented when we
            have completed the call
            */
        // add the context switch ref count 
        VNicAddCtxSRef(pVNic, REF_STOP_BSS);
        
        VNicUnlock(pVNic);        
        Hw11StopBSS(pVNic->pvHwContext);
        VNicLock(pVNic);

        MpTrace(COMP_HVL, DBG_NORMAL, ("VNic(%d): Called the hardware for stopping a BSS 0x%x\n", VNIC_PORT_NO, ndisStatus));
        
        VNicRemoveCtxSRef(pVNic, REF_STOP_BSS);
    } while (FALSE);

    return ndisStatus;
}

NDIS_STATUS
VNic11StopBSS(
    __in  PVNIC                   pVNic,
    __in  PORT11_GENERIC_CALLBACK_FUNC CompletionHandler
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN fLocked = FALSE;
    BOOLEAN fProcessReqNow = FALSE;
    
    do
    {
        VNicLock(pVNic);
        fLocked = TRUE;

        ndisStatus = VNicCanProcessReqNow(pVNic, &fProcessReqNow);
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_NORMAL, ("VNic(%d): VNicCanProcessReqNow failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }
        
        if (fProcessReqNow)
        {
            ndisStatus = VNicStopBSSHelper(pVNic);
            MpTrace(COMP_HVL, DBG_NORMAL, ("VNic(%d): VNicStopBSSHelper returned %!x!\n", VNIC_PORT_NO, ndisStatus));
        }
        else
        {
            /*
                We are not currently active or there are pending operations. 
                Queue the Start BSS request internally
                */
            ndisStatus = VNicQueueStopBSSRequest(
                        pVNic,
                        CompletionHandler
                        );
            if (NDIS_STATUS_SUCCESS == ndisStatus)
            {
                MpTrace(COMP_HVL, DBG_NORMAL, ("VNic(%d): Queued the stop BSS request\n", VNIC_PORT_NO));
                // Return pending since the request is not going to be completed synchrnously
                ndisStatus = NDIS_STATUS_PENDING;
            }
            else
            {
                MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): VNicQueueStopBSSRequest failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            }
        }
    } while (FALSE);
    
    if (fLocked)
    {
        VNicUnlock(pVNic);
    }
    
    return ndisStatus;
}

#ifndef HIDE_ANNOTATION
__drv_mustHold(KeSpinLockType)
__drv_requiresIRQL(DISPATCH_LEVEL)
#endif
NDIS_STATUS
VNicStartBSSHelper(
    __in  PVNIC                   pVNic,
    __in  PMP_BSS_DESCRIPTION     BSSDescription
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    ASSERT(VNicIsLocked(pVNic));
    ASSERT(VNicIsActive(pVNic) && !VNicIsInReset(pVNic));

    do
    {
        /*
            We can call the hardware for starting the BSS. We need to give up our lock before
            calling the hardware. The context switch ref count will be decremented when we
            have completed the call
            */

        // add the context switch ref count 
        VNicAddCtxSRef(pVNic, REF_START_BSS);
        
        VNicUnlock(pVNic);        
        ndisStatus = Hw11StartBSS(pVNic->pvHwContext, BSSDescription);
        VNicLock(pVNic);

        MpTrace(COMP_HVL, DBG_NORMAL, ("VNic(%d): Called the hardware for starting a BSS 0x%x\n", VNIC_PORT_NO, ndisStatus));
        
        VNicRemoveCtxSRef(pVNic, REF_START_BSS);
    } while (FALSE);

    return ndisStatus;
}

NDIS_STATUS
VNic11StartBSS(
    __in  PVNIC                   pVNic,
    __in  PMP_BSS_DESCRIPTION     BSSDescription,
    __in  PORT11_GENERIC_CALLBACK_FUNC CompletionHandler
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN fLocked = FALSE;
    BOOLEAN fProcessReqNow = FALSE;
    
    do
    {
        VNicLock(pVNic);
        fLocked = TRUE;

        ndisStatus = VNicCanProcessReqNow(pVNic, &fProcessReqNow);
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_NORMAL, ("VNic(%d): VNicCanProcessReqNow failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }
        
        if (fProcessReqNow)
        {
            ndisStatus = VNicStartBSSHelper(pVNic, BSSDescription);
            MpTrace(COMP_HVL, DBG_NORMAL, ("VNic(%d): VNicStartBSSHelper returned %!x!\n", VNIC_PORT_NO, ndisStatus));
        }
        else
        {
            /*
                We are not currently active or there are pending operations. 
                Queue the Start BSS request internally
                */
            ndisStatus = VNicQueueStartBSSRequest(
                        pVNic, 
                        BSSDescription,
                        CompletionHandler
                        );
            if (NDIS_STATUS_SUCCESS == ndisStatus)
            {
                MpTrace(COMP_HVL, DBG_NORMAL, ("VNic(%d): Queued the start BSS request\n", VNIC_PORT_NO));
                // Return pending since the request is not going to be completed synchrnously
                ndisStatus = NDIS_STATUS_PENDING;
            }
            else
            {
                MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): VNicQueueStartBSSRequest failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            }
        }
    } while (FALSE);
    
    if (fLocked)
    {
        VNicUnlock(pVNic);
    }
    
    return ndisStatus;
 }

#ifndef HIDE_ANNOTATION
__drv_mustHold(KeSpinLockType)
__drv_requiresIRQL(DISPATCH_LEVEL)
#endif
NDIS_STATUS 
VNicJoinComplete(
    __in PVNIC pVNic,
    __in BOOLEAN fReferenced,
    __in PORT11_GENERIC_CALLBACK_FUNC JoinCompleteHandler,
    __in PVOID Data
    )
{
    PVOID pvPort = NULL;
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    MpTrace(COMP_HVL, DBG_NORMAL, ("VNic(%d): Completing the Join operation \n", VNIC_PORT_NO));

    pvPort = pVNic->pvPort;

    // remove the ref count that was added for the join operation
    if (fReferenced)
    {
        VNicRemoveCtxSRef(pVNic, REF_JOIN);
    }

    // give up lock before calling into the port
    VNicUnlock(pVNic);

    // Call the completion handler in the port
    if (JoinCompleteHandler)
    {
        ndisStatus = JoinCompleteHandler(pvPort, Data);
    }

    VNicLock(pVNic);

    return ndisStatus;
}

NDIS_STATUS 
VNic11JoinCompleteCallback(
    __in  PVNIC                   pVNic,
    __in  PVOID                   pvCtx,
    __in  PVOID                   Data
    )
{
    PVNIC_COMPLETION_CTX pCtx = NULL;
    PORT11_GENERIC_CALLBACK_FUNC JoinCompleteHandler= NULL;
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
    MpTrace(COMP_HVL, DBG_NORMAL, ("VNic(%d): Hw completed Join operation \n", VNIC_PORT_NO));

    pCtx = (PVNIC_COMPLETION_CTX)pvCtx;
    ASSERT(pCtx);
    
    JoinCompleteHandler = pCtx->CompletionFn;

    FREE_MEM(pCtx);

    VNicLock(pVNic);
    ndisStatus = VNicJoinComplete(pVNic, TRUE, JoinCompleteHandler, Data);    
    VNicUnlock(pVNic);

    return ndisStatus;    
}

#ifndef HIDE_ANNOTATION
__drv_mustHold(KeSpinLockType)
__drv_requiresIRQL(DISPATCH_LEVEL)
#endif
NDIS_STATUS
VNicJoinBSSHelper(
    __in  PVNIC                   pVNic,
    __in  PMP_BSS_DESCRIPTION     BSSDescription,
    __in  ULONG                   JoinFailureTimeout,
    __in  PVNIC_COMPLETION_CTX    pCtx
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN fReferenced = FALSE;

    ASSERT(VNicIsLocked(pVNic));
    ASSERT(VNicIsActive(pVNic) && !VNicIsInReset(pVNic));

    do
    {
        /*
            We can call the hardware for a join. We need to give up our lock before
            calling the hardware. The context switch ref count will be decremented when we
            have completed the join
            */

        // add the context switch ref count for the async join call to the hardware
        VNicAddCtxSRef(pVNic, REF_JOIN);
        fReferenced = TRUE;
        
        VNicUnlock(pVNic);
        
        ndisStatus = Hw11JoinBSS(
                        pVNic->pvHwContext, 
                        BSSDescription, 
                        JoinFailureTimeout, 
                        VNic11JoinCompleteCallback,
                        pCtx
                        );

        MpTrace(COMP_HVL, DBG_NORMAL, ("VNic(%d): Called the hardware for the Join operation 0x%x\n", VNIC_PORT_NO, ndisStatus));
        
        VNicLock(pVNic);

    } while (FALSE);

    if (NDIS_STATUS_PENDING != ndisStatus)
    {
        // the call to the hardware completed. Remove the join ref count we added
        if (fReferenced)
        {
            VNicRemoveCtxSRef(pVNic, REF_JOIN);
        }
    }    
       
    return ndisStatus;
}

NDIS_STATUS
VNic11JoinBSS(
    __in  PVNIC                   pVNic,
    __in  PMP_BSS_DESCRIPTION     BSSDescription,
    __in  ULONG                   JoinFailureTimeout,
    __in  PORT11_GENERIC_CALLBACK_FUNC CompletionHandler
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN fLocked = FALSE;
    BOOLEAN fProcessReqNow = FALSE;
    PVNIC_COMPLETION_CTX pCtx = NULL;
    
    do
    {
        VNicLock(pVNic);
        fLocked = TRUE;

        ndisStatus = VNicCanProcessReqNow(pVNic, &fProcessReqNow);
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_NORMAL, ("VNic(%d): VNicCanProcessReqNow failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }
        
        if (fProcessReqNow)
        {
            ndisStatus = ALLOC_MEM(pVNic->MiniportAdapterHandle, sizeof(VNIC_COMPLETION_CTX), &pCtx);
            if (NDIS_STATUS_SUCCESS != ndisStatus)
            {
                MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): Failed to allocate memory for a new completion context \n", VNIC_PORT_NO));

                /*
                    Do not call the station's callback function since the station doesn't 
                    expect to be called back if there is a failure                    
                    */
                break;
            }

            pCtx->CompletionFn = CompletionHandler;
            
            ndisStatus = VNicJoinBSSHelper(pVNic, BSSDescription, JoinFailureTimeout, pCtx);

            if (NDIS_STATUS_PENDING != ndisStatus)
            {
                MpTrace(COMP_HVL, DBG_NORMAL, ("VNic(%d): VNicJoinBSSHelper completed synchronously %!x!\n", VNIC_PORT_NO, ndisStatus));
                
                /*
                    The call to the hardware completed synchronously. Do not call the station's 
                    callback function.
                    */
                FREE_MEM(pCtx);
                break;
            }    
        }
        else
        {
            /*
                We are not currently active or there are pending operations. Queue the Join request 
                internally
                */
            ndisStatus = VNicQueueJoinRequest(
                        pVNic, 
                        BSSDescription, 
                        JoinFailureTimeout, 
                        CompletionHandler
                        );
            if (NDIS_STATUS_SUCCESS == ndisStatus)
            {
                MpTrace(COMP_HVL, DBG_NORMAL, ("VNic(%d): Queued the join request \n", VNIC_PORT_NO));
                // Return pending since the request is not going to be completed synchrnously
                ndisStatus = NDIS_STATUS_PENDING;
            }
            else
            {
                MpTrace(COMP_HVL, DBG_NORMAL, ("VNic(%d): VNicQueueJoinRequest failed %!x!\n", VNIC_PORT_NO, ndisStatus));
            }
        }
    } while (FALSE);
    
    if (fLocked)
    {
        VNicUnlock(pVNic);
    }
    
    return ndisStatus;
}

VOID
VNic11NotifyConnectionStatus(
    __in  PVNIC                   pVNic,
    __in  BOOLEAN                 Connected,
    __in  ULONG                   StatusType,
    __in_bcount_opt(StatusBufferSize)  PVOID                   StatusBuffer,
    __in  ULONG                   StatusBufferSize
    )
{
    BOOLEAN fProcessReqNow = FALSE;
    //BOOLEAN fPendingConnStartDeleted = FALSE;

    VNicLock(pVNic);

    MpTrace(COMP_HVL, DBG_NORMAL, ("VNic(%d): VNic11NotifyConnectionStatus called with status type %s \n", VNIC_PORT_NO, VNicGetStatusTypeString(StatusType)));

    fProcessReqNow = VNicIsActive(pVNic) && VNicIsPendingOpQueueEmpty(pVNic);
    
    if ((NDIS_STATUS_DOT11_CONNECTION_START == StatusType) ||
        (NDIS_STATUS_DOT11_ROAMING_START == StatusType))
    {
        VNicSetReadyToCtxS(pVNic, TRUE);        
    }
    else if ((NDIS_STATUS_DOT11_CONNECTION_COMPLETION == StatusType) ||
        (NDIS_STATUS_DOT11_ROAMING_COMPLETION == StatusType))
    {
        /*
            The connection attempt has completed. We need to do the following
            1. Remove the context switch reference that was added for this connection attempt
            2. If there is a pending Join request, we should delete that
            */
        VNicDeletePendingJoinRequest(pVNic);
    }        

    Hw11NotifyEventConnectionState(
        pVNic->pvHwContext, 
        Connected, 
        StatusType,
        StatusBuffer,
        StatusBufferSize,
        fProcessReqNow
        );

    VNicUnlock(pVNic);

    /*
        Notify only after leaving the lock
        */
    if (Connected && 
        (NDIS_STATUS_DOT11_CONNECTION_COMPLETION == StatusType ||
        NDIS_STATUS_DOT11_ROAMING_COMPLETION == StatusType))
    {
        VNicSendChannelNotification(pVNic, Hw11QueryCurrentChannel(pVNic->pvHwContext, FALSE));
    }

    VNicSendLinkStateNotification(pVNic, Connected);
}

NDIS_STATUS 
VNic11ScanCompleteCallback(
    __in  PVNIC                   pVNic,
    __in  PVOID                   pvCtx,
    __in  PVOID                   Data
    )
{
    PVNIC_COMPLETION_CTX pCtx = (PVNIC_COMPLETION_CTX)pvCtx;
    PORT11_GENERIC_CALLBACK_FUNC scanCompleteHandler = pCtx->CompletionFn;
    PVOID pvPort = NULL;
    
    MpTrace(COMP_HVL, DBG_NORMAL, ("VNic(%d): Hw completed scan operation \n", VNIC_PORT_NO));

    pvPort = pVNic->pvPort;
    
    FREE_MEM(pvCtx);

    VNicLock(pVNic);
    VNicRemoveCtxSRef(pVNic, REF_SCAN);
    VNicUnlock(pVNic);
    
    // Call the completion handler in the port
    if (scanCompleteHandler)
    {
        return scanCompleteHandler(pvPort, Data);
    }
    else
    {
        return NDIS_STATUS_SUCCESS;
    }
}

NDIS_STATUS
VNic11StartScan(
    __in  PVNIC                   pVNic,
    __in  PMP_SCAN_REQUEST        ScanRequest,
    __in  PORT11_GENERIC_CALLBACK_FUNC CompletionHandler
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PVNIC_COMPLETION_CTX pCtx = NULL;
    BOOLEAN fReferenced = FALSE, fLocked = FALSE;
    
    VNicLock(pVNic);
    fLocked = TRUE;
    
    ASSERT(pVNic->PortType == HELPER_PORT);
    ASSERT(VNicHasExAccess(pVNic));
    
    do
    {    
        ndisStatus = ALLOC_MEM(pVNic->MiniportAdapterHandle, sizeof(VNIC_COMPLETION_CTX), &pCtx);
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): Failed to allocate memory for a new completion context \n", VNIC_PORT_NO));
            break;
        }
        
        pCtx->CompletionFn = CompletionHandler;
        
        /*
            We can call the hardware for a scan. We need to give up our lock before
            calling the hardware. The context switch ref count will be decremented when we
            have completed the scan
            */
        VNicAddCtxSRef(pVNic, REF_SCAN);
        fReferenced = TRUE;
        
        VNicUnlock(pVNic);
        fLocked = FALSE;
        
        ndisStatus = Hw11StartScan(
                        pVNic->pvHwContext, 
                        ScanRequest, 
                        VNic11ScanCompleteCallback,
                        pCtx
                        );
        MpTrace(COMP_HVL, DBG_NORMAL, ("VNic(%d): Called the hardware for the Scan operation 0x%x\n", VNIC_PORT_NO, ndisStatus));        

        VNicLock(pVNic);
        fLocked = TRUE;
    } while (FALSE);

    if (NDIS_STATUS_PENDING != ndisStatus)
    {
        MpTrace(COMP_HVL, DBG_NORMAL, ("VNic(%d): Hw11StartScan completed synchronously %!x!\n", VNIC_PORT_NO, ndisStatus));
        
        /*
            The call to the hardware completed synchronously. Do not call the station's 
            callback function.
            */
        if (pCtx)
        {
            FREE_MEM(pCtx);
        }

        // remove the reference
        if (fReferenced)
        {
            VNicRemoveCtxSRef(pVNic, REF_SCAN);
        }
    }

    if (fLocked)
    {
        VNicUnlock(pVNic);
    }
    
    return ndisStatus;
}

VOID
VNic11CancelScan(
    __in  PVNIC                   pVNic
    )
{
    MpTrace(COMP_HVL, DBG_NORMAL, ("VNic(%d): VNic11CancelScan called \n", VNIC_PORT_NO));
    Hw11CancelScan(pVNic->pvHwContext);
}

VOID
VNic11IndicateStatus(
    __in  PVNIC                   VNic,
    __in  NDIS_STATUS             StatusCode,
    __in  PVOID                   StatusBuffer,
    __in  ULONG                   StatusBufferSize
    )
{
    Port11IndicateStatus(VNic->pvPort, StatusCode, StatusBuffer, StatusBufferSize);
}

NDIS_STATUS
VNic11SetBeaconEnabledFlag(
    __in  PVNIC                   pVNic,
    __in  BOOLEAN                 BeaconEnabled
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN fProgramHw = FALSE, fPerformPostHwSyncCall = FALSE;

    do
    {
        ndisStatus = VNicPreHwSyncCallActions(pVNic, &fProgramHw);
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): VNicPreHwSyncCallActions failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }
        fPerformPostHwSyncCall = TRUE;
    
        ndisStatus = Hw11SetBeaconEnabledFlag(pVNic->pvHwContext, BeaconEnabled, fProgramHw);

    } while (FALSE);

    if (fPerformPostHwSyncCall)
    {
        VNicPostHwSyncCallActions(pVNic, fProgramHw);
    }    
    
    return ndisStatus;    
}

NDIS_STATUS 
VNic11SetBeaconIE(
    __in  PVNIC   pVNic,
    __in  PVOID   pBeaconIEBlob,
    __in  ULONG   uBeaconIEBlobSize
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN fProgramHw = FALSE, fPerformPostHwSyncCall = FALSE;

    do
    {
        ndisStatus = VNicPreHwSyncCallActions(pVNic, &fProgramHw);
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): VNicPreHwSyncCallActions failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }
        fPerformPostHwSyncCall = TRUE;
    
        ndisStatus = Hw11SetBeaconIE(pVNic->pvHwContext, pBeaconIEBlob, uBeaconIEBlobSize, fProgramHw);

    } while (FALSE);

    if (fPerformPostHwSyncCall)
    {
        VNicPostHwSyncCallActions(pVNic, fProgramHw);
    }    
    
    return ndisStatus;    
}

NDIS_STATUS 
VNic11SetProbeResponseIE(
    __in  PVNIC   pVNic,
    __in  PVOID   pResponseIEBlob,
    __in  ULONG   uResponseIEBlobSize
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN fProgramHw = FALSE, fPerformPostHwSyncCall = FALSE;

    do
    {
        ndisStatus = VNicPreHwSyncCallActions(pVNic, &fProgramHw);
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): VNicPreHwSyncCallActions failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }
        fPerformPostHwSyncCall = TRUE;
    
        ndisStatus = Hw11SetProbeResponseIE(pVNic->pvHwContext, pResponseIEBlob, uResponseIEBlobSize, fProgramHw);

    } while (FALSE);

    if (fPerformPostHwSyncCall)
    {
        VNicPostHwSyncCallActions(pVNic, fProgramHw);
    }    
    
    return ndisStatus;    
}

