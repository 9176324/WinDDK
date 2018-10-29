/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   wmptprop.h

Abstract:

   Watermark properties class definition. The Watermark properties class
   is responsible for holding and controling Watermark properties.

--*/

#pragma once

#include "wmdata.h"

class CWMPTProperties
{
public:
    CWMPTProperties(
        __in CONST XDPrintSchema::PageWatermark::WatermarkData& wmData
        );

    virtual ~CWMPTProperties();

    HRESULT
    GetType(
        __out XDPrintSchema::PageWatermark::EWatermarkOption* pType
        );

    HRESULT
    GetLayering(
        __out XDPrintSchema::PageWatermark::Layering::ELayeringOption* pLayering
        );

    HRESULT
    GetBounds(
        __out RectF* pBounds
        );

    HRESULT GetOrigin(
        __out PointF* pOrigin
        );

    HRESULT
    GetAngle(
        __out REAL* pAngle
        );

    HRESULT
    GetTransparency(
        __out INT* pTransparency
        );

    HRESULT
    GetOpacity(
        __out REAL* pOpacity
        );

    HRESULT
    GetTransparency(
        __deref_out BSTR* pbstrTransparency
        );

    HRESULT
    GetOpacity(
        __deref_out BSTR* pbstrOpacity
        );

    HRESULT
    GetFontColor(
        __deref_out BSTR* pbstrColor
        );

    HRESULT
    GetText(
        __deref_out BSTR* pbstrText
        );

    HRESULT
    GetFontSize(
        __out INT* pSize
        );

    HRESULT
    GetFontEmSize(
        __deref_out BSTR* pbstrSize
        );

protected:
    XDPrintSchema::PageWatermark::WatermarkData m_wmData;
};


