/*++

Copyright (c) 1999 - 2002  Microsoft Corporation

Module Name:

    cancelSafe.c

Abstract:

    This is the main module of the cancelSafe miniFilter driver.

Environment:

    Kernel mode

--*/

#include <fltKernel.h>
#include <dontuse.h>
#include <suppress.h>


//
//  Debug flags and helper functions
//

#define CSQ_TRACE_ERROR                     0x00000001
#define CSQ_TRACE_LOAD_UNLOAD               0x00000002
#define CSQ_TRACE_INSTANCE_CALLBACK         0x00000004
#define CSQ_TRACE_CONTEXT_CALLBACK          0x00000008
#define CSQ_TRACE_CBDQ_CALLBACK             0x00000010
#define CSQ_TRACE_PRE_READ                  0x00000020
#define CSQ_TRACE_ALL                       0xFFFFFFFF

#define DebugTrace(Level, Data)               \
    if ((Level) & Globals.DebugLevel) {       \
        DbgPrint Data;                        \
    }

//
//  Memory Pool Tags
//

#define INSTANCE_CONTEXT_TAG              'IqsC'
#define QUEUE_CONTEXT_TAG                 'QqsC'

//
//  Magic file name
//

const UNICODE_STRING CsqFile = RTL_CONSTANT_STRING( L"csqdemo.txt" );


//
//  Prototypes
//


//
//  Queue context data structure
//

typedef struct _QUEUE_CONTEXT {

    FLT_CALLBACK_DATA_QUEUE_IO_CONTEXT CbdqIoContext;

} QUEUE_CONTEXT, *PQUEUE_CONTEXT;

//
//  Instance context data structure
//

typedef struct _INSTANCE_CONTEXT {

    //
    //  Instance for this context.
    //

    PFLT_INSTANCE Instance;

    //
    //  Cancel safe queue members
    //

    FLT_CALLBACK_DATA_QUEUE Cbdq;
    LIST_ENTRY QueueHead;
    FAST_MUTEX Lock;

    //
    //  Flag to control the life/death of the work item thread
    //

    volatile LONG WorkerThreadFlag;

    //
    //  Notify the worker thread that the instance is being torndown
    //

    KEVENT TeardownEvent;

} INSTANCE_CONTEXT, *PINSTANCE_CONTEXT;


typedef struct _CSQ_GLOBAL_DATA {

    ULONG  DebugLevel;

    PFLT_FILTER  FilterHandle;

    NPAGED_LOOKASIDE_LIST OueueContextLookaside;

} CSQ_GLOBAL_DATA;



//
//  Global variables
//

CSQ_GLOBAL_DATA Globals;


//
//  Local function prototypes
//

DRIVER_INITIALIZE DriverEntry;
NTSTATUS
DriverEntry (
    __in PDRIVER_OBJECT DriverObject,
    __in PUNICODE_STRING RegistryPath
    );

NTSTATUS
Unload (
    __in FLT_FILTER_UNLOAD_FLAGS Flags
    );

VOID
ContextCleanup (
    __in PFLT_CONTEXT Context,
    __in FLT_CONTEXT_TYPE ContextType
    );

NTSTATUS
InstanceSetup (
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in FLT_INSTANCE_SETUP_FLAGS Flags,
    __in DEVICE_TYPE VolumeDeviceType,
    __in FLT_FILESYSTEM_TYPE VolumeFilesystemType
    );

NTSTATUS
InstanceQueryTeardown (
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags
    );

VOID
InstanceTeardownStart (
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in FLT_INSTANCE_TEARDOWN_FLAGS Flags
    );

VOID
InstanceTeardownComplete (
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in FLT_INSTANCE_TEARDOWN_FLAGS Flags
    );

VOID
InitializeDebugLevel (
    __in PUNICODE_STRING RegistryPath
    );

VOID
__drv_maxIRQL(APC_LEVEL)
__drv_setsIRQL(APC_LEVEL)
CsqAcquire(
    __in PFLT_CALLBACK_DATA_QUEUE DataQueue,
    __out PKIRQL Irql
    );
VOID
__drv_maxIRQL(APC_LEVEL)
__drv_minIRQL(APC_LEVEL)
__drv_setsIRQL(PASSIVE_LEVEL)
CsqRelease(
    __in PFLT_CALLBACK_DATA_QUEUE DataQueue,
    __in KIRQL Irql
    );
NTSTATUS
CsqInsertIo(
    __in PFLT_CALLBACK_DATA_QUEUE DataQueue,
    __in PFLT_CALLBACK_DATA Data,
    __in_opt PVOID Context
    );
VOID
CsqRemoveIo(
    __in PFLT_CALLBACK_DATA_QUEUE DataQueue,
    __in PFLT_CALLBACK_DATA Data
    );
PFLT_CALLBACK_DATA
CsqPeekNextIo(
    __in PFLT_CALLBACK_DATA_QUEUE DataQueue,
    __in_opt PFLT_CALLBACK_DATA Data,
    __in_opt PVOID PeekContext
    );
VOID
CsqCompleteCanceledIo(
    __in PFLT_CALLBACK_DATA_QUEUE DataQueue,
    __inout PFLT_CALLBACK_DATA Data
    );

FLT_PREOP_CALLBACK_STATUS
PreRead (
    __inout PFLT_CALLBACK_DATA Data,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __deref_out_opt PVOID *CompletionContext
    );

VOID
PreReadWorkItemRoutine(
    __in PFLT_GENERIC_WORKITEM WorkItem,
    __in PFLT_FILTER Filter,
    __in PVOID Context
    );

