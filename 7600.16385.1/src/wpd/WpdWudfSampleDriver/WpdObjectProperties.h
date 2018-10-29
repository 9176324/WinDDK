#pragma once

class WpdObjectProperties
{
public:
    WpdObjectProperties();
    ~WpdObjectProperties();

    HRESULT Initialize(FakeDevice *pFakeDevice);

    HRESULT DispatchWpdMessage(const PROPERTYKEY&   Command,
                               IPortableDeviceValues*      pParams,
                               IPortableDeviceValues*      pResults);

    HRESULT OnGetSupportedProperties(IPortableDeviceValues*  pParams,
                                     IPortableDeviceValues*  pResults);

    HRESULT OnGetValues(IPortableDeviceValues*  pParams,
                        IPortableDeviceValues*  pResults);

    HRESULT OnGetAllValues(IPortableDeviceValues*  pParams,
                           IPortableDeviceValues*  pResults);

    HRESULT OnWriteProperties(IPortableDeviceValues*  pParams,
                              IPortableDeviceValues*  pResults);

    HRESULT OnGetAttributes(IPortableDeviceValues*  pParams,
                            IPortableDeviceValues*  pResults);

    HRESULT OnDelete(IPortableDeviceValues*  pParams,
                     IPortableDeviceValues*  pResults);

private:

    FakeDevice*                     m_pFakeDevice;
};

