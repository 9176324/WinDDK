//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//  ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//  PARTICULAR PURPOSE.
//
//  Copyright  1996-2003  Microsoft Corporation.  All Rights Reserved.
//
//  FILE:	Debug.cpp
//    
//
//  PURPOSE:  Debug functions.
//
//
//	Functions:
//
//
//
//  PLATFORMS:	 Windows 2000, Windows XP, Windows Server 2003
//
//

#include "precomp.h"
#include "oem.h"
#include "debug.h"
#include <strsafe.h>



////////////////////////////////////////////////////////
//      INTERNAL DEFINES
////////////////////////////////////////////////////////

#define DEBUG_BUFFER_SIZE       1024
#define PATH_SEPARATOR          '\\'



// Determine what level of debugging messages to eject. 
#ifdef VERBOSE_MSG
    #define DEBUG_LEVEL     DBG_VERBOSE
#elif TERSE_MSG
    #define DEBUG_LEVEL     DBG_TERSE
#elif WARNING_MSG
    #define DEBUG_LEVEL     DBG_WARNING
#elif ERROR_MSG
    #define DEBUG_LEVEL     DBG_ERROR
#elif RIP_MSG
    #define DEBUG_LEVEL     DBG_RIP
#elif NO_DBG_MSG
    #define DEBUG_LEVEL     DBG_NONE
#else
    #define DEBUG_LEVEL     DBG_WARNING
#endif



////////////////////////////////////////////////////////
//      EXTERNAL GLOBALS
////////////////////////////////////////////////////////

INT giDebugLevel = DEBUG_LEVEL;




////////////////////////////////////////////////////////
//      INTERNAL PROTOTYPES
////////////////////////////////////////////////////////

static BOOL DebugMessageV(LPCSTR lpszMessage, va_list arglist);
static BOOL DebugMessageV(DWORD dwSize, LPCWSTR lpszMessage, va_list arglist);




//////////////////////////////////////////////////////////////////////////
//  Function:	DebugMessageV
//
//  Description:  Outputs variable argument debug string.
//    
//
//  Parameters:	
//
//      dwSize          Size of temp buffer to hold formated string.
//
//      lpszMessage     Format string.
//
//      arglist         Variable argument list..
//    
//
//  Returns:
//    
//
//  Comments:
//     
//
//  History:
//		12/18/96	APresley Created.
//
//////////////////////////////////////////////////////////////////////////

