/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    vnic_oids.c

Abstract:
    Implements the OID handling for the VNIC
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    09-04-2007    Created

Notes:

--*/

#include "precomp.h"

#if DOT11_TRACE_ENABLED
#include "vnic_oids.tmh"
#endif


/*
    Functions that are not tied to a specific instantiation of a VNIC. These functions work on the
    physical characterstics of the physical hardware
    */
BOOLEAN 
VNic11WEP104Implemented(
    __in  PVNIC                   pVNic
    )
{
    return Hw11WEP104Implemented(pVNic->pvHw);
}

BOOLEAN 
VNic11WEP40Implemented(
    __in  PVNIC                   pVNic
    )
{
    return Hw11WEP40Implemented(pVNic->pvHw);
}

BOOLEAN 
VNic11TKIPImplemented(
    __in  PVNIC                   pVNic
    )
{
    return Hw11TKIPImplemented(pVNic->pvHw);
}

BOOLEAN 
VNic11CCMPImplemented(
    __in  PVNIC                   pVNic
    )
{
    return Hw11CCMPImplemented(pVNic->pvHw);
}

ULONG
VNic11DefaultKeyTableSize(
    __in  PVNIC                   pVNic
    )
{
    return Hw11DefaultKeyTableSize(pVNic->pvHw);
}

ULONG
VNic11KeyMappingKeyTableSize(
    __in  PVNIC                   pVNic
    )
{
    return Hw11KeyMappingKeyTableSize(pVNic->pvHw);
}

ULONG
VNic11PerStaKeyTableSize(
    __in  PVNIC                   pVNic
    )
{
    return Hw11PerStaKeyTableSize(pVNic->pvHw);
}

NDIS_STATUS
VNic11SetInterruptModerationSettings(
    __in  PVNIC                   pVNic,
    __in  PNDIS_INTERRUPT_MODERATION_PARAMETERS   IntModParams
    )
{
    return Hw11SetInterruptModerationSettings(pVNic->pvHw, IntModParams);
}

BOOLEAN
VNic11QueryCFPollable(
    __in  PVNIC                   pVNic
    )
{
    return Hw11QueryCFPollable(pVNic->pvHw);
}

NDIS_STATUS
VNic11QuerySupportedPHYTypes(
    __in  PVNIC                   pVNic,
    __in  ULONG                   NumMaxEntries,
    __out PDOT11_SUPPORTED_PHY_TYPES Dot11SupportedPhyTypes
    )
{
    return Hw11QuerySupportedPHYTypes(pVNic->pvHw, NumMaxEntries, Dot11SupportedPhyTypes);
}

USHORT
VNic11QueryRSNCapabilityField(
    __in  PVNIC                   pVNic
    )
{
    return Hw11QueryRSNCapabilityField(pVNic->pvHw);
}

BOOLEAN
VNic11QueryHardwarePhyState(
    __in  PVNIC                   pVNic
    )
{
    return Hw11QueryHardwarePhyState(pVNic->pvHwContext);
}

NDIS_STATUS
VNic11QueryInterruptModerationSettings(
    __in  PVNIC                   pVNic,
    __out PNDIS_INTERRUPT_MODERATION_PARAMETERS   IntModParams
    )
{
    return Hw11QueryInterruptModerationSettings(pVNic->pvHw, IntModParams);
}

NDIS_STATUS
VNic11QuerySupportedRXAntenna(
    __in  PVNIC                   pVNic,
    __in  ULONG                   MaxEntries,
    __out PDOT11_SUPPORTED_ANTENNA_LIST   Dot11SupportedAntennaList    
    )
{
    return Hw11QuerySupportedRXAntenna(pVNic->pvHw, MaxEntries, Dot11SupportedAntennaList);
}

NDIS_STATUS
VNic11QuerySupportedTXAntenna(
    __in  PVNIC                   pVNic,
    __in  ULONG                   MaxEntries,
    __out PDOT11_SUPPORTED_ANTENNA_LIST   Dot11SupportedAntennaList
    )
{
    return Hw11QuerySupportedTXAntenna(pVNic->pvHw, MaxEntries, Dot11SupportedAntennaList);
}

ULONG
VNic11QueryHardwareStatus(
    __in  PVNIC                   pVNic
    )
{
    return Hw11QueryHardwareStatus(pVNic->pvHw);
}

ULONG
VNic11QueryReceiveBufferSpace(
    __in  PVNIC                   pVNic
    )
{
    return Hw11QueryReceiveBufferSpace(pVNic->pvHw);
}

ULONG
VNic11QueryTransmitBufferSpace(
    __in  PVNIC                   pVNic
    )
{
    return Hw11QueryTransmitBufferSpace(pVNic->pvHw);
}

NDIS_STATUS
VNic11QueryVendorDescription(
    __in  PVNIC                   pVNic,
    __out PVOID                   InformationBuffer,
    __in  ULONG                   InformationBufferLength,
    __out PULONG                  BytesWritten,
    __out PULONG                  BytesNeeded
    )
{
    return Hw11QueryVendorDescription(pVNic->pvHw, 
                InformationBuffer, 
                InformationBufferLength, 
                BytesWritten, 
                BytesNeeded
                );
}

ULONG
VNic11QueryVendorId(
    __in  PVNIC                   pVNic
     )
{
    return Hw11QueryVendorId(pVNic->pvHw);
}

NDIS_STATUS
VNic11QueryCurrentOptionalCapability(
    __in  PVNIC                   pVNic,
    __in PDOT11_CURRENT_OPTIONAL_CAPABILITY   Dot11CurrentOptionalCapability    
    )
{
    return Hw11QueryCurrentOptionalCapability(pVNic->pvHw, Dot11CurrentOptionalCapability);
}

NDIS_STATUS
VNic11QueryOperationModeCapability(
    __in  PVNIC                   pVNic,
    __out PDOT11_OPERATION_MODE_CAPABILITY    Dot11OpModeCapability
    )
{
    return Hw11QueryOperationModeCapability(pVNic->pvHw, Dot11OpModeCapability);
}

NDIS_STATUS
VNic11QueryOptionalCapability(
    __in  PVNIC                   pVNic,
    __out PDOT11_OPTIONAL_CAPABILITY  Dot11OptionalCapability
    )
{
    return Hw11QueryOptionalCapability(pVNic->pvHw, Dot11OptionalCapability);
}

PDOT11_MAC_ADDRESS
VNic11QueryHardwareAddress(
    __in  PVNIC                   pVNic
    )
{
    return Hw11QueryHardwareAddress(pVNic->pvHw);
}

ULONG
VNic11QueryRFUsage(
    __in  PVNIC                   pVNic
    )
{
    return Hw11QueryRFUsage(pVNic->pvHw);
}

ULONG
VNic11QueryMaxReceiveLifetime(
    __in  PVNIC                   pVNic
    )
{
    return Hw11QueryMaxReceiveLifetime(pVNic->pvHw);
}

ULONG
VNic11QueryMaxTransmitMSDULifetime(
    __in  PVNIC                   pVNic
    )
{
    return Hw11QueryMaxTransmitMSDULifetime(pVNic->pvHw);
}

/*
    Functions that are tied to a specific instantiation of a VNIC. 
    */

/*
    Query functions.
    VNIC holds its lock while querying data from the Hw. 
    */
    
PDOT11_MAC_ADDRESS
VNic11QueryMACAddress(
    __in  PVNIC                   pVNic
    )
{
    PDOT11_MAC_ADDRESS pMac = NULL;
    
    VNicLock(pVNic);
    pMac = Hw11QueryMACAddress(pVNic->pvHwContext);
    VNicUnlock(pVNic);

    return pMac;
}

PDOT11_MAC_ADDRESS
VNic11QueryCurrentBSSID(
    __in  PVNIC                   pVNic
    )
{
    PDOT11_MAC_ADDRESS pBSSID = NULL;

    VNicLock(pVNic);
    pBSSID = Hw11QueryCurrentBSSID(pVNic->pvHwContext);
    VNicUnlock(pVNic);

    return pBSSID;
}

ULONG
VNic11QueryBeaconPeriod(
    __in  PVNIC                   pVNic
    )
{
    ULONG ulBeaconPeriod = 0;
    
    VNicLock(pVNic);
    ulBeaconPeriod = Hw11QueryBeaconPeriod(pVNic->pvHwContext);
    VNicUnlock(pVNic);

    return ulBeaconPeriod;
}

