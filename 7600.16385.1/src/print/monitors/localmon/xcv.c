/*++

Copyright (c) 1990-2003  Microsoft Corporation
All rights reserved

Module Name:

    xcv.c

Abstract:

    Implements xcv functions.

--*/

#include "precomp.h"


#pragma hdrstop

#include <DriverSpecs.h>
__user_driver

//
// The ddk montior samples will be build with the name ddklocalmon and ddklocalui
// so they can be installed without clashing with existing files
//
//
// change this to the name of the dll that provides the ui for the monitor
//
#define SZLOCALUI  L"DDKLocalUI.dll"

DWORD
GetMonitorUI(
    __in_bcount(cbInputData)    PBYTE       pInputData,
    IN                          DWORD       cbInputData,
    __out_bcount(cbOutputData)  PBYTE       pOutputData,
    IN                          DWORD       cbOutputData,
    __out                       PDWORD      pcbOutputNeeded,
    __inout                     PINIXCVPORT pIniXcv
);

DWORD
DoPortExists(
    __in_bcount(cbInputData)    PBYTE       pInputData,
    IN                          DWORD       cbInputData,
    __out_bcount(cbOutputData)  PBYTE       pOutputData,
    IN                          DWORD       cbOutputData,
    __out                       PDWORD      pcbOutputNeeded,
    __inout                     PINIXCVPORT pIniXcv
);

DWORD
DoPortIsValid(
    __in_bcount(cbInputData)    PBYTE       pInputData,
    IN                          DWORD       cbInputData,
    __out_bcount(cbOutputData)  PBYTE       pOutputData,
    IN                          DWORD       cbOutputData,
    __out                       PDWORD      pcbOutputNeeded,
    __inout                     PINIXCVPORT pIniXcv
);

DWORD
DoDeletePort(
    __in_bcount(cbInputData)    PBYTE       pInputData,
    IN                          DWORD       cbInputData,
    __out_bcount(cbOutputData)  PBYTE       pOutputData,
    IN                          DWORD       cbOutputData,
    __out                       PDWORD      pcbOutputNeeded,
    __inout                     PINIXCVPORT pIniXcv
);

DWORD
DoAddPort(
    __in_bcount(cbInputData)    PBYTE       pInputData,
    IN                          DWORD       cbInputData,
    __out_bcount(cbOutputData) PBYTE        pOutputData,
    IN                          DWORD       cbOutputData,
    __out                       PDWORD      pcbOutputNeeded,
    __inout                     PINIXCVPORT pIniXcv
);

DWORD
DoSetDefaultCommConfig(
    __in_bcount(cbInputData)    PBYTE       pInputData,
    IN                          DWORD       cbInputData,
    __out_bcount(cbOutputData) PBYTE        pOutputData,
    IN                          DWORD       cbOutputData,
    __out                       PDWORD      pcbOutputNeeded,
    __inout                     PINIXCVPORT pIniXcv
);

DWORD
DoGetDefaultCommConfig(
    __in_bcount(cbInputData)    PBYTE       pInputData,
    IN                          DWORD       cbInputData,
    __out_bcount(cbOutputData)  PBYTE       pOutputData,
    IN                          DWORD       cbOutputData,
    __out                       PDWORD      pcbOutputNeeded,
    __inout                     PINIXCVPORT pIniXcv
);


DWORD
GetTransmissionRetryTimeout(
    __in_bcount(cbInputData)    PBYTE       pInputData,
    IN                          DWORD       cbInputData,
    __out_bcount(cbOutputData)  PBYTE       pOutputData,
    IN                          DWORD       cbOutputData,
    __out                       PDWORD      pcbOutputNeeded,
    __inout                     PINIXCVPORT pIniXcv
);


typedef struct {
    PWSTR   pszMethod;
    DWORD   (*pfn)( PBYTE  pInputData,
                    DWORD  cbInputData,
                    PBYTE  pOutputData,
                    DWORD  cbOutputData,
                    PDWORD pcbOutputNeeded,
                    PINIXCVPORT pIniXcv
                    );
} XCV_METHOD, *PXCV_METHOD;


