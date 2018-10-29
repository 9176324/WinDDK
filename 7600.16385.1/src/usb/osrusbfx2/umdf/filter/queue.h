/*++

Copyright (C) Microsoft Corporation, All Rights Reserved

Module Name:

    Queue.h

Abstract:

    This module contains the type definitions for the OSR USB Filter Sample
    driver's queue callback class.

Environment:

    Windows User-Mode Driver Framework (WUDF)

--*/

#pragma once

//
// Class for the queue callbacks.
// It implements
//        IQueueCallbackDeviceIoControl
//        IRequestCallbackRequestCompletion
// Queue callbacks       
//
// This class also implements IRequestCallbackRequestCompletion callback
// to get the request completion notification when request is sent down the
// stack. This callback can be implemented on a separate object as well.
// This callback is implemented here only for conenience.
//
class CMyQueue :
        public CUnknown,
        public IQueueCallbackWrite,
        public IQueueCallbackDefaultIoHandler,
        public IRequestCallbackRequestCompletion
{

//
// Private data members.
//
private:

    //
    // Weak reference to framework Queue object which this object implements callbacks for
    // This is kept as a weak reference to avoid circular reference
    // This object's lifetime is contained within framework Queue object's lifetime
    //
    
    IWDFIoQueue     *m_FxQueue;

    //
    // I/O Target to which we forward requests. Represents next device in the
    // device stack
    //
    
    IWDFIoTarget    *m_FxIoTarget;

//
// Private methods.
//

private:

    CMyQueue() : 
        m_FxQueue(NULL),
        m_FxIoTarget(NULL)
    {
    }

    virtual ~CMyQueue()
    {
        if (NULL != m_FxIoTarget)
        {
            m_FxIoTarget->Release();
        }
    }

    //
    // QueryInterface helpers
    //

    IRequestCallbackRequestCompletion *
    QueryIRequestCallbackRequestCompletion(
        VOID
        )
    {
        AddRef();
        return static_cast<IRequestCallbackRequestCompletion*>(this);
    }

    IQueueCallbackWrite *
    QueryIQueueCallbackWrite(
        VOID
        )
    {
        AddRef();
        return static_cast<IQueueCallbackWrite *>(this);
    }

    IQueueCallbackDefaultIoHandler *
    QueryIQueueCallbackDefaultIoHandler(
        VOID
        )
    {
        AddRef();
        return static_cast<IQueueCallbackDefaultIoHandler *>(this);
    }

    //
    // Initialize
    //

    HRESULT
    Initialize(
        __in IWDFDevice *FxDevice
        );

    //
    // Helper method to forward request down the stack
    //
    
    void
    ForwardRequest(
        __in IWDFIoRequest *pWdfRequest
        );

    //
    // Helper method to inverts bits in the buffer of a framework Memory object
    //

    void
    InvertBits(
        __inout IWDFMemory* FxMemory,
        __in    SIZE_T      NumBytes
        );

    //
    // Helper method to handle Read request completion
    //
    
    void
    HandleReadRequestCompletion(
        IWDFIoRequest*                 FxRequest,
        IWDFIoRequestCompletionParams* CompletionParams
        );

//
// Public methods
//
public:

    //
    // The factory method used to create an instance of this class
    //
    
    static
    HRESULT
    CreateInstance(
        __in IWDFDevice *FxDevice,
        __out CMyQueue **Queue
        );


    HRESULT
    Configure(
        VOID
        )
    {
        return S_OK;
    }

//
// COM methods
//
public:

    //
    // IUnknown methods.
    //

    virtual
    ULONG
    STDMETHODCALLTYPE
    AddRef(
        VOID
        )
    {
        return __super::AddRef();
    }

    __drv_arg(this, __drv_freesMem(object))
    virtual
    ULONG
    STDMETHODCALLTYPE
    Release(
        VOID
       )
    {
        return __super::Release();
    }

    virtual
    HRESULT
    STDMETHODCALLTYPE
    QueryInterface(
        __in REFIID InterfaceId,
        __out PVOID *Object
        );

    //
    // IQueueCallbackWrite method
    //
    
    virtual
    void
    STDMETHODCALLTYPE
    OnWrite(
        __in IWDFIoQueue*   FxQueue,
        __in IWDFIoRequest* FxRequest,        
        __in SIZE_T         NumOfBytesToWrite
        );


    //
    // IQueueCallbackDefaultIoHandler method
    //

    virtual
    void
    STDMETHODCALLTYPE
    OnDefaultIoHandler(
        __in IWDFIoQueue*    FxQueue,
        __in IWDFIoRequest*  FxRequest
        );

    //
    //IRequestCallbackRequestCompletion
    //
    
    virtual
    void
    STDMETHODCALLTYPE
    OnCompletion(
        IWDFIoRequest*                 FxRequest,
        IWDFIoTarget*                  FxIoTarget,
        IWDFRequestCompletionParams*   CompletionParams,
        PVOID                          Context
        );
};


