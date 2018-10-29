/*++

Copyright (c) 1990-2001  Microsoft Corporation
All Rights Reserved

Module Name:

    mem.h

--*/

#ifndef _MEM_H_
#define _MEM_H_

LPWSTR
AllocSplStr(
    IN LPCWSTR pStr
    );

LPVOID
AllocSplMem(
    IN DWORD cbAlloc
    );

BOOL
FreeSplStr(
    __in    LPWSTR pStr
    );

BOOL
FreeSplMem(
    __in    PVOID pMem
    );

#endif // _MEM_H_

