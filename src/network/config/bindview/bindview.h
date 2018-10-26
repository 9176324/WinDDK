//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 2001.
//
//  File:       B I N D V I E W . H
//
//  Contents:   Function Prototypes
//
//  Notes:      
//
//  Author:     Alok Sinha    15-May-01
//
//----------------------------------------------------------------------------

#ifndef _BINDVIEW_H_INCLUDED

#define _BINDVIEW_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <windowsx.h>
#include <wchar.h>
#include <commctrl.h>        // For common controls, e.g. Tree
#include <commdlg.h>
#include <setupapi.h>
#include <devguid.h>

#include "NetCfgAPI.h"
#include "resource.h"

#define ID_STATUS           100
#define APP_NAME            L"BindView"

#define CLIENTS_SELECTED    0
#define SERVICES_SELECTED   1
#define PROTOCOLS_SELECTED  2
#define ADAPTERS_SELECTED   3

#define ITEM_NET_COMPONENTS 1
#define ITEM_NET_BINDINGS   2
#define ITEM_NET_ADAPTERS   4

#define DEFAULT_COMPONENT_SELECTED  CLIENTS_SELECTED

#define WM_NO_COMPONENTS    WM_USER+1

#define MENUITEM_ENABLE     L"Enable"
#define MENUITEM_DISABLE    L"Disable"

extern HINSTANCE    hInstance;
extern const GUID   *pguidNetClass [];
extern LPWSTR       lpszNetClass [];

typedef struct _BIND_UNBIND_INFO {
  LPWSTR lpszInfId;
  BOOL   fBindTo;
} BIND_UNBIND_INFO, *LPBIND_UNBIND_INFO;

//
// Functions defined in bindview.cpp
//

INT_PTR CALLBACK MainDlgProc (HWND hwndDlg,
                              UINT uMsg,
                              WPARAM wParam,
                              LPARAM lParam);

INT_PTR CALLBACK BindComponentDlg (HWND hwndDlg,
                                   UINT uMsg,
                                   WPARAM wParam,
                                   LPARAM lParam);

INT_PTR CALLBACK InstallDlg (HWND hwndDlg,
                                      UINT uMsg,
                                      WPARAM wParam,
                                      LPARAM lParam);

INT_PTR CALLBACK UninstallDlg (HWND hwndDlg,
                               UINT uMsg,
                               WPARAM wParam,
                               LPARAM lParam);

VOID DumpBindings (LPWSTR lpszFile);

VOID InstallSelectedComponentType (HWND   hwndDlg,
                                   LPWSTR lpszInfFile);

HRESULT GetPnpID (LPWSTR lpszInfFile,
                  LPWSTR *lppszPnpID);

HRESULT GetKeyValue (HINF hInf,
                     LPCWSTR lpszSection,
                     LPCWSTR lpszKey,
                     DWORD  dwIndex,
                     LPWSTR *lppszValue);

VOID UninstallSelectedComponent (HWND hwndDlg);

VOID ExpandCollapseAll (HWND hwndTree,
                        HTREEITEM hTreeItem,
                        UINT uiFlag);

BOOL GetFileName (HWND hwndDlg,
                  LPWSTR lpszFilter,
                  LPWSTR lpszTitle,
                  DWORD dwFlags,
                  LPWSTR  lpszFile,
                  LPWSTR  lpszDefExt,
                  BOOL    fSave);

VOID ProcessRightClick (LPNMHDR lpnm);

VOID ShowComponentMenu (HWND hwndOwner,
                        HTREEITEM hItem,
                        LPARAM lParam);

VOID ShowBindingPathMenu (HWND hwndOwner,
                          HTREEITEM hItem,
                          LPARAM lParam,
                          BOOL fEnabled);

BOOL GetItemInfo (HWND hwndTree,
                  HTREEITEM hItem,
                  LPARAM *lParam,
                  LPDWORD  lpdwItemType,
                  BOOL *fEnabled);

HTREEITEM AddBindNameToTree (INetCfgBindingPath *pncbp,
                             HWND hwndTree,
                             HTREEITEM hParent,
                             ULONG  ulIndex);

HTREEITEM AddToTree (HWND hwndTree,
                     HTREEITEM hParent,
                     INetCfgComponent *pncc);

VOID RefreshAll (HWND hwndDlg);

VOID RefreshItemState (HWND hwndTree,
                       HTREEITEM hItem,
                       BOOL fEnable);

VOID RefreshBindings (HWND hwndTree,
                      LPWSTR lpszInfId);

VOID ReleaseMemory (HWND hwndTree,
                    HTREEITEM hTreeItem);


VOID DeleteChildren (HWND hwndTree,
                     HTREEITEM hTreeItem);

HTREEITEM InsertItem (HWND hwndTree,
                      UINT uiType);

BOOL UpdateComponentTypeList (HWND hwndTypeList);


VOID ErrMsg (HRESULT hr,
             LPCWSTR  lpFmt,
             ...);

//
// Functions defined in component.cpp
//

VOID HandleComponentOperation (HWND hwndOwner,
                               ULONG ulSelection,
                               HTREEITEM hItem,
                               LPARAM lParam);

VOID BindUnbindComponents( HWND hwndOwner,
                           HTREEITEM hItem,
                           LPWSTR lpszInfId,
                           BOOL fBindTo);

HRESULT InstallComponent (HWND hwndDlg,
                          const GUID *pguidClass);

HRESULT InstallSpecifiedComponent (LPWSTR lpszInfFile,
                                   LPWSTR lpszPnpID,
                                   const GUID *pguidClass);

DWORD ListCompToBindUnbind (LPWSTR lpszInfId,
                            UINT uiType,
                            HWND hwndTree,
                            BOOL fBound);

BOOL BindUnbind (LPWSTR lpszInfId,
                 HWND hwndTree,
                 BOOL fBind);

VOID ListInstalledComponents (HWND hwndTree,
                              const GUID *pguidClass);

HRESULT UninstallComponent (LPWSTR lpszInfId);

//
// Functions defined in binding.cpp
//

VOID WriteBindings (FILE *fp);

VOID WriteBindingPath (FILE *fp,
                       INetCfgComponent *pncc);

VOID WriteInterfaces (FILE *fp,
                      INetCfgBindingPath *pncbp);

BOOL EnumNetBindings (HWND hwndTree,
                      UINT uiTypeSelected);

VOID ListBindings (INetCfgComponent *pncc,
                   HWND hwndTree,
                   HTREEITEM hTreeItemRoot);

VOID ListInterfaces (INetCfgBindingPath *pncbp,
                     HWND hwndTree,
                     HTREEITEM hTreeItemRoot);

VOID HandleBindingPathOperation (HWND hwndOwner,
                                 ULONG ulSelection,
                                 HTREEITEM hItem,
                                 LPARAM lParam);

VOID EnableBindingPath (HWND hwndOwner,
                        HTREEITEM hItem,
                        LPWSTR lpszTokenPath,
                        BOOL fEnable);

LPWSTR GetComponentId (HWND hwndTree,
                       HTREEITEM hItem);

INetCfgBindingPath *FindBindingPath (INetCfg *pnc,
                                     LPWSTR lpszInfId,
                                     LPWSTR lpszPathTokenSelected);

#endif
