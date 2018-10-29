#include "stdafx.h"

#define MAX_OBJECTS_TO_RETURN 20

WpdObjectPropertiesBulk::WpdObjectPropertiesBulk()
{

}

WpdObjectPropertiesBulk::~WpdObjectPropertiesBulk()
{

}

HRESULT WpdObjectPropertiesBulk::Initialize(FakeDevice *pFakeDevice)
{

    HRESULT hr = S_OK;

    if(pFakeDevice == NULL)
    {
        hr = E_POINTER;
        CHECK_HR(hr, "Cannot have NULL parameter");
        return hr;
    }
    m_pFakeDevice = pFakeDevice;
    return hr;
}


HRESULT WpdObjectPropertiesBulk::DispatchWpdMessage(const PROPERTYKEY&  Command,
                                                IPortableDeviceValues*     pParams,
                                                IPortableDeviceValues*     pResults)
{

    HRESULT hr = S_OK;

    if (hr == S_OK)
    {
        if (Command.fmtid != WPD_CATEGORY_OBJECT_PROPERTIES_BULK)
        {
            hr = E_INVALIDARG;
            CHECK_HR(hr, "This object does not support this command category %ws",CComBSTR(Command.fmtid));
        }
    }

    if (hr == S_OK)
    {
        if(IsEqualPropertyKey(Command, WPD_COMMAND_OBJECT_PROPERTIES_BULK_GET_VALUES_BY_OBJECT_LIST_START))
        {
            hr = OnGetValuesByObjectListStart(pParams, pResults);
            if(FAILED(hr))
            {
                CHECK_HR(hr, "Failed to start bulk property operation");
            }
        }
        else if(IsEqualPropertyKey(Command, WPD_COMMAND_OBJECT_PROPERTIES_BULK_GET_VALUES_BY_OBJECT_LIST_NEXT))
        {
            hr = OnGetValuesByObjectListNext(pParams, pResults);
            if(FAILED(hr))
            {
                CHECK_HR(hr, "Failed to do next bulk property operation");
            }
        }
        else if(IsEqualPropertyKey(Command, WPD_COMMAND_OBJECT_PROPERTIES_BULK_GET_VALUES_BY_OBJECT_LIST_END))
        {
            hr = OnGetValuesByObjectListEnd(pParams, pResults);
            if(FAILED(hr))
            {
                CHECK_HR(hr, "Failed to end bulk property operation");
            }
        }
        else if(IsEqualPropertyKey(Command, WPD_COMMAND_OBJECT_PROPERTIES_BULK_GET_VALUES_BY_OBJECT_FORMAT_START))
        {
            hr = OnGetValuesByObjectFormatStart(pParams, pResults);
            if(FAILED(hr))
            {
                CHECK_HR(hr, "Failed to start bulk property operation");
            }
        }
        else if(IsEqualPropertyKey(Command, WPD_COMMAND_OBJECT_PROPERTIES_BULK_GET_VALUES_BY_OBJECT_FORMAT_NEXT))
        {
            hr = OnGetValuesByObjectFormatNext(pParams, pResults);
            if(FAILED(hr))
            {
                CHECK_HR(hr, "Failed to do next bulk property operation");
            }
        }
        else if(IsEqualPropertyKey(Command, WPD_COMMAND_OBJECT_PROPERTIES_BULK_GET_VALUES_BY_OBJECT_FORMAT_END))
        {
            hr = OnGetValuesByObjectFormatEnd(pParams, pResults);
            if(FAILED(hr))
            {
                CHECK_HR(hr, "Failed to end bulk property operation ");
            }
        }
        else if(IsEqualPropertyKey(Command, WPD_COMMAND_OBJECT_PROPERTIES_BULK_SET_VALUES_BY_OBJECT_LIST_START))
        {
            hr = OnSetValuesByObjectListStart(pParams, pResults);
            if(FAILED(hr))
            {
                CHECK_HR(hr, "Failed to set bulk property operation");
            }
        }
        else if(IsEqualPropertyKey(Command, WPD_COMMAND_OBJECT_PROPERTIES_BULK_SET_VALUES_BY_OBJECT_LIST_NEXT))
        {
            hr = OnSetValuesByObjectListNext(pParams, pResults);
            if(FAILED(hr))
            {
                CHECK_HR(hr, "Failed to set bulk property operation");
            }
        }
        else if(IsEqualPropertyKey(Command, WPD_COMMAND_OBJECT_PROPERTIES_BULK_SET_VALUES_BY_OBJECT_LIST_END))
        {
            hr = OnSetValuesByObjectListEnd(pParams, pResults);
            if(FAILED(hr))
            {
                CHECK_HR(hr, "Failed to set bulk property operation");
            }
        }
        else
        {
            hr = E_NOTIMPL;
            CHECK_HR(hr, "This object does not support this command id %d", Command.pid);
        }
    }
    return hr;
}

/**
 *  This method is called when we receive a WPD_COMMAND_OBJECT_PROPERTIES_BULK_GET_VALUES_BY_OBJECT_LIST_START
 *  command.
 *
 *  The parameters sent to us are:
 *  - WPD_PROPERTY_OBJECT_PROPERTIES_BULK_OBJECT_IDS: identifies the objects whose property
 *    values we want to return.
 *  - WPD_PROPERTY_OBJECT_PROPERTIES_BULK_PROPERTY_KEYS: a collection of property keys, identifying which
 *    specific property values we are requested to return.  If this property doesn't exist,
 *    then the client is asking for all values.
 *
 *  The driver should:
 *  - Create a new context for this bulk property operation.
 *  - Return an identifier for the context in WPD_PROPERTY_OBJECT_PROPERTIES_BULK_CONTEXT.
 */
