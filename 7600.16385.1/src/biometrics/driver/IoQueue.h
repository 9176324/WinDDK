/*++

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
    ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.

    Copyright (c) Microsoft Corporation. All rights reserved

Module Name:

    IoQueue.h

Abstract:

    This file defines the queue callback interface.

Environment:

    Windows User-Mode Driver Framework (WUDF)

--*/

#pragma once

//
// Queue Callback Object.
//

class CBiometricIoQueue :
    public CComObjectRootEx<CComMultiThreadModel>,
    public IQueueCallbackDeviceIoControl
{

public:

    DECLARE_NOT_AGGREGATABLE(CBiometricIoQueue)

    BEGIN_COM_MAP(CBiometricIoQueue)
        COM_INTERFACE_ENTRY(IQueueCallbackDeviceIoControl)
    END_COM_MAP()

    CBiometricIoQueue() : 
        m_FxQueue(NULL),
        m_BiometricDevice(NULL)
    {
    }

    ~CBiometricIoQueue()
    {
        // empty
    }

    HRESULT
    Initialize(
        __in IWDFDevice *FxDevice,
        __in CBiometricDevice *BiometricDevice
        );

    static 
    HRESULT 
    CreateInstanceAndInitialize( 
        __in IWDFDevice *FxDevice,
        __in CBiometricDevice *BiometricDevice,
        __out CBiometricIoQueue**    Queue
        );

    HRESULT
    Configure(
        VOID
        )
    {
        return S_OK;
    }

    VOID
    Start(
        )
    {
        m_FxQueue->Start();
    }

    VOID
    StopSynchronously(
        )
    {
        m_FxQueue->StopSynchronously();
    }

    //
    // Wdf Callbacks
    //

    //    
    // IQueueCallbackDeviceIoControl
    //
    virtual
    VOID
    STDMETHODCALLTYPE
    OnDeviceIoControl( 
        __in IWDFIoQueue *pWdfQueue,
        __in IWDFIoRequest *pWdfRequest,
        __in ULONG ControlCode,
        __in SIZE_T InputBufferSizeInBytes,
        __in SIZE_T OutputBufferSizeInBytes
        );

//
// Private member variables.
//
private:

    //
    // Weak reference to framework queue object.
    //
    IWDFIoQueue *                       m_FxQueue;

    //
    // Pointer to device class.
    //
    CBiometricDevice *                  m_BiometricDevice;

};

