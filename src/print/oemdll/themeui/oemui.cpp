//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//  ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//  PARTICULAR PURPOSE.
//
//  Copyright  1997 - 2003  Microsoft Corporation.  All Rights Reserved.
//
//  FILE:    OEMUI.cpp
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
#include "fusutils.h"
#include "oemui.h"

// StrSafe.h needs to be included last
// to disallow bad string functions.
#include <STRSAFE.H>



////////////////////////////////////////////////////////
//      INTERNAL MACROS and DEFINES
////////////////////////////////////////////////////////

typedef struct _tagCBUserData 
{
    HANDLE          hComPropSheet;
    HANDLE          hPropPage;
    POEMUIPSPARAM   pOEMUIParam;
    PFNCOMPROPSHEET pfnComPropSheet;

} CBUSERDATA, *PCBUSERDATA;



////////////////////////////////////////////////////////
//      INTERNAL PROTOTYPES
////////////////////////////////////////////////////////

static HRESULT hrDocumentPropertyPage(DWORD dwMode, POEMCUIPPARAM pOEMUIParam);
static HRESULT hrPrinterPropertyPage(DWORD dwMode, POEMCUIPPARAM pOEMUIParam);
LONG APIENTRY OEMPrinterUICallBack(PCPSUICBPARAM pCallbackParam, POEMCUIPPARAM pOEMUIParam);
LONG APIENTRY OEMDocUICallBack(PCPSUICBPARAM pCallbackParam, POEMCUIPPARAM pOEMUIParam);
LONG APIENTRY OEMDocUICallBack2(PCPSUICBPARAM pCallbackParam);
INT_PTR CALLBACK DevicePropPageProc(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam);
static POPTITEM CreateOptItems(HANDLE hHeap, DWORD dwOptItems);
static void InitOptItems(POPTITEM pOptItems, DWORD dwOptItems);
static POPTTYPE CreateOptType(HANDLE hHeap, WORD wOptParams);
static PTSTR GetStringResource(HANDLE hHeap, HANDLE hModule, UINT uResource);



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
            hResult = hrPrinterPropertyPage(dwMode, pOEMUIParam);
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
// Initializes OptItems to display OEM document property UI.
//
static HRESULT hrDocumentPropertyPage(DWORD dwMode, POEMCUIPPARAM pOEMUIParam)
{
    if(NULL == pOEMUIParam->pOEMOptItems)
    {
        // Fill in the number of OptItems to create for OEM document property UI.
        pOEMUIParam->cOEMOptItems = 1;

        VERBOSE(DLLTEXT("hrDocumentPropertyPage() requesting %d number of items.\r\n"), pOEMUIParam->cOEMOptItems);
    }
    else
    {
        POEMDEV pOEMDev = (POEMDEV) pOEMUIParam->pOEMDM;


        VERBOSE(DLLTEXT("hrDocumentPropertyPage() fill out %d items.\r\n"), pOEMUIParam->cOEMOptItems);

        // Init UI Callback reference.
        pOEMUIParam->OEMCUIPCallback = OEMDocUICallBack;

        // Init OEMOptItmes.
        InitOptItems(pOEMUIParam->pOEMOptItems, pOEMUIParam->cOEMOptItems);

        // Fill out tree view items.

        // New section.
        pOEMUIParam->pOEMOptItems[0].Level  = 1;
        pOEMUIParam->pOEMOptItems[0].Flags  = OPTIF_COLLAPSE;
        pOEMUIParam->pOEMOptItems[0].pName  = GetStringResource(pOEMUIParam->hOEMHeap, pOEMUIParam->hModule, IDS_ADV_SECTION);
        pOEMUIParam->pOEMOptItems[0].Sel    = pOEMDev->dwAdvancedData;

        pOEMUIParam->pOEMOptItems[0].pOptType = CreateOptType(pOEMUIParam->hOEMHeap, 2);

        pOEMUIParam->pOEMOptItems[0].pOptType->Type                 = TVOT_UDARROW;
        pOEMUIParam->pOEMOptItems[0].pOptType->pOptParam[1].IconID  = 0;
        pOEMUIParam->pOEMOptItems[0].pOptType->pOptParam[1].lParam  = 100;
    }

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
//
// Initializes OptItems to display OEM printer property UI.
//
static HRESULT hrPrinterPropertyPage(DWORD dwMode, POEMCUIPPARAM pOEMUIParam)
{
    if(NULL == pOEMUIParam->pOEMOptItems)
    {
        // Fill in the number of OptItems to create for OEM printer property UI.
        pOEMUIParam->cOEMOptItems = 1;

        VERBOSE(DLLTEXT("hrPrinterPropertyPage() requesting %d number of items.\r\n"), pOEMUIParam->cOEMOptItems);
    }
    else
    {
        DWORD   dwError;
        DWORD   dwDeviceValue;
        DWORD   dwType;
        DWORD   dwNeeded;


        VERBOSE(DLLTEXT("hrPrinterPropertyPage() fill out %d items.\r\n"), pOEMUIParam->cOEMOptItems);

        // Get device settings value from printer.
        dwError = GetPrinterData(pOEMUIParam->hPrinter, OEMUI_VALUE, &dwType, (PBYTE) &dwDeviceValue,
                                   sizeof(dwDeviceValue), &dwNeeded);
        if( (ERROR_SUCCESS != dwError)
            ||
            (dwDeviceValue > 100)
          )
        {
            // Failed to get the device value or value is invalid, just use the default.
            dwDeviceValue = 0;
        }

        // Init UI Callback reference.
        pOEMUIParam->OEMCUIPCallback = OEMPrinterUICallBack;

        // Init OEMOptItmes.
        InitOptItems(pOEMUIParam->pOEMOptItems, pOEMUIParam->cOEMOptItems);

        // Fill out tree view items.

        // New section.
        pOEMUIParam->pOEMOptItems[0].Level  = 1;
        pOEMUIParam->pOEMOptItems[0].Flags  = OPTIF_COLLAPSE;
        pOEMUIParam->pOEMOptItems[0].pName  = GetStringResource(pOEMUIParam->hOEMHeap, pOEMUIParam->hModule, IDS_DEV_SECTION);
        pOEMUIParam->pOEMOptItems[0].Sel    = dwDeviceValue;

        pOEMUIParam->pOEMOptItems[0].pOptType = CreateOptType(pOEMUIParam->hOEMHeap, 2);

        pOEMUIParam->pOEMOptItems[0].pOptType->Type                 = TVOT_UDARROW;
        pOEMUIParam->pOEMOptItems[0].pOptType->pOptParam[1].IconID  = 0;
        pOEMUIParam->pOEMOptItems[0].pOptType->pOptParam[1].lParam  = 100;
    }

    return S_OK;
}


////////////////////////////////////////////////////////////////////////////////
//
// Adds property page to Document property sheet.
//
HRESULT hrOEMDocumentPropertySheets(PPROPSHEETUI_INFO pPSUIInfo, LPARAM lParam, 
                                    IPrintOemDriverUI*  pOEMHelp)
{
    LONG_PTR    lResult;


    VERBOSE(DLLTEXT("OEMDocumentPropertySheets() entry.\r\n"));

    // Validate parameters.
    if( (NULL == pPSUIInfo)
        ||
        (PROPSHEETUI_INFO_VERSION != pPSUIInfo->Version)
      )
    {
        ERR(ERRORTEXT("OEMDocumentPropertySheets() ERROR_INVALID_PARAMETER.\r\n"));

        // Return invalid parameter error.
        SetLastError(ERROR_INVALID_PARAMETER);
        return  E_FAIL;
    }

    // Do action.
    switch(pPSUIInfo->Reason)
    {
        case PROPSHEETUI_REASON_INIT:
            {
                DWORD           dwSheets        = 0;
                PCBUSERDATA     pUserData;
                POEMUIPSPARAM   pOEMUIParam     = (POEMUIPSPARAM) pPSUIInfo->lParamInit;
                HANDLE          hHeap           = pOEMUIParam->hOEMHeap;
                POEMDEV         pOEMDev         = (POEMDEV) pOEMUIParam->pOEMDM;
                COMPROPSHEETUI  Sheet;


                // Init property page.
                memset(&Sheet, 0, sizeof(COMPROPSHEETUI));
                Sheet.cbSize            = sizeof(COMPROPSHEETUI);
                Sheet.Flags             = CPSUIF_UPDATE_PERMISSION;
                Sheet.hInstCaller       = ghInstance;
                Sheet.pCallerName       = GetStringResource(hHeap, ghInstance, IDS_NAME);
                Sheet.pHelpFile         = NULL;
                Sheet.pfnCallBack       = OEMDocUICallBack2;
                Sheet.pDlgPage          = CPSUI_PDLGPAGE_TREEVIEWONLY;
                Sheet.cOptItem          = 1;
                Sheet.IconID            = IDI_CPSUI_PRINTER;
                Sheet.pOptItemName      = GetStringResource(hHeap, ghInstance, IDS_SECTION);
                Sheet.CallerVersion     = 0x100;
                Sheet.OptItemVersion    = 0x100;

                // Init user data.
                pUserData = (PCBUSERDATA) HeapAlloc(hHeap, HEAP_ZERO_MEMORY, sizeof(CBUSERDATA));
                pUserData->hComPropSheet    = pPSUIInfo->hComPropSheet;
                pUserData->pfnComPropSheet  = pPSUIInfo->pfnComPropSheet;
                pUserData->pOEMUIParam      = pOEMUIParam;
                Sheet.UserData              = (ULONG_PTR) pUserData;

                // Create OptItems for page.
                Sheet.pOptItem = CreateOptItems(hHeap, Sheet.cOptItem);

                // Initialize OptItems
                Sheet.pOptItem[0].Level     = 1;
                Sheet.pOptItem[0].Flags     = OPTIF_COLLAPSE;
                Sheet.pOptItem[0].pName     = GetStringResource(hHeap, ghInstance, IDS_SECTION);
                Sheet.pOptItem[0].Sel       = pOEMDev->dwDriverData;

                Sheet.pOptItem[0].pOptType = CreateOptType(hHeap, 2);

                Sheet.pOptItem[0].pOptType->Type                    = TVOT_UDARROW;
                Sheet.pOptItem[0].pOptType->pOptParam[1].IconID     = 0;
                Sheet.pOptItem[0].pOptType->pOptParam[1].lParam     = 100;

                // NOTE: Don't need to do anything with Activation Contexts
                //       to get Themed UI, since Compstui will create this 
                //       page using it's Activation Context which specifies
                //       comctl v6.

                // Add property sheets.
                lResult = pPSUIInfo->pfnComPropSheet(pPSUIInfo->hComPropSheet, CPSFUNC_ADD_PCOMPROPSHEETUI, 
                                                     (LPARAM)&Sheet, (LPARAM)&dwSheets);
            }
            break;

        case PROPSHEETUI_REASON_GET_INFO_HEADER:
            {
                PPROPSHEETUI_INFO_HEADER    pHeader = (PPROPSHEETUI_INFO_HEADER) lParam;

                pHeader->pTitle = (LPTSTR)PROP_TITLE;
                lResult = TRUE;
            }
            break;

        case PROPSHEETUI_REASON_GET_ICON:
            // No icon
            lResult = 0;
            break;

        case PROPSHEETUI_REASON_SET_RESULT:
            {
                PSETRESULT_INFO pInfo = (PSETRESULT_INFO) lParam;

                lResult = pInfo->Result;
            }
            break;

        case PROPSHEETUI_REASON_DESTROY:
            lResult = TRUE;
            break;
    }

    pPSUIInfo->Result = lResult;
    return S_OK;
}


////////////////////////////////////////////////////////////////////////////////
//
// Adds property page to printer property sheet.
//
HRESULT hrOEMDevicePropertySheets(PPROPSHEETUI_INFO pPSUIInfo, LPARAM lParam)
{
    LONG_PTR    lResult;


    VERBOSE(DLLTEXT("hrOEMDevicePropertySheets(%#x, %#x) entry\r\n"), pPSUIInfo, lParam);

    // Validate parameters.
    if( (NULL == pPSUIInfo)
        ||
        (PROPSHEETUI_INFO_VERSION != pPSUIInfo->Version)
      )
    {
        ERR(ERRORTEXT("hrOEMDevicePropertySheets() ERROR_INVALID_PARAMETER.\r\n"));

        // Return invalid parameter error.
        SetLastError(ERROR_INVALID_PARAMETER);
        return E_FAIL;
    }

    Dump(pPSUIInfo);

    // Do action.
    switch(pPSUIInfo->Reason)
    {
        case PROPSHEETUI_REASON_INIT:
            {
                HANDLE          hActCtx;
                PROPSHEETPAGE   Page;

                // Init property page.
                memset(&Page, 0, sizeof(PROPSHEETPAGE));
                Page.dwSize         = sizeof(PROPSHEETPAGE);
                Page.dwFlags        = PSP_DEFAULT;
                Page.hInstance      = ghInstance;
                Page.pszTemplate    = MAKEINTRESOURCE(IDD_DEVICE_PROPPAGE);
                Page.pfnDlgProc     = DevicePropPageProc;
                Page.hActCtx        = GetMyActivationContext();

                // Set the flag to indicate that our PROPSHEETPAGE
                // has an Activation Context.
                // The Activation Context indicates with version of
                // comctl for Compstui to create our PROPSHEETPAGE
                // with. To get Themed UI we need to specify comctl v6.
                if( (NULL != Page.hActCtx) && (INVALID_HANDLE_VALUE != Page.hActCtx))
                {
                    Page.dwFlags |= PSP_USEFUSIONCONTEXT;
                }

                // Add property sheets.
                lResult = pPSUIInfo->pfnComPropSheet(pPSUIInfo->hComPropSheet, CPSFUNC_ADD_PROPSHEETPAGE, (LPARAM)&Page, 0);

                // NOTE: The Activation Context is released when the DLL is unloaded 
                //       during DLL_PROCESS_DETACH.

                VERBOSE(DLLTEXT("hrOEMDevicePropertySheets() pfnComPropSheet returned %d.\r\n"), lResult);
            }
            break;

        case PROPSHEETUI_REASON_GET_INFO_HEADER:
            {
                PPROPSHEETUI_INFO_HEADER    pHeader = (PPROPSHEETUI_INFO_HEADER) lParam;

                pHeader->pTitle = (LPTSTR)PROP_TITLE;
                lResult = TRUE;
            }
            break;

        case PROPSHEETUI_REASON_GET_ICON:
            // No icon
            lResult = 0;
            break;

        case PROPSHEETUI_REASON_SET_RESULT:
            {
                PSETRESULT_INFO pInfo = (PSETRESULT_INFO) lParam;

                lResult = pInfo->Result;
            }
            break;

        case PROPSHEETUI_REASON_DESTROY:
            lResult = TRUE;
            break;
    }

    pPSUIInfo->Result = lResult;
    return S_OK;
}


////////////////////////////////////////////////////////////////////////////////
//
// OptItems call back for OEM printer property UI.
//
LONG APIENTRY OEMPrinterUICallBack(PCPSUICBPARAM pCallbackParam, POEMCUIPPARAM pOEMUIParam)
{
    LONG    lReturn = CPSUICB_ACTION_NONE;
    POEMDEV pOEMDev = (POEMDEV) pOEMUIParam->pOEMDM;


    VERBOSE(DLLTEXT("OEMPrinterUICallBack() entry, Reason is %d.\r\n"), pCallbackParam->Reason);

    switch(pCallbackParam->Reason)
    {
        case CPSUICB_REASON_APPLYNOW:
            {
                DWORD   dwDriverValue = pOEMUIParam->pOEMOptItems[0].Sel;

                // Store OptItems state in printer data.
                SetPrinterData(pOEMUIParam->hPrinter, OEMUI_VALUE, REG_DWORD, (PBYTE) &dwDriverValue, sizeof(DWORD));
            }
            break;

        default:
            break;
    }

    return lReturn;
}


////////////////////////////////////////////////////////////////////////////////
//
// Call back for OEM device property UI.
//
INT_PTR CALLBACK DevicePropPageProc(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uiMsg)
    {
        case WM_COMMAND:
            switch(HIWORD(wParam))
            {
                case BN_CLICKED:
                    switch(LOWORD(wParam))
                    {
                        case IDC_CALIBRATE:
                            // Just display a message that the printer is calibrated,
                            // since we don't acutally calibrate anything.
                            {
                                TCHAR   szName[MAX_PATH];
                                TCHAR   szCalibrated[MAX_PATH];


                                LoadString(ghInstance, IDS_NAME, szName, sizeof(szName)/sizeof(szName[0]));
                                LoadString(ghInstance, IDS_CALIBRATED, szCalibrated, sizeof(szCalibrated)/sizeof(szCalibrated[0]));
                                MessageBox(hDlg, szCalibrated, szName, MB_OK);
                            }
                            break;
                    }
                    break;

                default:
                    return FALSE;
            }
            return TRUE;

        case WM_NOTIFY:
            {
                switch (((LPNMHDR)lParam)->code)  // type of notification message
                {
                    case PSN_SETACTIVE:
                        break;
    
                    case PSN_KILLACTIVE:
                        break;

                    case PSN_APPLY:
                        break;

                    case PSN_RESET:
                        break;
                }
            }
            break;
    }

    return FALSE;
} 


