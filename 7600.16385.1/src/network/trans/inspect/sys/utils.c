/*++

Copyright (c) Microsoft Corporation. All rights reserved

Abstract:

   This file implements the utility/helper functions for use by the classify
   functions and worker thread of the Transport Inspect sample.

Environment:

    Kernel mode

--*/

#include "ntddk.h"

#pragma warning(push)
#pragma warning(disable:4201)       // unnamed struct/union

#include "fwpsk.h"

#pragma warning(pop)

#include "fwpmk.h"

#include "inspect.h"

#include "utils.h"


BOOLEAN IsAleReauthorize(
   IN const FWPS_INCOMING_VALUES0* inFixedValues
   )
{
   UINT flagsIndex;

   GetFlagsIndexesForLayer(
      inFixedValues->layerId,
      &flagsIndex
      );

   if((flagsIndex != UINT_MAX) && ((inFixedValues->incomingValue\
      [flagsIndex].value.uint32 & FWP_CONDITION_FLAG_IS_REAUTHORIZE) != 0))
   {
      return TRUE;
   }

   return FALSE;
}

BOOLEAN IsSecureConnection(
   IN const FWPS_INCOMING_VALUES0* inFixedValues
   )
{
   UINT flagsIndex;

   GetFlagsIndexesForLayer(
      inFixedValues->layerId,
      &flagsIndex
      );

   if ((flagsIndex != UINT_MAX) && ((inFixedValues->incomingValue\
       [flagsIndex].value.uint32 & FWP_CONDITION_FLAG_IS_IPSEC_SECURED) != 0))
   {
      return TRUE;
   }

   return FALSE;
}

BOOLEAN
IsAleClassifyRequired(
   IN const FWPS_INCOMING_VALUES0* inFixedValues,
   IN const FWPS_INCOMING_METADATA_VALUES0* inMetaValues
   )
{
   //
   // Note that use of FWP_CONDITION_FLAG_REQUIRES_ALE_CLASSIFY has been
   // deprecated in Vista SP1 and Windows Server 2008.
   //
#if (NTDDI_VERSION >= NTDDI_WIN6SP1)
   UNREFERENCED_PARAMETER(inFixedValues);
   return
   FWPS_IS_METADATA_FIELD_PRESENT(inMetaValues,
                                  FWPS_METADATA_FIELD_ALE_CLASSIFY_REQUIRED);
#else
   UINT flagsIndex;

   GetFlagsIndexesForLayer(
      inFixedValues->layerId,
      &flagsIndex
      );

   if ((flagsIndex != UINT_MAX) && ((inFixedValues->incomingValue\
       [flagsIndex].value.uint32 & FWP_CONDITION_FLAG_REQUIRES_ALE_CLASSIFY) != 0))
   {
      return TRUE;
   }

   return FALSE;

#endif
}

BOOLEAN
IsMatchingConnectPacket(
   IN const FWPS_INCOMING_VALUES0* inFixedValues,
   IN ADDRESS_FAMILY addressFamily,
   FWP_DIRECTION direction,
   IN TL_INSPECT_PENDED_PACKET* pendedPacket
   )
{
   UINT localAddrIndex;
   UINT remoteAddrIndex;
   UINT localPortIndex;
   UINT remotePortIndex;
   UINT protocolIndex;

   ASSERT(pendedPacket->type == TL_INSPECT_CONNECT_PACKET);

   GetNetwork5TupleIndexesForLayer(
      inFixedValues->layerId,
      &localAddrIndex,
      &remoteAddrIndex,
      &localPortIndex,
      &remotePortIndex,
      &protocolIndex
      );

   if(localAddrIndex == UINT_MAX)
   {
      return FALSE;
   }

   if (addressFamily != pendedPacket->addressFamily)
   {
      return FALSE;
   }

   if (direction != pendedPacket->direction)
   {
      return FALSE;
   }

   if (inFixedValues->incomingValue[protocolIndex].value.uint8 != 
       pendedPacket->protocol)
   {
      return FALSE;
   }

   if (RtlUshortByteSwap(
         inFixedValues->incomingValue[localPortIndex].value.uint16
         ) != pendedPacket->localPort)
   {
      return FALSE;
   }

   if (RtlUshortByteSwap(
         inFixedValues->incomingValue[remotePortIndex].value.uint16
         ) != pendedPacket->remotePort)
   {
      return FALSE;
   }

   if (addressFamily == AF_INET)
   {
      UINT32 ipv4LocalAddr = 
         RtlUlongByteSwap(
            inFixedValues->incomingValue[localAddrIndex].value.uint32
            );
      UINT32 ipv4RemoteAddr = 
         RtlUlongByteSwap( /* host-order -> network-order conversion */
            inFixedValues->incomingValue[remoteAddrIndex].value.uint32
            );
      if (ipv4LocalAddr != pendedPacket->ipv4LocalAddr)
      {
         return FALSE;
      }

      if (ipv4RemoteAddr != pendedPacket->ipv4RemoteAddr)
      {
         return FALSE;
      }
   }
   else
   {
      if (RtlCompareMemory(
            inFixedValues->incomingValue[localAddrIndex].value.byteArray16, 
            &pendedPacket->localAddr,
            sizeof(FWP_BYTE_ARRAY16)) !=  sizeof(FWP_BYTE_ARRAY16))
      {
         return FALSE;
      }

      if (RtlCompareMemory(
            inFixedValues->incomingValue[remoteAddrIndex].value.byteArray16, 
            &pendedPacket->remoteAddr,
            sizeof(FWP_BYTE_ARRAY16)) !=  sizeof(FWP_BYTE_ARRAY16))
      {
         return FALSE;
      }
   }

   return TRUE;
}

