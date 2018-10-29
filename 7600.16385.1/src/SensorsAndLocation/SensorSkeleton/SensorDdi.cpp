/*++
 
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.

Copyright (C) Microsoft Corporation, All Rights Reserved

Module Name:

    SensorDdi.cpp

Abstract:

      This module implements the ISensorDriver interface which is used
      by the Sensor Class Extension.
--*/

#include "internal.h"
#include "Defines.h"
#include "SensorDdi.h"

#include "sensorddi.tmh"

const PROPERTYKEY g_SupportedCommonProperties[] =
{
    WPD_OBJECT_ID,
    WPD_OBJECT_PERSISTENT_UNIQUE_ID,
    WPD_OBJECT_PARENT_ID,
    WPD_OBJECT_NAME,
    WPD_OBJECT_FORMAT,
    WPD_OBJECT_CONTENT_TYPE,
    WPD_OBJECT_CAN_DELETE,
};

const PROPERTYKEY g_SupportedTemperatureProperties[] =
{
    SENSOR_PROPERTY_TYPE,
    SENSOR_PROPERTY_STATE,
    SENSOR_PROPERTY_MIN_REPORT_INTERVAL,
    SENSOR_PROPERTY_CURRENT_REPORT_INTERVAL,
    SENSOR_PROPERTY_PERSISTENT_UNIQUE_ID,
    SENSOR_PROPERTY_MANUFACTURER, 
    SENSOR_PROPERTY_MODEL,
    SENSOR_PROPERTY_SERIAL_NUMBER,
    SENSOR_PROPERTY_FRIENDLY_NAME,
    SENSOR_PROPERTY_DESCRIPTION, 
    SENSOR_PROPERTY_CONNECTION_TYPE,
    WPD_FUNCTIONAL_OBJECT_CATEGORY,
};

const PROPERTYKEY g_SupportedGpsProperties[] =
{
    SENSOR_PROPERTY_TYPE,
    SENSOR_PROPERTY_STATE,
    SENSOR_PROPERTY_MIN_REPORT_INTERVAL,
    SENSOR_PROPERTY_CURRENT_REPORT_INTERVAL,
    SENSOR_PROPERTY_PERSISTENT_UNIQUE_ID,
    SENSOR_PROPERTY_MANUFACTURER, 
    SENSOR_PROPERTY_MODEL,
    SENSOR_PROPERTY_SERIAL_NUMBER,
    SENSOR_PROPERTY_FRIENDLY_NAME,
    SENSOR_PROPERTY_DESCRIPTION, 
    SENSOR_PROPERTY_CONNECTION_TYPE,
    WPD_FUNCTIONAL_OBJECT_CATEGORY,
};

const PROPERTYKEY g_SupportedTemperatureDataFields[] =
{
    SENSOR_DATA_TYPE_TIMESTAMP,
    SENSOR_DATA_TYPE_TEMPERATURE_CELSIUS,
};

const PROPERTYKEY g_SupportedGpsDataFields[] =
{
    SENSOR_DATA_TYPE_TIMESTAMP,
    SENSOR_DATA_TYPE_LATITUDE_DEGREES,
    SENSOR_DATA_TYPE_LONGITUDE_DEGREES,
    SENSOR_DATA_TYPE_ALTITUDE_SEALEVEL_METERS,
    SENSOR_DATA_TYPE_ERROR_RADIUS_METERS,
};

// *************************************************************************
//
// CSensorDDI class
//
// ***************************************************************************



/////////////////////////////////////////////////////////////////////////
//
// CSensorDDI()
//
// Constructor.
//
/////////////////////////////////////////////////////////////////////////
CSensorDDI::CSensorDDI()
{

}

/////////////////////////////////////////////////////////////////////////
//
// ~CSensorDDI()
//
// Destructor
//
/////////////////////////////////////////////////////////////////////////
CSensorDDI::~CSensorDDI()
{

}

/////////////////////////////////////////////////////////////////////////
//
// Initialize()
//
// Initialize function that will set up all the propertykeys
//
/////////////////////////////////////////////////////////////////////////
HRESULT CSensorDDI::Initialize(IWDFDevice* pDevice)
{
    HRESULT hr = (NULL == pDevice) ? E_INVALIDARG : S_OK;

    if (SUCCEEDED(hr))
    {
        // Initialize the Temperature Sensor
        hr = InitializeTemperatureSensor(pDevice);

        if (SUCCEEDED(hr))
        {
            // Initialize the Gps Sensor
            hr = InitializeGpsSensor(pDevice);
        }
    }

    return hr;
}

