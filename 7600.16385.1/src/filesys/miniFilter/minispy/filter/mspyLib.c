/*++

Copyright (c) 1989-2002  Microsoft Corporation

Module Name:

    mspyLib.c

Abstract:
    This contains library support routines for MiniSpy

Environment:

    Kernel mode

--*/

#include "mspyKern.h"

//---------------------------------------------------------------------------
//  Assign text sections for each routine.
//---------------------------------------------------------------------------

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(INIT, SpyReadDriverParameters)
#endif

UCHAR TxNotificationToMinorCode (
    __in ULONG TxNotification
    )
/*++

Routine Description:

    This routine has been written to convert a transaction notification code
    to an Irp minor code. This function is needed because RECORD_DATA has a
    UCHAR field for the Irp minor code whereas TxNotification is ULONG. As
    of now all this function does is compute log_base_2(TxNotification) + 1.
    That fits our need for now but might have to be evolved later. This
    function is intricately tied with the enumeration TRANSACTION_NOTIFICATION_CODES
    in mspyLog.h and the case statements related to transactions in the function
    PrintIrpCode (Minispy\User\mspyLog.c).

Arguments:

    TxNotification - The transaction notification received.

Return Value:

    0 if TxNotification is 0;
    log_base_2(TxNotification) + 1 otherwise.

--*/
{
    UCHAR count = 0;

    if (TxNotification == 0)
        return 0;

    //
    //  This assert verifies if no more than one flag is set
    //  in the TxNotification variable. TxNotification flags are
    //  supposed to be mutually exclusive. The assert below verifies
    //  if the value of TxNotification is a power of 2. If it is not
    //  then we will break.
    //

    ASSERT( !(( TxNotification ) & ( TxNotification - 1 )) );

    while (TxNotification) {

        count++;

        TxNotification >>= 1;

        //
        //  If we hit this assert then we have more notification codes than
        //  can fit in a UCHAR. We need to revaluate our approach for
        //  storing minor codes now.
        //

        ASSERT( count != 0 );
    }

    return ( count );
}


//---------------------------------------------------------------------------
//                    Log Record allocation routines
//---------------------------------------------------------------------------

PRECORD_LIST
SpyAllocateBuffer (
    __out PULONG RecordType
    )
/*++

Routine Description:

    Allocates a new buffer from the MiniSpyData.FreeBufferList if there is
    enough memory to do so and we have not exceed our maximum buffer
    count.

    NOTE:  Because there is no interlock between testing if we have exceeded
           the record allocation limit and actually increment the in use
           count it is possible to temporarily allocate one or two buffers
           more then the limit.  Because this is such a rare situation there
           is not point to handling this.

    NOTE:  This code must be NON-PAGED because it can be called on the
           paging path or at DPC level.

Arguments:

    RecordType - Receives information on what type of record was allocated.

Return Value:

    Pointer to the allocated buffer, or NULL if the allocation failed.

--*/
{
    PVOID newBuffer;
    ULONG newRecordType = RECORD_TYPE_NORMAL;

    //
    //  See if we have room to allocate more buffers
    //

    if (MiniSpyData.RecordsAllocated < MiniSpyData.MaxRecordsToAllocate) {

        InterlockedIncrement( &MiniSpyData.RecordsAllocated );

        newBuffer = ExAllocateFromNPagedLookasideList( &MiniSpyData.FreeBufferList );

        if (newBuffer == NULL) {

            //
            //  We failed to allocate the memory.  Decrement our global count
            //  and return what type of memory we have.
            //

            InterlockedDecrement( &MiniSpyData.RecordsAllocated );

            newRecordType = RECORD_TYPE_FLAG_OUT_OF_MEMORY;
        }

    } else {

        //
        //  No more room to allocate memory, return we didn't get a buffer
        //  and why.
        //

        newRecordType = RECORD_TYPE_FLAG_EXCEED_MEMORY_ALLOWANCE;
        newBuffer = NULL;
    }

    *RecordType = newRecordType;
    return newBuffer;
}


VOID
SpyFreeBuffer (
    __in PVOID Buffer
    )
/*++

Routine Description:

    Free an allocate buffer.

    NOTE:  This code must be NON-PAGED because it can be called on the
           paging path or at DPC level.

Arguments:

    Buffer - The buffer to free.

Return Value:

    None.

--*/
{
    //
    //  Free the memory, update the counter
    //

    InterlockedDecrement( &MiniSpyData.RecordsAllocated );
    ExFreeToNPagedLookasideList( &MiniSpyData.FreeBufferList, Buffer );
}


