/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Sta_Adhoc.c

Abstract:
    STA layer adhoc connection functions
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
#include "precomp.h"
#include "st_aplst.h"
#include "st_adhoc.h"
#include "st_scan.h"
#include <intsafe.h>

#if DOT11_TRACE_ENABLED
#include "Sta_Adhoc.tmh"
#endif

//
// The following is how Ad Hoc connection works:
//    1. We have a list of beaconing Ad Hoc stations through active or passive scanning. 
//       Go through the list, find all the stations that match our desired SSID
//       and BSSID list.
//    2. For each station found in step 1, we do the following until the start request succeeds for the 
//       a station.
//        a. Issue a join request using the station's BSSID and SSID. 
//        b. If we get response, do a start request using the station's BSSID, SSID and ATIM.
//    3. If step 2 fails, we start our own Ad Hoc network.
//    4. Indicate connection start and connection complete. At this stage, no station is indicated 
//       as associated.
//    5. When a beacon frame or probe response frame is received from a Ad Hoc station, if it
//       has not been indicated as associated but it matches with our BSSID/SSID, indicate association
//       start and associate complete for the station. If the station has been indicated as associated
//       but its SSID/BSSID do not match with ours, indicate disassociation for the station.
//    6. We have a timer routine that disassociates any station from which no beacon or probe response
//       frame is received for a specified period of time. It also removes the station from the list if 
//       no beacon or probe response is received for another specified period of time.
//

NTSTATUS
Sta11ConnectAdHoc(
    __in  PSTATION pStation
    )
{
    NDIS_STATUS ndisStatus;

    //
    // Cannot connect when scan is in progress
    //
    if (STA_TEST_SCAN_FLAG(pStation, STA_EXTERNAL_SCAN_IN_PROGRESS))
    {
        MpTrace(COMP_SCAN, DBG_SERIOUS, ("External scan already in progress. Ignoring adhoc connect request\n"));
        return NDIS_STATUS_DOT11_MEDIA_IN_USE;
    }

    //
    // Check the AdHoc state
    //

    NdisAcquireSpinLock(&pStation->AdHocStaInfo.StaInfoLock);
    
    if (pStation->AdHocStaInfo.AdHocState != STA_ADHOC_DISCONNECTED)
    {
        ndisStatus = NDIS_STATUS_INVALID_STATE;
    }
    else
    {
        ndisStatus = NDIS_STATUS_SUCCESS;
        pStation->AdHocStaInfo.AdHocState |= STA_ADHOC_CONNECTION_IN_PROGRESS;
    }

    NdisReleaseSpinLock(&pStation->AdHocStaInfo.StaInfoLock);

    //
    // Schedule a work item to do Ad Hoc connect if we can proceed.
    //

    if (ndisStatus == NDIS_STATUS_SUCCESS) 
    {
        /*NdisQueueIoWorkItem(
            pStation->AdHocStaInfo.ConnectWorkItem,
            StaConnectAdHoc,
            pStation        
            );
            */
        WdfWorkItemEnqueue(pStation->AdHocStaInfo.ConnectWorkItem);

    }

    return ndisStatus;
}

NTSTATUS
Sta11DisconnectAdHoc(
    __in  PSTATION pStation
    )
{
    DOT11_DISASSOCIATION_PARAMETERS     disassocParam;
    BOOLEAN                             cancelled;
    NDIS_STATUS                         ndisStatus;

    //
    // Check the AdHoc state
    //
    NdisAcquireSpinLock(&pStation->AdHocStaInfo.StaInfoLock);
    
    if (pStation->AdHocStaInfo.AdHocState != STA_ADHOC_CONNECTED)
    {
        ndisStatus = NDIS_STATUS_INVALID_STATE;
    }
    else
    {
        ndisStatus = NDIS_STATUS_SUCCESS;
        pStation->AdHocStaInfo.AdHocState |= STA_ADHOC_DISCONNECTION_IN_PROGRESS;

        //
        // Cancel the AdHoc watchdog timer. If we failed to cancel the timer, 
        // wait till the timer counter goes to 0.
        //
        cancelled = WdfTimerStop(pStation->AdHocStaInfo.WatchdogTimer, FALSE);
        if (!cancelled) 
        {
            while (pStation->AdHocStaInfo.TimerCounter != 0) 
            {
                NdisStallExecution(50);      // 50 us
            }
        }
        else
        {
            NdisInterlockedDecrement(&pStation->AdHocStaInfo.TimerCounter);
        }
        
    }

    NdisReleaseSpinLock(&pStation->AdHocStaInfo.StaInfoLock);

    if (ndisStatus != NDIS_STATUS_SUCCESS) 
        return ndisStatus;

    //
    // Send every station on the ad hoc network a de-auth message and clear their
    // association states.
    //
    StaClearStaListAssocState(pStation, TRUE);

    //
    // Stop beaconing and receiving data frames.
    //
    Hw11StopAdHoc(pStation->pNic);

    //
    // Indicate up DISASSOCIATION status 
    //
    MP_ASSIGN_NDIS_OBJECT_HEADER(disassocParam.Header, 
                                 NDIS_OBJECT_TYPE_DEFAULT,
                                 DOT11_DISASSOCIATION_PARAMETERS_REVISION_1,
                                 sizeof(DOT11_DISASSOCIATION_PARAMETERS));
    disassocParam.uReason = DOT11_DISASSOC_REASON_OS;
    NdisFillMemory(disassocParam.MacAddr, sizeof(DOT11_MAC_ADDRESS), 0xff);

    StaIndicateDot11Status(pStation, 
                           NDIS_STATUS_DOT11_DISASSOCIATION,
                           NULL,
                           &disassocParam,
                           sizeof(DOT11_DISASSOCIATION_PARAMETERS));

    NdisAcquireSpinLock(&pStation->AdHocStaInfo.StaInfoLock);
    pStation->AdHocStaInfo.AdHocState = STA_ADHOC_DISCONNECTED;
    NdisReleaseSpinLock(&pStation->AdHocStaInfo.StaInfoLock);

    // notify hw about disconnected status
    Hw11NotifyEventConnectionState(pStation->pNic, FALSE);
    
    return ndisStatus;
}

NDIS_STATUS
StaInitializeAdHocStaInfo(
    __in  PSTATION        pStation
    )
{
    WDF_TIMER_CONFIG       timerConfig;
    WDF_WORKITEM_CONFIG     workItemConfig;
    WDF_OBJECT_ATTRIBUTES  attributes;
    NTSTATUS                ntStatus;
    NDIS_STATUS             ndisStatus;
    PSTATION_WORKITEM_CONTEXT  stationWorkItemContext; 
    PSTATION_TIMER_SCAN_CONTEXT stationTimerContext;

    
    ndisStatus = NDIS_STATUS_SUCCESS;       
    pStation->AdHocStaInfo.StaCount = 0;
    pStation->AdHocStaInfo.DeauthStaCount = 0;
    pStation->AdHocStaInfo.AdHocState = STA_ADHOC_DISCONNECTED;
    pStation->AdHocStaInfo.JoinAdHocAfterPowerResume = FALSE;
    NdisInitializeReadWriteLock(&pStation->AdHocStaInfo.StaListLock);
    NdisInitializeListHead(&pStation->AdHocStaInfo.StaList);
    NdisAllocateSpinLock(&pStation->AdHocStaInfo.StaInfoLock);

    //
    // Allocate the work item (StaConnectAdHoc)
    //
    //pStation->AdHocStaInfo.ConnectWorkItem = NdisAllocateIoWorkItem(pStation->MiniportAdapterHandle);
    WDF_WORKITEM_CONFIG_INIT(&workItemConfig, StaConnectAdHoc);

    //
    // Specify the context type for the WDF workitem object.
    //
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes, STATION_WORKITEM_CONTEXT);
    attributes.ParentObject = pStation->pAdapter->WdfDevice;

    ntStatus = WdfWorkItemCreate(&workItemConfig,
                                &attributes,
                                &pStation->AdHocStaInfo.ConnectWorkItem);
    if (!NT_SUCCESS(ntStatus))
    {
        NT_STATUS_TO_NDIS_STATUS(ntStatus, &ndisStatus);
       
        return ndisStatus;
    }
    if(pStation->AdHocStaInfo.ConnectWorkItem == NULL)
    {
        return NDIS_STATUS_RESOURCES;
    }
    stationWorkItemContext = GetStationWorkItemContext(pStation->AdHocStaInfo.ConnectWorkItem);
    stationWorkItemContext->Station = pStation;

    NdisInitializeEvent(&pStation->AdHocStaInfo.JoinCompletionEvent);
    /*
    NdisMInitializeTimer(&pStation->AdHocStaInfo.WatchdogTimer,
                         pStation->MiniportAdapterHandle,
                         StaAdHocWatchdogTimerRoutine,
                         pStation);
                         */
    //
    // Create a WDFTIMER object
    //
    //WDF_TIMER_CONFIG_INIT_PERIODIC(&timerConfig, StaAdHocWatchdogTimerRoutine, 2000);
    WDF_TIMER_CONFIG_INIT(&timerConfig, StaAdHocWatchdogTimerRoutine);
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(
                                   &attributes,
                                   STATION_TIMER_SCAN_CONTEXT
                                   );
    attributes.ParentObject = pStation->pAdapter->WdfDevice;

    ntStatus = WdfTimerCreate(
                             &timerConfig,
                             &attributes,
                             &pStation->AdHocStaInfo.WatchdogTimer) ;    // Output handle
                             
    if (!NT_SUCCESS(ntStatus)) {
        NT_STATUS_TO_NDIS_STATUS(ntStatus, &ndisStatus);
        return ndisStatus;
    }
    stationTimerContext = GetStationTimerContext(pStation->AdHocStaInfo.WatchdogTimer);
    stationTimerContext->Station = pStation;
    pStation->AdHocStaInfo.TimerCounter = 0;

    return ndisStatus;
}

VOID
StaFreeAdHocStaInfo(
    __in  PSTATION        pStation
    )
{
    if(pStation->AdHocStaInfo.ConnectWorkItem)
    {
        //NdisFreeIoWorkItem(pStation->AdHocStaInfo.ConnectWorkItem);
        WdfObjectDelete(pStation->AdHocStaInfo.ConnectWorkItem);
        pStation->AdHocStaInfo.ConnectWorkItem = NULL;
    }
}

