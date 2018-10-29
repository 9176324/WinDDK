/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Hw_Send.c

Abstract:
    Hw layer send processing functions
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
#include "hw_pcomp.h"

#include "hw_main.h"
#include "hw_nic.h"
#include "hw_ccmp.h"
#include "hw_Func.h"
#include "mp_util.h"
#include "usb_main.h"
#include "bus_intf.h"
#include "usb_xmit.h"
#include "8187_gen.h"

#ifdef USE_SAMPLE_PACKET_BUFFERING_IMPLEMENTATION
    #include "Nwf_Mp_PSPacketsManager.h"
#endif

#include "mp_send.h"
#if DOT11_TRACE_ENABLED
    #include "Hw_Send.tmh"
#endif

#pragma warning(disable:4748)
VOID
InitializeTxVariables(
                     PNIC            Nic,
                     __in  ULONG       NumTxd
                     )
/*++

Routine Description

    Few assumptions
    Harware Layer deals with TXMSDU ==> 1 NBL at a time.
    MP layer deals with NBL's
    HW layer deals with NB's and MDL's
    1 USB URB ==> 1 MDL ==> 1 WDFREQUEST
    So number of WDFREQUESTS==> #of NB's * AVg. #of MDL"s per NB
    WE may have a big number here so check for number of requests and if the above # > then what we have then DO calescing
       

Arguments:

    
Return Value:

    
--*/

{
    //UNREFERENCED_PARAMETER(NumTxd);

    Nic->TxInfo.TxTotalDescNum[LOW_QUEUE] = (USHORT)NumTxd;   // LOW_QUEUE
    Nic->TxInfo.TxTotalDescNum[NORMAL_QUEUE] = TOTAL_DESC_NORMAL_QUEUE;   // NORMAL_QUEUE
    Nic->TxInfo.TxTotalDescNum[HIGH_QUEUE] = TOTAL_DESC_HIGH_QUEUE;   // HIGH_QUEUE
    Nic->TxInfo.TxTotalDescNum[BEACON_QUEUE] = TOTAL_DESC_BEACON_QUEUE;    // BEACON_QUEUE    
}

NDIS_STATUS
Hw11InitializeSend(
                  __in  PNIC Nic,
                  __in  ULONG NumTxd
                  )
/*++

Routine Description:
    As apart of initializing the send engine we do the folowing:
    1) initilaize locks, queues, vrriables for send
    2) allocate memory which could be shared memory for PCI devices or
       just regular memory for USb devices.

     

Arguments:


Return Value:


--*/
{                                                    
    int         i, QueueIndex;
    NDIS_STATUS Status=NDIS_STATUS_SUCCESS;

    InitializeTxVariables(Nic, NumTxd);
    HwBusAllocateXmitResources(Nic, NumTxd);  

    __try 
    {

        NdisAllocateSpinLock(&Nic->TxInfo.TxSpinLock);
        InitializeListHead(&Nic->TxInfo.WaitMSDUList);
        NdisAllocateSpinLock(&Nic->TxInfo.WaitListLock);

        //
        // Allocate Beacon Buffer
        //
        for (i = 0; i < Nic->TxInfo.TxTotalDescNum[BEACON_QUEUE]; i++) {
            MP_ALLOCATE_MEMORY(Nic->MiniportAdapterHandle,
                               (PVOID *)&Nic->TxInfo.TxBeaconBufVa[i], 
                               MAX_TX_RX_PACKET_SIZE,
                               HW11_MEMORY_TAG);

            if (Nic->TxInfo.TxBeaconBufVa[i] == NULL) {
                MpTrace(COMP_SEND, DBG_SERIOUS, ("TxBeaconBufVa fail!\n"));
                __leave;
            }
        }
            
        //
        // Allocate Normal/High Queue local Buffer for Transmitting Management/ATIM Packet
        //
        for (QueueIndex=NORMAL_QUEUE; QueueIndex <= HIGH_QUEUE; QueueIndex++) {
            PUCHAR    **va;

            if (QueueIndex == NORMAL_QUEUE) {
                va = &Nic->TxInfo.pTxNormalQBufVa;                
            }
            else {
                va = &Nic->TxInfo.pTxHighQBufVa;
            }
            
            MP_ALLOCATE_MEMORY(Nic->MiniportAdapterHandle,
                               (PVOID *)va, 
                               sizeof(PUCHAR) * Nic->TxInfo.TxTotalDescNum[QueueIndex],
                               HW11_MEMORY_TAG);

            if (*va == NULL) {
                Status = NDIS_STATUS_RESOURCES;
                __leave;
            }
            NdisZeroMemory(*va, sizeof(PUCHAR) * Nic->TxInfo.TxTotalDescNum[QueueIndex]);

            for (i = 0; i < Nic->TxInfo.TxTotalDescNum[QueueIndex]; i++) {
                MP_ALLOCATE_MEMORY(Nic->MiniportAdapterHandle,
                               &((*va)[i]), 
                               MAX_TX_RX_PACKET_SIZE,
                               HW11_MEMORY_TAG);
                
                if ((*va)[i] == NULL) {
                    MpTrace(COMP_SEND, DBG_SERIOUS, ("NdisAllocateMemory fail for TxTotalDescNum!\n"));
                    __leave;
                }

             }

        }
    }
    __finally 
    {
        if (Status != NDIS_STATUS_SUCCESS) {
            //
            // Cleanup
            //
            Hw11TerminateSend(Nic);
        }
    }

    return Status;
}


VOID
Hw11TerminateSend(
                 __in  PNIC Nic
                 )
{

    int i, QueueIndex;
    PUCHAR    **va;
//    PNDIS_PHYSICAL_ADDRESS *pa;

    HwBusFreeXmitResources(Nic);

    //
    // Free Normal/High queue allocated buffers
    //
    for (QueueIndex = NORMAL_QUEUE; QueueIndex <= HIGH_QUEUE; QueueIndex++) {
        if (QueueIndex == NORMAL_QUEUE) {
            va = &Nic->TxInfo.pTxNormalQBufVa;
        }
        else {
            va = &Nic->TxInfo.pTxHighQBufVa;
         }

        if (*va != NULL) {
            //
            // Free Normal/High Queue Buffer
            //
            for (i = 0; i < Nic->TxInfo.TxTotalDescNum[QueueIndex]; i++) {
                if ((*va)[i] != NULL) {
                    MP_FREE_MEMORY((void *)((*va)[i]));
                    (*va)[i] = NULL;
                }
            }

            MP_FREE_MEMORY((PVOID)(*va));
        }
    }

    //
    // Free Beacon Buffer
    //
    for (i = 0; i < Nic->TxInfo.TxTotalDescNum[BEACON_QUEUE]; i++) {
        if (Nic->TxInfo.TxBeaconBufVa[i] != NULL) {
            MP_FREE_MEMORY((void *)Nic->TxInfo.TxBeaconBufVa[i]);
            Nic->TxInfo.TxBeaconBufVa[i] = NULL;
        }
    }

    NdisFreeSpinLock(&Nic->TxInfo.WaitListLock);
    NdisFreeSpinLock(&Nic->TxInfo.TxSpinLock);

}

NDIS_STATUS
Hw11AssociateTxMSDU(
                   __in  PNIC            Nic,
                   __in  PMP_TX_MSDU     pMpTxd,
                   __out PNIC_TX_MSDU*   ppNicTxd
                   )
{
    //NDIS_STATUS  ndisStatus;
    UNREFERENCED_PARAMETER(Nic);

    //
    // Allocate NIC_TX_MSDU and initialize it
    //
    MP_ALLOCATE_MEMORY(Nic->MiniportAdapterHandle,
                   ppNicTxd, 
                   sizeof(NIC_TX_MSDU),
                   HW11_MEMORY_TAG);
    
    if (*ppNicTxd == NULL) {
        MpTrace(COMP_SEND, DBG_SERIOUS, ("NdisAllocateMemory fail for NIC_TX_MSDU!\n"));
        return NDIS_STATUS_RESOURCES;

    }
    NdisZeroMemory(*ppNicTxd, sizeof(NIC_TX_MSDU));
    (*ppNicTxd)->pMpTxd = pMpTxd;
    //
    // Allocate Coalesce buffer
    //
    MP_ALLOCATE_MEMORY(Nic->MiniportAdapterHandle,
                   &((*ppNicTxd)->CoalesceBuf), 
                   MAX_TX_RX_PACKET_SIZE,
                   HW11_MEMORY_TAG);

    if ((*ppNicTxd)->CoalesceBuf == NULL) {
        return NDIS_STATUS_RESOURCES;
    }

    InitializeListHead(&(*ppNicTxd)->WaitLink);
    return NDIS_STATUS_SUCCESS;
}


