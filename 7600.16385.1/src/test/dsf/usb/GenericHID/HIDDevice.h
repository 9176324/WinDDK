/*++ BUILD Version 0000

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    HIDDevice.h

Abstract:
    Definition of the class CHIDDevice

--*/

#pragma once

#include "resource.h"        // Main symbols
#include "SoftHIDReceiver.h" // Main library header
#include "common.h"          // Contains common macros
#include <USBProtocolDefs.h> // Contains definitions and friendly names for USB related defines and structures
#include "CircularQueue.h"   // Contains the queue used for Input Reports

// This stuct is used to store information about a HID report to be sent 
struct HID_INPUT_REPORT {
    
    BYTE *pbInputReport;  // Array of bytes with the HID report payload
    UINT  cbInputReport;  // The number of bytes in the HID report
    UINT  timeout;        // Amount of time to pause after sending this payload

};

typedef CircularQueue<HID_INPUT_REPORT> InputReportQueue;

// CHIDDevice
class ATL_NO_VTABLE CHIDDevice : 
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CHIDDevice, &CLSID_HIDDevice>,
    public ISoftUSBEndpointEvents,
    public IDispatchImpl<IHIDDevice, &IID_IHIDDevice, &LIBID_SoftHIDReceiver, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
    CHIDDevice();
    virtual ~CHIDDevice();

DECLARE_REGISTRY_RESOURCEID(IDR_HIDDEVICE)

BEGIN_COM_MAP(CHIDDevice)
    COM_INTERFACE_ENTRY(IHIDDevice)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(ISoftUSBEndpointEvents)
END_COM_MAP()

    DECLARE_PROTECT_FINAL_CONSTRUCT()

    HRESULT FinalConstruct();
    void    FinalRelease();

private:

    void InitMemberVariables();
    void LogMessage( __in WCHAR* messageToLog );

    // DSF\USB required functions
    HRESULT CreateUSBDevice();
    
    HRESULT CreateStrings();
    
    HRESULT AddString( ISoftUSBStrings *piStringList,
                       ISoftUSBString  *piString,
                       BSTR            bstrVal,
                       VARIANT         *vIndex );
    
    HRESULT ConfigureINEndpoint();
    
    HRESULT ConfigureInterface(ISoftUSBInterface *piInterface);
    
    HRESULT ConfigureConfig(ISoftUSBConfiguration *piConfig);
    
    HRESULT SetupControlConnectionPoint(IUnknown *punkObject,
                                        REFIID    iidConnectionPoint);
                                        
    HRESULT ReleaseControlConnectionPoint();
    
    HRESULT EndpointDiagnosticsHelper(ISoftUSBEndpoint *piSoftUSBEndpoint, ULONG diagnosticType);

    HRESULT CreateHIDDescriptor();
    
    HRESULT CreateReportDescriptor();
    
    HRESULT GetReportDescriptor( BYTE  **ppbDataBuffer,
                                 ULONG  *pcbDataWritten,
                                 SHORT   cbMaxBufferLen );
                                 
    HRESULT Get_Report( USBSETUPREQUEST  *pSetupRequest,
                        BYTE            **ppbDataBuffer,
                        ULONG            *pcbDataWritten,
                        SHORT             cbMaxBufferLen );
                        
    HRESULT Get_Descriptor( USBSETUPREQUEST  *pSetupRequest,
                            BYTE            **ppbDataBuffer,
                            ULONG            *pcbDataWritten );
    
    // Function Prototypes thread Proc
    HRESULT WINAPI InputReportThreadProc(); 

    HRESULT SendInputReport(UINT *timeout);
                            
    // These variables are required for DSF/USB implementation 
    ISoftUSBDevice          *m_piSoftUSBDevice;         // Underlying SoftUSBDevice object
    ISoftUSBEndpoint        *m_piINEndpoint;            // IN Endpoint
    ISoftUSBEndpoint        *m_piEndpoint0;             // Control Endpoint
    IConnectionPoint        *m_piCntrlConnectionPoint;  // Control Connection point interface
    SAFEARRAY               *m_psaHIDDescriptor;        // Contains the contents of the HID Descriptor
    SAFEARRAY               *m_psaReportDescriptor;     // Contains the contents of the Report Descriptor
    DWORD                    m_dwCntrlConnectionCookie; // Control Connection point cookie.
    UINT                     m_SerialNumber;            // Contains the unique serial number of the device
    UINT                     m_IdleRate;                // The rate at which Input Reports should be process
    SHORT                    m_VendorID;                // Contains the ID for the manufacturer of the device
    SHORT                    m_ProductID;               // Contains the Product ID for the Device
    SHORT                    m_DeviceID;                // Contains the Revision number for the Device
    BOOLEAN                  m_Logging;                 // Tells the device whether it should log messages or not
    BOOLEAN                  m_ProcessInputReports;     // Used to control the InputReport Thread lifetime
    InputReportQueue         m_InputReportQueue;        // This Queue is used to store InputRequests
    HANDLE                   m_ThreadHandle;            // Used to store the thread handle
    DWORD                    m_InputReportThreadID;     // Used to store the ID for the InputReport thread
    
