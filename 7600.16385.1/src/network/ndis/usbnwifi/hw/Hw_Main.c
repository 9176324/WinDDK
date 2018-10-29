/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Hw_Main.c

Abstract:
    HW layer PNP functions
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/

#include "hw_pcomp.h"


#pragma warning(disable:4200)  // nameless struct/union
#pragma warning(disable:4201)  // nameless struct/union
#pragma warning(disable:4214)  // bit field types other than int

#include "hw_main.h"
#include "hw_Nic.h"
#include "hw_func.h"
#include "usb_main.h"
#include "bus_intf.h"
#include "8187_gen.h"

  
ULONG
RF_ReadReg(
    __in  PNIC    Nic,
    __in  UCHAR      offset
    );


#pragma warning(default:4200)
#pragma warning(default:4201)
#pragma warning(default:4214)

#ifdef USE_SAMPLE_PACKET_BUFFERING_IMPLEMENTATION
#include "Nwf_Mp_PSPacketsManager.h"
#endif

#if DOT11_TRACE_ENABLED
#include "Hw_Main.tmh"
#endif

 //
 // initialize the registry entry table
 //
 NIC_REG_ENTRY NICRegTable[] = {
    {NDIS_STRING_CONST("RxTotalDescNum"),       // Reg value name
     0,                                         // 1 -> required,0 -> optional
     NdisParameterInteger,                      // Type
     FIELD_OFFSET(NIC,RxInfo),                  // Struct offset in MP_ADAPTER
     FIELD_OFFSET(NICRXINFO,RxTotalDescNum),    // Struct offset
     sizeof(((NICRXINFO *)0)->RxTotalDescNum),  // Size of the field
     64,                                        // Default value
     1,                                         // Min value
     64},                                       // Max value

    {NDIS_STRING_CONST("RadioOff"),
     0,
     NdisParameterInteger,
     FIELD_OFFSET(NIC,RfInfo),
     FIELD_OFFSET(NICRFINFO,RadioOff),
     sizeof(((NICRFINFO *)0)->RadioOff),
     0,                                         // Default = On
     0,
     1},

    {NDIS_STRING_CONST("ShortRetryLimit"),
     0,
     NdisParameterInteger,
     FIELD_OFFSET(NIC,DupRegistryInfo),
     FIELD_OFFSET(NICDUPLICATEINFO,ShortRetryLimit),
     sizeof(((NICDUPLICATEINFO *)0)->ShortRetryLimit),
     7,
     0,
     15},

    {NDIS_STRING_CONST("LongRetryLimit"),
     0,
     NdisParameterInteger,
     FIELD_OFFSET(NIC,DupRegistryInfo),
     FIELD_OFFSET(NICDUPLICATEINFO,LongRetryLimit),
     sizeof(((NICDUPLICATEINFO *)0)->LongRetryLimit),
     7,
     0,
     15},

    {NDIS_STRING_CONST("DefaultBeaconRate"),
     0,
     NdisParameterInteger,
     FIELD_OFFSET(NIC,DupRegistryInfo),
     FIELD_OFFSET(NICDUPLICATEINFO,DefaultBeaconRate),
     sizeof(((NICDUPLICATEINFO *)0)->DefaultBeaconRate),
     1,
     2,
     2},

    {NDIS_STRING_CONST("InterfaceDebugMode"),
     0,
     NdisParameterInteger,
     FIELD_OFFSET(NIC,RfInfo),
     FIELD_OFFSET(NICRFINFO,InterfaceDebugMode),
     sizeof(((NICRFINFO *)0)->InterfaceDebugMode),
     1,
     0,
     3},

    {NDIS_STRING_CONST("EnableGPIO0"),
     0,
     NdisParameterInteger,
     FIELD_OFFSET(NIC,RfInfo),
     FIELD_OFFSET(NICRFINFO,EnableGPIO0),
     sizeof(((NICRFINFO *)0)->EnableGPIO0),
     0,
     0,
     1},

    {NDIS_STRING_CONST("b2ndCut"),
     0,
     NdisParameterInteger,
     FIELD_OFFSET(NIC,RfInfo),
     FIELD_OFFSET(NICRFINFO,b2ndCut),
     sizeof(((NICRFINFO *)0)->b2ndCut),
     0,
     0,
     0x1},

    {NDIS_STRING_CONST("ZebraVersion"),
     0,
     NdisParameterInteger,
     FIELD_OFFSET(NIC,RfInfo),
     FIELD_OFFSET(NICRFINFO,RegZebraVersion),
     sizeof(((NICRFINFO *)0)->RegZebraVersion),
     1,
     0,
     2},

    {NDIS_STRING_CONST("ChargePump"),
     0,
     NdisParameterInteger,
     FIELD_OFFSET(NIC,RfInfo),
     FIELD_OFFSET(NICRFINFO,ChargePump),
     sizeof(((NICRFINFO *)0)->ChargePump),
     2,
     0,
     6},

    {NDIS_STRING_CONST("TXChargePump"),
     0,
     NdisParameterInteger,
     FIELD_OFFSET(NIC,RfInfo),
     FIELD_OFFSET(NICRFINFO,TXChargePump),
     sizeof(((NICRFINFO *)0)->TXChargePump),
     6,
     0,
     7},

    {NDIS_STRING_CONST("RXChargePump"),
     0,
     NdisParameterInteger,
     FIELD_OFFSET(NIC,RfInfo),
     FIELD_OFFSET(NICRFINFO,RXChargePump),
     sizeof(((NICRFINFO *)0)->RXChargePump),
     0,
     0,
     7},

    {NDIS_STRING_CONST("CornerChange"),
     0,
     NdisParameterInteger,
     FIELD_OFFSET(NIC,RfInfo),
     FIELD_OFFSET(NICRFINFO,CornerChange),
     sizeof(((NICRFINFO *)0)->CornerChange),
     0,
     0,
     2},

    {NDIS_STRING_CONST("TXAntenna"),
     0,
     NdisParameterInteger,
     FIELD_OFFSET(NIC,RfInfo),
     FIELD_OFFSET(NICRFINFO,TXAntenna),
     sizeof(((NICRFINFO *)0)->TXAntenna),
     0,
     0,
     1},

    {NDIS_STRING_CONST("RXAntenna"),
     0,
     NdisParameterInteger,
     FIELD_OFFSET(NIC,RfInfo),
     FIELD_OFFSET(NICRFINFO,RXAntenna),
     sizeof(((NICRFINFO *)0)->RXAntenna),
     0,
     0,
     1},

/*
    {NDIS_STRING_CONST("TXOFDMPower"),
     0,
     NdisParameterInteger,
     MP_OFFSET(TXOFDMPower),
     MP_SIZE(TXOFDMPower),
     1,
     0,
     3},

    {NDIS_STRING_CONST("TXCCKPower"),
     0,
     NdisParameterInteger,
     MP_OFFSET(TXCCKPower),
     MP_SIZE(TXCCKPower),
     1,
     0,
     3},
*/

    {NDIS_STRING_CONST("FastFalseAlarm"),
     0,
     NdisParameterInteger,
     FIELD_OFFSET(NIC,RfInfo),
     FIELD_OFFSET(NICRFINFO,FastFalseAlarm),
     sizeof(((NICRFINFO *)0)->FastFalseAlarm),
     1,
     0,
     1},

    {NDIS_STRING_CONST("CCKFastFalseAlarm"),
     0,
     NdisParameterInteger,
     FIELD_OFFSET(NIC,RfInfo),
     FIELD_OFFSET(NICRFINFO,CCKFastFalseAlarm),
     sizeof(((NICRFINFO *)0)->CCKFastFalseAlarm),
     1,
     0,
     1},

    {NDIS_STRING_CONST("EVMCheck"),
     0,
     NdisParameterInteger,
     FIELD_OFFSET(NIC,RfInfo),
     FIELD_OFFSET(NICRFINFO,EVMCheck),
     sizeof(((NICRFINFO *)0)->EVMCheck),
     1,
     0,
     3},

    {NDIS_STRING_CONST("AGCmode"),
     0,
     NdisParameterInteger,
     FIELD_OFFSET(NIC,RfInfo),
     FIELD_OFFSET(NICRFINFO,AGCmode),
     sizeof(((NICRFINFO *)0)->AGCmode),
     1,
     0,
     1},

    {NDIS_STRING_CONST("SamplingPhase"),
     0,
     NdisParameterInteger,
     FIELD_OFFSET(NIC,RfInfo),
     FIELD_OFFSET(NICRFINFO,SamplingPhase),
     sizeof(((NICRFINFO *)0)->SamplingPhase),
     1,
     0,
     1},

    {NDIS_STRING_CONST("EnableGPIO0"),
     0,
     NdisParameterInteger,
     FIELD_OFFSET(NIC,RfInfo),
     FIELD_OFFSET(NICRFINFO,EnableGPIO0),
     sizeof(((NICRFINFO *)0)->EnableGPIO0),
     0,
     0,
     1},

    {NDIS_STRING_CONST("VerificationType"),
     0,
     NdisParameterInteger,
     FIELD_OFFSET(NIC,CardInfo),
     FIELD_OFFSET(NICCARDINFO,VerificationType),
     sizeof(((NICCARDINFO *)0)->VerificationType),
     1,
     0,
     4},
    {NDIS_STRING_CONST("PacketControlCW"),
     0,
     NdisParameterInteger,
     FIELD_OFFSET(NIC,CardInfo),
     FIELD_OFFSET(NICCARDINFO,RegPacketControlCW),
     sizeof(((NICCARDINFO *)0)->RegPacketControlCW),
     UNIVERSAL_CONTROL_CW,
     UNIVERSAL_CONTROL_CW,
     PACKET_CONTROL_CW},

    {NDIS_STRING_CONST("PacketControlRL"),
     0,
     NdisParameterInteger,
     FIELD_OFFSET(NIC,CardInfo),
     FIELD_OFFSET(NICCARDINFO,RegPacketControlRL),
     sizeof(((NICCARDINFO *)0)->RegPacketControlRL),
     UNIVERSAL_CONTROL_RL,
     UNIVERSAL_CONTROL_RL,
     PACKET_CONTROL_RL},
     
    {NDIS_STRING_CONST("PacketControlAGC"),
     0,
     NdisParameterInteger,
     FIELD_OFFSET(NIC,CardInfo),
     FIELD_OFFSET(NICCARDINFO,RegPacketControlAGC),
     sizeof(((NICCARDINFO *)0)->RegPacketControlAGC),
     UNIVERSAL_CONTROL_AGC,
     UNIVERSAL_CONTROL_AGC,
     PACKET_CONTROL_AGC},
    

    {NDIS_STRING_CONST("CCK_TxAGC"),
     0,
     NdisParameterInteger,
     FIELD_OFFSET(NIC,CardInfo),
     FIELD_OFFSET(NICCARDINFO,RegCCK_TxAGC),
     sizeof(((NICCARDINFO *)0)->RegCCK_TxAGC),
     128,
     1,
     255},

    {NDIS_STRING_CONST("OFDM_TxAGC"),
     0,
     NdisParameterInteger,
     FIELD_OFFSET(NIC,CardInfo),
     FIELD_OFFSET(NICCARDINFO,
     RegOFDM_TxAGC),
     sizeof(((NICCARDINFO *)0)->RegOFDM_TxAGC),
     128,
     1,
     255},

    {NDIS_STRING_CONST("AGC"),
     0,
     NdisParameterInteger,
     FIELD_OFFSET(NIC,CardInfo),
     FIELD_OFFSET(NICCARDINFO,RegAGC),
     sizeof(((NICCARDINFO *)0)->RegAGC),
     1,
     1,
     100},
    // /*    
    {NDIS_STRING_CONST("PacketControlAntenna"),
     0,
     NdisParameterInteger,
     FIELD_OFFSET(NIC,CardInfo),
     FIELD_OFFSET(NICCARDINFO,RegPacketControlAntenna),
     sizeof(((NICCARDINFO *)0)->RegPacketControlAntenna),
     UNIVERSAL_CONTROL_ANTENNA,
     UNIVERSAL_CONTROL_ANTENNA,
     PACKET_CONTROL_ANTENNA},           
    {NDIS_STRING_CONST("CCK_TxAntenna"),
     0,
     NdisParameterInteger,
     FIELD_OFFSET(NIC,CardInfo),
     FIELD_OFFSET(NICCARDINFO,RegCCK_TxAntenna),
     sizeof(((NICCARDINFO *)0)->RegCCK_TxAntenna),
     0,
     0,
     1},
    {NDIS_STRING_CONST("OFDM_TxAntenna"),
     0,
     NdisParameterInteger,
     FIELD_OFFSET(NIC,CardInfo),
     FIELD_OFFSET(NICCARDINFO,RegOFDM_TxAntenna),
     sizeof(((NICCARDINFO *)0)->RegOFDM_TxAntenna),
     0,
     0,
     1},

    {NDIS_STRING_CONST("FeedbackControAntenna"),
     0,
     NdisParameterInteger,
     FIELD_OFFSET(NIC,CardInfo),
     FIELD_OFFSET(NICCARDINFO,RegFeedbackControlAntenna),
     sizeof(((NICCARDINFO *)0)->RegFeedbackControlAntenna),
     1,
     0,
     1},

    {NDIS_STRING_CONST("bAutoRateFallback"),
     0,
     NdisParameterInteger,
     FIELD_OFFSET(NIC,CardInfo),
     FIELD_OFFSET(NICCARDINFO,bRegAutoRateFallback),
     sizeof(((NICCARDINFO *)0)->bRegAutoRateFallback),
     TRUE,
     FALSE,
     TRUE},

    {NDIS_STRING_CONST("AutoRateFallbackStep"),
     0,
     NdisParameterInteger,
     FIELD_OFFSET(NIC,CardInfo),
     FIELD_OFFSET(NICCARDINFO,RegAutoRateFallbackStep),
     sizeof(((NICCARDINFO *)0)->RegAutoRateFallbackStep),
     0,
     0,
     3},

    {NDIS_STRING_CONST("ChannelPlan"),
     0,
     NdisParameterInteger,
     FIELD_OFFSET(NIC,RfInfo),
     FIELD_OFFSET(NICRFINFO,ChannelPlan),
     sizeof(((NICRFINFO *)0)->ChannelPlan),
     DOMAIN_FROM_EEPROM,					//default , look at EEprom for values
     0,
     0x00ff},

    {NDIS_STRING_CONST("MinPacketsSentForTxRateUpdate"),
     0,
     NdisParameterInteger,
     FIELD_OFFSET(NIC,TxInfo),
     FIELD_OFFSET(NICTXINFO, MinPacketsSentForTxRateUpdate),
     sizeof(((NICTXINFO *)0)->MinPacketsSentForTxRateUpdate),
     100,
     10,
     500},

    {NDIS_STRING_CONST("TxFailureThresholdForRateFallback"),
     0,
     NdisParameterInteger,
     FIELD_OFFSET(NIC,TxInfo),
     FIELD_OFFSET(NICTXINFO, TxFailureThresholdForRateFallback),
     sizeof(((NICTXINFO *)0)->TxFailureThresholdForRateFallback),
     125,
     50,
     200},

    {NDIS_STRING_CONST("TxFailureThresholdForRateIncrease"),
     0,
     NdisParameterInteger,
     FIELD_OFFSET(NIC,TxInfo),
     FIELD_OFFSET(NICTXINFO,TxFailureThresholdForRateIncrease),
     sizeof(((NICTXINFO *)0)->TxFailureThresholdForRateIncrease),
     35,
     0,
     50},

    {NDIS_STRING_CONST("TxFailureThresholdForRoam"),
     0,
     NdisParameterInteger,
     FIELD_OFFSET(NIC,TxInfo),
     FIELD_OFFSET(NICTXINFO,TxFailureThresholdForRoam),
     sizeof(((NICTXINFO *)0)->TxFailureThresholdForRoam),
     350,
     300,
     600},

    {NDIS_STRING_CONST("TxDataRateFallbackSkipLevel"),
     0,
     NdisParameterInteger,
     FIELD_OFFSET(NIC,TxInfo),
     FIELD_OFFSET(NICTXINFO,TxDataRateFallbackSkipLevel),
     sizeof(((NICTXINFO *)0)->TxDataRateFallbackSkipLevel),
     3,
     1,
     6},
};




