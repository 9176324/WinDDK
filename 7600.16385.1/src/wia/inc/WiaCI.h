/*******************************************************************************
*
*  (C) COPYRIGHT MICROSOFT CORP., 2000-2003
*
*
*  PUBLISHED TITLE : WiaCI.h
*
*  VERSION:     1.0
*
*  AUTHOR:      KeisukeT
*
*  DATE:        11 Mar, 2003
*
*  DESCRIPTION:
*  Header file for class installer exports.
*
*******************************************************************************/




#ifndef _DEVMGR_H_
#define _DEVMGR_H_


//
// Include
//
#include <objbase.h>

//
// Define
//

#define WIA_DEVSEARCH_DRVKEY        0x00000001
#define WIA_DEVSEARCH_DEVICEDATA    0x00000002


#define MAX_FRIENDLYNAME        64
#define MAX_DEVICE_ID           64

//
// Struct
//

typedef struct _WIADEVICEINSTALL {
 IN     DWORD dwSize;                           // Size of the structure.
 IN     DWORD dwFlags;                          // Reserved, must be 0.
 IN     WCHAR szInfPath[MAX_PATH];              // Full path to the INF file.
 IN     WCHAR szPnPID[MAX_PATH];                // PnP ID string for INF install.
 IN     WCHAR szIhvID[MAX_PATH];                // IHV unique ID, will be in DeviceData.
 IN OUT WCHAR szFriendlyName[MAX_FRIENDLYNAME]; // Specify name, result will be stored too.
    OUT WCHAR szWiaDeviceID[MAX_DEVICE_ID];     // WIA Device ID upon successful install.
 } WIADEVICEINSTALL, *PWIADEVICEINSTALL;


//
// Prototype
//

DWORD
WINAPI
InstallWiaDevice(
    __in PWIADEVICEINSTALL pWiaDeviceInstall
    );

DWORD
WINAPI
UninstallWiaDevice(
    __in HANDLE  hWiaDeviceList,
         DWORD   dwIndex
    );

DWORD
WINAPI
CreateWiaDeviceList(
                                       DWORD   dwFlags,    
    __in_opt                           LPCWSTR szQueryEntry,
    __in_bcount(dwQueryParameterSize)  PVOID   pvQueryParameter,
                                       DWORD   dwQueryParameterSize,
    __out_opt                          HANDLE  *phWiaDeviceList
    );

DWORD
WINAPI
DestroyWiaDeviceList(
    __in HANDLE  hWiaDeviceList
    );

DWORD
WINAPI
GetWiaDeviceProperty(
    __in            HANDLE  hWiaDeviceList,
                    DWORD   dwIndex,
                    DWORD   dwFlags,
    __in_opt        LPCWSTR szEntry,
    __out_opt       LPDWORD pdwType,
    __out_opt       PVOID   pvBuffer,
    __inout_opt     LPDWORD pdwBufferSize
    );

DWORD
WINAPI
SetWiaDeviceProperty(
    __in HANDLE  hWiaDeviceList,
    DWORD   dwIndex,
    DWORD   dwFlags,
    __in_opt LPCWSTR szEntry,
    DWORD   dwType,
    __in_bcount(dwBufferSize) PVOID   pvBuffer,
    DWORD   dwBufferSize
    );

DWORD
WINAPI
EnableWiaDevice(
    __in HANDLE  hWiaDeviceList,
         DWORD   dwIndex
    );


DWORD
WINAPI
DisableWiaDevice(
    __in HANDLE  hWiaDeviceList,
         DWORD   dwIndex
    );


#endif // _DEVMGR_H_





