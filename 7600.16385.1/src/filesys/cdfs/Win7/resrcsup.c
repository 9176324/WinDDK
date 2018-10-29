/*++

Copyright (c) 1989-2000 Microsoft Corporation

Module Name:

    ResrcSup.c

Abstract:

    This module implements the Cdfs Resource acquisition routines


--*/

#include "CdProcs.h"

//
//  The Bug check file id for this module
//

#define BugCheckFileId                   (CDFS_BUG_CHECK_RESRCSUP)

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, CdAcquireForCache)
#pragma alloc_text(PAGE, CdFilterCallbackAcquireForCreateSection)
#pragma alloc_text(PAGE, CdAcquireResource)
#pragma alloc_text(PAGE, CdNoopAcquire)
#pragma alloc_text(PAGE, CdNoopRelease)
#pragma alloc_text(PAGE, CdReleaseForCreateSection)
#pragma alloc_text(PAGE, CdReleaseFromCache)
#endif



__drv_mustHoldCriticalRegion
BOOLEAN
CdAcquireResource (
    __in PIRP_CONTEXT IrpContext,
    __inout __drv_when( (!IgnoreWait && FlagOn( IrpContext->Flags, IRP_CONTEXT_FLAG_WAIT )), __drv_deref(__drv_acquiresResource(ExResourceType))) PERESOURCE Resource,
    __in BOOLEAN IgnoreWait,
    __in TYPE_OF_ACQUIRE Type
    )

/*++

Routine Description:

    This is the single routine used to acquire file system resources.  It
    looks at the IgnoreWait flag to determine whether to try to acquire the
    resource without waiting.  Returning TRUE/FALSE to indicate success or
    failure.  Otherwise it is driven by the WAIT flag in the IrpContext and
    will raise CANT_WAIT on a failure.

Arguments:

    Resource - This is the resource to try and acquire.

    IgnoreWait - If TRUE then this routine will not wait to acquire the
        resource and will return a boolean indicating whether the resource was
        acquired.  Otherwise we use the flag in the IrpContext and raise
        if the resource is not acquired.

    Type - Indicates how we should try to get the resource.

Return Value:

    BOOLEAN - TRUE if the resource is acquired.  FALSE if not acquired and
        IgnoreWait is specified.  Otherwise we raise CANT_WAIT.

--*/

{
    BOOLEAN Wait = FALSE;
    BOOLEAN Acquired;
    PAGED_CODE();

    //
    //  We look first at the IgnoreWait flag, next at the flag in the Irp
    //  Context to decide how to acquire this resource.
    //

    if (!IgnoreWait && FlagOn( IrpContext->Flags, IRP_CONTEXT_FLAG_WAIT )) {

        Wait = TRUE;
    }

    //
    //  Attempt to acquire the resource either shared or exclusively.
    //

    switch (Type) {
        case AcquireExclusive:

#pragma prefast( suppress:28137, "prefast believes Wait should be a constant, but this is ok for CDFS" )
            Acquired = ExAcquireResourceExclusiveLite( Resource, Wait );
            break;

        case AcquireShared:

#pragma prefast( suppress:28137, "prefast believes Wait should be a constant, but this is ok for CDFS" )
            Acquired = ExAcquireResourceSharedLite( Resource, Wait );
            break;

        case AcquireSharedStarveExclusive:

#pragma prefast( suppress:28137, "prefast believes Wait should be a constant, but this is ok for CDFS" )
            Acquired = ExAcquireSharedStarveExclusive( Resource, Wait );
            break;

        default:
            Acquired = FALSE;
            ASSERT( FALSE );
    }

    //
    //  If not acquired and the user didn't specifiy IgnoreWait then
    //  raise CANT_WAIT.
    //

    if (!Acquired && !IgnoreWait) {

        CdRaiseStatus( IrpContext, STATUS_CANT_WAIT );
    }

    return Acquired;
}



__drv_mustHoldCriticalRegion
BOOLEAN
CdAcquireForCache (
    __inout PFCB Fcb,
    __in BOOLEAN Wait
    )

/*++

Routine Description:

    The address of this routine is specified when creating a CacheMap for
    a file.  It is subsequently called by the Lazy Writer for synchronization.

Arguments:

    Fcb -  The pointer supplied as context to the cache initialization
           routine.

    Wait - TRUE if the caller is willing to block.

Return Value:

    None

--*/

{
    PAGED_CODE();

    ASSERT(IoGetTopLevelIrp() == NULL);
    IoSetTopLevelIrp((PIRP)FSRTL_CACHE_TOP_LEVEL_IRP);

    return ExAcquireResourceSharedLite( Fcb->Resource, Wait );
}