NDIS_STATUS
Hw11AllocateNic(
    __in  NDIS_HANDLE     MiniportAdapterHandle,
    __out PNIC*           ppNic,
    __in  PADAPTER        pAdapter,
    __in  PSTATION        pStation
    )
{
    NDIS_STATUS ndisStatus;
    ULONG       size;
    
    
    ndisStatus = NDIS_STATUS_RESOURCES;
    __try
    {

        //
        // Allocate NIC structure.
        //   
        MP_ALLOCATE_MEMORY(MiniportAdapterHandle, ppNic, sizeof(NIC), HW11_MEMORY_TAG);
        
        if (*ppNic == NULL) {
            MpTrace(COMP_INIT_PNP, DBG_SERIOUS, ("Failed to allocate memory for NIC\n"));
            __leave;
        }
        
        NdisZeroMemory((*ppNic), sizeof(NIC));

        size = sizeof(DOT11_SUPPORTED_PHY_TYPES) + (MAX_NUM_PHY_TYPES - 1) * sizeof(DOT11_PHY_TYPE);
        MP_ALLOCATE_MEMORY(MiniportAdapterHandle,
                           &((*ppNic)->ManagementInfo.pSupportedPhyTypes),
                           size,
                           HW11_MEMORY_TAG);
        
        if ((*ppNic)->ManagementInfo.pSupportedPhyTypes == NULL) {
            MpTrace(COMP_INIT_PNP,
                    DBG_SERIOUS,
                    ("Failed to allocate memory for NIC->ManagementInfo.pSupportedPhyTypes\n"));
            __leave;
        }
        
        NdisZeroMemory((*ppNic)->ManagementInfo.pSupportedPhyTypes, size);
        size = sizeof(DOT11_REG_DOMAINS_SUPPORT_VALUE) + 
               (MAX_NUM_DOT11_REG_DOMAINS_VALUE - 1) * sizeof(DOT11_REG_DOMAIN_VALUE);
        MP_ALLOCATE_MEMORY(MiniportAdapterHandle,
                           &((*ppNic)->ManagementInfo.pRegDomainsSupportValue),
                           size,
                           HW11_MEMORY_TAG);
        
        if ((*ppNic)->ManagementInfo.pRegDomainsSupportValue == NULL) {
            MpTrace(COMP_INIT_PNP,
                    DBG_SERIOUS,
                    ("Failed to allocate memory for NIC->ManagementInfo.pRegDomainsSupportValue\n"));
            __leave;
        }
        
        NdisZeroMemory((*ppNic)->ManagementInfo.pRegDomainsSupportValue, size);
        size = sizeof(DOT11_DIVERSITY_SELECTION_RX_LIST) +
              (MAX_NUM_DIVERSITY_SELECTION_RX_LIST - 1) * sizeof(DOT11_DIVERSITY_SELECTION_RX);
        MP_ALLOCATE_MEMORY(MiniportAdapterHandle,
                           &((*ppNic)->ManagementInfo.pDiversitySelectionRxList),
                           size, 
                           HW11_MEMORY_TAG);
        if ((*ppNic)->ManagementInfo.pDiversitySelectionRxList == NULL) {
            MpTrace(COMP_INIT_PNP,
                    DBG_SERIOUS,
                    ("Failed to allocate memory for NIC->ManagementInfo.pDiversitySelectionRxList\n"));
            __leave;
        }
        
        NdisZeroMemory((*ppNic)->ManagementInfo.pDiversitySelectionRxList, size);
        //
        // Allocate the ReceiveFilterLock lock
        //
        NdisAllocateSpinLock(&(*ppNic)->RxInfo.ReceiveFilterLock);

        (*ppNic)->MiniportAdapterHandle = MiniportAdapterHandle;
        (*ppNic)->Adapter = pAdapter;
        (*ppNic)->Station = pStation;
        ndisStatus = HwInitializeBus(*ppNic);
        
        if (ndisStatus != NDIS_STATUS_SUCCESS) {
            MpTrace(COMP_INIT_PNP,
                    DBG_SERIOUS,
                    ("Failed to initialize bus\n"));
            __leave;
        }

        HwInitializeVariable(*ppNic);        
        ndisStatus = NDIS_STATUS_SUCCESS;
    } 
    __finally 
    {
    }

    return ndisStatus;
}


