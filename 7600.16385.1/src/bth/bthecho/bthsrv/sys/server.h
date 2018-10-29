/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    Server.h

Abstract:

    Contains declarations for Bluetooth server functionality.

Environment:

    Kernel mode only


--*/

__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
BthEchoSrvInitialize(
    __in PBTHECHOSAMPLE_SERVER_CONTEXT DevCtx
    );

__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
BthEchoSrvRegisterPSM(
    __in PBTHECHOSAMPLE_SERVER_CONTEXT DevCtx
    );

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
BthEchoSrvUnregisterPSM(
    __in PBTHECHOSAMPLE_SERVER_CONTEXT DevCtx
    );

__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
BthEchoSrvRegisterL2CAPServer(
    __in PBTHECHOSAMPLE_SERVER_CONTEXT DevCtx
    );

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
BthEchoSrvUnregisterL2CAPServer(
    __in PBTHECHOSAMPLE_SERVER_CONTEXT DevCtx
    );

__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
BthEchoSrvPublishSdpRecord(
    __in PBTHECHOSAMPLE_SERVER_CONTEXT DevCtx,
    __in PUCHAR SdpRecord,
    __in ULONG  SdpRecordLength
    );

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
BthEchoSrvRemoveSdpRecord(
    __in PBTHECHOSAMPLE_SERVER_CONTEXT DevCtx
    );

__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
BthEchoSrvSendConnectResponse(
    __in PBTHECHOSAMPLE_SERVER_CONTEXT DevCtx,
    __in PINDICATION_PARAMETERS ConnectParams
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
BthEchoSrvDisconnectConnection(
    __in PBTHECHO_CONNECTION Connection
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
BthEchoSrvDisconnectConnectionsOnRemove(
    __in PBTHECHOSAMPLE_SERVER_CONTEXT devCtx
    );

EVT_WDF_REQUEST_COMPLETION_ROUTINE BthEchoSrvRemoteConnectResponseCompletion;
