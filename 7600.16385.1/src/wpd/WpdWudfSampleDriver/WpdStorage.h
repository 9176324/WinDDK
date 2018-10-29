#pragma once

class WpdStorage
{
public:
    WpdStorage();
    ~WpdStorage();

    HRESULT Initialize(FakeDevice *pFakeDevice);

    HRESULT DispatchWpdMessage(const PROPERTYKEY&   Command,
                               IPortableDeviceValues*      pParams,
                               IPortableDeviceValues*      pResults);

    HRESULT OnFormat(IPortableDeviceValues*  pParams,
                     IPortableDeviceValues*  pResults);

    HRESULT OnMoveObject(IPortableDeviceValues*  pParams,
                         IPortableDeviceValues*  pResults);

private:

    FakeDevice*                     m_pFakeDevice;
};

