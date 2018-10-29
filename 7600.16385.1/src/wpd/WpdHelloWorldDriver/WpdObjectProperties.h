#pragma once

#define DEVICE_PROTOCOL_VALUE                             L"Hello World Protocol ver 1.00"
#define DEVICE_FIRMWARE_VERSION_VALUE                     L"1.0.0.0"
#define DEVICE_POWER_LEVEL_VALUE                          100
#define DEVICE_MODEL_VALUE                                L"Hello World!"
#define DEVICE_FRIENDLY_NAME_VALUE                        L"Hello World!"
#define DEVICE_MANUFACTURER_VALUE                         L"Windows Portable Devices Group"
#define DEVICE_SERIAL_NUMBER_VALUE                        L"01234567890123-45676890123456"
#define DEVICE_SUPPORTS_NONCONSUMABLE_VALUE               TRUE

#define STORAGE_OBJECT_ID                                 L"123ABC"
#define STORAGE_CAPACITY_VALUE                            1024 * 1024
#define STORAGE_FREE_SPACE_IN_BYTES_VALUE                 STORAGE_CAPACITY_VALUE
#define STORAGE_SERIAL_NUMBER_VALUE                       L"98765432109876-54321098765432"
#define STORAGE_OBJECT_NAME_VALUE                         L"Internal Memory"
#define STORAGE_FILE_SYSTEM_TYPE_VALUE                    L"FAT32"
#define STORAGE_DESCRIPTION_VALUE                         L"Hello World! Memory Storage System"

#define DOCUMENTS_FOLDER_OBJECT_ID                        L"XYZ456"
#define DOCUMENTS_FOLDER_OBJECT_NAME_VALUE                L"Documents Folder"
#define DOCUMENTS_FOLDER_OBJECT_ORIGINAL_FILE_NAME_VALUE  L"Documents"

#define README_FILE_OBJECT_ID                             L"6543210"
#define README_FILE_OBJECT_NAME_VALUE                     L"Sample ReadMe Text File"
#define README_FILE_OBJECT_ORIGINAL_FILE_NAME_VALUE       L"ReadMe.txt"
#define README_FILE_OBJECT_CONTENTS                       "Hello World!\r\nThis is a text file transferred from the WPD Hello World sample driver.\r\n"

ULONGLONG GetObjectSize(CAtlStringW strObjectID);
GUID GetObjectFormat(CAtlStringW strObjectID);
GUID GetObjectContentType(CAtlStringW strObjectID);
HRESULT AddSupportedPropertyKeys(LPCWSTR                        wszObjectID,
                                 IPortableDeviceKeyCollection*  pKeys);

VOID AddCommonPropertyKeys(IPortableDeviceKeyCollection* pKeys);
VOID AddDevicePropertyKeys(IPortableDeviceKeyCollection* pKeys);
VOID AddStoragePropertyKeys(IPortableDeviceKeyCollection* pKeys);
VOID AddFilePropertyKeys(IPortableDeviceKeyCollection* pKeys);
VOID AddFolderPropertyKeys(IPortableDeviceKeyCollection* pKeys);

class WpdObjectProperties
{
public:
    WpdObjectProperties();
    virtual ~WpdObjectProperties();

    HRESULT Initialize();

    HRESULT DispatchWpdMessage(const PROPERTYKEY&     Command,
                               IPortableDeviceValues* pParams,
                               IPortableDeviceValues* pResults);

    HRESULT OnGetSupportedProperties(IPortableDeviceValues*  pParams,
                                     IPortableDeviceValues*  pResults);

    HRESULT OnGetPropertyValues(IPortableDeviceValues*  pParams,
                                IPortableDeviceValues*  pResults);

    HRESULT OnGetAllPropertyValues(IPortableDeviceValues*  pParams,
                                   IPortableDeviceValues*  pResults);

    HRESULT OnSetPropertyValues(IPortableDeviceValues*  pParams,
                                IPortableDeviceValues*  pResults);

    HRESULT OnGetPropertyAttributes(IPortableDeviceValues*  pParams,
                                    IPortableDeviceValues*  pResults);

    HRESULT OnDeleteProperties(IPortableDeviceValues*  pParams,
                               IPortableDeviceValues*  pResults);

private:

    HRESULT GetPropertyValuesForObject(LPCWSTR                        wszObjectID,
                                       IPortableDeviceKeyCollection*  pKeys,
                                       IPortableDeviceValues*         pValues);

    HRESULT GetPropertyAttributesForObject(LPCWSTR                wszObjectID,
                                           const PROPERTYKEY&     Key,
                                           IPortableDeviceValues* pAttributes);
};