DOT11_PHY_TYPE
VNic11QueryCurrentPhyType(
    __in  PVNIC                   pVNic
    )
{
    DOT11_PHY_TYPE phyType;
    
    VNicLock(pVNic);
    phyType = Hw11QueryCurrentPhyType(pVNic->pvHwContext);
    VNicUnlock(pVNic);

    return phyType;
}

BOOLEAN
VNic11QueryShortSlotTimeOptionImplemented(
    __in  PVNIC                   pVNic,
    __in  BOOLEAN                 SelectedPhy
    )
{
    BOOLEAN fImplemented = FALSE;
    
    VNicLock(pVNic);
    fImplemented = Hw11QueryShortSlotTimeOptionImplemented(pVNic->pvHwContext, SelectedPhy);
    VNicUnlock(pVNic);
    
    return fImplemented;
}

BOOLEAN
VNic11QueryShortSlotTimeOptionEnabled(
    __in  PVNIC                   pVNic,
    __in  BOOLEAN                 SelectedPhy
    )
{
    BOOLEAN fEnabled = FALSE;
    
    VNicLock(pVNic);
    fEnabled = Hw11QueryShortSlotTimeOptionEnabled(pVNic->pvHwContext, SelectedPhy);
    VNicUnlock(pVNic);

    return fEnabled;
}

BOOLEAN
VNic11QueryDsssOfdmOptionImplemented(
    __in  PVNIC                   pVNic,
    __in  BOOLEAN                 SelectedPhy
    )
{
    BOOLEAN fImplemented = FALSE;
    
    VNicLock(pVNic);
    fImplemented = Hw11QueryDsssOfdmOptionImplemented(pVNic->pvHwContext, SelectedPhy);
    VNicUnlock(pVNic);
    
    return fImplemented;
}

BOOLEAN
VNic11QueryDsssOfdmOptionEnabled(
    __in  PVNIC                   pVNic,
    __in  BOOLEAN                 SelectedPhy
    )
{
    BOOLEAN fEnabled = FALSE;
    
    VNicLock(pVNic);
    fEnabled = Hw11QueryDsssOfdmOptionEnabled(pVNic->pvHwContext, SelectedPhy);
    VNicUnlock(pVNic);

    return fEnabled;
}

BOOLEAN
VNic11QueryShortPreambleOptionImplemented(
    __in  PVNIC                   pVNic,
    __in  BOOLEAN                 SelectedPhy
    )
{
    BOOLEAN fImplemented = FALSE;
    
    VNicLock(pVNic);
    fImplemented = Hw11QueryShortPreambleOptionImplemented(pVNic->pvHwContext, SelectedPhy);
    VNicUnlock(pVNic);
    
    return fImplemented;
}

BOOLEAN
VNic11QueryPbccOptionImplemented(
    __in  PVNIC                   pVNic,
    __in  BOOLEAN                 SelectedPhy
    )
{
    BOOLEAN fImplemented = FALSE;
    
    VNicLock(pVNic);
    fImplemented = Hw11QueryPbccOptionImplemented(pVNic->pvHwContext, SelectedPhy);
    VNicUnlock(pVNic);
    
    return fImplemented;
}

BOOLEAN
VNic11QueryChannelAgilityPresent(
    __in  PVNIC                   pVNic,
    __in  BOOLEAN                 SelectedPhy
    )
{
    BOOLEAN fPresent = FALSE;
    
    VNicLock(pVNic);
    fPresent = Hw11QueryChannelAgilityPresent(pVNic->pvHwContext, SelectedPhy);
    VNicUnlock(pVNic);
    
    return fPresent;
}


BOOLEAN
VNic11QueryChannelAgilityEnabled(
    __in  PVNIC                   pVNic,
    __in  BOOLEAN                 SelectedPhy
    )
{
    BOOLEAN fEnabled = FALSE;
    
    VNicLock(pVNic);
    fEnabled = Hw11QueryChannelAgilityEnabled(pVNic->pvHwContext, SelectedPhy);
    VNicUnlock(pVNic);

    return fEnabled;
}

VOID
VNic11QueryBasicRateSet(
    __in  PVNIC                   pVNic,
    __out PDOT11_RATE_SET         Dot11RateSet,
    __in  BOOLEAN                 SelectedPhy
    )
{
    VNicLock(pVNic);
    Hw11QueryBasicRateSet(pVNic->pvHwContext, Dot11RateSet, SelectedPhy);
    VNicUnlock(pVNic);

    return;
}

VOID
VNic11QueryOperationalRateSet(
    __in  PVNIC                   pVNic,
    __out PDOT11_RATE_SET         Dot11RateSet,
    __in  BOOLEAN                 SelectedPhy
    )
{
    VNicLock(pVNic);
    Hw11QueryOperationalRateSet(pVNic->pvHwContext, Dot11RateSet, SelectedPhy);
    VNicUnlock(pVNic);

    return;
}

ULONG
VNic11QueryCurrentChannel(
    __in  PVNIC                   pVNic,
    __in  BOOLEAN                 SelectedPhy
    )
{
    ULONG ulChannel = 0;
    
    VNicLock(pVNic);
    ulChannel = Hw11QueryCurrentChannel(pVNic->pvHwContext, SelectedPhy);
    VNicUnlock(pVNic);

    return ulChannel;
}

ULONG
VNic11QueryATIMWindow(
    __in  PVNIC                   pVNic
    )
{
    ULONG ulATIMWindow = 0;
    
    VNicLock(pVNic);
    ulATIMWindow = Hw11QueryATIMWindow(pVNic->pvHwContext);
    VNicUnlock(pVNic);

    return ulATIMWindow;
}

ULONG
VNic11QueryOperatingPhyId(
    __in  PVNIC                   pVNic
    )
{
    ULONG ulPhyId = 0;
    
    VNicLock(pVNic);
    ulPhyId = Hw11QueryOperatingPhyId(pVNic->pvHwContext);
    VNicUnlock(pVNic);

    return ulPhyId;
}

DOT11_PHY_TYPE
VNic11DeterminePHYType(
    __in  PVNIC                   pVNic,
    __in  DOT11_CAPABILITY        Capability,
    __in  UCHAR                   Channel
    )
{
    DOT11_PHY_TYPE phyType;
    
    VNicLock(pVNic);
    phyType = Hw11DeterminePHYType(pVNic->pvHwContext, Capability, Channel);
    VNicUnlock(pVNic);

    return phyType;
}

NDIS_STATUS
VNic11ValidateOperationalRates(
    __in  PVNIC                   pVNic,
    __in  ULONG                   PhyId,
    __in_bcount(RateSetLength)  PUCHAR                  DataRateSet,
    __in  ULONG                   RateSetLength
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
    VNicLock(pVNic);
    ndisStatus = Hw11ValidateOperationalRates(pVNic->pvHwContext, PhyId, DataRateSet, RateSetLength);
    VNicUnlock(pVNic);

    return ndisStatus;
}

BOOLEAN
VNic11IsKeyMappingKeyAvailable(
    __in  PVNIC                   pVNic,
    __in_bcount(DOT11_ADDRESS_SIZE)  DOT11_MAC_ADDRESS       MacAddr
    )
{
    BOOLEAN fAvailable = FALSE;
    
    VNicLock(pVNic);
    fAvailable = Hw11IsKeyMappingKeyAvailable(pVNic->pvHwContext, MacAddr);
    VNicUnlock(pVNic);

    return fAvailable;
}

BOOLEAN
VNic11QueryNicPowerState(
    __in  PVNIC                   pVNic,
    __in  BOOLEAN                 SelectedPhy
    )
{
    BOOLEAN fPower = FALSE;
    
    VNicLock(pVNic);
    fPower = Hw11QueryNicPowerState(pVNic->pvHwContext, SelectedPhy);
    VNicUnlock(pVNic);

    return fPower;
}

NDIS_STATUS
VNic11QueryRecvSensitivityList(
    __in  PVNIC                   pVNic,
    __in  ULONG                   MaxEntries,
    __inout PDOT11_RECV_SENSITIVITY_LIST Dot11RecvSensitivityList
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
    VNicLock(pVNic);
    ndisStatus = Hw11QueryRecvSensitivityList(pVNic->pvHwContext, MaxEntries, Dot11RecvSensitivityList);
    VNicUnlock(pVNic);

    return ndisStatus;
}

