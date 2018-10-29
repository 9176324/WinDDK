/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    ap_misc.c

Abstract:
    Implements ExtAP supporting functions
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    09-26-2007    Created

Notes:

--*/
#include "precomp.h"
    
#if DOT11_TRACE_ENABLED
#include "ap_misc.tmh"
#endif

/** Get hardware capability */
NDIS_STATUS
ApInitializeCapability(
    __in PMP_EXTAP_PORT ApPort
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    UNREFERENCED_PARAMETER(ApPort);
    
    // TODO: get capability from base port?

    return ndisStatus;
}

/** Deinitialize hardware capability */
VOID
ApDeinitializeCapability(
    __in PAP_CAPABIITY Capability
    )
{
    MPASSERT(Capability != NULL);
    
    // TODO: free memory blocks

    // clear everything
    NdisZeroMemory(Capability, sizeof(AP_CAPABIITY));
}


/** 
 * Read AP settings from registry
 * A default value is used if
 *     1. The registry vlaue is not present, or
 *     2. The registry value is not valid based on the hardware capability
 */
VOID
ApInitializeRegInfo(
    __in PMP_EXTAP_PORT ApPort
    )
{
    PAP_REG_INFO regInfo = AP_GET_REG_INFO(ApPort);
    
    // TODO: Read the values from registry and validate them. Use defaults for now

    regInfo->AllowedAssociationCount = AP_DEFAULT_ALLOWED_ASSOCIATION_COUNT;

    regInfo->DefaultChannel = AP_DEFAULT_CHANNEL;

    regInfo->GroupKeyRenewalInterval = AP_DEFAULT_GROUP_KEY_RENEWAL_INTERVAL;

    regInfo->BeaconPeriod = AP_DEFAULT_BEACON_PERIOD;

    regInfo->DTimPeriod = AP_DEFAULT_DTIM_PERIOD;

    regInfo->RtsThreshold = AP_DEFAULT_RTS_THRESHOLD;

    regInfo->FragmentationThreshold = AP_DEFAULT_FRAGMENTATION_THRSHOLD;

    regInfo->Enable5GHz = AP_DEFAULT_ENABLE_5GHZ;

    regInfo->EnableCtsProtection = AP_DEFAULT_ENABLE_CTS_PROTECTION;

    regInfo->EnableFrameBurst = AP_DEFAULT_ENABLE_FRAME_BURST;

    regInfo->ShortRetryLimit = AP_DEFAULT_SHORT_RETRY_LIMIT;

    regInfo->LongRetryLimit = AP_DEFAULT_LONG_RETRY_LIMIT;

    regInfo->EnableWMM = AP_DEFAULT_ENABLE_WMM;
}

/** Deinitialize registry info */
VOID
ApDeinitializeRegInfo(
    __in PAP_REG_INFO RegInfo
    )
{
    MPASSERT(RegInfo != NULL);
    
    // TODO: free memory blocks

    // clear everything
    NdisZeroMemory(RegInfo, sizeof(AP_REG_INFO));
}

