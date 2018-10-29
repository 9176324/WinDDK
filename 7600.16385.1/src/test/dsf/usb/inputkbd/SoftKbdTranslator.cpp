/*++ BUILD Version: 0000    

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    SoftKbdTranslator

Abstract:
    Implements the class CDefaultASCIIXlator
Revision History:

--*/

#include "stdafx.h"
#include "SoftKbdTranslator.h"
#include "Common.h"

// CDefaultASCIIXlator

// SubTable that defines keys that have to be pressed in order to
// generate ASCII symbols in lower register. Keys that can't be
// generated from ASCII characters are marked as UNUASSIGNED.
CDefaultASCIIXlator::KBD_KEYCODESEQ1 CDefaultASCIIXlator::m_KeyCodeLen1SubTable[127] = 
{
    UNASSIGNED1,                    // Not used     0
    ASSIGN_CODE1(KEY_BACK_QUOTE),   // Key location 1
    ASSIGN_CODE1(KEY_1),            // Key location 2
    ASSIGN_CODE1(KEY_2),            // Key location 3
    ASSIGN_CODE1(KEY_3),            // Key location 4
    ASSIGN_CODE1(KEY_4),            // Key location 5
    ASSIGN_CODE1(KEY_5),            // Key location 6
    ASSIGN_CODE1(KEY_6),            // Key location 7
    ASSIGN_CODE1(KEY_7),            // Key location 8
    ASSIGN_CODE1(KEY_8),            // Key location 9
    ASSIGN_CODE1(KEY_9),            // Key location 10
    ASSIGN_CODE1(KEY_0),            // Key location 11
    ASSIGN_CODE1(KEY_MINUS),        // Key location 12
    ASSIGN_CODE1(KEY_EQUAL),        // Key location 13
    UNASSIGNED1,                    // Key location 14
    ASSIGN_CODE1(KEY_BCKSP),        // Key location 15
    ASSIGN_CODE1(KEY_TAB),          // Key location 16
    ASSIGN_CODE1(KEY_Q),            // Key location 17
    ASSIGN_CODE1(KEY_W),            // Key location 18
    ASSIGN_CODE1(KEY_E),            // Key location 19
    ASSIGN_CODE1(KEY_R),            // Key location 20
    ASSIGN_CODE1(KEY_T),            // Key location 21
    ASSIGN_CODE1(KEY_Y),            // Key location 22
    ASSIGN_CODE1(KEY_U),            // Key location 23
    ASSIGN_CODE1(KEY_I),            // Key location 24
    ASSIGN_CODE1(KEY_O),            // Key location 25
    ASSIGN_CODE1(KEY_P),            // Key location 26
    ASSIGN_CODE1(KEY_L_SQUARE),     // Key location 27
    ASSIGN_CODE1(KEY_R_SQUARE),     // Key location 28
    ASSIGN_CODE1(KEY_BACK_SLASH),   // Key location 29
    ASSIGN_CODE1(KEY_CAPS),         // Key location 30
    ASSIGN_CODE1(KEY_A),            // Key location 31
    ASSIGN_CODE1(KEY_S),            // Key location 32
    ASSIGN_CODE1(KEY_D),            // Key location 33
    ASSIGN_CODE1(KEY_F),            // Key location 34
    ASSIGN_CODE1(KEY_G),            // Key location 35
    ASSIGN_CODE1(KEY_H),            // Key location 36
    ASSIGN_CODE1(KEY_J),            // Key location 37
    ASSIGN_CODE1(KEY_K),            // Key location 38
    ASSIGN_CODE1(KEY_L),            // Key location 39
    ASSIGN_CODE1(KEY_SEMICOLON),    // Key location 40
    ASSIGN_CODE1(KEY_QUOTE),        // Key location 41
    UNASSIGNED1,                    // Key location 42
    ASSIGN_CODE1(KEY_ENTER),        // Key location 43
    ASSIGN_CODE1(KEY_L_SHIFT),      // Key location 44
    UNASSIGNED1,                    // Key location 45
    ASSIGN_CODE1(KEY_Z),            // Key location 46
    ASSIGN_CODE1(KEY_X),            // Key location 47
    ASSIGN_CODE1(KEY_C),            // Key location 48
    ASSIGN_CODE1(KEY_V),            // Key location 49
    ASSIGN_CODE1(KEY_B),            // Key location 50
    ASSIGN_CODE1(KEY_N),            // Key location 51
    ASSIGN_CODE1(KEY_M),            // Key location 52
    ASSIGN_CODE1(KEY_COMMA),        // Key location 53
    ASSIGN_CODE1(KEY_DOT),          // Key location 54
    ASSIGN_CODE1(KEY_SLASH),        // Key location 55
    UNASSIGNED1,                    // Key location 56
    ASSIGN_CODE1(KEY_R_SHIFT),      // Key location 57 (KEY_R_SHIFT)
    ASSIGN_CODE1(KEY_L_CTRL),       // Key location 58 (KEY_L_CTRL)
    ASSIGN_CODE1(KEY_L_WIN),        // Key location 59 (KEY_L_WIN)
    ASSIGN_CODE1(KEY_L_ALT),        // Key location 60 (KEY_L_ALT)
    ASSIGN_CODE1(KEY_SPACE),        // Key location 61 (KEY_SPACE)
    ASSIGN_CODE1(KEY_R_ALT),        // Key location 62 (KEY_R_ALT)
    ASSIGN_CODE1(KEY_R_WIN),        // Key location 63 (KEY_R_WIN)
    ASSIGN_CODE1(KEY_R_CTRL),       // Key location 64 (KEY_R_CTRL)
    UNASSIGNED1,                    // Key location 65 (KEY_APPS)
    UNASSIGNED1,                    // Key location 66
    UNASSIGNED1,                    // Key location 67
    UNASSIGNED1,                    // Key location 68
    UNASSIGNED1,                    // Key location 69
    UNASSIGNED1,                    // Key location 70
    UNASSIGNED1,                    // Key location 71
    UNASSIGNED1,                    // Key location 72
    UNASSIGNED1,                    // Key location 73
    UNASSIGNED1,                    // Key location 74
    ASSIGN_CODE1(KEY_INSERT),       // Key location 75
    ASSIGN_CODE1(KEY_DELETE),       // Key location 76
    UNASSIGNED1,                    // Key location 77
    UNASSIGNED1,                    // Key location 78
    ASSIGN_CODE1(KEY_LEFT),         // Key location 79
    ASSIGN_CODE1(KEY_HOME),         // Key location 80
    ASSIGN_CODE1(KEY_END),          // Key location 81
    UNASSIGNED1,                    // Key location 82
    ASSIGN_CODE1(KEY_UP ),          // Key location 83
    ASSIGN_CODE1(KEY_DOWN),         // Key location 84
    ASSIGN_CODE1(KEY_PGUP),         // Key location 85
    ASSIGN_CODE1(KEY_PGDOWN),       // Key location 86
    UNASSIGNED1,                    // Key location 87
    UNASSIGNED1,                    // Key location 88
    ASSIGN_CODE1(KEY_RIGHT),        // Key location 89
    ASSIGN_CODE1(KEY_NUM),          // Key location 90
    ASSIGN_CODE1(KEY_NUM_7),        // Key location 91
    ASSIGN_CODE1(KEY_NUM_4),        // Key location 92
    ASSIGN_CODE1(KEY_NUM_1),        // Key location 93
    UNASSIGNED1,                    // Key location 94
    ASSIGN_CODE1(KEY_NUM_SLASH),    // Key location 95
    ASSIGN_CODE1(KEY_NUM_8),        // Key location 96
    ASSIGN_CODE1(KEY_NUM_5),        // Key location 97
    ASSIGN_CODE1(KEY_NUM_2),        // Key location 98
    ASSIGN_CODE1(KEY_NUM_INS),      // Key location 99
    ASSIGN_CODE1(KEY_NUM_ASTERISK), // Key location 100
    ASSIGN_CODE1(KEY_NUM_9),        // Key location 101
    ASSIGN_CODE1(KEY_NUM_6),        // Key location 102
    ASSIGN_CODE1(KEY_NUM_3),        // Key location 103
    ASSIGN_CODE1(KEY_NUM_DEL),      // Key location 104
    ASSIGN_CODE1(KEY_NUM_MINUS),    // Key location 105
    ASSIGN_CODE1(KEY_NUM_PLUS),     // Key location 106
    UNASSIGNED1,                    // Key location 107
    ASSIGN_CODE1(KEY_NUM_ENTER),    // Key location 108
    UNASSIGNED1,                    // Key location 109
    ASSIGN_CODE1(KEY_ESC),          // Key location 110
    UNASSIGNED1,                    // Key location 111
    ASSIGN_CODE1(KEY_F1 ),          // Key location 112
    ASSIGN_CODE1(KEY_F2 ),          // Key location 113
    ASSIGN_CODE1(KEY_F3 ),          // Key location 114
    ASSIGN_CODE1(KEY_F4 ),          // Key location 115
    ASSIGN_CODE1(KEY_F5 ),          // Key location 116
    ASSIGN_CODE1(KEY_F6 ),          // Key location 117
    ASSIGN_CODE1(KEY_F7 ),          // Key location 118
    ASSIGN_CODE1(KEY_F8 ),          // Key location 119
    ASSIGN_CODE1(KEY_F9 ),          // Key location 120
    ASSIGN_CODE1(KEY_F10),          // Key location 121
    ASSIGN_CODE1(KEY_F11),          // Key location 122
    ASSIGN_CODE1(KEY_F12),          // Key location 123
    ASSIGN_CODE1(KEY_PRTSCR),       // Key location 124
    ASSIGN_CODE1(KEY_SCROLL),       // Key location 125
    ASSIGN_CODE1(KEY_PAUSE),        // Key location 126
};

