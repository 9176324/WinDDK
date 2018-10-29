/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   xdsmplui.h

Abstract:

   Definition of the UI plugin. This is responsible for initialising and maintining
   the property pages used in the XPSDrv feature sample UI.

--*/

#pragma once

#include "cunknown.h"
#include "docppg.h"

typedef std::vector<CDocPropPage*> DocPropertyPageMap;

class CXDSmplUI : public CUnknown<IPrintOemUI>
{
public:
    //
    // Construction and Destruction
    //
    CXDSmplUI();

    virtual ~CXDSmplUI();

    //
    // IPrintOemUI methods
    //
    STDMETHOD(PublishDriverInterface)(THIS_
        __in IUnknown* pIUnknown
        );

    STDMETHOD(GetInfo)(THIS_
        __in  DWORD  dwMode,
        __out_bcount(cbSize) PVOID  pBuffer,
        __in  DWORD  cbSize,
        __out PDWORD pcbNeeded
        );

    STDMETHOD(DevMode)(THIS_
        __in DWORD       dwMode,
        __in POEMDMPARAM pOemDMParam
        );

    STDMETHOD(CommonUIProp)(THIS_
        __in DWORD         dwMode,
        __in POEMCUIPPARAM pOemCUIPParam
        );

    STDMETHOD(DocumentPropertySheets)(THIS_
        __in PPROPSHEETUI_INFO pPSUIInfo,
        __in LPARAM            lParam
        );

    STDMETHOD(DevicePropertySheets)(THIS_
         __in PPROPSHEETUI_INFO pPSUIInfo,
         __in LPARAM            lParam
         );

    STDMETHOD(DevQueryPrintEx)(THIS_
        __in POEMUIOBJ           poemuiobj,
        __in PDEVQUERYPRINT_INFO pDQPInfo,
        __in PDEVMODE            pPublicDM,
        __in PVOID               pOEMDM
        );

    STDMETHOD(DeviceCapabilities)(THIS_
        __in   POEMUIOBJ poemuiobj,
        __in   HANDLE    hPrinter,
        __in_z PWSTR     pDeviceName,
        __in   WORD      wCapability,
        __in   PVOID     pOutput,
        __in   PDEVMODE  pPublicDM,
        __in   PVOID     pOEMDM,
        __in   DWORD     dwOld,
        __in   DWORD*    dwResult
        );

    STDMETHOD(UpgradePrinter)(THIS_
        __in DWORD dwLevel,
        __in PBYTE pDriverUpgradeInfo
        );

    STDMETHOD(PrinterEvent)(THIS_
        __in_z PWSTR  pPrinterName,
               INT    iDriverEvent,
               DWORD  dwFlags,
        __in   LPARAM lParam
        );

    STDMETHOD(DriverEvent)(THIS_
             DWORD  dwDriverEvent,
             DWORD  dwLevel,
        __in PBYTE  pDriverInfo,
        __in LPARAM lParam
        );

    STDMETHOD(QueryColorProfile)(THIS_
        __in HANDLE    hPrinter,
        __in POEMUIOBJ poemuiobj,
        __in PDEVMODE  pPublicDM,
        __in PVOID     pOEMDM,
             ULONG     ulReserved,
        __in VOID*     pvProfileData,
        __in ULONG*    pcbProfileData,
        __in FLONG*    pflProfileData
        );

    STDMETHOD(FontInstallerDlgProc)(THIS_
        __in HWND    hWnd,
             UINT    usMsg,
        __in WPARAM  wParam,
        __in LPARAM  lParam
        );

    STDMETHOD(UpdateExternalFonts)(THIS_
        __in   HANDLE hPrinter,
        __in   HANDLE hHeap,
        __in_z PWSTR  pwstrCartridges
        );

private:
    HRESULT
    CreatePropertyPages(
        VOID
        );

    inline VOID
    DestroyPropPages(
        VOID
        );

private:
    POEMCUIPPARAM                m_pOemCUIPParam;

    CUIProperties*               m_pUIProperties;

    CComPtr<IPrintOemDriverUI>   m_pDriverUIHelp;

    DocPropertyPageMap           m_vectPropPages;
};


