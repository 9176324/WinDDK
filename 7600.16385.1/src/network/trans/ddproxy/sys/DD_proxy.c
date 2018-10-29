/*++

Copyright (c) Microsoft Corporation. All rights reserved

Abstract:

   This file implements the classifyFn, notifiFn, and flowDeleteFn callout
   functions for the flow-established and datagram-data callouts. In addition
   the system worker thread that performs the actual packet modifications
   is also implemented here along with the eventing mechanisms shared between
   the classify function and the worker thread.

   Packet modification is done out-of-band by a system worker thread using 
   the reference-drop-clone-modify-reinject mechanism. Therefore the sample 
   can serve as a base in scenarios where filtering/modification decision 
   cannot be made within the classifyFn() callout and instead must be made, 
   for example, by an user-mode application.

Environment:

    Kernel mode

--*/

#include "ntddk.h"

#pragma warning(push)
#pragma warning(disable:4201)       // unnamed struct/union

#include "fwpsk.h"

#pragma warning(pop)

#include "fwpmk.h"

#include "DD_proxy.h"

__inline
void
DDProxyFreePendedPacket(
   IN OUT DD_PROXY_PENDED_PACKET* packet
   )
{
   FwpsDereferenceNetBufferList0(packet->netBufferList, FALSE);
   DDProxyDereferenceFlowContext(packet->belongingFlow);
   if (packet->controlData != NULL)
   {
      ExFreePoolWithTag(packet->controlData, DD_PROXY_CONTROL_DATA_POOL_TAG);
   }
   ExFreePoolWithTag(packet, DD_PROXY_PENDED_PACKET_POOL_TAG);
}

void
DDProxyFlowEstablishedClassify(
   IN const FWPS_INCOMING_VALUES0* inFixedValues,
   IN const FWPS_INCOMING_METADATA_VALUES0* inMetaValues,
   IN OUT void* layerData,
   IN const FWPS_FILTER0* filter,
   IN UINT64 flowContext,
   OUT FWPS_CLASSIFY_OUT0* classifyOut
   )
