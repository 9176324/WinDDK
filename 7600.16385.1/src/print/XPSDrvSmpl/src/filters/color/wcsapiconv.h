/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   wcsapiconv.h

Abstract:

   Provides a wrapper to the WCS API's

--*/

#pragma once

class CWCSApiConv;
extern CWCSApiConv g_WCSApiConv;

typedef BOOL (CALLBACK* WCSASSOCIATECOLORPROFILEWITHDEVICE)(WCS_PROFILE_MANAGEMENT_SCOPE, PCWSTR, PCWSTR);
typedef BOOL (CALLBACK* WCSDISASSOCIATECOLORPROFILEFROMDEVICE)(WCS_PROFILE_MANAGEMENT_SCOPE, PCWSTR, PCWSTR);
typedef BOOL (CALLBACK* WCSENUMCOLORPROFILESSIZE)(WCS_PROFILE_MANAGEMENT_SCOPE, PENUMTYPEW, PDWORD);
typedef BOOL (CALLBACK* WCSGETDEFAULTCOLORPROFILESIZE)(WCS_PROFILE_MANAGEMENT_SCOPE, PCWSTR, COLORPROFILETYPE, COLORPROFILESUBTYPE, DWORD, PDWORD);
typedef BOOL (CALLBACK* WCSGETDEFAULTCOLORPROFILE)(WCS_PROFILE_MANAGEMENT_SCOPE, PCWSTR, COLORPROFILETYPE, COLORPROFILESUBTYPE, DWORD, DWORD, LPWSTR);
typedef BOOL (CALLBACK* WCSSETDEFAULTCOLORPROFILE)(WCS_PROFILE_MANAGEMENT_SCOPE, PCWSTR, COLORPROFILETYPE, COLORPROFILESUBTYPE, DWORD, LPCWSTR);
typedef BOOL (CALLBACK* WCSSETDEFAULTRENDERINGINTENT)(WCS_PROFILE_MANAGEMENT_SCOPE, DWORD);
typedef BOOL (CALLBACK* WCSGETUSEPERUSERPROFILES)(LPCWSTR, DWORD, PBOOL);
typedef BOOL (CALLBACK* WCSSETUSEPERUSERPROFILES)(LPCWSTR, DWORD, BOOL);
typedef BOOL (CALLBACK* WCSTRANSLATECOLORS)(HTRANSFORM, DWORD, DWORD, COLORDATATYPE, DWORD, PVOID, DWORD, COLORDATATYPE, DWORD, PVOID);
typedef BOOL (CALLBACK* WCSCHECKCOLORS)(HTRANSFORM, DWORD, DWORD, COLORDATATYPE, DWORD, PVOID, PBYTE);
typedef HPROFILE (CALLBACK* WCSOPENCOLORPROFILE)(PPROFILE, PPROFILE, PPROFILE, DWORD, DWORD, DWORD, DWORD);
typedef HPROFILE (CALLBACK* WCSCREATEICCPROFILE)(HPROFILE, DWORD);

#define WcsAssociateColorProfileWithDeviceXD    g_WCSApiConv.WcsAssociateColorProfileWithDevice
#define WcsDisassociateColorProfileFromDeviceXD g_WCSApiConv.WcsDisassociateColorProfileFromDevice
#define WcsEnumColorProfilesSizeXD              g_WCSApiConv.WcsEnumColorProfilesSize
#define WcsGetDefaultColorProfileSizeXD         g_WCSApiConv.WcsGetDefaultColorProfileSize
#define WcsGetDefaultColorProfileXD             g_WCSApiConv.WcsGetDefaultColorProfile
#define WcsSetDefaultColorProfileXD             g_WCSApiConv.WcsSetDefaultColorProfile
#define WcsSetDefaultRenderingIntentXD          g_WCSApiConv.WcsSetDefaultRenderingIntent
#define WcsGetUsePerUserProfilesXD              g_WCSApiConv.WcsGetUsePerUserProfiles
#define WcsSetUsePerUserProfilesXD              g_WCSApiConv.WcsSetUsePerUserProfiles
#define WcsTranslateColorsXD                    g_WCSApiConv.WcsTranslateColors
#define WCSCheckColorsXD                        g_WCSApiConv.WCSCheckColors
#define WcsOpenColorProfileWXD                  g_WCSApiConv.WcsOpenColorProfileW
#define WcsOpenColorProfileAXD                  g_WCSApiConv.WcsOpenColorProfileA
#define WcsCreateIccProfileXD                   g_WCSApiConv.WcsCreateIccProfile

