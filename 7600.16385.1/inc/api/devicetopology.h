

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* Compiler settings for devicetopology.idl:
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

#ifndef __devicetopology_h__
#define __devicetopology_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IKsControl_FWD_DEFINED__
#define __IKsControl_FWD_DEFINED__
typedef interface IKsControl IKsControl;
#endif 	/* __IKsControl_FWD_DEFINED__ */


#ifndef __IPerChannelDbLevel_FWD_DEFINED__
#define __IPerChannelDbLevel_FWD_DEFINED__
typedef interface IPerChannelDbLevel IPerChannelDbLevel;
#endif 	/* __IPerChannelDbLevel_FWD_DEFINED__ */


#ifndef __IAudioVolumeLevel_FWD_DEFINED__
#define __IAudioVolumeLevel_FWD_DEFINED__
typedef interface IAudioVolumeLevel IAudioVolumeLevel;
#endif 	/* __IAudioVolumeLevel_FWD_DEFINED__ */


#ifndef __IAudioChannelConfig_FWD_DEFINED__
#define __IAudioChannelConfig_FWD_DEFINED__
typedef interface IAudioChannelConfig IAudioChannelConfig;
#endif 	/* __IAudioChannelConfig_FWD_DEFINED__ */


#ifndef __IAudioLoudness_FWD_DEFINED__
#define __IAudioLoudness_FWD_DEFINED__
typedef interface IAudioLoudness IAudioLoudness;
#endif 	/* __IAudioLoudness_FWD_DEFINED__ */


#ifndef __IAudioInputSelector_FWD_DEFINED__
#define __IAudioInputSelector_FWD_DEFINED__
typedef interface IAudioInputSelector IAudioInputSelector;
#endif 	/* __IAudioInputSelector_FWD_DEFINED__ */


#ifndef __IAudioOutputSelector_FWD_DEFINED__
#define __IAudioOutputSelector_FWD_DEFINED__
typedef interface IAudioOutputSelector IAudioOutputSelector;
#endif 	/* __IAudioOutputSelector_FWD_DEFINED__ */


#ifndef __IAudioMute_FWD_DEFINED__
#define __IAudioMute_FWD_DEFINED__
typedef interface IAudioMute IAudioMute;
#endif 	/* __IAudioMute_FWD_DEFINED__ */


#ifndef __IAudioBass_FWD_DEFINED__
#define __IAudioBass_FWD_DEFINED__
typedef interface IAudioBass IAudioBass;
#endif 	/* __IAudioBass_FWD_DEFINED__ */


#ifndef __IAudioMidrange_FWD_DEFINED__
#define __IAudioMidrange_FWD_DEFINED__
typedef interface IAudioMidrange IAudioMidrange;
#endif 	/* __IAudioMidrange_FWD_DEFINED__ */


#ifndef __IAudioTreble_FWD_DEFINED__
#define __IAudioTreble_FWD_DEFINED__
typedef interface IAudioTreble IAudioTreble;
#endif 	/* __IAudioTreble_FWD_DEFINED__ */


#ifndef __IAudioAutoGainControl_FWD_DEFINED__
#define __IAudioAutoGainControl_FWD_DEFINED__
typedef interface IAudioAutoGainControl IAudioAutoGainControl;
#endif 	/* __IAudioAutoGainControl_FWD_DEFINED__ */


#ifndef __IAudioPeakMeter_FWD_DEFINED__
#define __IAudioPeakMeter_FWD_DEFINED__
typedef interface IAudioPeakMeter IAudioPeakMeter;
#endif 	/* __IAudioPeakMeter_FWD_DEFINED__ */


#ifndef __IDeviceSpecificProperty_FWD_DEFINED__
#define __IDeviceSpecificProperty_FWD_DEFINED__
typedef interface IDeviceSpecificProperty IDeviceSpecificProperty;
#endif 	/* __IDeviceSpecificProperty_FWD_DEFINED__ */


#ifndef __IKsFormatSupport_FWD_DEFINED__
#define __IKsFormatSupport_FWD_DEFINED__
typedef interface IKsFormatSupport IKsFormatSupport;
#endif 	/* __IKsFormatSupport_FWD_DEFINED__ */


#ifndef __IKsJackDescription_FWD_DEFINED__
#define __IKsJackDescription_FWD_DEFINED__
typedef interface IKsJackDescription IKsJackDescription;
#endif 	/* __IKsJackDescription_FWD_DEFINED__ */


#ifndef __IKsJackDescription2_FWD_DEFINED__
#define __IKsJackDescription2_FWD_DEFINED__
typedef interface IKsJackDescription2 IKsJackDescription2;
#endif 	/* __IKsJackDescription2_FWD_DEFINED__ */


#ifndef __IKsJackSinkInformation_FWD_DEFINED__
#define __IKsJackSinkInformation_FWD_DEFINED__
typedef interface IKsJackSinkInformation IKsJackSinkInformation;
#endif 	/* __IKsJackSinkInformation_FWD_DEFINED__ */


#ifndef __IPartsList_FWD_DEFINED__
#define __IPartsList_FWD_DEFINED__
typedef interface IPartsList IPartsList;
#endif 	/* __IPartsList_FWD_DEFINED__ */


#ifndef __IPart_FWD_DEFINED__
#define __IPart_FWD_DEFINED__
typedef interface IPart IPart;
#endif 	/* __IPart_FWD_DEFINED__ */


#ifndef __IConnector_FWD_DEFINED__
#define __IConnector_FWD_DEFINED__
typedef interface IConnector IConnector;
#endif 	/* __IConnector_FWD_DEFINED__ */


#ifndef __ISubunit_FWD_DEFINED__
#define __ISubunit_FWD_DEFINED__
typedef interface ISubunit ISubunit;
#endif 	/* __ISubunit_FWD_DEFINED__ */


#ifndef __IControlInterface_FWD_DEFINED__
#define __IControlInterface_FWD_DEFINED__
typedef interface IControlInterface IControlInterface;
#endif 	/* __IControlInterface_FWD_DEFINED__ */


#ifndef __IControlChangeNotify_FWD_DEFINED__
#define __IControlChangeNotify_FWD_DEFINED__
typedef interface IControlChangeNotify IControlChangeNotify;
#endif 	/* __IControlChangeNotify_FWD_DEFINED__ */


#ifndef __IDeviceTopology_FWD_DEFINED__
#define __IDeviceTopology_FWD_DEFINED__
typedef interface IDeviceTopology IDeviceTopology;
#endif 	/* __IDeviceTopology_FWD_DEFINED__ */


#ifndef __DeviceTopology_FWD_DEFINED__
#define __DeviceTopology_FWD_DEFINED__

#ifdef __cplusplus
typedef class DeviceTopology DeviceTopology;
#else
typedef struct DeviceTopology DeviceTopology;
#endif /* __cplusplus */

#endif 	/* __DeviceTopology_FWD_DEFINED__ */


#ifndef __IPartsList_FWD_DEFINED__
#define __IPartsList_FWD_DEFINED__
typedef interface IPartsList IPartsList;
#endif 	/* __IPartsList_FWD_DEFINED__ */


#ifndef __IPerChannelDbLevel_FWD_DEFINED__
#define __IPerChannelDbLevel_FWD_DEFINED__
typedef interface IPerChannelDbLevel IPerChannelDbLevel;
#endif 	/* __IPerChannelDbLevel_FWD_DEFINED__ */


#ifndef __IAudioVolumeLevel_FWD_DEFINED__
#define __IAudioVolumeLevel_FWD_DEFINED__
typedef interface IAudioVolumeLevel IAudioVolumeLevel;
#endif 	/* __IAudioVolumeLevel_FWD_DEFINED__ */


#ifndef __IAudioLoudness_FWD_DEFINED__
#define __IAudioLoudness_FWD_DEFINED__
typedef interface IAudioLoudness IAudioLoudness;
#endif 	/* __IAudioLoudness_FWD_DEFINED__ */


#ifndef __IAudioInputSelector_FWD_DEFINED__
#define __IAudioInputSelector_FWD_DEFINED__
typedef interface IAudioInputSelector IAudioInputSelector;
#endif 	/* __IAudioInputSelector_FWD_DEFINED__ */


#ifndef __IAudioMute_FWD_DEFINED__
#define __IAudioMute_FWD_DEFINED__
typedef interface IAudioMute IAudioMute;
#endif 	/* __IAudioMute_FWD_DEFINED__ */


#ifndef __IAudioBass_FWD_DEFINED__
#define __IAudioBass_FWD_DEFINED__
typedef interface IAudioBass IAudioBass;
#endif 	/* __IAudioBass_FWD_DEFINED__ */


#ifndef __IAudioMidrange_FWD_DEFINED__
#define __IAudioMidrange_FWD_DEFINED__
typedef interface IAudioMidrange IAudioMidrange;
#endif 	/* __IAudioMidrange_FWD_DEFINED__ */


#ifndef __IAudioTreble_FWD_DEFINED__
#define __IAudioTreble_FWD_DEFINED__
typedef interface IAudioTreble IAudioTreble;
#endif 	/* __IAudioTreble_FWD_DEFINED__ */


#ifndef __IAudioAutoGainControl_FWD_DEFINED__
#define __IAudioAutoGainControl_FWD_DEFINED__
typedef interface IAudioAutoGainControl IAudioAutoGainControl;
#endif 	/* __IAudioAutoGainControl_FWD_DEFINED__ */


#ifndef __IAudioOutputSelector_FWD_DEFINED__
#define __IAudioOutputSelector_FWD_DEFINED__
typedef interface IAudioOutputSelector IAudioOutputSelector;
#endif 	/* __IAudioOutputSelector_FWD_DEFINED__ */


#ifndef __IAudioPeakMeter_FWD_DEFINED__
#define __IAudioPeakMeter_FWD_DEFINED__
typedef interface IAudioPeakMeter IAudioPeakMeter;
#endif 	/* __IAudioPeakMeter_FWD_DEFINED__ */


#ifndef __IDeviceSpecificProperty_FWD_DEFINED__
#define __IDeviceSpecificProperty_FWD_DEFINED__
typedef interface IDeviceSpecificProperty IDeviceSpecificProperty;
#endif 	/* __IDeviceSpecificProperty_FWD_DEFINED__ */


#ifndef __IKsFormatSupport_FWD_DEFINED__
#define __IKsFormatSupport_FWD_DEFINED__
typedef interface IKsFormatSupport IKsFormatSupport;
#endif 	/* __IKsFormatSupport_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "propidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_devicetopology_0000_0000 */
/* [local] */ 

