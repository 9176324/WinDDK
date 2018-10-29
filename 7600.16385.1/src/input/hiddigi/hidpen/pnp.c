/*++
    Copyright (c) Microsoft Corporation

    Module Name:
        pnp.c

    Abstract:
        This module contains code to handle PnP and Power IRPs.

    Environment:
        Kernel mode

--*/

#include "pch.h"

#define MODULE_ID                       1

#ifdef ALLOC_PRAGMA
  #pragma alloc_text(PAGE, HpenPnp)
  #pragma alloc_text(PAGE, HpenPower)
  #pragma alloc_text(PAGE, SendSyncIrp)
#endif

/*++
    @doc    EXTERNAL

    @func   NTSTATUS | HpenPnp |
            Plug and Play dispatch routine for this driver.

    @parm   IN PDEVICE_OBJECT | DevObj | Pointer to the device object.
    @parm   IN PIRP | Irp | Pointer to an I/O request packet.

    @rvalue SUCCESS | returns STATUS_SUCCESS.
    @rvalue FAILURE | returns NT status code.
--*/

NTSTATUS EXTERNAL
HpenPnp(
    IN PDEVICE_OBJECT DevObj,
    IN PIRP Irp
    )
{
    NTSTATUS status;
    PDEVICE_EXTENSION devext = GET_MINIDRIVER_DEVICE_EXTENSION(DevObj);
    PIO_STACK_LOCATION irpsp = IoGetCurrentIrpStackLocation(Irp);

    PAGED_CODE();
    TEnter(API, ("(DevObj=%p,Irp=%p,IrpSp=%p,Minor=%!PNPMN!)",
                 DevObj, Irp, irpsp, irpsp->MinorFunction));

    status = IoAcquireRemoveLock(&devext->RemoveLock, Irp);
    if (!NT_SUCCESS(status))
    {
        //
        // Someone sent us another plug and play IRP after removed
        //
        LogError(ERRLOG_DEVICE_REMOVED,
                 status,
                 UNIQUE_ERRID(0x10),
                 NULL,
                 NULL);
        TWarn(("Received IRP after device was removed (status=%x).", status));
        Irp->IoStatus.Information = 0;
        Irp->IoStatus.Status = status;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
    }
    else
    {
        BOOLEAN fRemove = FALSE;
        BOOLEAN fForward = FALSE;

        switch (irpsp->MinorFunction)
        {
            case IRP_MN_START_DEVICE:
            case IRP_MN_CANCEL_REMOVE_DEVICE:
                TAssert(!(devext->dwfHPen & HPENF_DEVICE_STARTED));
                //
                // Forware the IRP down the stack.
                //
                status = SendSyncIrp(devext->LowerDevObj, Irp, TRUE);
                if (NT_SUCCESS(status))
                {
                    status = OemStartDevice(devext, Irp);
                    if (NT_SUCCESS(status))
                    {
                        devext->dwfHPen |= HPENF_DEVICE_STARTED;
                    }
                }
                else
                {
                    LogError(ERRLOG_LOWERDRV_IRP_FAILED,
                             status,
                             UNIQUE_ERRID(0x20),
                             NULL,
                             NULL);
                    TErr(("Failed to forward IRP (status=%x).", status));
                }
                Irp->IoStatus.Information = 0;
                Irp->IoStatus.Status = status;
                IoCompleteRequest(Irp, IO_NO_INCREMENT);
                break;

            case IRP_MN_REMOVE_DEVICE:
            case IRP_MN_SURPRISE_REMOVAL:
                fRemove = TRUE;
                //
                // Fall through.
                //
            case IRP_MN_STOP_DEVICE:
            case IRP_MN_QUERY_REMOVE_DEVICE:
                if (devext->dwfHPen & HPENF_DEVICE_STARTED)
                {
                    devext->dwfHPen &= ~HPENF_DEVICE_STARTED;
                    status = OemStopDevice(devext, Irp);
                }

                if (NT_SUCCESS(status))
                {
                    fForward = TRUE;
                }
                else
                {
                    Irp->IoStatus.Status = status;
                    IoCompleteRequest(Irp, IO_NO_INCREMENT);
                }
                break;

            case IRP_MN_QUERY_CAPABILITIES:
                status = SendSyncIrp(devext->LowerDevObj, Irp, TRUE);
                if (NT_SUCCESS(status))
                {
                    PDEVICE_CAPABILITIES devcaps;

                    devcaps = irpsp->Parameters.DeviceCapabilities.Capabilities;
                    if (devcaps != NULL)
                    {
                        SYSTEM_POWER_STATE i;

                        //
                        // This device is built-in to the system, so it should
                        // be impossible to surprise remove this device, but
                        // we will handle it anyway.
                        //
                        devcaps->SurpriseRemovalOK = TRUE;

                        //
                        // While the underlying serial bus might be able to
                        // wake the machine from low power (via wake on ring),
                        // the digitizer cannot.
                        //
                        devcaps->SystemWake = PowerSystemUnspecified;
                        devcaps->DeviceWake = PowerDeviceUnspecified;
                        devcaps->WakeFromD0 =
                        devcaps->WakeFromD1 =
                        devcaps->WakeFromD2 =
                        devcaps->WakeFromD3 = FALSE;
                        devcaps->DeviceState[PowerSystemWorking] =
                                PowerDeviceD0;
                        for (i = PowerSystemSleeping1;
                             i < PowerSystemMaximum;
                             i++)
                        {
                            devcaps->DeviceState[i] = PowerDeviceD3;
                        }
                    }
                }
                else
                {
                    LogError(ERRLOG_LOWERDRV_IRP_FAILED,
                             status,
                             UNIQUE_ERRID(0x30),
                             NULL,
                             NULL);
                    TErr(("Failed to forward IRP (status=%x).", status));
                }
                Irp->IoStatus.Status = status;
                IoCompleteRequest(Irp, IO_NO_INCREMENT);
                break;


            default:
                fForward = TRUE;
                break;
        }

        if (fForward)
        {
            IoSkipCurrentIrpStackLocation(Irp);
            TEnter(Func, (".IoCallDriver(DevObj=%p,Irp=%p)",
                          devext->LowerDevObj, Irp));
            status = IoCallDriver(devext->LowerDevObj, Irp);
            TExit(Func, (".IoCallDriver=%x", status));
        }

        if (fRemove)
        {
            //
            // Wait for the remove lock to free.
            //
            IoReleaseRemoveLockAndWait(&devext->RemoveLock, Irp);
        }
        else
        {
            IoReleaseRemoveLock(&devext->RemoveLock, Irp);
        }
    }

    TExit(API, ("=%x", status));
    return status;
}       //HpenPnp