////////////////////////////////////////////////////////////////////////////////
//
// OptItems call back for OEM document property UI.
//
LONG APIENTRY OEMDocUICallBack(PCPSUICBPARAM pCallbackParam, POEMCUIPPARAM pOEMUIParam)
{
    LONG    lReturn = CPSUICB_ACTION_NONE;
    POEMDEV pOEMDev = (POEMDEV) pOEMUIParam->pOEMDM;


    VERBOSE(DLLTEXT("OEMDocUICallBack() entry, Reason is %d.\r\n"), pCallbackParam->Reason);

    switch(pCallbackParam->Reason)
    {
        case CPSUICB_REASON_APPLYNOW:
            // Store OptItems state in DEVMODE.
            pOEMDev->dwAdvancedData = pOEMUIParam->pOEMOptItems[0].Sel;
            break;

        case CPSUICB_REASON_KILLACTIVE:
            pOEMDev->dwAdvancedData = pOEMUIParam->pOEMOptItems[0].Sel;
            break;

        case CPSUICB_REASON_SETACTIVE:
            if(pOEMUIParam->pOEMOptItems[0].Sel != pOEMDev->dwAdvancedData)
            {
                pOEMUIParam->pOEMOptItems[0].Sel    = pOEMDev->dwAdvancedData;
                pOEMUIParam->pOEMOptItems[0].Flags |= OPTIF_CHANGED;
                lReturn                             = CPSUICB_ACTION_OPTIF_CHANGED;
            }
            break;

        default:
            break;
    }

    return lReturn;
}


