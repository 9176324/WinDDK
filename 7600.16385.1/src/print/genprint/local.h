/*++

Copyright (c) 1998-2003  Microsoft Corporation
All rights reserved

Module Name:

    local.h

--*/

#ifndef _LOCAL_H_
#define _LOCAL_H_

typedef long NTSTATUS;

#include <windows.h>
#include <winspool.h>
#include <winsplp.h>
#include <wchar.h>

#include "winprint.h"
#include "emf.h"


#include <winddiui.h>

#define CCHOF(x) (sizeof(x)/sizeof(*(x)))

typedef struct _pfnWinSpoolDrv {
    BOOL    (*pfnOpenPrinter)(LPTSTR, LPHANDLE, LPPRINTER_DEFAULTS);
    BOOL    (*pfnClosePrinter)(HANDLE);
    BOOL    (*pfnDevQueryPrint)(HANDLE, LPDEVMODE, DWORD *, LPWSTR, DWORD);
    BOOL    (*pfnPrinterEvent)(LPWSTR, INT, DWORD, LPARAM, DWORD *);
    LONG    (*pfnDocumentProperties)(HWND, HANDLE, LPWSTR, PDEVMODE, PDEVMODE, DWORD);
    HANDLE  (*pfnLoadPrinterDriver)(HANDLE);
    BOOL    (*pfnSetDefaultPrinter)(LPCWSTR);
    BOOL    (*pfnGetDefaultPrinter)(LPWSTR, LPDWORD);
    HANDLE  (*pfnRefCntLoadDriver)(LPWSTR, DWORD, DWORD, BOOL);
    BOOL    (*pfnRefCntUnloadDriver)(HANDLE, BOOL);
    BOOL    (*pfnForceUnloadDriver)(LPWSTR);
}   fnWinSpoolDrv, *pfnWinSpoolDrv;


BOOL
SplInitializeWinSpoolDrv(
    pfnWinSpoolDrv   pfnList
    );

BOOL
GetJobAttributes(
    __in                                   LPWSTR            pPrinterName,
    __in                                   LPDEVMODEW        pDevmode,
    __out_bcount(sizeof(ATTRIBUTE_INFO_3)) PATTRIBUTE_INFO_3 pAttributeInfo
    );

BOOL
LoadLibGetJobAttributesEx(
    __inout  PEMF_ATTRIBUTE_INFO  pEMFAttr,
    __in     LPCWSTR              pPrinterName,
    __in     LPDEVMODE            pDevmode,
    __out    PATTRIBUTE_INFO_4    pAttributeInfo
    );

HINSTANCE
HLoadLibraryFromSystemDirectory (
    __in        LPCTSTR        szDllName);





#define LOG_ERROR   EVENTLOG_ERROR_TYPE

LPWSTR AllocSplStr(LPCWSTR pStr);
LPVOID AllocSplMem(SIZE_T cbAlloc);

#define FreeSplMem( pMem )        (GlobalFree( pMem ) ? FALSE:TRUE)
#define FreeSplStr( lpStr )       ((lpStr) ? (GlobalFree(lpStr) ? FALSE:TRUE):TRUE)


//
//  DEBUGGING:
//

#if DBG


BOOL
DebugPrint(
    __in PCH pszFmt,
    ...
    );

//
// ODS - OutputDebugString
//
#define ODS( MsgAndArgs ) DebugPrint  MsgAndArgs

#else
//
// No debugging
//
#define ODS(x)
#endif             // DBG


#endif