VOID
Hw11DisassociateTxMSDU(
                      __in  PNIC         Nic,
                      __in  PNIC_TX_MSDU pNicTxd
                      )
{
    UNREFERENCED_PARAMETER(Nic);

    MP_FREE_MEMORY((pNicTxd)->CoalesceBuf);

    //
    // Free NIC_TX_MSDU
    //
   
    MP_FREE_MEMORY(pNicTxd);
}


NDIS_STATUS
HwCopyNBLToBuffer(
                 __in  PNET_BUFFER_LIST    NetBufferList,
                 __out PUCHAR              Dest,
                 __out PULONG              BytesCopied
                 )
/*++

Routine Description:
    This function is called to coalesce data into the buffer specified.

     

Arguments:


Return Value:


--*/ 
{
    NDIS_STATUS     ndisStatus = NDIS_STATUS_SUCCESS;
    PNET_BUFFER     CurrentNetBuffer;
    PMDL            CurrentMdl;
    LONG            CurrLength;
    PUCHAR          pSrc;
    PUCHAR          pDest;

    //
    //leave room for the Tx Desc
    //
    pDest = Dest + RTL8187_HWDESC_HEADER_LEN;
    __try
    {
        *BytesCopied = 0;
        for (CurrentNetBuffer = NET_BUFFER_LIST_FIRST_NB(NetBufferList);
            CurrentNetBuffer != NULL;
            CurrentNetBuffer = NET_BUFFER_NEXT_NB(CurrentNetBuffer)) {
            CurrentMdl = NET_BUFFER_CURRENT_MDL(CurrentNetBuffer);

            pSrc = MmGetSystemAddressForMdlSafe(CurrentMdl, NormalPagePriority);
            if (!pSrc) {
                ndisStatus = NDIS_STATUS_RESOURCES;
                __leave;
            }

            //
            // For the first MDL with data, we need to skip the free space
            //
            pSrc += NET_BUFFER_CURRENT_MDL_OFFSET(CurrentNetBuffer);
            CurrLength = MmGetMdlByteCount(CurrentMdl) - NET_BUFFER_CURRENT_MDL_OFFSET(CurrentNetBuffer);
            if (CurrLength > 0) 
            {
                if (CurrLength + *BytesCopied > MP_802_11_MAX_FRAME_SIZE)
                {
                    MpTrace(COMP_SEND, DBG_SERIOUS, ("%s: The total MSDU size (%d) is greater than max "
                        "allowed (%d)\n", __FUNCTION__, CurrLength + *BytesCopied, MP_802_11_MAX_FRAME_SIZE));
                    
                    ndisStatus = NDIS_STATUS_INVALID_LENGTH;
                    break;  /* break out for loop */
                }
                
                //
                // Copy the data.
                // 
                NdisMoveMemory(pDest, pSrc, CurrLength);
                *BytesCopied += CurrLength;
                pDest += CurrLength;
            }

            CurrentMdl = NDIS_MDL_LINKAGE(CurrentMdl);
            while (CurrentMdl) {
                pSrc = MmGetSystemAddressForMdlSafe(CurrentMdl, NormalPagePriority);
                
                if (!pSrc) {
                    ndisStatus = NDIS_STATUS_RESOURCES;
                    __leave;
                }

                CurrLength = MmGetMdlByteCount(CurrentMdl);
                
                if (CurrLength > 0)
                {
                    if (CurrLength + *BytesCopied > MP_802_11_MAX_FRAME_SIZE)
                    {
                        MpTrace(COMP_SEND, DBG_SERIOUS, ("%s: The total MSDU size (%d) is greater than max "
                            "allowed (%d)\n", __FUNCTION__, CurrLength + *BytesCopied, MP_802_11_MAX_FRAME_SIZE));
                        
                        ndisStatus = NDIS_STATUS_INVALID_LENGTH;
                        break;  /* break out while and for loop */
                    }

                    //
                    // Copy the data. 
                    //
                    NdisMoveMemory((PUCHAR)pDest, pSrc, CurrLength);
                    *BytesCopied += CurrLength;
                    pDest += CurrLength;                   
                }

                CurrentMdl = NDIS_MDL_LINKAGE(CurrentMdl);
            }
        }
    }
    __finally 
    {
    }

    return ndisStatus;
}

/*
    if UseProtection bit is set in beacon, send out a CTS to self  
    at 11mbps because other NonERP-STA cannot see the packets sent
    at high data rate.
*/
BOOLEAN
HwEnableSendCTSToSelf(
    __in  PNIC                    pNic,
    __in  USHORT                  currentDataRate,
    __out PUSHORT                 pRTSCTSRate
    )
{
    BOOLEAN     bCTSToSelf = FALSE;

    ASSERT (pRTSCTSRate != NULL);
    
    if (pNic->ManagementInfo.bEnableSendCTSToSelf == TRUE &&
        pNic->ManagementInfo.OperatingPhyMIB->PhyType == dot11_phy_type_erp &&
        currentDataRate > 22)       // if the data rate is at 11mbps or less, there is no need to send CTS to self
    {
        bCTSToSelf = TRUE;
        *pRTSCTSRate = 22;      // 11mbps
    }
    else
    {
        *pRTSCTSRate = currentDataRate;
    }

    return bCTSToSelf;
}

NDIS_STATUS
HwTransmitPacketCoalesce(
                        __in PNIC         Nic,
                        __in PMP_TX_MSDU  pMpTxd,
                        __in USHORT       QueueID,
                        __in USHORT       NumTxDescNeeded
                        )
