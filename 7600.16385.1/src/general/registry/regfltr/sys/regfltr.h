/*++
Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    regfltr.h

Abstract: 

    Header file for the sample driver

Environment:

    Kernel mode only


--*/

#pragma once

#include <ntifs.h>
#include <ntstrsafe.h>

#include "common.h"


//
// Pool tags
//

#define REGFLTR_CONTEXT_POOL_TAG          '0tfR'
#define REGFLTR_CAPTURE_POOL_TAG          '1tfR'


//
// Logging macros
//

#define InfoPrint(str, ...)                 \
    DbgPrintEx(DPFLTR_IHVDRIVER_ID,         \
               DPFLTR_INFO_LEVEL,           \
               "%S: "##str"\n",             \
               DRIVER_NAME,                 \
               __VA_ARGS__)

#define ErrorPrint(str, ...)                \
    DbgPrintEx(DPFLTR_IHVDRIVER_ID,         \
               DPFLTR_ERROR_LEVEL,          \
               "%S: %u: "##str"\n",         \
               DRIVER_NAME,                 \
               __LINE__,                    \
               __VA_ARGS__)


//
// The root key used in the samples
//
extern HANDLE g_RootKey;

//
// Pointer to the device object used to register registry callbacks
//
extern PDEVICE_OBJECT g_DeviceObj;

//
// Registry callback version
//
extern ULONG g_MajorVersion;
extern ULONG g_MinorVersion;

//
// Set to TRUE if TM and RM were successfully created and the transaction
// callback was successfully enabled. 
//
extern BOOLEAN g_RMCreated;


//
// Context data structure for the transaction callback RMCallback
//

typedef struct _RMCALLBACK_CONTEXT {

    //
    // A bit mask of all transaction notifications types that the RM Callback is 
    // notified of.
    //
    ULONG Notification;

    //
    // The handle to an enlistment
    //
    HANDLE Enlistment;
    
} RMCALLBACK_CONTEXT, *PRMCALLBACK_CONTEXT;


//
// The context data structure for the registry callback. It will be passed 
// to the callback function every time it is called. 
//

typedef struct _CALLBACK_CONTEXT {

    //
    // Specifies which callback helper method to use
    //
    CALLBACK_MODE CallbackMode;

    //
    // Records the current ProcessId to filter out registry operation from
    // other processes.
    //
    HANDLE ProcessId;

    //
    // Records the altitude that the callback was registered at
    //
    UNICODE_STRING Altitude; 
    WCHAR AltitudeBuffer[MAX_ALTITUDE_BUFFER_LENGTH];
        
    //
    // Records the cookie returned by the registry when the callback was 
    // registered
    //
    LARGE_INTEGER Cookie;

    //
    // A pointer to the context for the transaction callback. 
    // Used to enlist on a transaction. Only used in the transaction samples.
    //
    PRMCALLBACK_CONTEXT RMCallbackCtx;

    //
    // These fields record information for verifying the behavior of the
    // certain samples. They are not used in all samples
    //
    
    //
    // Number of times the RegNtCallbackObjectContextCleanup 
    // notification was received
    //
    LONG ContextCleanupCount;

    //
    // Number of times the callback saw a notification with the call or
    // object context set correctly.
    //
    LONG NotificationWithContextCount;

    //
    // Number of times callback saw a notirication without call or without
    // object context set correctly
    //
    LONG NotificationWithNoContextCount;

    //
    // Number of pre-notifications received
    //
    LONG PreNotificationCount;

    //
    // Number of post-notifications received
    //
    LONG PostNotificationCount;
    
} CALLBACK_CONTEXT, *PCALLBACK_CONTEXT;


//
// The registry and transaction callback routines
//

EX_CALLBACK_FUNCTION Callback;

NTSTATUS  
RMCallback(
    __in PKENLISTMENT EnlistmentObject,
    __in PVOID RMContext,    
    __in PVOID TransactionContext,    
    __in ULONG TransactionNotification,    
    __inout PLARGE_INTEGER TMVirtualClock,
    __in ULONG ArgumentLength,
    __in PVOID Argument
    );

//
// The samples and their corresponding callback helper methods
//

NTSTATUS 
CallbackPreNotificationBlock(
    __in PCALLBACK_CONTEXT CallbackCtx,
    __in REG_NOTIFY_CLASS NotifyClass,
    __inout PVOID Argument2
    );

BOOLEAN 
PreNotificationBlockSample();

NTSTATUS 
CallbackPreNotificationBlock(
    __in PCALLBACK_CONTEXT CallbackCtx,
    __in REG_NOTIFY_CLASS NotifyClass,
    __inout PVOID Argument2
    );

BOOLEAN 
PreNotificationBypassSample();

NTSTATUS 
CallbackPreNotificationBypass(
    __in PCALLBACK_CONTEXT CallbackCtx,
    __in REG_NOTIFY_CLASS NotifyClass,
    __inout PVOID Argument2
    );

BOOLEAN 
PostNotificationOverrideSuccessSample();

