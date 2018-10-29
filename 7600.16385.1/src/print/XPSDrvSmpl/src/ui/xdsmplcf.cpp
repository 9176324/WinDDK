/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   xdsmplcf.cpp

Abstract:

   XPSDrv feature sample class factory implementation.

--*/

#include "precomp.h"
#include "debug.h"
#include "globals.h"
#include "xdexcept.h"
#include "xdstring.h"
#include "xdsmplcf.h"
#include "xdsmplui.h"
#include "xdsmplptprov.h"

/*++

Routine Name:

    CXDSmplUICF::CreateInstance

Routine Description:

    Creates an object of the specified CLSID and retrieves an interface pointer to this object.
    The supported class objects are currently IPrintOemUI and IPrintOemPrintTicketProvider.

Arguments:

    pUnkOuter - This must be NULL, as aggregare object creation is not supported.
    riid - The IID of the requested interface.
    ppvObject - A pointer to the interface pointer identified by riid.
                If the object does not support this interface, ppvObj is set to NULL.

Return Value:

    HRESULT
    S_OK - On success
    E_*  - On error

--*/
HRESULT
CXDSmplUICF::CreateInstance(
    __in_opt    LPUNKNOWN pUnkOuter,
    __in        REFIID    riid,
    __deref_out PVOID*    ppvObject
    )
{
    HRESULT hr = S_OK;

    if (ppvObject == NULL)
    {
        hr = E_POINTER;
        goto Exit;
    }
    *ppvObject = NULL;

    if (pUnkOuter == NULL)
    {
        //
        // Create UI component
        //
        CXDSmplUI* pXDSmplUI = NULL;

        try
        {
            pXDSmplUI = new CXDSmplUI;
            hr = CHECK_POINTER(pXDSmplUI, E_OUTOFMEMORY);
        }
        catch (CXDException& e)
        {
            hr = e;
        }

        if (SUCCEEDED(hr))
        {
            //
            // Get the requested interface
            //
            hr = pXDSmplUI->QueryInterface(riid, ppvObject) ;

            //
            // Release the IUnknown pointer. If QueryInterface failed
            // the Release() call will clean up
            //
            pXDSmplUI->Release();

            //
            // If QueryInterface failed this could be a request for the
            // PTProvider Interface
            //
            if (FAILED(hr))
            {
                CXDSmplPTProvider* pXDSmplPT = NULL;

                try
                {
                    pXDSmplPT = new CXDSmplPTProvider;
                    hr = CHECK_POINTER(pXDSmplPT, E_OUTOFMEMORY);
                }
                catch (CXDException& e)
                {
                    __analysis_assume((*ppvObject) == NULL);
                    hr = e;
                }

                if (SUCCEEDED(hr))
                {
                    hr = pXDSmplPT->QueryInterface(riid, ppvObject) ;

                    //
                    // Release the IUnknown pointer.
                    // (If QueryInterface failed, component will delete itself.)
                    //
                    pXDSmplPT->Release();
                }
            }
        }
    }
    else
    {
        //
        // Cannot aggregate
        //
        hr = CLASS_E_NOAGGREGATION;
    }

Exit:
    ERR_ON_HR(hr);
    return hr;
}

/*++

Routine Name:

    CXDSmplUICF::LockServer

Routine Description:

    Increments and decrements the UI Plug-in Module lock count.

Arguments:

    bLock - If TRUE, the lock count is incremented; otherwise, the lock count is decremented.

Return Value:

    HRESULT
    S_OK - On success
    E_*  - On error

--*/
HRESULT
CXDSmplUICF::LockServer(
    __in BOOL bLock
    )
{
    if (bLock)
    {
        InterlockedIncrement(&g_cServerLocks);
    }
    else
    {
        InterlockedDecrement(&g_cServerLocks);
    }

    return S_OK;
}