NTSTATUS
PreReadPendIo(
    __in PINSTANCE_CONTEXT InstanceContext
    );

NTSTATUS
PreReadProcessIo(
    __inout PFLT_CALLBACK_DATA Data
    );

VOID
PreReadEmptyQueueAndComplete(
    __in PINSTANCE_CONTEXT InstanceContext
    );

//
//  Assign text sections for each routine.
//

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(INIT, InitializeDebugLevel)
#pragma alloc_text(PAGE, Unload)
#pragma alloc_text(PAGE, ContextCleanup)
#pragma alloc_text(PAGE, InstanceSetup)
#pragma alloc_text(PAGE, InstanceQueryTeardown)
#pragma alloc_text(PAGE, InstanceTeardownStart)
#pragma alloc_text(PAGE, InstanceTeardownComplete)
#endif

//
//  Filters callback routines
//

FLT_OPERATION_REGISTRATION Callbacks[] = {
    { IRP_MJ_READ,
      FLTFL_OPERATION_REGISTRATION_SKIP_PAGING_IO,
      PreRead,
      NULL },

    { IRP_MJ_OPERATION_END }
};

//
// Filters context registration data structure
//

const FLT_CONTEXT_REGISTRATION ContextRegistration[] = {

    { FLT_INSTANCE_CONTEXT,
        0,
        ContextCleanup,
        sizeof( INSTANCE_CONTEXT ),
        INSTANCE_CONTEXT_TAG },

    { FLT_CONTEXT_END }
};

//
// Filters registration data structure
//

FLT_REGISTRATION FilterRegistration = {

    sizeof( FLT_REGISTRATION ),             //  Size
    FLT_REGISTRATION_VERSION,               //  Version
    0,                                      //  Flags
    ContextRegistration,                    //  Context
    Callbacks,                              //  Operation callbacks
    Unload,                                 //  Filters unload routine
    InstanceSetup,                          //  InstanceSetup routine
    InstanceQueryTeardown,                  //  InstanceQueryTeardown routine
    InstanceTeardownStart,                  //  InstanceTeardownStart routine
    InstanceTeardownComplete,               //  InstanceTeardownComplete routine
    NULL, NULL, NULL                        //  Unused naming support callbacks
};

//
//  Filter driver initialization and unload routines
//

NTSTATUS
DriverEntry (
    __in PDRIVER_OBJECT DriverObject,
    __in PUNICODE_STRING RegistryPath
    )
/*++

Routine Description:

    This is the initialization routine for this filter driver. It registers
    itself with the filter manager and initializes all its global data structures.

Arguments:

    DriverObject - Pointer to driver object created by the system to
        represent this driver.

    RegistryPath - Unicode string identifying where the parameters for this
        driver are located in the registry.

Return Value:

    Returns STATUS_SUCCESS.

--*/
{
    NTSTATUS Status;

    //
    //  Initialize global debug level
    //

    InitializeDebugLevel( RegistryPath );

    DebugTrace( CSQ_TRACE_LOAD_UNLOAD,
                ("[Csq]: CancelSafe!DriverEntry\n") );

    //
    //  Initialize global lookaside list
    //

    ExInitializeNPagedLookasideList( &Globals.OueueContextLookaside,
                                     NULL,
                                     NULL,
                                     0,
                                     sizeof( QUEUE_CONTEXT ),
                                     QUEUE_CONTEXT_TAG,
                                     0 );

    //
    //  Register with the filter manager
    //

    Status = FltRegisterFilter( DriverObject,
                                &FilterRegistration,
                                &Globals.FilterHandle );

    if (!NT_SUCCESS( Status )) {

        DebugTrace( CSQ_TRACE_LOAD_UNLOAD | CSQ_TRACE_ERROR,
                    ("[Csq]: Failed to register filter (Status = 0x%x)\n",
                    Status) );

        return Status;
    }

    //
    //  Start filtering I/O
    //

    Status = FltStartFiltering( Globals.FilterHandle );

    if (!NT_SUCCESS( Status )) {

        DebugTrace( CSQ_TRACE_LOAD_UNLOAD | CSQ_TRACE_ERROR,
                    ("[Csq]: Failed to start filtering (Status = 0x%x)\n",
                    Status) );

        FltUnregisterFilter( Globals.FilterHandle );

        return Status;
    }


    DebugTrace( CSQ_TRACE_LOAD_UNLOAD,
                ("[Csq]: Driver loaded complete\n") );

    return Status;
}

VOID
InitializeDebugLevel (
    __in PUNICODE_STRING RegistryPath
    )
/*++

Routine Description:

    This routine tries to read the filter DebugLevel parameter from
    the registry.  This value will be found in the registry location
    indicated by the RegistryPath passed in.

Arguments:

    RegistryPath - The path key passed to the driver during DriverEntry.

Return Value:

    None.

--*/
{
    OBJECT_ATTRIBUTES attributes;
    HANDLE driverRegKey;
    NTSTATUS Status;
    ULONG resultLength;
    UNICODE_STRING valueName;
    UCHAR buffer[sizeof( KEY_VALUE_PARTIAL_INFORMATION ) + sizeof( LONG )];

    Globals.DebugLevel = CSQ_TRACE_ERROR;

    //
    //  Open the desired registry key
    //

    InitializeObjectAttributes( &attributes,
                                RegistryPath,
                                OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
                                NULL,
                                NULL );

    Status = ZwOpenKey( &driverRegKey,
                        KEY_READ,
                        &attributes );

    if (NT_SUCCESS( Status )) {

        //
        //  Read the DebugFlags value from the registry.
        //

        RtlInitUnicodeString( &valueName, L"DebugLevel" );

        Status = ZwQueryValueKey( driverRegKey,
                                  &valueName,
                                  KeyValuePartialInformation,
                                  buffer,
                                  sizeof(buffer),
                                  &resultLength );

        if (NT_SUCCESS( Status )) {

            Globals.DebugLevel = *((PULONG) &(((PKEY_VALUE_PARTIAL_INFORMATION) buffer)->Data));
        }
    }

    //
    //  Close the registry entry
    //

    ZwClose( driverRegKey );
}

