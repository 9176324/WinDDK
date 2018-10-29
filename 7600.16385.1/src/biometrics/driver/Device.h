/*++

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
    ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.

    Copyright (c) Microsoft Corporation. All rights reserved

Module Name:

    Device.h

Abstract:

    This module contains the type definitions of the Biometric
    device driver.

Environment:

    Windows User-Mode Driver Framework (WUDF)

--*/

#pragma once

//
// FX2 specifics (can be removed)
//
#define ENDPOINT_TIMEOUT        10000
#define NUM_OSRUSB_ENDPOINTS    3

//
// Define the vendor commands supported by our device
//
#define USBFX2LK_READ_7SEGMENT_DISPLAY      0xD4
#define USBFX2LK_READ_SWITCHES              0xD6
#define USBFX2LK_READ_BARGRAPH_DISPLAY      0xD7
#define USBFX2LK_SET_BARGRAPH_DISPLAY       0xD8
#define USBFX2LK_IS_HIGH_SPEED              0xD9
#define USBFX2LK_REENUMERATE                0xDA
#define USBFX2LK_SET_7SEGMENT_DISPLAY       0xDB

typedef struct {
    UCHAR Segments;
} SEVEN_SEGMENT, *PSEVEN_SEGMENT;


//
// Struct for passing parameters for capture request completion.
//
typedef struct _CAPTURE_SLEEP_PARAMS
{
    DWORD SleepValue;
    HRESULT Hr;
    DWORD Information;
} CAPTURE_SLEEP_PARAMS, *PCAPTURE_SLEEP_PARAMS;


//
// Class for the Biometric driver.
//

