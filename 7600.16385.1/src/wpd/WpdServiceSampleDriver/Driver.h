#pragma once

#include "resource.h"
#include "WpdServiceSampleDriver.h"

class ATL_NO_VTABLE CDriver :
    public CComObjectRootEx<CComMultiThreadModel>,
    public CComCoClass<CDriver, &CLSID_WpdServiceSampleDriver>,
    public IDriverEntry,
    public IObjectCleanup
{
public:
    CDriver();

    DECLARE_REGISTRY_RESOURCEID(IDR_WpdServiceSampleDriver)

    DECLARE_NOT_AGGREGATABLE(CDriver)

    BEGIN_COM_MAP(CDriver)
        COM_INTERFACE_ENTRY(IDriverEntry)
    END_COM_MAP()

public:
    //
    // IDriverEntry
    //
    STDMETHOD (OnInitialize)(
        __in IWDFDriver* pDriver
        );
    STDMETHOD (OnDeviceAdd)(
        __in IWDFDriver*             pDriver,
        __in IWDFDeviceInitialize*   pDeviceInit
        );
    STDMETHOD_ (void, OnDeinitialize)(
        __in IWDFDriver* pDriver
        );

    //
    // IObjectCleanup
    //
    STDMETHOD_ (void, OnCleanup)(
        __in IWDFObject* pWdfObject
        );
};

OBJECT_ENTRY_AUTO(__uuidof(WpdServiceSampleDriver), CDriver)