// SubTable that defines sequence of keys that have to be pressed in order to
// generate ASCII symbols in upper register. Keys that do not have upper register
// meaning are marked as UNASSIGNED.
CDefaultASCIIXlator::KBD_KEYCODESEQ3 CDefaultASCIIXlator::m_KeyCodeLen3SubTable[127] = 
{
    UNASSIGNED3,                    // Not used     0
    ASSIGN_CODE3(KEY_BACK_QUOTE),   // Key location 1
    ASSIGN_CODE3(KEY_1),            // Key location 2
    ASSIGN_CODE3(KEY_2),            // Key location 3
    ASSIGN_CODE3(KEY_3),            // Key location 4
    ASSIGN_CODE3(KEY_4),            // Key location 5
    ASSIGN_CODE3(KEY_5),            // Key location 6
    ASSIGN_CODE3(KEY_6),            // Key location 7
    ASSIGN_CODE3(KEY_7),            // Key location 8
    ASSIGN_CODE3(KEY_8),            // Key location 9
    ASSIGN_CODE3(KEY_9),            // Key location 10
    ASSIGN_CODE3(KEY_0),            // Key location 11
    ASSIGN_CODE3(KEY_MINUS),        // Key location 12
    ASSIGN_CODE3(KEY_EQUAL),        // Key location 13
    UNASSIGNED3,                    // Key location 14
    UNASSIGNED3,                    // Key location 15 (KEY_BCKSP)
    UNASSIGNED3,                    // Key location 16 (KEY_TAB)
    ASSIGN_CODE3(KEY_Q),            // Key location 17
    ASSIGN_CODE3(KEY_W),            // Key location 18
    ASSIGN_CODE3(KEY_E),            // Key location 19
    ASSIGN_CODE3(KEY_R),            // Key location 20
    ASSIGN_CODE3(KEY_T),            // Key location 21
    ASSIGN_CODE3(KEY_Y),            // Key location 22
    ASSIGN_CODE3(KEY_U),            // Key location 23
    ASSIGN_CODE3(KEY_I),            // Key location 24
    ASSIGN_CODE3(KEY_O),            // Key location 25
    ASSIGN_CODE3(KEY_P),            // Key location 26
    ASSIGN_CODE3(KEY_L_SQUARE),     // Key location 27
    ASSIGN_CODE3(KEY_R_SQUARE),     // Key location 28
    ASSIGN_CODE3(KEY_BACK_SLASH),   // Key location 29
    UNASSIGNED3,                    // Key location 30 (KEY_CAPS)
    ASSIGN_CODE3(KEY_A),            // Key location 31
    ASSIGN_CODE3(KEY_S),            // Key location 32
    ASSIGN_CODE3(KEY_D),            // Key location 33
    ASSIGN_CODE3(KEY_F),            // Key location 34
    ASSIGN_CODE3(KEY_G),            // Key location 35
    ASSIGN_CODE3(KEY_H),            // Key location 36
    ASSIGN_CODE3(KEY_J),            // Key location 37
    ASSIGN_CODE3(KEY_K),            // Key location 38
    ASSIGN_CODE3(KEY_L),            // Key location 39
    ASSIGN_CODE3(KEY_SEMICOLON),    // Key location 40
    ASSIGN_CODE3(KEY_QUOTE),        // Key location 41
    UNASSIGNED3,                    // Key location 42
    ASSIGN_CODE3(KEY_ENTER),        // Key location 43 (KEY_ENTER)
    ASSIGN_CODE3(KEY_L_SHIFT),      // Key location 44 (KEY_L_SHIFT)
    UNASSIGNED3,                    // Key location 45
    ASSIGN_CODE3(KEY_Z),            // Key location 46
    ASSIGN_CODE3(KEY_X),            // Key location 47
    ASSIGN_CODE3(KEY_C),            // Key location 48
    ASSIGN_CODE3(KEY_V),            // Key location 49
    ASSIGN_CODE3(KEY_B),            // Key location 50
    ASSIGN_CODE3(KEY_N),            // Key location 51
    ASSIGN_CODE3(KEY_M),            // Key location 52
    ASSIGN_CODE3(KEY_COMMA),        // Key location 53
    ASSIGN_CODE3(KEY_DOT),          // Key location 54
    ASSIGN_CODE3(KEY_SLASH),        // Key location 55
    UNASSIGNED3,                    // Key location 56
    ASSIGN_CODE3(KEY_R_SHIFT),      // Key location 57 (KEY_R_SHIFT)
    ASSIGN_CODE3(KEY_L_CTRL),       // Key location 58 (KEY_L_CTRL)
    ASSIGN_CODE3(KEY_L_WIN),        // Key location 59 (KEY_L_WIN)
    ASSIGN_CODE3(KEY_L_ALT),        // Key location 60 (KEY_L_ALT)
    UNASSIGNED3,                    // Key location 61 (KEY_SPACE)
    ASSIGN_CODE3(KEY_R_ALT),        // Key location 62 (KEY_R_ALT)
    ASSIGN_CODE3(KEY_R_WIN),        // Key location 63 (KEY_R_WIN)
    ASSIGN_CODE3(KEY_R_CTRL),       // Key location 64 (KEY_R_CTRL)
    UNASSIGNED3,                    // Key location 65 (KEY_APPS)
    UNASSIGNED3,                    // Key location 66
    UNASSIGNED3,                    // Key location 67
    UNASSIGNED3,                    // Key location 68
    UNASSIGNED3,                    // Key location 69
    UNASSIGNED3,                    // Key location 70
    UNASSIGNED3,                    // Key location 71
    UNASSIGNED3,                    // Key location 72
    UNASSIGNED3,                    // Key location 73
    UNASSIGNED3,                    // Key location 74
    UNASSIGNED3,                    // Key location 75 (KEY_INSERT)
    UNASSIGNED3,                    // Key location 76 (KEY_DELETE)
    UNASSIGNED3,                    // Key location 77
    UNASSIGNED3,                    // Key location 78
    UNASSIGNED3,                    // Key location 79 (KEY_LEFT)
    UNASSIGNED3,                    // Key location 80 (KEY_HOME)
    UNASSIGNED3,                    // Key location 81 (KEY_END)
    UNASSIGNED3,                    // Key location 82
    UNASSIGNED3,                    // Key location 83 (KEY_UP)
    UNASSIGNED3,                    // Key location 84 (KEY_DOWN)
    UNASSIGNED3,                    // Key location 85 (KEY_PGUP)
    UNASSIGNED3,                    // Key location 86 (KEY_PGDOWN)
    UNASSIGNED3,                    // Key location 87
    UNASSIGNED3,                    // Key location 88
    UNASSIGNED3,                    // Key location 89 (KEY_RIGHT)
    ASSIGN_CODE3(KEY_NUM),          // Key location 90
    ASSIGN_CODE3(KEY_NUM_7),        // Key location 91
    ASSIGN_CODE3(KEY_NUM_4),        // Key location 92
    ASSIGN_CODE3(KEY_NUM_1),        // Key location 93
    UNASSIGNED3,                    // Key location 94
    UNASSIGNED3,                    // Key location 95 (KEY_NUM_SLASH)
    UNASSIGNED3,                    // Key location 96 (KEY_NUM_8)
    UNASSIGNED3,                    // Key location 97 (KEY_NUM_5)
    UNASSIGNED3,                    // Key location 98 (KEY_NUM_2)
    UNASSIGNED3,                    // Key location 99 (KEY_NUM_INS)
    UNASSIGNED3,                    // Key location 100 (KEY_NUM_ASTERISK)
    UNASSIGNED3,                    // Key location 101 (KEY_NUM_9)
    UNASSIGNED3,                    // Key location 102 (KEY_NUM_6)
    UNASSIGNED3,                    // Key location 103 (KEY_NUM_3)
    UNASSIGNED3,                    // Key location 104 (KEY_NUM_DEL)
    UNASSIGNED3,                    // Key location 105 (KEY_NUM_MINUS)
    UNASSIGNED3,                    // Key location 106 (KEY_NUM_PLUS)
    UNASSIGNED3,                    // Key location 107 (KEY_ENTER)
    UNASSIGNED3,                    // Key location 108 (KEY_NUM_ENTER)
    UNASSIGNED3,                    // Key location 109
    UNASSIGNED3,                    // Key location 110 (KEY_ESC)
    UNASSIGNED3,                    // Key location 111
    UNASSIGNED3,                    // Key location 112 (KEY_F1)
    UNASSIGNED3,                    // Key location 113 (KEY_F2)
    UNASSIGNED3,                    // Key location 114 (KEY_F3)
    UNASSIGNED3,                    // Key location 115 (KEY_F4)
    UNASSIGNED3,                    // Key location 116 (KEY_F5)
    UNASSIGNED3,                    // Key location 117 (KEY_F6)
    UNASSIGNED3,                    // Key location 118 (KEY_F7)
    UNASSIGNED3,                    // Key location 119 (KEY_F8)
    UNASSIGNED3,                    // Key location 120 (KEY_F9)
    UNASSIGNED3,                    // Key location 121 (KEY_F10)
    UNASSIGNED3,                    // Key location 122 (KEY_F11)
    UNASSIGNED3,                    // Key location 123 (KEY_F12)
    UNASSIGNED3,                    // Key location 124 (KEY_PRTSCR)
    UNASSIGNED3,                    // Key location 125 (KEY_SCROLL)
    UNASSIGNED3,                    // Key location 126 (KEY_PAUSE)
};

