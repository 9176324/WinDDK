//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.
//
//==========================================================================;

#ifndef __BDATUNER_H__
#define __BDATUNER_H__


#include "common.h"

#include "bdadebug.h"
#include <ntstrsafe.h>
#include <pbdaerrors.h>

#define IsEqualGUID(rguid1, rguid2) (!memcmp(rguid1, rguid2, sizeof(GUID)))
#define IsEqualBDADeviceResource(res1, res2) (!memcmp(res1, res2, sizeof(BDATUNER_DEVICE_RESOURCE)))


/**************************************************************/
/* Driver Name - Change the driver name to reflect your executable name! */
/**************************************************************/


#define MODULENAME           "BDA Generic Tuner Sample"
#define MODULENAMEUNICODE   L"BDA Generic Tuner Sample"

#define STR_MODULENAME      MODULENAME

// This defines the name of the WMI device that manages service IOCTLS
#define DEVICENAME (L"\\\\.\\" MODULENAMEUNICODE)
#define SYMBOLICNAME (L"\\DosDevices\\" MODULENAMEUNICODE)

//#define ATSC_RECEIVER   1
//#define DVBS_RECEIVER    1
//#define DVBT_RECEIVER    1
//#define DVBC_RECEIVER   1
//#define DVBT_RECEIVER    1
//#define ISDBT_RECEIVER   1
//#define ISDBS_RECEIVER   1
//#define QAM_RECEIVER 1
//#define PBDAT_RECEIVER 1

// Must define exactly one of the 5 above
# if !(defined(ATSC_RECEIVER) || defined(DVBT_RECEIVER) || defined(DVBS_RECEIVER) || defined (DVBC_RECEIVER) || defined (QAM_RECEIVER) || defined (ISDBT_RECEIVER) || defined (ISDBS_RECEIVER) || defined(NETOP_RECEIVER) || defined(PBDAT_RECEIVER) || defined(DMBTH_RECEIVER))
#error "Must define exactly one of ATSC, DVBT, DMBTH, DVBS CABLE, ISDBT or Netop"
#endif
# if defined(ATSC_RECEIVER) && (defined(DVBT_RECEIVER) || defined(DVBS_RECEIVER) || defined (DVBC_RECEIVER)|| defined (ISDBT_RECEIVER) || defined(NETOP_RECEIVER) || defined(PBDAT_RECEIVER) || defined(DMBTH_RECEIVER))
#error "Multiple tranport definitions"
# elif defined(DVBT_RECEIVER) && (defined(ATSC_RECEIVER) || defined(DVBS_RECEIVER) || defined (DVBC_RECEIVER)|| defined (ISDBT_RECEIVER) || defined(NETOP_RECEIVER) || defined(PBDAT_RECEIVER) || defined(DMBTH_RECEIVER))
#error "Multiple tranport definitions"
# elif defined(DVBS_RECEIVER) && (defined(ATSC_RECEIVER) || defined(DVBT_RECEIVER) || defined (DVBC_RECEIVER)|| defined (ISDBT_RECEIVER) || defined(NETOP_RECEIVER) || defined(PBDAT_RECEIVER) || defined(DMBTH_RECEIVER))
#error "Multiple tranport definitions"
# elif defined(DVBC_RECEIVER) && (defined(ATSC_RECEIVER) || defined(DVBS_RECEIVER) || defined (DVBT_RECEIVER) || defined (ISDBT_RECEIVER) || defined(NETOP_RECEIVER) || defined(PBDAT_RECEIVER) || defined(DMBTH_RECEIVER))
#error "Multiple tranport definitions"
# elif defined(QAM_RECEIVER) && (defined(ATSC_RECEIVER) || defined(DVBT_RECEIVER) || defined(DVBS_RECEIVER) || defined (DVBC_RECEIVER)|| defined (ISDBT_RECEIVER) || defined(NETOP_RECEIVER) || defined(PBDAT_RECEIVER) || defined(DMBTH_RECEIVER))
#error "Multiple tranport definitions"
# elif defined(ISDBT_RECEIVER) && (defined(ATSC_RECEIVER) || defined(DVBS_RECEIVER) || defined (DVBT_RECEIVER) || defined (DVBC_RECEIVER) || defined(NETOP_RECEIVER) || defined(PBDAT_RECEIVER) || defined(DMBTH_RECEIVER))
#error "Multiple tranport definitions"
# elif defined(ISDBS_RECEIVER) && (defined(ATSC_RECEIVER) || defined(DVBS_RECEIVER) || defined (DVBT_RECEIVER) || defined (DVBC_RECEIVER) || defined (ISDBT_RECEIVER) || defined(NETOP_RECEIVER) || defined(PBDAT_RECEIVER) || defined(DMBTH_RECEIVER))
#error "Multiple tranport definitions"
# elif defined(NETOP_RECEIVER) && (defined(ATSC_RECEIVER) || defined(DVBS_RECEIVER) || defined (DVBT_RECEIVER) || defined (DVBC_RECEIVER) || defined(PBDAT_RECEIVER) || defined(DMBTH_RECEIVER))
#error "Multiple tranport definitions"
# elif defined(PBDAT_RECEIVER) && (defined(ATSC_RECEIVER) || defined(DVBS_RECEIVER) || defined (DVBT_RECEIVER) || defined (DVBC_RECEIVER) || defined(NETOP_RECEIVER) || defined(DMBTH_RECEIVER))
#error "Multiple tranport definitions"
# elif defined(DMBTH_RECEIVER) && (defined(ATSC_RECEIVER) || defined(DVBS_RECEIVER) || defined (DVBT_RECEIVER) || defined (DVBC_RECEIVER) || defined(NETOP_RECEIVER) || defined(PBDAT_RECEIVER))///DMBTH ADD
#error "Multiple tranport definitions"
#endif

#define MS_SAMPLE_TUNER_POOL_TAG 'TadB'

#define MS_SAMPLE_CAPTURE_POOL_TAG 'CadB'
#define MS_SAMPLE_ANALOG_POOL_TAG 'AadB'

//#define PS_BANDWIDTH 10 //Bandwidth in mega bits per second
#ifndef FRAME_RATE
#define FRAME_RATE 60
#endif

#define PS_PACKET_SIZE 188
#define PS_PACKETS_PER_SAMPLE 312
#define PS_SAMPLE_SIZE PS_PACKET_SIZE * PS_PACKETS_PER_SAMPLE
#define VIDEO_READ_BUFFER_SIZE PS_SAMPLE_SIZE*10
//#define NANO_SECONDS 1000000000


#define INVALID_TUNER_EVENT_ID     0

#ifndef mmioFOURCC    
#define mmioFOURCC( ch0, ch1, ch2, ch3 )                \
        ( (DWORD)(BYTE)(ch0) | ( (DWORD)(BYTE)(ch1) << 8 ) |    \
        ( (DWORD)(BYTE)(ch2) << 16 ) | ( (DWORD)(BYTE)(ch3) << 24 ) )
#endif

#ifdef ISDBT_RECEIVER
#define USE_DRM_SERVICE 1
#endif

#ifdef ISDBS_RECEIVER
#define USE_DRM_SERVICE 1
#endif

#ifdef DVBS_RECEIVER
#define USE_DRM_SERVICE 1
//test definition for a PBDA enhanced DVBS receiver to allow CAS and DRM services for DVBS 
#endif

#ifdef NETOP_RECEIVER
#define USE_DRM_SERVICE 1
#define USE_GPNV_UNICODE 1
#endif

#ifdef PBDAT_RECEIVER
#define USE_DRM_SERVICE 1
#define USE_GPNV_UNICODE 1
#endif

#ifdef DMBTH_RECEIVER    ///DMBTH ADD
#define USE_DRM_SERVICE 1 
#define USE_GPNV_UNICODE 1
#endif

#define RtlStringCbCopy RtlStringCbCopyW


// {2C8C8C28-9F12-4a1e-BD2B-F08677D44460}
static const GUID TEST_GUID = { 0x2c8c8c28, 0x9f12, 0x4a1e, { 0xbd, 0x2b, 0xf0, 0x86, 0x77, 0xd4, 0x44, 0x60 } };