/////////////////////////////////////////////////////////////////////////
//
// InitializeTemperatureSensor()
//
// Initialize function for Temperature sensor
//
/////////////////////////////////////////////////////////////////////////
HRESULT CSensorDDI::InitializeTemperatureSensor(IWDFDevice* pDevice)
{
    // Initialize the Temperature Sensor Property Keys
    HRESULT hr = m_pSupportedTemperatureSensorProperties.CoCreateInstance(CLSID_PortableDeviceKeyCollection);
    if (SUCCEEDED(hr))
    {
        // Add Common PROPERTYKEYs for ALL WPD objects
        for (DWORD dwIndex=0; dwIndex<ARRAYSIZE(g_SupportedCommonProperties); dwIndex++)
        {
            m_pSupportedTemperatureSensorProperties->Add(g_SupportedCommonProperties[dwIndex]);
        }

        // Add temperature sensor property keys
        for (DWORD dwIndex=0; dwIndex<ARRAYSIZE(g_SupportedTemperatureProperties); dwIndex++)
        {
            m_pSupportedTemperatureSensorProperties->Add(g_SupportedTemperatureProperties[dwIndex]);
        }
    }
    
    // Initialize the Temperature Sensor Property Values
    if (SUCCEEDED(hr))
    {
        hr = m_pTemperatureSensorPropertyValues.CoCreateInstance(CLSID_PortableDeviceValues);
        if (SUCCEEDED(hr))
        {
            // WPD Supported Properties
            m_pTemperatureSensorPropertyValues->SetStringValue(WPD_OBJECT_ID, SENSOR_TEMPERATURE_ID);
            m_pTemperatureSensorPropertyValues->SetStringValue(WPD_OBJECT_NAME, SENSOR_TEMPERATURE_ID);
            m_pTemperatureSensorPropertyValues->SetStringValue(WPD_OBJECT_PERSISTENT_UNIQUE_ID, SENSOR_TEMPERATURE_ID);
            m_pTemperatureSensorPropertyValues->SetStringValue(WPD_OBJECT_PARENT_ID, WPD_DEVICE_OBJECT_ID);
            m_pTemperatureSensorPropertyValues->SetGuidValue(WPD_OBJECT_FORMAT, WPD_OBJECT_FORMAT_UNSPECIFIED);
            m_pTemperatureSensorPropertyValues->SetGuidValue(WPD_OBJECT_CONTENT_TYPE, WPD_CONTENT_TYPE_FUNCTIONAL_OBJECT);
            m_pTemperatureSensorPropertyValues->SetBoolValue(WPD_OBJECT_CAN_DELETE, FALSE);

            // Sensor Supported Properties
            m_pTemperatureSensorPropertyValues->SetGuidValue(WPD_FUNCTIONAL_OBJECT_CATEGORY, SENSOR_CATEGORY_ENVIRONMENTAL);
            m_pTemperatureSensorPropertyValues->SetGuidValue(SENSOR_PROPERTY_TYPE, SENSOR_TYPE_ENVIRONMENTAL_TEMPERATURE);
            m_pTemperatureSensorPropertyValues->SetStringValue(SENSOR_PROPERTY_MANUFACTURER, DEVICE_MANUFACTURER_VALUE);
            m_pTemperatureSensorPropertyValues->SetStringValue(SENSOR_PROPERTY_MODEL, DEVICE_MODEL_VALUE);
            m_pTemperatureSensorPropertyValues->SetStringValue(SENSOR_PROPERTY_SERIAL_NUMBER, DEVICE_SERIAL_NUMBER_VALUE);
            m_pTemperatureSensorPropertyValues->SetStringValue(SENSOR_PROPERTY_FRIENDLY_NAME, SENSOR_TEMPERATURE_FRIENDLY_NAME);
            m_pTemperatureSensorPropertyValues->SetStringValue(SENSOR_PROPERTY_DESCRIPTION, SENSOR_TEMPERATURE_FRIENDLY_NAME);
            m_pTemperatureSensorPropertyValues->SetUnsignedIntegerValue(SENSOR_PROPERTY_STATE, SENSOR_STATE_INITIALIZING);
            m_pTemperatureSensorPropertyValues->SetUnsignedIntegerValue(SENSOR_PROPERTY_MIN_REPORT_INTERVAL, MINIMUM_REPORT_INTERVAL);
            m_pTemperatureSensorPropertyValues->SetUnsignedIntegerValue(SENSOR_PROPERTY_CURRENT_REPORT_INTERVAL, DEFAULT_REPORT_INTERVAL);
            m_pTemperatureSensorPropertyValues->SetUnsignedIntegerValue(SENSOR_PROPERTY_CONNECTION_TYPE, SENSOR_CONNECTION_TYPE_PC_ATTACHED);

            //
            // The persistent unique id of this temperature sensor
            //
            GUID idGuid = GUID_NULL;
            
            CComPtr<IWDFNamedPropertyStore> spPropStore;
            hr = pDevice->RetrieveDevicePropertyStore(NULL, WdfPropertyStoreCreateIfMissing, &spPropStore, NULL);
            if (SUCCEEDED(hr))
            {
                LPCWSTR lpcszKeyName = L"SENSOR_TEMPERATURE_ID";
                PROPVARIANT vID;
                
                hr = spPropStore->GetNamedValue(lpcszKeyName, &vID);
                if (SUCCEEDED(hr))
                {
                    hr = ::CLSIDFromString(vID.bstrVal, &idGuid);
                }
                else
                {
                    if (SUCCEEDED(::CoCreateGuid(&idGuid)))
                    {
                        LPOLESTR lpszGUID = NULL;
                        hr = ::StringFromCLSID(idGuid, &lpszGUID);
                        if (SUCCEEDED(hr))
                        {
                            PropVariantInit(&vID);
                            vID.vt = VT_LPWSTR;
                            vID.pwszVal = lpszGUID;
                            hr = spPropStore->SetNamedValue(lpcszKeyName, &vID);
                            PropVariantClear(&vID);
                        }
                    }
                }
            }
            
            m_pTemperatureSensorPropertyValues->SetGuidValue(SENSOR_PROPERTY_PERSISTENT_UNIQUE_ID, idGuid);
        }
    }

    // Initialize supported data field keys and values for the Temperature Sensor
    if (SUCCEEDED(hr))
    {
        hr = m_pSupportedTemperatureSensorDataFields.CoCreateInstance(CLSID_PortableDeviceKeyCollection);
        if(SUCCEEDED(hr))
        {
            for (DWORD dwIndex=0; dwIndex<ARRAYSIZE(g_SupportedTemperatureDataFields); dwIndex++)
            {
                m_pSupportedTemperatureSensorDataFields->Add(g_SupportedTemperatureDataFields[dwIndex]);
            }
        }

        if (SUCCEEDED(hr))
        {
            hr = m_pTemperatureSensorDataFieldValues.CoCreateInstance(CLSID_PortableDeviceValues);
            if (SUCCEEDED(hr))
            {
                // Timestamp value 

                //Get the current time in  SYSTEMTIME format
                SYSTEMTIME st; ::GetSystemTime(&st);

                // Convert the SYSTEMTIME into FILETIME
                FILETIME ft; hr = ::SystemTimeToFileTime(&st, &ft) ? S_OK : HRESULT_FROM_WIN32(GetLastError());

                if (S_OK == hr)
                {
                    PROPVARIANT var;
                    PropVariantInit( &var );
                    
                    var.vt                      = VT_FILETIME;
                    var.filetime.dwLowDateTime  = ft.dwLowDateTime;
                    var.filetime.dwHighDateTime = ft.dwHighDateTime;
                    hr = m_pTemperatureSensorDataFieldValues->SetValue(SENSOR_DATA_TYPE_TIMESTAMP, &var);
                    
                    PropVariantClear( &var );
                }

                // Temperature Value
                m_pTemperatureSensorDataFieldValues->SetFloatValue(SENSOR_DATA_TYPE_TEMPERATURE_CELSIUS, TEMPERATURE_VALUE_CELSIUS);
            }
        }

        if (SUCCEEDED(hr))
        {
            // Update the state to ready if everything has been initialized correctly
            m_pTemperatureSensorPropertyValues->SetUnsignedIntegerValue(SENSOR_PROPERTY_STATE, SENSOR_STATE_READY);
        }
        else
        {
            // Something went wrong during data initialization, so the state must be an error
            m_pTemperatureSensorPropertyValues->SetUnsignedIntegerValue(SENSOR_PROPERTY_STATE, SENSOR_STATE_ERROR);
        }
    }

    return hr;
}