VOID
Hw11FreeNic(
    __in  PNIC         pNic
    )
{
    MPVERIFY(pNic);

    MP_FREE_MEMORY(pNic->ManagementInfo.pSupportedPhyTypes);
    MP_FREE_MEMORY(pNic->ManagementInfo.pRegDomainsSupportValue);
    MP_FREE_MEMORY(pNic->ManagementInfo.pDiversitySelectionRxList);

    //
    // Free the ReceiveFilterSpinlock
    //
    NdisFreeSpinLock(&pNic->RxInfo.ReceiveFilterLock);

    if (pNic->ManagementInfo.SwChnlWorkItem != NULL) {
        WdfWorkItemFlush(pNic->ManagementInfo.SwChnlWorkItem);
        WdfObjectDelete(pNic->ManagementInfo.SwChnlWorkItem);
        pNic->ManagementInfo.SwChnlWorkItem = NULL;
    }

    MP_FREE_MEMORY(pNic);
}


NDIS_STATUS
Hw11StartNic(
    __in  PNIC pNic
    )
{
    UNREFERENCED_PARAMETER(pNic);

    WdfTimerStart(pNic->ManagementInfo.Timer_PeriodicCallback, WDF_REL_TIMEOUT_IN_MS(HWPERIOD_TIMER_IN_MS));

    return NDIS_STATUS_SUCCESS;
}


