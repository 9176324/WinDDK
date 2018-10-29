/*++

Copyright (c) Microsoft Corporation. All rights reserved

Abstract:

   Stream Edit Callout Driver Sample.

   This sample demonstrates finding and replacing a string pattern from a
   live TCP stream via the WFP stream API.

   The driver can function in one of the two modes --

      o  Inline Editing where all modification is carried out within the
         WFP ClassifyFn callout function.

      o  Out-of-band (OOB) Editing where all modification is done by a 
         worker thread. (this is the default)

   The mode setting, along with other inspection parameters are configurable
   via the following registry values

  HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\stmedit
      
      o  StringToFind (REG_SZ, default = "rainy")
      o  StringToReplace (REG_SZ, default = "sunny")
      o  InspectionPort (REG_DWORD, default = 5001)
      o  InspectOutbound (REG_DWORD, default = 0)
      o  EditInline (REG_DWORD, default = 0)

   The sample is IP version agnostic. It performs inspection on both IPv4 and
   IPv6 data streams.

   Before experimenting with the sample, please be sure to add an exception for
   the InspectionPort configured to the firewall. 

Environment:

    Kernel mode

--*/

#include "ntddk.h"

#pragma warning(push)
#pragma warning(disable:4201)       // unnamed struct/union

#include "fwpsk.h"

#pragma warning(pop)

#include "fwpmk.h"

#include "inline_edit.h"
#include "oob_edit.h"
#include "stream_callout.h"

#define INITGUID
#include <guiddef.h>

// 
// Configurable parameters
//

USHORT  configInspectionPort = 5001;
BOOLEAN configInspectionOutbound = FALSE; 
BOOLEAN configEditInline = FALSE;

CHAR configStringToFind[128] = "rainy";
CHAR configStringToReplace[128] = "sunny";
   
// 
// Callout driver keys
//

// e6011cdc-440b-4a6f-8499-6fdb55fb1f92
DEFINE_GUID(
    STREAM_EDITOR_STREAM_CALLOUT_V4,
    0xe6011cdc,
    0x440b,
    0x4a6f,
    0x84, 0x99, 0x6f, 0xdb, 0x55, 0xfb, 0x1f, 0x92
);
// c0bc07b4-aaf6-4242-a3dc-3ef341ffde5d
DEFINE_GUID(
    STREAM_EDITOR_STREAM_CALLOUT_V6,
    0xc0bc07b4,
    0xaaf6,
    0x4242,
    0xa3, 0xdc, 0x3e, 0xf3, 0x41, 0xff, 0xde, 0x5d
);

// 
// Callout driver global variables
//

PMDL gStringToReplaceMdl;

STREAM_EDITOR gStreamEditor;

HANDLE gEngineHandle;
UINT32 gCalloutIdV4;
UINT32 gCalloutIdV6;

PDEVICE_OBJECT gDeviceObject;

HANDLE gInjectionHandle;

PNDIS_GENERIC_OBJECT gNdisGenericObj;
NDIS_HANDLE gNetBufferListPool;

#define STREAM_EDITOR_NDIS_OBJ_TAG 'oneS'
#define STREAM_EDITOR_NBL_POOL_TAG 'pneS'
#define STREAM_EDITOR_FLAT_BUFFER_TAG 'bfeS'


DRIVER_INITIALIZE DriverEntry;
NTSTATUS
DriverEntry(
   IN  PDRIVER_OBJECT  driverObject,
   IN  PUNICODE_STRING registryPath
   );

DRIVER_UNLOAD DriverUnload;
VOID
DriverUnload(
   IN  PDRIVER_OBJECT driverObject
   );


NTSTATUS
StreamEditNotify(
   IN FWPS_CALLOUT_NOTIFY_TYPE notifyType,
   IN const GUID* filterKey,
   IN const FWPS_FILTER* filter
   )
{
   UNREFERENCED_PARAMETER(notifyType);
   UNREFERENCED_PARAMETER(filterKey);
   UNREFERENCED_PARAMETER(filter);

   return STATUS_SUCCESS;
}

