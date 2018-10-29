//
//  Windows (Printing) Driver Development Kit Samples.
//
//  Sample Print Provider template.
//
//  pp.c - Print provider initialization functions and stubs for the required
//         supported API set.
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



#include "pp.h"

HMODULE hmod = NULL;
WCHAR *pszRegistryPath = NULL;
WCHAR *pszRegistryPortNames=L"PortNames";
WCHAR szMachineName[MAX_COMPUTERNAME_LENGTH + 3];
CRITICAL_SECTION SplSem;
HMODULE hSpoolssDll = NULL;
FARPROC pfnSpoolssEnumPorts = NULL;

//
// Define the supported API set.
//
// Should have corresponding entries in the _PRINTPROVIDOR structure definition
// in winsplp.h.
//
// See "NOTES on PRINTPROVIDOR struct" in pp.h for details on which API's must
// be supported by a print processor.
//
static
PRINTPROVIDOR PrintProvidor =
{
    PP_OpenPrinter,
    PP_SetJob,
    PP_GetJob,
    PP_EnumJobs,
    PP_AddPrinter,
    PP_DeletePrinter,
    PP_SetPrinter,
    PP_GetPrinter,
    PP_EnumPrinters,
    PP_AddPrinterDriver,
    PP_EnumPrinterDrivers,
    PP_GetPrinterDriver,
    PP_GetPrinterDriverDirectory,
    PP_DeletePrinterDriver,
    PP_AddPrintProcessor,
    PP_EnumPrintProcessors,
    PP_GetPrintProcessorDirectory,
    PP_DeletePrintProcessor,
    PP_EnumPrintProcessorDatatypes,
    PP_StartDocPrinter,
    PP_StartPagePrinter,
    PP_WritePrinter,
    PP_EndPagePrinter,
    PP_AbortPrinter,
    PP_ReadPrinter,
    PP_EndDocPrinter,
    PP_AddJob,
    PP_ScheduleJob,
    PP_GetPrinterData,
    PP_SetPrinterData,
    PP_WaitForPrinterChange,
    PP_ClosePrinter,
    PP_AddForm,
    PP_DeleteForm,
    PP_GetForm,
    PP_SetForm,
    PP_EnumForms,
    PP_EnumMonitors,
    PP_EnumPorts,
    PP_AddPort,
    PP_ConfigurePort,
    PP_DeletePort,
    PP_CreatePrinterIC,
    PP_PlayGdiScriptOnPrinterIC,
    PP_DeletePrinterIC,
    PP_AddPrinterConnection,
    PP_DeletePrinterConnection,
    PP_PrinterMessageBox,
    PP_AddMonitor,
    PP_DeleteMonitor
};





//
// Required for DLL initialization.
//
BOOL DllMain
(
    __in HINSTANCE hdll,
    __in DWORD     dwReason,
    __in LPVOID    lpReserved
)
{
    UNREFERENCED_PARAMETER(lpReserved);

    if (dwReason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hdll);

        hmod = hdll;
    }

    else if (dwReason == DLL_PROCESS_DETACH)
    {

    }
    return TRUE;
}



//
// This function *must* be exported by a print provider.
//
BOOL
InitializePrintProvidor
(
    __out_bcount(cbPrintProvidor)  LPPRINTPROVIDOR pPrintProvidor,
    __in                           DWORD           cbPrintProvidor,
    __in                           LPWSTR          pszFullRegistryPath
)

{
    DWORD dwLen;

    if (!pPrintProvidor || !pszFullRegistryPath || !*pszFullRegistryPath)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    memcpy(pPrintProvidor, &PrintProvidor, min(sizeof(PRINTPROVIDOR), cbPrintProvidor));




    pszRegistryPath = AllocSplStr(pszFullRegistryPath);
    if (!pszRegistryPath)
    {
        return FALSE;
    }

    InitializeCriticalSection(&SplSem);
    szMachineName[0] = szMachineName[1] = L'\\';
    dwLen = MAX_COMPUTERNAME_LENGTH;
    GetComputerName(szMachineName + 2, &dwLen);


    InitializePortNames();

    return TRUE;
}



//
//
// Stub functions for all the API's supported by this sample print providor.
//
//


BOOL
PP_OpenPrinter
(
    __in                                      LPWSTR             pszPrinterName,
    __out                                     LPHANDLE           phPrinter,
    __in_bcount_opt(sizeof(PRINTER_DEFAULTS)) LPPRINTER_DEFAULTS pDefault
)

{
    DWORD err = NO_ERROR;

    UNREFERENCED_PARAMETER(phPrinter);
    UNREFERENCED_PARAMETER(pDefault);

    if (!pszPrinterName)
    {
        SetLastError(ERROR_INVALID_NAME);
        return FALSE;
    }





    if (err)
    {
        SetLastError(err);
    }
    return (err == NO_ERROR);
}