XCV_METHOD  gpLcmXcvMethod[] = {
                            {L"MonitorUI", GetMonitorUI},
                            {L"ConfigureLPTPortCommandOK", ConfigureLPTPortCommandOK},
                            {L"AddPort", DoAddPort},
                            {L"DeletePort", DoDeletePort},
                            {L"PortExists", DoPortExists},
                            {L"PortIsValid", DoPortIsValid},
                            {L"GetTransmissionRetryTimeout", GetTransmissionRetryTimeout},
                            {L"SetDefaultCommConfig", DoSetDefaultCommConfig},
                            {L"GetDefaultCommConfig", DoGetDefaultCommConfig},
                            {NULL, NULL}
                            };

DWORD
LcmXcvDataPort(
    __in IN                          HANDLE  hXcv,
    IN                          LPCWSTR pszDataName,
    __in_bcount(cbInputData)    PBYTE   pInputData,
    IN                          DWORD   cbInputData,
    __out_bcount(cbOutputData)  PBYTE   pOutputData,
    IN                          DWORD   cbOutputData,
    __out                       PDWORD  pcbOutputNeeded
    )
{
    DWORD dwRet = 0;
    DWORD i     = 0;


    for(i = 0 ; gpLcmXcvMethod[i].pszMethod &&
                wcscmp(gpLcmXcvMethod[i].pszMethod, pszDataName) ; ++i)
        ;

    if (gpLcmXcvMethod[i].pszMethod) {
        dwRet = (*gpLcmXcvMethod[i].pfn)(  pInputData,
                                        cbInputData,
                                        pOutputData,
                                        cbOutputData,
                                        pcbOutputNeeded,
                                        (PINIXCVPORT) hXcv);

    } else {
        dwRet = ERROR_INVALID_PARAMETER;
    }

    return dwRet;
}

BOOL
LcmXcvOpenPort(
    __in    HANDLE hMonitor,
            LPCWSTR pszObject,
            ACCESS_MASK GrantedAccess,
            PHANDLE phXcv
    )
{
    PINILOCALMON pIniLocalMon = (PINILOCALMON)hMonitor;


    *phXcv = CreateXcvPortEntry(pIniLocalMon, pszObject, GrantedAccess);

    return !!*phXcv;
}


BOOL
LcmXcvClosePort(
    __in    HANDLE  hXcv
    )
{

    LcmEnterSplSem();
    DeleteXcvPortEntry((PINIXCVPORT) hXcv);
    LcmLeaveSplSem();

    return TRUE;
}

DWORD
DoDeletePort(
    __in_bcount(cbInputData)    PBYTE       pInputData,
    IN                          DWORD       cbInputData,
    __out_bcount(cbOutputData)  PBYTE       pOutputData,
    IN                          DWORD       cbOutputData,
    __out                       PDWORD      pcbOutputNeeded,
    __inout                     PINIXCVPORT pIniXcv
)
{
    DWORD dwRet = ERROR_SUCCESS;

    UNREFERENCED_PARAMETER(cbInputData);
    UNREFERENCED_PARAMETER(pOutputData);
    UNREFERENCED_PARAMETER(cbOutputData);
    UNREFERENCED_PARAMETER(pcbOutputNeeded);


    if (!(pIniXcv->GrantedAccess & SERVER_ACCESS_ADMINISTER))
        return ERROR_ACCESS_DENIED;

    LcmEnterSplSem();

    if (LcmDeletePortEntry( pIniXcv->pIniLocalMon, (PWSTR)pInputData))
    {
        DeletePortFromRegistry ((PWSTR) pInputData);
    }
    else
        dwRet = ERROR_FILE_NOT_FOUND;

    LcmLeaveSplSem();

    return dwRet;
}


DWORD
DoPortExists(
    __in_bcount(cbInputData)    PBYTE       pInputData,
    IN                          DWORD       cbInputData,
    __out_bcount(cbOutputData)  PBYTE       pOutputData,
    IN                          DWORD       cbOutputData,
    __out                       PDWORD      pcbOutputNeeded,
    __inout                     PINIXCVPORT pIniXcv
)
{
    DWORD dwRet         = ERROR_SUCCESS;
    BOOL  bPortExists   = FALSE;

    UNREFERENCED_PARAMETER(cbInputData);
    UNREFERENCED_PARAMETER(pIniXcv);


    *pcbOutputNeeded = sizeof(DWORD);

    if (cbOutputData < sizeof(DWORD)) {
        return ERROR_INSUFFICIENT_BUFFER;
    }

    bPortExists = PortExists(NULL, (PWSTR) pInputData, &dwRet);

    if (dwRet == ERROR_SUCCESS)
        *(DWORD *) pOutputData = bPortExists;

    return dwRet;
}


