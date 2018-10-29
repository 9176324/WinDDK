/*++

// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.

Copyright (C) Microsoft Corporation, All Rights Reserved

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
    m_pClassExtension(NULL)
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

    // Store the IWDFDevice pointer
    m_pWdfDevice = pWdfDevice;

    // Configure the default IO Queue
    HRESULT hr = ConfigureQueue();

    if (SUCCEEDED(hr))
    {
        CComObject<CSensorDDI>* pSensorDdi = NULL;
        hr = CComObject<CSensorDDI>::CreateInstance(&pSensorDdi);

        if (SUCCEEDED(hr))
        {
            // AddRef after CreateInstance
            pSensorDdi->AddRef();

            // Initialize the DDI
            hr = pSensorDdi->Initialize(pWdfDevice);

            CComPtr<IUnknown> spUnknown;
            if (SUCCEEDED(hr))
            {
                hr = pSensorDdi->QueryInterface(IID_IUnknown, (void**)&spUnknown);
            }

            if (SUCCEEDED(hr))
            {
                hr = CoCreateInstance(CLSID_SensorClassExtension,
                                      NULL,
                                      CLSCTX_INPROC_SERVER,
                                      __uuidof(ISensorClassExtension),
                                      (VOID**)&m_pClassExtension);
                
                if (SUCCEEDED(hr))
                {
                    hr = m_pClassExtension->Initialize(pWdfDevice, spUnknown);
                }

                // Release the pSensorDdi pointer when done. Note: Sensor Class Extension holds a reference to it above
                pSensorDdi->Release();
            }
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
    
    // Uninitialize & Release the Class Extension
    if (m_pClassExtension != NULL)
    {
        m_pClassExtension->Uninitialize();
        
        SAFE_RELEASE(m_pClassExtension);
    }

    // Release the IWDFDevice handle, if it matches
    if (pWdfDevice == m_pWdfDevice.p)
    {
        m_pWdfDevice.Release();
    }

    return S_OK;
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