ULONG
VNic11QueryDefaultKeyId(
    __in  PVNIC                   pVNic
    )
{
    ULONG ulDefaultKetId = 0;
    
    VNicLock(pVNic);
    ulDefaultKetId = Hw11QueryDefaultKeyId(pVNic->pvHwContext);
    VNicUnlock(pVNic);

    return ulDefaultKetId;
}

ULONG
VNic11QuerySelectedPhyId(
    __in  PVNIC                   pVNic
    )
{
    ULONG ulPhyId = 0;
    
    VNicLock(pVNic);
    ulPhyId = Hw11QuerySelectedPhyId(pVNic->pvHwContext);
    VNicUnlock(pVNic);

    return ulPhyId;
}

NDIS_STATUS
VNic11QueryDataRateMappingTable(
    __in  PVNIC                   pVNic,
    __out PDOT11_DATA_RATE_MAPPING_TABLE  DataRateMappingTable,
    __in  ULONG                   TotalLength
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
    VNicLock(pVNic);
    ndisStatus = Hw11QueryDataRateMappingTable(pVNic->pvHwContext, DataRateMappingTable, TotalLength);
    VNicUnlock(pVNic);

    return ndisStatus;
}

NDIS_STATUS
VNic11QuerySupportedPowerLevels(
    __in  PVNIC                   pVNic,
    __out PDOT11_SUPPORTED_POWER_LEVELS   Dot11SupportedPowerLevels
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
    VNicLock(pVNic);
    ndisStatus = Hw11QuerySupportedPowerLevels(pVNic->pvHwContext, Dot11SupportedPowerLevels);
    VNicUnlock(pVNic);

    return ndisStatus;
}

NDIS_STATUS
VNic11QueryDiversitySelectionRX(
    __in  PVNIC                   pVNic,
    __in  BOOLEAN                 SelectedPhy,
    __in  ULONG                   MaxEntries,
    __out PDOT11_DIVERSITY_SELECTION_RX_LIST Dot11DiversitySelectionRXList
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
    VNicLock(pVNic);
    ndisStatus = Hw11QueryDiversitySelectionRX(pVNic->pvHwContext, 
                SelectedPhy,
                MaxEntries, 
                Dot11DiversitySelectionRXList
                );

    VNicUnlock(pVNic);

    return ndisStatus;
}

NDIS_STATUS
VNic11QueryRegDomainsSupportValue(
    __in  PVNIC                   pVNic,
    __in  BOOLEAN                 SelectedPhy,
    __in  ULONG                   MaxEntries,
    __out PDOT11_REG_DOMAINS_SUPPORT_VALUE    Dot11RegDomainsSupportValue
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
    VNicLock(pVNic);
    ndisStatus = Hw11QueryRegDomainsSupportValue(pVNic->pvHwContext, 
                SelectedPhy,
                MaxEntries, 
                Dot11RegDomainsSupportValue
                );

    VNicUnlock(pVNic);

    return ndisStatus;
}

ULONG
VNic11QueryLookahead(
    __in  PVNIC                   pVNic
    )
{
    ULONG ulLookahead = 0;
    
    VNicLock(pVNic);
    ulLookahead = Hw11QueryLookahead(pVNic->pvHwContext);
    VNicUnlock(pVNic);

    return ulLookahead;
}

ULONG
VNic11QueryMaxMPDULength(
    __in  PVNIC                   pVNic
    )
{
    ULONG ulLen = 0;
    
    VNicLock(pVNic);
    ulLen = Hw11QueryMaxMPDULength(pVNic->pvHwContext);
    VNicUnlock(pVNic);

    return ulLen;
}

NDIS_STATUS
VNic11QueryMulticastList(
    __in  PVNIC                   pVNic,
    __out PVOID                   AddressBuffer,
    __in  ULONG                   AddressBufferLength,
    __out PULONG                  BytesWritten,
    __out PULONG                  BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
    VNicLock(pVNic);
    ndisStatus = Hw11QueryMulticastList(pVNic->pvHwContext, 
                AddressBuffer, 
                AddressBufferLength, 
                BytesWritten, 
                BytesNeeded
                );

    VNicUnlock(pVNic);

    return ndisStatus;
}

NDIS_STATUS
VNic11QuerySupportedDataRatesValue(
    __in  PVNIC                   pVNic,
    __out PDOT11_SUPPORTED_DATA_RATES_VALUE_V2    Dot11SupportedDataRatesValue,
    __in  BOOLEAN                 SelectedPhy 
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
    VNicLock(pVNic);
    ndisStatus = Hw11QuerySupportedDataRatesValue(pVNic->pvHwContext, Dot11SupportedDataRatesValue, SelectedPhy);
    VNicUnlock(pVNic);

    return ndisStatus;
}

ULONG
VNic11QueryCCAModeSupported(
    __in  PVNIC                   pVNic,
    __in  BOOLEAN                 SelectedPhy
    )
{
    ULONG ulSupported = 0;
    
    VNicLock(pVNic);
    ulSupported = Hw11QueryCCAModeSupported(pVNic->pvHwContext, SelectedPhy);
    VNicUnlock(pVNic);

    return ulSupported;
}

NDIS_STATUS
VNic11QueryCountryString(
    __in  PVNIC                   pVNic,
    __out PDOT11_COUNTRY_OR_REGION_STRING Dot11CountryString
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
    VNicLock(pVNic);
    ndisStatus = Hw11QueryCountryString(pVNic->pvHwContext, Dot11CountryString);
    VNicUnlock(pVNic);

    return ndisStatus;
}

ULONG
VNic11QueryCurrentCCAMode(
    __in  PVNIC                   pVNic,
    __in  BOOLEAN                 SelectedPhy
    )
{
    ULONG ulMode = 0;
    
    VNicLock(pVNic);
    ulMode = Hw11QueryCurrentCCAMode(pVNic->pvHwContext, SelectedPhy);
    VNicUnlock(pVNic);

    return ulMode;
}

ULONG
VNic11QueryCurrentRegDomain(
    __in  PVNIC                   pVNic
    )
{
    ULONG ulRegDomain = 0;
    
    VNicLock(pVNic);
    ulRegDomain = Hw11QueryCurrentRegDomain(pVNic->pvHwContext);
    VNicUnlock(pVNic);

    return ulRegDomain;
}

ULONG
VNic11QueryCurrentTXPowerLevel(
    __in  PVNIC                   pVNic,
    __in  BOOLEAN                 SelectedPhy
    )
{
    ULONG ulPowerLevel = 0;
    
    VNicLock(pVNic);
    ulPowerLevel = Hw11QueryCurrentTXPowerLevel(pVNic->pvHwContext, SelectedPhy);
    VNicUnlock(pVNic);

    return ulPowerLevel;
}

DOT11_DIVERSITY_SUPPORT
VNic11QueryDiversitySupport(
    __in  PVNIC                   pVNic,
    __in  BOOLEAN                 SelectedPhy
    )
{
    DOT11_DIVERSITY_SUPPORT    dot11DiversitySupport;

    VNicLock(pVNic);
    dot11DiversitySupport = Hw11QueryDiversitySupport(pVNic->pvHwContext, SelectedPhy);
    VNicUnlock(pVNic);

    return dot11DiversitySupport;
}

ULONG
VNic11QueryEDThreshold(
    __in  PVNIC                   pVNic,
    __in  BOOLEAN                 SelectedPhy
    )
{
    ULONG ulThreshold = 0;
    
    VNicLock(pVNic);
    ulThreshold = Hw11QueryEDThreshold(pVNic->pvHwContext, SelectedPhy);
    VNicUnlock(pVNic);

    return ulThreshold;
}

BOOLEAN
VNic11QueryErpPbccOptionEnabled(
    __in  PVNIC                   pVNic,
    __in  BOOLEAN                 SelectedPhy
    )
{
    BOOLEAN fEnabled = FALSE;
    
    VNicLock(pVNic);
    fEnabled = Hw11QueryErpPbccOptionEnabled(pVNic->pvHwContext, SelectedPhy);
    VNicUnlock(pVNic);

    return fEnabled;
}

