//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//  ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//  PARTICULAR PURPOSE.
//
//  Copyright  2001 - 2003  Microsoft Corporation.  All Rights Reserved.
//
//  FILE:    Helper.cpp
//    
//
//  PURPOSE:  Implementation of wrapper class for Driver UI Helper interface.
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
#include <PRCOMOEM.H>
#include "Helper.h"

// StrSafe.h needs to be included last
// to disallow bad string functions.
#include <STRSAFE.H>




////////////////////////////////////////////////////////
//      Internal Macros
////////////////////////////////////////////////////////


// Macros for simplifying calling the Driver UI Helper interfaces correctly.

#define CALL_HELPER(MethodName, args)                                           \
    if (IsEqualGUID(&m_iidUIHelper, &IID_IPrintOemDriverUI))                    \
    {                                                                           \
        return static_cast<IPrintOemDriverUI *>(m_pUIHelper)->MethodName args;  \
    }                                                                           \
    else if (IsEqualGUID(&m_iidUIHelper, &IID_IPrintCoreUI2))                   \
    {                                                                           \
        return static_cast<IPrintCoreUI2 *>(m_pUIHelper)->MethodName args;      \
    }                                                                           \
    return E_NOINTERFACE;

#define CALL_HELPER2(MethodName, args)                                      \
    if (IsEqualGUID(&m_iidUIHelper, &IID_IPrintCoreUI2))                    \
    {                                                                       \
        return static_cast<IPrintCoreUI2 *>(m_pUIHelper)->MethodName args;  \
    }                                                                       \
    return E_NOINTERFACE;




/////////////////////////////////////////////////////////////
//
//  CUIHelper Class Methods
//

//
// Private Methods
//

// Clears or initializes data members.
void CUIHelper::Clear()
{
    // Clear stored interface
    m_pUIHelper     = NULL;
    m_iidUIHelper   = GUID_NULL;
}


//
// Public Methods
//

// Default contructor.
CUIHelper::CUIHelper()
{
    // Initialize the data members.
    Clear();
}

// Constructor with assignment.
CUIHelper::CUIHelper(const IID &HelperIID, PVOID pHelper)
{
    // Assign the interface.
    Assign(HelperIID, pHelper);
}

// Destructor
CUIHelper::~CUIHelper()
{
    // Release the interface reference.
    Release();
}

// Stores helper interface in helper entry structure.
void CUIHelper::Assign(const IID &HelperIID, PVOID pHelper)
{
    // If we already have a helper interface, release it. 
    if(IsValid())
    {
        Release();
    }

    // Store helper interface and IID for it.
    m_pUIHelper     = static_cast<IUnknown*>(pHelper);
    m_iidUIHelper   = HelperIID;
}

// Releases the helper interface and 
// our removes the references to it.
ULONG CUIHelper::Release() 
{ 
    ULONG   ulRef   = 0;


    if(IsValid())
    {
        // Release the interface.
        // Since IPrintCoreUI2 inherits from IPrintOemDriverUI,
        // it is safe to cast both types of helper interfaces
        // to IPrintOemDriverUI for purposes of calling a 
        // method implemented in IPrintOemDriverUI.
        // NOTE: can't cast to IUnknown since it just has pure virtual 
        //       calls for AddRef and Release.
        ulRef = static_cast<IPrintOemDriverUI *>(m_pUIHelper)->Release();

        // Clear the data members.
        Clear();
    }

    return ulRef;
}


//
// IPrintOemDriverUI methods
//

//
// Helper function to get driver settings. This function is only supported
// for UI plugins that do not fully replace core driver's standard UI.
//

HRESULT __stdcall CUIHelper::DrvGetDriverSetting(
                    PVOID   pci,
                    PCSTR   Feature,
                    PVOID   pOutput,
                    DWORD   cbSize,
                    PDWORD  pcbNeeded,
                    PDWORD  pdwOptionsReturned
                    )
{
    CALL_HELPER(DrvGetDriverSetting, 
                    (pci, 
                     Feature, 
                     pOutput, 
                     cbSize, 
                     pcbNeeded, 
                     pdwOptionsReturned
                    )
               );
}

//
// Helper function to allow OEM plugins upgrade private registry
// settings. This function is supported for any UI plugins and should be
// called only by OEM's UpgradePrinter.
//

