/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Hw_Nic.c

Abstract:
    HW layer NIC specific function
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/

#include "hw_pcomp.h"
#include <stdio.h>
#include "wdf.h"


#pragma warning(disable:4200)  // nameless struct/union
#pragma warning(disable:4201)  // nameless struct/union
#pragma warning(disable:4214)  // bit field types other than int
#pragma warning(disable:4748)  

#include "hw_main.h"
#include "hw_nic.h"
#include "hw_80211.h"
#include "hw_func.h"
#include "8187_gen.h"
#include "bus_intf.h"
#include "mp_util.h"
#include "mp_send.h"


#pragma warning(default:4200)
#pragma warning(default:4201)
#pragma warning(default:4214)


#if DOT11_TRACE_ENABLED
    #include "Hw_Nic.tmh"
#endif


BOOLEAN
Hw11WEP104Implemented(
                     __in  PNIC Nic
                     )
{
    UNREFERENCED_PARAMETER(Nic);
    return TRUE;
}

BOOLEAN
Hw11WEP40Implemented(
                    __in  PNIC Nic
                    )
{
    UNREFERENCED_PARAMETER(Nic);
    return TRUE;
}

BOOLEAN
Hw11TKIPImplemented(
                   __in  PNIC Nic
                   )
{
    return(BOOLEAN)(Nic->CardInfo.HardwareID != HW_ID_8180);
}

BOOLEAN
Hw11CCMPImplemented(
                   __in  PNIC Nic
                   )
{
    return(BOOLEAN)(Nic->CardInfo.HardwareID != HW_ID_8180);
}



// Combined a/b/g channels. 11a channels 
CHANNEL_LIST ChannelListAB[] = {
    {{1,2,3,4,5,6,7,8,9,10,11,36,40,44,48,52,56,60,64}, 19},		//default
    {{1,2,3,4,5,6,7,8,9,10,11,36,40,44,48,52,56,60,64},19},		//FCC
    {{1,2,3,4,5,6,7,8,9,10,11,36,40,44,48,52,56,60,64},19}, 		//DOC, FCC
    {{1,2,3,4,5,6,7,8,9,10,11,12,13,36,40,44,48,52,56,60,64},21},	//ETSI										//ETSI
    {{1,2,3,4,5,6,7,8,9,10,11,12,13,36,40,44,48,52,56,60,64},21},	//Spain, ETSI
    {{1,2,3,4,5,6,7,8,9,10,11,12,13,36,40,44,48,52,56,60,64},21},	//France, ETSI
    {{14,36,40,44,48,52,56,60,64},9},							   //mkk
    {{1,2,3,4,5,6,7,8,9,10,11,12,13,14, 36,40,44,48,52,56,60,64},22}, //mkk1 , also TELE
    {{1,2,3,4,5,6,7,8,9,10,11,12,13,36,40,44,48,52,56,60,64},21}	//Israel
};

// b/g channels only.
CHANNEL_LIST ChannelListB[] = {
    {{1,2,3,4,5,6,7,8,9,10,11}, 11},				//default
    {{1,2,3,4,5,6,7,8,9,10,11}, 11},				//FCC
    {{1,2,3,4,5,6,7,8,9,10,11}, 11},				//DOC, FCC
    {{1,2,3,4,5,6,7,8,9,10,11,12,13}, 13},			//ETSI
    {{1,2,3,4,5,6,7,8,9,10,11,12,13}, 13},			//SPAIN
    {{1,2,3,4,5,6,7,8,9,10,11,12,13}, 13},			//France
    {{14},1},									//MKK
    {{1,2,3,4,5,6,7,8,9,10,11,12,13,14}, 14},		//MKK1, tele
    {{1,2,3,4,5,6,7,8,9,10,11,12,13},13}			//Israel
};

#define HW_TRANSITION_SCAN_STATE(_Scan_State)    \
    (_Scan_State = ((_Scan_State + 1) % ScanStateMaxState))

VOID
InitializeLED(
    __in PNIC         Nic
    )
{
    WDF_TIMER_CONFIG       timerConfig;
    WDF_OBJECT_ATTRIBUTES   attributes;
    PTIMER_NIC_CONTEXT      nicTimerNicContext;    
    NTSTATUS                ntStatus;
    
    WDF_TIMER_CONFIG_INIT(&timerConfig, LedControlCallback8187);
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(
                                   &attributes,
                                   TIMER_NIC_CONTEXT
                                   );
    attributes.ParentObject = Mp11GetWdfDevice( Nic->Adapter );
    ntStatus = WdfTimerCreate(
                             &timerConfig,
                             &attributes,
                             &Nic->LedInfo.LedControlTimer) ;    // Output handle

    if (!NT_SUCCESS(ntStatus)) {
        MpTrace(COMP_INIT_PNP, DBG_SERIOUS, ("Failed to create LedControlTimer.\n"));
        return ;
    }
    nicTimerNicContext = GetTimerContext(Nic->LedInfo.LedControlTimer);
    nicTimerNicContext->Nic = Nic;        
    
    Nic->LedInfo.LedCurrentState = LED_OFF;
    Nic->LedInfo.bLedBlinkInProgress = FALSE;
    Nic->LedInfo.BlinkTimes = 0;
    Nic->LedInfo.BlinkingLedState = LED_OFF;

}



void 
HwInitializeVariable
(
                    __in  PNIC Nic)
{
    USHORT    Index;

    Nic->CardInfo.MWIEnable = FALSE;
    Nic->DupCurrentInfo.ShortRetryLimit = 7;
    Nic->DupCurrentInfo.LongRetryLimit = 7;
    Nic->DupCurrentInfo.EarlyRxThreshold = 7;
    Nic->RxInfo.NextDupeCacheIndexData = 0;
 
    Nic->TxInfo.TransmitConfig = (7l << TCR_MXDMA_OFFSET) |                        // TCR_MXDMA_OFFSET
                                 (Nic->DupCurrentInfo.ShortRetryLimit << 8) |    // Short retry limit
                                 (Nic->DupCurrentInfo.LongRetryLimit) |          // Long retry limit
                                 (0 ? 0x00060000 : 0) |                           // Continuous Tx
                                 (0 ? TCR_SAT : 0);                               // Rate dependent ACK


    //
    // Set to receive management packets initially
    //
    Nic->RxInfo.ReceiveFilter = NDIS_PACKET_TYPE_802_11_DIRECTED_MGMT | 
                                NDIS_PACKET_TYPE_802_11_MULTICAST_MGMT |
                                NDIS_PACKET_TYPE_802_11_BROADCAST_MGMT;

    Nic->RxInfo.ReceiveConfig =   Nic->RfInfo.CSMethod |RCR_ENMARP | 
                                  RCR_AMF | RCR_ADF |             //accept management/data
                                  RCR_ACF |                       //accept control frame for 8187 SW AP needs PS-poll, 
                                  RCR_AB | RCR_AM | RCR_APM |     //accept BC/MC/UC
                                  RCR_AICV | /*RCR_ACRC32 |*/         //accept ICV/CRC error packet
                                  ((USHORT)0x07<<RCR_FIFO_OFFSET) |
                                  (Nic->DupCurrentInfo.EarlyRxThreshold<<RCR_FIFO_OFFSET) |
                                  (7<<RCR_MXDMA_OFFSET) |
                                  (Nic->DupCurrentInfo.EarlyRxThreshold == 7 ? RCR_OnlyErlPkt:0);


    if ( IS_HARDWARE_TYPE_8185(Nic) || IS_HARDWARE_TYPE_8187(Nic) ) {
        Nic->CardInfo.InitialGain = 2;
    }    

    //
    // management info
    //
    RtlCopyMemory((PCHAR)Nic->ManagementInfo.SSID, "Realtek AP", sizeof("Realtek AP"));
    FillOctetString( Nic->ManagementInfo.SupportedRates, Nic->ManagementInfo.SupportedRatesBuf, 0 );
    FillOctetString( Nic->ManagementInfo.Regdot11OperationalRateSet, Nic->ManagementInfo.Regdot11OperationalRateBuf, 0 );

    Nic->ManagementInfo.PhyMIB[0].OperationalRateSet.ucRateSet[0] = 2 ;
    Nic->ManagementInfo.PhyMIB[0].OperationalRateSet.ucRateSet[1] = 4 ;
    Nic->ManagementInfo.PhyMIB[0].OperationalRateSet.ucRateSet[2] = 11;
    Nic->ManagementInfo.PhyMIB[0].OperationalRateSet.ucRateSet[3] = 22 ;
    Nic->ManagementInfo.PhyMIB[0].OperationalRateSet.uRateSetLength = 4;

    Nic->ManagementInfo.PhyMIB[0].BasicRateSet.ucRateSet[0] = 2;
    Nic->ManagementInfo.PhyMIB[0].BasicRateSet.ucRateSet[1] = 4;
    Nic->ManagementInfo.PhyMIB[0].BasicRateSet.ucRateSet[2] = 11;
    Nic->ManagementInfo.PhyMIB[0].BasicRateSet.ucRateSet[3] = 22;
    Nic->ManagementInfo.PhyMIB[0].BasicRateSet.uRateSetLength = 4;

    Nic->ManagementInfo.PhyMIB[0].SupportedDataRatesValue.ucSupportedTxDataRatesValue[0] = 2;
    Nic->ManagementInfo.PhyMIB[0].SupportedDataRatesValue.ucSupportedTxDataRatesValue[1] = 4;
    Nic->ManagementInfo.PhyMIB[0].SupportedDataRatesValue.ucSupportedTxDataRatesValue[2] = 11;
    Nic->ManagementInfo.PhyMIB[0].SupportedDataRatesValue.ucSupportedTxDataRatesValue[3] = 22;

    Nic->ManagementInfo.PhyMIB[0].SupportedDataRatesValue.ucSupportedRxDataRatesValue[0] = 2;
    Nic->ManagementInfo.PhyMIB[0].SupportedDataRatesValue.ucSupportedRxDataRatesValue[1] = 4;
    Nic->ManagementInfo.PhyMIB[0].SupportedDataRatesValue.ucSupportedRxDataRatesValue[2] = 11;
    Nic->ManagementInfo.PhyMIB[0].SupportedDataRatesValue.ucSupportedRxDataRatesValue[3] = 22;


    Nic->ManagementInfo.OperationModeCapability.uOpModeCapability = DOT11_OPERATION_MODE_EXTENSIBLE_STATION
                                                                    | DOT11_OPERATION_MODE_NETWORK_MONITOR;
    Nic->ManagementInfo.CurrentOperationMode.uCurrentOpMode = DOT11_OPERATION_MODE_EXTENSIBLE_STATION;
    Nic->RxInfo.ReceiveProcessingFunction = HwProcessReceiveSTAMode;

    Nic->DupCurrentInfo.RTS_Threshold = 2346;      //2346 vs 2347    dot11RtsThreshold

    Nic->DupCurrentInfo.Frag_Threshold = 2346;// 2346 vs 2400  FragThreshold

    Nic->ManagementInfo.PhyMIB[0].Channel = HW11_DEFAULT_CHANNEL;  //==> Regdot11ChannelNumber
    Nic->ManagementInfo.DtimPeriod = 2;    
    Nic->DupCurrentInfo.BeaconInterval = 100;    //==> pMgntInfo->Regdot11BeaconPeriod 

    Nic->ManagementInfo.CapInfo = cESS |  cIBSS /*|cShortPreamble*/;       //=> RegmCap mitesh cESS | cIBSS  vs    cESS


    Nic->ManagementInfo.bScanCompleteIndicated = FALSE ;
    Nic->ManagementInfo.bScanInProgress = FALSE;

    Nic->ManagementInfo.uMPDUMaxLength = MAX_MPDU_LENGTH;

    Nic->ManagementInfo.EDThreshold = -65;
    Nic->ManagementInfo.ShortSlotTimeOptionEnabled = FALSE;

    Nic->DupCurrentInfo.MediaOccupancyLimit = 100;
    Nic->DupCurrentInfo.CFPPeriod = 255;
    Nic->DupCurrentInfo.CFPMaxDuration = 100;

    Nic->DupCurrentInfo.MaxTxLifeTime = 512;
    Nic->DupCurrentInfo.MaxRxLifeTime = 512;
    Nic->DupCurrentInfo.DefaultBeaconRate = 2;        // Set default beacon rate to 2 Mbps

    Nic->CardInfo.ResetInProgress = FALSE;
    Nic->ManagementInfo.SwChnlInProgress = FALSE;


    Nic->RfInfo.RadioOff = FALSE;
    Nic->RfInfo.RFPowerState = eRfOn;

    Nic->ManagementInfo.PowerMgmtMode.dot11PowerMode = dot11_power_mode_active;
    Nic->ManagementInfo.PowerMgmtMode.usAID = 0;
    NdisAllocateSpinLock(&Nic->ManagementInfo.PowerMgmtLock);

    Nic->RfInfo.CurrentRxAntenna = 1;
    Nic->RfInfo.CurrentTxAntenna = 1;
    Nic->CardInfo.InitGainState = 5;
    Nic->ManagementInfo.AtimWindow = 0;    /* set only when power management is enabled */

    Nic->ManagementInfo.DtimCount = Nic->ManagementInfo.DtimPeriod - 1;

    Nic->ManagementInfo.NextBeaconIndex = 0;

    Nic->ManagementInfo.Tim.BitmapCtrl = Nic->ManagementInfo.Tim.PartialVirtualBitmap[0] = 0;

    // These variable should initialize from registry
    for (Index = 0; Index < DUPLICATE_DETECTION_CACHE_LENGTH; Index++) {
        NdisZeroMemory(&Nic->RxInfo.DupePacketCache[Index], sizeof(DUPE_CACHE_ENTRY));
    }
    Nic->RxInfo.NextDupeCacheIndex = 0;


    if (Nic->DupCurrentInfo.EarlyRxThreshold == 7)                 // Turn OnlyErlPkt when early Rx is turn off
        Nic->RxInfo.ReceiveConfig |= RCR_OnlyErlPkt;


    Nic->ManagementInfo.CCAModeSupported = DOT11_CCA_MODE_CS_ONLY;
    Nic->ManagementInfo.CurrentCCAMode = DOT11_CCA_MODE_CS_ONLY;

    Nic->ManagementInfo.OffloadCapability.uReserved = 0;
    Nic->ManagementInfo.OffloadCapability.uFlags = 0;
    Nic->ManagementInfo.OffloadCapability.uSupportedWEPAlgorithms = 0;
    Nic->ManagementInfo.OffloadCapability.uNumOfReplayWindows = 0;
    Nic->ManagementInfo.OffloadCapability.uMaxWEPKeyMappingLength = 0;
    Nic->ManagementInfo.OffloadCapability.uSupportedAuthAlgorithms = 0;
    Nic->ManagementInfo.OffloadCapability.uMaxAuthKeyMappingLength = 0;

    Nic->ManagementInfo.CurrentOffloadCapability.uReserved = 0;
    Nic->ManagementInfo.CurrentOffloadCapability.uFlags = 0;

    Nic->ManagementInfo.OperationModeCapability.uReserved = 0;
    Nic->ManagementInfo.OperationModeCapability.uMajorVersion = MP_OPERATION_MODE_CAPABILITY_MAJOR_VERSION;
    Nic->ManagementInfo.OperationModeCapability.uMinorVersion = MP_OPERATION_MODE_CAPABILITY_MINOR_VERSION;
    // We can buffer as many packets as OS sends.
    Nic->ManagementInfo.OperationModeCapability.uNumOfTXBuffers = 0;
    Nic->ManagementInfo.OperationModeCapability.uNumOfRXBuffers = Nic->RxInfo.RxTotalDescNum;

    Nic->ManagementInfo.CurrentOperationMode.uReserved = 0;


    Nic->ManagementInfo.OptionalCapability.uReserved = 0;
    Nic->ManagementInfo.OptionalCapability.bDot11PCF = FALSE;
    Nic->ManagementInfo.OptionalCapability.bDot11PCFMPDUTransferToPC = FALSE;
    Nic->ManagementInfo.OptionalCapability.bStrictlyOrderedServiceClass = FALSE;

    Nic->ManagementInfo.CurrentOptionalCapability.uReserved = 0;
    Nic->ManagementInfo.CurrentOptionalCapability.bDot11CFPollable = FALSE;
    Nic->ManagementInfo.CurrentOptionalCapability.bDot11PCF = FALSE;
    Nic->ManagementInfo.CurrentOptionalCapability.bDot11PCFMPDUTransferToPC = FALSE;
    Nic->ManagementInfo.CurrentOptionalCapability.bStrictlyOrderedServiceClass = FALSE;

    Nic->ManagementInfo.pSupportedPhyTypes->uNumOfEntries = 1;
    Nic->ManagementInfo.pSupportedPhyTypes->uTotalNumOfEntries = MAX_NUM_PHY_TYPES;
    Nic->ManagementInfo.pSupportedPhyTypes->dot11PHYType[0] = dot11_phy_type_hrdsss;
    Nic->ManagementInfo.PhyMIB[0].PhyType = dot11_phy_type_hrdsss;


    for (Index = 4; Index < MAX_NUM_SUPPORTED_RATES_V2; Index++) {
        Nic->ManagementInfo.PhyMIB[0].SupportedDataRatesValue.ucSupportedTxDataRatesValue[Index] = 0;
        Nic->ManagementInfo.PhyMIB[0].SupportedDataRatesValue.ucSupportedRxDataRatesValue[Index] = 0;
    }

    Nic->ManagementInfo.CurrentBeaconRate = 2 * Nic->DupCurrentInfo.DefaultBeaconRate;    // 1Mbps = 2, 2Mbps = 4
    Nic->ManagementInfo.CurrentTXDataRate = 4;        // Set default data rate to 2 Mbps

    Nic->ManagementInfo.OperatingPhyId = 0;
    Nic->ManagementInfo.SelectedPhyId = 0;
    Nic->ManagementInfo.OperatingPhyMIB = Nic->ManagementInfo.PhyMIB;
    Nic->ManagementInfo.SelectedPhyMIB = Nic->ManagementInfo.PhyMIB;

    Nic->ManagementInfo.DiversitySupport = dot11_diversity_support_dynamic;

    Nic->ManagementInfo.SupportedPowerLevels.uNumOfSupportedPowerLevels = 4;
    Nic->ManagementInfo.SupportedPowerLevels.uTxPowerLevelValues[0] = 10;
    Nic->ManagementInfo.SupportedPowerLevels.uTxPowerLevelValues[1] = 20;
    Nic->ManagementInfo.SupportedPowerLevels.uTxPowerLevelValues[2] = 30;
    Nic->ManagementInfo.SupportedPowerLevels.uTxPowerLevelValues[3] = 50;
    Nic->ManagementInfo.CurrentTxPowerLevel = 1;    // 1 based

    Nic->ManagementInfo.pRegDomainsSupportValue->uNumOfEntries = 7;
    Nic->ManagementInfo.pRegDomainsSupportValue->uTotalNumOfEntries = MAX_NUM_DOT11_REG_DOMAINS_VALUE;
    for (Index=0; Index < Nic->ManagementInfo.pRegDomainsSupportValue->uNumOfEntries; Index++)
        Nic->ManagementInfo.pRegDomainsSupportValue->dot11RegDomainValue[Index].uRegDomainsSupportIndex = Index;

    Nic->ManagementInfo.pRegDomainsSupportValue->dot11RegDomainValue[0].uRegDomainsSupportValue = DOT11_REG_DOMAIN_OTHER;
    Nic->ManagementInfo.pRegDomainsSupportValue->dot11RegDomainValue[1].uRegDomainsSupportValue = DOT11_REG_DOMAIN_FCC;
    Nic->ManagementInfo.pRegDomainsSupportValue->dot11RegDomainValue[2].uRegDomainsSupportValue = DOT11_REG_DOMAIN_DOC;
    Nic->ManagementInfo.pRegDomainsSupportValue->dot11RegDomainValue[3].uRegDomainsSupportValue = DOT11_REG_DOMAIN_ETSI;
    Nic->ManagementInfo.pRegDomainsSupportValue->dot11RegDomainValue[4].uRegDomainsSupportValue = DOT11_REG_DOMAIN_SPAIN;
    Nic->ManagementInfo.pRegDomainsSupportValue->dot11RegDomainValue[5].uRegDomainsSupportValue = DOT11_REG_DOMAIN_FRANCE;
    Nic->ManagementInfo.pRegDomainsSupportValue->dot11RegDomainValue[6].uRegDomainsSupportValue = DOT11_REG_DOMAIN_MKK;
    // These two are supported, but the OS doesnt recognize these and hence we wont report these to the OS (NumOfEntries = 7)
    Nic->ManagementInfo.pRegDomainsSupportValue->dot11RegDomainValue[7].uRegDomainsSupportValue = HW_REG_DOMAIN_MKK1;
    Nic->ManagementInfo.pRegDomainsSupportValue->dot11RegDomainValue[8].uRegDomainsSupportValue = HW_REG_DOMAIN_ISRAEL;

    Nic->ManagementInfo.pDiversitySelectionRxList->uNumOfEntries = 2; 
    Nic->ManagementInfo.pDiversitySelectionRxList->uTotalNumOfEntries = MAX_NUM_DIVERSITY_SELECTION_RX_LIST;
    Nic->ManagementInfo.pDiversitySelectionRxList->dot11DiversitySelectionRx[0].uAntennaListIndex = 1;
    Nic->ManagementInfo.pDiversitySelectionRxList->dot11DiversitySelectionRx[0].bDiversitySelectionRX = TRUE;
    Nic->ManagementInfo.pDiversitySelectionRxList->dot11DiversitySelectionRx[1].uAntennaListIndex = 2;
    Nic->ManagementInfo.pDiversitySelectionRxList->dot11DiversitySelectionRx[1].bDiversitySelectionRX = TRUE;

    Nic->ManagementInfo.DelaySleepValue = 0;

    if (Nic->CardInfo.HardwareID == HW_ID_8187) {
        //
        // Updates hardware specific variables and some of the Native Wi-Fi capability
        // to match the hardware capabilities
        //
        Init8185(Nic);
    }

    NdisAllocateSpinLock(&Nic->ManagementInfo.PhyMIBConfigLock);
    Nic->ManagementInfo.bDeviceConnected = FALSE;
    
    // initialization for rate negotiation
    Nic->ManagementInfo.TotalRetryCount = 0;
    Nic->ManagementInfo.PacketsSentForTxRateCheck = 0;
    Nic->ManagementInfo.PrevTxDataRate = 0;
    Nic->ManagementInfo.TxRateIncreaseWaitCount = 0;
    Nic->ManagementInfo.TxRateIncreaseWaitRequired = 1;
    
    // initialization for g-mode protection
    Nic->ManagementInfo.bEnableSendCTSToSelf = FALSE;
    
    InitializeLED(Nic);
    Hw11ResetMacMIBs(Nic);
    Hw11ResetPhyMIBs(Nic);
    HwResetParameters(Nic, 0);
}

