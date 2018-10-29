

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* Compiler settings for structuredquerycondition.idl:
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
#define __REQUIRED_RPCNDR_H_VERSION__ 475
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

#ifndef __structuredquerycondition_h__
#define __structuredquerycondition_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IRichChunk_FWD_DEFINED__
#define __IRichChunk_FWD_DEFINED__
typedef interface IRichChunk IRichChunk;
#endif 	/* __IRichChunk_FWD_DEFINED__ */


#ifndef __ICondition_FWD_DEFINED__
#define __ICondition_FWD_DEFINED__
typedef interface ICondition ICondition;
#endif 	/* __ICondition_FWD_DEFINED__ */


#ifndef __ICondition2_FWD_DEFINED__
#define __ICondition2_FWD_DEFINED__
typedef interface ICondition2 ICondition2;
#endif 	/* __ICondition2_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "propidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_structuredquerycondition_0000_0000 */
/* [local] */ 

typedef /* [v1_enum] */ 
enum tagCONDITION_TYPE
    {	CT_AND_CONDITION	= 0,
	CT_OR_CONDITION	= ( CT_AND_CONDITION + 1 ) ,
	CT_NOT_CONDITION	= ( CT_OR_CONDITION + 1 ) ,
	CT_LEAF_CONDITION	= ( CT_NOT_CONDITION + 1 ) 
    } 	CONDITION_TYPE;

typedef /* [v1_enum] */ 
enum tagCONDITION_OPERATION
    {	COP_IMPLICIT	= 0,
	COP_EQUAL	= ( COP_IMPLICIT + 1 ) ,
	COP_NOTEQUAL	= ( COP_EQUAL + 1 ) ,
	COP_LESSTHAN	= ( COP_NOTEQUAL + 1 ) ,
	COP_GREATERTHAN	= ( COP_LESSTHAN + 1 ) ,
	COP_LESSTHANOREQUAL	= ( COP_GREATERTHAN + 1 ) ,
	COP_GREATERTHANOREQUAL	= ( COP_LESSTHANOREQUAL + 1 ) ,
	COP_VALUE_STARTSWITH	= ( COP_GREATERTHANOREQUAL + 1 ) ,
	COP_VALUE_ENDSWITH	= ( COP_VALUE_STARTSWITH + 1 ) ,
	COP_VALUE_CONTAINS	= ( COP_VALUE_ENDSWITH + 1 ) ,
	COP_VALUE_NOTCONTAINS	= ( COP_VALUE_CONTAINS + 1 ) ,
	COP_DOSWILDCARDS	= ( COP_VALUE_NOTCONTAINS + 1 ) ,
	COP_WORD_EQUAL	= ( COP_DOSWILDCARDS + 1 ) ,
	COP_WORD_STARTSWITH	= ( COP_WORD_EQUAL + 1 ) ,
	COP_APPLICATION_SPECIFIC	= ( COP_WORD_STARTSWITH + 1 ) 
    } 	CONDITION_OPERATION;



extern RPC_IF_HANDLE __MIDL_itf_structuredquerycondition_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_structuredquerycondition_0000_0000_v0_0_s_ifspec;

#ifndef __IRichChunk_INTERFACE_DEFINED__
#define __IRichChunk_INTERFACE_DEFINED__

