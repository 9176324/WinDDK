/*++

// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

Module Name:

   SensorDdi.cpp

Abstract:

    This module implements the ISensorDriver interface which is used
      by the Sensor Class Extension.

--*/


#include "internal.h"
#include "SensorDdi.h"
#include "sensorddi.tmh"
#include "SampleEvents.h"

// Define an object ID.
// {0D77BEE3-7169-42bf-8379-28F9A9B59A57}
DEFINE_GUID(SAMPLE_SENSOR_TIME_ID,
0xd77bee3, 0x7169, 0x42bf, 0x83, 0x79, 0x28, 0xf9, 0xa9, 0xb5, 0x9a, 0x57);

// Define a custom category.
// {062A5C3B-44C1-4ad1-8EFC-0F65B2E4AD48}
DEFINE_GUID(SAMPLE_SENSOR_CATEGORY_DATE_TIME,
0x62a5c3b, 0x44c1, 0x4ad1, 0x8e, 0xfc, 0xf, 0x65, 0xb2, 0xe4, 0xad, 0x48);

// Define a custom type.
// {5F199A84-409F-4e35-B2DD-F9C79F5318A0}
DEFINE_GUID(SAMPLE_SENSOR_TYPE_TIME,
0x5f199a84, 0x409f, 0x4e35, 0xb2, 0xdd, 0xf9, 0xc7, 0x9f, 0x53, 0x18, 0xa0);

// Date sensor fields.
// Because these fields are related, each field uses the same GUID, but changes the PID.
// {340946F2-9A77-42b0-8176-57D4DF00E5CA}
DEFINE_PROPERTYKEY(SAMPLE_SENSOR_DATA_TYPE_HOUR,
0x340946f2, 0x9a77, 0x42b0, 0x81, 0x76, 0x57, 0xd4, 0xdf, 0x0, 0xe5, 0xca, PID_FIRST_USABLE); // PID = 2

// {340946F2-9A77-42b0-8176-57D4DF00E5CA}
DEFINE_PROPERTYKEY(SAMPLE_SENSOR_DATA_TYPE_MINUTE,
0x340946f2, 0x9a77, 0x42b0, 0x81, 0x76, 0x57, 0xd4, 0xdf, 0x0, 0xe5, 0xca, PID_FIRST_USABLE + 1); // PID = 3

// {340946F2-9A77-42b0-8176-57D4DF00E5CA}
DEFINE_PROPERTYKEY(SAMPLE_SENSOR_DATA_TYPE_SECOND,
0x340946f2, 0x9a77, 0x42b0, 0x81, 0x76, 0x57, 0xd4, 0xdf, 0x0, 0xe5, 0xca, PID_FIRST_USABLE + 2); // PID = 4

// Properties common to all sensors.
const PROPERTYKEY g_SupportedCommonProperties[] =
{
    WPD_OBJECT_ID,
    WPD_OBJECT_PERSISTENT_UNIQUE_ID,
    WPD_OBJECT_PARENT_ID,
    WPD_OBJECT_NAME,
    WPD_OBJECT_FORMAT,
    WPD_OBJECT_CONTENT_TYPE,
    WPD_OBJECT_CAN_DELETE,
    WPD_FUNCTIONAL_OBJECT_CATEGORY
};

// Properties supported by all devices.
const PROPERTYKEY g_SupportedDeviceProperties[] =
{
    WPD_DEVICE_FIRMWARE_VERSION,
    WPD_DEVICE_POWER_LEVEL,
    WPD_DEVICE_POWER_SOURCE,
    WPD_DEVICE_PROTOCOL,
    WPD_DEVICE_MODEL,
    WPD_DEVICE_SERIAL_NUMBER,
    WPD_DEVICE_SUPPORTS_NON_CONSUMABLE,
    WPD_DEVICE_MANUFACTURER,
    WPD_DEVICE_FRIENDLY_NAME,
    WPD_DEVICE_TYPE
};

// Properties supported by the sensor.
const PROPERTYKEY g_SupportedSensorProperties[] =
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
    SENSOR_PROPERTY_CONNECTION_TYPE    
};

// Data fields supported by the time sensor.
const PROPERTYKEY g_SupportedDataFields[] =
{
    SAMPLE_SENSOR_DATA_TYPE_HOUR,
    SAMPLE_SENSOR_DATA_TYPE_MINUTE,
    SAMPLE_SENSOR_DATA_TYPE_SECOND,
    SENSOR_DATA_TYPE_TIMESTAMP	
};


// Identifies the object type request for setting or getting properties.
enum ObjectType
{
    TIME_SENSOR = 0,
    DEVICE = 1,
    OBJECT_TYPE_LAST = 2
};


/////////////////////////////////////////////////////////////////////////
//
// CSensorDdi()
//
// Constructor.
//
/////////////////////////////////////////////////////////////////////////
CSensorDdi::CSensorDdi()
{
    m_pSampleEvents = NULL;
    m_ulReportInterval = g_dwDefaultInterval;
    m_cEventClients = 0;
    m_guidPUID = GUID_NULL;
    m_state = SENSOR_STATE_INITIALIZING;

}

/////////////////////////////////////////////////////////////////////////
//
// ~CSensorDdi()
//
// Destructor
//
/////////////////////////////////////////////////////////////////////////
CSensorDdi::~CSensorDdi()
{

}