BOOLEAN
VNic11QueryErpPbccOptionImplemented(
    __in  PVNIC                   pVNic,
    __in  BOOLEAN                 SelectedPhy
    )
{
    BOOLEAN fImplemented = FALSE;
    
    VNicLock(pVNic);
    fImplemented = Hw11QueryErpPbccOptionImplemented(pVNic->pvHwContext, SelectedPhy);
    VNicUnlock(pVNic);

    return fImplemented;
}

ULONG
VNic11QueryFragmentationThreshold(
    __in  PVNIC                   pVNic
    )
{
    ULONG ulThreshold = 0;
    
    VNicLock(pVNic);
    ulThreshold = Hw11QueryFragmentationThreshold(pVNic->pvHwContext);
    VNicUnlock(pVNic);

    return ulThreshold;
}

ULONG
VNic11QueryFrequencyBandsSupported(
    __in  PVNIC                   pVNic,
    __in  BOOLEAN                 SelectedPhy
    )
{
    ULONG ulBands = 0;
    
    VNicLock(pVNic);
    ulBands = Hw11QueryFrequencyBandsSupported(pVNic->pvHwContext, SelectedPhy);
    VNicUnlock(pVNic);

    return ulBands;
}

NDIS_STATUS
VNic11QuerySupportedChannels(
    __in  PVNIC                   pVNic,
    __in  ULONG                   PhyId,
    __out PULONG                  ChannelCount,
    __out_opt PULONG                  ChannelList
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
    VNicLock(pVNic);
    ndisStatus = Hw11QuerySupportedChannels(
                pVNic->pvHwContext, 
                PhyId,
                ChannelCount, 
                ChannelList
                );
    VNicUnlock(pVNic);

    return ndisStatus;
}

ULONG
VNic11QueryLongRetryLimit(
    __in  PVNIC                   pVNic
    )
{
    ULONG ulRetryLimit = 0;
    
    VNicLock(pVNic);
    ulRetryLimit = Hw11QueryLongRetryLimit(pVNic->pvHwContext);
    VNicUnlock(pVNic);

    return ulRetryLimit;
}

BOOLEAN
VNic11QueryMultiDomainCapabilityEnabled(
    __in  PVNIC                   pVNic
    )
{
    BOOLEAN fEnabled = FALSE;
    
    VNicLock(pVNic);
    fEnabled = Hw11QueryMultiDomainCapabilityEnabled(pVNic->pvHwContext);
    VNicUnlock(pVNic);

    return fEnabled;
}

BOOLEAN
VNic11QueryMultiDomainCapabilityImplemented(
    __in  PVNIC                   pVNic
    )
{
    BOOLEAN fImplemented = FALSE;
    
    VNicLock(pVNic);
    fImplemented = Hw11QueryMultiDomainCapabilityImplemented(pVNic->pvHwContext);
    VNicUnlock(pVNic);

    return fImplemented;
}

ULONG
VNic11QueryRTSThreshold(
    __in  PVNIC                   pVNic
    )
{
    ULONG ulThreshold = 0;
    
    VNicLock(pVNic);
    ulThreshold = Hw11QueryRTSThreshold(pVNic->pvHwContext);
    VNicUnlock(pVNic);

    return ulThreshold;
}

ULONG
VNic11QueryShortRetryLimit(
    __in  PVNIC                   pVNic
    )
{
    ULONG ulRetryLimit = 0;
    
    VNicLock(pVNic);
    ulRetryLimit = Hw11QueryShortRetryLimit(pVNic->pvHwContext);
    VNicUnlock(pVNic);

    return ulRetryLimit;
}

DOT11_TEMP_TYPE
VNic11QueryTempType(
    __in  PVNIC                   pVNic,
    __in  BOOLEAN                 SelectedPhy
    )
{
    DOT11_TEMP_TYPE    dot11TempType;
    
    VNicLock(pVNic);
    dot11TempType = Hw11QueryTempType(pVNic->pvHwContext, SelectedPhy);
    VNicUnlock(pVNic);

    return dot11TempType;
}


NDIS_STATUS
VNic11QueryLinkParameters(
    __in  PVNIC                   pVNic,
    __out PNDIS_LINK_PARAMETERS   LinkParams
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
    VNicLock(pVNic);
    ndisStatus = Hw11QueryLinkParameters(pVNic->pvHwContext, LinkParams);
    VNicUnlock(pVNic);

    return ndisStatus;
}


NDIS_STATUS
VNic11QueryDot11Statistics(
    __in  PVNIC                   pVNic,
    __in  PDOT11_STATISTICS       Dot11Stats,
    __in  ULONG                   InformationBufferLength,
    __out PULONG                  BytesWritten,
    __out PULONG                  BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
    VNicLock(pVNic);
    ndisStatus = Hw11QueryDot11Statistics(
                pVNic->pvHwContext, 
                Dot11Stats, 
                InformationBufferLength, 
                BytesWritten, 
                BytesNeeded
                );
    VNicUnlock(pVNic);

    return ndisStatus;
}

/*
    Set functions.
    
    The VNIC should give up its lock before calling into the hardware since the driver expects to
    be called at Dispatch. Also the VNIC gives the guarantee to the Hw layer that while the Hw
    call is in progress the VNIC would continue to remain active. This, along with no lock requirement, 
    implies that the VNIC must increment the context switch ref count before calling into the 
    hardware. This ref count will be removed when the sync function returns and when the 
    appropriate callback is called for the async functions.

    The Port layer will guarantee that it doesn't call multiple such set functions at the same time
    */
    

NDIS_STATUS
VNic11SetATIMWindow(
    __in  PVNIC                   pVNic,
    __in  ULONG                   Value
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN fProgramHw = FALSE, fPerformPostHwSyncCall = FALSE;

    do
    {
        ndisStatus = VNicPreHwSyncCallActions(pVNic, &fProgramHw);
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): VNicPreHwSyncCallActions failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }
        fPerformPostHwSyncCall = TRUE;
        
        ndisStatus = Hw11SetATIMWindow(pVNic->pvHwContext, Value, fProgramHw);
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): Hw11SetATIMWindow failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }
        
    } while (FALSE);

    if (fPerformPostHwSyncCall)
    {
        VNicPostHwSyncCallActions(pVNic, fProgramHw);
    }
    
    return ndisStatus;
}

VOID
VNic11SetCipher(
    __in  PVNIC                   pVNic,
    __in  BOOLEAN                 IsUnicast,
    __in  DOT11_CIPHER_ALGORITHM  AlgoId
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN fProgramHw = FALSE, fPerformPostHwSyncCall = FALSE;

    do
    {
        ndisStatus = VNicPreHwSyncCallActions(pVNic, &fProgramHw);
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): VNicPreHwSyncCallActions failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }
        fPerformPostHwSyncCall = TRUE;
    
        Hw11SetCipher(pVNic->pvHwContext, IsUnicast, AlgoId, fProgramHw);
        
    } while (FALSE);
    
    if (fPerformPostHwSyncCall)
    {
        VNicPostHwSyncCallActions(pVNic, fProgramHw);
    }    
    
    return;    
}

UCHAR
VNic11SelectTXDataRate(
    __in  PVNIC                   pVNic,
    __in  PDOT11_RATE_SET         PeerRateSet,
    __in  ULONG                   LinkQuality
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN fProgramHw = FALSE, fPerformPostHwSyncCall = FALSE;
    UCHAR ucRate = 0;

    do
    {
        ndisStatus = VNicPreHwSyncCallActions(pVNic, &fProgramHw);
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): VNicPreHwSyncCallActions failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }
        fPerformPostHwSyncCall = TRUE;
    
        ucRate = Hw11SelectTXDataRate(pVNic->pvHwContext, PeerRateSet, LinkQuality, fProgramHw);

    } while (FALSE);
    
    if (fPerformPostHwSyncCall)
    {
        VNicPostHwSyncCallActions(pVNic, fProgramHw);
    }    
    
    return ucRate;
}

// BUGBUG: Does this potentially trigger a context merge operation? How does the HVL know about this?
VOID
VNic11DeleteNonPersistentKey(
    __in  PVNIC                   pVNic
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN fProgramHw = FALSE, fPerformPostHwSyncCall = FALSE;

    do
    {
        ndisStatus = VNicPreHwSyncCallActions(pVNic, &fProgramHw);
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): VNicPreHwSyncCallActions failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }
        fPerformPostHwSyncCall = TRUE;
    
        Hw11DeleteNonPersistentKey(pVNic->pvHwContext, fProgramHw);

    } while (FALSE);
    
    if (fPerformPostHwSyncCall)
    {
        VNicPostHwSyncCallActions(pVNic, fProgramHw);
    }
    
    
    return;
}

