#pragma once

// This context is used for managing reads/writes for data transfer.
// It keeps track of the number of bytes read/written for the current transfer.
class ResourceContext : public IUnknown
{
public:
    ResourceContext() :
        NumBytesTransfered(0),
        m_cRef(1),
        CreateRequest(FALSE)
    {


        Key = WPD_PROPERTY_NULL;
    }

    ~ResourceContext()
    {

    }

    CAtlStringW ObjectID;
    PROPERTYKEY Key;
    DWORD       NumBytesTransfered;
    BOOL        CreateRequest;

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
    DWORD       m_cRef;
};

class WpdObjectResources
{
public:
    WpdObjectResources();
    ~WpdObjectResources();

    HRESULT Initialize(FakeDevice *pFakeDevice);

    HRESULT DispatchWpdMessage(const PROPERTYKEY&   Command,
                               IPortableDeviceValues*      pParams,
                               IPortableDeviceValues*      pResults);

    HRESULT OnGetSupportedResources(IPortableDeviceValues*  pParams,
                                    IPortableDeviceValues*  pResults);

    HRESULT OnGetAttributes(IPortableDeviceValues*  pParams,
                            IPortableDeviceValues*  pResults);

    HRESULT OnOpen(IPortableDeviceValues*  pParams,
                   IPortableDeviceValues*  pResults);

    HRESULT OnRead(IPortableDeviceValues*  pParams,
                   IPortableDeviceValues*  pResults);

    HRESULT OnWrite(IPortableDeviceValues*  pParams,
                    IPortableDeviceValues*  pResults);

    HRESULT OnClose(IPortableDeviceValues*  pParams,
                    IPortableDeviceValues*  pResults);

    HRESULT OnDelete(IPortableDeviceValues* pParams,
                     IPortableDeviceValues* pResults);

    HRESULT OnCreate(IPortableDeviceValues* pParams,
                     IPortableDeviceValues* pResults);

    HRESULT OnRevert(IPortableDeviceValues* pParams,
                     IPortableDeviceValues* pResults);

    HRESULT OnSeek(IPortableDeviceValues* pParams,
                     IPortableDeviceValues* pResults);

private:
    HRESULT CreateResourceContext(
        __inout         ContextMap*     pContextMap,
        __in            LPCWSTR         pszObjectID,
        __in            REFPROPERTYKEY  ResourceKey,
        __in            BOOL            bCreateRequest,
        __deref_out_opt LPWSTR*         ppszResourceContext);

    HRESULT DestroyResourceContext(
        __inout         ContextMap*     pContextMap,
        __in            LPCWSTR         pszResourceContext);

private:

    FakeDevice*                      m_pFakeDevice;
};

