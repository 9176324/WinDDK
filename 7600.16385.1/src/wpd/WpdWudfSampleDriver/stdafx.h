// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#include "resource.h"
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define STRSAFE_NO_DEPRECATE

#include <stdio.h>
#include <tchar.h>

#include <atlbase.h>
#include <atlcom.h>
#include <atlcoll.h>
#include <atlstr.h>

#include "WpdWudfSampleDriver.h"
#include "PortableDeviceTypes.h"
#include "PortableDeviceClassExtension.h"
#include "PortableDevice.h"
#include "ContextMap.h"
#include "helpers.h"
#include "FakeContent.h"
#include "FakeImageContent.h"
#include "FakeMusicContent.h"
#include "FakeVideoContent.h"
#include "FakeContactContent.h"
#include "FakeMemoContent.h"
#include "FakeFolderContent.h"
#include "RenderingInformationFakeContent.h"
#include "NetworkConfigFakeContent.h"
#include "DeviceObjectFakeContent.h"
#include "StorageObjectFakeContent.h"
#include "FakeDevice.h"
#include "WpdObjectEnum.h"
#include "WpdObjectManagement.h"
#include "WpdObjectProperties.h"
#include "WpdObjectPropertiesBulk.h"
#include "WpdObjectResources.h"
#include "WpdCapabilities.h"
#include "WpdStorage.h"
#include "WpdNetworkConfig.h"
#include "WpdBaseDriver.h"

extern HINSTANCE g_hInstance;


