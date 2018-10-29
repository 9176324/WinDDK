/*++

Copyright (c) Microsoft Corporation. All rights reserved

Abstract:

    MSN monitor sample driver initialization routines

Environment:

    Kernel mode

--*/

#include "ndis.h"
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
    WPP_DEFINE_CONTROL_GUID(MsnMntrInit,(e7db16bb, 41be, 4c05, b73e, 5feca06f8207),  \
        WPP_DEFINE_BIT(TRACE_INIT)               \
        WPP_DEFINE_BIT(TRACE_SHUTDOWN) )

#include "init.tmh"

PDEVICE_OBJECT monitorDeviceObject;
UNICODE_STRING monitorSymbolicLink;

// ===========================================================================
//
// LOCAL PROTOTYPES
//
// ===========================================================================

DRIVER_INITIALIZE DriverEntry;
NTSTATUS
DriverEntry(
    IN  PDRIVER_OBJECT          driverObject,
    IN  PUNICODE_STRING         registryPath
    );

DRIVER_UNLOAD DriverUnload;
VOID
DriverUnload(
    IN  PDRIVER_OBJECT          driverObject
    );

// ===========================================================================
//
// PUBLIC FUNCTIONS
//
// ===========================================================================

NTSTATUS
DriverEntry(
    IN  PDRIVER_OBJECT          driverObject,
    IN  PUNICODE_STRING         registryPath
    )
/*++

Routine Description:

    Main driver entry point. Called at driver load time

Arguments:

    driverObject            Our driver
    registryPath            A reg key where we can keep parameters

Return Value:

    status of our initialization. A status != STATUS_SUCCESS aborts the
    driver load and we don't get called again.

    Each component is responsible for logging any error that causes the
    driver load to fail.

--*/
{
   NTSTATUS          status;
   UNICODE_STRING    deviceName;
   BOOLEAN           validSymbolicLink = FALSE;
   BOOLEAN           initializedCallouts = FALSE;

   //
   // This macro is required to initialize software tracing on XP and beyond
   // For XP and beyond use the DriverObject as the first argument.
   // 
   
   WPP_INIT_TRACING(driverObject,registryPath);

   DoTraceMessage(TRACE_INIT, "Initializing MsnMonitor Driver");

   monitorDeviceObject = NULL;

   UNREFERENCED_PARAMETER(registryPath);

   driverObject->DriverUnload = DriverUnload;

   status = MonitorCtlDriverInit(driverObject);

   if (!NT_SUCCESS(status))
   {
      goto cleanup;
   }

   RtlInitUnicodeString(&deviceName,
                        MONITOR_DEVICE_NAME);

   status = IoCreateDevice(driverObject, 0, &deviceName, FILE_DEVICE_NETWORK, 0, FALSE, &monitorDeviceObject);
   if (!NT_SUCCESS(status))
   {
      goto cleanup;
   }

   status = MonitorCoInitialize(monitorDeviceObject);
   if (!NT_SUCCESS(status))
   {
      initializedCallouts = TRUE;
      goto cleanup;
   }

   RtlInitUnicodeString(&monitorSymbolicLink, MONITOR_SYMBOLIC_NAME);

   status = IoCreateSymbolicLink(&monitorSymbolicLink, &deviceName);

   if (!NT_SUCCESS(status))
   {
      goto cleanup;
   }
   validSymbolicLink = TRUE;

   status = MonitorNfInitialize(monitorDeviceObject);
   if (!NT_SUCCESS(status))
   {
      goto cleanup;
   }

cleanup:
   if (!NT_SUCCESS(status))
   {
      DoTraceMessage(TRACE_INIT, "MsnMonitor Initialization Failed.");

      WPP_CLEANUP(driverObject);

      if (initializedCallouts)
      if (validSymbolicLink)
      {
         IoDeleteSymbolicLink(&monitorSymbolicLink);
      }

      if (monitorDeviceObject)
      {
         IoDeleteDevice(monitorDeviceObject);
      }
   }

   return status;
}

VOID
DriverUnload(
    IN  PDRIVER_OBJECT          driverObject
    )
/*++

Routine Description:

    Called to indicate that we are being unloaded and to cause an orderly
    shutdown

Arguments:

    driverObject            Our driver

Return Value:

    None

--*/
{
   UNREFERENCED_PARAMETER(driverObject);

   MonitorCoUninitialize();

   MonitorNfUninitialize();

   IoDeleteDevice(monitorDeviceObject);
   IoDeleteSymbolicLink(&monitorSymbolicLink);

   DoTraceMessage(TRACE_SHUTDOWN, "MsnMonitor Driver Shutting Down");

   WPP_CLEANUP(driverObject);
}


