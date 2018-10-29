//
//  Windows Server (Printing) Driver Development Kit Samples.
//
//
//  Copyright (c) 1990 - 2005 Microsoft Corporation.
//  All Rights Reserved.
//
//
//  This source code is intended only as a supplement to Microsoft
//  Development Tools and/or on-line documentation.  See these other
//  materials for detailed information regarding Microsoft code samples.
// 
//  THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//  PARTICULAR PURPOSE.
//

#ifndef __ACALLBACK_HPP__
#define __ACALLBACK_HPP__

//
// CPrintOEMAsyncNotifyCallback implements callback interface required
// to receive notifications.
// 
class CPrintOEMAsyncNotifyCallback : public IPrintAsyncNotifyCallback
{
    
public:

    //
    // Default Constructor
    //
    CPrintOEMAsyncNotifyCallback(
    );
    
    //
    // IUnknown methods
    //
    STDMETHODIMP
    QueryInterface(
        IN  REFIID  riid,
        OUT VOID    **ppv
        );

    STDMETHODIMP_(ULONG)
    AddRef(
        VOID
        );

    __drv_at(this, __drv_freesMem(object))
    STDMETHODIMP_(ULONG)
    Release(
        VOID
        );

    //
    // IAsyncNotifications methods
    //
    STDMETHODIMP
    OnEventNotify(
        __in IPrintAsyncNotifyChannel        *pIAsyncNotification,
        __in IPrintAsyncNotifyDataObject     *pNotification
        );

    STDMETHODIMP
    ChannelClosed(
        __in IPrintAsyncNotifyChannel        *pIAsyncNotification,
        __in IPrintAsyncNotifyDataObject     *pNotification
        ); 

private:

    HRESULT
    ReplyOnClientEventNotify(
        __in IPrintAsyncNotifyChannel*
        );

    ~CPrintOEMAsyncNotifyCallback(
    );  
    
    LONG                        m_cRef;
};



#endif //__ACALLBACK_HPP__


