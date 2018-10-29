/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    cancel.h

Abstract:

Environment:

    Kernel mode only.


Revision History:

--*/

#include <initguid.h>

//
// Since this driver is a legacy driver and gets installed as a service
// (without an INF file),  we will define a class guid for use in
// IoCreateDeviceSecure function. This would allow  the system to store
// Security, DeviceType, Characteristics and Exclusivity information of the
// deviceobject in the registery under
// HKLM\SYSTEM\CurrentControlSet\Control\Class\ClassGUID\Properties.
// This information can be overrided by an Administrators giving them the ability
// to control access to the device beyond what is initially allowed
// by the driver developer.
//

// {5D006E1A-2631-466c-B8A0-32FD498E4424}  - generated using guidgen.exe
DEFINE_GUID (GUID_DEVCLASS_CANCEL_SAMPLE,
        0x5d006e1a, 0x2631, 0x466c, 0xb8, 0xa0, 0x32, 0xfd, 0x49, 0x8e, 0x44, 0x24);

//
// GUID definition are required to be outside of header inclusion pragma to avoid
// error during precompiled headers.
//

#ifndef __CANCEL_H
#define __CANCEL_H

//
// GUID definition are required to be outside of header inclusion pragma to
// avoid error during precompiled headers.
//
#include <ntddk.h>
//
// Include this header file and link with csq.lib to use this
// sample on Win2K, XP and above.
//
#include <csq.h>
#include <wdmsec.h> // for IoCreateDeviceSecure
#include <dontuse.h>

//  Debugging macros

#if DBG
#define CSAMP_KDPRINT(_x_) \
                DbgPrint("CANCEL.SYS: ");\
                DbgPrint _x_;
#else

#define CSAMP_KDPRINT(_x_)

#endif

#define CSAMP_DEVICE_NAME_U     L"\\Device\\CANCELSAMP"
#define CSAMP_DOS_DEVICE_NAME_U L"\\DosDevices\\CancelSamp"
#define CSAMP_RETRY_INTERVAL    500*1000 //500 ms
#define TAG (ULONG)'MASC'

typedef struct _INPUT_DATA{

    ULONG Data; //device data is stored here

} INPUT_DATA, *PINPUT_DATA;

typedef struct _DEVICE_EXTENSION{

    BOOLEAN ThreadShouldStop;

    // Irps waiting to be processed are queued here
    LIST_ENTRY   PendingIrpQueue;

    //  SpinLock to protect access to the queue
    KSPIN_LOCK QueueLock;

    IO_CSQ CancelSafeQueue;

    // Time at which the device was last polled
    LARGE_INTEGER LastPollTime;

    // Polling interval (retry interval)
    LARGE_INTEGER PollingInterval;

    KSEMAPHORE IrpQueueSemaphore;

    PETHREAD ThreadObject;
}  DEVICE_EXTENSION, *PDEVICE_EXTENSION;

typedef struct _FILE_CONTEXT{
    //
    // Lock to rundown threads that are dispatching I/Os on a file handle 
    // while the cleanup for that handle is in progress.
    //
    IO_REMOVE_LOCK  FileRundownLock;
} FILE_CONTEXT, *PFILE_CONTEXT;

DRIVER_INITIALIZE DriverEntry;

__drv_dispatchType(IRP_MJ_CREATE)
__drv_dispatchType(IRP_MJ_CLOSE)
DRIVER_DISPATCH CsampCreateClose;

__drv_dispatchType(IRP_MJ_CLEANUP)
DRIVER_DISPATCH CsampCleanup;

__drv_dispatchType(IRP_MJ_READ)
DRIVER_DISPATCH CsampRead;

DRIVER_DISPATCH CsampPollDevice;

DRIVER_UNLOAD CsampUnload;

KSTART_ROUTINE CsampPollingThread;

VOID
CsampPollingThread(
    __in PVOID Context
    );

VOID
CsampInsertIrp (
    __in PIO_CSQ   Csq,
    __in PIRP              Irp
    );

VOID
CsampRemoveIrp(
    __in  PIO_CSQ Csq,
    __in  PIRP    Irp
    );

PIRP
CsampPeekNextIrp(
    __in  PIO_CSQ Csq,
    __in  PIRP    Irp,
    __in  PVOID  PeekContext
    );

__drv_raisesIRQL(DISPATCH_LEVEL)
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
CsampAcquireLock(
    __in                                   PIO_CSQ Csq,
    __out __drv_out_deref(__drv_savesIRQL) PKIRQL  Irql
    );

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
CsampReleaseLock(
    __in                              PIO_CSQ Csq,
    __in __drv_in(__drv_restoresIRQL) KIRQL   Irql
    );

VOID
CsampCompleteCanceledIrp(
    __in  PIO_CSQ             pCsq,
    __in  PIRP                Irp
    );

#endif




