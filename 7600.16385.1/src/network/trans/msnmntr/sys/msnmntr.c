/*++
Copyright (c) Microsoft Corporation. All rights reserved

Abstract:

    MSN monitor sample driver callout routines

Environment:

    Kernel mode
--*/

#include "ntddk.h"
#include "ntstrsafe.h"

#include "fwpmk.h"

#pragma warning(push)
#pragma warning(disable:4201)       // unnamed struct/union

#include "fwpsk.h"

#pragma warning(pop)

#include "ioctl.h"

#include "msnmntr.h"
#include "ctl.h"
#include "notify.h"

#define INITGUID
#include <guiddef.h>
#include "mntrguid.h"

//
// Software Tracing Definitions
//
#define WPP_CONTROL_GUIDS \
    WPP_DEFINE_CONTROL_GUID(MsnMntrMonitor,(dd65554d, 9925, 49d1, 83b6, 46125feb4207),  \
        WPP_DEFINE_BIT(TRACE_FLOW_ESTABLISHED)      \
        WPP_DEFINE_BIT(TRACE_STATE_CHANGE)      \
        WPP_DEFINE_BIT(TRACE_LAYER_NOTIFY) )

#include "msnmntr.tmh"

#define TAG_NAME_CALLOUT 'CnoM'

UINT32 flowEstablishedId = 0;
UINT32 streamId = 0;
long monitoringEnabled = 0;
LIST_ENTRY flowContextList;
KSPIN_LOCK flowContextListLock;

NTSTATUS MonitorCoFlowEstablishedNotifyV4(
    IN  FWPS_CALLOUT_NOTIFY_TYPE        notifyType,
    IN  const GUID*             filterKey,
    IN  const FWPS_FILTER*     filter);

NTSTATUS MonitorCoStreamNotifyV4(
    IN  FWPS_CALLOUT_NOTIFY_TYPE         notifyType,
    IN  const GUID*             filterKey,
    IN  const FWPS_FILTER*     filter);

VOID MonitorCoStreamFlowDeletion(
   IN UINT16 layerId,
   IN UINT32 calloutId,
   IN UINT64 flowContext);

#if(NTDDI_VERSION >= NTDDI_WIN7)
NTSTATUS MonitorCoFlowEstablishedCalloutV4(
   IN const FWPS_INCOMING_VALUES* inFixedValues,
   IN const FWPS_INCOMING_METADATA_VALUES* inMetaValues,
   IN VOID* packet,
   IN const void* classifyContext,
   IN const FWPS_FILTER* filter,
   IN UINT64 flowContext,
   OUT FWPS_CLASSIFY_OUT* classifyOut);
#else if(NTDDI_VERSION < NTDDI_WIN7)
NTSTATUS MonitorCoFlowEstablishedCalloutV4(
   IN const FWPS_INCOMING_VALUES* inFixedValues,
   IN const FWPS_INCOMING_METADATA_VALUES* inMetaValues,
   IN VOID* packet,
   IN const FWPS_FILTER* filter,
   IN UINT64 flowContext,
   OUT FWPS_CLASSIFY_OUT* classifyOut);
#endif

#if(NTDDI_VERSION >= NTDDI_WIN7)
NTSTATUS MonitorCoStreamCalloutV4(
   IN const FWPS_INCOMING_VALUES* inFixedValues,
   IN const FWPS_INCOMING_METADATA_VALUES* inMetaValues,
   IN VOID* packet,
   IN const void* classifyContext,
   IN const FWPS_FILTER* filter,
   IN UINT64 flowContext,
   OUT FWPS_CLASSIFY_OUT* classifyOut);
#else if(NTDDI_VERSION < NTDDI_WIN7)
NTSTATUS MonitorCoStreamCalloutV4(
   IN const FWPS_INCOMING_VALUES* inFixedValues,
   IN const FWPS_INCOMING_METADATA_VALUES* inMetaValues,
   IN VOID* packet,
   IN const FWPS_FILTER* filter,
   IN UINT64 flowContext,
   OUT FWPS_CLASSIFY_OUT* classifyOut);
#endif

