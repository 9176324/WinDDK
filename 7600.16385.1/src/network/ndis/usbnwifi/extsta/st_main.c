/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Sta_main.c

Abstract:
    STA layer PNP related functions (reset/initialize)
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
#include "precomp.h"
#include "St_main.h"
#include "St_aplst.h"
#include "St_conn.h"
#include "St_adhoc.h"
#include "St_scan.h"
#include "St_oids.h"

#if DOT11_TRACE_ENABLED
#include "Sta_main.tmh"
#endif

MP_REG_ENTRY STARegTable[] = {
    {NDIS_STRING_CONST("RSSIRoamBeaconCount"),  // reg value name
     STA_OFFSET(RSSIRoamBeaconCount),           // Offset in STA_REG_INFO
     STA_SIZE(RSSIRoamBeaconCount),             // Field size
     STA_INFRA_RSSI_ROAM_BEACON_COUNT_DEFAULT,  // Default Value
     STA_INFRA_RSSI_ROAM_BEACON_COUNT_MIN,      // Min
     STA_INFRA_RSSI_ROAM_BEACON_COUNT_MAX       // Max
     },

    {NDIS_STRING_CONST("RSSILinkQualityThreshold"),
     STA_OFFSET(RSSILinkQualityThreshold),
     STA_SIZE(RSSILinkQualityThreshold),
     STA_INFRA_LINK_QUALITY_ROAM_THRESHOLD_DEFAULT,
     STA_INFRA_LINK_QUALITY_ROAM_THRESHOLD_MIN,
     STA_INFRA_LINK_QUALITY_ROAM_THRESHOLD_MAX
     },

    {NDIS_STRING_CONST("LostAPRoamBeaconCount"),
     STA_OFFSET(LostAPRoamBeaconCount),
     STA_SIZE(LostAPRoamBeaconCount),
     STA_INFRA_ROAM_NO_BEACON_COUNT_DEFAULT,
     STA_INFRA_ROAM_NO_BEACON_COUNT_MIN,
     STA_INFRA_ROAM_NO_BEACON_COUNT_MAX 
     },

    {NDIS_STRING_CONST("BSSEntryExpireTime"),
     STA_OFFSET(BSSEntryExpireTime),
     STA_SIZE(BSSEntryExpireTime),
     STA_BSS_ENTRY_EXPIRE_TIME_DEFAULT,
     STA_BSS_ENTRY_EXPIRE_TIME_MIN,
     STA_BSS_ENTRY_EXPIRE_TIME_MAX 
     },

    {NDIS_STRING_CONST("BSSEntryMaxCount"),
     STA_OFFSET(BSSEntryMaxCount),
     STA_SIZE(BSSEntryMaxCount),
     STA_BSS_ENTRY_MAX_ENTRIES_DEFAULT,
     STA_BSS_ENTRY_MAX_ENTRIES_MIN,
     STA_BSS_ENTRY_MAX_ENTRIES_MAX 
     },

    {NDIS_STRING_CONST("AdhocStationMaxCount"),
     STA_OFFSET(AdhocStationMaxCount),
     STA_SIZE(AdhocStationMaxCount),
     STA_ADHOC_STA_MAX_ENTRIES_DEFAULT,
     STA_ADHOC_STA_MAX_ENTRIES_MIN,
     STA_ADHOC_STA_MAX_ENTRIES_MAX 
     },     
};

#define STA_NUM_REG_PARAMS (sizeof (STARegTable) / sizeof(MP_REG_ENTRY))


