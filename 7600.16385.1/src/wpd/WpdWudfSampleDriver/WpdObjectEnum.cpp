#include "stdafx.h"

WpdObjectEnumerator::WpdObjectEnumerator()
{

}

WpdObjectEnumerator::~WpdObjectEnumerator()
{

}

HRESULT WpdObjectEnumerator::Initialize(FakeDevice *pFakeDevice)
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


HRESULT WpdObjectEnumerator::DispatchWpdMessage(const PROPERTYKEY&  Command,
                                                IPortableDeviceValues*     pParams,
                                                IPortableDeviceValues*     pResults)
{

    HRESULT hr = S_OK;

    if (hr == S_OK)
    {
        if (Command.fmtid != WPD_CATEGORY_OBJECT_ENUMERATION)
        {
            hr = E_INVALIDARG;
            CHECK_HR(hr, "This object does not support this command category %ws",CComBSTR(Command.fmtid));
        }
    }

    if (hr == S_OK)
    {
        if (Command.pid == WPD_COMMAND_OBJECT_ENUMERATION_START_FIND.pid)
        {
            hr = OnStartFind(pParams, pResults);
            CHECK_HR(hr, "Failed to begin enumeration");
        }
        else if (Command.pid == WPD_COMMAND_OBJECT_ENUMERATION_FIND_NEXT.pid)
        {
            hr = OnFindNext(pParams, pResults);
            if(FAILED(hr))
            {
                CHECK_HR(hr, "Failed to find next object");
            }
        }
        else if (Command.pid == WPD_COMMAND_OBJECT_ENUMERATION_END_FIND.pid)
        {
            hr = OnEndFind(pParams, pResults);
            CHECK_HR(hr, "Failed to end enumeration");
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
 *  This method is called when we receive a WPD_COMMAND_OBJECT_ENUMERATION_START_FIND
 *  command.
 *
 *  The parameters sent to us are:
 *  - WPD_PROPERTY_OBJECT_ENUMERATION_PARENT_ID: the parent where we should start
 *      the enumeration.
 *  - WPD_PROPERTY_OBJECT_ENUMERATION_FILTER: the filter to use when doing
 *      enumeration.  Since this parameter is optional, it may not exist.
 *      This driver currently ignores the filter parameter.
 *
 *  The driver should:
 *  - Create a new context for this enumeration.
 *  - Return an identifier for the context in WPD_PROPERTY_OBJECT_ENUMERATION_CONTEXT.
 *
 */
HRESULT WpdObjectEnumerator::OnStartFind(IPortableDeviceValues*  pParams,
                                         IPortableDeviceValues*  pResults)
{

    HRESULT     hr              = S_OK;
    LPWSTR      pszParentID     = NULL;
    LPWSTR      pszEnumContext  = NULL;
    IUnknown*   pContextMap     = NULL;

    hr = pParams->GetStringValue(WPD_PROPERTY_OBJECT_ENUMERATION_PARENT_ID, &pszParentID);
    if (hr != S_OK)
    {
        hr = E_INVALIDARG;
        CHECK_HR(hr, "Missing value for WPD_PROPERTY_OBJECT_ENUMERATION_PARENT_ID");
    }

    if (hr == S_OK)
    {
        hr = pParams->GetIUnknownValue(PRIVATE_SAMPLE_DRIVER_CLIENT_CONTEXT_MAP, &pContextMap);
        CHECK_HR(hr, "Failed to get PRIVATE_SAMPLE_DRIVER_CLIENT_CONTEXT_MAP");
    }

    if (hr == S_OK)
    {
        hr = CreateEnumContext((ContextMap*)pContextMap, pszParentID, &pszEnumContext);
        CHECK_HR(hr, "Failed to create enumeration context");
    }

    if (hr == S_OK)
    {
        hr = pResults->SetStringValue(WPD_PROPERTY_OBJECT_ENUMERATION_CONTEXT, pszEnumContext);
        CHECK_HR(hr, "Failed to set WPD_PROPERTY_OBJECT_ENUMERATION_CONTEXT");
    }

    // Free the memory.  CoTaskMemFree ignores NULLs so no need to check.
    CoTaskMemFree(pszParentID);
    // Free the memory.  CoTaskMemFree ignores NULLs so no need to check.
    CoTaskMemFree(pszEnumContext);

    SAFE_RELEASE(pContextMap);

    return hr;
}

HRESULT WpdObjectEnumerator::OnFindNext(IPortableDeviceValues*  pParams,
                                        IPortableDeviceValues*  pResults)
{

    HRESULT     hr                  = S_OK;
    LPWSTR      pszEnumContext      = NULL;
    DWORD       dwNumObjects        = 0;
    ContextMap* pContextMap         = NULL;
    CComPtr<IPortableDevicePropVariantCollection>   pObjectIDCollection;

    hr = pParams->GetStringValue(WPD_PROPERTY_OBJECT_ENUMERATION_CONTEXT, &pszEnumContext);
    if (hr != S_OK)
    {
        hr = E_INVALIDARG;
        CHECK_HR(hr, "Missing value for WPD_PROPERTY_OBJECT_ENUMERATION_CONTEXT");
    }
    hr = pParams->GetUnsignedIntegerValue(WPD_PROPERTY_OBJECT_ENUMERATION_NUM_OBJECTS_REQUESTED, &dwNumObjects);
    if (hr != S_OK)
    {
        hr = E_INVALIDARG;
        CHECK_HR(hr, "Missing value for WPD_PROPERTY_OBJECT_ENUMERATION_NUM_OBJECTS_REQUESTED");
    }

    // Get the context map which the driver stored in pParams for convenience
    if (hr == S_OK)
    {
        hr = pParams->GetIUnknownValue(PRIVATE_SAMPLE_DRIVER_CLIENT_CONTEXT_MAP, (IUnknown**)&pContextMap);
        CHECK_HR(hr, "Failed to get PRIVATE_SAMPLE_DRIVER_CLIENT_CONTEXT_MAP");
    }

    if (hr == S_OK)
    {
        //  Find the next objects in the enumeration
        hr = GetObjectIDs(pContextMap, dwNumObjects, pszEnumContext, &pObjectIDCollection);
        CHECK_HR(hr, "Failed to get the objectIDs");
    }

    if (hr == S_OK)
    {
        hr = pResults->SetIPortableDevicePropVariantCollectionValue(WPD_PROPERTY_OBJECT_ENUMERATION_OBJECT_IDS, pObjectIDCollection);
        CHECK_HR(hr, "Failed to set WPD_PROPERTY_OBJECT_ENUMERATION_OBJECT_IDS");
    }

    if(hr == S_OK)
    {
        if(pObjectIDCollection != NULL)
        {
            ULONG    ulCount    = 0;

            pObjectIDCollection->GetCount(&ulCount);
            if(ulCount < dwNumObjects)
            {
                hr = S_FALSE;
            }
        }
    }
    // Free the memory.  CoTaskMemFree ignores NULLs so no need to check.
    CoTaskMemFree(pszEnumContext);

    SAFE_RELEASE(pContextMap);

    return hr;
}

/**
 *  This method is called when we receive a WPD_COMMAND_OBJECT_ENUMERATION_END_FIND
 *  command.
 *
 *  The parameters sent to us are:
 *  - WPD_PROPERTY_OBJECT_ENUMERATION_CONTEXT: the context the driver returned to
 *      the client in OnStartFind.
 *
 *  The driver should:
 *  - Destroy any resources associated with this context.
 */
HRESULT WpdObjectEnumerator::OnEndFind(IPortableDeviceValues*  pParams,
                                       IPortableDeviceValues*  pResults)
{

    HRESULT     hr              = S_OK;
    LPWSTR      pszEnumContext  = NULL;
    IUnknown*   pContextMap     = NULL;

    UNREFERENCED_PARAMETER(pResults);

    hr = pParams->GetStringValue(WPD_PROPERTY_OBJECT_ENUMERATION_CONTEXT, &pszEnumContext);
    if (hr != S_OK)
    {
        hr = E_INVALIDARG;
        CHECK_HR(hr, "Missing value for WPD_PROPERTY_OBJECT_ENUMERATION_CONTEXT");
    }

    if (hr == S_OK)
    {
        hr = pParams->GetIUnknownValue(PRIVATE_SAMPLE_DRIVER_CLIENT_CONTEXT_MAP, &pContextMap);
        CHECK_HR(hr, "Failed to get PRIVATE_SAMPLE_DRIVER_CLIENT_CONTEXT_MAP");
    }

    if (hr == S_OK)
    {
        hr = DestroyEnumContext((ContextMap*) pContextMap, pszEnumContext);
        CHECK_HR(hr, "Failed to destroy enumeration context");
    }

    // Free the memory.  CoTaskMemFree ignores NULLs so no need to check.
    CoTaskMemFree(pszEnumContext);

    SAFE_RELEASE(pContextMap);

    return hr;
}

HRESULT WpdObjectEnumerator::CreateEnumContext(
    __inout         ContextMap*         pContextMap,
    __in            LPCWSTR             pszParentID,
    __deref_out_opt LPWSTR*             ppszEnumContext)
{

    HRESULT         hr              = S_OK;
    GUID            guidContext     = GUID_NULL;
    CComBSTR        bstrContext;
    EnumContext*    pContext        = NULL;

    if((pContextMap     == NULL) ||
       (pszParentID     == NULL) ||
       (ppszEnumContext == NULL))
    {
        hr = E_POINTER;
        CHECK_HR(hr, "Cannot have NULL parameter");
        return hr;
    }

    *ppszEnumContext = NULL;

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
        pContext = new EnumContext();
        if(pContext != NULL)
        {
            CAtlStringW strKey = bstrContext;
            pContext->ParentID = pszParentID;

            hr = pContextMap->Add(strKey, pContext);
            CHECK_HR(hr, "Failed to add enumeration context to client context map");

            pContext->Release();
        }
        else
        {
            hr = E_OUTOFMEMORY;
            CHECK_HR(hr, "Failed to allocate enumeration context");
        }
    }

    if (hr == S_OK)
    {
        *ppszEnumContext = AtlAllocTaskWideString(bstrContext);
    }

    return hr;
}

HRESULT WpdObjectEnumerator::DestroyEnumContext(
    __inout ContextMap* pContextMap,
    __in    LPCWSTR     pszEnumContext)
{

    HRESULT     hr  = S_OK;

    CAtlStringW strKey = pszEnumContext;
    pContextMap->Remove(strKey);

    return hr;
}


HRESULT WpdObjectEnumerator::GetObjectIDs(
    __in        ContextMap*                               pContextMap,
    __in        const DWORD                               dwNumObjects,
    __in        LPCWSTR                                   pszEnumContext,
    __deref_out IPortableDevicePropVariantCollection**    ppCollection)
{

    HRESULT                                         hr              = S_OK;
    EnumContext*                                    pContext        = NULL;
    CComPtr<IPortableDevicePropVariantCollection>   pObjectIDs;

    if(ppCollection == NULL)
    {
        hr = E_POINTER;
        CHECK_HR(hr, "Cannot have NULL collection parameter");
        return hr;
    }

    if (hr == S_OK)
    {
        hr = CoCreateInstance(CLSID_PortableDevicePropVariantCollection,
                              NULL,
                              CLSCTX_INPROC_SERVER,
                              IID_PPV_ARGS(&pObjectIDs));
        CHECK_HR(hr, "Failed to CoCreate CLSID_PortableDevicePropVariantCollection");
    }

    if (hr == S_OK)
    {
        CAtlStringW strKey = pszEnumContext;
        pContext = (EnumContext*) pContextMap->GetContext(strKey);
        if(pContext != NULL)
        {
            for(DWORD dwCount = 0; dwCount < dwNumObjects; dwCount++)
            {
                CAtlStringW strObjectID;

                DWORD dwNextStartIndex = 0;
                if(m_pFakeDevice->FindNext(pContext->SearchIndex,
                                           pContext->ParentID,
                                           strObjectID,
                                           &dwNextStartIndex))
                {
                    PropVariantWrapper pvObjectID(strObjectID);

                    hr = pObjectIDs->Add(&pvObjectID);
                    CHECK_HR(hr, "Failed to add object [%ws]", pvObjectID.pwszVal);
                    pContext->SearchIndex = dwNextStartIndex;
                }
            }

            pContext->Release();
        }
        else
        {
            hr = E_INVALIDARG;
            CHECK_HR(hr, "Cannot find enum context [%ws]", pszEnumContext);
        }
    }

    if (hr == S_OK)
    {
        hr = pObjectIDs->QueryInterface(IID_PPV_ARGS(ppCollection));
        CHECK_HR(hr, "Failed to QI for IID_IPortableDevicePropVariantCollection on IPortableDevicePropVariantCollection");
    }
    return hr;
}


