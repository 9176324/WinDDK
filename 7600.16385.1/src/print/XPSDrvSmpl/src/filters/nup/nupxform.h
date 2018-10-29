/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   nupxform.h

Abstract:

   NUp transform definition. The NUp transform class is responsible
   for calculating the appropriate matrix transform for a given page in
   an NUp sequence.

--*/

#pragma once

#include "nuptprps.h"

class CNUpTransform
{
public:
    CNUpTransform(
       __in CNUpPTProperties* pNUpPTProps
       );

    virtual ~CNUpTransform();

    VOID
    SetCurrentPage(
        __in UINT cPage
    );

    HRESULT
    GetPageTransform(
        __in  SizeF   sizePage,
        __out Matrix* pTransform
        );

    HRESULT
    MatrixToXML(
        __in        CONST Matrix* pMatrix,
        __deref_out BSTR*         pbstrMatrixXForm
        );

private:
    HRESULT
    InitTransformMap(
        VOID
        );

private:
    //
    // Nup count
    //
    UINT  m_CanvasCount;

    //
    // Current Nup index
    //
    UINT  m_cCurrPageIndex;

    //
    // Presentation order
    //
    XDPrintSchema::NUp::PresentationDirection::ENUpDirectionOption m_NUpPresentDirection;

    //
    // Target page size
    //
    SizeF m_sizeTargetPage;

    //
    // Nup count across
    //
    UINT  m_CanvasXCount;

    //
    // Nup count down
    //
    UINT  m_CanvasYCount;

    //
    // Vector of page maps
    //
    vector<UINT> m_pageNumVect;

    //
    // We need to handle page rotation for 2, 6 and 8 up
    //
    BOOL m_bRotatePage;
};


