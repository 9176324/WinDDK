/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    client.h

Abstract:

    Contains declarations for Bluetooth client functionality.
    
Environment:

    Kernel mode only


--*/

__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
BthEchoCliBthQueryInterfaces(
    __in PBTHECHOSAMPLE_CLIENT_CONTEXT DevCtx
    );

__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
BthEchoCliRetrieveServerBthAddress(
    __in PBTHECHOSAMPLE_CLIENT_CONTEXT DevCtx
    );

__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
BthEchoCliRetrieveServerSdpRecord(
    __in PBTHECHOSAMPLE_CLIENT_CONTEXT DevCtx,
    __out PBTH_SDP_STREAM_RESPONSE * ServerSdpRecord
    );

NTSTATUS
BthEchoCliRetrievePsmFromSdpRecord(
    __in PBTHDDI_SDP_PARSE_INTERFACE sdpParseInterface,
    __in PBTH_SDP_STREAM_RESPONSE ServerSdpRecord,
    __out USHORT * Psm
    );

__drv_sameIRQL
NTSTATUS
BthEchoCliOpenRemoteConnection(
    __in PBTHECHOSAMPLE_CLIENT_CONTEXT DevCtx,
    __in WDFFILEOBJECT FileObject,
    __in WDFREQUEST Request
    );