/////////////////////////////////////////////////////////////////////////
//
// CSensorDdi::OnGetSupportedSensorObjects
//
// Implementation of the ISensorDriver method.
//
// This driver supports one sensor: a time/date sensor.
//
/////////////////////////////////////////////////////////////////////////
HRESULT CSensorDdi:: OnGetSupportedSensorObjects(
        __out IPortableDeviceValuesCollection** ppSensorObjectCollection
        )
{
    if(NULL == ppSensorObjectCollection)
    {
        return E_POINTER;
    }

    HRESULT hr = S_OK;
    CComPtr<IPortableDeviceValuesCollection> spObjects;
    CComPtr<IPortableDeviceKeyCollection> spKeys;
    CComPtr<IPortableDeviceValues> spValues;

    // CoCreate a collection to store the sensor object identifiers.
    hr = spObjects.CoCreateInstance(CLSID_PortableDeviceValuesCollection);

   
    if (SUCCEEDED(hr))
    {
        // Retrieve the list of supported properties.
        hr = OnGetSupportedProperties((LPWSTR)g_wszSensorID, &spKeys);
    }

    if(SUCCEEDED(hr))
    {
        CComPtr<IWDFFile> spTemp;

        // Retrieve the values for the supported properties.
        // Note that we're using a DDI call as a helper function, here.
        // Setting the first parameter to NULL will be problematic if you
        // choose to track or use IWDFFile pointers in OnGetProperties.
        // This sample does not do so, therefore this is a safe thing to do
        // in this code.
        hr = OnGetProperties(spTemp, (LPWSTR)g_wszSensorID, spKeys, &spValues);
    }

    if (hr == S_OK)  // hr could be S_FALSE.
    {
        // Add the supported properties to the collection.
        hr = spObjects->Add(spValues);

        if(SUCCEEDED(hr))
        {
            *ppSensorObjectCollection = spObjects.Detach();
        }
    }

    return hr;
}

/////////////////////////////////////////////////////////////////////////
//
// CSensorDdi:: OnGetSupportedProperties
//
// Implementation of the ISensorDriver method.
//
/////////////////////////////////////////////////////////////////////////
HRESULT CSensorDdi:: OnGetSupportedProperties(
        __in  LPWSTR SensorID,
        __out IPortableDeviceKeyCollection** ppKeys
        )
{
    if(NULL == SensorID ||
       NULL == ppKeys)
    {
        return E_POINTER;
    }

    HRESULT hr = S_OK;

    CComPtr<IPortableDeviceKeyCollection> spKeys;
    hr = spKeys.CoCreateInstance(CLSID_PortableDeviceKeyCollection);

    if(SUCCEEDED(hr))
    {
        // Add the common properties.
        for (DWORD dwIndex = 0; dwIndex < ARRAYSIZE(g_SupportedCommonProperties); dwIndex++)
        {
             spKeys->Add(g_SupportedCommonProperties[dwIndex]);
        }

        if(wcsncmp(SensorID, WPD_DEVICE_OBJECT_ID, wcsnlen(SensorID, g_iMaxStrLen)) == 0)
        {
            // Add the supported device properties.
            for (DWORD dwIndex = 0; dwIndex < ARRAYSIZE(g_SupportedDeviceProperties); dwIndex++)
            {
                spKeys->Add(g_SupportedDeviceProperties[dwIndex]);
            }
        }
        else if (wcsncmp(SensorID, g_wszSensorID, wcsnlen(SensorID, g_iMaxStrLen)) == 0)
        {
             // Add the supported sensor properties.
            for (DWORD dwIndex = 0; dwIndex < ARRAYSIZE(g_SupportedSensorProperties); dwIndex++)
            {
                spKeys->Add(g_SupportedSensorProperties[dwIndex]);
            }
        }
    }

    if(SUCCEEDED(hr))
    {
        // Copy the property keys.
        *ppKeys = spKeys.Detach();
    }

    return hr;
}

/////////////////////////////////////////////////////////////////////////
//
// CSensorDdi:: OnGetSupportedDataFields
//
// Implementation of the ISensorDriver method.
//
/////////////////////////////////////////////////////////////////////////
HRESULT CSensorDdi:: OnGetSupportedDataFields(
        __in  LPWSTR SensorID,
        __out IPortableDeviceKeyCollection** ppKeys
        )
{
    if(NULL == ppKeys || 
       NULL == SensorID)
    {
        return E_POINTER;
    }

    HRESULT hr = S_OK;

    // This method is called by the class extension and
    // the event thread. We need a critical section to 
    // serialize access, here.
    Lock();

    CComPtr<IPortableDeviceKeyCollection> spKeys;
    hr = spKeys.CoCreateInstance(CLSID_PortableDeviceKeyCollection);

    if(SUCCEEDED(hr))
    {
        // Add the supported data fields to the collection.
        for (DWORD dwIndex = 0; dwIndex < ARRAYSIZE(g_SupportedDataFields); dwIndex++)
        {
            spKeys->Add(g_SupportedDataFields[dwIndex] );
        }
    }

    if(SUCCEEDED(hr))
    {
        *ppKeys = spKeys.Detach();
    }

    Unlock();

    return hr;
}

/////////////////////////////////////////////////////////////////////////
//
// CSensorDdi:: OnGetSupportedEvents
//
// Implementation of the ISensorDriver method.
//
/////////////////////////////////////////////////////////////////////////
HRESULT CSensorDdi:: OnGetSupportedEvents(
        __in  LPWSTR SensorID,
        __out GUID **ppSupportedEvents,
        __out ULONG *pulEventCount
        )
{
    if(NULL == SensorID ||
       NULL == ppSupportedEvents ||
       NULL == pulEventCount)
    {
        return E_POINTER;
    }

    HRESULT hr = S_OK;

    // Test for the correct sensor ID.
    if(wcsncmp(SensorID, g_wszSensorID, wcsnlen(SensorID, g_iMaxStrLen)) != 0)
    {
        return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
    }

    // Allocate a buffer for one event type GUID.
    // The platform will free this memory.
    GUID* pBuf = (GUID*)CoTaskMemAlloc(sizeof(GUID)*2);

    if(pBuf != NULL)
    {
        pBuf[0] = SENSOR_EVENT_DATA_UPDATED; // data event
        pBuf[1] = SENSOR_EVENT_STATE_CHANGED;

        // Return the pointer to the buffer and the count.
        *ppSupportedEvents = pBuf;
        *pulEventCount = 2;
        }
    else // Failed to allocate the GUID buffer.
    {
        hr = E_OUTOFMEMORY;

        *ppSupportedEvents = NULL;
        *pulEventCount = 0;
    }

    return hr;
}

