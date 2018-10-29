/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Sta_aplist.c

Abstract:
    STA layer BSS list functionality
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
#include "precomp.h"
#include "st_aplst.h"
#include "st_adhoc.h"

#if DOT11_TRACE_ENABLED
#include "Sta_APList.tmh"
#endif

INLINE NDIS_STATUS StaValidateBeacon(
    __in  PDOT11_BEACON_FRAME     pDot11BeaconFrame
    )
{
    UNREFERENCED_PARAMETER(pDot11BeaconFrame);
    return NDIS_STATUS_SUCCESS;
}

VOID
StaInitializeBSSList(
    __in  PSTA_BSS_LIST           pBSSList
    )
{
    NdisInitializeReadWriteLock(&(pBSSList->ListLock));

    pBSSList->MaxNumOfBSSEntries = STA_BSS_ENTRY_MAX_ENTRIES_DEFAULT;
    pBSSList->NumOfBSSEntries = 0;
    NdisInitializeListHead(&pBSSList->List);
}

// Write lock acquired
INLINE VOID
StaAddBSSEntry(
    __in  PSTA_BSS_LIST           pBSSList,
    __in  PSTA_BSS_ENTRY          pBSSEntry
    )
{
    InsertTailList(&(pBSSList->List), &(pBSSEntry->Link));
    pBSSList->NumOfBSSEntries++;
}

// Write lock acquired
INLINE VOID
StaRemoveBSSEntry(
    __in  PSTA_BSS_LIST           pBSSList,
    __in  PSTA_BSS_ENTRY          pBSSEntry
    )
{
    RemoveEntryList(&(pBSSEntry->Link));
    pBSSList->NumOfBSSEntries--;
}

// Read Lock must be held
PSTA_BSS_ENTRY
StaFindBSSEntry(
    __in  PSTA_BSS_LIST           pBSSList,
    __in  const unsigned char *   srcAddress
    )
{
    PLIST_ENTRY         pHead = NULL, pEntry = NULL;
    PSTA_BSS_ENTRY      pBSSEntry = NULL;
    BOOLEAN             bFound = FALSE;

    pHead = &(pBSSList->List);
    pEntry = pHead->Flink;
    while(pEntry != pHead) 
    {
        pBSSEntry = CONTAINING_RECORD(pEntry, STA_BSS_ENTRY, Link);
        pEntry = pEntry->Flink;

        NdisAcquireSpinLock(&(pBSSEntry->Lock));
        if (MP_COMPARE_MAC_ADDRESS(pBSSEntry->MacAddress, srcAddress)) 
        {
            NdisReleaseSpinLock(&(pBSSEntry->Lock));
            bFound = TRUE;
            break;
        }
        NdisReleaseSpinLock(&(pBSSEntry->Lock));
    }

    return ((bFound)? pBSSEntry: NULL);

}

// Write lock on the list must be held
// Returns an entry that can be expired. The caller frees it
PSTA_BSS_ENTRY
StaExpireBSSEntry(
    __in  PSTA_BSS_LIST           pBSSList,
    __in  ULONGLONG               ullMaxActiveTime,
    __in  ULONGLONG               ullExpireTimeStamp
    )
{
    PLIST_ENTRY         pHead = NULL, pEntry = NULL;
    PSTA_BSS_ENTRY      pBSSEntry = NULL;
    BOOLEAN             bFound = FALSE;

    //
    // We can expire an entry that has been around for longer 
    // than this time
    //
    if (ullMaxActiveTime <= ullExpireTimeStamp)
        ullExpireTimeStamp -= ullMaxActiveTime;

    pHead = &(pBSSList->List);
    pEntry = pHead->Flink;
    while(pEntry != pHead) 
    {
        pBSSEntry = CONTAINING_RECORD(pEntry, STA_BSS_ENTRY, Link);
        pEntry = pEntry->Flink;

        NdisAcquireSpinLock(&(pBSSEntry->Lock));

        //
        // If the entry is older than we expected and its not in
        // use, we can expire it
        //
        if (pBSSEntry->HostTimestamp < ullExpireTimeStamp)
        {
            if (NdisInterlockedDecrement(&(pBSSEntry->RefCount)) == 0)
            {
                MpTrace(COMP_SCAN, DBG_LOUD, ("Expiring AP: %02X-%02X-%02X-%02X-%02X-%02X\n", 
                    pBSSEntry->Dot11BSSID[0], pBSSEntry->Dot11BSSID[1], pBSSEntry->Dot11BSSID[2], 
                    pBSSEntry->Dot11BSSID[3], pBSSEntry->Dot11BSSID[4], pBSSEntry->Dot11BSSID[5]));

                MPVERIFY(pBSSEntry->pAssocRequest == NULL);
                MPVERIFY(pBSSEntry->pAssocResponse == NULL);
                
                //
                // This is the entry we can expire. Remove it from the list.
                //
                NdisReleaseSpinLock(&(pBSSEntry->Lock));
                StaRemoveBSSEntry(pBSSList, pBSSEntry);
                
                bFound = TRUE;
                break;
            }
            else
            {
                // Someone is using the entry, we cannot remove/delete this. Add back
                // a ref and we will delete later on

                // This is subobtimal. Ideally the last person to decrement
                // refcount should delete the entry and not us. Modify to remove
                // the entry from the list, decrement its refcount and only free
                // the memory if the refcount has gone to zero
                NdisInterlockedIncrement(&(pBSSEntry->RefCount));
            }
        }
        NdisReleaseSpinLock(&(pBSSEntry->Lock));
    }

    if (bFound != TRUE)
    {
        pBSSEntry = NULL;
    }
    
    return pBSSEntry;
}


