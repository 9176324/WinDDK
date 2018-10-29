//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.
//
//==========================================================================;

#pragma warning (disable : 6001 20051 28101 28103 28107 28110 28170 28172 28718 6297)

#ifndef _COMMON_H
#define _COMMON_H_

extern "C" {
#include <wdm.h>
}

#include <windef.h>
#define NOBITMAP
#include <mmreg.h>
#undef NOBITMAP
#include <ntstrsafe.h>

extern "C" {
//#include <ntddk.h>
#include "wdmdebug.h"
}

// #include <limits.h>
#include <winerror.h>
#include <unknown.h>
#include <ks.h>
#include <ksmedia.h>
#include <bdatypes.h>
#include <bdamedia.h>
#include <atsmedia.h>
#include <bdasup.h>
#include <pbdaerrors.h>

#include <kcom.h>
#include <ksdebug.h>
#include "splmedia.h"

#undef INTERFACE

#ifndef __nullterminated
#define __nullterminated 
#endif


#include "NewGUIDS.h"


#endif //_COMMON_H_

