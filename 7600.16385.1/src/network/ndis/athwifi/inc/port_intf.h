/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    port_intf.h

Abstract:
    Contains interfaces into the port
    
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

#define DEFAULT_NDIS_PORT_NUMBER    0
#define HELPER_PORT_PORT_NUMBER     0xFFFFFFFF

// Macro to verify if this port number is allocated/"registered" with NDIS
#define IS_ALLOCATED_PORT_NUMBER(_PortNumber)   (_PortNumber != DEFAULT_NDIS_PORT_NUMBER && _PortNumber != HELPER_PORT_PORT_NUMBER)



NDIS_STATUS
Port11Fill80211Attributes(
    __in  PADAPTER                Adapter,
    __out PNDIS_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES Attr
    );

VOID
Port11Cleanup80211Attributes(
    __in  PADAPTER                Adapter,
    __in  PNDIS_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES attr
    );

NDIS_STATUS
Port11LoadRegistryInformation(
    __in  PADAPTER                Adapter,
    __in  NDIS_HANDLE             ConfigurationHandle,
    __deref_out_opt PVOID*        RegistryInformation
    );

VOID
Port11FreeRegistryInformation(
    __in  PVOID                   RegistryInformation
    );
                                
NDIS_STATUS
Port11AllocatePort(
    __in  PADAPTER                Adapter,
    __in  MP_PORT_TYPE            PortType,
    __deref_out_opt PMP_PORT*     Port
    );

VOID
Port11FreePort(
    __in  PMP_PORT                Port,
    __in  BOOLEAN                 FreeBasePort
    );

NDIS_STATUS
Port11InitializePort(
    __in  PMP_PORT                Port,
    __in  PHVL                    Hvl,
    __in  PVOID                   RegistryInformation
    );

VOID
Port11TerminatePort(
    __in  PMP_PORT                Port,
    __in  BOOLEAN                 TerminateBasePort
    );

PMP_PORT
Port11TranslatePortNumberToPort(
    __in  PADAPTER                Adapter,
    __in  NDIS_PORT_NUMBER        PortNumber
    );

VOID
Port11RemovePortFromAdapterList(
    __in PMP_PORT                 PortToRemove
    );
    
NDIS_STATUS
Port11PausePort(
    __in  PMP_PORT                Port,
    __in  BOOLEAN                 IsInternal
    );

NDIS_STATUS
Port11RestartPort(
    __in  PMP_PORT                Port,
    __in  BOOLEAN                 IsInternal
    );

NDIS_STATUS
Port11NdisResetStart(
    __in  PMP_PORT                Port
    );

NDIS_STATUS
Port11NdisResetEnd(
    __in  PMP_PORT                Port
    );

NDIS_STATUS
Port11SetPower(
    __in  PMP_PORT                Port,
    __in  NDIS_DEVICE_POWER_STATE NewDevicePowerState
    );

/**
 * This is called from the MP layer when NDIS sends an OID request on a port using 
 * MiniportOidRequest. The call gets forwarded to the port specific OID request handler
 * for processing. The port specific handler may process the OID or may pass the
 * request to the BasePortOidHandler function in the base port to process some of the
 * OIDs.
 *
 */
NDIS_STATUS
Port11HandleOidRequest(
    __in  PMP_PORT                Port,
    __in  PNDIS_OID_REQUEST       NdisOidRequest
    );

VOID
Port11CompletePendingOidRequest(
    __in  PMP_PORT                Port,
    __in  NDIS_STATUS             NdisStatus
    );


/**
 * This is called from the MP layer when NDIS sends an OID request on a port using 
 * MiniportDirectOidRequest. The call gets forwarded to the port specific OID request handler
 * for processing
 *
 */
NDIS_STATUS
Port11HandleDirectOidRequest(
    __in  PMP_PORT                Port,
    __in  PNDIS_OID_REQUEST       NdisOidRequest
    );

VOID
Port11CompletePendingDirectOidRequest(
    __in  PMP_PORT                Port,
    __in  PNDIS_OID_REQUEST       NdisOidRequest,
    __in  NDIS_STATUS             NdisStatus
    );

