// Queue.h : Declaration of the CQueue

#pragma once
#include "resource.h"       // main symbols
#include "WpdBasicHardwareDriver.h"

class ATL_NO_VTABLE CQueue :
    public CComObjectRootEx<CComMultiThreadModel>,
    public IQueueCallbackDeviceIoControl,
    public IQueueCallbackCreate,
    public IObjectCleanup
{
public:
    CQueue()
    {

    }

    DECLARE_NOT_AGGREGATABLE(CQueue)

    BEGIN_COM_MAP(CQueue)
        COM_INTERFACE_ENTRY(IQueueCallbackDeviceIoControl)
        COM_INTERFACE_ENTRY(IQueueCallbackCreate)
    END_COM_MAP()

public:
    static
    HRESULT CreateInstance(
        IUnknown        **ppUkwn)
    {
        CComObject< CQueue> *pMyQueue = NULL;
        HRESULT hr = CComObject<CQueue>::CreateInstance( &pMyQueue );
        if( SUCCEEDED (hr) )
        {
            pMyQueue->AddRef();
            hr = pMyQueue->QueryInterface( __uuidof(IUnknown), (void **) ppUkwn );
            pMyQueue->Release();
            pMyQueue = NULL;
        }

        return hr;
    }

    //
    // Wdf Callbacks
    //

    // IQueueCallbackCreateClose
    //
    STDMETHOD_ (void, OnCreateFile)(
        /*[in]*/ IWDFIoQueue *pQueue,
        /*[in]*/ IWDFIoRequest *pRequest,
        /*[in]*/ IWDFFile *pFileObject
        );

    //
    // IQueueCallbackDeviceIoControl
    //
    STDMETHOD_ (void, OnDeviceIoControl)(
        IWDFIoQueue*    pQueue,
        IWDFIoRequest*  pRequest,
        ULONG           ControlCode,
        SIZE_T          InputBufferSizeInBytes,
        SIZE_T          OutputBufferSizeInBytes
        );

    //
    // IObjectCleanup
    //
    STDMETHOD_ (void, OnCleanup)(
        IWDFObject* pWdfObject
        );

private:
    HRESULT ProcessWpdMessage(
        ULONG       ControlCode,
        ContextMap* pClientContextMap,
        IWDFDevice* pDevice,
        PVOID       pInBuffer,
        ULONG       ulInputBufferLength,
        PVOID       pOutBuffer,
        ULONG       ulOutputBufferLength,
        DWORD*      pdwBytesWritten);

    HRESULT GetWpdBaseDriver(
        IWDFDevice*     pDevice,
        WpdBaseDriver** ppWpdBaseDriver);

    CComPtr<IWpdSerializer>         m_pWpdSerializer;
    CComAutoCriticalSection         m_CriticalSection;
};


