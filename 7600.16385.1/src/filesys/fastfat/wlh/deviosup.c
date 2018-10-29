/*++

Copyright (c) 1989-2000 Microsoft Corporation

Module Name:

    DevIoSup.c

Abstract:

    This module implements the low lever disk read/write support for Fat.


--*/

#include "FatProcs.h"

//
//  The Bug check file id for this module
//

#define BugCheckFileId                   (FAT_BUG_CHECK_DEVIOSUP)

//
//  Local debug trace level
//

#define Dbg                              (DEBUG_TRACE_DEVIOSUP)

#define CollectDiskIoStats(VCB,FUNCTION,IS_USER_IO,COUNT) {                                    \
    PFILESYSTEM_STATISTICS Stats = &(VCB)->Statistics[KeGetCurrentProcessorNumber() % FatData.NumberProcessors].Common;   \
    if (IS_USER_IO) {                                                                          \
        if ((FUNCTION) == IRP_MJ_WRITE) {                                                      \
            Stats->UserDiskWrites += (COUNT);                                                  \
        } else {                                                                               \
            Stats->UserDiskReads += (COUNT);                                                   \
        }                                                                                      \
    } else {                                                                                   \
        if ((FUNCTION) == IRP_MJ_WRITE) {                                                      \
            Stats->MetaDataDiskWrites += (COUNT);                                              \
        } else {                                                                               \
            Stats->MetaDataDiskReads += (COUNT);                                               \
        }                                                                                      \
    }                                                                                          \
}

//
// Completion Routine declarations
//

NTSTATUS
FatMultiSyncCompletionRoutine (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Contxt
    );

NTSTATUS
FatMultiAsyncCompletionRoutine (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Contxt
    );

NTSTATUS
FatSpecialSyncCompletionRoutine (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Contxt
    );

NTSTATUS
FatSingleSyncCompletionRoutine (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Contxt
    );

NTSTATUS
FatSingleAsyncCompletionRoutine (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Contxt
    );

NTSTATUS
FatPagingFileCompletionRoutine (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID MasterIrp
    );

NTSTATUS
FatPagingFileCompletionRoutineCatch (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Contxt
    );

VOID
FatSingleNonAlignedSync (
    IN PIRP_CONTEXT IrpContext,
    IN PVCB Vcb,
    IN PUCHAR Buffer,
    IN LBO Lbo,
    IN ULONG ByteCount,
    IN PIRP Irp
    );

//
//  The following macro decides whether to send a request directly to
//  the device driver, or to other routines.  It was meant to
//  replace IoCallDriver as transparently as possible.  It must only be
//  called with a read or write Irp.
//
//  NTSTATUS
//  FatLowLevelReadWrite (
//      PIRP_CONTEXT IrpContext,
//      PDEVICE_OBJECT DeviceObject,
//      PIRP Irp,
//      PVCB Vcb
//      );
//

#define FatLowLevelReadWrite(IRPCONTEXT,DO,IRP,VCB) ( \
    IoCallDriver((DO),(IRP))                          \
)

//
//  The following macro handles completion-time zeroing of buffers.
//

#define FatDoCompletionZero( I, C )                                     \
    if ((C)->ZeroMdl) {                                                 \
        ASSERT( (C)->ZeroMdl->MdlFlags & (MDL_MAPPED_TO_SYSTEM_VA |     \
                                          MDL_SOURCE_IS_NONPAGED_POOL));\
        if (NT_SUCCESS((I)->IoStatus.Status)) {                         \
            RtlZeroMemory( (C)->ZeroMdl->MappedSystemVa,                \
                           (C)->ZeroMdl->ByteCount );                   \
        }                                                               \
        IoFreeMdl((C)->ZeroMdl);                                        \
        (C)->ZeroMdl = NULL;                                            \
    }
    
#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, FatMultipleAsync)
#pragma alloc_text(PAGE, FatSingleAsync)
#pragma alloc_text(PAGE, FatSingleNonAlignedSync)
#pragma alloc_text(PAGE, FatWaitSync)
#pragma alloc_text(PAGE, FatLockUserBuffer)
#pragma alloc_text(PAGE, FatBufferUserBuffer)
#pragma alloc_text(PAGE, FatMapUserBuffer)
#pragma alloc_text(PAGE, FatNonCachedIo)
#pragma alloc_text(PAGE, FatNonCachedNonAlignedRead)
#pragma alloc_text(PAGE, FatPerformDevIoCtrl)
#endif

typedef struct FAT_PAGING_FILE_CONTEXT {
    KEVENT Event;
    PMDL RestoreMdl;
} FAT_PAGING_FILE_CONTEXT, *PFAT_PAGING_FILE_CONTEXT;


VOID
FatPagingFileIo (
    IN PIRP Irp,
    IN PFCB Fcb
    )

/*++

Routine Description:

    This routine performs the non-cached disk io described in its parameters.
    This routine nevers blocks, and should only be used with the paging
    file since no completion processing is performed.

Arguments:

    Irp - Supplies the requesting Irp.

    Fcb - Supplies the file to act on.

Return Value:

    None.

--*/

