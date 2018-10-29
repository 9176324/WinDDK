/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   wmpthnlr.h

Abstract:

   PageWatermark PrintTicket handling definition. The watermark PT handler
   is used to extract watermark settings from a PrintTicket and populate
   the watermark properties class with the retrieved settings. The class also
   defines a method for setting the feature in the PrintTicket given the
   data structure.

--*/

#pragma once

#include "pthndlr.h"
#include "wmdata.h"

class CWMPTHandler : public CPTHandler
{
public:
    CWMPTHandler(
        __in IXMLDOMDocument2* pPrintTicket
        );

    virtual ~CWMPTHandler();

    HRESULT
    GetData(
        __out XDPrintSchema::PageWatermark::WatermarkData* pWmData
        );

    HRESULT
    SetData(
        __in CONST XDPrintSchema::PageWatermark::WatermarkData* pWmData
        );

    HRESULT
    Delete(
        VOID
        );

private:
    HRESULT
    CreateCommonWMElements(
        __in        CONST XDPrintSchema::PageWatermark::WatermarkData* pWmData,
        __deref_out IXMLDOMElement**                                   ppWMDataElem,
        __deref_out IXMLDOMElement**                                   ppOptionElem,
        __out       PTDOMElementVector*                                pParamInitList
        );

    HRESULT
    CreateCommonVectBmpWMElements(
        __in  CONST XDPrintSchema::PageWatermark::WatermarkData* pWmData,
        __in  IXMLDOMElement*                                    pOptionElem,
        __out PTDOMElementVector*                                pParamInitList
        );

    HRESULT
    CreateTextWMElements(
        __in        CONST XDPrintSchema::PageWatermark::WatermarkData* pWmData,
        __deref_out IXMLDOMElement**                                   ppWMDataElem,
        __out       PTDOMElementVector*                                pParamInitList
        );

    HRESULT
    CreateBitmapWMElements(
        __in        CONST       XDPrintSchema::PageWatermark::WatermarkData* pWmData,
        __deref_out IXMLDOMElement**                                         ppWMDataElem,
        __out       PTDOMElementVector*                                      pParamInitList
        );

    HRESULT
    CreateVectorWMElements(
        __in        CONST XDPrintSchema::PageWatermark::WatermarkData* pWmData,
        __deref_out IXMLDOMElement**                                   ppWMDataElem,
        __out       PTDOMElementVector*                                pParamInitList
        );

    HRESULT
    GetCmnPropTypeAndValue(
        __in        CONST XDPrintSchema::PageWatermark::WatermarkData*        pWmData,
        __in        CONST XDPrintSchema::PageWatermark::ECommonWatermarkProps cmnProps,
        __deref_out BSTR*                                                     pbstrType,
        __deref_out BSTR*                                                     pbstrValue
        );

    HRESULT
    GetCmnVectBmpPropTypeAndValue(
        __in        CONST XDPrintSchema::PageWatermark::WatermarkData*         pWmData,
        __in        CONST XDPrintSchema::PageWatermark::EVectBmpWatermarkProps cmnProps,
        __deref_out BSTR*                                                      pbstrType,
        __deref_out BSTR*                                                      pbstrValue
        );

    HRESULT
    GetTxtPropTypeAndValue(
        __in        CONST XDPrintSchema::PageWatermark::WatermarkData*      pWmData,
        __in        CONST XDPrintSchema::PageWatermark::ETextWatermarkProps txtProps,
        __deref_out BSTR*                                                   pbstrType,
        __deref_out BSTR*                                                   pbstrValue
        );
};


