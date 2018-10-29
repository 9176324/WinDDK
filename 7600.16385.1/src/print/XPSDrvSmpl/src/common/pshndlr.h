/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

File Name:

    pshndlr.h

Abstract:

    Base PrintSchema Document handler class definition.
    This class provides common PrintTicket / PrintCapabilities handling functionality.
    A class can derive from this base class to get PrintTicket/PrintCapabilities
    unspecific XML handling functionality.

    Note: The PrintSchema handler code is only intended to work with the sdtandard
    public PrintSchema keywords.

--*/

#pragma once

#include "schema.h"

typedef vector< CComPtr<IXMLDOMElement> > PTDOMElementVector;

class CPSHandler
{
public:
    //
    // Constructors and destructors
    //
    CPSHandler(
        __in_z BSTR bstrDocumentType,
        __in IXMLDOMDocument2 *pPrintDocument
        );

    virtual ~CPSHandler();

public:
    HRESULT
    DeleteNode(
        __in IXMLDOMNode* pNode
        );

    HRESULT
    DeletePrivateFeatures(
        __in_z BSTR bstrPrivateNS
        );

protected:
    HRESULT
    QueryNode(
        __in_z BSTR  bstrQuery
        );

    HRESULT
    QueryNodeValue(
        __in_z      BSTR  bstrQuery,
        __deref_out BSTR* pbstrValue
        );

    HRESULT
    QueryNodeValue(
        __in_z BSTR  bstrQuery,
        __out  REAL* pValue
        );

    HRESULT
    QueryNodeValue(
        __in_z BSTR  bstrQuery,
        __out  INT* pValue
        );

    //
    // See pshndlr.cpp for a note about the annotation on pbstrValue
    //
    HRESULT
    GetNodeValue(
        __in_z      BSTR  bstrNodeQuery,
        __notnull __elem_readableTo(1) __pre __deref __valid __pre __deref __exceptthat __maybenull
        __post __drv_when(return != S_FALSE, __deref __notnull)
        __post __drv_when(return == S_FALSE, __deref __maybenull)
                    BSTR* pbstrValue
        );

    HRESULT
    GetAttributeValue(
        __in            CONST IXMLDOMNode* pNode,
        __in_z          BSTR               bstrAttribName,
        __deref_out_opt BSTR*              pbstrResult
        );

    HRESULT
    GetNodes(
        __in        BSTR              bstrElementName,
        __deref_out IXMLDOMNodeList** ppNodeList
        );

    HRESULT
    CreateProperty(
        __in        CONST BSTR       bstrPropName,
        __deref_out IXMLDOMElement** ppPropElement
        );

    HRESULT
    CreateProperty(
        __in        CONST BSTR       bstrPropName,
        __in        CONST BSTR       bstrType,
        __in        CONST BSTR       bstrValue,
        __deref_out IXMLDOMElement** ppPropElement
        );

    HRESULT
    CreateFWProperty(
        __in        CONST BSTR       bstrPropName,
        __deref_out IXMLDOMElement** ppPropElement
        );

    HRESULT
    CreateFWProperty(
        __in        CONST BSTR       bstrPropName,
        __in        CONST BSTR       bstrType,
        __in        CONST BSTR       bstrValue,
        __deref_out IXMLDOMElement** ppPropElement
        );

    HRESULT
    CreateScoredProperty(
        __in        CONST BSTR       bstrPropName,
        __deref_out IXMLDOMElement** ppScoredPropElement
        );

    HRESULT
    CreateScoredProperty(
        __in        CONST BSTR       bstrPropName,
        __in        CONST BSTR       bstrType,
        __in        CONST BSTR       bstrValue,
        __deref_out IXMLDOMElement** ppScoredPropElement
        );

    HRESULT
    CreateScoredProperty(
        __in        CONST BSTR       bstrPropName,
        __in        CONST BSTR       bstrValue,
        __deref_out IXMLDOMElement** ppScoredPropElement
        );

    HRESULT
    CreateScoredProperty(
        __in        CONST BSTR       bstrPropName,
        __in        CONST INT        intValue,
        __deref_out IXMLDOMElement** ppScoredPropElement
        );

    HRESULT
    CreateScoredProperty(
        __in        CONST BSTR       bstrPropName,
        __in        CONST REAL       realValue,
        __deref_out IXMLDOMElement** ppScoredPropElement
        );

    HRESULT
    CreateScoredProperty(
        __in        CONST BSTR       bstrPropName,
        __in        IXMLDOMNode*     pValueNode,
        __deref_out IXMLDOMElement** ppScoredPropElement
        );

    HRESULT
    GetNode(
    __in_z      BSTR          bstrNodeQuery,
    __deref_out IXMLDOMNode** ppNode
    );

    HRESULT
    CreateXMLAttribute(
        __inout    IXMLDOMElement *pTarget,
        __in       PCWSTR pszName,
        __in_opt   PCWSTR pszTargetURI,
        __in       PCWSTR pszValue
    );

    HRESULT
    CreateXMLElement(
        __in       PCWSTR pszName,
        __in       PCWSTR pszTargetURI,
        __out_opt  IXMLDOMElement **ppEl
    );

private:

    HRESULT
    GetPrefixFromURI(
        __in_z      BSTR  bstrNSURI,
        __deref_out BSTR* bstrNSPrefix
        );

protected:
    //
    // Document and Type
    //
    CComPtr<IXMLDOMDocument2> m_pPrintDocument;
    CComBSTR    m_bstrDocumentType;

    //
    // W3 namespace prefixes
    //
    CComBSTR    m_bstrSchemaPrefix;
    CComBSTR    m_bstrSchemaInstPrefix;

    //
    // Namespace abbreviations
    //
    CComBSTR    m_bstrFrameworkPrefix;
    CComBSTR    m_bstrKeywordsPrefix;
    CComBSTR    m_bstrUserKeywordsPrefix;
};