NDIS_STATUS
Hw11EnableNotification(
    __in  PNIC pNic
    )
{
    ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);
    HwBusStartDevice(pNic);
    //
    // PCI devices may need to do more here. Nothing for USB
    //
    return NDIS_STATUS_SUCCESS;
}



VOID
Hw11TerminateNic(
    __in  PNIC pNic
)
{
    BOOLEAN canceled;

    //
    // Cancel all timers.
    //
    canceled = WdfTimerStop(pNic->ManagementInfo.Timer_Scan, TRUE);
    canceled = WdfTimerStop(pNic->ManagementInfo.Timer_JoinTimeout, TRUE);
    canceled = WdfTimerStop(pNic->ManagementInfo.Timer_Awake, TRUE);
    canceled = WdfTimerStop(pNic->ManagementInfo.Timer_PeriodicCallback, TRUE);

    //
    // Delete encryption info and MIC data lookaside list.
    // 
    if (pNic->CardInfo.HardwareID == HW_ID_8187) {
        NdisDeleteNPagedLookasideList(&pNic->ManagementInfo.EncryptionInfoList);
        NdisDeleteNPagedLookasideList(&pNic->ManagementInfo.MICDataList);
    }
    HwDeInitBus(pNic);
}

NDIS_STATUS
Hw11SetOptionalServices(
    __in  NDIS_HANDLE  NdisMiniportDriverHandle,
    __in  NDIS_HANDLE  MiniportDriverContext
    )
{
    UNREFERENCED_PARAMETER(NdisMiniportDriverHandle);
    UNREFERENCED_PARAMETER(MiniportDriverContext);

    return NDIS_STATUS_SUCCESS;
}


VOID
Hw11Shutdown(
    __in  PNIC pNic)
{    
    Hw11HaltNic(pNic);
    HwBusStopNotification(pNic);
}

VOID
Hw11StopNotification(
    __in  PNIC pNic
    )
{
    //
    // for PCI disable interrupts here. for USb diable the Io Targets
    //
    HwBusStopNotification(pNic);
}

   //mostly for non surprise remove
VOID
Hw11HaltNic(
    __in  PNIC pNic)
{
    //
    // No I/O if device has been surprise removed
    //
    if (pNic->CardInfo.SurpriseRemoved == FALSE) {

        //
        // TODO: Is there anything to add here for USB ?
        // We already stop the notificiations earlier. 
        //
    }
    HaltNic8187(pNic, FALSE);
}

