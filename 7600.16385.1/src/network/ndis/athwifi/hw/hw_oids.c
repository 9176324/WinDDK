/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    hw_oids.c

Abstract:
    Implements the OIDs for the HW layer
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    09-04-2007    Created

Notes:

--*/


#include "precomp.h"
#include "hw_oids.h"
#include "hw_mac.h"
#include "hw_phy.h"
#include "hw_main.h"
#include "hw_rate.h"

#if DOT11_TRACE_ENABLED
#include "hw_oids.tmh"
#endif


NDIS_STATUS
Hw11Fill80211Attributes(
    __in  PHW                     Hw,
    __out PNDIS_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES attr
    )
{
    PDOT11_PHY_ATTRIBUTES   PhyAttr;
    ULONG                   PhyId;
    ULONG                   index;
    UCHAR                   rate;
    PNICPHYMIB              phyMib;

    attr->NumOfTXBuffers = Hw->RegInfo.NumTXBuffers;
    attr->NumOfRXBuffers = Hw->RegInfo.NumRXBuffers;
    attr->MultiDomainCapabilityImplemented = HwQueryMultiDomainCapabilityImplemented(Hw);
    attr->NumSupportedPhys = HalGetSupportedPhyTypes(Hw->Hal)->uNumOfEntries;

    MP_ALLOCATE_MEMORY(Hw->MiniportAdapterHandle, 
                       &attr->SupportedPhyAttributes,
                       attr->NumSupportedPhys * sizeof(DOT11_PHY_ATTRIBUTES),
                       HW_MEMORY_TAG);
    if (attr->SupportedPhyAttributes == NULL)
    {
        return NDIS_STATUS_RESOURCES;
    }

    //
    // Fill the PHY attributes
    //
    for (PhyId = 0, PhyAttr = attr->SupportedPhyAttributes; PhyId < attr->NumSupportedPhys; PhyId++, PhyAttr++)
    {
        MP_ASSIGN_NDIS_OBJECT_HEADER(PhyAttr->Header, 
            NDIS_OBJECT_TYPE_DEFAULT,
            DOT11_PHY_ATTRIBUTES_REVISION_1,
            sizeof(DOT11_PHY_ATTRIBUTES));

        phyMib = HalGetPhyMIB(Hw->Hal, PhyId);
        
        PhyAttr->PhyType = phyMib->PhyType;
        PhyAttr->bHardwarePhyState = HwQueryHardwarePhyState(Hw, PhyId);
        PhyAttr->bSoftwarePhyState = HwQuerySoftwarePhyState(Hw, PhyId);
        PhyAttr->bCFPollable = FALSE;
        PhyAttr->uMPDUMaxLength = HW11_MAX_FRAME_SIZE;
        PhyAttr->TempType = HwQueryTempType(Hw, PhyId);
        PhyAttr->DiversitySupport = HwQueryDiversitySupport(Hw, PhyId);

        PhyAttr->uNumberSupportedPowerLevels 
            = Hw->PhyState.SupportedPowerLevels.uNumOfSupportedPowerLevels;
        for (index = 0; index < PhyAttr->uNumberSupportedPowerLevels; index++)
        {
            PhyAttr->TxPowerLevels[index] = Hw->PhyState.SupportedPowerLevels.uTxPowerLevelValues[index];
        }

        switch (PhyAttr->PhyType)
        {
            case dot11_phy_type_hrdsss:
                PhyAttr->HRDSSSAttributes.bShortPreambleOptionImplemented 
                    = HwQueryShortPreambleOptionImplemented(Hw, PhyId);
                PhyAttr->HRDSSSAttributes.bPBCCOptionImplemented
                    = HwQueryPbccOptionImplemented(Hw, PhyId);
                PhyAttr->HRDSSSAttributes.bChannelAgilityPresent
                    = HwQueryChannelAgilityPresent(Hw, PhyId);
                PhyAttr->HRDSSSAttributes.uHRCCAModeSupported 
                    = HwQueryCCAModeSupported(Hw, PhyId);
                break;

            case dot11_phy_type_ofdm:
                PhyAttr->OFDMAttributes.uFrequencyBandsSupported 
                    = HwQueryFrequencyBandsSupported(Hw, PhyId);
                break;

            case dot11_phy_type_erp:
                PhyAttr->ERPAttributes.bShortPreambleOptionImplemented
                    = HwQueryShortPreambleOptionImplemented(Hw, PhyId);
                PhyAttr->ERPAttributes.bPBCCOptionImplemented
                    = HwQueryPbccOptionImplemented(Hw, PhyId);
                PhyAttr->ERPAttributes.bChannelAgilityPresent
                    = HwQueryChannelAgilityPresent(Hw, PhyId);
                PhyAttr->ERPAttributes.uHRCCAModeSupported
                    = HwQueryCCAModeSupported(Hw, PhyId);
                PhyAttr->ERPAttributes.bERPPBCCOptionImplemented = FALSE;
                PhyAttr->ERPAttributes.bDSSSOFDMOptionImplemented = FALSE;
                PhyAttr->ERPAttributes.bShortSlotTimeOptionImplemented 
                    = HwQueryShortSlotTimeOptionImplemented(Hw, PhyId);
                break;

            default:
                break;
        }

        NdisMoveMemory(&PhyAttr->SupportedDataRatesValue,
                       &HalGetPhyMIB(Hw->Hal, PhyId)->SupportedDataRatesValue,
                       sizeof(DOT11_SUPPORTED_DATA_RATES_VALUE_V2));

        index = 0;
        while ((rate = PhyAttr->SupportedDataRatesValue.ucSupportedTxDataRatesValue[index]) != 0 && 
               index < DOT11_RATE_SET_MAX_LENGTH)
        {
            PhyAttr->DataRateMappingEntries[index].ucDataRateIndex = rate;
            PhyAttr->DataRateMappingEntries[index].ucDataRateFlag = 0;
            PhyAttr->DataRateMappingEntries[index].usDataRateValue = (USHORT)rate;

            index++;
        }

        PhyAttr->uNumDataRateMappingEntries = index;
    }

    return NDIS_STATUS_SUCCESS;
}

VOID
Hw11Cleanup80211Attributes(
    __in  PHW                     Hw,
    __in  PNDIS_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES attr
    )
{
    UNREFERENCED_PARAMETER(Hw);

    if (attr->SupportedPhyAttributes)
    {
        MP_FREE_MEMORY(attr->SupportedPhyAttributes);
    }
}

PDOT11_MAC_ADDRESS
Hw11QueryMACAddress(
    __in  PHW_MAC_CONTEXT         HwMac
    )
{
    // Return the address assigned to this MAC
    return &HwMac->MacAddress;
}


PDOT11_MAC_ADDRESS
Hw11QueryCurrentAddress(
    __in  PHW                     Hw
    )
{
    // Return the MAC address programmed on the hardware
    return &Hw->MacState.MacAddress;
}

VOID
Hw11QueryPnPCapabilities(
    __in  PHW                     Hw,
    __out PNDIS_PNP_CAPABILITIES  NdisPnPCapabilities
    )
{
    UNREFERENCED_PARAMETER(Hw);
    NdisZeroMemory(NdisPnPCapabilities, sizeof(NDIS_PNP_CAPABILITIES));

    // If we support WOL, we would fill in additional capabilities here
}

VOID
Hw11QueryPMCapabilities(
    __in  PHW                     Hw,
    __out PNDIS_PM_CAPABILITIES   NdisPmCapabilities
    )
{
    UNREFERENCED_PARAMETER(Hw);
    NdisZeroMemory(NdisPmCapabilities, sizeof(NDIS_PM_CAPABILITIES));

    // Fill in the PM capabilities
    NdisPmCapabilities->Header.Type = NDIS_OBJECT_TYPE_DEFAULT;
    NdisPmCapabilities->Header.Revision = NDIS_PM_CAPABILITIES_REVISION_1;
    NdisPmCapabilities->Header.Size = NDIS_SIZEOF_NDIS_PM_CAPABILITIES_REVISION_1;

    // If we supported WOL, we would fill in additional capabilities here
}

BOOLEAN 
Hw11WEP104Implemented(
    __in  PHW                     Hw
    )
{
    UNREFERENCED_PARAMETER(Hw);
    return TRUE;
}

BOOLEAN 
Hw11WEP40Implemented(
    __in  PHW                     Hw
    )
{
    UNREFERENCED_PARAMETER(Hw);
    return TRUE;
}

BOOLEAN 
Hw11TKIPImplemented(
    __in  PHW                     Hw
    )
{
    return (HalGetEncryptionCapabilities(Hw->Hal) & HAL_ENCRYPTION_SUPPORT_TKIP) ? TRUE : FALSE;
}

BOOLEAN 
Hw11CCMPImplemented(
    __in  PHW                     Hw
    )
{
    return (HalGetEncryptionCapabilities(Hw->Hal) & HAL_ENCRYPTION_SUPPORT_CCMP) ? TRUE : FALSE;
}

ULONG
Hw11DefaultKeyTableSize(
    __in  PHW                     Hw
    )
{
    UNREFERENCED_PARAMETER(Hw);
    return DOT11_MAX_NUM_DEFAULT_KEY;
}

ULONG
Hw11KeyMappingKeyTableSize(
    __in  PHW                     Hw
    )
{
    return (HalGetEncryptionCapabilities(Hw->Hal) & HAL_ENCRYPTION_SUPPORT_KEYMAPPINGKEYTABLE) ? 0 : 
                HW_KEY_MAPPING_KEY_TABLE_SIZE;
}

ULONG
Hw11PerStaKeyTableSize(
    __in  PHW                     Hw
    )
{
    UNREFERENCED_PARAMETER(Hw);
    return HW_PER_STA_KEY_TABLE_SIZE;
}

