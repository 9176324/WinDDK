/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Hw_Recv.c

Abstract:
    Hw layer receive processing functions
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/

#include "hw_pcomp.h"
#include "hw_main.h"
#include "hw_nic.h"
#include "hw_Func.h"
#include "hw_ccmp.h"
#include "mp_recv.h"
#include "bus_intf.h"
#include "Windot11.h"

VOID
FreeNicFragmentLocked(
    PNIC    Nic,
    PNIC_RX_FRAGMENT     pNicFragment
    );

VOID
FreeNicFragment(
    PNIC    Nic,
    PNIC_RX_FRAGMENT     pNicFragment
    );

PNIC_RX_FRAGMENT
GetNicFragmentLocked(
    PNIC Nic
    );

#if DOT11_TRACE_ENABLED
    #include "Hw_Recv.tmh"
#endif

#pragma warning(disable:4748)
//
// If this table ever changes, adjust HwTranslateChannelFreqToLogical
//
ULONG DSSS_Freq_Channel[] = {
    0,
    2412,
    2417,
    2422,
    2427,
    2432,
    2437,
    2442,
    2447,
    2452,
    2457,
    2462,
    2467,
    2472,
    2484
};

#define MP_INCREMENT_RETURN_CURRENT_POINTER(_Nic)    \
    (_Nic->RxInfo.RxNextDescToReturn = (_Nic->RxInfo.RxNextDescToReturn + 1) % _Nic->RxInfo.RxTotalDescNum)

NDIS_STATUS
Hw11InitializeReceive(
                     __in  PNIC Nic,
                     __in  ULONG TotalFragments 
                     ) 
{
    NDIS_STATUS      ndisStatus = NDIS_STATUS_SUCCESS;
    UCHAR            descCount;
    PNIC_RX_FRAGMENT pNicFragment;
    BOOLEAN          bMpInitialized = FALSE;
    NTSTATUS         ntStatus;
    WDF_OBJECT_ATTRIBUTES attributes;
    __try 
    {
        InitializeListHead(&Nic->RxInfo.RxUnusedFragList); 
        InitializeListHead(&Nic->RxInfo.RxFragList); //RxUnusedFragList
        InitializeListHead(&Nic->RxInfo.PendingList); //RxUnusedFragList
        HwBusAllocateRecvResources(Nic);
        NdisInitializeNPagedLookasideList(&Nic->RxInfo.RecvFRAGMENTLookaside,
                                          NULL,
                                          NULL,
                                          0,
                                          sizeof(NIC_RX_FRAGMENT),
                                          '7818', 
                                          0);

        //
        // Allocate pAdapter->ReceivePacketPool and pAdapter->ReceiveBufferPool
        //
        ndisStatus = Mp11InitializeReceiveEngine(Nic->Adapter, TotalFragments);
        
        if (ndisStatus != NDIS_STATUS_SUCCESS) {
            MpTrace(COMP_RECV, DBG_SERIOUS, ("Mp11InitializeReceiveEngine fail!\n"));
            __leave;
        }
        
        bMpInitialized = TRUE;
        //
        // Set Receive Fragment List
        //
        for (descCount = 0; descCount < TotalFragments; descCount++) {
            pNicFragment = NdisAllocateFromNPagedLookasideList(&Nic->RxInfo.RecvFRAGMENTLookaside);
            
            if (pNicFragment == NULL) {
                MpTrace(COMP_RECV, DBG_SERIOUS, ("NIC_RX_MSDU_OUT_OF_LOOKASIDE_MEMORY for RecvFRAGMENTLookaside\n"));
                ndisStatus = NDIS_STATUS_FAILURE; 
                __leave;   // We try to allocate as many as we can
            }

            //
            // Clear out the NicFragment structure
            //
            NdisZeroMemory(pNicFragment, sizeof(NIC_RX_FRAGMENT));
            ndisStatus = Mp11AssociateRxFragment(Nic->Adapter, pNicFragment, &pNicFragment->MpFragment);
            
            if (ndisStatus != NDIS_STATUS_SUCCESS) {
                MpTrace(COMP_RECV, DBG_SERIOUS, ("Failed to Mp11AssociateRxFragment\n"));
                NdisFreeToNPagedLookasideList(&Nic->RxInfo.RecvFRAGMENTLookaside, pNicFragment);
                ndisStatus = NDIS_STATUS_SUCCESS;   // Error will be masked as we retry
                __leave;
            }

            if (Nic->RxInfo.RxFragListLen >= Nic->RxInfo.RxTotalDescNum) {
                //
                // This descriptor is ready, but not yet available to the hardware
                // Store this in the unused fragments list
                // 
                InsertTailList(&Nic->RxInfo.RxUnusedFragList, &pNicFragment->FragmentList);
            }
            else {
                //
                // Give control of the RX_DESC in this fragment to the hardware
                //
                HwReturnFragment(Nic, pNicFragment); 
                //
                // Add this fragment to the available Frag list
                //
                FreeNicFragmentLocked(Nic,pNicFragment);
            }
            
        }

        WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
        attributes.ParentObject = Mp11GetWdfDevice( Nic->Adapter );
        ntStatus = WdfSpinLockCreate(&attributes, &Nic->RxInfo.RecvLock);
        NT_STATUS_TO_NDIS_STATUS(ntStatus,&ndisStatus);

        if (ndisStatus != NDIS_STATUS_SUCCESS) {
            MpTrace(COMP_RECV, DBG_SERIOUS, ("WdfSpinLockCreate   fail!\n"));
            __leave;
        }

        NdisAllocateSpinLock(&Nic->RxInfo.ListLock);
        NdisAllocateSpinLock(&Nic->RxInfo.PendingListLock);

    }
    __finally 
    {
        if (ndisStatus != NDIS_STATUS_SUCCESS) {
            //
            // Cleanup
            //
            if (bMpInitialized) {
                Mp11TerminateReceiveEngine(Nic->Adapter);
            }

           NdisDeleteNPagedLookasideList(&Nic->RxInfo.RecvFRAGMENTLookaside);
            
        }
        else {    // NDIS_STATUS_SUCCESS
            
            if (Nic->RxInfo.RxFragListLen == 0) {
                //
                // We were not able to allocate any fragments, initialize failed
                //
                Hw11TerminateReceive(Nic);
                ndisStatus = NDIS_STATUS_RESOURCES;
            }
        }    
    }

    return ndisStatus;
}

NDIS_STATUS
HwAssociateNicFragment(
    PNIC                Nic,
    PNIC_RX_FRAGMENT    NicFragment
    )
{
    NDIS_STATUS     ndisStatus;    
    
    ndisStatus = Mp11AssociateRxFragment(Nic->Adapter,NicFragment, &NicFragment->MpFragment);
        
    if (ndisStatus != NDIS_STATUS_SUCCESS) {
            MpTrace(COMP_RECV, DBG_SERIOUS, ("Failed to Mp11AssociateRxFragment\n"));
    }
    else {
            //
            // Give control of the RX_DESC in this fragment to the hardware
            //
            HwReturnFragment(Nic, NicFragment);
            NdisAcquireSpinLock(&Nic->RxInfo.ListLock);
            if (Nic->RxInfo.RxFragListLen >= Nic->RxInfo.RxTotalDescNum) {
                //
                // This descriptor is ready, but not yet available to the hardware
                // Store this in the unused fragments list
                // 
                InsertTailList(&Nic->RxInfo.RxUnusedFragList, &NicFragment->FragmentList);
            }
            else {
                //
                // Add this fragment to the available Frag list
                //
                FreeNicFragmentLocked(Nic, NicFragment);
            }
            NdisReleaseSpinLock(&Nic->RxInfo.ListLock);

    }
            
    return ndisStatus;
}


VOID
Hw11TerminateReceive(
                    __in  PNIC Nic
                    ) 
{
    PNIC_RX_FRAGMENT pNicFragment;
    ULONG            TestOnly = 0;
    BOOLEAN          dispatchLevel;

    //
    // Merge unused queue into FragList queue, so we need only one resource-free code
    //
    dispatchLevel = FALSE;
    
    HwBusFreeRecvResources(Nic);
    MP_ACQUIRE_SPIN_LOCK(&Nic->RxInfo.ListLock, dispatchLevel);

    while (!IsListEmpty(&Nic->RxInfo.RxUnusedFragList)) {

        pNicFragment = (PNIC_RX_FRAGMENT) RemoveHeadList(&Nic->RxInfo.RxUnusedFragList);
        FreeNicFragment(Nic,pNicFragment);
    }

    NdisAcquireSpinLock(&Nic->RxInfo.PendingListLock);
    while (!IsListEmpty(&Nic->RxInfo.PendingList)) {

        pNicFragment = (PNIC_RX_FRAGMENT) RemoveHeadList(&Nic->RxInfo.PendingList);
        FreeNicFragmentLocked(Nic,pNicFragment);      
    }
    NdisReleaseSpinLock(&Nic->RxInfo.PendingListLock);
    ASSERT(Nic->RxInfo.RxFragListLen  == Nic->RxInfo.RxTotalDescNum);
    while (Nic->RxInfo.RxFragListLen > 0 && !IsListEmpty(&Nic->RxInfo.RxFragList)) {
        TestOnly++;
        pNicFragment = GetNicFragmentLocked(Nic);        
        MP_RELEASE_SPIN_LOCK(&Nic->RxInfo.ListLock, dispatchLevel); 
        Hw11FreeFragment(Nic,pNicFragment);
        MP_ACQUIRE_SPIN_LOCK(&Nic->RxInfo.ListLock, dispatchLevel);
    }
    
    MP_RELEASE_SPIN_LOCK(&Nic->RxInfo.ListLock, dispatchLevel);
    ASSERT(Nic->RxInfo.RxFragListLen == 0);
    ASSERT(IsListEmpty(&Nic->RxInfo.RxFragList));
    //
    // Free MP Rx MSDU LookAsideList, RxBufferPool, RxPacketPool
    //
    Mp11TerminateReceiveEngine(Nic->Adapter);

    //
    // Free shared memory allocated during Initialize
    //
    NdisDeleteNPagedLookasideList(&Nic->RxInfo.RecvFRAGMENTLookaside);
}


