/*++

// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

Module Name:

   SampleEvents.h

Abstract:

    This module contains the declarations for the 
    SampleEvents class.
--*/

#pragma once

class CSensorDdi; // forward decl

class CSampleEvents 
{
public:
    CSampleEvents();
    virtual ~CSampleEvents();

    HRESULT Initialize(ISensorClassExtension *pSensorCXT, CSensorDdi* pDdi, ULONG ulCurrentInterval);
    HRESULT Uninitialize();
    HRESULT SetInterval(DWORD dwInterval);
    HRESULT PostStateEvent();

private:

    // Smart pointer to sensor class extension object.
    CComPtr<ISensorClassExtension> m_spSensorCXT;
    CSensorDdi* m_pDdi;
    HANDLE m_hEventThread;
    HANDLE m_hCloseThread;
    DWORD m_dwInterval;

    // Data Event thread procedure.
    static DWORD WINAPI _EventThreadProc(__in LPVOID pvData);
    HRESULT PostDataEvent(IPortableDeviceValues* pValues);
};



