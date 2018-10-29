/*++

Copyright (c) Microsoft Corporation. All rights reserved

Abstract:

    Stream Edit Callout Driver Sample.
    
    This sample demonstrates inline stream inspection/editing 
    via the WFP stream API.

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

void
InlineEditInit(
   OUT STREAM_EDITOR* streamEditor
   )
{
   streamEditor->editInline = TRUE;
   streamEditor->inlineEditState = INLINE_EDIT_WAITING_FOR_DATA;
}

void 
NTAPI StreamInjectCompletionFn(
   IN void* context,
   IN OUT NET_BUFFER_LIST* netBufferList,
   IN BOOLEAN dispatchLevel
   )
{
   PMDL mdl = (PMDL)context;

   UNREFERENCED_PARAMETER(dispatchLevel);

   if (mdl != NULL)
   {
      IoFreeMdl(mdl);
   }

   FwpsFreeNetBufferList(netBufferList);
}

NTSTATUS
StreamEditFlushData(
   IN STREAM_EDITOR* streamEditor,
   IN UINT64 flowId,
   IN UINT32 calloutId,
   IN UINT16 layerId,
   IN UINT32 streamFlags
   )
/* ++

   This function re-injects buffered data back to the data stream upon
   receiving a FIN. The data was buffered because it was not big enough
   (size wise) to make an editing decision.

-- */
{
   NTSTATUS status;

   PMDL mdl = NULL;
   NET_BUFFER_LIST* netBufferList = NULL;

   ASSERT(streamEditor->dataOffset == 0);

   mdl = IoAllocateMdl(
            streamEditor->scratchBuffer,
            (ULONG)(streamEditor->dataLength),
            FALSE,
            FALSE,
            NULL
            );

   if (mdl == NULL)
   {
      status = STATUS_NO_MEMORY;
      goto Exit;
   }

   MmBuildMdlForNonPagedPool(mdl);

   status = FwpsAllocateNetBufferAndNetBufferList(
                  gNetBufferListPool,
                  0,
                  0,
                  mdl,
                  0,
                  streamEditor->dataLength,
                  &netBufferList
                  );
   if (!NT_SUCCESS(status))
   {
      goto Exit;
   }

   streamFlags &= ~(FWPS_STREAM_FLAG_SEND_DISCONNECT | FWPS_STREAM_FLAG_RECEIVE_DISCONNECT);

   status = FwpsStreamInjectAsync(
               gInjectionHandle,
               NULL,
               0,
               flowId,
               calloutId,
               layerId,
               streamFlags, 
               netBufferList,
               streamEditor->dataLength,
               StreamInjectCompletionFn,
               mdl
               );
   if (!NT_SUCCESS(status))
   {
      goto Exit;
   }

   mdl = NULL;
   netBufferList = NULL;

Exit:

   if (mdl != NULL)
   {
      IoFreeMdl(mdl);
   }
   if (netBufferList != NULL)
   {
      FwpsFreeNetBufferList(netBufferList);
   }

   return status;
}

void
StreamInlineEdit(
   IN STREAM_EDITOR* streamEditor,
   IN const FWPS_INCOMING_VALUES* inFixedValues,
   IN const FWPS_INCOMING_METADATA_VALUES* inMetaValues,
   IN const FWPS_FILTER* filter,
   IN const FWPS_STREAM_DATA* streamData,
   IN OUT FWPS_STREAM_CALLOUT_IO_PACKET* ioPacket,
   OUT FWPS_CLASSIFY_OUT* classifyOut
   )
