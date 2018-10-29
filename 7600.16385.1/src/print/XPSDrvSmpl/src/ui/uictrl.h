/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   uictrl.h

Abstract:

   Definition of the UI control interface called by the containing property page,
   the abstract UI control class and the base default UI controls for check boxe,
   list, combo, edit and spin controls.

--*/

#pragma once

#include "UIProperties.h"

class CUIControl
{
public:
    CUIControl();

    virtual ~CUIControl();

    virtual HRESULT
    SetPrintOemDriverUI(
        __in CONST IPrintOemDriverUI* pOEMDriverUI
        );

    virtual HRESULT
    SetOemCUIPParam(
        __in CONST POEMCUIPPARAM pOemCUIPParam
        );

    virtual HRESULT
    SetUIProperties(
        __in CUIProperties* pUIProperties
        );

    //
    // Message handling stubs
    //
    virtual HRESULT
    OnActivate(
        __in CONST HWND hDlg
        ) = 0;

    virtual HRESULT
    OnInit(
        __in CONST HWND hDlg
        );

    virtual HRESULT
    OnCommand(
        __in CONST HWND hDlg,
        __in INT        iCommand
        );

    virtual HRESULT
    OnNotify(
        __in CONST HWND hDlg,
        __in CONST NMHDR* pNmhdr
        );

protected:
    CComPtr<IPrintOemDriverUI> m_pDriverUIHelp;

    POEMCUIPPARAM              m_pOemCUIPParam;

    CUIProperties *            m_pUIProperties;
};

class CUICtrlDefaultCheck : public CUIControl
{
public:
    CUICtrlDefaultCheck(
        __in PCSTR gpdString,
        __in INT   iCheckResID
        );

    virtual ~CUICtrlDefaultCheck();

    HRESULT
    OnActivate(
        __in CONST HWND hDlg
        );

    HRESULT
    OnCommand(
        __in CONST HWND hDlg,
        __in INT        iCommand
        );

private:
    virtual HRESULT
    OnBnClicked(
        __in CONST HWND hDlg
        );

    virtual HRESULT
    EnableDependentCtrls(
        __in CONST HWND hDlg,
        __in CONST LONG lSel
        );

    CUICtrlDefaultCheck& operator = (CONST CUICtrlDefaultCheck&);

private:
    PCSTR     m_szGPDString;

    CONST INT m_iCheckResID;
};

class CUICtrlDefaultList : public CUIControl
{
public:
    CUICtrlDefaultList(
        __in PCSTR gpdString,
        __in INT   iListResID
        );

    virtual ~CUICtrlDefaultList();

    HRESULT
    OnActivate(
        __in CONST HWND hDlg
        );

    HRESULT
    OnCommand(
        __in CONST HWND hDlg,
        __in INT        iCommand
        );

protected:
    HRESULT
    AddString(
        __in CONST HWND      hDlg,
        __in CONST HINSTANCE hStringResDLL,
        __in CONST INT       idString
        );

private:
    virtual HRESULT
    EnableDependentCtrls(
        __in CONST HWND hDlg,
        __in CONST LONG lSel
        );

    virtual HRESULT
    OnSelChange(
        __in CONST HWND hDlg
        );

    CUICtrlDefaultList& operator = (CONST CUICtrlDefaultList&);

private:
    PCSTR     m_szGPDString;

    CONST INT m_iListResID;
};

class CUICtrlDefaultCombo : public CUIControl
{
public:
    CUICtrlDefaultCombo(
        __in PCSTR gpdString,
        __in INT   iComboResID
        );

    virtual ~CUICtrlDefaultCombo();

    HRESULT
    OnActivate(
        __in CONST HWND hDlg
        );

    HRESULT
    OnCommand(
        __in CONST HWND hDlg,
        __in INT        iCommand
        );

    virtual HRESULT
    OnSelChange(
        __in CONST HWND hDlg
        );

protected:
    HRESULT
    AddString(
        __in CONST HWND      hDlg,
        __in CONST HINSTANCE hStringResDLL,
        __in CONST INT       idString
        );

private:
    virtual HRESULT
    EnableDependentCtrls(
        __in CONST HWND hDlg,
        __in CONST LONG lSel
        );

