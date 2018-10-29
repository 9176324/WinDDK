/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   bmpconv.h

Abstract:

   WIC bitmap conversion class definition. This class provides a wrapper to a bitmap
   stream that uses WIC to access bitmap data and provide conversion functionality.

--*/

#pragma once

#include "bmpdata.h"

class CBmpConverter
{
public:
    CBmpConverter();

    CBmpConverter(
        __in CONST EWICPixelFormat& ePixFormat,
        __in CONST UINT&            cWidth,
        __in CONST UINT&            cHeight,
        __in CONST DOUBLE&          dpiX,
        __in CONST DOUBLE&          dpiY
        );

    CBmpConverter(
        __in IStream* pStream
        );

    CBmpConverter(
        __in CONST CBmpConverter& converter
        );

    virtual ~CBmpConverter();

    HRESULT
    Initialize(
        __in CONST EWICPixelFormat& ePixFormat,
        __in CONST UINT&            cWidth,
        __in CONST UINT&            cHeight,
        __in CONST DOUBLE&          dpiX,
        __in CONST DOUBLE&          dpiY
        );

    HRESULT
    Initialize(
        __in IStream* pStream
        );

    HRESULT
    Initialize(
        __in IWICBitmapSource* pSource
        );

    HRESULT
    Write(
        __in    REFGUID  guidContainerFormat,
        __inout IStream* pStream
        );

    HRESULT
    Convert(
        __in  EWICPixelFormat ePixFormat,
        __out BOOL*           pbCanConvert
        );

    HRESULT
    LockSurface(
        __in            WICRect*    prcLock,
        __in            CONST BOOL& bReadOnly,
        __out           UINT*       pcbStride,
        __out           UINT*       pcWidth,
        __out           UINT*       pcHeight,
        __inout         UINT*       pcbData,
        __deref_out_opt PBYTE*      ppbData
        );

    HRESULT
    UnlockSurface(
        VOID
        );

    HRESULT
    GetColorContext(
        __deref_out IWICColorContext** ppColorContext
        );

    BOOL
    HasAlphaChannel(
        VOID
        ) CONST;

    BOOL
    HasColorContext(
        VOID
        ) CONST;

    BOOL
    HasColorProfile(
        VOID
        ) CONST;

    EWICPixelFormat
    GetPixelFormat(
        VOID
        );

    HRESULT
    GetSize(
        __out UINT* pcWidth,
        __out UINT* pcHeight
        );

    HRESULT
    GetResolution(
        __out DOUBLE* pDpiX,
        __out DOUBLE* pDpiY
        );

    HRESULT
    SetProfile(
        __in_z LPWSTR szProfile
        );

    CBmpConverter&
    operator=(
        __in CONST CBmpConverter& converter
        );

private:
    HRESULT
    CreateImagingFactory(
        VOID
        );

    HRESULT
    PixelFormatFromGUID(
        __in  REFGUID          pixelFormat,
        __out EWICPixelFormat* pPixFormat
        );

protected:
    CComPtr<IWICImagingFactory>    m_pImagingFactory;

    CComPtr<IWICBitmap>            m_pBitmap;

    CComPtr<IWICColorContext>      m_pColorContext;

    CComPtr<IWICBitmapLock>        m_pCurrentLock;

    EWICPixelFormat                m_ePixelFormat;
};


