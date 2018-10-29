/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    vnic_intf.h

Abstract:
    Contains interfaces into the VNIC
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    09-04-2007    Created

Notes:

--*/
#pragma once

//
// Forward declaration
//
typedef struct _ADAPTER         ADAPTER, *PADAPTER;
typedef struct _HVL             HVL, *PHVL;
typedef struct _HW              HW, *PHW;
typedef struct _MP_PORT         MP_PORT, *PMP_PORT;
typedef struct _VNIC            VNIC, *PVNIC;

/**
 * PNP functionality
 */
NDIS_STATUS
VNic11Allocate(
    __in  NDIS_HANDLE             MiniportAdapterHandle,
    __deref_out_opt PVNIC*        ppVNic,
    __in  PMP_PORT                Port
    );

VOID
VNic11Free(
    __in  PVNIC                   VNic
    );

NDIS_STATUS
VNic11Initialize(
    __in  PVNIC                   VNic,
    __in  PVOID                   Hvl,
    __in  PVOID                   Hw,
    __in  MP_PORT_TYPE            PortType,
    __in  NDIS_PORT_NUMBER        PortNumber
    );

VOID
VNic11Terminate(
    __in  PVNIC                   VNic
    );

NDIS_STATUS
VNic11Pause(
    __in  PVNIC                   VNic
    );

NDIS_STATUS
VNic11Restart(
    __in  PVNIC                   VNic
    );

NDIS_STATUS
VNic11Dot11Reset(
    __in  PVNIC                   VNic,
    __in  PDOT11_RESET_REQUEST    Dot11ResetRequest
    );

NDIS_STATUS
VNic11Dot11ResetComplete(
    __in  PVNIC                   pVNic
    );
    
/**
 * Capability queries
 */
BOOLEAN 
VNic11WEP104Implemented(
    __in  PVNIC                   VNic
    );

BOOLEAN 
VNic11WEP40Implemented(
    __in  PVNIC                   VNic
    );

BOOLEAN 
VNic11TKIPImplemented(
    __in  PVNIC                   VNic
    );

BOOLEAN 
VNic11CCMPImplemented(
    __in  PVNIC                   VNic
    );

ULONG
VNic11DefaultKeyTableSize(
    __in  PVNIC                   VNic
    );

ULONG
VNic11KeyMappingKeyTableSize(
    __in  PVNIC                   VNic
    );

ULONG
VNic11PerStaKeyTableSize(
    __in  PVNIC                   VNic
    );

USHORT
VNic11QueryRSNCapabilityField(
    __in  PVNIC                   VNic
    );

ULONG
VNic11QueryReceiveBufferSpace(
    __in  PVNIC                   VNic
    );

ULONG
VNic11QueryTransmitBufferSpace(
    __in  PVNIC                   VNic
    );

ULONG
VNic11QueryMaxMPDULength(
    __in  PVNIC                   VNic
    );

NDIS_STATUS
VNic11QueryOperationModeCapability(
    __in  PVNIC                   VNic,
    __out PDOT11_OPERATION_MODE_CAPABILITY    Dot11OpModeCapability
    );

NDIS_STATUS
VNic11QueryOptionalCapability(
    __in  PVNIC                   VNic,
    __out PDOT11_OPTIONAL_CAPABILITY  Dot11OptionalCapability
    );


/*
 * Generic OIDs
 */
PDOT11_MAC_ADDRESS
VNic11QueryMACAddress(
    __in  PVNIC                   VNic
    );

PDOT11_MAC_ADDRESS
VNic11QueryHardwareAddress(
    __in  PVNIC                   VNic
    );

ULONG
VNic11QueryHardwareStatus(
    __in  PVNIC                   VNic
    );

NDIS_STATUS
VNic11SetPacketFilter(
    __in  PVNIC                   VNic,
    __in  ULONG                   PacketFilter
    );

NDIS_STATUS
VNic11QueryMulticastList(
    __in  PVNIC                   VNic,
    __out PVOID                   AddressBuffer,
    __in  ULONG                   AddressBufferLength,
    __out PULONG                  BytesWritten,
    __out PULONG                  BytesNeeded
    );

