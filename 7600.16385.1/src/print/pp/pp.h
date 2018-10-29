//
//  Windows Server (Printing) Driver Development Kit Samples.
//
//  Sample Print Provider template.
//
//  Copyright (c) 1990 - 2005 Microsoft Corporation.
//  All Rights Reserved.
//
//
//  This source code is intended only as a supplement to Microsoft
//  Development Tools and/or on-line documentation.  See these other
//  materials for detailed information regarding Microsoft code samples.
// 
//  THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//  PARTICULAR PURPOSE.
//

#include <windows.h>
#include <winspool.h>
#include <strsafe.h>
#include <common.ver>

//
// NOTES on PRINTPROVIDOR struct:
//
// Get the list of API's that a Print Provider can provide as defined in the 
// _PRINTPROVIDOR structure definition.  This list will vary on a platform by 
// platform basis.
//
// Not all the API's in the list need to be supported. See "Functions Defined 
// by Print Providers" in the SDK documentation to get a list of whcih API's 
// from the list *must* be supported by a print provider on a given platform.
//
#include <winsplp.h>


//
//
// Function prototypes for the stubs for the various print provider API's
//
//


BOOL PP_OpenPrinter
(
    __in                                      LPWSTR             pszPrinterName,
    __out                                     LPHANDLE           phPrinter,
    __in_bcount_opt(sizeof(PRINTER_DEFAULTS)) LPPRINTER_DEFAULTS pDefault
);

BOOL PP_SetJob
(
    __in HANDLE hPrinter,
    __in DWORD  JobId,
    __in DWORD  Level,
    __in LPBYTE pJob,
    __in DWORD  Command
);

BOOL PP_GetJob
(
    __in                    HANDLE   hPrinter,
    __in                    DWORD    dwJobId,
    __in                    DWORD    dwLevel,
    __out_bcount_opt(cbBuf) LPBYTE   pbJob,
    __in                    DWORD    cbBuf,
    __out                   LPDWORD  pcbNeeded
);

BOOL PP_EnumJobs
(
    __in                    HANDLE  hPrinter,
    __in                    DWORD   dwFirstJob,
    __in                    DWORD   dwNoJobs,
    __in                    DWORD   dwLevel,
    __out_bcount_opt(cbBuf) LPBYTE  pbJob,
    __in                    DWORD   cbBuf,
    __out                   LPDWORD pcbNeeded,
    __out                   LPDWORD pcReturned
);

HANDLE PP_AddPrinter
(
    __in                                LPWSTR  pszName,
    __in                                DWORD   dwLevel,
    __in_bcount(sizeof(PRINTER_INFO_2)) LPBYTE  pbPrinter
);

BOOL PP_DeletePrinter
(
    __in HANDLE  hPrinter
);

BOOL PP_SetPrinter
(
    __in HANDLE  hPrinter,
    __in DWORD   Level,
    __in LPBYTE  pPrinter,
    __in DWORD   Command
);

BOOL PP_GetPrinter
(
    __in                            HANDLE  hPrinter,
    __in                            DWORD   dwLevel,
    __out_bcount_opt(cbBuf)         LPBYTE  pbPrinter,
    __in                            DWORD   cbBuf,
    __out                           LPDWORD pcbNeeded
);

BOOL PP_EnumPrinters
(
    __in                    DWORD   dwFlags,
    __in                    LPWSTR  pszName,
    __in                    DWORD   dwLevel,
    __out_bcount_opt(cbBuf) LPBYTE  pbPrinter,
    __in                    DWORD   cbBuf,
    __out                   LPDWORD pcbNeeded,
    __out_opt               LPDWORD pcReturned
);

BOOL PP_AddPrinterDriver
(
    __in LPWSTR  pName,
    __in DWORD   Level,
    __in LPBYTE  pDriverInfo
);

BOOL PP_EnumPrinterDrivers
(
    __in                    LPWSTR   pszName,
    __in                    LPWSTR   pszEnvironment,
    __in                    DWORD    dwLevel,
    __out_bcount_opt(cbBuf) LPBYTE   pbDriverInfo,
    __in                    DWORD    cbBuf,
    __out                   LPDWORD  pcbNeeded,
    __out                   LPDWORD  pcReturned
);

