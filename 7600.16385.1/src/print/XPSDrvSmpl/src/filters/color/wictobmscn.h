/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   wictobmscn.h

Abstract:

   WIC pixel format to BMFORMAT conversion class implementation. This class provides methods
   for converting between a source WIC scanline to a target destination BMFORMAT scanline.

--*/

#pragma once

#include "bmpdata.h"

class CWICToBMFormatScan
{
public:
    CWICToBMFormatScan();

    virtual ~CWICToBMFormatScan();

    HRESULT
    Initialize(
        __in CONST WICToBMFORMAT& WICToBM
        );

    HRESULT
    SetData(
        __in                     CONST BOOL bIsSrc,
        __in_bcount(cbWicPxData) PBYTE      pWicPxData,
        __in                     CONST UINT cbWicPxData,
        __in                     CONST UINT cWidth
        );

    HRESULT
    Commit(
        __in_bcount(cbWicPxData) PBYTE      pWicPxData,
        __in                     CONST UINT cbWicPxData
        );

    HRESULT
    GetData(
        __deref_out_bcount(*pcbStride) PBYTE*    ppData,
        __out                          BMFORMAT* pBmFormat,
        __out                          UINT*     pcWidth,
        __out                          UINT*     pcbStride
        );

private:
    HRESULT
    CreateScanBuffer(
        __in CONST UINT   cWidth,
        __in CONST SIZE_T cbDataType,
        __in CONST UINT   cChannels
        );

    VOID
    FreeScanBuffer(
        VOID
        );

private:
    WICToBMFORMAT m_convData;

    PBYTE   m_pScanBuffer;

    size_t  m_cbScanBuffer;

    UINT    m_cWidth;

    PBYTE   m_pData;

    size_t  m_cbData;

    BOOL    m_bInitialized;
};

