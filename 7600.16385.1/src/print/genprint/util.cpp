/*++

Copyright (c) 1990-2003  Microsoft Corporation
All rights reserved

Module Name:

   util.c

--*/

#include "local.h"

LPWSTR
AllocSplStr(
    __in LPCWSTR pStr
    )

/*++

Routine Description:

    This function will allocate enough local memory to store the specified
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
    SIZE_T cbStr;

    if (!pStr) {
        return NULL;
    }

    cbStr = wcslen(pStr)*sizeof(WCHAR) + sizeof(WCHAR);

    if ((pMem = (LPWSTR) AllocSplMem(cbStr)) != NULL) {
        CopyMemory( pMem, pStr, cbStr );
    }
    return pMem;
}



LPVOID
AllocSplMem(
    SIZE_T cbAlloc
    )

{
    PVOID pvMemory;

    pvMemory = GlobalAlloc(GMEM_FIXED, cbAlloc);

    if( pvMemory ){
        ZeroMemory( pvMemory, cbAlloc );
    }

    return pvMemory;
}


/*++
Routine Name:
    HLoadLibraryFromSystemDirectory

Routine Description:

    Loads the specified dll from the system directory (e.g. %windir%\system32 for a 32bit system).
    Note: This routine is an internal routine and doesn't do full validation of the input file name.

Arguments:

    szDllName - The name of the dll. This dll should not include any PATH_SEPARATOR character
                i.e. it should be only be the name of the dll, without any path.

Comments:
    The input dll name should be less than MAX_PATH characters and the system directory path should be less 
    than MAX_PATH characters. Their combined length should also be less than MAX_PATH.

Return Value:

    On Success : The handle of the loaded library. 
    On Failure : NULL handle.

--*/

HINSTANCE
HLoadLibraryFromSystemDirectory (
    __in        LPCWSTR        szDllName)
{
    WCHAR       szFQDllName[MAX_PATH]   = {0};
    size_t      sysPathLength           = 0;
    size_t      dllNameLength           = 0;
    HINSTANCE   hLibrary                = (HINSTANCE)NULL;
    HRESULT     hResult                 = S_OK;

    if ( NULL == szDllName )
    {
        hResult = E_FAIL; 
    }

    if ( SUCCEEDED(hResult) )
    {
        hResult = StringCchLength(szDllName, MAX_PATH-1, &dllNameLength);
    }

    if ( SUCCEEDED(hResult) )
    {
        if (  NULL != wcsstr( (PWCHAR)szDllName, L"\\" ) ) //Make sure there is no \ in dll name.
        {
            hResult = E_FAIL;
        }
    }

    if ( SUCCEEDED(hResult) )
    {

        sysPathLength = (size_t) GetSystemDirectory( szFQDllName, MAX_PATH); //uSize has to be MAX_PATH according to MSDN.

        if ( sysPathLength                 == 0           || // GetSystemDirectory fails
             sysPathLength                 >= MAX_PATH-1  || // To prevent integer overflow in next if
             sysPathLength + dllNameLength >= MAX_PATH-1)
        {
            hResult = E_FAIL;
        }
    }

    if ( SUCCEEDED(hResult) )
    {
        szFQDllName[sysPathLength] = L'\\';
        hResult                    = StringCchCat ( szFQDllName, CCHOF(szFQDllName), szDllName);
    }

    if ( SUCCEEDED(hResult) )
    {
        hLibrary = LoadLibraryEx(szFQDllName, NULL, 0);
    }

    return hLibrary;
}
 

