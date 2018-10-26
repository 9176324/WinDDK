/*++

Copyright (c) 1997  Microsoft Corporation

Module Name:

    debug.c

Abstract:

    This module contains all debug-related code.

Revision History:

    Who         When        What
    --------    --------    ----------------------------------------------
    arvindm        05-29-97    Created, based on ATM ARP.

Notes:

--*/

#include <precomp.h>

#define __FILENUMBER 'GBED'

#if DBG

INT                 ndisprotDebugLevel=DL_WARN;

NDIS_SPIN_LOCK        ndisprotDbgLogLock;

PNUIOD_ALLOCATION    ndisprotdMemoryHead = (PNUIOD_ALLOCATION)NULL;
PNUIOD_ALLOCATION    ndisprotdMemoryTail = (PNUIOD_ALLOCATION)NULL;
ULONG                ndisprotdAllocCount = 0;    // how many allocated so far (unfreed)

NDIS_SPIN_LOCK        ndisprotdMemoryLock;
BOOLEAN                ndisprotdInitDone = FALSE;


PVOID
ndisprotAuditAllocMem(
    PVOID    pPointer,
    ULONG    Size,
    ULONG    FileNumber,
    ULONG    LineNumber
)
{
    PVOID                pBuffer;
    PNUIOD_ALLOCATION    pAllocInfo;

    if (!ndisprotdInitDone)
    {
        NdisAllocateSpinLock(&(ndisprotdMemoryLock));
        ndisprotdInitDone = TRUE;
    }

    NdisAllocateMemoryWithTag(
        (PVOID *)&pAllocInfo,
        Size+sizeof(NUIOD_ALLOCATION),
        (ULONG)'oiuN'
    );

    if (pAllocInfo == (PNUIOD_ALLOCATION)NULL)
    {
        DEBUGP(DL_VERY_LOUD+50,
            ("ndisprotAuditAllocMem: file %d, line %d, Size %d failed!\n",
                FileNumber, LineNumber, Size));
        pBuffer = NULL;
    }
    else
    {
        pBuffer = (PVOID)&(pAllocInfo->UserData);
        NPROT_SET_MEM(pBuffer, 0xaf, Size);
        pAllocInfo->Signature = NUIOD_MEMORY_SIGNATURE;
        pAllocInfo->FileNumber = FileNumber;
        pAllocInfo->LineNumber = LineNumber;
        pAllocInfo->Size = Size;
        pAllocInfo->Location = (ULONG_PTR)pPointer;
        pAllocInfo->Next = (PNUIOD_ALLOCATION)NULL;

        NdisAcquireSpinLock(&(ndisprotdMemoryLock));

        pAllocInfo->Prev = ndisprotdMemoryTail;
        if (ndisprotdMemoryTail == (PNUIOD_ALLOCATION)NULL)
        {
            // empty list
            ndisprotdMemoryHead = ndisprotdMemoryTail = pAllocInfo;
        }
        else
        {
            ndisprotdMemoryTail->Next = pAllocInfo;
        }
        ndisprotdMemoryTail = pAllocInfo;
        
        ndisprotdAllocCount++;
        NdisReleaseSpinLock(&(ndisprotdMemoryLock));
    }

    DEBUGP(DL_VERY_LOUD+100,
     ("ndisprotAuditAllocMem: file %c%c%c%c, line %d, %d bytes, [0x%x] <- 0x%x\n",
                 (CHAR)(FileNumber & 0xff),
                 (CHAR)((FileNumber >> 8) & 0xff),
                 (CHAR)((FileNumber >> 16) & 0xff),
                 (CHAR)((FileNumber >> 24) & 0xff),
                LineNumber, Size, pPointer, pBuffer));

    return (pBuffer);

}


