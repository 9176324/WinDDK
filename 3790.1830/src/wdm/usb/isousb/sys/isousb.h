/*++

Copyright (c) 2004  Microsoft Corporation

Module Name:

    isousb.h

Abstract:

Environment:

    Kernel mode

Notes:

    Copyright (c) 2004 Microsoft Corporation.
    All Rights Reserved.

--*/

#include <initguid.h>
#include <wdm.h>
#include <wdmguid.h>
#include <wmistr.h>
#include <wmilib.h>
#include "usbdi.h"
#include "usbdlib.h"
#include "usbbusif.h"

#ifndef _ISOUSB_H
#define _ISOUSB_H

#define ISOTAG (ULONG) 'OsI'

#undef ExAllocatePool
#define ExAllocatePool(type, size) \
    ExAllocatePoolWithTag(type, size, ISOTAG);

#if DBG

#define IsoUsb_DbgPrint(level, _x_) \
            if((level) <= DebugLevel) { \
                DbgPrint _x_; \
            }
#else

#define IsoUsb_DbgPrint(level, _x_)

#endif

typedef struct _GLOBALS {

    UNICODE_STRING IsoUsb_RegistryPath;

} GLOBALS;

#define IDLE_INTERVAL 5000

typedef enum _DEVSTATE {

    NotStarted,         // not started
    Stopped,            // device stopped
    Working,            // started and working
    PendingStop,        // stop pending
    PendingRemove,      // remove pending
    SurpriseRemoved,    // removed by surprise
    Removed             // removed

} DEVSTATE;

typedef enum _QUEUE_STATE {

    HoldRequests,       // device is not started yet
    AllowRequests,      // device is ready to process
    FailRequests        // fail both existing and queued up requests

} QUEUE_STATE;

typedef enum _WDM_VERSION {

    WinXpOrBetter,
    Win2kOrBetter,
    WinMeOrBetter,
    Win98OrBetter

} WDM_VERSION;

#define INITIALIZE_PNP_STATE(_Data_)    \
        (_Data_)->DeviceState =  NotStarted;\
        (_Data_)->PrevDevState = NotStarted;

#define SET_NEW_PNP_STATE(_Data_, _state_) \
        (_Data_)->PrevDevState =  (_Data_)->DeviceState;\
        (_Data_)->DeviceState = (_state_);

#define RESTORE_PREVIOUS_PNP_STATE(_Data_)   \
        (_Data_)->DeviceState =   (_Data_)->PrevDevState;

#define ISOUSB_MAX_TRANSFER_SIZE    256
#define ISOUSB_TEST_BOARD_TRANSFER_BUFFER_SIZE (64 * 1024)


// IsoUsb_GetDescriptor Recipient parameter definitions
//
#define USB_RECIPIENT_DEVICE    0
#define USB_RECIPIENT_INTERFACE 1
#define USB_RECIPIENT_ENDPOINT  2
#define USB_RECIPIENT_OTHER     3

// Endpoint numbers are 0-15.  Endpoint number 0 is the standard control
// endpoint which is not explicitly listed in the Configuration Descriptor.
// There can be an IN endpoint and an OUT endpoint at endpoint numbers
// 1-15 so there can be a maximum of 30 endpoints per device configuration.
//
#define ISOUSB_MAX_PIPES        30


//
// registry path used for parameters
// global to all instances of the driver
//

#define ISOUSB_REGISTRY_PARAMETERS_PATH  \
    L"\\REGISTRY\\Machine\\System\\CurrentControlSet\\SERVICES\\ISOUSB\\Parameters"


//
// A structure representing the instance information associated with
// this particular device.
//

