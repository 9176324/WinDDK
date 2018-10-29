

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* Compiler settings for objidl.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

/* verify that the <rpcsal.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCSAL_H_VERSION__
#define __REQUIRED_RPCSAL_H_VERSION__ 100
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __objidl_h__
#define __objidl_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IMarshal_FWD_DEFINED__
#define __IMarshal_FWD_DEFINED__
typedef interface IMarshal IMarshal;
#endif 	/* __IMarshal_FWD_DEFINED__ */


#ifndef __IMarshal2_FWD_DEFINED__
#define __IMarshal2_FWD_DEFINED__
typedef interface IMarshal2 IMarshal2;
#endif 	/* __IMarshal2_FWD_DEFINED__ */


#ifndef __IMalloc_FWD_DEFINED__
#define __IMalloc_FWD_DEFINED__
typedef interface IMalloc IMalloc;
#endif 	/* __IMalloc_FWD_DEFINED__ */


#ifndef __IMallocSpy_FWD_DEFINED__
#define __IMallocSpy_FWD_DEFINED__
typedef interface IMallocSpy IMallocSpy;
#endif 	/* __IMallocSpy_FWD_DEFINED__ */


#ifndef __IStdMarshalInfo_FWD_DEFINED__
#define __IStdMarshalInfo_FWD_DEFINED__
typedef interface IStdMarshalInfo IStdMarshalInfo;
#endif 	/* __IStdMarshalInfo_FWD_DEFINED__ */


#ifndef __IExternalConnection_FWD_DEFINED__
#define __IExternalConnection_FWD_DEFINED__
typedef interface IExternalConnection IExternalConnection;
#endif 	/* __IExternalConnection_FWD_DEFINED__ */


#ifndef __IMultiQI_FWD_DEFINED__
#define __IMultiQI_FWD_DEFINED__
typedef interface IMultiQI IMultiQI;
#endif 	/* __IMultiQI_FWD_DEFINED__ */


#ifndef __AsyncIMultiQI_FWD_DEFINED__
#define __AsyncIMultiQI_FWD_DEFINED__
typedef interface AsyncIMultiQI AsyncIMultiQI;
#endif 	/* __AsyncIMultiQI_FWD_DEFINED__ */


#ifndef __IInternalUnknown_FWD_DEFINED__
#define __IInternalUnknown_FWD_DEFINED__
typedef interface IInternalUnknown IInternalUnknown;
#endif 	/* __IInternalUnknown_FWD_DEFINED__ */


#ifndef __IEnumUnknown_FWD_DEFINED__
#define __IEnumUnknown_FWD_DEFINED__
typedef interface IEnumUnknown IEnumUnknown;
#endif 	/* __IEnumUnknown_FWD_DEFINED__ */


#ifndef __IBindCtx_FWD_DEFINED__
#define __IBindCtx_FWD_DEFINED__
typedef interface IBindCtx IBindCtx;
#endif 	/* __IBindCtx_FWD_DEFINED__ */


#ifndef __IEnumMoniker_FWD_DEFINED__
#define __IEnumMoniker_FWD_DEFINED__
typedef interface IEnumMoniker IEnumMoniker;
#endif 	/* __IEnumMoniker_FWD_DEFINED__ */


#ifndef __IRunnableObject_FWD_DEFINED__
#define __IRunnableObject_FWD_DEFINED__
typedef interface IRunnableObject IRunnableObject;
#endif 	/* __IRunnableObject_FWD_DEFINED__ */


#ifndef __IRunningObjectTable_FWD_DEFINED__
#define __IRunningObjectTable_FWD_DEFINED__
typedef interface IRunningObjectTable IRunningObjectTable;
#endif 	/* __IRunningObjectTable_FWD_DEFINED__ */


#ifndef __IPersist_FWD_DEFINED__
#define __IPersist_FWD_DEFINED__
typedef interface IPersist IPersist;
#endif 	/* __IPersist_FWD_DEFINED__ */


#ifndef __IPersistStream_FWD_DEFINED__
#define __IPersistStream_FWD_DEFINED__
typedef interface IPersistStream IPersistStream;
#endif 	/* __IPersistStream_FWD_DEFINED__ */


#ifndef __IMoniker_FWD_DEFINED__
#define __IMoniker_FWD_DEFINED__
typedef interface IMoniker IMoniker;
#endif 	/* __IMoniker_FWD_DEFINED__ */


#ifndef __IROTData_FWD_DEFINED__
#define __IROTData_FWD_DEFINED__
typedef interface IROTData IROTData;
#endif 	/* __IROTData_FWD_DEFINED__ */


#ifndef __IEnumString_FWD_DEFINED__
#define __IEnumString_FWD_DEFINED__
typedef interface IEnumString IEnumString;
#endif 	/* __IEnumString_FWD_DEFINED__ */


#ifndef __ISequentialStream_FWD_DEFINED__
#define __ISequentialStream_FWD_DEFINED__
typedef interface ISequentialStream ISequentialStream;
#endif 	/* __ISequentialStream_FWD_DEFINED__ */


#ifndef __IStream_FWD_DEFINED__
#define __IStream_FWD_DEFINED__
typedef interface IStream IStream;
#endif 	/* __IStream_FWD_DEFINED__ */


#ifndef __IEnumSTATSTG_FWD_DEFINED__
#define __IEnumSTATSTG_FWD_DEFINED__
typedef interface IEnumSTATSTG IEnumSTATSTG;
#endif 	/* __IEnumSTATSTG_FWD_DEFINED__ */


#ifndef __IStorage_FWD_DEFINED__
#define __IStorage_FWD_DEFINED__
typedef interface IStorage IStorage;
#endif 	/* __IStorage_FWD_DEFINED__ */


#ifndef __IPersistFile_FWD_DEFINED__
#define __IPersistFile_FWD_DEFINED__
typedef interface IPersistFile IPersistFile;
#endif 	/* __IPersistFile_FWD_DEFINED__ */


#ifndef __IPersistStorage_FWD_DEFINED__
#define __IPersistStorage_FWD_DEFINED__
typedef interface IPersistStorage IPersistStorage;
#endif 	/* __IPersistStorage_FWD_DEFINED__ */


#ifndef __ILockBytes_FWD_DEFINED__
#define __ILockBytes_FWD_DEFINED__
typedef interface ILockBytes ILockBytes;
#endif 	/* __ILockBytes_FWD_DEFINED__ */


#ifndef __IEnumFORMATETC_FWD_DEFINED__
#define __IEnumFORMATETC_FWD_DEFINED__
typedef interface IEnumFORMATETC IEnumFORMATETC;
#endif 	/* __IEnumFORMATETC_FWD_DEFINED__ */


#ifndef __IEnumSTATDATA_FWD_DEFINED__
#define __IEnumSTATDATA_FWD_DEFINED__
typedef interface IEnumSTATDATA IEnumSTATDATA;
#endif 	/* __IEnumSTATDATA_FWD_DEFINED__ */


#ifndef __IRootStorage_FWD_DEFINED__
#define __IRootStorage_FWD_DEFINED__
typedef interface IRootStorage IRootStorage;
#endif 	/* __IRootStorage_FWD_DEFINED__ */


#ifndef __IAdviseSink_FWD_DEFINED__
#define __IAdviseSink_FWD_DEFINED__
typedef interface IAdviseSink IAdviseSink;
#endif 	/* __IAdviseSink_FWD_DEFINED__ */


#ifndef __AsyncIAdviseSink_FWD_DEFINED__
#define __AsyncIAdviseSink_FWD_DEFINED__
typedef interface AsyncIAdviseSink AsyncIAdviseSink;
#endif 	/* __AsyncIAdviseSink_FWD_DEFINED__ */


#ifndef __IAdviseSink2_FWD_DEFINED__
#define __IAdviseSink2_FWD_DEFINED__
typedef interface IAdviseSink2 IAdviseSink2;
#endif 	/* __IAdviseSink2_FWD_DEFINED__ */


#ifndef __AsyncIAdviseSink2_FWD_DEFINED__
#define __AsyncIAdviseSink2_FWD_DEFINED__
typedef interface AsyncIAdviseSink2 AsyncIAdviseSink2;
#endif 	/* __AsyncIAdviseSink2_FWD_DEFINED__ */


#ifndef __IDataObject_FWD_DEFINED__
#define __IDataObject_FWD_DEFINED__
typedef interface IDataObject IDataObject;
#endif 	/* __IDataObject_FWD_DEFINED__ */


#ifndef __IDataAdviseHolder_FWD_DEFINED__
#define __IDataAdviseHolder_FWD_DEFINED__
typedef interface IDataAdviseHolder IDataAdviseHolder;
#endif 	/* __IDataAdviseHolder_FWD_DEFINED__ */


#ifndef __IMessageFilter_FWD_DEFINED__
#define __IMessageFilter_FWD_DEFINED__
typedef interface IMessageFilter IMessageFilter;
#endif 	/* __IMessageFilter_FWD_DEFINED__ */


#ifndef __IRpcChannelBuffer_FWD_DEFINED__
#define __IRpcChannelBuffer_FWD_DEFINED__
typedef interface IRpcChannelBuffer IRpcChannelBuffer;
#endif 	/* __IRpcChannelBuffer_FWD_DEFINED__ */


#ifndef __IRpcChannelBuffer2_FWD_DEFINED__
#define __IRpcChannelBuffer2_FWD_DEFINED__
typedef interface IRpcChannelBuffer2 IRpcChannelBuffer2;
#endif 	/* __IRpcChannelBuffer2_FWD_DEFINED__ */


#ifndef __IAsyncRpcChannelBuffer_FWD_DEFINED__
#define __IAsyncRpcChannelBuffer_FWD_DEFINED__
typedef interface IAsyncRpcChannelBuffer IAsyncRpcChannelBuffer;
#endif 	/* __IAsyncRpcChannelBuffer_FWD_DEFINED__ */


#ifndef __IRpcChannelBuffer3_FWD_DEFINED__
#define __IRpcChannelBuffer3_FWD_DEFINED__
typedef interface IRpcChannelBuffer3 IRpcChannelBuffer3;
#endif 	/* __IRpcChannelBuffer3_FWD_DEFINED__ */


#ifndef __IRpcSyntaxNegotiate_FWD_DEFINED__
#define __IRpcSyntaxNegotiate_FWD_DEFINED__
typedef interface IRpcSyntaxNegotiate IRpcSyntaxNegotiate;
#endif 	/* __IRpcSyntaxNegotiate_FWD_DEFINED__ */


#ifndef __IRpcProxyBuffer_FWD_DEFINED__
#define __IRpcProxyBuffer_FWD_DEFINED__
typedef interface IRpcProxyBuffer IRpcProxyBuffer;
#endif 	/* __IRpcProxyBuffer_FWD_DEFINED__ */


#ifndef __IRpcStubBuffer_FWD_DEFINED__
#define __IRpcStubBuffer_FWD_DEFINED__
typedef interface IRpcStubBuffer IRpcStubBuffer;
#endif 	/* __IRpcStubBuffer_FWD_DEFINED__ */


#ifndef __IPSFactoryBuffer_FWD_DEFINED__
#define __IPSFactoryBuffer_FWD_DEFINED__
typedef interface IPSFactoryBuffer IPSFactoryBuffer;
#endif 	/* __IPSFactoryBuffer_FWD_DEFINED__ */


#ifndef __IChannelHook_FWD_DEFINED__
#define __IChannelHook_FWD_DEFINED__
typedef interface IChannelHook IChannelHook;
#endif 	/* __IChannelHook_FWD_DEFINED__ */


#ifndef __IClientSecurity_FWD_DEFINED__
#define __IClientSecurity_FWD_DEFINED__
typedef interface IClientSecurity IClientSecurity;
#endif 	/* __IClientSecurity_FWD_DEFINED__ */


#ifndef __IServerSecurity_FWD_DEFINED__
#define __IServerSecurity_FWD_DEFINED__
typedef interface IServerSecurity IServerSecurity;
#endif 	/* __IServerSecurity_FWD_DEFINED__ */


#ifndef __IClassActivator_FWD_DEFINED__
#define __IClassActivator_FWD_DEFINED__
typedef interface IClassActivator IClassActivator;
#endif 	/* __IClassActivator_FWD_DEFINED__ */


#ifndef __IRpcOptions_FWD_DEFINED__
#define __IRpcOptions_FWD_DEFINED__
typedef interface IRpcOptions IRpcOptions;
#endif 	/* __IRpcOptions_FWD_DEFINED__ */


#ifndef __IGlobalOptions_FWD_DEFINED__
#define __IGlobalOptions_FWD_DEFINED__
typedef interface IGlobalOptions IGlobalOptions;
#endif 	/* __IGlobalOptions_FWD_DEFINED__ */


#ifndef __IFillLockBytes_FWD_DEFINED__
#define __IFillLockBytes_FWD_DEFINED__
typedef interface IFillLockBytes IFillLockBytes;
#endif 	/* __IFillLockBytes_FWD_DEFINED__ */


#ifndef __IProgressNotify_FWD_DEFINED__
#define __IProgressNotify_FWD_DEFINED__
typedef interface IProgressNotify IProgressNotify;
#endif 	/* __IProgressNotify_FWD_DEFINED__ */


#ifndef __ILayoutStorage_FWD_DEFINED__
#define __ILayoutStorage_FWD_DEFINED__
typedef interface ILayoutStorage ILayoutStorage;
#endif 	/* __ILayoutStorage_FWD_DEFINED__ */


#ifndef __IBlockingLock_FWD_DEFINED__
#define __IBlockingLock_FWD_DEFINED__
typedef interface IBlockingLock IBlockingLock;
#endif 	/* __IBlockingLock_FWD_DEFINED__ */


#ifndef __ITimeAndNoticeControl_FWD_DEFINED__
#define __ITimeAndNoticeControl_FWD_DEFINED__
typedef interface ITimeAndNoticeControl ITimeAndNoticeControl;
#endif 	/* __ITimeAndNoticeControl_FWD_DEFINED__ */


#ifndef __IOplockStorage_FWD_DEFINED__
#define __IOplockStorage_FWD_DEFINED__
typedef interface IOplockStorage IOplockStorage;
#endif 	/* __IOplockStorage_FWD_DEFINED__ */


#ifndef __ISurrogate_FWD_DEFINED__
#define __ISurrogate_FWD_DEFINED__
typedef interface ISurrogate ISurrogate;
#endif 	/* __ISurrogate_FWD_DEFINED__ */


#ifndef __IGlobalInterfaceTable_FWD_DEFINED__
#define __IGlobalInterfaceTable_FWD_DEFINED__
typedef interface IGlobalInterfaceTable IGlobalInterfaceTable;
#endif 	/* __IGlobalInterfaceTable_FWD_DEFINED__ */


#ifndef __IDirectWriterLock_FWD_DEFINED__
#define __IDirectWriterLock_FWD_DEFINED__
typedef interface IDirectWriterLock IDirectWriterLock;
#endif 	/* __IDirectWriterLock_FWD_DEFINED__ */


#ifndef __ISynchronize_FWD_DEFINED__
#define __ISynchronize_FWD_DEFINED__
typedef interface ISynchronize ISynchronize;
#endif 	/* __ISynchronize_FWD_DEFINED__ */


#ifndef __ISynchronizeHandle_FWD_DEFINED__
#define __ISynchronizeHandle_FWD_DEFINED__
typedef interface ISynchronizeHandle ISynchronizeHandle;
#endif 	/* __ISynchronizeHandle_FWD_DEFINED__ */


#ifndef __ISynchronizeEvent_FWD_DEFINED__
#define __ISynchronizeEvent_FWD_DEFINED__
typedef interface ISynchronizeEvent ISynchronizeEvent;
#endif 	/* __ISynchronizeEvent_FWD_DEFINED__ */


#ifndef __ISynchronizeContainer_FWD_DEFINED__
#define __ISynchronizeContainer_FWD_DEFINED__
typedef interface ISynchronizeContainer ISynchronizeContainer;
#endif 	/* __ISynchronizeContainer_FWD_DEFINED__ */


#ifndef __ISynchronizeMutex_FWD_DEFINED__
#define __ISynchronizeMutex_FWD_DEFINED__
typedef interface ISynchronizeMutex ISynchronizeMutex;
#endif 	/* __ISynchronizeMutex_FWD_DEFINED__ */


#ifndef __ICancelMethodCalls_FWD_DEFINED__
#define __ICancelMethodCalls_FWD_DEFINED__
typedef interface ICancelMethodCalls ICancelMethodCalls;
#endif 	/* __ICancelMethodCalls_FWD_DEFINED__ */


#ifndef __IAsyncManager_FWD_DEFINED__
#define __IAsyncManager_FWD_DEFINED__
typedef interface IAsyncManager IAsyncManager;
#endif 	/* __IAsyncManager_FWD_DEFINED__ */


#ifndef __ICallFactory_FWD_DEFINED__
#define __ICallFactory_FWD_DEFINED__
typedef interface ICallFactory ICallFactory;
#endif 	/* __ICallFactory_FWD_DEFINED__ */


#ifndef __IRpcHelper_FWD_DEFINED__
#define __IRpcHelper_FWD_DEFINED__
typedef interface IRpcHelper IRpcHelper;
#endif 	/* __IRpcHelper_FWD_DEFINED__ */


#ifndef __IReleaseMarshalBuffers_FWD_DEFINED__
#define __IReleaseMarshalBuffers_FWD_DEFINED__
typedef interface IReleaseMarshalBuffers IReleaseMarshalBuffers;
#endif 	/* __IReleaseMarshalBuffers_FWD_DEFINED__ */


#ifndef __IWaitMultiple_FWD_DEFINED__
#define __IWaitMultiple_FWD_DEFINED__
typedef interface IWaitMultiple IWaitMultiple;
#endif 	/* __IWaitMultiple_FWD_DEFINED__ */


#ifndef __IUrlMon_FWD_DEFINED__
#define __IUrlMon_FWD_DEFINED__
typedef interface IUrlMon IUrlMon;
#endif 	/* __IUrlMon_FWD_DEFINED__ */


#ifndef __IForegroundTransfer_FWD_DEFINED__
#define __IForegroundTransfer_FWD_DEFINED__
typedef interface IForegroundTransfer IForegroundTransfer;
#endif 	/* __IForegroundTransfer_FWD_DEFINED__ */


#ifndef __IAddrTrackingControl_FWD_DEFINED__
#define __IAddrTrackingControl_FWD_DEFINED__
typedef interface IAddrTrackingControl IAddrTrackingControl;
#endif 	/* __IAddrTrackingControl_FWD_DEFINED__ */


#ifndef __IAddrExclusionControl_FWD_DEFINED__
#define __IAddrExclusionControl_FWD_DEFINED__
typedef interface IAddrExclusionControl IAddrExclusionControl;
#endif 	/* __IAddrExclusionControl_FWD_DEFINED__ */


#ifndef __IPipeByte_FWD_DEFINED__
#define __IPipeByte_FWD_DEFINED__
typedef interface IPipeByte IPipeByte;
#endif 	/* __IPipeByte_FWD_DEFINED__ */


#ifndef __AsyncIPipeByte_FWD_DEFINED__
#define __AsyncIPipeByte_FWD_DEFINED__
typedef interface AsyncIPipeByte AsyncIPipeByte;
#endif 	/* __AsyncIPipeByte_FWD_DEFINED__ */


#ifndef __IPipeLong_FWD_DEFINED__
#define __IPipeLong_FWD_DEFINED__
typedef interface IPipeLong IPipeLong;
#endif 	/* __IPipeLong_FWD_DEFINED__ */


#ifndef __AsyncIPipeLong_FWD_DEFINED__
#define __AsyncIPipeLong_FWD_DEFINED__
typedef interface AsyncIPipeLong AsyncIPipeLong;
#endif 	/* __AsyncIPipeLong_FWD_DEFINED__ */


#ifndef __IPipeDouble_FWD_DEFINED__
#define __IPipeDouble_FWD_DEFINED__
typedef interface IPipeDouble IPipeDouble;
#endif 	/* __IPipeDouble_FWD_DEFINED__ */


#ifndef __AsyncIPipeDouble_FWD_DEFINED__
#define __AsyncIPipeDouble_FWD_DEFINED__
typedef interface AsyncIPipeDouble AsyncIPipeDouble;
#endif 	/* __AsyncIPipeDouble_FWD_DEFINED__ */


#ifndef __IThumbnailExtractor_FWD_DEFINED__
#define __IThumbnailExtractor_FWD_DEFINED__
typedef interface IThumbnailExtractor IThumbnailExtractor;
#endif 	/* __IThumbnailExtractor_FWD_DEFINED__ */


#ifndef __IDummyHICONIncluder_FWD_DEFINED__
#define __IDummyHICONIncluder_FWD_DEFINED__
typedef interface IDummyHICONIncluder IDummyHICONIncluder;
#endif 	/* __IDummyHICONIncluder_FWD_DEFINED__ */


#ifndef __IEnumContextProps_FWD_DEFINED__
#define __IEnumContextProps_FWD_DEFINED__
typedef interface IEnumContextProps IEnumContextProps;
#endif 	/* __IEnumContextProps_FWD_DEFINED__ */


#ifndef __IContext_FWD_DEFINED__
#define __IContext_FWD_DEFINED__
typedef interface IContext IContext;
#endif 	/* __IContext_FWD_DEFINED__ */


#ifndef __IObjContext_FWD_DEFINED__
#define __IObjContext_FWD_DEFINED__
typedef interface IObjContext IObjContext;
#endif 	/* __IObjContext_FWD_DEFINED__ */


#ifndef __IProcessLock_FWD_DEFINED__
#define __IProcessLock_FWD_DEFINED__
typedef interface IProcessLock IProcessLock;
#endif 	/* __IProcessLock_FWD_DEFINED__ */


#ifndef __ISurrogateService_FWD_DEFINED__
#define __ISurrogateService_FWD_DEFINED__
typedef interface ISurrogateService ISurrogateService;
#endif 	/* __ISurrogateService_FWD_DEFINED__ */


#ifndef __IComThreadingInfo_FWD_DEFINED__
#define __IComThreadingInfo_FWD_DEFINED__
typedef interface IComThreadingInfo IComThreadingInfo;
#endif 	/* __IComThreadingInfo_FWD_DEFINED__ */


#ifndef __IProcessInitControl_FWD_DEFINED__
#define __IProcessInitControl_FWD_DEFINED__
typedef interface IProcessInitControl IProcessInitControl;
#endif 	/* __IProcessInitControl_FWD_DEFINED__ */


#ifndef __IInitializeSpy_FWD_DEFINED__
#define __IInitializeSpy_FWD_DEFINED__
typedef interface IInitializeSpy IInitializeSpy;
#endif 	/* __IInitializeSpy_FWD_DEFINED__ */


/* header files for imported files */
#include "unknwn.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_objidl_0000_0000 */
/* [local] */ 

//+-------------------------------------------------------------------------
//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (c) Microsoft Corporation. All rights reserved.
//
//--------------------------------------------------------------------------
#if(NTDDI_VERSION >= NTDDI_WINLH && !defined(_WIN32_WINNT))
#define _WIN32_WINNT 0x0600
#endif
#if(NTDDI_VERSION >= NTDDI_WS03 && !defined(_WIN32_WINNT))
#define _WIN32_WINNT 0x0502
#endif
#if(NTDDI_VERSION >= NTDDI_WINXP && !defined(_WIN32_WINNT))
#define _WIN32_WINNT 0x0501
#endif
#if(NTDDI_VERSION >= NTDDI_WIN2K && !defined(_WIN32_WINNT))
#define _WIN32_WINNT 0x0500
#endif



#if ( _MSC_VER >= 800 )
#if _MSC_VER >= 1200
#pragma warning(push)
#endif
#pragma warning(disable:4201)
#endif
#if ( _MSC_VER >= 1020 )
#pragma once
#endif














typedef struct _COSERVERINFO
    {
    DWORD dwReserved1;
    LPWSTR pwszName;
    COAUTHINFO *pAuthInfo;
    DWORD dwReserved2;
    } 	COSERVERINFO;




extern RPC_IF_HANDLE __MIDL_itf_objidl_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_objidl_0000_0000_v0_0_s_ifspec;

#ifndef __IMarshal_INTERFACE_DEFINED__
#define __IMarshal_INTERFACE_DEFINED__

/* interface IMarshal */
/* [uuid][object][local] */ 

typedef /* [unique] */ IMarshal *LPMARSHAL;


EXTERN_C const IID IID_IMarshal;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000003-0000-0000-C000-000000000046")
    IMarshal : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetUnmarshalClass( 
            /* [annotation][in] */ 
            __in  REFIID riid,
            /* [annotation][unique][in] */ 
            __in_opt  void *pv,
            /* [annotation][in] */ 
            __in  DWORD dwDestContext,
            /* [annotation][unique][in] */ 
            __reserved  void *pvDestContext,
            /* [annotation][in] */ 
            __in  DWORD mshlflags,
            /* [annotation][out] */ 
            __out  CLSID *pCid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMarshalSizeMax( 
            /* [annotation][in] */ 
            __in  REFIID riid,
            /* [annotation][unique][in] */ 
            __in_opt  void *pv,
            /* [annotation][in] */ 
            __in  DWORD dwDestContext,
            /* [annotation][unique][in] */ 
            __reserved  void *pvDestContext,
            /* [annotation][in] */ 
            __in  DWORD mshlflags,
            /* [annotation][out] */ 
            __out  DWORD *pSize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MarshalInterface( 
            /* [annotation][unique][in] */ 
            __in  IStream *pStm,
            /* [annotation][in] */ 
            __in  REFIID riid,
            /* [annotation][unique][in] */ 
            __in_opt  void *pv,
            /* [annotation][in] */ 
            __in  DWORD dwDestContext,
            /* [annotation][unique][in] */ 
            __reserved  void *pvDestContext,
            /* [annotation][in] */ 
            __in  DWORD mshlflags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UnmarshalInterface( 
            /* [annotation][unique][in] */ 
            __in  IStream *pStm,
            /* [annotation][in] */ 
            __in  REFIID riid,
            /* [annotation][out] */ 
            __deref_out  void **ppv) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ReleaseMarshalData( 
            /* [annotation][unique][in] */ 
            __in  IStream *pStm) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DisconnectObject( 
            /* [annotation][in] */ 
            __in  DWORD dwReserved) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMarshalVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMarshal * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMarshal * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMarshal * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetUnmarshalClass )( 
            IMarshal * This,
            /* [annotation][in] */ 
            __in  REFIID riid,
            /* [annotation][unique][in] */ 
            __in_opt  void *pv,
            /* [annotation][in] */ 
            __in  DWORD dwDestContext,
            /* [annotation][unique][in] */ 
            __reserved  void *pvDestContext,
            /* [annotation][in] */ 
            __in  DWORD mshlflags,
            /* [annotation][out] */ 
            __out  CLSID *pCid);
        
        HRESULT ( STDMETHODCALLTYPE *GetMarshalSizeMax )( 
            IMarshal * This,
            /* [annotation][in] */ 
            __in  REFIID riid,
            /* [annotation][unique][in] */ 
            __in_opt  void *pv,
            /* [annotation][in] */ 
            __in  DWORD dwDestContext,
            /* [annotation][unique][in] */ 
            __reserved  void *pvDestContext,
            /* [annotation][in] */ 
            __in  DWORD mshlflags,
            /* [annotation][out] */ 
            __out  DWORD *pSize);
        
        HRESULT ( STDMETHODCALLTYPE *MarshalInterface )( 
            IMarshal * This,
            /* [annotation][unique][in] */ 
            __in  IStream *pStm,
            /* [annotation][in] */ 
            __in  REFIID riid,
            /* [annotation][unique][in] */ 
            __in_opt  void *pv,
            /* [annotation][in] */ 
            __in  DWORD dwDestContext,
            /* [annotation][unique][in] */ 
            __reserved  void *pvDestContext,
            /* [annotation][in] */ 
            __in  DWORD mshlflags);
        
        HRESULT ( STDMETHODCALLTYPE *UnmarshalInterface )( 
            IMarshal * This,
            /* [annotation][unique][in] */ 
            __in  IStream *pStm,
            /* [annotation][in] */ 
            __in  REFIID riid,
            /* [annotation][out] */ 
            __deref_out  void **ppv);
        
        HRESULT ( STDMETHODCALLTYPE *ReleaseMarshalData )( 
            IMarshal * This,
            /* [annotation][unique][in] */ 
            __in  IStream *pStm);
        
        HRESULT ( STDMETHODCALLTYPE *DisconnectObject )( 
            IMarshal * This,
            /* [annotation][in] */ 
            __in  DWORD dwReserved);
        
        END_INTERFACE
    } IMarshalVtbl;

    interface IMarshal
    {
        CONST_VTBL struct IMarshalVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMarshal_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMarshal_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMarshal_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMarshal_GetUnmarshalClass(This,riid,pv,dwDestContext,pvDestContext,mshlflags,pCid)	\
    ( (This)->lpVtbl -> GetUnmarshalClass(This,riid,pv,dwDestContext,pvDestContext,mshlflags,pCid) ) 

#define IMarshal_GetMarshalSizeMax(This,riid,pv,dwDestContext,pvDestContext,mshlflags,pSize)	\
    ( (This)->lpVtbl -> GetMarshalSizeMax(This,riid,pv,dwDestContext,pvDestContext,mshlflags,pSize) ) 

#define IMarshal_MarshalInterface(This,pStm,riid,pv,dwDestContext,pvDestContext,mshlflags)	\
    ( (This)->lpVtbl -> MarshalInterface(This,pStm,riid,pv,dwDestContext,pvDestContext,mshlflags) ) 

#define IMarshal_UnmarshalInterface(This,pStm,riid,ppv)	\
    ( (This)->lpVtbl -> UnmarshalInterface(This,pStm,riid,ppv) ) 

#define IMarshal_ReleaseMarshalData(This,pStm)	\
    ( (This)->lpVtbl -> ReleaseMarshalData(This,pStm) ) 

#define IMarshal_DisconnectObject(This,dwReserved)	\
    ( (This)->lpVtbl -> DisconnectObject(This,dwReserved) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMarshal_INTERFACE_DEFINED__ */


#ifndef __IMarshal2_INTERFACE_DEFINED__
#define __IMarshal2_INTERFACE_DEFINED__

/* interface IMarshal2 */
/* [uuid][object][local] */ 

typedef /* [unique] */ IMarshal2 *LPMARSHAL2;


EXTERN_C const IID IID_IMarshal2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("000001cf-0000-0000-C000-000000000046")
    IMarshal2 : public IMarshal
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IMarshal2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMarshal2 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMarshal2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMarshal2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetUnmarshalClass )( 
            IMarshal2 * This,
            /* [annotation][in] */ 
            __in  REFIID riid,
            /* [annotation][unique][in] */ 
            __in_opt  void *pv,
            /* [annotation][in] */ 
            __in  DWORD dwDestContext,
            /* [annotation][unique][in] */ 
            __reserved  void *pvDestContext,
            /* [annotation][in] */ 
            __in  DWORD mshlflags,
            /* [annotation][out] */ 
            __out  CLSID *pCid);
        
        HRESULT ( STDMETHODCALLTYPE *GetMarshalSizeMax )( 
            IMarshal2 * This,
            /* [annotation][in] */ 
            __in  REFIID riid,
            /* [annotation][unique][in] */ 
            __in_opt  void *pv,
            /* [annotation][in] */ 
            __in  DWORD dwDestContext,
            /* [annotation][unique][in] */ 
            __reserved  void *pvDestContext,
            /* [annotation][in] */ 
            __in  DWORD mshlflags,
            /* [annotation][out] */ 
            __out  DWORD *pSize);
        
        HRESULT ( STDMETHODCALLTYPE *MarshalInterface )( 
            IMarshal2 * This,
            /* [annotation][unique][in] */ 
            __in  IStream *pStm,
            /* [annotation][in] */ 
            __in  REFIID riid,
            /* [annotation][unique][in] */ 
            __in_opt  void *pv,
            /* [annotation][in] */ 
            __in  DWORD dwDestContext,
            /* [annotation][unique][in] */ 
            __reserved  void *pvDestContext,
            /* [annotation][in] */ 
            __in  DWORD mshlflags);
        
        HRESULT ( STDMETHODCALLTYPE *UnmarshalInterface )( 
            IMarshal2 * This,
            /* [annotation][unique][in] */ 
            __in  IStream *pStm,
            /* [annotation][in] */ 
            __in  REFIID riid,
            /* [annotation][out] */ 
            __deref_out  void **ppv);
        
        HRESULT ( STDMETHODCALLTYPE *ReleaseMarshalData )( 
            IMarshal2 * This,
            /* [annotation][unique][in] */ 
            __in  IStream *pStm);
        
        HRESULT ( STDMETHODCALLTYPE *DisconnectObject )( 
            IMarshal2 * This,
            /* [annotation][in] */ 
            __in  DWORD dwReserved);
        
        END_INTERFACE
    } IMarshal2Vtbl;

    interface IMarshal2
    {
        CONST_VTBL struct IMarshal2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMarshal2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMarshal2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMarshal2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMarshal2_GetUnmarshalClass(This,riid,pv,dwDestContext,pvDestContext,mshlflags,pCid)	\
    ( (This)->lpVtbl -> GetUnmarshalClass(This,riid,pv,dwDestContext,pvDestContext,mshlflags,pCid) ) 

#define IMarshal2_GetMarshalSizeMax(This,riid,pv,dwDestContext,pvDestContext,mshlflags,pSize)	\
    ( (This)->lpVtbl -> GetMarshalSizeMax(This,riid,pv,dwDestContext,pvDestContext,mshlflags,pSize) ) 

#define IMarshal2_MarshalInterface(This,pStm,riid,pv,dwDestContext,pvDestContext,mshlflags)	\
    ( (This)->lpVtbl -> MarshalInterface(This,pStm,riid,pv,dwDestContext,pvDestContext,mshlflags) ) 

#define IMarshal2_UnmarshalInterface(This,pStm,riid,ppv)	\
    ( (This)->lpVtbl -> UnmarshalInterface(This,pStm,riid,ppv) ) 

#define IMarshal2_ReleaseMarshalData(This,pStm)	\
    ( (This)->lpVtbl -> ReleaseMarshalData(This,pStm) ) 

#define IMarshal2_DisconnectObject(This,dwReserved)	\
    ( (This)->lpVtbl -> DisconnectObject(This,dwReserved) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMarshal2_INTERFACE_DEFINED__ */


#ifndef __IMalloc_INTERFACE_DEFINED__
#define __IMalloc_INTERFACE_DEFINED__

/* interface IMalloc */
/* [uuid][object][local] */ 

typedef /* [unique] */ IMalloc *LPMALLOC;


EXTERN_C const IID IID_IMalloc;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000002-0000-0000-C000-000000000046")
    IMalloc : public IUnknown
    {
    public:
        virtual void *STDMETHODCALLTYPE Alloc( 
            /* [annotation][in] */ 
            __in  SIZE_T cb) = 0;
        
        virtual void *STDMETHODCALLTYPE Realloc( 
            /* [annotation][in] */ 
            __in_opt  void *pv,
            /* [annotation][in] */ 
            __in  SIZE_T cb) = 0;
        
        virtual void STDMETHODCALLTYPE Free( 
            /* [annotation][in] */ 
            __in_opt  void *pv) = 0;
        
        virtual SIZE_T STDMETHODCALLTYPE GetSize( 
            /* [annotation][in] */ 
            __in_opt  void *pv) = 0;
        
        virtual int STDMETHODCALLTYPE DidAlloc( 
            /* [annotation][in] */ 
            __in_opt  void *pv) = 0;
        
        virtual void STDMETHODCALLTYPE HeapMinimize( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMallocVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMalloc * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMalloc * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMalloc * This);
        
        void *( STDMETHODCALLTYPE *Alloc )( 
            IMalloc * This,
            /* [annotation][in] */ 
            __in  SIZE_T cb);
        
        void *( STDMETHODCALLTYPE *Realloc )( 
            IMalloc * This,
            /* [annotation][in] */ 
            __in_opt  void *pv,
            /* [annotation][in] */ 
            __in  SIZE_T cb);
        
        void ( STDMETHODCALLTYPE *Free )( 
            IMalloc * This,
            /* [annotation][in] */ 
            __in_opt  void *pv);
        
        SIZE_T ( STDMETHODCALLTYPE *GetSize )( 
            IMalloc * This,
            /* [annotation][in] */ 
            __in_opt  void *pv);
        
        int ( STDMETHODCALLTYPE *DidAlloc )( 
            IMalloc * This,
            /* [annotation][in] */ 
            __in_opt  void *pv);
        
        void ( STDMETHODCALLTYPE *HeapMinimize )( 
            IMalloc * This);
        
        END_INTERFACE
    } IMallocVtbl;

    interface IMalloc
    {
        CONST_VTBL struct IMallocVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMalloc_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMalloc_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMalloc_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMalloc_Alloc(This,cb)	\
    ( (This)->lpVtbl -> Alloc(This,cb) ) 

#define IMalloc_Realloc(This,pv,cb)	\
    ( (This)->lpVtbl -> Realloc(This,pv,cb) ) 

#define IMalloc_Free(This,pv)	\
    ( (This)->lpVtbl -> Free(This,pv) ) 

#define IMalloc_GetSize(This,pv)	\
    ( (This)->lpVtbl -> GetSize(This,pv) ) 

#define IMalloc_DidAlloc(This,pv)	\
    ( (This)->lpVtbl -> DidAlloc(This,pv) ) 

#define IMalloc_HeapMinimize(This)	\
    ( (This)->lpVtbl -> HeapMinimize(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMalloc_INTERFACE_DEFINED__ */


#ifndef __IMallocSpy_INTERFACE_DEFINED__
#define __IMallocSpy_INTERFACE_DEFINED__

/* interface IMallocSpy */
/* [uuid][object][local] */ 

typedef /* [unique] */ IMallocSpy *LPMALLOCSPY;


EXTERN_C const IID IID_IMallocSpy;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0000001d-0000-0000-C000-000000000046")
    IMallocSpy : public IUnknown
    {
    public:
        virtual SIZE_T STDMETHODCALLTYPE PreAlloc( 
            /* [annotation][in] */ 
            __in  SIZE_T cbRequest) = 0;
        
        virtual void *STDMETHODCALLTYPE PostAlloc( 
            /* [annotation][in] */ 
            __in  void *pActual) = 0;
        
        virtual void *STDMETHODCALLTYPE PreFree( 
            /* [annotation][in] */ 
            __in  void *pRequest,
            /* [annotation][in] */ 
            __in  BOOL fSpyed) = 0;
        
        virtual void STDMETHODCALLTYPE PostFree( 
            /* [annotation][in] */ 
            __in  BOOL fSpyed) = 0;
        
        virtual SIZE_T STDMETHODCALLTYPE PreRealloc( 
            /* [annotation][in] */ 
            __in  void *pRequest,
            /* [annotation][in] */ 
            __in  SIZE_T cbRequest,
            /* [annotation][out] */ 
            __deref_out  void **ppNewRequest,
            /* [annotation][in] */ 
            __in  BOOL fSpyed) = 0;
        
        virtual void *STDMETHODCALLTYPE PostRealloc( 
            /* [annotation][in] */ 
            __in  void *pActual,
            /* [annotation][in] */ 
            __in  BOOL fSpyed) = 0;
        
        virtual void *STDMETHODCALLTYPE PreGetSize( 
            /* [annotation][in] */ 
            __in  void *pRequest,
            /* [annotation][in] */ 
            __in  BOOL fSpyed) = 0;
        
        virtual SIZE_T STDMETHODCALLTYPE PostGetSize( 
            /* [annotation][in] */ 
            __in  SIZE_T cbActual,
            /* [annotation][in] */ 
            __in  BOOL fSpyed) = 0;
        
        virtual void *STDMETHODCALLTYPE PreDidAlloc( 
            /* [annotation][in] */ 
            __in  void *pRequest,
            /* [annotation][in] */ 
            __in  BOOL fSpyed) = 0;
        
        virtual int STDMETHODCALLTYPE PostDidAlloc( 
            /* [annotation][in] */ 
            __in  void *pRequest,
            /* [annotation][in] */ 
            __in  BOOL fSpyed,
            /* [annotation][in] */ 
            __in  int fActual) = 0;
        
        virtual void STDMETHODCALLTYPE PreHeapMinimize( void) = 0;
        
        virtual void STDMETHODCALLTYPE PostHeapMinimize( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMallocSpyVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMallocSpy * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMallocSpy * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMallocSpy * This);
        
        SIZE_T ( STDMETHODCALLTYPE *PreAlloc )( 
            IMallocSpy * This,
            /* [annotation][in] */ 
            __in  SIZE_T cbRequest);
        
        void *( STDMETHODCALLTYPE *PostAlloc )( 
            IMallocSpy * This,
            /* [annotation][in] */ 
            __in  void *pActual);
        
        void *( STDMETHODCALLTYPE *PreFree )( 
            IMallocSpy * This,
            /* [annotation][in] */ 
            __in  void *pRequest,
            /* [annotation][in] */ 
            __in  BOOL fSpyed);
        
        void ( STDMETHODCALLTYPE *PostFree )( 
            IMallocSpy * This,
            /* [annotation][in] */ 
            __in  BOOL fSpyed);
        
        SIZE_T ( STDMETHODCALLTYPE *PreRealloc )( 
            IMallocSpy * This,
            /* [annotation][in] */ 
            __in  void *pRequest,
            /* [annotation][in] */ 
            __in  SIZE_T cbRequest,
            /* [annotation][out] */ 
            __deref_out  void **ppNewRequest,
            /* [annotation][in] */ 
            __in  BOOL fSpyed);
        
        void *( STDMETHODCALLTYPE *PostRealloc )( 
            IMallocSpy * This,
            /* [annotation][in] */ 
            __in  void *pActual,
            /* [annotation][in] */ 
            __in  BOOL fSpyed);
        
        void *( STDMETHODCALLTYPE *PreGetSize )( 
            IMallocSpy * This,
            /* [annotation][in] */ 
            __in  void *pRequest,
            /* [annotation][in] */ 
            __in  BOOL fSpyed);
        
        SIZE_T ( STDMETHODCALLTYPE *PostGetSize )( 
            IMallocSpy * This,
            /* [annotation][in] */ 
            __in  SIZE_T cbActual,
            /* [annotation][in] */ 
            __in  BOOL fSpyed);
        
        void *( STDMETHODCALLTYPE *PreDidAlloc )( 
            IMallocSpy * This,
            /* [annotation][in] */ 
            __in  void *pRequest,
            /* [annotation][in] */ 
            __in  BOOL fSpyed);
        
        int ( STDMETHODCALLTYPE *PostDidAlloc )( 
            IMallocSpy * This,
            /* [annotation][in] */ 
            __in  void *pRequest,
            /* [annotation][in] */ 
            __in  BOOL fSpyed,
            /* [annotation][in] */ 
            __in  int fActual);
        
        void ( STDMETHODCALLTYPE *PreHeapMinimize )( 
            IMallocSpy * This);
        
        void ( STDMETHODCALLTYPE *PostHeapMinimize )( 
            IMallocSpy * This);
        
        END_INTERFACE
    } IMallocSpyVtbl;

    interface IMallocSpy
    {
        CONST_VTBL struct IMallocSpyVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMallocSpy_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMallocSpy_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMallocSpy_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMallocSpy_PreAlloc(This,cbRequest)	\
    ( (This)->lpVtbl -> PreAlloc(This,cbRequest) ) 

#define IMallocSpy_PostAlloc(This,pActual)	\
    ( (This)->lpVtbl -> PostAlloc(This,pActual) ) 

#define IMallocSpy_PreFree(This,pRequest,fSpyed)	\
    ( (This)->lpVtbl -> PreFree(This,pRequest,fSpyed) ) 

#define IMallocSpy_PostFree(This,fSpyed)	\
    ( (This)->lpVtbl -> PostFree(This,fSpyed) ) 

#define IMallocSpy_PreRealloc(This,pRequest,cbRequest,ppNewRequest,fSpyed)	\
    ( (This)->lpVtbl -> PreRealloc(This,pRequest,cbRequest,ppNewRequest,fSpyed) ) 

#define IMallocSpy_PostRealloc(This,pActual,fSpyed)	\
    ( (This)->lpVtbl -> PostRealloc(This,pActual,fSpyed) ) 

#define IMallocSpy_PreGetSize(This,pRequest,fSpyed)	\
    ( (This)->lpVtbl -> PreGetSize(This,pRequest,fSpyed) ) 

#define IMallocSpy_PostGetSize(This,cbActual,fSpyed)	\
    ( (This)->lpVtbl -> PostGetSize(This,cbActual,fSpyed) ) 

#define IMallocSpy_PreDidAlloc(This,pRequest,fSpyed)	\
    ( (This)->lpVtbl -> PreDidAlloc(This,pRequest,fSpyed) ) 

#define IMallocSpy_PostDidAlloc(This,pRequest,fSpyed,fActual)	\
    ( (This)->lpVtbl -> PostDidAlloc(This,pRequest,fSpyed,fActual) ) 

#define IMallocSpy_PreHeapMinimize(This)	\
    ( (This)->lpVtbl -> PreHeapMinimize(This) ) 

#define IMallocSpy_PostHeapMinimize(This)	\
    ( (This)->lpVtbl -> PostHeapMinimize(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMallocSpy_INTERFACE_DEFINED__ */


#ifndef __IStdMarshalInfo_INTERFACE_DEFINED__
#define __IStdMarshalInfo_INTERFACE_DEFINED__

/* interface IStdMarshalInfo */
/* [uuid][object][local] */ 

typedef /* [unique] */ IStdMarshalInfo *LPSTDMARSHALINFO;


EXTERN_C const IID IID_IStdMarshalInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000018-0000-0000-C000-000000000046")
    IStdMarshalInfo : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetClassForHandler( 
            /* [annotation][in] */ 
            __in  DWORD dwDestContext,
            /* [annotation][unique][in] */ 
            __reserved  void *pvDestContext,
            /* [annotation][out] */ 
            __out  CLSID *pClsid) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IStdMarshalInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IStdMarshalInfo * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IStdMarshalInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IStdMarshalInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetClassForHandler )( 
            IStdMarshalInfo * This,
            /* [annotation][in] */ 
            __in  DWORD dwDestContext,
            /* [annotation][unique][in] */ 
            __reserved  void *pvDestContext,
            /* [annotation][out] */ 
            __out  CLSID *pClsid);
        
        END_INTERFACE
    } IStdMarshalInfoVtbl;

    interface IStdMarshalInfo
    {
        CONST_VTBL struct IStdMarshalInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IStdMarshalInfo_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IStdMarshalInfo_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IStdMarshalInfo_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IStdMarshalInfo_GetClassForHandler(This,dwDestContext,pvDestContext,pClsid)	\
    ( (This)->lpVtbl -> GetClassForHandler(This,dwDestContext,pvDestContext,pClsid) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IStdMarshalInfo_INTERFACE_DEFINED__ */


#ifndef __IExternalConnection_INTERFACE_DEFINED__
#define __IExternalConnection_INTERFACE_DEFINED__

/* interface IExternalConnection */
/* [uuid][local][object] */ 

typedef /* [unique] */ IExternalConnection *LPEXTERNALCONNECTION;

typedef 
enum tagEXTCONN
    {	EXTCONN_STRONG	= 0x1,
	EXTCONN_WEAK	= 0x2,
	EXTCONN_CALLABLE	= 0x4
    } 	EXTCONN;


EXTERN_C const IID IID_IExternalConnection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000019-0000-0000-C000-000000000046")
    IExternalConnection : public IUnknown
    {
    public:
        virtual DWORD STDMETHODCALLTYPE AddConnection( 
            /* [annotation][in] */ 
            __in  DWORD extconn,
            /* [annotation][in] */ 
            __in  DWORD reserved) = 0;
        
        virtual DWORD STDMETHODCALLTYPE ReleaseConnection( 
            /* [annotation][in] */ 
            __in  DWORD extconn,
            /* [annotation][in] */ 
            __in  DWORD reserved,
            /* [annotation][in] */ 
            __in  BOOL fLastReleaseCloses) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IExternalConnectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IExternalConnection * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IExternalConnection * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IExternalConnection * This);
        
        DWORD ( STDMETHODCALLTYPE *AddConnection )( 
            IExternalConnection * This,
            /* [annotation][in] */ 
            __in  DWORD extconn,
            /* [annotation][in] */ 
            __in  DWORD reserved);
        
        DWORD ( STDMETHODCALLTYPE *ReleaseConnection )( 
            IExternalConnection * This,
            /* [annotation][in] */ 
            __in  DWORD extconn,
            /* [annotation][in] */ 
            __in  DWORD reserved,
            /* [annotation][in] */ 
            __in  BOOL fLastReleaseCloses);
        
        END_INTERFACE
    } IExternalConnectionVtbl;

    interface IExternalConnection
    {
        CONST_VTBL struct IExternalConnectionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IExternalConnection_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IExternalConnection_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IExternalConnection_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IExternalConnection_AddConnection(This,extconn,reserved)	\
    ( (This)->lpVtbl -> AddConnection(This,extconn,reserved) ) 

#define IExternalConnection_ReleaseConnection(This,extconn,reserved,fLastReleaseCloses)	\
    ( (This)->lpVtbl -> ReleaseConnection(This,extconn,reserved,fLastReleaseCloses) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IExternalConnection_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_objidl_0000_0006 */
/* [local] */ 

typedef /* [unique] */  __RPC_unique_pointer IMultiQI *LPMULTIQI;

typedef struct tagMULTI_QI
    {
    const IID *pIID;
    IUnknown *pItf;
    HRESULT hr;
    } 	MULTI_QI;



extern RPC_IF_HANDLE __MIDL_itf_objidl_0000_0006_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_objidl_0000_0006_v0_0_s_ifspec;

#ifndef __IMultiQI_INTERFACE_DEFINED__
#define __IMultiQI_INTERFACE_DEFINED__

/* interface IMultiQI */
/* [async_uuid][uuid][local][object] */ 


EXTERN_C const IID IID_IMultiQI;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000020-0000-0000-C000-000000000046")
    IMultiQI : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE QueryMultipleInterfaces( 
            /* [annotation][in] */ 
            __in  ULONG cMQIs,
            /* [annotation][out][in] */ 
            __inout  MULTI_QI *pMQIs) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMultiQIVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMultiQI * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMultiQI * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMultiQI * This);
        
        HRESULT ( STDMETHODCALLTYPE *QueryMultipleInterfaces )( 
            IMultiQI * This,
            /* [annotation][in] */ 
            __in  ULONG cMQIs,
            /* [annotation][out][in] */ 
            __inout  MULTI_QI *pMQIs);
        
        END_INTERFACE
    } IMultiQIVtbl;

    interface IMultiQI
    {
        CONST_VTBL struct IMultiQIVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMultiQI_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMultiQI_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMultiQI_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMultiQI_QueryMultipleInterfaces(This,cMQIs,pMQIs)	\
    ( (This)->lpVtbl -> QueryMultipleInterfaces(This,cMQIs,pMQIs) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMultiQI_INTERFACE_DEFINED__ */


#ifndef __AsyncIMultiQI_INTERFACE_DEFINED__
#define __AsyncIMultiQI_INTERFACE_DEFINED__

/* interface AsyncIMultiQI */
/* [uuid][local][object] */ 


EXTERN_C const IID IID_AsyncIMultiQI;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("000e0020-0000-0000-C000-000000000046")
    AsyncIMultiQI : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Begin_QueryMultipleInterfaces( 
            /* [annotation][in] */ 
            __in  ULONG cMQIs,
            /* [annotation][out][in] */ 
            __inout  MULTI_QI *pMQIs) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Finish_QueryMultipleInterfaces( 
            /* [annotation][out][in] */ 
            __inout  MULTI_QI *pMQIs) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct AsyncIMultiQIVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            AsyncIMultiQI * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            AsyncIMultiQI * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            AsyncIMultiQI * This);
        
        HRESULT ( STDMETHODCALLTYPE *Begin_QueryMultipleInterfaces )( 
            AsyncIMultiQI * This,
            /* [annotation][in] */ 
            __in  ULONG cMQIs,
            /* [annotation][out][in] */ 
            __inout  MULTI_QI *pMQIs);
        
        HRESULT ( STDMETHODCALLTYPE *Finish_QueryMultipleInterfaces )( 
            AsyncIMultiQI * This,
            /* [annotation][out][in] */ 
            __inout  MULTI_QI *pMQIs);
        
        END_INTERFACE
    } AsyncIMultiQIVtbl;

    interface AsyncIMultiQI
    {
        CONST_VTBL struct AsyncIMultiQIVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define AsyncIMultiQI_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define AsyncIMultiQI_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define AsyncIMultiQI_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define AsyncIMultiQI_Begin_QueryMultipleInterfaces(This,cMQIs,pMQIs)	\
    ( (This)->lpVtbl -> Begin_QueryMultipleInterfaces(This,cMQIs,pMQIs) ) 

#define AsyncIMultiQI_Finish_QueryMultipleInterfaces(This,pMQIs)	\
    ( (This)->lpVtbl -> Finish_QueryMultipleInterfaces(This,pMQIs) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __AsyncIMultiQI_INTERFACE_DEFINED__ */


#ifndef __IInternalUnknown_INTERFACE_DEFINED__
#define __IInternalUnknown_INTERFACE_DEFINED__

/* interface IInternalUnknown */
/* [uuid][local][object] */ 


EXTERN_C const IID IID_IInternalUnknown;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000021-0000-0000-C000-000000000046")
    IInternalUnknown : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE QueryInternalInterface( 
            /* [annotation][in] */ 
            __in  REFIID riid,
            /* [annotation][out] */ 
            __deref_out  void **ppv) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IInternalUnknownVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IInternalUnknown * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IInternalUnknown * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IInternalUnknown * This);
        
        HRESULT ( STDMETHODCALLTYPE *QueryInternalInterface )( 
            IInternalUnknown * This,
            /* [annotation][in] */ 
            __in  REFIID riid,
            /* [annotation][out] */ 
            __deref_out  void **ppv);
        
        END_INTERFACE
    } IInternalUnknownVtbl;

    interface IInternalUnknown
    {
        CONST_VTBL struct IInternalUnknownVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IInternalUnknown_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IInternalUnknown_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IInternalUnknown_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IInternalUnknown_QueryInternalInterface(This,riid,ppv)	\
    ( (This)->lpVtbl -> QueryInternalInterface(This,riid,ppv) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IInternalUnknown_INTERFACE_DEFINED__ */


#ifndef __IEnumUnknown_INTERFACE_DEFINED__
#define __IEnumUnknown_INTERFACE_DEFINED__

/* interface IEnumUnknown */
/* [unique][uuid][object] */ 

typedef /* [unique] */  __RPC_unique_pointer IEnumUnknown *LPENUMUNKNOWN;


EXTERN_C const IID IID_IEnumUnknown;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000100-0000-0000-C000-000000000046")
    IEnumUnknown : public IUnknown
    {
    public:
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE Next( 
            /* [annotation][in] */ 
            __in  ULONG celt,
            /* [annotation][out] */ 
            __out_ecount_part(celt,*pceltFetched)  IUnknown **rgelt,
            /* [annotation][out] */ 
            __out_opt  ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ __RPC__deref_out_opt IEnumUnknown **ppenum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumUnknownVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IEnumUnknown * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IEnumUnknown * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IEnumUnknown * This);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumUnknown * This,
            /* [annotation][in] */ 
            __in  ULONG celt,
            /* [annotation][out] */ 
            __out_ecount_part(celt,*pceltFetched)  IUnknown **rgelt,
            /* [annotation][out] */ 
            __out_opt  ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            __RPC__in IEnumUnknown * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            __RPC__in IEnumUnknown * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            __RPC__in IEnumUnknown * This,
            /* [out] */ __RPC__deref_out_opt IEnumUnknown **ppenum);
        
        END_INTERFACE
    } IEnumUnknownVtbl;

    interface IEnumUnknown
    {
        CONST_VTBL struct IEnumUnknownVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumUnknown_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IEnumUnknown_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IEnumUnknown_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IEnumUnknown_Next(This,celt,rgelt,pceltFetched)	\
    ( (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched) ) 

#define IEnumUnknown_Skip(This,celt)	\
    ( (This)->lpVtbl -> Skip(This,celt) ) 

#define IEnumUnknown_Reset(This)	\
    ( (This)->lpVtbl -> Reset(This) ) 

#define IEnumUnknown_Clone(This,ppenum)	\
    ( (This)->lpVtbl -> Clone(This,ppenum) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT STDMETHODCALLTYPE IEnumUnknown_RemoteNext_Proxy( 
    __RPC__in IEnumUnknown * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ __RPC__out_ecount_part(celt, *pceltFetched) IUnknown **rgelt,
    /* [out] */ __RPC__out ULONG *pceltFetched);


void __RPC_STUB IEnumUnknown_RemoteNext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumUnknown_INTERFACE_DEFINED__ */


#ifndef __IBindCtx_INTERFACE_DEFINED__
#define __IBindCtx_INTERFACE_DEFINED__

/* interface IBindCtx */
/* [unique][uuid][object] */ 

typedef /* [unique] */  __RPC_unique_pointer IBindCtx *LPBC;

typedef /* [unique] */  __RPC_unique_pointer IBindCtx *LPBINDCTX;

#if defined(__cplusplus)
    typedef __struct_bcount(cbStruct) struct tagBIND_OPTS {
    DWORD           cbStruct;
    DWORD           grfFlags;
    DWORD           grfMode;
    DWORD           dwTickCountDeadline;
    } BIND_OPTS, * LPBIND_OPTS;
#else
typedef struct tagBIND_OPTS
    {
    DWORD cbStruct;
    DWORD grfFlags;
    DWORD grfMode;
    DWORD dwTickCountDeadline;
    } 	BIND_OPTS;

typedef struct tagBIND_OPTS *LPBIND_OPTS;

#endif
#if defined(__cplusplus)
    typedef struct tagBIND_OPTS2 : tagBIND_OPTS {
    DWORD           dwTrackFlags;
    DWORD           dwClassContext;
    LCID            locale;
    COSERVERINFO *  pServerInfo;
    } BIND_OPTS2, * LPBIND_OPTS2;
#else

typedef struct tagBIND_OPTS2
    {
    DWORD cbStruct;
    DWORD grfFlags;
    DWORD grfMode;
    DWORD dwTickCountDeadline;
    DWORD dwTrackFlags;
    DWORD dwClassContext;
    LCID locale;
    COSERVERINFO *pServerInfo;
    } 	BIND_OPTS2;

typedef struct tagBIND_OPTS2 *LPBIND_OPTS2;

#endif
#if defined(__cplusplus)
    typedef struct tagBIND_OPTS3 : tagBIND_OPTS2 {
   HWND           hwnd;
    } BIND_OPTS3, * LPBIND_OPTS3;
#else
typedef struct tagBIND_OPTS3
    {
    DWORD cbStruct;
    DWORD grfFlags;
    DWORD grfMode;
    DWORD dwTickCountDeadline;
    DWORD dwTrackFlags;
    DWORD dwClassContext;
    LCID locale;
    COSERVERINFO *pServerInfo;
    HWND hwnd;
    } 	BIND_OPTS3;

typedef struct tagBIND_OPTS3 *LPBIND_OPTS3;

#endif
typedef 
enum tagBIND_FLAGS
    {	BIND_MAYBOTHERUSER	= 1,
	BIND_JUSTTESTEXISTENCE	= 2
    } 	BIND_FLAGS;


EXTERN_C const IID IID_IBindCtx;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0000000e-0000-0000-C000-000000000046")
    IBindCtx : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE RegisterObjectBound( 
            /* [unique][in] */ __RPC__in_opt IUnknown *punk) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RevokeObjectBound( 
            /* [unique][in] */ __RPC__in_opt IUnknown *punk) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ReleaseBoundObjects( void) = 0;
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE SetBindOptions( 
            /* [in] */ BIND_OPTS *pbindopts) = 0;
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE GetBindOptions( 
            /* [out][in] */ BIND_OPTS *pbindopts) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetRunningObjectTable( 
            /* [out] */ __RPC__deref_out_opt IRunningObjectTable **pprot) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RegisterObjectParam( 
            /* [in] */ __RPC__in LPOLESTR pszKey,
            /* [unique][in] */ __RPC__in_opt IUnknown *punk) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetObjectParam( 
            /* [in] */ __RPC__in LPOLESTR pszKey,
            /* [out] */ __RPC__deref_out_opt IUnknown **ppunk) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumObjectParam( 
            /* [out] */ __RPC__deref_out_opt IEnumString **ppenum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RevokeObjectParam( 
            /* [in] */ __RPC__in LPOLESTR pszKey) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBindCtxVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IBindCtx * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IBindCtx * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IBindCtx * This);
        
        HRESULT ( STDMETHODCALLTYPE *RegisterObjectBound )( 
            __RPC__in IBindCtx * This,
            /* [unique][in] */ __RPC__in_opt IUnknown *punk);
        
        HRESULT ( STDMETHODCALLTYPE *RevokeObjectBound )( 
            __RPC__in IBindCtx * This,
            /* [unique][in] */ __RPC__in_opt IUnknown *punk);
        
        HRESULT ( STDMETHODCALLTYPE *ReleaseBoundObjects )( 
            __RPC__in IBindCtx * This);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *SetBindOptions )( 
            IBindCtx * This,
            /* [in] */ BIND_OPTS *pbindopts);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *GetBindOptions )( 
            IBindCtx * This,
            /* [out][in] */ BIND_OPTS *pbindopts);
        
        HRESULT ( STDMETHODCALLTYPE *GetRunningObjectTable )( 
            __RPC__in IBindCtx * This,
            /* [out] */ __RPC__deref_out_opt IRunningObjectTable **pprot);
        
        HRESULT ( STDMETHODCALLTYPE *RegisterObjectParam )( 
            __RPC__in IBindCtx * This,
            /* [in] */ __RPC__in LPOLESTR pszKey,
            /* [unique][in] */ __RPC__in_opt IUnknown *punk);
        
        HRESULT ( STDMETHODCALLTYPE *GetObjectParam )( 
            __RPC__in IBindCtx * This,
            /* [in] */ __RPC__in LPOLESTR pszKey,
            /* [out] */ __RPC__deref_out_opt IUnknown **ppunk);
        
        HRESULT ( STDMETHODCALLTYPE *EnumObjectParam )( 
            __RPC__in IBindCtx * This,
            /* [out] */ __RPC__deref_out_opt IEnumString **ppenum);
        
        HRESULT ( STDMETHODCALLTYPE *RevokeObjectParam )( 
            __RPC__in IBindCtx * This,
            /* [in] */ __RPC__in LPOLESTR pszKey);
        
        END_INTERFACE
    } IBindCtxVtbl;

    interface IBindCtx
    {
        CONST_VTBL struct IBindCtxVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBindCtx_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IBindCtx_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IBindCtx_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IBindCtx_RegisterObjectBound(This,punk)	\
    ( (This)->lpVtbl -> RegisterObjectBound(This,punk) ) 

#define IBindCtx_RevokeObjectBound(This,punk)	\
    ( (This)->lpVtbl -> RevokeObjectBound(This,punk) ) 

#define IBindCtx_ReleaseBoundObjects(This)	\
    ( (This)->lpVtbl -> ReleaseBoundObjects(This) ) 

#define IBindCtx_SetBindOptions(This,pbindopts)	\
    ( (This)->lpVtbl -> SetBindOptions(This,pbindopts) ) 

#define IBindCtx_GetBindOptions(This,pbindopts)	\
    ( (This)->lpVtbl -> GetBindOptions(This,pbindopts) ) 

#define IBindCtx_GetRunningObjectTable(This,pprot)	\
    ( (This)->lpVtbl -> GetRunningObjectTable(This,pprot) ) 

#define IBindCtx_RegisterObjectParam(This,pszKey,punk)	\
    ( (This)->lpVtbl -> RegisterObjectParam(This,pszKey,punk) ) 

#define IBindCtx_GetObjectParam(This,pszKey,ppunk)	\
    ( (This)->lpVtbl -> GetObjectParam(This,pszKey,ppunk) ) 

#define IBindCtx_EnumObjectParam(This,ppenum)	\
    ( (This)->lpVtbl -> EnumObjectParam(This,ppenum) ) 

#define IBindCtx_RevokeObjectParam(This,pszKey)	\
    ( (This)->lpVtbl -> RevokeObjectParam(This,pszKey) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT STDMETHODCALLTYPE IBindCtx_RemoteSetBindOptions_Proxy( 
    __RPC__in IBindCtx * This,
    /* [in] */ __RPC__in BIND_OPTS2 *pbindopts);


void __RPC_STUB IBindCtx_RemoteSetBindOptions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IBindCtx_RemoteGetBindOptions_Proxy( 
    __RPC__in IBindCtx * This,
    /* [out][in] */ __RPC__inout BIND_OPTS2 *pbindopts);


void __RPC_STUB IBindCtx_RemoteGetBindOptions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IBindCtx_INTERFACE_DEFINED__ */


#ifndef __IEnumMoniker_INTERFACE_DEFINED__
#define __IEnumMoniker_INTERFACE_DEFINED__

/* interface IEnumMoniker */
/* [unique][uuid][object] */ 

typedef /* [unique] */  __RPC_unique_pointer IEnumMoniker *LPENUMMONIKER;


EXTERN_C const IID IID_IEnumMoniker;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000102-0000-0000-C000-000000000046")
    IEnumMoniker : public IUnknown
    {
    public:
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [annotation] */ 
            __out_ecount_part(celt,*pceltFetched)  IMoniker **rgelt,
            /* [annotation] */ 
            __out_opt  ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ __RPC__deref_out_opt IEnumMoniker **ppenum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumMonikerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IEnumMoniker * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IEnumMoniker * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IEnumMoniker * This);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumMoniker * This,
            /* [in] */ ULONG celt,
            /* [annotation] */ 
            __out_ecount_part(celt,*pceltFetched)  IMoniker **rgelt,
            /* [annotation] */ 
            __out_opt  ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            __RPC__in IEnumMoniker * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            __RPC__in IEnumMoniker * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            __RPC__in IEnumMoniker * This,
            /* [out] */ __RPC__deref_out_opt IEnumMoniker **ppenum);
        
        END_INTERFACE
    } IEnumMonikerVtbl;

    interface IEnumMoniker
    {
        CONST_VTBL struct IEnumMonikerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumMoniker_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IEnumMoniker_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IEnumMoniker_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IEnumMoniker_Next(This,celt,rgelt,pceltFetched)	\
    ( (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched) ) 

#define IEnumMoniker_Skip(This,celt)	\
    ( (This)->lpVtbl -> Skip(This,celt) ) 

#define IEnumMoniker_Reset(This)	\
    ( (This)->lpVtbl -> Reset(This) ) 

#define IEnumMoniker_Clone(This,ppenum)	\
    ( (This)->lpVtbl -> Clone(This,ppenum) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT STDMETHODCALLTYPE IEnumMoniker_RemoteNext_Proxy( 
    __RPC__in IEnumMoniker * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ __RPC__out_ecount_part(celt, *pceltFetched) IMoniker **rgelt,
    /* [out] */ __RPC__out ULONG *pceltFetched);


void __RPC_STUB IEnumMoniker_RemoteNext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumMoniker_INTERFACE_DEFINED__ */


#ifndef __IRunnableObject_INTERFACE_DEFINED__
#define __IRunnableObject_INTERFACE_DEFINED__

/* interface IRunnableObject */
/* [uuid][object] */ 

typedef /* [unique] */  __RPC_unique_pointer IRunnableObject *LPRUNNABLEOBJECT;


EXTERN_C const IID IID_IRunnableObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000126-0000-0000-C000-000000000046")
    IRunnableObject : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetRunningClass( 
            /* [out] */ __RPC__out LPCLSID lpClsid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Run( 
            /* [in] */ __RPC__in_opt LPBINDCTX pbc) = 0;
        
        virtual /* [local] */ BOOL STDMETHODCALLTYPE IsRunning( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE LockRunning( 
            /* [in] */ BOOL fLock,
            /* [in] */ BOOL fLastUnlockCloses) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetContainedObject( 
            /* [in] */ BOOL fContained) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IRunnableObjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IRunnableObject * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IRunnableObject * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IRunnableObject * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetRunningClass )( 
            __RPC__in IRunnableObject * This,
            /* [out] */ __RPC__out LPCLSID lpClsid);
        
        HRESULT ( STDMETHODCALLTYPE *Run )( 
            __RPC__in IRunnableObject * This,
            /* [in] */ __RPC__in_opt LPBINDCTX pbc);
        
        /* [local] */ BOOL ( STDMETHODCALLTYPE *IsRunning )( 
            IRunnableObject * This);
        
        HRESULT ( STDMETHODCALLTYPE *LockRunning )( 
            __RPC__in IRunnableObject * This,
            /* [in] */ BOOL fLock,
            /* [in] */ BOOL fLastUnlockCloses);
        
        HRESULT ( STDMETHODCALLTYPE *SetContainedObject )( 
            __RPC__in IRunnableObject * This,
            /* [in] */ BOOL fContained);
        
        END_INTERFACE
    } IRunnableObjectVtbl;

    interface IRunnableObject
    {
        CONST_VTBL struct IRunnableObjectVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRunnableObject_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IRunnableObject_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IRunnableObject_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IRunnableObject_GetRunningClass(This,lpClsid)	\
    ( (This)->lpVtbl -> GetRunningClass(This,lpClsid) ) 

#define IRunnableObject_Run(This,pbc)	\
    ( (This)->lpVtbl -> Run(This,pbc) ) 

#define IRunnableObject_IsRunning(This)	\
    ( (This)->lpVtbl -> IsRunning(This) ) 

#define IRunnableObject_LockRunning(This,fLock,fLastUnlockCloses)	\
    ( (This)->lpVtbl -> LockRunning(This,fLock,fLastUnlockCloses) ) 

#define IRunnableObject_SetContainedObject(This,fContained)	\
    ( (This)->lpVtbl -> SetContainedObject(This,fContained) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT STDMETHODCALLTYPE IRunnableObject_RemoteIsRunning_Proxy( 
    __RPC__in IRunnableObject * This);


void __RPC_STUB IRunnableObject_RemoteIsRunning_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IRunnableObject_INTERFACE_DEFINED__ */


#ifndef __IRunningObjectTable_INTERFACE_DEFINED__
#define __IRunningObjectTable_INTERFACE_DEFINED__

/* interface IRunningObjectTable */
/* [uuid][object] */ 

typedef /* [unique] */  __RPC_unique_pointer IRunningObjectTable *LPRUNNINGOBJECTTABLE;


EXTERN_C const IID IID_IRunningObjectTable;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000010-0000-0000-C000-000000000046")
    IRunningObjectTable : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Register( 
            /* [in] */ DWORD grfFlags,
            /* [unique][in] */ __RPC__in_opt IUnknown *punkObject,
            /* [unique][in] */ __RPC__in_opt IMoniker *pmkObjectName,
            /* [out] */ __RPC__out DWORD *pdwRegister) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Revoke( 
            /* [in] */ DWORD dwRegister) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsRunning( 
            /* [unique][in] */ __RPC__in_opt IMoniker *pmkObjectName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetObject( 
            /* [unique][in] */ __RPC__in_opt IMoniker *pmkObjectName,
            /* [out] */ __RPC__deref_out_opt IUnknown **ppunkObject) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE NoteChangeTime( 
            /* [in] */ DWORD dwRegister,
            /* [in] */ __RPC__in FILETIME *pfiletime) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTimeOfLastChange( 
            /* [unique][in] */ __RPC__in_opt IMoniker *pmkObjectName,
            /* [out] */ __RPC__out FILETIME *pfiletime) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumRunning( 
            /* [out] */ __RPC__deref_out_opt IEnumMoniker **ppenumMoniker) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IRunningObjectTableVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IRunningObjectTable * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IRunningObjectTable * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IRunningObjectTable * This);
        
        HRESULT ( STDMETHODCALLTYPE *Register )( 
            __RPC__in IRunningObjectTable * This,
            /* [in] */ DWORD grfFlags,
            /* [unique][in] */ __RPC__in_opt IUnknown *punkObject,
            /* [unique][in] */ __RPC__in_opt IMoniker *pmkObjectName,
            /* [out] */ __RPC__out DWORD *pdwRegister);
        
        HRESULT ( STDMETHODCALLTYPE *Revoke )( 
            __RPC__in IRunningObjectTable * This,
            /* [in] */ DWORD dwRegister);
        
        HRESULT ( STDMETHODCALLTYPE *IsRunning )( 
            __RPC__in IRunningObjectTable * This,
            /* [unique][in] */ __RPC__in_opt IMoniker *pmkObjectName);
        
        HRESULT ( STDMETHODCALLTYPE *GetObject )( 
            __RPC__in IRunningObjectTable * This,
            /* [unique][in] */ __RPC__in_opt IMoniker *pmkObjectName,
            /* [out] */ __RPC__deref_out_opt IUnknown **ppunkObject);
        
        HRESULT ( STDMETHODCALLTYPE *NoteChangeTime )( 
            __RPC__in IRunningObjectTable * This,
            /* [in] */ DWORD dwRegister,
            /* [in] */ __RPC__in FILETIME *pfiletime);
        
        HRESULT ( STDMETHODCALLTYPE *GetTimeOfLastChange )( 
            __RPC__in IRunningObjectTable * This,
            /* [unique][in] */ __RPC__in_opt IMoniker *pmkObjectName,
            /* [out] */ __RPC__out FILETIME *pfiletime);
        
        HRESULT ( STDMETHODCALLTYPE *EnumRunning )( 
            __RPC__in IRunningObjectTable * This,
            /* [out] */ __RPC__deref_out_opt IEnumMoniker **ppenumMoniker);
        
        END_INTERFACE
    } IRunningObjectTableVtbl;

    interface IRunningObjectTable
    {
        CONST_VTBL struct IRunningObjectTableVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRunningObjectTable_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IRunningObjectTable_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IRunningObjectTable_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IRunningObjectTable_Register(This,grfFlags,punkObject,pmkObjectName,pdwRegister)	\
    ( (This)->lpVtbl -> Register(This,grfFlags,punkObject,pmkObjectName,pdwRegister) ) 

#define IRunningObjectTable_Revoke(This,dwRegister)	\
    ( (This)->lpVtbl -> Revoke(This,dwRegister) ) 

#define IRunningObjectTable_IsRunning(This,pmkObjectName)	\
    ( (This)->lpVtbl -> IsRunning(This,pmkObjectName) ) 

#define IRunningObjectTable_GetObject(This,pmkObjectName,ppunkObject)	\
    ( (This)->lpVtbl -> GetObject(This,pmkObjectName,ppunkObject) ) 

#define IRunningObjectTable_NoteChangeTime(This,dwRegister,pfiletime)	\
    ( (This)->lpVtbl -> NoteChangeTime(This,dwRegister,pfiletime) ) 

#define IRunningObjectTable_GetTimeOfLastChange(This,pmkObjectName,pfiletime)	\
    ( (This)->lpVtbl -> GetTimeOfLastChange(This,pmkObjectName,pfiletime) ) 

#define IRunningObjectTable_EnumRunning(This,ppenumMoniker)	\
    ( (This)->lpVtbl -> EnumRunning(This,ppenumMoniker) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IRunningObjectTable_INTERFACE_DEFINED__ */


#ifndef __IPersist_INTERFACE_DEFINED__
#define __IPersist_INTERFACE_DEFINED__

/* interface IPersist */
/* [uuid][object] */ 

typedef /* [unique] */  __RPC_unique_pointer IPersist *LPPERSIST;


EXTERN_C const IID IID_IPersist;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0000010c-0000-0000-C000-000000000046")
    IPersist : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetClassID( 
            /* [out] */ __RPC__out CLSID *pClassID) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPersistVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IPersist * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IPersist * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IPersist * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetClassID )( 
            __RPC__in IPersist * This,
            /* [out] */ __RPC__out CLSID *pClassID);
        
        END_INTERFACE
    } IPersistVtbl;

    interface IPersist
    {
        CONST_VTBL struct IPersistVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPersist_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IPersist_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IPersist_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IPersist_GetClassID(This,pClassID)	\
    ( (This)->lpVtbl -> GetClassID(This,pClassID) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IPersist_INTERFACE_DEFINED__ */


#ifndef __IPersistStream_INTERFACE_DEFINED__
#define __IPersistStream_INTERFACE_DEFINED__

/* interface IPersistStream */
/* [unique][uuid][object] */ 

typedef /* [unique] */  __RPC_unique_pointer IPersistStream *LPPERSISTSTREAM;


EXTERN_C const IID IID_IPersistStream;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000109-0000-0000-C000-000000000046")
    IPersistStream : public IPersist
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE IsDirty( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Load( 
            /* [unique][in] */ __RPC__in_opt IStream *pStm) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Save( 
            /* [unique][in] */ __RPC__in_opt IStream *pStm,
            /* [in] */ BOOL fClearDirty) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSizeMax( 
            /* [out] */ __RPC__out ULARGE_INTEGER *pcbSize) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPersistStreamVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IPersistStream * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IPersistStream * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IPersistStream * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetClassID )( 
            __RPC__in IPersistStream * This,
            /* [out] */ __RPC__out CLSID *pClassID);
        
        HRESULT ( STDMETHODCALLTYPE *IsDirty )( 
            __RPC__in IPersistStream * This);
        
        HRESULT ( STDMETHODCALLTYPE *Load )( 
            __RPC__in IPersistStream * This,
            /* [unique][in] */ __RPC__in_opt IStream *pStm);
        
        HRESULT ( STDMETHODCALLTYPE *Save )( 
            __RPC__in IPersistStream * This,
            /* [unique][in] */ __RPC__in_opt IStream *pStm,
            /* [in] */ BOOL fClearDirty);
        
        HRESULT ( STDMETHODCALLTYPE *GetSizeMax )( 
            __RPC__in IPersistStream * This,
            /* [out] */ __RPC__out ULARGE_INTEGER *pcbSize);
        
        END_INTERFACE
    } IPersistStreamVtbl;

    interface IPersistStream
    {
        CONST_VTBL struct IPersistStreamVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPersistStream_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IPersistStream_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IPersistStream_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IPersistStream_GetClassID(This,pClassID)	\
    ( (This)->lpVtbl -> GetClassID(This,pClassID) ) 


#define IPersistStream_IsDirty(This)	\
    ( (This)->lpVtbl -> IsDirty(This) ) 

#define IPersistStream_Load(This,pStm)	\
    ( (This)->lpVtbl -> Load(This,pStm) ) 

#define IPersistStream_Save(This,pStm,fClearDirty)	\
    ( (This)->lpVtbl -> Save(This,pStm,fClearDirty) ) 

#define IPersistStream_GetSizeMax(This,pcbSize)	\
    ( (This)->lpVtbl -> GetSizeMax(This,pcbSize) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IPersistStream_INTERFACE_DEFINED__ */


#ifndef __IMoniker_INTERFACE_DEFINED__
#define __IMoniker_INTERFACE_DEFINED__

/* interface IMoniker */
/* [unique][uuid][object] */ 

typedef /* [unique] */  __RPC_unique_pointer IMoniker *LPMONIKER;

typedef 
enum tagMKSYS
    {	MKSYS_NONE	= 0,
	MKSYS_GENERICCOMPOSITE	= 1,
	MKSYS_FILEMONIKER	= 2,
	MKSYS_ANTIMONIKER	= 3,
	MKSYS_ITEMMONIKER	= 4,
	MKSYS_POINTERMONIKER	= 5,
	MKSYS_CLASSMONIKER	= 7,
	MKSYS_OBJREFMONIKER	= 8,
	MKSYS_SESSIONMONIKER	= 9,
	MKSYS_LUAMONIKER	= 10
    } 	MKSYS;

typedef /* [v1_enum] */ 
enum tagMKREDUCE
    {	MKRREDUCE_ONE	= ( 3 << 16 ) ,
	MKRREDUCE_TOUSER	= ( 2 << 16 ) ,
	MKRREDUCE_THROUGHUSER	= ( 1 << 16 ) ,
	MKRREDUCE_ALL	= 0
    } 	MKRREDUCE;


EXTERN_C const IID IID_IMoniker;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0000000f-0000-0000-C000-000000000046")
    IMoniker : public IPersistStream
    {
    public:
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE BindToObject( 
            /* [unique][in] */ IBindCtx *pbc,
            /* [unique][in] */ IMoniker *pmkToLeft,
            /* [in] */ REFIID riidResult,
            /* [iid_is][out] */ void **ppvResult) = 0;
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE BindToStorage( 
            /* [unique][in] */ IBindCtx *pbc,
            /* [unique][in] */ IMoniker *pmkToLeft,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObj) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reduce( 
            /* [unique][in] */ __RPC__in_opt IBindCtx *pbc,
            /* [in] */ DWORD dwReduceHowFar,
            /* [unique][out][in] */ __RPC__deref_opt_inout_opt IMoniker **ppmkToLeft,
            /* [out] */ __RPC__deref_out_opt IMoniker **ppmkReduced) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ComposeWith( 
            /* [unique][in] */ __RPC__in_opt IMoniker *pmkRight,
            /* [in] */ BOOL fOnlyIfNotGeneric,
            /* [out] */ __RPC__deref_out_opt IMoniker **ppmkComposite) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Enum( 
            /* [in] */ BOOL fForward,
            /* [out] */ __RPC__deref_out_opt IEnumMoniker **ppenumMoniker) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsEqual( 
            /* [unique][in] */ __RPC__in_opt IMoniker *pmkOtherMoniker) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Hash( 
            /* [out] */ __RPC__out DWORD *pdwHash) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsRunning( 
            /* [unique][in] */ __RPC__in_opt IBindCtx *pbc,
            /* [unique][in] */ __RPC__in_opt IMoniker *pmkToLeft,
            /* [unique][in] */ __RPC__in_opt IMoniker *pmkNewlyRunning) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTimeOfLastChange( 
            /* [unique][in] */ __RPC__in_opt IBindCtx *pbc,
            /* [unique][in] */ __RPC__in_opt IMoniker *pmkToLeft,
            /* [out] */ __RPC__out FILETIME *pFileTime) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Inverse( 
            /* [out] */ __RPC__deref_out_opt IMoniker **ppmk) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CommonPrefixWith( 
            /* [unique][in] */ __RPC__in_opt IMoniker *pmkOther,
            /* [out] */ __RPC__deref_out_opt IMoniker **ppmkPrefix) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RelativePathTo( 
            /* [unique][in] */ __RPC__in_opt IMoniker *pmkOther,
            /* [out] */ __RPC__deref_out_opt IMoniker **ppmkRelPath) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDisplayName( 
            /* [unique][in] */ __RPC__in_opt IBindCtx *pbc,
            /* [unique][in] */ __RPC__in_opt IMoniker *pmkToLeft,
            /* [out] */ __RPC__deref_out_opt LPOLESTR *ppszDisplayName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ParseDisplayName( 
            /* [unique][in] */ __RPC__in_opt IBindCtx *pbc,
            /* [unique][in] */ __RPC__in_opt IMoniker *pmkToLeft,
            /* [in] */ __RPC__in LPOLESTR pszDisplayName,
            /* [out] */ __RPC__out ULONG *pchEaten,
            /* [out] */ __RPC__deref_out_opt IMoniker **ppmkOut) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsSystemMoniker( 
            /* [out] */ __RPC__out DWORD *pdwMksys) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMonikerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IMoniker * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IMoniker * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IMoniker * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetClassID )( 
            __RPC__in IMoniker * This,
            /* [out] */ __RPC__out CLSID *pClassID);
        
        HRESULT ( STDMETHODCALLTYPE *IsDirty )( 
            __RPC__in IMoniker * This);
        
        HRESULT ( STDMETHODCALLTYPE *Load )( 
            __RPC__in IMoniker * This,
            /* [unique][in] */ __RPC__in_opt IStream *pStm);
        
        HRESULT ( STDMETHODCALLTYPE *Save )( 
            __RPC__in IMoniker * This,
            /* [unique][in] */ __RPC__in_opt IStream *pStm,
            /* [in] */ BOOL fClearDirty);
        
        HRESULT ( STDMETHODCALLTYPE *GetSizeMax )( 
            __RPC__in IMoniker * This,
            /* [out] */ __RPC__out ULARGE_INTEGER *pcbSize);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *BindToObject )( 
            IMoniker * This,
            /* [unique][in] */ IBindCtx *pbc,
            /* [unique][in] */ IMoniker *pmkToLeft,
            /* [in] */ REFIID riidResult,
            /* [iid_is][out] */ void **ppvResult);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *BindToStorage )( 
            IMoniker * This,
            /* [unique][in] */ IBindCtx *pbc,
            /* [unique][in] */ IMoniker *pmkToLeft,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObj);
        
        HRESULT ( STDMETHODCALLTYPE *Reduce )( 
            __RPC__in IMoniker * This,
            /* [unique][in] */ __RPC__in_opt IBindCtx *pbc,
            /* [in] */ DWORD dwReduceHowFar,
            /* [unique][out][in] */ __RPC__deref_opt_inout_opt IMoniker **ppmkToLeft,
            /* [out] */ __RPC__deref_out_opt IMoniker **ppmkReduced);
        
        HRESULT ( STDMETHODCALLTYPE *ComposeWith )( 
            __RPC__in IMoniker * This,
            /* [unique][in] */ __RPC__in_opt IMoniker *pmkRight,
            /* [in] */ BOOL fOnlyIfNotGeneric,
            /* [out] */ __RPC__deref_out_opt IMoniker **ppmkComposite);
        
        HRESULT ( STDMETHODCALLTYPE *Enum )( 
            __RPC__in IMoniker * This,
            /* [in] */ BOOL fForward,
            /* [out] */ __RPC__deref_out_opt IEnumMoniker **ppenumMoniker);
        
        HRESULT ( STDMETHODCALLTYPE *IsEqual )( 
            __RPC__in IMoniker * This,
            /* [unique][in] */ __RPC__in_opt IMoniker *pmkOtherMoniker);
        
        HRESULT ( STDMETHODCALLTYPE *Hash )( 
            __RPC__in IMoniker * This,
            /* [out] */ __RPC__out DWORD *pdwHash);
        
        HRESULT ( STDMETHODCALLTYPE *IsRunning )( 
            __RPC__in IMoniker * This,
            /* [unique][in] */ __RPC__in_opt IBindCtx *pbc,
            /* [unique][in] */ __RPC__in_opt IMoniker *pmkToLeft,
            /* [unique][in] */ __RPC__in_opt IMoniker *pmkNewlyRunning);
        
        HRESULT ( STDMETHODCALLTYPE *GetTimeOfLastChange )( 
            __RPC__in IMoniker * This,
            /* [unique][in] */ __RPC__in_opt IBindCtx *pbc,
            /* [unique][in] */ __RPC__in_opt IMoniker *pmkToLeft,
            /* [out] */ __RPC__out FILETIME *pFileTime);
        
        HRESULT ( STDMETHODCALLTYPE *Inverse )( 
            __RPC__in IMoniker * This,
            /* [out] */ __RPC__deref_out_opt IMoniker **ppmk);
        
        HRESULT ( STDMETHODCALLTYPE *CommonPrefixWith )( 
            __RPC__in IMoniker * This,
            /* [unique][in] */ __RPC__in_opt IMoniker *pmkOther,
            /* [out] */ __RPC__deref_out_opt IMoniker **ppmkPrefix);
        
        HRESULT ( STDMETHODCALLTYPE *RelativePathTo )( 
            __RPC__in IMoniker * This,
            /* [unique][in] */ __RPC__in_opt IMoniker *pmkOther,
            /* [out] */ __RPC__deref_out_opt IMoniker **ppmkRelPath);
        
        HRESULT ( STDMETHODCALLTYPE *GetDisplayName )( 
            __RPC__in IMoniker * This,
            /* [unique][in] */ __RPC__in_opt IBindCtx *pbc,
            /* [unique][in] */ __RPC__in_opt IMoniker *pmkToLeft,
            /* [out] */ __RPC__deref_out_opt LPOLESTR *ppszDisplayName);
        
        HRESULT ( STDMETHODCALLTYPE *ParseDisplayName )( 
            __RPC__in IMoniker * This,
            /* [unique][in] */ __RPC__in_opt IBindCtx *pbc,
            /* [unique][in] */ __RPC__in_opt IMoniker *pmkToLeft,
            /* [in] */ __RPC__in LPOLESTR pszDisplayName,
            /* [out] */ __RPC__out ULONG *pchEaten,
            /* [out] */ __RPC__deref_out_opt IMoniker **ppmkOut);
        
        HRESULT ( STDMETHODCALLTYPE *IsSystemMoniker )( 
            __RPC__in IMoniker * This,
            /* [out] */ __RPC__out DWORD *pdwMksys);
        
        END_INTERFACE
    } IMonikerVtbl;

    interface IMoniker
    {
        CONST_VTBL struct IMonikerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMoniker_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMoniker_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMoniker_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMoniker_GetClassID(This,pClassID)	\
    ( (This)->lpVtbl -> GetClassID(This,pClassID) ) 


#define IMoniker_IsDirty(This)	\
    ( (This)->lpVtbl -> IsDirty(This) ) 

#define IMoniker_Load(This,pStm)	\
    ( (This)->lpVtbl -> Load(This,pStm) ) 

#define IMoniker_Save(This,pStm,fClearDirty)	\
    ( (This)->lpVtbl -> Save(This,pStm,fClearDirty) ) 

#define IMoniker_GetSizeMax(This,pcbSize)	\
    ( (This)->lpVtbl -> GetSizeMax(This,pcbSize) ) 


#define IMoniker_BindToObject(This,pbc,pmkToLeft,riidResult,ppvResult)	\
    ( (This)->lpVtbl -> BindToObject(This,pbc,pmkToLeft,riidResult,ppvResult) ) 

#define IMoniker_BindToStorage(This,pbc,pmkToLeft,riid,ppvObj)	\
    ( (This)->lpVtbl -> BindToStorage(This,pbc,pmkToLeft,riid,ppvObj) ) 

#define IMoniker_Reduce(This,pbc,dwReduceHowFar,ppmkToLeft,ppmkReduced)	\
    ( (This)->lpVtbl -> Reduce(This,pbc,dwReduceHowFar,ppmkToLeft,ppmkReduced) ) 

#define IMoniker_ComposeWith(This,pmkRight,fOnlyIfNotGeneric,ppmkComposite)	\
    ( (This)->lpVtbl -> ComposeWith(This,pmkRight,fOnlyIfNotGeneric,ppmkComposite) ) 

#define IMoniker_Enum(This,fForward,ppenumMoniker)	\
    ( (This)->lpVtbl -> Enum(This,fForward,ppenumMoniker) ) 

#define IMoniker_IsEqual(This,pmkOtherMoniker)	\
    ( (This)->lpVtbl -> IsEqual(This,pmkOtherMoniker) ) 

#define IMoniker_Hash(This,pdwHash)	\
    ( (This)->lpVtbl -> Hash(This,pdwHash) ) 

#define IMoniker_IsRunning(This,pbc,pmkToLeft,pmkNewlyRunning)	\
    ( (This)->lpVtbl -> IsRunning(This,pbc,pmkToLeft,pmkNewlyRunning) ) 

#define IMoniker_GetTimeOfLastChange(This,pbc,pmkToLeft,pFileTime)	\
    ( (This)->lpVtbl -> GetTimeOfLastChange(This,pbc,pmkToLeft,pFileTime) ) 

#define IMoniker_Inverse(This,ppmk)	\
    ( (This)->lpVtbl -> Inverse(This,ppmk) ) 

#define IMoniker_CommonPrefixWith(This,pmkOther,ppmkPrefix)	\
    ( (This)->lpVtbl -> CommonPrefixWith(This,pmkOther,ppmkPrefix) ) 

#define IMoniker_RelativePathTo(This,pmkOther,ppmkRelPath)	\
    ( (This)->lpVtbl -> RelativePathTo(This,pmkOther,ppmkRelPath) ) 

#define IMoniker_GetDisplayName(This,pbc,pmkToLeft,ppszDisplayName)	\
    ( (This)->lpVtbl -> GetDisplayName(This,pbc,pmkToLeft,ppszDisplayName) ) 

#define IMoniker_ParseDisplayName(This,pbc,pmkToLeft,pszDisplayName,pchEaten,ppmkOut)	\
    ( (This)->lpVtbl -> ParseDisplayName(This,pbc,pmkToLeft,pszDisplayName,pchEaten,ppmkOut) ) 

#define IMoniker_IsSystemMoniker(This,pdwMksys)	\
    ( (This)->lpVtbl -> IsSystemMoniker(This,pdwMksys) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT STDMETHODCALLTYPE IMoniker_RemoteBindToObject_Proxy( 
    __RPC__in IMoniker * This,
    /* [unique][in] */ __RPC__in_opt IBindCtx *pbc,
    /* [unique][in] */ __RPC__in_opt IMoniker *pmkToLeft,
    /* [in] */ __RPC__in REFIID riidResult,
    /* [iid_is][out] */ __RPC__deref_out_opt IUnknown **ppvResult);


void __RPC_STUB IMoniker_RemoteBindToObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IMoniker_RemoteBindToStorage_Proxy( 
    __RPC__in IMoniker * This,
    /* [unique][in] */ __RPC__in_opt IBindCtx *pbc,
    /* [unique][in] */ __RPC__in_opt IMoniker *pmkToLeft,
    /* [in] */ __RPC__in REFIID riid,
    /* [iid_is][out] */ __RPC__deref_out_opt IUnknown **ppvObj);


void __RPC_STUB IMoniker_RemoteBindToStorage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMoniker_INTERFACE_DEFINED__ */


#ifndef __IROTData_INTERFACE_DEFINED__
#define __IROTData_INTERFACE_DEFINED__

/* interface IROTData */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IROTData;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("f29f6bc0-5021-11ce-aa15-00006901293f")
    IROTData : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetComparisonData( 
            /* [size_is][out] */ __RPC__out_ecount_full(cbMax) byte *pbData,
            /* [in] */ ULONG cbMax,
            /* [out] */ __RPC__out ULONG *pcbData) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IROTDataVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IROTData * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IROTData * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IROTData * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetComparisonData )( 
            __RPC__in IROTData * This,
            /* [size_is][out] */ __RPC__out_ecount_full(cbMax) byte *pbData,
            /* [in] */ ULONG cbMax,
            /* [out] */ __RPC__out ULONG *pcbData);
        
        END_INTERFACE
    } IROTDataVtbl;

    interface IROTData
    {
        CONST_VTBL struct IROTDataVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IROTData_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IROTData_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IROTData_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IROTData_GetComparisonData(This,pbData,cbMax,pcbData)	\
    ( (This)->lpVtbl -> GetComparisonData(This,pbData,cbMax,pcbData) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IROTData_INTERFACE_DEFINED__ */


#ifndef __IEnumString_INTERFACE_DEFINED__
#define __IEnumString_INTERFACE_DEFINED__

/* interface IEnumString */
/* [unique][uuid][object] */ 

typedef /* [unique] */  __RPC_unique_pointer IEnumString *LPENUMSTRING;


EXTERN_C const IID IID_IEnumString;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000101-0000-0000-C000-000000000046")
    IEnumString : public IUnknown
    {
    public:
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [annotation] */ 
            __RPC__out_ecount_part(celt,*pceltFetched)  LPOLESTR *rgelt,
            /* [annotation] */ 
            __out_opt  ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ __RPC__deref_out_opt IEnumString **ppenum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumStringVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IEnumString * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IEnumString * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IEnumString * This);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumString * This,
            /* [in] */ ULONG celt,
            /* [annotation] */ 
            __RPC__out_ecount_part(celt,*pceltFetched)  LPOLESTR *rgelt,
            /* [annotation] */ 
            __out_opt  ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            __RPC__in IEnumString * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            __RPC__in IEnumString * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            __RPC__in IEnumString * This,
            /* [out] */ __RPC__deref_out_opt IEnumString **ppenum);
        
        END_INTERFACE
    } IEnumStringVtbl;

    interface IEnumString
    {
        CONST_VTBL struct IEnumStringVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumString_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IEnumString_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IEnumString_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IEnumString_Next(This,celt,rgelt,pceltFetched)	\
    ( (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched) ) 

#define IEnumString_Skip(This,celt)	\
    ( (This)->lpVtbl -> Skip(This,celt) ) 

#define IEnumString_Reset(This)	\
    ( (This)->lpVtbl -> Reset(This) ) 

#define IEnumString_Clone(This,ppenum)	\
    ( (This)->lpVtbl -> Clone(This,ppenum) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT STDMETHODCALLTYPE IEnumString_RemoteNext_Proxy( 
    __RPC__in IEnumString * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ __RPC__out_ecount_part(celt, *pceltFetched) LPOLESTR *rgelt,
    /* [out] */ __RPC__out ULONG *pceltFetched);


void __RPC_STUB IEnumString_RemoteNext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumString_INTERFACE_DEFINED__ */


#ifndef __ISequentialStream_INTERFACE_DEFINED__
#define __ISequentialStream_INTERFACE_DEFINED__

/* interface ISequentialStream */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_ISequentialStream;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0c733a30-2a1c-11ce-ade5-00aa0044773d")
    ISequentialStream : public IUnknown
    {
    public:
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE Read( 
            /* [annotation] */ 
            __out_bcount_part(cb, *pcbRead)  void *pv,
            /* [in] */ ULONG cb,
            /* [annotation] */ 
            __out_opt  ULONG *pcbRead) = 0;
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE Write( 
            /* [annotation] */ 
            __in_bcount(cb)  const void *pv,
            /* [in] */ ULONG cb,
            /* [annotation] */ 
            __out_opt  ULONG *pcbWritten) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISequentialStreamVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in ISequentialStream * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in ISequentialStream * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in ISequentialStream * This);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Read )( 
            ISequentialStream * This,
            /* [annotation] */ 
            __out_bcount_part(cb, *pcbRead)  void *pv,
            /* [in] */ ULONG cb,
            /* [annotation] */ 
            __out_opt  ULONG *pcbRead);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Write )( 
            ISequentialStream * This,
            /* [annotation] */ 
            __in_bcount(cb)  const void *pv,
            /* [in] */ ULONG cb,
            /* [annotation] */ 
            __out_opt  ULONG *pcbWritten);
        
        END_INTERFACE
    } ISequentialStreamVtbl;

    interface ISequentialStream
    {
        CONST_VTBL struct ISequentialStreamVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISequentialStream_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISequentialStream_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISequentialStream_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISequentialStream_Read(This,pv,cb,pcbRead)	\
    ( (This)->lpVtbl -> Read(This,pv,cb,pcbRead) ) 

#define ISequentialStream_Write(This,pv,cb,pcbWritten)	\
    ( (This)->lpVtbl -> Write(This,pv,cb,pcbWritten) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT STDMETHODCALLTYPE ISequentialStream_RemoteRead_Proxy( 
    __RPC__in ISequentialStream * This,
    /* [length_is][size_is][out] */ __RPC__out_ecount_part(cb, *pcbRead) byte *pv,
    /* [in] */ ULONG cb,
    /* [out] */ __RPC__out ULONG *pcbRead);


void __RPC_STUB ISequentialStream_RemoteRead_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT STDMETHODCALLTYPE ISequentialStream_RemoteWrite_Proxy( 
    __RPC__in ISequentialStream * This,
    /* [size_is][in] */ __RPC__in_ecount_full(cb) const byte *pv,
    /* [in] */ ULONG cb,
    /* [out] */ __RPC__out ULONG *pcbWritten);


void __RPC_STUB ISequentialStream_RemoteWrite_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISequentialStream_INTERFACE_DEFINED__ */


#ifndef __IStream_INTERFACE_DEFINED__
#define __IStream_INTERFACE_DEFINED__

/* interface IStream */
/* [unique][uuid][object] */ 

typedef /* [unique] */  __RPC_unique_pointer IStream *LPSTREAM;

//FSSpec is Macintosh only, defined in macos\files.h
#ifdef _MAC
    typedef struct tagSTATSTG
    {                      
        LPOLESTR pwcsName;
            FSSpec *pspec;
        DWORD type;
        ULARGE_INTEGER cbSize;
        FILETIME mtime;
        FILETIME ctime;
        FILETIME atime;
        DWORD grfMode;
        DWORD grfLocksSupported;
        CLSID clsid;
        DWORD grfStateBits;
        DWORD reserved;
    } STATSTG;
#else //_MAC
typedef struct tagSTATSTG
    {
    LPOLESTR pwcsName;
    DWORD type;
    ULARGE_INTEGER cbSize;
    FILETIME mtime;
    FILETIME ctime;
    FILETIME atime;
    DWORD grfMode;
    DWORD grfLocksSupported;
    CLSID clsid;
    DWORD grfStateBits;
    DWORD reserved;
    } 	STATSTG;

#endif //_MAC
typedef 
enum tagSTGTY
    {	STGTY_STORAGE	= 1,
	STGTY_STREAM	= 2,
	STGTY_LOCKBYTES	= 3,
	STGTY_PROPERTY	= 4
    } 	STGTY;

typedef 
enum tagSTREAM_SEEK
    {	STREAM_SEEK_SET	= 0,
	STREAM_SEEK_CUR	= 1,
	STREAM_SEEK_END	= 2
    } 	STREAM_SEEK;

typedef 
enum tagLOCKTYPE
    {	LOCK_WRITE	= 1,
	LOCK_EXCLUSIVE	= 2,
	LOCK_ONLYONCE	= 4
    } 	LOCKTYPE;


EXTERN_C const IID IID_IStream;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0000000c-0000-0000-C000-000000000046")
    IStream : public ISequentialStream
    {
    public:
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE Seek( 
            /* [in] */ LARGE_INTEGER dlibMove,
            /* [in] */ DWORD dwOrigin,
            /* [annotation] */ 
            __out_opt  ULARGE_INTEGER *plibNewPosition) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetSize( 
            /* [in] */ ULARGE_INTEGER libNewSize) = 0;
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE CopyTo( 
            /* [unique][in] */ IStream *pstm,
            /* [in] */ ULARGE_INTEGER cb,
            /* [annotation] */ 
            __out_opt  ULARGE_INTEGER *pcbRead,
            /* [annotation] */ 
            __out_opt  ULARGE_INTEGER *pcbWritten) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Commit( 
            /* [in] */ DWORD grfCommitFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Revert( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE LockRegion( 
            /* [in] */ ULARGE_INTEGER libOffset,
            /* [in] */ ULARGE_INTEGER cb,
            /* [in] */ DWORD dwLockType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UnlockRegion( 
            /* [in] */ ULARGE_INTEGER libOffset,
            /* [in] */ ULARGE_INTEGER cb,
            /* [in] */ DWORD dwLockType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Stat( 
            /* [out] */ __RPC__out STATSTG *pstatstg,
            /* [in] */ DWORD grfStatFlag) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ __RPC__deref_out_opt IStream **ppstm) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IStreamVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IStream * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IStream * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IStream * This);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Read )( 
            IStream * This,
            /* [annotation] */ 
            __out_bcount_part(cb, *pcbRead)  void *pv,
            /* [in] */ ULONG cb,
            /* [annotation] */ 
            __out_opt  ULONG *pcbRead);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Write )( 
            IStream * This,
            /* [annotation] */ 
            __in_bcount(cb)  const void *pv,
            /* [in] */ ULONG cb,
            /* [annotation] */ 
            __out_opt  ULONG *pcbWritten);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Seek )( 
            IStream * This,
            /* [in] */ LARGE_INTEGER dlibMove,
            /* [in] */ DWORD dwOrigin,
            /* [annotation] */ 
            __out_opt  ULARGE_INTEGER *plibNewPosition);
        
        HRESULT ( STDMETHODCALLTYPE *SetSize )( 
            __RPC__in IStream * This,
            /* [in] */ ULARGE_INTEGER libNewSize);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *CopyTo )( 
            IStream * This,
            /* [unique][in] */ IStream *pstm,
            /* [in] */ ULARGE_INTEGER cb,
            /* [annotation] */ 
            __out_opt  ULARGE_INTEGER *pcbRead,
            /* [annotation] */ 
            __out_opt  ULARGE_INTEGER *pcbWritten);
        
        HRESULT ( STDMETHODCALLTYPE *Commit )( 
            __RPC__in IStream * This,
            /* [in] */ DWORD grfCommitFlags);
        
        HRESULT ( STDMETHODCALLTYPE *Revert )( 
            __RPC__in IStream * This);
        
        HRESULT ( STDMETHODCALLTYPE *LockRegion )( 
            __RPC__in IStream * This,
            /* [in] */ ULARGE_INTEGER libOffset,
            /* [in] */ ULARGE_INTEGER cb,
            /* [in] */ DWORD dwLockType);
        
        HRESULT ( STDMETHODCALLTYPE *UnlockRegion )( 
            __RPC__in IStream * This,
            /* [in] */ ULARGE_INTEGER libOffset,
            /* [in] */ ULARGE_INTEGER cb,
            /* [in] */ DWORD dwLockType);
        
        HRESULT ( STDMETHODCALLTYPE *Stat )( 
            __RPC__in IStream * This,
            /* [out] */ __RPC__out STATSTG *pstatstg,
            /* [in] */ DWORD grfStatFlag);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            __RPC__in IStream * This,
            /* [out] */ __RPC__deref_out_opt IStream **ppstm);
        
        END_INTERFACE
    } IStreamVtbl;

    interface IStream
    {
        CONST_VTBL struct IStreamVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IStream_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IStream_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IStream_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IStream_Read(This,pv,cb,pcbRead)	\
    ( (This)->lpVtbl -> Read(This,pv,cb,pcbRead) ) 

#define IStream_Write(This,pv,cb,pcbWritten)	\
    ( (This)->lpVtbl -> Write(This,pv,cb,pcbWritten) ) 


#define IStream_Seek(This,dlibMove,dwOrigin,plibNewPosition)	\
    ( (This)->lpVtbl -> Seek(This,dlibMove,dwOrigin,plibNewPosition) ) 

#define IStream_SetSize(This,libNewSize)	\
    ( (This)->lpVtbl -> SetSize(This,libNewSize) ) 

#define IStream_CopyTo(This,pstm,cb,pcbRead,pcbWritten)	\
    ( (This)->lpVtbl -> CopyTo(This,pstm,cb,pcbRead,pcbWritten) ) 

#define IStream_Commit(This,grfCommitFlags)	\
    ( (This)->lpVtbl -> Commit(This,grfCommitFlags) ) 

#define IStream_Revert(This)	\
    ( (This)->lpVtbl -> Revert(This) ) 

#define IStream_LockRegion(This,libOffset,cb,dwLockType)	\
    ( (This)->lpVtbl -> LockRegion(This,libOffset,cb,dwLockType) ) 

#define IStream_UnlockRegion(This,libOffset,cb,dwLockType)	\
    ( (This)->lpVtbl -> UnlockRegion(This,libOffset,cb,dwLockType) ) 

#define IStream_Stat(This,pstatstg,grfStatFlag)	\
    ( (This)->lpVtbl -> Stat(This,pstatstg,grfStatFlag) ) 

#define IStream_Clone(This,ppstm)	\
    ( (This)->lpVtbl -> Clone(This,ppstm) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT STDMETHODCALLTYPE IStream_RemoteSeek_Proxy( 
    __RPC__in IStream * This,
    /* [in] */ LARGE_INTEGER dlibMove,
    /* [in] */ DWORD dwOrigin,
    /* [out] */ __RPC__out ULARGE_INTEGER *plibNewPosition);


void __RPC_STUB IStream_RemoteSeek_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IStream_RemoteCopyTo_Proxy( 
    __RPC__in IStream * This,
    /* [unique][in] */ __RPC__in_opt IStream *pstm,
    /* [in] */ ULARGE_INTEGER cb,
    /* [out] */ __RPC__out ULARGE_INTEGER *pcbRead,
    /* [out] */ __RPC__out ULARGE_INTEGER *pcbWritten);


void __RPC_STUB IStream_RemoteCopyTo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IStream_INTERFACE_DEFINED__ */


#ifndef __IEnumSTATSTG_INTERFACE_DEFINED__
#define __IEnumSTATSTG_INTERFACE_DEFINED__

/* interface IEnumSTATSTG */
/* [unique][uuid][object] */ 

typedef /* [unique] */  __RPC_unique_pointer IEnumSTATSTG *LPENUMSTATSTG;


EXTERN_C const IID IID_IEnumSTATSTG;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0000000d-0000-0000-C000-000000000046")
    IEnumSTATSTG : public IUnknown
    {
    public:
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [annotation] */ 
            __out_ecount_part(celt,*pceltFetched)  STATSTG *rgelt,
            /* [annotation] */ 
            __out_opt  ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ __RPC__deref_out_opt IEnumSTATSTG **ppenum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumSTATSTGVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IEnumSTATSTG * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IEnumSTATSTG * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IEnumSTATSTG * This);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumSTATSTG * This,
            /* [in] */ ULONG celt,
            /* [annotation] */ 
            __out_ecount_part(celt,*pceltFetched)  STATSTG *rgelt,
            /* [annotation] */ 
            __out_opt  ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            __RPC__in IEnumSTATSTG * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            __RPC__in IEnumSTATSTG * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            __RPC__in IEnumSTATSTG * This,
            /* [out] */ __RPC__deref_out_opt IEnumSTATSTG **ppenum);
        
        END_INTERFACE
    } IEnumSTATSTGVtbl;

    interface IEnumSTATSTG
    {
        CONST_VTBL struct IEnumSTATSTGVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumSTATSTG_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IEnumSTATSTG_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IEnumSTATSTG_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IEnumSTATSTG_Next(This,celt,rgelt,pceltFetched)	\
    ( (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched) ) 

#define IEnumSTATSTG_Skip(This,celt)	\
    ( (This)->lpVtbl -> Skip(This,celt) ) 

#define IEnumSTATSTG_Reset(This)	\
    ( (This)->lpVtbl -> Reset(This) ) 

#define IEnumSTATSTG_Clone(This,ppenum)	\
    ( (This)->lpVtbl -> Clone(This,ppenum) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT STDMETHODCALLTYPE IEnumSTATSTG_RemoteNext_Proxy( 
    __RPC__in IEnumSTATSTG * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ __RPC__out_ecount_part(celt, *pceltFetched) STATSTG *rgelt,
    /* [out] */ __RPC__out ULONG *pceltFetched);


void __RPC_STUB IEnumSTATSTG_RemoteNext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumSTATSTG_INTERFACE_DEFINED__ */


#ifndef __IStorage_INTERFACE_DEFINED__
#define __IStorage_INTERFACE_DEFINED__

/* interface IStorage */
/* [unique][uuid][object] */ 

typedef /* [unique] */  __RPC_unique_pointer IStorage *LPSTORAGE;

typedef struct tagRemSNB
    {
    unsigned long ulCntStr;
    unsigned long ulCntChar;
    OLECHAR rgString[ 1 ];
    } 	RemSNB;

typedef /* [unique] */  __RPC_unique_pointer RemSNB *wireSNB;

typedef /* [wire_marshal] */ LPOLESTR *SNB;


EXTERN_C const IID IID_IStorage;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0000000b-0000-0000-C000-000000000046")
    IStorage : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CreateStream( 
            /* [string][in] */ __RPC__in_string const OLECHAR *pwcsName,
            /* [in] */ DWORD grfMode,
            /* [in] */ DWORD reserved1,
            /* [in] */ DWORD reserved2,
            /* [out] */ __RPC__deref_out_opt IStream **ppstm) = 0;
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE OpenStream( 
            /* [string][in] */ const OLECHAR *pwcsName,
            /* [unique][in] */ void *reserved1,
            /* [in] */ DWORD grfMode,
            /* [in] */ DWORD reserved2,
            /* [out] */ IStream **ppstm) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateStorage( 
            /* [string][in] */ __RPC__in_string const OLECHAR *pwcsName,
            /* [in] */ DWORD grfMode,
            /* [in] */ DWORD reserved1,
            /* [in] */ DWORD reserved2,
            /* [out] */ __RPC__deref_out_opt IStorage **ppstg) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OpenStorage( 
            /* [string][unique][in] */ __RPC__in_opt_string const OLECHAR *pwcsName,
            /* [unique][in] */ __RPC__in_opt IStorage *pstgPriority,
            /* [in] */ DWORD grfMode,
            /* [unique][in] */ __RPC__deref_opt_in_opt SNB snbExclude,
            /* [in] */ DWORD reserved,
            /* [out] */ __RPC__deref_out_opt IStorage **ppstg) = 0;
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE CopyTo( 
            /* [in] */ DWORD ciidExclude,
            /* [size_is][unique][in] */ const IID *rgiidExclude,
            /* [annotation][unique][in] */ 
            __RPC__in_opt  SNB snbExclude,
            /* [unique][in] */ IStorage *pstgDest) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MoveElementTo( 
            /* [string][in] */ __RPC__in_string const OLECHAR *pwcsName,
            /* [unique][in] */ __RPC__in_opt IStorage *pstgDest,
            /* [string][in] */ __RPC__in_string const OLECHAR *pwcsNewName,
            /* [in] */ DWORD grfFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Commit( 
            /* [in] */ DWORD grfCommitFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Revert( void) = 0;
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE EnumElements( 
            /* [in] */ DWORD reserved1,
            /* [size_is][unique][in] */ void *reserved2,
            /* [in] */ DWORD reserved3,
            /* [out] */ IEnumSTATSTG **ppenum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DestroyElement( 
            /* [string][in] */ __RPC__in_string const OLECHAR *pwcsName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RenameElement( 
            /* [string][in] */ __RPC__in_string const OLECHAR *pwcsOldName,
            /* [string][in] */ __RPC__in_string const OLECHAR *pwcsNewName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetElementTimes( 
            /* [string][unique][in] */ __RPC__in_opt_string const OLECHAR *pwcsName,
            /* [unique][in] */ __RPC__in_opt const FILETIME *pctime,
            /* [unique][in] */ __RPC__in_opt const FILETIME *patime,
            /* [unique][in] */ __RPC__in_opt const FILETIME *pmtime) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetClass( 
            /* [in] */ __RPC__in REFCLSID clsid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetStateBits( 
            /* [in] */ DWORD grfStateBits,
            /* [in] */ DWORD grfMask) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Stat( 
            /* [out] */ __RPC__out STATSTG *pstatstg,
            /* [in] */ DWORD grfStatFlag) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IStorageVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IStorage * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IStorage * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IStorage * This);
        
        HRESULT ( STDMETHODCALLTYPE *CreateStream )( 
            __RPC__in IStorage * This,
            /* [string][in] */ __RPC__in_string const OLECHAR *pwcsName,
            /* [in] */ DWORD grfMode,
            /* [in] */ DWORD reserved1,
            /* [in] */ DWORD reserved2,
            /* [out] */ __RPC__deref_out_opt IStream **ppstm);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *OpenStream )( 
            IStorage * This,
            /* [string][in] */ const OLECHAR *pwcsName,
            /* [unique][in] */ void *reserved1,
            /* [in] */ DWORD grfMode,
            /* [in] */ DWORD reserved2,
            /* [out] */ IStream **ppstm);
        
        HRESULT ( STDMETHODCALLTYPE *CreateStorage )( 
            __RPC__in IStorage * This,
            /* [string][in] */ __RPC__in_string const OLECHAR *pwcsName,
            /* [in] */ DWORD grfMode,
            /* [in] */ DWORD reserved1,
            /* [in] */ DWORD reserved2,
            /* [out] */ __RPC__deref_out_opt IStorage **ppstg);
        
        HRESULT ( STDMETHODCALLTYPE *OpenStorage )( 
            __RPC__in IStorage * This,
            /* [string][unique][in] */ __RPC__in_opt_string const OLECHAR *pwcsName,
            /* [unique][in] */ __RPC__in_opt IStorage *pstgPriority,
            /* [in] */ DWORD grfMode,
            /* [unique][in] */ __RPC__deref_opt_in_opt SNB snbExclude,
            /* [in] */ DWORD reserved,
            /* [out] */ __RPC__deref_out_opt IStorage **ppstg);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *CopyTo )( 
            IStorage * This,
            /* [in] */ DWORD ciidExclude,
            /* [size_is][unique][in] */ const IID *rgiidExclude,
            /* [annotation][unique][in] */ 
            __RPC__in_opt  SNB snbExclude,
            /* [unique][in] */ IStorage *pstgDest);
        
        HRESULT ( STDMETHODCALLTYPE *MoveElementTo )( 
            __RPC__in IStorage * This,
            /* [string][in] */ __RPC__in_string const OLECHAR *pwcsName,
            /* [unique][in] */ __RPC__in_opt IStorage *pstgDest,
            /* [string][in] */ __RPC__in_string const OLECHAR *pwcsNewName,
            /* [in] */ DWORD grfFlags);
        
        HRESULT ( STDMETHODCALLTYPE *Commit )( 
            __RPC__in IStorage * This,
            /* [in] */ DWORD grfCommitFlags);
        
        HRESULT ( STDMETHODCALLTYPE *Revert )( 
            __RPC__in IStorage * This);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *EnumElements )( 
            IStorage * This,
            /* [in] */ DWORD reserved1,
            /* [size_is][unique][in] */ void *reserved2,
            /* [in] */ DWORD reserved3,
            /* [out] */ IEnumSTATSTG **ppenum);
        
        HRESULT ( STDMETHODCALLTYPE *DestroyElement )( 
            __RPC__in IStorage * This,
            /* [string][in] */ __RPC__in_string const OLECHAR *pwcsName);
        
        HRESULT ( STDMETHODCALLTYPE *RenameElement )( 
            __RPC__in IStorage * This,
            /* [string][in] */ __RPC__in_string const OLECHAR *pwcsOldName,
            /* [string][in] */ __RPC__in_string const OLECHAR *pwcsNewName);
        
        HRESULT ( STDMETHODCALLTYPE *SetElementTimes )( 
            __RPC__in IStorage * This,
            /* [string][unique][in] */ __RPC__in_opt_string const OLECHAR *pwcsName,
            /* [unique][in] */ __RPC__in_opt const FILETIME *pctime,
            /* [unique][in] */ __RPC__in_opt const FILETIME *patime,
            /* [unique][in] */ __RPC__in_opt const FILETIME *pmtime);
        
        HRESULT ( STDMETHODCALLTYPE *SetClass )( 
            __RPC__in IStorage * This,
            /* [in] */ __RPC__in REFCLSID clsid);
        
        HRESULT ( STDMETHODCALLTYPE *SetStateBits )( 
            __RPC__in IStorage * This,
            /* [in] */ DWORD grfStateBits,
            /* [in] */ DWORD grfMask);
        
        HRESULT ( STDMETHODCALLTYPE *Stat )( 
            __RPC__in IStorage * This,
            /* [out] */ __RPC__out STATSTG *pstatstg,
            /* [in] */ DWORD grfStatFlag);
        
        END_INTERFACE
    } IStorageVtbl;

    interface IStorage
    {
        CONST_VTBL struct IStorageVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IStorage_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IStorage_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IStorage_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IStorage_CreateStream(This,pwcsName,grfMode,reserved1,reserved2,ppstm)	\
    ( (This)->lpVtbl -> CreateStream(This,pwcsName,grfMode,reserved1,reserved2,ppstm) ) 

#define IStorage_OpenStream(This,pwcsName,reserved1,grfMode,reserved2,ppstm)	\
    ( (This)->lpVtbl -> OpenStream(This,pwcsName,reserved1,grfMode,reserved2,ppstm) ) 

#define IStorage_CreateStorage(This,pwcsName,grfMode,reserved1,reserved2,ppstg)	\
    ( (This)->lpVtbl -> CreateStorage(This,pwcsName,grfMode,reserved1,reserved2,ppstg) ) 

#define IStorage_OpenStorage(This,pwcsName,pstgPriority,grfMode,snbExclude,reserved,ppstg)	\
    ( (This)->lpVtbl -> OpenStorage(This,pwcsName,pstgPriority,grfMode,snbExclude,reserved,ppstg) ) 

#define IStorage_CopyTo(This,ciidExclude,rgiidExclude,snbExclude,pstgDest)	\
    ( (This)->lpVtbl -> CopyTo(This,ciidExclude,rgiidExclude,snbExclude,pstgDest) ) 

#define IStorage_MoveElementTo(This,pwcsName,pstgDest,pwcsNewName,grfFlags)	\
    ( (This)->lpVtbl -> MoveElementTo(This,pwcsName,pstgDest,pwcsNewName,grfFlags) ) 

#define IStorage_Commit(This,grfCommitFlags)	\
    ( (This)->lpVtbl -> Commit(This,grfCommitFlags) ) 

#define IStorage_Revert(This)	\
    ( (This)->lpVtbl -> Revert(This) ) 

#define IStorage_EnumElements(This,reserved1,reserved2,reserved3,ppenum)	\
    ( (This)->lpVtbl -> EnumElements(This,reserved1,reserved2,reserved3,ppenum) ) 

#define IStorage_DestroyElement(This,pwcsName)	\
    ( (This)->lpVtbl -> DestroyElement(This,pwcsName) ) 

#define IStorage_RenameElement(This,pwcsOldName,pwcsNewName)	\
    ( (This)->lpVtbl -> RenameElement(This,pwcsOldName,pwcsNewName) ) 

#define IStorage_SetElementTimes(This,pwcsName,pctime,patime,pmtime)	\
    ( (This)->lpVtbl -> SetElementTimes(This,pwcsName,pctime,patime,pmtime) ) 

#define IStorage_SetClass(This,clsid)	\
    ( (This)->lpVtbl -> SetClass(This,clsid) ) 

#define IStorage_SetStateBits(This,grfStateBits,grfMask)	\
    ( (This)->lpVtbl -> SetStateBits(This,grfStateBits,grfMask) ) 

#define IStorage_Stat(This,pstatstg,grfStatFlag)	\
    ( (This)->lpVtbl -> Stat(This,pstatstg,grfStatFlag) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT STDMETHODCALLTYPE IStorage_RemoteOpenStream_Proxy( 
    __RPC__in IStorage * This,
    /* [string][in] */ __RPC__in_string const OLECHAR *pwcsName,
    /* [in] */ unsigned long cbReserved1,
    /* [size_is][unique][in] */ __RPC__in_ecount_full_opt(cbReserved1) byte *reserved1,
    /* [in] */ DWORD grfMode,
    /* [in] */ DWORD reserved2,
    /* [out] */ __RPC__deref_out_opt IStream **ppstm);


void __RPC_STUB IStorage_RemoteOpenStream_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IStorage_RemoteCopyTo_Proxy( 
    __RPC__in IStorage * This,
    /* [in] */ DWORD ciidExclude,
    /* [size_is][unique][in] */ __RPC__in_ecount_full_opt(ciidExclude) const IID *rgiidExclude,
    /* [unique][in] */ __RPC__deref_opt_in_opt SNB snbExclude,
    /* [unique][in] */ __RPC__in_opt IStorage *pstgDest);


void __RPC_STUB IStorage_RemoteCopyTo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IStorage_RemoteEnumElements_Proxy( 
    __RPC__in IStorage * This,
    /* [in] */ DWORD reserved1,
    /* [in] */ unsigned long cbReserved2,
    /* [size_is][unique][in] */ __RPC__in_ecount_full_opt(cbReserved2) byte *reserved2,
    /* [in] */ DWORD reserved3,
    /* [out] */ __RPC__deref_out_opt IEnumSTATSTG **ppenum);


void __RPC_STUB IStorage_RemoteEnumElements_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IStorage_INTERFACE_DEFINED__ */


#ifndef __IPersistFile_INTERFACE_DEFINED__
#define __IPersistFile_INTERFACE_DEFINED__

/* interface IPersistFile */
/* [unique][uuid][object] */ 

typedef /* [unique] */  __RPC_unique_pointer IPersistFile *LPPERSISTFILE;


EXTERN_C const IID IID_IPersistFile;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0000010b-0000-0000-C000-000000000046")
    IPersistFile : public IPersist
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE IsDirty( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Load( 
            /* [in] */ __RPC__in LPCOLESTR pszFileName,
            /* [in] */ DWORD dwMode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Save( 
            /* [unique][in] */ __RPC__in_opt LPCOLESTR pszFileName,
            /* [in] */ BOOL fRemember) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SaveCompleted( 
            /* [unique][in] */ __RPC__in_opt LPCOLESTR pszFileName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCurFile( 
            /* [out] */ __RPC__deref_out_opt LPOLESTR *ppszFileName) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPersistFileVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IPersistFile * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IPersistFile * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IPersistFile * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetClassID )( 
            __RPC__in IPersistFile * This,
            /* [out] */ __RPC__out CLSID *pClassID);
        
        HRESULT ( STDMETHODCALLTYPE *IsDirty )( 
            __RPC__in IPersistFile * This);
        
        HRESULT ( STDMETHODCALLTYPE *Load )( 
            __RPC__in IPersistFile * This,
            /* [in] */ __RPC__in LPCOLESTR pszFileName,
            /* [in] */ DWORD dwMode);
        
        HRESULT ( STDMETHODCALLTYPE *Save )( 
            __RPC__in IPersistFile * This,
            /* [unique][in] */ __RPC__in_opt LPCOLESTR pszFileName,
            /* [in] */ BOOL fRemember);
        
        HRESULT ( STDMETHODCALLTYPE *SaveCompleted )( 
            __RPC__in IPersistFile * This,
            /* [unique][in] */ __RPC__in_opt LPCOLESTR pszFileName);
        
        HRESULT ( STDMETHODCALLTYPE *GetCurFile )( 
            __RPC__in IPersistFile * This,
            /* [out] */ __RPC__deref_out_opt LPOLESTR *ppszFileName);
        
        END_INTERFACE
    } IPersistFileVtbl;

    interface IPersistFile
    {
        CONST_VTBL struct IPersistFileVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPersistFile_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IPersistFile_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IPersistFile_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IPersistFile_GetClassID(This,pClassID)	\
    ( (This)->lpVtbl -> GetClassID(This,pClassID) ) 


#define IPersistFile_IsDirty(This)	\
    ( (This)->lpVtbl -> IsDirty(This) ) 

#define IPersistFile_Load(This,pszFileName,dwMode)	\
    ( (This)->lpVtbl -> Load(This,pszFileName,dwMode) ) 

#define IPersistFile_Save(This,pszFileName,fRemember)	\
    ( (This)->lpVtbl -> Save(This,pszFileName,fRemember) ) 

#define IPersistFile_SaveCompleted(This,pszFileName)	\
    ( (This)->lpVtbl -> SaveCompleted(This,pszFileName) ) 

#define IPersistFile_GetCurFile(This,ppszFileName)	\
    ( (This)->lpVtbl -> GetCurFile(This,ppszFileName) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IPersistFile_INTERFACE_DEFINED__ */


#ifndef __IPersistStorage_INTERFACE_DEFINED__
#define __IPersistStorage_INTERFACE_DEFINED__

/* interface IPersistStorage */
/* [unique][uuid][object] */ 

typedef /* [unique] */  __RPC_unique_pointer IPersistStorage *LPPERSISTSTORAGE;


EXTERN_C const IID IID_IPersistStorage;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0000010a-0000-0000-C000-000000000046")
    IPersistStorage : public IPersist
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE IsDirty( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE InitNew( 
            /* [unique][in] */ __RPC__in_opt IStorage *pStg) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Load( 
            /* [unique][in] */ __RPC__in_opt IStorage *pStg) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Save( 
            /* [unique][in] */ __RPC__in_opt IStorage *pStgSave,
            /* [in] */ BOOL fSameAsLoad) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SaveCompleted( 
            /* [unique][in] */ __RPC__in_opt IStorage *pStgNew) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE HandsOffStorage( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPersistStorageVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IPersistStorage * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IPersistStorage * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IPersistStorage * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetClassID )( 
            __RPC__in IPersistStorage * This,
            /* [out] */ __RPC__out CLSID *pClassID);
        
        HRESULT ( STDMETHODCALLTYPE *IsDirty )( 
            __RPC__in IPersistStorage * This);
        
        HRESULT ( STDMETHODCALLTYPE *InitNew )( 
            __RPC__in IPersistStorage * This,
            /* [unique][in] */ __RPC__in_opt IStorage *pStg);
        
        HRESULT ( STDMETHODCALLTYPE *Load )( 
            __RPC__in IPersistStorage * This,
            /* [unique][in] */ __RPC__in_opt IStorage *pStg);
        
        HRESULT ( STDMETHODCALLTYPE *Save )( 
            __RPC__in IPersistStorage * This,
            /* [unique][in] */ __RPC__in_opt IStorage *pStgSave,
            /* [in] */ BOOL fSameAsLoad);
        
        HRESULT ( STDMETHODCALLTYPE *SaveCompleted )( 
            __RPC__in IPersistStorage * This,
            /* [unique][in] */ __RPC__in_opt IStorage *pStgNew);
        
        HRESULT ( STDMETHODCALLTYPE *HandsOffStorage )( 
            __RPC__in IPersistStorage * This);
        
        END_INTERFACE
    } IPersistStorageVtbl;

    interface IPersistStorage
    {
        CONST_VTBL struct IPersistStorageVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPersistStorage_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IPersistStorage_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IPersistStorage_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IPersistStorage_GetClassID(This,pClassID)	\
    ( (This)->lpVtbl -> GetClassID(This,pClassID) ) 


#define IPersistStorage_IsDirty(This)	\
    ( (This)->lpVtbl -> IsDirty(This) ) 

#define IPersistStorage_InitNew(This,pStg)	\
    ( (This)->lpVtbl -> InitNew(This,pStg) ) 

#define IPersistStorage_Load(This,pStg)	\
    ( (This)->lpVtbl -> Load(This,pStg) ) 

#define IPersistStorage_Save(This,pStgSave,fSameAsLoad)	\
    ( (This)->lpVtbl -> Save(This,pStgSave,fSameAsLoad) ) 

#define IPersistStorage_SaveCompleted(This,pStgNew)	\
    ( (This)->lpVtbl -> SaveCompleted(This,pStgNew) ) 

#define IPersistStorage_HandsOffStorage(This)	\
    ( (This)->lpVtbl -> HandsOffStorage(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IPersistStorage_INTERFACE_DEFINED__ */


#ifndef __ILockBytes_INTERFACE_DEFINED__
#define __ILockBytes_INTERFACE_DEFINED__

/* interface ILockBytes */
/* [unique][uuid][object] */ 

typedef /* [unique] */  __RPC_unique_pointer ILockBytes *LPLOCKBYTES;


EXTERN_C const IID IID_ILockBytes;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0000000a-0000-0000-C000-000000000046")
    ILockBytes : public IUnknown
    {
    public:
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE ReadAt( 
            /* [in] */ ULARGE_INTEGER ulOffset,
            /* [annotation][length_is][size_is][out] */ 
            __out_bcount_part(cb, *pcbRead)  void *pv,
            /* [in] */ ULONG cb,
            /* [annotation][out] */ 
            __out_opt  ULONG *pcbRead) = 0;
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE WriteAt( 
            /* [in] */ ULARGE_INTEGER ulOffset,
            /* [annotation][size_is][in] */ 
            __in_bcount(cb)  const void *pv,
            /* [in] */ ULONG cb,
            /* [annotation][out] */ 
            __out_opt  ULONG *pcbWritten) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Flush( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetSize( 
            /* [in] */ ULARGE_INTEGER cb) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE LockRegion( 
            /* [in] */ ULARGE_INTEGER libOffset,
            /* [in] */ ULARGE_INTEGER cb,
            /* [in] */ DWORD dwLockType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UnlockRegion( 
            /* [in] */ ULARGE_INTEGER libOffset,
            /* [in] */ ULARGE_INTEGER cb,
            /* [in] */ DWORD dwLockType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Stat( 
            /* [out] */ __RPC__out STATSTG *pstatstg,
            /* [in] */ DWORD grfStatFlag) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ILockBytesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in ILockBytes * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in ILockBytes * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in ILockBytes * This);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *ReadAt )( 
            ILockBytes * This,
            /* [in] */ ULARGE_INTEGER ulOffset,
            /* [annotation][length_is][size_is][out] */ 
            __out_bcount_part(cb, *pcbRead)  void *pv,
            /* [in] */ ULONG cb,
            /* [annotation][out] */ 
            __out_opt  ULONG *pcbRead);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *WriteAt )( 
            ILockBytes * This,
            /* [in] */ ULARGE_INTEGER ulOffset,
            /* [annotation][size_is][in] */ 
            __in_bcount(cb)  const void *pv,
            /* [in] */ ULONG cb,
            /* [annotation][out] */ 
            __out_opt  ULONG *pcbWritten);
        
        HRESULT ( STDMETHODCALLTYPE *Flush )( 
            __RPC__in ILockBytes * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetSize )( 
            __RPC__in ILockBytes * This,
            /* [in] */ ULARGE_INTEGER cb);
        
        HRESULT ( STDMETHODCALLTYPE *LockRegion )( 
            __RPC__in ILockBytes * This,
            /* [in] */ ULARGE_INTEGER libOffset,
            /* [in] */ ULARGE_INTEGER cb,
            /* [in] */ DWORD dwLockType);
        
        HRESULT ( STDMETHODCALLTYPE *UnlockRegion )( 
            __RPC__in ILockBytes * This,
            /* [in] */ ULARGE_INTEGER libOffset,
            /* [in] */ ULARGE_INTEGER cb,
            /* [in] */ DWORD dwLockType);
        
        HRESULT ( STDMETHODCALLTYPE *Stat )( 
            __RPC__in ILockBytes * This,
            /* [out] */ __RPC__out STATSTG *pstatstg,
            /* [in] */ DWORD grfStatFlag);
        
        END_INTERFACE
    } ILockBytesVtbl;

    interface ILockBytes
    {
        CONST_VTBL struct ILockBytesVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILockBytes_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ILockBytes_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ILockBytes_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ILockBytes_ReadAt(This,ulOffset,pv,cb,pcbRead)	\
    ( (This)->lpVtbl -> ReadAt(This,ulOffset,pv,cb,pcbRead) ) 

#define ILockBytes_WriteAt(This,ulOffset,pv,cb,pcbWritten)	\
    ( (This)->lpVtbl -> WriteAt(This,ulOffset,pv,cb,pcbWritten) ) 

#define ILockBytes_Flush(This)	\
    ( (This)->lpVtbl -> Flush(This) ) 

#define ILockBytes_SetSize(This,cb)	\
    ( (This)->lpVtbl -> SetSize(This,cb) ) 

#define ILockBytes_LockRegion(This,libOffset,cb,dwLockType)	\
    ( (This)->lpVtbl -> LockRegion(This,libOffset,cb,dwLockType) ) 

#define ILockBytes_UnlockRegion(This,libOffset,cb,dwLockType)	\
    ( (This)->lpVtbl -> UnlockRegion(This,libOffset,cb,dwLockType) ) 

#define ILockBytes_Stat(This,pstatstg,grfStatFlag)	\
    ( (This)->lpVtbl -> Stat(This,pstatstg,grfStatFlag) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT __stdcall ILockBytes_RemoteReadAt_Proxy( 
    __RPC__in ILockBytes * This,
    /* [in] */ ULARGE_INTEGER ulOffset,
    /* [length_is][size_is][out] */ __RPC__out_ecount_part(cb, *pcbRead) byte *pv,
    /* [in] */ ULONG cb,
    /* [out] */ __RPC__out ULONG *pcbRead);


void __RPC_STUB ILockBytes_RemoteReadAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT STDMETHODCALLTYPE ILockBytes_RemoteWriteAt_Proxy( 
    __RPC__in ILockBytes * This,
    /* [in] */ ULARGE_INTEGER ulOffset,
    /* [size_is][in] */ __RPC__in_ecount_full(cb) const byte *pv,
    /* [in] */ ULONG cb,
    /* [out] */ __RPC__out ULONG *pcbWritten);


void __RPC_STUB ILockBytes_RemoteWriteAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ILockBytes_INTERFACE_DEFINED__ */


#ifndef __IEnumFORMATETC_INTERFACE_DEFINED__
#define __IEnumFORMATETC_INTERFACE_DEFINED__

/* interface IEnumFORMATETC */
/* [unique][uuid][object] */ 

typedef /* [unique] */  __RPC_unique_pointer IEnumFORMATETC *LPENUMFORMATETC;


typedef struct tagDVTARGETDEVICE
    {
    DWORD tdSize;
    WORD tdDriverNameOffset;
    WORD tdDeviceNameOffset;
    WORD tdPortNameOffset;
    WORD tdExtDevmodeOffset;
    BYTE tdData[ 1 ];
    } 	DVTARGETDEVICE;


typedef CLIPFORMAT *LPCLIPFORMAT;

typedef struct tagFORMATETC
    {
    CLIPFORMAT cfFormat;
    DVTARGETDEVICE *ptd;
    DWORD dwAspect;
    LONG lindex;
    DWORD tymed;
    } 	FORMATETC;

typedef struct tagFORMATETC *LPFORMATETC;


EXTERN_C const IID IID_IEnumFORMATETC;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000103-0000-0000-C000-000000000046")
    IEnumFORMATETC : public IUnknown
    {
    public:
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [annotation] */ 
            __out_ecount_part(celt,*pceltFetched)  FORMATETC *rgelt,
            /* [annotation] */ 
            __out_opt  ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ __RPC__deref_out_opt IEnumFORMATETC **ppenum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumFORMATETCVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IEnumFORMATETC * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IEnumFORMATETC * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IEnumFORMATETC * This);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumFORMATETC * This,
            /* [in] */ ULONG celt,
            /* [annotation] */ 
            __out_ecount_part(celt,*pceltFetched)  FORMATETC *rgelt,
            /* [annotation] */ 
            __out_opt  ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            __RPC__in IEnumFORMATETC * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            __RPC__in IEnumFORMATETC * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            __RPC__in IEnumFORMATETC * This,
            /* [out] */ __RPC__deref_out_opt IEnumFORMATETC **ppenum);
        
        END_INTERFACE
    } IEnumFORMATETCVtbl;

    interface IEnumFORMATETC
    {
        CONST_VTBL struct IEnumFORMATETCVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumFORMATETC_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IEnumFORMATETC_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IEnumFORMATETC_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IEnumFORMATETC_Next(This,celt,rgelt,pceltFetched)	\
    ( (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched) ) 

#define IEnumFORMATETC_Skip(This,celt)	\
    ( (This)->lpVtbl -> Skip(This,celt) ) 

#define IEnumFORMATETC_Reset(This)	\
    ( (This)->lpVtbl -> Reset(This) ) 

#define IEnumFORMATETC_Clone(This,ppenum)	\
    ( (This)->lpVtbl -> Clone(This,ppenum) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT STDMETHODCALLTYPE IEnumFORMATETC_RemoteNext_Proxy( 
    __RPC__in IEnumFORMATETC * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ __RPC__out_ecount_part(celt, *pceltFetched) FORMATETC *rgelt,
    /* [out] */ __RPC__out ULONG *pceltFetched);


void __RPC_STUB IEnumFORMATETC_RemoteNext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumFORMATETC_INTERFACE_DEFINED__ */


#ifndef __IEnumSTATDATA_INTERFACE_DEFINED__
#define __IEnumSTATDATA_INTERFACE_DEFINED__

/* interface IEnumSTATDATA */
/* [unique][uuid][object] */ 

typedef /* [unique] */  __RPC_unique_pointer IEnumSTATDATA *LPENUMSTATDATA;

typedef 
enum tagADVF
    {	ADVF_NODATA	= 1,
	ADVF_PRIMEFIRST	= 2,
	ADVF_ONLYONCE	= 4,
	ADVF_DATAONSTOP	= 64,
	ADVFCACHE_NOHANDLER	= 8,
	ADVFCACHE_FORCEBUILTIN	= 16,
	ADVFCACHE_ONSAVE	= 32
    } 	ADVF;

typedef struct tagSTATDATA
    {
    FORMATETC formatetc;
    DWORD advf;
    IAdviseSink *pAdvSink;
    DWORD dwConnection;
    } 	STATDATA;

typedef STATDATA *LPSTATDATA;


EXTERN_C const IID IID_IEnumSTATDATA;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000105-0000-0000-C000-000000000046")
    IEnumSTATDATA : public IUnknown
    {
    public:
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [annotation] */ 
            __out_ecount_part(celt,*pceltFetched)  STATDATA *rgelt,
            /* [annotation] */ 
            __out_opt  ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ __RPC__deref_out_opt IEnumSTATDATA **ppenum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumSTATDATAVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IEnumSTATDATA * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IEnumSTATDATA * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IEnumSTATDATA * This);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumSTATDATA * This,
            /* [in] */ ULONG celt,
            /* [annotation] */ 
            __out_ecount_part(celt,*pceltFetched)  STATDATA *rgelt,
            /* [annotation] */ 
            __out_opt  ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            __RPC__in IEnumSTATDATA * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            __RPC__in IEnumSTATDATA * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            __RPC__in IEnumSTATDATA * This,
            /* [out] */ __RPC__deref_out_opt IEnumSTATDATA **ppenum);
        
        END_INTERFACE
    } IEnumSTATDATAVtbl;

    interface IEnumSTATDATA
    {
        CONST_VTBL struct IEnumSTATDATAVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumSTATDATA_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IEnumSTATDATA_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IEnumSTATDATA_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IEnumSTATDATA_Next(This,celt,rgelt,pceltFetched)	\
    ( (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched) ) 

#define IEnumSTATDATA_Skip(This,celt)	\
    ( (This)->lpVtbl -> Skip(This,celt) ) 

#define IEnumSTATDATA_Reset(This)	\
    ( (This)->lpVtbl -> Reset(This) ) 

#define IEnumSTATDATA_Clone(This,ppenum)	\
    ( (This)->lpVtbl -> Clone(This,ppenum) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT STDMETHODCALLTYPE IEnumSTATDATA_RemoteNext_Proxy( 
    __RPC__in IEnumSTATDATA * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ __RPC__out_ecount_part(celt, *pceltFetched) STATDATA *rgelt,
    /* [out] */ __RPC__out ULONG *pceltFetched);


void __RPC_STUB IEnumSTATDATA_RemoteNext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumSTATDATA_INTERFACE_DEFINED__ */


#ifndef __IRootStorage_INTERFACE_DEFINED__
#define __IRootStorage_INTERFACE_DEFINED__

/* interface IRootStorage */
/* [unique][uuid][object] */ 

typedef /* [unique] */  __RPC_unique_pointer IRootStorage *LPROOTSTORAGE;


EXTERN_C const IID IID_IRootStorage;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000012-0000-0000-C000-000000000046")
    IRootStorage : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SwitchToFile( 
            /* [in] */ __RPC__in LPOLESTR pszFile) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IRootStorageVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IRootStorage * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IRootStorage * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IRootStorage * This);
        
        HRESULT ( STDMETHODCALLTYPE *SwitchToFile )( 
            __RPC__in IRootStorage * This,
            /* [in] */ __RPC__in LPOLESTR pszFile);
        
        END_INTERFACE
    } IRootStorageVtbl;

    interface IRootStorage
    {
        CONST_VTBL struct IRootStorageVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRootStorage_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IRootStorage_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IRootStorage_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IRootStorage_SwitchToFile(This,pszFile)	\
    ( (This)->lpVtbl -> SwitchToFile(This,pszFile) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IRootStorage_INTERFACE_DEFINED__ */


#ifndef __IAdviseSink_INTERFACE_DEFINED__
#define __IAdviseSink_INTERFACE_DEFINED__

/* interface IAdviseSink */
/* [unique][async_uuid][uuid][object] */ 

typedef IAdviseSink *LPADVISESINK;

typedef /* [v1_enum] */ 
enum tagTYMED
    {	TYMED_HGLOBAL	= 1,
	TYMED_FILE	= 2,
	TYMED_ISTREAM	= 4,
	TYMED_ISTORAGE	= 8,
	TYMED_GDI	= 16,
	TYMED_MFPICT	= 32,
	TYMED_ENHMF	= 64,
	TYMED_NULL	= 0
    } 	TYMED;

#ifndef RC_INVOKED
#if _MSC_VER >= 1200
#pragma warning(push)
#endif
#pragma warning(disable:4200)
#endif
typedef struct tagRemSTGMEDIUM
    {
    DWORD tymed;
    DWORD dwHandleType;
    unsigned long pData;
    unsigned long pUnkForRelease;
    unsigned long cbData;
    byte data[ 1 ];
    } 	RemSTGMEDIUM;

#ifndef RC_INVOKED
#if _MSC_VER >= 1200
#pragma warning(pop)
#else
#pragma warning(default:4200)
#endif
#endif
#ifdef NONAMELESSUNION
typedef struct tagSTGMEDIUM {
    DWORD tymed;
    union {
        HBITMAP hBitmap;
        HMETAFILEPICT hMetaFilePict;
        HENHMETAFILE hEnhMetaFile;
        HGLOBAL hGlobal;
        LPOLESTR lpszFileName;
        IStream *pstm;
        IStorage *pstg;
        } u;
    IUnknown *pUnkForRelease;
}uSTGMEDIUM;
#else
typedef struct tagSTGMEDIUM
    {
    DWORD tymed;
    union 
        {
        HBITMAP hBitmap;
        HMETAFILEPICT hMetaFilePict;
        HENHMETAFILE hEnhMetaFile;
        HGLOBAL hGlobal;
        LPOLESTR lpszFileName;
        IStream *pstm;
        IStorage *pstg;
         /* Empty union arm */ 
        } 	DUMMYUNIONNAME;
    IUnknown *pUnkForRelease;
    } 	uSTGMEDIUM;

#endif /* !NONAMELESSUNION */
typedef struct _GDI_OBJECT
    {
    DWORD ObjectType;
    /* [switch_type] */ union __MIDL_IAdviseSink_0002
        {
        wireHBITMAP hBitmap;
        wireHPALETTE hPalette;
        wireHGLOBAL hGeneric;
        } 	u;
    } 	GDI_OBJECT;

typedef struct _userSTGMEDIUM
    {
    struct _STGMEDIUM_UNION
        {
        DWORD tymed;
        /* [switch_type] */ union __MIDL_IAdviseSink_0003
            {
             /* Empty union arm */ 
            wireHMETAFILEPICT hMetaFilePict;
            wireHENHMETAFILE hHEnhMetaFile;
            GDI_OBJECT *hGdiHandle;
            wireHGLOBAL hGlobal;
            LPOLESTR lpszFileName;
            BYTE_BLOB *pstm;
            BYTE_BLOB *pstg;
            } 	u;
        } 	DUMMYUNIONNAME;
    IUnknown *pUnkForRelease;
    } 	userSTGMEDIUM;

typedef /* [unique] */  __RPC_unique_pointer userSTGMEDIUM *wireSTGMEDIUM;

typedef /* [wire_marshal] */ uSTGMEDIUM STGMEDIUM;

typedef /* [unique] */  __RPC_unique_pointer userSTGMEDIUM *wireASYNC_STGMEDIUM;

typedef /* [wire_marshal] */ STGMEDIUM ASYNC_STGMEDIUM;

typedef STGMEDIUM *LPSTGMEDIUM;

typedef struct _userFLAG_STGMEDIUM
    {
    long ContextFlags;
    long fPassOwnership;
    userSTGMEDIUM Stgmed;
    } 	userFLAG_STGMEDIUM;

typedef /* [unique] */  __RPC_unique_pointer userFLAG_STGMEDIUM *wireFLAG_STGMEDIUM;

typedef /* [wire_marshal] */ struct _FLAG_STGMEDIUM
    {
    long ContextFlags;
    long fPassOwnership;
    STGMEDIUM Stgmed;
    } 	FLAG_STGMEDIUM;


EXTERN_C const IID IID_IAdviseSink;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0000010f-0000-0000-C000-000000000046")
    IAdviseSink : public IUnknown
    {
    public:
        virtual /* [local] */ void STDMETHODCALLTYPE OnDataChange( 
            /* [unique][in] */ FORMATETC *pFormatetc,
            /* [unique][in] */ STGMEDIUM *pStgmed) = 0;
        
        virtual /* [local] */ void STDMETHODCALLTYPE OnViewChange( 
            /* [in] */ DWORD dwAspect,
            /* [in] */ LONG lindex) = 0;
        
        virtual /* [local] */ void STDMETHODCALLTYPE OnRename( 
            /* [in] */ IMoniker *pmk) = 0;
        
        virtual /* [local] */ void STDMETHODCALLTYPE OnSave( void) = 0;
        
        virtual /* [local] */ void STDMETHODCALLTYPE OnClose( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAdviseSinkVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IAdviseSink * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IAdviseSink * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IAdviseSink * This);
        
        /* [local] */ void ( STDMETHODCALLTYPE *OnDataChange )( 
            IAdviseSink * This,
            /* [unique][in] */ FORMATETC *pFormatetc,
            /* [unique][in] */ STGMEDIUM *pStgmed);
        
        /* [local] */ void ( STDMETHODCALLTYPE *OnViewChange )( 
            IAdviseSink * This,
            /* [in] */ DWORD dwAspect,
            /* [in] */ LONG lindex);
        
        /* [local] */ void ( STDMETHODCALLTYPE *OnRename )( 
            IAdviseSink * This,
            /* [in] */ IMoniker *pmk);
        
        /* [local] */ void ( STDMETHODCALLTYPE *OnSave )( 
            IAdviseSink * This);
        
        /* [local] */ void ( STDMETHODCALLTYPE *OnClose )( 
            IAdviseSink * This);
        
        END_INTERFACE
    } IAdviseSinkVtbl;

    interface IAdviseSink
    {
        CONST_VTBL struct IAdviseSinkVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAdviseSink_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAdviseSink_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAdviseSink_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAdviseSink_OnDataChange(This,pFormatetc,pStgmed)	\
    ( (This)->lpVtbl -> OnDataChange(This,pFormatetc,pStgmed) ) 

#define IAdviseSink_OnViewChange(This,dwAspect,lindex)	\
    ( (This)->lpVtbl -> OnViewChange(This,dwAspect,lindex) ) 

#define IAdviseSink_OnRename(This,pmk)	\
    ( (This)->lpVtbl -> OnRename(This,pmk) ) 

#define IAdviseSink_OnSave(This)	\
    ( (This)->lpVtbl -> OnSave(This) ) 

#define IAdviseSink_OnClose(This)	\
    ( (This)->lpVtbl -> OnClose(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT STDMETHODCALLTYPE IAdviseSink_RemoteOnDataChange_Proxy( 
    __RPC__in IAdviseSink * This,
    /* [unique][in] */ __RPC__in_opt FORMATETC *pFormatetc,
    /* [unique][in] */ __RPC__in_opt ASYNC_STGMEDIUM *pStgmed);


void __RPC_STUB IAdviseSink_RemoteOnDataChange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IAdviseSink_RemoteOnViewChange_Proxy( 
    __RPC__in IAdviseSink * This,
    /* [in] */ DWORD dwAspect,
    /* [in] */ LONG lindex);


void __RPC_STUB IAdviseSink_RemoteOnViewChange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IAdviseSink_RemoteOnRename_Proxy( 
    __RPC__in IAdviseSink * This,
    /* [in] */ __RPC__in_opt IMoniker *pmk);


void __RPC_STUB IAdviseSink_RemoteOnRename_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IAdviseSink_RemoteOnSave_Proxy( 
    __RPC__in IAdviseSink * This);


void __RPC_STUB IAdviseSink_RemoteOnSave_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IAdviseSink_RemoteOnClose_Proxy( 
    __RPC__in IAdviseSink * This);


void __RPC_STUB IAdviseSink_RemoteOnClose_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAdviseSink_INTERFACE_DEFINED__ */


#ifndef __AsyncIAdviseSink_INTERFACE_DEFINED__
#define __AsyncIAdviseSink_INTERFACE_DEFINED__

/* interface AsyncIAdviseSink */
/* [uuid][unique][object] */ 


EXTERN_C const IID IID_AsyncIAdviseSink;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000150-0000-0000-C000-000000000046")
    AsyncIAdviseSink : public IUnknown
    {
    public:
        virtual /* [local] */ void STDMETHODCALLTYPE Begin_OnDataChange( 
            /* [unique][in] */ FORMATETC *pFormatetc,
            /* [unique][in] */ STGMEDIUM *pStgmed) = 0;
        
        virtual /* [local] */ void STDMETHODCALLTYPE Finish_OnDataChange( void) = 0;
        
        virtual /* [local] */ void STDMETHODCALLTYPE Begin_OnViewChange( 
            /* [in] */ DWORD dwAspect,
            /* [in] */ LONG lindex) = 0;
        
        virtual /* [local] */ void STDMETHODCALLTYPE Finish_OnViewChange( void) = 0;
        
        virtual /* [local] */ void STDMETHODCALLTYPE Begin_OnRename( 
            /* [in] */ IMoniker *pmk) = 0;
        
        virtual /* [local] */ void STDMETHODCALLTYPE Finish_OnRename( void) = 0;
        
        virtual /* [local] */ void STDMETHODCALLTYPE Begin_OnSave( void) = 0;
        
        virtual /* [local] */ void STDMETHODCALLTYPE Finish_OnSave( void) = 0;
        
        virtual /* [local] */ void STDMETHODCALLTYPE Begin_OnClose( void) = 0;
        
        virtual /* [local] */ void STDMETHODCALLTYPE Finish_OnClose( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct AsyncIAdviseSinkVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in AsyncIAdviseSink * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in AsyncIAdviseSink * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in AsyncIAdviseSink * This);
        
        /* [local] */ void ( STDMETHODCALLTYPE *Begin_OnDataChange )( 
            AsyncIAdviseSink * This,
            /* [unique][in] */ FORMATETC *pFormatetc,
            /* [unique][in] */ STGMEDIUM *pStgmed);
        
        /* [local] */ void ( STDMETHODCALLTYPE *Finish_OnDataChange )( 
            AsyncIAdviseSink * This);
        
        /* [local] */ void ( STDMETHODCALLTYPE *Begin_OnViewChange )( 
            AsyncIAdviseSink * This,
            /* [in] */ DWORD dwAspect,
            /* [in] */ LONG lindex);
        
        /* [local] */ void ( STDMETHODCALLTYPE *Finish_OnViewChange )( 
            AsyncIAdviseSink * This);
        
        /* [local] */ void ( STDMETHODCALLTYPE *Begin_OnRename )( 
            AsyncIAdviseSink * This,
            /* [in] */ IMoniker *pmk);
        
        /* [local] */ void ( STDMETHODCALLTYPE *Finish_OnRename )( 
            AsyncIAdviseSink * This);
        
        /* [local] */ void ( STDMETHODCALLTYPE *Begin_OnSave )( 
            AsyncIAdviseSink * This);
        
        /* [local] */ void ( STDMETHODCALLTYPE *Finish_OnSave )( 
            AsyncIAdviseSink * This);
        
        /* [local] */ void ( STDMETHODCALLTYPE *Begin_OnClose )( 
            AsyncIAdviseSink * This);
        
        /* [local] */ void ( STDMETHODCALLTYPE *Finish_OnClose )( 
            AsyncIAdviseSink * This);
        
        END_INTERFACE
    } AsyncIAdviseSinkVtbl;

    interface AsyncIAdviseSink
    {
        CONST_VTBL struct AsyncIAdviseSinkVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define AsyncIAdviseSink_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define AsyncIAdviseSink_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define AsyncIAdviseSink_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define AsyncIAdviseSink_Begin_OnDataChange(This,pFormatetc,pStgmed)	\
    ( (This)->lpVtbl -> Begin_OnDataChange(This,pFormatetc,pStgmed) ) 

#define AsyncIAdviseSink_Finish_OnDataChange(This)	\
    ( (This)->lpVtbl -> Finish_OnDataChange(This) ) 

#define AsyncIAdviseSink_Begin_OnViewChange(This,dwAspect,lindex)	\
    ( (This)->lpVtbl -> Begin_OnViewChange(This,dwAspect,lindex) ) 

#define AsyncIAdviseSink_Finish_OnViewChange(This)	\
    ( (This)->lpVtbl -> Finish_OnViewChange(This) ) 

#define AsyncIAdviseSink_Begin_OnRename(This,pmk)	\
    ( (This)->lpVtbl -> Begin_OnRename(This,pmk) ) 

#define AsyncIAdviseSink_Finish_OnRename(This)	\
    ( (This)->lpVtbl -> Finish_OnRename(This) ) 

#define AsyncIAdviseSink_Begin_OnSave(This)	\
    ( (This)->lpVtbl -> Begin_OnSave(This) ) 

#define AsyncIAdviseSink_Finish_OnSave(This)	\
    ( (This)->lpVtbl -> Finish_OnSave(This) ) 

#define AsyncIAdviseSink_Begin_OnClose(This)	\
    ( (This)->lpVtbl -> Begin_OnClose(This) ) 

#define AsyncIAdviseSink_Finish_OnClose(This)	\
    ( (This)->lpVtbl -> Finish_OnClose(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink_Begin_RemoteOnDataChange_Proxy( 
    __RPC__in AsyncIAdviseSink * This,
    /* [unique][in] */ __RPC__in_opt FORMATETC *pFormatetc,
    /* [unique][in] */ __RPC__in_opt ASYNC_STGMEDIUM *pStgmed);


void __RPC_STUB AsyncIAdviseSink_Begin_RemoteOnDataChange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink_Finish_RemoteOnDataChange_Proxy( 
    __RPC__in AsyncIAdviseSink * This);


void __RPC_STUB AsyncIAdviseSink_Finish_RemoteOnDataChange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink_Begin_RemoteOnViewChange_Proxy( 
    __RPC__in AsyncIAdviseSink * This,
    /* [in] */ DWORD dwAspect,
    /* [in] */ LONG lindex);


void __RPC_STUB AsyncIAdviseSink_Begin_RemoteOnViewChange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink_Finish_RemoteOnViewChange_Proxy( 
    __RPC__in AsyncIAdviseSink * This);


void __RPC_STUB AsyncIAdviseSink_Finish_RemoteOnViewChange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink_Begin_RemoteOnRename_Proxy( 
    __RPC__in AsyncIAdviseSink * This,
    /* [in] */ __RPC__in_opt IMoniker *pmk);


void __RPC_STUB AsyncIAdviseSink_Begin_RemoteOnRename_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink_Finish_RemoteOnRename_Proxy( 
    __RPC__in AsyncIAdviseSink * This);


void __RPC_STUB AsyncIAdviseSink_Finish_RemoteOnRename_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink_Begin_RemoteOnSave_Proxy( 
    __RPC__in AsyncIAdviseSink * This);


void __RPC_STUB AsyncIAdviseSink_Begin_RemoteOnSave_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink_Finish_RemoteOnSave_Proxy( 
    __RPC__in AsyncIAdviseSink * This);


void __RPC_STUB AsyncIAdviseSink_Finish_RemoteOnSave_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink_Begin_RemoteOnClose_Proxy( 
    __RPC__in AsyncIAdviseSink * This);


void __RPC_STUB AsyncIAdviseSink_Begin_RemoteOnClose_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink_Finish_RemoteOnClose_Proxy( 
    __RPC__in AsyncIAdviseSink * This);


void __RPC_STUB AsyncIAdviseSink_Finish_RemoteOnClose_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __AsyncIAdviseSink_INTERFACE_DEFINED__ */


#ifndef __IAdviseSink2_INTERFACE_DEFINED__
#define __IAdviseSink2_INTERFACE_DEFINED__

/* interface IAdviseSink2 */
/* [unique][async_uuid][uuid][object] */ 

typedef /* [unique] */  __RPC_unique_pointer IAdviseSink2 *LPADVISESINK2;


EXTERN_C const IID IID_IAdviseSink2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000125-0000-0000-C000-000000000046")
    IAdviseSink2 : public IAdviseSink
    {
    public:
        virtual /* [local] */ void STDMETHODCALLTYPE OnLinkSrcChange( 
            /* [unique][in] */ IMoniker *pmk) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAdviseSink2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IAdviseSink2 * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IAdviseSink2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IAdviseSink2 * This);
        
        /* [local] */ void ( STDMETHODCALLTYPE *OnDataChange )( 
            IAdviseSink2 * This,
            /* [unique][in] */ FORMATETC *pFormatetc,
            /* [unique][in] */ STGMEDIUM *pStgmed);
        
        /* [local] */ void ( STDMETHODCALLTYPE *OnViewChange )( 
            IAdviseSink2 * This,
            /* [in] */ DWORD dwAspect,
            /* [in] */ LONG lindex);
        
        /* [local] */ void ( STDMETHODCALLTYPE *OnRename )( 
            IAdviseSink2 * This,
            /* [in] */ IMoniker *pmk);
        
        /* [local] */ void ( STDMETHODCALLTYPE *OnSave )( 
            IAdviseSink2 * This);
        
        /* [local] */ void ( STDMETHODCALLTYPE *OnClose )( 
            IAdviseSink2 * This);
        
        /* [local] */ void ( STDMETHODCALLTYPE *OnLinkSrcChange )( 
            IAdviseSink2 * This,
            /* [unique][in] */ IMoniker *pmk);
        
        END_INTERFACE
    } IAdviseSink2Vtbl;

    interface IAdviseSink2
    {
        CONST_VTBL struct IAdviseSink2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAdviseSink2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAdviseSink2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAdviseSink2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAdviseSink2_OnDataChange(This,pFormatetc,pStgmed)	\
    ( (This)->lpVtbl -> OnDataChange(This,pFormatetc,pStgmed) ) 

#define IAdviseSink2_OnViewChange(This,dwAspect,lindex)	\
    ( (This)->lpVtbl -> OnViewChange(This,dwAspect,lindex) ) 

#define IAdviseSink2_OnRename(This,pmk)	\
    ( (This)->lpVtbl -> OnRename(This,pmk) ) 

#define IAdviseSink2_OnSave(This)	\
    ( (This)->lpVtbl -> OnSave(This) ) 

#define IAdviseSink2_OnClose(This)	\
    ( (This)->lpVtbl -> OnClose(This) ) 


#define IAdviseSink2_OnLinkSrcChange(This,pmk)	\
    ( (This)->lpVtbl -> OnLinkSrcChange(This,pmk) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT STDMETHODCALLTYPE IAdviseSink2_RemoteOnLinkSrcChange_Proxy( 
    __RPC__in IAdviseSink2 * This,
    /* [unique][in] */ __RPC__in_opt IMoniker *pmk);


void __RPC_STUB IAdviseSink2_RemoteOnLinkSrcChange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAdviseSink2_INTERFACE_DEFINED__ */


#ifndef __AsyncIAdviseSink2_INTERFACE_DEFINED__
#define __AsyncIAdviseSink2_INTERFACE_DEFINED__

/* interface AsyncIAdviseSink2 */
/* [uuid][unique][object] */ 


EXTERN_C const IID IID_AsyncIAdviseSink2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000151-0000-0000-C000-000000000046")
    AsyncIAdviseSink2 : public AsyncIAdviseSink
    {
    public:
        virtual /* [local] */ void STDMETHODCALLTYPE Begin_OnLinkSrcChange( 
            /* [unique][in] */ IMoniker *pmk) = 0;
        
        virtual /* [local] */ void STDMETHODCALLTYPE Finish_OnLinkSrcChange( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct AsyncIAdviseSink2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in AsyncIAdviseSink2 * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in AsyncIAdviseSink2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in AsyncIAdviseSink2 * This);
        
        /* [local] */ void ( STDMETHODCALLTYPE *Begin_OnDataChange )( 
            AsyncIAdviseSink2 * This,
            /* [unique][in] */ FORMATETC *pFormatetc,
            /* [unique][in] */ STGMEDIUM *pStgmed);
        
        /* [local] */ void ( STDMETHODCALLTYPE *Finish_OnDataChange )( 
            AsyncIAdviseSink2 * This);
        
        /* [local] */ void ( STDMETHODCALLTYPE *Begin_OnViewChange )( 
            AsyncIAdviseSink2 * This,
            /* [in] */ DWORD dwAspect,
            /* [in] */ LONG lindex);
        
        /* [local] */ void ( STDMETHODCALLTYPE *Finish_OnViewChange )( 
            AsyncIAdviseSink2 * This);
        
        /* [local] */ void ( STDMETHODCALLTYPE *Begin_OnRename )( 
            AsyncIAdviseSink2 * This,
            /* [in] */ IMoniker *pmk);
        
        /* [local] */ void ( STDMETHODCALLTYPE *Finish_OnRename )( 
            AsyncIAdviseSink2 * This);
        
        /* [local] */ void ( STDMETHODCALLTYPE *Begin_OnSave )( 
            AsyncIAdviseSink2 * This);
        
        /* [local] */ void ( STDMETHODCALLTYPE *Finish_OnSave )( 
            AsyncIAdviseSink2 * This);
        
        /* [local] */ void ( STDMETHODCALLTYPE *Begin_OnClose )( 
            AsyncIAdviseSink2 * This);
        
        /* [local] */ void ( STDMETHODCALLTYPE *Finish_OnClose )( 
            AsyncIAdviseSink2 * This);
        
        /* [local] */ void ( STDMETHODCALLTYPE *Begin_OnLinkSrcChange )( 
            AsyncIAdviseSink2 * This,
            /* [unique][in] */ IMoniker *pmk);
        
        /* [local] */ void ( STDMETHODCALLTYPE *Finish_OnLinkSrcChange )( 
            AsyncIAdviseSink2 * This);
        
        END_INTERFACE
    } AsyncIAdviseSink2Vtbl;

    interface AsyncIAdviseSink2
    {
        CONST_VTBL struct AsyncIAdviseSink2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define AsyncIAdviseSink2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define AsyncIAdviseSink2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define AsyncIAdviseSink2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define AsyncIAdviseSink2_Begin_OnDataChange(This,pFormatetc,pStgmed)	\
    ( (This)->lpVtbl -> Begin_OnDataChange(This,pFormatetc,pStgmed) ) 

#define AsyncIAdviseSink2_Finish_OnDataChange(This)	\
    ( (This)->lpVtbl -> Finish_OnDataChange(This) ) 

#define AsyncIAdviseSink2_Begin_OnViewChange(This,dwAspect,lindex)	\
    ( (This)->lpVtbl -> Begin_OnViewChange(This,dwAspect,lindex) ) 

#define AsyncIAdviseSink2_Finish_OnViewChange(This)	\
    ( (This)->lpVtbl -> Finish_OnViewChange(This) ) 

#define AsyncIAdviseSink2_Begin_OnRename(This,pmk)	\
    ( (This)->lpVtbl -> Begin_OnRename(This,pmk) ) 

#define AsyncIAdviseSink2_Finish_OnRename(This)	\
    ( (This)->lpVtbl -> Finish_OnRename(This) ) 

#define AsyncIAdviseSink2_Begin_OnSave(This)	\
    ( (This)->lpVtbl -> Begin_OnSave(This) ) 

#define AsyncIAdviseSink2_Finish_OnSave(This)	\
    ( (This)->lpVtbl -> Finish_OnSave(This) ) 

#define AsyncIAdviseSink2_Begin_OnClose(This)	\
    ( (This)->lpVtbl -> Begin_OnClose(This) ) 

#define AsyncIAdviseSink2_Finish_OnClose(This)	\
    ( (This)->lpVtbl -> Finish_OnClose(This) ) 


#define AsyncIAdviseSink2_Begin_OnLinkSrcChange(This,pmk)	\
    ( (This)->lpVtbl -> Begin_OnLinkSrcChange(This,pmk) ) 

#define AsyncIAdviseSink2_Finish_OnLinkSrcChange(This)	\
    ( (This)->lpVtbl -> Finish_OnLinkSrcChange(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink2_Begin_RemoteOnLinkSrcChange_Proxy( 
    __RPC__in AsyncIAdviseSink2 * This,
    /* [unique][in] */ __RPC__in_opt IMoniker *pmk);


void __RPC_STUB AsyncIAdviseSink2_Begin_RemoteOnLinkSrcChange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink2_Finish_RemoteOnLinkSrcChange_Proxy( 
    __RPC__in AsyncIAdviseSink2 * This);


void __RPC_STUB AsyncIAdviseSink2_Finish_RemoteOnLinkSrcChange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __AsyncIAdviseSink2_INTERFACE_DEFINED__ */


#ifndef __IDataObject_INTERFACE_DEFINED__
#define __IDataObject_INTERFACE_DEFINED__

/* interface IDataObject */
/* [unique][uuid][object] */ 

typedef /* [unique] */  __RPC_unique_pointer IDataObject *LPDATAOBJECT;

typedef 
enum tagDATADIR
    {	DATADIR_GET	= 1,
	DATADIR_SET	= 2
    } 	DATADIR;


EXTERN_C const IID IID_IDataObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0000010e-0000-0000-C000-000000000046")
    IDataObject : public IUnknown
    {
    public:
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE GetData( 
            /* [unique][in] */ FORMATETC *pformatetcIn,
            /* [out] */ STGMEDIUM *pmedium) = 0;
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE GetDataHere( 
            /* [unique][in] */ FORMATETC *pformatetc,
            /* [out][in] */ STGMEDIUM *pmedium) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE QueryGetData( 
            /* [unique][in] */ __RPC__in_opt FORMATETC *pformatetc) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCanonicalFormatEtc( 
            /* [unique][in] */ __RPC__in_opt FORMATETC *pformatectIn,
            /* [out] */ __RPC__out FORMATETC *pformatetcOut) = 0;
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE SetData( 
            /* [unique][in] */ FORMATETC *pformatetc,
            /* [unique][in] */ STGMEDIUM *pmedium,
            /* [in] */ BOOL fRelease) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumFormatEtc( 
            /* [in] */ DWORD dwDirection,
            /* [out] */ __RPC__deref_out_opt IEnumFORMATETC **ppenumFormatEtc) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DAdvise( 
            /* [in] */ __RPC__in FORMATETC *pformatetc,
            /* [in] */ DWORD advf,
            /* [unique][in] */ __RPC__in_opt IAdviseSink *pAdvSink,
            /* [out] */ __RPC__out DWORD *pdwConnection) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DUnadvise( 
            /* [in] */ DWORD dwConnection) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumDAdvise( 
            /* [out] */ __RPC__deref_out_opt IEnumSTATDATA **ppenumAdvise) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDataObjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IDataObject * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IDataObject * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IDataObject * This);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *GetData )( 
            IDataObject * This,
            /* [unique][in] */ FORMATETC *pformatetcIn,
            /* [out] */ STGMEDIUM *pmedium);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *GetDataHere )( 
            IDataObject * This,
            /* [unique][in] */ FORMATETC *pformatetc,
            /* [out][in] */ STGMEDIUM *pmedium);
        
        HRESULT ( STDMETHODCALLTYPE *QueryGetData )( 
            __RPC__in IDataObject * This,
            /* [unique][in] */ __RPC__in_opt FORMATETC *pformatetc);
        
        HRESULT ( STDMETHODCALLTYPE *GetCanonicalFormatEtc )( 
            __RPC__in IDataObject * This,
            /* [unique][in] */ __RPC__in_opt FORMATETC *pformatectIn,
            /* [out] */ __RPC__out FORMATETC *pformatetcOut);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *SetData )( 
            IDataObject * This,
            /* [unique][in] */ FORMATETC *pformatetc,
            /* [unique][in] */ STGMEDIUM *pmedium,
            /* [in] */ BOOL fRelease);
        
        HRESULT ( STDMETHODCALLTYPE *EnumFormatEtc )( 
            __RPC__in IDataObject * This,
            /* [in] */ DWORD dwDirection,
            /* [out] */ __RPC__deref_out_opt IEnumFORMATETC **ppenumFormatEtc);
        
        HRESULT ( STDMETHODCALLTYPE *DAdvise )( 
            __RPC__in IDataObject * This,
            /* [in] */ __RPC__in FORMATETC *pformatetc,
            /* [in] */ DWORD advf,
            /* [unique][in] */ __RPC__in_opt IAdviseSink *pAdvSink,
            /* [out] */ __RPC__out DWORD *pdwConnection);
        
        HRESULT ( STDMETHODCALLTYPE *DUnadvise )( 
            __RPC__in IDataObject * This,
            /* [in] */ DWORD dwConnection);
        
        HRESULT ( STDMETHODCALLTYPE *EnumDAdvise )( 
            __RPC__in IDataObject * This,
            /* [out] */ __RPC__deref_out_opt IEnumSTATDATA **ppenumAdvise);
        
        END_INTERFACE
    } IDataObjectVtbl;

    interface IDataObject
    {
        CONST_VTBL struct IDataObjectVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDataObject_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IDataObject_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IDataObject_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IDataObject_GetData(This,pformatetcIn,pmedium)	\
    ( (This)->lpVtbl -> GetData(This,pformatetcIn,pmedium) ) 

#define IDataObject_GetDataHere(This,pformatetc,pmedium)	\
    ( (This)->lpVtbl -> GetDataHere(This,pformatetc,pmedium) ) 

#define IDataObject_QueryGetData(This,pformatetc)	\
    ( (This)->lpVtbl -> QueryGetData(This,pformatetc) ) 

#define IDataObject_GetCanonicalFormatEtc(This,pformatectIn,pformatetcOut)	\
    ( (This)->lpVtbl -> GetCanonicalFormatEtc(This,pformatectIn,pformatetcOut) ) 

#define IDataObject_SetData(This,pformatetc,pmedium,fRelease)	\
    ( (This)->lpVtbl -> SetData(This,pformatetc,pmedium,fRelease) ) 

#define IDataObject_EnumFormatEtc(This,dwDirection,ppenumFormatEtc)	\
    ( (This)->lpVtbl -> EnumFormatEtc(This,dwDirection,ppenumFormatEtc) ) 

#define IDataObject_DAdvise(This,pformatetc,advf,pAdvSink,pdwConnection)	\
    ( (This)->lpVtbl -> DAdvise(This,pformatetc,advf,pAdvSink,pdwConnection) ) 

#define IDataObject_DUnadvise(This,dwConnection)	\
    ( (This)->lpVtbl -> DUnadvise(This,dwConnection) ) 

#define IDataObject_EnumDAdvise(This,ppenumAdvise)	\
    ( (This)->lpVtbl -> EnumDAdvise(This,ppenumAdvise) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT STDMETHODCALLTYPE IDataObject_RemoteGetData_Proxy( 
    __RPC__in IDataObject * This,
    /* [unique][in] */ __RPC__in_opt FORMATETC *pformatetcIn,
    /* [out] */ __RPC__out STGMEDIUM *pRemoteMedium);


void __RPC_STUB IDataObject_RemoteGetData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IDataObject_RemoteGetDataHere_Proxy( 
    __RPC__in IDataObject * This,
    /* [unique][in] */ __RPC__in_opt FORMATETC *pformatetc,
    /* [out][in] */ __RPC__inout STGMEDIUM *pRemoteMedium);


void __RPC_STUB IDataObject_RemoteGetDataHere_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IDataObject_RemoteSetData_Proxy( 
    __RPC__in IDataObject * This,
    /* [unique][in] */ __RPC__in_opt FORMATETC *pformatetc,
    /* [unique][in] */ __RPC__in_opt FLAG_STGMEDIUM *pmedium,
    /* [in] */ BOOL fRelease);


void __RPC_STUB IDataObject_RemoteSetData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDataObject_INTERFACE_DEFINED__ */


#ifndef __IDataAdviseHolder_INTERFACE_DEFINED__
#define __IDataAdviseHolder_INTERFACE_DEFINED__

/* interface IDataAdviseHolder */
/* [uuid][object][local] */ 

typedef /* [unique] */ IDataAdviseHolder *LPDATAADVISEHOLDER;


EXTERN_C const IID IID_IDataAdviseHolder;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000110-0000-0000-C000-000000000046")
    IDataAdviseHolder : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Advise( 
            /* [annotation][unique][in] */ 
            __in_opt  IDataObject *pDataObject,
            /* [annotation][unique][in] */ 
            __in  FORMATETC *pFetc,
            /* [annotation][in] */ 
            __in  DWORD advf,
            /* [annotation][unique][in] */ 
            __in  IAdviseSink *pAdvise,
            /* [annotation][out] */ 
            __out  DWORD *pdwConnection) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Unadvise( 
            /* [annotation][in] */ 
            __in  DWORD dwConnection) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumAdvise( 
            /* [annotation][out] */ 
            __deref_out  IEnumSTATDATA **ppenumAdvise) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SendOnDataChange( 
            /* [annotation][unique][in] */ 
            __in  IDataObject *pDataObject,
            /* [annotation][in] */ 
            __reserved  DWORD dwReserved,
            /* [annotation][in] */ 
            __in  DWORD advf) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDataAdviseHolderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDataAdviseHolder * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDataAdviseHolder * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDataAdviseHolder * This);
        
        HRESULT ( STDMETHODCALLTYPE *Advise )( 
            IDataAdviseHolder * This,
            /* [annotation][unique][in] */ 
            __in_opt  IDataObject *pDataObject,
            /* [annotation][unique][in] */ 
            __in  FORMATETC *pFetc,
            /* [annotation][in] */ 
            __in  DWORD advf,
            /* [annotation][unique][in] */ 
            __in  IAdviseSink *pAdvise,
            /* [annotation][out] */ 
            __out  DWORD *pdwConnection);
        
        HRESULT ( STDMETHODCALLTYPE *Unadvise )( 
            IDataAdviseHolder * This,
            /* [annotation][in] */ 
            __in  DWORD dwConnection);
        
        HRESULT ( STDMETHODCALLTYPE *EnumAdvise )( 
            IDataAdviseHolder * This,
            /* [annotation][out] */ 
            __deref_out  IEnumSTATDATA **ppenumAdvise);
        
        HRESULT ( STDMETHODCALLTYPE *SendOnDataChange )( 
            IDataAdviseHolder * This,
            /* [annotation][unique][in] */ 
            __in  IDataObject *pDataObject,
            /* [annotation][in] */ 
            __reserved  DWORD dwReserved,
            /* [annotation][in] */ 
            __in  DWORD advf);
        
        END_INTERFACE
    } IDataAdviseHolderVtbl;

    interface IDataAdviseHolder
    {
        CONST_VTBL struct IDataAdviseHolderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDataAdviseHolder_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IDataAdviseHolder_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IDataAdviseHolder_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IDataAdviseHolder_Advise(This,pDataObject,pFetc,advf,pAdvise,pdwConnection)	\
    ( (This)->lpVtbl -> Advise(This,pDataObject,pFetc,advf,pAdvise,pdwConnection) ) 

#define IDataAdviseHolder_Unadvise(This,dwConnection)	\
    ( (This)->lpVtbl -> Unadvise(This,dwConnection) ) 

#define IDataAdviseHolder_EnumAdvise(This,ppenumAdvise)	\
    ( (This)->lpVtbl -> EnumAdvise(This,ppenumAdvise) ) 

#define IDataAdviseHolder_SendOnDataChange(This,pDataObject,dwReserved,advf)	\
    ( (This)->lpVtbl -> SendOnDataChange(This,pDataObject,dwReserved,advf) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IDataAdviseHolder_INTERFACE_DEFINED__ */


#ifndef __IMessageFilter_INTERFACE_DEFINED__
#define __IMessageFilter_INTERFACE_DEFINED__

/* interface IMessageFilter */
/* [uuid][object][local] */ 

typedef /* [unique] */ IMessageFilter *LPMESSAGEFILTER;

typedef 
enum tagCALLTYPE
    {	CALLTYPE_TOPLEVEL	= 1,
	CALLTYPE_NESTED	= 2,
	CALLTYPE_ASYNC	= 3,
	CALLTYPE_TOPLEVEL_CALLPENDING	= 4,
	CALLTYPE_ASYNC_CALLPENDING	= 5
    } 	CALLTYPE;

typedef 
enum tagSERVERCALL
    {	SERVERCALL_ISHANDLED	= 0,
	SERVERCALL_REJECTED	= 1,
	SERVERCALL_RETRYLATER	= 2
    } 	SERVERCALL;

typedef 
enum tagPENDINGTYPE
    {	PENDINGTYPE_TOPLEVEL	= 1,
	PENDINGTYPE_NESTED	= 2
    } 	PENDINGTYPE;

typedef 
enum tagPENDINGMSG
    {	PENDINGMSG_CANCELCALL	= 0,
	PENDINGMSG_WAITNOPROCESS	= 1,
	PENDINGMSG_WAITDEFPROCESS	= 2
    } 	PENDINGMSG;

typedef struct tagINTERFACEINFO
    {
    IUnknown *pUnk;
    IID iid;
    WORD wMethod;
    } 	INTERFACEINFO;

typedef struct tagINTERFACEINFO *LPINTERFACEINFO;


EXTERN_C const IID IID_IMessageFilter;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000016-0000-0000-C000-000000000046")
    IMessageFilter : public IUnknown
    {
    public:
        virtual DWORD STDMETHODCALLTYPE HandleInComingCall( 
            /* [annotation][in] */ 
            __in  DWORD dwCallType,
            /* [annotation][in] */ 
            __in  HTASK htaskCaller,
            /* [annotation][in] */ 
            __in  DWORD dwTickCount,
            /* [annotation][in] */ 
            __in_opt  LPINTERFACEINFO lpInterfaceInfo) = 0;
        
        virtual DWORD STDMETHODCALLTYPE RetryRejectedCall( 
            /* [annotation][in] */ 
            __in  HTASK htaskCallee,
            /* [annotation][in] */ 
            __in  DWORD dwTickCount,
            /* [annotation][in] */ 
            __in  DWORD dwRejectType) = 0;
        
        virtual DWORD STDMETHODCALLTYPE MessagePending( 
            /* [annotation][in] */ 
            __in  HTASK htaskCallee,
            /* [annotation][in] */ 
            __in  DWORD dwTickCount,
            /* [annotation][in] */ 
            __in  DWORD dwPendingType) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMessageFilterVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMessageFilter * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMessageFilter * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMessageFilter * This);
        
        DWORD ( STDMETHODCALLTYPE *HandleInComingCall )( 
            IMessageFilter * This,
            /* [annotation][in] */ 
            __in  DWORD dwCallType,
            /* [annotation][in] */ 
            __in  HTASK htaskCaller,
            /* [annotation][in] */ 
            __in  DWORD dwTickCount,
            /* [annotation][in] */ 
            __in_opt  LPINTERFACEINFO lpInterfaceInfo);
        
        DWORD ( STDMETHODCALLTYPE *RetryRejectedCall )( 
            IMessageFilter * This,
            /* [annotation][in] */ 
            __in  HTASK htaskCallee,
            /* [annotation][in] */ 
            __in  DWORD dwTickCount,
            /* [annotation][in] */ 
            __in  DWORD dwRejectType);
        
        DWORD ( STDMETHODCALLTYPE *MessagePending )( 
            IMessageFilter * This,
            /* [annotation][in] */ 
            __in  HTASK htaskCallee,
            /* [annotation][in] */ 
            __in  DWORD dwTickCount,
            /* [annotation][in] */ 
            __in  DWORD dwPendingType);
        
        END_INTERFACE
    } IMessageFilterVtbl;

    interface IMessageFilter
    {
        CONST_VTBL struct IMessageFilterVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMessageFilter_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMessageFilter_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMessageFilter_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMessageFilter_HandleInComingCall(This,dwCallType,htaskCaller,dwTickCount,lpInterfaceInfo)	\
    ( (This)->lpVtbl -> HandleInComingCall(This,dwCallType,htaskCaller,dwTickCount,lpInterfaceInfo) ) 

#define IMessageFilter_RetryRejectedCall(This,htaskCallee,dwTickCount,dwRejectType)	\
    ( (This)->lpVtbl -> RetryRejectedCall(This,htaskCallee,dwTickCount,dwRejectType) ) 

#define IMessageFilter_MessagePending(This,htaskCallee,dwTickCount,dwPendingType)	\
    ( (This)->lpVtbl -> MessagePending(This,htaskCallee,dwTickCount,dwPendingType) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMessageFilter_INTERFACE_DEFINED__ */


#ifndef __IRpcChannelBuffer_INTERFACE_DEFINED__
#define __IRpcChannelBuffer_INTERFACE_DEFINED__

/* interface IRpcChannelBuffer */
/* [uuid][object][local] */ 

typedef unsigned long RPCOLEDATAREP;

typedef struct tagRPCOLEMESSAGE
    {
    void *reserved1;
    RPCOLEDATAREP dataRepresentation;
    void *Buffer;
    ULONG cbBuffer;
    ULONG iMethod;
    void *reserved2[ 5 ];
    ULONG rpcFlags;
    } 	RPCOLEMESSAGE;

typedef RPCOLEMESSAGE *PRPCOLEMESSAGE;


EXTERN_C const IID IID_IRpcChannelBuffer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D5F56B60-593B-101A-B569-08002B2DBF7A")
    IRpcChannelBuffer : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetBuffer( 
            /* [annotation][out][in] */ 
            __inout  RPCOLEMESSAGE *pMessage,
            /* [annotation][in] */ 
            __in  REFIID riid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SendReceive( 
            /* [annotation][out][in] */ 
            __inout  RPCOLEMESSAGE *pMessage,
            /* [annotation][out] */ 
            __out_opt  ULONG *pStatus) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FreeBuffer( 
            /* [annotation][out][in] */ 
            __inout  RPCOLEMESSAGE *pMessage) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDestCtx( 
            /* [annotation][out] */ 
            __out  DWORD *pdwDestContext,
            /* [annotation][out] */ 
            __deref_out_opt  void **ppvDestContext) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsConnected( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IRpcChannelBufferVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IRpcChannelBuffer * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IRpcChannelBuffer * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IRpcChannelBuffer * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetBuffer )( 
            IRpcChannelBuffer * This,
            /* [annotation][out][in] */ 
            __inout  RPCOLEMESSAGE *pMessage,
            /* [annotation][in] */ 
            __in  REFIID riid);
        
        HRESULT ( STDMETHODCALLTYPE *SendReceive )( 
            IRpcChannelBuffer * This,
            /* [annotation][out][in] */ 
            __inout  RPCOLEMESSAGE *pMessage,
            /* [annotation][out] */ 
            __out_opt  ULONG *pStatus);
        
        HRESULT ( STDMETHODCALLTYPE *FreeBuffer )( 
            IRpcChannelBuffer * This,
            /* [annotation][out][in] */ 
            __inout  RPCOLEMESSAGE *pMessage);
        
        HRESULT ( STDMETHODCALLTYPE *GetDestCtx )( 
            IRpcChannelBuffer * This,
            /* [annotation][out] */ 
            __out  DWORD *pdwDestContext,
            /* [annotation][out] */ 
            __deref_out_opt  void **ppvDestContext);
        
        HRESULT ( STDMETHODCALLTYPE *IsConnected )( 
            IRpcChannelBuffer * This);
        
        END_INTERFACE
    } IRpcChannelBufferVtbl;

    interface IRpcChannelBuffer
    {
        CONST_VTBL struct IRpcChannelBufferVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRpcChannelBuffer_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IRpcChannelBuffer_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IRpcChannelBuffer_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IRpcChannelBuffer_GetBuffer(This,pMessage,riid)	\
    ( (This)->lpVtbl -> GetBuffer(This,pMessage,riid) ) 

#define IRpcChannelBuffer_SendReceive(This,pMessage,pStatus)	\
    ( (This)->lpVtbl -> SendReceive(This,pMessage,pStatus) ) 

#define IRpcChannelBuffer_FreeBuffer(This,pMessage)	\
    ( (This)->lpVtbl -> FreeBuffer(This,pMessage) ) 

#define IRpcChannelBuffer_GetDestCtx(This,pdwDestContext,ppvDestContext)	\
    ( (This)->lpVtbl -> GetDestCtx(This,pdwDestContext,ppvDestContext) ) 

#define IRpcChannelBuffer_IsConnected(This)	\
    ( (This)->lpVtbl -> IsConnected(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IRpcChannelBuffer_INTERFACE_DEFINED__ */


#ifndef __IRpcChannelBuffer2_INTERFACE_DEFINED__
#define __IRpcChannelBuffer2_INTERFACE_DEFINED__

/* interface IRpcChannelBuffer2 */
/* [uuid][object][local] */ 


EXTERN_C const IID IID_IRpcChannelBuffer2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("594f31d0-7f19-11d0-b194-00a0c90dc8bf")
    IRpcChannelBuffer2 : public IRpcChannelBuffer
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetProtocolVersion( 
            /* [annotation][out] */ 
            __out  DWORD *pdwVersion) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IRpcChannelBuffer2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IRpcChannelBuffer2 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IRpcChannelBuffer2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IRpcChannelBuffer2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetBuffer )( 
            IRpcChannelBuffer2 * This,
            /* [annotation][out][in] */ 
            __inout  RPCOLEMESSAGE *pMessage,
            /* [annotation][in] */ 
            __in  REFIID riid);
        
        HRESULT ( STDMETHODCALLTYPE *SendReceive )( 
            IRpcChannelBuffer2 * This,
            /* [annotation][out][in] */ 
            __inout  RPCOLEMESSAGE *pMessage,
            /* [annotation][out] */ 
            __out_opt  ULONG *pStatus);
        
        HRESULT ( STDMETHODCALLTYPE *FreeBuffer )( 
            IRpcChannelBuffer2 * This,
            /* [annotation][out][in] */ 
            __inout  RPCOLEMESSAGE *pMessage);
        
        HRESULT ( STDMETHODCALLTYPE *GetDestCtx )( 
            IRpcChannelBuffer2 * This,
            /* [annotation][out] */ 
            __out  DWORD *pdwDestContext,
            /* [annotation][out] */ 
            __deref_out_opt  void **ppvDestContext);
        
        HRESULT ( STDMETHODCALLTYPE *IsConnected )( 
            IRpcChannelBuffer2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetProtocolVersion )( 
            IRpcChannelBuffer2 * This,
            /* [annotation][out] */ 
            __out  DWORD *pdwVersion);
        
        END_INTERFACE
    } IRpcChannelBuffer2Vtbl;

    interface IRpcChannelBuffer2
    {
        CONST_VTBL struct IRpcChannelBuffer2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRpcChannelBuffer2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IRpcChannelBuffer2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IRpcChannelBuffer2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IRpcChannelBuffer2_GetBuffer(This,pMessage,riid)	\
    ( (This)->lpVtbl -> GetBuffer(This,pMessage,riid) ) 

#define IRpcChannelBuffer2_SendReceive(This,pMessage,pStatus)	\
    ( (This)->lpVtbl -> SendReceive(This,pMessage,pStatus) ) 

#define IRpcChannelBuffer2_FreeBuffer(This,pMessage)	\
    ( (This)->lpVtbl -> FreeBuffer(This,pMessage) ) 

#define IRpcChannelBuffer2_GetDestCtx(This,pdwDestContext,ppvDestContext)	\
    ( (This)->lpVtbl -> GetDestCtx(This,pdwDestContext,ppvDestContext) ) 

#define IRpcChannelBuffer2_IsConnected(This)	\
    ( (This)->lpVtbl -> IsConnected(This) ) 


#define IRpcChannelBuffer2_GetProtocolVersion(This,pdwVersion)	\
    ( (This)->lpVtbl -> GetProtocolVersion(This,pdwVersion) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IRpcChannelBuffer2_INTERFACE_DEFINED__ */


#ifndef __IAsyncRpcChannelBuffer_INTERFACE_DEFINED__
#define __IAsyncRpcChannelBuffer_INTERFACE_DEFINED__

/* interface IAsyncRpcChannelBuffer */
/* [unique][uuid][object][local] */ 


EXTERN_C const IID IID_IAsyncRpcChannelBuffer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("a5029fb6-3c34-11d1-9c99-00c04fb998aa")
    IAsyncRpcChannelBuffer : public IRpcChannelBuffer2
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Send( 
            /* [annotation][out][in] */ 
            __inout  RPCOLEMESSAGE *pMsg,
            /* [annotation][in] */ 
            __in  ISynchronize *pSync,
            /* [annotation][out] */ 
            __out  ULONG *pulStatus) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Receive( 
            /* [annotation][out][in] */ 
            __inout  RPCOLEMESSAGE *pMsg,
            /* [annotation][out] */ 
            __out  ULONG *pulStatus) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDestCtxEx( 
            /* [annotation][in] */ 
            __in  RPCOLEMESSAGE *pMsg,
            /* [annotation][out] */ 
            __out  DWORD *pdwDestContext,
            /* [annotation][out] */ 
            __deref_out  void **ppvDestContext) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAsyncRpcChannelBufferVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAsyncRpcChannelBuffer * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAsyncRpcChannelBuffer * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAsyncRpcChannelBuffer * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetBuffer )( 
            IAsyncRpcChannelBuffer * This,
            /* [annotation][out][in] */ 
            __inout  RPCOLEMESSAGE *pMessage,
            /* [annotation][in] */ 
            __in  REFIID riid);
        
        HRESULT ( STDMETHODCALLTYPE *SendReceive )( 
            IAsyncRpcChannelBuffer * This,
            /* [annotation][out][in] */ 
            __inout  RPCOLEMESSAGE *pMessage,
            /* [annotation][out] */ 
            __out_opt  ULONG *pStatus);
        
        HRESULT ( STDMETHODCALLTYPE *FreeBuffer )( 
            IAsyncRpcChannelBuffer * This,
            /* [annotation][out][in] */ 
            __inout  RPCOLEMESSAGE *pMessage);
        
        HRESULT ( STDMETHODCALLTYPE *GetDestCtx )( 
            IAsyncRpcChannelBuffer * This,
            /* [annotation][out] */ 
            __out  DWORD *pdwDestContext,
            /* [annotation][out] */ 
            __deref_out_opt  void **ppvDestContext);
        
        HRESULT ( STDMETHODCALLTYPE *IsConnected )( 
            IAsyncRpcChannelBuffer * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetProtocolVersion )( 
            IAsyncRpcChannelBuffer * This,
            /* [annotation][out] */ 
            __out  DWORD *pdwVersion);
        
        HRESULT ( STDMETHODCALLTYPE *Send )( 
            IAsyncRpcChannelBuffer * This,
            /* [annotation][out][in] */ 
            __inout  RPCOLEMESSAGE *pMsg,
            /* [annotation][in] */ 
            __in  ISynchronize *pSync,
            /* [annotation][out] */ 
            __out  ULONG *pulStatus);
        
        HRESULT ( STDMETHODCALLTYPE *Receive )( 
            IAsyncRpcChannelBuffer * This,
            /* [annotation][out][in] */ 
            __inout  RPCOLEMESSAGE *pMsg,
            /* [annotation][out] */ 
            __out  ULONG *pulStatus);
        
        HRESULT ( STDMETHODCALLTYPE *GetDestCtxEx )( 
            IAsyncRpcChannelBuffer * This,
            /* [annotation][in] */ 
            __in  RPCOLEMESSAGE *pMsg,
            /* [annotation][out] */ 
            __out  DWORD *pdwDestContext,
            /* [annotation][out] */ 
            __deref_out  void **ppvDestContext);
        
        END_INTERFACE
    } IAsyncRpcChannelBufferVtbl;

    interface IAsyncRpcChannelBuffer
    {
        CONST_VTBL struct IAsyncRpcChannelBufferVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAsyncRpcChannelBuffer_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAsyncRpcChannelBuffer_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAsyncRpcChannelBuffer_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAsyncRpcChannelBuffer_GetBuffer(This,pMessage,riid)	\
    ( (This)->lpVtbl -> GetBuffer(This,pMessage,riid) ) 

#define IAsyncRpcChannelBuffer_SendReceive(This,pMessage,pStatus)	\
    ( (This)->lpVtbl -> SendReceive(This,pMessage,pStatus) ) 

#define IAsyncRpcChannelBuffer_FreeBuffer(This,pMessage)	\
    ( (This)->lpVtbl -> FreeBuffer(This,pMessage) ) 

#define IAsyncRpcChannelBuffer_GetDestCtx(This,pdwDestContext,ppvDestContext)	\
    ( (This)->lpVtbl -> GetDestCtx(This,pdwDestContext,ppvDestContext) ) 

#define IAsyncRpcChannelBuffer_IsConnected(This)	\
    ( (This)->lpVtbl -> IsConnected(This) ) 


#define IAsyncRpcChannelBuffer_GetProtocolVersion(This,pdwVersion)	\
    ( (This)->lpVtbl -> GetProtocolVersion(This,pdwVersion) ) 


#define IAsyncRpcChannelBuffer_Send(This,pMsg,pSync,pulStatus)	\
    ( (This)->lpVtbl -> Send(This,pMsg,pSync,pulStatus) ) 

#define IAsyncRpcChannelBuffer_Receive(This,pMsg,pulStatus)	\
    ( (This)->lpVtbl -> Receive(This,pMsg,pulStatus) ) 

#define IAsyncRpcChannelBuffer_GetDestCtxEx(This,pMsg,pdwDestContext,ppvDestContext)	\
    ( (This)->lpVtbl -> GetDestCtxEx(This,pMsg,pdwDestContext,ppvDestContext) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAsyncRpcChannelBuffer_INTERFACE_DEFINED__ */


#ifndef __IRpcChannelBuffer3_INTERFACE_DEFINED__
#define __IRpcChannelBuffer3_INTERFACE_DEFINED__

/* interface IRpcChannelBuffer3 */
/* [uuid][object][local] */ 


EXTERN_C const IID IID_IRpcChannelBuffer3;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("25B15600-0115-11d0-BF0D-00AA00B8DFD2")
    IRpcChannelBuffer3 : public IRpcChannelBuffer2
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Send( 
            /* [annotation][out][in] */ 
            __inout  RPCOLEMESSAGE *pMsg,
            /* [annotation][out] */ 
            __out  ULONG *pulStatus) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Receive( 
            /* [annotation][out][in] */ 
            __inout  RPCOLEMESSAGE *pMsg,
            /* [annotation][in] */ 
            __in  ULONG ulSize,
            /* [annotation][out] */ 
            __out  ULONG *pulStatus) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Cancel( 
            /* [annotation][out][in] */ 
            __inout  RPCOLEMESSAGE *pMsg) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCallContext( 
            /* [annotation][in] */ 
            __in  RPCOLEMESSAGE *pMsg,
            /* [annotation][in] */ 
            __in  REFIID riid,
            /* [annotation][out] */ 
            __deref_out  void **pInterface) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDestCtxEx( 
            /* [annotation][in] */ 
            __in  RPCOLEMESSAGE *pMsg,
            /* [annotation][out] */ 
            __out  DWORD *pdwDestContext,
            /* [annotation][out] */ 
            __deref_out  void **ppvDestContext) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetState( 
            /* [annotation][in] */ 
            __in  RPCOLEMESSAGE *pMsg,
            /* [annotation][out] */ 
            __out  DWORD *pState) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RegisterAsync( 
            /* [annotation][out][in] */ 
            __inout  RPCOLEMESSAGE *pMsg,
            /* [annotation][in] */ 
            __in  IAsyncManager *pAsyncMgr) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IRpcChannelBuffer3Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IRpcChannelBuffer3 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IRpcChannelBuffer3 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IRpcChannelBuffer3 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetBuffer )( 
            IRpcChannelBuffer3 * This,
            /* [annotation][out][in] */ 
            __inout  RPCOLEMESSAGE *pMessage,
            /* [annotation][in] */ 
            __in  REFIID riid);
        
        HRESULT ( STDMETHODCALLTYPE *SendReceive )( 
            IRpcChannelBuffer3 * This,
            /* [annotation][out][in] */ 
            __inout  RPCOLEMESSAGE *pMessage,
            /* [annotation][out] */ 
            __out_opt  ULONG *pStatus);
        
        HRESULT ( STDMETHODCALLTYPE *FreeBuffer )( 
            IRpcChannelBuffer3 * This,
            /* [annotation][out][in] */ 
            __inout  RPCOLEMESSAGE *pMessage);
        
        HRESULT ( STDMETHODCALLTYPE *GetDestCtx )( 
            IRpcChannelBuffer3 * This,
            /* [annotation][out] */ 
            __out  DWORD *pdwDestContext,
            /* [annotation][out] */ 
            __deref_out_opt  void **ppvDestContext);
        
        HRESULT ( STDMETHODCALLTYPE *IsConnected )( 
            IRpcChannelBuffer3 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetProtocolVersion )( 
            IRpcChannelBuffer3 * This,
            /* [annotation][out] */ 
            __out  DWORD *pdwVersion);
        
        HRESULT ( STDMETHODCALLTYPE *Send )( 
            IRpcChannelBuffer3 * This,
            /* [annotation][out][in] */ 
            __inout  RPCOLEMESSAGE *pMsg,
            /* [annotation][out] */ 
            __out  ULONG *pulStatus);
        
        HRESULT ( STDMETHODCALLTYPE *Receive )( 
            IRpcChannelBuffer3 * This,
            /* [annotation][out][in] */ 
            __inout  RPCOLEMESSAGE *pMsg,
            /* [annotation][in] */ 
            __in  ULONG ulSize,
            /* [annotation][out] */ 
            __out  ULONG *pulStatus);
        
        HRESULT ( STDMETHODCALLTYPE *Cancel )( 
            IRpcChannelBuffer3 * This,
            /* [annotation][out][in] */ 
            __inout  RPCOLEMESSAGE *pMsg);
        
        HRESULT ( STDMETHODCALLTYPE *GetCallContext )( 
            IRpcChannelBuffer3 * This,
            /* [annotation][in] */ 
            __in  RPCOLEMESSAGE *pMsg,
            /* [annotation][in] */ 
            __in  REFIID riid,
            /* [annotation][out] */ 
            __deref_out  void **pInterface);
        
        HRESULT ( STDMETHODCALLTYPE *GetDestCtxEx )( 
            IRpcChannelBuffer3 * This,
            /* [annotation][in] */ 
            __in  RPCOLEMESSAGE *pMsg,
            /* [annotation][out] */ 
            __out  DWORD *pdwDestContext,
            /* [annotation][out] */ 
            __deref_out  void **ppvDestContext);
        
        HRESULT ( STDMETHODCALLTYPE *GetState )( 
            IRpcChannelBuffer3 * This,
            /* [annotation][in] */ 
            __in  RPCOLEMESSAGE *pMsg,
            /* [annotation][out] */ 
            __out  DWORD *pState);
        
        HRESULT ( STDMETHODCALLTYPE *RegisterAsync )( 
            IRpcChannelBuffer3 * This,
            /* [annotation][out][in] */ 
            __inout  RPCOLEMESSAGE *pMsg,
            /* [annotation][in] */ 
            __in  IAsyncManager *pAsyncMgr);
        
        END_INTERFACE
    } IRpcChannelBuffer3Vtbl;

    interface IRpcChannelBuffer3
    {
        CONST_VTBL struct IRpcChannelBuffer3Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRpcChannelBuffer3_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IRpcChannelBuffer3_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IRpcChannelBuffer3_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IRpcChannelBuffer3_GetBuffer(This,pMessage,riid)	\
    ( (This)->lpVtbl -> GetBuffer(This,pMessage,riid) ) 

#define IRpcChannelBuffer3_SendReceive(This,pMessage,pStatus)	\
    ( (This)->lpVtbl -> SendReceive(This,pMessage,pStatus) ) 

#define IRpcChannelBuffer3_FreeBuffer(This,pMessage)	\
    ( (This)->lpVtbl -> FreeBuffer(This,pMessage) ) 

#define IRpcChannelBuffer3_GetDestCtx(This,pdwDestContext,ppvDestContext)	\
    ( (This)->lpVtbl -> GetDestCtx(This,pdwDestContext,ppvDestContext) ) 

#define IRpcChannelBuffer3_IsConnected(This)	\
    ( (This)->lpVtbl -> IsConnected(This) ) 


#define IRpcChannelBuffer3_GetProtocolVersion(This,pdwVersion)	\
    ( (This)->lpVtbl -> GetProtocolVersion(This,pdwVersion) ) 


#define IRpcChannelBuffer3_Send(This,pMsg,pulStatus)	\
    ( (This)->lpVtbl -> Send(This,pMsg,pulStatus) ) 

#define IRpcChannelBuffer3_Receive(This,pMsg,ulSize,pulStatus)	\
    ( (This)->lpVtbl -> Receive(This,pMsg,ulSize,pulStatus) ) 

#define IRpcChannelBuffer3_Cancel(This,pMsg)	\
    ( (This)->lpVtbl -> Cancel(This,pMsg) ) 

#define IRpcChannelBuffer3_GetCallContext(This,pMsg,riid,pInterface)	\
    ( (This)->lpVtbl -> GetCallContext(This,pMsg,riid,pInterface) ) 

#define IRpcChannelBuffer3_GetDestCtxEx(This,pMsg,pdwDestContext,ppvDestContext)	\
    ( (This)->lpVtbl -> GetDestCtxEx(This,pMsg,pdwDestContext,ppvDestContext) ) 

#define IRpcChannelBuffer3_GetState(This,pMsg,pState)	\
    ( (This)->lpVtbl -> GetState(This,pMsg,pState) ) 

#define IRpcChannelBuffer3_RegisterAsync(This,pMsg,pAsyncMgr)	\
    ( (This)->lpVtbl -> RegisterAsync(This,pMsg,pAsyncMgr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IRpcChannelBuffer3_INTERFACE_DEFINED__ */


#ifndef __IRpcSyntaxNegotiate_INTERFACE_DEFINED__
#define __IRpcSyntaxNegotiate_INTERFACE_DEFINED__

/* interface IRpcSyntaxNegotiate */
/* [uuid][object][local] */ 


EXTERN_C const IID IID_IRpcSyntaxNegotiate;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("58a08519-24c8-4935-b482-3fd823333a4f")
    IRpcSyntaxNegotiate : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE NegotiateSyntax( 
            /* [annotation][out][in] */ 
            __inout  RPCOLEMESSAGE *pMsg) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IRpcSyntaxNegotiateVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IRpcSyntaxNegotiate * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IRpcSyntaxNegotiate * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IRpcSyntaxNegotiate * This);
        
        HRESULT ( STDMETHODCALLTYPE *NegotiateSyntax )( 
            IRpcSyntaxNegotiate * This,
            /* [annotation][out][in] */ 
            __inout  RPCOLEMESSAGE *pMsg);
        
        END_INTERFACE
    } IRpcSyntaxNegotiateVtbl;

    interface IRpcSyntaxNegotiate
    {
        CONST_VTBL struct IRpcSyntaxNegotiateVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRpcSyntaxNegotiate_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IRpcSyntaxNegotiate_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IRpcSyntaxNegotiate_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IRpcSyntaxNegotiate_NegotiateSyntax(This,pMsg)	\
    ( (This)->lpVtbl -> NegotiateSyntax(This,pMsg) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IRpcSyntaxNegotiate_INTERFACE_DEFINED__ */


#ifndef __IRpcProxyBuffer_INTERFACE_DEFINED__
#define __IRpcProxyBuffer_INTERFACE_DEFINED__

/* interface IRpcProxyBuffer */
/* [uuid][object][local] */ 


EXTERN_C const IID IID_IRpcProxyBuffer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D5F56A34-593B-101A-B569-08002B2DBF7A")
    IRpcProxyBuffer : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Connect( 
            /* [annotation][unique][in] */ 
            __in  IRpcChannelBuffer *pRpcChannelBuffer) = 0;
        
        virtual void STDMETHODCALLTYPE Disconnect( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IRpcProxyBufferVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IRpcProxyBuffer * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IRpcProxyBuffer * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IRpcProxyBuffer * This);
        
        HRESULT ( STDMETHODCALLTYPE *Connect )( 
            IRpcProxyBuffer * This,
            /* [annotation][unique][in] */ 
            __in  IRpcChannelBuffer *pRpcChannelBuffer);
        
        void ( STDMETHODCALLTYPE *Disconnect )( 
            IRpcProxyBuffer * This);
        
        END_INTERFACE
    } IRpcProxyBufferVtbl;

    interface IRpcProxyBuffer
    {
        CONST_VTBL struct IRpcProxyBufferVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRpcProxyBuffer_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IRpcProxyBuffer_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IRpcProxyBuffer_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IRpcProxyBuffer_Connect(This,pRpcChannelBuffer)	\
    ( (This)->lpVtbl -> Connect(This,pRpcChannelBuffer) ) 

#define IRpcProxyBuffer_Disconnect(This)	\
    ( (This)->lpVtbl -> Disconnect(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IRpcProxyBuffer_INTERFACE_DEFINED__ */


#ifndef __IRpcStubBuffer_INTERFACE_DEFINED__
#define __IRpcStubBuffer_INTERFACE_DEFINED__

/* interface IRpcStubBuffer */
/* [uuid][object][local] */ 


EXTERN_C const IID IID_IRpcStubBuffer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D5F56AFC-593B-101A-B569-08002B2DBF7A")
    IRpcStubBuffer : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Connect( 
            /* [annotation][in] */ 
            __in  IUnknown *pUnkServer) = 0;
        
        virtual void STDMETHODCALLTYPE Disconnect( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Invoke( 
            /* [annotation][out][in] */ 
            __inout  RPCOLEMESSAGE *_prpcmsg,
            /* [annotation][in] */ 
            __in  IRpcChannelBuffer *_pRpcChannelBuffer) = 0;
        
        virtual IRpcStubBuffer *STDMETHODCALLTYPE IsIIDSupported( 
            /* [annotation][in] */ 
            __in  REFIID riid) = 0;
        
        virtual ULONG STDMETHODCALLTYPE CountRefs( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DebugServerQueryInterface( 
            /* [annotation][out] */ 
            __deref_out  void **ppv) = 0;
        
        virtual void STDMETHODCALLTYPE DebugServerRelease( 
            /* [annotation][in] */ 
            __in  void *pv) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IRpcStubBufferVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IRpcStubBuffer * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IRpcStubBuffer * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IRpcStubBuffer * This);
        
        HRESULT ( STDMETHODCALLTYPE *Connect )( 
            IRpcStubBuffer * This,
            /* [annotation][in] */ 
            __in  IUnknown *pUnkServer);
        
        void ( STDMETHODCALLTYPE *Disconnect )( 
            IRpcStubBuffer * This);
        
        HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IRpcStubBuffer * This,
            /* [annotation][out][in] */ 
            __inout  RPCOLEMESSAGE *_prpcmsg,
            /* [annotation][in] */ 
            __in  IRpcChannelBuffer *_pRpcChannelBuffer);
        
        IRpcStubBuffer *( STDMETHODCALLTYPE *IsIIDSupported )( 
            IRpcStubBuffer * This,
            /* [annotation][in] */ 
            __in  REFIID riid);
        
        ULONG ( STDMETHODCALLTYPE *CountRefs )( 
            IRpcStubBuffer * This);
        
        HRESULT ( STDMETHODCALLTYPE *DebugServerQueryInterface )( 
            IRpcStubBuffer * This,
            /* [annotation][out] */ 
            __deref_out  void **ppv);
        
        void ( STDMETHODCALLTYPE *DebugServerRelease )( 
            IRpcStubBuffer * This,
            /* [annotation][in] */ 
            __in  void *pv);
        
        END_INTERFACE
    } IRpcStubBufferVtbl;

    interface IRpcStubBuffer
    {
        CONST_VTBL struct IRpcStubBufferVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRpcStubBuffer_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IRpcStubBuffer_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IRpcStubBuffer_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IRpcStubBuffer_Connect(This,pUnkServer)	\
    ( (This)->lpVtbl -> Connect(This,pUnkServer) ) 

#define IRpcStubBuffer_Disconnect(This)	\
    ( (This)->lpVtbl -> Disconnect(This) ) 

#define IRpcStubBuffer_Invoke(This,_prpcmsg,_pRpcChannelBuffer)	\
    ( (This)->lpVtbl -> Invoke(This,_prpcmsg,_pRpcChannelBuffer) ) 

#define IRpcStubBuffer_IsIIDSupported(This,riid)	\
    ( (This)->lpVtbl -> IsIIDSupported(This,riid) ) 

#define IRpcStubBuffer_CountRefs(This)	\
    ( (This)->lpVtbl -> CountRefs(This) ) 

#define IRpcStubBuffer_DebugServerQueryInterface(This,ppv)	\
    ( (This)->lpVtbl -> DebugServerQueryInterface(This,ppv) ) 

#define IRpcStubBuffer_DebugServerRelease(This,pv)	\
    ( (This)->lpVtbl -> DebugServerRelease(This,pv) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IRpcStubBuffer_INTERFACE_DEFINED__ */


#ifndef __IPSFactoryBuffer_INTERFACE_DEFINED__
#define __IPSFactoryBuffer_INTERFACE_DEFINED__

/* interface IPSFactoryBuffer */
/* [uuid][object][local] */ 


EXTERN_C const IID IID_IPSFactoryBuffer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D5F569D0-593B-101A-B569-08002B2DBF7A")
    IPSFactoryBuffer : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CreateProxy( 
            /* [annotation][in] */ 
            __in  IUnknown *pUnkOuter,
            /* [annotation][in] */ 
            __in  REFIID riid,
            /* [annotation][out] */ 
            __deref_out  IRpcProxyBuffer **ppProxy,
            /* [annotation][out] */ 
            __deref_out  void **ppv) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateStub( 
            /* [annotation][in] */ 
            __in  REFIID riid,
            /* [annotation][unique][in] */ 
            __in_opt  IUnknown *pUnkServer,
            /* [annotation][out] */ 
            __deref_out  IRpcStubBuffer **ppStub) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPSFactoryBufferVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPSFactoryBuffer * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPSFactoryBuffer * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPSFactoryBuffer * This);
        
        HRESULT ( STDMETHODCALLTYPE *CreateProxy )( 
            IPSFactoryBuffer * This,
            /* [annotation][in] */ 
            __in  IUnknown *pUnkOuter,
            /* [annotation][in] */ 
            __in  REFIID riid,
            /* [annotation][out] */ 
            __deref_out  IRpcProxyBuffer **ppProxy,
            /* [annotation][out] */ 
            __deref_out  void **ppv);
        
        HRESULT ( STDMETHODCALLTYPE *CreateStub )( 
            IPSFactoryBuffer * This,
            /* [annotation][in] */ 
            __in  REFIID riid,
            /* [annotation][unique][in] */ 
            __in_opt  IUnknown *pUnkServer,
            /* [annotation][out] */ 
            __deref_out  IRpcStubBuffer **ppStub);
        
        END_INTERFACE
    } IPSFactoryBufferVtbl;

    interface IPSFactoryBuffer
    {
        CONST_VTBL struct IPSFactoryBufferVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPSFactoryBuffer_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IPSFactoryBuffer_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IPSFactoryBuffer_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IPSFactoryBuffer_CreateProxy(This,pUnkOuter,riid,ppProxy,ppv)	\
    ( (This)->lpVtbl -> CreateProxy(This,pUnkOuter,riid,ppProxy,ppv) ) 

#define IPSFactoryBuffer_CreateStub(This,riid,pUnkServer,ppStub)	\
    ( (This)->lpVtbl -> CreateStub(This,riid,pUnkServer,ppStub) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IPSFactoryBuffer_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_objidl_0000_0041 */
/* [local] */ 

#if  (_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM) // DCOM
// This interface is only valid on Windows NT 4.0
typedef struct SChannelHookCallInfo
    {
    IID iid;
    DWORD cbSize;
    GUID uCausality;
    DWORD dwServerPid;
    DWORD iMethod;
    void *pObject;
    } 	SChannelHookCallInfo;



extern RPC_IF_HANDLE __MIDL_itf_objidl_0000_0041_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_objidl_0000_0041_v0_0_s_ifspec;

#ifndef __IChannelHook_INTERFACE_DEFINED__
#define __IChannelHook_INTERFACE_DEFINED__

/* interface IChannelHook */
/* [uuid][object][local] */ 


EXTERN_C const IID IID_IChannelHook;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1008c4a0-7613-11cf-9af1-0020af6e72f4")
    IChannelHook : public IUnknown
    {
    public:
        virtual void STDMETHODCALLTYPE ClientGetSize( 
            /* [annotation][in] */ 
            __in  REFGUID uExtent,
            /* [annotation][in] */ 
            __in  REFIID riid,
            /* [annotation][out] */ 
            __out  ULONG *pDataSize) = 0;
        
        virtual void STDMETHODCALLTYPE ClientFillBuffer( 
            /* [annotation][in] */ 
            __in  REFGUID uExtent,
            /* [annotation][in] */ 
            __in  REFIID riid,
            /* [annotation][out][in] */ 
            __inout  ULONG *pDataSize,
            /* [annotation][in] */ 
            __in  void *pDataBuffer) = 0;
        
        virtual void STDMETHODCALLTYPE ClientNotify( 
            /* [annotation][in] */ 
            __in  REFGUID uExtent,
            /* [annotation][in] */ 
            __in  REFIID riid,
            /* [annotation][in] */ 
            __in  ULONG cbDataSize,
            /* [annotation][in] */ 
            __in  void *pDataBuffer,
            /* [annotation][in] */ 
            __in  DWORD lDataRep,
            /* [annotation][in] */ 
            __in  HRESULT hrFault) = 0;
        
        virtual void STDMETHODCALLTYPE ServerNotify( 
            /* [annotation][in] */ 
            __in  REFGUID uExtent,
            /* [annotation][in] */ 
            __in  REFIID riid,
            /* [annotation][in] */ 
            __in  ULONG cbDataSize,
            /* [annotation][in] */ 
            __in  void *pDataBuffer,
            /* [annotation][in] */ 
            __in  DWORD lDataRep) = 0;
        
        virtual void STDMETHODCALLTYPE ServerGetSize( 
            /* [annotation][in] */ 
            __in  REFGUID uExtent,
            /* [annotation][in] */ 
            __in  REFIID riid,
            /* [annotation][in] */ 
            __in  HRESULT hrFault,
            /* [annotation][out] */ 
            __out  ULONG *pDataSize) = 0;
        
        virtual void STDMETHODCALLTYPE ServerFillBuffer( 
            /* [annotation][in] */ 
            __in  REFGUID uExtent,
            /* [annotation][in] */ 
            __in  REFIID riid,
            /* [annotation][out][in] */ 
            __inout  ULONG *pDataSize,
            /* [annotation][in] */ 
            __in  void *pDataBuffer,
            /* [annotation][in] */ 
            __in  HRESULT hrFault) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IChannelHookVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IChannelHook * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IChannelHook * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IChannelHook * This);
        
        void ( STDMETHODCALLTYPE *ClientGetSize )( 
            IChannelHook * This,
            /* [annotation][in] */ 
            __in  REFGUID uExtent,
            /* [annotation][in] */ 
            __in  REFIID riid,
            /* [annotation][out] */ 
            __out  ULONG *pDataSize);
        
        void ( STDMETHODCALLTYPE *ClientFillBuffer )( 
            IChannelHook * This,
            /* [annotation][in] */ 
            __in  REFGUID uExtent,
            /* [annotation][in] */ 
            __in  REFIID riid,
            /* [annotation][out][in] */ 
            __inout  ULONG *pDataSize,
            /* [annotation][in] */ 
            __in  void *pDataBuffer);
        
        void ( STDMETHODCALLTYPE *ClientNotify )( 
            IChannelHook * This,
            /* [annotation][in] */ 
            __in  REFGUID uExtent,
            /* [annotation][in] */ 
            __in  REFIID riid,
            /* [annotation][in] */ 
            __in  ULONG cbDataSize,
            /* [annotation][in] */ 
            __in  void *pDataBuffer,
            /* [annotation][in] */ 
            __in  DWORD lDataRep,
            /* [annotation][in] */ 
            __in  HRESULT hrFault);
        
        void ( STDMETHODCALLTYPE *ServerNotify )( 
            IChannelHook * This,
            /* [annotation][in] */ 
            __in  REFGUID uExtent,
            /* [annotation][in] */ 
            __in  REFIID riid,
            /* [annotation][in] */ 
            __in  ULONG cbDataSize,
            /* [annotation][in] */ 
            __in  void *pDataBuffer,
            /* [annotation][in] */ 
            __in  DWORD lDataRep);
        
        void ( STDMETHODCALLTYPE *ServerGetSize )( 
            IChannelHook * This,
            /* [annotation][in] */ 
            __in  REFGUID uExtent,
            /* [annotation][in] */ 
            __in  REFIID riid,
            /* [annotation][in] */ 
            __in  HRESULT hrFault,
            /* [annotation][out] */ 
            __out  ULONG *pDataSize);
        
        void ( STDMETHODCALLTYPE *ServerFillBuffer )( 
            IChannelHook * This,
            /* [annotation][in] */ 
            __in  REFGUID uExtent,
            /* [annotation][in] */ 
            __in  REFIID riid,
            /* [annotation][out][in] */ 
            __inout  ULONG *pDataSize,
            /* [annotation][in] */ 
            __in  void *pDataBuffer,
            /* [annotation][in] */ 
            __in  HRESULT hrFault);
        
        END_INTERFACE
    } IChannelHookVtbl;

    interface IChannelHook
    {
        CONST_VTBL struct IChannelHookVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IChannelHook_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IChannelHook_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IChannelHook_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IChannelHook_ClientGetSize(This,uExtent,riid,pDataSize)	\
    ( (This)->lpVtbl -> ClientGetSize(This,uExtent,riid,pDataSize) ) 

#define IChannelHook_ClientFillBuffer(This,uExtent,riid,pDataSize,pDataBuffer)	\
    ( (This)->lpVtbl -> ClientFillBuffer(This,uExtent,riid,pDataSize,pDataBuffer) ) 

#define IChannelHook_ClientNotify(This,uExtent,riid,cbDataSize,pDataBuffer,lDataRep,hrFault)	\
    ( (This)->lpVtbl -> ClientNotify(This,uExtent,riid,cbDataSize,pDataBuffer,lDataRep,hrFault) ) 

#define IChannelHook_ServerNotify(This,uExtent,riid,cbDataSize,pDataBuffer,lDataRep)	\
    ( (This)->lpVtbl -> ServerNotify(This,uExtent,riid,cbDataSize,pDataBuffer,lDataRep) ) 

#define IChannelHook_ServerGetSize(This,uExtent,riid,hrFault,pDataSize)	\
    ( (This)->lpVtbl -> ServerGetSize(This,uExtent,riid,hrFault,pDataSize) ) 

#define IChannelHook_ServerFillBuffer(This,uExtent,riid,pDataSize,pDataBuffer,hrFault)	\
    ( (This)->lpVtbl -> ServerFillBuffer(This,uExtent,riid,pDataSize,pDataBuffer,hrFault) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IChannelHook_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_objidl_0000_0042 */
/* [local] */ 

#endif //DCOM

// Well-known Property Set Format IDs
extern const FMTID FMTID_SummaryInformation;

extern const FMTID FMTID_DocSummaryInformation;

extern const FMTID FMTID_UserDefinedProperties;

extern const FMTID FMTID_DiscardableInformation;

extern const FMTID FMTID_ImageSummaryInformation;

extern const FMTID FMTID_AudioSummaryInformation;

extern const FMTID FMTID_VideoSummaryInformation;

extern const FMTID FMTID_MediaFileSummaryInformation;

#if  (_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM) // DCOM
// This interface is only valid on Windows NT 4.0


extern RPC_IF_HANDLE __MIDL_itf_objidl_0000_0042_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_objidl_0000_0042_v0_0_s_ifspec;

#ifndef __IClientSecurity_INTERFACE_DEFINED__
#define __IClientSecurity_INTERFACE_DEFINED__

/* interface IClientSecurity */
/* [uuid][object][local] */ 

typedef struct tagSOLE_AUTHENTICATION_SERVICE
    {
    DWORD dwAuthnSvc;
    DWORD dwAuthzSvc;
    OLECHAR *pPrincipalName;
    HRESULT hr;
    } 	SOLE_AUTHENTICATION_SERVICE;

typedef SOLE_AUTHENTICATION_SERVICE *PSOLE_AUTHENTICATION_SERVICE;

typedef 
enum tagEOLE_AUTHENTICATION_CAPABILITIES
    {	EOAC_NONE	= 0,
	EOAC_MUTUAL_AUTH	= 0x1,
	EOAC_STATIC_CLOAKING	= 0x20,
	EOAC_DYNAMIC_CLOAKING	= 0x40,
	EOAC_ANY_AUTHORITY	= 0x80,
	EOAC_MAKE_FULLSIC	= 0x100,
	EOAC_DEFAULT	= 0x800,
	EOAC_SECURE_REFS	= 0x2,
	EOAC_ACCESS_CONTROL	= 0x4,
	EOAC_APPID	= 0x8,
	EOAC_DYNAMIC	= 0x10,
	EOAC_REQUIRE_FULLSIC	= 0x200,
	EOAC_AUTO_IMPERSONATE	= 0x400,
	EOAC_NO_CUSTOM_MARSHAL	= 0x2000,
	EOAC_DISABLE_AAA	= 0x1000
    } 	EOLE_AUTHENTICATION_CAPABILITIES;

#define	COLE_DEFAULT_PRINCIPAL	( ( OLECHAR * )-1 )

#define	COLE_DEFAULT_AUTHINFO	( ( void * )-1 )

typedef struct tagSOLE_AUTHENTICATION_INFO
    {
    DWORD dwAuthnSvc;
    DWORD dwAuthzSvc;
    void *pAuthInfo;
    } 	SOLE_AUTHENTICATION_INFO;

typedef struct tagSOLE_AUTHENTICATION_INFO *PSOLE_AUTHENTICATION_INFO;

typedef struct tagSOLE_AUTHENTICATION_LIST
    {
    DWORD cAuthInfo;
    SOLE_AUTHENTICATION_INFO *aAuthInfo;
    } 	SOLE_AUTHENTICATION_LIST;

typedef struct tagSOLE_AUTHENTICATION_LIST *PSOLE_AUTHENTICATION_LIST;


EXTERN_C const IID IID_IClientSecurity;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0000013D-0000-0000-C000-000000000046")
    IClientSecurity : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE QueryBlanket( 
            /* [annotation][in] */ 
            __in  IUnknown *pProxy,
            /* [annotation][out] */ 
            __out  DWORD *pAuthnSvc,
            /* [annotation][out] */ 
            __out_opt  DWORD *pAuthzSvc,
            /* [annotation][out] */ 
            __RPC__deref_out_opt  OLECHAR **pServerPrincName,
            /* [annotation][out] */ 
            __out_opt  DWORD *pAuthnLevel,
            /* [annotation][out] */ 
            __out_opt  DWORD *pImpLevel,
            /* [annotation][out] */ 
            __deref_out_opt  void **pAuthInfo,
            /* [annotation][out] */ 
            __out_opt  DWORD *pCapabilites) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetBlanket( 
            /* [annotation][in] */ 
            __in  IUnknown *pProxy,
            /* [annotation][in] */ 
            __in  DWORD dwAuthnSvc,
            /* [annotation][in] */ 
            __in  DWORD dwAuthzSvc,
            /* [annotation][in] */ 
            __RPC__in_opt  OLECHAR *pServerPrincName,
            /* [annotation][in] */ 
            __in  DWORD dwAuthnLevel,
            /* [annotation][in] */ 
            __in  DWORD dwImpLevel,
            /* [annotation][in] */ 
            __in_opt  void *pAuthInfo,
            /* [annotation][in] */ 
            __in  DWORD dwCapabilities) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CopyProxy( 
            /* [annotation][in] */ 
            __in  IUnknown *pProxy,
            /* [annotation][out] */ 
            __deref_out  IUnknown **ppCopy) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IClientSecurityVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IClientSecurity * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IClientSecurity * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IClientSecurity * This);
        
        HRESULT ( STDMETHODCALLTYPE *QueryBlanket )( 
            IClientSecurity * This,
            /* [annotation][in] */ 
            __in  IUnknown *pProxy,
            /* [annotation][out] */ 
            __out  DWORD *pAuthnSvc,
            /* [annotation][out] */ 
            __out_opt  DWORD *pAuthzSvc,
            /* [annotation][out] */ 
            __RPC__deref_out_opt  OLECHAR **pServerPrincName,
            /* [annotation][out] */ 
            __out_opt  DWORD *pAuthnLevel,
            /* [annotation][out] */ 
            __out_opt  DWORD *pImpLevel,
            /* [annotation][out] */ 
            __deref_out_opt  void **pAuthInfo,
            /* [annotation][out] */ 
            __out_opt  DWORD *pCapabilites);
        
        HRESULT ( STDMETHODCALLTYPE *SetBlanket )( 
            IClientSecurity * This,
            /* [annotation][in] */ 
            __in  IUnknown *pProxy,
            /* [annotation][in] */ 
            __in  DWORD dwAuthnSvc,
            /* [annotation][in] */ 
            __in  DWORD dwAuthzSvc,
            /* [annotation][in] */ 
            __RPC__in_opt  OLECHAR *pServerPrincName,
            /* [annotation][in] */ 
            __in  DWORD dwAuthnLevel,
            /* [annotation][in] */ 
            __in  DWORD dwImpLevel,
            /* [annotation][in] */ 
            __in_opt  void *pAuthInfo,
            /* [annotation][in] */ 
            __in  DWORD dwCapabilities);
        
        HRESULT ( STDMETHODCALLTYPE *CopyProxy )( 
            IClientSecurity * This,
            /* [annotation][in] */ 
            __in  IUnknown *pProxy,
            /* [annotation][out] */ 
            __deref_out  IUnknown **ppCopy);
        
        END_INTERFACE
    } IClientSecurityVtbl;

    interface IClientSecurity
    {
        CONST_VTBL struct IClientSecurityVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IClientSecurity_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IClientSecurity_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IClientSecurity_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IClientSecurity_QueryBlanket(This,pProxy,pAuthnSvc,pAuthzSvc,pServerPrincName,pAuthnLevel,pImpLevel,pAuthInfo,pCapabilites)	\
    ( (This)->lpVtbl -> QueryBlanket(This,pProxy,pAuthnSvc,pAuthzSvc,pServerPrincName,pAuthnLevel,pImpLevel,pAuthInfo,pCapabilites) ) 

#define IClientSecurity_SetBlanket(This,pProxy,dwAuthnSvc,dwAuthzSvc,pServerPrincName,dwAuthnLevel,dwImpLevel,pAuthInfo,dwCapabilities)	\
    ( (This)->lpVtbl -> SetBlanket(This,pProxy,dwAuthnSvc,dwAuthzSvc,pServerPrincName,dwAuthnLevel,dwImpLevel,pAuthInfo,dwCapabilities) ) 

#define IClientSecurity_CopyProxy(This,pProxy,ppCopy)	\
    ( (This)->lpVtbl -> CopyProxy(This,pProxy,ppCopy) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IClientSecurity_INTERFACE_DEFINED__ */


#ifndef __IServerSecurity_INTERFACE_DEFINED__
#define __IServerSecurity_INTERFACE_DEFINED__

/* interface IServerSecurity */
/* [uuid][object][local] */ 


EXTERN_C const IID IID_IServerSecurity;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0000013E-0000-0000-C000-000000000046")
    IServerSecurity : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE QueryBlanket( 
            /* [annotation][out] */ 
            __out_opt  DWORD *pAuthnSvc,
            /* [annotation][out] */ 
            __out_opt  DWORD *pAuthzSvc,
            /* [annotation][out] */ 
            __RPC__deref_out_opt  OLECHAR **pServerPrincName,
            /* [annotation][out] */ 
            __out_opt  DWORD *pAuthnLevel,
            /* [annotation][out] */ 
            __out_opt  DWORD *pImpLevel,
            /* [annotation][out] */ 
            __deref_out_opt  void **pPrivs,
            /* [annotation][out][in] */ 
            __inout_opt  DWORD *pCapabilities) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ImpersonateClient( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RevertToSelf( void) = 0;
        
        virtual BOOL STDMETHODCALLTYPE IsImpersonating( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IServerSecurityVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IServerSecurity * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IServerSecurity * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IServerSecurity * This);
        
        HRESULT ( STDMETHODCALLTYPE *QueryBlanket )( 
            IServerSecurity * This,
            /* [annotation][out] */ 
            __out_opt  DWORD *pAuthnSvc,
            /* [annotation][out] */ 
            __out_opt  DWORD *pAuthzSvc,
            /* [annotation][out] */ 
            __RPC__deref_out_opt  OLECHAR **pServerPrincName,
            /* [annotation][out] */ 
            __out_opt  DWORD *pAuthnLevel,
            /* [annotation][out] */ 
            __out_opt  DWORD *pImpLevel,
            /* [annotation][out] */ 
            __deref_out_opt  void **pPrivs,
            /* [annotation][out][in] */ 
            __inout_opt  DWORD *pCapabilities);
        
        HRESULT ( STDMETHODCALLTYPE *ImpersonateClient )( 
            IServerSecurity * This);
        
        HRESULT ( STDMETHODCALLTYPE *RevertToSelf )( 
            IServerSecurity * This);
        
        BOOL ( STDMETHODCALLTYPE *IsImpersonating )( 
            IServerSecurity * This);
        
        END_INTERFACE
    } IServerSecurityVtbl;

    interface IServerSecurity
    {
        CONST_VTBL struct IServerSecurityVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IServerSecurity_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IServerSecurity_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IServerSecurity_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IServerSecurity_QueryBlanket(This,pAuthnSvc,pAuthzSvc,pServerPrincName,pAuthnLevel,pImpLevel,pPrivs,pCapabilities)	\
    ( (This)->lpVtbl -> QueryBlanket(This,pAuthnSvc,pAuthzSvc,pServerPrincName,pAuthnLevel,pImpLevel,pPrivs,pCapabilities) ) 

#define IServerSecurity_ImpersonateClient(This)	\
    ( (This)->lpVtbl -> ImpersonateClient(This) ) 

#define IServerSecurity_RevertToSelf(This)	\
    ( (This)->lpVtbl -> RevertToSelf(This) ) 

#define IServerSecurity_IsImpersonating(This)	\
    ( (This)->lpVtbl -> IsImpersonating(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IServerSecurity_INTERFACE_DEFINED__ */


#ifndef __IClassActivator_INTERFACE_DEFINED__
#define __IClassActivator_INTERFACE_DEFINED__

/* interface IClassActivator */
/* [uuid][object] */ 


EXTERN_C const IID IID_IClassActivator;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000140-0000-0000-C000-000000000046")
    IClassActivator : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetClassObject( 
            /* [in] */ __RPC__in REFCLSID rclsid,
            /* [in] */ DWORD dwClassContext,
            /* [in] */ LCID locale,
            /* [in] */ __RPC__in REFIID riid,
            /* [iid_is][out] */ __RPC__deref_out_opt void **ppv) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IClassActivatorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IClassActivator * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IClassActivator * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IClassActivator * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetClassObject )( 
            __RPC__in IClassActivator * This,
            /* [in] */ __RPC__in REFCLSID rclsid,
            /* [in] */ DWORD dwClassContext,
            /* [in] */ LCID locale,
            /* [in] */ __RPC__in REFIID riid,
            /* [iid_is][out] */ __RPC__deref_out_opt void **ppv);
        
        END_INTERFACE
    } IClassActivatorVtbl;

    interface IClassActivator
    {
        CONST_VTBL struct IClassActivatorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IClassActivator_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IClassActivator_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IClassActivator_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IClassActivator_GetClassObject(This,rclsid,dwClassContext,locale,riid,ppv)	\
    ( (This)->lpVtbl -> GetClassObject(This,rclsid,dwClassContext,locale,riid,ppv) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IClassActivator_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_objidl_0000_0045 */
/* [local] */ 

typedef 
enum tagRPCOPT_PROPERTIES
    {	COMBND_RPCTIMEOUT	= 0x1,
	COMBND_SERVER_LOCALITY	= 0x2
    } 	RPCOPT_PROPERTIES;

typedef 
enum tagRPCOPT_SERVER_LOCALITY_VALUES
    {	SERVER_LOCALITY_PROCESS_LOCAL	= 0,
	SERVER_LOCALITY_MACHINE_LOCAL	= 1,
	SERVER_LOCALITY_REMOTE	= 2
    } 	RPCOPT_SERVER_LOCALITY_VALUES;



extern RPC_IF_HANDLE __MIDL_itf_objidl_0000_0045_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_objidl_0000_0045_v0_0_s_ifspec;

#ifndef __IRpcOptions_INTERFACE_DEFINED__
#define __IRpcOptions_INTERFACE_DEFINED__

/* interface IRpcOptions */
/* [uuid][local][object] */ 


EXTERN_C const IID IID_IRpcOptions;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000144-0000-0000-C000-000000000046")
    IRpcOptions : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Set( 
            /* [annotation][in] */ 
            __in  IUnknown *pPrx,
            /* [annotation][in] */ 
            __in  RPCOPT_PROPERTIES dwProperty,
            /* [annotation][in] */ 
            __in  ULONG_PTR dwValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Query( 
            /* [annotation][in] */ 
            __in  IUnknown *pPrx,
            /* [annotation][in] */ 
            __in  RPCOPT_PROPERTIES dwProperty,
            /* [annotation][out] */ 
            __deref_out  ULONG_PTR *pdwValue) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IRpcOptionsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IRpcOptions * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IRpcOptions * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IRpcOptions * This);
        
        HRESULT ( STDMETHODCALLTYPE *Set )( 
            IRpcOptions * This,
            /* [annotation][in] */ 
            __in  IUnknown *pPrx,
            /* [annotation][in] */ 
            __in  RPCOPT_PROPERTIES dwProperty,
            /* [annotation][in] */ 
            __in  ULONG_PTR dwValue);
        
        HRESULT ( STDMETHODCALLTYPE *Query )( 
            IRpcOptions * This,
            /* [annotation][in] */ 
            __in  IUnknown *pPrx,
            /* [annotation][in] */ 
            __in  RPCOPT_PROPERTIES dwProperty,
            /* [annotation][out] */ 
            __deref_out  ULONG_PTR *pdwValue);
        
        END_INTERFACE
    } IRpcOptionsVtbl;

    interface IRpcOptions
    {
        CONST_VTBL struct IRpcOptionsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRpcOptions_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IRpcOptions_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IRpcOptions_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IRpcOptions_Set(This,pPrx,dwProperty,dwValue)	\
    ( (This)->lpVtbl -> Set(This,pPrx,dwProperty,dwValue) ) 

#define IRpcOptions_Query(This,pPrx,dwProperty,pdwValue)	\
    ( (This)->lpVtbl -> Query(This,pPrx,dwProperty,pdwValue) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IRpcOptions_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_objidl_0000_0046 */
/* [local] */ 

typedef 
enum tagGLOBALOPT_PROPERTIES
    {	COMGLB_EXCEPTION_HANDLING	= 1,
	COMGLB_APPID	= 2,
	COMGLB_RPC_THREADPOOL_SETTING	= 3
    } 	GLOBALOPT_PROPERTIES;

typedef 
enum tagGLOBALOPT_EH_VALUES
    {	COMGLB_EXCEPTION_HANDLE	= 0,
	COMGLB_EXCEPTION_DONOT_HANDLE_FATAL	= 1,
	COMGLB_EXCEPTION_DONOT_HANDLE	= COMGLB_EXCEPTION_DONOT_HANDLE_FATAL,
	COMGLB_EXCEPTION_DONOT_HANDLE_ANY	= 2
    } 	GLOBALOPT_EH_VALUES;

typedef 
enum tagGLOBALOPT_RPCTP_VALUES
    {	COMGLB_RPC_THREADPOOL_SETTING_DEFAULT_POOL	= 0,
	COMGLB_RPC_THREADPOOL_SETTING_PRIVATE_POOL	= 1
    } 	GLOBALOPT_RPCTP_VALUES;



extern RPC_IF_HANDLE __MIDL_itf_objidl_0000_0046_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_objidl_0000_0046_v0_0_s_ifspec;

#ifndef __IGlobalOptions_INTERFACE_DEFINED__
#define __IGlobalOptions_INTERFACE_DEFINED__

/* interface IGlobalOptions */
/* [uuid][unique][local][object] */ 


EXTERN_C const IID IID_IGlobalOptions;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0000015B-0000-0000-C000-000000000046")
    IGlobalOptions : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Set( 
            /* [annotation][in] */ 
            __in  GLOBALOPT_PROPERTIES dwProperty,
            /* [annotation][in] */ 
            __in  ULONG_PTR dwValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Query( 
            /* [annotation][in] */ 
            __in  GLOBALOPT_PROPERTIES dwProperty,
            /* [annotation][out] */ 
            __deref_out  ULONG_PTR *pdwValue) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IGlobalOptionsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IGlobalOptions * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IGlobalOptions * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IGlobalOptions * This);
        
        HRESULT ( STDMETHODCALLTYPE *Set )( 
            IGlobalOptions * This,
            /* [annotation][in] */ 
            __in  GLOBALOPT_PROPERTIES dwProperty,
            /* [annotation][in] */ 
            __in  ULONG_PTR dwValue);
        
        HRESULT ( STDMETHODCALLTYPE *Query )( 
            IGlobalOptions * This,
            /* [annotation][in] */ 
            __in  GLOBALOPT_PROPERTIES dwProperty,
            /* [annotation][out] */ 
            __deref_out  ULONG_PTR *pdwValue);
        
        END_INTERFACE
    } IGlobalOptionsVtbl;

    interface IGlobalOptions
    {
        CONST_VTBL struct IGlobalOptionsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IGlobalOptions_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IGlobalOptions_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IGlobalOptions_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IGlobalOptions_Set(This,dwProperty,dwValue)	\
    ( (This)->lpVtbl -> Set(This,dwProperty,dwValue) ) 

#define IGlobalOptions_Query(This,dwProperty,pdwValue)	\
    ( (This)->lpVtbl -> Query(This,dwProperty,pdwValue) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IGlobalOptions_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_objidl_0000_0047 */
/* [local] */ 

#endif //DCOM


extern RPC_IF_HANDLE __MIDL_itf_objidl_0000_0047_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_objidl_0000_0047_v0_0_s_ifspec;

#ifndef __IFillLockBytes_INTERFACE_DEFINED__
#define __IFillLockBytes_INTERFACE_DEFINED__

/* interface IFillLockBytes */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IFillLockBytes;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("99caf010-415e-11cf-8814-00aa00b569f5")
    IFillLockBytes : public IUnknown
    {
    public:
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE FillAppend( 
            /* [annotation][size_is][in] */ 
            __in_bcount(cb)  const void *pv,
            /* [annotation][in] */ 
            __in  ULONG cb,
            /* [annotation][out] */ 
            __out  ULONG *pcbWritten) = 0;
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE FillAt( 
            /* [annotation][in] */ 
            __in  ULARGE_INTEGER ulOffset,
            /* [annotation][size_is][in] */ 
            __in_bcount(cb)  const void *pv,
            /* [annotation][in] */ 
            __in  ULONG cb,
            /* [annotation][out] */ 
            __out  ULONG *pcbWritten) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetFillSize( 
            /* [in] */ ULARGE_INTEGER ulSize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Terminate( 
            /* [in] */ BOOL bCanceled) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IFillLockBytesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IFillLockBytes * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IFillLockBytes * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IFillLockBytes * This);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *FillAppend )( 
            IFillLockBytes * This,
            /* [annotation][size_is][in] */ 
            __in_bcount(cb)  const void *pv,
            /* [annotation][in] */ 
            __in  ULONG cb,
            /* [annotation][out] */ 
            __out  ULONG *pcbWritten);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *FillAt )( 
            IFillLockBytes * This,
            /* [annotation][in] */ 
            __in  ULARGE_INTEGER ulOffset,
            /* [annotation][size_is][in] */ 
            __in_bcount(cb)  const void *pv,
            /* [annotation][in] */ 
            __in  ULONG cb,
            /* [annotation][out] */ 
            __out  ULONG *pcbWritten);
        
        HRESULT ( STDMETHODCALLTYPE *SetFillSize )( 
            __RPC__in IFillLockBytes * This,
            /* [in] */ ULARGE_INTEGER ulSize);
        
        HRESULT ( STDMETHODCALLTYPE *Terminate )( 
            __RPC__in IFillLockBytes * This,
            /* [in] */ BOOL bCanceled);
        
        END_INTERFACE
    } IFillLockBytesVtbl;

    interface IFillLockBytes
    {
        CONST_VTBL struct IFillLockBytesVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFillLockBytes_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IFillLockBytes_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IFillLockBytes_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IFillLockBytes_FillAppend(This,pv,cb,pcbWritten)	\
    ( (This)->lpVtbl -> FillAppend(This,pv,cb,pcbWritten) ) 

#define IFillLockBytes_FillAt(This,ulOffset,pv,cb,pcbWritten)	\
    ( (This)->lpVtbl -> FillAt(This,ulOffset,pv,cb,pcbWritten) ) 

#define IFillLockBytes_SetFillSize(This,ulSize)	\
    ( (This)->lpVtbl -> SetFillSize(This,ulSize) ) 

#define IFillLockBytes_Terminate(This,bCanceled)	\
    ( (This)->lpVtbl -> Terminate(This,bCanceled) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT __stdcall IFillLockBytes_RemoteFillAppend_Proxy( 
    __RPC__in IFillLockBytes * This,
    /* [size_is][in] */ __RPC__in_ecount_full(cb) const byte *pv,
    /* [in] */ ULONG cb,
    /* [out] */ __RPC__out ULONG *pcbWritten);


void __RPC_STUB IFillLockBytes_RemoteFillAppend_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT __stdcall IFillLockBytes_RemoteFillAt_Proxy( 
    __RPC__in IFillLockBytes * This,
    /* [in] */ ULARGE_INTEGER ulOffset,
    /* [size_is][in] */ __RPC__in_ecount_full(cb) const byte *pv,
    /* [in] */ ULONG cb,
    /* [out] */ __RPC__out ULONG *pcbWritten);


void __RPC_STUB IFillLockBytes_RemoteFillAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IFillLockBytes_INTERFACE_DEFINED__ */


#ifndef __IProgressNotify_INTERFACE_DEFINED__
#define __IProgressNotify_INTERFACE_DEFINED__

/* interface IProgressNotify */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IProgressNotify;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("a9d758a0-4617-11cf-95fc-00aa00680db4")
    IProgressNotify : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OnProgress( 
            /* [in] */ DWORD dwProgressCurrent,
            /* [in] */ DWORD dwProgressMaximum,
            /* [in] */ BOOL fAccurate,
            /* [in] */ BOOL fOwner) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IProgressNotifyVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IProgressNotify * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IProgressNotify * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IProgressNotify * This);
        
        HRESULT ( STDMETHODCALLTYPE *OnProgress )( 
            __RPC__in IProgressNotify * This,
            /* [in] */ DWORD dwProgressCurrent,
            /* [in] */ DWORD dwProgressMaximum,
            /* [in] */ BOOL fAccurate,
            /* [in] */ BOOL fOwner);
        
        END_INTERFACE
    } IProgressNotifyVtbl;

    interface IProgressNotify
    {
        CONST_VTBL struct IProgressNotifyVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IProgressNotify_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IProgressNotify_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IProgressNotify_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IProgressNotify_OnProgress(This,dwProgressCurrent,dwProgressMaximum,fAccurate,fOwner)	\
    ( (This)->lpVtbl -> OnProgress(This,dwProgressCurrent,dwProgressMaximum,fAccurate,fOwner) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IProgressNotify_INTERFACE_DEFINED__ */


#ifndef __ILayoutStorage_INTERFACE_DEFINED__
#define __ILayoutStorage_INTERFACE_DEFINED__

/* interface ILayoutStorage */
/* [unique][uuid][object][local] */ 

typedef struct tagStorageLayout
    {
    DWORD LayoutType;
    OLECHAR *pwcsElementName;
    LARGE_INTEGER cOffset;
    LARGE_INTEGER cBytes;
    } 	StorageLayout;


EXTERN_C const IID IID_ILayoutStorage;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0e6d4d90-6738-11cf-9608-00aa00680db4")
    ILayoutStorage : public IUnknown
    {
    public:
        virtual HRESULT __stdcall LayoutScript( 
            /* [annotation][in] */ 
            __in_ecount(nEntries)  StorageLayout *pStorageLayout,
            /* [annotation][in] */ 
            __in  DWORD nEntries,
            /* [annotation][in] */ 
            __reserved  DWORD glfInterleavedFlag) = 0;
        
        virtual HRESULT __stdcall BeginMonitor( void) = 0;
        
        virtual HRESULT __stdcall EndMonitor( void) = 0;
        
        virtual HRESULT __stdcall ReLayoutDocfile( 
            /* [annotation][in] */ 
            __RPC__in  OLECHAR *pwcsNewDfName) = 0;
        
        virtual HRESULT __stdcall ReLayoutDocfileOnILockBytes( 
            /* [annotation][in] */ 
            __in  ILockBytes *pILockBytes) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ILayoutStorageVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ILayoutStorage * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ILayoutStorage * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ILayoutStorage * This);
        
        HRESULT ( __stdcall *LayoutScript )( 
            ILayoutStorage * This,
            /* [annotation][in] */ 
            __in_ecount(nEntries)  StorageLayout *pStorageLayout,
            /* [annotation][in] */ 
            __in  DWORD nEntries,
            /* [annotation][in] */ 
            __reserved  DWORD glfInterleavedFlag);
        
        HRESULT ( __stdcall *BeginMonitor )( 
            ILayoutStorage * This);
        
        HRESULT ( __stdcall *EndMonitor )( 
            ILayoutStorage * This);
        
        HRESULT ( __stdcall *ReLayoutDocfile )( 
            ILayoutStorage * This,
            /* [annotation][in] */ 
            __RPC__in  OLECHAR *pwcsNewDfName);
        
        HRESULT ( __stdcall *ReLayoutDocfileOnILockBytes )( 
            ILayoutStorage * This,
            /* [annotation][in] */ 
            __in  ILockBytes *pILockBytes);
        
        END_INTERFACE
    } ILayoutStorageVtbl;

    interface ILayoutStorage
    {
        CONST_VTBL struct ILayoutStorageVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILayoutStorage_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ILayoutStorage_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ILayoutStorage_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ILayoutStorage_LayoutScript(This,pStorageLayout,nEntries,glfInterleavedFlag)	\
    ( (This)->lpVtbl -> LayoutScript(This,pStorageLayout,nEntries,glfInterleavedFlag) ) 

#define ILayoutStorage_BeginMonitor(This)	\
    ( (This)->lpVtbl -> BeginMonitor(This) ) 

#define ILayoutStorage_EndMonitor(This)	\
    ( (This)->lpVtbl -> EndMonitor(This) ) 

#define ILayoutStorage_ReLayoutDocfile(This,pwcsNewDfName)	\
    ( (This)->lpVtbl -> ReLayoutDocfile(This,pwcsNewDfName) ) 

#define ILayoutStorage_ReLayoutDocfileOnILockBytes(This,pILockBytes)	\
    ( (This)->lpVtbl -> ReLayoutDocfileOnILockBytes(This,pILockBytes) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ILayoutStorage_INTERFACE_DEFINED__ */


#ifndef __IBlockingLock_INTERFACE_DEFINED__
#define __IBlockingLock_INTERFACE_DEFINED__

/* interface IBlockingLock */
/* [uuid][object] */ 


EXTERN_C const IID IID_IBlockingLock;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("30f3d47a-6447-11d1-8e3c-00c04fb9386d")
    IBlockingLock : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Lock( 
            /* [in] */ DWORD dwTimeout) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Unlock( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBlockingLockVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IBlockingLock * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IBlockingLock * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IBlockingLock * This);
        
        HRESULT ( STDMETHODCALLTYPE *Lock )( 
            __RPC__in IBlockingLock * This,
            /* [in] */ DWORD dwTimeout);
        
        HRESULT ( STDMETHODCALLTYPE *Unlock )( 
            __RPC__in IBlockingLock * This);
        
        END_INTERFACE
    } IBlockingLockVtbl;

    interface IBlockingLock
    {
        CONST_VTBL struct IBlockingLockVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBlockingLock_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IBlockingLock_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IBlockingLock_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IBlockingLock_Lock(This,dwTimeout)	\
    ( (This)->lpVtbl -> Lock(This,dwTimeout) ) 

#define IBlockingLock_Unlock(This)	\
    ( (This)->lpVtbl -> Unlock(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IBlockingLock_INTERFACE_DEFINED__ */


#ifndef __ITimeAndNoticeControl_INTERFACE_DEFINED__
#define __ITimeAndNoticeControl_INTERFACE_DEFINED__

/* interface ITimeAndNoticeControl */
/* [uuid][object] */ 


EXTERN_C const IID IID_ITimeAndNoticeControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("bc0bf6ae-8878-11d1-83e9-00c04fc2c6d4")
    ITimeAndNoticeControl : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SuppressChanges( 
            /* [in] */ DWORD res1,
            /* [in] */ DWORD res2) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITimeAndNoticeControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in ITimeAndNoticeControl * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in ITimeAndNoticeControl * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in ITimeAndNoticeControl * This);
        
        HRESULT ( STDMETHODCALLTYPE *SuppressChanges )( 
            __RPC__in ITimeAndNoticeControl * This,
            /* [in] */ DWORD res1,
            /* [in] */ DWORD res2);
        
        END_INTERFACE
    } ITimeAndNoticeControlVtbl;

    interface ITimeAndNoticeControl
    {
        CONST_VTBL struct ITimeAndNoticeControlVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITimeAndNoticeControl_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ITimeAndNoticeControl_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ITimeAndNoticeControl_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ITimeAndNoticeControl_SuppressChanges(This,res1,res2)	\
    ( (This)->lpVtbl -> SuppressChanges(This,res1,res2) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ITimeAndNoticeControl_INTERFACE_DEFINED__ */


#ifndef __IOplockStorage_INTERFACE_DEFINED__
#define __IOplockStorage_INTERFACE_DEFINED__

/* interface IOplockStorage */
/* [uuid][object] */ 


EXTERN_C const IID IID_IOplockStorage;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8d19c834-8879-11d1-83e9-00c04fc2c6d4")
    IOplockStorage : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CreateStorageEx( 
            /* [in] */ __RPC__in LPCWSTR pwcsName,
            /* [in] */ DWORD grfMode,
            /* [in] */ DWORD stgfmt,
            /* [in] */ DWORD grfAttrs,
            /* [in] */ __RPC__in REFIID riid,
            /* [iid_is][out] */ __RPC__deref_out_opt void **ppstgOpen) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OpenStorageEx( 
            /* [in] */ __RPC__in LPCWSTR pwcsName,
            /* [in] */ DWORD grfMode,
            /* [in] */ DWORD stgfmt,
            /* [in] */ DWORD grfAttrs,
            /* [in] */ __RPC__in REFIID riid,
            /* [iid_is][out] */ __RPC__deref_out_opt void **ppstgOpen) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOplockStorageVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IOplockStorage * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IOplockStorage * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IOplockStorage * This);
        
        HRESULT ( STDMETHODCALLTYPE *CreateStorageEx )( 
            __RPC__in IOplockStorage * This,
            /* [in] */ __RPC__in LPCWSTR pwcsName,
            /* [in] */ DWORD grfMode,
            /* [in] */ DWORD stgfmt,
            /* [in] */ DWORD grfAttrs,
            /* [in] */ __RPC__in REFIID riid,
            /* [iid_is][out] */ __RPC__deref_out_opt void **ppstgOpen);
        
        HRESULT ( STDMETHODCALLTYPE *OpenStorageEx )( 
            __RPC__in IOplockStorage * This,
            /* [in] */ __RPC__in LPCWSTR pwcsName,
            /* [in] */ DWORD grfMode,
            /* [in] */ DWORD stgfmt,
            /* [in] */ DWORD grfAttrs,
            /* [in] */ __RPC__in REFIID riid,
            /* [iid_is][out] */ __RPC__deref_out_opt void **ppstgOpen);
        
        END_INTERFACE
    } IOplockStorageVtbl;

    interface IOplockStorage
    {
        CONST_VTBL struct IOplockStorageVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOplockStorage_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IOplockStorage_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IOplockStorage_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IOplockStorage_CreateStorageEx(This,pwcsName,grfMode,stgfmt,grfAttrs,riid,ppstgOpen)	\
    ( (This)->lpVtbl -> CreateStorageEx(This,pwcsName,grfMode,stgfmt,grfAttrs,riid,ppstgOpen) ) 

#define IOplockStorage_OpenStorageEx(This,pwcsName,grfMode,stgfmt,grfAttrs,riid,ppstgOpen)	\
    ( (This)->lpVtbl -> OpenStorageEx(This,pwcsName,grfMode,stgfmt,grfAttrs,riid,ppstgOpen) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IOplockStorage_INTERFACE_DEFINED__ */


#ifndef __ISurrogate_INTERFACE_DEFINED__
#define __ISurrogate_INTERFACE_DEFINED__

/* interface ISurrogate */
/* [object][unique][version][uuid] */ 

typedef /* [unique] */  __RPC_unique_pointer ISurrogate *LPSURROGATE;


EXTERN_C const IID IID_ISurrogate;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000022-0000-0000-C000-000000000046")
    ISurrogate : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE LoadDllServer( 
            /* [in] */ __RPC__in REFCLSID Clsid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FreeSurrogate( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISurrogateVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in ISurrogate * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in ISurrogate * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in ISurrogate * This);
        
        HRESULT ( STDMETHODCALLTYPE *LoadDllServer )( 
            __RPC__in ISurrogate * This,
            /* [in] */ __RPC__in REFCLSID Clsid);
        
        HRESULT ( STDMETHODCALLTYPE *FreeSurrogate )( 
            __RPC__in ISurrogate * This);
        
        END_INTERFACE
    } ISurrogateVtbl;

    interface ISurrogate
    {
        CONST_VTBL struct ISurrogateVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISurrogate_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISurrogate_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISurrogate_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISurrogate_LoadDllServer(This,Clsid)	\
    ( (This)->lpVtbl -> LoadDllServer(This,Clsid) ) 

#define ISurrogate_FreeSurrogate(This)	\
    ( (This)->lpVtbl -> FreeSurrogate(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISurrogate_INTERFACE_DEFINED__ */


#ifndef __IGlobalInterfaceTable_INTERFACE_DEFINED__
#define __IGlobalInterfaceTable_INTERFACE_DEFINED__

/* interface IGlobalInterfaceTable */
/* [uuid][object][local] */ 

typedef /* [unique] */ IGlobalInterfaceTable *LPGLOBALINTERFACETABLE;


EXTERN_C const IID IID_IGlobalInterfaceTable;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000146-0000-0000-C000-000000000046")
    IGlobalInterfaceTable : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE RegisterInterfaceInGlobal( 
            /* [annotation][in] */ 
            __in  IUnknown *pUnk,
            /* [annotation][in] */ 
            __in  REFIID riid,
            /* [annotation][out] */ 
            __out  DWORD *pdwCookie) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RevokeInterfaceFromGlobal( 
            /* [annotation][in] */ 
            __in  DWORD dwCookie) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetInterfaceFromGlobal( 
            /* [annotation][in] */ 
            __in  DWORD dwCookie,
            /* [annotation][in] */ 
            __in  REFIID riid,
            /* [annotation][iid_is][out] */ 
            __deref_out  void **ppv) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IGlobalInterfaceTableVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IGlobalInterfaceTable * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IGlobalInterfaceTable * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IGlobalInterfaceTable * This);
        
        HRESULT ( STDMETHODCALLTYPE *RegisterInterfaceInGlobal )( 
            IGlobalInterfaceTable * This,
            /* [annotation][in] */ 
            __in  IUnknown *pUnk,
            /* [annotation][in] */ 
            __in  REFIID riid,
            /* [annotation][out] */ 
            __out  DWORD *pdwCookie);
        
        HRESULT ( STDMETHODCALLTYPE *RevokeInterfaceFromGlobal )( 
            IGlobalInterfaceTable * This,
            /* [annotation][in] */ 
            __in  DWORD dwCookie);
        
        HRESULT ( STDMETHODCALLTYPE *GetInterfaceFromGlobal )( 
            IGlobalInterfaceTable * This,
            /* [annotation][in] */ 
            __in  DWORD dwCookie,
            /* [annotation][in] */ 
            __in  REFIID riid,
            /* [annotation][iid_is][out] */ 
            __deref_out  void **ppv);
        
        END_INTERFACE
    } IGlobalInterfaceTableVtbl;

    interface IGlobalInterfaceTable
    {
        CONST_VTBL struct IGlobalInterfaceTableVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IGlobalInterfaceTable_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IGlobalInterfaceTable_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IGlobalInterfaceTable_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IGlobalInterfaceTable_RegisterInterfaceInGlobal(This,pUnk,riid,pdwCookie)	\
    ( (This)->lpVtbl -> RegisterInterfaceInGlobal(This,pUnk,riid,pdwCookie) ) 

#define IGlobalInterfaceTable_RevokeInterfaceFromGlobal(This,dwCookie)	\
    ( (This)->lpVtbl -> RevokeInterfaceFromGlobal(This,dwCookie) ) 

#define IGlobalInterfaceTable_GetInterfaceFromGlobal(This,dwCookie,riid,ppv)	\
    ( (This)->lpVtbl -> GetInterfaceFromGlobal(This,dwCookie,riid,ppv) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IGlobalInterfaceTable_INTERFACE_DEFINED__ */


#ifndef __IDirectWriterLock_INTERFACE_DEFINED__
#define __IDirectWriterLock_INTERFACE_DEFINED__

/* interface IDirectWriterLock */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDirectWriterLock;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0e6d4d92-6738-11cf-9608-00aa00680db4")
    IDirectWriterLock : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE WaitForWriteAccess( 
            /* [in] */ DWORD dwTimeout) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ReleaseWriteAccess( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE HaveWriteAccess( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDirectWriterLockVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IDirectWriterLock * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IDirectWriterLock * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IDirectWriterLock * This);
        
        HRESULT ( STDMETHODCALLTYPE *WaitForWriteAccess )( 
            __RPC__in IDirectWriterLock * This,
            /* [in] */ DWORD dwTimeout);
        
        HRESULT ( STDMETHODCALLTYPE *ReleaseWriteAccess )( 
            __RPC__in IDirectWriterLock * This);
        
        HRESULT ( STDMETHODCALLTYPE *HaveWriteAccess )( 
            __RPC__in IDirectWriterLock * This);
        
        END_INTERFACE
    } IDirectWriterLockVtbl;

    interface IDirectWriterLock
    {
        CONST_VTBL struct IDirectWriterLockVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDirectWriterLock_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IDirectWriterLock_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IDirectWriterLock_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IDirectWriterLock_WaitForWriteAccess(This,dwTimeout)	\
    ( (This)->lpVtbl -> WaitForWriteAccess(This,dwTimeout) ) 

#define IDirectWriterLock_ReleaseWriteAccess(This)	\
    ( (This)->lpVtbl -> ReleaseWriteAccess(This) ) 

#define IDirectWriterLock_HaveWriteAccess(This)	\
    ( (This)->lpVtbl -> HaveWriteAccess(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IDirectWriterLock_INTERFACE_DEFINED__ */


#ifndef __ISynchronize_INTERFACE_DEFINED__
#define __ISynchronize_INTERFACE_DEFINED__

/* interface ISynchronize */
/* [uuid][object] */ 


EXTERN_C const IID IID_ISynchronize;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000030-0000-0000-C000-000000000046")
    ISynchronize : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Wait( 
            /* [in] */ DWORD dwFlags,
            /* [in] */ DWORD dwMilliseconds) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Signal( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISynchronizeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in ISynchronize * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in ISynchronize * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in ISynchronize * This);
        
        HRESULT ( STDMETHODCALLTYPE *Wait )( 
            __RPC__in ISynchronize * This,
            /* [in] */ DWORD dwFlags,
            /* [in] */ DWORD dwMilliseconds);
        
        HRESULT ( STDMETHODCALLTYPE *Signal )( 
            __RPC__in ISynchronize * This);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            __RPC__in ISynchronize * This);
        
        END_INTERFACE
    } ISynchronizeVtbl;

    interface ISynchronize
    {
        CONST_VTBL struct ISynchronizeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISynchronize_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISynchronize_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISynchronize_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISynchronize_Wait(This,dwFlags,dwMilliseconds)	\
    ( (This)->lpVtbl -> Wait(This,dwFlags,dwMilliseconds) ) 

#define ISynchronize_Signal(This)	\
    ( (This)->lpVtbl -> Signal(This) ) 

#define ISynchronize_Reset(This)	\
    ( (This)->lpVtbl -> Reset(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISynchronize_INTERFACE_DEFINED__ */


#ifndef __ISynchronizeHandle_INTERFACE_DEFINED__
#define __ISynchronizeHandle_INTERFACE_DEFINED__

/* interface ISynchronizeHandle */
/* [uuid][object][local] */ 


EXTERN_C const IID IID_ISynchronizeHandle;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000031-0000-0000-C000-000000000046")
    ISynchronizeHandle : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetHandle( 
            /* [annotation][out] */ 
            __out  HANDLE *ph) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISynchronizeHandleVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISynchronizeHandle * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISynchronizeHandle * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISynchronizeHandle * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetHandle )( 
            ISynchronizeHandle * This,
            /* [annotation][out] */ 
            __out  HANDLE *ph);
        
        END_INTERFACE
    } ISynchronizeHandleVtbl;

    interface ISynchronizeHandle
    {
        CONST_VTBL struct ISynchronizeHandleVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISynchronizeHandle_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISynchronizeHandle_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISynchronizeHandle_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISynchronizeHandle_GetHandle(This,ph)	\
    ( (This)->lpVtbl -> GetHandle(This,ph) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISynchronizeHandle_INTERFACE_DEFINED__ */


#ifndef __ISynchronizeEvent_INTERFACE_DEFINED__
#define __ISynchronizeEvent_INTERFACE_DEFINED__

/* interface ISynchronizeEvent */
/* [uuid][object][local] */ 


EXTERN_C const IID IID_ISynchronizeEvent;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000032-0000-0000-C000-000000000046")
    ISynchronizeEvent : public ISynchronizeHandle
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetEventHandle( 
            /* [annotation][in] */ 
            __in  HANDLE *ph) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISynchronizeEventVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISynchronizeEvent * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISynchronizeEvent * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISynchronizeEvent * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetHandle )( 
            ISynchronizeEvent * This,
            /* [annotation][out] */ 
            __out  HANDLE *ph);
        
        HRESULT ( STDMETHODCALLTYPE *SetEventHandle )( 
            ISynchronizeEvent * This,
            /* [annotation][in] */ 
            __in  HANDLE *ph);
        
        END_INTERFACE
    } ISynchronizeEventVtbl;

    interface ISynchronizeEvent
    {
        CONST_VTBL struct ISynchronizeEventVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISynchronizeEvent_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISynchronizeEvent_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISynchronizeEvent_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISynchronizeEvent_GetHandle(This,ph)	\
    ( (This)->lpVtbl -> GetHandle(This,ph) ) 


#define ISynchronizeEvent_SetEventHandle(This,ph)	\
    ( (This)->lpVtbl -> SetEventHandle(This,ph) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISynchronizeEvent_INTERFACE_DEFINED__ */


#ifndef __ISynchronizeContainer_INTERFACE_DEFINED__
#define __ISynchronizeContainer_INTERFACE_DEFINED__

/* interface ISynchronizeContainer */
/* [uuid][object][local] */ 


EXTERN_C const IID IID_ISynchronizeContainer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000033-0000-0000-C000-000000000046")
    ISynchronizeContainer : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE AddSynchronize( 
            /* [annotation][in] */ 
            __in  ISynchronize *pSync) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE WaitMultiple( 
            /* [annotation][in] */ 
            __in  DWORD dwFlags,
            /* [annotation][in] */ 
            __in  DWORD dwTimeOut,
            /* [annotation][out] */ 
            __deref_out  ISynchronize **ppSync) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISynchronizeContainerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISynchronizeContainer * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISynchronizeContainer * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISynchronizeContainer * This);
        
        HRESULT ( STDMETHODCALLTYPE *AddSynchronize )( 
            ISynchronizeContainer * This,
            /* [annotation][in] */ 
            __in  ISynchronize *pSync);
        
        HRESULT ( STDMETHODCALLTYPE *WaitMultiple )( 
            ISynchronizeContainer * This,
            /* [annotation][in] */ 
            __in  DWORD dwFlags,
            /* [annotation][in] */ 
            __in  DWORD dwTimeOut,
            /* [annotation][out] */ 
            __deref_out  ISynchronize **ppSync);
        
        END_INTERFACE
    } ISynchronizeContainerVtbl;

    interface ISynchronizeContainer
    {
        CONST_VTBL struct ISynchronizeContainerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISynchronizeContainer_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISynchronizeContainer_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISynchronizeContainer_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISynchronizeContainer_AddSynchronize(This,pSync)	\
    ( (This)->lpVtbl -> AddSynchronize(This,pSync) ) 

#define ISynchronizeContainer_WaitMultiple(This,dwFlags,dwTimeOut,ppSync)	\
    ( (This)->lpVtbl -> WaitMultiple(This,dwFlags,dwTimeOut,ppSync) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISynchronizeContainer_INTERFACE_DEFINED__ */


#ifndef __ISynchronizeMutex_INTERFACE_DEFINED__
#define __ISynchronizeMutex_INTERFACE_DEFINED__

/* interface ISynchronizeMutex */
/* [uuid][object][local] */ 


EXTERN_C const IID IID_ISynchronizeMutex;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000025-0000-0000-C000-000000000046")
    ISynchronizeMutex : public ISynchronize
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ReleaseMutex( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISynchronizeMutexVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISynchronizeMutex * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISynchronizeMutex * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISynchronizeMutex * This);
        
        HRESULT ( STDMETHODCALLTYPE *Wait )( 
            ISynchronizeMutex * This,
            /* [in] */ DWORD dwFlags,
            /* [in] */ DWORD dwMilliseconds);
        
        HRESULT ( STDMETHODCALLTYPE *Signal )( 
            ISynchronizeMutex * This);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            ISynchronizeMutex * This);
        
        HRESULT ( STDMETHODCALLTYPE *ReleaseMutex )( 
            ISynchronizeMutex * This);
        
        END_INTERFACE
    } ISynchronizeMutexVtbl;

    interface ISynchronizeMutex
    {
        CONST_VTBL struct ISynchronizeMutexVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISynchronizeMutex_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISynchronizeMutex_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISynchronizeMutex_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISynchronizeMutex_Wait(This,dwFlags,dwMilliseconds)	\
    ( (This)->lpVtbl -> Wait(This,dwFlags,dwMilliseconds) ) 

#define ISynchronizeMutex_Signal(This)	\
    ( (This)->lpVtbl -> Signal(This) ) 

#define ISynchronizeMutex_Reset(This)	\
    ( (This)->lpVtbl -> Reset(This) ) 


#define ISynchronizeMutex_ReleaseMutex(This)	\
    ( (This)->lpVtbl -> ReleaseMutex(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISynchronizeMutex_INTERFACE_DEFINED__ */


#ifndef __ICancelMethodCalls_INTERFACE_DEFINED__
#define __ICancelMethodCalls_INTERFACE_DEFINED__

/* interface ICancelMethodCalls */
/* [uuid][object][local] */ 

typedef /* [unique] */ ICancelMethodCalls *LPCANCELMETHODCALLS;


EXTERN_C const IID IID_ICancelMethodCalls;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000029-0000-0000-C000-000000000046")
    ICancelMethodCalls : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Cancel( 
            /* [annotation][in] */ 
            __in  ULONG ulSeconds) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE TestCancel( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICancelMethodCallsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICancelMethodCalls * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICancelMethodCalls * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICancelMethodCalls * This);
        
        HRESULT ( STDMETHODCALLTYPE *Cancel )( 
            ICancelMethodCalls * This,
            /* [annotation][in] */ 
            __in  ULONG ulSeconds);
        
        HRESULT ( STDMETHODCALLTYPE *TestCancel )( 
            ICancelMethodCalls * This);
        
        END_INTERFACE
    } ICancelMethodCallsVtbl;

    interface ICancelMethodCalls
    {
        CONST_VTBL struct ICancelMethodCallsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICancelMethodCalls_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICancelMethodCalls_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICancelMethodCalls_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICancelMethodCalls_Cancel(This,ulSeconds)	\
    ( (This)->lpVtbl -> Cancel(This,ulSeconds) ) 

#define ICancelMethodCalls_TestCancel(This)	\
    ( (This)->lpVtbl -> TestCancel(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICancelMethodCalls_INTERFACE_DEFINED__ */


#ifndef __IAsyncManager_INTERFACE_DEFINED__
#define __IAsyncManager_INTERFACE_DEFINED__

/* interface IAsyncManager */
/* [uuid][object][local] */ 

typedef 
enum tagDCOM_CALL_STATE
    {	DCOM_NONE	= 0,
	DCOM_CALL_COMPLETE	= 0x1,
	DCOM_CALL_CANCELED	= 0x2
    } 	DCOM_CALL_STATE;


EXTERN_C const IID IID_IAsyncManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0000002A-0000-0000-C000-000000000046")
    IAsyncManager : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CompleteCall( 
            /* [annotation][in] */ 
            __in  HRESULT Result) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCallContext( 
            /* [annotation][in] */ 
            __in  REFIID riid,
            /* [annotation][out] */ 
            __deref_out  void **pInterface) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetState( 
            /* [annotation][out] */ 
            __out  ULONG *pulStateFlags) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAsyncManagerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAsyncManager * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAsyncManager * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAsyncManager * This);
        
        HRESULT ( STDMETHODCALLTYPE *CompleteCall )( 
            IAsyncManager * This,
            /* [annotation][in] */ 
            __in  HRESULT Result);
        
        HRESULT ( STDMETHODCALLTYPE *GetCallContext )( 
            IAsyncManager * This,
            /* [annotation][in] */ 
            __in  REFIID riid,
            /* [annotation][out] */ 
            __deref_out  void **pInterface);
        
        HRESULT ( STDMETHODCALLTYPE *GetState )( 
            IAsyncManager * This,
            /* [annotation][out] */ 
            __out  ULONG *pulStateFlags);
        
        END_INTERFACE
    } IAsyncManagerVtbl;

    interface IAsyncManager
    {
        CONST_VTBL struct IAsyncManagerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAsyncManager_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAsyncManager_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAsyncManager_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAsyncManager_CompleteCall(This,Result)	\
    ( (This)->lpVtbl -> CompleteCall(This,Result) ) 

#define IAsyncManager_GetCallContext(This,riid,pInterface)	\
    ( (This)->lpVtbl -> GetCallContext(This,riid,pInterface) ) 

#define IAsyncManager_GetState(This,pulStateFlags)	\
    ( (This)->lpVtbl -> GetState(This,pulStateFlags) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAsyncManager_INTERFACE_DEFINED__ */


#ifndef __ICallFactory_INTERFACE_DEFINED__
#define __ICallFactory_INTERFACE_DEFINED__

/* interface ICallFactory */
/* [unique][uuid][object][local] */ 


EXTERN_C const IID IID_ICallFactory;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1c733a30-2a1c-11ce-ade5-00aa0044773d")
    ICallFactory : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CreateCall( 
            /* [annotation][in] */ 
            __in  REFIID riid,
            /* [annotation][in] */ 
            __in_opt  IUnknown *pCtrlUnk,
            /* [annotation][in] */ 
            __in  REFIID riid2,
            /* [annotation][iid_is][out] */ 
            __deref_out  IUnknown **ppv) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICallFactoryVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICallFactory * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICallFactory * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICallFactory * This);
        
        HRESULT ( STDMETHODCALLTYPE *CreateCall )( 
            ICallFactory * This,
            /* [annotation][in] */ 
            __in  REFIID riid,
            /* [annotation][in] */ 
            __in_opt  IUnknown *pCtrlUnk,
            /* [annotation][in] */ 
            __in  REFIID riid2,
            /* [annotation][iid_is][out] */ 
            __deref_out  IUnknown **ppv);
        
        END_INTERFACE
    } ICallFactoryVtbl;

    interface ICallFactory
    {
        CONST_VTBL struct ICallFactoryVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICallFactory_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICallFactory_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICallFactory_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICallFactory_CreateCall(This,riid,pCtrlUnk,riid2,ppv)	\
    ( (This)->lpVtbl -> CreateCall(This,riid,pCtrlUnk,riid2,ppv) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICallFactory_INTERFACE_DEFINED__ */


#ifndef __IRpcHelper_INTERFACE_DEFINED__
#define __IRpcHelper_INTERFACE_DEFINED__

/* interface IRpcHelper */
/* [object][local][unique][version][uuid] */ 


EXTERN_C const IID IID_IRpcHelper;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000149-0000-0000-C000-000000000046")
    IRpcHelper : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetDCOMProtocolVersion( 
            /* [annotation][out] */ 
            __out  DWORD *pComVersion) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIIDFromOBJREF( 
            /* [annotation][in] */ 
            __in  void *pObjRef,
            /* [annotation][out] */ 
            __deref_out  IID **piid) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IRpcHelperVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IRpcHelper * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IRpcHelper * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IRpcHelper * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetDCOMProtocolVersion )( 
            IRpcHelper * This,
            /* [annotation][out] */ 
            __out  DWORD *pComVersion);
        
        HRESULT ( STDMETHODCALLTYPE *GetIIDFromOBJREF )( 
            IRpcHelper * This,
            /* [annotation][in] */ 
            __in  void *pObjRef,
            /* [annotation][out] */ 
            __deref_out  IID **piid);
        
        END_INTERFACE
    } IRpcHelperVtbl;

    interface IRpcHelper
    {
        CONST_VTBL struct IRpcHelperVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRpcHelper_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IRpcHelper_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IRpcHelper_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IRpcHelper_GetDCOMProtocolVersion(This,pComVersion)	\
    ( (This)->lpVtbl -> GetDCOMProtocolVersion(This,pComVersion) ) 

#define IRpcHelper_GetIIDFromOBJREF(This,pObjRef,piid)	\
    ( (This)->lpVtbl -> GetIIDFromOBJREF(This,pObjRef,piid) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IRpcHelper_INTERFACE_DEFINED__ */


#ifndef __IReleaseMarshalBuffers_INTERFACE_DEFINED__
#define __IReleaseMarshalBuffers_INTERFACE_DEFINED__

/* interface IReleaseMarshalBuffers */
/* [uuid][object][local] */ 


EXTERN_C const IID IID_IReleaseMarshalBuffers;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("eb0cb9e8-7996-11d2-872e-0000f8080859")
    IReleaseMarshalBuffers : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ReleaseMarshalBuffer( 
            /* [annotation][out][in] */ 
            __inout  RPCOLEMESSAGE *pMsg,
            /* [annotation][in] */ 
            __in  DWORD dwFlags,
            /* [annotation][unique][in] */ 
            __in_opt  IUnknown *pChnl) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IReleaseMarshalBuffersVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IReleaseMarshalBuffers * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IReleaseMarshalBuffers * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IReleaseMarshalBuffers * This);
        
        HRESULT ( STDMETHODCALLTYPE *ReleaseMarshalBuffer )( 
            IReleaseMarshalBuffers * This,
            /* [annotation][out][in] */ 
            __inout  RPCOLEMESSAGE *pMsg,
            /* [annotation][in] */ 
            __in  DWORD dwFlags,
            /* [annotation][unique][in] */ 
            __in_opt  IUnknown *pChnl);
        
        END_INTERFACE
    } IReleaseMarshalBuffersVtbl;

    interface IReleaseMarshalBuffers
    {
        CONST_VTBL struct IReleaseMarshalBuffersVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IReleaseMarshalBuffers_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IReleaseMarshalBuffers_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IReleaseMarshalBuffers_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IReleaseMarshalBuffers_ReleaseMarshalBuffer(This,pMsg,dwFlags,pChnl)	\
    ( (This)->lpVtbl -> ReleaseMarshalBuffer(This,pMsg,dwFlags,pChnl) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IReleaseMarshalBuffers_INTERFACE_DEFINED__ */


#ifndef __IWaitMultiple_INTERFACE_DEFINED__
#define __IWaitMultiple_INTERFACE_DEFINED__

/* interface IWaitMultiple */
/* [uuid][object][local] */ 


EXTERN_C const IID IID_IWaitMultiple;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0000002B-0000-0000-C000-000000000046")
    IWaitMultiple : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE WaitMultiple( 
            /* [annotation][in] */ 
            __in  DWORD timeout,
            /* [annotation][out] */ 
            __deref_out  ISynchronize **pSync) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddSynchronize( 
            /* [annotation][in] */ 
            __in  ISynchronize *pSync) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWaitMultipleVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWaitMultiple * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWaitMultiple * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWaitMultiple * This);
        
        HRESULT ( STDMETHODCALLTYPE *WaitMultiple )( 
            IWaitMultiple * This,
            /* [annotation][in] */ 
            __in  DWORD timeout,
            /* [annotation][out] */ 
            __deref_out  ISynchronize **pSync);
        
        HRESULT ( STDMETHODCALLTYPE *AddSynchronize )( 
            IWaitMultiple * This,
            /* [annotation][in] */ 
            __in  ISynchronize *pSync);
        
        END_INTERFACE
    } IWaitMultipleVtbl;

    interface IWaitMultiple
    {
        CONST_VTBL struct IWaitMultipleVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWaitMultiple_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWaitMultiple_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWaitMultiple_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWaitMultiple_WaitMultiple(This,timeout,pSync)	\
    ( (This)->lpVtbl -> WaitMultiple(This,timeout,pSync) ) 

#define IWaitMultiple_AddSynchronize(This,pSync)	\
    ( (This)->lpVtbl -> AddSynchronize(This,pSync) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWaitMultiple_INTERFACE_DEFINED__ */


#ifndef __IUrlMon_INTERFACE_DEFINED__
#define __IUrlMon_INTERFACE_DEFINED__

/* interface IUrlMon */
/* [uuid][object] */ 


EXTERN_C const IID IID_IUrlMon;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000026-0000-0000-C000-000000000046")
    IUrlMon : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE AsyncGetClassBits( 
            /* [in] */ __RPC__in REFCLSID rclsid,
            /* [unique][in] */ __RPC__in_opt LPCWSTR pszTYPE,
            /* [unique][in] */ __RPC__in_opt LPCWSTR pszExt,
            /* [in] */ DWORD dwFileVersionMS,
            /* [in] */ DWORD dwFileVersionLS,
            /* [unique][in] */ __RPC__in_opt LPCWSTR pszCodeBase,
            /* [in] */ __RPC__in_opt IBindCtx *pbc,
            /* [in] */ DWORD dwClassContext,
            /* [in] */ __RPC__in REFIID riid,
            /* [in] */ DWORD flags) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUrlMonVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IUrlMon * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IUrlMon * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IUrlMon * This);
        
        HRESULT ( STDMETHODCALLTYPE *AsyncGetClassBits )( 
            __RPC__in IUrlMon * This,
            /* [in] */ __RPC__in REFCLSID rclsid,
            /* [unique][in] */ __RPC__in_opt LPCWSTR pszTYPE,
            /* [unique][in] */ __RPC__in_opt LPCWSTR pszExt,
            /* [in] */ DWORD dwFileVersionMS,
            /* [in] */ DWORD dwFileVersionLS,
            /* [unique][in] */ __RPC__in_opt LPCWSTR pszCodeBase,
            /* [in] */ __RPC__in_opt IBindCtx *pbc,
            /* [in] */ DWORD dwClassContext,
            /* [in] */ __RPC__in REFIID riid,
            /* [in] */ DWORD flags);
        
        END_INTERFACE
    } IUrlMonVtbl;

    interface IUrlMon
    {
        CONST_VTBL struct IUrlMonVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUrlMon_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUrlMon_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUrlMon_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUrlMon_AsyncGetClassBits(This,rclsid,pszTYPE,pszExt,dwFileVersionMS,dwFileVersionLS,pszCodeBase,pbc,dwClassContext,riid,flags)	\
    ( (This)->lpVtbl -> AsyncGetClassBits(This,rclsid,pszTYPE,pszExt,dwFileVersionMS,dwFileVersionLS,pszCodeBase,pbc,dwClassContext,riid,flags) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUrlMon_INTERFACE_DEFINED__ */


#ifndef __IForegroundTransfer_INTERFACE_DEFINED__
#define __IForegroundTransfer_INTERFACE_DEFINED__

/* interface IForegroundTransfer */
/* [uuid][object][local] */ 


EXTERN_C const IID IID_IForegroundTransfer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000145-0000-0000-C000-000000000046")
    IForegroundTransfer : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE AllowForegroundTransfer( 
            /* [annotation][in] */ 
            __reserved  void *lpvReserved) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IForegroundTransferVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IForegroundTransfer * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IForegroundTransfer * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IForegroundTransfer * This);
        
        HRESULT ( STDMETHODCALLTYPE *AllowForegroundTransfer )( 
            IForegroundTransfer * This,
            /* [annotation][in] */ 
            __reserved  void *lpvReserved);
        
        END_INTERFACE
    } IForegroundTransferVtbl;

    interface IForegroundTransfer
    {
        CONST_VTBL struct IForegroundTransferVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IForegroundTransfer_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IForegroundTransfer_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IForegroundTransfer_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IForegroundTransfer_AllowForegroundTransfer(This,lpvReserved)	\
    ( (This)->lpVtbl -> AllowForegroundTransfer(This,lpvReserved) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IForegroundTransfer_INTERFACE_DEFINED__ */


#ifndef __IAddrTrackingControl_INTERFACE_DEFINED__
#define __IAddrTrackingControl_INTERFACE_DEFINED__

/* interface IAddrTrackingControl */
/* [uuid][object][local] */ 

typedef /* [unique] */ IAddrTrackingControl *LPADDRTRACKINGCONTROL;


EXTERN_C const IID IID_IAddrTrackingControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000147-0000-0000-C000-000000000046")
    IAddrTrackingControl : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE EnableCOMDynamicAddrTracking( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DisableCOMDynamicAddrTracking( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAddrTrackingControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAddrTrackingControl * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAddrTrackingControl * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAddrTrackingControl * This);
        
        HRESULT ( STDMETHODCALLTYPE *EnableCOMDynamicAddrTracking )( 
            IAddrTrackingControl * This);
        
        HRESULT ( STDMETHODCALLTYPE *DisableCOMDynamicAddrTracking )( 
            IAddrTrackingControl * This);
        
        END_INTERFACE
    } IAddrTrackingControlVtbl;

    interface IAddrTrackingControl
    {
        CONST_VTBL struct IAddrTrackingControlVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAddrTrackingControl_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAddrTrackingControl_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAddrTrackingControl_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAddrTrackingControl_EnableCOMDynamicAddrTracking(This)	\
    ( (This)->lpVtbl -> EnableCOMDynamicAddrTracking(This) ) 

#define IAddrTrackingControl_DisableCOMDynamicAddrTracking(This)	\
    ( (This)->lpVtbl -> DisableCOMDynamicAddrTracking(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAddrTrackingControl_INTERFACE_DEFINED__ */


#ifndef __IAddrExclusionControl_INTERFACE_DEFINED__
#define __IAddrExclusionControl_INTERFACE_DEFINED__

/* interface IAddrExclusionControl */
/* [uuid][object][local] */ 

typedef /* [unique] */ IAddrExclusionControl *LPADDREXCLUSIONCONTROL;


EXTERN_C const IID IID_IAddrExclusionControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000148-0000-0000-C000-000000000046")
    IAddrExclusionControl : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetCurrentAddrExclusionList( 
            /* [annotation][in] */ 
            __in  REFIID riid,
            /* [annotation][iid_is][out] */ 
            __deref_out  void **ppEnumerator) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UpdateAddrExclusionList( 
            /* [annotation][in] */ 
            __in  IUnknown *pEnumerator) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAddrExclusionControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAddrExclusionControl * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAddrExclusionControl * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAddrExclusionControl * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetCurrentAddrExclusionList )( 
            IAddrExclusionControl * This,
            /* [annotation][in] */ 
            __in  REFIID riid,
            /* [annotation][iid_is][out] */ 
            __deref_out  void **ppEnumerator);
        
        HRESULT ( STDMETHODCALLTYPE *UpdateAddrExclusionList )( 
            IAddrExclusionControl * This,
            /* [annotation][in] */ 
            __in  IUnknown *pEnumerator);
        
        END_INTERFACE
    } IAddrExclusionControlVtbl;

    interface IAddrExclusionControl
    {
        CONST_VTBL struct IAddrExclusionControlVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAddrExclusionControl_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAddrExclusionControl_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAddrExclusionControl_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAddrExclusionControl_GetCurrentAddrExclusionList(This,riid,ppEnumerator)	\
    ( (This)->lpVtbl -> GetCurrentAddrExclusionList(This,riid,ppEnumerator) ) 

#define IAddrExclusionControl_UpdateAddrExclusionList(This,pEnumerator)	\
    ( (This)->lpVtbl -> UpdateAddrExclusionList(This,pEnumerator) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAddrExclusionControl_INTERFACE_DEFINED__ */


#ifndef __IPipeByte_INTERFACE_DEFINED__
#define __IPipeByte_INTERFACE_DEFINED__

/* interface IPipeByte */
/* [unique][async_uuid][uuid][object] */ 


EXTERN_C const IID IID_IPipeByte;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DB2F3ACA-2F86-11d1-8E04-00C04FB9989A")
    IPipeByte : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Pull( 
            /* [length_is][size_is][out] */ __RPC__out_ecount_part(cRequest, *pcReturned) BYTE *buf,
            /* [in] */ ULONG cRequest,
            /* [out] */ __RPC__out ULONG *pcReturned) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Push( 
            /* [size_is][in] */ __RPC__in_ecount_full(cSent) BYTE *buf,
            /* [in] */ ULONG cSent) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPipeByteVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IPipeByte * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IPipeByte * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IPipeByte * This);
        
        HRESULT ( STDMETHODCALLTYPE *Pull )( 
            __RPC__in IPipeByte * This,
            /* [length_is][size_is][out] */ __RPC__out_ecount_part(cRequest, *pcReturned) BYTE *buf,
            /* [in] */ ULONG cRequest,
            /* [out] */ __RPC__out ULONG *pcReturned);
        
        HRESULT ( STDMETHODCALLTYPE *Push )( 
            __RPC__in IPipeByte * This,
            /* [size_is][in] */ __RPC__in_ecount_full(cSent) BYTE *buf,
            /* [in] */ ULONG cSent);
        
        END_INTERFACE
    } IPipeByteVtbl;

    interface IPipeByte
    {
        CONST_VTBL struct IPipeByteVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPipeByte_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IPipeByte_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IPipeByte_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IPipeByte_Pull(This,buf,cRequest,pcReturned)	\
    ( (This)->lpVtbl -> Pull(This,buf,cRequest,pcReturned) ) 

#define IPipeByte_Push(This,buf,cSent)	\
    ( (This)->lpVtbl -> Push(This,buf,cSent) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IPipeByte_INTERFACE_DEFINED__ */


#ifndef __AsyncIPipeByte_INTERFACE_DEFINED__
#define __AsyncIPipeByte_INTERFACE_DEFINED__

/* interface AsyncIPipeByte */
/* [uuid][unique][object] */ 


EXTERN_C const IID IID_AsyncIPipeByte;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DB2F3ACB-2F86-11d1-8E04-00C04FB9989A")
    AsyncIPipeByte : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Begin_Pull( 
            /* [in] */ ULONG cRequest) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Finish_Pull( 
            /* [length_is][size_is][out] */ __RPC__out_xcount_part(cRequest, *pcReturned) BYTE *buf,
            /* [out] */ __RPC__out ULONG *pcReturned) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Begin_Push( 
            /* [size_is][in] */ __RPC__in_xcount_full(cSent) BYTE *buf,
            /* [in] */ ULONG cSent) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Finish_Push( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct AsyncIPipeByteVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in AsyncIPipeByte * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in AsyncIPipeByte * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in AsyncIPipeByte * This);
        
        HRESULT ( STDMETHODCALLTYPE *Begin_Pull )( 
            __RPC__in AsyncIPipeByte * This,
            /* [in] */ ULONG cRequest);
        
        HRESULT ( STDMETHODCALLTYPE *Finish_Pull )( 
            __RPC__in AsyncIPipeByte * This,
            /* [length_is][size_is][out] */ __RPC__out_xcount_part(cRequest, *pcReturned) BYTE *buf,
            /* [out] */ __RPC__out ULONG *pcReturned);
        
        HRESULT ( STDMETHODCALLTYPE *Begin_Push )( 
            __RPC__in AsyncIPipeByte * This,
            /* [size_is][in] */ __RPC__in_xcount_full(cSent) BYTE *buf,
            /* [in] */ ULONG cSent);
        
        HRESULT ( STDMETHODCALLTYPE *Finish_Push )( 
            __RPC__in AsyncIPipeByte * This);
        
        END_INTERFACE
    } AsyncIPipeByteVtbl;

    interface AsyncIPipeByte
    {
        CONST_VTBL struct AsyncIPipeByteVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define AsyncIPipeByte_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define AsyncIPipeByte_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define AsyncIPipeByte_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define AsyncIPipeByte_Begin_Pull(This,cRequest)	\
    ( (This)->lpVtbl -> Begin_Pull(This,cRequest) ) 

#define AsyncIPipeByte_Finish_Pull(This,buf,pcReturned)	\
    ( (This)->lpVtbl -> Finish_Pull(This,buf,pcReturned) ) 

#define AsyncIPipeByte_Begin_Push(This,buf,cSent)	\
    ( (This)->lpVtbl -> Begin_Push(This,buf,cSent) ) 

#define AsyncIPipeByte_Finish_Push(This)	\
    ( (This)->lpVtbl -> Finish_Push(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __AsyncIPipeByte_INTERFACE_DEFINED__ */


#ifndef __IPipeLong_INTERFACE_DEFINED__
#define __IPipeLong_INTERFACE_DEFINED__

/* interface IPipeLong */
/* [unique][async_uuid][uuid][object] */ 


EXTERN_C const IID IID_IPipeLong;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DB2F3ACC-2F86-11d1-8E04-00C04FB9989A")
    IPipeLong : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Pull( 
            /* [length_is][size_is][out] */ __RPC__out_ecount_part(cRequest, *pcReturned) LONG *buf,
            /* [in] */ ULONG cRequest,
            /* [out] */ __RPC__out ULONG *pcReturned) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Push( 
            /* [size_is][in] */ __RPC__in_ecount_full(cSent) LONG *buf,
            /* [in] */ ULONG cSent) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPipeLongVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IPipeLong * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IPipeLong * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IPipeLong * This);
        
        HRESULT ( STDMETHODCALLTYPE *Pull )( 
            __RPC__in IPipeLong * This,
            /* [length_is][size_is][out] */ __RPC__out_ecount_part(cRequest, *pcReturned) LONG *buf,
            /* [in] */ ULONG cRequest,
            /* [out] */ __RPC__out ULONG *pcReturned);
        
        HRESULT ( STDMETHODCALLTYPE *Push )( 
            __RPC__in IPipeLong * This,
            /* [size_is][in] */ __RPC__in_ecount_full(cSent) LONG *buf,
            /* [in] */ ULONG cSent);
        
        END_INTERFACE
    } IPipeLongVtbl;

    interface IPipeLong
    {
        CONST_VTBL struct IPipeLongVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPipeLong_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IPipeLong_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IPipeLong_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IPipeLong_Pull(This,buf,cRequest,pcReturned)	\
    ( (This)->lpVtbl -> Pull(This,buf,cRequest,pcReturned) ) 

#define IPipeLong_Push(This,buf,cSent)	\
    ( (This)->lpVtbl -> Push(This,buf,cSent) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IPipeLong_INTERFACE_DEFINED__ */


#ifndef __AsyncIPipeLong_INTERFACE_DEFINED__
#define __AsyncIPipeLong_INTERFACE_DEFINED__

/* interface AsyncIPipeLong */
/* [uuid][unique][object] */ 


EXTERN_C const IID IID_AsyncIPipeLong;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DB2F3ACD-2F86-11d1-8E04-00C04FB9989A")
    AsyncIPipeLong : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Begin_Pull( 
            /* [in] */ ULONG cRequest) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Finish_Pull( 
            /* [length_is][size_is][out] */ __RPC__out_xcount_part(cRequest, *pcReturned) LONG *buf,
            /* [out] */ __RPC__out ULONG *pcReturned) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Begin_Push( 
            /* [size_is][in] */ __RPC__in_xcount_full(cSent) LONG *buf,
            /* [in] */ ULONG cSent) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Finish_Push( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct AsyncIPipeLongVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in AsyncIPipeLong * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in AsyncIPipeLong * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in AsyncIPipeLong * This);
        
        HRESULT ( STDMETHODCALLTYPE *Begin_Pull )( 
            __RPC__in AsyncIPipeLong * This,
            /* [in] */ ULONG cRequest);
        
        HRESULT ( STDMETHODCALLTYPE *Finish_Pull )( 
            __RPC__in AsyncIPipeLong * This,
            /* [length_is][size_is][out] */ __RPC__out_xcount_part(cRequest, *pcReturned) LONG *buf,
            /* [out] */ __RPC__out ULONG *pcReturned);
        
        HRESULT ( STDMETHODCALLTYPE *Begin_Push )( 
            __RPC__in AsyncIPipeLong * This,
            /* [size_is][in] */ __RPC__in_xcount_full(cSent) LONG *buf,
            /* [in] */ ULONG cSent);
        
        HRESULT ( STDMETHODCALLTYPE *Finish_Push )( 
            __RPC__in AsyncIPipeLong * This);
        
        END_INTERFACE
    } AsyncIPipeLongVtbl;

    interface AsyncIPipeLong
    {
        CONST_VTBL struct AsyncIPipeLongVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define AsyncIPipeLong_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define AsyncIPipeLong_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define AsyncIPipeLong_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define AsyncIPipeLong_Begin_Pull(This,cRequest)	\
    ( (This)->lpVtbl -> Begin_Pull(This,cRequest) ) 

#define AsyncIPipeLong_Finish_Pull(This,buf,pcReturned)	\
    ( (This)->lpVtbl -> Finish_Pull(This,buf,pcReturned) ) 

#define AsyncIPipeLong_Begin_Push(This,buf,cSent)	\
    ( (This)->lpVtbl -> Begin_Push(This,buf,cSent) ) 

#define AsyncIPipeLong_Finish_Push(This)	\
    ( (This)->lpVtbl -> Finish_Push(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __AsyncIPipeLong_INTERFACE_DEFINED__ */


#ifndef __IPipeDouble_INTERFACE_DEFINED__
#define __IPipeDouble_INTERFACE_DEFINED__

/* interface IPipeDouble */
/* [unique][async_uuid][uuid][object] */ 


EXTERN_C const IID IID_IPipeDouble;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DB2F3ACE-2F86-11d1-8E04-00C04FB9989A")
    IPipeDouble : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Pull( 
            /* [length_is][size_is][out] */ __RPC__out_ecount_part(cRequest, *pcReturned) DOUBLE *buf,
            /* [in] */ ULONG cRequest,
            /* [out] */ __RPC__out ULONG *pcReturned) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Push( 
            /* [size_is][in] */ __RPC__in_ecount_full(cSent) DOUBLE *buf,
            /* [in] */ ULONG cSent) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPipeDoubleVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IPipeDouble * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IPipeDouble * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IPipeDouble * This);
        
        HRESULT ( STDMETHODCALLTYPE *Pull )( 
            __RPC__in IPipeDouble * This,
            /* [length_is][size_is][out] */ __RPC__out_ecount_part(cRequest, *pcReturned) DOUBLE *buf,
            /* [in] */ ULONG cRequest,
            /* [out] */ __RPC__out ULONG *pcReturned);
        
        HRESULT ( STDMETHODCALLTYPE *Push )( 
            __RPC__in IPipeDouble * This,
            /* [size_is][in] */ __RPC__in_ecount_full(cSent) DOUBLE *buf,
            /* [in] */ ULONG cSent);
        
        END_INTERFACE
    } IPipeDoubleVtbl;

    interface IPipeDouble
    {
        CONST_VTBL struct IPipeDoubleVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPipeDouble_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IPipeDouble_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IPipeDouble_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IPipeDouble_Pull(This,buf,cRequest,pcReturned)	\
    ( (This)->lpVtbl -> Pull(This,buf,cRequest,pcReturned) ) 

#define IPipeDouble_Push(This,buf,cSent)	\
    ( (This)->lpVtbl -> Push(This,buf,cSent) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IPipeDouble_INTERFACE_DEFINED__ */


#ifndef __AsyncIPipeDouble_INTERFACE_DEFINED__
#define __AsyncIPipeDouble_INTERFACE_DEFINED__

/* interface AsyncIPipeDouble */
/* [uuid][unique][object] */ 


EXTERN_C const IID IID_AsyncIPipeDouble;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DB2F3ACF-2F86-11d1-8E04-00C04FB9989A")
    AsyncIPipeDouble : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Begin_Pull( 
            /* [in] */ ULONG cRequest) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Finish_Pull( 
            /* [length_is][size_is][out] */ __RPC__out_xcount_part(cRequest, *pcReturned) DOUBLE *buf,
            /* [out] */ __RPC__out ULONG *pcReturned) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Begin_Push( 
            /* [size_is][in] */ __RPC__in_xcount_full(cSent) DOUBLE *buf,
            /* [in] */ ULONG cSent) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Finish_Push( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct AsyncIPipeDoubleVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in AsyncIPipeDouble * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in AsyncIPipeDouble * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in AsyncIPipeDouble * This);
        
        HRESULT ( STDMETHODCALLTYPE *Begin_Pull )( 
            __RPC__in AsyncIPipeDouble * This,
            /* [in] */ ULONG cRequest);
        
        HRESULT ( STDMETHODCALLTYPE *Finish_Pull )( 
            __RPC__in AsyncIPipeDouble * This,
            /* [length_is][size_is][out] */ __RPC__out_xcount_part(cRequest, *pcReturned) DOUBLE *buf,
            /* [out] */ __RPC__out ULONG *pcReturned);
        
        HRESULT ( STDMETHODCALLTYPE *Begin_Push )( 
            __RPC__in AsyncIPipeDouble * This,
            /* [size_is][in] */ __RPC__in_xcount_full(cSent) DOUBLE *buf,
            /* [in] */ ULONG cSent);
        
        HRESULT ( STDMETHODCALLTYPE *Finish_Push )( 
            __RPC__in AsyncIPipeDouble * This);
        
        END_INTERFACE
    } AsyncIPipeDoubleVtbl;

    interface AsyncIPipeDouble
    {
        CONST_VTBL struct AsyncIPipeDoubleVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define AsyncIPipeDouble_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define AsyncIPipeDouble_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define AsyncIPipeDouble_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define AsyncIPipeDouble_Begin_Pull(This,cRequest)	\
    ( (This)->lpVtbl -> Begin_Pull(This,cRequest) ) 

#define AsyncIPipeDouble_Finish_Pull(This,buf,pcReturned)	\
    ( (This)->lpVtbl -> Finish_Pull(This,buf,pcReturned) ) 

#define AsyncIPipeDouble_Begin_Push(This,buf,cSent)	\
    ( (This)->lpVtbl -> Begin_Push(This,buf,cSent) ) 

#define AsyncIPipeDouble_Finish_Push(This)	\
    ( (This)->lpVtbl -> Finish_Push(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __AsyncIPipeDouble_INTERFACE_DEFINED__ */


#ifndef __IThumbnailExtractor_INTERFACE_DEFINED__
#define __IThumbnailExtractor_INTERFACE_DEFINED__

/* interface IThumbnailExtractor */
/* [object][uuid] */ 


EXTERN_C const IID IID_IThumbnailExtractor;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("969dc708-5c76-11d1-8d86-0000f804b057")
    IThumbnailExtractor : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ExtractThumbnail( 
            /* [in] */ __RPC__in_opt IStorage *pStg,
            /* [in] */ ULONG ulLength,
            /* [in] */ ULONG ulHeight,
            /* [out] */ __RPC__out ULONG *pulOutputLength,
            /* [out] */ __RPC__out ULONG *pulOutputHeight,
            /* [out] */ __RPC__deref_out_opt HBITMAP *phOutputBitmap) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnFileUpdated( 
            /* [in] */ __RPC__in_opt IStorage *pStg) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IThumbnailExtractorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IThumbnailExtractor * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IThumbnailExtractor * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IThumbnailExtractor * This);
        
        HRESULT ( STDMETHODCALLTYPE *ExtractThumbnail )( 
            __RPC__in IThumbnailExtractor * This,
            /* [in] */ __RPC__in_opt IStorage *pStg,
            /* [in] */ ULONG ulLength,
            /* [in] */ ULONG ulHeight,
            /* [out] */ __RPC__out ULONG *pulOutputLength,
            /* [out] */ __RPC__out ULONG *pulOutputHeight,
            /* [out] */ __RPC__deref_out_opt HBITMAP *phOutputBitmap);
        
        HRESULT ( STDMETHODCALLTYPE *OnFileUpdated )( 
            __RPC__in IThumbnailExtractor * This,
            /* [in] */ __RPC__in_opt IStorage *pStg);
        
        END_INTERFACE
    } IThumbnailExtractorVtbl;

    interface IThumbnailExtractor
    {
        CONST_VTBL struct IThumbnailExtractorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IThumbnailExtractor_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IThumbnailExtractor_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IThumbnailExtractor_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IThumbnailExtractor_ExtractThumbnail(This,pStg,ulLength,ulHeight,pulOutputLength,pulOutputHeight,phOutputBitmap)	\
    ( (This)->lpVtbl -> ExtractThumbnail(This,pStg,ulLength,ulHeight,pulOutputLength,pulOutputHeight,phOutputBitmap) ) 

#define IThumbnailExtractor_OnFileUpdated(This,pStg)	\
    ( (This)->lpVtbl -> OnFileUpdated(This,pStg) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IThumbnailExtractor_INTERFACE_DEFINED__ */


#ifndef __IDummyHICONIncluder_INTERFACE_DEFINED__
#define __IDummyHICONIncluder_INTERFACE_DEFINED__

/* interface IDummyHICONIncluder */
/* [uuid][unique][object] */ 


EXTERN_C const IID IID_IDummyHICONIncluder;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("947990de-cc28-11d2-a0f7-00805f858fb1")
    IDummyHICONIncluder : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Dummy( 
            /* [in] */ __RPC__in HICON h1,
            /* [in] */ __RPC__in HDC h2) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDummyHICONIncluderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IDummyHICONIncluder * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IDummyHICONIncluder * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IDummyHICONIncluder * This);
        
        HRESULT ( STDMETHODCALLTYPE *Dummy )( 
            __RPC__in IDummyHICONIncluder * This,
            /* [in] */ __RPC__in HICON h1,
            /* [in] */ __RPC__in HDC h2);
        
        END_INTERFACE
    } IDummyHICONIncluderVtbl;

    interface IDummyHICONIncluder
    {
        CONST_VTBL struct IDummyHICONIncluderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDummyHICONIncluder_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IDummyHICONIncluder_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IDummyHICONIncluder_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IDummyHICONIncluder_Dummy(This,h1,h2)	\
    ( (This)->lpVtbl -> Dummy(This,h1,h2) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IDummyHICONIncluder_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_objidl_0000_0076 */
/* [local] */ 

#if defined USE_COM_CONTEXT_DEF || defined BUILDTYPE_COMSVCS || defined _OLE32_
typedef DWORD CPFLAGS;

typedef struct tagContextProperty
    {
    GUID policyId;
    CPFLAGS flags;
    IUnknown *pUnk;
    } 	ContextProperty;



extern RPC_IF_HANDLE __MIDL_itf_objidl_0000_0076_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_objidl_0000_0076_v0_0_s_ifspec;

#ifndef __IEnumContextProps_INTERFACE_DEFINED__
#define __IEnumContextProps_INTERFACE_DEFINED__

/* interface IEnumContextProps */
/* [unique][uuid][object][local] */ 

typedef /* [unique] */ IEnumContextProps *LPENUMCONTEXTPROPS;


EXTERN_C const IID IID_IEnumContextProps;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("000001c1-0000-0000-C000-000000000046")
    IEnumContextProps : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [annotation][in] */ 
            __in  ULONG celt,
            /* [annotation][length_is][size_is][out] */ 
            __out_ecount_part(celt, *pceltFetched)  ContextProperty *pContextProperties,
            /* [annotation][out] */ 
            __out  ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [annotation][in] */ 
            __in  ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [annotation][out] */ 
            __deref_out  IEnumContextProps **ppEnumContextProps) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Count( 
            /* [annotation][out] */ 
            __out  ULONG *pcelt) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumContextPropsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumContextProps * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumContextProps * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumContextProps * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumContextProps * This,
            /* [annotation][in] */ 
            __in  ULONG celt,
            /* [annotation][length_is][size_is][out] */ 
            __out_ecount_part(celt, *pceltFetched)  ContextProperty *pContextProperties,
            /* [annotation][out] */ 
            __out  ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumContextProps * This,
            /* [annotation][in] */ 
            __in  ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumContextProps * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumContextProps * This,
            /* [annotation][out] */ 
            __deref_out  IEnumContextProps **ppEnumContextProps);
        
        HRESULT ( STDMETHODCALLTYPE *Count )( 
            IEnumContextProps * This,
            /* [annotation][out] */ 
            __out  ULONG *pcelt);
        
        END_INTERFACE
    } IEnumContextPropsVtbl;

    interface IEnumContextProps
    {
        CONST_VTBL struct IEnumContextPropsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumContextProps_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IEnumContextProps_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IEnumContextProps_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IEnumContextProps_Next(This,celt,pContextProperties,pceltFetched)	\
    ( (This)->lpVtbl -> Next(This,celt,pContextProperties,pceltFetched) ) 

#define IEnumContextProps_Skip(This,celt)	\
    ( (This)->lpVtbl -> Skip(This,celt) ) 

#define IEnumContextProps_Reset(This)	\
    ( (This)->lpVtbl -> Reset(This) ) 

#define IEnumContextProps_Clone(This,ppEnumContextProps)	\
    ( (This)->lpVtbl -> Clone(This,ppEnumContextProps) ) 

#define IEnumContextProps_Count(This,pcelt)	\
    ( (This)->lpVtbl -> Count(This,pcelt) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IEnumContextProps_INTERFACE_DEFINED__ */


#ifndef __IContext_INTERFACE_DEFINED__
#define __IContext_INTERFACE_DEFINED__

/* interface IContext */
/* [unique][uuid][object][local] */ 


EXTERN_C const IID IID_IContext;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("000001c0-0000-0000-C000-000000000046")
    IContext : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetProperty( 
            /* [annotation][in] */ 
            __in  REFGUID rpolicyId,
            /* [annotation][in] */ 
            __in  CPFLAGS flags,
            /* [annotation][in] */ 
            __in  IUnknown *pUnk) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveProperty( 
            /* [annotation][in] */ 
            __in  REFGUID rPolicyId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetProperty( 
            /* [annotation][in] */ 
            __in  REFGUID rGuid,
            /* [annotation][out] */ 
            __out  CPFLAGS *pFlags,
            /* [annotation][out] */ 
            __deref_out  IUnknown **ppUnk) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumContextProps( 
            /* [annotation][out] */ 
            __deref_out  IEnumContextProps **ppEnumContextProps) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IContextVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IContext * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IContext * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IContext * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetProperty )( 
            IContext * This,
            /* [annotation][in] */ 
            __in  REFGUID rpolicyId,
            /* [annotation][in] */ 
            __in  CPFLAGS flags,
            /* [annotation][in] */ 
            __in  IUnknown *pUnk);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveProperty )( 
            IContext * This,
            /* [annotation][in] */ 
            __in  REFGUID rPolicyId);
        
        HRESULT ( STDMETHODCALLTYPE *GetProperty )( 
            IContext * This,
            /* [annotation][in] */ 
            __in  REFGUID rGuid,
            /* [annotation][out] */ 
            __out  CPFLAGS *pFlags,
            /* [annotation][out] */ 
            __deref_out  IUnknown **ppUnk);
        
        HRESULT ( STDMETHODCALLTYPE *EnumContextProps )( 
            IContext * This,
            /* [annotation][out] */ 
            __deref_out  IEnumContextProps **ppEnumContextProps);
        
        END_INTERFACE
    } IContextVtbl;

    interface IContext
    {
        CONST_VTBL struct IContextVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IContext_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IContext_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IContext_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IContext_SetProperty(This,rpolicyId,flags,pUnk)	\
    ( (This)->lpVtbl -> SetProperty(This,rpolicyId,flags,pUnk) ) 

#define IContext_RemoveProperty(This,rPolicyId)	\
    ( (This)->lpVtbl -> RemoveProperty(This,rPolicyId) ) 

#define IContext_GetProperty(This,rGuid,pFlags,ppUnk)	\
    ( (This)->lpVtbl -> GetProperty(This,rGuid,pFlags,ppUnk) ) 

#define IContext_EnumContextProps(This,ppEnumContextProps)	\
    ( (This)->lpVtbl -> EnumContextProps(This,ppEnumContextProps) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IContext_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_objidl_0000_0078 */
/* [local] */ 

#if !defined BUILDTYPE_COMSVCS && !defined _OLE32_


extern RPC_IF_HANDLE __MIDL_itf_objidl_0000_0078_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_objidl_0000_0078_v0_0_s_ifspec;

#ifndef __IObjContext_INTERFACE_DEFINED__
#define __IObjContext_INTERFACE_DEFINED__

/* interface IObjContext */
/* [unique][uuid][object][local] */ 


EXTERN_C const IID IID_IObjContext;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("000001c6-0000-0000-C000-000000000046")
    IObjContext : public IContext
    {
    public:
        virtual void STDMETHODCALLTYPE Reserved1( void) = 0;
        
        virtual void STDMETHODCALLTYPE Reserved2( void) = 0;
        
        virtual void STDMETHODCALLTYPE Reserved3( void) = 0;
        
        virtual void STDMETHODCALLTYPE Reserved4( void) = 0;
        
        virtual void STDMETHODCALLTYPE Reserved5( void) = 0;
        
        virtual void STDMETHODCALLTYPE Reserved6( void) = 0;
        
        virtual void STDMETHODCALLTYPE Reserved7( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IObjContextVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IObjContext * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IObjContext * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IObjContext * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetProperty )( 
            IObjContext * This,
            /* [annotation][in] */ 
            __in  REFGUID rpolicyId,
            /* [annotation][in] */ 
            __in  CPFLAGS flags,
            /* [annotation][in] */ 
            __in  IUnknown *pUnk);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveProperty )( 
            IObjContext * This,
            /* [annotation][in] */ 
            __in  REFGUID rPolicyId);
        
        HRESULT ( STDMETHODCALLTYPE *GetProperty )( 
            IObjContext * This,
            /* [annotation][in] */ 
            __in  REFGUID rGuid,
            /* [annotation][out] */ 
            __out  CPFLAGS *pFlags,
            /* [annotation][out] */ 
            __deref_out  IUnknown **ppUnk);
        
        HRESULT ( STDMETHODCALLTYPE *EnumContextProps )( 
            IObjContext * This,
            /* [annotation][out] */ 
            __deref_out  IEnumContextProps **ppEnumContextProps);
        
        void ( STDMETHODCALLTYPE *Reserved1 )( 
            IObjContext * This);
        
        void ( STDMETHODCALLTYPE *Reserved2 )( 
            IObjContext * This);
        
        void ( STDMETHODCALLTYPE *Reserved3 )( 
            IObjContext * This);
        
        void ( STDMETHODCALLTYPE *Reserved4 )( 
            IObjContext * This);
        
        void ( STDMETHODCALLTYPE *Reserved5 )( 
            IObjContext * This);
        
        void ( STDMETHODCALLTYPE *Reserved6 )( 
            IObjContext * This);
        
        void ( STDMETHODCALLTYPE *Reserved7 )( 
            IObjContext * This);
        
        END_INTERFACE
    } IObjContextVtbl;

    interface IObjContext
    {
        CONST_VTBL struct IObjContextVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IObjContext_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IObjContext_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IObjContext_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IObjContext_SetProperty(This,rpolicyId,flags,pUnk)	\
    ( (This)->lpVtbl -> SetProperty(This,rpolicyId,flags,pUnk) ) 

#define IObjContext_RemoveProperty(This,rPolicyId)	\
    ( (This)->lpVtbl -> RemoveProperty(This,rPolicyId) ) 

#define IObjContext_GetProperty(This,rGuid,pFlags,ppUnk)	\
    ( (This)->lpVtbl -> GetProperty(This,rGuid,pFlags,ppUnk) ) 

#define IObjContext_EnumContextProps(This,ppEnumContextProps)	\
    ( (This)->lpVtbl -> EnumContextProps(This,ppEnumContextProps) ) 


#define IObjContext_Reserved1(This)	\
    ( (This)->lpVtbl -> Reserved1(This) ) 

#define IObjContext_Reserved2(This)	\
    ( (This)->lpVtbl -> Reserved2(This) ) 

#define IObjContext_Reserved3(This)	\
    ( (This)->lpVtbl -> Reserved3(This) ) 

#define IObjContext_Reserved4(This)	\
    ( (This)->lpVtbl -> Reserved4(This) ) 

#define IObjContext_Reserved5(This)	\
    ( (This)->lpVtbl -> Reserved5(This) ) 

#define IObjContext_Reserved6(This)	\
    ( (This)->lpVtbl -> Reserved6(This) ) 

#define IObjContext_Reserved7(This)	\
    ( (This)->lpVtbl -> Reserved7(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IObjContext_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_objidl_0000_0079 */
/* [local] */ 

#endif
#endif
typedef 
enum tagApplicationType
    {	ServerApplication	= 0,
	LibraryApplication	= ( ServerApplication + 1 ) 
    } 	ApplicationType;

typedef 
enum tagShutdownType
    {	IdleShutdown	= 0,
	ForcedShutdown	= ( IdleShutdown + 1 ) 
    } 	ShutdownType;



extern RPC_IF_HANDLE __MIDL_itf_objidl_0000_0079_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_objidl_0000_0079_v0_0_s_ifspec;

#ifndef __IProcessLock_INTERFACE_DEFINED__
#define __IProcessLock_INTERFACE_DEFINED__

/* interface IProcessLock */
/* [unique][uuid][local][object] */ 


EXTERN_C const IID IID_IProcessLock;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("000001d5-0000-0000-C000-000000000046")
    IProcessLock : public IUnknown
    {
    public:
        virtual ULONG STDMETHODCALLTYPE AddRefOnProcess( void) = 0;
        
        virtual ULONG STDMETHODCALLTYPE ReleaseRefOnProcess( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IProcessLockVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IProcessLock * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IProcessLock * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IProcessLock * This);
        
        ULONG ( STDMETHODCALLTYPE *AddRefOnProcess )( 
            IProcessLock * This);
        
        ULONG ( STDMETHODCALLTYPE *ReleaseRefOnProcess )( 
            IProcessLock * This);
        
        END_INTERFACE
    } IProcessLockVtbl;

    interface IProcessLock
    {
        CONST_VTBL struct IProcessLockVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IProcessLock_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IProcessLock_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IProcessLock_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IProcessLock_AddRefOnProcess(This)	\
    ( (This)->lpVtbl -> AddRefOnProcess(This) ) 

#define IProcessLock_ReleaseRefOnProcess(This)	\
    ( (This)->lpVtbl -> ReleaseRefOnProcess(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IProcessLock_INTERFACE_DEFINED__ */


#ifndef __ISurrogateService_INTERFACE_DEFINED__
#define __ISurrogateService_INTERFACE_DEFINED__

/* interface ISurrogateService */
/* [unique][uuid][local][object] */ 


EXTERN_C const IID IID_ISurrogateService;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("000001d4-0000-0000-C000-000000000046")
    ISurrogateService : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Init( 
            /* [annotation][in] */ 
            __in  REFGUID rguidProcessID,
            /* [annotation][in] */ 
            __in  IProcessLock *pProcessLock,
            /* [annotation][out] */ 
            __out  BOOL *pfApplicationAware) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ApplicationLaunch( 
            /* [annotation][in] */ 
            __in  REFGUID rguidApplID,
            /* [annotation][in] */ 
            __in  ApplicationType appType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ApplicationFree( 
            /* [annotation][in] */ 
            __in  REFGUID rguidApplID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CatalogRefresh( 
            /* [annotation][in] */ 
            __reserved  ULONG ulReserved) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ProcessShutdown( 
            /* [annotation][in] */ 
            __in  ShutdownType shutdownType) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISurrogateServiceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISurrogateService * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISurrogateService * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISurrogateService * This);
        
        HRESULT ( STDMETHODCALLTYPE *Init )( 
            ISurrogateService * This,
            /* [annotation][in] */ 
            __in  REFGUID rguidProcessID,
            /* [annotation][in] */ 
            __in  IProcessLock *pProcessLock,
            /* [annotation][out] */ 
            __out  BOOL *pfApplicationAware);
        
        HRESULT ( STDMETHODCALLTYPE *ApplicationLaunch )( 
            ISurrogateService * This,
            /* [annotation][in] */ 
            __in  REFGUID rguidApplID,
            /* [annotation][in] */ 
            __in  ApplicationType appType);
        
        HRESULT ( STDMETHODCALLTYPE *ApplicationFree )( 
            ISurrogateService * This,
            /* [annotation][in] */ 
            __in  REFGUID rguidApplID);
        
        HRESULT ( STDMETHODCALLTYPE *CatalogRefresh )( 
            ISurrogateService * This,
            /* [annotation][in] */ 
            __reserved  ULONG ulReserved);
        
        HRESULT ( STDMETHODCALLTYPE *ProcessShutdown )( 
            ISurrogateService * This,
            /* [annotation][in] */ 
            __in  ShutdownType shutdownType);
        
        END_INTERFACE
    } ISurrogateServiceVtbl;

    interface ISurrogateService
    {
        CONST_VTBL struct ISurrogateServiceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISurrogateService_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISurrogateService_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISurrogateService_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISurrogateService_Init(This,rguidProcessID,pProcessLock,pfApplicationAware)	\
    ( (This)->lpVtbl -> Init(This,rguidProcessID,pProcessLock,pfApplicationAware) ) 

#define ISurrogateService_ApplicationLaunch(This,rguidApplID,appType)	\
    ( (This)->lpVtbl -> ApplicationLaunch(This,rguidApplID,appType) ) 

#define ISurrogateService_ApplicationFree(This,rguidApplID)	\
    ( (This)->lpVtbl -> ApplicationFree(This,rguidApplID) ) 

#define ISurrogateService_CatalogRefresh(This,ulReserved)	\
    ( (This)->lpVtbl -> CatalogRefresh(This,ulReserved) ) 

#define ISurrogateService_ProcessShutdown(This,shutdownType)	\
    ( (This)->lpVtbl -> ProcessShutdown(This,shutdownType) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISurrogateService_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_objidl_0000_0081 */
/* [local] */ 

typedef 
enum _APTTYPEQUALIFIER
    {	APTTYPEQUALIFIER_NONE	= 0,
	APTTYPEQUALIFIER_IMPLICIT_MTA	= 1,
	APTTYPEQUALIFIER_NA_ON_MTA	= 2,
	APTTYPEQUALIFIER_NA_ON_STA	= 3,
	APTTYPEQUALIFIER_NA_ON_IMPLICIT_MTA	= 4,
	APTTYPEQUALIFIER_NA_ON_MAINSTA	= 5
    } 	APTTYPEQUALIFIER;

typedef 
enum _APTTYPE
    {	APTTYPE_CURRENT	= -1,
	APTTYPE_STA	= 0,
	APTTYPE_MTA	= 1,
	APTTYPE_NA	= 2,
	APTTYPE_MAINSTA	= 3
    } 	APTTYPE;

typedef 
enum _THDTYPE
    {	THDTYPE_BLOCKMESSAGES	= 0,
	THDTYPE_PROCESSMESSAGES	= 1
    } 	THDTYPE;

typedef DWORD APARTMENTID;



extern RPC_IF_HANDLE __MIDL_itf_objidl_0000_0081_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_objidl_0000_0081_v0_0_s_ifspec;

#ifndef __IComThreadingInfo_INTERFACE_DEFINED__
#define __IComThreadingInfo_INTERFACE_DEFINED__

/* interface IComThreadingInfo */
/* [unique][uuid][object][local] */ 


EXTERN_C const IID IID_IComThreadingInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("000001ce-0000-0000-C000-000000000046")
    IComThreadingInfo : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetCurrentApartmentType( 
            /* [annotation][out] */ 
            __out  APTTYPE *pAptType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCurrentThreadType( 
            /* [annotation][out] */ 
            __out  THDTYPE *pThreadType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCurrentLogicalThreadId( 
            /* [annotation][out] */ 
            __out  GUID *pguidLogicalThreadId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetCurrentLogicalThreadId( 
            /* [annotation][in] */ 
            __in  REFGUID rguid) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IComThreadingInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IComThreadingInfo * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IComThreadingInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IComThreadingInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetCurrentApartmentType )( 
            IComThreadingInfo * This,
            /* [annotation][out] */ 
            __out  APTTYPE *pAptType);
        
        HRESULT ( STDMETHODCALLTYPE *GetCurrentThreadType )( 
            IComThreadingInfo * This,
            /* [annotation][out] */ 
            __out  THDTYPE *pThreadType);
        
        HRESULT ( STDMETHODCALLTYPE *GetCurrentLogicalThreadId )( 
            IComThreadingInfo * This,
            /* [annotation][out] */ 
            __out  GUID *pguidLogicalThreadId);
        
        HRESULT ( STDMETHODCALLTYPE *SetCurrentLogicalThreadId )( 
            IComThreadingInfo * This,
            /* [annotation][in] */ 
            __in  REFGUID rguid);
        
        END_INTERFACE
    } IComThreadingInfoVtbl;

    interface IComThreadingInfo
    {
        CONST_VTBL struct IComThreadingInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IComThreadingInfo_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IComThreadingInfo_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IComThreadingInfo_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IComThreadingInfo_GetCurrentApartmentType(This,pAptType)	\
    ( (This)->lpVtbl -> GetCurrentApartmentType(This,pAptType) ) 

#define IComThreadingInfo_GetCurrentThreadType(This,pThreadType)	\
    ( (This)->lpVtbl -> GetCurrentThreadType(This,pThreadType) ) 

#define IComThreadingInfo_GetCurrentLogicalThreadId(This,pguidLogicalThreadId)	\
    ( (This)->lpVtbl -> GetCurrentLogicalThreadId(This,pguidLogicalThreadId) ) 

#define IComThreadingInfo_SetCurrentLogicalThreadId(This,rguid)	\
    ( (This)->lpVtbl -> SetCurrentLogicalThreadId(This,rguid) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IComThreadingInfo_INTERFACE_DEFINED__ */


#ifndef __IProcessInitControl_INTERFACE_DEFINED__
#define __IProcessInitControl_INTERFACE_DEFINED__

/* interface IProcessInitControl */
/* [uuid][unique][object] */ 


EXTERN_C const IID IID_IProcessInitControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("72380d55-8d2b-43a3-8513-2b6ef31434e9")
    IProcessInitControl : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ResetInitializerTimeout( 
            /* [in] */ DWORD dwSecondsRemaining) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IProcessInitControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IProcessInitControl * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IProcessInitControl * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IProcessInitControl * This);
        
        HRESULT ( STDMETHODCALLTYPE *ResetInitializerTimeout )( 
            __RPC__in IProcessInitControl * This,
            /* [in] */ DWORD dwSecondsRemaining);
        
        END_INTERFACE
    } IProcessInitControlVtbl;

    interface IProcessInitControl
    {
        CONST_VTBL struct IProcessInitControlVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IProcessInitControl_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IProcessInitControl_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IProcessInitControl_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IProcessInitControl_ResetInitializerTimeout(This,dwSecondsRemaining)	\
    ( (This)->lpVtbl -> ResetInitializerTimeout(This,dwSecondsRemaining) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IProcessInitControl_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_objidl_0000_0083 */
/* [local] */ 

#if  (_WIN32_WINNT >= 0x0501 )


extern RPC_IF_HANDLE __MIDL_itf_objidl_0000_0083_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_objidl_0000_0083_v0_0_s_ifspec;

#ifndef __IInitializeSpy_INTERFACE_DEFINED__
#define __IInitializeSpy_INTERFACE_DEFINED__

/* interface IInitializeSpy */
/* [unique][uuid][object][local] */ 

typedef /* [unique] */ IInitializeSpy *LPINITIALIZESPY;


EXTERN_C const IID IID_IInitializeSpy;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000034-0000-0000-C000-000000000046")
    IInitializeSpy : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE PreInitialize( 
            /* [annotation][in] */ 
            __in  DWORD dwCoInit,
            /* [annotation][in] */ 
            __in  DWORD dwCurThreadAptRefs) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PostInitialize( 
            /* [annotation][in] */ 
            __in  HRESULT hrCoInit,
            /* [annotation][in] */ 
            __in  DWORD dwCoInit,
            /* [annotation][in] */ 
            __in  DWORD dwNewThreadAptRefs) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PreUninitialize( 
            /* [annotation][in] */ 
            __in  DWORD dwCurThreadAptRefs) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PostUninitialize( 
            /* [annotation][in] */ 
            __in  DWORD dwNewThreadAptRefs) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IInitializeSpyVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IInitializeSpy * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IInitializeSpy * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IInitializeSpy * This);
        
        HRESULT ( STDMETHODCALLTYPE *PreInitialize )( 
            IInitializeSpy * This,
            /* [annotation][in] */ 
            __in  DWORD dwCoInit,
            /* [annotation][in] */ 
            __in  DWORD dwCurThreadAptRefs);
        
        HRESULT ( STDMETHODCALLTYPE *PostInitialize )( 
            IInitializeSpy * This,
            /* [annotation][in] */ 
            __in  HRESULT hrCoInit,
            /* [annotation][in] */ 
            __in  DWORD dwCoInit,
            /* [annotation][in] */ 
            __in  DWORD dwNewThreadAptRefs);
        
        HRESULT ( STDMETHODCALLTYPE *PreUninitialize )( 
            IInitializeSpy * This,
            /* [annotation][in] */ 
            __in  DWORD dwCurThreadAptRefs);
        
        HRESULT ( STDMETHODCALLTYPE *PostUninitialize )( 
            IInitializeSpy * This,
            /* [annotation][in] */ 
            __in  DWORD dwNewThreadAptRefs);
        
        END_INTERFACE
    } IInitializeSpyVtbl;

    interface IInitializeSpy
    {
        CONST_VTBL struct IInitializeSpyVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IInitializeSpy_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IInitializeSpy_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IInitializeSpy_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IInitializeSpy_PreInitialize(This,dwCoInit,dwCurThreadAptRefs)	\
    ( (This)->lpVtbl -> PreInitialize(This,dwCoInit,dwCurThreadAptRefs) ) 

#define IInitializeSpy_PostInitialize(This,hrCoInit,dwCoInit,dwNewThreadAptRefs)	\
    ( (This)->lpVtbl -> PostInitialize(This,hrCoInit,dwCoInit,dwNewThreadAptRefs) ) 

#define IInitializeSpy_PreUninitialize(This,dwCurThreadAptRefs)	\
    ( (This)->lpVtbl -> PreUninitialize(This,dwCurThreadAptRefs) ) 

#define IInitializeSpy_PostUninitialize(This,dwNewThreadAptRefs)	\
    ( (This)->lpVtbl -> PostUninitialize(This,dwNewThreadAptRefs) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IInitializeSpy_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_objidl_0000_0084 */
/* [local] */ 

#endif
#if ( _MSC_VER >= 800 )
#if _MSC_VER >= 1200
#pragma warning(pop)
#else
#pragma warning(default:4201)
#endif
#endif


extern RPC_IF_HANDLE __MIDL_itf_objidl_0000_0084_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_objidl_0000_0084_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  ASYNC_STGMEDIUM_UserSize(     __RPC__in unsigned long *, unsigned long            , __RPC__in ASYNC_STGMEDIUM * ); 
unsigned char * __RPC_USER  ASYNC_STGMEDIUM_UserMarshal(  __RPC__in unsigned long *, __RPC__inout_xcount(0) unsigned char *, __RPC__in ASYNC_STGMEDIUM * ); 
unsigned char * __RPC_USER  ASYNC_STGMEDIUM_UserUnmarshal(__RPC__in unsigned long *, __RPC__in_xcount(0) unsigned char *, __RPC__out ASYNC_STGMEDIUM * ); 
void                      __RPC_USER  ASYNC_STGMEDIUM_UserFree(     __RPC__in unsigned long *, __RPC__in ASYNC_STGMEDIUM * ); 

unsigned long             __RPC_USER  CLIPFORMAT_UserSize(     __RPC__in unsigned long *, unsigned long            , __RPC__in CLIPFORMAT * ); 
unsigned char * __RPC_USER  CLIPFORMAT_UserMarshal(  __RPC__in unsigned long *, __RPC__inout_xcount(0) unsigned char *, __RPC__in CLIPFORMAT * ); 
unsigned char * __RPC_USER  CLIPFORMAT_UserUnmarshal(__RPC__in unsigned long *, __RPC__in_xcount(0) unsigned char *, __RPC__out CLIPFORMAT * ); 
void                      __RPC_USER  CLIPFORMAT_UserFree(     __RPC__in unsigned long *, __RPC__in CLIPFORMAT * ); 

unsigned long             __RPC_USER  FLAG_STGMEDIUM_UserSize(     __RPC__in unsigned long *, unsigned long            , __RPC__in FLAG_STGMEDIUM * ); 
unsigned char * __RPC_USER  FLAG_STGMEDIUM_UserMarshal(  __RPC__in unsigned long *, __RPC__inout_xcount(0) unsigned char *, __RPC__in FLAG_STGMEDIUM * ); 
unsigned char * __RPC_USER  FLAG_STGMEDIUM_UserUnmarshal(__RPC__in unsigned long *, __RPC__in_xcount(0) unsigned char *, __RPC__out FLAG_STGMEDIUM * ); 
void                      __RPC_USER  FLAG_STGMEDIUM_UserFree(     __RPC__in unsigned long *, __RPC__in FLAG_STGMEDIUM * ); 

unsigned long             __RPC_USER  HBITMAP_UserSize(     __RPC__in unsigned long *, unsigned long            , __RPC__in HBITMAP * ); 
unsigned char * __RPC_USER  HBITMAP_UserMarshal(  __RPC__in unsigned long *, __RPC__inout_xcount(0) unsigned char *, __RPC__in HBITMAP * ); 
unsigned char * __RPC_USER  HBITMAP_UserUnmarshal(__RPC__in unsigned long *, __RPC__in_xcount(0) unsigned char *, __RPC__out HBITMAP * ); 
void                      __RPC_USER  HBITMAP_UserFree(     __RPC__in unsigned long *, __RPC__in HBITMAP * ); 

unsigned long             __RPC_USER  HDC_UserSize(     __RPC__in unsigned long *, unsigned long            , __RPC__in HDC * ); 
unsigned char * __RPC_USER  HDC_UserMarshal(  __RPC__in unsigned long *, __RPC__inout_xcount(0) unsigned char *, __RPC__in HDC * ); 
unsigned char * __RPC_USER  HDC_UserUnmarshal(__RPC__in unsigned long *, __RPC__in_xcount(0) unsigned char *, __RPC__out HDC * ); 
void                      __RPC_USER  HDC_UserFree(     __RPC__in unsigned long *, __RPC__in HDC * ); 

unsigned long             __RPC_USER  HICON_UserSize(     __RPC__in unsigned long *, unsigned long            , __RPC__in HICON * ); 
unsigned char * __RPC_USER  HICON_UserMarshal(  __RPC__in unsigned long *, __RPC__inout_xcount(0) unsigned char *, __RPC__in HICON * ); 
unsigned char * __RPC_USER  HICON_UserUnmarshal(__RPC__in unsigned long *, __RPC__in_xcount(0) unsigned char *, __RPC__out HICON * ); 
void                      __RPC_USER  HICON_UserFree(     __RPC__in unsigned long *, __RPC__in HICON * ); 

unsigned long             __RPC_USER  SNB_UserSize(     __RPC__in unsigned long *, unsigned long            , __RPC__in SNB * ); 
unsigned char * __RPC_USER  SNB_UserMarshal(  __RPC__in unsigned long *, __RPC__inout_xcount(0) unsigned char *, __RPC__in SNB * ); 
unsigned char * __RPC_USER  SNB_UserUnmarshal(__RPC__in unsigned long *, __RPC__in_xcount(0) unsigned char *, __RPC__out SNB * ); 
void                      __RPC_USER  SNB_UserFree(     __RPC__in unsigned long *, __RPC__in SNB * ); 

unsigned long             __RPC_USER  STGMEDIUM_UserSize(     __RPC__in unsigned long *, unsigned long            , __RPC__in STGMEDIUM * ); 
unsigned char * __RPC_USER  STGMEDIUM_UserMarshal(  __RPC__in unsigned long *, __RPC__inout_xcount(0) unsigned char *, __RPC__in STGMEDIUM * ); 
unsigned char * __RPC_USER  STGMEDIUM_UserUnmarshal(__RPC__in unsigned long *, __RPC__in_xcount(0) unsigned char *, __RPC__out STGMEDIUM * ); 
void                      __RPC_USER  STGMEDIUM_UserFree(     __RPC__in unsigned long *, __RPC__in STGMEDIUM * ); 

unsigned long             __RPC_USER  ASYNC_STGMEDIUM_UserSize64(     __RPC__in unsigned long *, unsigned long            , __RPC__in ASYNC_STGMEDIUM * ); 
unsigned char * __RPC_USER  ASYNC_STGMEDIUM_UserMarshal64(  __RPC__in unsigned long *, __RPC__inout_xcount(0) unsigned char *, __RPC__in ASYNC_STGMEDIUM * ); 
unsigned char * __RPC_USER  ASYNC_STGMEDIUM_UserUnmarshal64(__RPC__in unsigned long *, __RPC__in_xcount(0) unsigned char *, __RPC__out ASYNC_STGMEDIUM * ); 
void                      __RPC_USER  ASYNC_STGMEDIUM_UserFree64(     __RPC__in unsigned long *, __RPC__in ASYNC_STGMEDIUM * ); 

unsigned long             __RPC_USER  CLIPFORMAT_UserSize64(     __RPC__in unsigned long *, unsigned long            , __RPC__in CLIPFORMAT * ); 
unsigned char * __RPC_USER  CLIPFORMAT_UserMarshal64(  __RPC__in unsigned long *, __RPC__inout_xcount(0) unsigned char *, __RPC__in CLIPFORMAT * ); 
unsigned char * __RPC_USER  CLIPFORMAT_UserUnmarshal64(__RPC__in unsigned long *, __RPC__in_xcount(0) unsigned char *, __RPC__out CLIPFORMAT * ); 
void                      __RPC_USER  CLIPFORMAT_UserFree64(     __RPC__in unsigned long *, __RPC__in CLIPFORMAT * ); 

unsigned long             __RPC_USER  FLAG_STGMEDIUM_UserSize64(     __RPC__in unsigned long *, unsigned long            , __RPC__in FLAG_STGMEDIUM * ); 
unsigned char * __RPC_USER  FLAG_STGMEDIUM_UserMarshal64(  __RPC__in unsigned long *, __RPC__inout_xcount(0) unsigned char *, __RPC__in FLAG_STGMEDIUM * ); 
unsigned char * __RPC_USER  FLAG_STGMEDIUM_UserUnmarshal64(__RPC__in unsigned long *, __RPC__in_xcount(0) unsigned char *, __RPC__out FLAG_STGMEDIUM * ); 
void                      __RPC_USER  FLAG_STGMEDIUM_UserFree64(     __RPC__in unsigned long *, __RPC__in FLAG_STGMEDIUM * ); 

unsigned long             __RPC_USER  HBITMAP_UserSize64(     __RPC__in unsigned long *, unsigned long            , __RPC__in HBITMAP * ); 
unsigned char * __RPC_USER  HBITMAP_UserMarshal64(  __RPC__in unsigned long *, __RPC__inout_xcount(0) unsigned char *, __RPC__in HBITMAP * ); 
unsigned char * __RPC_USER  HBITMAP_UserUnmarshal64(__RPC__in unsigned long *, __RPC__in_xcount(0) unsigned char *, __RPC__out HBITMAP * ); 
void                      __RPC_USER  HBITMAP_UserFree64(     __RPC__in unsigned long *, __RPC__in HBITMAP * ); 

unsigned long             __RPC_USER  HDC_UserSize64(     __RPC__in unsigned long *, unsigned long            , __RPC__in HDC * ); 
unsigned char * __RPC_USER  HDC_UserMarshal64(  __RPC__in unsigned long *, __RPC__inout_xcount(0) unsigned char *, __RPC__in HDC * ); 
unsigned char * __RPC_USER  HDC_UserUnmarshal64(__RPC__in unsigned long *, __RPC__in_xcount(0) unsigned char *, __RPC__out HDC * ); 
void                      __RPC_USER  HDC_UserFree64(     __RPC__in unsigned long *, __RPC__in HDC * ); 

unsigned long             __RPC_USER  HICON_UserSize64(     __RPC__in unsigned long *, unsigned long            , __RPC__in HICON * ); 
unsigned char * __RPC_USER  HICON_UserMarshal64(  __RPC__in unsigned long *, __RPC__inout_xcount(0) unsigned char *, __RPC__in HICON * ); 
unsigned char * __RPC_USER  HICON_UserUnmarshal64(__RPC__in unsigned long *, __RPC__in_xcount(0) unsigned char *, __RPC__out HICON * ); 
void                      __RPC_USER  HICON_UserFree64(     __RPC__in unsigned long *, __RPC__in HICON * ); 

unsigned long             __RPC_USER  SNB_UserSize64(     __RPC__in unsigned long *, unsigned long            , __RPC__in SNB * ); 
unsigned char * __RPC_USER  SNB_UserMarshal64(  __RPC__in unsigned long *, __RPC__inout_xcount(0) unsigned char *, __RPC__in SNB * ); 
unsigned char * __RPC_USER  SNB_UserUnmarshal64(__RPC__in unsigned long *, __RPC__in_xcount(0) unsigned char *, __RPC__out SNB * ); 
void                      __RPC_USER  SNB_UserFree64(     __RPC__in unsigned long *, __RPC__in SNB * ); 

unsigned long             __RPC_USER  STGMEDIUM_UserSize64(     __RPC__in unsigned long *, unsigned long            , __RPC__in STGMEDIUM * ); 
unsigned char * __RPC_USER  STGMEDIUM_UserMarshal64(  __RPC__in unsigned long *, __RPC__inout_xcount(0) unsigned char *, __RPC__in STGMEDIUM * ); 
unsigned char * __RPC_USER  STGMEDIUM_UserUnmarshal64(__RPC__in unsigned long *, __RPC__in_xcount(0) unsigned char *, __RPC__out STGMEDIUM * ); 
void                      __RPC_USER  STGMEDIUM_UserFree64(     __RPC__in unsigned long *, __RPC__in STGMEDIUM * ); 

/* [local] */ HRESULT STDMETHODCALLTYPE IEnumUnknown_Next_Proxy( 
    IEnumUnknown * This,
    /* [annotation][in] */ 
    __in  ULONG celt,
    /* [annotation][out] */ 
    __out_ecount_part(celt,*pceltFetched)  IUnknown **rgelt,
    /* [annotation][out] */ 
    __out_opt  ULONG *pceltFetched);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IEnumUnknown_Next_Stub( 
    __RPC__in IEnumUnknown * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ __RPC__out_ecount_part(celt, *pceltFetched) IUnknown **rgelt,
    /* [out] */ __RPC__out ULONG *pceltFetched);

/* [local] */ HRESULT STDMETHODCALLTYPE IBindCtx_SetBindOptions_Proxy( 
    IBindCtx * This,
    /* [in] */ BIND_OPTS *pbindopts);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IBindCtx_SetBindOptions_Stub( 
    __RPC__in IBindCtx * This,
    /* [in] */ __RPC__in BIND_OPTS2 *pbindopts);

/* [local] */ HRESULT STDMETHODCALLTYPE IBindCtx_GetBindOptions_Proxy( 
    IBindCtx * This,
    /* [out][in] */ BIND_OPTS *pbindopts);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IBindCtx_GetBindOptions_Stub( 
    __RPC__in IBindCtx * This,
    /* [out][in] */ __RPC__inout BIND_OPTS2 *pbindopts);

/* [local] */ HRESULT STDMETHODCALLTYPE IEnumMoniker_Next_Proxy( 
    IEnumMoniker * This,
    /* [in] */ ULONG celt,
    /* [annotation] */ 
    __out_ecount_part(celt,*pceltFetched)  IMoniker **rgelt,
    /* [annotation] */ 
    __out_opt  ULONG *pceltFetched);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IEnumMoniker_Next_Stub( 
    __RPC__in IEnumMoniker * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ __RPC__out_ecount_part(celt, *pceltFetched) IMoniker **rgelt,
    /* [out] */ __RPC__out ULONG *pceltFetched);

/* [local] */ BOOL STDMETHODCALLTYPE IRunnableObject_IsRunning_Proxy( 
    IRunnableObject * This);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IRunnableObject_IsRunning_Stub( 
    __RPC__in IRunnableObject * This);

/* [local] */ HRESULT STDMETHODCALLTYPE IMoniker_BindToObject_Proxy( 
    IMoniker * This,
    /* [unique][in] */ IBindCtx *pbc,
    /* [unique][in] */ IMoniker *pmkToLeft,
    /* [in] */ REFIID riidResult,
    /* [iid_is][out] */ void **ppvResult);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IMoniker_BindToObject_Stub( 
    __RPC__in IMoniker * This,
    /* [unique][in] */ __RPC__in_opt IBindCtx *pbc,
    /* [unique][in] */ __RPC__in_opt IMoniker *pmkToLeft,
    /* [in] */ __RPC__in REFIID riidResult,
    /* [iid_is][out] */ __RPC__deref_out_opt IUnknown **ppvResult);

/* [local] */ HRESULT STDMETHODCALLTYPE IMoniker_BindToStorage_Proxy( 
    IMoniker * This,
    /* [unique][in] */ IBindCtx *pbc,
    /* [unique][in] */ IMoniker *pmkToLeft,
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ void **ppvObj);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IMoniker_BindToStorage_Stub( 
    __RPC__in IMoniker * This,
    /* [unique][in] */ __RPC__in_opt IBindCtx *pbc,
    /* [unique][in] */ __RPC__in_opt IMoniker *pmkToLeft,
    /* [in] */ __RPC__in REFIID riid,
    /* [iid_is][out] */ __RPC__deref_out_opt IUnknown **ppvObj);

/* [local] */ HRESULT STDMETHODCALLTYPE IEnumString_Next_Proxy( 
    IEnumString * This,
    /* [in] */ ULONG celt,
    /* [annotation] */ 
    __RPC__out_ecount_part(celt,*pceltFetched)  LPOLESTR *rgelt,
    /* [annotation] */ 
    __out_opt  ULONG *pceltFetched);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IEnumString_Next_Stub( 
    __RPC__in IEnumString * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ __RPC__out_ecount_part(celt, *pceltFetched) LPOLESTR *rgelt,
    /* [out] */ __RPC__out ULONG *pceltFetched);

/* [local] */ HRESULT STDMETHODCALLTYPE ISequentialStream_Read_Proxy( 
    ISequentialStream * This,
    /* [annotation] */ 
    __out_bcount_part(cb, *pcbRead)  void *pv,
    /* [in] */ ULONG cb,
    /* [annotation] */ 
    __out_opt  ULONG *pcbRead);


/* [call_as] */ HRESULT STDMETHODCALLTYPE ISequentialStream_Read_Stub( 
    __RPC__in ISequentialStream * This,
    /* [length_is][size_is][out] */ __RPC__out_ecount_part(cb, *pcbRead) byte *pv,
    /* [in] */ ULONG cb,
    /* [out] */ __RPC__out ULONG *pcbRead);

/* [local] */ HRESULT STDMETHODCALLTYPE ISequentialStream_Write_Proxy( 
    ISequentialStream * This,
    /* [annotation] */ 
    __in_bcount(cb)  const void *pv,
    /* [in] */ ULONG cb,
    /* [annotation] */ 
    __out_opt  ULONG *pcbWritten);


/* [call_as] */ HRESULT STDMETHODCALLTYPE ISequentialStream_Write_Stub( 
    __RPC__in ISequentialStream * This,
    /* [size_is][in] */ __RPC__in_ecount_full(cb) const byte *pv,
    /* [in] */ ULONG cb,
    /* [out] */ __RPC__out ULONG *pcbWritten);

/* [local] */ HRESULT STDMETHODCALLTYPE IStream_Seek_Proxy( 
    IStream * This,
    /* [in] */ LARGE_INTEGER dlibMove,
    /* [in] */ DWORD dwOrigin,
    /* [annotation] */ 
    __out_opt  ULARGE_INTEGER *plibNewPosition);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IStream_Seek_Stub( 
    __RPC__in IStream * This,
    /* [in] */ LARGE_INTEGER dlibMove,
    /* [in] */ DWORD dwOrigin,
    /* [out] */ __RPC__out ULARGE_INTEGER *plibNewPosition);

/* [local] */ HRESULT STDMETHODCALLTYPE IStream_CopyTo_Proxy( 
    IStream * This,
    /* [unique][in] */ IStream *pstm,
    /* [in] */ ULARGE_INTEGER cb,
    /* [annotation] */ 
    __out_opt  ULARGE_INTEGER *pcbRead,
    /* [annotation] */ 
    __out_opt  ULARGE_INTEGER *pcbWritten);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IStream_CopyTo_Stub( 
    __RPC__in IStream * This,
    /* [unique][in] */ __RPC__in_opt IStream *pstm,
    /* [in] */ ULARGE_INTEGER cb,
    /* [out] */ __RPC__out ULARGE_INTEGER *pcbRead,
    /* [out] */ __RPC__out ULARGE_INTEGER *pcbWritten);

/* [local] */ HRESULT STDMETHODCALLTYPE IEnumSTATSTG_Next_Proxy( 
    IEnumSTATSTG * This,
    /* [in] */ ULONG celt,
    /* [annotation] */ 
    __out_ecount_part(celt,*pceltFetched)  STATSTG *rgelt,
    /* [annotation] */ 
    __out_opt  ULONG *pceltFetched);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IEnumSTATSTG_Next_Stub( 
    __RPC__in IEnumSTATSTG * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ __RPC__out_ecount_part(celt, *pceltFetched) STATSTG *rgelt,
    /* [out] */ __RPC__out ULONG *pceltFetched);

/* [local] */ HRESULT STDMETHODCALLTYPE IStorage_OpenStream_Proxy( 
    IStorage * This,
    /* [string][in] */ const OLECHAR *pwcsName,
    /* [unique][in] */ void *reserved1,
    /* [in] */ DWORD grfMode,
    /* [in] */ DWORD reserved2,
    /* [out] */ IStream **ppstm);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IStorage_OpenStream_Stub( 
    __RPC__in IStorage * This,
    /* [string][in] */ __RPC__in_string const OLECHAR *pwcsName,
    /* [in] */ unsigned long cbReserved1,
    /* [size_is][unique][in] */ __RPC__in_ecount_full_opt(cbReserved1) byte *reserved1,
    /* [in] */ DWORD grfMode,
    /* [in] */ DWORD reserved2,
    /* [out] */ __RPC__deref_out_opt IStream **ppstm);

/* [local] */ HRESULT STDMETHODCALLTYPE IStorage_CopyTo_Proxy( 
    IStorage * This,
    /* [in] */ DWORD ciidExclude,
    /* [size_is][unique][in] */ const IID *rgiidExclude,
    /* [annotation][unique][in] */ 
    __RPC__in_opt  SNB snbExclude,
    /* [unique][in] */ IStorage *pstgDest);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IStorage_CopyTo_Stub( 
    __RPC__in IStorage * This,
    /* [in] */ DWORD ciidExclude,
    /* [size_is][unique][in] */ __RPC__in_ecount_full_opt(ciidExclude) const IID *rgiidExclude,
    /* [unique][in] */ __RPC__deref_opt_in_opt SNB snbExclude,
    /* [unique][in] */ __RPC__in_opt IStorage *pstgDest);

/* [local] */ HRESULT STDMETHODCALLTYPE IStorage_EnumElements_Proxy( 
    IStorage * This,
    /* [in] */ DWORD reserved1,
    /* [size_is][unique][in] */ void *reserved2,
    /* [in] */ DWORD reserved3,
    /* [out] */ IEnumSTATSTG **ppenum);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IStorage_EnumElements_Stub( 
    __RPC__in IStorage * This,
    /* [in] */ DWORD reserved1,
    /* [in] */ unsigned long cbReserved2,
    /* [size_is][unique][in] */ __RPC__in_ecount_full_opt(cbReserved2) byte *reserved2,
    /* [in] */ DWORD reserved3,
    /* [out] */ __RPC__deref_out_opt IEnumSTATSTG **ppenum);

/* [local] */ HRESULT STDMETHODCALLTYPE ILockBytes_ReadAt_Proxy( 
    ILockBytes * This,
    /* [in] */ ULARGE_INTEGER ulOffset,
    /* [annotation][length_is][size_is][out] */ 
    __out_bcount_part(cb, *pcbRead)  void *pv,
    /* [in] */ ULONG cb,
    /* [annotation][out] */ 
    __out_opt  ULONG *pcbRead);


/* [call_as] */ HRESULT __stdcall ILockBytes_ReadAt_Stub( 
    __RPC__in ILockBytes * This,
    /* [in] */ ULARGE_INTEGER ulOffset,
    /* [length_is][size_is][out] */ __RPC__out_ecount_part(cb, *pcbRead) byte *pv,
    /* [in] */ ULONG cb,
    /* [out] */ __RPC__out ULONG *pcbRead);

/* [local] */ HRESULT STDMETHODCALLTYPE ILockBytes_WriteAt_Proxy( 
    ILockBytes * This,
    /* [in] */ ULARGE_INTEGER ulOffset,
    /* [annotation][size_is][in] */ 
    __in_bcount(cb)  const void *pv,
    /* [in] */ ULONG cb,
    /* [annotation][out] */ 
    __out_opt  ULONG *pcbWritten);


/* [call_as] */ HRESULT STDMETHODCALLTYPE ILockBytes_WriteAt_Stub( 
    __RPC__in ILockBytes * This,
    /* [in] */ ULARGE_INTEGER ulOffset,
    /* [size_is][in] */ __RPC__in_ecount_full(cb) const byte *pv,
    /* [in] */ ULONG cb,
    /* [out] */ __RPC__out ULONG *pcbWritten);

/* [local] */ HRESULT STDMETHODCALLTYPE IEnumFORMATETC_Next_Proxy( 
    IEnumFORMATETC * This,
    /* [in] */ ULONG celt,
    /* [annotation] */ 
    __out_ecount_part(celt,*pceltFetched)  FORMATETC *rgelt,
    /* [annotation] */ 
    __out_opt  ULONG *pceltFetched);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IEnumFORMATETC_Next_Stub( 
    __RPC__in IEnumFORMATETC * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ __RPC__out_ecount_part(celt, *pceltFetched) FORMATETC *rgelt,
    /* [out] */ __RPC__out ULONG *pceltFetched);

/* [local] */ HRESULT STDMETHODCALLTYPE IEnumSTATDATA_Next_Proxy( 
    IEnumSTATDATA * This,
    /* [in] */ ULONG celt,
    /* [annotation] */ 
    __out_ecount_part(celt,*pceltFetched)  STATDATA *rgelt,
    /* [annotation] */ 
    __out_opt  ULONG *pceltFetched);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IEnumSTATDATA_Next_Stub( 
    __RPC__in IEnumSTATDATA * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ __RPC__out_ecount_part(celt, *pceltFetched) STATDATA *rgelt,
    /* [out] */ __RPC__out ULONG *pceltFetched);

/* [local] */ void STDMETHODCALLTYPE IAdviseSink_OnDataChange_Proxy( 
    IAdviseSink * This,
    /* [unique][in] */ FORMATETC *pFormatetc,
    /* [unique][in] */ STGMEDIUM *pStgmed);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IAdviseSink_OnDataChange_Stub( 
    __RPC__in IAdviseSink * This,
    /* [unique][in] */ __RPC__in_opt FORMATETC *pFormatetc,
    /* [unique][in] */ __RPC__in_opt ASYNC_STGMEDIUM *pStgmed);

/* [local] */ void STDMETHODCALLTYPE IAdviseSink_OnViewChange_Proxy( 
    IAdviseSink * This,
    /* [in] */ DWORD dwAspect,
    /* [in] */ LONG lindex);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IAdviseSink_OnViewChange_Stub( 
    __RPC__in IAdviseSink * This,
    /* [in] */ DWORD dwAspect,
    /* [in] */ LONG lindex);

/* [local] */ void STDMETHODCALLTYPE IAdviseSink_OnRename_Proxy( 
    IAdviseSink * This,
    /* [in] */ IMoniker *pmk);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IAdviseSink_OnRename_Stub( 
    __RPC__in IAdviseSink * This,
    /* [in] */ __RPC__in_opt IMoniker *pmk);

/* [local] */ void STDMETHODCALLTYPE IAdviseSink_OnSave_Proxy( 
    IAdviseSink * This);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IAdviseSink_OnSave_Stub( 
    __RPC__in IAdviseSink * This);

/* [local] */ void STDMETHODCALLTYPE IAdviseSink_OnClose_Proxy( 
    IAdviseSink * This);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IAdviseSink_OnClose_Stub( 
    __RPC__in IAdviseSink * This);

/* [local] */ void STDMETHODCALLTYPE IAdviseSink2_OnLinkSrcChange_Proxy( 
    IAdviseSink2 * This,
    /* [unique][in] */ IMoniker *pmk);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IAdviseSink2_OnLinkSrcChange_Stub( 
    __RPC__in IAdviseSink2 * This,
    /* [unique][in] */ __RPC__in_opt IMoniker *pmk);

/* [local] */ HRESULT STDMETHODCALLTYPE IDataObject_GetData_Proxy( 
    IDataObject * This,
    /* [unique][in] */ FORMATETC *pformatetcIn,
    /* [out] */ STGMEDIUM *pmedium);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IDataObject_GetData_Stub( 
    __RPC__in IDataObject * This,
    /* [unique][in] */ __RPC__in_opt FORMATETC *pformatetcIn,
    /* [out] */ __RPC__out STGMEDIUM *pRemoteMedium);

/* [local] */ HRESULT STDMETHODCALLTYPE IDataObject_GetDataHere_Proxy( 
    IDataObject * This,
    /* [unique][in] */ FORMATETC *pformatetc,
    /* [out][in] */ STGMEDIUM *pmedium);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IDataObject_GetDataHere_Stub( 
    __RPC__in IDataObject * This,
    /* [unique][in] */ __RPC__in_opt FORMATETC *pformatetc,
    /* [out][in] */ __RPC__inout STGMEDIUM *pRemoteMedium);

/* [local] */ HRESULT STDMETHODCALLTYPE IDataObject_SetData_Proxy( 
    IDataObject * This,
    /* [unique][in] */ FORMATETC *pformatetc,
    /* [unique][in] */ STGMEDIUM *pmedium,
    /* [in] */ BOOL fRelease);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IDataObject_SetData_Stub( 
    __RPC__in IDataObject * This,
    /* [unique][in] */ __RPC__in_opt FORMATETC *pformatetc,
    /* [unique][in] */ __RPC__in_opt FLAG_STGMEDIUM *pmedium,
    /* [in] */ BOOL fRelease);

/* [local] */ HRESULT STDMETHODCALLTYPE IFillLockBytes_FillAppend_Proxy( 
    IFillLockBytes * This,
    /* [annotation][size_is][in] */ 
    __in_bcount(cb)  const void *pv,
    /* [annotation][in] */ 
    __in  ULONG cb,
    /* [annotation][out] */ 
    __out  ULONG *pcbWritten);


/* [call_as] */ HRESULT __stdcall IFillLockBytes_FillAppend_Stub( 
    __RPC__in IFillLockBytes * This,
    /* [size_is][in] */ __RPC__in_ecount_full(cb) const byte *pv,
    /* [in] */ ULONG cb,
    /* [out] */ __RPC__out ULONG *pcbWritten);

/* [local] */ HRESULT STDMETHODCALLTYPE IFillLockBytes_FillAt_Proxy( 
    IFillLockBytes * This,
    /* [annotation][in] */ 
    __in  ULARGE_INTEGER ulOffset,
    /* [annotation][size_is][in] */ 
    __in_bcount(cb)  const void *pv,
    /* [annotation][in] */ 
    __in  ULONG cb,
    /* [annotation][out] */ 
    __out  ULONG *pcbWritten);


/* [call_as] */ HRESULT __stdcall IFillLockBytes_FillAt_Stub( 
    __RPC__in IFillLockBytes * This,
    /* [in] */ ULARGE_INTEGER ulOffset,
    /* [size_is][in] */ __RPC__in_ecount_full(cb) const byte *pv,
    /* [in] */ ULONG cb,
    /* [out] */ __RPC__out ULONG *pcbWritten);

/* [local] */ void STDMETHODCALLTYPE AsyncIAdviseSink_Begin_OnDataChange_Proxy( 
    AsyncIAdviseSink * This,
    /* [unique][in] */ FORMATETC *pFormatetc,
    /* [unique][in] */ STGMEDIUM *pStgmed);


/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink_Begin_OnDataChange_Stub( 
    __RPC__in AsyncIAdviseSink * This,
    /* [unique][in] */ __RPC__in_opt FORMATETC *pFormatetc,
    /* [unique][in] */ __RPC__in_opt ASYNC_STGMEDIUM *pStgmed);

/* [local] */ void STDMETHODCALLTYPE AsyncIAdviseSink_Finish_OnDataChange_Proxy( 
    AsyncIAdviseSink * This);


/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink_Finish_OnDataChange_Stub( 
    __RPC__in AsyncIAdviseSink * This);

/* [local] */ void STDMETHODCALLTYPE AsyncIAdviseSink_Begin_OnViewChange_Proxy( 
    AsyncIAdviseSink * This,
    /* [in] */ DWORD dwAspect,
    /* [in] */ LONG lindex);


/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink_Begin_OnViewChange_Stub( 
    __RPC__in AsyncIAdviseSink * This,
    /* [in] */ DWORD dwAspect,
    /* [in] */ LONG lindex);

/* [local] */ void STDMETHODCALLTYPE AsyncIAdviseSink_Finish_OnViewChange_Proxy( 
    AsyncIAdviseSink * This);


/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink_Finish_OnViewChange_Stub( 
    __RPC__in AsyncIAdviseSink * This);

/* [local] */ void STDMETHODCALLTYPE AsyncIAdviseSink_Begin_OnRename_Proxy( 
    AsyncIAdviseSink * This,
    /* [in] */ IMoniker *pmk);


/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink_Begin_OnRename_Stub( 
    __RPC__in AsyncIAdviseSink * This,
    /* [in] */ __RPC__in_opt IMoniker *pmk);

/* [local] */ void STDMETHODCALLTYPE AsyncIAdviseSink_Finish_OnRename_Proxy( 
    AsyncIAdviseSink * This);


/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink_Finish_OnRename_Stub( 
    __RPC__in AsyncIAdviseSink * This);

/* [local] */ void STDMETHODCALLTYPE AsyncIAdviseSink_Begin_OnSave_Proxy( 
    AsyncIAdviseSink * This);


/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink_Begin_OnSave_Stub( 
    __RPC__in AsyncIAdviseSink * This);

/* [local] */ void STDMETHODCALLTYPE AsyncIAdviseSink_Finish_OnSave_Proxy( 
    AsyncIAdviseSink * This);


/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink_Finish_OnSave_Stub( 
    __RPC__in AsyncIAdviseSink * This);

/* [local] */ void STDMETHODCALLTYPE AsyncIAdviseSink_Begin_OnClose_Proxy( 
    AsyncIAdviseSink * This);


/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink_Begin_OnClose_Stub( 
    __RPC__in AsyncIAdviseSink * This);

/* [local] */ void STDMETHODCALLTYPE AsyncIAdviseSink_Finish_OnClose_Proxy( 
    AsyncIAdviseSink * This);


/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink_Finish_OnClose_Stub( 
    __RPC__in AsyncIAdviseSink * This);

/* [local] */ void STDMETHODCALLTYPE AsyncIAdviseSink2_Begin_OnLinkSrcChange_Proxy( 
    AsyncIAdviseSink2 * This,
    /* [unique][in] */ IMoniker *pmk);


/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink2_Begin_OnLinkSrcChange_Stub( 
    __RPC__in AsyncIAdviseSink2 * This,
    /* [unique][in] */ __RPC__in_opt IMoniker *pmk);

/* [local] */ void STDMETHODCALLTYPE AsyncIAdviseSink2_Finish_OnLinkSrcChange_Proxy( 
    AsyncIAdviseSink2 * This);


/* [call_as] */ HRESULT STDMETHODCALLTYPE AsyncIAdviseSink2_Finish_OnLinkSrcChange_Stub( 
    __RPC__in AsyncIAdviseSink2 * This);



/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



