#pragma once

// This class is used to store the context for a specific enumeration.
// Currently, this is done by storing the object index.
class BulkPropertiesContext : public IUnknown
{
public:
    BulkPropertiesContext() :
        Scope(FULL_DEVICE_ACCESS),
        NextObject(0),
        m_cRef(1)
    {

    }

    ~BulkPropertiesContext()
    {

    }

    ACCESS_SCOPE                                  Scope;
    CComPtr<IPortableDevicePropVariantCollection> ObjectIDs;
    DWORD                                         NextObject;
    CComPtr<IPortableDeviceKeyCollection>         Properties;
    CComPtr<IPortableDeviceValuesCollection>      ValuesCollection;

public: // IUnknown
    ULONG __stdcall AddRef()
    {
        InterlockedIncrement((long*) &m_cRef);
        return m_cRef;
    }

    __drv_at(this, __drv_freesMem(Mem)) 
    ULONG __stdcall Release()
    {
        ULONG ulRefCount = m_cRef - 1;

        if (InterlockedDecrement((long*) &m_cRef) == 0)
        {
            delete this;
            return 0;
        }
        return ulRefCount;
    }

    HRESULT __stdcall QueryInterface(
        REFIID riid,
        void** ppv)
    {
        HRESULT hr = S_OK;

        if(riid == IID_IUnknown)
        {
            *ppv = static_cast<IUnknown*>(this);
            AddRef();
        }
        else
        {
            *ppv = NULL;
            hr = E_NOINTERFACE;
        }

        return hr;
    }

private:
    DWORD m_cRef;
};

class WpdObjectPropertiesBulk
{
public:
    WpdObjectPropertiesBulk();
    ~WpdObjectPropertiesBulk();

    HRESULT Initialize(__in FakeDevice *pDevice);

    HRESULT DispatchWpdMessage(
                 REFPROPERTYKEY         Command,
        __in     IPortableDeviceValues* pParams,
        __out    IPortableDeviceValues* pResults);

    HRESULT OnGetValuesByObjectListStart(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

    HRESULT OnGetValuesByObjectListNext(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

    HRESULT OnGetValuesByObjectListEnd(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

    HRESULT OnGetValuesByObjectFormatStart(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

    HRESULT OnGetValuesByObjectFormatNext(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

    HRESULT OnGetValuesByObjectFormatEnd(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

    HRESULT OnSetValuesByObjectListStart(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

    HRESULT OnSetValuesByObjectListNext(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

    HRESULT OnSetValuesByObjectListEnd(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

private:
    HRESULT CreateBulkPropertiesContext(
        __in            ACCESS_SCOPE                          Scope,
        __inout         ContextMap*                           pContextMap,
        __in            IPortableDevicePropVariantCollection* pObjectIDs,
        __in            IPortableDeviceKeyCollection*         pProperties,
        __deref_out_opt LPWSTR*                               ppszBulkPropertiesContext);

    HRESULT CreateBulkPropertiesContext(
        __in            ACCESS_SCOPE                          Scope,
        __inout         ContextMap*                           pContextMap,
        __in            REFGUID                               guidObjectFormat,
        __in            LPCWSTR                               pszParentObjectID,
        __in            DWORD                                 dwDepth,
        __in            IPortableDeviceKeyCollection*         pProperties,
        __deref_out_opt LPWSTR*                               ppszBulkPropertiesContext);

    HRESULT CreateBulkPropertiesContext(
        __in            ACCESS_SCOPE                          Scope,
        __inout         ContextMap*                           pContextMap,
        __in            IPortableDeviceValuesCollection*      pValuesCollection,
        __deref_out_opt LPWSTR*                               ppszBulkPropertiesContext);

    HRESULT DestroyBulkPropertiesContext(
        __inout         ContextMap*                           pContextMap, 
        __in            LPCWSTR                               pszBulkPropertiesContext);

    FakeDevice* m_pDevice;
};

