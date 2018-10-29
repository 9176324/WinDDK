/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Sta_Scan.c

Abstract:
    STA layer OS/internal scan functions
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
#include "precomp.h"
#include "st_aplst.h"
#include "st_scan.h"
#include "st_conn.h"

#if DOT11_TRACE_ENABLED
    #include "Sta_scan.tmh"
#endif


VOID
StaStopPeriodicScan(
                   __in  PSTATION            pStation
                   )
{
    BOOLEAN                 scanTimerCancelled;

    // MUST BE CALLED BELOW DISPATCH

    NdisAcquireSpinLock(&(pStation->ConnectContext.Lock));

    //
    // Stop the scan from requeuing itself
    //
    STA_SET_SCAN_FLAG(pStation, STA_STOP_PERIODIC_SCAN);

    //
    // Cancel any scans that may be queued by not yet run
    //
    scanTimerCancelled = WdfTimerStop(pStation->ScanContext.Timer_PeriodicScan, FALSE);
    if (scanTimerCancelled) {
        STA_CLEAR_SCAN_FLAG(pStation, (STA_PERIODIC_SCAN_IN_PROGRESS | STA_PERIODIC_SCAN_QUEUED));
    }

    //
    // Cancel any scans that are already running
    //
    if (STA_TEST_SCAN_FLAG(pStation, STA_PERIODIC_SCAN_IN_PROGRESS)) {
        NdisReleaseSpinLock(&(pStation->ConnectContext.Lock));

        //
        // Wait for the periodic scan to finish
        //
        StaWaitForPeriodicScan(pStation);
    }
    else {
        NdisReleaseSpinLock(&(pStation->ConnectContext.Lock));
    }    
}


VOID
StaFreeScanContext(
                  __in  PSTATION            pStation
                  )
{
    // Periodic scan is already stopped and cleaned up

    if (pStation->ScanContext.InternalScanRequestBuffer != NULL) {
        MP_FREE_MEMORY(pStation->ScanContext.InternalScanRequestBuffer);
        pStation->ScanContext.InternalScanRequestBuffer = NULL;
    }
    pStation->ScanContext.InternalScanBufferLength = 0;
}



NDIS_STATUS
StaInitializeScanContext(
                        __in  PSTATION            pStation
                        )
{
    ULONG       ScanRequestLength = 0;
    PDOT11_SCAN_REQUEST_V2  pInternalScanRequestBuffer = NULL;
    NTSTATUS        ntStatus;
    NDIS_STATUS             ndisStatus;
    WDF_TIMER_CONFIG       config;
    WDF_OBJECT_ATTRIBUTES  attributes;
    PSTATION_TIMER_SCAN_CONTEXT stationTimerContext ;

    // Create a WDFTIMER object
    //
    WDF_TIMER_CONFIG_INIT(&config, StaPeriodicScanCallback );

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(
                                   &attributes,
                                   STATION_TIMER_SCAN_CONTEXT
                                   );
    attributes.ParentObject = pStation->pAdapter->WdfDevice;

    ntStatus = WdfTimerCreate(
                             &config,
                             &attributes,
                             &(pStation->ScanContext.Timer_PeriodicScan)     // Output handle
                             );

    if (!NT_SUCCESS(ntStatus)) {
        NT_STATUS_TO_NDIS_STATUS(ntStatus, &ndisStatus);
        return ndisStatus;
    }
    stationTimerContext = GetStationTimerContext(pStation->ScanContext.Timer_PeriodicScan);
    stationTimerContext->Station = pStation;
   
    /*
    NdisMInitializeTimer(
        &(pStation->ScanContext.Timer_PeriodicScan),
        pStation->MiniportAdapterHandle,
        StaPeriodicScanCallback,
        pStation
        );
    */

    //
    // Determine the total length of the scan request
    //
    ScanRequestLength = sizeof(DOT11_SCAN_REQUEST_V2) +                 // The basic structure
                        + sizeof(DOT11_SSID)                                           // 1 SSID
                        + STA_DESIRED_PHY_MAX_COUNT * sizeof(DOT11_PHY_TYPE_INFO)   // PHY types info. We dont specify channels
                        ;                                                              // No IE's or RequestIDs

    MP_ALLOCATE_MEMORY(
                      pStation->MiniportAdapterHandle,
                      &pInternalScanRequestBuffer,
                      ScanRequestLength,
                      STA11_MEMORY_TAG
                      );

    if (pInternalScanRequestBuffer == NULL) {
        return NDIS_STATUS_RESOURCES;
    }

    //
    // Fill defaults into the internal scan request buffer
    //
    NdisZeroMemory(pInternalScanRequestBuffer, ScanRequestLength);    
    pInternalScanRequestBuffer->dot11BSSType = dot11_BSS_type_infrastructure;
    pInternalScanRequestBuffer->dot11BSSID[0] = 0xFF;
    pInternalScanRequestBuffer->dot11BSSID[1] = 0xFF;
    pInternalScanRequestBuffer->dot11BSSID[2] = 0xFF;
    pInternalScanRequestBuffer->dot11BSSID[3] = 0xFF;
    pInternalScanRequestBuffer->dot11BSSID[4] = 0xFF;
    pInternalScanRequestBuffer->dot11BSSID[5] = 0xFF;

    pInternalScanRequestBuffer->dot11ScanType = dot11_scan_type_auto;
    pInternalScanRequestBuffer->bRestrictedScan = FALSE;

    pInternalScanRequestBuffer->udot11SSIDsOffset = 0;
    pInternalScanRequestBuffer->uNumOfdot11SSIDs = 0;

    pInternalScanRequestBuffer->bUseRequestIE = FALSE;
    pInternalScanRequestBuffer->uNumOfRequestIDs = 0;

    pInternalScanRequestBuffer->uPhyTypeInfosOffset = sizeof(DOT11_SSID);   // Phy Info follows SSID

    pInternalScanRequestBuffer->uNumOfPhyTypeInfos = 0;
    // Phy info structures are all zeroed out

    pInternalScanRequestBuffer->uIEsOffset = 0;
    pInternalScanRequestBuffer->uIEsLength = 0;

    //
    // Save it. We will use this for all internal scan requests
    //
    pStation->ScanContext.InternalScanBufferLength = ScanRequestLength;
    pStation->ScanContext.InternalScanRequestBuffer = pInternalScanRequestBuffer;

    return NDIS_STATUS_SUCCESS;
}