BOOL
PP_ClosePrinter
(
    __in HANDLE  hPrinter
)

{
    DWORD err = NO_ERROR;

    UNREFERENCED_PARAMETER(hPrinter);





    if (err)
    {
        SetLastError(err);
    }
    return (err == NO_ERROR);
}


BOOL
PP_GetPrinter
(
    __in                    HANDLE  hPrinter,
    __in                    DWORD   dwLevel,
    __out_bcount_opt(cbBuf) LPBYTE  pbPrinter,
    __in                    DWORD   cbBuf,
    __out                   LPDWORD pcbNeeded
)

{
    DWORD err = NO_ERROR;

    UNREFERENCED_PARAMETER(pcbNeeded);
    UNREFERENCED_PARAMETER(cbBuf);
    UNREFERENCED_PARAMETER(pbPrinter);
    UNREFERENCED_PARAMETER(dwLevel);
    UNREFERENCED_PARAMETER(hPrinter);





    if (err)
    {
        SetLastError(err);
    }

    return (err == NO_ERROR);
}


BOOL
PP_SetPrinter
(
    __in HANDLE  hPrinter,
    __in DWORD   dwLevel,
    __in LPBYTE  pbPrinter,
    __in DWORD   dwCommand
)

{
    DWORD err = NO_ERROR;

    UNREFERENCED_PARAMETER(pbPrinter);
    UNREFERENCED_PARAMETER(dwCommand);
    UNREFERENCED_PARAMETER(hPrinter);

    switch (dwLevel)
    {
        case 0:
        case 1:
        case 2:
        case 3:
            break;

        default:
            SetLastError(ERROR_INVALID_LEVEL);
            return FALSE;
    }





    if (err)
    {
        SetLastError(err);
    }

    return (err == NO_ERROR);
}


BOOL
PP_EnumPrinters
(
    __in                    DWORD   dwFlags,
    __in                    LPWSTR  pszName,
    __in                    DWORD   dwLevel,
    __out_bcount_opt(cbBuf) LPBYTE  pbPrinter,
    __in                    DWORD   cbBuf,
    __out                   LPDWORD pcbNeeded,
    __out_opt               LPDWORD pcReturned
)

{
    DWORD  err = NO_ERROR;

    UNREFERENCED_PARAMETER(dwFlags);
    UNREFERENCED_PARAMETER(pszName);
    UNREFERENCED_PARAMETER(pbPrinter);
    UNREFERENCED_PARAMETER(cbBuf);

    if ((dwLevel != 1) && (dwLevel != 2))
    {
        SetLastError(ERROR_INVALID_NAME);
        return FALSE;
    }
    else if (!pcbNeeded || !pcReturned)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }





    if (err)
    {
        SetLastError(err);
    }

    return (err == NO_ERROR);
}


DWORD
PP_StartDocPrinter
(
    __in HANDLE  hPrinter,
    __in DWORD   dwLevel,
    __in LPBYTE  lpbDocInfo
)

{
    DWORD err = NO_ERROR;

    UNREFERENCED_PARAMETER(hPrinter);
    UNREFERENCED_PARAMETER(dwLevel);
    UNREFERENCED_PARAMETER(lpbDocInfo);

    if (dwLevel != 1)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }





    if (err)
    {
        SetLastError(err);
    }

    return (err == NO_ERROR);
}


BOOL
PP_WritePrinter
(
    __in                HANDLE  hPrinter,
    __in_bcount(cbBuf)  LPVOID  pBuf,
    __in                DWORD   cbBuf,
    __out               LPDWORD pcbWritten
)

{
    DWORD err = NO_ERROR;

    UNREFERENCED_PARAMETER(hPrinter);
    UNREFERENCED_PARAMETER(pBuf);
    UNREFERENCED_PARAMETER(cbBuf);
    UNREFERENCED_PARAMETER(pcbWritten);




    if (err)
    {
        SetLastError(err);
    }

    return (err == NO_ERROR);
}


BOOL
PP_AbortPrinter
(
    __in HANDLE  hPrinter
)

{
    DWORD err = NO_ERROR;

    UNREFERENCED_PARAMETER(hPrinter);




    if (err)
    {
        SetLastError(err);
    }

    return (err == NO_ERROR);

}


BOOL
PP_EndDocPrinter
(
    __in HANDLE   hPrinter
)

{
    DWORD err = NO_ERROR;

    UNREFERENCED_PARAMETER(hPrinter);




    if (err)
    {
        SetLastError(err);
    }

    return (err == NO_ERROR);
}


BOOL
PP_GetJob
(
    __in                    HANDLE   hPrinter,
    __in                    DWORD    dwJobId,
    __in                    DWORD    dwLevel,
    __out_bcount_opt(cbBuf) LPBYTE   pbJob,
    __in                    DWORD    cbBuf,
    __out                   LPDWORD  pcbNeeded
)

