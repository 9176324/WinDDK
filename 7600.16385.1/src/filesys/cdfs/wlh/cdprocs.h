/*++

Copyright (c) 1989-2000 Microsoft Corporation

Module Name:

    CdProcs.h

Abstract:

    This module defines all of the globally used procedures in the Cdfs
    file system.


--*/

#ifndef _CDPROCS_
#define _CDPROCS_

#include <ntifs.h>

#include <ntddcdrm.h>
#include <ntdddisk.h>
#include <ntddscsi.h>

#ifndef INLINE
#define INLINE __inline
#endif

#include "nodetype.h"
#include "Cd.h"
#include "CdStruc.h"
#include "CdData.h"


//**** x86 compiler bug ****

#if defined(_M_IX86)
#undef Int64ShraMod32
#define Int64ShraMod32(a, b) ((LONGLONG)(a) >> (b))
#endif

#ifndef Min
#define Min(a, b)   ((a) < (b) ? (a) : (b))
#endif

#ifndef Max
#define Max(a, b)   ((a) > (b) ? (a) : (b))
#endif

//
//  Here are the different pool tags.
//

#define TAG_CCB                 'ccdC'      //  Ccb
#define TAG_CDROM_TOC           'ctdC'      //  TOC
#define TAG_DIRENT_NAME         'nddC'      //  CdName in dirent
#define TAG_ENUM_EXPRESSION     'eedC'      //  Search expression for enumeration
#define TAG_FCB_DATA            'dfdC'      //  Data Fcb
#define TAG_FCB_INDEX           'ifdC'      //  Index Fcb
#define TAG_FCB_NONPAGED        'nfdC'      //  Nonpaged Fcb
#define TAG_FCB_TABLE           'tfdC'      //  Fcb Table entry
#define TAG_FILE_NAME           'nFdC'      //  Filename buffer
#define TAG_GEN_SHORT_NAME      'sgdC'      //  Generated short name
#define TAG_IO_BUFFER           'fbdC'      //  Temporary IO buffer
#define TAG_IO_CONTEXT          'oidC'      //  Io context for async reads
#define TAG_IRP_CONTEXT         'cidC'      //  Irp Context
#define TAG_IRP_CONTEXT_LITE    'lidC'      //  Irp Context lite
#define TAG_MCB_ARRAY           'amdC'      //  Mcb array
#define TAG_PATH_ENTRY_NAME     'nPdC'      //  CdName in path entry
#define TAG_PREFIX_ENTRY        'epdC'      //  Prefix Entry
#define TAG_PREFIX_NAME         'npdC'      //  Prefix Entry name
#define TAG_SPANNING_PATH_TABLE 'psdC'      //  Buffer for spanning path table
#define TAG_UPCASE_NAME         'nudC'      //  Buffer for upcased name
#define TAG_VOL_DESC            'dvdC'      //  Buffer for volume descriptor
#define TAG_VPB                 'pvdC'      //  Vpb allocated in filesystem

//
//  Tag all of our allocations if tagging is turned on
//

#ifdef POOL_TAGGING

#undef FsRtlAllocatePool
#undef FsRtlAllocatePoolWithQuota
#define FsRtlAllocatePool(a,b) FsRtlAllocatePoolWithTag(a,b,'sfdC')
#define FsRtlAllocatePoolWithQuota(a,b) FsRtlAllocatePoolWithQuotaTag(a,b,'sfdC')

#endif // POOL_TAGGING


//
//  File access check routine, implemented in AcChkSup.c
//

//
//  BOOLEAN
//  CdIllegalFcbAccess (
//      __in PIRP_CONTEXT IrpContext,
//      __in TYPE_OF_OPEN TypeOfOpen,
//      __in ACCESS_MASK DesiredAccess
//      );
//

#define CdIllegalFcbAccess(IC,T,DA) (                           \
           BooleanFlagOn( (DA),                                 \
                          ((T) != UserVolumeOpen ?              \
                           (FILE_WRITE_ATTRIBUTES           |   \
                            FILE_WRITE_DATA                 |   \
                            FILE_WRITE_EA                   |   \
                            FILE_ADD_FILE                   |   \
                            FILE_ADD_SUBDIRECTORY           |   \
                            FILE_APPEND_DATA) : 0)          |   \
                          FILE_DELETE_CHILD                 |   \
                          DELETE                            |   \
                          WRITE_DAC ))


//
//  Allocation support routines, implemented in AllocSup.c
//
//  These routines are for querying allocation on individual streams.
//

VOID
CdLookupAllocation (
    __in PIRP_CONTEXT IrpContext,
    __in PFCB Fcb,
    __in LONGLONG FileOffset,
    __out PLONGLONG DiskOffset,
    __out PULONG ByteCount
    );

VOID
CdAddAllocationFromDirent (
    __in PIRP_CONTEXT IrpContext,
    __inout PFCB Fcb,
    __in ULONG McbEntryOffset,
    __in LONGLONG StartingFileOffset,
    __in PDIRENT Dirent
    );

VOID
CdAddInitialAllocation (
    __in PIRP_CONTEXT IrpContext,
    __inout PFCB Fcb,
    __in ULONG StartingBlock,
    __in LONGLONG DataLength
    );

VOID
CdTruncateAllocation (
    __in PIRP_CONTEXT IrpContext,
    __inout PFCB Fcb,
    __in LONGLONG StartingFileOffset
    );

VOID
CdInitializeMcb (
    __in PIRP_CONTEXT IrpContext,
    __inout_bcount(Fcb->NodeByteSize) PFCB Fcb
    );

VOID
CdUninitializeMcb (
    __in PIRP_CONTEXT IrpContext,
    __inout_bcount(Fcb->NodeByteSize) PFCB Fcb
    );


//
//   Buffer control routines for data caching, implemented in CacheSup.c
//

VOID
CdCreateInternalStream (
    __in PIRP_CONTEXT IrpContext,
    __in PVCB Vcb,
    __inout PFCB Fcb,
    __in_opt PUNICODE_STRING Name
    );

VOID
CdDeleteInternalStream (
    __in PIRP_CONTEXT IrpContext,
    __inout PFCB Fcb
    );

NTSTATUS
CdCompleteMdl (
    __in PIRP_CONTEXT IrpContext,
    __inout PIRP Irp
    );

NTSTATUS
CdPurgeVolume (
    __in PIRP_CONTEXT IrpContext,
    __in PVCB Vcb,
    __in BOOLEAN DismountUnderway
    );

VOID
INLINE
CdVerifyOrCreateDirStreamFile (
    __in PIRP_CONTEXT IrpContext,
    __in PFCB Fcb
    )
{
    //
    //  Unsafe test to see if call / lock neccessary.
    //
    
    if (NULL == Fcb->FileObject) {
        
        CdCreateInternalStream( IrpContext,
                                Fcb->Vcb,
                                Fcb, 
                                &Fcb->FileNamePrefix.ExactCaseName.FileName);
    }
}


//
//  VOID
//  CdUnpinData (
//      __in PIRP_CONTEXT IrpContext,
//      __inout PBCB *Bcb
//      );
//

#define CdUnpinData(IC,B)   \
    if (*(B) != NULL) { CcUnpinData( *(B) ); *(B) = NULL; }


//
//  Device I/O routines, implemented in DevIoSup.c
//
//  These routines perform the actual device read and writes.  They only affect
//  the on disk structure and do not alter any other data structures.
//

VOID
CdFreeDirCache (
    __in PIRP_CONTEXT IrpContext
    );

NTSTATUS
CdNonCachedRead (
    __in PIRP_CONTEXT IrpContext,
    __in PFCB Fcb,
    __in LONGLONG StartingOffset,
    __in ULONG ByteCount
    );

NTSTATUS
CdNonCachedXARead (
    __in PIRP_CONTEXT IrpContext,
    __in PFCB Fcb,
    __in LONGLONG StartingOffset,
    __in ULONG ByteCount
    );

