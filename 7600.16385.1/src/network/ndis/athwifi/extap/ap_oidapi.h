/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    ap_oids.h

Abstract:
    ExtAP OID APIs with strong type
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    09-13-2007    Created

Notes:

--*/
#pragma once
    
#ifndef _AP_OIDAPI_H_
#define _AP_OIDAPI_H_

/** OID_DOT11_AUTO_CONFIG_ENABLED */
NDIS_STATUS
ApQueryAutoConfigEnabled(
    __in PMP_EXTAP_PORT ApPort,
    __out PULONG AutoConfigEnabled
    );

NDIS_STATUS
ApSetAutoConfigEnabled(
    __in PMP_EXTAP_PORT ApPort,
    __in ULONG AutoConfigEnabled
    );

/** OID_DOT11_NIC_POWER_STATE */
NDIS_STATUS
ApQueryNicPowerState(
    __in PMP_EXTAP_PORT ApPort,
    __out BOOLEAN * NicPowerState
    );

NDIS_STATUS
ApSetNicPowerState(
    __in PMP_EXTAP_PORT ApPort,
    __in BOOLEAN NicPowerState
    );

/** OID_DOT11_OPERATIONAL_RATE_SET */
NDIS_STATUS
ApQueryOperationalRateSet(
    __in PMP_EXTAP_PORT ApPort,
    __out PDOT11_RATE_SET OperationalRateSet
    );

NDIS_STATUS
ApSetOperationalRateSet(
    __in PMP_EXTAP_PORT ApPort,
    __in PDOT11_RATE_SET OperationalRateSet
    );

/** OID_DOT11_BEACON_PERIOD */
NDIS_STATUS
ApQueryBeaconPeriod(
    __in PMP_EXTAP_PORT ApPort,
    __out PULONG BeaconPeriod
    );

NDIS_STATUS
ApSetBeaconPeriod(
    __in PMP_EXTAP_PORT ApPort,
    __in_range(1, 65535) ULONG BeaconPeriod
    );

/** OID_DOT11_DTIM_PERIOD */
NDIS_STATUS
ApQueryDTimPeriod(
    __in PMP_EXTAP_PORT ApPort,
    __out PULONG DTimPeriod
    );

NDIS_STATUS
ApSetDTimPeriod(
    __in PMP_EXTAP_PORT ApPort,
    __in_range(1, 255) ULONG DTimPeriod
    );

/** OID_DOT11_RTS_THRESHOLD */
NDIS_STATUS
ApQueryRtsThreshold(
    __in PMP_EXTAP_PORT ApPort,
    __out PULONG RtsThreshold
    );

NDIS_STATUS
ApSetRtsThreshold(
    __in PMP_EXTAP_PORT ApPort,
    __in_range(0, 2347) ULONG RtsThreshold
    );

/** OID_DOT11_SHORT_RETRY_LIMIT */
NDIS_STATUS
ApQueryShortRetryLimit(
    __in PMP_EXTAP_PORT ApPort,
    __out PULONG ShortRetryLimit
    );

NDIS_STATUS
ApSetShortRetryLimit(
    __in PMP_EXTAP_PORT ApPort,
    __in_range(1, 255) ULONG ShortRetryLimit
    );

/** OID_DOT11_LONG_RETRY_LIMIT */
NDIS_STATUS
ApQueryLongRetryLimit(
    __in PMP_EXTAP_PORT ApPort,
    __out PULONG LongRetryLimit
    );

NDIS_STATUS
ApSetLongRetryLimit(
    __in PMP_EXTAP_PORT ApPort,
    __in_range(1, 255) ULONG LongRetryLimit
    );

/** OID_DOT11_FRAGMENTATION_THRESHOLD */
NDIS_STATUS
ApQueryFragmentationThreshold(
    __in PMP_EXTAP_PORT ApPort,
    __out PULONG FragmentationThreshold
    );