/////////////////////////////////////////////////////////////////////////
//
// CSensorDdi:: OnGetProperties
//
// Implementation of the ISensorDriver method.
//
/////////////////////////////////////////////////////////////////////////
HRESULT CSensorDdi:: OnGetProperties(
        __in  IWDFFile* appId,
        __in  LPWSTR SensorID,
        __in  IPortableDeviceKeyCollection* pKeys,
        __out IPortableDeviceValues** ppValues
        )
{
    // This code does not use or track the appId value.
    // However, OnGetSupportedSensorObjects calls through to this
    // function using a NULL value for appId. If you choose to 
    // to use or track the IWDFFile pointers passed from the platform,
    // you must be careful to check for NULL values.
    UNREFERENCED_PARAMETER(appId);

    HRESULT hr = S_OK;
    ObjectType objCaller = OBJECT_TYPE_LAST;
    BOOL fHasError = FALSE; // Tracks whether retrieving a property
                            // returned an error.

    // No need to check appId for NULL because
    // we do not use it in this implementation.
    if ((SensorID == NULL) ||
        (pKeys       == NULL) ||
        (ppValues     == NULL))
    {
        return  E_POINTER;
    }

    // Test for the correct sensor ID.
    if(wcsncmp(SensorID, g_wszSensorID, wcsnlen(SensorID, g_iMaxStrLen)) == 0)
    {
        objCaller = TIME_SENSOR;
    }
    else if(wcsncmp(SensorID, WPD_DEVICE_OBJECT_ID, wcsnlen(SensorID, g_iMaxStrLen)) == 0)
    {
        objCaller = DEVICE;
    }
    else
    {
        return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
    }

    // CoCreate a collection to store the property values.
    CComPtr<IPortableDeviceValues> spValues;
    hr = spValues.CoCreateInstance(CLSID_PortableDeviceValues);

    DWORD       cKeys       = 0;

    if(SUCCEEDED(hr))
    {
         hr = pKeys->GetCount(&cKeys);
    }

    if(SUCCEEDED(hr))
    {
        for (DWORD dwIndex = 0; dwIndex < cKeys; dwIndex++)
        {
            PROPERTYKEY Key = WPD_PROPERTY_NULL;
            hr = pKeys->GetAt(dwIndex, &Key);

            if(SUCCEEDED(hr))
            {
                if(TIME_SENSOR == objCaller)
                {
                    // Add the SENSOR property to the collection.
                    hr = GetSensorProperty(Key, spValues);
                }
                else if(DEVICE == objCaller)
                {
                    // Add the DEVICE property to the collection.
                    hr = GetDeviceProperty(Key, spValues);
                }
                else
                {
                    Trace(TRACE_LEVEL_ERROR, "%!FUNC!: Unexpected value for sensor ID.");
                    hr = E_UNEXPECTED;
                }
            }

            if(hr != S_OK)
            {
                // Set the failed HRESULT as the value for the property key.
                hr = spValues->SetErrorValue(Key, hr);
                fHasError = TRUE;
            }

        } // for loop: dwIndex < cKeys
    }

    // Return the results through the output parameter.
    *ppValues = spValues.Detach();


    // If we have set failures for the property get operations, we must let the application
    // know by returning S_FALSE. This will instruct the application to look at the
    // property get operation results for failure values.
    if( fHasError )
    {
        hr = S_FALSE;
    }

    return hr;
}