typedef struct _DEVICE_EXTENSION {

    // Functional Device Object
    PDEVICE_OBJECT FunctionalDeviceObject;

    // Device object we call when submitting Urbs
    PDEVICE_OBJECT TopOfStackDeviceObject;

    // The bus driver object
    PDEVICE_OBJECT PhysicalDeviceObject;

    // Name buffer for our named Functional device object link
    // The name is generated based on the driver's class GUID
    UNICODE_STRING InterfaceName;

    // Bus drivers set the appropriate values in this structure in response
    // to an IRP_MN_QUERY_CAPABILITIES IRP. Function and filter drivers might
    // alter the capabilities set by the bus driver.
    DEVICE_CAPABILITIES DeviceCapabilities;

    // Device Descriptor retrieved from the device
    //
    PUSB_DEVICE_DESCRIPTOR          DeviceDescriptor;

    // Configuration Descriptor retrieved from the device
    //
    PUSB_CONFIGURATION_DESCRIPTOR   ConfigurationDescriptor;

    // ConfigurationHandle returned by URB_FUNCTION_SELECT_CONFIGURATION.
    //
    USBD_CONFIGURATION_HANDLE       ConfigurationHandle;

    // Interface info returned by URB_FUNCTION_SELECT_CONFIGURATION.
    //
    PUSBD_INTERFACE_INFORMATION *   InterfaceInfoList;

    // Pipe info returned by URB_FUNCTION_SELECT_CONFIGURATION.
    // This array contains pointers back into the USBD_PIPE_INFORMATION
    // structures contained within USBD_INTERFACE_INFORMATION
    // structures which are pointed to by the above InterfaceInfoList.
    //
    PUSBD_PIPE_INFORMATION          PipeInformation[ISOUSB_MAX_PIPES];

    // Number of currently configured pipes, consistent with the above
    // InterfaceInfoList and PipeInformation fields.
    //
    ULONG                           NumberOfPipes;

    // Semaphore to provide exclusion around configuration changes.
    //
    KSEMAPHORE                      ConfigurationSemaphore;

    // current state of device
    DEVSTATE DeviceState;

    // state prior to removal query
    DEVSTATE PrevDevState;

    // obtain and hold this lock while changing the device state,
    // the queue state and while processing the queue.
    KSPIN_LOCK DevStateLock;

    // current system power state
    SYSTEM_POWER_STATE SysPower;

    // current device power state
    DEVICE_POWER_STATE DevPower;

    // Pending I/O queue state
    QUEUE_STATE QueueState;

    // Pending I/O queue
    LIST_ENTRY NewRequestsQueue;

    // I/O Queue Lock
    KSPIN_LOCK QueueLock;

    KEVENT RemoveEvent;

    KEVENT StopEvent;

    ULONG OutStandingIO;

    KSPIN_LOCK IOCountLock;

    // selective suspend variables

    LONG SSEnable;

    LONG SSRegistryEnable;

    PUSB_IDLE_CALLBACK_INFO IdleCallbackInfo;

    PIRP PendingIdleIrp;

    LONG IdleReqPend;

    LONG FreeIdleIrpCount;

    KSPIN_LOCK IdleReqStateLock;

    KEVENT NoIdleReqPendEvent;

    // default power state to power down to on self-susped
    ULONG PowerDownLevel;

    // remote wakeup variables
    PIRP WaitWakeIrp;

    LONG FlagWWCancel;

    LONG FlagWWOutstanding;

    LONG WaitWakeEnable;

    // open handle count
    LONG OpenHandleCount;

    // selective suspend model uses timers, dpcs and work item.
    KTIMER Timer;

    KDPC DeferredProcCall;

    // This event is cleared when a DPC/Work Item is queued.
    // and signaled when the work-item completes.
    // This is essential to prevent the driver from unloading
    // while we have DPC or work-item queued up.
    KEVENT NoDpcWorkItemPendingEvent;

    // WMI information
    WMILIB_CONTEXT WmiLibInfo;

    // WDM version
    WDM_VERSION WdmVersion;

    // High speed
    ULONG IsDeviceHighSpeed;

} DEVICE_EXTENSION, *PDEVICE_EXTENSION;


typedef struct _IRP_COMPLETION_CONTEXT {

    PDEVICE_EXTENSION DeviceExtension;

    PKEVENT Event;

} IRP_COMPLETION_CONTEXT, *PIRP_COMPLETION_CONTEXT;

extern ULONG DebugLevel;
extern GLOBALS Globals;

#endif