VOID
Hw11ResetStatistics(
                   __in  PNIC Nic
                   )
{
    // MAC statistics
    NdisZeroMemory(&Nic->StatisticsInfo, FIELD_OFFSET(NICSTATISTICSINFO, PhyCounters));

    // PHY statistics
    NdisZeroMemory(
                  Nic->StatisticsInfo.PhyCounters, 
                  sizeof(DOT11_PHY_FRAME_STATISTICS) * MAX_NUM_PHY_TYPES
                  );
}


VOID
Hw11ResetMacMIBs(
                __in   PNIC    Nic
                )
{
    //
    // If a variable must be reset during "MAC Reset", 
    // then it should be put here
    //

    Nic->RxInfo.MCAddressCount = 0;
    Nic->ManagementInfo.CurrentBSSType = dot11_BSS_type_infrastructure;
    ETH_COPY_NETWORK_ADDRESS(Nic->CardInfo.StationID, Nic->CardInfo.PermanentAddress);
    ETH_COPY_NETWORK_ADDRESS(Nic->CardInfo.MacAddr, Nic->CardInfo.PermanentAddress);
}


VOID
Hw11ResetPhyMIBs(
                __in  PNIC    Nic
                )
{
    Nic->ManagementInfo.SelectedPhyId = 0;
    Nic->ManagementInfo.SelectedPhyMIB = Nic->ManagementInfo.PhyMIB;
}


VOID
HwResetParameters(
                 __in  PNIC                Nic,
                 __in  DOT11_RESET_TYPE    ResetType
                 )
{
    //
    // Parameters that should be reset irrespective of MIB reset come here.
    //
    UNREFERENCED_PARAMETER(ResetType);

    Nic->ManagementInfo.DefaultKeyID = 0;
    Nic->ManagementInfo.HwAuthAlgorithm = DOT11_AUTH_ALGO_80211_OPEN;
    Nic->ManagementInfo.HwUnicastCipher = DOT11_CIPHER_ALGO_NONE;
    Nic->ManagementInfo.HwMulticastCipher = DOT11_CIPHER_ALGO_NONE;
    HwDeleteAllKeys(Nic);

    Nic->ManagementInfo.pPowerMgmtChangePktDesc = NULL;
    Nic->ManagementInfo.bPowerMgmtChangePktSent = FALSE;
    Nic->ManagementInfo.bATIMPacketReceived = FALSE;
    Nic->ManagementInfo.bUnicastDataAvailable = FALSE;
    Nic->ManagementInfo.WakeupTime = 0;
    Nic->ManagementInfo.PowerMgmtMode.usAID = 0;

    HwClearAllIE(Nic);
}


// Called at PASSIVE, DISPATCH & DIRQL
// Returns TRUE if at the end of the call, the new state of the RF is the requested
// state, returns FALSE otherwise
BOOLEAN
HwSetRFState(
    __in  PNIC                pNic,
    __in  RT_RF_POWER_STATE   NewRFState
    )
{
    BOOLEAN     bResult = TRUE;
    
    do
    {
        if (InterlockedIncrement(&pNic->RfInfo.AccessRefCount) == 1)
        {
            if (pNic->RfInfo.RadioStateChangeInProgress)
            {
                //
                // Radio state is being changed, dont proceed
                //
                bResult = FALSE;
                break;
            }
            
            if ((NewRFState == eRfOn) || (NewRFState == eRfSleep))
            {
                if (pNic->RfInfo.RadioOff == TRUE)
                {
                    //
                    // Radio is OFF, dont change RF state
                    //
                    bResult = FALSE;
                    break;
                }
            }
            
            SetRFPowerState(pNic, NewRFState);
        }
        else
        {
            //
            // Another thread is changing RF/Radio state,
            // dont modify. We bail out and the caller can decide if it wants to
            // reattempt (we dont wait since we may be called at high IRQL, etc)
            //
            MpTrace(COMP_POWER, DBG_LOUD, ("Unable to set RF/Radio state as something else is updating it\n"));
            bResult = FALSE;
        }
    }WHILE (FALSE);

    InterlockedDecrement(&pNic->RfInfo.AccessRefCount);
    
    return bResult;
}

VOID
HwSetRadioState(
    __in  PNIC    pNic,
    __in  BOOLEAN bPowerOn
    )
{
    //
    // Set flag so other threads (power save, scan timer, etc) wont change
    // state behind us
    //
    pNic->RfInfo.RadioStateChangeInProgress = TRUE;

    //
    // We wait for ever for other threads to not reach this state
    //
    while (pNic->RfInfo.AccessRefCount != 0);

    //
    // Set the Radio state
    //
    if (bPowerOn)
    {
        SetRFPowerState(pNic, eRfOn);
        pNic->RfInfo.RadioOff = FALSE;
    }
    else
    {
        pNic->RfInfo.RadioOff = TRUE;

        SetRFPowerState(pNic, eRfShutdown);
        UpdatePortUchar(pNic, MSR, &0xF3);
    }

    pNic->RfInfo.RadioStateChangeInProgress = FALSE;
}

USHORT
HwGenerateBeacon(
                __in  PNIC Nic,
                __in  PUCHAR BeaconBuf,
                __in  BOOLEAN APModeFlag
                )
{
    int                     i, j;
    OctetString             os;
    OctetString             Beacon;
    BeaconProbeRspPartial  *pBeaconPartial;
    MP_TIM_ELEMENT          dot11TimElement;
    UCHAR                   Rates[7] = {0x82, 0x84, 0x8b, 0x96, 0x8c, 0x98, 0xb0};
    UCHAR                   ExRates[8] = {0x0c, 0x12, 0x18, 0x24, 0x30, 0x48, 0x60, 0x6C};

    //
    // Setup Beacon Packet
    //
    pBeaconPartial = (BeaconProbeRspPartial *)BeaconBuf;

    pBeaconPartial->GenPacketPartial.FrameCtrl.CharData[0] = Type_Beacon;
    pBeaconPartial->GenPacketPartial.FrameCtrl.CharData[1] = 0;

    pBeaconPartial->GenPacketPartial.Duration = 0;
    NdisFillMemory( pBeaconPartial->GenPacketPartial.Addr1, 6, 0xff);
    ETH_COPY_NETWORK_ADDRESS(pBeaconPartial->GenPacketPartial.Addr2, Nic->CardInfo.MacAddr);
    ETH_COPY_NETWORK_ADDRESS(pBeaconPartial->GenPacketPartial.Addr3, Nic->ManagementInfo.BssId);

    pBeaconPartial->GenPacketPartial.Seq = 0;
    pBeaconPartial->BeaconInterval = (USHORT)Nic->DupCurrentInfo.BeaconInterval;
    pBeaconPartial->CapabilityInfo = Nic->ManagementInfo.CapInfo;

    FillOctetString(Beacon, BeaconBuf, 0x24);

    FillOctetString(os, Nic->ManagementInfo.SSID, (USHORT)strlen((PCHAR)Nic->ManagementInfo.SSID));


    PacketMakeElement(&Beacon, EID_SsId, os);

    if (Nic->ManagementInfo.dot11CurrentWirelessMode == WIRELESS_MODE_A) {
        FillOctetString(os, Rates + 4, 3);
    }
    else if(Nic->ManagementInfo.dot11CurrentWirelessMode == WIRELESS_MODE_B)
    {
        FillOctetString(os, Rates, 4);
        PacketMakeElement(&Beacon, EID_SupRates, os);
    }
    else //G mode
    {
        FillOctetString(os, Rates, 4);
        PacketMakeElement(&Beacon, EID_SupRates, os);

        //
        // fill up extented rate
        //
        FillOctetString(os,ExRates,8);
        PacketMakeElement(&Beacon, EID_ExtSupRates, os);     
    }

    FillOctetString(os, &Nic->ManagementInfo.OperatingPhyMIB->Channel, 1);
    PacketMakeElement(&Beacon, EID_DSParms, os);

    if (APModeFlag) {         // TIM parameter field
        USHORT      uSize;
#ifdef USE_SAMPLE_PACKET_BUFFERING_IMPLEMENTATION
        uSize = MpGetTimElement(Nic->Adapter, &dot11TimElement);
#else
        uSize = 6;
#endif
        FillOctetString(os, ((PUCHAR)&dot11TimElement) + sizeof(DOT11_INFO_ELEMENT), uSize - 2);
        PacketMakeElement(&Beacon, EID_Tim, os);
    }
    else {       // IBSS parameter field
        FillOctetString(os, (PUCHAR)&Nic->ManagementInfo.AtimWindow, 2);
        PacketMakeElement(&Beacon, EID_IbssParms, os);
    }

    //3 Append other IEs
    for (i = EID_IbssParms + 1; i < 256; i++) {
        UCHAR   Size;
        PUCHAR  Data;

        j = 0;
        do {
            Data = HwGetIE(Nic, (UCHAR)i, (UCHAR)j, &Size);
            if (Data != NULL) {
                FillOctetString(os, Data, Size);
                PacketMakeElement(&Beacon, i, os);
            }

            j++;
        } while (i == 0xdd && Data != NULL);
    }

    return Beacon.Length;
}

void
HwSetupBeacon(
             __in  PNIC Nic,
             __in  PUCHAR Descriptor,
             __in  USHORT BeaconBufLen,
             __in  ULONG  BeaconBufPaLow,
             __in  USHORT Rate
             )
{

    USHORT      seqNumber;
    PUCHAR      TxBuf;
    PMP_DOT11_MGMT_DATA_MAC_HEADER   fragmentHeader;
    UNREFERENCED_PARAMETER(BeaconBufPaLow);

    seqNumber = (GetSequenceNumber(Nic) << 4 );

    if (SetupBeaconQueue8187(Nic) == FALSE) {
        return;
    }
    
    TxBuf = HwGetHWDESCAdjustedBuffer(Descriptor);
    fragmentHeader =  (PMP_DOT11_MGMT_DATA_MAC_HEADER)TxBuf;
    NdisMoveMemory(&fragmentHeader->SequenceControl, &seqNumber, 2);

    TxFillDescriptor8187(Nic,                    
                         (PTX_DESC_8187)Descriptor,//Nic->TxInfo.TxDesc[BEACON_QUEUE],
                         BEACON_QUEUE,
                         TxBuf,
                         TRUE,  // Start Of MPDU
                         TRUE,  // End of MPDU
                         TxBuf,//pTcb->BufferList[i].VirtualAddress,
                         BeaconBufLen ,//pTcb->BufferList[i].Length,
                         BeaconBufLen,//pTcb->FragLength[FragIndex],
                         Rate,
                         TRUE, //bMulticast
                         FALSE, //bUseShortPreamble,
                         FALSE,//FragIndex!=(pTcb->FragCount-1),   //bMoreFrag
                         FALSE,  //  bCTSEnable
                         FALSE,//bRTSEnable,
                         0, //RtsDur
                         Rate, //RTS rate
                         FALSE, //bEncrypt,
                         0,//RetryLimit
                         NULL //TxD
                        );       

    HwBusSendPacketAsync(Nic,   BEACON_QUEUE, BeaconBufLen+RTL8187_HWDESC_HEADER_LEN, Descriptor, NULL);

}

void
HwPeriodTimerCallback(
    __in WDFTIMER     timer
	)
{
    PNIC                pNic;
    PTIMER_NIC_CONTEXT  timerNicContext;

    timerNicContext  = GetTimerContext(timer);
    pNic = timerNicContext->Nic;   

    // Hardware function does the work
    HwPeriodTimerHandler(pNic);

    // Tx data rate negotiation
        HwUpdateTxDataRate(pNic);
}


UCHAR
HwTxRetryLimit(
              __in  PNIC            Nic,
              __in  USHORT          DataRate    
              )
{
    UCHAR  retryLimit;

    UNREFERENCED_PARAMETER(Nic);

    // To let every data rate has the change to use 1M retry,
    // we set differnet retry count to each one.   
    switch (DataRate) {
        case 108: // 54M 
            retryLimit = 11;
            break;

        case 96: // 48M 
            retryLimit = 10;
            break;

        case 72: // 36M 
            retryLimit = 9;
            break;

        case 48: // 24M 
            retryLimit = 8;
            break;

        default:
            retryLimit = 7;
            break;
    }
    return retryLimit;
}

