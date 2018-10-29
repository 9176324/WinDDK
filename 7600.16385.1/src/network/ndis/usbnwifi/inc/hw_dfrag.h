#ifndef __INC_DEFRAG_H
#define __INC_DEFRAG_H

#define MaxDefragLifeTime           51200   // in us


VOID
DefragInitialize(
    PNIC        Nic
    );

PRT_RFD
DefragAddRFD(
    PNIC        Nic,
    PRT_RFD         pRfd,
    PUCHAR         pSenderAddr,
    USHORT          SeqNum,
    UCHAR          FragNum,
    BOOLEAN         bMoreFrag,
    BOOLEAN         bEncrypted
    );

VOID
DefragReset(
    PNIC        Nic
    );


/*
 *  Note:   Following functions should not be called outside Defrag.c
 *
*/
VOID
DefragInit(
    PDEFRAG_ENTRY   pDefragArray,
    USHORT          Size
    );

PDEFRAG_ENTRY
DefragSearch(
    PDEFRAG_ENTRY   pDefragArray,
    USHORT          Size,
    PUCHAR         pSenderAddr,
    USHORT          SeqNum,
    UCHAR          FragNum
    );

PDEFRAG_ENTRY
DefragFindFreeEntry(
    PDEFRAG_ENTRY   pDefragArray,
    USHORT          Size
    );

VOID
DefragAge(
    PDEFRAG_ENTRY   pDefragArray,
    USHORT          Size,
    ULONGLONG          usCurrentTime,
    PNIC        Nic
    );

VOID
DefragEntrySetRFD(
    PDEFRAG_ENTRY   pEntry,
    PRT_RFD         pRfd,
    PUCHAR         pSenderAddr,
    USHORT          SeqNum,
    UCHAR          FragNum 
    );

VOID
DefragEntryAddRFD(
    PDEFRAG_ENTRY   pEntry,
    PRT_RFD         pRfd,
    UCHAR          FragNum 
    );

VOID
DefragEntryFree(
    PDEFRAG_ENTRY   pEntry,
    PNIC        Nic 
    );


#endif // #ifndef __INC_DEFRAG_H

