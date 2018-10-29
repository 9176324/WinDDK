//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//  ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//  PARTICULAR PURPOSE.
//
//  Copyright  1998 - 2005  Microsoft Corporation.  All Rights Reserved.
//
//  FILE:   oemptprovider.hxx
//    
//
//  PURPOSE:  Definition of IOEMPTProvider Class.
//
//  

#pragma once

#include "xmlhandler.hxx"
#include "printschema.hxx"

using printschema::photoprintingintent::EIntentValue;

//
// The Provider class to Implement IPrintOemPrintTicketProvider Interface
//

class IOEMPTProvider : public IPrintOemPrintTicketProvider
{   
public:
    
    //  
    // Public Methods go here
    //
    IOEMPTProvider();
    virtual ~IOEMPTProvider();
    
    //
    // Basic COM Methods
    //
    STDMETHODIMP QueryInterface(THIS_ REFIID riid, void **ppvObj);

    STDMETHODIMP_(ULONG) AddRef(THIS);

    // the __drv_at tag here tells prefast that once release 
    // is called, the memory should not be considered leaked
    __drv_at(this, __drv_freesMem(object)) 
    STDMETHODIMP_(ULONG) Release(THIS);


    //
    // IPrintOemPrintTicketProvider methods
    //  

    STDMETHOD(GetSupportedVersions)(THIS_
        __in                               HANDLE  hPrinter,
        __deref_out_ecount_opt(*cVersions) INT     *ppVersions[],
        __out                              INT     *cVersions 
        );
            
    STDMETHOD(BindPrinter)(THIS_
        __in                                 HANDLE       hPrinter,
                                             INT          version,
        __out                                POEMPTOPTS   pOptions,
        __out                                INT          *cNamespaces,
        __deref_out_ecount_opt(*cNamespaces) BSTR         **ppNamespaces
        );

    STDMETHOD(PublishPrintTicketHelperInterface)(THIS_
        __in IUnknown *pHelper
        );

    STDMETHOD(QueryDeviceDefaultNamespace)(THIS_
        __out BSTR *pbstrNamespaceUri
        );

    STDMETHOD(ConvertPrintTicketToDevMode)(THIS_
        __in    IXMLDOMDocument2 *pPrintTicket,
                ULONG             cbDevmode,
        __inout PDEVMODE          pDevmode,            
                ULONG             cbDrvPrivateSize,
        __in    PVOID             pPrivateDevmode
        );
            
    STDMETHOD(ConvertDevModeToPrintTicket)(THIS_
                ULONG             cbDevmode,
        __in    PDEVMODE          pDevmode,
                ULONG             cbDrvPrivateSize,
        __in    PVOID             pPrivateDevmode,
        __inout IXMLDOMDocument2 *pPrintTicket
        );
            
    STDMETHOD(CompletePrintCapabilities)(THIS_
        __in_opt IXMLDOMDocument2 *pPrintTicket,
        __inout  IXMLDOMDocument2 *pCapabilities
        );
           
    STDMETHOD(ExpandIntentOptions)(THIS_
        __inout IXMLDOMDocument2 *pPrintTicket
        );

    STDMETHOD(ValidatePrintTicket)(THIS_
        __inout IXMLDOMDocument2 *pPrintTicket
        );

private:

    //
    // Data members go here
    //
    LONG                 m_cRef; // COM reference count 
    HANDLE               m_hPrinterCached; // cached printer handle
    IPrintCoreHelperUni *m_pCoreHelper; // cached pointer to plug-in helper interface
    
    struct IntentConversionEntry
    {
        LPCWSTR pageMediaType;
        LPCWSTR pageOutputQuality;
    };
    
    static IntentConversionEntry intentConversionTable[printschema::photoprintingintent::EIntentValueMax];

    //
    // Private Methods
    //

    HRESULT ReadAndDeletePhotoPrintingIntent(
        OEMPTXMLHandler &opxh,
        EIntentValue &eIntentValue
        );

    HRESULT AddOrOverridePageMediaType(
        OEMPTXMLHandler &opxh,
        LPCWSTR lpcwstrPageMediaType
        );

    HRESULT AddOrOverridePageOutputQuality(
        OEMPTXMLHandler &opxh,
        LPCWSTR lpcwstrPageOutputQuality
        );

}; 