NTSTATUS
Unload (
    __in FLT_FILTER_UNLOAD_FLAGS Flags
    )
/*++

Routine Description:

    This is the unload routine for this filter driver. This is called
    when the minifilter is about to be unloaded. We can fail this unload
    request if this is not a mandatory unloaded indicated by the Flags
    parameter.

Arguments:

    Flags - Indicating if this is a mandatory unload.

Return Value:

    Returns the final status of this operation.

--*/
{
    UNREFERENCED_PARAMETER( Flags );

    PAGED_CODE();

    DebugTrace( CSQ_TRACE_LOAD_UNLOAD,
                ("[Csq]: CancelSafe!Unload\n") );

    FltUnregisterFilter( Globals.FilterHandle );
    Globals.FilterHandle = NULL;

    ExDeleteNPagedLookasideList( &Globals.OueueContextLookaside );

    return STATUS_SUCCESS;
}


//
//  Context cleanup routine.
//

VOID
ContextCleanup (
    __in PFLT_CONTEXT Context,
    __in FLT_CONTEXT_TYPE ContextType
    )
/*++

Routine Description:

    FltMgr calls this routine immediately before it deletes the context.

Arguments:

    Context - Pointer to the minifilter driver's portion of the context.

    ContextType - Type of context. Must be one of the following values:
        FLT_FILE_CONTEXT (Microsoft Windows Vista and later only.),
        FLT_INSTANCE_CONTEXT, FLT_STREAM_CONTEXT, FLT_STREAMHANDLE_CONTEXT,
        FLT_TRANSACTION_CONTEXT (Windows Vista and later only.), and
        FLT_VOLUME_CONTEXT

Return Value:

    None.

--*/
{
    UNREFERENCED_PARAMETER( Context );
    UNREFERENCED_PARAMETER( ContextType );

    PAGED_CODE();

    DebugTrace( CSQ_TRACE_CONTEXT_CALLBACK,
                ("[Csq]: CancelSafe!ContextCleanup\n") );
}

//
//  Instance setup/teardown routines.
//

NTSTATUS
InstanceSetup (
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in FLT_INSTANCE_SETUP_FLAGS Flags,
    __in DEVICE_TYPE VolumeDeviceType,
    __in FLT_FILESYSTEM_TYPE VolumeFilesystemType
    )
/*++

Routine Description:

    This routine is called whenever a new instance is created on a volume. This
    gives us a chance to decide if we need to attach to this volume or not.

Arguments:

    FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance and its associated volume.

    Flags - Flags describing the reason for this attach request.

    VolumeDeviceType - Device type of the file system volume.
        Must be one of the following: FILE_DEVICE_CD_ROM_FILE_SYSTEM,
        FILE_DEVICE_DISK_FILE_SYSTEM, and FILE_DEVICE_NETWORK_FILE_SYSTEM.

    VolumeFilesystemType - File system type of the volume.

Return Value:

    STATUS_SUCCESS - attach
    STATUS_FLT_DO_NOT_ATTACH - do not attach

--*/
{
    PINSTANCE_CONTEXT InstCtx = NULL;
    NTSTATUS Status = STATUS_SUCCESS;

    UNREFERENCED_PARAMETER( Flags );
    UNREFERENCED_PARAMETER( VolumeDeviceType );
    UNREFERENCED_PARAMETER( VolumeFilesystemType );

    PAGED_CODE();

    DebugTrace( CSQ_TRACE_INSTANCE_CALLBACK,
                ("[Csq]: CancelSafe!InstanceSetup\n") );

    //
    //  Allocate and initialize the instance context.
    //

    Status = FltAllocateContext( FltObjects->Filter,
                                 FLT_INSTANCE_CONTEXT,
                                 sizeof( INSTANCE_CONTEXT ),
                                 NonPagedPool,
                                 &InstCtx );

    if (!NT_SUCCESS( Status )) {

        DebugTrace( CSQ_TRACE_INSTANCE_CALLBACK | CSQ_TRACE_ERROR,
                    ("[Csq]: Failed to allocate instance context (Volume = %p, Instance = %p, Status = 0x%x)\n",
                    FltObjects->Volume,
                    FltObjects->Instance,
                    Status) );

        goto InstanceSetupCleanup;
    }

    Status = FltCbdqInitialize( FltObjects->Instance,
                                &InstCtx->Cbdq,
                                CsqInsertIo,
                                CsqRemoveIo,
                                CsqPeekNextIo,
                                CsqAcquire,
                                CsqRelease,
                                CsqCompleteCanceledIo );

    if (!NT_SUCCESS( Status )) {

        DebugTrace( CSQ_TRACE_INSTANCE_CALLBACK | CSQ_TRACE_ERROR,
                    ("[Csq]: Failed to initialize callback data queue (Volume = %p, Instance = %p, Status = 0x%x)\n",
                    FltObjects->Volume,
                    FltObjects->Instance,
                    Status) );

        goto InstanceSetupCleanup;
    }

    //
    //  Initialize the internal queue head and lock of the cancel safe queue.
    //

    InitializeListHead( &InstCtx->QueueHead );

    ExInitializeFastMutex( &InstCtx->Lock );

    //
    //  Initialize other members of the instance context.
    //

    InstCtx->Instance = FltObjects->Instance;

    InstCtx->WorkerThreadFlag = 0;

    KeInitializeEvent( &InstCtx->TeardownEvent, NotificationEvent, FALSE );

    //
    //  Set the instance context.
    //

    Status = FltSetInstanceContext( FltObjects->Instance,
                                    FLT_SET_CONTEXT_KEEP_IF_EXISTS,
                                    InstCtx,
                                    NULL );

    if( !NT_SUCCESS( Status )) {

        DebugTrace( CSQ_TRACE_INSTANCE_CALLBACK | CSQ_TRACE_ERROR,
                    ("[Csq]: Failed to set instance context (Volume = %p, Instance = %p, Status = 0x%x)\n",
                    FltObjects->Volume,
                    FltObjects->Instance,
                    Status) );

        goto InstanceSetupCleanup;
    }


InstanceSetupCleanup:

    if ( InstCtx != NULL ) {

        FltReleaseContext( InstCtx );
    }

    return Status;
}