/* ++

   This is the classifyFn function of the flow-established callout. It 
   allocates flow context for the original and the proxy flow and associates 
   them with the indicated flow-id. This function also stores information 
   common to both flows in the context. The flow context is inserted into the 
   global flow list.

-- */
{
   NTSTATUS status = STATUS_SUCCESS;

   BOOLEAN locked = FALSE;

   KLOCK_QUEUE_HANDLE flowListLockHandle;

   DD_PROXY_FLOW_CONTEXT* flowContextLocal = NULL;

   UNREFERENCED_PARAMETER(flowContext);
   UNREFERENCED_PARAMETER(layerData);


   flowContextLocal = ExAllocatePoolWithTag(
                        NonPagedPool,
                        sizeof(DD_PROXY_FLOW_CONTEXT),
                        DD_PROXY_FLOW_CONTEXT_POOL_TAG
                        );

   if (flowContextLocal == NULL)
   {
      status = STATUS_NO_MEMORY;
      goto Exit;
   }

   RtlZeroMemory(flowContextLocal, sizeof(DD_PROXY_FLOW_CONTEXT));

   flowContextLocal->refCount = 1;
   flowContextLocal->flowType = (DD_PROXY_FLOW_TYPE)(filter->context);
   flowContextLocal->addressFamily = 
      (inFixedValues->layerId == FWPS_LAYER_ALE_FLOW_ESTABLISHED_V4) ? 
         AF_INET : AF_INET6;
   ASSERT(FWPS_IS_METADATA_FIELD_PRESENT(inMetaValues, 
                                         FWPS_METADATA_FIELD_FLOW_HANDLE));
   flowContextLocal->flowId = inMetaValues->flowHandle;

   //
   // Note that since the consumer of the flow context is the datagram-data
   // layer classifyFn, layerId and calloutId are set to those of DD and not
   // flow-established.
   //
   flowContextLocal->layerId = 
      (flowContextLocal->addressFamily == AF_INET) ? 
         FWPS_LAYER_DATAGRAM_DATA_V4 : FWPS_LAYER_DATAGRAM_DATA_V6;
   flowContextLocal->calloutId = 
      (flowContextLocal->addressFamily == AF_INET) ? 
         gCalloutIdV4 : gCalloutIdV6;

   if (flowContextLocal->addressFamily == AF_INET)
   {
      flowContextLocal->ipv4LocalAddr = 
         RtlUlongByteSwap(
            inFixedValues->incomingValue\
            [FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_LOCAL_ADDRESS].value.uint32
            );
      flowContextLocal->protocol =
         inFixedValues->incomingValue\
         [FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_PROTOCOL].value.uint8;
   }
   else
   {
      RtlCopyMemory(
         (UINT8*)&flowContextLocal->localAddr,
         inFixedValues->incomingValue\
         [FWPS_FIELD_ALE_FLOW_ESTABLISHED_V6_IP_LOCAL_ADDRESS].value.byteArray16,
         sizeof(FWP_BYTE_ARRAY16)
         );
      flowContextLocal->protocol = 
         inFixedValues->incomingValue\
         [FWPS_FIELD_ALE_FLOW_ESTABLISHED_V6_IP_PROTOCOL].value.uint8;
   }

   if (flowContextLocal->flowType == DD_PROXY_FLOW_ORIGINAL)
   {
      flowContextLocal->toRemoteAddr = 
         (flowContextLocal->addressFamily == AF_INET) ? 
            configNewDestAddrV4 : configNewDestAddrV6;
      // host-order -> network-order conversion for port.
      flowContextLocal->toRemotePort = RtlUshortByteSwap(configNewDestPort);
   }
   else
   {
      ASSERT(flowContextLocal->flowType == DD_PROXY_FLOW_PROXY);
      flowContextLocal->toRemoteAddr = 
         (flowContextLocal->addressFamily == AF_INET) ? 
            configInspectDestAddrV4 : configInspectDestAddrV6;
      // host-order -> network-order conversion for port.
      flowContextLocal->toRemotePort = RtlUshortByteSwap(configInspectDestPort);
   }
   if ((flowContextLocal->toRemoteAddr != NULL) &&
       (flowContextLocal->addressFamily == AF_INET))
   {
      // host-order -> network-order conversion for Ipv4 address.
      flowContextLocal->ipv4NetworkOrderStorage = 
         RtlUlongByteSwap(*(ULONG*)(flowContextLocal->toRemoteAddr));
      flowContextLocal->toRemoteAddr = 
         (UINT8*)&flowContextLocal->ipv4NetworkOrderStorage;
   }

   KeAcquireInStackQueuedSpinLock(
      &gFlowListLock,
      &flowListLockHandle
      );

   locked = TRUE;

   if (!gDriverUnloading)
   {
      //
      // Associate DD_PROXY_FLOW_CONTEXT with the indicated flow-id to be 
      // accessible by the Datagram-Data classifyFn. (i.e. when a packet 
      // belongs to the same flow being classified at Datagram-Data layer,
      // DD_PROXY_FLOW_CONTEXT will be passed onto the classifyFn as the
      // "flowContext" parameter.
      //
      status = FwpsFlowAssociateContext0(
                  flowContextLocal->flowId,
                  flowContextLocal->layerId,
                  flowContextLocal->calloutId,
                  (UINT64)flowContextLocal
                  );
      if(!NT_SUCCESS(status))
      {
         goto Exit;
      }

      InsertHeadList(&gFlowList, &flowContextLocal->listEntry);
      flowContextLocal = NULL; // ownership transferred
   }

   classifyOut->actionType = FWP_ACTION_PERMIT;

Exit:

   if(locked)
   {
      KeReleaseInStackQueuedSpinLock(&flowListLockHandle);
   }

   if (flowContextLocal != NULL)
   {
      ExFreePoolWithTag(flowContextLocal, DD_PROXY_FLOW_CONTEXT_POOL_TAG);
   }

   if(!NT_SUCCESS(status))
   {
      classifyOut->actionType = FWP_ACTION_BLOCK;
      classifyOut->rights &= ~FWPS_RIGHT_ACTION_WRITE;
   }

   return;
}