BOOL PP_GetPrinterDriver
(
    __in                    HANDLE   hPrinter,
    __in_opt                LPWSTR   pszEnvironment,
    __in                    DWORD    dwLevel,
    __out_bcount_opt(cbBuf) LPBYTE   pbDriverInfo,
    __in                    DWORD    cbBuf,
    __out                   LPDWORD  pcbNeeded
);

BOOL PP_GetPrinterDriverDirectory
(
    __in                    LPWSTR   pszName,
    __in_opt                LPWSTR   pszEnvironment,
    __in                    DWORD    dwLevel,
    __out_bcount_opt(cbBuf) LPBYTE   pbDriverDirectory,
    __in                    DWORD    cbBuf,
    __out                   LPDWORD  pcbNeeded
);

BOOL PP_DeletePrinterDriver
(
    __in LPWSTR  pszName,
    __in LPWSTR  pszEnvironment,
    __in LPWSTR  pszDriverName
);

BOOL PP_AddPrintProcessor
(
    __in LPWSTR  pszName,
    __in LPWSTR  pszEnvironment,
    __in LPWSTR  pszPathName,
    __in LPWSTR  pszPrintProcessorName
);

BOOL PP_EnumPrintProcessors
(
    __in                    LPWSTR   pszName,
    __in_opt                LPWSTR   pszEnvironment,
    __in                    DWORD    dwLevel,
    __out_bcount_opt(cbBuf) LPBYTE   pbPrintProcessorInfo,
    __in                    DWORD    cbBuf,
    __out                   LPDWORD  pcbNeeded,
    __out                   LPDWORD  pcReturned
);

BOOL PP_GetPrintProcessorDirectory
(
    __in                    LPWSTR   pszName,
    __in_opt                LPWSTR   pszEnvironment,
    __in                    DWORD    dwLevel,
    __out_bcount_opt(cbBuf) LPBYTE   pbPrintProcessorDirectory,
    __out                   DWORD    cbBuf,
    __out                   LPDWORD  pcbNeeded
);

BOOL PP_DeletePrintProcessor
(
    __in LPWSTR  pName,
    __in LPWSTR  pEnvironment,
    __in LPWSTR  pPrintProcessorName
);

BOOL PP_EnumPrintProcessorDatatypes
(
    __in                    LPWSTR   pszName,
    __in_opt                LPWSTR   pszPrintProcessorName,
    __in                    DWORD    dwLevel,
    __out_bcount_opt(cbBuf) LPBYTE   pbDatatypes,
    __in                    DWORD    cbBuf,
    __out                   LPDWORD  pcbNeeded,
    __out                   LPDWORD  pcReturned
);

DWORD PP_StartDocPrinter
(
    __in HANDLE  hPrinter,
    __in DWORD   Level,
    __in LPBYTE  pDocInfo
);

BOOL PP_StartPagePrinter
(
    __in HANDLE  hPrinter
);

BOOL PP_WritePrinter
(
    __in                HANDLE  hPrinter,
    __in_bcount(cbBuf)  LPVOID  pBuf,
    __in                DWORD   cbBuf,
    __out               LPDWORD pcbWritten
);

BOOL PP_EndPagePrinter
(
    __in HANDLE  hPrinter
);

BOOL PP_AbortPrinter
(
    __in HANDLE  hPrinter
);

BOOL PP_ReadPrinter
(
    __in  HANDLE   hPrinter,
    __out LPVOID   pBuf,
    __in  DWORD    cbBuf,
    __out LPDWORD  pcbRead
);

BOOL PP_EndDocPrinter
(
    __in HANDLE  hPrinter
);

BOOL PP_AddJob
(
    __in                    HANDLE  hPrinter,
    __in                    DWORD   dwLevel,
    __out_bcount_opt(cbBuf) LPBYTE  pbAddJob,
    __in                    DWORD   cbBuf,
    __out                   LPDWORD pcbNeeded
);

BOOL PP_ScheduleJob
(
    __in HANDLE  hPrinter,
    __in DWORD   JobId
);

