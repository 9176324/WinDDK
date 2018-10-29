/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Mp_Util.h

Abstract:
    Mp layer utility functions
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
#ifndef _MP_UTIL_H_
#define _MP_UTIL_H_

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

INLINE NDIS_STATUS
Dot11GetMacHeaderFromNB(
    IN  PNET_BUFFER                     NetBuffer,
    IN  PDOT11_MAC_HEADER*              ppDot11MacHeader
    )
{
    NDIS_STATUS         ndisStatus = NDIS_STATUS_SUCCESS;
    PUCHAR              pucMacHdr;
    PMDL                pMdl;

    do
    {
        *ppDot11MacHeader = NULL;
        pMdl = NET_BUFFER_CURRENT_MDL(NetBuffer);
        
        MPASSERTMSG("The entire Dot11 Mac header must be present in the first MDL\n", NET_BUFFER_DATA_LENGTH(NetBuffer) >= sizeof(DOT11_MAC_HEADER));
        
        pucMacHdr = (PUCHAR) MmGetSystemAddressForMdlSafe(pMdl, NormalPagePriority);
        if (pucMacHdr == NULL)
        {
            //
            // Low on PTE resources. Fail request
            //
            ndisStatus = NDIS_STATUS_RESOURCES;
            break;
        }
        
        *ppDot11MacHeader = (PDOT11_MAC_HEADER)(pucMacHdr + NET_BUFFER_CURRENT_MDL_OFFSET(NetBuffer));
    } WHILE(FALSE);

    return ndisStatus;
}

#endif  // _MP_UTIL_H_