NTSTATUS
DDProxyFlowEstablishedNotify(
   IN FWPS_CALLOUT_NOTIFY_TYPE notifyType,
   IN const GUID* filterKey,
   IN const FWPS_FILTER0* filter
   )
{
   UNREFERENCED_PARAMETER(notifyType);
   UNREFERENCED_PARAMETER(filterKey);
   UNREFERENCED_PARAMETER(filter);

   return STATUS_SUCCESS;
}

void
DDProxyClassify(
   IN const FWPS_INCOMING_VALUES0* inFixedValues,
   IN const FWPS_INCOMING_METADATA_VALUES0* inMetaValues,
   IN OUT void* layerData,
   IN const FWPS_FILTER0* filter,
   IN UINT64 flowContext,
   OUT FWPS_CLASSIFY_OUT0* classifyOut
   )
/* ++

   This is the classifyFn function of the datagram-data callout. It 
   allocates a packet structure to store the classify and meta data and 
   it references the net buffer list for out-of-band modification and 
   re-injection. The packet structure will be queued to the global packet 
   queue. The worker thread will then be signaled, if idle, to process 
   the queue. 

-- */
{
   DD_PROXY_PENDED_PACKET* packet = NULL;
   DD_PROXY_FLOW_CONTEXT* flowContextLocal = (DD_PROXY_FLOW_CONTEXT*)(DWORD_PTR)flowContext;

   FWPS_PACKET_INJECTION_STATE packetState;
   KLOCK_QUEUE_HANDLE packetQueueLockHandle;
   BOOLEAN signalWorkerThread;
   UNREFERENCED_PARAMETER(filter);

   //
   // We don't have the necessary right to alter the packet.
   //
   if ((classifyOut->rights & FWPS_RIGHT_ACTION_WRITE) == 0)
   {
      goto Exit;
   }

   //
   // We don't re-inspect packets that we've inspected earlier.
   //
   packetState = FwpsQueryPacketInjectionState0(
                  gInjectionHandle,
                  layerData,
                  NULL
                  );

   if ((packetState == FWPS_PACKET_INJECTED_BY_SELF) ||
       (packetState == FWPS_PACKET_PREVIOUSLY_INJECTED_BY_SELF))
   {
      classifyOut->actionType = FWP_ACTION_PERMIT;
      goto Exit;
   }


   // packet gets deleted in DDProxyFreePendedPacket
   #pragma warning( suppress : 28197 )
   packet = ExAllocatePoolWithTag(
                     NonPagedPool,
                     sizeof(DD_PROXY_PENDED_PACKET),
                     DD_PROXY_PENDED_PACKET_POOL_TAG
                     );

   if (packet == NULL)
   {
      classifyOut->actionType = FWP_ACTION_BLOCK;
      classifyOut->rights &= ~FWPS_RIGHT_ACTION_WRITE;
      goto Exit;
   }

   RtlZeroMemory(packet, sizeof(DD_PROXY_PENDED_PACKET));

   ASSERT(flowContextLocal != NULL);

   packet->belongingFlow = flowContextLocal;
   DDProxyReferenceFlowContext(packet->belongingFlow);
   if (flowContextLocal->addressFamily == AF_INET)
   {
      ASSERT(inFixedValues->layerId == FWPS_LAYER_DATAGRAM_DATA_V4);
      packet->direction = 
         inFixedValues->incomingValue[FWPS_FIELD_DATAGRAM_DATA_V4_DIRECTION].\
            value.uint32;
   }
   else
   {
      ASSERT(inFixedValues->layerId == FWPS_LAYER_DATAGRAM_DATA_V6);
      packet->direction = 
         inFixedValues->incomingValue[FWPS_FIELD_DATAGRAM_DATA_V6_DIRECTION].\
         value.uint32;
   }
   packet->netBufferList = layerData;

   //
   // Reference the net buffer list to make it accessible outside of 
   // classifyFn.
   //
   FwpsReferenceNetBufferList0(packet->netBufferList, TRUE);

   ASSERT(FWPS_IS_METADATA_FIELD_PRESENT(inMetaValues, 
                                         FWPS_METADATA_FIELD_COMPARTMENT_ID));
   packet->compartmentId = inMetaValues->compartmentId;

   if (packet->direction == FWP_DIRECTION_OUTBOUND)
   {
      ASSERT(FWPS_IS_METADATA_FIELD_PRESENT(
                  inMetaValues, 
                  FWPS_METADATA_FIELD_TRANSPORT_ENDPOINT_HANDLE));
      packet->endpointHandle = inMetaValues->transportEndpointHandle;

      if (flowContextLocal->addressFamily == AF_INET)
      {
         packet->ipv4RemoteAddr = 
            RtlUlongByteSwap( /* host-order -> network-order conversion */
               inFixedValues->incomingValue\
               [FWPS_FIELD_DATAGRAM_DATA_V4_IP_REMOTE_ADDRESS].value.uint32
               );
      }
      else
      {
         RtlCopyMemory(
            (UINT8*)&packet->remoteAddr,
            inFixedValues->incomingValue\
            [FWPS_FIELD_DATAGRAM_DATA_V6_IP_REMOTE_ADDRESS].value.byteArray16,
            sizeof(FWP_BYTE_ARRAY16)
            );

      }
      packet->remoteScopeId = inMetaValues->remoteScopeId;

      if (FWPS_IS_METADATA_FIELD_PRESENT(
            inMetaValues, 
            FWPS_METADATA_FIELD_TRANSPORT_CONTROL_DATA))
      {
         ASSERT(inMetaValues->controlDataLength > 0);

         // packet->controlData gets deleted in DDProxyFreePendedPacket
         #pragma warning( suppress : 28197 )
         packet->controlData = ExAllocatePoolWithTag(
                                 NonPagedPool,
                                 inMetaValues->controlDataLength,
                                 DD_PROXY_CONTROL_DATA_POOL_TAG
                                 );
         if (packet->controlData == NULL)
         {
            classifyOut->actionType = FWP_ACTION_BLOCK;
            classifyOut->rights &= ~FWPS_RIGHT_ACTION_WRITE;
            goto Exit;
         }

         RtlCopyMemory(
            packet->controlData,
            inMetaValues->controlData,
            inMetaValues->controlDataLength
            );

         packet->controlDataLength =  inMetaValues->controlDataLength;
      }
   }
   else
   {
      ASSERT(packet->direction == FWP_DIRECTION_INBOUND);

      if (flowContextLocal->addressFamily == AF_INET)
      {
         ASSERT(inFixedValues->layerId == FWPS_LAYER_DATAGRAM_DATA_V4);
         packet->interfaceIndex = 
            inFixedValues->incomingValue\
            [FWPS_FIELD_DATAGRAM_DATA_V4_INTERFACE_INDEX].value.uint32;
         packet->subInterfaceIndex = 
            inFixedValues->incomingValue\
            [FWPS_FIELD_DATAGRAM_DATA_V4_SUB_INTERFACE_INDEX].value.uint32;
      }
      else
      {
         ASSERT(inFixedValues->layerId == FWPS_LAYER_DATAGRAM_DATA_V6);
         packet->interfaceIndex = 
            inFixedValues->incomingValue\
            [FWPS_FIELD_DATAGRAM_DATA_V6_INTERFACE_INDEX].value.uint32;
         packet->subInterfaceIndex = 
            inFixedValues->incomingValue\
            [FWPS_FIELD_DATAGRAM_DATA_V6_SUB_INTERFACE_INDEX].value.uint32;
      }
      
      ASSERT(FWPS_IS_METADATA_FIELD_PRESENT(
               inMetaValues, 
               FWPS_METADATA_FIELD_IP_HEADER_SIZE));
      ASSERT(FWPS_IS_METADATA_FIELD_PRESENT(
               inMetaValues, 
               FWPS_METADATA_FIELD_TRANSPORT_HEADER_SIZE));
      packet->ipHeaderSize = inMetaValues->ipHeaderSize;
      packet->transportHeaderSize = inMetaValues->transportHeaderSize;

      packet->nblOffset = 
         NET_BUFFER_DATA_OFFSET(NET_BUFFER_LIST_FIRST_NB(packet->netBufferList));
   }

   KeAcquireInStackQueuedSpinLock(
      &gPacketQueueLock,
      &packetQueueLockHandle
      );

   if (!gDriverUnloading)
   {
      signalWorkerThread = IsListEmpty(&gPacketQueue);

      InsertTailList(&gPacketQueue, &packet->listEntry);
      packet = NULL; // ownership transferred

      classifyOut->actionType = FWP_ACTION_BLOCK;
      classifyOut->flags |= FWPS_CLASSIFY_OUT_FLAG_ABSORB;
   }
   else
   {
      //
      // Driver is being unloaded, permit any incoming packets.
      //
      signalWorkerThread = FALSE;

      classifyOut->actionType = FWP_ACTION_PERMIT;
   }

   if (signalWorkerThread)
   {
      KeSetEvent(
         &gPacketQueueEvent, 
         0, 
         FALSE
         );
   }

   KeReleaseInStackQueuedSpinLock(&packetQueueLockHandle);

Exit:
   
   if (packet != NULL)
   {
      DDProxyFreePendedPacket(packet);
   }

   return;
}