/*++
    @doc    EXTERNAL

    @func   NTSTATUS | HpenPower | The power dispatch routine for this driver.

    @parm   IN PDEVICE_OBJECT | DevObj | Points to the device object.
    @parm   IN PIRP | Irp | Points to an I/O request packet.

    @rvalue SUCCESS | returns STATUS_SUCCESS.
    @rvalue FAILURE | returns NT status code.
--*/

NTSTATUS EXTERNAL
HpenPower(
    IN PDEVICE_OBJECT DevObj,
    IN PIRP Irp
    )
{
    NTSTATUS status;
    PDEVICE_EXTENSION devext;

    PAGED_CODE();

    TEnter(API, ("(DevObj=%p,Irp=%p,Minor=%!PWRMN!)",
                 DevObj, Irp,
                 IoGetCurrentIrpStackLocation(Irp)->MinorFunction));

    devext = GET_MINIDRIVER_DEVICE_EXTENSION(DevObj);
    status = IoAcquireRemoveLock(&devext->RemoveLock, Irp);
    if (!NT_SUCCESS(status))
    {
        //
        // Someone sent us another power IRP after removed
        //
        LogError(ERRLOG_DEVICE_REMOVED,
                 status,
                 UNIQUE_ERRID(0x40),
                 NULL,
                 NULL);
        TErr(("Received IRP after device was removed (status=%x).", status));
        Irp->IoStatus.Status = status;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
    }
    else
    {
        PIO_STACK_LOCATION irpsp = IoGetCurrentIrpStackLocation(Irp);
        POWER_STATE_TYPE PowerType = irpsp->Parameters.Power.Type;
        POWER_STATE NewPowerState = irpsp->Parameters.Power.State;
        BOOLEAN fSkipCalldown = FALSE;
      #ifdef PERFORMANCE_TEST
        IO_STACK_LOCATION stack = *irpsp;
        ULONGLONG BeforeTime, AfterTime;
      #endif

        switch (irpsp->MinorFunction)
        {
            case IRP_MN_SET_POWER:
                //
                // We only handle DevicePowerState IRPs that change
                // power states.
                //
                if ((PowerType == DevicePowerState) &&
                    (NewPowerState.DeviceState != devext->PowerState))
                {
                    TInfo(("Power state change (%d->%d)",
                           devext->PowerState, NewPowerState.DeviceState));
                    switch (NewPowerState.DeviceState)
                    {
                        case PowerDeviceD0:
                            //
                            // Transitioning from a low D state to D0.
                            //
                            IoMarkIrpPending(Irp);               
                            IoCopyCurrentIrpStackLocationToNext(Irp);
                            IoSetCompletionRoutine(Irp, 
                                                    DevicePowerIrpCompletion,
                                                    (PVOID)devext,
                                                    TRUE,
                                                    TRUE,
                                                    TRUE);

                            (VOID)PoCallDriver(devext->LowerDevObj, Irp);
                            fSkipCalldown = TRUE;
                            break;

                        case PowerDeviceD1:
                        case PowerDeviceD2:
                        case PowerDeviceD3:
                          #ifdef PERFORMANCE_TEST
                            TInfo(("Begin:StandbyDevice"));
                            BeforeTime = KeQueryInterruptTime();
                          #endif
                            OemStandbyDevice(devext);
                          #ifdef PERFORMANCE_TEST
                            AfterTime = KeQueryInterruptTime();
                            AfterTime -= BeforeTime;
                            AfterTime /= 10000;
                            LOGDBGMSG((ERRLOG_DEBUG_INFORMATION,
                                       0,
                                       "%d:StandbyDevice",
                                       (ULONG)AfterTime));
                          #endif
                            devext->PowerState = NewPowerState.DeviceState;
                            devext->dwfHPen |= HPENF_DIGITIZER_STANDBY;
                            PoSetPowerState(DevObj, PowerType, NewPowerState);
                            IoReleaseRemoveLock(&devext->RemoveLock, Irp);
                            IoMarkIrpPending(Irp);               
                        IoCopyCurrentIrpStackLocationToNext(Irp);
                        IoSetCompletionRoutine(Irp, 
                                    DevicePowerIrpCompletion,
                                    (PVOID)devext,
                                    TRUE,
                                    TRUE,
                                    TRUE);                           
                            TEnter(Func, (".PoCallDriver(DevObj=%p,Irp=%p)",
                                          devext->LowerDevObj, Irp));
                            (VOID)PoCallDriver(devext->LowerDevObj, Irp);
                            TExit(Func, (".PoCallDriver=%x", status));

                            fSkipCalldown = TRUE;
                            break;
                    }
                }
                break;

            case IRP_MN_WAIT_WAKE:
            case IRP_MN_QUERY_POWER:
                break;

            default:
                TErr(("Unsupported power IRP (%d)", irpsp->MinorFunction));
                break;
        }

        if (!fSkipCalldown)
        {
            IoMarkIrpPending(Irp);               
            IoCopyCurrentIrpStackLocationToNext(Irp);
            IoSetCompletionRoutine(Irp, 
                                    DevicePowerIrpCompletion,
                                    (PVOID)devext,
                                    TRUE,
                                    TRUE,
                                    TRUE);            
            TEnter(Func, (".PoCallDriver(DevObj=%p,Irp=%p)",
                          devext->LowerDevObj, Irp));
          #ifdef PERFORMANCE_TEST
            TInfo(("Begin:CallDownPowerIrp"));
            BeforeTime = KeQueryInterruptTime();
          #endif
            (VOID)PoCallDriver(devext->LowerDevObj, Irp);
          #ifdef PERFORMANCE_TEST
            AfterTime = KeQueryInterruptTime();
            AfterTime -= BeforeTime;
            AfterTime /= 10000;
            LOGDBGMSG((ERRLOG_DEBUG_INFORMATION,
                       status,
                       "%d:CallDownPowerIrp(%x,%x,%x)",
                       (ULONG)AfterTime,
                       stack.MinorFunction,
                       stack.Parameters.Power.Type,
                       stack.Parameters.Power.State));
          #endif
            TExit(Func, (".PoCallDriver=%x", status));
            IoReleaseRemoveLock(&devext->RemoveLock, Irp);
        }
        //since we mark the IRP as pending in all cases we also have to return STATUS_PENDING
        //PoSendNextPowerIrp is called in HIDLCASS
        status = STATUS_PENDING;        
    }
    
    TExit(API, ("=%x", status));
    return status;
}       //HpenPower

