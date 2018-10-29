// Queue.h : Declaration of the CQueue

#pragma once
#include "resource.h"       // main symbols
#include "WpdMultiTransportDriver.h"

class ATL_NO_VTABLE CDefaultQueue :
    public CComObjectRootEx<CComMultiThreadModel>,
    public IQueueCallbackDeviceIoControl,
    public IQueueCallbackCreate
{
public:
    CDefaultQueue()
    {

    }

    DECLARE_NOT_AGGREGATABLE(CDefaultQueue)

    BEGIN_COM_MAP(CDefaultQueue)
        COM_INTERFACE_ENTRY(IQueueCallbackDeviceIoControl)
        COM_INTERFACE_ENTRY(IQueueCallbackCreate)
    END_COM_MAP()

public:
    static
    HRESULT CreateInstance(
        IUnknown        **ppUkwn)
    {
        CComObject< CDefaultQueue> *pMyQueue = NULL;
        HRESULT hr = CComObject<CDefaultQueue>::CreateInstance( &pMyQueue );
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

#if 0
    STDMETHOD_ (void, OnCleanupFile)(
        /*[in]*/ IWDFIoQueue *pQueue,
        /*[in]*/ IWDFIoRequest *pRequest,
        /*[in]*/ IWDFFile *pFileObject
        );

    STDMETHOD_ (void, OnCloseFile)(
        /*[in]*/ IWDFIoQueue *pQueue,
        /*[in]*/ IWDFIoRequest* pRequest,
        /*[in]*/ IWDFFile* pFileObject
        );
#endif

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
};


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

#if 0
    STDMETHOD_ (void, OnCleanupFile)(
        /*[in]*/ IWDFIoQueue *pQueue,
        /*[in]*/ IWDFIoRequest *pRequest,
        /*[in]*/ IWDFFile *pFileObject
        );

    STDMETHOD_ (void, OnCloseFile)(
        /*[in]*/ IWDFIoQueue *pQueue,
        /*[in]*/ IWDFIoRequest* pRequest,
        /*[in]*/ IWDFFile* pFileObject
        );
#endif

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

    CComPtr<IWpdSerializer>         m_pWpdSerializer;
    CComAutoCriticalSection         m_CriticalSection;
};


