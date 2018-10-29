/*++

Copyright (C) Microsoft Corporation, 1991 - 1999

Module Name:

    power.c

Abstract:

    SCSI class driver routines

Environment:

    kernel mode only

Notes:


Revision History:

--*/

#include "stddef.h"
#include "ntddk.h"
#include "scsi.h"
#include "classp.h"

#include <stdarg.h>

#ifdef DEBUG_USE_WPP
#include "power.tmh"
#endif

#define CLASS_TAG_POWER     'WLcS'

// constants for power transition process. (UNIT: seconds)
#define DEFAULT_POWER_IRP_TIMEOUT_VALUE 10*60   
#define TIME_LEFT_FOR_LOWER_DRIVERS     30      
#define TIME_LEFT_FOR_UPPER_DRIVERS     5   
#define DEFAULT_IO_TIMEOUT_VALUE        10   
#define MINIMUM_STOP_UNIT_TIMEOUT_VALUE 2   

// PoQueryWatchdogTime was introduced in Windows 7.
// Returns TRUE if a watchdog-enabled power IRP is found, otherwise FALSE.
#if (NTDDI_VERSION < NTDDI_WIN7)
#define PoQueryWatchdogTime(A, B) FALSE
#endif

IO_COMPLETION_ROUTINE ClasspPowerDownCompletion;

IO_COMPLETION_ROUTINE ClasspPowerUpCompletion;

IO_COMPLETION_ROUTINE ClasspStartNextPowerIrpCompletion;

NTSTATUS
ClasspPowerHandler(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN CLASS_POWER_OPTIONS Options
    );

VOID
RetryPowerRequest(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp,
    PCLASS_POWER_CONTEXT Context
    );


