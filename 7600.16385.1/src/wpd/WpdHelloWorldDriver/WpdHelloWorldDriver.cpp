#include "stdafx.h"
#include "resource.h"
#include "WpdHelloWorldDriver.h"

HINSTANCE g_hInstance = NULL;

#define DECLARE_PROPERTYKEY(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8, pid) \
        EXTERN_C const PROPERTYKEY name \
                = { { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }, pid }

// {CDD18979-A7B0-4D5E-9EB2-0A826805CBBD}
DECLARE_PROPERTYKEY(PRIVATE_SAMPLE_DRIVER_WUDF_DEVICE_OBJECT, 0xCDD18979, 0xA7B0, 0x4D5E, 0x9E, 0xB2, 0x0A, 0x82, 0x68, 0x05, 0xCB, 0xBD, 2);
// {9BD949E5-59CF-41AE-90A9-BE1D044F578F}
DECLARE_PROPERTYKEY(PRIVATE_SAMPLE_DRIVER_WPD_SERIALIZER_OBJECT, 0x9BD949E5, 0x59CF, 0x41AE, 0x90, 0xA9, 0xBE, 0x1D, 0x04, 0x4F, 0x57, 0x8F, 2);
// {4DF6C8C7-2CE5-457C-9F53-EFCECAA95C04}
DECLARE_PROPERTYKEY(PRIVATE_SAMPLE_DRIVER_CLIENT_CONTEXT_MAP, 0x4DF6C8C7, 0x2CE5, 0x457C, 0x9F, 0x53, 0xEF, 0xCE, 0xCA, 0xA9, 0x5C, 0x04, 0);

class CWpdHelloWorldDriverModule : public CAtlDllModuleT< CWpdHelloWorldDriverModule >
{
public :
    DECLARE_REGISTRY_APPID_RESOURCEID(IDR_WpdHelloWorldDriver, "{021AD204-6411-4698-8CFB-C1A72B581733}")
    DECLARE_LIBID(LIBID_WpdHelloWorldDriverLib)
};

CWpdHelloWorldDriverModule _AtlModule;

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
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

VOID CHECK_HR(
    HRESULT hr,
    LPCSTR format,
    ...)
{
    if (format == NULL)
    {
        return;
    }

    if(hr != S_OK)
    {
        CHAR Buffer[MAX_DEBUG_STRING_LENGTH] = {0};

        va_list arglist;
        va_start(arglist, format);
        vsprintf_s(Buffer, ARRAYSIZE(Buffer), format, arglist);
        va_end(arglist);

        // output to the debugger console
        CAtlStringA strBuffer;
        strBuffer.Format("%s, hr = 0x%lx\n", Buffer, hr);

        OutputDebugStringA(strBuffer);
    }
}