/*++

Routine Description:       

Arguments:


Return Value:


--*/
{
    BOOLEAN                 bShortPreamble;
    USHORT                  i, Rate;
    ULONG                   uBytesCopied = 0, DataOffset = 0;
    PNIC_TX_MSDU            NicTxd;
    //PTX_DESC_8187                Desc;
    PNET_BUFFER             CurrentNetBuffer;
    NDIS_STATUS             ndisStatus;
    BOOLEAN                 bCTSEnable;  
    MP_DOT11_MGMT_DATA_MAC_HEADER  FragmentHeader;
    PUCHAR                  TxBuf;
    BOOLEAN                 bCTSToSelf = FALSE;
    USHORT                  RTSCTSRate = 0;

    ndisStatus = NDIS_STATUS_SUCCESS;
    NicTxd = pMpTxd->NicTxd;
    
    bShortPreamble = (BOOLEAN)((Nic->ManagementInfo.CapInfo & DOT11_CAPABILITY_SHORT_PREAMBLE) && 
                               (Nic->ManagementInfo.OperatingPhyMIB->PhyType == dot11_phy_type_erp));
    Rate = Nic->ManagementInfo.CurrentTXDataRate;

    bCTSToSelf = HwEnableSendCTSToSelf (Nic, Rate, &RTSCTSRate);

    //
    // Coalesce all the data in the coalesce buffer
    //
    ndisStatus = HwCopyNBLToBuffer(pMpTxd->NetBufferList,
                                   NicTxd->CoalesceBuf,
                                   &uBytesCopied);
    if (ndisStatus != NDIS_STATUS_SUCCESS) {
        //
        // Failed to coalesce this packet. We will fail the Tx MSDU
        //
        MpTrace(COMP_SEND, DBG_SERIOUS, ("First Desc: %d\n", Nic->TxInfo.TxNextDescToSend[QueueID]));
        return ndisStatus;
    }

    NicTxd->QueueIDUsed = QueueID;
    NicTxd->FirstDescUsed = Nic->TxInfo.TxNextDescToSend[QueueID];
    NicTxd->TotalDescUsed = NumTxDescNeeded;
    MpTrace(COMP_TESTING, DBG_LOUD, ("First Desc: %d\n", Nic->TxInfo.TxNextDescToSend[QueueID]));

    //
    // Fill a TX_DESC for each scatter gather element for each MPDU
    //
    CurrentNetBuffer = NET_BUFFER_LIST_FIRST_NB(pMpTxd->NetBufferList);

    if (NumTxDescNeeded > 1) {
        //
        // We will start from the 2nd MPDU (if present) and will fill in the
        // first one when we are done with all the others. 2nd MPDU means 2nd NB
        // We will also need to skip the data offset (free space) and data length
        // of the first NetBuffer as well.
        //
        DataOffset = NET_BUFFER_DATA_LENGTH(CurrentNetBuffer);
        CurrentNetBuffer = NET_BUFFER_NEXT_NB(CurrentNetBuffer);
        MPVERIFY(CurrentNetBuffer);
    }
    else {
        //
        // There is only one MPDU so only one TX_DESC needed
        // We will not enter the loop and directly fill the
        // TX_DESC for the first NetBuffer
        //
        MPVERIFY(NumTxDescNeeded == 1);
    }
    //
    // If we do coalescing which we will need to do here.    NumTxDescNeeded shoudl be always 1. 
    // The loop starts at 1 and ends at 1 so we wouldn't get into it for the coalesce case.
    //
    for (i = 1; i < NumTxDescNeeded; i++) {

        bCTSEnable = NicTxd->CTSEnabled;
        if ((BOOLEAN)( HwIsMulticast(NicTxd->CoalesceBuf + DataOffset) ||
                                   HwIsBroadcast(NicTxd->CoalesceBuf + DataOffset) ) ) {
            bCTSEnable = FALSE; 
            NicTxd->RTSEnabled = FALSE;          
        }

        //
        // This is a new MPDU (802.11 fragment) to be sent out
        // Get the first MDL that has the 802.11 header present in it.
        //
        ndisStatus = Dot11GetMacHeaderFromNB(CurrentNetBuffer, (PDOT11_MAC_HEADER *) &FragmentHeader);
        if (ndisStatus != NDIS_STATUS_SUCCESS) {
            break;
        }

        // if bCTSToSelf is true, set bCTSEnable and bRTSEnable to true
        TxFillDescriptor8187(Nic,                        
                             (PTX_DESC_8187 )NicTxd->CoalesceBuf+ DataOffset,
                             (UCHAR)QueueID,
                             NicTxd->CoalesceBuf + DataOffset,
                             TRUE,  // Start Of MPDU
                             TRUE,  // End of MPDU if thsi MPDU has both teh start and end Fragments
                             NicTxd->CoalesceBuf + DataOffset,//pTcb->BufferList[i].VirtualAddress,
                             NET_BUFFER_DATA_LENGTH(CurrentNetBuffer),//pTcb->BufferList[i].Length,
                             NET_BUFFER_DATA_LENGTH(CurrentNetBuffer),//pTcb->FragLength[FragIndex],
                             Rate,
                             HwIsMulticast((PUCHAR)NicTxd->CoalesceBuf + DataOffset), //bMulticast
                             bShortPreamble,
                             (BOOLEAN)(i != NumTxDescNeeded - 1),//FragIndex!=(pTcb->FragCount-1),   //bMoreFrag
                             (bCTSToSelf || bCTSEnable),//bCTSEnable,
                             (bCTSToSelf || NicTxd->RTSEnabled),//bRTSEnable,
                             FragmentHeader.DurationID,   //rtsdur
                             RTSCTSRate,    //pTcb->DataRate, //RTS rate
                             FALSE, 
                             HwTxRetryLimit(Nic, Rate),
                             NicTxd
                            );  //bSetOwnBit

        ndisStatus = HwBusSendPacketAsync(Nic, 
                                           (UCHAR)QueueID, 
                                          NET_BUFFER_DATA_LENGTH(CurrentNetBuffer), 
                                          NicTxd->CoalesceBuf + DataOffset,
                                          NicTxd);

        //
        // One more TX_DESC used up.
        // Move to the next MPDU
        //
        DataOffset += NET_BUFFER_DATA_LENGTH(CurrentNetBuffer);
        CurrentNetBuffer = NET_BUFFER_NEXT_NB(CurrentNetBuffer);        
    }

    //
    // Now fill out TX_DESC for the first MPDU
    //
    CurrentNetBuffer = NET_BUFFER_LIST_FIRST_NB(pMpTxd->NetBufferList);


    //
    // This is a new MPDU (802.11 fragment) to be sent out
    // Get the first MDL that has the 802.11 header present in it.
    //
    ndisStatus = Dot11GetMacHeaderFromNB(CurrentNetBuffer, (PDOT11_MAC_HEADER *) &FragmentHeader);
    if (ndisStatus != NDIS_STATUS_SUCCESS) {
        return ndisStatus;
    }
    TxBuf =   HwGetHWDESCAdjustedBuffer(NicTxd->CoalesceBuf);
    bCTSEnable = NicTxd->CTSEnabled;
    if ((BOOLEAN)(HwIsMulticast(TxBuf ) ||
                             HwIsBroadcast(TxBuf))) {
        bCTSEnable = FALSE; 
        NicTxd->RTSEnabled = FALSE;          
    }

    /*bCTSEnable = (!(BOOLEAN)(HwIsMulticast(TxBuf ) ||
                             HwIsBroadcast(TxBuf)));
     */
    TxFillDescriptor8187(Nic,
                         (PTX_DESC_8187 )NicTxd->CoalesceBuf,
                         (UCHAR)QueueID,
                         TxBuf,
                         TRUE,  // Start Of MPDU
                         TRUE,  // End of MPDU
                         TxBuf ,//pTcb->BufferList[i].VirtualAddress,
                         uBytesCopied,
                         uBytesCopied,
                         Rate,
                         HwIsMulticast(TxBuf),    
                         bShortPreamble,
                         (BOOLEAN)(NumTxDescNeeded > 1),    //bMoreFrag
                         (bCTSToSelf || bCTSEnable),//bCTSEnable,
                         (bCTSToSelf || NicTxd->RTSEnabled),//bRTSEnable,
                         FragmentHeader.DurationID,
                         RTSCTSRate,    //RTSRate,
                         TRUE,//bEncrypt 
                         HwTxRetryLimit(Nic, Rate),
                         NicTxd
                        );  

    ASSERT((uBytesCopied + RTL8187_HWDESC_HEADER_LEN) <= MAX_TX_RX_PACKET_SIZE);
    ndisStatus = HwBusSendPacketAsync(Nic,
                         (UCHAR)QueueID, 
                         uBytesCopied + RTL8187_HWDESC_HEADER_LEN, 
                         NicTxd->CoalesceBuf,
                         NicTxd );
    if (ndisStatus != NDIS_STATUS_SUCCESS) {
        MpTrace(COMP_SEND, DBG_SERIOUS, ("Send packet async returned failure 0x%8x\n", ndisStatus));
    }

    HwPostSendHandler(Nic, uBytesCopied + RTL8187_HWDESC_HEADER_LEN);
    return ndisStatus;
}


ULONG
HwCountTxResourceNeeded(
                       __in  PMP_TX_MSDU         pMpTxd,
                       __out BOOLEAN             *UseCoalesce
                       )
{
    UNREFERENCED_PARAMETER(pMpTxd);
    
    ASSERT(UseCoalesce != NULL);
    ASSERT(pMpTxd->NetBufferList != NULL);
    
    //
    // For USB, always coalesce data into one coalesce buffer
    //
    *UseCoalesce = TRUE;
    return 1;
}


VOID
HwTransmitPacket(
                __in  PNIC   Nic,
                __in  PMP_TX_MSDU pMpTxd,
                __in  USHORT QueueID,
                __in  USHORT NumTxDescNeeded
                )