/** Dot11 status indication */
VOID 
ApIndicateDot11Status(
    __in PMP_EXTAP_PORT ApPort,
    __in NDIS_STATUS StatusCode,
    __in_opt PVOID RequestID,
    __in PVOID StatusBuffer,
    __in ULONG StatusBufferSize
    )
{
    NDIS_STATUS_INDICATION statusIndication;
    
    MpTrace(COMP_ASSOC_MGR, DBG_NORMAL, 
            ("Port(%u): Indicate Dot11 status. StatusCode = 0x%08x.", AP_GET_PORT_NUMBER(ApPort), StatusCode));

    NdisZeroMemory(&statusIndication, sizeof(NDIS_STATUS_INDICATION));
    
    //
    // Fill in object header
    //
    statusIndication.Header.Type = NDIS_OBJECT_TYPE_STATUS_INDICATION;
    statusIndication.Header.Revision = NDIS_STATUS_INDICATION_REVISION_1;
    statusIndication.Header.Size = sizeof(NDIS_STATUS_INDICATION);

    // fill in the correct port number
    statusIndication.PortNumber = (AP_GET_MP_PORT(ApPort))->PortNumber;
    
    //
    // Fill in the rest of the field
    // 
    statusIndication.StatusCode = StatusCode;
    statusIndication.SourceHandle = AP_GET_MP_HANDLE(ApPort);
    statusIndication.DestinationHandle = NULL;
    statusIndication.RequestId = RequestID;
    
    statusIndication.StatusBuffer = StatusBuffer;
    statusIndication.StatusBufferSize = StatusBufferSize;

    //
    // Indicate the status to NDIS
    //
    NdisMIndicateStatusEx(
        AP_GET_MP_HANDLE(ApPort),
        &statusIndication
        );
}

/** 
 * Functions that can be invoked before AP port is created 
 */

/** Query supported unicast auth/cipher algorithm pairs */
NDIS_STATUS
ApQuerySupportedUnicastAlgoPairs(
    __in PMP_PORT Port,
    __out_opt PDOT11_AUTH_CIPHER_PAIR AuthCipherPairs,
    __in ULONG PairCount,
    __out PULONG PairCountWritten,
    __out PULONG PairCountNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    ULONG count;
    PVNIC vnic = Port->VNic;
    BOOLEAN TKIPImplemented = VNic11TKIPImplemented(vnic);
    BOOLEAN CCMPImplemented = VNic11CCMPImplemented(vnic);

    do
    {
        // No-encryption is supported
        count = 1;

        if (TKIPImplemented)
        {
            // WPA2PSK/TKIP
            count += 1;
        }

        if (CCMPImplemented)
        {
            // WPA2PSK/AES
            count += 1;
        }

        // Ensure enough space for the list
        if (PairCount < count)
        {
            *PairCountNeeded = count;
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }

        // Buffer is large enough
        count = 0;

        AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_80211_OPEN;
        AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_NONE;

        if (TKIPImplemented)
        {
            count++;
            AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_RSNA_PSK;
            AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_TKIP;
        }

        if (CCMPImplemented)
        {
            count++;
            AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_RSNA_PSK;
            AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_CCMP;
        }

        *PairCountWritten = ++count;
    } while(FALSE);

    return ndisStatus;
}


/** Query supported multicast auth/cipher algorithm pairs */
NDIS_STATUS
ApQuerySupportedMulticastAlgoPairs(
    __in PMP_PORT Port,
    __out_opt PDOT11_AUTH_CIPHER_PAIR AuthCipherPairs,
    __in ULONG PairCount,
    __out PULONG PairCountWritten,
    __out PULONG PairCountNeeded
    )
{
    // It is the same as unicast
    return ApQuerySupportedUnicastAlgoPairs(
                Port, 
                AuthCipherPairs, 
                PairCount, 
                PairCountWritten, 
                PairCountNeeded
                );
}

/** Validate auth algorithm */
BOOLEAN
ApValidateAuthAlgo(
    __in PMP_EXTAP_PORT ApPort,
    __in DOT11_AUTH_ALGORITHM AuthAlgo
    )
{
    BOOLEAN TKIPImplemented = VNic11TKIPImplemented(AP_GET_VNIC(ApPort));
    BOOLEAN CCMPImplemented = VNic11CCMPImplemented(AP_GET_VNIC(ApPort));
    BOOLEAN ValidAuth = TRUE;

    // 
    // Check if we support the specified auth algorithm.
    //
    switch (AuthAlgo)
    {
        case DOT11_AUTH_ALGO_80211_OPEN:
            break;
    
        case DOT11_AUTH_ALGO_RSNA_PSK:
            if (!CCMPImplemented && !TKIPImplemented)
            {
                ValidAuth = FALSE;
            }
    
            break;
    
        default:
            ValidAuth = FALSE;
    }

    return ValidAuth;
}

