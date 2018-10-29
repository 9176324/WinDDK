

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* Compiler settings for xpsrassvc.idl:
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

#ifndef __xpsrassvc_h__
#define __xpsrassvc_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IXpsRasterizerNotificationCallback_FWD_DEFINED__
#define __IXpsRasterizerNotificationCallback_FWD_DEFINED__
typedef interface IXpsRasterizerNotificationCallback IXpsRasterizerNotificationCallback;
#endif 	/* __IXpsRasterizerNotificationCallback_FWD_DEFINED__ */


#ifndef __IXpsRasterizer_FWD_DEFINED__
#define __IXpsRasterizer_FWD_DEFINED__
typedef interface IXpsRasterizer IXpsRasterizer;
#endif 	/* __IXpsRasterizer_FWD_DEFINED__ */


#ifndef __IXpsRasterizationFactory_FWD_DEFINED__
#define __IXpsRasterizationFactory_FWD_DEFINED__
typedef interface IXpsRasterizationFactory IXpsRasterizationFactory;
#endif 	/* __IXpsRasterizationFactory_FWD_DEFINED__ */


/* header files for imported files */
#include "wincodec.h"
#include "XpsObjectModel.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_xpsrassvc_0000_0000 */
/* [local] */ 

//+--------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation. All rights reserved.
//
//----------------------------------------------------------------------------


extern RPC_IF_HANDLE __MIDL_itf_xpsrassvc_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_xpsrassvc_0000_0000_v0_0_s_ifspec;

#ifndef __IXpsRasterizerNotificationCallback_INTERFACE_DEFINED__
#define __IXpsRasterizerNotificationCallback_INTERFACE_DEFINED__

/* interface IXpsRasterizerNotificationCallback */
/* [ref][helpstring][nonextensible][uuid][object] */ 


