#pragma once

// This class is used to store the context for a specific enumeration.
class WpdObjectEnumeratorContext : public IUnknown
{
public:
    WpdObjectEnumeratorContext() :
        m_cRef(1),
        m_TotalChildren(0),
        m_ChildrenEnumerated(0)
    {

    }

    ~WpdObjectEnumeratorContext()
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

public:
    BOOL HasMoreChildrenToEnumerate()
    {
        return (m_TotalChildren > m_ChildrenEnumerated)?TRUE:FALSE;
    }

// WpdObjectEnumeratorContext specific data
public:
    CAtlStringW m_strParentObjectID;    // object identifier of the object whose children are being enumerated
    DWORD       m_TotalChildren;        // number of bytes transferred from the resource to the caller
    DWORD       m_ChildrenEnumerated;   // number of children returned during the enumeration operation
};

class WpdObjectEnumerator
{
public:
    WpdObjectEnumerator();
    virtual ~WpdObjectEnumerator();

    HRESULT Initialize(WpdBaseDriver* pBaseDriver);


    HRESULT DispatchWpdMessage(const PROPERTYKEY&     Command,
                               IPortableDeviceValues* pParams,
                               IPortableDeviceValues* pResults);

    HRESULT OnStartFind(IPortableDeviceValues*  pParams,
                        IPortableDeviceValues*  pResults);

    HRESULT OnFindNext(IPortableDeviceValues*  pParams,
                       IPortableDeviceValues*  pResults);

    HRESULT OnEndFind(IPortableDeviceValues*  pParams,
                      IPortableDeviceValues*  pResults);

private:
    WpdBaseDriver*           m_pBaseDriver;

    VOID InitializeEnumerationContext(
        WpdObjectEnumeratorContext* pEnumeratorContext,
        CAtlStringW                 strParentObjectID);

    HRESULT AddStringValueToPropVariantCollection(
        IPortableDevicePropVariantCollection* pCollection,
        LPCWSTR                               wszValue);
};