NTSTATUS
InstanceQueryTeardown (
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags
    )
/*++

Routine Description:

    This is called when an instance is being manually deleted by a
    call to FltDetachVolume or FilterDetach thereby giving us a
    chance to fail that detach request.

Arguments:

    FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance and its associated volume.

    Flags - Indicating where this detach request came from.

Return Value:

    Returns the status of this operation.

--*/
{
    UNREFERENCED_PARAMETER( FltObjects );
    UNREFERENCED_PARAMETER( Flags );

    PAGED_CODE();

    DebugTrace( CSQ_TRACE_INSTANCE_CALLBACK,
                ("[Csq]: CancelSafe!InstanceQueryTeardown\n") );

    return STATUS_SUCCESS;
}


VOID
InstanceTeardownStart (
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in FLT_INSTANCE_TEARDOWN_FLAGS Flags
    )
/*++

Routine Description:

    This routine is called at the start of instance teardown.

Arguments:

    FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance and its associated volume.

    Flags - Reason why this instance is been deleted.

Return Value:

    None.

--*/
{
    PINSTANCE_CONTEXT InstCtx = 0;
    NTSTATUS Status;

    UNREFERENCED_PARAMETER( FltObjects );
    UNREFERENCED_PARAMETER( Flags );

    PAGED_CODE();

    DebugTrace( CSQ_TRACE_INSTANCE_CALLBACK,
                ("[Csq]: CancelSafe!InstanceTeardownStart\n") );

    //
    //  Get a pointer to the instance context.
    //

    Status = FltGetInstanceContext( FltObjects->Instance,
                                    &InstCtx );

    if (!NT_SUCCESS(Status))
    {
        ASSERT( !"Instance Context is missing" );
        return;
    }

    //
    //  Disable the insert to the cancel safe queue.
    //

    FltCbdqDisable( &InstCtx->Cbdq );

    //
    //  Remove all callback data from the queue and complete them.
    //

    PreReadEmptyQueueAndComplete( InstCtx );

    //
    //  Signal the worker thread if it is pended.
    //

    KeSetEvent( &InstCtx->TeardownEvent, 0, FALSE );

    //
    //  Cleanup
    //

    FltReleaseContext( InstCtx );
}


VOID
InstanceTeardownComplete (
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in FLT_INSTANCE_TEARDOWN_FLAGS Flags
    )
/*++

Routine Description:

    This routine is called at the end of instance teardown.

Arguments:

    FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance and its associated volume.

    Flags - Reason why this instance is been deleted.

Return Value:

    None.

--*/
{
    UNREFERENCED_PARAMETER( FltObjects );
    UNREFERENCED_PARAMETER( Flags );

    DebugTrace( CSQ_TRACE_INSTANCE_CALLBACK,
                ("[Csq]: CancelSafe!InstanceTeardownComplete\n") );

    PAGED_CODE();
}


//
//  Cbdq callback routines.
//

VOID
__drv_maxIRQL(APC_LEVEL)
__drv_setsIRQL(APC_LEVEL)
CsqAcquire(
    __in PFLT_CALLBACK_DATA_QUEUE DataQueue,
    __out PKIRQL Irql
    )
/*++

Routine Description:

    FltMgr calls this routine to acquire the lock protecting the queue.

Arguments:

    DataQueue - Supplies a pointer to the queue itself.

    Irql - Returns the previous IRQL if a spinlock is acquired.  We do not use
           any spinlocks, so we ignore this.

Return Value:

    None.

--*/
{
    PINSTANCE_CONTEXT InstCtx;

    UNREFERENCED_PARAMETER( Irql );

    DebugTrace( CSQ_TRACE_CBDQ_CALLBACK,
                ("[Csq]: CancelSafe!CsqAcquire\n") );

    //
    //  Get a pointer to the instance context.
    //

    InstCtx = CONTAINING_RECORD( DataQueue, INSTANCE_CONTEXT, Cbdq );

    //
    //  Acquire the lock.
    //

    ExAcquireFastMutex( &InstCtx->Lock );
}