// {2C8C8C28-9F12-4a1e-BD2B-F08677D44460}
static const GUID TEST_GUID2 = { 0x2c8c8c28, 0x9f12, 0x4a1e, { 0xbd, 0x2b, 0xf0, 0x86, 0x77, 0xd4, 0x44, 0x60 } };

// {C80C0DF3-6052-4c16-9F56-C44C21F73C45}
static const GUID PBDA_GUIDE_DATATYPE = { 0xC80C0DF3, 0x6052, 0x4c16, { 0x9F, 0x56, 0xC4, 0x4C, 0x21, 0xF7, 0x3C, 0x45 } };

// MMI GUIDs
DEFINE_GUIDSTRUCT("E1F6DAE1-D4E4-485e-B5D8-E52C1E70B4BB", GUID_MMI_SIMPLE_HTML_PANEL);
#define GUID_MMI_SIMPLE_HTML_PANEL DEFINE_GUIDNAMED(GUID_MMI_SIMPLE_HTML_PANEL)

DEFINE_GUIDSTRUCT("88160063-B06E-4129-91DD-FC622661A480", GUID_MMI_SIMPLE_HTML_FULLSCREEN);
#define GUID_MMI_SIMPLE_HTML_FULLSCREEN DEFINE_GUIDNAMED(GUID_MMI_SIMPLE_HTML_FULLSCREEN)

DEFINE_GUIDSTRUCT("27B378EC-6F64-48b9-9325-B9C3CDCC718E", GUID_MMI_MCML_HANDLER);
#define GUID_MMI_MCML_HANDLER DEFINE_GUIDNAMED(GUID_MMI_MCML_HANDLER)

DEFINE_GUIDSTRUCT("35C84FAE-88F8-49d1-AB9B-0826598F36F7", GUID_MMI_HTML_HANDLER);
#define GUID_MMI_HTML_HANDLER DEFINE_GUIDNAMED(GUID_MMI_HTML_HANDLER)

DEFINE_GUIDSTRUCT("7F0588FF-6256-4b77-A456-35E99F31E686", GUID_MMI_PURCHASE_ENTITLEMENT_HANDLER);
#define GUID_MMI_PURCHASE_ENTITLEMENT_HANDLER DEFINE_GUIDNAMED(GUID_MMI_PURCHASE_ENTITLEMENT_HANDLER)

// {87CE35B9-E2E6-4486-93D8-D6AEE07DBCE4}
DEFINE_GUIDSTRUCT("87CE35B9-E2E6-4486-93D8-D6AEE07DBCE4", TEST_GUID3);
#define TEST_GUID3 DEFINE_GUIDNAMED(TEST_GUID3)


#define FOURCC_YUV422       mmioFOURCC('U', 'Y', 'V', 'Y')

/*************************************************

    Enums / Typedefs

*************************************************/

typedef enum _HARDWARE_STATE {

    HardwareStopped = 0,
    HardwarePaused,
    HardwareRunning

} HARDWARE_STATE, *PHARDWARE_STATE;



//  Define a structure that represents what the underlying device can do.
//
//  Note -  It is possible to set conflicting settings.  In this case, the 
//  CFilter::CheckChanges method should return an error. Only a 
//  self-consistent resource should be submitted to the underlying device.
//
typedef struct _BDATUNER_DEVICE_RESOURCE
{
    //  Tuner Resources
    //
    ULONG               ulCarrierFrequency;
    ULONG               ulFrequencyMultiplier;
    ULONG               ulPolarity;
    ULONG               ulDiseqC;
    BOOL                fDiseqCEnable;
    ULONG               ulPLPNumber;
    GUID                guidDemodulator;

    //  Demodulator Resources
    //
    ULONG               ulDemodProperty1;
    ULONG               ulDemodProperty2;
    ULONG               ulDemodProperty3;
} BDATUNER_DEVICE_RESOURCE, * PBDATUNER_DEVICE_RESOURCE;


//  Define a structure that represents the underlying device status.
//
typedef struct _BDATUNER_DEVICE_STATUS
{
    //  Tuner Status
    //
    DWORD               dwSignalStrength;
    DWORD               dwSignalQuality;
    BOOLEAN             fCarrierPresent;

    //  Demodulator Status
    //
    BOOLEAN             fSignalLocked;
    BOOLEAN             fStatusNeedSignaling;
} BDATUNER_DEVICE_STATUS, * PBDATUNER_DEVICE_STATUS;


extern const KSDEVICE_DESCRIPTOR DeviceDescriptor;

typedef struct _PBDA_SI_BUFFER
{
    LIST_ENTRY  ListEntry;
    ULONG       ulDataLength;
    BYTE        argbData[MIN_DIMENSION];

} PBDA_SI_BUFFER, *PPBDA_SI_BUFFER;

#define MAX_EIT_VERSION_ARRAY 4608

typedef struct _SERVICEID_EIT_VERSION
{
    LIST_ENTRY  ListEntry;
    WORD        wServiceId;
    BYTE        Version[MAX_EIT_VERSION_ARRAY];// 256 Sections, 2 P/F TableId's 16 ScheduleId's 

} SERVICEID_EIT_VERSION, *PSERVICEID_EIT_VERSION;


typedef struct _BDATUNER_EVENT
{
    LIST_ENTRY  ListEntry;
    GUID        uuidEvent;
    ULONG       ulEventId;
    ULONG       ulDataLength;
    BYTE        argbData[MIN_DIMENSION];

} BDATUNER_EVENT, *PBDATUNER_EVENT;


PBDATUNER_EVENT
AllocateTunerEvent(
    const GUID* Guid,
    ULONG Length,
    UCHAR* Data
    );

VOID
FreeTunerEvent(
    PBDATUNER_EVENT pTunerEvent
    );

class EventManager {
public:

    VOID
    UnInitialize();
    
    VOID
    ReleaseAllEvents();

    VOID
    ReleaseAllEventsWithoutLock();

    VOID
    ReleaseAllUnsignalledEvents();

    VOID
    Enable();

    VOID
    Disable();

    BOOL
    IsEnabled();


    PLIST_ENTRY
    NextEvent(PLIST_ENTRY);

    NTSTATUS
    ReadCurrentEvent(
        PBDATUNER_EVENT *ppEvent
    );
    
    NTSTATUS 
    GetCurrentEvent(
        PBDATUNER_EVENT *ppEvent
        );

    NTSTATUS  
    QueueEvent(
        PBDATUNER_EVENT Event,
        BOOL fNotifyIfPossible
        );

    NTSTATUS 
    Initialize(
        PVOID pContext
        );

    VOID
    MoveEventToSignalledList(
        PBDATUNER_EVENT pEvent
    );

    virtual 
    VOID
    CompleteEvent(
        PBDATUNER_EVENT Event, 
        ULONG dwEventId,
        HRESULT hrComplete    
        );

    virtual 
    VOID 
    ReleaseEvent(
        PBDATUNER_EVENT pEvent,
        ULONG CompletionStatus
        );
    

    virtual 
    NTSTATUS 
    NotifyEvent() = 0;

    virtual 
    BOOL 
    AreEventsEqual(
        PBDATUNER_EVENT pEvent, 
        PBDATUNER_EVENT pEventToComplete, 
        ULONG ulEventIdToComplete
        ) = 0;

protected:
    LIST_ENTRY  m_EventListPendingHead;
    LIST_ENTRY  m_EventListSignalledHead;
    BOOL        m_fEventSignalled;
    PVOID       m_pContext;
    BOOL        m_bEnabled;
};


class FilterEventManager: public EventManager
{
public:
    FilterEventManager();
    ~FilterEventManager();
    
    virtual NTSTATUS NotifyEvent();

    virtual BOOL AreEventsEqual(
        PBDATUNER_EVENT pEvent, 
        PBDATUNER_EVENT pEventToComplete, 
        ULONG ulEventIdToComplete
        );
};

class FilterDebugEventsManager: public EventManager
{
public:
    FilterDebugEventsManager();
    ~FilterDebugEventsManager();
    NTSTATUS
    QueueDebugMessage(
        UCHAR ucLevel,
        ULONG ulLength,
        UCHAR* pubData
        );
    
    virtual NTSTATUS NotifyEvent();

