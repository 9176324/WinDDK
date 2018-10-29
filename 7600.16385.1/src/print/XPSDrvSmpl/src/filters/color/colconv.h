/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   colconv.h

Abstract:

   Color conversion class definition. The CColorConverter class is responsible
   for managing the color conversion process for vector objects and bitmaps.

--*/

#pragma once

#include "colchan.h"
#include "rescache.h"
#include "profman.h"
#include "wcsapiconv.h"
#include "cmflt.h"

class CColorConverter
{
public:
    CColorConverter(
        __in IXpsDocumentConsumer* pXpsConsumer,
        __in IFixedPage*           pFixedPage,
        __in CFileResourceCache*   pResCache,
        __in CProfileManager*      pProfManager,
        __in ResDeleteMap*         pResDel
        );

    virtual ~CColorConverter();

    virtual HRESULT
    Convert(
        __inout BSTR* pbstr
        ) = 0;

protected:
    CComPtr<IXpsDocumentConsumer> m_pXpsConsumer;

    CComPtr<IFixedPage>           m_pFixedPage;

    CProfileManager*              m_pProfManager;

    CFileResourceCache*           m_pResCache;

    ResDeleteMap*                 m_pResDel;
};

class CBitmapColorConverter : public CColorConverter
{
public:
    CBitmapColorConverter(
        __in IXpsDocumentConsumer* pXpsConsumer,
        __in IFixedPage*           pFixedPage,
        __in CFileResourceCache*   pResCache,
        __in CProfileManager*      pProfManager,
        __in ResDeleteMap*         pResDel
        );

    virtual ~CBitmapColorConverter();

    HRESULT
    Convert(
        __inout BSTR* pbstrBmpURI
        );
};

class CColorRefConverter : public CColorConverter
{
public:
    CColorRefConverter(
        __in IXpsDocumentConsumer* pXpsConsumer,
        __in IFixedPage*           pFixedPage,
        __in CFileResourceCache*   pResCache,
        __in CProfileManager*      pProfManager,
        __in ResDeleteMap*         pResDel
        );

    virtual ~CColorRefConverter();

    HRESULT
    Convert(
        __inout BSTR* pbstrColorRef
        );

private:
    HRESULT
    InitDstChannels(
        __in    CColorChannelData* pChannelDataSrc,
        __inout CColorChannelData* pChannelDataDst
        );

    HRESULT
    GetFloatChannelData(
        __in    LPCWSTR            szColorRef,
        __inout CColorChannelData* pChannelData
        );

    HRESULT
    SetFloatChannelData(
        __in    CColorChannelData* pChannelData,
        __inout CStringXDW*        pcstrChannelData
        );

    HRESULT
    ParseColorString(
        __in    BSTR               bstrColorRef,
        __inout CColorChannelData* pChannelData,
        __out   BOOL*              pbIsResourceReference
        );

    HRESULT
    TransformColor(
        __in    CColorChannelData* pSrcData,
        __inout CColorChannelData* pDstData
        );

    HRESULT
    CreateColorString(
        __in    CColorChannelData* pDstData,
        __inout BSTR*              pbstrColorRef
        );

private:
    CColorChannelData m_srcData;

    CColorChannelData m_dstData;
};

class CResourceDictionaryConverter : public CColorConverter
{
public:
    CResourceDictionaryConverter(
        __in IXpsDocumentConsumer*  pXpsConsumer,
        __in IFixedPage*            pFixedPage,
        __in CFileResourceCache*    pResCache,
        __in CProfileManager*       pProfManager,
        __in ResDeleteMap*          pResDel,
        __in CBitmapColorConverter* pBmpConv,
        __in CColorRefConverter*    pRefConv
        );

    virtual ~CResourceDictionaryConverter();

    HRESULT
    Convert(
        __inout BSTR* pbstrDictionaryURI
        );

private:
    CBitmapColorConverter* m_pBmpConv;

    CColorRefConverter*    m_pRefConv;
};


