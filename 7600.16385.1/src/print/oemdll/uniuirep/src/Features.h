//+--------------------------------------------------------------------------
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//  ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//  PARTICULAR PURPOSE.
//
//  Copyright  1997 - 2005  Microsoft Corporation.  All Rights Reserved.
//
//  FILE:    Features.h
//
//  PURPOSE:    Defines wrapper class for WinXP PS Features and Options.
//
//--------------------------------------------------------------------------

#pragma once

////////////////////////////////////////////////////////
//      Type Definitions
////////////////////////////////////////////////////////

//
// Struct used to map keyword to information
// such as display name and stickiness.
//
typedef struct _tagKeywordMap
{
    PCSTR   pszKeyword;
    PCWSTR pwszModule;
    UINT uDisplayNameID;
    DWORD dwMode;

} KEYWORDMAP, *PKEYWORDMAP;

typedef CONST KEYWORDMAP *PCKEYWORDMAP;


////////////////////////////////////////////////////////
//      Class Definitions
////////////////////////////////////////////////////////


//
// Option Infomation
//
class COption
{
private:
    PCKEYWORDMAP m_pMapping;
    PWSTR        m_pszDisplayName;
    HANDLE       m_hHeap;

public:
    COption()
        :
        m_pMapping(NULL),
        m_pszDisplayName(NULL),
        m_hHeap(NULL)
    {
    }

    virtual ~COption()
    {
        if (m_hHeap && m_pszDisplayName && !IS_INTRESOURCE(m_pszDisplayName))
        {
            HeapFree(m_hHeap, 0, m_pszDisplayName);
        }
    }

    HRESULT
    Acquire(
        HANDLE hHeap,
        PCKEYWORDMAP pMapping
        );

    CONST PCWSTR
    GetDisplayName() CONST
    {
        return m_pszDisplayName;
    }

    CONST PCSTR
    GetKeyword() CONST
    {
        if (m_pMapping)
        {
            return m_pMapping->pszKeyword;
        }
        else
        {
            return NULL;
        }
    }
};


//
// Class wrapper and container for Core Driver Feature
// Options.
//
class CFeature
{
private:

    PCKEYWORDMAP m_pMapping;

    PWSTR m_pszDisplayName;

    DWORD m_dwOptions;
        // Count of the number of options contained for an instance of the class.

    PCSTR m_pszFeature;
        // Pointer to the feature for which the enumerated options belong.

    HANDLE m_hHeap;
        // Heap to do allocations from.

    COption *m_pOptions;
        // Array of info about each option for a feature

    DWORD m_Sel;
        // An optitem selection is indicated either with a pointer or an
        // index.  Since combo-boxes use indices, and this custom UI only
        // generates combo boxes, this class always uses an index to
        // indicate the current selection.

public:

    CFeature();

    virtual ~CFeature();

    //
    // Populate Options list for specified keyword.
    //
    HRESULT
    Acquire(
        __in HANDLE hHeap,
        __in IPrintCoreHelper* pHelper,
        __in PCSTR pszFeature
        );

    //
    // Returns number of feature options contained in class instance.
    //
    DWORD
    GetCount() const {return m_dwOptions;}

    //
    // Return nth option.
    //
    CONST COption *
    GetOption(
        DWORD dwIndex
        ) const;


    //
    // Get the currently selected option index.
    //
    LONG
    GetSelection()
    {
        return m_Sel;
    }

    //
    // Find option with matching keyword string.
    //
    BOOL
    FindOption(
        __in PCSTR pszOption,
        __out DWORD *pdwOption
        ) const;

    //
    // Initializes options portion of OPTITEM.
    //
    HRESULT
    InitOptItem(
        __in HANDLE hHeap,
        __inout POPTITEM pOptItem
        );

    //
    // Refresh option selection.
    //
    HRESULT
    RefreshSelection(
        __in IPrintCoreHelper* pHelper
        );

