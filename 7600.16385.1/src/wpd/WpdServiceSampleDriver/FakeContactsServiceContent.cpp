#include "stdafx.h"

#include "FakeContactsServiceContent.tmh"

// Change unit is a subset of the custom properties supported by a contact object.
// This typically contains at least one read-only property that indicates that the
// object has changed
const PROPERTYKEY* g_ContactsServiceChangeUnit[1] =
{
    &MyContactVersionIdentifier,
};

const PropertyAttributeInfo g_SupportedServiceProperties[] =
{
    {&WPD_OBJECT_ID,                                VT_LPWSTR,          UnspecifiedForm_CanRead_CannotWrite_CannotDelete_Fast, NULL},
    {&WPD_OBJECT_PERSISTENT_UNIQUE_ID,              VT_LPWSTR,          UnspecifiedForm_CanRead_CannotWrite_CannotDelete_Fast, NULL},
    {&WPD_OBJECT_PARENT_ID,                         VT_LPWSTR,          UnspecifiedForm_CanRead_CannotWrite_CannotDelete_Fast, NULL},
    {&WPD_OBJECT_NAME,                              VT_LPWSTR,          UnspecifiedForm_CanRead_CanWrite_CannotDelete_Fast,    NULL},
    {&WPD_OBJECT_FORMAT,                            VT_CLSID,           UnspecifiedForm_CanRead_CannotWrite_CannotDelete_Fast, NULL},
    {&WPD_OBJECT_CONTENT_TYPE,                      VT_CLSID,           UnspecifiedForm_CanRead_CannotWrite_CannotDelete_Fast, NULL},
    {&WPD_OBJECT_CAN_DELETE,                        VT_BOOL,            UnspecifiedForm_CanRead_CannotWrite_CannotDelete_Fast, NULL},
    {&WPD_OBJECT_CONTAINER_FUNCTIONAL_OBJECT_ID,    VT_LPWSTR,          UnspecifiedForm_CanRead_CannotWrite_CannotDelete_Fast, NULL},
    {&WPD_FUNCTIONAL_OBJECT_CATEGORY,               VT_CLSID,           UnspecifiedForm_CanRead_CannotWrite_CannotDelete_Fast, NULL},
    {&WPD_SERVICE_VERSION,                          VT_LPWSTR,          UnspecifiedForm_CanRead_CannotWrite_CannotDelete_Fast, NULL},
    {&PKEY_Services_ServiceDisplayName,             VT_LPWSTR,          UnspecifiedForm_CanRead_CannotWrite_CannotDelete_Fast, L"DisplayName"},
    {&PKEY_FullEnumSyncSvc_SyncFormat,              VT_CLSID,           UnspecifiedForm_CanRead_CannotWrite_CannotDelete_Fast, L"FullEnumSyncFormat"},
    {&PKEY_Services_ServiceIcon,                    VT_VECTOR | VT_UI1, UnspecifiedForm_CanRead_CannotWrite_CannotDelete_Fast, L"ServiceIcon"},
    {&PKEY_FullEnumSyncSvc_VersionProps,            VT_VECTOR | VT_UI1, UnspecifiedForm_CanRead_CannotWrite_CannotDelete_Fast, L"FullEnumVersionProps"}
};

HRESULT FakeContactsServiceContent::InitializeContent(
    __inout DWORD *pdwLastObjectID)
{
    HRESULT hr = S_OK;

    if (pdwLastObjectID == NULL)
    {
        hr = E_POINTER;
        CHECK_HR(hr, "Cannot have NULL parameter");
        return hr;
    }

    // Add contact objects to the contact service
    for(DWORD dwContactIndex = 1; dwContactIndex <= NUM_CONTACT_OBJECTS; dwContactIndex++)
    {
        (*pdwLastObjectID)++;

#pragma prefast(suppress: __WARNING_MEMORY_LEAK, "pContact does not leak memory, because ~FakeContent() will release it")
        FakeContactContent* pContact = new FakeContactContent();
        if (pContact)
        {
            pContact->Name.Format(L"Contact%d", *pdwLastObjectID);
            pContact->ParentID                    = ObjectID;
            pContact->ContainerFunctionalObjectID = ObjectID;
            pContact->ObjectID.Format(L"%d", *pdwLastObjectID);
            pContact->PersistentUniqueID.Format(L"PersistentUniqueID_%ws", pContact->ObjectID);
            pContact->ParentPersistentUniqueID    = PersistentUniqueID;
            pContact->RequiredScope               = CONTACTS_SERVICE_ACCESS;

            pContact->DisplayName.Format(L"Surname%d, FirstName%d", dwContactIndex, dwContactIndex);
            pContact->MobilePhoneNumber           = L"(425) 555 0821";
            pContact->PrimaryPhoneNumber          = L"(425) 556 6010";
            pContact->PrimaryEmail.Format(L"FirstName%d@Company%d.com", dwContactIndex, dwContactIndex);
            pContact->PersonalFullPostalAddress.Format(L"One Microsoft Way, Redmond, WA 98052, USA");

            m_Children.Add(pContact);
        }
        else
        {
            hr = E_OUTOFMEMORY;
            CHECK_HR(hr, "Failed to allocate contact content at index %d", dwContactIndex);
            return hr;
        }
    }

    return hr;
}