VOID
HwReturnFragment(
                __in  PNIC Nic,
                __in  PNIC_RX_FRAGMENT pNicFragment
                ) 
{    
    pNicFragment->DataOffset = 0;
    pNicFragment->DataSize = 0;
    pNicFragment->Encrypted = FALSE;
    pNicFragment->Unicast = FALSE;
    pNicFragment->Channel = 0;
    pNicFragment->NicKey = NULL;
    pNicFragment->FrameNumber = 0;
    Mp11FreeRxNetBuffer(pNicFragment);
    HwBusFreeRecvFragment(Nic, pNicFragment);
}


VOID
Hw11ReturnFragment(
                  __in  PNIC             Nic,
                  __in  PNIC_RX_FRAGMENT pNicFragment,
                  __in  BOOLEAN          DispatchLevel
                  ) 
{
    MP_ACQUIRE_SPIN_LOCK(&Nic->RxInfo.ListLock, DispatchLevel);

    //
    //how does the condition below heappen ?
    //   Nic->RxInfo.RxFragListLen >= Nic->RxInfo.RxTotalDescNum
    //

    if (Nic->RxInfo.RxFragListLen >= Nic->RxInfo.RxTotalDescNum) {
        //
        // If the RxFragList has the max possible allowed fragments already in
        // there (the Rtl8180 has a max on 64 desc), we cannot add any right now.
        // When we indicate any frag up, we can then get some Frags from the RxUnunsed
        // Frag list to keep the RxFragList populated
        // 
        
        InsertTailList(&Nic->RxInfo.RxUnusedFragList, &pNicFragment->FragmentList);
    }
    else {
        //
        // Return the RX_DESC in this fragment to the hardware
        //        
        HwReturnFragment(Nic, pNicFragment); 

        //
        // Add this fragment to the available Frag list
        //
        FreeNicFragmentLocked(Nic, pNicFragment);
    }

    MP_RELEASE_SPIN_LOCK(&Nic->RxInfo.ListLock, DispatchLevel);
}


VOID
Hw11FreeFragment(
                __in  PNIC             Nic,
                __in  PNIC_RX_FRAGMENT pNicFragment
                ) 
{
    Mp11DisassociateRxFragment(Nic->Adapter, pNicFragment);
    HwBusFreeRecvFragment(Nic, pNicFragment);
    NdisFreeToNPagedLookasideList(&Nic->RxInfo.RecvFRAGMENTLookaside, pNicFragment);
    
}


/**
 * While handling Interrupt for receive, the Mp functions will query and ask
 * the Hw11 if there are any packets that are ready to be indicated to NDIS. If
 * such a packet is ready this function should return true.
 * 
 * \param Nic      The NIC context for this miniport.
 * \return TRUE if an 802.11 MPDU is ready to be indicated, else false
 * \sa Hw11GetReceivedFragment, Hw11FragmentIsGood
 */

BOOLEAN
Hw11ReceiveIsAvailable(
                      __in  PNIC Nic
                      ) 
{
    BOOLEAN             available;
    PNIC_RX_FRAGMENT    pNicFragment;

    //
    // With USB we add the new Receieves to a list. If the list is non-empty means
    // receive is available. 
    //
    NdisAcquireSpinLock(&Nic->RxInfo.PendingListLock);
    if (!IsListEmpty(&Nic->RxInfo.PendingList)) {

        //
        // Get the next fragment we expect to be filled in by the hardware
        //
        pNicFragment = (PNIC_RX_FRAGMENT) GetListHeadEntry(&Nic->RxInfo.PendingList);
        available = TRUE; 
    }
    else {
        //
        // There cannot be a receive frame, since hardware has no RX_DESC free 
        // available to use.
        //

        available =  FALSE;
    }
    
    NdisReleaseSpinLock(&Nic->RxInfo.PendingListLock);    
    return available;

}


VOID
FillNicFragment(
                __in  PNIC         Nic,
                PNIC_RX_FRAGMENT pNicFragment,
                __in  BOOLEAN      bRawReceive
                ) 
{
    PDOT11_PHY_FRAME_STATISTICS pPhyStats = NULL;

    PDOT11_MAC_HEADER   pFragmentHdr;
    USHORT              headerSize;
    USHORT              overhead;
    USHORT              IVSize;
    UCHAR               keyId;
    PNICKEY             pNicKey = NULL;
    PUCHAR              EncryptionIV;
    NDIS_STATUS         status;
   
    if (bRawReceive) {
        //
        // In raw mode we return the raw packets without doing any adjustment
        // specific to our hardware
        //
        pNicFragment->DataSize = pNicFragment->RxDesc.Length;
        
        //
        // Update stats for received packets
        //
        pPhyStats = &Nic->StatisticsInfo.PhyCounters[Hw11QueryOperatingPhyId(Nic)];
        pPhyStats->ullReceivedFragmentCount++;
        //return pNicFragment;
    }

    //
    // Find the header size of the frame. 
    //
    pFragmentHdr = (PDOT11_MAC_HEADER) Hw11GetVirtualAddress(pNicFragment);
    headerSize = (pFragmentHdr->FrameControl.FromDS && pFragmentHdr->FrameControl.ToDS) ?
                 sizeof(DOT11_DATA_LONG_HEADER) : sizeof(DOT11_DATA_SHORT_HEADER);

    if (ETH_IS_UNICAST(pFragmentHdr->Address1) ) {
        pNicFragment->Unicast = TRUE;
    }
    else {
        pNicFragment->Unicast = FALSE;
    }

    //
    // Check if we need to decrypt the multicast data frame in software. We only
    // do so when we are doing WPA2PSK in adhoc mode.  
    //
    
    if (pFragmentHdr->FrameControl.WEP && 
        Nic->ManagementInfo.CurrentBSSType == dot11_BSS_type_independent &&
        Nic->ManagementInfo.HwAuthAlgorithm == DOT11_AUTH_ALGO_RSNA_PSK &&
        pFragmentHdr->FrameControl.Type == DOT11_FRAME_TYPE_DATA &&
        pFragmentHdr->FrameControl.FromDS == 0 &&
        pFragmentHdr->FrameControl.ToDS == 0 &&
        NdisEqualMemory(((PDOT11_DATA_SHORT_HEADER)pFragmentHdr)->Address3,
                        Nic->ManagementInfo.BssId,
                        sizeof(DOT11_MAC_ADDRESS)) &&
        ETH_IS_MULTICAST(pFragmentHdr->Address1) &&
        !pNicFragment->RxDesc.Decrypted) {
        
        ASSERT(Nic->CardInfo.HardwareID == HW_ID_8187);

        keyId = *((PUCHAR)Add2Ptr(pFragmentHdr, headerSize + 3));
        pNicKey = HwFindPerStaKey(Nic, 
                                  ((PDOT11_DATA_SHORT_HEADER)pFragmentHdr)->Address2,
                                  keyId >> 6);

        if (pNicKey && pNicKey->AlgoId == DOT11_CIPHER_ALGO_CCMP) {
            status = HwDecryptCCMP(pNicKey, 
                                   (PUCHAR)pFragmentHdr, 
                                   pNicFragment->RxDesc.Length - 4);

            //
            // We attempted to decrypt the data. Set the Decrypted flag, and if 
            // decryption failed, set the RXS_ICV in CRCstatus. 
            pNicFragment->RxDesc.Decrypted = 1;
            if (status != NDIS_STATUS_SUCCESS) {
                pNicFragment->RxDesc.ICV = TRUE;
                // Stats are incremented somewhere else
            }
        }
        
    }

    //
    // When encryption is enabled, RTL8180/8185 does the decryption on receive but 
    // does not automatically remove IV and ICV/MIC fields. Manually removes them. It is 
    // done by copying the MAC header forward for the size of IV field. 
    //
    
    if (pFragmentHdr->FrameControl.WEP && 
        Nic->ManagementInfo.HwUnicastCipher != DOT11_CIPHER_ALGO_NONE &&
        (Nic->CardInfo.HardwareID == HW_ID_8180 || pNicFragment->RxDesc.Decrypted)) {
        //
        // Clear the WEP flag.
        //
        pFragmentHdr->FrameControl.WEP = 0;

        //
        // Figure out which key decrypted this frame.
        //
        
        if (!pNicKey) {
            if (ETH_IS_UNICAST(pFragmentHdr->Address1) && pFragmentHdr->FrameControl.Type == DOT11_FRAME_TYPE_DATA) {
                pNicKey = Hw11FindKeyMappingKey(Nic, ((PDOT11_DATA_SHORT_HEADER)pFragmentHdr)->Address2);
            }

            if (!pNicKey) {
                keyId = *((PUCHAR)Add2Ptr(pFragmentHdr, headerSize + 3));
                pNicKey = Nic->ManagementInfo.KeyTable + (keyId >> 6);
            }
        }

        if (pNicKey->Valid) {
            //
            // Find the IVSize and total overhead due to encryption and CRC.
            //
            EncryptionIV = Add2Ptr(pFragmentHdr, headerSize);
            switch (pNicKey->AlgoId) {
                case DOT11_CIPHER_ALGO_TKIP:

                    // 
                    // 8 bytes IV, 4 byte ICV, 4 bytes CRC. The per-MSDU MIC is removed after frame 
                    // reassembling and MIC failure checking.
                    //

                    IVSize = 8;
                    overhead = 16;
                    pNicFragment->FrameNumber = (((ULONGLONG)EncryptionIV[0]) << 8) |
                                                ((ULONGLONG)EncryptionIV[2]) |
                                                (((ULONGLONG)EncryptionIV[4]) << 16) |
                                                (((ULONGLONG)EncryptionIV[5]) << 24) |
                                                (((ULONGLONG)EncryptionIV[6]) << 32) |
                                                (((ULONGLONG)EncryptionIV[7]) << 40);

                    break;

                case DOT11_CIPHER_ALGO_CCMP:

                    // 
                    // 8 bytes IV, 8 bytes MIC, 4 bytes CRC. 
                    //

                    IVSize = 8;
                    overhead = 20;
                    pNicFragment->FrameNumber = ((ULONGLONG)EncryptionIV[0]) |
                                                (((ULONGLONG)EncryptionIV[1]) << 8) |
                                                (((ULONGLONG)EncryptionIV[4]) << 16) |
                                                (((ULONGLONG)EncryptionIV[5]) << 24) |
                                                (((ULONGLONG)EncryptionIV[6]) << 32) |
                                                (((ULONGLONG)EncryptionIV[7]) << 40);

                    break;

                case DOT11_CIPHER_ALGO_WEP104:
                case DOT11_CIPHER_ALGO_WEP40:

                    // 
                    // 4 bytes IV, 4 bytes ICV, 4 bytes CRC. 
                    //

                    IVSize = 4;
                    overhead = 12;
                    break;

                default:
                    ASSERT(0);
                    IVSize = 0;
                    overhead = 0;
                    break;
            }

            //
            // Copy the header. It is probably the most efficient way to get rid of the IV field.
            // Note: cannot call NdisMoveMemory since source and destination overlaps.
            //
            RtlMoveMemory(((PCHAR)pFragmentHdr) + IVSize, pFragmentHdr, headerSize);

            //
            // Set the data size and offset.
            //

            pNicFragment->DataOffset = IVSize;
            pNicFragment->DataSize = pNicFragment->RxDesc.Length - overhead;
            pNicFragment->Encrypted = TRUE;
            pNicFragment->NicKey = pNicKey;
        }
        else {
            //
            // Hardware attempted decryption, but the key it used was not valid
            //
            pNicFragment->DataSize = pNicFragment->RxDesc.Length - 4;   // CRC
            pNicFragment->Encrypted = TRUE;
            pNicFragment->NicKey = pNicKey;
            pNicFragment->RxDesc.ICV = TRUE;      // Mark as an ICV error
        }
    }
    else {
        pNicFragment->DataSize = pNicFragment->RxDesc.Length - 4;   // CRC
        pNicFragment->Encrypted = (BOOLEAN)(pFragmentHdr->FrameControl.WEP == 1);
    }


    //return pNicFragment;
}


