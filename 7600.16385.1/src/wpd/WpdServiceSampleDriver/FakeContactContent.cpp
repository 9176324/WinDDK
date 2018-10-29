#include "stdafx.h"

#include "FakeContactContent.tmh"

// Properties supported by a contact
const PropertyAttributeInfo g_SupportedContactProperties[] =
{
    {&WPD_OBJECT_ID,                             VT_LPWSTR, UnspecifiedForm_CanRead_CannotWrite_CannotDelete_Fast,  NULL}, 
    {&WPD_OBJECT_PERSISTENT_UNIQUE_ID,           VT_LPWSTR, UnspecifiedForm_CanRead_CannotWrite_CannotDelete_Fast,  NULL}, 
    {&WPD_OBJECT_PARENT_ID,                      VT_LPWSTR, UnspecifiedForm_CanRead_CannotWrite_CannotDelete_Fast,  NULL}, 
    {&WPD_OBJECT_NAME,                           VT_LPWSTR, UnspecifiedForm_CanRead_CanWrite_CannotDelete_Fast,     NULL}, 
    {&WPD_OBJECT_FORMAT,                         VT_CLSID,  UnspecifiedForm_CanRead_CannotWrite_CannotDelete_Fast,  NULL}, 
    {&WPD_OBJECT_CONTENT_TYPE,                   VT_CLSID,  UnspecifiedForm_CanRead_CannotWrite_CannotDelete_Fast,  NULL}, 
    {&WPD_OBJECT_CAN_DELETE,                     VT_BOOL,   UnspecifiedForm_CanRead_CannotWrite_CannotDelete_Fast,  NULL}, 
    {&WPD_OBJECT_CONTAINER_FUNCTIONAL_OBJECT_ID, VT_LPWSTR, UnspecifiedForm_CanRead_CannotWrite_CannotDelete_Fast,  NULL}, 
    {&WPD_CONTACT_DISPLAY_NAME,                  VT_LPWSTR, UnspecifiedForm_CanRead_CanWrite_CannotDelete_Fast,     NULL},
    {&WPD_CONTACT_MOBILE_PHONE,                  VT_LPWSTR, UnspecifiedForm_CanRead_CanWrite_CannotDelete_Fast,     NULL},
    {&WPD_CONTACT_PRIMARY_EMAIL_ADDRESS,         VT_LPWSTR, UnspecifiedForm_CanRead_CanWrite_CannotDelete_Fast,     NULL},
    {&WPD_CONTACT_PRIMARY_PHONE,                 VT_LPWSTR, UnspecifiedForm_CanRead_CanWrite_CannotDelete_Fast,     NULL},
    {&WPD_CONTACT_PERSONAL_FULL_POSTAL_ADDRESS,  VT_LPWSTR, UnspecifiedForm_CanRead_CanWrite_CannotDelete_Fast,     NULL},
    {&MyContactVersionIdentifier,                VT_UI4,    UnspecifiedForm_CanRead_CannotWrite_CannotDelete_Fast,  L"ContactVersionIdentifier"},
};

HRESULT GetSupportedContactProperties(
    __out IPortableDeviceKeyCollection *pKeys)
{
    HRESULT  hr = S_OK;

    if(pKeys == NULL)
    {
        hr = E_POINTER;
        CHECK_HR(hr, "Cannot have NULL collection parameter");
        return hr;
    }

    for (DWORD dwIndex = 0; dwIndex < ARRAYSIZE(g_SupportedContactProperties); dwIndex++)
    {
        hr = pKeys->Add(*g_SupportedContactProperties[dwIndex].pKey);
        CHECK_HR(hr, "Failed to add custom contacts property");
    }

    return hr;
}

