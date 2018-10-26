//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//  ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//  PARTICULAR PURPOSE.
//
//  Copyright  1998 - 2003  Microsoft Corporation.  All Rights Reserved.
//
//  FILE:	command.cpp
//    
//
//  PURPOSE:  Source module for OEM customized Command(s).
//
//
//	Functions:
//
//		
//
//
//  PLATFORMS:	Windows 2000, Windows XP, Windows Server 2003
//
//

#include "precomp.h"
#include <PRCOMOEM.H>
#include "wmarkps.h"
#include "debug.h"
#include "command.h"
#include "resource.h"

// StrSafe.h needs to be included last
// to disallow bad string functions.
#include <STRSAFE.H>




/////////////////////////////////////////////////////////
//		Internal Macros & Defines
/////////////////////////////////////////////////////////

// Macros to convert from Windows RGB to PostScript RGB
#define GetPS2Color(dw)     ((dw) / 255.0)
#define GetPS2RValue(cr)    (GetPS2Color(GetRValue(cr)))
#define GetPS2GValue(cr)    (GetPS2Color(GetGValue(cr)))
#define GetPS2BValue(cr)    (GetPS2Color(GetBValue(cr)))


// Initial buffer size
#define INITIAL_BUFFER_SIZE     16


// String format defines characters
#define FORMAT_DELIM            '!'
#define FORMAT_STRING_ANSI      's'
#define FORMAT_STRING_UNICODE   'S'
#define FORMAT_CHAR             '%'


// Loop limiter.
#define MAX_LOOP    10


/////////////////////////////////////////////////////////
//		Internal ProtoTypes
/////////////////////////////////////////////////////////

static PSTR GetPostScriptResource(HMODULE hModule, LPCTSTR pszResource, PDWORD pdwSize);
static PSTR CreateWaterMarkProlog(HMODULE hModule, PDWORD pdwSize, LPWSTR pszWaterMark, 
                                  DWORD dwFontSize, LPSTR pszColor, LPSTR pszAngle);
static PSTR DoWaterMarkProlog(HMODULE hModule, POEMDEV pOemDevmode, PDWORD pdwSize);
static DWORD FormatResource(LPSTR pszResource, LPSTR *ppszProlog, ...);
static DWORD CharSize(DWORD dwValue, DWORD dwBase = 10);



////////////////////////////////////////////////////////////////////////////////////
//    The PSCRIPT driver calls this OEM function at specific points during output
//    generation. This gives the OEM DLL an opportunity to insert code fragments
//    at specific injection points in the driver's code. It should use
//    DrvWriteSpoolBuf for generating any output it requires.

HRESULT PSCommand(PDEVOBJ pdevobj, DWORD dwIndex, PVOID pData, DWORD cbSize, 
                  IPrintOemDriverPS* pOEMHelp, PDWORD pdwReturn)
{
    BOOL    bFreeProcedure      = FALSE;
    PSTR    pProcedure          = NULL;
    DWORD   dwLen               = 0;
    DWORD   dwSize              = 0;
    HRESULT hResult             = E_FAIL;


    VERBOSE(DLLTEXT("Entering OEMCommand...\r\n"));

    switch (dwIndex)
    {
        case PSINJECT_BEGINPROLOG:
            {
                POEMDEV pOemDevmode = (POEMDEV) pdevobj->pOEMDM;


                VERBOSE(DLLTEXT("OEMCommand PSINJECT_BEGINPROLOG\r\n"));

                // Only do Water Mark PS prolog injection if Water Mark is enabled.
                if(pOemDevmode->bEnabled)
                {
                    pProcedure = DoWaterMarkProlog((HMODULE) pdevobj->hOEM, pOemDevmode, &dwSize);
                    bFreeProcedure = (NULL != pProcedure);
                }
            }
            break;

        case PSINJECT_BEGINPAGESETUP:
            {
                POEMDEV pOemDevmode = (POEMDEV) pdevobj->pOEMDM;


                VERBOSE(DLLTEXT("OEMCommand PSINJECT_BEGINPAGESETUP\r\n"));

                // Only do Water Mark PS page injection if Water Mark is enabled.
                if(pOemDevmode->bEnabled)
                {
                    pProcedure = GetPostScriptResource((HMODULE) pdevobj->hOEM, MAKEINTRESOURCE(IDR_WATERMARK_DRAW), &dwSize);
                }
            }
            break;

        default:
            VERBOSE(DLLTEXT("PSCommand Default, command index %d...\r\n"), dwIndex);
            *pdwReturn = ERROR_NOT_SUPPORTED;
            return E_NOTIMPL;
    }

    if(NULL != pProcedure)
    {
        // Write PostScript to spool file.
        dwLen = strlen(pProcedure);
        hResult = pOEMHelp->DrvWriteSpoolBuf(pdevobj, pProcedure, dwLen, &dwSize);

        // Dump DrvWriteSpoolBuf parameters.
        VERBOSE(DLLTEXT("dwLen  = %d\r\n"), dwLen);
        VERBOSE(DLLTEXT("dwSize = %d\r\n"), dwSize);
        //VERBOSE(DLLTEXT("pProcedure is:\r\n\t%hs\r\n"), pProcedure);

        // Set return values.
        if(SUCCEEDED(hResult) && (dwLen == dwSize))
        {
            *pdwReturn = ERROR_SUCCESS;
        }
        else
        {
            // Try to return meaningful
            // error value.
            *pdwReturn = GetLastError();
            if(ERROR_SUCCESS == *pdwReturn)
            {
                *pdwReturn = ERROR_WRITE_FAULT;
            }

            // Make sure we return failure
            // if the write didn't succeded.
            if(SUCCEEDED(hResult))
            {
                hResult = HRESULT_FROM_WIN32(*pdwReturn);
            }
        }

        if(bFreeProcedure)
        {
            // INVARIANT: pProcedure was created with 'new' and needs to be freed.
            delete[] pProcedure;
        }
    }
    else
    {
        // pProcedure will be NULL if water mark isn't enabled.
        *pdwReturn = ERROR_NOT_SUPPORTED;
        hResult = E_NOTIMPL;
    }

    // dwLen should always equal dwSize.
    ASSERTMSG(dwLen == dwSize, DLLTEXT("number of bytes wrote should equal number of bytes written!"));

    return hResult;
}