PNIC_RX_FRAGMENT
Hw11GetReceivedFragment(
                       __in  PNIC        Nic,
                       __in  BOOLEAN     bRawReceive
                       ) 
{
    PNIC_RX_FRAGMENT    pNicFragment;

    NdisAcquireSpinLock(&Nic->RxInfo.PendingListLock);

    ASSERT(!IsListEmpty(&Nic->RxInfo.PendingList));
    //    
    // Get the fragment in question
    //
    pNicFragment = (PNIC_RX_FRAGMENT) RemoveHeadList(&Nic->RxInfo.PendingList);
    NdisReleaseSpinLock(&Nic->RxInfo.PendingListLock);
    FillNicFragment(Nic,
                    pNicFragment,
                    bRawReceive
                    );

    return  pNicFragment;   
}

BOOLEAN
Hw11FragmentIsGood(
                  __in  PNIC Nic,
                  __in  PNIC_RX_FRAGMENT pNicFragment
                  ) 
{
    PDOT11_PHY_FRAME_STATISTICS pPhyStats;
    PNICKEY                     pNicKey;
    
    pPhyStats = &Nic->StatisticsInfo.PhyCounters[Hw11QueryOperatingPhyId(Nic)];
    //
    // check the status of the fragment
    //

    if (pNicFragment->RxDesc.CRC32 || pNicFragment->RxDesc.FOVF ) {
        pPhyStats->ullFCSErrorCount++;
        return FALSE;
    }
    else if (pNicFragment->RxDesc.ICV) {

        if (pNicFragment->Encrypted) {
            //
            // For MAC counters, we shouldnt count duplicates, filter those 
            // counters out
            //
            if (HwFragmentIsDuplicate(Nic, pNicFragment)) {
                //
                // Drop and dont count
                //
                return FALSE;
            }

            pNicKey = pNicFragment->NicKey;
            if (pNicKey != NULL) {
                if (pNicKey->Valid) {
                    //
                    // Decryption failed even though we think we have
                    // a valid key. This counts as an ICV error
                    //
                    switch (pNicKey->AlgoId) {
                        case DOT11_CIPHER_ALGO_CCMP:
                            // This is unlikely
                            if (pNicFragment->Unicast) {
                                Nic->StatisticsInfo.UcastCounters.ullCCMPDecryptErrors++;
                            }
                            else {
                                Nic->StatisticsInfo.McastCounters.ullCCMPDecryptErrors++;
                            }
                            break;

                        case DOT11_CIPHER_ALGO_TKIP:
                            if (pNicFragment->Unicast) {
                                Nic->StatisticsInfo.UcastCounters.ullTKIPICVErrorCount++;
                            }
                            else {
                                Nic->StatisticsInfo.McastCounters.ullTKIPICVErrorCount++;
                            }                        
                            break;

                        case DOT11_CIPHER_ALGO_WEP104:
                        case DOT11_CIPHER_ALGO_WEP40:
                            if (pNicFragment->Unicast) {
                                Nic->StatisticsInfo.UcastCounters.ullWEPICVErrorCount++;
                            }
                            else {
                                Nic->StatisticsInfo.McastCounters.ullWEPICVErrorCount++;
                            }
                            break;

                        default:
                            break;
                    }
                }
                else {
                    //
                    // Decryption failed and the key we have is invalid. This happens 
                    // when the hardware uses a default key for decrypting a packet, but
                    // the default key was not set
                    //
                    if (pNicFragment->Unicast) {
                        Nic->StatisticsInfo.UcastCounters.ullWEPUndecryptableCount++;
                    }
                    else {
                        Nic->StatisticsInfo.McastCounters.ullWEPUndecryptableCount++;
                    }                
                }
            }
            else {
                //
                // No key found
                //                
            }
            
            if (pNicFragment->Unicast) {
                Nic->StatisticsInfo.UcastCounters.ullDecryptFailureCount++;
            }
            else {
                Nic->StatisticsInfo.McastCounters.ullDecryptFailureCount++;
            }
        }   
        return FALSE;    
    }
    else {
        pPhyStats->ullReceivedFragmentCount++;
    }

    return TRUE;
}



