/*++

Copyright (c) 1990-2001  Microsoft Corporation
All Rights Reserved

Module Name:

    mem.c

--*/

#include "precomp.h"
#pragma hdrstop

#include <DriverSpecs.h>
__user_driver

#include "mem.h"


LPWSTR
AllocSplStr(
    IN  LPCWSTR pStr
    )
/*++

Routine Description:

    This function will allocate memory to store the specified
    string, and copy that string to the allocated memory

Arguments:

    pStr - Pointer to the string that needs to be allocated and stored

Return Value:

    NON-NULL - A pointer to the allocated memory containing the string

    FALSE/NULL - The operation failed. Extended error status is available
    using GetLastError.

--*/
{
    LPWSTR pMem;
    DWORD  cbStr;

    if (!pStr) {
        return NULL;
    }

    cbStr = (DWORD)(wcslen(pStr)*sizeof(WCHAR) + sizeof(WCHAR));

    pMem = (LPWSTR)AllocSplMem( cbStr );

    if (pMem) {
        CopyMemory( pMem, pStr, cbStr );
    }
    return pMem;
}

BOOL
FreeSplStr(
    __in    LPWSTR pStr
    )
/*++

Routine Description:

    This function will release the memory allocated with a call
    to AllocSplStr

Arguments:

    pStr - Pointer to the string to release

Return Value:

    TRUE memory was release, FALSE an error occurred, use GetLastError
    for extended error information.

--*/
{
    return FreeSplMem(pStr);
}

LPVOID
AllocSplMem(
    DWORD cbAlloc
    )
/*++

Routine Description:

    This function allocates the specified amount of memory.

Arguments:

    cbAlloc - number in byte of memory to allocate

Return Value:

    Pointer to newly alloated memory block on success, NULL on failure use
    GetLastError for extended error information.

--*/
{
    PVOID pvMemory;

    pvMemory = GlobalAlloc(GMEM_FIXED, cbAlloc);

    if( pvMemory ){
        ZeroMemory( pvMemory, cbAlloc );
    }

    return pvMemory;
}

BOOL
FreeSplMem(
    __in_opt    PVOID pMem
    )
/*++

Routine Description:

    This function release the specified memory block.

Arguments:

    pMem - pointer to memory block

Return Value:

    TRUE memoy block was release, FALSE on failure.

--*/
{
    BOOL bRet = TRUE;

    if (pMem)
    {
        bRet = GlobalFree( pMem ) ? FALSE : TRUE;
    }

    return bRet;
}

