#pragma once

/**
 * This class represents an abstraction of a real device.
 * Driver implementors should replace this with their own
 * device I/O classes/libraries.
 */

class FakeDevice
{
public:
    FakeDevice() : m_dwLastObjectID(0)
    {
    }

    ~FakeDevice()
    {
    }

    HRESULT InitializeContent();

    FakeContactsService* GetContactsService();

    ACCESS_SCOPE GetAccessScope(
        __out   IPortableDeviceValues*                pParams);

    // Device Capabilities
    // These are legacy commands that apply to the whole device, no access scope is required
    HRESULT GetSupportedCommands(
        __out   IPortableDeviceKeyCollection*         pCommands);
    
    HRESULT GetCommandOptions(
                REFPROPERTYKEY                        Command,
        __out   IPortableDeviceValues*                pOptions);

    HRESULT GetSupportedFunctionalCategories(
        __out   IPortableDevicePropVariantCollection* pFunctionalCategories);
   
    HRESULT GetFunctionalObjects(
                REFGUID                               guidFunctionalCategory,
        __out   IPortableDevicePropVariantCollection* pFunctionalObjects);
   
    HRESULT GetSupportedContentTypes(
                REFGUID                               guidFunctionalCategory,
        __out   IPortableDevicePropVariantCollection* pContentTypes);
   
    HRESULT GetSupportedFormats(
                REFGUID                               guidContentType,
        __out   IPortableDevicePropVariantCollection* pFormats);
    
    HRESULT GetSupportedFormatProperties(
                REFGUID                               guidObjectFormat, 
        __out   IPortableDeviceKeyCollection*         pKeys);
    
    HRESULT GetFixedPropertyAttributes(
                REFGUID                               guidObjectFormat, 
                REFPROPERTYKEY                        Key, 
        __out   IPortableDeviceValues* pAttributes);
    
    HRESULT GetSupportedEvents(
        __out   IPortableDevicePropVariantCollection* pEvents);
    
    HRESULT GetEventOptions(
        __out   IPortableDeviceValues*                pOptions);

    // Enumeration
    // Depending on the access scope, the driver can display only objects within the current
    // scoped hierarchy tree
    void InitializeEnumerationContext(
                ACCESS_SCOPE                          Scope, 
        __in    LPCWSTR                               wszParentID, 
        __out   WpdObjectEnumeratorContext*           pEnumContext);
  
    HRESULT FindNext(
                const DWORD                           dwNumObjectsRequested, 
        __in    WpdObjectEnumeratorContext*           pEnumContext, 
        __out   IPortableDevicePropVariantCollection* pObjectIDCollection, 
        __out   DWORD*                                pdwNumObjectsEnumerated);

    HRESULT GetObjectIDsByFormat(
                ACCESS_SCOPE                          Scope,
                REFGUID                               guidObjectFormat,
        __in    LPCWSTR                               wszParentObjectID,
                const DWORD                           dwDepth,
        __out   IPortableDevicePropVariantCollection* pObjectIDs);

    HRESULT GetObjectIDsFromPersistentUniqueIDs(
                ACCESS_SCOPE                          Scope,
        __in    IPortableDevicePropVariantCollection* pPersistentIDs,
        __out   IPortableDevicePropVariantCollection* pObjectIDs);

    // Property Management
    // Depending on the access scope, the driver can allow access to properties of objects within the current
    // scoped hierarchy tree
    HRESULT GetSupportedProperties(
                ACCESS_SCOPE                          Scope, 
        __in    LPCWSTR                               wszObjectID, 
        __out   IPortableDeviceKeyCollection*         pKeys);

    HRESULT GetAllPropertyValues(
                ACCESS_SCOPE                          Scope,
        __in    LPCWSTR                               wszObjectID,
        __out   IPortableDeviceValues*                pValues);

    HRESULT GetPropertyValues(
                ACCESS_SCOPE                          Scope,
        __in    LPCWSTR                               wszObjectID,
        __in    IPortableDeviceKeyCollection*         pKeys,
        __out   IPortableDeviceValues*                pValues);

    HRESULT SetPropertyValues(
                ACCESS_SCOPE                          Scope,
        __in    LPCWSTR                               wszObjectID,
        __in    IPortableDeviceValues*                pValues,
        __out   IPortableDeviceValues*                pResults, 
        __out   IPortableDeviceValues*                pEventParams,
        __out   bool*                                 pbObjectChanged);

    HRESULT GetPropertyAtributes(
                ACCESS_SCOPE                          Scope,
        __in    LPCWSTR                               wszObjectID,
                REFPROPERTYKEY                        Key,
        __out   IPortableDeviceValues*                pAttributes);

    // Object Management
    // Depending on the access scope, the driver can limit access only to objects within the current
    // scoped hierarchy tree
    HRESULT CreatePropertiesOnlyObject(
                        ACCESS_SCOPE                      Scope,
        __in            IPortableDeviceValues*            pObjectProperties,
        __deref_out_opt LPWSTR*                           ppszNewObjectID,
        __out           IPortableDeviceValues*            pEventParams);

    HRESULT DeleteObject(
                ACCESS_SCOPE                          Scope,
                const DWORD                           dwDeleteOptions,
        __in    LPCWSTR                               wszObjectID,
        __out   IPortableDeviceValues*                pEventParams);

    // Resources
    HRESULT GetSupportedResources(
                ACCESS_SCOPE                          Scope,
        __in    LPCWSTR                               wszObjectID,
        __out   IPortableDeviceKeyCollection*         pResources);

    HRESULT GetResourceAttributes(
                ACCESS_SCOPE                          Scope,
        __in    LPCWSTR                               wszObjectID,
                REFPROPERTYKEY                        Resource,
        __out   IPortableDeviceValues*                pAttributes);
    
    HRESULT OpenResource(
                ACCESS_SCOPE                          Scope,
        __in    LPCWSTR                               wszObjectID,
                REFPROPERTYKEY                        Resource,
                const DWORD                           dwMode,
        __out   WpdObjectResourceContext*             pResourceContext); 

    HRESULT ReadResourceData(
        __in                            WpdObjectResourceContext*   pResourceContext,
        __out_bcount(dwNumBytesToRead)  BYTE*                       pBuffer,
                                        const DWORD                 dwNumBytesToRead,
        __out                           DWORD*                      pdwNumBytesRead);

private:
    HRESULT GetContent(
                ACCESS_SCOPE                          Scope,
        __in    LPCWSTR                               wszObjectID,
        __out   FakeContent**                         ppContent);

    HRESULT RemoveObjectsMarkedForDeletion();

private:

    // Simulates content on the device
    FakeDeviceContent   m_DeviceContent;

    // Simulates contacts service functionality
    FakeContactsService m_ContactsService;

    DWORD               m_dwLastObjectID;
};

