//**@@@*@@@****************************************************
//
// Microsoft Windows
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//**@@@*@@@****************************************************

//
// FileName:    SwapPropPage.h
//
// Abstract:    Declaration of the CSwapPropPage class
//
// ----------------------------------------------------------------------------


#pragma once

// ----------------------------------------------------------------------
// CSwapPropPage class
//
class ATL_NO_VTABLE CSwapPropPage :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CSwapPropPage, &CLSID_SwapPropPage>,
    public IDispatchImpl<ISwapPropPage, &__uuidof(ISwapPropPage), &LIBID_CplExtLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
    public IShellExtInit,
    public IShellPropSheetExt
{
public:
    CSwapPropPage();
    ~CSwapPropPage();

    DECLARE_REGISTRY_RESOURCEID(IDR_SWAP_PROP_PAGE)

    BEGIN_COM_MAP(CSwapPropPage)
        COM_INTERFACE_ENTRY(ISwapPropPage)
        COM_INTERFACE_ENTRY(IDispatch)
        COM_INTERFACE_ENTRY(IShellExtInit)
        COM_INTERFACE_ENTRY(IShellPropSheetExt)
    END_COM_MAP()

    DECLARE_PROTECT_FINAL_CONSTRUCT()

    HRESULT FinalConstruct()
    {
        return S_OK;
    }

    void FinalRelease()
    {
    }

    static INT_PTR CALLBACK DialogProcPage1(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static UINT CALLBACK PropSheetPageProc(HWND hwnd, UINT uMsg, LPPROPSHEETPAGE ppsp);

    // IShellExtInit
    STDMETHOD(Initialize)(LPCITEMIDLIST pidlFolder, IDataObject* pdtobj, HKEY hkeyProgID);

    // IShellPropSheetExt
    STDMETHOD(AddPages)(LPFNADDPROPSHEETPAGE lpfnAddPage, LPARAM lParam);
    STDMETHOD(ReplacePage)(UINT uPageID, LPFNSVADDPROPSHEETPAGE lpfnReplaceWith, LPARAM lParam);

private:
    AudioFXExtensionParams* m_pAudioFXExtParams;
    BOOL m_fDisableSysFX;
    BOOL m_fEnableSwapLFX;
    BOOL m_fEnableSwapGFX;

    HRESULT GetDeviceFriendlyName(__deref_out_opt LPWSTR* ppNameOut);
    HRESULT RetrieveSysFXState();
    HRESULT SetSysFXState();
    HRESULT RetrieveSwapLFXState();
    HRESULT SetSwapLFXState();
    HRESULT RetrieveSwapGFXState();
    HRESULT SetSwapGFXState();

    BOOL OnInitDialog(HWND hwndDlg, WPARAM wParam, LPARAM lParam);
    BOOL OnApply(HWND hwndDlg);
    BOOL OnCheckBoxClickedDisableSysFX(HWND hwndDlg);
    BOOL OnCheckBoxClickedEnableSwapLFX(HWND hwndDlg);
    BOOL OnCheckBoxClickedEnableSwapGFX(HWND hwndDlg);
};

OBJECT_ENTRY_AUTO(__uuidof(SwapPropPage), CSwapPropPage)

