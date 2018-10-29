/*++

 Copyright (c) 2000  Microsoft Corporation

Module Name:

    simple.c

--*/

#include "simple.h"

#include <ntverp.h>

//
// globals
//
EXT_API_VERSION         ApiVersion = { (VER_PRODUCTVERSION_W >> 8), (VER_PRODUCTVERSION_W & 0xff), EXT_API_VERSION_NUMBER64, 0 };
WINDBG_EXTENSION_APIS   ExtensionApis;
ULONG SavedMajorVersion;
ULONG SavedMinorVersion;

DllInit(
    HANDLE hModule,
    DWORD  dwReason,
    DWORD  dwReserved
    )
{
    switch (dwReason) {
        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH:
            break;

        case DLL_PROCESS_DETACH:
            break;

        case DLL_PROCESS_ATTACH:
            break;
    }

    return TRUE;
}


VOID
WinDbgExtensionDllInit(
    PWINDBG_EXTENSION_APIS lpExtensionApis,
    USHORT MajorVersion,
    USHORT MinorVersion
    )
{
    ExtensionApis = *lpExtensionApis;

    SavedMajorVersion = MajorVersion;
    SavedMinorVersion = MinorVersion;

    return;
}

LPEXT_API_VERSION
ExtensionApiVersion(
    VOID
    )
{
    //
    // ExtensionApiVersion should return EXT_API_VERSION_NUMBER64 in order for APIs
    // to recognize 64 bit addresses.  KDEXT_64BIT also has to be defined before including
    // wdbgexts.h to get 64 bit headers for WINDBG_EXTENSION_APIS
    //
    return &ApiVersion;
}

//
// Routine called by debugger after load
//
VOID
CheckVersion(
    VOID
    )
{
    return;
}