VOID 
StaIndicateDot11Status(
    __in  PSTATION        pStation,
    __in  NDIS_STATUS     StatusCode,
    __in_opt  PVOID           RequestID,
    __in  PVOID           pStatusBuffer,
    __in  ULONG           StatusBufferSize
    )
{
    NDIS_STATUS_INDICATION  statusIndication;
    
    NdisZeroMemory(&statusIndication, sizeof(NDIS_STATUS_INDICATION));
    
    //
    // Fill in object header
    //
    statusIndication.Header.Type = NDIS_OBJECT_TYPE_STATUS_INDICATION;
    statusIndication.Header.Revision = NDIS_STATUS_INDICATION_REVISION_1;
    statusIndication.Header.Size = sizeof(NDIS_STATUS_INDICATION);

    //
    // Fill in the rest of the field
    // 
    statusIndication.StatusCode = StatusCode;
    statusIndication.SourceHandle = pStation->MiniportAdapterHandle;
    statusIndication.DestinationHandle = NULL;
    statusIndication.RequestId = RequestID;
    
    statusIndication.StatusBuffer = pStatusBuffer;
    statusIndication.StatusBufferSize = StatusBufferSize;

    //
    // Indicate the status to NDIS
    //
    Mp11IndicateStatusIndication(
        pStation->pAdapter,
        &statusIndication
        );
}


NDIS_STATUS
Sta11AllocateStation(
    __in  NDIS_HANDLE     MiniportAdapterHandle,
    __out PSTATION*       ppStation
    )
{
    NDIS_STATUS     ndisStatus = NDIS_STATUS_SUCCESS;
    PSTATION        pStation = NULL;

    do
    {
        MP_ALLOCATE_MEMORY(MiniportAdapterHandle, ppStation, sizeof(STATION), STA11_MEMORY_TAG);
        if (*ppStation == NULL)
        {
            MpTrace(COMP_INIT_PNP, DBG_SERIOUS, ("Failed to allocate memory for Station\n"));
            ndisStatus = NDIS_STATUS_RESOURCES;
            break;
        }

        pStation = *ppStation;
        NdisZeroMemory(pStation, sizeof(STATION));

        pStation->MiniportAdapterHandle = MiniportAdapterHandle;

    } while (FALSE);

    return ndisStatus;
}

VOID
Sta11FreeStation(
    __in PSTATION         pStation
    )
{
    MPVERIFY(pStation);

    // Cleanup anything we create for scan
    StaFreeScanContext(pStation);

    // Cleanup connection context
    StaFreeConnectionContext(pStation);

    // Free any memory allocated for Adhoc
    StaFreeAdHocStaInfo(pStation);
    
    MP_FREE_MEMORY(pStation);
}

NDIS_STATUS
Sta11InitializeStation(
    __in  PSTATION        pStation,
    __in  PADAPTER        pAdapter,
    __in  PNIC            pNic
    )
{
    NDIS_STATUS     ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN         bFreeAdhoc = FALSE;

    do
    {
        pStation->pAdapter = pAdapter;
        pStation->pNic = pNic;

        // Setup default config
        StaInitializeStationConfig(pStation);

        StaInitializeBSSList(&pStation->BSSList);
        ndisStatus = StaInitializeAdHocStaInfo(pStation);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            break;
        }
        bFreeAdhoc = TRUE;

        ndisStatus = StaInitializeScanContext(pStation);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            break;
        }

        ndisStatus = StaInitializeConnectionContext(pStation);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            break;
        }
    } while (FALSE);

    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        StaFreeScanContext(pStation);

        if (bFreeAdhoc)
        {
            StaFreeAdHocStaInfo(pStation);
        }
    }

    return ndisStatus;
}


NDIS_STATUS
Sta11StartStation(
    __in  PSTATION        pStation
    )
{
    pStation->BSSList.MaxNumOfBSSEntries = pStation->RegInfo.BSSEntryMaxCount;

    StaStartPeriodicScan(pStation);

    return NDIS_STATUS_SUCCESS;
}

