/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   bkdmptcnv.h

Abstract:

   Booklet/Binding devmode <-> PrintTicket conversion class definition. The class
   defines a common data representation between the DevMode (GPD) and PrintTicket
   representations and implements the conversion and validation methods required
   by CFeatureDMPTConvert.

--*/

#pragma once

#include "ftrdmptcnv.h"
#include "bkpthndlr.h"

//
// The PT handling code defines a single BindingData structure that covers both
// JobBindAllDocuments and DocumentBinding to avoid conflicts. The GPD however
// controls both so we re-use the BindingData structure to handle both in
// the DevMode
//
struct BookletSettings
{
    XDPrintSchema::Binding::BindingData settings[XDPrintSchema::Binding::EBindingMax];
};

class CBookletDMPTConv : public CFeatureDMPTConvert<BookletSettings>
{
public:
    CBookletDMPTConv();

    ~CBookletDMPTConv();

private:
    HRESULT
    GetPTDataSettingsFromDM(
        __in    PDEVMODE         pDevmode,
        __in    ULONG            cbDevmode,
        __in    PVOID            pPrivateDevmode,
        __in    ULONG            cbDrvPrivateSize,
        __out   BookletSettings* pDataSettings
        );

    HRESULT
    MergePTDataSettingsWithPT(
        __in    IXMLDOMDocument2* pPrintTicket,
        __inout BookletSettings*  pDrvSettings
        );

    HRESULT
    SetPTDataInDM(
        __in    CONST BookletSettings& drvSettings,
        __inout PDEVMODE               pDevmode,
        __in    ULONG                  cbDevmode,
        __inout PVOID                  pPrivateDevmode,
        __in    ULONG                  cbDrvPrivateSize
        );

    HRESULT
    SetPTDataInPT(
        __in    CONST BookletSettings& drvSettings,
        __inout IXMLDOMDocument2*      pPrintTicket
        );

    HRESULT STDMETHODCALLTYPE
    CompletePrintCapabilities(
        __in_opt IXMLDOMDocument2* pPrintTicket,
        __inout  IXMLDOMDocument2* pPrintCapabilities
        );
};