/*++

Routine Description:       
        in this case we ca optmize if there is 1 MDL 
    If there is only 1 mdl in a NB then we 
    use Retreat operation o the Netbuffer to add space for the Hw Desc.
    How to store that we did a retreattod the advance later?
     
   ASSERT(1 mdl == 1 numDescreqd);
   NDIS_STATUS
  NdisRetreatNetBufferDataStart(
    NetBuffer,
    sizeof(TX_DESC_8187), //__in ULONG  DataOffsetDelta,
    0,
    NULL
    ); 


Arguments:


Return Value:


--*/
{
    BOOLEAN                 bShortPreamble, bFirstSeg;
    USHORT                  i, j, Rate, NumTxDescUsed = 0;
    ULONG                   MdlDataOffset;
    PNIC_TX_MSDU            NicTxd;
    PTX_DESC_8187                Desc/*, FirstDesc*/;
    PNET_BUFFER             CurrentNetBuffer;
    PUCHAR                  header = NULL;
    ULONG                   length;
    PMDL                    currentMdl;
    ULONG_PTR               currentMdlLength;
    PUCHAR                   currentMdlVa;
    BOOLEAN                 bCTSEnable;
    MP_DOT11_MGMT_DATA_MAC_HEADER  FragmentHeader;
    NDIS_STATUS             ndisStatus;
    USHORT                  RTSCTSRate = 0;
    BOOLEAN                 bCTSToSelf = FALSE;

    MPVERIFY(QueueID == LOW_QUEUE);
    NicTxd = pMpTxd->NicTxd;

    //
    // Information that applies to all fragments in the NBL
    //
    NicTxd->QueueIDUsed=QueueID;

    bShortPreamble = (BOOLEAN)((Nic->ManagementInfo.CapInfo & DOT11_CAPABILITY_SHORT_PREAMBLE) && 
                               (Nic->ManagementInfo.OperatingPhyMIB->PhyType == dot11_phy_type_erp));
    Rate = Nic->ManagementInfo.CurrentTXDataRate;

    bCTSToSelf = HwEnableSendCTSToSelf (Nic, Rate, &RTSCTSRate);

    NicTxd->FirstDescUsed = Nic->TxInfo.TxNextDescToSend[QueueID];

    //
    // Fill a TX_DESC for each scatter gather element for each MPDU
    //
    CurrentNetBuffer = NET_BUFFER_LIST_FIRST_NB(pMpTxd->NetBufferList);
    for (i = 0; i < pMpTxd->NetBufferCount; i++) {


        MdlDataOffset = NET_BUFFER_CURRENT_MDL_OFFSET(CurrentNetBuffer);
        bFirstSeg = TRUE;
        currentMdl = NET_BUFFER_FIRST_MDL(CurrentNetBuffer);
        NdisQueryBufferSafe(currentMdl,
                            &header,
                            &length,
                            NormalPagePriority);
        if (!header)
            return;

        header += MdlDataOffset;
        for (j =0 ; currentMdl != NULL ; j++,  currentMdl = NDIS_MDL_LINKAGE(currentMdl) ) {
            currentMdlLength = MmGetMdlByteCount(currentMdl);
            currentMdlVa = MmGetSystemAddressForMdlSafe(currentMdl, NormalPagePriority);
            if (currentMdlVa != NULL)
            {
                // Get the next descriptor we will be filling
                Desc = HwGetNextDescToSend(Nic, QueueID, NumTxDescUsed);

                //
                // One more TX_DESC used up
                //
                NumTxDescUsed++;
                 //
                 // This is a new MPDU (802.11 fragment) to be sent out
                 // Get the first MDL that has the 802.11 header present in it.
                 //
                ndisStatus = Dot11GetMacHeaderFromNB(CurrentNetBuffer, (PDOT11_MAC_HEADER *) &FragmentHeader);
                if (ndisStatus != NDIS_STATUS_SUCCESS) {
                    return ;
                }

                bCTSEnable = NicTxd->CTSEnabled;
                if ((BOOLEAN)( HwIsMulticast(NicTxd->CoalesceBuf ) ||
                                           HwIsBroadcast(NicTxd->CoalesceBuf ) ) ) {
                    bCTSEnable = FALSE; 
                    NicTxd->RTSEnabled = FALSE;          
                }                

                TxFillDescriptor8187(Nic,
                                     Desc,
                                     (UCHAR)QueueID,
                                     NicTxd->CoalesceBuf,
                                     bFirstSeg,  // Start Of MPDU
                                     TRUE,  // End of MPDU
                                     currentMdlVa,//pTcb->BufferList[i].VirtualAddress,
                                     currentMdlLength - MdlDataOffset,//pTcb->BufferList[i].Length,
                                     NET_BUFFER_DATA_LENGTH(CurrentNetBuffer),//pTcb->FragLength[FragIndex],
                                     Rate,
                                     ((bFirstSeg == FALSE) || (bFirstSeg && NicTxd->bMulticastDestAddr)), //bMulticast
                                     bShortPreamble,
                                     (BOOLEAN)((i != pMpTxd->NetBufferCount - 1) && (NumTxDescUsed != NumTxDescNeeded)),//FragIndex!=(pTcb->FragCount-1),   //bMoreFrag
                                     (bCTSToSelf || bCTSEnable),//bCTSEnable,
                                     (bCTSToSelf || NicTxd->RTSEnabled),//bRTSEnable,
                                     FragmentHeader.DurationID,
                                     RTSCTSRate, //RTS rate
                                     TRUE,//bEncrypt 
                                     HwTxRetryLimit(Nic, Rate),
                                     NicTxd
                                    );  //bSetOwnBit

                ndisStatus = HwBusSendPacketAsync(Nic,  
                                     LOW_QUEUE, 
                                     NET_BUFFER_DATA_LENGTH(CurrentNetBuffer), 
                                     NicTxd->CoalesceBuf,
                                     NicTxd ); 
                //
                // We only want to reduce MdlDataOffset from the first SG element
                //
                MdlDataOffset = 0;
                bFirstSeg = FALSE;
            }

        }

        CurrentNetBuffer = NET_BUFFER_NEXT_NB(CurrentNetBuffer);
    }

    MPVERIFY(CurrentNetBuffer == NULL);

    // We need this info for send completion
    NicTxd->TotalDescUsed = NumTxDescUsed;
    MPVERIFY(NumTxDescUsed <= NumTxDescNeeded);

    // We have used up these many descriptors
    HwIncrementNextDescToSend(Nic, QueueID, NumTxDescUsed);
}

BOOLEAN
Hw11CanTransmitNBL(
                  __in  PNIC                       Nic,
                  __in  PNET_BUFFER_LIST           NetBufferList,
                  __in  PDOT11_EXTSTA_SEND_CONTEXT Dot11SendContext
                  )
{
    UNREFERENCED_PARAMETER(Nic);
    UNREFERENCED_PARAMETER(NetBufferList);
    UNREFERENCED_PARAMETER(Dot11SendContext);

    return TRUE;
}


USHORT
GetSequenceNumber(
    __in PNIC        Nic
    )
{
    PNICMANAGEMENTINFO   pMgntInfo    = &Nic->ManagementInfo;
    USHORT      SeqNum;

    SeqNum = pMgntInfo->SequenceNumber;

    if ( pMgntInfo->SequenceNumber >= 0x0fff ) {
        InterlockedExchange((PLONG)&pMgntInfo->SequenceNumber, 0);
    }
    else {
        InterlockedIncrement((PLONG)&pMgntInfo->SequenceNumber);
    }
        
    return SeqNum;
}

NDIS_STATUS
HwPrepareTxMSDUForSend(
                      __in  PNIC Nic,
                      __in  PMP_TX_MSDU pMpTxd
                      )
