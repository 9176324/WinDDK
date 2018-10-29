/*++

Copyright (c) 1989-2000 Microsoft Corporation

Module Name:

    Write.c

Abstract:

    This module implements the File Write routine for Write called by the
    Fsd/Fsp dispatch drivers.


--*/

#include "CdProcs.h"

//
//  The Bug check file id for this module
//

#define BugCheckFileId                   (CDFS_BUG_CHECK_WRITE)

//
//  VOID
//  SafeZeroMemory (
//      __out PUCHAR At,
//      __in ULONG ByteCount
//      );
//

//
//  This macro just puts a nice little try-except around RtlZeroMemory
//

#define SafeZeroMemory(IC,AT,BYTE_COUNT) {                  \
    try {                                                   \
        RtlZeroMemory( (AT), (BYTE_COUNT) );                \
    } except( EXCEPTION_EXECUTE_HANDLER ) {                 \
         CdRaiseStatus( IC, STATUS_INVALID_USER_BUFFER );   \
    }                                                       \
}


#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, CdCommonWrite)
#endif


NTSTATUS
CdCommonWrite (
    __inout PIRP_CONTEXT IrpContext,
    __inout PIRP Irp
    )

/*++

Routine Description:

    This is the common entry point for NtWriteFile calls.  For synchronous requests,
    CommonWrite will complete the request in the current thread.  If not
    synchronous the request will be passed to the Fsp if there is a need to
    block.

Arguments:

    Irp - Supplies the Irp to process

Return Value:

    NTSTATUS - The result of this operation.

--*/