/*++
    @doc    INTERNAL

    @func   NTSTATUS | SendSyncIrp |
            Send an IRP synchronously down the stack.

    @parm   IN PDEVICE_OBJECT | DevObj | Points to the device object.
    @parm   IN PIRP | Irp | Points to the IRP.
    @parm   IN BOOLEAN | fCopyToNext | if TRUE, copy the irpsp to next location.

    @rvalue SUCCESS | returns STATUS_SUCCESS.
    @rvalue FAILURE | returns NT status code.
--*/

NTSTATUS INTERNAL
SendSyncIrp(
    __in PDEVICE_OBJECT DevObj,
    __in PIRP           Irp,
    __in BOOLEAN        fCopyToNext
    )
{
    NTSTATUS status;
    PIO_STACK_LOCATION irpsp = IoGetCurrentIrpStackLocation(Irp);
    KEVENT event;

    PAGED_CODE();

    TEnter(Func, ("(DevObj=%p,Irp=%p,fCopyToNext=%x,MajorFunc=%!IRPMJ!)",
                  DevObj, Irp, fCopyToNext,
                  irpsp->MajorFunction));

    KeInitializeEvent(&event, SynchronizationEvent, FALSE);
    if (fCopyToNext)
    {
        IoCopyCurrentIrpStackLocationToNext(Irp);
    }

    IoSetCompletionRoutine(Irp, IrpCompletion, &event, TRUE, TRUE, TRUE);
    if (irpsp->MajorFunction == IRP_MJ_POWER)
    {
        TEnter(Func, (".PoCallDriver(DevObj=%p,Irp=%p)", DevObj, Irp));
        status = PoCallDriver(DevObj, Irp);
        TExit(Func, (".IoCallDriver=%x", status));
    }
    else
    {
        TEnter(Func, (".IoCallDriver(DevObj=%p,Irp=%p)", DevObj, Irp));
        status = IoCallDriver(DevObj, Irp);
        TExit(Func, (".IoCallDriver=%x", status));
    }

    if (status == STATUS_PENDING)
    {
        status = KeWaitForSingleObject(&event,
                                       Executive,
                                       KernelMode,
                                       FALSE,
                                       NULL);
    }

    if (NT_SUCCESS(status))
    {
        status = Irp->IoStatus.Status;
    }

    TExit(Func, ("=%x", status));
    return status;
}       //SendSyncIrp