VOID
ndisprotAuditFreeMem(
    PVOID    Pointer
)
{
    PNUIOD_ALLOCATION    pAllocInfo;

    NdisAcquireSpinLock(&(ndisprotdMemoryLock));

    pAllocInfo = CONTAINING_RECORD(Pointer, NUIOD_ALLOCATION, UserData);

    if (pAllocInfo->Signature != NUIOD_MEMORY_SIGNATURE)
    {
        DEBUGP(DL_ERROR,
         ("ndisprotAuditFreeMem: unknown buffer 0x%x!\n", Pointer));
        NdisReleaseSpinLock(&(ndisprotdMemoryLock));
#if DBG
        DbgBreakPoint();
#endif
        return;
    }

    pAllocInfo->Signature = (ULONG)'DEAD';
    if (pAllocInfo->Prev != (PNUIOD_ALLOCATION)NULL)
    {
        pAllocInfo->Prev->Next = pAllocInfo->Next;
    }
    else
    {
        ndisprotdMemoryHead = pAllocInfo->Next;
    }
    if (pAllocInfo->Next != (PNUIOD_ALLOCATION)NULL)
    {
        pAllocInfo->Next->Prev = pAllocInfo->Prev;
    }
    else
    {
        ndisprotdMemoryTail = pAllocInfo->Prev;
    }
    ndisprotdAllocCount--;
    NdisReleaseSpinLock(&(ndisprotdMemoryLock));

    NdisFreeMemory(pAllocInfo, 0, 0);
}


VOID
ndisprotAuditShutdown(
    VOID
)
{
    if (ndisprotdInitDone)
    {
        if (ndisprotdAllocCount != 0)
        {
            DEBUGP(DL_ERROR, ("AuditShutdown: unfreed memory, %d blocks!\n",
                    ndisprotdAllocCount));
            DEBUGP(DL_ERROR, ("MemoryHead: 0x%x, MemoryTail: 0x%x\n",
                    ndisprotdMemoryHead, ndisprotdMemoryTail));
            DbgBreakPoint();
            {
                PNUIOD_ALLOCATION        pAllocInfo;

                while (ndisprotdMemoryHead != (PNUIOD_ALLOCATION)NULL)
                {
                    pAllocInfo = ndisprotdMemoryHead;
                    DEBUGP(DL_INFO, ("AuditShutdown: will free 0x%x\n", pAllocInfo));
                    ndisprotAuditFreeMem(&(pAllocInfo->UserData));
                }
            }
        }
        ndisprotdInitDone = FALSE;
    }
}

#define MAX_HD_LENGTH        128

VOID
DbgPrintHexDump(
    IN    PUCHAR            pBuffer,
    IN    ULONG            Length
)
/*++

Routine Description:

    Print a hex dump of the given contiguous buffer. If the length
    is too long, we truncate it.

Arguments:

    pBuffer            - Points to start of data to be dumped
    Length            - Length of above.

Return Value:

    None

--*/
{
    ULONG        i;

    if (Length > MAX_HD_LENGTH)
    {
        Length = MAX_HD_LENGTH;
    }

    for (i = 0; i < Length; i++)
    {
        //
        //  Check if we are at the end of a line
        //
        if ((i > 0) && ((i & 0xf) == 0))
        {
            DbgPrint("\n");
        }

        //
        //  Print addr if we are at start of a new line
        //
        if ((i & 0xf) == 0)
        {
            DbgPrint("%08x ", pBuffer);
        }

        DbgPrint(" %02x", *pBuffer++);
    }

    //
    //  Terminate the last line.
    //
    if (Length > 0)
    {
        DbgPrint("\n");
    }
}
#endif // DBG


#if DBG_SPIN_LOCK
ULONG    ndisprotdSpinLockInitDone = 0;
NDIS_SPIN_LOCK    ndisprotdLockLock;

VOID
ndisprotAllocateSpinLock(
    IN    PNPROT_LOCK        pLock,
    IN    ULONG                FileNumber,
    IN    ULONG                LineNumber
)
{
    if (ndisprotdSpinLockInitDone == 0)
    {
        ndisprotdSpinLockInitDone = 1;
        NdisAllocateSpinLock(&(ndisprotdLockLock));
    }

    NdisAcquireSpinLock(&(ndisprotdLockLock));
    pLock->Signature = NUIOL_SIG;
    pLock->TouchedByFileNumber = FileNumber;
    pLock->TouchedInLineNumber = LineNumber;
    pLock->IsAcquired = 0;
    pLock->OwnerThread = 0;
    NdisAllocateSpinLock(&(pLock->NdisLock));
    NdisReleaseSpinLock(&(ndisprotdLockLock));
}

VOID
ndisprotFreeSpinLock(
    IN    PNPROT_LOCK        pLock,
    IN    ULONG                FileNumber,
    IN    ULONG                LineNumber
)
{

    NdisAcquireSpinLock(&(ndisprotdLockLock));
    pLock->Signature = NUIOL_SIG;
    pLock->TouchedByFileNumber = FileNumber;
    pLock->TouchedInLineNumber = LineNumber;
    pLock->IsAcquired = 0;
    pLock->OwnerThread = 0;
    NdisFreeSpinLock(&(pLock->NdisLock));
    NdisReleaseSpinLock(&(ndisprotdLockLock));
}