public:
    //
    // Interface Prototypes
    //

    //IHIDDevice
    STDMETHOD(get_DSFDevice)(DSFDevice** ppDSFDevice);
    STDMETHOD(get_Logging)( VARIANT_BOOL* logLevel );
    STDMETHOD(put_Logging)( VARIANT_BOOL logLevel );
    STDMETHOD(get_IdleTimeout)( UINT* IdleTimeout );
    STDMETHOD(put_IdleTimeout)( UINT IdleTimeout );
    STDMETHOD(SetEndpointDiagnostics)( BYTE endpointNum, ULONG diagnosticType );
    STDMETHOD(QueueInputReport)( SAFEARRAY* psaInputReport, UINT timeoutDuration );
    STDMETHOD(StartProcessing)();
    STDMETHOD(StopProcessing)();
    STDMETHOD(Destroy)();
    STDMETHOD(CreateCustomHIDDescriptor)( SAFEARRAY* psaHIDDescriptor );
    STDMETHOD(CreateCustomReportDescriptor)( SAFEARRAY* psaReportDescriptor );
    STDMETHOD(ConfigureDevice)();
    STDMETHOD(BuildHIDDescriptors());
    
    //ISoftUSBEndpointEvents
    STDMETHOD(OnSetupTransfer)( BYTE   dataToggle,
                                BYTE  *pbDataBuffer,
                                ULONG  cbDataBuffer, 
                                BYTE  *pbStatus);

    STDMETHOD(OnWriteTransfer)( BYTE   dataToggle, 
                                BYTE  *pbDataBuffer,
                                ULONG  cbDataBuffer, 
                                BYTE  *pbStatus);

    STDMETHOD(OnReadTransfer)( BYTE   dataToggle, 
                               BYTE  *pbDataBuffer,
                               ULONG  cbDataBuffer,
                               ULONG *cbDataWritten,
                               BYTE  *pbStatus);       

    STDMETHOD(OnDeviceRequest)( USBSETUPREQUEST *pSetupRequest,
                                ULONG           *requestHandle, 
                                BYTE            *pbHostData,
                                ULONG            cbHostData,
                                BYTE           **ppbResponseData,
                                ULONG           *pcbResponseData,
                                BYTE            *pbSetupStatus);

    STDMETHOD(OnDeviceRequestComplete)(ULONG  requestHandle,
                                       BYTE  *pbFinalRequestStatus);
                                       
    // This function is static to work around the 'this*' limitation when creating a new thread
    static  HRESULT WINAPI InputReportThreadProcWrapper( void* lpVoid );                                      
};

OBJECT_ENTRY_AUTO(__uuidof(HIDDevice), CHIDDevice)