NDIS_STATUS
VNic11SetMulticastList(
    __in  PVNIC                   VNic,
    __in  PVOID                   AddressBuffer,
    __in  ULONG                   AddressBufferLength
    );

ULONG
VNic11QueryLookahead(
    __in  PVNIC                   VNic
    );

NDIS_STATUS
VNic11SetLookahead(
    __in  PVNIC                   VNic,
    __in  ULONG                   Lookahead
    );

NDIS_STATUS
VNic11QueryInterruptModerationSettings(
    __in  PVNIC                   VNic,
    __out PNDIS_INTERRUPT_MODERATION_PARAMETERS   IntModParams
    );

NDIS_STATUS
VNic11SetInterruptModerationSettings(
    __in  PVNIC                   VNic,
    __in  PNDIS_INTERRUPT_MODERATION_PARAMETERS   IntModParams
    );

NDIS_STATUS
VNic11QueryLinkParameters(
    __in  PVNIC                   VNic,
    __out PNDIS_LINK_PARAMETERS   LinkParams
    );

NDIS_STATUS
VNic11SetLinkParameters(
    __in  PVNIC                   VNic,
    __in  PNDIS_LINK_PARAMETERS   LinkParams
    );

NDIS_STATUS
VNic11QueryVendorDescription(
    __in  PVNIC                   VNic,
    __out PVOID                   InformationBuffer,
    __in  ULONG                   InformationBufferLength,
    __out PULONG                  BytesWritten,
    __out PULONG                  BytesNeeded
    );

ULONG
VNic11QueryVendorId(
    __in  PVNIC                   VNic
     );

/** 
 * Dot11 OIDs
 */

PDOT11_MAC_ADDRESS
VNic11QueryCurrentBSSID(
    __in  PVNIC                   VNic
    );


NDIS_STATUS
VNic11SetOperationMode(
    __in  PVNIC                   pVNic,
    __in  PDOT11_CURRENT_OPERATION_MODE Dot11CurrentOperationMode
    );

NDIS_STATUS
VNic11SetCurrentBSSType(
    __in  PVNIC                   VNic,
    __in  DOT11_BSS_TYPE          Type
    );
    
ULONG
VNic11QueryBeaconPeriod(
    __in  PVNIC                   VNic
    );

NDIS_STATUS
VNic11SetBeaconPeriod(
    __in  PVNIC                   VNic,
    __in  ULONG                   BeaconPeriod
    );

BOOLEAN
VNic11QueryCFPollable(
    __in  PVNIC                   VNic
    );

NDIS_STATUS
VNic11QueryCurrentOptionalCapability(
    __in  PVNIC                   VNic,
    __in PDOT11_CURRENT_OPTIONAL_CAPABILITY   Dot11CurrentOptionalCapability    
    );

NDIS_STATUS
VNic11QuerySupportedPowerLevels(
    __in  PVNIC                   VNic,
    __out PDOT11_SUPPORTED_POWER_LEVELS   Dot11SupportedPowerLevels
    );

ULONG
VNic11QueryCurrentTXPowerLevel(
    __in  PVNIC                   VNic,
    __in  BOOLEAN                 SelectedPhy
    );

ULONG
VNic11QueryATIMWindow(
    __in  PVNIC                   VNic
    );

NDIS_STATUS
VNic11SetATIMWindow(
    __in  PVNIC                   VNic,
    __in  ULONG                   Value
    );

NDIS_STATUS
VNic11SetPowerMgmtMode(
    __in  PVNIC                   VNic,
    __in  PDOT11_POWER_MGMT_MODE  PMMode
    );

BOOLEAN
VNic11QueryNicPowerState(
    __in  PVNIC                   VNic,
    __in  BOOLEAN                 SelectedPhy
    );

NDIS_STATUS
VNic11SetNicPowerState(
    __in  PVNIC                   VNic,
    __in  BOOLEAN                 PowerState,
    __in  BOOLEAN                 SelectedPhy
    );

BOOLEAN
VNic11QueryHardwarePhyState(
    __in  PVNIC                   VNic
    );

