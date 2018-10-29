/*++

// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.

Copyright (C) Microsoft Corporation, All Rights Reserved

Module Name:

    Defines.h

Abstract:

    This module contains the common definitions used throughout the 
    project

--*/

#pragma once

#define DEVICE_PROTOCOL_VALUE                   L"Sensor Protocol ver 1.00"
#define DEVICE_FIRMWARE_VERSION_VALUE           L"1.0.0.0"
#define DEVICE_POWER_LEVEL_VALUE                100
#define DEVICE_MODEL_VALUE                      L"Sensor Skeleton Sample"
#define DEVICE_FRIENDLY_NAME_VALUE              L"Sensor Skeleton Sample"
#define DEVICE_MANUFACTURER_VALUE               L"Microsoft Corporation"
#define DEVICE_SERIAL_NUMBER_VALUE              L"01234567890123-45676890123456"
#define DEVICE_SUPPORTS_NONCONSUMABLE_VALUE     TRUE

#define SENSOR_TEMPERATURE_ID                   L"Temperature"
#define SENSOR_TEMPERATURE_FRIENDLY_NAME        L"Temperature Sensor"
DEFINE_GUID(SENSOR_TEMPERATURE_GUID,            0XA8FFDAA7, 0X7F1F, 0X4160, 0X85, 0X8E, 0X13, 0XEF, 0X70, 0X9C, 0XE2, 0XDD);

#define SENSOR_GPS_ID                           L"GPS"
#define SENSOR_GPS_FRIENDLY_NAME                L"GPS Sensor"
DEFINE_GUID(SENSOR_GPS_GUID,                    0XB33DC926, 0XAB88, 0X492A, 0X8B, 0X97, 0X88, 0X60, 0XE0, 0X90, 0XB3, 0X80);

// Default Values 
#define DEFAULT_REPORT_INTERVAL                 0
#define MINIMUM_REPORT_INTERVAL                 0  

#define TEMPERATURE_VALUE_CELSIUS               22.5F
#define LATITUDE_VALUE_DEGREES                  47.449L
#define LONGITUDE_VALUE_DEGREES                -122.309L
#define ALTITUDE_VALUE_METERS                   132.0F
#define ERROR_RADIUS_VALUE_METERS               10.0L

