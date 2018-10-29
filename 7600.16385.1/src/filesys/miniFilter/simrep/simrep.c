/*++

Copyright (c) 1999 - 2002  Microsoft Corporation

Module Name:

    SimRep.c

Abstract:

The Simulate Reparse Sample demonstrates how to return STATUS_REPARSE
on precreates. This allows the filter to redirect opens down one path 
to another path. The Precreate path is complicated by network query opens 
which come down as Fast IO. Fast IO cannot be redirected with Status Reparse
because reparse only works on IRP based IO. 

Simulating reparse points requires that the filter replace the name in the 
file object. This will cause Driver Verifier to complain that the filter is
leaking pool and will prevent it from being unloaded. To solve this issue
SimRep attempts to use a Windows 7 Function called IoReplaceFileObjectName
which will allow IO Mgr to replace the name for us with the correct pool tag.
However, on downlevel OS Versions SimRep will go ahead and replace the name 
itself.

It is important to note that SimRep only demonstrates how to return 
STATUS_REPARSE, not how to deal with file names on NT. SimRep uses two strings
to act as a mapping. When the file open name starts with the "old name mapping"
string the filter replaces it with the "new name mapping" string. This does not
take short names into accound. Additionally we do not filter renames, so 
renames around the mapping will not work correctly. Finially, SimRep does not 
munge directory enumeration queries, which will break so name normalization,



Environment:

    Kernel mode


--*/

//
//  Enabled warnings
//

#pragma warning(error:4100)     //  Enable-Unreferenced formal parameter
#pragma warning(error:4101)     //  Enable-Unreferenced local variable
#pragma warning(error:4061)     //  Enable-missing enumeration in switch statement
#pragma warning(error:4505)     //  Enable-identify dead functions

//
//  Includes
//

#include <fltKernel.h>

//
//  Memory Pool Tags
//

#define SIMREP_STRING_TAG                        'tSpR'

//
// Constants
//

#define REPLACE_ROUTINE_NAME_STRING L"IoReplaceFileObjectName"

//
//  Context sample filter global data structures.
//

typedef struct _MAPPING_ENTRY {

    //
    //  Path underwhich we want to reparse.
    //

    UNICODE_STRING OldName;

    //
    //  Path to reparse to.
    //

    UNICODE_STRING NewName;

} MAPPING_ENTRY, *PMAPPING_ENTRY;


//
//  Starting with windows 7, the IO Manager provides IoReplaceFileObjectName, 
//  but old versions of Windows will not have this function. Rather than just 
//  writing our own function, and forfeiting future windows functionality, we can
//  use MmGetRoutineAddr, which will allow us to dynamically import IoReplaceFileObjectName
//  if it exists. If not it allows us to implement the function ourselves.
//

typedef
NTSTATUS
(* PReplaceFileObjectName ) (
    __in PFILE_OBJECT FileObject,
    __in_bcount(FileNameLength) PWSTR NewFileName,
    __in USHORT FileNameLength
    );


typedef struct _SIMREP_GLOBAL_DATA {

    //
    // Handle to minifilter returned from FltRegisterFilter()
    //

    PFLT_FILTER Filter;

    //
    //  Structure to hold mapping information.
    //

    MAPPING_ENTRY Mapping;

    //
    //  Pointer to the function we will use to 
    //  replace file names.
    //

    PReplaceFileObjectName ReplaceFileNameFunction;

    
#if DBG

    //
    // Field to control nature of debug output
    //
    
    ULONG DebugLevel;
#endif


} SIMREP_GLOBAL_DATA, *PSIMREP_GLOBAL_DATA;


//
//  Debug helper functions
//

#if DBG


#define DEBUG_TRACE_ERROR                               0x00000001  // Errors - whenever we return a failure code
#define DEBUG_TRACE_LOAD_UNLOAD                         0x00000002  // Loading/unloading of the filter
#define DEBUG_TRACE_INSTANCES                           0x00000004  // Attach / detach of instances

#define DEBUG_TRACE_REPARSE_OPERATIONS                  0x00000008  // Operations that are performed to determine if we should return STATUS_REPARSE
#define DEBUG_TRACE_REPARSED_OPERATIONS                 0x00000010  // Operations that return STATUS_REPARSE
#define DEBUG_TRACE_REPARSED_REISSUE                    0X00000020  // Operations that need to be reissued with an IRP.

#define DEBUG_TRACE_ALL_IO                              0x00000040  // All IO operations tracked by this filter

#define DEBUG_TRACE_ALL                                 0xFFFFFFFF  // All flags


#define DebugTrace(Level, Data)                     \
    if ((Level) & Globals.DebugLevel) {             \
        DbgPrint Data;                              \
    }


#else

#define DebugTrace(Level, Data)             {NOTHING;}

#endif


//
//  Function that handle driver load/unload and instance setup/cleanup
//

