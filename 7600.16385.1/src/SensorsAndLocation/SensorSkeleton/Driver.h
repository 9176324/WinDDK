/*++
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.

Copyright (C) Microsoft Corporation, All Rights Reserved

Module Name:

    Driver.h

Abstract:

    This module contains the type definitions for the Sensor Skeleton sample's
    driver callback class.
--*/

#pragma once

//
// This class handles driver events for the skeleton sample.  In particular
// it supports the OnDeviceAdd event, which occurs when the driver is called
// to setup per-device handlers for a new device stack.
//

class ATL_NO_VTABLE CMyDriver :
    public CComObjectRootEx<CComMultiThreadModel>,
    public CComCoClass<CMyDriver, &CLSID_SensorsSkeletonDriver>,
    public IDriverEntry
{
public:
    CMyDriver();

    DECLARE_NO_REGISTRY()
    DECLARE_CLASSFACTORY()
    DECLARE_NOT_AGGREGATABLE(CMyDriver)

    BEGIN_COM_MAP(CMyDriver)
        COM_INTERFACE_ENTRY(IDriverEntry)
    END_COM_MAP()

public:
    //
    // IDriverEntry
    //
    STDMETHOD  (OnInitialize)(__in IWDFDriver* pDriver);
    STDMETHOD  (OnDeviceAdd)(__in IWDFDriver* pDriver, __in IWDFDeviceInitialize* pDeviceInit);
    STDMETHOD_ (void, OnDeinitialize)(__in IWDFDriver* pDriver);
};

OBJECT_ENTRY_AUTO(__uuidof(SensorsSkeletonDriver), CMyDriver)