VOID
VNic11SetCTSToSelfOption(
    __in  PVNIC                   VNic,
    __in  BOOLEAN                 Enable
    );

ULONG
VNic11QueryRTSThreshold(
    __in  PVNIC                   VNic
    );

NDIS_STATUS
VNic11SetRTSThreshold(
    __in  PVNIC                   VNic,
    __in  ULONG                   BeaconPeriod
    );

ULONG
VNic11QueryFragmentationThreshold(
    __in  PVNIC                   VNic
    );

NDIS_STATUS
VNic11SetFragmentationThreshold(
    __in  PVNIC                   VNic,
    __in  ULONG                   BeaconPeriod
    );

ULONG
VNic11QueryLongRetryLimit(
    __in  PVNIC                   VNic
    );

ULONG
VNic11QueryShortRetryLimit(
    __in  PVNIC                   VNic
    );

NDIS_STATUS
VNic11QueryCountryString(
    __in  PVNIC                   VNic,
    __out PDOT11_COUNTRY_OR_REGION_STRING Dot11CountryString
    );

ULONG
VNic11QueryCurrentRegDomain(
    __in  PVNIC                   VNic
    );

BOOLEAN
VNic11QueryMultiDomainCapabilityImplemented(
    __in  PVNIC                   VNic
    );

BOOLEAN
VNic11QueryMultiDomainCapabilityEnabled(
    __in  PVNIC                   VNic
    );

NDIS_STATUS
VNic11SetMultiDomainCapabilityEnabled(
    __in  PVNIC                   VNic,
    __in  BOOLEAN                 MultiDomainCapabilityEnabled
    );

NDIS_STATUS
VNic11SetSafeModeOption(
    __in  PVNIC                   VNic,
    __in  BOOLEAN                 SafeModeEnabled
    );

NDIS_STATUS
VNic11QueryDot11Statistics(
    __in  PVNIC                   VNic,
    __in  PDOT11_STATISTICS       Dot11Stats,
    __in  ULONG                   InformationBufferLength,
    __out PULONG                  BytesWritten,
    __out PULONG                  BytesNeeded
    );

NDIS_STATUS
VNic11QueryRecvSensitivityList(
    __in  PVNIC                   VNic,
    __in  ULONG                   MaxEntries,
    __inout PDOT11_RECV_SENSITIVITY_LIST Dot11RecvSensitivityList
    );

NDIS_STATUS
VNic11QuerySupportedRXAntenna(
    __in  PVNIC                   VNic,
    __in  ULONG                   MaxEntries,
    __out PDOT11_SUPPORTED_ANTENNA_LIST   Dot11SupportedAntennaList    
    );

NDIS_STATUS
VNic11QuerySupportedTXAntenna(
    __in  PVNIC                   VNic,
    __in  ULONG                   MaxEntries,
    __out PDOT11_SUPPORTED_ANTENNA_LIST   Dot11SupportedAntennaList
    );

NDIS_STATUS
VNic11QueryDiversitySelectionRX(
    __in  PVNIC                   VNic,
    __in  BOOLEAN                 SelectedPhy,
    __in  ULONG                   MaxEntries,
    __out PDOT11_DIVERSITY_SELECTION_RX_LIST Dot11DiversitySelectionRXList
    );

NDIS_STATUS
VNic11QueryRegDomainsSupportValue(
    __in  PVNIC                   VNic,
    __in  BOOLEAN                 SelectedPhy,
    __in  ULONG                   MaxEntries,
    __out PDOT11_REG_DOMAINS_SUPPORT_VALUE    Dot11RegDomainsSupportValue
    );

ULONG
VNic11QueryRFUsage(
    __in  PVNIC                   VNic
    );

ULONG
VNic11QueryMaxReceiveLifetime(
    __in  PVNIC                   VNic
    );

ULONG
VNic11QueryMaxTransmitMSDULifetime(
    __in  PVNIC                   VNic
    );

DOT11_TEMP_TYPE
VNic11QueryTempType(
    __in  PVNIC                   VNic,
    __in  BOOLEAN                 SelectedPhy
    );

/*
 * PHY selection related
 */
