/*++
    Copyright (c) Microsoft Corporation

    Module Name:
        pch.h

    Abstract:
        Pre-compile C header file.

    Environment:
        Kernel mode

--*/

#pragma once

#define COMPONENT_NAME          "HTOUCH"
#define INTERNAL
#define EXTERNAL
//#define PRIORITY_UPDATE
#if DBG
#ifndef DEBUG
  #define DEBUG
#endif
#endif


#if _MSC_VER >= 1200

#pragma warning(push)

#endif

#pragma warning(disable:4201) // nameless struct/union
#include <wdm.h>
#include <wdf.h>
#include <ntddser.h>
#include <hidport.h>
#define NTSTRSAFE_LIB
#include <ntstrsafe.h>
#include <naturalInput.h>
#include "eloTouch.h"
#include <serial.h>
#include <errlog.h>
  #include <wtrace.h>

#if _MSC_VER >= 1200

#pragma warning(pop)

#else
#pragma warning(default:4201)
#endif