NDIS_STATUS 
StaInsertBSSEntry(
    __in  PSTATION        pStation,
    __in  PNIC_RX_FRAGMENT    pNicFragment,
    __in  PDOT11_BEACON_FRAME pDot11BeaconFrame,
    __in  ULONG           BeaconDataLength
    )
{
    NDIS_STATUS         ndisStatus = NDIS_STATUS_SUCCESS;
    ULONGLONG           ullHostTimeStamp;
    LOCK_STATE          LockState;
    PSTA_BSS_ENTRY      pBSSEntry = NULL;
    PDOT11_MGMT_HEADER  pMgmtPktHeader;
    BOOLEAN             bNewAp = FALSE;
    PSTA_BSS_LIST       pDiscoveredBSSList = &(pStation->BSSList);

    pMgmtPktHeader = Hw11GetFragmentDataStart(pNicFragment);
    NdisGetCurrentSystemTime((PLARGE_INTEGER)&ullHostTimeStamp);

    //
    // We acquire the write lock as we are adding entries to the list
    //
    MP_ACQUIRE_WRITE_LOCK(&(pStation->BSSList.ListLock), &LockState);

    do
    {
        //
        // Check again if this entry already exists in the list. This is to handle
        // if the AP was added since we first checked (possible if the
        // flush routine was running)
        //
        pBSSEntry = StaFindBSSEntry(
            pDiscoveredBSSList,
            pMgmtPktHeader->SA
            );

        if (pBSSEntry == NULL)
        {
            bNewAp = TRUE;      // New AP
            //
            // We havent found this AP yet, we would add it to the list
            //
            if (pDiscoveredBSSList->NumOfBSSEntries >= pDiscoveredBSSList->MaxNumOfBSSEntries)
            {
                //
                // We need to replace an entry thats in the list
                //
                pBSSEntry = StaExpireBSSEntry(
                    pDiscoveredBSSList,
                    pStation->RegInfo.BSSEntryExpireTime,
                    ullHostTimeStamp
                    );

                if (pBSSEntry != NULL)
                {
                    //
                    // Add initial in-use refcount
                    //
                    pBSSEntry->RefCount = 1;
                }
                
                //
                // Dont zero out the AP entry so that we can
                // reuse the info element blob
                //
            }
            else
            {
                //
                // Create a new entry for this AP
                //
                MP_ALLOCATE_MEMORY(pStation->MiniportAdapterHandle, 
                    &pBSSEntry, 
                    sizeof(STA_BSS_ENTRY), 
                    STA11_MEMORY_TAG
                    );
                if (pBSSEntry != NULL)
                {
                    //
                    // Initialize the new entry
                    //
                    NdisZeroMemory(pBSSEntry, sizeof(STA_BSS_ENTRY));
                    
                    pBSSEntry->MaxBeaconFrameSize = 0;
                    pBSSEntry->RefCount = 1; // Add initial in-use refcount
                    pBSSEntry->pDot11BeaconFrame = NULL;
                    NdisAllocateSpinLock(&(pBSSEntry->Lock));
                }
            }

            if (pBSSEntry == NULL)
            {
                ndisStatus = NDIS_STATUS_RESOURCES;
                break;
            }

            //
            // This Entry is not yet in the list
            //
            
            // We will be updating the beacon frame
            pBSSEntry->BeaconFrameSize = 0;
            pBSSEntry->AssocCost = 0;
            pBSSEntry->ProbeSSID.uSSIDLength = 0;

            NdisMoveMemory(
                pBSSEntry->Dot11BSSID,
                pMgmtPktHeader->BSSID,
                sizeof(DOT11_MAC_ADDRESS)
                );

            NdisMoveMemory(
                pBSSEntry->MacAddress,
                pMgmtPktHeader->SA,
                sizeof(DOT11_MAC_ADDRESS)
                );
        }

        // Update the information in this BSS entry (either new or reused entry)
        ndisStatus = StaUpdateBSSEntry(
            pStation, 
            pBSSEntry, 
            pNicFragment, 
            pDot11BeaconFrame, 
            BeaconDataLength
            );
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            break;
        }

        //
        // Add the new BSS to our list
        //
        if (bNewAp)
        {
            MpTrace(COMP_SCAN, DBG_LOUD, ("AP %02X-%02X-%02X-%02X-%02X-%02X at channel: %d (%d)\n", 
                    pBSSEntry->Dot11BSSID[0], pBSSEntry->Dot11BSSID[1], pBSSEntry->Dot11BSSID[2], 
                    pBSSEntry->Dot11BSSID[3], pBSSEntry->Dot11BSSID[4], pBSSEntry->Dot11BSSID[5],
                    pBSSEntry->Channel, Hw11GetFragmentChannel(pNicFragment)));

            StaAddBSSEntry(pDiscoveredBSSList, pBSSEntry);
        }

        //
        // Note: If any code is added below here, remember to remove entry
        // from the list
        //
    } while (FALSE);

    MP_RELEASE_WRITE_LOCK(&(pStation->BSSList.ListLock), &LockState);

    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        // Free the new entry we may have created
        if ((bNewAp) && (pBSSEntry != NULL))
        {
            if (pBSSEntry->pDot11BeaconFrame != NULL)
            {
                MP_FREE_MEMORY(pBSSEntry->pDot11BeaconFrame);
                pBSSEntry->pDot11BeaconFrame = NULL;
                pBSSEntry->pDot11InfoElemBlob = NULL;
                pBSSEntry->InfoElemBlobSize = 0;
                pBSSEntry->BeaconFrameSize = 0;
                pBSSEntry->MaxBeaconFrameSize= 0;
            }
            
            MP_FREE_MEMORY(pBSSEntry);
        }
    }

    return ndisStatus;
}


// BSS list lock acquired & called at Dispatch
NDIS_STATUS 
StaUpdateBSSEntry(
    __in  PSTATION        pStation,
    __in  PSTA_BSS_ENTRY  pBSSEntry,
    __in  PNIC_RX_FRAGMENT    pNicFragment,
    __in  PDOT11_BEACON_FRAME pDot11BeaconFrame,
    __in  ULONG           BeaconDataLength
    )
{
    NDIS_STATUS         ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_MGMT_HEADER  pMgmtPktHeader;
    ULONGLONG           ullHostTimeStamp;
    PVOID               pSavedBeaconBuffer = NULL;
    ULONG               uOffsetOfInfoElemBlob = FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements);
    UCHAR               channel;
    DOT11_PHY_TYPE      PhyType;

    pMgmtPktHeader = Hw11GetFragmentDataStart(pNicFragment);
    NdisGetCurrentSystemTime((PLARGE_INTEGER)&ullHostTimeStamp);
    
    do
    {
        // 
        // Modifying data in the AP entry
        //
        NdisDprAcquireSpinLock(&(pBSSEntry->Lock));

        if (pDot11BeaconFrame->Capability.IBSS)
        {
            pBSSEntry->Dot11BSSType = dot11_BSS_type_independent;
                    
            //
            // Adhoc station can leave adhoc cell and create a new cell.
            // In this case BSSID can change
            //

            NdisMoveMemory(
                pBSSEntry->Dot11BSSID,
                pMgmtPktHeader->BSSID,
                sizeof(DOT11_MAC_ADDRESS)
                );
        }
        else
        {   
            pBSSEntry->Dot11BSSType = dot11_BSS_type_infrastructure;
        }

        pBSSEntry->HostTimestamp = ullHostTimeStamp;
        pBSSEntry->BeaconTimestamp = pDot11BeaconFrame->Timestamp;
        pBSSEntry->BeaconInterval = pDot11BeaconFrame->BeaconInterval;

        pBSSEntry->NoBeaconRoamTime = pBSSEntry->BeaconInterval * pStation->RegInfo.LostAPRoamBeaconCount;
        if (pBSSEntry->NoBeaconRoamTime < STA_MIN_INFRA_UNREACHABLE_TIMEOUT)
        {
            //
            // Beacon interval is so short that we may think we lost 
            // connectivity when we are scanning. Adjust for that
            //
            pBSSEntry->NoBeaconRoamTime = STA_MIN_INFRA_UNREACHABLE_TIMEOUT;
        }

        pBSSEntry->Dot11Capability = pDot11BeaconFrame->Capability;
        pBSSEntry->RSSI = Hw11GetRSSI(pStation->pNic, pNicFragment);
        pBSSEntry->LinkQuality = Hw11GetCalibratedRSSI(pStation->pNic, pNicFragment);
        pBSSEntry->ChannelCenterFrequency 
            = Hw11GetChannelCenterFrequency(pStation->pNic, pNicFragment);

        //
        // If signal strength was below our threshold, 
        //
        if (pBSSEntry->LinkQuality < pStation->RegInfo.RSSILinkQualityThreshold)
        {
            pBSSEntry->LowQualityCount++;
        }
        else
        {
            pBSSEntry->LowQualityCount = 0;
        }

        //
        // Get channel number at which the frame was received.
        //
        if (Dot11GetChannelForDSPhy(Add2Ptr(pDot11BeaconFrame, uOffsetOfInfoElemBlob),
                                    BeaconDataLength - uOffsetOfInfoElemBlob, 
                                    &channel) != NDIS_STATUS_SUCCESS)
        {
            channel = Hw11GetFragmentChannel(pNicFragment);
        }

        if (channel != 0)
        {
            pBSSEntry->Channel = channel;
        }

        //
        // Get PhyType and PhyId
        //
        PhyType = Hw11DeterminePHYType(pStation->pNic, 
                                       pBSSEntry->Dot11Capability,
                                       pBSSEntry->Channel);
        if (pBSSEntry->Dot11PhyType != PhyType)
        {
            pBSSEntry->Dot11PhyType = PhyType;
            pBSSEntry->PhyId = StaGetPhyIdByType(pStation->pNic, PhyType);
        }

        //
        // Increase the beacon frame size if necessary
        //
        if (pBSSEntry->MaxBeaconFrameSize < BeaconDataLength)
        {
            MP_ALLOCATE_MEMORY(pStation->MiniportAdapterHandle, 
                &pSavedBeaconBuffer, 
                BeaconDataLength,
                STA11_MEMORY_TAG
                );
                
            if (pSavedBeaconBuffer == NULL)
            {
                //
                // Unable to allocate memory for information elements.
                // If this is a new AP entry, we wont be adding it to the list.
                // For existing entries, we end up ignoring the new IE blob
                //
                ndisStatus = NDIS_STATUS_RESOURCES;
                NdisDprReleaseSpinLock(&(pBSSEntry->Lock));
                break;
            }
            //
            // Delete any old blob buffer
            //
            if (pBSSEntry->pDot11BeaconFrame != NULL)
            {
                MP_FREE_MEMORY(pBSSEntry->pDot11BeaconFrame);
            }

            pBSSEntry->pDot11BeaconFrame = pSavedBeaconBuffer;
            pBSSEntry->pDot11InfoElemBlob = (PUCHAR)pSavedBeaconBuffer + uOffsetOfInfoElemBlob;
            pBSSEntry->MaxBeaconFrameSize = BeaconDataLength;
        }
        
        pBSSEntry->BeaconFrameSize = BeaconDataLength;
        pBSSEntry->InfoElemBlobSize = BeaconDataLength - uOffsetOfInfoElemBlob;

        //
        // Update/Save the information element block
        //
        NdisMoveMemory(
            pBSSEntry->pDot11BeaconFrame,
            pDot11BeaconFrame,
            BeaconDataLength
            );

        if (pMgmtPktHeader->FrameControl.Subtype == DOT11_MGMT_SUBTYPE_PROBE_RESPONSE)
        {
            DOT11_SSID probeSSID;
            
            //
            // If this is a probe response, save the SSID from
            // the probe into the AP entry. A probe response for hidden APs
            // may contain the SSID, but beacon need not. So if we just overwrite
            // the probe response buffer with beacon buffer, we would have
            // lost the SSID that we probed for
            //
            ndisStatus = Dot11CopySSIDFromMemoryBlob(
                pBSSEntry->pDot11InfoElemBlob,
                pBSSEntry->InfoElemBlobSize,
                &probeSSID
                );
            if (ndisStatus != NDIS_STATUS_SUCCESS)
            {   
                // Mask this error
                ndisStatus = NDIS_STATUS_SUCCESS;
            }
            else if (probeSSID.uSSIDLength != 0)
            {
                //
                // Probe response has an SSID, copy it in. If no SSID, we dont overwrite
                // any old SSID we may have saved
                //
                NdisMoveMemory(&pBSSEntry->ProbeSSID, &probeSSID, sizeof(DOT11_SSID));
            }
        }


        //
        // Done with our modification of the AP entry
        //
        NdisDprReleaseSpinLock(&(pBSSEntry->Lock));
        
    } while (FALSE);

    return ndisStatus;
}