NDIS_STATUS 
StaSaveAdHocStaInfo(
    __in  PSTATION        pStation,
    __in  PNIC_RX_FRAGMENT    pNicFragment,
    __in  PDOT11_BEACON_FRAME pDot11BeaconFrame,
    __in  ULONG           InfoElemBlobSize
    )
{
    NDIS_STATUS             ndisStatus = NDIS_STATUS_SUCCESS;
    ULONGLONG               HostTimeStamp;
    LOCK_STATE              LockState;
    PSTA_ADHOC_STA_ENTRY    StaEntry = NULL;
    PDOT11_MGMT_HEADER      pMgmtPktHeader;
    PVOID                   pInfoElemBlob = NULL;
    PSTA_ADHOC_STA_INFO     AdHocStaInfo = &pStation->AdHocStaInfo;
    PLIST_ENTRY             pHead = NULL, pEntry = NULL;
    UCHAR                   channel;

    pMgmtPktHeader = Hw11GetFragmentDataStart(pNicFragment);
    NdisGetCurrentSystemTime((PLARGE_INTEGER)&HostTimeStamp);

    if (AdHocStaInfo->StaCount >= pStation->RegInfo.AdhocStationMaxCount)
    {
        //
        // We have reached the limit on the number of adhoc networks we would
        // maintain state for. Dont add new ones. When the adhoc watchdog
        // runs, it would reduce this number
        //
        return NDIS_STATUS_RESOURCES;
    }

    __try 
    {

        //
        // Lock the entire list
        //

        MP_ACQUIRE_WRITE_LOCK(&AdHocStaInfo->StaListLock, &LockState);

        //
        // Search if we already have the entry for the station on the list.
        //

        pHead = &AdHocStaInfo->StaList;
        pEntry = pHead->Flink;
        while (pEntry != pHead) 
        {
            StaEntry = CONTAINING_RECORD(pEntry, STA_ADHOC_STA_ENTRY, Link);
            if (MP_COMPARE_MAC_ADDRESS(StaEntry->MacAddress, pMgmtPktHeader->SA)) 
                break;

            pEntry = pEntry->Flink;
        }

        if (pEntry == pHead) 
        {
            //
            // Create a new entry for this station.
            //

            MP_ALLOCATE_MEMORY(pStation->MiniportAdapterHandle, 
                               &StaEntry, 
                               sizeof(STA_ADHOC_STA_ENTRY), 
                               STA11_MEMORY_TAG);
            if (StaEntry == NULL)
            {
                ndisStatus = NDIS_STATUS_RESOURCES;
                __leave;
            }

            //
            // Initialize the new entry
            //

            StaEntry->AllocatedIEBlobSize = 0;
            StaEntry->InfoElemBlobPtr = NULL;           
            StaEntry->InfoElemBlobSize = 0;
            StaEntry->PhyId = STA_DESIRED_PHY_MAX_COUNT;        // set to an invalid Phy ID
            StaEntry->AssocState = dot11_assoc_state_unauth_unassoc;
            NdisMoveMemory(StaEntry->MacAddress, pMgmtPktHeader->SA, sizeof(DOT11_MAC_ADDRESS));
        }

        //
        // Update the information
        //
        NdisMoveMemory(StaEntry->Dot11BSSID, pMgmtPktHeader->BSSID, sizeof(DOT11_MAC_ADDRESS)); // StaEntry is guaranteed NULL

        StaEntry->HostTimestamp = HostTimeStamp;
        StaEntry->BeaconTimestamp = pDot11BeaconFrame->Timestamp;
        StaEntry->BeaconInterval = pDot11BeaconFrame->BeaconInterval;
        StaEntry->Dot11Capability = pDot11BeaconFrame->Capability;
        StaEntry->ProbeRequestsSent = 0;

        //
        // Get channel number at which the frame was received.
        //
        if (Dot11GetChannelForDSPhy((PUCHAR)&pDot11BeaconFrame->InfoElements,
                                    InfoElemBlobSize, 
                                    &channel) != NDIS_STATUS_SUCCESS)
        {
            channel = Hw11GetFragmentChannel(pNicFragment);
        }

        if (channel != 0)
        {
            StaEntry->Channel = channel;
        }

        //
        // Increase the information blob size if necessary
        //
        if (StaEntry->AllocatedIEBlobSize < InfoElemBlobSize)
        {
            MP_ALLOCATE_MEMORY(pStation->MiniportAdapterHandle, 
                               &pInfoElemBlob, 
                               InfoElemBlobSize, 
                               STA11_MEMORY_TAG);
            if (pInfoElemBlob == NULL)
            {
                ndisStatus = NDIS_STATUS_RESOURCES;
                __leave;
            }
                
            //
            // Delete any old blob buffer
            //
            if (StaEntry->InfoElemBlobPtr)
            {
                MP_FREE_MEMORY(StaEntry->InfoElemBlobPtr);   
            }

            StaEntry->InfoElemBlobPtr = pInfoElemBlob;
            StaEntry->AllocatedIEBlobSize = InfoElemBlobSize;
        }
        
        StaEntry->InfoElemBlobSize = InfoElemBlobSize;

        //
        // Update/Save the information element block
        //
        NdisMoveMemory(StaEntry->InfoElemBlobPtr, &pDot11BeaconFrame->InfoElements, InfoElemBlobSize);

        //
        // Add the new adhoc station to our list
        //
        if (pEntry == pHead) 
        {
            InsertTailList(pHead, &StaEntry->Link);
            AdHocStaInfo->StaCount++;
        }

        //
        // Indicate the possible association/disassociation for this StaEntry 
        //

        StaAdHocIndicateAssociation(pStation, StaEntry);
    }
    __finally 
    {
        MP_RELEASE_WRITE_LOCK(&AdHocStaInfo->StaListLock, &LockState);

        if (ndisStatus != NDIS_STATUS_SUCCESS) 
        {
            if (pEntry == pHead && StaEntry) 
            {
                if (StaEntry->InfoElemBlobPtr) 
                {
                    MP_FREE_MEMORY(StaEntry->InfoElemBlobPtr);
                }
                
                MP_FREE_MEMORY(StaEntry);
            }
        }
    }

    return ndisStatus;
}

NDIS_STATUS
StaResetAdHocStaInfo(
    __in  PSTATION       pStation,
    __in  BOOLEAN        flushStaList
    )
{
    LOCK_STATE              LockState;
    PSTA_ADHOC_STA_ENTRY    StaEntry;
    PSTA_ADHOC_STA_INFO     AdHocStaInfo = &pStation->AdHocStaInfo;
    PLIST_ENTRY             pEntry = NULL;
    BOOLEAN                 cancelled;
    BOOLEAN                 connected;
    BOOLEAN                 connecting;

    NdisAcquireSpinLock(&pStation->AdHocStaInfo.StaInfoLock);
    
    connected = (BOOLEAN)(pStation->AdHocStaInfo.AdHocState == STA_ADHOC_CONNECTED);
    connecting = (BOOLEAN)(pStation->AdHocStaInfo.AdHocState & STA_ADHOC_CONNECTION_IN_PROGRESS);
    
    pStation->AdHocStaInfo.AdHocState |= STA_ADHOC_RESET_PENDING;

    //
    // If we got a reset while adhoc connect is pending (possible for NdisReset)
    // wait for the adhoc thread to finish
    //
    if (connecting)
    {
        NdisReleaseSpinLock(&pStation->AdHocStaInfo.StaInfoLock);

        //
        // We loop waiting for the connect to finish
        //
        while ((pStation->AdHocStaInfo.AdHocState & STA_ADHOC_CONNECTION_IN_PROGRESS) != 0)
        {
            NdisMSleep(10000);
        }
        
        NdisAcquireSpinLock(&pStation->AdHocStaInfo.StaInfoLock);
    }


    //
    // If we are currently connected, cancel the AdHoc watchdog timer. 
    // If we failed to cancel the timer, wait till the timer counter goes to 0.
    //
    if (connected)
    {
        //NdisMCancelTimer(&AdHocStaInfo->WatchdogTimer, &cancelled);
        cancelled = WdfTimerStop(AdHocStaInfo->WatchdogTimer, FALSE);
        if (!cancelled) 
        {
            while (AdHocStaInfo->TimerCounter != 0) 
            {
                NdisStallExecution(50);      // 50 us
            }
        }
        else
        {
            NdisInterlockedDecrement(&AdHocStaInfo->TimerCounter);
        }
        
    }

    NdisReleaseSpinLock(&pStation->AdHocStaInfo.StaInfoLock);

    //
    // Free everything on the list if asked to do so.
    //
    if (flushStaList)
    {
        MP_ACQUIRE_WRITE_LOCK(&AdHocStaInfo->StaListLock, &LockState);

        while (!IsListEmpty(&AdHocStaInfo->StaList)) 
        {
            pEntry = RemoveHeadList(&AdHocStaInfo->StaList);
            StaEntry = CONTAINING_RECORD(pEntry, STA_ADHOC_STA_ENTRY, Link);
            
            if (StaEntry->InfoElemBlobPtr) 
            {
                MP_FREE_MEMORY(StaEntry->InfoElemBlobPtr);
            }
            
            MP_FREE_MEMORY(StaEntry);
        }

        AdHocStaInfo->StaCount = 0;
        AdHocStaInfo->DeauthStaCount = 0;
        MP_RELEASE_WRITE_LOCK(&AdHocStaInfo->StaListLock, &LockState);
    }

    //
    // Stop beaconing
    //
    Hw11StopAdHoc(pStation->pNic);

    //
    // Reset state
    //
    NdisAcquireSpinLock(&pStation->AdHocStaInfo.StaInfoLock);  
    pStation->AdHocStaInfo.AdHocState = STA_ADHOC_DISCONNECTED;
    NdisReleaseSpinLock(&pStation->AdHocStaInfo.StaInfoLock);

    return NDIS_STATUS_SUCCESS;
}

void
StaClearStaListAssocState(
    __in  PSTATION    pStation,
    __in  BOOLEAN     SendDeauth
    )
{
    LOCK_STATE              LockState;
    PSTA_ADHOC_STA_ENTRY    StaEntry = NULL;
    PSTA_ADHOC_STA_INFO     AdHocStaInfo = &pStation->AdHocStaInfo;
    PLIST_ENTRY             pHead = NULL, pEntry = NULL;
    UCHAR                   Buffer[sizeof(DOT11_MGMT_HEADER) + sizeof(DOT11_DEAUTH_FRAME)];
    PDOT11_MGMT_HEADER      MgmtPacket = (PDOT11_MGMT_HEADER)Buffer;
    PDOT11_DEAUTH_FRAME     DeauthFrame;

    //
    // Initialize the de-auth message if we are to send a de-auth message
    //
    if (SendDeauth)
    {
        MgmtPacket->FrameControl.Version = 0x0;
        MgmtPacket->FrameControl.Type = DOT11_FRAME_TYPE_MANAGEMENT;
        MgmtPacket->FrameControl.Subtype = DOT11_MGMT_SUBTYPE_DEAUTHENTICATION;
        MgmtPacket->FrameControl.ToDS = 0x0;      // Default value for Mgmt frames
        MgmtPacket->FrameControl.FromDS = 0x0;    // Default value for Mgmt frames
        MgmtPacket->FrameControl.MoreFrag = 0x0;  
        MgmtPacket->FrameControl.Retry = 0x0;
        MgmtPacket->FrameControl.PwrMgt = 0x0;
        MgmtPacket->FrameControl.MoreData = 0x0;
        MgmtPacket->FrameControl.WEP = 0x0;       // no WEP
        MgmtPacket->FrameControl.Order = 0x0;     // no order
        MgmtPacket->SequenceControl.usValue = 0;

        NdisMoveMemory(MgmtPacket->SA, 
                       Hw11GetMACAddress(pStation->pNic),
                       DOT11_ADDRESS_SIZE);

        NdisMoveMemory(MgmtPacket->BSSID,
                       Hw11GetCurrentBSSID(pStation->pNic),
                       DOT11_ADDRESS_SIZE);

        DeauthFrame = (PDOT11_DEAUTH_FRAME)Add2Ptr(MgmtPacket, sizeof(DOT11_MGMT_HEADER));
        DeauthFrame->ReasonCode = DOT11_MGMT_REASON_DEAUTH_LEAVE_SS;
    }
    
    MP_ACQUIRE_WRITE_LOCK(&AdHocStaInfo->StaListLock, &LockState);

    pHead = &AdHocStaInfo->StaList;
    pEntry = pHead->Flink;
    while (pEntry != pHead) 
    {
        StaEntry = CONTAINING_RECORD(pEntry, STA_ADHOC_STA_ENTRY, Link);
        pEntry = pEntry->Flink;

        //
        // If we are to send de-auth messages, send one to each associated station.
        //
        if (SendDeauth && StaEntry->AssocState == dot11_assoc_state_auth_assoc)
        {
            NdisMoveMemory(MgmtPacket->DA, 
                           StaEntry->MacAddress,
                           DOT11_ADDRESS_SIZE);

            Hw11SendMgmtPacket(pStation->pNic, 
                               NULL, 
                               Buffer, 
                               sizeof(Buffer));

            MpTrace(COMP_ASSOC, DBG_SERIOUS, 
                ("Sent deauth packet to Ad Hoc station: %02X-%02X-%02X-%02X-%02X-%02X\n", 
                StaEntry->MacAddress[0], StaEntry->MacAddress[1], StaEntry->MacAddress[2], 
                StaEntry->MacAddress[3], StaEntry->MacAddress[4], StaEntry->MacAddress[5]));
        }

        StaEntry->AssocState = dot11_assoc_state_unauth_unassoc;
    }

    AdHocStaInfo->DeauthStaCount = 0;
    
    MP_RELEASE_WRITE_LOCK(&AdHocStaInfo->StaListLock, &LockState);
}