NDIS_STATUS
ApSetFragmentationThreshold(
    __in PMP_EXTAP_PORT ApPort,
    __in_range(256, 2346) ULONG FragmentationThreshold
    );

/** OID_DOT11_AVAILABLE_CHANNEL_LIST */
NDIS_STATUS
ApQueryAvailableChannelList(
    __in PMP_EXTAP_PORT ApPort,
    __out PDOT11_AVAILABLE_CHANNEL_LIST AvailableChannelList
    );

/** Set not applicable */

/** OID_DOT11_CURRENT_CHANNEL */
NDIS_STATUS
ApQueryCurrentChannel(
    __in PMP_EXTAP_PORT ApPort,
    __out PULONG CurrentChannel
    );

NDIS_STATUS
ApSetCurrentChannel(
    __in PMP_EXTAP_PORT ApPort,
    __in_range(1, 14) ULONG CurrentChannel
    );

/** OID_DOT11_AVAILABLE_FREQUENCY_LIST */
NDIS_STATUS
ApQueryAvailableFrequencyList(
    __in PMP_EXTAP_PORT ApPort,
    __out PDOT11_AVAILABLE_FREQUENCY_LIST AvailableFrequencyList
    );

/** Set not applicable */

/** OID_DOT11_CURRENT_FREQUENCY */
NDIS_STATUS
ApQueryCurrentFrequency(
    __in PMP_EXTAP_PORT ApPort,
    __out PULONG CurrentFrequency
    );

NDIS_STATUS
ApSetCurrentFrequency(
    __in PMP_EXTAP_PORT ApPort,
    __in_range(0, 200) ULONG CurrentFrequency
    );