//---------------------------------------------------------------------------
//                    Logging routines
//---------------------------------------------------------------------------

PRECORD_LIST
SpyNewRecord (
    VOID
    )
/*++

Routine Description:

    Allocates a new RECORD_LIST structure if there is enough memory to do so. A
    sequence number is updated for each request for a new record.

    NOTE:  This code must be NON-PAGED because it can be called on the
           paging path or at DPC level.

Arguments:

    None

Return Value:

    Pointer to the RECORD_LIST allocated, or NULL if no memory is available.

--*/
{
    PRECORD_LIST newRecord;
    ULONG initialRecordType;

    //
    //  Allocate the buffer
    //

    newRecord = SpyAllocateBuffer( &initialRecordType );

    if (newRecord == NULL) {

        //
        //  We could not allocate a record, see if the static buffer is
        //  in use.  If not, we will use it
        //

        if (!InterlockedExchange( &MiniSpyData.StaticBufferInUse, TRUE )) {

            newRecord = (PRECORD_LIST)MiniSpyData.OutOfMemoryBuffer;
            initialRecordType |= RECORD_TYPE_FLAG_STATIC;
        }
    }

    //
    //  If we got a record (doesn't matter if it is static or not), init it
    //

    if (newRecord != NULL) {

        //
        // Init the new record
        //

        newRecord->LogRecord.RecordType = initialRecordType;
        newRecord->LogRecord.Length = sizeof(LOG_RECORD);
        newRecord->LogRecord.SequenceNumber = InterlockedIncrement( &MiniSpyData.LogSequenceNumber );
        RtlZeroMemory( &newRecord->LogRecord.Data, sizeof( RECORD_DATA ) );
    }

    return( newRecord );
}


VOID
SpyFreeRecord (
    __in PRECORD_LIST Record
    )
/*++

Routine Description:

    Free the given buffer

    NOTE:  This code must be NON-PAGED because it can be called on the
           paging path or at DPC level.

Arguments:

    Record - the buffer to free

Return Value:

    None.

--*/
{
    if (FlagOn(Record->LogRecord.RecordType,RECORD_TYPE_FLAG_STATIC)) {

        //
        // This was our static buffer, mark it available.
        //

        ASSERT(MiniSpyData.StaticBufferInUse);
        MiniSpyData.StaticBufferInUse = FALSE;

    } else {

        SpyFreeBuffer( Record );
    }
}


VOID
SpySetRecordName (
    __inout PLOG_RECORD LogRecord,
    __in PUNICODE_STRING Name
    )
/*++

Routine Description:

    Sets the given file name in the LogRecord.

    NOTE:  This code must be NON-PAGED because it can be called on the
           paging path.

Arguments:

    LogRecord - The record in which to set the name.

    Name - The name to insert

Return Value:

    None.

--*/
{
    ULONG nameCopyLength;
    PCHAR copyPointer = (PCHAR)LogRecord->Name;

    if (NULL != Name) {

        //
        //  Need space for NULL termination
        //

        if (Name->Length > (MAX_NAME_SPACE - sizeof( UNICODE_NULL ))) {

            nameCopyLength = MAX_NAME_SPACE - sizeof( UNICODE_NULL );

        } else {

            nameCopyLength = (ULONG)Name->Length;
        }

        //
        //  We will always round up log-record length to sizeof(PVOID) so that
        //  the next log record starts on the right PVOID boundary to prevent
        //  IA64 alignment faults.  The length of the record of course
        //  includes the additional NULL at the end.
        //

        LogRecord->Length = ROUND_TO_SIZE( (LogRecord->Length +
                                            nameCopyLength +
                                            sizeof( UNICODE_NULL )),
                                           sizeof( PVOID ) );

        RtlCopyMemory( copyPointer, Name->Buffer, nameCopyLength );

        //
        //  Null terminate the name
        //

        copyPointer += nameCopyLength;

        *((PWCHAR) copyPointer) = UNICODE_NULL;

        ASSERT(LogRecord->Length <= MAX_LOG_RECORD_LENGTH);
    }
}


VOID
SpyLogPreOperationData (
    __in PFLT_CALLBACK_DATA Data,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __inout PRECORD_LIST RecordList
    )
