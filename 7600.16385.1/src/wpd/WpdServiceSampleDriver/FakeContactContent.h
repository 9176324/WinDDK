#pragma once

/**
 * This class represents an abstraction of a contact content object
 * Driver implementors should replace this with their own
 * device I/O classes/libraries.
 */

class FakeContactContent : public FakeContent
{
public:
    FakeContactContent()
    {
        Format              = WPD_OBJECT_FORMAT_ABSTRACT_CONTACT;
        ContentType         = WPD_CONTENT_TYPE_CONTACT;
        RequiredScope       = CONTACTS_SERVICE_ACCESS;
        CanDelete           = true;
        VersionIdentifier   = 0;
    }

    FakeContactContent(const FakeContactContent& src)
    {
        *this = src;
    }

    ~FakeContactContent()
    {
    }

    FakeContactContent& operator= (const FakeContactContent& src)
    {
        DisplayName                 = src.DisplayName;
        MobilePhoneNumber           = src.MobilePhoneNumber;
        PrimaryPhoneNumber          = src.PrimaryPhoneNumber;
        PrimaryEmail                = src.PrimaryEmail;
        PersonalFullPostalAddress   = src.PersonalFullPostalAddress;
        VersionIdentifier           = src.VersionIdentifier;
        
        return *this;
    }

    HRESULT GetValue(
              REFPROPERTYKEY         Key, 
        __out IPortableDeviceValues* pStore);

    HRESULT WriteValue(
              REFPROPERTYKEY         Key, 
              const PROPVARIANT&     Value);

    HRESULT GetPropertyAttributes(
              REFPROPERTYKEY         Key, 
        __out IPortableDeviceValues* pAttributes);

    HRESULT GetSupportedProperties(
        __out IPortableDeviceKeyCollection* pKeys);
  
    HRESULT WriteValues(
        __in    IPortableDeviceValues* pValues,
        __out   IPortableDeviceValues* pResults,
        __out   bool*                  pbObjectChanged);

private:
    void UpdateVersion();

public:    
    // Custom properties defined by the contacts service
    CAtlStringW             DisplayName;
    CAtlStringW             MobilePhoneNumber;
    CAtlStringW             PrimaryPhoneNumber;
    CAtlStringW             PrimaryEmail;
    CAtlStringW             PersonalFullPostalAddress;

private:
    // Indicates whether the object has been updated
    DWORD                   VersionIdentifier;
};

HRESULT GetSupportedContactProperties(
    __out IPortableDeviceKeyCollection* pKeys);

HRESULT GetContactPropertyAttributes(
          REFPROPERTYKEY                Key, 
    __out IPortableDeviceValues*        pAttributes);