HRESULT __stdcall CUIHelper::DrvUpgradeRegistrySetting(
                    HANDLE   hPrinter,
                    PCSTR    pFeature,
                    PCSTR    pOption
                    )
{
    CALL_HELPER(DrvUpgradeRegistrySetting, 
                    (hPrinter,
                     pFeature,
                     pOption
                    )
               );
}

//
// Helper function to allow OEM plugins to update the driver UI settings.
// This function is only supported for UI plugins that do not fully replace
// core driver's standard UI. It should be called only when the UI is present.
//

HRESULT __stdcall CUIHelper::DrvUpdateUISetting(
                    PVOID    pci,
                    PVOID    pOptItem,
                    DWORD    dwPreviousSelection,
                    DWORD    dwMode
                    )
{
    CALL_HELPER(DrvUpdateUISetting,
                    (pci,
                     pOptItem,
                     dwPreviousSelection,
                     dwMode
                    )
                );
}

//
// IPrintCoreUI2 new methods
//

//
// Following four helper functions are only supported for UI plugins that fully
// replace core driver's standard UI. They should only be called by the UI plugin's
// DocumentPropertySheets, DevicePropertySheets and their property sheet callback
// functions.
//
// Helper function to retrieve driver's current setting as a list of
// feature/option keyword pairs.
//

HRESULT __stdcall CUIHelper::GetOptions(
                       IN  POEMUIOBJ  poemuiobj,
                       IN  DWORD      dwFlags,
                       IN  PCSTR      pmszFeaturesRequested,
                       IN  DWORD      cbIn,
                       OUT PSTR       pmszFeatureOptionBuf,
                       IN  DWORD      cbSize,
                       OUT PDWORD     pcbNeeded)
{
    CALL_HELPER2(GetOptions,
                    (poemuiobj,
                     dwFlags,
                     pmszFeaturesRequested,
                     cbIn,
                     pmszFeatureOptionBuf,
                     cbSize,
                     pcbNeeded
                    )
                );
}

//
// Helper function to change driver's setting using a list of feature/option
// keyword pairs.
//

HRESULT __stdcall CUIHelper::SetOptions(
                       IN  POEMUIOBJ  poemuiobj,
                       IN  DWORD      dwFlags,
                       IN  PCSTR      pmszFeatureOptionBuf,
                       IN  DWORD      cbIn,
                       OUT PDWORD     pdwResult)
{
    CALL_HELPER2(SetOptions,
                    (poemuiobj,
                     dwFlags,
                     pmszFeatureOptionBuf,
                     cbIn,
                     pdwResult
                    )
                );
}

//
// Helper function to retrieve the option(s) of a given feature that are
// constrained in driver's current setting.
//

HRESULT __stdcall CUIHelper::EnumConstrainedOptions(
                                   IN  POEMUIOBJ  poemuiobj,
                                   IN  DWORD      dwFlags,
                                   IN  PCSTR      pszFeatureKeyword,
                                   OUT PSTR       pmszConstrainedOptionList,
                                   IN  DWORD      cbSize,
                                   OUT PDWORD     pcbNeeded)
{
    CALL_HELPER2(EnumConstrainedOptions,
                    (poemuiobj,
                     dwFlags,
                     pszFeatureKeyword,
                     pmszConstrainedOptionList,
                     cbSize,
                     pcbNeeded
                    )
                );
}

//
// Helper function to retrieve a list of feature/option keyword pairs from
// driver's current setting that conflict with the given feature/option pair.
//

HRESULT __stdcall CUIHelper::WhyConstrained(
                           IN  POEMUIOBJ  poemuiobj,
                           IN  DWORD      dwFlags,
                           IN  PCSTR      pszFeatureKeyword,
                           IN  PCSTR      pszOptionKeyword,
                           OUT PSTR       pmszReasonList,
                           IN  DWORD      cbSize,
                           OUT PDWORD     pcbNeeded)
{
    CALL_HELPER2(WhyConstrained,
                    (poemuiobj,
                     dwFlags,
                     pszFeatureKeyword,
                     pszOptionKeyword,
                     pmszReasonList,
                     cbSize,
                     pcbNeeded
                    )
                );
}