/** Validate unicate auth/cipher algorithms */
BOOLEAN
ApValidateUnicastAuthCipherPair(
    __in PMP_EXTAP_PORT ApPort,
    __in DOT11_AUTH_ALGORITHM AuthAlgo,
    __in DOT11_CIPHER_ALGORITHM CipherAlgo
    )
{
    BOOLEAN WEP40Implemented = VNic11WEP40Implemented(AP_GET_VNIC(ApPort));
    BOOLEAN WEP104Implemented = VNic11WEP104Implemented(AP_GET_VNIC(ApPort));
    BOOLEAN TKIPImplemented = VNic11TKIPImplemented(AP_GET_VNIC(ApPort));
    BOOLEAN CCMPImplemented = VNic11CCMPImplemented(AP_GET_VNIC(ApPort));
    BOOLEAN ValidPair = FALSE;

    switch (AuthAlgo)
    {
        case DOT11_AUTH_ALGO_80211_OPEN:
            ValidPair = (BOOLEAN)((DOT11_CIPHER_ALGO_WEP == CipherAlgo && (WEP40Implemented || WEP104Implemented)) ||
                             (DOT11_CIPHER_ALGO_WEP104 == CipherAlgo && WEP104Implemented) ||
                             (DOT11_CIPHER_ALGO_WEP40 == CipherAlgo && WEP40Implemented) ||
                             (DOT11_CIPHER_ALGO_NONE == CipherAlgo));

            break;

        case DOT11_AUTH_ALGO_80211_SHARED_KEY:
            ValidPair = (BOOLEAN)((DOT11_CIPHER_ALGO_WEP == CipherAlgo && (WEP40Implemented || WEP104Implemented)) ||
                             (DOT11_CIPHER_ALGO_WEP104 == CipherAlgo && WEP104Implemented) ||
                             (DOT11_CIPHER_ALGO_WEP40 == CipherAlgo && WEP40Implemented));
            break;
            
        case DOT11_AUTH_ALGO_WPA:
        case DOT11_AUTH_ALGO_WPA_PSK:
        case DOT11_AUTH_ALGO_RSNA:
        case DOT11_AUTH_ALGO_RSNA_PSK:
            ValidPair = (BOOLEAN)((DOT11_CIPHER_ALGO_TKIP == CipherAlgo && TKIPImplemented) ||
                             (DOT11_CIPHER_ALGO_CCMP == CipherAlgo && CCMPImplemented));
            break;

        default:
            MPASSERT(FALSE);

    }

    return ValidPair;
}

