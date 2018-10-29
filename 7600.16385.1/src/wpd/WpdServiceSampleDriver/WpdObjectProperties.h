#pragma once

class WpdObjectProperties
{
public:
    WpdObjectProperties();
    virtual ~WpdObjectProperties();

    HRESULT Initialize(__in FakeDevice* pDevice);

    HRESULT DispatchWpdMessage(
                 REFPROPERTYKEY         Command,
        __in     IPortableDeviceValues* pParams,
        __out    IPortableDeviceValues* pResults);

    HRESULT OnGetSupportedProperties(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

    HRESULT OnGetPropertyValues(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

    HRESULT OnGetAllPropertyValues(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

    HRESULT OnSetPropertyValues(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

    HRESULT OnGetPropertyAttributes(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

    HRESULT OnDeleteProperties(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

private:
    FakeDevice* m_pDevice;
};