NTSTATUS
RegisterCalloutForLayer(
   IN const GUID* layerKey,
   IN const GUID* calloutKey,
   IN void* deviceObject,
   OUT UINT32* calloutId
   )
/* ++

   This function registers callouts and filters that intercept TCP
   traffic at WFP FWPM_LAYER_STREAM_V4 or FWPM_LAYER_STREAM_V6 layer.

-- */
{
   NTSTATUS status = STATUS_SUCCESS;

   FWPS_CALLOUT sCallout = {0};

   FWPM_FILTER filter = {0};
   FWPM_FILTER_CONDITION filterConditions[1] = {0}; 

   FWPM_CALLOUT mCallout = {0};
   FWPM_DISPLAY_DATA displayData = {0};

   BOOLEAN calloutRegistered = FALSE;

   sCallout.calloutKey = *calloutKey;
   sCallout.classifyFn = (configEditInline ? StreamInlineEditClassify :
                                             StreamOobEditClassify);
   sCallout.notifyFn = StreamEditNotify;

   status = FwpsCalloutRegister(
               deviceObject,
               &sCallout,
               calloutId
               );
   if (!NT_SUCCESS(status))
   {
      goto Exit;
   }
   calloutRegistered = TRUE;

   displayData.name = L"Stream Edit Callout";
   displayData.description = L"Callout that finds and replaces a token from a TCP stream";

   mCallout.calloutKey = *calloutKey;
   mCallout.displayData = displayData;
   mCallout.applicableLayer = *layerKey;
   status = FwpmCalloutAdd(
               gEngineHandle,
               &mCallout,
               NULL,
               NULL
               );

   if (!NT_SUCCESS(status))
   {
      goto Exit;
   }

   filter.layerKey = *layerKey;
   filter.displayData.name = L"Stream Edit Filter";
   filter.displayData.description = L"Filter that finds and replaces a token from a TCP stream";

   filter.action.type = FWP_ACTION_CALLOUT_TERMINATING;
   filter.action.calloutKey = *calloutKey;
   filter.filterCondition = filterConditions;
   filter.numFilterConditions = 1;
   filter.subLayerKey = FWPM_SUBLAYER_UNIVERSAL;
   filter.weight.type = FWP_EMPTY; // auto-weight.

   filterConditions[0].fieldKey = (configInspectionOutbound ? FWPM_CONDITION_IP_REMOTE_PORT :
                                                              FWPM_CONDITION_IP_LOCAL_PORT);
   filterConditions[0].matchType = FWP_MATCH_EQUAL;
   filterConditions[0].conditionValue.type = FWP_UINT16;
   filterConditions[0].conditionValue.uint16 = configInspectionPort;

   status = FwpmFilterAdd(
               gEngineHandle,
               &filter,
               NULL,
               NULL);
   if (!NT_SUCCESS(status))
   {
      goto Exit;
   }

Exit:

   if (!NT_SUCCESS(status))
   {
      if (calloutRegistered)
      {
         FwpsCalloutUnregisterById(*calloutId);
      }
   }

   return status;
}

NTSTATUS
StreamEditRegisterCallout(
   IN const STREAM_EDITOR* streamEditor,
   IN void* deviceObject
   )