HRESULT WpdObjectPropertiesBulk::OnGetValuesByObjectListStart(
    IPortableDeviceValues*  pParams,
    IPortableDeviceValues*  pResults)
{

    HRESULT                                       hr = S_OK;
    CComPtr<IPortableDevicePropVariantCollection> pObjectIDs;
    CComPtr<IPortableDeviceKeyCollection>         pKeys;
    ContextMap*                                   pContextMap = NULL;

    // Get the IPortableDevicePropVariantCollection which contains the collection
    // of object identifiers the bulk operation is being performed on.
    if (hr == S_OK)
    {
        hr = pParams->GetIPortableDevicePropVariantCollectionValue(WPD_PROPERTY_OBJECT_PROPERTIES_BULK_OBJECT_IDS, &pObjectIDs);
        CHECK_HR(hr, "Missing value for WPD_PROPERTY_OBJECT_PROPERTIES_BULK_OBJECT_IDS");
    }

    // Get the IPortableDeviceKeyCollection which contains the collection
    // keys of properties being read on the multiple objects.
    if (hr == S_OK)
    {
        hr = pParams->GetIPortableDeviceKeyCollectionValue(WPD_PROPERTY_OBJECT_PROPERTIES_BULK_PROPERTY_KEYS, &pKeys);
        if(hr != S_OK)
        {
            // Client is asking for all properties.
            pKeys = NULL;
            hr = S_OK;
        }
    }

    // Get the context map which the driver stored in pParams for convenience
    if (hr == S_OK)
    {
        hr = pParams->GetIUnknownValue(PRIVATE_SAMPLE_DRIVER_CLIENT_CONTEXT_MAP, (IUnknown**)&pContextMap);
        CHECK_HR(hr, "Failed to get PRIVATE_SAMPLE_DRIVER_CLIENT_CONTEXT_MAP");
    }

    if (hr == S_OK)
    {
        LPWSTR  pwszContext = NULL;
        hr = CreateBulkPropertiesContext(pContextMap, pObjectIDs, pKeys, &pwszContext);
        if (hr == S_OK)
        {
            hr = pResults->SetStringValue(WPD_PROPERTY_OBJECT_PROPERTIES_BULK_CONTEXT, pwszContext);
            CHECK_HR(hr, "Failed to set WPD_PROPERTY_OBJECT_PROPERTIES_BULK_CONTEXT");
        }

        // Free the memory.  CoTaskMemFree ignores NULLs so no need to check.
        CoTaskMemFree(pwszContext);
    }

    SAFE_RELEASE(pContextMap);

    return hr;
}

/**
 *  This method is called when we receive a WPD_COMMAND_OBJECT_PROPERTIES_BULK_GET_VALUES_BY_OBJECT_LIST_NEXT
 *  command.
 *
 *  The parameters sent to us are:
 *  - WPD_PROPERTY_OBJECT_PROPERTIES_BULK_CONTEXT: the context the driver returned to
 *      the client in OnGetValuesByObjectListStart.
 *
 *  The driver should:
 *  - Return the next set of property values in WPD_PROPERTY_OBJECT_PROPERTIES_BULK_VALUES.
 *    If there are no more properties to be read an
 *    empty collection should be returned.
 *  - It is up to the driver to return as many object property values as it wants.  If zero values are returned
 *    it is assumed the bulk operation is complete and the WPD_COMMAND_OBJECT_PROPERTIES_BULK_GET_VALUES_BY_OBJECT_LIST_END
 *    will be called next.
 *
 *  - S_OK should be returned if the collection can be returned successfully.
 *  - Any error return indicates that the driver did not fill in any results, and the caller will
 *      not attempt to unpack any property values.
 */