void
FillNetwork5Tuple(
   IN const FWPS_INCOMING_VALUES0* inFixedValues,
   IN ADDRESS_FAMILY addressFamily,
   IN OUT TL_INSPECT_PENDED_PACKET* packet
   )
{
   UINT localAddrIndex;
   UINT remoteAddrIndex;
   UINT localPortIndex;
   UINT remotePortIndex;
   UINT protocolIndex;

   GetNetwork5TupleIndexesForLayer(
      inFixedValues->layerId,
      &localAddrIndex,
      &remoteAddrIndex,
      &localPortIndex,
      &remotePortIndex,
      &protocolIndex
      );

   if (addressFamily == AF_INET)
   {
      packet->ipv4LocalAddr = 
         RtlUlongByteSwap( /* host-order -> network-order conversion */
            inFixedValues->incomingValue[localAddrIndex].value.uint32
            );
      packet->ipv4RemoteAddr = 
         RtlUlongByteSwap( /* host-order -> network-order conversion */
            inFixedValues->incomingValue[remoteAddrIndex].value.uint32
            );
   }
   else
   {
      RtlCopyMemory(
         (UINT8*)&packet->localAddr,
         inFixedValues->incomingValue[localAddrIndex].value.byteArray16,
         sizeof(FWP_BYTE_ARRAY16)
         );
      RtlCopyMemory(
         (UINT8*)&packet->remoteAddr,
         inFixedValues->incomingValue[remoteAddrIndex].value.byteArray16,
         sizeof(FWP_BYTE_ARRAY16)
         );
   }

   packet->localPort = 
      RtlUshortByteSwap(
         inFixedValues->incomingValue[localPortIndex].value.uint16
         );
   packet->remotePort = 
      RtlUshortByteSwap(
         inFixedValues->incomingValue[remotePortIndex].value.uint16
         );

   packet->protocol = inFixedValues->incomingValue[protocolIndex].value.uint8;

   return;
}

void
FreePendedPacket(
   IN OUT TL_INSPECT_PENDED_PACKET* packet
   )
{
   if (packet->netBufferList != NULL)
   {
      FwpsDereferenceNetBufferList0(packet->netBufferList, FALSE);
   }
   if (packet->controlData != NULL)
   {
      ExFreePoolWithTag(packet->controlData, TL_INSPECT_CONTROL_DATA_POOL_TAG);
   }
   if (packet->completionContext != NULL)
   {
      ASSERT(packet->type == TL_INSPECT_CONNECT_PACKET);
      ASSERT(packet->direction == FWP_DIRECTION_INBOUND); // complete for ALE connect
                                                          // is done prior to freeing
                                                          // of the packet.
      FwpsCompleteOperation0(packet->completionContext, NULL);
   }
   ExFreePoolWithTag(packet, TL_INSPECT_PENDED_PACKET_POOL_TAG);
}