VOID
StaCheckPMKIDCandidate(
                      __in  PSTATION            pStation
                      )
{
    NDIS_STATUS         ndisStatus;
    ULONGLONG           time;
    PSTA_BSS_ENTRY      APEntry;
    UCHAR               SecIELength;
    PUCHAR              SecIEData;
    RSN_IE_INFO         RSNIEInfo;
    ULONG               Count, Size;
    ULONG               index1, index2;
    BOOLEAN             NeedIndicateAgain;
    UCHAR               Buffer[sizeof(DOT11_PMKID_CANDIDATE_LIST_PARAMETERS) + 
                               STA_PMKID_MAX_COUNT * sizeof(DOT11_BSSID_CANDIDATE)];

    PDOT11_PMKID_CANDIDATE_LIST_PARAMETERS  PMKIDParam;
    PDOT11_BSSID_CANDIDATE                  PMKIDCandidate;

    __try 
    {
        NdisAcquireSpinLock(&(pStation->ConnectContext.Lock));        

        //
        // We must be connected and our auth algorithm must be RSNA.   
        //
        if (pStation->Config.AuthAlgorithm != DOT11_AUTH_ALGO_RSNA ||
            pStation->ConnectContext.ConnectState != CONN_STATE_READY_TO_ROAM ||
            pStation->ConnectContext.AssociateState != ASSOC_STATE_ASSOCIATED) {
            __leave;
        }

        //
        // If we just recently checked PMKID candidate, simply return.
        //
        NdisGetCurrentSystemTime((PLARGE_INTEGER)&time);
        if ((time - pStation->PMKIDCache.CheckingTime) / 10000000 < STA_PMKID_CHECK_INTERVAL) {
            __leave;
        }

        pStation->PMKIDCache.CheckingTime = time;

        //
        // Get all the AP we can connect to.
        //
        ndisStatus = StaGetCandidateAPList(pStation, TRUE);

        //
        // If we don't get any AP, quit.
        //
        if (ndisStatus != NDIS_STATUS_SUCCESS || pStation->ConnectContext.CandidateAPCount == 0) {
            __leave;
        }

        //
        // Check if we really need to indicate the PMKID candidates. We don't have to if any
        // candidates are already in our PMKID cache.
        //
        Count = pStation->ConnectContext.CandidateAPCount;
        if (Count > STA_PMKID_MAX_COUNT)
            Count = STA_PMKID_MAX_COUNT;

        if (Count == pStation->PMKIDCache.Count) {
            NeedIndicateAgain = FALSE;
            for (index1 = 0; index1 < Count; index1++) {
                APEntry = pStation->ConnectContext.CandidateAPList[index1];
                PMKIDCandidate = pStation->PMKIDCache.Candidate;
                for (index2 = 0; index2 < pStation->PMKIDCache.Count; index2++, PMKIDCandidate++) {
                    if (NdisEqualMemory(APEntry->Dot11BSSID, PMKIDCandidate->BSSID, sizeof(DOT11_MAC_ADDRESS)) == 1) {
                        break;
                    }
                }

                if (index2 == pStation->PMKIDCache.Count) {
                    NeedIndicateAgain = TRUE;
                    break;
                }
            }
        }
        else {
            NeedIndicateAgain = TRUE;
        }

        if (!NeedIndicateAgain) {
            //
            // Remove the extra refs that the get candidate list adds
            //
            for (index1 = 0; index1 < pStation->ConnectContext.CandidateAPCount; index1++)
            {
                STA_DECREMENT_REF(pStation->ConnectContext.CandidateAPList[index1]->RefCount);
            }
            __leave;
        }

        //
        // We will have to indicate the PMKID candidate again. Build the candidate list.
        //
        PMKIDCandidate = pStation->PMKIDCache.Candidate;
        for (index1 = 0; index1 < Count; index1++) {
            APEntry = pStation->ConnectContext.CandidateAPList[index1];

            NdisDprAcquireSpinLock(&(APEntry->Lock)); // Lock AP entry

            //
            // Get the RSN Capability. 
            //
            ndisStatus = Dot11GetInfoEle(APEntry->pDot11InfoElemBlob,
                                         APEntry->InfoElemBlobSize,
                                         DOT11_INFO_ELEMENT_ID_RSN,
                                         &SecIELength,
                                         (PVOID)&SecIEData);
            if (ndisStatus != NDIS_STATUS_SUCCESS)
            {
                NdisDprReleaseSpinLock(&(APEntry->Lock));
                continue;
            }

            ndisStatus = StaParseRNSIE(SecIEData, RSNA_OUI_PREFIX, SecIELength, &RSNIEInfo);
            if (ndisStatus != NDIS_STATUS_SUCCESS)
            {
                NdisDprReleaseSpinLock(&(APEntry->Lock));            
                continue;
            }

            //
            // Copy the PMKID candidate.
            //

            NdisMoveMemory(PMKIDCandidate->BSSID, APEntry->Dot11BSSID, sizeof(DOT11_MAC_ADDRESS));
            NdisDprReleaseSpinLock(&(APEntry->Lock));
            PMKIDCandidate->uFlags = (RSNIEInfo.Capability & RSNA_CAPABILITY_PRE_AUTH) ? 
                                     DOT11_PMKID_CANDIDATE_PREAUTH_ENABLED : 0;
            PMKIDCandidate++;
        }

        pStation->PMKIDCache.Count = (ULONG)(PMKIDCandidate - pStation->PMKIDCache.Candidate);

        //
        // Remove the extra refs that the get candidate list adds
        //
        for (index1 = 0; index1 < pStation->ConnectContext.CandidateAPCount; index1++)
        {
            STA_DECREMENT_REF(pStation->ConnectContext.CandidateAPList[index1]->RefCount);
        }

        //
        // Build DOT11_PMKID_CANDIDATE_LIST_PARAMETERS structure for indication.
        //
        PMKIDParam = (PDOT11_PMKID_CANDIDATE_LIST_PARAMETERS)Buffer;
        MP_ASSIGN_NDIS_OBJECT_HEADER(PMKIDParam->Header, 
                                     NDIS_OBJECT_TYPE_DEFAULT,
                                     DOT11_PMKID_CANDIDATE_LIST_PARAMETERS_REVISION_1,
                                     sizeof(DOT11_PMKID_CANDIDATE_LIST_PARAMETERS));
        PMKIDParam->uCandidateListSize = pStation->PMKIDCache.Count;
        PMKIDParam->uCandidateListOffset = sizeof(DOT11_PMKID_CANDIDATE_LIST_PARAMETERS);

        Size = pStation->PMKIDCache.Count * sizeof(DOT11_BSSID_CANDIDATE);
        NdisMoveMemory(Add2Ptr(PMKIDParam, sizeof(DOT11_PMKID_CANDIDATE_LIST_PARAMETERS)),
                       pStation->PMKIDCache.Candidate,
                       Size);

        StaIndicateDot11Status(pStation, 
                               NDIS_STATUS_DOT11_PMKID_CANDIDATE_LIST,
                               NULL,
                               Buffer,
                               sizeof(DOT11_PMKID_CANDIDATE_LIST_PARAMETERS) + Size);
    }
    __finally
    {
        NdisReleaseSpinLock(&(pStation->ConnectContext.Lock));
    }
}