HRESULT WpdObjectPropertiesBulk::OnGetValuesByObjectListNext(
    IPortableDeviceValues*  pParams,
    IPortableDeviceValues*  pResults)
{

    HRESULT                                  hr          = S_OK;
    LPWSTR                                   pwszContext = NULL;
    BulkPropertiesContext*                   pContext    = NULL;
    DWORD                                    cObjects    = 0;
    CComPtr<IPortableDeviceValuesCollection> pCollection;

    if (hr == S_OK)
    {
        hr = pParams->GetStringValue(WPD_PROPERTY_OBJECT_PROPERTIES_BULK_CONTEXT, &pwszContext);
        CHECK_HR(hr, "Failed to get WPD_PROPERTY_OBJECT_PROPERTIES_BULK_CONTEXT from IPortableDeviceValues");
    }

    // Get the bulk property operation context
    if (hr == S_OK)
    {
        hr = GetClientContext(pParams, pwszContext, (IUnknown**) &pContext);
        CHECK_HR(hr, "Failed to get bulk property context");
    }

    if (hr == S_OK)
    {
        hr = pContext->ObjectIDs->GetCount(&cObjects);
        CHECK_HR(hr, "Failed to get number of objectIDs from bulk properties context");
    }

    if (hr == S_OK)
    {
        cObjects = cObjects - pContext->NextObject;
        if(cObjects > MAX_OBJECTS_TO_RETURN)
        {
            cObjects = MAX_OBJECTS_TO_RETURN;
        }
    }

    // Make sure the the collection holds VT_LPWSTR values.
    if (hr == S_OK)
    {
        hr = pContext->ObjectIDs->ChangeType(VT_LPWSTR);
        CHECK_HR(hr, "Failed to change objectIDs collection to VT_LPWSTR");
    }

    if (hr == S_OK)
    {
        hr = CoCreateInstance(CLSID_PortableDeviceValuesCollection,
                              NULL,
                              CLSCTX_INPROC_SERVER,
                              IID_IPortableDeviceValuesCollection,
                              (VOID**) &pCollection);
        CHECK_HR(hr, "Failed to CoCreate CLSID_PortableDeviceValuesCollection");
    }

    if (hr == S_OK)
    {
        for (DWORD dwIndex = pContext->NextObject, dwCount = 0; dwCount < cObjects; dwCount++, dwIndex++)
        {
            CComPtr<IPortableDeviceValues> pValues;
            PROPVARIANT pv = {0};
            PropVariantInit(&pv);
            hr = pContext->ObjectIDs->GetAt(dwIndex, &pv);
            CHECK_HR(hr, "Failed to get next object ID from bulk properties context");

            if (hr == S_OK)
            {
                // If a key list was supplied, get the specified object properties, other get all
                // properties.
                if(pContext->Properties != NULL)
                {
                    hr = m_pFakeDevice->GetValues(pv.pwszVal, pContext->Properties, &pValues);
                    CHECK_HR(hr, "Failed to get property values for [%ws]", pv.pwszVal);
                }
                else
                {
                    hr = m_pFakeDevice->GetAllValues(pv.pwszVal, &pValues);
                    CHECK_HR(hr, "Failed to get property values for [%ws]", pv.pwszVal);
                }

                // Add the ObjectID to the returned results
                if (hr == S_OK)
                {
                    hr = pValues->SetStringValue(WPD_OBJECT_ID, pv.pwszVal);
                    CHECK_HR(hr, "Failed to set WPD_OBJECT_ID for %ws", pv.pwszVal);
                }
            }

            if (hr == S_OK)
            {
                hr = pCollection->Add(pValues);
                CHECK_HR(hr, "Failed to add IPortableDeviceValues to IPortableDeviceValuesCollection");
            }

            PropVariantClear(&pv);

            pContext->NextObject += 1;
        }
    }

    if (hr == S_OK)
    {
        hr = pResults->SetIUnknownValue(WPD_PROPERTY_OBJECT_PROPERTIES_BULK_VALUES, pCollection);
        CHECK_HR(hr, "Failed to set WPD_PROPERTY_OBJECT_PROPERTIES_BULK_VALUES");
    }

    // Free the memory.  CoTaskMemFree ignores NULLs so no need to check.
    CoTaskMemFree(pwszContext);

    SAFE_RELEASE(pContext);

    return hr;
}

/**
 *  This method is called when we receive a WPD_COMMAND_OBJECT_PROPERTIES_BULK_GET_VALUES_BY_OBJECT_LIST_END
 *  command.
 *
 *  The parameters sent to us are:
 *  - WPD_PROPERTY_OBJECT_PROPERTIES_BULK_CONTEXT: the context the driver returned to
 *      the client in OnGetValuesByObjectListStart.
 *
 *  The driver should:
 *  - Destroy any resources associated with this context.
 */
HRESULT WpdObjectPropertiesBulk::OnGetValuesByObjectListEnd(
    IPortableDeviceValues*  pParams,
    IPortableDeviceValues*  pResults)
{

    HRESULT     hr          = S_OK;
    LPWSTR      pwszContext = NULL;
    ContextMap* pContextMap = NULL;
    UNREFERENCED_PARAMETER(pResults);

    if (hr == S_OK)
    {
        hr = pParams->GetStringValue(WPD_PROPERTY_OBJECT_PROPERTIES_BULK_CONTEXT, &pwszContext);
        CHECK_HR(hr, "Failed to get WPD_PROPERTY_OBJECT_PROPERTIES_BULK_CONTEXT from IPortableDeviceValues");
    }

    // Get the context map which the driver stored in pParams for convenience
    if (hr == S_OK)
    {
        hr = pParams->GetIUnknownValue(PRIVATE_SAMPLE_DRIVER_CLIENT_CONTEXT_MAP, (IUnknown**)&pContextMap);
        CHECK_HR(hr, "Failed to get PRIVATE_SAMPLE_DRIVER_CLIENT_CONTEXT_MAP");
    }

    if (hr == S_OK)
    {
        hr = DestroyBulkPropertiesContext(pContextMap, pwszContext);
        CHECK_HR(hr, "Failed to destroy bulk property context %ws", pwszContext);
    }

    // Free the memory.  CoTaskMemFree ignores NULLs so no need to check.
    CoTaskMemFree(pwszContext);

    SAFE_RELEASE(pContextMap);

    return hr;
}

/**
 *  This method is called when we receive a WPD_COMMAND_OBJECT_PROPERTIES_BULK_GET_VALUES_BY_OBJECT_FORMAT_START
 *  command.
 *
 *  The parameters sent to us are:
 *  - WPD_PROPERTY_OBJECT_PROPERTIES_BULK_OBJECT_FORMAT:  Identifies the format of the objects the
 *      client is interested in.
 *  - WPD_PROPERTY_OBJECT_PROPERTIES_BULK_PARENT_OBJECT_ID: Identifies the parent object from which the
 *      operation should start.
 *  - WPD_PROPERTY_OBJECT_PROPERTIES_BULK_DEPTH: Indicates the hierarchical depth of the operation
 *      from the parent object.
 *  - WPD_PROPERTY_OBJECT_PROPERTIES_BULK_PROPERTY_KEYS: a collection of property keys, identifying which
 *      specific property values we are requested to return.  If this doesn't exist, then
 *      ALL object proeprties should be returned for the specified objects.
 *
 *  The driver should:
 *  - Create a new context for this bulk property operation.
 *  - Return an identifier for the context in WPD_PROPERTY_OBJECT_PROPERTIES_BULK_CONTEXT.
 */
