#pragma once

// This class is used to store the context for a specific enumeration.
// Currently, this is done by storing the object index.
class BulkPropertiesContext : public IUnknown
{
public:
    BulkPropertiesContext() :
        NextObject(0),
        m_cRef(1)
    {

    }

    ~BulkPropertiesContext()
    {

    }

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

    HRESULT Initialize(FakeDevice *pFakeDevice);

    HRESULT DispatchWpdMessage(const PROPERTYKEY&   Command,
                               IPortableDeviceValues*      pParams,
                               IPortableDeviceValues*      pResults);

    HRESULT OnGetValuesByObjectListStart(IPortableDeviceValues*  pParams,
                                         IPortableDeviceValues*  pResults);

    HRESULT OnGetValuesByObjectListNext(IPortableDeviceValues*  pParams,
                                        IPortableDeviceValues*  pResults);

    HRESULT OnGetValuesByObjectListEnd(IPortableDeviceValues*  pParams,
                                       IPortableDeviceValues*  pResults);

    HRESULT OnGetValuesByObjectFormatStart(IPortableDeviceValues*  pParams,
                                           IPortableDeviceValues*  pResults);

    HRESULT OnGetValuesByObjectFormatNext(IPortableDeviceValues*  pParams,
                                          IPortableDeviceValues*  pResults);

    HRESULT OnGetValuesByObjectFormatEnd(IPortableDeviceValues*  pParams,
                                         IPortableDeviceValues*  pResults);

    HRESULT OnSetValuesByObjectListStart(IPortableDeviceValues*  pParams,
                                         IPortableDeviceValues*  pResults);

    HRESULT OnSetValuesByObjectListNext(IPortableDeviceValues*  pParams,
                                        IPortableDeviceValues*  pResults);

    HRESULT OnSetValuesByObjectListEnd(IPortableDeviceValues*  pParams,
                                       IPortableDeviceValues*  pResults);

private:

    HRESULT CreateBulkPropertiesContext(
        __inout         ContextMap*                             pContextMap,
        __in            IPortableDevicePropVariantCollection*   pObjectIDs,
        __in            IPortableDeviceKeyCollection*           pProperties,
        __deref_out_opt LPWSTR*                                 ppszBulkPropertiesContext);

    HRESULT CreateBulkPropertiesContext(
        __inout         ContextMap*                             pContextMap,
        __in            REFGUID                                 guidObjectFormat,
        __in            LPCWSTR                                 pszParentObjectID,
        __in            DWORD                                   dwDepth,
        __in            IPortableDeviceKeyCollection*           pProperties,
        __deref_out_opt LPWSTR*                                 ppszBulkPropertiesContext);

    HRESULT CreateBulkPropertiesContext(
        __inout         ContextMap*                             pContextMap,
        __in            IPortableDeviceValuesCollection*        pValuesCollection,
        __deref_out_opt LPWSTR*                                 ppszBulkPropertiesContext);

    HRESULT DestroyBulkPropertiesContext(
        __inout         ContextMap* pContextMap, 
        __in            LPCWSTR     pszBulkPropertiesContext);

    FakeDevice*                     m_pFakeDevice;
};

