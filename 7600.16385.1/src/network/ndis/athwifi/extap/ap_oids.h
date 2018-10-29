/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    ap_oids.h

Abstract:
    ExtAP OID processing code
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    09-12-2007    Created

Notes:

--*/
#ifndef _AP_OIDS_H_

#define _AP_OIDS_H_

/** OID_DOT11_AUTO_CONFIG_ENABLED */
NDIS_STATUS
ApOidQueryAutoConfigEnabled(
    __in PMP_EXTAP_PORT ApPort,
    __out_bcount_part(InformationBufferLength, *BytesWritten) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

NDIS_STATUS
ApOidSetAutoConfigEnabled(
    __in PMP_EXTAP_PORT ApPort,
    __in_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesRead,
    __out PULONG BytesNeeded
    );

/** OID_DOT11_NIC_POWER_STATE */
NDIS_STATUS
ApOidQueryNicPowerState(
    __in PMP_EXTAP_PORT ApPort,
    __out_bcount_part(InformationBufferLength, *BytesWritten) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

NDIS_STATUS
ApOidSetNicPowerState(
    __in PMP_EXTAP_PORT ApPort,
    __in_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesRead,
    __out PULONG BytesNeeded
    );

/** OID_DOT11_OPERATIONAL_RATE_SET */
NDIS_STATUS
ApOidQueryOperationalRateSet(
    __in PMP_EXTAP_PORT ApPort,
    __out_bcount_part(InformationBufferLength, *BytesWritten) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

NDIS_STATUS
ApOidSetOperationalRateSet(
    __in PMP_EXTAP_PORT ApPort,
    __in_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesRead,
    __out PULONG BytesNeeded
    );

/** OID_DOT11_BEACON_PERIOD */
NDIS_STATUS
ApOidQueryBeaconPeriod(
    __in PMP_EXTAP_PORT ApPort,
    __out_bcount_part(InformationBufferLength, *BytesWritten) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

NDIS_STATUS
ApOidSetBeaconPeriod(
    __in PMP_EXTAP_PORT ApPort,
    __in_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesRead,
    __out PULONG BytesNeeded
    );

/** OID_DOT11_DTIM_PERIOD */
NDIS_STATUS
ApOidQueryDTimPeriod(
    __in PMP_EXTAP_PORT ApPort,
    __out_bcount_part(InformationBufferLength, *BytesWritten) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

NDIS_STATUS
ApOidSetDTimPeriod(
    __in PMP_EXTAP_PORT ApPort,
    __in_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesRead,
    __out PULONG BytesNeeded
    );

/** OID_DOT11_RTS_THRESHOLD */
NDIS_STATUS
ApOidQueryRtsThreshold(
    __in PMP_EXTAP_PORT ApPort,
    __out_bcount_part(InformationBufferLength, *BytesWritten) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

NDIS_STATUS
ApOidSetRtsThreshold(
    __in PMP_EXTAP_PORT ApPort,
    __in_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesRead,
    __out PULONG BytesNeeded
    );

/** OID_DOT11_SHORT_RETRY_LIMIT */
NDIS_STATUS
ApOidQueryShortRetryLimit(
    __in PMP_EXTAP_PORT ApPort,
    __out_bcount_part(InformationBufferLength, *BytesWritten) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

NDIS_STATUS
ApOidSetShortRetryLimit(
    __in PMP_EXTAP_PORT ApPort,
    __in_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesRead,
    __out PULONG BytesNeeded
    );

/** OID_DOT11_LONG_RETRY_LIMIT */
NDIS_STATUS
ApOidQueryLongRetryLimit(
    __in PMP_EXTAP_PORT ApPort,
    __out_bcount_part(InformationBufferLength, *BytesWritten) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

NDIS_STATUS
ApOidSetLongRetryLimit(
    __in PMP_EXTAP_PORT ApPort,
    __in_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesRead,
    __out PULONG BytesNeeded
    );

/** OID_DOT11_FRAGMENTATION_THRESHOLD */
NDIS_STATUS
ApOidQueryFragmentationThreshold(
    __in PMP_EXTAP_PORT ApPort,
    __out_bcount_part(InformationBufferLength, *BytesWritten) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

NDIS_STATUS
ApOidSetFragmentationThreshold(
    __in PMP_EXTAP_PORT ApPort,
    __in_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesRead,
    __out PULONG BytesNeeded
    );

/** OID_DOT11_AVAILABLE_CHANNEL_LIST */
NDIS_STATUS
ApOidQueryAvailableChannelList(
    __in PMP_EXTAP_PORT ApPort,
    __out_bcount_part(InformationBufferLength, *BytesWritten) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

/** Set not applicable */

/** OID_DOT11_CURRENT_CHANNEL */
NDIS_STATUS
ApOidQueryCurrentChannel(
    __in PMP_EXTAP_PORT ApPort,
    __out_bcount_part(InformationBufferLength, *BytesWritten) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

NDIS_STATUS
ApOidSetCurrentChannel(
    __in PMP_EXTAP_PORT ApPort,
    __in_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesRead,
    __out PULONG BytesNeeded
    );

/** OID_DOT11_AVAILABLE_FREQUENCY_LIST */
NDIS_STATUS
ApOidQueryAvailableFrequencyList(
    __in PMP_EXTAP_PORT ApPort,
    __out_bcount_part(InformationBufferLength, *BytesWritten) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

/** Set not applicable */

/** OID_DOT11_CURRENT_FREQUENCY */
NDIS_STATUS
ApOidQueryCurrentFrequency(
    __in PMP_EXTAP_PORT ApPort,
    __out_bcount_part(InformationBufferLength, *BytesWritten) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

NDIS_STATUS
ApOidSetCurrentFrequency(
    __in PMP_EXTAP_PORT ApPort,
    __in_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesRead,
    __out PULONG BytesNeeded
    );

/** OID_DOT11_DESIRED_SSID_LIST */
NDIS_STATUS
ApOidQueryDesiredSsidList(
    __in PMP_EXTAP_PORT ApPort,
    __out_bcount_part(InformationBufferLength, *BytesWritten) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

NDIS_STATUS
ApOidSetDesiredSsidList(
    __in PMP_EXTAP_PORT ApPort,
    __in_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesRead,
    __out PULONG BytesNeeded
    );

/** OID_DOT11_EXCLUDE_UNENCRYPTED */
NDIS_STATUS
ApOidQueryExcludeUnencrypted(
    __in PMP_EXTAP_PORT ApPort,
    __out_bcount_part(InformationBufferLength, *BytesWritten) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

NDIS_STATUS
ApOidSetExcludeUnencrypted(
    __in PMP_EXTAP_PORT ApPort,
    __in_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesRead,
    __out PULONG BytesNeeded
    );

/** OID_DOT11_STATISTICS */
NDIS_STATUS
ApOidQueryStatistics(
    __in PMP_EXTAP_PORT ApPort,
    __out_bcount_part(InformationBufferLength, *BytesWritten) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

/** Set not applicable */

/** OID_DOT11_PRIVACY_EXEMPTION_LIST */
NDIS_STATUS
ApOidQueryPrivacyExemptionList(
    __in PMP_EXTAP_PORT ApPort,
    __out_bcount_part(InformationBufferLength, *BytesWritten) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

NDIS_STATUS
ApOidSetPrivacyExemptionList(
    __in PMP_EXTAP_PORT ApPort,
    __in_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesRead,
    __out PULONG BytesNeeded
    );

/** OID_DOT11_ENABLED_AUTHENTICATION_ALGORITHM */
NDIS_STATUS
ApOidQueryEnabledAuthenticationAlgorithm(
    __in PMP_EXTAP_PORT ApPort,
    __out_bcount_part(InformationBufferLength, *BytesWritten) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

NDIS_STATUS
ApOidSetEnabledAuthenticationAlgorithm(
    __in PMP_EXTAP_PORT ApPort,
    __in_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesRead,
    __out PULONG BytesNeeded
    );

#if 0
/** OID_DOT11_SUPPORTED_UNICAST_ALGORITHM_PAIR */
NDIS_STATUS
ApOidQuerySupportedUnicastAlgorithmPair(
    __in PMP_EXTAP_PORT ApPort,
    __out_bcount_part(InformationBufferLength, *BytesWritten) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

/** Set not applicable */
#endif
/** OID_DOT11_ENABLED_UNICAST_CIPHER_ALGORITHM */
NDIS_STATUS
ApOidQueryEnabledUnicastCipherAlgorithm(
    __in PMP_EXTAP_PORT ApPort,
    __out_bcount_part(InformationBufferLength, *BytesWritten) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

NDIS_STATUS
ApOidSetEnabledUnicastCipherAlgorithm(
    __in PMP_EXTAP_PORT ApPort,
    __in_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesRead,
    __out PULONG BytesNeeded
    );

#if 0
/** OID_DOT11_SUPPORTED_MULTICAST_ALGORITHM_PAIR */
NDIS_STATUS
ApOidQuerySupportedMulticastAlgorithmPair(
    __in PMP_EXTAP_PORT ApPort,
    __out_bcount_part(InformationBufferLength, *BytesWritten) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

/** Set not applicable */
#endif
/** OID_DOT11_ENABLED_MULTICAST_CIPHER_ALGORITHM */
NDIS_STATUS
ApOidQueryEnabledMulticastCipherAlgorithm(
    __in PMP_EXTAP_PORT ApPort,
    __out_bcount_part(InformationBufferLength, *BytesWritten) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

NDIS_STATUS
ApOidSetEnabledMulticastCipherAlgorithm(
    __in PMP_EXTAP_PORT ApPort,
    __in_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesRead,
    __out PULONG BytesNeeded
    );

/** OID_DOT11_CIPHER_DEFAULT_KEY_ID */
NDIS_STATUS
ApOidQueryCipherDefaultKeyId(
    __in PMP_EXTAP_PORT ApPort,
    __out_bcount_part(InformationBufferLength, *BytesWritten) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

NDIS_STATUS
ApOidSetCipherDefaultKeyId(
    __in PMP_EXTAP_PORT ApPort,
    __in_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesRead,
    __out PULONG BytesNeeded
    );

/** OID_DOT11_CIPHER_DEFAULT_KEY */
/** Query not applicable */

NDIS_STATUS
ApOidSetCipherDefaultKey(
    __in PMP_EXTAP_PORT ApPort,
    __in_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesRead,
    __out PULONG BytesNeeded
    );

/** OID_DOT11_CIPHER_KEY_MAPPING_KEY */
/** Query not applicable */

NDIS_STATUS
ApOidSetCipherKeyMappingKey(
    __in PMP_EXTAP_PORT ApPort,
    __in_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesRead,
    __out PULONG BytesNeeded
    );

/** OID_DOT11_ENUM_PEER_INFO */
NDIS_STATUS
ApOidEnumPeerInfo(
    __in PMP_EXTAP_PORT ApPort,
    __out_bcount_part(InformationBufferLength, *BytesWritten) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

/** Set not applicable */

/** OID_DOT11_DISASSOCIATE_REQUEST */
/** Query not applicable */

NDIS_STATUS
ApOidDisassociateRequest(
    __in PMP_EXTAP_PORT ApPort,
    __in_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesRead,
    __out PULONG BytesNeeded
    );

/** OID_DOT11_DESIRED_PHY_LIST */
NDIS_STATUS
ApOidQueryDesiredPhyList(
    __in PMP_EXTAP_PORT ApPort,
    __out_bcount_part(InformationBufferLength, *BytesWritten) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

NDIS_STATUS
ApOidSetDesiredPhyList(
    __in PMP_EXTAP_PORT ApPort,
    __in_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesRead,
    __out PULONG BytesNeeded
    );

/** OID_DOT11_CURRENT_PHY_ID */
NDIS_STATUS
ApOidQueryCurrentPhyId(
    __in PMP_EXTAP_PORT ApPort,
    __out_bcount_part(InformationBufferLength, *BytesWritten) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

NDIS_STATUS
ApOidSetCurrentPhyId(
    __in PMP_EXTAP_PORT ApPort,
    __in_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesRead,
    __out PULONG BytesNeeded
    );

/** OID_DOT11_EXTAP_CAPABILITY */
NDIS_STATUS
ApOidQueryExtApCapability(
    __in PMP_EXTAP_PORT ApPort,
    __out_bcount_part(InformationBufferLength, *BytesWritten) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

/** Set not applicable */

/** OID_DOT11_PORT_STATE_NOTIFICATION */
/** Query not applicable */

NDIS_STATUS
ApOidSetPortStateNotification(
    __in PMP_EXTAP_PORT ApPort,
    __in_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesRead,
    __out PULONG BytesNeeded
    );

/** OID_DOT11_SCAN_REQUEST */
/** Query not applicable */

NDIS_STATUS
ApOidScanRequest(
    __in PMP_EXTAP_PORT ApPort,
    __in_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesRead,
    __out PULONG BytesNeeded
    );

/** OID_DOT11_INCOMING_ASSOCIATION_DECISION */
/** Query not applicable */

NDIS_STATUS
ApOidSetIncomingAssociationDecision(
    __in PMP_EXTAP_PORT ApPort,
    __in_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesRead,
    __out PULONG BytesNeeded
    );

/** OID_DOT11_ADDITIONAL_IE */
NDIS_STATUS
ApOidQueryAdditionalIe(
    __in PMP_EXTAP_PORT ApPort,
    __out_bcount_part(InformationBufferLength, *BytesWritten) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

NDIS_STATUS
ApOidSetAdditionalIe(
    __in PMP_EXTAP_PORT ApPort,
    __in_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesRead,
    __out PULONG BytesNeeded
    );

/** OID_DOT11_WPS_ENABLED */
NDIS_STATUS
ApOidQueryWpsEnabled(
    __in PMP_EXTAP_PORT ApPort,
    __out_bcount_part(InformationBufferLength, *BytesWritten) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

NDIS_STATUS
ApOidSetWpsEnabled(
    __in PMP_EXTAP_PORT ApPort,
    __in_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesRead,
    __out PULONG BytesNeeded
    );


/** OID_DOT11_START_AP_REQUEST */
/** Query not applicable */

NDIS_STATUS
ApOidStartApRequest(
    __in PMP_EXTAP_PORT ApPort,
    __in_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesRead,
    __out PULONG BytesNeeded
    );

/** OID_GEN_CURRENT_PACKET_FILTER */
/** Query is handled by the base port */

NDIS_STATUS
ApOidSetPacketFilter(
    __in PMP_EXTAP_PORT ApPort,
    __in_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesRead,
    __out PULONG BytesNeeded
    );

/** OID_DOT11_CURRENT_OPERATION_MODE */
/** OID_DOT11_ENUM_BSS_LIST */
/** OID_DOT11_FLUSH_BSS_LIST */
/** Handled by MP Shim layer */

/** Request methods */
/** OID_DOT11_RESET_REQUEST */
NDIS_STATUS
ApOidResetRequest(
    __in PMP_EXTAP_PORT ApPort,
    __in PNDIS_OID_REQUEST NdisOidRequest
    );

/** OID_DOT11_ENUM_BSS_LIST */
// TODO: move this OID to base port
NDIS_STATUS
ApOidEnumerateBSSList(
    __in PMP_EXTAP_PORT ApPort,
    __inout PVOID InformationBuffer,
    __in ULONG InputBufferLength,
    __in ULONG OutputBufferLength,
    __out PULONG BytesRead,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

#endif  // _AP_OIDS_H_