{
    DWORD err = NO_ERROR;

    UNREFERENCED_PARAMETER(hPrinter);
    UNREFERENCED_PARAMETER(dwJobId);
    UNREFERENCED_PARAMETER(pbJob);
    UNREFERENCED_PARAMETER(cbBuf);
    UNREFERENCED_PARAMETER(pcbNeeded);

    if ((dwLevel != 1) && (dwLevel != 2))
    {
        SetLastError(ERROR_INVALID_LEVEL);
        return FALSE;
    }





    if (err)
    {
        SetLastError(err);
    }

    return (err == NO_ERROR);
}


BOOL
PP_EnumJobs
(
    __in                    HANDLE  hPrinter,
    __in                    DWORD   dwFirstJob,
    __in                    DWORD   dwNoJobs,
    __in                    DWORD   dwLevel,
    __out_bcount_opt(cbBuf) LPBYTE  pbJob,
    __in                    DWORD   cbBuf,
    __out                   LPDWORD pcbNeeded,
    __out                   LPDWORD pcReturned
)

{
    DWORD err = NO_ERROR;

    UNREFERENCED_PARAMETER(hPrinter);
    UNREFERENCED_PARAMETER(dwFirstJob);
    UNREFERENCED_PARAMETER(dwNoJobs);
    UNREFERENCED_PARAMETER(pbJob);
    UNREFERENCED_PARAMETER(cbBuf);
    UNREFERENCED_PARAMETER(pcbNeeded);
    UNREFERENCED_PARAMETER(pcReturned);

    if ((dwLevel != 1) && (dwLevel != 2))
    {
        SetLastError(ERROR_INVALID_LEVEL);
        return FALSE;
    }





    if (err)
    {
        SetLastError(err);
    }
    return (err == NO_ERROR);
}


BOOL
PP_SetJob
(
    __in HANDLE  hPrinter,
    __in DWORD   dwJobId,
    __in DWORD   dwLevel,
    __in LPBYTE  pbJob,
    __in DWORD   dwCommand
)

{
    DWORD err = NO_ERROR;

    UNREFERENCED_PARAMETER(hPrinter);
    UNREFERENCED_PARAMETER(dwJobId);
    UNREFERENCED_PARAMETER(dwCommand);

    if ((dwLevel != 0) && (dwLevel != 1) && (dwLevel != 2))
    {
        SetLastError(ERROR_INVALID_LEVEL);
        return FALSE;
    }
    else if ((dwLevel == 0) && (pbJob != NULL))
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }





    if (err)
    {
        SetLastError(err);
    }
    return (err == NO_ERROR);
}


BOOL
PP_AddJob
(
    __in                    HANDLE  hPrinter,
    __in                    DWORD   dwLevel,
    __out_bcount_opt(cbBuf) LPBYTE  pbAddJob,
    __in                    DWORD   cbBuf,
    __out                   LPDWORD pcbNeeded
)

{
    DWORD err = NO_ERROR;

    ADDJOB_INFO_1W TempBuffer;
    PADDJOB_INFO_1W OutputBuffer;
    DWORD OutputBufferSize;

    UNREFERENCED_PARAMETER(hPrinter);
    UNREFERENCED_PARAMETER(pcbNeeded);

    if (dwLevel != 1)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }








    if (cbBuf < sizeof(ADDJOB_INFO_1W))
    {
        OutputBuffer = &TempBuffer;
        OutputBufferSize = sizeof(ADDJOB_INFO_1W);
    }
    else
    {
        OutputBuffer = (LPADDJOB_INFO_1W) pbAddJob;
        OutputBufferSize = cbBuf;
    }





    if (err)
    {
        SetLastError(err);
    }

    return (err == NO_ERROR);
}


BOOL
PP_ScheduleJob
(
    __in HANDLE  hPrinter,
    __in DWORD   dwJobId
)

{
    DWORD err = NO_ERROR;

    UNREFERENCED_PARAMETER(hPrinter);
    UNREFERENCED_PARAMETER(dwJobId);



    if (err)
    {
        SetLastError(err);
    }

    return (err == NO_ERROR);
}


DWORD
PP_WaitForPrinterChange
(
    __in HANDLE  hPrinter,
    __in DWORD   dwFlags
)

{
    DWORD err = NO_ERROR;

    UNREFERENCED_PARAMETER(hPrinter);





    if (err)
    {
        SetLastError(err);
        return 0;
    }

    return dwFlags;
}


BOOL
PP_EnumPorts
(
    __in                    LPWSTR   pszName,
    __in                    DWORD    dwLevel,
    __out_bcount_opt(cbBuf) LPBYTE   pbPorts,
    __in                    DWORD    cbBuf,
    __out                   LPDWORD  pcbNeeded,
    __out                   LPDWORD  pcReturned
)

