/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   xdsmplptprov.h

Abstract:

   Definition of the PrintTicket provider plugin. This is responsible for
   handling PrintTicket features that are too complex for the GPD->PrintTicket
   automapping facility.

--*/

#pragma once

#include "cunknown.h"
#include "schema.h"
#include "ftrdmptcnv.h"

typedef vector<IFeatureDMPTConvert*> DMPTConvCollection;

class CXDSmplPTProvider : public CUnknown<IPrintOemPrintTicketProvider>
{
public:
    CXDSmplPTProvider();

    virtual ~CXDSmplPTProvider();

    //
    // IPrintOemPrintTicketProvider methods
    //
    virtual HRESULT STDMETHODCALLTYPE
    GetSupportedVersions(
        __in                            HANDLE  hPrinter,
        __deref_out_ecount(*pcVersions) INT*    ppVersions[],
        __out                           INT*    pcVersions
        );

    virtual HRESULT STDMETHODCALLTYPE
    BindPrinter(
        __in                                  HANDLE     hPrinter,
        __in                                  INT        version,
        __out                                 POEMPTOPTS pOptions,
        __out                                 INT*       pcNamespaces,
        __deref_out_ecount_opt(*pcNamespaces) BSTR**     ppNamespaces
        );

    virtual HRESULT STDMETHODCALLTYPE
    PublishPrintTicketHelperInterface(
        __in IUnknown* pHelper
        );

    virtual HRESULT STDMETHODCALLTYPE
    QueryDeviceDefaultNamespace(
        __out BSTR* pbstrNamespaceUri
        );

    virtual HRESULT STDMETHODCALLTYPE
    ConvertPrintTicketToDevMode(
        __in    IXMLDOMDocument2* pPrintTicket,
        __in    ULONG             cbDevmode,
        __inout PDEVMODE          pDevmode,
        __in    ULONG             cbDrvPrivateSize,
        __in    PVOID             pPrivateDevmode
        );

    virtual HRESULT STDMETHODCALLTYPE
    ConvertDevModeToPrintTicket(
        __in    ULONG             cbDevmode,
        __in    PDEVMODE          pDevmode,
        __in    ULONG             cbDrvPrivateSize,
        __in    PVOID             pPrivateDevmode,
        __inout IXMLDOMDocument2* pPrintTicket
        );

    virtual HRESULT STDMETHODCALLTYPE
    CompletePrintCapabilities(
        __in_opt IXMLDOMDocument2* pPrintTicket,
        __inout  IXMLDOMDocument2* pCapabilities
        );

    virtual HRESULT STDMETHODCALLTYPE
    ExpandIntentOptions(
        __inout IXMLDOMDocument2* pPrintTicket
        );

    virtual HRESULT STDMETHODCALLTYPE
    ValidatePrintTicket(
        __inout IXMLDOMDocument2* pPrintTicket
        );

private:
    HRESULT
    DeleteConverters(
        VOID
        );


private:
    HANDLE                       m_hPrinterCached;

    CComPtr<IPrintCoreHelperUni> m_pCoreHelper;

    DMPTConvCollection           m_vectFtrDMPTConverters;

    CComBSTR                     m_bstrPrivateNS;
};


