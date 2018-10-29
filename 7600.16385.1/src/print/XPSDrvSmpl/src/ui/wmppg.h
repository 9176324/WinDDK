/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   wmppg.h

Abstract:

   Definiiion of the watermark property page. This class is
   responsible for initialising and registering the color management
   property page and its controls.

--*/

#pragma once

#include "precomp.h"
#include "docppg.h"

class CWatermarkPropPage : public CDocPropPage
{
public:
    CWatermarkPropPage();

    virtual ~CWatermarkPropPage();

    HRESULT
    InitDlgBox(
        __out LPCTSTR* ppszTemplate,
        __out LPCTSTR* ppszTitle
        );
};