NDIS_STATUS
Sta11StartScan(
              __in  PSTATION            pStation,
              __in  PVOID               ScanRequestID,
              __in  PDOT11_SCAN_REQUEST_V2 pDot11ScanRequest,
              __out ULONG               ScanRequestBufferLength
              )
{
    NDIS_STATUS             ndisStatus;
    PDOT11_SCAN_REQUEST_V2  pExternalScanRequest = NULL;    // The scan request structure we would modify

    if (STA_TEST_SCAN_FLAG(pStation, STA_EXTERNAL_SCAN_IN_PROGRESS)) {
        MpTrace(COMP_SCAN, DBG_SERIOUS, ("External scan already in progress. Ignoring new request\n"));
        return NDIS_STATUS_DOT11_MEDIA_IN_USE;
    }

    do {
        //
        // Using the connect context lock to synchronize between 
        // scanning and connecting
        //
        NdisAcquireSpinLock(&(pStation->ConnectContext.Lock));

        //
        // This stops any new internal scans from getting submitted to the
        // hardware portion
        //
        STA_SET_SCAN_FLAG(pStation, STA_EXTERNAL_SCAN_IN_PROGRESS);

        //
        // Check state of current internal scans
        //
        if (STA_TEST_SCAN_FLAG(pStation, STA_PERIODIC_SCAN_IN_PROGRESS) == TRUE) {
            NdisReleaseSpinLock(&(pStation->ConnectContext.Lock));

            //
            // The internal scan has been submitted to hardware. Check if the OS 
            // is asking for a forced scan. If it is, abort the internal scan
            // and do the OS initiated scan, else we ignore the OS scan request
            //
            if (pDot11ScanRequest->dot11ScanType != dot11_scan_type_forced) {
                //
                // Not a forced scan, we skip the OS scan since we are scanning 
                // anyways
                //
                ndisStatus = NDIS_STATUS_DOT11_MEDIA_IN_USE;
                MpTrace(COMP_SCAN, DBG_LOUD, ("Not a forced scan. Skipping OS scan due to scanning\n"));
                break;
            }

            //
            // Internal scan has been submitted to hardware. Cancel it (and wait 
            // for it to cancel). Note that we do not stop the DPC from running
            // again. It continues running, but it wont do a scan.
            //
            StaWaitForPeriodicScan(pStation);

            NdisAcquireSpinLock(&(pStation->ConnectContext.Lock));        
        }

        //
        // If we are performing an association, we skip the scan 
        // if its not forced. This would happen if the perdiodic scan 
        // decided to initiated roaming/connection and the OS sent down
        // a scan request
        //
        if ((pStation->ConnectContext.ConnectState >= CONN_STATE_READY_TO_CONNECT) &&
            ((pStation->ConnectContext.AssociateState != ASSOC_STATE_NOT_ASSOCIATED) &&
             (pStation->ConnectContext.AssociateState != ASSOC_STATE_ASSOCIATED))
           ) {
            if (pDot11ScanRequest->dot11ScanType != dot11_scan_type_forced) {
                //
                // Not a forced scan, we skip the OS scan since we are associating
                //
                NdisReleaseSpinLock(&(pStation->ConnectContext.Lock));
                ndisStatus = NDIS_STATUS_DOT11_MEDIA_IN_USE;
                MpTrace(COMP_SCAN, DBG_LOUD, ("Not a forced scan. Skipping OS scan due to association\n"));
                break;
            }
        }

        MpTrace(COMP_SCAN, DBG_LOUD, ("Processing OS requested scan\n"));
        //
        // Next scan completion is for external scan
        //

        STA_SET_SCAN_FLAG(pStation, STA_COMPLETE_EXTERNAL_SCAN);
        NdisReleaseSpinLock(&(pStation->ConnectContext.Lock));

        // Allocate a new scan request structure that we modify and cache internally
        MP_ALLOCATE_MEMORY(
                          pStation->MiniportAdapterHandle,
                          &pExternalScanRequest,
                          ScanRequestBufferLength,
                          STA11_MEMORY_TAG
                          );

        if (pExternalScanRequest == NULL) {
            ndisStatus = NDIS_STATUS_RESOURCES;
            break;
        }

        //
        // Save some state
        //
        pStation->ScanContext.ExternalScanRequestBuffer = pExternalScanRequest;
        pStation->ScanContext.ScanRequestID = ScanRequestID;

        NdisMoveMemory(
                      pExternalScanRequest,
                      pDot11ScanRequest,
                      ScanRequestBufferLength
                      );

        //
        // If desired we would modify the scan request buffer here
        //


        //
        // Save scan start time
        //
        NdisGetCurrentSystemTime((PLARGE_INTEGER)&pStation->ScanContext.LastScanTime);

        //
        // While we are scanning, we want all the sends to be blocked. The MP portion already
        // has a send lock. Rather than use a new lock, lets have the MP portion use that lock
        // to synchronize between the scan and the send
        //
        ndisStatus = Mp11StartScan(
                                  pStation->pAdapter,
                                  pExternalScanRequest,
                                  ScanRequestBufferLength
                                  );

    }while (FALSE);

    if (ndisStatus != NDIS_STATUS_SUCCESS) {
        NdisAcquireSpinLock(&(pStation->ConnectContext.Lock));
        STA_CLEAR_SCAN_FLAG(pStation, (STA_EXTERNAL_SCAN_IN_PROGRESS | STA_COMPLETE_EXTERNAL_SCAN));
        NdisReleaseSpinLock(&(pStation->ConnectContext.Lock));

        if (pExternalScanRequest != NULL)
            MP_FREE_MEMORY(pExternalScanRequest);

        pStation->ScanContext.ExternalScanRequestBuffer = NULL;
    }

    return ndisStatus;
}