NTSTATUS
CdVolumeDasdWrite (
    __in PIRP_CONTEXT IrpContext,
    __in PFCB Fcb,
    __in LONGLONG StartingOffset,
    __in ULONG ByteCount
    );

BOOLEAN
CdReadSectors (
    __in PIRP_CONTEXT IrpContext,
    __in LONGLONG StartingOffset,
    __in ULONG ByteCount,
    __in BOOLEAN ReturnError,
    __out_bcount(ByteCount) PVOID Buffer,
    __in PDEVICE_OBJECT TargetDeviceObject
    );

NTSTATUS
CdCreateUserMdl (
    __in PIRP_CONTEXT IrpContext,
    __in ULONG BufferLength,
    __in BOOLEAN RaiseOnError,
    __in LOCK_OPERATION Operation
    );

NTSTATUS
FASTCALL
CdPerformDevIoCtrl (
    __in PIRP_CONTEXT IrpContext,
    __in ULONG IoControlCode,
    __in PDEVICE_OBJECT Device,
    __out_bcount_opt(OutputBufferLength) PVOID OutputBuffer,
    __in ULONG OutputBufferLength,
    __in BOOLEAN InternalDeviceIoControl,
    __in BOOLEAN OverrideVerify,
    __out_opt PIO_STATUS_BLOCK Iosb
    );

NTSTATUS
CdPerformDevIoCtrlEx (
    __in PIRP_CONTEXT IrpContext,
    __in ULONG IoControlCode,
    __in PDEVICE_OBJECT Device,
    __in_bcount_opt(InputBufferLength) PVOID InputBuffer,
    __in ULONG InputBufferLength,
    __out_bcount_opt(OutputBufferLength) PVOID OutputBuffer,
    __in ULONG OutputBufferLength,
    __in BOOLEAN InternalDeviceIoControl,
    __in BOOLEAN OverrideVerify,
    __out_opt PIO_STATUS_BLOCK Iosb
    );

NTSTATUS
CdHijackIrpAndFlushDevice (
    __in PIRP_CONTEXT IrpContext,
    __inout PIRP Irp,
    __in PDEVICE_OBJECT TargetDeviceObject
    );


//
//  VOID
//  CdMapUserBuffer (
//      __in PIRP_CONTEXT IrpContext
//      __out PVOID UserBuffer
//      );
//
//  Returns pointer to sys address.  Will raise on failure.
//
//
//  VOID
//  CdLockUserBuffer (
//      __inout PIRP_CONTEXT IrpContext,
//      __in ULONG BufferLength
//      );
//

#define CdMapUserBuffer(IC, UB) {                                               \
            *(UB) = (PVOID) ( ((IC)->Irp->MdlAddress == NULL) ?                 \
                    (IC)->Irp->UserBuffer :                                     \
                    (MmGetSystemAddressForMdlSafe( (IC)->Irp->MdlAddress, NormalPagePriority)));   \
            if (NULL == *(UB))  {                         \
                CdRaiseStatus( (IC), STATUS_INSUFFICIENT_RESOURCES);            \
            }                                                                   \
        }                                                                       
        

#define CdLockUserBuffer(IC,BL,OP) {                        \
    if ((IC)->Irp->MdlAddress == NULL) {                    \
        (VOID) CdCreateUserMdl( (IC), (BL), TRUE, (OP) );   \
    }                                                       \
}


//
//  Dirent support routines, implemented in DirSup.c
//

VOID
CdLookupDirent (
    __in PIRP_CONTEXT IrpContext,
    __in PFCB Fcb,
    __in ULONG DirentOffset,
    __out PDIRENT_ENUM_CONTEXT DirContext
    );

BOOLEAN
CdLookupNextDirent (
    __in PIRP_CONTEXT IrpContext,
    __in PFCB Fcb,
    __in PDIRENT_ENUM_CONTEXT CurrentDirContext,
    __out PDIRENT_ENUM_CONTEXT NextDirContext
    );

VOID
CdUpdateDirentFromRawDirent (
    __in PIRP_CONTEXT IrpContext,
    __in PFCB Fcb,
    __in PDIRENT_ENUM_CONTEXT DirContext,
    __inout PDIRENT Dirent
    );

VOID
CdUpdateDirentName (
    __in PIRP_CONTEXT IrpContext,
    __inout PDIRENT Dirent,
    __in ULONG IgnoreCase
    );

BOOLEAN
CdFindFile (
    __in PIRP_CONTEXT IrpContext,
    __in PFCB Fcb,
    __in PCD_NAME Name,
    __in BOOLEAN IgnoreCase,
    __inout PFILE_ENUM_CONTEXT FileContext,
    __out PCD_NAME *MatchingName
    );

BOOLEAN
CdFindDirectory (
    __in PIRP_CONTEXT IrpContext,
    __in PFCB Fcb,
    __in PCD_NAME Name,
    __in BOOLEAN IgnoreCase,
    __inout PFILE_ENUM_CONTEXT FileContext
    );

BOOLEAN
CdFindFileByShortName (
    __in PIRP_CONTEXT IrpContext,
    __in PFCB Fcb,
    __in PCD_NAME Name,
    __in BOOLEAN IgnoreCase,
    __in ULONG ShortNameDirentOffset,
    __inout PFILE_ENUM_CONTEXT FileContext
    );

BOOLEAN
CdLookupNextInitialFileDirent (
    __in PIRP_CONTEXT IrpContext,
    __in PFCB Fcb,
    __inout PFILE_ENUM_CONTEXT FileContext
    );

VOID
CdLookupLastFileDirent (
    __in PIRP_CONTEXT IrpContext,
    __in PFCB Fcb,
    __in PFILE_ENUM_CONTEXT FileContext
    );

VOID
CdCleanupFileContext (
    __in PIRP_CONTEXT IrpContext,
    __in PFILE_ENUM_CONTEXT FileContext
    );

//
//  VOID
//  CdInitializeFileContext (
//      __in PIRP_CONTEXT IrpContext,
//      __out PFILE_ENUM_CONTEXT FileContext
//      );
//
//
//  VOID
//  CdInitializeDirent (
//      __in PIRP_CONTEXT IrpContext,
//      __out PDIRENT Dirent
//      );
//
//  VOID
//  CdInitializeDirContext (
//      __in PIRP_CONTEXT IrpContext,
//      __out PDIRENT_ENUM_CONTEXT DirContext
//      );
//
//  VOID
//  CdCleanupDirent (
//      __in PIRP_CONTEXT IrpContext,
//      __inout PDIRENT Dirent
//      );
//
//  VOID
//  CdCleanupDirContext (
//      __in PIRP_CONTEXT IrpContext,
//      __inout PDIRENT_ENUM_CONTEXT DirContext
//      );
//
//  VOID
//  CdLookupInitialFileDirent (
//      __in PIRP_CONTEXT IrpContext,
//      __in PFCB Fcb,
//      __out PFILE_ENUM_CONTEXT FileContext,
//      __in ULONG DirentOffset
//      );
//

#define CdInitializeFileContext(IC,FC) {                                \
    RtlZeroMemory( FC, sizeof( FILE_ENUM_CONTEXT ));                    \
    (FC)->PriorDirent = &(FC)->Dirents[0];                              \
    (FC)->InitialDirent = &(FC)->Dirents[1];                            \
    (FC)->CurrentDirent = &(FC)->Dirents[2];                            \
    (FC)->ShortName.FileName.MaximumLength = BYTE_COUNT_8_DOT_3;        \
    (FC)->ShortName.FileName.Buffer = (FC)->ShortNameBuffer;            \
}

#define CdInitializeDirent(IC,D)                                \
    RtlZeroMemory( D, sizeof( DIRENT ))