/*++////////////////////////////////////////////////////////////////////////////

ClassDispatchPower()

Routine Description:

    This routine acquires the removelock for the irp and then calls the
    appropriate power callback.

Arguments:

    DeviceObject -
    Irp -

Return Value:

--*/
NTSTATUS
ClassDispatchPower(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{
    PCOMMON_DEVICE_EXTENSION commonExtension = DeviceObject->DeviceExtension;
    ULONG isRemoved;
    PCLASS_POWER_DEVICE powerRoutine = NULL;

    //
    // NOTE: This code may be called at PASSIVE or DISPATCH, depending
    //       upon the device object it is being called for.
    //       don't do anything that would break under either circumstance.
    //

    NTSTATUS status;

    //
    // If device is added but not yet started, we need to send the Power
    // request down the stack.  If device is started and then stopped,
    // we have enough state to process the power request.
    //

    if (!commonExtension->IsInitialized) {

        PoStartNextPowerIrp(Irp);
        IoSkipCurrentIrpStackLocation(Irp);
        return PoCallDriver(commonExtension->LowerDeviceObject, Irp);
    }

    isRemoved = ClassAcquireRemoveLock(DeviceObject, Irp);

    if(isRemoved) {
        ClassReleaseRemoveLock(DeviceObject, Irp);
        Irp->IoStatus.Status = STATUS_DEVICE_DOES_NOT_EXIST;
        PoStartNextPowerIrp(Irp);
        ClassCompleteRequest(DeviceObject, Irp, IO_NO_INCREMENT);
        return STATUS_DEVICE_DOES_NOT_EXIST;
    }

    return commonExtension->DevInfo->ClassPowerDevice(DeviceObject, Irp);
} // end ClassDispatchPower()

/*++////////////////////////////////////////////////////////////////////////////

ClasspPowerUpCompletion()

Routine Description:

    This routine is used for intermediate completion of a power up request.
    PowerUp requires four requests to be sent to the lower driver in sequence.

        * The queue is "power locked" to ensure that the class driver power-up
          work can be done before request processing resumes.

        * The power irp is sent down the stack for any filter drivers and the
          port driver to return power and resume command processing for the
          device.  Since the queue is locked, no queued irps will be sent
          immediately.

        * A start unit command is issued to the device with appropriate flags
          to override the "power locked" queue.

        * The queue is "power unlocked" to start processing requests again.

    This routine uses the function in the srb which just completed to determine
    which state it is in.

Arguments:

    DeviceObject - the device object being powered up

    Irp - Context->Irp: original power irp; fdoExtension->PrivateFdoData->PowerProcessIrp: power process irp

    Srb - the SRB used to perform port/class operations.

Return Value:

    STATUS_MORE_PROCESSING_REQUIRED or
    STATUS_SUCCESS

--*/
NTSTATUS
ClasspPowerUpCompletion(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    )
{
    PCLASS_POWER_CONTEXT PowerContext = (PCLASS_POWER_CONTEXT)Context;
    PCOMMON_DEVICE_EXTENSION commonExtension = PowerContext->DeviceObject->DeviceExtension;
    PFUNCTIONAL_DEVICE_EXTENSION fdoExtension = PowerContext->DeviceObject->DeviceExtension;
    PIRP OriginalIrp = PowerContext->Irp;


    // currentStack - from original power irp
    // nextStack - from power process irp
    PIO_STACK_LOCATION currentStack = IoGetCurrentIrpStackLocation(OriginalIrp);
    PIO_STACK_LOCATION nextStack = IoGetNextIrpStackLocation(fdoExtension->PrivateFdoData->PowerProcessIrp);

    NTSTATUS status = STATUS_MORE_PROCESSING_REQUIRED;

    UNREFERENCED_PARAMETER(DeviceObject);

    TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "ClasspPowerUpCompletion: Device Object %p, Irp %p, "
                   "Context %p\n",
                PowerContext->DeviceObject, Irp, Context));

    ASSERT(!TEST_FLAG(PowerContext->Srb.SrbFlags, SRB_FLAGS_FREE_SENSE_BUFFER));
    ASSERT(!TEST_FLAG(PowerContext->Srb.SrbFlags, SRB_FLAGS_PORT_DRIVER_ALLOCSENSE));
    ASSERT(PowerContext->Options.PowerDown == FALSE);
    ASSERT(PowerContext->Options.HandleSpinUp);

    if((Irp == OriginalIrp) && (Irp->PendingReturned)) {
        // only for original power irp
        IoMarkIrpPending(Irp);
    }

    PowerContext->PowerChangeState.PowerUp++;

    switch(PowerContext->PowerChangeState.PowerUp) {

        case PowerUpDeviceLocked: {

            TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tPreviously sent power lock\n", Irp));

            //
            // Lock Queue operation has been sent.
            // Now, send the original power irp down to get lower driver and device ready.
            //

            IoCopyCurrentIrpStackLocationToNext(OriginalIrp);

            if((PowerContext->Options.LockQueue == TRUE) &&
               (!NT_SUCCESS(Irp->IoStatus.Status))) {

                //
                // Lock was not successful:
                // Issue the original power request to the lower driver and next power irp will be started in completion routine.
                //

                TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tIrp status was %lx\n",
                            Irp, Irp->IoStatus.Status));
                TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tSrb status was %lx\n",
                            Irp, PowerContext->Srb.SrbStatus));

                //
                // Set the new power state
                //

                fdoExtension->DevicePowerState = currentStack->Parameters.Power.State.DeviceState;

                OriginalIrp->IoStatus.Status = STATUS_NOT_SUPPORTED;

                IoSetCompletionRoutine(OriginalIrp,
                                       ClasspStartNextPowerIrpCompletion,
                                       PowerContext,
                                       TRUE,
                                       TRUE,
                                       TRUE);

                //
                // Indicate to Po that we've been successfully powered up so
                // it can do it's notification stuff.
                //

                PoSetPowerState(PowerContext->DeviceObject,
                                currentStack->Parameters.Power.Type,
                                currentStack->Parameters.Power.State);

                PoCallDriver(commonExtension->LowerDeviceObject, OriginalIrp);

                ClassReleaseRemoveLock(commonExtension->DeviceObject,
                                       OriginalIrp);

                return STATUS_MORE_PROCESSING_REQUIRED;

            } else {
                PowerContext->QueueLocked = (UCHAR)PowerContext->Options.LockQueue;
            }

            Irp->IoStatus.Status = STATUS_NOT_SUPPORTED;

            PowerContext->PowerChangeState.PowerUp = PowerUpDeviceLocked;

            IoSetCompletionRoutine(OriginalIrp,
                                   ClasspPowerUpCompletion,
                                   PowerContext,
                                   TRUE,
                                   TRUE,
                                   TRUE);

            status = PoCallDriver(commonExtension->LowerDeviceObject, OriginalIrp);

            TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tIoCallDriver returned %lx\n", OriginalIrp, status));
            break;
        }

        case PowerUpDeviceOn: {

            //
            // Original power irp has been completed by lower driver.
            //

            if(NT_SUCCESS(Irp->IoStatus.Status)) {
                //
                // If power irp succeeded, START UNIT command will be sent.
                //
                PCDB cdb;
                ULONG secondsRemaining = 0;
                ULONG timeoutValue = 0;

                if(PoQueryWatchdogTime(fdoExtension->LowerPdo, &secondsRemaining)) {

                    // do not exceed DEFAULT_POWER_IRP_TIMEOUT_VALUE.
                    secondsRemaining = min(secondsRemaining, DEFAULT_POWER_IRP_TIMEOUT_VALUE);
                    
                    // plan to leave (TIME_LEFT_FOR_UPPER_DRIVERS) seconds to upper level drivers
                    // for processing original power irp.
                    if (secondsRemaining >= (TIME_LEFT_FOR_UPPER_DRIVERS + START_UNIT_TIMEOUT)) {
                        fdoExtension->PrivateFdoData->MaxPowerOperationRetryCount = 
                            (secondsRemaining - TIME_LEFT_FOR_UPPER_DRIVERS) / START_UNIT_TIMEOUT;


                        // * No 'short' timeouts
                        //
			//
                        // timeoutValue = (secondsRemaining - TIME_LEFT_FOR_UPPER_DRIVERS) % 
                        //                START_UNIT_TIMEOUT;
                        //                        
                        // if (timeoutValue < DEFAULT_IO_TIMEOUT_VALUE)
                        // {

                            if (--fdoExtension->PrivateFdoData->MaxPowerOperationRetryCount)
                            {
                                timeoutValue = START_UNIT_TIMEOUT;
                            } else {
                                timeoutValue = secondsRemaining - TIME_LEFT_FOR_UPPER_DRIVERS;
                            }
                        // }
                    } else {
                        // issue the command with minimum timeout value and do not retry on it.
                        // case of (secondsRemaining < DEFAULT_IO_TIMEOUT_VALUE) is ingored as it should not happen.
                        ASSERT(secondsRemaining >= DEFAULT_IO_TIMEOUT_VALUE);

                        fdoExtension->PrivateFdoData->MaxPowerOperationRetryCount = 0;
                        timeoutValue = DEFAULT_IO_TIMEOUT_VALUE; // use normal IO timeout value for this corner case.
                    }

                } else {
                    // don't know how long left, do not exceed DEFAULT_POWER_IRP_TIMEOUT_VALUE.
                    fdoExtension->PrivateFdoData->MaxPowerOperationRetryCount =
                        DEFAULT_POWER_IRP_TIMEOUT_VALUE / START_UNIT_TIMEOUT - 1;
                    timeoutValue = START_UNIT_TIMEOUT;
                }


                TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tSending start unit to device\n", Irp));

                //
                // Issue the start unit command to the device.
                //

                PowerContext->RetryCount = fdoExtension->PrivateFdoData->MaxPowerOperationRetryCount;
                PowerContext->Srb.TimeOutValue = timeoutValue;

                PowerContext->Srb.Length = sizeof(SCSI_REQUEST_BLOCK);
                PowerContext->Srb.Function = SRB_FUNCTION_EXECUTE_SCSI;

                PowerContext->Srb.SrbStatus = PowerContext->Srb.ScsiStatus = 0;
                PowerContext->Srb.DataTransferLength = 0;

                PowerContext->Srb.SrbFlags = SRB_FLAGS_NO_DATA_TRANSFER |
                                        SRB_FLAGS_DISABLE_AUTOSENSE |
                                        SRB_FLAGS_DISABLE_SYNCH_TRANSFER |
                                        SRB_FLAGS_NO_QUEUE_FREEZE;

                if(PowerContext->Options.LockQueue) {
                    SET_FLAG(PowerContext->Srb.SrbFlags, SRB_FLAGS_BYPASS_LOCKED_QUEUE);
                }

                PowerContext->Srb.CdbLength = 6;

                cdb = (PCDB) (PowerContext->Srb.Cdb);
                RtlZeroMemory(cdb, sizeof(CDB));

                cdb->START_STOP.OperationCode = SCSIOP_START_STOP_UNIT;
                cdb->START_STOP.Start = 1;

                PowerContext->PowerChangeState.PowerUp = PowerUpDeviceOn;

                IoSetCompletionRoutine(fdoExtension->PrivateFdoData->PowerProcessIrp,
                                       ClasspPowerUpCompletion,
                                       PowerContext,
                                       TRUE,
                                       TRUE,
                                       TRUE);

                nextStack->Parameters.Scsi.Srb = &(PowerContext->Srb);
                nextStack->MajorFunction = IRP_MJ_SCSI;

                status = IoCallDriver(commonExtension->LowerDeviceObject, fdoExtension->PrivateFdoData->PowerProcessIrp);

                TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tIoCallDriver returned %lx\n", fdoExtension->PrivateFdoData->PowerProcessIrp, status));

            } else {

                //
                // power irp is failed by lower driver. we're done.
                //

                PowerContext->FinalStatus = Irp->IoStatus.Status;
                goto ClasspPowerUpCompletionFailure;
            }

            break;
        }

        case PowerUpDeviceStarted: { // 3

            //
            // First deal with an error if one occurred.
            //

            if(SRB_STATUS(PowerContext->Srb.SrbStatus) != SRB_STATUS_SUCCESS) {

                BOOLEAN retry;
                LONGLONG delta100nsUnits = 0;

                TracePrint((TRACE_LEVEL_ERROR, TRACE_FLAG_POWER, "%p\tError occured when issuing START_UNIT "
                            "command to device. Srb %p, Status %x\n",
                            Irp,
                            &PowerContext->Srb,
                            PowerContext->Srb.SrbStatus));

                ASSERT(!(TEST_FLAG(PowerContext->Srb.SrbStatus,
                                   SRB_STATUS_QUEUE_FROZEN)));
                ASSERT(PowerContext->Srb.Function == SRB_FUNCTION_EXECUTE_SCSI);

                PowerContext->RetryInterval = 0;
                retry = InterpretSenseInfoWithoutHistory(
                            fdoExtension->DeviceObject,
                            Irp,
                            &PowerContext->Srb,
                            IRP_MJ_SCSI,
                            IRP_MJ_POWER,
                            fdoExtension->PrivateFdoData->MaxPowerOperationRetryCount - PowerContext->RetryCount,
                            &status,
                            &delta100nsUnits);

                // NOTE: Power context is a public structure, and thus cannot be
                //       updated to use 100ns units.  Therefore, must store the
                //       one-second equivalent.  Round up to ensure minimum delay
                //       requirements have been met.
                delta100nsUnits += (10*1000*1000) - 1;
                delta100nsUnits /= (10*1000*1000);
                // guaranteed not to have high bits set per SAL annotations
                PowerContext->RetryInterval = (ULONG)(delta100nsUnits);


                if((retry == TRUE) && (PowerContext->RetryCount-- != 0)) {

                    TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tRetrying failed request\n", Irp));

                    //
                    // Decrement the state so we come back through here the
                    // next time.
                    //

                    PowerContext->PowerChangeState.PowerUp--;

                    PowerContext->Srb.TimeOutValue = START_UNIT_TIMEOUT;

                    RetryPowerRequest(commonExtension->DeviceObject,
                                      Irp,
                                      PowerContext);

                    break;

                }

                // reset retry count for UNLOCK command.
                fdoExtension->PrivateFdoData->MaxPowerOperationRetryCount = MAXIMUM_RETRIES;
                PowerContext->RetryCount = MAXIMUM_RETRIES;
            }

ClasspPowerUpCompletionFailure:

            TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tPreviously spun device up\n", Irp));

            if (PowerContext->QueueLocked) {
                TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tUnlocking queue\n", Irp));

                PowerContext->Srb.Function = SRB_FUNCTION_UNLOCK_QUEUE;
                PowerContext->Srb.SrbFlags = SRB_FLAGS_BYPASS_LOCKED_QUEUE;
                PowerContext->Srb.SrbStatus = PowerContext->Srb.ScsiStatus = 0;
                PowerContext->Srb.DataTransferLength = 0;

                nextStack->Parameters.Scsi.Srb = &(PowerContext->Srb);
                nextStack->MajorFunction = IRP_MJ_SCSI;

                PowerContext->PowerChangeState.PowerUp = PowerUpDeviceStarted;

                IoSetCompletionRoutine(fdoExtension->PrivateFdoData->PowerProcessIrp,
                                       ClasspPowerUpCompletion,
                                       PowerContext,
                                       TRUE,
                                       TRUE,
                                       TRUE);

                status = IoCallDriver(commonExtension->LowerDeviceObject, fdoExtension->PrivateFdoData->PowerProcessIrp);
                TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tIoCallDriver returned %lx\n",
                            fdoExtension->PrivateFdoData->PowerProcessIrp, status));
                break;
            }

            // Fall-through to next case...

        }

        case PowerUpDeviceUnlocked: {

            //
            // This is the end of the dance.  
            // We're ignoring possible intermediate error conditions ....
            //

            if (PowerContext->QueueLocked) {
                TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tPreviously unlocked queue\n", OriginalIrp));
                ASSERT(NT_SUCCESS(Irp->IoStatus.Status));
                ASSERT(PowerContext->Srb.SrbStatus == SRB_STATUS_SUCCESS);
            } else {
                TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tFall-through (queue not locked)\n", OriginalIrp));
            }

            TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tFreeing srb and completing\n", OriginalIrp));

            status = PowerContext->FinalStatus;
            OriginalIrp->IoStatus.Status = status;

            //
            // Set the new power state
            //

            if(NT_SUCCESS(status)) {
                fdoExtension->DevicePowerState = currentStack->Parameters.Power.State.DeviceState;
            }

            //
            // Indicate to Po that we've been successfully powered up so
            // it can do it's notification stuff.
            //

            PoSetPowerState(PowerContext->DeviceObject,
                            currentStack->Parameters.Power.Type,
                            currentStack->Parameters.Power.State);

            TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tStarting next power irp\n", OriginalIrp));
 
            ClassReleaseRemoveLock(PowerContext->DeviceObject, OriginalIrp);
            
            PowerContext->InUse = FALSE;

            PoStartNextPowerIrp(OriginalIrp);

            // prevent from completing the irp allocated by ourselves
            if (Irp == fdoExtension->PrivateFdoData->PowerProcessIrp) {
                // complete original irp if we are processing powerprocess irp,
                // otherwise, by returning status other than STATUS_MORE_PROCESSING_REQUIRED, IO manager will complete it.
                ClassCompleteRequest(commonExtension->DeviceObject, OriginalIrp, IO_NO_INCREMENT);
                status = STATUS_MORE_PROCESSING_REQUIRED;
            }

            return status;
        }
    }

    return STATUS_MORE_PROCESSING_REQUIRED;
} // end ClasspPowerUpCompletion()

