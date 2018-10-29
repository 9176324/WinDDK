/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   docppg.h

Abstract:

   Definition of the document property page class. This is an abstract
   class that provides common functionality for all property pages including
   accessors and the dialog proc.

--*/

#pragma once

#include "uictrl.h"

#define ID_XDSMPL_DLG_ATOM 1000

typedef std::map<INT_PTR, CUIControl*> UIControlMap;

class CDocPropPage
{
public:
    CDocPropPage();

    virtual ~CDocPropPage();

    virtual HRESULT
    InitDlgBox(
        __out LPCTSTR* ppszTemplate,
        __out LPCTSTR* ppszTitle
        ) = 0;

    //
    // Implementation
    //
    virtual HRESULT
    PropPageInit(
        __in CONST PPROPSHEETUI_INFO pPSUIInfo
        );

    virtual HRESULT
    SetPrintOemDriverUI(
        __in CONST IPrintOemDriverUI* pOEMDriverUI
        );

    virtual HRESULT
    SetOemCUIPParam(
        __in CONST POEMCUIPPARAM pOemCUIParam
        );

    virtual HRESULT
    SetUIProperties(
        __in CUIProperties* pUIProperties
    );

    static INT_PTR CALLBACK
    DlgProc(
        __in HWND   hDlg,
        __in UINT   uiMessage,
        __in WPARAM wParam,
        __in LPARAM lParam
        );

protected:
    HRESULT
    StoreThis(
        __in CONST HWND hDlg
        );

    static HRESULT
    RetrieveThis(
        __in        CONST HWND     hDlg,
        __deref_out CDocPropPage** pDocPropPage
        );

    HRESULT
    RemoveThis(
        __in CONST HWND hDlg
        );

    HRESULT
    SetComPropSheetFunc(
        __in CONST PFNCOMPROPSHEET pfnComPropSheet
        );

    HRESULT
    SetPageHandle(
        __in CONST HANDLE hPage
        );

    HRESULT
    SetComPropSheetHandle(
        __in CONST HANDLE hComPropSheet
        );

    HRESULT
    GetComPropSheetFunc(
        __deref_out PFNCOMPROPSHEET* ppfnComPropSheet
        );

    HANDLE
    GetPageHandle(
        VOID
        );

    HANDLE
    GetComPropSheetHandle(
        VOID
        );

    HRESULT
    AddUIControl(
        __in CONST INT iCtrlID,
        __in CUIControl* pUIControl
        );

    HRESULT
    GetUIComponents(
        __deref_out UIControlMap** ppUIComponents
        );

    HRESULT
    GetOemCUIPParam(
        __deref_out POEMCUIPPARAM* ppOemCUIPParam
        );

    HRESULT
    PublishHelpToControls(
        VOID
        );

    HRESULT
    SendCommand(
        __in CONST HWND   hDlg,
        __in CONST WPARAM wParam
        );

    HRESULT
    SendSetActive(
        __in CONST HWND   hDlg
        );

    HRESULT
    SendInit(
        __in CONST HWND   hDlg
        );

    HRESULT
    SendNotify(
        __in CONST HWND   hDlg,
        __in CONST NMHDR* pNMhdr
        );

    HRESULT
    DestroyUIComponents(
        VOID
        );

private:
    UIControlMap                 m_UIControls;

    CComPtr<IPrintOemDriverUI>   m_pDriverUIHelp;

    HANDLE                       m_hPage;

    HANDLE                       m_hComPropSheet;

    PFNCOMPROPSHEET              m_pfnComPropSheet;

    POEMCUIPPARAM                m_pOemCUIPParam;

    CUIProperties*               m_pUIProperties;
};