/** Validate multicate auth/cipher algorithms */
BOOLEAN
ApValidateMulticastAuthCipherPair(
    __in PMP_EXTAP_PORT ApPort,
    __in DOT11_AUTH_ALGORITHM AuthAlgo,
    __in DOT11_CIPHER_ALGORITHM CipherAlgo
    )
{
    BOOLEAN WEP40Implemented = VNic11WEP40Implemented(AP_GET_VNIC(ApPort));
    BOOLEAN WEP104Implemented = VNic11WEP104Implemented(AP_GET_VNIC(ApPort));
    BOOLEAN TKIPImplemented = VNic11TKIPImplemented(AP_GET_VNIC(ApPort));
    BOOLEAN CCMPImplemented = VNic11CCMPImplemented(AP_GET_VNIC(ApPort));
    BOOLEAN ValidPair = FALSE;

    switch (AuthAlgo)
    {
        case DOT11_AUTH_ALGO_80211_OPEN:
            ValidPair = (BOOLEAN)((DOT11_CIPHER_ALGO_WEP == CipherAlgo && (WEP40Implemented || WEP104Implemented)) ||
                             (DOT11_CIPHER_ALGO_WEP104 == CipherAlgo && WEP104Implemented) ||
                             (DOT11_CIPHER_ALGO_WEP40 == CipherAlgo && WEP40Implemented) ||
                             (DOT11_CIPHER_ALGO_NONE == CipherAlgo));
            break;

        case DOT11_AUTH_ALGO_80211_SHARED_KEY:
            ValidPair = (BOOLEAN)((DOT11_CIPHER_ALGO_WEP == CipherAlgo && (WEP40Implemented || WEP104Implemented)) ||
                             (DOT11_CIPHER_ALGO_WEP104 == CipherAlgo && WEP104Implemented) ||
                             (DOT11_CIPHER_ALGO_WEP40 == CipherAlgo && WEP40Implemented));
            break;

        case DOT11_AUTH_ALGO_WPA:
        case DOT11_AUTH_ALGO_WPA_PSK:
        case DOT11_AUTH_ALGO_RSNA:
        case DOT11_AUTH_ALGO_RSNA_PSK:
            ValidPair = (BOOLEAN)((DOT11_CIPHER_ALGO_TKIP == CipherAlgo && TKIPImplemented) ||
                             (DOT11_CIPHER_ALGO_CCMP == CipherAlgo && CCMPImplemented) ||
                             (DOT11_CIPHER_ALGO_WEP104 == CipherAlgo && WEP104Implemented) ||
                             (DOT11_CIPHER_ALGO_WEP40 == CipherAlgo && WEP40Implemented));
            break;

        default:
            MPASSERT(FALSE);
    }

    return ValidPair;
}

/**
 * Validate the size of a SSID list.
 * Return NDIS_STATUS_INVALID_LENGTH if the list size is less than the needed size.
 * Return NDIS_STATUS_SUCCESS is returned if the list size is equal to or greater than the needed size.
 * SizeNeeded contains the required size in these cases.
 * Return NDIS_STATUS_INVALID_DATA if size calculation overflows.
 */
NDIS_STATUS
ValiateSsidListSize(
    __in PDOT11_SSID_LIST SsidList,
    __in ULONG ListSize,
    __out PULONG SizeNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_INVALID_LENGTH;

    do
    {
        if (ListSize < sizeof(DOT11_SSID_LIST))
        {
            *SizeNeeded = sizeof(DOT11_SSID_LIST);
            break;
        }

        if (!GetRequiredListSize(
                FIELD_OFFSET(DOT11_SSID_LIST, SSIDs), 
                sizeof(DOT11_SSID), 
                SsidList->uNumOfEntries, 
                SizeNeeded
                ))
        {
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            break;
        }

        if (ListSize >= *SizeNeeded)
        {
            ndisStatus = NDIS_STATUS_SUCCESS;
        }
        
    } while (FALSE);

    return ndisStatus;
}

/**
 * Validate the size of a privacy exemption list.
 * Return NDIS_STATUS_INVALID_LENGTH if the list size is less than the needed size.
 * Return NDIS_STATUS_SUCCESS is returned if the list size is equal to or greater than the needed size.
 * SizeNeeded contains the required size in these cases.
 * Return NDIS_STATUS_INVALID_DATA if size calculation overflows.
 */
NDIS_STATUS
ValiatePrivacyExemptionListSize(
    __in PDOT11_PRIVACY_EXEMPTION_LIST PrivacyExemptionList,
    __in ULONG ListSize,
    __out PULONG SizeNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_INVALID_LENGTH;

    do
    {
        if (ListSize < FIELD_OFFSET(DOT11_PRIVACY_EXEMPTION_LIST, PrivacyExemptionEntries))
        {
            *SizeNeeded = FIELD_OFFSET(DOT11_PRIVACY_EXEMPTION_LIST, PrivacyExemptionEntries);
            break;
        }

        if (!GetRequiredListSize(
                FIELD_OFFSET(DOT11_PRIVACY_EXEMPTION_LIST, PrivacyExemptionEntries), 
                sizeof(DOT11_PRIVACY_EXEMPTION), 
                PrivacyExemptionList->uNumOfEntries, 
                SizeNeeded
                ))
        {
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            break;
        }

        if (ListSize >= *SizeNeeded)
        {
            ndisStatus = NDIS_STATUS_SUCCESS;
        }
        
    } while (FALSE);

    return ndisStatus;
}

