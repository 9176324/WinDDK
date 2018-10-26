/*++

Copyright (C) Microsoft Corporation, 1991 - 1999

Module Name:

    clntirp.c

Abstract:

    Client IRP queuing routines for CLASSPNP

Environment:

    kernel mode only

Notes:


Revision History:

--*/

#include "classp.h"
#include "debug.h"


/*
 *  EnqueueDeferredClientIrp
 *
 *      Note: we currently do not support Cancel for storage irps.
 */
VOID EnqueueDeferredClientIrp(PCLASS_PRIVATE_FDO_DATA FdoData, PIRP Irp)
{
    KIRQL oldIrql;

    KeAcquireSpinLock(&FdoData->SpinLock, &oldIrql);
    InsertTailList(&FdoData->DeferredClientIrpList, &Irp->Tail.Overlay.ListEntry);
    KeReleaseSpinLock(&FdoData->SpinLock, oldIrql);
}


/*
 *  DequeueDeferredClientIrp
 *
 */
PIRP DequeueDeferredClientIrp(PCLASS_PRIVATE_FDO_DATA FdoData)
{
    PIRP irp;

    /*
     *  The DeferredClientIrpList is almost always empty.
     *  We don't want to grab the spinlock every time we check it (which is on every xfer completion)
     *  so check once first before we grab the spinlock.
     */
    if (IsListEmpty(&FdoData->DeferredClientIrpList)){
        irp = NULL;
    }
    else {
        PLIST_ENTRY listEntry;
        KIRQL oldIrql;
        
        KeAcquireSpinLock(&FdoData->SpinLock, &oldIrql);
        if (IsListEmpty(&FdoData->DeferredClientIrpList)){
            listEntry = NULL;
        }
        else {
            listEntry = RemoveHeadList(&FdoData->DeferredClientIrpList);
        }
        KeReleaseSpinLock(&FdoData->SpinLock, oldIrql);
        
        if (listEntry == NULL) {
            irp = NULL;
        } 
        else {
            irp = CONTAINING_RECORD(listEntry, IRP, Tail.Overlay.ListEntry);
            ASSERT(irp->Type == IO_TYPE_IRP);
            InitializeListHead(&irp->Tail.Overlay.ListEntry);
        }
    }

    return irp;
}



