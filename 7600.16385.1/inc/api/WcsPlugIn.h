

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* Compiler settings for wcsplugin.idl:
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

#ifndef __wcsplugin_h__
#define __wcsplugin_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IDeviceModelPlugIn_FWD_DEFINED__
#define __IDeviceModelPlugIn_FWD_DEFINED__
typedef interface IDeviceModelPlugIn IDeviceModelPlugIn;
#endif 	/* __IDeviceModelPlugIn_FWD_DEFINED__ */


#ifndef __IGamutMapModelPlugIn_FWD_DEFINED__
#define __IGamutMapModelPlugIn_FWD_DEFINED__
typedef interface IGamutMapModelPlugIn IGamutMapModelPlugIn;
#endif 	/* __IGamutMapModelPlugIn_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_wcsplugin_0000_0000 */
/* [local] */ 

//+-------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//--------------------------------------------------------------------------
typedef struct _XYZColorF
    {
    FLOAT X;
    FLOAT Y;
    FLOAT Z;
    } 	XYZColorF;

typedef struct _JChColorF
    {
    FLOAT J;
    FLOAT C;
    FLOAT h;
    } 	JChColorF;

typedef struct _JabColorF
    {
    FLOAT J;
    FLOAT a;
    FLOAT b;
    } 	JabColorF;

typedef struct _GamutShellTriangle
    {
    UINT aVertexIndex[ 3 ];
    } 	GamutShellTriangle;

typedef struct _GamutShell
    {
    FLOAT JMin;
    FLOAT JMax;
    UINT cVertices;
    UINT cTriangles;
    JabColorF *pVertices;
    GamutShellTriangle *pTriangles;
    } 	GamutShell;

typedef struct _PrimaryJabColors
    {
    JabColorF red;
    JabColorF yellow;
    JabColorF green;
    JabColorF cyan;
    JabColorF blue;
    JabColorF magenta;
    JabColorF black;
    JabColorF white;
    } 	PrimaryJabColors;

typedef struct _PrimaryXYZColors
    {
    XYZColorF red;
    XYZColorF yellow;
    XYZColorF green;
    XYZColorF cyan;
    XYZColorF blue;
    XYZColorF magenta;
    XYZColorF black;
    XYZColorF white;
    } 	PrimaryXYZColors;

typedef struct _GamutBoundaryDescription
    {
    PrimaryJabColors *pPrimaries;
    UINT cNeutralSamples;
    JabColorF *pNeutralSamples;
    GamutShell *pReferenceShell;
    GamutShell *pPlausibleShell;
    GamutShell *pPossibleShell;
    } 	GamutBoundaryDescription;

typedef struct _BlackInformation
    {
    BOOL fBlackOnly;
    FLOAT blackWeight;
    } 	BlackInformation;



extern RPC_IF_HANDLE __MIDL_itf_wcsplugin_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_wcsplugin_0000_0000_v0_0_s_ifspec;

#ifndef __IDeviceModelPlugIn_INTERFACE_DEFINED__
#define __IDeviceModelPlugIn_INTERFACE_DEFINED__

