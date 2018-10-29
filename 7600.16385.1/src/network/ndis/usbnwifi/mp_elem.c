/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Mp_Element.c

Abstract:
    Utility functions to work with information elements
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
#include "precomp.h"

#include "mp_util.h"
#include "mp_elem.h"


NDIS_STATUS
Dot11ValidateInfoBlob(
    __in PUCHAR pucInfoBlob,
    __in ULONG uSizeOfBlob
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_INFO_ELEMENT pInfoEleHdr = NULL;
    ULONG uRequiredSize = 0;

    while(uSizeOfBlob) {

        pInfoEleHdr = (PDOT11_INFO_ELEMENT)pucInfoBlob;
        if (uSizeOfBlob < sizeof(DOT11_INFO_ELEMENT)) {
            break;
        }

        uRequiredSize = (ULONG)(pInfoEleHdr->Length) + sizeof(DOT11_INFO_ELEMENT);
        if (uSizeOfBlob < uRequiredSize) {
            break;
        }

        uSizeOfBlob -= uRequiredSize;
        pucInfoBlob += uRequiredSize;
    }

    if (uSizeOfBlob) {
        ndisStatus = NDIS_STATUS_FAILURE;
    }
    return ndisStatus;
}


/**
Mitesh: Update documentation
    Validate the info blob by walking through the information
    element chain.

    This routine return the size of information element blob
    that it walks thru. In case of malformed beacon, caller
 */
/**
 * 
 * 
 * \param pPacketBuffer
 * \param PacketLength
 * \param OffsetOfInfoElemBlob
 * \param pInfoElemBlobSize
 * \return 
 * \sa 
 */

NDIS_STATUS
Dot11GetInfoBlobSize(
    __in  PUCHAR                  pPacketBuffer,
    __in  ULONG                   PacketLength,
    __in  ULONG                   OffsetOfInfoElemBlob,
    __out PULONG                  pInfoElemBlobSize
    )
{
    NDIS_STATUS         ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_INFO_ELEMENT pInfoElemHdr = NULL;
    ULONG               RemainingLength = PacketLength - OffsetOfInfoElemBlob;
    ULONG               InfoBlobSize = 0;

    pPacketBuffer += OffsetOfInfoElemBlob;
    
    // Walk through to check nothing is malformed
    while (RemainingLength >= sizeof(DOT11_INFO_ELEMENT))
    {
        // Atleast one more header is present
        pInfoElemHdr = (PDOT11_INFO_ELEMENT)(pPacketBuffer + InfoBlobSize);
        RemainingLength -= sizeof(DOT11_INFO_ELEMENT);
        InfoBlobSize += sizeof(DOT11_INFO_ELEMENT);
        
        if (pInfoElemHdr->Length == 0)
        {
            continue;
        }

        if (RemainingLength < pInfoElemHdr->Length)
        {
            // Incomplete/bad info element
            ndisStatus = NDIS_STATUS_NOT_ACCEPTED;
            break;
        }

        // Consume info element 
        RemainingLength -= pInfoElemHdr->Length;
        InfoBlobSize += pInfoElemHdr->Length;
    }

    *pInfoElemBlobSize = InfoBlobSize;
    return ndisStatus;
}