VOID
Sta11ScanComplete(
                 __in  PSTATION        pStation,
                 __in  NDIS_STATUS     ndisStatus
                 )
{
    NdisAcquireSpinLock(&(pStation->ConnectContext.Lock));

    if (STA_TEST_SCAN_FLAG(pStation, STA_COMPLETE_PERIODIC_SCAN)) {
        //
        // This is the completion of a periodic scan. 
        //
        STA_CLEAR_SCAN_FLAG(pStation, STA_COMPLETE_PERIODIC_SCAN);

        //
        // Run our "roaming" code to determine if we need to roam/connect
        //
        StaPeriodicScanComplete(pStation, ndisStatus);

        if (!STA_TEST_SCAN_FLAG(pStation, STA_STOP_PERIODIC_SCAN)) {
            //
            // Permitted to requeue
            //
            STA_SET_SCAN_FLAG(pStation, STA_PERIODIC_SCAN_QUEUED);
            WdfTimerStart(pStation->ScanContext.Timer_PeriodicScan, 
                           WDF_REL_TIMEOUT_IN_MS(STA_DEFAULT_SCAN_TIMER_INTERVAL));
            /*NdisMSetTimer(
                &(pStation->ScanContext.Timer_PeriodicScan),
                STA_DEFAULT_SCAN_TIMER_INTERVAL
                );
                */
        }

        //
        // The clear is intentionally after the complete. In the complete, we
        // may roam. The Connect/Scan OID routines would wait for this flag
        // to clear, thus ensuring that nobody else would attempt to change
        // channels as the same time as they
        //
        STA_CLEAR_SCAN_FLAG(pStation, STA_PERIODIC_SCAN_IN_PROGRESS);        
        NdisReleaseSpinLock(&(pStation->ConnectContext.Lock));        
    }
    else {
        STA_CLEAR_SCAN_FLAG(pStation, STA_COMPLETE_EXTERNAL_SCAN);
        NdisReleaseSpinLock(&(pStation->ConnectContext.Lock));

        //
        // This is the completion of a scan request from the OS
        //
        MPVERIFY(pStation->ScanContext.ExternalScanRequestBuffer != NULL);

        MP_FREE_MEMORY(pStation->ScanContext.ExternalScanRequestBuffer);
        pStation->ScanContext.ExternalScanRequestBuffer = NULL;

        StaIndicateDot11Status(pStation, 
                               NDIS_STATUS_DOT11_SCAN_CONFIRM,
                               pStation->ScanContext.ScanRequestID,
                               &ndisStatus,
                               sizeof(NDIS_STATUS)
                              );
        pStation->ScanContext.ScanRequestID = NULL;

        NdisAcquireSpinLock(&(pStation->ConnectContext.Lock));        
        STA_CLEAR_SCAN_FLAG(pStation, STA_EXTERNAL_SCAN_IN_PROGRESS);
        NdisReleaseSpinLock(&(pStation->ConnectContext.Lock));
    }

    //
    // Just completed a scan. Perfect time to check new PMKID
    // candidates. This applies to RNSA (WPA2) only.
    //
    if (pStation->Config.AuthAlgorithm == DOT11_AUTH_ALGO_RSNA)
    {
        StaCheckPMKIDCandidate(pStation);
    }

    if (pStation->ConnectContext.AssociateState == ASSOC_STATE_ASSOCIATED)
    {
        pStation->Config.CheckForProtectionInERP = TRUE;
    }
}