NDIS_STATUS
Hw11InitializeNic(
    __in  PNIC         pNic,
    __in WDFDEVICE    WdfDevice
    )
{
    NDIS_STATUS         ndisStatus = NDIS_STATUS_ADAPTER_NOT_FOUND;
    NTSTATUS                ntStatus;
    WDF_TIMER_CONFIG       timerConfig;
    WDF_OBJECT_ATTRIBUTES   attributes;
    WDF_WORKITEM_CONFIG     workitemConfig;
    PTIMER_NIC_CONTEXT      nicTimerNicContext;    
    PWORKITEM_CONTEXT       nicWorkItemContext;

    MPVERIFY(pNic);

    __try 
    {

        //
        // Create a WDFTIMER object
        //
        WDF_TIMER_CONFIG_INIT(&timerConfig, HwScanCallback);
    
        WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(
                                       &attributes,
                                       TIMER_NIC_CONTEXT
                                       );
        attributes.ParentObject = WdfDevice; 

        ntStatus = WdfTimerCreate(
                                 &timerConfig,
                                 &attributes,
                                 &pNic->ManagementInfo.Timer_Scan     // Output handle
                                 );

        if (!NT_SUCCESS(ntStatus))
        {
            NT_STATUS_TO_NDIS_STATUS(ntStatus, &ndisStatus);
            NdisWriteErrorLogEntry(pNic->MiniportAdapterHandle,
                                   ndisStatus,
                                   0);
            
            __leave;
        }
        nicTimerNicContext = GetTimerContext(pNic->ManagementInfo.Timer_Scan);
        nicTimerNicContext->Nic = pNic;
        //
        // Create a WDFTIMER object
        //
        WDF_TIMER_CONFIG_INIT(&timerConfig, HwJoinTimeoutCallback );
    
        WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(
                                       &attributes,
                                       TIMER_NIC_CONTEXT
                                       );
        attributes.ParentObject = WdfDevice;  

        ntStatus = WdfTimerCreate(
                                 &timerConfig,
                                 &attributes,
                                 &pNic->ManagementInfo.Timer_JoinTimeout     // Output handle
                                 );
        if (!NT_SUCCESS(ntStatus))
        {
            NT_STATUS_TO_NDIS_STATUS(ntStatus, &ndisStatus);
            NdisWriteErrorLogEntry(pNic->MiniportAdapterHandle,
                                   ndisStatus,
                                   0);
            __leave;
        }
        nicTimerNicContext = GetTimerContext(pNic->ManagementInfo.Timer_JoinTimeout);
        nicTimerNicContext->Nic = pNic;

        //
        // Create a WDFTIMER object
        //
        WDF_TIMER_CONFIG_INIT(&timerConfig, HwAwakeCallback );   
        WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(
                                       &attributes,
                                       TIMER_NIC_CONTEXT
                                       );
        attributes.ParentObject = WdfDevice;
        ntStatus = WdfTimerCreate(
                                 &timerConfig,
                                 &attributes,
                                 &pNic->ManagementInfo.Timer_Awake     // Output handle
                                 );
        if (!NT_SUCCESS(ntStatus))
        {
            NT_STATUS_TO_NDIS_STATUS(ntStatus, &ndisStatus);
            NdisWriteErrorLogEntry(pNic->MiniportAdapterHandle,
                                   ndisStatus,
                                   0);           
            __leave;
        }
        nicTimerNicContext = GetTimerContext(pNic->ManagementInfo.Timer_Awake);
        nicTimerNicContext->Nic = pNic;

        WDF_WORKITEM_CONFIG_INIT(&workitemConfig, HwSwChnlWorkItem);

        //
        // Specify the context type for the WDF workitem object.
        //
        WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes, WORKITEM_CONTEXT);
        attributes.ParentObject = WdfDevice;   
        ntStatus = WdfWorkItemCreate(&workitemConfig,
                                    &attributes,
                                    &pNic->ManagementInfo.SwChnlWorkItem);
        if (!NT_SUCCESS(ntStatus))
        {
            NT_STATUS_TO_NDIS_STATUS(ntStatus, &ndisStatus);
            NdisWriteErrorLogEntry(pNic->MiniportAdapterHandle,
                                   ndisStatus,
                                   0);
           

            __leave;
        }
        nicWorkItemContext = GetWorkItemContext(pNic->ManagementInfo.SwChnlWorkItem);
        nicWorkItemContext->Nic = pNic;        
       
        if(pNic->ManagementInfo.SwChnlWorkItem == NULL)
        {
            NdisWriteErrorLogEntry(pNic->MiniportAdapterHandle,
                                   NDIS_ERROR_CODE_OUT_OF_RESOURCES,
                                   0);
            ndisStatus = NDIS_STATUS_RESOURCES;
            __leave;
        }

        // Create a WDFTIMER object for periodic timer callback
        WDF_TIMER_CONFIG_INIT(&timerConfig, HwPeriodTimerCallback);
        WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(
                                       &attributes,
                                       TIMER_NIC_CONTEXT
                                       );
        attributes.ParentObject = WdfDevice;
        ntStatus = WdfTimerCreate(
                                 &timerConfig,
                                 &attributes,
                                 &pNic->ManagementInfo.Timer_PeriodicCallback
                                 );
        if (!NT_SUCCESS(ntStatus))
        {
            NT_STATUS_TO_NDIS_STATUS(ntStatus, &ndisStatus);
            NdisWriteErrorLogEntry(pNic->MiniportAdapterHandle,
                                   ndisStatus,
                                   0);           
            __leave;
        }
        nicTimerNicContext = GetTimerContext(pNic->ManagementInfo.Timer_PeriodicCallback);
        nicTimerNicContext->Nic = pNic;

        //
        // NOTE : Initially the Bus specific initialization was done here. But since we need to
        // read the NIC registers to decide on initialization we need to 
        // initialize bus earlier.
        //
        ndisStatus = HwReadAdapterInfo(pNic);
        
        if (ndisStatus != NDIS_STATUS_SUCCESS) {
            NdisWriteErrorLogEntry(pNic->MiniportAdapterHandle,
                                   NDIS_ERROR_CODE_OUT_OF_RESOURCES,
                                   0);
            ndisStatus = NDIS_STATUS_RESOURCES;
            __leave;
        }    
        ndisStatus = HwInitializeAdapter(pNic);
        
        if(ndisStatus != NDIS_STATUS_SUCCESS) {
            MpTrace(COMP_INIT_PNP, DBG_SERIOUS, ("Failed to do HW specifc initialization successfully.\n"));
            __leave;
        }
                
        if (pNic->CardInfo.HardwareID == HW_ID_8187 || pNic->CardInfo.HardwareID == HW_ID_8185) 
        {

            //
            // Allocate two lookaside lists for encryption info structure and MIC data strucure.
            //

            NdisInitializeNPagedLookasideList(&pNic->ManagementInfo.EncryptionInfoList,
                                              NULL,
                                              NULL,
                                              0,
                                              sizeof(NIC_ENCRYPTION_INFO),
                                              HW11_MEMORY_TAG,
                                              0);

            NdisInitializeNPagedLookasideList(&pNic->ManagementInfo.MICDataList,
                                              NULL,
                                              NULL,
                                              0,
                                              8,
                                              HW11_MEMORY_TAG,
                                              0);
        }

        
    }
    __finally 
    {
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            if (pNic->ManagementInfo.SwChnlWorkItem != NULL)
            {
                
                WdfObjectDelete(pNic->ManagementInfo.SwChnlWorkItem);
                pNic->ManagementInfo.SwChnlWorkItem = NULL;
            }
        }
    }

    return ndisStatus;
}