/**
 * Validate the size of a auth algorithm list.
 * Return NDIS_STATUS_INVALID_LENGTH if the list size is less than the needed size.
 * Return NDIS_STATUS_SUCCESS is returned if the list size is equal to or greater than the needed size.
 * SizeNeeded contains the required size in these cases.
 * Return NDIS_STATUS_INVALID_DATA if size calculation overflows.
 */
NDIS_STATUS
ValiateAuthAlgorithmListSize(
    __in PDOT11_AUTH_ALGORITHM_LIST AuthAlgorithmList,
    __in ULONG ListSize,
    __out PULONG SizeNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_INVALID_LENGTH;

    do
    {
        if (ListSize < sizeof(DOT11_AUTH_ALGORITHM_LIST))
        {
            *SizeNeeded = sizeof(DOT11_AUTH_ALGORITHM_LIST);
            break;
        }

        if (!GetRequiredListSize(
                FIELD_OFFSET(DOT11_AUTH_ALGORITHM_LIST, AlgorithmIds), 
                sizeof(DOT11_AUTH_ALGORITHM), 
                AuthAlgorithmList->uNumOfEntries, 
                SizeNeeded
                ))
        {
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            break;
        }

        if (ListSize >= *SizeNeeded)
        {
            ndisStatus = NDIS_STATUS_SUCCESS;
        }
        
    } while (FALSE);

    return ndisStatus;
}

/**
 * Validate the size of a cipher algorithm list.
 * Return NDIS_STATUS_INVALID_LENGTH if the list size is less than the needed size.
 * Return NDIS_STATUS_SUCCESS is returned if the list size is equal to or greater than the needed size.
 * SizeNeeded contains the required size in these cases.
 * Return NDIS_STATUS_INVALID_DATA if size calculation overflows.
 */
NDIS_STATUS
ValiateCipherAlgorithmListSize(
    __in PDOT11_CIPHER_ALGORITHM_LIST CipherAlgorithmList,
    __in ULONG ListSize,
    __out PULONG SizeNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_INVALID_LENGTH;

    do
    {
        if (ListSize < sizeof(DOT11_CIPHER_ALGORITHM_LIST))
        {
            *SizeNeeded = sizeof(DOT11_CIPHER_ALGORITHM_LIST);
            break;
        }

        if (!GetRequiredListSize(
                FIELD_OFFSET(DOT11_CIPHER_ALGORITHM_LIST, AlgorithmIds), 
                sizeof(DOT11_CIPHER_ALGORITHM), 
                CipherAlgorithmList->uNumOfEntries, 
                SizeNeeded
                ))
        {
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            break;
        }

        if (ListSize >= *SizeNeeded)
        {
            ndisStatus = NDIS_STATUS_SUCCESS;
        }
        
    } while (FALSE);

    return ndisStatus;
}

/**
 * Validate the size of a cipher default key.
 * Return NDIS_STATUS_INVALID_LENGTH if the key size is less than the needed size.
 * Return NDIS_STATUS_SUCCESS is returned if the key size is equal to or greater than the needed size.
 * SizeNeeded contains the required size in these cases.
 * Return NDIS_STATUS_INVALID_DATA if size calculation overflows.
 */