VOID
Sta11StopStation(
    __in  PSTATION        pStation,
    __in  NDIS_HALT_ACTION    HaltAction
    )
{
    UNREFERENCED_PARAMETER(HaltAction);
    
    // Periodic scan
    StaStopPeriodicScan(pStation);

    //
    // Cleanup all the connection state
    //
    StaResetConnection(pStation, FALSE);

    // Cleanup the BSS list
    Sta11FlushBSSList(pStation);

    MPVERIFY(IsListEmpty(&(pStation->BSSList.List)));
    
    // Cleanup station configuration
    StaInitializeStationConfig(pStation);

    // Cleanup the AdHoc station list
    StaResetAdHocStaInfo(pStation, TRUE);
}

VOID
Sta11ReadRegistryConfiguration(
    __in PSTATION         pStation,
    __in NDIS_HANDLE      hConfigurationHandle
    )
{
    NDIS_STATUS                     ndisStatus = NDIS_STATUS_SUCCESS;
    ULONG                           i, ValueRead;
    PUCHAR                          pucDestination;
    PMP_REG_ENTRY                   pRegEntry;
    PNDIS_CONFIGURATION_PARAMETER   Parameter = NULL;

    //
    // If the station wants to read/store any parameters from the registry
    // we would do it here. Information that we may want to load from 
    // registry includes tuning parameters for roaming, connecting, scan, etc
    //

    for(i = 0; i < STA_NUM_REG_PARAMS; i++)
    {
        //
        // Get the registry entry we will be reading
        //
        pRegEntry= &STARegTable[i];

        //
        // Figure out where in the station/reginfo structure this value will be placed
        //
        pucDestination = (PUCHAR) &(pStation->RegInfo) + pRegEntry->FieldOffset;

        //
        // Read this entry from the registry. All parameters under NT are DWORDs
        //
        NdisReadConfiguration(
            &ndisStatus,
            &Parameter,
            hConfigurationHandle,
            &pRegEntry->RegName,
            NdisParameterInteger
            );

        if (ndisStatus == NDIS_STATUS_SUCCESS)
        {
            if(Parameter->ParameterData.IntegerData < pRegEntry->Min || 
                Parameter->ParameterData.IntegerData > pRegEntry->Max)
            {
                MpTrace(COMP_INIT_PNP, DBG_SERIOUS, ("A bad value %d read from registry. Reverting to default value %d",
                                            Parameter->ParameterData.IntegerData,
                                            pRegEntry->Default
                                            ));
                ValueRead = pRegEntry->Default;
            }
            else
            {
                ValueRead = Parameter->ParameterData.IntegerData;
            }
        }
        else
        {
            MpTrace(COMP_INIT_PNP, DBG_NORMAL, ("Unable to read from registry. Reverting to default value: %d\n",
                                        pRegEntry->Default
                                        ));
            ValueRead = pRegEntry->Default;
        }

        //
        // Moving the registry values in the adapter data structure
        //
        switch(pRegEntry->FieldSize)
        {
            case 1:
                *((PUCHAR) pucDestination)  = (UCHAR) ValueRead;
                break;
            case 2:
                *((PUSHORT) pucDestination) = (USHORT) ValueRead;
                break;
            case 4:
                *((PULONG) pucDestination)  = (ULONG) ValueRead;
                break;
            default:
                MpTrace(COMP_INIT_PNP, DBG_SERIOUS, ("Bogus field size %d\n", pRegEntry->FieldSize));
                break;
        }
    }
}

VOID
Sta11ResetStep1(
    __in  PSTATION        pStation,
    __in  MP_RESET_TYPE   ResetType
    )
{
    UNREFERENCED_PARAMETER(ResetType);

    MP_SET_FLAG(pStation, STA_RESET_IN_PROGRESS);
    
    //
    // Pause the beaconing 
    //
    Hw11StopAdHoc(pStation->pNic);

    // Stop periodic scan (must call before stopping the
    // connection, since a periodic scan can start
    // off the association process)
    StaStopPeriodicScan(pStation);

    // Cleanup connection context
    StaResetConnection(pStation, TRUE);

    // Reset the state we had maintained about for roaming
    StaResetRoamState(pStation);
    
    // Reset the AdHoc station Info but do not clear AdHoc station list.
    StaResetAdHocStaInfo(pStation, FALSE);

    // Reset configuration (including resetting cipher and key on hardware)
    StaResetStationConfig(pStation);
    
    // We dont clear the BSS list on a reset 

    // Clears PMKID cache
    pStation->PMKIDCache.CheckingTime = 0;
    pStation->PMKIDCache.Count = 0;

    pStation->Statistics.ullMcastWEPExcludedCount = 0;
    pStation->Statistics.ullUcastWEPExcludedCount = 0;
}


