/*++ BUILD Version: 0000    

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    SoftKeyboard.cpp

Abstract:
    Implements the class CSoftKeyboard
 
Revision History:

--*/

#include "stdafx.h"
#include "MessageTask.h"
#include "SoftKeyboard.h"
#include ".\softkeyboard.h"
#include "Common.h"


// CSoftKeyboard


// Scan Code Table as taken from 
// "Keyboard Scan Code Specification. Revision 1.3a" and
// "USB HID to PS/2 Scan Code Translation Table"
// Notes:
// ~~~~~~
// *   - Key 29 is available on the US and not on the international Keyboard.
// **  - Keys 42 and 45 are available on the international Keyboard but not 
//       on the US Keyboard.
// *** - Keys 56 and 107 are used on Brazilian and some Far East keyboards.
//       They are not available on US Keyboards.
// Key 126 is not typematic and has no Break Code. Break field is used as
// second part of Make code.
#define UNASSIGNED\
    {0x00, 0x00}

CSoftKeyboard::SCANCODE_TABLE CSoftKeyboard::m_KeyCode2ScanCodeTable[127] =
{
    {{0x00, 0x80},     {0x00, 0xF000},     { 0x07, 0x00 }},  // Not used
    {{0x29, 0xA9},     {0x0E, 0xF00E},     { 0x07, 0x35 }},  // Key location 1  ( ~ ` )
    {{0x02, 0x82},     {0x16, 0xF016},     { 0x07, 0x1E }},  // Key location 2  ( ! 1 )
    {{0x03, 0x83},     {0x1E, 0xF01E},     { 0x07, 0x1F }},  // Key location 3  ( @ 2 )
    {{0x04, 0x84},     {0x26, 0xF026},     { 0x07, 0x20 }},  // Key location 4  ( # 3 )
    {{0x05, 0x85},     {0x25, 0xF025},     { 0x07, 0x21 }},  // Key location 5  ( $ 4 )
    {{0x06, 0x86},     {0x2E, 0xF02E},     { 0x07, 0x22 }},  // Key location 6  ( % 5 )
    {{0x07, 0x87},     {0x36, 0xF036},     { 0x07, 0x23 }},  // Key location 7  ( ^ 6 )
    {{0x08, 0x88},     {0x3D, 0xF03D},     { 0x07, 0x24 }},  // Key location 8  ( & 7 )
    {{0x09, 0x89},     {0x3E, 0xF03E},     { 0x07, 0x25 }},  // Key location 9  ( * 8 )
    {{0x0A, 0x8A},     {0x46, 0xF046},     { 0x07, 0x26 }},  // Key location 10 ( ( 9 )
    {{0x0B, 0x8B},     {0x45, 0xF045},     { 0x07, 0x27 }},  // Key location 11 ( ) 0 )
    {{0x0C, 0x8C},     {0x4E, 0xF04E},     { 0x07, 0x2D }},  // Key location 12 ( _ - )
    {{0x0D, 0x8D},     {0x55, 0xF055},     { 0x07, 0x2E }},  // Key location 13 ( + = )
    {UNASSIGNED,       UNASSIGNED,         UNASSIGNED},      // Key location 14 ( NONE )
    {{0x0E, 0x8E},     {0x66, 0xF066},     { 0x07, 0x2A }},  // Key location 15 ( Backspace )
    {{0x0F, 0x8F},     {0x0D, 0xF00D},     { 0x07, 0x2B }},  // Key location 16 ( Tab )
    {{0x10, 0x90},     {0x15, 0xF015},     { 0x07, 0x14 }},  // Key location 17 ( Q )
    {{0x11, 0x91},     {0x1D, 0xF01D},     { 0x07, 0x1A }},  // Key location 18 ( W )
    {{0x12, 0x92},     {0x24, 0xF024},     { 0x07, 0x08 }},  // Key location 19 ( E )
    {{0x13, 0x93},     {0x2D, 0xF02D},     { 0x07, 0x15 }},  // Key location 20 ( R )
    {{0x14, 0x94},     {0x2C, 0xF02C},     { 0x07, 0x17 }},  // Key location 21 ( T )
    {{0x15, 0x95},     {0x35, 0xF035},     { 0x07, 0x1C }},  // Key location 22 ( Y )
    {{0x16, 0x96},     {0x3C, 0xF03C},     { 0x07, 0x18 }},  // Key location 23 ( U )
    {{0x17, 0x97},     {0x43, 0xF043},     { 0x07, 0x0C }},  // Key location 24 ( I )
    {{0x18, 0x98},     {0x44, 0xF044},     { 0x07, 0x12 }},  // Key location 25 ( O )
    {{0x19, 0x99},     {0x4D, 0xF04D},     { 0x07, 0x13 }},  // Key location 26 ( P )
    {{0x1A, 0x9A},     {0x54, 0xF054},     { 0x07, 0x2F }},  // Key location 27 ( { [ )
    {{0x1B, 0x9B},     {0x5B, 0xF05B},     { 0x07, 0x30 }},  // Key location 28 ( } ] )
    {{0x2B, 0xAB},     {0x5D, 0xF05D},     { 0x07, 0x31 }},  // Key location 29* ( | \ )
    {{0x3A, 0xBA},     {0x58, 0xF058},     { 0x07, 0x39 }},  // Key location 30 ( Caps Lock )
    {{0x1E, 0x9E},     {0x1C, 0xF01C},     { 0x07, 0x04 }},  // Key location 31 ( A )
    {{0x1F, 0x9F},     {0x1B, 0xF01B},     { 0x07, 0x16 }},  // Key location 32 ( S )
    {{0x20, 0xA0},     {0x23, 0xF023},     { 0x07, 0x07 }},  // Key location 33 ( D )
    {{0x21, 0xA1},     {0x2B, 0xF02B},     { 0x07, 0x09 }},  // Key location 34 ( F )
    {{0x22, 0xA2},     {0x34, 0xF034},     { 0x07, 0x0A }},  // Key location 35 ( G )
    {{0x23, 0xA3},     {0x33, 0xF033},     { 0x07, 0x0B }},  // Key location 36 ( H )
    {{0x24, 0xA4},     {0x3B, 0xF03B},     { 0x07, 0x0D }},  // Key location 37 ( J )
    {{0x25, 0xA5},     {0x42, 0xF042},     { 0x07, 0x0E }},  // Key location 38 ( K )
    {{0x26, 0xA6},     {0x4B, 0xF04B},     { 0x07, 0x0F }},  // Key location 39 ( L )
    {{0x27, 0xA7},     {0x4C, 0xF04C},     { 0x07, 0x33 }},  // Key location 40 ( : ; )
    {{0x28, 0xA8},     {0x52, 0xF052},     { 0x07, 0x34 }},  // Key location 41 ( “ ‘ )
    {{0x2B, 0xAB},     {0x5D, 0xF05D},     { 0x07, 0x32 }},  // Key location 42 ( NONE ) **
    {{0x1C, 0x9C},     {0x5A, 0xF05A},     { 0x07, 0x28 }},  // Key location 43 ( Enter )
    {{0x2A, 0xAA},     {0x12, 0xF012},     { 0x07, 0xE1 }},  // Key location 44 ( L SHIFT )
    {{0x56, 0xD6},     {0x61, 0xF061},     { 0x07, 0x64 }},  // Key location 45 ( NONE ) **
    {{0x2C, 0xAC},     {0x1A, 0xF01A},     { 0x07, 0x1D }},  // Key location 46 ( Z )
    {{0x2D, 0xAD},     {0x22, 0xF022},     { 0x07, 0x1B }},  // Key location 47 ( X )
    {{0x2E, 0xAE},     {0x21, 0xF021},     { 0x07, 0x06 }},  // Key location 48 ( C )
    {{0x2F, 0xAF},     {0x2A, 0xF02A},     { 0x07, 0x19 }},  // Key location 49 ( V )
    {{0x30, 0xB0},     {0x32, 0xF032},     { 0x07, 0x05 }},  // Key location 50 ( B )
    {{0x31, 0xB1},     {0x31, 0xF031},     { 0x07, 0x11 }},  // Key location 51 ( N )
    {{0x32, 0xB2},     {0x3A, 0xF03A},     { 0x07, 0x10 }},  // Key location 52 ( M )
    {{0x33, 0xB3},     {0x41, 0xF041},     { 0x07, 0x36 }},  // Key location 53 ( < , )
    {{0x34, 0xB4},     {0x49, 0xF049},     { 0x07, 0x37 }},  // Key location 54 ( > . )
    {{0x35, 0xB5},     {0x4A, 0xF04A},     { 0x07, 0x38 }},  // Key location 55 ( ? / )
    {{0x73, 0xF3},     {0x51, 0xF051},     { 0x07, 0x87 }},  // Key location 56 ( NONE ) ***
    {{0x36, 0xB6},     {0x59, 0xF059},     { 0x07, 0xE5 }},  // Key location 57 ( R SHIFT )
    {{0x1D, 0x9D},     {0x14, 0xF014},     { 0x07, 0xE0 }},  // Key location 58 ( L CTRL )
    {{0xE05B, 0xE0DB}, {0xE01F, 0xE0F01F}, { 0x07, 0xE3 }},  // Key location 59 ( L WIN )
    {{0x38, 0xB8},     {0x11, 0xF011},     { 0x07, 0xE2 }},  // Key location 60 ( L ALT )
    {{0x39, 0xB9},     {0x29, 0xF029},     { 0x07, 0x2C }},  // Key location 61 ( Space Bar )
    {{0xE038, 0xE0B8}, {0xE011, 0xE0F011}, { 0x07, 0xE6 }},  // Key location 62 ( R ALT )
    {{0xE05C, 0xE0DC}, {0xE027, 0xE0F027}, { 0x07, 0xE7 }},  // Key location 63 ( R WIN )
    {{0xE01D, 0xE09D}, {0xE014, 0xE0F014}, { 0x07, 0xE4 }},  // Key location 64 ( R CTRL )
    {{0xE05D, 0xE0DD}, {0xE02F, 0xE0F02F}, { 0x07, 0x65 }},  // Key location 65 ( APP )
    {UNASSIGNED,       UNASSIGNED,         UNASSIGNED},      // Key location 66 ( NONE )
    {UNASSIGNED,       UNASSIGNED,         UNASSIGNED},      // Key location 67 ( NONE )
    {UNASSIGNED,       UNASSIGNED,         UNASSIGNED},      // Key location 68 ( NONE )
    {UNASSIGNED,       UNASSIGNED,         UNASSIGNED},      // Key location 69 ( NONE )
    {UNASSIGNED,       UNASSIGNED,         UNASSIGNED},      // Key location 70 ( NONE )
    {UNASSIGNED,       UNASSIGNED,         UNASSIGNED},      // Key location 71 ( NONE )
    {UNASSIGNED,       UNASSIGNED,         UNASSIGNED},      // Key location 72 ( NONE )
    {UNASSIGNED,       UNASSIGNED,         UNASSIGNED},      // Key location 73 ( NONE )
    {UNASSIGNED,       UNASSIGNED,         UNASSIGNED},      // Key location 74 ( NONE )
    {{0xE052, 0xE0D2}, {0xE070, 0xE0F070}, { 0x07, 0x49 }},  // Key location 75 ( Insert )
    {{0xE053, 0xE0D3}, {0xE071, 0xE0F071}, { 0x07, 0x4C }},  // Key location 76 ( Delete )
    {UNASSIGNED,       UNASSIGNED,         UNASSIGNED},      // Key location 77 ( NONE )
    {UNASSIGNED,       UNASSIGNED,         UNASSIGNED},      // Key location 78 ( NONE )
    {{0xE04B, 0xE0CB}, {0xE06B, 0xE0F06B}, { 0x07, 0x50 }},  // Key location 79 ( Left Arrow )
    {{0xE047, 0xE0C7}, {0xE06C, 0xE0F06C}, { 0x07, 0x4A }},  // Key location 80 ( Home )
    {{0xE04F, 0xE0CF}, {0xE069, 0xE0F069}, { 0x07, 0x4D }},  // Key location 81 ( End )
    {UNASSIGNED,       UNASSIGNED,         UNASSIGNED},      // Key location 82 ( NONE )
    {{0xE048, 0xE0C8}, {0xE075, 0xE0F075}, { 0x07, 0x52 }},  // Key location 83 ( Up Arrow )
    {{0xE050, 0xE0D0}, {0xE072, 0xE0F072}, { 0x07, 0x51 }},  // Key location 84 ( Dn Arrow )
    {{0xE049, 0xE0C9}, {0xE07D, 0xE0F07D}, { 0x07, 0x4B }},  // Key location 85 ( Page Up )
    {{0xE051, 0xE0D1}, {0xE07A, 0xE0F07A}, { 0x07, 0x4C }},  // Key location 86 ( Page Down )
    {UNASSIGNED,       UNASSIGNED,         UNASSIGNED},      // Key location 87 ( NONE )
    {UNASSIGNED,       UNASSIGNED,         UNASSIGNED},      // Key location 88 ( NONE )
    {{0xE04D, 0xE0CD}, {0xE074, 0xE0F074}, { 0x07, 0x4F }},  // Key location 89 ( Right Arrow )
    {{0x45, 0xC5},     {0x77, 0xF077},     { 0x07, 0x53 }},  // Key location 90 ( Num Lock )
    {{0x47, 0xC7},     {0x6C, 0xF06C},     { 0x07, 0x5F }},  // Key location 91 ( Numeric 7 )
    {{0x4B, 0xCB},     {0x6B, 0xF06B},     { 0x07, 0x5C }},  // Key location 92 ( Numeric 4 )
    {{0x4F, 0xCF},     {0x69, 0xF069},     { 0x07, 0x59 }},  // Key location 93 ( Numeric 1 )
    {UNASSIGNED,       UNASSIGNED,         UNASSIGNED},      // Key location 94 ( NONE )
    {{0xE035, 0xE0B5}, {0xE04A, 0xE0F04A}, { 0x07, 0x54 }},  // Key location 95 ( Numeric / )
    {{0x48, 0xC8},     {0x75, 0xF075},     { 0x07, 0x60 }},  // Key location 96 ( Numeric 8 )
    {{0x4C, 0xCC},     {0x73, 0xF073},     { 0x07, 0x5D }},  // Key location 97 ( Numeric 5 )
    {{0x50, 0xD0},     {0x72, 0xF072},     { 0x07, 0x5A }},  // Key location 98 ( Numeric 2 )
    {{0x52, 0xD2},     {0x70, 0xF070},     { 0x07, 0x62 }},  // Key location 99 ( Numeric 0 )
    {{0x37, 0xB7},     {0x7C, 0xF07C},     { 0x07, 0x55 }},  // Key location 100 ( Numeric * )
    {{0x49, 0xC9},     {0x7D, 0xF07D},     { 0x07, 0x61 }},  // Key location 101 ( Numeric 9 )
    {{0x4D, 0xCD},     {0x74, 0xF074},     { 0x07, 0x5E }},  // Key location 102 ( Numeric 6 )
    {{0x51, 0xD1},     {0x7A, 0xF07A},     { 0x07, 0x5B }},  // Key location 103 ( Numeric 3 )
    {{0x53, 0xD3},     {0x71, 0xF071},     { 0x07, 0x63 }},  // Key location 104 ( Numeric . )
    {{0x4A, 0xCA},     {0x7B, 0xF07B},     { 0x07, 0x56 }},  // Key location 105 ( Numeric - )
    {{0x4E, 0xCE},     {0x79, 0xF079},     { 0x07, 0x57 }},  // Key location 106 ( Numeric + )
    {{0x7E, 0xFE},     {0x6D, 0xF06D},     { 0x07, 0x85 }},  // Key location 107 ( NONE ) ***
    {{0xE01C, 0xE09C}, {0xE05A, 0xE0F05A}, { 0x07, 0x58 }},  // Key location 108 ( Numeric Enter )
    {UNASSIGNED,       UNASSIGNED,         UNASSIGNED},      // Key location 109 ( NONE )
    {{0x01, 0x81},     {0x76, 0xF076},     { 0x07, 0x29 }},  // Key location 110 ( Esc )
    {UNASSIGNED,       UNASSIGNED,         UNASSIGNED},      // Key location 111 ( NONE )
    {{0x3B, 0xBB},     {0x05, 0xF005},     { 0x07, 0x3A }},  // Key location 112 ( F1 )
    {{0x3C, 0xBC},     {0x06, 0xF006},     { 0x07, 0x3B }},  // Key location 113 ( F2 )
    {{0x3D, 0xBD},     {0x04, 0xF005},     { 0x07, 0x3C }},  // Key location 114 ( F3 )
    {{0x3E, 0xBE},     {0x0C, 0xF00C},     { 0x07, 0x3D }},  // Key location 115 ( F4 )
    {{0x3F, 0xBF},     {0x03, 0xF003},     { 0x07, 0x3E }},  // Key location 116 ( F5 )
    {{0x40, 0xC0},     {0x0B, 0xF00B},     { 0x07, 0x3F }},  // Key location 117 ( F6 )
    {{0x41, 0xC1},     {0x83, 0xF083},     { 0x07, 0x40 }},  // Key location 118 ( F7 )
    {{0x42, 0xC2},     {0x0A, 0xF00A},     { 0x07, 0x41 }},  // Key location 119 ( F8 )
    {{0x43, 0xC3},     {0x01, 0xF001},     { 0x07, 0x42 }},  // Key location 120 ( F9 )
    {{0x44, 0xC4},     {0x09, 0xF009},     { 0x07, 0x43 }},  // Key location 121 ( F10 )
    {{0x57, 0xD7},     {0x78, 0xF078},     { 0x07, 0x44 }},  // Key location 122 ( F11 )
    {{0x58, 0xD8},     {0x07, 0xF007},     { 0x07, 0x45 }},  // Key location 123 ( F12 )
    {{0xE037, 0xE0B7}, {0xE07C, 0xE0F07C}, { 0x07, 0x46 }},  // Key location 124 ( Print Screen )
    {{0x46, 0xC6},     {0x7E, 0xF07E},     { 0x07, 0x47 }},  // Key location 125 ( Scroll Lock )
    {{0xE11D45, 0xE19DC5}, {0xE11477E1, 0xF014F077}, { 0x07, 0x48 }},  // Key location 126 ( Pause )
};