HRESULT WpdObjectPropertiesBulk::OnGetValuesByObjectFormatStart(
    IPortableDeviceValues*  pParams,
    IPortableDeviceValues*  pResults)
{

    HRESULT                                       hr = S_OK;
    CComPtr<IPortableDeviceKeyCollection>         pKeys;
    GUID                                          guidOjbectFormat  = GUID_NULL;
    LPWSTR                                        pszParentObjectID = NULL;
    DWORD                                         dwDepth           = 0;
    ContextMap*                                   pContextMap       = NULL;

    // Get the object format.
    if (hr == S_OK)
    {
        hr = pParams->GetGuidValue(WPD_PROPERTY_OBJECT_PROPERTIES_BULK_OBJECT_FORMAT, &guidOjbectFormat);
        CHECK_HR(hr, "Missing value for WPD_PROPERTY_OBJECT_PROPERTIES_BULK_OBJECT_FORMAT");
    }

    // Get the parent object id.
    if (hr == S_OK)
    {
        hr = pParams->GetStringValue(WPD_PROPERTY_OBJECT_PROPERTIES_BULK_PARENT_OBJECT_ID, &pszParentObjectID);
        CHECK_HR(hr, "Missing value for WPD_PROPERTY_OBJECT_PROPERTIES_BULK_PARENT_OBJECT_ID");
    }

    // Get the depth.
    if (hr == S_OK)
    {
        hr = pParams->GetUnsignedIntegerValue(WPD_PROPERTY_OBJECT_PROPERTIES_BULK_DEPTH, &dwDepth);
        CHECK_HR(hr, "Missing value for WPD_PROPERTY_OBJECT_PROPERTIES_BULK_DEPTH");
    }

    // Get the IPortableDeviceKeyCollection which contains the collection
    // keys of properties being read on the multiple objects.
    if (hr == S_OK)
    {
        hr = pParams->GetIPortableDeviceKeyCollectionValue(WPD_PROPERTY_OBJECT_PROPERTIES_BULK_PROPERTY_KEYS, &pKeys);
        if(hr != S_OK)
        {
            // Client is asking for all properties.
            pKeys = NULL;
            hr = S_OK;
        }
    }

    // Get the context map which the driver stored in pParams for convenience
    if (hr == S_OK)
    {
        hr = pParams->GetIUnknownValue(PRIVATE_SAMPLE_DRIVER_CLIENT_CONTEXT_MAP, (IUnknown**)&pContextMap);
        CHECK_HR(hr, "Failed to get PRIVATE_SAMPLE_DRIVER_CLIENT_CONTEXT_MAP");
    }

    if (hr == S_OK)
    {
        LPWSTR  pwszContext = NULL;
        hr = CreateBulkPropertiesContext(pContextMap, guidOjbectFormat, pszParentObjectID, dwDepth, pKeys, &pwszContext);
        if (hr == S_OK)
        {
            hr = pResults->SetStringValue(WPD_PROPERTY_OBJECT_PROPERTIES_BULK_CONTEXT, pwszContext);
            CHECK_HR(hr, "Failed to set WPD_PROPERTY_OBJECT_PROPERTIES_BULK_CONTEXT");
        }

        // Free the memory.  CoTaskMemFree ignores NULLs so no need to check.
        CoTaskMemFree(pwszContext);
    }

    // Free the memory.  CoTaskMemFree ignores NULLs so no need to check.
    CoTaskMemFree(pszParentObjectID);

    SAFE_RELEASE(pContextMap);

    return hr;
}

/**
 *  This method is called when we receive a WPD_COMMAND_OBJECT_PROPERTIES_BULK_GET_VALUES_BY_OBJECT_FORMAT_NEXT
 *  command.
 *
 *  The parameters sent to us are:
 *  - WPD_PROPERTY_OBJECT_PROPERTIES_BULK_CONTEXT: the context the driver returned to
 *      the client in OnGetValuesByObjectFormatStart.
 *
 *  The driver should:
 *  - Return the next set of property values in WPD_PROPERTY_OBJECT_PROPERTIES_BULK_VALUES.
 *    If there are no more properties to be read an
 *    empty collection should be returned.
 *  - It is up to the driver to return as many object property values as it wants.  If zero values are returned
 *    it is assumed the bulk operation is complete and the WPD_COMMAND_OBJECT_PROPERTIES_BULK_GET_VALUES_BY_OBJECT_FORMAT_END
 *    will be called next.
 *
 *  - S_OK should be returned if the collection can be returned successfully.
 *  - Any error return indicates that the driver did not fill in any results, and the caller will
 *      not attempt to unpack any property values.
 */
