/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   wmrast.h

Abstract:

   RasterGraphic watermark class definition. CRasterWatermark is the
   raster implementation of the CWatermark class. This implements methods
   for creating the page mark-up and adding the watermark resource to the
   resource cache.

--*/

#pragma once

#include "wmbase.h"
#include "wmimg.h"
#include "wmptprop.h"

class CRasterWatermark : public CWatermark
{
public:
    CRasterWatermark(
        __in CONST CWMPTProperties& wmProps,
        __in CONST INT              resourceID
        );

    virtual ~CRasterWatermark();

    virtual HRESULT
    CreateXMLElement(
        VOID
        );

    virtual HRESULT
    AddParts(
        __in IXpsDocumentConsumer* pXpsConsumer,
        __in IFixedPage*           pFixedPage,
        __in CFileResourceCache*   pResCache
        );

private:
    CComBSTR         m_bstrImageURI;

    CWatermarkImage  m_wmBMP;
};