// BUGBUG: Does this potentially trigger a context merge operation? How does the HVL know about this?
VOID
VNic11DeleteNonPersistentMappingKey(
    __in  PVNIC                   pVNic,
    __in_bcount(DOT11_ADDRESS_SIZE)  DOT11_MAC_ADDRESS       MacAddr
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN fProgramHw = FALSE, fPerformPostHwSyncCall = FALSE;

    do
    {
        ndisStatus = VNicPreHwSyncCallActions(pVNic, &fProgramHw);
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): VNicPreHwSyncCallActions failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }
        fPerformPostHwSyncCall = TRUE;
    
        Hw11DeleteNonPersistentMappingKey(pVNic->pvHwContext, MacAddr, fProgramHw);

    } while (FALSE);
    
    if (fPerformPostHwSyncCall)
    {
        VNicPostHwSyncCallActions(pVNic, fProgramHw);
    }
        
    return;    
}

NDIS_STATUS
VNic11SetPowerMgmtMode(
    __in  PVNIC                       pVNic,
    __in  PDOT11_POWER_MGMT_MODE      PMMode
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN fProgramHw = FALSE, fPerformPostHwSyncCall = FALSE;

    do
    {
        ndisStatus = VNicPreHwSyncCallActions(pVNic, &fProgramHw);
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): VNicPreHwSyncCallActions failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }
        fPerformPostHwSyncCall = TRUE;
    
        ndisStatus = Hw11SetPowerMgmtMode(pVNic->pvHwContext, PMMode, fProgramHw);
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): Hw11SetPowerMgmtMode failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }

    } while (FALSE);
    
    if (fPerformPostHwSyncCall)
    {
        VNicPostHwSyncCallActions(pVNic, fProgramHw);
    }
    

    return ndisStatus;    
}

VOID
VNic11SetCTSToSelfOption(
    __in  PVNIC                   pVNic,
    __in  BOOLEAN                 fEnable
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN fProgramHw = FALSE, fPerformPostHwSyncCall = FALSE;

    do
    {
        ndisStatus = VNicPreHwSyncCallActions(pVNic, &fProgramHw);
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): VNicPreHwSyncCallActions failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }
        fPerformPostHwSyncCall = TRUE;
    
        Hw11SetCTSToSelfOption(pVNic->pvHwContext, fEnable, fProgramHw);

    } while (FALSE);

    if (fPerformPostHwSyncCall)
    {
        VNicPostHwSyncCallActions(pVNic, fProgramHw);
    }    
    
    return;
}

NDIS_STATUS
VNic11SetPacketFilter(
    __in  PVNIC                   pVNic,
    __in  ULONG                   PacketFilter
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN fProgramHw = FALSE, fPerformPostHwSyncCall = FALSE;

    do
    {
        ndisStatus = VNicPreHwSyncCallActions(pVNic, &fProgramHw);
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): VNicPreHwSyncCallActions failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }
        fPerformPostHwSyncCall = TRUE;
    
        ndisStatus = Hw11SetPacketFilter(pVNic->pvHwContext, PacketFilter, fProgramHw);
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): Hw11SetPacketFilter failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }

    } while (FALSE);

    if (fPerformPostHwSyncCall)
    {
        VNicPostHwSyncCallActions(pVNic, fProgramHw);
    }
       
    return ndisStatus;
}


NDIS_STATUS
VNic11SetOperationMode(
    __in  PVNIC                   pVNic,
    __in  PDOT11_CURRENT_OPERATION_MODE Dot11CurrentOperationMode
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN fProgramHw = FALSE, fPerformPostHwSyncCall = FALSE;

    do
    {
        ndisStatus = VNicPreHwSyncCallActions(pVNic, &fProgramHw);
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): VNicPreHwSyncCallActions failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }
        fPerformPostHwSyncCall = TRUE;

        ndisStatus = Hw11SetOperationMode(pVNic->pvHwContext, Dot11CurrentOperationMode, fProgramHw);
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): Hw11SetOperationMode failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }

        /*
            if the operational mode being set is netmon, set our context switch flag to true
            */
        VNicLock(pVNic);
        if (DOT11_OPERATION_MODE_NETWORK_MONITOR == Dot11CurrentOperationMode->uCurrentOpMode)
        {
            VNicSetReadyToCtxS(pVNic, TRUE);
        }
        else
        {
            VNicSetReadyToCtxS(pVNic, FALSE);
        }
        VNicUnlock(pVNic);
        
    } while (FALSE);

    if (fPerformPostHwSyncCall)
    {
        VNicPostHwSyncCallActions(pVNic, fProgramHw);
    }
        
    return ndisStatus;
}

NDIS_STATUS
VNic11SetCurrentBSSType(
    __in  PVNIC                   pVNic,
    __in  DOT11_BSS_TYPE          Type
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN fProgramHw = FALSE, fPerformPostHwSyncCall = FALSE;

    do
    {
        ndisStatus = VNicPreHwSyncCallActions(pVNic, &fProgramHw);
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): VNicPreHwSyncCallActions failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }
        fPerformPostHwSyncCall = TRUE;

        ndisStatus = Hw11SetCurrentBSSType(pVNic->pvHwContext, Type, fProgramHw);
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): Hw11SetCurrentBSSType failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }

    } while (FALSE);

    if (fPerformPostHwSyncCall)
    {
        VNicPostHwSyncCallActions(pVNic, fProgramHw);
    }
    
    
    return ndisStatus;
}

NDIS_STATUS
VNic11SetAuthentication(
    __in  PVNIC                   pVNic,
    __in  DOT11_AUTH_ALGORITHM    AlgoId
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN fProgramHw = FALSE, fPerformPostHwSyncCall = FALSE;

    do 
    {
        ndisStatus = VNicPreHwSyncCallActions(pVNic, &fProgramHw);
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): VNicPreHwSyncCallActions failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }
        fPerformPostHwSyncCall = TRUE;

        ndisStatus = Hw11SetAuthentication(pVNic->pvHwContext, AlgoId, fProgramHw);
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): Hw11SetAuthentication failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }

    } while (FALSE);

    if (fPerformPostHwSyncCall)
    {
        VNicPostHwSyncCallActions(pVNic, fProgramHw);
    }
        
    return ndisStatus;
}

NDIS_STATUS
VNic11SetSelectedPhyId(
    __in  PVNIC                   pVNic,
    __in  ULONG                   PhyId
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN fProgramHw = FALSE, fPerformPostHwSyncCall = FALSE;

    do
    {
        ndisStatus = VNicPreHwSyncCallActions(pVNic, &fProgramHw);    
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): VNicPreHwSyncCallActions failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }
        fPerformPostHwSyncCall = TRUE;

        ndisStatus = Hw11SetSelectedPhyId(pVNic->pvHwContext, PhyId, fProgramHw);
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): Hw11SetSelectedPhyId failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }

    } while (FALSE);

    if (fPerformPostHwSyncCall)
    {
        VNicPostHwSyncCallActions(pVNic, fProgramHw);
    }    
    
    return ndisStatus;
}

NDIS_STATUS
VNic11SetSafeModeOption(
    __in  PVNIC                   pVNic,
    __in  BOOLEAN                 SafeModeEnabled
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN fProgramHw = FALSE, fPerformPostHwSyncCall = FALSE;

    do
    {
        ndisStatus = VNicPreHwSyncCallActions(pVNic, &fProgramHw);    
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): VNicPreHwSyncCallActions failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }
        fPerformPostHwSyncCall = TRUE;

        ndisStatus = Hw11SetSafeModeOption(pVNic->pvHwContext, SafeModeEnabled, fProgramHw);
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): Hw11SetSafeModeOption failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }

    } while (FALSE);

    if (fPerformPostHwSyncCall)
    {
        VNicPostHwSyncCallActions(pVNic, fProgramHw);
    }
    
    
    return ndisStatus;
}

