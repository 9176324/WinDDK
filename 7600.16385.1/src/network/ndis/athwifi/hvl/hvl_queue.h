#pragma once

typedef enum _HVL_PENDING_OP_TYPE
{
    HVL_PENDING_OP_EX_ACCESS
}HVL_PENDING_OP_TYPE;

typedef struct _HVL_PENDING_OP
{
    LIST_ENTRY Link;
    
    HVL_PENDING_OP_TYPE Type;
    PVOID pvOpData;
}HVL_PENDING_OP, *PHVL_PENDING_OP;

typedef struct _HVL_EX_ACCESS_REQ
{    
    PVNIC               pVNic;
    ULONG               ulRefCount;
} HVL_EX_ACCESS_REQ, *PHVL_EX_ACCESS_REQ;

typedef struct _HVL_NOTIFICATION
{
    LIST_ENTRY Link;
    
    PVOID pvNotif;
}HVL_NOTIFICATION, *PHVL_NOTIFICATION;

NDIS_STATUS
HvlQueueExAccessRequest(
    __in  PHVL                pHvl,
    __in  PVNIC               pVNic,
    __in  BOOLEAN             fPnPOperation
    );

VOID
HvlDeletePendingOperation(
    __in  PHVL                 pHvl,
    __in  PHVL_PENDING_OP      pPendingOp
    );

BOOLEAN
HvlIsPendingOpQueueEmpty(
    __in  PHVL                   pHvl
    );

PHVL_EX_ACCESS_REQ
HvlDequeueNextReqForVNic(PHVL pHvl, PVNIC pVNic);

PHVL_EX_ACCESS_REQ
HvlGetNextReqForVNic(PHVL pHvl, PVNIC pVNic, BOOLEAN fDequeue);

PHVL_EX_ACCESS_REQ
HvlGetNextReq(PHVL pHvl, BOOLEAN fDequeue);

NDIS_STATUS
HvlQueueNotification(
    PHVL pHvl,
    PVOID pvNotif
    );

VOID
HvlFreeNotification(
    PHVL_NOTIFICATION pHvlNotif
    );

VOID
HvlInitPreAllocatedOp(
    PHVL pHvl
    );

VOID
HvlDeleteExAccessRequest(
    __in  PHVL                        pHvl,
    __in  PHVL_EX_ACCESS_REQ          pExReq
    );

VOID
HvlDeleteAllPendingOperations(
    PHVL pHvl
    );


