/*++

Copyright (c) Microsoft Corporation. All rights reserved

Abstract:

    MSN monitor sample callout driver IOCTL header

Environment:

    Kernel mode
    
--*/

#pragma once

#define MONITOR_DEVICE_NAME     L"\\Device\\MSNMonitor"
#define MONITOR_SYMBOLIC_NAME   L"\\DosDevices\\Global\\MSNMonitor"
#define MONITOR_DOS_NAME   L"\\\\.\\MSNMonitor"

typedef enum _MONITOR_OPERATION_MODE
{
   invalidOperation = 0,
   monitorTraffic = 1,
   monitorOperationMax
} MONITOR_OPERATION_MODE;

typedef struct _MONITOR_SETTINGS
{
   MONITOR_OPERATION_MODE  monitorOperation;
   UINT32                  flags;
} MONITOR_SETTINGS;

#define	MONITOR_IOCTL_ENABLE_MONITOR  CTL_CODE(FILE_DEVICE_NETWORK, 0x1, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define	MONITOR_IOCTL_DISABLE_MONITOR CTL_CODE(FILE_DEVICE_NETWORK, 0x2, METHOD_BUFFERED, FILE_ANY_ACCESS)


