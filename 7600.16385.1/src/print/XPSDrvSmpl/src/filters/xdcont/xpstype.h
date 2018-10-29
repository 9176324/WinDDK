/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   xpstype.h

Abstract:

   Definition of the XPS content type part SAX handler. This class is responsible
   for retrieving all the content types information and storing them for access by the
   XPS processor.

--*/

#pragma once

#include "saxhndlr.h"
#include "xps.h"

class CContentTypes : public CSaxHandler
{
public:
    CContentTypes();

    virtual ~CContentTypes();

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

    HRESULT
    ValidateContentType(
        __in PCSTR              szPartName,
        __in CONST EContentType contentType
        );

private:
    HRESULT
    GetContentTypeFromString(
        __in  CONST CStringXDA& cstrPartType,
        __out EContentType*     pContentType
        );

private:
    ContentMap m_contentMap;
};