/*++////////////////////////////////////////////////////////////////////////////

ClasspPowerDownCompletion()

Routine Description:

    This routine is used for intermediate completion of a power up request.
    PowerUp requires four requests to be sent to the lower driver in sequence.

        * The queue is "power locked" to ensure that the class driver power-up
          work can be done before request processing resumes.

        * The power irp is sent down the stack for any filter drivers and the
          port driver to return power and resume command processing for the
          device.  Since the queue is locked, no queued irps will be sent
          immediately.

        * A start unit command is issued to the device with appropriate flags
          to override the "power locked" queue.

        * The queue is "power unlocked" to start processing requests again.

    This routine uses the function in the srb which just completed to determine
    which state it is in.

Arguments:

    DeviceObject - the device object being powered up

    Irp - the IO_REQUEST_PACKET containing the power request

    Srb - the SRB used to perform port/class operations.

Return Value:

    STATUS_MORE_PROCESSING_REQUIRED or
    STATUS_SUCCESS

--*/
NTSTATUS
ClasspPowerDownCompletion(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    )
{
    PCLASS_POWER_CONTEXT PowerContext = (PCLASS_POWER_CONTEXT)Context;
    PFUNCTIONAL_DEVICE_EXTENSION fdoExtension = PowerContext->DeviceObject->DeviceExtension;
    PCOMMON_DEVICE_EXTENSION commonExtension = PowerContext->DeviceObject->DeviceExtension;
    PIRP OriginalIrp = PowerContext->Irp;

    // currentStack is for original power irp
    // nextStack is for power process irp
    PIO_STACK_LOCATION currentStack = IoGetCurrentIrpStackLocation(OriginalIrp);
    PIO_STACK_LOCATION nextStack = IoGetNextIrpStackLocation(fdoExtension->PrivateFdoData->PowerProcessIrp);

    NTSTATUS status = STATUS_MORE_PROCESSING_REQUIRED;

    UNREFERENCED_PARAMETER(DeviceObject);

    TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "ClasspPowerDownCompletion: Device Object %p, "
                   "Irp %p, Context %p\n",
                PowerContext->DeviceObject, Irp, Context));

    ASSERT(!TEST_FLAG(PowerContext->Srb.SrbFlags, SRB_FLAGS_FREE_SENSE_BUFFER));
    ASSERT(!TEST_FLAG(PowerContext->Srb.SrbFlags, SRB_FLAGS_PORT_DRIVER_ALLOCSENSE));
    ASSERT(PowerContext->Options.PowerDown == TRUE);
    ASSERT(PowerContext->Options.HandleSpinDown);

    if((Irp == OriginalIrp) && (Irp->PendingReturned)) {
        // only for original power irp
        IoMarkIrpPending(Irp);
    }

    PowerContext->PowerChangeState.PowerDown2++;

    switch(PowerContext->PowerChangeState.PowerDown2) {

        case PowerDownDeviceLocked2: {

            PCDB cdb;

            TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tPreviously sent power lock\n", Irp));

            if((PowerContext->Options.LockQueue == TRUE) &&
               (!NT_SUCCESS(Irp->IoStatus.Status))) {

                TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tIrp status was %lx\n",
                            Irp,
                            Irp->IoStatus.Status));
                TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tSrb status was %lx\n",
                            Irp,
                            PowerContext->Srb.SrbStatus));

                OriginalIrp->IoStatus.Status = STATUS_NOT_SUPPORTED;

                //
                // Lock was not successful - throw down the power IRP
                // by itself and don't try to spin down the drive or unlock
                // the queue.
                //

                //
                // Set the new power state
                //

                fdoExtension->DevicePowerState =
                    currentStack->Parameters.Power.State.DeviceState;

                //
                // Indicate to Po that we've been successfully powered down
                // so it can do it's notification stuff.
                //

                IoCopyCurrentIrpStackLocationToNext(OriginalIrp);
                IoSetCompletionRoutine(OriginalIrp,
                                       ClasspStartNextPowerIrpCompletion,
                                       PowerContext,
                                       TRUE,
                                       TRUE,
                                       TRUE);

                PoSetPowerState(PowerContext->DeviceObject,
                                currentStack->Parameters.Power.Type,
                                currentStack->Parameters.Power.State);

                fdoExtension->PowerDownInProgress = FALSE;

                PoCallDriver(commonExtension->LowerDeviceObject, OriginalIrp);

                ClassReleaseRemoveLock(commonExtension->DeviceObject,
                                       OriginalIrp);

                return STATUS_MORE_PROCESSING_REQUIRED;

            } else {
                PowerContext->QueueLocked = (UCHAR) PowerContext->Options.LockQueue;
            }

            if (!TEST_FLAG(fdoExtension->PrivateFdoData->HackFlags,
                           FDO_HACK_NO_SYNC_CACHE)) {

                //
                // send SCSIOP_SYNCHRONIZE_CACHE
                //
                
                fdoExtension->PrivateFdoData->MaxPowerOperationRetryCount = MAXIMUM_RETRIES;
                PowerContext->RetryCount = MAXIMUM_RETRIES;

                PowerContext->Srb.Length = sizeof(SCSI_REQUEST_BLOCK);
                PowerContext->Srb.Function = SRB_FUNCTION_EXECUTE_SCSI;

                PowerContext->Srb.TimeOutValue = fdoExtension->TimeOutValue;

                PowerContext->Srb.SrbFlags = SRB_FLAGS_NO_DATA_TRANSFER |
                                        SRB_FLAGS_DISABLE_AUTOSENSE |
                                        SRB_FLAGS_DISABLE_SYNCH_TRANSFER |
                                        SRB_FLAGS_NO_QUEUE_FREEZE |
                                        SRB_FLAGS_BYPASS_LOCKED_QUEUE;

                PowerContext->Srb.SrbStatus = PowerContext->Srb.ScsiStatus = 0;
                PowerContext->Srb.DataTransferLength = 0;

                PowerContext->Srb.CdbLength = 10;

                cdb = (PCDB) PowerContext->Srb.Cdb;

                RtlZeroMemory(cdb, sizeof(CDB));
                cdb->SYNCHRONIZE_CACHE10.OperationCode = SCSIOP_SYNCHRONIZE_CACHE;

                IoSetCompletionRoutine(fdoExtension->PrivateFdoData->PowerProcessIrp,
                                       ClasspPowerDownCompletion,
                                       PowerContext,
                                       TRUE,
                                       TRUE,
                                       TRUE);

                nextStack->Parameters.Scsi.Srb = &(PowerContext->Srb);
                nextStack->MajorFunction = IRP_MJ_SCSI;

                status = IoCallDriver(commonExtension->LowerDeviceObject, fdoExtension->PrivateFdoData->PowerProcessIrp);

                TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tIoCallDriver returned %lx\n", fdoExtension->PrivateFdoData->PowerProcessIrp, status));
                break;

            } else {

                TracePrint((TRACE_LEVEL_WARNING, TRACE_FLAG_POWER, "(%p)\tPower Down: not sending SYNCH_CACHE\n",
                            PowerContext->DeviceObject));
                PowerContext->PowerChangeState.PowerDown2++;
                PowerContext->Srb.SrbStatus = SRB_STATUS_SUCCESS;
                // and fall through....
            }
            // no break in case the device doesn't like synch_cache commands

        }

        case PowerDownDeviceFlushed2: {

            PCDB cdb;

            TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tPreviously send SCSIOP_SYNCHRONIZE_CACHE\n",
                        Irp));

            //
            // SCSIOP_SYNCHRONIZE_CACHE was sent
            //

            if(SRB_STATUS(PowerContext->Srb.SrbStatus) != SRB_STATUS_SUCCESS) {

                BOOLEAN retry;
                LONGLONG delta100nsUnits = 0;

                TracePrint((TRACE_LEVEL_ERROR, TRACE_FLAG_POWER, "(%p)\tError occured when issuing "
                            "SYNCHRONIZE_CACHE command to device. "
                            "Srb %p, Status %lx\n",
                            Irp,
                            &PowerContext->Srb,
                            PowerContext->Srb.SrbStatus));

                ASSERT(!(TEST_FLAG(PowerContext->Srb.SrbStatus,
                                   SRB_STATUS_QUEUE_FROZEN)));
                ASSERT(PowerContext->Srb.Function == SRB_FUNCTION_EXECUTE_SCSI);

                PowerContext->RetryInterval = 0;
                retry = InterpretSenseInfoWithoutHistory(
                            fdoExtension->DeviceObject,
                            Irp,
                            &PowerContext->Srb,
                            IRP_MJ_SCSI,
                            IRP_MJ_POWER,
                            fdoExtension->PrivateFdoData->MaxPowerOperationRetryCount - PowerContext->RetryCount,
                            &status,
                            &delta100nsUnits);

                // NOTE: Power context is a public structure, and thus cannot be
                //       updated to use 100ns units.  Therefore, must store the
                //       one-second equivalent.  Round up to ensure minimum delay
                //       requirements have been met.
                delta100nsUnits += (10*1000*1000) - 1;
                delta100nsUnits /= (10*1000*1000);
                // guaranteed not to have high bits set per SAL annotations
                PowerContext->RetryInterval = (ULONG)(delta100nsUnits);


                if((retry == TRUE) && (PowerContext->RetryCount-- != 0)) {

                        TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tRetrying failed request\n", Irp));

                        //
                        // decrement the state so we come back through here
                        // the next time.
                        //

                        PowerContext->PowerChangeState.PowerDown2--;
                        RetryPowerRequest(commonExtension->DeviceObject,
                                          Irp,
                                          PowerContext);
                        break;
                }

                TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tSYNCHRONIZE_CACHE not retried\n", Irp));
                fdoExtension->PrivateFdoData->MaxPowerOperationRetryCount = MAXIMUM_RETRIES;
                PowerContext->RetryCount = MAXIMUM_RETRIES;
            } // end !SRB_STATUS_SUCCESS

            //
            // note: we are purposefully ignoring any errors.  if the drive
            //       doesn't support a synch_cache, then we're up a creek
            //       anyways.
            //

            if ((currentStack->Parameters.Power.State.DeviceState == PowerDeviceD3) &&
                (currentStack->Parameters.Power.ShutdownType == PowerActionHibernate) &&
                (commonExtension->HibernationPathCount != 0)) {

                TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tPower Down: not sending SPIN DOWN due to hibernation path\n",
                            PowerContext->DeviceObject));

                PowerContext->PowerChangeState.PowerDown2++;
                PowerContext->Srb.SrbStatus = SRB_STATUS_SUCCESS;
                status = STATUS_SUCCESS;

                // Fall through to next case...

            } else {
                // Send STOP UNIT command. As "Imme" bit is set to '1', this command should be completed in short time.
                // This command is at low importance, failure of this command has very small impact.

                ULONG secondsRemaining;
                ULONG timeoutValue;

                TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tSending stop unit to device\n", Irp));

                if(PoQueryWatchdogTime(fdoExtension->LowerPdo, &secondsRemaining)) {
                    // plan to leave some time (TIME_LEFT_FOR_LOWER_DRIVERS) to lower level drivers 
                    // for processing the original power irp.
                    if (secondsRemaining >= (TIME_LEFT_FOR_LOWER_DRIVERS + DEFAULT_IO_TIMEOUT_VALUE)) {
                        fdoExtension->PrivateFdoData->MaxPowerOperationRetryCount =
                            (secondsRemaining - TIME_LEFT_FOR_LOWER_DRIVERS) / DEFAULT_IO_TIMEOUT_VALUE;
                        
                        // * No 'short' timeouts
			//
                        // timeoutValue = (secondsRemaining - TIME_LEFT_FOR_LOWER_DRIVERS) %
                        //                DEFAULT_IO_TIMEOUT_VALUE;

                        // if (timeoutValue < MINIMUM_STOP_UNIT_TIMEOUT_VALUE)
                        // {
                            if (--fdoExtension->PrivateFdoData->MaxPowerOperationRetryCount)
                            {
                                timeoutValue = DEFAULT_IO_TIMEOUT_VALUE;
                            } else {
                                timeoutValue = secondsRemaining - TIME_LEFT_FOR_LOWER_DRIVERS;
                            }
                        // }

                        // Limit to maximum retry count.
                        if (fdoExtension->PrivateFdoData->MaxPowerOperationRetryCount > MAXIMUM_RETRIES) {
                            fdoExtension->PrivateFdoData->MaxPowerOperationRetryCount = MAXIMUM_RETRIES;
                        }
                    } else {
                        // issue the command with minimum timeout value and do not retry on it.
                        fdoExtension->PrivateFdoData->MaxPowerOperationRetryCount = 0;

                        // minimum as MINIMUM_STOP_UNIT_TIMEOUT_VALUE.
                        if (secondsRemaining > 2 * MINIMUM_STOP_UNIT_TIMEOUT_VALUE) {
                            timeoutValue = secondsRemaining - MINIMUM_STOP_UNIT_TIMEOUT_VALUE; 
                        } else {
                            timeoutValue = MINIMUM_STOP_UNIT_TIMEOUT_VALUE;
                        }

                    }

                } else {
                    // do not know how long, use default values.
                    fdoExtension->PrivateFdoData->MaxPowerOperationRetryCount = MAXIMUM_RETRIES;
                    timeoutValue = DEFAULT_IO_TIMEOUT_VALUE;
                }

                //
                // Issue STOP UNIT command to the device.
                //

                PowerContext->RetryCount = fdoExtension->PrivateFdoData->MaxPowerOperationRetryCount;
                PowerContext->Srb.TimeOutValue = timeoutValue;

                PowerContext->Srb.Length = sizeof(SCSI_REQUEST_BLOCK);
                PowerContext->Srb.Function = SRB_FUNCTION_EXECUTE_SCSI;

                PowerContext->Srb.SrbFlags = SRB_FLAGS_NO_DATA_TRANSFER |
                                        SRB_FLAGS_DISABLE_AUTOSENSE |
                                        SRB_FLAGS_DISABLE_SYNCH_TRANSFER |
                                        SRB_FLAGS_NO_QUEUE_FREEZE |
                                        SRB_FLAGS_BYPASS_LOCKED_QUEUE;

                PowerContext->Srb.SrbStatus = PowerContext->Srb.ScsiStatus = 0;
                PowerContext->Srb.DataTransferLength = 0;

                PowerContext->Srb.CdbLength = 6;

                cdb = (PCDB) PowerContext->Srb.Cdb;
                RtlZeroMemory(cdb, sizeof(CDB));

                cdb->START_STOP.OperationCode = SCSIOP_START_STOP_UNIT;
                cdb->START_STOP.Start = 0;
                cdb->START_STOP.Immediate = 1;

                IoSetCompletionRoutine(fdoExtension->PrivateFdoData->PowerProcessIrp,
                                       ClasspPowerDownCompletion,
                                       PowerContext,
                                       TRUE,
                                       TRUE,
                                       TRUE);

                nextStack->Parameters.Scsi.Srb = &(PowerContext->Srb);
                nextStack->MajorFunction = IRP_MJ_SCSI;

                status = IoCallDriver(commonExtension->LowerDeviceObject, fdoExtension->PrivateFdoData->PowerProcessIrp);

                TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tIoCallDriver returned %lx\n", fdoExtension->PrivateFdoData->PowerProcessIrp, status));
                break;
            }
        }

        case PowerDownDeviceStopped2: {

            BOOLEAN ignoreError = TRUE;

            //
            // stop was sent
            //

            if(SRB_STATUS(PowerContext->Srb.SrbStatus) != SRB_STATUS_SUCCESS) {

                BOOLEAN retry;
                LONGLONG delta100nsUnits = 0;

                TracePrint((TRACE_LEVEL_ERROR, TRACE_FLAG_POWER, "(%p)\tError occured when issueing STOP_UNIT "
                            "command to device. Srb %p, Status %lx\n",
                            Irp,
                            &PowerContext->Srb,
                            PowerContext->Srb.SrbStatus));

                ASSERT(!(TEST_FLAG(PowerContext->Srb.SrbStatus,
                                   SRB_STATUS_QUEUE_FROZEN)));
                ASSERT(PowerContext->Srb.Function == SRB_FUNCTION_EXECUTE_SCSI);

                PowerContext->RetryInterval = 0;
                retry = InterpretSenseInfoWithoutHistory(
                            fdoExtension->DeviceObject,
                            Irp,
                            &PowerContext->Srb,
                            IRP_MJ_SCSI,
                            IRP_MJ_POWER,
                            fdoExtension->PrivateFdoData->MaxPowerOperationRetryCount - PowerContext->RetryCount,
                            &status,
                            &delta100nsUnits);

                // NOTE: Power context is a public structure, and thus cannot be
                //       updated to use 100ns units.  Therefore, must store the
                //       one-second equivalent.  Round up to ensure minimum delay
                //       requirements have been met.
                delta100nsUnits += (10*1000*1000) - 1;
                delta100nsUnits /= (10*1000*1000);
                // guaranteed not to have high bits set per SAL annotations
                PowerContext->RetryInterval = (ULONG)(delta100nsUnits);


                if((retry == TRUE) && (PowerContext->RetryCount-- != 0)) {

                        TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tRetrying failed request\n", Irp));

                        //
                        // decrement the state so we come back through here
                        // the next time.
                        //

                        PowerContext->PowerChangeState.PowerDown2--;

                        PowerContext->Srb.TimeOutValue = DEFAULT_IO_TIMEOUT_VALUE;

                        RetryPowerRequest(commonExtension->DeviceObject,
                                          Irp,
                                          PowerContext);
                        break;
                }

                TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tSTOP_UNIT not retried\n", Irp));
                fdoExtension->PrivateFdoData->MaxPowerOperationRetryCount = MAXIMUM_RETRIES;
                PowerContext->RetryCount = MAXIMUM_RETRIES;

            } // end !SRB_STATUS_SUCCESS


            TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tPreviously sent stop unit\n", Irp));

            //
            // some operations, such as a physical format in progress,
            // should not be ignored and should fail the power operation.
            //

            if (!NT_SUCCESS(status)) {

                PSENSE_DATA senseBuffer = PowerContext->Srb.SenseInfoBuffer;

                if (TEST_FLAG(PowerContext->Srb.SrbStatus,
                              SRB_STATUS_AUTOSENSE_VALID) &&
                    ((senseBuffer->SenseKey & 0xf) == SCSI_SENSE_NOT_READY) &&
                    (senseBuffer->AdditionalSenseCode == SCSI_ADSENSE_LUN_NOT_READY) &&
                    (senseBuffer->AdditionalSenseCodeQualifier == SCSI_SENSEQ_FORMAT_IN_PROGRESS)
                    ) {
                    ignoreError = FALSE;
                    PowerContext->FinalStatus = STATUS_DEVICE_BUSY;
                    status = PowerContext->FinalStatus;
                }

            }

            if (NT_SUCCESS(status) || ignoreError) {

                //
                // Issue the original power request to the lower driver.
                //

                OriginalIrp->IoStatus.Status = STATUS_NOT_SUPPORTED;

                IoCopyCurrentIrpStackLocationToNext(OriginalIrp);

                IoSetCompletionRoutine(OriginalIrp,
                                       ClasspPowerDownCompletion,
                                       PowerContext,
                                       TRUE,
                                       TRUE,
                                       TRUE);

                status = PoCallDriver(commonExtension->LowerDeviceObject, OriginalIrp);

                TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tPoCallDriver returned %lx\n", OriginalIrp, status));
                break;
            }

            // else fall through w/o sending the power irp, since the device
            // is reporting an error that would be "really bad" to power down
            // during.

        }

        case PowerDownDeviceOff2: {

            //
            // SpinDown request completed ... whether it succeeded or not is
            // another matter entirely.
            //

            TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tPreviously sent power irp\n", OriginalIrp));

            if (PowerContext->QueueLocked) {

                TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tUnlocking queue\n", OriginalIrp));

                PowerContext->Srb.Length = sizeof(SCSI_REQUEST_BLOCK);

                PowerContext->Srb.SrbStatus = PowerContext->Srb.ScsiStatus = 0;
                PowerContext->Srb.DataTransferLength = 0;

                PowerContext->Srb.Function = SRB_FUNCTION_UNLOCK_QUEUE;
                PowerContext->Srb.SrbFlags = SRB_FLAGS_BYPASS_LOCKED_QUEUE;
                nextStack->Parameters.Scsi.Srb = &(PowerContext->Srb);
                nextStack->MajorFunction = IRP_MJ_SCSI;

                IoSetCompletionRoutine(fdoExtension->PrivateFdoData->PowerProcessIrp,
                                       ClasspPowerDownCompletion,
                                       PowerContext,
                                       TRUE,
                                       TRUE,
                                       TRUE);

                status = IoCallDriver(commonExtension->LowerDeviceObject, fdoExtension->PrivateFdoData->PowerProcessIrp);
                TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tIoCallDriver returned %lx\n",
                            fdoExtension->PrivateFdoData->PowerProcessIrp,
                            status));
                break;
            }

        }

        case PowerDownDeviceUnlocked2: {

            //
            // This is the end of the dance. 
            // We're ignoring possible intermediate error conditions ....
            //

            if (PowerContext->QueueLocked == FALSE) {
                TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tFall through (queue not locked)\n", OriginalIrp));
            } else {
                TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tPreviously unlocked queue\n", OriginalIrp));
                ASSERT(NT_SUCCESS(Irp->IoStatus.Status));
                ASSERT(PowerContext->Srb.SrbStatus == SRB_STATUS_SUCCESS);

                if (NT_SUCCESS(Irp->IoStatus.Status)) {
                    PowerContext->QueueLocked = FALSE;
                }
            }

            TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tFreeing srb and completing\n", OriginalIrp));
            status = PowerContext->FinalStatus; // allow failure to propogate

            OriginalIrp->IoStatus.Status = status;
            OriginalIrp->IoStatus.Information = 0;

            if (NT_SUCCESS(status)) {

                //
                // Set the new power state
                //

                fdoExtension->DevicePowerState =
                    currentStack->Parameters.Power.State.DeviceState;

            }


            TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tStarting next power irp\n", OriginalIrp));

            ClassReleaseRemoveLock(PowerContext->DeviceObject, OriginalIrp);
            
            PowerContext->InUse = FALSE;

            PoStartNextPowerIrp(OriginalIrp);

            fdoExtension->PowerDownInProgress = FALSE;
 
            // prevent from completing the irp allocated by ourselves
            if (Irp == fdoExtension->PrivateFdoData->PowerProcessIrp) {
                // complete original irp if we are processing powerprocess irp,
                // otherwise, by returning status other than STATUS_MORE_PROCESSING_REQUIRED, IO manager will complete it.
                ClassCompleteRequest(commonExtension->DeviceObject, OriginalIrp, IO_NO_INCREMENT);
                status = STATUS_MORE_PROCESSING_REQUIRED;
            }

            return status;
        }
    }

    return STATUS_MORE_PROCESSING_REQUIRED;
} // end ClasspPowerDownCompletion()

