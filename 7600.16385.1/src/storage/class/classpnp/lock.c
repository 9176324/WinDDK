/*++

Copyright (C) Microsoft Corporation, 1990 - 1998

Module Name:

    lock.c

Abstract:

    This is the NT SCSI port driver.

Environment:

    kernel mode only

Notes:

    This module is a driver dll for scsi miniports.

Revision History:

--*/

#include "classp.h"
#include "debug.h"

#ifdef DEBUG_USE_WPP
#include "lock.tmh"
#endif


LONG LockHighWatermark = 0;
LONG LockLowWatermark = 0;
LONG MaxLockedMinutes = 5;

//
// Structure used for tracking remove lock allocations in checked builds
//
typedef struct _REMOVE_TRACKING_BLOCK {
    struct _REMOVE_TRACKING_BLOCK *NextBlock;
    PVOID Tag;
    LARGE_INTEGER TimeLocked;
    PCSTR File;
    ULONG Line;
} REMOVE_TRACKING_BLOCK, *PREMOVE_TRACKING_BLOCK;


/*++////////////////////////////////////////////////////////////////////////////

ClassAcquireRemoveLockEx()

Routine Description:

    This routine is called to acquire the remove lock on the device object.
    While the lock is held, the caller can assume that no pending pnp REMOVE
    requests will be completed.

    The lock should be acquired immediately upon entering a dispatch routine.
    It should also be acquired before creating any new reference to the
    device object if there's a chance of releasing the reference before the
    new one is done.

    This routine will return TRUE if the lock was successfully acquired or
    FALSE if it cannot be because the device object has already been removed.

Arguments:

    DeviceObject - the device object to lock

    Tag - Used for tracking lock allocation and release.  If an irp is
          specified when acquiring the lock then the same Tag must be
          used to release the lock before the Tag is completed.

Return Value:

    The value of the IsRemoved flag in the device extension.  If this is
    non-zero then the device object has received a Remove irp and non-cleanup
    IRP's should fail.

    If the value is REMOVE_COMPLETE, the caller should not even release the
    lock.

--*/
ULONG
ClassAcquireRemoveLockEx(
    __in PDEVICE_OBJECT DeviceObject,
         PVOID Tag,
    __in PCSTR File,
    __in ULONG Line
    )
// This function implements the acquisition of Tag
#pragma warning(suppress:28104)
{
    PCOMMON_DEVICE_EXTENSION commonExtension = DeviceObject->DeviceExtension;

    //
    // Grab the remove lock
    //
    
    #if DBG

        LONG lockValue;

        lockValue = InterlockedIncrement(&commonExtension->RemoveLock);

        TracePrint((TRACE_LEVEL_VERBOSE, TRACE_FLAG_LOCK,  "ClassAcquireRemoveLock: "
                    "Acquired for Object %p & irp %p - count is %d\n",
                    DeviceObject, Tag, lockValue));

        ASSERTMSG("ClassAcquireRemoveLock - lock value was negative : ",
                  (lockValue > 0));

        ASSERTMSG("RemoveLock increased to meet LockHighWatermark",
                  ((LockHighWatermark == 0) ||
                   (lockValue != LockHighWatermark)));

        if (commonExtension->IsRemoved != REMOVE_COMPLETE){
            PREMOVE_TRACKING_BLOCK trackingBlock;

            trackingBlock = ExAllocatePool(NonPagedPool,
                                           sizeof(REMOVE_TRACKING_BLOCK));

            if(trackingBlock == NULL) {

                KIRQL oldIrql;

                KeAcquireSpinLock(&commonExtension->RemoveTrackingSpinlock,
                                  &oldIrql);

                commonExtension->RemoveTrackingUntrackedCount++;

                TracePrint((TRACE_LEVEL_WARNING, TRACE_FLAG_LOCK, ">>>>>ClassAcquireRemoveLock: "
                            "Cannot track Tag %p - currently %d untracked requsts\n",
                            Tag, commonExtension->RemoveTrackingUntrackedCount));

                KeReleaseSpinLock(&commonExtension->RemoveTrackingSpinlock,
                                  oldIrql);
            }
            else {
                PREMOVE_TRACKING_BLOCK *removeTrackingList =
                    &((PREMOVE_TRACKING_BLOCK) commonExtension->RemoveTrackingList);

                KIRQL oldIrql;

                trackingBlock->Tag = Tag;

                trackingBlock->File = File;
                trackingBlock->Line = Line;

                KeQueryTickCount((&trackingBlock->TimeLocked));

                KeAcquireSpinLock(&commonExtension->RemoveTrackingSpinlock,
                                  &oldIrql);

                while(*removeTrackingList != NULL) {

                    if((*removeTrackingList)->Tag > Tag) {
                        break;
                    }

                    if((*removeTrackingList)->Tag == Tag) {

                        TracePrint((TRACE_LEVEL_ERROR, TRACE_FLAG_LOCK, ">>>>>ClassAcquireRemoveLock: "
                                    "already tracking Tag %p\n", Tag));
                        TracePrint((TRACE_LEVEL_ERROR, TRACE_FLAG_LOCK, ">>>>>ClassAcquireRemoveLock: "
                                    "acquired in file %s on line %d\n",
                                    (*removeTrackingList)->File, (*removeTrackingList)->Line));
                        ASSERT(FALSE);
                    }

                    removeTrackingList = &((*removeTrackingList)->NextBlock);
                }

                trackingBlock->NextBlock = *removeTrackingList;
                *removeTrackingList = trackingBlock;

                KeReleaseSpinLock(&commonExtension->RemoveTrackingSpinlock,
                                  oldIrql);

            }
        }
    #else

        InterlockedIncrement(&commonExtension->RemoveLock);

    #endif

    return (commonExtension->IsRemoved);
}

