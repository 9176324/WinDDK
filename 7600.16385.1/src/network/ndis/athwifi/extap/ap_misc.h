/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    ap_misc.h

Abstract:
    Contains ExtAP internal supporting functions
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    10-10-2007    Created

Notes:

--*/

#pragma once

#ifndef _AP_MISC_H
#define _AP_MISC_H

/** 
 * DOT11 Frame Class
 * See definitions in 802.11 standard.
 */
typedef enum _DOT11_FRAME_CLASS
{
    DOT11_FRAME_CLASS_1,
    DOT11_FRAME_CLASS_2,
    DOT11_FRAME_CLASS_3,
    DOT11_FRAME_CLASS_INVALID
} DOT11_FRAME_CLASS, *PDOT11_FRAME_CLASS;

DOT11_FRAME_CLASS
FORCEINLINE
Dot11GetFrameClass(
    __in PDOT11_FRAME_CTRL FrameControl
    )
{
    DOT11_FRAME_CLASS frameClass = DOT11_FRAME_CLASS_INVALID;

    switch(FrameControl->Type)
    {
        case DOT11_FRAME_TYPE_CONTROL:
            switch(FrameControl->Subtype)
            {
                case DOT11_CTRL_SUBTYPE_RTS:
                case DOT11_CTRL_SUBTYPE_CTS:
                case DOT11_CTRL_SUBTYPE_ACK:
                case DOT11_CTRL_SUBTYPE_CFE_CFA:
                case DOT11_CTRL_SUBTYPE_CFE:
                    frameClass = DOT11_FRAME_CLASS_1;
                    break;
                    
                case DOT11_CTRL_SUBTYPE_PS_POLL:
                    frameClass = DOT11_FRAME_CLASS_3;
                    break;
            }
            break;

        case DOT11_FRAME_TYPE_MANAGEMENT:
            switch(FrameControl->Subtype)
            {
                case DOT11_MGMT_SUBTYPE_PROBE_REQUEST:
                case DOT11_MGMT_SUBTYPE_PROBE_RESPONSE:
                case DOT11_MGMT_SUBTYPE_BEACON:
                case DOT11_MGMT_SUBTYPE_AUTHENTICATION:
                case DOT11_MGMT_SUBTYPE_DEAUTHENTICATION:
                case DOT11_MGMT_SUBTYPE_ATIM:
                    frameClass = DOT11_FRAME_CLASS_1;
                    break;
                    
                case DOT11_MGMT_SUBTYPE_ASSOCIATION_REQUEST:
                case DOT11_MGMT_SUBTYPE_ASSOCIATION_RESPONSE:
                case DOT11_MGMT_SUBTYPE_REASSOCIATION_REQUEST:
                case DOT11_MGMT_SUBTYPE_REASSOCIATION_RESPONSE:
                    case DOT11_MGMT_SUBTYPE_DISASSOCIATION:
                    frameClass = DOT11_FRAME_CLASS_2;
                    break;
            }
            break;

        case DOT11_FRAME_TYPE_DATA:
            if (FrameControl->ToDS == 0 && FrameControl->FromDS == 0)
            {
                frameClass = DOT11_FRAME_CLASS_1;
            }
            else
            {
                frameClass = DOT11_FRAME_CLASS_3;
            }
            break;
    }

    return frameClass;
}

/** Hardware related */

/** Get hardware capability */
NDIS_STATUS
ApInitializeCapability(
    __in PMP_EXTAP_PORT ApPort
    );

/** Deinitialize hardware capability */
VOID
ApDeinitializeCapability(
    __in PAP_CAPABIITY Capability
    );

/** Registry settings */
/** 
 * Read AP settings from registry
 * A default value is used if
 *     1. The registry vlaue is not present, or
 *     2. The registry value is not valid based on the hardware capability
 */
VOID
ApInitializeRegInfo(
    __in PMP_EXTAP_PORT ApPort
    );

/** Deinitialize registry info */
VOID
ApDeinitializeRegInfo(
    __in PAP_REG_INFO RegInfo
    );

/** commonly used supporting functions */

/** Get AP state */
AP_STATE
FORCEINLINE
ApGetState(
    __in PMP_EXTAP_PORT ApPort
    )
{
    return (AP_STATE)InterlockedExchangeAdd(
                (LONG *)&ApPort->State,
                0
                );
}

