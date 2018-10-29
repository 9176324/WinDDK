/*++
 
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.

Copyright (C) Microsoft Corporation, All Rights Reserved

Module Name:

    Driver.cpp

Abstract:

    This module contains the implementation of the Sensor Skeleton Sample's 
    core driver callback object.
--*/

#include "internal.h"
#include "SensorSkeleton.h" // IDL Generated File
#include "Driver.h"
#include "Device.h"

#include "driver.tmh"

/////////////////////////////////////////////////////////////////////////
//
// CMyDriver::CMyDriver
//
// Object constructor function
//
/////////////////////////////////////////////////////////////////////////
CMyDriver::CMyDriver()
{
}

/////////////////////////////////////////////////////////////////////////
//
// CMyDriver::OnDeviceAdd
//
// The framework call this function when device is detected. This driver
// creates a device callback object
//
// Parameters:
//      pDriver     - pointer to an IWDFDriver object
//      pDeviceInit - pointer to a device initialization object
//
// Return Values:
//      S_OK: device initialized successfully
//
/////////////////////////////////////////////////////////////////////////
HRESULT CMyDriver::OnDeviceAdd(
    __in IWDFDriver* pDriver,
    __in IWDFDeviceInitialize* pDeviceInit
    )
{
    HRESULT hr = CMyDevice::CreateInstance(pDriver, pDeviceInit);

    return hr;
}
/////////////////////////////////////////////////////////////////////////
//
// CMyDriver::OnInitialize
//
//  The framework calls this function just after loading the driver. The driver
//  can perform any global, device independent intialization in this routine.
//
/////////////////////////////////////////////////////////////////////////
HRESULT CMyDriver::OnInitialize(
    __in IWDFDriver* pDriver
    )
{
    UNREFERENCED_PARAMETER(pDriver);
    return S_OK;
}

/////////////////////////////////////////////////////////////////////////
//
// CMyDriver::OnDeinitialize
//
//  The framework calls this function just before de-initializing itself. All
//  WDF framework resources should be released by driver before returning
//  from this call.
//
/////////////////////////////////////////////////////////////////////////
void CMyDriver::OnDeinitialize(
    __in IWDFDriver* pDriver
    )
{
    UNREFERENCED_PARAMETER(pDriver);
    return;
}

