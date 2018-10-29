/****************************************************************************
 *
 *  SampUSD.CPP
 *
 *  Copyright (C) Microsoft Corporation 1996-1999
 *  All rights reserved
 *
 ***************************************************************************/

#define INITGUID

#include "Sampusd.h"
#include "resource.h"

/*****************************************************************************
 *
 *      Globals
 *
 *****************************************************************************/

// Reference counter for the whole library
DWORD       g_cRef;

// DLL module instance
HINSTANCE   g_hInst;

/*****************************************************************************
 *
 *  @doc    INTERNAL
 *
 *  @func   void | DllAddRef |
 *
 *          Increment the reference count on the DLL.
 *
 *****************************************************************************/

void
DllAddRef(void)
{
    InterlockedIncrement((LPLONG)&g_cRef);
}

/*****************************************************************************
 *
 *  @doc    INTERNAL
 *
 *  @func   void | DllRelease |
 *
 *          Decrement the reference count on the DLL.
 *
 *****************************************************************************/

void
DllRelease(void)
{
    InterlockedDecrement((LPLONG)&g_cRef);
}


/*****************************************************************************
 *
 *  @class UsdSampClassFactory |
 *
 *****************************************************************************/

class UsdSampClassFactory : public IClassFactory
{
private:
    ULONG   m_cRef;

public:
    STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppv);
    STDMETHODIMP_(ULONG) AddRef(void);
    STDMETHODIMP_(ULONG) Release(void);

    STDMETHODIMP CreateInstance(
            /* [unique][in] */ IUnknown __RPC_FAR *pUnkOuter,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

    STDMETHODIMP LockServer(
            /* [in] */ BOOL fLock);

    UsdSampClassFactory();
};

UsdSampClassFactory::UsdSampClassFactory()
{
    //DEBUGPRINTF((DBG_LVL_DEBUG, TEXT("UsdSampClassFactory: Constructor")));

    // Constructor logic
    m_cRef = 0;
}


STDMETHODIMP UsdSampClassFactory::QueryInterface(
    /* [in] */ REFIID riid,
    /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject)
{
    // DEBUGPRINTF((DBG_LVL_DEBUG, TEXT("UsdSampClassFactory: QueryInterface")));

    *ppvObject = NULL;

    if (IsEqualIID(riid, IID_IUnknown) ||
        IsEqualIID(riid, IID_IClassFactory)) {
        *ppvObject = (LPVOID)this;
        AddRef();
        return NOERROR;
    }
    return ResultFromScode(E_NOINTERFACE);
}


STDMETHODIMP_(ULONG) UsdSampClassFactory::AddRef(void)
{
    DllAddRef();
    return ++m_cRef;
}


STDMETHODIMP_(ULONG) UsdSampClassFactory::Release(void)
{
    DllRelease();
    if (--m_cRef == 0) {
        delete this;
        return 0;
    }
    return m_cRef;
}


STDMETHODIMP UsdSampClassFactory::CreateInstance(
    /* [unique][in] */ IUnknown __RPC_FAR *punkOuter,
    /* [in] */ REFIID riid,
    /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject)
{

    if (ppvObject == NULL)
    {
        return E_INVALIDARG;
    }
    *ppvObject = NULL;

    if (!IsEqualIID(riid, IID_IStiUSD) &&
        !IsEqualIID(riid, IID_IUnknown)) {
        return STIERR_NOINTERFACE;
    }

    // When created for aggregation, only IUnknown can be requested, so fail
    // call if not.
    if (punkOuter && !IsEqualIID(riid, IID_IUnknown)) {
        return CLASS_E_NOAGGREGATION;
    }

    UsdSampDevice    *pDev = NULL;
    HRESULT         hres;

#pragma prefast(suppress:__WARNING_ALIASED_MEMORY_LEAK, "pDev freed when Release()'d.")
    pDev = new UsdSampDevice(punkOuter);
    if (!pDev) {
        return STIERR_OUTOFMEMORY;
    }

    //
    // Move to the requested interface
    //
    hres = pDev->NonDelegatingQueryInterface(riid,ppvObject);
    pDev->NonDelegatingRelease();

    return hres;
}


STDMETHODIMP UsdSampClassFactory::LockServer(
    /* [in] */ BOOL fLock)
{
    if (fLock) {
        DllAddRef();
    } else {
        DllRelease();
    }

    return NOERROR;
}

/*****************************************************************************
 *
 *  @class UsdSampDevice | INonDelegatingUnknown
 *
 *****************************************************************************/
STDMETHODIMP UsdSampDevice::NonDelegatingQueryInterface( REFIID riid, LPVOID* ppvObj )
{
    HRESULT hres;

    if( !IsValid() || !ppvObj )
    {
        return STIERR_INVALID_PARAM;
    }

    *ppvObj = NULL;

    if( IsEqualIID( riid, IID_IUnknown ))
    {
        *ppvObj = static_cast<INonDelegatingUnknown*>(this);
        hres = S_OK;
    }
    else if( IsEqualIID( riid, IID_IStiUSD ))
    {
        *ppvObj = static_cast<IStiUSD*>(this);
        hres = S_OK;
    }
    else
    {
        hres =  STIERR_NOINTERFACE;
    }

    if (SUCCEEDED(hres)) {
        (reinterpret_cast<IUnknown*>(*ppvObj))->AddRef();
    }

    return hres;
}


