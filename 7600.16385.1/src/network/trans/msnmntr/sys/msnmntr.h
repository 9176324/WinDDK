/*++

Copyright (c) Microsoft Corporation. All rights reserved

Abstract:

    MSN monitor sample driver callout header

Environment:

    Kernel mode
    
--*/

#pragma once

typedef struct _FLOW_DATA
{
   UINT64      flowHandle;
   UINT64      flowContext;
   UINT64      calloutId;
   ULONG       localAddressV4;
   USHORT      localPort;
   USHORT      ipProto;
   ULONG       remoteAddressV4;
   USHORT      remotePort;
   WCHAR*      processPath;
   LIST_ENTRY  listEntry;
   BOOLEAN     deleting;
} FLOW_DATA;

NTSTATUS
MonitorCoInitialize(PDEVICE_OBJECT deviceObject);

VOID
MonitorCoUninitialize();

NTSTATUS 
MonitorCoRegisterCallouts(IN OUT void* deviceObject);

NTSTATUS 
MonitorCoUnregisterCallouts();

NTSTATUS 
MonitorCoSetApplication(
   __in_ecount(size) wchar_t*    fileName,
   IN    ULONG       size);

NTSTATUS 
MonitorCoEnableMonitoring(
   IN    MONITOR_SETTINGS* monitorSettings);

VOID 
MonitorCoDisableMonitoring();


