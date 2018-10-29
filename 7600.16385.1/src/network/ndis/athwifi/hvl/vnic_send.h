#pragma once

VOID
VNicProcessQueuedPkts(
    __in PVNIC pVNic, 
    BOOLEAN fDispatchLevel
    );

#ifndef HIDE_ANNOTATION
__drv_mustHold(KeSpinLockType)
__drv_requiresIRQL(DISPATCH_LEVEL)
#endif
VOID
VNicCancelPendingSends(
    __in  PVNIC                   pVNic
    );