#ifdef _UNICODE
#define WcsOpenColorProfileXD WcsOpenColorProfileWXD
#else
#define WcsOpenColorProfileXD WcsOpenColorProfileAXD
#endif

template <typename _T>
class CEncodedFuncPtr
{
public:
    CEncodedFuncPtr() :
        m_pFunc(NULL)
    {
    }

    ~CEncodedFuncPtr(){}

    CEncodedFuncPtr<_T>&
    operator=(
         __in FARPROC pFunc
         )
    {
        m_pFunc = EncodePointer(pFunc);
        return *this;
    }

    BOOL
    operator==(
        __in_opt PVOID pv
        ) const
    {
        return m_pFunc == pv;
    }

    BOOL
    operator!=(
        __in_opt PVOID pv
        ) const
    {
        return !operator==(pv);
    }

    _T
    GetFunc(
        VOID
        )
    {
        return reinterpret_cast<_T>(DecodePointer(m_pFunc));
    }

private:
    __field_encoded_pointer PVOID m_pFunc;
};

class CWCSApiConv
{
public:
    CWCSApiConv();

    ~CWCSApiConv();

    BOOL
    WcsAssociateColorProfileWithDevice(
        __in WCS_PROFILE_MANAGEMENT_SCOPE scope,
        __in PCWSTR pProfileName,
        __in PCWSTR pDeviceName
        );

    BOOL
    WcsDisassociateColorProfileFromDevice(
        __in WCS_PROFILE_MANAGEMENT_SCOPE scope,
        __in PCWSTR pProfileName,
        __in PCWSTR pDeviceName
        );

    BOOL
    WcsEnumColorProfilesSize(
        __in WCS_PROFILE_MANAGEMENT_SCOPE scope,
        __in PENUMTYPEW pEnumRecord,
        __out PDWORD pdwSize
        );

    BOOL
    WcsGetDefaultColorProfileSize(
        __in WCS_PROFILE_MANAGEMENT_SCOPE scope,
        __in_opt PCWSTR pDeviceName,
        __in COLORPROFILETYPE cptColorProfileType,
        __in COLORPROFILESUBTYPE cpstColorProfileSubType,
        __in DWORD dwProfileID,
        __out PDWORD pcbProfileName
        );

    BOOL
    WcsGetDefaultColorProfile(
        __in WCS_PROFILE_MANAGEMENT_SCOPE scope,
        __in_opt PCWSTR pDeviceName,
        __in COLORPROFILETYPE cptColorProfileType,
        __in COLORPROFILESUBTYPE cpstColorProfileSubType,
        __in DWORD dwProfileID,
        __in DWORD cbProfileName,
        __out_bcount(cbProfileName) LPWSTR pProfileName
        );

    BOOL
    WcsSetDefaultColorProfile(
        __in WCS_PROFILE_MANAGEMENT_SCOPE scope,
        __in_opt PCWSTR pDeviceName,
        __in COLORPROFILETYPE cptColorProfileType,
        __in COLORPROFILESUBTYPE cpstColorProfileSubType,
        __in DWORD dwProfileID,
        __in_opt LPCWSTR pProfileName
        );

    BOOL
    WcsSetDefaultRenderingIntent(
        __in WCS_PROFILE_MANAGEMENT_SCOPE scope,
        __in DWORD dwRenderingIntent
        );

    BOOL
    WcsGetUsePerUserProfiles(
        __in LPCWSTR pDeviceName,
        __in DWORD dwDeviceClass,
        __out PBOOL pUsePerUserProfiles
        );

