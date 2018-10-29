/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    dllmain.cpp

Abstract:

    The module contains the routines to handle the loading and unloading of the
    notify object dll and the Wdf Coinstaller library.

--*/
          
#include "Common.hpp"
#include <DriverSpecs.h>
__user_code  
#include <strsafe.h>
#include "ProtNotify_i.c"


CComModule _Module;
#define MAX_VERSION_SIZE 6

#define CONVERT_VERSION_TO_STRING_WORKER(major, minor)  L#major L#minor
#define CONVERT_VERSION_TO_STRING(major, minor)         CONVERT_VERSION_TO_STRING_WORKER(major, minor)


WCHAR G_coInstallerVersion[MAX_VERSION_SIZE] = {0};

HMODULE                  CoinstallerLibrary      = NULL;
PFN_WDFPREDEVICEINSTALL  pfnWdfPreDeviceInstall  = NULL;
PFN_WDFPOSTDEVICEINSTALL pfnWdfPostDeviceInstall = NULL;
PFN_WDFPREDEVICEREMOVE   pfnWdfPreDeviceRemove   = NULL;
PFN_WDFPOSTDEVICEREMOVE  pfnWdfPostDeviceRemove  = NULL;

//
// Private methods.
//
HMODULE
LoadWdfCoInstaller(
    );

VOID
UnloadWdfCoInstaller(
    HMODULE Library
    );


extern "C"
BOOL
WINAPI
DllMain(
    __in HINSTANCE Instance,
    __in DWORD Reason,
    __in LPVOID Reserved
    )
{
    UNREFERENCED_PARAMETER(Reserved);

    if (Reason == DLL_PROCESS_ATTACH) {
        //
        // Initialize the COM Server module with the object map.
	// Do this prior to loading the coinstaller as the detach 
        // assumes the Module is initialized.
        //
        _Module.Init(ObjectMap, Instance);
        DisableThreadLibraryCalls(Instance);

        //
        // Load the Wdf Coinstaller library.
        //
        CoinstallerLibrary = LoadWdfCoInstaller();
        if (CoinstallerLibrary == NULL) {
            return FALSE;
        }

    }
    else if (Reason == DLL_PROCESS_DETACH) {

        //
        // Relesae the COM Server.
        //
        _Module.Term();

        //
        // Unload the Wdf Coinstaller library.
        //
        UnloadWdfCoInstaller(CoinstallerLibrary);
    }

    return TRUE;
}


STDAPI DllCanUnloadNow(
    )
{
    //
    // Determine whether the DLL can be unloaded by OLE
    //
    return (_Module.GetLockCount() == 0) ? S_OK : S_FALSE;
}


STDAPI DllGetClassObject(
    REFCLSID ClassId,
    REFIID InterfaceId,
    LPVOID* InterfacePtr
    )
{
    //
    // Return a class factory to create an object of the requested type
    //
    return _Module.GetClassObject(ClassId, InterfaceId, InterfacePtr);
}


STDAPI DllRegisterServer(
    )
{

    //
    // Register object, typelib and all interfaces in typelib
    //
    return _Module.RegisterServer(TRUE);
}


STDAPI DllUnregisterServer(
    )
{
    //
    // Remove entries from the system registry
    //
    _Module.UnregisterServer();
    return S_OK;
}

PWCHAR
GetCoinstallerVersion(
    VOID
    )
{
    if (FAILED( StringCchCopy(G_coInstallerVersion,
                              MAX_VERSION_SIZE,
                              CONVERT_VERSION_TO_STRING(KMDF_MAJOR_VERSION_STRING, KMDF_MINOR_VERSION_STRING)) ))
    {
        printf("StringCchCopy failed with error \n");
    }

    return (PWCHAR)&G_coInstallerVersion;
}

HMODULE
LoadWdfCoInstaller(
    )
{
   
    #pragma prefast(suppress:6262, "Supprress overflow warnings")        
    HRESULT hr = S_OK;
    HMODULE library = NULL;
    WCHAR coinstaller[MAX_PATH] = {0};
    WCHAR   coinstallerName[MAX_PATH/2];
    PWCHAR  coinstallerVersion;

    //
    // Construct the full file name for the Wdf Coinstaller dll.
    //
    if (GetSystemDirectory(coinstaller, MAX_PATH) == 0) {
        hr = GetLastError();
        goto exit;
    }
    coinstallerVersion = GetCoinstallerVersion();
    if (FAILED( StringCchPrintf(coinstallerName,
                              MAX_PATH/2,
                              L"\\WdfCoInstaller%s.dll",
                              coinstallerVersion) )) {
        goto exit;
    }

    hr = StringCchCat(coinstaller, MAX_PATH, coinstallerName);
    if (FAILED(hr)) {
        goto exit;
    }

    //
    // Load the Wdf Coinstaller library.
    //
#pragma prefast(suppress:28160, "Suppressing false positive from PFD")                
    library = LoadLibrary(coinstaller);
    if (library == NULL) {
        hr = GetLastError();
        goto exit;
    }

    pfnWdfPreDeviceInstall = (PFN_WDFPREDEVICEINSTALL) GetProcAddress(library, "WdfPreDeviceInstall");
    if (pfnWdfPreDeviceInstall == NULL) {
        hr = GetLastError();
        goto exit;
    }

    pfnWdfPostDeviceInstall = (PFN_WDFPOSTDEVICEINSTALL) GetProcAddress(library, "WdfPostDeviceInstall");
    if (pfnWdfPostDeviceInstall == NULL) {
        hr = GetLastError();
        goto exit;
    }

    pfnWdfPreDeviceRemove = (PFN_WDFPREDEVICEREMOVE) GetProcAddress(library, "WdfPreDeviceRemove");
    if (pfnWdfPreDeviceRemove == NULL) {
        hr = GetLastError();
        goto exit;
    }

    pfnWdfPostDeviceRemove = (PFN_WDFPREDEVICEREMOVE) GetProcAddress(library, "WdfPostDeviceRemove");
    if (pfnWdfPostDeviceRemove == NULL) {
        hr = GetLastError();
        goto exit;
    }

exit:

    //
    // Unload the Wdf Coinstaller library in case of an error.
    //
    if (FAILED(hr)) {
        UnloadWdfCoInstaller(library);
        library = NULL;
    }

    SetLastError(hr);
    return library;
}


VOID
UnloadWdfCoInstaller(
    HMODULE Library
    )
{
    if (Library) {
        FreeLibrary( Library );
    }

    pfnWdfPreDeviceInstall  = NULL;
    pfnWdfPostDeviceInstall = NULL;
    pfnWdfPreDeviceRemove   = NULL;
    pfnWdfPostDeviceRemove  = NULL;
}