#define CdInitializeDirContext(IC,DC)                           \
    RtlZeroMemory( DC, sizeof( DIRENT_ENUM_CONTEXT ))

#define CdCleanupDirent(IC,D)  {                                \
    if (FlagOn( (D)->Flags, DIRENT_FLAG_ALLOC_BUFFER )) {       \
        CdFreePool( &(D)->CdFileName.FileName.Buffer );          \
    }                                                           \
}

#define CdCleanupDirContext(IC,DC)                              \
    CdUnpinData( (IC), &(DC)->Bcb )

#define CdLookupInitialFileDirent(IC,F,FC,DO)                       \
    CdLookupDirent( IC,                                             \
                    F,                                              \
                    DO,                                             \
                    &(FC)->InitialDirent->DirContext );             \
    CdUpdateDirentFromRawDirent( IC,                                \
                                 F,                                 \
                                 &(FC)->InitialDirent->DirContext,  \
                                 &(FC)->InitialDirent->Dirent )


//
//  The following routines are used to manipulate the fscontext fields
//  of the file object, implemented in FilObSup.c
//

//
//  Type of opens.  FilObSup.c depends on this order.
//

typedef enum _TYPE_OF_OPEN {

    UnopenedFileObject = 0,
    StreamFileOpen,
    UserVolumeOpen,
    UserDirectoryOpen,
    UserFileOpen,
    BeyondValidType

} TYPE_OF_OPEN;
typedef TYPE_OF_OPEN *PTYPE_OF_OPEN;

VOID
CdSetFileObject (
    __in PIRP_CONTEXT IrpContext,
    __inout PFILE_OBJECT FileObject,
    __in TYPE_OF_OPEN TypeOfOpen,
    __in_opt PFCB Fcb,
    __in_opt PCCB Ccb
    );

TYPE_OF_OPEN
CdDecodeFileObject (
    __in PIRP_CONTEXT IrpContext,
    __in PFILE_OBJECT FileObject,
    __out PFCB *Fcb,
    __out PCCB *Ccb
    );

TYPE_OF_OPEN
CdFastDecodeFileObject (
    __in PFILE_OBJECT FileObject,
    __out PFCB *Fcb
    );


//
//  Name support routines, implemented in NameSup.c
//

VOID
CdConvertNameToCdName (
    __in PIRP_CONTEXT IrpContext,
    __inout PCD_NAME CdName
    );

VOID
CdConvertBigToLittleEndian (
    __in PIRP_CONTEXT IrpContext,
    __in_bcount(ByteCount) PCHAR BigEndian,
    __in ULONG ByteCount,
    __out_bcount(ByteCount) PCHAR LittleEndian
    );

VOID
CdUpcaseName (
    __in PIRP_CONTEXT IrpContext,
    __in PCD_NAME Name,
    __inout PCD_NAME UpcaseName
    );

VOID
CdDissectName (
    __in PIRP_CONTEXT IrpContext,
    __inout PUNICODE_STRING RemainingName,
    __out PUNICODE_STRING FinalName
    );

BOOLEAN
CdIs8dot3Name (
    __in PIRP_CONTEXT IrpContext,
    __in UNICODE_STRING FileName
    );

VOID
CdGenerate8dot3Name (
    __in PIRP_CONTEXT IrpContext,
    __in PUNICODE_STRING FileName,
    __in ULONG DirentOffset,
    __out_bcount_part(BYTE_COUNT_8_DOT_3, *ShortByteCount) PWCHAR ShortFileName,
    __out PUSHORT ShortByteCount
    );

BOOLEAN
CdIsNameInExpression (
    __in PIRP_CONTEXT IrpContext,
    __in PCD_NAME CurrentName,
    __in PCD_NAME SearchExpression,
    __in ULONG  WildcardFlags,
    __in BOOLEAN CheckVersion
    );

ULONG
CdShortNameDirentOffset (
    __in PIRP_CONTEXT IrpContext,
    __in PUNICODE_STRING Name
    );

FSRTL_COMPARISON_RESULT
CdFullCompareNames (
    __in PIRP_CONTEXT IrpContext,
    __in PUNICODE_STRING NameA,
    __in PUNICODE_STRING NameB
    );


//
//  Filesystem control operations.  Implemented in Fsctrl.c
//

NTSTATUS
CdLockVolumeInternal (
    __in PIRP_CONTEXT IrpContext,
    __inout PVCB Vcb,
    __in_opt PFILE_OBJECT FileObject
    );

NTSTATUS
CdUnlockVolumeInternal (
    __in PIRP_CONTEXT IrpContext,
    __inout PVCB Vcb,
    __in_opt PFILE_OBJECT FileObject
    );


//
//  Path table enumeration routines.  Implemented in PathSup.c
//

VOID
CdLookupPathEntry (
    __in PIRP_CONTEXT IrpContext,
    __in ULONG PathEntryOffset,
    __in ULONG Ordinal,
    __in BOOLEAN VerifyBounds,
    __inout PCOMPOUND_PATH_ENTRY CompoundPathEntry
    );

BOOLEAN
CdLookupNextPathEntry (
    __in PIRP_CONTEXT IrpContext,
    __inout PPATH_ENUM_CONTEXT PathContext,
    __inout PPATH_ENTRY PathEntry
    );

BOOLEAN
CdFindPathEntry (
    __in PIRP_CONTEXT IrpContext,
    __in PFCB ParentFcb,
    __in PCD_NAME DirName,
    __in BOOLEAN IgnoreCase,
    __inout PCOMPOUND_PATH_ENTRY CompoundPathEntry
    );

VOID
CdUpdatePathEntryName (
    __in PIRP_CONTEXT IrpContext,
    __inout PPATH_ENTRY PathEntry,
    __in BOOLEAN IgnoreCase
    );

//
//  VOID
//  CdInitializeCompoundPathEntry (
//      __in PIRP_CONTEXT IrpContext,
//      __out PCOMPOUND_PATH_ENTRY CompoundPathEntry
//      );
//
//  VOID
//  CdCleanupCompoundPathEntry (
//      __in PIRP_CONTEXT IrpContext,
//      __out PCOMPOUND_PATH_ENTRY CompoundPathEntry
//      );
//

#define CdInitializeCompoundPathEntry(IC,CP)                                    \
    RtlZeroMemory( CP, sizeof( COMPOUND_PATH_ENTRY ))

#define CdCleanupCompoundPathEntry(IC,CP)     {                                 \
    CdUnpinData( (IC), &(CP)->PathContext.Bcb );                                \
    if ((CP)->PathContext.AllocatedData) {                                      \
        CdFreePool( &(CP)->PathContext.Data );                                   \
    }                                                                           \
    if (FlagOn( (CP)->PathEntry.Flags, PATH_ENTRY_FLAG_ALLOC_BUFFER )) {        \
        CdFreePool( &(CP)->PathEntry.CdDirName.FileName.Buffer );                \
    }                                                                           \
}


//
//  Largest matching prefix searching routines, implemented in PrefxSup.c
//

VOID
CdInsertPrefix (
    __in PIRP_CONTEXT IrpContext,
    __inout PFCB Fcb,
    __in PCD_NAME Name,
    __in BOOLEAN IgnoreCase,
    __in BOOLEAN ShortNameMatch,
    __inout PFCB ParentFcb
    );

VOID
CdRemovePrefix (
    __in PIRP_CONTEXT IrpContext,
    __inout PFCB Fcb
    );

VOID
CdFindPrefix (
    __in PIRP_CONTEXT IrpContext,
    __inout PFCB *CurrentFcb,
    __inout PUNICODE_STRING RemainingName,
    __in BOOLEAN IgnoreCase
    );


