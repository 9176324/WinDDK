/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   xpsproc.h

Abstract:

   Definition of the XPS processor. This class is responsible for handling
   the logical document and page structure of the XPS container. The class starts
   with the content types part and .rels part to find the Fixed Document Sequence.
   It then parses the FDS to find the Fixed Documents and finally the FDs to find
   each Fixed Page. Each fixed page is then reported to a client FixedPageProcessor
   to interpret the  mark-up and write out modifications as required. The class is
   additionally responsible for ensuring all XPS parts and accompanying resources are
   sent on to the PK archive handler to be written out.

--*/

#pragma once

#include "xps.h"
#include "xpsarch.h"
#include "xpstype.h"
#include "xpsrels.h"
#include "xpsfds.h"
#include "xpsfd.h"
#include "ptmanage.h"

class IFixedPageProcessor
{
public:
    IFixedPageProcessor(){}

    virtual ~IFixedPageProcessor(){}

    virtual HRESULT
    ProcessFixedPage(
        __in  IXMLDOMDocument2*  pFPPT,
        __in  ISequentialStream* pPageReadStream,
        __out ISequentialStream* pPageWriteStream
        ) = 0;
};

class CXPSProcessor
{
public:
    CXPSProcessor(
        __in IPrintReadStream*          pReadStream,
        __in IPrintWriteStream*         pWriteStream,
        __in IFixedPageProcessor*       pPageProcessor,
        __in IPrintPipelinePropertyBag* pPropertyBag,
        __in CPTManager*                pPtManager
        );

    virtual ~CXPSProcessor();

    HRESULT
    Start(
        VOID
        );

private:
    HRESULT
    GetFixedDocumentSequenceParts(
        __out FileList* pFixedDocumentSequenceParts
        );

    HRESULT
    GetFixedDocumentParts(
        __in  PCSTR     szFixedDocSeq,
        __out FileList* pFixedDocumentParts
        );

    HRESULT
    GetFixedPageParts(
        __in  PCSTR     szFixedDoc,
        __out FileList* pFixedPageParts
        );

    HRESULT
    GetRelsForPart(
        __in PCSTR szPartName
        );

    HRESULT
    MakeRelsPartName(
        __in  PCSTR     szPartName,
        __out CStringXDA* pcstrRelsPartName
        );

    HRESULT
    ProcessRelsParts(
        __in PCSTR              szPartName,
        __in CONST EContentType contentType
        );

    HRESULT
    AddPrintTicket(
        __in PCSTR              szPartName,
        __in CONST EContentType eContentType
        );

    HRESULT
    ProcessFixedPage(
        __in PCSTR szPartName
        );

private:
    CXPSArchive                        m_xpsArchive;

    CContentTypes                      m_contentTypes;

    CRels                              m_rels;

    CFixedDocumentSequence             m_fixedDocSeq;

    CFixedDocument                     m_fixedDoc;

    CComPtr<ISAXXMLReader>             m_pSaxRdr;

    IFixedPageProcessor*               m_pPageProcessor;

    CPTManager*                        m_pPtManager;

    CComPtr<IPrintPipelinePropertyBag> m_pPrintPropertyBag;
};