HRESULT WpdObjectPropertiesBulk::OnGetValuesByObjectFormatNext(
    IPortableDeviceValues*  pParams,
    IPortableDeviceValues*  pResults)
{

    HRESULT                                  hr = S_OK;
    LPWSTR                                   pwszContext = NULL;
    BulkPropertiesContext*                   pContext    = NULL;
    DWORD                                    cObjects    = 0;
    CComPtr<IPortableDeviceValuesCollection> pCollection;

    if (hr == S_OK)
    {
        hr = pParams->GetStringValue(WPD_PROPERTY_OBJECT_PROPERTIES_BULK_CONTEXT, &pwszContext);
        CHECK_HR(hr, "Failed to get WPD_PROPERTY_OBJECT_PROPERTIES_BULK_CONTEXT from IPortableDeviceValues");
    }

    // Get the bulk property operation context
    if (hr == S_OK)
    {
        hr = GetClientContext(pParams, pwszContext, (IUnknown**) &pContext);
        CHECK_HR(hr, "Failed to get bulk property context");
    }

    // Make sure the the collection holds VT_LPWSTR values.
    if (hr == S_OK)
    {
        hr = pContext->ObjectIDs->ChangeType(VT_LPWSTR);
        CHECK_HR(hr, "Failed to change objectIDs collection to VT_LPWSTR");
    }

    if (hr == S_OK)
    {
        hr = pContext->ObjectIDs->GetCount(&cObjects);
        CHECK_HR(hr, "Failed to get number of objectIDs from bulk properties context");
    }

    if (hr == S_OK)
    {
        cObjects = cObjects - pContext->NextObject;
        if(cObjects > MAX_OBJECTS_TO_RETURN)
        {
            cObjects = MAX_OBJECTS_TO_RETURN;
        }
    }

    if (hr == S_OK)
    {
        hr = CoCreateInstance(CLSID_PortableDeviceValuesCollection,
                              NULL,
                              CLSCTX_INPROC_SERVER,
                              IID_IPortableDeviceValuesCollection,
                              (VOID**) &pCollection);
        CHECK_HR(hr, "Failed to CoCreate CLSID_PortableDeviceValuesCollection");
    }

    if (hr == S_OK)
    {
        for (DWORD dwIndex = pContext->NextObject, dwCount = 0; dwCount < cObjects; dwCount++, dwIndex++)
        {
            CComPtr<IPortableDeviceValues> pValues;
            PROPVARIANT pv = {0};
            PropVariantInit(&pv);
            hr = pContext->ObjectIDs->GetAt(dwIndex, &pv);
            CHECK_HR(hr, "Failed to get next object ID from bulk properties context");

            if (hr == S_OK)
            {
                // If a key list was supplied, get the specified object properties, other get all
                // properties.
                if(pContext->Properties != NULL)
                {
                    hr = m_pFakeDevice->GetValues(pv.pwszVal, pContext->Properties, &pValues);
                    CHECK_HR(hr, "Failed to get property values for [%ws]", pv.pwszVal);
                }
                else
                {
                    hr = m_pFakeDevice->GetAllValues(pv.pwszVal, &pValues);
                    CHECK_HR(hr, "Failed to get property values for [%ws]", pv.pwszVal);
                }

                // Add the ObjectID to the returned results
                if (hr == S_OK)
                {
                    hr = pValues->SetStringValue(WPD_OBJECT_ID, pv.pwszVal);
                    CHECK_HR(hr, "Failed to set WPD_OBJECT_ID for %ws", pv.pwszVal);
                }
            }

            if (hr == S_OK)
            {
                hr = pCollection->Add(pValues);
                CHECK_HR(hr, "Failed to add IPortableDeviceValues to IPortableDeviceValuesCollection");
            }

            PropVariantClear(&pv);
            pContext->NextObject++;
        }
    }

    if (hr == S_OK)
    {
        hr = pResults->SetIUnknownValue(WPD_PROPERTY_OBJECT_PROPERTIES_BULK_VALUES, pCollection);
        CHECK_HR(hr, "Failed to set WPD_PROPERTY_OBJECT_PROPERTIES_BULK_VALUES");
    }

    // Free the memory.  CoTaskMemFree ignores NULLs so no need to check.
    CoTaskMemFree(pwszContext);
    SAFE_RELEASE(pContext);

    return hr;
}

/**
 *  This method is called when we receive a WPD_COMMAND_OBJECT_PROPERTIES_BULK_GET_VALUES_BY_OBJECT_FORMAT_END
 *  command.
 *
 *  The parameters sent to us are:
 *  - WPD_PROPERTY_OBJECT_PROPERTIES_BULK_CONTEXT: the context the driver returned to
 *      the client in OnGetValuesByObjectFormatStart.
 *
 *  The driver should:
 *  - Destroy any resources associated with this context.
 */
HRESULT WpdObjectPropertiesBulk::OnGetValuesByObjectFormatEnd(
    IPortableDeviceValues*  pParams,
    IPortableDeviceValues*  pResults)
{

    HRESULT     hr          = S_OK;
    LPWSTR      pwszContext = NULL;
    ContextMap* pContextMap = NULL;
    UNREFERENCED_PARAMETER(pResults);

    if (hr == S_OK)
    {
        hr = pParams->GetStringValue(WPD_PROPERTY_OBJECT_PROPERTIES_BULK_CONTEXT, &pwszContext);
        CHECK_HR(hr, "Failed to get WPD_PROPERTY_OBJECT_PROPERTIES_BULK_CONTEXT from IPortableDeviceValues");
    }

    // Get the context map which the driver stored in pParams for convenience
    if (hr == S_OK)
    {
        hr = pParams->GetIUnknownValue(PRIVATE_SAMPLE_DRIVER_CLIENT_CONTEXT_MAP, (IUnknown**)&pContextMap);
        CHECK_HR(hr, "Failed to get PRIVATE_SAMPLE_DRIVER_CLIENT_CONTEXT_MAP");
    }

    if (hr == S_OK)
    {
        hr = DestroyBulkPropertiesContext(pContextMap, pwszContext);
        CHECK_HR(hr, "Failed to destroy bulk property context %ws", pwszContext);
    }

    // Free the memory.  CoTaskMemFree ignores NULLs so no need to check.
    CoTaskMemFree(pwszContext);

    SAFE_RELEASE(pContextMap);

    return hr;
}

