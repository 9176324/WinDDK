/*++

// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.

Copyright (C) Microsoft Corporation, All Rights Reserved

Module Name:

    Device.h

Abstract:

    This module contains the type definitions for the Sensor Skeleton sample
    driver's device callback class.

--*/

#pragma once

class CSampleEvents;  // forward decl
class CSensorDdi;

class ATL_NO_VTABLE CMyDevice :
    public CComObjectRootEx<CComMultiThreadModel>,
    public IPnpCallbackHardware,
    public IFileCallbackCleanup
{
public:
    ~CMyDevice();

    DECLARE_NOT_AGGREGATABLE(CMyDevice)

    BEGIN_COM_MAP(CMyDevice)
        COM_INTERFACE_ENTRY(IPnpCallbackHardware)
        COM_INTERFACE_ENTRY(IFileCallbackCleanup)
    END_COM_MAP()

protected:
    CMyDevice();

    HRESULT ConfigureQueue();

// COM Interface methods
public:

    //
    // IPnpCallbackHardware
    //
    STDMETHOD_ (HRESULT, OnPrepareHardware)(__in IWDFDevice* pWdfDevice);
    STDMETHOD_ (HRESULT, OnReleaseHardware)(__in IWDFDevice* pWdfDevice);
    //
    // IFileCallbackCleanup
    //
    STDMETHOD_ (VOID, OnCleanupFile)(__in IWDFFile *pWdfFile);

public:

    // The factory method used to create an instance of this device
    static HRESULT CreateInstance(__in IWDFDriver* pDriver, __in IWDFDeviceInitialize*  pDeviceInit);

    HRESULT ProcessIoControl(__in IWDFIoQueue*     pQueue,
                             __in IWDFIoRequest*   pRequest,
                             __in ULONG            ControlCode,
                                  SIZE_T           InputBufferSizeInBytes,
                                  SIZE_T           OutputBufferSizeInBytes,
                                  DWORD*           pcbWritten);
private:

    HRESULT     SetUniqueID(__in IWDFDevice* pWdfDevice, __out GUID* puid);

    CComPtr<IWDFDevice>     m_pWdfDevice;
    ISensorClassExtension*  m_pClassExtension;
    CSampleEvents*       m_pSampleEvents;
    CComObject<CSensorDdi>* m_pSensorDdi;
};