__drv_mustHoldCriticalRegion
VOID
CdReleaseFromCache (
    __inout PFCB Fcb
    )

/*++

Routine Description:

    The address of this routine is specified when creating a CacheMap for
    a virtual file.  It is subsequently called by the Lazy Writer to release
    a resource acquired above.

Arguments:

    Fcb -  The pointer supplied as context to the cache initialization
           routine.

Return Value:

    None

--*/

{
    PAGED_CODE();

    ASSERT(IoGetTopLevelIrp() == (PIRP)FSRTL_CACHE_TOP_LEVEL_IRP);
    IoSetTopLevelIrp( NULL );
    
    ExReleaseResourceLite( Fcb->Resource );
}


BOOLEAN
CdNoopAcquire (
    __in PVOID Fcb,
    __in BOOLEAN Wait
    )

/*++

Routine Description:

    This routine does nothing.

Arguments:

    Fcb - The Fcb/Vcb which was specified as a context parameter for this
          routine.

    Wait - TRUE if the caller is willing to block.

Return Value:

    TRUE

--*/

{
    PAGED_CODE();

    UNREFERENCED_PARAMETER( Fcb );
    UNREFERENCED_PARAMETER( Wait );
    
    return TRUE;
}


VOID
CdNoopRelease (
    __in PVOID Fcb
    )

/*++

Routine Description:

    This routine does nothing.

Arguments:

    Fcb - The Fcb/Vcb which was specified as a context parameter for this
          routine.

Return Value:

    None

--*/

{
    PAGED_CODE();

    UNREFERENCED_PARAMETER( Fcb );
}



__drv_mustHoldCriticalRegion
NTSTATUS
CdFilterCallbackAcquireForCreateSection (
    __in PFS_FILTER_CALLBACK_DATA CallbackData,
    __out PVOID *CompletionContext
    )

/*++

Routine Description:

    This is the callback routine for MM to use to acquire the file exclusively.

Arguments:

    FS_FILTER_CALLBACK_DATA - Filter based callback data that provides the file object we
                              want to acquire.

    CompletionContext - Ignored.

Return Value:

    On success we return STATUS_FSFILTER_OP_COMPLETED_SUCCESSFULLY.

    If SyncType is SyncTypeCreateSection, we return a status that indicates there are no
    writers to this file.

--*/

{
    PFILE_OBJECT FileObject;


    PAGED_CODE();
    
    ASSERT( CallbackData->Operation == FS_FILTER_ACQUIRE_FOR_SECTION_SYNCHRONIZATION );
    ASSERT( CallbackData->SizeOfFsFilterCallbackData == sizeof(FS_FILTER_CALLBACK_DATA) );

    //
    //  Get the file object from the callback data.
    //

    FileObject = CallbackData->FileObject;

    //
    //  Get the Fcb resource exclusively.
    //

    ExAcquireResourceExclusiveLite( &((PFCB) FileObject->FsContext)->FcbNonpaged->FcbResource,
                                    TRUE );
                                
    //
    //  Take the File resource shared.  We need this later on when MM calls 
    //  QueryStandardInfo to get the file size.  
    //
    //  If we don't use StarveExclusive,  then we can get wedged behind an 
    //  exclusive waiter who is waiting on someone else holding it shared in the 
    //  read->initializecachemap path (which calls createsection) who is in turn 
    //  waiting on us to finish the create section.
    //

    ExAcquireSharedStarveExclusive( ((PFCB) FileObject->FsContext)->Resource,
                                    TRUE );

    //
    //  CDFS is a read-only file system, so we can always indicate no writers.
    //  We only do this for create section synchronization.  For others we
    //  return the generic success STATUS_FSFILTER_OP_COMPLETED_SUCCESSFULLY.
    //

    if (CallbackData->Parameters.AcquireForSectionSynchronization.SyncType == SyncTypeCreateSection) {

        return STATUS_FILE_LOCKED_WITH_ONLY_READERS;

    } else {

        return STATUS_FSFILTER_OP_COMPLETED_SUCCESSFULLY;
    }

    UNREFERENCED_PARAMETER( CompletionContext );
}


__drv_mustHoldCriticalRegion
VOID
CdReleaseForCreateSection (
    __in PFILE_OBJECT FileObject
    )

/*++

Routine Description:

    This is the callback routine for MM to use to release a file acquired with
    the AcquireForCreateSection call above.

Arguments:

    FileObject - File object for a Cdfs stream.

Return Value:

    None

--*/

{
    PAGED_CODE();

    //
    //  Release the resources.
    //

    ExReleaseResourceLite( &((PFCB) FileObject->FsContext)->FcbNonpaged->FcbResource );
    ExReleaseResourceLite( ((PFCB) FileObject->FsContext)->Resource);
}

