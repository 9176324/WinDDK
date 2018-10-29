//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//  ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//  PARTICULAR PURPOSE.
//
//  Copyright  1998 - 2003  Microsoft Corporation.  All Rights Reserved.
//
//  FILE:   dllentry.cpp
//
//  PURPOSE:  Source module for DLL entry function(s).
//

#include "precomp.h"
#include "debug.h"
#include "globals.h"

// This indicates to Prefast that this is a usermode driver file.
__user_driver;

///////////////////////////////////////////////////////////
//
// DLL entry point
//
BOOL WINAPI DllMain(HINSTANCE hInst, WORD wReason, LPVOID lpReserved)
{
    UNREFERENCED_PARAMETER(lpReserved);

    switch(wReason)
    {
        case DLL_PROCESS_ATTACH:
            VERBOSE("Process attach.\r\n");
            ghInstance = hInst;
            break;

        case DLL_THREAD_ATTACH:
            VERBOSE("Thread attach.\r\n");
            break;

        case DLL_PROCESS_DETACH:
            VERBOSE("Process detach.\r\n");
            break;

        case DLL_THREAD_DETACH:
            VERBOSE("Thread detach.\r\n");
            break;
    }

    return TRUE;
}

