//+--------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  This source code is intended only as a supplement to Microsoft
//  Development Tools and/or on-line documentation.  See these other
//  materials for detailed information regarding Microsoft code samples.
//
//  THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
//
//  Abstract:
//     WDK print filter sample. This file contains the implementation for
//     DllGetClassObject and DllMain
//
//----------------------------------------------------------------------------

#include "precomp.hxx"
#include "main.hxx"
#include "XpsFilter.hxx"
#include "StreamFilter.hxx"
#include "main.tmh"

__user_driver

LONG DllLockManager::m_cGlobalRef = 0;

HRESULT
DllCanUnloadNow(void) throw()
{
    return 0 == DllLockManager::GetGlobalRef()  ? S_OK : S_FALSE;
}

HRESULT
DllGetClassObject(
    __in    REFCLSID    rclsid,
    __in    REFIID      riid,
    __out   PVOID       *ppv
    ) throw()
{
    HRESULT hRetval = E_POINTER;

    if (ppv)
    {
        *ppv = NULL;

        if (rclsid == XpsFilter::FilterClsid())
        {
            Tools::SmartPtr<XpsFilter>   pFilter;

            pFilter.Attach(new XpsFilter());

            if (pFilter)
            {
                hRetval = pFilter->QueryInterface(riid, ppv);
            }
            else
            {
                hRetval = E_OUTOFMEMORY;
            }
        }
        else if (rclsid == StreamFilter::FilterClsid())
        {
            Tools::SmartPtr<StreamFilter>   pFilter;

            pFilter.Attach(new StreamFilter());

            if (pFilter)
            {
                hRetval = pFilter->QueryInterface(riid, ppv);
            }
            else
            {
                hRetval = E_OUTOFMEMORY;
            }
        }
        else
        {
            hRetval = CLASS_E_CLASSNOTAVAILABLE;
        }
    }

    return hRetval;
}

BOOL WINAPI
DllMain(
    __in        HINSTANCE    hInst,
    __in        DWORD        dwReason,
    __in_opt    PVOID        pReserved
    )
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hInst);

        WPP_INIT_TRACING(L"WdkPipelineFilter");
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
        WPP_CLEANUP();
    }

    UNREFERENCED_PARAMETER(dwReason);
    UNREFERENCED_PARAMETER(pReserved);

    return TRUE;
}

void
WppTraceDebugOut(
    __in    const WCHAR *pszFmt,
    ...
    )
{
    va_list pArgs;

    va_start(pArgs, pszFmt);

    WCHAR buf[MAX_PATH];

    if (SUCCEEDED(StringCchVPrintf(buf,
                                   MAX_PATH,
                                   pszFmt,
                                   pArgs)) &&
        SUCCEEDED(StringCchCat(buf,
                               MAX_PATH,
                               L"\n")))
    {
        OutputDebugString(buf);
    }

    va_end(pArgs);
}

void
WppTraceDebugOut(
    __in    const CHAR *pszFmt,
    ...
    )
{
    va_list pArgs;

    va_start(pArgs, pszFmt);

    CHAR buf[MAX_PATH];

    if (SUCCEEDED(StringCchVPrintfA(buf,
                                    MAX_PATH,
                                    pszFmt,
                                    pArgs)) &&
        SUCCEEDED(StringCchCatA(buf,
                                MAX_PATH,
                                "\n")))
    {
        OutputDebugStringA(buf);
    }

    va_end(pArgs);
}










