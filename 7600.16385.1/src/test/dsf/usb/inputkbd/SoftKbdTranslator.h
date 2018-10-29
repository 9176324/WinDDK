/*++ BUILD Version: 0000    

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

   SoftKdbTranslator.h

Abstract:
    Defines the class CDefaultASCIIXlator
    
Revision History:

--*/

#pragma once
#include "resource.h"       // main symbols

#include "InputKbd.h"

#define SET_KEY(n)\
    { CDefaultASCIIXlator::m_KeyCodeLen1SubTable[n].Sequence }

#define SET_SHIFT_KEY(n)\
    { CDefaultASCIIXlator::m_KeyCodeLen3SubTable[n].Sequence }

#define ASSIGN_CODE1(n)\
    { n, KEY_MAKE | KEY_BREAK,\
      KEY_NONE, KEY_NONE }

#define UNASSIGNED1\
    { KEY_NONE, KEY_NONE,\
      KEY_NONE, KEY_NONE }

#define ASSIGN_CODE3(n)\
    { KEY_L_SHIFT, KEY_MAKE,\
      n, KEY_MAKE | KEY_BREAK,\
      KEY_L_SHIFT, KEY_BREAK,\
      KEY_NONE, KEY_NONE }

#define UNASSIGNED3\
    { KEY_NONE, KEY_NONE,\
      KEY_NONE, KEY_NONE,\
      KEY_NONE, KEY_NONE,\
      KEY_NONE, KEY_NONE }


// CDefaultASCIIXlator

class ATL_NO_VTABLE CDefaultASCIIXlator : 
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CDefaultASCIIXlator, &CLSID_SoftKbdTranslator>,
    public IDispatchImpl<ISoftKbdTranslator, &IID_ISoftKbdTranslator, &LIBID_INPUTKBD, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
    // Define two types for code sequences with lengths 1 and 3
    // Each sequence has last element set to 0
    typedef struct _KBD_KEYCODESEQ1 
    {
        KBD_KEYCODE Sequence[2];
    } KBD_KEYCODESEQ1;

    typedef struct _KBD_KEYCODESEQ3
    {
        KBD_KEYCODE Sequence[4];
    } KBD_KEYCODESEQ3;

    CDefaultASCIIXlator();
    ~CDefaultASCIIXlator();
    
DECLARE_REGISTRY_RESOURCEID(IDR_SOFTKBDTRANSLATOR)


BEGIN_COM_MAP(CDefaultASCIIXlator)
    COM_INTERFACE_ENTRY(ISoftKbdTranslator)
    COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()


    DECLARE_PROTECT_FINAL_CONSTRUCT()

    HRESULT FinalConstruct();

    void FinalRelease() 
    {
    }

protected:
    STDMETHOD(get_Language)(__out ULONG *pulLCID);

    STDMETHOD(TranslateString2Keycodes)(
              __in UINT nSize,
              __in_ecount(nSize) WCHAR *szStrW,
              __out UINT *pnUnits,
              __deref_out_ecount(*pnUnits) KBD_KEYCODE **ppKeyCodes);


private:

    void                  InitMemberVariables();

    ULONG       m_ulLCID; // Hold the current language setting

    LPMALLOC    m_pMalloc; // Pointer to COM task memory allocator

    static KBD_KEYCODE*     m_Ascii2KeyCodeTable[128];
    static KBD_KEYCODESEQ1  m_KeyCodeLen1SubTable[127];
    static KBD_KEYCODESEQ3  m_KeyCodeLen3SubTable[127];

};

OBJECT_ENTRY_AUTO(__uuidof(SoftKbdTranslator), CDefaultASCIIXlator)
