/*++

Copyright (c) Microsoft Corporation. All rights reserved

Abstract:

    Stream Edit Callout Driver Sample.
    
    This sample demonstrates inline stream inspection/editing 
    via the WFP stream API.

Environment:

    Kernel mode

--*/

#ifndef _INLINE_EDIT_H
#define _INLINE_EDIT_H

typedef enum INLINE_EDIT_STATE_
{
   INLINE_EDIT_WAITING_FOR_DATA,
   INLINE_EDIT_SKIPPING,
   INLINE_EDIT_MODIFYING,
   INLINE_EDIT_SCANNING
} INLINE_EDIT_STATE;

typedef struct STREAM_EDITOR_ STREAM_EDITOR;

void
InlineEditInit(
   OUT STREAM_EDITOR*
   );

#if(NTDDI_VERSION >= NTDDI_WIN7)
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
   );
#else if(NTDDI_VERSION < NTDDI_WIN7)
void 
NTAPI
StreamInlineEditClassify(
   IN const FWPS_INCOMING_VALUES* inFixedValues,
   IN const FWPS_INCOMING_METADATA_VALUES* inMetaValues,
   IN OUT void* layerData,
   IN const FWPS_FILTER* filter,
   IN UINT64 flowContext,
   OUT FWPS_CLASSIFY_OUT* classifyOut
   );
#endif

#endif // _INLINE_EDIT_H