DRIVER_INITIALIZE DriverEntry;
NTSTATUS
DriverEntry (
    __in PDRIVER_OBJECT DriverObject,
    __in PUNICODE_STRING RegistryPath
    );



#if DBG

NTSTATUS
SimRepInitializeDebugLevel (
    __in PUNICODE_STRING RegistryPath
    );

#endif

NTSTATUS 
SimRepInitializeMapping( __in PUNICODE_STRING RegistryPath );

void SimRepFreeGlobals();

NTSTATUS
SimRepUnload (
    FLT_FILTER_UNLOAD_FLAGS Flags
    );


NTSTATUS
SimRepInstanceSetup (
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in FLT_INSTANCE_SETUP_FLAGS Flags,
    __in DEVICE_TYPE VolumeDeviceType,
    __in FLT_FILESYSTEM_TYPE VolumeFilesystemType
    );

NTSTATUS
SimRepInstanceQueryTeardown (
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags
    );

//
//  Functions that track operations on the volume
//

FLT_PREOP_CALLBACK_STATUS
SimRepPreCreate (
    __inout PFLT_CALLBACK_DATA Cbd,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __out PVOID *CompletionContext
    );

//
//  Functions that provide string allocation support
//

NTSTATUS
SimRepAllocateUnicodeString (
    PUNICODE_STRING String
    );

VOID
SimRepFreeUnicodeString (
    PUNICODE_STRING String
    );

NTSTATUS
SimRepReplaceFileObjectName (
    __in PFILE_OBJECT FileObject,
    __in_bcount(FileNameLength) PWSTR NewFileName,
    __in USHORT FileNameLength
    );

//
//  Filter callback routines
//

FLT_OPERATION_REGISTRATION Callbacks[] = {

    { IRP_MJ_CREATE,
        FLTFL_OPERATION_REGISTRATION_SKIP_PAGING_IO,
        SimRepPreCreate,
        NULL },

    { IRP_MJ_NETWORK_QUERY_OPEN,
        FLTFL_OPERATION_REGISTRATION_SKIP_PAGING_IO,
        SimRepPreCreate,
        NULL },

    { IRP_MJ_OPERATION_END }
};

//
// Filter registration data structure
//

FLT_REGISTRATION FilterRegistration = {

    sizeof( FLT_REGISTRATION ),                     //  Size
    FLT_REGISTRATION_VERSION,                       //  Version
    0,                                              //  Flags
    NULL,                                           //  Context
    Callbacks,                                      //  Operation callbacks
    SimRepUnload,                                   //  Filters unload routine
    SimRepInstanceSetup,                            //  InstanceSetup routine
    SimRepInstanceQueryTeardown,                    //  InstanceQueryTeardown routine
    NULL,                                           //  InstanceTeardownStart routine
    NULL,                                           //  InstanceTeardownComplete routine
    NULL, NULL, NULL                                //  Unused naming support callbacks
};

//
//  Global variables
//

SIMREP_GLOBAL_DATA Globals;

//
//  Assign text sections for each routine.
//

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)

#if DBG
#pragma alloc_text(INIT, SimRepInitializeDebugLevel)
#endif

#pragma alloc_text(PAGE, SimRepInitializeMapping)
#pragma alloc_text(PAGE, SimRepUnload)
#pragma alloc_text(PAGE, SimRepInstanceSetup)
#pragma alloc_text(PAGE, SimRepInstanceQueryTeardown)
#pragma alloc_text(PAGE, SimRepAllocateUnicodeString)
#pragma alloc_text(PAGE, SimRepFreeUnicodeString)
#pragma alloc_text(PAGE, SimRepReplaceFileObjectName)
#pragma alloc_text(PAGE, SimRepPreCreate)
#pragma alloc_text(PAGE, SimRepFreeGlobals)
#endif

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
    NTSTATUS status;
    UNICODE_STRING replaceRoutineName;

    //
    //  Zero Out Globals
    //

    RtlZeroMemory( &Globals, sizeof( Globals ) );

#if DBG

    //
    //  Initialize global debug level
    //

    status = SimRepInitializeDebugLevel( RegistryPath );

    if (!NT_SUCCESS(status) ) {

        goto DriverEntryCleanup;
    }

#endif

    DebugTrace( DEBUG_TRACE_LOAD_UNLOAD,
                ("[SimRep]: Driver being loaded\n") );

    //
    //  Import function to replace file names.
    //

    RtlInitUnicodeString( &replaceRoutineName, REPLACE_ROUTINE_NAME_STRING );

#pragma warning(push)
#pragma warning(disable:4152) // nonstandard extension, function/data pointer conversion in expression

    Globals.ReplaceFileNameFunction = MmGetSystemRoutineAddress( &replaceRoutineName );
    if (Globals.ReplaceFileNameFunction == NULL) {

        Globals.ReplaceFileNameFunction = SimRepReplaceFileObjectName;
    }