    virtual BOOL AreEventsEqual(
        PBDATUNER_EVENT pEvent, 
        PBDATUNER_EVENT pEventToComplete, 
        ULONG ulEventIdToComplete
        );
    VOID
        SetDebugLevel(UCHAR ucDebugLevel) {m_ucDebugLevel = ucDebugLevel;}
    VOID
        GetDebugLevel(UCHAR* pUcDebugLevel) {*pUcDebugLevel = m_ucDebugLevel;}
protected:
    UCHAR m_ucDebugLevel;        
};

class FilterGPNVEventsManager: public EventManager
{
public:
    FilterGPNVEventsManager();
    ~FilterGPNVEventsManager();
    NTSTATUS
    QueueNameValue(
        ULONG ulLength,
        UCHAR* pubData,
        BOOL  fSignalEvent 
        );

    virtual NTSTATUS NotifyEvent();

    virtual BOOL AreEventsEqual(
        PBDATUNER_EVENT pEvent, 
        PBDATUNER_EVENT pEventToComplete, 
        ULONG ulEventIdToComplete
        );

protected:
    ULONG m_ulNameValueSize;        
};


//
// Define the filter class.
//
class CFilter {
public:
    KSPIN_LOCK                  spinLock;

    //
    //  Define the AVStream Filter Dispatch Functions
    //  Network provider and AVStream use these functions 
    //  to create or remove a filter instance
    //
    static
    STDMETHODIMP_(NTSTATUS)
    Create(
        IN OUT PKSFILTER Filter,
        IN PIRP Irp
        );

    static
    STDMETHODIMP_(NTSTATUS)
    FilterClose(
        IN OUT PKSFILTER Filter,
        IN PIRP Irp
        );

/**************************************************************/
/* Only used to process frames. 
 *  Filters that transport data do not implement this dispatch function.
    static
    STDMETHODIMP
    Process(
        IN PKSFILTER Filter,
        IN PKSPROCESSPIN_INDEXENTRY ProcessPinsIndex
        );*/
/**************************************************************/

    //
    //  KSMETHODSETID_BdaChangeSync 
    //  Filter change sync methods
    //
    static
    STDMETHODIMP_(NTSTATUS)
    StartChanges(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OPTIONAL PVOID  pvIgnored
        );

    static
    STDMETHODIMP_(NTSTATUS)
    CheckChanges(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OPTIONAL PVOID  pvIgnored
        );

    static
    STDMETHODIMP_(NTSTATUS)
    CommitChanges(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OPTIONAL PVOID  pvIgnored
        );

    static
    STDMETHODIMP_(NTSTATUS)
    GetChangeState(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT PULONG      pulChangeState
        );

    static
    STDMETHODIMP_(NTSTATUS)
    GetMedium(
        IN PIRP             pIrp,
        IN PKSPROPERTY      pKSProperty,
        IN KSPIN_MEDIUM *   pulProperty
        );

    //
    //  KSMETHODSETID_BdaDeviceConfiguration 
    //  Methods to modify filter topology.
    //
    static
    STDMETHODIMP_(NTSTATUS)
    CreateTopology(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OPTIONAL PVOID  pvIgnored
        );

    static
    STDMETHODIMP_(NTSTATUS)
    GetEvent(
        IN PIRP         pIrp,
        IN PKSMETHOD  pKSMethod,
        IN PBDA_EVENT_DATA pData
        );

    static
    STDMETHODIMP_(NTSTATUS)
    GetDebugData(
        IN PIRP         pIrp,
        IN PKSMETHOD  pKSMethod,
        IN BDA_DEBUG_DATA *pData
        );

    static
    STDMETHODIMP_(NTSTATUS)
    CFilter::SetDebugLevel(
        IN PIRP         pIrp,
        IN PKSMETHOD  pKSMethod,
        IN PLONG   pPbdaResult
        );

    static
    STDMETHODIMP_(NTSTATUS)
    CompleteEvent(
        IN PIRP         pIrp,
        IN PKSMETHOD  pKSMethod,
        IN PLONG   pPbdaResult
        );
        
    static
    NTSTATUS 
    CFilter::ProcessFrequencyFromTuner(
    IN PIRP         pIrp,
    IN PKSPROPERTY  pKSProperty,
    IN ULONG       *pulFreg
    );

    static
    STDMETHODIMP_(NTSTATUS)
    EnablePendingEvent(
        IN PIRP Irp,
        IN PKSEVENTDATA EventData,
        IN struct _KSEVENT_ENTRY* EventEntry
    );

    static
    STDMETHODIMP_(VOID)
    DisablePendingEvent(
        IN PFILE_OBJECT FileObject,
        IN struct _KSEVENT_ENTRY* EventEntry
    );

// DMBTH ADD
// GDNVS COMMAND FUNCTION

    static
    bool 
    GetDMBTHCommandParams(
        WCHAR *pSourceStr,
        const WCHAR *pKeyStr,
        WCHAR *pParmStr
        );

    static
    NTSTATUS
    GetSignalStatus(
        IN PIRP     pIrp,
        __in_ecount(ulSourceLen) PWSTR pSourceStr,
        ULONG ulSourceLen,
        __in_ecount(*pulValueStrLen) PWSTR pValueStr, 
        ULONG *pulValueStrLen
        );

    static
    NTSTATUS
    ReplaceDMBTHXML(
        WCHAR *pSourceStr,
        ULONG ulSourceLen,
        PCSTR pKeyStr,
        WCHAR *pTargetStr,
        WCHAR *pValueStr, 
        ULONG *pulValueStrLen
        );

    static
    STDMETHODIMP_(NTSTATUS)
    GetNameValue(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT BDA_STRING  *pData
        );

    static
    STDMETHODIMP_(NTSTATUS)
    SetNameValue(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT PULONG      pulReturn
        );

    static
    STDMETHODIMP_(NTSTATUS)
    GetNameFromIndex(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT BDA_STRING  *pData
        );

    static
    STDMETHODIMP_(NTSTATUS)
    GetValueUpdateName(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT BDA_STRING  *pData
        );

    static
    STDMETHODIMP_(NTSTATUS)
	GuideDataType(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT BDA_GDDS_DATATYPE  *pDataType
        );

    static
    STDMETHODIMP_(NTSTATUS)
	GuideDataValue(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT BDA_GDDS_DATA  *pDataValue
        );

    static
    STDMETHODIMP_(NTSTATUS)
	GetTuneXMLFromIDX(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT BDA_STRING  *pTuneXML
        );

    static
    STDMETHODIMP_(NTSTATUS)
	GetServices(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT BDA_BUFFER  *pServicesData
        );

    static
    STDMETHODIMP_(NTSTATUS)
	GetServiceFromTuneXML(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT BDA_STRING  *pServiceData
        );

    static
    STDMETHODIMP_(NTSTATUS)
	GetGuideDataUpdate(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT PULONG      pulReturn
        );

    static
    STDMETHODIMP_(NTSTATUS)
    SetUserActivityUseReason(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT PLONG       pPbdaResult
        );

    static
    STDMETHODIMP_(NTSTATUS)
	GetUserActivityInterval(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT BDA_USERACTIVITY_INTERVAL  *pActivityInterval
        );

    static
    STDMETHODIMP_(NTSTATUS)
    UserActivityDetected(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT PLONG       pPbdaResult
        );

    //
    //  Filter Implementation Methods
    //
    STDMETHODIMP_(class CDevice *)
    GetDevice() { return m_pDevice;};

    STDMETHODIMP_(NTSTATUS)
    PutFrequency(
        IN ULONG        ulBdaParameter
        );

    STDMETHODIMP_(NTSTATUS)
    GetFrequency(
        IN PULONG        pulBdaParameter
        );

    STDMETHODIMP_(NTSTATUS)
    PutMultiplier(
        IN ULONG        ulBdaParameter
        );

    STDMETHODIMP_(NTSTATUS)
    GetMultiplier(
        IN PULONG        pulBdaParameter
        );

    STDMETHODIMP_(NTSTATUS)
    PutPolarity (
        IN ULONG        ulBdaParameter
        );

    STDMETHODIMP_(NTSTATUS)
    GetPolarity (
        IN PULONG       pulBdaParameter
        );

    STDMETHODIMP_(NTSTATUS)
    PutPLPNumber (
        IN ULONG        ulBdaParameter
        );