NDIS_STATUS
Hw11ValidateOperationalRates(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  ULONG                   PhyId,
    __in_bcount(RateSetLength)  PUCHAR                  DataRateSet,
    __in  ULONG                   RateSetLength
    )
{
    NDIS_STATUS                 ndisStatus = NDIS_STATUS_SUCCESS;
    ULONG                       i, j;
    BOOLEAN                     rateFound;
    PDOT11_RATE_SET             opRateSet;

    do
    {
        opRateSet = &(HalGetPhyMIB(HwMac->Hw->Hal, PhyId)->OperationalRateSet);

        if (RateSetLength > DOT11_RATE_SET_MAX_LENGTH)
        {
            MpTrace(COMP_ASSOC, DBG_LOUD, ("Bad rate set size in Rate Set\n"));
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            break;
        }

        for (i = 0; i < RateSetLength; i++)
        {
            rateFound = FALSE;
            
            for (j = 0; j < opRateSet->uRateSetLength; j++)
            {
                if ((DataRateSet[i] & 0x7F) == opRateSet->ucRateSet[j])
                {
                    rateFound = TRUE;
                    break;
                }
            }
                
            if (rateFound == FALSE)
            {
                MpTrace(COMP_ASSOC, DBG_LOUD,  ("Bad data rate %d detected\n", DataRateSet[i]));
                ndisStatus = NDIS_STATUS_INVALID_DATA;
                break;
            }
        }
    } while(FALSE);

    return ndisStatus;
}

PDOT11_MAC_ADDRESS
Hw11QueryCurrentBSSID(
    __in  PHW_MAC_CONTEXT         HwMac
    )
{
    return &HwMac->DesiredBSSID;
}

ULONG
Hw11QueryBeaconPeriod(
    __in  PHW_MAC_CONTEXT         HwMac
    )
{
    return HwMac->BeaconPeriod;
}

DOT11_PHY_TYPE
Hw11QueryCurrentPhyType(
    __in  PHW_MAC_CONTEXT         HwMac
    )
{
    PNICPHYMIB                  operatingPhyMib;

    //
    // Get the PHY MIB for the operating phy of this MAC context
    //
    operatingPhyMib = HalGetPhyMIB(HwMac->Hw->Hal, HwMac->OperatingPhyId);
    
    return operatingPhyMib->PhyType;
}

BOOLEAN
Hw11QueryShortSlotTimeOptionImplemented(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy
    )
{
    //
    // Determine the PHY that the user wants to query
    //
    if (SelectedPhy)
        return HwQueryShortSlotTimeOptionImplemented(HwMac->Hw, HwMac->SelectedPhyId);
    else
        return HwQueryShortSlotTimeOptionImplemented(HwMac->Hw, HwMac->OperatingPhyId);
}

BOOLEAN
Hw11QueryShortSlotTimeOptionEnabled(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy
    )
{
    PNICPHYMIB                  phyMib;

    //
    // Determine the PHY that the user wants to query
    //
    if (SelectedPhy)
        phyMib = HalGetPhyMIB(HwMac->Hw->Hal, HwMac->SelectedPhyId);
    else
        phyMib = HalGetPhyMIB(HwMac->Hw->Hal, HwMac->OperatingPhyId);

    //
    // Information about short slot time option is stored in the MAC context
    //
    if (phyMib->PhyType == dot11_phy_type_erp)
        return HwMac->ShortSlotTimeOptionEnabled;
    else 
        return FALSE;
}

BOOLEAN
Hw11QueryDsssOfdmOptionImplemented(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy
    )
{
    if (SelectedPhy)
        return HwQueryDsssOfdmOptionImplemented(HwMac->Hw, HwMac->SelectedPhyId);
    else
        return HwQueryDsssOfdmOptionImplemented(HwMac->Hw, HwMac->OperatingPhyId);
}

BOOLEAN
Hw11QueryDsssOfdmOptionEnabled(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy
    )
{
    UNREFERENCED_PARAMETER(HwMac);
    UNREFERENCED_PARAMETER(SelectedPhy);

    //
    // Not implemented, so always returning false
    //
    return FALSE;
}


BOOLEAN
Hw11QueryShortPreambleOptionImplemented(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy
    )
{
    //
    // Determine the PHY that the user wants to query
    //
    if (SelectedPhy)
        return HwQueryShortPreambleOptionImplemented(HwMac->Hw, HwMac->SelectedPhyId);
    else
        return HwQueryShortPreambleOptionImplemented(HwMac->Hw, HwMac->OperatingPhyId);
}

BOOLEAN
Hw11QueryPbccOptionImplemented(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy
    )
{
    //
    // Determine the PHY that the user wants to query
    //
    if (SelectedPhy)
        return HwQueryPbccOptionImplemented(HwMac->Hw, HwMac->SelectedPhyId);
    else
        return HwQueryPbccOptionImplemented(HwMac->Hw, HwMac->OperatingPhyId);
}

BOOLEAN
Hw11QueryChannelAgilityPresent(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy
    )
{
    //
    // Determine the PHY that the user wants to query
    //
    if (SelectedPhy)
        return HwQueryChannelAgilityPresent(HwMac->Hw, HwMac->SelectedPhyId);
    else
        return HwQueryChannelAgilityPresent(HwMac->Hw, HwMac->OperatingPhyId);
}


BOOLEAN
Hw11QueryChannelAgilityEnabled(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy
    )
{
    UNREFERENCED_PARAMETER(HwMac);
    UNREFERENCED_PARAMETER(SelectedPhy);

    //
    // Not present, so always returning false
    //
    return FALSE;
}

BOOLEAN
Hw11QueryCFPollable(
    __in  PHW                     Hw
    )
{
    UNREFERENCED_PARAMETER(Hw);
    return FALSE;
}

VOID
Hw11QueryBasicRateSet(
    __in  PHW_MAC_CONTEXT         HwMac,
    __out PDOT11_RATE_SET         Dot11RateSet,
    __in  BOOLEAN                 SelectedPhy
    )
{
    PNICPHYMIB                  phyMib;
    ULONG                       index;

    //
    // Determine the PHY that the user wants to query
    //
    if (SelectedPhy)
        phyMib = HalGetPhyMIB(HwMac->Hw->Hal, HwMac->SelectedPhyId);
    else
        phyMib = HalGetPhyMIB(HwMac->Hw->Hal, HwMac->OperatingPhyId);

    //
    // Copy the rate set
    //
    for (index = 0; 
         (index < phyMib->BasicRateSet.uRateSetLength) && (index < DOT11_RATE_SET_MAX_LENGTH); 
         index++)
    {
        Dot11RateSet->ucRateSet[index] = phyMib->BasicRateSet.ucRateSet[index];
    }

    Dot11RateSet->uRateSetLength = index;
}

VOID
Hw11QueryOperationalRateSet(
    __in  PHW_MAC_CONTEXT         HwMac,
    __out PDOT11_RATE_SET         Dot11RateSet,
    __in  BOOLEAN                 SelectedPhy
    )
{
    PNICPHYMIB                  phyMib;
    ULONG                       index;

    //
    // Determine the PHY that the user wants to query
    //
    if (SelectedPhy)
        phyMib = HalGetPhyMIB(HwMac->Hw->Hal, HwMac->SelectedPhyId);
    else
        phyMib = HalGetPhyMIB(HwMac->Hw->Hal, HwMac->OperatingPhyId);

    //
    // Copy the rate set
    //
    for (index = 0; 
         (index < phyMib->OperationalRateSet.uRateSetLength) && (index < DOT11_RATE_SET_MAX_LENGTH); 
         index++) 
    {
        Dot11RateSet->ucRateSet[index] = phyMib->OperationalRateSet.ucRateSet[index];
    }

    Dot11RateSet->uRateSetLength = index;
}

NDIS_STATUS
Hw11QuerySupportedPHYTypes(
    __in  PHW                     Hw,
    __in  ULONG                   NumMaxEntries,
    __out PDOT11_SUPPORTED_PHY_TYPES Dot11SupportedPhyTypes
    )
{
    USHORT                      index;
    PDOT11_SUPPORTED_PHY_TYPES  supportedPhyTypes = HalGetSupportedPhyTypes(Hw->Hal);

    if (NumMaxEntries < supportedPhyTypes->uNumOfEntries) 
    {
        Dot11SupportedPhyTypes->uTotalNumOfEntries = supportedPhyTypes->uNumOfEntries;    
        Dot11SupportedPhyTypes->uNumOfEntries = NumMaxEntries;

        for (index = 0; index < NumMaxEntries; index++)
            Dot11SupportedPhyTypes->dot11PHYType[index] = supportedPhyTypes->dot11PHYType[index];
        
        return NDIS_STATUS_BUFFER_OVERFLOW;
    }
    else 
    {
        Dot11SupportedPhyTypes->uTotalNumOfEntries = supportedPhyTypes->uNumOfEntries;
        Dot11SupportedPhyTypes->uNumOfEntries = supportedPhyTypes->uNumOfEntries;
        
        for (index = 0; index < supportedPhyTypes->uNumOfEntries; index++)
            Dot11SupportedPhyTypes->dot11PHYType[index] = supportedPhyTypes->dot11PHYType[index];
        
        return NDIS_STATUS_SUCCESS;
    }
}

NDIS_STATUS
Hw11QuerySupportedChannels(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  ULONG                   PhyId,
    __out PULONG                  ChannelCount,
    __out_opt PULONG                  ChannelList
    )
{
    return HalGetChannelList(HwMac->Hw->Hal, PhyId, ChannelCount, ChannelList);
}

ULONG
Hw11QueryCurrentChannel(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy
    )
{
    //
    // Determine the PHY that the user wants to query
    //
    if (SelectedPhy)
        return HwMac->PhyContext[HwMac->SelectedPhyId].Channel;
    else
        return HwMac->PhyContext[HwMac->OperatingPhyId].Channel;
}

ULONG
Hw11QueryATIMWindow(
    __in  PHW_MAC_CONTEXT         HwMac
    )
{
    return HwMac->AtimWindow;
}

ULONG
Hw11QueryOperatingPhyId(
    __in  PHW_MAC_CONTEXT         HwMac
    )
{
    return HwMac->OperatingPhyId;
}

BOOLEAN
Hw11IsConnected(
    __in  PHW_MAC_CONTEXT         HwMac
    )
{
    return ((HwMac->Status & HW_MAC_CONTEXT_LINK_UP)?TRUE:FALSE);
}


USHORT
Hw11QueryRSNCapabilityField(
    __in  PHW                     Hw
    )
{
    UNREFERENCED_PARAMETER(Hw);
    
    return 0;
}