/////////////////////////////////////////////////////////////////////////
//
// InitializeGpsSensor()
//
// Initialize function for GPS sensor
//
/////////////////////////////////////////////////////////////////////////
HRESULT CSensorDDI::InitializeGpsSensor(IWDFDevice* pDevice)
{   
    // Initialize the GPS Sensor Property Keys
    HRESULT hr = m_pSupportedGpsSensorProperties.CoCreateInstance(CLSID_PortableDeviceKeyCollection);
    if (SUCCEEDED(hr))
    {
        // Add Common PROPERTYKEYs for ALL WPD objects
        for (DWORD dwIndex=0; dwIndex<ARRAYSIZE(g_SupportedCommonProperties); dwIndex++)
        {
            m_pSupportedGpsSensorProperties->Add(g_SupportedCommonProperties[dwIndex]);
        }

        // Add GPS sensor property keys
        for (DWORD dwIndex=0; dwIndex<ARRAYSIZE(g_SupportedGpsProperties); dwIndex++)
        {
            m_pSupportedGpsSensorProperties->Add(g_SupportedGpsProperties[dwIndex]);
        }
    }

      // Initialize the GPS Sensor Property Values
    if (SUCCEEDED(hr))
    {
        hr = m_pGpsSensorPropertyValues.CoCreateInstance(CLSID_PortableDeviceValues);
        if (SUCCEEDED(hr))
        {
            // WPD Supported Properties
            m_pGpsSensorPropertyValues->SetStringValue(WPD_OBJECT_ID, SENSOR_GPS_ID);
            m_pGpsSensorPropertyValues->SetStringValue(WPD_OBJECT_NAME, SENSOR_GPS_ID);
            m_pGpsSensorPropertyValues->SetStringValue(WPD_OBJECT_PERSISTENT_UNIQUE_ID, SENSOR_GPS_ID);
            m_pGpsSensorPropertyValues->SetStringValue(WPD_OBJECT_PARENT_ID, WPD_DEVICE_OBJECT_ID);
            m_pGpsSensorPropertyValues->SetGuidValue(WPD_OBJECT_FORMAT, WPD_OBJECT_FORMAT_UNSPECIFIED);
            m_pGpsSensorPropertyValues->SetGuidValue(WPD_OBJECT_CONTENT_TYPE, WPD_CONTENT_TYPE_FUNCTIONAL_OBJECT);
            m_pGpsSensorPropertyValues->SetBoolValue(WPD_OBJECT_CAN_DELETE, FALSE);

            // Sensor Supported Properties
            m_pGpsSensorPropertyValues->SetGuidValue(WPD_FUNCTIONAL_OBJECT_CATEGORY, SENSOR_CATEGORY_LOCATION);
            m_pGpsSensorPropertyValues->SetGuidValue(SENSOR_PROPERTY_TYPE, SENSOR_TYPE_LOCATION_GPS);
            m_pGpsSensorPropertyValues->SetStringValue(SENSOR_PROPERTY_MANUFACTURER, DEVICE_MANUFACTURER_VALUE);
            m_pGpsSensorPropertyValues->SetStringValue(SENSOR_PROPERTY_MODEL, DEVICE_MODEL_VALUE);
            m_pGpsSensorPropertyValues->SetStringValue(SENSOR_PROPERTY_SERIAL_NUMBER, DEVICE_SERIAL_NUMBER_VALUE);
            m_pGpsSensorPropertyValues->SetStringValue(SENSOR_PROPERTY_FRIENDLY_NAME, SENSOR_GPS_FRIENDLY_NAME);
            m_pGpsSensorPropertyValues->SetStringValue(SENSOR_PROPERTY_DESCRIPTION, SENSOR_GPS_FRIENDLY_NAME);
            m_pGpsSensorPropertyValues->SetUnsignedIntegerValue(SENSOR_PROPERTY_STATE, SENSOR_STATE_INITIALIZING);
            m_pGpsSensorPropertyValues->SetUnsignedIntegerValue(SENSOR_PROPERTY_MIN_REPORT_INTERVAL, MINIMUM_REPORT_INTERVAL);
            m_pGpsSensorPropertyValues->SetUnsignedIntegerValue(SENSOR_PROPERTY_CURRENT_REPORT_INTERVAL, DEFAULT_REPORT_INTERVAL);
            m_pGpsSensorPropertyValues->SetUnsignedIntegerValue(SENSOR_PROPERTY_CONNECTION_TYPE, SENSOR_CONNECTION_TYPE_PC_ATTACHED);

            //
            // The persistent unique id of this GPS sensor
            //
            GUID idGuid = GUID_NULL;

            CComPtr<IWDFNamedPropertyStore> spPropStore;
            hr = pDevice->RetrieveDevicePropertyStore(NULL, WdfPropertyStoreCreateIfMissing, &spPropStore, NULL);
            if (SUCCEEDED(hr))
            {
                LPCWSTR lpcszKeyName = L"SENSOR_GPS_ID";
                PROPVARIANT vID;
                
                hr = spPropStore->GetNamedValue(lpcszKeyName, &vID);
                if (SUCCEEDED(hr))
                {
                    hr = ::CLSIDFromString(vID.bstrVal, &idGuid);
                }
                else
                {
                    if (SUCCEEDED(::CoCreateGuid(&idGuid)))
                    {
                        LPOLESTR lpszGUID = NULL;
                        hr = ::StringFromCLSID(idGuid, &lpszGUID);
                        if (SUCCEEDED(hr))
                        {
                            PropVariantInit(&vID);
                            vID.vt = VT_LPWSTR;
                            vID.pwszVal = lpszGUID;
                            hr = spPropStore->SetNamedValue(lpcszKeyName, &vID);
                            PropVariantClear(&vID);
                        }
                    }
                }
            }
            
            m_pGpsSensorPropertyValues->SetGuidValue(SENSOR_PROPERTY_PERSISTENT_UNIQUE_ID, idGuid);
        }
    }

    // Initialize supported data field keys and values for the GPS Sensor
    if (SUCCEEDED(hr))
    {
        hr = m_pSupportedGpsSensorDataFields.CoCreateInstance(CLSID_PortableDeviceKeyCollection);
        if(SUCCEEDED(hr))
        {
            for (DWORD dwIndex=0; dwIndex<ARRAYSIZE(g_SupportedGpsDataFields); dwIndex++)
            {
                m_pSupportedGpsSensorDataFields->Add(g_SupportedGpsDataFields[dwIndex]);
            }
        }

        if (SUCCEEDED(hr))
        {
            hr = m_pGpsSensorDataFieldValues.CoCreateInstance(CLSID_PortableDeviceValues);
            if (SUCCEEDED(hr))
            {
                // Timestamp value 

                //Get the current time in  SYSTEMTIME format
                SYSTEMTIME st; ::GetSystemTime(&st);

                // Convert the SYSTEMTIME into FILETIME
                FILETIME ft; hr = ::SystemTimeToFileTime(&st, &ft) ? S_OK : HRESULT_FROM_WIN32(GetLastError());

                PROPVARIANT var;
                PropVariantInit( &var );

                if (S_OK == hr )
                {
                    var.vt                      = VT_FILETIME;
                    var.filetime.dwLowDateTime  = ft.dwLowDateTime;
                    var.filetime.dwHighDateTime = ft.dwHighDateTime;
                    hr = m_pGpsSensorDataFieldValues->SetValue(SENSOR_DATA_TYPE_TIMESTAMP, &var);
                }

                // Latitude Value
                var.vt      = VT_R8;
                var.dblVal  = LATITUDE_VALUE_DEGREES;
                m_pGpsSensorDataFieldValues->SetValue(SENSOR_DATA_TYPE_LATITUDE_DEGREES, &var);

                // Longitude Value
                var.vt      = VT_R8;
                var.dblVal  = LONGITUDE_VALUE_DEGREES;
                m_pGpsSensorDataFieldValues->SetValue(SENSOR_DATA_TYPE_LONGITUDE_DEGREES,  &var);

                // Altitude Value
                var.vt      = VT_R8;
                var.dblVal  = ALTITUDE_VALUE_METERS;
                m_pGpsSensorDataFieldValues->SetValue(SENSOR_DATA_TYPE_ALTITUDE_ELLIPSOID_METERS,  &var); 
                
                // Error Radius Value
                var.vt      = VT_R8;
                var.dblVal  = ERROR_RADIUS_VALUE_METERS;
                m_pGpsSensorDataFieldValues->SetValue(SENSOR_DATA_TYPE_ERROR_RADIUS_METERS, &var);

                PropVariantClear( &var );
            }
        }

        if (SUCCEEDED(hr))
        {
            // Update the state to ready if everything has been initialized correctly
            m_pGpsSensorPropertyValues->SetUnsignedIntegerValue(SENSOR_PROPERTY_STATE, SENSOR_STATE_READY);
        }
        else
        {
            // Something went wrong during data initialization, so the state must be an error
            m_pGpsSensorPropertyValues->SetUnsignedIntegerValue(SENSOR_PROPERTY_STATE, SENSOR_STATE_ERROR);
        }
    }

    return hr;
}

