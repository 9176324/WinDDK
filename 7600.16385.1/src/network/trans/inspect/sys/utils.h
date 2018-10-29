/*++

Copyright (c) Microsoft Corporation. All rights reserved

Abstract:

   This file declares the utility/helper functions for use by the classify
   functions and worker thread of the Transport Inspect sample.

Environment:

    Kernel mode

--*/

#include "limits.h"

#ifndef _TL_INSPECT_UTILS_H_
#define _TL_INSPECT_UTILS_H_

__inline
ADDRESS_FAMILY GetAddressFamilyForLayer(
   IN UINT16 layerId
   )
{
   ADDRESS_FAMILY addressFamily;

   switch (layerId)
   {
   case FWPS_LAYER_ALE_AUTH_CONNECT_V4:
   case FWPS_LAYER_ALE_AUTH_RECV_ACCEPT_V4:
   case FWPS_LAYER_OUTBOUND_TRANSPORT_V4:
   case FWPS_LAYER_INBOUND_TRANSPORT_V4:
      addressFamily = AF_INET;
      break;
   case FWPS_LAYER_ALE_AUTH_CONNECT_V6:
   case FWPS_LAYER_ALE_AUTH_RECV_ACCEPT_V6:
   case FWPS_LAYER_OUTBOUND_TRANSPORT_V6:
   case FWPS_LAYER_INBOUND_TRANSPORT_V6:
      addressFamily = AF_INET6;
      break;
   default:
      addressFamily = AF_UNSPEC;
      ASSERT(0);
   }

   return addressFamily;
}

__inline
FWP_DIRECTION GetPacketDirectionForLayer(
   IN UINT16 layerId
   )
{
   FWP_DIRECTION direction;

   switch (layerId)
   {
   case FWPS_LAYER_OUTBOUND_TRANSPORT_V4:
   case FWPS_LAYER_OUTBOUND_TRANSPORT_V6:
      direction = FWP_DIRECTION_OUTBOUND;
      break;
   case FWPS_LAYER_INBOUND_TRANSPORT_V4:
   case FWPS_LAYER_INBOUND_TRANSPORT_V6:
      direction = FWP_DIRECTION_INBOUND;
      break;
   default:
      direction = FWP_DIRECTION_MAX;
      ASSERT(0);
   }

   return direction;
}

__inline
void
GetFlagsIndexesForLayer(
   IN UINT16 layerId,
   OUT UINT* flagsIndex
   )
{
   switch (layerId)
   {
   case FWPS_LAYER_ALE_AUTH_CONNECT_V4:
      *flagsIndex = FWPS_FIELD_ALE_AUTH_CONNECT_V4_FLAGS;
      break;
   case FWPS_LAYER_ALE_AUTH_CONNECT_V6:
      *flagsIndex = FWPS_FIELD_ALE_AUTH_CONNECT_V6_FLAGS;
      break;
   case FWPS_LAYER_ALE_AUTH_RECV_ACCEPT_V4:
      *flagsIndex = FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V4_FLAGS;
      break;
   case FWPS_LAYER_ALE_AUTH_RECV_ACCEPT_V6:
      *flagsIndex = FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V6_FLAGS;
      break;
   case FWPS_LAYER_OUTBOUND_TRANSPORT_V4:
      *flagsIndex = FWPS_FIELD_OUTBOUND_TRANSPORT_V4_FLAGS;
      break;
   case FWPS_LAYER_OUTBOUND_TRANSPORT_V6:
      *flagsIndex = FWPS_FIELD_OUTBOUND_TRANSPORT_V6_FLAGS;
      break;
   case FWPS_LAYER_INBOUND_TRANSPORT_V4:
      *flagsIndex = FWPS_FIELD_INBOUND_TRANSPORT_V4_FLAGS;
      break;
   case FWPS_LAYER_INBOUND_TRANSPORT_V6:
      *flagsIndex = FWPS_FIELD_INBOUND_TRANSPORT_V6_FLAGS;
      break;
   default:
      *flagsIndex = UINT_MAX;
      ASSERT(0);
      break;
   }
}