{
    DWORD         err = NO_ERROR;
    DWORD         cb = 0;
    PPORT         pPort;
    WCHAR        *pbEnd;
    PORT_INFO_1W *pInfoStruct;
    HRESULT       hr = S_OK;

    if (dwLevel != 1)
    {
        SetLastError(ERROR_INVALID_LEVEL);
        return FALSE;
    }
    else if (!IsLocalMachine(pszName))
    {
        SetLastError(ERROR_INVALID_NAME);
        return FALSE;
    }
    else if (!pcbNeeded || !pcReturned)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    else if (cbBuf && !pbPorts)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    EnterCriticalSection(&SplSem);

    pPort = pFirstPort;
    while (pPort)
    {
        cb += (DWORD)(sizeof(PORT_INFO_1W) + (wcslen(pPort->pName) + 1) * sizeof(WCHAR));
        pPort = pPort->pNext;
    }

    *pcbNeeded = cb;
    *pcReturned = 0;

    if (cb && cb <= cbBuf)
    {
        pbEnd = (WCHAR *)(pbPorts + cb);
        pInfoStruct = (PORT_INFO_1W *)pbPorts;

        cb = 0;
        pPort = pFirstPort;
        while (pPort && cb < cbBuf)
        {
            int cwbName = (int)((wcslen(pPort->pName)) + 1);
            pbEnd -= cwbName;
            cb    += cwbName * sizeof(WCHAR);

            if (cb > cbBuf)
            {
                err = ERROR_INSUFFICIENT_BUFFER;
                break;
            }

            //
            // Set a NULL just in case pPort->pName is NULL
            //
            *pbEnd = L'\0';

            hr = StringCchCopy(pbEnd, cwbName, pPort->pName);

            if (FAILED(hr))
            {
                err = StatusFromHResult(hr);
                break;
            }

            if (cb + sizeof(PORT_INFO_1W) <= cbBuf)
            {
                pInfoStruct->pName = pbEnd;
                pInfoStruct++;
                cb += sizeof(PORT_INFO_1W);
            }
            else
            {
                err = ERROR_INSUFFICIENT_BUFFER;
                break;
            }

            pPort = pPort->pNext;
            (*pcReturned)++;
        }
    }
    else
    {
        err = ERROR_INSUFFICIENT_BUFFER;
    }

    LeaveCriticalSection(&SplSem);

    if (err)
    {
        SetLastError(err);
    }

    return (err == NO_ERROR);
}


BOOL
PP_DeletePort
(
    __in LPWSTR  pszName,
    __in HWND    hWnd,
    __in LPWSTR  pszPortName
)

{
    DWORD err = NO_ERROR;
    BOOL fPortDeleted;

    UNREFERENCED_PARAMETER(hWnd);

    if (!IsLocalMachine(pszName))
    {
        SetLastError(ERROR_NOT_SUPPORTED);
        return FALSE;
    }

    fPortDeleted = DeletePortEntry(pszPortName);

    if (fPortDeleted)
    {
        err = DeleteRegistryEntry(pszPortName);
    }
    else
    {
        err = ERROR_UNKNOWN_PORT;
    }

    if (err)
    {
        SetLastError(err);
    }
    return (err == NO_ERROR);
}


BOOL
PP_ConfigurePort
(
    __in LPWSTR  pszName,
    __in HWND    hWnd,
    __in LPWSTR  pszPortName
)

{
    UNREFERENCED_PARAMETER(hWnd);

    if (!IsLocalMachine(pszName))
    {
        SetLastError(ERROR_NOT_SUPPORTED);
        return FALSE;
    }
    else if (!PortKnown(pszPortName))
    {
        SetLastError(ERROR_UNKNOWN_PORT);
        return FALSE;
    }

    return TRUE;
}

BOOL
PP_AddPrinterConnection
(
    __in LPWSTR  pszPrinterName
)
{
    UNREFERENCED_PARAMETER(pszPrinterName);

    SetLastError(ERROR_INVALID_NAME);
    return FALSE;
}



BOOL
PP_EnumMonitors
(
    __in                    LPWSTR   pszName,
    __in                    DWORD    dwLevel,
    __out_bcount_opt(cbBuf) LPBYTE   pbMonitor,
    __in                    DWORD    cbBuf,
    __out                   LPDWORD  pcbNeeded,
    __out                   LPDWORD  pcReturned
)
{
    UNREFERENCED_PARAMETER(pszName);
    UNREFERENCED_PARAMETER(dwLevel);
    UNREFERENCED_PARAMETER(pbMonitor);
    UNREFERENCED_PARAMETER(cbBuf);
    UNREFERENCED_PARAMETER(pcbNeeded);
    UNREFERENCED_PARAMETER(pcReturned);

    SetLastError(ERROR_INVALID_NAME);
    return FALSE;
}