NDIS_STATUS
Dot11GetInfoEle(
    __in PUCHAR pucInfoBlob,
    __in ULONG uSizeOfBlob,
    __in UCHAR ucInfoId,
    __out PUCHAR pucLength,
    __deref_out_opt PVOID * ppvInfoEle
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_INFO_ELEMENT pInfoEleHdr = NULL;
    ULONG uRequiredSize = 0;
    BOOL bFound = FALSE;

    *pucLength = 0;
    *ppvInfoEle = NULL;
    while(uSizeOfBlob) {

        pInfoEleHdr = (PDOT11_INFO_ELEMENT)pucInfoBlob;
        if (uSizeOfBlob < sizeof(DOT11_INFO_ELEMENT)) {
            break;
        }

        uRequiredSize = (ULONG)(pInfoEleHdr->Length) + sizeof(DOT11_INFO_ELEMENT);
        if (uSizeOfBlob < uRequiredSize) {
            break;
        }

        if (pInfoEleHdr->ElementID == ucInfoId) {
            *pucLength = pInfoEleHdr->Length;
            *ppvInfoEle = pucInfoBlob + sizeof(DOT11_INFO_ELEMENT);
            bFound = TRUE;
            break;
        }

        uSizeOfBlob -= uRequiredSize;
        pucInfoBlob += uRequiredSize;
    }

    if (!bFound) {
        ndisStatus = NDIS_STATUS_FAILURE;
    }
    return ndisStatus;
}

NDIS_STATUS
Dot11ValidatePacketInfoBlob(
    __in PNDIS_BUFFER pMdlHead,
    __in ULONG uOffsetOfInfoEleBlob
    )