/////////////////////////////////////////////////////////////////////////
//
// CSensorDDI::OnClientConnect
//
// Routine Description:
//
//  This method is called by Sensor Class Extension when a client app connects
//  to a Sensor
//  
// Arguments:
//
//  pwszObjectID - String that represents the object whose supported property keys 
//             are being requested
// 
//
// Return Value:
//  Status
// 
//
/////////////////////////////////////////////////////////////////////////
HRESULT CSensorDDI:: OnClientConnect(
        __in IWDFFile* pClientFile,
        __in LPWSTR pwszObjectID
        )
{
    UNREFERENCED_PARAMETER(pClientFile);
    UNREFERENCED_PARAMETER(pwszObjectID);

    HRESULT hr = S_OK;

    return hr;
}

/////////////////////////////////////////////////////////////////////////
//
// CSensorDDI::OnClientDisconnect
//
// Routine Description:
//
//  This method is called by Sensor Class Extension when a client app disconnects
//  from a Sensor
//  
// Arguments:
//
//  pwszObjectID - String that represents the object whose supported property keys 
//             are being requested
// 
//
// Return Value:
//  Status
// 
//
/////////////////////////////////////////////////////////////////////////
HRESULT CSensorDDI:: OnClientDisconnect(
        __in IWDFFile* pClientFile,
        __in LPWSTR pwszObjectID
        )
{
    UNREFERENCED_PARAMETER(pClientFile);
    UNREFERENCED_PARAMETER(pwszObjectID);

    HRESULT hr = S_OK;

    return hr;
}

