/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   coldmptcnv.h

Abstract:

   PageSourceColorProfile devmode <-> PrintTicket conversion class definition.
   The class defines a common data representation between the DevMode (GPD) and PrintTicket
   representations and implements the conversion and validation methods required
   by CFeatureDMPTConvert.

--*/

#pragma once

#include "ftrdmptcnv.h"
#include "cmprofpthndlr.h"
#include "cmprofiledata.h"

class CColorProfileDMPTConv :
    public CFeatureDMPTConvert<XDPrintSchema::PageSourceColorProfile::PageSourceColorProfileData>
{
public:
    CColorProfileDMPTConv();

    virtual ~CColorProfileDMPTConv();

private:
    HRESULT
    GetPTDataSettingsFromDM(
        __in    PDEVMODE         pDevmode,
        __in    ULONG            cbDevmode,
        __in    PVOID            pPrivateDevmode,
        __in    ULONG            cbDrvPrivateSize,
        __out   XDPrintSchema::PageSourceColorProfile::PageSourceColorProfileData* pDataSettings
        );

    HRESULT
    MergePTDataSettingsWithPT(
        __in    IXMLDOMDocument2* pPrintTicket,
        __inout XDPrintSchema::PageSourceColorProfile::PageSourceColorProfileData*  pDrvSettings
        );

    HRESULT
    SetPTDataInDM(
        __in    CONST XDPrintSchema::PageSourceColorProfile::PageSourceColorProfileData& drvSettings,
        __inout PDEVMODE pDevmode,
        __in    ULONG    cbDevmode,
        __inout PVOID    pPrivateDevmode,
        __in    ULONG    cbDrvPrivateSize
        );

    HRESULT
    SetPTDataInPT(
        __in    CONST XDPrintSchema::PageSourceColorProfile::PageSourceColorProfileData& drvSettings,
        __inout IXMLDOMDocument2* pPrintTicket
        );

    HRESULT STDMETHODCALLTYPE
    CompletePrintCapabilities(
        __in_opt IXMLDOMDocument2*,
        __inout  IXMLDOMDocument2* pPrintCapabilities
        );
};


