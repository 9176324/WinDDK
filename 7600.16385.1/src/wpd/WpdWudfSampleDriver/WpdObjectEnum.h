#pragma once

// This class is used to store the context for a specific enumeration.
// Currrently, this is done by storing the object index.
class EnumContext : public IUnknown
{
public:
    EnumContext() :
        m_cRef(1),
        SearchIndex(0)
    {

    }

    ~EnumContext()
    {

    }

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

public:
    DWORD       SearchIndex;
    CAtlStringW ParentID;
private:
    DWORD       m_cRef;
};

class WpdObjectEnumerator
{
public:
    WpdObjectEnumerator();
    ~WpdObjectEnumerator();

    HRESULT Initialize(FakeDevice *pFakeDevice);

    HRESULT DispatchWpdMessage(const PROPERTYKEY&   Command,
                               IPortableDeviceValues*      pParams,
                               IPortableDeviceValues*      pResults);

    HRESULT OnStartFind(IPortableDeviceValues*  pParams,
                        IPortableDeviceValues*  pResults);

    HRESULT OnFindNext(IPortableDeviceValues*  pParams,
                       IPortableDeviceValues*  pResults);

    HRESULT OnEndFind(IPortableDeviceValues*  pParams,
                      IPortableDeviceValues*  pResults);

private:
    HRESULT CreateEnumContext(
        __inout         ContextMap* pContextMap,  
        __in            LPCWSTR     pszParentID, 
        __deref_out_opt LPWSTR*     ppszEnumContext);

    HRESULT DestroyEnumContext(
        __inout         ContextMap* pContextMap, 
        __in            LPCWSTR     pszEnumContext);

    HRESULT GetObjectIDs(
        __in            ContextMap*                             pContextMap, 
        __in            const DWORD                             dwNumObjects, 
        __in            LPCWSTR                                 pszEnumContext, 
        __deref_out     IPortableDevicePropVariantCollection**  ppCollection);

    FakeDevice* m_pFakeDevice;
};

