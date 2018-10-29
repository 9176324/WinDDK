#pragma once

typedef struct _HVL             HVL, *PHVL;
typedef struct _VNIC            VNIC, *PVNIC;

/*
    Context switches happen between HVL_CONTEXTS. Each such context contains at least
    one VNIC. 
    */
typedef struct _HVL_CONTEXT
{    
    BOOLEAN fCtxInUse;

    // linkage for a list of contexts
    LIST_ENTRY Link;

    // number of VNICs present in this context
    ULONG ulNumVNics;
    
    // the list of VNICs present in this context
    LIST_ENTRY VNicList;

    VNIC_SIGNATURE CtxSig;
}HVL_CONTEXT, *PHVL_CONTEXT;

typedef VOID (* PVNIC_FUNCTION) (PVNIC pVNic, ULONG ulFlags);

VOID
HvlAssignVNicToContext(
    __in  PHVL            pHvl,
    __in  PVNIC           pVNic,
    __out PHVL_CONTEXT   *ppCtx
    );

VOID
HvlinitContext(
    __in PHVL_CONTEXT pCtx,
    __in BOOLEAN fInUse
    );

VOID
HvlRemoveVNicFromCtx(
    __in  PHVL            pHvl,
    __in  PVNIC           pVNic
    );

VOID
HvlPerformCtxMerge(
    PHVL pHvl,
    BOOLEAN *pfMerged
    ); 

VOID
HvlPerformCtxSplit(
    PHVL pHvl,
    PVNIC pVNic
    );

#ifndef HIDE_ANNOTATION
__drv_mustHold(KeSpinLockType)
__drv_requiresIRQL(DISPATCH_LEVEL)
#endif
VOID
HvlNotifyAllVNicsInContext(
    PHVL            pHvl,
    PHVL_CONTEXT    pCtx,
    PVNIC_FUNCTION  pVnicFn,
    ULONG           ulFlags
    );

PHVL_CONTEXT
HvlFindNextCtx(PHVL pHvl);

VOID
HvlUpdateActiveCtx(PHVL pHvl, PHVL_CONTEXT pCurrCtx, PHVL_CONTEXT pNextCtx);


