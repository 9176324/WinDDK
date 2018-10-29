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

#pragma once

// This file is copied from multimedia\dshow\filters\bda\PBDAFilter\FakeAESLib.

typedef struct _PBDA_AES_COUNTER
{
    __int64 qMajorCounter;
    __int64 qMinorCounter;
} PBDA_AES_COUNTER;

HRESULT __stdcall AESKeyStreamGenerator(BYTE pbKEYc[16], 
                              PBDA_AES_COUNTER *pAESCounter,
                              BYTE pbKEYStream[16]);

