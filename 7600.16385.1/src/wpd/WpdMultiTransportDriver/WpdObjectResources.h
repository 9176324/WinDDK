#pragma once

#define FILE_OPTIMAL_READ_BUFFER_SIZE_VALUE  (2 * 1024 * 1024)
#define FILE_OPTIMAL_WRITE_BUFFER_SIZE_VALUE (2 * 1024 * 1024)

// This class is used to store the context for a specific resource operation.
class WpdObjectResourceContext : public IUnknown
{
public:
    WpdObjectResourceContext() :
        m_cRef(1),
        m_Resource(WPD_RESOURCE_DEFAULT),
        m_BytesTransferred(0),
        m_BytesTotal(0)
    {

    }

    ~WpdObjectResourceContext()
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

private:
    DWORD m_cRef;

// WpdObjectResourceContext specific data
public:
    CAtlStringW m_strObjectID;      // object identifier of the object whose resource is being transferred
    PROPERTYKEY m_Resource;         // the specific resource being transferred
    ULONGLONG   m_BytesTransferred; // number of bytes transferred from the resource to the caller
    ULONGLONG   m_BytesTotal;       // total number of bytes of the resource data
};

class WpdObjectResources
{
public:
    WpdObjectResources();
    virtual ~WpdObjectResources();

    HRESULT DispatchWpdMessage(const PROPERTYKEY&      Command,
                               IPortableDeviceValues*  pParams,
                               IPortableDeviceValues*  pResults);

    HRESULT OnGetSupportedResources(IPortableDeviceValues*  pParams,
                                    IPortableDeviceValues*  pResults);

    HRESULT OnGetResourceAttributes(IPortableDeviceValues*  pParams,
                                    IPortableDeviceValues*  pResults);

    HRESULT OnOpenResource(IPortableDeviceValues*  pParams,
                           IPortableDeviceValues*  pResults);

    HRESULT OnReadResource(IPortableDeviceValues*  pParams,
                           IPortableDeviceValues*  pResults);

    HRESULT OnCloseResource(IPortableDeviceValues*  pParams,
                            IPortableDeviceValues*  pResults);
private:
    HRESULT GetSupportedResourcesForObject(LPCWSTR                       wszObjectID,
                                           IPortableDeviceKeyCollection* pKeys);

    HRESULT GetResourceAttributesForObject(LPCWSTR                wszObjectID,
                                           const PROPERTYKEY&     Key,
                                           IPortableDeviceValues* pAttributes);

    HRESULT ReadDataFromResource(WpdObjectResourceContext* pResourceContext,
                                 __out_bcount(dwNumBytesToRead) BYTE*                     pBuffer,
                                 DWORD                     dwNumBytesToRead,
                                 DWORD*                    pdwNumBytesRead);
};