NTSTATUS
DDProxyNotify(
   IN FWPS_CALLOUT_NOTIFY_TYPE notifyType,
   IN const GUID* filterKey,
   IN const FWPS_FILTER0* filter
   )
{
   UNREFERENCED_PARAMETER(notifyType);
   UNREFERENCED_PARAMETER(filterKey);
   UNREFERENCED_PARAMETER(filter);

   return STATUS_SUCCESS;
}

void
DDProxyFlowDelete(
   IN UINT16 layerId,
   IN UINT32 calloutId,
   IN UINT64 flowContext
   )
/* ++

   This is the flowDeleteFn function of the datagram-data callout. It 
   removes the flow context from the global flow list and dereference the 
   context.

-- */
{
   DD_PROXY_FLOW_CONTEXT* flowContextLocal = (DD_PROXY_FLOW_CONTEXT*)(DWORD_PTR)flowContext;

   KLOCK_QUEUE_HANDLE flowListLockHandle;

   UNREFERENCED_PARAMETER(layerId);
   UNREFERENCED_PARAMETER(calloutId);

   KeAcquireInStackQueuedSpinLock(
      &gFlowListLock,
      &flowListLockHandle
      );

   if (!flowContextLocal->deleted)
   {
      RemoveEntryList(&flowContextLocal->listEntry);
   }

   KeReleaseInStackQueuedSpinLock(&flowListLockHandle);

   DDProxyDereferenceFlowContext(flowContextLocal);
}

