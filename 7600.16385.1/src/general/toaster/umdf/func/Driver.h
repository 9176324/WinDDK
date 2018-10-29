/*++

  Copyright (c) Microsoft Corporation, All Rights Reserved

  Module Name:

    Driver.h

  Abstract:

    This file contains the class definition for the driver object.

  Environment:

    Windows User-Mode Driver Framework (WUDF)

--*/

#pragma once
#include "resource.h"       
#include "WUDFToaster.h"

class ATL_NO_VTABLE CDriver : 
    public CComObjectRootEx<CComMultiThreadModel>,
    public CComCoClass<CDriver, &CLSID_WUDFToaster>,
    public IDriverEntry
{
public:
    CDriver()
    {
    }
        
    DECLARE_REGISTRY_RESOURCEID(IDR_TOASTERDRIVER)
    DECLARE_NOT_AGGREGATABLE(CDriver)

    //
    // The driver object suppports the IDriverEntry interface.
    // 
    BEGIN_COM_MAP(CDriver)
        COM_INTERFACE_ENTRY(IDriverEntry)
    END_COM_MAP()

public:
    //   
    // IDriverEntry
    //
    STDMETHOD (OnDeviceAdd)(
        __in IWDFDriver           *pDriver,
        __in IWDFDeviceInitialize *pDeviceInit
        );
    STDMETHOD (OnInitialize)(
        __in IWDFDriver* pDriver
        );
    STDMETHOD_ (void, OnDeinitialize)(
        __in IWDFDriver* pDriver
        );
};

OBJECT_ENTRY_AUTO(__uuidof(WUDFToaster), CDriver)

