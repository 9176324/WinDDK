/*++

Copyright (c) Microsoft Corporation. All rights reserved

Abstract:

   Stream Edit Callout Driver Sample.

   This sample demonstrates finding and replacing a string pattern from a
   live TCP stream via the WFP stream API.

--*/

#ifndef _STREAM_CALLOUT_H
#define _STREAM_CALLOUT_H

extern PMDL gStringToReplaceMdl;
extern HANDLE gInjectionHandle;
extern NDIS_HANDLE gNetBufferListPool;
extern STREAM_EDITOR gStreamEditor;

// 
// Configurable parameters
//

extern USHORT  configInspectionPort;
extern BOOLEAN configInspectionOutbound; 
extern BOOLEAN configEditInline;

extern CHAR configStringToFind[];
extern CHAR configStringToReplace[];

#pragma warning(push)
#pragma warning(disable:4201)       // unnamed struct/union

typedef struct STREAM_EDITOR_
{
   BOOLEAN editInline;

   union
   {
      INLINE_EDIT_STATE inlineEditState;
      struct
      {
         OOB_EDIT_STATE editState;
         BOOLEAN shuttingDown;

         KSPIN_LOCK editLock;
         NET_BUFFER_LIST* nblHead;
         NET_BUFFER_LIST* nblTail;
         SIZE_T totalDataLength;
         BOOLEAN noMoreData;
         NET_BUFFER_LIST* nblEof;
         SIZE_T busyThreshold;
         UINT64 flowId;
         UINT32 calloutId;
         UINT16 layerId;
         DWORD streamFlags;
         KEVENT editEvent;
         LIST_ENTRY outgoingDataQueue;
      } oobEditInfo;
   };

   PVOID scratchBuffer;
   SIZE_T bufferSize;
   SIZE_T dataOffset;
   SIZE_T dataLength;

}STREAM_EDITOR;

#pragma warning(pop)

BOOLEAN
StreamCopyDataForInspection(
   IN STREAM_EDITOR* streamEditor,
   IN const FWPS_STREAM_DATA* streamData
   );

extern
NTSYSAPI
NTSTATUS
NTAPI
RtlUnicodeToMultiByteN(
    __out_bcount_part(MaxBytesInMultiByteString, *BytesInMultiByteString) PCHAR MultiByteString,
    __in ULONG MaxBytesInMultiByteString,
    __out_opt PULONG BytesInMultiByteString,
    __in_bcount(BytesInUnicodeString) PWCH UnicodeString,
    __in ULONG BytesInUnicodeString
    );

#endif // _STREAM_CALLOUT_H