BOOL
PP_AddPort
(
    __in LPWSTR  pszName,
    __in HWND    hWnd,
    __in LPWSTR  pszMonitorName
)
{
    UNREFERENCED_PARAMETER(pszName);
    UNREFERENCED_PARAMETER(hWnd);
    UNREFERENCED_PARAMETER(pszMonitorName);

    SetLastError(ERROR_NOT_SUPPORTED);
    return FALSE;
}


HANDLE
PP_AddPrinter
(
    __in                                LPWSTR  pszName,
    __in                                DWORD   dwLevel,
    __in_bcount(sizeof(PRINTER_INFO_2)) LPBYTE  pbPrinter
)
{
    LPTSTR     pszPrinterName = NULL;
    LPTSTR     pszQueue  =  NULL;
    HANDLE     hPrinter = NULL;
    size_t     uSize = 0;
    DWORD      err;
    PPRINTER_INFO_2 pPrinterInfo;

    UNREFERENCED_PARAMETER(pszName);

    pPrinterInfo = (PPRINTER_INFO_2)pbPrinter;

    if (dwLevel != 2)
    {
        err = ERROR_INVALID_PARAMETER;
        goto ErrorExit;
    }

    uSize = wcslen(((PRINTER_INFO_2 *)pbPrinter)->pPrinterName) + 1;

    pszPrinterName = (LPTSTR)LocalAlloc(LPTR, uSize * sizeof(WCHAR));
    if (!pszPrinterName)
    {
        err = ERROR_NOT_ENOUGH_MEMORY;
        goto ErrorExit;
    }

    err = StatusFromHResult(StringCchCopy(pszPrinterName,
                                          uSize,
                                          pPrinterInfo->pPrinterName));

    if (err != ERROR_SUCCESS)
    {
        goto ErrorExit;
    }


    if (  ( !ValidateUNCName(pszPrinterName))
       || ( (pszQueue = wcschr(pszPrinterName + 2, L'\\')) == NULL)
       || ( pszQueue == (pszPrinterName + 2))
       || ( *(pszQueue + 1) == L'\0')
       )
    {
        err =  ERROR_INVALID_NAME;
        goto ErrorExit;
    }

    if (pszPrinterName == NULL)
    {
        SetLastError(ERROR_INVALID_NAME);
        goto ErrorExit;
    }

    if (PortExists(pszPrinterName, &err) && !err)
    {
        SetLastError(ERROR_ALREADY_ASSIGNED);
        goto ErrorExit;
    }

    return hPrinter;

ErrorExit:
    if (!pszPrinterName)
    {
        (void)LocalFree((HLOCAL)pszPrinterName);
    }

    SetLastError(err);
    return (HANDLE)0x0;
}


BOOL
PP_DeletePrinter
(
    __in HANDLE  hPrinter
)
{
    LPWSTR pszPrinterName = NULL ;
    DWORD err = NO_ERROR;
    DWORD DoesPortExist = FALSE;

    UNREFERENCED_PARAMETER(hPrinter);

    pszPrinterName = (LPWSTR)LocalAlloc(LPTR,sizeof(WCHAR)*MAX_PATH);

    if (pszPrinterName == NULL)
    {
        err = ERROR_NOT_ENOUGH_MEMORY;
        return FALSE;
    }





    if (!err && PortExists(pszPrinterName, &DoesPortExist) && DoesPortExist)
    {
        if (DeleteRegistryEntry(pszPrinterName))
        {
            (void) DeletePortEntry(pszPrinterName);
        }
    }

    if (err)
    {
        SetLastError(err);
    }
    return (err == NO_ERROR);
}


BOOL
PP_DeletePrinterConnection
(
    __in LPWSTR  pszName
)
{
    UNREFERENCED_PARAMETER(pszName);

    SetLastError(ERROR_INVALID_NAME);
    return FALSE;
}


BOOL
PP_AddPrinterDriver
(
    __in LPWSTR  pszName,
    __in DWORD   dwLevel,
    __in LPBYTE  pbPrinter
)
{
    UNREFERENCED_PARAMETER(pbPrinter);
    UNREFERENCED_PARAMETER(dwLevel);
    UNREFERENCED_PARAMETER(pszName);

    SetLastError(ERROR_INVALID_NAME);
    return FALSE;
}

BOOL
PP_EnumPrinterDrivers
(
    __in                    LPWSTR   pszName,
    __in                    LPWSTR   pszEnvironment,
    __in                    DWORD    dwLevel,
    __out_bcount_opt(cbBuf) LPBYTE   pbDriverInfo,
    __in                    DWORD    cbBuf,
    __out                   LPDWORD  pcbNeeded,
    __out                   LPDWORD  pcReturned
)
{
    UNREFERENCED_PARAMETER(pszName);
    UNREFERENCED_PARAMETER(pszEnvironment);
    UNREFERENCED_PARAMETER(dwLevel);
    UNREFERENCED_PARAMETER(pbDriverInfo);
    UNREFERENCED_PARAMETER(cbBuf);
    UNREFERENCED_PARAMETER(pcbNeeded);
    UNREFERENCED_PARAMETER(pcReturned);

    SetLastError(ERROR_INVALID_NAME);
    return FALSE;
}