DWORD PP_GetPrinterData
(
    __in                    HANDLE   hPrinter,
    __in                    LPWSTR   pszValueName,
    __out                   LPDWORD  pdwType,
    __out_bcount_opt(cbBuf) LPBYTE   pbData,
    __in                    DWORD    cbBuf,
    __out                   LPDWORD  pcbNeeded
);

DWORD PP_SetPrinterData
(
    __in                HANDLE  hPrinter,
    __in                LPWSTR  pszValueName,
    __in                DWORD   dwType,
    __in_bcount(cbData) LPBYTE  pbData,
    __in                DWORD   cbData
);

DWORD PP_WaitForPrinterChange
(
    __in HANDLE hPrinter,
    __in DWORD  Flags
);

BOOL PP_ClosePrinter
(
    __in HANDLE hPrinter
);

BOOL PP_AddForm
(
    __in HANDLE  hPrinter,
    __in DWORD   Level,
    __in LPBYTE  pForm
);

BOOL PP_DeleteForm
(
    __in HANDLE  hPrinter,
    __in LPWSTR  pFormName
);

BOOL PP_GetForm
(
    __in                    HANDLE   hPrinter,
    __in                    LPWSTR   pszFormName,
    __in                    DWORD    dwLevel,
    __out_bcount_opt(cbBuf) LPBYTE   pbForm,
    __in                    DWORD    cbBuf,
    __out                   LPDWORD  pcbNeeded
);

BOOL PP_SetForm
(
    __in HANDLE  hPrinter,
    __in LPWSTR  pFormName,
    __in DWORD   Level,
    __in LPBYTE  pForm
);

BOOL PP_EnumForms
(
    __in                    HANDLE   hPrinter,
    __in                    DWORD    dwLevel,
    __out_bcount_opt(cbBuf) LPBYTE   pbForm,
    __in                    DWORD    cbBuf,
    __out                   LPDWORD  pcbNeeded,
    __out                   LPDWORD  pcReturned
);

BOOL PP_EnumMonitors
(
    __in                    LPWSTR   pszName,
    __in                    DWORD    dwLevel,
    __out_bcount_opt(cbBuf) LPBYTE   pbMonitor,
    __in                    DWORD    cbBuf,
    __out                   LPDWORD  pcbNeeded,
    __out                   LPDWORD  pcReturned
);

BOOL PP_EnumPorts
(
    __in                    LPWSTR   pszName,
    __in                    DWORD    dwLevel,
    __out_bcount_opt(cbBuf) LPBYTE   pbPort,
    __in                    DWORD    cbBuf,
    __out                   LPDWORD  pcbNeeded,
    __out                   LPDWORD  pcReturned
);

BOOL PP_AddPort
(
    __in LPWSTR  pName,
    __in HWND    hWnd,
    __in LPWSTR  pMonitorName
);

BOOL PP_ConfigurePort
(
    __in LPWSTR  pName,
    __in HWND    hWnd,
    __in LPWSTR  pPortName
);

BOOL PP_DeletePort
(
    __in LPWSTR  pName,
    __in HWND    hWnd,
    __in LPWSTR  pPortName
);

HANDLE PP_CreatePrinterIC
(
    __in HANDLE     hPrinter,
    __in LPDEVMODEW pDevMode
);

BOOL PP_PlayGdiScriptOnPrinterIC
(
    __in                    HANDLE  hPrinterIC,
    __in_bcount(cbIn)       LPBYTE  pbIn,
    __in                    DWORD   cbIn,
    __out_bcount_opt(cbOut) LPBYTE  pbOut,
    __in                    DWORD   cbOut,
    __in                    DWORD   ul
);

BOOL PP_DeletePrinterIC
(
    __in HANDLE  hPrinterIC
);

BOOL PP_AddPrinterConnection
(
    __in LPWSTR  pName
);

BOOL PP_DeletePrinterConnection
(
    __in LPWSTR pName
);

DWORD PP_PrinterMessageBox
(
    __in HANDLE  hPrinter,
    __in DWORD   Error,
    __in HWND    hWnd,
    __in LPWSTR  pText,
    __in LPWSTR  pCaption,
    __in DWORD   dwType
);