DOT11_PHY_TYPE
Hw11DeterminePHYType(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  DOT11_CAPABILITY        Capability,
    __in  UCHAR                   Channel
    )
{
    PNICPHYMIB                  phyMib;

    if (Channel > 14)
    {
        return dot11_phy_type_ofdm;
    }
    else
    {
        phyMib = HalGetPhyMIB(HwMac->Hw->Hal, HwMac->OperatingPhyId);

        //
        // We have to determine between B and G.
        // If the Capability contains G-only field, it must be G
        // Otherwise, use our current operating PHY type.
        //
        if (Capability.ShortSlotTime || 
            Capability.DSSSOFDM ||
            phyMib->PhyType == dot11_phy_type_ofdm)
        {
            return dot11_phy_type_erp;
        }
        else
        {
            return phyMib->PhyType;
        }
    }
}

BOOLEAN
Hw11IsKeyMappingKeyAvailable(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in_bcount(DOT11_ADDRESS_SIZE)  DOT11_MAC_ADDRESS       MacAddr
    )
{
    UCHAR                       index;
    UCHAR                       count = HwMac->KeyMappingKeyCount;
    PHW_KEY_ENTRY               keyEntry;

    //
    // This is quicker more optimal version than HwFindKeyMappingKeyIndex used
    // on the data path
    //
    
    if (count == 0)
        return FALSE;

    for (index = DOT11_MAX_NUM_DEFAULT_KEY; index < HW11_KEY_TABLE_SIZE; index++)
    {
        keyEntry = &HwMac->KeyTable[index];
        if (keyEntry->Key.Valid)
        {
            if (MP_COMPARE_MAC_ADDRESS(keyEntry->Key.MacAddr, MacAddr))
            {
                return TRUE;
            }

            count--;
            if (count == 0)
            {
                return FALSE;
            }
        }
    }

    //
    // Can reach here if we dont find a key for the specific AP
    //
    return FALSE;
}


NDIS_STATUS
Hw11QueryDot11Statistics(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  PDOT11_STATISTICS       Dot11Stats,
    __in  ULONG                   InformationBufferLength,
    __out PULONG                  BytesWritten,
    __out PULONG                  BytesNeeded
    )
{
    PDOT11_SUPPORTED_PHY_TYPES  supportedPhyTypes = HalGetSupportedPhyTypes(HwMac->Hw->Hal);
    ULONG                       statsSize = sizeof(DOT11_STATISTICS) +
        (supportedPhyTypes->uNumOfEntries - 1) * sizeof(DOT11_PHY_FRAME_STATISTICS);
    PDOT11_PHY_FRAME_STATISTICS phyStats;
    ULONG                       phyId;

    if (InformationBufferLength < statsSize)
    {
        *BytesNeeded = statsSize;
        return NDIS_STATUS_BUFFER_OVERFLOW;
    }

    // Unicast counters
    NdisMoveMemory(&Dot11Stats->MacUcastCounters, 
        &HwMac->UnicastCounters, 
        sizeof(DOT11_MAC_FRAME_STATISTICS)
        );

    // Multicast counters
    NdisMoveMemory(&Dot11Stats->MacMcastCounters, 
        &HwMac->MulticastCounters, 
        sizeof(DOT11_MAC_FRAME_STATISTICS)
        );

    // Phy counters
    for (phyId = 0, phyStats = Dot11Stats->PhyCounters; 
            phyId < supportedPhyTypes->uNumOfEntries; 
            phyId++, phyStats++
        )
    {
        NdisMoveMemory(phyStats, 
            &HwMac->Hw->Stats.PhyCounters[phyId],
            sizeof(DOT11_PHY_FRAME_STATISTICS)
            );
    }

    *BytesWritten = statsSize;

    return NDIS_STATUS_SUCCESS;
}

BOOLEAN
Hw11QueryNicPowerState(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy
    )
{
    //
    // Determine the PHY that the user wants to query
    //
    if (SelectedPhy)
        return HwQueryNicPowerState(HwMac->Hw, HwMac->SelectedPhyId);
    else
        return HwQueryNicPowerState(HwMac->Hw, HwMac->OperatingPhyId);
}


NDIS_STATUS
Hw11QueryRecvSensitivityList(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  ULONG                   MaxEntries,
    __inout PDOT11_RECV_SENSITIVITY_LIST Dot11RecvSensitivityList
    )
{
    NDIS_STATUS                 ndisStatus = NDIS_STATUS_SUCCESS;
    ULONG                       i;
    PNICPHYMIB                  phyMib = NULL;
    PDOT11_SUPPORTED_PHY_TYPES  supportedPhyTypes = HalGetSupportedPhyTypes(HwMac->Hw->Hal);
    
    do
    {
        if (Dot11RecvSensitivityList->uPhyId >= supportedPhyTypes->uNumOfEntries)
        {
            MpTrace(COMP_OID, DBG_SERIOUS, ("Phy Id not supported by the NIC\n"));
            ndisStatus = NDIS_STATUS_FAILURE;
            break;
        }

        phyMib = HalGetPhyMIB(HwMac->Hw->Hal, Dot11RecvSensitivityList->uPhyId);

        Dot11RecvSensitivityList->uTotalNumOfEntries = 0;
        for (i = 0; i < MAX_NUM_SUPPORTED_RATES_V2; i++)
        {
            if (phyMib->SupportedDataRatesValue.ucSupportedRxDataRatesValue[i] != 0)
                Dot11RecvSensitivityList->uTotalNumOfEntries++;
        }
        
        //
        // Find out how many entries can we place?
        //
        if (MaxEntries < Dot11RecvSensitivityList->uTotalNumOfEntries)
        {
            Dot11RecvSensitivityList->uNumOfEntries = MaxEntries;
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
        }
        else
        {
            Dot11RecvSensitivityList->uNumOfEntries = Dot11RecvSensitivityList->uTotalNumOfEntries;
            ndisStatus = NDIS_STATUS_SUCCESS;
        }

        MPASSERT(Dot11RecvSensitivityList->uNumOfEntries >= 1 && Dot11RecvSensitivityList->uNumOfEntries <= 4);

        //
        // There is at least one element we can fill.
        //
        for (i = 0; i < Dot11RecvSensitivityList->uNumOfEntries; i++)
        {
            Dot11RecvSensitivityList->dot11RecvSensitivity[i].ucDataRate = phyMib->SupportedDataRatesValue.ucSupportedRxDataRatesValue[i];           
            Dot11RecvSensitivityList->dot11RecvSensitivity[i].lRSSIMax = HwQueryMaxRSSI(HwMac->Hw, Dot11RecvSensitivityList->dot11RecvSensitivity[i].ucDataRate);
            Dot11RecvSensitivityList->dot11RecvSensitivity[i].lRSSIMin = HwQueryMinRSSI(HwMac->Hw, Dot11RecvSensitivityList->dot11RecvSensitivity[i].ucDataRate);
        }
    } while(FALSE);

    return ndisStatus;
}

ULONG
Hw11QueryDefaultKeyId(
    __in  PHW_MAC_CONTEXT         HwMac
    )
{
    return HwMac->DefaultKeyIndex;
}

ULONG
Hw11QuerySelectedPhyId(
    __in  PHW_MAC_CONTEXT         HwMac
    )
{
    return HwMac->SelectedPhyId;
}

BOOLEAN
Hw11QueryHardwarePhyState(
    __in  PHW_MAC_CONTEXT         HwMac
    )
{
    return HwQueryHardwarePhyState(HwMac->Hw, HwMac->SelectedPhyId);
}

BOOLEAN
Hw11QuerySoftwarePhyState(
    __in  PHW_MAC_CONTEXT         HwMac
    )
{
    return HwQuerySoftwarePhyState(HwMac->Hw, HwMac->SelectedPhyId);
}

NDIS_STATUS
Hw11QueryInterruptModerationSettings(
    __in  PHW                     Hw,
    __out PNDIS_INTERRUPT_MODERATION_PARAMETERS   IntModParams
    )
{
    UNREFERENCED_PARAMETER(Hw);
    
    // Interrupt moderation not supported
    IntModParams->Flags = 0;
    IntModParams->InterruptModeration = NdisInterruptModerationNotSupported;

    return NDIS_STATUS_SUCCESS;
}