VOID
Hw11ReadRegistryConfiguration(
    __in  PNIC         pNic,
    __in  NDIS_HANDLE  hConfigurationHandle
    )
{
    NDIS_STATUS     Status = NDIS_STATUS_SUCCESS;
    PNIC_REG_ENTRY  pRegEntry;
    UINT            i;
    UINT            value = 0;
    PUCHAR          pointer;
    PNDIS_CONFIGURATION_PARAMETER ReturnedValue;
    PUCHAR          NetworkAddress;
    UINT            Length;
    
    //
    // read all the registry values 
    //
    for (i = 0, pRegEntry = NICRegTable; i < NIC_NUM_REG_PARAMS; i++, pRegEntry++)
    {

        //
        // Read the registry value if the hConfigurationHandle is valid.
        //
        Status = NDIS_STATUS_FAILURE;
        pointer = ((PUCHAR)pNic) + pRegEntry->StructOffset + pRegEntry->FieldOffset;

        if (hConfigurationHandle != NULL)
        {

            //
            // Get the configuration value for a specific parameter.  Under NT the
            // parameters are all read in as DWORDs.
            //
            NdisReadConfiguration(&Status,
                                  &ReturnedValue,
                                  hConfigurationHandle,
                                  &pRegEntry->RegName,
                                  (enum _NDIS_PARAMETER_TYPE)pRegEntry->Type);

            //
            // If the parameter was present,then check its value for validity.
            //
            if (Status == NDIS_STATUS_SUCCESS) 
            {
                if ((pRegEntry->Type == NdisParameterInteger) || (pRegEntry->Type == NdisParameterHexInteger))
                {
                    
                    //
                    // Check that param value is not too small or too large
                    //
                    if (ReturnedValue->ParameterData.IntegerData < pRegEntry->Min ||
                        ReturnedValue->ParameterData.IntegerData > pRegEntry->Max)
                    {
                        value = pRegEntry->Default;
                    }
                    else
                    {
                        value = ReturnedValue->ParameterData.IntegerData;
                    }

                }
                else {
                    //
                    // TODO: allow other types?
                    //
                    ASSERT(FALSE);
                    continue;
                }
            }
        }

        //
        // If the configuration handle is invalid or if we failed to read the
        // registry value, use the default value.
        //
        if (Status != NDIS_STATUS_SUCCESS)
        {

            //
            // If this registry value is required, we fail.
            //
            if (pRegEntry->bRequired) 
            {
                ASSERT(FALSE);
                Status = NDIS_STATUS_FAILURE;
                break;
            }
            else 
            {
                if (pRegEntry->Type == NdisParameterInteger || pRegEntry->Type == NdisParameterHexInteger) 
                {
                    value = pRegEntry->Default;
                }
                else 
                {
                    //
                    // TODO: allow other types?
                    //
                    ASSERT(FALSE);
                    continue;
                }
            }

            Status = NDIS_STATUS_SUCCESS;
        }

        //
        // Store the value in the adapter structure.
        //
        switch (pRegEntry->FieldSize) 
        {
            case 1:
                *((PUCHAR) pointer) = (UCHAR) value;
                break;

            case 2:
                *((PUSHORT) pointer) = (USHORT) value;
                break;

            case 4:
                *((PULONG) pointer) = (ULONG) value;
                break;

            default:
                break;
        }
    }

    //
    // Read NetworkAddress registry value. Use it as the current address if any.
    //
    if (Status == NDIS_STATUS_SUCCESS && hConfigurationHandle != NULL) 
    {
        NdisReadNetworkAddress(&Status,
                               (void **)&NetworkAddress,
                               &Length,
                               hConfigurationHandle);

        //
        // If there is a NetworkAddress override in registry,use it 
        //
        if ((Status == NDIS_STATUS_SUCCESS) && (Length == ETH_LENGTH_OF_ADDRESS)) 
        {
            if (!ETH_IS_MULTICAST(NetworkAddress) && !ETH_IS_BROADCAST(NetworkAddress)) 
            {
                if ((NetworkAddress[0] == 0x00) &&
                    (NetworkAddress[1] == 0x00) &&
                    (NetworkAddress[2] == 0x00) &&
                    (NetworkAddress[3] == 0x00) &&
                    (NetworkAddress[4] == 0x00) &&
                    (NetworkAddress[5] == 0x00)) 
                { // Network addr = 00 00 00 00 00 00
                    pNic->CardInfo.bOverrideAddress = FALSE;
                }
                else 
                {
                    ETH_COPY_NETWORK_ADDRESS(pNic->CardInfo.MacAddr,NetworkAddress);
                    ETH_COPY_NETWORK_ADDRESS(pNic->ManagementInfo.BssId,NetworkAddress);
                    pNic->CardInfo.bOverrideAddress = TRUE;
                }
            }
        }

        Status = NDIS_STATUS_SUCCESS;
    }
    
}

NDIS_STATUS
Hw11SelfTest(
    __in  PNIC pNic
    )
{
    UNREFERENCED_PARAMETER(pNic);
    return NDIS_STATUS_SUCCESS;
}

VOID
Hw11MiniportPause(
    __in  PNIC                                pNic,
    __in  PNDIS_MINIPORT_PAUSE_PARAMETERS     MiniportPauseParameters
    )
{
    UNREFERENCED_PARAMETER(MiniportPauseParameters);
    
    HwBusStopNotification(pNic);
}


NDIS_STATUS
Hw11MiniportRestart(
    __in  PNIC                                pNic,
    __in  PNDIS_MINIPORT_RESTART_PARAMETERS   MiniportRestartParameters
    )
{
    UNREFERENCED_PARAMETER(MiniportRestartParameters);

    Hw11EnableNotification(pNic);
    return NDIS_STATUS_SUCCESS;
}

           
ULONG
Hw11GetMPDUMaxLength(
    __in  PNIC pNic
    )
{
    UNREFERENCED_PARAMETER(pNic);
    return pNic->ManagementInfo.uMPDUMaxLength;
}

BOOL
Hw11StatusHasChanged(
    __in  PNIC        pNic
    )
{
    UNREFERENCED_PARAMETER(pNic);
    return FALSE;
}

BOOL
Hw11NicIsResetting(
    __in  PNIC pNic
    )
{
    UNREFERENCED_PARAMETER(pNic);
    return FALSE;
}

ULONG64
Hw11GetLinkSpeed(
    __in  PNIC pNic
    )
{
    //
    // Return the current link speed or default (non-zero) value 
    //
    return HwDataRateToLinkSpeed((UCHAR)pNic->ManagementInfo.CurrentTXDataRate);
}


VOID
Hw11DevicePnPEvent(
    __in  PNIC  pNic,
    __in  NDIS_DEVICE_PNP_EVENT DevicePnPEvent,
    __in  PVOID InformationBuffer,
    __in  ULONG InformationBufferLength
    )
{
    UNREFERENCED_PARAMETER(InformationBuffer);
    UNREFERENCED_PARAMETER(InformationBufferLength);

    if (DevicePnPEvent == NdisDevicePnPEventSurpriseRemoved)
    {
        //
        // If hardware has been surprise removed,remember that.
        // We have to make sure we do not try to do any I/O on ports
        // if device has been surprise removed.
        //
        pNic->CardInfo.SurpriseRemoved = TRUE;

        //
        // We must stop advertising sending beacons and probes.
        // as the hardware is no longer present
        //
        pNic->ManagementInfo.bAdvertiseBSS = FALSE;
    }
}