/** Set AP state, return the old state */
AP_STATE
FORCEINLINE
ApSetState(
    __in PMP_EXTAP_PORT ApPort,
    __in AP_STATE NewState
    )
{
    return (AP_STATE)InterlockedExchange(
                (LONG *)&ApPort->State,
                (LONG)NewState
                );
}

/** Reference AP port */
LONG
FORCEINLINE
ApRefPort(
    __in PMP_EXTAP_PORT ApPort
    )
{
    return InterlockedIncrement(&ApPort->RefCount);
}

/** Dereference AP port */
LONG
FORCEINLINE
ApDerefPort(
    __in PMP_EXTAP_PORT ApPort
    )
{
    return InterlockedDecrement(&ApPort->RefCount);
}

/** Dot11 status indication */
// TODO: shall we share the function with station
VOID 
ApIndicateDot11Status(
    __in PMP_EXTAP_PORT ApPort,
    __in NDIS_STATUS StatusCode,
    __in_opt PVOID RequestID,
    __in PVOID StatusBuffer,
    __in ULONG StatusBufferSize
    );


/** 
 * Functions that can be invoked before AP port is created 
 */

/** Query supported unicast auth/cipher algorithm pairs */
NDIS_STATUS
ApQuerySupportedUnicastAlgoPairs(
    __in PMP_PORT Port,
    __out_opt PDOT11_AUTH_CIPHER_PAIR AuthCipherPairs,
    __in ULONG BufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

/** Query supported multicast auth/cipher algorithm pairs */
NDIS_STATUS
ApQuerySupportedMulticastAlgoPairs(
    __in PMP_PORT Port,
    __out_opt PDOT11_AUTH_CIPHER_PAIR AuthCipherPairs,
    __in ULONG BufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

/** Validate auth algorithm */
BOOLEAN
ApValidateAuthAlgo(
    __in PMP_EXTAP_PORT ApPort,
    __in DOT11_AUTH_ALGORITHM AuthAlgo
    );

/** Validate unicate auth/cipher algorithms */
BOOLEAN
ApValidateUnicastAuthCipherPair(
    __in PMP_EXTAP_PORT ApPort,
    __in DOT11_AUTH_ALGORITHM AuthAlgo,
    __in DOT11_CIPHER_ALGORITHM CipherAlgo
    );

/** Validate multicate auth/cipher algorithms */
BOOLEAN
ApValidateMulticastAuthCipherPair(
    __in PMP_EXTAP_PORT ApPort,
    __in DOT11_AUTH_ALGORITHM AuthAlgo,
    __in DOT11_CIPHER_ALGORITHM CipherAlgo
    );

/** 
 * Calculate required list size 
 * Return FALSE if overflow
 */
BOOLEAN
FORCEINLINE
GetRequiredListSize(
    __in ULONG ElemOffset,
    __in ULONG ElemSize,
    __in ULONG ElemCount,
    __out PULONG RequiredSize
    )
{
    ElemCount = (ElemCount == 0) ? 1 : ElemCount;
    
    if (RtlULongMult(ElemSize, ElemCount, RequiredSize) != STATUS_SUCCESS)
    {
        return FALSE;
    }

    if (RtlULongAdd(*RequiredSize, ElemOffset, RequiredSize) != STATUS_SUCCESS)
    {
        return FALSE;
    }

    return TRUE;
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
    );

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
    );

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
    );

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
    );

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
    );


/**
 * Validate the size of a DOT11_BYTE_ARRAY.
 * Return NDIS_STATUS_INVALID_LENGTH if the buffer size is less than the needed size.
 * Return NDIS_STATUS_SUCCESS otherwise.
 */
NDIS_STATUS
ValidateDot11ByteArray(
    __in PDOT11_BYTE_ARRAY pDot11ByteArray,
    __in ULONG ArraySize,
    __out PULONG SizeNeeded
    );

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
    );

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
    );

/** Commonly used MACROs */

/** Set BytesNeeded to the needed buffer size and bail with corresponding NDIS status */
#define SET_NEEDED_BUFFER_SIZE_AND_BREAK(NeededSize) \
        { \
            *BytesNeeded = (NeededSize); \
            ndisStatus = NDIS_STATUS_INVALID_LENGTH; \
            break; \
        }

/**
 * Validate supported cipher algorithm
 */
BOOLEAN
IsSupportedCipher(
    __in PMP_EXTAP_PORT ApPort,
    __in ULONG AlgorithmId,
    __in USHORT KeyLength
    );

VOID 
ApIndicateFrequencyAdoped(
    __in PMP_EXTAP_PORT ApPort
    );

#endif  // _AP_MISC_H