NDIS_STATUS
Hw11QueryLinkParameters(
    __in  PHW_MAC_CONTEXT         HwMac,
    __out PNDIS_LINK_PARAMETERS   LinkParams
    )
{
    //
    // Report the current link speed to the OS
    //
    LinkParams->MediaDuplexState = MediaDuplexStateHalf;
    LinkParams->XmitLinkSpeed = HwDataRateToLinkSpeed((UCHAR)HwMac->DefaultTXDataRate);
    LinkParams->RcvLinkSpeed = HwDataRateToLinkSpeed((UCHAR)HwMac->DefaultTXDataRate);
    LinkParams->PauseFunctions = NdisPauseFunctionsUnsupported;
    LinkParams->AutoNegotiationFlags = NDIS_LINK_STATE_DUPLEX_AUTO_NEGOTIATED;

    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Hw11QueryDataRateMappingTable(
    __in  PHW_MAC_CONTEXT         HwMac,
    __out PDOT11_DATA_RATE_MAPPING_TABLE  DataRateMappingTable,
    __in  ULONG                   TotalLength
    )
{
    UNREFERENCED_PARAMETER(HwMac);
    UNREFERENCED_PARAMETER(DataRateMappingTable);
    UNREFERENCED_PARAMETER(TotalLength);

    //
    // We don't support non-standard data rate.
    //
    return NDIS_STATUS_NOT_SUPPORTED;
}

NDIS_STATUS
Hw11QuerySupportedPowerLevels(
    __in  PHW_MAC_CONTEXT         HwMac,
    __out PDOT11_SUPPORTED_POWER_LEVELS   Dot11SupportedPowerLevels
    )
{
    USHORT    Index;

    // HwMac->Hw->PhyState.SupportedPowerLevels.uNumOfSupportedPowerLevels <= 8
    Dot11SupportedPowerLevels->uNumOfSupportedPowerLevels = HwMac->Hw->PhyState.SupportedPowerLevels.uNumOfSupportedPowerLevels;
    for( Index=0; Index < HwMac->Hw->PhyState.SupportedPowerLevels.uNumOfSupportedPowerLevels; Index++ )
        Dot11SupportedPowerLevels->uTxPowerLevelValues[Index] = HwMac->Hw->PhyState.SupportedPowerLevels.uTxPowerLevelValues[Index];

    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Hw11QuerySupportedRXAntenna(
    __in  PHW                     Hw,
    __in  ULONG                   MaxEntries,
    __out PDOT11_SUPPORTED_ANTENNA_LIST   Dot11SupportedAntennaList    
    )
{
    ULONG                       i;
    
    UNREFERENCED_PARAMETER(Hw);

    if (MaxEntries < HW11_MAX_RX_ANTENNA)
    {
        Dot11SupportedAntennaList->uNumOfEntries = 0;
        Dot11SupportedAntennaList->uTotalNumOfEntries = HW11_MAX_RX_ANTENNA;

        return NDIS_STATUS_BUFFER_OVERFLOW;
    }
    else
    {
        Dot11SupportedAntennaList->uTotalNumOfEntries = HW11_MAX_RX_ANTENNA;

        for(i = 0; i < HW11_MAX_RX_ANTENNA && i < MaxEntries;i++)
        {
            Dot11SupportedAntennaList->dot11SupportedAntenna[i].uAntennaListIndex = i+1;
            Dot11SupportedAntennaList->dot11SupportedAntenna[i].bSupportedAntenna = TRUE;
        }
        
        Dot11SupportedAntennaList->uNumOfEntries = HW11_MAX_RX_ANTENNA;
        
        return NDIS_STATUS_SUCCESS;
    }
}

NDIS_STATUS
Hw11QuerySupportedTXAntenna(
    __in  PHW                     Hw,
    __in  ULONG                   MaxEntries,
    __out PDOT11_SUPPORTED_ANTENNA_LIST   Dot11SupportedAntennaList
    )
{
    ULONG                       i;
    
    UNREFERENCED_PARAMETER(Hw);

    if (MaxEntries < HW11_MAX_TX_ANTENNA)
    {
        Dot11SupportedAntennaList->uNumOfEntries = 0;
        Dot11SupportedAntennaList->uTotalNumOfEntries = HW11_MAX_TX_ANTENNA;

        return NDIS_STATUS_BUFFER_OVERFLOW;
    }
    else
    {
        Dot11SupportedAntennaList->uTotalNumOfEntries = HW11_MAX_TX_ANTENNA;

        for(i = 0; i < HW11_MAX_TX_ANTENNA && i < MaxEntries;i++)
        {
            Dot11SupportedAntennaList->dot11SupportedAntenna[i].uAntennaListIndex = i+1;
            Dot11SupportedAntennaList->dot11SupportedAntenna[i].bSupportedAntenna = TRUE;
        }
        
        Dot11SupportedAntennaList->uNumOfEntries = HW11_MAX_TX_ANTENNA;
        
        return NDIS_STATUS_SUCCESS;
    }
}

NDIS_STATUS
Hw11QueryDiversitySelectionRX(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy,
    __in  ULONG                   MaxEntries,
    __out PDOT11_DIVERSITY_SELECTION_RX_LIST Dot11DiversitySelectionRXList
    )
{
    //
    // Determine the PHY that the user wants to query
    //
    if (SelectedPhy)
        return HwQueryDiversitySelectionRX(HwMac->Hw, 
                    HwMac->SelectedPhyId, 
                    MaxEntries, 
                    Dot11DiversitySelectionRXList
                    );
    else
        return HwQueryDiversitySelectionRX(HwMac->Hw,
                    HwMac->SelectedPhyId, 
                    MaxEntries, 
                    Dot11DiversitySelectionRXList
                    );
}


NDIS_STATUS
Hw11QueryRegDomainsSupportValue(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy,
    __in  ULONG                   MaxEntries,
    __out PDOT11_REG_DOMAINS_SUPPORT_VALUE    Dot11RegDomainsSupportValue
    )
{
    //
    // Determine the PHY that the user wants to query
    //
    if (SelectedPhy)
        return HwQueryRegDomainsSupportValue(HwMac->Hw, 
                    HwMac->SelectedPhyId, 
                    MaxEntries, 
                    Dot11RegDomainsSupportValue
                    );
    else
        return HwQueryRegDomainsSupportValue(HwMac->Hw,
                    HwMac->OperatingPhyId, 
                    MaxEntries, 
                    Dot11RegDomainsSupportValue
                    );
}

ULONG
Hw11QueryLookahead(
    __in  PHW_MAC_CONTEXT         HwMac
    )
{
    UNREFERENCED_PARAMETER(HwMac);

    // We always return the same lookahead size
    return (DOT11_MIN_PDU_SIZE - (sizeof(DOT11_DATA_SHORT_HEADER) + 12));
}

ULONG
Hw11QueryHardwareStatus(
    __in  PHW                     Hw    
    )
{
    //
    // We are currently only using the software status to report the hardware 
    // status. We can also look at the physical hardware status to report this
    //
    if (HW_TEST_ADAPTER_STATUS(Hw, (HW_ADAPTER_SURPRISE_REMOVED | HW_ADAPTER_HALTING 
            | HW_ADAPTER_IN_LOW_POWER)))
    {
        return NdisHardwareStatusNotReady;
    }
    else if (HW_TEST_ADAPTER_STATUS(Hw, (HW_ADAPTER_IN_RESET)))
    {
        return NdisHardwareStatusReset;
    }
    else
    {
        return NdisHardwareStatusReady;
    }
}

ULONG
Hw11QueryReceiveBufferSpace(
    __in  PHW                     Hw    
    )
{
    return HW11_MAX_FRAME_SIZE * Hw->RegInfo.NumRXBuffers;
}

ULONG
Hw11QueryTransmitBufferSpace(
    __in  PHW                     Hw    
    )
{
    return HW11_MAX_FRAME_SIZE * Hw->RegInfo.NumTXBuffers;
}

NDIS_STATUS
Hw11QueryVendorDescription(
    __in  PHW                     Hw,
    __out PVOID                   InformationBuffer,
    __in  ULONG                   InformationBufferLength,
    __out PULONG                  BytesWritten,
    __out PULONG                  BytesNeeded
    )
{
    UCHAR       VendorDesc[] = HW11_VENDOR_DESCRIPTION;

    UNREFERENCED_PARAMETER(Hw);
    
    *BytesNeeded = sizeof(VendorDesc);
    if (InformationBufferLength < *BytesNeeded)
    {
        return NDIS_STATUS_BUFFER_OVERFLOW;
    }

    NdisMoveMemory(InformationBuffer, VendorDesc, *BytesNeeded);
    *BytesWritten = *BytesNeeded;

    return NDIS_STATUS_SUCCESS;
}

ULONG
Hw11QueryVendorId(
    __in  PHW                     Hw    
     )
{
    UNREFERENCED_PARAMETER(Hw);
    return MP_VENDOR_ID;
}

NDIS_STATUS
Hw11QueryCurrentOptionalCapability(
    __in  PHW                     Hw,
    __in  PDOT11_CURRENT_OPTIONAL_CAPABILITY   Dot11CurrentOptionalCapability    
    )
{
    UNREFERENCED_PARAMETER(Hw);

    //
    // Support for nothing
    //
    Dot11CurrentOptionalCapability->uReserved = 0;
    Dot11CurrentOptionalCapability->bDot11CFPollable = FALSE;
    Dot11CurrentOptionalCapability->bDot11PCF = FALSE;
    Dot11CurrentOptionalCapability->bDot11PCFMPDUTransferToPC = FALSE;
    Dot11CurrentOptionalCapability->bStrictlyOrderedServiceClass = FALSE;
        
    return NDIS_STATUS_SUCCESS;
}

ULONG
Hw11QueryMaxMPDULength(
    __in  PHW_MAC_CONTEXT         HwMac
    )
{
    //
    // Always reporting the same list to the caller
    //
    UNREFERENCED_PARAMETER(HwMac);
    return HW11_MAX_FRAME_SIZE;
}

NDIS_STATUS
Hw11QueryMulticastList(
    __in  PHW_MAC_CONTEXT         HwMac,
    __out PVOID                   AddressBuffer,
    __in  ULONG                   AddressBufferLength,
    __out PULONG                  BytesWritten,
    __out PULONG                  BytesNeeded
    )
{
    *BytesNeeded = HwMac->MulticastAddressCount * DOT11_ADDRESS_SIZE;
    *BytesWritten = 0;

    if (AddressBufferLength < *BytesNeeded)
    {
        return NDIS_STATUS_BUFFER_OVERFLOW;
    }

    // Copy the addresses over        
    NdisMoveMemory(AddressBuffer, HwMac->MulticastAddressList, *BytesNeeded);
    *BytesWritten = *BytesNeeded;
       
    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Hw11QueryOperationModeCapability(
    __in  PHW                     Hw,
    __out PDOT11_OPERATION_MODE_CAPABILITY    Dot11OpModeCapability
    )
{
    Dot11OpModeCapability->uReserved = 0;
    Dot11OpModeCapability->uMajorVersion = MP_OPERATION_MODE_CAPABILITY_MAJOR_VERSION;
    Dot11OpModeCapability->uMinorVersion = MP_OPERATION_MODE_CAPABILITY_MINOR_VERSION;

    // We can buffer as many packets as OS sends.
    Dot11OpModeCapability->uNumOfTXBuffers = Hw->RegInfo.NumTXBuffers;
    Dot11OpModeCapability->uNumOfRXBuffers = Hw->RegInfo.NumRXBuffers;

    Dot11OpModeCapability->uOpModeCapability = DOT11_OPERATION_MODE_EXTENSIBLE_STATION 
        | DOT11_OPERATION_MODE_NETWORK_MONITOR
        | DOT11_OPERATION_MODE_EXTENSIBLE_AP;
        
    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Hw11QueryOptionalCapability(
    __in  PHW                     Hw,
    __out PDOT11_OPTIONAL_CAPABILITY  Dot11OptionalCapability
    )
{
    UNREFERENCED_PARAMETER(Hw);

    //
    // Support for nothing
    //
    Dot11OptionalCapability->uReserved = 0;
    Dot11OptionalCapability->bDot11PCF = FALSE;
    Dot11OptionalCapability->bDot11PCFMPDUTransferToPC = FALSE;
    Dot11OptionalCapability->bStrictlyOrderedServiceClass = FALSE;
        
    return NDIS_STATUS_SUCCESS;
}

PDOT11_MAC_ADDRESS
Hw11QueryHardwareAddress(
    __in  PHW                     Hw
    )
{
    return (PDOT11_MAC_ADDRESS)HalGetPermanentAddress(Hw->Hal);
}

ULONG
Hw11QueryRFUsage(
    __in  PHW                     Hw
    )
{
    UNREFERENCED_PARAMETER(Hw);

    //
    // NOTE: The hardware does not support this.
    //
    
    return 10;    // In percentage
}


NDIS_STATUS
Hw11QuerySupportedDataRatesValue(
    __in  PHW_MAC_CONTEXT         HwMac,
    __out PDOT11_SUPPORTED_DATA_RATES_VALUE_V2    Dot11SupportedDataRatesValue,
    __in  BOOLEAN                 SelectedPhy 
    )
{
    //
    // Determine the PHY that the user wants to query
    //
    if (SelectedPhy)
        return HwQuerySupportedDataRatesValue(HwMac->Hw, 
                    HwMac->SelectedPhyId, 
                    Dot11SupportedDataRatesValue
                    );
    else
        return HwQuerySupportedDataRatesValue(HwMac->Hw,
                    HwMac->OperatingPhyId, 
                    Dot11SupportedDataRatesValue
                    );
}

ULONG
Hw11QueryCCAModeSupported(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy
    )
{
    //
    // Determine the PHY that the user wants to query
    //
    if (SelectedPhy)
        return HwQueryCCAModeSupported(HwMac->Hw, 
                    HwMac->SelectedPhyId
                    );
    else
        return HwQueryCCAModeSupported(HwMac->Hw,
                    HwMac->OperatingPhyId
                    );
}

NDIS_STATUS
Hw11QueryCountryString(
    __in  PHW_MAC_CONTEXT         HwMac,
    __out PDOT11_COUNTRY_OR_REGION_STRING Dot11CountryString
    )
{
    UNREFERENCED_PARAMETER(HwMac);
    UNREFERENCED_PARAMETER(Dot11CountryString);
    return NDIS_STATUS_NOT_SUPPORTED;
}

ULONG
Hw11QueryCurrentCCAMode(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy
    )
{
    PNICPHYMIB                  phyMib;

    if (SelectedPhy)
        phyMib = HalGetPhyMIB(HwMac->Hw->Hal, HwMac->SelectedPhyId);
    else
        phyMib = HalGetPhyMIB(HwMac->Hw->Hal, HwMac->OperatingPhyId);

    if (phyMib->PhyType == dot11_phy_type_dsss ||
        phyMib->PhyType == dot11_phy_type_hrdsss ||
        phyMib->PhyType == dot11_phy_type_erp)
    {
        // NOTE: Hardcoded value is being used here
        return DOT11_CCA_MODE_CS_ONLY;
    }
    else
    {
        return 0;
    }
}

ULONG
Hw11QueryCurrentFrequency(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy
    )
{
    PHW_PHY_CONTEXT             phyContext;
    PNICPHYMIB                  phyMib;

    if (SelectedPhy)
    {
        phyContext = &HwMac->PhyContext[HwMac->SelectedPhyId];
        phyMib = HalGetPhyMIB(HwMac->Hw->Hal, HwMac->SelectedPhyId);
    }
    else
    {
        phyContext = &HwMac->PhyContext[HwMac->OperatingPhyId];
        phyMib = HalGetPhyMIB(HwMac->Hw->Hal, HwMac->OperatingPhyId);
    }
    
    if (phyMib->PhyType == dot11_phy_type_ofdm)
    {
        return phyContext->Channel;
    }

    return 0;
}

ULONG
Hw11QueryCurrentRegDomain(
    __in  PHW_MAC_CONTEXT         HwMac
    )
{
    return HwMac->CurrentRegDomain;
}

ULONG
Hw11QueryCurrentTXPowerLevel(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy
    )
{
    //
    // We dont throttle the TX power level per MAC context
    //
    if (SelectedPhy)
    {
        return HwQueryCurrentTXPowerLevel(HwMac->Hw, HwMac->SelectedPhyId);
    }
    else
    {
        return HwQueryCurrentTXPowerLevel(HwMac->Hw, HwMac->OperatingPhyId);
    }
}

DOT11_DIVERSITY_SUPPORT
Hw11QueryDiversitySupport(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy
    )
{
    if (SelectedPhy)
        return HwQueryDiversitySupport(HwMac->Hw, HwMac->SelectedPhyId);
    else
        return HwQueryDiversitySupport(HwMac->Hw, HwMac->OperatingPhyId);
}

ULONG
Hw11QueryEDThreshold(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy
    )
{
    if (SelectedPhy)
        return HwQueryEDThreshold(HwMac->Hw, HwMac->SelectedPhyId);
    else
        return HwQueryEDThreshold(HwMac->Hw, HwMac->OperatingPhyId);
}

BOOLEAN
Hw11QueryErpPbccOptionEnabled(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy
    )
{
    UNREFERENCED_PARAMETER(HwMac);
    UNREFERENCED_PARAMETER(SelectedPhy);

    //
    // Not implemented so its always false
    //
    return FALSE;
}

BOOLEAN
Hw11QueryErpPbccOptionImplemented(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy
    )
{
    UNREFERENCED_PARAMETER(HwMac);
    UNREFERENCED_PARAMETER(SelectedPhy);

    //
    // Not implemented
    //
    return FALSE;
}

ULONG
Hw11QueryFragmentationThreshold(
    __in  PHW_MAC_CONTEXT         HwMac
    )
{
    return HwMac->FragmentationThreshold;
}

ULONG
Hw11QueryFrequencyBandsSupported(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy
    )
{
    if (SelectedPhy)
        return HwQueryFrequencyBandsSupported(HwMac->Hw, HwMac->SelectedPhyId);
    else
        return HwQueryFrequencyBandsSupported(HwMac->Hw, HwMac->OperatingPhyId);
}

ULONG
Hw11QueryLongRetryLimit(
    __in  PHW_MAC_CONTEXT         HwMac
    )
{
    return HwMac->LongRetryLimit;
}

ULONG
Hw11QueryMaxReceiveLifetime(
    __in  PHW                     Hw
    )
{
    return Hw->MacState.MaxReceiveLifetime;
}

ULONG
Hw11QueryMaxTransmitMSDULifetime(
    __in  PHW                     Hw
    )
{
    return Hw->MacState.MaxTransmitMSDULifetime;
}

BOOLEAN
Hw11QueryMultiDomainCapabilityEnabled(
    __in  PHW_MAC_CONTEXT         HwMac
    )
{
    UNREFERENCED_PARAMETER(HwMac);

    //
    // Not implemented so not enabled
    //
    return FALSE;
}


BOOLEAN
Hw11QueryMultiDomainCapabilityImplemented(
    __in  PHW_MAC_CONTEXT         HwMac
    )
{
    return HwQueryMultiDomainCapabilityImplemented(HwMac->Hw);
}

ULONG
Hw11QueryRTSThreshold(
    __in  PHW_MAC_CONTEXT         HwMac
    )
{
    return HwMac->RTSThreshold;
}

ULONG
Hw11QueryShortRetryLimit(
    __in  PHW_MAC_CONTEXT         HwMac
    )
{
    return HwMac->ShortRetryLimit;
}


DOT11_TEMP_TYPE
Hw11QueryTempType(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SelectedPhy
    )
{
    if (SelectedPhy)
        return HwQueryTempType(HwMac->Hw, HwMac->SelectedPhyId);
    else
        return HwQueryTempType(HwMac->Hw, HwMac->OperatingPhyId);
}


ULONG
Hw11QueryPacketFilter(
    __in  PHW_MAC_CONTEXT         HwMac
    )
{
    return HwMac->PacketFilter;
}

DOT11_CIPHER_ALGORITHM
Hw11QueryUnicastCipher(
    __in  PHW_MAC_CONTEXT         HwMac
    )
{
    return HwMac->UnicastCipher;
}

DOT11_CIPHER_ALGORITHM
Hw11QueryMulticastCipher(
    __in  PHW_MAC_CONTEXT         HwMac
    )
{
    return HwMac->MulticastCipher;
}

DOT11_AUTH_ALGORITHM
Hw11QueryAuthentication(
    __in  PHW_MAC_CONTEXT         HwMac
    )
{
    return HwMac->AuthAlgorithm;
}

UCHAR
Hw11QueryDefaultKeyMask(
    __in  PHW_MAC_CONTEXT         HwMac
    )
{
    UCHAR                       validKeyMask = 0;

    // Since the NicKeyIndex can change, acquire the lock
    HW_ACQUIRE_HARDWARE_LOCK(HwMac->Hw, FALSE);
    validKeyMask = HwQueryDefaultKeyMask(HwMac);
    HW_RELEASE_HARDWARE_LOCK(HwMac->Hw, FALSE);

    return validKeyMask;
}

DOT11_BSS_TYPE
Hw11QueryCurrentBSSType(
    __in  PHW_MAC_CONTEXT         HwMac
    )
{
    return HwMac->BssType;
}


NDIS_STATUS
Hw11SetLookahead(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  ULONG                   Lookahead,
    __in  BOOLEAN                 fProgramHardware
    )
{
    UNREFERENCED_PARAMETER(HwMac);
    UNREFERENCED_PARAMETER(fProgramHardware);

    if (Lookahead > (DOT11_MIN_PDU_SIZE - (sizeof(DOT11_DATA_SHORT_HEADER) + 12)))
    {
        return NDIS_STATUS_NOT_SUPPORTED;
    }
    // Else this is the default lookahead that we have
    
    return NDIS_STATUS_SUCCESS;
    
}


NDIS_STATUS
Hw11SetInterruptModerationSettings(
    __in  PHW                     Hw,
    __in  PNDIS_INTERRUPT_MODERATION_PARAMETERS   IntModParams
    )
{
    UNREFERENCED_PARAMETER(Hw);
    UNREFERENCED_PARAMETER(IntModParams);
    
    // Interrupt moderation not supported
    return NDIS_STATUS_INVALID_DATA;
}

NDIS_STATUS
Hw11SetLinkParameters(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  PNDIS_LINK_PARAMETERS   LinkParams,
    __in  BOOLEAN                 fProgramHardware
    )
{
    PDOT11_RATE_SET             myRateSet;
    ULONG                       index;
    UCHAR                       dataRate;

    UNREFERENCED_PARAMETER(fProgramHardware);
    
    //
    // We dont program the hardware. So the fProgramHardware flag is not
    // relevant
    //
    if (LinkParams->RcvLinkSpeed != LinkParams->XmitLinkSpeed)
    {
        // For us the TX and RX rates must be the same
        return NDIS_STATUS_INVALID_DATA;
    }

    // If the rate specified by the user is valid, we are going to attempt to 
    // transmit at that rate (even if it may not be acceptable to the AP)
    dataRate = HwLinkSpeedToDataRate(LinkParams->XmitLinkSpeed);

    myRateSet = &HwMac->PhyContext[HwMac->OperatingPhyId].OperationalRateSet;

    // Is this a supported data rate?
    for (index = 0; index < myRateSet->uRateSetLength; index++)
    {
        if (myRateSet->ucRateSet[index] == dataRate)
        {
            HwMac->DefaultTXDataRate = myRateSet->ucRateSet[index];
            MpTrace(COMP_ASSOC, DBG_LOUD, ("TX Data rate: %d\n", myRateSet->ucRateSet[index]));
            
            //
            // Indicate NDIS_STATUS_LINK_STATE to inform the OS about the new 
            // link speed
            //
            HwIndicateLinkSpeed(HwMac, HwMac->DefaultTXDataRate);

            return NDIS_STATUS_SUCCESS;
        }
    }

    // Unsupported data rate
    return NDIS_STATUS_INVALID_DATA;
}

NDIS_STATUS
Hw11SetMulticastList(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  PVOID                   AddressBuffer,
    __in  ULONG                   AddressBufferLength,
    __in  BOOLEAN                 fProgramHardware
    )
{
    NDIS_STATUS                 ndisStatus;
    
    if(AddressBufferLength > 0)
    {
        if (AddressBufferLength > (HW11_MAX_MULTICAST_LIST_SIZE * DOT11_ADDRESS_SIZE))
        {
            MpTrace(COMP_OID, DBG_LOUD, ("Too many multicast addresses being set\n"));
            return NDIS_STATUS_MULTICAST_FULL;
        }

        //        
        // Save address list
        //
        HwMac->MulticastAddressCount = (UCHAR)(AddressBufferLength / DOT11_ADDRESS_SIZE);
        NdisMoveMemory(HwMac->MulticastAddressList, AddressBuffer, AddressBufferLength);

        if (fProgramHardware)
        {
            //
            // Set the multicast address on the NIC
            //
            if (HwMac->PacketFilter & (NDIS_PACKET_TYPE_ALL_MULTICAST |
        					    NDIS_PACKET_TYPE_802_11_ALL_MULTICAST_MGMT))
            {
                HwMac->AcceptAllMulticast = TRUE;
                ndisStatus = HwUpdateMulticastAddressList(HwMac->Hw, 
                                HwMac
                                );
                if (ndisStatus != NDIS_STATUS_SUCCESS)
                {
                    MpTrace(COMP_OID, DBG_LOUD,  ("Unable to set multicast address list\n"));
                    return ndisStatus;
                }
            }
            else if (HwMac->PacketFilter & (NDIS_PACKET_TYPE_MULTICAST |
    		                 NDIS_PACKET_TYPE_802_11_MULTICAST_MGMT))
            {
                HwMac->AcceptAllMulticast = FALSE;
                ndisStatus = HwUpdateMulticastAddressList(HwMac->Hw, 
                                HwMac
                                );
                if (ndisStatus != NDIS_STATUS_SUCCESS)
                {
                    MpTrace(COMP_OID, DBG_LOUD,  ("Unable to set multicast address list\n"));
                    return ndisStatus;
                }
            }
        }
    }
    else
    {
        HwMac->MulticastAddressCount = 0;
        // We dont program the hardware for this
    }
    return NDIS_STATUS_SUCCESS;                        
}


NDIS_STATUS
Hw11SetOperationalRateSet(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  PDOT11_RATE_SET         Dot11RateSet,
    __in  BOOLEAN                 SelectedPhy,
    __in  BOOLEAN                 fProgramHardware 
    )
{
    ULONG                       phyId;
    ULONG                       index;
    PHW_PHY_CONTEXT             phyContext;
    
    //
    // Determine the PHY that the user wants to query
    //
    if (SelectedPhy)
        phyId = HwMac->SelectedPhyId;
    else
        phyId = HwMac->OperatingPhyId;

    phyContext = &HwMac->PhyContext[phyId];
    
    //
    // Copy the rates into the phy context for this MAC
    //
    for (index = 0; index < Dot11RateSet->uRateSetLength; index++)
    {
        phyContext->OperationalRateSet.ucRateSet[index] = Dot11RateSet->ucRateSet[index];
    }
    phyContext->OperationalRateSet.uRateSetLength = Dot11RateSet->uRateSetLength;

    if (fProgramHardware)
    {
        //
        // Set the rate set on the hardware
        //
        return HwProgramPhy(HwMac->Hw, 
                    HwMac,
                    phyId,
                    phyContext,
                    NULL
                    );
    }

    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Hw11SetDefaultKeyId(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  ULONG                   KeyId,
    __in  BOOLEAN                 fProgramHardware
    )
{
    // Check for valid default key ID
    if (KeyId >= DOT11_MAX_NUM_DEFAULT_KEY)
    {
        return NDIS_STATUS_INVALID_DATA;
    }

    // Save the default key
    HwMac->DefaultKeyIndex = (UCHAR)KeyId;

    if (fProgramHardware)
    {
        HwSetDefaultKeyId(HwMac->Hw, HwMac, KeyId);
    }

    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Hw11SetSelectedPhyId(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  ULONG                   PhyId,
    __in  BOOLEAN                 fProgramHardware
    )
{
    PDOT11_SUPPORTED_PHY_TYPES supportedPhyTypes = HalGetSupportedPhyTypes(HwMac->Hw->Hal);

    UNREFERENCED_PARAMETER(fProgramHardware);

    if (PhyId >= supportedPhyTypes->uNumOfEntries)
    {
        return NDIS_STATUS_INVALID_DATA;
    }
    else
    {
        HwMac->SelectedPhyId = PhyId;

        //
        // We dont need to do anything on the NIC when the Phy Id is selected
        //
    }

    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Hw11SetSafeModeOption(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 SafeModeEnabled,
    __in  BOOLEAN                 fProgramHardware
    )
{
    if (SafeModeEnabled)
    {
        //
        // We would program the hardware to not do any encryption (both unicast and multicast ciphers)
        //
        Hw11SetCipher(HwMac, TRUE, DOT11_CIPHER_ALGO_NONE, fProgramHardware);
        Hw11SetCipher(HwMac, FALSE, DOT11_CIPHER_ALGO_NONE, fProgramHardware);
    }
    else
    {
        //
        // Cipher would have already been set when safe mode is configured
        // (enabled or disabled)
        //
    }

    // Program the hardware
    HwMac->Hw->MacState.SafeModeEnabled = SafeModeEnabled;
    HalEnableSafeMode(HwMac->Hw->Hal, SafeModeEnabled);

    return NDIS_STATUS_SUCCESS;
}


NDIS_STATUS 
HwOidChannelSwitchComplete(
    __in  PHW                     Hw,
    __in  PHW_MAC_CONTEXT         MacContext,
    __in  PVOID                   Data
    )
{
    VNIC11_GENERIC_CALLBACK_FUNC    completionHandler;
    MpTrace(COMP_SCAN, DBG_LOUD, ("Oid channel switched\n"));

    UNREFERENCED_PARAMETER(Hw);
    
    completionHandler = MacContext->OidChannelSwitchCallback;
    MacContext->OidChannelSwitchCallback = NULL;

    MPASSERT(completionHandler != NULL);
    
    // Invoke the completion handler
    completionHandler(MacContext->VNic, MacContext->ChannelSwitchContext, Data);

    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Hw11SetChannel(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  ULONG                   ChannelOrFrequency,
    __in  ULONG                   PhyId,
    __in  BOOLEAN                 SwitchPhy,
    __in  VNIC11_GENERIC_CALLBACK_FUNC    CompletionHandler,
    __in  PVOID                   ChannelSwitchContext
    )
{
    PHW_PHY_CONTEXT             phyContext;
    NDIS_STATUS                 ndisStatus;

    MPASSERT(HwMac->OidChannelSwitchCallback == NULL);
    
    //
    // Validate that this channel set is OK
    //
    ndisStatus = HwValidateChannel(HwMac->Hw, PhyId, (UCHAR)ChannelOrFrequency);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        MpTrace(COMP_MISC, DBG_SERIOUS, ("Invalid phy(%d) & channel/frequency(%d) set requested\n", 
            PhyId, ChannelOrFrequency));
        // Maintain asynchronous semantics for this API even in case of failure
        CompletionHandler(HwMac->VNic, ChannelSwitchContext, &ndisStatus);
        ndisStatus = NDIS_STATUS_PENDING;
        return ndisStatus;
    }

    phyContext = &HwMac->PhyContext[PhyId];

    phyContext->Channel = (UCHAR)ChannelOrFrequency;

    // If this is the current PHY programmed on the hardware or the caller has requested a phy set, set it
    if ((PhyId == HwMac->OperatingPhyId) || (SwitchPhy))
    {
        HwMac->OidChannelSwitchCallback = CompletionHandler;
        HwMac->ChannelSwitchContext = ChannelSwitchContext;
        
        return HwProgramPhy(HwMac->Hw, 
                    HwMac, 
                    PhyId, 
                    phyContext, 
                    HwOidChannelSwitchComplete
                    );
    }
    else
    {
        // Maintain asynchronous semantics for this API even in case of failure
        CompletionHandler(HwMac->VNic, ChannelSwitchContext, &ndisStatus);
        // This really translates to ndisStatus = NDIS_STATUS_PENDING;
    }
    
    return NDIS_STATUS_SUCCESS;
}


NDIS_STATUS
Hw11SetOperatingPhyId(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  ULONG                   PhyId,
    __in  BOOLEAN                 fProgramHardware
    )
{
    HwMac->OperatingPhyId = PhyId;

    if (fProgramHardware)
    {
        return HwProgramPhy(HwMac->Hw, 
                    HwMac, 
                    PhyId, 
                    &HwMac->PhyContext[PhyId], 
                    NULL
                    );
    }

    return NDIS_STATUS_SUCCESS;
}



VOID
Hw11DeleteNonPersistentKey(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 fProgramHardware
    )
{
    PHW_KEY_ENTRY               keyEntry;
    PHW_PEER_NODE               peerNode;
    UCHAR                       index, defaultIndex;

    //
    // Go through the key table, invalidate any key which is not persistent. 
    //
    for (index = 0; index < HW11_KEY_TABLE_SIZE; index++)
    {
        keyEntry = &HwMac->KeyTable[index];

        if (keyEntry->Key.Valid && !keyEntry->Key.Persistent)
        {
            //
            // Was valid/non-persistent key, remove it
            //
            HwDeleteKeyFromContext(HwMac, index, fProgramHardware);
        }
    }

    //
    // Invalidate any per-STA key which is not persistent. 
    //
    for (index = 0; index < HW11_PEER_TABLE_SIZE; index++)
    {
        peerNode = &HwMac->PeerTable[index];

        if (peerNode->Valid)
        {
            for (defaultIndex = 0; defaultIndex < DOT11_MAX_NUM_DEFAULT_KEY; defaultIndex++)
            {
                keyEntry = &peerNode->PrivateKeyTable[defaultIndex];
                
                if (keyEntry->Key.Valid && !keyEntry->Key.Persistent)
                {
                    // We dont have to delete it from the hardware (that is done
                    // above). Here we just invalidate it
                    NdisZeroMemory(&(keyEntry->Key), sizeof(NICKEY));
                }
            }
        }
    }
}

VOID
Hw11DeleteNonPersistentMappingKey(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in_bcount(DOT11_ADDRESS_SIZE)  DOT11_MAC_ADDRESS       MacAddr,
    __in  BOOLEAN                 fProgramHardware
    )
{
    PHW_KEY_ENTRY               keyEntry;
    PHW_PEER_NODE               peerNode;
    UCHAR                       index, defaultIndex;

    //
    // Go through the key mapping table, invalidate any key which is not persistent. 
    //
    for (index = DOT11_MAX_NUM_DEFAULT_KEY; index < HW11_KEY_TABLE_SIZE; index++)
    {
        keyEntry = &HwMac->KeyTable[index];

        if (keyEntry->Key.Valid && !keyEntry->Key.Persistent)
        {
            if (MP_COMPARE_MAC_ADDRESS(MacAddr, keyEntry->Key.MacAddr))
            {
                //
                // Was valid/non-persistent key, remove it
                //
                HwDeleteKeyFromContext(HwMac, index, fProgramHardware);
            }
        }
    }

    //
    // Invalidate any per-STA key which is not persistent. 
    //
    for (index = 0; index < HW11_PEER_TABLE_SIZE; index++)
    {
        peerNode = &HwMac->PeerTable[index];

        if (peerNode->Valid)
        {
            if (MP_COMPARE_MAC_ADDRESS(MacAddr, peerNode->PeerMac))
            {        
                for (defaultIndex = 0; defaultIndex < DOT11_MAX_NUM_DEFAULT_KEY; defaultIndex++)
                {
                    keyEntry = &peerNode->PrivateKeyTable[defaultIndex];
                    
                    if (keyEntry->Key.Valid && !keyEntry->Key.Persistent)
                    {
                        // We dont have to delete it from the hardware (that is done
                        // above). Here we just invalidate it
                        NdisZeroMemory(&(keyEntry->Key), sizeof(NICKEY));
                    }
                }
            }
        }
    }
}

NDIS_STATUS
Hw11SetPacketFilter(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  ULONG                   PacketFilter,
    __in  BOOLEAN                 fProgramHardware
    )
{
    NDIS_STATUS                 ndisStatus = NDIS_STATUS_SUCCESS;
    
    do
    {
        HwMac->PacketFilter = PacketFilter;

        if (fProgramHardware)
        {
            ndisStatus = HwUpdatePacketFilter(HwMac->Hw, HwMac);
            if (ndisStatus != NDIS_STATUS_SUCCESS)
            {
                MpTrace(COMP_OID, DBG_LOUD,  ("Unable to set packet filter to %d\n", PacketFilter));
                break;
            }

            // Set the multicast address
            if (PacketFilter & (NDIS_PACKET_TYPE_ALL_MULTICAST |
        					    NDIS_PACKET_TYPE_802_11_ALL_MULTICAST_MGMT))
            {
                HwMac->AcceptAllMulticast = TRUE;
                ndisStatus = HwUpdateMulticastAddressList(HwMac->Hw, 
                                HwMac
                                );
                if (ndisStatus != NDIS_STATUS_SUCCESS)
                {
                    MpTrace(COMP_OID, DBG_LOUD,  ("Unable to set multicast address list\n"));
                    break;
                }
            }
            else if (PacketFilter & (NDIS_PACKET_TYPE_MULTICAST |
			                 NDIS_PACKET_TYPE_802_11_MULTICAST_MGMT))
            {
                HwMac->AcceptAllMulticast = FALSE;
                ndisStatus = HwUpdateMulticastAddressList(HwMac->Hw, 
                                HwMac
                                );
                if (ndisStatus != NDIS_STATUS_SUCCESS)
                {
                    MpTrace(COMP_OID, DBG_LOUD,  ("Unable to set multicast address list\n"));
                    break;
                }
            }
            
        }
    } while (FALSE);
    
    return ndisStatus;
}

NDIS_STATUS
Hw11SetATIMWindow(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  ULONG                   Value,
    __in  BOOLEAN                 fProgramHardware
    )
{
    HwMac->AtimWindow = HW_LIMIT_BEACON_PERIOD(Value);

    //
    // If we are supposed to program the hardware, let that happen
    //
    if (fProgramHardware)
        return HwSetATIMWindow(HwMac->Hw, Value);

    return NDIS_STATUS_SUCCESS;
}

VOID
Hw11SetCipher(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 IsUnicast,
    __in  DOT11_CIPHER_ALGORITHM  AlgoId,
    __in  BOOLEAN                 fProgramHardware
    )
{
    /*** Valid combinations of multicast/unicast cipher algorithms

    Multicast\Unicast       WEP      WEP40      WEP104      TKIP     CCMP      NONE
        WEP                  Y
        WEP40                          Y*                     Y        Y
        WEP104                                    Y*          Y        Y
        TKIP                                                  Y        Y
        CCMP                                                           Y
        NONE                                                                    Y
 
      Y*: Not currently used by Microsoft 802.11 Security module but can be supported by vendors 
          if they want to connect to WPA AP that does not support pairwise keys.

    ***/

    //
    // Update the encryption algorithm
    //
    if (IsUnicast)
    {
        //
        // When set unicast cipher, also clear all keys. 
        //
        HwDeleteAllKeysFromContext(HwMac, fProgramHardware);
        HwMac->UnicastCipher = AlgoId;

        if (fProgramHardware)
        {
            HwUpdateUnicastCipher(HwMac->Hw, HwMac);
        }
    }
    else
    {
        //
        // Save the multicast cipher in our software state
        //
        HwMac->MulticastCipher = AlgoId;
    }
}

UCHAR
Hw11SelectTXDataRate(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  PDOT11_RATE_SET         PeerRateSet,
    __in  ULONG                   LinkQuality,
    __in  BOOLEAN                 fProgramHardware
    )
{
    UNREFERENCED_PARAMETER(fProgramHardware);
    return (UCHAR)HwSelectTXDataRate(HwMac, PeerRateSet, LinkQuality);
}

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
    )
{
    //
    // Note that both this function and other functions accessing key table are called
    // as a result of OID set. As such, no locking is needed since NDIS serializes
    // the OID set operation. If this premise is no longer valid, locking will
    // be needed.
    //
    if (KeyID >= DOT11_MAX_NUM_DEFAULT_KEY)
        return NDIS_STATUS_INVALID_DATA;

    if (HwMac->BssType == dot11_BSS_type_independent && 
            (MacAddr[0] != 0 || MacAddr[1] != 0 || MacAddr[2] != 0 || MacAddr[3] != 0 ||
                MacAddr[4] != 0 || MacAddr[5] != 0)
       )
    {
        //
        // For Adhoc we store per-STA default keys in the peer node
        //
        MPASSERT(DOT11_IS_UNICAST(MacAddr));
        return HwSetSoftwareDefaultKeyInContext(HwMac, 
                    MacAddr,
                    (UCHAR)KeyID,
                    Persistent,
                    AlgoId,
                    KeyLength,
                    KeyValue
                    );
    }
    else
    {
        //
        // Save the new default key in the key table. 
        //
        return HwSetDefaultKeyInContext(HwMac,  
                    MacAddr,
                    (UCHAR)KeyID, 
                    Persistent, 
                    AlgoId, 
                    KeyLength, 
                    KeyValue,
                    fProgramHardware
                    );
    }
}

NDIS_STATUS
Hw11SetKeyMappingKey(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in_bcount(DOT11_ADDRESS_SIZE)  DOT11_MAC_ADDRESS       MacAddr,
    __in  DOT11_DIRECTION         Direction,
    __in  BOOLEAN                 Persistent,
    __in  DOT11_CIPHER_ALGORITHM  AlgoId,
    __in  ULONG                   KeyLength,
    __in_bcount(KeyLength)  PUCHAR                  KeyValue,
    __in  BOOLEAN                 fProgramHardware
    )
{
    return HwSetKeyMappingKeyInContext(HwMac,
                MacAddr,
                Direction,
                Persistent,
                AlgoId,
                KeyLength,
                KeyValue,
                fProgramHardware
                );
}

NDIS_STATUS
Hw11SetOperationMode(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  PDOT11_CURRENT_OPERATION_MODE Dot11CurrentOperationMode,
    __in  BOOLEAN                 fProgramHardware
    )
{
    NDIS_STATUS                 ndisStatus = NDIS_STATUS_SUCCESS;
    
    //
    // Save the value locally
    //
    HwMac->CurrentOpMode = Dot11CurrentOperationMode->uCurrentOpMode;
    HwMac->Hw->MacState.NetmonModeEnabled = FALSE;

    if (fProgramHardware)
    {
        ndisStatus = HwUpdateOperationMode(HwMac->Hw, HwMac);

        if (Dot11CurrentOperationMode->uCurrentOpMode == DOT11_OPERATION_MODE_NETWORK_MONITOR)
        {
            //
            // Turn off encryption in the hardware
            //
            HwMac->UnicastCipher = DOT11_CIPHER_ALGO_NONE;
            HwUpdateUnicastCipher(HwMac->Hw, HwMac);
        }
    }
    
    return ndisStatus;
}

NDIS_STATUS
Hw11SetCurrentBSSType(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  DOT11_BSS_TYPE          Type,
    __in  BOOLEAN                 fProgramHardware
    )
{
    //
    // Save the value locally
    //
    HwMac->BssType = Type;

    if (fProgramHardware)
    {
        return HwUpdateBSSType(HwMac->Hw, HwMac);
    }
    
    return NDIS_STATUS_SUCCESS;
}


NDIS_STATUS
Hw11SetPowerMgmtMode(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  PDOT11_POWER_MGMT_MODE  PMMode,
    __in  BOOLEAN                 fProgramHardware
    )
{
    NdisMoveMemory(&HwMac->PowerMgmtMode, PMMode, sizeof(DOT11_POWER_MGMT_MODE));

    if (fProgramHardware)
    {
        return HwSetPowerMgmtMode(HwMac->Hw, HwMac, PMMode);
    }

    return NDIS_STATUS_SUCCESS;
}

VOID
Hw11SetCTSToSelfOption(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 Enable,
    __in  BOOLEAN                 fProgramHardware
    )
{
    UNREFERENCED_PARAMETER(fProgramHardware);

    //
    // We just set it here. The send routines would pick it up
    // when sending packets for this node
    //
    HwMac->CTSToSelfEnabled = Enable;    
}

NDIS_STATUS
Hw11SetDesiredPhyIdList(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  PULONG                  PhyIDList,
    __in  ULONG                   PhyIDCount,
    __in  BOOLEAN                 fProgramHardware
    )
{
    ULONG                       phyId;
    
    if (PhyIDCount < 1)
        return NDIS_STATUS_INVALID_DATA;

    if (PhyIDList[0] == DOT11_PHY_ID_ANY)
        return NDIS_STATUS_SUCCESS;

    phyId = PhyIDList[0];

    if (HwMac->CurrentOpMode == DOT11_OPERATION_MODE_NETWORK_MONITOR)
    {
        //
        // In netmon mode, we should be active
        //
        MPASSERT(fProgramHardware == TRUE);

        //
        // Immediately switch to the first PHY in the list
        //
        fProgramHardware = TRUE;
    }
    else
    {
        //
        // Automatically set the PHY to the first PHY in the list. This
        // can change later
        //
        if (phyId == HwMac->OperatingPhyId)
        {
            //
            // Already set, dont need to set again
            //
            fProgramHardware = FALSE;
        }
    }

    HwMac->OperatingPhyId = phyId;
    
    if (fProgramHardware)
    {
        //
        // Set the operating phy ID on the hardware
        //
        return HwProgramPhy(HwMac->Hw, 
                    HwMac, 
                    phyId, 
                    &HwMac->PhyContext[phyId], 
                    NULL
                    );
    }

    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Hw11SetAuthentication(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  DOT11_AUTH_ALGORITHM    AlgoId,
    __in  BOOLEAN                 fProgramHardware
    )
{
    UNREFERENCED_PARAMETER(fProgramHardware);

    HwMac->AuthAlgorithm = AlgoId;
    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Hw11SetNicPowerState(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 PowerState,
    __in  BOOLEAN                 SelectedPhy,
    __in  BOOLEAN                 fProgramHardware
    )
{
    UNREFERENCED_PARAMETER(fProgramHardware);
    
    // When this OID is submitted we have to do it immediately
    
    //
    // Determine the PHY that the user wants to query
    //
    if (SelectedPhy)
        return HwSetNicPowerState(HwMac->Hw, HwMac->SelectedPhyId, PowerState);
    else
        return HwSetNicPowerState(HwMac->Hw, HwMac->OperatingPhyId, PowerState);
}

NDIS_STATUS
Hw11SetBeaconPeriod(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  ULONG                   BeaconPeriod,
    __in  BOOLEAN                 fProgramHardware
    )
{
    UNREFERENCED_PARAMETER(fProgramHardware);
    
    // Update our local value
    HwMac->BeaconPeriod = BeaconPeriod;

    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Hw11SetFragmentationThreshold(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  ULONG                   FragmentationThreshold,
    __in  BOOLEAN                 fProgramHardware
    )
{
    if(FragmentationThreshold > 2346)
        HwMac->FragmentationThreshold = 2346;
    else if(FragmentationThreshold < 256)
        HwMac->FragmentationThreshold = 256;
    else    
        HwMac->FragmentationThreshold = FragmentationThreshold;

    // We dont support fragmentation, so we dont 
    UNREFERENCED_PARAMETER(fProgramHardware);
    
    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Hw11SetMultiDomainCapabilityEnabled(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 MultiDomainCapabilityEnabled,
    __in  BOOLEAN                 fProgramHardware
    )
{
    UNREFERENCED_PARAMETER(HwMac);
    UNREFERENCED_PARAMETER(MultiDomainCapabilityEnabled);
    UNREFERENCED_PARAMETER(fProgramHardware);

    //
    // Multidomain capability is not supported
    //
    return NDIS_STATUS_NOT_SUPPORTED;
}

NDIS_STATUS
Hw11SetRTSThreshold(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  ULONG                   RTSThreshold,
    __in  BOOLEAN                 fProgramHardware
    )
{
    UNREFERENCED_PARAMETER(fProgramHardware);
    
    // Update our local value
    if(RTSThreshold > 2347)
        HwMac->RTSThreshold = 2347;
    else
        HwMac->RTSThreshold = RTSThreshold;

    return NDIS_STATUS_SUCCESS;
}   


NDIS_STATUS
Hw11CanTransitionPower(
    __in  PHW                     Hw,
    __in  NDIS_DEVICE_POWER_STATE NewDevicePowerState
    )
{
    //
    // Transition is always possible. If ever a need occurs for the driver
    // to veto a power transtion, we will do it here.
    //
    UNREFERENCED_PARAMETER(Hw);
    UNREFERENCED_PARAMETER(NewDevicePowerState);
    
    return NDIS_STATUS_SUCCESS;
}

// Called at device IRQL (via SynchronizeWithInterrupt)
VOID
HwSetHardwarePowerState(
    __in  PHW                    Hw
    )
{
    NDIS_DEVICE_POWER_STATE    devicePowerState = Hw->NextDevicePowerState;

    //
    // When coming out of low power, we set the bit before resuming the hardware.
    // When going to low power we set the bit after resuming the hardware. This is
    // because interrupts can be enabled (or disabled) before the call returns and
    // HWIsr may reject it if it thinks the hardware is still in low power
    //
    if (devicePowerState == NdisDeviceStateD0)
    {
        HW_CLEAR_ADAPTER_STATUS(Hw, HW_ADAPTER_IN_LOW_POWER);
    }
    MpTrace(COMP_POWER, DBG_LOUD,  ("Setting device to power state D%d\n", devicePowerState-1));
    HalSetDevicePowerState(Hw->Hal, devicePowerState);

    // If going to low power, set the low power bit
    if (devicePowerState != NdisDeviceStateD0)
    {
        HW_SET_ADAPTER_STATUS(Hw, HW_ADAPTER_IN_LOW_POWER);
    }
}

NDIS_STATUS
Hw11SetPower(
    __in  PHW                     Hw,
    __in  NDIS_DEVICE_POWER_STATE NewDevicePowerState
    )
{
    HW_HAL_RESET_PARAMETERS     resetParams;
    ULONG                       size;
    NDIS_STATUS                 ndisStatus = NDIS_STATUS_SUCCESS;
    UCHAR                       buffer[HW11_PCI_CONFIG_BUFFER_LENGTH];
    PPCI_COMMON_CONFIG          pciConfig = (PPCI_COMMON_CONFIG) buffer;

    //
    // Hardware power state change must be synchronized with Interrupt
    //
    Hw->NextDevicePowerState = NewDevicePowerState;

    if (NewDevicePowerState == NdisDeviceStateD0)
    {
        //
        // Some BIOS modify the PCI configuration space on resume.
        // We go read that again
        //
        do
        {
            // Load the PCI config information into our local buffer
            size = NdisMGetBusData(Hw->MiniportAdapterHandle,
                        PCI_WHICHSPACE_CONFIG,
                        FIELD_OFFSET(PCI_COMMON_CONFIG,VendorID),
                        pciConfig,
                        HW11_PCI_CONFIG_BUFFER_LENGTH
                        );
            
            if (size != HW11_PCI_CONFIG_BUFFER_LENGTH) 
            {
                MpTrace(COMP_INIT_PNP,
                        DBG_SERIOUS,
                        ("NdisReadPciSlotInformation failed. Number of bytes of PCI config info returned is %d\n", size));
                ndisStatus = NDIS_STATUS_ADAPTER_NOT_FOUND;
                break;
            }

            
            ndisStatus = HalParsePciConfiguration(Hw->Hal, buffer, size);    
            if (ndisStatus != NDIS_STATUS_SUCCESS) 
            {
                MpTrace(COMP_INIT_PNP, 
                        DBG_SERIOUS, 
                        ("HalParsePciConfiguration failed. Status = 0x%08x\n", ndisStatus));
            
                break;                
            }
        }while (FALSE);
    }
    
    NdisMSynchronizeWithInterruptEx(
        Hw->InterruptHandle,
        0,
        (MINIPORT_SYNCHRONIZE_INTERRUPT_HANDLER)HwSetHardwarePowerState,
        (PVOID)Hw
        );

    if (NewDevicePowerState == NdisDeviceStateD0)
    {
        //
        // Do an internal reset to start fresh
        //
        NdisZeroMemory(&resetParams, sizeof(HW_HAL_RESET_PARAMETERS));
        resetParams.FullReset = TRUE;
        HwResetHAL(Hw, &resetParams, FALSE);
    }

    return NDIS_STATUS_SUCCESS;
}


