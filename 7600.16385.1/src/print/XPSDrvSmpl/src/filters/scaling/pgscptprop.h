/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   pgscptprop.h

Abstract:

   Page Scaling properties class definition. The Page Scaling properties class
   is responsible for holding and controling Page Scaling properties.

--*/

#pragma once

#include "pgscdata.h"
#include "psizedata.h"
#include "pimagedata.h"
#include "porientdata.h"

class CPGSCPTProperties
{
public:
    CPGSCPTProperties(
        __in CONST XDPrintSchema::PageScaling::PageScalingData&         pgscData,
        __in CONST XDPrintSchema::PageMediaSize::PageMediaSizeData&     psizeData,
        __in CONST XDPrintSchema::PageImageableSize::PageImageableData& pimageableData,
        __in CONST XDPrintSchema::PageOrientation::PageOrientationData& pageOrientData
        );

    virtual ~CPGSCPTProperties();

    HRESULT
    GetOption(
        __out XDPrintSchema::PageScaling::EScaleOption* pScaleOption
        );

    HRESULT
    GetWidthOffset(
        __out REAL* pWidthOffset
        );

    HRESULT
    GetHeightOffset(
        __out REAL* pHeightOffset
        );

    HRESULT
    GetWidthScale(
        __out REAL* pWidthScale
        );

    HRESULT
    GetHeightScale(
        __out REAL* pHeightScale
        );

    HRESULT
    GetOffsetOption(
        __out XDPrintSchema::PageScaling::OffsetAlignment::EScaleOffsetOption* pOffsetOption
        );

    HRESULT
    GetPageSize(
        __out SizeF* pSizePage
    );

    HRESULT
    GetImageableRect(
        __out RectF* pImageableRect
        );

protected:
    XDPrintSchema::PageScaling::PageScalingData         m_pgscData;

    XDPrintSchema::PageMediaSize::PageMediaSizeData     m_pageMediaSizeData;

    XDPrintSchema::PageImageableSize::PageImageableData m_pageImageableData;

    XDPrintSchema::PageOrientation::PageOrientationData m_pageOrientData;
};


