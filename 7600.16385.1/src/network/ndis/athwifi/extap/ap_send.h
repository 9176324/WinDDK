/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    ap_send.h

Abstract:
    ExtAP send processing functions
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    10-15-2007    Created

Notes:

--*/


#define MAX_SEND_MSDU_TO_PROCESS             5

NDIS_STATUS
ApSendMgmtPacket(
    __in PMP_EXTAP_PORT ApPort,
    __in_bcount(MgmtPacketSize) PUCHAR MgmtPacket,
    __in USHORT MgmtPacketSize
    );

/** 
 * Called with PORT::Lock held
 */
NDIS_STATUS 
Ap11SendEventHandler(
    __in PMP_PORT Port,
    __in PMP_TX_MSDU   PacketList,
    __in ULONG SendFlags
    );

/**
 * Handle send completion event
 */
VOID 
Ap11SendCompleteEventHandler(
    __in PMP_PORT Port,
    __in PMP_TX_MSDU   PacketList,
    __in ULONG SendCompleteFlags
    );


