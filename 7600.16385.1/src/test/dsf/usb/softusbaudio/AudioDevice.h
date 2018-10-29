/*++ BUILD Version 0000

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    AudioDevice.h

Abstract:
    Definition of the class CAudioDevice

--*/

#pragma once
#include "resource.h"       // main symbols

#include "SoftUSBAudio.h"
#include "AudioDeviceEvents.h"


//Release and Add ref macros
#define ADDREF(punk)\
{\
    if ((punk) != NULL)\
    {\
        (punk)->AddRef();\
    }\
}


#define RELEASE(punk)\
{\
    if ((punk) != NULL)\
    {\
        IUnknown *_punkXxx = (punk);\
        (punk) = NULL;\
        _punkXxx->Release();\
    }\
}

//HR check
#define IfFailHrGo(EXPR) { hr = (EXPR); if(FAILED(hr)) goto Exit; }
#define IfFalseHrGo(EXPR, HR) { if(!(EXPR)) { hr = (HR); goto Exit; } }


#pragma warning(disable: 4995) //Pragma deprecated

// CAudioDevice

class ATL_NO_VTABLE CAudioDevice : 
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CAudioDevice, &CLSID_AudioDevice>,
    public IConnectionPointContainerImpl<CAudioDevice>,
    public CProxy_IAudioDeviceEvents<CAudioDevice>, 
    public ISoftUSBEndpointEvents,
    public IDispatchImpl<IAudioDevice, &IID_IAudioDevice, &LIBID_SoftUSBAudioLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
    CAudioDevice();
    virtual ~CAudioDevice();

DECLARE_REGISTRY_RESOURCEID(IDR_AUDIODEVICE)


BEGIN_COM_MAP(CAudioDevice)
    COM_INTERFACE_ENTRY(IAudioDevice)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(ISoftUSBEndpointEvents)
    COM_INTERFACE_ENTRY(IConnectionPointContainer)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CAudioDevice)
    CONNECTION_POINT_ENTRY(__uuidof(IAudioDeviceEvents))
END_CONNECTION_POINT_MAP()


    DECLARE_PROTECT_FINAL_CONSTRUCT()

    HRESULT FinalConstruct();
    
    void FinalRelease() 
    {
        CoUninitialize();
    }

public:

private:
     void InitMemberVariables();

     HRESULT CreateUSBDevice();
     HRESULT CreateAudioControlInterface(ISoftUSBInterface *&rpiAudioCtrlIntf);
     HRESULT CreateAudioStreamingInterfaces(ISoftUSBDevice *piSoftUSBDevice, ISoftUSBInterfaces *piInterfaces);
     HRESULT CreateStrings();
     HRESULT ByteArrayToSafeArray(BYTE *pbData, ULONG cbData, SAFEARRAY *&rpsa);
     VOID DisplayDeviceRequest(USBSETUPREQUEST *pSetupRequest);
     LPCSTR EntityIDToString(BYTE bEntityID);
     
     HRESULT ProcessAudioControlInterfaceRequest(USBSETUPREQUEST  *pSetupRequest,
                                                 BYTE             *pbHostData,
                                                 ULONG            cbHostData,
                                                 BYTE            **ppbResponseData,
                                                 ULONG            *pcbResponseData,
                                                 BYTE             *pbSetupStatus
                                                );

     HRESULT ProcessUpDownMixProcessingRequest(USBSETUPREQUEST  *pSetupRequest,
                                               BYTE             *pbHostData,
                                               ULONG            cbHostData,
                                               BYTE            **ppbResponseData,
                                               ULONG            *pcbResponseData,
                                               BYTE             *pbSetupStatus
                                              );

     HRESULT ProcessFeatureUnitRequest(USBSETUPREQUEST  *pSetupRequest,
                                       BYTE             *pbHostData,
                                       ULONG            cbHostData,
                                       BYTE            **ppbResponseData,
                                       ULONG            *pcbResponseData,
                                       BYTE             *pbSetupStatus
                                      );

     HRESULT ProcessMixerUnitRequest(USBSETUPREQUEST  *pSetupRequest,
                                     BYTE             *pbHostData,
                                     ULONG            cbHostData,
                                     BYTE            **ppbResponseData,
                                     ULONG            *pcbResponseData,
                                     BYTE             *pbSetupStatus
                                    );

     HRESULT ProcessEndpointRequest(USBSETUPREQUEST  *pSetupRequest,
                                    BYTE             *pbHostData,
                                    ULONG             cbHostData,
                                    BYTE            **ppbResponseData,
                                    ULONG            *pcbResponseData,
                                    BYTE             *pbSetupStatus
                                   );

     VOID DrainIsochEndpoints();

     HRESULT SetupConnectionPoint(IUnknown *punkObject,
                                  REFIID    iidConnectionPoint);
     HRESULT ReleaseConnectionPoint();

     typedef struct
     {
         struct BandsPresent
         {
             BYTE fBand14:1;    // =1 if band 14 is present, =0 if band 14 is not present
             BYTE fBand15:1;    // =1 if band 15 is present etc.
             BYTE fBand16:1;
             BYTE fBand17:1;
             BYTE fBand18:1;
             BYTE fBand19:1;
             BYTE fBand20:1;
             BYTE fBand21:1;
             BYTE fBand22:1;
             BYTE fBand23:1;
             BYTE fBand24:1;
             BYTE fBand25:1;
             BYTE fBand26:1;
             BYTE fBand27:1;
             BYTE fBand28:1;
             BYTE fBand29:1;
             BYTE fBand30:1;
             BYTE fBand31:1;
             BYTE fBand32:1;
             BYTE fBand33:1;
             BYTE fBand34:1;
             BYTE fBand35:1;
             BYTE fBand36:1;
             BYTE fBand37:1;
             BYTE fBand38:1;
             BYTE fBand39:1;
             BYTE fBand40:1;
             BYTE fBand41:1;
             BYTE fBand42:1;
             BYTE fBand43:1;
             BYTE Reserved:2;
         } bmBandsPresent;

         // These are the bands we implement

         BYTE bBand15;          // 31.5 Hz band
         BYTE bBand18;          // 62 Hz band
         BYTE bBand21;          // 125 Hz band
         BYTE bBand24;          // 250 Hz band
         BYTE bBand27;          // 500 Hz band
         BYTE bBand30;          // 1 KHz band
         BYTE bBand33;          // 2 KHz band
         BYTE bBand36;          // 4 KHz band
         BYTE bBand39;          // 8 KHz band
         BYTE bBand42;          // 16 KHz band

     } EQDATA;

     ISoftUSBDevice        *m_piUSBDevice;                   // Underlying SoftUSBDevice object
     ISoftUSBEndpoint      *m_piEndpoint0;                   // control endpoint
     IConnectionPoint      *m_piConnectionPoint;             // Connection point interface on control endpoint
     ISoftUSBConfiguration *m_piConfig;                      // Configuration assoicated with the device
     ISoftUSBInterface     *m_piCurrentInterface;            // The currently select alternate for the streaming inteface
     ISoftUSBEndpoint      *m_piEndpoint;                    // Cached endpoint so thae queue can be drained quicker
     BYTE                   m_bStreamInterface;              // Number of the streaming interface
     BYTE                   m_bAlternateInterface;           // Alternate for the streaming interface
     DWORD                  m_dwConnectionCookie;            // Connection point cookie.
     BYTE                   m_fMuted;                        // TRUE=mute control currently set to muted
     BYTE                   m_fPitchControlEnabled;          // TRUE=pitch control enabled for isoch endpoint
     WORD                   m_wVol;                          // current value of volume control
     BYTE                   m_bBass;                         // current value of bass control
     BYTE                   m_bTreble;                       // current value of treble control
     WORD                   m_wLineVol;                      // current value of line in volume control
     BYTE                   m_bLineBass;                     // current value of line in bass control
     BYTE                   m_bLineTreble;                   // current value of line in treble control
     WORD                   m_wLowFreqVol;                   // current value of low freq volume control
     BYTE                   m_bLowFreqBass;                  // current value of low freq bass control
     BYTE                   m_bLowFreqTreble;                // current value of low freq treble control
     WORD                   m_wCombineCh1Vol;                // current value of Combine feature unit ch1 volume control
     BYTE                   m_bCombineCh1Bass;               // current value of Combine feature unit ch1 bass control
     BYTE                   m_bCombineCh1Treble;             // current value of Combine feature unit ch1 treble control
     WORD                   m_wCombineCh2Vol;                // current value of Combine feature unit ch2 volume control
     BYTE                   m_bCombineCh2Bass;               // current value of Combine feature unit ch2 bass control
     BYTE                   m_bCombineCh2Treble;             // current value of Combine feature unit ch2 treble control
     BYTE                   m_ProcessingUnitMode2[4];        // current mode of Audio Control Processing Unit2 channels
     BYTE                   m_ProcessingUnitMode3[2];        // current mode of Audio Control Processing Unit3 channels
     BYTE                   m_abMinIsochEndpointSampFreq[3]; // min value of isoch endpoint sampling frequency control
     BYTE                   m_abMaxIsochEndpointSampFreq[3]; // max value of isoch endpoint sampling frequency control
     BYTE                   m_abResIsochEndpointSampFreq[3]; // resolution of isoch endpoint sampling frequency control
     BYTE                   m_abCurIsochEndpointSampFreq[3]; // current value of isoch endpoint sampling frequency control
     EQDATA                 m_EqDataCur;                     // graphic equalizer settings
     EQDATA                 m_EqDataMin;
     EQDATA                 m_EqDataMax;
     EQDATA                 m_EqDataRes;
     BOOL                   m_fDisplayDeviceRequests;        // TRUE=display audio class-specific device requests in console window
     LONG                   m_lPacketCount;                  // Holds number of packets received by device

public:

    //IAudioDevice
    STDMETHOD(get_DSFDevice)(DSFDevice** ppDSFDevice);
    STDMETHOD(StartSimulation)();
    STDMETHOD(AreKeystrokesWaiting)(VARIANT_BOOL *pfvarKeyWaiting);
    STDMETHOD(DisplayDeviceRequests)(VARIANT_BOOL fvarDisplay);
    STDMETHOD(get_PacketsReceived)(LONG* plPacketCount);

    //ISoftUSBEndpointEvents
    STDMETHOD(OnSetupTransfer)(BYTE DataToggle,BYTE *pbDataBuffer,
                               ULONG cbDataBuffer, BYTE *pbStatus);

    STDMETHOD(OnWriteTransfer)(BYTE DataToggle, BYTE *pbDataBuffer,
                               ULONG cbDataBuffer, BYTE *pbStatus);

    STDMETHOD(OnReadTransfer)(BYTE DataToggle, BYTE  *pbDataBuffer,
                              ULONG   cbDataBuffer,ULONG *cbDataWritten,
                              BYTE *pbStatus);       

    STDMETHOD(OnDeviceRequest)(USBSETUPREQUEST *pSetupRequest,
                               ULONG *RequestHandle, 
                               BYTE  *pbHostData, ULONG cbHostData,
                               BYTE **ppbResponseData,
                               ULONG *pcbResponseData,BYTE  *pbSetupStatus);

    STDMETHOD(OnDeviceRequestComplete)(ULONG RequestHandle,
                                       BYTE *pbFinalRequestStatus);
};

OBJECT_ENTRY_AUTO(__uuidof(AudioDevice), CAudioDevice)


