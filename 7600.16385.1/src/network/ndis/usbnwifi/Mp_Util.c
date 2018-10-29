/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Mp_Utils.h

Abstract:
    Utility functions
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
#include "precomp.h"

#include "Mp_Util.h"
#if DOT11_TRACE_ENABLED
#include "Mp_Util.tmh"
#endif


//
// Unable to include the following calls from the OS.
// We are defining them ourselves.
//
ULONG
FASTCALL
MpInterlockedSetClearBits (
    __inout PULONG Flags,
    __in ULONG sFlag,
    __in ULONG cFlag
    )
{

    ULONG NewFlags, OldFlags;

    OldFlags = * (volatile ULONG *) Flags;
    NewFlags = (OldFlags | sFlag) & ~cFlag;
    while (NewFlags != OldFlags) {
        NewFlags = InterlockedCompareExchange ((PLONG) Flags, (LONG) NewFlags, (LONG) OldFlags);
        if (NewFlags == OldFlags) {
            break;
        }

        OldFlags = NewFlags;
        NewFlags = (NewFlags | sFlag) & ~cFlag;
    }

    return OldFlags;
}


NDIS_STATUS
Dot11GetDataFromMdlChain(
    PNDIS_BUFFER * ppMdlChain,
    ULONG uOffset,
    ULONG uBytesNeeded,
    PVOID pvStorage,
    PULONG puLastWalkedMdlOffset,
    PUCHAR * ppucReturnBuf
    )
/*++

Routine Description:

    Retrieves data from an MDL chain.
    If the desired data is contiguous, then just returns
    a pointer directly to the data in the buffer chain.
    Otherwise the data is copied to the supplied buffer
    and returns a pointer to the supplied buffer.

Arguments:

    ppMdlChain - Pointer to a pointer to a chain of MDLs describing the source
                 data. On successful return, this points to the last walked MDL.

    uOffset - Number of initial bytes to skip in the MDL chain.

    uBytesNeeded - Number of bytes needed from the specified offset.

    pvStorage - Pointer to the flat buffer of uBytesNeeded size.
                Client of this call should free this buffer only when its
                done using *ppucReturnBuf.

    puLastWalkedMdlOffset - Pointer to a location that will contain the offset
                            into the last walked MDL from where the next send
                            buffer can be retrieved.

    ppucReturnBuf - Pointer to a location that will contain the pointer to
                    the flat buffer. Must not be freed by the client.
Return Value:

    Success - STATUS_SUCCESS.

    Failure - NT STATUS FAILURE CODE.

--*/
{
    NDIS_STATUS ndisStatus = STATUS_SUCCESS;

    PNDIS_BUFFER pMdl = *ppMdlChain;
    ULONG uMdlOffset = uOffset;

    ULONG uMdlByteCount = 0, uDummy;
    PUCHAR pucVa = NULL;
    PUCHAR pucReturnBuf = NULL;
    ULONG uLastWalkedMdlOffset = 0;
    ULONG uBytesCopied = 0;


    //
    // Find which MDL.
    //

    if (!pMdl) {
        ndisStatus = STATUS_BUFFER_TOO_SMALL;
        BAIL_ON_NDIS_ERROR(ndisStatus);
    }

    NdisQueryBufferSafe(pMdl, NULL, (PUINT)&uMdlByteCount, NormalPagePriority);
    while (uMdlOffset >= uMdlByteCount) {
        uMdlOffset -= uMdlByteCount;
        pMdl = pMdl->Next;
        if (!pMdl) {
            ndisStatus = STATUS_BUFFER_TOO_SMALL;
            BAIL_ON_NDIS_ERROR(ndisStatus);
        }
        else {
            NdisQueryBufferSafe(pMdl, NULL, (PUINT)&uMdlByteCount, NormalPagePriority);
        }
    }

    //
    // See if the found MDL contains uMdlOffset + uBytesNeeded bytes of data.
    //
    if (uMdlOffset + uBytesNeeded <= uMdlByteCount) {

        NdisQueryBufferSafe(pMdl, &pucVa, (PUINT)&uDummy, LowPagePriority);
        if (!pucVa) {
            ndisStatus = STATUS_INSUFFICIENT_RESOURCES;
            BAIL_ON_NDIS_ERROR(ndisStatus);
        }
        pucReturnBuf = pucVa + uMdlOffset;

        if (uMdlOffset + uBytesNeeded < uMdlByteCount) {
            uLastWalkedMdlOffset = uMdlOffset + uBytesNeeded;
        }
        else {
            pMdl = pMdl->Next;
            uLastWalkedMdlOffset = 0;
        }
    }
    else {

        if (!pvStorage) {
            ndisStatus = STATUS_BUFFER_OVERFLOW;
            BAIL_ON_NDIS_ERROR(ndisStatus);
        }

        ndisStatus = Dot11CopyMdlToBuffer(
                       &pMdl,
                       uMdlOffset,
                       pvStorage,
                       uBytesNeeded,
                       &uLastWalkedMdlOffset,
                       &uBytesCopied
                       );
        BAIL_ON_NDIS_ERROR(ndisStatus);

        if (uBytesCopied != uBytesNeeded) {
            ndisStatus = STATUS_BUFFER_TOO_SMALL;
            BAIL_ON_NDIS_ERROR(ndisStatus);
        }
        pucReturnBuf = pvStorage;

    }

    if (ppMdlChain) {
        *ppMdlChain = pMdl;
    }
    if (puLastWalkedMdlOffset) {
        *puLastWalkedMdlOffset = uLastWalkedMdlOffset;
    }
    if (ppucReturnBuf) {
        *ppucReturnBuf = pucReturnBuf;
    }
    return (ndisStatus);

error:

    *puLastWalkedMdlOffset = 0;
    *ppucReturnBuf = NULL;
    return (ndisStatus);
}