//SoftKeyboard Class implementation
CSoftKeyboard::CSoftKeyboard()
/*++
Routine Description:
    Constructor for CSoftKeyboard

Synchronization: None
 
Arguments:
    None

Return Value:
    None
    
--*/
{
    InitMemberVariables();

} // CSoftKeyboard::CSoftKeyboard


CSoftKeyboard::~CSoftKeyboard()
/*++
Routine Description:
    Destructor for CSoftKeyboard

Synchronization: None
 
Arguments:
    None

Return Value:
    None
    
--*/
{
    // Stop internal thread
    (void)End();

    // Release the softtranslator interface
    RELEASE(m_piSoftKbdTranslator);
    // Release IMalloc interface
    RELEASE(m_pMalloc);
    
    //Initialize the member variables 
    InitMemberVariables();

} // CSoftKeyboard::~CSoftKeyboard


void CSoftKeyboard::InitMemberVariables()
/*++
Routine Description:
    Initialize member variables of the class CSoftKeyboard

Synchronization: 
    None
 
Arguments:
    None
    
Return Value:
    None
--*/
{
    m_pMalloc = NULL;
    m_State = IDLE;
    m_DelayTimer = 0;
    m_RepeatTimer = 0;
    m_KeyPressedMsg = 0;
    m_KeyReleasedMsg = 0;
    m_LastKeyPressed = KEY_NONE;

    m_usHeadAnchor = 0;
    m_usTailAnchor = 0;
    ZeroMemory(&m_KbdBuffer, sizeof(m_KbdBuffer));

    m_piSoftKbdTranslator = NULL;
    ZeroMemory(&m_KbdAttributes, sizeof(m_KbdAttributes));
    ZeroMemory(&m_KbdIndicatorTrans, sizeof(m_KbdIndicatorTrans));
    ZeroMemory(&m_KbdTypematicParam, sizeof(m_KbdTypematicParam));
    ZeroMemory(&m_KbdIndicatorParam, sizeof(m_KbdIndicatorParam));

} // CSoftKeyboard::InitMemberVariables