/////////////////////////////////////////////////////////////////////////
//
// CSensorDDI::OnClientSubscribeToEvents
//
// Routine Description:
//
//  This method is called by Sensor Class Extension when a client subscribes to
//  events by calling SetEventSink
//  
// Arguments:
//
//  pwszObjectID - String that represents the object whose supported property keys 
//             are being requested
// 
//
// Return Value:
//  Status
// 
//
/////////////////////////////////////////////////////////////////////////
HRESULT CSensorDDI:: OnClientSubscribeToEvents(
        __in IWDFFile* pClientFile,
        __in LPWSTR pwszObjectID
        )
{
    UNREFERENCED_PARAMETER(pClientFile);
    UNREFERENCED_PARAMETER(pwszObjectID);

    HRESULT hr = S_OK;

    return hr;
}

/////////////////////////////////////////////////////////////////////////
//
// CSensorDDI::OnClientUnsubscribeFromEvents
//
// Routine Description:
//
//  This method is called by Sensor Class Extension when a client unsubscribes from
//  events by calling SetEventSink(NULL)
//  
// Arguments:
//
//  pwszObjectID - String that represents the object whose supported property keys 
//             are being requested
// 
//
// Return Value:
//  Status
// 
//
/////////////////////////////////////////////////////////////////////////
HRESULT CSensorDDI:: OnClientUnsubscribeFromEvents(
        __in IWDFFile* pClientFile,
        __in LPWSTR pwszObjectID
        )
{
    UNREFERENCED_PARAMETER(pClientFile);
    UNREFERENCED_PARAMETER(pwszObjectID);

    HRESULT hr = S_OK;

    return hr;
}

///////////////////////////////////////////////////////////////////////
//
// CSensorDDI::OnProcessWpdMessage
//
///////////////////////////////////////////////////////////////////////
HRESULT CSensorDDI:: OnProcessWpdMessage(
        __in IUnknown* pPortableDeviceValuesParamsUnknown,
        __in IUnknown* pPortableDeviceValuesResultsUnknown
        )
{
    UNREFERENCED_PARAMETER(pPortableDeviceValuesParamsUnknown);
    UNREFERENCED_PARAMETER(pPortableDeviceValuesResultsUnknown);

    HRESULT hr = S_OK;

    return hr;
}


/////////////////////////////////////////////////////////////////////////
//
// CSensorDDI::OnGetSupportedSensorObjects
//
// Routine Description:
//
//  This method is called by Sensor Class Extension during the initialize procedure to get 
//  the list of sensor objects and their supported properties.
//  
//  NOTE: This driver supports two sensors, One temperature sensor and one GPS sensor
//
// Arguments:
//
//  ppPortableDeviceValuesCollection - A double IPortableDeviceValuesCollection pointer
//                                     that receives the set of Sensor property values
//
// Return Value:
//
//  Status
// 
//
/////////////////////////////////////////////////////////////////////////
HRESULT CSensorDDI::OnGetSupportedSensorObjects(
        __out IPortableDeviceValuesCollection** ppSensorObjectCollection
        )
{
    HRESULT hr = (NULL == ppSensorObjectCollection) ? E_INVALIDARG : S_OK;
        
    if (SUCCEEDED(hr))
    {
        CComPtr<IPortableDeviceValuesCollection> spObjects;
        hr = spObjects.CoCreateInstance(CLSID_PortableDeviceValuesCollection);
    
        // Get the supported TEMPERATURE sensor supported property keys
        if (SUCCEEDED(hr))
        {
            CComPtr<IPortableDeviceKeyCollection> spKeys;
            hr = OnGetSupportedProperties(SENSOR_TEMPERATURE_ID, &spKeys);

            if (SUCCEEDED(hr))
            {
                // Get the TEMPERATURE sensor property values
                CComPtr<IPortableDeviceValues> spValues;
                CComPtr<IWDFFile>              tmpFile;
                if (SUCCEEDED(hr))
                {
                    hr = OnGetProperties(tmpFile, SENSOR_TEMPERATURE_ID, spKeys, &spValues);
                }

                if (SUCCEEDED(hr))
                {
                    hr = spObjects->Add(spValues);
                }
            }
        }

        // Get the supported GPS sensor supported property keys
        if (SUCCEEDED(hr))
        {
            CComPtr<IPortableDeviceKeyCollection> spKeys;
            hr = OnGetSupportedProperties(SENSOR_GPS_ID, &spKeys);

            if (SUCCEEDED(hr))
            {
                // Get the supported GPS sensor supported property values
                CComPtr<IPortableDeviceValues> spValues;
                CComPtr<IWDFFile>              tmpFile;
                if (SUCCEEDED(hr))
                {
                    hr = OnGetProperties(tmpFile, SENSOR_GPS_ID, spKeys, &spValues);
                }

                if (SUCCEEDED(hr))
                {
                    hr = spObjects->Add(spValues);
                }
            }
        }

        // Copy over the collection
       *ppSensorObjectCollection = spObjects.Detach();
    }
   
   return hr;
}

