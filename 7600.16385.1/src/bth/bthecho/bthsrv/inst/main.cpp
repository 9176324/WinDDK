/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    Main.cpp

Abstract:

    Utility for installing and uninstalling WdfBthSampleSrv echo bth server.

    bthsrvinst /i (installs bth service) 
    bthsrvinst /u (uninstalls bth service)

Environment:

    user mode only

--*/

#include <windows.h>
#include <bthsdpdef.h>
#include <bthdef.h>
#include <BlueToothApis.h>
#include <stdio.h>
#include <strsafe.h>
#include <initguid.h>
#include "public.h"

#define ASSERT(b)

#define SWITCH_ARG_LEN 2

DWORD
ReadSwitch(
    __in LPCSTR SwitchArg,
    __out char * Switch
    );

BOOL
AdjustProcessPrivileges(
    );

DWORD
SetBthServiceInfo(
    BOOLEAN bEnabled
    );

void
__cdecl
main(
    __in UINT nArgs,
    __in_ecount(nArgs) LPCSTR Args[]
    )
{
    if (nArgs < 2)
    {
        goto usage;
    }

    char Switch;

    if (ERROR_SUCCESS != ReadSwitch(Args[1], &Switch))
    {
        goto exit;
    }

    if (FALSE == AdjustProcessPrivileges())
    {
        goto exit;
    }

    switch(Switch)
    {
        case 'i':
            SetBthServiceInfo(TRUE);
            break;
        case 'u':
            SetBthServiceInfo(FALSE);
            break;
        default:
            goto usage;
    }    
    
exit:
    return;
    
usage:
    printf("Usage: bthsrvinst [/i | /u]\n");
    goto exit;
}

DWORD SetBthServiceInfo(
    BOOLEAN bEnabled
    )
{
    DWORD err = ERROR_SUCCESS;
    BLUETOOTH_LOCAL_SERVICE_INFO SvcInfo = {0};
    SvcInfo.Enabled = bEnabled;

    if (FAILED(StringCbCopyW(SvcInfo.szName, sizeof(SvcInfo.szName), BthEchoSampleSvcName)))
    {
        printf("Copying svc name failed\n");
        goto exit;
    }

    if (ERROR_SUCCESS != (err = BluetoothSetLocalServiceInfo(
        NULL, //callee would select the first found radio
        &BTHECHOSAMPLE_SVC_GUID,
        0,
        &SvcInfo
        )))
    {
        printf("BluetoothSetLocalServiceInfo failed, err = %d\n", err);
        goto exit;        
    }
exit:
    return err;    
}

DWORD
ReadSwitch(
    __in LPCSTR SwitchArg,
    __out char * Switch
    )
{
    DWORD retval = ERROR_INVALID_PARAMETER;
    
    size_t len = 0;
    if (FAILED(StringCchLength(SwitchArg, SWITCH_ARG_LEN+1, &len)) ||
        len != SWITCH_ARG_LEN)
    {
        printf("Invalid arg %s, expected arg length: %d, actual length: %d\n", SwitchArg, SWITCH_ARG_LEN, len);
        goto exit;
    }

    if (SwitchArg[0] != L'/' && SwitchArg[0] != L'-')
    {
        printf("Invalid first character in arg %s\n", SwitchArg);
        goto exit;        
    }

    retval = ERROR_SUCCESS;
    *Switch = SwitchArg[1];
exit:
    return retval;
}

BOOL
AdjustProcessPrivileges(
    )
{
    HANDLE procToken;
    LUID luid;
    TOKEN_PRIVILEGES tp;
    BOOL bRetVal;
    DWORD err;

    bRetVal = OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &procToken);
    if (!bRetVal)
    {
        err = GetLastError();
        printf("OpenProcessToken failed, err = %d\n", err);
        goto exit;
    }
        
    bRetVal = LookupPrivilegeValue(NULL, SE_LOAD_DRIVER_NAME, &luid);
    if (!bRetVal)
    {
        err = GetLastError();
        printf("LookupPrivilegeValue failed, err = %d\n", err);
        goto exit1;
    }

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    //
    // AdjustTokenPrivileges can succeed even when privileges are not adjusted.
    // In such case GetLastError returns ERROR_NOT_ALL_ASSIGNED.
    //
    // Hence we check for GetLastError in both success and failure case.
    //

    (void) AdjustTokenPrivileges(procToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES) NULL, (PDWORD)NULL);
    err = GetLastError();

    if (err != ERROR_SUCCESS)
    {
        bRetVal = FALSE;
        printf("AdjustTokenPrivileges failed, err = %d\n", err);
        goto exit1;
    }

exit1:
    CloseHandle(procToken);
exit:
    return bRetVal;
}

