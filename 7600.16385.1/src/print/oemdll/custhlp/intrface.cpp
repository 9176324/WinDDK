//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//  ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//  PARTICULAR PURPOSE.
//
//  Copyright  1998 - 2003  Microsoft Corporation.  All Rights Reserved.
//
//  FILE:    intrface.cpp
//    
//  PURPOSE:  Implementation of OEMGetInfo and OEMDevMode. Shared by all Unidrv 
//            OEM test dll's.
//

#include "precomp.h"

#include "oemui.h"
#include "debug.h"
#include "intrface.h"

// This indicates to Prefast that this is a usermode driver file.
__user_driver;

////////////////////////////////////////////////////////
//      Internal Globals
////////////////////////////////////////////////////////

static long g_cComponents = 0;     // Count of active components
static long g_cServerLocks = 0;    // Count of locks


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
    ASSERT(0 == m_cRef);
}

HRESULT __stdcall IOemUI::QueryInterface(const IID& iid, void** ppv)
{
    if (iid == IID_IUnknown)
    {
        *ppv = static_cast<IUnknown*>(this);
    }
    else if (iid == IID_IPrintOemUI)
    {
        *ppv = static_cast<IPrintOemUI*>(this);
    }
    else
    {
        *ppv = NULL;
        return E_NOINTERFACE;
    }
    reinterpret_cast<IUnknown*>(*ppv)->AddRef();
    return S_OK;
}

ULONG __stdcall IOemUI::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

__drv_at(this, __drv_freesMem(object)) 
ULONG __stdcall IOemUI::Release()
{
    ASSERT( 0 != m_cRef);
    ULONG cRef = InterlockedDecrement(&m_cRef);
    if (0 == cRef)
    {
        delete this;

    }
    return cRef;
}

//
// (Implementation is required) Supplies a pointer to UNIDRIVE /PScript IPrintOemDriverUI
//
HRESULT __stdcall IOemUI::PublishDriverInterface(
    IUnknown *pIUnknown)
{
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

//
//(Implementation is required) Returns the UI Plugin's identification information
//
HRESULT __stdcall IOemUI::GetInfo(
    DWORD  dwMode,
    PVOID  pBuffer,
    DWORD  cbSize,
    PDWORD pcbNeeded)
{
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

    return S_OK;
}


//
//Performs operation on UI Plugins Private DevMode Members.
//
HRESULT __stdcall IOemUI::DevMode(
    DWORD  dwMode,
    POEMDMPARAM pOemDMParam)
{
    return hrOEMDevMode(dwMode, pOemDMParam);
}


//
//Allows UI Plugin to Modifiy Existing printer porp sheet and document property sheet page..
//
HRESULT __stdcall IOemUI::CommonUIProp(
    DWORD  dwMode,
    POEMCUIPPARAM   pOemCUIPParam)
{
    return hrOEMPropertyPage(dwMode, pOemCUIPParam);
}


//
//Adds a New page to the device Documment property sheet.
//

HRESULT __stdcall IOemUI::DocumentPropertySheets(
    PPROPSHEETUI_INFO   pPSUIInfo,
    LPARAM              lParam)
{
    return hrOEMDocumentPropertySheets(pPSUIInfo, lParam, m_pOEMHelp);
}

//
//Adds a New page to the device printer property page.
//
HRESULT __stdcall IOemUI::DevicePropertySheets(
    PPROPSHEETUI_INFO   pPSUIInfo,
    LPARAM              lParam)
{
    return hrOEMDevicePropertySheets(pPSUIInfo, lParam);
}


//
//Allows the UI plugin to spec Customized device capabilaties (TODO)
//
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


//
//Allows the UI plugin to help determine if a print Job is printable.
//
HRESULT __stdcall IOemUI::DevQueryPrintEx(
    POEMUIOBJ               poemuiobj,
    PDEVQUERYPRINT_INFO     pDQPInfo,
    PDEVMODE                pPublicDM,
    PVOID                   pOEMDM)
{
    UNREFERENCED_PARAMETER(poemuiobj);
    UNREFERENCED_PARAMETER(pDQPInfo);
    UNREFERENCED_PARAMETER(pPublicDM);
    UNREFERENCED_PARAMETER(pOEMDM);

    return E_NOTIMPL;
}


//
//Allows the UI Plugin to upgrade its dev options stored in the registry.
//
HRESULT __stdcall IOemUI::UpgradePrinter(
    DWORD   dwLevel,
    PBYTE   pDriverUpgradeInfo)
{
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
    UNREFERENCED_PARAMETER(dwDriverEvent);
    UNREFERENCED_PARAMETER(dwLevel);
    UNREFERENCED_PARAMETER(pDriverInfo);
    UNREFERENCED_PARAMETER(lParam);

    return E_NOTIMPL;
};


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
    UNREFERENCED_PARAMETER(hPrinter);
    UNREFERENCED_PARAMETER(poemuiobj);
    UNREFERENCED_PARAMETER(pPublicDM);
    UNREFERENCED_PARAMETER(pOEMDM);

    HRESULT Result = E_FAIL;
    static const WCHAR PROFILE_NAME[] = L"OEMPROFILE.icm\0";

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
    __drv_at(this, __drv_freesMem(object)) 
    STDMETHOD_(ULONG,Release) (THIS);

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
        *ppv = static_cast<IOemCF*>(this);
    }
    else
    {
        *ppv = NULL;
        return E_NOINTERFACE;
    }
    reinterpret_cast<IUnknown*>(*ppv)->AddRef();
    return S_OK;
}

ULONG __stdcall IOemCF::AddRef()
{
    return InterlockedIncrement(&m_cRef);
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
        return CLASS_E_NOAGGREGATION;
    }

    // Create component.
    IOemUI* pOemCB = new IOemUI;
    if (pOemCB == NULL)
    {
        return E_OUTOFMEMORY;
    }
    // Get the requested interface.
    HRESULT hr = pOemCB->QueryInterface(iid, ppv);

    // Release the IUnknown pointer.
    // (If QueryInterface failed, component will delete itself.)
    pOemCB->Release();
    return hr;
}

HRESULT __stdcall IOemCF::LockServer(BOOL bLock)
{
    if (bLock)
    {
        InterlockedIncrement(&g_cServerLocks);
    }
    else
    {
        InterlockedDecrement(&g_cServerLocks);
    }
    return S_OK;
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
        return S_OK;
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
    if (ppv == NULL)
    {
        return E_POINTER;
    }
    *ppv = NULL;

    // Can we create this component?
    if (clsid != CLSID_OEMUI)
    {
        return CLASS_E_CLASSNOTAVAILABLE;
    }

    // Create class factory.
    IOemCF* pFontCF = new IOemCF;  // Reference count set to 1
                                         // in constructor
    if (pFontCF == NULL)
    {
        return E_OUTOFMEMORY;
    }

    // Get requested interface.
    HRESULT hr = pFontCF->QueryInterface(iid, ppv);
    pFontCF->Release();

    return hr;
}