static BOOL DebugMessageV(LPCSTR lpszMessage, va_list arglist)
{
    DWORD   dwSize = DEBUG_BUFFER_SIZE;
    LPSTR   lpszMsgBuf = NULL;
    HRESULT hr = S_FALSE;


    // Parameter checking.
    if( (NULL == lpszMessage)
        ||
        (0 == dwSize)
      )
    {
      return FALSE;
    }

    do
    {
        // Allocate memory for message buffer.
        if(NULL != lpszMsgBuf)
        {
            delete[] lpszMsgBuf;
            dwSize *= 2;
        }

        //
        // Just to make sure we dont go into infinite loop and allocate
        // lots of memory, lets bail out if memory requirement becomes huge.
        // Multiplying by 8, i.e. will loop at most 4 times.
        //
        if ( dwSize > 8*DEBUG_BUFFER_SIZE )
        {
            return FALSE;
        }

        lpszMsgBuf = new CHAR[dwSize + 1];
        if(NULL == lpszMsgBuf)
            return FALSE;

        hr = StringCchVPrintfA ( lpszMsgBuf, dwSize, lpszMessage, arglist);
    // Pass the variable parameters to wvsprintf to be formated.
    } while (FAILED (hr) );

    // Dump string to Debug output.
    OutputDebugStringA(lpszMsgBuf);

    // Cleanup.
    delete[] lpszMsgBuf;

    return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//  Function:	DebugMessageV
//
//  Description:  Outputs variable argument debug string.
//    
//
//  Parameters:	
//
//      dwSize          Size of temp buffer to hold formated string.
//
//      lpszMessage     Format string.
//
//      arglist         Variable argument list..
//    
//
//  Returns:
//    
//
//  Comments:
//     
//
//  History:
//		12/18/96	APresley Created.
//
//////////////////////////////////////////////////////////////////////////

static BOOL DebugMessageV(DWORD dwSize, LPCWSTR lpszMessage, va_list arglist)
{
    LPWSTR     lpszMsgBuf;
    HRESULT    hr = S_FALSE;


    // Parameter checking.
    if( (NULL == lpszMessage)
        ||
        (0 == dwSize)
      )
    {
      return FALSE;
    }

    // Allocate memory for message buffer.
    lpszMsgBuf = new WCHAR[dwSize + 1];    
    if(NULL == lpszMsgBuf)
        return FALSE;

    // Pass the variable parameters to wvsprintf to be formated.
    hr = StringCchVPrintfW (lpszMsgBuf, dwSize, lpszMessage, arglist);
    if ( SUCCEEDED (hr) )
    {
        // Dump string to debug output.
        OutputDebugStringW(lpszMsgBuf);
    }

    // Clean up.
    delete[] lpszMsgBuf;

    return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//  Function:	DebugMessage
//
//  Description:  Outputs variable argument debug string.
//    
//
//  Parameters:	
//
//      lpszMessage     Format string.
//
//
//  Returns:
//    
//
//  Comments:
//     
//
//  History:
//		12/18/96	APresley Created.
//
//////////////////////////////////////////////////////////////////////////

BOOL DebugMessage(LPCSTR lpszMessage, ...)
{
    BOOL    bResult;
    va_list VAList;


    // Pass the variable parameters to DebugMessageV for processing.
    va_start(VAList, lpszMessage);
    bResult = DebugMessageV(lpszMessage, VAList);
    va_end(VAList);

    return bResult;
}


//////////////////////////////////////////////////////////////////////////
//  Function:	DebugMessage
//
//  Description:  Outputs variable argument debug string.
//    
//
//  Parameters:	
//
//      lpszMessage     Format string.
//
//
//  Returns:
//    
//
//  Comments:
//     
//
//  History:
//		12/18/96	APresley Created.
//
//////////////////////////////////////////////////////////////////////////

BOOL DebugMessage(LPCWSTR lpszMessage, ...)
{
    BOOL    bResult;
    va_list VAList;


    // Pass the variable parameters to DebugMessageV to be processed.
    va_start(VAList, lpszMessage);
    bResult = DebugMessageV(MAX_PATH, lpszMessage, VAList);
    va_end(VAList);

    return bResult;
}

void Dump(PPUBLISHERINFO pPublisherInfo)
{
    VERBOSE(__TEXT("pPublisherInfo:\r\n"));
    VERBOSE(__TEXT("\tdwMode           =   %#x\r\n"), pPublisherInfo->dwMode);
    VERBOSE(__TEXT("\twMinoutlinePPEM  =   %d\r\n"), pPublisherInfo->wMinoutlinePPEM);
    VERBOSE(__TEXT("\twMaxbitmapPPEM   =   %d\r\n"), pPublisherInfo->wMaxbitmapPPEM);
}

void Dump(POEMDMPARAM pOemDMParam)
{
    VERBOSE(__TEXT("pOemDMParam:\r\n"));
    VERBOSE(__TEXT("\tcbSize = %d\r\n"), pOemDMParam->cbSize);
    VERBOSE(__TEXT("\tpdriverobj = %#x\r\n"), pOemDMParam->pdriverobj);
    VERBOSE(__TEXT("\thPrinter = %#x\r\n"), pOemDMParam->hPrinter);
    VERBOSE(__TEXT("\thModule = %#x\r\n"), pOemDMParam->hModule);
    VERBOSE(__TEXT("\tpPublicDMIn = %#x\r\n"), pOemDMParam->pPublicDMIn);
    VERBOSE(__TEXT("\tpPublicDMOut = %#x\r\n"), pOemDMParam->pPublicDMOut);
    VERBOSE(__TEXT("\tpOEMDMIn = %#x\r\n"), pOemDMParam->pOEMDMIn);
    VERBOSE(__TEXT("\tpOEMDMOut = %#x\r\n"), pOemDMParam->pOEMDMOut);
    VERBOSE(__TEXT("\tcbBufSize = %d\r\n"), pOemDMParam->cbBufSize);
}

void Dump(PPROPSHEETUI_INFO pPSUIInfo)
{
    VERBOSE(__TEXT("pPSUIInfo:\r\n"));
    VERBOSE(__TEXT("\tcbSize          = %d\r\n"), pPSUIInfo->cbSize);
    VERBOSE(__TEXT("\tVersion         = %#x\r\n"), pPSUIInfo->Version);
    VERBOSE(__TEXT("\tFlags           = %#x\r\n"), pPSUIInfo->Flags);
    VERBOSE(__TEXT("\tReason          = %d\r\n"), pPSUIInfo->Reason);
    VERBOSE(__TEXT("\thComPropSheet   = %#x\r\n"), pPSUIInfo->hComPropSheet);
    VERBOSE(__TEXT("\tpfnComPropSheet = %#x\r\n"), pPSUIInfo->pfnComPropSheet);
    VERBOSE(__TEXT("\tlParamInit      = %#x\r\n"), pPSUIInfo->lParamInit);
    VERBOSE(__TEXT("\tUserData        = %#x\r\n"), pPSUIInfo->UserData);
    VERBOSE(__TEXT("\tResult          = %#x\r\n"), pPSUIInfo->Result);
}



PCSTR
StripDirPrefixA(
    IN PCSTR    pstrFilename
    )

/*++

Routine Description:

    Strip the directory prefix off a filename (ANSI version)

Arguments:

    pstrFilename - Pointer to filename string

Return Value:

    Pointer to the last component of a filename (without directory prefix)

--*/

{
    PCSTR   pstr;

    if (pstr = strrchr(pstrFilename, PATH_SEPARATOR))
        return pstr + 1;

    return pstrFilename;
}