/* ++

   This function registers dynamic callouts and filters that intercept
   TCP traffic at WFP FWPM_LAYER_STREAM_V4 and FWPM_LAYER_STREAM_V6 
   layer.

   Callouts and filters will be removed during DriverUnload.

-- */
{
   NTSTATUS status = STATUS_SUCCESS;

   BOOLEAN engineOpened = FALSE;
   BOOLEAN inTransaction = FALSE;

   FWPM_SESSION session = {0};

   UNREFERENCED_PARAMETER(streamEditor);

   session.flags = FWPM_SESSION_FLAG_DYNAMIC;

   status = FwpmEngineOpen(
                NULL,
                RPC_C_AUTHN_WINNT,
                NULL,
                &session,
                &gEngineHandle
                );
   if (!NT_SUCCESS(status))
   {
      goto Exit;
   }
   engineOpened = TRUE;

   status = FwpmTransactionBegin(gEngineHandle, 0);
   if (!NT_SUCCESS(status))
   {
      goto Exit;
   }
   inTransaction = TRUE;

   status = RegisterCalloutForLayer(
               &FWPM_LAYER_STREAM_V4,
               &STREAM_EDITOR_STREAM_CALLOUT_V4,
               deviceObject,
               &gCalloutIdV4
               );
   if (!NT_SUCCESS(status))
   {
      goto Exit;
   }

   status = RegisterCalloutForLayer(
               &FWPM_LAYER_STREAM_V6,
               &STREAM_EDITOR_STREAM_CALLOUT_V6,
               deviceObject,
               &gCalloutIdV6
               );
   if (!NT_SUCCESS(status))
   {
      goto Exit;
   }

   status = FwpmTransactionCommit(gEngineHandle);
   if (!NT_SUCCESS(status))
   {
      goto Exit;
   }
   inTransaction = FALSE;

Exit:

   if (!NT_SUCCESS(status))
   {
      if (inTransaction)
      {
         FwpmTransactionAbort(gEngineHandle);
      }
      if (engineOpened)
      {
         FwpmEngineClose(gEngineHandle);
         gEngineHandle = NULL;
      }
   }

   return status;
}

void
StreamEditUnregisterCallout()
{
   FwpmEngineClose(gEngineHandle);
   gEngineHandle = NULL;

   FwpsCalloutUnregisterById(gCalloutIdV6);
   FwpsCalloutUnregisterById(gCalloutIdV4);
}

VOID
DriverUnload(
   IN  PDRIVER_OBJECT driverObject
   )
{

   UNREFERENCED_PARAMETER(driverObject);

   if (!configEditInline)
   {
      OobEditShutdown(&gStreamEditor);
   }

   if (gStreamEditor.scratchBuffer != NULL)
   {
      ExFreePoolWithTag(
         gStreamEditor.scratchBuffer,
         STREAM_EDITOR_FLAT_BUFFER_TAG
         );

   }

   StreamEditUnregisterCallout();

   FwpsInjectionHandleDestroy(gInjectionHandle);

   NdisFreeNetBufferListPool(gNetBufferListPool);
   NdisFreeGenericObject(gNdisGenericObj);

   IoFreeMdl(gStringToReplaceMdl);

   IoDeleteDevice(gDeviceObject);
}

