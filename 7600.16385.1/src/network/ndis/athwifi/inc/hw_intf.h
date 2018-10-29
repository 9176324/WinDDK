/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    hw_intf.h

Abstract:
    Contains interfaces into the HW layer
    
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
typedef struct _HW_MAC_CONTEXT  HW_MAC_CONTEXT, *PHW_MAC_CONTEXT;


// First call into the HW
NDIS_STATUS
Hw11Allocate(
    __in  NDIS_HANDLE             MiniportAdapterHandle,
    __deref_out_opt PHW*          Hw,
    __in  PADAPTER                Adapter
    );

VOID
Hw11Free(
    __in  PHW                     Hw
    );

// This is called after the Hw11FindNic & Hw11DiscoverNicResources calls
NDIS_STATUS
Hw11Initialize(
    __in  PHW                     Hw,
    __in  PHVL                    Hvl,
    __out NDIS_ERROR_CODE*        ErrorCode,
    __out PULONG                  ErrorValue
    );

VOID
Hw11Terminate(
    __in  PHW                     Hw
    );

VOID
Hw11Shutdown(
    __in  PHW                     Hw,
    __in  NDIS_SHUTDOWN_ACTION    ShutdownAction
    );

VOID
Hw11Halt(
    __in  PHW                     Hw
    );
    
NDIS_STATUS
Hw11Pause(
    __in  PHW         Hw
    );

VOID
Hw11Restart(
    __in  PHW         Hw
    );

VOID
Hw11DevicePnPEvent(
    __in  PHW                     Hw,
    __in  PNET_DEVICE_PNP_EVENT   NetDevicePnPEvent
    );

BOOLEAN
Hw11CheckForHang(
    __in  PHW                     Hw
    );

VOID
Hw11NdisResetStep1(
    __in  PHW                     Hw
    );

VOID
Hw11NdisResetStep2(
    __in  PHW                     Hw
    );

NDIS_STATUS
Hw11NdisResetStep3(
    __in  PHW                     Hw,
    __out PBOOLEAN                AddressingReset
    );


NDIS_STATUS
Hw11Dot11Reset(
    __in  PHW_MAC_CONTEXT         MacContext,
    __in  PDOT11_RESET_REQUEST    Dot11ResetRequest    
    );


NDIS_STATUS
Hw11AllocateMACContext(
    __in  PHW                     Hw,
    __deref_out_opt PHW_MAC_CONTEXT* MacContext,
    __in  PVNIC                   VNic,
    __in  NDIS_PORT_NUMBER        PortNumber    
    );

VOID
Hw11FreeMACContext(
    __in  PHW                     Hw,
    __in  PHW_MAC_CONTEXT         MacContext
    );

/* 
 * Called to inform the HW that this MAC context should be used (is active).
 * 
 */
NDIS_STATUS
Hw11EnableMACContext(
    __in  PHW                     Hw,
    __in  PHW_MAC_CONTEXT         MacContext
    );
    
/* 
 * Called to inform the HW that this MAC context should not be used (is NOT active) 
 * If the HW is using this context, it would block the call
 */
NDIS_STATUS
Hw11DisableMACContext(
    __in  PHW                     Hw,
    __in  PHW_MAC_CONTEXT         MacContext
    );

NDIS_STATUS
Hw11PauseMACContext(
    __in  PHW_MAC_CONTEXT         MacContext
    );

VOID
Hw11RestartMACContext(
    __in  PHW_MAC_CONTEXT         MacContext
    );

NDIS_STATUS
Hw11InitializeSendEngine(
    __in  PHW                     Hw,
    __out NDIS_ERROR_CODE*        ErrorCode,
    __out PULONG                  ErrorValue
    );

VOID
Hw11TerminateSendEngine(
    __in  PHW                     Hw
    );


NDIS_STATUS
Hw11InitializeReceiveEngine(
    __in  PHW                     Hw,
    __out NDIS_ERROR_CODE*        ErrorCode,
    __out PULONG                  ErrorValue
    );

VOID
Hw11TerminateReceiveEngine(
    __in  PHW                     Hw
    );