NDIS_STATUS 
StaSaveBSSInformation(
    __in  PSTATION        pStation,
    __in  PNIC_RX_FRAGMENT    pNicFragment,
    __in  PDOT11_BEACON_FRAME pDot11BeaconFrame,
    __in  ULONG           BeaconDataLength
    )
{
    NDIS_STATUS         ndisStatus = NDIS_STATUS_SUCCESS;
    LOCK_STATE          LockState;
    PSTA_BSS_ENTRY      pBSSEntry = NULL;
    PDOT11_MGMT_HEADER  pMgmtPktHeader;
    PSTA_BSS_LIST       pDiscoveredBSSList = &(pStation->BSSList);

    pMgmtPktHeader = Hw11GetFragmentDataStart(pNicFragment);

    //
    // In most cases, we would be updating information about this
    // AP in our list. For this reason, we only acquire read lock
    // for most cases
    //
    MP_ACQUIRE_READ_LOCK(&(pStation->BSSList.ListLock), &LockState);

    //
    // See if this entry already exists in the list
    //
    pBSSEntry = StaFindBSSEntry(
        pDiscoveredBSSList,
        pMgmtPktHeader->SA
        );

    if (pBSSEntry == NULL)
    {
        //
        // Entry does not exist, we are adding information about a new BSS
        //

        MP_RELEASE_READ_LOCK(&(pStation->BSSList.ListLock), &LockState);

        ndisStatus = StaInsertBSSEntry(
            pStation, 
            pNicFragment, 
            pDot11BeaconFrame, 
            BeaconDataLength
            );
        
    }
    else
    {
        //
        // Entry already exists, we are just updating
        //
        
        ndisStatus = StaUpdateBSSEntry(
            pStation, 
            pBSSEntry,
            pNicFragment, 
            pDot11BeaconFrame, 
            BeaconDataLength
            );  // Read lock still held

        MP_RELEASE_READ_LOCK(&(pStation->BSSList.ListLock), &LockState);
    }

    return ndisStatus;
}

INLINE BOOLEAN
StaIsHiddenSSID(
    __in PUCHAR               SSID,
    __in ULONG                SSIDLength
    )
{
    ULONG i;
    //
    // We flag this as Hidden SSID if the Length is 0
    // of the SSID only contains 0's
    //
    if (SSIDLength == 0)
    {
        // Zero length is hidden SSID
        return TRUE;
    }

    for (i = 0; i < SSIDLength; i++)
    {
        if (SSID[i] != 0)
        {
            // Non zero SSID value
            return FALSE;
        }
    }

    // All 0's
    return TRUE;
}
    
// ENTRY lock held
ULONG
StaGetEnumBSSIELength(
    __in PSTA_BSS_ENTRY       pStaBSSEntry
    )
{
    PUCHAR  pInfoElemBlob = pStaBSSEntry->pDot11InfoElemBlob;
    ULONG   SizeOfBlob = pStaBSSEntry->InfoElemBlobSize;
    ULONG   TotalLengthRequired = 0;
    ULONG   IEEntrySize = 0;
    PDOT11_INFO_ELEMENT pInfoElemHdr = NULL;
    BOOLEAN bSSIDFound = FALSE;

    while(SizeOfBlob) 
    {
        pInfoElemHdr = (PDOT11_INFO_ELEMENT)pInfoElemBlob;
        if (SizeOfBlob < sizeof(DOT11_INFO_ELEMENT)) 
        {
            // Shouldnt happen. The IE's must already be verified
            break;
        }

        IEEntrySize = sizeof(DOT11_INFO_ELEMENT) + 
            pInfoElemHdr->Length;

        // IEEntrySize maybe change below
        SizeOfBlob -= IEEntrySize;
        pInfoElemBlob += IEEntrySize;

        switch (pInfoElemHdr->ElementID)
        {
            case DOT11_INFO_ELEMENT_ID_SSID:
                {
                    //
                    // For SSID, we copy cached Probe SSID if we do not have a SSID
                    // in the Beacon. This is to handle hidden SSID. For hidden SSID, 
                    // the OS would first do a scan and expects us to indicate the 
                    // found SSID so that it can do a connect. Since we may overwrite the 
                    // probe with beacon, we copy the cached SSID over
                    //
                    bSSIDFound = TRUE;
                    
                    if (StaIsHiddenSSID((((PUCHAR)pInfoElemHdr) + sizeof(DOT11_INFO_ELEMENT)), pInfoElemHdr->Length))
                    {
                        if (pStaBSSEntry->ProbeSSID.uSSIDLength != 0)
                        {
                            // Will use cached SSID
                            MpTrace(COMP_SCAN, DBG_LOUD, ("Will use Probe SSID for %02X-%02X-%02X-%02X-%02X-%02X\n", 
                                    pStaBSSEntry->Dot11BSSID[0], pStaBSSEntry->Dot11BSSID[1], pStaBSSEntry->Dot11BSSID[2], 
                                    pStaBSSEntry->Dot11BSSID[3], pStaBSSEntry->Dot11BSSID[4], pStaBSSEntry->Dot11BSSID[5]));
                            IEEntrySize = sizeof(DOT11_INFO_ELEMENT) +
                                pStaBSSEntry->ProbeSSID.uSSIDLength;
                        }
                    }
                }
                break;

            default:
                // For everything else we copy what is in the
                // beacon/probe response
                break;
        }

        TotalLengthRequired += IEEntrySize;
    }

    if (bSSIDFound == FALSE)
    {
        //
        // We didnt have a SSID IE. Check if we have a cached probe SSID
        // If yes, we would add that to the IE's we return
        //
        if (pStaBSSEntry->ProbeSSID.uSSIDLength != 0)
        {
            MpTrace(COMP_SCAN, DBG_LOUD, ("Will use Probe SSID for %02X-%02X-%02X-%02X-%02X-%02X\n", 
                    pStaBSSEntry->Dot11BSSID[0], pStaBSSEntry->Dot11BSSID[1], pStaBSSEntry->Dot11BSSID[2], 
                    pStaBSSEntry->Dot11BSSID[3], pStaBSSEntry->Dot11BSSID[4], pStaBSSEntry->Dot11BSSID[5]));

            TotalLengthRequired += (pStaBSSEntry->ProbeSSID.uSSIDLength + sizeof(DOT11_INFO_ELEMENT));
        }
    }
    
    return TotalLengthRequired;

}