/* ++

   This function implements the state machine that scans the content 
   and computes the number of bytes to permit, bytes to block, and 
   performs stream injection to replace the blocked data.

-- */
{
   UINT findLength = strlen(configStringToFind);
   UINT replaceLength = strlen(configStringToReplace);

   if ((streamData->flags & FWPS_STREAM_FLAG_SEND_DISCONNECT) || 
       (streamData->flags & FWPS_STREAM_FLAG_RECEIVE_DISCONNECT))
   {
      if (streamEditor->dataLength > 0)
      {
         StreamEditFlushData(
            streamEditor,
            inMetaValues->flowHandle,
            filter->action.calloutId,
            inFixedValues->layerId,
            streamData->flags
            );

         streamEditor->dataLength = 0;
         streamEditor->dataOffset = 0;
      }

      ASSERT(streamEditor->inlineEditState == INLINE_EDIT_WAITING_FOR_DATA);

      ioPacket->streamAction = FWPS_STREAM_ACTION_NONE;
      classifyOut->actionType = FWP_ACTION_PERMIT;
      goto Exit;
   }

   if (streamData->dataLength == 0)
   {
      ioPacket->streamAction = FWPS_STREAM_ACTION_NONE;
      classifyOut->actionType = FWP_ACTION_PERMIT;
      goto Exit;
   }

   if (streamEditor->inlineEditState != INLINE_EDIT_SKIPPING)
   {
      if ((streamData->dataLength < findLength) && 
          !(classifyOut->flags & FWPS_CLASSIFY_OUT_FLAG_NO_MORE_DATA))
      {
         ioPacket->streamAction = FWPS_STREAM_ACTION_NEED_MORE_DATA;
         ioPacket->countBytesRequired = findLength;

         classifyOut->actionType = FWP_ACTION_NONE;
         goto Exit;
      }
   }

   switch (streamEditor->inlineEditState)
   {
      case INLINE_EDIT_WAITING_FOR_DATA:
      {
         if (StreamCopyDataForInspection(
               streamEditor,
               streamData
               ) == FALSE)
         {
            ioPacket->streamAction = FWPS_STREAM_ACTION_DROP_CONNECTION;
            classifyOut->actionType = FWP_ACTION_NONE;
            goto Exit;
         }

         //
         // Pass-thru to scanning
         //
      }
      case INLINE_EDIT_SCANNING:
      {
         UINT i;
         BYTE* dataStart =  (BYTE*)streamEditor->scratchBuffer + streamEditor->dataOffset;
         BOOLEAN found = FALSE;

         for (i = 0; i < streamEditor->dataLength; ++i)
         {
            if (i + findLength <= streamEditor->dataLength)
            {
               if (RtlCompareMemory(
                     dataStart + i,
                     configStringToFind,
                     findLength
                     ) == findLength)
               {
                  found = TRUE;

                  streamEditor->inlineEditState = INLINE_EDIT_MODIFYING;

                  if (i != 0)
                  {
                     ioPacket->streamAction = FWPS_STREAM_ACTION_NONE;
                     ioPacket->countBytesEnforced = i;

                     classifyOut->actionType = FWP_ACTION_PERMIT;

                     streamEditor->dataOffset += i;
                     streamEditor->dataLength -= i;

                     break;
                  }
                  else
                  {
                     goto modify_data;
                  }
               }
            }
            else
            {
               if (classifyOut->flags & FWPS_CLASSIFY_OUT_FLAG_NO_MORE_DATA)
               {
                  break;
               }

               if (RtlCompareMemory(
                     dataStart + i,
                     configStringToFind,
                     streamEditor->dataLength - i
                     ) == streamEditor->dataLength - i)
               {
                  found = TRUE;  // this is a partial find
   
                  ioPacket->streamAction = FWPS_STREAM_ACTION_NONE;
                  ioPacket->countBytesEnforced = i;

                  classifyOut->actionType = FWP_ACTION_PERMIT;

                  RtlMoveMemory(
                     streamEditor->scratchBuffer,
                     dataStart + i,
                     streamEditor->dataLength - i
                     );

                  streamEditor->dataOffset = 0;
                  streamEditor->dataLength = streamEditor->dataLength - i;

                  streamEditor->inlineEditState = INLINE_EDIT_SKIPPING;
   
                  break;
               }
            }
         }

         if (!found)
         {
            ioPacket->streamAction = FWPS_STREAM_ACTION_NONE;
            ioPacket->countBytesEnforced = 0;

            classifyOut->actionType = FWP_ACTION_PERMIT;

            streamEditor->dataOffset = 0;
            streamEditor->dataLength = 0;

            streamEditor->inlineEditState = INLINE_EDIT_WAITING_FOR_DATA;
         }

         break;
      }
      case INLINE_EDIT_SKIPPING:
      {
         ioPacket->streamAction = FWPS_STREAM_ACTION_NONE;
         ioPacket->countBytesEnforced = 0;

         classifyOut->actionType = FWP_ACTION_BLOCK;

         streamEditor->inlineEditState = INLINE_EDIT_WAITING_FOR_DATA;

         break;
      }
      case INLINE_EDIT_MODIFYING:

modify_data:
      
      {
         NTSTATUS status;
         NET_BUFFER_LIST* netBufferList;

         status = FwpsAllocateNetBufferAndNetBufferList(
                     gNetBufferListPool,
                     0,
                     0,
                     gStringToReplaceMdl,
                     0,
                     replaceLength,
                     &netBufferList
                     );

         if (!NT_SUCCESS(status))
         {
            ioPacket->streamAction = FWPS_STREAM_ACTION_DROP_CONNECTION;
            classifyOut->actionType = FWP_ACTION_NONE;
            goto Exit;
         }

         status = FwpsStreamInjectAsync(
                     gInjectionHandle,
                     NULL,
                     0,
                     inMetaValues->flowHandle,
                     filter->action.calloutId,
                     inFixedValues->layerId,
                     streamData->flags,
                     netBufferList,
                     replaceLength,
                     StreamInjectCompletionFn,
                     NULL
                     );

         if (!NT_SUCCESS(status))
         {
            FwpsFreeNetBufferList(netBufferList);

            ioPacket->streamAction = FWPS_STREAM_ACTION_DROP_CONNECTION;
            classifyOut->actionType = FWP_ACTION_NONE;
            goto Exit;
         }

         ioPacket->streamAction = FWPS_STREAM_ACTION_NONE;
         ioPacket->countBytesEnforced = findLength;

         classifyOut->actionType = FWP_ACTION_BLOCK;

         streamEditor->dataOffset += findLength;
         streamEditor->dataLength -= findLength;

         if (streamEditor->dataLength > 0)
         {
            streamEditor->inlineEditState = INLINE_EDIT_SCANNING;
         }
         else
         {
            streamEditor->dataOffset = 0;

            streamEditor->inlineEditState = INLINE_EDIT_WAITING_FOR_DATA;
         }

         break;
      }
      default:
         ASSERT(FALSE);
         break;
   };

Exit:

   return;
}

