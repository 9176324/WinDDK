/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    st_oids.h

Abstract:
    Contains the defines for OID handling for the Station layer layer
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    09-04-2007    Created

Notes:

--*/
#pragma once

typedef NDIS_STATUS (*STA_QUERY_ALGO_PAIR_FUNC)(
    __in  PMP_PORT                        Port,
    __in  DOT11_BSS_TYPE                  BssType,
    __out PDOT11_AUTH_CIPHER_PAIR_LIST    AuthCipherList,
    __in  ULONG                           TotalLength
    );

/** Maximum number of MAC addresses we support in the excluded list */
#define STA_EXCLUDED_MAC_ADDRESS_MAX_COUNT      4
/** Max number of desired BSSIDs we can handle */
#define STA_DESIRED_BSSID_MAX_COUNT             8
/** Max number of desired PHYs we can handle */
#define STA_DESIRED_PHY_MAX_COUNT               5
/** Max number of PMKID we can handle */
#define STA_PMKID_MAX_COUNT                     3
/** Max number of enabled multicast cipher algorithms */
#define STA_MULTICAST_CIPHER_MAX_COUNT          6

VOID
StaInitializeStationConfig(
    __in  PMP_EXTSTA_PORT         Station
    );

VOID
StaResetStationConfig(
    __in  PMP_EXTSTA_PORT         Station
    );

NDIS_STATUS
StaGetAlgorithmPair(
    __in  PMP_PORT                    Port,
    __in  DOT11_BSS_TYPE              BssType,
    __in  STA_QUERY_ALGO_PAIR_FUNC    QueryFunction,
    __deref_out_opt PDOT11_AUTH_CIPHER_PAIR *AlgoPairs,
    __out PULONG                      NumAlgoPairs
    );

NDIS_STATUS
StaQuerySupportedUnicastAlgorithmPairCallback(
    __in  PMP_PORT                Port,
    __in  DOT11_BSS_TYPE          BssType,
    __out_bcount(TotalLength) PDOT11_AUTH_CIPHER_PAIR_LIST    AuthCipherList,
    __in __range(sizeof(DOT11_AUTH_CIPHER_PAIR_LIST) - sizeof(DOT11_AUTH_CIPHER_PAIR), ULONG_MAX)   ULONG                   TotalLength
    );

NDIS_STATUS
StaQuerySupportedMulticastAlgorithmPairCallback(
    __in  PMP_PORT                Port,
    __in  DOT11_BSS_TYPE          BssType,
    __out_bcount(TotalLength) PDOT11_AUTH_CIPHER_PAIR_LIST    AuthCipherList,
    __in __range(sizeof(DOT11_AUTH_CIPHER_PAIR_LIST) - sizeof(DOT11_AUTH_CIPHER_PAIR), ULONG_MAX)   ULONG                   TotalLength
    );

NDIS_STATUS
StaQueryExtStaCapability(
    __in  PMP_PORT                Port,
    __out PDOT11_EXTSTA_CAPABILITY   Dot11ExtStaCap
    );

VOID
StaSetDefaultAuthAlgo(
    __in  PMP_EXTSTA_PORT         Station
    );

VOID
StaSetDefaultCipher(
    __in  PMP_EXTSTA_PORT         Station
    );

NDIS_STATUS
StaSetPowerSavingLevel(
    __in  PMP_EXTSTA_PORT         Station,
    __in  ULONG                   PowerSavingLevel
    );

VOID
StaPowerSleep(
    __in  PMP_EXTSTA_PORT         Station
    );

VOID
StaPowerWakeup(
    __in  PMP_EXTSTA_PORT         Station
    );