//
// Following five helper functions are supported for any UI plugins.
//
// Helper function to retrieve global attribute.
//

HRESULT __stdcall CUIHelper::GetGlobalAttribute(
                               IN  POEMUIOBJ  poemuiobj,
                               IN  DWORD      dwFlags,
                               IN  PCSTR      pszAttribute,
                               OUT PDWORD     pdwDataType,
                               OUT PBYTE      pbData,
                               IN  DWORD      cbSize,
                               OUT PDWORD     pcbNeeded)
{
    CALL_HELPER2(GetGlobalAttribute,
                    (poemuiobj,
                     dwFlags,
                     pszAttribute,
                     pdwDataType,
                     pbData,
                     cbSize,
                     pcbNeeded
                    )
                );
}


//
// Helper function to retrieve attribute of a given feature.
//

HRESULT __stdcall CUIHelper::GetFeatureAttribute(
                                IN  POEMUIOBJ  poemuiobj,
                                IN  DWORD      dwFlags,
                                IN  PCSTR      pszFeatureKeyword,
                                IN  PCSTR      pszAttribute,
                                OUT PDWORD     pdwDataType,
                                OUT PBYTE      pbData,
                                IN  DWORD      cbSize,
                                OUT PDWORD     pcbNeeded)
{
    CALL_HELPER2(GetFeatureAttribute,
                    (poemuiobj,
                     dwFlags,
                     pszFeatureKeyword,
                     pszAttribute,
                     pdwDataType,
                     pbData,
                     cbSize,
                     pcbNeeded
                    )
                );
}

//
// Helper function to retrieve attribute of a given feature/option selection.
//

HRESULT __stdcall CUIHelper::GetOptionAttribute(
                               IN  POEMUIOBJ  poemuiobj,
                               IN  DWORD      dwFlags,
                               IN  PCSTR      pszFeatureKeyword,
                               IN  PCSTR      pszOptionKeyword,
                               IN  PCSTR      pszAttribute,
                               OUT PDWORD     pdwDataType,
                               OUT PBYTE      pbData,
                               IN  DWORD      cbSize,
                               OUT PDWORD     pcbNeeded)
{
    CALL_HELPER2(GetOptionAttribute,
                    (poemuiobj,
                     dwFlags,
                     pszFeatureKeyword,
                     pszOptionKeyword,
                     pszAttribute,
                     pdwDataType,
                     pbData,
                     cbSize,
                     pcbNeeded
                    )
                );
}

//
// Helper function to retrieve the list of feature keyword.
//

HRESULT __stdcall CUIHelper::EnumFeatures(
                         IN  POEMUIOBJ  poemuiobj,
                         IN  DWORD      dwFlags,
                         OUT PSTR       pmszFeatureList,
                         IN  DWORD      cbSize,
                         OUT PDWORD     pcbNeeded)
{
    CALL_HELPER2(EnumFeatures,
                    (poemuiobj,
                     dwFlags,
                     pmszFeatureList,
                     cbSize,
                     pcbNeeded
                    )
                );
}

//
// Helper function to retrieve the list of options keyword of a given feature.
//

HRESULT __stdcall CUIHelper::EnumOptions(
                        IN  POEMUIOBJ  poemuiobj,
                        IN  DWORD      dwFlags,
                        IN  PCSTR      pszFeatureKeyword,
                        OUT PSTR       pmszOptionList,
                        IN  DWORD      cbSize,
                        OUT PDWORD     pcbNeeded)
{
    CALL_HELPER2(EnumOptions,
                    (poemuiobj,
                     dwFlags,
                     pszFeatureKeyword,
                     pmszOptionList,
                     cbSize,
                     pcbNeeded
                    )
                );
}

//
// Helper function to query system simulation support
//

HRESULT __stdcall CUIHelper::QuerySimulationSupport(
                                   IN  HANDLE  hPrinter,
                                   IN  DWORD   dwLevel,
                                   OUT PBYTE   pCaps,
                                   IN  DWORD   cbSize,
                                   OUT PDWORD  pcbNeeded)
{
    CALL_HELPER2(QuerySimulationSupport,
                    (hPrinter,
                     dwLevel,
                     pCaps,
                     cbSize,
                     pcbNeeded
                    )
                );
}