#pragma warning(pop)

    //
    //  Set up the mapping
    //

    status = SimRepInitializeMapping( RegistryPath );

    if (!NT_SUCCESS( status )) {

        goto DriverEntryCleanup;
    }

    //
    //  Register with the filter manager
    //

    status = FltRegisterFilter( DriverObject,
                                &FilterRegistration,
                                &Globals.Filter );

    if (!NT_SUCCESS( status )) {

        goto DriverEntryCleanup;
    }

    //
    //  Start filtering I/O
    //

    status = FltStartFiltering( Globals.Filter );

    if (!NT_SUCCESS( status )) {

        FltUnregisterFilter( Globals.Filter );
    }

    DebugTrace( DEBUG_TRACE_LOAD_UNLOAD,
                ("[SimRep]: Driver loaded complete (Status = 0x%08X)\n",
                status) );

DriverEntryCleanup:

    if (!NT_SUCCESS(status)) {

        SimRepFreeGlobals();
    }

    return status;
}

#if DBG

NTSTATUS
SimRepInitializeDebugLevel (
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
    BOOLEAN closeHandle = FALSE;
    NTSTATUS status;
    ULONG resultLength;
    UNICODE_STRING valueName;
    UCHAR buffer[sizeof( KEY_VALUE_PARTIAL_INFORMATION ) + sizeof( LONG )];

    Globals.DebugLevel = DEBUG_TRACE_ALL;

    //
    //  Open the desired registry key
    //

    InitializeObjectAttributes( &attributes,
                                RegistryPath,
                                OBJ_CASE_INSENSITIVE,
                                NULL,
                                NULL );

    status = ZwOpenKey( &driverRegKey,
                        KEY_READ,
                        &attributes );

    if (!NT_SUCCESS( status )) {
        
        goto SimRepInitializeDebugLevelCleanup;
    }

    closeHandle = TRUE;

    //
    // Read the DebugFlags value from the registry.
    //

    RtlInitUnicodeString( &valueName, L"DebugLevel" );
        
    status = ZwQueryValueKey( driverRegKey,
                              &valueName,
                              KeyValuePartialInformation,
                              buffer,
                              sizeof(buffer),
                              &resultLength );

    if (!NT_SUCCESS( status )) {

        goto SimRepInitializeDebugLevelCleanup;
    }

    Globals.DebugLevel = *((PULONG) &(((PKEY_VALUE_PARTIAL_INFORMATION) buffer)->Data));

SimRepInitializeDebugLevelCleanup:

    //
    //  Close the registry entry
    //

    if (closeHandle) {

        ZwClose( driverRegKey );
    }

    return status;
}

#endif

