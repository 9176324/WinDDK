#pragma once

class WpdService
{
public:
    WpdService();
    virtual ~WpdService();

    HRESULT Initialize(__in FakeDevice* pDevice);

    HRESULT DispatchWpdMessage(
                REFPROPERTYKEY         Command,
        __in    IPortableDeviceValues* pParams,
        __out   IPortableDeviceValues* pResults);

private:
    HRESULT OnGetServiceObjectID(
        __in    LPCWSTR                pszRequestFilename,
        __in    IPortableDeviceValues* pParams,
        __out   IPortableDeviceValues* pResults);

    HRESULT CheckRequestFilename(
        __in    LPCWSTR                 pszRequestFilename);

private:
    WpdServiceMethods       m_ServiceMethods;
    WpdServiceCapabilities  m_ServiceCapabilities;
    FakeContactsService*    m_pContactsService;
};


