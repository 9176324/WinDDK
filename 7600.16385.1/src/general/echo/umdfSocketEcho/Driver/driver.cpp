/*++
 
Copyright (C) Microsoft Corporation, All Rights Reserved.

Module Name:

    Driver.cpp

Abstract:

    This module contains the implementation of the UMDF Socketecho Sample's 
    core driver callback object.

Environment:

   Windows User-Mode Driver Framework (WUDF)

--*/

#include "internal.h"
#include "driver.tmh"

STDMETHODIMP 
CMyDriver::OnInitialize(
    __in IWDFDriver* pWdfDriver
    )

   
/*++
 
  Routine Description:

    This routine is invoked by the framework at driver load . 
    This method will invoke the Winsock Library for using 
    Winsock API in this driver.
    
  Arguments:

    pWdfDriver - Framework driver object

  Return Value:

    S_OK if successful, or error otherwise.

--*/

{
    Trace(
        TRACE_LEVEL_INFORMATION, 
        "%!FUNC!"
        );
    UNREFERENCED_PARAMETER(pWdfDriver);

    WORD sockVersion;
    WSADATA wsaData;

    sockVersion = MAKEWORD(2, 0);

    int result = WSAStartup(sockVersion, &wsaData);

    if (result != 0)
    {
        DWORD err = WSAGetLastError();
         Trace(
            TRACE_LEVEL_ERROR,
            L"ERROR: Failed to initialize Winsock 2.0 %!winerr!",
            err
            );
        return HRESULT_FROM_WIN32(err);
    }

    return S_OK;
}

STDMETHODIMP_(void) 
CMyDriver::OnDeinitialize(
  __in IWDFDriver* pWdfDriver
  )

/*++
  Routine Description:

    The FX invokes this method when it unloads the driver. 
    This routine will Cleanup Winsock library 

  Arguments:

    pWdfDriver - the Fx driver object.

  Return Value:

    None

    
    --*/

{
    Trace(
        TRACE_LEVEL_INFORMATION, 
        "%!FUNC!"
        );
   
    UNREFERENCED_PARAMETER(pWdfDriver);

    WSACleanup();
}

STDMETHODIMP
CMyDriver::OnDeviceAdd(
    __in IWDFDriver *FxWdfDriver,
    __in IWDFDeviceInitialize *FxDeviceInit
    )
/*++
 
  Routine Description:

    The FX invokes this method when it wants to install our driver on a device
    stack.  This method creates a device callback object, then calls the Fx
    to create an Fx device object and associate the new callback object with
    it.

  Arguments:

    FxWdfDriver - the Fx driver object.

    FxDeviceInit - the initialization information for the device.

  Return Value:

    status

--*/
{
    Trace(
        TRACE_LEVEL_INFORMATION, 
        "%!FUNC!"
        );
   
    HRESULT hr;

    CComObject<CMyDevice> * device = NULL;

    //
    // Create a new instance of our device callback object 
    //

    hr = CComObject<CMyDevice>::CreateInstance(&device);

    if (SUCCEEDED(hr))
    {
        hr = device->Initialize(FxWdfDriver, FxDeviceInit);
    }

    //
    // If that succeeded then call the device's configure method.  This 
    // allows the device to create any queues or other structures that it
    // needs now that the corresponding fx device object has been created.
    //

    if (SUCCEEDED(hr)) 
    {
        hr = device->Configure();
    }

    if (FAILED(hr))
    {
        delete device;
    }

    return hr;
}

