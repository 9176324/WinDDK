/*++

Copyright (C) Microsoft Corporation, 1991 - 1999

Module Name:

    clntirp.c

Abstract:

    Client IRP queuing routines for CLASSPNP

Environment:

    kernel mode only

Notes:


Revision History:

--*/

#include <ntifs.h>
#include "classp.h"
#include "debug.h"


#ifdef DEBUG_USE_WPP
#include "clntirp.tmh"
#endif

VOID
ClasspStartIdleTimer(
    PCLASS_PRIVATE_FDO_DATA FdoData
    );

VOID
ClasspStopIdleTimer(
    PCLASS_PRIVATE_FDO_DATA FdoData
    );

KDEFERRED_ROUTINE ClasspIdleTimerDpc;

VOID
ClasspServiceIdleRequest(
        PFUNCTIONAL_DEVICE_EXTENSION FdoExtension
    );

PIRP
ClasspDequeueIdleRequest(
    PCLASS_PRIVATE_FDO_DATA FdoData
    );


/*++

EnqueueDeferredClientIrp

Routine Description:

    Insert the deferred irp into the list.

    Note: we currently do not support Cancel for storage irps.

Arguments:

    FdoData - Pointer to the private fdo data
    Irp     - Pointer to the I/O request packet

Return Value:

    None

--*/
VOID
EnqueueDeferredClientIrp(
    PCLASS_PRIVATE_FDO_DATA FdoData,
    PIRP Irp
    )
{
    KIRQL oldIrql;

    KeAcquireSpinLock(&FdoData->SpinLock, &oldIrql);
    InsertTailList(&FdoData->DeferredClientIrpList, &Irp->Tail.Overlay.ListEntry);
    KeReleaseSpinLock(&FdoData->SpinLock, oldIrql);
}

/*++

DequeueDeferredClientIrp

Routine Description:

    Remove the deferred irp from the list.

Arguments:

    FdoData - Pointer to the private fdo data

Return Value:

    Pointer to removed IRP

--*/
PIRP
DequeueDeferredClientIrp(
    PCLASS_PRIVATE_FDO_DATA FdoData
    )
{
    PIRP irp;

    //
    // The DeferredClientIrpList is almost always empty.
    // We don't want to grab the spinlock every time we check it (which is on every xfer completion)
    // so check once first before we grab the spinlock.
    //
    if (IsListEmpty(&FdoData->DeferredClientIrpList)){
        irp = NULL;
    }
    else {
        PLIST_ENTRY listEntry;
        KIRQL oldIrql;

        KeAcquireSpinLock(&FdoData->SpinLock, &oldIrql);
        if (IsListEmpty(&FdoData->DeferredClientIrpList)){
            listEntry = NULL;
        }
        else {
            listEntry = RemoveHeadList(&FdoData->DeferredClientIrpList);
        }
        KeReleaseSpinLock(&FdoData->SpinLock, oldIrql);

        if (listEntry == NULL) {
            irp = NULL;
        }
        else {
            irp = CONTAINING_RECORD(listEntry, IRP, Tail.Overlay.ListEntry);
            ASSERT(irp->Type == IO_TYPE_IRP);
            InitializeListHead(&irp->Tail.Overlay.ListEntry);
        }
    }

    return irp;
}

/*++

ClasspInitializeIdleTimer

Routine Description:

    Initialize the idle timer for the given device.

Arguments:

    FdoExtension    - Pointer to the device extension
    IdleInterval    - Timer interval

Return Value:

    None

--*/
VOID
ClasspInitializeIdleTimer(
    PFUNCTIONAL_DEVICE_EXTENSION FdoExtension
    )
{
    PCLASS_PRIVATE_FDO_DATA fdoData = FdoExtension->PrivateFdoData;
    ULONG idleInterval = CLASS_IDLE_INTERVAL;
    ULONG idlePrioritySupported = TRUE;

    ClassGetDeviceParameter(FdoExtension,
                            CLASSP_REG_SUBKEY_NAME,
                            CLASSP_REG_IDLE_PRIORITY_SUPPORTED,
                            &idlePrioritySupported);


    if (idlePrioritySupported == FALSE) {
        //
        // User has set the registry to disable idle priority for this disk.
        // No need to initialize any further. 
    // Always ensure that none of the other fields used for idle priority
    // io are ever used without checking if it is supported.
        //
        TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_TIMER, "ClasspInitializeIdleTimer: Idle priority not supported for disk %p\n", FdoExtension));
    fdoData->IdlePrioritySupported = FALSE;
        fdoData->IdleIoCount = 0;
        fdoData->ActiveIoCount = 0;
        return;
    }

    ClassGetDeviceParameter(FdoExtension,
                            CLASSP_REG_SUBKEY_NAME,
                            CLASSP_REG_IDLE_INTERVAL_NAME,
                            &idleInterval);

    if (idleInterval < 2) {
        //
        // If the interval is too low, reset it to the default value
        //
        idleInterval = CLASS_IDLE_INTERVAL;
    }

    fdoData->IdlePrioritySupported = TRUE;
    KeInitializeSpinLock(&fdoData->IdleListLock);
    KeInitializeTimer(&fdoData->IdleTimer);
    KeInitializeDpc(&fdoData->IdleDpc, ClasspIdleTimerDpc, FdoExtension);
    InitializeListHead(&fdoData->IdleIrpList);
    fdoData->IdleTimerStarted = FALSE;
    fdoData->IdleTimerInterval = (USHORT) (idleInterval / 2);
    fdoData->StarvationCount = CLASS_STARVATION_INTERVAL / fdoData->IdleTimerInterval;
    fdoData->IdleTimerTicks = 0;
    fdoData->IdleTicks = 0;
    fdoData->IdleIoCount = 0;
    fdoData->ActiveIoCount = 0;
    fdoData->ActiveIdleIoCount = 0;

    return;
}

