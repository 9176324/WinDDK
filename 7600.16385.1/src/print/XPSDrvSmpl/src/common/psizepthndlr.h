/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   psizepthndlr.h

Abstract:

   PageMediaSize PrintTicket handling definition. The PageMediaSize PT handler
   is used to extract PageMediaSize settings from a PrintTicket and populate
   the PageMediaSize data structure with the retrieved settings. The class also
   defines a method for setting the feature in the PrintTicket given the
   data structure.

--*/

#pragma once

#include "pthndlr.h"
#include "psizedata.h"

class CPageSizePTHandler : public CPTHandler
{
public:
    CPageSizePTHandler(
        __in IXMLDOMDocument2* pPrintTicket
        );

    virtual ~CPageSizePTHandler();

    HRESULT
    GetData(
        __out XDPrintSchema::PageMediaSize::PageMediaSizeData* pPageMediaSizeData
        );
};


