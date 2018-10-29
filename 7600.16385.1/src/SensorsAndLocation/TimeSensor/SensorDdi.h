/*++

// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.

Copyright (C) Microsoft Corporation, All Rights Reserved

Module Name:

    SensorDdi.h

Abstract:

    This module contains the type definitions for the 
    ISensorDriver interface which is used by the Sensor Class Extension.
--*/

#pragma once
#include "DeviceConst.h"

class CSampleEvents; // forward decl

class CSensorDdi : 
    public CComObjectRoot,
    public ISensorDriver
{
public:
    CSensorDdi();
    virtual ~CSensorDdi();

    DECLARE_NOT_AGGREGATABLE(CSensorDdi)

    BEGIN_COM_MAP(CSensorDdi)
        COM_INTERFACE_ENTRY(ISensorDriver)
    END_COM_MAP()

//
// COM Interface methods
//
public:

    // ISensorDriver methods.
    HRESULT STDMETHODCALLTYPE OnGetSupportedSensorObjects(
        __out IPortableDeviceValuesCollection** ppSensorObjectCollection
        );

    HRESULT STDMETHODCALLTYPE OnGetSupportedProperties(
        __in  LPWSTR pwszObjectID,
        __out IPortableDeviceKeyCollection** ppSupportedProperties
        );

    HRESULT STDMETHODCALLTYPE OnGetSupportedDataFields(
        __in  LPWSTR pwszObjectID,
        __out IPortableDeviceKeyCollection** ppSupportedDataFields
        );

    HRESULT STDMETHODCALLTYPE OnGetSupportedEvents(
        __in  LPWSTR pwszObjectID,
        __out GUID** ppSupportedEvents,
        __out ULONG* pulEventCount
        );

    HRESULT STDMETHODCALLTYPE OnGetProperties(
        __in  IWDFFile* pClientFile,
        __in  LPWSTR pwszObjectID,
        __in  IPortableDeviceKeyCollection* pProperties,
        __out IPortableDeviceValues** ppPropertyValues
        );

    HRESULT STDMETHODCALLTYPE OnGetDataFields(
        __in  IWDFFile* pClientFile,
        __in  LPWSTR pwszObjectID,
        __in  IPortableDeviceKeyCollection* pDataFields,
        __out IPortableDeviceValues** ppDataValues
        );

    HRESULT STDMETHODCALLTYPE OnSetProperties(
        __in  IWDFFile* pClientFile,
        __in  LPWSTR pwszObjectID,
        __in  IPortableDeviceValues* pPropertiesToSet,
        __out IPortableDeviceValues** ppResults
        );

    HRESULT STDMETHODCALLTYPE OnClientConnect(
        __in IWDFFile* pClientFile,
        __in LPWSTR pwszObjectID
        );

    HRESULT STDMETHODCALLTYPE OnClientDisconnect(
        __in IWDFFile* pClientFile,
        __in LPWSTR pwszObjectID
        );

    HRESULT STDMETHODCALLTYPE OnClientSubscribeToEvents(
        __in IWDFFile* pClientFile,
        __in LPWSTR pwszObjectID
        );

    HRESULT STDMETHODCALLTYPE OnClientUnsubscribeFromEvents(
        __in IWDFFile* pClientFile,
        __in LPWSTR pwszObjectID
        );

    HRESULT STDMETHODCALLTYPE OnProcessWpdMessage(
        __in IUnknown* pUnkPortableDeviceValuesParams,
        __in IUnknown* pUnkPortableDeviceValuesResults
        );

    HRESULT SetSensorClassXT(ISensorClassExtension *pCXT);
    HRESULT SetPUID(GUID puid);
    HRESULT GetSensorState(SensorState* pState);
    BOOL    UpdateSensorState(SensorState state);
    
private:
    // Private data members.

    ULONG m_ulReportInterval;
    CSampleEvents* m_pSampleEvents;
    DWORD m_cEventClients;
    CComPtr<ISensorClassExtension> m_spSensorCXT;
    GUID m_guidPUID;
    SensorState m_state;

    // Struct to keep track of connected client status.
    struct ClientData
    {
        BOOL bListening;  // Client is listening to events.
        ULONG ulInterval;  // Interval requested by client.
    };

    // ATL CSimpleMap class to store 
    // an array of connected clients and their report intervals.
    // This class provides a very simple hash table implementation.
    // For better performance, you may consider using a sorted list if you anticipate 
    // many connected clients or frequent updates to the report interval value.
    CSimpleMap<IWDFFile*, ClientData*> Clients;


    // Private methods.

    // A function that simulates retrieving data from the device hardware.
    void    GetNewDataFromDevice(WORD* wHour, WORD* wMinute, WORD* wSecond, SYSTEMTIME* timestamp);
    
    // Helper functions to get property value. These helpers simply keep 
    // the number of lines of code in the function more manageable.
    HRESULT GetDeviceProperty(PROPERTYKEY Key, IPortableDeviceValues* pValues);
    HRESULT GetSensorProperty(PROPERTYKEY Key, IPortableDeviceValues* pValues);

    // Chooses a new report interval from the list of connected clients.
    ULONG   GetNewReportInterval();
};
