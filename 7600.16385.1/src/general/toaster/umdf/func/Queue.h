/*++

  Copyright (c) Microsoft Corporation, All Rights Reserved

  Module Name:

    Queue.h

  Abstract:

    This file contains the class definition for the queue
    callback object.

  Environment:

    Windows User-Mode Driver Framework (WUDF)

--*/

#pragma once
#include "WUDFToaster.h"

class CQueue :  
    public IQueueCallbackDeviceIoControl,
    public IQueueCallbackRead,
    public IQueueCallbackWrite
{
    public:

        CQueue() : m_cRefs(0)
        {
        }

    public:

        //
        // Static method that creates a queue callback object.
        //        
        static HRESULT CreateInstance(__out IUnknown **ppUkwn)
        {
            *ppUkwn = NULL;
                
#pragma warning( suppress : 6014 )// PFD ISSUE: counted memory locks
            CQueue *pMyQueue = new CQueue();
            
            if (NULL == pMyQueue)
            {
                return E_OUTOFMEMORY;
            }
            return (pMyQueue->QueryInterface(__uuidof(IUnknown), (void **)ppUkwn ));
        }
        
        //
        // IUnknown
        //
        virtual HRESULT __stdcall QueryInterface(__in REFIID riid, __out LPVOID* ppvObject);
        virtual ULONG   __stdcall AddRef();
        __drv_arg(this, __drv_freesMem(object))
        virtual ULONG   __stdcall Release();

        //
        // IQueueCallbackDeviceIoControl
        //        
        virtual void __stdcall OnDeviceIoControl(
            __in IWDFIoQueue*    pQueue,
            __in IWDFIoRequest*  pRequest,
            __in ULONG           ControlCode,    
            __in SIZE_T          InputBufferSizeInBytes,
            __in SIZE_T          OutputBufferSizeInBytes
            );

        //
        // IQueueCallbackRead
        //
        virtual void __stdcall OnRead(
             __in IWDFIoQueue *pWdfQueue,
             __in IWDFIoRequest *pWdfRequest,
             __in SIZE_T NumOfBytesToRead
            );

        //
        // IQueueCallbackWrite
        //
        virtual void __stdcall OnWrite(
            __in IWDFIoQueue *pWdfQueue,
            __in IWDFIoRequest *pWdfRequest,
            __in SIZE_T NumOfBytesToWrite
            );

       //
       // TODO: Add your interfaces here
       //

    private:
        LONG    m_cRefs;
};

