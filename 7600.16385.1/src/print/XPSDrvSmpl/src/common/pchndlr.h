/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

File Name:

    pchndlr.h

Abstract:

    Base PrintCapabilities handler class definition. This class provides common
    PrintCapabilities handling functionality for any feature that requires
    PrintCapabilities handling. A feature specific handler can inherit from
    this class to get feature unspecific XML handling functionality.

--*/

#pragma once

#include "schema.h"
#include "pshndlr.h"

class CPCHandler : public CPSHandler
{
public:
    //
    // Constructors and destructors
    //
    CPCHandler(
        __in IXMLDOMDocument2 *pPrintCapabilities
        );

    virtual ~CPCHandler();

protected:

    HRESULT
    CreateStringParameterDef(
        __in_z      CONST BSTR       bstrParamName,
        __in        CONST BOOL       bIsPublicKeyword,
        __in_z_opt  CONST BSTR       bstrDisplayName,
        __in        CONST BSTR       bstrDefaultValue,
        __in        CONST INT        minLength,
        __in        CONST INT        maxLength,
        __in_z      CONST BSTR       bstrUnitType,
        __deref_out IXMLDOMElement** ppParameterDef
        );

    HRESULT
    CreateIntParameterDef(
        __in_z      CONST BSTR       bstrParamName,
        __in        CONST BOOL       bIsPublicKeyword,
        __in_z_opt  CONST BSTR       bstrDisplayName,
        __in        CONST INT        defaultValue,
        __in        CONST INT        minValue,
        __in        CONST INT        maxValue,
        __in        CONST INT        multiple,
        __in_z      CONST BSTR       bstrUnitType,
        __deref_out IXMLDOMElement** ppParameterDef
        );

    HRESULT
    CreateFeature(
        __in_z      CONST BSTR bstrFeatureName,
        __in_z_opt  CONST BSTR bstrDisplayName,
        __deref_out IXMLDOMElement** ppFeatureElement
        );

    HRESULT
    CreateOption(
        __in_z      CONST BSTR bstrOptionName,
        __in_z_opt  CONST BSTR bstrDisplayName,
        __deref_out IXMLDOMElement** ppOptionElement
        );

    HRESULT
    CreateFeatureSelection(
        __in_z      CONST BSTR bstrFeatureName,
        __in_z_opt  CONST BSTR bstrDisplayName,
        __deref_out IXMLDOMElement** ppFeatureElement
        );

    HRESULT
    CreateParameterRef(
        __in_z      CONST BSTR bstrParamRefName,
        __deref_out IXMLDOMElement** ppParamRefElement
        );
};


