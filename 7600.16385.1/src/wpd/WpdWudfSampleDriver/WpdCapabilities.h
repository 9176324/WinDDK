#pragma once

class WpdCapabilities
{
public:
    WpdCapabilities();
    ~WpdCapabilities();

    HRESULT Initialize(
        FakeDevice *pFakeDevice);

    HRESULT DispatchWpdMessage(
        const PROPERTYKEY&   Command,
        IPortableDeviceValues*      pParams,
        IPortableDeviceValues*      pResults);

    HRESULT OnGetSupportedCommands(
        IPortableDeviceValues*  pParams,
        IPortableDeviceValues*  pResults);

    HRESULT OnGetCommandOptions(
        IPortableDeviceValues*  pParams,
        IPortableDeviceValues*  pResults);

    HRESULT OnGetFunctionalCategories(
        IPortableDeviceValues*  pParams,
        IPortableDeviceValues*  pResults);

    HRESULT OnGetFunctionalObjects(
        IPortableDeviceValues*  pParams,
        IPortableDeviceValues*  pResults);

    HRESULT OnGetSupportedContentTypes(
        IPortableDeviceValues*  pParams,
        IPortableDeviceValues*  pResults);

    HRESULT OnGetSupportedFormats(
        IPortableDeviceValues*  pParams,
        IPortableDeviceValues*  pResults);

    HRESULT OnGetSupportedFormatProperties(
        IPortableDeviceValues*  pParams,
        IPortableDeviceValues*  pResults);

    HRESULT OnGetFixedPropertyAttributes(
        IPortableDeviceValues*  pParams,
        IPortableDeviceValues*  pResults);

    HRESULT OnGetSupportedEvents(
        IPortableDeviceValues*  pParams,
        IPortableDeviceValues*  pResults);

    HRESULT OnGetEventOptions(
        IPortableDeviceValues*  pParams,
        IPortableDeviceValues*  pResults);

private:

    FakeDevice*                     m_pFakeDevice;
};