/////////////////////////////////////////////////////////////////////////
//
// CSensorDdi:: OnGetDataFields
//
// Implementation of the ISensorDriver method.
//
/////////////////////////////////////////////////////////////////////////
HRESULT CSensorDdi:: OnGetDataFields(
        __in  IWDFFile* appId,
        __in  LPWSTR SensorID,
        __in  IPortableDeviceKeyCollection* pKeys,
        __out IPortableDeviceValues** ppValues
        )
{
    // This code does not use or track the appId value.
    // However, the event loop calls through to this
    // function using a NULL value for appId. If you choose to 
    // to use or track the IWDFFile pointers passed from the platform,
    // you must be careful to check for NULL values.
    UNREFERENCED_PARAMETER(appId);

    HRESULT hr = S_OK;

    if ((SensorID == NULL) ||
        (pKeys       == NULL) ||
        (ppValues     == NULL))
    {
        return E_POINTER;
    }

    DWORD       cKeys       = 0;
    WORD        wHour       = 0;
    WORD        wMinute     = 0;
    WORD        wSecond     = 0;
    SYSTEMTIME  timestamp;
    ::ZeroMemory(&timestamp, sizeof(SYSTEMTIME));

    // This method is called by the class extension and
    // the event thread. We need a critical section to 
    // serialize access, here.
    Lock();

    // CoCreate a collection to store the property values.
    CComPtr<IPortableDeviceValues> spValues;
    hr = spValues.CoCreateInstance(CLSID_PortableDeviceValues);

    if(SUCCEEDED(hr))
    {
        GetNewDataFromDevice(&wHour, &wMinute, &wSecond, &timestamp);
    
        // Update the sensor state to READY
        if (TRUE == UpdateSensorState(SENSOR_STATE_READY))
        {
            if (NULL != m_pSampleEvents)
            {
                // Post the State Change event
                hr = m_pSampleEvents->PostStateEvent();
            }
        }
    }

    if(SUCCEEDED(hr))
    {
        hr = pKeys->GetCount(&cKeys);
    }

    if(SUCCEEDED(hr))
    {
        PROPVARIANT var;
        PropVariantInit( &var ); // The platform will free this PROPVARIANT.

        for (DWORD dwIndex = 0; dwIndex < cKeys; dwIndex++)
        {
            PROPERTYKEY Key = WPD_PROPERTY_NULL;

            hr = pKeys->GetAt(dwIndex, &Key);

            if(SUCCEEDED(hr))
            {
                // Preset the data field value to 'error not found'.  The actual value
                // will replace this value, if read from the device.
                hr = spValues->SetErrorValue(Key, HRESULT_FROM_WIN32(ERROR_NOT_FOUND));
            }

            if(SUCCEEDED(hr))
            {
                if (IsEqualPropertyKey(Key, SENSOR_DATA_TYPE_TIMESTAMP))
                {
                    // Convert the SYSTEMTIME into FILETIME
                    FILETIME ft;
                    if(FALSE == ::SystemTimeToFileTime(&timestamp, &ft))
                    {
                        hr = HRESULT_FROM_WIN32(GetLastError());
                    }

                    if(SUCCEEDED(hr))
                    {
                        var.vt                      = VT_FILETIME;
                        var.filetime.dwLowDateTime  = ft.dwLowDateTime;
                        var.filetime.dwHighDateTime = ft.dwHighDateTime;

                        hr = spValues->SetValue(SENSOR_DATA_TYPE_TIMESTAMP, &var);
                    }
                }
                else if(IsEqualPropertyKey(Key, SAMPLE_SENSOR_DATA_TYPE_HOUR))
                {
                    hr = spValues->SetUnsignedIntegerValue(SAMPLE_SENSOR_DATA_TYPE_HOUR, (ULONG)wHour);
                }
                else if(IsEqualPropertyKey(Key, SAMPLE_SENSOR_DATA_TYPE_MINUTE))
                {
                    hr = spValues->SetUnsignedIntegerValue(SAMPLE_SENSOR_DATA_TYPE_MINUTE, (ULONG)wMinute);
                }
                else if(IsEqualPropertyKey(Key, SAMPLE_SENSOR_DATA_TYPE_SECOND))
                {
                    hr = spValues->SetUnsignedIntegerValue(SAMPLE_SENSOR_DATA_TYPE_SECOND, (ULONG)wSecond);
                }
            }
        } // for loop: dwIndex < cKeys
    }

    if(SUCCEEDED(hr))
    {
        *ppValues = spValues.Detach();
    }

    Unlock();

    return hr;
}

/////////////////////////////////////////////////////////////////////////
//
// CSensorDdi:: OnSetProperties
//
// Implementation of the ISensorDriver method.
//
/////////////////////////////////////////////////////////////////////////
HRESULT CSensorDdi:: OnSetProperties(
        __in IWDFFile* appId,
        __in LPWSTR SensorID,
        __in IPortableDeviceValues* pValues,
        __out IPortableDeviceValues** ppResults
        )
{
    HRESULT hr = S_OK;
    DWORD   cValues     = 0;
    BOOL fHasError = FALSE;

    if ((SensorID == NULL) ||
        (pValues       == NULL) ||
        (ppResults     == NULL))
    {
        return E_POINTER;
    }

     if(wcsncmp(SensorID, g_wszSensorID, wcsnlen(SensorID, g_iMaxStrLen)) != 0)
     {
         return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
     }


    // CoCreate a collection to store the property set operation results.
    CComPtr<IPortableDeviceValues> spResults;
    hr = spResults.CoCreateInstance(CLSID_PortableDeviceValues);

    if(SUCCEEDED(hr))
    {
        hr = pValues->GetCount(&cValues);
    }

    if(SUCCEEDED(hr))
    {
        for (DWORD dwIndex = 0; dwIndex < cValues; dwIndex++)
        {
            PROPERTYKEY Key = WPD_PROPERTY_NULL;
            PROPVARIANT var;

            PropVariantInit(&var);

            hr = pValues->GetAt(dwIndex, &Key, &var);

            if(SUCCEEDED(hr))
            {
                //Check for properties that can be set.
                if( IsEqualPropertyKey( Key, SENSOR_PROPERTY_CURRENT_REPORT_INTERVAL ) )
                {
                    ClientData *pCD = NULL;

                    // Find the connected client info.
                    pCD = Clients.Lookup(appId);
                    if(NULL == pCD)
                    {
                        hr = E_UNEXPECTED;
                    }

                    if(SUCCEEDED(hr) && var.vt == VT_UI4)
                    {
                        // Update the client's information in the map.
                        if(var.ulVal == 0)
                        {
                            // Client requested the driver's default interval.
                            // Explicitly set the default value.
                            // Only freshly initialized ClientData structs should
                            // have a zero value for ulInterval. Zero is an indication
                            // that the client app never requested an interval.
                            pCD->ulInterval = g_dwDefaultInterval;
                        }
                        else
                        {
                            // Update the stored report interval for this client.
                            pCD->ulInterval = var.ulVal;
                        }

                        // Update the actual report interval.
                        m_ulReportInterval = GetNewReportInterval();

                        // Notify the event class about the new interval.
                        if(NULL != m_pSampleEvents)
                        {
                            m_pSampleEvents->SetInterval((DWORD)m_ulReportInterval);
                        }
                    }
                    else if( var.vt == VT_EMPTY )
                    {
                        //Do not allow deletion of the property
                        hr = spResults->SetErrorValue(Key, E_ACCESSDENIED);
                        fHasError = TRUE;
                    }
                    else
                    {
                        hr = spResults->SetErrorValue(Key, E_INVALIDARG);
                        fHasError = TRUE;
                    }
                }
                else
                {
                    // Cannot set an unsupported property.
                    hr = spResults->SetErrorValue(Key, HRESULT_FROM_WIN32(ERROR_NOT_FOUND));
                    fHasError = TRUE;
                }

                PropVariantClear(&var);
            }
        } // for loop: dwIndex < cValues
    }
  
    // Return the results through the output parameter.      
    *ppResults = spResults.Detach();
 
    // If we have set failures for the property set operations, we must let the application
    // know by returning S_FALSE. This will instruct the application to look at the
    // property set operation results for failure values.
    if( fHasError )
    {
        hr = S_FALSE;
    }

    return hr;
}