/*++

Routine Description:

    Validate the info blob by walking through the information
    element chain.

Arguments:

Return Value:

--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    DOT11_INFO_ELEMENT InfoEleHdr = { 0 };
    PDOT11_INFO_ELEMENT pInfoEleHdr = NULL;
    ULONG uCurrentOffset = uOffsetOfInfoEleBlob;
    UCHAR ucLastByte = 0;
    PUCHAR pucLastByte = NULL;

    while(pMdlHead) {
        pInfoEleHdr = NULL;
        ndisStatus = Dot11GetDataFromMdlChain(
                            &pMdlHead,
                            uCurrentOffset,
                            sizeof(DOT11_INFO_ELEMENT),
                            &InfoEleHdr,
                            &uCurrentOffset,
                            (PVOID)&pInfoEleHdr
                            );
        BAIL_ON_NDIS_ERROR(ndisStatus);

        MPASSERT (NULL != pInfoEleHdr);

        if (pInfoEleHdr->Length == 0) {
            continue;
        }

        //
        // Try to read the last byte of the information element
        //
        uCurrentOffset += pInfoEleHdr->Length - 1;

        pucLastByte = NULL;
        ndisStatus = Dot11GetDataFromMdlChain(
                            &pMdlHead,
                            uCurrentOffset,
                            1,      // only one byte
                            &ucLastByte,
                            &uCurrentOffset,
                            (PVOID)&pucLastByte
                            );
        BAIL_ON_NDIS_ERROR(ndisStatus);

        //
        // The buffer should be always contiguous since
        // we read only one bye.
        //
        MPASSERT(pucLastByte != &ucLastByte);
    }

error:
    return ndisStatus;
}

NDIS_STATUS
Dot11GetInfoEleFromPacket(
    __in PNDIS_BUFFER pMdlHead,
    __in ULONG uOffsetOfInfoEleBlob,
    __in UCHAR ucInfoId,
    __in UCHAR ucMaxLength,
    __out PUCHAR pucLength,
    __inout PVOID * ppvInfoEle
    )
/*++

Routine Description:

    This routine copy information element from an MDL chain

Arguments:

    pMdlHead    the MDL chain

    uOffsetOfInfoEleBlob    the offset of the information element blob in the packet

    ucInfoId    the ID of the information element to be look up

    ucMaxLength the size of the buffer pointed by (*ppvInfoEle)

    pucLength   when the function is successfully completed, it will
                hole the size of the informaion element.

    ppvInfoEle  as input, (*ppvInfoEle) specify the pointer to the storage to be used
                in case of the information element buffer is discontiguous
                as output, (*ppvInfoEle) stores the pointer to the returned information blob.
                it could the same input pointer if the buffer is discontiguous. Otherwise,
                it will point to the buffer in the packet.

Return Value:

--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    DOT11_INFO_ELEMENT InfoEleHdr = { 0 };
    PDOT11_INFO_ELEMENT pInfoEleHdr = NULL;
    ULONG uCurrentOffset = uOffsetOfInfoEleBlob;

    ASSERT(ppvInfoEle && *ppvInfoEle);

    while(1) {
        pInfoEleHdr = NULL;
        ndisStatus = Dot11GetDataFromMdlChain(
                            &pMdlHead,
                            uCurrentOffset,
                            sizeof(DOT11_INFO_ELEMENT),
                            &InfoEleHdr,
                            &uCurrentOffset,
                            (PVOID)&pInfoEleHdr
                            );
        BAIL_ON_NDIS_ERROR(ndisStatus);

        ASSERT (NULL != pInfoEleHdr);

        if (pInfoEleHdr->ElementID == ucInfoId) {
            break;
        }

        //
        // Skip the information content and move the pointer
        // to the header of next information element.
        //
        uCurrentOffset += pInfoEleHdr->Length;
    }

    *pucLength = pInfoEleHdr->Length;
    if (pInfoEleHdr->Length > ucMaxLength) {
        ndisStatus = NDIS_STATUS_BUFFER_TOO_SHORT;
        BAIL_ON_NDIS_ERROR(ndisStatus);
    }

    ndisStatus = Dot11GetDataFromMdlChain(
                            &pMdlHead,
                            uCurrentOffset,
                            pInfoEleHdr->Length,
                            *ppvInfoEle,
                            &uCurrentOffset,
                            (PUCHAR*)ppvInfoEle
                            );
    BAIL_ON_NDIS_ERROR(ndisStatus);

error:
    return ndisStatus;
}

NDIS_STATUS
Dot11CopySSIDFromPacket(
    __in PNDIS_BUFFER pMdlHead,
    __in ULONG uOffsetOfInfoEleBlob,
    __in PDOT11_SSID pDot11SSID
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    UCHAR ucLength = 0;
    PVOID pvStorage = pDot11SSID->ucSSID;

    ndisStatus = Dot11GetInfoEleFromPacket(
                    pMdlHead,
                    uOffsetOfInfoEleBlob,
                    DOT11_INFO_ELEMENT_ID_SSID,
                    sizeof(pDot11SSID->ucSSID),
                    &ucLength,
                    &pvStorage
                    );
    BAIL_ON_NDIS_ERROR(ndisStatus);

    pDot11SSID->uSSIDLength = ucLength;
    if (pvStorage != pDot11SSID->ucSSID) {
        memcpy(pDot11SSID->ucSSID, pvStorage, ucLength);
    }

error:
    return ndisStatus;
}


NDIS_STATUS
Dot11GetChannelForDSPhy(
    __in PUCHAR pucInfoBlob,
    __in ULONG uSizeOfBlob,
    __in PUCHAR pucChannel
    )
{
    UCHAR ucLength = 0;
    PUCHAR pucParam = NULL;
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    ndisStatus = Dot11GetInfoEle(
                    pucInfoBlob,
                    uSizeOfBlob,
                    DOT11_INFO_ELEMENT_ID_DS_PARAM_SET,
                    &ucLength,
                    &pucParam
                    );
    BAIL_ON_NDIS_ERROR(ndisStatus);

    if (ucLength != 1) {
        ndisStatus = NDIS_STATUS_INVALID_LENGTH;
        BAIL_ON_NDIS_ERROR(ndisStatus);
    }
    *pucChannel = *pucParam;

error:
    return ndisStatus;
}

NDIS_STATUS
Dot11CopySSIDFromMemoryBlob(
    __in PUCHAR pucInfoBlob,
    __in ULONG uSizeOfBlob,
    __in PDOT11_SSID pDot11SSID
    )
{
    UCHAR ucLength = 0;
    PUCHAR pucParam = NULL;
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    ndisStatus = Dot11GetInfoEle(
                    pucInfoBlob,
                    uSizeOfBlob,
                    DOT11_INFO_ELEMENT_ID_SSID,
                    &ucLength,
                    &pucParam
                    );
    BAIL_ON_NDIS_ERROR(ndisStatus);

    if (ucLength > DOT11_SSID_MAX_LENGTH) {
        ndisStatus = NDIS_STATUS_INVALID_LENGTH;
        BAIL_ON_NDIS_ERROR(ndisStatus);
    }

    pDot11SSID->uSSIDLength = (ULONG)ucLength;
    RtlCopyMemory(pDot11SSID->ucSSID, pucParam, pDot11SSID->uSSIDLength);

error:
    return ndisStatus;
}

NDIS_STATUS
Dot11AttachElement(
    __inout PUCHAR *ppucBlob,
    __inout USHORT *pusBlobSize,
    __in UCHAR ucElementId,
    __in UCHAR ucElementLength,
    __in PUCHAR pucElementInfo
    )
{
    PUCHAR      pucBlob = *ppucBlob;
    USHORT      usBlobSize = *pusBlobSize;

    if (usBlobSize < (2U + ucElementLength))
    {
        return STATUS_BUFFER_TOO_SMALL;
    }

    *pucBlob = ucElementId;
    pucBlob++; 
    usBlobSize--;
    
    *pucBlob = ucElementLength;
    pucBlob++; 
    usBlobSize--;
    
    memcpy(pucBlob, pucElementInfo, ucElementLength);
    pucBlob = pucBlob + ucElementLength;
    usBlobSize = usBlobSize - ucElementLength;

    *ppucBlob = pucBlob;
    *pusBlobSize = usBlobSize;

    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Dot11CopyInfoEle(
    __in PUCHAR pucInfoBlob,
    __in ULONG uSizeOfBlob,
    __in UCHAR ucInfoId,
    __out PUCHAR pucLength,
    __in ULONG uBufSize,
    __inout PVOID pvInfoEle
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    UCHAR ucLength = 0;
    PVOID pvTemp = NULL;

    ndisStatus = Dot11GetInfoEle(
            pucInfoBlob,
            uSizeOfBlob,
            ucInfoId,
            &ucLength,
            &pvTemp
            );
    BAIL_ON_NDIS_ERROR(ndisStatus);

    *pucLength = ucLength;
    if (uBufSize < ucLength) {
        ndisStatus = STATUS_BUFFER_TOO_SMALL;
        BAIL_ON_NDIS_ERROR(ndisStatus);
    }

    RtlCopyMemory(
            pvInfoEle,
            pvTemp,
            ucLength
            );

error:
    return ndisStatus;
}

NDIS_STATUS
Dot11GetWPAIE(
    __in PUCHAR pucInfoBlob,
    __in ULONG uSizeOfBlob,
    __out PUCHAR pucLength,
    __inout PVOID * ppvInfoEle
    )
{
    UCHAR ucLength = 0;
    PUCHAR pucParam = NULL;
    PUCHAR InfoElemPtr;
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    InfoElemPtr = pucInfoBlob;
    while (TRUE)
    {
        ndisStatus = Dot11GetInfoEle(InfoElemPtr,
                                     uSizeOfBlob - PtrOffset(pucInfoBlob, InfoElemPtr),
                                     DOT11_INFO_ELEMENT_ID_VENDOR_SPECIFIC,
                                     &ucLength,
                                     &pucParam);
        BAIL_ON_NDIS_ERROR(ndisStatus);

        //
        // WPA IE contains 4 bytes WPA_IE_TAG (0x01f25000) at the very beginning of the IE data. Check for the tag.
        // If we don't find the tag, it's not a WPA IE. Keep searching.
        //
        if (ucLength < 4 || *((ULONG UNALIGNED *)pucParam) != WPA_IE_TAG)
        {
            InfoElemPtr = pucParam + ucLength;
            continue;
        }

        ucLength -= 4;
        pucParam += 4;

        break;
    }

    *ppvInfoEle = pucParam;
    *pucLength = ucLength;

error:
    return ndisStatus;
}

