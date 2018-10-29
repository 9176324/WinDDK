/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    hw_utils.h

Abstract:
    Contains defines for helper routines for the HW layer
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    09-04-2007    Created

Notes:

--*/
#pragma once

NDIS_STATUS
HwGetAdapterStatus(
    __in  PHW                     Hw
    );

PHW_PEER_NODE
HwFindPeerNode(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in_bcount(DOT11_ADDRESS_SIZE)  DOT11_MAC_ADDRESS       MacAddr,
    __in  BOOLEAN                 Allocate
    );

NDIS_STATUS
HwFindKeyMappingKeyIndex(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in_bcount(DOT11_ADDRESS_SIZE)  DOT11_MAC_ADDRESS       MacAddr,
    __in  BOOLEAN                 Allocate,
    __out PULONG                  ReturnIndex
    );

ULONG64
HwDataRateToLinkSpeed(
    UCHAR  rate
    );

UCHAR
HwLinkSpeedToDataRate(
    ULONG64 linkSpeed
    );


ULONG
HwChannelToFrequency(
    __in  UCHAR               Channel
    );

NDIS_STATUS
HwTranslateChannelFreqToLogicalID(
    __in_ecount(NumChannels)  PULONG  ChannelFreqList,
    __in  ULONG                   NumChannels,
    __out_ecount(NumChannels) PULONG  LogicalChannelList
    );

BOOLEAN
HwSelectLowestMatchingRate(
    PUCHAR          remoteRatesIE,
    UCHAR           remoteRatesIELength,
    PDOT11_RATE_SET localRateSet,
    PUSHORT         SelectedRate
    );

VOID
HwIndicateLinkSpeed(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  USHORT                  DataRate
    );

VOID
HwIndicateMICFailure(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  PDOT11_DATA_SHORT_HEADER Header,
    __in  ULONG                   KeyId,
    __in  BOOLEAN                 IsDefaultKey
    );

VOID
HwIndicatePhyPowerState(
    __in  PHW                     Hw,
    __in  ULONG                   PhyId
    );