/*
    Description -- This routine manipulates the MAC header.
*/
{
    NDIS_STATUS                     ndisStatus = NDIS_STATUS_FAILURE;
    PNET_BUFFER                     CurrentNetBuffer;
    PNIC_TX_MSDU                    pNicTxd;
    PMP_DOT11_MGMT_DATA_MAC_HEADER  FragmentHeader;
    BOOLEAN                         bLastFrag, bShortPreamble;
    ULONG                           MSDULength = 0;
    USHORT                          Rate, AckCtsTime, usDuration, usFragNumber = 0;
    USHORT                            sequenceControl;
    PNICKEY                         pNicKey = NULL;
    ULONG                           headerSize;
    PUCHAR                          EncryptionIV;
    UCHAR                           keyId;
    PNIC_ENCRYPTION_INFO            EncryptionInfo = NULL;
    PUCHAR                          MICData = NULL;
    PMDL                            MICMdl = NULL;
    PMDL                            LastMdl = NULL;
    BOOLEAN                         KeyMappingKey = FALSE;
    BOOLEAN                         DoSoftwareEncryption = FALSE;

    pNicTxd = pMpTxd->NicTxd;

    bShortPreamble = (BOOLEAN)((Nic->ManagementInfo.CapInfo & DOT11_CAPABILITY_SHORT_PREAMBLE) && 
                               (Nic->ManagementInfo.OperatingPhyMIB->PhyType == dot11_phy_type_erp));
    Rate = Nic->ManagementInfo.CurrentTXDataRate;

    CurrentNetBuffer = NET_BUFFER_LIST_FIRST_NB(pMpTxd->NetBufferList);

    while (CurrentNetBuffer != NULL) {

        //
        // This is a new MPDU (802.11 fragment) to be sent out
        // Get the first MDL that has the 802.11 header present in it.
        //
        ndisStatus = Dot11GetMacHeaderFromNB(CurrentNetBuffer, (PDOT11_MAC_HEADER *) &FragmentHeader);
        if (ndisStatus != NDIS_STATUS_SUCCESS) {
            break;
        }


        //
        // If hardware encryption is enabled, check if this frame needs to be encrypted.
        // If so, make sure default WEP key is properly set, and set the WEP bit in the frame header.
        //
        // Note that for extensible model, there is only one net buffer in a NBL.
        //

        ASSERT(!NET_BUFFER_NEXT_NB(CurrentNetBuffer));

        if (Nic->ManagementInfo.HwUnicastCipher != DOT11_CIPHER_ALGO_NONE &&
            FragmentHeader->FrameControl.Type == DOT11_FRAME_TYPE_DATA) {

            //
            // Find the key that would be used to encrypt the frame if the frame were to be encrypted
            // For unicast frame, search the matching key in the key mapping table first. If not found, 
            // used default key. For multicast frame, only use the default key.
            //
            if (ETH_IS_UNICAST(FragmentHeader->Address1)) {
                pNicKey = Hw11FindKeyMappingKey(Nic, FragmentHeader->Address1);
            }

            if (pNicKey) {
                KeyMappingKey = TRUE;
            }
            else {
                pNicKey = Nic->ManagementInfo.KeyTable + Nic->ManagementInfo.DefaultKeyID;
                if (!pNicKey->Valid)
                    pNicKey = NULL;
            }

            //
            // Lookup the ExemptionActionType in the send context info of this frame
            // to determine if we need to encrypt the frame.
            //

            switch (pMpTxd->Dot11SendContext->usExemptionActionType) {
                case DOT11_EXEMPT_NO_EXEMPTION:

                    //
                    // We want to encrypt this frame.
                    //

                    FragmentHeader->FrameControl.WEP = 1;
                    break;

                case DOT11_EXEMPT_ALWAYS:

                    //
                    // We don't encrypt this frame.
                    //

                    FragmentHeader->FrameControl.WEP = 0;
                    break;

                case DOT11_EXEMPT_ON_KEY_MAPPING_KEY_UNAVAILABLE:

                    //
                    // We encrypt this frame if and only if a key mapping key is set.
                    //

                    FragmentHeader->FrameControl.WEP = (KeyMappingKey ? 1 : 0);
                    break;

                default:
                    ASSERT(0);
                    FragmentHeader->FrameControl.WEP = 1;
                    break;
            }

            //
            // If the frame is to be encrypted, but no key is not set, either reject the frame 
            // or clear the WEP bit.
            //

            if (FragmentHeader->FrameControl.WEP && !pNicKey) {
                //
                // If this is a unicast frame or if the BSSPrivacy is on, reject the frame. Otherwise,
                // clear the WEP bit so we will not encrypt the frame.
                //

                if (ETH_IS_UNICAST(FragmentHeader->Address1) || 
                    (Nic->ManagementInfo.CapInfo & DOT11_CAPABILITY_INFO_PRIVACY)) {
                    return NDIS_STATUS_NOT_ACCEPTED;
                }
                else {
                    FragmentHeader->FrameControl.WEP = 0;
                }
            }

            //
            // For RTL8185/87, reserve IV field if hardware needs to do the encryption. 
            //

            if (Nic->CardInfo.HardwareID == HW_ID_8187 && FragmentHeader->FrameControl.WEP) {
                __try
                {
                    //
                    // Allocate an encryption info structure to save our encryption context
                    //
                    EncryptionInfo = NdisAllocateFromNPagedLookasideList(&Nic->ManagementInfo.EncryptionInfoList);
                    if (!EncryptionInfo) {
                        ndisStatus = NDIS_STATUS_RESOURCES;
                        __leave;
                    }

                    EncryptionInfo->RetreatedSize = 0;
                    EncryptionInfo->MICMdlAdded = FALSE;
                    EncryptionInfo->MICData = NULL;
                    EncryptionInfo->LastMdlByteCount = 0;
                    EncryptionInfo->MICMdl = NULL;

                    //
                    // For TKIP, we need to calculate MIC and append it at the end of MSDU.
                    // For WPA2PSK adhoc, we need to do software encryption. Also allocate
                    // a MIC, but don't do MIC calculation.
                    //
                    if (pNicKey->AlgoId == DOT11_CIPHER_ALGO_TKIP ||
                        (pNicKey->AlgoId == DOT11_CIPHER_ALGO_CCMP &&
                         !ETH_IS_UNICAST(FragmentHeader->Address1) &&
                         Nic->ManagementInfo.CurrentBSSType == dot11_BSS_type_independent &&
                         Nic->ManagementInfo.HwAuthAlgorithm == DOT11_AUTH_ALGO_RSNA_PSK)) {

                         ULONG   remainingDataLength = 0, byteCount = 0;

                        //
                        // Allocate a MDL of 8 bytes.
                        //
                        MICData = NdisAllocateFromNPagedLookasideList(&Nic->ManagementInfo.MICDataList);
                        if (!MICData) {
                            ndisStatus = NDIS_STATUS_RESOURCES;
                            __leave;
                        }

                        MICMdl = NdisAllocateMdl(Nic->MiniportAdapterHandle, MICData, 8);
                        if (!MICMdl) {
                            ndisStatus = NDIS_STATUS_RESOURCES;
                            __leave;
                        }

                        //
                        // Calculate MIC
                        //
                        if (pNicKey->AlgoId == DOT11_CIPHER_ALGO_TKIP) {
                            ndisStatus = HwCalculateMIC(CurrentNetBuffer, 0, pNicKey->TxMICKey, MICData);
                            if (ndisStatus != NDIS_STATUS_SUCCESS) {
                                __leave;
                            }
                        }
                        else {
                            DoSoftwareEncryption = TRUE;
                        }

                        //
                        // Chain the new MDL at the end of the NET_BUFFER.
                        //
                        LastMdl = NET_BUFFER_FIRST_MDL(CurrentNetBuffer);
                        remainingDataLength = NET_BUFFER_DATA_OFFSET(CurrentNetBuffer) + NET_BUFFER_DATA_LENGTH(CurrentNetBuffer);
                        byteCount = MmGetMdlByteCount(LastMdl);
                        while(remainingDataLength > byteCount)
                        {
                            remainingDataLength -= byteCount;
                            LastMdl = LastMdl->Next;
                            byteCount = MmGetMdlByteCount(LastMdl);
                        }

                        EncryptionInfo->LastMdlByteCount = byteCount;
                        EncryptionInfo->MICMdl = MICMdl;
                        HW_MDL_BYTECOUNT(LastMdl) = remainingDataLength;
                        MICMdl->Next = LastMdl->Next;

                        LastMdl->Next = MICMdl;
                        EncryptionInfo->MICMdlAdded = TRUE;
                        EncryptionInfo->MICData = MICData;
                        NET_BUFFER_DATA_LENGTH(CurrentNetBuffer) += 8;

                    }

                    //
                    // Find the size of header and IV field.
                    //
                    headerSize = (FragmentHeader->FrameControl.FromDS && FragmentHeader->FrameControl.ToDS) ?
                                 sizeof(DOT11_DATA_LONG_HEADER) : sizeof(DOT11_DATA_SHORT_HEADER);

                    if (pNicKey->AlgoId == DOT11_CIPHER_ALGO_TKIP ||
                        pNicKey->AlgoId == DOT11_CIPHER_ALGO_CCMP) {
                        EncryptionInfo->RetreatedSize = 8;
                    }
                    else {
                        EncryptionInfo->RetreatedSize = 4;
                    }

                    //
                    // IM driver must guarantee 8 available bytes for retreat. WEP IV will use 4 bytes,
                    // CCMP/TKIP IV will use 8 bytes.
                    //
                    ASSERT(NET_BUFFER_DATA_OFFSET(CurrentNetBuffer) >= HW11_REQUIRED_BACKFILL_SIZE);
                    ASSERT(NET_BUFFER_FIRST_MDL(CurrentNetBuffer) == NET_BUFFER_CURRENT_MDL(CurrentNetBuffer));

                    ndisStatus = NdisRetreatNetBufferDataStart(CurrentNetBuffer, 
                                                               EncryptionInfo->RetreatedSize, 
                                                               0, 
                                                               NULL);
                    if (ndisStatus != NDIS_STATUS_SUCCESS) {
                        ndisStatus = NDIS_STATUS_RESOURCES;
                        __leave;
                    }

                    //
                    // Because we have enough space for retreat and we are at the first MDL, the retreated 
                    // portion is still in the first MDL. We can simply move the 802.11 header to make room
                    // for encryption IV.
                    // Note: cannot call NdisMoveMemory since source and destination overlaps.
                    //
                    FragmentHeader = (PMP_DOT11_MGMT_DATA_MAC_HEADER)
                                     (((PCHAR)FragmentHeader) - EncryptionInfo->RetreatedSize);

                    RtlMoveMemory(FragmentHeader, 
                                  ((PCHAR)FragmentHeader) + EncryptionInfo->RetreatedSize, 
                                  headerSize);

                    //
                    // Now initialize the IV field.
                    //
                    EncryptionIV = Add2Ptr(FragmentHeader, headerSize);
                    keyId = (UCHAR)(pNicKey - Nic->ManagementInfo.KeyTable);
                    if (keyId >= DOT11_MAX_NUM_DEFAULT_KEY)
                        keyId = 0;

                    switch (pNicKey->AlgoId) {
                        case DOT11_CIPHER_ALGO_WEP40:
                        case DOT11_CIPHER_ALGO_WEP104:
                            EncryptionIV[0] = (UCHAR)(pNicKey->IV & 0xff);
                            EncryptionIV[1] = (UCHAR)((pNicKey->IV >> 8) & 0xff);
                            EncryptionIV[2] = (UCHAR)((pNicKey->IV >> 16) & 0xff);
                            EncryptionIV[3] = (keyId << 6);

                            pNicKey->IV++;

                            break;

                        case DOT11_CIPHER_ALGO_CCMP:
                            EncryptionIV[0] = (UCHAR)(pNicKey->PN & 0xff);
                            EncryptionIV[1] = (UCHAR)((pNicKey->PN >> 8) & 0xff);
                            EncryptionIV[2] = 0;
                            EncryptionIV[3] = 0x20 | (keyId << 6);
                            EncryptionIV[4] = (UCHAR)((pNicKey->PN >> 16) & 0xff);
                            EncryptionIV[5] = (UCHAR)((pNicKey->PN >> 24) & 0xff);
                            EncryptionIV[6] = (UCHAR)((pNicKey->PN >> 32) & 0xff);
                            EncryptionIV[7] = (UCHAR)((pNicKey->PN >> 40) & 0xff);

                            pNicKey->PN++;

                            if (DoSoftwareEncryption) {
                                ndisStatus = HwEncryptCCMP(pNicKey, CurrentNetBuffer);
                                if (ndisStatus != NDIS_STATUS_SUCCESS)
                                    __leave;
                            }

                            break;

                        case DOT11_CIPHER_ALGO_TKIP:
                            EncryptionIV[0] = (UCHAR)((pNicKey->TSC >> 8) & 0xff);
                            EncryptionIV[1] = (EncryptionIV[0] | 0x20) & 0x7f;
                            EncryptionIV[2] = (UCHAR)(pNicKey->TSC & 0xff);
                            EncryptionIV[3] = 0x20 | (keyId << 6);
                            EncryptionIV[4] = (UCHAR)((pNicKey->TSC >> 16) & 0xff);
                            EncryptionIV[5] = (UCHAR)((pNicKey->TSC >> 24) & 0xff);
                            EncryptionIV[6] = (UCHAR)((pNicKey->TSC >> 32) & 0xff);
                            EncryptionIV[7] = (UCHAR)((pNicKey->TSC >> 40) & 0xff);

                            pNicKey->TSC++;

                            break;

                        case DOT11_CIPHER_ALGO_WEP:     // this should not be there.
                        default:
                            ASSERT(0);
                            break;
                    }

                    //
                    // Use the MiniportReserved[0] to save the encryption info context.
                    //
                    ndisStatus = NDIS_STATUS_SUCCESS;
                    CurrentNetBuffer->MiniportReserved[0] = EncryptionInfo;
                }
                __finally 
                {
                    if (ndisStatus != NDIS_STATUS_SUCCESS) {
                        if (EncryptionInfo) {
                            NdisFreeToNPagedLookasideList(&Nic->ManagementInfo.EncryptionInfoList, EncryptionInfo);
                        }

                        if (LastMdl) {
                            LastMdl->Next = NULL;
                            NET_BUFFER_DATA_LENGTH(CurrentNetBuffer) -= 8;
                        }

                        if (MICMdl) {
                            NdisFreeMdl(MICMdl);
                        }

                        if (MICData) {
                            NdisFreeToNPagedLookasideList(&Nic->ManagementInfo.MICDataList, MICData);
                        }
                    }
                }

                if (ndisStatus != NDIS_STATUS_SUCCESS)
                    return ndisStatus;
            }
        }

        //
        // We need to know if this is the last fragment for some operations
        // We also need to total length of this MSDU
        //
        bLastFrag = (BOOLEAN)(NET_BUFFER_NEXT_NB(CurrentNetBuffer) == NULL);
        MSDULength += NET_BUFFER_DATA_LENGTH(CurrentNetBuffer);

        //
        // Calculate the Duration/ID field for this packet
        //
        if (HwIsMulticast((PUCHAR)FragmentHeader)) {
            // For multicast/broadcast packets no duration field needed
            pNicTxd->bMulticastDestAddr = TRUE;
            usDuration = 0;
        }
        else {
            pNicTxd->bMulticastDestAddr = FALSE;

            // Calculate the duration field value
            AckCtsTime = ComputeTxTime(sAckCtsLng / 8, Rate, FALSE, bShortPreamble);
            if (bLastFrag) {
                usDuration = aSifsTime + AckCtsTime;
            }
            else {
                usDuration = 3 * aSifsTime + 2 * AckCtsTime + 
                             ComputeTxTime((USHORT)(NET_BUFFER_DATA_LENGTH(CurrentNetBuffer) + sCrcLng), 
                                           Rate, 
                                           FALSE, 
                                           bShortPreamble);
            }
        }

        //
        // Set the calculated duration in the packet
        //
        NdisMoveMemory(&FragmentHeader->DurationID, &usDuration, sizeof(USHORT));

        //
        // Set the Sequence control in the packet
        //
        sequenceControl =   ( (GetSequenceNumber(Nic) << 4 ) | usFragNumber ) ;
        NdisMoveMemory(&FragmentHeader->SequenceControl, &sequenceControl, 2);
        usFragNumber = usFragNumber + 1;

        //
        // Determine if we will use RTS to send this MSDU
        //
        if (bLastFrag) {
            
            if ((MSDULength + 4) > Nic->DupCurrentInfo.RTS_Threshold && 
                Nic->DupCurrentInfo.Frag_Threshold > Nic->DupCurrentInfo.RTS_Threshold) {
                    pNicTxd->RTSEnabled = TRUE;
                    pNicTxd->CTSEnabled = FALSE; 
            }
            else {
                pNicTxd->RTSEnabled = FALSE;
                pNicTxd->CTSEnabled = FALSE;
            }

            //
            // Save the Total MSDU length in the NicTxd
            //
            pNicTxd->TotalMSDULength = MSDULength;
            
        }

        //
        // Move to the next MPDU
        //
        CurrentNetBuffer = NET_BUFFER_NEXT_NB(CurrentNetBuffer);
    }

    return ndisStatus;
}