{
    NTSTATUS Status;
    PIO_STACK_LOCATION IrpSp = IoGetCurrentIrpStackLocation( Irp );

    TYPE_OF_OPEN TypeOfOpen;
    PFCB Fcb;
    PCCB Ccb;

    BOOLEAN Wait;
    ULONG SynchronousIo;
    PVOID UserBuffer;

    LONGLONG StartingOffset;
    LONGLONG ByteRange;
    ULONG ByteCount;
    ULONG WriteByteCount;
    ULONG OriginalByteCount;

    PVOID SystemBuffer;

    BOOLEAN ReleaseFile = TRUE;

    CD_IO_CONTEXT LocalIoContext;

    PAGED_CODE();

    //
    //  If this is a zero length write then return SUCCESS immediately.
    //

    if (IrpSp->Parameters.Write.Length == 0) {

        CdCompleteRequest( IrpContext, Irp, STATUS_SUCCESS );
        return STATUS_SUCCESS;
    }

    //
    //  Decode the file object and verify we support write on this.  It
    //  must be a volume file.
    //

    TypeOfOpen = CdDecodeFileObject( IrpContext, IrpSp->FileObject, &Fcb, &Ccb );

    if (TypeOfOpen != UserVolumeOpen) {

        CdCompleteRequest( IrpContext, Irp, STATUS_INVALID_DEVICE_REQUEST );
        return STATUS_INVALID_DEVICE_REQUEST;
    }

    //
    //  Examine our input parameters to determine if this is noncached and/or
    //  a paging io operation.
    //

    Wait = BooleanFlagOn( IrpContext->Flags, IRP_CONTEXT_FLAG_WAIT );
    SynchronousIo = FlagOn( IrpSp->FileObject->Flags, FO_SYNCHRONOUS_IO );


    //
    //  Extract the range of the Io.
    //

    StartingOffset = IrpSp->Parameters.Write.ByteOffset.QuadPart;
    OriginalByteCount = ByteCount = IrpSp->Parameters.Write.Length;

    ByteRange = StartingOffset + ByteCount;

    //
    //  Acquire the file shared to perform the write.
    //

    CdAcquireFileShared( IrpContext, Fcb );

    //
    //  Use a try-finally to facilitate cleanup.
    //

    try {

        //
        //  Verify the Fcb.  Allow writes if this is a DASD handle that is 
        //  dismounting the volume.
        //

        if (!FlagOn( Ccb->Flags, CCB_FLAG_DISMOUNT_ON_CLOSE ))  {

            CdVerifyFcbOperation( IrpContext, Fcb );
        }

        if (!FlagOn( Ccb->Flags, CCB_FLAG_ALLOW_EXTENDED_DASD_IO )) {

            //
            //  Complete the request if it begins beyond the end of file.
            //

            if (StartingOffset >= Fcb->FileSize.QuadPart) {

                try_return( Status = STATUS_END_OF_FILE );
            }

            //
            //  Truncate the write if it extends beyond the end of the file.
            //

            if (ByteRange > Fcb->FileSize.QuadPart) {

                ByteCount = (ULONG) (Fcb->FileSize.QuadPart - StartingOffset);
                ByteRange = Fcb->FileSize.QuadPart;
            }
        }

        //
        //  If we have an unaligned transfer then post this request if
        //  we can't wait.  Unaligned means that the starting offset
        //  is not on a sector boundary or the write is not integral
        //  sectors.
        //

        WriteByteCount = BlockAlign( Fcb->Vcb, ByteCount );

        if (SectorOffset( StartingOffset ) ||
            SectorOffset( WriteByteCount ) ||
            (WriteByteCount > OriginalByteCount)) {

            if (!Wait) {

                CdRaiseStatus( IrpContext, STATUS_CANT_WAIT );
            }

            //
            //  Make sure we don't overwrite the buffer.
            //

            WriteByteCount = ByteCount;
        }

        //
        //  Initialize the IoContext for the write.
        //  If there is a context pointer, we need to make sure it was
        //  allocated and not a stale stack pointer.
        //

        if (IrpContext->IoContext == NULL ||
            !FlagOn( IrpContext->Flags, IRP_CONTEXT_FLAG_ALLOC_IO )) {

            //
            //  If we can wait, use the context on the stack.  Otherwise
            //  we need to allocate one.
            //

            if (Wait) {

                IrpContext->IoContext = &LocalIoContext;
                ClearFlag( IrpContext->Flags, IRP_CONTEXT_FLAG_ALLOC_IO );

            } else {

                IrpContext->IoContext = CdAllocateIoContext();
                SetFlag( IrpContext->Flags, IRP_CONTEXT_FLAG_ALLOC_IO );
            }
        }

        RtlZeroMemory( IrpContext->IoContext, sizeof( CD_IO_CONTEXT ) );

        //
        //  Store whether we allocated this context structure in the structure
        //  itself.
        //

        IrpContext->IoContext->AllocatedContext =
            BooleanFlagOn( IrpContext->Flags, IRP_CONTEXT_FLAG_ALLOC_IO );

        if (Wait) {

            KeInitializeEvent( &IrpContext->IoContext->SyncEvent,
                               NotificationEvent,
                               FALSE );

        } else {

            IrpContext->IoContext->ResourceThreadId = ExGetCurrentResourceThread();
            IrpContext->IoContext->Resource = Fcb->Resource;
            IrpContext->IoContext->RequestedByteCount = ByteCount;
        }

        Irp->IoStatus.Information = WriteByteCount;

        //
        //  Set the FO_MODIFIED flag here to trigger a verify when this
        //  handle is closed.  Note that we can err on the conservative
        //  side with no problem, i.e. if we accidently do an extra
        //  verify there is no problem.
        //

        SetFlag( IrpSp->FileObject->Flags, FO_FILE_MODIFIED );

        //
        //  Dasd access is always non-cached. Call the Dasd write routine to
        //  perform the actual write.
        //

        Status = CdVolumeDasdWrite( IrpContext, Fcb, StartingOffset, WriteByteCount );

        //
        //  Don't complete this request now if STATUS_PENDING was returned.
        //

        if (Status == STATUS_PENDING) {

            Irp = NULL;
            ReleaseFile = FALSE;

        //
        //  Test is we should zero part of the buffer or update the
        //  synchronous file position.
        //

        } else {

            //
            //  Convert any unknown error code to IO_ERROR.
            //

            if (!NT_SUCCESS( Status )) {

                //
                //  Set the information field to zero.
                //

                Irp->IoStatus.Information = 0;

                //
                //  Raise if this is a user induced error.
                //

                if (IoIsErrorUserInduced( Status )) {

                    CdRaiseStatus( IrpContext, Status );
                }

                Status = FsRtlNormalizeNtstatus( Status, STATUS_UNEXPECTED_IO_ERROR );

            //
            //  Check if there is any portion of the user's buffer to zero.
            //

            } else if (WriteByteCount != ByteCount) {

                CdMapUserBuffer( IrpContext, &UserBuffer );
                
                SafeZeroMemory( IrpContext,
                                Add2Ptr( UserBuffer,
                                         ByteCount,
                                         PVOID ),
                                WriteByteCount - ByteCount );

                Irp->IoStatus.Information = ByteCount;
            }

            //
            //  Update the file position if this is a synchronous request.
            //

            if (SynchronousIo && NT_SUCCESS( Status )) {

                IrpSp->FileObject->CurrentByteOffset.QuadPart = ByteRange;
            }
        }

    try_exit:  NOTHING;
    } finally {

        //
        //  Release the Fcb.
        //

        if (ReleaseFile) {

            CdReleaseFile( IrpContext, Fcb );
        }
    }

    //
    //  Post the request if we got CANT_WAIT.
    //

    if (Status == STATUS_CANT_WAIT) {

        Status = CdFsdPostRequest( IrpContext, Irp );

    //
    //  Otherwise complete the request.
    //

    } else {

        CdCompleteRequest( IrpContext, Irp, Status );
    }

    return Status;
}