BOOLEAN
Hw11FilterFragment(
                  __in  PNIC Nic,
                  __in  PNIC_RX_FRAGMENT pNicFragment
                  ) {
    PUCHAR      pktbuf = Add2Ptr(pNicFragment->RxBuffer, pNicFragment->DataOffset);
    UCHAR       type = (pktbuf[0] & 0x0f) >> 2;
    BOOLEAN     bAddrMatch = HwIsAddr1Match(pktbuf, Nic->CardInfo.MacAddr);
    BOOLEAN     bIsBroadcast = HwIsBroadcast(pktbuf);
    BOOLEAN     bIsMulticast = HwIsMulticast(pktbuf);
    PDOT11_PHY_FRAME_STATISTICS pPhyStats = NULL;

    switch (type) {
        case DOT11_FRAME_TYPE_MANAGEMENT:
            if (bAddrMatch && (Nic->RxInfo.ReceiveFilter & NDIS_PACKET_TYPE_802_11_DIRECTED_MGMT))
                return TRUE;

            if (bIsBroadcast && (Nic->RxInfo.ReceiveFilter & NDIS_PACKET_TYPE_802_11_BROADCAST_MGMT))
                return TRUE;

            if (bIsMulticast && (Nic->RxInfo.ReceiveFilter & (NDIS_PACKET_TYPE_802_11_MULTICAST_MGMT | 
                                                              NDIS_PACKET_TYPE_802_11_ALL_MULTICAST_MGMT)))
                return TRUE;

            if (Nic->RxInfo.ReceiveFilter & NDIS_PACKET_TYPE_802_11_PROMISCUOUS_MGMT) {
                pPhyStats = &Nic->StatisticsInfo.PhyCounters[Hw11QueryOperatingPhyId(Nic)];
                pPhyStats->ullPromiscuousReceivedFragmentCount++;
                return TRUE;
            }

            break;

        case DOT11_FRAME_TYPE_CONTROL:
            if (bAddrMatch && (Nic->RxInfo.ReceiveFilter & NDIS_PACKET_TYPE_802_11_DIRECTED_CTRL))
                return TRUE;

            if (bIsBroadcast && (Nic->RxInfo.ReceiveFilter & NDIS_PACKET_TYPE_802_11_BROADCAST_CTRL))
                return TRUE;

            if (Nic->RxInfo.ReceiveFilter & NDIS_PACKET_TYPE_802_11_PROMISCUOUS_CTRL) {
                pPhyStats = &Nic->StatisticsInfo.PhyCounters[Hw11QueryOperatingPhyId(Nic)];
                pPhyStats->ullPromiscuousReceivedFragmentCount++;
                return TRUE;
            }
            //
            // Note: no multicast control frame.
            //

            break;

        case DOT11_FRAME_TYPE_DATA:
            if (bAddrMatch && (Nic->RxInfo.ReceiveFilter & NDIS_PACKET_TYPE_DIRECTED))
                return TRUE;

            if (bIsBroadcast && (Nic->RxInfo.ReceiveFilter & NDIS_PACKET_TYPE_BROADCAST))
                return TRUE;

            if (bIsMulticast&& (Nic->RxInfo.ReceiveFilter & (NDIS_PACKET_TYPE_MULTICAST | 
                                                             NDIS_PACKET_TYPE_ALL_MULTICAST)))
                return TRUE;

            if (Nic->RxInfo.ReceiveFilter & (NDIS_PACKET_TYPE_PROMISCUOUS | 
                                             NDIS_PACKET_TYPE_802_11_RAW_DATA)) {
                pPhyStats = &Nic->StatisticsInfo.PhyCounters[Hw11QueryOperatingPhyId(Nic)];
                pPhyStats->ullPromiscuousReceivedFragmentCount++;
                return TRUE;
            }

            break;

        default:
            //
            // Reserved packet should always be filtered
            //
            return FALSE;
    }

    return FALSE;
}



NDIS_STATUS
Hw11GrowFragmentPool(
                    __in  PNIC         Nic,
                    __in  ULONG        NumFragmentsToAllocate,
                    __in  ULONG        MaxFragmentsToAllocate
                    )
/*++

Routine Description
    This function is called when the miniport driver starts to run low on
 * the available Receive Buffers for receiving packets. The hardware should
 * allocate NicFragment and call Mp11AssociateRxFragment so that the corresponding
 * MpFragment can be setup. As usual, the Hw11 function should add the frag into the
 * existing pool of fragments appropriately. This function would require shared
 * memory to be allocated via Mp11AllocateSharedMemoryAsync so it should return
 * NDIS_STATUS_PENDING when the allocation request pends.

    

Arguments:
        Nic                      The context for the NIC
        NumFragmentsToAllocate    This is the number of fragments NIC should grow the Pool
 *                                  by. This number can be overridden, The Nic can choose
 *                                  to allocate a different number of fragments as long as it
 *                                  does not exceed MaxFragmentsToAllocate
        MaxFragmentsToAllocate    The maximum number of frags the hardware is allowed to
 *                                  grow the pool by. This number MUST NOT be exceeded.

    
Return Value:
      NDIS_STATUS_PENDING if request to grow the pool pended else NDIS_STATUS_FAILURE
    
--*/

 {
    ULONG           i;
    PNIC_RX_FRAGMENT    pNicFragment;

    UNREFERENCED_PARAMETER(MaxFragmentsToAllocate);
    for (i = 0; i < NumFragmentsToAllocate; i++) {
        pNicFragment = NdisAllocateFromNPagedLookasideList(&Nic->RxInfo.RecvFRAGMENTLookaside);

        if (!pNicFragment) {
            MpTrace(COMP_RECV, DBG_SERIOUS, ("NIC_RX_MSDU_OUT_OF_LOOKASIDE_MEMORY for RecvFRAGMENTLookaside\n"));
            continue;
        }

        //
        // Clear out the NicFragment structure
        //
        NdisZeroMemory(pNicFragment, sizeof(NIC_RX_FRAGMENT));

        //
        // Hw11SharedMemoryAllocateComplete() will be called when complete
        //
        if (HwAssociateNicFragment(Nic,
                                     pNicFragment) != NDIS_STATUS_SUCCESS) {
            //
            // We were not able to allocate memory! Free the fragment associated
            //
            MpTrace(COMP_RECV, DBG_SERIOUS, ("Failed to RxBufferMemory\n"));
            NdisFreeToNPagedLookasideList(&Nic->RxInfo.RecvFRAGMENTLookaside, pNicFragment);

            //
            // Do not try to allocate any more memory. System is running low.
            //
            break;
        }
    }
    
    return NDIS_STATUS_SUCCESS;
}


VOID
HwResetReceive(
              __in  PNIC Nic
              ) {
    
    PNIC_RX_FRAGMENT    pNicFragment;
    PLIST_ENTRY      ple;

    WdfSpinLockAcquire(Nic->RxInfo.RecvLock);

    //
    // Reset the receive list of fragments.
    // First we take ownership of all the RX_DESC away from the HW
    // Next we will assocaite the RX_DESC at the 
    // head of the array with the fragments that are
    // available to us. When hardware resumes, it will start
    // from the head of the array.
    // Note: This mechanism can potentially causes frames to be lost.
    //
    NdisAcquireSpinLock(&Nic->RxInfo.ListLock);

    NdisAcquireSpinLock(&Nic->RxInfo.PendingListLock);
    while (!IsListEmpty(&Nic->RxInfo.PendingList)) {

        pNicFragment = (PNIC_RX_FRAGMENT) RemoveHeadList(&Nic->RxInfo.PendingList);
        FreeNicFragmentLocked(Nic, pNicFragment);
    }
    //    
    // Get the fragment in question
    //
    NdisReleaseSpinLock(&Nic->RxInfo.PendingListLock);
    
    
    ple = GetListHeadEntry(&Nic->RxInfo.RxFragList);
    pNicFragment = CONTAINING_RECORD(ple, NIC_RX_FRAGMENT, FragmentList);
   
    while (pNicFragment) {
        
        if ((PVOID)pNicFragment == (PVOID)&Nic->RxInfo.RxFragList) {
            break;
        }
        //
        // return ownership to hardware
        //
        HwReturnFragment(Nic, pNicFragment);

        //
        // Go to next fragment
        //
        pNicFragment = (PNIC_RX_FRAGMENT)GetListHeadEntry(&pNicFragment->FragmentList);        
    }
    NdisReleaseSpinLock(&Nic->RxInfo.ListLock);
    WdfSpinLockRelease(Nic->RxInfo.RecvLock);
}


ULONG
Hw11GetCalibratedRSSI(
                     __in  PNIC            Nic,
                     __in  PNIC_RX_FRAGMENT    pNicFragment
                     ) 
{   
    return (LONG)HwRSSI(Nic, pNicFragment->RxDesc); //0-100;
}

LONG
Hw11GetRSSI(
           __in  PNIC            Nic,
           __in  PNIC_RX_FRAGMENT    pNicFragment
           ) 
{   
    return(LONG)((HwRSSI(Nic,pNicFragment->RxDesc) + 1) >> 1); //dBm
}

LONG
Hw11GetMinRSSI(
              __in  PNIC Nic,
              __in  ULONG uDataRate
              ) 
{
    UNREFERENCED_PARAMETER(Nic);
    UNREFERENCED_PARAMETER(uDataRate);

    // TODO: RSSI typically does change with Data rate
    return -95;
}


LONG
Hw11GetMaxRSSI(
              __in  PNIC Nic,
              __in  ULONG uDataRate
              ) 
{
    UNREFERENCED_PARAMETER(Nic);
    UNREFERENCED_PARAMETER(uDataRate);

    // TODO: RSSI typically does change with Data rate
    return -45;
}

UCHAR
Hw11GetDataRate(
               __in  PNIC            Nic,
               __in  PNIC_RX_FRAGMENT    pNicFragment
               ) 
{
    UCHAR HwRate;
    UNREFERENCED_PARAMETER(Nic);

    HwRate = (UCHAR) (pNicFragment->RxDesc.RXRATE); // RxDesc.status Bit7-4 is RXRATE

    return HwHwRateToMRate(HwRate);
}

