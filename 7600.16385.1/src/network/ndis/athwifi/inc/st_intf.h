/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    st_intf.h

Abstract:
    Contains interfaces into the ExtSTA port
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    09-04-2007    Created

Notes:

--*/
#pragma once

//
// Forward declaration
//
typedef struct _ADAPTER         ADAPTER, *PADAPTER;
typedef struct _HVL             HVL, *PHVL;
typedef struct _HW              HW, *PHW;
typedef struct _MP_PORT         MP_PORT, *PMP_PORT;
typedef struct _VNIC            VNIC, *PVNIC;


NDIS_STATUS
Sta11Fill80211Attributes(
    __in  PMP_PORT                Port,
    __out PNDIS_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES Attr
    );

VOID
Sta11Cleanup80211Attributes(
    __in  PMP_PORT                Port,
    __in  PNDIS_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES Attr
    );


NDIS_STATUS
Sta11LoadRegistryInformation(
	__in  NDIS_HANDLE             MiniportAdapterHandle,
    __in  NDIS_HANDLE             ConfigurationHandle,
    __out PVOID*                  RegistryInformation
    );

VOID
Sta11FreeRegistryInformation(
    __in  PVOID                   RegistryInformation
    );

NDIS_STATUS
Sta11AllocatePort(
	__in  NDIS_HANDLE             MiniportAdapterHandle,
    __in  PADAPTER                Adapter,
    __in  PMP_PORT                Port
    );

VOID
Sta11FreePort(
    __in  PMP_PORT                Port
    );


NDIS_STATUS
Sta11InitializePort(
    __in  PMP_PORT                Port,
    __in  PVOID                   RegistryInformation
    );

VOID
Sta11TerminatePort(
    __in  PMP_PORT                Port
    );

NDIS_STATUS
Sta11PausePort(
    __in  PMP_PORT                Port
    );

NDIS_STATUS
Sta11RestartPort(
    __in  PMP_PORT                Port
    );

NDIS_STATUS
Sta11NdisResetStart(
    __in  PMP_PORT                Port
    );

NDIS_STATUS
Sta11NdisResetEnd(
    __in  PMP_PORT                Port
    );

NDIS_STATUS
Sta11SetPower(
    __in  PMP_PORT                Port,
    __in  NDIS_DEVICE_POWER_STATE NewDevicePowerState
    );

/*
 * This is the OID handler for a station. It handles most of the station specific 
 * OIDs and forwards the rest up to the BasePort for processing.
 * 
 * Look at Port11HandleOidRequest for information about when this function is
 * called
 */
NDIS_STATUS
Sta11OidHandler(
    __in  PMP_PORT                Port,
    __in  PNDIS_OID_REQUEST       NdisOidRequest
    );

/*
 * This is the send notifications handler for a station. It can return
 * NDIS_STATUS_PENDING if the station is currently scanning or in the middle of 
 * a roam. Else it would permit the packets go through
 * 
 * Look at Port11NotifySend for information about when this function is
 * called
 */
NDIS_STATUS
Sta11SendEventHandler(
    __in  PMP_PORT                Port,
    __in  PMP_TX_MSDU             PacketList,
    __in  ULONG                   SendFlags
    );

NDIS_STATUS
Sta11SetOperationMode(
    __in  PMP_PORT                Port,
    __in  ULONG                   OpMode
    );

/*
 * This is the receive notifications handler for a station. It looks at
 * management packets to update association state. It also looks at data packets
 * and does filtering based on privacy settings. If the packet is mal-formed
 * or not correctly ciphered, this function can return a false to block
 * the packet from getting indicated up to the OS
 * 
 * Look at Port11NotifyReceive for information about when this function is
 * called
 */
NDIS_STATUS
Sta11ReceiveEventHandler(
    __in  PMP_PORT                Port,
    __in  PMP_RX_MSDU             PacketList,
    __in  ULONG                   ReceiveFlags
    );


VOID
Sta11IndicateStatus(
    __in  PMP_PORT                Port,
    __in  NDIS_STATUS             StatusCode,
    __in  PVOID                   StatusBuffer,
    __in  ULONG                   StatusBufferSize
    );

NDIS_STATUS
Sta11ScanComplete(
    __in  PMP_PORT                Port,
    __in  PVOID                   Data
    );

VOID
Sta11Notify(
    __in  PMP_PORT        Port,
    PVOID               Notif
);


