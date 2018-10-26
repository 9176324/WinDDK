/*++

Copyright (c) 1999 Microsoft Corporation

Module Name:

    newdev.h

Abstract:

    Public header file for Windows NT Driver Installation DLL.

--*/

#ifndef _INC_NEWDEV
#define _INC_NEWDEV

#include <pshpack1.h>   // Assume byte packing throughout

#ifdef __cplusplus
extern "C" {
#endif


#define INSTALLFLAG_FORCE   0x00000001      // Force the installation of the specified driver
#define INSTALLFLAG_BITS    0x00000001


BOOL
WINAPI
UpdateDriverForPlugAndPlayDevicesA(
    HWND hwndParent,
    LPCSTR HardwareId,
    LPCSTR FullInfPath,
    DWORD InstallFlags,
    PBOOL bRebootRequired OPTIONAL
    );

BOOL
WINAPI
UpdateDriverForPlugAndPlayDevicesW(
    HWND hwndParent,
    LPCWSTR HardwareId,
    LPCWSTR FullInfPath,
    DWORD InstallFlags,
    PBOOL bRebootRequired OPTIONAL
    );

#ifdef UNICODE
#define UpdateDriverForPlugAndPlayDevices UpdateDriverForPlugAndPlayDevicesW
#else
#define UpdateDriverForPlugAndPlayDevices UpdateDriverForPlugAndPlayDevicesA
#endif



#ifdef __cplusplus
}
#endif

#include <poppack.h>

#endif // _INC_NEWDEV