DWORD
DoPortIsValid(
    __in_bcount(cbInputData)    PBYTE       pInputData,
    IN                          DWORD       cbInputData,
    __out_bcount(cbOutputData)  PBYTE       pOutputData,
    IN                          DWORD       cbOutputData,
    __out                       PDWORD      pcbOutputNeeded,
    __inout                     PINIXCVPORT pIniXcv
)
{
    BOOL bRet = FALSE;

    UNREFERENCED_PARAMETER(cbInputData);
    UNREFERENCED_PARAMETER(pOutputData);
    UNREFERENCED_PARAMETER(cbOutputData);
    UNREFERENCED_PARAMETER(pcbOutputNeeded);
    UNREFERENCED_PARAMETER(pIniXcv);


    bRet = PortIsValid((PWSTR) pInputData);

    return bRet ? ERROR_SUCCESS : GetLastError();
}



DWORD
DoAddPort(
    __in_bcount(cbInputData)    PBYTE       pInputData,
    IN                          DWORD       cbInputData,
    __out_bcount(cbOutputData)  PBYTE       pOutputData,
    IN                          DWORD       cbOutputData,
    __out                       PDWORD      pcbOutputNeeded,
    __inout                     PINIXCVPORT pIniXcv
)
{
    DWORD       dwRet       = ERROR_SUCCESS;
    BOOL        bPortExists = FALSE;
    PLCMINIPORT pIniPort    = NULL;

    UNREFERENCED_PARAMETER(pOutputData);
    UNREFERENCED_PARAMETER(cbOutputData);
    UNREFERENCED_PARAMETER(pcbOutputNeeded);


    if (!(pIniXcv->GrantedAccess & SERVER_ACCESS_ADMINISTER))
        return ERROR_ACCESS_DENIED;

    if ( cbInputData == 0   ||
         pInputData == NULL ||
         ( wcslen((LPWSTR)pInputData) + 1 ) * sizeof(WCHAR) != cbInputData )
        return ERROR_INVALID_PARAMETER;

    bPortExists = PortExists(NULL, (PWSTR) pInputData, &dwRet);

    if (dwRet == ERROR_SUCCESS) {
        if (bPortExists) {
            SetLastError(ERROR_ALREADY_EXISTS);
        } else {
            pIniPort = LcmCreatePortEntry( pIniXcv->pIniLocalMon, (PWSTR)pInputData );

            if (pIniPort) {
                if (!AddPortInRegistry ((PWSTR) pInputData))
                {
                    LcmDeletePortEntry(pIniXcv->pIniLocalMon, (PWSTR) pInputData);
                    pIniPort = NULL;
                }
            }
        }
    }

    return pIniPort ? ERROR_SUCCESS : GetLastError();
}


DWORD
DoSetDefaultCommConfig(
    __in_bcount(cbInputData)    PBYTE       pInputData,
    IN                          DWORD       cbInputData,
    __out_bcount(cbOutputData)  PBYTE       pOutputData,
    IN                          DWORD       cbOutputData,
    __out                       PDWORD      pcbOutputNeeded,
    __inout                     PINIXCVPORT pIniXcv
)
{
    BOOL        bRet            = FALSE;
    DWORD       dwLength        = 0;
    PWSTR       pszPortName     = NULL;
    COMMCONFIG  *pCommConfig    = (COMMCONFIG *) pInputData;
    HANDLE      hToken          = NULL;

    UNREFERENCED_PARAMETER(cbInputData);
    UNREFERENCED_PARAMETER(pOutputData);
    UNREFERENCED_PARAMETER(cbOutputData);
    UNREFERENCED_PARAMETER(pcbOutputNeeded);


    if (!(pIniXcv->GrantedAccess & SERVER_ACCESS_ADMINISTER))
        return ERROR_ACCESS_DENIED;

    dwLength = (DWORD)wcslen(pIniXcv->pszName);
    if (pIniXcv->pszName[dwLength - 1] == L':') {
        pszPortName = AllocSplStr(pIniXcv->pszName);
        if (!pszPortName)
            goto Done;
        pszPortName[dwLength - 1] = L'\0';
    }

    hToken = RevertToPrinterSelf();

    if (hToken)
    {
        bRet = SetDefaultCommConfig(pszPortName,
                                    pCommConfig,
                                    pCommConfig->dwSize);

        ImpersonatePrinterClient(hToken);
    }

Done:

    FreeSplStr(pszPortName);

    return bRet ? ERROR_SUCCESS : GetLastError();
}