/////////////////////////////////////////////////////////////////////////
//
// CSensorDdi:: OnClientConnect
//
// Implementation of the ISensorDriver method.
//
/////////////////////////////////////////////////////////////////////////
HRESULT CSensorDdi:: OnClientConnect(
        __in IWDFFile* pClientFile,
        __in LPWSTR pwszSensorID
        )
{
    if(NULL == pClientFile ||
       NULL == pwszSensorID)
    {
        return E_POINTER;
    }

    HRESULT hr = S_OK;
    BOOL bRet = FALSE;

    // PFD ISSUE: False positive possibly leaking memory
    // PreFast is not able to detect the fact that the new ClientData object
    // is actually being freed in OnClientDisconnect.
    // Since the ATL Simple Map class (Clients) does not have the right SAL annotations,
    // PreFast will raise a warning. We are suppressing the warning here.

    #pragma warning (push)
    #pragma warning (disable:28197) 

    // This memory freed in OnClientDisconnect.
    ClientData* cd = new ClientData();
    
    #pragma warning (pop)
    
    if(NULL == cd)
    {
        hr = E_OUTOFMEMORY;
    }

    if(SUCCEEDED(hr))
    {
        ::ZeroMemory(cd, sizeof(ClientData));

        // Add this client to the map.
        // Use the current report interval as the default.
        bRet = Clients.Add(pClientFile, cd);
        if(bRet == FALSE)
        {
            hr = E_OUTOFMEMORY;
        }
    }

    return hr;
}

/////////////////////////////////////////////////////////////////////////
//
// CSensorDdi:: OnClientDisconnect
//
// Implementation of the ISensorDriver method.
//
/////////////////////////////////////////////////////////////////////////
HRESULT CSensorDdi:: OnClientDisconnect(
        __in IWDFFile* pClientFile,
        __in LPWSTR pwszSensorID
        )
{
    if(NULL == pClientFile ||
       NULL == pwszSensorID)
    {
        return E_POINTER;
    }

    HRESULT hr = S_OK;
    ClientData* pCD = NULL;
    BOOL bRet = FALSE;

    // Find this client in the map.
    pCD = Clients.Lookup(pClientFile);
    if(pCD == NULL)
    {
        hr = E_UNEXPECTED;
    }

    if(SUCCEEDED(hr))
    {
        // Free the client data memory.
        delete pCD;
        pCD = NULL;

        // Remove this client from the array.
        bRet = Clients.Remove(pClientFile);
        if(FALSE == bRet)
        {
            hr = E_UNEXPECTED;
        }
    }

    return hr;
}

/////////////////////////////////////////////////////////////////////////
//
// CSensorDdi:: OnClientSubscribeToEvents
//
// Implementation of the ISensorDriver method.
//
/////////////////////////////////////////////////////////////////////////
HRESULT CSensorDdi::OnClientSubscribeToEvents(
        __in IWDFFile* pClientFile,
        __in LPWSTR pwszSensorID
        )
{
    if(NULL == pClientFile ||
       NULL == pwszSensorID)
    {
        return E_POINTER;
    }

    HRESULT hr = S_OK;
    ClientData* pCD = NULL;

    if(m_cEventClients == 0)
    {
        // Create the event class.
        m_pSampleEvents = new(CSampleEvents);

        if(NULL == m_pSampleEvents)
        {
            hr = E_OUTOFMEMORY;
        }

        if(SUCCEEDED(hr))
        {
            // Initialize the event class.
            hr = m_pSampleEvents->Initialize(m_spSensorCXT, this, m_ulReportInterval);
        }
    }

    m_cEventClients++;

    pCD = Clients.Lookup(pClientFile);
    if(NULL == pCD)
    {
        hr = E_UNEXPECTED;
    }

    if(SUCCEEDED(hr))
    {
        // Mark this client as an event listener.
        pCD->bListening = TRUE;

        // Update the actual report interval.

        // For this sample, we'll just use the helper function to choose the 
        // the correct interval. 

        // You could instead choose to compare the interval requested by
        // the subscribing app to the current interval and select the shorter
        // of the two. In that case, you will also need to:
        // - Check for zero-report-interval values. Zero indicates that the app
        //   never requested a value.
        // - Check whether the current interval is also the default. 
        // - Track why the default interval was set. If it's the default, did
        //   a client app explicitly request the default? Was the default set
        //   because no apps requested an explicit interval? Or did an app just
        //   happen to set the same value as the default?
       
        m_ulReportInterval = GetNewReportInterval();

        // Notify the event class about the new interval.
        if(NULL != m_pSampleEvents)
        {
            m_pSampleEvents->SetInterval((DWORD)m_ulReportInterval);
        }
    }

    return hr;
}