/* interface IRichChunk */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IRichChunk;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4FDEF69C-DBC9-454e-9910-B34F3C64B510")
    IRichChunk : public IUnknown
    {
    public:
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE GetData( 
            /* [annotation][unique][out] */ 
            __out_opt  ULONG *pFirstPos,
            /* [annotation][unique][out] */ 
            __out_opt  ULONG *pLength,
            /* [annotation][unique][out] */ 
            __deref_opt_out_opt  LPWSTR *ppsz,
            /* [annotation][unique][out] */ 
            __out_opt  PROPVARIANT *pValue) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IRichChunkVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IRichChunk * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IRichChunk * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IRichChunk * This);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *GetData )( 
            IRichChunk * This,
            /* [annotation][unique][out] */ 
            __out_opt  ULONG *pFirstPos,
            /* [annotation][unique][out] */ 
            __out_opt  ULONG *pLength,
            /* [annotation][unique][out] */ 
            __deref_opt_out_opt  LPWSTR *ppsz,
            /* [annotation][unique][out] */ 
            __out_opt  PROPVARIANT *pValue);
        
        END_INTERFACE
    } IRichChunkVtbl;

    interface IRichChunk
    {
        CONST_VTBL struct IRichChunkVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRichChunk_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IRichChunk_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IRichChunk_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IRichChunk_GetData(This,pFirstPos,pLength,ppsz,pValue)	\
    ( (This)->lpVtbl -> GetData(This,pFirstPos,pLength,ppsz,pValue) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT STDMETHODCALLTYPE IRichChunk_RemoteGetData_Proxy( 
    __RPC__in IRichChunk * This,
    /* [out] */ __RPC__out ULONG *pFirstPos,
    /* [out] */ __RPC__out ULONG *pLength,
    /* [out] */ __RPC__deref_out_opt LPWSTR *ppsz,
    /* [out] */ __RPC__out PROPVARIANT *pValue);


void __RPC_STUB IRichChunk_RemoteGetData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IRichChunk_INTERFACE_DEFINED__ */


#ifndef __ICondition_INTERFACE_DEFINED__
#define __ICondition_INTERFACE_DEFINED__

/* interface ICondition */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_ICondition;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0FC988D4-C935-4b97-A973-46282EA175C8")
    ICondition : public IPersistStream
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetConditionType( 
            /* [retval][out] */ __RPC__out CONDITION_TYPE *pNodeType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSubConditions( 
            /* [in] */ __RPC__in REFIID riid,
            /* [iid_is][retval][out] */ __RPC__deref_out_opt void **ppv) = 0;
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE GetComparisonInfo( 
            /* [annotation][unique][out] */ 
            __deref_opt_out_opt  LPWSTR *ppszPropertyName,
            /* [annotation][unique][out] */ 
            __out_opt  CONDITION_OPERATION *pcop,
            /* [annotation][unique][out] */ 
            __out_opt  PROPVARIANT *ppropvar) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetValueType( 
            /* [retval][out] */ __RPC__deref_out_opt LPWSTR *ppszValueTypeName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetValueNormalization( 
            /* [retval][out] */ __RPC__deref_out_opt LPWSTR *ppszNormalization) = 0;
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE GetInputTerms( 
            /* [annotation][unique][out] */ 
            __out_opt  IRichChunk **ppPropertyTerm,
            /* [annotation][unique][out] */ 
            __out_opt  IRichChunk **ppOperationTerm,
            /* [annotation][unique][out] */ 
            __out_opt  IRichChunk **ppValueTerm) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [retval][out] */ __RPC__deref_out_opt ICondition **ppc) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IConditionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in ICondition * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in ICondition * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in ICondition * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetClassID )( 
            __RPC__in ICondition * This,
            /* [out] */ __RPC__out CLSID *pClassID);
        
        HRESULT ( STDMETHODCALLTYPE *IsDirty )( 
            __RPC__in ICondition * This);
        
        HRESULT ( STDMETHODCALLTYPE *Load )( 
            __RPC__in ICondition * This,
            /* [unique][in] */ __RPC__in_opt IStream *pStm);
        
        HRESULT ( STDMETHODCALLTYPE *Save )( 
            __RPC__in ICondition * This,
            /* [unique][in] */ __RPC__in_opt IStream *pStm,
            /* [in] */ BOOL fClearDirty);
        
        HRESULT ( STDMETHODCALLTYPE *GetSizeMax )( 
            __RPC__in ICondition * This,
            /* [out] */ __RPC__out ULARGE_INTEGER *pcbSize);
        
        HRESULT ( STDMETHODCALLTYPE *GetConditionType )( 
            __RPC__in ICondition * This,
            /* [retval][out] */ __RPC__out CONDITION_TYPE *pNodeType);
        
        HRESULT ( STDMETHODCALLTYPE *GetSubConditions )( 
            __RPC__in ICondition * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [iid_is][retval][out] */ __RPC__deref_out_opt void **ppv);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *GetComparisonInfo )( 
            ICondition * This,
            /* [annotation][unique][out] */ 
            __deref_opt_out_opt  LPWSTR *ppszPropertyName,
            /* [annotation][unique][out] */ 
            __out_opt  CONDITION_OPERATION *pcop,
            /* [annotation][unique][out] */ 
            __out_opt  PROPVARIANT *ppropvar);
        
        HRESULT ( STDMETHODCALLTYPE *GetValueType )( 
            __RPC__in ICondition * This,
            /* [retval][out] */ __RPC__deref_out_opt LPWSTR *ppszValueTypeName);
        
        HRESULT ( STDMETHODCALLTYPE *GetValueNormalization )( 
            __RPC__in ICondition * This,
            /* [retval][out] */ __RPC__deref_out_opt LPWSTR *ppszNormalization);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *GetInputTerms )( 
            ICondition * This,
            /* [annotation][unique][out] */ 
            __out_opt  IRichChunk **ppPropertyTerm,
            /* [annotation][unique][out] */ 
            __out_opt  IRichChunk **ppOperationTerm,
            /* [annotation][unique][out] */ 
            __out_opt  IRichChunk **ppValueTerm);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            __RPC__in ICondition * This,
            /* [retval][out] */ __RPC__deref_out_opt ICondition **ppc);
        
        END_INTERFACE
    } IConditionVtbl;

    interface ICondition
    {
        CONST_VTBL struct IConditionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICondition_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICondition_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICondition_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICondition_GetClassID(This,pClassID)	\
    ( (This)->lpVtbl -> GetClassID(This,pClassID) ) 


#define ICondition_IsDirty(This)	\
    ( (This)->lpVtbl -> IsDirty(This) ) 

#define ICondition_Load(This,pStm)	\
    ( (This)->lpVtbl -> Load(This,pStm) ) 

#define ICondition_Save(This,pStm,fClearDirty)	\
    ( (This)->lpVtbl -> Save(This,pStm,fClearDirty) ) 

#define ICondition_GetSizeMax(This,pcbSize)	\
    ( (This)->lpVtbl -> GetSizeMax(This,pcbSize) ) 


#define ICondition_GetConditionType(This,pNodeType)	\
    ( (This)->lpVtbl -> GetConditionType(This,pNodeType) ) 

#define ICondition_GetSubConditions(This,riid,ppv)	\
    ( (This)->lpVtbl -> GetSubConditions(This,riid,ppv) ) 

#define ICondition_GetComparisonInfo(This,ppszPropertyName,pcop,ppropvar)	\
    ( (This)->lpVtbl -> GetComparisonInfo(This,ppszPropertyName,pcop,ppropvar) ) 

#define ICondition_GetValueType(This,ppszValueTypeName)	\
    ( (This)->lpVtbl -> GetValueType(This,ppszValueTypeName) ) 

#define ICondition_GetValueNormalization(This,ppszNormalization)	\
    ( (This)->lpVtbl -> GetValueNormalization(This,ppszNormalization) ) 

#define ICondition_GetInputTerms(This,ppPropertyTerm,ppOperationTerm,ppValueTerm)	\
    ( (This)->lpVtbl -> GetInputTerms(This,ppPropertyTerm,ppOperationTerm,ppValueTerm) ) 

#define ICondition_Clone(This,ppc)	\
    ( (This)->lpVtbl -> Clone(This,ppc) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT STDMETHODCALLTYPE ICondition_RemoteGetComparisonInfo_Proxy( 
    __RPC__in ICondition * This,
    /* [out] */ __RPC__deref_out_opt LPWSTR *ppszPropertyName,
    /* [out] */ __RPC__out CONDITION_OPERATION *pcop,
    /* [out] */ __RPC__out PROPVARIANT *ppropvar);


void __RPC_STUB ICondition_RemoteGetComparisonInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT STDMETHODCALLTYPE ICondition_RemoteGetInputTerms_Proxy( 
    __RPC__in ICondition * This,
    /* [out] */ __RPC__deref_out_opt IRichChunk **ppPropertyTerm,
    /* [out] */ __RPC__deref_out_opt IRichChunk **ppOperationTerm,
    /* [out] */ __RPC__deref_out_opt IRichChunk **ppValueTerm);


void __RPC_STUB ICondition_RemoteGetInputTerms_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICondition_INTERFACE_DEFINED__ */


#ifndef __ICondition2_INTERFACE_DEFINED__
#define __ICondition2_INTERFACE_DEFINED__

/* interface ICondition2 */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_ICondition2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0DB8851D-2E5B-47eb-9208-D28C325A01D7")
    ICondition2 : public ICondition
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetLocale( 
            /* [out] */ __RPC__deref_out_opt LPWSTR *ppszLocaleName) = 0;
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE GetLeafConditionInfo( 
            /* [annotation][out] */ 
            __out_opt  PROPERTYKEY *ppropkey,
            /* [annotation][out] */ 
            __out_opt  CONDITION_OPERATION *pcop,
            /* [annotation][out] */ 
            __out_opt  PROPVARIANT *ppropvar) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICondition2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in ICondition2 * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in ICondition2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in ICondition2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetClassID )( 
            __RPC__in ICondition2 * This,
            /* [out] */ __RPC__out CLSID *pClassID);
        
        HRESULT ( STDMETHODCALLTYPE *IsDirty )( 
            __RPC__in ICondition2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Load )( 
            __RPC__in ICondition2 * This,
            /* [unique][in] */ __RPC__in_opt IStream *pStm);
        
        HRESULT ( STDMETHODCALLTYPE *Save )( 
            __RPC__in ICondition2 * This,
            /* [unique][in] */ __RPC__in_opt IStream *pStm,
            /* [in] */ BOOL fClearDirty);
        
        HRESULT ( STDMETHODCALLTYPE *GetSizeMax )( 
            __RPC__in ICondition2 * This,
            /* [out] */ __RPC__out ULARGE_INTEGER *pcbSize);
        
        HRESULT ( STDMETHODCALLTYPE *GetConditionType )( 
            __RPC__in ICondition2 * This,
            /* [retval][out] */ __RPC__out CONDITION_TYPE *pNodeType);
        
        HRESULT ( STDMETHODCALLTYPE *GetSubConditions )( 
            __RPC__in ICondition2 * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [iid_is][retval][out] */ __RPC__deref_out_opt void **ppv);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *GetComparisonInfo )( 
            ICondition2 * This,
            /* [annotation][unique][out] */ 
            __deref_opt_out_opt  LPWSTR *ppszPropertyName,
            /* [annotation][unique][out] */ 
            __out_opt  CONDITION_OPERATION *pcop,
            /* [annotation][unique][out] */ 
            __out_opt  PROPVARIANT *ppropvar);
        
        HRESULT ( STDMETHODCALLTYPE *GetValueType )( 
            __RPC__in ICondition2 * This,
            /* [retval][out] */ __RPC__deref_out_opt LPWSTR *ppszValueTypeName);
        
        HRESULT ( STDMETHODCALLTYPE *GetValueNormalization )( 
            __RPC__in ICondition2 * This,
            /* [retval][out] */ __RPC__deref_out_opt LPWSTR *ppszNormalization);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *GetInputTerms )( 
            ICondition2 * This,
            /* [annotation][unique][out] */ 
            __out_opt  IRichChunk **ppPropertyTerm,
            /* [annotation][unique][out] */ 
            __out_opt  IRichChunk **ppOperationTerm,
            /* [annotation][unique][out] */ 
            __out_opt  IRichChunk **ppValueTerm);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            __RPC__in ICondition2 * This,
            /* [retval][out] */ __RPC__deref_out_opt ICondition **ppc);
        
        HRESULT ( STDMETHODCALLTYPE *GetLocale )( 
            __RPC__in ICondition2 * This,
            /* [out] */ __RPC__deref_out_opt LPWSTR *ppszLocaleName);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *GetLeafConditionInfo )( 
            ICondition2 * This,
            /* [annotation][out] */ 
            __out_opt  PROPERTYKEY *ppropkey,
            /* [annotation][out] */ 
            __out_opt  CONDITION_OPERATION *pcop,
            /* [annotation][out] */ 
            __out_opt  PROPVARIANT *ppropvar);
        
        END_INTERFACE
    } ICondition2Vtbl;

    interface ICondition2
    {
        CONST_VTBL struct ICondition2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICondition2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICondition2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICondition2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICondition2_GetClassID(This,pClassID)	\
    ( (This)->lpVtbl -> GetClassID(This,pClassID) ) 


#define ICondition2_IsDirty(This)	\
    ( (This)->lpVtbl -> IsDirty(This) ) 

#define ICondition2_Load(This,pStm)	\
    ( (This)->lpVtbl -> Load(This,pStm) ) 

#define ICondition2_Save(This,pStm,fClearDirty)	\
    ( (This)->lpVtbl -> Save(This,pStm,fClearDirty) ) 

#define ICondition2_GetSizeMax(This,pcbSize)	\
    ( (This)->lpVtbl -> GetSizeMax(This,pcbSize) ) 


#define ICondition2_GetConditionType(This,pNodeType)	\
    ( (This)->lpVtbl -> GetConditionType(This,pNodeType) ) 

#define ICondition2_GetSubConditions(This,riid,ppv)	\
    ( (This)->lpVtbl -> GetSubConditions(This,riid,ppv) ) 

#define ICondition2_GetComparisonInfo(This,ppszPropertyName,pcop,ppropvar)	\
    ( (This)->lpVtbl -> GetComparisonInfo(This,ppszPropertyName,pcop,ppropvar) ) 

#define ICondition2_GetValueType(This,ppszValueTypeName)	\
    ( (This)->lpVtbl -> GetValueType(This,ppszValueTypeName) ) 

#define ICondition2_GetValueNormalization(This,ppszNormalization)	\
    ( (This)->lpVtbl -> GetValueNormalization(This,ppszNormalization) ) 

#define ICondition2_GetInputTerms(This,ppPropertyTerm,ppOperationTerm,ppValueTerm)	\
    ( (This)->lpVtbl -> GetInputTerms(This,ppPropertyTerm,ppOperationTerm,ppValueTerm) ) 

#define ICondition2_Clone(This,ppc)	\
    ( (This)->lpVtbl -> Clone(This,ppc) ) 


#define ICondition2_GetLocale(This,ppszLocaleName)	\
    ( (This)->lpVtbl -> GetLocale(This,ppszLocaleName) ) 

#define ICondition2_GetLeafConditionInfo(This,ppropkey,pcop,ppropvar)	\
    ( (This)->lpVtbl -> GetLeafConditionInfo(This,ppropkey,pcop,ppropvar) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT STDMETHODCALLTYPE ICondition2_RemoteGetLeafConditionInfo_Proxy( 
    __RPC__in ICondition2 * This,
    /* [out] */ __RPC__out PROPERTYKEY *ppropkey,
    /* [out] */ __RPC__out CONDITION_OPERATION *pcop,
    /* [out] */ __RPC__out PROPVARIANT *ppropvar);


void __RPC_STUB ICondition2_RemoteGetLeafConditionInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICondition2_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     __RPC__in unsigned long *, unsigned long            , __RPC__in BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  __RPC__in unsigned long *, __RPC__inout_xcount(0) unsigned char *, __RPC__in BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(__RPC__in unsigned long *, __RPC__in_xcount(0) unsigned char *, __RPC__out BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     __RPC__in unsigned long *, __RPC__in BSTR * ); 

unsigned long             __RPC_USER  LPSAFEARRAY_UserSize(     __RPC__in unsigned long *, unsigned long            , __RPC__in LPSAFEARRAY * ); 
unsigned char * __RPC_USER  LPSAFEARRAY_UserMarshal(  __RPC__in unsigned long *, __RPC__inout_xcount(0) unsigned char *, __RPC__in LPSAFEARRAY * ); 
unsigned char * __RPC_USER  LPSAFEARRAY_UserUnmarshal(__RPC__in unsigned long *, __RPC__in_xcount(0) unsigned char *, __RPC__out LPSAFEARRAY * ); 
void                      __RPC_USER  LPSAFEARRAY_UserFree(     __RPC__in unsigned long *, __RPC__in LPSAFEARRAY * ); 

/* [local] */ HRESULT STDMETHODCALLTYPE IRichChunk_GetData_Proxy( 
    IRichChunk * This,
    /* [annotation][unique][out] */ 
    __out_opt  ULONG *pFirstPos,
    /* [annotation][unique][out] */ 
    __out_opt  ULONG *pLength,
    /* [annotation][unique][out] */ 
    __deref_opt_out_opt  LPWSTR *ppsz,
    /* [annotation][unique][out] */ 
    __out_opt  PROPVARIANT *pValue);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IRichChunk_GetData_Stub( 
    __RPC__in IRichChunk * This,
    /* [out] */ __RPC__out ULONG *pFirstPos,
    /* [out] */ __RPC__out ULONG *pLength,
    /* [out] */ __RPC__deref_out_opt LPWSTR *ppsz,
    /* [out] */ __RPC__out PROPVARIANT *pValue);

/* [local] */ HRESULT STDMETHODCALLTYPE ICondition_GetComparisonInfo_Proxy( 
    ICondition * This,
    /* [annotation][unique][out] */ 
    __deref_opt_out_opt  LPWSTR *ppszPropertyName,
    /* [annotation][unique][out] */ 
    __out_opt  CONDITION_OPERATION *pcop,
    /* [annotation][unique][out] */ 
    __out_opt  PROPVARIANT *ppropvar);


/* [call_as] */ HRESULT STDMETHODCALLTYPE ICondition_GetComparisonInfo_Stub( 
    __RPC__in ICondition * This,
    /* [out] */ __RPC__deref_out_opt LPWSTR *ppszPropertyName,
    /* [out] */ __RPC__out CONDITION_OPERATION *pcop,
    /* [out] */ __RPC__out PROPVARIANT *ppropvar);

/* [local] */ HRESULT STDMETHODCALLTYPE ICondition_GetInputTerms_Proxy( 
    ICondition * This,
    /* [annotation][unique][out] */ 
    __out_opt  IRichChunk **ppPropertyTerm,
    /* [annotation][unique][out] */ 
    __out_opt  IRichChunk **ppOperationTerm,
    /* [annotation][unique][out] */ 
    __out_opt  IRichChunk **ppValueTerm);


/* [call_as] */ HRESULT STDMETHODCALLTYPE ICondition_GetInputTerms_Stub( 
    __RPC__in ICondition * This,
    /* [out] */ __RPC__deref_out_opt IRichChunk **ppPropertyTerm,
    /* [out] */ __RPC__deref_out_opt IRichChunk **ppOperationTerm,
    /* [out] */ __RPC__deref_out_opt IRichChunk **ppValueTerm);

/* [local] */ HRESULT STDMETHODCALLTYPE ICondition2_GetLeafConditionInfo_Proxy( 
    ICondition2 * This,
    /* [annotation][out] */ 
    __out_opt  PROPERTYKEY *ppropkey,
    /* [annotation][out] */ 
    __out_opt  CONDITION_OPERATION *pcop,
    /* [annotation][out] */ 
    __out_opt  PROPVARIANT *ppropvar);


/* [call_as] */ HRESULT STDMETHODCALLTYPE ICondition2_GetLeafConditionInfo_Stub( 
    __RPC__in ICondition2 * This,
    /* [out] */ __RPC__out PROPERTYKEY *ppropkey,
    /* [out] */ __RPC__out CONDITION_OPERATION *pcop,
    /* [out] */ __RPC__out PROPVARIANT *ppropvar);



/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



