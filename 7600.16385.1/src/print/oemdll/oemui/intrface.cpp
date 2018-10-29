//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//  ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//  PARTICULAR PURPOSE.
//
//  Copyright  1998 - 2003  Microsoft Corporation.  All Rights Reserved.
//
//  FILE:    Intrface.cpp
//
//  PURPOSE:  Implementation of interface for PScript5 and Unidrv UI plug-ins.
//

#include "precomp.h"

#include "debug.h"
#include "devmode.h"
#include "oemui.h"
#include "intrface.h"

// This indicates to Prefast that this is a usermode driver file.
__user_driver;

////////////////////////////////////////////////////////
//      Internal Globals
////////////////////////////////////////////////////////

static long g_cComponents = 0 ;     // Count of active components
static long g_cServerLocks = 0 ;    // Count of locks

////////////////////////////////////////////////////////////////////////////////
//
// IOemUI body
//
IOemUI::~IOemUI()
{
    // Make sure that helper interface is released.
    if(NULL != m_pOEMHelp)
    {
        m_pOEMHelp->Release();
        m_pOEMHelp = NULL;
    }

    // If this instance of the object is being deleted, then the reference
    // count should be zero.
    assert(0 == m_cRef);
}

HRESULT __stdcall IOemUI::QueryInterface(const IID& iid, void** ppv)
{
    VERBOSE("IOemUI:QueryInterface entry.\r\n\r\n");
    if (iid == IID_IUnknown)
    {
        *ppv = static_cast<IUnknown*>(this);
        VERBOSE("IOemUI:Return pointer to IUnknown.\r\n\r\n");
    }
    else if (iid == IID_IPrintOemUI)
    {
        *ppv = static_cast<IPrintOemUI*>(this) ;
        VERBOSE("IOemUI:Return pointer to IPrintOemUI.\r\n");
    }
    else
    {
        VERBOSE("IOemUI::QueryInterface IID not supported.\r\n");
        *ppv = NULL ;
        return E_NOINTERFACE ;
    }
    reinterpret_cast<IUnknown*>(*ppv)->AddRef() ;
    return S_OK ;
}

ULONG __stdcall IOemUI::AddRef()
{
    VERBOSE("IOemUI:AddRef entry.\r\n");
    return InterlockedIncrement(&m_cRef) ;
}

__drv_at(this, __drv_freesMem(object))
ULONG __stdcall IOemUI::Release()
{
   VERBOSE("IOemUI:Release entry.\r\n");
   ASSERT( 0 != m_cRef);
   ULONG cRef = InterlockedDecrement(&m_cRef);
   if (0 == cRef)
   {
      delete this;

   }
   return cRef;
}

HRESULT __stdcall IOemUI::PublishDriverInterface(
    IUnknown *pIUnknown)
{
    VERBOSE("IOemUI:PublishDriverInterface entry.\r\n");

    // Need to store pointer to Driver Helper functions, if we already haven't.
    if (m_pOEMHelp == NULL)
    {
        HRESULT hResult;


        // Get Interface to Helper Functions.
        hResult = pIUnknown->QueryInterface(IID_IPrintOemDriverUI, (void** ) &(m_pOEMHelp));

        if(!SUCCEEDED(hResult))
        {
            // Make sure that interface pointer reflects interface query failure.
            m_pOEMHelp = NULL;

            return E_FAIL;
        }
    }

    return S_OK;
}