// LOCK HELD
BOOLEAN
StaInternalScanPermitted(
                        __in  PSTATION            pStation
                        )
{
    // Dont queue if OS scan is asking for a scan or we are not supposed to scan
    if (STA_TEST_SCAN_FLAG(pStation, STA_EXTERNAL_SCAN_IN_PROGRESS | STA_STOP_PERIODIC_SCAN)) {
        return FALSE;
    }

    //
    // Do not queue scan if we are in reset. ResetEnd will
    // queue it
    //
    if (MP_TEST_FLAG(pStation, STA_RESET_IN_PROGRESS)) {
        return FALSE;
    }

    //
    // If media streaming, only perform the scan if we do not have an active
    // connection
    //    
    if (pStation->Config.MediaStreamingEnabled == TRUE) {
        //
        // Media streaming is on. Can only scan if we are not connected
        //
        if (pStation->ConnectContext.ConnectState >= CONN_STATE_READY_TO_CONNECT) {
            if (pStation->ConnectContext.AssociateState == ASSOC_STATE_NOT_ASSOCIATED) {
                // Expected to be connected, but am not
                return TRUE;
            }
        }
        else {
            // Not connected
            return TRUE;
        }
        
        if (pStation->ScanContext.MustRoam)
        {
            // Lost connection
            return TRUE;
        }

        return FALSE;
    }

    //
    // If we are in the middle of attempting to associate, dont scan
    //
    if ((pStation->ConnectContext.ConnectState >= CONN_STATE_READY_TO_CONNECT) &&
        ((pStation->ConnectContext.AssociateState != ASSOC_STATE_NOT_ASSOCIATED) &&
         (pStation->ConnectContext.AssociateState != ASSOC_STATE_ASSOCIATED))
       ) {
        return FALSE;
    }

    //
    // Additional conditions we can check for are whether or not the
    // medium is busy or not for us to perform a scan
    //

    return TRUE;
}


