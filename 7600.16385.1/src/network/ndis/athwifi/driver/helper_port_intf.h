/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    helper_port_intf.h

Abstract:
    Defines the interfaces for the helper port
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    09-04-2007    Created

Notes:

--*/
#pragma once

typedef struct _MP_HELPER_PORT  MP_HELPER_PORT, *PMP_HELPER_PORT;
typedef struct _MP_BSS_LIST     MP_BSS_LIST, *PMP_BSS_LIST;
typedef struct _MP_RW_LOCK_STATE    MP_RW_LOCK_STATE, *PMP_RW_LOCK_STATE;

NDIS_STATUS
HelperPortAllocatePort(
	__in  NDIS_HANDLE             MiniportAdapterHandle,
    __in  PADAPTER                Adapter,
    __in  PMP_PORT                Port
    );

VOID
HelperPortFreePort(
    __in  PMP_PORT                Port
    );


NDIS_STATUS
HelperPortInitializePort(
    __in  PMP_PORT                Port,
    __in  PVOID                   RegistryInformation
    );

VOID
HelperPortTerminatePort(
    __in  PMP_PORT                Port
    );

NDIS_STATUS
HelperPortLoadRegistryInformation(
	__in  NDIS_HANDLE             MiniportAdapterHandle,
    __in  NDIS_HANDLE             ConfigurationHandle,
    __out PVOID*                  RegistryInformation
    );

VOID
HelperPortFreeRegistryInformation(
    __in  PVOID                   RegistryInformation
    );

VOID
HelperPortHandleMacCleanup(
    __in PMP_PORT                 Port
    );

NDIS_STATUS 
HelperPortHandleMiniportPause(
    __in  PMP_PORT                Port,
    __in_opt  PNDIS_MINIPORT_PAUSE_PARAMETERS     MiniportPauseParameters
    );

NDIS_STATUS 
HelperPortHandleMiniportRestart(
    __in  PMP_PORT                Port,
    __in_opt  PNDIS_MINIPORT_RESTART_PARAMETERS   MiniportRestartParameters
    );

NDIS_STATUS 
HelperPortHandlePortPause(
    __in  PMP_PORT                Port,
    __in  PMP_PORT                PortToPause
    );

NDIS_STATUS 
HelperPortHandlePortRestart(
    __in  PMP_PORT                Port,
    __in  PMP_PORT                PortToRestart
    );

VOID
HelperPortHandlePortTerminate(
    __in PMP_PORT                 Port,
    __in PMP_PORT                 PortToTerminate
    );

NDIS_STATUS
HelperPortHandleMiniportReset(
    __in  PMP_PORT                Port,
    __out PBOOLEAN                AddressingReset
    );

NDIS_STATUS
HelperPortHandleOidRequest(
    __in  PMP_PORT                Port,
    __in  PNDIS_OID_REQUEST       NdisOidRequest,
    __out BOOLEAN                 *pfOidCompleted
    );

NDIS_STATUS 
HelperPortReceiveEventHandler(
    __in  PMP_PORT                Port,
    __in  PMP_RX_MSDU             PacketList,
    __in  ULONG                   ReceiveFlags
    );

NDIS_STATUS
HelperPortHandleScan(
    __in  PMP_PORT                Port,
    __in  PMP_PORT                RequestingPort,
    __in  PMP_SCAN_REQUEST        ScanRequest,
    __in  PORT11_GENERIC_CALLBACK_FUNC    CompletionHandler
    );

VOID
HelperPortCancelScan(
    __in  PMP_PORT                Port,
    __in  PMP_PORT                RequestingPort
    );

NDIS_STATUS
HelperPortFlushBSSList(
    __in  PMP_PORT                Port
    );

PMP_BSS_LIST
HelperPortQueryAndRefBSSList(
    __in  PMP_PORT                Port,
    __in  PMP_PORT                RequestingPort,
    __in  PMP_RW_LOCK_STATE       LockState
    );

VOID
HelperPortReleaseBSSListRef(
    __in  PMP_PORT                Port,
    __in  PMP_BSS_LIST            BSSList,
    __in  PMP_RW_LOCK_STATE       LockState
    );

VOID
HelperPortNotify(
    __in  PMP_PORT        Port,
    PVOID               Notif
    );


