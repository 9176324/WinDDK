/*++

Copyright (C) Microsoft Corporation, All Rights Reserved.

Module Name:

    Device.cpp

Abstract:

    This module contains the implementation of the UMDF socketecho sample
    driver's device callback object.

    It does not implement either of the PNP interfaces so once the device
    is setup, it won't ever get any callbacks until the device is removed.

Environment:

   Windows User-Mode Driver Framework (WUDF)

--*/

#include "internal.h"
#include "device.tmh"

const GUID GUID_DEVINTERFACE_SOCKETECHO =
  {0xcdc35b6e, 0xbe4, 0x4936, { 0xbf, 0x5f, 0x55, 0x37, 0x38, 0xa, 0x7c, 0x1a }};


HRESULT
CMyDevice::Initialize(
    __in IWDFDriver* FxDriver,
    __in IWDFDeviceInitialize* FxDeviceInit
    )
/*++

  Routine Description:

    This method initializes the device callback object and creates the
    partner device object.

    The method should perform any device-specific configuration that:
        *  could fail (these can't be done in the constructor)
        *  must be done before the partner object is created -or-
        *  can be done after the partner object is created and which aren't
           influenced by any device-level parameters the parent (the driver
           in this case) might set.

  Arguments:

    FxDeviceInit - the settings for this device.
    FxDriver    - IWDF Driver for this device.

  Return Value:

    status.

--*/
{
    Trace(TRACE_LEVEL_INFORMATION, "%!FUNC!");

    CComPtr<IWDFDevice> fxDevice;
    HRESULT hr;
    BOOL bFilter = FALSE;

    //
    // Configure things like the locking model before we go to create our
    // partner device.
    //

    //
    // Set the locking model
    //

    FxDeviceInit->SetLockingConstraint(None);

    //
    // Mark filter if we are a filter
    //

    if (bFilter)
    {
        FxDeviceInit->SetFilter();
    }

    //
    // TODO: Any per-device initialization which must be done before
    //       creating the partner object.
    //

    //
    // Create a new FX device object and assign the new callback object to
    // handle any device level events that occur.
    //

    //
    // QueryIUnknown references the IUnknown interface that it returns
    // (which is the same as referencing the device).  We pass that to
    // CreateDevice, which takes its own reference if everything works.
    //

    CComPtr<IUnknown> pUnk;
    HRESULT hrQI = this->QueryInterface(__uuidof(IUnknown),(void**)&pUnk);
    WUDF_SAMPLE_DRIVER_ASSERT(SUCCEEDED(hrQI));

    hr = FxDriver->CreateDevice(FxDeviceInit, pUnk, &fxDevice);

    //
    // If that succeeded then set our FxDevice member variable.
    //

    if (SUCCEEDED(hr))
    {
        m_FxDevice = fxDevice;
    }

    return hr;
}

HRESULT
CMyDevice::Configure(
    VOID
    )
/*++

  Routine Description:

    This method is called after the device callback object has been initialized
    and returned to the driver.  It would setup the device's queues and their
    corresponding callback objects.

  Arguments:

    FxDevice - the framework device object for which we're handling events.

  Return Value:

    status

--*/
{
    Trace(TRACE_LEVEL_INFORMATION, "%!FUNC!");

    HRESULT hr;
    CComObject<CMyQueue> * defaultQueue = NULL;

    //
    // Create a new instance of our device callback object
    //
    hr = CComObject<CMyQueue>::CreateInstance(&defaultQueue);

    if (SUCCEEDED(hr))
    {
        defaultQueue->AddRef();
        hr = defaultQueue->Initialize(this);
    }

    if (SUCCEEDED(hr))
    {
        hr = defaultQueue->Configure();
    }

    //
    // Create and Enable Device Interface for this device.
    //
    if (SUCCEEDED(hr))
    {
        hr = m_FxDevice->CreateDeviceInterface(&GUID_DEVINTERFACE_SOCKETECHO,
                                               NULL);
    }
    if (SUCCEEDED(hr))
    {
        hr = m_FxDevice->AssignDeviceInterfaceState(&GUID_DEVINTERFACE_SOCKETECHO,
                                                    NULL,
                                                    TRUE);
    }

    if (SUCCEEDED(hr))
    {
        hr = ReadAndAssignPropertyStoreValue();
    }

    if (FAILED(hr))
    {
        delete defaultQueue;
    }

    return hr;
}

