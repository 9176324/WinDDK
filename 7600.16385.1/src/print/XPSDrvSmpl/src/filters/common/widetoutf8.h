/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   widetoutf8.h

Abstract:

   The CWideToUTF8 class converts a Unicode character string into the UTF-8 code page format.

--*/

#pragma once

class CWideToUTF8
{
public:
    CWideToUTF8(
        CStringXDW* pcstrWide
        );

    virtual ~CWideToUTF8();

    HRESULT
    GetBuffer(
        __deref_out PVOID* ppBuffer,
        __out       ULONG* pcbBuffer
        );

private:
    VOID
    FreeBuffer();

private:
    CStringXDW* m_pcstrWide;

    PSTR      m_pMultiByte;
};