NDIS_STATUS
HwTransmitTxMSDU(
                __in  PNIC Nic,
                __in  PMP_TX_MSDU pMpTxd
                )
/*++

Routine Description:

    Thsi function is called to send queued packets . 
     

Arguments:


Return Value:
      NDIS_STATUS_SUCCESS: The packet was handed to hardware
      NDIS_STATUS_RESOURCES: The packet could not be handed
             to hardware due to lack of resources
     NDIS_STATUS_FAILURE: Send failed due to reasons other
             than low Hw resources



--*/
{
    BOOLEAN         UseCoalesce;
    ULONG           NumTxDescNeeded, NumTxDescAvailable;
    NDIS_STATUS     ndisStatus;
    BOOLEAN         CanTransmit = FALSE;

    NumTxDescNeeded = HwCountTxResourceNeeded(pMpTxd, &UseCoalesce);
    NumTxDescAvailable = HwGetTxFreeDescNum(Nic, LOW_QUEUE);
    CanTransmit = TRUE;
    
    if (CanTransmit) {
        HwAwake(Nic);

        if (pMpTxd->Dot11SendContext->uDelayedSleepValue > 0) {
            //
            // Save DelaySleepValue
            //
            NdisGetCurrentSystemTime(&Nic->ManagementInfo.DelaySleepTimeStamp);
            Nic->ManagementInfo.DelaySleepValue += pMpTxd->Dot11SendContext->uDelayedSleepValue;
        }

        if (UseCoalesce == FALSE) {
            MpTrace(COMP_TESTING, DBG_LOUD, ("Tx Tx MSDU %p. Use Desc: %d\n", pMpTxd, NumTxDescNeeded));
            HwTransmitPacket(Nic, pMpTxd, LOW_QUEUE, (USHORT)NumTxDescNeeded);
            ndisStatus = NDIS_STATUS_SUCCESS;
        }
        else {
            MpTrace(COMP_TESTING, DBG_LOUD, ("Tx Coalesce Tx MSDU %p. Use Desc: %d\n", pMpTxd, NumTxDescNeeded));
            ndisStatus = HwTransmitPacketCoalesce(Nic, pMpTxd, LOW_QUEUE, (USHORT)NumTxDescNeeded);
            if (ndisStatus != NDIS_STATUS_SUCCESS) {
                ndisStatus = NDIS_STATUS_RESOURCES;
            }
        }
    }
    else {
        MpTrace(COMP_TESTING, DBG_LOUD, ("Tx Queued: Needed %d, Available %d\n",
                                         NumTxDescNeeded,
                                         NumTxDescAvailable)
                                        );

        ASSERT(FALSE);
        ndisStatus = NDIS_STATUS_RESOURCES;
    }

    return ndisStatus;
}



VOID
Hw11CleanupNetBufferList(
                        __in  PNIC Nic,
                        __in  PNET_BUFFER_LIST    NetBufferList
                        )
{

    PNET_BUFFER_LIST        CurrentNetBufferList;
    PNET_BUFFER             CurrentNetBuffer;
    PNIC_ENCRYPTION_INFO    EncryptionInfo;
    PMDL                    LastMdl;

    //
    // Cleanup is only applicable for RTL8187
    //
    if (Nic->CardInfo.HardwareID != HW_ID_8187)
        return;

    //
    // For each NET_BUFFER that contains our encryption info, advance the 
    // user data, drop the last MDL, and free the encryption info structure.
    //
    for (CurrentNetBufferList = NetBufferList;
        CurrentNetBufferList != NULL;
        CurrentNetBufferList = NET_BUFFER_LIST_NEXT_NBL(CurrentNetBufferList)) {
        CurrentNetBuffer = NET_BUFFER_LIST_FIRST_NB(CurrentNetBufferList);
        ASSERT(!NET_BUFFER_NEXT_NB(CurrentNetBuffer));

        EncryptionInfo = (PNIC_ENCRYPTION_INFO)CurrentNetBuffer->MiniportReserved[0];

        if (EncryptionInfo) {
            if (EncryptionInfo->RetreatedSize) {
                NdisAdvanceNetBufferDataStart(CurrentNetBuffer, 
                                              EncryptionInfo->RetreatedSize,
                                              FALSE,
                                              NULL);
            }

            if (EncryptionInfo->MICMdlAdded) {
                LastMdl = NET_BUFFER_FIRST_MDL(CurrentNetBuffer);
                ASSERT(LastMdl->Next);
                while (LastMdl->Next != EncryptionInfo->MICMdl) {
                    LastMdl = LastMdl->Next;
                }

                NdisFreeToNPagedLookasideList(&Nic->ManagementInfo.MICDataList, EncryptionInfo->MICData);
                HW_MDL_BYTECOUNT(LastMdl) = EncryptionInfo->LastMdlByteCount;
                LastMdl->Next = EncryptionInfo->MICMdl->Next;
                NdisFreeMdl(EncryptionInfo->MICMdl);
                NET_BUFFER_DATA_LENGTH(CurrentNetBuffer) -= 8;
            }

            CurrentNetBuffer->MiniportReserved[0] = NULL;
            NdisFreeToNPagedLookasideList(&Nic->ManagementInfo.EncryptionInfoList, EncryptionInfo);
        }
    }

}