VOID
Hw11SetDevicePowerState(
    __in  PNIC                        pNic,
    __in  NDIS_DEVICE_POWER_STATE     DevicePowerState
    )
{
    if (DevicePowerState != NdisDeviceStateD0) {
        Hw11StopNotification(pNic);
        UpdatePortUchar(pNic, MSR, &0XF3);      // default network type to 'No  Link'   
        HwPlatformEFIOWrite1Byte(pNic, CMDR, CR_RST);     // turn of LED
        NdisStallExecution(200000);
        SetRFPowerState(pNic, eRfOff);
    }
}


NDIS_STATUS
Hw11FindAdapterAndResources(
    __in  PNIC pNic,
    __out NDIS_ERROR_CODE *ErrorCode,
    __out PULONG ErrorValue
    )
{
    UNREFERENCED_PARAMETER(ErrorCode);
    UNREFERENCED_PARAMETER(ErrorValue);
    
    HwBusFindAdapterAndResources(pNic);
    return NDIS_STATUS_SUCCESS;   
}

NDIS_STATUS
Hw11QueryStatistics(
    __in PNIC             pNic,
    __in NDIS_OID         Oid,
    __inout PVOID        StatBuffer,
    __inout PULONG       BytesWritten
    )
{
    ULONG               PhyId;
    ULONGLONG           StatValue = 0;
    NDIS_STATISTICS_INFO    StatStruct;

    NdisZeroMemory(&StatStruct, sizeof(NDIS_STATISTICS_INFO));
    StatStruct.Header.Revision = NDIS_OBJECT_REVISION_1;
    StatStruct.Header.Type = NDIS_OBJECT_TYPE_DEFAULT;
    StatStruct.Header.Size = sizeof(NDIS_STATISTICS_INFO);

    StatStruct.SupportedStatistics = NDIS_STATISTICS_FLAGS_VALID_RCV_DISCARDS          |
                                     NDIS_STATISTICS_FLAGS_VALID_RCV_ERROR             |
                                     NDIS_STATISTICS_FLAGS_VALID_XMIT_ERROR            |
                                     NDIS_STATISTICS_FLAGS_VALID_DIRECTED_FRAMES_RCV   |
                                     NDIS_STATISTICS_FLAGS_VALID_DIRECTED_FRAMES_XMIT;
                                     
    //
    // Merge phy counters
    //
    for (PhyId = 0; 
            PhyId < pNic->ManagementInfo.pSupportedPhyTypes->uNumOfEntries; 
            PhyId++
        )
    {
        switch (Oid)
        {
            case OID_GEN_XMIT_OK:
                StatValue += pNic->StatisticsInfo.PhyCounters[PhyId].ullTransmittedFragmentCount;
                break;
                
            case OID_GEN_XMIT_ERROR:
                StatValue += pNic->StatisticsInfo.PhyCounters[PhyId].ullFailedCount;
                break;

            case OID_GEN_RCV_OK:
                StatValue += pNic->StatisticsInfo.PhyCounters[PhyId].ullReceivedFragmentCount;
                break;
            
            case OID_GEN_RCV_ERROR:
                StatValue += pNic->StatisticsInfo.PhyCounters[PhyId].ullFCSErrorCount;
                break;

            case OID_GEN_RCV_CRC_ERROR:
                StatValue += pNic->StatisticsInfo.PhyCounters[PhyId].ullFCSErrorCount;
                break;

            case OID_GEN_STATISTICS:
                StatStruct.ifInDiscards += pNic->StatisticsInfo.PhyCounters[PhyId].ullFCSErrorCount;
                StatStruct.ifInErrors += pNic->StatisticsInfo.PhyCounters[PhyId].ullFCSErrorCount;
                           
                StatStruct.ifHCInUcastPkts += pNic->StatisticsInfo.PhyCounters[PhyId].ullReceivedFragmentCount;                           
                StatStruct.ifHCOutUcastPkts += pNic->StatisticsInfo.PhyCounters[PhyId].ullTransmittedFragmentCount;
                
                StatStruct.ifOutErrors += pNic->StatisticsInfo.PhyCounters[PhyId].ullFailedCount;
                break;
        }
    }

    //
    // For some OIDs, we add other stats
    //

    switch (Oid)
    {
        case OID_GEN_XMIT_ERROR:
#ifdef USE_NWIFI_SPEC_1_66
            StatValue += pNic->StatisticsInfo.TxError;
#else
            StatValue += pNic->StatisticsInfo.UcastCounters.ullTransmittedFailureFrameCount;
            StatValue += pNic->StatisticsInfo.McastCounters.ullTransmittedFailureFrameCount;
#endif
            break;
        
        case OID_GEN_RCV_ERROR:
#ifdef USE_NWIFI_SPEC_1_66
            StatValue += pNic->StatisticsInfo.RxError;
#else
            StatValue += pNic->StatisticsInfo.UcastCounters.ullReceivedFailureFrameCount;
            StatValue += pNic->StatisticsInfo.McastCounters.ullReceivedFailureFrameCount;
#endif
            break;

        case OID_GEN_RCV_NO_BUFFER:
            StatValue = pNic->StatisticsInfo.RxNoBuf;
            break;

        case OID_GEN_STATISTICS:
            StatStruct.ifInDiscards += (pNic->StatisticsInfo.RxNoBuf +
                                        pNic->StatisticsInfo.UcastCounters.ullReceivedFailureFrameCount +
                                        pNic->StatisticsInfo.McastCounters.ullReceivedFailureFrameCount
                                        );
        
            StatStruct.ifInErrors += (pNic->StatisticsInfo.UcastCounters.ullReceivedFailureFrameCount +
                                        pNic->StatisticsInfo.McastCounters.ullReceivedFailureFrameCount
                                        );
            
            StatStruct.ifOutErrors += (pNic->StatisticsInfo.UcastCounters.ullTransmittedFailureFrameCount +
                                        pNic->StatisticsInfo.McastCounters.ullTransmittedFailureFrameCount
                                        );

            break;
    }

    if (Oid == OID_GEN_STATISTICS)
    {
        NdisMoveMemory(StatBuffer, &StatStruct, sizeof(NDIS_STATISTICS_INFO));    
        *BytesWritten = sizeof(NDIS_STATISTICS_INFO);
    }
    else
    {    
        NdisMoveMemory(StatBuffer, &StatValue, sizeof(ULONGLONG));
        *BytesWritten = sizeof(ULONGLONG);
    }

    return NDIS_STATUS_SUCCESS;
}



