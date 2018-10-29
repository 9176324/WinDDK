// Device.cpp : Implementation of CDevice

#include "stdafx.h"
#include "Device.h"
#include "WpdWudfSampleDriver_i.c"

// CDevice
//

STDMETHODIMP_(HRESULT)
CDevice::OnD0Entry(IWDFDevice* /*pDevice*/,
                   WDF_POWER_DEVICE_STATE /*previousState*/)
{

    HRESULT   hr     = S_OK;
    return hr;
}

STDMETHODIMP_(HRESULT)
CDevice::OnD0Exit(IWDFDevice* /*pDevice*/,
                  WDF_POWER_DEVICE_STATE /*newState*/)
{
    return S_OK;
}


STDMETHODIMP_(VOID)
CDevice::OnSurpriseRemoval(IWDFDevice* /*pDevice*/)
{
    return;
}

STDMETHODIMP_(HRESULT)
CDevice::OnQueryRemove(IWDFDevice* /*pDevice*/)
{

    return S_OK;
}

STDMETHODIMP_(HRESULT)
CDevice::OnQueryStop(IWDFDevice* /*pDevice*/)
{

    return S_OK;
}



STDMETHODIMP_(VOID)
CDevice::OnSelfManagedIoCleanup(IWDFDevice* /*pDevice*/)
{
    return;
}


STDMETHODIMP_(VOID)
CDevice::OnSelfManagedIoFlush(IWDFDevice* /*pDevice*/)
{
    return;
}


STDMETHODIMP_(HRESULT)
CDevice::OnSelfManagedIoInit(IWDFDevice* /*pDevice*/)
{
    return S_OK;
}

STDMETHODIMP_(HRESULT)
CDevice::OnSelfManagedIoSuspend(IWDFDevice* /*pDevice*/)
{
    return S_OK;
}

STDMETHODIMP_(HRESULT)
CDevice::OnSelfManagedIoRestart(IWDFDevice* /*pDevice*/)
{
    return S_OK;
}

STDMETHODIMP_(HRESULT)
CDevice::OnSelfManagedIoStop(IWDFDevice* /*pDevice*/)
{
    return S_OK;
}


STDMETHODIMP_(HRESULT)
CDevice::OnPrepareHardware(IWDFDevice* pDevice)
{
    HRESULT hr = S_OK;

    if (m_pPortableDeviceClassExtension == NULL)
    {
        hr = CoCreateInstance(CLSID_PortableDeviceClassExtension,
                              NULL,
                              CLSCTX_INPROC_SERVER,
                              IID_IPortableDeviceClassExtension,
                              (VOID**)&m_pPortableDeviceClassExtension);
        CHECK_HR(hr, "Failed to CoCreate CLSID_PortableDeviceClassLibrary");

        // Initialize the WPD Class Extension. This will enable the appropriate WPD interface GUID,
        // as well as do any additional initialization (e.g. enabling Legacy Compatibility layers for those drivers
        // which requsted support in their INF).
        if (hr == S_OK)
        {
            CComPtr<IPortableDeviceValues>                pOptions;
            CComPtr<IPortableDevicePropVariantCollection> pContentTypes;

            hr = CoCreateInstance(CLSID_PortableDeviceValues,
                                  NULL,
                                  CLSCTX_INPROC_SERVER,
                                  IID_IPortableDeviceValues,
                                  (VOID**)&pOptions);
            CHECK_HR(hr, "Failed to CoCreate CLSID_PortableDeviceClassLibrary");

            if (hr == S_OK)
            {
                hr = GetSupportedContentTypes(&pContentTypes);
                CHECK_HR(hr, "Failed to get supported content types");

                // Add the supported types to the options
                if (hr == S_OK)
                {
                    hr = pOptions->SetIPortableDevicePropVariantCollectionValue(WPD_CLASS_EXTENSION_OPTIONS_SUPPORTED_CONTENT_TYPES, pContentTypes);
                    CHECK_HR(hr, "Failed to set WPD_CLASS_EXTENSION_OPTIONS_SUPPORTED_CONTENT_TYPES");
                }

                if (hr == S_OK)
                {
                    hr = m_pPortableDeviceClassExtension->Initialize(pDevice, pOptions);
                    CHECK_HR(hr, "Failed to Initialize portable device class extension object");
                }
            }
        }

        if (hr == S_OK)
        {
            DWORD   dwLength        = 0;
            WCHAR*  pszDeviceName   = NULL;
            hr = pDevice->RetrieveDeviceName(NULL, &dwLength);
            if(dwLength > 0)
            {
                pszDeviceName = new WCHAR[dwLength + 1];
                if(pszDeviceName)
                {
                    DWORD dwLengthTemp = dwLength;
                    hr = pDevice->RetrieveDeviceName(pszDeviceName, &dwLengthTemp);
                    CHECK_HR(hr, "Failed to get device name");
                    if (hr == S_OK)
                    {
                        pszDeviceName[dwLength] = L'\0';
                        if (m_pWpdBaseDriver != NULL)
                        {
                            hr = m_pWpdBaseDriver->Initialize(pszDeviceName, m_pPortableDeviceClassExtension);
                            CHECK_HR(hr, "Failed to initialize the fake device");
                        }
                        else
                        {
                            hr = E_UNEXPECTED;
                            CHECK_HR(hr, "NULL driver class used, Driver may not be initialized");
                        }
                    }
                    delete[] pszDeviceName;
                    pszDeviceName = NULL;
                }
                else
                {
                    hr = E_OUTOFMEMORY;
                    CHECK_HR(hr, "Failed to allocate memory for device name");
                }
            }
            else
            {
                CHECK_HR(hr, "Failed to get device name length");
            }
        }

        // Send the latest device friendly name to the Portable Device class extension library to process
        if (hr == S_OK)
        {
            LPWSTR pwszDeviceFriendlyName = NULL;

            HRESULT hrTemp = GetDeviceFriendlyName(&pwszDeviceFriendlyName);
            CHECK_HR(hrTemp, "Failed to get the device friendly name");

            if (hrTemp == S_OK)
            {
                hrTemp = UpdateDeviceFriendlyName(m_pPortableDeviceClassExtension, pwszDeviceFriendlyName);
                CHECK_HR(hrTemp, "Failed to update device friendly name information");
            }

            CoTaskMemFree(pwszDeviceFriendlyName);
            pwszDeviceFriendlyName = NULL;
        }
    }
    return hr;
}