BOOLEAN
StaAcceptStation(
    __in  PSTATION pStation,
    __in  PSTA_ADHOC_STA_ENTRY StaEntry
    )
{
    ULONG           index;
    DOT11_PHY_TYPE  PhyType;
    DOT11_RATE_SET  rateSet;
    NDIS_STATUS     ndisStatus;
    UCHAR           SecIELength;
    PUCHAR          SecIEData;
    RSN_IE_INFO     RSNIEInfo;

    //
    // This function determines if we could accept a station on the same
    // ad hoc network as ours based on its attributes other than SSID/BSSID.
    //

    //
    // Check if the StaEntry is on the exclused MAC list
    //

    for (index = 0; index < pStation->Config.ExcludedMACAddressCount; index++) 
    {
        if (NdisEqualMemory(StaEntry->MacAddress,
                            pStation->Config.ExcludedMACAddressList[index],
                            sizeof(DOT11_MAC_ADDRESS)) == 1)
            break;
    }

    if (index < pStation->Config.ExcludedMACAddressCount)
        return FALSE;

    //
    // Check if the StaEntry matches the desired PHY list.
    //

    PhyType = Hw11DeterminePHYType(pStation->pNic, 
                                   StaEntry->Dot11Capability,
                                   StaEntry->Channel);

    StaEntry->PhyId = StaGetPhyIdByType(pStation->pNic, PhyType);
    if (!StaMatchPhyId(pStation, StaEntry->PhyId))
    {
        return FALSE;
    }

    //
    // Check if all basic data rates are supported.
    //

    ndisStatus = StaGetRateSetFromInfoEle(StaEntry->InfoElemBlobPtr,
                                          StaEntry->InfoElemBlobSize,
                                          TRUE, 
                                          &rateSet);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        return FALSE;
    }

    ndisStatus = Hw11ValidateOperationalRates(pStation->pNic,
                                              StaEntry->PhyId,
                                              rateSet.ucRateSet,
                                              rateSet.uRateSetLength);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        return FALSE;
    }

    //
    // Check for Privacy attribute.
    //

    if (StaEntry->Dot11Capability.Privacy && pStation->Config.UnicastCipherAlgorithm == DOT11_CIPHER_ALGO_NONE)
        return FALSE;

    //
    // Check RSNA IE if our auth algo is RSNA_PSK
    //
    if (pStation->Config.AuthAlgorithm == DOT11_AUTH_ALGO_RSNA_PSK)
    {
        ndisStatus = Dot11GetInfoEle(StaEntry->InfoElemBlobPtr,
                                     StaEntry->InfoElemBlobSize,
                                     DOT11_INFO_ELEMENT_ID_RSN,
                                     &SecIELength,
                                     (PVOID)&SecIEData);

        if (ndisStatus != NDIS_STATUS_SUCCESS)
            return FALSE;

        ndisStatus = StaParseRNSIE(SecIEData, RSNA_OUI_PREFIX, SecIELength, &RSNIEInfo);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
            return FALSE;

        //
        // Check if the station is running RNSA with AKM and ciphers that meet our requirement. 
        //
        if (!StaMatchRSNInfo(pStation, &RSNIEInfo))
            return FALSE;

        //
        // Save station's group cipher
        //
        StaEntry->GroupCipher = StaGetGroupCipherFromRSNIEInfo(&RSNIEInfo);
    }

    return TRUE;
}

NDIS_STATUS
StaJoinAdHoc (
    __in  PSTATION pStation,
    __in  PSTA_ADHOC_STA_ENTRY StaEntry
    )
{
    PDOT11_BSS_DESCRIPTION  BSSDescription = NULL;
    NDIS_STATUS             ndisStatus = NDIS_STATUS_SUCCESS;
    PNIC                    pNic = pStation->pNic;
    ULONG                   SizeToAlloc = 0;
    UCHAR                   channel;

    __try 
    {
        //
        // Allocate a BSS description structure (we allocate 3 bytes more for possible addition of
        // DS parameter IE, that contains channel number).
        //

        // Integer overflow        
        if (FAILED(ULongAdd(FIELD_OFFSET(DOT11_BSS_DESCRIPTION, ucBuffer),
                            StaEntry->InfoElemBlobSize, &SizeToAlloc)) ||
            FAILED(ULongAdd(SizeToAlloc, 3, &SizeToAlloc)))

        {
            ndisStatus = NDIS_STATUS_FAILURE;
            __leave;
        }

        MP_ALLOCATE_MEMORY(pStation->MiniportAdapterHandle,
                           &BSSDescription, 
                           SizeToAlloc,
                           MP_MEMORY_TAG);
        if (BSSDescription == NULL)
        {
            ndisStatus = NDIS_STATUS_RESOURCES;
            __leave;
        }
        
        //
        // Initialize the BSS description structure from StaEntry
        //

        BSSDescription->uReserved = 0;
        NdisMoveMemory(BSSDescription->dot11BSSID, StaEntry->Dot11BSSID, sizeof(DOT11_MAC_ADDRESS));
        BSSDescription->dot11BSSType = dot11_BSS_type_independent;
        BSSDescription->usBeaconPeriod = StaEntry->BeaconInterval;
        BSSDescription->ullTimestamp = StaEntry->BeaconTimestamp;
        BSSDescription->usCapabilityInformation = StaEntry->Dot11Capability.usValue;
        BSSDescription->uBufferLength = StaEntry->InfoElemBlobSize;
        NdisMoveMemory(BSSDescription->ucBuffer, StaEntry->InfoElemBlobPtr, StaEntry->InfoElemBlobSize);

        //
        // For 11a, it's possible that the Info element blob does not contain a channel number, if so and
        // if we have the channel number, add it here.
        //
        if (Dot11GetChannelForDSPhy(BSSDescription->ucBuffer,
                                    BSSDescription->uBufferLength,
                                    &channel) != NDIS_STATUS_SUCCESS &&
            StaEntry->Channel != 0)
        {
            BSSDescription->ucBuffer[BSSDescription->uBufferLength++] = DOT11_INFO_ELEMENT_ID_DS_PARAM_SET;
            BSSDescription->ucBuffer[BSSDescription->uBufferLength++] = 1;
            BSSDescription->ucBuffer[BSSDescription->uBufferLength++] = StaEntry->Channel;
        }
            
        //
        // Before we are doing the sync join, which could take a while, check if reset
        // is pending. If so, we quit.
        //

        NdisAcquireSpinLock(&pStation->AdHocStaInfo.StaInfoLock);

        if (pStation->AdHocStaInfo.AdHocState & STA_ADHOC_RESET_PENDING)
            ndisStatus = NDIS_STATUS_REQUEST_ABORTED;

        NdisReleaseSpinLock(&pStation->AdHocStaInfo.StaInfoLock);
        
        if (ndisStatus != NDIS_STATUS_SUCCESS)
            __leave;

        //
        // Reset an event for signalling the join completion. Also calculate the wait timeout.
        // Then call hardware interface function to perform join request.
        //

        NdisResetEvent(&pStation->AdHocStaInfo.JoinCompletionEvent);
        ndisStatus = Hw11JoinAdHoc(pNic, BSSDescription);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
            __leave;
        
        //
        // Wait for the event that signals join completion. In case of timeout, we need to restore the 
        // packet filter.
        //

        if (NdisWaitEvent(&pStation->AdHocStaInfo.JoinCompletionEvent, STA11_ADHOC_JOIN_TIMEOUT))
        {
            ndisStatus = NDIS_STATUS_SUCCESS;
        }
        else 
        {
            ndisStatus = STATUS_NOT_FOUND;
            Hw11ResetFilterFunction(pNic);
        }
    }
    __finally 
    {
        if (BSSDescription)
        {
            MP_FREE_MEMORY(BSSDescription);
        }
    }

    return ndisStatus;
}

VOID
Sta11JoinCompleteAdHoc(
    __in  PSTATION pStation,
    __in  NDIS_STATUS ndisStatus
    )
{
    if (ndisStatus == NDIS_STATUS_SUCCESS) 
    {
        NdisSetEvent(&pStation->AdHocStaInfo.JoinCompletionEvent);
    }
}


NDIS_STATUS
StaStartAdHoc(
    __in  PSTATION pStation,
    __in_opt  PSTA_ADHOC_STA_ENTRY StaEntry,
    __in_opt  DOT11_MAC_ADDRESS BSSID
    )