STDMETHODIMP_(ULONG) UsdSampDevice::NonDelegatingAddRef( VOID )
{
    ULONG ulRef;
    ulRef = InterlockedIncrement((LPLONG)&m_cRef);
    return ulRef;
}

STDMETHODIMP_(ULONG) UsdSampDevice::NonDelegatingRelease( VOID )
{
    ULONG ulRef;
    ulRef = InterlockedDecrement((LPLONG)&m_cRef);

    if(!ulRef)
    {
        delete this;
    }

    return ulRef;
}

/*****************************************************************************
 *
 *  @class UsdSampDevice | IUnknown (Delegating)
 *
 *         Delegating unknown methods.
 *
 *****************************************************************************/

STDMETHODIMP UsdSampDevice::QueryInterface( REFIID riid, LPVOID* ppvObj )
{
    return m_punkOuter->QueryInterface(riid,ppvObj);
}


STDMETHODIMP_(ULONG) UsdSampDevice::AddRef( VOID )
{
    return m_punkOuter->AddRef();
}

STDMETHODIMP_(ULONG) UsdSampDevice::Release( VOID )
{
    return m_punkOuter->Release();
}


/*****************************************************************************
 *
 *  @doc    INTERNAL
 *
 *  @func   BOOL | DllEntryPoint |
 *
 *          Called to notify the DLL about various things that can happen.
 *
 *          We are not interested in thread attaches and detaches,
 *          so we disable thread notifications for performance reasons.
 *
 *  @parm   HINSTANCE | hinst |
 *
 *          The instance handle of this DLL.
 *
 *  @parm   DWORD | dwReason |
 *
 *          Notification code.
 *
 *  @parm   LPVOID | lpReserved |
 *
 *          Not used.
 *
 *  @returns
 *
 *          Returns <c TRUE> to allow the DLL to load.
 *
 *****************************************************************************/


extern "C"
DLLEXPORT
BOOL APIENTRY
DllEntryPoint(HINSTANCE hinst, DWORD dwReason, LPVOID lpReserved)
{
    UNREFERENCED_PARAMETER(lpReserved);

    BOOL bSuccess = TRUE;

    switch (dwReason) {
        case DLL_PROCESS_ATTACH:

        g_hInst = hinst;

        // Disable thread library calls to avoid
        // deadlock when we spin up the worker thread

        DisableThreadLibraryCalls(hinst);

        break;

    case DLL_PROCESS_DETACH:
        if (g_cRef) {
            // DPRINTF("Sampusd: Unloaded before all objects Release()d! Crash soon!\r\n");
        }

        break;
    }

    return bSuccess;
}


extern "C"
DLLEXPORT
BOOL WINAPI
DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID lpReserved)
{
    return DllEntryPoint(hinst, dwReason, lpReserved);
}

/*****************************************************************************
 *
 *  @doc    INTERNAL
 *
 *  @func   HRESULT | DllCanUnloadNow |
 *
 *          Determine whether the DLL has any outstanding interfaces.
 *
 *  @returns
 *
 *          Returns <c S_OK> if the DLL can unload, <c S_FALSE> if
 *          it is not safe to unload.
 *
 *****************************************************************************/
extern "C"
STDMETHODIMP
DllCanUnloadNow(void)
{
    HRESULT hres;

    hres = g_cRef ? S_FALSE : S_OK;

    return hres;
}

/*****************************************************************************
 *
 *  @doc    INTERNAL
 *
 *  @func   HRESULT | DllGetClassObject |
 *
 *          Create an <i IClassFactory> instance for this DLL.
 *
 *  @parm   REFCLSID | rclsid |
 *
 *          The object being requested.
 *
 *  @parm   RIID | riid |
 *
 *          The desired interface on the object.
 *
 *  @parm   PPV | ppvOut |
 *
 *          Output pointer.
 *
 *  @notes
 *          We support only one class of objects , so this function does not need
 *          to go through table of supported classes , looking for proper constructor
 *
 *
 *****************************************************************************/
#pragma prefast(push)
#pragma prefast(disable:__WARNING_RETURNING_BAD_RESULT, "*ppv set to NULL in case of error")
extern "C"
STDAPI DllGetClassObject(
    __in            REFCLSID    rclsid,
    __in            REFIID      riid,
    __deref_out     LPVOID      *ppv)
{

    if (!ppv) {
        return ResultFromScode(E_FAIL);
    }

    *ppv = NULL;
    if (!IsEqualCLSID(rclsid, CLSID_SampUSDObj) ) {
        return ResultFromScode(E_FAIL);
    }

    if (!IsEqualIID(riid, IID_IUnknown) &&
        !IsEqualIID(riid, IID_IClassFactory)) {
        return ResultFromScode(E_NOINTERFACE);
    }

    if (IsEqualCLSID(rclsid, CLSID_SampUSDObj)) {
        UsdSampClassFactory *pcf = new UsdSampClassFactory;
        if (pcf) {
            *ppv = (LPVOID)pcf;
        }
        if (!pcf) {
            return E_OUTOFMEMORY;
        }
    }

    return NOERROR;
}
#pragma prefast(popd)