UCHAR
HwMRateToHwRate(
               __in  UCHAR          rate
               )
{
    UCHAR  ret = DESC85_RATE1M;

    switch (rate) {
        case 0x02:  ret = DESC85_RATE1M;    break;
        case 0x04:  ret = DESC85_RATE2M;    break;
        case 0x0b:  ret = DESC85_RATE5_5M;  break;
        case 0x16:  ret = DESC85_RATE11M;   break;
        case 0x0c:  ret = DESC85_RATE6M;    break;
        case 0x12:  ret = DESC85_RATE9M;    break;
        case 0x18:  ret = DESC85_RATE12M;   break;
        case 0x24:  ret = DESC85_RATE18M;   break;
        case 0x30:  ret = DESC85_RATE24M;   break;
        case 0x48:  ret = DESC85_RATE36M;   break;
        case 0x60:  ret = DESC85_RATE48M;   break;
        case 0x6c:  ret = DESC85_RATE54M;   break;
        case 0x90:  ret = DESC85_RATE72M;   break;

        default:
            MpTrace(COMP_SEND, DBG_SERIOUS, ("!!MRate not supported. Rate: %d!!\n", rate));
            MPASSERT(FALSE);
            break;
    }

    return ret;
}


UCHAR
HwHwRateToMRate(
               __in  UCHAR    rate)
{
    static UCHAR RateMap[] = {2, 4, 11, 22, 12, 18, 24, 36, 48, 72, 96, 108, 144};

    if (rate < sizeof(RateMap))
        return RateMap[rate];

    MpTrace(COMP_SEND, DBG_SERIOUS, ("!!HwRate not supported. Rate: %d!!\n", rate));
    MPASSERT(FALSE);

    return 2;
}

ULONG64
HwDataRateToLinkSpeed(
                     UCHAR  rate
                     )
{
    return((ULONG64)500000) * rate;
}

UCHAR
HwLinkSpeedToDataRate(
    ULONG64 linkSpeed
    )
{
    return (UCHAR)(linkSpeed / 500000);
}



ULONG
HwGetTxBusyDescNum(
                  __in  PNIC Nic,
                  __in  int QueueIndex)
{
    int BusyNum ;

    BusyNum = Nic->TxInfo.TxNextDescToSend[QueueIndex] - Nic->TxInfo.TxNextDescToCheck[QueueIndex];

    if (BusyNum < 0) {
        BusyNum += Nic->TxInfo.TxTotalDescNum[QueueIndex];
    }

    return BusyNum;
}


ULONG
HwGetTxFreeDescNum(
                  __in  PNIC Nic,
                  __in  int QueueIndex)
{
    //
    // The -1 is there because we cannot use all the desc for rtl8180. We have to leave at least one
    // free at all times.
    //
    ULONG x;
    
    NdisAcquireSpinLock(&Nic->TxInfo.TxSpinLock);
    x= Nic->TxInfo.TxTotalDescNum[QueueIndex] - HwGetTxBusyDescNum(Nic,QueueIndex) - 1;
    NdisReleaseSpinLock(&Nic->TxInfo.TxSpinLock); 
    return x;
}

PVOID
HwNormalQGetNextToSendBuffer(
    __in  PNIC       Nic
    )
{
    PVOID pDesc;

    pDesc= NULL;
    NdisAcquireSpinLock(&Nic->TxInfo.TxSpinLock);
    pDesc = (Nic)->TxInfo.pTxNormalQBufVa[(Nic)->TxInfo.TxNextDescToSend[NORMAL_QUEUE]];
    HwIncrementNextDescToSend(Nic,NORMAL_QUEUE, 1 );
    NdisReleaseSpinLock(&Nic->TxInfo.TxSpinLock); 
    return pDesc;  
}

PVOID
HwHighQGetNextToSendBuffer(
    __in  PNIC       Nic
    )
{
    PVOID pDesc;

    pDesc= NULL;
    NdisAcquireSpinLock(&Nic->TxInfo.TxSpinLock);
    pDesc = (Nic)->TxInfo.pTxHighQBufVa[(Nic)->TxInfo.TxNextDescToSend[HIGH_QUEUE]];
    HwIncrementNextDescToSend(Nic,HIGH_QUEUE, 1 );
    NdisReleaseSpinLock(&Nic->TxInfo.TxSpinLock);   
    return pDesc;   
}

void 
HwTransmitNextPacketWithLocalBuffer(
                                   __in  PNIC       Nic,
                                   __in  UCHAR      QueueIndex,
                                   __in  USHORT     PktSize,
                                   __in  USHORT     Rate,
                                   __in  BOOLEAN    bAIDContained,
                                   __in  PUCHAR     pDesc
                                   )
{
    PUCHAR      TxBuf ;
    USHORT      AckCtsTime;
    USHORT      rtsDuration;
    USHORT      seqNumber;
    PMP_DOT11_MGMT_DATA_MAC_HEADER   fragmentHeader;
    
    seqNumber = (GetSequenceNumber(Nic) << 4 );
    //
    // TODO: for adhoc mode rtsDuration is a function of RtsFrametime for 8187
    //
    rtsDuration = 0;   
    if (HwGetTxFreeDescNum(Nic, QueueIndex) > 0) {

        if (HwAwake(Nic) == FALSE) {            
            return; // This packet wont be sent, we drop it silently
        }       
                
        TxBuf = HwGetHWDESCAdjustedBuffer(pDesc);
        fragmentHeader =  (PMP_DOT11_MGMT_DATA_MAC_HEADER)TxBuf;
        NdisMoveMemory(&fragmentHeader->SequenceControl, &seqNumber, 2);

        if (!bAIDContained) {
            if (HwIsMulticast(TxBuf)) {
                *((USHORT *)(&TxBuf[2])) = 0;    // Set duration to 0
                rtsDuration =  0;     
            }
            else {
                AckCtsTime = ComputeTxTime(sAckCtsLng / 8, Rate, TRUE, FALSE);    // calculate AckCtsTime
                *((USHORT *)(&TxBuf[2])) = aSifsTime + AckCtsTime;
                rtsDuration = aSifsTime + AckCtsTime;
            }
        }

        TxFillDescriptor8187(Nic,
                             (PTX_DESC_8187)pDesc,
                             QueueIndex,
                             TxBuf,
                             TRUE,  // Start Of MPDU
                             TRUE,  // End of MPDU
                             TxBuf,//pTcb->BufferList[i].VirtualAddress,
                             PktSize,//pTcb->BufferList[i].Length,
                             PktSize,//pTcb->FragLength[FragIndex],
                             Rate,
                             HwIsMulticast(TxBuf), //bMulticast
                             FALSE, //bUseShortPreamble,
                             FALSE,   //bMoreFrag
                             FALSE,  //   bCTSEnable CTS == Clear TO send control packet
                             FALSE,//bRTSEnable,  RTS == Ready TO send contol packet
                             rtsDuration,    //rts duration
                             22,//RTS rate
                             TRUE, //bEncrypt,
                             HwTxRetryLimit(Nic,  Rate), //retyrLimit 
                             NULL //TxD
                             );
        
        //
        // Note we pass the address of the pDesc here which is the beginning of the buffer  
        //  which has the HW_TX_DESC followed by teh actual dot 11 data.
        //    
        HwBusSendPacketAsync(Nic,   QueueIndex, PktSize+RTL8187_HWDESC_HEADER_LEN, pDesc, NULL);
    }
}

void
HwOnProbe(
         __in  PNIC      Nic,
         __in  PUCHAR    ProbePktBuf,
         __in  USHORT    ProbePktLen,
         __in  USHORT    ProbePktRate
         )
{
    int                     i, j;
    PUCHAR                  pDesc;
    PUCHAR                  TxBuf;
    OctetString             os;
    OctetString             ProbeReq;
    OctetString             ProbeRsp;
    BeaconProbeRspPartial  *pProbeRspPartial;
    UCHAR                   Rates[7] = {0x82, 0x84, 0x8b, 0x96, 0x8c, 0x98, 0xb0};
    UCHAR                   ExRates[8] = {0x0c, 0x12, 0x18, 0x24, 0x30, 0x48, 0x60, 0x6C};
    ULONGLONG               timeStamp;
    LARGE_INTEGER           ts;
    
    if (HwGetTxFreeDescNum(Nic, NORMAL_QUEUE) > 0) {
        pDesc = HwNormalQGetNextToSendBuffer(Nic);
        TxBuf = HwGetHWDESCAdjustedBuffer(pDesc);

        //
        // Setup Probe Response Packet
        //
        FillOctetString(ProbeReq, ProbePktBuf, ProbePktLen);

        pProbeRspPartial = (BeaconProbeRspPartial *) TxBuf;

        pProbeRspPartial->GenPacketPartial.FrameCtrl.CharData[0] = Type_Probe_Rsp;
        pProbeRspPartial->GenPacketPartial.FrameCtrl.CharData[1] = 0;
        pProbeRspPartial->GenPacketPartial.Duration = 0;

        NdisMoveMemory(pProbeRspPartial->GenPacketPartial.Addr1, ProbeReq.Octet + 10, 6);
        ETH_COPY_NETWORK_ADDRESS(pProbeRspPartial->GenPacketPartial.Addr2, Nic->CardInfo.MacAddr);
        ETH_COPY_NETWORK_ADDRESS(pProbeRspPartial->GenPacketPartial.Addr3, Nic->ManagementInfo.BssId);
        pProbeRspPartial->GenPacketPartial.Seq = 0;        
        pProbeRspPartial->BeaconInterval = (USHORT)Nic->DupCurrentInfo.BeaconInterval;
        pProbeRspPartial->CapabilityInfo = Nic->ManagementInfo.CapInfo;

        // Size of Probe Response, can not use sizeof(BeaconProbeRspPartial)
        FillOctetString(ProbeRsp, TxBuf, 0x24);

        FillOctetString(os, Nic->ManagementInfo.SSID, (USHORT)strlen((CHAR*)Nic->ManagementInfo.SSID));
        PacketMakeElement(&ProbeRsp, EID_SsId, os);

        if (Nic->ManagementInfo.dot11CurrentWirelessMode == WIRELESS_MODE_A) {
            FillOctetString(os, ExRates, 8);
            PacketMakeElement(&ProbeRsp, EID_SupRates, os);
        }
        else if (Nic->ManagementInfo.dot11CurrentWirelessMode == WIRELESS_MODE_B)
        {
            FillOctetString(os, Rates, 4);
            PacketMakeElement(&ProbeRsp, EID_SupRates, os);
        }
        else //wirelessmodeG
        {
            FillOctetString(os, Rates, 4);
            PacketMakeElement(&ProbeRsp, EID_SupRates, os);

            //fill up extented rate
            FillOctetString(os,ExRates,8);
            PacketMakeElement(&ProbeRsp, EID_ExtSupRates, os);
        }

        FillOctetString(os, &Nic->ManagementInfo.OperatingPhyMIB->Channel, 1);
        PacketMakeElement(&ProbeRsp, EID_DSParms, os);

        if (Nic->ManagementInfo.CurrentBSSType == dot11_BSS_type_independent) {
            FillOctetString(os, (PUCHAR)&Nic->ManagementInfo.AtimWindow, 2);
            PacketMakeElement(&ProbeRsp, EID_IbssParms, os);
        }

        //
        // Append other IEs
        //
        for (i = EID_IbssParms + 1; i < 256; i++) {
            UCHAR   Size;
            PUCHAR  Data;

            j = 0;
            do {
                Data = HwGetIE(Nic, (UCHAR)i, (UCHAR)j, &Size);
                if (Data != NULL) {
                    FillOctetString(os, Data, Size);
                    PacketMakeElement(&ProbeRsp, i, os);
                }

                j++;
            } while (i == 0xdd && Data!=NULL);
        }

        //
        // Read the timestamp from TSF and place it in the Probe Response
        // Read the high word first. Low word changes frequently and we 
        // want the freshest value we can get.
        //
        NdisGetCurrentSystemTime(&ts);  // driver version
        timeStamp = ts.QuadPart/10;
        pProbeRspPartial->TimeStamp = (ULONG)(timeStamp & 0xffffffff);
        *(((PULONG)&pProbeRspPartial->TimeStamp) + 1) = (ULONG)(timeStamp>>32);    
#ifdef OLD_TIMESTAMP_WAY
    
        *(((PULONG)&pProbeRspPartial->TimeStamp) + 1) = HwPlatformEFIORead4Byte(Nic, TSFR+0x04);
        pProbeRspPartial->TimeStamp = HwPlatformEFIORead4Byte(Nic, TSFR);
#endif
        HwTransmitNextPacketWithLocalBuffer(Nic,
                                            NORMAL_QUEUE,
                                            ProbeRsp.Length,
                                            ProbePktRate,
                                            FALSE,
                                            pDesc);
    }
}

void
HwSendProbe(
           __in PNIC                 Nic,
           __in PDOT11_MAC_ADDRESS   pBSSID,
           __in PDOT11_SSID          pSSID,
           __in PUCHAR               ScanAppendIEByteArray,
           __in USHORT               ScanAppendIEByteArrayLength
           )
{
    PUCHAR                  TxBuf;
    OctetString             os;
    OctetString             ProbeRsp;
    GeneralPacketPartial   *pProbeRequestPartial;
    BOOLEAN                 bAnyProbe;
    UCHAR                   Rates[7] = {0x82, 0x84, 0x8b, 0x96, 0x8c, 0x98, 0xb0};
    PUCHAR                  pDesc;

    bAnyProbe = FALSE;

    if (HwGetTxFreeDescNum(Nic, NORMAL_QUEUE) > 0) {
        pDesc = HwNormalQGetNextToSendBuffer(Nic);
        TxBuf = HwGetHWDESCAdjustedBuffer(pDesc);
        //
        // Setup Probe Response Packet
        //
        pProbeRequestPartial = (GeneralPacketPartial *) TxBuf;

        pProbeRequestPartial->FrameCtrl.CharData[0] = Type_Probe_Req;
        pProbeRequestPartial->FrameCtrl.CharData[1] = 0;
        pProbeRequestPartial->Duration = 0;
        NdisFillMemory(pProbeRequestPartial->Addr1, 6, (UCHAR)0xff);
        ETH_COPY_NETWORK_ADDRESS(pProbeRequestPartial->Addr2, Nic->CardInfo.MacAddr);
        NdisMoveMemory(pProbeRequestPartial->Addr3, pBSSID, 6);
        pProbeRequestPartial->Seq = 0;

        //
        // Size of Probe Request, can not use sizeof(GeneralPacketPartial)
        //
        FillOctetString(ProbeRsp, TxBuf, 0x18);

        FillOctetString(os, pSSID->ucSSID, bAnyProbe ? 0 : (USHORT)pSSID->uSSIDLength);
        //
        // TODO: Should we do anything special here if  any pSSID->uSSIDLength != 0
        //
        PacketMakeElement(&ProbeRsp, EID_SsId, os);

        if (Nic->ManagementInfo.dot11CurrentWirelessMode == WIRELESS_MODE_A) {
            FillOctetString(os, Rates + 4, 3);
        }
        else {
            FillOctetString(os, Rates, 4);
        }

        PacketMakeElement(&ProbeRsp, EID_SupRates, os);

        if (ScanAppendIEByteArrayLength > 0) {
            //
            // Append IEs needed for WPA etc.
            //
            FillOctetString(os, ScanAppendIEByteArray, ScanAppendIEByteArrayLength);
            PacketAppendData(&ProbeRsp, os);
        }

        ASSERT(Nic->ManagementInfo.CurrentBeaconRate > 1);
        HwTransmitNextPacketWithLocalBuffer(Nic,
                                            NORMAL_QUEUE,
                                            ProbeRsp.Length,
                                            Nic->ManagementInfo.CurrentBeaconRate, 
                                            FALSE,
                                            pDesc);
    }
}

LONG
HwGetPositionInBuffer(
                     __in  ULONG    PriorLength,
                     __in  ULONG    BufferLength,
                     __in  ULONG    TotalOffset)
{
    if (TotalOffset >= PriorLength && TotalOffset < (PriorLength + BufferLength))
        return TotalOffset - PriorLength;
    else
        return -1;
}

void
HwScanComplete(
              __in  PNIC Nic
              )
{
    MpTrace(COMP_SCAN, DBG_LOUD, ("Scan operation completed\n"));

    //
    // Free the memory allocated for this operation
    //
    MP_FREE_MEMORY(Nic->ManagementInfo.ScanChannelList);
    // 2    MP_FREE_MEMORY(Nic->ManagementInfo.ScanRequest);

    Nic->ManagementInfo.ScanChannelList = NULL;
    Nic->ManagementInfo.ScanRequest = NULL;

    //
    // Indicate scan completion to the OS
    //
    if (Nic->ManagementInfo.ScanCancelOperation)
        Mp11ScanComplete(Nic->Adapter, NDIS_STATUS_REQUEST_ABORTED);
    else
        Mp11ScanComplete(Nic->Adapter, NDIS_STATUS_SUCCESS);

    //
    // reset the scan state variables (after we have completed the scan)
    //
    Nic->ManagementInfo.bScanInProgress = FALSE;
    Nic->ManagementInfo.bScanCompleteIndicated = TRUE;

    //
    // restore Hw related parameter 
    //
    HwRestoreToBeforeScan(Nic);

    //
    // If we are in power saving mode and we are not associated, turn off the RF.
    // If we are associated, RF will be turned off by HwResponseToPacket.
    //
    NdisAcquireSpinLock(&Nic->ManagementInfo.PowerMgmtLock);
    if (Nic->ManagementInfo.PowerMgmtMode.dot11PowerMode == dot11_power_mode_powersave &&
        Nic->ManagementInfo.PowerMgmtMode.usAID == 0) {
        MpTrace(COMP_POWER, DBG_LOUD, (" *** RF OFF indefinitely\n"));
        SetRFPowerState(Nic, eRfOff);
    }
    NdisReleaseSpinLock(&Nic->ManagementInfo.PowerMgmtLock);

}


