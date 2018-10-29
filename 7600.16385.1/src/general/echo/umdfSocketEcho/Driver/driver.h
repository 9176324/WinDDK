/*++

Copyright (C) Microsoft Corporation, All Rights Reserved

Module Name:

    Driver.h

Abstract:

    This module contains the type definitions for the UMDF Socketecho sample's
    driver callback class.

Environment:

    Windows User-Mode Driver Framework (WUDF)

--*/

#pragma once

//
// This class handles driver events for the socktecho sample.  In particular
// it supports the OnDeviceAdd event, which occurs when the driver is called
// to setup per-device handlers for a new device stack.
//

extern const GUID CLSID_MyDriverCoClass;

class ATL_NO_VTABLE CMyDriver : 
    public CComObjectRootEx<CComMultiThreadModel>,
    public CComCoClass<CMyDriver, &CLSID_MyDriverCoClass>,
    public IDriverEntry
{
public:

DECLARE_NOT_AGGREGATABLE(CMyDriver) 

DECLARE_CLASSFACTORY();
    
DECLARE_NO_REGISTRY();
                                  
BEGIN_COM_MAP(CMyDriver)
    COM_INTERFACE_ENTRY(IDriverEntry)
END_COM_MAP()

public:
    // IDriverEntry
    STDMETHOD(OnInitialize)(__in IWDFDriver* pWdfDriver);
    STDMETHOD(OnDeviceAdd)(__in IWDFDriver* pWdfDriver, __in IWDFDeviceInitialize* pWdfDeviceInit);
    STDMETHOD_(void,OnDeinitialize)(__in IWDFDriver* pWdfDriver);
};


