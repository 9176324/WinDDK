/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   scalepage.h

Abstract:

   Page Scaling class implementation. The Page Scaling class provides
   functionality required to generate the XPS markup to perform page scaling.
   This includes methods for converting a GDI matrix object
   into the appropriate XPS matrix mark-up and intialising the matrix according to
   the Page Scaling options.

--*/

#pragma once

#include "pgscptprop.h"

class CPageScaling
{
public:
    CPageScaling(
        __in CONST CPGSCPTProperties& psProps
        );

    virtual ~CPageScaling();

    HRESULT
    GetFixedPageWidth(
        __deref_out BSTR* pbstrWidth
        );

    HRESULT
    GetFixedPageHeight(
        __deref_out BSTR* pbstrHeight
        );

    HRESULT
    GetOpenTagXML(
        __deref_out BSTR* pbstrXML
        );

    HRESULT
    GetCloseTagXML(
        __deref_out BSTR* pbstrXML
        );

    HRESULT
    SetPageDimensions(
        __in CONST REAL width,
        __in CONST REAL height
        );

    HRESULT
    SetBleedBox(
        __in RectF* pBleedBox
        );

    HRESULT
    SetContentBox(
        __in RectF* pContentBox
        );

private:
    HRESULT
    CreateTransform(
        __deref_out BSTR* pbstrMatrixXForm
        );

    HRESULT
    CalculateMatrix(
        __inout Matrix* pMatrix
    );

    HRESULT
    MatrixToXML(
        __in        CONST Matrix* pMatrix,
        __deref_out BSTR*         pbstrMatrixXForm
        );

private:
    CComPtr<IXMLDOMDocument2> m_pDOMDoc;

    CComPtr<IXMLDOMElement>   m_pCanvasElem;

    CPGSCPTProperties         m_PGSCProps;

    RectF                     m_bleedBox;

    RectF                     m_contentBox;

    SizeF                     m_pageDimensions;

    BOOL                      m_bAddCanvas;
};


