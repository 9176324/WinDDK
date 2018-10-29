/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   xpsarch.h

Abstract:

   Definition of the XPS archive class. This class is responsible for providing
   an interface to clients that removes the potentially interleaved nature of an
   XPS document. This allows clients to manipulate files using the parts full name
   instead of having to worry about the semantics of interleaved parts.

--*/

#pragma once

#include "ipkarch.h"
#include "xpsfiler.h"

class CXPSArchive
{
public:
    CXPSArchive(
        __in IPrintReadStream*  pReadStream,
        __in IPrintWriteStream* pWriteStream
        );

    virtual ~CXPSArchive();

    HRESULT
    InitialiseFile(
        __in_z PCSTR szFileName
        );

    HRESULT
    GetFileStream(
        __in_z      PCSTR               szFileName,
        __deref_out ISequentialStream** ppFileStream
        );

    HRESULT
    CloseCurrent(
        VOID
        );

    HRESULT
    SendCurrentFile(
        __in ECompressionType eCompType
        );

    HRESULT
    SendCurrentFile(
        VOID
        );

    HRESULT
    SendFile(
        __in_z                PCSTR            szFileName,
        __in_bcount(cbBuffer) PVOID            pBuffer,
        __in                  ULONG            cbBuffer,
        __in                  ECompressionType eCompType
        );

private:
    HRESULT
    AddFile(
        __in_z PCSTR szFileName
        );

private:
    HMODULE                    m_hPkArch;

    CComPtr<IPKArchive>        m_pPkArchive;

    CXPSReadFile               m_XpsFile;

    SentList                   m_sentList;

    CComPtr<IPrintWriteStream> m_pWriteStream;
};