STDMETHODIMP CSoftKeyboard::get_RepeatDelay(ULONG *pulDelay)
/*++
Routine Description:
    This property returns a keyboard repeat delay.

Synchronization: 
    None
 
Arguments:
    pulDelay - caller allocated space to hold the property value.
    
Return Value:
    E_POINTER
        Invalid pointer for output parameter
    From called function
    
--*/
{
    HRESULT hr = S_OK;

    // Check if we have valid pointer to returned parameter
    IfFalseHrGo(NULL != pulDelay, E_POINTER);
    *pulDelay = 0;

    *pulDelay = m_KbdTypematicParam.ulDelay;
    
Exit:
    return hr;

} // CSoftKeyboard::get_RepeatDelay


STDMETHODIMP CSoftKeyboard::get_RepeatRate(ULONG *pulRate)
/*++
Routine Description:
    This property returns a keyboard repeat rate.

Synchronization: 
    None
 
Arguments:
    pulRate - caller allocated space to hold the property value.
    
Return Value:
    E_POINTER
        Invalid pointer for output parameter
    From called function
    
--*/
{
    HRESULT hr = S_OK;

    // Check if we have valid pointer to returned parameter
    IfFalseHrGo(NULL != pulRate, E_POINTER);
    *pulRate = m_KbdTypematicParam.ulRate;

Exit:
    return hr;

} // CSoftKeyboard::get_RepeatRate