// Table that maps ASCII symbols to key sequences.
KBD_KEYCODE* CDefaultASCIIXlator::m_Ascii2KeyCodeTable[128] =
{
    SET_KEY         ( KEY_NONE ),       //    0x00
    SET_KEY         ( KEY_NONE ),       // ?  0x01
    SET_KEY         ( KEY_NONE ),       // ?  0x02
    SET_KEY         ( KEY_NONE ),       // ?  0x03
    SET_KEY         ( KEY_NONE ),       // ?  0x04
    SET_KEY         ( KEY_NONE ),       // ?  0x05
    SET_KEY         ( KEY_NONE ),       // ?  0x06
    SET_KEY         ( KEY_NONE ),       //    0x07
    SET_KEY         ( KEY_BCKSP ),      //    0x08
    SET_KEY         ( KEY_TAB ),        //    0x09
    SET_KEY         ( KEY_NONE ),       //    0x0a
    SET_KEY         ( KEY_NONE ),       // ?  0x0b
    SET_KEY         ( KEY_NONE ),       // ?  0x0c
    SET_KEY         ( KEY_ENTER ),      //    0x0d
    SET_KEY         ( KEY_NONE ),       // ?  0x0e
    SET_KEY         ( KEY_NONE ),       // ¤  0x0f
    SET_KEY         ( KEY_NONE ),       // ?  0x10
    SET_KEY         ( KEY_NONE ),       // ?  0x11
    SET_KEY         ( KEY_NONE ),       // ?  0x12
    SET_KEY         ( KEY_NONE ),       // ?  0x13
    SET_KEY         ( KEY_NONE ),       // ¶  0x14
    SET_KEY         ( KEY_NONE ),       // §  0x15
    SET_KEY         ( KEY_NONE ),       // ?  0x16
    SET_KEY         ( KEY_NONE ),       // ?  0x17
    SET_KEY         ( KEY_NONE ),       // ?  0x18
    SET_KEY         ( KEY_NONE ),       // ?  0x19
    SET_KEY         ( KEY_NONE ),       // ?  0x1a
    SET_KEY         ( KEY_ESC ),        // ?  0x1b
    SET_KEY         ( KEY_NONE ),       // ?  0x1c
    SET_KEY         ( KEY_NONE ),       // ?  0x1d
    SET_KEY         ( KEY_NONE ),       // ?  0x1e
    SET_KEY         ( KEY_NONE ),       // ?  0x1f
    SET_KEY         ( KEY_SPACE ),      //    0x20
    SET_SHIFT_KEY   ( KEY_1 ),          // !  0x21
    SET_SHIFT_KEY   ( KEY_QUOTE ),      // "  0x22
    SET_SHIFT_KEY   ( KEY_3 ),          // #  0x23
    SET_SHIFT_KEY   ( KEY_4 ),          // $  0x24
    SET_SHIFT_KEY   ( KEY_5 ),          // %  0x25
    SET_SHIFT_KEY   ( KEY_7 ),          // &  0x26
    SET_KEY         ( KEY_QUOTE ),      // '  0x27
    SET_SHIFT_KEY   ( KEY_9 ),          // (  0x28
    SET_SHIFT_KEY   ( KEY_0 ),          // )  0x29
    SET_SHIFT_KEY   ( KEY_8 ),          // *  0x2a
    SET_SHIFT_KEY   ( KEY_EQUAL ),      // +  0x2b
    SET_KEY         ( KEY_COMMA ),      // ,  0x2c
    SET_KEY         ( KEY_MINUS ),      // -  0x2d
    SET_KEY         ( KEY_DOT ),        // .  0x2e
    SET_KEY         ( KEY_SLASH ),      // /  0x2f
    SET_KEY         ( KEY_0 ),          // 0  0x30
    SET_KEY         ( KEY_1 ),          // 1  0x31
    SET_KEY         ( KEY_2 ),          // 2  0x32
    SET_KEY         ( KEY_3 ),          // 3  0x33
    SET_KEY         ( KEY_4 ),          // 4  0x34
    SET_KEY         ( KEY_5 ),          // 5  0x35
    SET_KEY         ( KEY_6 ),          // 6  0x36
    SET_KEY         ( KEY_7 ),          // 7  0x37
    SET_KEY         ( KEY_8 ),          // 8  0x38
    SET_KEY         ( KEY_9 ),          // 9  0x39
    SET_SHIFT_KEY   ( KEY_SEMICOLON ),  // :  0x3a
    SET_KEY         ( KEY_SEMICOLON ),  // ;  0x3b
    SET_SHIFT_KEY   ( KEY_COMMA ),      // <  0x3c
    SET_KEY         ( KEY_EQUAL ),      // =  0x3d
    SET_SHIFT_KEY   ( KEY_DOT ),        // >  0x3e
    SET_SHIFT_KEY   ( KEY_SLASH ),      // ?  0x3f
    SET_SHIFT_KEY   ( KEY_2 ),          // @  0x40
    SET_SHIFT_KEY   ( KEY_A ),          // A  0x41
    SET_SHIFT_KEY   ( KEY_B ),          // B  0x42
    SET_SHIFT_KEY   ( KEY_C ),          // C  0x43
    SET_SHIFT_KEY   ( KEY_D ),          // D  0x44
    SET_SHIFT_KEY   ( KEY_E ),          // E  0x45
    SET_SHIFT_KEY   ( KEY_F ),          // F  0x46
    SET_SHIFT_KEY   ( KEY_G ),          // G  0x47
    SET_SHIFT_KEY   ( KEY_H ),          // H  0x48
    SET_SHIFT_KEY   ( KEY_I ),          // I  0x49
    SET_SHIFT_KEY   ( KEY_J ),          // J  0x4a
    SET_SHIFT_KEY   ( KEY_K ),          // K  0x4b
    SET_SHIFT_KEY   ( KEY_L ),          // L  0x4c
    SET_SHIFT_KEY   ( KEY_M ),          // M  0x4d
    SET_SHIFT_KEY   ( KEY_N ),          // N  0x4e
    SET_SHIFT_KEY   ( KEY_O ),          // O  0x4f
    SET_SHIFT_KEY   ( KEY_P ),          // P  0x50
    SET_SHIFT_KEY   ( KEY_Q ),          // Q  0x51
    SET_SHIFT_KEY   ( KEY_R ),          // R  0x52
    SET_SHIFT_KEY   ( KEY_S ),          // S  0x53
    SET_SHIFT_KEY   ( KEY_T ),          // T  0x54
    SET_SHIFT_KEY   ( KEY_U ),          // U  0x55
    SET_SHIFT_KEY   ( KEY_V ),          // V  0x56
    SET_SHIFT_KEY   ( KEY_W ),          // W  0x57
    SET_SHIFT_KEY   ( KEY_X ),          // X  0x58
    SET_SHIFT_KEY   ( KEY_Y ),          // Y  0x59
    SET_SHIFT_KEY   ( KEY_Z ),          // Z  0x5a
    SET_KEY         ( KEY_L_SQUARE ),   // [  0x5b
    SET_KEY         ( KEY_BACK_SLASH ), // \  0x5c
    SET_KEY         ( KEY_R_SQUARE),    // ]  0x5d
    SET_KEY         ( KEY_6 ),          // ^  0x5e
    SET_SHIFT_KEY   ( KEY_MINUS ),      // _  0x5f
    SET_KEY         ( KEY_BACK_QUOTE ), // `  0x60
    SET_KEY         ( KEY_A ),          // a  0x61
    SET_KEY         ( KEY_B ),          // b  0x62
    SET_KEY         ( KEY_C ),          // c  0x63
    SET_KEY         ( KEY_D ),          // d  0x64
    SET_KEY         ( KEY_E ),          // e  0x65
    SET_KEY         ( KEY_F ),          // f  0x66
    SET_KEY         ( KEY_G ),          // g  0x67
    SET_KEY         ( KEY_H ),          // h  0x68
    SET_KEY         ( KEY_I ),          // i  0x69
    SET_KEY         ( KEY_J ),          // j  0x6a
    SET_KEY         ( KEY_K ),          // k  0x6b
    SET_KEY         ( KEY_L ),          // l  0x6c
    SET_KEY         ( KEY_M ),          // m  0x6d
    SET_KEY         ( KEY_N ),          // n  0x6e
    SET_KEY         ( KEY_O ),          // o  0x6f
    SET_KEY         ( KEY_P ),          // p  0x70
    SET_KEY         ( KEY_Q ),          // q  0x71
    SET_KEY         ( KEY_R ),          // r  0x72
    SET_KEY         ( KEY_S ),          // s  0x73
    SET_KEY         ( KEY_T ),          // t  0x74
    SET_KEY         ( KEY_U ),          // u  0x75
    SET_KEY         ( KEY_V ),          // v  0x76
    SET_KEY         ( KEY_W ),          // w  0x77
    SET_KEY         ( KEY_X ),          // x  0x78
    SET_KEY         ( KEY_Y ),          // y  0x79
    SET_KEY         ( KEY_Z ),          // z  0x7a
    SET_SHIFT_KEY   ( KEY_L_SQUARE ),   // {  0x7b
    SET_SHIFT_KEY   ( KEY_BACK_SLASH ), // |  0x7c
    SET_SHIFT_KEY   ( KEY_R_SQUARE ),   // }  0x7d
    SET_SHIFT_KEY   ( KEY_BACK_QUOTE ), // ~  0x7e
    SET_KEY         ( KEY_NUM_DEL )     // ¦  0x7f
};