NDIS_STATUS
ValiateCipherDefaultKeySize(
    __in PDOT11_CIPHER_DEFAULT_KEY_VALUE CipherDefaultKey,
    __in ULONG KeySize,
    __out PULONG SizeNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_INVALID_LENGTH;

    do
    {
        if (KeySize < sizeof(DOT11_CIPHER_DEFAULT_KEY_VALUE))
        {
            *SizeNeeded = sizeof(DOT11_CIPHER_DEFAULT_KEY_VALUE);
            break;
        }

        if (!GetRequiredListSize(
                FIELD_OFFSET(DOT11_CIPHER_DEFAULT_KEY_VALUE, ucKey), 
                sizeof(UCHAR), 
                CipherDefaultKey->usKeyLength, 
                SizeNeeded
                ))
        {
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            break;
        }

        if (KeySize >= *SizeNeeded)
        {
            ndisStatus = NDIS_STATUS_SUCCESS;
        }
        
    } while (FALSE);

    return ndisStatus;
}


/**
 * Validate the size of a DOT11_BYTE_ARRAY.
 * Return NDIS_STATUS_INVALID_LENGTH if the buffer size is less than the needed size.
 * Return NDIS_STATUS_INVALID_DATA if the buffer is malformed.
 * Return NDIS_STATUS_SUCCESS otherwise.
 */
NDIS_STATUS
ValidateDot11ByteArray(
    __in PDOT11_BYTE_ARRAY pDot11ByteArray,
    __in ULONG ArraySize,
    __out PULONG SizeNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    do
    {
        if (ArraySize < FIELD_OFFSET(DOT11_BYTE_ARRAY,ucBuffer))
        {
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            *SizeNeeded = FIELD_OFFSET(DOT11_BYTE_ARRAY,ucBuffer);
            break;
        }

        if (pDot11ByteArray->uNumOfBytes  != pDot11ByteArray->uTotalNumOfBytes)
        {
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            break;
        }

        ndisStatus = RtlULongAdd(
                FIELD_OFFSET(DOT11_BYTE_ARRAY,ucBuffer),
                pDot11ByteArray->uNumOfBytes,
                SizeNeeded
                );
        if ( ndisStatus != STATUS_SUCCESS)
        {
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            break;
        }

        if (ArraySize < *SizeNeeded)
        {
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }

    } while (FALSE);

    return ndisStatus;
}

/**
 * Validate the size of a PHY ID list.
 * Return NDIS_STATUS_INVALID_LENGTH if the list size is less than the needed size.
 * Return NDIS_STATUS_SUCCESS is returned if the list size is equal to or greater than the needed size.
 * SizeNeeded contains the required size in these cases.
 * Return NDIS_STATUS_INVALID_DATA if size calculation overflows.
 */
NDIS_STATUS
ValiatePhyIdListSize(
    __in PDOT11_PHY_ID_LIST PhyIdList,
    __in ULONG ListSize,
    __out PULONG SizeNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_INVALID_LENGTH;

    do
    {
        if (ListSize < sizeof(DOT11_PHY_ID_LIST))
        {
            *SizeNeeded = sizeof(DOT11_PHY_ID_LIST);
            break;
        }

        if (!GetRequiredListSize(
                FIELD_OFFSET(DOT11_PHY_ID_LIST, dot11PhyId), 
                sizeof(ULONG), 
                PhyIdList->uNumOfEntries, 
                SizeNeeded
                ))
        {
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            break;
        }

        if (ListSize >= *SizeNeeded)
        {
            ndisStatus = NDIS_STATUS_SUCCESS;
        }
        
    } while (FALSE);

    return ndisStatus;
}

/**
 * Validate the size of additional IEs.
 * Return NDIS_STATUS_INVALID_LENGTH if the IE size is less than the needed size.
 * Return NDIS_STATUS_SUCCESS is returned if the IE size is equal to or greater than the needed size.
 * SizeNeeded contains the required size in these cases.
 * Return NDIS_STATUS_INVALID_DATA if size calculation overflows.
 */