////////////////////////////////////////////////////////////////////////////////////
//
//  Retrieves pointer to a PostScript resource.
//
static PSTR GetPostScriptResource(HMODULE hModule, LPCTSTR pszResource, PDWORD pdwSize)
{
    PSTR    pszPostScript   = NULL;
    HRSRC   hFind           = NULL;
    HGLOBAL hResource       = NULL;


    VERBOSE(DLLTEXT("GetPostScriptResource() entered.\r\n"));

    // pszResource and pdwSize Parameters should not be NULL.
    assert(NULL != pszResource);
    assert(NULL != pdwSize);

    // Load PostScript resource.
    hFind = FindResource(hModule, pszResource, MAKEINTRESOURCE(RC_PSCRIPT));
    //hFind = FindResource(hModule, pszResource, TEXT("PSCRIPT"));    
    if(NULL != hFind)
    {
        hResource = LoadResource(hModule, hFind);
        if(NULL != hResource)
        {
            pszPostScript = (PSTR) LockResource(hResource);
            *pdwSize = SizeofResource(hModule, hFind);
        }
        else
        {
            ERR(DLLTEXT("ERROR:  Failed to load PSCRIPT resource %#x!\r\n"), hFind);
        }
    }
    else
    {
        ERR(DLLTEXT("ERROR:  Failed to find PSCRIPT resource %#x!\r\n"), pszResource);
    }

    // Should have found the PScript resource.
    assert(NULL != pszPostScript);

    VERBOSE(DLLTEXT("GetPostScriptResource() returned %#x.\r\n"), pszPostScript);

    return pszPostScript;
}


////////////////////////////////////////////////////////////////////////////////////
//
//  Formats Water Mark prolog with parameter.
//
static PSTR CreateWaterMarkProlog(HMODULE hModule, PDWORD pdwSize, LPWSTR pszWaterMark, 
                                  DWORD dwFontSize, LPSTR pszColor, LPSTR pszAngle)
{
    PSTR    pszProlog   = NULL;
    PSTR    pszResource = NULL;


    // Parameters that are pointers should not be NULL!
    assert(NULL != pdwSize);
    assert(NULL != pszWaterMark);
    assert(NULL != pszColor);
    assert(NULL != pszAngle);

    // Dump parameters.
    VERBOSE(DLLTEXT("CreateWaterMarkProlog() paramters:\r\n"));
    VERBOSE(_TEXT("\tpszWaterMark = \"%ls\"\r\n"), pszWaterMark);
    VERBOSE(_TEXT("\tdwFontSize   = %d\r\n"), dwFontSize);
    VERBOSE(_TEXT("\tpszColor     = \"%hs\"\r\n"), pszColor);
    VERBOSE(_TEXT("\tpszAngle     = \"%hs\"\r\n"), pszAngle);

    // Get Water Mark prolog resource.
    pszResource = GetPostScriptResource(hModule, MAKEINTRESOURCE(IDR_WATERMARK_PROLOGUE), pdwSize);
    assert(NULL != pszResource);

    VERBOSE(DLLTEXT("CreateWaterMarkProlog(): pszResource is %hs\r\n"), pszResource != NULL ? pszResource : "<NULL>" );

    // Allocate and format the Water Mark Prolog with the correct values.
    if(NULL != pszResource)
    {
        *pdwSize = FormatResource(pszResource, &pszProlog, pszWaterMark, dwFontSize, pszColor, pszAngle);
    }

    // Returned values should not be NULL.
    assert(0 != *pdwSize);
    assert(NULL != pszProlog);

    VERBOSE(_TEXT("\t*pdwSize     = %d\r\n"), *pdwSize);

    return pszProlog;
}


