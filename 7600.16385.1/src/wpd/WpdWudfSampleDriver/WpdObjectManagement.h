#pragma once

// This class is used to store the context for a specific enumeration.
// Currently, this is done by storing the object index.
class ObjectManagementContext  : public IUnknown
{
public:
    ObjectManagementContext() :
        m_cRef(1),
        UpdateRequest(FALSE)
    {

    }

    ~ObjectManagementContext()
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
    CAtlStringW                     Name;
    CComPtr<IPortableDeviceValues>  ObjectProperties;
    BOOL                            UpdateRequest;
private:
    DWORD                           m_cRef;
};

class WpdObjectManagement
{
public:
    WpdObjectManagement();
    ~WpdObjectManagement();

    HRESULT Initialize(FakeDevice *pFakeDevice);

    HRESULT DispatchWpdMessage(const PROPERTYKEY&   Command,
                               IPortableDeviceValues*      pParams,
                               IPortableDeviceValues*      pResults);

    HRESULT OnDelete(IPortableDeviceValues*  pParams,
                     IPortableDeviceValues*  pResults);

    HRESULT OnCreateObjectWithPropertiesOnly(IPortableDeviceValues*  pParams,
                                             IPortableDeviceValues*  pResults);

    HRESULT OnCreateObjectWithPropertiesAndData(IPortableDeviceValues*  pParams,
                                                IPortableDeviceValues*  pResults);

    HRESULT OnWriteObjectData(IPortableDeviceValues*  pParams,
                              IPortableDeviceValues*  pResults);

    HRESULT OnRevert(IPortableDeviceValues*  pParams,
                     IPortableDeviceValues*  pResults);

    HRESULT OnCommit(IPortableDeviceValues*  pParams,
                     IPortableDeviceValues*  pResults);

    HRESULT OnMove(IPortableDeviceValues*  pParams,
                   IPortableDeviceValues*  pResults);

    HRESULT OnCopy(IPortableDeviceValues*  pParams,
                   IPortableDeviceValues*  pResults);

    HRESULT OnUpdateObjectWithPropertiesAndData(IPortableDeviceValues*  pParams,
                                                IPortableDeviceValues*  pResults);

private:
    HRESULT CreateObjectManagementContext(
        __inout         ContextMap*             pContextMap,
        __in            LPCWSTR                 pszObjectName,
        __in            IPortableDeviceValues*  pObjectProperties,
        __in            BOOL                    bUpdateRequest,
        __deref_out_opt LPWSTR*                 ppszObjectManagementContext);

    HRESULT DestroyObjectManagementContext(
        __inout         ContextMap*             pContextMap,
        __in            LPCWSTR                 pszObjectManagementContext);

    HRESULT CommitNewObject(
        __in            ObjectManagementContext*    pContext,
        __in            LPCWSTR                     pszContext,
        __in            IPortableDeviceValues*      pParams,
        __out           IPortableDeviceValues*      pResults);

    HRESULT UpdateObject(
        __in            ObjectManagementContext*    pContext,
        __in            IPortableDeviceValues*      pParams,
        __out           IPortableDeviceValues*      pResults);

    FakeDevice* m_pFakeDevice;
};

