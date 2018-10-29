/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Sta_misc.c

Abstract:
    STA layer utility functions
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
#include "precomp.h"

#if DOT11_TRACE_ENABLED
#include "Sta_misc.tmh"
#endif

#define PHY_TYPE_BUFFER_SIZE    (sizeof(DOT11_SUPPORTED_PHY_TYPES) + \
                                 sizeof(DOT11_PHY_TYPE) * STA_DESIRED_PHY_MAX_COUNT)

VOID
StaGenerateRandomBSSID(
    __in  DOT11_MAC_ADDRESS MACAddr,
    __out DOT11_MAC_ADDRESS BSSID
    )
{
    ULONGLONG   time;

    //
    // Initialize the random BSSID to be the same as MAC address.
    //

    RtlCopyMemory(BSSID, MACAddr, sizeof(DOT11_MAC_ADDRESS));

    //
    // Get the system time in 10 millisecond.
    //

    NdisGetCurrentSystemTime((PLARGE_INTEGER)&time);
    time /= 100000;

    //
    // Randomize the first 4 bytes of BSSID.
    //

    BSSID[0] ^= (UCHAR)(time & 0xff); 
    BSSID[0] &= ~0x01;              // Turn off multicast bit
    BSSID[0] |= 0x02;               // Turn on local bit
    
    time >>= 8;
    BSSID[1] ^= (UCHAR)(time & 0xff); 
    
    time >>= 8;
    BSSID[2] ^= (UCHAR)(time & 0xff); 
    
    time >>= 8;
    BSSID[3] ^= (UCHAR)(time & 0xff);
}

