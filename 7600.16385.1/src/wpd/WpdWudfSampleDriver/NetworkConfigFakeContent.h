class NetworkConfigFakeContent : public FakeContent
{
public:
    NetworkConfigFakeContent()
    {
    }

    virtual ~NetworkConfigFakeContent()
    {
    }

    virtual HRESULT GetSupportedProperties(IPortableDeviceKeyCollection **ppKeys)
    {
        HRESULT                                 hr      = S_OK;

        if(ppKeys == NULL)
        {
            hr = E_POINTER;
            CHECK_HR(hr, "Cannot have NULL collection parameter");
            return hr;
        }

        if (hr == S_OK)
        {
            hr = AddSupportedProperties(WPD_FUNCTIONAL_CATEGORY_NETWORK_CONFIGURATION, ppKeys);
            CHECK_HR(hr, "Failed to add additional properties for NetworkConfigFakeContent");
        }
        return hr;
    }

    virtual HRESULT GetAllValues(
        IPortableDeviceValues*  pStore)
    {
        HRESULT             hr          = S_OK;
        HRESULT             hrSetValue  = S_OK;

        if(pStore == NULL)
        {
            hr = E_POINTER;
            CHECK_HR(hr, "Cannot have NULL parameter");
            return hr;
        }

        // Call the base class to fill in the standard properties
        hr = FakeContent::GetAllValues(pStore);
        if (FAILED(hr))
        {
            CHECK_HR(hr, "Failed to get basic property set");
            return hr;
        }

        // Add WPD_FUNCTIONAL_OBJECT_CATEGORY
        hrSetValue = pStore->SetGuidValue(WPD_FUNCTIONAL_OBJECT_CATEGORY, WPD_FUNCTIONAL_CATEGORY_NETWORK_CONFIGURATION);
        if (hrSetValue != S_OK)
        {
            CHECK_HR(hr, "Failed to set WPD_FUNCTIONAL_OBJECT_CATEGORY");
            return hrSetValue;
        }

        return hr;
    }

    virtual HRESULT GetAttributes(
        REFPROPERTYKEY          Key,
        IPortableDeviceValues** ppAttributes)
    {
        HRESULT                         hr      = S_OK;
        CComPtr<IPortableDeviceValues>  pAttributes;

        if(ppAttributes == NULL)
        {
            hr = E_POINTER;
            CHECK_HR(hr, "Cannot have NULL attributes parameter");
            return hr;
        }

        if (hr == S_OK)
        {
            hr = CoCreateInstance(CLSID_PortableDeviceValues,
                                  NULL,
                                  CLSCTX_INPROC_SERVER,
                                  IID_IPortableDeviceValues,
                                  (VOID**) &pAttributes);
            CHECK_HR(hr, "Failed to CoCreate CLSID_PortableDeviceValues");
        }

        if (hr == S_OK)
        {
            hr = AddFixedPropertyAttributes(WPD_FUNCTIONAL_CATEGORY_NETWORK_CONFIGURATION, Key, pAttributes);
            CHECK_HR(hr, "Failed to add fixed property attributes for %ws.%d on NetworkConfigFakeContent", CComBSTR(Key.fmtid), Key.pid);
        }
#if 0
        // Some of our properties have extra attributes on top of the ones that are common to all
        if(IsEqualPropertyKey(Key, WPD_OBJECT_NAME))
        {
            CAtlStringW strDefaultName;

            strDefaultName.Format(L"%ws%ws", L"Name", ObjectID);

            hr = pAttributes->SetStringValue(WPD_PROPERTY_ATTRIBUTE_DEFAULT_VALUE, strDefaultName.GetString());;
            CHECK_HR(hr, "Failed to set WPD_PROPERTY_ATTRIBUTE_DEFAULT_VALUE");
        }
#endif
        if (hr == S_OK)
        {
            hr = pAttributes->QueryInterface(IID_IPortableDeviceValues, (VOID**) ppAttributes);
            CHECK_HR(hr, "Failed to QI for IPortableDeviceValues on IPortableDeviceValues");
        }
        return hr;
    }

    virtual HRESULT GetSupportedResources(IPortableDeviceKeyCollection **ppKeys)
    {

        HRESULT                                 hr      = S_OK;
        CComPtr<IPortableDeviceKeyCollection>   pKeys;

        if(ppKeys == NULL)
        {
            hr = E_POINTER;
            CHECK_HR(hr, "Cannot have NULL collection parameter");
            return hr;
        }

        if (hr == S_OK)
        {
            hr = CoCreateInstance(CLSID_PortableDeviceKeyCollection,
                                  NULL,
                                  CLSCTX_INPROC_SERVER,
                                  IID_IPortableDeviceKeyCollection,
                                  (VOID**) &pKeys);
            CHECK_HR(hr, "Failed to CoCreate CLSID_PortableDeviceKeyCollection");
        }

        if (hr == S_OK)
        {
            hr = pKeys->QueryInterface(IID_IPortableDeviceKeyCollection, (VOID**) ppKeys);
            CHECK_HR(hr, "Failed to QI for IPortableDeviceKeyCollection on IPortableDeviceKeyCollection");
        }

        return hr;
    }

    virtual HRESULT GetResourceAttributes(
        REFPROPERTYKEY,
        IPortableDeviceValues**)
    {
        return E_NOTIMPL;
    }

    virtual HRESULT ReadData(
        REFPROPERTYKEY,
        DWORD,
        BYTE*,
        DWORD,
        DWORD*)
    {
        return E_NOTIMPL;
    }

    virtual HRESULT WriteData(
        REFPROPERTYKEY,
        DWORD,
        BYTE*,
        DWORD,
        DWORD*)
    {
        return E_NOTIMPL;
    }

    virtual GUID GetObjectFormat()
    {
        return WPD_OBJECT_FORMAT_PROPERTIES_ONLY;
    }
};

