/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    fail_driver6.h

Environment:

    Kernel mode

--*/

#ifdef __cplusplus
extern "C" {
#endif
#include <NTDDK.h>  
#include <wdf.h>
#ifdef __cplusplus
}
#endif

#include "fail_library6.h"

//
// This is the context that can be placed per queue
// and would contain per queue information.
//
typedef struct _QUEUE_CONTEXT {
    WDFREQUEST  CurrentRequest;

} QUEUE_CONTEXT, *PQUEUE_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(QUEUE_CONTEXT, QueueGetContext)

#ifdef __cplusplus
extern "C"
#endif
DRIVER_INITIALIZE DriverEntry; 

EVT_WDF_DRIVER_DEVICE_ADD EvtDriverDeviceAdd;

EVT_WDF_IO_QUEUE_IO_READ EvtIoRead;

EVT_WDF_IO_QUEUE_IO_WRITE EvtIoWrite;

EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL EvtIoDeviceControl;

EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL EvtIoIntDeviceControl;

EVT_WDF_DEVICE_CONTEXT_CLEANUP DeviceContextCleanUp;

EVT_WDF_DEVICE_CONTEXT_DESTROY DeviceContextDestroy;

EVT_WDF_IO_QUEUE_CONTEXT_CLEANUP_CALLBACK QueueCleanup;

EVT_WDF_IO_QUEUE_CONTEXT_DESTROY_CALLBACK QueueDestroy;

EVT_WDF_FILE_CONTEXT_CLEANUP_CALLBACK FileContextCleanup;

EVT_WDF_FILE_CONTEXT_DESTROY_CALLBACK FileContextDestroy;

EVT_WDF_TIMER EvtTimerFunc;