HRESULT FakeContactsServiceContent::CreatePropertiesOnlyObject(
    __in    IPortableDeviceValues* pObjectProperties,
    __out   DWORD*                 pdwLastObjectID,
    __out   FakeContent**          ppNewObject)
{
    HRESULT hr              = S_OK;
    HRESULT hrTemp          = S_OK;
    GUID    guidContentType = WPD_CONTENT_TYPE_UNSPECIFIED;
    GUID    guidFormat      = WPD_OBJECT_FORMAT_UNSPECIFIED;

    if (ppNewObject == NULL)
    {
        hr = E_POINTER;
        CHECK_HR(hr, "Cannot have NULL parameter");
        return hr;
    }
    *ppNewObject = NULL;

    if (pObjectProperties == NULL || pdwLastObjectID == NULL)
    {
        hr = E_POINTER;
        CHECK_HR(hr, "Cannot have NULL parameter");
        return hr;
    }

    // Get WPD_OBJECT_FORMAT
    if (hr == S_OK)
    {
        hr = pObjectProperties->GetGuidValue(WPD_OBJECT_FORMAT, &guidFormat);
        CHECK_HR(hr, "Failed to get WPD_OBJECT_FORMAT");
    }

    if ((hr == S_OK) && (guidFormat != WPD_OBJECT_FORMAT_VCARD2) && (guidFormat != WPD_OBJECT_FORMAT_ABSTRACT_CONTACT))
    {
        hr = E_INVALIDARG;
        CHECK_HR(hr, "Invalid Format [%ws]", CComBSTR(guidFormat));
    }

    if (hr == S_OK)
    {
        // Create the object
        FakeContactContent* pContent = NULL;

        pContent = new FakeContactContent();

        if (pContent)
        {
            (*pdwLastObjectID)++;
            pContent->ParentID                    = ObjectID;
            pContent->ParentPersistentUniqueID    = PersistentUniqueID;
            pContent->ObjectID.Format(L"%d", (*pdwLastObjectID));
            pContent->ContentType                 = guidContentType;
            pContent->Format                      = guidFormat;
            pContent->PersistentUniqueID.Format(L"PersistentUniqueID_%ws", pContent->ObjectID);
            pContent->ContainerFunctionalObjectID = ObjectID;
            pContent->RequiredScope               = CONTACTS_SERVICE_ACCESS;

            // Get the other optional contact properties.
            LPWSTR  pszTempString   = NULL;
            hrTemp = pObjectProperties->GetStringValue(WPD_OBJECT_NAME, &pszTempString);
            if(hrTemp == S_OK)
            {
                pContent->Name = pszTempString;
                CoTaskMemFree(pszTempString);
            }

            hrTemp = pObjectProperties->GetStringValue(WPD_CONTACT_DISPLAY_NAME, &pszTempString);
            if(hrTemp == S_OK)
            {
                pContent->DisplayName = pszTempString;
                CoTaskMemFree(pszTempString);
            }

            hrTemp = pObjectProperties->GetStringValue(WPD_CONTACT_PRIMARY_PHONE, &pszTempString);
            if(hrTemp == S_OK)
            {
                pContent->PrimaryPhoneNumber = pszTempString;
                CoTaskMemFree(pszTempString);
            }

            hrTemp = pObjectProperties->GetStringValue(WPD_CONTACT_MOBILE_PHONE, &pszTempString);
            if(hrTemp == S_OK)
            {
                pContent->MobilePhoneNumber = pszTempString;
                CoTaskMemFree(pszTempString);
            }

            hrTemp = pObjectProperties->GetStringValue(WPD_CONTACT_PRIMARY_EMAIL_ADDRESS, &pszTempString);
            if(hrTemp == S_OK)
            {
                pContent->PrimaryEmail = pszTempString;
                CoTaskMemFree(pszTempString);
            }

            hrTemp = pObjectProperties->GetStringValue(WPD_CONTACT_PERSONAL_FULL_POSTAL_ADDRESS, &pszTempString);
            if(hrTemp == S_OK)
            {
                pContent->PersonalFullPostalAddress = pszTempString;
                CoTaskMemFree(pszTempString);
            }

            m_Children.Add(pContent);
            *ppNewObject = pContent;
        }
        else
        {
            hr = E_OUTOFMEMORY;
            CHECK_HR(hr, "Failed to allocate new FakeContactContent object");
        }
    }

    return hr;
}