VOID
HwScanRestoreOriginalChannel(
                            __in  PNIC Nic
                            )
{
    HwScanComplete(Nic);
}

VOID
HwScanChannelSwitchComplete(
                           __in  PNIC Nic
                           )
{
    MpTrace(COMP_SCAN, DBG_LOUD, ("Channel switched\n"));

    //
    // The current scan state should be switch channel
    //
    MPVERIFY(Nic->ManagementInfo.ScanCurrentState == ScanStateSwChnl);

    //
    // ScanCallback has requested a channel switch. That operation has
    // been completed. We need to:
    // 1. Transition the scan state from Switch to Scan
    // 2. Schedule the ScanCallback DPC asap to do the scan.
    //
    HW_TRANSITION_SCAN_STATE(Nic->ManagementInfo.ScanCurrentState);

    WdfTimerStart(Nic->ManagementInfo.Timer_Scan, 
                  WDF_REL_TIMEOUT_IN_MS(Nic->ManagementInfo.ScanProbeDelay));
}

VOID
HwScanCallback(
              __in WDFTIMER  Timer
              )
{
    PNIC                Nic;
    ULONG               uRescheduleTime;
    PTIMER_NIC_CONTEXT  timerNicContext;

    timerNicContext  = GetTimerContext(Timer);
    Nic = timerNicContext->Nic;

    WHILE (TRUE) 
    {
        if (Nic->ManagementInfo.ScanCurrentChannelIndex == Nic->ManagementInfo.ScanChannelCount) {
            //
            // Last channel has been scanned. Revert to the channel we had before scan started
            //
            MpTrace(COMP_SCAN, DBG_LOUD, ("Done with the last channel. Revert back to original channel(%d)\n",
                                          Nic->ManagementInfo.OperatingPhyMIB->Channel));

            if (Nic->ManagementInfo.DestChannel == Nic->ManagementInfo.OperatingPhyMIB->Channel) {
                MpTrace(COMP_SCAN, DBG_LOUD, ("Already on target channel ! Switch not required\n"));
                HwScanComplete(Nic);
            }
            else {
                HwSetChannel(Nic, Nic->ManagementInfo.OperatingPhyMIB->Channel, HwScanRestoreOriginalChannel);
            }

             //
            // Don't reschedule the DPC. We are done.
            //
            return;
        }
        else if (Nic->ManagementInfo.ScanCancelOperation) {
            //
            // Scan operation has been cancelled
            //
            MpTrace(COMP_SCAN, DBG_LOUD, ("Scan operation has been cancelled\n"));

            //
            // Set the termination condition to true. This will cancel scan automatically
            //
            Nic->ManagementInfo.ScanCurrentChannelIndex = Nic->ManagementInfo.ScanChannelCount;

            continue;
        }
        else {
            if (Nic->ManagementInfo.ScanCurrentState == ScanStateSwChnl) {
                ULONG   uDestChannel;

                //
                // Switch to the next channel. ScanCurrentChannelIndex point to it.
                //                
                uDestChannel = Nic->ManagementInfo.ScanChannelList[Nic->ManagementInfo.ScanCurrentChannelIndex];

                MpTrace(COMP_SCAN, DBG_LOUD, ("Switching to Channel %d\n", uDestChannel));
                HwSetChannel(Nic, (UCHAR)uDestChannel, HwScanChannelSwitchComplete);  

                //
                // Don't rescedule the DPC. We will do that when the channel switch completes
                //
                return;
            }
            else {
                ULONG                   i, BytesParsed = 0;
                PDOT11_SSID             pDot11SSID;
                PDOT11_SCAN_REQUEST_V2  pDot11ScanRequest = Nic->ManagementInfo.ScanRequest;

                //
                // Scan the current channel
                //
                if ((pDot11ScanRequest->dot11ScanType == dot11_scan_type_active) ||
                    (pDot11ScanRequest->dot11ScanType == (dot11_scan_type_active | dot11_scan_type_forced)) ||
                    (pDot11ScanRequest->dot11ScanType == dot11_scan_type_auto) ||
                    (pDot11ScanRequest->dot11ScanType == (dot11_scan_type_auto | dot11_scan_type_forced))) {
                    //
                    // For active scan send out probes. Auto is active for us.
                    // We will send out a probe request for each SSID in the request
                    //
                    for (i = 0; i < pDot11ScanRequest->uNumOfdot11SSIDs; i++) {
                        pDot11SSID = (PDOT11_SSID) (pDot11ScanRequest->ucBuffer + 
                                                    pDot11ScanRequest->udot11SSIDsOffset + 
                                                    BytesParsed);

                        HwSendProbe(Nic,
                                    &pDot11ScanRequest->dot11BSSID,
                                    pDot11SSID,
                                    pDot11ScanRequest->ucBuffer + pDot11ScanRequest->uIEsOffset,
                                    (USHORT) pDot11ScanRequest->uIEsLength);

                        BytesParsed += sizeof(DOT11_SSID);
                    }
                }
                else {
                    //
                    // If passive scanning, we will park longer. Beacons and probes
                    // are indicated up anyways. We need not do anything.
                    //
                }

                //
                // Move to the next Scan state.
                //
                HW_TRANSITION_SCAN_STATE(Nic->ManagementInfo.ScanCurrentState);

                //
                // If we are going to switch to the next channel, 
                // move to the desired channel
                //
                if (Nic->ManagementInfo.ScanCurrentState == ScanStateSwChnl) {
                    Nic->ManagementInfo.ScanCurrentChannelIndex++;
                }

                uRescheduleTime = Nic->ManagementInfo.ScanChannelTime;
            }
        }

        //
        // Always break out. We will loop only if cancel is received.
        //
        break;
    }

    //
    // Reschedule the DPC
    //
    MPVERIFY(uRescheduleTime > 0);
    WdfTimerStart(Nic->ManagementInfo.Timer_Scan, WDF_REL_TIMEOUT_IN_MS(uRescheduleTime));
    
}

VOID
HwSwChnlWorker(
              PNIC    Nic 
              )
{      
    SwChnlPhy(Nic, Nic->ManagementInfo.DestChannel); 

    if (Nic->ManagementInfo.bScanInProgress) {
        *(((PULONG)&Nic->ManagementInfo.ScanChannelSwitchTime) + 1) = HwPlatformEFIORead4Byte(Nic, TSFR + 0x04);
        Nic->ManagementInfo.ScanChannelSwitchTime = HwPlatformEFIORead4Byte(Nic, TSFR );
        if (Nic->ManagementInfo.ScanChannelSwitchTime == 0xffffffffffffffff) {
            ASSERT(FALSE) ;
        }

#ifdef OLD_TIMESTAMP_WAY
        NdisGetCurrentSystemTime(&ts);  // usb driver version
        timeStamp = ts.QuadPart/10;
        Nic->ManagementInfo.ScanChannelSwitchTime = (ULONG)(timeStamp & 0xffffffff);  
         *(((PULONG)&Nic->ManagementInfo.ScanChannelSwitchTime) + 1) = (ULONG)(timeStamp>>32);
#endif         
        Nic->ManagementInfo.ScanCurrentChannel = Nic->ManagementInfo.DestChannel;
    }
    else {
        Nic->ManagementInfo.OperatingPhyMIB->Channel = Nic->ManagementInfo.DestChannel;
    }

    //
    // Call the completion routine if provided.
    // There is need for a completion routine if OS calls
    // us (OID_DOT11_CURRENT_CHANNEL, START_REQUEST, JOIN_REQUEST)
    // but not if we are changing the channel ourselves (example
    // during a scan operation)
    //
    if (Nic->ManagementInfo.ChannelSwitchedHandler != NULL) {
        Nic->ManagementInfo.ChannelSwitchedHandler(Nic);
    }
}


VOID 
HwSwChnlWorkItem(
                __in WDFWORKITEM  WorkItem
                )
{
    PNIC Nic ;
    PWORKITEM_CONTEXT  workItemContext;

    workItemContext  = GetWorkItemContext(WorkItem);
    Nic = workItemContext->Nic;
    HwSwChnlWorker(Nic);

}


VOID
HwJoinTimeoutCallback(
                     __in  WDFTIMER  Timer
                     )
{
    PNIC                Nic;
    PTIMER_NIC_CONTEXT  timerNicContext;

    timerNicContext  = GetTimerContext(Timer);
    Nic = timerNicContext->Nic;

    MPVERIFY(HW_IS_IN_STA_ADHOC_MODE(Nic) == FALSE);

    //
    // Cancel the Join operation if it has not already completed
    //
    if (HW_STOP_WAITING_FOR_JOIN(Nic) == TRUE) {
        //
        // Restore the packet filter before failing Join
        //
        Hw11ResetFilterFunction(Nic);

        MpTrace(COMP_ASSOC, DBG_NORMAL, ("Cancelled the Join Operation as it took too long\n"));

       Sta11JoinCompleteInfra(Nic->Station, NDIS_STATUS_FAILURE);

    }
}


NDIS_STATUS
HwGetInfoIEWithOUI(
                  __in PUCHAR pucInfoBlob,
                  __in ULONG uSizeOfBlob,
                  __in UCHAR ucInfoId,
                  __out PUCHAR pucLength,
                  __deref_out_opt PVOID * ppvInfoEle,
                  __inout PULONG pOffset
                  )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_FAILURE;
    PDOT11_INFO_ELEMENT pInfoEleHdr = NULL;
    ULONG uRequiredSize = 0;

    *pucLength = 0;
    *ppvInfoEle = NULL;
    uSizeOfBlob -= *pOffset;
    pucInfoBlob += *pOffset;
    while (uSizeOfBlob) {
        pInfoEleHdr = (PDOT11_INFO_ELEMENT)pucInfoBlob;
        if (uSizeOfBlob < sizeof(DOT11_INFO_ELEMENT))
            break;

        uRequiredSize = (ULONG)(pInfoEleHdr->Length) + sizeof(DOT11_INFO_ELEMENT);
        if (uSizeOfBlob < uRequiredSize)
            break;

        *pOffset+=uRequiredSize;
        if (pInfoEleHdr->ElementID == ucInfoId) {
            *pucLength = pInfoEleHdr->Length;
            *ppvInfoEle = pucInfoBlob + sizeof(DOT11_INFO_ELEMENT);
            ndisStatus = NDIS_STATUS_SUCCESS;
            break;
        }

        uSizeOfBlob -= uRequiredSize;
        pucInfoBlob += uRequiredSize;
    }

    return ndisStatus;
}


BOOLEAN
HwAddIE(
       __in  PNIC     Nic,
       __in  UCHAR    ID,
       __in  UCHAR    Size,
       __in  PUCHAR   Data
       )
{
    //
    // This function only stores IE which has ID large than EID_IbssParms.
    // It can not be used to update IE.
    // To update IE, you must call HwRemoveIE() before this function
    //
    int index;

    //
    // Find the IE buffer to add the new IE.
    //
    for (index = 0; index < MAX_IE_NUMBER; index++) {
        if (Nic->ManagementInfo.IEs[index].bUse == FALSE) {      // Add IE if there is unused IE buffer
            if (ID == 0xdd) {
                Nic->ManagementInfo.WPAIECount++;
                Nic->ManagementInfo.IEs[index].OUI = *((PULONG)Data);
            }

            Nic->ManagementInfo.IEs[index].bUse=TRUE;
            Nic->ManagementInfo.IEs[index].ID=ID;
            Nic->ManagementInfo.IEs[index].Size=Size;
            NdisMoveMemory(Nic->ManagementInfo.IEs[index].Data, Data, Size);
            Nic->ManagementInfo.IEMap[ID] = (CHAR) index;

            return TRUE;
        }
    }

    return FALSE;
}

VOID
HwRemoveIE(
          __in  PNIC   Nic,
          __in  UCHAR  ID,
          __in  PUCHAR Data
          )
{
    int index;

    if (Nic->ManagementInfo.IEMap[ID] != -1) {    // Update IE(this ID already exist)
        if (ID != 0xdd) {        // Not WPA IE
            index=Nic->ManagementInfo.IEMap[ID];
            Nic->ManagementInfo.IEMap[ID] = -1;
            Nic->ManagementInfo.IEs[index].bUse = FALSE;
        }
        else {
            //
            // Search IE with ID and OUI
            //
            for (index = 0; index < MAX_IE_NUMBER; index++) {
                if (Nic->ManagementInfo.IEs[index].bUse == TRUE &&
                    Nic->ManagementInfo.IEs[index].ID == 0xdd && 
                    Nic->ManagementInfo.IEs[index].OUI == *((ULONG UNALIGNED *)Data)) {
                    Nic->ManagementInfo.WPAIECount--;
                    Nic->ManagementInfo.IEs[index].bUse = FALSE;

                    if (Nic->ManagementInfo.WPAIECount == 0) {
                        Nic->ManagementInfo.IEMap[ID] = -1;
                        break;
                    }
                }
            }
        }
    }
}

PUCHAR
HwGetIE(
       __in  PNIC               Nic,
       __in  UCHAR              ID,
       __in  UCHAR              SubIEIndex,
       __out PUCHAR             Size
       )
{
    int index;

    if (Nic->ManagementInfo.IEMap[ID] == -1)
        return NULL;

    if (ID != 0xdd)
        index = Nic->ManagementInfo.IEMap[ID];
    else {
        if (SubIEIndex >= Nic->ManagementInfo.WPAIECount)
            return NULL;

        for (index = 0; index < MAX_IE_NUMBER; index++) {
            if (Nic->ManagementInfo.IEs[index].bUse == TRUE && Nic->ManagementInfo.IEs[index].ID == 0xdd) {
                if (SubIEIndex == 0)
                    break;

                SubIEIndex--;
            }
        }
    }

    MPASSERTMSG("Get IE Error", index < MAX_IE_NUMBER);
    MPASSERTMSG("Get IE Error", Nic->ManagementInfo.IEs[index].bUse == TRUE && Nic->ManagementInfo.IEs[index].ID == ID);

    if (index >= MAX_IE_NUMBER)
    {
        ASSERT(FALSE);
        return NULL;
    }
    
    *Size = Nic->ManagementInfo.IEs[index].Size;
    return Nic->ManagementInfo.IEs[index].Data;
}

VOID
HwClearAllIE(
            __in  PNIC        Nic
            )
{
    int i;

    for ( i = 0; i < MAX_IE_NUMBER; i++)
        Nic->ManagementInfo.IEs[i].bUse = FALSE;

    Nic->ManagementInfo.WPAIECount = 0;

    //
    // Set all Map to -1(unused)
    //
    NdisFillMemory(Nic->ManagementInfo.IEMap, sizeof(Nic->ManagementInfo.IEMap), 0xff);
}

BOOLEAN
HwSendNullPkt(
             __in  PNIC   Nic
             )
{
    PUCHAR                  TxBuf;
    GeneralPacketPartial   *pNullPkt;

    if (HwGetTxFreeDescNum(Nic, NORMAL_QUEUE) > 0) {
        //
        // Save Desc pointer, so that we can check send status of this packet
        //
        Nic->ManagementInfo.pPowerMgmtChangePktDesc= (PTX_DESC_8187)HwNormalQGetNextToSendBuffer(Nic)/*HwGetNextDescToSend(Nic, NORMAL_QUEUE, 0)*/;
        TxBuf=HwGetHWDESCAdjustedBuffer(Nic->ManagementInfo.pPowerMgmtChangePktDesc);

        //
        // Setup NULL Packet
        //
        pNullPkt = (GeneralPacketPartial *) TxBuf;

        pNullPkt->FrameCtrl.CharData[0] = Type_Null_Frame;
        pNullPkt->FrameCtrl.Field.ToDS= 1;
        pNullPkt->Duration = 0;
        NdisMoveMemory(pNullPkt->Addr1, Nic->ManagementInfo.BssId, 6);
        ETH_COPY_NETWORK_ADDRESS(pNullPkt->Addr2, Nic->CardInfo.MacAddr);
        NdisMoveMemory(pNullPkt->Addr3, Nic->ManagementInfo.BssId, 6);
        pNullPkt->Seq = 0;

        HwTransmitNextPacketWithLocalBuffer(Nic,
                                            NORMAL_QUEUE,
                                            24,
                                            Nic->ManagementInfo.CurrentBeaconRate,
                                            FALSE,
                                            (PUCHAR)Nic->ManagementInfo.pPowerMgmtChangePktDesc);

        return TRUE;
    }
    else
        return FALSE;
}

BOOLEAN
HwSendPSPoll(
            __in  PNIC Nic
            )
{
    PUCHAR                  pDesc;
    PUCHAR                  TxBuf;
    GeneralPacketPartial   *pPSPoll;

    if (HwGetTxFreeDescNum(Nic, NORMAL_QUEUE) > 0) {
        pDesc =  HwNormalQGetNextToSendBuffer(Nic);
        TxBuf=HwGetHWDESCAdjustedBuffer(pDesc);

        //
        // Setup PS-Poll Packet
        //
        pPSPoll = (GeneralPacketPartial *) TxBuf;

        pPSPoll->FrameCtrl.CharData[0] = Type_PS_poll;
        pPSPoll->FrameCtrl.CharData[1] = 0;
        pPSPoll->Duration = Nic->ManagementInfo.PowerMgmtMode.usAID | 0xC000;
        NdisMoveMemory(pPSPoll->Addr1, Nic->ManagementInfo.BssId, 6);
        ETH_COPY_NETWORK_ADDRESS(pPSPoll->Addr2, Nic->CardInfo.MacAddr);

        HwTransmitNextPacketWithLocalBuffer(Nic,
                                            NORMAL_QUEUE,
                                            16,
                                            Nic->ManagementInfo.CurrentBeaconRate,
                                            TRUE,
                                            pDesc);

        return TRUE;
    }
    else
        return FALSE;
}