//
//  Synchronization routines.  Implemented in Resrcsup.c
//
//  The following routines/macros are used to synchronize the in-memory structures.
//
//      Routine/Macro               Synchronizes                            Subsequent
//
//      CdAcquireCdData             Volume Mounts/Dismounts,Vcb Queue       CdReleaseCdData
//      CdAcquireVcbExclusive       Vcb for open/close                      CdReleaseVcb
//      CdAcquireVcbShared          Vcb for open/close                      CdReleaseVcb
//      CdAcquireAllFiles           Locks out operations to all files       CdReleaseAllFiles
//      CdAcquireFileExclusive      Locks out file operations               CdReleaseFile
//      CdAcquireFileShared         Files for file operations               CdReleaseFile
//      CdAcquireFcbExclusive       Fcb for open/close                      CdReleaseFcb
//      CdAcquireFcbShared          Fcb for open/close                      CdReleaseFcb
//      CdLockCdData                Fields in CdData                        CdUnlockCdData
//      CdLockVcb                   Vcb fields, FcbReference, FcbTable      CdUnlockVcb
//      CdLockFcb                   Fcb fields, prefix table, Mcb           CdUnlockFcb
//

typedef enum _TYPE_OF_ACQUIRE {
    
    AcquireExclusive,
    AcquireShared,
    AcquireSharedStarveExclusive

} TYPE_OF_ACQUIRE, *PTYPE_OF_ACQUIRE;

BOOLEAN
CdAcquireResource (
    __in PIRP_CONTEXT IrpContext,
    __inout PERESOURCE Resource,
    __in BOOLEAN IgnoreWait,
    __in TYPE_OF_ACQUIRE Type
    );

//
//  BOOLEAN
//  CdAcquireCdData (
//      __in PIRP_CONTEXT IrpContext
//      );
//
//  VOID
//  CdReleaseCdData (
//      __in PIRP_CONTEXT IrpContext
//    );
//
//  BOOLEAN
//  CdAcquireVcbExclusive (
//      __in PIRP_CONTEXT IrpContext,
//      __inout PVCB Vcb,
//      __in BOOLEAN IgnoreWait
//      );
//
//  BOOLEAN
//  CdAcquireVcbShared (
//      __in PIRP_CONTEXT IrpContext,
//      __inout PVCB Vcb,
//      __in BOOLEAN IgnoreWait
//      );
//
//  VOID
//  CdReleaseVcb (
//      __in PIRP_CONTEXT IrpContext,
//      __inout PVCB Vcb
//      );
//
//  VOID
//  CdAcquireAllFiles (
//      __in PIRP_CONTEXT,
//      __in PVCB Vcb
//      );
//
//  VOID
//  CdReleaseAllFiles (
//      __in PIRP_CONTEXT,
//      __in PVCB Vcb
//      );
//
//  VOID
//  CdAcquireFileExclusive (
//      __in PIRP_CONTEXT IrpContext,
//      __inout PFCB Fcb,
//      );
//
//  VOID
//  CdAcquireFileShared (
//      __in PIRP_CONTEXT IrpContext,
//      __inout PFCB Fcb
//      );
//
//  VOID
//  CdReleaseFile (
//      __in PIRP_CONTEXT IrpContext,
//      __inout PFCB Fcb
//    );
//
//  BOOLEAN
//  CdAcquireFcbExclusive (
//      __in PIRP_CONTEXT IrpContext,
//      __inout PFCB Fcb,
//      __in BOOLEAN IgnoreWait
//      );
//
//  BOOLEAN
//  CdAcquireFcbShared (
//      __in PIRP_CONTEXT IrpContext,
//      __inout PFCB Fcb,
//      __in BOOLEAN IgnoreWait
//      );
//
//  BOOLEAN
//  CdReleaseFcb (
//      __in PIRP_CONTEXT IrpContext,
//      __inout PFCB Fcb
//      );
//
//  VOID
//  CdLockCdData (
//      );
//
//  VOID
//  CdUnlockCdData (
//      );
//
//  VOID
//  CdLockVcb (
//      __in PIRP_CONTEXT IrpContext
//      );
//
//  VOID
//  CdUnlockVcb (
//      __in PIRP_CONTEXT IrpContext
//      );
//
//  VOID
//  CdLockFcb (
//      __in PIRP_CONTEXT IrpContext,
//      __inout PFCB Fcb
//      );
//
//  VOID
//  CdUnlockFcb (
//      __in PIRP_CONTEXT IrpContext,
//      __inout PFCB Fcb
//      );
//


#define CdAcquireCacheForRead( IC)                                                      \
    ExAcquireResourceSharedLite( &(IC)->Vcb->SectorCacheResource, TRUE)
    
#define CdAcquireCacheForUpdate( IC)                                                    \
    ExAcquireResourceExclusiveLite( &(IC)->Vcb->SectorCacheResource, TRUE)
    
#define CdReleaseCache( IC)                                                             \
    ExReleaseResourceLite( &(IC)->Vcb->SectorCacheResource);

#define CdConvertCacheToShared( IC)                                                     \
    ExConvertExclusiveToSharedLite( &(IC)->Vcb->SectorCacheResource);

#define CdAcquireCdData(IC)                                                             \
    ExAcquireResourceExclusiveLite( &CdData.DataResource, TRUE )

#define CdReleaseCdData(IC)                                                             \
    ExReleaseResourceLite( &CdData.DataResource )

#define CdAcquireVcbExclusive(IC,V,I)                                                   \
    CdAcquireResource( (IC), &(V)->VcbResource, (I), AcquireExclusive )

#define CdAcquireVcbShared(IC,V,I)                                                      \
    CdAcquireResource( (IC), &(V)->VcbResource, (I), AcquireShared )

#define CdReleaseVcb(IC,V)                                                              \
    ExReleaseResourceLite( &(V)->VcbResource )

#define CdAcquireAllFiles(IC,V)                                                         \
    CdAcquireResource( (IC), &(V)->FileResource, FALSE, AcquireExclusive )

#define CdReleaseAllFiles(IC,V)                                                         \
    ExReleaseResourceLite( &(V)->FileResource )

#define CdAcquireFileExclusive(IC,F)                                                    \
    CdAcquireResource( (IC), (F)->Resource, FALSE, AcquireExclusive )

#define CdAcquireFileShared(IC,F)                                                       \
    CdAcquireResource( (IC), (F)->Resource, FALSE, AcquireShared )

#define CdAcquireFileSharedStarveExclusive(IC,F)                                        \
    CdAcquireResource( (IC), (F)->Resource, FALSE, AcquireSharedStarveExclusive )

#define CdReleaseFile(IC,F)                                                             \
    ExReleaseResourceLite( (F)->Resource )

#define CdAcquireFcbExclusive(IC,F,I)                                                   \
    CdAcquireResource( (IC), &(F)->FcbNonpaged->FcbResource, (I), AcquireExclusive )

#define CdAcquireFcbShared(IC,F,I)                                                      \
    CdAcquireResource( (IC), &(F)->FcbNonpaged->FcbResource, (I), AcquireShared )

#define CdReleaseFcb(IC,F)                                                              \
    ExReleaseResourceLite( &(F)->FcbNonpaged->FcbResource )

#define CdLockCdData()                                                                  \
    ExAcquireFastMutex( &CdData.CdDataMutex );                                          \
    CdData.CdDataLockThread = PsGetCurrentThread()

#define CdUnlockCdData()                                                                \
    CdData.CdDataLockThread = NULL;                                                     \
    ExReleaseFastMutex( &CdData.CdDataMutex )

#define CdLockVcb(IC,V)                                                                 \
    ExAcquireFastMutex( &(V)->VcbMutex );                                               \
    ASSERT( NULL == (V)->VcbLockThread);                                                \
    (V)->VcbLockThread = PsGetCurrentThread()

#define CdUnlockVcb(IC,V)                                                               \
    ASSERT( NULL != (V)->VcbLockThread);                                                \
    (V)->VcbLockThread = NULL;                                                          \
    ExReleaseFastMutex( &(V)->VcbMutex )

