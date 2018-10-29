// Implementation of DLL Exports.

#include "stdafx.h"
#include "resource.h"
#include "WpdWudfSampleDriver.h"

HINSTANCE g_hInstance = NULL;

class CWpdWudfSampleDriverModule : public CAtlDllModuleT< CWpdWudfSampleDriverModule >
{
public :
    DECLARE_LIBID(LIBID_WpdWudfSampleDriverLib)
    DECLARE_REGISTRY_APPID_RESOURCEID(IDR_WpdWudfSampleDriver, "{9FF28171-F2BA-4720-AAE1-92DA54E8BB0E}")
};

CWpdWudfSampleDriverModule _AtlModule;


// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    hInstance;

    if(dwReason == DLL_PROCESS_ATTACH)
    {
        g_hInstance = hInstance;
    }

    return _AtlModule.DllMain(dwReason, lpReserved);
}


// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void)
{
    return _AtlModule.DllCanUnloadNow();
}


// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(__in REFCLSID rclsid, __in REFIID riid, __deref_out LPVOID* ppv)
{
    return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}


// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
    HRESULT hr = _AtlModule.DllRegisterServer();
    return hr;
}


// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
    HRESULT hr = _AtlModule.DllUnregisterServer();
    return hr;
}

