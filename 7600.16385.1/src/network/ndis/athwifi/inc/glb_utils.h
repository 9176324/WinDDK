/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    glb_utils.h

Abstract:
    Contains utility functions used by the whole driver
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    09-04-2007    Created

Notes:

--*/
#pragma once

ULONG
MpReadRegistry(
    __in  PVOID                   StructureStart,
    __in  NDIS_HANDLE             ConfigurationHandle,
    __in  PMP_REG_ENTRY           RegKeyList,
    __in  ULONG                   NumKeys
    );

NDIS_STATUS
Dot11CopyMdlToBuffer(
    PNDIS_BUFFER * ppMdlChain,
    ULONG uOffset,
    PVOID pvBuffer,
    ULONG uBytesToCopy,
    PULONG puLastWalkedMdlOffset,
    PULONG puBytesCopied
    );

NDIS_STATUS
Dot11GetDataFromMdlChain(
    PNDIS_BUFFER * ppMdlChain,
    ULONG uOffset,
    ULONG uBytesNeeded,
    PVOID pvStorage,
    PULONG puLastWalkedMdlOffset,
    PUCHAR * ppucReturnBuf
    );

NDIS_STATUS
Dot11GetMacHeaderFromNB(
    __in  PNET_BUFFER                     NetBuffer,
    __in  PDOT11_MAC_HEADER*              ppDot11MacHeader
    );

PVOID
Dot11GetNetBufferData(
    __in  PNET_BUFFER                     NetBuffer,
    __in  ULONG                           BytesNeeded
    );



#define WPA_IE_TAG      0x01f25000
#define WCN_IE_TAG      0x04f25000

//
// Dot11ValidateInfoBlob and Dot11ValidatePacketInfoBlob
//  Validate the information blob. Dot11ValidateInfoBlob works
//  on a contiguous buffer while Dot11ValidatePacketInfoBlob
//  works on a MDL chain.
//
NDIS_STATUS
Dot11ValidateInfoBlob(
    __in_bcount(uSizeOfBlob) PUCHAR pucInfoBlob,
    __in ULONG uSizeOfBlob
    );

NDIS_STATUS
Dot11GetInfoBlobSize(
    __in_bcount(PacketLength)  PUCHAR                  pPacketBuffer,
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
    __in_bcount(uSizeOfBlob) PUCHAR pucInfoBlob,
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
    __out PVOID * ppvInfoEle
    );

NDIS_STATUS
Dot11CopySSIDFromPacket(
    __in PNDIS_BUFFER pMdlHead,
    __in ULONG uOffsetOfInfoEleBlob,
    __out PDOT11_SSID pDot11SSID
    );

NDIS_STATUS
Dot11GetChannelForDSPhy(
    __in_bcount(uSizeOfBlob) PUCHAR pucInfoBlob,
    __in ULONG uSizeOfBlob,
    __out PUCHAR pucChannel
    );

NDIS_STATUS
Dot11CopySSIDFromMemoryBlob(
    __in_bcount(uSizeOfBlob) PUCHAR pucInfoBlob,
    __in ULONG uSizeOfBlob,
    __out PDOT11_SSID pDot11SSID
    );

PUCHAR
Dot11GetBSSID(
    __in_bcount(fragmentLength) PUCHAR fragmentHeader,
    __in ULONG fragmentLength
    );

NDIS_STATUS
Dot11AttachElement(
    __inout PUCHAR *ppucBlob,
    __inout USHORT *pusBlobSize,
    __in UCHAR ucElementId,
    __in UCHAR ucElementLength,
    __in_bcount(ucElementLength) PUCHAR pucElementInfo
    );

NDIS_STATUS
Dot11CopyInfoEle(
    __in_bcount(uSizeOfBlob) PUCHAR pucInfoBlob,
    __in ULONG uSizeOfBlob,
    __in UCHAR ucInfoId,
    __out PUCHAR pucLength,
    __in ULONG uBufSize,
    __out PVOID pvInfoEle
    );

NDIS_STATUS
Dot11GetWPAIE(
    __in_bcount(uSizeOfBlob) PUCHAR pucInfoBlob,
    __in ULONG uSizeOfBlob,
    __out PUCHAR pucLength,
    __out PVOID * ppvInfoEle
    );

BOOLEAN
Dot11IsHiddenSSID(
    __in_bcount(SSIDLength) PUCHAR               SSID,
    __in ULONG                SSIDLength
    );

NDIS_STATUS
Dot11GetRateSetFromInfoEle(
    __in_bcount(InfoElemBlobSize)  PUCHAR InfoElemBlobPtr,
    __in  ULONG InfoElemBlobSize,
    __in  BOOLEAN basicRateOnly,
    __out PDOT11_RATE_SET rateSet
    );

VOID
Dot11GenerateRandomBSSID(
    __in_bcount(DOT11_ADDRESS_SIZE)  DOT11_MAC_ADDRESS MACAddr,
    __out_bcount(DOT11_ADDRESS_SIZE) DOT11_MAC_ADDRESS BSSID
    );

NDIS_STATUS
Dot11ParseRNSIE(
    __in_bcount(RSNIELength) PUCHAR RSNIEData,
    __in ULONG OUIPrefix,
    __in UCHAR RSNIELength,
    __out PRSN_IE_INFO RSNIEInfo
    );

DOT11_CIPHER_ALGORITHM
Dot11GetGroupCipherFromRSNIEInfo(
    __in PRSN_IE_INFO RSNIEInfo
    );

DOT11_CIPHER_ALGORITHM
Dot11GetPairwiseCipherFromRSNIEInfo(
    __in PRSN_IE_INFO RSNIEInfo,
    __in USHORT index
    );

DOT11_AUTH_ALGORITHM
Dot11GetAKMSuiteFromRSNIEInfo(
    __in PRSN_IE_INFO RSNIEInfo,
    __in USHORT index
    );

ULONG
Dot11GetRSNOUIFromCipher(
    __in ULONG prefix,
    __in DOT11_CIPHER_ALGORITHM cipher
    );

ULONG
Dot11GetRSNOUIFromAuthAlgo(
    __in DOT11_AUTH_ALGORITHM algo
    );

NDIS_STATUS
MpCreateThread(
    __in PKSTART_ROUTINE StartFunc,
    __in PVOID Context,
    __in KPRIORITY Priority,
    __deref_out PKTHREAD* Thread
    );