VOID
Sta11ResetStep2(
    __in PSTATION         pStation,
    __in MP_RESET_TYPE    ResetType
    )
{
    UNREFERENCED_PARAMETER(ResetType);
    UNREFERENCED_PARAMETER(pStation);

    // CALLED AT DISPATCH
}

VOID
Sta11ResetStep4(
    __in  PSTATION        pStation,
    __in  MP_RESET_TYPE   ResetType
    )
{
    MP_CLEAR_FLAG(pStation, STA_RESET_IN_PROGRESS);

    //
    // Start scanning 
    //
    StaStartPeriodicScan(pStation);
    
    if (ResetType == MP_RESET_TYPE_NDIS_RESET)
    {
        //
        // Restart beaconing. NDIS Reset should stop beacons & probes only temporarily
        //
//        Hw11StartBSSAdvertisment(pStation->pNic);

        //
        // Here we could reestablish the connection. However,
        // to reduce state to be maintained, we indicated 
        // disconnect and let the OS reestablish the connection
        //
    }
}

VOID
StaInitializeStationConfig(
    __in PSTATION         pStation
    )
{
    PSTA_CURRENT_CONFIG         pConfig = &pStation->Config;

    //
    // free PrivacyExemptionList
    //
    if (pConfig->PrivacyExemptionList)
    {
        MP_FREE_MEMORY(pStation->Config.PrivacyExemptionList);
    }

    //
    // free IBSS params
    //
    if (pConfig->AdditionalIEData)
    {
        MP_FREE_MEMORY(pConfig->AdditionalIEData);
    }

    //
    // Any dynamically allocated structure must be freed prior to this point.
    //
    NdisZeroMemory(pConfig, sizeof(STA_CURRENT_CONFIG));

    pConfig->BSSType = dot11_BSS_type_infrastructure;
    // Wildcard SSID
    pConfig->SSID.uSSIDLength = 0;

    // Setup to accept an BSSID
    pConfig->DesiredBSSIDList[0][0] = 0xFF;
    pConfig->DesiredBSSIDList[0][1] = 0xFF;
    pConfig->DesiredBSSIDList[0][2] = 0xFF;
    pConfig->DesiredBSSIDList[0][3] = 0xFF;
    pConfig->DesiredBSSIDList[0][4] = 0xFF;
    pConfig->DesiredBSSIDList[0][5] = 0xFF;
    pConfig->DesiredBSSIDCount = 1;
    pConfig->AcceptAnyBSSID = TRUE;

    //
    // Reset other configuration parameters.
    //
    pConfig->UnreachableDetectionThreshold = 2000;
    pConfig->ExcludeUnencrypted = FALSE;

    pConfig->AID = 0;
    pConfig->ValidAID = FALSE;
    pConfig->ListenInterval = 3;        // TODO: FIX this.

    pConfig->MediaStreamingEnabled = FALSE;
    pConfig->UnicastUseGroupEnabled = TRUE;
    pConfig->HiddenNetworkEnabled = FALSE;
    
    //
    // Reset desired PHY ID list
    //
    pConfig->DesiredPhyCount = 1;
    pConfig->DesiredPhyList[0] = DOT11_PHY_ID_ANY;
    pConfig->ActivePhyId = DOT11_PHY_ID_ANY;

    //
    // This is scan specific setting 
    //
    pStation->ScanContext.SSIDInProbeRequest = TRUE;         // True until we go to low power state

}