NDIS_STATUS
StaGetRateSetFromInfoEle(
    __in  PUCHAR InfoElemBlobPtr,
    __in  ULONG InfoElemBlobSize,
    __in  BOOLEAN basicRateOnly,
    __out PDOT11_RATE_SET rateSet
    )
{
    NDIS_STATUS ndisStatus;
    PUCHAR      tmpPtr;
    PUCHAR      rate = rateSet->ucRateSet;
    UCHAR       size;
    UCHAR       remainingLength = DOT11_RATE_SET_MAX_LENGTH;

    ndisStatus = Dot11GetInfoEle(InfoElemBlobPtr,
                                 InfoElemBlobSize,
                                 DOT11_INFO_ELEMENT_ID_SUPPORTED_RATES,
                                 &size,
                                 &tmpPtr);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        return ndisStatus;

    while ((size > 0) && (remainingLength > 0))
    {
        if (!basicRateOnly || (*tmpPtr & 0x80)) 
        {
            *rate = *tmpPtr;
            rate++;
            remainingLength--;
        }

        tmpPtr++;
        size--;
    }

    //
    // If size > 0, it means we have only copied a portion of the rates into our buffer
    //

    //
    // 802.11g make copy of extended supported rates as well
    //

    ndisStatus = Dot11GetInfoEle(InfoElemBlobPtr,
                                 InfoElemBlobSize,
                                 DOT11_INFO_ELEMENT_ID_EXTD_SUPPORTED_RATES,
                                 &size,
                                 &tmpPtr);

    if (ndisStatus == NDIS_STATUS_SUCCESS) 
    {
        while ((size > 0) && (remainingLength > 0))
        {
            if (!basicRateOnly || (*tmpPtr & 0x80)) 
            {
                *rate = *tmpPtr;
                rate++;
                remainingLength--;
            }

            tmpPtr++;
            size--;
        }
    }
    //
    // If size > 0, it means we have only copied a portion of the rates into our buffer
    //

    rateSet->uRateSetLength = (ULONG)PtrOffset(rateSet->ucRateSet, rate);
    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
StaFilterUnsupportedRates(
    __in PSTA_BSS_ENTRY pAPEntry, 
    __in PDOT11_RATE_SET rateSet
    )
{
    NDIS_STATUS     ndisStatus;
    DOT11_RATE_SET  APRateSet;
    ULONG           i;
    ULONG           j;

    //
    // Get the rate set supported by AP from its beacon
    //
    ndisStatus = StaGetRateSetFromInfoEle(pAPEntry->pDot11InfoElemBlob,
                                          pAPEntry->InfoElemBlobSize,
                                          FALSE, 
                                          &APRateSet);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        return ndisStatus;

    //
    // Filter out any rates that are not supported by AP
    //
    i = 0;
    while (i < rateSet->uRateSetLength) 
    {
        for (j = 0; j < APRateSet.uRateSetLength; j++) 
        {
            if ((rateSet->ucRateSet[i] & 0x7f) == (APRateSet.ucRateSet[j] & 0x7f))
                break;
        }

        //
        // remove the rate if it is not in AP's rate set
        //
        if (j == APRateSet.uRateSetLength)
        {
            rateSet->uRateSetLength--;
            rateSet->ucRateSet[i] = rateSet->ucRateSet[rateSet->uRateSetLength];
        }
        else
        {
            i++;
        }
    }

    //
    // We must have at least one good data rate
    //
    ASSERT(rateSet->uRateSetLength >= 1);
    
    return NDIS_STATUS_SUCCESS;
}           

ULONG
StaGetPhyIdByType(
    __in PNIC pNic,
    __in DOT11_PHY_TYPE PhyType
    )
{
    ULONG                       index;
    PDOT11_SUPPORTED_PHY_TYPES  supportedPhyTypes;
    UCHAR                       buffer[PHY_TYPE_BUFFER_SIZE];  
    //
    // Get supported PHY types.
    //
    supportedPhyTypes = (PDOT11_SUPPORTED_PHY_TYPES) buffer;
    supportedPhyTypes->uNumOfEntries = 0;
    Hw11QuerySupportedPHYTypes(pNic, 
                               STA_DESIRED_PHY_MAX_COUNT, 
                               supportedPhyTypes);

    //
    // Go through the list to find the matching type
    //
    for (index = 0; index < supportedPhyTypes->uNumOfEntries; index++)
    {
        if (PhyType == supportedPhyTypes->dot11PHYType[index])
            return (index);
    }

    //MPASSERTMSG("Couldn't find PhyType returning STA_INVALID_PHY_ID", FALSE );
    // 
    // No match, return an invalid PhyId value.
    //
    return STA_INVALID_PHY_ID;
}

DOT11_PHY_TYPE
StaGetPhyTypeById(
    __in PNIC pNic,
    __in ULONG PhyId
    )
{
    PDOT11_SUPPORTED_PHY_TYPES  supportedPhyTypes;
    UCHAR                       buffer[PHY_TYPE_BUFFER_SIZE];  

    //
    // Get supported PHY types.
    //
    supportedPhyTypes = (PDOT11_SUPPORTED_PHY_TYPES) buffer;
    supportedPhyTypes->uNumOfEntries = 0;
    Hw11QuerySupportedPHYTypes(pNic, 
                               STA_DESIRED_PHY_MAX_COUNT, 
                               supportedPhyTypes);

    //
    // Validate PhyId 
    //
    if (PhyId >= supportedPhyTypes->uNumOfEntries)
        return dot11_phy_type_unknown;

    //
    // Return the phy type
    //
    return supportedPhyTypes->dot11PHYType[PhyId];
}

NDIS_STATUS
StaParseRNSIE(
    __in PUCHAR RSNIEData,
    __in ULONG OUIPrefix,
    __in UCHAR RSNIELength,
    __out PRSN_IE_INFO RSNIEInfo
    )
{
    NdisZeroMemory(RSNIEInfo, sizeof(RSN_IE_INFO));
    RSNIEInfo->OUIPrefix = OUIPrefix;

    //
    // Get version (only mendatory field)
    //
    if (RSNIELength < 2)
    {
        return NDIS_STATUS_INVALID_DATA;
    }

    RSNIEInfo->Version = *((USHORT UNALIGNED *)RSNIEData);
    RSNIEData += 2;
    RSNIELength -= 2;

    // 
    // Get group cipher 
    //
    if (RSNIELength < 4)
    {
        return (RSNIELength == 0) ? NDIS_STATUS_SUCCESS : NDIS_STATUS_INVALID_DATA;
    }

    RSNIEInfo->GroupCipherCount = 1;
    RSNIEInfo->GroupCipher = RSNIEData;
    RSNIEData += 4;
    RSNIELength -= 4;

    // 
    // Get pairwise cipher count
    //
    if (RSNIELength < 2)
    {
        return (RSNIELength == 0) ? NDIS_STATUS_SUCCESS : NDIS_STATUS_INVALID_DATA;
    }

    RSNIEInfo->PairwiseCipherCount = *((USHORT UNALIGNED *)RSNIEData);
    RSNIEData += 2;
    RSNIELength -= 2;

    //
    // Get pairwise cipher
    //
    if (RSNIELength < RSNIEInfo->PairwiseCipherCount * 4)
    {
        return NDIS_STATUS_INVALID_DATA;
    }

    RSNIEInfo->PairwiseCipher = RSNIEData;
    RSNIEData += RSNIEInfo->PairwiseCipherCount * 4;
    RSNIELength -= (UCHAR)RSNIEInfo->PairwiseCipherCount * 4;

    //
    // Get AKM suite count
    //
    if (RSNIELength < 2)
    {
        return (RSNIELength == 0) ? NDIS_STATUS_SUCCESS : NDIS_STATUS_INVALID_DATA;
    }

    RSNIEInfo->AKMSuiteCount = *((USHORT UNALIGNED *)RSNIEData);
    RSNIEData += 2;
    RSNIELength -= 2;

    //
    // Get AKM suite
    //
    if (RSNIELength < RSNIEInfo->AKMSuiteCount * 4)
    {
        return NDIS_STATUS_INVALID_DATA;
    }

    RSNIEInfo->AKMSuite = RSNIEData;
    RSNIEData += RSNIEInfo->AKMSuiteCount * 4;
    RSNIELength -= (UCHAR)RSNIEInfo->AKMSuiteCount * 4;

    //
    // Get RSN capability
    //

    if (RSNIELength < 2)
    {
        return (RSNIELength == 0) ? NDIS_STATUS_SUCCESS : NDIS_STATUS_INVALID_DATA;
    }

    RSNIEInfo->Capability = *((USHORT UNALIGNED *)RSNIEData);
    RSNIEData += 2;
    RSNIELength -= 2;

    //
    // Get PMKID count
    //

    if (RSNIELength < 2)
    {
        return (RSNIELength == 0) ? NDIS_STATUS_SUCCESS : NDIS_STATUS_INVALID_DATA;
    }

    RSNIEInfo->PMKIDCount = *((USHORT UNALIGNED *)RSNIEData);
    RSNIEData += 2;
    RSNIELength -= 2;

    //
    // Get PMKID
    //
    if (RSNIELength != RSNIEInfo->PMKIDCount * 16)
    {
        return NDIS_STATUS_INVALID_DATA;
    }

    RSNIEInfo->PMKID = RSNIEData;
    return NDIS_STATUS_SUCCESS;
}

DOT11_CIPHER_ALGORITHM
StaGetCipherFromRSNOUI(
    __in ULONG prefix,
    __in ULONG oui
    )
{
    switch (oui - prefix)
    {
        case RSNA_CIPHER_WEP40:
            return DOT11_CIPHER_ALGO_WEP40;

        case RSNA_CIPHER_TKIP:
            return DOT11_CIPHER_ALGO_TKIP;

        case RSNA_CIPHER_CCMP:
            return DOT11_CIPHER_ALGO_CCMP;

        case RSNA_CIPHER_WEP104:
            return DOT11_CIPHER_ALGO_WEP104;

        default:
            return DOT11_CIPHER_ALGO_NONE;
    }
}

ULONG
StaGetRSNOUIFromCipher(
    __in ULONG prefix,
    __in DOT11_CIPHER_ALGORITHM cipher
    )
{
    switch (cipher)
    {
        case DOT11_CIPHER_ALGO_WEP40:
            return prefix + RSNA_CIPHER_WEP40;

        case DOT11_CIPHER_ALGO_TKIP:
            return prefix + RSNA_CIPHER_TKIP;

        case DOT11_CIPHER_ALGO_CCMP:
            return prefix + RSNA_CIPHER_CCMP;

        case DOT11_CIPHER_ALGO_WEP104:
            return prefix + RSNA_CIPHER_WEP104;

        default:
            ASSERT(0);
            return prefix + RSNA_CIPHER_CCMP;
    }
}

DOT11_AUTH_ALGORITHM
StaGetAuthAlgoFromRSNOUI(
    __in ULONG oui
    )
{
    switch (oui)
    {
        case RSNA_OUI_PREFIX + RSNA_AKM_RSNA:
            return DOT11_AUTH_ALGO_RSNA;

        case RSNA_OUI_PREFIX + RSNA_AKM_RSNA_PSK:
            return DOT11_AUTH_ALGO_RSNA_PSK;

        case WPA_OUI_PREFIX + RSNA_AKM_RSNA:
            return DOT11_AUTH_ALGO_WPA;

        case WPA_OUI_PREFIX + RSNA_AKM_RSNA_PSK:
            return DOT11_AUTH_ALGO_WPA_PSK;

        default:
            return DOT11_AUTH_ALGO_80211_OPEN;
    }
}

ULONG
StaGetRSNOUIFromAuthAlgo(
    __in DOT11_AUTH_ALGORITHM algo
    )
{
    switch (algo)
    {
        case DOT11_AUTH_ALGO_RSNA:
            return RSNA_OUI_PREFIX + RSNA_AKM_RSNA;

        case DOT11_AUTH_ALGO_RSNA_PSK:
            return RSNA_OUI_PREFIX + RSNA_AKM_RSNA_PSK;

        case DOT11_AUTH_ALGO_WPA:
            return WPA_OUI_PREFIX + RSNA_AKM_RSNA;

        case DOT11_AUTH_ALGO_WPA_PSK:
            return WPA_OUI_PREFIX + RSNA_AKM_RSNA_PSK;

        default:
            ASSERT(0);
            return RSNA_OUI_PREFIX + RSNA_AKM_RSNA;
    }
}

DOT11_CIPHER_ALGORITHM
StaGetGroupCipherFromRSNIEInfo(
    __in PRSN_IE_INFO RSNIEInfo
    )
{
    if (RSNIEInfo->GroupCipherCount == 1)
    {
        return StaGetCipherFromRSNOUI(RSNIEInfo->OUIPrefix, *((ULONG UNALIGNED *)RSNIEInfo->GroupCipher));
    }
    else
    {
        return DOT11_CIPHER_ALGO_NONE;
    }
}

DOT11_CIPHER_ALGORITHM
StaGetPairwiseCipherFromRSNIEInfo(
    __in PRSN_IE_INFO RSNIEInfo,
    __in USHORT index
    )
{
    if (RSNIEInfo->PairwiseCipherCount > index)
    {
        return StaGetCipherFromRSNOUI(RSNIEInfo->OUIPrefix, *((ULONG UNALIGNED *)Add2Ptr(RSNIEInfo->PairwiseCipher, index * 4)));
    }
    else
    {
        return DOT11_CIPHER_ALGO_NONE;
    }
}

DOT11_AUTH_ALGORITHM
StaGetAKMSuiteFromRSNIEInfo(
    __in PRSN_IE_INFO RSNIEInfo,
    __in USHORT index
    )
{
    if (RSNIEInfo->AKMSuiteCount > index)
    {
        return StaGetAuthAlgoFromRSNOUI(*((ULONG UNALIGNED *)Add2Ptr(RSNIEInfo->AKMSuite, index * 4)));
    }
    else
    {
        return DOT11_CIPHER_ALGO_NONE;
    }
}

NDIS_STATUS
StaAttachInfraRSNIE(
    __in PSTATION pStation, 
    __in PSTA_BSS_ENTRY pAPEntry, 
    __inout PUCHAR *ppCurrentIE,
    __inout PUSHORT pIESize
    )
{
    USHORT      size;
    NDIS_STATUS ndisStatus;
    UCHAR       SecIELength;
    PUCHAR      SecIEData;
    RSN_IE_INFO RSNIEInfo;
    UCHAR       IEId;
    ULONG       pmkidIndex = 0;
    BOOLEAN     attachPMKID = FALSE;

    //
    // Get RSNIEInfo from AP
    //
    if (pStation->Config.AuthAlgorithm == DOT11_AUTH_ALGO_RSNA ||
        pStation->Config.AuthAlgorithm == DOT11_AUTH_ALGO_RSNA_PSK)
    {
        IEId = DOT11_INFO_ELEMENT_ID_RSN;
        ndisStatus = Dot11GetInfoEle(pAPEntry->pDot11InfoElemBlob,
                                     pAPEntry->InfoElemBlobSize,
                                     DOT11_INFO_ELEMENT_ID_RSN,
                                     &SecIELength,
                                     (PVOID)&SecIEData);

        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            return ndisStatus;
        }

        ndisStatus = StaParseRNSIE(SecIEData, RSNA_OUI_PREFIX, SecIELength, &RSNIEInfo);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            return ndisStatus;
        }
    }
    else 
    {
        IEId = DOT11_INFO_ELEMENT_ID_VENDOR_SPECIFIC;
        ndisStatus = Dot11GetWPAIE(pAPEntry->pDot11InfoElemBlob,
                                   pAPEntry->InfoElemBlobSize,
                                   &SecIELength,
                                   (PVOID)&SecIEData);

        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            return ndisStatus;
        }

        ndisStatus = StaParseRNSIE(SecIEData, WPA_OUI_PREFIX, SecIELength, &RSNIEInfo);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            return ndisStatus;
        }
    }

    //
    // Our RSN IE will contain 1 group cipher, 1 pairwise cipher, 1 AKM suite, and capability.
    // In addition, WPA IE has 4 bytes WPA_IE_TAG, RNSA IE has PMKIDs.
    //
    size = sizeof(DOT11_INFO_ELEMENT) +                 // IE ID and length
           sizeof(USHORT) +                             // version
           sizeof(ULONG) +                              // group cipher
           sizeof(USHORT) + sizeof(ULONG) +             // pairwise cipher
           sizeof(USHORT) + sizeof(ULONG) +             // AKM suite
           sizeof(USHORT);                              // capability.  // TODO: should there be a USHORT for WPA

    if (RSNIEInfo.OUIPrefix == WPA_OUI_PREFIX)
    {
        size += sizeof(ULONG);                          // WPA IE tag, 0x01f25000
    }
    else if (pStation->Config.AuthAlgorithm == DOT11_AUTH_ALGO_RSNA &&
             pStation->Config.PMKIDCount > 0)
    {
        // walk through PMKIDList and locate a PMKID of which BSSID matchs AP's BSSID
        for (pmkidIndex = 0; pmkidIndex < pStation->Config.PMKIDCount; pmkidIndex++)
        {
            if (NdisEqualMemory(pStation->Config.PMKIDList[pmkidIndex].BSSID, pAPEntry->Dot11BSSID, sizeof(DOT11_MAC_ADDRESS)) == 1)
            {
                attachPMKID = TRUE;
                break;      // there should be at most one pmkid to attach
            }
        }

        if (attachPMKID == TRUE)
        {
            size += sizeof(USHORT) + sizeof(DOT11_PMKID_VALUE);
        }
    }

    ASSERT(size <= 255 + sizeof(DOT11_INFO_ELEMENT));
    if (*pIESize < size)
    {
        return STATUS_BUFFER_TOO_SMALL;
    }
    
    //
    // Set IE ID and length
    //
    ((DOT11_INFO_ELEMENT UNALIGNED *)(*ppCurrentIE))->ElementID = IEId;
    ((DOT11_INFO_ELEMENT UNALIGNED *)(*ppCurrentIE))->Length = (UCHAR)(size - sizeof(DOT11_INFO_ELEMENT));
    *ppCurrentIE += sizeof(DOT11_INFO_ELEMENT);

    //
    // Set WPA_IE_TAG for WPA IE
    //
    if (RSNIEInfo.OUIPrefix == WPA_OUI_PREFIX)
    {
        *((ULONG UNALIGNED *)(*ppCurrentIE)) = WPA_IE_TAG;
        *ppCurrentIE += sizeof(ULONG);
    }

    //
    // Set version
    //
    *((USHORT UNALIGNED *)(*ppCurrentIE)) = 1;
    *ppCurrentIE += sizeof(USHORT);

    //
    // If AP RSN IE did not contain a group cipher, we wont add any
    // fields except version
    //
    if (RSNIEInfo.GroupCipher == NULL)
    {
        //
        // Update remaining IE size.
        //
        *pIESize = *pIESize - size;
        return NDIS_STATUS_SUCCESS;
    }

    //
    // Set group cipher
    //
    *((ULONG UNALIGNED *)(*ppCurrentIE)) = *((ULONG UNALIGNED *)RSNIEInfo.GroupCipher);
    *ppCurrentIE += sizeof(ULONG);

    //
    // Temoporarily set our enabled multicast cipher. We will notify hardware layer of this selection
    // when association is successful.
    //
    pStation->Config.MulticastCipherAlgorithm = StaGetGroupCipherFromRSNIEInfo(&RSNIEInfo);

    //
    // Set pairwise cipher
    //
    *((USHORT UNALIGNED *)(*ppCurrentIE)) = 1;
    *ppCurrentIE += sizeof(USHORT);
    *((ULONG UNALIGNED *)(*ppCurrentIE)) = StaGetRSNOUIFromCipher(RSNIEInfo.OUIPrefix, 
                                                                  pStation->Config.UnicastCipherAlgorithm);
    *ppCurrentIE += sizeof(ULONG);

    //
    // Set AKM suite
    //
    *((USHORT UNALIGNED *)(*ppCurrentIE)) = 1;
    *ppCurrentIE += sizeof(USHORT);
    *((ULONG UNALIGNED *)(*ppCurrentIE)) = StaGetRSNOUIFromAuthAlgo(pStation->Config.AuthAlgorithm);
    *ppCurrentIE += sizeof(ULONG);

    //
    // Set capability. Get the capability from hardware layer.
    //
    *((USHORT UNALIGNED *)(*ppCurrentIE)) = Hw11GetRSNCapability(pStation->pNic);
    *ppCurrentIE += sizeof(USHORT);

    //
    // Set PMKID
    //
    if (attachPMKID == TRUE)
    {
        ASSERT(pmkidIndex < pStation->Config.PMKIDCount);

        *((USHORT UNALIGNED *)(*ppCurrentIE)) = 1;  // one pmkid at most to attach
        *ppCurrentIE += sizeof(USHORT);

        NdisMoveMemory(*ppCurrentIE,
                        pStation->Config.PMKIDList[pmkidIndex].PMKID,
                        sizeof(DOT11_PMKID_VALUE));
        *ppCurrentIE += sizeof(DOT11_PMKID_VALUE);
    }

    //
    // Update remaining IE size.
    //
    *pIESize = *pIESize - size;

    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