HRESULT FakeContactsServiceContent::GetSupportedProperties(
    __out   IPortableDeviceKeyCollection* pKeys)
{
    HRESULT hr = S_OK;

    if (pKeys == NULL)
    {
        hr = E_POINTER;
        CHECK_HR(hr, "Cannot have NULL parameter");
        return hr;
    }

    for (DWORD dwIndex = 0; (dwIndex < ARRAYSIZE(g_SupportedServiceProperties)) && (hr == S_OK); dwIndex++)
    {
        // Common WPD service properties
        hr = pKeys->Add(*g_SupportedServiceProperties[dwIndex].pKey);
        CHECK_HR(hr, "Failed to add common service property");
    }

    return hr;
}

HRESULT FakeContactsServiceContent::GetPropertyAttributes(
            REFPROPERTYKEY         Key,
    __out   IPortableDeviceValues* pAttributes)
{
    HRESULT hr = S_OK;

    if(pAttributes == NULL)
    {
        hr = E_POINTER;
        CHECK_HR(hr, "Cannot have NULL parameter");
        return hr;
    }

    hr = SetPropertyAttributes(Key, &g_SupportedServiceProperties[0], ARRAYSIZE(g_SupportedServiceProperties), pAttributes);

    return hr;
}

HRESULT FakeContactsServiceContent::GetValue(
            REFPROPERTYKEY         Key,
    __out   IPortableDeviceValues* pStore)
{
    HRESULT hr = S_OK;

    PropVariantWrapper pvValue;

    if (IsEqualPropertyKey(Key, WPD_OBJECT_ID))
    {
        // Add WPD_OBJECT_ID
        pvValue = ObjectID;
        hr = pStore->SetValue(WPD_OBJECT_ID, &pvValue);
        CHECK_HR(hr, ("Failed to set WPD_OBJECT_ID"));
    }
    else if (IsEqualPropertyKey(Key, WPD_OBJECT_NAME))
    {
        // Add WPD_OBJECT_NAME
        pvValue = Name;
        hr = pStore->SetValue(WPD_OBJECT_NAME, &pvValue);
        CHECK_HR(hr, ("Failed to set WPD_OBJECT_NAME"));
    }
    else if (IsEqualPropertyKey(Key, WPD_OBJECT_PERSISTENT_UNIQUE_ID))
    {
        // Add WPD_OBJECT_PERSISTENT_UNIQUE_ID
        pvValue = PersistentUniqueID;
        hr = pStore->SetValue(WPD_OBJECT_PERSISTENT_UNIQUE_ID, &pvValue);
        CHECK_HR(hr, ("Failed to set WPD_OBJECT_PERSISTENT_UNIQUE_ID"));
    }
    else if (IsEqualPropertyKey(Key, WPD_OBJECT_PARENT_ID))
    {
        // Add WPD_OBJECT_PARENT_ID
        pvValue = ParentID;
        hr = pStore->SetValue(WPD_OBJECT_PARENT_ID, &pvValue);
        CHECK_HR(hr, ("Failed to set WPD_OBJECT_PARENT_ID"));
    }
    else if (IsEqualPropertyKey(Key, WPD_OBJECT_FORMAT))
    {
        // Add WPD_OBJECT_FORMAT
        hr = pStore->SetGuidValue(WPD_OBJECT_FORMAT, Format);
        CHECK_HR(hr, ("Failed to set WPD_OBJECT_FORMAT"));
    }
    else if (IsEqualPropertyKey(Key, WPD_OBJECT_CONTENT_TYPE))
    {
        // Add WPD_OBJECT_CONTENT_TYPE
        hr = pStore->SetGuidValue(WPD_OBJECT_CONTENT_TYPE, ContentType);
        CHECK_HR(hr, ("Failed to set WPD_OBJECT_CONTENT_TYPE"));
    }
    else if (IsEqualPropertyKey(Key, WPD_OBJECT_CAN_DELETE))
    {
        // Add WPD_OBJECT_CAN_DELETE
        hr = pStore->SetBoolValue(WPD_OBJECT_CAN_DELETE, CanDelete);
        CHECK_HR(hr, ("Failed to set WPD_OBJECT_CAN_DELETE"));
    }
    else if (IsEqualPropertyKey(Key, WPD_FUNCTIONAL_OBJECT_CATEGORY))
    {
        // Add WPD_FUNCTIONAL_OBJECT_CATEGORY
        hr = pStore->SetGuidValue(WPD_FUNCTIONAL_OBJECT_CATEGORY, FunctionalCategory);
        CHECK_HR(hr, ("Failed to set WPD_FUNCTIONAL_OBJECT_CATEGORY"));
    }
    else if (IsEqualPropertyKey(Key, WPD_OBJECT_CONTAINER_FUNCTIONAL_OBJECT_ID))
    {
        // Add WPD_OBJECT_CONTAINER_FUNCTIONAL_OBJECT_ID
        hr = pStore->SetStringValue(WPD_OBJECT_CONTAINER_FUNCTIONAL_OBJECT_ID, ContainerFunctionalObjectID);
        CHECK_HR(hr, ("Failed to set WPD_OBJECT_CONTAINER_FUNCTIONAL_OBJECT_ID"));
    }
    else if (IsEqualPropertyKey(Key, WPD_SERVICE_VERSION))
    {
        // Add WPD_SERVICE_VERSION
        hr = pStore->SetStringValue(WPD_SERVICE_VERSION, Version);
        CHECK_HR(hr, ("Failed to set WPD_SERVICE_VERSION"));
    }
    else if (IsEqualPropertyKey(Key, PKEY_Services_ServiceDisplayName))
    {
        pvValue = HumanReadableName;
        hr = pStore->SetValue(PKEY_Services_ServiceDisplayName, &pvValue);
        CHECK_HR(hr, ("Failed to set PKEY_Services_ServiceDisplayName"));
    }
    else if (IsEqualPropertyKey(Key, PKEY_FullEnumSyncSvc_SyncFormat))
    {
        hr = pStore->SetGuidValue(PKEY_FullEnumSyncSvc_SyncFormat, PreferredSyncFormat);
        CHECK_HR(hr, ("Failed to set PKEY_FullEnumSyncSvc_SyncFormat"));
    }
    else if (IsEqualPropertyKey(Key, PKEY_Services_ServiceIcon))
    {
        hr = GetIconData(pStore);
        CHECK_HR(hr, "Failed to set PKEY_Services_ServiceIcon");
    }
    else if (IsEqualPropertyKey(Key, PKEY_FullEnumSyncSvc_VersionProps))
    {
        hr = GetVICData(pStore);
        CHECK_HR(hr, "Failed to set PKEY_FullEnumSyncSvc_VersionProps");
    }
    else
    {
        hr = HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
        CHECK_HR(hr, "Property {%ws}.%d is not supported", CComBSTR(Key.fmtid), Key.pid);
    }

    return hr;
}