/*++

ClasspStartIdleTimer

Routine Description:

    Start the idle timer if not already running. Reset the
    timer counters before starting the timer. Use the IdleInterval
    in the private fdo data to setup the timer.

Arguments:

    FdoData - Pointer to the private fdo data

Return Value:

    None

--*/
VOID
ClasspStartIdleTimer(
    PCLASS_PRIVATE_FDO_DATA FdoData
    )
{
    LARGE_INTEGER dueTime;
    ULONG mstotimer;
    LONG timerStarted;

    TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_TIMER, "ClasspStartIdleTimer: Start idle timer\n"));

    timerStarted = InterlockedCompareExchange(&FdoData->IdleTimerStarted, 1, 0);

    if (!timerStarted) {

        FdoData->IdleTimerTicks = 0;
        FdoData->IdleTicks = 0;

        //
        // convert milliseconds to a relative 100ns
        //
        mstotimer = (-10) * 1000;

        //
        // multiply the period
        //
        dueTime.QuadPart = Int32x32To64(FdoData->IdleTimerInterval, mstotimer);

        KeSetTimerEx(&FdoData->IdleTimer,
                     dueTime,
                     FdoData->IdleTimerInterval,
                     &FdoData->IdleDpc);
    }
    return;
}

/*++

ClasspStopIdleTimer

Routine Description:

    Stop the idle timer if running. Also reset the timer counters.

Arguments:

    FdoData - Pointer to the private fdo data

Return Value:

    None

--*/
VOID
ClasspStopIdleTimer(
    PCLASS_PRIVATE_FDO_DATA FdoData
    )
{
    LONG timerStarted;

    TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_TIMER, "ClasspStopIdleTimer: Stop idle timer\n"));

    timerStarted = InterlockedCompareExchange(&FdoData->IdleTimerStarted, 0, 1);

    if (timerStarted) {
        (VOID)KeCancelTimer(&FdoData->IdleTimer);
        FdoData->IdleTimerTicks = 0;
        FdoData->IdleTicks = 0;
    }
    return;
}

/*++

ClasspIdleTimerDpc

Routine Description:

    Timer dpc function. This function will be called once every
    IdleInterval. This will increment the IdleTicks and
    if it goes above 1 (i.e., disk is in idle state) then
    it will service an idle request.

    This function will increment IdleTimerTicks if the IdleTicks
    does not go above 1 (i.e., disk is not in idle state). When it
    reaches the starvation idle count (1 second) it will process
    one idle request.

Arguments:

    Dpc             - Pointer to DPC object
    Context         - Pointer to the fdo device extension
    SystemArgument1 - Not used
    SystemArgument2 - Not used

Return Value:

    None

--*/
VOID
ClasspIdleTimerDpc(
    IN PKDPC Dpc,
    IN PVOID Context,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    )
{
    PFUNCTIONAL_DEVICE_EXTENSION fdoExtension = Context;
    PCLASS_PRIVATE_FDO_DATA fdoData = fdoExtension->PrivateFdoData;

    if ((fdoData->ActiveIoCount <= 0) && (++fdoData->IdleTicks > 1)) {
        ClasspServiceIdleRequest(fdoExtension);
        return;
    }

    //
    // If the timer is running then there must be at least one idle priority I/O pending
    //
    if (++fdoData->IdleTimerTicks >= fdoData->StarvationCount) {
        fdoData->IdleTimerTicks = 0;
        TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_TIMER, "ClasspIdleTimerDpc: Starvation timer. Send one idle request\n"));
        ClasspServiceIdleRequest(fdoExtension);
    }
    return;
}