VOID
StaResetStationConfig(
    __in PSTATION         pStation
    )
{
    //
    // Reset all state we have maintained
    //
    StaInitializeStationConfig(pStation);

    //
    // Set default auth and cipher algorithms
    //
    StaSetDefaultAuthAlgo(pStation);

    //
    // Reset power management state on hardware
    //
    StaSetPowerSavingLevel(
        pStation,
        DOT11_POWER_SAVING_NO_POWER_SAVING
        );
}

VOID
StaSetDefaultAuthAlgo(
    __in PSTATION pStation
    )
{
    PSTA_CURRENT_CONFIG     pConfig = &pStation->Config;

    //
    // Set the current authentication algorithm depending on bss type and hardware cipher implemented.
    //
    if (Hw11CCMPImplemented(pStation->pNic) || Hw11TKIPImplemented(pStation->pNic))
    {
        if (pConfig->BSSType == dot11_BSS_type_infrastructure)
        {
            pConfig->AuthAlgorithm = DOT11_AUTH_ALGO_RSNA;
        }
        else if (Hw11CCMPImplemented(pStation->pNic))
        {
            pConfig->AuthAlgorithm = DOT11_AUTH_ALGO_RSNA_PSK;
        }
        else
        {
            pConfig->AuthAlgorithm = DOT11_AUTH_ALGO_80211_OPEN;
        }
    }
    else
    {
        pConfig->AuthAlgorithm = DOT11_AUTH_ALGO_80211_OPEN;
    }

    //
    // Set the default cipher depending on the new authentication algorithm selected.
    //
    StaSetDefaultCipher(pStation);
}

VOID
StaSetDefaultCipher(
    __in PSTATION pStation
    )
{
    PNIC                    pNic = pStation->pNic;
    PSTA_CURRENT_CONFIG     pConfig = &pStation->Config;
    BOOLEAN                 WEP40Implemented = Hw11WEP40Implemented(pNic);
    BOOLEAN                 WEP104Implemented = Hw11WEP104Implemented(pNic);
    BOOLEAN                 TKIPImplemented = Hw11TKIPImplemented(pNic);
    BOOLEAN                 CCMPImplemented = Hw11CCMPImplemented(pNic);
    ULONG                   index = 0;

    switch (pConfig->AuthAlgorithm)
    {
        case DOT11_AUTH_ALGO_80211_OPEN:
            if (WEP104Implemented || WEP40Implemented)
            {
                pConfig->UnicastCipherAlgorithm = DOT11_CIPHER_ALGO_WEP;
                pConfig->MulticastCipherAlgorithmList[index++] = DOT11_CIPHER_ALGO_WEP;
            }
            else 
            {
                pConfig->UnicastCipherAlgorithm = DOT11_CIPHER_ALGO_NONE;
            }

            if (WEP104Implemented)
            {
                pConfig->MulticastCipherAlgorithmList[index++] = DOT11_CIPHER_ALGO_WEP104;
            }

            if (WEP40Implemented)
            {
                pConfig->MulticastCipherAlgorithmList[index++] = DOT11_CIPHER_ALGO_WEP40;
            }

            pConfig->MulticastCipherAlgorithmList[index++] = DOT11_CIPHER_ALGO_NONE;

            break;

        case DOT11_AUTH_ALGO_80211_SHARED_KEY:
            ASSERT(WEP104Implemented || WEP40Implemented);
            ASSERT(pStation->Config.BSSType == dot11_BSS_type_infrastructure);

            pConfig->UnicastCipherAlgorithm = DOT11_CIPHER_ALGO_WEP;
            pConfig->MulticastCipherAlgorithmList[index++] = DOT11_CIPHER_ALGO_WEP;

            if (WEP104Implemented)
            {
                pConfig->MulticastCipherAlgorithmList[index++] = DOT11_CIPHER_ALGO_WEP104;
            }

            if (WEP40Implemented)
            {
                pConfig->MulticastCipherAlgorithmList[index++] = DOT11_CIPHER_ALGO_WEP40;
            }

            break;

        case DOT11_AUTH_ALGO_WPA:
        case DOT11_AUTH_ALGO_WPA_PSK:
        case DOT11_AUTH_ALGO_RSNA:
            ASSERT(pStation->Config.BSSType == dot11_BSS_type_infrastructure);
            // fall through

        case DOT11_AUTH_ALGO_RSNA_PSK:
            ASSERT(TKIPImplemented || CCMPImplemented);
            if (CCMPImplemented)
            {
                pConfig->UnicastCipherAlgorithm = DOT11_CIPHER_ALGO_CCMP;
            }
            else 
            {
                ASSERT(pStation->Config.BSSType == dot11_BSS_type_infrastructure);
                pConfig->UnicastCipherAlgorithm = DOT11_CIPHER_ALGO_TKIP;
            }

            if (CCMPImplemented)
            {
                pConfig->MulticastCipherAlgorithmList[index++] = DOT11_CIPHER_ALGO_CCMP;
            }

            if (pStation->Config.BSSType == dot11_BSS_type_infrastructure)
            {
                if (TKIPImplemented)
                {
                    pConfig->MulticastCipherAlgorithmList[index++] = DOT11_CIPHER_ALGO_TKIP;
                }
                
                if (WEP104Implemented)
                {
                    pConfig->MulticastCipherAlgorithmList[index++] = DOT11_CIPHER_ALGO_WEP104;
                }

                if (WEP40Implemented)
                {
                    pConfig->MulticastCipherAlgorithmList[index++] = DOT11_CIPHER_ALGO_WEP40;
                }
            }

            break;

        default:
            ASSERT(0);
            return;
    }

    pConfig->MulticastCipherAlgorithmCount = index;
    if (index > 1)
    {
        pConfig->MulticastCipherAlgorithm = DOT11_CIPHER_ALGO_NONE;
    }
    else
    {
        pConfig->MulticastCipherAlgorithm = pConfig->MulticastCipherAlgorithmList[0];
    }
    
    Hw11SetEncryption(pNic, TRUE, pConfig->UnicastCipherAlgorithm);
    Hw11SetEncryption(pNic, FALSE, pConfig->MulticastCipherAlgorithm);
}

