/*++

Copyright (c) Microsoft Corporation. All rights reserved

Abstract:

    MSN monitor sample driver notification header

Environment:

    Kernel mode
    
--*/

#pragma once

#define TAG_NOTIFY 'yftN'

NTSTATUS
MonitorNfInitialize(
   IN PDEVICE_OBJECT deviceObject);

NTSTATUS
MonitorNfUninitialize();

NTSTATUS MonitorNfNotifyMessage(
   IN FWPS_STREAM_DATA*       streamBuffer,
   IN BOOLEAN                    inbound,
   IN USHORT                     localPort,
   IN USHORT                     remotePort);