class FakeNetworkAssociationContent : public FakeContent
{
public:
    FakeNetworkAssociationContent()
    {
        PropVariantInit(&HostEUI64Array);
    }

    virtual ~FakeNetworkAssociationContent()
    {
    }

    virtual HRESULT GetAllValues(IPortableDeviceValues* pStore)
    {
        HRESULT             hr          = S_OK;
        HRESULT             hrSetValue  = S_OK;
        PropVariantWrapper  pvValue;

        if(pStore == NULL)
        {
            hr = E_POINTER;
            CHECK_HR(hr, "Cannot have NULL parameter");
            return hr;
        }

        hr = FakeContent::GetAllValues(pStore);
        if (FAILED(hr))
        {
            CHECK_HR(hr, "Failed to get basic property set");
            return hr;
        }

        // Add WPD_NETWORK_ASSOCIATION_HOST_NETWORK_IDENTIFIERS
        if (HostEUI64Array.vt == VT_EMPTY)
        {
            // This value is supposed to be available, but it is just a soft error if it is missing
            pvValue.SetErrorValue(HRESULT_FROM_WIN32(ERROR_NOT_FOUND));
            hrSetValue = pStore->SetValue(WPD_NETWORK_ASSOCIATION_HOST_NETWORK_IDENTIFIERS, &pvValue);

            hr = S_FALSE;
        }
        else
        {
            hrSetValue = pStore->SetValue(WPD_NETWORK_ASSOCIATION_HOST_NETWORK_IDENTIFIERS, &HostEUI64Array);
        }
        if (hrSetValue != S_OK)
        {
            CHECK_HR(hrSetValue, ("Failed to set WPD_NETWORK_ASSOCIATION_HOST_NETWORK_IDENTIFIERS"));
            return hrSetValue;
        }

        return hr;
    }

    virtual HRESULT WriteValue(
        const PROPERTYKEY& key,
        const PROPVARIANT& Value)
    {
        HRESULT             hr      = S_OK;
        PropVariantWrapper  pvValue;

        if(IsEqualPropertyKey(key, WPD_NETWORK_ASSOCIATION_HOST_NETWORK_IDENTIFIERS))
        {
            if (Value.vt == (VARTYPE)(VT_VECTOR | VT_UI1))
            {
                if ((Value.caub.cElems % 8) == 0)
                {
                    if (FAILED(PropVariantClear(&HostEUI64Array)))
                    {
                        PropVariantInit(&HostEUI64Array);
                    }

                    hr = PropVariantCopy(&HostEUI64Array, &Value);
                    CHECK_HR(hr, "Error setting WPD_NETWORK_ASSOCIATION_HOST_NETWORK_IDENTIFIERS");
                }
                else
                {
                    hr = E_INVALIDARG;
                    CHECK_HR(hr, "Failed to set WPD_NETWORK_ASSOCIATION_HOST_NETWORK_IDENTIFIERS because value was the wrong length");
                }
            }
            else
            {
                hr = E_INVALIDARG;
                CHECK_HR(hr, "Failed to set WPD_NETWORK_ASSOCIATION_HOST_NETWORK_IDENTIFIERS because type was not VT_VECTOR|VT_UI1");
            }
        }
        else
        {
            hr = FakeContent::WriteValue(key, Value);
        }

        return hr;
    }

    virtual HRESULT GetSupportedResources(IPortableDeviceKeyCollection **ppKeys)
    {

        HRESULT                                 hr      = S_OK;
        CComPtr<IPortableDeviceKeyCollection>   pKeys;

        if(ppKeys == NULL)
        {
            hr = E_POINTER;
            CHECK_HR(hr, "Cannot have NULL collection parameter");
            return hr;
        }

        if (hr == S_OK)
        {
            hr = CoCreateInstance(CLSID_PortableDeviceKeyCollection,
                                  NULL,
                                  CLSCTX_INPROC_SERVER,
                                  IID_IPortableDeviceKeyCollection,
                                  (VOID**) &pKeys);
            CHECK_HR(hr, "Failed to CoCreate CLSID_PortableDeviceKeyCollection");
        }

        if (hr == S_OK)
        {
            hr = pKeys->QueryInterface(IID_IPortableDeviceKeyCollection, (VOID**) ppKeys);
            CHECK_HR(hr, "Failed to QI for IPortableDeviceKeyCollection on IPortableDeviceKeyCollection");
        }

        return hr;
    }

    virtual HRESULT GetResourceAttributes(
        REFPROPERTYKEY,
        IPortableDeviceValues**)
    {
        return E_NOTIMPL;
    }

    virtual HRESULT ReadData(
        REFPROPERTYKEY,
        DWORD,
        BYTE*,
        DWORD,
        DWORD*)
    {
        return E_NOTIMPL;
    }

    virtual HRESULT WriteData(
        REFPROPERTYKEY,
        DWORD,
        BYTE*,
        DWORD,
        DWORD*)
    {
        return E_NOTIMPL;
    }

    virtual GUID GetObjectFormat()
    {
        return WPD_OBJECT_FORMAT_NETWORK_ASSOCIATION;
    }

    PROPVARIANT HostEUI64Array;
};


class FakeWirelessProfileContent : public FakeContent
{
public:
    FakeWirelessProfileContent()
    {
    }

    virtual ~FakeWirelessProfileContent()
    {
    }

    virtual GUID GetObjectFormat()
    {
        return WPD_OBJECT_FORMAT_MICROSOFT_WFC;
    }
};