{
    //
    // Declare some local variables for enumeration through the
    // runs of the file.
    //

    VBO Vbo;
    ULONG ByteCount;

    PMDL Mdl;
    LBO NextLbo;
    VBO NextVbo;
    ULONG NextByteCount;
    ULONG RemainingByteCount;
    BOOLEAN MustSucceed;

    ULONG FirstIndex;
    ULONG CurrentIndex;
    ULONG LastIndex;

    LBO LastLbo;
    ULONG LastByteCount;

    BOOLEAN MdlIsReserve = FALSE;
    BOOLEAN IrpIsMaster = FALSE;
    FAT_PAGING_FILE_CONTEXT Context;
    LONG IrpCount;

    PIRP AssocIrp;
    PIO_STACK_LOCATION IrpSp;
    PIO_STACK_LOCATION NextIrpSp;
    ULONG BufferOffset;
    PDEVICE_OBJECT DeviceObject;

    DebugTrace(+1, Dbg, "FatPagingFileIo\n", 0);
    DebugTrace( 0, Dbg, "Irp = %08lx\n", Irp );
    DebugTrace( 0, Dbg, "Fcb = %08lx\n", Fcb );

    ASSERT( FlagOn( Fcb->FcbState, FCB_STATE_PAGING_FILE ));

    //
    //  Initialize some locals.
    //

    BufferOffset = 0;
    DeviceObject = Fcb->Vcb->TargetDeviceObject;
    IrpSp = IoGetCurrentIrpStackLocation( Irp );

    Vbo = IrpSp->Parameters.Read.ByteOffset.LowPart;
    ByteCount = IrpSp->Parameters.Read.Length;

    MustSucceed = FatLookupMcbEntry( Fcb->Vcb, &Fcb->Mcb,
                                     Vbo,
                                     &NextLbo,
                                     &NextByteCount,
                                     &FirstIndex);

    //
    //  If this run isn't present, something is very wrong.
    //

    if (!MustSucceed) {

        FatBugCheck( Vbo, ByteCount, 0 );
    }

    //
    // See if the write covers a single valid run, and if so pass
    // it on.
    //

    if ( NextByteCount >= ByteCount ) {

        DebugTrace( 0, Dbg, "Passing Irp on to Disk Driver\n", 0 );

        //
        //  Setup the next IRP stack location for the disk driver beneath us.
        //

        NextIrpSp = IoGetNextIrpStackLocation( Irp );

        NextIrpSp->MajorFunction = IrpSp->MajorFunction;
        NextIrpSp->Parameters.Read.Length = ByteCount;
        NextIrpSp->Parameters.Read.ByteOffset.QuadPart = NextLbo;

        //
        //  Since this is Paging file IO, we'll just ignore the verify bit.
        //

        SetFlag( NextIrpSp->Flags, SL_OVERRIDE_VERIFY_VOLUME );

        //
        //  Set up the completion routine address in our stack frame.
        //  This is only invoked on error or cancel, and just copies
        //  the error Status into master irp's iosb.
        //
        //  If the error implies a media problem, it also enqueues a
        //  worker item to write out the dirty bit so that the next
        //  time we run we will do a autochk /r
        //

        IoSetCompletionRoutine( Irp,
                                &FatPagingFileCompletionRoutine,
                                Irp,
                                FALSE,
                                TRUE,
                                TRUE );

        //
        //  Issue the read/write request
        //
        //  If IoCallDriver returns an error, it has completed the Irp
        //  and the error will be dealt with as a normal IO error.
        //

        (VOID)IoCallDriver( DeviceObject, Irp );

        DebugTrace(-1, Dbg, "FatPagingFileIo -> VOID\n", 0);
        return;
    }

    //
    //  Find out how may runs there are.
    //

    MustSucceed = FatLookupMcbEntry( Fcb->Vcb, &Fcb->Mcb,
                                     Vbo + ByteCount - 1,
                                     &LastLbo,
                                     &LastByteCount,
                                     &LastIndex);

    //
    //  If this run isn't present, something is very wrong.
    //

    if (!MustSucceed) {

        FatBugCheck( Vbo + ByteCount - 1, 1, 0 );
    }

    CurrentIndex = FirstIndex;

    //
    //  Now set up the Irp->IoStatus.  It will be modified by the
    //  multi-completion routine in case of error or verify required.
    //

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = ByteCount;

    //
    //  Loop while there are still byte writes to satisfy.  The way we'll work this
    //  is to hope for the best - one associated IRP per run, which will let us be
    //  completely async after launching all the IO.
    //
    //  IrpCount will indicate the remaining number of associated Irps to launch.
    //
    //  All we have to do is make sure IrpCount doesn't hit zero before we're building
    //  the very last Irp.  If it is positive when we're done, it means we have to
    //  wait for the rest of the associated Irps to come back before we complete the
    //  master by hand.
    //
    //  This will keep the master from completing early.
    //

    Irp->AssociatedIrp.IrpCount = IrpCount = LastIndex - FirstIndex + 1;

    while (CurrentIndex <= LastIndex) {

        //
        //  Reset this for unwinding purposes
        //

        AssocIrp = NULL;

        //
        //  If next run is larger than we need, "ya get what ya need".
        //

        if (NextByteCount > ByteCount) {
            NextByteCount = ByteCount;
        }

        RemainingByteCount = 0;

        //
        // Allocate and build a partial Mdl for the request.
        //

        Mdl = IoAllocateMdl( (PCHAR)Irp->UserBuffer + BufferOffset,
                             NextByteCount,
                             FALSE,
                             FALSE,
                             AssocIrp );

        if (Mdl == NULL) {

            //
            //  Pick up the reserve MDL
            //

            KeWaitForSingleObject( &FatReserveEvent, Executive, KernelMode, FALSE, NULL );

            Mdl = FatReserveMdl;
            MdlIsReserve = TRUE;

            //
            //  Trim to fit the size of the reserve MDL.
            //

            if (NextByteCount > FAT_RESERVE_MDL_SIZE * PAGE_SIZE) {

                RemainingByteCount = NextByteCount - FAT_RESERVE_MDL_SIZE * PAGE_SIZE;
                NextByteCount = FAT_RESERVE_MDL_SIZE * PAGE_SIZE;
            }
        }

        IoBuildPartialMdl( Irp->MdlAddress,
                           Mdl,
                           (PCHAR)Irp->UserBuffer + BufferOffset,
                           NextByteCount );

        //
        //  Now that we have properly bounded this piece of the transfer, it is
        //  time to read/write it.  We can simplify life slightly by always
        //  re-using the master IRP for cases where we use the reserve MDL,
        //  since we'll always be synchronous for those and can use a single
        //  completion context on our local stack.
        //
        //  We also must prevent ourselves from issuing an associated IRP that would
        //  complete the master UNLESS this is the very last IRP we'll issue.
        //
        //  This logic looks a bit nasty, but is hopefully straightforward.
        //

        if (!MdlIsReserve &&
            (IrpCount != 1 ||
             (CurrentIndex == LastIndex &&
              RemainingByteCount == 0))) {

            AssocIrp = IoMakeAssociatedIrp( Irp, (CCHAR)(DeviceObject->StackSize + 1) );
        }
        
        if (AssocIrp == NULL) {

            AssocIrp = Irp;
            IrpIsMaster = TRUE;

            //
            //  We need to drain the associated Irps so we can reliably figure out if
            //  the master Irp is showing a failed status, in which case we bail out
            //  immediately - as opposed to putting the value in the status field in
            //  jeopardy due to our re-use of the master Irp.
            //

            while (Irp->AssociatedIrp.IrpCount != IrpCount) {

                KeDelayExecutionThread (KernelMode, FALSE, &Fat30Milliseconds);
            }

            //
            //  Note that since we failed to launch this associated Irp, that the completion
            //  code at the bottom will take care of completing the master Irp.
            //
            
            if (!NT_SUCCESS(Irp->IoStatus.Status)) {

                ASSERT( IrpCount );
                break;
            }

        } else {
                        
            //
            //  Indicate we used an associated Irp.
            //

            IrpCount -= 1;
        }
        
        //
        //  With an associated IRP, we must take over the first stack location so
        //  we can have one to put the completion routine on.  When re-using the
        //  master IRP, its already there.
        //
        
        if (!IrpIsMaster) {
            
            //
            //  Get the first IRP stack location in the associated Irp
            //

            IoSetNextIrpStackLocation( AssocIrp );
            NextIrpSp = IoGetCurrentIrpStackLocation( AssocIrp );

            //
            //  Setup the Stack location to describe our read.
            //

            NextIrpSp->MajorFunction = IrpSp->MajorFunction;
            NextIrpSp->Parameters.Read.Length = NextByteCount;
            NextIrpSp->Parameters.Read.ByteOffset.QuadPart = Vbo;

            //
            //  We also need the VolumeDeviceObject in the Irp stack in case
            //  we take the failure path.
            //

            NextIrpSp->DeviceObject = IrpSp->DeviceObject;
            
        } else {

            //
            //  Save the MDL in the IRP and prepare the stack
            //  context for the completion routine.
            //

            KeInitializeEvent( &Context.Event, SynchronizationEvent, FALSE );
            Context.RestoreMdl = Irp->MdlAddress;
        }

        //
        //  And drop our Mdl into the Irp.
        //

        AssocIrp->MdlAddress = Mdl;

        //
        //  Set up the completion routine address in our stack frame.
        //  For true associated IRPs, this is only invoked on error or
        //  cancel, and just copies the error Status into master irp's
        //  iosb.
        //
        //  If the error implies a media problem, it also enqueues a
        //  worker item to write out the dirty bit so that the next
        //  time we run we will do a autochk /r
        //

        if (IrpIsMaster) {
            
            IoSetCompletionRoutine( AssocIrp,
                                    FatPagingFileCompletionRoutineCatch,
                                    &Context,
                                    TRUE,
                                    TRUE,
                                    TRUE );

        } else {
            
            IoSetCompletionRoutine( AssocIrp,
                                    FatPagingFileCompletionRoutine,
                                    Irp,
                                    FALSE,
                                    TRUE,
                                    TRUE );
        }

        //
        //  Setup the next IRP stack location for the disk driver beneath us.
        //

        NextIrpSp = IoGetNextIrpStackLocation( AssocIrp );

        //
        //  Since this is paging file IO, we'll just ignore the verify bit.
        //

        SetFlag( NextIrpSp->Flags, SL_OVERRIDE_VERIFY_VOLUME );

        //
        //  Setup the Stack location to do a read from the disk driver.
        //

        NextIrpSp->MajorFunction = IrpSp->MajorFunction;
        NextIrpSp->Parameters.Read.Length = NextByteCount;
        NextIrpSp->Parameters.Read.ByteOffset.QuadPart = NextLbo;

        (VOID)IoCallDriver( DeviceObject, AssocIrp );

        //
        //  Wait for the Irp in the catch case and drop the flags.
        //

        if (IrpIsMaster) {
            
            KeWaitForSingleObject( &Context.Event, Executive, KernelMode, FALSE, NULL );
            IrpIsMaster = MdlIsReserve = FALSE;

            //
            //  If the Irp is showing a failed status, there is no point in continuing.
            //  In doing so, we get to avoid squirreling away the failed status in case
            //  we were to re-use the master irp again.
            //
            //  Note that since we re-used the master, we must not have issued the "last"
            //  associated Irp, and thus the completion code at the bottom will take care
            //  of that for us.
            //
            
            if (!NT_SUCCESS(Irp->IoStatus.Status)) {

                ASSERT( IrpCount );
                break;
            }
        }

        //
        //  Now adjust everything for the next pass through the loop.
        //

        Vbo += NextByteCount;
        BufferOffset += NextByteCount;
        ByteCount -= NextByteCount;

        //
        //  Try to lookup the next run, if we are not done and we got
        //  all the way through the current run.
        //

        if (RemainingByteCount) {

            //
            //  Advance the Lbo/Vbo if we have more to do in the current run.
            //
            
            NextLbo += NextByteCount;
            NextVbo += NextByteCount;

            NextByteCount = RemainingByteCount;
        
        } else {
        
            CurrentIndex += 1;

            if ( CurrentIndex <= LastIndex ) {

                ASSERT( ByteCount != 0 );

                FatGetNextMcbEntry( Fcb->Vcb, &Fcb->Mcb,
                                    CurrentIndex,
                                    &NextVbo,
                                    &NextLbo,
                                    &NextByteCount );

                ASSERT( NextVbo == Vbo );
            }
        }
    } // while ( CurrentIndex <= LastIndex )

    //
    //  If we didn't get enough associated Irps going to make this asynchronous, we
    //  twiddle our thumbs and wait for those we did launch to complete.
    //
    
    if (IrpCount) {

        while (Irp->AssociatedIrp.IrpCount != IrpCount) {
            
            KeDelayExecutionThread (KernelMode, FALSE, &Fat30Milliseconds);
        }

        IoCompleteRequest( Irp, IO_DISK_INCREMENT );
    }

    DebugTrace(-1, Dbg, "FatPagingFileIo -> VOID\n", 0);
    return;
}


NTSTATUS
FatNonCachedIo (
    IN PIRP_CONTEXT IrpContext,
    IN PIRP Irp,
    IN PFCB FcbOrDcb,
    IN ULONG StartingVbo,
    IN ULONG ByteCount,
    IN ULONG UserByteCount
    )

/*++

Routine Description:

    This routine performs the non-cached disk io described in its parameters.
    The choice of a single run is made if possible, otherwise multiple runs
    are executed.

Arguments:

    IrpContext->MajorFunction - Supplies either IRP_MJ_READ or IRP_MJ_WRITE.

    Irp - Supplies the requesting Irp.

    FcbOrDcb - Supplies the file to act on.

    StartingVbo - The starting point for the operation.

    ByteCount - The lengh of the operation.
    
    UserByteCount - The last byte the user can see, rest to be zeroed.

Return Value:

    None.

--*/