__inline
void
GetDeliveryInterfaceIndexesForLayer(
   IN UINT16 layerId,
   OUT UINT* interfaceIndexIndex,
   OUT UINT* subInterfaceIndexIndex
   )
{
   switch (layerId)
   {
   case FWPS_LAYER_ALE_AUTH_CONNECT_V4:
#if (NTDDI_VERSION >= NTDDI_WIN6SP1)
      *interfaceIndexIndex = 
         FWPS_FIELD_ALE_AUTH_CONNECT_V4_INTERFACE_INDEX;
      *subInterfaceIndexIndex = 
         FWPS_FIELD_ALE_AUTH_CONNECT_V4_SUB_INTERFACE_INDEX;
#else
      //
      // Prior to Vista SP1, sub/interface-index are not being indicated to
      // ALE_AUTH_CONNECT layers. A callout driver would need to derive them
      // from the interface LUID using iphlpapi functions.
      //
      ASSERT(0);
#endif
      break;
   case FWPS_LAYER_ALE_AUTH_CONNECT_V6:
#if (NTDDI_VERSION >= NTDDI_WIN6SP1)
      *interfaceIndexIndex = 
         FWPS_FIELD_ALE_AUTH_CONNECT_V6_INTERFACE_INDEX;
      *subInterfaceIndexIndex = 
         FWPS_FIELD_ALE_AUTH_CONNECT_V6_SUB_INTERFACE_INDEX;
#else
      ASSERT(0);
#endif
      break;
   case FWPS_LAYER_ALE_AUTH_RECV_ACCEPT_V4:
      *interfaceIndexIndex = 
         FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V4_INTERFACE_INDEX;
      *subInterfaceIndexIndex = 
         FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V4_SUB_INTERFACE_INDEX;
      break;
   case FWPS_LAYER_ALE_AUTH_RECV_ACCEPT_V6:
      *interfaceIndexIndex = 
         FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V6_INTERFACE_INDEX;
      *subInterfaceIndexIndex = 
         FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V6_SUB_INTERFACE_INDEX;
      break;
   case FWPS_LAYER_INBOUND_TRANSPORT_V4:
      *interfaceIndexIndex = 
         FWPS_FIELD_INBOUND_TRANSPORT_V4_INTERFACE_INDEX;
      *subInterfaceIndexIndex = 
         FWPS_FIELD_INBOUND_TRANSPORT_V4_SUB_INTERFACE_INDEX;
      break;
   case FWPS_LAYER_INBOUND_TRANSPORT_V6:
      *interfaceIndexIndex = 
         FWPS_FIELD_INBOUND_TRANSPORT_V6_INTERFACE_INDEX;
      *subInterfaceIndexIndex = 
         FWPS_FIELD_INBOUND_TRANSPORT_V6_SUB_INTERFACE_INDEX;
      break;
   default:
      ASSERT(0);
      break;
   }
}

