/*++

// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.

Copyright (C) Microsoft Corporation, All Rights Reserved

Module Name:

    Internal.h

Abstract:

    This module contains the local type definitions for the Sensor Skeleton
    driver sample.

--*/

#pragma once

#include <windows.h>
#include <atlbase.h>
#include <atlcom.h>
#include <atlstr.h>
#include "resource.h"

__user_driver;  // Macro letting the compiler know this is not a kernel driver (this will help surpress needless warnings)

// Common WPD and WUDF headers
#include <devioctl.h>
#include <initguid.h>
#include <propkeydef.h>
#include <propvarutil.h>
#include "PortableDeviceTypes.h"
#include "PortableDeviceClassExtension.h"
#include "PortableDevice.h"

// Headers for Sensor specific defines and WpdCommands
#include "Sensors.h"
#include <SensorsClassExtension.h>

// One forward-declare that pretty much everyone is going to need to know about
class CMyDevice;

///////////////////////////////////////////////////////////////////
// Common macro expansions that are used throughout the project
#define SAFE_RELEASE(p)     {if ((p)) { (p)->Release(); (p) = NULL; }}
#define ARRAY_SIZE(x)       (sizeof(x) / sizeof(x[0]))


///////////////////////////////////////////////////////////////////
// TRACING SUPPORT
//
// Define the tracing flags.
// Choose a different trace control GUID
//
// Tracing Information
#define MYDRIVER_TRACING_ID L"Microsoft\\Sensor\\TimeSensor"

//
// Define the tracing flags.
//
// Choose a different trace control GUID
//

//{398A3C0C-72F9-4320-A704-E3319BC45700}
#define WPP_CONTROL_GUIDS                                                   \
    WPP_DEFINE_CONTROL_GUID(                                                \
        TimeSensorDriverTraceControl, (398A3C0C,72F9,4320,A704,E3319BC45700),       \
                                                                            \
        WPP_DEFINE_BIT(MYDRIVER_ALL_INFO)                                   \
        )

#define WPP_FLAG_LEVEL_LOGGER(flag, level)      WPP_LEVEL_LOGGER(flag)

#define WPP_FLAG_LEVEL_ENABLED(flag, level)    ( WPP_LEVEL_ENABLED(flag) && \
                                                 WPP_CONTROL(WPP_BIT_ ## flag).Level >= level )

//
// This comment block is scanned by the trace preprocessor to define our
// Trace function.
//
// begin_wpp config
// FUNC Trace{FLAG=MYDRIVER_ALL_INFO}(LEVEL, MSG, ...);
// end_wpp
//