// LOCK HELD
BOOLEAN
StaInternalScanRequired(
                       __in  PSTATION            pStation
                       )
{
    ULONGLONG               ullCurrentSystemTime;
    ULONGLONG               ullOldestBeaconTime;
    ULONGLONG               ullAcceptableScanTime;
    PSTA_BSS_ENTRY          pAPEntry;
    BOOLEAN                 bScanRequired = FALSE;
    // Depending on whether we are scanning for updates or scanning
    // for roaming/connecting, we choose appropriate interscan gap
    ULONGLONG               ScanGapSeconds = STA_PERIODIC_SCAN_NORMAL_GAP;
    PCHAR                   pScanReason = NULL;        // For debug purposes only

    NdisGetCurrentSystemTime((PLARGE_INTEGER)&ullCurrentSystemTime);

    do {
        //
        // We scan if:
        //  1. we are not connected and should be connected
        //  2. if we are connected, but need to roam 
        //  3. to periodically collect environment information
        //
        pStation->ScanContext.ScanForRoam = FALSE;
        pStation->ScanContext.MustRoam = FALSE;

        if (pStation->ConnectContext.ConnectState >= CONN_STATE_READY_TO_CONNECT) {
            if (pStation->ConnectContext.AssociateState == ASSOC_STATE_NOT_ASSOCIATED) {
                //
                // Not connected when we should be connected. We will scan and attempt
                // to connect
                //
                bScanRequired = TRUE;
                ScanGapSeconds = pStation->ScanContext.RoamingScanGap;
                pStation->ScanContext.ScanForRoam = TRUE;
                pScanReason = "Scan/Roam required because we are not associated but should be\n";
                break;
            }
            else if ((pStation->ConnectContext.ConnectState == CONN_STATE_READY_TO_ROAM) &&
                     (pStation->ConnectContext.AssociateState == ASSOC_STATE_ASSOCIATED)
                    ) {
                //
                // We are associated. The AP is good
                //

                //
                // Check the time that we last received a beacon from the access point
                // we are associated with. If its been long, we roam
                //
                pAPEntry = pStation->ConnectContext.ActiveAP;
                MPVERIFY(pAPEntry);

                NdisDprAcquireSpinLock(&(pAPEntry->Lock));      // Called with lock held = DISPATCH
                ullOldestBeaconTime = ullCurrentSystemTime;
                if (pAPEntry->NoBeaconRoamTime <= ullOldestBeaconTime)
                    ullOldestBeaconTime -= pAPEntry->NoBeaconRoamTime;

                if (pAPEntry->HostTimestamp < ullOldestBeaconTime) {
                    //
                    // Determine if we need to drop connectivity
                    //
                    ullOldestBeaconTime = ullCurrentSystemTime - STA_MAX_INFRA_UNREACHABLE_TIMEOUT;
                    if (pAPEntry->HostTimestamp < ullOldestBeaconTime) {
                        //
                        // Havent received a beacon for a long while. If we do not find
                        // a better AP, we must assume we are disconnected
                        //
                        pStation->ScanContext.MustRoam = TRUE;
                        //                        
                        // Reset its timestamp to be zero, so that we do not
                        // report this AP to the OS if we dont receive a new beacon from it
                        //
                        pAPEntry->HostTimestamp = 0;                        


                    }
                    NdisDprReleaseSpinLock(&(pAPEntry->Lock));

                    //
                    // Havent received a beacon from this AP for a while
                    // lets try to roam
                    //
                    bScanRequired = TRUE;
                    ScanGapSeconds = pStation->ScanContext.RoamingScanGap;
                    pStation->ScanContext.ScanForRoam = TRUE;
                    pScanReason = "Scan/Roam required because we havent received a beacon for a while\n";
                    break;
                }

                //
                // Check the number of beacons that we have received with 
                // RSSI/LinkQuality value below threshold
                //
                if (pAPEntry->LowQualityCount > pStation->RegInfo.RSSIRoamBeaconCount)
                {
                    NdisDprReleaseSpinLock(&(pAPEntry->Lock));

                    //
                    // Have received a sequence of low strength beacons
                    // from this AP. Lets check if we can find a better AP
                    // and if yes roam
                    //
                    bScanRequired = TRUE;
                    ScanGapSeconds = pStation->ScanContext.RoamingScanGap;
                    pStation->ScanContext.ScanForRoam = TRUE;
                    pScanReason = "Scan/Roam required because we have low signal strength\n";
                    break;
                }

                //
                // Need to scan and roam due send failures reported by hardware
                //
                if (pStation->ConnectContext.RoamForSendFailures == TRUE)
                {
                    NdisDprReleaseSpinLock(&(pAPEntry->Lock));

                    // reset RoamForSendFailures so next time it will not try to scan/roam again
                    pStation->ConnectContext.RoamForSendFailures = FALSE;
                    
                    bScanRequired = TRUE;
                    ScanGapSeconds = pStation->ScanContext.RoamingScanGap;
                    pStation->ScanContext.ScanForRoam = TRUE;
                    pScanReason = "Scan/Roam required because there were too many retries for send\n";
                    break;
                }

                //
                // TODO: Check the SSID. If the AP was stopped/restarted
                // the SSID may have changed and we may still see the beacon
                // but the AP has lost all our state. It should send us a 
                // disassociate, but it may not
                //
                NdisDprReleaseSpinLock(&(pAPEntry->Lock));
            }
        }
        else if (pStation->AdHocStaInfo.JoinAdHocAfterPowerResume == TRUE)
        {
            bScanRequired = TRUE;

            // we get into this state when system wakes up from hibernation.
            // have to set ScanGapSeconds to 0 to force the scan so the 
            // device will connect to adhoc network when scan complete.
            ScanGapSeconds = 0;
            break;
        }

        //
        // Check if we are due for doing a periodic scan
        //
        if (pStation->ScanContext.PeriodicScanCounter++ >= STA_DEFAULT_SCAN_TICK_COUNT) {
            //
            // Not scanned for specified duration. Lets scan
            //
            bScanRequired = TRUE;
            break;
        }

    } while (FALSE);
    
    if (bScanRequired) {
        //
        // To avoid too many scans, lets check if we have scanned recently.
        // If yes, we delay our scan for a while. If we are not connected
        // and should be use short scan delay, else use larger scan delay
        //
        ullAcceptableScanTime = pStation->ScanContext.LastScanTime +             
                                ScanGapSeconds * 10000000; // Convert seconds to 100nS

        if (ullAcceptableScanTime > ullCurrentSystemTime) {
            //
            // Scanned recently. Dont scan again
            //
            bScanRequired = FALSE;
        }
        else {
            //
            // We are going to scan. Increase the scan gap we use during roaming
            //
            pStation->ScanContext.RoamingScanGap = 2 * pStation->ScanContext.RoamingScanGap;
            if (pStation->ScanContext.RoamingScanGap > STA_PERIODIC_SCAN_NORMAL_GAP) {
                pStation->ScanContext.RoamingScanGap = STA_PERIODIC_SCAN_SHORT_GAP;
            }

            //
            // Log if scanning due to roaming needs
            //
            if (pScanReason != NULL)
                MpTrace(COMP_ASSOC, DBG_NORMAL, ("%s\n", pScanReason));


        }
    }

    return bScanRequired;
}