/////////////////////////////////////////////////////////////////////////
//
// CSensorDdi:: OnClientUnsubscribeFromEvents
//
// Implementation of the ISensorDriver method.
//
/////////////////////////////////////////////////////////////////////////
HRESULT CSensorDdi::OnClientUnsubscribeFromEvents(
        __in IWDFFile* pClientFile,
        __in LPWSTR pwszSensorID
        )
{
    if(NULL == pClientFile ||
       NULL == pwszSensorID)
    {
        return E_POINTER;
    }

    HRESULT hr = S_OK;
    ClientData* pCD = NULL;
    BOOL bCurrent = FALSE;

    if(m_cEventClients > 0)
    {
        // Decrement the count of event listeners.
        m_cEventClients--;
    }

    // Find the client that is unsubscribing.
    pCD = Clients.Lookup(pClientFile);
    if(NULL == pCD)
    {
        hr = E_UNEXPECTED;
    }

    if(SUCCEEDED(hr))
    {
        // Unmark this client as an event listener.
        pCD->bListening = FALSE;
    }

    if(SUCCEEDED(hr))
    {
        // Test whether the current report interval equals
        // the one set by this client.
        // There may be multiple clients that requested the
        // same interval, but we have no way of knowing that.
        if(pCD->ulInterval == m_ulReportInterval)
        {
            bCurrent = TRUE;
        }

        // Update the actual report interval.
        m_ulReportInterval = GetNewReportInterval();
    }

    if(m_cEventClients == 0 &&
       m_pSampleEvents != NULL)
    {
        // No clients listening to events.
        // This case will also occur when the class extension
        // shuts down.
        hr = m_pSampleEvents->Uninitialize();

        // Destroy the event class.
        delete m_pSampleEvents;
        m_pSampleEvents = NULL;

        // Return to the default interval.
        m_ulReportInterval = (ULONG) g_dwDefaultInterval;
    }
    else if(SUCCEEDED(hr) && bCurrent == TRUE)
    {
        // Notify the event class about the new interval.
        if(NULL != m_pSampleEvents)
        {
            Lock();

            m_pSampleEvents->SetInterval((DWORD)m_ulReportInterval);

            Unlock();
        }
    }

    return hr;
}

