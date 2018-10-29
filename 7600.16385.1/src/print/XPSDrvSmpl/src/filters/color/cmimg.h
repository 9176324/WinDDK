/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   cmimg.h

Abstract:

   Color management image definition. The CColorManagedImage class is responsible
   for managing the image resources used in a container. This implements
   the IResWriter interface so that the font can be added to the resource
   cache.

--*/

#pragma once

#include "rescache.h"
#include "profman.h"
#include "scaniter.h"
#include "cmflt.h"

class CColorManagedImage : public IResWriter
{
public:
    CColorManagedImage(
        __in BSTR             bstrResURI,
        __in CProfileManager* pProfManager,
        __in IFixedPage*      pFixedPage,
        __in ResDeleteMap*    pResDel
        );

    ~CColorManagedImage();

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
    SetSrcProfile(
        __in CBmpConverter* pScanIter
        );

    HRESULT
    TransformScanLines(
        __in CScanIterator* pSrcScans,
        __in CScanIterator* pDstScans
        );

private:
    CComBSTR            m_bstrBitmapURI;

    CComBSTR            m_bstrSrcProfileURI;

    CProfileManager*    m_pProfManager;

    CBmpConverter       m_srcBmp;

    CBmpConverter       m_dstBmp;

    CComPtr<IFixedPage> m_pFixedPage;

    ResDeleteMap*       m_pResDel;
};