    STDMETHODIMP_(NTSTATUS)
    GetPLPNumber (
        IN PULONG       pulBdaParameter
        );

    STDMETHODIMP_(NTSTATUS)
    PutLNBSource (
        IN ULONG        ulBdaParameter
        );

    STDMETHODIMP_(NTSTATUS)
    GetLNBSource (
        IN PULONG       pulBdaParameter
        );

    STDMETHODIMP_(NTSTATUS)
    PutDiseqCEnable (
        IN BOOL        fBdaParameter
        );

    STDMETHODIMP_(NTSTATUS)
    GetDiseqCEnable (
        IN PBOOL       pfBdaParameter
        );

    STDMETHODIMP_(NTSTATUS)
    SetDemodulator(
        IN const GUID *       pguidDemodulator
        );

    STDMETHODIMP_(NTSTATUS)
    SetDemodProperty1(
        IN ULONG        ulDemodProperty1
        )
    {
            m_NewResource.ulDemodProperty1 = ulDemodProperty1;
            m_BdaChangeState = BDA_CHANGES_PENDING;

            return STATUS_SUCCESS;
    }

    STDMETHODIMP_(NTSTATUS)
    GetDemodProperty1(
        IN PULONG       pulDemodProperty1
        )
    {
        if (pulDemodProperty1)
        {
            *pulDemodProperty1 = m_CurResource.ulDemodProperty1;
            return STATUS_SUCCESS;
        }
        else
        {
            return STATUS_INVALID_PARAMETER;
        }
    }

    STDMETHODIMP_(NTSTATUS)
    SetDemodProperty2(
        IN ULONG        ulDemodProperty2
        )
    {
            m_NewResource.ulDemodProperty2 = ulDemodProperty2;
            m_BdaChangeState = BDA_CHANGES_PENDING;

            return STATUS_SUCCESS;
    }

    STDMETHODIMP_(NTSTATUS)
    GetDemodProperty3(
        IN PULONG       pulDemodProperty3
        )
    {
        if (pulDemodProperty3)
        {
            *pulDemodProperty3 = m_CurResource.ulDemodProperty3;
            return STATUS_SUCCESS;
        }
        else
        {
            return STATUS_INVALID_PARAMETER;
        }
    }

    STDMETHODIMP_(NTSTATUS)
    GetStatus(
        PBDATUNER_DEVICE_STATUS     pDeviceStatus
        );

    STDMETHODIMP_(NTSTATUS)
    SetDeviceState(
        KSSTATE     newKsState
        )
    {
        m_KsState = newKsState;
        return STATUS_SUCCESS;
    };

    VOID
    SignalPendingEvent();

    STDMETHODIMP_(NTSTATUS)
    AcquireResources();

    STDMETHODIMP_(NTSTATUS)
    ReleaseResources();
    PDEVICE_OBJECT              pDevObject;
    PFILE_OBJECT                pFiObject;
    LIST_ENTRY                  m_FilterListEntry;
    KSPIN_LOCK                  m_Lock;  
    FilterEventManager          m_FilterEventsManager;
    FilterDebugEventsManager    m_FilterDebugEventsManager;
    FilterGPNVEventsManager     m_FilterGPNVEventsManager;

    

