/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    hw_rate.h

Abstract:
    Contains defines used for the rate adaptation logic in the H/W
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    09-04-2007    Created

Notes:

--*/
#pragma once


USHORT
HwSelectTXDataRate(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  PDOT11_RATE_SET         RemoteRateSet,
    __in  ULONG                   LinkQuality
    );


USHORT
HwDetermineTxRate(
    __in  PHW_MAC_CONTEXT         MacContext,
    __in  PHW_PEER_NODE           PeerNode,
    __in  PHW_TX_MSDU             Msdu,
    __in  PDOT11_MAC_HEADER       FragmentHeader
    );

VOID
HwUpdatePeerTxStatistics(
    __in  PHW_MAC_CONTEXT         MacContext,
    __in  PHW_TX_MSDU             Msdu,
    __in  HAL_TX_DESC_STATUS      DescStatus
    );

VOID 
HwUpdateTxDataRate(
    __in  PHW_MAC_CONTEXT         MacContext
    );