NTSTATUS 
CallbackPostNotificationOverrideSuccess(
    __in PCALLBACK_CONTEXT CallbackCtx,
    __in REG_NOTIFY_CLASS NotifyClass,
    __inout PVOID Argument2
    );

BOOLEAN 
PostNotificationOverrideErrorSample();

NTSTATUS 
CallbackPostNotificationOverrideError(
    __in PCALLBACK_CONTEXT CallbackCtx,
    __in REG_NOTIFY_CLASS NotifyClass,
    __inout PVOID Argument2
    );

BOOLEAN
TransactionEnlistSample();

NTSTATUS
CallbackTransactionEnlist(
    __in PCALLBACK_CONTEXT CallbackCtx,
    __in REG_NOTIFY_CLASS NotifyClass,
    __inout PVOID Argument2
    );

BOOLEAN
TransactionReplaySample();

NTSTATUS
CallbackTransactionReplay(
    __in PCALLBACK_CONTEXT CallbackCtx,
    __in REG_NOTIFY_CLASS NotifyClass,
    __inout PVOID Argument2
    );

BOOLEAN
SetObjectContextSample();

NTSTATUS
CallbackSetObjectContext(
    __in PCALLBACK_CONTEXT CallbackCtx,
    __in REG_NOTIFY_CLASS NotifyClass,
    __inout PVOID Argument2
    );

BOOLEAN
SetCallContextSample();

NTSTATUS
CallbackSetCallContext(
    __in PCALLBACK_CONTEXT CallbackCtx,
    __in REG_NOTIFY_CLASS NotifyClass,
    __inout PVOID Argument2
    );

BOOLEAN
MultipleAltitudeBlockDuringPreSample();

BOOLEAN
MultipleAltitudeInternalInvocationSample();

NTSTATUS
CallbackMonitor(
    __in PCALLBACK_CONTEXT CallbackCtx,
    __in REG_NOTIFY_CLASS NotifyClass,
    __inout PVOID Argument2
    );

NTSTATUS
CallbackMultipleAltitude(
    __in PCALLBACK_CONTEXT CallbackCtx,
    __in REG_NOTIFY_CLASS NotifyClass,
    __inout PVOID Argument2
    );

NTSTATUS
CallbackCapture(
    __in PCALLBACK_CONTEXT CallbackCtx,
    __in REG_NOTIFY_CLASS NotifyClass,
    __inout PVOID Argument2
    );

VOID
BugCheckSample();

NTSTATUS
CallbackBugcheck(
    __in PCALLBACK_CONTEXT CallbackCtx,
    __in REG_NOTIFY_CLASS NotifyClass,
    __inout PVOID Argument2
    );

BOOLEAN 
CreateOpenV1Sample();

NTSTATUS
CallbackCreateOpenV1(
    __in PCALLBACK_CONTEXT CallbackCtx,
    __in REG_NOTIFY_CLASS NotifyClass,
    __inout PVOID Argument2
    );

//
// Driver dispatch functions
//

NTSTATUS
DoCallbackSamples(
    __in PDEVICE_OBJECT DeviceObject,
    __in PIRP Irp
    );

NTSTATUS
RegisterCallback(
    __in PDEVICE_OBJECT DeviceObject,
    __in PIRP Irp
    );

NTSTATUS
UnRegisterCallback(
    __in PDEVICE_OBJECT DeviceObject,
    __in PIRP Irp
    );

NTSTATUS
GetCallbackVersion(
    __in PDEVICE_OBJECT DeviceObject,
    __in PIRP Irp
    );

//
// Transaction related routines
//

NTSTATUS
CreateKTMResourceManager(
    __in PTM_RM_NOTIFICATION CallbackRoutine,
    __in_opt PVOID RMKey
    );

NTSTATUS
EnlistInTransaction(
    __out PHANDLE EnlistmentHandle,
    __in ACCESS_MASK DesiredAccess,
    __in PVOID Transaction,
    __in NOTIFICATION_MASK NotificationMask,
    __in_opt PVOID EnlistmentKey
    );

VOID
DeleteKTMResourceManager(
    );


//
// Capture methods
//

NTSTATUS
CaptureBuffer(
    __deref_out_opt PVOID *CapturedBuffer,
    __in_ecount(Length)PVOID Buffer, 
    __in SIZE_T Length, 
    __in ULONG PoolTag
    );

VOID
FreeCapturedBuffer(
    __in PVOID Buffer, 
    __in ULONG PoolTag
    );

NTSTATUS
CaptureUnicodeString(
    __inout UNICODE_STRING * DestString, 
    __in PCUNICODE_STRING SourceString, 
    __in ULONG PoolTag
    );

VOID
FreeCapturedUnicodeString(
    __in UNICODE_STRING * String, 
    __in ULONG PoolTag
    );


//
// Utility methods
//

PVOID
CreateCallbackContext(
    __in CALLBACK_MODE CallbackMode, 
    __in PCWSTR AltitudeString
    );

ULONG 
ExceptionFilter (
    __in PEXCEPTION_POINTERS ExceptionPointers
    );
    


