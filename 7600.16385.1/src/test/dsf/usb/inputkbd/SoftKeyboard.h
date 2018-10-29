/*++ BUILD Version: 0000    

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    SoftKeyboard.h

Abstract:
    Defines the class SoftKeyboard which implements the ISoftKeyboard interface
 
Revision History:

--*/
// SoftKeyboard.h : Declaration of the CSoftKeyboard

#pragma once
#include "resource.h"       // main symbols

#include "InputKbd.h"
#include "Common.h"
#include "_ISoftKeyboardEvents_CP.h"
#include "_ISoftKbdDeviceEvents_CP.h"
#include "SoftKbdTranslator.h"

// Event messages 
#define MSG_KEYPRESSED          TEXT("C827C563-31D8-4E4C-82D9-9272225F78F9")
#define MSG_KEYRELEASED         TEXT("F937C49E-2DEC-4789-87AE-9E854944C12B")

// CSoftKeyboard

class ATL_NO_VTABLE CSoftKeyboard : 
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CSoftKeyboard, &CLSID_SoftKeyboard>,
    public IConnectionPointContainerImpl<CSoftKeyboard>,
    public CProxy_ISoftKeyboardEvents<CSoftKeyboard>, 
    public CProxy_ISoftInputKbdDeviceEvents<CSoftKeyboard>, 
    public IDispatchImpl<ISoftKeyboard, &IID_ISoftKeyboard, &LIBID_INPUTKBD, /*wMajor =*/ 1, /*wMinor =*/ 0>,
    public CMessageTask,
    public ISoftInputKbdDevice
{
public:
    CSoftKeyboard();
    ~CSoftKeyboard();

    DECLARE_REGISTRY_RESOURCEID(IDR_SOFTKEYBOARD)


    BEGIN_COM_MAP(CSoftKeyboard)
        COM_INTERFACE_ENTRY(ISoftKeyboard)
        COM_INTERFACE_ENTRY(IDispatch)
        COM_INTERFACE_ENTRY(IConnectionPointContainer)
        COM_INTERFACE_ENTRY(ISoftInputKbdDevice)
    END_COM_MAP()

    BEGIN_CONNECTION_POINT_MAP(CSoftKeyboard)
        CONNECTION_POINT_ENTRY(__uuidof(ISoftKeyboardEvents))
        CONNECTION_POINT_ENTRY(__uuidof(ISoftInputKbdDeviceEvents))
    END_CONNECTION_POINT_MAP()

    DECLARE_PROTECT_FINAL_CONSTRUCT()

    HRESULT FinalConstruct();

    void FinalRelease() 
    {
    }

    // Constants
    static const DWORD SOFTKBD_TRACE_DATA   = 0x10000000;
    static const DWORD SOFTKBD_TRACE_STATUS = 0x20000000;
    static const DWORD SOFTKBD_TRACE_CMDS   = 0x40000000;
    static const DWORD SOFTKBD_TRACE_INFO   = 0x80000000;
    static const DWORD SOFTKBD_TRACE_SIMPLE = 0x01000000;

    static const USHORT cusKeyboardBufferSize = 100;

protected:

    STDMETHOD(get_RepeatDelay)(ULONG *psDelay);
    STDMETHOD(get_RepeatRate)(ULONG *psRate);
    STDMETHOD(get_IndicatorStatus)(INDICATOR_KEY LedKey,
        VARIANT_BOOL *pfvarStatus);
    STDMETHOD(get_BufferSize)(SHORT *psNumberOfEvents);
    STDMETHOD(get_Translator)(SoftKbdTranslator **ppTranslator);
    STDMETHOD(put_Translator)(SoftKbdTranslator *pTranslator);
    STDMETHOD(putref_Translator)(SoftKbdTranslator *pTranslator);
    STDMETHOD(HitKeyOnce)(KBD_KEYS KeyCode);
    STDMETHOD(PressKey)(KBD_KEYS KeyCode);
    STDMETHOD(ReleaseKey)(KBD_KEYS KeyCode);
    STDMETHOD(PressAndReleaseKeys)(BSTR bstrKeyStrokes);
 
    // ISoftInputKbdDevice interface
    STDMETHOD(get_Attributes)(
                KEYBOARD_ATTRIBUTES **ppAttributes);

    STDMETHOD(put_Attributes)(KEYBOARD_ATTRIBUTES *pAttributes);
    
    STDMETHOD(get_IndicatorTranslation)(
           KEYBOARD_INDICATOR_TRANSLATION **ppIndicatorTrans);

    STDMETHOD(put_IndicatorTranslation)(
               KEYBOARD_INDICATOR_TRANSLATION *pIndicatorTrans);

    STDMETHOD(get_Typematic)(
                 KEYBOARD_TYPEMATIC_PARAMETERS **ppTypematic);

    STDMETHOD(put_Typematic)(
                    KEYBOARD_TYPEMATIC_PARAMETERS *pTypematic);
    STDMETHOD(get_Indicators)(
                KEYBOARD_INDICATOR_PARAMETERS **ppIndicator);
                    
    STDMETHOD(put_Indicators)(
                    KEYBOARD_INDICATOR_PARAMETERS *pIndicator);

    STDMETHOD(Read)(USHORT *pUnits,
                KEYBOARD_INPUT_DATA **ppKbdData); 
         

private:    

    // Scan code (PS/2 sets 1 and 2, HID usage)
    typedef struct _SCANCODE_TABLE
    {
        struct PS2ScanCode
        {
            ULONG ulMake;     // Make scan code
            ULONG ulBreak;    // Break scan code
        } Set1, Set2;

        struct HIDUsage
        {
            USHORT usUsagePage;   // HID Usage Page (0x07 or 0x01)
            USHORT usUsageID;     // HID Usage ID
        } HID;
    } SCANCODE_TABLE;

    // Enum of keyboard states
    typedef enum _INTERNAL_STATE
    {
        IDLE,
        MAKECODE,
        WAIT,
        AUTOREPEAT,
    } INTERNAL_STATE;

    
    // Methods
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////

    void            InitMemberVariables(void);
    virtual void    OnMsgReceived(CMessageTask::TASK_MSG msg, WPARAM wParam, LPARAM lParam);
    BOOLEAN         PutMessageToBuffer(KBD_KEYS KeyCode,USHORT usFlags);
    HRESULT         TranslateKeyCode2ScanCode(ULONG* pulMakeCode,KBD_KEYS KeyCode, USHORT usFlags);
    void            SetState(INTERNAL_STATE state);
    void            DetectIndicatorStatusChange(USHORT usNewFlags, INDICATOR_KEY LedKey);


    // Properties
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////

    __deref_volatile ISoftKbdTranslator*               m_piSoftKbdTranslator;  // Soft translator interface
    KEYBOARD_ATTRIBUTES            m_KbdAttributes;     // Keyboard attribs
    KEYBOARD_INDICATOR_TRANSLATION m_KbdIndicatorTrans; // Indicator translation
    KEYBOARD_TYPEMATIC_PARAMETERS  m_KbdTypematicParam; // Typematic params
    KEYBOARD_INDICATOR_PARAMETERS  m_KbdIndicatorParam; // Indicator params


    // Internal variables
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////

    CComAutoCriticalSection     m_KbdBufferLock; 
    USHORT                      m_usHeadAnchor;   // Head index in circular buffer
    USHORT                      m_usTailAnchor;   // Tail index in circular buffer
    KEYBOARD_INPUT_DATA         m_KbdBuffer[cusKeyboardBufferSize]; // Circular buffer
    LPMALLOC                    m_pMalloc;        // Pointer to COM task memory allocator
    INTERNAL_STATE              m_State;          // Keyboard state
    CMessageTask::TASK_TIMER    m_DelayTimer;     // Delay timer 
    CMessageTask::TASK_TIMER    m_RepeatTimer;    // Autorepeat timer
    CMessageTask::TASK_MSG      m_KeyPressedMsg;  // KeyPressed message
    CMessageTask::TASK_MSG      m_KeyReleasedMsg; // KeyReleased message
    KBD_KEYS                    m_LastKeyPressed; // Last key that was "pressed" on keyboard
    static SCANCODE_TABLE       m_KeyCode2ScanCodeTable[127];


    // ISoftKbdDevice Methods
public:
};

OBJECT_ENTRY_AUTO(__uuidof(SoftKeyboard), CSoftKeyboard)

