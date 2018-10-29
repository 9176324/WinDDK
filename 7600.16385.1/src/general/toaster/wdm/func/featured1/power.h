/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    Power.h

Abstract:

    Power.h defines the data types used in all the stages of the function driver
    which implement power management support in Power.c.

Environment:

    Kernel mode

Revision History:

--*/

#if !defined(_POWER_H_)
#define _POWER_H_

//
// Define an enumeration to describe whether a power IRP has been processed by the
// underlying bus driver and does not need to be passed down the device stack, or
// whether the power IRP has not been passed down to the underlying bus driver to
// be processed.
//
// IRP_NEEDS_FORWARDING indicates that the function driver must pass the power IRP
// down the device stack to be processed by the underlying bus driver.
//
// IRP_MN_ALREADY_FORWARDED indicates that the underlying bus driver has already
// processed the power IRP and the function driver does not need to pass the power
// IRP down the device stack.
//
typedef enum {

    IRP_NEEDS_FORWARDING = 1,
    IRP_ALREADY_FORWARDED

} IRP_DIRECTION;

//
// Define a function signature for a work item for the system worker thread to
// call back at IRQL = PASSIVE_LEVEL.
//
typedef VOID (*PFN_QUEUE_SYNCHRONIZED_CALLBACK)(
    __in  PDEVICE_OBJECT      DeviceObject,
    __in  PIRP                Irp,
    __in  IRP_DIRECTION       Direction
    );

//
// Define the operating context for a work item for the system worker thread to
// process a power IRP at IRQL = PASSIVE_LEVEL.
//
// The context contains the hardware instance and power IRP to process at
// IRQL = PASSIVE_LEVEL. The context also contains whether the power IRP has
// already been passed down the device stack to be processed by the underlying
// bus driver, or if the power IRP must still be passed down the device stack.
// The context also contains a pointer to the callback function implemented by
// the function driver. The callback function must match the function signature
// defined by the PFN_QUEUE_SYNCHRONIZED_CALLBACK data type.
// The context also contains the work item for the system worker thread to process
// at IRQL = PASSIVE_LEVEL.
//
typedef struct _WORKER_THREAD_CONTEXT {

    PDEVICE_OBJECT                  DeviceObject;
    PIRP                            Irp;
    IRP_DIRECTION                   IrpDirection;
    PFN_QUEUE_SYNCHRONIZED_CALLBACK Callback;
    PIO_WORKITEM                    WorkItem;

} WORKER_THREAD_CONTEXT, *PWORKER_THREAD_CONTEXT;


//
// Declare the function prototypes for all of the function driver's power routines in
// the Featured1 and Featured2 stages of the function driver.
//

DRIVER_DISPATCH ToasterDispatchPowerDefault;

DRIVER_DISPATCH ToasterDispatchQueryPowerState;

DRIVER_DISPATCH ToasterDispatchSetPowerState;

DRIVER_DISPATCH ToasterDispatchSystemPowerIrp;

DRIVER_DISPATCH ToasterDispatchDeviceQueryPower;

DRIVER_DISPATCH ToasterDispatchDeviceSetPower;

IO_COMPLETION_ROUTINE ToasterCompletionSystemPowerUp;

IO_COMPLETION_ROUTINE ToasterCompletionDevicePowerUp;

IO_WORKITEM_ROUTINE ToasterQueuePassiveLevelPowerCallbackWorker;

VOID
ToasterQueueCorrespondingDeviceIrp (
    __in PIRP SIrp,
    __in PDEVICE_OBJECT DeviceObject
     );

REQUEST_POWER_COMPLETE ToasterCompletionOnFinalizedDeviceIrp;

NTSTATUS
ToasterFinalizeDevicePowerIrp (
    __in  PDEVICE_OBJECT      DeviceObject,
    __in  PIRP                Irp,
    __in  IRP_DIRECTION       Direction,
    __in  NTSTATUS            Result
    );

NTSTATUS
ToasterQueuePassiveLevelPowerCallback(
    __in  PDEVICE_OBJECT                      DeviceObject,
    __in  PIRP                                Irp,
    __in  IRP_DIRECTION                       Direction,
    __in  PFN_QUEUE_SYNCHRONIZED_CALLBACK     Callback
    );

VOID
ToasterCallbackHandleDeviceQueryPower (
    __in  PDEVICE_OBJECT      DeviceObject,
    __in  PIRP                Irp,
    __in  IRP_DIRECTION       Direction
    );

VOID
ToasterCallbackHandleDeviceSetPower (
    __in  PDEVICE_OBJECT      DeviceObject,
    __in  PIRP                Irp,
    __in  IRP_DIRECTION       Direction
    );

NTSTATUS
ToasterPowerBeginQueuingIrps(
    __in  PDEVICE_OBJECT      DeviceObject,
    __in  ULONG               IrpIoCharges,
    __in  BOOLEAN             Query
    );

NTSTATUS
ToasterGetPowerPoliciesDeviceState(
    __in  PIRP                SIrp,
    __in  PDEVICE_OBJECT      DeviceObject,
     __out DEVICE_POWER_STATE *DevicePowerState
    );

NTSTATUS
ToasterCanSuspendDevice(
    __in PDEVICE_OBJECT   DeviceObject
    );

PCHAR
DbgPowerMinorFunctionString (
    __in UCHAR MinorFunction
    );

PCHAR
DbgSystemPowerString(
    __in SYSTEM_POWER_STATE Type
    );

PCHAR
DbgDevicePowerString(
    __in DEVICE_POWER_STATE Type
    );

#endif


