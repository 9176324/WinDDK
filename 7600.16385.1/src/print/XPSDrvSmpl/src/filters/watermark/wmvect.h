/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   wmvect.h

Abstract:

   VectorGraphic watermark class definition. CVectorWatermark is the
   Vector implementation of the CWatermark class. This implements methods
   for creating the page mark-up and adding the watermark resource to fixed page.

--*/

#pragma once

#include "wmbase.h"
#include "wmxps.h"
#include "wmptprop.h"

class CVectorWatermark : public CWatermark
{
public:
    CVectorWatermark(
        __in CONST CWMPTProperties& wmProps,
        __in CONST INT              resourceID
        );

    virtual ~CVectorWatermark();

    virtual HRESULT
    CreateXMLElement(
        VOID
        );

    virtual HRESULT
    GetXML(
        __deref_out BSTR* pbstrXML
        );

    virtual HRESULT
    AddParts(
        __in IXpsDocumentConsumer* pXpsConsumer,
        __in IFixedPage*           pFixedPage,
        __in CFileResourceCache*   pResCache
        );

private:
    CWatermarkMarkup  m_wmMarkup;

    CComBSTR          m_bstrMarkup;
};


