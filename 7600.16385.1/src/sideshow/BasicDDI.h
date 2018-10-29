//-----------------------------------------------------------------------
// <copyright file="BasicDDI.h" company="Microsoft">
//      Copyright (c) 2006 Microsoft Corporation. All rights reserved.
// </copyright>
//
// Module:
//      BasicDDI.h
//
// Description:
//      This module implements the ISideShowDriver DDI which is used
//      by the Windows SideShow class extension.
//
//-----------------------------------------------------------------------

#pragma once

#include <WindowsSideShowClassExtension.h>

class CWssBasicDDI :
    public CComObjectRoot,
    public ISideShowDriver
{
public:
    CWssBasicDDI();
    virtual ~CWssBasicDDI();

    HRESULT Initialize(IWDFNamedPropertyStore* pStore);

    HRESULT Deinitialize(void);

    DECLARE_NOT_AGGREGATABLE(CWssBasicDDI)

    BEGIN_COM_MAP(CWssBasicDDI)
        COM_INTERFACE_ENTRY(ISideShowDriver)
    END_COM_MAP()

    //
    // Overriden DDI methods
    //
public:
    __override STDMETHOD(OnSetCurrentUser)(__in const SID* pUser);

    __override STDMETHOD(OnGetCurrentUser)(__out SID** ppUser);

    __override STDMETHOD(OnSetUserState)(__in const SID* pUser,
                                         __in const USER_STATE state);

    __override STDMETHOD(OnSetTime)(__in const FILETIME time);

    __override STDMETHOD(OnSetTimeZone)(__in const SIDESHOW_TIME_ZONE_INFORMATION* pTimeZoneInformation);

    __override STDMETHOD(OnSetShortDateFormat)(__in const SID* user,
                                               __in LPCWSTR wszDateFormat);

    __override STDMETHOD(OnSetLongDateFormat)(__in const SID* user,
                                              __in LPCWSTR wszDateFormat);

    __override STDMETHOD(OnSetShortTimeFormat)(__in const SID* user,
                                               __in LPCWSTR wszTimeFormat);

    __override STDMETHOD(OnSetLongTimeFormat)(__in const SID* user,
                                              __in LPCWSTR wszTimeFormat);

    __override STDMETHOD(OnGetDeviceFirmwareVersion)(__deref_out_opt LPWSTR* pwszVersion);

    __override STDMETHOD(OnGetDeviceManufacturer)(__deref_out_opt LPWSTR* pwszManufacturer);

    __override STDMETHOD(OnGetDeviceName)(__deref_out_opt LPWSTR* pwszName);

    __override STDMETHOD(OnGetDeviceEndpoints)(__deref_out_opt ENDPOINT_ID** rgEndpoints,
                                               __out DWORD* pcEndpoints);

    __override STDMETHOD(OnGetDeviceCapabilities)(__in const PROPERTYKEY* pKey,
                                                  __out PROPVARIANT* pvValue);

    __override STDMETHOD(OnGetPreEnabledApplications)(__deref_ecount(*pcApplications) APPLICATION_ID** ppguidApps,
                                                      __deref_ecount(*pcApplications) ENDPOINT_ID** ppEndpoints,
                                                      __out DWORD* pcApplications);

    __override STDMETHOD(OnGetApplicationOrder)(__deref_out_opt APPLICATION_ID** ppAppIds,
                                                __out DWORD* pcAppIds);

    __override STDMETHOD(OnSetApplicationOrder)(__in_ecount(cApps) const APPLICATION_ID* pApps,
                                                __in const DWORD cApps);

    __override STDMETHOD(OnAddApplication)(__in const SID* user,
                                           __in REFAPPLICATION_ID rAppId,
                                           __in REFENDPOINT_ID rEndPoint,
                                           __in LPCWSTR wszName,
                                           __in const DWORD cachePolicy,
                                           __in const DWORD dwOnlineOnly,
                                           __in_bcount_opt(cbLargeIcon) const unsigned char* pbLargeIcon,
                                           __in const DWORD cbLargeIcon,
                                           __in_bcount_opt(cbSmallIcon) const unsigned char* pbSmallIcon,
                                           __in const DWORD cbSmallIcon,
                                           __in_bcount_opt(cbMiniIcon) const unsigned char* pbMiniIcon,
                                           __in const DWORD cbMiniIcon);

    __override STDMETHOD(OnRemoveApplication)(__in const SID* user,
                                              __in REFAPPLICATION_ID rAppId);

    __override STDMETHOD(OnRemoveAllApplications)(__in const SID* user);

    __override STDMETHOD(OnSetNotificationsEnabled)(__in const SID* pUserSid,
                                                    __in const BOOL fIsEnabled);

    __override STDMETHOD(OnAddNotification)(__in const SID* user,
                                            __in REFAPPLICATION_ID rAppId,
                                            __in const NOTIFICATION_ID notificationId,
                                            __in const FILETIME ftExpiration,
                                            __in LPCWSTR wszTitle,
                                            __in LPCWSTR wszMessage,
                                            __in_bcount_opt(cbImage) const unsigned char* pbImage,
                                            __in const DWORD cbImage);

    __override STDMETHOD(OnRemoveNotification)(__in const SID* user,
                                               __in REFAPPLICATION_ID rAppId,
                                               __in const NOTIFICATION_ID notificationId);

    __override STDMETHOD(OnRemoveAllNotifications)(__in const SID* user,
                                                   __in REFAPPLICATION_ID rAppId);

    __override STDMETHOD(OnAddContent)(__in const SID* user,
                                       __in REFAPPLICATION_ID rAppId,
                                       __in REFENDPOINT_ID rRendererId,
                                       __in const CONTENT_ID contentId,
                                       __in_bcount_opt(cbData) const unsigned char* pData,
                                       __in const DWORD cbData);

    __override STDMETHOD(OnRemoveContent)(__in const SID* user,
                                          __in REFAPPLICATION_ID rAppId,
                                          __in REFENDPOINT_ID rRenderId,
                                          __in const CONTENT_ID contentId);

    __override STDMETHOD(OnRemoveAllContent)(__in const SID* user,
                                             __in REFAPPLICATION_ID rAppId,
                                             __in REFENDPOINT_ID rRenderId);

    __override STDMETHOD(OnProcessWpdMessage)(__in IUnknown* pinValues,
                                              __in IUnknown* poutValues);

    __override STDMETHOD(OnSetLanguage)(__in const SID* user,
                                        __in LPCWSTR wszLang);

protected:
    STDMETHOD(GetApplicationOrderString)(__deref_out_opt LPWSTR* ppwszKey);

    STDMETHOD(GetStoredApplicationOrder)(APPLICATION_ID** ppAppIds,
                                         DWORD* pcAppIds);

    STDMETHOD(SetStoredApplicationOrder)(const APPLICATION_ID* pApps,
                                         const DWORD cApps);

protected:
    SID* m_pUserSID;
    size_t m_cbUserSID; // This is the maximum size of a SID, not the actual size of the current m_pUserSID

    CComPtr<IWDFNamedPropertyStore> m_pPropertyStore;
};

