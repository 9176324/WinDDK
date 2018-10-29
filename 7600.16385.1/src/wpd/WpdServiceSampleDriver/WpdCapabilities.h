#pragma once

class WpdCapabilities
{
public:
    WpdCapabilities();
    virtual ~WpdCapabilities();

    HRESULT Initialize(__in FakeDevice* pDevice);

    HRESULT DispatchWpdMessage(
                REFPROPERTYKEY          Command,
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

    HRESULT OnGetSupportedCommands(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

    HRESULT OnGetCommandOptions(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

    HRESULT OnGetFunctionalCategories(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

    HRESULT OnGetFunctionalObjects(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

    HRESULT OnGetSupportedContentTypes(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

    HRESULT OnGetSupportedFormats(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

    HRESULT OnGetSupportedFormatProperties(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

    HRESULT OnGetFixedPropertyAttributes(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

    HRESULT OnGetSupportedEvents(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

    HRESULT OnGetEventOptions(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

private:
    FakeDevice* m_pDevice;
};


