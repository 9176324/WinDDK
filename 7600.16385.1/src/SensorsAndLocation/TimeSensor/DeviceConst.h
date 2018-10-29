/*++

// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.

Copyright (C) Microsoft Corporation, All Rights Reserved

Module Name:

    DeviceConst.h

Abstract:

    This module contains global constant definitions used by the Time Sensor
    sample driver.
--*/

#pragma once

// Sensor properties
static const LPWSTR g_wszSensorID = L"Time Sensor";
const LPWSTR g_wszSensorFriendlyName = L"Time Sensor Sample";
const LPWSTR g_wszDeviceProtocol = L"Sensor Protocol ver 1.00";
const LPWSTR g_wszDeviceFirmareVersion = L"1.0.0.0";
const LPWSTR g_wszDeviceModel = L"Time Sensor Sample";
const LPWSTR g_wszDeviceFriendlyName = L"Time Sensor Sample Device";
const LPWSTR g_wszSensorDescription = L"Provides the current system time.";
const LPWSTR g_wszDeviceManufacturer = L"Microsoft Corporation";

// A static serial number should not be used as a
// persistent unique ID. This sample generates the PUID per instance (in Device.cpp)
// because the driver does not connect to hardware. If hardware devices
// are used, the serial number value should be unique to each device and
// the driver should retrieve the PUID value from the device.
const LPWSTR g_wszDeviceSerialNumber = L"01234567890123-45676890123457";

const BOOL g_bDeviceSupportsNonconsumable = TRUE;
const int g_iDevicePowerLevel = 100;

// Event intervals
static const DWORD g_dwDefaultInterval = 1000; // one second
static const DWORD g_dwMinInterval = 500; // one-half second

// Maximum length for property strings.
const int g_iMaxStrLen = 100;



