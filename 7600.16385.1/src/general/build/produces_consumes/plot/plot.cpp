/*++

Copyright (c) 2008  Microsoft Corporation

Module Name:

...plot.cpp

Abstract:

This is a dummy plot source file

Environment:

    User mode

--*/

#include <windows.h>
#include <drawlib.h>


BOOL WINAPI DllMain(
    HINSTANCE hInst,
    DWORD reason,
    LPVOID
    )
{
    UNREFERENCED_PARAMETER( hInst );
    UNREFERENCED_PARAMETER( reason );
    
    //
    // Call drawlib functions ...
    //
    SetMetrix(1);
    
    return TRUE;
}

