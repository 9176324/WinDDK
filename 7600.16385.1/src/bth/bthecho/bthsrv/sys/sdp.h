/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    Sdp.h

Abstract:

    Contains declarations for Sdp record related functions
    
Environment:

    Kernel mode only


--*/

NTSTATUS
CreateSdpRecord(
    __in PBTHDDI_SDP_NODE_INTERFACE SdpNodeInterface,
    __in PBTHDDI_SDP_PARSE_INTERFACE SdpParseInterface,
    __in const GUID * ClassId,
    __in LPWSTR Name,
    __in USHORT Psm,
    __out PUCHAR * Stream,
    __out ULONG * Size
    );

VOID
FreeSdpRecord(
    PUCHAR SdpRecord
    );

