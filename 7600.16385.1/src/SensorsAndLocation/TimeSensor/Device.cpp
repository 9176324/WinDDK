/*++

// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
 
Copyright (C) Microsoft Corporation, All Rights Reserved.

Module Name:

    Device.cpp

Abstract:

    This module contains the implementation of the Sensor Skeleton sample's
    device callback object.

--*/

#include "internal.h"
#include "Device.h"
#include "Queue.h"
#include "SensorDdi.h"
#include "SampleEvents.h"

#include "device.tmh"

/////////////////////////////////////////////////////////////////////////
//
// CMyDevice::CMyDevice
//
// Object constructor function
//
/////////////////////////////////////////////////////////////////////////
CMyDevice::CMyDevice() :
    m_pWdfDevice(NULL),
    m_pClassExtension(NULL),
    m_pSampleEvents(NULL), 
    m_pSensorDdi(NULL)
{

}

/////////////////////////////////////////////////////////////////////////
//
// CMyDevice::~CMyDevice
//
// Object destructor function
//
/////////////////////////////////////////////////////////////////////////
CMyDevice::~CMyDevice()
{
    SAFE_RELEASE(m_pClassExtension);
}

/////////////////////////////////////////////////////////////////////////
//
// CMyDevice::CreateInstance
//
// This static method is used to create and initialize an instance of
// CMyDevice for use with a given hardware device.
//
// Parameters:
//      
//      pDeviceInit  - pointer to an interface used to intialize the device
//      pDriver      - pointer to an IWDFDriver interface
//
// Return Values:
//      S_OK: object created successfully
//
/////////////////////////////////////////////////////////////////////////
HRESULT CMyDevice::CreateInstance(
    __in  IWDFDriver*           pDriver,
    __in  IWDFDeviceInitialize* pDeviceInit
    )
{
    CComObject<CMyDevice>* pMyDevice = NULL;

    HRESULT hr = CComObject<CMyDevice>::CreateInstance(&pMyDevice);

    if (SUCCEEDED(hr))
    {
        pMyDevice->AddRef();
    
        // Prepare device parameters
        pDeviceInit->SetLockingConstraint(WdfDeviceLevel);  // The skeleton uses device level locking
        pDeviceInit->SetPowerPolicyOwnership(FALSE);        // Power policy
        // pDeviceInit->SetFilter();                        // If you're writing a filter driver then set this flag

        CComPtr<IUnknown> spCallback;
        hr = pMyDevice->QueryInterface(IID_IUnknown, (void**)&spCallback);

        CComPtr<IWDFDevice> spIWDFDevice;
        if (SUCCEEDED(hr))
        {
            // Create the IWDFDevice object
            hr = pDriver->CreateDevice(pDeviceInit, spCallback, &spIWDFDevice);
        }

        // Release the pMyDevice pointer when done. Note: UMDF holds a reference to it above
        SAFE_RELEASE(pMyDevice);
    }

    return hr;
}

/////////////////////////////////////////////////////////////////////////
//
// CMyDevice::ConfigureQueue
//  
//  This method is called after the device callback object has been initialized 
//  and returned to the driver.  It would setup the device's queues and their 
//  corresponding callback objects.
//
// Parameters:
//
// Return Values:
//      S_OK: success
//
/////////////////////////////////////////////////////////////////////////
HRESULT CMyDevice::ConfigureQueue()
{
    HRESULT hr = CMyQueue::CreateInstance(m_pWdfDevice, this);

    return hr;
}

/////////////////////////////////////////////////////////////////////////
//
// CMyDevice::OnPrepareHardware
//
//  Called by UMDF to prepare the hardware for use. In our case
//  we create the SensorDdi object and initialize the Sensor Class Extension
//
// Parameters:
//      pWdfDevice - pointer to an IWDFDevice object representing the
//      device
//
// Return Values:
//      S_OK: success
//
/////////////////////////////////////////////////////////////////////////
HRESULT CMyDevice::OnPrepareHardware(
        __in IWDFDevice* pWdfDevice
        )
{
    Trace(TRACE_LEVEL_INFORMATION, "%!FUNC! Entry");

    GUID guidPUID = GUID_NULL;

    // Store the IWDFDevice pointer
    m_pWdfDevice = pWdfDevice;

    // Configure the default IO Queue
    HRESULT hr = ConfigureQueue();

    if(SUCCEEDED(hr))
    {
        
        hr = CComObject<CSensorDdi>::CreateInstance(&m_pSensorDdi);

        if(SUCCEEDED(hr))
        {
            m_pSensorDdi->AddRef();

            CComPtr<IUnknown> spUnknown;
            hr = m_pSensorDdi->QueryInterface(IID_IUnknown, (void**)&spUnknown);

            if( SUCCEEDED(hr))
            {
                hr = CoCreateInstance(CLSID_SensorClassExtension,
                                      NULL,
                                      CLSCTX_INPROC_SERVER,
                                      __uuidof(ISensorClassExtension),
                                      (VOID**)&m_pClassExtension);
                
                if(SUCCEEDED(hr))
                {
                    hr = m_pClassExtension->Initialize(pWdfDevice, spUnknown);
                }
            }
        }

		// You might choose to initialize the event class here 
		// instead of in CSensorDdi::OnClientSubscribeToEvents.
		// However, doing so may cause greater power consumption
		// in the device.

		if(SUCCEEDED(hr))
		{
			hr = m_pSensorDdi->SetSensorClassXT(m_pClassExtension);
		}

        if(SUCCEEDED(hr))
        {
            hr = SetUniqueID(pWdfDevice, &guidPUID);
        }

        if(SUCCEEDED(hr))
        {
            hr = m_pSensorDdi->SetPUID(guidPUID);
        }
    }

    return hr;
}