class CBiometricDevice :
    public CComObjectRootEx<CComMultiThreadModel>,
    public IRequestCallbackRequestCompletion,
    public IRequestCallbackCancel,
    public IPnpCallbackHardware
{
public:

    DECLARE_NOT_AGGREGATABLE(CBiometricDevice)

    BEGIN_COM_MAP(CBiometricDevice)
        COM_INTERFACE_ENTRY(IPnpCallbackHardware)
        COM_INTERFACE_ENTRY(IRequestCallbackRequestCompletion)
        COM_INTERFACE_ENTRY(IRequestCallbackCancel)
    END_COM_MAP()

    CBiometricDevice() :
        m_FxDevice(NULL),
        m_IoQueue(NULL),
        m_SwitchChangeQueue(NULL),
        m_pIUsbTargetDevice(NULL),
        m_pIUsbInterface(NULL),
        m_pIUsbInputPipe(NULL),
        m_pIUsbOutputPipe(NULL),
        m_pIUsbInterruptPipe(NULL),
        m_PendingRequest(NULL),
        m_Speed(0),
        m_InterruptReadProblem(S_OK),
        m_SleepThread(INVALID_HANDLE_VALUE)

    {
    }

    ~CBiometricDevice()
    {
    }

//
// Private data members.
//
private:

    //
    // Weak reference to framework device object.
    //
    IWDFDevice *            m_FxDevice;

    //
    // Weak reference to I/O queue
    //
    PCBiometricIoQueue      m_IoQueue;

    //
    // USB Device I/O Target
    //
    IWDFUsbTargetDevice *   m_pIUsbTargetDevice;

    //
    // USB Interface
    //
    IWDFUsbInterface *      m_pIUsbInterface;

    //
    // USB Input pipe for Reads
    //
    IWDFUsbTargetPipe *     m_pIUsbInputPipe;

    //
    // USB Output pipe for writes
    //
    IWDFUsbTargetPipe *     m_pIUsbOutputPipe;

    //
    // USB interrupt pipe
    //
    IWDFUsbTargetPipe *     m_pIUsbInterruptPipe;

    //
    // Device Speed (Low, Full, High)
    //
    UCHAR                   m_Speed;    

    //
    // Current switch state
    //
    SWITCH_STATE            m_SwitchState;

    //
    // If reads stopped because of a transient problem, the error status
    // is stored here.
    //

    HRESULT                 m_InterruptReadProblem;
    
    //
    // Switch state buffer - this might hold the transient value
    // m_SwitchState holds stable value of the switch state
    //
    SWITCH_STATE            m_SwitchStateBuffer;

    //
    // A manual queue to hold requests for changes in the I/O switch state.
    //

    IWDFIoQueue *           m_SwitchChangeQueue;

    //
    // Holds a reference to a pending data I/O request.
    //

    IWDFIoRequest               *m_PendingRequest;

    //
    // Handle to a thread that will sleep before completing a request.
    //
    HANDLE                  m_SleepThread;
    CAPTURE_SLEEP_PARAMS    m_SleepParams;

//
// Private methods.
//
private:

    HRESULT
    Initialize(
        __in IWDFDriver *FxDriver,
        __in IWDFDeviceInitialize *FxDeviceInit
        );

    //
    // Helper methods
    //

    HRESULT
    CreateUsbIoTargets(
        VOID
        );

    
    HRESULT
    ConfigureUsbPipes(
        );

    HRESULT
    SetPowerManagement(
        VOID
        );
    
    HRESULT
    IndicateDeviceReady(
        VOID
        );

    //
    // Helper functions
    //
    
    HRESULT
    SendControlTransferSynchronously(
        __in PWINUSB_SETUP_PACKET SetupPacket,
        __inout_ecount(BufferLength) PBYTE Buffer,
        __in ULONG BufferLength,
        __out PULONG LengthTransferred
        );

    static
    WDF_IO_TARGET_STATE
    GetTargetState(
        IWDFIoTarget * pTarget
        );
    
    HRESULT
    InitiatePendingRead(
        );

    VOID
    ServiceSwitchChangeQueue(
        __in SWITCH_STATE NewState,
        __in HRESULT CompletionStatus,
        __in_opt IWDFFile *SpecificFile
        );

//
// Public methods
//
public:

    //
    // The factory method used to create an instance of this driver.
    //
    
    static
    HRESULT
    CreateInstanceAndInitialize(
        __in IWDFDriver *FxDriver,
        __in IWDFDeviceInitialize *FxDeviceInit,
        __out CBiometricDevice **Device
        );

    HRESULT
    Configure(
        VOID
        );

    //
    // FX2 specific routines
    //
    
    HRESULT
    GetSevenSegmentDisplay(
        __in PSEVEN_SEGMENT SevenSegment
        );

    HRESULT
    SetSevenSegmentDisplay(
        __in PSEVEN_SEGMENT SevenSegment
        );

//
// COM methods
//
public:

    //
    // IPnpCallbackHardware
    //

    virtual
    HRESULT
    STDMETHODCALLTYPE
    OnPrepareHardware(
            __in IWDFDevice *FxDevice
            );

    virtual
    HRESULT
    STDMETHODCALLTYPE
    OnReleaseHardware(
        __in IWDFDevice *FxDevice
        );


    //
    // IRequestCallbackRequestCompletion
    //
    virtual
    void
    STDMETHODCALLTYPE
    OnCompletion(
        IWDFIoRequest*                 FxRequest,
        IWDFIoTarget*                  pIoTarget,
        IWDFRequestCompletionParams*   pParams,
        PVOID                          pContext
        );

    //
    // IRequestCallbackCancel
    //
    virtual
    VOID
    STDMETHODCALLTYPE
    OnCancel(
        __in IWDFIoRequest *pWdfRequest
        );

public:

    //
    // I/O handlers.
    //
    void
    GetIoRequestParams(
        __in IWDFIoRequest *FxRequest,
        __out ULONG *MajorControlCode,
        __deref_out_bcount(*InputBufferSizeInBytes) PUCHAR *InputBuffer,
        __out SIZE_T *InputBufferSizeInBytes,
        __deref_out_bcount(*OutputBufferSizeInBytes) PUCHAR *OutputBuffer,
        __out SIZE_T *OutputBufferSizeInBytes
        );

    void
    OnGetAttributes(
        __inout IWDFIoRequest *FxRequest
        );

    void
    OnReset(
        __inout IWDFIoRequest *FxRequest
        );

    void
    OnCalibrate(
        __inout IWDFIoRequest *FxRequest
        );

    void
    OnGetSensorStatus(
        __inout IWDFIoRequest *FxRequest
        );

    void
    OnCaptureData(
        __inout IWDFIoRequest *FxRequest
        );

    void
    OnUpdateFirmware(
        __inout IWDFIoRequest *FxRequest
        );
 
    void
    OnGetSupportedAlgorithms(
        __inout IWDFIoRequest *FxRequest
        );

    void
    OnGetIndicator(
        __inout IWDFIoRequest *FxRequest
        );

    void
    OnSetIndicator(
        __inout IWDFIoRequest *FxRequest
        );
 
    void
    OnControlUnit(
        __inout IWDFIoRequest *FxRequest
        );

    VOID
    CompletePendingRequest(
        HRESULT hr,
        DWORD   information
        );

    inline PCAPTURE_SLEEP_PARAMS
    GetCaptureSleepParams()
    {
        return &m_SleepParams;
    }

};