ULONG
HwComputeCrc(
            __in  PUCHAR Buffer,
            __in  UINT Length
            )
{
    ULONG Crc, Carry;
    UINT i, j;
    UCHAR CurByte;

    Crc = 0xffffffff;

    for (i = 0; i < Length; i++) {
        CurByte = Buffer[i];
        for (j = 0; j < 8; j++) {
            Carry = ((Crc & 0x80000000) ? 1 : 0) ^ (CurByte & 0x01);
            Crc <<= 1;
            CurByte >>= 1;
            if (Carry) {
                Crc = (Crc ^ 0x04c11db6) | Carry;
            }
        }
    }
    Crc = Crc & 0x0ff000000;                    // 95.05.04     Sid
    return Crc;
}

VOID
HwGetMulticastBit(
                 __in  PUCHAR Address,
                 __out PUCHAR Byte,
                 __out PUCHAR Value
                 )
{
    ULONG     Crc;
    UINT     BitNumber;

    Crc = HwComputeCrc(Address, ETH_LENGTH_OF_ADDRESS);
    //
    // The bit number is now in the 6 most significant bits of CRC.
    //
    BitNumber = (UINT)((Crc >> 26) & 0x3f);
    *Byte = (UCHAR)(BitNumber / 8);
    *Value = (UCHAR)((UCHAR)1 << (BitNumber % 8));
}


NDIS_STATUS
HwSetMulticastMask(
                  __in  PNIC       Nic,
                  __in  BOOLEAN    bAcceptAll
                  )
{
    UINT     i;
    UCHAR    Byte;
    UCHAR    Bit;
    ULONG    MultiCast_03;
    ULONG    MultiCast_47;
    ULONG    AddressCount = Nic->RxInfo.MCAddressCount;
    UCHAR    ExceptMAddress[6] = {0x01, 0x40, 0x96, 0x00, 0x00, 0x00};

    if (bAcceptAll) {
        HwPlatformIOWrite4Byte( Nic, MAR0, 0xffffffff );
        HwPlatformIOWrite4Byte( Nic, MAR4, 0xffffffff );
    }
    else {
        //
        // Clear all bits
        //
        NdisZeroMemory(Nic->RxInfo.MCRegValues, 8);

        //
        // Now turn on the bit for each address in the multicast list.
        //
        for (i = 0; (i < AddressCount) && (i < HW11_MAX_MCAST_LIST_SIZE); i++) {
            HwGetMulticastBit(Nic->RxInfo.MCAddressList[i], &Byte, &Bit);
            Nic->RxInfo.MCRegValues[Byte] |= Bit;
        }

        //
        // Turn Except Multicast Address
        //
        HwGetMulticastBit(ExceptMAddress, &Byte, &Bit);    // For Cisco to get IP address
        Nic->RxInfo.MCRegValues[Byte] |= Bit;

        //
        // Write Multicast bits to register
        //
        MultiCast_03 = 0;
        MultiCast_47 = 0;

        for (i = 0; i < 4; i++)
            MultiCast_03 = MultiCast_03 + ((Nic->RxInfo.MCRegValues[i])<<(8*i));

        for (; i<8; i++)
            MultiCast_47 = MultiCast_47 + ((Nic->RxInfo.MCRegValues[i])<<(8*(i-4)));

        HwPlatformIOWrite4Byte(Nic, MAR0, MultiCast_03);
        HwPlatformIOWrite4Byte(Nic, MAR4, MultiCast_47);
    }

    return NDIS_STATUS_SUCCESS;
}

VOID
HwDoze(
      __in  PNIC     Nic,
      __in  ULONG    ulDozeTime)       // in micro-second
{
    LARGE_INTEGER CurrentTime;
    ULONGLONG TimeDifference;

    if(Nic->ManagementInfo.bScanInProgress)
    {
        return;
    }
    
    //
    // Shorten the doze time by 15ms. This accommodates the radio switch on
    // time and kernel DPC timer inaccuracy.
    //
    ulDozeTime = ulDozeTime / 1000;    // in ms
    if (ulDozeTime > 20)
        ulDozeTime -= 15;
    else
        return;

    //
    // Check if we can NOT sleep (due to uDelaySleepValue)
    //
    if (Nic->ManagementInfo.DelaySleepValue > 0) {
        NdisGetCurrentSystemTime(&CurrentTime);

        //
        // TimeDifference is in micro-second.
        //
        TimeDifference = (CurrentTime.QuadPart - Nic->ManagementInfo.DelaySleepTimeStamp.QuadPart) / 10;
        if (TimeDifference > Nic->ManagementInfo.DelaySleepValue)
            Nic->ManagementInfo.DelaySleepValue = 0;
        else
            return;
    }

    if (Nic->ManagementInfo.bPowerMgmtChangePktSent) {

        //
        // Make sure a NULL packet with power management set is sent out
        //
        if (Nic->RfInfo.RFPowerState == eRfOn) {

            //
            // Make sure we are not already in PS mode
            //
            if (!Nic->ManagementInfo.bScanInProgress && HwGetTxBusyDescNum(Nic, LOW_QUEUE) == 0) {

                //NdisMSetTimer(&Nic->ManagementInfo.Timer_Awake, ulDozeTime);
                WdfTimerStart(Nic->ManagementInfo.Timer_Awake, WDF_REL_TIMEOUT_IN_MS(ulDozeTime));

                MpTrace(COMP_POWER, DBG_LOUD, (" *** RF OFF for %dms\n", ulDozeTime));

                if (Nic->ManagementInfo.PowerMgmtMode.uPowerSaveLevel == DOT11_POWER_SAVE_LEVEL_FAST_PSP)
                    SetRFPowerState(Nic, eRfSleep);
                else if (Nic->ManagementInfo.PowerMgmtMode.uPowerSaveLevel == DOT11_POWER_SAVE_LEVEL_MAX_PSP) {
                    //TODO: Find a right way to make this chip turn off quickly
                        if(Nic->RfInfo.RFChipID==10)
                            HwSetRFState(Nic, eRfSleep);
                        else
                            HwSetRFState(Nic, eRfOff);

                }                    
            }
        }
    }
}

// Called at PASSIVE or DISPATCH
BOOLEAN
HwSetRFOn(
    __in PNIC     pNic,
    __in UCHAR    MaxRetries
    )
{
    UCHAR   RetryCount = 0;
    
    //
    // This is at dispatch, dont spin for long
    //
    while (RetryCount < MaxRetries)
    {
        if (HwSetRFState(pNic, eRfOn) == FALSE)
        {
            //
            // If radio is OFF, hw awake can fail. It can also fail
            // if something else is changing radio state, in which case, we try
            // try again
            //
            if (pNic->RfInfo.RadioOff == TRUE)
            {
                //
                // Failed because radio was OFF
                //
                break;
            }
            else
            {
                //
                // Something else is toggling RF state
                //
                NdisStallExecution(10); // Stall for 10 microseconds
            }
        }
        else
        {
            //
            // Hw awake OK
            //
            return TRUE;
        }
        RetryCount++;
    }

    return FALSE;
}


BOOLEAN
HwAwake(
       __in  PNIC Nic
       )
{
    BOOLEAN        cancelled;

    if (Nic->RfInfo.RFPowerState != eRfOn) {
        MpTrace(COMP_POWER, DBG_LOUD, (" *** RF ON\n"));

        //
        // Try to cancel timer in case we are not called by timer this time
        //
        cancelled = WdfTimerStop(Nic->ManagementInfo.Timer_Awake, FALSE);
        //
        // Enable RF and retry 3 times if something else is using it
        //
        return HwSetRFOn(Nic, 3);
    }
    
    return TRUE;
}

VOID
HwAwakeCallback(
               __in WDFTIMER  Timer
               )
{
    PNIC                pNic;
    PTIMER_NIC_CONTEXT  timerNicContext;

    timerNicContext  = GetTimerContext(Timer);
    pNic = timerNicContext->Nic;   
    HwAwake(pNic);
}

void
Hw11DeleteNonPersistentKey(
                          __in  PNIC Nic
                          )
{
    PNICKEY             pNicKey;
    UCHAR               index;
    UCHAR               index2;
    PNIC_PER_STA_KEY    pPerStaKey;
    BOOLEAN             noValidKeys;

    //
    // Go through the key table, invalidate any key which is not persistent. 
    //
    pNicKey = Nic->ManagementInfo.KeyTable;
    for (index = 0; index < KEY_TABLE_SIZE; index++, pNicKey++) {
        if (pNicKey->Valid && !pNicKey->Persistent) {
            NdisZeroMemory(pNicKey, sizeof(NICKEY));
            HwRemoveKeyEntry(Nic, index, pNicKey->AlgoId);           

        }
    }

    //
    // Invalidate any per-STA key which is not persistent. 
    //
    pPerStaKey = Nic->ManagementInfo.PerStaKey;
    for (index = 0; index < PER_STA_KEY_TABLE_SIZE; index++, pPerStaKey++) {
        if (pPerStaKey->Valid) {
            noValidKeys = TRUE;
            pNicKey = pPerStaKey->NicKey;
            for (index2 = 0; index2 < DOT11_MAX_NUM_DEFAULT_KEY; index2++, pNicKey++) {
                if (pNicKey->Valid) {
                    if (!pNicKey->Persistent) {
                        NdisZeroMemory(pNicKey, sizeof(NICKEY));
                    }
                    else {
                        noValidKeys = FALSE;
                    }
                }
            }

            if (noValidKeys) {
                pPerStaKey->Valid = FALSE;
                ASSERT(Nic->ManagementInfo.PerStaKeyCount > 0);
                Nic->ManagementInfo.PerStaKeyCount--;
            }
        }
    }
}

void
Hw11DeleteNonPersistentMappingKey(
                                 __in  PNIC Nic,
                                 __in  DOT11_MAC_ADDRESS MacAddr
                                 )
{
    PNICKEY             pNicKey;
    UCHAR               index;
    UCHAR               index2;
    PNIC_PER_STA_KEY    pPerStaKey;
    BOOLEAN             noValidKeys;

    //
    // Go through the key mapping key table, invalidate any matching key which is not persistent. 
    //
    pNicKey = Nic->ManagementInfo.KeyTable;
    for (index = DOT11_MAX_NUM_DEFAULT_KEY; index < KEY_TABLE_SIZE; index++, pNicKey++) {
        if (pNicKey->Valid && 
            !pNicKey->Persistent && 
            NdisEqualMemory(pNicKey->MacAddr, MacAddr, sizeof(DOT11_MAC_ADDRESS)) == 1) {
            NdisZeroMemory(pNicKey, sizeof(NICKEY));
            HwRemoveKeyEntry(Nic, index, pNicKey->AlgoId);

        }
    }

    //
    // Invalidate any matching per-STA key which is not persistent. 
    //
    pPerStaKey = Nic->ManagementInfo.PerStaKey;
    for (index = 0; index < PER_STA_KEY_TABLE_SIZE; index++, pPerStaKey++) {
        if (pPerStaKey->Valid &&
            NdisEqualMemory(pPerStaKey->StaMacAddr, MacAddr, sizeof(DOT11_MAC_ADDRESS)) == 1) {
            noValidKeys = TRUE;
            pNicKey = pPerStaKey->NicKey;
            for (index2 = 0; index2 < DOT11_MAX_NUM_DEFAULT_KEY; index2++, pNicKey++) {
                if (pNicKey->Valid) {
                    if (!pNicKey->Persistent) {
                        NdisZeroMemory(pNicKey, sizeof(NICKEY));
                    }
                    else {
                        noValidKeys = FALSE;
                    }
                }
            }

            if (noValidKeys) {
                pPerStaKey->Valid = FALSE;
                ASSERT(Nic->ManagementInfo.PerStaKeyCount > 0);
                Nic->ManagementInfo.PerStaKeyCount--;
            }
        }
    }
}

void
HwDeleteAllKeys(
               __in  PNIC Nic
               )
{
    PNICKEY     pNicKey;
    UCHAR       index;

    //
    // Clear key table and hardware key setting.
    //
    pNicKey = Nic->ManagementInfo.KeyTable;
    for (index = 0; index < KEY_TABLE_SIZE; index++, pNicKey++) {
        NdisZeroMemory(pNicKey, sizeof(NICKEY));
        HwRemoveKeyEntry(Nic, index, pNicKey->AlgoId);
       
    }

    Nic->ManagementInfo.KeyMappingKeyCount = 0;

    //
    // Clear per-Sta key table.
    //
    for (index = 0; index < PER_STA_KEY_TABLE_SIZE; index++) {
        NdisZeroMemory(Nic->ManagementInfo.PerStaKey + index, sizeof(NIC_PER_STA_KEY));
    }

    Nic->ManagementInfo.PerStaKeyCount = 0;
}

void 
Hw11SetDefaultKeyID(
                   __in  PNIC Nic,
                   __in  ULONG keyID
                   )
{
    ASSERT(keyID < DOT11_MAX_NUM_DEFAULT_KEY);
    Nic->ManagementInfo.DefaultKeyID = (UCHAR)keyID;
}

ULONG
Hw11GetDefaultKeyID(
                   __in  PNIC Nic
                   )
{
    return Nic->ManagementInfo.DefaultKeyID;
}


USHORT
Hw11GetRSNCapability(
                    __in  PNIC Nic
                    )
{
    UNREFERENCED_PARAMETER(Nic);
    return 0;
}

void
Hw11SetAuthentication(
                     __in  PNIC Nic,
                     __in  DOT11_AUTH_ALGORITHM  algoId
                     )
{
    Nic->ManagementInfo.HwAuthAlgorithm = algoId;
}

void
Hw11SetCurrentBSSType(
                     __in  PNIC Nic,
                     __in  DOT11_BSS_TYPE  type
                     )
{
    Nic->ManagementInfo.CurrentBSSType = type;
}

NDIS_STATUS
Hw11SetSafeModeOption(
    __in  PNIC            pNic,
    __in  BOOLEAN         safeModeEnabled
    )
{
    if (safeModeEnabled) {
        //
        // We would program the hardware to not do any encryption (both unicast and multicast ciphers)
        //
        Hw11SetEncryption(pNic, TRUE, DOT11_CIPHER_ALGO_NONE);
        Hw11SetEncryption(pNic, FALSE, DOT11_CIPHER_ALGO_NONE);
    }
    else {
        //
        // The hardware would be programmed when the OS sets the ciphers
        //
        MPVERIFY(pNic->ManagementInfo.HwUnicastCipher == DOT11_CIPHER_ALGO_NONE);
    }

    pNic->ManagementInfo.SafeModeEnabled = safeModeEnabled;
    
    return NDIS_STATUS_SUCCESS;
}

BOOLEAN
Hw11GetSafeModeOption(
    __in  PNIC            pNic 
    )
{
    return pNic->ManagementInfo.SafeModeEnabled;
}

void
Hw11SetEncryption(
                 __in  PNIC Nic,
                 __in  BOOLEAN isUnicast,
                 __in  DOT11_CIPHER_ALGORITHM  algoId
                 )
{
    BOOLEAN useDefaultKey;

    MpTrace(COMP_OID, DBG_LOUD, ("Hw11SetEncrytion: %s %d\n", isUnicast ? "Unicast" : "Multicast", algoId));

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
    // Update the encryption algorithm. When set unicast cipher, also clear all keys. 
    //
    if (isUnicast) {
        HwDeleteAllKeys(Nic);
        Nic->ManagementInfo.HwUnicastCipher = algoId;

        //
        // Program the hardware
        //
        if (Nic->ManagementInfo.SafeModeEnabled == FALSE) {
            if (Nic->CardInfo.HardwareID == HW_ID_8187) {
                //
                // Don't use default key for WPA2PSK in adhoc mode.
                //
                useDefaultKey = (BOOLEAN)(!(Nic->ManagementInfo.CurrentBSSType == dot11_BSS_type_independent &&
                                            Nic->ManagementInfo.HwAuthAlgorithm == DOT11_AUTH_ALGO_RSNA_PSK));
                SetEncryption8187(Nic, algoId, useDefaultKey);
            }
        }
    }
    else {
        Nic->ManagementInfo.HwMulticastCipher = algoId;
    }
}