//SoftKbdTranslator Class implementation
CDefaultASCIIXlator::CDefaultASCIIXlator()
/*++
Routine Description:
    Constructor for CDefaultASCIIXlator

Synchronization: None
 
Arguments:
    None

Return Value:
    None
    
--*/
{
    InitMemberVariables();

} // CDefaultASCIIXlator::CDefaultASCIIXlator


CDefaultASCIIXlator::~CDefaultASCIIXlator()
/*++
Routine Description:
    Destructor for CDefaultASCIIXlator

Synchronization: None
 
Arguments:
    None

Return Value:
    None
    
--*/
{
    // Release IMalloc interface
    RELEASE(m_pMalloc);

    //Initialize the member variables 
    InitMemberVariables();

} // CDefaultASCIIXlator::~CDefaultASCIIXlator


void CDefaultASCIIXlator::InitMemberVariables()
/*++
Routine Description:
    Initialize member variables of the class CDefaultASCIIXlator

Synchronization: 
    None
 
Arguments:
    None
Return Value:
    None
--*/
{
    m_ulLCID = 0;
    m_pMalloc = NULL;

} // CDefaultASCIIXlator::InitMemberVariables


HRESULT CDefaultASCIIXlator::FinalConstruct()
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

    hr = CoGetMalloc(1, &m_pMalloc);
    IfFailHrGo(hr);