NDIS_STATUS
VNic11QuerySupportedPHYTypes(
    __in  PVNIC                   VNic,
    __in  ULONG                   NumMaxEntries,
    __out PDOT11_SUPPORTED_PHY_TYPES Dot11SupportedPhyTypes
    );

DOT11_PHY_TYPE
VNic11QueryCurrentPhyType(
    __in  PVNIC                   VNic
    );

DOT11_PHY_TYPE
VNic11DeterminePHYType(
    __in  PVNIC                   VNic,
    __in  DOT11_CAPABILITY        Capability,
    __in  UCHAR                   Channel
    );

NDIS_STATUS
VNic11SetDesiredPhyIdList(
    __in  PVNIC                   VNic,
    __in_ecount(PhyIDCount)  PULONG                  PhyIDList,
    __in  ULONG                   PhyIDCount
    );

NDIS_STATUS
VNic11SetOperatingPhyId(
    __in  PVNIC                   VNic,
    __in  ULONG                   PhyId
    );

ULONG
VNic11QueryOperatingPhyId(
    __in  PVNIC                   VNic
    );

NDIS_STATUS
VNic11SetOperationalRateSet(
    __in  PVNIC                   VNic,
    __in  PDOT11_RATE_SET         Dot11RateSet,
    __in  BOOLEAN                 SelectedPhy 
    );

ULONG
VNic11QuerySelectedPhyId(
    __in  PVNIC                   VNic
    );

NDIS_STATUS
VNic11SetSelectedPhyId(
    __in  PVNIC                   VNic,
    __in  ULONG                   PhyId
    );

/*
 * Rates
 */
VOID
VNic11QueryBasicRateSet(
    __in  PVNIC                   VNic,
    __out PDOT11_RATE_SET         Dot11RateSet,
    __in  BOOLEAN                 SelectedPhy
    );

VOID
VNic11QueryOperationalRateSet(
    __in  PVNIC                   VNic,
    __out PDOT11_RATE_SET         Dot11RateSet,
    __in  BOOLEAN                 SelectedPhy
    );

UCHAR
VNic11SelectTXDataRate(
    __in  PVNIC                   VNic,
    __in  PDOT11_RATE_SET         PeerRateSet,
    __in  ULONG                   LinkQuality
    );

NDIS_STATUS
VNic11QuerySupportedDataRatesValue(
    __in  PVNIC                   VNic,
    __out PDOT11_SUPPORTED_DATA_RATES_VALUE_V2    Dot11SupportedDataRatesValue,
    __in  BOOLEAN                 SelectedPhy 
    );

NDIS_STATUS
VNic11ValidateOperationalRates(
    __in  PVNIC                   VNic,
    __in  ULONG                   PhyId,
    __in_bcount(RateSetLength)  PUCHAR                  DataRateSet,
    __in  ULONG                   RateSetLength
    );

NDIS_STATUS
VNic11QueryDataRateMappingTable(
    __in  PVNIC                   VNic,
    __out PDOT11_DATA_RATE_MAPPING_TABLE  DataRateMappingTable,
    __in  ULONG                   TotalLength
    );

/*
 * Phy specific configuration parameters
 */
BOOLEAN
VNic11QueryShortSlotTimeOptionImplemented(
    __in  PVNIC                   VNic,
    __in  BOOLEAN                 SelectedPhy
    );

BOOLEAN
VNic11QueryShortSlotTimeOptionEnabled(
    __in  PVNIC                   VNic,
    __in  BOOLEAN                 SelectedPhy
    );

BOOLEAN
VNic11QueryDsssOfdmOptionImplemented(
    __in  PVNIC                   VNic,
    __in  BOOLEAN                 SelectedPhy
    );

BOOLEAN
VNic11QueryDsssOfdmOptionEnabled(
    __in  PVNIC                   VNic,
    __in  BOOLEAN                 SelectedPhy
    );

BOOLEAN
VNic11QueryShortPreambleOptionImplemented(
    __in  PVNIC                   VNic,
    __in  BOOLEAN                 SelectedPhy
    );