// ENTRY lock held
NDIS_STATUS
StaCopyEnumBSSIEBuffer(
    __in PSTA_BSS_ENTRY       pStaBSSEntry,
    __in PUCHAR               pDestBuffer,
    __in ULONG                DestinationLength,
    __out PULONG               pBytesWritten
    )
{
    PUCHAR  pSrcBlob = pStaBSSEntry->pDot11InfoElemBlob;
    ULONG   SizeOfSrcBlob = pStaBSSEntry->InfoElemBlobSize;
    ULONG   IEEntrySize = 0;
    PDOT11_INFO_ELEMENT pInfoElemHdr = NULL, pTempIEHdr;
    BOOLEAN bSSIDFound = FALSE;
    BOOLEAN bCopySrcEntry = FALSE;

    *pBytesWritten = 0;
    
    while(SizeOfSrcBlob) 
    {
        pInfoElemHdr = (PDOT11_INFO_ELEMENT)pSrcBlob;
        if (SizeOfSrcBlob < sizeof(DOT11_INFO_ELEMENT)) 
        {
            // Shouldnt happen. The IE's must already be verified
            break;
        }

        // Copy the entry from source IE blob
        bCopySrcEntry = TRUE;

        switch (pInfoElemHdr->ElementID)
        {
            case DOT11_INFO_ELEMENT_ID_SSID:
                {
                    //
                    // For SSID, we copy cached Probe SSID if we do not have a SSID
                    // in the Beacon. This is to handle hidden SSID. For hidden SSID, 
                    // the OS would first do a scan and expects us to indicate the 
                    // found SSID so that it can do a connect. Since we may overwrite the 
                    // probe with beacon, we copy the cached SSID over
                    //
                    bSSIDFound = TRUE;

                    if (StaIsHiddenSSID((((PUCHAR)pInfoElemHdr) + sizeof(DOT11_INFO_ELEMENT)), pInfoElemHdr->Length))
                    {
                        // Hidden of length 0
                        if (pStaBSSEntry->ProbeSSID.uSSIDLength != 0)
                        {
                            // Will use cached SSID and not copy source SSID IE
                            bCopySrcEntry = FALSE;

                            IEEntrySize = sizeof(DOT11_INFO_ELEMENT) + 
                                pStaBSSEntry->ProbeSSID.uSSIDLength;

                            if (IEEntrySize > DestinationLength)
                            {
                                return NDIS_STATUS_BUFFER_OVERFLOW;
                            }

                            pTempIEHdr = (PDOT11_INFO_ELEMENT)pDestBuffer;

                            pTempIEHdr->ElementID = DOT11_INFO_ELEMENT_ID_SSID;
                            pTempIEHdr->Length = (UCHAR)pStaBSSEntry->ProbeSSID.uSSIDLength;

                            NdisMoveMemory(pDestBuffer + sizeof(DOT11_INFO_ELEMENT), 
                                pStaBSSEntry->ProbeSSID.ucSSID, 
                                pStaBSSEntry->ProbeSSID.uSSIDLength
                                );

                            DestinationLength -= IEEntrySize;
                            pDestBuffer += IEEntrySize;
                            *pBytesWritten += IEEntrySize;
                        }
                    }
                }
                break;
                
            default:
                //
                // For everything else we copy what is in the
                // beacon/probe response
                //

                //
                // The SSID IE should be first. If it has not yet been found
                // copy the SSID from the probe (if available) before copying the
                // other IEs
                //
                if (bSSIDFound == FALSE)
                {
                    bSSIDFound = TRUE;
                    if (pStaBSSEntry->ProbeSSID.uSSIDLength != 0)
                    {
                        IEEntrySize = sizeof(DOT11_INFO_ELEMENT) + 
                            pStaBSSEntry->ProbeSSID.uSSIDLength;

                        if (IEEntrySize > DestinationLength)
                        {
                            return NDIS_STATUS_BUFFER_OVERFLOW;
                        }

                        pTempIEHdr = (PDOT11_INFO_ELEMENT)pDestBuffer;

                        pTempIEHdr->ElementID = DOT11_INFO_ELEMENT_ID_SSID;
                        pTempIEHdr->Length = (UCHAR)pStaBSSEntry->ProbeSSID.uSSIDLength;

                        NdisMoveMemory(pDestBuffer + sizeof(DOT11_INFO_ELEMENT), 
                            pStaBSSEntry->ProbeSSID.ucSSID, 
                            pStaBSSEntry->ProbeSSID.uSSIDLength
                            );

                        DestinationLength -= IEEntrySize;
                        pDestBuffer += IEEntrySize;
                        
                        *pBytesWritten += IEEntrySize;                        
                    }
                }

                break;
        }

        //
        // Copy/skip past IE in original buffer
        // 
        IEEntrySize = sizeof(DOT11_INFO_ELEMENT) + 
            pInfoElemHdr->Length;

        if (bCopySrcEntry)
        {
            if (IEEntrySize > DestinationLength)
            {
                return NDIS_STATUS_BUFFER_OVERFLOW;
            }        
            //
            // Copy IE from source buffer to destination
            //
            NdisMoveMemory(pDestBuffer, pSrcBlob, IEEntrySize);

            DestinationLength -= IEEntrySize;
            pDestBuffer += IEEntrySize;
            *pBytesWritten += IEEntrySize;
        }

        SizeOfSrcBlob -= IEEntrySize;
        pSrcBlob += IEEntrySize;
    }

    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Sta11EnumerateBSSList(
    __in PSTATION         pStation,
    __inout PDOT11_BYTE_ARRAY        pDot11ByteArray,
    __in ULONG            TotalLength
    )
{
    NDIS_STATUS         ndisStatus = NDIS_STATUS_SUCCESS;
    PLIST_ENTRY         pHead = NULL, pEntry = NULL;
    PSTA_BSS_ENTRY      pStaBSSEntry = NULL;
    LOCK_STATE          LockState;
    ULONG               RemainingBytes = 0;
    ULONG               BSSEntrySize = 0;
    PDOT11_BSS_ENTRY    pDot11BSSEntry = NULL;
    PUCHAR              pCurrPtr = pDot11ByteArray->ucBuffer;
    ULONGLONG           ullOldestAllowedEntry;
    PSTA_BSS_LIST       pDiscoveredBSSList = &(pStation->BSSList);
    ULONG               IEBlobSize;

    pDot11ByteArray->uNumOfBytes = 0;
    pDot11ByteArray->uTotalNumOfBytes = 0;

    //
    // Determine time to use for expiring AP's
    //
    NdisGetCurrentSystemTime((PLARGE_INTEGER)&ullOldestAllowedEntry);
    if (pStation->RegInfo.BSSEntryExpireTime <= ullOldestAllowedEntry)
        ullOldestAllowedEntry -= pStation->RegInfo.BSSEntryExpireTime;

    RemainingBytes = TotalLength 
        - FIELD_OFFSET(DOT11_BYTE_ARRAY, ucBuffer);

    //
    // Only need read access to the list
    //
    MP_ACQUIRE_READ_LOCK(&(pStation->BSSList.ListLock), &LockState);

    pHead = &(pDiscoveredBSSList->List);
    pEntry = pHead->Flink;
    while(pEntry != pHead) 
    {
        pStaBSSEntry = CONTAINING_RECORD(pEntry, STA_BSS_ENTRY, Link);
        pEntry = pEntry->Flink;

        NdisAcquireSpinLock(&(pStaBSSEntry->Lock));
        
        //
        // Ignore stale entries
        //
        if (pStaBSSEntry->HostTimestamp < ullOldestAllowedEntry)
        {
            NdisReleaseSpinLock(&(pStaBSSEntry->Lock));
            continue;
        }

        //
        // Determine number of bytes needed for writing this entry
        //
        BSSEntrySize = FIELD_OFFSET(DOT11_BSS_ENTRY, ucBuffer)
            + StaGetEnumBSSIELength(pStaBSSEntry);

        pDot11ByteArray->uTotalNumOfBytes += BSSEntrySize;

        if (RemainingBytes >= BSSEntrySize)
        {
            //
            // Copy this AP information to caller buffer
            //
            pDot11BSSEntry = (PDOT11_BSS_ENTRY)pCurrPtr;

            pDot11BSSEntry->uPhyId = pStaBSSEntry->PhyId;
            pDot11BSSEntry->usBeaconPeriod = pStaBSSEntry->BeaconInterval;
            pDot11BSSEntry->ullTimestamp = pStaBSSEntry->BeaconTimestamp;
            pDot11BSSEntry->ullHostTimestamp = pStaBSSEntry->HostTimestamp;
            pDot11BSSEntry->dot11BSSType = pStaBSSEntry->Dot11BSSType;
            pDot11BSSEntry->usCapabilityInformation = pStaBSSEntry->Dot11Capability.usValue;
            pDot11BSSEntry->lRSSI = pStaBSSEntry->RSSI;
            pDot11BSSEntry->uLinkQuality = pStaBSSEntry->LinkQuality;

            pDot11BSSEntry->PhySpecificInfo.uChCenterFrequency 
                = pStaBSSEntry->ChannelCenterFrequency;

            //
            // TODO: set the following properly.
            //
            pDot11BSSEntry->bInRegDomain = TRUE;
            
            NdisMoveMemory(
                pDot11BSSEntry->dot11BSSID,
                pStaBSSEntry->Dot11BSSID,
                sizeof(DOT11_MAC_ADDRESS)
                );

            BSSEntrySize = FIELD_OFFSET(DOT11_BSS_ENTRY, ucBuffer);
            pDot11ByteArray->uNumOfBytes += BSSEntrySize;
            pCurrPtr+= BSSEntrySize;
            RemainingBytes -= BSSEntrySize;

            //
            // Copy the IEs
            //
            ndisStatus = StaCopyEnumBSSIEBuffer(
                pStaBSSEntry,
                pDot11BSSEntry->ucBuffer,
                RemainingBytes,
                &IEBlobSize
                );

            if (ndisStatus == NDIS_STATUS_SUCCESS)
            {
                pDot11BSSEntry->uBufferLength = IEBlobSize;

                pDot11ByteArray->uNumOfBytes += IEBlobSize;
                pCurrPtr+= IEBlobSize;
                RemainingBytes -= IEBlobSize;
            }
            else
            {
                pDot11BSSEntry->uBufferLength = 0;
                RemainingBytes = 0;
                //
                // We continue walking through the list to determine the total
                // space required for this OID
                //
            }
        }
        else
        {
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            RemainingBytes = 0;
            //
            // We continue walking through the list to determine the total
            // space required for this OID
            //
        }
        NdisReleaseSpinLock(&(pStaBSSEntry->Lock));
    }

    MP_RELEASE_READ_LOCK(&(pStation->BSSList.ListLock),&LockState);

    return ndisStatus;
}

NDIS_STATUS
Sta11FlushBSSList(
    __in PSTATION         pStation
    )
{
    LOCK_STATE          LockState;
    PLIST_ENTRY         pListEntry;
    PSTA_BSS_ENTRY      pBSSEntry = NULL;
    PSTA_BSS_LIST       pDiscoveredBSSList = &(pStation->BSSList);
    LONG                APRefCount;
    LIST_ENTRY          TempList;

    //
    // Entries that are currently in use (eg for connection)
    // we cannot flush and instead would put in the temporary queue
    //
    InitializeListHead(&TempList);
    
    MP_ACQUIRE_WRITE_LOCK(&(pStation->BSSList.ListLock), &LockState);
        
    while (!IsListEmpty(&(pDiscoveredBSSList->List)))
    {
        pListEntry = RemoveHeadList(&(pDiscoveredBSSList->List));
        pBSSEntry = CONTAINING_RECORD(pListEntry, STA_BSS_ENTRY, Link);

        APRefCount = NdisInterlockedDecrement(&(pBSSEntry->RefCount));
        if (APRefCount == 0)
        {        
            NdisAcquireSpinLock(&(pBSSEntry->Lock));
            MPVERIFY(pBSSEntry->pAssocRequest == NULL);
            MPVERIFY(pBSSEntry->pAssocResponse == NULL);
            
            if (pBSSEntry->pDot11BeaconFrame != NULL)
            {
                MP_FREE_MEMORY(pBSSEntry->pDot11BeaconFrame);
                pBSSEntry->pDot11BeaconFrame = NULL;
                pBSSEntry->pDot11InfoElemBlob = NULL;
                pBSSEntry->InfoElemBlobSize = 0;
                pBSSEntry->BeaconFrameSize = 0;
                pBSSEntry->MaxBeaconFrameSize= 0;
            }
            NdisReleaseSpinLock(&(pBSSEntry->Lock));
            
            MP_FREE_MEMORY(pBSSEntry);        
        }
        else
        {
            // Restore refcount and save for adding back to list
            NdisInterlockedIncrement(&(pBSSEntry->RefCount));
            InsertTailList(&TempList, pListEntry);
        }
    }
    pDiscoveredBSSList->NumOfBSSEntries = 0;

    //
    // Restore entries that are in use
    //
    while (!IsListEmpty(&TempList))
    {
        pListEntry = RemoveHeadList(&TempList);

        InsertTailList(&(pDiscoveredBSSList->List), pListEntry);
        pDiscoveredBSSList->NumOfBSSEntries++;
    }
    
    MP_RELEASE_WRITE_LOCK(&(pStation->BSSList.ListLock), &LockState);

    return NDIS_STATUS_SUCCESS;
}

// TODO: indicate link quality after connection
NDIS_STATUS 
StaProcessBeaconForConfigInfo(
    __in  PSTATION                        pStation,
    __in  PNIC_RX_FRAGMENT                pNicFragment,
    __in  PUCHAR                          pInfoBlob,
    __in  ULONG                           TotalLength
    )
{
    NDIS_STATUS         status = NDIS_STATUS_SUCCESS;
    BOOLEAN             beaconIsFromAP = FALSE;

    do
    {
        if (pStation->Config.CheckForProtectionInERP == FALSE &&
            pStation->ConnectContext.UpdateLinkQuality == FALSE)
        {
            break;
        }
            
        NdisDprAcquireSpinLock(&(pStation->ConnectContext.Lock));
        if (pStation->ConnectContext.AssociateState == ASSOC_STATE_ASSOCIATED &&
            NdisEqualMemory(pStation->ConnectContext.ActiveAP->Dot11BSSID,
                            ((DOT11_MGMT_HEADER*)Hw11GetFragmentDataStart(pNicFragment))->SA,
                            sizeof(DOT11_MAC_ADDRESS)))
        {
            beaconIsFromAP = TRUE;
        }
        NdisDprReleaseSpinLock(&(pStation->ConnectContext.Lock));

        if (beaconIsFromAP == FALSE)
        {
            break;
        }

        // look for UsbProtection bit in beacon's ERP IE
        if (pStation->Config.CheckForProtectionInERP == TRUE)
        {
            Hw11CheckForProtectionInERP(pStation->pNic, pInfoBlob, TotalLength);
            pStation->Config.CheckForProtectionInERP = FALSE;
        }

        // indicate link quality indication to the os
        if (pStation->ConnectContext.UpdateLinkQuality == TRUE &&
            pStation->pAdapter->Dot11RunningMode == RUNNING_MODE_STA_INFRASTRUCTURE)
        {
            UCHAR                           buffer[sizeof(DOT11_LINK_QUALITY_PARAMETERS) + sizeof(DOT11_LINK_QUALITY_ENTRY)];
            ULONG                           bufferLength = sizeof(buffer);
            DOT11_LINK_QUALITY_PARAMETERS*  pLinkQualityParams = (DOT11_LINK_QUALITY_PARAMETERS*)&buffer[0];
            DOT11_LINK_QUALITY_ENTRY*       pEntry = (DOT11_LINK_QUALITY_ENTRY*)&buffer[sizeof(DOT11_LINK_QUALITY_PARAMETERS)];

            // reset the variable
            pStation->ConnectContext.UpdateLinkQuality = FALSE;

            // initialize indication buffer
            NdisZeroMemory(&buffer[0], bufferLength);

            MP_ASSIGN_NDIS_OBJECT_HEADER(pLinkQualityParams->Header, 
                                         NDIS_OBJECT_TYPE_DEFAULT,
                                         DOT11_LINK_QUALITY_PARAMETERS_REVISION_1,
                                         sizeof(DOT11_LINK_QUALITY_PARAMETERS));

            pLinkQualityParams->uLinkQualityListSize = 1;
            pLinkQualityParams->uLinkQualityListOffset = sizeof(DOT11_LINK_QUALITY_PARAMETERS);

            // previous NdisZeroMemory already set pEntry->PeerMacAddr to all 0x00, which
            // means the link quality is for current network
            pEntry->ucLinkQuality = (UCHAR)Hw11GetCalibratedRSSI(pStation->pNic, pNicFragment);

            StaIndicateDot11Status(pStation, 
                                   NDIS_STATUS_DOT11_LINK_QUALITY,
                                   NULL,
                                   &buffer[0],
                                   bufferLength);
        }
    } while (FALSE);
    
    return status;
}

VOID 
StaReceiveBeacon(
    __in  PSTATION                        pStation,
    __in  PNIC_RX_FRAGMENT                pNicFragment,
    __in  ULONG                           TotalLength
    )
{
    NDIS_STATUS         ndisStatus = NDIS_STATUS_SUCCESS;
    PUCHAR              pPacketBuffer;
    PDOT11_BEACON_FRAME pDot11BeaconFrame;
    ULONG               uOffsetOfInfoElemBlob =
                            FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements) + sizeof(DOT11_MGMT_HEADER);
    ULONG               uInfoElemBlobSize = 0;

    pPacketBuffer = Hw11GetFragmentDataStart(pNicFragment);
    do
    {
        // 
        // Drop if its doesnt contain atleast the
        // fixed size portion (DOT11_BEACON_FRAME)
        //
        if (uOffsetOfInfoElemBlob > TotalLength)
        {
            break;
        }

        // Get/Validate beacon is okay
        pDot11BeaconFrame = (PDOT11_BEACON_FRAME)(pPacketBuffer + sizeof(DOT11_MGMT_HEADER));
        ndisStatus = StaValidateBeacon(pDot11BeaconFrame);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            break;
        }

        // Validate information elements blob
        ndisStatus = Dot11GetInfoBlobSize(
            pPacketBuffer,
            TotalLength,
            uOffsetOfInfoElemBlob,
            &uInfoElemBlobSize
            );
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            break;
        }

        if (pDot11BeaconFrame->Capability.IBSS) {
            ndisStatus = StaSaveAdHocStaInfo(
                pStation,
                pNicFragment,
                pDot11BeaconFrame,
                uInfoElemBlobSize
                );
        }

        // Save information from this beacon into our BSS list
        ndisStatus = StaSaveBSSInformation(
            pStation,
            pNicFragment,
            pDot11BeaconFrame,
            (uInfoElemBlobSize + FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements))    // Info elements + header
            );
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            break;
        }

        ndisStatus = StaProcessBeaconForConfigInfo(
            pStation,
            pNicFragment,
            (PUCHAR)&pDot11BeaconFrame->InfoElements,
            TotalLength
            );
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            break;
        }
    } while (FALSE);
}

