#pragma once

#define DEVICE_PROTOCOL_VALUE                             L"Contacts Services Sample Protocol ver 1.00"
#define DEVICE_FIRMWARE_VERSION_VALUE                     L"1.0.0.0"
#define DEVICE_POWER_LEVEL_VALUE                          100
#define DEVICE_MODEL_VALUE                                L"Contacts Service Device 2000"
#define DEVICE_FRIENDLY_NAME_VALUE                        L"Sample Device"
#define DEVICE_MANUFACTURER_VALUE                         L"Windows Portable Devices Group"
#define DEVICE_SERIAL_NUMBER_VALUE                        L"01234567890123-45676890123456"
#define DEVICE_SUPPORTS_NONCONSUMABLE_VALUE               FALSE

class FakeDeviceContent : public FakeContent
{
public:
    FakeDeviceContent()
    {
        ObjectID                    = WPD_DEVICE_OBJECT_ID;
        PersistentUniqueID          = WPD_DEVICE_OBJECT_ID;
        ParentID                    = L"";
        ParentPersistentUniqueID    = L"";
        ContainerFunctionalObjectID = L"";
        Name                        = WPD_DEVICE_OBJECT_ID;
        Protocol                    = DEVICE_PROTOCOL_VALUE;
        FirmwareVersion             = DEVICE_FIRMWARE_VERSION_VALUE;
        Model                       = DEVICE_MODEL_VALUE;
        Manufacturer                = DEVICE_MANUFACTURER_VALUE;
        FriendlyName                = DEVICE_FRIENDLY_NAME_VALUE;
        SerialNumber                = DEVICE_SERIAL_NUMBER_VALUE;
        PowerLevel                  = DEVICE_POWER_LEVEL_VALUE;
        PowerSource                 = WPD_POWER_SOURCE_EXTERNAL;
        DeviceType                  = WPD_DEVICE_TYPE_GENERIC;
        Format                      = WPD_OBJECT_FORMAT_UNSPECIFIED;
        ContentType                 = WPD_CONTENT_TYPE_FUNCTIONAL_OBJECT;
        FunctionalCategory          = WPD_FUNCTIONAL_CATEGORY_DEVICE;
        RequiredScope               = CONTACTS_SERVICE_ACCESS;
        SupportsNonConsumable       = DEVICE_SUPPORTS_NONCONSUMABLE_VALUE;
    }

    virtual ~FakeDeviceContent()
    {
    }

    FakeDeviceContent(const FakeContent& src)
    {
        *this = src;
    }

    virtual HRESULT FakeDeviceContent::InitializeContent(
        __inout DWORD *pdwLastObjectID);

    virtual HRESULT InitializeEnumerationContext(
                ACCESS_SCOPE                Scope,
        __out   WpdObjectEnumeratorContext* pEnumeratorContext);

    // Property Management
    virtual HRESULT GetSupportedProperties(
        __out   IPortableDeviceKeyCollection*         pKeys);

    virtual HRESULT GetValue(
                REFPROPERTYKEY                        Key, 
        __out   IPortableDeviceValues*                pStore);

    // Resources
    virtual HRESULT GetSupportedResources(
        __out   IPortableDeviceKeyCollection*         pResources);

    virtual HRESULT GetResourceAttributes(
                REFPROPERTYKEY                        Resource,
        __out   IPortableDeviceValues*                pAttributes);
    
    virtual HRESULT OpenResource(
                REFPROPERTYKEY                        Resource,
                const DWORD                           dwMode,
        __out   WpdObjectResourceContext*             pResourceContext); 

    virtual HRESULT ReadResourceData(
        __in                            WpdObjectResourceContext*   pResourceContext,
        __out_bcount(dwNumBytesToRead)  BYTE*                       pBuffer,
                                        const DWORD                 dwNumBytesToRead,
        __out                           DWORD*                      pdwNumBytesRead);

public:
    CAtlStringW  Protocol;
    CAtlStringW  FirmwareVersion;
    CAtlStringW  Model;
    CAtlStringW  FriendlyName;
    CAtlStringW  SerialNumber;
    CAtlStringW  Manufacturer;

    GUID         FunctionalCategory;
    BOOL         SupportsNonConsumable;
    DWORD        PowerLevel;
    DWORD        PowerSource;
    DWORD        DeviceType;
};

