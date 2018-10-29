/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    base_port_intf.h

Abstract:
    Contains interfaces into the base port
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    09-04-2007    Created

Notes:

--*/
#pragma once


NDIS_STATUS
BasePortAllocatePort(
    __in  PADAPTER                Adapter,
    __in  MP_PORT_TYPE            PortType,
    __deref_out_opt PMP_PORT*     Port
    );

VOID
BasePortFreePort(
    __in  PMP_PORT                Port
    );


NDIS_STATUS
BasePortInitializePort(
    __in  PMP_PORT                Port,
    __in  PHVL                    Hvl
    );

VOID
BasePortTerminatePort(
    __in  PMP_PORT                Port
    );

NDIS_STATUS
BasePortPausePort(
    __in  PMP_PORT                Port
    );

NDIS_STATUS
BasePortRestartPort(
    __in  PMP_PORT                Port
    );

NDIS_STATUS
BasePortResetPort(
    __in  PMP_PORT                Port,
    __in  PDOT11_RESET_REQUEST    Dot11ResetRequest
    );

VOID
BasePortInvalidatePort(
    __in  PMP_PORT                Port
    );
    
/**
 * This is the port OID handler for OIDs that are not specific to an op mode. This is
 * invoked from within the context of a Port11HandleOidRequest call by the mode specific
 * OID handler. eg. the Sta11OidHandler handler calls this when it receives a request
 * for OID_DOT11_RTS_THRESHOLD
 *
 * See mode specific OID handlers and Port11HandleOidRequest for more information
 */
NDIS_STATUS 
BasePortOidHandler(
    __in  PMP_PORT                Port,
    __in  PNDIS_OID_REQUEST       OidRequest
    );

NDIS_STATUS 
BasePortDirectOidHandler(
    __in  PMP_PORT                Port,
    __in  PNDIS_OID_REQUEST       OidRequest
    );

NDIS_STATUS
BasePortValidateScanRequest(
    __in  PMP_PORT                Port,
    __in  PDOT11_SCAN_REQUEST_V2  Dot11ScanRequest
    );


NDIS_STATUS
BasePortCopyBSSList(
    __in  PMP_PORT                Port,
    __in  DOT11_COUNTRY_OR_REGION_STRING  CountryRegionString,
    __in  ULONG                   ExpireTime,          // Max entry age in 100 nano-seconds
    __inout PDOT11_BYTE_ARRAY    Dot11ByteArray,
    __in  ULONG                   TotalLength    
    );



// Oid level helper functions
ULONG
BasePortGetPhyIdFromType(
    __in  PMP_PORT                Port,
    __in  DOT11_PHY_TYPE          PhyType
    );

DOT11_PHY_TYPE
BasePortGetPhyTypeFromId(
    __in  PMP_PORT                Port,
    __in  ULONG                   PhyId
    );


/**
 * This is the default Send notification handler that is invoked by Port11NotifySend.
 * This is used by ports that are not interested in being notified on a send
 */
NDIS_STATUS 
BasePortSendEventHandler(
    __in  PMP_PORT                Port,
    __in  PMP_TX_MSDU             PacketList,
    __in  ULONG                   SendFlags
    );

/**
 * This is the default Send complete notification handler that is invoked by Port11NotifySendComplete.
 * This is used by ports that are not interested in being notified on a send complete
 */
VOID 
BasePortSendCompleteEventHandler(
    __in  PMP_PORT                Port,
    __in  PMP_TX_MSDU             PacketList,
    __in  ULONG                   SendCompleteFlags
    );

/**
 * This is the default receive notification handler that is invoked by Port11NotifyReceive
 * This is used by ports that are not interested in being notified on a receive
 */
NDIS_STATUS 
BasePortReceiveEventHandler(
    __in  PMP_PORT                Port,
    __in  PMP_RX_MSDU             PacketList,
    __in  ULONG                   ReceiveFlags
    );

/**
 * This is the default return notification handler that is invoked by Port11NotifyReturn
 * This is used by ports that are not interested in being notified on a return
 */
VOID 
BasePortReturnEventHandler(
    __in  PMP_PORT                Port,
    __in  PMP_RX_MSDU             PacketList,
    __in  ULONG                   ReturnFlags
    );


/**
 * Used to convert the NET_BUFFER_LISTs to TX_PACKETs that
 * we can process
 */
NDIS_STATUS
BasePortTranslateTxNBLsToTxPackets(
    __in  PMP_PORT                Port,
    __in  PNET_BUFFER_LIST        NetBufferLists,
    __out PMP_TX_MSDU  *          PacketList
    );

/**
 * Used to free the TX_PACKETs 
 * allocated via BasePortTranslateTxNBLsToTxPackets 
 */