BOOLEAN
VNic11QueryPbccOptionImplemented(
    __in  PVNIC                   VNic,
    __in  BOOLEAN                 SelectedPhy
    );

BOOLEAN
VNic11QueryChannelAgilityPresent(
    __in  PVNIC                   VNic,
    __in  BOOLEAN                 SelectedPhy
    );

BOOLEAN
VNic11QueryChannelAgilityEnabled(
    __in  PVNIC                   VNic,
    __in  BOOLEAN                 SelectedPhy
    );

ULONG
VNic11QueryCCAModeSupported(
    __in  PVNIC                   VNic,
    __in  BOOLEAN                 SelectedPhy
    );

ULONG
VNic11QueryCurrentCCAMode(
    __in  PVNIC                   VNic,
    __in  BOOLEAN                 SelectedPhy
    );

DOT11_DIVERSITY_SUPPORT
VNic11QueryDiversitySupport(
    __in  PVNIC                   VNic,
    __in  BOOLEAN                 SelectedPhy
    );

ULONG
VNic11QueryEDThreshold(
    __in  PVNIC                   VNic,
    __in  BOOLEAN                 SelectedPhy
    );

BOOLEAN
VNic11QueryErpPbccOptionEnabled(
    __in  PVNIC                   VNic,
    __in  BOOLEAN                 SelectedPhy
    );

BOOLEAN
VNic11QueryErpPbccOptionImplemented(
    __in  PVNIC                   VNic,
    __in  BOOLEAN                 SelectedPhy
    );

ULONG
VNic11QueryFrequencyBandsSupported(
    __in  PVNIC                   VNic,
    __in  BOOLEAN                 SelectedPhy
    );

NDIS_STATUS
VNic11QuerySupportedChannels(
    __in  PVNIC                   VNic,
    __in  ULONG                   PhyId,
    __out PULONG                  ChannelCount,
    __out_opt PULONG                  ChannelList
    );

/*
 * Current channel/frequency
 */
ULONG
VNic11QueryCurrentChannel(
    __in  PVNIC                   VNic,
    __in  BOOLEAN                 SelectedPhy
    );

NDIS_STATUS
VNic11SetChannel(
    __in  PVNIC                   VNic,
    __in  ULONG                   ChannelOrFrequency,
    __in  ULONG                   PhyId,
    __in  BOOLEAN                 SwitchPhy,
    __in  PORT11_GENERIC_CALLBACK_FUNC CompletionHandler    
    );

/*
 * Auth cipher
 */
NDIS_STATUS
VNic11SetAuthentication(
    __in  PVNIC                   VNic,
    __in  DOT11_AUTH_ALGORITHM    AlgoId
    );

VOID
VNic11SetCipher(
    __in  PVNIC                   VNic,
    __in  BOOLEAN                 IsUnicast,
    __in  DOT11_CIPHER_ALGORITHM  AlgoId
    );

NDIS_STATUS
VNic11SetDefaultKey(
    __in  PVNIC                   VNic,
    __in_bcount(DOT11_ADDRESS_SIZE)  DOT11_MAC_ADDRESS       MacAddr,
    __in  ULONG                   KeyID,
    __in  BOOLEAN                 Persistent,
    __in  DOT11_CIPHER_ALGORITHM  AlgoId,
    __in  ULONG                   KeyLength,
    __in_bcount(KeyLength)  PUCHAR                  KeyValue    
    );

ULONG
VNic11QueryDefaultKeyId(
    __in  PVNIC                   VNic
    );

NDIS_STATUS
VNic11SetDefaultKeyId(
    __in  PVNIC                   VNic,
    __in  ULONG                   KeyId
    );

VOID
VNic11DeleteNonPersistentKey(
    __in  PVNIC                   VNic
    );

NDIS_STATUS
VNic11SetKeyMappingKey(
    __in  PVNIC                   VNic,
    __in_bcount(DOT11_ADDRESS_SIZE)  DOT11_MAC_ADDRESS       MacAddr,
    __in  ULONG                   KeyID,
    __in  BOOLEAN                 Persistent,
    __in  DOT11_CIPHER_ALGORITHM  AlgoId,
    __in  ULONG                   KeyLength,
    __in_bcount(KeyLength)  PUCHAR                  KeyValue    
    );