/**
 * This is called from the MP layer when NDIS calls MiniportSend to send packets on this port.
 * This gets forwarded to the BasePortHandleSendNetBufferLists handler. 
 * 
 * See BasePortHandleSendNetBufferLists for more information
 */
VOID
Port11HandleSendNetBufferLists(
    __in  PMP_PORT                Port,
    __in  PNET_BUFFER_LIST        NetBufferLists,
    __in  ULONG                   SendFlags
    );

/**
 * This is called by the base port for all OS submitted send packets to let a port
 * modify/reject packets before they are sent to the hardware. This is
 * called from the context of Port11HandleSendNetBufferLists or Port11SendCompletePackets.
 *
 * This function is invoked with the PORT Lock held.
 *
 * See BasePortHandleSendNetBufferLists for information on when this is called
 */
NDIS_STATUS
Port11NotifySend(
    __in  PMP_PORT                Port,
    __in  PMP_TX_MSDU             PacketList,
    __in  ULONG                   SendFlags
    );

/**
 * This is called by the HW when send packets are completed. This gets forwarded to 
 * the BasePortSendCompletePackets handler. 
 *
 * See BasePortSendCompletePackets for more information on what happens when this is called
 */

VOID 
Port11SendCompletePackets(
    __in  PMP_PORT                Port,
    __in  PMP_TX_MSDU             PacketList,
    __in  ULONG                   SendCompleteFlags
    );

/**
 * This is called by the base port on completion of all OS submitted send packets. This
 * allows a port to undo any actions it may have taken on the packets in Port11NotifySend.
 * This is called in the context of Port11SendCompletePackets
 * 
 * See BasePortSendCompletePackets for more information on when this is called 
 */
VOID
Port11NotifySendComplete(
    __in  PMP_PORT                Port,
    __in  PMP_TX_MSDU             PacketList,
    __in  ULONG                   SendCompleteFlags
    );

/**
 * This is called by the HW when receive packets are available for indication.
 * This gets forwarded to the BasePortIndicateReceivePackets handler. 
 *
 * See BasePortIndicateReceivePackets for more information on what happens 
 * when this is called
 */
VOID
Port11IndicateReceivePackets(
    __in  PMP_PORT                Port,
    __in  PMP_RX_MSDU             PacketList,
    __in  ULONG                   ReceiveFlags
    );

/**
 * This is called by the base port when it received packet indications from the HW.
 * This allows a port to reject any packets that it believes should not be indicated to the
 * OS. This is called in the context of Port11IndicateReceivePackets
 * 
 * See BasePortIndicateReceivePackets for more information on when this is called 
 */
NDIS_STATUS
Port11NotifyReceive(
    __in  PMP_PORT                Port,
    __in  PMP_RX_MSDU             PacketList,
    __in  ULONG                   ReceiveFlags
    );

/**
 * This is called by the MP layer when packets receive indicated on this port are returned 
 * by NDIS. This gets forwarded to the BasePortHandleReturnNetBufferLists handler.
 *
 * See BasePortHandleReturnNetBufferLists for more information on what happens 
 * when this is called
 */
VOID
Port11HandleReturnNetBufferLists(
    __in  PMP_PORT                Port,
    __in  PNET_BUFFER_LIST        NetBufferLists,
    __in  ULONG                   ReturnFlags
    );

/**
 * This is called by the base port on the return of all packets receive indicated to the OS. 
 * This allows a port to undo any actions it may have taken on the packets in 
 * Port11NotifyReceive. This is called in the context of Port11HandleReturnNetBufferLists
 * 
 * See BasePortHandleReturnNetBufferLists for more information on when this is called 
 */
VOID
Port11NotifyReturn(
    __in  PMP_PORT                Port,
    __in  PMP_RX_MSDU             PacketList,
    __in  ULONG                   ReturnFlags
    );

VOID
Port11IndicateStatus(
    __in  PMP_PORT                Port,
    __in  NDIS_STATUS             StatusCode,
    __in  PVOID                   StatusBuffer,
    __in  ULONG                   StatusBufferSize
    );

VOID
Port11Notify(
    __in  PMP_PORT            Port,
    __in  PVOID               Notif
);