DWORD
DoGetDefaultCommConfig(
    __in_bcount(cbInputData)    PBYTE       pInputData,
    IN                          DWORD       cbInputData,
    __out_bcount(cbOutputData)  PBYTE       pOutputData,
    IN                          DWORD       cbOutputData,
    __out                       PDWORD      pcbOutputNeeded,
    __inout                     PINIXCVPORT pIniXcv
)
{
    PWSTR       pszPortName     = (PWSTR) pInputData;
    COMMCONFIG  *pCommConfig    = (COMMCONFIG *) pOutputData;
    BOOL        bRet            = FALSE;

    UNREFERENCED_PARAMETER(cbInputData);
    UNREFERENCED_PARAMETER(pIniXcv);


    if (cbOutputData < sizeof(COMMCONFIG))
    {
        //
        // at least size of COMMCONFIG is needed
        //
        if (pcbOutputNeeded)
        {
            *pcbOutputNeeded = sizeof (COMMCONFIG);
        }
        return ERROR_INSUFFICIENT_BUFFER;
    }

    pCommConfig->dwProviderSubType = PST_RS232;
    *pcbOutputNeeded = cbOutputData;

    bRet = GetDefaultCommConfig(pszPortName, pCommConfig, pcbOutputNeeded);

    return bRet ? ERROR_SUCCESS : GetLastError();
}

DWORD
GetTransmissionRetryTimeout(
    __in_bcount(cbInputData)    PBYTE       pInputData,
    IN                          DWORD       cbInputData,
    __out_bcount(cbOutputData)  PBYTE       pOutputData,
    IN                          DWORD       cbOutputData,
    __out                       PDWORD      pcbOutputNeeded,
    __inout                     PINIXCVPORT pIniXcv
)
{
    UNREFERENCED_PARAMETER(pInputData);
    UNREFERENCED_PARAMETER(cbInputData);
    UNREFERENCED_PARAMETER(pIniXcv);


    *pcbOutputNeeded = sizeof(DWORD);

    if (cbOutputData < sizeof(DWORD))
        return ERROR_INSUFFICIENT_BUFFER;

    GetTransmissionRetryTimeoutFromRegistry ((PDWORD) pOutputData);

    return ERROR_SUCCESS;
}

DWORD
GetMonitorUI(
    __in_bcount(cbInputData)    PBYTE       pInputData,
    IN                          DWORD       cbInputData,
    __out_bcount(cbOutputData)  PBYTE       pOutputData,
    IN                          DWORD       cbOutputData,
    __out                       PDWORD      pcbOutputNeeded,
    __inout                     PINIXCVPORT pIniXcv
)
{
    DWORD dwRet = ERROR_SUCCESS;

    UNREFERENCED_PARAMETER(pInputData);
    UNREFERENCED_PARAMETER(cbInputData);
    UNREFERENCED_PARAMETER(pIniXcv);


    *pcbOutputNeeded = sizeof( SZLOCALUI );

    if (cbOutputData < *pcbOutputNeeded) {

        dwRet =  ERROR_INSUFFICIENT_BUFFER;

    } else {

        dwRet = HRESULT_CODE (StringCbCopy ((PWSTR) pOutputData, cbOutputData, SZLOCALUI));
    }

    return dwRet;
}


DWORD
ConfigureLPTPortCommandOK(
    __in_bcount(cbInputData)    PBYTE       pInputData,
    IN                          DWORD       cbInputData,
    __out_bcount(cbOutputData)  PBYTE       pOutputData,
    IN                          DWORD       cbOutputData,
    __out                       PDWORD      pcbOutputNeeded,
    __inout                     PINIXCVPORT pIniXcv
)
{
    DWORD dwStatus  = ERROR_SUCCESS;

    UNREFERENCED_PARAMETER(cbInputData);
    UNREFERENCED_PARAMETER(pOutputData);
    UNREFERENCED_PARAMETER(cbOutputData);
    UNREFERENCED_PARAMETER(pcbOutputNeeded);


    if (!(pIniXcv->GrantedAccess & SERVER_ACCESS_ADMINISTER))
    {
        dwStatus = ERROR_ACCESS_DENIED;
    }
    //
    // set timeout value in Registry
    //
    if (dwStatus == ERROR_SUCCESS)
    {
        dwStatus = SetTransmissionRetryTimeoutInRegistry ((LPWSTR) pInputData);
    }
    return dwStatus;
}


