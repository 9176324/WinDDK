/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   ptmanage.cpp

Abstract:

   PrintTicket management class definition. This class encapsulate
   PrintTicket handling algorithm defined in the XPS Document specification.
   It provides a simple set and get interace for to filters and handles
   merging of tickets and the use of the Win32 PrintTicket provider API.
   The algorithm for determining the PrintTicket applies as follows:

   1. Validate and merge the PrintTicket from the FDS with the default
      printicket converted from the default devmode in the property bag.
      The resultant ticket will be the Job level ticket.

   2. Validate and merge the PrintTicket from the current FD with the Job
      level ticket from step 1. The resultant ticket will be the document
      level ticket.

   3. Validate and merge the PrintTicket from the current FP with the Doc
      level ticket from step 2. The resultant ticket will be the page
      level ticket.

--*/

#pragma once

class CPTManager
{
public:
    CPTManager();

    virtual ~CPTManager();

    HRESULT
    Initialise(
        __in   IPrintReadStream* pDefaultTicketStream,
        __in_z BSTR              bstrPrinterName,
        __in   HANDLE            userToken
        );

    HRESULT
    SetTicket(
        __in CONST IFixedDocumentSequence* pFDS
        );

    HRESULT
    SetTicket(
        __in CONST IFixedDocument* pFD
        );

    HRESULT
    SetTicket(
        __in CONST IFixedPage* pFP
        );

    HRESULT
    SetTicket(
        __in     CONST EPrintTicketScope ptScope,
        __in_opt CONST IXMLDOMDocument2* pPT
        );

    HRESULT
    GetTicket(
        __in        CONST EPrintTicketScope ptScope,
        __deref_out IXMLDOMDocument2**      ppTicket
        );

    HRESULT
    GetCapabilities(
        __in        IXMLDOMDocument2*  pTicket,
        __deref_out IXMLDOMDocument2** ppCapabilities
        );

private:
    HRESULT
    InitialisePrintTickets(
        __in IStream* pDefaultPTStream
        );

    HRESULT
    SetPTFromDOMDoc(
        __in        IXMLDOMDocument2*  pPTDOMDoc,
        __deref_out IXMLDOMDocument2** ppDomDoc
        );

    HRESULT
    SetPTFromStream(
        __in        IStream*           pPTStream,
        __deref_out IXMLDOMDocument2** ppDomDoc
        );

    HRESULT
    MergeTicket(
        __in CONST EPrintTicketScope ptScope,
        __in CONST IPartPrintTicket* pPTRef
        );

    HRESULT
    MergeTicket(
        __in CONST EPrintTicketScope ptScope,
        __in CONST IXMLDOMDocument2* pPT
        );

    HRESULT
    GetMergedTicket(
        __in        CONST EPrintTicketScope ptScope,
        __in        CONST IXMLDOMDocument2* pDelta,
        __in        IXMLDOMDocument2*       pBase,
        __deref_out IXMLDOMDocument2**      ppResult
        );

    HRESULT
    CloseProvider(
        VOID
        );

    HRESULT
    UpdateDefaultPTs(
        __in CONST EPrintTicketScope ptScope
        );

    VOID
    FreePrintTickets(
        __in CONST EPrintTicketScope ptScope
        );

private:
    CComPtr<IXMLDOMDocument2> m_pDefaultPT;

    CComPtr<IXMLDOMDocument2> m_pJobPT;

    CComPtr<IXMLDOMDocument2> m_pDocPT;

    CComPtr<IXMLDOMDocument2> m_pPagePT;

    HPTPROVIDER               m_hProvider;

    HANDLE                    m_hToken;
};