VOID
Sta11IndicateMICFailure(
    __in PSTATION                 pStation,
    __in PDOT11_DATA_SHORT_HEADER Header,
    __in ULONG                    KeyId,
    __in BOOLEAN                  IsDefaultKey
    )
{
    DOT11_TKIPMIC_FAILURE_PARAMETERS    param;

    MP_ASSIGN_NDIS_OBJECT_HEADER(param.Header, 
                                 NDIS_OBJECT_TYPE_DEFAULT,
                                 DOT11_TKIPMIC_FAILURE_PARAMETERS_REVISION_1,
                                 sizeof(DOT11_TKIPMIC_FAILURE_PARAMETERS));
    param.bDefaultKeyFailure = IsDefaultKey;
    param.uKeyIndex = KeyId;
    NdisMoveMemory(param.PeerMac, Header->Address2, sizeof(DOT11_MAC_ADDRESS));

    StaIndicateDot11Status(pStation,
                                   NDIS_STATUS_DOT11_TKIPMIC_FAILURE,
                                   NULL,
                                   &param,
                                   sizeof(DOT11_TKIPMIC_FAILURE_PARAMETERS));
}

VOID
Sta11NotifyEventRoamForSendFailures(
    __in  PSTATION                pStation
    )
{
    // this will cause StaInternalScanRequired to return TRUE and
    // forces the system to scan and roam
    pStation->ConnectContext.RoamForSendFailures = TRUE;
}

VOID
Sta11NotifyEventUpdateLinkQuality(
    __in  PSTATION        pStation,
    __in  BOOLEAN         bState
    )
{
    pStation->ConnectContext.UpdateLinkQuality = bState;
}