// 2nd call into the HW
VOID
Hw11ReadRegistryConfiguration(
    __in  PHW                     Hw,
    __in  NDIS_HANDLE             ConfigurationHandle
    );


// 3rd call into the HW
/**
 * This looks through the PCI configuration space and tries to find the hardware
 * that this driver should control
 */ 
NDIS_STATUS
Hw11FindNic(
    __in  PHW                     Hw,
    __out NDIS_ERROR_CODE*        ErrorCode,
    __out PULONG                  ErrorValue
    );

// 4th call into the HW
NDIS_STATUS
Hw11DiscoverNicResources(
    __in  PHW                     Hw,
    __in  PNDIS_RESOURCE_LIST     ResList,
    __out NDIS_ERROR_CODE*        ErrorCode,
    __out PULONG                  ErrorValue
    );

// 5th call into the HW
NDIS_STATUS
Hw11ReadNicInformation(
    __in  PHW                     Hw
    );

NDIS_STATUS
Hw11Start(
    __in  PHW                     Hw,
    __out NDIS_ERROR_CODE*        ErrorCode,
    __out PULONG                  ErrorValue
    );

// First disable interrupts and then deregister interrupts
VOID
Hw11Stop(
    __in  PHW                     Hw,
    __in  NDIS_HALT_ACTION        HaltAction
    );


NDIS_STATUS
Hw11SelfTest(
    __in  PHW                     Hw
    );

// Maps to interrupts for PCI
VOID
Hw11DisableHardwareNotifications(
    __in  PHW                     Hw
    );

// Maps to interrupts for PCI
VOID
Hw11EnableHardwareNotifications(
    __in  PHW                     Hw
    );


BOOLEAN 
Hw11WEP104Implemented(
    __in  PHW                     Hw
    );

BOOLEAN 
Hw11WEP40Implemented(
    __in  PHW                     Hw
    );

BOOLEAN 
Hw11TKIPImplemented(
    __in  PHW                     Hw
    );

BOOLEAN 
Hw11CCMPImplemented(
    __in  PHW                     Hw
    );

ULONG
Hw11DefaultKeyTableSize(
    __in  PHW                     Hw
    );

ULONG
Hw11KeyMappingKeyTableSize(
    __in  PHW                     Hw
    );

ULONG
Hw11PerStaKeyTableSize(
    __in  PHW                     Hw
    );

NDIS_STATUS
Hw11Fill80211Attributes(
    __in  PHW                     Hw,
    __out PNDIS_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES attr
    );

VOID
Hw11Cleanup80211Attributes(
    __in  PHW                     Hw,
    __in  PNDIS_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES attr
    );

PDOT11_MAC_ADDRESS
Hw11QueryHardwareAddress(
    __in  PHW                     Hw
    );

PDOT11_MAC_ADDRESS
Hw11QueryCurrentAddress(
    __in  PHW                     Hw
    );


PDOT11_MAC_ADDRESS
Hw11QueryMACAddress(
    __in  PHW_MAC_CONTEXT         HwMac
    );

VOID
Hw11QueryPnPCapabilities(
    __in  PHW                     Hw,
    __out PNDIS_PNP_CAPABILITIES  NdisPnPCapabilities
    );

VOID
Hw11QueryPMCapabilities(
    __in  PHW                     Hw,
    __out PNDIS_PM_CAPABILITIES   NdisPmCapabilities
    );

PDOT11_MAC_ADDRESS
Hw11QueryCurrentBSSID(
    __in  PHW_MAC_CONTEXT         HwMac
    );

    
ULONG
Hw11QueryBeaconPeriod(
    __in  PHW_MAC_CONTEXT         HwMac
    );

DOT11_PHY_TYPE
Hw11QueryCurrentPhyType(
    __in  PHW_MAC_CONTEXT         HwMac
    );

BOOLEAN
Hw11QueryShortSlotTimeOptionImplemented(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy
    );

BOOLEAN
Hw11QueryShortSlotTimeOptionEnabled(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy
    );

BOOLEAN
Hw11QueryDsssOfdmOptionImplemented(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy
    );

