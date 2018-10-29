#pragma once

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
    );

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
    );

#ifndef HIDE_ANNOTATION
__drv_mustHold(KeSpinLockType)
__drv_requiresIRQL(DISPATCH_LEVEL)
#endif
NDIS_STATUS
VNicStartBSSHelper(
    __in  PVNIC                   pVNic,
    __in  PMP_BSS_DESCRIPTION     BSSDescription
    );

#ifndef HIDE_ANNOTATION
__drv_mustHold(KeSpinLockType)
__drv_requiresIRQL(DISPATCH_LEVEL)
#endif
NDIS_STATUS
VNicStopBSSHelper(
    __in  PVNIC                   pVNic
    );


