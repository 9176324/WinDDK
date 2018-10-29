/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   xpsrels.h

Abstract:

   Definition of the XPS rels part SAX handler. This class is responsible
   for retrieving all the rels parts for a particular rels file and storing
   them for access by the XPS processor.

--*/

#pragma once

#include "saxhndlr.h"
#include "xps.h"

class CRels : public CSaxHandler
{
public:
    CRels();

    virtual ~CRels();

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
    GetRelsTypeList(
        __in  PCSTR                szFileName,
        __out CONST RelsTypeList** ppFileList
        );

    HRESULT
    SetCurrentFileName(
        __in PCSTR szFileName
        );

private:
    HRESULT
    GetRelsTypeFromString(
        __in  CONST CStringXDA& cstrRelsType,
        __out ERelsType*        pRelsType
        );

private:
    RelsMap  m_relsMap;

    CStringXDA m_cstrCurrentFileName;
};