NDIS_STATUS
VNic11SetLinkParameters(
    __in  PVNIC                   pVNic,
    __in  PNDIS_LINK_PARAMETERS   LinkParams
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN fProgramHw = FALSE, fPerformPostHwSyncCall = FALSE;

    do
    {
        ndisStatus = VNicPreHwSyncCallActions(pVNic, &fProgramHw);    
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): VNicPreHwSyncCallActions failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }
        fPerformPostHwSyncCall = TRUE;

        ndisStatus = Hw11SetLinkParameters(pVNic->pvHwContext, LinkParams, fProgramHw);
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): Hw11SetLinkParameters failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }

    } while (FALSE);

    if (fPerformPostHwSyncCall)
    {
        VNicPostHwSyncCallActions(pVNic, fProgramHw);
    }
        
    return ndisStatus;
}

NDIS_STATUS
VNic11SetMulticastList(
    __in  PVNIC                   pVNic,
    __in  PVOID                   AddressBuffer,
    __in  ULONG                   AddressBufferLength
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN fProgramHw = FALSE, fPerformPostHwSyncCall = FALSE;

    do
    {
        ndisStatus = VNicPreHwSyncCallActions(pVNic, &fProgramHw);    
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): VNicPreHwSyncCallActions failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }
        fPerformPostHwSyncCall = TRUE;

        ndisStatus = Hw11SetMulticastList(pVNic->pvHwContext, AddressBuffer, AddressBufferLength, fProgramHw);
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): Hw11SetMulticastList failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }

    } while (FALSE);

    if (fPerformPostHwSyncCall)
    {
        VNicPostHwSyncCallActions(pVNic, fProgramHw);
    }
        
    return ndisStatus;
}

NDIS_STATUS
VNic11SetOperationalRateSet(
    __in  PVNIC                   pVNic,
    __in  PDOT11_RATE_SET         Dot11RateSet,
    __in  BOOLEAN                 SelectedPhy 
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN fProgramHw = FALSE, fPerformPostHwSyncCall = FALSE;

    do
    {
        ndisStatus = VNicPreHwSyncCallActions(pVNic, &fProgramHw);    
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): VNicPreHwSyncCallActions failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }
        fPerformPostHwSyncCall = TRUE;

        ndisStatus = Hw11SetOperationalRateSet(pVNic->pvHwContext, Dot11RateSet, SelectedPhy, fProgramHw);
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): Hw11SetOperationalRateSet failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }

    } while (FALSE);

    if (fPerformPostHwSyncCall)
    {
        VNicPostHwSyncCallActions(pVNic, fProgramHw);
    }
        
    return ndisStatus;
}

NDIS_STATUS
VNic11SetLookahead(
    __in  PVNIC                   pVNic,
    __in  ULONG                   Lookahead
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN fProgramHw = FALSE, fPerformPostHwSyncCall = FALSE;

    do
    {
        ndisStatus = VNicPreHwSyncCallActions(pVNic, &fProgramHw);    
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): VNicPreHwSyncCallActions failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }
        fPerformPostHwSyncCall = TRUE;

        ndisStatus = Hw11SetLookahead(pVNic->pvHwContext, Lookahead, fProgramHw);
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): Hw11SetLookahead failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }

    } while (FALSE);

    if (fPerformPostHwSyncCall)
    {
        VNicPostHwSyncCallActions(pVNic, fProgramHw);
    }
    
    
    return ndisStatus;
}

NDIS_STATUS
VNic11SetNicPowerState(
    __in  PVNIC                   pVNic,
    __in  BOOLEAN                 PowerState,
    __in  BOOLEAN                 SelectedPhy
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN fProgramHw = FALSE, fPerformPostHwSyncCall = FALSE;

    do
    {
        ndisStatus = VNicPreHwSyncCallActions(pVNic, &fProgramHw);    
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): VNicPreHwSyncCallActions failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }
        fPerformPostHwSyncCall = TRUE;

        ndisStatus = Hw11SetNicPowerState(pVNic->pvHwContext, PowerState, SelectedPhy, fProgramHw);
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): Hw11SetNicPowerState failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }

    } while (FALSE);

    if (fPerformPostHwSyncCall)
    {
        VNicPostHwSyncCallActions(pVNic, fProgramHw);
    }    
    
    return ndisStatus;
}

NDIS_STATUS
VNic11SetBeaconPeriod(
    __in  PVNIC                   pVNic,
    __in  ULONG                   BeaconPeriod
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN fProgramHw = FALSE, fPerformPostHwSyncCall = FALSE;

    do
    {
        ndisStatus = VNicPreHwSyncCallActions(pVNic, &fProgramHw);    
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): VNicPreHwSyncCallActions failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }
        fPerformPostHwSyncCall = TRUE;

        ndisStatus = Hw11SetBeaconPeriod(pVNic->pvHwContext, BeaconPeriod, fProgramHw);
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): Hw11SetBeaconPeriod failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }

    } while (FALSE);

    if (fPerformPostHwSyncCall)
    {
        VNicPostHwSyncCallActions(pVNic, fProgramHw);
    }    
    
    return ndisStatus;
}

NDIS_STATUS
VNic11SetFragmentationThreshold(
    __in  PVNIC                   pVNic,
    __in  ULONG                   BeaconPeriod
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN fProgramHw = FALSE, fPerformPostHwSyncCall = FALSE;

    do
    {
        ndisStatus = VNicPreHwSyncCallActions(pVNic, &fProgramHw);
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): VNicPreHwSyncCallActions failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }
        fPerformPostHwSyncCall = TRUE;

        ndisStatus = Hw11SetFragmentationThreshold(pVNic->pvHwContext, BeaconPeriod, fProgramHw);
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): Hw11SetFragmentationThreshold failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }

    } while (FALSE);

    if (fPerformPostHwSyncCall)
    {
        VNicPostHwSyncCallActions(pVNic, fProgramHw);
    }
    
    
    return ndisStatus;
}

NDIS_STATUS
VNic11SetMultiDomainCapabilityEnabled(
    __in  PVNIC                   pVNic,
    __in  BOOLEAN                 MultiDomainCapabilityEnabled
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN fProgramHw = FALSE, fPerformPostHwSyncCall = FALSE;

    do
    {
        ndisStatus = VNicPreHwSyncCallActions(pVNic, &fProgramHw);    
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): VNicPreHwSyncCallActions failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }
        fPerformPostHwSyncCall = TRUE;

        ndisStatus = Hw11SetMultiDomainCapabilityEnabled(pVNic->pvHwContext, MultiDomainCapabilityEnabled, fProgramHw);
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): Hw11SetMultiDomainCapabilityEnabled failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }

    } while (FALSE);

    if (fPerformPostHwSyncCall)
    {
        VNicPostHwSyncCallActions(pVNic, fProgramHw);
    }    
    
    return ndisStatus;
}

NDIS_STATUS
VNic11SetRTSThreshold(
    __in  PVNIC                   pVNic,
    __in  ULONG                   ulRTSThreshold
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN fProgramHw = FALSE, fPerformPostHwSyncCall = FALSE;

    do
    {
        ndisStatus = VNicPreHwSyncCallActions(pVNic, &fProgramHw);    
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): VNicPreHwSyncCallActions failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }
        fPerformPostHwSyncCall = TRUE;

        ndisStatus = Hw11SetRTSThreshold(pVNic->pvHwContext, ulRTSThreshold, fProgramHw);
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): Hw11SetRTSThreshold failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }

    } while (FALSE);

    if (fPerformPostHwSyncCall)
    {
        VNicPostHwSyncCallActions(pVNic, fProgramHw);
    }
    
    
    return ndisStatus;
}

NDIS_STATUS
VNic11SetDefaultKeyId(
    __in  PVNIC                   pVNic,
    __in  ULONG                   KeyId
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN fProgramHw = FALSE, fPerformPostHwSyncCall = FALSE;

    do
    {
        ndisStatus = VNicPreHwSyncCallActions(pVNic, &fProgramHw);    
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): VNicPreHwSyncCallActions failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }
        fPerformPostHwSyncCall = TRUE;

        ndisStatus = Hw11SetDefaultKeyId(pVNic->pvHwContext, KeyId, fProgramHw);
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): Hw11SetDefaultKeyId failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }

    } while (FALSE);

    if (fPerformPostHwSyncCall)
    {
        VNicPostHwSyncCallActions(pVNic, fProgramHw);
    }
    
    
    return ndisStatus;
}

