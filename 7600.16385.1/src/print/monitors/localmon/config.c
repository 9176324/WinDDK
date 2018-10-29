/*++

Copyright (c) 1990-2003  Microsoft Corporation
All rights reserved

Module Name:

    config.c

Abstract:

    Handles spooler entry points for adding, deleting, and configuring
    localmon ports.

--*/

#include "precomp.h"


#pragma hdrstop

#include <DriverSpecs.h>
__user_driver

PLCMINIPORT
LcmCreatePortEntry(
    __inout PINILOCALMON pIniLocalMon,
    __in    PWSTR pPortName
    )
{
    DWORD       cb          = 0;
    PLCMINIPORT pIniPort    = NULL;
    PLCMINIPORT pPort       = NULL;
    size_t      cchPortName = wcslen (pPortName) + 1;


    if (!pPortName || wcslen(pPortName) > 247)
    {
        SetLastError(ERROR_INVALID_NAME);
        return NULL;
    }

    cb = (DWORD) (sizeof(LCMINIPORT) + cchPortName * sizeof (WCHAR));

    pIniPort = (PLCMINIPORT)AllocSplMem(cb);

    if( pIniPort )
    {
        ZeroMemory(pIniPort, cb);

        pIniPort->pName = (LPWSTR)(pIniPort+1);
        (VOID) StringCchCopy (pIniPort->pName, cchPortName, pPortName);
        pIniPort->cb = cb;
        pIniPort->cRef = 0;
        pIniPort->pNext = 0;
        pIniPort->pIniLocalMon = pIniLocalMon;
        pIniPort->signature = IPO_SIGNATURE;


        pIniPort->hFile = INVALID_HANDLE_VALUE;

        LcmEnterSplSem();

        pPort = pIniLocalMon->pIniPort;
        if (pPort) {

            while (pPort->pNext)
                pPort = pPort->pNext;

            pPort->pNext = pIniPort;

        } else
            pIniLocalMon->pIniPort = pIniPort;

        LcmLeaveSplSem();
    }

    return pIniPort;
}


PINIXCVPORT
CreateXcvPortEntry(
    __inout PINILOCALMON pIniLocalMon,
            LPCWSTR pszName,
            ACCESS_MASK GrantedAccess
)
{
    DWORD       cb          = 0;
    PINIXCVPORT pIniXcvPort = NULL;
    PINIXCVPORT pPort       = NULL;
    size_t      cchName     = wcslen (pszName) + 1;


    cb = (DWORD) (sizeof(INIXCVPORT) + cchName*sizeof(WCHAR));

    pIniXcvPort = (PINIXCVPORT)AllocSplMem(cb);

    if( pIniXcvPort )
    {
        pIniXcvPort->pszName = (LPWSTR)(pIniXcvPort+1);
        (VOID) StringCchCopy (pIniXcvPort->pszName, cchName, pszName);
        pIniXcvPort->dwMethod = 0;
        pIniXcvPort->cb = cb;
        pIniXcvPort->pNext = 0;
        pIniXcvPort->signature = XCV_SIGNATURE;
        pIniXcvPort->GrantedAccess = GrantedAccess;
        pIniXcvPort->pIniLocalMon = pIniLocalMon;


        pPort = pIniLocalMon->pIniXcvPort;
        if (pPort) {

            while (pPort->pNext)
                pPort = pPort->pNext;

            pPort->pNext = pIniXcvPort;

        } else
            pIniLocalMon->pIniXcvPort = pIniXcvPort;
    }

    return pIniXcvPort;
}

BOOL
DeleteXcvPortEntry(
    __in    PINIXCVPORT  pIniXcvPort
)
{
    PINILOCALMON pIniLocalMon   = pIniXcvPort->pIniLocalMon;
    PINIXCVPORT  pPort          = NULL;
    PINIXCVPORT  pPrevPort      = NULL;


    for (pPort = pIniLocalMon->pIniXcvPort;
         pPort && pPort != pIniXcvPort;
         pPort = pPort->pNext){

        pPrevPort = pPort;
    }

    if (pPort) {    // found the port
        if (pPort == pIniLocalMon->pIniXcvPort) {
            pIniLocalMon->pIniXcvPort = pPort->pNext;
        } else {
            pPrevPort->pNext = pPort->pNext;
        }

        FreeSplMem(pPort);

        return TRUE;
    }
    else            // port not found
        return FALSE;
}



BOOL
LcmDeletePortEntry(
    __inout PINILOCALMON pIniLocalMon,
    __in    LPWSTR   pPortName
)
{
    PLCMINIPORT    pPort = NULL, pPrevPort = NULL;


    pPort = pIniLocalMon->pIniPort;

    while (pPort) {

        if (!lstrcmpi(pPort->pName, pPortName)) {
            if (pPort->Status & PP_FILEPORT) {
                pPrevPort = pPort;
                pPort = pPort->pNext;
                continue;
            }
            break;
        }

        pPrevPort = pPort;
        pPort = pPort->pNext;
    }

    if (pPort) {
        if (pPort == pIniLocalMon->pIniPort) {
            pIniLocalMon->pIniPort = pPort->pNext;
        } else {
            pPrevPort->pNext = pPort->pNext;
        }
        FreeSplMem(pPort);

        return TRUE;
    }
    else
        return FALSE;
}



