/*++

Copyright (c) Microsoft Corporation. All rights reserved

Abstract:

    MSN monitor sample driver IO control routines

Environment:

    Kernel mode

--*/

#include "ntddk.h"
#include "fwpmk.h"

#pragma warning(push)
#pragma warning(disable:4201)       // unnamed struct/union

#include "fwpsk.h"

#pragma warning(pop)


#include "ioctl.h"

#include "msnmntr.h"
#include "ctl.h"

#include "notify.h"

//
// Software Tracing Definitions 
//
#define WPP_CONTROL_GUIDS \
    WPP_DEFINE_CONTROL_GUID(MsnMntrCtl,(eab718af, 52de, 477c, 874d, cb49746bb131),  \
        WPP_DEFINE_BIT(TRACE_INIT)               \
        WPP_DEFINE_BIT(TRACE_DEVICE_CONTROL)     \
        WPP_DEFINE_BIT(TRACE_STATE) )

#include "ctl.tmh"

__drv_dispatchType(IRP_MJ_CREATE) DRIVER_DISPATCH MonitorCtlCreate;
NTSTATUS
MonitorCtlCreate (
    IN PDEVICE_OBJECT deviceObject,
    IN PIRP irp
    );

__drv_dispatchType(IRP_MJ_CLOSE) DRIVER_DISPATCH MonitorCtlClose;
NTSTATUS
MonitorCtlClose (
    IN PDEVICE_OBJECT deviceObject,
    IN PIRP irp
    );

__drv_dispatchType(IRP_MJ_CLEANUP) DRIVER_DISPATCH MonitorCtlCleanup;
NTSTATUS
MonitorCtlCleanup (
    IN PDEVICE_OBJECT deviceObject,
    IN PIRP irp
    );

__drv_dispatchType_other DRIVER_DISPATCH MonitorCtlDispatchRequest;
NTSTATUS
MonitorCtlDispatchRequest (
    IN PDEVICE_OBJECT deviceObject,
    IN PIRP irp
    );

__drv_dispatchType(IRP_MJ_DEVICE_CONTROL) DRIVER_DISPATCH MonitorCtlDeviceControl;
NTSTATUS
MonitorCtlDeviceControl (
    IN PDEVICE_OBJECT deviceObject,
    IN PIRP irp
    );


NTSTATUS
MonitorCtlDriverInit(
    IN  PDRIVER_OBJECT  driverObject)
/*++

Routine Description:

   Initializes the Dispatch information for our driver.

Arguments:
   
   [in]  PDRIVER_OBJECT driverObject - Our driver.

Return Value:

   STATUS_SUCCESS

--*/
{
    long                l;
    
    DoTraceMessage(TRACE_INIT, "MsnMonitor Control Initialization in progress.");

    // suppress "The 'MajorFunction' member of _DRIVER_OBJECT should not be accessed by a driver warnings"                        
    #pragma warning(push)
    #pragma warning(disable:28175) 
    for (l = 0; l < IRP_MJ_MAXIMUM_FUNCTION; l++)
    {
        driverObject->MajorFunction[l] = MonitorCtlDispatchRequest;
    }

    driverObject->MajorFunction[IRP_MJ_CREATE] = MonitorCtlCreate;
    driverObject->MajorFunction[IRP_MJ_CLOSE] = MonitorCtlClose;
    driverObject->MajorFunction[IRP_MJ_CLEANUP] = MonitorCtlCleanup;

    driverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = MonitorCtlDeviceControl;
    #pragma warning(pop)

    return STATUS_SUCCESS;
}

NTSTATUS
MonitorCtlDispatchRequest (
    IN PDEVICE_OBJECT deviceObject,
    IN PIRP irp
    )