STDMETHODIMP CSoftKeyboard::get_IndicatorStatus(
                               INDICATOR_KEY LedKey,
                               VARIANT_BOOL *pfvarStatus)
/*++
Routine Description:
    This property returns the status of a LED indicator.

Synchronization: 
    None
 
Arguments:
    sLedKey - LedKey for which the status is being asked for.
    pfvarStatus - caller allocated space to hold the property value.
    
Return Value:
    E_POINTER
        Invalid pointer for output parameter
    From called function
    
--*/
{
    HRESULT hr = S_OK;
    VARIANT_BOOL fvarStatus = FALSE;

    // Check if we have valid pointer to returned parameter
    IfFalseHrGo(NULL != pfvarStatus, E_POINTER);
    *pfvarStatus = FALSE;

    fvarStatus = ((m_KbdIndicatorParam.usLedFlags & LedKey) == LedKey);
    
    *pfvarStatus = fvarStatus;
    
Exit:
    return hr;

} // CSoftKeyboard::get_IndicatorStatus


STDMETHODIMP CSoftKeyboard::get_BufferSize(
                                SHORT *psNumberOfEvents)
/*++
Routine Description:
    Implement the get BufferSize propery for the interface
    ISoftKeyboard.

Synchronization: 
    None
 
Arguments:
    psNumberOfEvents - caller allocated space to hold the 
                        property value.
    
Return Value:
    E_POINTER
        Invalid pointer for output parameter
    From called function
    
--*/
{
    HRESULT hr = S_OK;

    // Check if we have valid pointer to returned parameter
    IfFalseHrGo(NULL != psNumberOfEvents, E_POINTER);
    *psNumberOfEvents = (SHORT)cusKeyboardBufferSize;

Exit:
    return hr;

} // CSoftKeyboard::get_BufferSize


STDMETHODIMP CSoftKeyboard::get_Translator(
                SoftKbdTranslator **ppTranslator)
/*++
Routine Description:
    Implement the get Translator propery for the interface
    ISoftKeyboard.

Synchronization: 
    None
 
Arguments:
    ppTranslator - caller allocated space to hold the property value.
    
Return Value:
    E_POINTER
        Invalid pointer for output parameter
    From called function
  
--*/
{
    HRESULT hr = S_OK;

    // Check if we have valid pointer to returned parameter
    IfFalseHrGo(NULL != ppTranslator, E_POINTER);
    *ppTranslator = NULL;

    ADDREF(m_piSoftKbdTranslator);
    *ppTranslator  = reinterpret_cast<SoftKbdTranslator *>(m_piSoftKbdTranslator);

    IfFailHrGo(hr);

Exit:
    return hr;

} // CSoftKeyboard::get_Translator


STDMETHODIMP CSoftKeyboard::put_Translator(
                SoftKbdTranslator *pTranslator)
/*++
Routine Description:
    Implement the put Translator propery for the interface
    ISoftKeyboard.

Synchronization: 
    None
 
Arguments:
    pTranslator - new value of the property.
    
Return Value:
    From called function
  
--*/
{
    HRESULT hr = S_OK;
    ISoftKbdTranslator *piNewInterface = NULL;
    ISoftKbdTranslator *piOldInterface = m_piSoftKbdTranslator;
    
    
    if (NULL != pTranslator)
    {
        IfFailHrGo(reinterpret_cast<IUnknown *>(pTranslator)->QueryInterface(__uuidof(ISoftKbdTranslator),
                                                                           reinterpret_cast<void **>(&piNewInterface)));
    }

    (void)InterlockedExchangePointer((void**)m_piSoftKbdTranslator, piNewInterface);
    RELEASE(piOldInterface);

Exit:
    return hr;

} // CSoftKeyboard::put_Translator


STDMETHODIMP CSoftKeyboard::putref_Translator(
                SoftKbdTranslator *pTranslator)
{
    return (put_Translator(pTranslator));

} // CSoftKeyboard::putref_Translator


STDMETHODIMP CSoftKeyboard::HitKeyOnce(KBD_KEYS KeyCode)
/*++
Routine Description:
    Implement the HitKeyOnce method for the interface
    ISoftKeyboard. This simulate a key being hit.

Synchronization: 
    None
 
Arguments:
    KeyCode - keycode of "pressed" key.
    
Return Value:
    E_FAIL
        Could not queue request for processing.
    E_INVALIDARG
        Returned if supplied keycode is out of supported range of keys.
    S_OK
        Success.
  
--*/
{
    HRESULT hr = S_OK;

    // Check for faulty input
    IfFalseHrGo(KeyCode > KEY_NONE && KeyCode <= KEY_PAUSE, E_INVALIDARG);

    IfFailHrGo(PostMessage2Task(m_KeyPressedMsg, KeyCode, 0));
    IfFailHrGo(PostMessage2Task(m_KeyReleasedMsg, KeyCode, 0));

Exit:
    return hr;

} // CSoftKeyboard::HitKeyOnce


STDMETHODIMP CSoftKeyboard::PressKey(KBD_KEYS KeyCode)
/*++
Routine Description:
    Implement the PressKey method for the interface
    ISoftKeyboard. This simulate a key being pressed.

Synchronization: 
    None
 
Arguments:
    KeyCode - keycode of "pressed" key.
    
Return Value:
    E_FAIL
        Could not queue request for processing.
    E_INVALIDARG
        Returned if supplied keycode is out of supported range of keys.
    S_OK
        Success.
  
--*/{
    HRESULT hr = S_OK;

    // Check for faulty input
    IfFalseHrGo(KeyCode > KEY_NONE && KeyCode <= KEY_PAUSE, E_INVALIDARG);

    IfFailHrGo(PostMessage2Task(m_KeyPressedMsg, KeyCode, 0));

Exit:
    return hr;

} // CSoftKeyboard::PressKey