/*++

Routine Description:

    Start AdHoc mode

    The caller should guarantee serialization!!!

Arguments:

    pStation -  The pStation on which the Ad Hoc mode should be started

    StaEntry -  One of the existing pStations in the Ad Hoc network, or
                NULL if it is a new network.

    BSSID -     BSSID of the ad hoc network we are starting when StaEntry is NULL.

Return Value:

--*/
{
    PDOT11_BSS_DESCRIPTION  BSSDescription = NULL;
    DOT11_RATE_SET          rateSet;
    ULONG                   index;
    NDIS_STATUS             ndisStatus = NDIS_STATUS_SUCCESS;
    PNIC                    pNic = pStation->pNic;
    UCHAR                   channel;
    PUCHAR                  tmpPtr;
    UCHAR                   size;
    USHORT                  ATIMWindow;
    DOT11_CAPABILITY        dot11Capability = {0};
    STA_FHSS_IE             FHSSIE = {0};
    BOOL                    IEPresent;
    DOT11_SSID              dot11SSID;
    PUCHAR                  infoBlobPtr;
    USHORT                  infoBlobSize;
    DOT11_PHY_TYPE          PhyType;
    BOOLEAN                 set;

    __try 
    {
        
        //
        // Allocate a BSS description structure with maximum possible IE field.
        //

        MP_ALLOCATE_MEMORY(pStation->MiniportAdapterHandle,
                           &BSSDescription, 
                           FIELD_OFFSET(DOT11_BSS_DESCRIPTION, ucBuffer) + STA11_MAX_IE_BLOB_SIZE,
                           MP_MEMORY_TAG);
        if (BSSDescription == NULL)
        {
            ndisStatus = NDIS_STATUS_RESOURCES;
            __leave;
        }

        //
        // Initialize the BSS description structure from StaEntry or our own pStation information.
        //

        BSSDescription->uReserved = 0;
        BSSDescription->dot11BSSType = dot11_BSS_type_independent;
        BSSDescription->ullTimestamp = 0;

        if (StaEntry) 
        {
            NdisMoveMemory(BSSDescription->dot11BSSID, StaEntry->Dot11BSSID, sizeof(DOT11_MAC_ADDRESS));
            BSSDescription->usBeaconPeriod = StaEntry->BeaconInterval;
        }
        else 
        {
            NdisMoveMemory(BSSDescription->dot11BSSID, BSSID, sizeof(DOT11_MAC_ADDRESS));
            BSSDescription->usBeaconPeriod = (USHORT) Hw11QueryBeaconPeriod(pNic);
        }

        //
        // Fill the capabilityInformation 
        // 

        dot11Capability.ESS = 0;
        dot11Capability.IBSS = 1;
        dot11Capability.CFPollable = 0;     // CFPollable is always unavailable in AdHoc mode
        dot11Capability.CFPollRequest = 0;
        
        if (StaEntry)
        {
            dot11Capability.Privacy = StaEntry->Dot11Capability.Privacy;
        }
        else
        {
            dot11Capability.Privacy = (pStation->Config.UnicastCipherAlgorithm != DOT11_CIPHER_ALGO_NONE) ? 1 : 0;
        }

        PhyType = Hw11QueryCurrentPhyType(pNic);
        switch (PhyType) 
        {
            case dot11_phy_type_erp:
                Hw11QueryShortSlotTimeOptionImplemented(pNic, &set, FALSE);
                if (set)
                {
                    Hw11QueryShortSlotTimeOptionEnabled(pNic, &set, FALSE);
                }
                dot11Capability.ShortSlotTime = set ? 1 : 0;

                Hw11QueryDsssOfdmOptionImplemented(pNic, &set, FALSE);
                if (set)
                {
                    Hw11QueryDsssOfdmOptionEnabled(pNic, &set, FALSE);
                }
                dot11Capability.DSSSOFDM = set ? 1 : 0;

                // fall through

            case dot11_phy_type_hrdsss:
                Hw11QueryShortPreambleOptionImplemented(pNic, &set, FALSE);
                dot11Capability.ShortPreamble = set ? 1: 0;

                Hw11QueryPbccOptionImplemented(pNic, &set, FALSE);
                dot11Capability.PBCC = set ? 1: 0;

                Hw11QueryChannelAgilityPresent(pNic, &set, FALSE);
                if (set)
                {
                    Hw11QueryChannelAgilityEnabled(pNic, &set, FALSE);
                }
                dot11Capability.ChannelAgility = set ? 1 : 0;
        }

        BSSDescription->usCapabilityInformation = dot11Capability.usValue;  // usValue is initialized to 0

        //
        // Set the starting address and size of the info blob.
        //

        infoBlobPtr = BSSDescription->ucBuffer;
        infoBlobSize = STA11_MAX_IE_BLOB_SIZE;

        //
        // Add SSID.
        //

        if (StaEntry) 
        {
            ndisStatus = Dot11GetInfoEle(StaEntry->InfoElemBlobPtr,
                                         StaEntry->InfoElemBlobSize,
                                         DOT11_INFO_ELEMENT_ID_SSID,
                                         &size,
                                         &tmpPtr);
            if (ndisStatus != NDIS_STATUS_SUCCESS)
                __leave;

            // extra check for prefast
            if (size > DOT11_SSID_MAX_LENGTH)
            {
              ndisStatus = NDIS_STATUS_INVALID_LENGTH;
              __leave;
            }

            dot11SSID.uSSIDLength = size;
            NdisMoveMemory(dot11SSID.ucSSID, tmpPtr, dot11SSID.uSSIDLength);
        } 
        else
        {

            //
            // Use our desired SSID.
            //

            dot11SSID = pStation->Config.SSID;
        }

        ndisStatus = Dot11AttachElement(&infoBlobPtr,
                                        &infoBlobSize,
                                        DOT11_INFO_ELEMENT_ID_SSID,
                                        (UCHAR)(dot11SSID.uSSIDLength),
                                        dot11SSID.ucSSID);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
            __leave;

        //
        // Add basic rate set.
        //

        if (StaEntry) 
        {
            ndisStatus = StaGetRateSetFromInfoEle(StaEntry->InfoElemBlobPtr,
                                                  StaEntry->InfoElemBlobSize,
                                                  TRUE, 
                                                  &rateSet);
            if (ndisStatus != NDIS_STATUS_SUCCESS)
                __leave;
        }
        else 
        {
            Hw11QueryBasicRateSet(pStation->pNic, &rateSet, FALSE);
            for (index = 0; index < rateSet.uRateSetLength; index++)
            {
                rateSet.ucRateSet[index] |= 0x80;
            }
        }

        ndisStatus = Dot11AttachElement(&infoBlobPtr,
                                        &infoBlobSize,
                                        DOT11_INFO_ELEMENT_ID_SUPPORTED_RATES,
                                        (UCHAR)((rateSet.uRateSetLength > 8) ? 8 : rateSet.uRateSetLength),
                                        rateSet.ucRateSet);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
            __leave;
        
        if (rateSet.uRateSetLength > (UCHAR)8) 
        {
            ndisStatus = Dot11AttachElement(&infoBlobPtr,
                                            &infoBlobSize,
                                            DOT11_INFO_ELEMENT_ID_EXTD_SUPPORTED_RATES,
                                            (UCHAR)(rateSet.uRateSetLength - 8),
                                            rateSet.ucRateSet + 8);
            if (ndisStatus != NDIS_STATUS_SUCCESS)
                __leave;
        }
    
        //
        // Add PHY specific IEs
        //
        switch (PhyType)
        {
            case dot11_phy_type_erp:
            case dot11_phy_type_hrdsss:
            case dot11_phy_type_dsss:

                //
                // Attach DSSS IE
                //
                
                if (StaEntry) 
                {
                    ndisStatus = Dot11CopyInfoEle(StaEntry->InfoElemBlobPtr,
                                                  StaEntry->InfoElemBlobSize,
                                                  DOT11_INFO_ELEMENT_ID_DS_PARAM_SET,
                                                  &size,
                                                  sizeof(channel),
                                                  &channel);
 
                    IEPresent = (ndisStatus == NDIS_STATUS_SUCCESS && size == sizeof(channel));
                } 
                else 
                {
                    channel = (UCHAR)Hw11GetCurrentChannelId(pNic);
                    IEPresent = TRUE;
                }

                if (IEPresent)
                {
                    ndisStatus = Dot11AttachElement(&infoBlobPtr,
                                                    &infoBlobSize,
                                                    DOT11_INFO_ELEMENT_ID_DS_PARAM_SET,
                                                    sizeof(channel),
                                                    &channel);
                    if (ndisStatus != NDIS_STATUS_SUCCESS)
                        __leave;
                }

                break;

            case dot11_phy_type_fhss:

                //
                // Attach FHSS IE
                //
                
                if (StaEntry) 
                {
                    ndisStatus = Dot11CopyInfoEle(StaEntry->InfoElemBlobPtr,
                                                  StaEntry->InfoElemBlobSize,
                                                  DOT11_INFO_ELEMENT_ID_FH_PARAM_SET,
                                                  &size,
                                                  sizeof(FHSSIE),
                                                  &FHSSIE);
 
                    IEPresent = (ndisStatus == NDIS_STATUS_SUCCESS && size == sizeof(FHSSIE));
                } 
                else 
                {                    
                    IEPresent = FALSE;
                }

                if (IEPresent) 
                {
                    ndisStatus = Dot11AttachElement(&infoBlobPtr,
                                                    &infoBlobSize,
                                                    DOT11_INFO_ELEMENT_ID_FH_PARAM_SET,
                                                    sizeof(FHSSIE),
                                                    (PVOID)&FHSSIE);
                    if (ndisStatus != NDIS_STATUS_SUCCESS)
                        __leave;
                }
      
                break;

            case dot11_phy_type_ofdm:
                break;

            case dot11_phy_type_irbaseband:
                break;

            default:
                break;
        }

        //
        // Update ATIM window
        //

        if (StaEntry) 
        {
            ndisStatus = Dot11CopyInfoEle(StaEntry->InfoElemBlobPtr,
                                          StaEntry->InfoElemBlobSize,
                                          DOT11_INFO_ELEMENT_ID_IBSS_PARAM_SET,
                                          &size,
                                          sizeof(ATIMWindow),
                                          &ATIMWindow);

            if (ndisStatus == NDIS_STATUS_SUCCESS && size == sizeof(ATIMWindow)) 
            {
                ndisStatus = Hw11SetATIMWindow(pNic, (ULONG)ATIMWindow);
                if (ndisStatus != NDIS_STATUS_SUCCESS)
                    __leave;
            }
        }

        //
        // If we are running RSNA_PSK, add RSN IE.
        //
        if (pStation->Config.AuthAlgorithm == DOT11_AUTH_ALGO_RSNA_PSK)
        {
            //
            // If we are joining an exsiting network, set our group cipher to the one used by
            // the existing network if our group cipher is not specified.
            //
            if (pStation->Config.MulticastCipherAlgorithmCount > 1)
            {
                if (StaEntry)
                {
                    //
                    // If we are joining an exsiting network, set our group cipher to the one used by
                    // the existing network.
                    //
                    pStation->Config.MulticastCipherAlgorithm = StaEntry->GroupCipher;
                }
                else
                {
                    //
                    // If we are creating our own network, set our group cipher to the first one 
                    // in the enabled multicast cipher list.
                    //
                    pStation->Config.MulticastCipherAlgorithm = pStation->Config.MulticastCipherAlgorithmList[0];
                }

                //
                // Tell hardware layer what group cipher we use.
                //
                Hw11SetEncryption(pStation->pNic, FALSE, pStation->Config.MulticastCipherAlgorithm);
            }

            ndisStatus = StaAttachAdHocRSNIE(pStation, 
                                             &infoBlobPtr,
                                             &infoBlobSize);
            if (ndisStatus != NDIS_STATUS_SUCCESS)
                __leave;
        }

        //
        // Add any additional IEs set by IBSS parameter if we still have space.
        //

        if (pStation->Config.AdditionalIESize != 0 && pStation->Config.AdditionalIESize <= infoBlobSize)
        {
            NdisMoveMemory(infoBlobPtr, 
                           pStation->Config.AdditionalIEData,
                           pStation->Config.AdditionalIESize);
            infoBlobPtr += pStation->Config.AdditionalIESize;
            infoBlobSize = infoBlobSize - ((USHORT)pStation->Config.AdditionalIESize);
        }

        BSSDescription->uBufferLength = (ULONG)PtrOffset(BSSDescription->ucBuffer, infoBlobPtr);

        //
        // Start beaconing and receiving data frames.
        //

        ndisStatus = Hw11StartAdHoc(pNic, BSSDescription);
        if (ndisStatus == NDIS_STATUS_SUCCESS)
        {
            //
            // Set TX data rate and our active PhyId
            //

            Hw11SetTXDataRate(pNic, &rateSet, 100);
            pStation->Config.ActivePhyId = StaGetPhyIdByType(pNic, PhyType);
        }
    }
    __finally 
    {
        if (BSSDescription)
        {
            MP_FREE_MEMORY(BSSDescription);
        }
    }

    return ndisStatus;
}

VOID
StaResumeAdHoc(
    __in  PSTATION pStation
    )
{
    BOOLEAN connected;
    
    //
    // Get the current AdHoc state
    //
    NdisAcquireSpinLock(&pStation->AdHocStaInfo.StaInfoLock);
    connected = (BOOLEAN)(pStation->AdHocStaInfo.AdHocState == STA_ADHOC_CONNECTED);
    NdisReleaseSpinLock(&pStation->AdHocStaInfo.StaInfoLock);

    //
    // Resume beaconing only if we are currently connected
    //
    if (connected)
    {
        // device was in connected state before sleep. since hardware loses all 
        // contents after going into hibernation, we need to join adhoc network
        // again to bring the device back to the original state.
        // when waking up from hibernation, OS has to give the miniport some
        // time to re-join the network.
        // device will attempt to join when periodic scan completes.

        Sta11DisconnectAdHoc(pStation);     // send deauth to peer and indicate disconnect to OS
        pStation->AdHocStaInfo.JoinAdHocAfterPowerResume = TRUE;
    }
}

NDIS_STATUS
StaGetMatchingAdHocStaList(
    __in  PSTATION pStation,
    __out PULONG StaCount,
    __deref_out_opt PSTA_ADHOC_STA_ENTRY **StaEntryArray
    )
{
    NDIS_STATUS             ndisStatus = NDIS_STATUS_SUCCESS;
    PLIST_ENTRY             pHead = NULL, pEntry = NULL;
    PSTA_ADHOC_STA_ENTRY    StaEntry = NULL;
    ULONG                   SizeToAlloc = 0;
    LOCK_STATE              LockState;
    UCHAR                   size;
    PUCHAR                  tmpPtr;
    ULONG                   index;
    PDOT11_SSID             SSID;

    *StaEntryArray = NULL;
    *StaCount = 0;

    __try 
    {

        //
        // Only need read access to the list
        //
        
        MP_ACQUIRE_READ_LOCK(&pStation->AdHocStaInfo.StaListLock, &LockState);

        if (pStation->AdHocStaInfo.StaCount == 0 || pStation->Config.IgnoreAllMACAddresses)
            __leave;

        MP_ALLOCATE_MEMORY(pStation->MiniportAdapterHandle,
                           StaEntryArray,
                           sizeof(PSTA_ADHOC_STA_ENTRY) * pStation->AdHocStaInfo.StaCount, 
                           MP_MEMORY_TAG);          
        if ((*StaEntryArray) == NULL)
        {
            ndisStatus = NDIS_STATUS_RESOURCES;
            __leave;
        }

        //
        // Go through the adhoc station list and find all the matching stations and 
        // add them to the StaEntry array.
        //

        pHead = &pStation->AdHocStaInfo.StaList;
        pEntry = pHead->Flink;
        while (pEntry != pHead) 
        {

            StaEntry = CONTAINING_RECORD(pEntry, STA_ADHOC_STA_ENTRY, Link);
            pEntry = pEntry->Flink;

            
            //
            // Check if the StaEntry matches one of the BSSID in the BSSID list.
            //

            if (!pStation->Config.AcceptAnyBSSID) 
            {
                for (index = 0; index < pStation->Config.DesiredBSSIDCount; index++)
                {
                    if (NdisEqualMemory(pStation->Config.DesiredBSSIDList[index], 
                                        StaEntry->Dot11BSSID, 
                                        sizeof(DOT11_MAC_ADDRESS)) == 1)
                        break;
                }

                if (index == pStation->Config.DesiredBSSIDCount)
                    continue;
            }

            //
            // Check if the StaEntry matches the desired SSID.
            //

            SSID = &pStation->Config.SSID;
            if (SSID->uSSIDLength > 0) 
            {
                ndisStatus = Dot11GetInfoEle(StaEntry->InfoElemBlobPtr,
                                             StaEntry->InfoElemBlobSize,
                                             DOT11_INFO_ELEMENT_ID_SSID,
                                             &size,
                                             &tmpPtr);
                if (ndisStatus != NDIS_STATUS_SUCCESS)
                    continue;

                if (SSID->uSSIDLength != size || NdisEqualMemory(SSID->ucSSID, tmpPtr, size) == 0)
                    continue;
            }

            //
            // Check for other attributes.
            //
            if (!StaAcceptStation(pStation, StaEntry))
                continue;
            
            //
            // Found a matching station. Allocate memory for it.
            //
            if (sizeof(STA_ADHOC_STA_ENTRY) > 
               (sizeof(STA_ADHOC_STA_ENTRY) + StaEntry->InfoElemBlobSize))
            {
                ndisStatus = NDIS_STATUS_FAILURE;
                continue;
            }

            if (FAILED(ULongAdd(sizeof(STA_ADHOC_STA_ENTRY),
                                StaEntry->InfoElemBlobSize,
                                &SizeToAlloc)))
            {
                ndisStatus = NDIS_STATUS_FAILURE;
                continue;
            }

            MP_ALLOCATE_MEMORY(pStation->MiniportAdapterHandle,
                               &tmpPtr,
                               SizeToAlloc, 
                               MP_MEMORY_TAG);
            if (tmpPtr == NULL)
            {
                ndisStatus = NDIS_STATUS_RESOURCES;
                continue;
            }

            //
            // Copy the information of the matching station.
            //

            (*StaEntryArray)[*StaCount] = (PSTA_ADHOC_STA_ENTRY)tmpPtr;
            NdisMoveMemory(tmpPtr, StaEntry, sizeof(STA_ADHOC_STA_ENTRY));

            tmpPtr += sizeof(STA_ADHOC_STA_ENTRY);
            (*StaEntryArray)[*StaCount]->InfoElemBlobPtr = tmpPtr;
            NdisMoveMemory(tmpPtr, StaEntry->InfoElemBlobPtr, StaEntry->InfoElemBlobSize);

            (*StaCount)++;
        }
    }
    __finally
    {
        MP_RELEASE_READ_LOCK(&pStation->AdHocStaInfo.StaListLock, &LockState);
    }

    return ndisStatus;
}