/*++////////////////////////////////////////////////////////////////////////////

ClasspPowerHandler()

Routine Description:

    This routine reduces the number of useless spinups and spindown requests
    sent to a given device by ignoring transitions to power states we are
    currently in.

    ISSUE-2000/02/20-henrygab - by ignoring spin-up requests, we may be
          allowing the drive

Arguments:

    DeviceObject - the device object which is transitioning power states
    Irp - the power irp
    Options - a set of flags indicating what the device handles

Return Value:

--*/
NTSTATUS
ClasspPowerHandler(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN CLASS_POWER_OPTIONS Options  // ISSUE-2000/02/20-henrygab - pass pointer, not whole struct
    )
{
    PCOMMON_DEVICE_EXTENSION commonExtension = DeviceObject->DeviceExtension;
    PDEVICE_OBJECT lowerDevice = commonExtension->LowerDeviceObject;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    PIO_STACK_LOCATION nextIrpStack;
    PFUNCTIONAL_DEVICE_EXTENSION fdoExtension = DeviceObject->DeviceExtension;
    PCLASS_POWER_CONTEXT context;

    if (!commonExtension->IsFdo) {

        //
        // certain assumptions are made here,
        // particularly: having the fdoExtension
        //

        TracePrint((TRACE_LEVEL_ERROR, TRACE_FLAG_POWER, "ClasspPowerHandler: Called for PDO %p???\n",
                    DeviceObject));
        ASSERT(!"PDO using ClasspPowerHandler");

        ClassReleaseRemoveLock(DeviceObject, Irp);
        Irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
        PoStartNextPowerIrp(Irp);
        ClassCompleteRequest(DeviceObject, Irp, IO_NO_INCREMENT);
        return STATUS_NOT_SUPPORTED;
    }

    TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "ClasspPowerHandler: Power irp %p to %s %p\n",
                Irp, (commonExtension->IsFdo ? "fdo" : "pdo"), DeviceObject));

    switch(irpStack->MinorFunction) {

        case IRP_MN_SET_POWER: {
            PCLASS_PRIVATE_FDO_DATA fdoData = fdoExtension->PrivateFdoData;

            TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tIRP_MN_SET_POWER\n", Irp));

            TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tSetting %s state to %d\n",
                        Irp,
                        (irpStack->Parameters.Power.Type == SystemPowerState ?
                            "System" : "Device"),
                        irpStack->Parameters.Power.State.SystemState));

                switch (irpStack->Parameters.Power.ShutdownType){

                    case PowerActionSleep:
                    case PowerActionHibernate:
                        if (fdoData->HotplugInfo.MediaRemovable || fdoData->HotplugInfo.MediaHotplug){
                            /*
                             *  We are suspending and this drive is either hot-pluggable
                             *  or contains removeable media.
                             *  Set the media dirty bit, since the media may change while
                             *  we are suspended.
                             */
                            SET_FLAG(DeviceObject->Flags, DO_VERIFY_VOLUME);

                            //
                            // Bumping the media  change count  will force the
                            // file system to verify the volume when we resume
                            //

                            InterlockedIncrement(&fdoExtension->MediaChangeCount);
                        }
                        break;
                }

            break;
        }

        default: {

            TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tIrp minor code = %#x\n",
                        Irp, irpStack->MinorFunction));
            break;
        }
    }

    if (irpStack->Parameters.Power.Type != DevicePowerState ||
        irpStack->MinorFunction != IRP_MN_SET_POWER) {

        TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tSending to lower device\n", Irp));

        goto ClasspPowerHandlerCleanup;

    }

    //
    // already in exact same state, don't work to transition to it.
    //

    if(irpStack->Parameters.Power.State.DeviceState ==
       fdoExtension->DevicePowerState) {

        TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tAlready in device state %x\n",
                    Irp, fdoExtension->DevicePowerState));
        goto ClasspPowerHandlerCleanup;

    }

    //
    // or powering down from non-d0 state (device already stopped)
    // NOTE -- we're not sure whether this case can exist or not (the
    // power system may never send this sort of request) but it's trivial
    // to deal with.
    //

    if ((irpStack->Parameters.Power.State.DeviceState != PowerDeviceD0) &&
        (fdoExtension->DevicePowerState != PowerDeviceD0)) {
        TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tAlready powered down to %x???\n",
                    Irp, fdoExtension->DevicePowerState));
        fdoExtension->DevicePowerState =
            irpStack->Parameters.Power.State.DeviceState;
        goto ClasspPowerHandlerCleanup;
    }

    //
    // or when not handling powering up and are powering up
    //

    if ((!Options.HandleSpinUp) &&
        (irpStack->Parameters.Power.State.DeviceState == PowerDeviceD0)) {

        TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tNot handling spinup to state %x\n",
                    Irp, fdoExtension->DevicePowerState));
        fdoExtension->DevicePowerState =
            irpStack->Parameters.Power.State.DeviceState;
        goto ClasspPowerHandlerCleanup;

    }

    //
    // or when not handling powering down and are powering down
    //

    if ((!Options.HandleSpinDown) &&
        (irpStack->Parameters.Power.State.DeviceState != PowerDeviceD0)) {

        TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tNot handling spindown to state %x\n",
                    Irp, fdoExtension->DevicePowerState));
        fdoExtension->DevicePowerState =
            irpStack->Parameters.Power.State.DeviceState;
        goto ClasspPowerHandlerCleanup;

    }

    // 
    // validation completed, start the real work.
    //

    IoReuseIrp(fdoExtension->PrivateFdoData->PowerProcessIrp, STATUS_SUCCESS);
    IoSetNextIrpStackLocation(fdoExtension->PrivateFdoData->PowerProcessIrp);
    nextIrpStack = IoGetNextIrpStackLocation(fdoExtension->PrivateFdoData->PowerProcessIrp);

    context = &(fdoExtension->PowerContext);

    ASSERT(context->InUse == FALSE);

    RtlZeroMemory(context, sizeof(CLASS_POWER_CONTEXT));
    context->InUse = TRUE;

    nextIrpStack->Parameters.Scsi.Srb = &(context->Srb);
    nextIrpStack->MajorFunction = IRP_MJ_SCSI;

    context->FinalStatus = STATUS_SUCCESS;

    context->Srb.Length = sizeof(SCSI_REQUEST_BLOCK);
    context->Srb.OriginalRequest = fdoExtension->PrivateFdoData->PowerProcessIrp;
    context->Srb.SrbFlags |= SRB_FLAGS_BYPASS_LOCKED_QUEUE | SRB_FLAGS_NO_QUEUE_FREEZE;
    context->Srb.Function = SRB_FUNCTION_LOCK_QUEUE;

    context->Srb.SenseInfoBuffer = commonExtension->PartitionZeroExtension->SenseData;
    context->Srb.SenseInfoBufferLength = SENSE_BUFFER_SIZE;
    fdoExtension->PrivateFdoData->MaxPowerOperationRetryCount = MAXIMUM_RETRIES;
    context->RetryCount = MAXIMUM_RETRIES;

    context->Options = Options;
    context->DeviceObject = DeviceObject;
    context->Irp = Irp;

    if(irpStack->Parameters.Power.State.DeviceState == PowerDeviceD0) {

        ASSERT(Options.HandleSpinUp);

        TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tpower up - locking queue\n", Irp));

        //
        // We need to issue a queue lock request so that we
        // can spin the drive back up after the power is restored
        // but before any requests are processed.
        //

        context->Options.PowerDown = FALSE;
        context->PowerChangeState.PowerUp = PowerUpDeviceInitial;
        context->CompletionRoutine = ClasspPowerUpCompletion;

    } else {

        ASSERT(Options.HandleSpinDown);

        fdoExtension->PowerDownInProgress = TRUE;

        TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tPowering down - locking queue\n", Irp));

        PoSetPowerState(DeviceObject,
                        irpStack->Parameters.Power.Type,
                        irpStack->Parameters.Power.State);

        context->Options.PowerDown = TRUE;
        context->PowerChangeState.PowerDown2 = PowerDownDeviceInitial2;
        context->CompletionRoutine = ClasspPowerDownCompletion;

    }

    //
    // we are not dealing with port-allocated sense in these routines.
    //

    ASSERT(!TEST_FLAG(context->Srb.SrbFlags, SRB_FLAGS_FREE_SENSE_BUFFER));
    ASSERT(!TEST_FLAG(context->Srb.SrbFlags, SRB_FLAGS_PORT_DRIVER_ALLOCSENSE));

    //
    // Mark the original power irp pending.
    //

    IoMarkIrpPending(Irp);

    if(Options.LockQueue) {

        //
        // Send the lock irp down.
        //

        IoSetCompletionRoutine(fdoExtension->PrivateFdoData->PowerProcessIrp,
                               context->CompletionRoutine,
                               context,
                               TRUE,
                               TRUE,
                               TRUE);

        IoCallDriver(lowerDevice, fdoExtension->PrivateFdoData->PowerProcessIrp);

    } else {

        //
        // Call the completion routine directly.  It won't care what the
        // status of the "lock" was - it will just go and do the next
        // step of the operation.
        //

        context->CompletionRoutine(DeviceObject, fdoExtension->PrivateFdoData->PowerProcessIrp, context);
    }

    return STATUS_PENDING;

