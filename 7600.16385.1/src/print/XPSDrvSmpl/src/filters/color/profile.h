/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   profile.h

Abstract:

   Profile class definition. The profile class is responsible for loading and maintaining
   colo profile resources. The class provides methods for loading profiles from filename or
   URI and is responsible for limited caching based off the file name and the (WCS)OpenProfile
   options.

--*/

#pragma once

class CProfile
{
public:
    CProfile();

    ~CProfile();

    HRESULT
    SetProfile(
        __in IFixedPage* pFP,
        __in LPCWSTR     szURI
        );

    HRESULT
    SetProfile(
        __in LPCWSTR szURI
        );

    HRESULT
    SetProfile(
        __in                  LPWSTR szProfile,
        __in_bcount(cbBuffer) PBYTE  pBuffer,
        __in                  UINT   cbBuffer
        );

    HRESULT
    GetProfileHandle(
        __out HPROFILE* phProfile
        );

    HRESULT
    IsWCSCompatible(
        __out BOOL* pbWCSCompat
        );

    HRESULT
    GetProfileKey(
        __out CStringXDW* pcstrProfileURI
        );

    BOOL
    operator==(
        __in CONST CStringXDW& cstrProfileKey
        ) CONST;

    BOOL
    operator!=(
        __in CONST CStringXDW& cstrProfileKey
        ) CONST;

private:
    HRESULT
    CreateProfileBuffer(
        VOID
        );

    VOID
    FreeProfile(
        VOID
        );

    HRESULT
    OpenProfile(
        __in  PROFILE*  pProfile,
        __out HPROFILE* phProfile
        );

    HRESULT
    CreateProfileKey(
        __in     LPCWSTR pszCDMP,
        __in_opt LPCWSTR pszCAMP,
        __in_opt LPCWSTR pszGMMP,
        __out    CStringXDW* pcstrKey
        );

private:
    HPROFILE   m_hProfile;

    DWORD      m_dwDesiredAccess;

    DWORD      m_dwShareMode;

    DWORD      m_dwCreationMode;

    DWORD      m_dwWCSFlags;

    //
    // We define a key for a profile as the URI(s) plus any options used when
    // creating the profile. This allows anything that uses this profile in
    // combination with other profiles to identify the constituents of the
    // transform and hence cache that transform without needing to store the
    // profile handles.
    //
    // Note: Do not cache against the handle as handles can be re-used
    //
    CStringXDW m_cstrProfileKey;
};


