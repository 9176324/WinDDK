/*++ BUILD Version: 0000    

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    SoftHIDDevice.cpp

Abstract:
    Implementation of DLL Exports for the executable
 
Revision History:

--*/ 
#include "stdafx.h"
#include <dsfif.h>
#include <SoftHidUsbIf.h>
#include <InputKbd.h>
#include "SoftHidDevice.h"
#include "MessageTask.h"
#include "SoftHID.h"
#include "SoftHIDInputKbdMapper.h"

class CSoftHIDDeviceModule : public CAtlDllModuleT<CSoftHIDDeviceModule>
{
    public:
        DECLARE_LIBID(LIBID_SOFTHID)
        DECLARE_REGISTRY_APPID_RESOURCEID(IDR_SOFTHIDDEVICE, "{CC1B9A6C-C841-446D-B296-0068695CA747}")
};

CSoftHIDDeviceModule _AtlModule;

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





