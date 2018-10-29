// Queue.h : Declaration of the CQueue

#pragma once
#include "resource.h"       // main symbols
#include "WpdServiceSampleDriver.h"

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
        __deref_out IUnknown        **ppUkwn)
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
        __in  IWDFIoQueue *pQueue,
        __in  IWDFIoRequest* pRequest,
        __in  IWDFFile*      pFileObject
        );

    //
    // IQueueCallbackDeviceIoControl
    //
    STDMETHOD_ (void, OnDeviceIoControl)(
        __in    IWDFIoQueue*    pQueue,
        __in    IWDFIoRequest*  pRequest,
                ULONG           ControlCode,
                SIZE_T          InputBufferSizeInBytes,
                SIZE_T          OutputBufferSizeInBytes
        );

    //
    // IObjectCleanup
    //
    STDMETHOD_ (void, OnCleanup)(
        __in    IWDFObject* pWdfObject
        );

private:
    HRESULT ProcessWpdMessage(
                ULONG       ControlCode,
        __in    ContextMap* pClientContextMap,
        __in    LPCWSTR     pszFileName,
        __in    IWDFDevice* pDevice,
        __in    PVOID       pInBuffer,
                ULONG       ulInputBufferLength,
        __out   PVOID       pOutBuffer,
                ULONG       ulOutputBufferLength,
        __out   DWORD*      pdwBytesWritten);

    HRESULT GetWpdBaseDriver(
        __in    IWDFDevice*     pDevice,
        __out   WpdBaseDriver** ppWpdBaseDriver);

    HRESULT GetFileName(
        __in            IWDFFile*   pFileObject,
        __deref_out_opt LPWSTR*     ppszFilename);

    CComPtr<IWpdSerializer>         m_pWpdSerializer;
    CComAutoCriticalSection         m_CriticalSection;
};