Exit:
    return hr;

} // CSoftKeyboard::FinalConstruct


STDMETHODIMP CDefaultASCIIXlator::get_Language(ULONG *pulLCID)
/*++
Routine Description:
    Implements the get_Language property for the interface
    ISoftKbdTranslator

Synchronization: 
    None
 
Arguments:
    oulLCID - caller allocated space to hold the LCID value
    
Return Value:
    E_POINTER
        Invalid pointer to output parameter
    S_OK
        Success
--*/
{
    HRESULT hr = S_OK;

    // Check if we have valid pointer to returned parameter
    IfFalseHrGo(NULL != pulLCID, E_POINTER);
    *pulLCID = m_ulLCID;
    
Exit:
    return hr;

} // CDefaultASCIIXlator::get_Language
    
        
STDMETHODIMP CDefaultASCIIXlator::TranslateString2Keycodes(
              __in UINT nSize,
              __in_ecount(nSize) WCHAR *szStrW,
              __out UINT *pnUnits,
              __deref_out_ecount(*pnUnits) KBD_KEYCODE **ppKeyCodes)
/*++
Routine Description:
    Implements the TranslateString2KeyCodes method for the interface
    ISoftKbdTranslator. The method translates a Unicode string in the 
    sequence of keycodes required to generate the character

Synchronization: 
    None
 
Arguments:
    szStrW - unicode string to be translated
    psUnits - Number of keycodes in the array ppKeyCodes
    ppKeyCodes - array which keycode which make translate to   
                 the character. Note the caller must release.
    
Return Value:

    E_OUTOFMEMORY
        Could not allocate memory for array of KBD_KEYCODE structures.
    E_POINTER
        Invalid pointer to output parameters
    E_INVALIDARG
        Invalid keystroke length.
    E_FAIL
        Could not convert source Unicode string into sequence of keycodes.
    S_OK
        Success.

--*/
{
    HRESULT hr = S_OK;
    UINT nUnits = 0;   
    KBD_KEYCODE* pKeyCodes = NULL;
    char* szStrA = NULL;

    // Check if we have valid pointer to returned parameter
    IfFalseHrGo(NULL != pnUnits, E_POINTER);
    IfFalseHrGo(NULL != ppKeyCodes, E_POINTER);
    *pnUnits = 0;
    *ppKeyCodes = 0;

    // Check input string length. 
    // Do not allow strings longer than 255 characters
    IfFalseHrGo(nSize > 0 && nSize < UCHAR_MAX, E_INVALIDARG);

    // Convert Unicode string to ANSI string
    szStrA = new char[nSize+1];
    IfFalseHrGo(NULL != szStrA, E_OUTOFMEMORY);
    IfFalseHrGo(-1 != wcstombs(szStrA, szStrW, nSize+1), E_FAIL);

    // Allocate memory for keycode sequence. Assume the worst case when each
    // character needs a sequence of 3 keys.
    pKeyCodes = (KBD_KEYCODE*)m_pMalloc->Alloc(nSize*sizeof(KBD_KEYCODE)*3);
    IfFalseHrGo(NULL != pKeyCodes, E_OUTOFMEMORY);

    // Convert input string into keycode sequence
    for(UINT i = 0; i < nSize; i++)
    {
        char c = szStrA[i];

        // Check if ASCII character is from range 0 - 127
        if (c >= 0 && c <= 127)
        {
            KBD_KEYCODE* pSeq = m_Ascii2KeyCodeTable[c];
            // Copy keycode sequence
            for(int j = 0; pSeq[j].KeyCode != KEY_NONE; j++)
            {
                pKeyCodes[nUnits++] = pSeq[j];
            }
        }
    }

Exit:
    delete[] szStrA;
    *ppKeyCodes = pKeyCodes;
    *pnUnits = nUnits;

    return hr;

} // CDefaultASCIIXlator::TranslateString2Keycodes
 