STDMETHODIMP_(void)
CMyDevice::OnCloseFile(
    __in IWDFFile* pWdfFileObject
    )
/*++

  Routine Description:

    This method is called when an app closes the file handle to this device.
    This will free the context memory associated with this file object, close
    the connection object associated with this file object and delete the file
    handle i/o target object associated with this file object.

  Arguments:

    pWdfFileObject - the framework file object for which close is handled.

  Return Value:

    None

--*/
{
    Trace(TRACE_LEVEL_INFORMATION, "%!FUNC!");

    HRESULT hr = S_OK ;
    FileContext *pContext = NULL;

    hr = pWdfFileObject->RetrieveContext((void**)&pContext);

    if (SUCCEEDED(hr) && (pContext != NULL ) )
    {
        pContext->pConnection->Close();
        pContext->pFileTarget->DeleteWdfObject();

        delete pContext->pConnection;
        delete pContext;
    }

    return ;
}


STDMETHODIMP_(void)
CMyDevice::OnCleanupFile(
    __in IWDFFile* pWdfFileObject
    )
/*++

  Routine Description:

    This method is when app with open handle device terminates.

  Arguments:

    pWdfFileObject - the framework file object for which close is handled.

  Return Value:

    None

--*/
{
    UNREFERENCED_PARAMETER(pWdfFileObject);
}

STDMETHODIMP_(void)
CMyDevice::OnCleanup(
    __in IWDFObject* pWdfObject
    )
/*++

  Routine Description:

    This device callback method is invoked by the framework when the WdfObject
    is about to be released by the framework. This will free the context memory
    associated with the device object.

  Arguments:

    pWdfObject - the framework device object for which OnCleanup.

  Return Value:

    None

--*/
{
    Trace(TRACE_LEVEL_INFORMATION, "%!FUNC!");

    HRESULT hr ;
    DeviceContext *pContext = NULL;

    WUDF_SAMPLE_DRIVER_ASSERT(pWdfObject == m_FxDevice);

    hr = pWdfObject->RetrieveContext((void**)&pContext);

    if (SUCCEEDED(hr) && (pContext != NULL))
    {
        if (pContext->hostStrA != NULL)
        {
            delete pContext->hostStrA;
        }

        if (pContext->portStrA != NULL)
        {
            delete pContext->portStrA;
        }

        delete pContext;
    }
//
//CMyDevice has a reference to framework device object via m_Device. 
//Framework device object has a reference to CMyDevice object via the callbacks. 
//This leads to circular reference and both the objects can't be destroyed until this circular reference is broken. 
//To break the circular reference we release the reference to the framework device object here in OnCleanup.

    m_FxDevice = NULL;
}

HRESULT
CMyDevice::ReadAndAssignPropertyStoreValue(
    VOID
    )