HRESULT __stdcall IOemUI::GetInfo(
    DWORD  dwMode,
    PVOID  pBuffer,
    DWORD  cbSize,
    PDWORD pcbNeeded)
{
    VERBOSE("IOemUI::GetInfo entry.\r\r\n");

    // Validate parameters.
    if( (NULL == pcbNeeded)
        ||
        ( (OEMGI_GETSIGNATURE != dwMode)
          &&
          (OEMGI_GETVERSION != dwMode)
          &&
          (OEMGI_GETPUBLISHERINFO != dwMode)
        )
      )
    {
        WARNING("IOemUI::GetInfo() exit pcbNeeded is NULL! ERROR_INVALID_PARAMETER\r\r\n");
        SetLastError(ERROR_INVALID_PARAMETER);
        return E_FAIL;
    }

    // Set expected buffer size and number of bytes written.
    *pcbNeeded = sizeof(DWORD);

    // Check buffer size is sufficient.
    if((cbSize < *pcbNeeded) || (NULL == pBuffer))
    {
        WARNING("IOemUI::GetInfo() exit insufficient buffer!\r\r\n");
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return E_FAIL;
    }

    switch(dwMode)
    {
        // OEM DLL Signature
        case OEMGI_GETSIGNATURE:
            *(PDWORD)pBuffer = OEM_SIGNATURE;
            break;

        // OEM DLL version
        case OEMGI_GETVERSION:
            *(PDWORD)pBuffer = OEM_VERSION;
            break;

        // dwMode not supported.
        default:
            // Set written bytes to zero since nothing was written.
            WARNING("IOemUI::GetInfo() exit mode not supported.\r\r\n");
            *pcbNeeded = 0;
            SetLastError(ERROR_NOT_SUPPORTED);
            return E_FAIL;
    }

    VERBOSE("IOemUI::GetInfo() exit S_OK\r\n");
    return S_OK;
}

HRESULT __stdcall IOemUI::DevMode(
    DWORD  dwMode,
    POEMDMPARAM pOemDMParam)
{
    VERBOSE("IOemUI:DevMode entry.\r\n");

    return hrOEMDevMode(dwMode, pOemDMParam);
}

HRESULT __stdcall IOemUI::CommonUIProp(
    DWORD  dwMode,
    POEMCUIPPARAM   pOemCUIPParam)
{
    VERBOSE("IOemUI:CommonUIProp entry.\r\n");

    return hrOEMPropertyPage(dwMode, pOemCUIPParam);
}


HRESULT __stdcall IOemUI::DocumentPropertySheets(
    PPROPSHEETUI_INFO   pPSUIInfo,
    LPARAM              lParam)
{
    VERBOSE("IOemUI:DocumentPropertySheets entry.\r\n");

    return hrOEMDocumentPropertySheets(pPSUIInfo, lParam, m_pOEMHelp);
}

HRESULT __stdcall IOemUI::DevicePropertySheets(
    PPROPSHEETUI_INFO   pPSUIInfo,
    LPARAM              lParam)
{
    VERBOSE("IOemUI:DevicePropertySheets entry.\r\n");

    return hrOEMDevicePropertySheets(pPSUIInfo, lParam);
}

HRESULT __stdcall IOemUI::DeviceCapabilities(
            POEMUIOBJ   poemuiobj,
            HANDLE      hPrinter,
            __in PWSTR  pDeviceName,
            WORD        wCapability,
            PVOID       pOutput,
            PDEVMODE    pPublicDM,
            PVOID       pOEMDM,
            DWORD       dwOld,
            DWORD       *dwResult)
{
    VERBOSE("IOemUI:DeviceCapabilities entry.\r\n");

    UNREFERENCED_PARAMETER(poemuiobj);
    UNREFERENCED_PARAMETER(hPrinter);
    UNREFERENCED_PARAMETER(pDeviceName);
    UNREFERENCED_PARAMETER(wCapability);
    UNREFERENCED_PARAMETER(pOutput);
    UNREFERENCED_PARAMETER(pPublicDM);
    UNREFERENCED_PARAMETER(pOEMDM);
    UNREFERENCED_PARAMETER(dwOld);
    UNREFERENCED_PARAMETER(dwResult);

    return E_NOTIMPL;
}

HRESULT __stdcall IOemUI::DevQueryPrintEx(
    POEMUIOBJ               poemuiobj,
    PDEVQUERYPRINT_INFO     pDQPInfo,
    PDEVMODE                pPublicDM,
    PVOID                   pOEMDM)
{
    VERBOSE("IOemUI:DevQueryPrintEx entry.\r\n");

    UNREFERENCED_PARAMETER(poemuiobj);
    UNREFERENCED_PARAMETER(pDQPInfo);
    UNREFERENCED_PARAMETER(pPublicDM);
    UNREFERENCED_PARAMETER(pOEMDM);

    return E_NOTIMPL;
}

HRESULT __stdcall IOemUI::UpgradePrinter(
    DWORD   dwLevel,
    PBYTE   pDriverUpgradeInfo)
{
    VERBOSE("IOemUI:UpgradePrinter entry.\r\n");

    UNREFERENCED_PARAMETER(dwLevel);
    UNREFERENCED_PARAMETER(pDriverUpgradeInfo);

    return E_NOTIMPL;
}