NTSTATUS 
SimRepInitializeMapping( __in PUNICODE_STRING RegistryPath )
/*++

Routine Descrition:

    This routine initializes the mapping structure. It will
    try to populate it from the registry, and if that fails
    use a default string.

Arguments:

    RegistryPath - The path key passed to the driver during DriverEntry.

Return Value:

    None.

--*/
{
    NTSTATUS status;
    OBJECT_ATTRIBUTES attributes;
    HANDLE driverRegKey = NULL;
    UNICODE_STRING valueName;
    BOOLEAN closeHandle = FALSE;

    PKEY_VALUE_PARTIAL_INFORMATION oldMappingBuffer = NULL;
    ULONG oldMappingKeyLength = 0;

    PKEY_VALUE_PARTIAL_INFORMATION newMappingBuffer = NULL;
    ULONG newMappingKeyLength = 0;

    PAGED_CODE();

    //
    //  Open the mapping registry key.
    //

    InitializeObjectAttributes( &attributes,
                                RegistryPath,
                                OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
                                NULL,
                                NULL );

    status = ZwOpenKey( &driverRegKey,
                        KEY_READ,
                        &attributes );

    if (!NT_SUCCESS( status )) {

        goto SimRepInitializeMappingCleanup;
    }

    closeHandle = TRUE;

    //
    //  Query the length of the old mapping.
    //
    
    RtlInitUnicodeString( &valueName, L"OldMapping" );

    status = ZwQueryValueKey( driverRegKey,
                              &valueName,
                              KeyValuePartialInformation,
                              NULL,
                              0,
                              &oldMappingKeyLength );

    if (status!=STATUS_BUFFER_TOO_SMALL && status!=STATUS_BUFFER_OVERFLOW) {

        status = STATUS_INVALID_PARAMETER;
        goto SimRepInitializeMappingCleanup;
    }

    //
    //  Extract the old mapping string.
    //

    oldMappingBuffer = ExAllocatePoolWithTag( NonPagedPool,
                                              oldMappingKeyLength,
                                              SIMREP_STRING_TAG);

    if (oldMappingBuffer == NULL) {

        status = STATUS_INSUFFICIENT_RESOURCES;
        goto SimRepInitializeMappingCleanup;
    }

    status = ZwQueryValueKey( driverRegKey,
                              &valueName,
                              KeyValuePartialInformation,
                              oldMappingBuffer,
                              oldMappingKeyLength,
                              &oldMappingKeyLength);

    if (!NT_SUCCESS(status)) {

        goto SimRepInitializeMappingCleanup;
    }

    //
    //  Query the length of the new mapping.
    //
    
    RtlInitUnicodeString( &valueName, L"NewMapping" );

    status = ZwQueryValueKey( driverRegKey,
                              &valueName,
                              KeyValuePartialInformation,
                              NULL,
                              0,
                              &newMappingKeyLength );

    if (status!=STATUS_BUFFER_TOO_SMALL && status!=STATUS_BUFFER_OVERFLOW) {

        status = STATUS_INVALID_PARAMETER;
        goto SimRepInitializeMappingCleanup;
    }

    //
    //  Extract the new mapping string.
    //

    newMappingBuffer = ExAllocatePoolWithTag( NonPagedPool,
                                              newMappingKeyLength,
                                              SIMREP_STRING_TAG);

    if (newMappingBuffer == NULL) {

        status = STATUS_INSUFFICIENT_RESOURCES;
        goto SimRepInitializeMappingCleanup;
    }

    status = ZwQueryValueKey( driverRegKey,
                              &valueName,
                              KeyValuePartialInformation,
                              newMappingBuffer,
                              newMappingKeyLength,
                              &newMappingKeyLength);

    if (!NT_SUCCESS(status)) {

        goto SimRepInitializeMappingCleanup;
    }

    //
    //  Now that we have both of the mapping strings we can set up the mapping in globals.
    //  When extracting the path from the registry key its important to remember to not copy the trailing
    //  null terminator because the string comparisons we will do in create should not use the null.
    //

    //
    //  Set up the new mapping
    //

    Globals.Mapping.NewName.MaximumLength = (USHORT) newMappingBuffer->DataLength-sizeof(WCHAR);
    status = SimRepAllocateUnicodeString( &Globals.Mapping.NewName );

    if (!NT_SUCCESS(status)) {

        goto SimRepInitializeMappingCleanup;
    }

    RtlCopyMemory( Globals.Mapping.NewName.Buffer, 
                   newMappingBuffer->Data, 
                   newMappingBuffer->DataLength-sizeof(WCHAR) );
    Globals.Mapping.NewName.Length = (USHORT) newMappingBuffer->DataLength-sizeof(WCHAR);

    //
    //  Set up the old mapping
    //

    Globals.Mapping.OldName.MaximumLength = (USHORT) oldMappingBuffer->DataLength-sizeof(WCHAR);

    status = SimRepAllocateUnicodeString( &Globals.Mapping.OldName );

    if (!NT_SUCCESS(status)) {

        goto SimRepInitializeMappingCleanup;
    }

    RtlCopyMemory( Globals.Mapping.OldName.Buffer, 
                   oldMappingBuffer->Data, 
                   oldMappingBuffer->DataLength-sizeof(WCHAR) );
    Globals.Mapping.OldName.Length = (USHORT) oldMappingBuffer->DataLength-sizeof(WCHAR);

SimRepInitializeMappingCleanup:

    //
    //  Note that this function leaks buffers in Globals.Mapping.NewName
    //  and Globals.Mapping.OldName. On failure DriverEntry will clean up
    //  the Globals structure so we don't have to do that here.
    //

    if (newMappingBuffer != NULL) {

        ExFreePoolWithTag( newMappingBuffer, SIMREP_STRING_TAG );
        newMappingBuffer = NULL;
    }

    if (oldMappingBuffer != NULL) {

        ExFreePoolWithTag( oldMappingBuffer, SIMREP_STRING_TAG );
        newMappingBuffer = NULL;
    }

    if (closeHandle) {

        status = ZwClose( driverRegKey );
        ASSERT( NT_SUCCESS(status) );
    }

    return status;
}

void SimRepFreeGlobals()
/*++

Routine Descrition:

    This routine cleans up the global structure on both
    teardown and initialization failure.

Arguments:

Return Value:

    None.

--*/
{
    PAGED_CODE();

    SimRepFreeUnicodeString( &Globals.Mapping.NewName );
    SimRepFreeUnicodeString( &Globals.Mapping.OldName );
}

NTSTATUS
SimRepUnload (
    FLT_FILTER_UNLOAD_FLAGS Flags
    )