ULONG
Hw11GetChannelCenterFrequency(
                             __in  PNIC            Nic,
                             __in  PNIC_RX_FRAGMENT    pNicFragment
                             ) 
{
    UCHAR               Channel = Nic->ManagementInfo.OperatingPhyMIB->Channel;

    if (pNicFragment->Channel != 0) {
        // For Beacons & probes received during scanning, we would 
        // have saved the channel. Use that instead
        Channel = pNicFragment->Channel;
    }

    if (Channel >= 15) {
        // 11a channel
        return 5000 + 5 * Channel;
    }
    else {
        return DSSS_Freq_Channel[Channel];
    }
   
}

VOID
Hw11FillReceiveContext(
                      __in  PNIC Nic,
                      __in  PNIC_RX_FRAGMENT pNicFragment,
                      __inout PDOT11_EXTSTA_RECV_CONTEXT Dot11RecvContext
                      ) 
{
    //
    // Fill in the send context fields that are always needed
    //
    MP_ASSIGN_NDIS_OBJECT_HEADER(Dot11RecvContext->Header, 
                                 NDIS_OBJECT_TYPE_DEFAULT,
                                 DOT11_EXTSTA_RECV_CONTEXT_REVISION_1,
                                 sizeof(DOT11_EXTSTA_RECV_CONTEXT));
    Dot11RecvContext->uPhyId = Hw11QueryOperatingPhyId(Nic);
    Dot11RecvContext->lRSSI = Hw11GetRSSI(Nic, pNicFragment);
    Dot11RecvContext->ucDataRate = Hw11GetDataRate(Nic, pNicFragment);
    Dot11RecvContext->uChCenterFrequency = Hw11GetChannelCenterFrequency(Nic, pNicFragment);   
    Dot11RecvContext->uReceiveFlags = 0;

    // TODO: what's the equivalent of CRCstatus in _RX_STATUS_DESC_8187?
/*
    if (HW_IS_IN_NETWORK_MONITOR_MODE(Nic))
    {
        //
        // If this packet has a FCS error, set the RAW packet FCS error flag
        //
        if (pNicFragment->RxDesc.CRCstatus & RXS_CRC32) 
            Dot11RecvContext->uReceiveFlags = DOT11_RECV_FLAG_RAW_PACKET_FCS_FAILURE;
    }
*/
}

BOOLEAN
HwIsSSIDAny(
           __in  PDOT11_SSID SSID
           ) 
{
    if (SSID->uSSIDLength == 0) {   // a kind of "ANY SSID"
        return TRUE;
    }

    return(BOOLEAN)((SSID->uSSIDLength == 3) &&
                    (SSID->ucSSID[0] == 'A' || SSID->ucSSID[0] == 'a') &&
                    (SSID->ucSSID[1] == 'N' || SSID->ucSSID[1] == 'n') &&
                    (SSID->ucSSID[2] == 'Y' || SSID->ucSSID[2] == 'y'));
}


//stationrecv already handles becons. 
// is the handling complete because starecv is called by the dpc whereas this is called 
// from the ISR
void
Hw11HandleTimingCriticalPacket(
                            __in  PNIC Nic,
                            PNIC_RX_FRAGMENT    ParamNicFragment
                            ) 
{
    DOT11_SSID          SSID;
    NDIS_STATUS         ndisStatus;
    PUCHAR              pktbuf;
    PNIC_RX_FRAGMENT    pNicFragment;

    pNicFragment = ParamNicFragment;
    
    if (Nic->ManagementInfo.bAdvertiseBSS == FALSE) {
        //
        // We should not be advertising the BSS right now.
        // No need to handle probe requests.
        //
        return;
    }
    
    pktbuf = Add2Ptr(pNicFragment->RxBuffer, pNicFragment->DataOffset);
    
    //
    // Scan Probr_req for all RecvFragList
    //

    if (pNicFragment->RxDesc.CRC32  ||
        pNicFragment->RxDesc.ICV ||
        pNicFragment->RxDesc.FOVF ) {
        //
        // discard CRC packet(Except sniffer mode)
        //
        return;
    }
    if (*pktbuf == 0x40 && pNicFragment->RxDesc.Length > sizeof(DOT11_MGMT_HEADER)) {

        //
        // received a Probe Request, need to send a Probe Response.
        // If the SSID is not ANY or we are a hidden node and this is not
        // a directed request, do not respond to probe
        // 

        ndisStatus = Dot11CopySSIDFromMemoryBlob(pktbuf + sizeof(DOT11_MGMT_HEADER),
                                                 (ULONG)(pNicFragment->RxDesc.Length - sizeof(DOT11_MGMT_HEADER)),
                                                 &SSID);

        if (ndisStatus == NDIS_STATUS_SUCCESS) {

            //
            // Not responding to a probe-request packet with different SSID
            //
            if (HwIsSSIDAny(&SSID) == FALSE &&
                RtlCompareMemory(Nic->ManagementInfo.SSID, SSID.ucSSID, SSID.uSSIDLength) != SSID.uSSIDLength) {
                return;
            }

            HwOnProbe(Nic, 
                      pktbuf, 
                      (USHORT)pNicFragment->RxDesc.Length, 
                      HwHwRateToMRate((UCHAR)((pNicFragment->RxDesc.RXRATE))));
        }
    }
}

VOID
HwResponseToPacket(
                  __in  PNIC Nic,
                  __in  PNIC_RX_FRAGMENT pNicFragment
                  ) 
{
    PUCHAR                  pktbuf;
    USHORT                  pktlen;
    UCHAR                   type;
    UCHAR                   subtype;
    BOOLEAN                 bKeepAwake;
    OctetString             Packet, Tim;
    PDOT11_POWER_MGMT_MODE  PowerMgmtMode;
    PDOT11_BEACON_FRAME     BeaconFrame;
    LARGE_INTEGER           CurrentTime;
    ULONGLONG               TimeDiff;
    ULONG                   SkipCount;
    ULONG                   DTIMCount;

    //
    // if power saving is not turned on simply return. We will stay awake
    //
    PowerMgmtMode = &Nic->ManagementInfo.PowerMgmtMode;
    if (PowerMgmtMode->dot11PowerMode == dot11_power_mode_active){
        return;
    }

    //
    // If we are not associated (with an AP), unless we are in the middle of a scanning, turn 
    // off RF indifinitely. If we are in the middle of a scanning, stay awake.
    //

    if (PowerMgmtMode->usAID == 0){
        if (!Nic->ManagementInfo.bScanInProgress){
            MpTrace(COMP_POWER, DBG_LOUD, (" *** RF OFF indefinitely\n"));
            SetRFPowerState(Nic, eRfOff);
        }
        return;
    }

    //
    // Initialize local variables.
    //

    pktbuf = Add2Ptr(pNicFragment->RxBuffer, pNicFragment->DataOffset);
    pktlen = pNicFragment->RxDesc.Length - 4;
    type = (pktbuf[0]  & 0x0f) >> 2;
    subtype = (pktbuf[0]  & 0xf0) >> 4;
    bKeepAwake = TRUE;

    //
    // Determine if we can doze depending on the frame received.
    //

    switch (type) {
        case 0:             // Management
            switch (subtype) {
                case 0x8:       // Beacon
                    if (HW_IS_IN_STA_INFRA_MODE(Nic) &&
                        PowerMgmtMode->dot11PowerMode == dot11_power_mode_powersave &&
                            memcmp(pktbuf + 16, Nic->ManagementInfo.BssId, 6) == 0) {
                        //
                        // This Beacon is mine
                        //

                                NdisGetCurrentSystemTime(&CurrentTime);
                                CurrentTime.QuadPart /= 10;

                                //
                                // Determine how many more beacons we can skip.
                                //

                                SkipCount = PowerMgmtMode->usListenInterval;

                                FillOctetString(Packet, pktbuf, pktlen);
                                Tim=PacketGetElement(Packet, EID_Tim);

                                if (Tim.Length != 0){

                                    //
                                    // Sucessfully found TIM element. If there is UC data for us, send PS-Poll.
                                    // If we are to receive DTIM and the DTIM count is 0 and there is MC/BC data, keep
                                    // awake to receive it.
                                    //
                                    if (TimGetAIDBit(Tim, (USHORT)(PowerMgmtMode->usAID & 0x3fff))) {
                                        HwSendPSPoll(Nic);
                                        Nic->ManagementInfo.bUnicastDataAvailable = TRUE;
                                    }
                                    else if (!PowerMgmtMode->bReceiveDTIMs || Tim.Octet[0] != 0 || !TimGetBcMcBit(Tim)) {
                                        bKeepAwake = FALSE;
                                    }

                                    //
                                    // Update SkipCount if we are to receive DTIM.
                                    //

                                    if (PowerMgmtMode->bReceiveDTIMs) {
                                        DTIMCount = Tim.Octet[0] ? Tim.Octet[0] : Tim.Octet[1];
                                        if (SkipCount > DTIMCount)
                                            SkipCount = DTIMCount;
                                        }
                                    }

                                    //
                                    // Calculate the time different between when the beacon is supposed to be transmitted 
                                    // and when it is actually transmitted.
                                    //

                                    BeaconFrame = Add2Ptr(pktbuf, sizeof(DOT11_MGMT_HEADER));
                                    TimeDiff = BeaconFrame->Timestamp % (((ULONGLONG)BeaconFrame->BeaconInterval) << 10);

                                    //
                                    // Calculate the time instant (in micro-second) we would have to wake up for the next beacon.
                                    //
                                    ASSERT(SkipCount > 0);
                                    Nic->ManagementInfo.WakeupTime = CurrentTime.QuadPart - TimeDiff +
                                    (((ULONGLONG)(SkipCount * BeaconFrame->BeaconInterval)) << 10);
                                    }

                                    break;

                 case 0x9:       // ATIM
                                         if (HW_IS_IN_STA_ADHOC_MODE(Nic) && memcmp(pktbuf + 4, Nic->CardInfo.MacAddr, 6) == 0 &&
                                             PowerMgmtMode->dot11PowerMode == dot11_power_mode_powersave){
                                        //
                                        // We have been notified by a station that it has packets 
                                        // buffered for us. We must stay awake.
                                        //
                                         Nic->ManagementInfo.bATIMPacketReceived = TRUE;
                                         }

                                         break;
                                         }

                                         break;

                case 1:             // Control
                                         break;

                case 2:             // Data
                        switch (subtype) {
                            case 0x0:       // Data
                                 if (PowerMgmtMode->dot11PowerMode == dot11_power_mode_powersave && 
                                     HW_IS_IN_STA_INFRA_MODE(Nic)) {
                                     //
                                     // For BC/MC data, we don't send PS-poll. We keep awake if either the "more data" bit is  
                                     // set or we know there is unicast data for us.
                                     //
                                     // For UC data, if the "more data" bit is set, send PS-Poll, otherwise, we will not keep awake.
                                     //

                                         if (HwIsMulticast(pktbuf) || HwIsBroadcast(pktbuf)) {
                                             bKeepAwake = ((pktbuf[1] & 0x20) || Nic->ManagementInfo.bUnicastDataAvailable);
                                         }
                                         else if (memcmp(pktbuf + 4, Nic->CardInfo.MacAddr, 6) == 0) {
                                            //
                                            // More data bit is set
                                            //
                                             if ((pktbuf[1] & 0x20) != 0) {
                                                 HwSendPSPoll(Nic);
                                             }
                                             else{
                                                 Nic->ManagementInfo.bUnicastDataAvailable = FALSE;
                                                 bKeepAwake = FALSE;
                                             }
                                         }
                                   }

                                   break;
                               }

                            break;
                         }

                        if (PowerMgmtMode->dot11PowerMode == dot11_power_mode_powersave && !bKeepAwake) {
                            NdisGetCurrentSystemTime(&CurrentTime);
                            CurrentTime.QuadPart /= 10;

                            if (Nic->ManagementInfo.WakeupTime > (ULONGLONG)CurrentTime.QuadPart)
                                HwDoze(Nic, (ULONG)(Nic->ManagementInfo.WakeupTime - (ULONGLONG)CurrentTime.QuadPart));
                        }
}