BOOLEAN
StaMatchAPSSID(
    __in  PSTA_BSS_ENTRY                  pAPEntry,
    __in  PSTA_CURRENT_CONFIG             pConfig
    )
{
    DOT11_SSID      APSSID, *pMatchSSID;
    NDIS_STATUS     ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN         bIsAcceptable = TRUE;  // Default accept
    ULONG           i;

    do
    {
        // Zero length DesiredSSID is the wildcard SSID. That would match any AP and
        // we dont need to compare                      
        if (pConfig->SSID.uSSIDLength != 0)
        {
            // Get the SSID from the IE's
            ndisStatus = Dot11CopySSIDFromMemoryBlob(
                pAPEntry->pDot11InfoElemBlob,
                pAPEntry->InfoElemBlobSize,
                &APSSID
                );

            if (ndisStatus != NDIS_STATUS_SUCCESS)
            {
                // No SSID IE in the AP entry. We assume this is a bad AP and
                // reject it
                MpTrace(COMP_ASSOC, DBG_LOUD, (" - Reject (No SSID IE)\n"));
                bIsAcceptable = FALSE;
                break;
            }        

            pMatchSSID = &APSSID;
            if (StaIsHiddenSSID(pMatchSSID->ucSSID, pMatchSSID->uSSIDLength))
            {
                // Hidden SSID. See if we have an ssid from a probe response
                pMatchSSID = &(pAPEntry->ProbeSSID);
                if (pAPEntry->ProbeSSID.uSSIDLength == 0)
                {
                    // SSID not available. Reject AP
                    MpTrace(COMP_ASSOC, DBG_LOUD, (" - Reject (hidden SSID )\n"));
                    bIsAcceptable = FALSE;
                    break;
                }
            }
            
            // Check that SSID matches (Note: case sensitive comparison)
            if ((pMatchSSID->uSSIDLength != pConfig->SSID.uSSIDLength) ||
                (NdisEqualMemory(pMatchSSID->ucSSID, pConfig->SSID.ucSSID, pMatchSSID->uSSIDLength) != TRUE))
            {
                MpTrace(COMP_ASSOC, DBG_LOUD, (" - Reject (mismatched SSID)\n"));
                bIsAcceptable = FALSE;
                break;
            }
        }
        
        // Check the excluded MAC address list
        if (pConfig->IgnoreAllMACAddresses)
        {
            MpTrace(COMP_ASSOC, DBG_LOUD, (" - Reject (Ignore all MAC addresses)\n"));
            bIsAcceptable = FALSE;
            break;
        }
        else
        {
            bIsAcceptable = TRUE;        

            // Walk through the excluded MAC address list
            for (i = 0; i < pConfig->ExcludedMACAddressCount; i++)
            {
                if (MP_COMPARE_MAC_ADDRESS(pAPEntry->MacAddress,
                        pConfig->ExcludedMACAddressList[i]) == TRUE)
                {
                    bIsAcceptable = FALSE;
                    break;
                }
            }

            if (bIsAcceptable == FALSE)
            {
                MpTrace(COMP_ASSOC, DBG_LOUD, (" - Reject (Excluded MAC addresses)\n"));
                break;
            }
        }

        // Check the desired BSSID list
        if (pConfig->AcceptAnyBSSID == FALSE)
        {
            bIsAcceptable = FALSE;        

            // Walk through the desired BSSID list
            for (i = 0; i < pConfig->DesiredBSSIDCount; i++)
            {
                if (MP_COMPARE_MAC_ADDRESS(pAPEntry->Dot11BSSID,
                        pConfig->DesiredBSSIDList[i]) == TRUE)
                {
                    // This is an acceptable BSSID
                    bIsAcceptable = TRUE;
                    break;
                }
            }

            if (bIsAcceptable == FALSE)
            {
                MpTrace(COMP_ASSOC, DBG_LOUD, (" - Reject (Not in desired BSSID list)\n"));
                break;
            }
        }
        
        // The SSID matches
        bIsAcceptable = TRUE;
        break;
        
    }while (FALSE);

    return bIsAcceptable;
}