HRESULT GetContactPropertyAttributes(
          REFPROPERTYKEY         Key, 
    __out IPortableDeviceValues* pAttributes)
{
    HRESULT hr = S_OK;

    if(pAttributes == NULL)
    {
        hr = E_POINTER;
        CHECK_HR(hr, "Cannot have NULL parameter");
        return hr;
    }

    hr = SetPropertyAttributes(Key, &g_SupportedContactProperties[0], ARRAYSIZE(g_SupportedContactProperties), pAttributes);

    return hr;
}


// For this object, the supported properties are the same as the supported 
// format properties.
// This is where customization for supported properties per object can happen
HRESULT FakeContactContent::GetSupportedProperties(
    __out  IPortableDeviceKeyCollection *pKeys)
{
    return GetSupportedContactProperties(pKeys);
}

HRESULT FakeContactContent::GetValue(
           REFPROPERTYKEY         Key, 
    __out  IPortableDeviceValues* pStore)
{
    HRESULT hr = S_OK;

    PropVariantWrapper pvValue;

    if(pStore == NULL)
    {
        hr = E_POINTER;
        CHECK_HR(hr, "Cannot have NULL parameter");
        return hr;
    }

    if (IsEqualPropertyKey(Key, WPD_OBJECT_ID))
    {
        // Add WPD_OBJECT_ID
        pvValue = ObjectID;
        hr = pStore->SetValue(WPD_OBJECT_ID, &pvValue);
        CHECK_HR(hr, ("Failed to set WPD_OBJECT_ID"));
    }
    else if (IsEqualPropertyKey(Key, WPD_OBJECT_PERSISTENT_UNIQUE_ID))
    {
        // Add WPD_OBJECT_PERSISTENT_UNIQUE_ID
        pvValue = this->PersistentUniqueID;
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
    else if (IsEqualPropertyKey(Key, WPD_OBJECT_NAME))
    {
        // Add WPD_OBJECT_NAME
        pvValue = Name;
        hr = pStore->SetValue(WPD_OBJECT_NAME, &pvValue);
        CHECK_HR(hr, ("Failed to set WPD_OBJECT_NAME"));
    }
    else if (IsEqualPropertyKey(Key, WPD_OBJECT_CONTENT_TYPE))
    {    
        // Add WPD_OBJECT_CONTENT_TYPE
        hr = pStore->SetGuidValue(WPD_OBJECT_CONTENT_TYPE, ContentType);
        CHECK_HR(hr, ("Failed to set WPD_OBJECT_CONTENT_TYPE"));
    }
    else if (IsEqualPropertyKey(Key, WPD_OBJECT_FORMAT))
    {
        // Add WPD_OBJECT_FORMAT
        hr = pStore->SetGuidValue(WPD_OBJECT_FORMAT, Format);
        CHECK_HR(hr, ("Failed to set WPD_OBJECT_FORMAT"));
    }
    else if (IsEqualPropertyKey(Key, WPD_OBJECT_CAN_DELETE))
    {
        // Add WPD_OBJECT_CAN_DELETE
        hr = pStore->SetBoolValue(WPD_OBJECT_CAN_DELETE, CanDelete);
        CHECK_HR(hr, ("Failed to set WPD_OBJECT_CAN_DELETE"));
    }
    else if (IsEqualPropertyKey(Key, WPD_OBJECT_CONTAINER_FUNCTIONAL_OBJECT_ID))
    {
        // Add WPD_OBJECT_CONTAINER_FUNCTIONAL_OBJECT_ID
        hr = pStore->SetStringValue(WPD_OBJECT_CONTAINER_FUNCTIONAL_OBJECT_ID, ContainerFunctionalObjectID);
        CHECK_HR(hr, ("Failed to set WPD_OBJECT_CONTAINER_FUNCTIONAL_OBJECT_ID"));
    }
    else if (IsEqualPropertyKey(Key, WPD_CONTACT_DISPLAY_NAME))
    {
        // Add WPD_CONTACT_DISPLAY_NAME
        pvValue = DisplayName;
        hr = pStore->SetValue(WPD_CONTACT_DISPLAY_NAME, &pvValue);
        CHECK_HR(hr, ("Failed to set WPD_CONTACT_DISPLAY_NAME"));
    }
    else if (IsEqualPropertyKey(Key, WPD_CONTACT_MOBILE_PHONE))
    {
        // Add WPD_CONTACT_MOBILE_NUMBER
        pvValue = MobilePhoneNumber;
        hr = pStore->SetValue(WPD_CONTACT_MOBILE_PHONE, &pvValue);
        CHECK_HR(hr, ("Failed to set WPD_CONTACT_MOBILE_PHONE"));
    }
    else if (IsEqualPropertyKey(Key, WPD_CONTACT_PRIMARY_EMAIL_ADDRESS))
    {
        // Add WPD_CONTACT_PRIMARY_EMAIL
        pvValue = PrimaryEmail;
        hr = pStore->SetValue(WPD_CONTACT_PRIMARY_EMAIL_ADDRESS, &pvValue);
        CHECK_HR(hr, ("Failed to set WPD_CONTACT_PRIMARY_EMAIL_ADDRESS"));
    }
    else if (IsEqualPropertyKey(Key, WPD_CONTACT_PRIMARY_PHONE))
    {
        // Add WPD_CONTACT_PRIMARY_PHONE_NUMBER
        pvValue = PrimaryPhoneNumber;
        hr = pStore->SetValue(WPD_CONTACT_PRIMARY_PHONE, &pvValue);
        CHECK_HR(hr, ("Failed to set WPD_CONTACT_PRIMARY_PHONE"));
    }
    else if (IsEqualPropertyKey(Key, WPD_CONTACT_PERSONAL_FULL_POSTAL_ADDRESS))
    {
        // Add WPD_CONTACT_PERSONAL_FULL_POSTAL_ADDRESS
        pvValue = PersonalFullPostalAddress;
        hr = pStore->SetValue(WPD_CONTACT_PERSONAL_FULL_POSTAL_ADDRESS, &pvValue);
        CHECK_HR(hr, ("Failed to set WPD_CONTACT_PERSONAL_FULL_POSTAL_ADDRESS"));
    }
    else if (IsEqualPropertyKey(Key, MyContactVersionIdentifier))
    {
        // Add MyContactVersionIdentifier
        pvValue = VersionIdentifier;
        hr = pStore->SetValue(MyContactVersionIdentifier, &pvValue);
        CHECK_HR(hr, ("Failed to set MyContactVersionIdentifier"));
    }
    else
    {
        hr = HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
        CHECK_HR(hr, "Property {%ws}.%d is not supported", CComBSTR(Key.fmtid), Key.pid);
    }

    return hr;
}


HRESULT FakeContactContent::WriteValue(
    REFPROPERTYKEY     Key, 
    const PROPVARIANT& Value)
{
    HRESULT             hr      = S_OK;
    PropVariantWrapper  pvValue;

    if(IsEqualPropertyKey(Key, WPD_OBJECT_NAME))
    {
        if(Value.vt == VT_LPWSTR)
        {
            if (lstrlenW(Value.pwszVal) > 0)
            {
                Name = Value.pwszVal;
            }
            else
            {
                hr = E_INVALIDARG;
                CHECK_HR(hr, "Failed to set WPD_OBJECT_NAME because value was an empty string");
            }
        }
        else
        {
            hr = E_INVALIDARG;
            CHECK_HR(hr, "Failed to set WPD_OBJECT_NAME because type was not VT_LPWSTR");
        }
    }
    else if(IsEqualPropertyKey(Key, WPD_CONTACT_DISPLAY_NAME))
    {
        if(Value.vt == VT_LPWSTR)
        {
            DisplayName = Value.pwszVal;
        }
        else
        {
            hr = E_INVALIDARG;
            CHECK_HR(hr, "Failed to set WPD_CONTACT_DISPLAY_NAME because type was not VT_LPWSTR");
        }
    }
    else if(IsEqualPropertyKey(Key, WPD_CONTACT_MOBILE_PHONE))
    {
        if(Value.vt == VT_LPWSTR)
        {
            MobilePhoneNumber = Value.pwszVal;
        }
        else
        {
            hr = E_INVALIDARG;
            CHECK_HR(hr, "Failed to set WPD_CONTACT_MOBILE_PHONE because type was not VT_LPWSTR");
        }
    }
    else if(IsEqualPropertyKey(Key, WPD_CONTACT_PRIMARY_PHONE))
    {
        if(Value.vt == VT_LPWSTR)
        {
            PrimaryPhoneNumber = Value.pwszVal;
        }
        else
        {
            hr = E_INVALIDARG;
            CHECK_HR(hr, "Failed to set WPD_CONTACT_PRIMARY_PHONE because type was not VT_LPWSTR");
        }
    }
    else if(IsEqualPropertyKey(Key, WPD_CONTACT_PRIMARY_EMAIL_ADDRESS))
    {
        if(Value.vt == VT_LPWSTR)
        {
            PrimaryEmail = Value.pwszVal;
        }
        else
        {
            hr = E_INVALIDARG;
            CHECK_HR(hr, "Failed to set WPD_CONTACT_PRIMARY_EMAIL_ADDRESS because type was not VT_LPWSTR");
        }
    }
    else if(IsEqualPropertyKey(Key, WPD_CONTACT_PERSONAL_FULL_POSTAL_ADDRESS))
    {
        if(Value.vt == VT_LPWSTR)
        {
            PersonalFullPostalAddress = Value.pwszVal;
        }
        else
        {
            hr = E_INVALIDARG;
            CHECK_HR(hr, "Failed to set WPD_CONTACT_PERSONAL_FULL_POSTAL_ADDRESS because type was not VT_LPWSTR");
        }
    }
    else
    {
        hr = E_ACCESSDENIED;
        CHECK_HR(hr, "Property %ws.%d on [%ws] does not support set value operation", CComBSTR(Key.fmtid), Key.pid, ObjectID);
    }

    return hr;
}

HRESULT FakeContactContent::GetPropertyAttributes(
            REFPROPERTYKEY          Key,
    __out   IPortableDeviceValues*  pAttributes)
{
    HRESULT hr = S_OK;

    if(pAttributes == NULL)
    {
        hr = E_POINTER;
        CHECK_HR(hr, "Cannot have NULL attributes parameter");
        return hr;
    }

    hr = GetContactPropertyAttributes(Key, pAttributes);
    CHECK_HR(hr, "Failed to add property attributes for %ws.%d", CComBSTR(Key.fmtid), Key.pid);

    // Some of our properties have extra attributes on top of the ones that are common to all
    if(IsEqualPropertyKey(Key, WPD_OBJECT_NAME))
    {
        CAtlStringW strDefaultName;

        strDefaultName.Format(L"%ws%ws", L"Name", ObjectID);

        hr = pAttributes->SetStringValue(WPD_PROPERTY_ATTRIBUTE_DEFAULT_VALUE, strDefaultName.GetString());;
        CHECK_HR(hr, "Failed to set WPD_PROPERTY_ATTRIBUTE_DEFAULT_VALUE");
    }

    return hr;
}

HRESULT FakeContactContent::WriteValues(
    __in    IPortableDeviceValues* pValues,
    __out   IPortableDeviceValues* pResults,
    __out   bool*                  pbObjectChanged)
{
    HRESULT hr = FakeContent::WriteValues(pValues, pResults, pbObjectChanged);

    if (SUCCEEDED(hr) && (*pbObjectChanged == true))
    {
        UpdateVersion();
    }

    return hr;
}

void FakeContactContent::UpdateVersion()
{
    if (VersionIdentifier < ULONG_MAX)
    {
        VersionIdentifier++;
    }
    else
    {
        VersionIdentifier = 0;
    }
}