VOID
__drv_maxIRQL(APC_LEVEL)
__drv_minIRQL(APC_LEVEL)
__drv_setsIRQL(PASSIVE_LEVEL)
CsqRelease(
    __in PFLT_CALLBACK_DATA_QUEUE DataQueue,
    __in KIRQL Irql
    )
/*++

Routine Description:

    FltMgr calls this routine to release the lock protecting the queue.

Arguments:

    DataQueue - Supplies a pointer to the queue itself.

    Irql - Supplies the previous IRQL if a spinlock is acquired.  We do not use
           any spinlocks, so we ignore this.

Return Value:

    None.

--*/
{
    PINSTANCE_CONTEXT InstCtx;

    UNREFERENCED_PARAMETER( Irql );

    DebugTrace( CSQ_TRACE_CBDQ_CALLBACK,
                ("[Csq]: CancelSafe!CsqRelease\n") );

    //
    //  Get a pointer to the instance context.
    //

    InstCtx = CONTAINING_RECORD( DataQueue, INSTANCE_CONTEXT, Cbdq );

    //
    //  Release the lock.
    //

    ExReleaseFastMutex( &InstCtx->Lock );
}


NTSTATUS
CsqInsertIo(
    __in PFLT_CALLBACK_DATA_QUEUE DataQueue,
    __in PFLT_CALLBACK_DATA Data,
    __in_opt PVOID Context
    )
/*++

Routine Description:

    FltMgr calls this routine to insert an entry into our pending I/O queue.
    The queue is already locked before this routine is called.

Arguments:

    DataQueue - Supplies a pointer to the queue itself.

    Data - Supplies the callback data for the operation that is being
           inserted into the queue.

    Context - Supplies user-defined context information.

Return Value:

    STATUS_SUCCESS if the function completes successfully.  Otherwise a valid
    NTSTATUS code is returned.

--*/
{
    PINSTANCE_CONTEXT InstCtx;
    PFLT_GENERIC_WORKITEM WorkItem = NULL;
    NTSTATUS Status = STATUS_SUCCESS;
    BOOLEAN WasQueueEmpty;

    UNREFERENCED_PARAMETER( Context );

    DebugTrace( CSQ_TRACE_CBDQ_CALLBACK,
                ("[Csq]: CancelSafe!CsqInsertIo\n") );

    //
    //  Get a pointer to the instance context.
    //

    InstCtx = CONTAINING_RECORD( DataQueue, INSTANCE_CONTEXT, Cbdq );

    //
    //  Save the queue state before inserting to it.
    //

    WasQueueEmpty = IsListEmpty( &InstCtx->QueueHead );

    //
    //  Insert the callback data entry into the queue.
    //

    InsertTailList( &InstCtx->QueueHead,
                    &Data->QueueLinks );

    //
    //  Queue a work item if no worker thread present.
    //

    if (WasQueueEmpty &&
        InterlockedIncrement( &InstCtx->WorkerThreadFlag ) == 1) {

        WorkItem = FltAllocateGenericWorkItem();

        if (WorkItem) {

            Status = FltQueueGenericWorkItem( WorkItem,
                                              InstCtx->Instance,
                                              PreReadWorkItemRoutine,
                                              DelayedWorkQueue,
                                              InstCtx->Instance );

            if (!NT_SUCCESS(Status)) {

                DebugTrace( CSQ_TRACE_CBDQ_CALLBACK | CSQ_TRACE_ERROR,
                            ("[Csq]: Failed to queue the work item (Status = 0x%x)\n",
                            Status) );

                FltFreeGenericWorkItem( WorkItem );
            }

        } else {

            Status = STATUS_INSUFFICIENT_RESOURCES;
        }

        if ( !NT_SUCCESS( Status )) {

            //
            //  Remove the callback data that was inserted into the queue.
            //

            RemoveTailList( &InstCtx->QueueHead );
        }
    }

    return Status;
}


VOID
CsqRemoveIo(
    __in PFLT_CALLBACK_DATA_QUEUE DataQueue,
    __in PFLT_CALLBACK_DATA Data
    )
/*++

Routine Description:

    FltMgr calls this routine to remove an entry from our pending I/O queue.
    The queue is already locked before this routine is called.

Arguments:

    DataQueue - Supplies a pointer to the queue itself.

    Data - Supplies the callback data that is to be removed.

Return Value:

    None.

--*/
{
    UNREFERENCED_PARAMETER( DataQueue );

    DebugTrace( CSQ_TRACE_CBDQ_CALLBACK,
                ("[Csq]: CancelSafe!CsqRemoveIo\n") );

    //
    //  Remove the callback data entry from the queue.
    //

    RemoveEntryList( &Data->QueueLinks );
}


PFLT_CALLBACK_DATA
CsqPeekNextIo(
    __in PFLT_CALLBACK_DATA_QUEUE DataQueue,
    __in_opt PFLT_CALLBACK_DATA Data,
    __in_opt PVOID PeekContext
    )