/////////////////////////////////////////////////////////////////////////
//
// CSensorDDI::OnGetSupportedProperties
//
// Routine Description:
//
//  This method is called by Sensor Class Extension to get the list of supported properties
//  for a particular Sensor.
//  
// Arguments:
//
//  pwszObjectID - String that represents the object whose supported property keys are being requested
//  ppKeys - An IPortableDeviceKeyCollection to be populated with supported PROPERTYKEYs
//
// Return Value:
//  Status
// 
//
/////////////////////////////////////////////////////////////////////////
HRESULT CSensorDDI::OnGetSupportedProperties(
        __in  LPWSTR pwszObjectID,
        __out IPortableDeviceKeyCollection** ppKeys
        )
{
    HRESULT hr = ( (NULL == pwszObjectID) || (NULL == ppKeys) ) ? E_INVALIDARG : S_OK;

    if (SUCCEEDED(hr))
    {
        CComPtr<IPortableDeviceKeyCollection> spKeys;        
        CString strObjectID = pwszObjectID;

        // Copy the Temperature Sensor Property Keys 
        if (0 == strObjectID.CompareNoCase(SENSOR_TEMPERATURE_ID))
        {
            if (NULL != m_pSupportedTemperatureSensorProperties)
            {
                spKeys = m_pSupportedTemperatureSensorProperties;
            }
            else
            {
                hr = E_UNEXPECTED;
            }
        }
        // Copy the GPS Sensor Property Keys 
        else if (0 == strObjectID.CompareNoCase(SENSOR_GPS_ID))
        {
            if (NULL != m_pSupportedGpsSensorProperties)
            {
                spKeys = m_pSupportedGpsSensorProperties;
            }
            else
            {
                hr = E_UNEXPECTED;
            }
        }
        else
        {
            hr = E_UNEXPECTED;
        }

        // Copy over the property keys
        *ppKeys = spKeys.Detach();
    }

    return hr;
}

/////////////////////////////////////////////////////////////////////////
//
// CSensorDDI::OnGetSupportedDataFields
//
// Routine Description:
//
//  This method is called by Sensor Class Extension to get the list of supported data fields
//  for a particular Sensor.
//  
// Arguments:
//
//  pwszObjectID - String that represents the object whose supported property keys are being requested
//  ppKeys - An IPortableDeviceKeyCollection to be populated with supported PROPERTYKEYs
//
// Return Value:
//  Status
// 
//
/////////////////////////////////////////////////////////////////////////
HRESULT CSensorDDI:: OnGetSupportedDataFields(
        __in  LPWSTR pwszObjectID,
        __out IPortableDeviceKeyCollection** ppKeys
        )
{
    HRESULT hr = ( (NULL == pwszObjectID) || (NULL == ppKeys) ) ? E_INVALIDARG : S_OK;

    if (SUCCEEDED(hr))
    {
        CComPtr<IPortableDeviceKeyCollection> spKeys;
        CString strObjectID = pwszObjectID;

        // Copy the Temperature Sensor Data Field Keys 
        if (0 == strObjectID.CompareNoCase(SENSOR_TEMPERATURE_ID))
        {
            if (NULL != m_pSupportedTemperatureSensorDataFields)
            {
                spKeys = m_pSupportedTemperatureSensorDataFields;
            }
            else
            {
                hr = E_UNEXPECTED;
            }
        }
        // Copy the GPS Sensor Data Field Keys 
        else if (0 == strObjectID.CompareNoCase(SENSOR_GPS_ID))
        {
            if (NULL != m_pSupportedGpsSensorDataFields)
            {
                spKeys = m_pSupportedGpsSensorDataFields;
            }
            else
            {
                hr = E_UNEXPECTED;
            }
        }
        else
        {
            hr = E_UNEXPECTED;
        }

        // Copy over the property keys
        *ppKeys = spKeys.Detach();
    }

    return hr;
}

/////////////////////////////////////////////////////////////////////////
//
// CSensorDDI::OnGetSupportedEvents
//
// Routine Description:
//
//  This method is called by Sensor Class Extension to get the list of supported events
//  for a particular Sensor.
//  
// Arguments:
//
// pwszObjectID - String that represents the object whose supported property keys are being requested
//  ppSupportedEvents - A set of GUIDs that represent the supported events
//  pulEventCount - Count of the number of events supported
//
// Return Value:
//  Status
// 
//
/////////////////////////////////////////////////////////////////////////
HRESULT CSensorDDI::OnGetSupportedEvents(
        __in    LPWSTR pwszObjectID,
        __out   GUID** ppSupportedEvents,
        __out   ULONG* pulEventCount
        )
{
    UNREFERENCED_PARAMETER(pwszObjectID);

    HRESULT hr = ( NULL == ppSupportedEvents ) ? E_INVALIDARG : S_OK;

    // This driver does not support any events
    if ( SUCCEEDED(hr) )
    {
        // Return a single GUID buffer set to GUID_NULL
        GUID* pBuf = (GUID*)CoTaskMemAlloc( sizeof(GUID) );

        if ( NULL == pBuf )
        {            
            *ppSupportedEvents = NULL;
            *pulEventCount = 0;
            hr = E_OUTOFMEMORY;
        }
        else
        {        
            *pBuf = GUID_NULL;
            *ppSupportedEvents = pBuf;
            *pulEventCount = 0;
        }
    }

    return hr;
}