VOID
StaConnectAdHoc(
    __in WDFWORKITEM WdfWorkItem
    )
{
    NDIS_STATUS                             ndisStatus = NDIS_STATUS_SUCCESS;
    PSTATION                                pStation;
    PNIC                                    pNic;
    ULONG                                   index;
    ULONG                                   StaCount = 0;
    PSTA_ADHOC_STA_ENTRY                   *StaEntryArray = NULL;
    PSTA_ADHOC_STA_ENTRY                    StaEntry = NULL;
    DOT11_MAC_ADDRESS                       newBSSID;
    DOT11_CONNECTION_START_PARAMETERS       connStart;
    DOT11_CONNECTION_COMPLETION_PARAMETERS  connComp;
    PSTATION_WORKITEM_CONTEXT               stationWorkItemContext;
    ULONG                                   PhyId;
    
    stationWorkItemContext = GetStationWorkItemContext(WdfWorkItem);
    pStation = stationWorkItemContext->Station;
    pNic = pStation->pNic;

    __try 
    {

        //
        // Delete all non-persistent WEP keys and clear the association state.
        //

        Hw11DeleteNonPersistentKey(pNic);
        StaClearStaListAssocState(pStation, FALSE);

        //
        // Search for an existing AdHoc network to connect to. If IBSSJoinOnly is set,
        // keep search until one is found or a reset is pending.
        //

        do
        {

            //
            // If this is not the first time we execute this loop, wait for a short while
            // to prevent us from spinning.
            //

            if (ndisStatus != NDIS_STATUS_SUCCESS)
            {
                NdisMSleep(100000);    // 100 ms.
            }
            else
            {
                //
                // Lets force a periodic scan
                //
                NdisAcquireSpinLock(&(pStation->ConnectContext.Lock));
                pStation->ScanContext.PeriodicScanCounter = STA_DEFAULT_SCAN_TICK_COUNT;
                NdisReleaseSpinLock(&(pStation->ConnectContext.Lock));
            }

            //
            // Take a snapshot of the current AdHoc station list. We can't hold the station list 
            // lock and access the list directly since connect operation is a lengthy process in 
            // which we may call wait functions. 
            //

            if (StaEntryArray) 
            {
                for (index = 0; index < StaCount; index++)
                {
                    MP_FREE_MEMORY(StaEntryArray[index]);
                }

                MP_FREE_MEMORY(StaEntryArray);
                StaEntryArray = NULL;
            }

            ndisStatus = StaGetMatchingAdHocStaList(pStation, &StaCount, &StaEntryArray);
            if (ndisStatus != NDIS_STATUS_SUCCESS) 
                __leave;

            //
            // Go through the matching station list and try to join the existing Ad Hoc network.
            //

            ndisStatus = STATUS_NOT_FOUND;
            for (index = 0; index < StaCount; index++) 
            {

                StaEntry = StaEntryArray[index];

                //
                // Do a synchronize join request with the station's SSID and BSSID.
                //

                ndisStatus = StaJoinAdHoc(pStation, StaEntry);
                if (ndisStatus == NDIS_STATUS_REQUEST_ABORTED)
                    break;
                else if (ndisStatus != NDIS_STATUS_SUCCESS) 
                    continue;

                // sucessfully joined adhoc, check for protection info in next beacon
                pStation->Config.CheckForProtectionInERP = TRUE;
                
                //
                // Start the distributed beacon for Ad Hoc
                //

                ndisStatus = StaStartAdHoc(pStation, StaEntry, NULL);
                if (ndisStatus == NDIS_STATUS_SUCCESS) 
                    break;
            }

            //
            // Check again if reset is pending.
            //

            if (ndisStatus != NDIS_STATUS_REQUEST_ABORTED)
            {
                NdisAcquireSpinLock(&pStation->AdHocStaInfo.StaInfoLock);
                if (pStation->AdHocStaInfo.AdHocState & STA_ADHOC_RESET_PENDING)
                    ndisStatus = NDIS_STATUS_REQUEST_ABORTED;
                NdisReleaseSpinLock(&pStation->AdHocStaInfo.StaInfoLock);
            }

        } while (pStation->Config.IBSSJoinOnly && 
                 ndisStatus != NDIS_STATUS_SUCCESS && 
                 ndisStatus != NDIS_STATUS_REQUEST_ABORTED);

        if (ndisStatus == NDIS_STATUS_REQUEST_ABORTED)
        {
            MpTrace(COMP_ASSOC, DBG_SERIOUS, ("AdHoc connect: Aborted due to reset\n"));
        }

        //
        // No existing matching AdHoc network found, start our own.
        //

        if (ndisStatus != NDIS_STATUS_SUCCESS && ndisStatus != NDIS_STATUS_REQUEST_ABORTED) 
        {

            //
            // start a new Adhoc cell if our beacon interval is longer than ATIM windows
            // and we have the desired SSID.
            //
            
            if (Hw11QueryBeaconPeriod(pNic) > Hw11QueryATIMWindow(pNic) &&
                pStation->Config.SSID.uSSIDLength > 0)
            {

                //
                // If we have desired BSSID list, use the first BSSID from the list as the
                // BSSID of the new Ad Hoc network. Otherwise, generate a random BSSID. 
                //

                if (!pStation->Config.AcceptAnyBSSID && pStation->Config.DesiredBSSIDCount > 0)
                    NdisMoveMemory(newBSSID, pStation->Config.DesiredBSSIDList[0], sizeof(DOT11_MAC_ADDRESS));
                else
                    StaGenerateRandomBSSID((PVOID)Hw11GetMACAddress(pNic), newBSSID);

                //
                // If our current phy type isn't in the desired phy list, set it to the first phy in
                // the list.
                //

                PhyId = Hw11QueryOperatingPhyId(pNic);
                if (!StaMatchPhyId(pStation, PhyId))
                {
                    PhyId = pStation->Config.DesiredPhyList[0];
                    ASSERT(PhyId != DOT11_PHY_ID_ANY);
                    ndisStatus = Hw11SetOperatingPhyId(pNic, PhyId);
                }
                else 
                {
                    ndisStatus = NDIS_STATUS_SUCCESS;
                }

                //
                // Start a new Ad Hoc network
                //
                if (ndisStatus == NDIS_STATUS_SUCCESS)
                {
                    ndisStatus = StaStartAdHoc(pStation, NULL, newBSSID);                
                }
            }
            else
                __leave;
        }

        if (pStation->AdHocStaInfo.JoinAdHocAfterPowerResume == TRUE)
        {
            // don't indicate connection start if joining adhoc after wake up
            // from hibernation as this join is not initiated by the OS
            pStation->AdHocStaInfo.JoinAdHocAfterPowerResume = FALSE;
            __leave;
        }
        
        //
        // Indicate connection start. If the connection status is not successful, 
        // use all-zeros as the BSSID in the connection start structure.
        //

        MP_ASSIGN_NDIS_OBJECT_HEADER(connStart.Header, 
                                     NDIS_OBJECT_TYPE_DEFAULT,
                                     DOT11_CONNECTION_START_PARAMETERS_REVISION_1,
                                     sizeof(DOT11_CONNECTION_START_PARAMETERS));
        connStart.BSSType = dot11_BSS_type_independent;
        if (ndisStatus == NDIS_STATUS_SUCCESS) 
        {
            NdisMoveMemory(connStart.AdhocBSSID, Hw11GetCurrentBSSID(pNic), sizeof(DOT11_MAC_ADDRESS));
        }
        else
        {
            NdisZeroMemory(connStart.AdhocBSSID, sizeof(DOT11_MAC_ADDRESS));
        }

        connStart.AdhocSSID = pStation->Config.SSID;

        StaIndicateDot11Status(pStation, 
                               NDIS_STATUS_DOT11_CONNECTION_START,
                               NULL,
                               &connStart,
                               sizeof(DOT11_CONNECTION_START_PARAMETERS));

        //
        // Indicate connection complete regardless of the connection status
        //
        MP_ASSIGN_NDIS_OBJECT_HEADER(connComp.Header, 
                                     NDIS_OBJECT_TYPE_DEFAULT,
                                     DOT11_CONNECTION_COMPLETION_PARAMETERS_REVISION_1,
                                     sizeof(DOT11_CONNECTION_COMPLETION_PARAMETERS));

        switch (ndisStatus)
        {
            case NDIS_STATUS_SUCCESS:
                connComp.uStatus = DOT11_CONNECTION_STATUS_SUCCESS;
                break;

            case NDIS_STATUS_REQUEST_ABORTED:
                connComp.uStatus = DOT11_CONNECTION_STATUS_CANCELLED;
                break;

            default:
                connComp.uStatus = DOT11_CONNECTION_STATUS_FAILURE;
        }

        StaIndicateDot11Status(pStation, 
                               NDIS_STATUS_DOT11_CONNECTION_COMPLETION,
                               NULL,
                               &connComp,
                               sizeof(DOT11_CONNECTION_COMPLETION_PARAMETERS));
    }
    __finally
    {
        if (StaEntryArray) 
        {
            for (index = 0; index < StaCount; index++)
            {
                MP_FREE_MEMORY(StaEntryArray[index]);
            }

            MP_FREE_MEMORY(StaEntryArray);
        }

        NdisAcquireSpinLock(&pStation->AdHocStaInfo.StaInfoLock);

        //
        // Check if reset is pending one last time.
        //
        if (ndisStatus == NDIS_STATUS_SUCCESS &&
            (pStation->AdHocStaInfo.AdHocState & STA_ADHOC_RESET_PENDING))
        {
            ndisStatus = NDIS_STATUS_REQUEST_ABORTED;
        }

        //
        // If connection is successful, start a timer watching for disconnected stations.
        //
        if (ndisStatus == NDIS_STATUS_SUCCESS) 
        {
            NdisInterlockedIncrement(&pStation->AdHocStaInfo.TimerCounter);
            WdfTimerStart(pStation->AdHocStaInfo.WatchdogTimer, WDF_REL_TIMEOUT_IN_MS(2000));
            //NdisMSetTimer(&pStation->AdHocStaInfo.WatchdogTimer, 2000);
            pStation->AdHocStaInfo.AdHocState = STA_ADHOC_CONNECTED;
        }
        else
        {
            pStation->AdHocStaInfo.AdHocState = STA_ADHOC_DISCONNECTED;
        }
        
        NdisReleaseSpinLock(&pStation->AdHocStaInfo.StaInfoLock);
    }

    return ;
}

