#pragma once

class WpdNetworkConfig
{
public:
    WpdNetworkConfig();
    ~WpdNetworkConfig();

    HRESULT Initialize(FakeDevice *pFakeDevice);

    HRESULT DispatchWpdMessage(const PROPERTYKEY&   Command,
                               IPortableDeviceValues*      pParams,
                               IPortableDeviceValues*      pResults);


    HRESULT OnProcessWFCObject(IPortableDeviceValues*  pParams,
                               IPortableDeviceValues*  pResults);
private:
    FakeDevice*                     m_pFakeDevice;
};