NDIS_STATUS
HwSetKey(
        __in  PNIC Nic,
        __in  PNICKEY pNicKey,
        __in  UCHAR keyIndex,
        __in  BOOLEAN perStaKey,
        __in  BOOLEAN persistent,
        __in  DOT11_CIPHER_ALGORITHM algoId,
        __in  ULONG keyLength,
        __in  PUCHAR keyValue
        )
{
    ULONG                       i;
    PDOT11_KEY_ALGO_CCMP        CCMPKey = NULL;
    PDOT11_KEY_ALGO_TKIP_MIC    TKIPKey = NULL;

    //
    // If keyLength is non-zero, the key is to be added, otherwise, it is to be deleted.
    // Note: MacAddr of the key is already set.
    //

    if (keyLength != 0) {
        //
        // If the algoId is generic WEP, change it to WEP40 or WEP104 depending on the key length.
        //

        switch (algoId) {
            case DOT11_CIPHER_ALGO_WEP:
                switch (keyLength) {
                    case 40 / 8:
                        algoId = DOT11_CIPHER_ALGO_WEP40;
                        break;

                    case 104 / 8:
                        algoId = DOT11_CIPHER_ALGO_WEP104;
                        break;

                    default:
                        return NDIS_STATUS_INVALID_DATA;
                }

                //
                // fall through
                //

            case DOT11_CIPHER_ALGO_WEP104:
            case DOT11_CIPHER_ALGO_WEP40:
                break;

            case DOT11_CIPHER_ALGO_CCMP:
                //
                // Validate the key length
                //
                if (keyLength < FIELD_OFFSET(DOT11_KEY_ALGO_CCMP, ucCCMPKey)) 
                {
                    return NDIS_STATUS_INVALID_DATA;
                }

                CCMPKey = (PDOT11_KEY_ALGO_CCMP)keyValue;
                                if (keyLength < FIELD_OFFSET(DOT11_KEY_ALGO_CCMP, ucCCMPKey) + 
                                CCMPKey->ulCCMPKeyLength)
                {
                    return NDIS_STATUS_INVALID_DATA;
                }

                //
                // Only support 16-byte CCMP key 
                //
                if (CCMPKey->ulCCMPKeyLength != 16)
                {
                    return NDIS_STATUS_INVALID_DATA;
                }

                keyLength = CCMPKey->ulCCMPKeyLength;
                keyValue = CCMPKey->ucCCMPKey;
                break;

            case DOT11_CIPHER_ALGO_TKIP:
                //
                // Validate the key length
                //
                if (keyLength < FIELD_OFFSET(DOT11_KEY_ALGO_TKIP_MIC, ucTKIPMICKeys)) 
                {
                    return NDIS_STATUS_INVALID_DATA;
                }
                

                TKIPKey = (PDOT11_KEY_ALGO_TKIP_MIC)keyValue;
                                if (keyLength < FIELD_OFFSET(DOT11_KEY_ALGO_TKIP_MIC, ucTKIPMICKeys) + 
                                TKIPKey->ulTKIPKeyLength +
                                TKIPKey->ulMICKeyLength) 
                {
                    return NDIS_STATUS_INVALID_DATA;
                }
                
                //
                // Only support 16-byte TKIP key and 8-byte Tx/Rx MIC key
                //
                if (TKIPKey->ulTKIPKeyLength != 16 || TKIPKey->ulMICKeyLength != 16)
                {
                    return NDIS_STATUS_INVALID_DATA;
                }

                keyLength = TKIPKey->ulTKIPKeyLength;
                keyValue = TKIPKey->ucTKIPMICKeys;
                break;
        }

        //
        // The real key length cannot be larger than our register length for a key.
        // Also, for TKIP, we only support 8-byte Tx MIC key and 8-byte Rx MIC key 
        //
        if (keyLength > sizeof(pNicKey->KeyValue) ||
            algoId == DOT11_CIPHER_ALGO_TKIP && TKIPKey->ulMICKeyLength != 16) {
            return NDIS_STATUS_INVALID_DATA;
        }

        //
        // If the current encryption algorithm is WEP, change it to more specific WEP40 or WEP104.
        //

        if (Nic->ManagementInfo.HwUnicastCipher == DOT11_CIPHER_ALGO_WEP &&
            (algoId == DOT11_CIPHER_ALGO_WEP40 || algoId == DOT11_CIPHER_ALGO_WEP104)) {
            Nic->ManagementInfo.HwUnicastCipher = algoId;

            //
            // For RTL8180, also program the hardware
            //

        }

        if (Nic->ManagementInfo.HwMulticastCipher == DOT11_CIPHER_ALGO_WEP &&
            (algoId == DOT11_CIPHER_ALGO_WEP40 || algoId == DOT11_CIPHER_ALGO_WEP104)) {
            Nic->ManagementInfo.HwMulticastCipher = algoId;
        }

        //
        // We should never have unicast and multicast cipher with different length of WEP.
        // See comment in function Hw11SetEncryption().
        //
        ASSERT(!(Nic->ManagementInfo.HwUnicastCipher == DOT11_CIPHER_ALGO_WEP40 && 
                 Nic->ManagementInfo.HwMulticastCipher == DOT11_CIPHER_ALGO_WEP104));

        ASSERT(!(Nic->ManagementInfo.HwUnicastCipher == DOT11_CIPHER_ALGO_WEP104 && 
                 Nic->ManagementInfo.HwMulticastCipher == DOT11_CIPHER_ALGO_WEP40));

        //
        // For key mapping key, its algorithm must match current unicast cipher (unless 
        // the key is for multicast/broadcast data frames).
        //
        // For per-STA key, or key mapping key for multicast/broadcast data frames, 
        // its algorithm must match the current unicast cipher.
        //
        // For default key, its algorithm must match either the current unicast cipher
        // or the current multicast cipher. 
        //
        if (Nic->ManagementInfo.HwUnicastCipher != algoId && 
            keyIndex >= DOT11_MAX_NUM_DEFAULT_KEY && 
            ETH_IS_UNICAST(pNicKey->MacAddr)) {
            return NDIS_STATUS_INVALID_DATA;
        }

        if (Nic->ManagementInfo.HwMulticastCipher != algoId && 
            (perStaKey || ETH_IS_MULTICAST(pNicKey->MacAddr))) {
            return NDIS_STATUS_INVALID_DATA;
        }

        if (Nic->ManagementInfo.HwUnicastCipher != algoId && 
            Nic->ManagementInfo.HwMulticastCipher != algoId) {
            return NDIS_STATUS_INVALID_DATA;
        }

        //
        // Update the key entry for valid key. We cannot fail after this point. Otherwise, 
        // the existing key will be modified when we fail to set the new key.
        //

        pNicKey->Valid = TRUE;
        pNicKey->Persistent = persistent;
        pNicKey->KeyLength = (UCHAR) keyLength;
        pNicKey->AlgoId = algoId;

#if DBG
        if (keyIndex < DOT11_MAX_NUM_DEFAULT_KEY) {
            MpTrace(COMP_OID, DBG_NORMAL, ("Update %s key %d (algo %d): ", 
                                           perStaKey ? "per-STA" : "default", keyIndex, algoId));
        }
        else {
            MpTrace(COMP_OID, DBG_NORMAL, ("Update key mapping key for %02X-%02X-%02X-%02X-%02X-%02X (algo %d): ", 
                                           pNicKey->MacAddr[0], pNicKey->MacAddr[1], pNicKey->MacAddr[2], 
                                           pNicKey->MacAddr[3], pNicKey->MacAddr[4], pNicKey->MacAddr[5], algoId));
        }
#endif

        for (i = 0; i < keyLength; i++) {
            pNicKey->KeyValue[i] = keyValue[i];
            MpTrace(COMP_OID, DBG_NORMAL, ("%02X", keyValue[i]));
        }
        MpTrace(COMP_OID, DBG_NORMAL, ("\n"));

        for (i = keyLength; i < sizeof(pNicKey->KeyValue); i++)
            pNicKey->KeyValue[i] = 0;

        switch (algoId) {
            case DOT11_CIPHER_ALGO_WEP:
            case DOT11_CIPHER_ALGO_WEP104:
            case DOT11_CIPHER_ALGO_WEP40:
                pNicKey->IV = 1;
                break;

            case DOT11_CIPHER_ALGO_CCMP:
                pNicKey->PN = 1;
                pNicKey->ReplayCounter = ((ULONGLONG)CCMPKey->ucIV48Counter[0]) |
                                         (((ULONGLONG)CCMPKey->ucIV48Counter[1]) << 8) |
                                         (((ULONGLONG)CCMPKey->ucIV48Counter[2]) << 16) |
                                         (((ULONGLONG)CCMPKey->ucIV48Counter[3]) << 24) |
                                         (((ULONGLONG)CCMPKey->ucIV48Counter[4]) << 32) |
                                         (((ULONGLONG)CCMPKey->ucIV48Counter[5]) << 40);
                break;

            case DOT11_CIPHER_ALGO_TKIP:
                pNicKey->TSC = 1;

                pNicKey->ReplayCounter = ((ULONGLONG)TKIPKey->ucIV48Counter[0]) |
                                         (((ULONGLONG)TKIPKey->ucIV48Counter[1]) << 8) |
                                         (((ULONGLONG)TKIPKey->ucIV48Counter[2]) << 16) |
                                         (((ULONGLONG)TKIPKey->ucIV48Counter[3]) << 24) |
                                         (((ULONGLONG)TKIPKey->ucIV48Counter[4]) << 32) |
                                         (((ULONGLONG)TKIPKey->ucIV48Counter[5]) << 40);
                NdisMoveMemory(pNicKey->RxMICKey, Add2Ptr(keyValue, keyLength), 8);
                NdisMoveMemory(pNicKey->TxMICKey, Add2Ptr(keyValue, keyLength + 8), 8);
                break;
        }

        //
        // Program the hardware.
        //
        HwAddKeyEntry(Nic, pNicKey, keyIndex);
    }
    else {
        //
        // Remove the key from hardware.
        //
        NdisZeroMemory(pNicKey, sizeof(NICKEY));
        if (!perStaKey) {
            HwRemoveKeyEntry(Nic, keyIndex, pNicKey->AlgoId);
        }
       
    }

    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS 
HwSetPerStaDefaultKey(
                     __in  PNIC Nic,
                     __in  DOT11_MAC_ADDRESS MacAddr,
                     __in  ULONG keyID,
                     __in  BOOLEAN persistent,
                     __in  DOT11_CIPHER_ALGORITHM algoId,
                     __in  ULONG keyLength,
                     __in  PUCHAR keyValue
                     )
{
    PNIC_PER_STA_KEY    pPerStaKey;
    ULONG               index;
    ULONG               emptyIndex;
    NDIS_STATUS         ndisStatus;

    //
    // per-STA default key is only supported by RTL8185. Actually even RTL8185
    // doesn't support it in hardware, but we support WPA2PSK for adhoc on RTL8185, 
    // so we just have to support per-STA default key in software.
    //
    ASSERT(Nic->CardInfo.HardwareID == HW_ID_8187);
    ASSERT(Nic->ManagementInfo.HwAuthAlgorithm == DOT11_AUTH_ALGO_RSNA_PSK);
    if (Nic->CardInfo.HardwareID != HW_ID_8187 ||
        Nic->ManagementInfo.HwAuthAlgorithm != DOT11_AUTH_ALGO_RSNA_PSK) {
        return NDIS_STATUS_NOT_SUPPORTED;
    }

    //
    // Search the per-STA key table to find either a matching MacAddr or an empty key entry.
    //
    emptyIndex = PER_STA_KEY_TABLE_SIZE;
    pPerStaKey = Nic->ManagementInfo.PerStaKey;
    for (index = 0; index < PER_STA_KEY_TABLE_SIZE; index++, pPerStaKey++) {
        if (pPerStaKey->Valid && NdisEqualMemory(pPerStaKey->StaMacAddr, MacAddr, sizeof(DOT11_MAC_ADDRESS)) == 1) {
            break;
        }

        if (!pPerStaKey->Valid && emptyIndex == PER_STA_KEY_TABLE_SIZE) {
            emptyIndex = index;
        }
    }

    if (index == PER_STA_KEY_TABLE_SIZE) {
        //
        // We did not find a per-STA key with matching MacAddr. 
        //

        if (keyLength == 0) {
            //
            // If we are asked to delete a per-STA key, fail the request.
            //
            return NDIS_STATUS_INVALID_DATA;
        }
        else if (emptyIndex == PER_STA_KEY_TABLE_SIZE) {
            //
            // If we are asked to add a key but the table is full, fail the request.
            //
            return NDIS_STATUS_RESOURCES;
        }

        pPerStaKey = Nic->ManagementInfo.PerStaKey + emptyIndex;
        ETH_COPY_NETWORK_ADDRESS(pPerStaKey->StaMacAddr, MacAddr);
    }

    //
    // Set the key in NicKey structure.
    //
    ndisStatus = HwSetKey(Nic, 
                          pPerStaKey->NicKey + keyID, 
                          (UCHAR)keyID, 
                          TRUE,
                          persistent, 
                          algoId, 
                          keyLength, 
                          keyValue);

    //
    // If key is set successfully, update the "Valid" flag of the per-Sta key.
    //
    if (ndisStatus == NDIS_STATUS_SUCCESS) {
        if (keyLength != 0) {
            if (!pPerStaKey->Valid) {
                pPerStaKey->Valid = TRUE;
                ASSERT(Nic->ManagementInfo.PerStaKeyCount < PER_STA_KEY_TABLE_SIZE);
                Nic->ManagementInfo.PerStaKeyCount++;
            }
        }
        else {
            ASSERT(pPerStaKey->Valid);
            for (index = 0; index < DOT11_MAX_NUM_DEFAULT_KEY; index++) {
                if (pPerStaKey->NicKey[index].Valid)
                    break;
            }

            if (index == DOT11_MAX_NUM_DEFAULT_KEY) {
                pPerStaKey->Valid = FALSE;
                ASSERT(Nic->ManagementInfo.PerStaKeyCount > 0);
                Nic->ManagementInfo.PerStaKeyCount--;
            }
        }
    }

    return ndisStatus;
}

NDIS_STATUS 
Hw11SetDefaultKey(
                 __in  PNIC Nic,
                 __in  DOT11_MAC_ADDRESS MacAddr,
                 __in  ULONG keyID,
                 __in  BOOLEAN persistent,
                 __in  DOT11_CIPHER_ALGORITHM algoId,
                 __in  ULONG keyLength,
                 __in  PUCHAR keyValue
                 )
{
    //
    // Note that both this function and other functions accessing key table are called
    // as a result of OID set. As such, no locking is needed since NDIS serializes
    // the OID set operation. If this premise is no longer valid, locking will
    // be needed.
    //

    ASSERT(keyID < DOT11_MAX_NUM_DEFAULT_KEY);
    if (keyID >= DOT11_MAX_NUM_DEFAULT_KEY)
        return NDIS_STATUS_INVALID_DATA;

    if (Nic->ManagementInfo.CurrentBSSType == dot11_BSS_type_independent && (MacAddr[0] != 0 || 
                                                                             MacAddr[1] != 0 || 
                                                                             MacAddr[2] != 0 || 
                                                                             MacAddr[3] != 0 ||
                                                                             MacAddr[4] != 0 ||
                                                                             MacAddr[5] != 0)) {
        //
        // Handle the per-STA default key.
        //
        ASSERT(ETH_IS_UNICAST(MacAddr));
        return HwSetPerStaDefaultKey(Nic,
                                     MacAddr,
                                     keyID,
                                     persistent,
                                     algoId,
                                     keyLength,
                                     keyValue);
    }
    else {
        //
        // Save the new default key in the key table. 
        //
        return HwSetKey(Nic, 
                        Nic->ManagementInfo.KeyTable + keyID,
                        (UCHAR)keyID, 
                        FALSE,
                        persistent, 
                        algoId, 
                        keyLength, 
                        keyValue);
    }
}

NDIS_STATUS 
Hw11SetKeyMappingKey(
                    __in  PNIC Nic,
                    __in  DOT11_MAC_ADDRESS MacAddr,
                    __in  DOT11_DIRECTION direction,
                    __in  BOOLEAN persistent,
                    __in  DOT11_CIPHER_ALGORITHM algoId,
                    __in  ULONG keyLength,
                    __in  PUCHAR keyValue
                    )
{
    PNICKEY pNicKey = NULL;
    ULONG   index;
    ULONG   emptyIndex;

    //
    // We don't support uni-direction key mapping keys
    //
    if (direction != DOT11_DIR_BOTH) {
        return NDIS_STATUS_NOT_SUPPORTED;
    }

    //
    // Search the key mapping table to find either a matching MacAddr or an empty key entry.
    //
    emptyIndex = 0;
    for (index = DOT11_MAX_NUM_DEFAULT_KEY; index < KEY_TABLE_SIZE; index++) {
        pNicKey = Nic->ManagementInfo.KeyTable + index;
        if (pNicKey->Valid && NdisEqualMemory(pNicKey->MacAddr, MacAddr, sizeof(DOT11_MAC_ADDRESS)) == 1) {
            break;
        }

        if (!pNicKey->Valid && emptyIndex == 0) {
            emptyIndex = index;
        }
    }

    if (index == KEY_TABLE_SIZE) {
        //
        // We did not find a key with matching MacAddr. 
        //

        if (keyLength == 0) {
            //
            // If we are asked to delete a key, fail the request.
            //
            return NDIS_STATUS_INVALID_DATA;
        }
        else if (emptyIndex == 0) {
            //
            // If we are asked to add a key but the table is full, fail the request.
            //
            return NDIS_STATUS_RESOURCES;
        }

        pNicKey = Nic->ManagementInfo.KeyTable + emptyIndex;
        ETH_COPY_NETWORK_ADDRESS(pNicKey->MacAddr, MacAddr);
    }

    return HwSetKey(Nic, 
                    pNicKey, 
                    (UCHAR)(pNicKey - Nic->ManagementInfo.KeyTable), 
                    FALSE,
                    persistent, 
                    algoId, 
                    keyLength, 
                    keyValue);
}

PNICKEY
Hw11FindKeyMappingKey(
                     __in PNIC                 Nic,
                     __in DOT11_MAC_ADDRESS    MacAddr
                     )
{
    PNICKEY pNicKey;
    UCHAR   index;
    UCHAR   count = Nic->ManagementInfo.KeyMappingKeyCount;

    if (count == 0)
        return NULL;

    for (index = DOT11_MAX_NUM_DEFAULT_KEY; index < KEY_TABLE_SIZE; index++) {
        pNicKey = Nic->ManagementInfo.KeyTable + index;
        if (pNicKey->Valid) {
            if (NdisEqualMemory(pNicKey->MacAddr, MacAddr, sizeof(DOT11_MAC_ADDRESS)) == 1) {
                return pNicKey;
            }

            count--;
            if (count == 0) {
                return NULL;
            }
        }
    }

    //
    // Can reach here if we dont find a key for the specific AP
    //
    return NULL;
}

PNICKEY
HwFindPerStaKey(
               __in PNIC                 Nic,
               __in DOT11_MAC_ADDRESS    MacAddr,
               __in UCHAR                keyId
               )
{
    PNIC_PER_STA_KEY    pPerStaKey;
    PNICKEY             pNicKey;
    UCHAR               index;
    UCHAR               count = Nic->ManagementInfo.PerStaKeyCount;

    ASSERT(keyId < DOT11_MAX_NUM_DEFAULT_KEY);

    if (count == 0)
        return NULL;

    for (index = 0; index < PER_STA_KEY_TABLE_SIZE; index++) {
        pPerStaKey = Nic->ManagementInfo.PerStaKey + index;
        if (pPerStaKey->Valid) {
            if (NdisEqualMemory(pPerStaKey->StaMacAddr, MacAddr, sizeof(DOT11_MAC_ADDRESS)) == 1) {
                pNicKey = pPerStaKey->NicKey + keyId;
                return(pNicKey->Valid ? pNicKey : NULL);
            }

            count--;
            if (count == 0) {
                return NULL;
            }
        }
    }

    //
    // Should not reach here.
    //

    ASSERT(0);
    return NULL;
}

BOOLEAN
HwSelectLowestMatchingRate(
    PUCHAR          APRatesIE,
    UCHAR           APRatesIELength,
    PDOT11_RATE_SET STARateSet,
    PUSHORT         SelectedRate
    )
{
    UCHAR           indexHwRates;
    UCHAR           indexAPRates;

    for (indexHwRates = 0; indexHwRates < STARateSet->uRateSetLength; indexHwRates++)
    {
        for (indexAPRates = 0; indexAPRates < APRatesIELength; indexAPRates++)
        {
            if (STARateSet->ucRateSet[indexHwRates] == (APRatesIE[indexAPRates] & 0x7f)) {
                *SelectedRate = STARateSet->ucRateSet[indexHwRates];
                return TRUE;
            }
        }
    }

    return FALSE;
}

NDIS_STATUS
HwUpdateInfoFromBSSInformation(
                              __in PNIC        Nic,
                              __in PSTA_BSS_ENTRY        pBSSEntry
                              )
{
    NDIS_STATUS     ndisStatus;
    DOT11_SSID      SSID;
    PUCHAR          APRatesIE;
    UCHAR           APRatesIELength;
    PDOT11_RATE_SET STARateSet;

    ndisStatus = Dot11CopySSIDFromMemoryBlob(
                                            pBSSEntry->pDot11InfoElemBlob,
                                            pBSSEntry->InfoElemBlobSize,
                                            &SSID
                                            );

    if (ndisStatus == NDIS_STATUS_SUCCESS) {
        NdisMoveMemory(Nic->ManagementInfo.SSID, &(SSID.ucSSID), SSID.uSSIDLength);
        Nic->ManagementInfo.SSID[SSID.uSSIDLength] = 0;

        MpTrace(COMP_OID, DBG_LOUD, ("Update Info:SSID = %s\n", (CHAR*)Nic->ManagementInfo.SSID));
    }
    else {
        //
        // A required IE is missing. Fail the request.
        //
        MpTrace(COMP_OID, DBG_SERIOUS, ("Failure! Mandatory SSID Info Element not found in request\n"));
        return NDIS_STATUS_INVALID_DATA;
    }

    //
    //3 Get the BSSID
    //
    ETH_COPY_NETWORK_ADDRESS(Nic->ManagementInfo.BssId, pBSSEntry->Dot11BSSID);
    MpTrace(COMP_OID, DBG_LOUD, ("Update Info:BSSID = %02x-%02x-%02x-%02x-%02x-%02x\n", 
                                 Nic->ManagementInfo.BssId[0], Nic->ManagementInfo.BssId[1], 
                                 Nic->ManagementInfo.BssId[2], Nic->ManagementInfo.BssId[3], 
                                 Nic->ManagementInfo.BssId[4], Nic->ManagementInfo.BssId[5]));

    Nic->ManagementInfo.CapInfo = pBSSEntry->Dot11Capability.usValue;
    MpTrace(COMP_OID, DBG_LOUD, ("Update Info:CapInfo = %4x\n", Nic->ManagementInfo.CapInfo));

    ASSERT(Nic->ManagementInfo.CurrentBSSType == pBSSEntry->Dot11BSSType);

    Nic->DupCurrentInfo.BeaconInterval = pBSSEntry->BeaconInterval;
    LIMIT_BEACON_PERIOD(Nic->DupCurrentInfo.BeaconInterval);

    //
    // Verify that ATIM window is smaller than beacon interval.
    // This kind of misconfiguration can put hardware into unpredictable state
    //
    MPVERIFY(Nic->DupCurrentInfo.BeaconInterval > Nic->ManagementInfo.AtimWindow);
    MpTrace(COMP_OID, DBG_LOUD, ("Update Info:BeaconInterval = %d\n", Nic->DupCurrentInfo.BeaconInterval));
    //
    // For management packets, pick the lowest data rate supported between the AP and us as the
    // rate for management packets
    //
    STARateSet = &Nic->ManagementInfo.PhyMIB[pBSSEntry->PhyId].BasicRateSet;       
    Nic->ManagementInfo.CurrentTXMgmtRate = Nic->ManagementInfo.CurrentBeaconRate;    // Default
    
    ndisStatus = Dot11GetInfoEle(
        pBSSEntry->pDot11InfoElemBlob,
        pBSSEntry->InfoElemBlobSize,
        DOT11_INFO_ELEMENT_ID_SUPPORTED_RATES,
        &APRatesIELength,
        &APRatesIE
        );
    if (ndisStatus == NDIS_STATUS_SUCCESS)
    {
        if (!HwSelectLowestMatchingRate(
                APRatesIE,
                APRatesIELength,
                STARateSet,
                &Nic->ManagementInfo.CurrentTXMgmtRate
                ))
        {
            //
            // Look in the extended rates set
            //
            ndisStatus = Dot11GetInfoEle(
                pBSSEntry->pDot11InfoElemBlob,
                pBSSEntry->InfoElemBlobSize,
                DOT11_INFO_ELEMENT_ID_EXTD_SUPPORTED_RATES,
                &APRatesIELength,
                &APRatesIE
                );
            if (ndisStatus == NDIS_STATUS_SUCCESS)
            {
                if (!HwSelectLowestMatchingRate(
                        APRatesIE,
                        APRatesIELength,
                        STARateSet,
                        &Nic->ManagementInfo.CurrentTXMgmtRate
                        ))
                {
                    // 
                    // Didnt find any prefered data rate, select the default
                    //
                    Nic->ManagementInfo.CurrentTXMgmtRate = Nic->ManagementInfo.CurrentBeaconRate;                    
                }
            }
        }
    }

    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
HwUpdateInfoFromdot11BSSDescription(
                                   __in PNIC        Nic,
                                   __in PDOT11_BSS_DESCRIPTION    dot11BSSDescription
                                   )
{
    int i;
    ULONG ulOffset;
    UCHAR ucLength = 0;
    PUCHAR pucParam = NULL;
    NDIS_STATUS     ndisStatus;
    DOT11_SSID      SSID;

    ndisStatus = Dot11CopySSIDFromMemoryBlob(
                                            dot11BSSDescription->ucBuffer,
                                            dot11BSSDescription->uBufferLength,
                                            &SSID
                                            );

    if (ndisStatus == NDIS_STATUS_SUCCESS) {
        NdisMoveMemory(Nic->ManagementInfo.SSID, SSID.ucSSID, SSID.uSSIDLength);
        Nic->ManagementInfo.SSID[SSID.uSSIDLength] = 0;

        MpTrace(COMP_OID, DBG_LOUD, ("Update Info:SSID = %s\n", (CHAR*)Nic->ManagementInfo.SSID));
    }
    else {
        //
        // A required IE is missing. Fail the request.
        //
        MpTrace(COMP_OID, DBG_SERIOUS, ("Failure! Mandatory SSID Info Element not found in request\n"));
        return NDIS_STATUS_INVALID_DATA;
    }


    //
    //3 Get the BSSID
    //
    ETH_COPY_NETWORK_ADDRESS(Nic->ManagementInfo.BssId, dot11BSSDescription->dot11BSSID);
    MpTrace(COMP_OID, DBG_LOUD, ("Update Info:BSSID = %02x-%02x-%02x-%02x-%02x-%02x\n", 
                                 Nic->ManagementInfo.BssId[0], Nic->ManagementInfo.BssId[1], 
                                 Nic->ManagementInfo.BssId[2], Nic->ManagementInfo.BssId[3], 
                                 Nic->ManagementInfo.BssId[4], Nic->ManagementInfo.BssId[5]));

    //
    //3 Update all other IEs
    //
    for (i=EID_IbssParms+1;i<256;i++) {
        ulOffset=0;
        do {
            ndisStatus = HwGetInfoIEWithOUI(
                                           dot11BSSDescription->ucBuffer,
                                           dot11BSSDescription->uBufferLength,
                                           (UCHAR)i,
                                           &ucLength,
                                           &pucParam,
                                           &ulOffset
                                           );
            if (ndisStatus!=NDIS_STATUS_SUCCESS)
                break;

            HwAddIE(Nic,(UCHAR)i,ucLength,pucParam);
        }while (i==0xdd);
    }

    Nic->ManagementInfo.CapInfo=dot11BSSDescription->usCapabilityInformation;
    MpTrace(COMP_OID, DBG_LOUD, ("Update Info:CapInfo = %4x\n", Nic->ManagementInfo.CapInfo));

    ASSERT(Nic->ManagementInfo.CurrentBSSType == dot11BSSDescription->dot11BSSType);

    Nic->DupCurrentInfo.BeaconInterval=dot11BSSDescription->usBeaconPeriod;
    LIMIT_BEACON_PERIOD(Nic->DupCurrentInfo.BeaconInterval);

    //
    // Verify that ATIM window is smaller than beacon interval.
    // This kind of misconfiguration can put hardware into unpredictable state
    //
    MPVERIFY(Nic->DupCurrentInfo.BeaconInterval > Nic->ManagementInfo.AtimWindow);
    MpTrace(COMP_OID, DBG_LOUD, ("Update Info:BeaconInterval = %d\n", Nic->DupCurrentInfo.BeaconInterval));

    return NDIS_STATUS_SUCCESS;
}

void
Hw11ResetFilterFunction(
                       __in  PNIC Nic
                       )
{
        Nic->RxInfo.ReceiveProcessingFunction = HwProcessReceiveSTAMode;

}

NDIS_STATUS
Hw11SetPowerMgmtMode(
                    __in  PNIC Nic,
                    __in  PDOT11_POWER_MGMT_MODE mode
                    )
{
    NdisAcquireSpinLock(&Nic->ManagementInfo.PowerMgmtLock);
    if (mode->dot11PowerMode != dot11_power_mode_unknown) {    // ignore unknown case
        Nic->ManagementInfo.PowerMgmtMode = *mode;

        if (mode->dot11PowerMode == dot11_power_mode_active) {    // dot11_power_mode_active
            HwSetNicPSMode(Nic, eActive);
        }
        else if (mode->dot11PowerMode == dot11_power_mode_powersave) {    // dot11_power_mode_powersave
            HwSetNicPSMode(Nic, eFastPs); 
        }

        //
        // If we are associated, we have to send a NULL data frame first notifying 
        // the AP if we are in PS mode. Otherwise, set the RF immediately, unless we are
        // in the middle of scanning. 
        //

        if (mode->usAID != 0) {
            Nic->ManagementInfo.bPowerMgmtChangePktSent = FALSE;
            HwSendNullPkt(Nic);
        }
        else if (!Nic->ManagementInfo.bScanInProgress) {
            MpTrace(COMP_POWER, DBG_LOUD, 
                    (" *** RF %s indefinitely\n", mode->dot11PowerMode == dot11_power_mode_active ? "ON" : "OFF"));
            
            NdisReleaseSpinLock(&Nic->ManagementInfo.PowerMgmtLock);
            ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);
            SetRFPowerState(Nic, (mode->dot11PowerMode == dot11_power_mode_active ? eRfOn: eRfOff));
            NdisAcquireSpinLock(&Nic->ManagementInfo.PowerMgmtLock);               
            
        }
    }
    NdisReleaseSpinLock(&Nic->ManagementInfo.PowerMgmtLock);
    return NDIS_STATUS_SUCCESS;    
}

VOID
HwIndicateNewLinkSpeed(
                      __in PNIC     Nic,
                      __in ULONG64  linkSpeed
                      )    
{
    NDIS_STATUS_INDICATION  statusIndication;
    NDIS_LINK_STATE         linkState;

    NdisZeroMemory(&statusIndication, sizeof(NDIS_STATUS_INDICATION));
    NdisZeroMemory(&linkState, sizeof(NDIS_LINK_STATE));

    //
    // Fill in object headers
    //
    statusIndication.Header.Type = NDIS_OBJECT_TYPE_STATUS_INDICATION;
    statusIndication.Header.Revision = NDIS_STATUS_INDICATION_REVISION_1;
    statusIndication.Header.Size = sizeof(NDIS_STATUS_INDICATION);

    linkState.Header.Revision = NDIS_LINK_STATE_REVISION_1;
    linkState.Header.Type = NDIS_OBJECT_TYPE_DEFAULT;
    linkState.Header.Size = sizeof(NDIS_LINK_STATE);

    //
    // Link state buffer
    //
    linkState.MediaConnectState = MediaConnectStateConnected;
    linkState.MediaDuplexState = MediaDuplexStateFull;
    linkState.RcvLinkSpeed = linkSpeed;
    linkState.XmitLinkSpeed = linkSpeed;

    //
    // Fill in the status buffer
    // 
    statusIndication.StatusCode = NDIS_STATUS_LINK_STATE;
    statusIndication.SourceHandle = Nic->MiniportAdapterHandle;
    statusIndication.DestinationHandle = NULL;
    statusIndication.RequestId = 0;

    statusIndication.StatusBuffer = &linkState;
    statusIndication.StatusBufferSize = sizeof(NDIS_LINK_STATE);

    //
    // Indicate the status to NDIS
    //
    Mp11IndicateStatusIndication(
                                Nic->Adapter,
                                &statusIndication
                                );
}


void
Hw11SetTXDataRate(
                 __in PNIC Nic,
                 __in PDOT11_RATE_SET APRateSet,
                 __in ULONG    LinkQuality
                 )
{
    PDOT11_RATE_SET STARateSet;
    ULONG           indexHwRates;
    ULONG           indexAPRates;
    
    //
    // Select the best rate supported by both AP and us. 
    //

    STARateSet = &Nic->ManagementInfo.OperatingPhyMIB->OperationalRateSet;
    NdisAcquireSpinLock(&Nic->ManagementInfo.PhyMIBConfigLock);
    Nic->ManagementInfo.OperatingPhyMIB->ActiveRateSet.uRateSetLength = 0;

    //
    // We merge the rates from the AP and the client to create the 
    // currently active data rates table
    //
    for (indexHwRates = 0; indexHwRates < STARateSet->uRateSetLength; indexHwRates++) {
        for (indexAPRates = 0; indexAPRates < APRateSet->uRateSetLength; indexAPRates++) {
            if (STARateSet->ucRateSet[indexHwRates] == (APRateSet->ucRateSet[indexAPRates] & 0x7f)) {
                Nic->ManagementInfo.OperatingPhyMIB->ActiveRateSet.ucRateSet
                    [Nic->ManagementInfo.OperatingPhyMIB->ActiveRateSet.uRateSetLength]
                        = STARateSet->ucRateSet[indexHwRates];

                Nic->ManagementInfo.OperatingPhyMIB->ActiveRateSet.uRateSetLength++;
                break;
            }
        }
    }

    //
    // If we dont find any rates, we will stick with our management packet rate
    //
    if (Nic->ManagementInfo.OperatingPhyMIB->ActiveRateSet.uRateSetLength == 0) {
        Nic->ManagementInfo.OperatingPhyMIB->ActiveRateSet.ucRateSet[0] = 
            (UCHAR) Nic->ManagementInfo.CurrentBeaconRate;
        Nic->ManagementInfo.OperatingPhyMIB->ActiveRateSet.uRateSetLength = 1;
    }
    else if (Nic->ManagementInfo.OperatingPhyMIB->ActiveRateSet.uRateSetLength > 1)
    {
        // bubble sort data rates in ascending order
        INT     i, j;
        UCHAR   temp;
        for (i = Nic->ManagementInfo.OperatingPhyMIB->ActiveRateSet.uRateSetLength - 1; i >= 0; i--)
        {
            for (j = 1; j <= i; j++)
            {
                if (Nic->ManagementInfo.OperatingPhyMIB->ActiveRateSet.ucRateSet[j - 1] > 
                    Nic->ManagementInfo.OperatingPhyMIB->ActiveRateSet.ucRateSet[j])
                {
                    temp = Nic->ManagementInfo.OperatingPhyMIB->ActiveRateSet.ucRateSet[j - 1];
                    Nic->ManagementInfo.OperatingPhyMIB->ActiveRateSet.ucRateSet[j - 1] = 
                        Nic->ManagementInfo.OperatingPhyMIB->ActiveRateSet.ucRateSet[j];
                    Nic->ManagementInfo.OperatingPhyMIB->ActiveRateSet.ucRateSet[j] = temp;
                }
            }
        }
    }

    if (LinkQuality < LOW_RATE_LINK_QUALITY_THRESHOLD) {
        //
        // The link quality is low, we will go for the lower start rate instead of 
        // the max supported. The hardware does rate fallback, so we pick
        // something that would cause the least number of retrys
        //
        for (indexHwRates = Nic->ManagementInfo.OperatingPhyMIB->ActiveRateSet.uRateSetLength - 1; 
             indexHwRates > 0;
             indexHwRates--) {
            //
            // Check for a rate supported by both us and the AP that is below the max we prefer to use
            //
            if (Nic->ManagementInfo.OperatingPhyMIB->ActiveRateSet.ucRateSet[indexHwRates] <= 
                LOW_RATE_MAX_DATA_RATE) {
                //
                // Found the rate we want to use
                //
                break;
            }
        }
        
        Nic->ManagementInfo.CurrentTXDataRate = 
                    Nic->ManagementInfo.OperatingPhyMIB->ActiveRateSet.ucRateSet[indexHwRates];
        
    }
    else {
        // 
        // Pick the highest data rate supported between us and the AP
        //
        Nic->ManagementInfo.CurrentTXDataRate = 
                    Nic->ManagementInfo.OperatingPhyMIB->ActiveRateSet.ucRateSet
                        [Nic->ManagementInfo.OperatingPhyMIB->ActiveRateSet.uRateSetLength - 1];
    }

    NdisReleaseSpinLock(&Nic->ManagementInfo.PhyMIBConfigLock);
    MpTrace(COMP_ASSOC, DBG_LOUD, ("TX Data rate: %d\n", Nic->ManagementInfo.CurrentTXDataRate));

    //
    // Indicate NDIS_STATUS_LINK_STATE to inform the OS about the new 
    // link speed
    //
    HwIndicateNewLinkSpeed(Nic, HwDataRateToLinkSpeed((UCHAR)Nic->ManagementInfo.CurrentTXDataRate));    

    // indicate link quality after receive next packet
    Sta11NotifyEventUpdateLinkQuality(Nic->Station, TRUE);
}

DOT11_PHY_TYPE
Hw11DeterminePHYType(
                    __in PNIC Nic,
                    __in DOT11_CAPABILITY Capability,
                    __in UCHAR Channel
                    )
{
    if (Nic->CardInfo.HardwareID == HW_ID_8180) {    
        return dot11_phy_type_hrdsss;
    }
    else if (Channel > 14) {
        return dot11_phy_type_ofdm;
    }
    else {
        //
        // We have to determine between B and G.
        // If the Capability contains G-only field, it must be G
        // Otherwise, use our current operating PHY type.
        //
        if (Capability.ShortSlotTime || 
            Capability.DSSSOFDM ||
            Nic->ManagementInfo.OperatingPhyMIB->PhyType == dot11_phy_type_ofdm) {
            return dot11_phy_type_erp;
        }
        else {
            return Nic->ManagementInfo.OperatingPhyMIB->PhyType;
        }
    }

}

VOID
HwUpdateTxRetryStat(
    __in PNIC                 pNic,
    __in PMP_TX_MSDU          pMpTxd,
    __in USHORT               retryCount
    )
{
    PMP_DOT11_MGMT_DATA_MAC_HEADER  fragmentHeader = NULL;
    PNET_BUFFER                     currentNetBuffer = NULL;

    do
    {
        if (pNic->ManagementInfo.bDeviceConnected == FALSE)
        {
            // only update retry statistics when device is connected
            break;
        }
        
        currentNetBuffer = NET_BUFFER_LIST_FIRST_NB(pMpTxd->NetBufferList);
        if (currentNetBuffer == NULL)
        {
            break;
        }

        if (Dot11GetMacHeaderFromNB(currentNetBuffer, (PDOT11_MAC_HEADER*)&fragmentHeader) != NDIS_STATUS_SUCCESS)
        {
            break;
        }

        if (fragmentHeader->FrameControl.Type != DOT11_FRAME_TYPE_DATA)
        {
            // only update retry statistics for control, management, and unicast data packets
            break;
        }
        
        InterlockedExchangeAdd(&pNic->ManagementInfo.TotalRetryCount, retryCount);

        // this function is called for every packet sent
        InterlockedIncrement((LONG*)&pNic->ManagementInfo.PacketsSentForTxRateCheck);
    } while (FALSE);
}

/*
    Tx rate negotiation algorithm:
    Preconditions to update tx data rate:
            1) device is in connect state; 
            2)device has sent at lease 100 data packets

    retransmitPercentage = (toal retry) / (total packets sent)

    if retransmit percentage <= threshold for rate increase which default to 35%
        if not sending at the highest rate
            if previously came down from higher rate and wait count < wait required
                increase wait count and exit
            else
                if not previously came down from higher rate
                    reset wait required
                else leave wait required unchanged
                increase current tx rate to next higher rate, reset all other counters and exit

    if retransmit percentage >= threshold for roam which default to 350%
        if already send at the lowest rate
            try to roam to a better AP, reset all counters and exit
        else if there are at least fallback skip (which default to 3) lower data rates available
            reduce current tx rate to third lower data rate, reset all counters and exit
        else if current sending at second lowest rate
            set current tx rate to the lowest value, reset all counters and exit
        else fall through to following rate fallback handler

    if retransmit percentage >= threshold for rate fallback which default to 125%
    and not sending at the lowest rate
        if previously came up from lower rate
            double current wait required
        else reset wait required
        reduce current tx rate to next lower value, reset all other counters and exit
*/
VOID 
HwUpdateTxDataRate(
    __in PNIC                     pNic
    )
{
    PNICMANAGEMENTINFO  pManagementInfo = &pNic->ManagementInfo;
    PDOT11_RATE_SET     pActiveRateSet = &pManagementInfo->OperatingPhyMIB->ActiveRateSet;
    ULONG               rateIndex = 0;
    USHORT              prevTxRate = 0;
    ULONG               totalRetry = pManagementInfo->TotalRetryCount;
    ULONG               totalSend = pManagementInfo->PacketsSentForTxRateCheck;

    if (totalSend < pNic->TxInfo.MinPacketsSentForTxRateUpdate ||
        pManagementInfo->bDeviceConnected == FALSE)
    {
        return;
    }

    // reset counters
    InterlockedExchange(&pManagementInfo->TotalRetryCount, 0);
    InterlockedExchange((LONG*)&pManagementInfo->PacketsSentForTxRateCheck, 0);

    // acquire lock before accessing ActiveRateSet in PhyMIB
    NdisAcquireSpinLock(&pNic->ManagementInfo.PhyMIBConfigLock);
    
    for (rateIndex = 0; rateIndex < pActiveRateSet->uRateSetLength; rateIndex++)
    {
        if (pManagementInfo->CurrentTXDataRate == pActiveRateSet->ucRateSet[rateIndex])
        {
            prevTxRate = pManagementInfo->CurrentTXDataRate;
            break;
        }
    }

    do
    {
        ULONG   retransmitPercentage = 0;

        // prevTxRate equals 0 means pManagementInfo->CurrentTXDataRate 
        // is not in pActiveRateSet->ucRateSet list
        if (prevTxRate == 0)
        {
            break;
        }

        retransmitPercentage = totalRetry * 100 / totalSend;

        MpTrace(COMP_DBG, DBG_TRACE, ("%s: retransmitPercentage=%d(%d/%d), waitCount=%d, "
            "waitRequired=%d, prevRate=%d\n", 
            __FUNCTION__, 
            retransmitPercentage, totalRetry, totalSend, 
            pManagementInfo->TxRateIncreaseWaitCount, 
            pManagementInfo->TxRateIncreaseWaitRequired, 
            pManagementInfo->PrevTxDataRate));

        if (retransmitPercentage <= pNic->TxInfo.TxFailureThresholdForRateIncrease)
        {
            // Consider going up
            
            if (rateIndex < pActiveRateSet->uRateSetLength - 1)
            {
                if ((pManagementInfo->PrevTxDataRate == pActiveRateSet->ucRateSet[rateIndex + 1]) &&
                    (pManagementInfo->TxRateIncreaseWaitCount + 1 < pManagementInfo->TxRateIncreaseWaitRequired))
                {
                    // I just came down from the rate above me, i dont want to go up yet due to WaitRequired
                    pManagementInfo->TxRateIncreaseWaitCount++;

                    MpTrace(COMP_DBG, DBG_TRACE, ("%s: WAIT before increasing Tx rate. retransmitPercentage=%d(%d/%d), "
                        "waitCount=%d, waitRequired=%d, prevRate=%d\n",
                        __FUNCTION__,
                        retransmitPercentage, totalRetry, totalSend, pManagementInfo->TxRateIncreaseWaitCount, 
                        pManagementInfo->TxRateIncreaseWaitRequired, pManagementInfo->PrevTxDataRate));
                }
                else
                {
                    // 1. I came down rate above me and WaitCount >= WaitRequired
                    // 2. I came up from rate below me, no need to wait for additional time
                    if (pManagementInfo->PrevTxDataRate != pActiveRateSet->ucRateSet[rateIndex + 1])
                    {
                        // Case 2 above
                        pManagementInfo->TxRateIncreaseWaitRequired = 1;
                    }
                    else
                    {
                        // Case 1 above
                        /** don't reset TxRateIncreaseWaitRequired as we need to double */
                        /** the value if fallback to the current rate again */
                    }
                    pManagementInfo->PrevTxDataRate = pManagementInfo->CurrentTXDataRate;
                    pManagementInfo->TxRateIncreaseWaitCount = 0;
                    
                    MpTrace(COMP_DBG, DBG_TRACE, ("%s: increasing Tx data rate from %d to %d. "
                        "retransmitPercentage = %d(%d/%d), waitCount=%d, waitRequired=%d, prevRate=%d\n",
                        __FUNCTION__, 
                        pManagementInfo->CurrentTXDataRate, pActiveRateSet->ucRateSet[rateIndex + 1],
                        retransmitPercentage, totalRetry, totalSend,
                        pManagementInfo->TxRateIncreaseWaitCount, 
                        pManagementInfo->TxRateIncreaseWaitRequired, 
                        pManagementInfo->PrevTxDataRate));
                    
                    pManagementInfo->CurrentTXDataRate = pActiveRateSet->ucRateSet[rateIndex + 1];
                }
            }
            break;
        }

        if (retransmitPercentage >= pNic->TxInfo.TxFailureThresholdForRoam)
        {
            ULONG   rateSkipLevel = pNic->TxInfo.TxDataRateFallbackSkipLevel;
            
            // Really large retry count, consider roaming
            
            if (rateIndex == 0)
            {
                // I am sending at the lowest rate. Either roam or disconnect.
                MpTrace(COMP_DBG, DBG_TRACE, ("%s: too many retransmit happened while "
                    "transmitting at the lowest data rate. Attempting to roam. "
                    "retransmitPercentage=%d(%d/%d), waitCount=%d, waitRequired=%d, prevRate=%d", 
                    __FUNCTION__,
                    retransmitPercentage, totalRetry, totalSend, 
                    pManagementInfo->TxRateIncreaseWaitCount,
                    pManagementInfo->TxRateIncreaseWaitRequired, 
                    pManagementInfo->PrevTxDataRate));

                Sta11NotifyEventRoamForSendFailures(pNic->Station);

                // reset counters
                pManagementInfo->PrevTxDataRate = 0;
                pManagementInfo->TxRateIncreaseWaitRequired = 1;
                pManagementInfo->TxRateIncreaseWaitCount = 0;
                break;
            }
            else if (rateIndex >= rateSkipLevel)
            {
                // try to go down three rates
                MpTrace(COMP_DBG, DBG_TRACE, ("%s: too many retransmit happened. Reducing Tx rate from %d to %d.\n",
                        __FUNCTION__, 
                        pManagementInfo->CurrentTXDataRate,
                        pActiveRateSet->ucRateSet[rateIndex - rateSkipLevel]));

                pManagementInfo->CurrentTXDataRate = pActiveRateSet->ucRateSet[rateIndex - rateSkipLevel];
                pManagementInfo->PrevTxDataRate = 0;
                pManagementInfo->TxRateIncreaseWaitRequired = 1;
                pManagementInfo->TxRateIncreaseWaitCount = 0;
                break;
            }
            else if (rateIndex != 1)
            {
                // set tx rate to the lowest rate
                MpTrace(COMP_DBG, DBG_TRACE, ("%s: too many retransmit happened. Reducing Tx rate from "
                        "%d to lowest %d.\n",
                        __FUNCTION__, 
                        pManagementInfo->CurrentTXDataRate,
                        pActiveRateSet->ucRateSet[0]));

                pManagementInfo->CurrentTXDataRate = pActiveRateSet->ucRateSet[0];
                pManagementInfo->PrevTxDataRate = 0;
                pManagementInfo->TxRateIncreaseWaitRequired = 1;
                pManagementInfo->TxRateIncreaseWaitCount = 0;
                break;
            }
            else
            {
                // rateIndex equals to 1. this is the same as lowering tx rate by 1 level
                // which is what rate fallback handler does. fall through.
            }
        }

        if (retransmitPercentage >= pNic->TxInfo.TxFailureThresholdForRateFallback && rateIndex > 0)
        {
            // consider going down. no need to wait for additional time
            
            if (pManagementInfo->PrevTxDataRate == pActiveRateSet->ucRateSet[rateIndex - 1])
            {
                pManagementInfo->TxRateIncreaseWaitRequired *= 2;
            }
            else
            {
                pManagementInfo->TxRateIncreaseWaitRequired = 1;
            }
            pManagementInfo->PrevTxDataRate = pManagementInfo->CurrentTXDataRate;
            pManagementInfo->TxRateIncreaseWaitCount = 0;
            
            MpTrace(COMP_DBG, DBG_TRACE, ("%s: reducing Tx data rate from %d to %d. "
                "retransmitPercentage=%d(%d/%d), waitCount=%d, waitRequired=%d, prevRate=%d\n",
                __FUNCTION__, 
                pManagementInfo->CurrentTXDataRate, 
                pActiveRateSet->ucRateSet[rateIndex - 1],
                retransmitPercentage, totalRetry, totalSend, 
                pManagementInfo->TxRateIncreaseWaitCount, 
                pManagementInfo->TxRateIncreaseWaitRequired,
                pManagementInfo->PrevTxDataRate));

            pManagementInfo->CurrentTXDataRate = pActiveRateSet->ucRateSet[rateIndex - 1];
            break;
        }
    } while (FALSE);

    NdisReleaseSpinLock(&pNic->ManagementInfo.PhyMIBConfigLock);

    if (prevTxRate != pManagementInfo->CurrentTXDataRate)
    {
        // set state so that new link quality will be indicated 
        // when next packet comes in
        Sta11NotifyEventUpdateLinkQuality(pNic->Station, TRUE);
        
        // indicate new link speed
        HwIndicateNewLinkSpeed(pNic, HwDataRateToLinkSpeed((UCHAR)pManagementInfo->CurrentTXDataRate));
    }
}


NDIS_STATUS
Hw11QueryLinkParameters(
    __in PNIC     pNic,
    __out PNDIS_LINK_PARAMETERS      pLinkParams
    )
{
    //
    // Report the current link speed to the OS
    //
    pLinkParams->MediaDuplexState = MediaDuplexStateHalf;
    pLinkParams->XmitLinkSpeed = HwDataRateToLinkSpeed((UCHAR)pNic->ManagementInfo.CurrentTXDataRate);
    pLinkParams->RcvLinkSpeed = HwDataRateToLinkSpeed((UCHAR)pNic->ManagementInfo.CurrentTXDataRate);
    pLinkParams->PauseFunctions = NdisPauseFunctionsUnsupported;
    pLinkParams->AutoNegotiationFlags = NDIS_LINK_STATE_DUPLEX_AUTO_NEGOTIATED;

    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Hw11SetLinkParameters(
    __in PNIC     pNic,
    __in PNDIS_LINK_PARAMETERS      pLinkParams
    )
{
    PDOT11_RATE_SET STARateSet;
    ULONG           index;
    UCHAR           dataRate;
    
    if (pLinkParams->RcvLinkSpeed != pLinkParams->XmitLinkSpeed) {
        //
        // For us the TX and RX rates must be the same
        //
        return NDIS_STATUS_INVALID_DATA;
    }

    //
    // If the rate specified by the user is valid, we are going to attempt to 
    // transmit at that rate (even if it may not be acceptable to the AP)
    //
    dataRate = HwLinkSpeedToDataRate(pLinkParams->XmitLinkSpeed);

    STARateSet = &pNic->ManagementInfo.OperatingPhyMIB->OperationalRateSet;

    //
    // Is this a supported data rate?
    //
    for (index = 0; index < STARateSet->uRateSetLength; index++)
    {
        if (STARateSet->ucRateSet[index] == dataRate) {
            pNic->ManagementInfo.CurrentTXDataRate = STARateSet->ucRateSet[index];
            MpTrace(COMP_ASSOC, DBG_LOUD, ("TX Data rate: %d\n", STARateSet->ucRateSet[index]));
            
            //
            // Indicate NDIS_STATUS_LINK_STATE to inform the OS about the new 
            // link speed
            //
            HwIndicateNewLinkSpeed(pNic, HwDataRateToLinkSpeed((UCHAR)pNic->ManagementInfo.CurrentTXDataRate));

            // TODO: indicate link quality here???
            
            return NDIS_STATUS_SUCCESS;
        }
    }
    //
    // Unsupported data rate
    //
    return NDIS_STATUS_INVALID_DATA;
}