BOOL
PP_GetPrinterDriver
(
    __in                    HANDLE   hPrinter,
    __in_opt                LPWSTR   pszEnvironment,
    __in                    DWORD    dwLevel,
    __out_bcount_opt(cbBuf) LPBYTE   pbDriverInfo,
    __in                    DWORD    cbBuf,
    __out                   LPDWORD  pcbNeeded
)
{
    UNREFERENCED_PARAMETER(hPrinter);
    UNREFERENCED_PARAMETER(pszEnvironment);
    UNREFERENCED_PARAMETER(dwLevel);
    UNREFERENCED_PARAMETER(cbBuf);
    UNREFERENCED_PARAMETER(pbDriverInfo);
    UNREFERENCED_PARAMETER(pcbNeeded);

    SetLastError(ERROR_NOT_SUPPORTED);
    return FALSE;
}

BOOL
PP_GetPrinterDriverDirectory
(
    __in                    LPWSTR   pszName,
    __in_opt                LPWSTR   pszEnvironment,
    __in                    DWORD    dwLevel,
    __out_bcount_opt(cbBuf) LPBYTE   pbDriverDirectory,
    __in                    DWORD    cbBuf,
    __out                   LPDWORD  pcbNeeded
)
{
    UNREFERENCED_PARAMETER(pszName);
    UNREFERENCED_PARAMETER(pszEnvironment);
    UNREFERENCED_PARAMETER(dwLevel);
    UNREFERENCED_PARAMETER(cbBuf);
    UNREFERENCED_PARAMETER(pbDriverDirectory);
    UNREFERENCED_PARAMETER(pcbNeeded);

    SetLastError(ERROR_INVALID_NAME);
    return FALSE;
}

BOOL
PP_DeletePrinterDriver
(
    __in LPWSTR  pszName,
    __in LPWSTR  pszEnvironment,
    __in LPWSTR  pszDriverName
)
{
    UNREFERENCED_PARAMETER(pszName);
    UNREFERENCED_PARAMETER(pszEnvironment);
    UNREFERENCED_PARAMETER(pszDriverName);

    SetLastError(ERROR_INVALID_NAME);
    return FALSE;
}

BOOL
PP_AddPrintProcessor
(
    __in LPWSTR  pszName,
    __in LPWSTR  pszEnvironment,
    __in LPWSTR  pszPathName,
    __in LPWSTR  pszPrintProcessorName
)
{
    UNREFERENCED_PARAMETER(pszName);
    UNREFERENCED_PARAMETER(pszEnvironment);
    UNREFERENCED_PARAMETER(pszPathName);
    UNREFERENCED_PARAMETER(pszPrintProcessorName);

    SetLastError(ERROR_INVALID_NAME);
    return FALSE;
}

BOOL
PP_EnumPrintProcessors
(
    __in                    LPWSTR   pszName,
    __in_opt                LPWSTR   pszEnvironment,
    __in                    DWORD    dwLevel,
    __out_bcount_opt(cbBuf) LPBYTE   pbPrintProcessorInfo,
    __in                    DWORD    cbBuf,
    __out                   LPDWORD  pcbNeeded,
    __out                   LPDWORD  pcReturned
)
{
    UNREFERENCED_PARAMETER(pszName);
    UNREFERENCED_PARAMETER(pszEnvironment);
    UNREFERENCED_PARAMETER(dwLevel);
    UNREFERENCED_PARAMETER(cbBuf);
    UNREFERENCED_PARAMETER(pbPrintProcessorInfo);
    UNREFERENCED_PARAMETER(pcbNeeded);
    UNREFERENCED_PARAMETER(pcReturned);

    SetLastError(ERROR_INVALID_NAME);
    return FALSE;
}

BOOL
PP_EnumPrintProcessorDatatypes
(
    __in                    LPWSTR   pszName,
    __in_opt                LPWSTR   pszPrintProcessorName,
    __in                    DWORD    dwLevel,
    __out_bcount_opt(cbBuf) LPBYTE   pbDatatypes,
    __in                    DWORD    cbBuf,
    __out                   LPDWORD  pcbNeeded,
    __out                   LPDWORD  pcReturned
)
{
    UNREFERENCED_PARAMETER(pszName);
    UNREFERENCED_PARAMETER(pszPrintProcessorName);
    UNREFERENCED_PARAMETER(dwLevel);
    UNREFERENCED_PARAMETER(cbBuf);
    UNREFERENCED_PARAMETER(pbDatatypes);
    UNREFERENCED_PARAMETER(pcbNeeded);
    UNREFERENCED_PARAMETER(pcReturned);

    SetLastError(ERROR_INVALID_NAME);
    return FALSE;
}