ClasspPowerHandlerCleanup:

    //
    // Send the original power irp down, we will start the next power irp in completion routine.
    //
    ClassReleaseRemoveLock(DeviceObject, Irp);

    TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tStarting next power irp\n", Irp));
    IoCopyCurrentIrpStackLocationToNext(Irp);
    IoSetCompletionRoutine(Irp,
                           ClasspStartNextPowerIrpCompletion,
                           NULL,
                           TRUE,
                           TRUE,
                           TRUE);
    return PoCallDriver(lowerDevice, Irp);
} // end ClasspPowerHandler()

/*++////////////////////////////////////////////////////////////////////////////

ClassMinimalPowerHandler()

Routine Description:

    This routine is the minimum power handler for a storage driver.  It does
    the least amount of work possible.

--*/
NTSTATUS
ClassMinimalPowerHandler(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{
    PCOMMON_DEVICE_EXTENSION commonExtension = DeviceObject->DeviceExtension;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    NTSTATUS status;

    ClassReleaseRemoveLock(DeviceObject, Irp);
    PoStartNextPowerIrp(Irp);

    switch (irpStack->MinorFunction)
    {
        case IRP_MN_SET_POWER:
        {
            switch (irpStack->Parameters.Power.ShutdownType)
            {
                case PowerActionSleep:
                case PowerActionHibernate:
                {
                    if (TEST_FLAG(DeviceObject->Characteristics, FILE_REMOVABLE_MEDIA))
                    {
                        if ((ClassGetVpb(DeviceObject) != NULL) && (ClassGetVpb(DeviceObject)->Flags & VPB_MOUNTED))
                        {
                            //
                            // This flag will cause the filesystem to verify the
                            // volume when  coming out of hibernation or standby
                            //
                            SET_FLAG(DeviceObject->Flags, DO_VERIFY_VOLUME);
                        }
                    }
                }
                break;
            }
        }

        //
        // Fall through
        //

        case IRP_MN_QUERY_POWER:
        {
            if (!commonExtension->IsFdo)
            {
                Irp->IoStatus.Status = STATUS_SUCCESS;
                Irp->IoStatus.Information = 0;
            }
        }
        break;
    }

    if (commonExtension->IsFdo)
    {
        IoCopyCurrentIrpStackLocationToNext(Irp);
        status = PoCallDriver(commonExtension->LowerDeviceObject, Irp);
    }
    else
    {
        status = Irp->IoStatus.Status;
        ClassCompleteRequest(DeviceObject, Irp, IO_NO_INCREMENT);
    }

    return status;
} // end ClassMinimalPowerHandler()

/*++////////////////////////////////////////////////////////////////////////////

ClassSpinDownPowerHandler()

Routine Description:

    This routine is a callback for disks and other things which require both
    a start and a stop to be sent to the device.  (actually the starts are
    almost always optional, since most device power themselves on to process
    commands, but i digress).

    Determines proper use of spinup, spindown, and queue locking based upon
    ScanForSpecialFlags in the FdoExtension.  This is the most common power
    handler passed into classpnp.sys

Arguments:

    DeviceObject - Supplies the functional device object

    Irp - Supplies the request to be retried.

Return Value:

    None

--*/
__control_entrypoint(DeviceDriver)
NTSTATUS
ClassSpinDownPowerHandler(
    __in PDEVICE_OBJECT DeviceObject,
    __in PIRP Irp
    )
{
    PFUNCTIONAL_DEVICE_EXTENSION fdoExtension;
    CLASS_POWER_OPTIONS options = {0};

    fdoExtension = (PFUNCTIONAL_DEVICE_EXTENSION)DeviceObject->DeviceExtension;

    //
    // check the flags to see what options we need to worry about
    //

    if (!TEST_FLAG(fdoExtension->ScanForSpecialFlags,
                  CLASS_SPECIAL_DISABLE_SPIN_DOWN)) {
        options.HandleSpinDown = TRUE;
    }

    if (!TEST_FLAG(fdoExtension->ScanForSpecialFlags,
                  CLASS_SPECIAL_DISABLE_SPIN_UP)) {
        options.HandleSpinUp = TRUE;
    }

    if (!TEST_FLAG(fdoExtension->ScanForSpecialFlags,
                  CLASS_SPECIAL_NO_QUEUE_LOCK)) {
        options.LockQueue = TRUE;
    }

    TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "ClasspPowerHandler: Devobj %p\n"
                "\t%shandling spin down\n"
                "\t%shandling spin up\n"
                "\t%slocking queue\n",
                DeviceObject,
                (options.HandleSpinDown ? "" : "not "),
                (options.HandleSpinUp   ? "" : "not "),
                (options.LockQueue      ? "" : "not ")
                ));

    //
    // do all the dirty work
    //

    return ClasspPowerHandler(DeviceObject, Irp, options);
} // end ClassSpinDownPowerHandler()