    CONST PCWSTR
    GetDisplayName()
    {
        return m_pszDisplayName;
    }

    CONST PCSTR
    GetKeyword()
    {
        return m_pszFeature;
    }

    DWORD
    GetMode()
    {
        return m_pMapping->dwMode;
    }

private:

    VOID
    Clear();
};


//
// Class wrapper and container for Core Driver Features.
//
class CFeatureCollection
{
private:

    DWORD m_dwFeatures;
        // Count of the number of features.

    DWORD m_dwDocFeatures;
        // Count of the number of document sticky features

    DWORD m_dwPrintFeatures;
        // Count of the number of printer sticky features.

    HANDLE m_hHeap;
        // Heap to do allocations from.

    CFeature *m_pFeatures;
        // Array of feature information about each of the enumerate features.

public:

    CFeatureCollection();
    virtual ~CFeatureCollection();

    //
    // Populates the Feature list
    //
    HRESULT Acquire(
        HANDLE hHeap,
        IPrintCoreHelper* pHelper
        );

    //
    // Returns number of features contained in class instance.
    //
    DWORD GetCount(
        DWORD dwMode = 0
        ) const;

    // Returns pointer to option class for nth feature.
    //
    CFeature* GetFeature(
        DWORD wIndex,
        DWORD dwMode = 0
        ) const;

private:

    VOID Clear();

    DWORD GetModelessIndex(
        DWORD dwIndex,
        DWORD dwMode
        ) const;
};


class CFeaturePairs
{
private:

    PRINT_FEATURE_OPTION * m_pData;
    size_t m_cElements;

    VOID
    ClearPair(
        size_t iIndex
        );

public:

    CFeaturePairs(size_t cElements);

    ~CFeaturePairs();

    HRESULT
    SetPair(
        size_t iIndex,
        PCSTR pcszFeature,
        PCSTR pcszOption
        );

    PRINT_FEATURE_OPTION *
    GetData()
    {
        return m_pData;
    }

    size_t
    GetElementCount()
    {
        return m_cElements;
    }
};

////////////////////////////////////////////////////////
//      Prototypes
////////////////////////////////////////////////////////

HRESULT
GetDisplayNameFromMapping(
    __in HANDLE hHeap,
    __in PCKEYWORDMAP pMapping,
    __out PWSTR *ppszDisplayName
    );

PCKEYWORDMAP
GetKeywordMapping(
    __in_ecount(dwMapSize) PCKEYWORDMAP pKeywordMap,
    DWORD dwMapSize,
    PCSTR pszKeyword
    );

BOOL
IsFeatureOptItem(
    POPTITEM pOptItem
    );

HRESULT
SaveFeatureOptItems(
    HANDLE hHeap,
    IPrintCoreHelper* pHelper,
    HWND hWnd,
    __in_ecount(dwItems) POPTITEM pOptItem,
    DWORD dwItems
    );

HRESULT
GetChangedFeatureOptions(
    __in_ecount(dwItems) POPTITEM pOptItem,
    DWORD dwItems,
    __deref_out_opt CFeaturePairs **ppPairs
    );

PCSTR
GetOptionKeywordFromOptItem(
    POPTITEM pOptItem
    );

HRESULT
RefreshOptItemSelection(
    IPrintCoreHelper* pHelper,
    __in_ecount(dwItems) POPTITEM pOptItems,
    DWORD dwItems
    );

HRESULT
GetFirstConflictingFeature(
    IPrintCoreHelper *pHelper,
    __in_ecount(dwItems) POPTITEM pOptItem,
    DWORD dwItems,
    CONST PRINT_FEATURE_OPTION **pConstraints,
    DWORD *pdwConstraints
    );

POPTTYPE
CreateOptType(
    HANDLE hHeap,
    WORD wOptParams
    );

HRESULT
GetStringResource(
    __in HANDLE hHeap,
    __in HMODULE hModule,
    UINT uResource,
    __out PWSTR *ppszString
    );