BOOL PP_AddMonitor
(
    __in LPWSTR  pName,
    __in DWORD   Level,
    __in LPBYTE  pMonitorInfo
);

BOOL PP_DeleteMonitor
(
    __in LPWSTR  pName,
    __in LPWSTR  pEnvironment,
    __in LPWSTR  pMonitorName
);

BOOL PP_ResetPrinter
(
    __in                                  HANDLE             hPrinter,
    __in_bcount(sizeof(PRINTER_DEFAULTS)) LPPRINTER_DEFAULTS pDefault
);

BOOL PP_AddPortEx
(
    __in     LPWSTR pName,
    __in     DWORD  Level,
    __in     LPBYTE lpBuffer,
    __in_opt LPWSTR lpMonitorName
);

BOOL PP_OpenPrinterEx
(
    __in                                      LPWSTR             pPrinterName,
    __out                                     LPHANDLE           phPrinter,
    __in_bcount_opt(sizeof(PRINTER_DEFAULTS)) LPPRINTER_DEFAULTS pDefault,
    __in_opt                                  LPBYTE             pClientInfo,
    __in                                      DWORD              Level
);

HANDLE PP_AddPrinterEx
(
    __in LPWSTR  pName,
    __in DWORD   Level,
    __in LPBYTE  pPrinter,
    __in LPBYTE  pClientInfo,
    __in DWORD   ClientInfoLevel
);

BOOL PP_SetPort
(
    __in LPWSTR  pName,
    __in LPWSTR  pPortName,
    __in DWORD   Level,
    __in LPBYTE  pPortInfo
);

DWORD PP_EnumPrinterData
(
    __in                      HANDLE  hPrinter,
    __in                      DWORD   dwIndex,
    __out_bcount(cbValueName) LPWSTR  pValueName,
    __in                      DWORD   cbValueName,
    __out                     LPDWORD pcbValueName,
    __out                     LPDWORD pType,
    __out_bcount_opt(cbData)  LPBYTE  pData,
    __in                      DWORD   cbData,
    __out                     LPDWORD pcbData
);

DWORD PP_DeletePrinterData
(
    __in HANDLE   hPrinter,
    __in LPWSTR   pValueName
);




//
// Simple port structure to maintain an internal list of added ports.
//
typedef struct _PORT
{
    DWORD   cb;
    struct  _PORT *pNext;
    LPWSTR  pName;
} PORT, *PPORT;


//
// Function prototypes for various port utility functions.
//
DWORD
InitializePortNames
(
    VOID
);

PPORT CreatePortEntry
(
    __in LPWSTR pPortName
);

BOOL DeletePortEntry
(
    __in LPWSTR pPortName
);

DWORD
CreateRegistryEntry
(
    __in LPWSTR pPortName
);

DWORD DeleteRegistryEntry
(
    __in LPWSTR pPortName
);

BOOL PortKnown
(
    __in LPWSTR pPortName
);

BOOL PortExists
(
    __in  LPWSTR pPortName,
    __out LPDWORD pError
);

BOOL IsLocalMachine
(
    __in LPWSTR pszName
);



//
// Function prototypes for miscellaneous utility functions.
//
LPWSTR AllocSplStr
(
    __in LPWSTR pStr
);

VOID
FreeSplStr
(
   __in LPWSTR pStr
);

BOOL ValidateUNCName
(
    __in LPWSTR pName
);

DWORD
StatusFromHResult
(
    __in HRESULT hr
);

BOOL
BoolFromStatus
(
    __in DWORD Status
);



//
// Make global variables available to other files in the sample.
//
extern HMODULE hmod;
extern HMODULE hSpoolssDll;

extern CRITICAL_SECTION SplSem;

extern FARPROC pfnSpoolssEnumPorts;

extern WCHAR *pszRegistryPath;
extern WCHAR *pszRegistryPortNames;
extern WCHAR szMachineName[MAX_COMPUTERNAME_LENGTH + 3];

extern PPORT pFirstPort;