/*++////////////////////////////////////////////////////////////////////////////

ClassStopUnitPowerHandler()

Routine Description:

    This routine is an outdated call.  To achieve equivalent functionality,
    the driver should set the following flags in ScanForSpecialFlags in the
    FdoExtension:

        CLASS_SPECIAL_DISABLE_SPIN_UP
        CLASS_SPECIAL_NO_QUEUE_LOCK

--*/
NTSTATUS
ClassStopUnitPowerHandler(
    __in PDEVICE_OBJECT DeviceObject,
    __in PIRP Irp
    )
{
    PFUNCTIONAL_DEVICE_EXTENSION fdoExtension;

    TracePrint((TRACE_LEVEL_ERROR, TRACE_FLAG_POWER, "ClassStopUnitPowerHandler - Devobj %p using outdated call\n"
                "Drivers should set the following flags in ScanForSpecialFlags "
                " in the FDO extension:\n"
                "\tCLASS_SPECIAL_DISABLE_SPIN_UP\n"
                "\tCLASS_SPECIAL_NO_QUEUE_LOCK\n"
                "This will provide equivalent functionality if the power "
                "routine is then set to ClassSpinDownPowerHandler\n\n",
                DeviceObject));

    fdoExtension = (PFUNCTIONAL_DEVICE_EXTENSION)DeviceObject->DeviceExtension;

    SET_FLAG(fdoExtension->ScanForSpecialFlags,
             CLASS_SPECIAL_DISABLE_SPIN_UP);
    SET_FLAG(fdoExtension->ScanForSpecialFlags,
             CLASS_SPECIAL_NO_QUEUE_LOCK);

    return ClassSpinDownPowerHandler(DeviceObject, Irp);
} // end ClassStopUnitPowerHandler()