STDMETHODIMP_(HRESULT)
CDevice::OnReleaseHardware(IWDFDevice* pDevice)
{
    UNREFERENCED_PARAMETER(pDevice);
    if (m_pWpdBaseDriver != NULL)
    {
        m_pWpdBaseDriver->Uninitialize();
    }

    if (m_pPortableDeviceClassExtension != NULL)
    {
        m_pPortableDeviceClassExtension = NULL;
    }

    return S_OK;
}

HRESULT CDevice::GetSupportedContentTypes(
    IPortableDevicePropVariantCollection** ppContentTypes)
{
    HRESULT hr = S_OK;

    if(ppContentTypes)
    {
        CComPtr<IPortableDeviceValues>  pParams;
        CComPtr<IPortableDeviceValues>  pResults;

        // Prepare to make a call to query for the content types
        hr = CoCreateInstance(CLSID_PortableDeviceValues,
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_IPortableDeviceValues,
            (VOID**)&pParams);
        CHECK_HR(hr, "Failed to CoCreate CLSID_PortableDeviceValues");

        if (hr == S_OK)
        {
            hr = CoCreateInstance(CLSID_PortableDeviceValues,
                NULL,
                CLSCTX_INPROC_SERVER,
                IID_IPortableDeviceValues,
                (VOID**)&pResults);
            CHECK_HR(hr, "Failed to CoCreate CLSID_PortableDeviceValues for results");
        }

        // Set the params
        if (hr == S_OK)
        {
            hr = pParams->SetGuidValue(WPD_PROPERTY_COMMON_COMMAND_CATEGORY, WPD_COMMAND_CAPABILITIES_GET_SUPPORTED_CONTENT_TYPES.fmtid);
            CHECK_HR(hr, ("Failed to set WPD_PROPERTY_COMMON_COMMAND_CATEGORY"));
        }
        if (hr == S_OK)
        {
            hr = pParams->SetUnsignedIntegerValue(WPD_PROPERTY_COMMON_COMMAND_ID, WPD_COMMAND_CAPABILITIES_GET_SUPPORTED_CONTENT_TYPES.pid);
            CHECK_HR(hr, ("Failed to set WPD_PROPERTY_COMMON_COMMAND_ID"));
        }
        if (hr == S_OK)
        {
            hr = pParams->SetGuidValue(WPD_PROPERTY_CAPABILITIES_FUNCTIONAL_CATEGORY, WPD_FUNCTIONAL_CATEGORY_ALL);
            CHECK_HR(hr, ("Failed to set WPD_PROPERTY_CAPABILITIES_FUNCTIONAL_CATEGORY"));
        }

        // Make the call
        if (hr == S_OK)
        {
            hr = m_pWpdBaseDriver->DispatchWpdMessage(pParams, pResults);
            CHECK_HR(hr, ("Failed to dispatch message to get supported content types"));
        }

        if (hr == S_OK)
        {
            hr = pResults->GetIPortableDevicePropVariantCollectionValue(WPD_PROPERTY_CAPABILITIES_CONTENT_TYPES, ppContentTypes);
            CHECK_HR(hr, ("Failed to get WPD_PROPERTY_CAPABILITIES_CONTENT_TYPES"));
        }
    }

    return hr;
}