NTSTATUS
MonitorCoRegisterCallout(
   IN OUT void* deviceObject,
   IN  FWPS_CALLOUT_CLASSIFY_FN ClassifyFunction,
   IN  FWPS_CALLOUT_NOTIFY_FN NotifyFunction,
   IN  FWPS_CALLOUT_FLOW_DELETE_NOTIFY_FN FlowDeleteFunction,
   IN  GUID const* calloutKey,
   IN  UINT32 flags,
   OUT UINT32* calloutId
   )
{
    FWPS_CALLOUT sCallout;
    NTSTATUS status = STATUS_SUCCESS;

    memset(&sCallout, 0, sizeof(FWPS_CALLOUT));

    sCallout.calloutKey = *calloutKey;
    sCallout.flags = flags;
    sCallout.classifyFn = ClassifyFunction;
    sCallout.notifyFn = NotifyFunction;
    sCallout.flowDeleteFn = FlowDeleteFunction;

    status = FwpsCalloutRegister(deviceObject, &sCallout, calloutId);

    return status;
}

NTSTATUS
MonitorCoRegisterCallouts(
   IN OUT void* deviceObject
   )
{
    NTSTATUS status;

   //
   // We won't be called for flow deletion for the flow established layer 
   // since we only establish a flow for the stream layer, so we don't
   // specify a flow deletion function.
   //
   status = MonitorCoRegisterCallout(deviceObject,
                                     MonitorCoFlowEstablishedCalloutV4,
                                     MonitorCoFlowEstablishedNotifyV4,
                                     NULL, // We don't need a flow delete function at this layer.
                                     &MSN_MONITOR_FLOW_ESTABLISHED_CALLOUT_V4,
                                     0, // No flags.
                                     &flowEstablishedId);

   if (NT_SUCCESS(status))
   {
      status = MonitorCoRegisterCallout(deviceObject,
                                        MonitorCoStreamCalloutV4,
                                        MonitorCoStreamNotifyV4,
                                        MonitorCoStreamFlowDeletion,
                                        &MSN_MONITOR_STREAM_CALLOUT_V4,
                                        FWP_CALLOUT_FLAG_CONDITIONAL_ON_FLOW,
                                        &streamId);
   }

   return status;
}

NTSTATUS
MonitorCoUnregisterCallout(
   IN  GUID const* calloutKey
   )
{
   NTSTATUS status;

   status = FwpsCalloutUnregisterByKey(calloutKey);

   return status;
}

NTSTATUS
MonitorCoUnregisterCallouts()
{
   NTSTATUS status;

   status = MonitorCoUnregisterCallout(&MSN_MONITOR_FLOW_ESTABLISHED_CALLOUT_V4);

   if (NT_SUCCESS(status))
   {
      status = MonitorCoUnregisterCallout(&MSN_MONITOR_STREAM_CALLOUT_V4);
   }

   return status;
}


NTSTATUS
MonitorCoInsertFlowContext(
   IN FLOW_DATA* flowContext)
{
   KLOCK_QUEUE_HANDLE lockHandle;
   NTSTATUS status;

   KeAcquireInStackQueuedSpinLock(&flowContextListLock, &lockHandle);

   // Catch the case where we disabled monitoring after we had intended to
   // associate the context to the flow so that we don't bugcheck due to
   // our driver being unloaded and then receiving a call for a particular
   // flow or leak the memory because we unloaded without freeing it.
   if (monitoringEnabled)
   {
      DoTraceMessage(TRACE_FLOW_ESTABLISHED, "Creating flow for traffic.\r\n");

      InsertTailList(&flowContextList, &flowContext->listEntry);
      status = STATUS_SUCCESS;
   }
   else
   {
      DoTraceMessage(TRACE_FLOW_ESTABLISHED, "Unable to create flow, driver shutting down.\r\n");

      // Our driver is shutting down.
      status = STATUS_SHUTDOWN_IN_PROGRESS;
   }

   KeReleaseInStackQueuedSpinLock(&lockHandle);
   return status;
}

VOID
MonitorCoRemoveFlowContext(
   IN FLOW_DATA* flowContext)
{
   KLOCK_QUEUE_HANDLE lockHandle;

   KeAcquireInStackQueuedSpinLock(&flowContextListLock, &lockHandle);

   RemoveEntryList(&flowContext->listEntry);

   KeReleaseInStackQueuedSpinLock(&lockHandle);
}

void
MonitorCoCleanupFlowContext(
   IN FLOW_DATA*  flowContext)