/*++
    @doc    INTERNAL

    @func   NTSTATUS | IrpCompletion | Completion routine for all IRPs.

    @parm   IN PDEVICE_OBJECT | DevObj | Points to the device object.
    @parm   IN PIRP | Irp | Points to an I/O request packet.
    @parm   IN PKEVENT | Event | Points to the event to notify.

    @rvalue Always returns STATUS_MORE_PROCESSING_REQUIRED.
--*/

NTSTATUS INTERNAL
IrpCompletion(
    __in PDEVICE_OBJECT DevObj,
    __in PIRP           Irp,
    __in_xcount_opt("varies") PVOID          Event
    )
{
   
#ifdef _DBG
    TEnter(Func, ("(DevObj=%p,Irp=%p,Event=%p,Status=%x)",
                  DevObj, Irp, Event, Irp->IoStatus.Status));
#else

    UNREFERENCED_PARAMETER(DevObj);
    UNREFERENCED_PARAMETER(Irp);
#endif

   
    /*
     *  If the lower driver returned PENDING, mark our stack location as
     *  pending also. This prevents the IRP's thread from being freed if
     *  the client's call returns pending.
     */
     //this should only be done when the completion routine returns anything but STATUS_MORE_PROCESSING_REQUIRED
    /*if (Irp->PendingReturned)
    {
        IoMarkIrpPending(Irp);
    }*/
    if(NULL != Event)
    {
        KeSetEvent((PKEVENT)Event, 0, FALSE);
    }

#ifdef _DBG
    TExit(Func, ("=%x", (unsigned int)STATUS_MORE_PROCESSING_REQUIRED));
#endif
    return STATUS_MORE_PROCESSING_REQUIRED;
}       //IrpCompletion

/*++
    @doc    INTERNAL

    @func   NTSTATUS | DevicePowerIrpCompletion | Completion routine for all Power IRPs.

    @parm   IN PDEVICE_OBJECT | DevObj | Points to the device object.
    @parm   IN PIRP | Irp | Points to an I/O request packet.
    @parm   IN PVOID | PDEVICE_EXTENSION | Points to the device extension.

    @rvalue Always returns STATUS_SUCCESS.
--*/