__drv_aliasesMem
PMP_RX_FRAGMENT
Hw11GetMpFragment(
                 __in  PNIC_RX_FRAGMENT pNicFragment
                 )
{
    MPVERIFY(pNicFragment);
    
    return pNicFragment->MpFragment;
}

USHORT
Hw11GetFragmentDataSize(
                       __in  PNIC_RX_FRAGMENT pNicFragment
                       )
{
    return pNicFragment->DataSize; 
}

USHORT
Hw11GetFragmentDataOffset(
                         __in  PNIC_RX_FRAGMENT pNicFragment
                         )
{
    return pNicFragment->DataOffset;
}

BOOLEAN
Hw11IsFragmentEncrypted(
                       __in  PNIC_RX_FRAGMENT pNicFragment
                       )
{
    return pNicFragment->Encrypted; 
}

BOOLEAN
Hw11GetFragmentChannel(
                      __in  PNIC_RX_FRAGMENT pNicFragment
                      )
{
    return pNicFragment->Channel; 
}

PVOID
Hw11GetVirtualAddress(
                     __in  PNIC_RX_FRAGMENT pNicFragment
                     ){
    return (PVOID)pNicFragment->RxBuffer;
}

PMDL
Hw11GetMdl(
          __in  PNIC_RX_FRAGMENT pNicFragment
          )
{
    return pNicFragment->Mdl;
}


PVOID
Hw11GetFragmentDataStart(
                        __in  PNIC_RX_FRAGMENT pNicFragment
                        )
{
    return (PVOID)Add2Ptr(Hw11GetVirtualAddress(pNicFragment), Hw11GetFragmentDataOffset(pNicFragment));
}

//
// Called for Data and Management packets on 
//
BOOLEAN
HwIsFragmentInDuplicateTable(
    __in  PNIC                            pNic,
    __in  PUCHAR                          pPacketBuffer
    )
{
    UCHAR                               Index;
    PMP_DOT11_MGMT_DATA_MAC_HEADER      pPacketHeader = (PMP_DOT11_MGMT_DATA_MAC_HEADER)pPacketBuffer;
    UCHAR                               StartIndex, EndIndex;

    //
    // For duplicate detection, we compare the <Address 2, sequence-number, fragment-number> tuple to
    // the last received value. If all match, this is a retransmitted packet that we have already
    // receive indicated and would drop
    //

    if (HwIsMulticast(pPacketBuffer))
    {
        // We dont cache broadcast/multicast packet information
        return FALSE;   
    }

    //
    // A bunch of probe responses come in during scans, etc. This could cause us
    // to indicate up retry data packets. We handle this by separating cached entries for data
    // packets from other packet types
    //
    if (pPacketHeader->FrameControl.Type == DOT11_FRAME_TYPE_DATA)
    {
        StartIndex = 0;
        EndIndex = DUPLICATE_DETECTION_CACHE_LENGTH;
    }
    else
    {
        StartIndex = DUPLICATE_DETECTION_CACHE_LENGTH;
        EndIndex = 2 * DUPLICATE_DETECTION_CACHE_LENGTH;
    }
    

    //
    // The duplicate's cache is indexed using addresses
    //
    for (Index = StartIndex; Index < EndIndex; Index++)
    {
        if (MP_COMPARE_MAC_ADDRESS(pNic->RxInfo.DupePacketCache[Index].Address2, pPacketHeader->Address2))
        {
            if (pPacketHeader->FrameControl.Retry)
            {
                //
                // This is a retry if same sequence control
                //
                if (pNic->RxInfo.DupePacketCache[Index].SequenceControl == pPacketHeader->SequenceControl.usValue)
                {
                    return TRUE;
                }
            }

            //
            // Save latest sequence number
            //
            pNic->RxInfo.DupePacketCache[Index].SequenceControl = pPacketHeader->SequenceControl.usValue;
            return FALSE;
        }
    }

    //
    // Store the tuple information from this packet
    //

    //
    // This address is not cached. We just add it to the next index in the cache
    //
    if (pPacketHeader->FrameControl.Type == DOT11_FRAME_TYPE_DATA)
    {
        Index = ((pNic->RxInfo.NextDupeCacheIndexData + 1) % DUPLICATE_DETECTION_CACHE_LENGTH);
        pNic->RxInfo.NextDupeCacheIndexData = Index;
    }
    else
    {
        Index = ((pNic->RxInfo.NextDupeCacheIndexOther + 1) % DUPLICATE_DETECTION_CACHE_LENGTH)
                    + DUPLICATE_DETECTION_CACHE_LENGTH;
        pNic->RxInfo.NextDupeCacheIndexOther = Index;
    }
    
    NdisMoveMemory(pNic->RxInfo.DupePacketCache[Index].Address2, 
        pPacketHeader->Address2, 
        sizeof(DOT11_MAC_ADDRESS)
        );
    pNic->RxInfo.DupePacketCache[Index].SequenceControl = pPacketHeader->SequenceControl.usValue;

    return FALSE;
}

//
// Returns TRUE if this fragment is duplicate and should be dropped. FALSE otherwise
//
BOOLEAN
HwFragmentIsDuplicate(
    __in  PNIC                            pNic,
    __in  PNIC_RX_FRAGMENT                pNicFragment
    )
{
    PUCHAR                              pPacketBuffer = Add2Ptr(pNicFragment->RxBuffer, pNicFragment->DataOffset);
    PDOT11_MAC_HEADER                   pPacketHeader = (PDOT11_MAC_HEADER)pPacketBuffer;

    //
    // Reject packets which are not correct length
    //
    switch(pPacketHeader->FrameControl.Type)
    {
    case DOT11_FRAME_TYPE_MANAGEMENT:
    case DOT11_FRAME_TYPE_DATA:
        if (pNicFragment->DataSize < sizeof(MP_DOT11_MGMT_DATA_MAC_HEADER))
            return TRUE;    // Drop the packet

        //
        // Additional packet filtering - duplication detection
        //
        if (HwIsFragmentInDuplicateTable(pNic, pPacketBuffer))
        {
            PDOT11_PHY_FRAME_STATISTICS pPhyStats = &pNic->StatisticsInfo.PhyCounters[Hw11QueryOperatingPhyId(pNic)];
            pPhyStats->ullFrameDuplicateCount++;    // Duplicate
        
            return TRUE;    // Drop the packet
        }

        break;
        
    case DOT11_FRAME_TYPE_CONTROL:    
        break;
        
    default:
        //
        // Reserved packet are filtered out
        //
        return TRUE;    // Drop the packet
    }

    // Not duplicate
    return FALSE;
}


