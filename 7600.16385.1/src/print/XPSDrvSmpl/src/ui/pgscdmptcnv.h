/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   pgscdmptcnv.h

Abstract:

   PageScaling devmode <-> PrintTicket conversion class definition.
   The class defines a common data representation between the DevMode (GPD) and PrintTicket
   representations and implements the conversion and validation methods required
   by CFeatureDMPTConvert.

--*/

#pragma once

#include "ftrdmptcnv.h"
#include "pgscpthndlr.h"
#include "pgscpchndlr.h"
#include "uiproperties.h"

class CPageScalingDMPTConv : public CFeatureDMPTConvert<XDPrintSchema::PageScaling::PageScalingData>
{
public:
    CPageScalingDMPTConv();

    ~CPageScalingDMPTConv();

private:
    HRESULT
    GetPTDataSettingsFromDM(
        __in  PDEVMODE pDevmode,
        __in  ULONG    cbDevmode,
        __in  PVOID    pPrivateDevmode,
        __in  ULONG    cbDrvPrivateSize,
        __out XDPrintSchema::PageScaling::PageScalingData* pDataSettings
        );

    HRESULT
    MergePTDataSettingsWithPT(
        __in    IXMLDOMDocument2* pPrintTicket,
        __inout XDPrintSchema::PageScaling::PageScalingData*  pDrvSettings
        );

    HRESULT
    SetPTDataInDM(
        __in    CONST XDPrintSchema::PageScaling::PageScalingData& drvSettings,
        __inout PDEVMODE pDevmode,
        __in    ULONG    cbDevmode,
        __inout PVOID    pPrivateDevmode,
        __in    ULONG    cbDrvPrivateSize
        );

    HRESULT
    SetPTDataInPT(
        __in    CONST XDPrintSchema::PageScaling::PageScalingData&  drvSettings,
        __inout IXMLDOMDocument2* pPrintTicket
        );

    HRESULT STDMETHODCALLTYPE
    CompletePrintCapabilities(
        __in_opt IXMLDOMDocument2*,
        __inout  IXMLDOMDocument2* pPrintCapabilities
        );
};


