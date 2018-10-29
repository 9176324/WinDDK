/*++BUILD Version 0000

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    SoftUSBLoopback.cpp

Abstract:
    Definies entry point for the DLL.

--*/

#include "stdafx.h"
#include "resource.h"
#include <dsfif.h>
#include <USBProtocolDefs.h>
#include <softusbif.h>
#include "LoopbackDevice.h"
#include "SoftUSBLoopback.h"

class CSoftUSBLoopbackModule : public CAtlDllModuleT<CSoftUSBLoopbackModule>
{
    public: 
        DECLARE_LIBID(LIBID_SoftUSBLoopbackLib)
        DECLARE_REGISTRY_APPID_RESOURCEID(IDR_SOFTUSBLOOPBACK, "{D1C80253-8DB4-4F72-BF74-270A0EDA1FA9}")
};

CSoftUSBLoopbackModule _AtlModule;

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    hInstance;
    return _AtlModule.DllMain(dwReason, lpReserved);
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
{
    return (_AtlModule.DllCanUnloadNow());
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
    HRESULT hr =  _AtlModule.DllRegisterServer();
    return hr;
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
    HRESULT hr = _AtlModule.DllUnregisterServer();
    return hr;
}