#define ROL32(A,n)      ((A << n) | ((A >> (32 - n)) & ((1UL << n) - 1)))
#define ROR32(A,n)      ROL32(A, (32 - n))

INLINE 
VOID
HwMICBlock(
    PULONG  L,
    PULONG  R
    )
{
    *R ^= ROL32(*L, 17);
    *L += *R;
    *R ^= ((*L & 0xff00ff00) >> 8) | ((*L & 0x00ff00ff) << 8);
    *L += *R;
    *R ^= ROL32(*L, 3);
    *L += *R;
    *R ^= ROR32(*L, 2);
    *L += *R;
}

NDIS_STATUS
HwCalculateMIC(
    __in  PNET_BUFFER     NetBuffer,
    __in  UCHAR           Priority,
    __in  PUCHAR          MICKey,
    __out PUCHAR          MICData
    )
{
    ULONG                   L, R;
    ULONG                   length;
    PMDL                    mdl;
    ULONG                   offset;
    PUCHAR                  bytePtr;
    ULONG                   index;
    ULONG                   count;
    ULONG                   dataSize;
    ULONG                   data;
    PUCHAR                  SA;
    PUCHAR                  DA;
    ULONG                   headerSize;
    PDOT11_DATA_LONG_HEADER dataHeader;

    // 
    // Note: only the first MDL is guaranteed to be mapped to system virtual space.
    //

    //
    // Find data header.
    //
    mdl = NET_BUFFER_CURRENT_MDL(NetBuffer);
    dataHeader = Add2Ptr(mdl->MappedSystemVa, NET_BUFFER_CURRENT_MDL_OFFSET(NetBuffer));

    //
    // Find SA, DA and header size.
    //
    if (dataHeader->FrameControl.ToDS)
    {
        DA = dataHeader->Address3;
        if (dataHeader->FrameControl.FromDS) 
        {
            headerSize = sizeof(DOT11_DATA_LONG_HEADER);
            SA = dataHeader->Address4;
        } 
        else 
        {
            headerSize = sizeof(DOT11_DATA_SHORT_HEADER);
            SA = dataHeader->Address2;
        }
    } 
    else 
    {
        headerSize = sizeof(DOT11_DATA_SHORT_HEADER);
        DA = dataHeader->Address1;
        if (dataHeader->FrameControl.FromDS) 
        {
            SA = dataHeader->Address3;
        } 
        else 
        {
            SA = dataHeader->Address2;
        }
    }

    //
    // Find the size of data the MIC is calculated on.
    //
    dataSize = NET_BUFFER_DATA_LENGTH(NetBuffer) - headerSize;
    length = 16 + dataSize + 5;                     // MSDU + 5 bytes pad (one 0x5a, four 0s)
    if (length & 0x03)
    {
        length += 4 - (length & 0x03);              // extra 0-3 bytes of 0s pad
    }
    length >>= 2;

    //
    // Find the starting offset for actual data (payload).
    // Caller guarantees that the header is in one single MDL.
    //
    offset = NET_BUFFER_CURRENT_MDL_OFFSET(NetBuffer) + headerSize;
    ASSERT(offset <= mdl->ByteCount);
    bytePtr = Add2Ptr(mdl->MappedSystemVa, offset);

    //
    // Initial L and R value from MIC key.
    //
    L = *((ULONG UNALIGNED *)MICKey);
    R = *((ULONG UNALIGNED *)(MICKey + 4));

    //
    // Loop to calculate the MIC
    //
    for (index = 0; index < length - 1; index++)
    {
        switch (index)
        {
            case 0:
                data = *((ULONG UNALIGNED *)DA);
                break;

            case 1:
                data = *((USHORT UNALIGNED *)SA);
                data <<= 16;
                data |= *((USHORT UNALIGNED *)(DA + 4));
                break;

            case 2:
                data = *((ULONG UNALIGNED *)(SA + 2));
                break;

            case 3:
                data = Priority;
                break;

            default:
                //
                // Data
                //
                if (offset + 4 <= mdl->ByteCount && (index - 4) * 4 + 4 <= dataSize)
                {
                    //
                    // We can get the whole ULONG in contiguous virtual memory.
                    //
                    data = *((ULONG UNALIGNED *)bytePtr);
                    bytePtr += 4;
                    offset += 4;
                }
                else 
                {
                    //
                    // We can't get the whole ULONG in contiguous virtual memory.
                    // Get one byte at a time. 
                    //
                    count = 0;
                    data = 0;
                    while (count < 4)
                    {
                        ASSERT((index - 4) * 4 + count <= dataSize);
                        if ((index - 4) * 4 + count == dataSize)
                        {
                            //
                            // We are done reading. Append 0x5a at the end.
                            //
                            data |= 0x5a << (count * 8);
                            break;
                        }
                        else if (offset < mdl->ByteCount)
                        {
                            //
                            // We can get the next byte without going to next MDL
                            //
                            data |= ((ULONG)(*bytePtr)) << (count * 8);
                            bytePtr++;
                            offset++;
                            count++;
                        }
                        else 
                        {
                            //
                            // We are at the end of a MDL. Follow the link to
                            // traverse to the next MDL.
                            //
                            mdl = mdl->Next;
                            ASSERT(mdl);

                            //
                            // Map the new MDL to system virtual space.
                            //
                            bytePtr = NdisBufferVirtualAddressSafe(mdl, NormalPagePriority);
                            if (!bytePtr)
                            {
                                return NDIS_STATUS_RESOURCES;
                            }

                            offset = 0;
                        }
                    }
                }
        }
    
        L ^= data;
        HwMICBlock(&L, &R); 
    }

    HwMICBlock(&L, &R); 
    *((ULONG UNALIGNED *)MICData) = L;
    *((ULONG UNALIGNED *)(MICData + 4)) = R;

    return NDIS_STATUS_SUCCESS;
}

VOID
Hw11NotifyEventConnectionState(
    __in  PNIC            pNic,
    __in  BOOLEAN         bState
    )
{
    pNic->ManagementInfo.bDeviceConnected = bState;

    // when connection state is changed, reset counters for rate negotiation
    pNic->ManagementInfo.TotalRetryCount = 0;
    pNic->ManagementInfo.PacketsSentForTxRateCheck = 0;
    pNic->ManagementInfo.PrevTxDataRate = 0;
    pNic->ManagementInfo.TxRateIncreaseWaitCount = 0;
    pNic->ManagementInfo.TxRateIncreaseWaitRequired = 1;
}



