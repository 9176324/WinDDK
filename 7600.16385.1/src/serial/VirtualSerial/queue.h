/*++

Copyright (c) Microsoft Corporation, All Rights Reserved

Module Name:

    queue.h

Abstract:

    This file defines the queue callback interface.

Environment:

    Windows User-Mode Driver Framework (WUDF)

--*/

#pragma once

#include "internal.h"

// Set ring buffer size
#define DATA_BUFFER_SIZE 1024

//
// Macro definition for defining IOCTL and FSCTL function control codes.  Note
// that function codes 0-2047 are reserved for Microsoft Corporation, and
// 2048-4095 are reserved for customers.
//

#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)


//
// Queue Callback Object.
//

class CMyQueue : 
    public IQueueCallbackDeviceIoControl,
    public IQueueCallbackRead,
    public IQueueCallbackWrite,
    public CUnknown
{
    UCHAR           m_CommandMatchState; // Device state
    BOOL            m_ConnectCommand;
    BOOL            m_IgnoreNextChar;
    BOOL            m_ConnectionStateChanged;
    BOOL            m_CurrentlyConnected;
    
    IWDFIoQueue    *m_FxQueue;           // Default parallel queue
    IWDFIoQueue    *m_FxReadQueue;       // Manual queue for pending reads
    
    CRingBuffer    m_RingBuffer;        // Ring buffer for pending data
    
    PCMyDevice     m_Device;

    CMyQueue(
        CMyDevice *pDevice
        ) : 
        m_CommandMatchState(COMMAND_MATCH_STATE_IDLE),
        m_ConnectCommand(FALSE),
        m_IgnoreNextChar(FALSE),
        m_ConnectionStateChanged(FALSE),
        m_CurrentlyConnected(FALSE),
        m_FxQueue(NULL),
        m_FxReadQueue(NULL)
    {
        WUDF_TEST_DRIVER_ASSERT(pDevice);

        pDevice->AddRef();
        m_Device = pDevice;        
    }

    virtual ~CMyQueue();

    HRESULT
    Initialize(
        __in IWDFDevice *FxDevice
        );
    
    VOID
    CMyQueue::ProcessWriteBytes(
        __in_bcount(Length) PUCHAR Characters,
        __in SIZE_T Length
        );
        
public:

    static 
    HRESULT 
    CreateInstance( 
        __in  CMyDevice *pDevice,
        __in IWDFDevice *FxDevice,
        __out PCMyQueue *Queue
        );

    HRESULT
    Configure(
        VOID
        )
    {
        return S_OK;
    }

   
    IQueueCallbackDeviceIoControl *
    QueryIQueueCallbackDeviceIoControl(
        VOID
        )
    {
        AddRef();
        return static_cast<IQueueCallbackDeviceIoControl *>(this);
    }
    
    IQueueCallbackRead *
    QueryIQueueCallbackRead(
        VOID
        )
    {
        AddRef();
        return static_cast<IQueueCallbackRead *>(this);
    }

    IQueueCallbackWrite *
    QueryIQueueCallbackWrite(
        VOID
        )
    {
        AddRef();
        return static_cast<IQueueCallbackWrite *>(this);
    }

    //
    // IUnknown
    //

    virtual
    ULONG 
    STDMETHODCALLTYPE
    AddRef(
        VOID
        ) {
        return CUnknown::AddRef();
    }

    __drv_arg(this, __drv_freesMem(object))
    virtual
    ULONG
    STDMETHODCALLTYPE
    Release(
        VOID
        ) {
        return CUnknown::Release();
    }

    virtual
    HRESULT
    STDMETHODCALLTYPE
    QueryInterface(
        __in REFIID InterfaceId, 
        __out PVOID *Object
        );

    //
    // Wdf Callbacks
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

    // IQueueCallbackWrite
    //
    virtual
    VOID
    STDMETHODCALLTYPE
    OnWrite( 
        __in IWDFIoQueue *pWdfQueue,
        __in IWDFIoRequest *pWdfRequest,
        __in SIZE_T NumOfBytesToWrite
        );

    // IQueueCallbackRead
    //
    virtual
    VOID
    STDMETHODCALLTYPE
    OnRead( 
        __in IWDFIoQueue *pWdfQueue,
        __in IWDFIoRequest *pWdfRequest,
        __in SIZE_T NumOfBytesToRead
        );
};

