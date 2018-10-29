/*++

Copyright (c) Microsoft 1998, All Rights Reserved

Module Name:

    strings.h

Abstract:

    This module contains the public function definitions for the routines
    in strings.c that handle conversion of integer/data buffer to/from 
    string represenation

Environment:

    User mode

Revision History:

    May-98 : Created 

--*/

#ifndef __STRINGS_H__
#define __STRINGS_H__

VOID
Strings_CreateDataBufferString(
    __in_bcount(DataBufferLength) PCHAR    DataBuffer,
    __in  ULONG    DataBufferLength,
    __in  ULONG    NumBytesToDisplay,
    __in  ULONG    DisplayBlockSize,
    __deref_out LPSTR  *BufferString
);

BOOL
Strings_StringToUnsignedList(
    __inout LPSTR   InString,
    __in    ULONG   UnsignedSize,
    __in    ULONG   Base,
    __deref_out_bcount(*nUnsigneds) PCHAR *UnsignedList,
    __out   PULONG  nUnsigneds
);

#endif