/*++
Routine Name:
    LoadLibGetJobAttributesEx

Routine Description:

    Loads spoolss.dll and calls its exported function GetJobAttributesEx. 

Arguments:

    pEMFAttr,
    pPrinterName,
    pDevmode,
    pAttributeInfo

Comments:
    
    GetJobAttributesEx was added to the spooler (spoolss.dll) after Windows2003.
    For OS including Windows2003 and earlier, GetJobAttributesEx was not available.
    So any print processor built from Windows2003+ DDK will not run on
    earlier OSes because print processor will not find GetJobAttributesEx
    exported by spooler. So we have to do LoadLibrary/GetProcAddress.

Return Value:

    On Success : TRUE. If TRUE is returned then
                 a. pAttributeInfo is filled 
                 b. pEMFAttr->hSpoolss and pEMFAttr->pfnGetJobAttributesEx set properly.

    On Failure : FALSE. This could mean that either LoadLibrary failed, GetProcAddress failed or 
                 GetJobAttributesEx failed. If FALSE is returned, we could still have set 
                 pEMFAttr->hSpoolss OR both pEMFAttr->hSpoolss and pEMFAttr->pfnGetJobAttributesEx.

--*/

BOOL
LoadLibGetJobAttributesEx(
    __inout  PEMF_ATTRIBUTE_INFO  pEMFAttr,
    __in     LPCWSTR              pPrinterName,
    __in     LPDEVMODE            pDevmode,
    __out    PATTRIBUTE_INFO_4    pAttributeInfo
    )
{
    HMODULE               hSpoolss              = NULL;
    BOOL                  bRetVal               = FALSE;
    PFNGetJobAttributesEx pfnGetJobAttributesEx = NULL;

    //
    // GetJobAttributesEx was added to the spooler (spoolss.dll) after Windows2003. 
    // For OS including Windows2003 and earlier, GetJobAttributesEx was not available.
    // So any print processor built from Windows2003+ DDK will not run on
    // earlier OSes because print processor will not find GetJobAttributesEx
    // exported by spooler. So we have to do LoadLibrary/GetProcAddress.
    //

    //
    // LoadLibGetJobAttributesEx function may be called multiple times during printing 
    // of a job. It is a waste of time to do LoadLibrary/GetProcAddress each time.
    // So we will do it only once and remember the results.
    //
    // If
    // a. Both pEMFAttr->hSpoolss and pEMFAttr->pfnGetJobAttributesEx are
    //    NULL. That would indicate this function is being called the first 
    //    time, so we should do LoadLibrary/GetProcAddress.
    // b. If both pEMFAttr->hSpoolss and pEMFAttr->pfnGetJobAttributesEx are non-null
    //    it indicates GetJobAttributesEx is present in spoolss.dll, 
    //    So we straightaway call pfnGetJobAttributesEx
    // c. If pEMFAttr->hSpoolss is non-null and pEMFAttr->pfnGetJobAttributesEx is null. 
    //    It indicates that GetJobAttributesEx is not present in spoolss.dll. So we 
    //    dont even attempt to call pfnGetJobAttributesEx. This is likely to happen if 
    //    this code is running on Windows2003 or earlier systems. See Note below.
    //
    // Note: If we do LoadLibrary spoolss.dll but we dont find GetJobAttributesEx in it, 
    // we dont immediately call FreeLibrary on it. spoolss.dll is always loaded in the spooler 
    // process, so we are not taking any extra hit by keeping it loaded. 
    // We release the handle to this dll, once the job finishes printing.
    //

    hSpoolss              = pEMFAttr->hSpoolss;
    pfnGetJobAttributesEx = pEMFAttr->pfnGetJobAttributesEx;

    if ( NULL == hSpoolss )
    {
        if ( NULL == pfnGetJobAttributesEx )
        {
            if ( NULL != (hSpoolss = HLoadLibraryFromSystemDirectory(L"spoolss.dll") ) )
            {

                pEMFAttr->hSpoolss = hSpoolss;

                if ( NULL != (pfnGetJobAttributesEx = 
                                    (PFNGetJobAttributesEx)GetProcAddress(hSpoolss, "GetJobAttributesEx") ))
                {
                    pEMFAttr->pfnGetJobAttributesEx = pfnGetJobAttributesEx;
                }
            }
        }
    }

    if ( NULL != pfnGetJobAttributesEx )
    {

        bRetVal = pfnGetJobAttributesEx((LPWSTR)pPrinterName,
                                         pDevmode,
                                         4,
                                         (LPBYTE)pAttributeInfo,
                                         sizeof(ATTRIBUTE_INFO_4),
                                         0);
    }
    return bRetVal;
}