/*++

Routine Description:

    This is the unload routine for this filter driver. This is called 
    when the minifilter is about to be unloaded. SimRep can unload 
    easily because it does not own any IOs. When the filter is unloaded
    existing reparsed creates will continue to work, but new creates will
    not be reparsed. This is fine from the filter's perspective, but could
    result in unexpected bahavior for apps.

Arguments:

    Flags - Indicating if this is a mandatory unload.

Return Value:

    Returns the final status of this operation.

--*/
{
    UNREFERENCED_PARAMETER( Flags );

    PAGED_CODE();

    DebugTrace( DEBUG_TRACE_LOAD_UNLOAD,
                ("[SimRep]: Unloading driver\n") );

    FltUnregisterFilter( Globals.Filter );

    SimRepFreeGlobals();

    return STATUS_SUCCESS;
}


//
//  Instance setup/teardown routines.
//

NTSTATUS
SimRepInstanceSetup (
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in FLT_INSTANCE_SETUP_FLAGS Flags,
    __in DEVICE_TYPE VolumeDeviceType,
    __in FLT_FILESYSTEM_TYPE VolumeFilesystemType
    )
/*++

Routine Description:

    This routine is called whenever a new instance is created on a volume. This
    gives us a chance to decide if we need to attach to this volume or not.
    SimRep does not attach on automatic attachment, but will attach when asked
    manually.

Arguments:

    FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance and its associated volume.

    Flags - Flags describing the reason for this attach request.

Return Value:

    STATUS_SUCCESS - attach
    STATUS_FLT_DO_NOT_ATTACH - do not attach

--*/
{
    
    UNREFERENCED_PARAMETER( FltObjects );
    UNREFERENCED_PARAMETER( Flags );
    UNREFERENCED_PARAMETER( VolumeDeviceType );
    UNREFERENCED_PARAMETER( VolumeFilesystemType );

    PAGED_CODE();

    if ( FlagOn( Flags, FLTFL_INSTANCE_SETUP_AUTOMATIC_ATTACHMENT ) ) {

        //
        //  Do not automatically attach to a volume.
        //

        DebugTrace( DEBUG_TRACE_INSTANCES,
                    ("[Simrep]: Instance setup skipped (Volume = %p, Instance = %p)\n",
                    FltObjects->Volume,
                    FltObjects->Instance) );

        return STATUS_FLT_DO_NOT_ATTACH;
    }

    //
    //  Attach on manual attachment.
    //

    DebugTrace( DEBUG_TRACE_INSTANCES, 
                ("[SimRep]: Instance setup started (Volume = %p, Instance = %p)\n",
                 FltObjects->Volume, 
                 FltObjects->Instance) );


    return STATUS_SUCCESS;
}

NTSTATUS
SimRepInstanceQueryTeardown (
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags
    )