#define CdLockFcb(IC,F) {                                                               \
    PVOID _CurrentThread = PsGetCurrentThread();                                        \
    if (_CurrentThread != (F)->FcbLockThread) {                                         \
        ExAcquireFastMutex( &(F)->FcbNonpaged->FcbMutex );                              \
        ASSERT( (F)->FcbLockCount == 0 );                                               \
        __analysis_assume( (F)->FcbLockCount == 0 );                                    \
        (F)->FcbLockThread = _CurrentThread;                                            \
    }                                                                                   \
    (F)->FcbLockCount += 1;                                                             \
}

#define CdUnlockFcb(IC,F) {                                                             \
    (F)->FcbLockCount -= 1;                                                             \
    if ((F)->FcbLockCount == 0) {                                                       \
        (F)->FcbLockThread = NULL;                                                      \
        ExReleaseFastMutex( &(F)->FcbNonpaged->FcbMutex );                              \
    }                                                                                   \
}

BOOLEAN
CdNoopAcquire (
    __in PVOID Fcb,
    __in BOOLEAN Wait
    );

VOID
CdNoopRelease (
    __in PVOID Fcb
    );

BOOLEAN
CdAcquireForCache (
    __inout PFCB Fcb,
    __in BOOLEAN Wait
    );

VOID
CdReleaseFromCache (
    __inout PFCB Fcb
    );

NTSTATUS
CdFilterCallbackAcquireForCreateSection (
    __in PFS_FILTER_CALLBACK_DATA CallbackData,
    __out PVOID *CompletionContext
    );

VOID
CdReleaseForCreateSection (
    __in PFILE_OBJECT FileObject
    );


//
//  In-memory structure support routines.  Implemented in StrucSup.c
//

VOID
CdInitializeVcb (
    __in PIRP_CONTEXT IrpContext,
    __inout PVCB Vcb,
    __in PDEVICE_OBJECT TargetDeviceObject,
    __in PVPB Vpb,
    __in PCDROM_TOC_LARGE CdromToc,
    __in ULONG TocLength,
    __in ULONG TocTrackCount,
    __in ULONG TocDiskFlags,
    __in ULONG BlockFactor,
    __in ULONG MediaChangeCount
    );

VOID
CdUpdateVcbFromVolDescriptor (
    __in PIRP_CONTEXT IrpContext,
    __inout PVCB Vcb,
    __in_bcount_opt(SECTOR_SIZE) PCHAR RawIsoVd
    );

VOID
CdDeleteVcb (
    __in PIRP_CONTEXT IrpContext,
    __inout PVCB Vcb
    );

PFCB
CdCreateFcb (
    __in PIRP_CONTEXT IrpContext,
    __in FILE_ID FileId,
    __in NODE_TYPE_CODE NodeTypeCode,
    __out_opt PBOOLEAN FcbExisted
    );

VOID
CdInitializeFcbFromPathEntry (
    __in PIRP_CONTEXT IrpContext,
    __inout PFCB Fcb,
    __in_opt PFCB ParentFcb,
    __in PPATH_ENTRY PathEntry
    );

VOID
CdInitializeFcbFromFileContext (
    __in PIRP_CONTEXT IrpContext,
    __inout PFCB Fcb,
    __in PFCB ParentFcb,
    __in PFILE_ENUM_CONTEXT FileContext
    );

PCCB
CdCreateCcb (
    __in PIRP_CONTEXT IrpContext,
    __in PFCB Fcb,
    __in ULONG Flags
    );

VOID
CdDeleteCcb (
    __in PIRP_CONTEXT IrpContext,
    __inout PCCB Ccb
    );

BOOLEAN
CdCreateFileLock (
    __in_opt PIRP_CONTEXT IrpContext,
    __inout PFCB Fcb,
    __in BOOLEAN RaiseOnError
    );

VOID
CdDeleteFileLock (
    __in PIRP_CONTEXT IrpContext,
    __inout PFILE_LOCK FileLock
    );

PIRP_CONTEXT
CdCreateIrpContext (
    __in PIRP Irp,
    __in BOOLEAN Wait
    );

VOID
CdCleanupIrpContext (
    __inout PIRP_CONTEXT IrpContext,
    __in BOOLEAN Post
    );

VOID
CdInitializeStackIrpContext (
    __out PIRP_CONTEXT IrpContext,
    __in PIRP_CONTEXT_LITE IrpContextLite
    );

//
//  PIRP_CONTEXT_LITE
//  CdCreateIrpContextLite (
//      __in PIRP_CONTEXT IrpContext
//      );
//
//  VOID
//  CdFreeIrpContextLite (
//      __inout PIRP_CONTEXT_LITE IrpContextLite
//      );
//

#define CdCreateIrpContextLite(IC)  \
    ExAllocatePoolWithTag( CdNonPagedPool, sizeof( IRP_CONTEXT_LITE ), TAG_IRP_CONTEXT_LITE )

#define CdFreeIrpContextLite(ICL)  \
    CdFreePool( &(ICL) )

VOID
CdTeardownStructures (
    __in PIRP_CONTEXT IrpContext,
    __inout PFCB StartingFcb,
    __out PBOOLEAN RemovedStartingFcb
    );

//
//  VOID
//  CdIncrementCleanupCounts (
//      __in PIRP_CONTEXT IrpContext,
//      __inout PFCB Fcb
//      );
//
//  VOID
//  CdDecrementCleanupCounts (
//      __in PIRP_CONTEXT IrpContext,
//      __inout PFCB Fcb
//      );
//
//  VOID
//  CdIncrementReferenceCounts (
//      __in PIRP_CONTEXT IrpContext,
//      __inout PFCB Fcb,
//      __in ULONG ReferenceCount
//      __in ULONG UserReferenceCount
//      );
//
//  VOID
//  CdDecrementReferenceCounts (
//      __in PIRP_CONTEXT IrpContext,
//      __inout PFCB Fcb,
//      __in ULONG ReferenceCount
//      __in ULONG UserReferenceCount
//      );
//
//  VOID
//  CdIncrementFcbReference (
//      __in PIRP_CONTEXT IrpContext,
//      __inout PFCB Fcb
//      );
//
//  VOID
//  CdDecrementFcbReference (
//      __in PIRP_CONTEXT IrpContext,
//      __inout PFCB Fcb
//      );
//

#define CdIncrementCleanupCounts(IC,F) {        \
    ASSERT_LOCKED_VCB( (F)->Vcb );              \
    (F)->FcbCleanup += 1;                       \
    (F)->Vcb->VcbCleanup += 1;                  \
}

#define CdDecrementCleanupCounts(IC,F) {        \
    ASSERT_LOCKED_VCB( (F)->Vcb );              \
    (F)->FcbCleanup -= 1;                       \
    (F)->Vcb->VcbCleanup -= 1;                  \
}

#define CdIncrementReferenceCounts(IC,F,C,UC) { \
    ASSERT_LOCKED_VCB( (F)->Vcb );              \
    (F)->FcbReference += (C);                   \
    (F)->FcbUserReference += (UC);              \
    (F)->Vcb->VcbReference += (C);              \
    (F)->Vcb->VcbUserReference += (UC);         \
}

#define CdDecrementReferenceCounts(IC,F,C,UC) { \
    ASSERT_LOCKED_VCB( (F)->Vcb );              \
    (F)->FcbReference -= (C);                   \
    (F)->FcbUserReference -= (UC);              \
    (F)->Vcb->VcbReference -= (C);              \
    (F)->Vcb->VcbUserReference -= (UC);         \
}

//
//  PCD_IO_CONTEXT
//  CdAllocateIoContext (
//      );
//
//  VOID
//  CdFreeIoContext (
//      PCD_IO_CONTEXT IoContext
//      );
//