#define E_NOTFOUND HRESULT_FROM_WIN32(ERROR_NOT_FOUND)
//
//   Flag for clients of IControlChangeNotify::OnNotify to allow those clients to identify hardware initiated notifications
//
#define DEVTOPO_HARDWARE_INITIATED_EVENTCONTEXT 'draH'
/* E2C2E9DE-09B1-4B04-84E5-07931225EE04 */
DEFINE_GUID(EVENTCONTEXT_VOLUMESLIDER, 0xE2C2E9DE,0x09B1,0x4B04,0x84, 0xE5, 0x07, 0x93, 0x12, 0x25, 0xEE, 0x04);
#define _IKsControl_
#include "ks.h"
#include "ksmedia.h"
#ifndef _KS_
typedef /* [public] */ struct __MIDL___MIDL_itf_devicetopology_0000_0000_0001
    {
    ULONG FormatSize;
    ULONG Flags;
    ULONG SampleSize;
    ULONG Reserved;
    GUID MajorFormat;
    GUID SubFormat;
    GUID Specifier;
    } 	KSDATAFORMAT;

typedef struct __MIDL___MIDL_itf_devicetopology_0000_0000_0001 *PKSDATAFORMAT;

typedef /* [public][public][public][public][public][public][public][public][public][public] */ struct __MIDL___MIDL_itf_devicetopology_0000_0000_0002
    {
    union 
        {
        struct 
            {
            GUID Set;
            ULONG Id;
            ULONG Flags;
            } 	;
        LONGLONG Alignment;
        } 	;
    } 	KSIDENTIFIER;

typedef struct __MIDL___MIDL_itf_devicetopology_0000_0000_0002 *PKSIDENTIFIER;

typedef KSIDENTIFIER KSPROPERTY;

typedef KSIDENTIFIER *PKSPROPERTY;

typedef KSIDENTIFIER KSMETHOD;

typedef KSIDENTIFIER *PKSMETHOD;

typedef KSIDENTIFIER KSEVENT;

typedef KSIDENTIFIER *PKSEVENT;

typedef /* [public][public][public][public] */ 
enum __MIDL___MIDL_itf_devicetopology_0000_0000_0005
    {	eConnTypeUnknown	= 0,
	eConnType3Point5mm	= ( eConnTypeUnknown + 1 ) ,
	eConnTypeQuarter	= ( eConnType3Point5mm + 1 ) ,
	eConnTypeAtapiInternal	= ( eConnTypeQuarter + 1 ) ,
	eConnTypeRCA	= ( eConnTypeAtapiInternal + 1 ) ,
	eConnTypeOptical	= ( eConnTypeRCA + 1 ) ,
	eConnTypeOtherDigital	= ( eConnTypeOptical + 1 ) ,
	eConnTypeOtherAnalog	= ( eConnTypeOtherDigital + 1 ) ,
	eConnTypeMultichannelAnalogDIN	= ( eConnTypeOtherAnalog + 1 ) ,
	eConnTypeXlrProfessional	= ( eConnTypeMultichannelAnalogDIN + 1 ) ,
	eConnTypeRJ11Modem	= ( eConnTypeXlrProfessional + 1 ) ,
	eConnTypeCombination	= ( eConnTypeRJ11Modem + 1 ) 
    } 	EPcxConnectionType;

typedef /* [public][public][public][public] */ 
enum __MIDL___MIDL_itf_devicetopology_0000_0000_0006
    {	eGeoLocRear	= 0x1,
	eGeoLocFront	= ( eGeoLocRear + 1 ) ,
	eGeoLocLeft	= ( eGeoLocFront + 1 ) ,
	eGeoLocRight	= ( eGeoLocLeft + 1 ) ,
	eGeoLocTop	= ( eGeoLocRight + 1 ) ,
	eGeoLocBottom	= ( eGeoLocTop + 1 ) ,
	eGeoLocRearPanel	= ( eGeoLocBottom + 1 ) ,
	eGeoLocRiser	= ( eGeoLocRearPanel + 1 ) ,
	eGeoLocInsideMobileLid	= ( eGeoLocRiser + 1 ) ,
	eGeoLocDrivebay	= ( eGeoLocInsideMobileLid + 1 ) ,
	eGeoLocHDMI	= ( eGeoLocDrivebay + 1 ) ,
	eGeoLocOutsideMobileLid	= ( eGeoLocHDMI + 1 ) ,
	eGeoLocATAPI	= ( eGeoLocOutsideMobileLid + 1 ) ,
	eGeoLocReserved5	= ( eGeoLocATAPI + 1 ) ,
	eGeoLocReserved6	= ( eGeoLocReserved5 + 1 ) 
    } 	EPcxGeoLocation;

typedef /* [public][public][public][public] */ 
enum __MIDL___MIDL_itf_devicetopology_0000_0000_0007
    {	eGenLocPrimaryBox	= 0,
	eGenLocInternal	= ( eGenLocPrimaryBox + 1 ) ,
	eGenLocSeparate	= ( eGenLocInternal + 1 ) ,
	eGenLocOther	= ( eGenLocSeparate + 1 ) 
    } 	EPcxGenLocation;

typedef /* [public][public][public][public] */ 
enum __MIDL___MIDL_itf_devicetopology_0000_0000_0008
    {	ePortConnJack	= 0,
	ePortConnIntegratedDevice	= ( ePortConnJack + 1 ) ,
	ePortConnBothIntegratedAndJack	= ( ePortConnIntegratedDevice + 1 ) ,
	ePortConnUnknown	= ( ePortConnBothIntegratedAndJack + 1 ) 
    } 	EPxcPortConnection;

typedef /* [public][public] */ struct __MIDL___MIDL_itf_devicetopology_0000_0000_0009
    {
    DWORD ChannelMapping;
    COLORREF Color;
    EPcxConnectionType ConnectionType;
    EPcxGeoLocation GeoLocation;
    EPcxGenLocation GenLocation;
    EPxcPortConnection PortConnection;
    BOOL IsConnected;
    } 	KSJACK_DESCRIPTION;

typedef struct __MIDL___MIDL_itf_devicetopology_0000_0000_0009 *PKSJACK_DESCRIPTION;

typedef struct _LUID
    {
    DWORD LowPart;
    LONG HighPart;
    } 	LUID;

typedef struct _LUID *PLUID;

typedef /* [public][public][public] */ 
enum __MIDL___MIDL_itf_devicetopology_0000_0000_0010
    {	KSJACK_SINK_CONNECTIONTYPE_HDMI	= 0,
	KSJACK_SINK_CONNECTIONTYPE_DISPLAYPORT	= ( KSJACK_SINK_CONNECTIONTYPE_HDMI + 1 ) 
    } 	KSJACK_SINK_CONNECTIONTYPE;

typedef struct _tagKSJACK_SINK_INFORMATION
    {
    KSJACK_SINK_CONNECTIONTYPE ConnType;
    WORD ManufacturerId;
    WORD ProductId;
    WORD AudioLatency;
    BOOL HDCPCapable;
    BOOL AICapable;
    UCHAR SinkDescriptionLength;
    WCHAR SinkDescription[ 32 ];
    LUID PortId;
    } 	KSJACK_SINK_INFORMATION;

typedef struct _tagKSJACK_DESCRIPTION2
    {
    DWORD DeviceStateInfo;
    DWORD JackCapabilities;
    } 	KSJACK_DESCRIPTION2;

typedef struct _tagKSJACK_DESCRIPTION2 *PKSJACK_DESCRIPTION2;

#endif








typedef /* [public][public] */ 
enum __MIDL___MIDL_itf_devicetopology_0000_0000_0011
    {	In	= 0,
	Out	= ( In + 1 ) 
    } 	DataFlow;

typedef /* [public][public] */ 
enum __MIDL___MIDL_itf_devicetopology_0000_0000_0012
    {	Connector	= 0,
	Subunit	= ( Connector + 1 ) 
    } 	PartType;

typedef /* [public][public] */ 
enum __MIDL___MIDL_itf_devicetopology_0000_0000_0013
    {	Unknown_Connector	= 0,
	Physical_Internal	= ( Unknown_Connector + 1 ) ,
	Physical_External	= ( Physical_Internal + 1 ) ,
	Software_IO	= ( Physical_External + 1 ) ,
	Software_Fixed	= ( Software_IO + 1 ) ,
	Network	= ( Software_Fixed + 1 ) 
    } 	ConnectorType;



extern RPC_IF_HANDLE __MIDL_itf_devicetopology_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_devicetopology_0000_0000_v0_0_s_ifspec;

#ifndef __IKsControl_INTERFACE_DEFINED__
#define __IKsControl_INTERFACE_DEFINED__

/* interface IKsControl */
/* [unique][helpstring][nonextensible][uuid][local][object] */ 


