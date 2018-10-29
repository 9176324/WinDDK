//+--------------------------------------------------------------------------
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//  ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//  PARTICULAR PURPOSE.
//
//  Copyright  1997 - 2006  Microsoft Corporation.  All Rights Reserved.
//
//  FILE:       devmode.h
//    
//  PURPOSE:    Define common data types, and external function prototypes
//              for devmode functions.
//
//--------------------------------------------------------------------------

#pragma once

#define OEM_SIGNATURE               'GDLS'
#define OEM_VERSION                 0x00000001L

//
// OEM private DEVMODE data structure.  This gets included in the DEVMODE
// used by Unidrv.
//
typedef struct tagOEMDEV
{
    OEM_DMEXTRAHEADER   dmOEMExtra;
        // This structure MUST be prefixed by OEM_DMEXTRAHEADER

    //
    // Add private DevMode members here, after the standard header 
    // information.
    //

} OEMDEV, *POEMDEV;

typedef const OEMDEV *PCOEMDEV;


//
// Prototypes
//

HRESULT 
hrOEMDevMode(
    DWORD dwMode, 
    __inout POEMDMPARAM pOemDMParam
    );