BOOL
PP_GetPrintProcessorDirectory
(
    __in                    LPWSTR   pszName,
    __in_opt                LPWSTR   pszEnvironment,
    __in                    DWORD    dwLevel,
    __out_bcount_opt(cbBuf) LPBYTE   pbPrintProcessorDirectory,
    __in                    DWORD    cbBuf,
    __out                   LPDWORD  pcbNeeded
)
{
    UNREFERENCED_PARAMETER(pszName);
    UNREFERENCED_PARAMETER(pszEnvironment);
    UNREFERENCED_PARAMETER(dwLevel);
    UNREFERENCED_PARAMETER(cbBuf);
    UNREFERENCED_PARAMETER(pbPrintProcessorDirectory);
    UNREFERENCED_PARAMETER(pcbNeeded);

    SetLastError(ERROR_INVALID_NAME);
    return FALSE;
}

BOOL
PP_StartPagePrinter
(
    __in HANDLE  hPrinter
)
{
    UNREFERENCED_PARAMETER(hPrinter);

    SetLastError(ERROR_NOT_SUPPORTED);
    return FALSE;
}

BOOL
PP_EndPagePrinter
(
    __in HANDLE  hPrinter
)
{
    UNREFERENCED_PARAMETER(hPrinter);

    SetLastError(ERROR_NOT_SUPPORTED);
    return FALSE;
}

BOOL
PP_ReadPrinter
(
    __in  HANDLE   hPrinter,
    __out LPVOID   pBuf,
    __in  DWORD    cbBuf,
    __out LPDWORD  pcbRead
)
{
    UNREFERENCED_PARAMETER(hPrinter);
    UNREFERENCED_PARAMETER(cbBuf);
    UNREFERENCED_PARAMETER(pBuf);
    UNREFERENCED_PARAMETER(pcbRead);

    SetLastError(ERROR_NOT_SUPPORTED);
    return FALSE;
}

DWORD
PP_GetPrinterData
(
    __in                    HANDLE   hPrinter,
    __in                    LPWSTR   pszValueName,
    __out                   LPDWORD  pdwType,
    __out_bcount_opt(cbBuf) LPBYTE   pbData,
    __in                    DWORD    cbBuf,
    __out                   LPDWORD  pcbNeeded
)
{
    UNREFERENCED_PARAMETER(hPrinter);
    UNREFERENCED_PARAMETER(pszValueName);
    UNREFERENCED_PARAMETER(cbBuf);
    UNREFERENCED_PARAMETER(pdwType);
    UNREFERENCED_PARAMETER(pbData);
    UNREFERENCED_PARAMETER(pcbNeeded);

    SetLastError(ERROR_NOT_SUPPORTED);
    return FALSE;
}

DWORD
PP_SetPrinterData
(
    __in                HANDLE  hPrinter,
    __in                LPWSTR  pszValueName,
    __in                DWORD   dwType,
    __in_bcount(cbData) LPBYTE  pbData,
    __in                DWORD   cbData
)
{
    UNREFERENCED_PARAMETER(hPrinter);
    UNREFERENCED_PARAMETER(pszValueName);
    UNREFERENCED_PARAMETER(dwType);
    UNREFERENCED_PARAMETER(pbData);
    UNREFERENCED_PARAMETER(cbData);

    SetLastError(ERROR_NOT_SUPPORTED);
    return FALSE;
}

BOOL
PP_AddForm
(
    __in   HANDLE  hPrinter,
    __in   DWORD   dwLevel,
    __in   LPBYTE  pbForm
)
{
    UNREFERENCED_PARAMETER(hPrinter);
    UNREFERENCED_PARAMETER(dwLevel);
    UNREFERENCED_PARAMETER(pbForm);

    SetLastError(ERROR_NOT_SUPPORTED);
    return FALSE;
}

BOOL
PP_DeleteForm
(
    __in   HANDLE  hPrinter,
    __in   LPWSTR  pszFormName
)
{
    UNREFERENCED_PARAMETER(hPrinter);
    UNREFERENCED_PARAMETER(pszFormName);

    SetLastError(ERROR_NOT_SUPPORTED);
    return FALSE;
}

BOOL
PP_GetForm
(
    __in                    HANDLE   hPrinter,
    __in                    LPWSTR   pszFormName,
    __in                    DWORD    dwLevel,
    __out_bcount_opt(cbBuf) LPBYTE   pbForm,
    __in                    DWORD    cbBuf,
    __out                   LPDWORD  pcbNeeded
)
{
    UNREFERENCED_PARAMETER(hPrinter);
    UNREFERENCED_PARAMETER(pszFormName);
    UNREFERENCED_PARAMETER(dwLevel);
    UNREFERENCED_PARAMETER(cbBuf);
    UNREFERENCED_PARAMETER(pbForm);
    UNREFERENCED_PARAMETER(pcbNeeded);

    SetLastError(ERROR_NOT_SUPPORTED);
    return FALSE;
}