BOOLEAN
VNic11IsKeyMappingKeyAvailable(
    __in  PVNIC                   VNic,
    __in_bcount(DOT11_ADDRESS_SIZE)  DOT11_MAC_ADDRESS       MacAddr
    );

VOID
VNic11DeleteNonPersistentMappingKey(
    __in  PVNIC                   VNic,
    __in_bcount(DOT11_ADDRESS_SIZE)  DOT11_MAC_ADDRESS       MacAddr
    );


/*
 * Adhoc & Infrastructure
 */
NDIS_STATUS
VNic11JoinBSS(
    __in  PVNIC                   VNic,
    __in  PMP_BSS_DESCRIPTION     BSSDescription,
    __in  ULONG                   JoinFailureTimeout,
    __in  PORT11_GENERIC_CALLBACK_FUNC CompletionHandler
    );

NDIS_STATUS
VNic11StopBSS(
    __in  PVNIC                   pVNic,
    __in  PORT11_GENERIC_CALLBACK_FUNC CompletionHandler
    );

NDIS_STATUS
VNic11StartBSS(
    __in  PVNIC                   pVNic,
    __in  PMP_BSS_DESCRIPTION     BSSDescription,
    __in  PORT11_GENERIC_CALLBACK_FUNC CompletionHandler
    );

NDIS_STATUS
VNic11SetBeaconEnabledFlag(
    __in  PVNIC                   VNic,
    __in  BOOLEAN                 BeaconEnabled
    );



/*
 * Scan
 */

NDIS_STATUS
VNic11StartScan(
    __in  PVNIC                   VNic,
    __in  PMP_SCAN_REQUEST        ScanRequest,
    __in  PORT11_GENERIC_CALLBACK_FUNC CompletionHandler
    );

VOID
VNic11CancelScan(
    __in  PVNIC                   VNic
    );


/*
 * Control Notifications
 */

//
// NDIS_STATUS_DOT11_ASSOCIATION_COMPLETION - Informs the VNIC about association of a peer
// Caleld before the OS is informed
//
// NDIS_STATUS_DOT11_DISASSOCIATION - Informs the VNIC about loss of connectivity.
// Called after the OS has been informed
//
// NDIS_STATUS_DOT11_CONNECTION_START - Called after OS
//
// NDIS_STATUS_DOT11_CONNECTION_COMPLETION - Before OS
//
// On a Reset, the status should be reset
VOID
VNic11NotifyConnectionStatus(
    __in  PVNIC                   VNic,
    __in  BOOLEAN                 Connected,
    __in  ULONG                   StatusType,
    __in_bcount_opt(StatusBufferSize)  PVOID                   StatusBuffer,
    __in  ULONG                   StatusBufferSize
    );

VOID
VNic11IndicateStatus(
    __in  PVNIC                   VNic,
    __in  NDIS_STATUS             StatusCode,
    __in  PVOID                   StatusBuffer,
    __in  ULONG                   StatusBufferSize
    );


/*
 * Send
 */
BOOLEAN
VNic11CanTransmit(
    __in  PVNIC                   VNic
    );


VOID
VNic11SendPackets(
    __in PVNIC                   VNic,
    __in PMP_TX_MSDU             PacketList,
    __in ULONG                   NumPkts,
    __in ULONG                   SendFlags
    );


VOID 
VNic11SendCompletePackets(
    __in  PVNIC                   VNic,
    __in  PMP_TX_MSDU             PacketList,
    __in  ULONG                   NumPkts,
    __in  ULONG                   SendCompleteFlags
    );

/*
 * Receive
 */

VOID 
VNic11ReturnPackets(
    __in  PVNIC                   VNic,
    __in  PMP_RX_MSDU             PacketList,
    __in  ULONG                   ReturnFlags
    );

VOID 
VNic11IndicateReceivePackets(
    __in  PVNIC                   VNic,
    __in  PMP_RX_MSDU             PacketList,
    __in  ULONG                   ReceiveFlags
    );