StaAttachAdHocRSNIE(
    __in PSTATION pStation, 
    __inout PUCHAR *ppCurrentIE,
    __inout PUSHORT pIESize
    )
{
    USHORT      size;

    ASSERT(pStation->Config.AuthAlgorithm == DOT11_AUTH_ALGO_RSNA_PSK);

    //
    // Our RSN IE will contain 1 group cipher, 1 pairwise cipher, 1 AKM suite, and capability.
    //
    size = sizeof(DOT11_INFO_ELEMENT) +                 // IE ID and length
           sizeof(USHORT) +                             // version
           sizeof(ULONG) +                              // group cipher
           sizeof(USHORT) + sizeof(ULONG) +             // pairwise cipher
           sizeof(USHORT) + sizeof(ULONG) +             // AKM suite
           sizeof(USHORT);                              // capability.

    //
    // Set IE ID and length
    //
    ((DOT11_INFO_ELEMENT UNALIGNED *)(*ppCurrentIE))->ElementID = DOT11_INFO_ELEMENT_ID_RSN;
    ((DOT11_INFO_ELEMENT UNALIGNED *)(*ppCurrentIE))->Length = (UCHAR)(size - sizeof(DOT11_INFO_ELEMENT));
    *ppCurrentIE += sizeof(DOT11_INFO_ELEMENT);

    //
    // Set version
    //
    *((USHORT UNALIGNED *)(*ppCurrentIE)) = 1;
    *ppCurrentIE += sizeof(USHORT);

    //
    // Set group cipher
    //
    *((ULONG UNALIGNED *)(*ppCurrentIE)) = StaGetRSNOUIFromCipher(RSNA_OUI_PREFIX, 
                                                                  pStation->Config.MulticastCipherAlgorithm);
    *ppCurrentIE += sizeof(ULONG);

    //
    // Set pairwise cipher
    //
    *((USHORT UNALIGNED *)(*ppCurrentIE)) = 1;
    *ppCurrentIE += sizeof(USHORT);
    *((ULONG UNALIGNED *)(*ppCurrentIE)) = StaGetRSNOUIFromCipher(RSNA_OUI_PREFIX, 
                                                                  pStation->Config.UnicastCipherAlgorithm);
    *ppCurrentIE += sizeof(ULONG);

    //
    // Set AKM suite
    //
    *((USHORT UNALIGNED *)(*ppCurrentIE)) = 1;
    *ppCurrentIE += sizeof(USHORT);
    *((ULONG UNALIGNED *)(*ppCurrentIE)) = StaGetRSNOUIFromAuthAlgo(pStation->Config.AuthAlgorithm);
    *ppCurrentIE += sizeof(ULONG);

    //
    // Set capability. Get the capability from hardware layer.
    //
    *((USHORT UNALIGNED *)(*ppCurrentIE)) = Hw11GetRSNCapability(pStation->pNic);
    *ppCurrentIE += sizeof(USHORT);

    //
    // Update remaining IE size.
    //
    *pIESize = *pIESize - size;

    return NDIS_STATUS_SUCCESS;
}