/////////////////////////////////////////////////////////////////////////
//
// CSensorDDI::OnGetProperties
//
// Routine Description:
//
//  This method is called by Sensor Class Extension to get Sensor property values 
//  for a particular Sensor.
//  
// Arguments:
//
//  pwszObjectID - String that represents the object whose supported property keys are being requested
// 
//
// Return Value:
// 
// ppValues - IPortableDeviceValues that contains the values for the keys requested
// 
//
/////////////////////////////////////////////////////////////////////////
HRESULT CSensorDDI:: OnGetProperties(
        __in  IWDFFile* pClientFile,
        __in  LPWSTR pwszObjectID,
        __in  IPortableDeviceKeyCollection* pKeys,
        __out IPortableDeviceValues** ppValues
        )
{   
    UNREFERENCED_PARAMETER(pClientFile);
    BOOL fError = FALSE;

    HRESULT hr = ( (NULL == pwszObjectID) || (NULL == pKeys) || (NULL == ppValues) ) ? E_INVALIDARG : S_OK;

    if ( SUCCEEDED(hr) )
    {
        CComPtr<IPortableDeviceValues> spValues;
        hr = spValues.CoCreateInstance(CLSID_PortableDeviceValues);

        if (SUCCEEDED(hr))
        {
             CString strObjectID = pwszObjectID;
             if (0 == strObjectID.CompareNoCase(SENSOR_TEMPERATURE_ID))
             {
                DWORD cKeys = 0;
                hr = pKeys->GetCount(&cKeys);

                for (DWORD dwIndex = 0; SUCCEEDED(hr) && dwIndex < cKeys; dwIndex++)
                {
                    PROPERTYKEY Key = WPD_PROPERTY_NULL;
                    if ( (S_OK == pKeys->GetAt(dwIndex, &Key)) && !IsEqualPropertyKey(Key, WPD_PROPERTY_NULL) )
                    {
                        PROPVARIANT value;
                        PropVariantInit(&value);

                        hr = (NULL == m_pTemperatureSensorPropertyValues) ? E_UNEXPECTED : S_OK;
                        if (SUCCEEDED(hr))
                        {
                            HRESULT hrTemp = m_pTemperatureSensorPropertyValues->GetValue(Key, &value);
                        
                            if (SUCCEEDED(hrTemp))
                            {
                                spValues->SetValue(Key, &value);
                            }
                            else
                            {
                                // Failed to find the requested property, convey the hr back to the caller
                                spValues->SetErrorValue(Key, hrTemp);
                                fError = TRUE;
                            }
                        }

                        PropVariantClear(&value);
                    }
                } // end of for loop
             }
             else if (0 == strObjectID.CompareNoCase(SENSOR_GPS_ID))
             {
                DWORD cKeys = 0;
                hr = pKeys->GetCount(&cKeys);

                for (DWORD dwIndex = 0; SUCCEEDED(hr) && dwIndex < cKeys; dwIndex++)
                {
                    PROPERTYKEY Key = WPD_PROPERTY_NULL;
                    if ((S_OK == pKeys->GetAt(dwIndex, &Key)) && !IsEqualPropertyKey(Key, WPD_PROPERTY_NULL) )
                    {
                        PROPVARIANT value;
                        PropVariantInit(&value);

                        hr= (NULL == m_pGpsSensorPropertyValues) ? E_UNEXPECTED : S_OK;
                        if (SUCCEEDED(hr))
                        {
                            HRESULT hrTemp = m_pGpsSensorPropertyValues->GetValue(Key, &value);
                        
                            if (SUCCEEDED(hrTemp))
                            {
                                spValues->SetValue(Key, &value);
                            }
                            else
                            {
                                // Failed to find the requested property, convey the hr back to the caller
                                spValues->SetErrorValue(Key, hrTemp);
                                fError = TRUE;
                            }
                        }

                        PropVariantClear(&value);
                    }
                } // end of for loop
             }
             else
             {
                  hr = E_UNEXPECTED;
             }

             // Copy the IPortableDeviceValues
             *ppValues = spValues.Detach();
        }
    }

    return (FALSE == fError) ? hr : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////
//
// CSensorDDI::OnGetDataFields
//
// Routine Description:
//
//  This method is called by Sensor Class Extension to get Sensor data values 
//  for a particular Sensor.
//  
// Arguments:
//
//  pwszObjectID - String that represents the object whose supported property keys are being requested
// 
//
// Return Value:
// 
//  ppValues - IPortableDeviceValues that contains the values for the keys requested
// 
//
/////////////////////////////////////////////////////////////////////////
HRESULT CSensorDDI:: OnGetDataFields(
        __in  IWDFFile* pClientFile,
        __in  LPWSTR pwszObjectID,
        __in  IPortableDeviceKeyCollection* pKeys,
        __out IPortableDeviceValues** ppValues
        )
{
    UNREFERENCED_PARAMETER(pClientFile);

    HRESULT hr = ((NULL == pwszObjectID) || (NULL == pKeys) || (NULL == ppValues)) ? E_INVALIDARG : S_OK;

    if ( SUCCEEDED(hr) )
    {
        CComPtr<IPortableDeviceValues> spValues;
        hr = spValues.CoCreateInstance(CLSID_PortableDeviceValues);

        if ( SUCCEEDED(hr) )
        {
            CString strObjectID = pwszObjectID;
            if (0 == strObjectID.CompareNoCase(SENSOR_TEMPERATURE_ID))
            {
                DWORD cKeys = 0;
                hr = pKeys->GetCount(&cKeys);

                for (DWORD dwIndex = 0; SUCCEEDED(hr) && dwIndex < cKeys; dwIndex++)
                {
                    PROPERTYKEY Key = WPD_PROPERTY_NULL;
                    if ( (S_OK == pKeys->GetAt(dwIndex, &Key)) && !IsEqualPropertyKey(Key, WPD_PROPERTY_NULL) )
                    {
                        PROPVARIANT value;
                        PropVariantInit(&value);

                        hr = (NULL == m_pTemperatureSensorDataFieldValues) ? E_UNEXPECTED : S_OK;
                        if (SUCCEEDED(hr))
                        {
                            // Preset the property value to 'error not supported'.  The actual value
                            // will replace this value, if read from the device.
                            spValues->SetErrorValue(Key, HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED));
                        
                            if ( SUCCEEDED(m_pTemperatureSensorDataFieldValues->GetValue(Key, &value)) )
                            {
                                spValues->SetValue(Key, &value);
                            }
                        }

                        PropVariantClear(&value);
                    }
                }
            }
            else if (0 == strObjectID.CompareNoCase(SENSOR_GPS_ID))
            {
                DWORD cKeys = 0;
                pKeys->GetCount(&cKeys);

                for (DWORD dwIndex = 0; SUCCEEDED(hr) && dwIndex < cKeys; dwIndex++)
                {
                    PROPERTYKEY Key = WPD_PROPERTY_NULL;
                    if ((S_OK == pKeys->GetAt(dwIndex, &Key)) && !IsEqualPropertyKey(Key, WPD_PROPERTY_NULL) )
                    {
                        PROPVARIANT value;
                        PropVariantInit(&value);

                        hr = (NULL == m_pGpsSensorDataFieldValues) ? E_UNEXPECTED : S_OK;
                        if (SUCCEEDED(hr))
                        {
                            // Preset the property value to 'error not supported'.  The actual value
                            // will replace this value, if read from the device.
                            spValues->SetErrorValue(Key, HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED));
                        
                            if (  SUCCEEDED(m_pGpsSensorDataFieldValues->GetValue(Key, &value)) )
                            {
                                spValues->SetValue(Key, &value);
                            }
                        }

                        PropVariantClear(&value);
                    }
                }
            }
            else
            {
                hr = E_UNEXPECTED;
            }

            // Copy over the IPortableDeviceValues
            *ppValues = spValues.Detach();
        }
    }

    return hr ;
}

