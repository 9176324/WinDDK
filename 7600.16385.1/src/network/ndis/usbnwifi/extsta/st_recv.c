/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Sta_Recv.c

Abstract:
    STA layer receive processing functions
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
#include "precomp.h"
#include "st_aplst.h"
#include "st_assoc.h"
#include "st_recv.h"
#include "st_adhoc.h"

#if DOT11_TRACE_ENABLED
#include "Sta_Recv.tmh"
#endif

NDIS_STATUS
StaReceiveMgmtPacket(
    __in  PSTATION                        pStation,
    __in  PMP_RX_FRAGMENT                 pMpFragment,
    __in  PNIC_RX_FRAGMENT                pNicFragment,
    __in  PDOT11_MAC_HEADER               pFragmentHdr,
    __in  USHORT                          FragmentSize
    )
{
    NDIS_STATUS         ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_MGMT_HEADER  pMgmtPktHeader;

    UNREFERENCED_PARAMETER(pFragmentHdr);
    UNREFERENCED_PARAMETER(pMpFragment);
    
    // This is a management packet, reject if size isnt right
    if (FragmentSize < sizeof(DOT11_MGMT_HEADER))
    {
        return NDIS_STATUS_NOT_ACCEPTED;
    }

    pMgmtPktHeader = Hw11GetFragmentDataStart(pNicFragment);
    switch(pMgmtPktHeader->FrameControl.Subtype)
    {
        case DOT11_MGMT_SUBTYPE_BEACON:
        case DOT11_MGMT_SUBTYPE_PROBE_RESPONSE:
            StaReceiveBeacon(
                pStation,
                pNicFragment,
                FragmentSize
                );
            break;

        case DOT11_MGMT_SUBTYPE_AUTHENTICATION:
            StaReceiveAuthentication(
                pStation,
                pNicFragment,
                FragmentSize
                );
            break;

        case DOT11_MGMT_SUBTYPE_ASSOCIATION_RESPONSE:
            StaReceiveAssociationResponse(
                pStation,
                pNicFragment,
                FragmentSize
                );
            break;

        case DOT11_MGMT_SUBTYPE_DEAUTHENTICATION:
            StaReceiveDeauthentication(
                pStation,
                pNicFragment,
                FragmentSize
                );
            break;

        case DOT11_MGMT_SUBTYPE_DISASSOCIATION:
            StaReceiveDisassociation(
                pStation,
                pNicFragment,
                FragmentSize
                );
            break;

        default:
            break;
    }

    return ndisStatus;    
}

