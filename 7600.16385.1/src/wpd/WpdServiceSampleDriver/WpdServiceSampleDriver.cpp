#include "stdafx.h"
#include "resource.h"
#include "WpdServiceSampleDriver.h"

#include "WpdServiceSampleDriver.tmh"

HINSTANCE g_hInstance = NULL;

#define DECLARE_PROPERTYKEY(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8, pid) \
        EXTERN_C const PROPERTYKEY name \
                = { { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }, pid }

class CWpdServiceSampleDriverModule : public CAtlDllModuleT< CWpdServiceSampleDriverModule >
{
public :
    DECLARE_REGISTRY_APPID_RESOURCEID(IDR_WpdServiceSampleDriver, "{95B558CB-F6B1-4B37-A105-3B7B6A196FB5}")
    DECLARE_LIBID(LIBID_WpdServiceSampleDriverLib)
};

CWpdServiceSampleDriverModule _AtlModule;

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    if(dwReason == DLL_PROCESS_ATTACH)
    {
        g_hInstance = hInstance;

        // Initialize tracing.
        WPP_INIT_TRACING(MYDRIVER_TRACING_ID);
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
        // Cleanup tracing.
        WPP_CLEANUP();
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