BOOLEAN
Hw11QueryDsssOfdmOptionEnabled(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy
    );

BOOLEAN
Hw11QueryShortPreambleOptionImplemented(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy
    );

BOOLEAN
Hw11QueryPbccOptionImplemented(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy
    );

BOOLEAN
Hw11QueryChannelAgilityPresent(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy
    );


BOOLEAN
Hw11QueryChannelAgilityEnabled(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy
    );

BOOLEAN
Hw11QueryCFPollable(
    __in  PHW                     Hw
    );

VOID
Hw11QueryBasicRateSet(
    __in  PHW_MAC_CONTEXT         HwMac,
    __out PDOT11_RATE_SET         Dot11RateSet,
    __in  BOOLEAN                 SelectedPhy
    );

VOID
Hw11QueryOperationalRateSet(
    __in  PHW_MAC_CONTEXT         HwMac,
    __out PDOT11_RATE_SET         Dot11RateSet,
    __in  BOOLEAN                 SelectedPhy
    );

NDIS_STATUS
Hw11QuerySupportedPHYTypes(
    __in  PHW                     Hw,
    __in  ULONG                   NumMaxEntries,
    __out PDOT11_SUPPORTED_PHY_TYPES Dot11SupportedPhyTypes
    );

NDIS_STATUS
Hw11QuerySupportedChannels(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  ULONG                   PhyId,
    __out PULONG                  ChannelCount,
    __out_opt PULONG                  ChannelList
    );

ULONG
Hw11QueryCurrentChannel(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy
    );

ULONG
Hw11QueryATIMWindow(
    __in  PHW_MAC_CONTEXT         HwMac
    );

NDIS_STATUS
Hw11SetATIMWindow(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  ULONG                   Value,
    __in  BOOLEAN                 fProgramHardware
    );

VOID
Hw11SetCipher(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 IsUnicast,
    __in  DOT11_CIPHER_ALGORITHM  AlgoId,
    __in  BOOLEAN                 fProgramHardware
    );

UCHAR
Hw11SelectTXDataRate(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  PDOT11_RATE_SET         PeerRateSet,
    __in  ULONG                   LinkQuality,
    __in  BOOLEAN                 fProgramHardware
    );

VOID
Hw11DeleteNonPersistentKey(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 fProgramHardware
    );

VOID
Hw11DeleteNonPersistentMappingKey(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in_bcount(DOT11_ADDRESS_SIZE)  DOT11_MAC_ADDRESS       MacAddr,
    __in  BOOLEAN                 fProgramHardware
    );

ULONG
Hw11QueryOperatingPhyId(
    __in  PHW_MAC_CONTEXT         HwMac
    );

BOOLEAN
Hw11IsConnected(
    __in  PHW_MAC_CONTEXT         HwMac
    );

NDIS_STATUS
Hw11SetOperatingPhyId(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  ULONG                   PhyId,
    __in  BOOLEAN                 fProgramHardware
    );

USHORT
Hw11QueryRSNCapabilityField(
    __in  PHW                     Hw
    );

/** Control */

DOT11_PHY_TYPE
Hw11DeterminePHYType(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  DOT11_CAPABILITY        Capability,
    __in  UCHAR                   Channel
    );


NDIS_STATUS
Hw11ValidateOperationalRates(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  ULONG                   PhyId,
    __in_bcount(RateSetLength)  PUCHAR                  DataRateSet,
    __in  ULONG                   RateSetLength
    );

NDIS_STATUS
Hw11SetPowerMgmtMode(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  PDOT11_POWER_MGMT_MODE  PMMode,
    __in  BOOLEAN                 fProgramHardware
    );

VOID
Hw11SetCTSToSelfOption(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 Enable,
    __in  BOOLEAN                 fProgramHardware
    );

BOOLEAN
Hw11IsKeyMappingKeyAvailable(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in_bcount(DOT11_ADDRESS_SIZE)  DOT11_MAC_ADDRESS       MacAddr
    );

VOID
Hw11StopBSS(
    __in  PHW_MAC_CONTEXT         HwMac
    );

