/*++

Copyright (c) 1990-2000  Microsoft Corporation

Module Name:

    forward_progress.h

Abstract:

    This is the driver object header for forward progress


Environment:

    kernel mode only

Revision History:

--*/

#pragma once

//
// NOTE: Forward progress was added in v1.9 
//

#define MAX_RESERVED_REQUESTS 8
#define MEMORY_SIZE 0x100

typedef struct _FWD_PROGRESS_REQUEST_CONTEXT {
  WDFMEMORY Memory;
} FWD_PROGRESS_REQUEST_CONTEXT, *PFWD_PROGRESS_REQUEST_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(FWD_PROGRESS_REQUEST_CONTEXT, GetForwardProgressRequestContext)   


NTSTATUS 
SetForwardProgressOnQueue(
    __in WDFQUEUE Queue
    );