__inline
void
GetNetwork5TupleIndexesForLayer(
   IN UINT16 layerId,
   OUT UINT* localAddressIndex,
   OUT UINT* remoteAddressIndex,
   OUT UINT* localPortIndex,
   OUT UINT* remotePortIndex,
   OUT UINT* protocolIndex
   )
{
   switch (layerId)
   {
   case FWPS_LAYER_ALE_AUTH_CONNECT_V4:
      *localAddressIndex = FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_LOCAL_ADDRESS;
      *remoteAddressIndex = FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_REMOTE_ADDRESS;
      *localPortIndex = FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_LOCAL_PORT;
      *remotePortIndex = FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_REMOTE_PORT;
      *protocolIndex = FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_PROTOCOL;
      break;
   case FWPS_LAYER_ALE_AUTH_CONNECT_V6:
      *localAddressIndex = FWPS_FIELD_ALE_AUTH_CONNECT_V6_IP_LOCAL_ADDRESS;
      *remoteAddressIndex = FWPS_FIELD_ALE_AUTH_CONNECT_V6_IP_REMOTE_ADDRESS;
      *localPortIndex = FWPS_FIELD_ALE_AUTH_CONNECT_V6_IP_LOCAL_PORT;
      *remotePortIndex = FWPS_FIELD_ALE_AUTH_CONNECT_V6_IP_REMOTE_PORT;
      *protocolIndex = FWPS_FIELD_ALE_AUTH_CONNECT_V6_IP_PROTOCOL;
      break;
   case FWPS_LAYER_ALE_AUTH_RECV_ACCEPT_V4:
      *localAddressIndex = FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V4_IP_LOCAL_ADDRESS;
      *remoteAddressIndex = FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V4_IP_REMOTE_ADDRESS;
      *localPortIndex = FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V4_IP_LOCAL_PORT;
      *remotePortIndex = FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V4_IP_REMOTE_PORT;
      *protocolIndex = FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V4_IP_PROTOCOL;
      break;
   case FWPS_LAYER_ALE_AUTH_RECV_ACCEPT_V6:
      *localAddressIndex = FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V6_IP_LOCAL_ADDRESS;
      *remoteAddressIndex = FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V6_IP_REMOTE_ADDRESS;
      *localPortIndex = FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V6_IP_LOCAL_PORT;
      *remotePortIndex = FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V6_IP_REMOTE_PORT;
      *protocolIndex = FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V6_IP_PROTOCOL;
      break;
   case FWPS_LAYER_OUTBOUND_TRANSPORT_V4:
      *localAddressIndex = FWPS_FIELD_OUTBOUND_TRANSPORT_V4_IP_LOCAL_ADDRESS;
      *remoteAddressIndex = FWPS_FIELD_OUTBOUND_TRANSPORT_V4_IP_REMOTE_ADDRESS;
      *localPortIndex = FWPS_FIELD_OUTBOUND_TRANSPORT_V4_IP_LOCAL_PORT;
      *remotePortIndex = FWPS_FIELD_OUTBOUND_TRANSPORT_V4_IP_REMOTE_PORT;
      *protocolIndex = FWPS_FIELD_OUTBOUND_TRANSPORT_V4_IP_PROTOCOL;
      break;
   case FWPS_LAYER_OUTBOUND_TRANSPORT_V6:
      *localAddressIndex = FWPS_FIELD_OUTBOUND_TRANSPORT_V6_IP_LOCAL_ADDRESS;
      *remoteAddressIndex = FWPS_FIELD_OUTBOUND_TRANSPORT_V6_IP_REMOTE_ADDRESS;
      *localPortIndex = FWPS_FIELD_OUTBOUND_TRANSPORT_V6_IP_LOCAL_PORT;
      *remotePortIndex = FWPS_FIELD_OUTBOUND_TRANSPORT_V6_IP_REMOTE_PORT;
      *protocolIndex = FWPS_FIELD_OUTBOUND_TRANSPORT_V6_IP_PROTOCOL;
      break;
   case FWPS_LAYER_INBOUND_TRANSPORT_V4:
      *localAddressIndex = FWPS_FIELD_INBOUND_TRANSPORT_V4_IP_LOCAL_ADDRESS;
      *remoteAddressIndex = FWPS_FIELD_INBOUND_TRANSPORT_V4_IP_REMOTE_ADDRESS;
      *localPortIndex = FWPS_FIELD_INBOUND_TRANSPORT_V4_IP_LOCAL_PORT;
      *remotePortIndex = FWPS_FIELD_INBOUND_TRANSPORT_V4_IP_REMOTE_PORT;
      *protocolIndex = FWPS_FIELD_INBOUND_TRANSPORT_V4_IP_PROTOCOL;
      break;
   case FWPS_LAYER_INBOUND_TRANSPORT_V6:
      *localAddressIndex = FWPS_FIELD_INBOUND_TRANSPORT_V6_IP_LOCAL_ADDRESS;
      *remoteAddressIndex = FWPS_FIELD_INBOUND_TRANSPORT_V6_IP_REMOTE_ADDRESS;
      *localPortIndex = FWPS_FIELD_INBOUND_TRANSPORT_V6_IP_LOCAL_PORT;
      *remotePortIndex = FWPS_FIELD_INBOUND_TRANSPORT_V6_IP_REMOTE_PORT;
      *protocolIndex = FWPS_FIELD_INBOUND_TRANSPORT_V6_IP_PROTOCOL;
      break;
   default:
      *localAddressIndex = UINT_MAX;
      *remoteAddressIndex = UINT_MAX;
      *localPortIndex = UINT_MAX;
      *remotePortIndex = UINT_MAX;
      *protocolIndex = UINT_MAX;      
      ASSERT(0);
   }
}

BOOLEAN IsAleReauthorize(
   IN const FWPS_INCOMING_VALUES0* inFixedValues
   );

BOOLEAN IsSecureConnection(
   IN const FWPS_INCOMING_VALUES0* inFixedValues
   );

BOOLEAN
IsAleClassifyRequired(
   IN const FWPS_INCOMING_VALUES0* inFixedValues,
   IN const FWPS_INCOMING_METADATA_VALUES0* inMetaValues
   );

void
FillNetwork5Tuple(
   IN const FWPS_INCOMING_VALUES0* inFixedValues,
   IN ADDRESS_FAMILY addressFamily,
   IN OUT TL_INSPECT_PENDED_PACKET* packet
   );

BOOLEAN
IsMatchingConnectPacket(
   IN const FWPS_INCOMING_VALUES0* inFixedValues,
   IN ADDRESS_FAMILY addressFamily,
   FWP_DIRECTION direction,
   IN TL_INSPECT_PENDED_PACKET* pendedPacket
   );

TL_INSPECT_PENDED_PACKET*
AllocateAndInitializePendedPacket(
   IN const FWPS_INCOMING_VALUES0* inFixedValues,
   IN const FWPS_INCOMING_METADATA_VALUES0* inMetaValues,
   IN ADDRESS_FAMILY addressFamily,
   IN OUT void* layerData,
   IN TL_INSPECT_PACKET_TYPE packetType,
   IN FWP_DIRECTION packetDirection
   );

void
FreePendedPacket(
   IN OUT TL_INSPECT_PENDED_PACKET* packet
   );

BOOLEAN
IsTrafficPermitted();

#endif // _TL_INSPECT_UTILS_H_

