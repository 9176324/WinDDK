//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//  ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//  PARTICULAR PURPOSE.
//
//  Copyright  1997 - 2003  Microsoft Corporation.  All Rights Reserved.
//
//  FILE:    WMarkUI.cpp
//    
//
//  PURPOSE:  Main file for OEM UI test module.
//
//
//    Functions:
//
//        
//
//
//  PLATFORMS:    Windows 2000, Windows XP, Windows Server 2003
//
//

#include "precomp.h"
#include "resource.h"
#include "debug.h"
#include "wmarkui.h"

// StrSafe.h needs to be included last
// to disallow bad string functions.
#include <STRSAFE.H>



////////////////////////////////////////////////////////
//      INTERNAL MACROS and DEFINES
////////////////////////////////////////////////////////



////////////////////////////////////////////////////////
//      INTERNAL PROTOTYPES
////////////////////////////////////////////////////////

LONG APIENTRY OEMUICallBack(PCPSUICBPARAM pCallbackParam, POEMCUIPPARAM pOEMUIParam);
static HRESULT hrDocumentPropertyPage(DWORD dwMode, POEMCUIPPARAM pOEMUIParam);
static void InitOptItems(POPTITEM pOptItems, DWORD dwOptItems);
static POPTTYPE CreateOptType(HANDLE hHeap, WORD wOptParams);
static DWORD FontSizeToIndex(DWORD dwWmarkFontSize);
static DWORD FontIndexToSize(DWORD dwIndex);
static DWORD TextColorToIndex(COLORREF crWmarkTextColor);
static COLORREF IndexToTextColor(DWORD dwIndex);
static PTSTR GetStringResource(HANDLE hHeap, HMODULE hModule, UINT uResource);



////////////////////////////////////////////////////////////////////////////////
//
// Initializes OptItems to display OEM device or document property UI.
//
HRESULT hrOEMPropertyPage(DWORD dwMode, POEMCUIPPARAM pOEMUIParam)
{
    HRESULT hResult = S_OK;


    VERBOSE(DLLTEXT("hrOEMPropertyPage(%d) entry.\r\n"), dwMode);

    // Validate parameters.
    if( (OEMCUIP_DOCPROP != dwMode)
        &&
        (OEMCUIP_PRNPROP != dwMode)        
      )
    {
        ERR(ERRORTEXT("hrOEMPropertyPage() ERROR_INVALID_PARAMETER.\r\n"));
        VERBOSE(DLLTEXT("\tdwMode = %d, pOEMUIParam = %#lx.\r\n"), dwMode, pOEMUIParam);

        // Return invalid parameter error.
        SetLastError(ERROR_INVALID_PARAMETER);
        return E_FAIL;
    }

    switch(dwMode)
    {
        case OEMCUIP_DOCPROP:
            hResult = hrDocumentPropertyPage(dwMode, pOEMUIParam);
            break;

        case OEMCUIP_PRNPROP:
            // Don't have any Printer Proptery UI.
            hResult = E_NOTIMPL;
            break;

        default:
            // Should never reach this!
            ERR(ERRORTEXT("hrOEMPropertyPage() Invalid dwMode, %d"), dwMode);
            SetLastError(ERROR_INVALID_PARAMETER);
            hResult = E_FAIL;
            break;
    }

    return hResult;
}

////////////////////////////////////////////////////////////////////////////////
//
// OptItems call back for OEM device or document property UI.
//
LONG APIENTRY OEMUICallBack(PCPSUICBPARAM pCallbackParam, POEMCUIPPARAM pOEMUIParam)
{
    LONG    lReturn = CPSUICB_ACTION_NONE;
    POEMDEV pOEMDev = (POEMDEV) pOEMUIParam->pOEMDM;


    VERBOSE(DLLTEXT("OEMUICallBack() entry, Reason is %d.\r\n"), pCallbackParam->Reason);

    switch(pCallbackParam->Reason)
    {
        case CPSUICB_REASON_APPLYNOW:
            // Store OptItems state in DEVMODE.
            pOEMDev->bWmarkEnabled = !pOEMUIParam->pOEMOptItems[0].Sel;
            if(FAILED(StringCbCopyW(pOEMDev->szWaterMark, sizeof(pOEMDev->szWaterMark), (LPWSTR)pOEMUIParam->pOEMOptItems[1].pSel)))
            {
                ERR(ERRORTEXT("OEMUICallBack() failed to copy water mark text\r\n"));
            }
            pOEMDev->dwWmarkFontSize = FontIndexToSize(pOEMUIParam->pOEMOptItems[2].Sel);
            pOEMDev->crWmarkTextColor = IndexToTextColor(pOEMUIParam->pOEMOptItems[3].Sel);
            break;

        default:
            break;
    }

    return lReturn;
}