/*++////////////////////////////////////////////////////////////////////////////

RetryPowerRequest()

Routine Description:

    This routine reinitalizes the necessary fields, and sends the request
    to the lower driver.

Arguments:

    DeviceObject - Supplies the device object associated with this request.

    Irp - Supplies the request to be retried.

    Context - Supplies a pointer to the power up context for this request.

Return Value:

    None

--*/
VOID
RetryPowerRequest(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp,
    PCLASS_POWER_CONTEXT Context
    )
{
    PIO_STACK_LOCATION nextIrpStack = IoGetNextIrpStackLocation(Irp);
    PSCSI_REQUEST_BLOCK srb = &(Context->Srb);
    LONGLONG dueTime;

    TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tDelaying retry by queueing DPC\n", Irp));

    //ASSERT(Context->Irp == Irp);
    ASSERT(Context->DeviceObject == DeviceObject);
    ASSERT(!TEST_FLAG(Context->Srb.SrbFlags, SRB_FLAGS_FREE_SENSE_BUFFER));
    ASSERT(!TEST_FLAG(Context->Srb.SrbFlags, SRB_FLAGS_PORT_DRIVER_ALLOCSENSE));

    if (Context->RetryInterval == 0) {

        TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tDelaying minimum time (.2 sec)\n", Irp));
        dueTime = (LONGLONG)1000000 * 2;

    } else {

        TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_POWER, "(%p)\tDelaying %x seconds\n",
                    Irp, Context->RetryInterval));
        dueTime = (LONGLONG)1000000 * 10 * Context->RetryInterval;

    }

    //
    // reset the retry interval
    //

    Context->RetryInterval = 0;

    //
    // Reset byte count of transfer in SRB Extension.
    //

    srb->DataTransferLength = 0;

    //
    // Zero SRB statuses.
    //

    srb->SrbStatus = srb->ScsiStatus = 0;

    //
    // Set up major SCSI function.
    //

    nextIrpStack->MajorFunction = IRP_MJ_SCSI;

    //
    // Save SRB address in next stack for port driver.
    //

    nextIrpStack->Parameters.Scsi.Srb = srb;

    //
    // Set the completion routine up again.
    //

    IoSetCompletionRoutine(Irp, Context->CompletionRoutine, Context,
                           TRUE, TRUE, TRUE);

    ClassRetryRequest(DeviceObject, Irp, dueTime);

    return;

} // end RetryRequest()

/*++////////////////////////////////////////////////////////////////////////////

ClasspStartNextPowerIrpCompletion()

Routine Description:

    This routine guarantees that the next power irp (power up or down) is not
    sent until the previous one has fully completed.

--*/
NTSTATUS
ClasspStartNextPowerIrpCompletion(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    )
{
    PCLASS_POWER_CONTEXT PowerContext = (PCLASS_POWER_CONTEXT)Context;

    if(Irp->PendingReturned) {
        IoMarkIrpPending(Irp);
    }

    if (PowerContext != NULL)
    {
        PowerContext->InUse = FALSE;
    }

    PoStartNextPowerIrp(Irp);
    return STATUS_SUCCESS;
} // end ClasspStartNextPowerIrpCompletion()

