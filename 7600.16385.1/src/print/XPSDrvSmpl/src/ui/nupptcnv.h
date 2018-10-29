/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   nupptcnv.h

Abstract:

   JobNUpAllDocumentsContiguously and DocumentNUp devmode <-> PrintTicket conversion class definition.
   The class defines a common data representation between the DevMode (GPD) and PrintTicket
   representations and implements the conversion and validation methods required
   by CFeatureDMPTConvert.

--*/

#pragma once

#include "ftrdmptcnv.h"
#include "nupthndlr.h"
#include "nupdata.h"

//
// The PT handling code defines a single NUpData structure that covers
// both JobNUpAllDocumentsContiguously and DocumentNUp to avoid conflicts. The GPD however
// controls both so we re-use the NUpData structure to handle both in
// the DevMode by using a NUpData for each Job and Document feature
//
struct NUpSettings
{
    XDPrintSchema::NUp::NUpData settings[XDPrintSchema::NUp::ENUpFeatureMax];
};

class CNUpDMPTConv : public CFeatureDMPTConvert<NUpSettings>
{
public:
    CNUpDMPTConv();

    virtual ~CNUpDMPTConv();

private:
    HRESULT
    GetPTDataSettingsFromDM(
        __in    PDEVMODE     pDevmode,
        __in    ULONG        cbDevmode,
        __in    PVOID        pPrivateDevmode,
        __in    ULONG        cbDrvPrivateSize,
        __out   NUpSettings* pDataSettings
        );

    HRESULT
    MergePTDataSettingsWithPT(
        __in    IXMLDOMDocument2* pPrintTicket,
        __inout NUpSettings*      pDrvSettings
        );

    HRESULT
    SetPTDataInDM(
        __in    CONST NUpSettings& drvSettings,
        __inout PDEVMODE           pDevmode,
        __in    ULONG              cbDevmode,
        __inout PVOID              pPrivateDevmode,
        __in    ULONG              cbDrvPrivateSize
        );

    HRESULT
    SetPTDataInPT(
        __in    CONST NUpSettings& drvSettings,
        __inout IXMLDOMDocument2*  pPrintTicket
        );

    HRESULT STDMETHODCALLTYPE
    CompletePrintCapabilities(
        __in_opt IXMLDOMDocument2*,
        __inout  IXMLDOMDocument2* pPrintCapabilities
        );
};