/*
Routine Description

    Called to cleanup a flow context on flow deletion.

Arguments
    [IN] FLOW_DATA*     flowContext -  Context associated with the flow now
                                       being deleted.

Return values

    None.

Notes


*/
{
   //
   // If we're already being deleted from the list then we mustn't try to 
   // remove ourselves here.
   //
   if (!flowContext->deleting)
   {
      MonitorCoRemoveFlowContext(flowContext);
   }

   if (flowContext->processPath)
   {
      ExFreePoolWithTag(flowContext->processPath, TAG_NAME_CALLOUT);
   }
   ExFreePoolWithTag(flowContext, TAG_NAME_CALLOUT);
}

UINT64
MonitorCoCreateFlowContext(
   IN const FWPS_INCOMING_VALUES* inFixedValues,
   IN const FWPS_INCOMING_METADATA_VALUES* inMetaValues,
   OUT UINT64* flowHandle)
/*
Routine Description

    Creates a flow context that is associated with the current flow

Arguments
    [IN] FWPS_CALLOUT_NOTIFY_TYPE        notifyType  -   Type of notification

    [IN] GUID*                  filterKey   -   Key of the filter that was
                                                added/deleted/modified.

    [IN] struct FWPS_FILTER_*  filter      -   pointer to the Filter itself.

Return values

    STATUS_SUCCESS or a specific error code.

Notes


*/
{
   FLOW_DATA*     flowContext = NULL;
   NTSTATUS       status;
   FWP_BYTE_BLOB* processPath;
   UINT32         index;

   if (!FWPS_IS_METADATA_FIELD_PRESENT(inMetaValues, FWPS_METADATA_FIELD_PROCESS_PATH))
   {
      status = STATUS_NOT_FOUND;
      goto cleanup;
   }

   processPath = inMetaValues->processPath;

   //  Flow context is always created at the Flow established layer.

   // flowContext gets deleted in MonitorCoCleanupFlowContext 
   #pragma warning( suppress : 28197 )
   flowContext = ExAllocatePoolWithTag(NonPagedPool,
                                       sizeof(FLOW_DATA),
                                       TAG_NAME_CALLOUT);

   if (!flowContext)
   {
      return (UINT64) NULL;
   }

   RtlZeroMemory(flowContext, sizeof(FLOW_DATA));

   flowContext->deleting = FALSE;
   flowContext->flowHandle = inMetaValues->flowHandle;
   *flowHandle = flowContext->flowHandle;

   index = FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_LOCAL_ADDRESS;
   flowContext->localAddressV4 = inFixedValues->incomingValue[index].value.uint32;


   index = FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_LOCAL_PORT;
   flowContext->localPort = inFixedValues->incomingValue[index].value.uint16;

   index = FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_REMOTE_ADDRESS;
   flowContext->remoteAddressV4 = inFixedValues->incomingValue[index].value.uint32;

   index = FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_REMOTE_PORT;
   flowContext->remotePort = inFixedValues->incomingValue[index].value.uint16;

   index = FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_PROTOCOL;
   flowContext->ipProto = inFixedValues->incomingValue[index].value.uint16;

   // flowContext->processPath gets deleted in MonitorCoCleanupFlowContext 
   #pragma warning( suppress : 28197 )
   flowContext->processPath = ExAllocatePoolWithTag(NonPagedPool,
                                                    processPath->size,
                                                    TAG_NAME_CALLOUT);

   if (!flowContext->processPath)
   {
      status = STATUS_NOT_FOUND;
      goto cleanup;
   }

   memcpy(flowContext->processPath, processPath->data, processPath->size);

   status = MonitorCoInsertFlowContext(flowContext);

cleanup:

   if (!NT_SUCCESS(status) && flowContext)
   {
      MonitorCoCleanupFlowContext(flowContext);

      flowContext = NULL;
   }

   return (UINT64) flowContext;
}

NTSTATUS MonitorCoInitialize(PDEVICE_OBJECT deviceObject)
/*
Routine Description

   Initializes our flow tracking so that we can handle the case where
   the driver is shutdown with flows that are still active.

Arguments

   None.

Return values

   STATUS_SUCCESS or a specific error code.

Notes


*/
{
   NTSTATUS status;

   //  Initialize the flow context list and lock.  We need this to be able
   //  to handle the case where our driver is stopped while we still have
   //  contexts associated with flows.
   InitializeListHead(&flowContextList);
   KeInitializeSpinLock(&flowContextListLock);

   status = MonitorCoRegisterCallouts(deviceObject);

   return status;
}