void 
StaAdHocIndicateAssociation(
    __in  PSTATION pStation,
    __in  PSTA_ADHOC_STA_ENTRY StaEntry
    )
{
    NDIS_STATUS                                 ndisStatus = NDIS_STATUS_SUCCESS;
    UCHAR                                       size;
    PUCHAR                                      tmpPtr;
    DOT11_ASSOCIATION_START_PARAMETERS          assocStart;
    PDOT11_ASSOCIATION_COMPLETION_PARAMETERS    assocComp;
    ULONG                                       assocCompSize;
    DOT11_DISASSOCIATION_PARAMETERS             disassocParam;
    DOT11_BEACON_FRAME UNALIGNED               *beaconFrame;
    BOOLEAN                                     match;
    BOOLEAN                                     connected;
    PSTA_ADHOC_STA_INFO                         AdHocStaInfo = &pStation->AdHocStaInfo;

    //
    // If we are not in the connected state, return.
    //

    NdisAcquireSpinLock(&AdHocStaInfo->StaInfoLock);
    connected = (BOOLEAN)(AdHocStaInfo->AdHocState == STA_ADHOC_CONNECTED);
    NdisReleaseSpinLock(&AdHocStaInfo->StaInfoLock);

    if (!connected)
        return;

    //
    // Check BSSID
    //

    match = (BOOLEAN)(NdisEqualMemory(Hw11GetCurrentBSSID(pStation->pNic), 
                                      StaEntry->Dot11BSSID, 
                                      sizeof(DOT11_MAC_ADDRESS)) == 1);

    //
    // Check SSID
    //

    if (match) 
    {
        ndisStatus = Dot11GetInfoEle(StaEntry->InfoElemBlobPtr,
                                     StaEntry->InfoElemBlobSize,
                                     DOT11_INFO_ELEMENT_ID_SSID,
                                     &size,
                                     &tmpPtr);
        match = (BOOLEAN)(ndisStatus == NDIS_STATUS_SUCCESS &&
                          pStation->Config.SSID.uSSIDLength == size &&
                          NdisEqualMemory(pStation->Config.SSID.ucSSID, tmpPtr, size) == 1);
    }

    //
    // If the station was associated, but now its SSID or BSSID is changed, indicate disassociation.
    // Likewise, if the station was not associated, but its SSID and BSSID match with our current
    // BSSID and SSID, indicate association.
    //

    if (StaEntry->AssocState == dot11_assoc_state_auth_assoc && !match) 
    {

        //
        // Indicate DISASSOCIATION
        //
        MP_ASSIGN_NDIS_OBJECT_HEADER(disassocParam.Header, 
                                     NDIS_OBJECT_TYPE_DEFAULT,
                                     DOT11_DISASSOCIATION_PARAMETERS_REVISION_1,
                                     sizeof(DOT11_DISASSOCIATION_PARAMETERS));

       disassocParam.uReason = DOT11_ASSOC_STATUS_PEER_DISASSOCIATED_START | DOT11_MGMT_REASON_DISASSO_LEAVE_SS;

        NdisMoveMemory(disassocParam.MacAddr, StaEntry->MacAddress, sizeof(DOT11_MAC_ADDRESS));

        StaIndicateDot11Status(pStation, 
                               NDIS_STATUS_DOT11_DISASSOCIATION,
                               NULL,
                               &disassocParam,
                               sizeof(DOT11_DISASSOCIATION_PARAMETERS));

        //
        // This station is no longer assocated.
        //

        StaEntry->AssocState = dot11_assoc_state_unauth_unassoc;

        //
        // Delete key mapping key and per-STA key associated with the leaving station.
        //

        Hw11DeleteNonPersistentMappingKey(pStation->pNic, StaEntry->MacAddress);

        MpTrace(COMP_ASSOC, DBG_SERIOUS, 
                ("Ad Hoc station disassociated due to mismatch SSID/BSSID: %02X-%02X-%02X-%02X-%02X-%02X\n", 
                StaEntry->MacAddress[0], StaEntry->MacAddress[1], StaEntry->MacAddress[2], 
                StaEntry->MacAddress[3], StaEntry->MacAddress[4], StaEntry->MacAddress[5]));
    }
    else if (StaEntry->AssocState == dot11_assoc_state_unauth_unassoc && 
             match && 
             StaAcceptStation(pStation, StaEntry))
    {

        //
        // Allocate enough memory for ASSOCIATION_COMPLETE indication. If allocation fails,
        // we skip this beaconing station.
        //

        // Integer overflow
        if ((FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements) + StaEntry->InfoElemBlobSize) > 
                (FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements) +
                StaEntry->InfoElemBlobSize +
                sizeof(ULONG)))
        {
            ndisStatus = NDIS_STATUS_FAILURE;
            return;
        }

        if (FAILED(ULongAdd(sizeof(DOT11_ASSOCIATION_COMPLETION_PARAMETERS) + sizeof(ULONG),  // for single entry PHY list
                            StaEntry->InfoElemBlobSize, &assocCompSize)) || // for beacon
            FAILED(ULongAdd(assocCompSize, 
                            FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements), &assocCompSize)))
        {
            ndisStatus = NDIS_STATUS_RESOURCES;
            return;
        }


        MP_ALLOCATE_MEMORY(pStation->MiniportAdapterHandle,
                           &assocComp, 
                           assocCompSize,
                           MP_MEMORY_TAG);
        if (assocComp == NULL) 
        {
            ndisStatus = NDIS_STATUS_RESOURCES;
            return;
        }
    
        //
        // Indicate ASSOCIATION_START
        //
        MP_ASSIGN_NDIS_OBJECT_HEADER(assocStart.Header, 
                                     NDIS_OBJECT_TYPE_DEFAULT,
                                     DOT11_ASSOCIATION_START_PARAMETERS_REVISION_1,
                                     sizeof(DOT11_ASSOCIATION_START_PARAMETERS));
        assocStart.uIHVDataOffset = 0;
        assocStart.uIHVDataSize = 0;

        assocStart.SSID = pStation->Config.SSID;
        NdisMoveMemory(assocStart.MacAddr, StaEntry->MacAddress, sizeof(DOT11_MAC_ADDRESS));

        StaIndicateDot11Status(pStation, 
                               NDIS_STATUS_DOT11_ASSOCIATION_START,
                               NULL,
                               &assocStart,
                               sizeof(DOT11_ASSOCIATION_START_PARAMETERS));

        //
        // Indicate ASSOCIATION_COMPLETE
        // 
        MP_ASSIGN_NDIS_OBJECT_HEADER(assocComp->Header, 
                                     NDIS_OBJECT_TYPE_DEFAULT,
                                     DOT11_ASSOCIATION_COMPLETION_PARAMETERS_REVISION_1,
                                     sizeof(DOT11_ASSOCIATION_COMPLETION_PARAMETERS));

        NdisMoveMemory(assocComp->MacAddr, StaEntry->MacAddress, sizeof(DOT11_MAC_ADDRESS));
        assocComp->uStatus = 0;

        assocComp->bReAssocReq = FALSE;
        assocComp->bReAssocResp = FALSE;
        assocComp->uAssocReqOffset = 0;
        assocComp->uAssocReqSize = 0;
        assocComp->uAssocRespOffset = 0;
        assocComp->uAssocRespSize = 0;
        
        //
        // Append the beacon information of this beaconing station.
        //

        beaconFrame = (DOT11_BEACON_FRAME UNALIGNED *)
                      Add2Ptr(assocComp, sizeof(DOT11_ASSOCIATION_COMPLETION_PARAMETERS));
        beaconFrame->Timestamp = StaEntry->BeaconTimestamp;
        beaconFrame->BeaconInterval = StaEntry->BeaconInterval; 
        beaconFrame->Capability.usValue = StaEntry->Dot11Capability.usValue;
        NdisMoveMemory((PVOID)&beaconFrame->InfoElements,
                      StaEntry->InfoElemBlobPtr,
                      StaEntry->InfoElemBlobSize);
        assocComp->uBeaconOffset = sizeof(DOT11_ASSOCIATION_COMPLETION_PARAMETERS);
        assocComp->uBeaconSize = FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements) + StaEntry->InfoElemBlobSize;
        
        assocComp->uIHVDataOffset = 0;
        assocComp->uIHVDataSize = 0;
        //
        // Set the auth and cipher algorithm.
        //

        assocComp->AuthAlgo = pStation->Config.AuthAlgorithm;
        assocComp->UnicastCipher = pStation->Config.UnicastCipherAlgorithm;
        assocComp->MulticastCipher = pStation->Config.MulticastCipherAlgorithm;

        //
        // Set the PHY list. It just contains our active phy id.
        //

        assocComp->uActivePhyListOffset = sizeof(DOT11_ASSOCIATION_COMPLETION_PARAMETERS) + 
                                          FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements) +
                                          StaEntry->InfoElemBlobSize;
        assocComp->uActivePhyListSize = sizeof(ULONG);
        *((ULONG UNALIGNED *)Add2Ptr(assocComp, assocComp->uActivePhyListOffset)) = pStation->Config.ActivePhyId;

        assocComp->bFourAddressSupported = FALSE;
        assocComp->bPortAuthorized = FALSE;
        assocComp->DSInfo = DOT11_DS_UNKNOWN;

        assocComp->uEncapTableOffset = 0;
        assocComp->uEncapTableSize = 0;

        StaIndicateDot11Status(pStation, 
                               NDIS_STATUS_DOT11_ASSOCIATION_COMPLETION,
                               NULL,
                               assocComp,
                               assocCompSize);

        //
        // Free the preallocated ASSOCIATION_COMPLETE indication structure.
        //

        MP_FREE_MEMORY(assocComp);

        //
        // This station is assocated.
        //
        StaEntry->AssocState = dot11_assoc_state_auth_assoc;

        MpTrace(COMP_ASSOC, DBG_SERIOUS, ("Ad Hoc station associated: %02X-%02X-%02X-%02X-%02X-%02X\n", 
                StaEntry->MacAddress[0], StaEntry->MacAddress[1], StaEntry->MacAddress[2], 
                StaEntry->MacAddress[3], StaEntry->MacAddress[4], StaEntry->MacAddress[5]));
    }
}

void
StaProbeInactiveStation(
    __in  PSTATION pStation,
    __in  PSTA_ADHOC_STA_ENTRY StaEntry
    )
{
    char                buffer[256];    // big enough for a probe request message.
    PDOT11_MGMT_HEADER  MgmtPacket = (PDOT11_MGMT_HEADER)buffer;
    NDIS_STATUS         ndisStatus;
    PUCHAR              infoBlobPtr;
    USHORT              infoBlobSize;
    DOT11_RATE_SET      rateSet;

    MgmtPacket->FrameControl.Version = 0x0;
    MgmtPacket->FrameControl.Type = DOT11_FRAME_TYPE_MANAGEMENT;
    MgmtPacket->FrameControl.Subtype = DOT11_MGMT_SUBTYPE_PROBE_REQUEST;
    MgmtPacket->FrameControl.ToDS = 0x0;      // Default value for Mgmt frames
    MgmtPacket->FrameControl.FromDS = 0x0;    // Default value for Mgmt frames
    MgmtPacket->FrameControl.MoreFrag = 0x0;  
    MgmtPacket->FrameControl.Retry = 0x0;
    MgmtPacket->FrameControl.PwrMgt = 0x0;
    MgmtPacket->FrameControl.MoreData = 0x0;
    MgmtPacket->FrameControl.WEP = 0x0;       // no WEP
    MgmtPacket->FrameControl.Order = 0x0;     // no order
    MgmtPacket->SequenceControl.usValue = 0;

    NdisMoveMemory(MgmtPacket->DA, 
                   StaEntry->MacAddress,
                   DOT11_ADDRESS_SIZE);
    
    NdisMoveMemory(MgmtPacket->SA, 
                   Hw11GetMACAddress(pStation->pNic),
                   DOT11_ADDRESS_SIZE);

    NdisMoveMemory(MgmtPacket->BSSID,
                   Hw11GetCurrentBSSID(pStation->pNic),
                   DOT11_ADDRESS_SIZE);

    //
    // Add SSID to the probe request.
    //
    infoBlobPtr = Add2Ptr(MgmtPacket, DOT11_MGMT_HEADER_SIZE);
    infoBlobSize = sizeof(buffer) - DOT11_MGMT_HEADER_SIZE;
    
    ndisStatus = Dot11AttachElement(&infoBlobPtr,
                                    &infoBlobSize,
                                    DOT11_INFO_ELEMENT_ID_SSID,
                                    (UCHAR)pStation->Config.SSID.uSSIDLength,
                                    pStation->Config.SSID.ucSSID);

    ASSERT(ndisStatus == NDIS_STATUS_SUCCESS);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        return;

    //
    // Add supported rates to the probe request.
    //
    ndisStatus = StaGetRateSetFromInfoEle(StaEntry->InfoElemBlobPtr,
                                          StaEntry->InfoElemBlobSize,
                                          TRUE, 
                                          &rateSet);
    ASSERT(ndisStatus == NDIS_STATUS_SUCCESS);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        return;

    ndisStatus = Dot11AttachElement(&infoBlobPtr,
                                    &infoBlobSize,
                                    DOT11_INFO_ELEMENT_ID_SUPPORTED_RATES,
                                    (UCHAR)((rateSet.uRateSetLength > 8) ? 8 : rateSet.uRateSetLength),
                                    rateSet.ucRateSet);
    ASSERT(ndisStatus == NDIS_STATUS_SUCCESS);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        return;
    
    if (rateSet.uRateSetLength > (UCHAR)8) 
    {
        ndisStatus = Dot11AttachElement(&infoBlobPtr,
                                        &infoBlobSize,
                                        DOT11_INFO_ELEMENT_ID_EXTD_SUPPORTED_RATES,
                                        (UCHAR)(rateSet.uRateSetLength - 8),
                                        rateSet.ucRateSet + 8);
        ASSERT(ndisStatus == NDIS_STATUS_SUCCESS);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
            return;
    }
        
    Hw11SendMgmtPacket(pStation->pNic, 
                       NULL, 
                       (PUCHAR)MgmtPacket, 
                       sizeof(buffer) - infoBlobSize);

    MpTrace(COMP_ASSOC, DBG_SERIOUS, 
            ("Sent direct probe request to inactive Ad Hoc station: %02X-%02X-%02X-%02X-%02X-%02X\n", 
            StaEntry->MacAddress[0], StaEntry->MacAddress[1], StaEntry->MacAddress[2], 
            StaEntry->MacAddress[3], StaEntry->MacAddress[4], StaEntry->MacAddress[5]));
}