HRESULT __stdcall IOemUI::PrinterEvent(
    __in PWSTR   pPrinterName,
    INT          iDriverEvent,
    DWORD        dwFlags,
    LPARAM       lParam)
{
    VERBOSE("IOemUI:PrinterEvent entry.\r\n");

    UNREFERENCED_PARAMETER(pPrinterName);
    UNREFERENCED_PARAMETER(iDriverEvent);
    UNREFERENCED_PARAMETER(dwFlags);
    UNREFERENCED_PARAMETER(lParam);

    return E_NOTIMPL;
}

HRESULT __stdcall IOemUI::DriverEvent(
    DWORD   dwDriverEvent,
    DWORD   dwLevel,
    LPBYTE  pDriverInfo,
    LPARAM  lParam)
{
    VERBOSE("IOemUI:DriverEvent entry.\r\n");

    UNREFERENCED_PARAMETER(dwDriverEvent);
    UNREFERENCED_PARAMETER(dwLevel);
    UNREFERENCED_PARAMETER(pDriverInfo);
    UNREFERENCED_PARAMETER(lParam);

    return E_NOTIMPL;
};


#define PROFILE_NAME    L"OEMPROFILE.icm\0"

HRESULT __stdcall IOemUI::QueryColorProfile(
            HANDLE      hPrinter,
            POEMUIOBJ   poemuiobj,
            PDEVMODE    pPublicDM,
            PVOID       pOEMDM,
            ULONG       ulQueryMode,
            VOID       *pvProfileData,
            ULONG      *pcbProfileData,
            FLONG      *pflProfileData)
{
    HRESULT Result = E_FAIL;

    VERBOSE("IOemUI:QueryColorProfile entry.\r\n");

    UNREFERENCED_PARAMETER(hPrinter);
    UNREFERENCED_PARAMETER(poemuiobj);
    UNREFERENCED_PARAMETER(pPublicDM);
    UNREFERENCED_PARAMETER(pOEMDM);

    if(QCP_DEVICEPROFILE == ulQueryMode)
    {
        if(NULL == pvProfileData)
        {
            *pcbProfileData = sizeof(PROFILE_NAME);
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
        }
        else
        {
            if(*pcbProfileData < sizeof(PROFILE_NAME))
            {
                *pcbProfileData = sizeof(PROFILE_NAME);
                SetLastError(ERROR_INSUFFICIENT_BUFFER);
            }
            else
            {
                Result = StringCbCopy((LPWSTR)pvProfileData, *pcbProfileData, PROFILE_NAME);
                *pcbProfileData = sizeof(PROFILE_NAME);
                *pflProfileData = QCP_PROFILEDISK;

                if(FAILED(Result))
                {
                    SetLastError(Result);
                }
            }
        }
    }

    return Result;
};

HRESULT __stdcall IOemUI::FontInstallerDlgProc(
        HWND    hWnd,
        UINT    usMsg,
        WPARAM  wParam,
        LPARAM  lParam)
{
    VERBOSE("IOemUI:FontInstallerDlgProc entry.\r\n");

    UNREFERENCED_PARAMETER(hWnd);
    UNREFERENCED_PARAMETER(usMsg);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);

    return E_NOTIMPL;
};

HRESULT __stdcall IOemUI::UpdateExternalFonts(
        HANDLE       hPrinter,
        HANDLE       hHeap,
        __in PWSTR   pwstrCartridges)
{
    VERBOSE("IOemUI:UpdateExternalFonts entry.\r\n");

    UNREFERENCED_PARAMETER(hPrinter);
    UNREFERENCED_PARAMETER(hHeap);
    UNREFERENCED_PARAMETER(pwstrCartridges);

    return E_NOTIMPL;
}

////////////////////////////////////////////////////////////////////////////////
//
// oem class factory
//
class IOemCF : public IClassFactory
{
public:
    // *** IUnknown methods ***

    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR* ppvObj);

    STDMETHOD_(ULONG,AddRef)  (THIS);

    // the __drv_at tag here tells prefast that once release 
    // is called, the memory should not be considered leaked
    __drv_at(this, __drv_freesMem(object)) STDMETHOD_(ULONG,Release)(THIS);

    // *** IClassFactory methods ***
    STDMETHOD(CreateInstance) (THIS_
                               LPUNKNOWN pUnkOuter,
                               REFIID riid,
                               LPVOID FAR* ppvObject);
    STDMETHOD(LockServer)     (THIS_ BOOL bLock);


    // Constructor
    IOemCF(): m_cRef(1) { };
    ~IOemCF() { };