LONG APIENTRY OEMDocUICallBack2(PCPSUICBPARAM pCallbackParam)
{
    LONG            lReturn = CPSUICB_ACTION_NONE;
    PCBUSERDATA     pUserData = (PCBUSERDATA) pCallbackParam->UserData;
    POEMDEV         pOEMDev = (POEMDEV) pUserData->pOEMUIParam->pOEMDM;


    VERBOSE(DLLTEXT("OEMDocUICallBack2() entry, Reason is %d.\r\n"), pCallbackParam->Reason);

    switch(pCallbackParam->Reason)
    {
        case CPSUICB_REASON_APPLYNOW:
            pOEMDev->dwDriverData = pCallbackParam->pOptItem[0].Sel;
            pUserData->pfnComPropSheet(pUserData->hComPropSheet, CPSFUNC_SET_RESULT,
            	                       (LPARAM)pUserData->hPropPage,
               	                       (LPARAM)CPSUI_OK);
            break;

        case CPSUICB_REASON_KILLACTIVE:
            pOEMDev->dwDriverData = pCallbackParam->pOptItem[0].Sel;
            break;

        case CPSUICB_REASON_SETACTIVE:
            if(pCallbackParam->pOptItem[0].Sel != pOEMDev->dwDriverData)
            {
                pCallbackParam->pOptItem[0].Sel     = pOEMDev->dwDriverData;
                pCallbackParam->pOptItem[0].Flags  |= OPTIF_CHANGED;
                lReturn                             = CPSUICB_ACTION_OPTIF_CHANGED;
            }
            break;

        default:
            break;
    }

    return lReturn;
}


