/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Mp_Element.h

Abstract:
    Prototypes of information element processing functions
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
#ifndef _NATIVE_WIFI_UTIL_H_

#define _NATIVE_WIFI_UTIL_H_

#define WPA_IE_TAG      0x01f25000

//
// Dot11ValidateInfoBlob and Dot11ValidatePacketInfoBlob
//  Validate the information blob. Dot11ValidateInfoBlob works
//  on a contiguous buffer while Dot11ValidatePacketInfoBlob
//  works on a MDL chain.
//
NDIS_STATUS
Dot11ValidateInfoBlob(
    __in PUCHAR pucInfoBlob,
    __in ULONG uOffsetOfInfoEleBlob
    );

NDIS_STATUS
Dot11GetInfoBlobSize(
    __in  PUCHAR                  pPacketBuffer,
    __in  ULONG                   PacketLength,
    __in  ULONG                   OffsetOfInfoElemBlob,
    __out PULONG                  pInfoElemBlobSize
    );

NDIS_STATUS
Dot11ValidatePacketInfoBlob(
    __in PNDIS_BUFFER pMdlHead,
    __in ULONG uOffsetOfInfoEleBlob
    );

//
// Dot11GetInfoEle and Dot11GetInfoEleFromPacket
//      Look up a particular information element in the
//      information blob.
//
// Dot11GetInfoEle works on a contiguous buffer while
// Dot11GetInfoEleFromPacket works on a MDL chain
//
NDIS_STATUS
Dot11GetInfoEle(
    __in PUCHAR pucInfoBlob,
    __in ULONG uSizeOfBlob,
    __in UCHAR ucInfoId,
    __out PUCHAR pucLength,
    __deref_out_opt PVOID * ppvInfoEle
    );

NDIS_STATUS
Dot11GetInfoEleFromPacket(
    __in PNDIS_BUFFER pMdlHead,
    __in ULONG uOffsetOfInfoEleBlob,
    __in UCHAR ucInfoId,
    __in UCHAR ucMaxLength,
    __out PUCHAR pucLength,
    __inout PVOID * ppvInfoEle
    );

NDIS_STATUS
Dot11CopySSIDFromPacket(
    __in PNDIS_BUFFER pMdlHead,
    __in ULONG uOffsetOfInfoEleBlob,
    __in PDOT11_SSID pDot11SSID
    );

NDIS_STATUS
Dot11GetChannelForDSPhy(
    __in PUCHAR pucInfoBlob,
    __in ULONG uSizeOfBlob,
    __in PUCHAR pucChannel
    );

NDIS_STATUS
Dot11CopySSIDFromMemoryBlob(
    __in PUCHAR pucInfoBlob,
    __in ULONG uSizeOfBlob,
    __in PDOT11_SSID pDot11SSID
    );

NDIS_STATUS
Dot11AttachElement(
    __inout PUCHAR *ppucBlob,
    __inout USHORT *pusBlobSize,
    __in UCHAR ucElementId,
    __in UCHAR ucElementLength,
    __in PUCHAR pucElementInfo
    );

NDIS_STATUS
Dot11CopyInfoEle(
    __in PUCHAR pucInfoBlob,
    __in ULONG uSizeOfBlob,
    __in UCHAR ucInfoId,
    __out PUCHAR pucLength,
    __in ULONG uBufSize,
    __inout PVOID pvInfoEle
    );

NDIS_STATUS
Dot11GetWPAIE(
    __in PUCHAR pucInfoBlob,
    __in ULONG uSizeOfBlob,
    __out PUCHAR pucLength,
    __inout PVOID * ppvInfoEle
    );

#endif  // _NATIVE_WIFI_UTIL_H_