/*++

ClasspEnqueueIdleRequest

Routine Description:

    This function will insert the idle request into the list.
    If the inserted reqeust is the first request then it will
    start the timer.

Arguments:

    DeviceObject    - Pointer to device object
    Irp             - Pointer to the idle I/O request packet

Return Value:

    NT status code.

--*/
NTSTATUS
ClasspEnqueueIdleRequest(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp
    )
{
    PFUNCTIONAL_DEVICE_EXTENSION fdoExtension = DeviceObject->DeviceExtension;
    PCLASS_PRIVATE_FDO_DATA fdoData = fdoExtension->PrivateFdoData;
    KIRQL oldIrql;
    LARGE_INTEGER currentTime;
    BOOLEAN isFirstRequest = FALSE;
    ULONGLONG diffTime;
    NTSTATUS status;

    TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_TIMER, "ClasspEnqueueIdleRequest: Queue idle request %p\n", Irp));

    IoMarkIrpPending(Irp);

    KeQuerySystemTime(&currentTime);

    //
    // Get the time difference between currnt time and last I/O complete time
    //
    status = RtlULongLongSub((ULONGLONG)currentTime.QuadPart, (ULONGLONG)fdoData->LastIoTime.QuadPart, &diffTime);
    if (NT_SUCCESS(status)) {
        //
        // Convert the time to milliseconds
        //
        diffTime = diffTime / 10000;
    } else {
        //
        // Failed to get time difference, use zero as the time difference.
        //
        diffTime = 0;
    }

    //
    // If there is any active reqeust in the port driver, then queue the idle request.
    //
    if (fdoData->ActiveIoCount > 0) {
        diffTime = 0;
    }

    TracePrint((TRACE_LEVEL_VERBOSE, TRACE_FLAG_TIMER, "ClasspEnqueueIdleRequest: Diff time %I64d\n", diffTime));

    KeAcquireSpinLock(&fdoData->IdleListLock, &oldIrql);
    if (IsListEmpty(&fdoData->IdleIrpList)) {
        ASSERT(fdoData->IdleIoCount == 0);
    }
    InsertTailList(&fdoData->IdleIrpList, &Irp->Tail.Overlay.ListEntry);
    fdoData->IdleIoCount++;
    if (!fdoData->IdleTimerStarted) {
        ClasspStartIdleTimer(fdoData);
    }
    isFirstRequest = fdoData->IdleIoCount == 1 ? TRUE : FALSE;

    
    KeReleaseSpinLock(&fdoData->IdleListLock, oldIrql);

    if (isFirstRequest && (diffTime > CLASS_IDLE_INTERVAL)) {
        ClasspServiceIdleRequest(fdoExtension);
    }

    return STATUS_PENDING;
}

/*++

ClasspDequeueIdleRequest

Routine Description:

    This function will remove the next idle request from the list.
    If there are no requests in the queue, then it will return NULL.

Arguments:

    FdoData         - Pointer to the private fdo data
    PendingIoCount  - Count of pending idle requests in the list

Return Value:

    Pointer to removed IRP

--*/
PIRP
ClasspDequeueIdleRequest(
    PCLASS_PRIVATE_FDO_DATA FdoData
    )
{
    PLIST_ENTRY listEntry = NULL;
    PIRP irp = NULL;
    KIRQL oldIrql;

    KeAcquireSpinLock(&FdoData->IdleListLock, &oldIrql);

    if (FdoData->IdleIoCount > 0) {
        listEntry = RemoveHeadList(&FdoData->IdleIrpList);
        //
        // Make sure we actaully removed a request from the list
        //
        ASSERT(listEntry != &FdoData->IdleIrpList);
        //
        // Decrement the idle I/O count.
        //
        FdoData->IdleIoCount--;
        //
        // Stop the timer on last request
        //
        if (FdoData->IdleIoCount == 0) {
            ClasspStopIdleTimer(FdoData);
        }
        irp = CONTAINING_RECORD(listEntry, IRP, Tail.Overlay.ListEntry);
        ASSERT(irp->Type == IO_TYPE_IRP);
        InitializeListHead(&irp->Tail.Overlay.ListEntry);        
    }
    
    KeReleaseSpinLock(&FdoData->IdleListLock, oldIrql);
    return irp;
}

/*++

ClasspCompleteIdleRequest

Routine Description:

    This function will be called every time an idle request is completed.
    This will call ClasspServiceIdleRequest to process any other pending idle requests.

Arguments:

    FdoExtension    - Pointer to the device extension

Return Value:

    None

--*/
VOID
ClasspCompleteIdleRequest(
    PFUNCTIONAL_DEVICE_EXTENSION FdoExtension
    )
{
    PCLASS_PRIVATE_FDO_DATA fdoData = FdoExtension->PrivateFdoData;

    if ((fdoData->IdleIoCount > 0) && (fdoData->IdleTicks > 0)) {
        TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_TIMER, "ClasspCompleteIdleRequest: Service next idle reqeusts\n"));
        ClasspServiceIdleRequest(FdoExtension);
    }
    return;
}

/*++

ClasspServiceIdleRequest

Routine Description:

    Remove the next pending idle request from the queue and process it.
    If a request was removed then it will be processed otherwise it will
    just return.

Arguments:

    FdoExtension    - Pointer to the device extension

Return Value:

    None

--*/
VOID
ClasspServiceIdleRequest(
    PFUNCTIONAL_DEVICE_EXTENSION FdoExtension
    )
{
    PCLASS_PRIVATE_FDO_DATA fdoData = FdoExtension->PrivateFdoData;
    PIRP irp;

    irp = ClasspDequeueIdleRequest(fdoData);
    if (irp != NULL) {
        ServiceTransferRequest(FdoExtension->DeviceObject, irp);
    }
    return;
}