NDIS_STATUS
Dot11CopyMdlToBuffer(
    PNDIS_BUFFER * ppMdlChain,
    ULONG uOffset,
    PVOID pvBuffer,
    ULONG uBytesToCopy,
    PULONG puLastWalkedMdlOffset,
    PULONG puBytesCopied
    )
/*++

Routine Description:

    Copies a maximum of uBytesToCopy bytes of data from an MDL chain
    to a flat buffer.

Arguments:

    ppMdlChain - Pointer to a pointer to a chain of MDLs describing the source
                 data. On successfully copying uBytesToCopy bytes of data, this
                 points to the last walked MDL.

    uOffset - Number of initial bytes to skip in the MDL chain.

    pvBuffer - Pointer to the flat buffer to copy into.

    uBytesToCopy - Number of bytes to copy.

    puLastWalkedMdlOffset - Pointer to a location that will contain the offset
                            into the last walked MDL from where the next send
                            buffer can be retrieved.

    puBytesCopied - Pointer to a location to contain the actual number of bytes
                    copied.

Return Value:

    Success - STATUS_SUCCESS.

    Failure - NT STATUS FAILURE CODE.

--*/
{
    PNDIS_BUFFER pMdl = *ppMdlChain;
    ULONG uMdlOffset = uOffset;
    ULONG uMdlByteCount = 0, uDummy;
    ULONG uNumBytes = uBytesToCopy;
    PUCHAR pucSysVa = NULL;
    ULONG uCopySize = 0;
    PNDIS_BUFFER pLastWalkedMdl = pMdl; // Initialize to pMdl to avoid Prefast warning


    *puBytesCopied = 0;

    if (pMdl == NULL)
    {
        if (uBytesToCopy == 0)
        {
            return NDIS_STATUS_SUCCESS;
        }
        else
        {
            return NDIS_STATUS_FAILURE;
        }
    }

    //
    // Skip the offset bytes in the MDL chain.
    //    
    while (pMdl) {
        NdisQueryBufferSafe(pMdl, NULL, (PUINT)&uMdlByteCount, LowPagePriority);
        if (uMdlOffset >= uMdlByteCount)
        {
            uMdlOffset -= uMdlByteCount;
            pMdl = pMdl->Next;
        }
        else
            break;
    }

    while (pMdl) {

        if (uNumBytes > 0) {
            NdisQueryBufferSafe(pMdl, NULL, (PUINT)&uMdlByteCount, LowPagePriority); 
            if (uMdlByteCount == 0) {
                pLastWalkedMdl = pMdl;
                pMdl = pMdl->Next;
                continue;
            }

            NdisQueryBufferSafe(pMdl, &pucSysVa, (PUINT)&uDummy, LowPagePriority);
            if (!pucSysVa) {
                return (STATUS_INSUFFICIENT_RESOURCES);
            }

            pucSysVa += uMdlOffset;
            uMdlByteCount -= uMdlOffset;
            uMdlOffset = 0;

            //
            // uMdlByteCount can never be zero because at this point its always
            // greater than uMdlOffset.
            //

            uCopySize = MIN(uNumBytes, uMdlByteCount);
            NdisMoveMemory(pvBuffer, pucSysVa, uCopySize);
            pvBuffer = (PVOID) ((PUCHAR) pvBuffer + uCopySize);
            uNumBytes -= uCopySize;

            pLastWalkedMdl = pMdl;
            pMdl = pMdl->Next;
        }
        else
            break;
    }

    if (!uNumBytes) {

        if (uCopySize < uMdlByteCount) {
            *ppMdlChain = pLastWalkedMdl;
            *puLastWalkedMdlOffset = uCopySize;
        }
        else {
            pLastWalkedMdl = pLastWalkedMdl->Next;
            *ppMdlChain = pLastWalkedMdl;
            *puLastWalkedMdlOffset = 0;
        }
    }

    *puBytesCopied = uBytesToCopy - uNumBytes;

    return (STATUS_SUCCESS);
}

