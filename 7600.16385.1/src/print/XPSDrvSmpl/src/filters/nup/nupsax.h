/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   nupsax.h

Abstract:

   NUp SAX handler definition. The class derives from the default SAX handler
   and implements only the necessary SAX APIs to process the mark-up. The
   handler is responsible for copying page mark-up to a writer, removing
   the fixed page opening and closing tags. It is also responsible for
   identifying resources that need to be copied from the source page to
   the NUp page.

--*/

#pragma once

#include "saxhndlr.h"
#include "rescpy.h"
#include "nupxform.h"

class CNUpSaxHandler : public CSaxHandler
{
public:
    CNUpSaxHandler(
        __in IPrintWriteStream* pWriter,
        __in CResourceCopier*   pResCopier,
        __in CNUpTransform*     pNUpTransform
       );

    virtual ~CNUpSaxHandler();

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

private:
    CComPtr<IPrintWriteStream>  m_pWriter;

    CComBSTR                    m_bstrOpenElement;

    BOOL                       m_bOpenTag;

    CResourceCopier*           m_pResCopier;

    CNUpTransform*             m_pNUpTransform;
};