DWORD
GetPortSize(
    __in    PLCMINIPORT pIniPort,
            DWORD   Level
)
{
    DWORD   cb  = 0;


    switch (Level) {

    case 1:

        cb = (DWORD) (sizeof(PORT_INFO_1) + wcslen(pIniPort->pName)*sizeof(WCHAR) + sizeof(WCHAR));
        break;

    case 2:
        {
            LPWSTR  pszLocalMonitor = NULL;
            LPWSTR  pszPortDesc     = NULL;

            pszLocalMonitor = (LPWSTR)AllocSplMem((MAX_PATH + 1) * sizeof(WCHAR));
            pszPortDesc = (LPWSTR)AllocSplMem((MAX_PATH + 1) * sizeof(WCHAR));

            if (pszLocalMonitor && pszPortDesc)
            {
                LoadString(LcmhInst, IDS_LOCALMONITORNAME, pszLocalMonitor, MAX_PATH);
                LoadString(LcmhInst, IDS_LOCALMONITOR, pszPortDesc, MAX_PATH);
                cb = (DWORD) (wcslen(pIniPort->pName) + 1 +
                              wcslen(pszLocalMonitor) + 1 +
                              wcslen(pszPortDesc) + 1);
                cb *= sizeof(WCHAR);
                cb += sizeof(PORT_INFO_2);
            }

            if (pszLocalMonitor)
            {
                FreeSplMem(pszLocalMonitor);
                pszLocalMonitor = NULL;
            }

            if (pszPortDesc)
            {
                FreeSplMem(pszPortDesc);
                pszPortDesc = NULL;
            }
        }

        break;

    default:
        cb = 0;
        break;
    }

    return cb;
}

LPBYTE
CopyIniPortToPort(
    __in    PLCMINIPORT pIniPort,
            DWORD       Level,
    __out   LPBYTE      pPortInfo,
    __inout LPBYTE      pEnd
)
{
    LPWSTR          *SourceStrings              = NULL;
    LPWSTR          *pSourceStrings             = NULL;
    PPORT_INFO_2    pPort2                      = (PPORT_INFO_2)pPortInfo;
    LPWSTR          pszLocalMonitor             = NULL;
    LPWSTR          pszPortDesc                 = NULL;
    DWORD           *pOffsets                   = NULL;
    DWORD           Count                       = 0;
    LPBYTE          pReturnPointer              = NULL;


    switch (Level) {

    case 1:
        pOffsets = LcmPortInfo1Strings;
        break;

    case 2:
        pOffsets = LcmPortInfo2Strings;
        break;

    default:

        goto Done;
    }

    for ( Count = 0 ; pOffsets[Count] != -1 ; ++Count ) {
    }

    SourceStrings = pSourceStrings = (LPWSTR *)AllocSplMem(Count * sizeof(LPWSTR));

    if ( !SourceStrings ) {


        goto Done;
    }

    switch (Level) {

    case 1:
        *pSourceStrings++=pIniPort->pName;

        break;

    case 2:
        {
            pszLocalMonitor = (LPWSTR)AllocSplMem((MAX_PATH + 1) * sizeof(WCHAR));
            pszPortDesc = (LPWSTR)AllocSplMem((MAX_PATH + 1) * sizeof(WCHAR));

            if (pszLocalMonitor && pszPortDesc)
            {
                *pSourceStrings++=pIniPort->pName;

                LoadString(LcmhInst, IDS_LOCALMONITORNAME, pszLocalMonitor, MAX_PATH);
                LoadString(LcmhInst, IDS_LOCALMONITOR, pszPortDesc, MAX_PATH);
                *pSourceStrings++ = pszLocalMonitor;
                *pSourceStrings++ = pszPortDesc;

                pPort2->fPortType = PORT_TYPE_WRITE | PORT_TYPE_READ;

                // Reserved
                pPort2->Reserved = 0;
            }
            else
            {
                goto Done;
            }
        }

        break;

    default:
        goto Done;
    }

    pEnd = LcmPackStrings(Count, SourceStrings, pPortInfo, pOffsets, pEnd);

    pReturnPointer = pEnd;

Done:
    if (SourceStrings)
    {
        FreeSplMem(SourceStrings);
        SourceStrings = NULL;
    }

    if (pszLocalMonitor)
    {
        FreeSplMem(pszLocalMonitor);
        pszLocalMonitor = NULL;
    }

    if (pszPortDesc)
    {
        FreeSplMem(pszPortDesc);
        pszPortDesc = NULL;
    }

    return pReturnPointer;
}