/*++

Routine Description:

    This is called when an instance is being manually deleted by a
    call to FltDetachVolume or FilterDetach thereby giving us a
    chance to fail that detach request. SimRep only implements it 
    because otherwise calls to FltDetachVolume or FilterDetach would
    fail to detach.

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

    DebugTrace( DEBUG_TRACE_INSTANCES,
                ("[SimRep]: Instance query teadown ended (Instance = %p)\n",
                 FltObjects->Instance) );

    return STATUS_SUCCESS;
}



FLT_PREOP_CALLBACK_STATUS
SimRepPreCreate (
    __inout PFLT_CALLBACK_DATA Cbd,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __out PVOID *CompletionContext
    )
/*++

Routine Description:

    This routine does the work for SimRep sample. SimRepPreCreate is called in 
    the pre-operation path for IRP_MJ_CREATE and IRP_MJ_NETWORK_QUERY_OPEN. 
    The function queries the requested file name for  the create and compares
    it to the mapping path. If the file is down the "old mapping path", the 
    filter checks to see if the request is fast io based. If it is we cannot
    reparse the create because fast io does not support STATUS_REPARSE. 
    Instead we return FLT_PREOP_DISALLOW_FASTIO to force the io to be reissued 
    on the IRP path. If the create is IRP based, then we replace the file 
    object's file name field with a new path based on the "new mapping path".

    This is pageable because it could not be called on the paging path

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
    PFLT_FILE_NAME_INFORMATION nameInfo = NULL;
    NTSTATUS status;
    FLT_PREOP_CALLBACK_STATUS callbackStatus;
    UNICODE_STRING fileName; //The open name from the end of the volume name to the end.
    UNICODE_STRING newFileName; //The output file name.
    UNICODE_STRING matchFileName; //The string we use to do the prefix comparison.

    UNREFERENCED_PARAMETER( FltObjects );
    UNREFERENCED_PARAMETER( CompletionContext );
        
    PAGED_CODE();
    
    DebugTrace( DEBUG_TRACE_ALL_IO,
                ("[SimRep]: SimRepPreCreate -> Enter (Cbd = %p, FileObject = %p)\n",
                 Cbd,
                 FltObjects->FileObject) );


    //
    // Initialize defaults
    //

    status = STATUS_SUCCESS;
    callbackStatus = FLT_PREOP_SUCCESS_NO_CALLBACK; // pass through - default is no post op callback
    newFileName.Buffer = NULL;
    newFileName.Length = newFileName.MaximumLength = 0;
    
    //
    // We only registered for these two irps, so thats all we better get!
    //

    ASSERT( Cbd->Iopb->MajorFunction == IRP_MJ_CREATE ||
            Cbd->Iopb->MajorFunction == IRP_MJ_NETWORK_QUERY_OPEN );

    //
    //  Check if this is a paging file as we don't want to redirect
    //  the location of the paging file.
    //

    if (FlagOn( Cbd->Iopb->OperationFlags, SL_OPEN_PAGING_FILE )) {

        DebugTrace( DEBUG_TRACE_ALL_IO,
                    ("[SimRep]: SimRepPreCreate -> Ignoring paging file open (Cbd = %p, FileObject = %p)\n",
                     Cbd,
                     FltObjects->FileObject) );

        goto SimRepPreCreateCleanup;
    }

    //
    //  We are not allowing volume opens to be reparsed in the sample. 
    //

    if (FlagOn( Cbd->Iopb->TargetFileObject->Flags, FO_VOLUME_OPEN )) { 

        DebugTrace( DEBUG_TRACE_ALL_IO,
                    ("[SimRep]: SimRepPreCreate -> Ignoring volume open (Cbd = %p, FileObject = %p)\n",
                     Cbd,
                     FltObjects->FileObject) );

        goto SimRepPreCreateCleanup;

    }
    
    //
    //  Get the name information.
    //  Note that we use FLT_FILE_NAME_QUERY_ALWAYS_ALLOW_CACHE_LOOKUP 
    //  instead of FLT_FILE_NAME_QUERY_DEFAULT. 
    //
    //  In some cases it is not safe for filter manager to do generate a 
    //  file name, and FLT_FILE_NAME_QUERY_DEFAULT will detech those cases
    //  and fail without looking in the cache. 
    //  FLT_FILE_NAME_QUERY_ALWAYS_ALLOW_CACHE_LOOKUP always checks the cache,
    //  and then queries the file system if its safe. This is a safer option, 
    //  even though it should always be safe to query on a pre-create.
    //

    status = FltGetFileNameInformation( Cbd,
                                        FLT_FILE_NAME_OPENED |
                                        FLT_FILE_NAME_QUERY_ALWAYS_ALLOW_CACHE_LOOKUP,
                                        &nameInfo );
    if (!NT_SUCCESS( status )) {

        DebugTrace( DEBUG_TRACE_REPARSE_OPERATIONS | DEBUG_TRACE_ERROR,
                    ("[SimRep]: SimRepPreCreate -> Failed to get name information (Cbd = %p, FileObject = %p)\n",
                     Cbd,
                     FltObjects->FileObject) );

        goto SimRepPreCreateCleanup;
    }


    DebugTrace( DEBUG_TRACE_REPARSE_OPERATIONS,
                ("[SimRep]: SimRepPreCreate -> Processing create for file %wZ (Cbd = %p, FileObject = %p)\n",
                 &nameInfo->Name,
                 Cbd,
                 FltObjects->FileObject) );
    
    //
    //  Parse the filename information
    //

    status = FltParseFileNameInformation( nameInfo );
    if (!NT_SUCCESS( status )) {
    
        DebugTrace( DEBUG_TRACE_REPARSE_OPERATIONS | DEBUG_TRACE_ERROR,
                    ("[SimRep]: SimRepPreCreate -> Failed to parse name information for file %wZ (Cbd = %p, FileObject = %p)\n",
                     &nameInfo->Name,
                     Cbd,
                     FltObjects->FileObject) );
    
        goto SimRepPreCreateCleanup;
    }


    //
    //  Point filename to the name of the file, excluding the name of the volume
    //

    ASSERT( nameInfo->Name.Buffer == nameInfo->Volume.Buffer );    
    fileName.Buffer = Add2Ptr( nameInfo->Name.Buffer, nameInfo->Volume.Length );
    fileName.Length = nameInfo->Name.Length - nameInfo->Volume.Length;
    fileName.MaximumLength = fileName.Length;


    //
    //  Check if we need to map this open to another file instead
    //

    //
    //  Initialize the part of the filename we are going to match against.
    //  Note that machFileName.Length could extend beyond the buffer,
    //  but we only access it after checking the bounds first.
    //

    matchFileName.Buffer = fileName.Buffer;
    matchFileName.Length = Globals.Mapping.OldName.Length;
    matchFileName.MaximumLength = matchFileName.Length;
        
        

    //
    //  Check if the filename matches this mapping entry
    //
    //  Case 1: The filename being opened in the mapping entry itself
    //  Case 2: The filename is some child directory of the mapping entry
    //

    if (((fileName.Length == Globals.Mapping.OldName.Length) &&
                RtlEqualUnicodeString( &matchFileName, &Globals.Mapping.OldName, TRUE )) 

            ||

            ((fileName.Length > Globals.Mapping.OldName.Length) &&
             (fileName.Buffer[(Globals.Mapping.OldName.Length/2)] == L'\\') &&  
             RtlEqualUnicodeString( &matchFileName, &Globals.Mapping.OldName, TRUE )) ) {

        DebugTrace( DEBUG_TRACE_REPARSE_OPERATIONS,
                    ("[SimRep]: SimRepPreCreate -> File name %wZ matches mapping. (Cbd = %p, FileObject = %p)\n"
                     "\tOpenedFileName =  %wZ\n"
                     "\tMapping.OldFileName = %wZ\n"
                     "\tMapping.NewFileName = %wZ\n",
                     &fileName, Cbd, FltObjects->FileObject,
                     &nameInfo->Name,
                     Globals.Mapping.OldName,
                     Globals.Mapping.NewName) );

        //
        // Because the file matched the mapping, we need to redirect this open with a new name.
        //

        //
        // If this is a network query open we can't cant return STATUS_REPARSE because its FastIO.
        // So we will return FLT_PREOP_DISALLOW_FASTIO, so it will be reissued down the slow path.
        //

        if (Cbd->Iopb->MajorFunction == IRP_MJ_NETWORK_QUERY_OPEN) {

            DebugTrace(DEBUG_TRACE_REPARSED_REISSUE, 
                        ("[SimRep]: Disallow fast IO that is to a mapped path! %wZ\n",
                         &nameInfo->Name));

            callbackStatus = FLT_PREOP_DISALLOW_FASTIO;
            goto SimRepPreCreateCleanup;
        }
        

        //
        //  Calculate the new length of the file name
        //
            
        newFileName.MaximumLength = nameInfo->Name.Length - Globals.Mapping.OldName.Length + Globals.Mapping.NewName.Length;

        //
        //  Allocate a unicode string for the new filename
        //
            
        status = SimRepAllocateUnicodeString( &newFileName );

        if (!NT_SUCCESS( status )) {
            
            DebugTrace( DEBUG_TRACE_REPARSE_OPERATIONS | DEBUG_TRACE_ERROR,
                        ("[SimRep]: SimRepPreCreate -> Failed to allocate unicode scratch string for file %wZ (Cbd = %p, FileObject = %p)\n",
                         &nameInfo->Name,
                         Cbd,
                         FltObjects->FileObject) );
            
            goto SimRepPreCreateCleanup;
        }

        //
        //  Construct the new filename
        //

        //
        //  Copy the volume portion of the name (part of the name preceding the matching part)
        //
            
        RtlCopyUnicodeString( &newFileName,
                              &nameInfo->Volume );
                              
        //
        //  Copy the new file name in place of the matching part of the name
        //
         
        status = RtlAppendUnicodeStringToString( &newFileName,
                                        &Globals.Mapping.NewName );

        ASSERT( NT_SUCCESS(status) );

        //
        //  Copy the portion of the name following the matching part of the name
        //

        RtlCopyMemory( Add2Ptr( newFileName.Buffer, nameInfo->Volume.Length + Globals.Mapping.NewName.Length ), 
                       Add2Ptr( fileName.Buffer, Globals.Mapping.OldName.Length ), 
                       fileName.Length - Globals.Mapping.OldName.Length );


        //
        //  Compute the final length of the new name
        //
            
        newFileName.Length = nameInfo->Volume.Length + Globals.Mapping.NewName.Length + fileName.Length - Globals.Mapping.OldName.Length;


        //
        //  Switch names
        //

        status = Globals.ReplaceFileNameFunction( Cbd->Iopb->TargetFileObject,
                                                  newFileName.Buffer,
                                                  newFileName.Length );

        if ( !NT_SUCCESS( status )) {

            DebugTrace( DEBUG_TRACE_REPARSE_OPERATIONS | DEBUG_TRACE_ERROR,
                        ("[SimRep]: SimRepPreCreate -> Failed to allocate string for file %wZ (Cbd = %p, FileObject = %p)\n",
                        &nameInfo->Name,
                        Cbd,
                        FltObjects->FileObject ));

            goto SimRepPreCreateCleanup;
        }

        //
        //  Set the status to STATUS_REPARSE
        //

        status = STATUS_REPARSE;


        DebugTrace( DEBUG_TRACE_REPARSE_OPERATIONS | DEBUG_TRACE_REPARSED_OPERATIONS,
                    ("[SimRep]: SimRepPreCreate -> Returning STATUS_REPARSE for file %wZ. (Cbd = %p, FileObject = %p)\n"
                     "\tOpenedFileName =  %wZ\n"
                     "\tNewName = %wZ\n",
                     &fileName, Cbd, FltObjects->FileObject,
                     &nameInfo->Name,
                     &newFileName) );
        
            
        goto SimRepPreCreateCleanup;


    }

SimRepPreCreateCleanup:
    
    //
    //  Release the references we have acquired
    //    

    SimRepFreeUnicodeString( &newFileName );
    
    if (nameInfo != NULL) {

        FltReleaseFileNameInformation( nameInfo );
    }

    if (status == STATUS_REPARSE) {

        //
        //  Reparse the open
        //

        Cbd->IoStatus.Status = STATUS_REPARSE;
        Cbd->IoStatus.Information = IO_REPARSE;
        callbackStatus = FLT_PREOP_COMPLETE;        
    }   
    else if (!NT_SUCCESS( status )) {

        //
        //  An error occurred, fail the open
        //

        DebugTrace( DEBUG_TRACE_ERROR,
                    ("[SimRep]: SimRepPreCreate -> Failed with status 0x%x \n",
                    status) );
            
        Cbd->IoStatus.Status = status;
        callbackStatus = FLT_PREOP_COMPLETE;
    }

    DebugTrace( DEBUG_TRACE_ALL_IO,
                ("[SimRep]: SimRepPreCreate -> Exit (Cbd = %p, FileObject = %p)\n",
                 Cbd,
                 FltObjects->FileObject) );

    return callbackStatus;

}

//
//  Support Routines
//

NTSTATUS
SimRepAllocateUnicodeString (
    PUNICODE_STRING String
    )
/*++

Routine Description:

    This routine allocates a unicode string

Arguments:

    String - supplies the size of the string to be allocated in the MaximumLength field 
             return the unicode string

Return Value:

    STATUS_SUCCESS                  - success
    STATUS_INSUFFICIENT_RESOURCES   - failure
  
--*/
{

    PAGED_CODE();

    String->Buffer = ExAllocatePoolWithTag( NonPagedPool,
                                            String->MaximumLength,
                                            SIMREP_STRING_TAG );

    if (String->Buffer == NULL) {

        DebugTrace( DEBUG_TRACE_ERROR,
                    ("[SimRep]: Failed to allocate unicode string of size 0x%x\n",
                    String->MaximumLength) );

        return STATUS_INSUFFICIENT_RESOURCES;
    }

    String->Length = 0;

    return STATUS_SUCCESS;
}

