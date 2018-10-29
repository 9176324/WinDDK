/*++BUILD Version 0000

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    SoftUSBAudio.cpp

Abstract:
    Definies entry point for the DLL.

--*/

#include "stdafx.h"
#include "resource.h"
#include <dsfif.h>
#include <USBProtocolDefs.h>
#include <softusbif.h>
#include "AudioDevice.h"
#include "SoftUSBAudio.h"

class CSoftUSBAudioModule : public CAtlDllModuleT<CSoftUSBAudioModule>
{
    public:
        DECLARE_LIBID(LIBID_SoftUSBAudioLib)
        DECLARE_REGISTRY_APPID_RESOURCEID(IDR_SOFTUSBAUDIO, "{71BDF034-E64A-430C-A068-48341791141E}")
};

CSoftUSBAudioModule _AtlModule;

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
    HRESULT hr = _AtlModule.DllRegisterServer();
    return hr;
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
    HRESULT hr = _AtlModule.DllUnregisterServer();
    return hr;
}