/**
 *  This method is called when we receive a WPD_COMMAND_OBJECT_PROPERTIES_BULK_SET_VALUES_BY_OBJECT_LIST_START
 *  command.
 *
 *  The parameters sent to us are:
 *  - WPD_PROPERTY_OBJECT_PROPERTIES_BULK_VALUES:  holds a collection of IPortableDeviceValues which
 *    indicate which object properties to set.
 *
 *  The driver should:
 *  - Create a new context for this bulk property operation.
 *  - Return an identifier for the context in WPD_PROPERTY_OBJECT_PROPERTIES_BULK_CONTEXT.
 */
HRESULT WpdObjectPropertiesBulk::OnSetValuesByObjectListStart(
    IPortableDeviceValues*  pParams,
    IPortableDeviceValues*  pResults)
{

    HRESULT                                       hr            = S_OK;
    CComPtr<IPortableDeviceValuesCollection>      pValuesCollection;
    ContextMap*                                   pContextMap   = NULL;

    // Get the IPortableDevicePropVariantCollection which contains the collection
    // of object identifiers the bulk operation is being performed on.
    if (hr == S_OK)
    {
        hr = pParams->GetIPortableDeviceValuesCollectionValue(WPD_PROPERTY_OBJECT_PROPERTIES_BULK_VALUES, &pValuesCollection);
        CHECK_HR(hr, "Missing value for WPD_PROPERTY_OBJECT_PROPERTIES_BULK_VALUES");
    }

    // Get the context map which the driver stored in pParams for convenience
    if (hr == S_OK)
    {
        hr = pParams->GetIUnknownValue(PRIVATE_SAMPLE_DRIVER_CLIENT_CONTEXT_MAP, (IUnknown**)&pContextMap);
        CHECK_HR(hr, "Failed to get PRIVATE_SAMPLE_DRIVER_CLIENT_CONTEXT_MAP");
    }

    if (hr == S_OK)
    {
        LPWSTR  pwszContext = NULL;
        hr = CreateBulkPropertiesContext(pContextMap, pValuesCollection, &pwszContext);
        if (hr == S_OK)
        {
            hr = pResults->SetStringValue(WPD_PROPERTY_OBJECT_PROPERTIES_BULK_CONTEXT, pwszContext);
            CHECK_HR(hr, "Failed to set WPD_PROPERTY_OBJECT_PROPERTIES_BULK_CONTEXT");
        }

        // Free the memory.  CoTaskMemFree ignores NULLs so no need to check.
        CoTaskMemFree(pwszContext);
    }

    SAFE_RELEASE(pContextMap);

    return hr;
}

/**
 *  This method is called when we receive a WPD_COMMAND_OBJECT_PROPERTIES_BULK_SET_VALUES_BY_OBJECT_LIST_NEXT
 *  command.
 *
 *  The parameters sent to us are:
 *  - WPD_PROPERTY_OBJECT_PROPERTIES_BULK_CONTEXT: the context the driver returned to
 *      the client in OnGetValuesByObjectListStart.
 *
 *  The driver should:
 *  - Write the next set of property values, and return the write results in WPD_PROPERTY_OBJECT_PROPERTIES_BULK_WRITE_RESULTS.
 *    If there are no more properties to be written, an empty collection should be returned.
 *  - It is up to the driver to write as many object property values as it wants.  If zero write results are returned
 *    it is assumed the bulk operation is complete and the WPD_COMMAND_OBJECT_PROPERTIES_BULK_SET_VALUES_BY_OBJECT_LIST_END
 *    will be called next.
 *
 *  - S_OK should be returned if the collection can be returned successfully.
 *  - Any error return indicates that the driver did not fill in any results, and the caller will
 *      not attempt to unpack any property values.
 */