#define CdAllocateIoContext()                           \
    FsRtlAllocatePoolWithTag( CdNonPagedPool,           \
                              sizeof( CD_IO_CONTEXT ),  \
                              TAG_IO_CONTEXT )

#define CdFreeIoContext(IO)     CdFreePool( &(IO) )

PFCB
CdLookupFcbTable (
    __in PIRP_CONTEXT IrpContext,
    __in PVCB Vcb,
    __in FILE_ID FileId
    );

PFCB
CdGetNextFcb (
    __in PIRP_CONTEXT IrpContext,
    __in PVCB Vcb,
    __in PVOID *RestartKey
    );

NTSTATUS
CdProcessToc (
    __in PIRP_CONTEXT IrpContext,
    __in PDEVICE_OBJECT TargetDeviceObject,
    __in PCDROM_TOC_LARGE CdromToc,
    __inout PULONG Length,
    __out PULONG TrackCount,
    __out PULONG DiskFlags
    );

//
//  For debugging purposes we sometimes want to allocate our structures from nonpaged
//  pool so that in the kernel debugger we can walk all the structures.
//

#define CdPagedPool                 PagedPool
#define CdNonPagedPool              NonPagedPool
#define CdNonPagedPoolCacheAligned  NonPagedPoolCacheAligned


//
//  Verification support routines.  Contained in verfysup.c
//


INLINE
BOOLEAN
CdOperationIsDasdOpen (
    __in PIRP_CONTEXT IrpContext
    )
{
    PIO_STACK_LOCATION IrpSp = IoGetCurrentIrpStackLocation( IrpContext->Irp);
    
    return ((IrpContext->MajorFunction == IRP_MJ_CREATE) &&
            (IrpSp->FileObject->FileName.Length == 0) &&
            (IrpSp->FileObject->RelatedFileObject == NULL));
}


NTSTATUS
CdPerformVerify (
    __inout PIRP_CONTEXT IrpContext,
    __inout PIRP Irp,
    __in PDEVICE_OBJECT DeviceToVerify
    );

BOOLEAN
CdCheckForDismount (
    __in PIRP_CONTEXT IrpContext,
    __inout PVCB Vcb,
    __in BOOLEAN Force
    );

BOOLEAN
CdMarkDevForVerifyIfVcbMounted (
    __inout PVCB Vcb
    );

VOID
CdVerifyVcb (
    __in PIRP_CONTEXT IrpContext,
    __inout PVCB Vcb
    );

BOOLEAN
CdVerifyFcbOperation (
    __in_opt PIRP_CONTEXT IrpContext,
    __in PFCB Fcb
    );

BOOLEAN
CdDismountVcb (
    __in PIRP_CONTEXT IrpContext,
    __inout PVCB Vcb
    );


//
//  Macros to abstract device verify flag changes.
//

#define CdUpdateMediaChangeCount( V, C)  (V)->MediaChangeCount = (C)
#define CdUpdateVcbCondition( V, C)      (V)->VcbCondition = (C)

#define CdMarkRealDevForVerify( DO)  SetFlag( (DO)->Flags, DO_VERIFY_VOLUME)
                                     
#define CdMarkRealDevVerifyOk( DO)   ClearFlag( (DO)->Flags, DO_VERIFY_VOLUME)


#define CdRealDevNeedsVerify( DO)    BooleanFlagOn( (DO)->Flags, DO_VERIFY_VOLUME)

//
//  BOOLEAN
//  CdIsRawDevice (
//      __in PIRP_CONTEXT IrpContext,
//      __in NTSTATUS Status
//      );
//

#define CdIsRawDevice(IC,S) (           \
    ((S) == STATUS_DEVICE_NOT_READY) || \
    ((S) == STATUS_NO_MEDIA_IN_DEVICE)  \
)


//
//  Work queue routines for posting and retrieving an Irp, implemented in
//  workque.c
//

NTSTATUS
CdFsdPostRequest (
    __inout PIRP_CONTEXT IrpContext,
    __inout PIRP Irp
    );

VOID
CdPrePostIrp (
    __inout PIRP_CONTEXT IrpContext,
    __inout PIRP Irp
    );

VOID
CdOplockComplete (
    __inout PIRP_CONTEXT IrpContext,
    __inout PIRP Irp
    );


//
//  Miscellaneous support routines
//

//
//  This macro returns TRUE if a flag in a set of flags is on and FALSE
//  otherwise
//

//#ifndef BooleanFlagOn
//#define BooleanFlagOn(F,SF) (    \
//    (BOOLEAN)(((F) & (SF)) != 0) \
//)
//#endif

//#ifndef SetFlag
//#define SetFlag(Flags,SingleFlag) { \
//    (Flags) |= (SingleFlag);        \
//}
//#endif

//#ifndef ClearFlag
//#define ClearFlag(Flags,SingleFlag) { \
//    (Flags) &= ~(SingleFlag);         \
//}
//#endif

//
//      CAST
//      Add2Ptr (
//          __in PVOID Pointer,
//          __in ULONG Increment
//          __in (CAST)
//          );
//
//      ULONG
//      PtrOffset (
//          __in PVOID BasePtr,
//          __in PVOID OffsetPtr
//          );
//

#define Add2Ptr(PTR,INC,CAST) ((CAST)((PUCHAR)(PTR) + (INC)))

#define PtrOffset(BASE,OFFSET) ((ULONG)((ULONG_PTR)(OFFSET) - (ULONG_PTR)(BASE)))

//
//  This macro takes a pointer (or ulong) and returns its rounded up word
//  value
//

#define WordAlign(Ptr) (                \
    ((((ULONG)(Ptr)) + 1) & 0xfffffffe) \
    )

//
//  This macro takes a pointer (or ulong) and returns its rounded up longword
//  value
//

#define LongAlign(Ptr) (                \
    ((((ULONG)(Ptr)) + 3) & 0xfffffffc) \
    )

//
//  This macro takes a pointer (or ulong) and returns its rounded up quadword
//  value
//

#define QuadAlign(Ptr) (                \
    ((((ULONG)(Ptr)) + 7) & 0xfffffff8) \
    )

//
//  The following macros round up and down to sector boundaries.
//

#define SectorAlign(L) (                                                \
    ((((ULONG)(L)) + (SECTOR_SIZE - 1)) & ~(SECTOR_SIZE - 1))           \
)

#define LlSectorAlign(L) (                                              \
    ((((LONGLONG)(L)) + (SECTOR_SIZE - 1)) & ~(SECTOR_SIZE - 1))        \
)

#define SectorTruncate(L) (                                             \
    ((ULONG)(L)) & ~(SECTOR_SIZE - 1)                                   \
)

#define LlSectorTruncate(L) (                                           \
    ((LONGLONG)(L)) & ~(SECTOR_SIZE - 1)                                \
)

#define BytesFromSectors(L) (                                           \
    ((ULONG) (L)) << SECTOR_SHIFT                                       \
)

#define SectorsFromBytes(L) (                                           \
    ((ULONG) (L)) >> SECTOR_SHIFT                                       \
)

INLINE
ULONG
SectorsFromLlBytes( 
    ULONGLONG Bytes
) {

    return (ULONG)(Bytes >> SECTOR_SHIFT);
}

#define LlBytesFromSectors(L) (                                         \
    Int64ShllMod32( (LONGLONG)(L), SECTOR_SHIFT )                       \
)

#define LlSectorsFromBytes(L) (                                         \
    Int64ShraMod32( (LONGLONG)(L), SECTOR_SHIFT )                       \
)

#define SectorOffset(L) (                                               \
    ((ULONG)(ULONG_PTR) (L)) & SECTOR_MASK                              \
)

#define SectorBlockOffset(V,LB) (                                       \
    ((ULONG) (LB)) & ((V)->BlocksPerSector - 1)                         \
)

#define BytesFromBlocks(V,B) (                                          \
    (ULONG) (B) << (V)->BlockToByteShift                                \
)