/**
 * Informs the hardware of a packet receive. This is invoked in the 
 * context of a receive handler DPC and invoked for every valid receive
 * packet.
 * 
 * \param Nic          NIC structure      
 * \param pNicFragment  NIC_RX_FRAGMENT structure to use when invoking HW functions
 * \param pMpFragment   MP_RX_FRAGMENT structure
 * \return NDIS_STATUS whether the packet is acceptable or not
 */
NDIS_STATUS
Hw11PacketReceived(
                  __in  PNIC                            Nic,
                  __in  PNIC_RX_FRAGMENT                pNicFragment,
                  __in  PMP_RX_FRAGMENT                 pMpFragment
                  )
{
PUCHAR      pktbuf = Add2Ptr(pNicFragment->RxBuffer, pNicFragment->DataOffset);
UCHAR       type = (pktbuf[0] & 0x0f) >> 2;
UCHAR       subtype = (pktbuf[0] & 0xf0) >> 4;
ULONGLONG   timestamp;

UNREFERENCED_PARAMETER(pMpFragment);

    //
    // Reject packets which are not correct length
    //
    switch (type) {
        case DOT11_FRAME_TYPE_MANAGEMENT:
        case DOT11_FRAME_TYPE_DATA:
            if (pNicFragment->DataSize < sizeof(MP_DOT11_MGMT_DATA_MAC_HEADER))
                return NDIS_STATUS_NOT_ACCEPTED;

        //
        // Additional packet filtering - duplication detection
        //
            if (HwFragmentIsDuplicate(Nic, pNicFragment)) {
                PDOT11_PHY_FRAME_STATISTICS pPhyStats = &Nic->StatisticsInfo.PhyCounters[Hw11QueryOperatingPhyId(Nic)];
                pPhyStats->ullFrameDuplicateCount++;    // Duplicate   
                return NDIS_STATUS_NOT_ACCEPTED;
            }

            break;

        case DOT11_FRAME_TYPE_CONTROL:    
            break;

    default:
//
// Reserved packet are filtered
//
        return NDIS_STATUS_NOT_ACCEPTED;
    }


    //
    // During a scan operation, for beacon and probe response frames, check its timestamp. 
    // If it's not out-dated, save the channel number at which it was received. 
    // Otherwise, drop the frame.
    //

    if (Nic->ManagementInfo.bScanInProgress &&
       type == DOT11_FRAME_TYPE_MANAGEMENT &&
       (subtype == DOT11_MGMT_SUBTYPE_BEACON || 
        subtype == DOT11_MGMT_SUBTYPE_PROBE_RESPONSE)) {
#ifdef OLD_TIMESTAMP_WAY
        timestamp = ((ULONGLONG)NdisGetPhysicalAddressLow(pNicFragment->RxDesc.BufferAddress)) |
        (((ULONGLONG)NdisGetPhysicalAddressHigh(pNicFragment->RxDesc.BufferAddress)) << 32);
#endif
        timestamp = ((ULONGLONG)pNicFragment->RxDesc.TSFTL) |
        ((((ULONGLONG)pNicFragment->RxDesc.TSFTH)) << 32);

#ifdef OLD_TIMESTAMP_WAY
        if (timestamp < Nic->ManagementInfo.ScanChannelSwitchTime) {
            if (subtype == 0x8 && type == 0) {
            }
            else{
                ASSERT(FALSE);
            }
            
            return NDIS_STATUS_NOT_ACCEPTED;
        }
#endif    
    
        pNicFragment->Channel = Nic->ManagementInfo.ScanCurrentChannel;
    }

    //
    // Check if we need to send ps-poll (probe is handled 
    // in function HwHandleTimingCriticalPacket() )
    //
    HwResponseToPacket(Nic, pNicFragment);

    //
    // Do any other processing that we may need
    //
    return Nic->RxInfo.ReceiveProcessingFunction(Nic, pNicFragment);
    }                                     


// Default, we dont need any special processing
NDIS_STATUS
HwProcessReceiveSTAMode(
                       __in  PNIC Nic,
                       __in  PNIC_RX_FRAGMENT pNicFragment
                       )
{
    UNREFERENCED_PARAMETER(Nic);
    UNREFERENCED_PARAMETER(pNicFragment);
    
    return NDIS_STATUS_SUCCESS;
}