EXTERN_C const IID IID_IXpsRasterizerNotificationCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9AB8FD0D-CB94-49c2-9CB0-97EC1D5469D2")
    IXpsRasterizerNotificationCallback : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Continue( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IXpsRasterizerNotificationCallbackVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IXpsRasterizerNotificationCallback * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IXpsRasterizerNotificationCallback * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IXpsRasterizerNotificationCallback * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Continue )( 
            __RPC__in IXpsRasterizerNotificationCallback * This);
        
        END_INTERFACE
    } IXpsRasterizerNotificationCallbackVtbl;

    interface IXpsRasterizerNotificationCallback
    {
        CONST_VTBL struct IXpsRasterizerNotificationCallbackVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IXpsRasterizerNotificationCallback_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IXpsRasterizerNotificationCallback_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IXpsRasterizerNotificationCallback_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IXpsRasterizerNotificationCallback_Continue(This)	\
    ( (This)->lpVtbl -> Continue(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IXpsRasterizerNotificationCallback_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_xpsrassvc_0000_0001 */
/* [local] */ 

typedef /* [public][public][public] */ 
enum __MIDL___MIDL_itf_xpsrassvc_0000_0001_0001
    {	XPSRAS_RENDERING_MODE_ANTIALIASED	= 0,
	XPSRAS_RENDERING_MODE_ALIASED	= 1
    } 	XPSRAS_RENDERING_MODE;



extern RPC_IF_HANDLE __MIDL_itf_xpsrassvc_0000_0001_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_xpsrassvc_0000_0001_v0_0_s_ifspec;

#ifndef __IXpsRasterizer_INTERFACE_DEFINED__
#define __IXpsRasterizer_INTERFACE_DEFINED__

/* interface IXpsRasterizer */
/* [ref][helpstring][nonextensible][uuid][object] */ 


EXTERN_C const IID IID_IXpsRasterizer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7567CFC8-C156-47a8-9DAC-11A2AE5BDD6B")
    IXpsRasterizer : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE RasterizeRect( 
            /* [in] */ INT x,
            /* [in] */ INT y,
            /* [in] */ INT width,
            /* [in] */ INT height,
            /* [in] */ __RPC__in_opt IXpsRasterizerNotificationCallback *notificationCallback,
            /* [out] */ __RPC__deref_out_opt IWICBitmap **bitmap) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetMinimalLineWidth( 
            /* [in] */ INT width) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IXpsRasterizerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IXpsRasterizer * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IXpsRasterizer * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IXpsRasterizer * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *RasterizeRect )( 
            __RPC__in IXpsRasterizer * This,
            /* [in] */ INT x,
            /* [in] */ INT y,
            /* [in] */ INT width,
            /* [in] */ INT height,
            /* [in] */ __RPC__in_opt IXpsRasterizerNotificationCallback *notificationCallback,
            /* [out] */ __RPC__deref_out_opt IWICBitmap **bitmap);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetMinimalLineWidth )( 
            __RPC__in IXpsRasterizer * This,
            /* [in] */ INT width);
        
        END_INTERFACE
    } IXpsRasterizerVtbl;

    interface IXpsRasterizer
    {
        CONST_VTBL struct IXpsRasterizerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IXpsRasterizer_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IXpsRasterizer_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IXpsRasterizer_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IXpsRasterizer_RasterizeRect(This,x,y,width,height,notificationCallback,bitmap)	\
    ( (This)->lpVtbl -> RasterizeRect(This,x,y,width,height,notificationCallback,bitmap) ) 

#define IXpsRasterizer_SetMinimalLineWidth(This,width)	\
    ( (This)->lpVtbl -> SetMinimalLineWidth(This,width) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IXpsRasterizer_INTERFACE_DEFINED__ */


#ifndef __IXpsRasterizationFactory_INTERFACE_DEFINED__
#define __IXpsRasterizationFactory_INTERFACE_DEFINED__

/* interface IXpsRasterizationFactory */
/* [ref][helpstring][nonextensible][uuid][object] */ 


EXTERN_C const IID IID_IXpsRasterizationFactory;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E094808A-24C6-482b-A3A7-C21AC9B55F17")
    IXpsRasterizationFactory : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE CreateRasterizer( 
            /* [in] */ __RPC__in_opt IXpsOMPage *xpsPage,
            /* [in] */ FLOAT DPI,
            /* [in] */ XPSRAS_RENDERING_MODE nonTextRenderingMode,
            /* [in] */ XPSRAS_RENDERING_MODE textRenderingMode,
            /* [out] */ __RPC__deref_out_opt IXpsRasterizer **ppIXPSRasterizer) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IXpsRasterizationFactoryVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IXpsRasterizationFactory * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IXpsRasterizationFactory * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IXpsRasterizationFactory * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *CreateRasterizer )( 
            __RPC__in IXpsRasterizationFactory * This,
            /* [in] */ __RPC__in_opt IXpsOMPage *xpsPage,
            /* [in] */ FLOAT DPI,
            /* [in] */ XPSRAS_RENDERING_MODE nonTextRenderingMode,
            /* [in] */ XPSRAS_RENDERING_MODE textRenderingMode,
            /* [out] */ __RPC__deref_out_opt IXpsRasterizer **ppIXPSRasterizer);
        
        END_INTERFACE
    } IXpsRasterizationFactoryVtbl;

    interface IXpsRasterizationFactory
    {
        CONST_VTBL struct IXpsRasterizationFactoryVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IXpsRasterizationFactory_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IXpsRasterizationFactory_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IXpsRasterizationFactory_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IXpsRasterizationFactory_CreateRasterizer(This,xpsPage,DPI,nonTextRenderingMode,textRenderingMode,ppIXPSRasterizer)	\
    ( (This)->lpVtbl -> CreateRasterizer(This,xpsPage,DPI,nonTextRenderingMode,textRenderingMode,ppIXPSRasterizer) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IXpsRasterizationFactory_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