#define LlBytesFromBlocks(V,B) (                                        \
    Int64ShllMod32( (LONGLONG) (B), (V)->BlockToByteShift )             \
)

#define BlockAlign(V,L) (                                               \
    ((ULONG)(L) + (V)->BlockMask) & (V)->BlockInverseMask               \
)

//
//  Carefully make sure the mask is sign extended to 64bits
//

#define LlBlockAlign(V,L) (                                                     \
    ((LONGLONG)(L) + (V)->BlockMask) & (LONGLONG)((LONG)(V)->BlockInverseMask)  \
)

#define BlockOffset(V,L) (                                              \
    ((ULONG) (L)) & (V)->BlockMask                                      \
)

#define RawSectorAlign( B) ((((B)+(RAW_SECTOR_SIZE - 1)) / RAW_SECTOR_SIZE) * RAW_SECTOR_SIZE)

//
//  The following types and macros are used to help unpack the packed and
//  misaligned fields found in the Bios parameter block
//

typedef union _UCHAR1 {
    UCHAR  Uchar[1];
    UCHAR  ForceAlignment;
} UCHAR1, *PUCHAR1;

typedef union _UCHAR2 {
    UCHAR  Uchar[2];
    USHORT ForceAlignment;
} UCHAR2, *PUCHAR2;

typedef union _UCHAR4 {
    UCHAR  Uchar[4];
    ULONG  ForceAlignment;
} UCHAR4, *PUCHAR4;

typedef union _USHORT2 {
    USHORT Ushort[2];
    ULONG  ForceAlignment;
} USHORT2, *PUSHORT2;

//
//  This macro copies an unaligned src byte to an aligned dst byte
//

#define CopyUchar1(Dst,Src) {                           \
    *((UCHAR1 *)(Dst)) = *((UNALIGNED UCHAR1 *)(Src));  \
    }

//
//  This macro copies an unaligned src word to an aligned dst word
//

#define CopyUchar2(Dst,Src) {                           \
    *((UCHAR2 *)(Dst)) = *((UNALIGNED UCHAR2 *)(Src));  \
    }

//
//  This macro copies an unaligned src longword to an aligned dsr longword
//

#define CopyUchar4(Dst,Src) {                           \
    *((UCHAR4 *)(Dst)) = *((UNALIGNED UCHAR4 *)(Src));  \
    }

//
//  This macro copies an unaligned src longword to an aligned dsr longword
//  accessing the source on a word boundary.
//

#define CopyUshort2(Dst,Src) {                          \
    *((USHORT2 *)(Dst)) = *((UNALIGNED USHORT2 *)(Src));\
    }

//
//  This macro copies an unaligned src longword to a dst longword,
//  performing an little/big endian swap.
//

#define SwapCopyUchar4(Dst,Src) {                                        \
    *((UNALIGNED UCHAR1 *)(Dst)) = *((UNALIGNED UCHAR1 *)(Src) + 3);     \
    *((UNALIGNED UCHAR1 *)(Dst) + 1) = *((UNALIGNED UCHAR1 *)(Src) + 2); \
    *((UNALIGNED UCHAR1 *)(Dst) + 2) = *((UNALIGNED UCHAR1 *)(Src) + 1); \
    *((UNALIGNED UCHAR1 *)(Dst) + 3) = *((UNALIGNED UCHAR1 *)(Src));     \
}

VOID
CdLbnToMmSsFf (
    __in ULONG Blocks,
    __out PUCHAR Msf
    );

//
//  Following routines handle entry in and out of the filesystem.  They are
//  contained in CdData.c
//

NTSTATUS
CdFsdDispatch (
    __in PVOLUME_DEVICE_OBJECT VolumeDeviceObject,
    __inout PIRP Irp
    );

LONG
CdExceptionFilter (
    __inout PIRP_CONTEXT IrpContext,
    __in PEXCEPTION_POINTERS ExceptionPointer
    );

NTSTATUS
CdProcessException (
    __in_opt PIRP_CONTEXT IrpContext,
    __inout PIRP Irp,
    __in NTSTATUS ExceptionCode
    );

VOID
CdCompleteRequest (
    __inout_opt PIRP_CONTEXT IrpContext,
    __inout_opt PIRP Irp,
    __in NTSTATUS Status
    );

//
//  VOID
//  CdRaiseStatus (
//      __in PRIP_CONTEXT IrpContext,
//      __in NT_STATUS Status
//      );
//
//  VOID
//  CdNormalizeAndRaiseStatus (
//      __in PRIP_CONTEXT IrpContext,
//      __in NT_STATUS Status
//      );
//

#if 0
#define AssertVerifyDevice(C, S)                                                    \
    ASSERT( (C) == NULL ||                                                          \
            FlagOn( (C)->Flags, IRP_CONTEXT_FLAG_IN_FSP ) ||                        \
            !((S) == STATUS_VERIFY_REQUIRED &&                                      \
              IoGetDeviceToVerify( PsGetCurrentThread() ) == NULL ));

#define AssertVerifyDeviceIrp(I)                                                    \
    ASSERT( (I) == NULL ||                                                          \
            !(((I)->IoStatus.Status) == STATUS_VERIFY_REQUIRED &&                   \
              ((I)->Tail.Overlay.Thread == NULL ||                                  \
                IoGetDeviceToVerify( (I)->Tail.Overlay.Thread ) == NULL )));
#else
#define AssertVerifyDevice(C, S)
#define AssertVerifyDeviceIrp(I)
#endif


#ifdef CD_SANITY

DECLSPEC_NORETURN
VOID
CdRaiseStatusEx (
    __in PIRP_CONTEXT IrpContext,
    __in NTSTATUS Status,
    __in BOOLEAN NormalizeStatus,
    __in_opt ULONG FileId,
    __in_opt ULONG Line
    );

#else

INLINE
DECLSPEC_NORETURN
VOID
CdRaiseStatusEx(
    __in PIRP_CONTEXT IrpContext,
    __in NTSTATUS Status,
    __in BOOLEAN NormalizeStatus,
    __in ULONG Fileid,
    __in ULONG Line
    )
{
    if (NormalizeStatus)  {

        IrpContext->ExceptionStatus = FsRtlNormalizeNtstatus( Status, STATUS_UNEXPECTED_IO_ERROR);
    }
    else {

        IrpContext->ExceptionStatus = Status;
    }

    IrpContext->RaisedAtLineFile = (Fileid << 16) | Line;

    ExRaiseStatus( IrpContext->ExceptionStatus );
}

#endif

#define CdRaiseStatus( IC, S)               CdRaiseStatusEx( (IC), (S), FALSE, BugCheckFileId, __LINE__);
#define CdNormalizeAndRaiseStatus( IC, S)   CdRaiseStatusEx( (IC), (S), TRUE, BugCheckFileId, __LINE__);

//
//  Following are the fast entry points.
//

BOOLEAN
CdFastQueryBasicInfo (
    __in PFILE_OBJECT FileObject,
    __in BOOLEAN Wait,
    __out PFILE_BASIC_INFORMATION Buffer,
    __out PIO_STATUS_BLOCK IoStatus,
    __in PDEVICE_OBJECT DeviceObject
    );

BOOLEAN
CdFastQueryStdInfo (
    __in PFILE_OBJECT FileObject,
    __in BOOLEAN Wait,
    __out PFILE_STANDARD_INFORMATION Buffer,
    __out PIO_STATUS_BLOCK IoStatus,
    __in PDEVICE_OBJECT DeviceObject
    );

BOOLEAN
CdFastLock (
    __in PFILE_OBJECT FileObject,
    __in PLARGE_INTEGER FileOffset,
    __in PLARGE_INTEGER Length,
    __in PEPROCESS ProcessId,
    __in ULONG Key,
    __in BOOLEAN FailImmediately,
    __in BOOLEAN ExclusiveLock,
    __out PIO_STATUS_BLOCK IoStatus,
    __in PDEVICE_OBJECT DeviceObject
    );