BOOLEAN
StaMatchRSNInfo(
    __in PSTATION        pStation,
    __in PRSN_IE_INFO    RSNIEInfo
    )
{
    DOT11_CIPHER_ALGORITHM  cipher;
    DOT11_AUTH_ALGORITHM    authAlgo;
    ULONG                   index;

    //
    // Check the RSNIEInfo structure.
    // Only version 1 is supported
    //
    if (RSNIEInfo->Version != 1)
    {
        MpTrace(COMP_ASSOC, DBG_LOUD, 
                (" - Reject (AP is running unspported RSNA version %d)\n", 
                RSNIEInfo->Version));
        return FALSE;
    }

    //
    // In adhoc mode, make sure there is exactly one pairwise cipher, one AKM in
    // the IE. Also, the RSN capability should match ours and there should be no PMKID.
    //
    if (pStation->Config.BSSType == dot11_BSS_type_independent)
    {
        if (RSNIEInfo->PairwiseCipherCount != 1)
        {
            MpTrace(COMP_ASSOC, DBG_LOUD, (" - Reject (AdHoc network does not specify exactly one pairwise cipher)\n"));
            return FALSE;
        }

        if (RSNIEInfo->AKMSuiteCount != 1)
        {
            MpTrace(COMP_ASSOC, DBG_LOUD, (" - Reject (AdHoc network does not specify exactly one AKM)\n"));
            return FALSE;
        }

        if (RSNIEInfo->PMKIDCount != 0)
        {
            MpTrace(COMP_ASSOC, DBG_LOUD, (" - Reject (AdHoc network uses PMKID)\n"));
            return FALSE;
        }
    }

    //
    // Check AP's group cipher is one of our enabled multicast ciphers.
    //
    ASSERT(RSNIEInfo->GroupCipherCount <= 1);
    if (RSNIEInfo->GroupCipherCount == 0)
    {
        MpTrace(COMP_ASSOC, DBG_LOUD, (" - Reject (AP does not specify group cipher)\n"));
        return FALSE;
    }

    cipher = StaGetGroupCipherFromRSNIEInfo(RSNIEInfo);
    for (index = 0; index < pStation->Config.MulticastCipherAlgorithmCount; index++)
    {
        if (cipher == pStation->Config.MulticastCipherAlgorithmList[index])
            break;
    }

    if (index == pStation->Config.MulticastCipherAlgorithmCount)
    {
        MpTrace(COMP_ASSOC, DBG_LOUD, (" - Reject (no matching group cipher)\n"));
        return FALSE;
    }

    //
    // Check AP's pairwise ciphers. At least one must match with our unicast cipher
    //
    for (index = 0; index < RSNIEInfo->PairwiseCipherCount; index++)
    {
        cipher = StaGetPairwiseCipherFromRSNIEInfo(RSNIEInfo, (USHORT)index);
        if (cipher == pStation->Config.UnicastCipherAlgorithm)
            break;
    }

    if (index == RSNIEInfo->PairwiseCipherCount)
    {
        MpTrace(COMP_ASSOC, DBG_LOUD, (" - Reject (no matching pairwise cipher)\n"));
        return FALSE;
    }

    //
    // Check AP's AKM suite list. At least one must match with our auth algorithm
    //
    for (index = 0; index < RSNIEInfo->AKMSuiteCount; index++)
    {
        authAlgo = StaGetAKMSuiteFromRSNIEInfo(RSNIEInfo, (USHORT)index);
        if (authAlgo == pStation->Config.AuthAlgorithm)
            break;
    }

    if (index == RSNIEInfo->AKMSuiteCount)
    {
        MpTrace(COMP_ASSOC, DBG_LOUD, (" - Reject (no matching AKM suite)\n"));
        return FALSE;
    }

    return TRUE;
}

