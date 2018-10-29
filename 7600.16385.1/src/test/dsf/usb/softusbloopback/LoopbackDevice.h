/*++ BUILD Version 0000

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    LoopbackDevice.h

Abstract:
    Definition of the class CLoopbackDevice

--*/

#pragma once
#include "resource.h"       // main symbols

#include "SoftUSBLoopback.h"
#include "LoopbackDeviceEvents.h"


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

// CLoopbackDevice

class ATL_NO_VTABLE CLoopbackDevice : 
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CLoopbackDevice, &CLSID_LoopbackDevice>,
    public IConnectionPointContainerImpl<CLoopbackDevice>,
    public CProxy_ILoopbackDeviceEvents<CLoopbackDevice>, 
    public ISoftUSBEndpointEvents,
    public IDispatchImpl<ILoopbackDevice, &IID_ILoopbackDevice, &LIBID_SoftUSBLoopbackLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
    CLoopbackDevice();
    virtual ~CLoopbackDevice();

DECLARE_REGISTRY_RESOURCEID(IDR_LOOPBACKDEVICE)


BEGIN_COM_MAP(CLoopbackDevice)
    COM_INTERFACE_ENTRY(ILoopbackDevice)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(ISoftUSBEndpointEvents)
    COM_INTERFACE_ENTRY(IConnectionPointContainer)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CLoopbackDevice)
    CONNECTION_POINT_ENTRY(__uuidof(ILoopbackDeviceEvents))
END_CONNECTION_POINT_MAP()


    DECLARE_PROTECT_FINAL_CONSTRUCT()

    HRESULT FinalConstruct();
    
    void FinalRelease();

public:

private:
     void InitMemberVariables();

     HRESULT CreateUSBDevice();
     HRESULT CreateStrings();
     HRESULT ConfigureDevice();
     HRESULT ConfigureOUTEndpoint();
     HRESULT ConfigureINEndpoint();
     HRESULT ConfigureInterface(ISoftUSBInterface *piInterface);
     HRESULT ConfigureConfig(ISoftUSBConfiguration *piConfig);

     HRESULT SetupConnectionPoint(IUnknown *punkObject,
                                  REFIID    iidConnectionPoint);
     HRESULT ReleaseConnectionPoint();

     ISoftUSBDevice          *m_piSoftUSBDevice;    //Underlying SoftUSBDevice object
     ISoftUSBEndpoint        *m_piINEndpoint;       //IN Endpoint
     ISoftUSBEndpoint        *m_piOUTEndpoint;      //OUT Endpoint
     IConnectionPoint        *m_piConnectionPoint;  //Connection point interface
     DWORD                    m_dwConnectionCookie; //Connection point cookie.
     int                      m_iInterfaceString;   //Index of interface identifier string
     int                      m_iConfigString;      //Index of config identifier string
     
public:

    //ILoopbackDevice
    STDMETHOD(get_DSFDevice)(DSFDevice** ppDSFDevice);
    STDMETHOD(DoPolledLoopback)(long lTimeInterval);
    STDMETHOD(StartEventProcessing)();
    STDMETHOD(StartAsyncEventProcessing)();
    STDMETHOD(StopAsyncEventProcessing)();
    STDMETHOD(AreKeystrokesWaiting)(VARIANT_BOOL *pfvarKeyWaiting);
    STDMETHOD(put_SerialNumber)(BSTR bstrSerialNumber);

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

OBJECT_ENTRY_AUTO(__uuidof(LoopbackDevice), CLoopbackDevice)