typedef struct UDP_HEADER_ {
    UINT16 srcPort;
    UINT16 destPort;
    UINT16 length;
    UINT16 checksum;
} UDP_HEADER;

void DDProxyInjectComplete(
   IN void* context,
   IN OUT NET_BUFFER_LIST* netBufferList,
   IN BOOLEAN dispatchLevel
   )
{
   DD_PROXY_PENDED_PACKET* packet = context;
   UNREFERENCED_PARAMETER(dispatchLevel);

   FwpsFreeCloneNetBufferList0(netBufferList, 0);

   DDProxyFreePendedPacket(packet);
}

NTSTATUS
DDProxyCloneModifyReinjectOutbound(
   IN DD_PROXY_PENDED_PACKET* packet
   )
/* ++

   This function clones the outbound net buffer list and, if needed, 
   modifies the destination port of all indicated packets (i.e. NET_BUFFER) 
   and/or send-injects the clone to a new destination address.

-- */
{
   NTSTATUS status = STATUS_SUCCESS;

   NET_BUFFER_LIST* clonedNetBufferList = NULL;
   UDP_HEADER* udpHeader;
   FWPS_TRANSPORT_SEND_PARAMS0 sendArgs = {0};

   status = FwpsAllocateCloneNetBufferList0(
               packet->netBufferList,
               NULL,
               NULL,
               0,
               &clonedNetBufferList
               );
   if (!NT_SUCCESS(status))
   {
      goto Exit;
   }

   //
   // Check to see if port modification is required.
   //
   if ((packet->belongingFlow->protocol == IPPROTO_UDP) && 
       (packet->belongingFlow->toRemotePort != 0))
   {
      NET_BUFFER* netBuffer;

      //
      // The data offset of outbound transport packets is the beginning of 
      // transport header (e.g. UDP header). The IP header has not yet been
      // constructed at Datagram-Data (or outbound Transport) layer.
      //
      // Note the packet offset is inherited by the clone.
      //

      //
      // Outbound net buffer list can contain more than one net buffer (e.g. 
      // one UDP packet).
      //

      for (netBuffer = NET_BUFFER_LIST_FIRST_NB(clonedNetBufferList);
           netBuffer != NULL;
           netBuffer = NET_BUFFER_NEXT_NB(netBuffer))
      {
         udpHeader = NdisGetDataBuffer(
                        netBuffer,
                        sizeof(UDP_HEADER),
                        NULL,
                        sizeof(UINT16),
                        0
                        );
         ASSERT(udpHeader != NULL); // We can assume UDP header in a net buffer
                                    // is contiguous and 2-byte aligned.
         
         udpHeader->destPort = packet->belongingFlow->toRemotePort;
         udpHeader->checksum = 0;
      }
   }

   //
   // Determine whehter we need to proxy the destination address. If not, 
   // we set the remoteAddress to the same address that was initially 
   // classified.
   //
   sendArgs.remoteAddress = 
      (packet->belongingFlow->toRemoteAddr ? packet->belongingFlow->toRemoteAddr
                                           : (UINT8*)&packet->remoteAddr);
   sendArgs.remoteScopeId = packet->remoteScopeId;
   sendArgs.controlData = packet->controlData;
   sendArgs.controlDataLength = packet->controlDataLength;

   //
   // Send-inject the modified net buffer list to the new destination address.
   //

   status = FwpsInjectTransportSendAsync0(
               gInjectionHandle,
               NULL,
               packet->endpointHandle,
               0,
               &sendArgs,
               packet->belongingFlow->addressFamily,
               packet->compartmentId,
               clonedNetBufferList,
               DDProxyInjectComplete,
               packet
               );

   if (!NT_SUCCESS(status))
   {
      goto Exit;
   }

   clonedNetBufferList = NULL; // ownership transferred to the 
                               // completion function.

Exit:

   if (clonedNetBufferList != NULL)
   {
      FwpsFreeCloneNetBufferList0(clonedNetBufferList, 0);
   }

   return status;
}