{
    //
    // Declare some local variables for enumeration through the
    // runs of the file, and an array to store parameters for
    // parallel I/Os
    //

    BOOLEAN Wait;

    LBO NextLbo;
    VBO NextVbo;
    ULONG NextByteCount;
    BOOLEAN NextIsAllocated;

    LBO LastLbo;
    ULONG LastByteCount;
    BOOLEAN LastIsAllocated;

    BOOLEAN EndOnMax;

    ULONG FirstIndex;
    ULONG CurrentIndex;
    ULONG LastIndex;

    ULONG NextRun;
    ULONG BufferOffset;
    ULONG OriginalByteCount;

    IO_RUN StackIoRuns[FAT_MAX_IO_RUNS_ON_STACK];
    PIO_RUN IoRuns;

    PAGED_CODE();

    DebugTrace(+1, Dbg, "FatNonCachedIo\n", 0);
    DebugTrace( 0, Dbg, "Irp           = %08lx\n", Irp );
    DebugTrace( 0, Dbg, "MajorFunction = %08lx\n", IrpContext->MajorFunction );
    DebugTrace( 0, Dbg, "FcbOrDcb      = %08lx\n", FcbOrDcb );
    DebugTrace( 0, Dbg, "StartingVbo   = %08lx\n", StartingVbo );
    DebugTrace( 0, Dbg, "ByteCount     = %08lx\n", ByteCount );

    if (!FlagOn(Irp->Flags, IRP_PAGING_IO)) {

        PFILE_SYSTEM_STATISTICS Stats =
            &FcbOrDcb->Vcb->Statistics[KeGetCurrentProcessorNumber() % FatData.NumberProcessors];

        if (IrpContext->MajorFunction == IRP_MJ_READ) {
            Stats->Fat.NonCachedReads += 1;
            Stats->Fat.NonCachedReadBytes += ByteCount;
        } else {
            Stats->Fat.NonCachedWrites += 1;
            Stats->Fat.NonCachedWriteBytes += ByteCount;
        }
    }

    //
    //  Initialize some locals.
    //

    NextRun = 0;
    BufferOffset = 0;
    OriginalByteCount = ByteCount;

    Wait = BooleanFlagOn(IrpContext->Flags, IRP_CONTEXT_FLAG_WAIT);

    //
    // For nonbuffered I/O, we need the buffer locked in all
    // cases.
    //
    // This call may raise.  If this call succeeds and a subsequent
    // condition is raised, the buffers are unlocked automatically
    // by the I/O system when the request is completed, via the
    // Irp->MdlAddress field.
    //

    FatLockUserBuffer( IrpContext,
                       Irp,
                       (IrpContext->MajorFunction == IRP_MJ_READ) ?
                       IoWriteAccess : IoReadAccess,
                       ByteCount );

    //
    //  Setup the required zeroing for read requests.
    //

    if (UserByteCount != ByteCount) {

        PMDL Mdl;

        ASSERT( ByteCount > UserByteCount );
        __analysis_assume(ByteCount > UserByteCount);

        Mdl = IoAllocateMdl( (PUCHAR) Irp->UserBuffer + UserByteCount,
                             ByteCount - UserByteCount,
                             FALSE,
                             FALSE,
                             NULL );

        if (Mdl == NULL) {

            FatRaiseStatus( IrpContext, STATUS_INSUFFICIENT_RESOURCES );
        }

        IoBuildPartialMdl( Irp->MdlAddress,
                           Mdl,
                           (PUCHAR) Irp->UserBuffer + UserByteCount,
                           ByteCount - UserByteCount );

        IrpContext->FatIoContext->ZeroMdl = Mdl;

        //
        //  Map the MDL now so we can't fail at IO completion time.  Note
        //  that this will be only a single page.
        //

        if (MmGetSystemAddressForMdlSafe( Mdl, NormalPagePriority ) == NULL) {

            FatRaiseStatus( IrpContext, STATUS_INSUFFICIENT_RESOURCES );
        }
    }

    //
    //  Try to lookup the first run.  If there is just a single run,
    //  we may just be able to pass it on.
    //

    FatLookupFileAllocation( IrpContext,
                             FcbOrDcb,
                             StartingVbo,
                             &NextLbo,
                             &NextByteCount,
                             &NextIsAllocated,
                             &EndOnMax,
                             &FirstIndex );

    //
    //  We just added the allocation, thus there must be at least
    //  one entry in the mcb corresponding to our write, ie.
    //  NextIsAllocated must be true.  If not, the pre-existing file
    //  must have an allocation error.
    //

    if ( !NextIsAllocated ) {

        FatPopUpFileCorrupt( IrpContext, FcbOrDcb );

        FatRaiseStatus( IrpContext, STATUS_FILE_CORRUPT_ERROR );
    }

    ASSERT( NextByteCount != 0 );

    //
    //  If the request was not aligned correctly, read in the first
    //  part first.
    //


    //
    //  See if the write covers a single valid run, and if so pass
    //  it on.  We must bias this by the byte that is lost at the
    //  end of the maximal file.
    //

    if ( NextByteCount >= ByteCount - (EndOnMax ? 1 : 0)) {

        if (FlagOn(Irp->Flags, IRP_PAGING_IO)) {
            CollectDiskIoStats(FcbOrDcb->Vcb, IrpContext->MajorFunction,
                               FlagOn(IrpContext->Flags, IRP_CONTEXT_FLAG_USER_IO), 1);
        } else {

            PFILE_SYSTEM_STATISTICS Stats =
                &FcbOrDcb->Vcb->Statistics[KeGetCurrentProcessorNumber() % FatData.NumberProcessors];

            if (IrpContext->MajorFunction == IRP_MJ_READ) {
                Stats->Fat.NonCachedDiskReads += 1;
            } else {
                Stats->Fat.NonCachedDiskWrites += 1;
            }
        }

        DebugTrace( 0, Dbg, "Passing 1 Irp on to Disk Driver\n", 0 );

        FatSingleAsync( IrpContext,
                        FcbOrDcb->Vcb,
                        NextLbo,
                        ByteCount,
                        Irp );

    } else {

        //
        //  If there we can't wait, and there are more runs than we can handle,
        //  we will have to post this request.
        //

        FatLookupFileAllocation( IrpContext,
                                 FcbOrDcb,
                                 StartingVbo + ByteCount - 1,
                                 &LastLbo,
                                 &LastByteCount,
                                 &LastIsAllocated,
                                 &EndOnMax,
                                 &LastIndex );

        //
        // Since we already added the allocation for the whole
        // write, assert that we find runs until ByteCount == 0
        // Otherwise this file is corrupt.
        //

        if ( !LastIsAllocated ) {

            FatPopUpFileCorrupt( IrpContext, FcbOrDcb );

            FatRaiseStatus( IrpContext, STATUS_FILE_CORRUPT_ERROR );
        }

        if (LastIndex - FirstIndex + 1 > FAT_MAX_IO_RUNS_ON_STACK) {

            IoRuns = FsRtlAllocatePoolWithTag( PagedPool,
                                               (LastIndex - FirstIndex + 1) * sizeof(IO_RUN),
                                               TAG_IO_RUNS );

        } else {

            IoRuns = StackIoRuns;
        }

        ASSERT( LastIndex != FirstIndex );

        CurrentIndex = FirstIndex;

        //
        // Loop while there are still byte writes to satisfy.
        //

        while (CurrentIndex <= LastIndex) {


            ASSERT( NextByteCount != 0);
            ASSERT( ByteCount != 0);

            //
            // If next run is larger than we need, "ya get what you need".
            //

            if (NextByteCount > ByteCount) {
                NextByteCount = ByteCount;
            }

            //
            // Now that we have properly bounded this piece of the
            // transfer, it is time to write it.
            //
            // We remember each piece of a parallel run by saving the
            // essential information in the IoRuns array.  The tranfers
            // are started up in parallel below.
            //

            IoRuns[NextRun].Vbo = StartingVbo;
            IoRuns[NextRun].Lbo = NextLbo;
            IoRuns[NextRun].Offset = BufferOffset;
            IoRuns[NextRun].ByteCount = NextByteCount;
            NextRun += 1;

            //
            // Now adjust everything for the next pass through the loop.
            //

            StartingVbo += NextByteCount;
            BufferOffset += NextByteCount;
            ByteCount -= NextByteCount;

            //
            // Try to lookup the next run (if we are not done).
            //

            CurrentIndex += 1;

            if ( CurrentIndex <= LastIndex ) {

                ASSERT( ByteCount != 0 );

                FatGetNextMcbEntry( FcbOrDcb->Vcb, &FcbOrDcb->Mcb,
                                    CurrentIndex,
                                    &NextVbo,
                                    &NextLbo,
                                    &NextByteCount );

                ASSERT( NextVbo == StartingVbo );
            }

        } // while ( CurrentIndex <= LastIndex )

        //
        //  Now set up the Irp->IoStatus.  It will be modified by the
        //  multi-completion routine in case of error or verify required.
        //

        Irp->IoStatus.Status = STATUS_SUCCESS;
        Irp->IoStatus.Information = OriginalByteCount;

        if (FlagOn(Irp->Flags, IRP_PAGING_IO)) {
            CollectDiskIoStats(FcbOrDcb->Vcb, IrpContext->MajorFunction,
                               FlagOn(IrpContext->Flags, IRP_CONTEXT_FLAG_USER_IO), NextRun);
        }

        //
        //  OK, now do the I/O.
        //

        try {

            DebugTrace( 0, Dbg, "Passing Multiple Irps on to Disk Driver\n", 0 );

            FatMultipleAsync( IrpContext,
                              FcbOrDcb->Vcb,
                              Irp,
                              NextRun,
                              IoRuns );

        } finally {

            if (IoRuns != StackIoRuns) {

                ExFreePool( IoRuns );
            }
        }
    }

    if (!Wait) {

        DebugTrace(-1, Dbg, "FatNonCachedIo -> STATUS_PENDING\n", 0);
        return STATUS_PENDING;
    }

    FatWaitSync( IrpContext );

    DebugTrace(-1, Dbg, "FatNonCachedIo -> 0x%08lx\n", Irp->IoStatus.Status);
    return Irp->IoStatus.Status;
}


VOID
FatNonCachedNonAlignedRead (
    IN PIRP_CONTEXT IrpContext,
    IN PIRP Irp,
    IN PFCB FcbOrDcb,
    IN ULONG StartingVbo,
    IN ULONG ByteCount
    )

/*++

Routine Description:

    This routine performs the non-cached disk io described in its parameters.
    This routine differs from the above in that the range does not have to be
    sector aligned.  This accomplished with the use of intermediate buffers.

Arguments:

    IrpContext->MajorFunction - Supplies either IRP_MJ_READ or IRP_MJ_WRITE.

    Irp - Supplies the requesting Irp.

    FcbOrDcb - Supplies the file to act on.

    StartingVbo - The starting point for the operation.

    ByteCount - The lengh of the operation.

Return Value:

    None.

--*/

