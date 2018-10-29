#pragma once

class WpdObjectManagement
{
public:
    WpdObjectManagement();
    ~WpdObjectManagement();

    HRESULT Initialize(__in FakeDevice* m_pDevice);

    HRESULT DispatchWpdMessage(
                 REFPROPERTYKEY         Command,
        __in     IPortableDeviceValues* pParams,
        __out    IPortableDeviceValues* pResults);


    HRESULT OnCreateObjectWithPropertiesOnly(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

    HRESULT OnDelete(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

private:
    FakeDevice* m_pDevice;
};

