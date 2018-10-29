#pragma once

class WpdBaseDriver :
    public IUnknown
{
public:
    WpdBaseDriver();
    virtual ~WpdBaseDriver();

    HRESULT Initialize();
    VOID    Uninitialize();

    HRESULT DispatchWpdMessage(__in     IPortableDeviceValues* pParams,
                               __out    IPortableDeviceValues* pResults);

private:
    HRESULT OnGetObjectIDsFromPersistentUniqueIDs(__in  IPortableDeviceValues* pParams,
                                                  __out IPortableDeviceValues* pResults);


    HRESULT OnSaveClientInfo(__in   IPortableDeviceValues* pParams,
                             __out  IPortableDeviceValues* pResults);

public: // IUnknown
    ULONG __stdcall AddRef();

    __drv_at(this, __drv_freesMem(Mem)) 
    ULONG __stdcall Release();

    HRESULT __stdcall QueryInterface(REFIID riid, void** ppv);

public:
    WpdObjectEnumerator     m_ObjectEnum;
    WpdObjectManagement     m_ObjectManagement;
    WpdObjectProperties     m_ObjectProperties;
    WpdObjectResources      m_ObjectResources;
    WpdObjectPropertiesBulk m_ObjectPropertiesBulk;
    WpdCapabilities         m_Capabilities;
    WpdService              m_Service;

private:
    FakeDevice              m_Device;
    ULONG                   m_cRef;
};


