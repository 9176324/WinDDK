/*++

// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.

Copyright (C) Microsoft Corporation, All Rights Reserved

Module Name:

    Queue.h

Abstract:

    This module contains the type definitions for the Sensor Skeleton Sample
    driver's queue callback class.
--*/

#pragma once

class ATL_NO_VTABLE CMyQueue :
    public CComObjectRootEx<CComMultiThreadModel>,
    public IQueueCallbackDeviceIoControl
{
public:
    virtual ~CMyQueue();

    DECLARE_NOT_AGGREGATABLE(CMyQueue)

    BEGIN_COM_MAP(CMyQueue)
        COM_INTERFACE_ENTRY(IQueueCallbackDeviceIoControl)
    END_COM_MAP()

    static HRESULT CreateInstance(__in IWDFDevice* pWdfDevice, CMyDevice* pMyDevice);

protected:
   CMyQueue();

// COM Interface methods
public:

    // IQueueCallbackDeviceIoControl
    STDMETHOD_ (void, OnDeviceIoControl)(
        __in IWDFIoQueue*    pQueue,
        __in IWDFIoRequest*  pRequest,
        __in ULONG           ControlCode,
        SIZE_T          InputBufferSizeInBytes,
        SIZE_T          OutputBufferSizeInBytes
        );

private:
    CMyDevice*          m_pParentDevice; // Parent device object
};


