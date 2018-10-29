/*++ BUILD Version: 0000    

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    SoftHIDInputKbdMapper.h

Abstract:
    Definition of the CSoftHIDInputKbdMapper class which implements
        the ISoftHIDInputKbdMapper interface
 
Revision History:

--*/ 
#pragma once


#define MSG_KEYTOUCHED           TEXT("53A34157-B1F1-47FE-B462-C336FCC72B8F")
#define MSG_OUTREPORTREADY       TEXT("2895B3B4-BD86-4A37-9F36-4586469D3500")

// Maximum number of keys allowed to be simultaneously pressed
#define MAX_KEYSEQ              6

// CSoftHIDInputKbdMapper

class ATL_NO_VTABLE CSoftHIDInputKbdMapper : 
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CSoftHIDInputKbdMapper, &CLSID_SoftHIDInputKbdMapper>,
    public IDispatchImpl<ISoftHIDInputKbdMapper, &IID_ISoftHIDInputKbdMapper, &LIBID_SOFTHID, /*wMajor =*/ 1, /*wMinor =*/ 0>,
    public ISoftInputKbdDeviceEvents,
    public CSoftHID,
    public CMessageTask
{
public:
    CSoftHIDInputKbdMapper();
    virtual ~CSoftHIDInputKbdMapper();

DECLARE_REGISTRY_RESOURCEID(IDR_SOFTHIDINPUTKBDMAPPER)


BEGIN_COM_MAP(CSoftHIDInputKbdMapper)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(ISoftInputKbdDeviceEvents)
    COM_INTERFACE_ENTRY(ISoftHIDInputKbdMapper)
END_COM_MAP()


    DECLARE_PROTECT_FINAL_CONSTRUCT()

    HRESULT FinalConstruct();
    
    void FinalRelease() 
    {
    }

public:

    STDMETHOD(get_InputReportMask)(SHORT* pVal);
    STDMETHOD(put_InputReportMask)(SHORT newVal);
    STDMETHOD(get_OutputReportMask)(BYTE* pVal);
    STDMETHOD(put_OutputReportMask)(BYTE newVal);
    STDMETHOD(AttachKbd)(SoftKeyboard *pSoftKbd);
    STDMETHOD(AttachProtocolXlator)(SoftHIDProtocolXlator* pSoftProtXlator);
    STDMETHOD(KeyTouched) (void);
private:

    // Structure that defines default HID Input Report
    typedef struct _HID_INPUTREPORT
    {
        BYTE bModifier;              
        BYTE bReserved;
        BYTE bUsageID[MAX_KEYSEQ];
    } HID_INPUTREPORT;


    // Methods
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    void TearDownEventSink(IConnectionPoint *&rpiConnectionPoint, DWORD &rdwCookie);
    HRESULT SetupEventSink(IUnknown *punkObject,IUnknown *punkSink,REFIID iidConnectionPoint,
                           IConnectionPoint *&rpiConnectionPoint,DWORD &rdwCookie);
    void            InitMemberVariables();
    virtual void    OnMsgReceived(CMessageTask::TASK_MSG msg,WPARAM wParam,LPARAM lParam);
    HRESULT         ProcessKbdInput();
    HRESULT         ProcessKbdOutput(SAFEARRAY* psa);
    HRESULT         SendInputReport(PVOID pbReportData,
                                    ULONG cbReportData);
    BOOLEAN         GenInputReport(USHORT usUsageID, USHORT usFlag);
    BOOLEAN         IsModifierKey(USHORT usUsageID)
    {
        return (usUsageID >= 0xE0 && usUsageID <= 0xE7)? TRUE : FALSE;
    }
    BYTE            XlateModifierKey(USHORT usUsageID)
    {
        return (BYTE)(1 << (usUsageID & 0x0F));
    }


    // Properties
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////

    ISoftInputKbdDevice*    m_piSoftKeyboard;   // Soft Keyboard interface
    ISoftHIDProtocolXlator* m_piSoftProtXlator; // Soft HID Protocol Translator interface
    SHORT                   m_sInputMask;       // Current input mask value
    UCHAR                   m_ucOutputMask;     // Current output mask value


    // Internal variables
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////

    LPMALLOC                m_pMalloc;              // Pointer to COM task memory allocator
    IConnectionPoint*       m_piConnectionPointKbd; // Pointer to ISoftInputKbdDeviceEvents connection point
    DWORD                   m_dwKbdSinkCookie;      // Cookie for ISoftInputKbdDeviceEvents sink
    HID_INPUTREPORT         m_InputReport;          // HID Input Report buffer
    BYTE                    m_bKeySeqLen;           // Number of keys that are pressed simultaneously
    CMessageTask::TASK_MSG  m_KeyTouchedMsg;        // KeyTouched message
    CMessageTask::TASK_MSG  m_OutReportReadyMsg;    // OutReportReady message
    SAFEARRAY*              m_psaReportDescriptor;  // Report descriptor wrapped in safearray
    static BYTE             m_ReportDescriptor[63]; // Report descriptor
};


OBJECT_ENTRY_AUTO(__uuidof(SoftHIDInputKbdMapper), CSoftHIDInputKbdMapper)