/*++

Routine Description:

    FltMgr calls this routine to look for an entry on our pending I/O queue.
    The queue is already locked before this routine is called.

Arguments:

    DataQueue - Supplies a pointer to the queue itself.

    Data - Supplies the callback data we should start our search from.
           If this is NULL, we start at the beginning of the list.

    PeekContext - Supplies user-defined context information.

Return Value:

    A pointer to the next callback data structure, or NULL.

--*/
{
    PINSTANCE_CONTEXT InstCtx;
    PLIST_ENTRY NextEntry;
    PFLT_CALLBACK_DATA NextData;

    UNREFERENCED_PARAMETER( PeekContext );

    DebugTrace( CSQ_TRACE_CBDQ_CALLBACK,
                ("[Csq]: CancelSafe!CsqPeekNextIo\n") );

    //
    //  Get a pointer to the instance context.
    //

    InstCtx = CONTAINING_RECORD( DataQueue, INSTANCE_CONTEXT, Cbdq );

    //
    //  If the supplied callback "Data" is NULL, the "NextIo" is the first entry
    //  in the queue; or it is the next list entry in the queue.
    //

    if (Data == NULL) {

        NextEntry = InstCtx->QueueHead.Flink;

    } else {

        NextEntry =  Data->QueueLinks.Flink;
    }

    //
    //  Return NULL if we hit the end of the queue or the queue is empty.
    //

    if (NextEntry == &InstCtx->QueueHead) {

        return NULL;
    }

    NextData = CONTAINING_RECORD( NextEntry, FLT_CALLBACK_DATA, QueueLinks );

    return NextData;
}


VOID
CsqCompleteCanceledIo(
    __in PFLT_CALLBACK_DATA_QUEUE DataQueue,
    __inout PFLT_CALLBACK_DATA Data
    )
/*++

Routine Description:

    FltMgr calls this routine to complete an operation as cancelled that was
    previously pended. The queue is already locked before this routine is called.

Arguments:

    DataQueue - Supplies a pointer to the queue itself.

    Data - Supplies the callback data that is to be canceled.

Return Value:

    None.

--*/
{
    PQUEUE_CONTEXT QueueCtx;

    UNREFERENCED_PARAMETER( DataQueue );

    DebugTrace( CSQ_TRACE_CBDQ_CALLBACK,
                ("[Csq]: CancelSafe!CsqCompleteCanceledIo\n") );

    QueueCtx = (PQUEUE_CONTEXT) Data->QueueContext[0];

    //
    //  Just complete the operation as canceled.
    //

    Data->IoStatus.Status = STATUS_CANCELLED;
    Data->IoStatus.Information = 0;

    FltCompletePendedPreOperation( Data,
                                   FLT_PREOP_COMPLETE,
                                   0 );

    //
    //  Free the extra storage that was allocated for this canceled I/O.
    //

    ExFreeToNPagedLookasideList( &Globals.OueueContextLookaside,
                                 QueueCtx );
}


FLT_PREOP_CALLBACK_STATUS
PreRead (
    __inout PFLT_CALLBACK_DATA Data,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __deref_out_opt PVOID *CompletionContext
    )
/*++

Routine Description:

    Handle pre-read.

Arguments:

    Data - Pointer to the filter callbackData that is passed to us.

    FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance, its associated volume and
        file object.

    CompletionContext - The context for the completion routine for this
        operation.

Return Value:

    The return value is the status of the operation.

--*/
{

    PINSTANCE_CONTEXT InstCtx = NULL;
    PQUEUE_CONTEXT QueueCtx = NULL;
    PFLT_FILE_NAME_INFORMATION NameInfo = NULL;
    NTSTATUS CbStatus = FLT_PREOP_SUCCESS_NO_CALLBACK;
    NTSTATUS Status;

    UNREFERENCED_PARAMETER( CompletionContext );

    DebugTrace( CSQ_TRACE_PRE_READ,
                ("[Csq]: CancelSafe!PreRead\n") );

    //
    //  Skip IRP_PAGING_IO, IRP_SYNCHRONOUS_PAGING_IO and
    //  TopLevelIrp.
    //

    if ((Data->Iopb->IrpFlags & IRP_PAGING_IO) ||
        (Data->Iopb->IrpFlags & IRP_SYNCHRONOUS_PAGING_IO) ||
        IoGetTopLevelIrp()) {

        return FLT_PREOP_SUCCESS_NO_CALLBACK;
    }

    //
    //  Get and parse the file name
    //

    Status = FltGetFileNameInformation( Data,
                                        FLT_FILE_NAME_NORMALIZED
                                          | FLT_FILE_NAME_QUERY_DEFAULT,
                                        &NameInfo );

    if (!NT_SUCCESS( Status )) {

        DebugTrace( CSQ_TRACE_PRE_READ | CSQ_TRACE_ERROR,
                    ("[Csq]: Failed to get filename (Status = 0x%x)\n",
                    Status) );

        goto PreReadCleanup;
    }

    Status = FltParseFileNameInformation( NameInfo );

    if (!NT_SUCCESS( Status )) {

        DebugTrace( CSQ_TRACE_PRE_READ | CSQ_TRACE_ERROR,
                    ("[Csq]: Failed to parse filename (Name = %wZ, Status = 0x%x)\n",
                    &NameInfo->Name,
                    Status) );

        goto PreReadCleanup;
    }

    //
    //  Compare to see if this is the magic file whose I/O is to be pended.
    //

    if (RtlCompareUnicodeString( &NameInfo->FinalComponent,
                                 &CsqFile,
                                 TRUE ) != 0) {

        goto PreReadCleanup;
    }

    //
    //  Since Fast I/O operations cannot be queued, we could return 
    //  FLT_PREOP_SUCCESS_NO_CALLBACK at this point. In this sample,
    //  we disallow Fast I/O for this magic file in order to force an IRP
    //  to be sent to us again. The purpose of doing that is to demonstrate
    //  the cancel safe queue, which may not be true in the real world.
    //

    if (!FLT_IS_IRP_OPERATION( Data )) {

        CbStatus = FLT_PREOP_DISALLOW_FASTIO;
        goto PreReadCleanup;
    }

    //
    //  Allocate a context for each I/O to be inserted into the queue.
    //

    QueueCtx = ExAllocateFromNPagedLookasideList( &Globals.OueueContextLookaside );

    if (QueueCtx == NULL) {

        DebugTrace( CSQ_TRACE_PRE_READ | CSQ_TRACE_ERROR,
                    ("[Csq]: Failed to allocate from NPagedLookasideList (Status = 0x%x)\n",
                    Status) );

        goto PreReadCleanup;
    }

    RtlZeroMemory(QueueCtx, sizeof(QUEUE_CONTEXT));

    //
    //  Get the instance context.
    //

    Status = FltGetInstanceContext( FltObjects->Instance,
                                    &InstCtx );

    if (!NT_SUCCESS( Status )) {

        ASSERT( !"Instance context is missing" );
        goto PreReadCleanup;
    }

    //
    //  Set the queue context
    //

    Data->QueueContext[0] = (PVOID) QueueCtx;
    Data->QueueContext[1] = NULL;

    //
    //  Insert the callback data into the cancel safe queue
    //

    Status = FltCbdqInsertIo( &InstCtx->Cbdq,
                              Data,
                              &QueueCtx->CbdqIoContext,
                              0 );

    if (Status == STATUS_SUCCESS) {

        //
        //  In general, we can create a worker thread here as long as we can
        //  correctly handle the insert/remove race conditions b/w multi threads.
        //  In this sample, the worker thread creation is done in CsqInsertIo.
        //  This is a simpler solution because CsqInsertIo is atomic with 
        //  respect to other CsqXxxIo callback routines.
        //

        CbStatus = FLT_PREOP_PENDING;

    } else {

        DebugTrace( CSQ_TRACE_PRE_READ | CSQ_TRACE_ERROR,
                    ("[Csq]: Failed to insert into cbdq (Status = 0x%x)\n",
                    Status) );
    }

PreReadCleanup:

    //
    //  Clean up
    //

    if (QueueCtx && CbStatus != FLT_PREOP_PENDING) {

        ExFreeToNPagedLookasideList( &Globals.OueueContextLookaside, QueueCtx );
    }

    if (NameInfo) {

        FltReleaseFileNameInformation( NameInfo );
    }

    if (InstCtx) {

        FltReleaseContext( InstCtx );
    }

    return CbStatus;
}