NDIS_STATUS
Hw11StartBSS(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  PMP_BSS_DESCRIPTION     BSSDescription
    );

NDIS_STATUS
Hw11JoinBSS(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  PMP_BSS_DESCRIPTION     BSSDescription,
    __in  ULONG                   JoinFailureTimeout,
    __in  VNIC11_GENERIC_CALLBACK_FUNC    CompletionHandler,
    __in  PVOID                   JoinContext
    );

VOID
Hw11NotifyEventConnectionState(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 Connected,
    __in  ULONG                   StatusType,
    __in  PVOID                   StatusBuffer,
    __in  ULONG                   StatusBufferSize,
    __in  BOOLEAN                 fProgramHardware
    );

// Scan happen on the HW & not the MAC_CONTEXT
NDIS_STATUS
Hw11StartScan(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  PMP_SCAN_REQUEST        ScanRequest,
    __in  VNIC11_GENERIC_CALLBACK_FUNC    CompletionHandler,
    __in  PVOID                   ScanContext
    );

// Cancel scan happens on the HW & not the MAC context
VOID
Hw11CancelScan(
    __in  PHW_MAC_CONTEXT         HwMac
    );

/** Send */

BOOLEAN
Hw11CanTransmit(
    __in  PHW_MAC_CONTEXT         HwMac
    );


VOID
Hw11SendPackets(
    __in PHW_MAC_CONTEXT         HwMac,
    __in PMP_TX_MSDU             PacketList,
    __in ULONG                   SendFlags
    );


/** Receive */

UCHAR
Hw11GetFragmentChannel(
    __in  PMP_RX_MPDU             Fragment
    );

PVOID
Hw11GetFragmentDataStart(
    __in  PMP_RX_MPDU             Fragment
    );

ULONG
Hw11GetFragmentDataLength(
    __in  PMP_RX_MPDU             Fragment
    );

VOID 
Hw11ReturnPackets(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  PMP_RX_MSDU             PacketList,
    __in  ULONG                   ReturnFlags
    );

NDIS_STATUS
Hw11QueryDot11Statistics(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  PDOT11_STATISTICS       Dot11Stats,
    __in  ULONG                   InformationBufferLength,
    __out PULONG                  BytesWritten,
    __out PULONG                  BytesNeeded
    );

NDIS_STATUS
Hw11SetPacketFilter(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  ULONG                   PacketFilter,
    __in  BOOLEAN                 fProgramHardware
    );

BOOLEAN
Hw11QueryNicPowerState(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy
    );

NDIS_STATUS
Hw11SetChannel(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  ULONG                   ChannelOrFrequency,
    __in  ULONG                   PhyId,
    __in  BOOLEAN                 SwitchPhy,
    __in  VNIC11_GENERIC_CALLBACK_FUNC    CompletionHandler,
    __in  PVOID                   ChannelSwitchContext
    );

NDIS_STATUS
Hw11SetDefaultKey(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in_bcount(DOT11_ADDRESS_SIZE)  DOT11_MAC_ADDRESS       MacAddr,
    __in  ULONG                   KeyID,
    __in  BOOLEAN                 Persistent,
    __in  DOT11_CIPHER_ALGORITHM  AlgoId,
    __in  ULONG                   KeyLength,
    __in_bcount(KeyLength)  PUCHAR                  KeyValue,
    __in  BOOLEAN                 fProgramHardware
    );

NDIS_STATUS
Hw11SetKeyMappingKey(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in_bcount(DOT11_ADDRESS_SIZE)  DOT11_MAC_ADDRESS       MacAddr,
    __in  ULONG                   KeyID,
    __in  BOOLEAN                 Persistent,
    __in  DOT11_CIPHER_ALGORITHM  AlgoId,
    __in  ULONG                   KeyLength,
    __in_bcount(KeyLength)  PUCHAR                  KeyValue,
    __in  BOOLEAN                 fProgramHardware
    );

NDIS_STATUS
Hw11SetOperationMode(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  PDOT11_CURRENT_OPERATION_MODE Dot11CurrentOperationMode,
    __in  BOOLEAN                 fProgramHardware
    );

