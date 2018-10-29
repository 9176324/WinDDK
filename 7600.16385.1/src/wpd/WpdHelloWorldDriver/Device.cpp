#include "stdafx.h"
#include "Device.h"
#include "WpdHelloWorldDriver_i.c"

STDMETHODIMP_(HRESULT)
CDevice::OnD0Entry(IWDFDevice* /*pDevice*/,
                   WDF_POWER_DEVICE_STATE /*previousState*/)
{
    return S_OK;
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

    if (m_pWpdBaseDriver != NULL)
    {
        hr = m_pWpdBaseDriver->Initialize();
        CHECK_HR(hr, "Failed to Initialize the driver class");
    }

    // Initialize the WPD Class Extension. This will enable the appropriate WPD interface GUID,
    // as well as do any additional initialization (e.g. enabling Legacy Compatibility layers for those drivers
    // which requested support in their INF).
    if (hr == S_OK && m_pPortableDeviceClassExtension == NULL)
    {
        CComPtr<IPortableDeviceValues>                pOptions;
        CComPtr<IPortableDevicePropVariantCollection> pContentTypes;

        hr = CoCreateInstance(CLSID_PortableDeviceClassExtension,
                              NULL,
                              CLSCTX_INPROC_SERVER,
                              IID_IPortableDeviceClassExtension,
                              (VOID**)&m_pPortableDeviceClassExtension);
        CHECK_HR(hr, "Failed to CoCreate CLSID_PortableDeviceClassExtension");

        if (hr == S_OK)
        {
            hr = CoCreateInstance(CLSID_PortableDeviceValues,
                                  NULL,
                                  CLSCTX_INPROC_SERVER,
                                  IID_IPortableDeviceValues,
                                  (VOID**)&pOptions);
            CHECK_HR(hr, "Failed to CoCreate CLSID_PortableDeviceValues");

            if (hr == S_OK)
            {
                // Get supported content types
                if (hr == S_OK)
                {
                    hr = GetSupportedContentTypes(&pContentTypes);
                    CHECK_HR(hr, "Failed to get supported content types");
                }

                // Add the supported types to the options
                if (hr == S_OK)
                {
                    hr = pOptions->SetIPortableDevicePropVariantCollectionValue(WPD_CLASS_EXTENSION_OPTIONS_SUPPORTED_CONTENT_TYPES, pContentTypes);
                    CHECK_HR(hr, "Failed to set WPD_CLASS_EXTENSION_OPTIONS_SUPPORTED_CONTENT_TYPES");
                }

                // Initialize the PortableDeviceClassExtension with a list of supported content types for the
                // connected device.  This will ensure that the correct application compatibility settings will
                // be applied for your device.
                if (hr == S_OK)
                {
                    hr = m_pPortableDeviceClassExtension->Initialize(pDevice, pOptions);
                    CHECK_HR(hr, "Failed to Initialize portable device class extension object");
                }

                // Since users commonly have the abiltity to customize their device even when it is not
                // connected to the PC, we need to make sure the PC is current when the driver loads.
                //
                // Send the latest device friendly name to the PortableDeviceClassExtension component
                // so the system is always updated with the current device name.
                //
                // This call should also be made after a successful property set operation of
                // WPD_DEVICE_FRIENDLY_NAME.

                LPWSTR wszDeviceFriendlyName = NULL;

                if (hr == S_OK)
                {
                    hr = GetDeviceFriendlyName(&wszDeviceFriendlyName);
                    CHECK_HR(hr, "Failed to get device's friendly name");
                }

                if (hr == S_OK)
                {
                    hr = UpdateDeviceFriendlyName(m_pPortableDeviceClassExtension, wszDeviceFriendlyName);
                    CHECK_HR(hr, "Failed to update device's friendly name");
                }

                // Free the memory.  CoTaskMemFree ignores NULLs so no need to check.
                CoTaskMemFree(wszDeviceFriendlyName);
            }
        }
    }

    return hr;
}