NDIS_STATUS
ValiateAdditionalIeSize(
    __in PDOT11_ADDITIONAL_IE IeData,
    __in ULONG IeSize,
    __out PULONG SizeNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_INVALID_LENGTH;
    ULONG requiredBeaconIeSize = 0;
    ULONG requiredResponseIeSize = 0;

    do
    {
        if (IeSize < sizeof(DOT11_ADDITIONAL_IE))
        {
            *SizeNeeded = sizeof(DOT11_ADDITIONAL_IE);
            break;
        }

        // check the size for the beacon IEs
        if (RtlULongAdd(IeData->uBeaconIEsOffset, IeData->uBeaconIEsLength, &requiredBeaconIeSize) != STATUS_SUCCESS)
        {
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            break;
        }

        // check the size for the probe response IEs
        if (RtlULongAdd(IeData->uResponseIEsOffset, IeData->uResponseIEsLength, &requiredResponseIeSize) != STATUS_SUCCESS)
        {
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            break;
        }

        // the size needed is the larger one
        *SizeNeeded = (requiredBeaconIeSize > requiredResponseIeSize) ? requiredBeaconIeSize : requiredResponseIeSize;
        
        if (IeSize >= *SizeNeeded)
        {
            ndisStatus = NDIS_STATUS_SUCCESS;
        }
        
    } while (FALSE);

    return ndisStatus;
}



/**
 * Validate supported cipher algorithm
 */
BOOLEAN
IsSupportedCipher(
    __in PMP_EXTAP_PORT ApPort,
    __in ULONG AlgorithmId,
    __in USHORT KeyLength
    )
{
    BOOLEAN bResult = FALSE;

    //
    // Check cipher algorithm
    //
    switch (AlgorithmId)
    {
        case DOT11_CIPHER_ALGO_CCMP:
            bResult = VNic11CCMPImplemented(AP_GET_VNIC(ApPort));
            break;

        case DOT11_CIPHER_ALGO_TKIP:
            bResult = VNic11TKIPImplemented(AP_GET_VNIC(ApPort));
            break;

        case DOT11_CIPHER_ALGO_WEP:
            bResult =
            (
                VNic11WEP40Implemented(AP_GET_VNIC(ApPort))  || 
                VNic11WEP104Implemented(AP_GET_VNIC(ApPort))
            );
            break;

        case DOT11_CIPHER_ALGO_WEP40:
            bResult =
            (
                VNic11WEP40Implemented(AP_GET_VNIC(ApPort))  && 
                ((KeyLength == 40 / 8) ? TRUE : FALSE)
            );
            break;

        case DOT11_CIPHER_ALGO_WEP104:
            bResult =
            (
                VNic11WEP104Implemented(AP_GET_VNIC(ApPort))  && 
                ((KeyLength == 104 / 8) ? TRUE : FALSE)
            );
            break;

        default:
            break;
    }

    return bResult;
}

VOID 
ApIndicateFrequencyAdoped(
    __in PMP_EXTAP_PORT ApPort
    )
{
    DOT11_PHY_FREQUENCY_ADOPTED_PARAMETERS params;
    PVNIC vnic = AP_GET_VNIC(ApPort);

    // send out the NDIS_STATUS_DOT11_PHY_FREQUENCY_ADOPTED status indication
    NdisZeroMemory(&params, sizeof(DOT11_PHY_FREQUENCY_ADOPTED_PARAMETERS));

    MP_ASSIGN_NDIS_OBJECT_HEADER(
        params.Header,
        NDIS_OBJECT_TYPE_DEFAULT,
        DOT11_PHY_FREQUENCY_ADOPTED_PARAMETERS_REVISION_1,
        sizeof(DOT11_PHY_FREQUENCY_ADOPTED_PARAMETERS)
        );

    params.ulChannel = VNic11QueryCurrentChannel(vnic, FALSE);
    params.ulPhyId = VNic11QueryOperatingPhyId(vnic);
    
    ApIndicateDot11Status(
        ApPort, 
        NDIS_STATUS_DOT11_PHY_FREQUENCY_ADOPTED, 
        NULL,                   // no request ID
        &params, 
        sizeof(params)
        );
}