HRESULT WpdObjectPropertiesBulk::OnSetValuesByObjectListNext(
    IPortableDeviceValues*  pParams,
    IPortableDeviceValues*  pResults)
{

    HRESULT                                  hr          = S_OK;
    LPWSTR                                   pwszContext = NULL;
    BulkPropertiesContext*                   pContext    = NULL;
    DWORD                                    cObjects    = 0;
    CComPtr<IPortableDeviceValuesCollection> pWriteResults;
    CComPtr<IPortableDeviceValuesCollection> pValuesCollection;

    if (hr == S_OK)
    {
        hr = pParams->GetStringValue(WPD_PROPERTY_OBJECT_PROPERTIES_BULK_CONTEXT, &pwszContext);
        CHECK_HR(hr, "Failed to get WPD_PROPERTY_OBJECT_PROPERTIES_BULK_CONTEXT from IPortableDeviceValues");
    }

    // Get the bulk property operation context
    if (hr == S_OK)
    {
        hr = GetClientContext(pParams, pwszContext, (IUnknown**) &pContext);
        CHECK_HR(hr, "Failed to get bulk property context");
    }

    // Make sure the the collection holds a ValuesCollection, then get the number of elements.
    if (hr == S_OK)
    {
        if(pContext->ValuesCollection != NULL)
        {
            hr = pContext->ValuesCollection->GetCount(&cObjects);
            CHECK_HR(hr, "Failed to get number of objectIDs from bulk properties context");
        }
        else
        {
            hr = E_INVALIDARG;
            CHECK_HR(hr, "Incorrect context specified - this context does not contain a values collection");
        }
    }

    // Create the collection to hold the write results
    if (hr == S_OK)
    {
        hr = CoCreateInstance(CLSID_PortableDeviceValuesCollection,
                              NULL,
                              CLSCTX_INPROC_SERVER,
                              IID_IPortableDeviceValuesCollection,
                              (VOID**) &pWriteResults);
        CHECK_HR(hr, "Failed to CoCreate CLSID_PortableDeviceValuesCollection");
    }

    if (hr == S_OK)
    {
        for (DWORD dwIndex = pContext->NextObject; dwIndex < cObjects; dwIndex++)
        {
            CComPtr<IPortableDeviceValues> pValues;
            CComPtr<IPortableDeviceValues> pResults;
            hr = pContext->ValuesCollection->GetAt(dwIndex, &pValues);
            CHECK_HR(hr, "Failed to get next values from bulk properties context");

            if (hr == S_OK)
            {
                LPWSTR pszObjectID = NULL;

                // Get which object this is for
                hr =  pValues->GetStringValue(WPD_OBJECT_ID, &pszObjectID);
                if (hr == S_OK)
                {
                    // Set the values
                    hr = m_pFakeDevice->WritePropertiesOnObject(pszObjectID, pValues, &pResults);
                    CHECK_HR(hr, "Failed to set proeprties on [%ws]", pszObjectID);

                    // Ensure the write results contain which ObjectID this was for
                    if (hr == S_OK)
                    {
                        hr = pResults->SetStringValue(WPD_OBJECT_ID, pszObjectID);
                        CHECK_HR(hr, "Failed to set WPD_OBJECT_ID in write resutls");
                    }
                }
                CoTaskMemFree(pszObjectID);
            }

            if (hr == S_OK)
            {
                hr = pWriteResults->Add(pResults);
                CHECK_HR(hr, "Failed to add IPortableDeviceValues to IPortableDeviceValuesCollection");
            }

            pContext->NextObject++;
        }
    }

    if (hr == S_OK)
    {
        hr = pResults->SetIUnknownValue(WPD_PROPERTY_OBJECT_PROPERTIES_BULK_WRITE_RESULTS, pWriteResults);
        CHECK_HR(hr, "Failed to set WPD_PROPERTY_OBJECT_PROPERTIES_BULK_WRITE_RESULTS");
    }

    // Free the memory.  CoTaskMemFree ignores NULLs so no need to check.
    CoTaskMemFree(pwszContext);

    SAFE_RELEASE(pContext);

    return hr;
}

/**
 *  This method is called when we receive a WPD_COMMAND_OBJECT_PROPERTIES_BULK_SET_VALUES_BY_OBJECT_LIST_END
 *  command.
 *
 *  The parameters sent to us are:
 *  - WPD_PROPERTY_OBJECT_PROPERTIES_BULK_CONTEXT: the context the driver returned to
 *      the client in OnSetValuesByObjectListStart.
 *
 *  The driver should:
 *  - Destroy any resources associated with this context.
 */
HRESULT WpdObjectPropertiesBulk::OnSetValuesByObjectListEnd(
    IPortableDeviceValues*  pParams,
    IPortableDeviceValues*  pResults)
{

    HRESULT     hr          = S_OK;
    LPWSTR      pwszContext = NULL;
    ContextMap* pContextMap = NULL;
    UNREFERENCED_PARAMETER(pResults);

    if (hr == S_OK)
    {
        hr = pParams->GetStringValue(WPD_PROPERTY_OBJECT_PROPERTIES_BULK_CONTEXT, &pwszContext);
        CHECK_HR(hr, "Failed to get WPD_PROPERTY_OBJECT_PROPERTIES_BULK_CONTEXT from IPortableDeviceValues");
    }

    // Get the context map which the driver stored in pParams for convenience
    if (hr == S_OK)
    {
        hr = pParams->GetIUnknownValue(PRIVATE_SAMPLE_DRIVER_CLIENT_CONTEXT_MAP, (IUnknown**)&pContextMap);
        CHECK_HR(hr, "Failed to get PRIVATE_SAMPLE_DRIVER_CLIENT_CONTEXT_MAP");
    }

    if (hr == S_OK)
    {
        hr = DestroyBulkPropertiesContext(pContextMap, pwszContext);
        CHECK_HR(hr, "Failed to destroy bulk property context %ws", pwszContext);
    }

    // Free the memory.  CoTaskMemFree ignores NULLs so no need to check.
    CoTaskMemFree(pwszContext);

    SAFE_RELEASE(pContextMap);

    return hr;
}

HRESULT WpdObjectPropertiesBulk::CreateBulkPropertiesContext(
    __inout         ContextMap*                           pContextMap,
    __in            IPortableDevicePropVariantCollection* pObjectIDs,
    __in            IPortableDeviceKeyCollection*         pProperties,
    __deref_out_opt LPWSTR*                               ppszBulkPropertiesContext)
{

    HRESULT                 hr              = S_OK;
    GUID                    guidContext     = GUID_NULL;
    CComBSTR                bstrContext;
    BulkPropertiesContext*  pContext        = NULL;

    if((pContextMap               == NULL) ||
       (pObjectIDs                == NULL) ||
       (ppszBulkPropertiesContext == NULL))
    {
        hr = E_POINTER;
        CHECK_HR(hr, "Cannot have NULL parameter");
        return hr;
    }

    *ppszBulkPropertiesContext = NULL;

    hr = CoCreateGuid(&guidContext);
    if (hr == S_OK)
    {
        bstrContext = guidContext;
        if(bstrContext.Length() == 0)
        {
            hr = E_OUTOFMEMORY;
            CHECK_HR(hr, "Failed to create BSTR from GUID");
        }
    }

    if (hr == S_OK)
    {
        pContext = new BulkPropertiesContext();
        if(pContext == NULL)
        {
            hr = E_OUTOFMEMORY;
            CHECK_HR(hr, "Failed to allocate new bulk properties context");
        }
    }

    if (hr == S_OK)
    {
        pContext->ObjectIDs  = pObjectIDs;
        pContext->Properties = pProperties;

        CAtlStringW strKey = bstrContext;
        hr = pContextMap->Add(strKey, pContext); // calls AddRef on pContext
        CHECK_HR(hr, "Failed to insert bulk property operation context into our context Map");
    }

    if (hr == S_OK)
    {
        *ppszBulkPropertiesContext = AtlAllocTaskWideString(bstrContext);
    }

    SAFE_RELEASE(pContext);

    return hr;
}

