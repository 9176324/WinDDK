/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Sta_misc.h

Abstract:
    STA layer utility functions
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
#ifndef _STATION_MISC_H
#define _STATION_MISC_H

#define STA_INVALID_PHY_ID          0x80000000U

#define RSNA_OUI_PREFIX             0xac0f00
#define WPA_OUI_PREFIX              0xf25000

#define RSNA_CIPHER_WEP40           0x01000000
#define RSNA_CIPHER_TKIP            0x02000000
#define RSNA_CIPHER_CCMP            0x04000000
#define RSNA_CIPHER_WEP104          0x05000000

#define RSNA_AKM_RSNA               0x01000000
#define RSNA_AKM_RSNA_PSK           0x02000000

#define RSNA_CAPABILITY_PRE_AUTH    0x01
#define RSNA_CAPABILITY_NO_PAIRWISE 0x02

//
// This structure is used for extracting RSN IE in beacon or probe response frame.
// Both RSN (WPA2) and WPA share the same structure. However, for WPA, PMKIDCount
// is also 0 and PKMID NULL.
// 
typedef struct _RSN_IE_INFO
{
    ULONG   OUIPrefix;
    USHORT  Version;
    USHORT  GroupCipherCount;
    USHORT  PairwiseCipherCount;
    USHORT  AKMSuiteCount;
    USHORT  Capability;
    USHORT  PMKIDCount;
    PUCHAR  GroupCipher;
    PUCHAR  PairwiseCipher;
    PUCHAR  AKMSuite;
    PUCHAR  PMKID;
} RSN_IE_INFO, *PRSN_IE_INFO;

NDIS_STATUS
StaGetRateSetFromInfoEle(
    __in  PUCHAR InfoElemBlobPtr,
    __in  ULONG InfoElemBlobSize,
    __in  BOOLEAN basicRateOnly,
    __out PDOT11_RATE_SET rateSet
    );

NDIS_STATUS
StaFilterUnsupportedRates(
    __in PSTA_BSS_ENTRY pAPEntry, 
    __in PDOT11_RATE_SET rateSet
    );

VOID
StaGenerateRandomBSSID(
    __in  DOT11_MAC_ADDRESS MACAddr,
    __out DOT11_MAC_ADDRESS BSSID
    );

ULONG
StaGetPhyIdByType(
    __in PNIC pNic,
    __in DOT11_PHY_TYPE PhyType
    );

DOT11_PHY_TYPE
StaGetPhyTypeById(
    __in PNIC pNic,
    __in ULONG PhyId
    );

NDIS_STATUS
StaParseRNSIE(
    __in PUCHAR RSNIEData,
    __in ULONG OUIPrefix,
    __in UCHAR RSNIELength,
    __out PRSN_IE_INFO RSNIEInfo
    );

DOT11_CIPHER_ALGORITHM
StaGetGroupCipherFromRSNIEInfo(
    __in PRSN_IE_INFO RSNIEInfo
    );

DOT11_CIPHER_ALGORITHM
StaGetPairwiseCipherFromRSNIEInfo(
    __in PRSN_IE_INFO RSNIEInfo,
    __in USHORT index
    );

DOT11_AUTH_ALGORITHM
StaGetAKMSuiteFromRSNIEInfo(
    __in PRSN_IE_INFO RSNIEInfo,
    __in USHORT index
    );

NDIS_STATUS
StaAttachInfraRSNIE(
    __in PSTATION pStation, 
    __in PSTA_BSS_ENTRY pAPEntry, 
    __inout PUCHAR *ppCurrentIE,
    __inout PUSHORT pIESize
    );

NDIS_STATUS
StaAttachAdHocRSNIE(
    __in PSTATION pStation, 
    __inout PUCHAR *ppCurrentIE,
    __inout PUSHORT pIESize
    );

BOOLEAN
StaMatchRSNInfo(
    __in PSTATION        pStation,
    __in PRSN_IE_INFO    RSNIEInfo
    );

#endif // _STATION_MAIN_H