NTSTATUS
DDProxyCloneModifyReinjectInbound(
   DD_PROXY_PENDED_PACKET* packet
   )
/* ++

   This function clones the inbound net buffer list and, if needed, 
   modifies the source port and/or source address and receive-injects 
   the clone back to the tcpip stack.

-- */
{
   NTSTATUS status = STATUS_SUCCESS;

   NET_BUFFER_LIST* clonedNetBufferList = NULL;
   NET_BUFFER* netBuffer;
   UDP_HEADER* udpHeader;
   ULONG nblOffset;

   //
   // For inbound net buffer list, we can assume it contains only one 
   // net buffer.
   //
   netBuffer = NET_BUFFER_LIST_FIRST_NB(packet->netBufferList);
   
   nblOffset = NET_BUFFER_DATA_OFFSET(netBuffer);

   //
   // The TCP/IP stack could have retreated the net buffer list by the 
   // transportHeaderSize amount; detect the condition here to avoid
   // retreating twice.
   //
   if (nblOffset != packet->nblOffset)
   {
      ASSERT(packet->nblOffset - nblOffset == packet->transportHeaderSize);
      packet->transportHeaderSize = 0;
   }

   //
   // Adjust the net buffer list offset to the start of the IP header.
   //
   NdisRetreatNetBufferDataStart(
      netBuffer,
      packet->ipHeaderSize + packet->transportHeaderSize,
      0,
      NULL
      );

   //
   // Note that the clone will inherit the original net buffer list's offset.
   //

   status = FwpsAllocateCloneNetBufferList0(
               packet->netBufferList,
               NULL,
               NULL,
               0,
               &clonedNetBufferList
               );

   //
   // Undo the adjustment on the original net buffer list.
   //

   NdisAdvanceNetBufferDataStart(
      netBuffer,
      packet->ipHeaderSize + packet->transportHeaderSize,
      FALSE,
      NULL
      );

   if (!NT_SUCCESS(status))
   {
      goto Exit;
   }

   //
   // Check to see if port modification is required.
   //
   if ((packet->belongingFlow->protocol == IPPROTO_UDP) && 
       (packet->belongingFlow->toRemotePort != 0))
   {
      netBuffer = NET_BUFFER_LIST_FIRST_NB(clonedNetBufferList);

      //
      // Advance to the beginning of the transport header (i.e. UDP header).
      //
      NdisAdvanceNetBufferDataStart(
         netBuffer,
         packet->ipHeaderSize,
         FALSE,
         NULL
         );

      udpHeader = NdisGetDataBuffer(
                     netBuffer,
                     sizeof(UDP_HEADER),
                     NULL,
                     sizeof(UINT16),
                     0
                     );
      ASSERT(udpHeader != NULL); // We can assume UDP header in a net buffer
                                 // is contiguous and 2-byte aligned.
      
      udpHeader->destPort = 
         packet->belongingFlow->toRemotePort; 
                                    // This is our new source port -- or
                                    // the destination port of the original
                                    // outbound traffic.
      udpHeader->checksum = 0;

      //
      // Undo the advance. Net buffer list needs to be positioned at the 
      // beginning of IP header for address modification and/or receive-
      // injection.
      //
      NdisRetreatNetBufferDataStart(
         netBuffer,
         packet->ipHeaderSize,
         0,
         NULL
         );
   }

   if (packet->belongingFlow->toRemoteAddr != NULL)
   {
#if (NTDDI_VERSION >= NTDDI_WIN6SP1)

      status = FwpsConstructIpHeaderForTransportPacket0(
                  clonedNetBufferList,
                  packet->ipHeaderSize,
                  packet->belongingFlow->addressFamily,
                  packet->belongingFlow->toRemoteAddr,  
                                       // This is our new source address --
                                       // or the destination address of the
                                       // original outbound traffic.
                  (UINT8*)&packet->belongingFlow->localAddr, 
                                       // This is the destination address of
                                       // the clone -- or the source of the
                                       // original outbound traffic.
                  packet->belongingFlow->protocol,
                  0,
                  NULL,
                  0,
                  0,
                  NULL,
                  0,
                  0
                  );
#else
      ASSERT(FALSE); // prior to Vista SP1, IP address needs to be modified 
                     // manually (including updating IP checksum).

      status = STATUS_NOT_IMPLEMENTED;
#endif

      if (!NT_SUCCESS(status))
      {
         goto Exit;
      }
   }

   status = FwpsInjectTransportReceiveAsync0(
               gInjectionHandle,
               NULL,
               NULL,
               0,
               packet->belongingFlow->addressFamily,
               packet->compartmentId,
               packet->interfaceIndex,
               packet->subInterfaceIndex,
               clonedNetBufferList,
               DDProxyInjectComplete,
               packet
               );

   if (!NT_SUCCESS(status))
   {
      goto Exit;
   }

   clonedNetBufferList = NULL; // ownership transferred to the 
                               // completion function.

Exit:

   if (clonedNetBufferList != NULL)
   {
      FwpsFreeCloneNetBufferList0(clonedNetBufferList, 0);
   }

   return status;
}
   