HRESULT WpdObjectPropertiesBulk::CreateBulkPropertiesContext(
    __inout         ContextMap*                     pContextMap,
    __in            REFGUID                         guidObjectFormat,
    __in            LPCWSTR                         pszParentObjectID,
    __in            DWORD                           dwDepth,
    __in            IPortableDeviceKeyCollection*   pProperties,
    __deref_out_opt LPWSTR*                         ppszBulkPropertiesContext)
{

    HRESULT               hr              = S_OK;
    GUID                  guidContext     = GUID_NULL;
    CComBSTR              bstrContext;
    BulkPropertiesContext*  pContext      = NULL;
    CComPtr<IPortableDevicePropVariantCollection> pObjectIDs;

    if((pContextMap               == NULL) ||
       (pszParentObjectID         == NULL) ||
       (ppszBulkPropertiesContext == NULL))
    {
        hr = E_POINTER;
        CHECK_HR(hr, "Cannot have NULL parameter");
        return hr;
    }

    *ppszBulkPropertiesContext = NULL;

    if (hr == S_OK)
    {
        hr = m_pFakeDevice->GetObjectIDsByFormat(guidObjectFormat, pszParentObjectID, dwDepth, &pObjectIDs);
        CHECK_HR(hr, "Faield to get list of object ids by format");
    }

    hr = CoCreateGuid(&guidContext);
    if (hr == S_OK)
    {
        bstrContext = guidContext;
        if(bstrContext.Length() == 0)
        {
            hr = E_OUTOFMEMORY;
            CHECK_HR(hr, "Failed to create BSTR from GUID");
        }
    }

    if (hr == S_OK)
    {
        pContext = new BulkPropertiesContext();
        if(pContext == NULL)
        {
            hr = E_OUTOFMEMORY;
            CHECK_HR(hr, "Failed to allocate new bulk properties context");
        }
    }

    if (hr == S_OK)
    {
        pContext->Properties = pProperties;
        pContext->ObjectIDs  = pObjectIDs;

        CAtlStringW strKey = bstrContext;
        hr = pContextMap->Add(strKey, pContext);
        CHECK_HR(hr, "Failed to insert bulk property operation context into our context Map");
    }

    if (hr == S_OK)
    {
        *ppszBulkPropertiesContext = AtlAllocTaskWideString(bstrContext);
    }

    SAFE_RELEASE(pContext);

    return hr;
}

HRESULT WpdObjectPropertiesBulk::CreateBulkPropertiesContext(
    __inout         ContextMap*                         pContextMap,
    __in            IPortableDeviceValuesCollection*    pValuesCollection,
    __deref_out_opt LPWSTR*                             ppszBulkPropertiesContext)
{

    HRESULT               hr              = S_OK;
    GUID                  guidContext     = GUID_NULL;
    CComBSTR              bstrContext;
    BulkPropertiesContext*  pContext      = NULL;

    if((pContextMap               == NULL) ||
       (pValuesCollection         == NULL) ||
       (ppszBulkPropertiesContext == NULL))
    {
        hr = E_POINTER;
        CHECK_HR(hr, "Cannot have NULL parameter");
        return hr;
    }

    *ppszBulkPropertiesContext = NULL;
    
    hr = CoCreateGuid(&guidContext);
    if (hr == S_OK)
    {
        bstrContext = guidContext;
        if(bstrContext.Length() == 0)
        {
            hr = E_OUTOFMEMORY;
            CHECK_HR(hr, "Failed to create BSTR from GUID");
        }
    }

    if (hr == S_OK)
    {
        pContext = new BulkPropertiesContext();
        if(pContext == NULL)
        {
            hr = E_OUTOFMEMORY;
            CHECK_HR(hr, "Failed to allocate new bulk properties context");
        }
    }

    if (hr == S_OK)
    {
        pContext->ValuesCollection  = pValuesCollection;

        CAtlStringW strKey = bstrContext;
        hr = pContextMap->Add(strKey, pContext);
        CHECK_HR(hr, "Failed to insert bulk property operation context into our context Map");
    }

    if (hr == S_OK)
    {
        *ppszBulkPropertiesContext = AtlAllocTaskWideString(bstrContext);
    }

    SAFE_RELEASE(pContext);

    return hr;
}

HRESULT WpdObjectPropertiesBulk::DestroyBulkPropertiesContext(
    __inout ContextMap* pContextMap,
    __in    LPCWSTR     pszBulkPropertiesContext)
{

    HRESULT hr = S_OK;

    CAtlStringW strKey = pszBulkPropertiesContext;
    pContextMap->Remove(strKey);

    return hr;
}