/*++

Routine Description:

    This is called from the pre-operation callback routine to copy the
    necessary information into the log record.

    NOTE:  This code must be NON-PAGED because it can be called on the
           paging path.

Arguments:

    Data - The Data structure that contains the information we want to record.

    FltObjects - Pointer to the io objects involved in this operation.

    RecordList - Where we want to save the data

Return Value:

    None.

--*/
{
    PRECORD_DATA recordData = &RecordList->LogRecord.Data;
    PDEVICE_OBJECT devObj;
    NTSTATUS status;

    status = FltGetDeviceObject(FltObjects->Volume,&devObj);
    if (NT_SUCCESS(status)) {

        ObDereferenceObject(devObj);

    } else {

        devObj = NULL;
    }

    //
    //  Save the information we want
    //

    recordData->CallbackMajorId = Data->Iopb->MajorFunction;
    recordData->CallbackMinorId = Data->Iopb->MinorFunction;
    recordData->IrpFlags        = Data->Iopb->IrpFlags;
    recordData->Flags           = Data->Flags;

    recordData->DeviceObject    = (FILE_ID)devObj;
    recordData->FileObject      = (FILE_ID)FltObjects->FileObject;
    recordData->Transaction     = (FILE_ID)FltObjects->Transaction;
    recordData->ProcessId       = (FILE_ID)PsGetCurrentProcessId();
    recordData->ThreadId        = (FILE_ID)PsGetCurrentThreadId();

    recordData->Arg1 = Data->Iopb->Parameters.Others.Argument1;
    recordData->Arg2 = Data->Iopb->Parameters.Others.Argument2;
    recordData->Arg3 = Data->Iopb->Parameters.Others.Argument3;
    recordData->Arg4 = Data->Iopb->Parameters.Others.Argument4;
    recordData->Arg5 = Data->Iopb->Parameters.Others.Argument5;
    recordData->Arg6.QuadPart = Data->Iopb->Parameters.Others.Argument6.QuadPart;

    KeQuerySystemTime( &recordData->OriginatingTime );
}


VOID
SpyLogPostOperationData (
    __in PFLT_CALLBACK_DATA Data,
    __inout PRECORD_LIST RecordList
    )
/*++

Routine Description:

    This is called from the post-operation callback routine to copy the
    necessary information into the log record.

    NOTE:  This code must be NON-PAGED because it can be called on the
           paging path or at DPC level.

Arguments:

    Data - The Data structure that contains the information we want to record.

    RecordList - Where we want to save the data

Return Value:

    None.

--*/
{
    PRECORD_DATA recordData = &RecordList->LogRecord.Data;

    recordData->Status = Data->IoStatus.Status;
    recordData->Information = Data->IoStatus.Information;
    KeQuerySystemTime( &recordData->CompletionTime );
}


VOID
SpyLogTransactionNotify (
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __inout PRECORD_LIST RecordList,
    __in ULONG TransactionNotification
    )
/*++

Routine Description:

    This routine logs the transaction notification.

Arguments:

    FltObjects - Pointer to the io objects involved in this operation.

    RecordList - Where we want to save the data

    TransactionNotification - Notification for this transaction.

Return Value:

    None.

--*/
{
    PRECORD_DATA recordData = &RecordList->LogRecord.Data;
    PDEVICE_OBJECT devObj;
    NTSTATUS status;

    status = FltGetDeviceObject(FltObjects->Volume,&devObj);
    if (NT_SUCCESS(status)) {

        ObDereferenceObject(devObj);

    } else {

        devObj = NULL;
    }


    recordData->CallbackMajorId = IRP_MJ_TRANSACTION_NOTIFY;
    recordData->CallbackMinorId = TxNotificationToMinorCode(TransactionNotification);

    recordData->DeviceObject    = (FILE_ID)devObj;
    recordData->FileObject      = (FILE_ID)FltObjects->FileObject;
    recordData->Transaction     = (FILE_ID)FltObjects->Transaction;
    recordData->ProcessId       = (FILE_ID)PsGetCurrentProcessId();
    recordData->ThreadId        = (FILE_ID)PsGetCurrentThreadId();

    KeQuerySystemTime( &recordData->OriginatingTime );
}


VOID
SpyLog (
    __in PRECORD_LIST RecordList
    )