void MonitorCoUninitialize()
/*
Routine Description

   Uninitializes the callouts module (this module) by ensuring that all
   flow contexts are no longer associated with a flow to ensure that
   our driver is not called after it is unloaded.

Arguments

   None.

Return values

   STATUS_SUCCESS or a specific error code.

Notes


*/
{
   KLOCK_QUEUE_HANDLE lockHandle;

   // Make sure we don't associate any more contexts to flows.
   MonitorCoDisableMonitoring();

   KeAcquireInStackQueuedSpinLock(&flowContextListLock, &lockHandle);

   while (!IsListEmpty(&flowContextList))
   {
      FLOW_DATA* flowContext;
      LIST_ENTRY* entry;
      NTSTATUS status;

      entry = RemoveHeadList(&flowContextList);

      flowContext = CONTAINING_RECORD(entry, FLOW_DATA, listEntry);
      flowContext->deleting = TRUE; // We don't want our flow deletion function
                                    // to try to remove this from the list.

      status = FwpsFlowRemoveContext(flowContext->flowHandle,
                                      FWPS_LAYER_STREAM_V4,
                                      streamId);
      ASSERT(NT_SUCCESS(status));
   }

   KeReleaseInStackQueuedSpinLock(&lockHandle);

   MonitorCoUnregisterCallouts();
}

NTSTATUS MonitorCoEnableMonitoring(
   IN    MONITOR_SETTINGS* monitorSettings)
/*
Routine Description

   Enables monitoring of traffic.  Before this is called the driver will not
   associate any context to flows and will therefore not do any inspection.
   Once this is called we will start to track flows for the applications that
   we are interested in.

Arguments
    [IN] MONITOR_SETTINS monitorSettings - Settings that govern our behavior.
                                           Nothing is specified at this time.

Return values

    STATUS_SUCCESS or a specific error code.

Notes


*/
{
   KLOCK_QUEUE_HANDLE lockHandle;

   if (!monitorSettings)
   {
      return STATUS_INVALID_PARAMETER;
   }

   DoTraceMessage(TRACE_STATE_CHANGE, "Enabling monitoring.\r\n");

   KeAcquireInStackQueuedSpinLock(&flowContextListLock, &lockHandle);

   monitoringEnabled = 1;

   KeReleaseInStackQueuedSpinLock(&lockHandle);

   return STATUS_SUCCESS;
}

VOID
MonitorCoDisableMonitoring()
/*
Routine Description

   Disables monitoring of new connections so that we can safely shutdown.

Arguments

   None.

Return values

   None.

Notes


*/
{
   KLOCK_QUEUE_HANDLE lockHandle;

   DoTraceMessage(TRACE_STATE_CHANGE, "Disabling monitoring.\r\n");

   KeAcquireInStackQueuedSpinLock(&flowContextListLock, &lockHandle);

   monitoringEnabled = 0;

   KeReleaseInStackQueuedSpinLock(&lockHandle);
}

#if(NTDDI_VERSION >= NTDDI_WIN7)
NTSTATUS MonitorCoFlowEstablishedCalloutV4(
   IN const FWPS_INCOMING_VALUES* inFixedValues,
   IN const FWPS_INCOMING_METADATA_VALUES* inMetaValues,
   IN VOID* packet,
   IN const void* classifyContext,
   IN const FWPS_FILTER* filter,
   IN UINT64 flowContext,
   OUT FWPS_CLASSIFY_OUT* classifyOut)

#else if(NTDDI_VERSION < NTDDI_WIN7)
NTSTATUS MonitorCoFlowEstablishedCalloutV4(
   IN const FWPS_INCOMING_VALUES* inFixedValues,
   IN const FWPS_INCOMING_METADATA_VALUES* inMetaValues,
   IN VOID* packet,
   IN const FWPS_FILTER* filter,
   IN UINT64 flowContext,
   OUT FWPS_CLASSIFY_OUT* classifyOut)
