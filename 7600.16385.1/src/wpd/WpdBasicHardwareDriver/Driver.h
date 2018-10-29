#pragma once

class ATL_NO_VTABLE CDriver :
    public CComObjectRootEx<CComMultiThreadModel>,
    public CComCoClass<CDriver, &CLSID_WpdBasicHardwareDriver>,
    public IDriverEntry,
    public IObjectCleanup
{
public:
    CDriver();

    DECLARE_REGISTRY_RESOURCEID(IDR_WpdBasicHardwareDriver)

    DECLARE_NOT_AGGREGATABLE(CDriver)

    BEGIN_COM_MAP(CDriver)
        COM_INTERFACE_ENTRY(IDriverEntry)
    END_COM_MAP()

public:
    //
    // IDriverEntry
    //
    STDMETHOD (OnInitialize)(
        IWDFDriver* pDriver
        );
    STDMETHOD (OnDeviceAdd)(
        IWDFDriver*             pDriver,
        IWDFDeviceInitialize*   pDeviceInit
        );
    STDMETHOD_ (void, OnDeinitialize)(
        IWDFDriver* pDriver
        );

    //
    // IObjectCleanup
    //
    STDMETHOD_ (void, OnCleanup)(
        IWDFObject* pWdfObject
        );
};

OBJECT_ENTRY_AUTO(__uuidof(WpdBasicHardwareDriver), CDriver)