/**
 *  This method is called to get the contacts service icon data.
 *
 *  The parameters sent to us are:
 *  pValues - An IPortableDeviceValues to be populated with the icon data
 *
 *  The driver should:
 *  Retrieve the icon data and set it in pValues for PKEY_Services_ServiceIcon
 */
HRESULT FakeContactsServiceContent::GetIconData(
    __out   IPortableDeviceValues* pStore)
{
    HRESULT hr         = S_OK;
    PBYTE   pIconData  = NULL;
    DWORD   cbIconData = 0;

    if (pStore == NULL)
    {
        hr = E_POINTER;
        CHECK_HR(hr, "Cannot have NULL parameter");
        return hr;
    }

    pIconData = GetResourceData(IDR_WPD_SAMPLEDRIVER_SERVICE_ICON);
    cbIconData = GetResourceSize(IDR_WPD_SAMPLEDRIVER_SERVICE_ICON);

    if ((pIconData == NULL) || (cbIconData == 0))
    {
        hr = E_UNEXPECTED;
        CHECK_HR(hr, "Failed to get resource representing the service icon data");
    }

    if (hr == S_OK)
    {
        hr = pStore->SetBufferValue(PKEY_Services_ServiceIcon, pIconData, cbIconData);
        CHECK_HR(hr, "Failed to copy the icon data to IPortableDeviceValues");
    }

    return hr;
}

