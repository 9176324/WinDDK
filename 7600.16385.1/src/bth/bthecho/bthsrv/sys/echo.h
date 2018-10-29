/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    Echo.c

Abstract:

    Contains declaration for echo related functionality

Environment:

    Kernel mode only


--*/

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
BthEchoSrvSendEcho(
    __in PBTHECHOSAMPLE_DEVICE_CONTEXT_HEADER DevCtxHdr,
    __in PBTHECHO_CONNECTION Connection,
    __in PVOID SrcBuffer,
    __in size_t SrcBufferLength
    );

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(BRB, GetEchoRequestContext)    

EVT_WDF_REQUEST_COMPLETION_ROUTINE BthEchoSrvWriteCompletion;  