VOID
PreReadWorkItemRoutine(
    __in PFLT_GENERIC_WORKITEM WorkItem,
    __in PFLT_FILTER Filter,
    __in PVOID Context
    )
/*++

Routine Description:

    This WorkItem routine is called in the system thread context to process
    all the pended I/O in this mini filter's cancel safe queue. For each I/O
    in the queue, it completes the I/O after pending the operation for a
    period of time. The thread exits when the queue is empty.

Arguments:

    WorkItem - Unused.

    Filter - Unused.

    Context - Context information.

Return Value:

    None.

--*/
{
    PINSTANCE_CONTEXT InstCtx = NULL;
    PFLT_CALLBACK_DATA Data;
    PFLT_INSTANCE Instance = (PFLT_INSTANCE)Context;
    PQUEUE_CONTEXT QueueCtx;
    NTSTATUS Status;
    FLT_PREOP_CALLBACK_STATUS callbackStatus;

    UNREFERENCED_PARAMETER( WorkItem );
    UNREFERENCED_PARAMETER( Filter );

    DebugTrace( CSQ_TRACE_PRE_READ,
                ("[Csq]: CancelSafe!PreReadWorkItemRoutine\n") );

    //
    //  Get a pointer to the instance context.
    //

    Status = FltGetInstanceContext( Instance,
                                    &InstCtx );

    if (!NT_SUCCESS( Status ))
    {
        ASSERT( !"Instance Context is missing" );
        return;
    }

    //
    //  Process all the pended I/O in the cancel safe queue
    //

    for (;;) {

        callbackStatus = FLT_PREOP_SUCCESS_NO_CALLBACK;

        PreReadPendIo( InstCtx );

        //
        //  WorkerThreadFlag >= 1;
        //  Here we reduce it to 1.
        //

        InterlockedExchange( &InstCtx->WorkerThreadFlag, 1 );

        //
        //  Remove an I/O from the cancel safe queue.
        //

        Data = FltCbdqRemoveNextIo( &InstCtx->Cbdq,
                                    NULL);

        if (Data) {

            QueueCtx = (PQUEUE_CONTEXT) Data->QueueContext[0];

            PreReadProcessIo( Data );

            //
            //  Check to see if we need to lock the user buffer.
            //
            //  If the FLTFL_CALLBACK_DATA_SYSTEM_BUFFER flag is set we don't 
            //  have to lock the buffer because its already a system buffer.
            //
            //  If the MdlAddress is NULL and the buffer is a user buffer, 
            //  then we have to construct one in order to look at the buffer.
            //
            //  If the length of the buffer is zero there is nothing to read,
            //  so we cannot construct a MDL.
            //

            if ( !FlagOn(Data->Flags, FLTFL_CALLBACK_DATA_SYSTEM_BUFFER) && 
                 Data->Iopb->Parameters.Read.MdlAddress == NULL &&
                 Data->Iopb->Parameters.Read.Length > 0 ) {

                Status = FltLockUserBuffer( Data );

                if (!NT_SUCCESS(Status)) {
                    
                    //
                    //  If could not lock the user buffer we cannot
                    //  allow the IO to go below us. Because we are 
                    //  in a different VA space and the buffer is a
                    //  user mode address, we will either fault or 
                    //  corrpt data
                    //
                   
                    DebugTrace( CSQ_TRACE_PRE_READ | CSQ_TRACE_ERROR,
                                ("[Csq]: Failed to lock user buffer (Status = 0x%x)\n",
                                Status) );

                    callbackStatus = FLT_PREOP_COMPLETE;
                    Data->IoStatus.Status = Status;
                }
            }

            //
            //  Complete the I/O
            //

            FltCompletePendedPreOperation( Data,
                                           callbackStatus,
                                           NULL );

            //
            //  Free the extra storage that was allocated for this I/O.
            //

            ExFreeToNPagedLookasideList( &Globals.OueueContextLookaside,
                                         QueueCtx );

        } else {

            //
            //  At this moment it is possible that a new IO is being inserted
            //  into the queue in the CsqInsertIo routine. Now that the queue is
            //  empty, CsqInsertIo needs to make a decision on whether to create
            //  a new worker thread. The decision is based on the race between
            //  the InterlockedIncrement in CsqInsertIo and the
            //  InterlockedDecrement as below. There are two situations:
            //
            //  (1) If the decrement executes earlier before the increment,
            //      the flag will be decremented to 0 so this worker thread
            //      will return. Then CsqInsertIo will increment the flag
            //      from 0 to 1, and therefore create a new worker thread.
            //  (2) If the increment executes earlier before the decrement,
            //      the flag will be first incremented to 2 in CsqInsertIo
            //      so a new worker thread will not be satisfied. Then the
            //      decrement as below will lower the flag down to 1, and
            //      therefore continue this worker thread.
            //

            if (InterlockedDecrement( &InstCtx->WorkerThreadFlag ) == 0)
                break;
        }
    }

    //
    //  Clean up
    //

    FltReleaseContext(InstCtx);

    FltFreeGenericWorkItem(WorkItem);
}