NDIS_STATUS
VNic11SetKeyMappingKey(
    __in  PVNIC                   pVNic,
    __in_bcount(DOT11_ADDRESS_SIZE)  DOT11_MAC_ADDRESS       MacAddr,
    __in  ULONG                   KeyID,
    __in  BOOLEAN                 Persistent,
    __in  DOT11_CIPHER_ALGORITHM  AlgoId,
    __in  ULONG                   KeyLength,
    __in_bcount(KeyLength)  PUCHAR                  KeyValue    
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN fProgramHw = FALSE, fPerformPostHwSyncCall = FALSE;

    do
    {
        ndisStatus = VNicPreHwSyncCallActions(pVNic, &fProgramHw);    
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): VNicPreHwSyncCallActions failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }
        fPerformPostHwSyncCall = TRUE;

        ndisStatus = Hw11SetKeyMappingKey(pVNic->pvHwContext, MacAddr, KeyID, Persistent, AlgoId, KeyLength, KeyValue, fProgramHw);
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): Hw11SetKeyMappingKey failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }

    } while (FALSE);
    
    if (fPerformPostHwSyncCall)
    {
        VNicPostHwSyncCallActions(pVNic, fProgramHw);
    }    
    
    return ndisStatus;
}


// DISRUPTIVE OPERATIONS BEGIN

/*
    For asynchrnous operations the contract with the port layer is that if a function completes
    synchronously (either a success or failure), the VNIC layer is not going to call it back
    */
    
NDIS_STATUS
VNic11SetChannel(
    __in  PVNIC                   pVNic,
    __in  ULONG                   ulChannel,
    __in  ULONG                   ulPhyId,
    __in  BOOLEAN                 fSwitchPhy,
    __in  PORT11_GENERIC_CALLBACK_FUNC pfnCompletionHandler
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PCH_SWITCH_REQ pChSwReq = NULL;

    do
    {
        ndisStatus = VNicAllocateChSwReq(pVNic, ulChannel, ulPhyId, fSwitchPhy, pfnCompletionHandler, &pChSwReq);
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): VNicAllocateChSwReq failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }
        
        ndisStatus = VNicHandleExAccessOp(pVNic, PENDING_OP_CH_SW_REQ, pChSwReq, TRUE, FALSE);
        MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): VNicHandleExAccessOp return 0x%x\n", VNIC_PORT_NO, ndisStatus));

        if (NDIS_STATUS_PENDING != ndisStatus)
        {
            // the operation completed synchronously - free the allocated memory
            VNicFreeChSwReq(pChSwReq);
        }
    } while (FALSE);

    return ndisStatus;
}


#ifndef HIDE_ANNOTATION
__drv_mustHold(KeSpinLockType)
__drv_requiresIRQL(DISPATCH_LEVEL)
#endif
NDIS_STATUS 
VNicChSwComplete(
    __in  PVNIC                   pVNic,
    __in  BOOLEAN                 fReferenced,
    __in  PORT11_GENERIC_CALLBACK_FUNC ChSwCompleteHandler,
    __in  PVOID                   Data
    )
{
    PVOID pvPort = NULL;
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
    MpTrace(COMP_HVL, DBG_NORMAL, ("VNic(%d): Hw completed channel switch operation \n", VNIC_PORT_NO));


    pvPort = pVNic->pvPort;

    // remove the ref count that was added for the channel switch operation
    if (fReferenced)
    {
        VNicRemoveCtxSRef(pVNic, REF_CH_SW);
    }

    // unlock before calling into the port
    VNicUnlock(pVNic);

    // Call the completion handler in the port
    if (ChSwCompleteHandler)
    {
        ndisStatus = ChSwCompleteHandler(pvPort, Data);
    }

    VNicLock(pVNic);

    return ndisStatus;
}


NDIS_STATUS 
VNic11ChSwCompleteCallback(
    __in  PVNIC                   pVNic,
    __in  PVOID                   pvCtx,
    __in  PVOID                   Data
    )
{
    PVNIC_COMPLETION_CTX pCtx = NULL;
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PORT11_GENERIC_CALLBACK_FUNC ChSwCompleteHandler = NULL;
    
    MpTrace(COMP_HVL, DBG_NORMAL, ("VNic(%d): Hw completed channel switch operation \n", VNIC_PORT_NO));

    pCtx = (PVNIC_COMPLETION_CTX)pvCtx;
    ASSERT(pCtx);

    ChSwCompleteHandler = pCtx->CompletionFn;

    VNicLock(pVNic);
    ndisStatus = VNicChSwComplete(pVNic, TRUE, ChSwCompleteHandler, Data);
    VNicUnlock(pVNic);

    FREE_MEM(pvCtx);
    
    return ndisStatus;
}

#ifndef HIDE_ANNOTATION
__drv_mustHold(KeSpinLockType)
__drv_requiresIRQL(DISPATCH_LEVEL)
#endif
NDIS_STATUS
VNicSwChHelper(
    __in  PVNIC                   pVNic,
    __in  ULONG                   ulChannel,
    __in  ULONG                   ulPhyId,
    __in  BOOLEAN                 fSwitchPhy,
    __in  PVNIC_COMPLETION_CTX    pCtx
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN fReferenced = FALSE;

    ASSERT(VNicIsLocked(pVNic));
    ASSERT(VNicIsActive(pVNic) && VNicHasExAccess(pVNic) && !VNicIsInReset(pVNic));

    do
    {
        /*
            We can call the hardware for a channel switch. We need to give up our lock before
            calling the hardware. The context switch ref count will be decremented when we
            have completed the channel switch
            */

        // add the context switch ref count for the async join call to the hardware
        VNicAddCtxSRef(pVNic, REF_CH_SW);
        fReferenced = TRUE;
        
        VNicUnlock(pVNic);

        ndisStatus = Hw11SetChannel(
                    pVNic->pvHwContext, 
                    ulChannel, 
                    ulPhyId, 
                    fSwitchPhy,
                    VNic11ChSwCompleteCallback,
                    pCtx
                    );
        MpTrace(COMP_HVL, DBG_NORMAL, ("VNic(%d): Called the hardware for the channel switch operation 0x%x\n", VNIC_PORT_NO, ndisStatus));
        
        VNicLock(pVNic);
    } while (FALSE);

    if (NDIS_STATUS_PENDING != ndisStatus)
    {
        // the call to the hardware completed. Remove the ref count we added
        if (fReferenced)
        {
            VNicRemoveCtxSRef(pVNic, REF_CH_SW);
        }
    }    
           
    return ndisStatus;
}


 NDIS_STATUS
VNic11SetDefaultKey(
    __in  PVNIC                   pVNic,
    __in_bcount(DOT11_ADDRESS_SIZE)  DOT11_MAC_ADDRESS       MacAddr,
    __in  ULONG                   KeyID,
    __in  BOOLEAN                 Persistent,
    __in  DOT11_CIPHER_ALGORITHM  AlgoId,
    __in  ULONG                   KeyLength,
    __in_bcount(KeyLength)  PUCHAR                  KeyValue    
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDEF_KEY_REQ pDefKeyReq = NULL;
    NDIS_EVENT CompletionEvent;

    do
    {
        NdisZeroMemory(&CompletionEvent, sizeof(NDIS_EVENT));
        NdisInitializeEvent(&CompletionEvent);
        NdisResetEvent(&CompletionEvent);
        
        ndisStatus = VNicAllocateDefaultKeyReq(pVNic, &CompletionEvent, MacAddr, KeyID, Persistent, AlgoId, KeyLength, KeyValue, &pDefKeyReq);
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): VNicAllocateDefaultKeyReq failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }
        
        ndisStatus = VNicHandleExAccessOp(pVNic, PENDING_OP_DEF_KEY, pDefKeyReq, TRUE, FALSE);
        MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): VNicHandleExAccessOp called 0x%x\n", VNIC_PORT_NO, ndisStatus));

        // we provide a synchronous model to the Port so wait for the completion event
        if (NDIS_STATUS_PENDING == ndisStatus)
        {
            NdisWaitEvent(&CompletionEvent, 0);
            ndisStatus = NDIS_STATUS_SUCCESS;
        }
        else
        {
            // the operation completed synchronously - free the allocated memory
            VNicFreeDefKeyReq(pDefKeyReq);
        }
    } while (FALSE);

    return ndisStatus;
}