void
StreamEditLoadConfig(
   IN  PUNICODE_STRING registryPath
   )
{
   NTSTATUS status;

   OBJECT_ATTRIBUTES objectAttributes;
   HANDLE registryKey;
   UNICODE_STRING valueName;
   UCHAR regValueStorage[sizeof(KEY_VALUE_PARTIAL_INFORMATION) + 128 * sizeof(WCHAR)]; 
   KEY_VALUE_PARTIAL_INFORMATION* regValue = (KEY_VALUE_PARTIAL_INFORMATION*)regValueStorage;
   ULONG resultLength;

   InitializeObjectAttributes(
      &objectAttributes,
      registryPath,
      OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
      NULL,
      NULL
      );

   status = ZwOpenKey(
               &registryKey,
               KEY_READ,
               &objectAttributes
               );
   if (NT_SUCCESS(status))
   {
      RtlInitUnicodeString(
         &valueName,
         L"StringToFind"
         );

      status = ZwQueryValueKey(
                  registryKey,
                  &valueName,
                  KeyValuePartialInformation,
                  regValue,
                  sizeof(regValueStorage),
                  &resultLength                                                                          
                  );

      if (NT_SUCCESS(status))
      {
         ULONG ansiLength;
         RtlUnicodeToMultiByteN(
            configStringToFind,
            sizeof(configStringToFind),
            &ansiLength,
            (PWCH)regValue->Data,
            regValue->DataLength
            );
      }

      RtlInitUnicodeString(
         &valueName,
         L"StringToReplace"
         );

      status = ZwQueryValueKey(
                  registryKey,
                  &valueName,
                  KeyValuePartialInformation,
                  regValue,
                  sizeof(regValueStorage),
                  &resultLength                                                                          
                  );

      if (NT_SUCCESS(status))
      {
         ULONG ansiLength;
         RtlUnicodeToMultiByteN(
            configStringToReplace,
            sizeof(configStringToReplace),
            &ansiLength,
            (PWCH)regValue->Data,
            regValue->DataLength
            );
      }

      RtlInitUnicodeString(
         &valueName,
         L"InspectionPort"
         );

      status = ZwQueryValueKey(
                  registryKey,
                  &valueName,
                  KeyValuePartialInformation,
                  regValue,
                  sizeof(regValueStorage),
                  &resultLength                                                                          
                  );

      if (NT_SUCCESS(status))
      {
         configInspectionPort = (USHORT)(*(PULONG)regValue->Data);
      }

      RtlInitUnicodeString(
         &valueName,
         L"EditInline"
         );

      status = ZwQueryValueKey(
                  registryKey,
                  &valueName,
                  KeyValuePartialInformation,
                  regValue,
                  sizeof(regValueStorage),
                  &resultLength                                                                          
                  );

      if (NT_SUCCESS(status))
      {
         if ((*(PULONG)regValue->Data) != 0)
         {
            configEditInline = TRUE;
         }
      }

      RtlInitUnicodeString(
         &valueName,
         L"InspectOutbound"
         );

      status = ZwQueryValueKey(
                  registryKey,
                  &valueName,
                  KeyValuePartialInformation,
                  regValue,
                  sizeof(regValueStorage),
                  &resultLength                                                                          
                  );

      if (NT_SUCCESS(status))
      {
         if ((*(PULONG)regValue->Data) != 0)
         {
            configInspectionOutbound = TRUE;
         }
      }

      ZwClose(registryKey);
   }
}

NTSTATUS
DriverEntry(
   IN  PDRIVER_OBJECT  driverObject,
   IN  PUNICODE_STRING registryPath
   )
{
   NTSTATUS status = STATUS_SUCCESS;

   UNICODE_STRING deviceName;
   NET_BUFFER_LIST_POOL_PARAMETERS nblPoolParams = {0};

   StreamEditLoadConfig(registryPath);

   RtlInitUnicodeString(
      &deviceName,
      L"\\Device\\StreamEitor"
      );

   status = IoCreateDevice(
               driverObject, 
               0, 
               &deviceName, 
               FILE_DEVICE_NETWORK, 
               0, 
               FALSE, 
               &gDeviceObject
               );
   if (!NT_SUCCESS(status))
   {
      goto Exit;
   }

   gStringToReplaceMdl = IoAllocateMdl(
                           configStringToReplace,
                           strlen(configStringToReplace),
                           FALSE,
                           FALSE,
                           NULL
                           );
   if (gStringToReplaceMdl == NULL)
   {
      status = STATUS_NO_MEMORY;
      goto Exit;
   }

   MmBuildMdlForNonPagedPool(gStringToReplaceMdl);

   gNdisGenericObj = NdisAllocateGenericObject(
                        driverObject, 
                        STREAM_EDITOR_NDIS_OBJ_TAG, 
                        0
                        );

   if (gNdisGenericObj == NULL)
   {
      status = STATUS_NO_MEMORY;
      goto Exit;
   }

   nblPoolParams.Header.Type = NDIS_OBJECT_TYPE_DEFAULT;
   nblPoolParams.Header.Revision = NET_BUFFER_LIST_POOL_PARAMETERS_REVISION_1;
   nblPoolParams.Header.Size = sizeof(nblPoolParams);

   nblPoolParams.fAllocateNetBuffer = TRUE;
   nblPoolParams.DataSize = 0;

   nblPoolParams.PoolTag = STREAM_EDITOR_NBL_POOL_TAG;

   gNetBufferListPool = NdisAllocateNetBufferListPool(
                           gNdisGenericObj,
                           &nblPoolParams
                           );

   if (gNetBufferListPool == NULL)
   {
      status = STATUS_NO_MEMORY;
      goto Exit;
   }

   status = FwpsInjectionHandleCreate(
               AF_UNSPEC,
               FWPS_INJECTION_TYPE_STREAM,
               &gInjectionHandle
               );

   if (!NT_SUCCESS(status))
   {
      goto Exit;
   }

   status = StreamEditRegisterCallout(
               &gStreamEditor,
               gDeviceObject
               );

   if (!NT_SUCCESS(status))
   {
      goto Exit;
   }

   if (configEditInline)
   {
      InlineEditInit(&gStreamEditor);
   }
   else
   {

      status = OobEditInit(&gStreamEditor);

      if (!NT_SUCCESS(status))
      {
         goto Exit;
      }
   }

   driverObject->DriverUnload = DriverUnload;

Exit:
   
   if (!NT_SUCCESS(status))
   {
      if (gEngineHandle != NULL)
      {
         StreamEditUnregisterCallout();
      }

      if (gInjectionHandle != NULL)
      {
         FwpsInjectionHandleDestroy(gInjectionHandle);
      }
      if (gNetBufferListPool != NULL)
      {
         NdisFreeNetBufferListPool(gNetBufferListPool);
      }
      if (gNdisGenericObj != NULL)
      {
         NdisFreeGenericObject(gNdisGenericObj);
      }
      if (gStringToReplaceMdl != NULL)
      {
         IoFreeMdl(gStringToReplaceMdl);
      }
      if (gDeviceObject)
      {
         IoDeleteDevice(gDeviceObject);
      }
   }

   return status;
}

