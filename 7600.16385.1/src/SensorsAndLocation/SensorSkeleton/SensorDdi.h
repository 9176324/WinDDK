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

class CSensorDDI : 
    public CComObjectRoot,
    public ISensorDriver
{
public:
    CSensorDDI();
    virtual ~CSensorDDI();

    DECLARE_NOT_AGGREGATABLE(CSensorDDI)

    BEGIN_COM_MAP(CSensorDDI)
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
        __out IPortableDeviceKeyCollection** ppKeys
        );

    HRESULT STDMETHODCALLTYPE OnGetSupportedDataFields(
        __in  LPWSTR pwszObjectID,
        __out IPortableDeviceKeyCollection** ppKeys
        );

    HRESULT STDMETHODCALLTYPE OnGetSupportedEvents(
        __in  LPWSTR pwszObjectID,
        __out GUID** ppSupportedEvents,
        __out ULONG* pulEventCount
        );

    HRESULT STDMETHODCALLTYPE OnGetProperties(
        __in  IWDFFile* pClientFile,
        __in  LPWSTR pwszObjectID,
        __in  IPortableDeviceKeyCollection* pKeys,
        __out IPortableDeviceValues** ppValues
        );

    HRESULT STDMETHODCALLTYPE OnGetDataFields(
        __in  IWDFFile* pClientFile,
        __in  LPWSTR pwszObjectID,
        __in  IPortableDeviceKeyCollection* pKeys,
        __out IPortableDeviceValues** ppValues
        );

    HRESULT STDMETHODCALLTYPE OnSetProperties(
        __in  IWDFFile* pClientFile,
        __in  LPWSTR pwszObjectID,
        __in  IPortableDeviceValues* pValues,
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
        __in IUnknown* pPortableDeviceValuesParamsUnknown,
        __in IUnknown* pPortableDeviceValuesResultsUnknown
        );

    // Non-interface methods
public:

    HRESULT Initialize(IWDFDevice* pDevice);

// Private data members.
private:
    
    // PropertyKeys
    CComPtr<IPortableDeviceKeyCollection>   m_pSupportedTemperatureSensorProperties;
    CComPtr<IPortableDeviceKeyCollection>   m_pSupportedGpsSensorProperties;
    CComPtr<IPortableDeviceKeyCollection>   m_pSupportedTemperatureSensorDataFields;
    CComPtr<IPortableDeviceKeyCollection>   m_pSupportedGpsSensorDataFields;
    
    // Property Values
    CComPtr<IPortableDeviceValues>          m_pTemperatureSensorPropertyValues;
    CComPtr<IPortableDeviceValues>          m_pGpsSensorPropertyValues;
    CComPtr<IPortableDeviceValues>          m_pTemperatureSensorDataFieldValues;
    CComPtr<IPortableDeviceValues>          m_pGpsSensorDataFieldValues;

private:
    HRESULT InitializeTemperatureSensor(IWDFDevice* pDevice);
    HRESULT InitializeGpsSensor(IWDFDevice* pDevice);
};