{
    //
    // Declare some local variables for enumeration through the
    // runs of the file, and an array to store parameters for
    // parallel I/Os
    //

    LBO NextLbo;
    ULONG NextByteCount;
    BOOLEAN NextIsAllocated;

    ULONG SectorSize;
    ULONG BytesToCopy;
    ULONG OriginalByteCount;
    ULONG OriginalStartingVbo;

    BOOLEAN EndOnMax;

    PUCHAR UserBuffer;
    PUCHAR DiskBuffer = NULL;

    PMDL Mdl;
    PMDL SavedMdl;
    PVOID SavedUserBuffer;

    PAGED_CODE();

    DebugTrace(+1, Dbg, "FatNonCachedNonAlignedRead\n", 0);
    DebugTrace( 0, Dbg, "Irp           = %08lx\n", Irp );
    DebugTrace( 0, Dbg, "MajorFunction = %08lx\n", IrpContext->MajorFunction );
    DebugTrace( 0, Dbg, "FcbOrDcb      = %08lx\n", FcbOrDcb );
    DebugTrace( 0, Dbg, "StartingVbo   = %08lx\n", StartingVbo );
    DebugTrace( 0, Dbg, "ByteCount     = %08lx\n", ByteCount );

    //
    //  Initialize some locals.
    //

    OriginalByteCount = ByteCount;
    OriginalStartingVbo = StartingVbo;
    SectorSize = FcbOrDcb->Vcb->Bpb.BytesPerSector;

    ASSERT( FlagOn(IrpContext->Flags, IRP_CONTEXT_FLAG_WAIT) );

    //
    // For nonbuffered I/O, we need the buffer locked in all
    // cases.
    //
    // This call may raise.  If this call succeeds and a subsequent
    // condition is raised, the buffers are unlocked automatically
    // by the I/O system when the request is completed, via the
    // Irp->MdlAddress field.
    //

    FatLockUserBuffer( IrpContext,
                       Irp,
                       IoWriteAccess,
                       ByteCount );

    UserBuffer = FatMapUserBuffer( IrpContext, Irp );

    //
    //  Allocate the local buffer
    //

    DiskBuffer = FsRtlAllocatePoolWithTag( NonPagedPoolCacheAligned,
                                           (ULONG) ROUND_TO_PAGES( SectorSize ),
                                           TAG_IO_BUFFER );

    //
    //  We use a try block here to ensure the buffer is freed, and to
    //  fill in the correct byte count in the Iosb.Information field.
    //

    try {

        //
        //  If the beginning of the request was not aligned correctly, read in
        //  the first part first.
        //

        if ( StartingVbo & (SectorSize - 1) ) {

            VBO Hole;

            //
            // Try to lookup the first run.
            //

            FatLookupFileAllocation( IrpContext,
                                     FcbOrDcb,
                                     StartingVbo,
                                     &NextLbo,
                                     &NextByteCount,
                                     &NextIsAllocated,
                                     &EndOnMax,
                                     NULL );

            //
            // We just added the allocation, thus there must be at least
            // one entry in the mcb corresponding to our write, ie.
            // NextIsAllocated must be true.  If not, the pre-existing file
            // must have an allocation error.
            //

            if ( !NextIsAllocated ) {

                FatPopUpFileCorrupt( IrpContext, FcbOrDcb );

                FatRaiseStatus( IrpContext, STATUS_FILE_CORRUPT_ERROR );
            }

            FatSingleNonAlignedSync( IrpContext,
                                     FcbOrDcb->Vcb,
                                     DiskBuffer,
                                     NextLbo & ~((LONG)SectorSize - 1),
                                     SectorSize,
                                     Irp );

            if (!NT_SUCCESS( Irp->IoStatus.Status )) {

                try_return( NOTHING );
            }

            //
            //  Now copy the part of the first sector that we want to the user
            //  buffer.
            //

            Hole = StartingVbo & (SectorSize - 1);

            BytesToCopy = ByteCount >= SectorSize - Hole ?
                                       SectorSize - Hole : ByteCount;

            RtlCopyMemory( UserBuffer, DiskBuffer + Hole, BytesToCopy );

            StartingVbo += BytesToCopy;
            ByteCount -= BytesToCopy;

            if ( ByteCount == 0 ) {

                try_return( NOTHING );
            }
        }

        ASSERT( (StartingVbo & (SectorSize - 1)) == 0 );

        //
        //  If there is a tail part that is not sector aligned, read it.
        //

        if ( ByteCount & (SectorSize - 1) ) {

            VBO LastSectorVbo;

            LastSectorVbo = StartingVbo + (ByteCount & ~(SectorSize - 1));

            //
            // Try to lookup the last part of the requested range.
            //

            FatLookupFileAllocation( IrpContext,
                                     FcbOrDcb,
                                     LastSectorVbo,
                                     &NextLbo,
                                     &NextByteCount,
                                     &NextIsAllocated,
                                     &EndOnMax,
                                     NULL );

            //
            // We just added the allocation, thus there must be at least
            // one entry in the mcb corresponding to our write, ie.
            // NextIsAllocated must be true.  If not, the pre-existing file
            // must have an allocation error.
            //

            if ( !NextIsAllocated ) {

                FatPopUpFileCorrupt( IrpContext, FcbOrDcb );

                FatRaiseStatus( IrpContext, STATUS_FILE_CORRUPT_ERROR );
            }

            FatSingleNonAlignedSync( IrpContext,
                                     FcbOrDcb->Vcb,
                                     DiskBuffer,
                                     NextLbo,
                                     SectorSize,
                                     Irp );

            if (!NT_SUCCESS( Irp->IoStatus.Status )) {

                try_return( NOTHING );
            }

            //
            //  Now copy over the part of this last sector that we need.
            //

            BytesToCopy = ByteCount & (SectorSize - 1);

            UserBuffer += LastSectorVbo - OriginalStartingVbo;

            RtlCopyMemory( UserBuffer, DiskBuffer, BytesToCopy );

            ByteCount -= BytesToCopy;

            if ( ByteCount == 0 ) {

                try_return( NOTHING );
            }
        }

        ASSERT( ((StartingVbo | ByteCount) & (SectorSize - 1)) == 0 );

        //
        //  Now build a Mdl describing the sector aligned balance of the transfer,
        //  and put it in the Irp, and read that part.
        //

        SavedMdl = Irp->MdlAddress;
        Irp->MdlAddress = NULL;

        SavedUserBuffer = Irp->UserBuffer;

        Irp->UserBuffer = (PUCHAR)MmGetMdlVirtualAddress( SavedMdl ) +
                          (StartingVbo - OriginalStartingVbo);

        Mdl = IoAllocateMdl( Irp->UserBuffer,
                             ByteCount,
                             FALSE,
                             FALSE,
                             Irp );

        if (Mdl == NULL) {

            Irp->MdlAddress = SavedMdl;
            Irp->UserBuffer = SavedUserBuffer;
            FatRaiseStatus( IrpContext, STATUS_INSUFFICIENT_RESOURCES );
        }

        IoBuildPartialMdl( SavedMdl,
                           Mdl,
                           Irp->UserBuffer,
                           ByteCount );

        //
        //  Try to read in the pages.
        //

        try {

            FatNonCachedIo( IrpContext,
                            Irp,
                            FcbOrDcb,
                            StartingVbo,
                            ByteCount,
                            ByteCount );

        } finally {

            IoFreeMdl( Irp->MdlAddress );

            Irp->MdlAddress = SavedMdl;
            Irp->UserBuffer = SavedUserBuffer;
        }

    try_exit: NOTHING;

    } finally {

        ExFreePool( DiskBuffer );

        if ( !AbnormalTermination() && NT_SUCCESS(Irp->IoStatus.Status) ) {

            Irp->IoStatus.Information = OriginalByteCount;

            //
            //  We now flush the user's buffer to memory.
            //

            KeFlushIoBuffers( Irp->MdlAddress, TRUE, FALSE );
        }
    }

    DebugTrace(-1, Dbg, "FatNonCachedNonAlignedRead -> VOID\n", 0);
    return;
}


VOID
FatMultipleAsync (
    IN PIRP_CONTEXT IrpContext,
    IN PVCB Vcb,
    IN PIRP MasterIrp,
    IN ULONG MultipleIrpCount,
    IN PIO_RUN IoRuns
    )

/*++

Routine Description:

    This routine first does the initial setup required of a Master IRP that is
    going to be completed using associated IRPs.  This routine should not
    be used if only one async request is needed, instead the single read/write
    async routines should be called.

    A context parameter is initialized, to serve as a communications area
    between here and the common completion routine.  This initialization
    includes allocation of a spinlock.  The spinlock is deallocated in the
    FatWaitSync routine, so it is essential that the caller insure that
    this routine is always called under all circumstances following a call
    to this routine.

    Next this routine reads or writes one or more contiguous sectors from
    a device asynchronously, and is used if there are multiple reads for a
    master IRP.  A completion routine is used to synchronize with the
    completion of all of the I/O requests started by calls to this routine.

    Also, prior to calling this routine the caller must initialize the
    IoStatus field in the Context, with the correct success status and byte
    count which are expected if all of the parallel transfers complete
    successfully.  After return this status will be unchanged if all requests
    were, in fact, successful.  However, if one or more errors occur, the
    IoStatus will be modified to reflect the error status and byte count
    from the first run (by Vbo) which encountered an error.  I/O status
    from all subsequent runs will not be indicated.

Arguments:

    IrpContext->MajorFunction - Supplies either IRP_MJ_READ or IRP_MJ_WRITE.

    Vcb - Supplies the device to be read

    MasterIrp - Supplies the master Irp.

    MulitpleIrpCount - Supplies the number of multiple async requests
        that will be issued against the master irp.

    IoRuns - Supplies an array containing the Vbo, Lbo, BufferOffset, and
        ByteCount for all the runs to executed in parallel.

Return Value:

    None.

--*/

{
    PIRP Irp;
    PIO_STACK_LOCATION IrpSp;
    PMDL Mdl;
    BOOLEAN Wait;
    PFAT_IO_CONTEXT Context;

    ULONG UnwindRunCount = 0;

    BOOLEAN ExceptionExpected = TRUE;

    PAGED_CODE();

    DebugTrace(+1, Dbg, "FatMultipleAsync\n", 0);
    DebugTrace( 0, Dbg, "MajorFunction    = %08lx\n", IrpContext->MajorFunction );
    DebugTrace( 0, Dbg, "Vcb              = %08lx\n", Vcb );
    DebugTrace( 0, Dbg, "MasterIrp        = %08lx\n", MasterIrp );
    DebugTrace( 0, Dbg, "MultipleIrpCount = %08lx\n", MultipleIrpCount );
    DebugTrace( 0, Dbg, "IoRuns           = %08lx\n", IoRuns );

    //
    //  If this I/O originating during FatVerifyVolume, bypass the
    //  verify logic.
    //

    if (Vcb->VerifyThread == KeGetCurrentThread()) {

        SetFlag( IrpContext->Flags, IRP_CONTEXT_FLAG_OVERRIDE_VERIFY );
    }

    //
    //  Set up things according to whether this is truely async.
    //

    Wait = BooleanFlagOn( IrpContext->Flags, IRP_CONTEXT_FLAG_WAIT );

    Context = IrpContext->FatIoContext;

    //
    //  Finish initializing Context, for use in Read/Write Multiple Asynch.
    //

    Context->MasterIrp = MasterIrp;

    try {

        //
        //  Itterate through the runs, doing everything that can fail
        //

        for ( UnwindRunCount = 0;
              UnwindRunCount < MultipleIrpCount;
              UnwindRunCount++ ) {

            //
            //  Create an associated IRP, making sure there is one stack entry for
            //  us, as well.
            //

            IoRuns[UnwindRunCount].SavedIrp = 0;

            Irp = IoMakeAssociatedIrp( MasterIrp,
                                       (CCHAR)(Vcb->TargetDeviceObject->StackSize + 1) );

            if (Irp == NULL) {

                FatRaiseStatus( IrpContext, STATUS_INSUFFICIENT_RESOURCES );
            }

            IoRuns[UnwindRunCount].SavedIrp = Irp;

            //
            // Allocate and build a partial Mdl for the request.
            //

            Mdl = IoAllocateMdl( (PCHAR)MasterIrp->UserBuffer +
                                 IoRuns[UnwindRunCount].Offset,
                                 IoRuns[UnwindRunCount].ByteCount,
                                 FALSE,
                                 FALSE,
                                 Irp );

            if (Mdl == NULL) {

                FatRaiseStatus( IrpContext, STATUS_INSUFFICIENT_RESOURCES );
            }

            //
            //  Sanity Check
            //

            ASSERT( Mdl == Irp->MdlAddress );

            IoBuildPartialMdl( MasterIrp->MdlAddress,
                               Mdl,
                               (PCHAR)MasterIrp->UserBuffer +
                               IoRuns[UnwindRunCount].Offset,
                               IoRuns[UnwindRunCount].ByteCount );

            //
            //  Get the first IRP stack location in the associated Irp
            //

            IoSetNextIrpStackLocation( Irp );
            IrpSp = IoGetCurrentIrpStackLocation( Irp );

            //
            //  Setup the Stack location to describe our read.
            //

            IrpSp->MajorFunction = IrpContext->MajorFunction;
            IrpSp->Parameters.Read.Length = IoRuns[UnwindRunCount].ByteCount;
            IrpSp->Parameters.Read.ByteOffset.QuadPart = IoRuns[UnwindRunCount].Vbo;

            //
            // Set up the completion routine address in our stack frame.
            //

            IoSetCompletionRoutine( Irp,
                                    Wait ?
                                    &FatMultiSyncCompletionRoutine :
                                    &FatMultiAsyncCompletionRoutine,
                                    Context,
                                    TRUE,
                                    TRUE,
                                    TRUE );

            //
            //  Setup the next IRP stack location in the associated Irp for the disk
            //  driver beneath us.
            //

            IrpSp = IoGetNextIrpStackLocation( Irp );

            //
            //  Setup the Stack location to do a read from the disk driver.
            //

            IrpSp->MajorFunction = IrpContext->MajorFunction;
            IrpSp->Parameters.Read.Length = IoRuns[UnwindRunCount].ByteCount;
            IrpSp->Parameters.Read.ByteOffset.QuadPart = IoRuns[UnwindRunCount].Lbo;

            //
            //  If this Irp is the result of a WriteThough operation,
            //  tell the device to write it through.
            //

            if (FlagOn( IrpContext->Flags, IRP_CONTEXT_FLAG_WRITE_THROUGH )) {

                SetFlag( IrpSp->Flags, SL_WRITE_THROUGH );
            }

            //
            //  If this I/O requires override verify, bypass the verify logic.
            //

            if (FlagOn( IrpContext->Flags, IRP_CONTEXT_FLAG_OVERRIDE_VERIFY )) {

                SetFlag( IrpSp->Flags, SL_OVERRIDE_VERIFY_VOLUME );
            }
        }

        //
        //  Now we no longer expect an exception.  If the driver raises, we
        //  must bugcheck, because we do not know how to recover from that
        //  case.
        //

        ExceptionExpected = FALSE;

        //
        //  We only need to set the associated IRP count in the master irp to
        //  make it a master IRP.  But we set the count to one more than our
        //  caller requested, because we do not want the I/O system to complete
        //  the I/O.  We also set our own count.
        //

        Context->IrpCount = MultipleIrpCount;
        MasterIrp->AssociatedIrp.IrpCount = MultipleIrpCount;

        if (Wait) {

            MasterIrp->AssociatedIrp.IrpCount += 1;
        }

        //
        //  Now that all the dangerous work is done, issue the read requests
        //

        for (UnwindRunCount = 0;
             UnwindRunCount < MultipleIrpCount;
             UnwindRunCount++) {

            Irp = IoRuns[UnwindRunCount].SavedIrp;

            DebugDoit( FatIoCallDriverCount += 1);

            //
            //  If IoCallDriver returns an error, it has completed the Irp
            //  and the error will be caught by our completion routines
            //  and dealt with as a normal IO error.
            //

            (VOID)FatLowLevelReadWrite( IrpContext,
                                        Vcb->TargetDeviceObject,
                                        Irp,
                                        Vcb );
        }

    } finally {

        ULONG i;

        DebugUnwind( FatMultipleAsync );

        //
        //  Only allocating the spinlock, making the associated Irps
        //  and allocating the Mdls can fail.
        //

        if ( AbnormalTermination() ) {

            //
            //  If the driver raised, we are hosed.  He is not supposed to raise,
            //  and it is impossible for us to figure out how to clean up.
            //

            if (!ExceptionExpected) {
                ASSERT( ExceptionExpected );
                FatBugCheck( 0, 0, 0 );
            }

            //
            //  Unwind
            //

            for (i = 0; i <= UnwindRunCount; i++) {

                if ( (Irp = IoRuns[i].SavedIrp) != NULL ) {

                    if ( Irp->MdlAddress != NULL ) {

                        IoFreeMdl( Irp->MdlAddress );
                    }

                    IoFreeIrp( Irp );
                }
            }
        }

        //
        //  And return to our caller
        //

        DebugTrace(-1, Dbg, "FatMultipleAsync -> VOID\n", 0);
    }

    return;
}