/*++////////////////////////////////////////////////////////////////////////////

ClassReleaseRemoveLock()

Routine Description:

    This routine is called to release the remove lock on the device object.  It
    must be called when finished using a previously locked reference to the
    device object.  If an Tag was specified when acquiring the lock then the
    same Tag must be specified when releasing the lock.

    When the lock count reduces to zero, this routine will signal the waiting
    remove Tag to delete the device object.  As a result the DeviceObject
    pointer should not be used again once the lock has been released.

Arguments:

    DeviceObject - the device object to lock

    Tag - The irp (if any) specified when acquiring the lock.  This is used
          for lock tracking purposes

Return Value:

    none

--*/
VOID
ClassReleaseRemoveLock(
    __in PDEVICE_OBJECT DeviceObject,
         PIRP Tag
    )
// This function implements the release of Tag
#pragma warning(suppress:28103)		
{
    PCOMMON_DEVICE_EXTENSION commonExtension = DeviceObject->DeviceExtension;
    LONG lockValue;

    #if DBG
        PREMOVE_TRACKING_BLOCK *listEntry =
            &((PREMOVE_TRACKING_BLOCK) commonExtension->RemoveTrackingList);

        BOOLEAN found = FALSE;

        LONGLONG maxCount;

        BOOLEAN isRemoved = (commonExtension->IsRemoved == REMOVE_COMPLETE);

        KIRQL oldIrql;

        if(isRemoved) {
            TracePrint((TRACE_LEVEL_VERBOSE, TRACE_FLAG_LOCK, "ClassReleaseRemoveLock: REMOVE_COMPLETE set; this should never happen"));
            InterlockedDecrement(&(commonExtension->RemoveLock));
            return;
        }

        //
        // Check the tick count and make sure this thing hasn't been locked
        // for more than MaxLockedMinutes.
        //

        maxCount = KeQueryTimeIncrement() * 10;     // microseconds
        maxCount *= 1000;                           // milliseconds
        maxCount *= 1000;                           // seconds
        maxCount *= 60;                             // minutes
        maxCount *= MaxLockedMinutes;

        TracePrint((TRACE_LEVEL_VERBOSE, TRACE_FLAG_LOCK,  "ClassReleaseRemoveLock: "
                    "maxCount = %0I64x\n", maxCount));

        KeAcquireSpinLock(&commonExtension->RemoveTrackingSpinlock,
                          &oldIrql);

        while(*listEntry != NULL) {

            PREMOVE_TRACKING_BLOCK block;
            LARGE_INTEGER difference;

            block = *listEntry;

            KeQueryTickCount((&difference));

            difference.QuadPart -= block->TimeLocked.QuadPart;

            TracePrint((TRACE_LEVEL_VERBOSE, TRACE_FLAG_LOCK,  "ClassReleaseRemoveLock: "
                        "Object %p (tag %p) locked for %I64d ticks\n", DeviceObject, block->Tag, difference.QuadPart));

            if(difference.QuadPart >= maxCount) {

                TracePrint((TRACE_LEVEL_ERROR, TRACE_FLAG_LOCK, ">>>>>ClassReleaseRemoveLock: "
                            "Object %p (tag %p) locked for %I64d ticks - TOO LONG\n", DeviceObject, block->Tag, difference.QuadPart));
                TracePrint((TRACE_LEVEL_ERROR, TRACE_FLAG_LOCK, ">>>>>ClassReleaseRemoveLock: "
                            "Lock acquired in file %s on line %d\n", block->File, block->Line));
                ASSERT(FALSE);
            }

            if((found == FALSE) && ((*listEntry)->Tag == Tag)) {

                *listEntry = block->NextBlock;
                FREE_POOL(block);
                found = TRUE;

            } else {

                listEntry = &((*listEntry)->NextBlock);

            }
        }

        if(!found) {
            if(commonExtension->RemoveTrackingUntrackedCount == 0) {
                TracePrint((TRACE_LEVEL_ERROR, TRACE_FLAG_LOCK, ">>>>>ClassReleaseRemoveLock: "
                            "Couldn't find Tag %p in the lock tracking list\n", Tag));
                ASSERT(FALSE);
            } else {
                TracePrint((TRACE_LEVEL_ERROR, TRACE_FLAG_LOCK, ">>>>>ClassReleaseRemoveLock: "
                            "Couldn't find Tag %p in the lock tracking list - "
                            "may be one of the %d untracked requests still outstanding\n",
                            Tag, commonExtension->RemoveTrackingUntrackedCount));

                commonExtension->RemoveTrackingUntrackedCount--;
                ASSERT(commonExtension->RemoveTrackingUntrackedCount >= 0);
            }
        }

        KeReleaseSpinLock(&commonExtension->RemoveTrackingSpinlock,
                          oldIrql);

    #endif

    lockValue = InterlockedDecrement(&commonExtension->RemoveLock);

    TracePrint((TRACE_LEVEL_VERBOSE, TRACE_FLAG_LOCK,  "ClassReleaseRemoveLock: "
                "Released for Object %p & irp %p - count is %d\n",
                DeviceObject, Tag, lockValue));

    ASSERT(lockValue >= 0);

    ASSERTMSG("RemoveLock decreased to meet LockLowWatermark",
              ((LockLowWatermark == 0) || !(lockValue == LockLowWatermark)));

    if(lockValue == 0) {

        ASSERT(commonExtension->IsRemoved);

        //
        // The device needs to be removed.  Signal the remove event
        // that it's safe to go ahead.
        //

        TracePrint((TRACE_LEVEL_VERBOSE, TRACE_FLAG_LOCK,  "ClassReleaseRemoveLock: "
                    "Release for object %p & irp %p caused lock to go to zero\n",
                    DeviceObject, Tag));

        KeSetEvent(&commonExtension->RemoveEvent,
                   IO_NO_INCREMENT,
                   FALSE);

    }
    return;
}

