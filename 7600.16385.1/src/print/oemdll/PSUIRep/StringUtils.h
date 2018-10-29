//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//  ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//  PARTICULAR PURPOSE.
//
//  Copyright  2001 - 2003  Microsoft Corporation.  All Rights Reserved.
//
//  FILE:   StringUtils.h
//
//  PURPOSE:  Header file for string utility routines.
//
#pragma once

////////////////////////////////////////////////////////
//      Prototypes
////////////////////////////////////////////////////////

HRESULT MakeStrPtrList(HANDLE hHeap, PCSTR pmszMultiSz, PCSTR **pppszList, PWORD pwCount);
WORD mstrcount(PCSTR pmszMultiSz);
PWSTR MakeUnicodeString(HANDLE hHeap, PCSTR pszAnsi);
PWSTR MakeStringCopy(HANDLE hHeap, PCWSTR pszSource);
PSTR MakeStringCopy(HANDLE hHeap, PCSTR pszSource);
void FreeStringList(HANDLE hHeap, __in_ecount(wCount) PWSTR *ppszList, WORD wCount);
HRESULT GetStringResource(HANDLE hHeap, HMODULE hModule, UINT uResource, __deref_out_opt PWSTR *ppszString);
HRESULT GetStringResource(HANDLE hHeap, HMODULE hModule, UINT uResource, __deref_out_opt PSTR *ppszString);

