/*++
Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    util.c

Abstract: 

    Utility routines for the sample driver.

Environment:

    Kernel mode only

--*/


#include "regfltr.h"



ULONG 
ExceptionFilter (
    __in PEXCEPTION_POINTERS ExceptionPointers
    )
/*++

Routine Description:

    ExceptionFilter breaks into the debugger if an exception happens
    inside the callback.

Arguments:

    ExceptionPointers - unused

Return Value:

    Always returns EXCEPTION_CONTINUE_SEARCH

--*/
{

    ErrorPrint("Exception %lx, ExceptionPointers = %p", 
               ExceptionPointers->ExceptionRecord->ExceptionCode,
               ExceptionPointers);

    DbgBreakPoint();
    
    return EXCEPTION_EXECUTE_HANDLER;

}


PVOID
CreateCallbackContext(
    __in CALLBACK_MODE CallbackMode,
    __in PCWSTR AltitudeString
    ) 
/*++

Routine Description:

    Utility method to create a callback context. Callback context 
    should be freed using ExFreePoolWithTag.
    
Arguments:

    CallbackMode - the callback mode value

    AltitudeString - a string with the altitude the callback will be 
        registered at

Return Value:

    Pointer to the allocated and initialized callback context 

--*/
{

    PCALLBACK_CONTEXT CallbackCtx = NULL;
    NTSTATUS Status;
    BOOLEAN Success = FALSE;
    
    CallbackCtx = (PCALLBACK_CONTEXT) ExAllocatePoolWithTag (
                        PagedPool, 
                        sizeof(CALLBACK_CONTEXT), 
                        REGFLTR_CONTEXT_POOL_TAG);

    if  (CallbackCtx == NULL) {
        ErrorPrint("CreateCallbackContext failed due to insufficient resources.");
        goto Exit;
    }
    
    RtlZeroMemory(CallbackCtx, sizeof(CALLBACK_CONTEXT));

    CallbackCtx->CallbackMode = CallbackMode;
    CallbackCtx->ProcessId = PsGetCurrentProcessId();

    Status = RtlStringCbPrintfW(CallbackCtx->AltitudeBuffer,
                                 MAX_ALTITUDE_BUFFER_LENGTH * sizeof(WCHAR),
                                 L"%s",
                                 AltitudeString);
    
    if (!NT_SUCCESS(Status)) {
        ErrorPrint("RtlStringCbPrintfW in CreateCallbackContext failed. Status 0x%x", Status);
        goto Exit;
    }
    
    RtlInitUnicodeString (&CallbackCtx->Altitude, CallbackCtx->AltitudeBuffer);

    Success = TRUE;

  Exit:

    if (Success == FALSE) {
        if (CallbackCtx != NULL) {
            ExFreePoolWithTag(CallbackCtx, REGFLTR_CONTEXT_POOL_TAG);
            CallbackCtx = NULL;
        }
    }

    return CallbackCtx;
    
}



