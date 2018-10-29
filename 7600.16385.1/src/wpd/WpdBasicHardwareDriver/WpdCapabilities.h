#pragma once

class WpdCapabilities
{
public:
    WpdCapabilities();
    virtual ~WpdCapabilities();

    HRESULT Initialize();

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

    HRESULT OnGetSupportedEvents(
        IPortableDeviceValues*  pParams,
        IPortableDeviceValues*  pResults);

    HRESULT OnGetEventOptions(
        IPortableDeviceValues*  pParams,
        IPortableDeviceValues*  pResults);

};


