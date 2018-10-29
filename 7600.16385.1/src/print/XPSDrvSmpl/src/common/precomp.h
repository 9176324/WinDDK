/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   precomp.h

Abstract:

   Precompiled header for all filters

--*/

#pragma once

//
// Annotate this as a usermode driver for static analysis
//
#include <DriverSpecs.h>
__user_driver

//
// Standard Annotation Language include
//
#include <sal.h>

//
// Windows includes
//
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <limits.h>

//
// COM includes
//
#include <objbase.h>
#include <oleauto.h>

//
// ATL Includes
//
#include <atlbase.h>


//
// GDIPlus includes
//
#include <GDIPlus.h>

//
// MSXML includes
//
#include <msxml6.h>

//
// WCS Includes
//
#include <icm.h>

//
// Filter pipeline includes
//
#include <winspool.h>
#include <filterpipeline.h>
#include <filterpipelineutil.h>
#include <prntvpt.h>

//
// Standard library includes
//
#include <math.h>
#include <vector>
#include <deque>
#include <map>

//
// Commonly used namespaces
//
using namespace std;
using namespace Gdiplus;

//
// String safe includes - included last to prevent build warnings
//
#include <strsafe.h>

#include "common.ver"