/**
 *  This method is called to get the contacts service's full enumeration version properties
 *
 *  The parameters sent to us are:
 *  pValues - An IPortableDeviceValues to be populated with the version property data
 *
 *  The driver should:
 *  Retrieve the version property data blob and set it in pValues for SVCPROP_FullEnumVersionProps
 *
 * Version property data blob must adhere to the following format:
 *
 * Count of Change Unit Groups
 *     Change Unit PROPERTYKEY (group 0)
 *     Count of Keys (group 0)
 *     Key0, Key1..Keyn(group 0)
 *     ...
 *     Change Unit PROPERTYKEY (group 1)
 *     Count of Keys (group 1)
 *     Key0, Key1..Keyn(group 1)
 */
HRESULT FakeContactsServiceContent::GetVICData(
    __out   IPortableDeviceValues* pStore)
{
    HRESULT hr                = E_OUTOFMEMORY;
    const DWORD   cGroup      = 1;   // currently support only 1 group
    DWORD         cVIC        = ARRAYSIZE(g_ContactsServiceChangeUnit);

    // Our VIC contains only a single change unit, identified by GUID_NULL
    DWORD   cbVIC =
                    sizeof(cGroup)      +                                           // count of groups of change units
                    // Currently we support only 1 group
                    sizeof(PROPERTYKEY)   +                                         // change unit identifier
                    sizeof(cVIC)        +                                           // change unit count
                    ARRAYSIZE(g_ContactsServiceChangeUnit) * sizeof(PROPERTYKEY);   // Properties in this change unit

    BYTE    *pVIC  = new BYTE[cbVIC];

    if (pVIC)
    {
        hr = E_FAIL;

        BYTE *pPos   = pVIC;
        const BYTE *pEnd = pVIC + cbVIC;

        if ((pPos + sizeof(cGroup)) <= pEnd)
        {
            // count of groups
            memcpy(pPos, &cGroup, sizeof(cGroup));
            pPos+=sizeof(cGroup);

            if ((pPos + sizeof(PROPERTYKEY)) <= pEnd)
            {
                // Change Unit Identifier
                memcpy(pPos, &WPD_PROPERTY_NULL, sizeof(PROPERTYKEY));
                pPos+=sizeof(PROPERTYKEY);

                if ((pPos + sizeof(cVIC)) <= pEnd)
                {
                    // Number of items in the change unit
                    memcpy(pPos, &cVIC, sizeof(cVIC));
                    pPos+=sizeof(cVIC);

                    // Change Unit Property List
                    DWORD i = 0;
                    while ((i < cVIC) && (pPos + sizeof(PROPERTYKEY) <= pEnd))
                    {
                        memcpy(pPos, g_ContactsServiceChangeUnit[i], sizeof(PROPERTYKEY));
                        pPos+=sizeof(PROPERTYKEY);
                        i++;
                    }

                    if ((pPos == pEnd) && (i == cVIC))
                    {
                        // All done
                        hr = pStore->SetBufferValue(PKEY_FullEnumSyncSvc_VersionProps, pVIC, cbVIC);
                    }
                }
            }
        }
        delete [] pVIC;
    }

    CHECK_HR(hr, "Failed to copy the PKEY_FullEnumSyncSvc_VersionProps data to IPortableDeviceValues");
    return hr;
}

