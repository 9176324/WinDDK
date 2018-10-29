/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   wmbase.h

Abstract:

   Base watermark class implementation. The base watermark class provides
   common functionality required between different watermarks (Text, RasterGraphic
   and VectorGraphic. This includes methods for converting a GDI matrix object
   into the appropriate XPS matrix mark-up and intialising the matrix according to
   the watermark options.

--*/

#pragma once

#include "rescache.h"
#include "wmptprop.h"

class CWatermark
{
public:
    CWatermark(
        __in CONST CWMPTProperties& wmProps
        );

    virtual ~CWatermark();

    virtual HRESULT
    CreateXMLElement(
        VOID
        ) = 0;

    virtual HRESULT
    AddParts(
        __in IXpsDocumentConsumer* pXpsConsumer,
        __in IFixedPage*           pFixedPage,
        __in CFileResourceCache*   pResCache

        ) = 0;

    virtual HRESULT
    GetXML(
        __deref_out BSTR* pbstrXML
        );

    virtual BOOL
    InsertStart(
        VOID
        );

    virtual BOOL
    InsertEnd(
        VOID
        );

protected:
    HRESULT
    CreateWMTransform(
        __in        RectF wmBounds,
        __deref_out BSTR* pbstrMatrixXForm
        );

    HRESULT
    CreateWMTransform(
        __in        PointF wmOrigin,
        __deref_out BSTR*  pbstrMatrixXForm
        );

private:
    HRESULT
    MatrixToXML(
        __in        CONST Matrix* pMatrix,
        __deref_out BSTR*         pbstrMatrixXForm
        );

protected:
    CComPtr<IXMLDOMDocument2> m_pDOMDoc;

    CComPtr<IXMLDOMElement>   m_pWMElem;

    CWMPTProperties           m_WMProps;
};


