class FakeMemoContent : public FakeContent
{
public:
    FakeMemoContent()
    {
    }

    FakeMemoContent(const FakeMemoContent& src)
    {
        *this = src;
    }

    virtual ~FakeMemoContent()
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
            hr = AddSupportedProperties(FakeMemoContent_Format, ppKeys);
            CHECK_HR(hr, "Failed to add additional properties for FakeMemoContent");
        }

        return hr;
    }

    virtual HRESULT GetAllValues(
        IPortableDeviceValues*  pStore)
    {
        HRESULT             hr          = S_OK;
        HRESULT             hrSetValue  = S_OK;
        CAtlStringW         strVal;
        PropVariantWrapper  pvValue;

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

        // Add WPD_OBJECT_SIZE
        hrSetValue = pStore->SetUnsignedLargeIntegerValue(WPD_OBJECT_SIZE, GetResourceSize(IDR_WPD_SAMPLEDRIVER_MEMO));
        if (hrSetValue != S_OK)
        {
            CHECK_HR(hrSetValue, ("Failed to set WPD_OBJECT_SIZE"));
            return hrSetValue;
        }

        // Add WPD_OBJECT_DATE_AUTHORED and WPD_OBJECT_DATE_MODIFIED
        SYSTEMTIME systemtime = {0};
        PROPVARIANT pv = {0};
        PropVariantInit(&pv);
        pv.vt = VT_DATE;
        systemtime.wDay          = 26;
        systemtime.wDayOfWeek    = 0;
        systemtime.wHour         = 5;
        systemtime.wMinute       = 30;
        systemtime.wMilliseconds = 100;
        systemtime.wMonth        = 6;
        systemtime.wSecond       = 15;
        systemtime.wYear         = 2004;

        SystemTimeToVariantTime(&systemtime, &pv.date);

        hrSetValue = pStore->SetValue(WPD_OBJECT_DATE_AUTHORED , &pv);
        if (hrSetValue != S_OK)
        {
            CHECK_HR(hrSetValue, ("Failed to set WPD_OBJECT_DATE_AUTHORED "));
            return hrSetValue;
        }
        hrSetValue = pStore->SetValue(WPD_OBJECT_DATE_MODIFIED , &pv);
        if (hrSetValue != S_OK)
        {
            CHECK_HR(hrSetValue, ("Failed to set WPD_OBJECT_DATE_MODIFIED "));
            return hrSetValue;
        }

        PropVariantClear(&pv);

        // Add WPD_OBJECT_ORIGINAL_FILE_NAME
        pvValue = FileName;
        hrSetValue = pStore->SetValue(WPD_OBJECT_ORIGINAL_FILE_NAME, &pvValue);
        if (hrSetValue != S_OK)
        {
            CHECK_HR(hrSetValue, ("Failed to set WPD_OBJECT_ORIGINAL_FILE_NAME"));
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

        if(IsEqualPropertyKey(key, WPD_OBJECT_ORIGINAL_FILE_NAME))
        {
            if(Value.vt == VT_LPWSTR)
            {
                if (lstrlenW(Value.pwszVal) > 0)
                {
                    FileName = Value.pwszVal;
                }
                else
                {
                    hr = E_INVALIDARG;
                    CHECK_HR(hr, "Failed to set WPD_OBJECT_ORIGINAL_FILE_NAME because value was an empty string");
                }
            }
            else
            {
                hr = E_INVALIDARG;
                CHECK_HR(hr, "Failed to set WPD_OBJECT_ORIGINAL_FILE_NAME because type was not VT_LPWSTR");
            }
        }
        else
        {
            hr = FakeContent::WriteValue(key, Value);
            CHECK_HR(hr, "Property %ws.%d on [%ws] does not support set value operation", CComBSTR(key.fmtid), key.pid, ObjectID);
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
            // Call the base class to fill in the standard resources if any
            hr = FakeContent::GetSupportedResources(&pKeys);
            CHECK_HR(hr, "Failed to get basic supported resources");
        }

        if (hr == S_OK)
        {
            // Add WPD_RESOURCE_DEFAULT
            hr = pKeys->Add(WPD_RESOURCE_DEFAULT);
            CHECK_HR(hr, "Failed to add WPD_RESOURCE_DEFAULT to collection");
        }

        if (hr == S_OK)
        {
            // Add WPD_RESOURCE_ICON
            hr = pKeys->Add(WPD_RESOURCE_ICON);
            CHECK_HR(hr, "Failed to add WPD_RESOURCE_ICON to collection");
        }

        if (hr == S_OK)
        {
            hr = pKeys->QueryInterface(IID_IPortableDeviceKeyCollection, (VOID**) ppKeys);
            CHECK_HR(hr, "Failed to QI for IPortableDeviceKeyCollection on IPortableDeviceKeyCollection");
        }

        return hr;
    }

    virtual HRESULT GetResourceAttributes(
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

        if (IsEqualPropertyKey(Key, WPD_RESOURCE_ICON))
        {
            // Fill in the common resource attributes
            hr = GetCommonResourceAttributes(&pAttributes);
            CHECK_HR(hr, "Failed to get common resource attributes set");

            // Override the size attribute for this resource.
            if (hr == S_OK)
            {
                hr = pAttributes->SetUnsignedIntegerValue(WPD_RESOURCE_ATTRIBUTE_TOTAL_SIZE, GetResourceSize(IDR_WPD_SAMPLEDRIVER_MEMO_ICON));
                CHECK_HR(hr, "Failed to set WPD_RESOURCE_ATTRIBUTE_TOTAL_SIZE");
            }

            // Override the format attribute for this resource.
            if (hr == S_OK)
            {
                hr = pAttributes->SetGuidValue(WPD_RESOURCE_ATTRIBUTE_FORMAT, WPD_OBJECT_FORMAT_ICON);
                CHECK_HR(hr, "Failed to set WPD_RESOURCE_ATTRIBUTE_FORMAT");
            }

        }
        else if (IsEqualPropertyKey(Key, WPD_RESOURCE_DEFAULT))
        {
            // Fill in the common resource attributes
            hr = GetCommonResourceAttributes(&pAttributes);
            CHECK_HR(hr, "Failed to get common resource attributes set");

            // Override the size attribute for this resource.
            if (hr == S_OK)
            {
                hr = pAttributes->SetUnsignedIntegerValue(WPD_RESOURCE_ATTRIBUTE_TOTAL_SIZE, GetResourceSize(IDR_WPD_SAMPLEDRIVER_MEMO));
                CHECK_HR(hr, "Failed to set WPD_RESOURCE_ATTRIBUTE_TOTAL_SIZE");
            }

            // Override the format attribute for this resource.
            if (hr == S_OK)
            {
                hr = pAttributes->SetGuidValue(WPD_RESOURCE_ATTRIBUTE_FORMAT, FakeMemoContent_Format);
                CHECK_HR(hr, "Failed to set WPD_RESOURCE_ATTRIBUTE_FORMAT");
            }
        }
        else
        {
            hr = E_INVALIDARG;
            CHECK_HR(hr, "Object does not support this resource");        
        }

        // Return the resource attributes
        if (hr == S_OK)
        {
            hr = pAttributes->QueryInterface(IID_IPortableDeviceValues, (VOID**) ppAttributes);
            CHECK_HR(hr, "Failed to QI for IPortableDeviceValues on Wpd IPortableDeviceValues");
        }
                
        return hr;
    }

    // This sample driver uses a embedded resources as its data.
    virtual HRESULT ReadData(
        REFPROPERTYKEY  ResourceKey,
        DWORD           dwStartByte,
        BYTE*           pBuffer,
        DWORD           dwNumBytesToRead,
        DWORD*          pdwNumBytesRead)
    {
        HRESULT hr                = S_OK;
        DWORD   dwBytesToTransfer = 0;
        DWORD   dwObjectDataSize  = 0;
        PBYTE   pResource         = NULL;

        if((pBuffer         == NULL) ||
           (pdwNumBytesRead == NULL))
        {
            hr = E_POINTER;
            CHECK_HR(hr, "Cannot have NULL parameter");
            return hr;
        }

        if (IsEqualPropertyKey(ResourceKey, WPD_RESOURCE_ICON))
        {
            pResource = GetResourceData(IDR_WPD_SAMPLEDRIVER_MEMO_ICON);
            dwObjectDataSize = GetResourceSize(IDR_WPD_SAMPLEDRIVER_MEMO_ICON);
        }
        else if (IsEqualPropertyKey(ResourceKey, WPD_RESOURCE_DEFAULT))
        {
            pResource = GetResourceData(IDR_WPD_SAMPLEDRIVER_MEMO);
            dwObjectDataSize = GetResourceSize(IDR_WPD_SAMPLEDRIVER_MEMO);        
        }
        else
        {
            hr = E_INVALIDARG;
            CHECK_HR(hr, "Object does not support this resource");
        }

        if (hr == S_OK)
        {
            if (pResource == NULL)
            {
                hr = E_UNEXPECTED;
                CHECK_HR(hr, "Failed to get resource representing image data");
            }

            //  Calculate how many bytes to transfer
            if (hr == S_OK)
            {
                if (dwStartByte < dwObjectDataSize)
                {
                    dwBytesToTransfer = (dwObjectDataSize - dwStartByte);
                    if (dwBytesToTransfer > dwNumBytesToRead)
                    {
                        dwBytesToTransfer = dwNumBytesToRead;
                    }
                }
            }

            // Copy the embedded data.
            if ((hr == S_OK) && (dwBytesToTransfer > 0))
            {
                memcpy(pBuffer, pResource + dwStartByte, dwBytesToTransfer);
            }

            if (hr == S_OK)
            {
                *pdwNumBytesRead = dwBytesToTransfer;
            }
        }

        return hr;
    }

    virtual GUID GetObjectFormat()
    {
        return FakeMemoContent_Format;
    }

};

