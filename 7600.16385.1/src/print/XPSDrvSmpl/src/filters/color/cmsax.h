/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   cmsax.h

Abstract:

   Color management sax handler definition. The color management SAX handler
   is responsible for parsing the FixedPage mark-up for and vector objects
   or bitmaps which contain color data and altering the mark-up where appropriate.

--*/

#pragma once

#include "saxhndlr.h"
#include "colconv.h"

class CCMSaxHandler : public CSaxHandler
{
public:
    CCMSaxHandler(
        __in     IPrintWriteStream*            pWriter,
        __in     CBitmapColorConverter*        pBmpConverter,
        __in     CColorRefConverter*           pRefConverter,
        __in_opt CResourceDictionaryConverter* pDictConverter
        );

    virtual ~CCMSaxHandler();

    virtual HRESULT STDMETHODCALLTYPE
    startElement(
        CONST wchar_t*,
        INT,
        CONST wchar_t*,
        INT,
        __in_ecount(cchQName) CONST wchar_t*  pwchQName,
        __in                  INT             cchQName,
        __in                  ISAXAttributes* pAttributes
        );

    virtual HRESULT STDMETHODCALLTYPE
    endElement(
        CONST wchar_t*,
        INT,
        CONST wchar_t*,
        INT,
        __in_ecount(cchQName) CONST wchar_t* pwchQName,
        __in                  INT            cchQName
        );

    HRESULT STDMETHODCALLTYPE
    startDocument(
        void
        );

private:
    CComPtr<IPrintWriteStream> m_pWriter;

    CComBSTR                   m_bstrOpenElement;

    BOOL                       m_bOpenTag;

    CColorConverter*           m_pBmpConv;

    CColorConverter*           m_pRefConv;

    CColorConverter*           m_pDictConv;
};