/////////////////////////////////////////////////////////////////////////
//
// CMyDevice::OnReleaseHardware
//
// Called by WUDF to uninitialize the hardware.
//
// Parameters:
//      pWdfDevice - pointer to an IWDFDevice object for the device
//
// Return Values:
//      S_OK:
//
/////////////////////////////////////////////////////////////////////////
HRESULT CMyDevice::OnReleaseHardware(
        __in IWDFDevice* pWdfDevice
        )
{
    UNREFERENCED_PARAMETER(pWdfDevice);

	HRESULT hr = S_OK;

    
    // Uninitialize and release the Class Extension
	// When the class extension shuts down, it will
	// unsubscribe all the event listeners, which will
	// cause the event class to be destroyed.
    if (m_pClassExtension != NULL)
    {
        hr = m_pClassExtension->Uninitialize();
        
        SAFE_RELEASE(m_pClassExtension);
    }

	// Release the DDI class
	if(m_pSensorDdi != NULL)
	{
		m_pSensorDdi->Release();
		m_pSensorDdi = NULL;
	}

    // Release the IWDFDevice handle, if it matches
    if (pWdfDevice == m_pWdfDevice.p)
    {
        m_pWdfDevice.Release();
    }

    return hr;
}

/////////////////////////////////////////////////////////////////////////
//
// CMyDevice::OnCleanupFile
//
// This method is called when the file handle to the device is closed
//
// Parameters:
//      pWdfFile - pointer to a file object
//
/////////////////////////////////////////////////////////////////////////
VOID CMyDevice::OnCleanupFile(
            __in IWDFFile* pWdfFile
            )
{
    if (NULL != m_pClassExtension)
    {
        m_pClassExtension->CleanupFile(pWdfFile);
    }

    return;
}

/////////////////////////////////////////////////////////////////////////
//
// CMyDevice::ProcessIoControl
//
// This method is a helper that takes the incoming IOCTL and forwards
// it to the Windows Sensor Class Extension for processing.
//
// Parameters:
//      pQueue                  - [in] pointer to the UMDF queue that handled the request
//      pRequest                - [in] pointer to the request
//      ControlCode             - [in] the IOCTL code
//      InputBufferSizeInBytes  - [in] size of the incoming IOCTL buffer
//      OutputBufferSizeInBytes - [out] size of the outgoing IOCTL buffer
//      pcbWritten              - pointer to a DWORD containing the number of bytes returned
//
// Return Values:
//      S_OK:
//
/////////////////////////////////////////////////////////////////////////
HRESULT CMyDevice::ProcessIoControl(__in IWDFIoQueue*     pQueue,
                                    __in IWDFIoRequest*   pRequest,
                                    __in ULONG            ControlCode,
                                         SIZE_T           InputBufferSizeInBytes,
                                         SIZE_T           OutputBufferSizeInBytes,
                                         DWORD*           pcbWritten)
{
    UNREFERENCED_PARAMETER(pQueue);
    UNREFERENCED_PARAMETER(ControlCode);
    UNREFERENCED_PARAMETER(InputBufferSizeInBytes);
    UNREFERENCED_PARAMETER(OutputBufferSizeInBytes);
    UNREFERENCED_PARAMETER(pcbWritten);

    if (NULL == m_pClassExtension)
    {
        return E_UNEXPECTED;
    }
     
    HRESULT hr = m_pClassExtension->ProcessIoControl(pRequest);

    return hr;
}

// Sets the persistent unique ID property in the WDF property store
// and returns the GUID for use in PortableDeviceValues property bags.
HRESULT CMyDevice::SetUniqueID(__in IWDFDevice* pWdfDevice, __out GUID* puid)
{
    HRESULT hr = S_OK;

    if(pWdfDevice == NULL ||
       puid == NULL)
    {
        Trace(TRACE_LEVEL_ERROR, "%!FUNC!: Invalid argument or NULL pointer in helper function.");
        return E_INVALIDARG;
    }

    CComPtr<IWDFNamedPropertyStore> spPropStore;
    if (SUCCEEDED(hr))
    {
        hr = pWdfDevice->RetrieveDevicePropertyStore(NULL, WdfPropertyStoreCreateIfMissing, &spPropStore, NULL);
    }

    if(SUCCEEDED(hr))
    {
        GUID idGuid;
        
        PROPVARIANT vID;
        hr = spPropStore->GetNamedValue(g_wszSensorID, &vID);
        if (SUCCEEDED(hr))
        {
            hr = ::CLSIDFromString(vID.bstrVal, &idGuid);
        }
        else
        {
            hr = ::CoCreateGuid(&idGuid);
            if (SUCCEEDED(hr))
            {
                LPOLESTR lpszGUID = NULL;
                hr = ::StringFromCLSID(idGuid, &lpszGUID);
                if (SUCCEEDED(hr))
                {
                    PropVariantInit(&vID);
                    vID.vt = VT_LPWSTR;
                    vID.pwszVal = lpszGUID;
                    hr = spPropStore->SetNamedValue(g_wszSensorID, &vID);
                    PropVariantClear(&vID);
                }
            }
        }

        // Return the GUID.
        *puid = idGuid;
    }

    return hr;
}



