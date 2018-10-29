/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Sta_Oids.c

Abstract:
    STA layer OID handling functions
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
#include "precomp.h"
#include "st_Oids.h"
#include "st_scan.h"
#include "st_conn.h"
#include "st_adhoc.h"



#if DOT11_TRACE_ENABLED
#include "Sta_Oids.tmh"
#endif


NDIS_STATUS
Sta11QueryExtStaCapability(
    __in  PSTATION        pStation,
    __out PDOT11_EXTSTA_CAPABILITY   pDot11ExtStaCap
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PNIC pNic = pStation->pNic;

    UNREFERENCED_PARAMETER(pStation);
    
    do
    {
        pDot11ExtStaCap->uScanSSIDListSize = 4;     // minimum required.
        pDot11ExtStaCap->uDesiredBSSIDListSize = STA_DESIRED_BSSID_MAX_COUNT;
        pDot11ExtStaCap->uDesiredSSIDListSize = 1;
        pDot11ExtStaCap->uExcludedMacAddressListSize = STA_EXCLUDED_MAC_ADDRESS_MAX_COUNT;
        pDot11ExtStaCap->uPrivacyExemptionListSize = 32;
        pDot11ExtStaCap->uKeyMappingTableSize = Hw11KeyMappingKeyTableSize(pNic);
        pDot11ExtStaCap->uDefaultKeyTableSize = Hw11DefaultKeyTableSize(pNic);
        pDot11ExtStaCap->uWEPKeyValueMaxLength = Hw11WEP104Implemented(pNic) ? 
                                                 104 / 8 : (Hw11WEP40Implemented(pNic) ? 40 / 8 : 0);
        pDot11ExtStaCap->uPMKIDCacheSize = STA_PMKID_MAX_COUNT;
        pDot11ExtStaCap->uMaxNumPerSTADefaultKeyTables = Hw11PerStaKeyTableSize(pNic);

    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
Sta11QuerySupportedUnicastAlgorithmPair(
    __in  PSTATION        pStation,
    __out_bcount(TotalLength) PDOT11_AUTH_CIPHER_PAIR_LIST    pAuthCipherList,
    __in __range(sizeof(DOT11_AUTH_CIPHER_PAIR_LIST) - sizeof(DOT11_AUTH_CIPHER_PAIR), 0x7FFFFFFF) ULONG           TotalLength
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    ULONG       BytesNeeded = 0;
    ULONG       count;
    PNIC        pNic = pStation->pNic;
    BOOLEAN     WEP40Implemented = Hw11WEP40Implemented(pNic);
    BOOLEAN     WEP104Implemented = Hw11WEP104Implemented(pNic);
    BOOLEAN     TKIPImplemented = Hw11TKIPImplemented(pNic);
    BOOLEAN     CCMPImplemented = Hw11CCMPImplemented(pNic);

    UNREFERENCED_PARAMETER(pStation);

    do
    {
        count = 1;
        if (WEP40Implemented) 
        {
            count++;
            if (pStation->Config.BSSType == dot11_BSS_type_infrastructure)
                count++;
        }

        if (WEP104Implemented)
        {
            count++;
            if (pStation->Config.BSSType == dot11_BSS_type_infrastructure)
                count++;
        }

        if (WEP40Implemented || WEP104Implemented)
        {
            count++;
            if (pStation->Config.BSSType == dot11_BSS_type_infrastructure)
                count++;
        }

        if (TKIPImplemented && pStation->Config.BSSType == dot11_BSS_type_infrastructure)
        {
            count += 4;
        }

        if (CCMPImplemented)
        {
            count++;
            if (pStation->Config.BSSType == dot11_BSS_type_infrastructure)
                count += 3;
        }

        // Ensure enough space for one entry (though this would
        // get saved as part of the DOT11_AUTH_CIPHER_PAIR_LIST structure
        // itself)
        BytesNeeded = FIELD_OFFSET(DOT11_AUTH_CIPHER_PAIR_LIST, AuthCipherPairs) +
                      count * sizeof(DOT11_AUTH_CIPHER_PAIR);
        
        pAuthCipherList->uNumOfEntries = 0;
        pAuthCipherList->uTotalNumOfEntries = count;

        if (TotalLength < BytesNeeded)
        {
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;            
            break;
        }

        pAuthCipherList->uNumOfEntries = count;
        
        count = 0;
        pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_80211_OPEN;
        pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_NONE;

        if (WEP40Implemented)
        {
            count++;
            pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_80211_OPEN;
            pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_WEP40;

            if (pStation->Config.BSSType == dot11_BSS_type_infrastructure)
            {
                count++;
                pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_80211_SHARED_KEY;
                pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_WEP40;
            }
        }

        if (WEP104Implemented) 
        {
            count++;
            pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_80211_OPEN;
            pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_WEP104;

            if (pStation->Config.BSSType == dot11_BSS_type_infrastructure)
            {
                count++;
                pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_80211_SHARED_KEY;
                pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_WEP104;
            }
        }

        if (WEP40Implemented || WEP104Implemented)
        {
            count++;
            pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_80211_OPEN;
            pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_WEP;

            if (pStation->Config.BSSType == dot11_BSS_type_infrastructure)
            {
                count++;
                pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_80211_SHARED_KEY;
                pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_WEP;
            }
        }

        if (TKIPImplemented && pStation->Config.BSSType == dot11_BSS_type_infrastructure)
        {
            count++;
            pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_WPA;
            pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_TKIP;

            count++;
            pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_WPA_PSK;
            pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_TKIP;

            count++;
            pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_RSNA;
            pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_TKIP;

            count++;
            pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_RSNA_PSK;
            pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_TKIP;
        }

        if (CCMPImplemented)
        {
            if (pStation->Config.BSSType == dot11_BSS_type_infrastructure)
            {
                count++;
                pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_WPA;
                pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_CCMP;

                count++;
                pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_WPA_PSK;
                pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_CCMP;

                count++;
                pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_RSNA;
                pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_CCMP;
            }

            count++;
            pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_RSNA_PSK;
            pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_CCMP;
        }

    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
Sta11QuerySupportedMulticastAlgorithmPair(
    __in  PSTATION        pStation,
    __out_bcount(TotalLength) PDOT11_AUTH_CIPHER_PAIR_LIST    pAuthCipherList,
    __in __range(sizeof(DOT11_AUTH_CIPHER_PAIR_LIST) - sizeof(DOT11_AUTH_CIPHER_PAIR), 0x7FFFFFFF)  ULONG           TotalLength
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    ULONG       BytesNeeded = 0;
    ULONG       count;
    PNIC        pNic = pStation->pNic;
    BOOLEAN     WEP40Implemented = Hw11WEP40Implemented(pNic);
    BOOLEAN     WEP104Implemented = Hw11WEP104Implemented(pNic);
    BOOLEAN     TKIPImplemented = Hw11TKIPImplemented(pNic);
    BOOLEAN     CCMPImplemented = Hw11CCMPImplemented(pNic);

    UNREFERENCED_PARAMETER(pStation);

    do
    {
        count = 1;

        if (WEP40Implemented) 
        {
            count++;
            if (pStation->Config.BSSType == dot11_BSS_type_infrastructure)
            {
                count++;
                if (TKIPImplemented || CCMPImplemented)
                    count += 4;
            }
        }

        if (WEP104Implemented) 
        {
            count++;
            if (pStation->Config.BSSType == dot11_BSS_type_infrastructure)
            {
                count++;
                if (TKIPImplemented || CCMPImplemented)
                    count += 4;
            }
        }

        if (WEP40Implemented || WEP104Implemented)
        {
            count++;
            if (pStation->Config.BSSType == dot11_BSS_type_infrastructure)
                count++;
        }

        if (TKIPImplemented && pStation->Config.BSSType == dot11_BSS_type_infrastructure)
        {
            count += 4;
        }

        if (CCMPImplemented)
        {
            count++;
            if (pStation->Config.BSSType == dot11_BSS_type_infrastructure)
                count += 3;
        }

        // Ensure enough space for one entry (though this would
        // get saved as part of the DOT11_AUTH_CIPHER_PAIR_LIST structure
        // itself)
        BytesNeeded = FIELD_OFFSET(DOT11_AUTH_CIPHER_PAIR_LIST, AuthCipherPairs) +
                      count * sizeof(DOT11_AUTH_CIPHER_PAIR);
        
        pAuthCipherList->uNumOfEntries = 0;
        pAuthCipherList->uTotalNumOfEntries = count;

        if (TotalLength < BytesNeeded)
        {
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;            
            break;
        }

        pAuthCipherList->uNumOfEntries = count;

        count = 0;
        pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_80211_OPEN;
        pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_NONE;

        if (WEP40Implemented)
        {
            count++;
            pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_80211_OPEN;
            pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_WEP40;

            if (pStation->Config.BSSType == dot11_BSS_type_infrastructure)
            {
                count++;
                pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_80211_SHARED_KEY;
                pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_WEP40;
 
                if (TKIPImplemented || CCMPImplemented) 
                {
                    count++;
                    pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_WPA;
                    pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_WEP40;

                    count++;
                    pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_WPA_PSK;
                    pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_WEP40;

                    count++;
                    pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_RSNA;
                    pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_WEP40;

                    count++;
                    pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_RSNA_PSK;
                    pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_WEP40;

                }
            }
        }

        if (WEP104Implemented) 
        {
            count++;
            pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_80211_OPEN;
            pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_WEP104;

            if (pStation->Config.BSSType == dot11_BSS_type_infrastructure)
            {
                count++;
                pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_80211_SHARED_KEY;
                pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_WEP104;

                if (TKIPImplemented || CCMPImplemented)
                {
                    count++;
                    pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_WPA;
                    pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_WEP104;

                    count++;
                    pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_WPA_PSK;
                    pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_WEP104;

                    count++;
                    pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_RSNA;
                    pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_WEP104;

                    count++;
                    pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_RSNA_PSK;
                    pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_WEP104;

                }
            }
        }

        if (WEP40Implemented || WEP104Implemented)
        {
            count++;
            pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_80211_OPEN;
            pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_WEP;

            if (pStation->Config.BSSType == dot11_BSS_type_infrastructure)
            {
                count++;
                pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_80211_SHARED_KEY;
                pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_WEP;
            }
        }

        if (TKIPImplemented && pStation->Config.BSSType == dot11_BSS_type_infrastructure)
        {
            count++;
            pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_WPA;
            pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_TKIP;

            count++;
            pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_WPA_PSK;
            pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_TKIP;

            count++;
            pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_RSNA;
            pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_TKIP;

            count++;
            pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_RSNA_PSK;
            pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_TKIP;
        }

        if (CCMPImplemented)
        {
            if (pStation->Config.BSSType == dot11_BSS_type_infrastructure)
            {
                count++;
                pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_WPA;
                pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_CCMP;

                count++;
                pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_WPA_PSK;
                pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_CCMP;

                count++;
                pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_RSNA;
                pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_CCMP;
            }

            count++;
            pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_RSNA_PSK;
            pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_CCMP;
        }

    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
Sta11QueryEnabledUnicastCipherAlgorithm(
    __in  PSTATION        pStation,
    __out PDOT11_CIPHER_ALGORITHM_LIST  pAuthCipherList,
    __in  ULONG           TotalLength
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    ULONG       BytesNeeded = 0;

    UNREFERENCED_PARAMETER(pStation);
    
    do
    {
        // Ensure enough space for one entry (though this would
        // get saved as part of the DOT11_CIPHER_ALGORITHM_LIST structure
        // itself)

        BytesNeeded = FIELD_OFFSET(DOT11_CIPHER_ALGORITHM_LIST, AlgorithmIds) +
                      1 * sizeof(DOT11_CIPHER_ALGORITHM);
        
        if (TotalLength < BytesNeeded)
        {
            pAuthCipherList->uNumOfEntries = 0;
            pAuthCipherList->uTotalNumOfEntries = 1;
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;            
            break;
        }

        pAuthCipherList->uNumOfEntries = 1;
        pAuthCipherList->uTotalNumOfEntries = 1;
        pAuthCipherList->AlgorithmIds[0] = pStation->Config.UnicastCipherAlgorithm;

    } while(FALSE);

    return ndisStatus;
}



NDIS_STATUS
StaGetAlgorithmPair(
    __in  PSTATION                    pStation,
    __in  STA_QUERY_ALGO_PAIR_FUNC    QueryFunction,
    __deref_out_opt PDOT11_AUTH_CIPHER_PAIR    *AlgoPairs,
    __out PULONG                      NumAlgoPairs
    )
{
    DOT11_AUTH_CIPHER_PAIR_LIST     CipherPairList;
    PDOT11_AUTH_CIPHER_PAIR_LIST    FullPairList;
    NDIS_STATUS                     ndisStatus;
    ULONG                           size;

    *AlgoPairs = NULL;
    *NumAlgoPairs = 0;

    //
    // First get the total size of the algorithm pair list.
    //
    ndisStatus = (*QueryFunction)(pStation, &CipherPairList, sizeof(CipherPairList));
    if (ndisStatus != NDIS_STATUS_SUCCESS && ndisStatus != NDIS_STATUS_BUFFER_OVERFLOW)
    {
        return ndisStatus;
    }

    // Integer overflow
    if (FIELD_OFFSET(DOT11_AUTH_CIPHER_PAIR_LIST, AuthCipherPairs) > 
            FIELD_OFFSET(DOT11_AUTH_CIPHER_PAIR_LIST, AuthCipherPairs) + 
            CipherPairList.uTotalNumOfEntries * sizeof(DOT11_AUTH_CIPHER_PAIR))
    {
        return NDIS_STATUS_FAILURE;
    }

    size = FIELD_OFFSET(DOT11_AUTH_CIPHER_PAIR_LIST, AuthCipherPairs) +
           CipherPairList.uTotalNumOfEntries * sizeof(DOT11_AUTH_CIPHER_PAIR);

    MP_ALLOCATE_MEMORY(pStation->MiniportAdapterHandle, 
                       &FullPairList,
                       size,
                       STA11_MEMORY_TAG);
    if (FullPairList == NULL)
    {
        return NDIS_STATUS_RESOURCES;
    }

    //
    // Get the size of the list and copy the algorithm pair list data. Note that we over-allocated a little
    // bit for convenience.
    //

    ndisStatus = (*QueryFunction)(pStation, FullPairList, size);
    ASSERT(ndisStatus == NDIS_STATUS_SUCCESS && FullPairList->uTotalNumOfEntries == FullPairList->uNumOfEntries);

    *AlgoPairs = (PDOT11_AUTH_CIPHER_PAIR) FullPairList;
    *NumAlgoPairs = FullPairList->uNumOfEntries;

    //
    // Copy the algorithm pair to the beginning of the allocated buffer. Note that we cannot
    // use NdisMoveMemory as the source and destination overlap.
    //
    RtlMoveMemory(FullPairList,
                  FullPairList->AuthCipherPairs,
                  FullPairList->uNumOfEntries * sizeof(DOT11_AUTH_CIPHER_PAIR));

    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Sta11Fill80211Attributes(
    __in  PSTATION        pStation,
    __out PNDIS_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES attr
    )
{
    DOT11_EXTSTA_CAPABILITY         ExtStaCap;
    NDIS_STATUS                     ndisStatus;
    DOT11_BSS_TYPE                  savedBssType;

    MP_ALLOCATE_MEMORY(pStation->MiniportAdapterHandle, 
                       &attr->ExtSTAAttributes,
                       sizeof(DOT11_EXTSTA_ATTRIBUTES),
                       STA11_MEMORY_TAG);
    if (attr->ExtSTAAttributes == NULL)
    {
        return NDIS_STATUS_RESOURCES;
    }

    NdisZeroMemory(attr->ExtSTAAttributes, sizeof(DOT11_EXTSTA_ATTRIBUTES));

    //
    // First part of the attribute is the same as the capability. Get it
    // from Sta11QueryExtStaCapability.
    //

    ndisStatus = Sta11QueryExtStaCapability(pStation, &ExtStaCap);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        return ndisStatus;
    }

    MP_ASSIGN_NDIS_OBJECT_HEADER(attr->ExtSTAAttributes->Header, 
                                 NDIS_OBJECT_TYPE_DEFAULT,
                                 DOT11_EXTSTA_ATTRIBUTES_REVISION_1,
                                 sizeof(DOT11_EXTSTA_ATTRIBUTES));

    attr->ExtSTAAttributes->uScanSSIDListSize = ExtStaCap.uScanSSIDListSize;
    attr->ExtSTAAttributes->uDesiredBSSIDListSize = ExtStaCap.uDesiredBSSIDListSize;
    attr->ExtSTAAttributes->uDesiredSSIDListSize = ExtStaCap.uDesiredSSIDListSize;
    attr->ExtSTAAttributes->uExcludedMacAddressListSize = ExtStaCap.uExcludedMacAddressListSize;
    attr->ExtSTAAttributes->uPrivacyExemptionListSize = ExtStaCap.uPrivacyExemptionListSize;
    attr->ExtSTAAttributes->uKeyMappingTableSize = ExtStaCap.uKeyMappingTableSize;
    attr->ExtSTAAttributes->uDefaultKeyTableSize = ExtStaCap.uDefaultKeyTableSize;
    attr->ExtSTAAttributes->uWEPKeyValueMaxLength = ExtStaCap.uWEPKeyValueMaxLength;
    attr->ExtSTAAttributes->uPMKIDCacheSize = ExtStaCap.uPMKIDCacheSize;
    attr->ExtSTAAttributes->uMaxNumPerSTADefaultKeyTables = ExtStaCap.uMaxNumPerSTADefaultKeyTables;
    attr->ExtSTAAttributes->bStrictlyOrderedServiceClassImplemented = FALSE;

    //
    // Safe mode enabled
    //
    attr->ExtSTAAttributes->bSafeModeImplemented = TRUE;

    //
    // 11d stuff.
    //
    attr->ExtSTAAttributes->uNumSupportedCountryOrRegionStrings = 0;
    attr->ExtSTAAttributes->pSupportedCountryOrRegionStrings = NULL;

    //
    // We are about to query the supported algorithm pairs for both infrastructure and ad hoc.
    // We save our current bss type and temporarily set bss type to infrastructure and ad hoc
    // before each query. After we are done, we restore our bss type.
    //
    savedBssType = pStation->Config.BSSType;
    __try 
    {
        //
        // Set bss type to infra
        //
        pStation->Config.BSSType = dot11_BSS_type_infrastructure;

        //
        // Get unicast algorithm pair list for infrastructure
        //
        ndisStatus = StaGetAlgorithmPair(pStation,
                                         Sta11QuerySupportedUnicastAlgorithmPair,
                                         &attr->ExtSTAAttributes->pInfraSupportedUcastAlgoPairs,
                                         &attr->ExtSTAAttributes->uInfraNumSupportedUcastAlgoPairs);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
            __leave;

        //
        // Get multicast algorithm pair list for infrastructure
        //
        ndisStatus = StaGetAlgorithmPair(pStation,
                                         Sta11QuerySupportedMulticastAlgorithmPair,
                                         &attr->ExtSTAAttributes->pInfraSupportedMcastAlgoPairs,
                                         &attr->ExtSTAAttributes->uInfraNumSupportedMcastAlgoPairs);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
            __leave;

        //
        // Now set bss type to adhoc
        //
        pStation->Config.BSSType = dot11_BSS_type_independent;

        //
        // Get unicast algorithm pair list for ad hoc
        //
        ndisStatus = StaGetAlgorithmPair(pStation,
                                         Sta11QuerySupportedUnicastAlgorithmPair,
                                         &attr->ExtSTAAttributes->pAdhocSupportedUcastAlgoPairs,
                                         &attr->ExtSTAAttributes->uAdhocNumSupportedUcastAlgoPairs);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
            __leave;

        //
        // Get multicast algorithm pair list for ad hoc
        //
        ndisStatus = StaGetAlgorithmPair(pStation,
                                         Sta11QuerySupportedMulticastAlgorithmPair,
                                         &attr->ExtSTAAttributes->pAdhocSupportedMcastAlgoPairs,
                                         &attr->ExtSTAAttributes->uAdhocNumSupportedMcastAlgoPairs);
    }
    __finally
    {
        //
        // Restore our bss type 
        //
        pStation->Config.BSSType = savedBssType;
    }

    return ndisStatus;
}


NDIS_STATUS
Sta11QueryDesiredBSSType(
    __in  PSTATION        pStation,
    __in  PDOT11_BSS_TYPE bssType
    )
{
    *bssType = pStation->Config.BSSType;
    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Sta11QueryDesiredSSIDList(
    __in  PSTATION        pStation,
    __out PDOT11_SSID_LIST    pDot11SSIDList,
    __in  ULONG           TotalLength
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    ULONG       BytesNeeded = 0;
    
    do
    {
        // Ensure enough space for one entry (though this would
        // get saved as part of the DOT11_SSID_LIST structure
        // itself)
        BytesNeeded =  FIELD_OFFSET(DOT11_SSID_LIST, SSIDs)
                        + 1 * sizeof(DOT11_SSID);
        
        if (TotalLength < BytesNeeded)
        {
            pDot11SSIDList->uNumOfEntries = 0;
            pDot11SSIDList->uTotalNumOfEntries = 1;
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;            
            break;
        }

        NdisMoveMemory(
            pDot11SSIDList->SSIDs, 
            &(pStation->Config.SSID),
            sizeof(DOT11_SSID)
            );
        pDot11SSIDList->uNumOfEntries = 1;
        pDot11SSIDList->uTotalNumOfEntries = 1;

        ndisStatus = NDIS_STATUS_SUCCESS;
    }while (FALSE);
    
    return ndisStatus;
}

NDIS_STATUS
Sta11QueryExcludedMACAddressList(
    __in  PSTATION        pStation,
    __out_bcount(TotalLength) PDOT11_MAC_ADDRESS_LIST pDot11MacAddrList,
    __in __range(sizeof(DOT11_MAC_ADDRESS_LIST) - sizeof(DOT11_MAC_ADDRESS), 0x7FFFFFFF)  ULONG           TotalLength
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    ULONG       BytesNeeded = 0;
    do
    {
        //
        // Ensure enough space to return complete list
        //
        BytesNeeded = pStation->Config.ExcludedMACAddressCount 
                            * sizeof(DOT11_MAC_ADDRESS)
                        + FIELD_OFFSET(DOT11_MAC_ADDRESS_LIST, MacAddrs);
                        
        if (TotalLength < BytesNeeded)
        {
            pDot11MacAddrList->uNumOfEntries = 0;
            pDot11MacAddrList->uTotalNumOfEntries = pStation->Config.ExcludedMACAddressCount;
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }

        NdisMoveMemory(
            pDot11MacAddrList->MacAddrs, 
            &(pStation->Config.ExcludedMACAddressList),
            sizeof(DOT11_MAC_ADDRESS) * pStation->Config.ExcludedMACAddressCount
            );

        pDot11MacAddrList->uNumOfEntries = pStation->Config.ExcludedMACAddressCount;
        pDot11MacAddrList->uTotalNumOfEntries = pStation->Config.ExcludedMACAddressCount;

        ndisStatus = NDIS_STATUS_SUCCESS;
    }while (FALSE);
    
    return ndisStatus;
}

NDIS_STATUS
Sta11QueryDesiredBSSIDList(
    __in  PSTATION        pStation,
    __out_bcount(TotalLength) PDOT11_BSSID_LIST pDot11BSSIDList,
    __in __range(sizeof(DOT11_BSSID_LIST) - sizeof(DOT11_MAC_ADDRESS), 0x7FFFFFFF) ULONG           TotalLength
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    ULONG       BytesNeeded = 0;
    do
    {
        //
        // Ensure enough space to return complete list
        //
        BytesNeeded = pStation->Config.DesiredBSSIDCount
                            * sizeof(DOT11_MAC_ADDRESS)
                        + FIELD_OFFSET(DOT11_BSSID_LIST, BSSIDs);
                        
        if (TotalLength < BytesNeeded)
        {
            pDot11BSSIDList->uNumOfEntries = 0;
            pDot11BSSIDList->uTotalNumOfEntries = pStation->Config.DesiredBSSIDCount;
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }

        NdisMoveMemory(
            pDot11BSSIDList->BSSIDs, 
            &(pStation->Config.DesiredBSSIDList),
            sizeof(DOT11_MAC_ADDRESS) * pStation->Config.DesiredBSSIDCount
            );

        pDot11BSSIDList->uNumOfEntries = pStation->Config.DesiredBSSIDCount;
        pDot11BSSIDList->uTotalNumOfEntries = pStation->Config.DesiredBSSIDCount;

        ndisStatus = NDIS_STATUS_SUCCESS;
    }while (FALSE);
    
    return ndisStatus;
}


NDIS_STATUS
Sta11QueryEnabledAuthenticationAlgorithm(
    __in  PSTATION        pStation,
    __out PDOT11_AUTH_ALGORITHM_LIST  pAuthAlgoList,
    __in  ULONG           TotalLength
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    ULONG       BytesNeeded = 0;

    do
    {
        // Ensure enough space for one entry (though this would
        // get saved as part of the DOT11_AUTH_ALGORITHM_LIST structure
        // itself)

        BytesNeeded =  FIELD_OFFSET(DOT11_AUTH_ALGORITHM_LIST, AlgorithmIds)
                        + 1 * sizeof(DOT11_AUTH_ALGORITHM);
        
        if (TotalLength < BytesNeeded)
        {
            pAuthAlgoList->uNumOfEntries = 0;
            pAuthAlgoList->uTotalNumOfEntries = 1;
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;            
            break;
        }

        pAuthAlgoList->uNumOfEntries = 1;
        pAuthAlgoList->uTotalNumOfEntries = 1;
        
        pAuthAlgoList->AlgorithmIds[0] = pStation->Config.AuthAlgorithm;

    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
Sta11QueryEnabledMulticastCipherAlgorithm(
    __in  PSTATION        pStation,
    __out PDOT11_CIPHER_ALGORITHM_LIST  pAuthCipherList,
    __in  ULONG           TotalLength
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    ULONG       BytesNeeded = 0;

    UNREFERENCED_PARAMETER(pStation);
    
    do
    {
        // Integer overflow
        if (FIELD_OFFSET(DOT11_CIPHER_ALGORITHM_LIST, AlgorithmIds) > 
                FIELD_OFFSET(DOT11_CIPHER_ALGORITHM_LIST, AlgorithmIds) + 
                pStation->Config.MulticastCipherAlgorithmCount * sizeof(DOT11_CIPHER_ALGORITHM))
        {
            return NDIS_STATUS_FAILURE;
        }

        // Ensure enough space for one entry (though this would
        // get saved as part of the DOT11_CIPHER_ALGORITHM_LIST structure
        // itself)

        BytesNeeded = FIELD_OFFSET(DOT11_CIPHER_ALGORITHM_LIST, AlgorithmIds) +
                      pStation->Config.MulticastCipherAlgorithmCount * sizeof(DOT11_CIPHER_ALGORITHM);
        
        if (TotalLength < BytesNeeded)
        {
            pAuthCipherList->uNumOfEntries = 0;
            pAuthCipherList->uTotalNumOfEntries = pStation->Config.MulticastCipherAlgorithmCount;
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;            
            break;
        }
        
        pAuthCipherList->uNumOfEntries = pStation->Config.MulticastCipherAlgorithmCount;
        pAuthCipherList->uTotalNumOfEntries = pStation->Config.MulticastCipherAlgorithmCount;
        
        NdisMoveMemory(pAuthCipherList->AlgorithmIds,
                       pStation->Config.MulticastCipherAlgorithmList,
                       pStation->Config.MulticastCipherAlgorithmCount * sizeof(DOT11_CIPHER_ALGORITHM));

    } while(FALSE);

    return ndisStatus;
}
    

VOID
Sta11Cleanup80211Attributes(
    __in PSTATION        pStation,
    __in PNDIS_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES attr
    )
{
    UNREFERENCED_PARAMETER(pStation);

    if (attr->ExtSTAAttributes)
    {
        if (attr->ExtSTAAttributes->pSupportedCountryOrRegionStrings)
        {
            MP_FREE_MEMORY(attr->ExtSTAAttributes->pSupportedCountryOrRegionStrings);
        }
        if (attr->ExtSTAAttributes->pInfraSupportedUcastAlgoPairs)
        {
            MP_FREE_MEMORY(attr->ExtSTAAttributes->pInfraSupportedUcastAlgoPairs);
        }

        if (attr->ExtSTAAttributes->pInfraSupportedMcastAlgoPairs)
        {
            MP_FREE_MEMORY(attr->ExtSTAAttributes->pInfraSupportedMcastAlgoPairs);
        }

        if (attr->ExtSTAAttributes->pAdhocSupportedUcastAlgoPairs)
        {
            MP_FREE_MEMORY(attr->ExtSTAAttributes->pAdhocSupportedUcastAlgoPairs);
        }

        if (attr->ExtSTAAttributes->pAdhocSupportedMcastAlgoPairs)
        {
            MP_FREE_MEMORY(attr->ExtSTAAttributes->pAdhocSupportedMcastAlgoPairs);
        }

        MP_FREE_MEMORY(attr->ExtSTAAttributes);
    }
}

NDIS_STATUS
Sta11QueryDataRateMappingTable(
    __in  PSTATION        pStation,
    __out PDOT11_DATA_RATE_MAPPING_TABLE  pDataRateMappingTable,
    __in  ULONG           TotalLength
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    ULONG       BytesNeeded = 0;

    static DOT11_DATA_RATE_MAPPING_ENTRY Std_abg_DataRateMappingTable[] = 
    {
        {2, 0, 2},
        {4, 0, 4},
        {6, 0, 6},
        {9, 0, 9},
        {11, 0, 11},
        {12, 0, 12},
        {18, 0, 18},
        {22, 0, 22},
        {24, 0, 24},
        {36, 0, 36},
        {44, 0, 44},
        {48, 0, 48},
        {54, 0, 54},
        {66, 0, 66},
        {72, 0, 72},
        {96, 0, 96},
        {108, 0, 108}
    };
    
    do
    {
        //
        // If vendor provides its own mapping table, use it. Otherwise, use our standard a/b/g table.
        //
        ndisStatus = Hw11QueryDataRateMappingTable(pStation->pNic, pDataRateMappingTable, TotalLength);
        if (ndisStatus != NDIS_STATUS_NOT_SUPPORTED)
            break;

        //
        // Use our map. Initialize the table.
        //
        pDataRateMappingTable->uDataRateMappingLength = sizeof(Std_abg_DataRateMappingTable) / 
                                                        sizeof(DOT11_DATA_RATE_MAPPING_ENTRY);

        // Ensure enough space 
        BytesNeeded = FIELD_OFFSET(DOT11_DATA_RATE_MAPPING_TABLE, DataRateMappingEntries) +
                      sizeof(Std_abg_DataRateMappingTable);
        
        if (TotalLength < BytesNeeded)
        {
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;            
            break;
        }

        //
        // Copy the standard a/b/g data rate mapping table.
        //
        NdisMoveMemory(pDataRateMappingTable->DataRateMappingEntries,
                       Std_abg_DataRateMappingTable,
                       sizeof(Std_abg_DataRateMappingTable));

        ndisStatus = NDIS_STATUS_SUCCESS;
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
Sta11QueryCipherDefaultKeyID(
    __in   PSTATION pStation,
    __in   PULONG keyID
    )
{
    *keyID = Hw11GetDefaultKeyID(pStation->pNic);
    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Sta11QueryExcludeUnencrypted(
    __in   PSTATION pStation,
    __in   PBOOLEAN param
    )
{
    *param = pStation->Config.ExcludeUnencrypted;
    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Sta11QueryPrivacyExemptionList(
    __in   PSTATION                      pStation,
    __out_bcount(TotalLength) PDOT11_PRIVACY_EXEMPTION_LIST list,
    __in  __range(sizeof(DOT11_PRIVACY_EXEMPTION_LIST) - sizeof(DOT11_PRIVACY_EXEMPTION), 0x7FFFFFFF) ULONG                         TotalLength
    )
{
    PDOT11_PRIVACY_EXEMPTION_LIST   PrivacyExemptionList = pStation->Config.PrivacyExemptionList;

    //
    // If we don't have privacy exemption list, simply return success with number of entries set to 0.
    //

    list->uNumOfEntries = 0;
    if (!PrivacyExemptionList || !PrivacyExemptionList->uNumOfEntries) {
        list->uTotalNumOfEntries = 0;
        return NDIS_STATUS_SUCCESS;
    }

    // Integer overflow
    if (FIELD_OFFSET(DOT11_PRIVACY_EXEMPTION_LIST, PrivacyExemptionEntries) > 
            FIELD_OFFSET(DOT11_PRIVACY_EXEMPTION_LIST, PrivacyExemptionEntries) + 
            PrivacyExemptionList->uNumOfEntries * sizeof(DOT11_PRIVACY_EXEMPTION))
    {
        return NDIS_STATUS_FAILURE;
    }

    //
    // Check if we have enough space to copy all lists. If not, simply fail the request.
    // we don't copy partial list.
    //

    list->uTotalNumOfEntries = PrivacyExemptionList->uNumOfEntries;
    if (TotalLength < FIELD_OFFSET(DOT11_PRIVACY_EXEMPTION_LIST, PrivacyExemptionEntries) +
                      PrivacyExemptionList->uNumOfEntries * sizeof(DOT11_PRIVACY_EXEMPTION))
    {
        return NDIS_STATUS_BUFFER_OVERFLOW;
    }

    //
    // We have enough space, copy all lists.
    //

    list->uNumOfEntries = PrivacyExemptionList->uNumOfEntries;
    NdisMoveMemory(list->PrivacyExemptionEntries,
                   PrivacyExemptionList->PrivacyExemptionEntries,
                   PrivacyExemptionList->uNumOfEntries * sizeof(DOT11_PRIVACY_EXEMPTION));

    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Sta11QueryPowerMgmtRequest(
    __in   PSTATION pStation,
    __in   PULONG PowerSavingLevel
    )
{
    *PowerSavingLevel = pStation->Config.PowerSavingLevel;
    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS 
Sta11QueryIBSSParameters(
    __in  PSTATION pStation,
    __in  PVOID InformationBuffer,
    __in  ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    PDOT11_IBSS_PARAMS  pDot11IBSSParams = (PDOT11_IBSS_PARAMS)InformationBuffer;
    
    *BytesNeeded = sizeof(DOT11_IBSS_PARAMS) + pStation->Config.AdditionalIESize;
    if (InformationBufferLength < *BytesNeeded)
    {
        *BytesWritten = 0;
        return NDIS_STATUS_BUFFER_OVERFLOW;
    }

    pDot11IBSSParams->bJoinOnly = pStation->Config.IBSSJoinOnly;
    pDot11IBSSParams->uIEsLength = pStation->Config.AdditionalIESize;
    pDot11IBSSParams->uIEsOffset = sizeof(DOT11_IBSS_PARAMS);

    if (pStation->Config.AdditionalIESize > 0)
    {
        NdisMoveMemory(Add2Ptr(pDot11IBSSParams, sizeof(DOT11_IBSS_PARAMS)),
                       pStation->Config.AdditionalIEData,
                       pStation->Config.AdditionalIESize);
    }                  

    *BytesWritten = *BytesNeeded;
    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Sta11QueryHardwarePhyState(
    __in  PSTATION        pStation,
    __out PBOOLEAN        pState
    )
{
    *pState = Hw11QueryHardwarePhyState(pStation->pNic);
    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Sta11QueryDesiredPhyList(
    __in  PSTATION        pStation,
    __out PDOT11_PHY_ID_LIST  PhyIdList,
    __in  ULONG           TotalLength
    )
{
    PhyIdList->uTotalNumOfEntries = pStation->Config.DesiredPhyCount;
    PhyIdList->uNumOfEntries = 0;

    //
    // Integer overflow check
    //
    if (FIELD_OFFSET(DOT11_PHY_ID_LIST, dot11PhyId) > 
            FIELD_OFFSET(DOT11_PHY_ID_LIST, dot11PhyId) + 
            pStation->Config.DesiredPhyCount * sizeof(ULONG))
    {
        return NDIS_STATUS_FAILURE;
    }

    //
    // If the buffer is not big enough, simply return error.
    //
    if (TotalLength < 
        FIELD_OFFSET(DOT11_PHY_ID_LIST, dot11PhyId) + pStation->Config.DesiredPhyCount * sizeof(ULONG))
    {
        return NDIS_STATUS_BUFFER_OVERFLOW;
    }

    //
    // Copy the desired PHY list.
    //
    PhyIdList->uNumOfEntries = pStation->Config.DesiredPhyCount;
    NdisMoveMemory(PhyIdList->dot11PhyId,
                   pStation->Config.DesiredPhyList,
                   pStation->Config.DesiredPhyCount * sizeof(ULONG));

    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Sta11QueryActivePhyList(
    __in  PSTATION        pStation,
    __out PDOT11_PHY_ID_LIST  PhyIdList,
    __in  ULONG           TotalLength
    )
{
    //
    // Our NIC only supports one active PHY at a time.
    //
    PhyIdList->uTotalNumOfEntries = 1;

    //
    // If the buffer is not big enough, simply return error.
    //
    if (TotalLength < FIELD_OFFSET(DOT11_PHY_ID_LIST, dot11PhyId) + PhyIdList->uTotalNumOfEntries * sizeof(ULONG))
    {
        PhyIdList->uNumOfEntries = 0;
        return NDIS_STATUS_BUFFER_OVERFLOW;
    }

    //
    // Copy the desired PHY list.
    //
    PhyIdList->uNumOfEntries = 1;
    PhyIdList->dot11PhyId[0] = pStation->Config.ActivePhyId;

    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Sta11QueryCurrentPhyId(
    __in  PSTATION        pStation,
    __in  PULONG          PhyId
    )
{
    *PhyId = Hw11QuerySelectedPhyId(pStation->pNic);
    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Sta11QueryPMKIDList(
    __in  PSTATION        pStation,
    __out_bcount(TotalLength) PDOT11_PMKID_LIST  PMKIDList,
    __in __range(sizeof(DOT11_PMKID_LIST) - sizeof(DOT11_PMKID_ENTRY), 0x7FFFFFFF) ULONG           TotalLength
    )
{
    PMKIDList->uTotalNumOfEntries = pStation->Config.PMKIDCount;
    PMKIDList->uNumOfEntries = 0;

    //
    // Integer overflow check
    //
    if (FIELD_OFFSET(DOT11_PMKID_LIST, PMKIDs) > 
            FIELD_OFFSET(DOT11_PMKID_LIST, PMKIDs) + 
            pStation->Config.PMKIDCount * sizeof(DOT11_PMKID_ENTRY))
    {
        return NDIS_STATUS_FAILURE;
    }

    //
    // If the buffer is not big enough, simply return error.
    //
    if (TotalLength < FIELD_OFFSET(DOT11_PMKID_LIST, PMKIDs) + pStation->Config.PMKIDCount * sizeof(DOT11_PMKID_ENTRY))
    {

        return NDIS_STATUS_BUFFER_OVERFLOW;
    }

    //
    // Copy the PMKID list.
    //
    PMKIDList->uNumOfEntries = pStation->Config.PMKIDCount;
    NdisMoveMemory(PMKIDList->PMKIDs,
                   pStation->Config.PMKIDList,
                   pStation->Config.PMKIDCount * sizeof(DOT11_PMKID_ENTRY));

    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Sta11QueryUnreachableDetectionThreshold(
    __in  PSTATION        pStation,
    __in  PULONG          threshold
    )
{
    *threshold = pStation->Config.UnreachableDetectionThreshold;
    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Sta11QueryUnicastUseGroupOption(
    __in  PSTATION        pStation,
    __out PBOOLEAN        unicastUseGroupEnabled
    )
{
    *unicastUseGroupEnabled = pStation->Config.UnicastUseGroupEnabled;
    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Sta11QuerySafeModeOption(
    __in  PSTATION        pStation,
    __out PBOOLEAN        safeModeEnabled
    )
{
    *safeModeEnabled = Hw11GetSafeModeOption(pStation->pNic);
    return NDIS_STATUS_SUCCESS;
}


NDIS_STATUS
Sta11QueryHiddenNetworkOption(
    __in  PSTATION        pStation,
    __out PBOOLEAN        hiddenNetworkEnabled
    )
{
    *hiddenNetworkEnabled = pStation->Config.HiddenNetworkEnabled;
    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Sta11QueryMediaStreamingOption(
    __in  PSTATION        pStation,
    __out PBOOLEAN        mediaStreamEnabled
    )
{
    *mediaStreamEnabled = pStation->Config.MediaStreamingEnabled;
    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Sta11QueryDot11Statistics(
    __in  PSTATION        pStation,
    __in PDOT11_STATISTICS pDot11Stats,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    )
{
    NDIS_STATUS     status;
    
    status = Hw11QueryDot11Statistics(
            pStation->pNic,
            pDot11Stats,
            InformationBufferLength,
            BytesWritten,
            BytesNeeded
            );

    if (status == NDIS_STATUS_SUCCESS)
    {
        // Excluded counts are kept by the station
        pDot11Stats->MacUcastCounters.ullWEPExcludedCount = pStation->Statistics.ullUcastWEPExcludedCount;
        pDot11Stats->MacMcastCounters.ullWEPExcludedCount = pStation->Statistics.ullMcastWEPExcludedCount;
    }
    return status;
}

NDIS_STATUS
Sta11SetDesiredBSSType(
    __in  PSTATION        pStation,
    __in  DOT11_BSS_TYPE  bssType
    )
{
    //
    // For our implementation, the BSS type any corresponds to
    // infrastructure
    //
    if (bssType == dot11_BSS_type_any)
        bssType = dot11_BSS_type_infrastructure;
    
    if (pStation->Config.BSSType != bssType)
    {
        pStation->Config.BSSType = bssType;

        Hw11SetCurrentBSSType(pStation->pNic, bssType);

        if (pStation->Config.PowerSavingLevel != DOT11_POWER_SAVING_NO_POWER_SAVING)
        {
            //
            // If the new type is infra, we need to make sure the configured power
            // saving level is in agreement with what's in HW layer. If the new type
            // is IBSS, we need to make sure power saving mode is off in HW layer.
            //
            StaSetPowerSavingLevel(pStation, 
                                   bssType == dot11_BSS_type_infrastructure ?
                                       pStation->Config.PowerSavingLevel :
                                       DOT11_POWER_SAVING_NO_POWER_SAVING);
        }

        //
        // Set default auth and cipher algorithms based on the new bss type
        //
        StaSetDefaultAuthAlgo(pStation);
    }

    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Sta11SetOperationMode(
    __in  PSTATION        pStation,
    __in  PDOT11_CURRENT_OPERATION_MODE currentOperationMode
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    if (currentOperationMode->uCurrentOpMode == DOT11_OPERATION_MODE_NETWORK_MONITOR)
    {
        //
        // Turn off periodic scanning in netmon mode
        //
        StaStopPeriodicScan(pStation);
    }
        
    ndisStatus = Hw11SetOperationMode(
        pStation->pNic,
        currentOperationMode
        );
    
    return ndisStatus;
}


NDIS_STATUS
Sta11SetDesiredSSIDList(
    __in  PSTATION        pStation,
    __in  PDOT11_SSID_LIST    pDot11SSIDList,
    __out PULONG          pBytesRead
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    do
    {
        // Only support Single SSID (also reported in MAX table
        // size)
        if (pDot11SSIDList->uNumOfEntries != 1)
        {
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }

        // Copy the data locally
        NdisMoveMemory(&(pStation->Config.SSID),
            &(pDot11SSIDList->SSIDs[0]),
            sizeof(DOT11_SSID)
            );

        *pBytesRead =  FIELD_OFFSET(DOT11_SSID_LIST, SSIDs)
                        + 1 * sizeof(DOT11_SSID);

        // We dont need to process anything just yet
        // store it and we will use it when the connect
        // request comes in
        
        ndisStatus = NDIS_STATUS_SUCCESS;
    }while (FALSE);
    
    return ndisStatus;
}

NDIS_STATUS
Sta11SetExcludedMACAddressList(
    __in  PSTATION        pStation,
    __in  PDOT11_MAC_ADDRESS_LIST pDot11MacAddrList,
    __out PULONG          pBytesRead
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PSTA_INFRA_CONNECT_CONTEXT    pConnectContext = &(pStation->ConnectContext);
    PSTA_BSS_ENTRY      pAPEntry = NULL;
    ULONG               i;
    PSTA_CURRENT_CONFIG pConfig = &(pStation->Config);
    BOOLEAN             bDisconnect = FALSE;

    do
    {
        // Can only support STA_EXCLUDED_MAC_ADDRESS_MAX_COUNT
        // MAC addresses
        //
        if (pDot11MacAddrList->uNumOfEntries > STA_EXCLUDED_MAC_ADDRESS_MAX_COUNT)
        {
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }
        pConfig->IgnoreAllMACAddresses = FALSE;

        NdisMoveMemory(&(pConfig->ExcludedMACAddressList),
            &(pDot11MacAddrList->MacAddrs[0]),
            pDot11MacAddrList->uNumOfEntries * sizeof(DOT11_MAC_ADDRESS)
            );
        pConfig->ExcludedMACAddressCount = pDot11MacAddrList->uNumOfEntries;

        //
        // If only the broadcast MAC is present, ignore all MAC addresses
        //
        if ((pDot11MacAddrList->uNumOfEntries == 1) &&
            (ETH_IS_BROADCAST(pDot11MacAddrList->MacAddrs[0]) == TRUE)
            )
        {
            pConfig->IgnoreAllMACAddresses = TRUE;
        }

        *pBytesRead =  FIELD_OFFSET(DOT11_MAC_ADDRESS_LIST, MacAddrs)
                        + pDot11MacAddrList->uNumOfEntries * sizeof(DOT11_MAC_ADDRESS);

        // If the access point we are currently
        // associated matches this MAC address, begin disassociation
        // process
                STA_INCREMENT_REF(pStation->ConnectContext.AsyncFuncCount);
        NdisAcquireSpinLock(&(pConnectContext->Lock));

        if ((pStation->ConnectContext.ConnectState == CONN_STATE_READY_TO_ROAM) &&
            (pStation->ConnectContext.AssociateState == ASSOC_STATE_ASSOCIATED)
            )
        {
        
            //
            // Get the AP entry
            //
            pAPEntry = pConnectContext->ActiveAP;
            MPVERIFY(pAPEntry != NULL);     // SYNC:

            // Check if our current BSSID is in the new excluded MAC address list
            if (pConfig->IgnoreAllMACAddresses)
            {
                bDisconnect = TRUE;
            }
            else
            {
                bDisconnect = FALSE;        

                // Walk through the excluded MAC address list
                for (i = 0; i < pConfig->ExcludedMACAddressCount; i++)
                {
                    if (MP_COMPARE_MAC_ADDRESS(pAPEntry->MacAddress,
                            pConfig->ExcludedMACAddressList[i]) == TRUE)
                    {
                        bDisconnect = TRUE;
                    }
                }
            }
            
            if (bDisconnect)
            {
                MpTrace(COMP_ASSOC, DBG_LOUD, (" - Disconnecting from excluded MAC AP\n"));
                pConnectContext->ActiveAP = NULL;

                //
                // Set state to disconnected. Then, when we wake up, we would
                // perform a roam
                //
                pConnectContext->AssociateState = ASSOC_STATE_NOT_ASSOCIATED;

                NdisReleaseSpinLock(&(pConnectContext->Lock));
                MpTrace(COMP_ASSOC, DBG_LOUD, ("Low power with Active connection\n"));

                pAPEntry->AssocState = dot11_assoc_state_unauth_unassoc;    

                StaDisconnect(
                    pStation, 
                    pAPEntry, 
                    ASSOC_STATE_ASSOCIATED
                    );
                
                // Release the ref on the AP entry
                STA_DECREMENT_REF(pAPEntry->RefCount);
            }
            else
            {
                NdisReleaseSpinLock(&(pConnectContext->Lock));                
            }
        }
        else
        {
            NdisReleaseSpinLock(&(pConnectContext->Lock));
        }

        STA_DECREMENT_REF(pStation->ConnectContext.AsyncFuncCount);


        ndisStatus = NDIS_STATUS_SUCCESS;        
    }while (FALSE);
    
    return ndisStatus;
}

NDIS_STATUS
Sta11SetDesiredBSSIDList(
    __in  PSTATION        pStation,
    __in  PDOT11_BSSID_LIST   pDot11BSSIDList,
    __out PULONG          pBytesRead
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    do
    {
        // Can only support STA_DESIRED_BSSID_MAX_COUNT
        // BSSIDs
        //
        if (pDot11BSSIDList->uNumOfEntries > STA_DESIRED_BSSID_MAX_COUNT)
        {
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }
        pStation->Config.AcceptAnyBSSID = FALSE;

        NdisMoveMemory(&(pStation->Config.DesiredBSSIDList),
            &(pDot11BSSIDList->BSSIDs[0]),
            pDot11BSSIDList->uNumOfEntries * sizeof(DOT11_MAC_ADDRESS)
            );

        pStation->Config.DesiredBSSIDCount = pDot11BSSIDList->uNumOfEntries;

        //
        // If only the broadcast MAC is present, accept any BSSID
        //
        if ((pDot11BSSIDList->uNumOfEntries == 1) &&
            (ETH_IS_BROADCAST(pDot11BSSIDList->BSSIDs[0]) == TRUE)
            )
        {
            pStation->Config.AcceptAnyBSSID = TRUE;
        }

        *pBytesRead =  FIELD_OFFSET(DOT11_BSSID_LIST, BSSIDs)
                        + pDot11BSSIDList->uNumOfEntries * sizeof(DOT11_MAC_ADDRESS);

        ndisStatus = NDIS_STATUS_SUCCESS;        
    }while (FALSE);
    
    return ndisStatus;
}

NDIS_STATUS
Sta11SetEnabledAuthenticationAlgorithm(
    __in  PSTATION        pStation,
    __in  PDOT11_AUTH_ALGORITHM_LIST  pAuthAlgoList,
    __out PULONG          pBytesRead
    )
{
    PNIC        pNic = pStation->pNic;
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
    __try
    {
        // Only support one authentication algorithm
        if (pAuthAlgoList->uNumOfEntries != 1)
        {
            *pBytesRead = FIELD_OFFSET(DOT11_AUTH_ALGORITHM_LIST, AlgorithmIds);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            __leave;
        }

        *pBytesRead = FIELD_OFFSET(DOT11_AUTH_ALGORITHM_LIST, AlgorithmIds) +
                      1 * sizeof(DOT11_AUTH_ALGORITHM);

        // 
        // Check if we support the specified auth algorithm.
        //
        switch (pAuthAlgoList->AlgorithmIds[0])
        {
            case DOT11_AUTH_ALGO_80211_OPEN:
                break;

            case DOT11_AUTH_ALGO_80211_SHARED_KEY:
                if (!Hw11WEP104Implemented(pNic) && !Hw11WEP40Implemented(pNic) ||
                    pStation->Config.BSSType == dot11_BSS_type_independent)
                {
                    ndisStatus = NDIS_STATUS_INVALID_DATA;
                    __leave;
                }

                break;

            case DOT11_AUTH_ALGO_WPA:
            case DOT11_AUTH_ALGO_WPA_PSK:
            case DOT11_AUTH_ALGO_RSNA:
                if (pStation->Config.BSSType == dot11_BSS_type_independent)
                {
                    ndisStatus = NDIS_STATUS_INVALID_DATA;
                    __leave;
                }

                // fall through

            case DOT11_AUTH_ALGO_RSNA_PSK:
                if (!Hw11CCMPImplemented(pNic) && 
                    (!Hw11TKIPImplemented(pNic) || pStation->Config.BSSType == dot11_BSS_type_independent))
                {
                    ndisStatus = NDIS_STATUS_INVALID_DATA;
                    __leave;
                }

                break;

            default:
                ndisStatus = NDIS_STATUS_INVALID_DATA;
                __leave;
        }

        // Copy the data locally
        if (pStation->Config.AuthAlgorithm != pAuthAlgoList->AlgorithmIds[0])
        {
            pStation->Config.AuthAlgorithm = pAuthAlgoList->AlgorithmIds[0];

            // reload enabled unicast and multicast cipher based on current bss type and auth algo.
            StaSetDefaultCipher(pStation);
        }
    
        //
        // Tell HW layer of the auth algorithm
        //
        Hw11SetAuthentication(pStation->pNic, pStation->Config.AuthAlgorithm);

        // We dont need to process anything just yet
        // store it and we will use it when the connect
        // request comes in        
        ndisStatus = NDIS_STATUS_SUCCESS;
    } 
    __finally
    {
    }

    return ndisStatus;
}

BOOLEAN
StaValidateUnicastAuthCipherPair(
    __in PSTATION pStation,
    __in DOT11_AUTH_ALGORITHM AuthAlgo,
    __in DOT11_CIPHER_ALGORITHM CipherAlgo
    )
{
    PNIC                    pNic = pStation->pNic;
    BOOLEAN                 WEP40Implemented = Hw11WEP40Implemented(pNic);
    BOOLEAN                 WEP104Implemented = Hw11WEP104Implemented(pNic);
    BOOLEAN                 TKIPImplemented = Hw11TKIPImplemented(pNic);
    BOOLEAN                 CCMPImplemented = Hw11CCMPImplemented(pNic);

    switch (AuthAlgo)
    {
        case DOT11_AUTH_ALGO_80211_OPEN:
            return (BOOLEAN)((CipherAlgo == DOT11_CIPHER_ALGO_WEP && (WEP40Implemented || WEP104Implemented)) ||
                             (CipherAlgo == DOT11_CIPHER_ALGO_WEP104 && WEP104Implemented) ||
                             (CipherAlgo == DOT11_CIPHER_ALGO_WEP40 && WEP40Implemented) ||
                             (CipherAlgo == DOT11_CIPHER_ALGO_NONE));

        case DOT11_AUTH_ALGO_80211_SHARED_KEY:
            return (BOOLEAN)((CipherAlgo == DOT11_CIPHER_ALGO_WEP && (WEP40Implemented || WEP104Implemented)) ||
                             (CipherAlgo == DOT11_CIPHER_ALGO_WEP104 && WEP104Implemented) ||
                             (CipherAlgo == DOT11_CIPHER_ALGO_WEP40 && WEP40Implemented));

        case DOT11_AUTH_ALGO_WPA:
        case DOT11_AUTH_ALGO_WPA_PSK:
        case DOT11_AUTH_ALGO_RSNA:
        case DOT11_AUTH_ALGO_RSNA_PSK:
            return (BOOLEAN)((CipherAlgo == DOT11_CIPHER_ALGO_TKIP && TKIPImplemented) ||
                             (CipherAlgo == DOT11_CIPHER_ALGO_CCMP && CCMPImplemented));

        default:
            ASSERT(0);
            return FALSE;
    }
}

BOOLEAN
StaValidateMulticastAuthCipherPair(
    __in PSTATION pStation,
    __in DOT11_AUTH_ALGORITHM AuthAlgo,
    __in DOT11_CIPHER_ALGORITHM CipherAlgo
    )
{
    PNIC                    pNic = pStation->pNic;
    BOOLEAN                 WEP40Implemented = Hw11WEP40Implemented(pNic);
    BOOLEAN                 WEP104Implemented = Hw11WEP104Implemented(pNic);
    BOOLEAN                 TKIPImplemented = Hw11TKIPImplemented(pNic);
    BOOLEAN                 CCMPImplemented = Hw11CCMPImplemented(pNic);

    switch (AuthAlgo)
    {
        case DOT11_AUTH_ALGO_80211_OPEN:
            return (BOOLEAN)((CipherAlgo == DOT11_CIPHER_ALGO_WEP && (WEP40Implemented || WEP104Implemented)) ||
                             (CipherAlgo == DOT11_CIPHER_ALGO_WEP104 && WEP104Implemented) ||
                             (CipherAlgo == DOT11_CIPHER_ALGO_WEP40 && WEP40Implemented) ||
                             (CipherAlgo == DOT11_CIPHER_ALGO_NONE));

        case DOT11_AUTH_ALGO_80211_SHARED_KEY:
            return (BOOLEAN)((CipherAlgo == DOT11_CIPHER_ALGO_WEP && (WEP40Implemented || WEP104Implemented)) ||
                             (CipherAlgo == DOT11_CIPHER_ALGO_WEP104 && WEP104Implemented) ||
                             (CipherAlgo == DOT11_CIPHER_ALGO_WEP40 && WEP40Implemented));

        case DOT11_AUTH_ALGO_WPA:
        case DOT11_AUTH_ALGO_WPA_PSK:
        case DOT11_AUTH_ALGO_RSNA:
        case DOT11_AUTH_ALGO_RSNA_PSK:
            return (BOOLEAN)((CipherAlgo == DOT11_CIPHER_ALGO_TKIP && TKIPImplemented) ||
                             (CipherAlgo == DOT11_CIPHER_ALGO_CCMP && CCMPImplemented) ||
                             (CipherAlgo == DOT11_CIPHER_ALGO_WEP104 && WEP104Implemented) ||
                             (CipherAlgo == DOT11_CIPHER_ALGO_WEP40 && WEP40Implemented));

        default:
            ASSERT(0);
            return FALSE;
    }
}

NDIS_STATUS
Sta11SetEnabledUnicastCipherAlgorithm(
    __in  PSTATION        pStation,
    __in  PDOT11_CIPHER_ALGORITHM_LIST  pCipherAlgoList,
    __out PULONG          pBytesRead
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PNIC        pNic = pStation->pNic;
    
    __try 
    {

        // Only support one cipher algorithm
        if (pCipherAlgoList->uNumOfEntries != 1)
        {
            *pBytesRead = FIELD_OFFSET(DOT11_CIPHER_ALGORITHM_LIST, AlgorithmIds);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            __leave;
        }
        
        *pBytesRead = FIELD_OFFSET(DOT11_CIPHER_ALGORITHM_LIST, AlgorithmIds) +
                      1 * sizeof(DOT11_CIPHER_ALGORITHM);

        //
        // Check if we can support the cipher algorithms given current auth algorithm.
        //
        if (!StaValidateUnicastAuthCipherPair(pStation, 
                                              pStation->Config.AuthAlgorithm, 
                                              pCipherAlgoList->AlgorithmIds[0]))
        {
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            __leave;
        }

        Hw11SetEncryption(pNic, TRUE, pCipherAlgoList->AlgorithmIds[0]);

        // Copy the data locally
        pStation->Config.UnicastCipherAlgorithm = pCipherAlgoList->AlgorithmIds[0];
    
        ndisStatus = NDIS_STATUS_SUCCESS;
    } 
    __finally 
    {
    }

    return ndisStatus;
}

NDIS_STATUS
Sta11SetEnabledMulticastCipherAlgorithm(
    __in  PSTATION        pStation,
    __in  PDOT11_CIPHER_ALGORITHM_LIST  pCipherAlgoList,
    __out PULONG          pBytesRead
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PNIC        pNic = pStation->pNic;
    ULONG       index;

    __try 
    {

        // Only support no more than STA_MULTICAST_CIPHER_MAX_COUNT cipher algorithms
        if (pCipherAlgoList->uNumOfEntries > STA_MULTICAST_CIPHER_MAX_COUNT)
        {
            *pBytesRead = FIELD_OFFSET(DOT11_CIPHER_ALGORITHM_LIST, AlgorithmIds);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            __leave;
        }

        *pBytesRead = FIELD_OFFSET(DOT11_CIPHER_ALGORITHM_LIST, AlgorithmIds) +
                      pCipherAlgoList->uNumOfEntries * sizeof(DOT11_CIPHER_ALGORITHM);

        //
        // Check if we can support each of the cipher algorithms given current auth algorithm.
        //
        
        for (index = 0; index < pCipherAlgoList->uNumOfEntries; index++)
        {
            //
            // Check if we can support the cipher algorithms given current auth algorithm.
            //
            if (!StaValidateMulticastAuthCipherPair(pStation, 
                                                    pStation->Config.AuthAlgorithm, 
                                                    pCipherAlgoList->AlgorithmIds[index]))
            {
                ndisStatus = NDIS_STATUS_INVALID_DATA;
                __leave;
            }
        }

        //
        // If there is only one enabled multicast cipher, we known exactly what the 
        // multicast cipher will be. Program the hardware. Otherwise, we have to wait
        // until we know which multicast cipher will be used among those enabled.
        //
        if (pCipherAlgoList->uNumOfEntries == 1)
        {
            pStation->Config.MulticastCipherAlgorithm = pCipherAlgoList->AlgorithmIds[0];
            Hw11SetEncryption(pNic, FALSE, pCipherAlgoList->AlgorithmIds[0]);
        }
        else
        {
            pStation->Config.MulticastCipherAlgorithm = DOT11_CIPHER_ALGO_NONE;
            Hw11SetEncryption(pNic, FALSE, DOT11_CIPHER_ALGO_NONE);
        }

        // Copy the data locally
        pStation->Config.MulticastCipherAlgorithmCount = pCipherAlgoList->uNumOfEntries;
        for (index = 0; index < pCipherAlgoList->uNumOfEntries; index++)
        {
            pStation->Config.MulticastCipherAlgorithmList[index] = pCipherAlgoList->AlgorithmIds[index];
        }

        ndisStatus = NDIS_STATUS_SUCCESS;
    } 
    __finally 
    {
    }

    return ndisStatus;
}

NDIS_STATUS
Sta11SetCipherDefaultKeyID(
    __in   PSTATION pStation,
    __in   ULONG keyID
    )
{
    //
    // Check the KeyID range
    //
    
    if (keyID >= Hw11DefaultKeyTableSize(pStation->pNic)) 
    {
        return NDIS_STATUS_INVALID_DATA;
    }
    else
    {
        Hw11SetDefaultKeyID(pStation->pNic, keyID);
        return NDIS_STATUS_SUCCESS;
    }
}

NDIS_STATUS
Sta11SetCipherDefaultKey(
    __in   PSTATION pStation,
    __in   PDOT11_CIPHER_DEFAULT_KEY_VALUE defaultKey
    )
{
    PNIC pNic = pStation->pNic;

    //
    // Check the validity of the defaultKey
    //

    if (defaultKey->uKeyIndex >= Hw11DefaultKeyTableSize(pNic)) 
        return NDIS_STATUS_INVALID_DATA;

    //
    // Check cipher algorithm
    //

    switch (defaultKey->AlgorithmId)
    {
        case DOT11_CIPHER_ALGO_CCMP:
            if (!Hw11CCMPImplemented(pNic))
                return NDIS_STATUS_INVALID_DATA;

            break;

        case DOT11_CIPHER_ALGO_TKIP:
            if (!Hw11TKIPImplemented(pNic))
                return NDIS_STATUS_INVALID_DATA;

            break;

        case DOT11_CIPHER_ALGO_WEP:
            if (!Hw11WEP40Implemented(pNic) && !Hw11WEP104Implemented(pNic))
                return NDIS_STATUS_INVALID_DATA;

            break;

        case DOT11_CIPHER_ALGO_WEP40:
            if (!Hw11WEP40Implemented(pNic) || defaultKey->usKeyLength != 40 / 8)
                return NDIS_STATUS_INVALID_DATA;

            break;

        case DOT11_CIPHER_ALGO_WEP104:
            if (!Hw11WEP104Implemented(pNic) || defaultKey->usKeyLength != 104 / 8)
                return NDIS_STATUS_INVALID_DATA;

            break;

        default:
            return NDIS_STATUS_INVALID_DATA;
    }

    //
    // Set HW default key
    //

    return Hw11SetDefaultKey(pNic, 
                             defaultKey->MacAddr,
                             defaultKey->uKeyIndex, 
                             defaultKey->bStatic,
                             defaultKey->AlgorithmId,
                             defaultKey->bDelete ? 0 : defaultKey->usKeyLength,
                             defaultKey->ucKey);
}

NDIS_STATUS
Sta11SetCipherKeyMappingKey(
    __in  PSTATION pStation,
    __in  PDOT11_CIPHER_KEY_MAPPING_KEY_VALUE pKeyMappingKeys,
    __in  ULONG TotalLength
    )
{
    ULONG       size;
    NDIS_STATUS ndisStatus;
    PNIC        pNic = pStation->pNic;

    if (TotalLength == 0)
        return NDIS_STATUS_INVALID_DATA;

    while (TotalLength >= FIELD_OFFSET(DOT11_CIPHER_KEY_MAPPING_KEY_VALUE, ucKey))
    {
        size = FIELD_OFFSET(DOT11_CIPHER_KEY_MAPPING_KEY_VALUE, ucKey) + pKeyMappingKeys->usKeyLength;
        if (TotalLength < size)
        {
            return NDIS_STATUS_INVALID_DATA;
        }

        //
        // Check cipher algorithm
        //
        switch (pKeyMappingKeys->AlgorithmId)
        {
            case DOT11_CIPHER_ALGO_CCMP:
                if (!Hw11CCMPImplemented(pNic))
                    return NDIS_STATUS_INVALID_DATA;

                break;

            case DOT11_CIPHER_ALGO_TKIP:
                if (!Hw11TKIPImplemented(pNic))
                    return NDIS_STATUS_INVALID_DATA;

                break;

            case DOT11_CIPHER_ALGO_WEP:
                if (!Hw11WEP40Implemented(pNic) && !Hw11WEP104Implemented(pNic))
                    return NDIS_STATUS_INVALID_DATA;

                break;

            case DOT11_CIPHER_ALGO_WEP40:
                if (!Hw11WEP40Implemented(pNic) || pKeyMappingKeys->usKeyLength != 40 / 8)
                    return NDIS_STATUS_INVALID_DATA;

                break;

            case DOT11_CIPHER_ALGO_WEP104:
                if (!Hw11WEP104Implemented(pNic) || pKeyMappingKeys->usKeyLength != 104 / 8)
                    return NDIS_STATUS_INVALID_DATA;

                break;

            default:
                return NDIS_STATUS_INVALID_DATA;
        }

        ndisStatus = Hw11SetKeyMappingKey(pNic,
                                          pKeyMappingKeys->PeerMacAddr,
                                          pKeyMappingKeys->Direction,
                                          pKeyMappingKeys->bStatic,
                                          pKeyMappingKeys->AlgorithmId,
                                          pKeyMappingKeys->bDelete ? 0 : pKeyMappingKeys->usKeyLength,
                                          pKeyMappingKeys->ucKey);

        TotalLength -= size;
        pKeyMappingKeys = Add2Ptr(pKeyMappingKeys, size);
    }

    //
    // Return success only if all data are consumed.
    //
    return (TotalLength == 0) ? NDIS_STATUS_SUCCESS : NDIS_STATUS_INVALID_DATA;
}

NDIS_STATUS
Sta11SetExcludeUnencrypted(
    __in   PSTATION pStation,
    __in   BOOLEAN param
    )
{
    pStation->Config.ExcludeUnencrypted = param;
    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Sta11SetPrivacyExemptionList(
    __in   PSTATION                      pStation,
    __in   PDOT11_PRIVACY_EXEMPTION_LIST list,
    __out  PULONG                        pBytesRead
    )
{
    PDOT11_PRIVACY_EXEMPTION_LIST   PrivacyExemptionList = pStation->Config.PrivacyExemptionList;
    ULONG                           size;

    //
    // Check if we already have the buffer allocated for storing privacy exemption list.
    // If we don't, or if the buffer isn't big enough, allocate a buffer.
    //

    size = FIELD_OFFSET(DOT11_PRIVACY_EXEMPTION_LIST, PrivacyExemptionEntries) +
           list->uNumOfEntries * sizeof(DOT11_PRIVACY_EXEMPTION);

    if (!PrivacyExemptionList || 
        PrivacyExemptionList && PrivacyExemptionList->uTotalNumOfEntries < list->uNumOfEntries) 
    {
        MP_ALLOCATE_MEMORY(pStation->MiniportAdapterHandle, 
                           &PrivacyExemptionList,
                           size,
                           STA11_MEMORY_TAG);
        if (PrivacyExemptionList == NULL) 
        {
            *pBytesRead = FIELD_OFFSET(DOT11_PRIVACY_EXEMPTION_LIST, PrivacyExemptionEntries);
            return NDIS_STATUS_RESOURCES;
        }

        PrivacyExemptionList->uTotalNumOfEntries = list->uNumOfEntries;
    }

    //
    // Copy the new privacy exemption list
    //

    PrivacyExemptionList->uNumOfEntries = list->uNumOfEntries;
    if (list->uNumOfEntries)
    {
        NdisMoveMemory(PrivacyExemptionList->PrivacyExemptionEntries,
                       list->PrivacyExemptionEntries,
                       list->uNumOfEntries * sizeof(DOT11_PRIVACY_EXEMPTION));
    }

    //
    // If new buffer is allocated, free the existing buffer if any.
    //

    if (pStation->Config.PrivacyExemptionList && pStation->Config.PrivacyExemptionList != PrivacyExemptionList)
    {
        MP_FREE_MEMORY(pStation->Config.PrivacyExemptionList);
    }

    //
    // Set the new buffer as the one for the current privacy exemption list.
    //

    pStation->Config.PrivacyExemptionList = PrivacyExemptionList;

    *pBytesRead = size;
    return (NDIS_STATUS_SUCCESS);
}

NDIS_STATUS
Sta11SetPowerMgmtRequest(
    __in   PSTATION pStation,
    __in   ULONG PowerSavingLevel

    )
{
    //
    // Note: RNWIFI IM guarantees that we will not get POWER_MGMT_REQUEST OID
    //       when we are in the middle of a connection request. However, we 
    //       may get POWER_MGMT_REQUEST OID when we are in a scan or when
    //       we are roaming
    //

    //
    // We only support PS mode for IS. For IBSS, simply fake it: We accept
    // the request and pretend we go to the specified power saving level,
    // but internally the request does not reach HW layer.
    //

    if (pStation->Config.BSSType == dot11_BSS_type_independent)
    {
        pStation->Config.PowerSavingLevel = PowerSavingLevel;
        return NDIS_STATUS_SUCCESS;
    }
    else 
    {

        //
        // If we are performing an association, we dont actually change
        // power management state, just save new power management state. 
        // The connect complete routine would set it on hardware
        //
        if ((pStation->ConnectContext.ConnectState >= CONN_STATE_READY_TO_CONNECT) &&
            ((pStation->ConnectContext.AssociateState != ASSOC_STATE_NOT_ASSOCIATED) &&
             (pStation->ConnectContext.AssociateState != ASSOC_STATE_ASSOCIATED))
           )
        {
            pStation->Config.PowerSavingLevel = PowerSavingLevel;
            return NDIS_STATUS_SUCCESS;
        }
    
        return StaSetPowerSavingLevel(
            pStation,
            PowerSavingLevel
            );
    }
}

NDIS_STATUS
StaSetPowerSavingLevel(
    __in   PSTATION pStation,
    __in   ULONG PowerSavingLevel
    )
{
    DOT11_POWER_MGMT_MODE   mode;
    NDIS_STATUS             ndisStatus;

    // if device is in connected state and AP sent an invalid AID, 
    // we can't do power save. simplely return success here.
    // no need to configure power mgmt mode to no power saving as
    // it's already done in StaReceiveAssociationResponse when an 
    // invalid AID was received.
    if (pStation->ConnectContext.ConnectState == CONN_STATE_READY_TO_ROAM &&
        pStation->ConnectContext.AssociateState == ASSOC_STATE_ASSOCIATED &&
        pStation->Config.ValidAID == FALSE)
    {
        return NDIS_STATUS_SUCCESS;
    }

    //
    // Initialize the DOT11_POWER_MGMT_MODE structure depending on specified power level.
    //

    switch (PowerSavingLevel) 
    {
        case DOT11_POWER_SAVING_FAST_PSP:
            mode.dot11PowerMode = dot11_power_mode_powersave;
            mode.uPowerSaveLevel = DOT11_POWER_SAVE_LEVEL_FAST_PSP;
            mode.usListenInterval = STA_LISTEN_INTERVAL_LOW_PS_MODE;
            mode.usAID = pStation->Config.AID;
            mode.bReceiveDTIMs = TRUE;
            break;

        case DOT11_POWER_SAVING_MAX_PSP:
        case DOT11_POWER_SAVING_MAXIMUM_LEVEL:
            mode.dot11PowerMode = dot11_power_mode_powersave;
            mode.uPowerSaveLevel = DOT11_POWER_SAVE_LEVEL_MAX_PSP;
            mode.usListenInterval = pStation->Config.ListenInterval;
            mode.usAID = pStation->Config.AID;
            mode.bReceiveDTIMs = TRUE;
            break;

        case DOT11_POWER_SAVING_NO_POWER_SAVING:
            mode.dot11PowerMode = dot11_power_mode_active;
            mode.usAID = 0;
            break;

        default:
            return NDIS_STATUS_INVALID_DATA;
    }

    //
    // Pass the request to hardware.
    //
    
    ndisStatus = Hw11SetPowerMgmtMode(pStation->pNic, &mode);
    if (ndisStatus == NDIS_STATUS_SUCCESS)
        pStation->Config.PowerSavingLevel = PowerSavingLevel;

    return ndisStatus;
}

NDIS_STATUS
Sta11SetIBSSParameters(
    __in  PSTATION pStation,
    __in  PDOT11_IBSS_PARAMS pDot11IBSSParams,
    __out PULONG BytesRead
    )
{    
    PVOID               tmpBuf = NULL;

    if (pDot11IBSSParams->uIEsLength > 0)
    {
        MP_ALLOCATE_MEMORY(pStation->MiniportAdapterHandle, 
                           &tmpBuf,
                           pDot11IBSSParams->uIEsLength,
                           STA11_MEMORY_TAG);

        if (tmpBuf == NULL) 
        {
            *BytesRead = sizeof(DOT11_IBSS_PARAMS);
            return NDIS_STATUS_RESOURCES;
        }

        NdisMoveMemory(tmpBuf, 
                       Add2Ptr(pDot11IBSSParams, pDot11IBSSParams->uIEsOffset),
                       pDot11IBSSParams->uIEsLength);
    }

    if (pStation->Config.AdditionalIEData)
    {
        MP_FREE_MEMORY(pStation->Config.AdditionalIEData);
    }

    pStation->Config.IBSSJoinOnly = pDot11IBSSParams->bJoinOnly;
    pStation->Config.AdditionalIESize = pDot11IBSSParams->uIEsLength;
    pStation->Config.AdditionalIEData = tmpBuf;

    *BytesRead = pDot11IBSSParams->uIEsLength + pDot11IBSSParams->uIEsOffset;
    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Sta11SetDesiredPhyList(
    __in  PSTATION            pStation,
    __in  PDOT11_PHY_ID_LIST  PhyIdList,
    __out PULONG              pBytesRead
    )
{
    ULONG                       index;
    DOT11_SUPPORTED_PHY_TYPES   SupportedPhyTypes;
    BOOLEAN                     AnyPhyId = FALSE;

    //
    // If the list is too long or too short, simply return error.
    //
    if (PhyIdList->uNumOfEntries > STA_DESIRED_PHY_MAX_COUNT || PhyIdList->uNumOfEntries < 1)
    {
        *pBytesRead = FIELD_OFFSET(DOT11_PHY_ID_LIST, dot11PhyId);
        return NDIS_STATUS_INVALID_LENGTH;
    }

    *pBytesRead = FIELD_OFFSET(DOT11_PHY_ID_LIST, dot11PhyId) + PhyIdList->uNumOfEntries * sizeof(ULONG);
    
    //
    // Make sure we support all the PHY IDs in the list.
    //
    Hw11QuerySupportedPHYTypes(pStation->pNic, 0, &SupportedPhyTypes);
    for (index = 0; index < PhyIdList->uNumOfEntries; index++)
    {
        if (PhyIdList->dot11PhyId[index] == DOT11_PHY_ID_ANY)
        {
            AnyPhyId = TRUE;
        }
        else if (PhyIdList->dot11PhyId[index] >= SupportedPhyTypes.uTotalNumOfEntries) {
            return NDIS_STATUS_INVALID_DATA;
        }
    }

    //
    // Copy the desired PHY list.
    //
    if (AnyPhyId)
    {
        pStation->Config.DesiredPhyCount = 1;
        pStation->Config.DesiredPhyList[0] = DOT11_PHY_ID_ANY;
    }
    else
    {
        pStation->Config.DesiredPhyCount = PhyIdList->uNumOfEntries;
        NdisMoveMemory(pStation->Config.DesiredPhyList,
                       PhyIdList->dot11PhyId,
                       pStation->Config.DesiredPhyCount * sizeof(ULONG));
    }
  
    //
    // Pass to hardware
    //
    return Hw11SetDesiredPhyIdList(
        pStation->pNic,
        pStation->Config.DesiredPhyList,
        pStation->Config.DesiredPhyCount
        );
}

NDIS_STATUS
Sta11SetCurrentPhyId(
    __in  PSTATION        pStation,
    __in  ULONG           PhyId
    )
{
    return Hw11SetSelectedPhyId(pStation->pNic, PhyId);
}

NDIS_STATUS
Sta11SetPMKIDList(
    __in  PSTATION            pStation,
    __in  PDOT11_PMKID_LIST  PMKIDList,
    __out PULONG              pBytesRead
    )
{
    ULONG   index1, index2;

    //
    // If the list is too long or too short, simply return error.
    //
    if (PMKIDList->uNumOfEntries > STA_PMKID_MAX_COUNT || PMKIDList->uNumOfEntries < 1)
    {
        *pBytesRead = FIELD_OFFSET(DOT11_PMKID_LIST, PMKIDs);
        return NDIS_STATUS_INVALID_LENGTH;
    }

    *pBytesRead = FIELD_OFFSET(DOT11_PMKID_LIST, PMKIDs) + PMKIDList->uNumOfEntries * sizeof(DOT11_PMKID_ENTRY);
    
    //
    // Copy the PMKID list.
    //
    pStation->Config.PMKIDCount = PMKIDList->uNumOfEntries;
    NdisMoveMemory(pStation->Config.PMKIDList,
                   PMKIDList->PMKIDs,
                   pStation->Config.PMKIDCount * sizeof(DOT11_PMKID_ENTRY));

    //
    // Make sure all BSSID specified in the list are in our desired BSSID list.
    //
    if (!pStation->Config.AcceptAnyBSSID)
    {
        for (index1 = 0; index1 < pStation->Config.PMKIDCount; index1++)
        {
            for (index2 = 0; index2 < pStation->Config.DesiredBSSIDCount; index2++)
            {
                if (MP_COMPARE_MAC_ADDRESS(pStation->Config.PMKIDList[index1].BSSID,
                                           pStation->Config.DesiredBSSIDList[index2]))
                {
                    break;
                }
            }

            //
            // BSSID of PMKID is not in our desired BSSID list. delete the PMKID.
            //
            if (index2 == pStation->Config.DesiredBSSIDCount)
            {
                if (index1 != pStation->Config.PMKIDCount - 1)
                {
                    //
                    // This is not the last entry, copy the last entry to replace this one.
                    //
                    NdisMoveMemory(pStation->Config.PMKIDList + index1,
                                   pStation->Config.PMKIDList + pStation->Config.PMKIDCount - 1,
                                   sizeof(DOT11_PMKID_ENTRY));

                    //
                    // Have to check the current index again.
                    //
                    index1--;
                }

                pStation->Config.PMKIDCount--;
            }
        }
    }

    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Sta11SetUnreachableDetectionThreshold(
    __in  PSTATION        pStation,
    __in  ULONG           threshold
    )
{
    pStation->Config.UnreachableDetectionThreshold = threshold;
    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Sta11SetUnicastUseGroupOption(
    __in  PSTATION        pStation,
    __in  BOOLEAN         unicastUseGroupEnabled
    )
{
    pStation->Config.UnicastUseGroupEnabled = unicastUseGroupEnabled;
    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Sta11SetSafeModeOption(
    __in  PSTATION        pStation,
    __in  BOOLEAN         safeModeEnabled
    )
{
    NDIS_STATUS         ndisStatus = NDIS_STATUS_SUCCESS;

    ndisStatus = Hw11SetSafeModeOption(
        pStation->pNic,
        safeModeEnabled
        );

    return ndisStatus;
}

NDIS_STATUS
Sta11SetHiddenNetworkOption(
    __in  PSTATION        pStation,
    __in  BOOLEAN         hiddenNetworkEnabled
    )
{
    pStation->Config.HiddenNetworkEnabled = hiddenNetworkEnabled;
    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Sta11SetMediaStreamingOption(
    __in  PSTATION        pStation,
    __in  BOOLEAN         mediaStreamingEnabled
    )
{
    //
    // Save the setting. Next time we do a unsolicited periodic scan,
    // we will check this value and not do the scan
    //
    pStation->Config.MediaStreamingEnabled = mediaStreamingEnabled;
    return NDIS_STATUS_SUCCESS;
}

VOID
Sta11PowerSleep(
    __in PSTATION                 pStation,
    __in NDIS_DEVICE_POWER_STATE  NewDeviceState
    )
{
    PSTA_INFRA_CONNECT_CONTEXT    pConnectContext = &(pStation->ConnectContext);
    PSTA_BSS_ENTRY      pAPEntry = NULL;
    
    UNREFERENCED_PARAMETER(NewDeviceState);

        //
    // When going to sleep. We stop scanning
    //
    StaStopPeriodicScan(pStation);

    //
    // We are going to sleep. Unless we are connecting to a hidden network, we wont
    // add the SSID to the probe request
    //
    if (pStation->Config.HiddenNetworkEnabled)
    {
        // Hidden network, add SSID
        pStation->ScanContext.SSIDInProbeRequest = TRUE;
    }
    else
    {
        // Not hidden network, dont add SSID
        pStation->ScanContext.SSIDInProbeRequest = FALSE;
    }
    
    //
    // If we are associated with an AP, we would disassociate
    // and later when we wake up, we would perform a roam. However while going to 
    // sleep, we wont inform the OS that we have disassociated
    //
    STA_INCREMENT_REF(pStation->ConnectContext.AsyncFuncCount);    
    NdisAcquireSpinLock(&(pConnectContext->Lock));

    if ((pStation->ConnectContext.ConnectState == CONN_STATE_READY_TO_ROAM) &&
        (pStation->ConnectContext.AssociateState == ASSOC_STATE_ASSOCIATED)
        )
    {
        //
        // Get the AP entry
        //
        pAPEntry = pConnectContext->ActiveAP;
        pConnectContext->ActiveAP = NULL;

        //
        // Set state to disconnected. Then, when we wake up, we would
        // perform a roam
        //
        pConnectContext->AssociateState = ASSOC_STATE_NOT_ASSOCIATED;

        NdisReleaseSpinLock(&(pConnectContext->Lock));
        MpTrace(COMP_ASSOC, DBG_LOUD, ("Low power with Active connection\n"));

        MPVERIFY(pAPEntry != NULL);     // SYNC:        
        pAPEntry->AssocState = dot11_assoc_state_unauth_unassoc;    

        // This would perform a disconnect and cause a disassociation indication to the OS
        StaDisconnect(
            pStation, 
            pAPEntry, 
            ASSOC_STATE_ASSOCIATED
            );

        // Release the ref on the AP entry
        STA_DECREMENT_REF(pAPEntry->RefCount);
    }
    else
    {
        NdisReleaseSpinLock(&(pConnectContext->Lock));
    }
    STA_DECREMENT_REF(pStation->ConnectContext.AsyncFuncCount);       
}

VOID
Sta11PowerWakeup(
    __in PSTATION                 pStation
    )
{
    //
    // We call the hardware and have it restore any keys on hardware
    //
    Hw11RestoreEncryption(pStation->pNic);

    //
    // Resume beaconing for adhoc station.
    //
    if (pStation->Config.BSSType == dot11_BSS_type_independent)
        StaResumeAdHoc(pStation);

    //
    // Restart the periodic scan that we had stopped when going to sleep
    //
    StaStartPeriodicScan(pStation);

}


