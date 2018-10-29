/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    ap_intf.h

Abstract:
    Contains interfaces into the ExtAP port
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    09-20-2007    Created

Notes:

--*/
#pragma once

#ifndef _EXTAP_INTF_H
#define _EXTAP_INTF_H

//
// Forward declaration
//
typedef struct _ADAPTER         ADAPTER, *PADAPTER;
typedef struct _HVL             HVL, *PHVL;
typedef struct _HW              HW, *PHW;
typedef struct _MP_PORT         MP_PORT, *PMP_PORT;
typedef struct _VNIC            VNIC, *PVNIC;


NDIS_STATUS
Ap11Fill80211Attributes(
    __in  PMP_PORT              Port,
    __out PNDIS_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES Attr
    );

VOID
Ap11Cleanup80211Attributes(
    __in  PMP_PORT              Port,
    __in  PNDIS_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES Attr
    );

NDIS_STATUS
Ap11LoadRegistryInformation(
    __in  NDIS_HANDLE           MiniportAdapterHandle,
    __in  NDIS_HANDLE           ConfigurationHandle,
    __deref_out_opt PVOID*      RegistryInformation
    );

VOID
Ap11FreeRegistryInformation(
    __in  PVOID                 RegistryInformation
    );

NDIS_STATUS
Ap11AllocatePort(
	__in  NDIS_HANDLE           MiniportAdapterHandle,
    __in  PADAPTER              Adapter,
    __in  PMP_PORT              Port
    );

VOID
Ap11FreePort(
    __in  PMP_PORT              Port
    );


NDIS_STATUS
Ap11InitializePort(
    __in  PMP_PORT              Port,
    __in  PVOID                 RegistryInformation
    );

VOID
Ap11TerminatePort(
    __in  PMP_PORT              Port
    );

NDIS_STATUS
Ap11PausePort(
    __in  PMP_PORT              Port
    );

NDIS_STATUS
Ap11RestartPort(
    __in  PMP_PORT              Port
    );

NDIS_STATUS
Ap11NdisResetStart(
    __in  PMP_PORT              Port
    );

NDIS_STATUS
Ap11NdisResetEnd(
    __in  PMP_PORT              Port
    );

NDIS_STATUS
Ap11SetPower(
    __in  PMP_PORT              Port,
    __in  NDIS_DEVICE_POWER_STATE NewDevicePowerState
    );
    
NDIS_STATUS
Ap11OidHandler(
    __in  PMP_PORT              Port,
    __in  PNDIS_OID_REQUEST     NdisOidRequest
    );


NDIS_STATUS 
Ap11DirectOidHandler(
    __in  PMP_PORT              Port,
    __in  PNDIS_OID_REQUEST     OidRequest
    );

VOID 
Ap11SendCompleteHandler(
    __in  PMP_PORT              Port,
    __in  PMP_TX_MSDU           PacketList,
    __in  ULONG                 SendCompleteFlags
    );

VOID 
Ap11SendNBLHandler(
    __in  PMP_PORT              Port,
    __in  PNET_BUFFER_LIST      NetBufferLists,
    __in  ULONG                 SendFlags
    );

NDIS_STATUS 
Ap11ReceiveHandler(
    __in  PMP_PORT              Port,
    __in  PMP_RX_MSDU           PacketList,
    __in  ULONG                 ReceiveFlags
    );

VOID
Ap11IndicateStatus(
    __in  PMP_PORT              Port,
    __in  NDIS_STATUS           StatusCode,
    __in  PVOID                 StatusBuffer,
    __in  ULONG                 StatusBufferSize
    );

VOID
Ap11Notify(
    __in  PMP_PORT        Port,
    PVOID               Notif
);

#endif  // _EXTAP_INTF_H