/*++

Routine Description:

   Handles all requests that are not dealt with by other dispatch handlers.

Arguments:
   
   [in]  PDRIVER_OBJECT driverObject - Our driver.
   [in]  IRP irp - The IO request packet to process

Return Value:

   STATUS_SUCCESS

--*/
{
    NTSTATUS    status = STATUS_SUCCESS;

    UNREFERENCED_PARAMETER(deviceObject);
    
    IoCompleteRequest(irp, IO_NO_INCREMENT);

    return status;
}


NTSTATUS
MonitorCtlCreate (
    IN PDEVICE_OBJECT deviceObject,
    IN PIRP irp
    )
/*++

Routine Description:

   Handles create requests (we don't do any additional processing at this point).

Arguments:
   
   [in]  PDRIVER_OBJECT driverObject - Our driver.
   [in]  IRP irp - The IO request packet to process

Return Value:

   STATUS_SUCCESS

--*/
{
    NTSTATUS    status = STATUS_SUCCESS;

    UNREFERENCED_PARAMETER(deviceObject);

    IoCompleteRequest(irp, IO_NO_INCREMENT);

    return status;
}

NTSTATUS
MonitorCtlClose (
    IN PDEVICE_OBJECT deviceObject,
    IN PIRP irp
    )
/*++

Routine Description:

   Handles close requests (we don't do any additional processing at this point).

Arguments:
   
   [in]  PDRIVER_OBJECT driverObject - Our driver.
   [in]  IRP irp - The IO request packet to process

Return Value:

   STATUS_SUCCESS

--*/
{
    NTSTATUS    status = STATUS_SUCCESS;

    UNREFERENCED_PARAMETER(deviceObject);
    
    IoCompleteRequest(irp, IO_NO_INCREMENT);

    return status;
}

NTSTATUS
MonitorCtlCleanup (
    IN PDEVICE_OBJECT deviceObject,
    IN PIRP irp
    )
/*++

Routine Description:

   Handles cleanup requests (we don't do any additional processing at this point).

Arguments:
   
   [in]  PDRIVER_OBJECT driverObject - Our driver.
   [in]  IRP irp - The IO request packet to process

Return Value:

   STATUS_SUCCESS

--*/
{
    NTSTATUS    status = STATUS_SUCCESS;

    UNREFERENCED_PARAMETER(deviceObject);
    
    IoCompleteRequest(irp, IO_NO_INCREMENT);

    return status;
}

NTSTATUS
MonitorCtlDeviceControl (
    IN PDEVICE_OBJECT deviceObject,
    IN PIRP irp
    )
/*++

Routine Description:

   Handles device IO control requests. This function drives all communication
   between the usermode exe and this driver.

Arguments:
   
   [in]  PDRIVER_OBJECT driverObject - Our driver.
   [in]  IRP irp - The IO request packet to process

Return Value:

   STATUS_SUCCESS

--*/
{
   NTSTATUS            status = STATUS_SUCCESS;
   PIO_STACK_LOCATION  irpSp;
   ULONG               ioControlCode;

   UNREFERENCED_PARAMETER(deviceObject);

   irpSp = IoGetCurrentIrpStackLocation(irp);

   ioControlCode = irpSp->Parameters.DeviceIoControl.IoControlCode;

   DoTraceMessage(TRACE_DEVICE_CONTROL, "MsnMonitor Dispatch Device Control: 0x%x", ioControlCode);

   switch (ioControlCode)
   {
   case MONITOR_IOCTL_ENABLE_MONITOR:
      {
          MONITOR_SETTINGS* monitorSettings = (MONITOR_SETTINGS*) irp->AssociatedIrp.SystemBuffer;
         
          status = MonitorCoEnableMonitoring(monitorSettings);
          break;
      }

   case MONITOR_IOCTL_DISABLE_MONITOR:
      {
         status = STATUS_SUCCESS;
         
         MonitorCoDisableMonitoring();

         break;
      }

   default:
       irp->IoStatus.Information = 0L;
       irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
       status = irp->IoStatus.Status;
   }

   if (status != STATUS_PENDING)
   {
      IoCompleteRequest(irp, IO_NO_INCREMENT);
   }

   return status;
}