NDIS_STATUS
StaReceiveDataPacket(
    __in  PSTATION                        pStation,
    __in  PMP_RX_FRAGMENT                 pMpFragment,
    __in  PNIC_RX_FRAGMENT                pNicFragment,
    __in  PDOT11_MAC_HEADER               pFragmentHdr,
    __in  USHORT                          FragmentSize
    )
{
    NDIS_STATUS                     ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_PRIVACY_EXEMPTION_LIST   PrivacyExemptionList = pStation->Config.PrivacyExemptionList;
    USHORT                          EtherType;
    USHORT                          PacketType;
    PDOT11_PRIVACY_EXEMPTION        PrivacyExemption;
    ULONG                           index;
    BOOLEAN                         isUnicast;
    USHORT                          SequenceNumber;

    UNREFERENCED_PARAMETER(pMpFragment);

    //
    // We don't check the fragmented data frame that is not the first fragment.
    //

    if (FragmentSize < sizeof(DOT11_DATA_SHORT_HEADER))
        {
        return NDIS_STATUS_NOT_ACCEPTED;
    }
    
    NdisMoveMemory(&SequenceNumber, 
                   &((PMP_DOT11_MGMT_DATA_MAC_HEADER)pFragmentHdr)->SequenceControl, 
                   2);
    
    if ((SequenceNumber & 0x0f) != 0)
    {
        return NDIS_STATUS_SUCCESS;
    }
    
    //
    // Data frame must also contain 802.2 LLC and 802.2 SNAP (8 bytes total)
    //

    if (FragmentSize < sizeof(DOT11_DATA_SHORT_HEADER) + sizeof(IEEE_8022_LLC_SNAP))
    {
        return NDIS_STATUS_NOT_ACCEPTED;
    }

    if (Hw11GetSafeModeOption(pStation->pNic) == TRUE)
    {
        // For safe mode, we dont apply the privacy exemption stuff
        return NDIS_STATUS_SUCCESS;
    }
    
    __try 
    {
        //
        // Go through the privacy exemption list to see if we can accept the data frame.
        //
        if (PrivacyExemptionList && PrivacyExemptionList->uNumOfEntries > 0)
        {
            //
            // Find the EtherType and PacketType of the frame
            //

            EtherType = ((PIEEE_8022_LLC_SNAP) Add2Ptr(pFragmentHdr, sizeof(DOT11_DATA_SHORT_HEADER)))->sh_etype;
            isUnicast = (BOOLEAN)ETH_IS_UNICAST(pFragmentHdr->Address1);
            PacketType = isUnicast ? DOT11_EXEMPT_UNICAST : DOT11_EXEMPT_MULTICAST;

            //
            // Check the disposition of the frame.
            //

            PrivacyExemption = PrivacyExemptionList->PrivacyExemptionEntries;
            for (index = 0; index < PrivacyExemptionList->uNumOfEntries; index++, PrivacyExemption++)
            {
                //
                // Skip if EtherType does not match
                //

                if (PrivacyExemption->usEtherType != EtherType)
                {
                    continue;
                }

                //
                // Skip if PacketType does not match
                //

                if (PrivacyExemption->usExemptionPacketType != PacketType &&
                    PrivacyExemption->usExemptionPacketType != DOT11_EXEMPT_BOTH)
                {
                    continue;
                }

                if (PrivacyExemption->usExemptionActionType == DOT11_EXEMPT_ALWAYS)
                {
                    //
                    // In this case, we drop the frame if it was originally
                    // encrypted.
                    //

                    if (Hw11IsFragmentEncrypted(pNicFragment))
                    {
                        ndisStatus = NDIS_STATUS_NOT_ACCEPTED;
                    }

                    __leave;
                }
                else if (PrivacyExemption->usExemptionActionType == DOT11_EXEMPT_ON_KEY_MAPPING_KEY_UNAVAILABLE)
                {
                    //
                    // In this case, we reject the frame if it was originally NOT encrypted but 
                    // we have the key mapping key for this frame.
                    //
                    if (!Hw11IsFragmentEncrypted(pNicFragment) && 
                        isUnicast && 
                        Hw11FindKeyMappingKey(pStation->pNic, ((PDOT11_DATA_SHORT_HEADER)pFragmentHdr)->Address2))
                    {
                        ndisStatus = NDIS_STATUS_NOT_ACCEPTED;
                    }

                    __leave;
                }
                else 
                {
                    //
                    // The privacy exemption does not apply to this frame.
                    //

                    break;
                }
            }
        }

        //
        // If the privacy exemption list does not apply to the frame, check ExcludeUnencrypted.
        // if ExcludeUnencrypted is set and this frame was not oringially an encrypted frame, 
        // dropped it.
        //
        if (pStation->Config.ExcludeUnencrypted && !Hw11IsFragmentEncrypted(pNicFragment))
        {
            ndisStatus = NDIS_STATUS_NOT_ACCEPTED;
            
            if (ETH_IS_MULTICAST(pFragmentHdr->Address1))
            {
                pStation->Statistics.ullMcastWEPExcludedCount++;
            }
            else
            {
                pStation->Statistics.ullUcastWEPExcludedCount++;
            }
        }
    }
    __finally 
    {
    }

    //
    // When we receive a good unicast data to use in ad hoc mode, notify the adhoc module.
    //
    if (ndisStatus == NDIS_STATUS_SUCCESS &&
        MP_COMPARE_MAC_ADDRESS(pFragmentHdr->Address1, Hw11GetMACAddress(pStation->pNic)) &&
        pStation->Config.BSSType == dot11_BSS_type_independent)
    {
        StaAdHocReceiveDirectData(pStation, (PDOT11_DATA_SHORT_HEADER)pFragmentHdr);
    }
        
    return ndisStatus;
}

NDIS_STATUS
Sta11PacketReceived(
    __in  PSTATION                        pStation,
    __in  PMP_RX_FRAGMENT                 pMpFragment,
    __in  PNIC_RX_FRAGMENT                pNicFragment,
    __in  PDOT11_MAC_HEADER               pFragmentHdr,
    __in  USHORT                          FragmentSize
    )
{
    NDIS_STATUS                     ndisStatus = NDIS_STATUS_SUCCESS;

    // Reject too small a packet
    if (FragmentSize < sizeof(DOT11_MAC_HEADER))
    {
        return NDIS_STATUS_NOT_ACCEPTED;
    }
    
    switch (pFragmentHdr->FrameControl.Type)
    {
        case DOT11_FRAME_TYPE_MANAGEMENT:
            //
            // Process management packet
            //
            ndisStatus = StaReceiveMgmtPacket(
                pStation,
                pMpFragment,
                pNicFragment,
                pFragmentHdr,
                FragmentSize
                );
            break;
            
        case DOT11_FRAME_TYPE_DATA:
            //
            // Process data packet
            //
            ndisStatus = StaReceiveDataPacket(
                pStation,
                pMpFragment,
                pNicFragment,
                pFragmentHdr,
                FragmentSize
                );
            break;

        default:
            break;
    }
    
    return ndisStatus;
}


