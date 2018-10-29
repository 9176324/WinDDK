#pragma once

/**
 * This class represents an abstraction of a contacts service that implements 
 * the full enumeration sync model.
 * Driver implementors should replace this with their own
 * device I/O classes/libraries.
 */

class FakeContactsService
{
public:
    FakeContactsService() : RequestFilename(CONTACTS_SERVICE_OBJECT_ID)
    {
    }

    ~FakeContactsService()
    {
    }

    // Capabilities
    HRESULT GetSupportedCommands(
        __out   IPortableDeviceKeyCollection*         pCommands);
    
    HRESULT GetCommandOptions(
                REFPROPERTYKEY                        Command,
        __out   IPortableDeviceValues*                pOptions);

    HRESULT GetSupportedMethods(
        __out   IPortableDevicePropVariantCollection* pMethods);

    BOOL IsMethodSupported(
                REFGUID Method);

    HRESULT GetSupportedMethodsByFormat(
                REFGUID                               Format, 
        __out   IPortableDevicePropVariantCollection* pMethods);

    HRESULT GetMethodAttributes(
                REFGUID                               Method, 
        __out   IPortableDeviceValues*                pAttributes);

    HRESULT GetMethodParameterAttributes(
                REFPROPERTYKEY                        Parameter, 
        __out   IPortableDeviceValues*                pAttributes);

    HRESULT GetSupportedFormats(
        __out   IPortableDevicePropVariantCollection* pFormats);

    HRESULT GetFormatAttributes(
                REFGUID                               Format, 
        __out   IPortableDeviceValues*                pAttributes);

    HRESULT GetSupportedFormatProperties(
                REFGUID                               Format, 
        __out   IPortableDeviceKeyCollection*         pKeys);

    HRESULT GetPropertyAttributes(
                REFGUID                               Format,
                REFPROPERTYKEY                        Property,
        __out   IPortableDeviceValues*                pAttributes);

    HRESULT GetSupportedEvents(
        __out   IPortableDevicePropVariantCollection* pEvents);

    HRESULT GetEventAttributes(
                REFGUID                               Event, 
        __out   IPortableDeviceValues*                pAttributes);

    HRESULT GetEventParameterAttributes(
                REFPROPERTYKEY                        Parameter, 
        __out   IPortableDeviceValues*                pAttributes);

    HRESULT GetInheritedServices(
                const DWORD                           dwInheritanceType, 
        __out   IPortableDevicePropVariantCollection* pServices);

    // Methods
    HRESULT OnBeginSync(
        __in    IPortableDeviceValues*                pParams, 
        __out   IPortableDeviceValues*                pResults);

    HRESULT OnEndSync(
        __in    IPortableDeviceValues*                pParams, 
        __out   IPortableDeviceValues*                pResults);

    HRESULT OnMyCustomMethod(
        __in    IPortableDeviceValues*                pParams, 
        __out   IPortableDeviceValues*                pResults,
        __out   IPortableDeviceValues*                pEventParameters);

    LPWSTR GetRequestFilename()
    {
        return RequestFilename.GetBuffer();
    }

private:
    CAtlStringW RequestFilename;
};