VOID
VNic11CtxSFromVNic(    
    __in  PVNIC      VNic,
    __in  ULONG      Flags
    );

#define VNIC_FLAG_GRANTED_EX_ACCESS             0x00000001
#define VNIC_FLAG_EX_ACCESS_HVL_TRIGGERED       0x00000002
#define VNIC_FLAG_HVL_ACTIVATED                 0x00000004

VOID
VNic11CtxSToVNic(    
    __in  PVNIC      VNic,
    __in  ULONG      Flags
    );

VOID
VNic11ProgramHw(    
    __in  PVNIC      VNic,
    __in  ULONG      Flags
    );

NDIS_STATUS 
VNic11ReqExAccess(
    PVNIC VNic,
    PORT11_GENERIC_CALLBACK_FUNC CallbkFn,
    PVOID FnCtx,
    BOOLEAN fPnPOperation
    );

NDIS_STATUS 
VNic11ReleaseExAccess(
    PVNIC VNic
    );

BOOLEAN
VNic11IsOkToCtxS(
    PVNIC VNic
    );

typedef struct _VNIC_SIGNATURE
{
    ULONG ulChannel;
    ULONG ulPhyId;
    UCHAR ucDefKeyMask;
} VNIC_SIGNATURE, *PVNIC_SIGNATURE;


VNIC_SIGNATURE
VNic11GetSignature(
    PVNIC pVNic
    );

VNIC_SIGNATURE
VNic11MergeSignatures(
    PVNIC_SIGNATURE pSig1,
    PVNIC_SIGNATURE pSig2
    );  

BOOLEAN
VNic11AreCompatibleSignatures(
    PVNIC_SIGNATURE pSig1,
    PVNIC_SIGNATURE pSig2
    );

typedef enum _NOTIFICATION_TYPE
{
    NotificationOpChannel,
    NotificationOpLinkState,
    NotificationOpRateChange
}NOTIFICATION_TYPE, *PNOTIFICATION_TYPE;

typedef struct _NOTIFICATION_DATA_HEADER
{
    PVNIC pSourceVNic;
    NOTIFICATION_TYPE Type;
    UCHAR Size;
}NOTIFICATION_DATA_HEADER, *PNOTIFICATION_DATA_HEADER;

typedef struct _OP_CHANNEL_NOTIFICATION
{
    NOTIFICATION_DATA_HEADER Header;
    ULONG ulChannel;
}OP_CHANNEL_NOTIFICATION, *POP_CHANNEL_NOTIFICATION;

typedef struct OP_LINK_STATE_NOTIFICATION
{
    NOTIFICATION_DATA_HEADER Header;
    BOOLEAN     MediaConnected;
}OP_LINK_STATE_NOTIFICATION, *POP_LINK_STATE_NOTIFICATION;

typedef struct OP_RATE_CHANGE_NOTIFICATION
{
    NOTIFICATION_DATA_HEADER Header;
    USHORT      OldRate;
    USHORT      NewRate;
    BOOLEAN     LowestRate;
}OP_RATE_CHANGE_NOTIFICATION, *POP_RATE_CHANGE_NOTIFICATION;



VOID
VNic11Notify(
    PVNIC               VNic,
    PVOID                   pvNotif
);

ULONG
VNic11QueryPreferredChannel(
    __in  PVNIC               pVNic,
    __out PBOOLEAN            pPreferredChannel
    );

VOID
VNic11UpdatePortType(
    __in  PVNIC                   pVNic,
    __in  MP_PORT_TYPE            PortType
    );

VOID
VNic11AcquireCtxSBarrier(
    PVNIC pVNic
    );

VOID
VNic11ReleaseCtxSBarrier(
    PVNIC pVNic
    );

NDIS_STATUS 
VNic11SetBeaconIE(
    __in  PVNIC   pVNic,
    __in  PVOID   pBeaconIEBlob,
    __in  ULONG   uBeaconIEBlobSize
    );

NDIS_STATUS 
VNic11SetProbeResponseIE(
    __in  PVNIC   pVNic,
    __in  PVOID   pResponseIEBlob,
    __in  ULONG   uResponseIEBlobSize
    );


