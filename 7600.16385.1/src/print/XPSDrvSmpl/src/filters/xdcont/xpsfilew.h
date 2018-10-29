/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   xpsfilew.h

Abstract:

   Definition of an XPS file writer. This implements ISequentialStream::Write
   and essentially wraps a buffer that recieves and stores the part information
   so that in can be later compressed and written out.

--*/

#pragma once

#include "workbuff.h"
#include "cunknown.h"
#include "xdstring.h"

class CXPSWriteFile : public CUnknown<ISequentialStream>
{
public:
    CXPSWriteFile();

    CXPSWriteFile(
        PCSTR szFileName
        );

    virtual ~CXPSWriteFile();

    //
    // ISequentialStream members
    //
    HRESULT STDMETHODCALLTYPE
    Read(
        void*,
        ULONG,
        ULONG*
        );

    HRESULT STDMETHODCALLTYPE
    Write(
        __in_bcount(cbData) CONST void* pData,
        __in                ULONG       cbData,
        __out               ULONG*      pcbWritten
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
    CWorkingBuffer m_workFile;

    ULONG          m_cbWritten;

    CStringXDA       m_cstrFileName;
};