TL_INSPECT_PENDED_PACKET*
AllocateAndInitializePendedPacket(
   IN const FWPS_INCOMING_VALUES0* inFixedValues,
   IN const FWPS_INCOMING_METADATA_VALUES0* inMetaValues,
   IN ADDRESS_FAMILY addressFamily,
   IN OUT void* layerData,
   IN TL_INSPECT_PACKET_TYPE packetType,
   IN FWP_DIRECTION packetDirection
   )
{
   TL_INSPECT_PENDED_PACKET* pendedPacket;

   // pendedPacket gets deleted in FreePendedPacket
   #pragma warning( suppress : 28197 )
   pendedPacket = ExAllocatePoolWithTag(
                        NonPagedPool,
                        sizeof(TL_INSPECT_PENDED_PACKET),
                        TL_INSPECT_PENDED_PACKET_POOL_TAG
                        );

   if (pendedPacket == NULL)
   {
      return NULL;
   }

   RtlZeroMemory(pendedPacket, sizeof(TL_INSPECT_PENDED_PACKET));

   pendedPacket->type = packetType;
   pendedPacket->direction = packetDirection;

   pendedPacket->addressFamily = addressFamily;

   FillNetwork5Tuple(
      inFixedValues,
      addressFamily,
      pendedPacket
      );

   if (layerData != NULL)
   {
      pendedPacket->netBufferList = layerData;

      //
      // Reference the net buffer list to make it accessible outside of 
      // classifyFn.
      //
      FwpsReferenceNetBufferList0(pendedPacket->netBufferList, TRUE);
   }

   ASSERT(FWPS_IS_METADATA_FIELD_PRESENT(inMetaValues, 
                                         FWPS_METADATA_FIELD_COMPARTMENT_ID));
   pendedPacket->compartmentId = inMetaValues->compartmentId;

   if ((pendedPacket->direction == FWP_DIRECTION_OUTBOUND) &&
       (layerData != NULL))
   {
      ASSERT(FWPS_IS_METADATA_FIELD_PRESENT(
                  inMetaValues, 
                  FWPS_METADATA_FIELD_TRANSPORT_ENDPOINT_HANDLE));
      pendedPacket->endpointHandle = inMetaValues->transportEndpointHandle;

      pendedPacket->remoteScopeId = inMetaValues->remoteScopeId;

      if (FWPS_IS_METADATA_FIELD_PRESENT(
            inMetaValues, 
            FWPS_METADATA_FIELD_TRANSPORT_CONTROL_DATA))
      {
         ASSERT(inMetaValues->controlDataLength > 0);

         // pendedPacket->controlData gets deleted in FreePendedPacket
         #pragma warning( suppress : 28197 )
         pendedPacket->controlData = ExAllocatePoolWithTag(
                                       NonPagedPool,
                                       inMetaValues->controlDataLength,
                                       TL_INSPECT_CONTROL_DATA_POOL_TAG
                                       );
         if (pendedPacket->controlData == NULL)
         {
            goto Exit;
         }

         RtlCopyMemory(
            pendedPacket->controlData,
            inMetaValues->controlData,
            inMetaValues->controlDataLength
            );

         pendedPacket->controlDataLength =  inMetaValues->controlDataLength;
      }
   }
   else if (pendedPacket->direction == FWP_DIRECTION_INBOUND)
   {
      UINT interfaceIndexIndex;
      UINT subInterfaceIndexIndex;

      GetDeliveryInterfaceIndexesForLayer(
         inFixedValues->layerId,
         &interfaceIndexIndex,
         &subInterfaceIndexIndex
         );

      pendedPacket->interfaceIndex = 
         inFixedValues->incomingValue[interfaceIndexIndex].value.uint32;
      pendedPacket->subInterfaceIndex = 
         inFixedValues->incomingValue[subInterfaceIndexIndex].value.uint32;
      
      ASSERT(FWPS_IS_METADATA_FIELD_PRESENT(
               inMetaValues, 
               FWPS_METADATA_FIELD_IP_HEADER_SIZE));
      ASSERT(FWPS_IS_METADATA_FIELD_PRESENT(
               inMetaValues, 
               FWPS_METADATA_FIELD_TRANSPORT_HEADER_SIZE));
      pendedPacket->ipHeaderSize = inMetaValues->ipHeaderSize;
      pendedPacket->transportHeaderSize = inMetaValues->transportHeaderSize;

      if (pendedPacket->netBufferList != NULL)
      {
         FWPS_PACKET_LIST_INFORMATION0 packetInfo = {0};
         FwpsGetPacketListSecurityInformation0(
            pendedPacket->netBufferList,
            FWPS_PACKET_LIST_INFORMATION_QUERY_IPSEC |
            FWPS_PACKET_LIST_INFORMATION_QUERY_INBOUND,
            &packetInfo
            );

         pendedPacket->ipSecProtected = 
            (BOOLEAN)packetInfo.ipsecInformation.inbound.isSecure;

         pendedPacket->nblOffset = 
            NET_BUFFER_DATA_OFFSET(\
               NET_BUFFER_LIST_FIRST_NB(pendedPacket->netBufferList));
      }
   }

   return pendedPacket;

Exit:

   if (pendedPacket != NULL)
   {
      FreePendedPacket(pendedPacket);
   }

   return NULL;
}

extern UCHAR gRegValueStorage[];

BOOLEAN
IsTrafficPermitted()
{
   NTSTATUS status;

   UNICODE_STRING valueName;
   KEY_VALUE_PARTIAL_INFORMATION* regValue = 
      (KEY_VALUE_PARTIAL_INFORMATION*)gRegValueStorage;
   ULONG resultLength;

   BOOLEAN permitTraffic = TRUE;

   RtlInitUnicodeString(
      &valueName,
      L"BlockTraffic"
      );

   status = ZwQueryValueKey(
               gRegistryKey,
               &valueName,
               KeyValuePartialInformation,
               regValue,
               sizeof(KEY_VALUE_PARTIAL_INFORMATION) + sizeof(ULONG),
               &resultLength
               );

   if (NT_SUCCESS(status))
   {
      if ((*(PULONG)regValue->Data) != 0)
      {
         permitTraffic = FALSE;
      }
   }

   return permitTraffic;
}