/////////////////////////////////////////////////////////////////////////
//
// CSensorDDI::OnSetProperties
//
// Routine Description:
//
//  This method is called by Sensor Class Extension to set Sensor property values 
//  for a particular Sensor.
//  
// Arguments:
//
//  pwszObjectID - String that represents the object whose supported property keys are being requested
// 
//
// Return Value:
// 
//  ppResults - IPortableDeviceValues that contains the results of the set operation
// 
//
/////////////////////////////////////////////////////////////////////////
HRESULT CSensorDDI:: OnSetProperties(
        __in IWDFFile* pClientFile,
        __in LPWSTR pwszObjectID,
        __in IPortableDeviceValues* pValues,
        __out IPortableDeviceValues** ppResults
        )
{
    UNREFERENCED_PARAMETER(pClientFile);
    BOOL fError = FALSE;

    HRESULT hr = ( (NULL == pwszObjectID) || (NULL == pValues) || (NULL == ppResults) ) ? E_INVALIDARG : S_OK;
    
    CComPtr<IPortableDeviceValues> spValues;
    if (SUCCEEDED(hr))
    {
        hr = spValues.CoCreateInstance(CLSID_PortableDeviceValues);
    }

    if (SUCCEEDED(hr))
    {
        DWORD cValues = 0;
        hr = pValues->GetCount(&cValues);
        
        if (SUCCEEDED(hr))
        {
            CString strObjectID = pwszObjectID;
            
            for (DWORD dwIndex = 0; dwIndex < cValues; dwIndex++)
            {
                PROPERTYKEY key = WPD_PROPERTY_NULL;
                PROPVARIANT var;
                PropVariantInit( &var );

                if (SUCCEEDED(pValues->GetAt(dwIndex, &key, &var)))
                {
                    if (0 == strObjectID.CompareNoCase(SENSOR_TEMPERATURE_ID))
                    {
                        // The only settable property supported is the Current Report Interval
                        if( IsEqualPropertyKey( key, SENSOR_PROPERTY_CURRENT_REPORT_INTERVAL ) &&
                            VT_UI4 == var.vt )
                        {
                            m_pTemperatureSensorPropertyValues->SetValue(key, &var);
                        }
                        else
                        {
                            fError = TRUE;
                        }
                    }
                    else if (0 == strObjectID.CompareNoCase(SENSOR_GPS_ID))
                    {
                        // The only settable property supported is the Current Report Interval
                        if( IsEqualPropertyKey( key, SENSOR_PROPERTY_CURRENT_REPORT_INTERVAL ) &&
                            VT_UI4 == var.vt )
                        {
                            m_pGpsSensorPropertyValues->SetValue(key, &var);
                        }
                        else
                        {
                            fError = TRUE;
                        }
                    }
                    else
                    {
                        fError = TRUE;
                    }

                    if (TRUE == fError)
                    {
                        // Set the hr if the Key is not found or is not settable
                        spValues->SetErrorValue(key, HRESULT_FROM_WIN32(ERROR_NOT_FOUND));
                    }                        
                }

                PropVariantClear(&var);

            }// end of for loop

            *ppResults = spValues.Detach();
        }
    }

    return (FALSE == fError) ? hr : S_FALSE;
}
