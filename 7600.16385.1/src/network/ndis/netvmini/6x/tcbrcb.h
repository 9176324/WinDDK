/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

   TcbRcb.H

Abstract:

    This module declares the TCB and RCB structures, and the functions to
    manipulate them.

    See the comments in TcbRcb.c.

--*/


#ifndef _TCBRCB_H
#define _TCBRCB_H



//
// TCB (Transmit Control Block)
// -----------------------------------------------------------------------------
//

typedef struct _TCB
{
    LIST_ENTRY              TcbLink;
    PNET_BUFFER             NetBuffer;
    ULONG                   FrameType;
    ULONG                   BytesActuallySent;
} TCB, *PTCB;



VOID
ReturnTCB(
    __in  PMP_ADAPTER  Adapter,
    __in  PTCB         Tcb);


//
// RCB (Receive Control Block)
// -----------------------------------------------------------------------------
//

typedef struct _RCB
{
    LIST_ENTRY              RcbLink;
    PNET_BUFFER_LIST        Nbl;
    PVOID                   Data;
#if defined(NDIS620_MINIPORT)    
    PVOID                   LookaheadData;
#endif
} RCB, *PRCB;

__checkReturn
__success(return != 0)
PRCB
GetRCB(
    __in  PMP_ADAPTER  Adapter,
    __in  PNDIS_NET_BUFFER_LIST_8021Q_INFO Nbl1QInfo,
    __in  PFRAME       Frame);

VOID
ReturnRCB(
    __in  PMP_ADAPTER   Adapter,
    __in  PRCB          Rcb);



#endif // _TCBRCB_H