    BOOL
    WcsSetUsePerUserProfiles(
        __in LPCWSTR pDeviceName,
        __in DWORD dwDeviceClass,
        __in BOOL usePerUserProfiles
        );

    BOOL
    WcsTranslateColors(
        __in HTRANSFORM hColorTransform,
        __in DWORD nColors,
        __in DWORD nInputChannels,
        __in COLORDATATYPE cdtInput,
        __in DWORD cbInput,
        __in_bcount(cbInput) PVOID pInputData,
        __in DWORD nOutputChannels,
        __in COLORDATATYPE cdtOutput,
        __in DWORD cbOutput,
        __out_bcount(cbOutput)PVOID pOutputData
        );

    BOOL
    WCSCheckColors(
        __in HTRANSFORM hColorTransform,
        __in DWORD nColors,
        __in DWORD nInputChannels,
        __in COLORDATATYPE cdtInput,
        __in DWORD cbInput,
        __in_bcount(cbInput) PVOID pInputData,
        __out_bcount(nColors)PBYTE pResult
    );

    HPROFILE WINAPI
    WcsOpenColorProfileA(
        __in        PPROFILE pCDMPProfile,
        __in_opt    PPROFILE pCAMPProfile,
        __in_opt    PPROFILE pGMMPProfile,
        __in        DWORD    dwDesireAccess,
        __in        DWORD    dwShareMode,
        __in        DWORD    dwCreationMode,
        __in        DWORD    dwFlags
        );

    HPROFILE WINAPI
    WcsOpenColorProfileW(
        __in        PPROFILE pCDMPProfile,
        __in_opt    PPROFILE pCAMPProfile,
        __in_opt    PPROFILE pGMMPProfile,
        __in        DWORD    dwDesireAccess,
        __in        DWORD    dwShareMode,
        __in        DWORD    dwCreationMode,
        __in        DWORD    dwFlags
        );

    HPROFILE
    WcsCreateIccProfile(
        __in HPROFILE hWcsProfile,
        __in DWORD dwOptions
        );

private:
    HINSTANCE m_dllHandle;

    //
    // WCS API
    //
    CEncodedFuncPtr<WCSASSOCIATECOLORPROFILEWITHDEVICE>    m_WcsAssociateColorProfileWithDevice;

    CEncodedFuncPtr<WCSDISASSOCIATECOLORPROFILEFROMDEVICE> m_WcsDisassociateColorProfileFromDevice;

    CEncodedFuncPtr<WCSENUMCOLORPROFILESSIZE>              m_WcsEnumColorProfilesSize;

    CEncodedFuncPtr<WCSGETDEFAULTCOLORPROFILESIZE>         m_WcsGetDefaultColorProfileSize;

    CEncodedFuncPtr<WCSGETDEFAULTCOLORPROFILE>             m_WcsGetDefaultColorProfile;

    CEncodedFuncPtr<WCSSETDEFAULTCOLORPROFILE>             m_WcsSetDefaultColorProfile;

    CEncodedFuncPtr<WCSSETDEFAULTRENDERINGINTENT>          m_WcsSetDefaultRenderingIntent;

    CEncodedFuncPtr<WCSGETUSEPERUSERPROFILES>              m_WcsGetUsePerUserProfiles;

    CEncodedFuncPtr<WCSSETUSEPERUSERPROFILES>              m_WcsSetUsePerUserProfiles;

    CEncodedFuncPtr<WCSTRANSLATECOLORS>                    m_WcsTranslateColors;

    CEncodedFuncPtr<WCSCHECKCOLORS>                        m_WcsCheckColors;

    CEncodedFuncPtr<WCSOPENCOLORPROFILE>                   m_WcsOpenColorProfileA;

    CEncodedFuncPtr<WCSOPENCOLORPROFILE>                   m_WcsOpenColorProfileW;

    CEncodedFuncPtr<WCSCREATEICCPROFILE>                   m_WcsCreateIccProfile;
};