/*++

  Routine Description:
    Helper function for reading property store values and storing them in the
    device level context.

  Arguments:

    pWdfFileObject - the framework file object for which close is handled.

  Return Value:

    None

--*/
{
    Trace(TRACE_LEVEL_INFORMATION, "%!FUNC!");

    CComPtr<IWDFNamedPropertyStore> pPropStore;
    WDF_PROPERTY_STORE_DISPOSITION disposition;
    PROPVARIANT val;
    HRESULT hr ;

    PropVariantInit(&val);

    DeviceContext *pContext = new DeviceContext;
    if (pContext == NULL)
    {
        hr = E_OUTOFMEMORY;
        Trace(TRACE_LEVEL_ERROR,
              L"ERROR: Could not create device context object %!hresult!",
              hr);

        goto CleanUp;
    }

    pContext->hostStrA = NULL;
    pContext->portStrA = NULL;

    //
    // Retreive property store for reading drivers custom settings as specified
    // in the INF
    //
    hr = m_FxDevice->RetrieveDevicePropertyStore(L"SocketEcho",
                                                 WdfPropertyStoreNormal,
                                                 &pPropStore,
                                                 &disposition);
    if (FAILED(hr))
    {
        Trace(TRACE_LEVEL_ERROR,
              "Failed to retrieve device property store for reading custom "
              "settings as specified in the INF %!hresult!",
              hr);

        goto CleanUp;
    }

    //
    // Get the key for this device with Named value "host"
    //
    hr = pPropStore->GetNamedValue(L"Host", &val);
    if (FAILED(hr))
    {
        Trace(TRACE_LEVEL_ERROR,
              "Failed to get \"Host\" key value %!hresult!",
              hr);

        goto CleanUp;
    }

    if (val.vt != VT_LPWSTR)
    {
        hr = HRESULT_FROM_WIN32(ERROR_BAD_CONFIGURATION);
        Trace(TRACE_LEVEL_ERROR,
              "Unexpected string format for value in \"Host\" key %!hresult!",
              hr);

        goto CleanUp;
    }

    //
    // Wide String Propvariant value need to be converted to ansi string
    // for use in socket API calls getaddrinfo for XP downlevel support.
    //
    hr = GetAnsiValFromPropVariant(val, &(pContext->hostStrA));
    if (FAILED(hr))
    {
        Trace(TRACE_LEVEL_ERROR,
              "Failed to get value from property variant for host %!hresult!",
              hr);

        goto CleanUp;
    }

    //
    // Clear property variant for reading next key
    //
    PropVariantClear(&val);

    //
    // Get the key for this device with Named value "Port"
    //
    hr = pPropStore->GetNamedValue(L"Port", &val);
    if (FAILED(hr))
    {
        Trace(TRACE_LEVEL_ERROR,
              "Failed to get \"Port\" key value %!hresult!",
              hr);

        goto CleanUp;
    }

    if (val.vt != VT_LPWSTR)
    {
        hr = HRESULT_FROM_WIN32(ERROR_BAD_CONFIGURATION);
        Trace(TRACE_LEVEL_ERROR,
              "Unexpected string format for value in \"Port\" key %!hresult!",
              hr);

        goto CleanUp;
    }

    hr = GetAnsiValFromPropVariant(val, &(pContext->portStrA));
    if (FAILED(hr))
    {
        Trace(TRACE_LEVEL_ERROR,
              "Failed to get value from property variant for port %!hresult!",
              hr);

        goto CleanUp;
    }

    hr = m_FxDevice->AssignContext(NULL, (void*)pContext);
    if (FAILED(hr))
    {
        Trace(TRACE_LEVEL_ERROR,
              "Failed to assign property store value to device %!hresult!",
              hr);

        //
        // Fall through to clean up and exit ...
        //
    }

CleanUp:

    PropVariantClear(&val);

    if (FAILED(hr))
    {
        if (pContext != NULL)
        {
            if (pContext->hostStrA != NULL)
            {
                delete [] pContext->hostStrA;
            }

            if (pContext->portStrA != NULL)
            {
                delete [] pContext->portStrA;
            }

            delete pContext;
        }
    }

    return hr;
}

HRESULT
CMyDevice::GetAnsiValFromPropVariant(
    __in PROPVARIANT val,
    __inout LPSTR *PropertyValueA
    )
/*++

  Routine Description:

    Helper function for translating  Wide String value to Ansi string

  Arguments:

    val : PROPVARIANT

  Return Value:

    HRESULT to indicate if the transaltion was successful.

--*/
{
    LPSTR ValStrA = NULL;
    HRESULT hr = S_OK;	
    int lenA = 0;

    *PropertyValueA = NULL;

    if (val.vt == VT_LPWSTR)
    {
        lenA = WideCharToMultiByte(CP_ACP,
                                   0,
                                   val.pwszVal,
                                   -1,
                                   NULL,
                                   0,
                                   NULL,
                                   FALSE);
        if (!lenA)
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
	     goto Clean0;	
        }

        ValStrA = new CHAR[lenA];		
	 ZeroMemory(ValStrA,lenA*sizeof(CHAR));
        if (ValStrA == NULL)
        {
            hr =  E_OUTOFMEMORY;
	     goto Clean0;	
        }

        if (!WideCharToMultiByte(CP_ACP,
                                 0,
                                 val.pwszVal,
                                 -1,
                                 ValStrA,
                                 lenA,
                                 NULL,
                                 FALSE))
        {
            delete [] ValStrA;
            hr = HRESULT_FROM_WIN32(GetLastError());
	     goto Clean0;	
        }
	*PropertyValueA = ValStrA;	
    }
    else 
    {
		hr = E_FAIL;
		goto Clean0;
    }
Clean0:
   
    return hr;
}