/* interface IDeviceModelPlugIn */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IDeviceModelPlugIn;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1CD63475-07C4-46FE-A903-D655316D11FD")
    IDeviceModelPlugIn : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ __RPC__in BSTR bstrXml,
            /* [in] */ UINT cNumModels,
            /* [in] */ UINT iModelPosition) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetNumChannels( 
            /* [out] */ __RPC__out UINT *pNumChannels) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DeviceToColorimetricColors( 
            /* [in] */ UINT cColors,
            /* [in] */ UINT cChannels,
            /* [size_is][in] */ __RPC__in_ecount_full(( cColors * cChannels ) ) const FLOAT *pDeviceValues,
            /* [size_is][out] */ __RPC__out_ecount_full(cColors) XYZColorF *pXYZColors) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ColorimetricToDeviceColors( 
            /* [in] */ UINT cColors,
            /* [in] */ UINT cChannels,
            /* [size_is][in] */ __RPC__in_ecount_full(cColors) const XYZColorF *pXYZColors,
            /* [size_is][out] */ __RPC__out_ecount_full(( cColors * cChannels ) ) FLOAT *pDeviceValues) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ColorimetricToDeviceColorsWithBlack( 
            /* [in] */ UINT cColors,
            /* [in] */ UINT cChannels,
            /* [size_is][in] */ __RPC__in_ecount_full(cColors) const XYZColorF *pXYZColors,
            /* [size_is][in] */ __RPC__in_ecount_full(cColors) const BlackInformation *pBlackInformation,
            /* [size_is][out] */ __RPC__out_ecount_full(( cColors * cChannels ) ) FLOAT *pDeviceValues) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetTransformDeviceModelInfo( 
            /* [in] */ UINT iModelPosition,
            /* [in] */ __RPC__in_opt IDeviceModelPlugIn *pIDeviceModelOther) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPrimarySamples( 
            /* [out] */ __RPC__out PrimaryXYZColors *pPrimaryColor) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetGamutBoundaryMeshSize( 
            /* [out] */ __RPC__out UINT *pNumVertices,
            /* [out] */ __RPC__out UINT *pNumTriangles) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetGamutBoundaryMesh( 
            /* [in] */ UINT cChannels,
            /* [in] */ UINT cVertices,
            /* [in] */ UINT cTriangles,
            /* [size_is][out] */ __RPC__out_ecount_full(( cVertices * cChannels ) ) FLOAT *pVertices,
            /* [size_is][out] */ __RPC__out_ecount_full(cTriangles) GamutShellTriangle *pTriangles) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetNeutralAxisSize( 
            /* [out] */ __RPC__out UINT *pcColors) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetNeutralAxis( 
            /* [in] */ UINT cColors,
            /* [size_is][out] */ __RPC__out_ecount_full(cColors) XYZColorF *pXYZColors) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDeviceModelPlugInVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IDeviceModelPlugIn * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IDeviceModelPlugIn * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IDeviceModelPlugIn * This);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            __RPC__in IDeviceModelPlugIn * This,
            /* [in] */ __RPC__in BSTR bstrXml,
            /* [in] */ UINT cNumModels,
            /* [in] */ UINT iModelPosition);
        
        HRESULT ( STDMETHODCALLTYPE *GetNumChannels )( 
            __RPC__in IDeviceModelPlugIn * This,
            /* [out] */ __RPC__out UINT *pNumChannels);
        
        HRESULT ( STDMETHODCALLTYPE *DeviceToColorimetricColors )( 
            __RPC__in IDeviceModelPlugIn * This,
            /* [in] */ UINT cColors,
            /* [in] */ UINT cChannels,
            /* [size_is][in] */ __RPC__in_ecount_full(( cColors * cChannels ) ) const FLOAT *pDeviceValues,
            /* [size_is][out] */ __RPC__out_ecount_full(cColors) XYZColorF *pXYZColors);
        
        HRESULT ( STDMETHODCALLTYPE *ColorimetricToDeviceColors )( 
            __RPC__in IDeviceModelPlugIn * This,
            /* [in] */ UINT cColors,
            /* [in] */ UINT cChannels,
            /* [size_is][in] */ __RPC__in_ecount_full(cColors) const XYZColorF *pXYZColors,
            /* [size_is][out] */ __RPC__out_ecount_full(( cColors * cChannels ) ) FLOAT *pDeviceValues);
        
        HRESULT ( STDMETHODCALLTYPE *ColorimetricToDeviceColorsWithBlack )( 
            __RPC__in IDeviceModelPlugIn * This,
            /* [in] */ UINT cColors,
            /* [in] */ UINT cChannels,
            /* [size_is][in] */ __RPC__in_ecount_full(cColors) const XYZColorF *pXYZColors,
            /* [size_is][in] */ __RPC__in_ecount_full(cColors) const BlackInformation *pBlackInformation,
            /* [size_is][out] */ __RPC__out_ecount_full(( cColors * cChannels ) ) FLOAT *pDeviceValues);
        
        HRESULT ( STDMETHODCALLTYPE *SetTransformDeviceModelInfo )( 
            __RPC__in IDeviceModelPlugIn * This,
            /* [in] */ UINT iModelPosition,
            /* [in] */ __RPC__in_opt IDeviceModelPlugIn *pIDeviceModelOther);
        
        HRESULT ( STDMETHODCALLTYPE *GetPrimarySamples )( 
            __RPC__in IDeviceModelPlugIn * This,
            /* [out] */ __RPC__out PrimaryXYZColors *pPrimaryColor);
        
        HRESULT ( STDMETHODCALLTYPE *GetGamutBoundaryMeshSize )( 
            __RPC__in IDeviceModelPlugIn * This,
            /* [out] */ __RPC__out UINT *pNumVertices,
            /* [out] */ __RPC__out UINT *pNumTriangles);
        
        HRESULT ( STDMETHODCALLTYPE *GetGamutBoundaryMesh )( 
            __RPC__in IDeviceModelPlugIn * This,
            /* [in] */ UINT cChannels,
            /* [in] */ UINT cVertices,
            /* [in] */ UINT cTriangles,
            /* [size_is][out] */ __RPC__out_ecount_full(( cVertices * cChannels ) ) FLOAT *pVertices,
            /* [size_is][out] */ __RPC__out_ecount_full(cTriangles) GamutShellTriangle *pTriangles);
        
        HRESULT ( STDMETHODCALLTYPE *GetNeutralAxisSize )( 
            __RPC__in IDeviceModelPlugIn * This,
            /* [out] */ __RPC__out UINT *pcColors);
        
        HRESULT ( STDMETHODCALLTYPE *GetNeutralAxis )( 
            __RPC__in IDeviceModelPlugIn * This,
            /* [in] */ UINT cColors,
            /* [size_is][out] */ __RPC__out_ecount_full(cColors) XYZColorF *pXYZColors);
        
        END_INTERFACE
    } IDeviceModelPlugInVtbl;

    interface IDeviceModelPlugIn
    {
        CONST_VTBL struct IDeviceModelPlugInVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDeviceModelPlugIn_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IDeviceModelPlugIn_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IDeviceModelPlugIn_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IDeviceModelPlugIn_Initialize(This,bstrXml,cNumModels,iModelPosition)	\
    ( (This)->lpVtbl -> Initialize(This,bstrXml,cNumModels,iModelPosition) ) 

#define IDeviceModelPlugIn_GetNumChannels(This,pNumChannels)	\
    ( (This)->lpVtbl -> GetNumChannels(This,pNumChannels) ) 

#define IDeviceModelPlugIn_DeviceToColorimetricColors(This,cColors,cChannels,pDeviceValues,pXYZColors)	\
    ( (This)->lpVtbl -> DeviceToColorimetricColors(This,cColors,cChannels,pDeviceValues,pXYZColors) ) 

#define IDeviceModelPlugIn_ColorimetricToDeviceColors(This,cColors,cChannels,pXYZColors,pDeviceValues)	\
    ( (This)->lpVtbl -> ColorimetricToDeviceColors(This,cColors,cChannels,pXYZColors,pDeviceValues) ) 

#define IDeviceModelPlugIn_ColorimetricToDeviceColorsWithBlack(This,cColors,cChannels,pXYZColors,pBlackInformation,pDeviceValues)	\
    ( (This)->lpVtbl -> ColorimetricToDeviceColorsWithBlack(This,cColors,cChannels,pXYZColors,pBlackInformation,pDeviceValues) ) 

#define IDeviceModelPlugIn_SetTransformDeviceModelInfo(This,iModelPosition,pIDeviceModelOther)	\
    ( (This)->lpVtbl -> SetTransformDeviceModelInfo(This,iModelPosition,pIDeviceModelOther) ) 

#define IDeviceModelPlugIn_GetPrimarySamples(This,pPrimaryColor)	\
    ( (This)->lpVtbl -> GetPrimarySamples(This,pPrimaryColor) ) 

#define IDeviceModelPlugIn_GetGamutBoundaryMeshSize(This,pNumVertices,pNumTriangles)	\
    ( (This)->lpVtbl -> GetGamutBoundaryMeshSize(This,pNumVertices,pNumTriangles) ) 

#define IDeviceModelPlugIn_GetGamutBoundaryMesh(This,cChannels,cVertices,cTriangles,pVertices,pTriangles)	\
    ( (This)->lpVtbl -> GetGamutBoundaryMesh(This,cChannels,cVertices,cTriangles,pVertices,pTriangles) ) 

#define IDeviceModelPlugIn_GetNeutralAxisSize(This,pcColors)	\
    ( (This)->lpVtbl -> GetNeutralAxisSize(This,pcColors) ) 

#define IDeviceModelPlugIn_GetNeutralAxis(This,cColors,pXYZColors)	\
    ( (This)->lpVtbl -> GetNeutralAxis(This,cColors,pXYZColors) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IDeviceModelPlugIn_INTERFACE_DEFINED__ */


#ifndef __IGamutMapModelPlugIn_INTERFACE_DEFINED__
#define __IGamutMapModelPlugIn_INTERFACE_DEFINED__

/* interface IGamutMapModelPlugIn */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IGamutMapModelPlugIn;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2DD80115-AD1E-41F6-A219-A4F4B583D1F9")
    IGamutMapModelPlugIn : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ __RPC__in BSTR bstrXml,
            /* [in] */ __RPC__in_opt IDeviceModelPlugIn *pSrcPlugIn,
            /* [in] */ __RPC__in_opt IDeviceModelPlugIn *pDestPlugIn,
            /* [in] */ __RPC__in GamutBoundaryDescription *pSrcGBD,
            /* [in] */ __RPC__in GamutBoundaryDescription *pDestGBD) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SourceToDestinationAppearanceColors( 
            /* [in] */ UINT cColors,
            /* [size_is][in] */ __RPC__in_ecount_full(cColors) const JChColorF *pInputColors,
            /* [size_is][out] */ __RPC__out_ecount_full(cColors) JChColorF *pOutputColors) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IGamutMapModelPlugInVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IGamutMapModelPlugIn * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IGamutMapModelPlugIn * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IGamutMapModelPlugIn * This);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            __RPC__in IGamutMapModelPlugIn * This,
            /* [in] */ __RPC__in BSTR bstrXml,
            /* [in] */ __RPC__in_opt IDeviceModelPlugIn *pSrcPlugIn,
            /* [in] */ __RPC__in_opt IDeviceModelPlugIn *pDestPlugIn,
            /* [in] */ __RPC__in GamutBoundaryDescription *pSrcGBD,
            /* [in] */ __RPC__in GamutBoundaryDescription *pDestGBD);
        
        HRESULT ( STDMETHODCALLTYPE *SourceToDestinationAppearanceColors )( 
            __RPC__in IGamutMapModelPlugIn * This,
            /* [in] */ UINT cColors,
            /* [size_is][in] */ __RPC__in_ecount_full(cColors) const JChColorF *pInputColors,
            /* [size_is][out] */ __RPC__out_ecount_full(cColors) JChColorF *pOutputColors);
        
        END_INTERFACE
    } IGamutMapModelPlugInVtbl;

    interface IGamutMapModelPlugIn
    {
        CONST_VTBL struct IGamutMapModelPlugInVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IGamutMapModelPlugIn_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IGamutMapModelPlugIn_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IGamutMapModelPlugIn_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IGamutMapModelPlugIn_Initialize(This,bstrXml,pSrcPlugIn,pDestPlugIn,pSrcGBD,pDestGBD)	\
    ( (This)->lpVtbl -> Initialize(This,bstrXml,pSrcPlugIn,pDestPlugIn,pSrcGBD,pDestGBD) ) 

#define IGamutMapModelPlugIn_SourceToDestinationAppearanceColors(This,cColors,pInputColors,pOutputColors)	\
    ( (This)->lpVtbl -> SourceToDestinationAppearanceColors(This,cColors,pInputColors,pOutputColors) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IGamutMapModelPlugIn_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     __RPC__in unsigned long *, unsigned long            , __RPC__in BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  __RPC__in unsigned long *, __RPC__inout_xcount(0) unsigned char *, __RPC__in BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(__RPC__in unsigned long *, __RPC__in_xcount(0) unsigned char *, __RPC__out BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     __RPC__in unsigned long *, __RPC__in BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