VOID
StaAdHocWatchdogTimerRoutine(
    __in WDFTIMER WdfTimer
    )
{
    PSTATION_TIMER_SCAN_CONTEXT     stationTimerContext;
    PSTATION                        pStation;
    ULONGLONG                       currentTime;
    ULONGLONG                       disassocTime;
    ULONGLONG                       removeTime;
    DOT11_DISASSOCIATION_PARAMETERS disassocParam;
    PSTA_ADHOC_STA_INFO             AdHocStaInfo ;
    PSTA_ADHOC_STA_ENTRY            StaEntry;
    PLIST_ENTRY                     pHead;
    PLIST_ENTRY                     pEntry;
    LOCK_STATE                      LockState;
    BOOLEAN                         StopTimer;
    ULONG                           index;

    stationTimerContext = GetStationTimerContext(WdfTimer);
    pStation = stationTimerContext->Station;
    AdHocStaInfo = &pStation->AdHocStaInfo;
    //
    // If we are not in the connected state, return.
    //

    NdisAcquireSpinLock(&AdHocStaInfo->StaInfoLock);
    StopTimer = (BOOLEAN)(AdHocStaInfo->AdHocState != STA_ADHOC_CONNECTED);
    NdisReleaseSpinLock(&AdHocStaInfo->StaInfoLock);

    if (StopTimer)
    {
        NdisInterlockedDecrement(&AdHocStaInfo->TimerCounter);
        return;
    }

    __try
    {
        MP_ACQUIRE_WRITE_LOCK(&AdHocStaInfo->StaListLock, &LockState);

        //
        // Calculate the various times. For Ad Hoc, since only one station in the entire network
        // will transmit beacon frame during a beacon interval, the UnreachableDetectionThreshold
        // multiplied by number of stations in the network is used as the actual threshold.
        //

        NdisGetCurrentSystemTime((PLARGE_INTEGER)&currentTime);
        disassocTime = pStation->Config.UnreachableDetectionThreshold;
        disassocTime *= (AdHocStaInfo->StaCount + 1);

        //
        // We enforce a range.
        //
        if (disassocTime < STA_ADHOC_MIN_UNREACHABLE_THRESHOLD)
            disassocTime = STA_ADHOC_MIN_UNREACHABLE_THRESHOLD;
        if (disassocTime > STA_ADHOC_MAX_UNREACHABLE_THRESHOLD)
            disassocTime = STA_ADHOC_MAX_UNREACHABLE_THRESHOLD;

        disassocTime *= 10000;
        
        removeTime = pStation->RegInfo.BSSEntryExpireTime;

        //
        // Go through the Ad Hoc station list. If we have not received a beacon or probe response from
        // an associated station for more than disassocTime, we disassociat the station. If we have not 
        // received a beacon or probe response from any station for more than removeTime, we remove the 
        // station from the list.
        //

        pHead = &AdHocStaInfo->StaList;
        pEntry = pHead->Flink;
        while (pEntry != pHead)
        {

            StaEntry = CONTAINING_RECORD(pEntry, STA_ADHOC_STA_ENTRY, Link);
            pEntry = pEntry->Flink;

            //
            // Disassociate the station if it appears on the exclusion list.
            //

            if (StaEntry->AssocState == dot11_assoc_state_auth_assoc)
            {
                for (index = 0; index < pStation->Config.ExcludedMACAddressCount; index++) 
                {
                    if (NdisEqualMemory(StaEntry->MacAddress,
                                        pStation->Config.ExcludedMACAddressList[index],
                                        sizeof(DOT11_MAC_ADDRESS)) == 1)
                        break;
                }

                if (index < pStation->Config.ExcludedMACAddressCount)
                {
                    //
                    // Indicate DISASSOCIATION
                    //
                    MP_ASSIGN_NDIS_OBJECT_HEADER(disassocParam.Header, 
                                                 NDIS_OBJECT_TYPE_DEFAULT,
                                                 DOT11_DISASSOCIATION_PARAMETERS_REVISION_1,
                                                 sizeof(DOT11_DISASSOCIATION_PARAMETERS));
                    disassocParam.uIHVDataOffset = 0;
                    disassocParam.uIHVDataSize = 0;
                    disassocParam.uReason = DOT11_DISASSOC_REASON_OS;
                    NdisMoveMemory(disassocParam.MacAddr, StaEntry->MacAddress, sizeof(DOT11_MAC_ADDRESS));

                    StaIndicateDot11Status(pStation, 
                                           NDIS_STATUS_DOT11_DISASSOCIATION,
                                           NULL,
                                           &disassocParam,
                                           sizeof(DOT11_DISASSOCIATION_PARAMETERS));

                    //
                    // This station is no longer assocated.
                    //

                    StaEntry->AssocState = dot11_assoc_state_unauth_unassoc;

                    //
                    // Delete key mapping key and per-STA key associated with the leaving station.
                    //

                    Hw11DeleteNonPersistentMappingKey(pStation->pNic, StaEntry->MacAddress);

                    MpTrace(COMP_ASSOC, DBG_SERIOUS, 
                            ("Ad Hoc station disassociated due to exclusion: %02X-%02X-%02X-%02X-%02X-%02X\n", 
                            StaEntry->MacAddress[0], StaEntry->MacAddress[1], StaEntry->MacAddress[2], 
                            StaEntry->MacAddress[3], StaEntry->MacAddress[4], StaEntry->MacAddress[5]));
                }
            }
            
            //
            // Disassociate the station if we have not received a beacon or probe response from
            // from it for more than disassocTime.
            //

            if (StaEntry->AssocState == dot11_assoc_state_auth_assoc && 
                StaEntry->HostTimestamp + disassocTime < currentTime)
            {
                if (StaEntry->ProbeRequestsSent++ < STA_PROBE_REQUEST_LIMIT)
                {
                    StaProbeInactiveStation(pStation, StaEntry);
                    continue;
                }
                
                //
                // Indicate DISASSOCIATION
                //
                MP_ASSIGN_NDIS_OBJECT_HEADER(disassocParam.Header, 
                                             NDIS_OBJECT_TYPE_DEFAULT,
                                             DOT11_DISASSOCIATION_PARAMETERS_REVISION_1,
                                             sizeof(DOT11_DISASSOCIATION_PARAMETERS));
                disassocParam.uIHVDataOffset = 0;
                disassocParam.uIHVDataSize = 0;
                disassocParam.uReason = DOT11_DISASSOC_REASON_PEER_UNREACHABLE;
                NdisMoveMemory(disassocParam.MacAddr, StaEntry->MacAddress, sizeof(DOT11_MAC_ADDRESS));

                StaIndicateDot11Status(pStation, 
                                       NDIS_STATUS_DOT11_DISASSOCIATION,
                                       NULL,
                                       &disassocParam,
                                       sizeof(DOT11_DISASSOCIATION_PARAMETERS));

                //
                // This station is no longer assocated.
                //

                StaEntry->AssocState = dot11_assoc_state_unauth_unassoc;

                //
                // Delete key mapping key and per-STA key associated with the leaving station.
                //

                Hw11DeleteNonPersistentMappingKey(pStation->pNic, StaEntry->MacAddress);

                MpTrace(COMP_ASSOC, DBG_SERIOUS, 
                        ("Ad Hoc station disassociated due to inactivity: %02X-%02X-%02X-%02X-%02X-%02X\n", 
                        StaEntry->MacAddress[0], StaEntry->MacAddress[1], StaEntry->MacAddress[2], 
                        StaEntry->MacAddress[3], StaEntry->MacAddress[4], StaEntry->MacAddress[5]));
            }

            //
            // If we just received de-auth from the station, it's in a special state. Change it to 
            // dot11_assoc_state_unauth_unassoc after it reaches its waiting period. This prevents 
            // us from re-associating a station in the situation where the station sends a de-auth 
            // frame, then sends a few beacon before finally quits.
            //

            if (StaEntry->AssocState == dot11_assoc_state_zero)
            {
                StaEntry->DeauthWaitingTick++;
                if (StaEntry->DeauthWaitingTick > STA_DEAUTH_WAITING_THRESHOLD)
                {
                    StaEntry->AssocState = dot11_assoc_state_unauth_unassoc;

                    ASSERT(AdHocStaInfo->DeauthStaCount >= 1);
                    AdHocStaInfo->DeauthStaCount--;
                }
            }

            //
            // Remove the station from the list if we have not received a beacon or probe response from
            // from it for more than removeTime.
            //

            if (StaEntry->HostTimestamp + removeTime < currentTime &&
                StaEntry->AssocState == dot11_assoc_state_unauth_unassoc) 
            {
                ASSERT(AdHocStaInfo->StaCount >= 1);
                RemoveEntryList(&StaEntry->Link);
                AdHocStaInfo->StaCount--;

                MpTrace(COMP_ASSOC, DBG_SERIOUS, ("Ad Hoc station removed: %02X-%02X-%02X-%02X-%02X-%02X\n", 
                        StaEntry->MacAddress[0], StaEntry->MacAddress[1], StaEntry->MacAddress[2], 
                        StaEntry->MacAddress[3], StaEntry->MacAddress[4], StaEntry->MacAddress[5]));

                if (StaEntry->InfoElemBlobPtr) 
                {
                    MP_FREE_MEMORY(StaEntry->InfoElemBlobPtr);
                }
                
                MP_FREE_MEMORY(StaEntry);
            }
        }
    }
    __finally
    {
        MP_RELEASE_WRITE_LOCK(&AdHocStaInfo->StaListLock, &LockState);
    }

    //
    // Set the timer again.
    //

    //NdisMSetTimer(&pStation->AdHocStaInfo.WatchdogTimer, 2000);
    WdfTimerStart(pStation->AdHocStaInfo.WatchdogTimer, WDF_REL_TIMEOUT_IN_MS(2000));

}