NDIS_STATUS
HwProcessReceiveSTAModeWaitForJoin(
                                  __in PNIC             Nic,
                                  __in PNIC_RX_FRAGMENT pNicFragment
                                  )
{
PUCHAR      pktbuf = Add2Ptr(pNicFragment->RxBuffer, pNicFragment->DataOffset);
UCHAR       type=(pktbuf[0]  & 0x0f)>>2;
UCHAR       subtype=(pktbuf[0]  & 0xf0)>>4;
BOOLEAN     cancelled;

    if (type == DOT11_FRAME_TYPE_MANAGEMENT     &&
        subtype ==  DOT11_MGMT_SUBTYPE_BEACON    &&
        MP_COMPARE_MAC_ADDRESS(((PDOT11_MGMT_HEADER)pktbuf)->SA, Nic->ManagementInfo.BssId) &&
        HW_STOP_WAITING_FOR_JOIN(Nic) == TRUE) {
        //
        // We have received the beacon that synchronises us with the BSS.
        // We will complete the pending Join/Start request and swap filter
        // handler to a function that does not make this unecessary check
        // We don't care whether the Timer got cancelled or not. The macro
        // HW_STOP_WAITING_FOR_JOIN synchronizes us with the cancel operation
        //
        //NdisMCancelTimer(&Nic->ManagementInfo.Timer_JoinTimeout, &Canceled); vm
         cancelled = WdfTimerStop(Nic->ManagementInfo.Timer_JoinTimeout, FALSE);

        //
        // Set the Associated OK with link in hardware
        //
        HwPlatformEFIOWrite1Byte( Nic, MSR, MSR_INFRA);
        //
        // Set the filtering function to the one optimized for STA mode
        //

        Nic->RxInfo.ReceiveProcessingFunction =  HwProcessReceiveSTAMode;
        Sta11JoinCompleteInfra(Nic->Station, NDIS_STATUS_SUCCESS);
        MpTrace(COMP_RECV, DBG_NORMAL, ("Received beacon that sync us with the BSS cur channel %d\n",
                                         Nic->ManagementInfo.DestChannel ));
    }

    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
HwProcessReceiveAdHocSTAModeWaitForJoin(
                                       __in PNIC             Nic,
                                       __in PNIC_RX_FRAGMENT pNicFragment
                                       )
{   
    //3 Return false if this packet should be dropped
    PUCHAR      pktbuf = Add2Ptr(pNicFragment->RxBuffer, pNicFragment->DataOffset);
    UCHAR       type=(pktbuf[0]  & 0x0f)>>2, tmpMSR;
    UCHAR       subtype=(pktbuf[0]  & 0xf0)>>4;

    if (type == DOT11_FRAME_TYPE_MANAGEMENT     &&
        subtype ==  DOT11_MGMT_SUBTYPE_BEACON    &&
        MP_COMPARE_MAC_ADDRESS(((PDOT11_MGMT_HEADER)pktbuf)->BSSID, Nic->ManagementInfo.BssId) &&
        HW_STOP_WAITING_FOR_JOIN(Nic) == TRUE){   
//
// Set the Associated OK with link in hardware
//
        tmpMSR = HwPlatformEFIORead1Byte(Nic,MSR );
        tmpMSR |= MSR_INFRA;      
        HwPlatformEFIOWrite1Byte( Nic, MSR, tmpMSR);
//
// Set the filtering function to the one optimized for STA mode
//
        Nic->RxInfo.ReceiveProcessingFunction =  HwProcessReceiveSTAMode;
        Sta11JoinCompleteAdHoc(Nic->Station, NDIS_STATUS_SUCCESS);
        MpTrace(COMP_RECV, DBG_NORMAL, ("Received beacon that sync us with the BSS\n"));
    }

    return NDIS_STATUS_SUCCESS;
}

//
// Call the hardware layer function to do security checking. This includes
// MIC failure checking and replay counter checking.
//

NDIS_STATUS
Hw11SecurityCheck(
                 __in PNIC                 Nic,
                 __in PNET_BUFFER          NetBuffer,
                 __in PNIC_RX_FRAGMENT    *ppNicFragment,
                 __in ULONG                FragmentCount
                 ){
    PNIC_RX_FRAGMENT    pNicFragment;
    PNICKEY             pNicKey;
    UCHAR               ReceivedMIC[8];
    UCHAR               CalculatedMIC[8];
    NDIS_STATUS         ndisStatus;
    PMDL                mdl;
    ULONG               length;
    ULONG               offset;
    ULONG               index;
    
    UNREFERENCED_PARAMETER(Nic);

    //
    // This function does the MIC failure checking and replay counter checking.
    // If either one is done by hardware, its respective portion can be skipped.
    //

    //
    // Get the key that decrypted this MSDU. If we did not decrypted this MSDU, as
    // indicated by a NULL key, simply return success.
    //
    ASSERT(FragmentCount >= 1);
    pNicFragment = ppNicFragment[0];
    pNicKey = pNicFragment->NicKey;
    
    if (!pNicKey){
        if (pNicFragment->Unicast){
            Nic->StatisticsInfo.UcastCounters.ullReceivedFrameCount++;
        }
        else{
            Nic->StatisticsInfo.McastCounters.ullReceivedFrameCount++;
        }
        return NDIS_STATUS_SUCCESS;
    }

    if (pNicKey->AlgoId == DOT11_CIPHER_ALGO_TKIP) {
        //
        // If the MSDU was decrypted by TKIP, check MIC and replay counter.
        // First, get the MIC field. Note that MIC could span across two MDLs.
        // All MDLs on the NET_BUFFER must have been mapped to some virtual addresses.
        //
        mdl = NET_BUFFER_CURRENT_MDL(NetBuffer);
        length = NET_BUFFER_DATA_LENGTH(NetBuffer);
        offset = NET_BUFFER_CURRENT_MDL_OFFSET(NetBuffer);
        ASSERT(length > sizeof(ReceivedMIC));
        ASSERT(mdl->ByteCount >= offset);

        //
        // Find the MDL that that contains (at least the first byte of) MIC.
        //

        while (length - (mdl->ByteCount - offset) >= sizeof(ReceivedMIC)){
            length -= mdl->ByteCount - offset;
            mdl = mdl->Next;
            ASSERT(mdl);
            offset = 0;
        }

        //
        // Found that MDL. Calculate the offset of MIC in the MDL. Copy MIC to our buffer.
        //
        offset += length - sizeof(ReceivedMIC);
        for (index = 0; index < sizeof(ReceivedMIC); index++){
            ReceivedMIC[index] = *((PUCHAR)Add2Ptr(mdl->MappedSystemVa, offset));
            offset++;
    
            if (offset >= mdl->ByteCount){
                mdl = mdl->Next;
                offset = 0;
            }
        }

        //
        // Shorten the actual data by size of MIC. Then calculate the MIC based on
        // receiving data and Rx MIC key.
        //
        NET_BUFFER_DATA_LENGTH(NetBuffer) -= sizeof(ReceivedMIC);
        ndisStatus = HwCalculateMIC(NetBuffer, 0, pNicKey->RxMICKey, CalculatedMIC);
        ASSERT(ndisStatus == NDIS_STATUS_SUCCESS);

        //
        // Compare the received MIC vs. calculated MIC. If there is mismatch, indicated
        // MIC failure.
        //
        if (NdisEqualMemory(ReceivedMIC, CalculatedMIC, sizeof(ReceivedMIC)) != 1) {
                mdl = NET_BUFFER_CURRENT_MDL(NetBuffer);
                offset = NET_BUFFER_CURRENT_MDL_OFFSET(NetBuffer);
                index = (ULONG)(pNicKey - Nic->ManagementInfo.KeyTable);       // key index
        
                Sta11IndicateMICFailure(Nic->Station, 
                                        Add2Ptr(mdl->MappedSystemVa, offset),
                                        (index < DOT11_MAX_NUM_DEFAULT_KEY) ?  index : 0,
                                        (BOOLEAN)(index < DOT11_MAX_NUM_DEFAULT_KEY));
        
                MpTrace(COMP_RECV, DBG_SERIOUS, ("MIC failure detected!\n"));
        
                if (pNicFragment->Unicast) {
                    Nic->StatisticsInfo.UcastCounters.ullTKIPLocalMICFailures++;
                    Nic->StatisticsInfo.UcastCounters.ullDecryptFailureCount++;
                }
                else {
                    Nic->StatisticsInfo.McastCounters.ullTKIPLocalMICFailures++;
                    Nic->StatisticsInfo.McastCounters.ullDecryptFailureCount++;
                }
                return NDIS_STATUS_NOT_ACCEPTED;
            }
    
           //
           // Replay counter checking for TKIP. This is done after MIC verification. 
           // Two things are checked:
           //   1. The frame number of the first fragment in a MSDU must be greater than
           //      the replay counter for the key that decrypted the MSDU.
           //   2. The frame number of the all fragments in a MSDU must be monotonically
           //      increased, but not necessarily sequential (unlike in CCMP).
           //
           if (pNicFragment->FrameNumber <= pNicKey->ReplayCounter) {
               MpTrace(COMP_RECV, DBG_SERIOUS, ("Replay counter failed!\n"));
               if (pNicFragment->Unicast) {
                   Nic->StatisticsInfo.UcastCounters.ullTKIPReplays++;
                   Nic->StatisticsInfo.UcastCounters.ullDecryptFailureCount++;
               }
               else {
                   Nic->StatisticsInfo.McastCounters.ullTKIPReplays++;
                   Nic->StatisticsInfo.McastCounters.ullDecryptFailureCount++;
               }
               return NDIS_STATUS_NOT_ACCEPTED;
           }
        
           for (index = 0; index < FragmentCount - 1; index++) {
               if (ppNicFragment[index]->FrameNumber >= ppNicFragment[index + 1]->FrameNumber) {
                   MpTrace(COMP_RECV, DBG_SERIOUS, ("Replay counter failed!\n"));
                   if (pNicFragment->Unicast) {
                       Nic->StatisticsInfo.UcastCounters.ullTKIPReplays++;
                       Nic->StatisticsInfo.UcastCounters.ullDecryptFailureCount++;
                   }
                   else {
                       Nic->StatisticsInfo.McastCounters.ullTKIPReplays++;
                       Nic->StatisticsInfo.McastCounters.ullDecryptFailureCount++;
                   }
                   return NDIS_STATUS_NOT_ACCEPTED;
               }
           }
        
          //
          // Passed replay counter check. Update the replay counter in the key to frame number of
          // the last fragment in the MSDU.
          //
          pNicKey->ReplayCounter = ppNicFragment[FragmentCount - 1]->FrameNumber;
      }
      else if (pNicKey->AlgoId == DOT11_CIPHER_ALGO_CCMP) {
             //
             // If the MSDU was decrypted by CCMP, check replay counter.
             // Two things are checked:
             //   1. The frame number of the first fragment in a MSDU must be greater than
             //      the replay counter for the key that decrypted the MSDU.
             //   2. The frame number of the all fragments in a MSDU must be sequential.
             //
             if (pNicFragment->FrameNumber <= pNicKey->ReplayCounter) {
                 MpTrace(COMP_RECV, DBG_SERIOUS, ("Replay counter failed!\n"));
                 if (pNicFragment->Unicast) {
                     Nic->StatisticsInfo.UcastCounters.ullCCMPReplays++;
                     Nic->StatisticsInfo.UcastCounters.ullDecryptFailureCount++;
                 }
                 else {
                     Nic->StatisticsInfo.McastCounters.ullCCMPReplays++;
                     Nic->StatisticsInfo.McastCounters.ullDecryptFailureCount++;
                 }
                 return NDIS_STATUS_NOT_ACCEPTED;
             }
        
             for (index = 0; index < FragmentCount - 1; index++) {
                 if (ppNicFragment[index]->FrameNumber + 1 != ppNicFragment[index + 1]->FrameNumber){
                 MpTrace(COMP_RECV, DBG_SERIOUS, ("Replay counter failed!\n"));
                 if (pNicFragment->Unicast) {
                     Nic->StatisticsInfo.UcastCounters.ullCCMPReplays++;
                     Nic->StatisticsInfo.UcastCounters.ullDecryptFailureCount++;
                 }
                 else {
                     Nic->StatisticsInfo.McastCounters.ullCCMPReplays++;
                     Nic->StatisticsInfo.McastCounters.ullDecryptFailureCount++;
                 }
                 return NDIS_STATUS_NOT_ACCEPTED;
             }
         }
    
         //
         // Passed replay counter check. Update the replay counter in the key to frame number of
         // the last fragment in the MSDU.
         //
         pNicKey->ReplayCounter = ppNicFragment[FragmentCount - 1]->FrameNumber;
    }
 
    if (pNicFragment->Unicast) {
        Nic->StatisticsInfo.UcastCounters.ullDecryptSuccessCount++;
        Nic->StatisticsInfo.UcastCounters.ullReceivedFrameCount++;
    }
    else {
        Nic->StatisticsInfo.McastCounters.ullDecryptSuccessCount++;
        Nic->StatisticsInfo.McastCounters.ullReceivedFrameCount++;
    }

    return NDIS_STATUS_SUCCESS;
}




                                         