NDIS_STATUS
Hw11SetCurrentBSSType(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  DOT11_BSS_TYPE          Type,
    __in  BOOLEAN                 fProgramHardware
    );

NDIS_STATUS
Hw11SetDesiredPhyIdList(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  PULONG                  PhyIDList,
    __in  ULONG                   PhyIDCount,
    __in  BOOLEAN                 fProgramHardware
    );

NDIS_STATUS
Hw11SetAuthentication(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  DOT11_AUTH_ALGORITHM    AlgoId,
    __in  BOOLEAN                 fProgramHardware
    );

NDIS_STATUS
Hw11QueryRecvSensitivityList(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  ULONG                   MaxEntries,
    __inout PDOT11_RECV_SENSITIVITY_LIST Dot11RecvSensitivityList
    );


ULONG
Hw11QueryDefaultKeyId(
    __in  PHW_MAC_CONTEXT         HwMac
    );

ULONG
Hw11QuerySelectedPhyId(
    __in  PHW_MAC_CONTEXT         HwMac
    );

BOOLEAN
Hw11QueryHardwarePhyState(
    __in  PHW_MAC_CONTEXT         HwMac
    );

NDIS_STATUS
Hw11SetDefaultKeyId(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  ULONG                   KeyId,
    __in  BOOLEAN                 fProgramHardware
    );

NDIS_STATUS
Hw11SetSelectedPhyId(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  ULONG                   PhyId,
    __in  BOOLEAN                 fProgramHardware
    );

NDIS_STATUS
Hw11SetSafeModeOption(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SafeModeEnabled,
    __in  BOOLEAN                 fProgramHardware
    );

NDIS_STATUS
Hw11QueryInterruptModerationSettings(
    __in  PHW                     Hw,
    __out PNDIS_INTERRUPT_MODERATION_PARAMETERS   IntModParams
    );

NDIS_STATUS
Hw11QueryLinkParameters(
    __in  PHW_MAC_CONTEXT         HwMac,
    __out PNDIS_LINK_PARAMETERS   LinkParams
    );

NDIS_STATUS
Hw11QueryDataRateMappingTable(
    __in  PHW_MAC_CONTEXT         HwMac,
    __out PDOT11_DATA_RATE_MAPPING_TABLE  DataRateMappingTable,
    __in  ULONG                   TotalLength
    );

NDIS_STATUS
Hw11QuerySupportedPowerLevels(
    __in  PHW_MAC_CONTEXT         HwMac,
    __out PDOT11_SUPPORTED_POWER_LEVELS   Dot11SupportedPowerLevels
    );

NDIS_STATUS
Hw11QuerySupportedRXAntenna(
    __in  PHW                     Hw,
    __in  ULONG                   MaxEntries,
    __out PDOT11_SUPPORTED_ANTENNA_LIST   Dot11SupportedAntennaList    
    );

NDIS_STATUS
Hw11QuerySupportedTXAntenna(
    __in  PHW                     Hw,
    __in  ULONG                   MaxEntries,
    __out PDOT11_SUPPORTED_ANTENNA_LIST   Dot11SupportedAntennaList
    );

NDIS_STATUS
Hw11QueryDiversitySelectionRX(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy,
    __in  ULONG                   MaxEntries,
    __out PDOT11_DIVERSITY_SELECTION_RX_LIST Dot11DiversitySelectionRXList
    );

NDIS_STATUS
Hw11QueryRegDomainsSupportValue(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy,
    __in  ULONG                   MaxEntries,
    __out PDOT11_REG_DOMAINS_SUPPORT_VALUE    Dot11RegDomainsSupportValue
    );

NDIS_STATUS
Hw11SetInterruptModerationSettings(
    __in  PHW                     Hw,
    __in  PNDIS_INTERRUPT_MODERATION_PARAMETERS   IntModParams
    );

NDIS_STATUS
Hw11SetLinkParameters(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  PNDIS_LINK_PARAMETERS   LinkParams,
    __in  BOOLEAN                 fProgramHardware
    );

