// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <DriverSpecs.h>
__user_driver

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers
#endif
// Windows Header Files:
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <windowsx.h>
#include <stdio.h>
#include <tchar.h>
#include <shellapi.h>
#include <shlwapi.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
// WIA headers
#include <wia.h>
// STI headers
#include <sti.h>
#include <strsafe.h>

VOID TRC(LPCTSTR format,...);


