/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   xpsfiler.h

Abstract:

   Definition of the XPS file reader class. This class implements
   ISequentialStream::Read by using the IPKFile interface to supply the
   client with decompressed data as requested. This allows the file to
   be passed directly to a SAX or DOM handler for parsing.

--*/

#pragma once

#include "xps.h"
#include "ipkarch.h"
#include "workbuff.h"
#include "cunknown.h"

class CXPSReadFile : public CUnknown<ISequentialStream>
{
public:
    CXPSReadFile();

    virtual ~CXPSReadFile();

    //
    // ISequentialStream members
    //
    HRESULT STDMETHODCALLTYPE
    Read(
        __out_bcount(cb) void*  pv,
        __in             ULONG  cb,
        __out            ULONG* pcbRead
        );

    HRESULT STDMETHODCALLTYPE
    Write(
        CONST void*,
        ULONG,
        ULONG*
        );

    HRESULT
    Open(
        __in PCSTR szFileName
        );

    HRESULT
    Close(
        VOID
        );

    HRESULT
    AddFilePart(
        __in PCSTR          szFileName,
        __in CONST IPKFile* pPkFile
        );

    HRESULT
    GetFileParts(
        __deref_out XPSPartStack** ppPartStack
        );

    HRESULT
    GetBuffer(
        __deref_out_bcount(*pcb) PVOID* ppv,
        __out                    ULONG* pcb
        );

    HRESULT
    GetFileName(
        __deref_out PSTR* pszFileName
        );

private:
    HRESULT
    DecompressNextFile(
        VOID
        );

    HRESULT
    Decompress(
        __in CONST IPKFile* pRecord
        );

    HRESULT
    CopyBuffer(
        __out_bcount(cb) void*  pv,
        __in             ULONG  cb,
        __out            ULONG* pcbRead
        );

private:
    CWorkingBuffer            m_workFile;

    CStringXDA                m_cstrFileName;

    XPSPartStack              m_partStack;

    SIZE_T                    m_cbSent;

    SIZE_T                    m_cbExtracted;

    BOOL                      m_bExtractedAll;
};


