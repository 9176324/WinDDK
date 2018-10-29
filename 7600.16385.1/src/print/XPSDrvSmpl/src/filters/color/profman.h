/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   profman.h

Abstract:

   Color profile manager definition. The CProfileManager class is responsible
   for managing the color profile set in the driver.

--*/

#pragma once

#include "cmprofiledata.h"
#include "cmintentsdata.h"
#include "rescache.h"
#include "transform.h"

class CProfileManager : public IResWriter
{
public:
    CProfileManager(
        __in LPCWSTR                                                                                 pszDeviceName,
        __in XDPrintSchema::PageSourceColorProfile::PageSourceColorProfileData cmProfData,
        __in XDPrintSchema::PageICMRenderingIntent::PageICMRenderingIntentData                       cmIntData,
        __in IFixedPage*                                                                             pFP
        );

    virtual ~CProfileManager();

    HRESULT
    GetColorTransform(
        __out HTRANSFORM* phColorTrans,
        __out BOOL*       pbUseWCS
        );

    HRESULT
    GetDstProfileType(
        __out XDPrintSchema::PageSourceColorProfile::EProfileOption* pType
        );

    HRESULT
    GetDstProfileName(
        __inout BSTR* pbstrProfileName
        );

    HRESULT
    SetSrcProfileFromContainer(
        __in LPWSTR szProfileURI
        );

    HRESULT
    SetSrcProfileFromColDir(
        __in LPWSTR szProfile
        );

    HRESULT
    SetSrcProfileFromBuffer(
        __in                  LPWSTR szProfile,
        __in_bcount(cbBuffer) PBYTE  pBuffer,
        __in                  UINT   cbBuffer
        );

    HRESULT
    GetProfileOption(
        __out XDPrintSchema::PageSourceColorProfile::EProfileOption* pProfileOption
        );

    //
    // IResWriter interface
    //
    HRESULT
    WriteData(
        __in IPartBase*         pResource,
        __in IPrintWriteStream* pStream
        );

    HRESULT
    GetKeyName(
        __deref_out BSTR* pbstrKeyName
        );

    HRESULT
    GetResURI(
        __deref_out BSTR* pbstrResURI
        );

private:
    HRESULT
    SetProfileFromColDir(
        __in CProfile* pProfile,
        __in LPWSTR    szProfile
        );

    HRESULT
    GetColDir(
        __out CStringXDW* pcstrColDir
        );

private:
    CStringXDW                       m_strDeviceName;

    XDPrintSchema::PageSourceColorProfile::PageSourceColorProfileData m_cmProfData;

    XDPrintSchema::PageICMRenderingIntent::PageICMRenderingIntentData                       m_cmIntData;

    CProfile                         m_srcProfile;

    CProfile                         m_dstProfile;

    CTransform                       m_colorTrans;

    CComPtr<IFixedPage>              m_pFixedPage;
};