BOOLEAN
StaMatchPhyId(
    __in  PSTATION    pStation,
    __in  ULONG       PhyId
    )
{
    ULONG                       index;

    ASSERT(PhyId != DOT11_PHY_ID_ANY);

    if (PhyId == DOT11_PHY_ID_ANY || PhyId == STA_INVALID_PHY_ID)
    {
        return FALSE;
    }

    if (pStation->Config.DesiredPhyList[0] == DOT11_PHY_ID_ANY)
        return TRUE;

    //
    // Check if the PHY ID is in our desired PHY ID list
    //
    for (index = 0; index < pStation->Config.DesiredPhyCount; index++)
    {
        if (PhyId == pStation->Config.DesiredPhyList[index])
            return TRUE;
    }

    MpTrace(COMP_ASSOC, DBG_LOUD, (" - Reject (Not in desired PHY list)\n"));
    return FALSE;
}

BOOLEAN
StaMatchAPDataRates(
    __in  PSTA_BSS_ENTRY                  pAPEntry,
    __in  PSTATION                        pStation
    )
{
    NDIS_STATUS     ndisStatus = NDIS_STATUS_SUCCESS;
    DOT11_RATE_SET  rateSet;
    BOOLEAN         bIsAcceptable = TRUE;  // Default accept
    
    do
    {
        ndisStatus = StaGetRateSetFromInfoEle(
                pAPEntry->pDot11InfoElemBlob,
                pAPEntry->InfoElemBlobSize,
                TRUE, 
                &rateSet
                );

        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            MpTrace(COMP_ASSOC, DBG_LOUD, (" - Reject (No Supported Rates IE)\n"));
            bIsAcceptable = FALSE;
            break;
        }

        //
        // Check that all the basic data rates required by the 
        // access point are supported by us
        // 
        ndisStatus = Hw11ValidateOperationalRates(
            pStation->pNic,
            pAPEntry->PhyId,
            rateSet.ucRateSet,
            rateSet.uRateSetLength
            );
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            MpTrace(COMP_ASSOC, DBG_LOUD, (" - Reject (Mismatched data rate)\n"));
            bIsAcceptable = FALSE;
            break;
        }
            
        bIsAcceptable = TRUE;
    }while (FALSE);

    return bIsAcceptable;
}

