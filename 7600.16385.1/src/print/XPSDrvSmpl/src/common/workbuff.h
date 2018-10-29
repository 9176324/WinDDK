/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   workbuff.h

Abstract:

   CWorkingBuffer class definition. This class provides a means of working
   with a local buffer by defining a simple interface that allows the retrieval
   of a buffer of an appropriate size, handling reallocation as necessary.

--*/

#pragma once

class CWorkingBuffer
{
public:
    CWorkingBuffer();

    virtual ~CWorkingBuffer();

    HRESULT
    GetBuffer(
        __in                       CONST ULONGLONG cbSize,
        __deref_out_bcount(cbSize) PVOID*          ppBuffer
        );

    HRESULT
    GetBufferAt(
        __in                       ULONG           cbOffset,
        __in                       CONST ULONGLONG cbSize,
        __deref_out_bcount(cbSize) PVOID*          ppBuffer
        );

private:
    __field_bcount(m_cbSize) PVOID  m_pBuffer;

    SIZE_T m_cbSize;
};