#if (NTDDI_VERSION >= NTDDI_WIN7)
void 
NTAPI
StreamInlineEditClassify(
   IN const FWPS_INCOMING_VALUES* inFixedValues,
   IN const FWPS_INCOMING_METADATA_VALUES* inMetaValues,
   IN OUT void* layerData,
   IN const void* classifyContext,
   IN const FWPS_FILTER* filter,
   IN UINT64 flowContext,
   OUT FWPS_CLASSIFY_OUT* classifyOut
   )
#else if (NTDDI_VERSION < NTDDI_WIN7)
void 
NTAPI
StreamInlineEditClassify(
   IN const FWPS_INCOMING_VALUES* inFixedValues,
   IN const FWPS_INCOMING_METADATA_VALUES* inMetaValues,
   IN OUT void* layerData,
   IN const FWPS_FILTER* filter,
   IN UINT64 flowContext,
   OUT FWPS_CLASSIFY_OUT* classifyOut
   )
#endif
/* ++

   This is the ClassifyFn function registered by the inline stream edit callout.

   An inline stream modification callout performs editing from within the
   ClassifyFn call by permitting sections of the content and replacing other
   sections by removing them and injecting new content.

-- */
{
   FWPS_STREAM_CALLOUT_IO_PACKET* ioPacket;
   FWPS_STREAM_DATA* streamData;

   ioPacket = (FWPS_STREAM_CALLOUT_IO_PACKET*)layerData;
   ASSERT(ioPacket != NULL);

   streamData = ioPacket->streamData;
   ASSERT(streamData != NULL);

   #if (NTDDI_VERSION >= NTDDI_WIN7) 
   UNREFERENCED_PARAMETER(classifyContext);
   #endif
   UNREFERENCED_PARAMETER(flowContext);

   //
   // Let go the traffic that the editor does not care about.
   //

   if ((configInspectionOutbound  && (streamData->flags & FWPS_STREAM_FLAG_RECEIVE)) ||
       (!configInspectionOutbound && (streamData->flags & FWPS_STREAM_FLAG_SEND)))
   {
      ioPacket->streamAction = FWPS_STREAM_ACTION_NONE;
      classifyOut->actionType = FWP_ACTION_PERMIT;
      goto Exit;
   }

   //
   // In this sample we don't edit TCP urgent data
   //

   if ((streamData->flags & FWPS_STREAM_FLAG_SEND_EXPEDITED) ||
       (streamData->flags & FWPS_STREAM_FLAG_RECEIVE_EXPEDITED))
   {
      ioPacket->streamAction = FWPS_STREAM_ACTION_NONE;
      classifyOut->actionType = FWP_ACTION_PERMIT;
      goto Exit;
   }

   StreamInlineEdit(
      &gStreamEditor,
      inFixedValues,
      inMetaValues,
      filter,
      streamData,
      ioPacket,
      classifyOut
      );

Exit:

   return;
}

