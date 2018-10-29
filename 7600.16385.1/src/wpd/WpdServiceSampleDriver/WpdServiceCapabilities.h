#pragma once

class WpdServiceCapabilities
{
public:
    WpdServiceCapabilities();
    virtual ~WpdServiceCapabilities();

    HRESULT Initialize(__in FakeContactsService* pContactsService);

    HRESULT DispatchWpdMessage(
                REFPROPERTYKEY         Command,
        __in    IPortableDeviceValues* pParams,
        __out   IPortableDeviceValues* pResults);

private:
    HRESULT OnGetSupportedCommands(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

    HRESULT OnGetCommandOptions(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

    HRESULT OnGetSupportedMethods(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

    HRESULT OnGetSupportedMethodsByFormat(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

    HRESULT OnGetMethodAttributes(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

    HRESULT OnGetMethodParameterAttributes(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

    HRESULT OnGetSupportedFormats(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

    HRESULT OnGetFormatAttributes(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

    HRESULT OnGetSupportedFormatProperties(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

    HRESULT OnGetFormatPropertyAttributes(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

    HRESULT OnGetSupportedEvents(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

    HRESULT OnGetEventAttributes(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

    HRESULT OnGetEventParameterAttributes(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

    HRESULT OnGetInheritedServices(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

private:
    FakeContactsService* m_pContactsService;
};


