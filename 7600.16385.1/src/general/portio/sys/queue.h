/*++

Copyright (c) 1990-2000  Microsoft Corporation

Module Name:

    queue.h

Abstract:

    This is a C version of a very simple sample driver that illustrates
    how to use the driver framework and demonstrates best practices.

--*/

#include "gpioctl.h"        // Get IOCTL interface definitions


NTSTATUS
PortIOQueueInitialize(
    __in WDFDEVICE hDevice
    );

VOID
PortIOIoctlWritePort(
    __in PDEVICE_CONTEXT devContext,
    __in WDFREQUEST Request,
    __in size_t OutBufferSize,
    __in size_t InBufferSize,
    __in ULONG IoctlCode);    

VOID 
PortIOIoctlReadPort(
    __in PDEVICE_CONTEXT devContext,
    __in WDFREQUEST Request,
    __in size_t OutBufferSize,
    __in size_t InBufferSize,
    __in ULONG IoctlCode);

//
// Events from the IoQueue object
//
EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL PortIOEvtIoDeviceControl;
EVT_WDF_DEVICE_FILE_CREATE PortIOEvtFileCreate;
EVT_WDF_FILE_CLOSE PortIOEvtFileClose;

