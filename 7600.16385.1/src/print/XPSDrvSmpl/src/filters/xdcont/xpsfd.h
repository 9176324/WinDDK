/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   xpsfd.h

Abstract:

   Definition of the XPS Fixed Document (FD) SAX handler. This class is
   responsible for retrieving and storing in the correct order the Fixed Pages
   that comprise the Fixed Document.

--*/

#pragma once

#include "saxhndlr.h"
#include "xps.h"

class CFixedDocument : public CSaxHandler
{
public:
    CFixedDocument();

    virtual ~CFixedDocument();

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
    GetFixedPageList(
        __out FileList* pFixedPageList
        );

    HRESULT
    Clear(
        VOID
        );

private:
    FileList m_FixedPageList;
};