////////////////////////////////////////////////////////////////////////////////////
//
//  Does the pre-formating of parameters before calling the routine 
//  that creates the prolog.
//
static PSTR DoWaterMarkProlog(HMODULE hModule, POEMDEV pOemDevmode, PDWORD pdwSize)
{
    PSTR    pszProlog = NULL;


    // Parameters should not be NULL.
    assert(NULL != hModule);
    assert(NULL != pOemDevmode);
    assert(NULL != pdwSize);

    // Only do prolog if Water Mark is enabled.
    if(pOemDevmode->bEnabled)
    {
        CHAR    szColor[INITIAL_BUFFER_SIZE] = "\0";
        DWORD   dwAngleSize = INITIAL_BUFFER_SIZE;
        LPSTR   pszAngle = NULL;

        // Format angle as a string.
        do
        {
            if(NULL != pszAngle)
            {
                delete[] pszAngle;
                dwAngleSize *= 2;
            }
            pszAngle = new CHAR[dwAngleSize];

        } while( (NULL != pszAngle) 
                 &&
                 (dwAngleSize < 1024)
                 && 
                 (FAILED(StringCbPrintfA(pszAngle, dwAngleSize, "%.1f", pOemDevmode->dfRotate)) ) 
               );

        // pszAngle should only be NULL if we run out of memory.
        assert(NULL != pszAngle);

        VERBOSE(DLLTEXT("DoWaterMarkProlog(): pszAngle is %hs\r\n"), pszAngle != NULL ? pszAngle : "<NULL>" );

        if(NULL != pszAngle)
        {
            // Format text color as string.
            if(FAILED(StringCbPrintfA(szColor, sizeof(szColor), 
                                      "%1.2f %1.2f %1.2f", 
                                      GetPS2RValue(pOemDevmode->crTextColor),
                                      GetPS2GValue(pOemDevmode->crTextColor), 
                                      GetPS2BValue(pOemDevmode->crTextColor))))
            {
                ERR(ERRORTEXT("DoWaterMarkProlog() failed to create PostScript color string for water mark."));
            }

            VERBOSE(DLLTEXT("DoWaterMarkProlog(): szColor is %hs\r\n"), szColor);

            // Create Water Mark prolog.
            pszProlog = CreateWaterMarkProlog(hModule, pdwSize, pOemDevmode->szWaterMark, 
                                              pOemDevmode->dwFontSize, szColor, pszAngle);

            VERBOSE(DLLTEXT("DoWaterMarkProlog(): pszProlog is %hs\r\n"), pszProlog != NULL ? pszProlog : "<NULL>" );

            // Angle string is no longer needed.
            delete[] pszAngle;
        }
    }

    return pszProlog;
}


////////////////////////////////////////////////////////////////////////////////////
//
//  Formats Resource.
//
static DWORD FormatResource(LPSTR pszResource, LPSTR *ppszBuffer, ...)
{
    DWORD   dwSize  = strlen(pszResource) + MAX_PATH;
    DWORD   dwLoop  = 0;
    va_list vaList;
    HRESULT hResult = E_FAIL;


    va_start(vaList, ppszBuffer);

    // *ppszBuffer should be NULL when passed in.
    *ppszBuffer = NULL;

    // Allocate and format the string.
    do {

        if(NULL != *ppszBuffer)
        {
            delete[] *ppszBuffer;
        }
        *ppszBuffer = new CHAR[dwSize];
        if(NULL == *ppszBuffer)
        {
            goto Cleanup;
        }

        hResult = StringCbVPrintfA(*ppszBuffer, dwSize, pszResource, vaList);

        if(STRSAFE_E_INSUFFICIENT_BUFFER == hResult)
        {
            dwSize *= 2;
        }

    } while ( FAILED(hResult) && (dwLoop++ < MAX_LOOP));

Cleanup:

    // Check to see if we hit error.
    if(FAILED(hResult))
    {
        if(NULL != *ppszBuffer)
        {
            delete[] *ppszBuffer;
            *ppszBuffer = NULL;
        }
    }

    va_end(vaList);

    return lstrlenA(*ppszBuffer);
}


DWORD CharSize(DWORD dwValue, DWORD dwBase)
{
    DWORD dwSize = 1;


    // Make sure taht base is more than 2.
    if(dwBase < 2)
    {
        return dwSize;
    }

    // Loop until dwValue is less than dwBase, 
    // dividing by dwBase each time.
    while(dwValue >= dwBase)
    {
        dwValue /= dwBase;
        ++dwSize;
    }

    return dwSize;
}