STDMETHODIMP CSoftKeyboard::ReleaseKey(KBD_KEYS KeyCode)
/*++
Routine Description:
    Implement the ReleaseKey method for the interface
    ISoftKeyboard. This simulate a key being released.

Synchronization: 
    None
 
Arguments:
    KeyCode - keycode of "released" key.
    
Return Value:
    E_FAIL
        Could not queue request for processing.
    E_INVALIDARG
        Returned if supplied keycode is out of supported range of keys.
    S_OK
        Success.
  
--*/
{
    HRESULT hr = S_OK;

    // Check for faulty input
    IfFalseHrGo(KeyCode > KEY_NONE && KeyCode <= KEY_PAUSE, E_INVALIDARG);

    IfFailHrGo(PostMessage2Task(m_KeyReleasedMsg, KeyCode, 0));

Exit:
    return hr;

} // CSoftKeyboard::ReleaseKey


STDMETHODIMP CSoftKeyboard::PressAndReleaseKeys(
                BSTR bstrKeyStrokes)

/*++
Routine Description:
    Implement the PressAndRelease method for the interface
    ISoftKeyboard. This simulate a key stroke for a UNICODE character

Synchronization: 
    None
 
Arguments:
    bstrKeyStrokes - Keystrokes to be simulated
    
Return Value:
    E_FAIL 
        Could not generate a sequence of keycodes to simulate a keystroke.
    E_INVALIDARG
        Invalid keystroke length.
    S_OK
        Success.
--*/
{
    HRESULT hr = S_OK;
    UINT nUnits = 0;
    KBD_KEYCODE* pKeyCodes = NULL;

    UINT nSize = SysStringLen(bstrKeyStrokes);

    // Check if string is not NULL
    IfFalseHrGo(nSize != 0, E_INVALIDARG);

    // Check is Translator property is set
    IfFalseHrGo(NULL != m_piSoftKbdTranslator, E_UNEXPECTED);

    hr = m_piSoftKbdTranslator->TranslateString2Keycodes(nSize, bstrKeyStrokes, &nUnits, &pKeyCodes);
    IfFailHrGo(hr);

    // Go through returned sequence and queue it for processing
    for(UINT i = 0; i < nUnits; i++)
    {
        // Call PressKey method
        if (KEY_MAKE == pKeyCodes[i].ucFlag) {
            IfFailHrGo(PressKey(pKeyCodes[i].KeyCode));
        }
        // Call ReleaseKey method
        else if (KEY_BREAK == pKeyCodes[i].ucFlag) {
            IfFailHrGo(ReleaseKey(pKeyCodes[i].KeyCode));
        }
        // Call HitKeyOnce method
        else if ((KEY_BREAK | KEY_MAKE) == pKeyCodes[i].ucFlag) {
            IfFailHrGo(HitKeyOnce(pKeyCodes[i].KeyCode));
        }
        // Should never hit this code, but check anyway
        else {
            IfFailHrGo(E_FAIL);
        }
    }

Exit:
    m_pMalloc->Free(pKeyCodes);
    return hr;

} // CSoftKeyboard::PressAndReleaseKeys

STDMETHODIMP CSoftKeyboard::get_Attributes(
                KEYBOARD_ATTRIBUTES **ppAttributes)
