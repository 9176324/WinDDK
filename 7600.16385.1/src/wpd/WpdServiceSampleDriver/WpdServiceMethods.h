#pragma once

class CMethodTask;

// This class is used to store the context for a specific method invocation
class ServiceMethodContext : public IUnknown
{
public:
    ServiceMethodContext();
    ~ServiceMethodContext();

    HRESULT Initialize(
        __in WpdServiceMethods*     pServiceMethods,
        __in IPortableDeviceValues* pStartParams,
        __in LPCWSTR                pwszContext);

    VOID InvokeMethod();

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
    HRESULT                        m_hrStatus;
    CComPtr<IPortableDeviceValues> m_pResults;

private:
    DWORD                          m_cRef;
    CAtlStringW                    m_strContext;
    CMethodTask*                   m_pTask;
    CComPtr<IPortableDeviceValues> m_pStartParameters;
    WpdServiceMethods*             m_pServiceMethods;
};


class CMethodTask
{
public:
    CMethodTask(__in ServiceMethodContext* pContext);

    ~CMethodTask();

    HRESULT Run();

    static DWORD ThreadProc(LPVOID pData)
    {
        // Initialize COM
        if (SUCCEEDED(CoInitializeEx(NULL, COINIT_MULTITHREADED)))
        {
            ServiceMethodContext* pContext = (ServiceMethodContext*) pData;                
            if (pContext != NULL)
            {
                pContext->AddRef();
                pContext->InvokeMethod();
                pContext->Release();
            }

            // Uninitialize COM
            CoUninitialize();
        }
        return 0;
    }
private:
    HANDLE                m_hThread;
    ServiceMethodContext* m_pContext;
};

class WpdServiceMethods
{
public:
    WpdServiceMethods();
    virtual ~WpdServiceMethods();

    HRESULT Initialize(
        __in    FakeContactsService*    pContactsService);

    // Handler for WPD_COMMAND_SERVICE_METHODS_START_INVOKE
    HRESULT OnStartInvoke(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

    // Handler for WPD_COMMAND_SERVICE_METHODS_END_INVOKE
    HRESULT OnEndInvoke(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

    // Handler for WPD_COMMAND_SERVICE_METHODS_CANCEL_INVOKE
    HRESULT OnCancelInvoke(
        __in    IPortableDeviceValues*  pParams,
        __out   IPortableDeviceValues*  pResults);

    HRESULT DispatchMethod(
        __in        LPCWSTR                 pwszContext,
        __in        IPortableDeviceValues*  pStartParams,
        __deref_out IPortableDeviceValues** ppResults);

private:
    HRESULT StartMethod(
        __in            IPortableDeviceValues*  pCommandParams,
        __deref_out_opt LPWSTR*                 ppwszMethodContext);

    HRESULT EndMethod(
        __in        ContextMap*             pContextMap, 
        __in        LPCWSTR                 pwszMethodContext,
        __deref_out IPortableDeviceValues** ppResults,
        __out       HRESULT*                phrStatus);

    HRESULT CancelMethod(
        __in        ContextMap*             pContextMap, 
        __in        LPCWSTR                 pwszMethodContext);

private:
    FakeContactsService* m_pContactsService;
};