BOOLEAN
StreamCopyDataForInspection(
   IN STREAM_EDITOR* streamEditor,
   IN const FWPS_STREAM_DATA* streamData
   )
/* ++

   This function copies stream data described by the FWPS_STREAM_DATA
   structure into a flat buffer.

-- */

{
   SIZE_T bytesCopied;

   SIZE_T existingDataLength = streamEditor->dataLength;
   ASSERT(streamEditor->dataOffset == 0);
   if (streamEditor->bufferSize - existingDataLength < streamData->dataLength)
   {
      SIZE_T newBufferSize = (streamData->dataLength + existingDataLength) * 2;
      PVOID newBuffer = ExAllocatePoolWithTag(
                           NonPagedPool,
                           newBufferSize,
                           STREAM_EDITOR_FLAT_BUFFER_TAG
                           );

      if (newBuffer != NULL)
      {
         if (existingDataLength > 0)
         {
            ASSERT(streamEditor->scratchBuffer != NULL);
            RtlCopyMemory(
               newBuffer, 
               streamEditor->scratchBuffer, 
               existingDataLength
               );
         }
      }

      if (streamEditor->scratchBuffer != NULL)
      {
         ExFreePoolWithTag(
            streamEditor->scratchBuffer, 
            STREAM_EDITOR_FLAT_BUFFER_TAG
            );

         streamEditor->scratchBuffer = NULL;
         streamEditor->bufferSize = 0;
         streamEditor->dataLength = 0;
      }

      if (newBuffer != NULL)
      {
         streamEditor->scratchBuffer = newBuffer;
         streamEditor->bufferSize = newBufferSize;
         streamEditor->dataLength = existingDataLength;
      }
      else
      {
         return FALSE;
      }
   }

   FwpsCopyStreamDataToBuffer(
      streamData,
      (BYTE*)streamEditor->scratchBuffer + streamEditor->dataLength,
      streamData->dataLength,
      &bytesCopied
      );

   ASSERT(bytesCopied == streamData->dataLength);

   streamEditor->dataLength += bytesCopied;

   return TRUE;
}