NDIS_STATUS
Hw11SetMulticastList(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  PVOID                   AddressBuffer,
    __in  ULONG                   AddressBufferLength,
    __in  BOOLEAN                 fProgramHardware
    );

NDIS_STATUS
Hw11SetOperationalRateSet(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  PDOT11_RATE_SET         Dot11RateSet,
    __in  BOOLEAN                 SelectedPhy,
    __in  BOOLEAN                 fProgramHardware 
    );

ULONG
Hw11QueryLookahead(
    __in  PHW_MAC_CONTEXT         HwMac
    );

ULONG
Hw11QueryHardwareStatus(
    __in  PHW                     Hw
    );

ULONG
Hw11QueryReceiveBufferSpace(
    __in  PHW                     Hw
    );

ULONG
Hw11QueryTransmitBufferSpace(
    __in  PHW                     Hw
    );

NDIS_STATUS
Hw11QueryVendorDescription(
    __in  PHW                     Hw,
    __out PVOID                   InformationBuffer,
    __in  ULONG                   InformationBufferLength,
    __out PULONG                  BytesWritten,
    __out PULONG                  BytesNeeded
    );

ULONG
Hw11QueryVendorId(
    __in  PHW                     Hw
     );

NDIS_STATUS
Hw11QueryCurrentOptionalCapability(
    __in  PHW                     Hw,
    __in PDOT11_CURRENT_OPTIONAL_CAPABILITY   Dot11CurrentOptionalCapability    
    );

ULONG
Hw11QueryMaxMPDULength(
    __in  PHW_MAC_CONTEXT         HwMac
    );

NDIS_STATUS
Hw11QueryMulticastList(
    __in  PHW_MAC_CONTEXT         HwMac,
    __out PVOID                   AddressBuffer,
    __in  ULONG                   AddressBufferLength,
    __out PULONG                  BytesWritten,
    __out PULONG                  BytesNeeded
    );

NDIS_STATUS
Hw11QueryOperationModeCapability(
    __in  PHW                     Hw,
    __out PDOT11_OPERATION_MODE_CAPABILITY    Dot11OpModeCapability
    );

NDIS_STATUS
Hw11QueryOptionalCapability(
    __in  PHW                     Hw,
    __out PDOT11_OPTIONAL_CAPABILITY  Dot11OptionalCapability
    );

ULONG
Hw11QueryRFUsage(
    __in  PHW                     Hw
    );

NDIS_STATUS
Hw11QuerySupportedDataRatesValue(
    __in  PHW_MAC_CONTEXT         HwMac,
    __out PDOT11_SUPPORTED_DATA_RATES_VALUE_V2    Dot11SupportedDataRatesValue,
    __in  BOOLEAN                 SelectedPhy 
    );

ULONG
Hw11QueryCCAModeSupported(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy
    );

NDIS_STATUS
Hw11QueryCountryString(
    __in  PHW_MAC_CONTEXT         HwMac,
    __out PDOT11_COUNTRY_OR_REGION_STRING Dot11CountryString
    );

ULONG
Hw11QueryCurrentCCAMode(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy
    );

ULONG
Hw11QueryCurrentFrequency(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy
    );

ULONG
Hw11QueryCurrentRegDomain(
    __in  PHW_MAC_CONTEXT         HwMac
    );

ULONG
Hw11QueryCurrentTXPowerLevel(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy
    );

DOT11_DIVERSITY_SUPPORT
Hw11QueryDiversitySupport(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy
    );

ULONG
Hw11QueryEDThreshold(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy
    );

BOOLEAN
Hw11QueryErpPbccOptionEnabled(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy
    );

BOOLEAN
Hw11QueryErpPbccOptionImplemented(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy
    );

ULONG
Hw11QueryFragmentationThreshold(
    __in  PHW_MAC_CONTEXT         HwMac
    );

ULONG
Hw11QueryFrequencyBandsSupported(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy
    );

ULONG
Hw11QueryLongRetryLimit(
    __in  PHW_MAC_CONTEXT         HwMac
    );

ULONG
Hw11QueryMaxReceiveLifetime(
    __in  PHW                     Hw
    );