BOOL
PP_SetForm
(
    __in HANDLE  hPrinter,
    __in LPWSTR  pszFormName,
    __in DWORD   dwLevel,
    __in LPBYTE  pbForm
)
{
    UNREFERENCED_PARAMETER(hPrinter);
    UNREFERENCED_PARAMETER(pszFormName);
    UNREFERENCED_PARAMETER(dwLevel);
    UNREFERENCED_PARAMETER(pbForm);

    SetLastError(ERROR_NOT_SUPPORTED);
    return FALSE;
}

BOOL
PP_EnumForms
(
    __in                    HANDLE   hPrinter,
    __in                    DWORD    dwLevel,
    __out_bcount_opt(cbBuf) LPBYTE   pbForm,
    __in                    DWORD    cbBuf,
    __out                   LPDWORD  pcbNeeded,
    __out                   LPDWORD  pcReturned
)
{
    UNREFERENCED_PARAMETER(hPrinter);
    UNREFERENCED_PARAMETER(dwLevel);
    UNREFERENCED_PARAMETER(cbBuf);
    UNREFERENCED_PARAMETER(pbForm);
    UNREFERENCED_PARAMETER(pcbNeeded);
    UNREFERENCED_PARAMETER(pcReturned);

    SetLastError(ERROR_NOT_SUPPORTED);
    return FALSE;
}


HANDLE
PP_CreatePrinterIC
(
    __in HANDLE     hPrinter,
    __in LPDEVMODE  pDevMode
)
{
    UNREFERENCED_PARAMETER(hPrinter);
    UNREFERENCED_PARAMETER(pDevMode);

    SetLastError(ERROR_NOT_SUPPORTED);
    return FALSE;
}

BOOL
PP_PlayGdiScriptOnPrinterIC
(
    __in                    HANDLE  hPrinterIC,
    __in_bcount(cbIn)       LPBYTE  pbIn,
    __in                    DWORD   cbIn,
    __out_bcount_opt(cbOut) LPBYTE  pbOut,
    __in                    DWORD   cbOut,
    __in                    DWORD   ul
)
{
    UNREFERENCED_PARAMETER(hPrinterIC);
    UNREFERENCED_PARAMETER(pbIn);
    UNREFERENCED_PARAMETER(cbIn);
    UNREFERENCED_PARAMETER(cbOut);
    UNREFERENCED_PARAMETER(ul);
    UNREFERENCED_PARAMETER(pbOut);

    SetLastError(ERROR_NOT_SUPPORTED);
    return FALSE;
}

BOOL
PP_DeletePrinterIC
(
    __in HANDLE  hPrinterIC
)
{
    UNREFERENCED_PARAMETER(hPrinterIC);

    SetLastError(ERROR_NOT_SUPPORTED);
    return FALSE;
}

DWORD
PP_PrinterMessageBox
(
    __in HANDLE  hPrinter,
    __in DWORD   dwError,
    __in HWND    hWnd,
    __in LPWSTR  pszText,
    __in LPWSTR  pszCaption,
    __in DWORD   dwType
)
{
    UNREFERENCED_PARAMETER(hPrinter);
    UNREFERENCED_PARAMETER(dwError);
    UNREFERENCED_PARAMETER(hWnd);
    UNREFERENCED_PARAMETER(pszText);
    UNREFERENCED_PARAMETER(pszCaption);
    UNREFERENCED_PARAMETER(dwType);

    SetLastError(ERROR_NOT_SUPPORTED);
    return FALSE;
}

BOOL
PP_AddMonitor
(
    __in LPWSTR  pszName,
    __in DWORD   dwLevel,
    __in LPBYTE  pbMonitorInfo
)
{
    UNREFERENCED_PARAMETER(pszName);
    UNREFERENCED_PARAMETER(dwLevel);
    UNREFERENCED_PARAMETER(pbMonitorInfo);

    SetLastError(ERROR_INVALID_NAME);
    return FALSE;
}

BOOL
PP_DeleteMonitor
(
    __in LPWSTR  pszName,
    __in LPWSTR  pszEnvironment,
    __in LPWSTR  pszMonitorName
)
{
    UNREFERENCED_PARAMETER(pszName);
    UNREFERENCED_PARAMETER(pszEnvironment);
    UNREFERENCED_PARAMETER(pszMonitorName);

    SetLastError(ERROR_INVALID_NAME);
    return FALSE;
}

BOOL
PP_DeletePrintProcessor
(
    __in LPWSTR  pszName,
    __in LPWSTR  pszEnvironment,
    __in LPWSTR  pszPrintProcessorName
)
{
    UNREFERENCED_PARAMETER(pszName);
    UNREFERENCED_PARAMETER(pszEnvironment);
    UNREFERENCED_PARAMETER(pszPrintProcessorName);

    SetLastError(ERROR_INVALID_NAME);
    return FALSE;
}