/*++////////////////////////////////////////////////////////////////////////////

ClassCompleteRequest()

Routine Description:

    This routine is a wrapper around (and should be used instead of)
    IoCompleteRequest.  It is used primarily for debugging purposes.
    The routine will assert if the Irp being completed is still holding
    the release lock.

Arguments:

    DeviceObject - the device object that was handling this request

    Irp - the irp to be completed by IoCompleteRequest

    PriorityBoost - the priority boost to pass to IoCompleteRequest

Return Value:

    none

--*/
VOID
ClassCompleteRequest(
    __in PDEVICE_OBJECT DeviceObject,
    __in PIRP Irp,
    __in CCHAR PriorityBoost
    )
{

    #if DBG
        PCOMMON_DEVICE_EXTENSION commonExtension = DeviceObject->DeviceExtension;
        PREMOVE_TRACKING_BLOCK *listEntry =
            &((PREMOVE_TRACKING_BLOCK) commonExtension->RemoveTrackingList);

        KIRQL oldIrql;

        KeAcquireSpinLock(&commonExtension->RemoveTrackingSpinlock,
                          &oldIrql);

        while(*listEntry != NULL) {

            if((*listEntry)->Tag == Irp) {
                break;
            }

            listEntry = &((*listEntry)->NextBlock);
        }

        if(*listEntry != NULL) {

            TracePrint((TRACE_LEVEL_ERROR, TRACE_FLAG_LOCK, ">>>>>ClassCompleteRequest: "
                        "Irp %p completed while still holding the remove lock\n", Irp));
            TracePrint((TRACE_LEVEL_ERROR, TRACE_FLAG_LOCK, ">>>>>ClassCompleteRequest: "
                        "Lock acquired in file %s on line %d\n",
                        (*listEntry)->File, (*listEntry)->Line));
            ASSERT(FALSE);
        }

        KeReleaseSpinLock(&commonExtension->RemoveTrackingSpinlock, oldIrql);
    #endif

    IoCompleteRequest(Irp, PriorityBoost);
    return;
} // end ClassCompleteRequest()