    CUICtrlDefaultCombo& operator = (CONST CUICtrlDefaultCombo&);

private:
    PCSTR     m_szGPDString;

    CONST INT m_iComboResID;
};

class CUICtrlDefaultSpin;

class CUICtrlDefaultEditNum : public CUIControl
{
public:
    CUICtrlDefaultEditNum(
        __in PCSTR     propString,
        __in CONST INT iEditResID,
        __in CONST INT iPropMin,
        __in CONST INT iPropMax,
        __in CONST INT iSpinResID
        );

    virtual ~CUICtrlDefaultEditNum();

    friend class CUICtrlDefaultSpin;

    HRESULT
    virtual OnActivate(
        __in CONST HWND hDlg
        );

    virtual HRESULT
    OnCommand(
        __in CONST HWND hDlg,
        __in INT iCommand
        );

private:
    CUICtrlDefaultEditNum() :
        m_iEditResID(0),
        m_iPropMin(0),
        m_iPropMax(0),
        m_iSpinResID(0)
    {}

    virtual HRESULT
    CheckInRange(
        __in PINT pValue
        );

    virtual HRESULT
    OnEnChange(
        __in CONST HWND hDlg
        );

    CUICtrlDefaultEditNum& operator = (CONST CUICtrlDefaultEditNum&);


private:
    PCSTR     m_szPropString;

    CONST INT m_iPropMin;

    CONST INT m_iPropMax;

    CONST INT m_iEditResID;

    CONST INT m_iSpinResID;
};

class CUICtrlDefaultSpin : public CUIControl
{

public:
    CUICtrlDefaultSpin(
        __in CUICtrlDefaultEditNum* pEdit
        );

    virtual ~CUICtrlDefaultSpin();

    virtual HRESULT
    OnActivate(
        __in CONST HWND hDlg
        );

    virtual HRESULT
    OnNotify(
        __in CONST HWND hDlg,
        __in CONST NMHDR* pNmhdr
        );


private:
    virtual HRESULT
    CheckInRange(
        __in PINT pValue
        );

    virtual HRESULT
    OnDeltaPos(
        __in CONST HWND hDlg,
        __in CONST NMUPDOWN* pNmud
        );

    CUICtrlDefaultSpin& operator = (CONST CUICtrlDefaultSpin&);

private:
    PCSTR     m_szPropString;

    INT m_iPropMin;

    INT m_iPropMax;

    INT m_iSpinResID;

    INT m_iEditResID;
};

class CUICtrlDefaultEditText : public CUIControl
{
public:
    CUICtrlDefaultEditText(
        __in PCSTR     propString,
        __in CONST INT iEditResID,
        __in CONST INT cbMaxLength
        );

    virtual ~CUICtrlDefaultEditText();

    HRESULT
    virtual OnActivate(
        __in CONST HWND hDlg
        );

    virtual HRESULT
    OnCommand(
        __in CONST HWND hDlg,
        __in INT        iCommand
        );

private:
    virtual HRESULT
    OnEnChange(
        __in CONST HWND hDlg
        );

    CUICtrlDefaultEditText& operator = (CONST CUICtrlDefaultEditText&);


private:
    PCSTR     m_szPropString;

    CONST INT m_cbMaxLength;

    CONST INT m_iEditResID;
};

class CUICtrlDefaultBtn : public CUIControl
{
public:
    CUICtrlDefaultBtn(
        __in PCSTR propertyString,
        __in INT   iCheckResID
        );

    virtual ~CUICtrlDefaultBtn();

    HRESULT
    OnActivate(
        __in CONST HWND hDlg
        );

    HRESULT
    OnCommand(
        __in CONST HWND hDlg,
        __in INT        iCommand
        );

private:
    virtual HRESULT
    OnBnClicked(
        __in CONST HWND hDlg
        ) = 0;

    CUICtrlDefaultBtn& operator = (CONST CUICtrlDefaultCheck&);

protected:
    PCSTR     m_szPropertyString;

private:
    CONST INT m_iBtnResID;
};