BOOLEAN
CdFastUnlockSingle (
    __in PFILE_OBJECT FileObject,
    __in PLARGE_INTEGER FileOffset,
    __in PLARGE_INTEGER Length,
    __in PEPROCESS ProcessId,
    __in ULONG Key,
    __out PIO_STATUS_BLOCK IoStatus,
    __in PDEVICE_OBJECT DeviceObject
    );

BOOLEAN
CdFastUnlockAll (
    __in PFILE_OBJECT FileObject,
    __in PEPROCESS ProcessId,
    __out PIO_STATUS_BLOCK IoStatus,
    __in PDEVICE_OBJECT DeviceObject
    );

BOOLEAN
CdFastUnlockAllByKey (
    __in PFILE_OBJECT FileObject,
    __in PVOID ProcessId,
    __in ULONG Key,
    __out PIO_STATUS_BLOCK IoStatus,
    __in PDEVICE_OBJECT DeviceObject
    );

BOOLEAN
CdFastIoCheckIfPossible (
    __in PFILE_OBJECT FileObject,
    __in PLARGE_INTEGER FileOffset,
    __in ULONG Length,
    __in BOOLEAN Wait,
    __in ULONG LockKey,
    __in BOOLEAN CheckForReadOperation,
    __out PIO_STATUS_BLOCK IoStatus,
    __in PDEVICE_OBJECT DeviceObject
    );

BOOLEAN
CdFastQueryNetworkInfo (
    __in PFILE_OBJECT FileObject,
    __in BOOLEAN Wait,
    __out PFILE_NETWORK_OPEN_INFORMATION Buffer,
    __out PIO_STATUS_BLOCK IoStatus,
    __in PDEVICE_OBJECT DeviceObject
    );

//
//  Following are the routines to handle the top level thread logic.
//

VOID
CdSetThreadContext (
    __inout PIRP_CONTEXT IrpContext,
    __in PTHREAD_CONTEXT ThreadContext
    );


//
//  VOID
//  CdRestoreThreadContext (
//      __inout PIRP_CONTEXT IrpContext
//      );
//

#define CdRestoreThreadContext(IC)                              \
    (IC)->ThreadContext->Cdfs = 0;                              \
    IoSetTopLevelIrp( (IC)->ThreadContext->SavedTopLevelIrp );  \
    (IC)->ThreadContext = NULL

ULONG
CdSerial32 (
    __in_bcount(ByteCount) PCHAR Buffer,
    __in ULONG ByteCount
    );

//
//  The following macro is used to determine if an FSD thread can block
//  for I/O or wait for a resource.  It returns TRUE if the thread can
//  block and FALSE otherwise.  This attribute can then be used to call
//  the FSD & FSP common work routine with the proper wait value.
//

#define CanFsdWait(I)   IoIsOperationSynchronous(I)

//
//  The following macro is used to set the fast i/o possible bits in the
//  FsRtl header.
//
//      FastIoIsNotPossible - If the Fcb is bad or there are oplocks on the file.
//
//      FastIoIsQuestionable - If there are file locks.
//
//      FastIoIsPossible - In all other cases.
//
//

#define CdIsFastIoPossible(F) ((BOOLEAN)                                            \
    ((((F)->Vcb->VcbCondition != VcbMounted ) ||                                    \
      !FsRtlOplockIsFastIoPossible( &(F)->Oplock )) ?                               \
                                                                                    \
     FastIoIsNotPossible :                                                          \
                                                                                    \
     ((((F)->FileLock != NULL) && FsRtlAreThereCurrentFileLocks( (F)->FileLock )) ? \
                                                                                    \
        FastIoIsQuestionable :                                                      \
                                                                                    \
        FastIoIsPossible))                                                          \
)


//
//  The FSP level dispatch/main routine.  This is the routine that takes
//  IRP's off of the work queue and calls the appropriate FSP level
//  work routine.
//

VOID
CdFspDispatch (                             //  implemented in FspDisp.c
    __inout PIRP_CONTEXT IrpContext
    );

VOID
CdFspClose (                                //  implemented in Close.c
    __in_opt PVCB Vcb
    );

//
//  The following routines are the entry points for the different operations
//  based on the IrpSp major functions.
//

NTSTATUS
CdCommonCreate (                            //  Implemented in Create.c
    __inout PIRP_CONTEXT IrpContext,
    __inout PIRP Irp
    );

NTSTATUS
CdCommonClose (                             //  Implemented in Close.c
    __inout PIRP_CONTEXT IrpContext,
    __inout PIRP Irp
    );

NTSTATUS
CdCommonRead (                              //  Implemented in Read.c
    __inout PIRP_CONTEXT IrpContext,
    __inout PIRP Irp
    );

NTSTATUS
CdCommonWrite (                             //  Implemented in Write.c
    __inout PIRP_CONTEXT IrpContext,
    __inout PIRP Irp
    );

NTSTATUS
CdCommonQueryInfo (                         //  Implemented in FileInfo.c
    __inout PIRP_CONTEXT IrpContext,
    __inout PIRP Irp
    );

NTSTATUS
CdCommonSetInfo (                           //  Implemented in FileInfo.c
    __inout PIRP_CONTEXT IrpContext,
    __inout PIRP Irp
    );

NTSTATUS
CdCommonQueryVolInfo (                      //  Implemented in VolInfo.c
    __inout PIRP_CONTEXT IrpContext,
    __inout PIRP Irp
    );

NTSTATUS
CdCommonDirControl (                        //  Implemented in DirCtrl.c
    __inout PIRP_CONTEXT IrpContext,
    __inout PIRP Irp
    );

NTSTATUS
CdCommonFsControl (                         //  Implemented in FsCtrl.c
    __inout PIRP_CONTEXT IrpContext,
    __inout PIRP Irp
    );

NTSTATUS
CdCommonDevControl (                        //  Implemented in DevCtrl.c
    __inout PIRP_CONTEXT IrpContext,
    __inout PIRP Irp
    );

NTSTATUS
CdCommonLockControl (                       //  Implemented in LockCtrl.c
    __inout PIRP_CONTEXT IrpContext,
    __inout PIRP Irp
    );

NTSTATUS
CdCommonCleanup (                           //  Implemented in Cleanup.c
    __inout PIRP_CONTEXT IrpContext,
    __inout PIRP Irp
    );

NTSTATUS
CdCommonPnp (                               //  Implemented in Pnp.c
    __inout PIRP_CONTEXT IrpContext,
    __inout PIRP Irp
    );

NTSTATUS
CdCommonShutdown (                         //  Implemented in Shutdown.c
    __inout PIRP_CONTEXT IrpContext,
    __inout PIRP Irp
    );



//
//  The following macros are used to establish the semantics needed
//  to do a return from within a try-finally clause.  As a rule every
//  try clause must end with a label call try_exit.  For example,
//
//      try {
//              :
//              :
//
//      try_exit: NOTHING;
//      } finally {
//
//              :
//              :
//      }
//
//  Every return statement executed inside of a try clause should use the
//  try_return macro.  If the compiler fully supports the try-finally construct
//  then the macro should be
//
//      #define try_return(S)  { return(S); }
//
//  If the compiler does not support the try-finally construct then the macro
//  should be
//
//      #define try_return(S)  { S; goto try_exit; }
//

#define try_return(S) { S; goto try_exit; }
#define try_leave(S) { S; leave; }

//
//  Encapsulate safe pool freeing
//

INLINE
VOID
CdFreePool(
    __inout PVOID *Pool
    )
{
    if (*Pool != NULL) {

        ExFreePool(*Pool);
        *Pool = NULL;
    }
}

#endif // _CDPROCS_