VOID
StaAdhocProcessMgmtPacket(
    __in  PSTATION            pStation,
    __in  PDOT11_MGMT_HEADER  MgmtPacket,
    __in  ULONG               PacketLength
    )
{
    BOOLEAN                         connected;
    PDOT11_AUTH_FRAME               AuthFrame;
    PSTA_ADHOC_STA_INFO             AdHocStaInfo = &pStation->AdHocStaInfo;
    PSTA_ADHOC_STA_ENTRY            StaEntry;
    PLIST_ENTRY                     pHead;
    PLIST_ENTRY                     pEntry;
    LOCK_STATE                      LockState;
    DOT11_DISASSOCIATION_PARAMETERS disassocParam;

    //
    // If we are not in the connected state, return.
    //

    NdisAcquireSpinLock(&AdHocStaInfo->StaInfoLock);
    connected = (BOOLEAN)(AdHocStaInfo->AdHocState == STA_ADHOC_CONNECTED);
    NdisReleaseSpinLock(&AdHocStaInfo->StaInfoLock);

    if (!connected)
        return;

    //
    // We only handle authentication and de-authentication requests.
    //
    ASSERT(MgmtPacket->FrameControl.Type == DOT11_FRAME_TYPE_MANAGEMENT);

    switch (MgmtPacket->FrameControl.Subtype)
    {
        case DOT11_MGMT_SUBTYPE_AUTHENTICATION:
            
            //
            // We only process open system authentication request. When we receive such a request,
            // if the sender's association state is dot11_assoc_state_auth_assoc, we respond with
            // a success authentication packet.
            //
            
            //
            // Check frame length.
            //
            if (PacketLength < sizeof(DOT11_MGMT_HEADER) + sizeof(DOT11_AUTH_FRAME))
                break;

            //
            // Check BSSID and DA
            //
            if (!MP_COMPARE_MAC_ADDRESS(MgmtPacket->BSSID, Hw11GetCurrentBSSID(pStation->pNic)) ||
                !MP_COMPARE_MAC_ADDRESS(MgmtPacket->DA, Hw11GetMACAddress(pStation->pNic)))
            {
                break;
            }

            //
            // Get auth frame, make sure it's open system auth request.
            //
            AuthFrame = (PDOT11_AUTH_FRAME)Add2Ptr(MgmtPacket, sizeof(DOT11_MGMT_HEADER));
            if (AuthFrame->usAlgorithmNumber != DOT11_AUTH_OPEN_SYSTEM || AuthFrame->usXid != 1)
                break;

            //
            // Go through the StaEntry list to find the sender.
            //
            MP_ACQUIRE_READ_LOCK(&AdHocStaInfo->StaListLock, &LockState);

            pHead = &AdHocStaInfo->StaList;
            pEntry = pHead->Flink;
            while (pEntry != pHead) 
            {
                StaEntry = CONTAINING_RECORD(pEntry, STA_ADHOC_STA_ENTRY, Link);
                pEntry = pEntry->Flink;

                if (MP_COMPARE_MAC_ADDRESS(MgmtPacket->SA, StaEntry->MacAddress))
                {
                    //
                    // We found the sender on the list. If the sender's association state
                    // is dot11_assoc_state_auth_assoc, we send a response.
                    //
                    if (StaEntry->AssocState == dot11_assoc_state_auth_assoc)
                    {
                        //
                        // Reuse the received frame to format the response.
                        //
                        MgmtPacket->SequenceControl.usValue = 0;
                        NdisMoveMemory(MgmtPacket->SA, MgmtPacket->DA, sizeof(DOT11_MAC_ADDRESS));
                        NdisMoveMemory(MgmtPacket->DA, StaEntry->MacAddress, sizeof(DOT11_MAC_ADDRESS));
                        AuthFrame->usXid = 2;
                        AuthFrame->usStatusCode = DOT11_FRAME_STATUS_SUCCESSFUL;
                        
                        Hw11SendMgmtPacket(pStation->pNic, 
                                           NULL, 
                                           (PUCHAR)MgmtPacket, 
                                           sizeof(DOT11_MGMT_HEADER) + sizeof(DOT11_AUTH_FRAME));
                    }

                    break;
                }
            }

            MP_RELEASE_READ_LOCK(&AdHocStaInfo->StaListLock, &LockState);

            break;

        case DOT11_MGMT_SUBTYPE_DEAUTHENTICATION:
            
            //
            // When we receive a deauthentication request, if the sender's association state is 
            // dot11_assoc_state_auth_assoc, we disassociate the sender and change its association
            // state to dot11_assoc_state_zero.
            //
            
            //
            // Check frame length.
            //
            if (PacketLength < sizeof(DOT11_MGMT_HEADER) + sizeof(DOT11_DEAUTH_FRAME))
                break;

            //
            // Check BSSID and DA
            //
            if (!MP_COMPARE_MAC_ADDRESS(MgmtPacket->BSSID, Hw11GetCurrentBSSID(pStation->pNic)) ||
                !MP_COMPARE_MAC_ADDRESS(MgmtPacket->DA, Hw11GetMACAddress(pStation->pNic)))
            {
                break;
            }

            //
            // Go through the StaEntry list to find the sender.
            //
            MP_ACQUIRE_WRITE_LOCK(&AdHocStaInfo->StaListLock, &LockState);

            pHead = &AdHocStaInfo->StaList;
            pEntry = pHead->Flink;
            while (pEntry != pHead) 
            {
                StaEntry = CONTAINING_RECORD(pEntry, STA_ADHOC_STA_ENTRY, Link);
                pEntry = pEntry->Flink;

                if (MP_COMPARE_MAC_ADDRESS(MgmtPacket->SA, StaEntry->MacAddress))
                {
                    //
                    // We found the sender on the list. If the sender's association state
                    // is dot11_assoc_state_auth_assoc, we change it to dot11_assoc_state_zero
                    // and indicate disassociation.
                    //
                    if (StaEntry->AssocState == dot11_assoc_state_auth_assoc)
                    {
                        MP_ASSIGN_NDIS_OBJECT_HEADER(disassocParam.Header, 
                                                     NDIS_OBJECT_TYPE_DEFAULT,
                                                     DOT11_DISASSOCIATION_PARAMETERS_REVISION_1,
                                                     sizeof(DOT11_DISASSOCIATION_PARAMETERS));

                        disassocParam.uReason = DOT11_ASSOC_STATUS_PEER_DISASSOCIATED_START | DOT11_MGMT_REASON_DISASSO_LEAVE_SS;
                        NdisMoveMemory(disassocParam.MacAddr, StaEntry->MacAddress, sizeof(DOT11_MAC_ADDRESS));

                        StaIndicateDot11Status(pStation, 
                                               NDIS_STATUS_DOT11_DISASSOCIATION,
                                               NULL,
                                               &disassocParam,
                                               sizeof(DOT11_DISASSOCIATION_PARAMETERS));

                        //
                        // Delete key mapping key and per-STA key associated with the leaving station.
                        //

                        Hw11DeleteNonPersistentMappingKey(pStation->pNic, StaEntry->MacAddress);

                        //
                        // This station is no longer assocated. We assign dot11_assoc_state_zero
                        // as its association state so that if we receive a beacon from it right away,
                        // we would not re-associate it.
                        //

                        StaEntry->AssocState = dot11_assoc_state_zero;
                        StaEntry->DeauthWaitingTick = 0;

                        ASSERT(AdHocStaInfo->DeauthStaCount < AdHocStaInfo->StaCount);
                        AdHocStaInfo->DeauthStaCount++;

                        MpTrace(COMP_ASSOC, DBG_SERIOUS, 
                                ("Ad Hoc station disassociated due to receiving deauth packet: %02X-%02X-%02X-%02X-%02X-%02X\n", 
                                StaEntry->MacAddress[0], StaEntry->MacAddress[1], StaEntry->MacAddress[2], 
                                StaEntry->MacAddress[3], StaEntry->MacAddress[4], StaEntry->MacAddress[5]));
                    }

                    break;
                }
            }

            MP_RELEASE_WRITE_LOCK(&AdHocStaInfo->StaListLock, &LockState);

            break;

        default:
            //
            // All other management packets are ignored.
            //
            break;
    }
}

NDIS_STATUS
Sta11EnumerateAssociationInfoAdHoc(
    __in  PSTATION        pStation,
    __out PDOT11_ASSOCIATION_INFO_LIST   pAssocInfoList,
    __in  ULONG           TotalLength
    )
{
    //LOCK_STATE              LockState;
    PSTA_ADHOC_STA_ENTRY    StaEntry = NULL;
    //PSTA_ADHOC_STA_INFO     AdHocStaInfo = &pStation->AdHocStaInfo;
    //PLIST_ENTRY             pHead = NULL, pEntry = NULL;
    PDOT11_ASSOCIATION_INFO_EX  pAssocInfo = NULL;
    ULONG                   EntrySize;
    ULONG                   RemainingBytes = 0;
    NDIS_STATUS             ndisStatus = NDIS_STATUS_SUCCESS;
    ULONG                   StaCount = 0;
    PSTA_ADHOC_STA_ENTRY    *StaEntryArray = NULL;
    ULONG                   index;

    pAssocInfo = &(pAssocInfoList->dot11AssocInfo[0]);

    RemainingBytes = TotalLength - FIELD_OFFSET(DOT11_ASSOCIATION_INFO_LIST, dot11AssocInfo);

    do
    {
        //
        // Find adhoc nodes that match our settings
        //
        ndisStatus = StaGetMatchingAdHocStaList(pStation, &StaCount, &StaEntryArray);
        if (ndisStatus != NDIS_STATUS_SUCCESS) 
        {
            // No adhoc stations found or failed when creating the list
            break;
        }

        for (index = 0; index < StaCount; index++)      
        {
            StaEntry = StaEntryArray[index];
            EntrySize = sizeof(DOT11_ASSOCIATION_INFO_EX);    // Only storing assoc info
            
            if (RemainingBytes >= EntrySize)
            {
                // Store the entry
                NdisZeroMemory(pAssocInfo, sizeof(DOT11_ASSOCIATION_INFO_EX));
    
                NdisMoveMemory(pAssocInfo->PeerMacAddress, StaEntry->MacAddress, sizeof(DOT11_MAC_ADDRESS));
                NdisMoveMemory(pAssocInfo->BSSID, StaEntry->Dot11BSSID, sizeof(DOT11_MAC_ADDRESS));
                pAssocInfo->usCapabilityInformation = StaEntry->Dot11Capability.usValue;
                pAssocInfo->usListenInterval = pStation->Config.ListenInterval;
    
                pAssocInfo->dot11AssociationState = StaEntry->AssocState;
                if (pAssocInfo->dot11AssociationState == dot11_assoc_state_zero)
                    pAssocInfo->dot11AssociationState = dot11_assoc_state_unauth_unassoc;
                    
                MpTrace(COMP_OID, DBG_LOUD, 
                        ("Assoc State For %02X-%02X-%02X-%02X-%02X-%02X is %d\n", 
                        pAssocInfo->PeerMacAddress[0], pAssocInfo->PeerMacAddress[1], 
                        pAssocInfo->PeerMacAddress[2], pAssocInfo->PeerMacAddress[3], 
                        pAssocInfo->PeerMacAddress[4], pAssocInfo->PeerMacAddress[5], 
                        pAssocInfo->dot11AssociationState));
    
                // TODO: Supported rates
    
                //
                // We do not keep per station statistics
                //
                
                pAssocInfoList->uNumOfEntries++;        
                pAssocInfoList->uTotalNumOfEntries++;
                RemainingBytes -= EntrySize;
                pAssocInfo++;
            }
            else
            {
                // Not enough space to store this entry
                pAssocInfoList->uTotalNumOfEntries++;
                ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
                RemainingBytes = 0;
                //
                // We continue walking through the list to determine the total
                // space required for this OID
                //            
            }
        }
    } while (FALSE);
    
    if (StaEntryArray) 
    {
        for (index = 0; index < StaCount; index++)
        {
            MP_FREE_MEMORY(StaEntryArray[index]);
        }

        MP_FREE_MEMORY(StaEntryArray);
    }    

    return ndisStatus;
}

void
StaAdHocReceiveDirectData(
    __in  PSTATION                    pStation,
    __in  PDOT11_DATA_SHORT_HEADER    pDataHdr
    )
{
    BOOLEAN                         connected;
    PSTA_ADHOC_STA_INFO             AdHocStaInfo = &pStation->AdHocStaInfo;
    PSTA_ADHOC_STA_ENTRY            StaEntry;
    PLIST_ENTRY                     pHead;
    PLIST_ENTRY                     pEntry;
    LOCK_STATE                      LockState;
    
    //
    // If we are not in the connected state, return.
    //

    NdisAcquireSpinLock(&AdHocStaInfo->StaInfoLock);
    connected = (BOOLEAN)(AdHocStaInfo->AdHocState == STA_ADHOC_CONNECTED);
    NdisReleaseSpinLock(&AdHocStaInfo->StaInfoLock);

    if (!connected)
        return;

    //
    // Find if the station that sent this data frame also just sent us de-auth frame,
    // if so, the station must have tried to re-connect to the ad hoc network. In this
    // case, change its associate state to dot11_assoc_state_unauth_unassoc.
    //

    __try
    {
        MP_ACQUIRE_WRITE_LOCK(&AdHocStaInfo->StaListLock, &LockState);

        if (AdHocStaInfo->DeauthStaCount == 0)
            __leave;

        pHead = &AdHocStaInfo->StaList;
        pEntry = pHead->Flink;
        while (pEntry != pHead) 
        {
            StaEntry = CONTAINING_RECORD(pEntry, STA_ADHOC_STA_ENTRY, Link);
            pEntry = pEntry->Flink;

            if (MP_COMPARE_MAC_ADDRESS(pDataHdr->Address2, StaEntry->MacAddress) &&
                StaEntry->AssocState == dot11_assoc_state_zero)
            {
                StaEntry->AssocState = dot11_assoc_state_unauth_unassoc;
                AdHocStaInfo->DeauthStaCount--;
                break;
            }
                
        }
    }
    __finally
    {
        MP_RELEASE_WRITE_LOCK(&AdHocStaInfo->StaListLock, &LockState);
    }
}