BOOLEAN
StaMatchAPSecurity(
    __in  PSTA_BSS_ENTRY                  pAPEntry,
    __in  PSTATION                        pStation
    )
{
    BOOLEAN                 bIsAcceptable = FALSE;
    UCHAR                   SecIELength;
    PUCHAR                  SecIEData;
    RSN_IE_INFO             RSNIEInfo;
    NDIS_STATUS             ndisStatus;

    __try
    {
        //
        // Privacy bit
        //
        if (pStation->Config.UnicastCipherAlgorithm == DOT11_CIPHER_ALGO_NONE)
        {
            if (pAPEntry->Dot11Capability.Privacy)
            {
                MpTrace(COMP_ASSOC, DBG_LOUD, (" - Reject (Privacy bit set)\n"));
                __leave;
            }
        }
        else if (pStation->Config.UnicastCipherAlgorithm == DOT11_CIPHER_ALGO_WEP)
        {
            //
            // If ExcludeUnencrypted is false, we associate with an AP even if 
            // it is not beaconing privacy bit
            //
            if (pStation->Config.ExcludeUnencrypted == TRUE)
            {
                if (!pAPEntry->Dot11Capability.Privacy)
                {
                    MpTrace(COMP_ASSOC, DBG_LOUD, (" - Reject (Privacy bit clear)\n"));
                    __leave;
                }
            }
        }
        else
        {
            if (!pAPEntry->Dot11Capability.Privacy)
            {
                MpTrace(COMP_ASSOC, DBG_LOUD, (" - Reject (Privacy bit clear)\n"));
                __leave;
            }
        }

        //
        // Check RSNA (WPA2) or WPA
        //
        if (pStation->Config.AuthAlgorithm == DOT11_AUTH_ALGO_RSNA ||
            pStation->Config.AuthAlgorithm == DOT11_AUTH_ALGO_RSNA_PSK)
        {
            ndisStatus = Dot11GetInfoEle(pAPEntry->pDot11InfoElemBlob,
                                         pAPEntry->InfoElemBlobSize,
                                         DOT11_INFO_ELEMENT_ID_RSN,
                                         &SecIELength,
                                         (PVOID)&SecIEData);

            if (ndisStatus != NDIS_STATUS_SUCCESS)
            {
                MpTrace(COMP_ASSOC, DBG_LOUD, (" - Reject (AP not RSNA enabled)\n"));
                __leave;
            }

            ndisStatus = StaParseRNSIE(SecIEData, RSNA_OUI_PREFIX, SecIELength, &RSNIEInfo);
            if (ndisStatus != NDIS_STATUS_SUCCESS)
            {
                MpTrace(COMP_ASSOC, DBG_LOUD, (" - Reject (AP contains invalid RSN IE)\n"));
                __leave;
            }
        }
        else if (pStation->Config.AuthAlgorithm == DOT11_AUTH_ALGO_WPA ||
                 pStation->Config.AuthAlgorithm == DOT11_AUTH_ALGO_WPA_PSK)
        {
            ndisStatus = Dot11GetWPAIE(pAPEntry->pDot11InfoElemBlob,
                                       pAPEntry->InfoElemBlobSize,
                                       &SecIELength,
                                       (PVOID)&SecIEData);

            if (ndisStatus != NDIS_STATUS_SUCCESS)
            {
                MpTrace(COMP_ASSOC, DBG_LOUD, (" - Reject (AP not WPA enabled)\n"));
                __leave;
            }

            ndisStatus = StaParseRNSIE(SecIEData, WPA_OUI_PREFIX, SecIELength, &RSNIEInfo);
            if (ndisStatus != NDIS_STATUS_SUCCESS)
            {
                MpTrace(COMP_ASSOC, DBG_LOUD, (" - Reject (AP contains invalid WPA IE)\n"));
                __leave;
            }
        }
        else
        {
            bIsAcceptable = TRUE;
            __leave;
        }

        //
        // Check if the AP is running RNSA/WPA with AKM and ciphers that meet our requirement. 
        //
        bIsAcceptable = StaMatchRSNInfo(pStation, &RSNIEInfo);
    }
    __finally
    {
    }

    return bIsAcceptable;
}