HRESULT CDevice::GetDeviceFriendlyName(
    __deref_out_opt LPWSTR*       pwszDeviceFriendlyName)
{
    HRESULT hr = S_OK;

    CComPtr<IPortableDeviceValues>        pParams;
    CComPtr<IPortableDeviceValues>        pResults;
    CComPtr<IPortableDeviceKeyCollection> pKeys;
    CComPtr<IPortableDeviceValues>        pValues;

    if (pwszDeviceFriendlyName == NULL)
    {
        hr = E_POINTER;
        CHECK_HR(hr, "Cannot have NULL parameter");
        return hr;
    }

    *pwszDeviceFriendlyName = NULL;

    // Prepare to make a call to query for the device friendly name
    if (hr == S_OK)
    {
        hr = CoCreateInstance(CLSID_PortableDeviceValues, NULL, CLSCTX_INPROC_SERVER, IID_IPortableDeviceValues, (VOID**)&pParams);
        CHECK_HR(hr, "Failed to CoCreate CLSID_PortableDeviceValues");
    }

    if (hr == S_OK)
    {
        hr = CoCreateInstance(CLSID_PortableDeviceValues, NULL, CLSCTX_INPROC_SERVER, IID_IPortableDeviceValues, (VOID**)&pResults);
        CHECK_HR(hr, "Failed to CoCreate CLSID_PortableDeviceValues");
    }

    if (hr == S_OK)
    {
        hr = CoCreateInstance(CLSID_PortableDeviceKeyCollection, NULL, CLSCTX_INPROC_SERVER, IID_IPortableDeviceKeyCollection, (VOID**)&pKeys);
        CHECK_HR(hr, "Failed to CoCreate CLSID_PortableDeviceKeyCollection for results");
    }

    // Set the params
    if (hr == S_OK)
    {
        hr = pParams->SetGuidValue(WPD_PROPERTY_COMMON_COMMAND_CATEGORY, WPD_COMMAND_OBJECT_PROPERTIES_GET.fmtid);
        CHECK_HR(hr, ("Failed to set WPD_PROPERTY_COMMON_COMMAND_CATEGORY"));
    }

    if (hr == S_OK)
    {
        hr = pParams->SetUnsignedIntegerValue(WPD_PROPERTY_COMMON_COMMAND_ID, WPD_COMMAND_OBJECT_PROPERTIES_GET.pid);
        CHECK_HR(hr, ("Failed to set WPD_PROPERTY_COMMON_COMMAND_ID"));
    }

    if (hr == S_OK)
    {
        hr = pParams->SetStringValue(WPD_PROPERTY_OBJECT_PROPERTIES_OBJECT_ID, WPD_DEVICE_OBJECT_ID);
        CHECK_HR(hr, ("Failed to set WPD_PROPERTY_OBJECT_PROPERTIES_OBJECT_ID"));
    }

    if (hr == S_OK)
    {
        hr = pKeys->Add(WPD_DEVICE_FRIENDLY_NAME);
        CHECK_HR(hr, ("Failed to add WPD_DEVICE_FRIENDLY_NAME to key collection"));
    }

    if (hr == S_OK)
    {
        hr = pParams->SetIPortableDeviceKeyCollectionValue(WPD_PROPERTY_OBJECT_PROPERTIES_PROPERTY_KEYS, pKeys);
        CHECK_HR(hr, ("Failed to set WPD_PROPERTY_OBJECT_PROPERTIES_PROPERTY_KEYS"));
    }

    // Make the call
    if (hr == S_OK)
    {
        hr = m_pWpdBaseDriver->DispatchWpdMessage(pParams, pResults);
        CHECK_HR(hr, ("Failed to dispatch message to get supported content types"));
    }

    if (hr == S_OK)
    {
        hr = pResults->GetIPortableDeviceValuesValue(WPD_PROPERTY_OBJECT_PROPERTIES_PROPERTY_VALUES, &pValues);
        CHECK_HR(hr, ("Failed to get WPD_PROPERTY_OBJECT_PROPERTIES_PROPERTY_VALUES"));
    }

    if (hr == S_OK)
    {
        hr = pValues->GetStringValue(WPD_DEVICE_FRIENDLY_NAME, pwszDeviceFriendlyName);
        CHECK_HR(hr, ("Failed to get WPD_DEVICE_FRIENDLY_NAME"));
    }

    return hr;
}