/*++

Routine Description:

    This routine inserts the given log record into the list to be sent
    to the user mode application.

    NOTE:  This code must be NON-PAGED because it can be called on the
           paging path or at DPC level and uses a spin-lock

Arguments:

    RecordList - The record to append to the MiniSpyData.OutputBufferList

Return Value:

    The function returns STATUS_SUCCESS.



--*/
{
    KIRQL oldIrql;

    KeAcquireSpinLock(&MiniSpyData.OutputBufferLock, &oldIrql);
    InsertTailList(&MiniSpyData.OutputBufferList, &RecordList->List);
    KeReleaseSpinLock(&MiniSpyData.OutputBufferLock, oldIrql);
}


NTSTATUS
SpyGetLog (
    __out_bcount_part(OutputBufferLength,*ReturnOutputBufferLength) PUCHAR OutputBuffer,
    __in ULONG OutputBufferLength,
    __out PULONG ReturnOutputBufferLength
    )
/*++

Routine Description:
    This function fills OutputBuffer with as many LOG_RECORDs as possible.
    The LOG_RECORDs are variable sizes and are tightly packed in the
    OutputBuffer.

    NOTE:  This code must be NON-PAGED because it uses a spin-lock.

Arguments:
    OutputBuffer - The user's buffer to fill with the log data we have
        collected

    OutputBufferLength - The size in bytes of OutputBuffer

    ReturnOutputBufferLength - The amount of data actually written into the
        OutputBuffer.

Return Value:
    STATUS_SUCCESS if some records were able to be written to the OutputBuffer.

    STATUS_NO_MORE_ENTRIES if we have no data to return.

    STATUS_BUFFER_TOO_SMALL if the OutputBuffer is too small to
        hold even one record and we have data to return.

--*/
{
    PLIST_ENTRY pList;
    ULONG bytesWritten = 0;
    PLOG_RECORD pLogRecord;
    NTSTATUS status = STATUS_NO_MORE_ENTRIES;
    PRECORD_LIST pRecordList;
    KIRQL oldIrql;
    BOOLEAN recordsAvailable = FALSE;

    KeAcquireSpinLock( &MiniSpyData.OutputBufferLock, &oldIrql );

    while (!IsListEmpty( &MiniSpyData.OutputBufferList ) && (OutputBufferLength > 0)) {

        //
        //  Mark we have records
        //

        recordsAvailable = TRUE;

        //
        //  Get the next available record
        //

        pList = RemoveHeadList( &MiniSpyData.OutputBufferList );

        pRecordList = CONTAINING_RECORD( pList, RECORD_LIST, List );

        pLogRecord = &pRecordList->LogRecord;

        //
        //  If no filename was set then make it into a NULL file name.
        //

        if (REMAINING_NAME_SPACE( pLogRecord ) == MAX_NAME_SPACE) {

            //
            //  We don't have a name, so return an empty string.
            //  We have to always start a new log record on a PVOID aligned boundary.
            //

            pLogRecord->Length += ROUND_TO_SIZE( sizeof( UNICODE_NULL ), sizeof( PVOID ) );
            pLogRecord->Name[0] = UNICODE_NULL;
        }

        //
        //  Put it back if we've run out of room.
        //

        if (OutputBufferLength < pLogRecord->Length) {

            InsertHeadList( &MiniSpyData.OutputBufferList, pList );
            break;
        }

        KeReleaseSpinLock( &MiniSpyData.OutputBufferLock, oldIrql );

        //
        //  The lock is released, return the data, adjust pointers.
        //  Protect access to raw user-mode OutputBuffer with an exception handler
        //

        try {
            RtlCopyMemory( OutputBuffer, pLogRecord, pLogRecord->Length );
        } except( EXCEPTION_EXECUTE_HANDLER ) {

            //
            //  Put the record back in
            //

            KeAcquireSpinLock( &MiniSpyData.OutputBufferLock, &oldIrql );
            InsertHeadList( &MiniSpyData.OutputBufferList, pList );
            KeReleaseSpinLock( &MiniSpyData.OutputBufferLock, oldIrql );

            return GetExceptionCode();

        }

        bytesWritten += pLogRecord->Length;

        OutputBufferLength -= pLogRecord->Length;

        OutputBuffer += pLogRecord->Length;

        SpyFreeRecord( pRecordList );

        //
        //  Relock the list
        //

        KeAcquireSpinLock( &MiniSpyData.OutputBufferLock, &oldIrql );
    }

    KeReleaseSpinLock( &MiniSpyData.OutputBufferLock, oldIrql );

    //
    //  Set proper status
    //

    if ((bytesWritten == 0) && recordsAvailable) {

        //
        //  There were records to be sent up but
        //  there was not enough room in the buffer.
        //

        status = STATUS_BUFFER_TOO_SMALL;

    } else if (bytesWritten > 0) {

        //
        //  We were able to write some data to the output buffer,
        //  so this was a success.
        //

        status = STATUS_SUCCESS;
    }

    *ReturnOutputBufferLength = bytesWritten;

    return status;
}


