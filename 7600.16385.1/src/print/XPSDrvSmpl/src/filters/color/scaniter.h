/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   scaniter.h

Abstract:

   CScanIterator class definition. The scan iterator class provides a convenient
   interface for iterating over WIC data and retrieving scanline data approriate for
   consumption in WCS/ICM. For example, the WIC pixel formats do not have alpha channel
   positions that correspond with the WCS/ICM BMFORMAT types so this class is responsible
   for presenting bitmap data without the alpha channel and for copying alpha data from
   source to destination when scnaline changes are commited to the underlying WIC bitmap.

--*/

#pragma once

#include "bmpconv.h"
#include "wictobmscn.h"

/*
Iterates through all scanlines in a bitmap creating a buffer appropriate
to color modification. These are:

    RGB channels (in that order) as byte, word, float or fixed - no alpha
    CMYK channels (order as the source bitmap) as byte, word, float or fixed - no alpha
    nChannel (order as the source bitmap) as byte - no alpha

*/
class CScanIterator : public CBmpConverter
{
public:
    CScanIterator(
        __in     CONST CBmpConverter& bmpConv,
        __in_opt WICRect*             pRect
        );

    virtual ~CScanIterator();

    HRESULT
    Initialize(
        __in CONST BOOL& bReadOnly
        );

    VOID
    Reset(
        VOID
        );

    CScanIterator&
    operator++(
        INT
        );

    virtual HRESULT
    GetScanBuffer(
        __deref_bcount(*pcbStride) PBYTE*    ppData,
        __out                      BMFORMAT* pBmFormat,
        __out                      UINT*     pcWidth,
        __out                      UINT*     pcHeight,
        __out                      UINT*     pcbStride
        );

    HRESULT
    Commit(
        __in CONST CScanIterator& cbAlphaBuffer
        );

    BOOL
    Finished(
        VOID
        );

private:
    HRESULT
    SetCurrentIterationData(
        VOID
        );

    template <class _T, class _U>
    HRESULT
    ConvertCopyAlphaChannels(
        __in_ecount(cSrcPix) _T*         pSrc,
        __in                 CONST UINT& cSrcPix,
        __in                 CONST UINT& cSrcChan,
        __in_ecount(cDstPix) _U*         pDst,
        __in                 CONST UINT& cDstPix,
        __in                 CONST UINT& cDstChan
        )
    {
        HRESULT hr = S_OK;

        if (cDstPix == cSrcPix)
        {
            for (UINT cCurPix = 0;
                cCurPix < cDstPix;
                cCurPix++, pSrc += cSrcChan, pDst += cDstChan)
            {
                ConvertCopy(*pDst, *pSrc);
            }
        }
        else
        {
            hr = E_INVALIDARG;
        }

        ERR_ON_HR(hr);
        return hr;
    }

    template <class _T>
    HRESULT
    ConvertCopyAlphaChannels(
        __in_ecount(cSrcPix) _T*                  pSrc,
        __in                 CONST UINT&          cSrcPix,
        __in                 CONST UINT&          cSrcChan,
        __in                 CONST COLORDATATYPE& dstColType,
        __in                 PBYTE                pDst,
        __in                 CONST UINT&          cDstPix,
        __in                 CONST UINT&          cDstChan
        )
    {
        HRESULT hr = S_OK;

        if (SUCCEEDED(hr = CHECK_POINTER(pSrc, E_POINTER)) &&
            SUCCEEDED(hr = CHECK_POINTER(pDst, E_POINTER)))
        {
            //
            // Call the appropriate convert copy function by casting the dst pointer
            // to the underlying data type
            //
            switch (dstColType)
            {
                case COLOR_BYTE:
                    hr = ConvertCopyAlphaChannels(reinterpret_cast<_T*>(pSrc),
                                                  cSrcPix,
                                                  cSrcChan,
                                                  reinterpret_cast<PBYTE>(pDst),
                                                  cDstPix,
                                                  cDstChan);
                    break;

                case COLOR_WORD:
                    hr = ConvertCopyAlphaChannels(reinterpret_cast<_T*>(pSrc),
                                                  cSrcPix,
                                                  cSrcChan,
                                                  reinterpret_cast<PWORD>(pDst),
                                                  cDstPix,
                                                  cDstChan);
                    break;

                case COLOR_FLOAT:
                    hr = ConvertCopyAlphaChannels(reinterpret_cast<_T*>(pSrc),
                                                  cSrcPix,
                                                  cSrcChan,
                                                  reinterpret_cast<PFLOAT>(pDst),
                                                  cDstPix,
                                                  cDstChan);
                    break;

                case COLOR_S2DOT13FIXED:
                    hr = ConvertCopyAlphaChannels(reinterpret_cast<_T*>(pSrc),
                                                  cSrcPix,
                                                  cSrcChan,
                                                  reinterpret_cast<PS2DOT13FIXED>(pDst),
                                                  cDstPix,
                                                  cDstChan);
                    break;

                default:
                {
                    RIP("Unrecognized destination format.\n");

                    hr = E_INVALIDARG;
                }
                break;
            }
        }

        ERR_ON_HR(hr);
        return hr;
    }

private:
    WICRect            m_rectTotal;

    WICRect            m_rectCurrLock;

    BOOL               m_bSrcLine;

    CWICToBMFormatScan m_currScan;

    UINT               m_cbWICStride;

    UINT               m_cWICWidth;

    UINT               m_cWICHeight;

    UINT               m_cbWICData;

    PBYTE              m_pbWICData;
};


