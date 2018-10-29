/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   transform.h

Abstract:

   CTransform class definition. This class creates and manages color transforms providing
   limited caching functionality based off the source profile keys.


--*/

#pragma once

#include "profile.h"

typedef vector<CProfile*> ProfileList;

class CTransform
{
public:
    CTransform();

    ~CTransform();

    HRESULT
    CreateTransform(
        __in ProfileList* pProfiles,
        __in CONST DWORD  intent,
        __in CONST DWORD  renderFlags
        );

    HRESULT
    GetTransformHandle(
        __out HTRANSFORM* phTrans
        );


private:
    VOID
    FreeTransform(
        VOID
        );

    HRESULT
    CreateProfileKeysBuffer(
        __in CONST UINT& cProfiles
        );

    VOID
    FreeProfileKeysBuffer(
        VOID
        );

    HRESULT
    UpdateProfiles(
        __in  ProfileList* pProfiles,
        __out BOOL*        pbUpdate
        );

private:
    HTRANSFORM  m_hColorTrans;

    DWORD       m_intents;

    DWORD       m_renderFlags;

    CStringXDW* m_pcstrProfileKeys;

    DWORD       m_cProfiles;
};