VOID
StaStartPeriodicScan(
                    __in  PSTATION            pStation
                    )
{
    NdisAcquireSpinLock(&(pStation->ConnectContext.Lock));
    STA_CLEAR_SCAN_FLAG(pStation, STA_STOP_PERIODIC_SCAN);
    STA_SET_SCAN_FLAG(pStation, STA_PERIODIC_SCAN_QUEUED);

    //
    // Reset the scan time
    //
    NdisGetCurrentSystemTime((PLARGE_INTEGER)&pStation->ScanContext.LastScanTime);
    pStation->ScanContext.PeriodicScanCounter = 0;

    //
    // If we are not connected
    //
    if ((pStation->ConnectContext.ConnectState >= CONN_STATE_READY_TO_CONNECT) &&
        (pStation->ConnectContext.AssociateState == ASSOC_STATE_NOT_ASSOCIATED)
       ) {
        //
        // Scan as quickly as possible
        //
        WdfTimerStart(pStation->ScanContext.Timer_PeriodicScan, WDF_REL_TIMEOUT_IN_MS(STA_FORCED_PERIODIC_SCAN_INTERVAL));
             /*
        NdisMSetTimer(
            &(pStation->ScanContext.Timer_PeriodicScan),
            STA_FORCED_PERIODIC_SCAN_INTERVAL
            );
            */
    }
    else {
        //
        // Queue the periodic scan request
        //
        WdfTimerStart(pStation->ScanContext.Timer_PeriodicScan, WDF_REL_TIMEOUT_IN_MS(STA_DEFAULT_SCAN_TIMER_INTERVAL));
           /*
        NdisMSetTimer(
            &(pStation->ScanContext.Timer_PeriodicScan),
            STA_DEFAULT_SCAN_TIMER_INTERVAL
            );
            */
    }
    NdisReleaseSpinLock(&(pStation->ConnectContext.Lock));   
}

//
// Waits for the current periodic scan to complete. It cancels the scan
// and also waits for the routine to complete executing
//
VOID
StaWaitForPeriodicScan(
                      __in  PSTATION            pStation
                      )
{
    while (TRUE) {
        //
        // Cancel scanning. If periodic scan request
        // had not yet been submitted, we would return without 
        // the scan completed. Hence we use this loop
        //
        Hw11CancelScan(pStation->pNic);

        if (!STA_TEST_SCAN_FLAG(pStation, STA_PERIODIC_SCAN_IN_PROGRESS)) {
            //
            // Its done
            //
            break;
        }
    }
}


// LOCK HELD
VOID
StaForceInternalScan(
                    __in  PSTATION            pStation,
                    __in  BOOLEAN             bScanToRoam
                    )
{
    BOOLEAN                 scanTimerCancelled;

    UNREFERENCED_PARAMETER(bScanToRoam);

    //
    // If we are allowed to queue a periodic scan and its not already running
    //
    if (!STA_TEST_SCAN_FLAG(pStation, (STA_STOP_PERIODIC_SCAN | STA_PERIODIC_SCAN_IN_PROGRESS))) {
        //
        // The periodic scan runs at big intervals. To make it
        // run now, lets try to cancel it and if successful
        // queue it again with short interval. If we cannot cancel it,
        // its either already running or isnt supposed to run
        //        
        //
        // NOTE: the boolean is set to FALSE because it shouldn't wait since we are called at raised IRQL
        //
        scanTimerCancelled = WdfTimerStop(pStation->ScanContext.Timer_PeriodicScan, FALSE);
        if (scanTimerCancelled) {
            // Not touching the flags since I requeue
            WdfTimerStart(pStation->ScanContext.Timer_PeriodicScan, WDF_REL_TIMEOUT_IN_MS(STA_FORCED_PERIODIC_SCAN_INTERVAL));
        }
    }
}


// LOCK HELD
__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
StaPeriodicScanComplete(
                       __in  PSTATION            pStation,
                       __in  NDIS_STATUS         ndisStatus
                       )
{
    if (ndisStatus == NDIS_STATUS_REQUEST_ABORTED) 
    {
        //
        // Scan request had aborted
        //
        return;
    }

    if ((pStation->ConnectContext.ConnectState == CONN_STATE_READY_TO_CONNECT) &&
        (pStation->ConnectContext.AssociateState == ASSOC_STATE_NOT_ASSOCIATED)) 
    {
        //
        // This is a scan complete after a connect request. We havent yet
        // tried to connect. Initiate the connection process
        //
        NdisReleaseSpinLock(&(pStation->ConnectContext.Lock));
        StaConnectStart(pStation, FALSE);
        NdisAcquireSpinLock(&(pStation->ConnectContext.Lock));
        return;
    }
    else if (pStation->ConnectContext.ConnectState == CONN_STATE_READY_TO_ROAM) 
    {
            //
            // Check if we can roam. If yes, lets attempt to roam
            //
            StaRoamStart(pStation);
    }
    else if (pStation->AdHocStaInfo.JoinAdHocAfterPowerResume == TRUE)
    {
        Sta11ConnectAdHoc(pStation);
    }

}