/////////////////////////////////////////////////////////////////////////
//
// CSensorDdi:: OnProcessWpdMessage
//
// Implementation of the ISensorDriver method.
//
// This driver does not process WPD messages.
//
/////////////////////////////////////////////////////////////////////////
HRESULT CSensorDdi:: OnProcessWpdMessage(
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
// CSensorDdi::SetSensorClassXT
//
// Custom method to receive a pointer to the sensor class extension.
//
// This pointer is provided from CMyDevice::OnPrepareHardware after the
// sensor class extension is created.
//
/////////////////////////////////////////////////////////////////////////
HRESULT CSensorDdi::SetSensorClassXT(ISensorClassExtension *pCXT)
{
    HRESULT hr = S_OK;

    if(NULL == pCXT)
    {
        return E_POINTER;
    }

    m_spSensorCXT = pCXT;

    return hr;
}

/////////////////////////////////////////////////////////////////////////
//
// CSensorDdi::SetPUID
//
// Receives the persistent unique identifier. This method is called
// from CMyDevice::OnPrepareHardware.
//
/////////////////////////////////////////////////////////////////////////
HRESULT CSensorDdi::SetPUID(GUID puid)
{
    HRESULT hr = S_OK;

    if(puid == GUID_NULL)
    {
        return E_INVALIDARG;
    }

    m_guidPUID = puid;

    return hr;
}

/////////////////////////////////////////////////////////////////////////
//
// CSensorDdi::GetSensorState
//
// Retrieves the current state of the sensor. This method is called 
// whenever we want to post a state change event from
// CSampleEvents::PostStateEvent
//
/////////////////////////////////////////////////////////////////////////
HRESULT CSensorDdi::GetSensorState(SensorState* pState)
{
    HRESULT hr = (NULL == pState) ? E_INVALIDARG : S_OK;

    if (SUCCEEDED(hr))
    {
        *pState = m_state;
    }

    return hr;
}

/////////////////////////////////////////////////////////////////////////
//
// CSensorDdi::UpdateSensorState
//
// Updates the current state of the sensor. This method is called 
// whenever we want to update the sensor state
//
/////////////////////////////////////////////////////////////////////////
BOOL CSensorDdi::UpdateSensorState(SensorState state)
{
    BOOL fStateChanged = FALSE;

    if ( m_state != state )
    {
        m_state = state;
        fStateChanged = TRUE;
    }

    return fStateChanged;
}


/////////////////////////////////////////////////////////////////////////
//
// CSensorDdi::GetNewDataFromDevice
//
// This method simulates retrieving data from a hardware device.
//
/////////////////////////////////////////////////////////////////////////
void CSensorDdi::GetNewDataFromDevice(WORD* wHour, WORD* wMinute, WORD* wSecond,
                                            SYSTEMTIME* timestamp)
{
    if(wHour == NULL ||
        wMinute == NULL || 
        wSecond == NULL ||
        timestamp == NULL)
    {
        Trace(TRACE_LEVEL_ERROR, "%!FUNC!: NULL pointer in helper function.");
        return;
    }

    // The "sensor" here is simply the system clock.
    SYSTEMTIME st;

    ::GetLocalTime(&st);

    *wHour = st.wHour;
    *wMinute = st.wMinute;
    *wSecond = st.wSecond;

    // Get the time stamp.
    ::GetSystemTime(timestamp);
}

/////////////////////////////////////////////////////////////////////////
//
// CSensorDdi::GetDeviceProperty
//
// Helper method to retrieve a particular device property.
//
// Parameters:
//				[in]  Key - the name of the property to retrieve.
//				[out] pValues - pointer to the values collection to which
//								to add the requested value.
//
/////////////////////////////////////////////////////////////////////////
HRESULT CSensorDdi::GetDeviceProperty(PROPERTYKEY Key, IPortableDeviceValues* pValues)
{
    // The lifetime of the pointer passed into this function must stay entirely within
    // the lifetime of the calling function. Therefore, we won't addref or release this pointer.
    
    if(NULL == pValues)
    {
        Trace(TRACE_LEVEL_ERROR, "%!FUNC!: NULL pointer in helper function.");
        return E_POINTER;
    }

    HRESULT hr = S_OK;

    // Set DEVICE object properties
    if (IsEqualPropertyKey(Key, WPD_DEVICE_FIRMWARE_VERSION))
    {
        hr = pValues->SetStringValue(WPD_DEVICE_FIRMWARE_VERSION, g_wszDeviceFirmareVersion);
    }

    else if (IsEqualPropertyKey(Key, WPD_DEVICE_POWER_LEVEL))
    {
        hr = pValues->SetUnsignedIntegerValue(WPD_DEVICE_POWER_LEVEL, g_iDevicePowerLevel);
    }

    else if (IsEqualPropertyKey(Key, WPD_DEVICE_POWER_SOURCE))
    {
        hr = pValues->SetUnsignedIntegerValue(WPD_DEVICE_POWER_SOURCE, WPD_POWER_SOURCE_EXTERNAL);
    }

    else if (IsEqualPropertyKey(Key, WPD_DEVICE_PROTOCOL))
    {
        hr = pValues->SetStringValue(WPD_DEVICE_PROTOCOL, g_wszDeviceProtocol);
    }

    else if (IsEqualPropertyKey(Key, WPD_DEVICE_MODEL))
    {
        hr = pValues->SetStringValue(WPD_DEVICE_MODEL, g_wszDeviceModel);
    }

    else if (IsEqualPropertyKey(Key, WPD_DEVICE_SERIAL_NUMBER))
    {
        hr = pValues->SetStringValue(WPD_DEVICE_SERIAL_NUMBER, g_wszDeviceSerialNumber);
    }

    else if (IsEqualPropertyKey(Key, WPD_DEVICE_SUPPORTS_NON_CONSUMABLE))
    {
        hr = pValues->SetBoolValue(WPD_DEVICE_SUPPORTS_NON_CONSUMABLE, g_bDeviceSupportsNonconsumable);
    }

    else if (IsEqualPropertyKey(Key, WPD_DEVICE_MANUFACTURER))
    {
        hr = pValues->SetStringValue(WPD_DEVICE_MANUFACTURER, g_wszDeviceManufacturer);
    }

    else if (IsEqualPropertyKey(Key, WPD_DEVICE_FRIENDLY_NAME))
    {
        hr = pValues->SetStringValue(WPD_DEVICE_FRIENDLY_NAME, g_wszSensorFriendlyName);
    }

    else if (IsEqualPropertyKey(Key, WPD_DEVICE_TYPE))
    {
        hr = pValues->SetUnsignedIntegerValue(WPD_DEVICE_TYPE, WPD_DEVICE_TYPE_GENERIC);
    }

    // Set general properties for DEVICE
    else if (IsEqualPropertyKey(Key, WPD_OBJECT_ID))
    {
        hr = pValues->SetStringValue(WPD_OBJECT_ID, WPD_DEVICE_OBJECT_ID);
    }

    else if (IsEqualPropertyKey(Key, WPD_OBJECT_NAME))
    {
        hr = pValues->SetStringValue(WPD_OBJECT_NAME, WPD_DEVICE_OBJECT_ID);
    }

    else if (IsEqualPropertyKey(Key, WPD_OBJECT_PERSISTENT_UNIQUE_ID))
    {
        hr = pValues->SetStringValue(WPD_OBJECT_PERSISTENT_UNIQUE_ID, WPD_DEVICE_OBJECT_ID);
    }

    else if (IsEqualPropertyKey(Key, WPD_OBJECT_PARENT_ID))
    {
        hr = pValues->SetStringValue(WPD_OBJECT_PARENT_ID, L"");
    }

    else if (IsEqualPropertyKey(Key, WPD_OBJECT_FORMAT))
    {
        hr = pValues->SetGuidValue(WPD_OBJECT_FORMAT, WPD_OBJECT_FORMAT_UNSPECIFIED);
    }

    else if (IsEqualPropertyKey(Key, WPD_OBJECT_CONTENT_TYPE))
    {
        hr = pValues->SetGuidValue(WPD_OBJECT_CONTENT_TYPE, WPD_CONTENT_TYPE_FUNCTIONAL_OBJECT);
    }

    else if (IsEqualPropertyKey(Key, WPD_OBJECT_CAN_DELETE))
    {
        hr = pValues->SetBoolValue(WPD_OBJECT_CAN_DELETE, FALSE);
    }

    else if (IsEqualPropertyKey(Key, WPD_FUNCTIONAL_OBJECT_CATEGORY))
    {
        hr = pValues->SetGuidValue(WPD_FUNCTIONAL_OBJECT_CATEGORY, WPD_FUNCTIONAL_CATEGORY_DEVICE);
    }

    else
    {
        hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    } 

    return hr;
}

/////////////////////////////////////////////////////////////////////////
//
// CSensorDdi::GetSensorProperty
//
// Helper method to retrieve a particular sensor property.
//
// Parameters:
//				[in]  Key - the name of the property to retrieve.
//				[out] pValues - pointer to the values collection to which
//								to add the requested value.
//
/////////////////////////////////////////////////////////////////////////
HRESULT CSensorDdi::GetSensorProperty(PROPERTYKEY Key, IPortableDeviceValues* pValues)
{
    // The lifetime of the pointers passed into this function must stay entirely within
    // the lifetime of the calling function. Therefore, we won't addref or release these pointers.
    if(pValues == NULL)
    {
        Trace(TRACE_LEVEL_ERROR, "%!FUNC!: NULL pointer in helper function.");
        return E_POINTER;
    }

    HRESULT hr = S_OK;

    // Return the SENSOR property.
    if (IsEqualPropertyKey(Key, SENSOR_PROPERTY_TYPE))
    {
        hr = pValues->SetGuidValue(SENSOR_PROPERTY_TYPE, SAMPLE_SENSOR_TYPE_TIME);
    }

    else if (IsEqualPropertyKey(Key, SENSOR_PROPERTY_STATE))
    {
        hr = pValues->SetUnsignedIntegerValue(SENSOR_PROPERTY_STATE, m_state);
    }

    else if (IsEqualPropertyKey(Key, SENSOR_PROPERTY_MIN_REPORT_INTERVAL))
    {
        hr = pValues->SetUnsignedIntegerValue(SENSOR_PROPERTY_MIN_REPORT_INTERVAL, g_dwMinInterval);
    }

    else if (IsEqualPropertyKey(Key, SENSOR_PROPERTY_CURRENT_REPORT_INTERVAL))
    {
        hr = pValues->SetUnsignedIntegerValue(SENSOR_PROPERTY_CURRENT_REPORT_INTERVAL, m_ulReportInterval);
    }

    else if (IsEqualPropertyKey(Key, SENSOR_PROPERTY_PERSISTENT_UNIQUE_ID))
    {
        hr = pValues->SetGuidValue(SENSOR_PROPERTY_PERSISTENT_UNIQUE_ID, m_guidPUID);
    }

    else if (IsEqualPropertyKey(Key, SENSOR_PROPERTY_MANUFACTURER))
    {
        hr = pValues->SetStringValue(SENSOR_PROPERTY_MANUFACTURER, g_wszDeviceManufacturer);
    }

    else if (IsEqualPropertyKey(Key, SENSOR_PROPERTY_MODEL))
    {
        hr = pValues->SetStringValue(SENSOR_PROPERTY_MODEL, g_wszDeviceModel);
    }

    else if (IsEqualPropertyKey(Key, SENSOR_PROPERTY_SERIAL_NUMBER))
    {
        hr = pValues->SetStringValue(SENSOR_PROPERTY_SERIAL_NUMBER, g_wszDeviceSerialNumber);
    }

    else if (IsEqualPropertyKey(Key, SENSOR_PROPERTY_FRIENDLY_NAME))
    {
        hr = pValues->SetStringValue(SENSOR_PROPERTY_FRIENDLY_NAME, g_wszSensorFriendlyName);
    }

    else if (IsEqualPropertyKey(Key, SENSOR_PROPERTY_DESCRIPTION))
    {
        hr = pValues->SetStringValue(SENSOR_PROPERTY_DESCRIPTION, g_wszSensorDescription);
    }

    else if (IsEqualPropertyKey(Key, SENSOR_PROPERTY_CONNECTION_TYPE))
    {
        hr = pValues->SetUnsignedIntegerValue(SENSOR_PROPERTY_CONNECTION_TYPE, SENSOR_CONNECTION_TYPE_PC_INTEGRATED);
    }

    // Set general properties for sensor
    else if (IsEqualPropertyKey(Key, WPD_OBJECT_ID))
    {
        hr = pValues->SetStringValue(WPD_OBJECT_ID, g_wszSensorID);
    }

    else if (IsEqualPropertyKey(Key, WPD_OBJECT_NAME))
    {
        hr = pValues->SetStringValue(WPD_OBJECT_NAME, g_wszSensorID);
    }

    else if (IsEqualPropertyKey(Key, WPD_OBJECT_PERSISTENT_UNIQUE_ID))
    {
        hr = pValues->SetStringValue(WPD_OBJECT_PERSISTENT_UNIQUE_ID, g_wszSensorID);
    }

    else if (IsEqualPropertyKey(Key, WPD_OBJECT_PARENT_ID))
    {
        hr = pValues->SetStringValue(WPD_OBJECT_PARENT_ID, WPD_DEVICE_OBJECT_ID);
    }

    else if (IsEqualPropertyKey(Key, WPD_OBJECT_FORMAT))
    {
        hr = pValues->SetGuidValue(WPD_OBJECT_FORMAT, WPD_OBJECT_FORMAT_UNSPECIFIED);
    }

    else if (IsEqualPropertyKey(Key, WPD_OBJECT_CONTENT_TYPE))
    {
        hr = pValues->SetGuidValue(WPD_OBJECT_CONTENT_TYPE, WPD_CONTENT_TYPE_FUNCTIONAL_OBJECT);
    }

    else if (IsEqualPropertyKey(Key, WPD_OBJECT_CAN_DELETE))
    {
        hr = pValues->SetBoolValue(WPD_OBJECT_CAN_DELETE, FALSE);
    }

    else if (IsEqualPropertyKey(Key, WPD_FUNCTIONAL_OBJECT_CATEGORY))
    {
        hr = pValues->SetGuidValue(WPD_FUNCTIONAL_OBJECT_CATEGORY, SAMPLE_SENSOR_CATEGORY_DATE_TIME);
    }

    else
    {
        hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    }

    return hr;
}

/////////////////////////////////////////////////////////////////////////
//
// CSensorDdi::GetNewReportInterval
//
// Returns the shortest report interval suggested from
//     the list of connected clients.
//
/////////////////////////////////////////////////////////////////////////
ULONG CSensorDdi::GetNewReportInterval()
{
    int iSize = Clients.GetSize();

    ULONG temp = 0;

    // Find the shortest interval stored in the array.
    for(int i = 0; i < iSize; i++)
    {
        ClientData* pCurrent = Clients.GetValueAt(i);

        // We choose the shortest interval
        // from the clients that are listening to events
        // and have explicitly requested an interval.

        // Note that clients that have explicitly requested an interval
        // should never have a zero value for ulInterval. For clients that
        // requested interval zero (an explicit request for the driver's
        // default interval), ulInterval will equal the default, not zero.
        // See the implementation of OnSetProperties.

        // A zero value for ulInterval means no value has ever been set by the client.

        // Also note that once a client has requested any interval, they can't
        // un-request, only change to another explicit value (including default).

        if(pCurrent->bListening == TRUE && // listening to events
           pCurrent->ulInterval != 0 && // set an interval at some time in the past
           (0 == temp || pCurrent->ulInterval < temp)) // shortest valid interval
        {
            temp = pCurrent->ulInterval;
        }
    }

    // Never return zero. Zero means use the default because no value is set.
    return 0 == temp ? g_dwDefaultInterval : temp;
}
