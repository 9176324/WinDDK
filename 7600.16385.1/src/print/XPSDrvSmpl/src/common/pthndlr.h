/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

File Name:

   pthndlr.h

Abstract:

   Base PrintTicket handler class definition. This class provides common
   PrintTicket handling functionality for any filter that requires print
   ticket handling. A filter can from this class to get feature unspecific
   XML handling functionality.

--*/

#pragma once

#include "schema.h"
#include "pshndlr.h"

class CPTHandler : public CPSHandler
{
public:
    //
    // Constructors and destructors
    //
    CPTHandler(
        __in IXMLDOMDocument2 *pPrintTicket
        );

    virtual ~CPTHandler();

public:
    HRESULT
    DeleteFeature(
        __in_z BSTR bstrFeature
        );

    HRESULT
    DeleteProperty(
        __in_z BSTR bstrProperty
        );

protected:
    HRESULT
    QueryNodeOption(
        __in_z          BSTR  bstrQuery,
        __deref_out_opt BSTR* pbstrOption
        );

    HRESULT
    FeaturePresent(
        __in_z          BSTR          bstrFeature,
        __deref_opt_out IXMLDOMNode** ppFeatureNode = NULL
        );

    HRESULT
    GetFeatureOption(
        __in_z          BSTR  bstrFeature,
        __deref_out_opt BSTR* pbstrOption
        );

    HRESULT
    GetSubFeatureOption(
        __in_z          BSTR  bstrParentFeature,
        __in_z          BSTR  bstrFeature,
        __deref_out_opt BSTR* pbstrOption
        );

    HRESULT
    GetScoredPropertyValue(
        __in_z          BSTR  bstrParentFeature,
        __in_z          BSTR  bstrProperty,
        __deref_out_opt BSTR* pbstrValue
        );

    HRESULT
    GetScoredPropertyValue(
        __in_z BSTR bstrParentFeature,
        __in_z BSTR bstrProperty,
        __out  INT* pValue
        );

    HRESULT
    GetScoredPropertyValue(
        __in_z BSTR  bstrParentFeature,
        __in_z BSTR  bstrProperty,
        __out  REAL* pValue
        );

    HRESULT
    SetFeatureOption(
        __in_z BSTR bstrFeature,
        __in_z BSTR bstrOption
        );

    HRESULT
    SetSubFeatureOption(
        __in_z BSTR bstrParentFeature,
        __in_z BSTR bstrFeature,
        __in_z BSTR bstrOption
        );

    HRESULT
    SetPropertyAsValue(
        __in_z BSTR bstrFeature,
        __in_z BSTR bstrProperty,
        __in_z BSTR bstrValue
        );

    HRESULT
    SetPropertyAsParameterRef(
        __in_z BSTR bstrFeature,
        __in_z BSTR bstrProperty,
        __in_z BSTR bstrParameterRef,
        __in_z BSTR bstrValue
        );

    HRESULT
    CreateParamRefInitPair(
        __in        CONST BSTR       bstrParam,
        __in        CONST BSTR       bstrType,
        __in        CONST BSTR       bstrValue,
        __deref_out IXMLDOMElement** ppParamRef,
        __deref_out IXMLDOMElement** ppParamInit
        );

    HRESULT
    CreateParamRefInitPair(
        __in        CONST BSTR       bstrParam,
        __in        CONST INT        intValue,
        __deref_out IXMLDOMElement** ppParamRef,
        __deref_out IXMLDOMElement** ppParamInit
        );

    HRESULT
    CreateFeatureOptionPair(
        __in        CONST BSTR       bstrFeatureName,
        __deref_out IXMLDOMElement** ppFeatureElement,
        __deref_out IXMLDOMElement** ppOptionElement
        );

    HRESULT
    CreateFeatureOptionPair(
        __in        CONST BSTR       bstrFeatureName,
        __in        CONST BSTR       bstrOptionName,
        __deref_out IXMLDOMElement** ppFeatureElement,
        __deref_out IXMLDOMElement** ppOptionElement
        );

    HRESULT
    AppendToElement(
        __in  CONST BSTR   bstrElementName,
        __in  IXMLDOMNode* pAppendNode
        );

private:
    HRESULT
    StripKeywordNamespace(
        __inout_z BSTR* pbstrValue
        );

    HRESULT
    DeleteNodeList(
       __inout IXMLDOMNodeList* pNodeList
       );

    HRESULT
    DeleteParamInitOrphans(
       __in    IXMLDOMNodeList* pRefList,
       __inout IXMLDOMNodeList* pInitList
       );
};


