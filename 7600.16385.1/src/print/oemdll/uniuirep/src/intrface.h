//+--------------------------------------------------------------------------
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//  ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//  PARTICULAR PURPOSE.
//
//  Copyright  1997 - 2005  Microsoft Corporation.  All Rights Reserved.
//
//  FILE:   Intrface.h
//
//  PURPOSE:    Define COM interface for User Mode Printer Customization DLL.
//
//--------------------------------------------------------------------------

#pragma once

///////////////////////////////////////
//          Globals
///////////////////////////////////////

extern HINSTANCE   ghInstance;
    // Module's Instance handle from DLLEntry of process.

class COemUI2: public IPrintOemUI2
{
public:
    //
    // *** IUnknown methods ***
    // 

    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR* ppvObj);

    STDMETHOD_(ULONG,AddRef)  (THIS);

    // the __drv_at tag here tells prefast that once release 
    // is called, the memory should not be considered leaked
    __drv_at(this, __drv_freesMem(object)) 
    STDMETHOD_(ULONG,Release) (THIS);

    //
    // Method for publishing Driver interface.
    //
    STDMETHOD(PublishDriverInterface)(THIS_ IUnknown *pIUnknown);

    //
    // Get OEM dll related information
    //
    STDMETHOD(GetInfo) (THIS_ 
        DWORD  dwMode, 
        __out_bcount(cbSize) PVOID  pBuffer, 
        DWORD  cbSize,
        PDWORD pcbNeeded);

    //
    // OEMDevMode
    //
    STDMETHOD(DevMode) (THIS_  DWORD  dwMode, POEMDMPARAM pOemDMParam);

    //
    // OEMCommonUIProp
    //
    STDMETHOD(CommonUIProp) (THIS_  
            DWORD  dwMode, 
            POEMCUIPPARAM   pOemCUIPParam
            );

    //
    // OEMDocumentPropertySheets
    //
    STDMETHOD(DocumentPropertySheets) (THIS_
            PPROPSHEETUI_INFO   pPSUIInfo,
            LPARAM              lParam
            );

    //
    // OEMDevicePropertySheets
    //
    STDMETHOD(DevicePropertySheets) (THIS_
            PPROPSHEETUI_INFO   pPSUIInfo,
            LPARAM              lParam
            );


    //
    // OEMDevQueryPrintEx
    //
    STDMETHOD(DevQueryPrintEx) (THIS_
            POEMUIOBJ               poemuiobj,
            PDEVQUERYPRINT_INFO     pDQPInfo,
            PDEVMODE                pPublicDM,
            PVOID                   pOEMDM
            );

    //
    // OEMDeviceCapabilities
    //
    STDMETHOD(DeviceCapabilities) (THIS_
            __inout POEMUIOBJ   poemuiobj,
            __in  HANDLE      hPrinter,
            __in  PWSTR       pDeviceName,
                  WORD        wCapability,
            __out PVOID       pOutput,
            __in  PDEVMODE    pPublicDM,
            __in  PVOID       pOEMDM,
                  DWORD       dwOld,
            __out DWORD       *dwResult
            );

    //
    // OEMUpgradePrinter
    //
    STDMETHOD(UpgradePrinter) (THIS_
            DWORD   dwLevel,
            PBYTE   pDriverUpgradeInfo
            );

    //
    // OEMPrinterEvent
    //
    STDMETHOD(PrinterEvent) (THIS_
            __in PWSTR   pPrinterName,
            INT     iDriverEvent,
            DWORD   dwFlags,
            LPARAM  lParam
            );

    //
    // OEMDriverEvent
    //
    STDMETHOD(DriverEvent)(THIS_
            DWORD   dwDriverEvent,
            DWORD   dwLevel,
            LPBYTE  pDriverInfo,
            LPARAM  lParam
            );
 
    //
    // OEMQueryColorProfile
    //
    STDMETHOD( QueryColorProfile) (THIS_
            HANDLE      hPrinter,
            POEMUIOBJ   poemuiobj,
            PDEVMODE    pPublicDM,
            PVOID       pOEMDM,
            ULONG       ulReserved,
            VOID       *pvProfileData,
            ULONG      *pcbProfileData,
            FLONG      *pflProfileData);

    //
    // OEMFontInstallerDlgProc
    //
    STDMETHOD(FontInstallerDlgProc) (THIS_ 
            HWND    hWnd,
            UINT    usMsg,
            WPARAM  wParam,
            LPARAM  lParam
            );
    //
    // UpdateExternalFonts
    //
    STDMETHOD(UpdateExternalFonts) (THIS_
            __in HANDLE  hPrinter,
            __in HANDLE  hHeap,
            __in PWSTR   pwstrCartridges
            );

    //
    // IPrintOemUI2 methods
    //

    //
    // QueryJobAttributes
    //
    STDMETHOD(QueryJobAttributes)  (THIS_
            HANDLE      hPrinter,
            PDEVMODE    pDevmode,
            DWORD       dwLevel,
            LPBYTE      lpAttributeInfo
           );

    //
    // Hide Standard UI
    //
    STDMETHOD(HideStandardUI)  (THIS_
            DWORD       dwMode
           );

    //
    // DocumentEvent
    //
    STDMETHOD(DocumentEvent) (THIS_
            HANDLE      hPrinter,
            HDC         hdc,
            INT         iEsc,
            ULONG       cbIn,
            PVOID       pbIn,
            ULONG       cbOut,
            PVOID       pbOut,
            PINT        piResult
           );

    COemUI2()
    {
        m_cRef = 1;
        m_pCoreHelper = NULL;
    };
    
    virtual ~COemUI2();

protected:

    LONG m_cRef;
        // reference count of this object

    IPrintCoreHelper* m_pCoreHelper;
        // Pointer to the helper interface introduced in Windows Vista.

    CFeatureCollection m_Features;
        // Collection of features advertised as supported by the helper 
        // interface.
};