VOID
BasePortFreeTranslatedTxPackets(
    __in  PMP_PORT                Port,
    __out PMP_TX_MSDU             PacketList
    );

/**
 * Used to convert the RX_PACKETs to NET_BUFFER_LISTS that
 * we can pass to the OS
 */
NDIS_STATUS
BasePortTranslateRxPacketsToRxNBLs(
    __in  PMP_PORT                Port,
    __in  PMP_RX_MSDU             PacketList,
    __out PNET_BUFFER_LIST*       NetBufferLists
    );

/**
 * Used to free the NET_BUFFER_LISTs
 * allocated via BasePortTranslateRxPacketsToRxNBLs 
 */
VOID
BasePortFreeTranslatedRxNBLs(
    __in  PMP_PORT                Port,
    __out PNET_BUFFER_LIST        NetBufferLists
    );


/**
 * This is used to send packets originated from within the driver. This can be used
 * by the ports to send managements packets (association request, responses, etc).
 * The caller can reclaim memory used for the packet data as soon as the call returns.
 * The packets are sent asynchronously.
 *
 * These packets are forwarded to the hardware without notifying the port ie. without
 * the port's SendEvent handler getting invoked.
 * 
 * Caller of this API needs to ensure that this call returns before
 * a pause is initiated
 *
 * This API would be extended in the future to allow the caller to get notified
 * when the packet has been send completed and to specify the priority, rate etc
 * for the packets.
 */
NDIS_STATUS
BasePortSendInternalPacket(
    __in  PMP_PORT                Port,
    __in_bcount(PacketLength)  PUCHAR                  PacketData,
    __in  USHORT                  PacketLength
    );

/**
 * This is invoked from the Port11HandleSendNetBufferLists handler. This is the
 * main handler for processing send packets from the OS. 
 * 
 * The sequence of actions performed here are:
 * - Convert all the NET_BUFFER_LISTs to MP_TX_MSDUs
 * - Notify the port about the transmission by calling Port11NotifySend
 * - Depending on the port's response the MP_TX_MSDUs are either queued for later
 *   sending or passed to the VNIC or dropped
 */
VOID
BasePortHandleSendNetBufferLists(
    __in  PMP_PORT                Port,
    __in  PNET_BUFFER_LIST        NetBufferLists,
    __in  ULONG                   SendFlags
    );

/**
 * This is invoked from the Port11SendCompletePackets handler when packets are
 * send completed by the VNIC. This is called for both packets sent from 
 * BasePortSendInternalPacket and BasePortHandleSendNetBufferLists.
 *
 * The sequence of actions performed here are:
 * - Notify the port about the completion by calling Port11NotifySendComplete
 * - Recover the NET_BUFFER_LISTs that originally submitted by the OS and complete
 *   them back to the OS
 * - If there are unsubmitted packets pending in the send queue, attempt to 
 *   submit them to the hardware, repeating the steps from BasePortHandleSendNetBufferLists
 *
 * For internal packets that were sent using BasePortSendInternalPacket, none of the
 * above steps are taken.
 */
VOID
BasePortSendCompletePackets(
    __in  PMP_PORT                Port,
    __in  PMP_TX_MSDU             PacketList,
    __in  ULONG                   SendCompleteFlags
    );


/**
 * This is invoked from the Port11IndicateReceivePackets handler. This is the
 * handler for processing receive indications from the HW
 * 
 * The sequence of actions performed here are:
 * - Notify the port about the reception of these packets by calling Port11NotifyReceive
 * - Depending on the port's response the MP_RX_MSDUs are either returned to the HW
 *   or considered for forwarding up the OS
 * - Filter out packets that shouldnt be indicated up based on the packet filter set by NDIS
 * - Convert the receive indicate candidate MP_RX_MSDUs to NET_BUFFER_LISTs and indicate
 *   to NDIS. These packets would be returned through BasePortHandleReturnNetBufferLists
 *
 */
VOID
BasePortIndicateReceivePackets(
    __in  PMP_PORT                Port,
    __in  PMP_RX_MSDU             PacketList,
    __in  ULONG                   ReceiveFlags
    );

/**
 * This is invoked from the Port11HandleReturnNetBufferLists handler when packets are
 * returned to the miniport by NDIS. This is called for packets that were successfully
 * indicated up by BasePortIndicateReceivePackets.
 *
 * The sequence of actions performed here are:
 * - Recover the MP_RX_MSDU that originally receive indicated by the HW
 * - Notify the port about the return by calling Port11NotifyReturn
 * - Return all the packets to the HW
 *
 */
VOID
BasePortHandleReturnNetBufferLists(
    __in  PMP_PORT                Port,
    __in  PNET_BUFFER_LIST        NetBufferLists,
    __in  ULONG                   ReturnFlags
    );

