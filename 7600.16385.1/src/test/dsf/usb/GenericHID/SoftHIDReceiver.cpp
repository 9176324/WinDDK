/*++BUILD Version 0000

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    SoftHIDReceiver.cpp

Abstract:
    Definies entry point for the DLL.

--*/

#include "stdafx.h"
#include "resource.h"
#include <dsfif.h>
#include <USBProtocolDefs.h>
#include <softusbif.h>
#include "HIDDevice.h"
#include "SoftHIDReceiver.h"

class CSoftHIDReceiverModule : public CAtlDllModuleT<CSoftHIDReceiverModule>
{
    public: 
        DECLARE_LIBID(LIBID_SoftHIDReceiver)
        DECLARE_REGISTRY_APPID_RESOURCEID(IDR_SOFTHIDRECEIVER, "{68B3DBD9-23A4-4a60-8E27-864A79C77460}")
};

CSoftHIDReceiverModule _AtlModule;

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