NDIS_STATUS 
HwTxMSDUSend(
            __in  PNIC Nic,
            __in  PMP_TX_MSDU pMpTxd
            )
{
    NDIS_STATUS    ndisStatus;
    //
    // If we manage send this packet successfully, we will wait for it to complete.
    // Else, we fail it
    //
    ndisStatus = HwTransmitTxMSDU(Nic, pMpTxd);
    switch (ndisStatus) {
        case NDIS_STATUS_SUCCESS:
        {
                pMpTxd->NicTxd->bFailedDuringSend = FALSE;
                break;
        }
        case NDIS_STATUS_RESOURCES:
            {
            //
            // Queue the TX_MSDU so that we can send it later. Send is successful
            //
                MpTrace(COMP_TESTING, DBG_LOUD, ("Queuing send %p\n", pMpTxd));

                NdisAcquireSpinLock(&Nic->TxInfo.WaitListLock);
                InsertTailList(
                              &Nic->TxInfo.WaitMSDUList,
                              &(pMpTxd->NicTxd->WaitLink)
                              );
                NdisReleaseSpinLock(&Nic->TxInfo.WaitListLock);
                NdisInterlockedIncrement(&Nic->TxInfo.WaitMSDUListLength);

                pMpTxd->NicTxd->bFailedDuringSend = FALSE;                    
                break;
            }
        default:
            {
            // Other send failure
                pMpTxd->NicTxd->WaitSendToComplete = FALSE;
                pMpTxd->NicTxd->bFailedDuringSend = TRUE;
            }
    }

    return ndisStatus;
}

NDIS_STATUS
Hw11PrepareTxMSDUForSend(
    __in  PNIC    Nic,
    __in  PMP_TX_MSDU  pMpTxd
    )
{
    NDIS_STATUS     status;
    
    pMpTxd->NicTxd->WaitSendToComplete = TRUE;
    pMpTxd->NicTxd->TxSucceeded = FALSE;
    pMpTxd->NicTxd->bFailedDuringSend = FALSE;
    pMpTxd->NicTxd->SucceedFragCount = 0;

    status = HwPrepareTxMSDUForSend(Nic, pMpTxd);
    if (status != NDIS_STATUS_SUCCESS)
    {
        //
        // We cannot send this packet! Fail it during SendComplete
        //
        MpTrace(COMP_TESTING, DBG_SERIOUS, ("Could not prepare Tx MSDU %p\n", pMpTxd));
        pMpTxd->NicTxd->WaitSendToComplete = FALSE;
        pMpTxd->NicTxd->bFailedDuringSend = TRUE;
    }

    return status;
}

NDIS_STATUS
Hw11TransmitTxMSDU(
                  __in  PNIC Nic,
                  __in  PMP_TX_MSDU pMpTxd,
                  __in  BOOLEAN DispatchLevel
                  )
{
    NDIS_STATUS     status = NDIS_STATUS_SUCCESS;
    UNREFERENCED_PARAMETER(DispatchLevel);
    
    MPVERIFY(pMpTxd->SendFlags & MP_TX_MSDU_IS_IN_SEND);
    status = HwTxMSDUSend(Nic,  pMpTxd);

    return status;
}


BOOLEAN
Hw11CanTransmitATIMPacket(
                         __in  PNIC Nic
                         )
{
    return(BOOLEAN)(HwGetTxFreeDescNum(Nic, HIGH_QUEUE) > 0);
}


VOID
Hw11TransmitATIMPacket(
                      __in  PNIC Nic,
                      __in  PDOT11_MAC_ADDRESS pDestinationMacAddress
                      )
{
    PUCHAR                  TxBuf;
    PUCHAR                  pDesc;
    OctetString             AtimPkt;
    GeneralPacketPartial   *pAtimPartial;

    if (HwGetTxFreeDescNum(Nic, HIGH_QUEUE) > 0) {
        pDesc = HwHighQGetNextToSendBuffer(Nic);
        TxBuf = HwGetHWDESCAdjustedBuffer(pDesc);

        //
        // Setup ATIM Packet
        //
        pAtimPartial = (GeneralPacketPartial *)TxBuf;

        pAtimPartial->FrameCtrl.CharData[0] = Type_Atim;
        pAtimPartial->FrameCtrl.CharData[1] = 0;                                                 
        pAtimPartial->Duration = 0;

        ETH_COPY_NETWORK_ADDRESS(pAtimPartial->Addr1, pDestinationMacAddress);
        ETH_COPY_NETWORK_ADDRESS(pAtimPartial->Addr2, Nic->CardInfo.MacAddr);
        ETH_COPY_NETWORK_ADDRESS(pAtimPartial->Addr3, Nic->ManagementInfo.BssId);
        pAtimPartial->Seq = 0;

        //
        // Size of Probe Response, can not use sizeof(BeaconProbeRspPartial)
        //
        FillOctetString(AtimPkt, TxBuf, 0x24);

        HwTransmitNextPacketWithLocalBuffer(Nic,
                                            HIGH_QUEUE,
                                            AtimPkt.Length,
                                            0x04,
                                            FALSE,
                                            pDesc);
    }
    else{
        MPASSERTMSG("No descr in High queue\n",
                            FALSE);
    }
}

BOOLEAN
Hw11TxMSDUIsComplete(
                    __in  PNIC Nic,
                    __in  PNIC_TX_MSDU pNicTxd
                    )
{
    int                         DescUsed;
    PDOT11_PHY_FRAME_STATISTICS pPhyStats = &Nic->StatisticsInfo.PhyCounters[Hw11QueryOperatingPhyId(Nic)];
    BOOLEAN                     isComplete = TRUE;

    UNREFERENCED_PARAMETER(DescUsed);
    UNREFERENCED_PARAMETER(pPhyStats);

    do
    {
        if (pNicTxd->WaitSendToComplete == TRUE)
        {
            MpTrace(COMP_SEND, DBG_LOUD, ("Hw11TxMSDUIsComplete Not waiting MpTxd: %p\n", pNicTxd->pMpTxd));
            ASSERT(IsListEmpty(&pNicTxd->WaitLink)) ;  
            isComplete = FALSE;
            break;
        }
        
        if (pNicTxd->bFailedDuringSend == TRUE) 
        {
            MpTrace(COMP_SEND, DBG_NORMAL, ("Failed Tx MSDU: %p\n", pNicTxd->pMpTxd));

            pNicTxd->SucceedFragCount = 0;

            //
            // Generic send failure
            //
            if (pNicTxd->bMulticastDestAddr) 
            {
                Nic->StatisticsInfo.McastCounters.ullTransmittedFailureFrameCount++;
            }
            else 
            {
                Nic->StatisticsInfo.UcastCounters.ullTransmittedFailureFrameCount++;
            }
        }
        else
        {
            pNicTxd->SucceedFragCount++;
            
            if (pNicTxd->bMulticastDestAddr)
            {
                pPhyStats->ullMulticastTransmittedFrameCount++;
            }
        }
    } while(FALSE);

    if (isComplete == TRUE)
    {
        if (pNicTxd->bMulticastDestAddr)
        {
            Nic->StatisticsInfo.McastCounters.ullTransmittedFrameCount++;
        }
        else
        {
            Nic->StatisticsInfo.UcastCounters.ullTransmittedFrameCount++;
        }
        pPhyStats->ullTransmittedFrameCount++;
        pPhyStats->ullTransmittedFragmentCount++;
    }
    
    return isComplete;
}

VOID
Hw11ReinitializeTxMSDU(
                      __in  PNIC_TX_MSDU pNicTxd
                      )
{
    UNREFERENCED_PARAMETER(pNicTxd);
}


