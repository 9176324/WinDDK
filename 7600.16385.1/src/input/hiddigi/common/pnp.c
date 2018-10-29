/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    pnp.c

Abstract:

    Code for handling HID related requests

Author:
   Olumuyiwa Durojaiye Sept-20-2007

Environment:

    kernel mode only

Revision History:

--*/

#include "pch.h"
#define MODULE_ID                       2


#ifdef ALLOC_PRAGMA
  #pragma alloc_text(PAGE, NInputEvtDeviceD0Exit)
  #pragma alloc_text(PAGE, NInputStopDevice)
#endif

NTSTATUS
NInputEvtDeviceD0Entry(
   __in WDFDEVICE  Device,    
   __in WDF_POWER_DEVICE_STATE  PreviousState    
   )
/*++

Routine Description:

Used to configure device on D0 entry

Arguments:
 Device - WDF device 

 PreviousState - previous power state

 Return Value:
 On success, the function returns STATUS_SUCCESS
 On failure it passes the relevant error code to the caller.

*/
{
    NTSTATUS            status = STATUS_SUCCESS;
    PDEVICE_EXTENSION   DevContext = GetDeviceContext(Device);

    UNREFERENCED_PARAMETER(PreviousState);

    TEnter(Func, ("(Device=%p)", Device));
    KdPrint(("DO entry\n"));    
    if(DevContext->dwfHPen & HPENF_DEVICE_STARTED)
    {
#ifdef PERFORMANCE        
        ULONGLONG BeforeTime, AfterTime;
        BeforeTime = KeQueryInterruptTime();
#endif
        status = NInputInitSerialPort(DevContext); 
#ifdef PERFORMANCE 
        AfterTime = KeQueryInterruptTime();
        AfterTime -= BeforeTime;
        AfterTime /= 10000;
        TInfo(("Wake Time: %d", (ULONG)AfterTime));
#endif
        if(NT_SUCCESS(status))
        {
            status = NInputWakeDevice(Device);            
        }
    }
    else
    {
        KdPrint(("device not started\n"));
    }

    TExit(Func,("=%x", status));
    return status;
}

NTSTATUS
NInputEvtDeviceD0Exit(
   __in WDFDEVICE  Device,    
   __in WDF_POWER_DEVICE_STATE  TargetState   
   )
/*++

Routine Description:

   Used to configure device on D0 exit

Arguments:
   Device - WDF device 

   PreviousState - Target power state

Return Value:
   On success, the function returns STATUS_SUCCESS
   On failure it passes the relevant error code to the caller.

*/
{
    NTSTATUS          status = STATUS_SUCCESS;    
    
    PAGED_CODE();
    TEnter(Func, ("(Device=%p)", Device));
    UNREFERENCED_PARAMETER(TargetState);    
    status = NInputStandbyDevice(Device);    
    TExit(Func, ("=%x", status));
    return status;
}
    
NTSTATUS
NInputEvtDeviceQueryRemove(
   IN WDFDEVICE  Device
   )
{
    NTSTATUS            status = STATUS_SUCCESS;
    TEnter(Func, ("(DevContext=%p)", Device));
    
    status = NInputEvtDeviceStop(Device);
    TExit(Func, ("=%x", status));
    return status;
}

NTSTATUS
NInputEvtDeviceStop(
   IN WDFDEVICE  Device
   )
{
    
    NTSTATUS            status = STATUS_SUCCESS;
    PDEVICE_EXTENSION   DevContext = GetDeviceContext(Device);

    TEnter(Func, ("(DevContext=%p)", DevContext));

    KdPrint(("NInputEvtDeviceStop\n"));
    status = NInputStopDevice(DevContext);
    if(!NT_SUCCESS(status))
    {
        KdPrint(("stop failed!!!\n"));
    }    
    TExit(Func, ("=%x", status));
    return status;
}

NTSTATUS INTERNAL
NInputStartDevice(
    __in PDEVICE_EXTENSION DevContext
    )
/*++
Routine Description:
  This function does all the configuration and initialization necessary to communicate with the serial device
Argument:
  DevContext - Device context

Return Value:
 On success, the function returns STATUS_SUCCESS
 On failure it passes the relevant error code to the caller.
*/ 
{
    
    NTSTATUS status = STATUS_SUCCESS;

    TEnter(Func, ("(DevContext=%p)", DevContext));

    if ((DevContext->dwfHPen & HPENF_DEVICE_STARTED) == 0)
    {
        status = SerialSendInitIRP(DevContext, IRP_MJ_CREATE);
        if(NT_SUCCESS(status))
        {
            DevContext->dwfHPen |= HPENF_SERIAL_OPENED;
            status = SerialSyncSendIoctl(IOCTL_SERIAL_INTERNAL_BASIC_SETTINGS,
                                     DevContext->IoTarget,
                                     NULL,
                                     0,
                                     &DevContext->OemData.PrevSerialSettings,
                                     sizeof(DevContext->OemData.PrevSerialSettings),                                     
                                     TRUE);
            if (NT_SUCCESS(status))
            {
                DevContext->dwfHPen |= HPENF_SERIAL_INITED;
                status = NInputInitSerialPort(DevContext);
                if(NT_SUCCESS(status))
                {
                    status = NInputInitDevice(DevContext);
                    if(NT_SUCCESS(status))
                    {
                        DevContext->dwfHPen |= HPENF_DEVICE_STARTED;
                    }
                }
                else
                {
                    KdPrint(("failed to init serial port\n"));
                }
            }
            
        }
        else
        {
            KdPrint(("failed to create serial port\n"));
        }
    }
    TExit(Func, ("=%x", status));
    return status;
}

NTSTATUS INTERNAL
NInputStopDevice(
    __in PDEVICE_EXTENSION DevContext
    )
/*++

Routine Description:
  This function restore old port settings and closes the serial port

Argument:

  DevContext - Device context

Return Value:
  On success, the function returns STATUS_SUCCESS
  On failure it passes the relevant error code to the caller.

*/
{
    NTSTATUS    status = STATUS_SUCCESS;
    
    PAGED_CODE();
    TEnter(Func, ("(DevContext=%p)", DevContext));
   
    if ((DevContext->dwfHPen & HPENF_DEVICE_STARTED) &&
        (DevContext->dwfHPen & HPENF_SERIAL_INITED)
        )
    {
        
        DevContext->dwfHPen &= ~HPENF_DEVICE_STARTED;
        DevContext->dwfHPen &= ~HPENF_SERIAL_INITED;
        status = SerialSyncSendIoctl(IOCTL_SERIAL_INTERNAL_RESTORE_SETTINGS,
                                     DevContext->IoTarget,
                                     &DevContext->OemData.PrevSerialSettings,
                                     sizeof(DevContext->OemData.PrevSerialSettings),
                                     NULL,
                                     0,
                                     TRUE);
        if (!NT_SUCCESS(status))
        {
            LogError(ERRLOG_RESTORE_SERIAL_FAILED,
                     status,
                     UNIQUE_ERRID(0x20),
                     NULL,
                     NULL);
            TWarn(("Failed to restore COM settings (status=%x).", status));
        }
    }

    if (DevContext->dwfHPen & HPENF_SERIAL_OPENED)
    {
        DevContext->dwfHPen &= ~HPENF_SERIAL_OPENED;
        status = SerialClosePort(DevContext);
    }
    TExit(Func, ("=%x", status));
    return status;
}