#ifndef HIDE_ANNOTATION
__drv_mustHold(KeSpinLockType)
__drv_requiresIRQL(DISPATCH_LEVEL)
#endif
NDIS_STATUS
VNicSetDefaultKeyHelper(
    __in  PVNIC                   pVNic,
    __in_bcount(DOT11_ADDRESS_SIZE)  DOT11_MAC_ADDRESS       MacAddr,
    __in  ULONG                   KeyID,
    __in  BOOLEAN                 Persistent,
    __in  DOT11_CIPHER_ALGORITHM  AlgoId,
    __in  ULONG                   KeyLength,
    __in_bcount(KeyLength)  PUCHAR                  KeyValue    
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    ASSERT(VNicIsLocked(pVNic));
    ASSERT(VNicIsActive(pVNic) && VNicHasExAccess(pVNic) && !VNicIsInReset(pVNic));
    
    do
    {
        /*
            add the context switch ref count since we need to give up our lock before we call the
            hardware
            */
        
        VNicAddCtxSRef(pVNic, REF_DEF_KEY);
        
        VNicUnlock(pVNic);

        ndisStatus = Hw11SetDefaultKey(
                    pVNic->pvHwContext,
                    MacAddr,
                    KeyID,
                    Persistent,
                    AlgoId,
                    KeyLength,
                    KeyValue,
                    TRUE
                    );
        MpTrace(COMP_HVL, DBG_NORMAL, ("VNic(%d): Called the hardware to set the default key %d. Status = %!x!\n", VNIC_PORT_NO, KeyID, ndisStatus));

        ASSERT(NDIS_STATUS_PENDING != ndisStatus);
        
        VNicLock(pVNic);

        VNicRemoveCtxSRef(pVNic, REF_DEF_KEY);
    } while (FALSE);

    return ndisStatus;
}

NDIS_STATUS
VNic11SetOperatingPhyId(
    __in  PVNIC                   pVNic,
    __in  ULONG                   PhyId
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    POPERATING_PHY_ID_REQ pPhyIdReq = NULL;
    NDIS_EVENT CompletionEvent;

    do
    {
        NdisZeroMemory(&CompletionEvent, sizeof(NDIS_EVENT));
        NdisInitializeEvent(&CompletionEvent);
        NdisResetEvent(&CompletionEvent);
        
        ndisStatus = VNicAllocateOperatingPhyIdReq(pVNic, &CompletionEvent, PhyId, &pPhyIdReq);
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): VNicAllocateOperatingPhyIdReq failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }
        
        ndisStatus = VNicHandleExAccessOp(pVNic, PENDING_OP_OPERATING_PHY_ID, pPhyIdReq, TRUE, FALSE);
        MpTrace(COMP_HVL, DBG_NORMAL, ("VNic(%d): Called the hardware to set the operating phy id %d. Status = %!x!\n", VNIC_PORT_NO, PhyId, ndisStatus));

        // we provide a synchronous model to the Port so wait for the completion event
        if (NDIS_STATUS_PENDING == ndisStatus)
        {
            NdisWaitEvent(&CompletionEvent, 0);
            ndisStatus = NDIS_STATUS_SUCCESS;
        }
        else
        {
            // the operation completed synchronously - free the allocated memory
            VNicFreeOperatingPhyIdReq(pPhyIdReq);
        }
    } while (FALSE);

    return ndisStatus;
}

#ifndef HIDE_ANNOTATION
__drv_mustHold(KeSpinLockType)
__drv_requiresIRQL(DISPATCH_LEVEL)
#endif
NDIS_STATUS
VNicSetOperatingPhyIdHelper(
    __in  PVNIC                   pVNic,
    __in  ULONG                   PhyId
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    ASSERT(VNicIsLocked(pVNic));

    do
    {
        if (VNicIsInReset(pVNic))
        {
            ndisStatus = NDIS_STATUS_RESET_IN_PROGRESS;
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): Reset is in progress. Discarding the set operating Phy ID request.\n", VNIC_PORT_NO));
            break;
        }        
        
        /*
            add the context switch ref count since we need to give up our lock before we call the
            hardware
            */
        
        VNicAddCtxSRef(pVNic, REF_OPERATING_PHY_ID);
        
        VNicUnlock(pVNic);

        ndisStatus = Hw11SetOperatingPhyId(
                    pVNic->pvHwContext,
                    PhyId,
                    TRUE
                    );
        MpTrace(COMP_HVL, DBG_NORMAL, ("VNic(%d): Called the hardware to set the operating phy ID %d. Status = %!x!\n", VNIC_PORT_NO, PhyId, ndisStatus));
        
        VNicLock(pVNic);

        VNicRemoveCtxSRef(pVNic, REF_OPERATING_PHY_ID);
    } while (FALSE);

    return ndisStatus;
}

NDIS_STATUS
VNic11SetDesiredPhyIdList(
    __in  PVNIC                   pVNic,
    __in_ecount(PhyIDCount)  PULONG                  PhyIDList,
    __in  ULONG                   PhyIDCount
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDESIRED_PHY_ID_LIST_REQ pDesiredPhyIdListReq = NULL;
    NDIS_EVENT CompletionEvent;

    do
    {
        NdisZeroMemory(&CompletionEvent, sizeof(NDIS_EVENT));
        NdisInitializeEvent(&CompletionEvent);
        NdisResetEvent(&CompletionEvent);
        
        ndisStatus = VNicAllocateDesiredPhyIdListReq(pVNic, &CompletionEvent, PhyIDList, PhyIDCount, &pDesiredPhyIdListReq);
        if (NDIS_STATUS_SUCCESS != ndisStatus)
        {
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): VNicAllocateDesiredPhyIdListReq failed 0x%x\n", VNIC_PORT_NO, ndisStatus));
            break;
        }
        
        ndisStatus = VNicHandleExAccessOp(pVNic, PENDING_OP_DESIRED_PHY_ID_LIST, pDesiredPhyIdListReq, TRUE, FALSE);
        MpTrace(COMP_HVL, DBG_NORMAL, ("VNic(%d): Called the hardware to set the desired phy id list. Status = %!x!\n", VNIC_PORT_NO, ndisStatus));

        // we provide a synchronous model to the Port so wait for the completion event
        if (NDIS_STATUS_PENDING == ndisStatus)
        {
            NdisWaitEvent(&CompletionEvent, 0);
            ndisStatus = NDIS_STATUS_SUCCESS;
        }
        else
        {
            // the operation completed synchronously - free the allocated memory
            VNicFreeDesiredPhyIdListReq(pDesiredPhyIdListReq);
        }
    } while (FALSE);

    return ndisStatus;
}

#ifndef HIDE_ANNOTATION
__drv_mustHold(KeSpinLockType)
__drv_requiresIRQL(DISPATCH_LEVEL)
#endif
NDIS_STATUS
VNicSetDesiredPhyIdListHelper(
    __in  PVNIC                   pVNic,
    __in  PULONG                  PhyIDList,
    __in  ULONG                   PhyIDCount
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    ASSERT(VNicIsLocked(pVNic));

    do
    {
        if (VNicIsInReset(pVNic))
        {
            ndisStatus = NDIS_STATUS_RESET_IN_PROGRESS;
            MpTrace(COMP_HVL, DBG_SERIOUS, ("VNic(%d): Reset is in progress. Discarding the set operating Phy ID request.\n", VNIC_PORT_NO));
            break;
        }        
        
        /*
            add the context switch ref count since we need to give up our lock before we call the
            hardware
            */
        
        VNicAddCtxSRef(pVNic, REF_DESIRED_PHY_ID_LIST);
        
        VNicUnlock(pVNic);

        ndisStatus = Hw11SetDesiredPhyIdList(pVNic->pvHwContext, PhyIDList, PhyIDCount, TRUE);
        MpTrace(COMP_HVL, DBG_NORMAL, ("VNic(%d): Called the hardware to set the desired phy ID list. Status = %!x!\n", VNIC_PORT_NO, ndisStatus));
        
        VNicLock(pVNic);

        VNicRemoveCtxSRef(pVNic, REF_DESIRED_PHY_ID_LIST);
    } while (FALSE);

    return ndisStatus;
}