//
// AP entry must be locked against change
//
BOOLEAN
StaMatchAPEntry(
    __in  PSTA_BSS_ENTRY                  pAPEntry,
    __in  PSTATION                        pStation
    )
{
    PSTA_CURRENT_CONFIG  pConfig = &(pStation->Config);

    MpTrace(COMP_ASSOC, DBG_LOUD, ("Matching AP: %02X-%02X-%02X-%02X-%02X-%02X", 
        pAPEntry->Dot11BSSID[0], pAPEntry->Dot11BSSID[1], pAPEntry->Dot11BSSID[2], 
        pAPEntry->Dot11BSSID[3], pAPEntry->Dot11BSSID[4], pAPEntry->Dot11BSSID[5]));
    
    // Ignore entries ready for deletion
    if (pAPEntry->RefCount < 1)
    {
        MpTrace(COMP_ASSOC, DBG_LOUD, (" - Reject (Deleting)\n"));
        return FALSE;
    }

    // BSS Type
    if (pConfig->BSSType != pAPEntry->Dot11BSSType)
    {
        MpTrace(COMP_ASSOC, DBG_LOUD, (" - Reject (mismatched BSS)\n"));
        return FALSE;
    }

    // Check AP
    if (StaMatchAPSSID(pAPEntry, pConfig) == FALSE)
    {
        return FALSE;
    }

    // Check PHY type
    if (StaMatchPhyId(pStation, pAPEntry->PhyId) == FALSE)
    {
        return FALSE;
    }

    // Match data rates
    if (StaMatchAPDataRates(pAPEntry, pStation) == FALSE)
    {
        return FALSE;
    }

    // Match WEP/WPA/WPA2 capabilities
    if (StaMatchAPSecurity(pAPEntry, pStation) == FALSE)
    {
        return FALSE;
    }

    MpTrace(COMP_ASSOC, DBG_LOUD, (" - Accepted, RSSI %d\n", pAPEntry->RSSI));
    
    // We can use this AP
    return TRUE;
}

NDIS_STATUS
StaRankCandidateAPList(
    __in  PSTATION                        pStation
    )
{
    PSTA_BSS_ENTRY *APList;
    PSTA_BSS_ENTRY  tmpAP;
    ULONG           APCount;
    ULONG           i, j;

    APList = pStation->ConnectContext.CandidateAPList;
    APCount = pStation->ConnectContext.CandidateAPCount;
    if (APCount < 2)
        return NDIS_STATUS_SUCCESS;

    //
    // Order all the candidate APs according to RSSI. 
    //

    for (i = 0; i < APCount - 1; i++) 
    {
        for (j = i + 1; j < APCount; j++)
        {
            if (APList[i]->LinkQuality < APList[j]->LinkQuality)
            {
                tmpAP = APList[i];
                APList[i] = APList[j];
                APList[j] = tmpAP;
            }
        }
    }

    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
StaGetCandidateAPList(
    __in  PSTATION                        pStation,
    __in  BOOLEAN                         bStrictFiltering
    )
{
    NDIS_STATUS         ndisStatus = NDIS_STATUS_SUCCESS;
    LOCK_STATE          LockState;
    PSTA_BSS_ENTRY      pAPEntry = NULL;
    PLIST_ENTRY         pHead = NULL, pEntry = NULL;
    PSTA_BSS_LIST       pDiscoveredAPList = &(pStation->BSSList);
    ULONGLONG           ullOldestAllowedEntry;

    //
    // Determine the expiry time we will use for determining if we
    // we will be picking an access point or not    
    //
    NdisGetCurrentSystemTime((PLARGE_INTEGER)&ullOldestAllowedEntry);
    if (bStrictFiltering)
    {
        //
        // We get stricter when determining which APs have expired 
        //
        if (STA_MIN_INFRA_UNREACHABLE_TIMEOUT <= ullOldestAllowedEntry)
            ullOldestAllowedEntry -= STA_MIN_INFRA_UNREACHABLE_TIMEOUT;
    }
    else
    {
        //
        // We will be more relaxed in picking stale APs
        //
        if (pStation->RegInfo.BSSEntryExpireTime <= ullOldestAllowedEntry)
            ullOldestAllowedEntry -= pStation->RegInfo.BSSEntryExpireTime;
    }

    pStation->ConnectContext.CandidateAPCount = 0;
    
    MP_ACQUIRE_READ_LOCK(&(pStation->BSSList.ListLock), &LockState);

    pHead = &(pDiscoveredAPList->List);
    pEntry = pHead->Flink;
    //
    // Try to find as many access points as we can
    //
    while((pEntry != pHead) && 
          (pStation->ConnectContext.CandidateAPCount < STA_CANDIDATE_AP_MAX_COUNT)
          )
    {
        pAPEntry = CONTAINING_RECORD(pEntry, STA_BSS_ENTRY, Link);
        pEntry = pEntry->Flink;

        NdisDprAcquireSpinLock(&(pAPEntry->Lock)); // Lock AP entry
        
        //
        // Ignore stale entries. We do periodic scanning, so it
        // an AP is not reasonably fresh, we dont accept it
        //
        if (pAPEntry->HostTimestamp < ullOldestAllowedEntry)
        {
            NdisDprReleaseSpinLock(&(pAPEntry->Lock));
            continue;
        }

        //
        // If we have failed/dropped association with this AP too many
        // times, we wont roam to it. This mainly helps us ensure
        // that we dont keep jumping between APs
        //
        if ((bStrictFiltering) && (pAPEntry->AssocCost > STA_ASSOC_COST_MAX_DONT_CONNECT))
        {
            // If the AP we are connected to goes away and all others 
            // are costly, we could lose connectivity. The roaming caller
            // needs to ensure that in such case, StrictFiltering is off
            NdisDprReleaseSpinLock(&(pAPEntry->Lock));
            continue;
        }

        //
        // Match the AP information with our AP/roaming filter
        //
        if (StaMatchAPEntry(
            pAPEntry,
            pStation
            ) == TRUE)
        {
            // Add a refcount so that we can ensure that the access point
            // does not go away while we rank/use it. Note we still have the
            // list lock, so the entry hasnt yet been deleted or modified
            if (NdisInterlockedIncrement(&(pAPEntry->RefCount)) > 1)
            {
                pStation->ConnectContext.CandidateAPList[pStation->ConnectContext.CandidateAPCount] = pAPEntry;
                pStation->ConnectContext.CandidateAPCount++;
            }
            else
            {
                // This entry maybe going away, dont use it
                NdisInterlockedDecrement(&(pAPEntry->RefCount));
            }
        }
        
        NdisDprReleaseSpinLock(&(pAPEntry->Lock));
    }

    //
    // Now reorder APs in our candidate list to have the most 
    // preferred AP first
    //
    ndisStatus = StaRankCandidateAPList(pStation);

    MP_RELEASE_READ_LOCK(&(pStation->BSSList.ListLock), &LockState);

    return ndisStatus;
}

