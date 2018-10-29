//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//  ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//  PARTICULAR PURPOSE.
//
//  Copyright  1998 - 2005  Microsoft Corporation.  All Rights Reserved.
//
//  FILE:   xmlhandler.hxx
//
//
//  PURPOSE: Helper Class that handles all functionality related to
//  processing of input PrintTicket/PrintCapabilities XML document
//
//

#pragma once

//
// Print ticket handler class that performs all XML handling for the
// sample code
//

class OEMPTXMLHandler
{

public:

    OEMPTXMLHandler();
    virtual ~OEMPTXMLHandler();

    HRESULT SetRoot(
        __in IXMLDOMDocument2 *pRoot,
        __in IPrintCoreHelper *pHelper
        );

    //
    // Node creation methods
    //

    HRESULT CreateFeatureNode(
        __in_opt IXMLDOMElement *pParent,
        __in PCWSTR pszNamespaceURI,
        __in PCWSTR pszFeatureName,
        __out_opt IXMLDOMElement **ppFeatureNode
        );

    HRESULT CreateOptionNode(
        __in IXMLDOMElement *pParent,
        __in PCWSTR pszNamespaceURI,
        __in PCWSTR pszOptionName,
        __out_opt IXMLDOMElement **ppOptionElement
        );

    //
    // Get methods
    //

    HRESULT GetXMLAttribute(
        __in IXMLDOMElement *pElement,
        __deref_out_opt BSTR *pszAttrNamespaceURI,
        __deref_out_opt BSTR *pszAttrName
        ) const;

    HRESULT GetFeatureNode(
        __in_opt IXMLDOMElement *pParent,
        __in PCWSTR pszAttrNamespaceURI,
        __in PCWSTR pszAttrName,
        __deref_out_opt IXMLDOMElement **ppElement
        ) const;

    HRESULT GetOptionNode(
        __in IXMLDOMElement *pParent,
        __deref_out_opt IXMLDOMElement **ppElement
        ) const;

    HRESULT DeleteFeatureNode(
        __in_opt IXMLDOMNode *pParent,
        __in IXMLDOMNode *pElement
        );

private:

    IXMLDOMDocument2    *m_pRootDocument;
    IXMLDOMElement      *m_pRootElement;
    IMXNamespaceManager *m_pNSManager;
    DWORD                m_dwNextIndex;

    HRESULT GetXMLElement(
        __in IXMLDOMElement *pParent,
        __in PCWSTR pszElementNamespaceURI,
        __in PCWSTR pszElementName,
        __in PCWSTR pszAttrNamespaceURI,
        __in PCWSTR pszAttrName,
        __deref_out_opt IXMLDOMElement **ppElement
        ) const;

    HRESULT GetXMLElementWithoutAttribute(
        __in IXMLDOMElement *pContext,
        __in PCWSTR pszElementNamespaceURI,
        __in PCWSTR pszElementName,
        __deref_out_opt IXMLDOMElement **ppChildElement
        ) const;

    HRESULT getPrefix(
        __in IXMLDOMElement *pContext,
        __in PCWSTR pszUri,
        __deref_opt_out_opt BSTR *pbstrPrefix
        ) const;

    HRESULT declarePrefix(
        __in IXMLDOMElement *pContext,
        __in LPCWSTR pszUri,
        __in_opt LPCWSTR pszPreferredPrefix,
        __out BSTR *newPrefix
        );

    HRESULT CreateQName(
        __in IXMLDOMElement *pElement,
        __in PCWSTR pszUri,
        __in PCWSTR pszLocalName,
        __out BSTR *pQName
        );

    //
    // Adds attributes to existing XML element
    //

    HRESULT CreateXMLAttribute(
        __in IXMLDOMElement *pElement,
        __in PCWSTR pszAttributeName,
        __in PCWSTR pszNamespaceURI,
        __in PCWSTR pszAttributeValue
        );

    //
    // Creates new XML element node
    //
    HRESULT CreateXMLElement(
        __in IXMLDOMElement *pParent,
        __in PCWSTR pszElementName,
        __in PCWSTR pszNamespaceURI,
        __out IXMLDOMElement **ppCreatedElement
        );

    //
    // Helpers
    //
    HRESULT getLocalName(
        __in PCWSTR pszQName,
        __out BSTR *ppbstrLocalName
        ) const;

    HRESULT getUri(
        IXMLDOMElement *pContext,
        __in PCWSTR pszQName,
        __out BSTR *pbstrURI
        ) const;

};