/** OID_DOT11_DESIRED_SSID_LIST */
NDIS_STATUS
ApQueryDesiredSsidList(
    __in PMP_EXTAP_PORT ApPort,
    __out PDOT11_SSID_LIST SsidList,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

NDIS_STATUS
ApSetDesiredSsidList(
    __in PMP_EXTAP_PORT ApPort,
    __in PDOT11_SSID_LIST SsidList
    );

/** OID_DOT11_EXCLUDE_UNENCRYPTED */
NDIS_STATUS
ApQueryExcludeUnencrypted(
    __in PMP_EXTAP_PORT ApPort,
    __out BOOLEAN * ExcludeUnencrypted
    );

NDIS_STATUS
ApSetExcludeUnencrypted(
    __in PMP_EXTAP_PORT ApPort,
    __in BOOLEAN ExcludeUnencrypted
    );

/** OID_DOT11_STATISTICS */
NDIS_STATUS
ApQueryStatistics(
    __in PMP_EXTAP_PORT ApPort,
    __out_bcount(InformationBufferLength) PVOID  InformationBuffer,
    __range(sizeof(DOT11_STATISTICS), ULONG_MAX) ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

/** Set not applicable */

/** OID_DOT11_PRIVACY_EXEMPTION_LIST */
NDIS_STATUS
ApQueryPrivacyExemptionList(
    __in PMP_EXTAP_PORT ApPort,
    __out PDOT11_PRIVACY_EXEMPTION_LIST PrivacyExemptionList,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

NDIS_STATUS
ApSetPrivacyExemptionList(
    __in PMP_EXTAP_PORT ApPort,
    __in PDOT11_PRIVACY_EXEMPTION_LIST PrivacyExemptionList
    );

/** OID_DOT11_ENABLED_AUTHENTICATION_ALGORITHM */
NDIS_STATUS
ApQueryEnabledAuthenticationAlgorithm(
    __in PMP_EXTAP_PORT ApPort,
    __out PDOT11_AUTH_ALGORITHM_LIST EnabledAuthenticationAlgorithm,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

NDIS_STATUS
ApSetEnabledAuthenticationAlgorithm(
    __in PMP_EXTAP_PORT ApPort,
    __in PDOT11_AUTH_ALGORITHM_LIST EnabledAuthenticationAlgorithm
    );

/** OID_DOT11_SUPPORTED_UNICAST_ALGORITHM_PAIR */
NDIS_STATUS
ApQuerySupportedUnicastAlgorithmPair(
    __in PMP_EXTAP_PORT ApPort,
    __out PDOT11_AUTH_CIPHER_PAIR_LIST SupportedUnicastAlgorithmPair
    );

/** Set not applicable */

/** OID_DOT11_ENABLED_UNICAST_CIPHER_ALGORITHM */
NDIS_STATUS
ApQueryEnabledUnicastCipherAlgorithm(
    __in PMP_EXTAP_PORT ApPort,
    __out PDOT11_CIPHER_ALGORITHM_LIST EnabledUnicastCipherAlgorithm,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

NDIS_STATUS
ApSetEnabledUnicastCipherAlgorithm(
    __in PMP_EXTAP_PORT ApPort,
    __in PDOT11_CIPHER_ALGORITHM_LIST EnabledUnicastCipherAlgorithm
    );

/** OID_DOT11_SUPPORTED_MULTICAST_ALGORITHM_PAIR */
NDIS_STATUS
ApQuerySupportedMulticastAlgorithmPair(
    __in PMP_EXTAP_PORT ApPort,
    __out PDOT11_AUTH_CIPHER_PAIR_LIST SupportedMulticastAlgorithmPair
    );

/** Set not applicable */

/** OID_DOT11_ENABLED_MULTICAST_CIPHER_ALGORITHM */
NDIS_STATUS
ApQueryEnabledMulticastCipherAlgorithm(
    __in PMP_EXTAP_PORT ApPort,
    __out PDOT11_CIPHER_ALGORITHM_LIST EnabledMulticastCipherAlgorithm,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

NDIS_STATUS
ApSetEnabledMulticastCipherAlgorithm(
    __in PMP_EXTAP_PORT ApPort,
    __in PDOT11_CIPHER_ALGORITHM_LIST EnabledMulticastCipherAlgorithm
    );

/** OID_DOT11_CIPHER_DEFAULT_KEY_ID */
NDIS_STATUS
ApQueryCipherDefaultKeyId(
    __in PMP_EXTAP_PORT ApPort,
    __out PULONG CipherDefaultKeyId
    );

NDIS_STATUS
ApSetCipherDefaultKeyId(
    __in PMP_EXTAP_PORT ApPort,
    __in ULONG CipherDefaultKeyId
    );

/** OID_DOT11_CIPHER_DEFAULT_KEY */
/** Query not applicable */

NDIS_STATUS
ApSetCipherDefaultKey(
    __in PMP_EXTAP_PORT ApPort,
    __in PDOT11_CIPHER_DEFAULT_KEY_VALUE CipherDefaultKey
    );

/** OID_DOT11_CIPHER_KEY_MAPPING_KEY */
/** Query not applicable */

NDIS_STATUS
ApSetCipherKeyMappingKey(
    __in PMP_EXTAP_PORT ApPort,
    __in PDOT11_BYTE_ARRAY CipherKeyMappingKeyByteArray
    );

/** OID_DOT11_ENUM_PEER_INFO */
NDIS_STATUS
ApEnumPeerInfo(
    __in PMP_EXTAP_PORT ApPort,
    __out PDOT11_PEER_INFO_LIST PeerInfo,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

/** Set not applicable */

/** OID_DOT11_DISASSOCIATE_REQUEST */
/** Query not applicable */

NDIS_STATUS
ApDisassociatePeerRequest(
    __in PMP_EXTAP_PORT ApPort,
    __in PDOT11_DISASSOCIATE_PEER_REQUEST DisassociateRequest
    );

/** OID_DOT11_DESIRED_PHY_LIST */
NDIS_STATUS
ApQueryDesiredPhyList(
    __in PMP_EXTAP_PORT ApPort,
    __out PDOT11_PHY_ID_LIST DesiredPhyList,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

NDIS_STATUS
ApSetDesiredPhyList(
    __in PMP_EXTAP_PORT ApPort,
    __in PDOT11_PHY_ID_LIST DesiredPhyList
    );

/** OID_DOT11_CURRENT_PHY_ID */
NDIS_STATUS
ApQueryCurrentPhyId(
    __in PMP_EXTAP_PORT ApPort,
    __out PULONG CurrentPhyId
    );

NDIS_STATUS
ApSetCurrentPhyId(
    __in PMP_EXTAP_PORT ApPort,
    __in ULONG CurrentPhyId
    );

/** OID_DOT11_PORT_STATE_NOTIFICATION */
/** Query not applicable */

NDIS_STATUS
ApSetPortStateNotification(
    __in PMP_EXTAP_PORT ApPort,
    __in PDOT11_PORT_STATE_NOTIFICATION PortStateNotification
    );

/** OID_DOT11_SCAN_REQUEST */
/** Query not applicable */

NDIS_STATUS
ApScanRequest(
    __in PMP_EXTAP_PORT ApPort,
    __in PVOID ScanRequestId,
    __in PDOT11_SCAN_REQUEST_V2 ScanRequest,
    __in ULONG ScanRequestBufferLength
    );

/** OID_DOT11_INCOMING_ASSOCIATION_DECISION */
/** Query not applicable */

NDIS_STATUS
ApSetIncomingAssociationDecision(
    __in PMP_EXTAP_PORT ApPort,
    __in PDOT11_INCOMING_ASSOC_DECISION IncomingAssociationDecision
    );

/** OID_DOT11_ADDITIONAL_IE */
NDIS_STATUS
ApQueryAdditionalIe(
    __in PMP_EXTAP_PORT ApPort,
    __out PDOT11_ADDITIONAL_IE AdditionalIe,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

NDIS_STATUS
ApSetAdditionalIe(
    __in PMP_EXTAP_PORT ApPort,
    __in PDOT11_ADDITIONAL_IE AdditionalIe
    );

/** OID_DOT11_WPS_ENABLED */
NDIS_STATUS
ApQueryWpsEnabled(
    __in PMP_EXTAP_PORT ApPort,
    __out BOOLEAN * WpsEnabled
    );

NDIS_STATUS
ApSetWpsEnabled(
    __in PMP_EXTAP_PORT ApPort,
    __in BOOLEAN WpsEnabled
    );


/** OID_DOT11_START_AP_REQUEST */
/** Query not applicable */

NDIS_STATUS
ApStartApRequest(
    __in PMP_EXTAP_PORT ApPort
    );

/** OID_GEN_CURRENT_PACKET_FILTER */
/** Query is handled by the base port */

NDIS_STATUS
ApSetPacketFilter(
    __in PMP_EXTAP_PORT ApPort,
    __in ULONG PacketFilter
    );


/** OID_DOT11_CURRENT_OPERATION_MODE */
/** OID_DOT11_FLUSH_BSS_LIST */
/** OID_DOT11_SCAN_REQUEST */
/** Handled by MP Shim layer */

/** Request methods */
/** OID_DOT11_RESET_REQUEST */
NDIS_STATUS
ApResetRequest(
    __in PMP_EXTAP_PORT ApPort,
    __in PDOT11_RESET_REQUEST ResetRequest,
    __out PDOT11_STATUS_INDICATION StatusIndication
    );

/** OID_DOT11_ENUM_BSS_LIST */
// TODO: move this OID to base port
NDIS_STATUS
ApEnumerateBSSList(
    __in PMP_EXTAP_PORT ApPort,
    __inout PVOID InformationBuffer,
    __in ULONG InputBufferLength,
    __in ULONG OutputBufferLength,
    __out PULONG BytesRead,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

#endif  // _AP_OIDAPI_H_