ULONG
Hw11QueryMaxTransmitMSDULifetime(
    __in  PHW                     Hw
    );

BOOLEAN
Hw11QueryMultiDomainCapabilityEnabled(
    __in  PHW_MAC_CONTEXT         HwMac
    );

BOOLEAN
Hw11QueryMultiDomainCapabilityImplemented(
    __in  PHW_MAC_CONTEXT         HwMac
    );

ULONG
Hw11QueryRTSThreshold(
    __in  PHW_MAC_CONTEXT         HwMac
    );

ULONG
Hw11QueryShortRetryLimit(
    __in  PHW_MAC_CONTEXT         HwMac
    );

DOT11_TEMP_TYPE
Hw11QueryTempType(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy
    );

ULONG
Hw11QueryPacketFilter(
    __in  PHW_MAC_CONTEXT         HwMac
    );

DOT11_CIPHER_ALGORITHM
Hw11QueryUnicastCipher(
    __in  PHW_MAC_CONTEXT         HwMac
    );

DOT11_CIPHER_ALGORITHM
Hw11QueryMulticastCipher(
    __in  PHW_MAC_CONTEXT         HwMac
    );

DOT11_AUTH_ALGORITHM
Hw11QueryAuthentication(
    __in  PHW_MAC_CONTEXT         HwMac
    );

UCHAR
Hw11QueryDefaultKeyMask(
    __in  PHW_MAC_CONTEXT         HwMac
    );
    
NDIS_STATUS
Hw11SetLookahead(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  ULONG                   Lookahead,
    __in  BOOLEAN                 fProgramHardware
    );

NDIS_STATUS
Hw11SetNicPowerState(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 PowerState,
    __in  BOOLEAN                 SelectedPhy,
    __in  BOOLEAN                 fProgramHardware
    );

NDIS_STATUS
Hw11SetBeaconPeriod(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  ULONG                   BeaconPeriod,
    __in  BOOLEAN                 fProgramHardware
    );

NDIS_STATUS
Hw11SetFragmentationThreshold(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  ULONG                   FragmentationThreshold,
    __in  BOOLEAN                 fProgramHardware
    );

NDIS_STATUS
Hw11SetMultiDomainCapabilityEnabled(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 MultiDomainCapabilityEnabled,
    __in  BOOLEAN                 fProgramHardware
    );
NDIS_STATUS
Hw11SetRTSThreshold(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  ULONG                   RTSThreshold,
    __in  BOOLEAN                 fProgramHardware
    );

NDIS_STATUS
Hw11CanTransitionPower(
    __in  PHW                     Hw,
    __in  NDIS_DEVICE_POWER_STATE NewDevicePowerState
    );

NDIS_STATUS 
Hw11SetPower(
    __in  PHW                     Hw,
    __in  NDIS_DEVICE_POWER_STATE NewDevicePowerState
    );

NDIS_STATUS
Hw11CtxSStart(
    __in  PHW                     Hw
    );

VOID
Hw11CtxSComplete(
    __in  PHW                     Hw
    );


NDIS_STATUS
Hw11SendNullPkt(
    __in PHW_MAC_CONTEXT          MacContext,
    __in BOOLEAN                  PowerBitSet
    );

NDIS_STATUS
Hw11SetBeaconEnabledFlag(    
    __in  PHW_MAC_CONTEXT         HwMac,
    BOOLEAN                     BeaconEnabled,
    BOOLEAN                     fProgramHardware
    );

NDIS_STATUS 
Hw11SetBeaconIE(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  PVOID                   pBeaconIEBlob,
    __in  ULONG                   uBeaconIEBlobSize,
    __in  BOOLEAN                 fProgramHardware
    );

NDIS_STATUS 
Hw11SetProbeResponseIE(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  PVOID                   pResponseIEBlob,
    __in  ULONG                   uResponseIEBlobSize,
    __in  BOOLEAN                 fProgramHardware
    );

DOT11_BSS_TYPE
Hw11QueryCurrentBSSType(
    __in  PHW_MAC_CONTEXT         HwMac
    );

BOOLEAN
Hw11ArePktsPending(
    __in  PHW                     Hw
    );