NDIS_STATUS
Hw11TxMSDUTransmissionStatus(
                            __in  PNIC Nic,
                            __in  PNIC_TX_MSDU pNicTxd
                            )
{
    UNREFERENCED_PARAMETER(Nic);

    if (pNicTxd->bFailedDuringSend)
    {
        // Failed due to low resources, etc...
        return NDIS_STATUS_FAILURE;
    }
    else
    {
        // Either succeeded or did not get an ACK (pNicTxd->TxSucceeded == FALSE), both 
        // translate to success for the OS
        return NDIS_STATUS_SUCCESS;
    }
}

#ifdef USE_SAMPLE_PACKET_BUFFERING_IMPLEMENTATION

VOID
HwCheckForATIMPacketSendCompletion(
                                  __in  PNIC  Nic
                                  )
{
    PTX_STATUS_DESC pDesc;

    //
    // Check High Queue(This should be transparent for upper layer)
    // We are check if an ATIM packet has been send completed
    //

    while (HwGetTxBusyDescNum(Nic, HIGH_QUEUE) > 0) {

        //
        // We should only be placing ATIM packets in high queue today
        // If any other packets are found, investigate why
        //
        MPVERIFY(HW_IS_IN_STA_ADHOC_MODE(Nic));

        pDesc = (PTX_STATUS_DESC)HwGetNextDescToCheck(Nic, HIGH_QUEUE, 0);

        if (!pDesc->OWN) {
            MpATIMPacketSendComplete(Nic->pAdapter,
                                     pDesc->TOK ? NDIS_STATUS_SUCCESS : NDIS_STATUS_FAILURE,
                                     TRUE);
            HwIncrementTxNextDescToCheck(Nic, HIGH_QUEUE, 1);
        }
        else {
            break;
        }
    }
}


#endif  // USE_SAMPLE_PACKET_BUFFERING_IMPLEMENTATION

NDIS_STATUS
Hw11SendMgmtPacket(
                  __in  PNIC            Nic,
                  __in_opt  PSTA_BSS_ENTRY  pBSSEntry,
                  __in  PUCHAR          pMgmtPacket,
                  __in  USHORT          MgmtPacketLength
                  )
{
    PUCHAR              TxBuf;
    PUCHAR                  pDesc;

    PNICKEY             pNicKey = NULL;
    USHORT              ExtraIV = 0;
    PUCHAR              EncryptionIV;

    UNREFERENCED_PARAMETER(pBSSEntry);

    //
    // If hardware WEP encryption is enabled and this frame requires WEP, 
    // check if default WEP key is properly set.
    //

    if (Nic->ManagementInfo.HwUnicastCipher != DOT11_CIPHER_ALGO_NONE && ((PDOT11_FRAME_CTRL)pMgmtPacket)->WEP) {
        pNicKey = Nic->ManagementInfo.KeyTable + Nic->ManagementInfo.DefaultKeyID;
        if (!pNicKey->Valid) {
            return NDIS_STATUS_NOT_ACCEPTED;
        }

        //
        // For RTL8185/87, we need to insert the IV field. 
        //
        if (Nic->CardInfo.HardwareID == HW_ID_8187) {
            ExtraIV = 4;
        }
    }

    //
    // For our hardware, we send management packets on the 
    // NORMAL_QUEUE
    // 
    if (HwGetTxFreeDescNum(Nic, NORMAL_QUEUE) > 0) {
        pDesc = HwNormalQGetNextToSendBuffer(Nic);
        TxBuf = HwGetHWDESCAdjustedBuffer(pDesc);

        NdisMoveMemory(
                      TxBuf,
                      pMgmtPacket,
                      DOT11_MGMT_HEADER_SIZE
                      );

        if (ExtraIV) {
            EncryptionIV = Add2Ptr(TxBuf, DOT11_MGMT_HEADER_SIZE);
            EncryptionIV[0] = (UCHAR)(pNicKey->IV & 0xff);
            EncryptionIV[1] = (UCHAR)((pNicKey->IV >> 8) & 0xff);
            EncryptionIV[2] = (UCHAR)((pNicKey->IV >> 16) & 0xff);
            EncryptionIV[3] = (Nic->ManagementInfo.DefaultKeyID << 6);

            pNicKey->IV++;
        }

        NdisMoveMemory(
                      TxBuf + DOT11_MGMT_HEADER_SIZE + ExtraIV,
                      pMgmtPacket + DOT11_MGMT_HEADER_SIZE,
                      MgmtPacketLength - DOT11_MGMT_HEADER_SIZE
                      );

        MgmtPacketLength = MgmtPacketLength + ExtraIV;


        HwTransmitNextPacketWithLocalBuffer(Nic,
                                            NORMAL_QUEUE,
                                            MgmtPacketLength,
                                            Nic->ManagementInfo.CurrentBeaconRate,
                                            FALSE,
                                            pDesc
                                            );

        return NDIS_STATUS_SUCCESS;
    }
    else {
        return NDIS_STATUS_RESOURCES;
    }
}

VOID
Hw11ProcessPendingSend(
                      __in  PNIC Nic
                      )

/**
 * This function is called from the SendComplete interrupt handler to
 * allow the Hw to send any packets it may have pended due to lack of
 * resources. This function is not serialized against the send handler
 *
 * \param Nic              The NIC context for this miniport
 */

{
    PNIC_TX_MSDU    pNicTxd = NULL;
    PMP_TX_MSDU     pMpTxd;
    PLIST_ENTRY     pPendingSend;
    NDIS_STATUS     ndisStatus;

    while (Nic->TxInfo.WaitMSDUListLength != 0) {
        //
        // We have entries queued that have been SGed but
        // not yet been sent. Lets try to send them
        //
        NdisAcquireSpinLock(&Nic->TxInfo.WaitListLock);

        //
        // We always increment WaitMSDUListLength after insert,
        // so this should not happen
        //
        MPVERIFY(!IsListEmpty(&Nic->TxInfo.WaitMSDUList));
        pPendingSend = RemoveHeadList(&Nic->TxInfo.WaitMSDUList);
        pNicTxd = CONTAINING_RECORD(pPendingSend, NIC_TX_MSDU, WaitLink);
        NdisReleaseSpinLock(&Nic->TxInfo.WaitListLock);
        InitializeListHead(&pNicTxd->WaitLink);
        pMpTxd = pNicTxd->pMpTxd;

        MpTrace(COMP_TESTING, DBG_LOUD, ("Reattempting send %p\n", pMpTxd));

        ndisStatus = HwTransmitTxMSDU(Nic, pMpTxd);
        switch (ndisStatus) {
            case NDIS_STATUS_SUCCESS:
            {
                    pMpTxd->NicTxd->bFailedDuringSend = FALSE;
                    break;
            }
            case NDIS_STATUS_RESOURCES:
                {
                //
                // Requeue this packet at the head and we wont 
                // attempt to send any more
                //
                 NdisAcquireSpinLock(&Nic->TxInfo.WaitListLock);
                 InsertHeadList(
                                  &Nic->TxInfo.WaitMSDUList,
                                  &(pMpTxd->NicTxd->WaitLink)
                                  );
                 NdisReleaseSpinLock(&Nic->TxInfo.WaitListLock);
                 MpTrace(COMP_TESTING, DBG_LOUD, ("Requeuing to send %p\n", pMpTxd));

                 pMpTxd->NicTxd->bFailedDuringSend = FALSE; 
                 break;
                }
            default:
                {
                // Other send failure
                    pMpTxd->NicTxd->WaitSendToComplete = FALSE;
                    pMpTxd->NicTxd->bFailedDuringSend = TRUE;
                    NdisInterlockedDecrement(&Nic->TxInfo.WaitMSDUListLength);

                    // need to send complete this nbl
                    Mp11SendCompleteNBL(Nic->Adapter);
                    break;
                }
        }

        if (ndisStatus == NDIS_STATUS_RESOURCES) {
            //
            // Still not enough resources. Dont send (packet has been
            // requeued)
            //
            break;
        }
        else {
            //
            // To avoid out of order sending, we decrement only after
            // we have submitted this packet to the hardware
            //
            NdisInterlockedDecrement(&Nic->TxInfo.WaitMSDUListLength);
        }
    }
}
VOID
Hw11CheckForProtectionInERP(
    __in  PNIC            pNic,
    __in  PUCHAR          pInfoBlob,
    __in  ULONG           infoBlobLength
    )
{
    NDIS_STATUS     status = NDIS_STATUS_SUCCESS;
    UCHAR           erpIELength = 0;
    PUCHAR          erpIEBuf = NULL;

    status = Dot11GetInfoEle(
                pInfoBlob,
                infoBlobLength,
                DOT11_INFO_ELEMENT_ID_ERP,
                &erpIELength,
                &erpIEBuf
                );

    if (status != NDIS_STATUS_SUCCESS || erpIELength != 1)    // ERP IE length has to be 1
    {
        pNic->ManagementInfo.bEnableSendCTSToSelf = FALSE;
    }
    else
    {
        pNic->ManagementInfo.bEnableSendCTSToSelf = (((DOT11_ERP_IE*)erpIEBuf)->UseProtection == 1);;
    }
}





