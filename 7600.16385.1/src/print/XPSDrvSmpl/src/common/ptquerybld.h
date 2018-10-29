/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   ptquerybld.h

Abstract:

   PrintTicket XPath query builder definition. The CPTQueryBuilder class
   provides a means of constructing PrintTicket specific XPath queries for
   retrieving nodes from within the DOM document describing the PrintTicket.

--*/

#pragma once

class CPTQueryBuilder
{
public:
    CPTQueryBuilder(
        __in_z BSTR bstrFrameworkNS
        );

    CPTQueryBuilder(
        __in_z BSTR bstrFrameworkNS,
        __in_z BSTR bstrQuery
        );

    virtual ~CPTQueryBuilder();

    HRESULT
    Clear(
        VOID
        );

    HRESULT
    GetQuery(
        __deref_out BSTR*  bstrQuery
        );

    HRESULT
    AddFeature(
        __in_z_opt BSTR  bstrKeywordNS,
        __in_z     BSTR  bstrPropertyName
        );

    HRESULT
    AddProperty(
        __in_z_opt BSTR  bstrKeywordNS,
        __in_z     BSTR  bstrPropertyName
        );

    HRESULT
    AddScoredProperty(
        __in_z_opt BSTR  bstrKeywordNS,
        __in_z     BSTR  bstrPropertyName
        );

    HRESULT
    AddParamRef(
        __in_z_opt BSTR  bstrKeywordNS,
        __in_z     BSTR  bstrParameterRef
        );

    HRESULT
    AddOption(
        __in_z_opt BSTR  bstrKeywordNS
        );

private:
    CComBSTR    m_bstrQuery;

    CComBSTR    m_bstrFrameworkNS;
};


