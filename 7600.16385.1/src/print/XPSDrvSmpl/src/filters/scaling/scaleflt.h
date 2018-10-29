/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   scaleflt.h

Abstract:

   Page Scaling filter defnition. This class derives from the Xps filter
   class and implements the necessary part handlers to support Page Scaling.
   The Page Scaling filter is responsible for adding mark-up to
   the XPS document to allow scaling of pages.

--*/

#pragma once

#include "xdstrmflt.h"
#include "pagescale.h"
#include "gdip.h"

class CPageScalingFilter : public CXDStreamFilter
{
public:
    CPageScalingFilter();

    virtual ~CPageScalingFilter();

    virtual HRESULT
    ProcessFixedPage(
        __in  IXMLDOMDocument2*  pFPPT,
        __in  ISequentialStream* pPageReadStream,
        __out ISequentialStream* pPageWriteStream
        );

private:
    HRESULT
    GetPageScaling(
        __in            IXMLDOMDocument2*  pPrintTicket,
        __in            IXMLDOMDocument2*  pPrintCapabilities,
        __deref_out_opt CPageScaling** ppPageScaling
        );

private:
    GDIPlus                   m_gdiPlus;
};