STDMETHODIMP_(HRESULT)
CDevice::OnReleaseHardware(IWDFDevice* /*pDevice*/)
{
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
    CComPtr<IPortableDeviceValues> pParams;
    CComPtr<IPortableDeviceValues> pResults;

    if (ppContentTypes == NULL)
    {
        hr = E_INVALIDARG;
        return hr;
    }

    // CoCreate a collection to store the WPD_COMMAND_CAPABILITIES_GET_SUPPORTED_CONTENT_TYPES command parameters.
    if (hr == S_OK)
    {
        hr = CoCreateInstance(CLSID_PortableDeviceValues,
                              NULL,
                              CLSCTX_INPROC_SERVER,
                              IID_IPortableDeviceValues,
                              (VOID**)&pParams);
        CHECK_HR(hr, "Failed to CoCreate CLSID_PortableDeviceValues");
    }

    // CoCreate a collection to store the WPD_COMMAND_CAPABILITIES_GET_SUPPORTED_CONTENT_TYPES command results.
    if (hr == S_OK)
    {
        hr = CoCreateInstance(CLSID_PortableDeviceValues,
                              NULL,
                              CLSCTX_INPROC_SERVER,
                              IID_IPortableDeviceValues,
                              (VOID**)&pResults);
        CHECK_HR(hr, "Failed to CoCreate CLSID_PortableDeviceValues");
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

    // Get the results
    if (hr == S_OK)
    {
        hr = pResults->GetIPortableDevicePropVariantCollectionValue(WPD_PROPERTY_CAPABILITIES_CONTENT_TYPES, ppContentTypes);
        CHECK_HR(hr, ("Failed to get WPD_PROPERTY_CAPABILITIES_CONTENT_TYPES"));
    }

    return hr;
}

HRESULT CDevice::GetDeviceFriendlyName(
    __deref_out_opt LPWSTR*   pwszDeviceFriendlyName)
{
    HRESULT hr = S_OK;

    CComPtr<IPortableDeviceValues>        pParams;
    CComPtr<IPortableDeviceValues>        pResults;
    CComPtr<IPortableDeviceKeyCollection> pKeys;
    CComPtr<IPortableDeviceValues>        pValues;

    if (pwszDeviceFriendlyName == NULL)
    {
        hr = E_INVALIDARG;
        return hr;
    }

    *pwszDeviceFriendlyName = NULL;

    // CoCreate a collection to store the WPD_COMMAND_OBJECT_PROPERTIES_GET command parameters.
    if (hr == S_OK)
    {
        hr = CoCreateInstance(CLSID_PortableDeviceValues,
                              NULL,
                              CLSCTX_INPROC_SERVER,
                              IID_IPortableDeviceValues,
                              (VOID**)&pParams);
        CHECK_HR(hr, "Failed to CoCreate CLSID_PortableDeviceValues");
    }

    // CoCreate a collection to store the WPD_COMMAND_OBJECT_PROPERTIES_GET command results.
    if (hr == S_OK)
    {
        hr = CoCreateInstance(CLSID_PortableDeviceValues,
                              NULL,
                              CLSCTX_INPROC_SERVER,
                              IID_IPortableDeviceValues,
                              (VOID**)&pResults);
        CHECK_HR(hr, "Failed to CoCreate CLSID_PortableDeviceValues");
    }

    // CoCreate a collection to store the requested property keys.  In our case, we are requesting just the device friendly name
    // (WPD_DEVICE_FRIENDLY_NAME)
    if (hr == S_OK)
    {
        hr = CoCreateInstance(CLSID_PortableDeviceKeyCollection,
                              NULL,
                              CLSCTX_INPROC_SERVER,
                              IID_IPortableDeviceKeyCollection,
                              (VOID**)&pKeys);
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

    // Get the results
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

HRESULT UpdateDeviceFriendlyName(
    IPortableDeviceClassExtension*  pPortableDeviceClassExtension,
    LPCWSTR                         wszDeviceFriendlyName)
{
    HRESULT hr = S_OK;

    // If we were passed NULL parameters we have nothing to do, return S_OK.
    if ((pPortableDeviceClassExtension == NULL) ||
        (wszDeviceFriendlyName         == NULL))
    {
        return S_OK;
    }

    CComPtr<IPortableDeviceValues>  pParams;
    CComPtr<IPortableDeviceValues>  pResults;
    CComPtr<IPortableDeviceValues>  pValues;

    // Prepare to make a call to set the device information
    if (hr == S_OK)
    {
        hr = CoCreateInstance(CLSID_PortableDeviceValues,
                              NULL,
                              CLSCTX_INPROC_SERVER,
                              IID_IPortableDeviceValues,
                              (VOID**)&pParams);
        CHECK_HR(hr, "Failed to CoCreate CLSID_PortableDeviceValues");
    }

    if (hr == S_OK)
    {
        hr = CoCreateInstance(CLSID_PortableDeviceValues,
                              NULL,
                              CLSCTX_INPROC_SERVER,
                              IID_IPortableDeviceValues,
                              (VOID**)&pResults);
        CHECK_HR(hr, "Failed to CoCreate CLSID_PortableDeviceValues for results");
    }

    if (hr == S_OK)
    {
        hr = CoCreateInstance(CLSID_PortableDeviceValues,
                              NULL,
                              CLSCTX_INPROC_SERVER,
                              IID_IPortableDeviceValues,
                              (VOID**)&pValues);
        CHECK_HR(hr, "Failed to CoCreate CLSID_PortableDeviceValues for results");
    }

    // Get the information values to update and set them in WPD_PROPERTY_CLASS_EXTENSION_DEVICE_INFORMATION_VALUES
    if (hr == S_OK)
    {
        hr = pValues->SetStringValue(WPD_DEVICE_FRIENDLY_NAME, wszDeviceFriendlyName);
        CHECK_HR(hr, ("Failed to set WPD_DEVICE_FRIENDLY_NAME"));
    }

    // Set the params
    if (hr == S_OK)
    {
        hr = pParams->SetGuidValue(WPD_PROPERTY_COMMON_COMMAND_CATEGORY, WPD_COMMAND_CLASS_EXTENSION_WRITE_DEVICE_INFORMATION.fmtid);
        CHECK_HR(hr, ("Failed to set WPD_PROPERTY_COMMON_COMMAND_CATEGORY"));
    }
    if (hr == S_OK)
    {
        hr = pParams->SetUnsignedIntegerValue(WPD_PROPERTY_COMMON_COMMAND_ID, WPD_COMMAND_CLASS_EXTENSION_WRITE_DEVICE_INFORMATION.pid);
        CHECK_HR(hr, ("Failed to set WPD_PROPERTY_COMMON_COMMAND_ID"));
    }
    if (hr == S_OK)
    {
        hr = pParams->SetIPortableDeviceValuesValue(WPD_PROPERTY_CLASS_EXTENSION_DEVICE_INFORMATION_VALUES, pValues);
        CHECK_HR(hr, ("Failed to set WPD_PROPERTY_CLASS_EXTENSION_DEVICE_INFORMATION_VALUES"));
    }

    // Make the call
    if (hr == S_OK)
    {
        hr = pPortableDeviceClassExtension->ProcessLibraryMessage(pParams, pResults);
        CHECK_HR(hr, ("Failed to process update device information message"));
    }

    // A Failed ProcessLibraryMessage operation for updating this value is not considered
    // fatal and should return S_OK.

    return S_OK;
}