VOID
SpyEmptyOutputBufferList (
    VOID
    )
/*++

Routine Description:

    This routine frees all the remaining log records in the OutputBufferList
    that are not going to get sent up to the user mode application since
    MiniSpy is shutting down.

    NOTE:  This code must be NON-PAGED because it uses a spin-lock

Arguments:

    None.

Return Value:

    None.

--*/
{
    PLIST_ENTRY pList;
    PRECORD_LIST pRecordList;
    KIRQL oldIrql;

    KeAcquireSpinLock( &MiniSpyData.OutputBufferLock, &oldIrql );

    while (!IsListEmpty( &MiniSpyData.OutputBufferList )) {

        pList = RemoveHeadList( &MiniSpyData.OutputBufferList );
        KeReleaseSpinLock( &MiniSpyData.OutputBufferLock, oldIrql );

        pRecordList = CONTAINING_RECORD( pList, RECORD_LIST, List );

        SpyFreeRecord( pRecordList );

        KeAcquireSpinLock( &MiniSpyData.OutputBufferLock, &oldIrql );
    }

    KeReleaseSpinLock( &MiniSpyData.OutputBufferLock, oldIrql );
}

//---------------------------------------------------------------------------
//                    Logging routines
//---------------------------------------------------------------------------

VOID
SpyReadDriverParameters (
    __in PUNICODE_STRING RegistryPath
    )
/*++

Routine Description:

    This routine tries to read the MiniSpy-specific parameters from
    the registry.  These values will be found in the registry location
    indicated by the RegistryPath passed in.

    This processes the following registry keys:
    hklm\system\CurrentControlSet\Services\Minispy\MaxRecords
    hklm\system\CurrentControlSet\Services\Minispy\NameQueryMethod


Arguments:

    RegistryPath - the path key which contains the values that are
        the MiniSpy parameters

Return Value:

    None.

--*/
{
    OBJECT_ATTRIBUTES attributes;
    HANDLE driverRegKey;
    NTSTATUS status;
    ULONG resultLength;
    UNICODE_STRING valueName;
    PKEY_VALUE_PARTIAL_INFORMATION pValuePartialInfo;
    UCHAR buffer[sizeof( KEY_VALUE_PARTIAL_INFORMATION ) + sizeof( LONG )];

    //
    //  Open the registry
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

        return;
    }

    //
    // Read the MaxRecordsToAllocate entry from the registry
    //

    RtlInitUnicodeString( &valueName, MAX_RECORDS_TO_ALLOCATE );

    status = ZwQueryValueKey( driverRegKey,
                              &valueName,
                              KeyValuePartialInformation,
                              buffer,
                              sizeof(buffer),
                              &resultLength );

    if (NT_SUCCESS( status )) {

        pValuePartialInfo = (PKEY_VALUE_PARTIAL_INFORMATION) buffer;
        ASSERT( pValuePartialInfo->Type == REG_DWORD );
        MiniSpyData.MaxRecordsToAllocate = *((PLONG)&(pValuePartialInfo->Data));
    }

    //
    // Read the NameQueryMethod entry from the registry
    //

    RtlInitUnicodeString( &valueName, NAME_QUERY_METHOD );

    status = ZwQueryValueKey( driverRegKey,
                              &valueName,
                              KeyValuePartialInformation,
                              buffer,
                              sizeof(buffer),
                              &resultLength );

    if (NT_SUCCESS( status )) {

        pValuePartialInfo = (PKEY_VALUE_PARTIAL_INFORMATION) buffer;
        ASSERT( pValuePartialInfo->Type == REG_DWORD );
        MiniSpyData.NameQueryMethod = *((PLONG)&(pValuePartialInfo->Data));
    }

    ZwClose(driverRegKey);
}