NTSTATUS
PreReadPendIo(
    __in PINSTANCE_CONTEXT InstanceContext
    )
/*++

Routine Description:

    This routine waits for a period of time or until the instance is
    torndown.

Arguments:

    InstanceContext - Supplies a pointer to the instance context.

Return Value:

    The return value is the status of the operation.

--*/
{
    LARGE_INTEGER DueTime;
    NTSTATUS Status;

    //
    //  Delay 15 seconds or get signaled if the instance is torndown.
    //

    DueTime.QuadPart = (LONGLONG) - (15 * 1000 * 1000 * 10);

    Status = KeWaitForSingleObject( &InstanceContext->TeardownEvent,
                                    Executive,
                                    KernelMode,
                                    FALSE,
                                    &DueTime );

    return Status;
}


NTSTATUS
PreReadProcessIo(
    __inout PFLT_CALLBACK_DATA Data
    )
/*++

Routine Description:

    This routine process the I/O that was removed from the queue.

Arguments:

    Data - Supplies the callback data that was removed from the queue.

Return Value:

    The return value is the status of the operation.

--*/
{
    UNREFERENCED_PARAMETER( Data );

    return STATUS_SUCCESS;
}


VOID
PreReadEmptyQueueAndComplete(
    __in PINSTANCE_CONTEXT InstanceContext
    )
/*++

Routine Description:

    This routine empties the cancel safe queue and complete all the
    pended pre-read operations.

Arguments:

    InstanceContext - Supplies a pointer to the instance context.

Return Value:

    None.

--*/
{
    NTSTATUS status;
    FLT_PREOP_CALLBACK_STATUS callbackStatus;
    PFLT_CALLBACK_DATA Data;
    PQUEUE_CONTEXT QueueCtx;

    do {

        callbackStatus = FLT_PREOP_SUCCESS_NO_CALLBACK;

        Data = FltCbdqRemoveNextIo( &InstanceContext->Cbdq,
                                    NULL );

        if (Data) {

            QueueCtx = (PQUEUE_CONTEXT) Data->QueueContext[0];

            //
            //  Check to see if we need to lock the user buffer.
            //
            //  If the FLTFL_CALLBACK_DATA_SYSTEM_BUFFER flag is set we don't 
            //  have to lock the buffer because its already a system buffer.
            //
            //  If the MdlAddress is NULL and the buffer is a user buffer, 
            //  then we have to construct one in order to look at the buffer.
            //
            //  If the length of the buffer is zero there is nothing to read,
            //  so we cannot construct a MDL.
            //

            if ( !FlagOn(Data->Flags, FLTFL_CALLBACK_DATA_SYSTEM_BUFFER) && 
                 Data->Iopb->Parameters.Read.MdlAddress == NULL &&
                 Data->Iopb->Parameters.Read.Length > 0 ) {

                status = FltLockUserBuffer( Data );

                if ( ! NT_SUCCESS( status ) ) {

                    //
                    //  If could not lock the user buffer we cannot
                    //  allow the IO to go below us. Because we are 
                    //  in a different VA space and the buffer is a
                    //  user mode address, we will either fault or 
                    //  corrpt data
                    //
                   
                    callbackStatus = FLT_PREOP_COMPLETE;
                    Data->IoStatus.Status = status;
                }
            }

            FltCompletePendedPreOperation( Data,
                                           callbackStatus,
                                           NULL );

            ExFreeToNPagedLookasideList( &Globals.OueueContextLookaside,
                                         QueueCtx );
        }

    } while (Data);
}