    static
    STDMETHODIMP_(NTSTATUS)
    PBDA_DRM_SetDRM(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT PULONG      pulReturn
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PBDA_GetDRMStatus(
        IN  PIRP         pIrp,
        IN  PKSMETHOD    pKSMethod,
        OUT PBDA_DRM_DRMSTATUS pDRMStatus
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PBDA_GetWMDRMStatus(
        IN  PIRP         pIrp,
        IN  PKSMETHOD    pKSMethod,
        OUT PBDA_WMDRM_STATUS pWMDRMStatus
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PBDA_GetWMDRMKeyInfo(
        IN  PIRP         pIrp,
        IN  PKSMETHOD    pKSMethod,
        OUT PBDA_WMDRM_KEYINFOLIST pWMDRMKeyInfo
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PBDA_GetWMDRMTunerStartCodeProfile(
        IN  PIRP         pIrp,
        IN  PKSMETHOD    pKSMethod,
        OUT PBDA_BUFFER  pProfileBuffer
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PBDA_WMDRMRevInfo(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT PULONG      pulReturn
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PBDA_WMDRMCrl(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT PULONG      pulReturn
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PBDA_WMDRMTransactMessage(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT PBDA_BUFFER pMessageOutBuffer
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PBDA_WMDRMReissueLicense(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT PULONG      pulReturn
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PBDA_WMDRMRenewLicense(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT PBDA_WMDRM_RENEWLICENSE pXMROutBuffer
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PBDA_WMDRMGetLicense(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT PBDA_BUFFER  pLicenseBuffer
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PBDA_WMDRMTunerCancelCapToken(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT PULONG      pulReturn
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PBDA_WMDRMTunerSetPidProtection(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT PULONG      pulReturn
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PBDA_WMDRMTunerGetPidProtection(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT PBDA_WMDRMTUNER_PIDPROTECTION pUUIDBuffer
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PBDA_WMDRMTunerSetSyncValue(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT PULONG      pulReturn
        );
    
    static
    STDMETHODIMP_(NTSTATUS)
    PBDA_WMDRMTunerPurchaseEntitlement(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT PBDA_WMDRMTUNER_PURCHASEENTITLEMENT pPurchaseEntitlementOutBuffer
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PBDA_MUX_SetPIDList(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT PULONG      pulReturn
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PBDA_MUX_GetPIDList(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT PBDA_BUFFER pPIDListBuffer
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PBDA_ISDBCAS_SetRequest(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT PULONG      pulReturn
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PBDA_ISDBCAS_GetResponseData(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT BDA_ISDBCAS_RESPONSEDATA *pResponseBuffer
        );

    static
    STDMETHODIMP_(NTSTATUS)
	PBDA_CoreCAS_CheckEntitlementToken(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT BDA_CAS_CHECK_ENTITLEMENTTOKEN *pDescrambleStatus
        );

    static
    STDMETHODIMP_(NTSTATUS)
	PBDA_CoreCAS_SetCaptureToken(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT PULONG      pulReturn
        );

    static
    STDMETHODIMP_(NTSTATUS)
	PBDA_CoreCAS_OpenBroadcastMMI(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT PULONG      pulReturn
        );

    static
    STDMETHODIMP_(NTSTATUS)
	PBDA_CoreCAS_CloseMMIDialog(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT BDA_CAS_CLOSE_MMIDIALOG *pSessionResult
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PBDA_Tuning_SetTuner(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT PULONG      pulReturn
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PBDA_Tuning_GetTunerState(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT BDA_TUNER_TUNERSTATE      *pTunerState
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PBDA_Tuning_GetSignalNoiseRatio(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT BDA_TUNER_DIAGNOSTICS      *pTunerDiagnostic
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PBDA_Scanning_GetCapabilities(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT BDA_SCAN_CAPABILTIES      *pScanCapabilities
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PBDA_Scanning_GetScanningState(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT BDA_SCAN_STATE      *pScanState
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PBDA_Scanning_SetScanFilter(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT PULONG      pulReturn
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PBDA_Scanning_ScanStart(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT PULONG      pulReturn
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PBDA_Scanning_ScanResume(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT PULONG      pulReturn
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PBDA_Scanning_ScanStop(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT PULONG      pulReturn
        );

    static
    STDMETHODIMP_(NTSTATUS)
    BDA_TsSelectorSetTSID(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT PULONG      pulReturn
        );

    static
    STDMETHODIMP_(NTSTATUS)
    BDA_TsSelectorGetTSInformation(
        IN PIRP                     pIrp,
        IN PKSMETHOD                pKSMethod,
        OUT PBDA_TS_SELECTORINFO    pulReturn
        );

    PKSEVENT_ENTRY              m_PendingEventEntry;

    NTSTATUS FindAndConvertStringValue(__in_ecount(ulSourceLen)char *pSourceStr, ULONG &ulSourceLen,__in_ecount(ulSourceLen+1) PCSTR pTargetStr, LONG *plValue, ULONG64 *pulValue64=NULL);
    NTSTATUS FindXMLString(__in_ecount(ulSourceLen)char *pSourceStr, ULONG &ulSourceLen,__in_ecount(ulSourceLen+1) PCSTR pTargetStr,__in_ecount(*pulValueLen) char **pValue, ULONG *pulValueLen);
    STDMETHODIMP_(NTSTATUS)SendBroadcastMMI(UINT uiType);

    STDMETHODIMP_(NTSTATUS) SendOpenMMI(ULONG ulDialogRequest, GUID uuidDialogType, PCSTR pPageData);

    GUID                        m_guideDataType;


 private:
    class CDevice * m_pDevice;

    //  Filter Properties
    //
#define GPNVS_CASE_COMMAND	L"PBDA://2734DC82-774D-4daa-A5AD-2838B5A58127/Gpnvs Command" //DMBTH tunerid/Resource filter
#define GPNVS_COMMAND_LEN  256 //

    //  Filter Resources
    //
#define GPNVS_NUMBER_OF_ROWS 31
#define GPNVS_NUMBER_OF_ROWS_VISIBLE 25
#define GPNVS_NUMBER_OF_COLUMNS 2

    WCHAR*                      m_GpnvsTable[GPNVS_NUMBER_OF_ROWS][GPNVS_NUMBER_OF_COLUMNS];
    void SetGPNVValue(__in __drv_in(__drv_aliasesMem)WCHAR* pValue, int RowIndex);
    KSSTATE                     m_KsState;
    BDA_CHANGE_STATE            m_BdaChangeState;
    BDATUNER_DEVICE_RESOURCE    m_CurResource;
    BDATUNER_DEVICE_RESOURCE    m_NewResource;
    ULONG                       m_ulResourceID;
    BOOLEAN                     m_fResourceAcquired;
    HANDLE                      hFileHandle;
    BOOLEAN                     m_bPendingEventsEnabled;
    BOOL                        m_bSignallingPendingEvent;
    KEVENT                      m_evPendingEventsComplete;
    UCHAR                       m_ucDebugLevel;
    LONG                        m_lEventId;
    ULONG                       m_AvailableGuideDataPercentage;
    ULONG                       m_CurrentScanningFrequency;
    ULONG                       m_ScanEstimateTime;
    BYTE*                       m_pGuideServices;
    ULONG                       m_ulCurrentDialogRequest;
    ULONG                       m_ulCurrentDialogNumber;
    BOOLEAN                     m_fBroadcastMMIRequested;
    UINT                        m_uiBroadcastMMIType;
    ULONG                       m_ulDeviceInstance;
    CFilter();
    ~CFilter();
};

/*************************************************

    Class Definitions

*************************************************/
class    CHardwareSimulation;
class    CTsSynthesizer;
//
// IHardwareSink:
//
// This interface is used by the hardware simulation to fake interrupt
// service routines.  The Interrupt method is called at DPC as a fake
// interrupt.
//
class IHardwareSink {

public:

    virtual
    void
    Interrupt (
        ) = 0;

};

//
// ICaptureSink:
//
// This is a capture sink interface.  The device level calls back the
// CompleteMappings method passing the number of completed mappings for
// the capture pin.  This method is called during the device DPC.
//
class ICaptureSink {

public:

    virtual
    void
    CompleteMappings (
        IN ULONG NumMappings
        ) = 0;

};

//
// Define the device class.
//
class CDevice : public IHardwareSink  
{
public:

    //
    //  Define the AVStream Device Dispatch Functions
    //  AVStream uses these functions to create and start the device
    //
    static
    STDMETHODIMP_(NTSTATUS)
    Create(
        IN PKSDEVICE    pKSDevice
        );

    //
    // DispatchPnpStart():
    //
    // This is the Pnp Start dispatch for the capture device.  It simply
    // bridges to PnpStart() in the context of the CCaptureDevice.
    //
    static
    NTSTATUS
    DispatchPnPStart (
        IN PKSDEVICE pKSDevice,
        IN PIRP Irp,
        IN PCM_RESOURCE_LIST TranslatedResourceList,
        IN PCM_RESOURCE_LIST UntranslatedResourceList
        )
    {
    	 UNREFERENCED_PARAMETER(Irp);
        return 
            (reinterpret_cast <CDevice *> (pKSDevice -> Context)) ->
            PnPStart (
                TranslatedResourceList,
                UntranslatedResourceList
                );
    }

    //
    // DispatchPnPStop():
    //
    // This is the Pnp stop dispatch for the capture device.  It simply
    // bridges to PnpStop() in the context of the CCaptureDevice.
    //
    static
    void
    DispatchPnPStop (
        IN PKSDEVICE pKSDevice,
        IN PIRP Irp
        )
    {
        
    	     UNREFERENCED_PARAMETER(Irp);
            (reinterpret_cast <CDevice *> (pKSDevice -> Context)) ->
            PnPStop (
                );
    }

    //
    // PnpStart():
    //
    // This is the Pnp start routine for our simulated hardware.  Note that
    // DispatchStart bridges to here in the context of the CCaptureDevice.
    //
    NTSTATUS
    PnPStart (
        IN PCM_RESOURCE_LIST TranslatedResourceList,
        IN PCM_RESOURCE_LIST UntranslatedResourceList
        );

	WCHAR* 
	GetDeviceInstanceID();

    //
    // PnpStop():
    //
    // This is the Pnp stop routine for our simulated hardware.  Note that
    // DispatchStop bridges to here in the context of the CCaptureDevice.
    //
    void
    PnPStop (
        );

    static
    __drv_dispatchType(IRP_MJ_DEVICE_CONTROL)
    DRIVER_DISPATCH DeviceControlDispatch;
    //
    //  Utility functions for the device
    //  An instance of the filter uses these functions 
    //  to manage resources on the device. 
    //

    STDMETHODIMP_(NTSTATUS)
    InitializeHW(
        );


    STDMETHODIMP_(NTSTATUS)
    GetStatus(
        PBDATUNER_DEVICE_STATUS     pDeviceStatus
        );

    STDMETHODIMP_(NTSTATUS)
    GetImplementationGUID(
        GUID *                      pguidImplementation
        )
    {
        if (pguidImplementation)
        {
            *pguidImplementation = m_guidImplemenation;
            return STATUS_SUCCESS;
        }
        else
        {
            return STATUS_INVALID_PARAMETER;
        }
    }

    STDMETHODIMP_(NTSTATUS)
    GetDeviceInstance(
        ULONG *                     pulDeviceInstance
        )
    {
        if (pulDeviceInstance)
        {
            *pulDeviceInstance = m_ulDeviceInstance;
            return STATUS_SUCCESS;
        }
        else
        {
            return STATUS_INVALID_PARAMETER;
        }
    }

    NTSTATUS
    AcquireResources(
        PBDATUNER_DEVICE_RESOURCE     pNewResource,
        PULONG                        pulAcquiredResourceID
        );

    NTSTATUS
    UpdateResources(
        PBDATUNER_DEVICE_RESOURCE     pNewResource,
        ULONG                         ulResourceID
        );

    NTSTATUS
    ReleaseResources(
        ULONG                   ulResourceID
        );

    //
    // AcquireHardwareResources():
    //
    // Called to acquire hardware resources for the device based on a given
    // video info header.  This will fail if another object has already
    // acquired hardware resources since we emulate a single capture
    // device.
    //
    NTSTATUS
    AcquireHardwareResources (
        IN ICaptureSink *CaptureSink,
        IN PBDA_TRANSPORT_INFO TransportInfo
        );

    //
    // ReleaseHardwareResources():
    //
    // Called to release hardware resources for the device.
    //
    void
    ReleaseHardwareResources (
        );

    //
    // Start():
    //
    // Called to start the hardware simulation.  This causes us to simulate
    // interrupts, simulate filling buffers with synthesized data, etc...
    //
    NTSTATUS
    Start (
        );

    //
    // Pause():
    //
    // Called to pause or unpause the hardware simulation.  This will be
    // indentical to a start or stop but it will not reset formats and 
    // counters.
    //
    NTSTATUS
    Pause (
        IN BOOLEAN Pausing
        );

    //
    // Stop():
    //
    // Called to stop the hardware simulation.  This causes interrupts to
    // stop issuing.  When this call returns, the "fake" hardware has
    // stopped accessing all s/g buffers, etc...
    //
    NTSTATUS
    Stop (
        );

    //
    // ProgramScatterGatherMappings():
    //
    // Called to program the hardware simulation's scatter / gather table.
    // This synchronizes with the "fake" ISR and hardware simulation via
    // a spinlock.
    //
    ULONG
    ProgramScatterGatherMappings (
        IN PUCHAR *Buffer,
        IN PKSMAPPING Mappings,
        IN ULONG MappingsCount
        );

    //
    // QueryInterruptTime():
    //
    // Determine the frame number that this frame corresponds to.  
    //
    ULONG
    QueryInterruptTime (
        );

    //
    // IHardwareSink::Interrupt():
    //
    // The interrupt service routine as called through the hardware sink
    // interface.  The "fake" hardware uses this method to inform the device
    // of a "fake" ISR.  The routine is called at dispatch level and must
    // be in locked code.
    //
    virtual
    void
    Interrupt (
        );

    NTSTATUS
    SetFrequency (
     IN   PULONG ulFrequency
    );

    ULONG 
    Get_theFrequency();

    ULONG 
    GetFrequency();

    NTSTATUS
    GetNextAvailableFrequency(
        IN OUT PULONG ulFrequency
        );

    NTSTATUS 
        GetNextGuideSection(BYTE *pNextGuideSection,ULONG *pulSectionLength, ULONG *pulPercentage);

    NTSTATUS 
        OpenEPGStreamCached(HANDLE *pFileHandlePBDASI);

    NTSTATUS 
        GetSwtunerInstallPath(OUT PUNICODE_STRING FilePath); 

    NTSTATUS 
        ReadGuideServiceListFromFile(BYTE **pBuffer, ULONG *pBufferLength);

    NTSTATUS 
        ReadGuideDataFromFile(BYTE *pBuffer, ULONG *pBufferLength);

    NTSTATUS
        GetDeviceParameterString(IN PWSTR pwsParam, OUT PUNICODE_STRING pSzValue);

    NTSTATUS 
        ReadMCMLFromFile(BYTE *pBuffer, ULONG *pBufferLength, UNICODE_STRING MCMLFileName);

//DMBTH ADD
#ifdef DMBTH_RECEIVER
    NTSTATUS 
        ReadOEMMCMLFile(BYTE *pBuffer, ULONG *pBufferLength, UNICODE_STRING MCMLFileName);
#endif

    void FileReaderFreqChanged();

    void
    InsertFilter(
        CFilter* Filter
    );

    void
    RemoveFilter(
        CFilter* Filter
    );

    LIST_ENTRY                m_FilterList;
    LIST_ENTRY                m_EventList;
    
private:
    
    
    PKSDEVICE                 m_pKSDevice;

    GUID                      m_guidImplemenation;
    ULONG                     m_ulDeviceInstance;
    BDATUNER_DEVICE_RESOURCE  m_CurResource;
    ULONG                     m_ulCurResourceID; 
    ULONG                     m_ulcResourceUsers;
    KSPIN_LOCK                m_Lock;  

    WCHAR                     m_czDeviceInstanceID[MAX_PATH + 1];
    bool                      m_bGetDeviceInstanceID;

    //Guide Loading variables
    HANDLE m_hGuideHandleCache;
    BYTE m_pLastGuideBuffer[8192];
    ULONG m_LastGuideBufferLength;
    ULONG m_LastGuidePercentage;

    //
    // Number of pins with resources acquired.  This is used as a locking
    // mechanism for resource acquisition on the device.
    //
    LONG m_PinsWithResources;

    //
    // Since we don't have physical hardware, this provides the hardware
    // simulation.  m_HardwareSimulation provides the fake ISR, fake DPC,
    // etc...  m_TsSynth provides a place to put transport stream synthesis
    // in software.
    //
    CHardwareSimulation *m_HardwareSimulation;
    CTsSynthesizer *m_TsSynth;

    //
    // Frequency on the tuner
    //
    ULONG m_ulFrequency;
    BOOLEAN m_bFrequencyChanged;
    DWORD m_dwSignalQuality;
    DWORD m_dwSignalStrength;
    BOOLEAN m_fSignalLocked;
    BOOLEAN m_fCarrierPresent;

    //
    // The number of ISR's that have occurred since capture started.
    //
    ULONG m_InterruptTime;

    //
    // The last reading of mappings completed.
    //
    ULONG m_LastMappingsCompleted;

    //
    // The Dma adapter object we acquired through IoGetDmaAdapter() during
    // Pnp start.  This must be initialized with AVStream in order to perform
    // Dma directly into the capture buffers.
    //
    PADAPTER_OBJECT m_DmaAdapterObject;

    //
    // The number of map registers returned from IoGetDmaAdapter().
    //
    ULONG m_NumberOfMapRegisters;

    //
    // The capture sink.  When we complete scatter / gather mappings, we
    // notify the capture sink.
    //
    ICaptureSink *m_CaptureSink;

    //
    // The video info header we're basing hardware settings on.  The pin
    // provides this to us when acquiring resources and must guarantee its
    // stability until resources are released.
    //
    PBDA_TRANSPORT_INFO m_TransportInfo;

    //
    // The latest TSID which is found in TS stream.
    //
    LONG            m_lLatestTSID;

    //
    // Cleanup():
    //
    // This is the free callback for the bagged capture device.  Not providing
    // one will call ExFreePool, which is not what we want for a constructed
    // C++ object.  This simply deletes the capture device.
    //
    static
    void
    Cleanup (
        IN CDevice *Device
        )
    {
        delete Device;
    }

    NTSTATUS InitReadDeviceInstance(); //initialize and read the DeviceInstance

};


//
// Define the Input-pin class.
//
class CAntennaPin {
public:
    //
    //  Define the AVStream Pin Dispatch Functions
    //  Network provider and AVStream use these functions 
    //  to create or remove a pin instance or to change the pin's 
    //  state after the minidriver receives a connection state 
    //  property 'set' IOCTL. 
    //
    static
    STDMETHODIMP_(NTSTATUS)
    PinCreate(
        IN OUT PKSPIN Pin,
        IN PIRP Irp
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PinClose(
        IN OUT PKSPIN Pin,
        IN PIRP Irp
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PinSetDeviceState(
        IN PKSPIN Pin,
        IN KSSTATE ToState,
        IN KSSTATE FromState
        );

    //
    //  Define a data intersection handler function for the 
    //  pin (KSPIN_DESCRIPTOR_EX structure). 
    //  Network provider and AVStream use this function 
    //  to connect the input pin with an upstream filter.   
    //
    static
    STDMETHODIMP_(NTSTATUS)
    IntersectDataFormat(
        IN PVOID pContext,
        IN PIRP pIrp,
        IN PKSP_PIN Pin,
        IN PKSDATARANGE DataRange,
        IN PKSDATARANGE MatchingDataRange,
        IN ULONG DataBufferSize,
        OUT PVOID Data OPTIONAL,
        OUT PULONG DataSize
        );

    //
    //  Network provider and AVStream use these functions 
    //  to set and get properties of nodes that are controlled 
    //  by the input pin. 
    //
    static
    STDMETHODIMP_(NTSTATUS)
    GetCenterFrequency(
        IN PIRP         Irp,
        IN PKSPROPERTY  pKSProperty,
        IN PULONG       pulProperty
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PutCenterFrequency(
        IN PIRP         Irp,
        IN PKSPROPERTY  pKSProperty,
        IN PULONG       pulProperty
        );

    static
    STDMETHODIMP_(NTSTATUS)
    GetMultiplier(
        IN PIRP         Irp,
        IN PKSPROPERTY  pKSProperty,
        IN PULONG       pulProperty
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PutMultiplier(
        IN PIRP         Irp,
        IN PKSPROPERTY  pKSProperty,
        IN PULONG       pulProperty
        );

    static
    STDMETHODIMP_(NTSTATUS)
    GetSignalStatus(
        IN PIRP         Irp,
        IN PKSPROPERTY  pKSProperty,
        IN PULONG       pulProperty
        );

    //#if DVBS_RECEIVER
    static
    STDMETHODIMP_(NTSTATUS)
    GetDiseqCProperty(
        IN PIRP         Irp,
        IN PKSPROPERTY  pKSProperty,
        IN PULONG       pulProperty
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PutDiseqCProperty(
        IN PIRP         Irp,
        IN PKSPROPERTY  pKSProperty,
        IN PULONG       pulProperty
        );
    //#endif // DVBS_RECEIVER

    static
    STDMETHODIMP_(NTSTATUS)
    PutTunerParamProperty(
        IN PIRP         Irp,
        IN PKSPROPERTY  pKSProperty,
        IN PULONG       pulProperty
        );

    static
    STDMETHODIMP_(NTSTATUS)
    GetTunerParamProperty(
        IN PIRP         Irp,
        IN PKSPROPERTY  pKSProperty,
        IN PULONG       pulProperty
        );

    //
    //  Utility functions for the pin
    //
    STDMETHODIMP_(class CFilter *)
    GetFilter() { return m_pFilter;};

    STDMETHODIMP_(void)
    SetFilter(class CFilter * pFilter) { m_pFilter = pFilter;};

private:
    class CFilter*  m_pFilter;
    ULONG           ulReserved;
    KSSTATE         m_KsState;

    //  Node Properties
    //
    BOOLEAN         m_fResourceChanged;
    ULONG           m_ulCurrentFrequency;
    ULONG           m_ulPendingFrequency;
};
//
//  Topology Constants
//
typedef enum {
    PIN_TYPE_ANTENNA = 0,
    PIN_TYPE_TRANSPORT
} FilterPinTypes;

typedef enum {
    INITIAL_ANNTENNA_PIN_ID = 0
} InitialPinIDs;

//
//  Data declarations
//

extern const BDA_FILTER_TEMPLATE    BdaFilterTemplate;
extern const KSFILTER_DESCRIPTOR    InitialFilterDescriptor;
extern const KSFILTER_DESCRIPTOR    TemplateFilterDescriptor;
extern const BDA_FILTER_TEMPLATE    CaptureFilterTemplate;
extern const KSFILTER_DESCRIPTOR    CaptureFilterDescriptor;


/*
VOID
ParGetDriverParameterDword(
    IN     PUNICODE_STRING ServicePath,
    IN     PWSTR           ParameterName,
    IN OUT PULONG          ParameterValue
    );*/

NTSTATUS
PptRegGetDword(
    IN     ULONG   RelativeTo,
    IN     __nullterminated PWSTR   Path,
    IN     __nullterminated PWSTR   ParameterName,
    IN OUT PULONG  ParameterValue
    );
/*
NTSTATUS
PptRegSetDword(
    IN  ULONG  RelativeTo,               
    IN  PWSTR  Path,
    IN  PWSTR  ParameterName,
    IN  PULONG ParameterValue
    );
*/

NTSTATUS
PptRegGetSz(
    IN      ULONG  RelativeTo,               
    IN      __nullterminated PWSTR  Path,
    IN      __nullterminated PWSTR  ParameterName,
    IN OUT  PUNICODE_STRING ParameterValue
    );

/*
NTSTATUS
PptRegSetSz(
    IN  ULONG  RelativeTo,               
    IN  PWSTR  Path,
    IN  PWSTR  ParameterName,
    IN  PWSTR  ParameterValue
    );
*/

NTSTATUS
PptRegGetDeviceParameterDword(
    IN     PDEVICE_OBJECT  Pdo,
    IN     __nullterminated PWSTR           ParameterName,
    IN OUT PULONG          ParameterValue
    );

NTSTATUS
PptRegSetDeviceParameterDword(
    IN PDEVICE_OBJECT  Pdo,
    IN __nullterminated PWSTR           ParameterName,
    IN PULONG          ParameterValue
    );

//
// CAPTURE_OUT_PIN_DATA_RANGE_COUNT:
//
// The number of ranges supported on the capture output pin.
//
#define CAPTURE_OUT_PIN_DATA_RANGE_COUNT 1

//
// CAPTURE_IN_PIN_DATA_RANGE_COUNT:
//
// The number of ranges supported on the capture input pin.
//
#define CAPTURE_IN_PIN_DATA_RANGE_COUNT 1

//
// CAPTURE_FILTER_PIN_COUNT:
//
// The number of pins on the capture filter.
//
#define CAPTURE_FILTER_PIN_COUNT 2

//
// CAPTURE_FILTER_CATEGORIES_COUNT:
//
// The number of categories for the capture filter.
//
#define CAPTURE_FILTER_CATEGORIES_COUNT 1


/*************************************************

    Externed information

*************************************************/

//
// filter.cpp externs:
//
extern
const
KSFILTER_DISPATCH
CaptureFilterDispatch;

extern
const
KSFILTER_DESCRIPTOR
CaptureFilterDescriptor;

extern
const
KSPIN_DESCRIPTOR_EX
CaptureFilterPinDescriptors [CAPTURE_FILTER_PIN_COUNT];

extern
const
GUID
CaptureFilterCategories [CAPTURE_FILTER_CATEGORIES_COUNT];

//
// capture.cpp externs:
//
extern 
const
KSALLOCATOR_FRAMING_EX
CapturePinAllocatorFraming;

extern 
const
KSPIN_DISPATCH
CapturePinDispatch;

extern 
const
KSPIN_DISPATCH
InputPinDispatch;

extern
const
PKSDATARANGE
CaptureOutPinDataRanges [CAPTURE_OUT_PIN_DATA_RANGE_COUNT];

extern
const
PKSDATARANGE
CaptureInPinDataRanges [CAPTURE_IN_PIN_DATA_RANGE_COUNT];

class trhdr
{
public:
	BYTE sync; // 0x47
	BYTE error:1;
	BYTE payloadstart:1;
	BYTE transportpriority:1;
	WORD pid:13;
	BYTE scrambling:2;
	BYTE adapfield:1;
	BYTE payload:1;
	BYTE counter:4;
	// if adapfield set
	BYTE length;
	BYTE discontinuity:1;
	BYTE randomaccess:1;
	BYTE priority:1;
	BYTE PCR:1;
	BYTE OPCR:1;
	BYTE splicingpoint:1;
	BYTE privatedata:1;
	BYTE extension:1;

	__int64 pcrValue;

	int bytes;
	__int64 next;
	int index;//index of the file(*188)
};

class peshdr
{
public:
	WORD len;

	BYTE type:2, fpts:1, fdts:1;
	__int64 pts, dts;

	// mpeg2 stuff
	BYTE scrambling:2, priority:1, alignment:1, copyright:1, original:1;
	BYTE escr:1, esrate:1, dsmtrickmode:1, morecopyright:1, crc:1, extension:1;
	BYTE hdrlen;

	peshdr() {RtlZeroMemory(this, sizeof(*this));}
};

struct trsechdr
{
	BYTE table_id;
	WORD section_syntax_indicator:1;
	WORD zero:1;
	WORD reserved1:2;
	WORD section_length:12;
	WORD transport_stream_id;
	BYTE reserved2:2;
	BYTE version_number:5;
	BYTE current_next_indicator:1;
	BYTE section_number;
	BYTE last_section_number;

	//additional control
	WORD section_above_length;
};

class IStorge
{
public:
	virtual int Read(BYTE *pDest,int iLen) = 0;
	virtual UINT64 GetLength()const = 0;
	virtual int Seek(UINT64 pos) = 0;
	virtual UINT64 GetPos()=0;
	virtual ~IStorge(){}
};

class CMemStorage : public IStorge
{
public:
	CMemStorage(BYTE *buffer,UINT64 llLen);

	int Read(__out_bcount(iLen) BYTE *pDest,int iLen);
	UINT64 GetLength()const;
	int Seek(UINT64 pos);
	virtual ~CMemStorage();
	virtual UINT64 GetPos();
protected:
	BYTE *m_pData;
	UINT64 m_llPos;
	UINT64 m_llTotalLenght;
};

class BitReadHelper
{
public:
	BitReadHelper(IStorge *pStorage);
	virtual ~BitReadHelper();
public:
	void BitByteAlign();
	UINT64 GetPos();
	int BitFlush();
	UINT64 BitRead(int nBits, bool fPeek=false);
	int ByteRead(BYTE *pData,UINT64 iLen);

	int Seek(UINT64 pos);

	UINT64 GetLength()const
	{
		if(!m_pStorage)
			return 0;
		return m_pStorage->GetLength();
	}
protected:
	IStorge *m_pStorage;
	int m_bitlen;
	UINT64 m_bitbuff;
};

const int MAX_Program_Number = 128;
const int MAX_Pid_Number = 32;
#define MAX_SECTION_LENGTH 1391  // 1024 + 184 possible stuffing bytes + 183 possible bytes from previous section 
#define MAX_EITSECTION_LENGTH 4280 //4096 + 184 possible stuffing bytes

class CProgram
{
public:
      CProgram()
 		:m_llCurrentPtsBase(0)
		,m_llCurrentDtsBase(0)
		,m_llCurrentPts(0)
		,m_llCurrentDts(0)
		,m_llBeginPts(-1)
		,m_llBeginDts(-1)     	
		,m_wPmtId(0)
		,m_wProgramNumber(0)
		,m_wCurCacheLen(0)
		,m_llCurrentPcrValue(0)
		,m_llCurrentPcrDuration(0)
		,m_llBeginPcr(-1)
		
        {
            memset(m_bDiscontinuity_Indicated, true, sizeof(BOOL) * MAX_Pid_Number);
            RtlZeroMemory(&m_wEsPid,sizeof(m_wEsPid));
        }
    friend class CTsTimestamp;
    void UpdateCurrentTimestampBase();
    void Reset();
    void ModifyTimestampBase();
    BOOL IsDiscontinuity(BYTE bCounterVal,int iPidNumber);
    BOOL m_bDiscontinuity_Indicated[MAX_Pid_Number];
    DWORD m_dwTsStopPacket[MAX_Pid_Number];//last received Number of alignment Headers
    DWORD m_dwTsCurrentPacket[MAX_Pid_Number];//counter from start of the TS file
    BOOL m_fStartUp[MAX_Pid_Number];

protected:
       //90 khz
	__int64 m_llCurrentPtsBase;
	__int64 m_llCurrentDtsBase;

	__int64 m_llCurrentPts;
	__int64 m_llCurrentDts;

	__int64 m_llBeginPts;
	__int64 m_llBeginDts;
	
	__int64 m_llCurrentPcrValue;
	__int64 m_llCurrentPcrDuration;
	__int64 m_llBeginPcr;

       WORD m_wEsPid[MAX_Pid_Number];
       WORD m_wPmtId;
       WORD m_wProgramNumber;
       BYTE m_bCache[MAX_SECTION_LENGTH];
       WORD m_wCurCacheLen;
       BYTE m_bLastCounter[MAX_Pid_Number];
};

class CTsTimestamp
{
    public:
    void gen_crc_table();
    void CleanUpPBDASIList();

    CTsTimestamp()
        :m_iPatCurLen(0)
        ,m_programs(new(NonPagedPool,MS_SAMPLE_TUNER_POOL_TAG) CProgram[MAX_Program_Number])
        ,m_fRepeating(FALSE)
        ,m_wMJDDelta(0)
        ,m_iUTCDeltaH(0)
        ,m_iUTCDeltaM(0)
        ,m_iEITCurLen(0)
        ,m_iDistanceToNextEIT_starttime(16)
        ,m_dwEITRestamp(0)
        ,m_lPCRDriftPPM(0)
        ,m_CurrentPBDASISections(0)
        ,m_TotalPBDASISectionsSchedule(0)
        ,m_TotalPBDASISectionsPF(0)
        ,m_wNrOfServices(0)
        {
            gen_crc_table();
            InitializeListHead(&m_PBDASIList);
            InitializeListHead(&m_ServiceIdList);
	}
	
	~CTsTimestamp()
	{
            delete[] m_programs;
            CleanUpPBDASIList();
	}

    void InitParams(DWORD dwEITRestamp,DWORD dwPCRDriftPPM,DWORD dwTSPacketBlank);

    bool TsPacketEnter(BYTE *bBuffer,UINT64 len = 188);

    void UpdateCurrentTimestampBase();

    static bool Read(trhdr &hTsHeader,BitReadHelper &bitReader);
    static bool Read(trsechdr& h,BitReadHelper &BitHelper);
    static bool ReadShort(trsechdr& h,BitReadHelper &BitHelper);

    bool ReadAndUpdateTimestamp(peshdr &h,BitReadHelper &bitReader,BYTE code,BYTE *buffer,int dwTsHeaderPast,CProgram &it);

    bool IsPmtTable(WORD pid,int &iIndex);

    bool IsEsPid(WORD pid,int &iProgramIndex, int &iPidIndex);

    bool TsStreamProcess(BYTE *bBuffer,UINT64 len);

    void Reset();

    void DbgCurrentTimestamp();

    void ModifyTimestampBase();

    unsigned long
        calc_crc(
        unsigned char *data_blk_ptr,
        unsigned long data_blk_size,
        unsigned long crc_last = 0xFFFFFFFF
        );

    void MJDUTCPlus(WORD &mjd, BYTE &utc_h_bcd, BYTE &utc_m_bcd);

    bool ProcessTDTPacket(BYTE *bBuffer,trhdr &hTsHeader);

    bool ProcessEITPacket(BYTE *bBuffer,trhdr &hTsHeader);

    void ConvertGuideToPBDASI(BYTE *pEITBuffer, WORD wEITBufferLength);

    bool GetPBDASIGuideData(PPBDA_SI_BUFFER *pPBDABuffer);

    void GetNrOfTables(ULONG *pulNrOfCached, ULONG *pulTotalSections);

    void ReleasePBDASIData(PBDA_SI_BUFFER *pPBDASIData);

protected:
	CProgram* m_programs;//[MAX_Program_Number];//most 16 program per ts stream
    BYTE      m_bPatCache[MAX_SECTION_LENGTH];
    int       m_iPatCurLen;
    BOOL      m_fRepeating;
    WORD      m_wMJDDelta;
    int       m_iUTCDeltaH;
    int       m_iUTCDeltaM;
    BYTE      m_bEITCache[MAX_EITSECTION_LENGTH];
    int       m_iEITCurLen;
    int       m_iDistanceToNextEIT_starttime;
    BYTE*     m_pLastEITTime;
    BYTE      m_bLastEITVersion;
    BYTE*     m_pCRC32Start;
    DWORD     m_dwEITRestamp;
    DWORD     m_lPCRDriftPPM;
    DWORD     m_dwTSPacketBlank;
    WORD      m_wNrOfServices;

    LIST_ENTRY  m_PBDASIList;
    ULONG       m_TotalPBDASISectionsPF;
    ULONG       m_TotalPBDASISectionsSchedule;
    ULONG       m_CurrentPBDASISections;
    LARGE_INTEGER  m_liTime1970;
    KSPIN_LOCK  m_TSProcessingLock;
    LIST_ENTRY  m_ServiceIdList;
    BYTE        m_bVersionExtend;

    PBDA_SI_BUFFER *AllocatePBDASIData( ULONG ulEITLength );

    bool ConvertEITStartTimeToUTC(BYTE *pEITStartTime, BYTE *pUTCStartTime, LARGE_INTEGER *pliStartTime);
    bool ConvertEITDurationToSeconds(BYTE *pEITDuration, BYTE *pSecDuration, LARGE_INTEGER liStartTime, BYTE bRunningStatus);
    bool ConvertEITEventIDToPBDAEventId(WORD wEITEventId, BYTE *pPBDAEventId);
    bool CheckEITVersion(WORD wServiceID, BYTE bTableId, BYTE bSectionNr, BYTE bVersion, bool &bVersionCheckResultNow);
    void CleanUpSIDList();


};




/*************************************************

    Internal Includes

*************************************************/
#include "commonx.h"
#include <bdasup.h>
#include "TStream.h"
#include "hwsim.h"
#include "CCapturePin.h"

// Create a connection through the capture filter so that graph render will
// work.
//
const
KSTOPOLOGY_CONNECTION CaptureFilterConnections[] =
{   // KSFILTER_NODE  is defined as ((ULONG)-1) in ks.h 
    { KSFILTER_NODE, 0,                 KSFILTER_NODE, 1 }
};


#endif