/*++
Routine Description:
    Implement the get attribute property for the interface
    ISoftInputKbdDevice.

Synchronization: 
    None
 
Arguments:
    ppAttributes - caller gets allocated buffer with the property.
                   Caller must release.
    
Return Value:
    E_POINTER
        Invalid pointer for output parameter
    E_OUTOFMEMORY
        Could not allocate memory for KEYBOARD_ATTRIBUTES structure.
    S_OK otherwise
--*/
{
    HRESULT hr = S_OK;

    IfFalseHrGo(NULL != ppAttributes, E_POINTER);
    *ppAttributes = NULL;

    // Allocate memory for the returned structure
    *ppAttributes = (KEYBOARD_ATTRIBUTES*)m_pMalloc->Alloc(sizeof(m_KbdAttributes));
    if (NULL == *ppAttributes)
    {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    CopyMemory(*ppAttributes, &m_KbdAttributes, sizeof(m_KbdAttributes));

Exit:
    return hr;

} // CSoftKeyboard::get_Attributes


STDMETHODIMP CSoftKeyboard::put_Attributes(
                    KEYBOARD_ATTRIBUTES *pAttributes)
/*++
Routine Description:
    Implement the put attribute property for the interface
    ISoftInputKbdDevice.

Synchronization: 
    None
 
Arguments:
    pAttributes - New value of the property.
    
Return Value:
    E_POINTER 
        if the input is not valid
    S_OK otherwise

--*/
{
    HRESULT hr = S_OK;

    IfFalseHrGo(NULL != pAttributes, E_POINTER);

    CopyMemory(&m_KbdAttributes, pAttributes, sizeof(m_KbdAttributes));

Exit:
    return hr;

} // CSoftKeyboard::put_Attributes
    

STDMETHODIMP CSoftKeyboard::get_IndicatorTranslation(
           KEYBOARD_INDICATOR_TRANSLATION **ppIndicatorTrans)
/*++
Routine Description:
    Implement the get Indicator translation property for the interface
    ISoftInputKbdDevice.

Synchronization: 
    None
 
Arguments:
    ppIndicatorTrans - caller gets allocated buffer with the property.
                       Caller must release.
    
Return Value:
    E_POINTER
        Invalid pointer for output parameter
    E_OUTOFMEMORY
        Could not allocate memory for KEYBOARD_INDICATOR_TRANSLATION structure.
    S_OK otherwise
    
--*/
{
    HRESULT hr = S_OK;
   
    IfFalseHrGo(NULL != ppIndicatorTrans, E_POINTER);
    *ppIndicatorTrans = NULL;

    // Allocate memory for the returned structure
    *ppIndicatorTrans = 
        (KEYBOARD_INDICATOR_TRANSLATION*)m_pMalloc->Alloc(sizeof(m_KbdIndicatorTrans));
    if (NULL == *ppIndicatorTrans)
    {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    CopyMemory(*ppIndicatorTrans, &m_KbdIndicatorTrans, sizeof(m_KbdIndicatorTrans));

Exit:
    return hr;

} // CSoftKeyboard::get_IndicatorTranslation


STDMETHODIMP CSoftKeyboard::put_IndicatorTranslation(
               KEYBOARD_INDICATOR_TRANSLATION *pIndicatorTrans)
/*++
Routine Description:
    Implement the put IndicatorTranslations property for the interface
    ISoftInputKbdDevice.

Synchronization: 
    None
 
Arguments:
    pIndicatorTrans - New value of the property.
    
Return Value:
    E_POINTER 
        if the input is not valid
    S_OK otherwise

--*/
{
    HRESULT hr = S_OK;

    IfFalseHrGo(NULL != pIndicatorTrans, E_POINTER);

    CopyMemory(&m_KbdIndicatorTrans, pIndicatorTrans, sizeof(m_KbdIndicatorTrans));

Exit:
    return hr;

} // CSoftKeyboard::put_IndicatorTranslation


STDMETHODIMP CSoftKeyboard::get_Typematic(
                 KEYBOARD_TYPEMATIC_PARAMETERS **ppTypematic)
/*++
Routine Description:
    Implement the get Typematic propery for the interface
    ISoftInputKbdDevice.

Synchronization: 
    None
 
Arguments:
    ppTypematic - caller gets allocated buffer with the property.
                  Caller must release.
    
Return Value:
    E_POINTER
        Invalid pointer for output parameter
    E_OUTOFMEMORY
        Could not allocate memory for KEYBOARD_TYPEMATIC_PARAMETERS structure.
    S_OK otherwise
    
--*/
{
    HRESULT hr = S_OK;
   
    IfFalseHrGo(NULL != ppTypematic, E_POINTER);
    *ppTypematic = NULL;

    // Allocate memory for the returned structure
    *ppTypematic = 
        (KEYBOARD_TYPEMATIC_PARAMETERS*)m_pMalloc->Alloc(sizeof(m_KbdTypematicParam));
    if (NULL == *ppTypematic)
    {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    CopyMemory(*ppTypematic, &m_KbdTypematicParam, sizeof(m_KbdTypematicParam));

Exit:
    return hr;

} // CSoftKeyboard::get_Typematic


STDMETHODIMP CSoftKeyboard::put_Typematic(
                    KEYBOARD_TYPEMATIC_PARAMETERS *pTypematic)
/*++
Routine Description:
    Implement the put Typematic property for the interface
    ISoftInputKbdDevice.

Synchronization: 
    None
 
Arguments:
    pTypematic - New value of the property.
    
Return Value:
    E_POINTER 
        if the input is not valid
    S_OK otherwise

--*/
{
    HRESULT hr = S_OK;

    IfFalseHrGo(NULL != pTypematic, E_POINTER);

    // Sanity check of new parameters
    pTypematic->ulRate = 
        max(pTypematic->ulRate, KEYBOARD_TYPEMATIC_RATE_MINIMUM);
    pTypematic->ulRate =
        min(pTypematic->ulRate, KEYBOARD_TYPEMATIC_RATE_MAXIMUM);

    if (pTypematic->ulDelay != KEYBOARD_TYPEMATIC_DELAY_INFINITE)
    {
        pTypematic->ulDelay = 
            max(pTypematic->ulDelay, KEYBOARD_TYPEMATIC_DELAY_MINIMUM);
        pTypematic->ulDelay =
            min(pTypematic->ulDelay, KEYBOARD_TYPEMATIC_DELAY_MAXIMUM);
    }

    CopyMemory(&m_KbdTypematicParam, pTypematic, sizeof(m_KbdTypematicParam));

Exit:
    return hr;

} // CSoftKeyboard::put_Typematic
STDMETHODIMP CSoftKeyboard::get_Indicators(
                KEYBOARD_INDICATOR_PARAMETERS **ppIndicator)
/*++
Routine Description:
    Implement the get indicators property for the interface
    ISoftInputKbdDevice.

Synchronization: 
    None
 
Arguments:
    ppIndicators - caller gets allocated buffer with the property.
                   Caller must release.
    
Return Value:
    E_POINTER
        Invalid pointer for output parameter
    E_OUTOFMEMORY
        Could not allocate memory for KEYBOARD_INDICATOR_PARAMETERS structure.
    S_OK otherwise
    
--*/
{
    HRESULT hr = S_OK;

    IfFalseHrGo(NULL != ppIndicator, E_POINTER);
    *ppIndicator = NULL;

    // Allocate memory for the returned structure
    *ppIndicator = 
        (KEYBOARD_INDICATOR_PARAMETERS*)m_pMalloc->Alloc(sizeof(m_KbdIndicatorParam));
    if (NULL == *ppIndicator)
    {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    CopyMemory(*ppIndicator, &m_KbdIndicatorParam, sizeof(m_KbdIndicatorParam));

Exit:
    return hr;

} // CSoftKeyboard::get_Indicators

STDMETHODIMP CSoftKeyboard::put_Indicators(
                    KEYBOARD_INDICATOR_PARAMETERS *pIndicator)
/*++
Routine Description:
    Implement the put Indicators property for the interface
    ISoftInputKbdDevice.

Synchronization: 
    None
 
Arguments:
    pIndicator - New value of the property.
    
Return Value:
    E_POINTER 
        if the input is not valid
    S_OK otherwise

--*/
{
    HRESULT hr = S_OK;

    IfFalseHrGo(NULL != pIndicator, E_POINTER);

    DetectIndicatorStatusChange(pIndicator->usLedFlags, IND_NUM);
    DetectIndicatorStatusChange(pIndicator->usLedFlags, IND_CAPS);
    DetectIndicatorStatusChange(pIndicator->usLedFlags, IND_SCROLL);

    CopyMemory(&m_KbdIndicatorParam, pIndicator, sizeof(m_KbdIndicatorParam));

Exit:
    return hr;

} // CSoftKeyboard::put_Indicators


STDMETHODIMP CSoftKeyboard::Read(USHORT *pUnits,
                KEYBOARD_INPUT_DATA **ppKbdData)
/*++
Routine Description:
    Implement the read method for the interface
    ISoftInputKbdDevice. Transfers Keyboard input data
    to the lower layers.
    When simulated key is pressed or released, a KEYBOARD_INPUT_DATA structure 
    is put into keyboard buffer and IDSFKbdDeviceEvents::KeyTouched event 
    is fired, indicating that Read() method can be called.

Synchronization: 
    None
 
Arguments:
    pUnits - number of KEYBOARD_INPUT_DATA structures there are in 
             the array
    ppKbdData - Array of KEYBOARD_INPUT_DATA.
         
Return Value:
    E_POINTER
        Invalid pointer for output parameter
    E_OUTOFMEMORY
        Could not allocate memory for KEYBOARD_INPUT_DATA structures.
    E_FAIL
        Internal error prevents proceeding.
    S_OK
        Success
--*/
{
    HRESULT hr = S_OK;
    USHORT usUnits = 0;
    PVOID pKbdData = NULL;

    // This method is called from msg pump thread of 
    // the SoftHidDevice so we must protect the buffer
    m_KbdBufferLock.Lock();
 
    // Check if we have valid pointer to returned parameter
    IfFalseHrGo(NULL != pUnits, E_POINTER);
    IfFalseHrGo(NULL != ppKbdData, E_POINTER);
    *pUnits = 0;
    *ppKbdData = NULL;

    // All data in the buffer is stored consequently
    if (m_usTailAnchor > m_usHeadAnchor)
    {
        UINT_PTR cbSize = 0;

        usUnits = m_usTailAnchor - m_usHeadAnchor;
        // Calculate size of memory to allocate
        cbSize = sizeof(KEYBOARD_INPUT_DATA) * usUnits;
        
        // Allocate memory
        pKbdData = m_pMalloc->Alloc(cbSize);
        IfFalseHrGo(NULL != pKbdData, E_OUTOFMEMORY);

        // Copy data from keyboard buffer
        CopyMemory(pKbdData, &m_KbdBuffer[m_usHeadAnchor], cbSize);
    }
    // Data is stored in two chunks due to wrap around
    else if (m_usTailAnchor < m_usHeadAnchor)
    {
        UINT_PTR cbSize1 = 0;
        UINT_PTR cbSize2 = 0;

        usUnits = cusKeyboardBufferSize - m_usHeadAnchor + m_usTailAnchor;
        // Calculate size of chunk 1
        cbSize1 = sizeof(KEYBOARD_INPUT_DATA) * (cusKeyboardBufferSize - m_usHeadAnchor);

        // Calculate size of chunk 2
        cbSize2 = sizeof(KEYBOARD_INPUT_DATA) * m_usTailAnchor;
        
        // Check buffer boundaries for integer uderflow or overflow
        if ((cbSize1 + cbSize2) > (cusKeyboardBufferSize * sizeof(KEYBOARD_INPUT_DATA)) ||
            (cbSize1 + cbSize2) < cbSize1)
        {
            hr = E_FAIL;
            goto Exit;
        }

        // Allocate memory
        pKbdData = m_pMalloc->Alloc(cbSize1 + cbSize2);
        IfFalseHrGo(NULL != pKbdData, E_OUTOFMEMORY);

        // Copy data chunks from keyboard buffer
        CopyMemory(pKbdData, &m_KbdBuffer[m_usHeadAnchor], cbSize1);
        //buffer has wrapped around need to copy from the start of the buffer.
        CopyMemory((PUCHAR)pKbdData + cbSize1, &m_KbdBuffer[0], cbSize2);
    }
    
    // Buffer is empty now
    m_usHeadAnchor = m_usTailAnchor;

Exit:

    m_KbdBufferLock.Unlock();

    *pUnits = usUnits;
    *ppKbdData = (KEYBOARD_INPUT_DATA*)pKbdData;
    
    return hr;

} // CSoftKeyboard::Read

void CSoftKeyboard::DetectIndicatorStatusChange(
                    USHORT usNewFlags, INDICATOR_KEY LedKey)
/*++
Routine Description:
    Detects whether status of indicator key has changed and fires appropriate event.

Synchronization: 
    None
 
Arguments:
    usNewFlags - New value of indicator flags.
    LedKey - indicator key.
    
Return Value:
    None

--*/
{
    USHORT usOldVal = m_KbdIndicatorParam.usLedFlags & (USHORT)LedKey;
    USHORT usNewVal = usNewFlags & (USHORT)LedKey;

    // Detect indicator status change
    if (usOldVal != usNewVal)
    {
        if (usNewVal)
            (void)Fire_IndicatorOn(LedKey);
        else
            (void)Fire_IndicatorOff(LedKey);
        
    }

    return;

} // CSoftKeyboard::DetectIndicatorStatusChange

void CSoftKeyboard::OnMsgReceived(
            CMessageTask::TASK_MSG msg, WPARAM wParam, LPARAM lParam)
/*++
Routine Description:
    Message handler routine. Manages states and state transitions 
    of internal state machine. 

Synchronization: 
    None
 
Arguments:
    msg - message received by the task
    wParam - 1-st message parameter
    lParam - 2-nd message parameter
    
Return Value:
    None
  
--*/
{
    UNREFERENCED_PARAMETER(lParam);

    // KeyPressed: Same for all states
    if (msg == m_KeyPressedMsg)
    {
        // Goto MAKECODE state
        SetState(MAKECODE);
        m_LastKeyPressed = (KBD_KEYS)wParam;
        PutMessageToBuffer((KBD_KEYS)wParam, KEY_MAKE);
    }
    // KeyReleased: Same for all states
    else if (msg == m_KeyReleasedMsg)
    {
        // Goto IDLE state
        SetState(IDLE);
        PutMessageToBuffer((KBD_KEYS)wParam, KEY_BREAK);
    }
    // TIMER messages
    else if (msg == WM_TIMER)
    {
        // DELAY message (generated by Delay timer)
        if (wParam == m_DelayTimer)
        {
            switch(m_State)
            {
            case MAKECODE:
                // Goto WAIT state
                SetState(WAIT);
                break;
            case IDLE:
            case WAIT:
            case AUTOREPEAT:
                // Path not defined
                _ASSERT(FALSE);
                break;
            }
        }
        // REPEAT message (generated by Repeat timer)
        else if (wParam == m_RepeatTimer)
        {
            switch(m_State)
            {
            case IDLE:
            case MAKECODE:
                // Path not defined
                _ASSERT(FALSE);
                break;
            case WAIT:
                // Delay expired, goto AUTOREPEAT
                SetState(AUTOREPEAT);
                break;
            case AUTOREPEAT:
                // Stays in the same state
                PutMessageToBuffer(m_LastKeyPressed, KEY_MAKE);
                break;
            }
        }
    }
    // Quit message
    else if (IsQuitMessage(msg))
    {
        // Last message, stop timers
        (void)KillTimer(m_DelayTimer);
        (void)KillTimer(m_RepeatTimer);
    }
    // Unknown message
    else
    {
        _ASSERT(FALSE);
    }

    return;

} // CSoftKeyboard::OnMsgReceived



void CSoftKeyboard::SetState(
                INTERNAL_STATE state)
/*++
Routine Description:
    Sets new state to the state machine.

Synchronization: 
    None
 
Arguments:
    state - new state
    
Return Value:
    None
  
--*/
{
    m_State = state;

    switch(m_State)
    {
    case IDLE:
        // Cancel old timers
        (void)KillTimer(m_DelayTimer);
        (void)KillTimer(m_RepeatTimer);
        break;
    case MAKECODE:
        // Cancel old timers
        (void)KillTimer(m_DelayTimer);
        (void)KillTimer(m_RepeatTimer);
        // Schedule DELAY message
        (void)SetTimer(m_DelayTimer, m_KbdTypematicParam.ulDelay);
        break;
    case WAIT:
        // Cancel Delay timer
        (void)KillTimer(m_DelayTimer);
        // Schedule REPEAT message
        (void)SetTimer(m_RepeatTimer, 1000 / m_KbdTypematicParam.ulRate);
        break;
    case AUTOREPEAT:
        // No state transition
        break;
    }

    return;

} // CSoftKeyboard::SetState


BOOLEAN CSoftKeyboard::PutMessageToBuffer(
                        KBD_KEYS KeyCode, USHORT usFlags)
/*++
Routine Description:
    Writes input data generated by key press/release into circular buffer.
    m_usHeadAnchor refers to the buffer index from which valid data starts.
    m_usTailAnchor refers to the buffer index at which valid data ends + 1.

    Possible cases:

    1) 
      0   1   2   3   4   5   6   7   8  
    +---+---+---+---+---+---+---+---+---+
    |   |   |   |   |   |   |   |   |   |
    +---+---+---+---+---+---+---+---+---+
      ^  
      Head,Tail
    Next available cell is 0. (Buffer is empty)

    2.a)
      0   1   2   3   4   5   6   7   8  
    +---+---+---+---+---+---+---+---+---+
    | x | x | x | x |   |   |   |   |   |
    +---+---+---+---+---+---+---+---+---+
      ^               ^
      Head            Tail
    Next available cell is 4.

    2.b)
      0   1   2   3   4   5   6   7   8  
    +---+---+---+---+---+---+---+---+---+
    |   |   |   |   |   | x | x | x | x |
    +---+---+---+---+---+---+---+---+---+
      ^                    ^
      Tail                 Head
    Next available cell is 0.


    3)
      0   1   2   3   4   5   6   7   8 
    +---+---+---+---+---+---+---+---+---+
    | x | x |   |   |   |   |   | x | x |
    +---+---+---+---+---+---+---+---+---+
              ^                   ^      
              Tail                Head
    Next available cell is 2.


    4.a)
      0   1   2   3   4   5   6   7   8
    +---+---+---+---+---+---+---+---+---+
    | x | x | x | x | x | x |   | x | x |
    +---+---+---+---+---+---+---+---+---+
                              ^    ^
                              Tail Head
    4.b)
      0   1   2   3   4   5   6   7   8
    +---+---+---+---+---+---+---+---+---+
    | x | x | x | x | x | x | x | x |   |
    +---+---+---+---+---+---+---+---+---+
      ^                               ^
      Head                            Tail
    Buffer is full. (Tail and Head are not equal!)

Synchronization: 
    None
 
Arguments:
    KeyCode - specifies the key code.
    usFlags - specifies a key press/release event.
    
Return Value:
    TRUE on success,
    FALSE on failure (buffer overflow)
  
--*/
{
    BOOLEAN fBufferOverflow = FALSE;
    USHORT usNextIndex = m_usTailAnchor;

    m_KbdBufferLock.Lock();

    // Check if there is enough space in keyboard buffer
    usNextIndex = ++usNextIndex % cusKeyboardBufferSize;
    if (usNextIndex != m_usHeadAnchor)
    {
        KEYBOARD_INPUT_DATA& rKbdData = m_KbdBuffer[m_usTailAnchor];
        rKbdData.usFlags = usFlags;
        // Translate keycode into scancode.
        if (FAILED(TranslateKeyCode2ScanCode(&rKbdData.ulMakeCode, KeyCode, usFlags)))
        {
            // Just ignore faulty KeyCode and do not report the error.
            // This situation is unlikely to happen because KeyCodes are
            // checked in KeyPressed, KeyReleased and KeyHitOnce routines.
            m_KbdBufferLock.Unlock();
            return TRUE;
        }

        // Move Tail
        m_usTailAnchor = usNextIndex;
    }
    else
        fBufferOverflow = TRUE;

    m_KbdBufferLock.Unlock();
 
    // Fire events
    if (fBufferOverflow)
    {
        (void)Fire_BufferOverflow();
    }
    else
    {
        (void)Fire_KeyTouched();   
    }

    return !fBufferOverflow;

} // CSoftKeyboard::PutMessageToBuffer

HRESULT CSoftKeyboard::TranslateKeyCode2ScanCode(
                    ULONG* pulMakeCode, KBD_KEYS KeyCode, USHORT usFlags)
/*++
Routine Description:
    Based on current keyboard attributes this function translates keycode 
    into Scan Code Set1 or Scan Code Set2 or HID Usage ID.

Synchronization: 
    None
 
Arguments:
    pulMakeCode - translated scancode
    KeyCode - original keycode
    usFlags - specifies whether MAKE or BREAK scancode should be returned
    
Return Value:
    E_POINTER
        Invalid pointer for output parameter
    E_FAIL
        On failure
    S_OK
        Success

--*/
{
    HRESULT hr = S_OK;

    // Check if we have valid pointer to returned parameter
    IfFalseHrGo(NULL != pulMakeCode, E_POINTER);
    *pulMakeCode = 0;

    // Check for faulty input
    IfFalseHrGo((KeyCode > KEY_NONE && KeyCode <= KEY_PAUSE), E_FAIL);

    // Convert keycode into scancode
    switch(m_KbdAttributes.usKeyboardMode)
    {
    case KEYBOARD_SCAN_CODE_SET1:
        if (KEY_MAKE == usFlags)
            *pulMakeCode = m_KeyCode2ScanCodeTable[KeyCode].Set1.ulMake;
        else
            *pulMakeCode = m_KeyCode2ScanCodeTable[KeyCode].Set1.ulBreak;
        break;

    case KEYBOARD_SCAN_CODE_SET2:
        if (KEY_MAKE == usFlags)
            *pulMakeCode = m_KeyCode2ScanCodeTable[KeyCode].Set2.ulMake;
        else
            *pulMakeCode = m_KeyCode2ScanCodeTable[KeyCode].Set2.ulBreak;
        break;

    case KEYBOARD_HID_USAGE:
            *pulMakeCode = m_KeyCode2ScanCodeTable[KeyCode].HID.usUsageID;
        break;
    }

Exit:
    return hr;

} // CSoftKeyboard::TranslateKeyCode2ScanCode


HRESULT CSoftKeyboard::FinalConstruct()
/*++
Routine Description:
    Overwrites the default FinalConstruct. Creates the default
    properties, gets pointer to COM task memory allocator.

Synchronization: 
    None
 
Arguments:
    None
    
Return Value:
    From called function
--*/
{
    HRESULT hr = S_OK;
    ISoftKbdTranslator* piDefaultASCIIXlator = NULL;

    // Acquire COM memory management interface
    IfFailHrGo(CoGetMalloc(1, &m_pMalloc));

    // Register notification messages
    IfFailHrGo(RegisterMessage(MSG_KEYPRESSED, &m_KeyPressedMsg));
    IfFailHrGo(RegisterMessage(MSG_KEYRELEASED, &m_KeyReleasedMsg));

    // Construct default translator object
    //pcDefaultASCIIXlator = new CDefaultASCIIXlator();
    hr = CDefaultASCIIXlator::CreateInstance(&piDefaultASCIIXlator);
    IfFailHrGo(hr);

    m_piSoftKbdTranslator = piDefaultASCIIXlator;

    // Set default values to properties
    m_KbdAttributes.usKeyboardMode = KEYBOARD_SCAN_CODE_SET2;

    m_KbdTypematicParam.ulRate = KEYBOARD_TYPEMATIC_RATE_DEFAULT;
    m_KbdTypematicParam.ulDelay = KEYBOARD_TYPEMATIC_DELAY_DEFAULT;
    
    // Start internal thread
    IfFailHrGo(Start(60000));

Exit:
    return hr;

} // CSoftKeyboard::FinalConstruct