#endif
/*
Routine Description

   Our flow established callout for Ipv4 traffic.

Arguments
   [IN] const FWPS_INCOMING_VALUES* inFixedValues -  The fixed values passed in
                                                      based on the traffic.
   [IN] const FWPS_INCOMING_METADATA_VALUES* inMetaValues - Metadata the
                                                             provides additional
                                                             information about the
                                                             connection.
   [IN] VOID* packet - Depending on the layer and protocol this can be NULL or a
                       layer specific type.
   [IN] const FWPS_FILTER* filter - The filter that has specified this callout.

   [IN] UINT64 flowContext - Flow context associated with a flow
   [OUT] FWPS_CLASSIFY_OUT* classifyOut - Out parameter that is used to inform
                                           the filter engine of our decision

Return values

    STATUS_SUCCESS or a specific error code.

Notes


*/
{

   NTSTATUS status = STATUS_SUCCESS;
   UINT64   flowHandle;
   UINT64   flowContextLocal;

   UNREFERENCED_PARAMETER(packet);
   #if(NTDDI_VERSION >= NTDDI_WIN7)
   UNREFERENCED_PARAMETER(classifyContext);
   #endif
   UNREFERENCED_PARAMETER(filter);
   UNREFERENCED_PARAMETER(flowContext);

   if (monitoringEnabled)
   {
      flowContextLocal = MonitorCoCreateFlowContext(inFixedValues, inMetaValues, &flowHandle);

      if (!flowContextLocal)
      {
         classifyOut->actionType = FWP_ACTION_CONTINUE;
         goto cleanup;
      }

      status = FwpsFlowAssociateContext(flowHandle,
                                         FWPS_LAYER_STREAM_V4,
                                         streamId,
                                         flowContextLocal);
      if (!NT_SUCCESS(status))
      {
         classifyOut->actionType = FWP_ACTION_CONTINUE;
         goto cleanup;
      }
   }

   classifyOut->actionType = FWP_ACTION_PERMIT;

cleanup:

   return status;
}

#if(NTDDI_VERSION >= NTDDI_WIN7)
NTSTATUS MonitorCoStreamCalloutV4(
   IN const FWPS_INCOMING_VALUES* inFixedValues,
   IN const FWPS_INCOMING_METADATA_VALUES* inMetaValues,
   IN VOID* packet,
   IN const void* classifyContext,
   IN const FWPS_FILTER* filter,
   IN UINT64 flowContext,
   OUT FWPS_CLASSIFY_OUT* classifyOut)

#else if(NTDDI_VERSION < NTDDI_WIN7)
NTSTATUS MonitorCoStreamCalloutV4(
   IN const FWPS_INCOMING_VALUES* inFixedValues,
   IN const FWPS_INCOMING_METADATA_VALUES* inMetaValues,
   IN VOID* packet,
   IN const FWPS_FILTER* filter,
   IN UINT64 flowContext,
   OUT FWPS_CLASSIFY_OUT* classifyOut)
#endif
/*
Routine Description

   Our stream layer callout for traffic to/from the application we're
   interested in. Since we specified the filter that matches this callout
   as conditional on flow, we only get called if we've associated a flow with
   the traffic.

Arguments
   [IN] const FWPS_INCOMING_VALUES* inFixedValues -  The fixed values passed in
                                                      based on the traffic.
   [IN] const FWPS_INCOMING_METADATA_VALUES* inMetaValues - Metadata the
                                                             provides additional
                                                             information about the
                                                             connection.
   [IN] VOID* packet - Depending on the layer and protocol this can be NULL or a
                       layer specific type.
   [IN] const FWPS_FILTER* filter - The filter that has specified this callout.

   [IN] UINT64 flowContext - Flow context associated with a flow
   [OUT] FWPS_CLASSIFY_OUT* classifyOut - Out parameter that is used to inform
                                           the filter engine of our decision

Return values

    STATUS_SUCCESS or a specific error code.

Notes


*/
{
   FLOW_DATA* flowData;
   FWPS_STREAM_CALLOUT_IO_PACKET* streamPacket;
   NTSTATUS status = STATUS_SUCCESS;
   BOOLEAN inbound;

   UNREFERENCED_PARAMETER(inFixedValues);
   UNREFERENCED_PARAMETER(inMetaValues);
   #if(NTDDI_VERSION >= NTDDI_WIN7)
   UNREFERENCED_PARAMETER(classifyContext);
   #endif
   UNREFERENCED_PARAMETER(filter);
   UNREFERENCED_PARAMETER(flowContext);

   if (!monitoringEnabled)
   {
      goto cleanup;
   }

   streamPacket = (FWPS_STREAM_CALLOUT_IO_PACKET*) packet;

   if (streamPacket->streamData != NULL &&
       streamPacket->streamData->dataLength != 0)
   {
      flowData = *(FLOW_DATA**)(UINT64*) &flowContext;

      inbound = (BOOLEAN) ((streamPacket->streamData->flags & FWPS_STREAM_FLAG_RECEIVE) == FWPS_STREAM_FLAG_RECEIVE);

      status = MonitorNfNotifyMessage(streamPacket->streamData,
                                      inbound,
                                      flowData->localPort,
                                      flowData->remotePort);
   }

cleanup:

   // Return CONTINUE to the filter engine, we're just monitoring.

   classifyOut->actionType = FWP_ACTION_CONTINUE;

   return status;
}

