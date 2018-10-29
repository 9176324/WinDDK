/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

   DataPath.H

Abstract:

    This module declares the TCB and RCB structures, and the functions to
    manipulate them.

    See the comments in TcbRcb.c.

--*/




#ifndef _DATAPATH_H
#define _DATAPATH_H




NDIS_STATUS
TXNblReference(
    __in  PMP_ADAPTER       Adapter,
    __in  PNET_BUFFER_LIST  NetBufferList);

VOID
TXNblRelease(
    __in  PMP_ADAPTER       Adapter,
    __in  PNET_BUFFER_LIST  NetBufferList,
    __in  BOOLEAN           fAtDispatch);

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
TXSendCompleteDpc(
    __reserved PVOID             SystemSpecific1,
    __in       PVOID             FunctionContext,
    __reserved PVOID             SystemSpecific2,
    __reserved PVOID             SystemSpecific3);


KDEFERRED_ROUTINE RXReceiveIndicateDpc;

VOID
RXDeliverFrameToEveryAdapter(
    __in  PMP_ADAPTER  SendAdapter,
    __in  PNDIS_NET_BUFFER_LIST_8021Q_INFO Nbl1QInfo,
    __in  PFRAME       Frame,
    __in  BOOLEAN      fAtDispatch);

VOID
RXFlushReceiveQueue(
    __in PMP_ADAPTER Adapter,
    __in  PMP_ADAPTER_RECEIVE_DPC AdapterDpc);

VOID
TXFlushSendQueue(
    __in  PMP_ADAPTER  Adapter,
    __in  NDIS_STATUS  CompleteStatus);

VOID
NICStartTheDatapath(
    __in  PMP_ADAPTER  Adapter);

VOID
NICStopTheDatapath(
    __in  PMP_ADAPTER  Adapter);

ULONG
NICGetFrameTypeFromDestination(
    __in_bcount(NIC_MACADDR_SIZE) PUCHAR  DestAddress);

#endif // _DATAPATH_H


