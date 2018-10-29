/*++

Copyright (c) 1999 - 2002  Microsoft Corporation

Module Name:

    MetadataManagerProc.h

Abstract:

    This is the header file defining the functions of the kernel mode
    filter driver implementing filter metadata management.


Environment:

    Kernel mode


--*/

#define MAKE_RESOURCE_OWNER(X) (((ERESOURCE_THREAD)(X)) | 0x3)

//
//  Functions implemented in operations.c
//

FLT_PREOP_CALLBACK_STATUS
FmmPreCreate (
    __inout PFLT_CALLBACK_DATA Cbd,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __deref_out_opt PVOID *CompletionContext
    );

FLT_POSTOP_CALLBACK_STATUS
FmmPostCreate (
    __inout PFLT_CALLBACK_DATA Cbd,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in PVOID CbdContext,
    __in FLT_POST_OPERATION_FLAGS Flags
    );

FLT_PREOP_CALLBACK_STATUS
FmmPreCleanup (
    __inout PFLT_CALLBACK_DATA Cbd,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __deref_out_opt PVOID *CompletionContext
    );

FLT_POSTOP_CALLBACK_STATUS
FmmPostCleanup (
    __inout PFLT_CALLBACK_DATA Cbd,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __inout PVOID CompletionContext,
    __in FLT_POST_OPERATION_FLAGS Flags
    );


FLT_PREOP_CALLBACK_STATUS
FmmPreFSControl (
    __inout PFLT_CALLBACK_DATA Cbd,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __deref_out_opt PVOID *CompletionContext
    );

FLT_POSTOP_CALLBACK_STATUS
FmmPostFSControl (
    __inout PFLT_CALLBACK_DATA Cbd,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in PVOID CompletionContext,
    __in FLT_POST_OPERATION_FLAGS Flags
    );

FLT_PREOP_CALLBACK_STATUS
FmmPreDeviceControl (
    __inout PFLT_CALLBACK_DATA Cbd,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __deref_out_opt PVOID *CompletionContext
    );

FLT_POSTOP_CALLBACK_STATUS
FmmPostDeviceControl (
    __inout PFLT_CALLBACK_DATA Cbd,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in PVOID CbdContext,
    __in FLT_POST_OPERATION_FLAGS Flags
    );

FLT_PREOP_CALLBACK_STATUS
FmmPreShutdown (
    __inout PFLT_CALLBACK_DATA Cbd,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __deref_out_opt PVOID *CompletionContext
    );

FLT_PREOP_CALLBACK_STATUS
FmmPrePnp (
    __inout PFLT_CALLBACK_DATA Cbd,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __deref_out_opt PVOID *CompletionContext
    );

FLT_POSTOP_CALLBACK_STATUS
FmmPostPnp (
    __inout PFLT_CALLBACK_DATA Cbd,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in PVOID CbdContext,
    __in FLT_POST_OPERATION_FLAGS Flags
    );

//
//  Functions implemented in datastore.c
//

__drv_mustHoldCriticalRegion
__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
FmmOpenMetadata (
    __in PFMM_INSTANCE_CONTEXT InstanceContext,
    __in BOOLEAN CreateIfNotPresent
    );

__drv_mustHoldCriticalRegion
__drv_maxIRQL(PASSIVE_LEVEL)
VOID
FmmCloseMetadata (
    __in PFMM_INSTANCE_CONTEXT InstanceContext
    );

NTSTATUS
FmmReleaseMetadataFileReferences (
    __inout PFLT_CALLBACK_DATA Cbd
    );

NTSTATUS
FmmReacquireMetadataFileReferences (
    __inout PFLT_CALLBACK_DATA Cbd
    );

NTSTATUS
FmmSetMetadataOpenTriggerFileObject (
    __inout PFLT_CALLBACK_DATA Cbd
    );

__drv_mustHoldCriticalRegion
__drv_releasesCriticalRegion
__drv_maxIRQL(APC_LEVEL)
VOID
FmmBeginFileSystemOperation (
    IN PFMM_INSTANCE_CONTEXT InstanceContext
    );

__drv_neverHoldCriticalRegion
__drv_acquiresCriticalRegion
__drv_maxIRQL(APC_LEVEL)
VOID
FmmEndFileSystemOperation (
    IN PFMM_INSTANCE_CONTEXT InstanceContext
    );


#if VERIFY_METADATA_OPENED
    
NTSTATUS
FmmIsMetadataOpen (
    __inout PFLT_CALLBACK_DATA Cbd,
    __out BOOLEAN* MetadataOpen
    );

#endif

//
//  Functions implemented in support.c
//

NTSTATUS
FmmAllocateUnicodeString (
    __inout PUNICODE_STRING String
    );

VOID
FmmFreeUnicodeString (
    __inout PUNICODE_STRING String
    );

BOOLEAN
FmmTargetIsVolumeOpen (
    __in PFLT_CALLBACK_DATA Cbd
    );

NTSTATUS
FmmIsImplicitVolumeLock( 
    __in PFLT_CALLBACK_DATA Cbd,
    __out PBOOLEAN IsLock
    );

//
//  Lock primitives
//

__drv_neverHoldCriticalRegion
__drv_acquiresCriticalRegion
__drv_maxIRQL(APC_LEVEL)
FORCEINLINE
VOID
FmmAcquireResourceExclusive (
    IN PERESOURCE Resource
    )
{
    ASSERT(KeGetCurrentIrql() <= APC_LEVEL);
    ASSERT(ExIsResourceAcquiredExclusiveLite(Resource) ||
           !ExIsResourceAcquiredSharedLite(Resource));

    KeEnterCriticalRegion();
    (VOID)ExAcquireResourceExclusiveLite( Resource, TRUE );
}

__drv_neverHoldCriticalRegion
__drv_acquiresCriticalRegion
__drv_maxIRQL(APC_LEVEL)
FORCEINLINE
VOID
FmmAcquireResourceShared (
    IN PERESOURCE Resource
    )
{
    ASSERT(KeGetCurrentIrql() <= APC_LEVEL);

    KeEnterCriticalRegion();
    (VOID)ExAcquireResourceSharedLite( Resource, TRUE );
}

__drv_mustHoldCriticalRegion
__drv_releasesCriticalRegion
__drv_maxIRQL(APC_LEVEL)
FORCEINLINE
VOID
FmmReleaseResource (
    IN PERESOURCE Resource
    )
{
    ASSERT(KeGetCurrentIrql() <= DISPATCH_LEVEL);
    ASSERT(ExIsResourceAcquiredExclusiveLite(Resource) ||
           ExIsResourceAcquiredSharedLite(Resource));

    ExReleaseResourceLite(Resource);
    KeLeaveCriticalRegion();
}