void
DDProxyWorker(
   IN PVOID StartContext
   )
/* ++

   This worker thread waits for the packet queue event when the queue is
   empty; and it will be woken up when there are packets queued needing to 
   be proxied to or from the new destination address/port. Once awaking, 
   It will run in a loop to clone-modify-reinject packets until the packet 
   queue is exhausted (and it will go to sleep waiting for more work).

   The worker thread will end once it detected the driver is unloading.

-- */
{
   DD_PROXY_PENDED_PACKET* packet;
   LIST_ENTRY* listEntry;
   KLOCK_QUEUE_HANDLE packetQueueLockHandle;

   UNREFERENCED_PARAMETER(StartContext);

   for(;;)
   {
      KeWaitForSingleObject(
         &gPacketQueueEvent,
         Executive, 
         KernelMode, 
         FALSE, 
         NULL
         );

      if (gDriverUnloading)
      {
         break;
      }

      ASSERT(!IsListEmpty(&gPacketQueue));

      KeAcquireInStackQueuedSpinLock(
         &gPacketQueueLock,
         &packetQueueLockHandle
         );

      listEntry = RemoveHeadList(&gPacketQueue);

      KeReleaseInStackQueuedSpinLock(&packetQueueLockHandle);

      packet = CONTAINING_RECORD(
                        listEntry,
                        DD_PROXY_PENDED_PACKET,
                        listEntry
                        );

      if (!packet->belongingFlow->deleted)
      {
         NTSTATUS status;

         if (packet->direction == FWP_DIRECTION_OUTBOUND)
         {
            status = DDProxyCloneModifyReinjectOutbound(packet);
         }
         else
         {
            status = DDProxyCloneModifyReinjectInbound(packet);
         }

         if (NT_SUCCESS(status))
         {
            packet = NULL; // ownership transferred.
         }
      }

      if (packet != NULL)
      {
         DDProxyFreePendedPacket(packet);
      }

      KeAcquireInStackQueuedSpinLock(
         &gPacketQueueLock,
         &packetQueueLockHandle
         );

      if (IsListEmpty(&gPacketQueue) && !gDriverUnloading)
      {
         KeClearEvent(&gPacketQueueEvent);
      }

      KeReleaseInStackQueuedSpinLock(&packetQueueLockHandle);
   }

   ASSERT(gDriverUnloading);

   //
   // Discard all the pended packets if driver is being unloaded.
   //

   KeAcquireInStackQueuedSpinLock(
      &gPacketQueueLock,
      &packetQueueLockHandle
      );

   while (!IsListEmpty(&gPacketQueue))
   {
      listEntry = RemoveHeadList(&gPacketQueue);

      packet = CONTAINING_RECORD(
                        listEntry,
                        DD_PROXY_PENDED_PACKET,
                        listEntry
                        );

      DDProxyFreePendedPacket(packet);
   }

   KeReleaseInStackQueuedSpinLock(&packetQueueLockHandle);
   PsTerminateSystemThread(STATUS_SUCCESS);


}