protected:
    LONG m_cRef;

};

///////////////////////////////////////////////////////////
//
// Class factory body
//
HRESULT __stdcall IOemCF::QueryInterface(const IID& iid, void** ppv)
{
    if ((iid == IID_IUnknown) || (iid == IID_IClassFactory))
    {
        *ppv = static_cast<IOemCF*>(this) ;
    }
    else
    {
        VERBOSE("IOemCF::QueryInterface IID not supported.\r\n");
        *ppv = NULL ;
        return E_NOINTERFACE ;
    }
    reinterpret_cast<IUnknown*>(*ppv)->AddRef() ;
    return S_OK ;
}

ULONG __stdcall IOemCF::AddRef()
{
    return InterlockedIncrement(&m_cRef) ;
}

__drv_at(this, __drv_freesMem(object))
ULONG __stdcall IOemCF::Release()
{
   ASSERT( 0 != m_cRef);
   ULONG cRef = InterlockedDecrement(&m_cRef);
   if (0 == cRef)
   {
      delete this;

   }
   return cRef;
}

// IClassFactory implementation
HRESULT __stdcall IOemCF::CreateInstance(IUnknown* pUnknownOuter,
                                           const IID& iid,
                                           void** ppv)
{
    if (ppv == NULL)
    {
        return E_POINTER;
    }
    *ppv = NULL;

    // Cannot aggregate.
    if (pUnknownOuter != NULL)
    {
        return CLASS_E_NOAGGREGATION ;
    }

    // Create component.
    IOemUI* pOemCB = new IOemUI ;
    if (pOemCB == NULL)
    {
        return E_OUTOFMEMORY ;
    }
    // Get the requested interface.
    HRESULT hr = pOemCB->QueryInterface(iid, ppv) ;

    // Release the IUnknown pointer.
    // (If QueryInterface failed, component will delete itself.)
    pOemCB->Release() ;
    return hr ;
}

// LockServer
HRESULT __stdcall IOemCF::LockServer(BOOL bLock)
{
    if (bLock)
    {
        InterlockedIncrement(&g_cServerLocks) ;
    }
    else
    {
        InterlockedDecrement(&g_cServerLocks) ;
    }
    return S_OK ;
}

///////////////////////////////////////////////////////////
//
// Exported functions
//


// Can DLL unload now?
//
STDAPI DllCanUnloadNow()
{
    //
    // To avoid leaving OEM DLL still in memory when Unidrv or Pscript drivers
    // are unloaded, Unidrv and Pscript driver ignore the return value of
    // DllCanUnloadNow of the OEM DLL, and always call FreeLibrary on the OEMDLL.
    //
    // If OEM DLL spins off a working thread that also uses the OEM DLL, the
    // thread needs to call LoadLibrary and FreeLibraryAndExitThread, otherwise
    // it may crash after Unidrv or Pscript calls FreeLibrary.
    //

    if ((g_cComponents == 0) && (g_cServerLocks == 0))
    {
        return S_OK ;
    }
    else
    {
        return S_FALSE;
    }
}

//
// Get class factory
//
STDAPI  DllGetClassObject(
    __in REFCLSID clsid, 
    __in REFIID iid, 
    __deref_out LPVOID* ppv)
{
    VERBOSE("DllGetClassObject:Create class factory.\r\n");

    if (ppv == NULL)
    {
        return E_POINTER;
    }
    *ppv = NULL;

    // Can we create this component?
    if (clsid != CLSID_OEMUI)
    {
        return CLASS_E_CLASSNOTAVAILABLE ;
    }

    // Create class factory.
    IOemCF* pFontCF = new IOemCF ;  // Reference count set to 1
                                         // in constructor
    if (pFontCF == NULL)
    {
        return E_OUTOFMEMORY ;
    }

    // Get requested interface.
    HRESULT hr = pFontCF->QueryInterface(iid, ppv) ;
    pFontCF->Release() ;

    return hr ;
}