NTSTATUS INTERNAL
DevicePowerIrpCompletion(
    __in PDEVICE_OBJECT DevObj,
    __in PIRP           Irp,
    __in_xcount_opt("varies") PVOID          Context
    )
{
    NTSTATUS            status = Irp->IoStatus.Status;
    PIO_STACK_LOCATION  irpsp = IoGetCurrentIrpStackLocation(Irp);
    POWER_STATE_TYPE    PowerType = irpsp->Parameters.Power.Type;
    POWER_STATE         NewPowerState = irpsp->Parameters.Power.State;
    PDEVICE_EXTENSION   devext = (PDEVICE_EXTENSION)Context;
    PWORKITEM_DATA      workItemLoad;
    TEnter(Func, ("(DevObj=%p,Irp=%p,Context=%p,Status=%x)",
                  DevObj, Irp, Context, status));
    
    
    if(NULL != devext && 
        STATUS_SUCCESS == status)
    {
        if((devext->PowerState > NewPowerState.DeviceState) &&
            (PowerType == DevicePowerState))
        {

            //We're going to D0 so we need to reinitialize the serial driver.
            //this can't be done at DISPATCH_LEVEL so we queue a work item to take care of it.
            workItemLoad = ExAllocatePoolWithTag(NonPagedPool, sizeof(WORKITEM_DATA), HPEN_POOL_TAG);
            if(NULL != workItemLoad)
            {
                workItemLoad->workItem = IoAllocateWorkItem(DevObj);
                if(NULL != workItemLoad->workItem)
                {
                    //set new power state
                    workItemLoad->NewPowerState = NewPowerState;
                    workItemLoad->Irp = Irp;
                    IoQueueWorkItem(workItemLoad->workItem, WakeDeviceWorkItem, DelayedWorkQueue, workItemLoad);
                    status = STATUS_MORE_PROCESSING_REQUIRED;
                        
                }
                else
                {
                    //if we can't allocte the work item we have to free the space allocated for WORKITEM_DATA
                    ExFreePool(workItemLoad);
                }
            }            			 
        }
    }
    TExit(Func, ("=%x", status));    
    //we can oly return SUCCESS or MORE_PROCESSING_REQUIRED
    if(STATUS_MORE_PROCESSING_REQUIRED != status)
    {
        //this will only happen in the unlikely event that we are unable to queue the work item.
        if (Irp->PendingReturned)
        {
            IoMarkIrpPending(Irp);
        }
        status = STATUS_SUCCESS;
    }
    return status;
}       //IrpCompletion

VOID
  WakeDeviceWorkItem (
    __in PDEVICE_OBJECT  DevObj,
    __in_opt PVOID  Context 
    )
{
   
   
    PDEVICE_EXTENSION devext = GET_MINIDRIVER_DEVICE_EXTENSION(DevObj);    
    POWER_STATE       NewPowerState;
    PWORKITEM_DATA    workItemLoad = (PWORKITEM_DATA)Context;       
    PIRP              Irp;
    #ifdef PERFORMANCE_TEST
        ULONGLONG BeforeTime, AfterTime;
    #endif
    if(NULL != workItemLoad)
    {
        NewPowerState.DeviceState = workItemLoad->NewPowerState.DeviceState;
        #ifdef PERFORMANCE_TEST
            TInfo(("Begin:WakeDevice"));
            BeforeTime = KeQueryInterruptTime();
        #endif
             OemWakeupDevice(devext);            
        #ifdef PERFORMANCE_TEST
            AfterTime = KeQueryInterruptTime();
            AfterTime -= BeforeTime;
            AfterTime /= 10000;

            LOGDBGMSG((ERRLOG_DEBUG_INFORMATION,
                    0,
                    "%d:WakeDevice",
                    (ULONG)AfterTime));
        #endif
        //this is only called when we're going to D0
        devext->PowerState = PowerDeviceD0;
        devext->dwfHPen &= ~HPENF_DIGITIZER_STANDBY;
        PoSetPowerState(DevObj,
                        DevicePowerState,
                        NewPowerState);       	
        Irp = workItemLoad->Irp;        
        if(NULL != Irp)
        {
            IoReleaseRemoveLock(&devext->RemoveLock, Irp);
            Irp->IoStatus.Information = 0;
            Irp->IoStatus.Status = STATUS_SUCCESS;
            IoCompleteRequest(Irp, IO_NO_INCREMENT);
        }
        IoFreeWorkItem(workItemLoad->workItem);
        ExFreePool(workItemLoad);
    }
}