NTSTATUS MonitorCoFlowEstablishedNotifyV4(
    IN  FWPS_CALLOUT_NOTIFY_TYPE        notifyType,
    IN  const GUID*             filterKey,
    IN  const FWPS_FILTER*     filter)
/*
Routine Description

    Notification routine that is called whenever a filter is added, deleted or
    modified on the layer that our callout is registered against.

Arguments
    [IN] FWPS_CALLOUT_NOTIFY_TYPE       notifyType  -   Type of notification

    [IN] GUID*                  filterKey   -   Key of the filter that was
                                                added/deleted/modified.

    [IN] struct FWPS_FILTER_*  filter      -   pointer to the Filter itself.

Return values

    STATUS_SUCCESS or a specific error code.

Notes


*/
{
    UNREFERENCED_PARAMETER(filterKey);
    UNREFERENCED_PARAMETER(filter);

    switch (notifyType)
    {
    case FWPS_CALLOUT_NOTIFY_ADD_FILTER:
        DoTraceMessage(TRACE_LAYER_NOTIFY,
                       "Filter Added to Flow Established layer.\r\n");

       break;
    case FWPS_CALLOUT_NOTIFY_DELETE_FILTER:
        DoTraceMessage(TRACE_LAYER_NOTIFY,
                       "Filter Deleted from Flow Established layer.\r\n");
       break;
    }

    return STATUS_SUCCESS;
}

VOID MonitorCoStreamFlowDeletion(
   IN UINT16 layerId,
   IN UINT32 calloutId,
   IN UINT64 flowContext)
{
   FLOW_DATA** flowData;
   UINT64* flow;

   UNREFERENCED_PARAMETER(layerId);
   UNREFERENCED_PARAMETER(calloutId);

   flow = &flowContext;
   flowData = ((FLOW_DATA**)flow);

   MonitorCoCleanupFlowContext(*flowData);
}

NTSTATUS MonitorCoStreamNotifyV4(
    IN  FWPS_CALLOUT_NOTIFY_TYPE        notifyType,
    IN  const GUID*             filterKey,
    IN  const FWPS_FILTER*     filter)
/*
Routine Description

    Notification routine that is called whenever a filter is added, deleted or
    modified on the layer that our callout is registered against.

Arguments
    [IN] FWPS_CALLOUT_NOTIFY_TYPE        notifyType  -   Type of notification

    [IN] GUID*                  filterKey   -   Key of the filter that was
                                                added/deleted/modified.

    [IN] struct FWPS_FILTER_*  filter      -   pointer to the Filter itself.

Return values

    STATUS_SUCCESS or a specific error code.

Notes


*/
{
    UNREFERENCED_PARAMETER(notifyType);
    UNREFERENCED_PARAMETER(filterKey);
    UNREFERENCED_PARAMETER(filter);

    switch (notifyType)
    {
    case FWPS_CALLOUT_NOTIFY_ADD_FILTER:
        DoTraceMessage(TRACE_LAYER_NOTIFY,
                       "Filter Added to Stream layer.\r\n");

       break;
    case FWPS_CALLOUT_NOTIFY_DELETE_FILTER:
        DoTraceMessage(TRACE_LAYER_NOTIFY,
                       "Filter Deleted from Stream layer.\r\n");
       break;
    }
    return STATUS_SUCCESS;
}



