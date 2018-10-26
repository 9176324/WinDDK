//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//  ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//  PARTICULAR PURPOSE.
//
//  Copyright  1998 - 2003  Microsoft Corporation.  All Rights Reserved.
//
//  FILE:	Devmode.h
//    
//
//  PURPOSE:	Define common data types, and external function prototypes
//				for devmode functions.
//
//  PLATFORMS:
//
//    Windows 2000, Windows XP, Windows Server 2003
//
//
#ifndef _DEVMODE_H
#define _DEVMODE_H


////////////////////////////////////////////////////////
//      OEM Devmode Defines
////////////////////////////////////////////////////////

#define WATER_MARK_TEXT_SIZE            128
#define WATER_MARK_DEFAULT_ENABLED      TRUE
#define WATER_MARK_DEFAULT_FONTSIZE     28
#define WATER_MARK_DEFAULT_COLOR        RGB(230, 230, 230)
#define WATER_MARK_DEFAULT_TEXT         L"WaterMark"



////////////////////////////////////////////////////////
//      OEM Devmode Type Definitions
////////////////////////////////////////////////////////

typedef struct tagOEMDEV
{
    OEM_DMEXTRAHEADER   dmOEMExtra;
    BOOL                bWmarkEnabled;
    DWORD               dwWmarkFontSize;
    COLORREF            crWmarkTextColor;
    WCHAR               szWaterMark[WATER_MARK_TEXT_SIZE];

} OEMDEV, *POEMDEV;

typedef const OEMDEV *PCOEMDEV;



/////////////////////////////////////////////////////////
//		ProtoTypes
/////////////////////////////////////////////////////////

HRESULT hrOEMDevMode(DWORD dwMode, POEMDMPARAM pOemDMParam);
BOOL ConvertOEMDevmode(PCOEMDEV pOEMDevIn, POEMDEV pOEMDevOut);
BOOL MakeOEMDevmodeValid(POEMDEV pOEMDevmode);
BOOL IsValidFontSize(DWORD dwWmarkFontSize);
void Dump(PCOEMDEV pOEMDevIn);


#endif



