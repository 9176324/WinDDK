/*++

Copyright (c) 1999 - 2002  Microsoft Corporation

Module Name:

    CtxProc.h

Abstract:

    This is the header file defining the functions of the kernel mode
    filter driver implementing the context sample.


Environment:

    Kernel mode


--*/


//
//  Functions implemented in operations.c
//

FLT_PREOP_CALLBACK_STATUS
CtxPreCreate (
    __inout PFLT_CALLBACK_DATA Cbd,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __deref_out_opt PVOID *CompletionContext
    );

FLT_POSTOP_CALLBACK_STATUS
CtxPostCreate (
    __inout PFLT_CALLBACK_DATA Cbd,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __inout_opt PVOID CbdContext,
    __in FLT_POST_OPERATION_FLAGS Flags
    );

FLT_PREOP_CALLBACK_STATUS
CtxPreCleanup (
    __inout PFLT_CALLBACK_DATA Cbd,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __deref_out_opt PVOID *CompletionContext
    );

FLT_PREOP_CALLBACK_STATUS
CtxPreClose (
    __inout PFLT_CALLBACK_DATA Cbd,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __deref_out_opt PVOID *CompletionContext
    );


FLT_PREOP_CALLBACK_STATUS
CtxPreSetInfo (
    __inout PFLT_CALLBACK_DATA Cbd,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __deref_out_opt PVOID *CompletionContext
    );

FLT_POSTOP_CALLBACK_STATUS
CtxPostSetInfo (
    __inout PFLT_CALLBACK_DATA Cbd,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __inout_opt PVOID CbdContext,
    __in FLT_POST_OPERATION_FLAGS Flags
    );


//
//  Functions implemented in context.c
//

NTSTATUS
CtxFindOrCreateFileContext (
    __in PFLT_CALLBACK_DATA Cbd,
    __in BOOLEAN CreateIfNotFound,
    __in_opt PUNICODE_STRING FileName,
    __deref_out PCTX_FILE_CONTEXT *StreamContext,
    __out_opt PBOOLEAN ContextCreated
    );

NTSTATUS
CtxCreateFileContext (
    __in PUNICODE_STRING FileName,
    __deref_out PCTX_FILE_CONTEXT *StreamContext
    );


NTSTATUS
CtxFindOrCreateStreamContext (
    __in PFLT_CALLBACK_DATA Cbd,
    __in BOOLEAN CreateIfNotFound,
    __deref_out PCTX_STREAM_CONTEXT *StreamContext,
    __out_opt PBOOLEAN ContextCreated
    );

NTSTATUS
CtxCreateStreamContext (
    __deref_out PCTX_STREAM_CONTEXT *StreamContext
    );


NTSTATUS
CtxUpdateNameInStreamContext (
    __in PUNICODE_STRING DirectoryName,
    __inout PCTX_STREAM_CONTEXT StreamContext
    );

NTSTATUS
CtxCreateOrReplaceStreamHandleContext (
    __in PFLT_CALLBACK_DATA Cbd,
    __in BOOLEAN ReplaceIfExists,
    __deref_out PCTX_STREAMHANDLE_CONTEXT *StreamHandleContext,
    __out_opt PBOOLEAN ContextReplaced
    );

NTSTATUS
CtxCreateStreamHandleContext (
    __deref_out PCTX_STREAMHANDLE_CONTEXT *StreamHandleContext
    );

NTSTATUS
CtxUpdateNameInStreamHandleContext (
    __in PUNICODE_STRING DirectoryName,
    __inout PCTX_STREAMHANDLE_CONTEXT StreamHandleContext
    );


//
//  Functions implemented in support.c
//

NTSTATUS
CtxAllocateUnicodeString (
    __inout PUNICODE_STRING String
    );

VOID
CtxFreeUnicodeString (
    __inout PUNICODE_STRING String
    );


//
//  Resource support
//

FORCEINLINE
PERESOURCE
CtxAllocateResource (
    VOID
    )
{

    return ExAllocatePoolWithTag( NonPagedPool,
                                  sizeof( ERESOURCE ),
                                  CTX_RESOURCE_TAG );
}

FORCEINLINE
VOID
CtxFreeResource (
    __in PERESOURCE Resource
    )
{

    ExFreePoolWithTag( Resource,
                       CTX_RESOURCE_TAG );
}

FORCEINLINE
VOID
__drv_acquiresCriticalRegion
CtxAcquireResourceExclusive (
    __inout PERESOURCE Resource
    )
{
    ASSERT(KeGetCurrentIrql() <= APC_LEVEL);
    ASSERT(ExIsResourceAcquiredExclusiveLite(Resource) ||
           !ExIsResourceAcquiredSharedLite(Resource));

    KeEnterCriticalRegion();
    (VOID)ExAcquireResourceExclusiveLite( Resource, TRUE );
}

FORCEINLINE
VOID
__drv_acquiresCriticalRegion
CtxAcquireResourceShared (
    __inout PERESOURCE Resource
    )
{
    ASSERT(KeGetCurrentIrql() <= APC_LEVEL);

    KeEnterCriticalRegion();
    (VOID)ExAcquireResourceSharedLite( Resource, TRUE );
}

FORCEINLINE
VOID
__drv_releasesCriticalRegion
__drv_mustHoldCriticalRegion
CtxReleaseResource (
    __inout PERESOURCE Resource
    )
{
    ASSERT(KeGetCurrentIrql() <= APC_LEVEL);
    ASSERT(ExIsResourceAcquiredExclusiveLite(Resource) ||
           ExIsResourceAcquiredSharedLite(Resource));

    ExReleaseResourceLite(Resource);
    KeLeaveCriticalRegion();
}


