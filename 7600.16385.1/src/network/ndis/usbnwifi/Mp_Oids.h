/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Mp_Oids.h

Abstract:
    OID processing code
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
#ifndef _MP_OIDS_H_

#define _MP_OIDS_H_

VOID
MpQuerySupportedOidsList(
    __inout PNDIS_OID *SupportedOidList,
    __inout PULONG    SupportedOidListLength
    );

NDIS_STATUS
MpQuerySupportedPHYTypes(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpResetRequest(
    __in  PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in  ULONG InputBufferLength,
    __in  ULONG OutputBufferLength,
    __out PULONG BytesRead,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

NDIS_STATUS
MpSetMulticastList(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryMPDUMaxLength(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryOperationModeCapability(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryCurrentOperationMode(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryATIMWindow(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryIndicateTXStatus(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryNicPowerState(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryOptionalCapability(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryCurrentOptionalCapability(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryCFPollable(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryPowerMgmtMode(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryOperationalRateSet(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryBeaconPeriod(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryRTSThreshold(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryShortRetryLimit(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryLongRetryLimit(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryFragmentationThreshold(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryMaxTXMSDULifeTime(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryMaxReceiveLifeTime(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryCurrentRegDomain(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryTempType(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryCurrentTXAntenna(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryDiversitySupport(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryCurrentRXAntenna(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQuerySupportedPowerLevels(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpPnPQueryPower(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength
    );

NDIS_STATUS
MpQueryCurrentTXPowerLevel(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryCurrentChannel(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryCCAModeSupported(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryCurrentCCA(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryEDThreshold(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryRegDomainsSupportValue(
    __in PADAPTER pAdapter,
    __out_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQuerySupportedTXAntenna(
    __in PADAPTER pAdapter,
    __out_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQuerySupportedRXAntenna(
    __in PADAPTER pAdapter,
    __out_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryDiversitySelectionRX(
    __in PADAPTER pAdapter,
    __out_bcount(InformationBufferLength) PVOID  InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG                  BytesWritten,
    __out PULONG                  BytesNeeded
    );

NDIS_STATUS
MpQuerySupportedDataRatesValue(
    __in PADAPTER pAdapter,
    __inout_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpSetATIMWindow(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpSetIndicateTXStatus(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpSetNicPowerState(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryStationId(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryPnPCapabilities(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpSetPower(
    __in  PADAPTER        pAdapter,
    __in  PVOID           InformationBuffer,
    __in  ULONG           InformationBufferLength,
    __in  PULONG          pulBytesNeeded,
    __in  PULONG          pulBytesRead
    );

NDIS_STATUS
MpAddWakeUpPattern(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpRemoveWakeUpPattern(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpSetOperationalRateSet(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpSetBeaconPeriod(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpSetRTSThreshold(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpSetFragmentationThreshold(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpSetCurrentRegDomain(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );
    
NDIS_STATUS
MpSetCurrentChannel(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpSetCurrentOperationMode(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpScanRequest(
    PADAPTER pAdapter,
    __in PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesRead,
    __out PULONG BytesNeeded
    );



NDIS_STATUS
MpSetPacketFilter(
    __in PADAPTER pAdapter,
    __in ULONG PacketFilter
    );

NDIS_STATUS
MpEnumerateBSSList(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InputBufferLength,
    __in ULONG OutputBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryDesiredSSIDList(
    __in PADAPTER pAdapter,
    __inout_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryExcludedMACAddressList(
    __in PADAPTER pAdapter,
    __inout_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryDesiredBSSIDList(
    __in PADAPTER pAdapter,
    __out_bcount(InformationBufferLength) PVOID  InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG                  BytesWritten,
    __out PULONG                  BytesNeeded
    );

NDIS_STATUS
MpQueryDesiredBSSType(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryDot11Statistics(
    __in PADAPTER pAdapter,
    __inout_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryEnabledAuthenticationAlgorithm(
    __in PADAPTER pAdapter,
    __inout_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQuerySupportedUnicastAlgorithmPair(
    __in PADAPTER pAdapter,
    __out_bcount(InformationBufferLength) PVOID  InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG                  BytesWritten,
    __out PULONG                  BytesNeeded
    );

NDIS_STATUS
MpQuerySupportedMulticastAlgorithmPair(
    __in PADAPTER pAdapter,
    __out_bcount(InformationBufferLength) PVOID  InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG                  BytesWritten,
    __out PULONG                  BytesNeeded
    );

NDIS_STATUS
MpQueryEnabledUnicastCipherAlgorithm(
    __in PADAPTER pAdapter,
    __out_bcount(InformationBufferLength) PVOID  InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG                  BytesWritten,
    __out PULONG                  BytesNeeded
    );

NDIS_STATUS
MpQueryEnabledMulticastCipherAlgorithm(
    __in PADAPTER pAdapter,
    __out_bcount(InformationBufferLength) PVOID  InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG                  BytesWritten,
    __out PULONG                  BytesNeeded
    );

NDIS_STATUS
MpEnumerateAssociationInformation(
    __in PADAPTER pAdapter,
    __inout_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryHardwarePHYState(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryDesiredPHYList(
    __in PADAPTER pAdapter,
    __inout_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryCurrentPHYID(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryMediaSteamingOption(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryUnreachableDetectionThreshold(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryActivePHYList(
    __in PADAPTER pAdapter,
    __inout_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );
    
NDIS_STATUS
MpQueryDot11TableSize(
    __in PADAPTER pAdapter,
    __inout_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryDataRateMappingTable(
    __in PADAPTER pAdapter,
    __inout_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpFlushBSSList(
    __in PADAPTER pAdapter
    );

NDIS_STATUS
MpSetPowerMgmtMode(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpSetDesiredSSIDList(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpSetExcludedMACAddressList(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpSetDesiredBSSIDList(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpSetDesiredBSSType(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpConnectRequest(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );


NDIS_STATUS
MpDisconnectRequest(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpSetEnabledAuthenticationAlgorithm(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpSetDesiredPHYList(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpSetCurrentPHYID(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );
    
NDIS_STATUS
MpSetMediaStreamingOption(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );
    
NDIS_STATUS
MpSetUnreachableDetectionThreshold(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );


NDIS_STATUS
MpQueryInformation(
    __in    NDIS_HANDLE               MiniportAdapterContext,
    __in    NDIS_OID                  Oid,
    __in    PVOID                     InformationBuffer,
    __in    ULONG                     InformationBufferLength,
    __out   PULONG                    BytesWritten,
    __out   PULONG                    BytesNeeded
    );


NDIS_STATUS
MpSetInformation(
    __in    NDIS_HANDLE               MiniportAdapterContext,
    __in    NDIS_OID                  Oid,
    __in    PVOID                     InformationBuffer,
    __in    ULONG                     InformationBufferLength,
    __out   PULONG                    BytesRead,
    __out   PULONG                    BytesNeeded
    );

NDIS_STATUS
MpQuerySetInformation(
    __in    PADAPTER                  pAdapter,
    __in    NDIS_OID                  Oid,
    __in    PVOID                     InformationBuffer,
    __in    ULONG                     InputBufferLength,
    __in    ULONG                     OutputBufferLength,
    __in    ULONG                     MethodId,
    __out   PULONG                    BytesWritten,
    __out   PULONG                    BytesRead,
    __out   PULONG                    BytesNeeded
    );

#endif  // _MP_OIDS_H_