VOID
SimRepFreeUnicodeString (
    PUNICODE_STRING String
    )
/*++

Routine Description:

    This routine frees a unicode string

Arguments:

    String - supplies the string to be freed 

Return Value:

    None    

--*/
{
    PAGED_CODE();

    if (String->Buffer) {

        ExFreePoolWithTag( String->Buffer,
                           SIMREP_STRING_TAG );
        String->Buffer = NULL;
    }

    String->Length = String->MaximumLength = 0;
    String->Buffer = NULL;
}

NTSTATUS
SimRepReplaceFileObjectName (
    __in PFILE_OBJECT FileObject,
    __in_bcount(FileNameLength) PWSTR NewFileName,
    __in USHORT FileNameLength
    )
/*++
Routine Description:

    This routine is used to replace a file object's name
    with a provided name. This should only be called if 
    IoReplaceFileObjectName is not on the system.
    If this function is used and verifier is enabled
    the filter will fail to unload due to a false 
    positive on the leaked pool test.

Arguments:

    FileObject - Pointer to file object whose name is to be replaced.

    NewFileName - Pointer to buffer containing the new name.

    FileNameLength - Length of the new name in bytes.

Return Value:

    STATUS_INSUFFICIENT_RESOURCES - No memory to allocate the new buffer.

    STATUS_SUCCESS otherwise.

--*/
{
    PWSTR buffer;
    PUNICODE_STRING fileName;
    USHORT newMaxLength;

    PAGED_CODE();

    fileName = &FileObject->FileName;

    //
    // If the new name fits inside the current buffer we simply copy it over
    // instead of allocating a new buffer (and keep the MaximumLength value
    // the same).
    //
    if (FileNameLength <= fileName->MaximumLength) {

        goto CopyAndReturn;
    }

    //
    // Use an optimal buffer size
    //
    newMaxLength = FileNameLength;

    buffer = ExAllocatePoolWithTag( PagedPool,
                                    newMaxLength,
                                    SIMREP_STRING_TAG );

    if (!buffer) {

        return STATUS_INSUFFICIENT_RESOURCES;
    }

    if (fileName->Buffer != NULL) {

        ExFreePool(fileName->Buffer);
    }

    fileName->Buffer = buffer;
    fileName->MaximumLength = newMaxLength;

CopyAndReturn:

    fileName->Length = FileNameLength;
    RtlZeroMemory(fileName->Buffer, fileName->MaximumLength);
    RtlCopyMemory(fileName->Buffer, NewFileName, FileNameLength);

    return STATUS_SUCCESS;
}


