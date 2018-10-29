/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   wmdmptcnv.h

Abstract:

   PageWatermark devmode <-> PrintTicket conversion class definition.
   The class defines a common data representation between the DevMode (GPD) and PrintTicket
   representations and implements the conversion and validation methods required
   by CFeatureDMPTConvert.

--*/

#pragma once

#include "ftrdmptcnv.h"
#include "wmpthndlr.h"
#include "uiproperties.h"

class CWatermarkDMPTConv : public CFeatureDMPTConvert<XDPrintSchema::PageWatermark::WatermarkData>
{
public:
    CWatermarkDMPTConv();

    ~CWatermarkDMPTConv();

private:
    HRESULT
    GetPTDataSettingsFromDM(
        __in  PDEVMODE pDevmode,
        __in  ULONG    cbDevmode,
        __in  PVOID    pPrivateDevmode,
        __in  ULONG    cbDrvPrivateSize,
        __out XDPrintSchema::PageWatermark::WatermarkData* pDataSettings
        );

    HRESULT
    MergePTDataSettingsWithPT(
        __in    IXMLDOMDocument2* pPrintTicket,
        __inout XDPrintSchema::PageWatermark::WatermarkData*    pDrvSettings
        );

    HRESULT
    SetPTDataInDM(
        __in    CONST   XDPrintSchema::PageWatermark::WatermarkData& drvSettings,
        __inout PDEVMODE pDevmode,
        __in    ULONG    cbDevmode,
        __inout PVOID    pPrivateDevmode,
        __in    ULONG    cbDrvPrivateSize
        );

    HRESULT
    SetPTDataInPT(
        __in    CONST XDPrintSchema::PageWatermark::WatermarkData&    drvSettings,
        __inout IXMLDOMDocument2* pPrintTicket
        );

    HRESULT STDMETHODCALLTYPE
    CompletePrintCapabilities(
        __in_opt IXMLDOMDocument2*,
        __inout  IXMLDOMDocument2* pPrintCapabilities
        );
};