//
// This is running periodically (when we are not reset or stopping)
// NOTE: Make sure this si stopped on a surprise remove
//
VOID
StaPeriodicScanCallback(
                       __in WDFTIMER WdfTimer
                       )
{
    PSTATION                pStation ;
    NDIS_STATUS             ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_SCAN_REQUEST_V2  pInternalScanRequest = NULL;
    PDOT11_SSID             pSSID;
    BOOLEAN                 bRequeueScan = FALSE;
    PSTATION_TIMER_SCAN_CONTEXT     stationTimerContext;

    stationTimerContext = GetStationTimerContext(WdfTimer);
    pStation = stationTimerContext->Station;

    NdisAcquireSpinLock(&(pStation->ConnectContext.Lock));
    STA_CLEAR_SCAN_FLAG(pStation, STA_PERIODIC_SCAN_QUEUED);
    STA_SET_SCAN_FLAG(pStation, STA_PERIODIC_SCAN_IN_PROGRESS);

    do {
        //
        // Check if we NEED to do a scan
        //
        if (StaInternalScanRequired(pStation) == FALSE) {
            STA_CLEAR_SCAN_FLAG(pStation, STA_PERIODIC_SCAN_IN_PROGRESS);
            NdisReleaseSpinLock(&(pStation->ConnectContext.Lock));
            bRequeueScan = TRUE;
            break;
        }

        //
        // Check if we can do a scan
        //
        if (StaInternalScanPermitted(pStation) == FALSE) {
            STA_CLEAR_SCAN_FLAG(pStation, STA_PERIODIC_SCAN_IN_PROGRESS);
            NdisReleaseSpinLock(&(pStation->ConnectContext.Lock));
            bRequeueScan = TRUE;
            break;
        }

        MpTrace(COMP_ASSOC, DBG_LOUD, ("Initiating periodic scan\n"));

        //
        // Reset scan tick count
        //
        pStation->ScanContext.PeriodicScanCounter = 0;

        //
        // Next scan complete would be for a periodic scan
        //
        STA_SET_SCAN_FLAG(pStation, STA_COMPLETE_PERIODIC_SCAN);

        //
        // Copy the SSID from the current config into the internal
        // scan request buffer
        //
        pInternalScanRequest = pStation->ScanContext.InternalScanRequestBuffer;
        NdisReleaseSpinLock(&(pStation->ConnectContext.Lock));
                if (pStation->ScanContext.SSIDInProbeRequest)
        {
            pSSID = (PDOT11_SSID) pInternalScanRequest->ucBuffer;
            NdisMoveMemory(pSSID, &(pStation->Config.SSID), sizeof(DOT11_SSID));
            pInternalScanRequest->uNumOfdot11SSIDs = 1;
        }
        else
        {
            //
            // Insert the broadcast SSID
            //
            NdisZeroMemory(pInternalScanRequest->ucBuffer, sizeof(DOT11_SSID));
            pInternalScanRequest->uNumOfdot11SSIDs = 1;
        }

        //
        // Save scan time
        //
        NdisGetCurrentSystemTime((PLARGE_INTEGER)&pStation->ScanContext.LastScanTime);

        //
        // Start the scan
        //
        ndisStatus = Mp11StartScan(
                                  pStation->pAdapter,
                                  pStation->ScanContext.InternalScanRequestBuffer,
                                  pStation->ScanContext.InternalScanBufferLength
                                  );

        if (ndisStatus != NDIS_STATUS_SUCCESS) {
            bRequeueScan = TRUE;
            // Flag is cleared below where we have the lock
        }
    } while (FALSE);

    //
    // We were not able to scan for some reason. Check if we
    // should reschedule ourselves for later scanning
    //
    if (bRequeueScan == TRUE) {
        NdisAcquireSpinLock(&(pStation->ConnectContext.Lock));
        if (!STA_TEST_SCAN_FLAG(pStation, STA_STOP_PERIODIC_SCAN)) {
            //
            // Permitted to requeue
            //
            STA_SET_SCAN_FLAG(pStation, STA_PERIODIC_SCAN_QUEUED);
            STA_CLEAR_SCAN_FLAG(pStation, (STA_PERIODIC_SCAN_IN_PROGRESS | STA_COMPLETE_PERIODIC_SCAN));
            WdfTimerStart(pStation->ScanContext.Timer_PeriodicScan, WDF_REL_TIMEOUT_IN_MS(STA_DEFAULT_SCAN_TIMER_INTERVAL));
            /*NdisMSetTimer(
                &(pStation->ScanContext.Timer_PeriodicScan),
                STA_DEFAULT_SCAN_TIMER_INTERVAL
                );
                */
        }
        else {
            STA_CLEAR_SCAN_FLAG(pStation, (STA_PERIODIC_SCAN_IN_PROGRESS | STA_COMPLETE_PERIODIC_SCAN));
        }
        NdisReleaseSpinLock(&(pStation->ConnectContext.Lock));
    }
}