VOID
ndisprotFreeDbgLock(
    VOID
    )
{
    
    ASSERT(ndisprotdSpinLockInitDone == 1);
    
    ndisprotdSpinLockInitDone = 0;
    NdisFreeSpinLock(&(ndisprotdLockLock));
}


VOID
ndisprotAcquireSpinLock(
    IN    PNPROT_LOCK        pLock,
    IN    ULONG                FileNumber,
    IN    ULONG                LineNumber
)
{
    PKTHREAD        pThread;

    pThread = KeGetCurrentThread();
    NdisAcquireSpinLock(&(ndisprotdLockLock));
    if (pLock->Signature != NUIOL_SIG)
    {
        DbgPrint("Trying to acquire uninited lock 0x%x, File %c%c%c%c, Line %d\n",
                pLock,
                (CHAR)(FileNumber & 0xff),
                (CHAR)((FileNumber >> 8) & 0xff),
                (CHAR)((FileNumber >> 16) & 0xff),
                (CHAR)((FileNumber >> 24) & 0xff),
                LineNumber);
        DbgBreakPoint();
    }

    if (pLock->IsAcquired != 0)
    {
        if (pLock->OwnerThread == pThread)
        {
            DbgPrint("Detected multiple locking!: pLock 0x%x, File %c%c%c%c, Line %d\n",
                pLock,
                (CHAR)(FileNumber & 0xff),
                (CHAR)((FileNumber >> 8) & 0xff),
                (CHAR)((FileNumber >> 16) & 0xff),
                (CHAR)((FileNumber >> 24) & 0xff),
                LineNumber);
            DbgPrint("pLock 0x%x already acquired in File %c%c%c%c, Line %d\n",
                pLock,
                (CHAR)(pLock->TouchedByFileNumber & 0xff),
                (CHAR)((pLock->TouchedByFileNumber >> 8) & 0xff),
                (CHAR)((pLock->TouchedByFileNumber >> 16) & 0xff),
                (CHAR)((pLock->TouchedByFileNumber >> 24) & 0xff),
                pLock->TouchedInLineNumber);
            DbgBreakPoint();
        }
    }

    pLock->IsAcquired++;

    NdisReleaseSpinLock(&(ndisprotdLockLock));
    NdisAcquireSpinLock(&(pLock->NdisLock));

    //
    //  Mark this lock.
    //
    pLock->OwnerThread = pThread;
    pLock->TouchedByFileNumber = FileNumber;
    pLock->TouchedInLineNumber = LineNumber;
}


VOID
ndisprotReleaseSpinLock(
    IN    PNPROT_LOCK        pLock,
    IN    ULONG                FileNumber,
    IN    ULONG                LineNumber
)
{
    NdisDprAcquireSpinLock(&(ndisprotdLockLock));
    if (pLock->Signature != NUIOL_SIG)
    {
        DbgPrint("Trying to release uninited lock 0x%x, File %c%c%c%c, Line %d\n",
                pLock,
                (CHAR)(FileNumber & 0xff),
                (CHAR)((FileNumber >> 8) & 0xff),
                (CHAR)((FileNumber >> 16) & 0xff),
                (CHAR)((FileNumber >> 24) & 0xff),
                LineNumber);
        DbgBreakPoint();
    }

    if (pLock->IsAcquired == 0)
    {
        DbgPrint("Detected release of unacquired lock 0x%x, File %c%c%c%c, Line %d\n",
                pLock,
                (CHAR)(FileNumber & 0xff),
                (CHAR)((FileNumber >> 8) & 0xff),
                (CHAR)((FileNumber >> 16) & 0xff),
                (CHAR)((FileNumber >> 24) & 0xff),
                LineNumber);
        DbgBreakPoint();
    }
    pLock->TouchedByFileNumber = FileNumber;
    pLock->TouchedInLineNumber = LineNumber;
    pLock->IsAcquired--;
    pLock->OwnerThread = 0;
    NdisDprReleaseSpinLock(&(ndisprotdLockLock));

    NdisReleaseSpinLock(&(pLock->NdisLock));
}
#endif // DBG_SPIN_LOCK