////////////////////////////////////////////////////////////////////////////////
//
// Creates and Initializes OptItems.
//
static POPTITEM CreateOptItems(HANDLE hHeap, DWORD dwOptItems)
{
    POPTITEM    pOptItems = NULL;


    // Allocate memory for OptItems;
    pOptItems = (POPTITEM) HeapAlloc(hHeap, HEAP_ZERO_MEMORY, sizeof(OPTITEM) * dwOptItems);
    if(NULL != pOptItems)
    {
        InitOptItems(pOptItems, dwOptItems);
    }
    else
    {
        ERR(ERRORTEXT("CreateOptItems() failed to allocate memory for OPTITEMs!\r\n"));
    }

    return pOptItems;
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
        pOptItems[dwCount].cbSize   = sizeof(OPTITEM);
        pOptItems[dwCount].DMPubID  = DMPUB_NONE;
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
        pOptType->cbSize    = sizeof(OPTTYPE);
        pOptType->Count     = wOptParams;

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

////////////////////////////////////////////////////////////////////////////////////
//
//  Retrieves pointer to a String resource.
//
static PTSTR GetStringResource(HANDLE hHeap, HANDLE hModule, UINT uResource)
{
    int     nResult;
    DWORD   dwSize = MAX_PATH;
    PTSTR   pszString = NULL;


    VERBOSE(DLLTEXT("GetStringResource(%#x, %#x, %d) entered.\r\n"), hHeap, hModule, uResource);

    // Allocate buffer for string resource from heap; let the driver clean it up.
    pszString = (PTSTR) HeapAlloc(hHeap, HEAP_ZERO_MEMORY, dwSize * sizeof(TCHAR));
    if(NULL != pszString)
    {
        // Load string resource; resize after loading so as not to waste memory.
        nResult = LoadString((HMODULE)hModule, uResource, pszString, dwSize);
        if(nResult > 0)
        {
            PTSTR   pszTemp;


            VERBOSE(DLLTEXT("LoadString() returned %d!\r\n"), nResult);
            VERBOSE(DLLTEXT("String load was \"%s\".\r\n"), pszString);

            pszTemp = (PTSTR) HeapReAlloc(hHeap, HEAP_ZERO_MEMORY, pszString, (nResult + 1) * sizeof(TCHAR));
            if(NULL != pszTemp)
            {
                pszString = pszTemp;
            }
            else
            {
                ERR(ERRORTEXT("GetStringResource() HeapReAlloc() of string retrieved failed! (Last Error was %d)\r\n"), GetLastError());
            }
        }
        else
        {
            ERR(ERRORTEXT("LoadString() returned %d! (Last Error was %d)\r\n"), nResult, GetLastError());
            ERR(ERRORTEXT("GetStringResource() failed to load string resource %d!\r\n"), uResource);

            pszString = NULL;
        }
    }
    else
    {
        ERR(ERRORTEXT("GetStringResource() failed to allocate string buffer!\r\n"));
    }

    return pszString;
}



