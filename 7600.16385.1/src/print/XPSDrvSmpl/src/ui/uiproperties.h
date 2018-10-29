/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   uiproperties.h

Abstract:

   This class encapsulates all handling of GPD and OEM private devmode settings
   used in the XPSDrv feature sample UI.

--*/

#pragma once

#include "devmode.h"

extern PCSTR g_pszPSScaleWidth;
extern PCSTR g_pszPSScaleHeight;
extern PCSTR g_pszOffsetWidth;
extern PCSTR g_pszOffsetHeight;
extern PCSTR g_pszWMTransparency;
extern PCSTR g_pszWMAngle;
extern PCSTR g_pszWMOffsetWidth;
extern PCSTR g_pszWMOffsetHeight;
extern PCSTR g_pszWMSizeWidth;
extern PCSTR g_pszWMSizeHeight;
extern PCSTR g_pszWMFontSize;
extern PCSTR g_pszWMFontColor;
extern PCSTR g_pszWMText;

//
// Forward declaration
//
class CUIProperties;

typedef VOID UIProperty;

typedef pair<SIZE_T, SIZE_T> UIPropertyPair;
typedef map<CStringXDA, UIPropertyPair> UIPropertyMap;

typedef vector<CStringXDA> UIOptItemList;

class CUIProperties
{
public:
    CUIProperties();

    CUIProperties(POEMDEV pOEMDev);

    virtual ~CUIProperties();

    HRESULT
    GetItem(
        __in                 PCSTR       pFeatureName,
        __out_bcount(cbSize) UIProperty* pUIProperty,
        __in                 SIZE_T      cbSize
        );

    HRESULT
    SetItem(
        __in                      PCSTR        pFeatureName,
        __in_bcount(cbSize) CONST UIProperty*  pUIProperty,
        __in                      SIZE_T       cbSize
        );

    HRESULT
    SetDefaults();

    HRESULT
    Validate();

    HRESULT
    Convert(
        __in CUIProperties* pUIProperties
        );

    HRESULT
    GetOptItem(
        __in            POEMCUIPPARAM pOemCUIPParam,
        __in            PCSTR         pFeatureName,
        __deref_out_opt POPTITEM*     ppOptItem
        );

    HRESULT
    HideOptItems(
        __in POEMCUIPPARAM pOemCUIPParam
        );

private:
    HRESULT SetHeader();

    HRESULT InitialiseMap();

private:
    POEMDEV         m_pOEMDev;

    UIPropertyMap   m_PropertyMap;

    UIOptItemList   m_OptItemList;
};