////////////////////////////////////////////////////////////////////////////////
//
// Initializes OptItems to display OEM document property UI.
//
static HRESULT hrDocumentPropertyPage(DWORD dwMode, POEMCUIPPARAM pOEMUIParam)
{   
    if ( NULL == pOEMUIParam || NULL == pOEMUIParam->pOEMDM )
    {
        ERR(ERRORTEXT("hrDocumentPropertyPage() ERROR_INVALID_PARAMETER.\r\n"));
        // Return invalid parameter error.
        SetLastError(ERROR_INVALID_PARAMETER);
        return E_FAIL;
    }
    if(NULL == pOEMUIParam->pOEMOptItems)
    {
        // Fill in the number of OptItems to create for OEM document property UI.
        pOEMUIParam->cOEMOptItems = 4;

        VERBOSE(DLLTEXT("hrDocumentPropertyPage() requesting %d number of items.\r\n"), pOEMUIParam->cOEMOptItems);
    }
    else if(dwMode == OEMCUIP_DOCPROP)
    {
        POEMDEV pOEMDev = (POEMDEV) pOEMUIParam->pOEMDM;


        VERBOSE(DLLTEXT("hrDocumentPropertyPage() fill out items.\r\n"), pOEMUIParam->cOEMOptItems);

        // Init UI Callback reference.
        pOEMUIParam->OEMCUIPCallback = OEMUICallBack;

        // Init OEMOptItmes.
        InitOptItems(pOEMUIParam->pOEMOptItems, pOEMUIParam->cOEMOptItems);

        // Fill out tree view items.

        // Water Mark Section Name.
        pOEMUIParam->pOEMOptItems[0].Level = 1;
        pOEMUIParam->pOEMOptItems[0].Flags = OPTIF_COLLAPSE;
        pOEMUIParam->pOEMOptItems[0].pName = GetStringResource(pOEMUIParam->hOEMHeap, ghInstance, IDS_WATERMARK);
        pOEMUIParam->pOEMOptItems[0].Sel = pOEMDev->bWmarkEnabled ? 0 : 1;

        pOEMUIParam->pOEMOptItems[0].pOptType = CreateOptType(pOEMUIParam->hOEMHeap, 2);

        pOEMUIParam->pOEMOptItems[0].pOptType->Type = TVOT_COMBOBOX;
        pOEMUIParam->pOEMOptItems[0].pOptType->pOptParam[0].pData = GetStringResource(pOEMUIParam->hOEMHeap, ghInstance, IDS_ENABLED);
        pOEMUIParam->pOEMOptItems[0].pOptType->pOptParam[0].IconID = IDI_CPSUI_ON;
        pOEMUIParam->pOEMOptItems[0].pOptType->pOptParam[1].pData = GetStringResource(pOEMUIParam->hOEMHeap, ghInstance, IDS_DISABLED);
        pOEMUIParam->pOEMOptItems[0].pOptType->pOptParam[1].IconID = IDI_CPSUI_OFF;


        // WaterMark Text.
        pOEMUIParam->pOEMOptItems[1].Level = 2;
        pOEMUIParam->pOEMOptItems[1].Flags = 0;
        pOEMUIParam->pOEMOptItems[1].pName = GetStringResource(pOEMUIParam->hOEMHeap, ghInstance, IDS_TEXT);
        pOEMUIParam->pOEMOptItems[1].pSel = (LPTSTR) HeapAlloc(pOEMUIParam->hOEMHeap, HEAP_ZERO_MEMORY, MAX_PATH * sizeof(WCHAR));
        if(FAILED(StringCbCopyW((LPWSTR)pOEMUIParam->pOEMOptItems[1].pSel, MAX_PATH * sizeof(WCHAR), pOEMDev->szWaterMark)))
        {
            ERR(ERRORTEXT("hrDocumentPropertyPage() failed to copy water mark text \r\n"));
        }

        pOEMUIParam->pOEMOptItems[1].pOptType = CreateOptType(pOEMUIParam->hOEMHeap, 2);

        pOEMUIParam->pOEMOptItems[1].pOptType->Type = TVOT_EDITBOX;
        pOEMUIParam->pOEMOptItems[1].pOptType->pOptParam[1].IconID = sizeof(((POEMDEV)NULL)->szWaterMark)/sizeof(WCHAR);


        // WaterMark Font Size.
        pOEMUIParam->pOEMOptItems[2].Level = 2;
        pOEMUIParam->pOEMOptItems[2].Flags = 0;
        pOEMUIParam->pOEMOptItems[2].pName = GetStringResource(pOEMUIParam->hOEMHeap, ghInstance, IDS_FONTSIZE);
        pOEMUIParam->pOEMOptItems[2].Sel = FontSizeToIndex(pOEMDev->dwWmarkFontSize);

        pOEMUIParam->pOEMOptItems[2].pOptType = CreateOptType(pOEMUIParam->hOEMHeap, 16);

        pOEMUIParam->pOEMOptItems[2].pOptType->Type = TVOT_COMBOBOX;
        pOEMUIParam->pOEMOptItems[2].pOptType->pOptParam[0].pData = L"8";
        pOEMUIParam->pOEMOptItems[2].pOptType->pOptParam[1].pData = L"9";
        pOEMUIParam->pOEMOptItems[2].pOptType->pOptParam[2].pData = L"10";
        pOEMUIParam->pOEMOptItems[2].pOptType->pOptParam[3].pData = L"11";
        pOEMUIParam->pOEMOptItems[2].pOptType->pOptParam[4].pData = L"12";
        pOEMUIParam->pOEMOptItems[2].pOptType->pOptParam[5].pData = L"14";
        pOEMUIParam->pOEMOptItems[2].pOptType->pOptParam[6].pData = L"16";
        pOEMUIParam->pOEMOptItems[2].pOptType->pOptParam[7].pData = L"18";
        pOEMUIParam->pOEMOptItems[2].pOptType->pOptParam[8].pData = L"20";
        pOEMUIParam->pOEMOptItems[2].pOptType->pOptParam[9].pData = L"22";
        pOEMUIParam->pOEMOptItems[2].pOptType->pOptParam[10].pData = L"24";
        pOEMUIParam->pOEMOptItems[2].pOptType->pOptParam[11].pData = L"26";
        pOEMUIParam->pOEMOptItems[2].pOptType->pOptParam[12].pData = L"28";
        pOEMUIParam->pOEMOptItems[2].pOptType->pOptParam[13].pData = L"36";
        pOEMUIParam->pOEMOptItems[2].pOptType->pOptParam[14].pData = L"48";
        pOEMUIParam->pOEMOptItems[2].pOptType->pOptParam[15].pData = L"72";


        // WaterMark Color.
        pOEMUIParam->pOEMOptItems[3].Level = 2;
        pOEMUIParam->pOEMOptItems[3].Flags = 0;
        pOEMUIParam->pOEMOptItems[3].pName = GetStringResource(pOEMUIParam->hOEMHeap, ghInstance, IDS_COLOR);
        pOEMUIParam->pOEMOptItems[3].Sel = TextColorToIndex(pOEMDev->crWmarkTextColor);

        pOEMUIParam->pOEMOptItems[3].pOptType = CreateOptType(pOEMUIParam->hOEMHeap, 4);

        pOEMUIParam->pOEMOptItems[3].pOptType->Type = TVOT_COMBOBOX;
        pOEMUIParam->pOEMOptItems[3].pOptType->pOptParam[0].pData = GetStringResource(pOEMUIParam->hOEMHeap, ghInstance, IDS_GRAY);
        pOEMUIParam->pOEMOptItems[3].pOptType->pOptParam[1].pData = GetStringResource(pOEMUIParam->hOEMHeap, ghInstance, IDS_RED);
        pOEMUIParam->pOEMOptItems[3].pOptType->pOptParam[2].pData = GetStringResource(pOEMUIParam->hOEMHeap, ghInstance, IDS_GREEN);
        pOEMUIParam->pOEMOptItems[3].pOptType->pOptParam[3].pData = GetStringResource(pOEMUIParam->hOEMHeap, ghInstance, IDS_BLUE);
    }

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
//
// Initializes OptItems.
//
static void InitOptItems(POPTITEM pOptItems, DWORD dwOptItems)
{
    VERBOSE(DLLTEXT("InitOptItems() entry.\r\n"));

    // Zero out memory.
    memset(pOptItems, 0, sizeof(OPTITEM) * dwOptItems);

    // Set each OptItem's size, and Public DM ID.
    for(DWORD dwCount = 0; dwCount < dwOptItems; dwCount++)
    {
        pOptItems[dwCount].cbSize = sizeof(OPTITEM);
        pOptItems[dwCount].DMPubID = DMPUB_NONE;
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// Allocates and initializes OptType for OptItem.
//
static POPTTYPE CreateOptType(HANDLE hHeap, WORD wOptParams)
{
    POPTTYPE    pOptType = NULL;


    VERBOSE(DLLTEXT("CreateOptType() entry.\r\n"));

    // Allocate memory from the heap for the OPTTYPE; the driver will take care of clean up.
    pOptType = (POPTTYPE) HeapAlloc(hHeap, HEAP_ZERO_MEMORY, sizeof(OPTTYPE));
    if(NULL != pOptType)
    {
        // Initialize OPTTYPE.
        pOptType->cbSize = sizeof(OPTTYPE);
        pOptType->Count = wOptParams;

        // Allocate memory from the heap for the OPTPARAMs for the OPTTYPE.
        pOptType->pOptParam = (POPTPARAM) HeapAlloc(hHeap, HEAP_ZERO_MEMORY, wOptParams * sizeof(OPTPARAM));
        if(NULL != pOptType->pOptParam)
        {
            // Initialize the OPTPARAMs.
            for(WORD wCount = 0; wCount < wOptParams; wCount++)
            {
                pOptType->pOptParam[wCount].cbSize = sizeof(OPTPARAM);
            }
        }
        else
        {
            ERR(ERRORTEXT("CreateOptType() failed to allocated memory for OPTPARAMs!\r\n"));

            // Free allocated memory and return NULL.
            HeapFree(hHeap, 0, pOptType);
            pOptType = NULL;
        }
    }
    else
    {
        ERR(ERRORTEXT("CreateOptType() failed to allocated memory for OPTTYPE!\r\n"));
    }

    return pOptType;
}

////////////////////////////////////////////////////////////////////////////////
//
// Converts Font point size to index in combo box.
//
static DWORD FontSizeToIndex(DWORD dwWmarkFontSize)
{
    DWORD   dwIndex;


    switch(dwWmarkFontSize)
    {
        case 8:
            dwIndex = 0;
            break;

        case 9:
            dwIndex = 1;
            break;

        case 10:
            dwIndex = 2;
            break;

        case 11:
            dwIndex = 3;
            break;

        case 12:
            dwIndex = 4;
            break;

        case 14:
            dwIndex = 5;
            break;

        case 16:
            dwIndex = 6;
            break;

        case 18:
            dwIndex = 7;
            break;

        case 20:
            dwIndex = 8;
            break;

        case 22:
            dwIndex = 9;
            break;

        case 24:
            dwIndex = 10;
            break;

        case 26:
            dwIndex = 11;
            break;

        default:
        case 28:
            dwIndex = 12;
            break;

        case 36:
            dwIndex = 13;
            break;

        case 48:
            dwIndex = 14;
            break;

        case 72:
            dwIndex = 15;
            break;
    }


    return dwIndex;
}


////////////////////////////////////////////////////////////////////////////////
//
// Converts Font combo box index to font point size.
//
static DWORD FontIndexToSize(DWORD dwIndex)
{
    DWORD   dwWmarkFontSize;


    switch(dwIndex)
    {
        case 0:
            dwWmarkFontSize = 8;
            break;

        case 1:
            dwWmarkFontSize = 9;
            break;

        case 2:
            dwWmarkFontSize = 10;
            break;

        case 3:
            dwWmarkFontSize = 11;
            break;

        case 4:
            dwWmarkFontSize = 12;
            break;

        case 5:
            dwWmarkFontSize = 14;
            break;

        case 6:
            dwWmarkFontSize = 16;
            break;

        case 7:
            dwWmarkFontSize = 18;
            break;

        case 8:
            dwWmarkFontSize = 20;
            break;

        case 9:
            dwWmarkFontSize = 22;
            break;

        case 10:
            dwWmarkFontSize = 24;
            break;

        case 11:
            dwWmarkFontSize = 26;
            break;

        case 12:
            dwWmarkFontSize = 28;
            break;

        case 13:
            dwWmarkFontSize = 36;
            break;

        case 14:
            dwWmarkFontSize = 48;
            break;

        case 15:
            dwWmarkFontSize = 72;
            break;
    }

    return dwWmarkFontSize;
}


////////////////////////////////////////////////////////////////////////////////
//
// Converts text color to combo box index.
//
static DWORD TextColorToIndex(COLORREF crWmarkTextColor)
{
    DWORD   dwIndex;


    // The color is what ever color is dominate.  If none are, then it is gray.
    if( (GetRValue(crWmarkTextColor) > GetGValue(crWmarkTextColor))
        &&
        (GetGValue(crWmarkTextColor) >= GetBValue(crWmarkTextColor))
      )
    {
        // Set index to red.
        dwIndex = 1;
    }
    else if( (GetRValue(crWmarkTextColor) < GetGValue(crWmarkTextColor))
             &&
             (GetGValue(crWmarkTextColor) > GetBValue(crWmarkTextColor))
            )
    {
        // Set index to green.
        dwIndex = 2;
    }
    else if( (GetRValue(crWmarkTextColor) <= GetGValue(crWmarkTextColor))
             &&
             (GetGValue(crWmarkTextColor) < GetBValue(crWmarkTextColor))
            )
    {
        // Set index to blue.
        dwIndex = 3;
    }
    else
    {
        // Set index to gray.
        dwIndex = 0;
    }

    return dwIndex;
}

////////////////////////////////////////////////////////////////////////////////
//
// Converts text combo box index to text color.
//
static COLORREF IndexToTextColor(DWORD dwIndex)
{
    COLORREF    crWmarkTextColor;

    // Map index to desired text color.
    // We just support 4 colors in the UI: Red, Green, Blue, and gray.
    // The rendering module is capable of using any color for the Water Mark Text.
    switch(dwIndex)
    {
        case 1:
            // Color is red.
            crWmarkTextColor = RGB(255, 216, 216);
            break;

        case 2:
            // Color is green.
            crWmarkTextColor = RGB(216, 255, 216);
            break;

        case 3:
            // Color is blue.
            crWmarkTextColor = RGB(216, 216, 255);
            break;

        default:
        case 0:
            // Color is gray.
            crWmarkTextColor = WATER_MARK_DEFAULT_COLOR;
            break;
    }

    return crWmarkTextColor;
}

////////////////////////////////////////////////////////////////////////////////////
//
//  Retrieves pointer to a String resource.
//
static PTSTR GetStringResource(HANDLE hHeap, HMODULE hModule, UINT uResource)
{
    int     nResult;
    DWORD   dwSize = MAX_PATH;
    PTSTR   pszString = NULL;


    VERBOSE(DLLTEXT("GetStringResource() entered.\r\n"));

    // Allocate buffer for string resource from heap; let the driver clean it up.
    pszString = (PTSTR) HeapAlloc(hHeap, HEAP_ZERO_MEMORY, dwSize * sizeof(TCHAR));
    if(NULL != pszString)
    {
        PTSTR   pTemp;

        // Load string resource; resize after loading so as not to waste memory.
        nResult = LoadString(hModule, uResource, pszString, dwSize);
        if (0 == nResult)
        {
            ERR(ERRORTEXT("GetStringResource() failed to load the the string resource!\r\n"));
            HeapFree(hHeap, 0, pszString);
            pszString=NULL;
        }
        else 
        {
            pTemp = (PTSTR) HeapReAlloc(hHeap, HEAP_ZERO_MEMORY, pszString, (nResult + 1) * sizeof(TCHAR));
            if(NULL != pTemp)
            {
                pszString = pTemp;
            }
        }
    }
    else
    {
        ERR(ERRORTEXT("GetStringResource() failed to allocate string buffer!\r\n"));
    }

    return pszString;
}



