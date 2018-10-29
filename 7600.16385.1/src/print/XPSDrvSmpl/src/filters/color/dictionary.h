/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   dict.h

Abstract:

   Dictionary class definition. The CRemoteDictionary class is responsible
   for representing a single dictionay including managing any color
   transforms in that dictionary and writing the dictionary out.

--*/

#pragma once

#include "colconv.h"

class CRemoteDictionary : public IResWriter
{
public:
    CRemoteDictionary(
        __in IFixedPage*            pFixedPage,
        __in CBitmapColorConverter* pBmpConverter,
        __in CColorRefConverter*    pRefConverter,
        __in BSTR                   bstrResURI
        );

    ~CRemoteDictionary();

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
    CComBSTR                m_bstrDictionaryURI;

    CBitmapColorConverter*  m_pBmpConverter;

    CColorRefConverter*     m_pRefConverter;

    CComPtr<IFixedPage>     m_pFixedPage;
};