EXTERN_C const IID IID_IKsControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("28F54685-06FD-11D2-B27A-00A0C9223196")
    IKsControl : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE KsProperty( 
            /* [in] */ PKSPROPERTY Property,
            /* [in] */ ULONG PropertyLength,
            /* [out][in] */ void *PropertyData,
            /* [in] */ ULONG DataLength,
            /* [out] */ ULONG *BytesReturned) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE KsMethod( 
            /* [in] */ PKSMETHOD Method,
            /* [in] */ ULONG MethodLength,
            /* [out][in] */ void *MethodData,
            /* [in] */ ULONG DataLength,
            /* [out] */ ULONG *BytesReturned) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE KsEvent( 
            /* [in] */ PKSEVENT Event,
            /* [in] */ ULONG EventLength,
            /* [out][in] */ void *EventData,
            /* [in] */ ULONG DataLength,
            /* [out] */ ULONG *BytesReturned) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IKsControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IKsControl * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IKsControl * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IKsControl * This);
        
        HRESULT ( STDMETHODCALLTYPE *KsProperty )( 
            IKsControl * This,
            /* [in] */ PKSPROPERTY Property,
            /* [in] */ ULONG PropertyLength,
            /* [out][in] */ void *PropertyData,
            /* [in] */ ULONG DataLength,
            /* [out] */ ULONG *BytesReturned);
        
        HRESULT ( STDMETHODCALLTYPE *KsMethod )( 
            IKsControl * This,
            /* [in] */ PKSMETHOD Method,
            /* [in] */ ULONG MethodLength,
            /* [out][in] */ void *MethodData,
            /* [in] */ ULONG DataLength,
            /* [out] */ ULONG *BytesReturned);
        
        HRESULT ( STDMETHODCALLTYPE *KsEvent )( 
            IKsControl * This,
            /* [in] */ PKSEVENT Event,
            /* [in] */ ULONG EventLength,
            /* [out][in] */ void *EventData,
            /* [in] */ ULONG DataLength,
            /* [out] */ ULONG *BytesReturned);
        
        END_INTERFACE
    } IKsControlVtbl;

    interface IKsControl
    {
        CONST_VTBL struct IKsControlVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IKsControl_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IKsControl_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IKsControl_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IKsControl_KsProperty(This,Property,PropertyLength,PropertyData,DataLength,BytesReturned)	\
    ( (This)->lpVtbl -> KsProperty(This,Property,PropertyLength,PropertyData,DataLength,BytesReturned) ) 

#define IKsControl_KsMethod(This,Method,MethodLength,MethodData,DataLength,BytesReturned)	\
    ( (This)->lpVtbl -> KsMethod(This,Method,MethodLength,MethodData,DataLength,BytesReturned) ) 

#define IKsControl_KsEvent(This,Event,EventLength,EventData,DataLength,BytesReturned)	\
    ( (This)->lpVtbl -> KsEvent(This,Event,EventLength,EventData,DataLength,BytesReturned) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IKsControl_INTERFACE_DEFINED__ */


#ifndef __IPerChannelDbLevel_INTERFACE_DEFINED__
#define __IPerChannelDbLevel_INTERFACE_DEFINED__

/* interface IPerChannelDbLevel */
/* [unique][helpstring][nonextensible][uuid][local][object] */ 


EXTERN_C const IID IID_IPerChannelDbLevel;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C2F8E001-F205-4BC9-99BC-C13B1E048CCB")
    IPerChannelDbLevel : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetChannelCount( 
            /* [annotation][out] */ 
            __out  UINT *pcChannels) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetLevelRange( 
            /* [annotation][in] */ 
            __in  UINT nChannel,
            /* [annotation][out] */ 
            __out  float *pfMinLevelDB,
            /* [annotation][out] */ 
            __out  float *pfMaxLevelDB,
            /* [annotation][out] */ 
            __out  float *pfStepping) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetLevel( 
            /* [annotation][in] */ 
            __in  UINT nChannel,
            /* [annotation][out] */ 
            __out  float *pfLevelDB) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetLevel( 
            /* [annotation][in] */ 
            __in  UINT nChannel,
            /* [annotation][in] */ 
            __in  float fLevelDB,
            /* [annotation][unique][in] */ 
            __in_opt  LPCGUID pguidEventContext) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetLevelUniform( 
            /* [annotation][in] */ 
            __in  float fLevelDB,
            /* [annotation][unique][in] */ 
            __in_opt  LPCGUID pguidEventContext) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetLevelAllChannels( 
            /* [annotation][size_is][in] */ 
            __in_ecount(cChannels)  float aLevelsDB[  ],
            /* [annotation][in] */ 
            __in  ULONG cChannels,
            /* [annotation][unique][in] */ 
            __in_opt  LPCGUID pguidEventContext) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPerChannelDbLevelVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPerChannelDbLevel * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPerChannelDbLevel * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPerChannelDbLevel * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetChannelCount )( 
            IPerChannelDbLevel * This,
            /* [annotation][out] */ 
            __out  UINT *pcChannels);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetLevelRange )( 
            IPerChannelDbLevel * This,
            /* [annotation][in] */ 
            __in  UINT nChannel,
            /* [annotation][out] */ 
            __out  float *pfMinLevelDB,
            /* [annotation][out] */ 
            __out  float *pfMaxLevelDB,
            /* [annotation][out] */ 
            __out  float *pfStepping);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetLevel )( 
            IPerChannelDbLevel * This,
            /* [annotation][in] */ 
            __in  UINT nChannel,
            /* [annotation][out] */ 
            __out  float *pfLevelDB);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetLevel )( 
            IPerChannelDbLevel * This,
            /* [annotation][in] */ 
            __in  UINT nChannel,
            /* [annotation][in] */ 
            __in  float fLevelDB,
            /* [annotation][unique][in] */ 
            __in_opt  LPCGUID pguidEventContext);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetLevelUniform )( 
            IPerChannelDbLevel * This,
            /* [annotation][in] */ 
            __in  float fLevelDB,
            /* [annotation][unique][in] */ 
            __in_opt  LPCGUID pguidEventContext);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetLevelAllChannels )( 
            IPerChannelDbLevel * This,
            /* [annotation][size_is][in] */ 
            __in_ecount(cChannels)  float aLevelsDB[  ],
            /* [annotation][in] */ 
            __in  ULONG cChannels,
            /* [annotation][unique][in] */ 
            __in_opt  LPCGUID pguidEventContext);
        
        END_INTERFACE
    } IPerChannelDbLevelVtbl;

    interface IPerChannelDbLevel
    {
        CONST_VTBL struct IPerChannelDbLevelVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPerChannelDbLevel_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IPerChannelDbLevel_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IPerChannelDbLevel_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IPerChannelDbLevel_GetChannelCount(This,pcChannels)	\
    ( (This)->lpVtbl -> GetChannelCount(This,pcChannels) ) 

#define IPerChannelDbLevel_GetLevelRange(This,nChannel,pfMinLevelDB,pfMaxLevelDB,pfStepping)	\
    ( (This)->lpVtbl -> GetLevelRange(This,nChannel,pfMinLevelDB,pfMaxLevelDB,pfStepping) ) 

#define IPerChannelDbLevel_GetLevel(This,nChannel,pfLevelDB)	\
    ( (This)->lpVtbl -> GetLevel(This,nChannel,pfLevelDB) ) 

#define IPerChannelDbLevel_SetLevel(This,nChannel,fLevelDB,pguidEventContext)	\
    ( (This)->lpVtbl -> SetLevel(This,nChannel,fLevelDB,pguidEventContext) ) 

#define IPerChannelDbLevel_SetLevelUniform(This,fLevelDB,pguidEventContext)	\
    ( (This)->lpVtbl -> SetLevelUniform(This,fLevelDB,pguidEventContext) ) 

#define IPerChannelDbLevel_SetLevelAllChannels(This,aLevelsDB,cChannels,pguidEventContext)	\
    ( (This)->lpVtbl -> SetLevelAllChannels(This,aLevelsDB,cChannels,pguidEventContext) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IPerChannelDbLevel_INTERFACE_DEFINED__ */


#ifndef __IAudioVolumeLevel_INTERFACE_DEFINED__
#define __IAudioVolumeLevel_INTERFACE_DEFINED__

/* interface IAudioVolumeLevel */
/* [unique][helpstring][nonextensible][uuid][local][object] */ 


EXTERN_C const IID IID_IAudioVolumeLevel;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7FB7B48F-531D-44A2-BCB3-5AD5A134B3DC")
    IAudioVolumeLevel : public IPerChannelDbLevel
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IAudioVolumeLevelVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAudioVolumeLevel * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAudioVolumeLevel * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAudioVolumeLevel * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetChannelCount )( 
            IAudioVolumeLevel * This,
            /* [annotation][out] */ 
            __out  UINT *pcChannels);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetLevelRange )( 
            IAudioVolumeLevel * This,
            /* [annotation][in] */ 
            __in  UINT nChannel,
            /* [annotation][out] */ 
            __out  float *pfMinLevelDB,
            /* [annotation][out] */ 
            __out  float *pfMaxLevelDB,
            /* [annotation][out] */ 
            __out  float *pfStepping);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetLevel )( 
            IAudioVolumeLevel * This,
            /* [annotation][in] */ 
            __in  UINT nChannel,
            /* [annotation][out] */ 
            __out  float *pfLevelDB);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetLevel )( 
            IAudioVolumeLevel * This,
            /* [annotation][in] */ 
            __in  UINT nChannel,
            /* [annotation][in] */ 
            __in  float fLevelDB,
            /* [annotation][unique][in] */ 
            __in_opt  LPCGUID pguidEventContext);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetLevelUniform )( 
            IAudioVolumeLevel * This,
            /* [annotation][in] */ 
            __in  float fLevelDB,
            /* [annotation][unique][in] */ 
            __in_opt  LPCGUID pguidEventContext);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetLevelAllChannels )( 
            IAudioVolumeLevel * This,
            /* [annotation][size_is][in] */ 
            __in_ecount(cChannels)  float aLevelsDB[  ],
            /* [annotation][in] */ 
            __in  ULONG cChannels,
            /* [annotation][unique][in] */ 
            __in_opt  LPCGUID pguidEventContext);
        
        END_INTERFACE
    } IAudioVolumeLevelVtbl;

    interface IAudioVolumeLevel
    {
        CONST_VTBL struct IAudioVolumeLevelVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAudioVolumeLevel_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAudioVolumeLevel_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAudioVolumeLevel_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAudioVolumeLevel_GetChannelCount(This,pcChannels)	\
    ( (This)->lpVtbl -> GetChannelCount(This,pcChannels) ) 

#define IAudioVolumeLevel_GetLevelRange(This,nChannel,pfMinLevelDB,pfMaxLevelDB,pfStepping)	\
    ( (This)->lpVtbl -> GetLevelRange(This,nChannel,pfMinLevelDB,pfMaxLevelDB,pfStepping) ) 

#define IAudioVolumeLevel_GetLevel(This,nChannel,pfLevelDB)	\
    ( (This)->lpVtbl -> GetLevel(This,nChannel,pfLevelDB) ) 

#define IAudioVolumeLevel_SetLevel(This,nChannel,fLevelDB,pguidEventContext)	\
    ( (This)->lpVtbl -> SetLevel(This,nChannel,fLevelDB,pguidEventContext) ) 

#define IAudioVolumeLevel_SetLevelUniform(This,fLevelDB,pguidEventContext)	\
    ( (This)->lpVtbl -> SetLevelUniform(This,fLevelDB,pguidEventContext) ) 

#define IAudioVolumeLevel_SetLevelAllChannels(This,aLevelsDB,cChannels,pguidEventContext)	\
    ( (This)->lpVtbl -> SetLevelAllChannels(This,aLevelsDB,cChannels,pguidEventContext) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAudioVolumeLevel_INTERFACE_DEFINED__ */


#ifndef __IAudioChannelConfig_INTERFACE_DEFINED__
#define __IAudioChannelConfig_INTERFACE_DEFINED__

/* interface IAudioChannelConfig */
/* [unique][helpstring][nonextensible][uuid][local][object] */ 


EXTERN_C const IID IID_IAudioChannelConfig;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("BB11C46F-EC28-493C-B88A-5DB88062CE98")
    IAudioChannelConfig : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetChannelConfig( 
            /* [in] */ DWORD dwConfig,
            /* [annotation][unique][in] */ 
            __in_opt  LPCGUID pguidEventContext) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetChannelConfig( 
            /* [retval][out] */ DWORD *pdwConfig) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAudioChannelConfigVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAudioChannelConfig * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAudioChannelConfig * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAudioChannelConfig * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetChannelConfig )( 
            IAudioChannelConfig * This,
            /* [in] */ DWORD dwConfig,
            /* [annotation][unique][in] */ 
            __in_opt  LPCGUID pguidEventContext);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetChannelConfig )( 
            IAudioChannelConfig * This,
            /* [retval][out] */ DWORD *pdwConfig);
        
        END_INTERFACE
    } IAudioChannelConfigVtbl;

    interface IAudioChannelConfig
    {
        CONST_VTBL struct IAudioChannelConfigVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAudioChannelConfig_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAudioChannelConfig_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAudioChannelConfig_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAudioChannelConfig_SetChannelConfig(This,dwConfig,pguidEventContext)	\
    ( (This)->lpVtbl -> SetChannelConfig(This,dwConfig,pguidEventContext) ) 

#define IAudioChannelConfig_GetChannelConfig(This,pdwConfig)	\
    ( (This)->lpVtbl -> GetChannelConfig(This,pdwConfig) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAudioChannelConfig_INTERFACE_DEFINED__ */


#ifndef __IAudioLoudness_INTERFACE_DEFINED__
#define __IAudioLoudness_INTERFACE_DEFINED__

/* interface IAudioLoudness */
/* [unique][helpstring][nonextensible][uuid][local][object] */ 


EXTERN_C const IID IID_IAudioLoudness;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7D8B1437-DD53-4350-9C1B-1EE2890BD938")
    IAudioLoudness : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetEnabled( 
            /* [annotation][out] */ 
            __out  BOOL *pbEnabled) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetEnabled( 
            /* [annotation][in] */ 
            __in  BOOL bEnable,
            /* [annotation][unique][in] */ 
            __in_opt  LPCGUID pguidEventContext) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAudioLoudnessVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAudioLoudness * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAudioLoudness * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAudioLoudness * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetEnabled )( 
            IAudioLoudness * This,
            /* [annotation][out] */ 
            __out  BOOL *pbEnabled);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetEnabled )( 
            IAudioLoudness * This,
            /* [annotation][in] */ 
            __in  BOOL bEnable,
            /* [annotation][unique][in] */ 
            __in_opt  LPCGUID pguidEventContext);
        
        END_INTERFACE
    } IAudioLoudnessVtbl;

    interface IAudioLoudness
    {
        CONST_VTBL struct IAudioLoudnessVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAudioLoudness_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAudioLoudness_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAudioLoudness_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAudioLoudness_GetEnabled(This,pbEnabled)	\
    ( (This)->lpVtbl -> GetEnabled(This,pbEnabled) ) 

#define IAudioLoudness_SetEnabled(This,bEnable,pguidEventContext)	\
    ( (This)->lpVtbl -> SetEnabled(This,bEnable,pguidEventContext) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAudioLoudness_INTERFACE_DEFINED__ */


#ifndef __IAudioInputSelector_INTERFACE_DEFINED__
#define __IAudioInputSelector_INTERFACE_DEFINED__

/* interface IAudioInputSelector */
/* [unique][helpstring][nonextensible][uuid][local][object] */ 


EXTERN_C const IID IID_IAudioInputSelector;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4F03DC02-5E6E-4653-8F72-A030C123D598")
    IAudioInputSelector : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSelection( 
            /* [annotation][out] */ 
            __out  UINT *pnIdSelected) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetSelection( 
            /* [annotation][in] */ 
            __in  UINT nIdSelect,
            /* [annotation][unique][in] */ 
            __in_opt  LPCGUID pguidEventContext) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAudioInputSelectorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAudioInputSelector * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAudioInputSelector * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAudioInputSelector * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetSelection )( 
            IAudioInputSelector * This,
            /* [annotation][out] */ 
            __out  UINT *pnIdSelected);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetSelection )( 
            IAudioInputSelector * This,
            /* [annotation][in] */ 
            __in  UINT nIdSelect,
            /* [annotation][unique][in] */ 
            __in_opt  LPCGUID pguidEventContext);
        
        END_INTERFACE
    } IAudioInputSelectorVtbl;

    interface IAudioInputSelector
    {
        CONST_VTBL struct IAudioInputSelectorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAudioInputSelector_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAudioInputSelector_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAudioInputSelector_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAudioInputSelector_GetSelection(This,pnIdSelected)	\
    ( (This)->lpVtbl -> GetSelection(This,pnIdSelected) ) 

#define IAudioInputSelector_SetSelection(This,nIdSelect,pguidEventContext)	\
    ( (This)->lpVtbl -> SetSelection(This,nIdSelect,pguidEventContext) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAudioInputSelector_INTERFACE_DEFINED__ */


#ifndef __IAudioOutputSelector_INTERFACE_DEFINED__
#define __IAudioOutputSelector_INTERFACE_DEFINED__

/* interface IAudioOutputSelector */
/* [unique][helpstring][nonextensible][uuid][local][object] */ 


EXTERN_C const IID IID_IAudioOutputSelector;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("BB515F69-94A7-429e-8B9C-271B3F11A3AB")
    IAudioOutputSelector : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSelection( 
            /* [annotation][out] */ 
            __out  UINT *pnIdSelected) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetSelection( 
            /* [annotation][in] */ 
            __in  UINT nIdSelect,
            /* [annotation][unique][in] */ 
            __in_opt  LPCGUID pguidEventContext) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAudioOutputSelectorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAudioOutputSelector * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAudioOutputSelector * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAudioOutputSelector * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetSelection )( 
            IAudioOutputSelector * This,
            /* [annotation][out] */ 
            __out  UINT *pnIdSelected);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetSelection )( 
            IAudioOutputSelector * This,
            /* [annotation][in] */ 
            __in  UINT nIdSelect,
            /* [annotation][unique][in] */ 
            __in_opt  LPCGUID pguidEventContext);
        
        END_INTERFACE
    } IAudioOutputSelectorVtbl;

    interface IAudioOutputSelector
    {
        CONST_VTBL struct IAudioOutputSelectorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAudioOutputSelector_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAudioOutputSelector_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAudioOutputSelector_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAudioOutputSelector_GetSelection(This,pnIdSelected)	\
    ( (This)->lpVtbl -> GetSelection(This,pnIdSelected) ) 

#define IAudioOutputSelector_SetSelection(This,nIdSelect,pguidEventContext)	\
    ( (This)->lpVtbl -> SetSelection(This,nIdSelect,pguidEventContext) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAudioOutputSelector_INTERFACE_DEFINED__ */


#ifndef __IAudioMute_INTERFACE_DEFINED__
#define __IAudioMute_INTERFACE_DEFINED__

/* interface IAudioMute */
/* [unique][helpstring][nonextensible][uuid][local][object] */ 


EXTERN_C const IID IID_IAudioMute;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DF45AEEA-B74A-4B6B-AFAD-2366B6AA012E")
    IAudioMute : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetMute( 
            /* [annotation][in] */ 
            __in  BOOL bMuted,
            /* [annotation][unique][in] */ 
            __in_opt  LPCGUID pguidEventContext) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetMute( 
            /* [annotation][out] */ 
            __out  BOOL *pbMuted) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAudioMuteVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAudioMute * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAudioMute * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAudioMute * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetMute )( 
            IAudioMute * This,
            /* [annotation][in] */ 
            __in  BOOL bMuted,
            /* [annotation][unique][in] */ 
            __in_opt  LPCGUID pguidEventContext);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetMute )( 
            IAudioMute * This,
            /* [annotation][out] */ 
            __out  BOOL *pbMuted);
        
        END_INTERFACE
    } IAudioMuteVtbl;

    interface IAudioMute
    {
        CONST_VTBL struct IAudioMuteVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAudioMute_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAudioMute_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAudioMute_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAudioMute_SetMute(This,bMuted,pguidEventContext)	\
    ( (This)->lpVtbl -> SetMute(This,bMuted,pguidEventContext) ) 

#define IAudioMute_GetMute(This,pbMuted)	\
    ( (This)->lpVtbl -> GetMute(This,pbMuted) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAudioMute_INTERFACE_DEFINED__ */


#ifndef __IAudioBass_INTERFACE_DEFINED__
#define __IAudioBass_INTERFACE_DEFINED__

/* interface IAudioBass */
/* [unique][helpstring][nonextensible][uuid][local][object] */ 


EXTERN_C const IID IID_IAudioBass;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A2B1A1D9-4DB3-425D-A2B2-BD335CB3E2E5")
    IAudioBass : public IPerChannelDbLevel
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IAudioBassVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAudioBass * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAudioBass * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAudioBass * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetChannelCount )( 
            IAudioBass * This,
            /* [annotation][out] */ 
            __out  UINT *pcChannels);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetLevelRange )( 
            IAudioBass * This,
            /* [annotation][in] */ 
            __in  UINT nChannel,
            /* [annotation][out] */ 
            __out  float *pfMinLevelDB,
            /* [annotation][out] */ 
            __out  float *pfMaxLevelDB,
            /* [annotation][out] */ 
            __out  float *pfStepping);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetLevel )( 
            IAudioBass * This,
            /* [annotation][in] */ 
            __in  UINT nChannel,
            /* [annotation][out] */ 
            __out  float *pfLevelDB);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetLevel )( 
            IAudioBass * This,
            /* [annotation][in] */ 
            __in  UINT nChannel,
            /* [annotation][in] */ 
            __in  float fLevelDB,
            /* [annotation][unique][in] */ 
            __in_opt  LPCGUID pguidEventContext);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetLevelUniform )( 
            IAudioBass * This,
            /* [annotation][in] */ 
            __in  float fLevelDB,
            /* [annotation][unique][in] */ 
            __in_opt  LPCGUID pguidEventContext);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetLevelAllChannels )( 
            IAudioBass * This,
            /* [annotation][size_is][in] */ 
            __in_ecount(cChannels)  float aLevelsDB[  ],
            /* [annotation][in] */ 
            __in  ULONG cChannels,
            /* [annotation][unique][in] */ 
            __in_opt  LPCGUID pguidEventContext);
        
        END_INTERFACE
    } IAudioBassVtbl;

    interface IAudioBass
    {
        CONST_VTBL struct IAudioBassVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAudioBass_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAudioBass_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAudioBass_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAudioBass_GetChannelCount(This,pcChannels)	\
    ( (This)->lpVtbl -> GetChannelCount(This,pcChannels) ) 

#define IAudioBass_GetLevelRange(This,nChannel,pfMinLevelDB,pfMaxLevelDB,pfStepping)	\
    ( (This)->lpVtbl -> GetLevelRange(This,nChannel,pfMinLevelDB,pfMaxLevelDB,pfStepping) ) 

#define IAudioBass_GetLevel(This,nChannel,pfLevelDB)	\
    ( (This)->lpVtbl -> GetLevel(This,nChannel,pfLevelDB) ) 

#define IAudioBass_SetLevel(This,nChannel,fLevelDB,pguidEventContext)	\
    ( (This)->lpVtbl -> SetLevel(This,nChannel,fLevelDB,pguidEventContext) ) 

#define IAudioBass_SetLevelUniform(This,fLevelDB,pguidEventContext)	\
    ( (This)->lpVtbl -> SetLevelUniform(This,fLevelDB,pguidEventContext) ) 

#define IAudioBass_SetLevelAllChannels(This,aLevelsDB,cChannels,pguidEventContext)	\
    ( (This)->lpVtbl -> SetLevelAllChannels(This,aLevelsDB,cChannels,pguidEventContext) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAudioBass_INTERFACE_DEFINED__ */


#ifndef __IAudioMidrange_INTERFACE_DEFINED__
#define __IAudioMidrange_INTERFACE_DEFINED__

/* interface IAudioMidrange */
/* [unique][helpstring][nonextensible][uuid][local][object] */ 


EXTERN_C const IID IID_IAudioMidrange;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5E54B6D7-B44B-40D9-9A9E-E691D9CE6EDF")
    IAudioMidrange : public IPerChannelDbLevel
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IAudioMidrangeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAudioMidrange * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAudioMidrange * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAudioMidrange * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetChannelCount )( 
            IAudioMidrange * This,
            /* [annotation][out] */ 
            __out  UINT *pcChannels);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetLevelRange )( 
            IAudioMidrange * This,
            /* [annotation][in] */ 
            __in  UINT nChannel,
            /* [annotation][out] */ 
            __out  float *pfMinLevelDB,
            /* [annotation][out] */ 
            __out  float *pfMaxLevelDB,
            /* [annotation][out] */ 
            __out  float *pfStepping);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetLevel )( 
            IAudioMidrange * This,
            /* [annotation][in] */ 
            __in  UINT nChannel,
            /* [annotation][out] */ 
            __out  float *pfLevelDB);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetLevel )( 
            IAudioMidrange * This,
            /* [annotation][in] */ 
            __in  UINT nChannel,
            /* [annotation][in] */ 
            __in  float fLevelDB,
            /* [annotation][unique][in] */ 
            __in_opt  LPCGUID pguidEventContext);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetLevelUniform )( 
            IAudioMidrange * This,
            /* [annotation][in] */ 
            __in  float fLevelDB,
            /* [annotation][unique][in] */ 
            __in_opt  LPCGUID pguidEventContext);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetLevelAllChannels )( 
            IAudioMidrange * This,
            /* [annotation][size_is][in] */ 
            __in_ecount(cChannels)  float aLevelsDB[  ],
            /* [annotation][in] */ 
            __in  ULONG cChannels,
            /* [annotation][unique][in] */ 
            __in_opt  LPCGUID pguidEventContext);
        
        END_INTERFACE
    } IAudioMidrangeVtbl;

    interface IAudioMidrange
    {
        CONST_VTBL struct IAudioMidrangeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAudioMidrange_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAudioMidrange_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAudioMidrange_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAudioMidrange_GetChannelCount(This,pcChannels)	\
    ( (This)->lpVtbl -> GetChannelCount(This,pcChannels) ) 

#define IAudioMidrange_GetLevelRange(This,nChannel,pfMinLevelDB,pfMaxLevelDB,pfStepping)	\
    ( (This)->lpVtbl -> GetLevelRange(This,nChannel,pfMinLevelDB,pfMaxLevelDB,pfStepping) ) 

#define IAudioMidrange_GetLevel(This,nChannel,pfLevelDB)	\
    ( (This)->lpVtbl -> GetLevel(This,nChannel,pfLevelDB) ) 

#define IAudioMidrange_SetLevel(This,nChannel,fLevelDB,pguidEventContext)	\
    ( (This)->lpVtbl -> SetLevel(This,nChannel,fLevelDB,pguidEventContext) ) 

#define IAudioMidrange_SetLevelUniform(This,fLevelDB,pguidEventContext)	\
    ( (This)->lpVtbl -> SetLevelUniform(This,fLevelDB,pguidEventContext) ) 

#define IAudioMidrange_SetLevelAllChannels(This,aLevelsDB,cChannels,pguidEventContext)	\
    ( (This)->lpVtbl -> SetLevelAllChannels(This,aLevelsDB,cChannels,pguidEventContext) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAudioMidrange_INTERFACE_DEFINED__ */


#ifndef __IAudioTreble_INTERFACE_DEFINED__
#define __IAudioTreble_INTERFACE_DEFINED__

/* interface IAudioTreble */
/* [unique][helpstring][nonextensible][uuid][local][object] */ 


EXTERN_C const IID IID_IAudioTreble;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0A717812-694E-4907-B74B-BAFA5CFDCA7B")
    IAudioTreble : public IPerChannelDbLevel
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IAudioTrebleVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAudioTreble * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAudioTreble * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAudioTreble * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetChannelCount )( 
            IAudioTreble * This,
            /* [annotation][out] */ 
            __out  UINT *pcChannels);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetLevelRange )( 
            IAudioTreble * This,
            /* [annotation][in] */ 
            __in  UINT nChannel,
            /* [annotation][out] */ 
            __out  float *pfMinLevelDB,
            /* [annotation][out] */ 
            __out  float *pfMaxLevelDB,
            /* [annotation][out] */ 
            __out  float *pfStepping);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetLevel )( 
            IAudioTreble * This,
            /* [annotation][in] */ 
            __in  UINT nChannel,
            /* [annotation][out] */ 
            __out  float *pfLevelDB);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetLevel )( 
            IAudioTreble * This,
            /* [annotation][in] */ 
            __in  UINT nChannel,
            /* [annotation][in] */ 
            __in  float fLevelDB,
            /* [annotation][unique][in] */ 
            __in_opt  LPCGUID pguidEventContext);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetLevelUniform )( 
            IAudioTreble * This,
            /* [annotation][in] */ 
            __in  float fLevelDB,
            /* [annotation][unique][in] */ 
            __in_opt  LPCGUID pguidEventContext);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetLevelAllChannels )( 
            IAudioTreble * This,
            /* [annotation][size_is][in] */ 
            __in_ecount(cChannels)  float aLevelsDB[  ],
            /* [annotation][in] */ 
            __in  ULONG cChannels,
            /* [annotation][unique][in] */ 
            __in_opt  LPCGUID pguidEventContext);
        
        END_INTERFACE
    } IAudioTrebleVtbl;

    interface IAudioTreble
    {
        CONST_VTBL struct IAudioTrebleVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAudioTreble_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAudioTreble_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAudioTreble_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAudioTreble_GetChannelCount(This,pcChannels)	\
    ( (This)->lpVtbl -> GetChannelCount(This,pcChannels) ) 

#define IAudioTreble_GetLevelRange(This,nChannel,pfMinLevelDB,pfMaxLevelDB,pfStepping)	\
    ( (This)->lpVtbl -> GetLevelRange(This,nChannel,pfMinLevelDB,pfMaxLevelDB,pfStepping) ) 

#define IAudioTreble_GetLevel(This,nChannel,pfLevelDB)	\
    ( (This)->lpVtbl -> GetLevel(This,nChannel,pfLevelDB) ) 

#define IAudioTreble_SetLevel(This,nChannel,fLevelDB,pguidEventContext)	\
    ( (This)->lpVtbl -> SetLevel(This,nChannel,fLevelDB,pguidEventContext) ) 

#define IAudioTreble_SetLevelUniform(This,fLevelDB,pguidEventContext)	\
    ( (This)->lpVtbl -> SetLevelUniform(This,fLevelDB,pguidEventContext) ) 

#define IAudioTreble_SetLevelAllChannels(This,aLevelsDB,cChannels,pguidEventContext)	\
    ( (This)->lpVtbl -> SetLevelAllChannels(This,aLevelsDB,cChannels,pguidEventContext) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAudioTreble_INTERFACE_DEFINED__ */


#ifndef __IAudioAutoGainControl_INTERFACE_DEFINED__
#define __IAudioAutoGainControl_INTERFACE_DEFINED__

/* interface IAudioAutoGainControl */
/* [unique][helpstring][nonextensible][uuid][local][object] */ 


EXTERN_C const IID IID_IAudioAutoGainControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("85401FD4-6DE4-4b9d-9869-2D6753A82F3C")
    IAudioAutoGainControl : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetEnabled( 
            /* [annotation][out] */ 
            __out  BOOL *pbEnabled) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetEnabled( 
            /* [annotation][in] */ 
            __in  BOOL bEnable,
            /* [annotation][unique][in] */ 
            __in_opt  LPCGUID pguidEventContext) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAudioAutoGainControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAudioAutoGainControl * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAudioAutoGainControl * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAudioAutoGainControl * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetEnabled )( 
            IAudioAutoGainControl * This,
            /* [annotation][out] */ 
            __out  BOOL *pbEnabled);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetEnabled )( 
            IAudioAutoGainControl * This,
            /* [annotation][in] */ 
            __in  BOOL bEnable,
            /* [annotation][unique][in] */ 
            __in_opt  LPCGUID pguidEventContext);
        
        END_INTERFACE
    } IAudioAutoGainControlVtbl;

    interface IAudioAutoGainControl
    {
        CONST_VTBL struct IAudioAutoGainControlVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAudioAutoGainControl_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAudioAutoGainControl_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAudioAutoGainControl_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAudioAutoGainControl_GetEnabled(This,pbEnabled)	\
    ( (This)->lpVtbl -> GetEnabled(This,pbEnabled) ) 

#define IAudioAutoGainControl_SetEnabled(This,bEnable,pguidEventContext)	\
    ( (This)->lpVtbl -> SetEnabled(This,bEnable,pguidEventContext) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAudioAutoGainControl_INTERFACE_DEFINED__ */


#ifndef __IAudioPeakMeter_INTERFACE_DEFINED__
#define __IAudioPeakMeter_INTERFACE_DEFINED__

/* interface IAudioPeakMeter */
/* [unique][helpstring][nonextensible][uuid][local][object] */ 


EXTERN_C const IID IID_IAudioPeakMeter;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DD79923C-0599-45e0-B8B6-C8DF7DB6E796")
    IAudioPeakMeter : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetChannelCount( 
            /* [annotation][out] */ 
            __out  UINT *pcChannels) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetLevel( 
            /* [annotation][in] */ 
            __in  UINT nChannel,
            /* [annotation][out] */ 
            __out  float *pfLevel) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAudioPeakMeterVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAudioPeakMeter * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAudioPeakMeter * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAudioPeakMeter * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetChannelCount )( 
            IAudioPeakMeter * This,
            /* [annotation][out] */ 
            __out  UINT *pcChannels);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetLevel )( 
            IAudioPeakMeter * This,
            /* [annotation][in] */ 
            __in  UINT nChannel,
            /* [annotation][out] */ 
            __out  float *pfLevel);
        
        END_INTERFACE
    } IAudioPeakMeterVtbl;

    interface IAudioPeakMeter
    {
        CONST_VTBL struct IAudioPeakMeterVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAudioPeakMeter_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAudioPeakMeter_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAudioPeakMeter_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAudioPeakMeter_GetChannelCount(This,pcChannels)	\
    ( (This)->lpVtbl -> GetChannelCount(This,pcChannels) ) 

#define IAudioPeakMeter_GetLevel(This,nChannel,pfLevel)	\
    ( (This)->lpVtbl -> GetLevel(This,nChannel,pfLevel) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAudioPeakMeter_INTERFACE_DEFINED__ */


#ifndef __IDeviceSpecificProperty_INTERFACE_DEFINED__
#define __IDeviceSpecificProperty_INTERFACE_DEFINED__

/* interface IDeviceSpecificProperty */
/* [unique][helpstring][nonextensible][uuid][local][object] */ 


EXTERN_C const IID IID_IDeviceSpecificProperty;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3B22BCBF-2586-4af0-8583-205D391B807C")
    IDeviceSpecificProperty : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetType( 
            /* [annotation][out] */ 
            __out  VARTYPE *pVType) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetValue( 
            /* [annotation][out] */ 
            __out  void *pvValue,
            /* [annotation][out][in] */ 
            __inout  DWORD *pcbValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetValue( 
            /* [annotation][in] */ 
            __in  void *pvValue,
            /* [in] */ DWORD cbValue,
            /* [annotation][unique][in] */ 
            __in_opt  LPCGUID pguidEventContext) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Get4BRange( 
            /* [annotation][out] */ 
            __out  LONG *plMin,
            /* [annotation][out] */ 
            __out  LONG *plMax,
            /* [annotation][out] */ 
            __out  LONG *plStepping) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDeviceSpecificPropertyVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDeviceSpecificProperty * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDeviceSpecificProperty * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDeviceSpecificProperty * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetType )( 
            IDeviceSpecificProperty * This,
            /* [annotation][out] */ 
            __out  VARTYPE *pVType);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetValue )( 
            IDeviceSpecificProperty * This,
            /* [annotation][out] */ 
            __out  void *pvValue,
            /* [annotation][out][in] */ 
            __inout  DWORD *pcbValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetValue )( 
            IDeviceSpecificProperty * This,
            /* [annotation][in] */ 
            __in  void *pvValue,
            /* [in] */ DWORD cbValue,
            /* [annotation][unique][in] */ 
            __in_opt  LPCGUID pguidEventContext);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Get4BRange )( 
            IDeviceSpecificProperty * This,
            /* [annotation][out] */ 
            __out  LONG *plMin,
            /* [annotation][out] */ 
            __out  LONG *plMax,
            /* [annotation][out] */ 
            __out  LONG *plStepping);
        
        END_INTERFACE
    } IDeviceSpecificPropertyVtbl;

    interface IDeviceSpecificProperty
    {
        CONST_VTBL struct IDeviceSpecificPropertyVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDeviceSpecificProperty_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IDeviceSpecificProperty_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IDeviceSpecificProperty_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IDeviceSpecificProperty_GetType(This,pVType)	\
    ( (This)->lpVtbl -> GetType(This,pVType) ) 

#define IDeviceSpecificProperty_GetValue(This,pvValue,pcbValue)	\
    ( (This)->lpVtbl -> GetValue(This,pvValue,pcbValue) ) 

#define IDeviceSpecificProperty_SetValue(This,pvValue,cbValue,pguidEventContext)	\
    ( (This)->lpVtbl -> SetValue(This,pvValue,cbValue,pguidEventContext) ) 

#define IDeviceSpecificProperty_Get4BRange(This,plMin,plMax,plStepping)	\
    ( (This)->lpVtbl -> Get4BRange(This,plMin,plMax,plStepping) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IDeviceSpecificProperty_INTERFACE_DEFINED__ */


#ifndef __IKsFormatSupport_INTERFACE_DEFINED__
#define __IKsFormatSupport_INTERFACE_DEFINED__

/* interface IKsFormatSupport */
/* [unique][helpstring][nonextensible][uuid][local][object] */ 


EXTERN_C const IID IID_IKsFormatSupport;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3CB4A69D-BB6F-4D2B-95B7-452D2C155DB5")
    IKsFormatSupport : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsFormatSupported( 
            /* [size_is][in] */ PKSDATAFORMAT pKsFormat,
            /* [annotation][in] */ 
            __in  DWORD cbFormat,
            /* [annotation][out] */ 
            __out  BOOL *pbSupported) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDevicePreferredFormat( 
            /* [out] */ PKSDATAFORMAT *ppKsFormat) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IKsFormatSupportVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IKsFormatSupport * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IKsFormatSupport * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IKsFormatSupport * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *IsFormatSupported )( 
            IKsFormatSupport * This,
            /* [size_is][in] */ PKSDATAFORMAT pKsFormat,
            /* [annotation][in] */ 
            __in  DWORD cbFormat,
            /* [annotation][out] */ 
            __out  BOOL *pbSupported);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDevicePreferredFormat )( 
            IKsFormatSupport * This,
            /* [out] */ PKSDATAFORMAT *ppKsFormat);
        
        END_INTERFACE
    } IKsFormatSupportVtbl;

    interface IKsFormatSupport
    {
        CONST_VTBL struct IKsFormatSupportVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IKsFormatSupport_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IKsFormatSupport_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IKsFormatSupport_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IKsFormatSupport_IsFormatSupported(This,pKsFormat,cbFormat,pbSupported)	\
    ( (This)->lpVtbl -> IsFormatSupported(This,pKsFormat,cbFormat,pbSupported) ) 

#define IKsFormatSupport_GetDevicePreferredFormat(This,ppKsFormat)	\
    ( (This)->lpVtbl -> GetDevicePreferredFormat(This,ppKsFormat) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IKsFormatSupport_INTERFACE_DEFINED__ */


#ifndef __IKsJackDescription_INTERFACE_DEFINED__
#define __IKsJackDescription_INTERFACE_DEFINED__

/* interface IKsJackDescription */
/* [unique][helpstring][nonextensible][uuid][local][object] */ 


EXTERN_C const IID IID_IKsJackDescription;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4509F757-2D46-4637-8E62-CE7DB944F57B")
    IKsJackDescription : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetJackCount( 
            /* [annotation][out] */ 
            __out  UINT *pcJacks) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetJackDescription( 
            /* [in] */ UINT nJack,
            /* [annotation][out] */ 
            __out  KSJACK_DESCRIPTION *pDescription) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IKsJackDescriptionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IKsJackDescription * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IKsJackDescription * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IKsJackDescription * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetJackCount )( 
            IKsJackDescription * This,
            /* [annotation][out] */ 
            __out  UINT *pcJacks);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetJackDescription )( 
            IKsJackDescription * This,
            /* [in] */ UINT nJack,
            /* [annotation][out] */ 
            __out  KSJACK_DESCRIPTION *pDescription);
        
        END_INTERFACE
    } IKsJackDescriptionVtbl;

    interface IKsJackDescription
    {
        CONST_VTBL struct IKsJackDescriptionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IKsJackDescription_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IKsJackDescription_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IKsJackDescription_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IKsJackDescription_GetJackCount(This,pcJacks)	\
    ( (This)->lpVtbl -> GetJackCount(This,pcJacks) ) 

#define IKsJackDescription_GetJackDescription(This,nJack,pDescription)	\
    ( (This)->lpVtbl -> GetJackDescription(This,nJack,pDescription) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IKsJackDescription_INTERFACE_DEFINED__ */


#ifndef __IKsJackDescription2_INTERFACE_DEFINED__
#define __IKsJackDescription2_INTERFACE_DEFINED__

/* interface IKsJackDescription2 */
/* [unique][helpstring][nonextensible][uuid][local][object] */ 


EXTERN_C const IID IID_IKsJackDescription2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("478F3A9B-E0C9-4827-9228-6F5505FFE76A")
    IKsJackDescription2 : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetJackCount( 
            /* [annotation][out] */ 
            __out  UINT *pcJacks) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetJackDescription2( 
            /* [in] */ UINT nJack,
            /* [annotation][out] */ 
            __out  KSJACK_DESCRIPTION2 *pDescription2) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IKsJackDescription2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IKsJackDescription2 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IKsJackDescription2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IKsJackDescription2 * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetJackCount )( 
            IKsJackDescription2 * This,
            /* [annotation][out] */ 
            __out  UINT *pcJacks);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetJackDescription2 )( 
            IKsJackDescription2 * This,
            /* [in] */ UINT nJack,
            /* [annotation][out] */ 
            __out  KSJACK_DESCRIPTION2 *pDescription2);
        
        END_INTERFACE
    } IKsJackDescription2Vtbl;

    interface IKsJackDescription2
    {
        CONST_VTBL struct IKsJackDescription2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IKsJackDescription2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IKsJackDescription2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IKsJackDescription2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IKsJackDescription2_GetJackCount(This,pcJacks)	\
    ( (This)->lpVtbl -> GetJackCount(This,pcJacks) ) 

#define IKsJackDescription2_GetJackDescription2(This,nJack,pDescription2)	\
    ( (This)->lpVtbl -> GetJackDescription2(This,nJack,pDescription2) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IKsJackDescription2_INTERFACE_DEFINED__ */


#ifndef __IKsJackSinkInformation_INTERFACE_DEFINED__
#define __IKsJackSinkInformation_INTERFACE_DEFINED__

/* interface IKsJackSinkInformation */
/* [unique][helpstring][nonextensible][uuid][local][object] */ 


EXTERN_C const IID IID_IKsJackSinkInformation;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D9BD72ED-290F-4581-9FF3-61027A8FE532")
    IKsJackSinkInformation : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetJackSinkInformation( 
            /* [annotation][out] */ 
            __out  KSJACK_SINK_INFORMATION *pJackSinkInformation) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IKsJackSinkInformationVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IKsJackSinkInformation * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IKsJackSinkInformation * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IKsJackSinkInformation * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetJackSinkInformation )( 
            IKsJackSinkInformation * This,
            /* [annotation][out] */ 
            __out  KSJACK_SINK_INFORMATION *pJackSinkInformation);
        
        END_INTERFACE
    } IKsJackSinkInformationVtbl;

    interface IKsJackSinkInformation
    {
        CONST_VTBL struct IKsJackSinkInformationVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IKsJackSinkInformation_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IKsJackSinkInformation_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IKsJackSinkInformation_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IKsJackSinkInformation_GetJackSinkInformation(This,pJackSinkInformation)	\
    ( (This)->lpVtbl -> GetJackSinkInformation(This,pJackSinkInformation) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IKsJackSinkInformation_INTERFACE_DEFINED__ */


#ifndef __IPartsList_INTERFACE_DEFINED__
#define __IPartsList_INTERFACE_DEFINED__

/* interface IPartsList */
/* [object][unique][helpstring][uuid][local] */ 


EXTERN_C const IID IID_IPartsList;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6DAA848C-5EB0-45CC-AEA5-998A2CDA1FFB")
    IPartsList : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCount( 
            /* [annotation][out] */ 
            __out  UINT *pCount) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPart( 
            /* [annotation][in] */ 
            __in  UINT nIndex,
            /* [annotation][out] */ 
            __out  IPart **ppPart) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPartsListVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPartsList * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPartsList * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPartsList * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCount )( 
            IPartsList * This,
            /* [annotation][out] */ 
            __out  UINT *pCount);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPart )( 
            IPartsList * This,
            /* [annotation][in] */ 
            __in  UINT nIndex,
            /* [annotation][out] */ 
            __out  IPart **ppPart);
        
        END_INTERFACE
    } IPartsListVtbl;

    interface IPartsList
    {
        CONST_VTBL struct IPartsListVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPartsList_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IPartsList_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IPartsList_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IPartsList_GetCount(This,pCount)	\
    ( (This)->lpVtbl -> GetCount(This,pCount) ) 

#define IPartsList_GetPart(This,nIndex,ppPart)	\
    ( (This)->lpVtbl -> GetPart(This,nIndex,ppPart) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IPartsList_INTERFACE_DEFINED__ */


#ifndef __IPart_INTERFACE_DEFINED__
#define __IPart_INTERFACE_DEFINED__

/* interface IPart */
/* [object][unique][helpstring][uuid][local] */ 


EXTERN_C const IID IID_IPart;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("AE2DE0E4-5BCA-4F2D-AA46-5D13F8FDB3A9")
    IPart : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetName( 
            /* [annotation][out] */ 
            __deref_out  LPWSTR *ppwstrName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetLocalId( 
            /* [annotation][out] */ 
            __out  UINT *pnId) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetGlobalId( 
            /* [annotation][out] */ 
            __deref_out  LPWSTR *ppwstrGlobalId) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPartType( 
            /* [annotation][out] */ 
            __out  PartType *pPartType) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSubType( 
            /* [out] */ GUID *pSubType) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetControlInterfaceCount( 
            /* [annotation][out] */ 
            __out  UINT *pCount) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetControlInterface( 
            /* [annotation][in] */ 
            __in  UINT nIndex,
            /* [annotation][out] */ 
            __out  IControlInterface **ppInterfaceDesc) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE EnumPartsIncoming( 
            /* [annotation][out] */ 
            __out  IPartsList **ppParts) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE EnumPartsOutgoing( 
            /* [annotation][out] */ 
            __out  IPartsList **ppParts) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetTopologyObject( 
            /* [annotation][out] */ 
            __out  IDeviceTopology **ppTopology) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Activate( 
            /* [annotation][in] */ 
            __in  DWORD dwClsContext,
            /* [annotation][in] */ 
            __in  REFIID refiid,
            /* [annotation][iid_is][out] */ 
            __out_opt  void **ppvObject) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RegisterControlChangeCallback( 
            /* [annotation][in] */ 
            __in  REFGUID riid,
            /* [annotation][in] */ 
            __in  IControlChangeNotify *pNotify) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UnregisterControlChangeCallback( 
            /* [annotation][in] */ 
            __in  IControlChangeNotify *pNotify) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPartVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPart * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPart * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPart * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetName )( 
            IPart * This,
            /* [annotation][out] */ 
            __deref_out  LPWSTR *ppwstrName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetLocalId )( 
            IPart * This,
            /* [annotation][out] */ 
            __out  UINT *pnId);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetGlobalId )( 
            IPart * This,
            /* [annotation][out] */ 
            __deref_out  LPWSTR *ppwstrGlobalId);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPartType )( 
            IPart * This,
            /* [annotation][out] */ 
            __out  PartType *pPartType);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetSubType )( 
            IPart * This,
            /* [out] */ GUID *pSubType);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetControlInterfaceCount )( 
            IPart * This,
            /* [annotation][out] */ 
            __out  UINT *pCount);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetControlInterface )( 
            IPart * This,
            /* [annotation][in] */ 
            __in  UINT nIndex,
            /* [annotation][out] */ 
            __out  IControlInterface **ppInterfaceDesc);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *EnumPartsIncoming )( 
            IPart * This,
            /* [annotation][out] */ 
            __out  IPartsList **ppParts);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *EnumPartsOutgoing )( 
            IPart * This,
            /* [annotation][out] */ 
            __out  IPartsList **ppParts);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetTopologyObject )( 
            IPart * This,
            /* [annotation][out] */ 
            __out  IDeviceTopology **ppTopology);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Activate )( 
            IPart * This,
            /* [annotation][in] */ 
            __in  DWORD dwClsContext,
            /* [annotation][in] */ 
            __in  REFIID refiid,
            /* [annotation][iid_is][out] */ 
            __out_opt  void **ppvObject);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RegisterControlChangeCallback )( 
            IPart * This,
            /* [annotation][in] */ 
            __in  REFGUID riid,
            /* [annotation][in] */ 
            __in  IControlChangeNotify *pNotify);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *UnregisterControlChangeCallback )( 
            IPart * This,
            /* [annotation][in] */ 
            __in  IControlChangeNotify *pNotify);
        
        END_INTERFACE
    } IPartVtbl;

    interface IPart
    {
        CONST_VTBL struct IPartVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPart_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IPart_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IPart_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IPart_GetName(This,ppwstrName)	\
    ( (This)->lpVtbl -> GetName(This,ppwstrName) ) 

#define IPart_GetLocalId(This,pnId)	\
    ( (This)->lpVtbl -> GetLocalId(This,pnId) ) 

#define IPart_GetGlobalId(This,ppwstrGlobalId)	\
    ( (This)->lpVtbl -> GetGlobalId(This,ppwstrGlobalId) ) 

#define IPart_GetPartType(This,pPartType)	\
    ( (This)->lpVtbl -> GetPartType(This,pPartType) ) 

#define IPart_GetSubType(This,pSubType)	\
    ( (This)->lpVtbl -> GetSubType(This,pSubType) ) 

#define IPart_GetControlInterfaceCount(This,pCount)	\
    ( (This)->lpVtbl -> GetControlInterfaceCount(This,pCount) ) 

#define IPart_GetControlInterface(This,nIndex,ppInterfaceDesc)	\
    ( (This)->lpVtbl -> GetControlInterface(This,nIndex,ppInterfaceDesc) ) 

#define IPart_EnumPartsIncoming(This,ppParts)	\
    ( (This)->lpVtbl -> EnumPartsIncoming(This,ppParts) ) 

#define IPart_EnumPartsOutgoing(This,ppParts)	\
    ( (This)->lpVtbl -> EnumPartsOutgoing(This,ppParts) ) 

#define IPart_GetTopologyObject(This,ppTopology)	\
    ( (This)->lpVtbl -> GetTopologyObject(This,ppTopology) ) 

#define IPart_Activate(This,dwClsContext,refiid,ppvObject)	\
    ( (This)->lpVtbl -> Activate(This,dwClsContext,refiid,ppvObject) ) 

#define IPart_RegisterControlChangeCallback(This,riid,pNotify)	\
    ( (This)->lpVtbl -> RegisterControlChangeCallback(This,riid,pNotify) ) 

#define IPart_UnregisterControlChangeCallback(This,pNotify)	\
    ( (This)->lpVtbl -> UnregisterControlChangeCallback(This,pNotify) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IPart_INTERFACE_DEFINED__ */


#ifndef __IConnector_INTERFACE_DEFINED__
#define __IConnector_INTERFACE_DEFINED__

/* interface IConnector */
/* [object][unique][helpstring][uuid][local] */ 


EXTERN_C const IID IID_IConnector;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9c2c4058-23f5-41de-877a-df3af236a09e")
    IConnector : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetType( 
            /* [annotation][out] */ 
            __out  ConnectorType *pType) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDataFlow( 
            /* [annotation][out] */ 
            __out  DataFlow *pFlow) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ConnectTo( 
            /* [annotation][in] */ 
            __in  IConnector *pConnectTo) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Disconnect( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsConnected( 
            /* [annotation][out] */ 
            __out  BOOL *pbConnected) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetConnectedTo( 
            /* [annotation][out] */ 
            __out  IConnector **ppConTo) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetConnectorIdConnectedTo( 
            /* [annotation][out] */ 
            __deref_out  LPWSTR *ppwstrConnectorId) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDeviceIdConnectedTo( 
            /* [annotation][out] */ 
            __deref_out  LPWSTR *ppwstrDeviceId) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IConnectorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IConnector * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IConnector * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IConnector * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetType )( 
            IConnector * This,
            /* [annotation][out] */ 
            __out  ConnectorType *pType);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDataFlow )( 
            IConnector * This,
            /* [annotation][out] */ 
            __out  DataFlow *pFlow);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ConnectTo )( 
            IConnector * This,
            /* [annotation][in] */ 
            __in  IConnector *pConnectTo);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Disconnect )( 
            IConnector * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *IsConnected )( 
            IConnector * This,
            /* [annotation][out] */ 
            __out  BOOL *pbConnected);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetConnectedTo )( 
            IConnector * This,
            /* [annotation][out] */ 
            __out  IConnector **ppConTo);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetConnectorIdConnectedTo )( 
            IConnector * This,
            /* [annotation][out] */ 
            __deref_out  LPWSTR *ppwstrConnectorId);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDeviceIdConnectedTo )( 
            IConnector * This,
            /* [annotation][out] */ 
            __deref_out  LPWSTR *ppwstrDeviceId);
        
        END_INTERFACE
    } IConnectorVtbl;

    interface IConnector
    {
        CONST_VTBL struct IConnectorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IConnector_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IConnector_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IConnector_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IConnector_GetType(This,pType)	\
    ( (This)->lpVtbl -> GetType(This,pType) ) 

#define IConnector_GetDataFlow(This,pFlow)	\
    ( (This)->lpVtbl -> GetDataFlow(This,pFlow) ) 

#define IConnector_ConnectTo(This,pConnectTo)	\
    ( (This)->lpVtbl -> ConnectTo(This,pConnectTo) ) 

#define IConnector_Disconnect(This)	\
    ( (This)->lpVtbl -> Disconnect(This) ) 

#define IConnector_IsConnected(This,pbConnected)	\
    ( (This)->lpVtbl -> IsConnected(This,pbConnected) ) 

#define IConnector_GetConnectedTo(This,ppConTo)	\
    ( (This)->lpVtbl -> GetConnectedTo(This,ppConTo) ) 

#define IConnector_GetConnectorIdConnectedTo(This,ppwstrConnectorId)	\
    ( (This)->lpVtbl -> GetConnectorIdConnectedTo(This,ppwstrConnectorId) ) 

#define IConnector_GetDeviceIdConnectedTo(This,ppwstrDeviceId)	\
    ( (This)->lpVtbl -> GetDeviceIdConnectedTo(This,ppwstrDeviceId) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IConnector_INTERFACE_DEFINED__ */


#ifndef __ISubunit_INTERFACE_DEFINED__
#define __ISubunit_INTERFACE_DEFINED__

/* interface ISubunit */
/* [object][unique][helpstring][uuid][local] */ 


EXTERN_C const IID IID_ISubunit;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("82149A85-DBA6-4487-86BB-EA8F7FEFCC71")
    ISubunit : public IUnknown
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct ISubunitVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISubunit * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISubunit * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISubunit * This);
        
        END_INTERFACE
    } ISubunitVtbl;

    interface ISubunit
    {
        CONST_VTBL struct ISubunitVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISubunit_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISubunit_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISubunit_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISubunit_INTERFACE_DEFINED__ */


#ifndef __IControlInterface_INTERFACE_DEFINED__
#define __IControlInterface_INTERFACE_DEFINED__

/* interface IControlInterface */
/* [object][unique][helpstring][uuid][local] */ 


EXTERN_C const IID IID_IControlInterface;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("45d37c3f-5140-444a-ae24-400789f3cbf3")
    IControlInterface : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetName( 
            /* [annotation][out] */ 
            __deref_out  LPWSTR *ppwstrName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetIID( 
            /* [annotation][out] */ 
            __out  GUID *pIID) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IControlInterfaceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IControlInterface * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IControlInterface * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IControlInterface * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetName )( 
            IControlInterface * This,
            /* [annotation][out] */ 
            __deref_out  LPWSTR *ppwstrName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetIID )( 
            IControlInterface * This,
            /* [annotation][out] */ 
            __out  GUID *pIID);
        
        END_INTERFACE
    } IControlInterfaceVtbl;

    interface IControlInterface
    {
        CONST_VTBL struct IControlInterfaceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IControlInterface_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IControlInterface_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IControlInterface_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IControlInterface_GetName(This,ppwstrName)	\
    ( (This)->lpVtbl -> GetName(This,ppwstrName) ) 

#define IControlInterface_GetIID(This,pIID)	\
    ( (This)->lpVtbl -> GetIID(This,pIID) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IControlInterface_INTERFACE_DEFINED__ */


#ifndef __IControlChangeNotify_INTERFACE_DEFINED__
#define __IControlChangeNotify_INTERFACE_DEFINED__

/* interface IControlChangeNotify */
/* [unique][helpstring][nonextensible][uuid][local][object] */ 


EXTERN_C const IID IID_IControlChangeNotify;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A09513ED-C709-4d21-BD7B-5F34C47F3947")
    IControlChangeNotify : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnNotify( 
            /* [annotation][in] */ 
            __in  DWORD dwSenderProcessId,
            /* [annotation][unique][in] */ 
            __in_opt  LPCGUID pguidEventContext) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IControlChangeNotifyVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IControlChangeNotify * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IControlChangeNotify * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IControlChangeNotify * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnNotify )( 
            IControlChangeNotify * This,
            /* [annotation][in] */ 
            __in  DWORD dwSenderProcessId,
            /* [annotation][unique][in] */ 
            __in_opt  LPCGUID pguidEventContext);
        
        END_INTERFACE
    } IControlChangeNotifyVtbl;

    interface IControlChangeNotify
    {
        CONST_VTBL struct IControlChangeNotifyVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IControlChangeNotify_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IControlChangeNotify_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IControlChangeNotify_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IControlChangeNotify_OnNotify(This,dwSenderProcessId,pguidEventContext)	\
    ( (This)->lpVtbl -> OnNotify(This,dwSenderProcessId,pguidEventContext) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IControlChangeNotify_INTERFACE_DEFINED__ */


#ifndef __IDeviceTopology_INTERFACE_DEFINED__
#define __IDeviceTopology_INTERFACE_DEFINED__

/* interface IDeviceTopology */
/* [unique][helpstring][nonextensible][uuid][local][object] */ 


EXTERN_C const IID IID_IDeviceTopology;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2A07407E-6497-4A18-9787-32F79BD0D98F")
    IDeviceTopology : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetConnectorCount( 
            /* [annotation][out] */ 
            __out  UINT *pCount) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetConnector( 
            /* [annotation][in] */ 
            __in  UINT nIndex,
            /* [annotation][out] */ 
            __out  IConnector **ppConnector) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSubunitCount( 
            /* [annotation][out] */ 
            __out  UINT *pCount) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSubunit( 
            /* [annotation][in] */ 
            __in  UINT nIndex,
            /* [annotation][out] */ 
            __deref_out  ISubunit **ppSubunit) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPartById( 
            /* [annotation][in] */ 
            __in  UINT nId,
            /* [annotation][out] */ 
            __deref_out  IPart **ppPart) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDeviceId( 
            /* [annotation][out] */ 
            __deref_out  LPWSTR *ppwstrDeviceId) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSignalPath( 
            /* [annotation][in] */ 
            __in  IPart *pIPartFrom,
            /* [annotation][in] */ 
            __in  IPart *pIPartTo,
            /* [annotation][in] */ 
            __in  BOOL bRejectMixedPaths,
            /* [annotation][out] */ 
            __deref_out  IPartsList **ppParts) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDeviceTopologyVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDeviceTopology * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDeviceTopology * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDeviceTopology * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetConnectorCount )( 
            IDeviceTopology * This,
            /* [annotation][out] */ 
            __out  UINT *pCount);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetConnector )( 
            IDeviceTopology * This,
            /* [annotation][in] */ 
            __in  UINT nIndex,
            /* [annotation][out] */ 
            __out  IConnector **ppConnector);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetSubunitCount )( 
            IDeviceTopology * This,
            /* [annotation][out] */ 
            __out  UINT *pCount);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetSubunit )( 
            IDeviceTopology * This,
            /* [annotation][in] */ 
            __in  UINT nIndex,
            /* [annotation][out] */ 
            __deref_out  ISubunit **ppSubunit);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPartById )( 
            IDeviceTopology * This,
            /* [annotation][in] */ 
            __in  UINT nId,
            /* [annotation][out] */ 
            __deref_out  IPart **ppPart);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDeviceId )( 
            IDeviceTopology * This,
            /* [annotation][out] */ 
            __deref_out  LPWSTR *ppwstrDeviceId);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetSignalPath )( 
            IDeviceTopology * This,
            /* [annotation][in] */ 
            __in  IPart *pIPartFrom,
            /* [annotation][in] */ 
            __in  IPart *pIPartTo,
            /* [annotation][in] */ 
            __in  BOOL bRejectMixedPaths,
            /* [annotation][out] */ 
            __deref_out  IPartsList **ppParts);
        
        END_INTERFACE
    } IDeviceTopologyVtbl;

    interface IDeviceTopology
    {
        CONST_VTBL struct IDeviceTopologyVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDeviceTopology_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IDeviceTopology_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IDeviceTopology_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IDeviceTopology_GetConnectorCount(This,pCount)	\
    ( (This)->lpVtbl -> GetConnectorCount(This,pCount) ) 

#define IDeviceTopology_GetConnector(This,nIndex,ppConnector)	\
    ( (This)->lpVtbl -> GetConnector(This,nIndex,ppConnector) ) 

#define IDeviceTopology_GetSubunitCount(This,pCount)	\
    ( (This)->lpVtbl -> GetSubunitCount(This,pCount) ) 

#define IDeviceTopology_GetSubunit(This,nIndex,ppSubunit)	\
    ( (This)->lpVtbl -> GetSubunit(This,nIndex,ppSubunit) ) 

#define IDeviceTopology_GetPartById(This,nId,ppPart)	\
    ( (This)->lpVtbl -> GetPartById(This,nId,ppPart) ) 

#define IDeviceTopology_GetDeviceId(This,ppwstrDeviceId)	\
    ( (This)->lpVtbl -> GetDeviceId(This,ppwstrDeviceId) ) 

#define IDeviceTopology_GetSignalPath(This,pIPartFrom,pIPartTo,bRejectMixedPaths,ppParts)	\
    ( (This)->lpVtbl -> GetSignalPath(This,pIPartFrom,pIPartTo,bRejectMixedPaths,ppParts) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IDeviceTopology_INTERFACE_DEFINED__ */



#ifndef __DevTopologyLib_LIBRARY_DEFINED__
#define __DevTopologyLib_LIBRARY_DEFINED__

/* library DevTopologyLib */
/* [helpstring][version][uuid] */ 
















EXTERN_C const IID LIBID_DevTopologyLib;

EXTERN_C const CLSID CLSID_DeviceTopology;

#ifdef __cplusplus

class DECLSPEC_UUID("1DF639D0-5EC1-47AA-9379-828DC1AA8C59")
DeviceTopology;
#endif
#endif /* __DevTopologyLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