VOID
FatSingleAsync (
    IN PIRP_CONTEXT IrpContext,
    IN PVCB Vcb,
    IN LBO Lbo,
    IN ULONG ByteCount,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine reads or writes one or more contiguous sectors from a device
    asynchronously, and is used if there is only one read necessary to
    complete the IRP.  It implements the read by simply filling
    in the next stack frame in the Irp, and passing it on.  The transfer
    occurs to the single buffer originally specified in the user request.

Arguments:

    IrpContext->MajorFunction - Supplies either IRP_MJ_READ or IRP_MJ_WRITE.

    Vcb - Supplies the device to read

    Lbo - Supplies the starting Logical Byte Offset to begin reading from

    ByteCount - Supplies the number of bytes to read from the device

    Irp - Supplies the master Irp to associated with the async
          request.

Return Value:

    None.

--*/

{
    PIO_STACK_LOCATION IrpSp;

    PAGED_CODE();

    DebugTrace(+1, Dbg, "FatSingleAsync\n", 0);
    DebugTrace( 0, Dbg, "MajorFunction = %08lx\n", IrpContext->MajorFunction );
    DebugTrace( 0, Dbg, "Vcb           = %08lx\n", Vcb );
    DebugTrace( 0, Dbg, "Lbo           = %08lx\n", Lbo);
    DebugTrace( 0, Dbg, "ByteCount     = %08lx\n", ByteCount);
    DebugTrace( 0, Dbg, "Irp           = %08lx\n", Irp );

    //
    //  If this I/O originating during FatVerifyVolume, bypass the
    //  verify logic.
    //

    if (Vcb->VerifyThread == KeGetCurrentThread()) {

        SetFlag( IrpContext->Flags, IRP_CONTEXT_FLAG_OVERRIDE_VERIFY );
    }

    //
    //  Set up the completion routine address in our stack frame.
    //

    IoSetCompletionRoutine( Irp,
                            FlagOn(IrpContext->Flags, IRP_CONTEXT_FLAG_WAIT) ?
                            &FatSingleSyncCompletionRoutine :
                            &FatSingleAsyncCompletionRoutine,
                            IrpContext->FatIoContext,
                            TRUE,
                            TRUE,
                            TRUE );

    //
    //  Setup the next IRP stack location in the associated Irp for the disk
    //  driver beneath us.
    //

    IrpSp = IoGetNextIrpStackLocation( Irp );

    //
    //  Setup the Stack location to do a read from the disk driver.
    //

    IrpSp->MajorFunction = IrpContext->MajorFunction;
    IrpSp->Parameters.Read.Length = ByteCount;
    IrpSp->Parameters.Read.ByteOffset.QuadPart = Lbo;

    //
    //  If this Irp is the result of a WriteThough operation,
    //  tell the device to write it through.
    //

    if (FlagOn( IrpContext->Flags, IRP_CONTEXT_FLAG_WRITE_THROUGH )) {

        SetFlag( IrpSp->Flags, SL_WRITE_THROUGH );
    }

    //
    //  If this I/O requires override verify, bypass the verify logic.
    //
    
    if (FlagOn( IrpContext->Flags, IRP_CONTEXT_FLAG_OVERRIDE_VERIFY )) {
    
        SetFlag( IrpSp->Flags, SL_OVERRIDE_VERIFY_VOLUME );
    }

    //
    //  Issue the read request
    //

    DebugDoit( FatIoCallDriverCount += 1);

    //
    //  If IoCallDriver returns an error, it has completed the Irp
    //  and the error will be caught by our completion routines
    //  and dealt with as a normal IO error.
    //

    (VOID)FatLowLevelReadWrite( IrpContext,
                                Vcb->TargetDeviceObject,
                                Irp,
                                Vcb );

    //
    //  And return to our caller
    //

    DebugTrace(-1, Dbg, "FatSingleAsync -> VOID\n", 0);

    return;
}


VOID
FatSingleNonAlignedSync (
    IN PIRP_CONTEXT IrpContext,
    IN PVCB Vcb,
    IN PUCHAR Buffer,
    IN LBO Lbo,
    IN ULONG ByteCount,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine reads or writes one or more contiguous sectors from a device
    Synchronously, and does so to a buffer that must come from non paged
    pool.  It saves a pointer to the Irp's original Mdl, and creates a new
    one describing the given buffer.  It implements the read by simply filling
    in the next stack frame in the Irp, and passing it on.  The transfer
    occurs to the single buffer originally specified in the user request.

Arguments:

    IrpContext->MajorFunction - Supplies either IRP_MJ_READ or IRP_MJ_WRITE.

    Vcb - Supplies the device to read

    Buffer - Supplies a buffer from non-paged pool.

    Lbo - Supplies the starting Logical Byte Offset to begin reading from

    ByteCount - Supplies the number of bytes to read from the device

    Irp - Supplies the master Irp to associated with the async
          request.

Return Value:

    None.

--*/

{
    PIO_STACK_LOCATION IrpSp;

    PMDL Mdl;
    PMDL SavedMdl;

    PAGED_CODE();

    DebugTrace(+1, Dbg, "FatSingleNonAlignedAsync\n", 0);
    DebugTrace( 0, Dbg, "MajorFunction = %08lx\n", IrpContext->MajorFunction );
    DebugTrace( 0, Dbg, "Vcb           = %08lx\n", Vcb );
    DebugTrace( 0, Dbg, "Buffer        = %08lx\n", Buffer );
    DebugTrace( 0, Dbg, "Lbo           = %08lx\n", Lbo);
    DebugTrace( 0, Dbg, "ByteCount     = %08lx\n", ByteCount);
    DebugTrace( 0, Dbg, "Irp           = %08lx\n", Irp );

    //
    //  Create a new Mdl describing the buffer, saving the current one in the
    //  Irp
    //

    SavedMdl = Irp->MdlAddress;

    Irp->MdlAddress = 0;

    Mdl = IoAllocateMdl( Buffer,
                         ByteCount,
                         FALSE,
                         FALSE,
                         Irp );

    if (Mdl == NULL) {

        Irp->MdlAddress = SavedMdl;

        FatRaiseStatus( IrpContext, STATUS_INSUFFICIENT_RESOURCES );
    }

    //
    //  Lock the new Mdl in memory.
    //

    try {

        MmProbeAndLockPages( Mdl, KernelMode, IoWriteAccess );

    } finally {

        if ( AbnormalTermination() ) {

            IoFreeMdl( Mdl );
            Irp->MdlAddress = SavedMdl;
        }
    }

    //
    // Set up the completion routine address in our stack frame.
    //

    IoSetCompletionRoutine( Irp,
                            &FatSingleSyncCompletionRoutine,
                            IrpContext->FatIoContext,
                            TRUE,
                            TRUE,
                            TRUE );

    //
    //  Setup the next IRP stack location in the associated Irp for the disk
    //  driver beneath us.
    //

    IrpSp = IoGetNextIrpStackLocation( Irp );

    //
    //  Setup the Stack location to do a read from the disk driver.
    //

    IrpSp->MajorFunction = IrpContext->MajorFunction;
    IrpSp->Parameters.Read.Length = ByteCount;
    IrpSp->Parameters.Read.ByteOffset.QuadPart = Lbo;

    //
    //  If this I/O originating during FatVerifyVolume, bypass the
    //  verify logic.
    //

    if (Vcb->VerifyThread == KeGetCurrentThread()) {

        SetFlag( IrpContext->Flags, IRP_CONTEXT_FLAG_OVERRIDE_VERIFY );
    }

    //
    //  If this I/O requires override verify, bypass the verify logic.
    //
    
    if (FlagOn( IrpContext->Flags, IRP_CONTEXT_FLAG_OVERRIDE_VERIFY )) {
    
        SetFlag( IrpSp->Flags, SL_OVERRIDE_VERIFY_VOLUME );
    }

    //
    //  Issue the read request
    //

    DebugDoit( FatIoCallDriverCount += 1);

    //
    //  If IoCallDriver returns an error, it has completed the Irp
    //  and the error will be caught by our completion routines
    //  and dealt with as a normal IO error.
    //

    try {

        (VOID)FatLowLevelReadWrite( IrpContext,
                                    Vcb->TargetDeviceObject,
                                    Irp,
                                    Vcb );

        FatWaitSync( IrpContext );

    } finally {

        MmUnlockPages( Mdl );
        IoFreeMdl( Mdl );
        Irp->MdlAddress = SavedMdl;
    }

    //
    //  And return to our caller
    //

    DebugTrace(-1, Dbg, "FatSingleNonAlignedSync -> VOID\n", 0);

    return;
}


VOID
FatWaitSync (
    IN PIRP_CONTEXT IrpContext
    )

/*++

Routine Description:

    This routine waits for one or more previously started I/O requests
    from the above routines, by simply waiting on the event.

Arguments:

Return Value:

    None

--*/

{
    PAGED_CODE();

    DebugTrace(+1, Dbg, "FatWaitSync, Context = %08lx\n", IrpContext->FatIoContext );

    KeWaitForSingleObject( &IrpContext->FatIoContext->Wait.SyncEvent,
                           Executive, KernelMode, FALSE, NULL );

    KeClearEvent( &IrpContext->FatIoContext->Wait.SyncEvent );

    DebugTrace(-1, Dbg, "FatWaitSync -> VOID\n", 0 );
}


//
// Internal Support Routine
//

NTSTATUS
FatMultiSyncCompletionRoutine (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Contxt
    )

/*++

Routine Description:

    This is the completion routine for all reads and writes started via
    FatRead/WriteMultipleAsynch.  It must synchronize its operation for
    multiprocessor environments with itself on all other processors, via
    a spin lock found via the Context parameter.

    The completion routine has the following responsibilities:

        If the individual request was completed with an error, then
        this completion routine must see if this is the first error
        (essentially by Vbo), and if so it must correctly reduce the
        byte count and remember the error status in the Context.

        If the IrpCount goes to 1, then it sets the event in the Context
        parameter to signal the caller that all of the asynch requests
        are done.

Arguments:

    DeviceObject - Pointer to the file system device object.

    Irp - Pointer to the associated Irp which is being completed.  (This
          Irp will no longer be accessible after this routine returns.)

    Contxt - The context parameter which was specified for all of
             the multiple asynch I/O requests for this MasterIrp.

Return Value:

    The routine returns STATUS_MORE_PROCESSING_REQUIRED so that we can
    immediately complete the Master Irp without being in a race condition
    with the IoCompleteRequest thread trying to decrement the IrpCount in
    the Master Irp.

--*/

{

    PFAT_IO_CONTEXT Context = Contxt;
    PIRP MasterIrp = Context->MasterIrp;

    DebugTrace(+1, Dbg, "FatMultiSyncCompletionRoutine, Context = %08lx\n", Context );

    //
    //  If we got an error (or verify required), remember it in the Irp
    //

    MasterIrp = Context->MasterIrp;

    if (!NT_SUCCESS( Irp->IoStatus.Status )) {

        ASSERT( NT_SUCCESS( FatAssertNotStatus ) || Irp->IoStatus.Status != FatAssertNotStatus );

#ifdef SYSCACHE_COMPILE
        DbgPrint( "FAT SYSCACHE: MultiSync (IRP %08x for Master %08x) -> %08x\n", Irp, MasterIrp, Irp->IoStatus );
#endif

        MasterIrp->IoStatus = Irp->IoStatus;
    }

    ASSERT( !(NT_SUCCESS( Irp->IoStatus.Status ) && Irp->IoStatus.Information == 0 ));

    //
    //  We must do this here since IoCompleteRequest won't get a chance
    //  on this associated Irp.
    //

    IoFreeMdl( Irp->MdlAddress );
    IoFreeIrp( Irp );

    if (InterlockedDecrement(&Context->IrpCount) == 0) {

        FatDoCompletionZero( MasterIrp, Context );
        KeSetEvent( &Context->Wait.SyncEvent, 0, FALSE );
    }

    DebugTrace(-1, Dbg, "FatMultiSyncCompletionRoutine -> SUCCESS\n", 0 );

    UNREFERENCED_PARAMETER( DeviceObject );

    return STATUS_MORE_PROCESSING_REQUIRED;
}


//
// Internal Support Routine
//

NTSTATUS
FatMultiAsyncCompletionRoutine (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Contxt
    )

/*++

Routine Description:

    This is the completion routine for all reads and writes started via
    FatRead/WriteMultipleAsynch.  It must synchronize its operation for
    multiprocessor environments with itself on all other processors, via
    a spin lock found via the Context parameter.

    The completion routine has has the following responsibilities:

        If the individual request was completed with an error, then
        this completion routine must see if this is the first error
        (essentially by Vbo), and if so it must correctly reduce the
        byte count and remember the error status in the Context.

        If the IrpCount goes to 1, then it sets the event in the Context
        parameter to signal the caller that all of the asynch requests
        are done.

Arguments:

    DeviceObject - Pointer to the file system device object.

    Irp - Pointer to the associated Irp which is being completed.  (This
          Irp will no longer be accessible after this routine returns.)

    Contxt - The context parameter which was specified for all of
             the multiple asynch I/O requests for this MasterIrp.

Return Value:

    The routine returns STATUS_MORE_PROCESSING_REQUIRED so that we can
    immediately complete the Master Irp without being in a race condition
    with the IoCompleteRequest thread trying to decrement the IrpCount in
    the Master Irp.

--*/

{

    PFAT_IO_CONTEXT Context = Contxt;
    PIRP MasterIrp = Context->MasterIrp;

    DebugTrace(+1, Dbg, "FatMultiAsyncCompletionRoutine, Context = %08lx\n", Context );

    //
    //  If we got an error (or verify required), remember it in the Irp
    //

    MasterIrp = Context->MasterIrp;

    if (!NT_SUCCESS( Irp->IoStatus.Status )) {

        ASSERT( NT_SUCCESS( FatAssertNotStatus ) || Irp->IoStatus.Status != FatAssertNotStatus );

#ifdef SYSCACHE_COMPILE
        DbgPrint( "FAT SYSCACHE: MultiAsync (IRP %08x for Master %08x) -> %08x\n", Irp, MasterIrp, Irp->IoStatus );
#endif

        MasterIrp->IoStatus = Irp->IoStatus;
    
    }

    ASSERT( !(NT_SUCCESS( Irp->IoStatus.Status ) && Irp->IoStatus.Information == 0 ));
    
    if (InterlockedDecrement(&Context->IrpCount) == 0) {

        FatDoCompletionZero( MasterIrp, Context );

        if (NT_SUCCESS(MasterIrp->IoStatus.Status)) {

            MasterIrp->IoStatus.Information =
                Context->Wait.Async.RequestedByteCount;

            ASSERT(MasterIrp->IoStatus.Information != 0);

            //
            //  Now if this wasn't PagingIo, set either the read or write bit.
            //

            if (!FlagOn(MasterIrp->Flags, IRP_PAGING_IO)) {

                SetFlag( Context->Wait.Async.FileObject->Flags,
                         IoGetCurrentIrpStackLocation(MasterIrp)->MajorFunction == IRP_MJ_READ ?
                         FO_FILE_FAST_IO_READ : FO_FILE_MODIFIED );
            }
        }

        //
        //  If this was a special async write, decrement the count.  Set the
        //  event if this was the final outstanding I/O for the file.  We will
        //  also want to queue an APC to deal with any error conditionions.
        //

        if ((Context->Wait.Async.NonPagedFcb) &&
            (ExInterlockedAddUlong( &Context->Wait.Async.NonPagedFcb->OutstandingAsyncWrites,
                                    0xffffffff,
                                    &FatData.GeneralSpinLock ) == 1)) {

            KeSetEvent( Context->Wait.Async.NonPagedFcb->OutstandingAsyncEvent, 0, FALSE );
        }

        //
        //  Now release the resources.
        //

        if (Context->Wait.Async.Resource != NULL) {

            ExReleaseResourceForThreadLite( Context->Wait.Async.Resource,
                                        Context->Wait.Async.ResourceThreadId );
        }

        if (Context->Wait.Async.Resource2 != NULL) {

            ExReleaseResourceForThreadLite( Context->Wait.Async.Resource2,
                                        Context->Wait.Async.ResourceThreadId );
        }

        //
        //  Mark the master Irp pending
        //

        IoMarkIrpPending( MasterIrp );

        //
        //  and finally, free the context record.
        //

        ExFreePool( Context );
    }

    DebugTrace(-1, Dbg, "FatMultiAsyncCompletionRoutine -> SUCCESS\n", 0 );

    UNREFERENCED_PARAMETER( DeviceObject );

    return STATUS_SUCCESS;
}


NTSTATUS
FatPagingFileErrorHandler (
    IN PIRP Irp,
    IN PKEVENT Event OPTIONAL
    )

/*++

Routine Description:

    This routine attempts to guarantee that the media is marked dirty
    with the surface test bit if a paging file IO fails.
    
    The work done here has several basic problems
    
        1) when paging file writes start failing, this is a good sign
           that the rest of the system is about to fall down around us
           
        2) it has no forward progress guarantee
        
    With Whistler, it is actually quite intentional that we're rejiggering
    the paging file write path to make forward progress at all times.  This
    means that the cases where it *does* fail, we're truly seeing media errors
    and this is probably going to mean the paging file is going to stop working
    very soon.
    
    It'd be nice to make this guarantee progress.  It would need
    
        1) a guaranteed worker thread which can only be used by items which
           will make forward progress (i.e., not block out this one)
           
        2) the virtual volume file's pages containing the boot sector and
           1st FAT entry would have to be pinned resident and have a guaranteed
           mapping address
           
        3) mark volume would have to have a stashed irp/mdl and roll the write
           irp, or use a generalized mechanism to guarantee issue of the irp
           
        4) the lower stack would have to guarantee progress
        
    Of these, 1 and 4 may actually exist shortly.
        
Arguments:

    Irp - Pointer to the associated Irp which is being failed.

    Event - Pointer to optional event to be signalled instead of completing
        the IRP

Return Value:

    Returns STATUS_MORE_PROCESSING_REQUIRED if we managed to queue off the workitem,
    STATUS_SUCCESS otherwise.

--*/

{
    NTSTATUS Status;

    //
    //  If this was a media error, we want to chkdsk /r the next time we boot.
    //

    if (FsRtlIsTotalDeviceFailure(Irp->IoStatus.Status)) {

        Status = STATUS_SUCCESS;

    } else {

        PCLEAN_AND_DIRTY_VOLUME_PACKET Packet;

        //
        //  We are going to try to mark the volume needing recover.
        //  If we can't get pool, oh well....
        //

        Packet = ExAllocatePoolWithTag(NonPagedPool, sizeof(CLEAN_AND_DIRTY_VOLUME_PACKET), ' taF');

        if ( Packet ) {

            Packet->Vcb = &((PVOLUME_DEVICE_OBJECT)IoGetCurrentIrpStackLocation(Irp)->DeviceObject)->Vcb;
            Packet->Irp = Irp;
            Packet->Event = Event;

            ExInitializeWorkItem( &Packet->Item,
                                  &FatFspMarkVolumeDirtyWithRecover,
                                  Packet );

            ExQueueWorkItem( &Packet->Item, CriticalWorkQueue );

            Status = STATUS_MORE_PROCESSING_REQUIRED;

        } else {

            Status = STATUS_SUCCESS;
        }
    }

    return Status;
}


//
// Internal Support Routine
//

NTSTATUS
FatPagingFileCompletionRoutineCatch (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Contxt
    )

/*++

Routine Description:

    This is the completion routine for all reads and writes started via
    FatPagingFileIo that reuse the master irp (that we have to catch
    on the way back).  It is always invoked.

    The completion routine has has the following responsibility:

        If the error implies a media problem, it enqueues a
        worker item to write out the dirty bit so that the next
        time we run we will do a autochk /r.  This is not forward
        progress guaranteed at the moment.
        
        Clean up the Mdl used for this partial request.
        
    Note that if the Irp is failing, the error code is already where
    we want it.

Arguments:

    DeviceObject - Pointer to the file system device object.

    Irp - Pointer to the associated Irp which is being completed.  (This
          Irp will no longer be accessible after this routine returns.)

    MasterIrp - Pointer to the master Irp.

Return Value:

    Always returns STATUS_MORE_PROCESSING_REQUIRED.

--*/

{
    PFAT_PAGING_FILE_CONTEXT Context = (PFAT_PAGING_FILE_CONTEXT) Contxt;

    DebugTrace(+1, Dbg, "FatPagingFileCompletionRoutineCatch, Context = %08lx\n", Context );
    
    //
    //  Cleanup the existing Mdl, perhaps by returning the reserve.
    //

    if (Irp->MdlAddress == FatReserveMdl) {

        MmPrepareMdlForReuse( Irp->MdlAddress );
        KeSetEvent( &FatReserveEvent, 0, FALSE );
    
    } else {

        IoFreeMdl( Irp->MdlAddress );
    }

    //
    //  Restore the original Mdl.
    //

    Irp->MdlAddress = Context->RestoreMdl;

    DebugTrace(-1, Dbg, "FatPagingFileCompletionRoutine => (done)\n", 0 );

    //
    //  If the IRP is succeeding or the failure handler did not post off the
    //  completion, we're done and should set the event to let the master
    //  know the IRP is his again.
    //

    if (NT_SUCCESS( Irp->IoStatus.Status ) ||
        FatPagingFileErrorHandler( Irp, &Context->Event ) == STATUS_SUCCESS) {

        KeSetEvent( &Context->Event, 0, FALSE );
    }

    return STATUS_MORE_PROCESSING_REQUIRED;
    
}


//
// Internal Support Routine
//

NTSTATUS
FatPagingFileCompletionRoutine (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID MasterIrp
    )

/*++

Routine Description:

    This is the completion routine for all reads and writes started via
    FatPagingFileIo.  It should only be invoked on error or cancel.

    The completion routine has has the following responsibility:

        Since the individual request was completed with an error,
        this completion routine must stuff it into the master irp.

        If the error implies a media problem, it also enqueues a
        worker item to write out the dirty bit so that the next
        time we run we will do a autochk /r

Arguments:

    DeviceObject - Pointer to the file system device object.

    Irp - Pointer to the associated Irp which is being completed.  (This
          Irp will no longer be accessible after this routine returns.)

    MasterIrp - Pointer to the master Irp.

Return Value:

    Always returns STATUS_SUCCESS.

--*/

{
    NTSTATUS Status;

    DebugTrace(+1, Dbg, "FatPagingFileCompletionRoutine, MasterIrp = %08lx\n", MasterIrp );

    //
    //  If we got an error (or verify required), remember it in the Irp
    //

    ASSERT( !NT_SUCCESS( Irp->IoStatus.Status ));

    //
    //  If we were invoked with an assoicated Irp, copy the error over.
    //

    if (Irp != MasterIrp) {

        ((PIRP)MasterIrp)->IoStatus = Irp->IoStatus;
    }

    DebugTrace(-1, Dbg, "FatPagingFileCompletionRoutine => (done)\n", 0 );

    UNREFERENCED_PARAMETER( DeviceObject );

    return FatPagingFileErrorHandler( Irp, NULL );
}


//
// Internal Support Routine
//

NTSTATUS
FatSpecialSyncCompletionRoutine (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Contxt
    )

/*++

Routine Description:

    This is the completion routine for a special set of sub irps
    that have to work at APC level.

    The completion routine has has the following responsibilities:

        It sets the event passed as the context to signal that the
        request is done.

    By doing this, the caller will be released before final APC
    completion with knowledge that the IRP is finished.  Final
    completion will occur at an indeterminate time after this
    occurs, and by using this completion routine the caller expects
    to not have any output or status returned.  A junk user Iosb
    should be used to capture the status without forcing Io to take
    an exception on NULL.

Arguments:

    DeviceObject - Pointer to the file system device object.

    Irp - Pointer to the Irp for this request.  (This Irp will no longer
    be accessible after this routine returns.)

    Contxt - The context parameter which was specified in the call to
             FatRead/WriteSingleAsynch.

Return Value:

    Currently always returns STATUS_SUCCESS.

--*/

{
    PKEVENT Event = (PKEVENT)Contxt;

    DebugTrace(+1, Dbg, "FatSpecialSyncCompletionRoutine, Context = %08lx\n", Contxt );

    KeSetEvent( Event, 0, FALSE );

    DebugTrace(-1, Dbg, "FatSpecialSyncCompletionRoutine -> STATUS_SUCCESS\n", 0 );

    UNREFERENCED_PARAMETER( DeviceObject );

    return STATUS_SUCCESS;
}


//
// Internal Support Routine
//

NTSTATUS
FatSingleSyncCompletionRoutine (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Contxt
    )

/*++

Routine Description:

    This is the completion routine for all reads and writes started via
    FatRead/WriteSingleAsynch.

    The completion routine has has the following responsibilities:

        Copy the I/O status from the Irp to the Context, since the Irp
        will no longer be accessible.

        It sets the event in the Context parameter to signal the caller
        that all of the asynch requests are done.

Arguments:

    DeviceObject - Pointer to the file system device object.

    Irp - Pointer to the Irp for this request.  (This Irp will no longer
    be accessible after this routine returns.)

    Contxt - The context parameter which was specified in the call to
             FatRead/WriteSingleAsynch.

Return Value:

    Currently always returns STATUS_SUCCESS.

--*/

{
    PFAT_IO_CONTEXT Context = Contxt;

    DebugTrace(+1, Dbg, "FatSingleSyncCompletionRoutine, Context = %08lx\n", Context );

    FatDoCompletionZero( Irp, Context );

    if (!NT_SUCCESS( Irp->IoStatus.Status )) {

        ASSERT( NT_SUCCESS( FatAssertNotStatus ) || Irp->IoStatus.Status != FatAssertNotStatus );
    }

    ASSERT( !(NT_SUCCESS( Irp->IoStatus.Status ) && Irp->IoStatus.Information == 0 ));

    KeSetEvent( &Context->Wait.SyncEvent, 0, FALSE );

    DebugTrace(-1, Dbg, "FatSingleSyncCompletionRoutine -> STATUS_MORE_PROCESSING_REQUIRED\n", 0 );

    UNREFERENCED_PARAMETER( DeviceObject );

    return STATUS_MORE_PROCESSING_REQUIRED;
}


//
// Internal Support Routine
//

NTSTATUS
FatSingleAsyncCompletionRoutine (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Contxt
    )

/*++

Routine Description:

    This is the completion routine for all reads and writes started via
    FatRead/WriteSingleAsynch.

    The completion routine has has the following responsibilities:

        Copy the I/O status from the Irp to the Context, since the Irp
        will no longer be accessible.

        It sets the event in the Context parameter to signal the caller
        that all of the asynch requests are done.

Arguments:

    DeviceObject - Pointer to the file system device object.

    Irp - Pointer to the Irp for this request.  (This Irp will no longer
    be accessible after this routine returns.)

    Contxt - The context parameter which was specified in the call to
             FatRead/WriteSingleAsynch.

Return Value:

    Currently always returns STATUS_SUCCESS.

--*/

{
    PFAT_IO_CONTEXT Context = Contxt;

    DebugTrace(+1, Dbg, "FatSingleAsyncCompletionRoutine, Context = %08lx\n", Context );

    //
    //  Fill in the information field correctedly if this worked.
    //

    FatDoCompletionZero( Irp, Context );

    if (NT_SUCCESS(Irp->IoStatus.Status)) {

        ASSERT( Irp->IoStatus.Information != 0 );
        Irp->IoStatus.Information = Context->Wait.Async.RequestedByteCount;
        ASSERT( Irp->IoStatus.Information != 0 );

        //
        //  Now if this wasn't PagingIo, set either the read or write bit.
        //

        if (!FlagOn(Irp->Flags, IRP_PAGING_IO)) {

            SetFlag( Context->Wait.Async.FileObject->Flags,
                     IoGetCurrentIrpStackLocation(Irp)->MajorFunction == IRP_MJ_READ ?
                     FO_FILE_FAST_IO_READ : FO_FILE_MODIFIED );
        }

    } else {

        ASSERT( NT_SUCCESS( FatAssertNotStatus ) || Irp->IoStatus.Status != FatAssertNotStatus );
    
#ifdef SYSCACHE_COMPILE
        DbgPrint( "FAT SYSCACHE: SingleAsync (IRP %08x) -> %08x\n", Irp, Irp->IoStatus );
#endif

    }

    //
    //  If this was a special async write, decrement the count.  Set the
    //  event if this was the final outstanding I/O for the file.  We will
    //  also want to queue an APC to deal with any error conditionions.
    //

    if ((Context->Wait.Async.NonPagedFcb) &&
        (ExInterlockedAddUlong( &Context->Wait.Async.NonPagedFcb->OutstandingAsyncWrites,
                                0xffffffff,
                                &FatData.GeneralSpinLock ) == 1)) {

        KeSetEvent( Context->Wait.Async.NonPagedFcb->OutstandingAsyncEvent, 0, FALSE );
    }

    //
    //  Now release the resources
    //

    if (Context->Wait.Async.Resource != NULL) {

        ExReleaseResourceForThreadLite( Context->Wait.Async.Resource,
                                    Context->Wait.Async.ResourceThreadId );
    }
    
    if (Context->Wait.Async.Resource2 != NULL) {

        ExReleaseResourceForThreadLite( Context->Wait.Async.Resource2,
                                    Context->Wait.Async.ResourceThreadId );
    }

    //
    //  Mark the Irp pending
    //

    IoMarkIrpPending( Irp );

    //
    //  and finally, free the context record.
    //

    ExFreePool( Context );

    DebugTrace(-1, Dbg, "FatSingleAsyncCompletionRoutine -> STATUS_MORE_PROCESSING_REQUIRED\n", 0 );

    UNREFERENCED_PARAMETER( DeviceObject );

    return STATUS_SUCCESS;
}


VOID
FatLockUserBuffer (
    IN PIRP_CONTEXT IrpContext,
    IN OUT PIRP Irp,
    IN LOCK_OPERATION Operation,
    IN ULONG BufferLength
    )

/*++

Routine Description:

    This routine locks the specified buffer for the specified type of
    access.  The file system requires this routine since it does not
    ask the I/O system to lock its buffers for direct I/O.  This routine
    may only be called from the Fsd while still in the user context.

    Note that this is the *input/output* buffer.

Arguments:

    Irp - Pointer to the Irp for which the buffer is to be locked.

    Operation - IoWriteAccess for read operations, or IoReadAccess for
                write operations.

    BufferLength - Length of user buffer.

Return Value:

    None

--*/

{
    PMDL Mdl = NULL;

    PAGED_CODE();

    if (Irp->MdlAddress == NULL) {

        //
        // Allocate the Mdl, and Raise if we fail.
        //

        Mdl = IoAllocateMdl( Irp->UserBuffer, BufferLength, FALSE, FALSE, Irp );

        if (Mdl == NULL) {

            FatRaiseStatus( IrpContext, STATUS_INSUFFICIENT_RESOURCES );
        }

        //
        // Now probe the buffer described by the Irp.  If we get an exception,
        // deallocate the Mdl and return the appropriate "expected" status.
        //

        try {

            MmProbeAndLockPages( Mdl,
                                 Irp->RequestorMode,
                                 Operation );

        } except(EXCEPTION_EXECUTE_HANDLER) {

            NTSTATUS Status;

            Status = GetExceptionCode();

            IoFreeMdl( Mdl );
            Irp->MdlAddress = NULL;

            FatRaiseStatus( IrpContext,
                            FsRtlIsNtstatusExpected(Status) ? Status : STATUS_INVALID_USER_BUFFER );
        }
    }

    UNREFERENCED_PARAMETER( IrpContext );
}


PVOID
FatMapUserBuffer (
    IN PIRP_CONTEXT IrpContext,
    IN OUT PIRP Irp
    )

/*++

Routine Description:

    This routine conditionally maps the user buffer for the current I/O
    request in the specified mode.  If the buffer is already mapped, it
    just returns its address.
    
    Note that this is the *input/output* buffer.

Arguments:

    Irp - Pointer to the Irp for the request.

Return Value:

    Mapped address

--*/

{
    UNREFERENCED_PARAMETER( IrpContext );

    PAGED_CODE();

    //
    // If there is no Mdl, then we must be in the Fsd, and we can simply
    // return the UserBuffer field from the Irp.
    //

    if (Irp->MdlAddress == NULL) {

        return Irp->UserBuffer;
    
    } else {

        PVOID Address = MmGetSystemAddressForMdlSafe( Irp->MdlAddress, NormalPagePriority );

        if (Address == NULL) {

            ExRaiseStatus( STATUS_INSUFFICIENT_RESOURCES );
        }

        return Address;
    }
}


PVOID
FatBufferUserBuffer (
    IN PIRP_CONTEXT IrpContext,
    IN OUT PIRP Irp,
    IN ULONG BufferLength
    )

/*++

Routine Description:

    This routine conditionally buffers the user buffer for the current I/O
    request.  If the buffer is already buffered, it just returns its address.
    
    Note that this is the *input* buffer.

Arguments:

    Irp - Pointer to the Irp for the request.

    BufferLength - Length of user buffer.
    
Return Value:

    Buffered address.

--*/

{
    PUCHAR UserBuffer;

    UNREFERENCED_PARAMETER( IrpContext );

    PAGED_CODE();

    //
    //  Handle the no buffer case.
    //
    
    if (BufferLength == 0) {

        return NULL;
    }
    
    //
    //  If there is no system buffer we must have been supplied an Mdl
    //  describing the users input buffer, which we will now snapshot.
    //

    if (Irp->AssociatedIrp.SystemBuffer == NULL) {

        UserBuffer = FatMapUserBuffer( IrpContext, Irp );

        Irp->AssociatedIrp.SystemBuffer = FsRtlAllocatePoolWithQuotaTag( NonPagedPool,
                                                                         BufferLength,
                                                                         TAG_IO_USER_BUFFER );

        //
        // Set the flags so that the completion code knows to deallocate the
        // buffer.
        //

        Irp->Flags |= (IRP_BUFFERED_IO | IRP_DEALLOCATE_BUFFER);

        try {

            RtlCopyMemory( Irp->AssociatedIrp.SystemBuffer,
                           UserBuffer,
                           BufferLength );

        } except (EXCEPTION_EXECUTE_HANDLER) {
              
              NTSTATUS Status;
  
              Status = GetExceptionCode();
              FatRaiseStatus( IrpContext,
                              FsRtlIsNtstatusExpected(Status) ? Status : STATUS_INVALID_USER_BUFFER );
        }
    }
        
    return Irp->AssociatedIrp.SystemBuffer;
}


NTSTATUS
FatToggleMediaEjectDisable (
    IN PIRP_CONTEXT IrpContext,
    IN PVCB Vcb,
    IN BOOLEAN PreventRemoval
    )

/*++

Routine Description:

    The routine either enables or disables the eject button on removable
    media.

Arguments:

    Vcb - Descibes the volume to operate on

    PreventRemoval - TRUE if we should disable the media eject button.  FALSE
        if we want to enable it.

Return Value:

    Status of the operation.

--*/

{
    PIRP Irp;
    KEVENT Event;
    KIRQL SavedIrql;
    NTSTATUS Status;
    IO_STATUS_BLOCK Iosb;
    PREVENT_MEDIA_REMOVAL Prevent;

    //
    //  If PreventRemoval is the same as VCB_STATE_FLAG_REMOVAL_PREVENTED,
    //  no-op this call, otherwise toggle the state of the flag.
    //

    KeAcquireSpinLock( &FatData.GeneralSpinLock, &SavedIrql );

    if ((PreventRemoval ^
         BooleanFlagOn(Vcb->VcbState, VCB_STATE_FLAG_REMOVAL_PREVENTED)) == 0) {

        KeReleaseSpinLock( &FatData.GeneralSpinLock, SavedIrql );

        return STATUS_SUCCESS;

    } else {

        Vcb->VcbState ^= VCB_STATE_FLAG_REMOVAL_PREVENTED;

        KeReleaseSpinLock( &FatData.GeneralSpinLock, SavedIrql );
    }

    Prevent.PreventMediaRemoval = PreventRemoval;

    KeInitializeEvent( &Event, NotificationEvent, FALSE );

    //
    //  We build this IRP using a junk Iosb that will receive the final
    //  completion status since we won't be around for it.
    //

    Irp = IoBuildDeviceIoControlRequest( IOCTL_DISK_MEDIA_REMOVAL,
                                         Vcb->TargetDeviceObject,
                                         &Prevent,
                                         sizeof(PREVENT_MEDIA_REMOVAL),
                                         NULL,
                                         0,
                                         FALSE,
                                         NULL,
                                         &Iosb );

    if ( Irp != NULL ) {

        //
        //  Use our special completion routine which will remove the requirement that
        //  the caller must be below APC level.  All it tells us is that the Irp got
        //  back, but will not tell us if it was succesful or not.  We don't care,
        //  and there is of course no fallback if the attempt to prevent removal
        //  doesn't work for some mysterious reason.
        //
        //  Normally, all IO is done at passive level. However, MM needs to be able
        //  to issue IO with fast mutexes locked down, which raises us to APC.  The
        //  overlying IRP is set up to complete in yet another magical fashion even
        //  though APCs are disabled, and any IRPage we do in these cases has to do
        //  the same.  Marking media dirty (and toggling eject state) is one.
        //

        IoSetCompletionRoutine( Irp,
                                FatSpecialSyncCompletionRoutine,
                                &Event,
                                TRUE,
                                TRUE,
                                TRUE );

        Status = IoCallDriver( Vcb->TargetDeviceObject, Irp );

        if (Status == STATUS_PENDING) {
            
            (VOID) KeWaitForSingleObject( &Event,
                                          Executive,
                                          KernelMode,
                                          FALSE,
                                          NULL );

            Status = Iosb.Status;
        }

        return Status;
    }

    return STATUS_INSUFFICIENT_RESOURCES;
}


NTSTATUS
FatPerformDevIoCtrl (
    IN PIRP_CONTEXT IrpContext,
    IN ULONG IoControlCode,
    IN PDEVICE_OBJECT Device,
    OUT PVOID OutputBuffer OPTIONAL,
    IN ULONG OutputBufferLength,
    IN BOOLEAN InternalDeviceIoControl,
    IN BOOLEAN OverrideVerify,
    OUT PIO_STATUS_BLOCK Iosb OPTIONAL
    )

/*++

Routine Description:

    This routine is called to perform DevIoCtrl functions internally within
    the filesystem.  We take the status from the driver and return it to our
    caller.

Arguments:

    IoControlCode - Code to send to driver.

    Device - This is the device to send the request to.

    OutPutBuffer - Pointer to output buffer.

    OutputBufferLength - Length of output buffer above.

    InternalDeviceIoControl - Indicates if this is an internal or external
        Io control code.

    OverrideVerify - Indicates if we should tell the driver not to return
        STATUS_VERIFY_REQUIRED for mount and verify.

    Iosb - If specified, we return the results of the operation here.

Return Value:

    NTSTATUS - Status returned by next lower driver.

--*/

{
    NTSTATUS Status;
    PIRP Irp;
    KEVENT Event;
    IO_STATUS_BLOCK LocalIosb;
    PIO_STATUS_BLOCK IosbToUse = &LocalIosb;

    PAGED_CODE();

    //
    //  Check if the user gave us an Iosb.
    //

    if (ARGUMENT_PRESENT( Iosb )) {

        IosbToUse = Iosb;
    }

    IosbToUse->Status = 0;
    IosbToUse->Information = 0;

    KeInitializeEvent( &Event, NotificationEvent, FALSE );

    Irp = IoBuildDeviceIoControlRequest( IoControlCode,
                                         Device,
                                         NULL,
                                         0,
                                         OutputBuffer,
                                         OutputBufferLength,
                                         InternalDeviceIoControl,
                                         &Event,
                                         IosbToUse );

    if (Irp == NULL) {

        return STATUS_INSUFFICIENT_RESOURCES;
    }

    if (OverrideVerify) {

        SetFlag( IoGetNextIrpStackLocation( Irp )->Flags, SL_OVERRIDE_VERIFY_VOLUME );
    }

    Status = IoCallDriver( Device, Irp );

    //
    //  We check for device not ready by first checking Status
    //  and then if status pending was returned, the Iosb status
    //  value.
    //

    if (Status == STATUS_PENDING) {

        (VOID) KeWaitForSingleObject( &Event,
                                      Executive,
                                      KernelMode,
                                      FALSE,
                                      (PLARGE_INTEGER)NULL );

        Status = IosbToUse->Status;
    }

    return Status;
}


