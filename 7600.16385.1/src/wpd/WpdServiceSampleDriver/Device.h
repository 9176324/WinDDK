#pragma once

#include "resource.h"
#include "WpdServiceSampleDriver.h"

class ATL_NO_VTABLE CDevice :
    public CComObjectRootEx<CComMultiThreadModel>,
    public IPnpCallback,
    public IPnpCallbackSelfManagedIo,
    public IPnpCallbackHardware
{
public:
    CDevice() :
        m_pWpdBaseDriver(NULL)
    {
    }

    DECLARE_NOT_AGGREGATABLE(CDevice)

    BEGIN_COM_MAP(CDevice)
        COM_INTERFACE_ENTRY(IPnpCallback)
        COM_INTERFACE_ENTRY(IPnpCallbackSelfManagedIo)
        COM_INTERFACE_ENTRY(IPnpCallbackHardware)
    END_COM_MAP()

public:
    static HRESULT
    CreateInstance(
        __deref_out IUnknown**              ppUnkwn,
        __in        IWDFDeviceInitialize*   pDeviceInit,
        __in        WpdBaseDriver*          pWpdBaseDriver)
    {
        *ppUnkwn = NULL;

        //
        // Set device properties.
        //
        pDeviceInit->SetLockingConstraint(None);

        CComObject< CDevice> *pMyDevice = NULL;
        HRESULT hr = CComObject<CDevice>::CreateInstance( &pMyDevice );
        if( SUCCEEDED (hr) )
        {
            pMyDevice->AddRef();
            hr = pMyDevice->QueryInterface( __uuidof(IUnknown),(void **) ppUnkwn);
            if (hr == S_OK)
            {
                pMyDevice->m_pWpdBaseDriver = pWpdBaseDriver;
            }
            pMyDevice->Release();
            pMyDevice = NULL;
        }

        return hr;
    }

    // IPnpCallback
    //
    STDMETHOD_(HRESULT, OnD0Entry)        (__in IWDFDevice* pDevice,  __in WDF_POWER_DEVICE_STATE previousState);
    STDMETHOD_(HRESULT, OnD0Exit)         (__in IWDFDevice* pDevice,  __in WDF_POWER_DEVICE_STATE newState);
    STDMETHOD_(VOID,    OnSurpriseRemoval)(__in IWDFDevice* pDevice);
    STDMETHOD_(HRESULT, OnQueryRemove)    (__in IWDFDevice* pDevice);
    STDMETHOD_(HRESULT, OnQueryStop)      (__in IWDFDevice* pDevice);

    // IPnpCallbackSelfManagedIo
    //
    STDMETHOD_(VOID,     OnSelfManagedIoCleanup)(__in IWDFDevice* pDevice);
    STDMETHOD_(VOID,     OnSelfManagedIoFlush)  (__in IWDFDevice* pDevice);
    STDMETHOD_(HRESULT, OnSelfManagedIoInit)    (__in IWDFDevice* pDevice);
    STDMETHOD_(HRESULT, OnSelfManagedIoSuspend) (__in IWDFDevice* pDevice);
    STDMETHOD_(HRESULT, OnSelfManagedIoRestart) (__in IWDFDevice* pDevice);
    STDMETHOD_(HRESULT, OnSelfManagedIoStop)    (__in IWDFDevice* pDevice);

    // IPnpCallbackHardware
    //
    STDMETHOD_(HRESULT, OnPrepareHardware)(__in IWDFDevice* pDevice);
    STDMETHOD_(HRESULT, OnReleaseHardware)(__in IWDFDevice* pDevice);

private:
    HRESULT GetSupportedContentTypes(
       __deref_out IPortableDevicePropVariantCollection** ppContentTypes);

    HRESULT GetDeviceFriendlyName(
       __deref_out_opt LPWSTR*   pwszDeviceFriendlyName);

private:

    WpdBaseDriver*                         m_pWpdBaseDriver;
    CComPtr<IPortableDeviceClassExtension> m_pPortableDeviceClassExtension;
};


