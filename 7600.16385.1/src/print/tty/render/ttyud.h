//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//  ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//  PARTICULAR PURPOSE.
//
//  Copyright  1997-2003  Microsoft Corporation.  All Rights Reserved.
//
//  FILE:   TTYUD.H
//
//  PURPOSE:    Define common data types, and external function prototypes
//              for TTYUD rendering Module.
//
#pragma once


////////////////////////////////////////////////////////
//      Text Driver Function Prototypes
////////////////////////////////////////////////////////

BOOL 
TextGetInfo(
    DWORD dwInfo, 
    __out_bcount(cbSize) PVOID pBuffer, 
    DWORD cbSize,
    __out PDWORD pcbNeeded
    );

PDEVOEM 
TextEnablePDEV(
    PDEVOBJ pdevobj, 
    __in PWSTR pPrinterName, 
    ULONG cPatterns,
    __in_ecount(cPatterns) HSURF *phsurfPatterns, 
    ULONG cjGdiInfo, 
    __in_bcount(cjGdiInfo) GDIINFO *pGdiInfo,
    ULONG cjDevInfo, 
    __in_bcount(cjDevInfo) DEVINFO *pDevInfo, 
    DRVENABLEDATA *pded
    );

VOID 
TextDisablePDEV(
    __in PDEVOBJ pdevobj
    );

BOOL
TextEnableDriver(
    DWORD dwOEMintfVersion, 
    DWORD dwSize, 
    PDRVENABLEDATA pded
    );

BOOL 
TextTTYGetInfo(
    __in PDEVOBJ pdevobj,    
    DWORD  dwInfoIndex,
    __out_bcount(dwSize) PVOID   pOutBuf,  
    DWORD  dwSize, 
    __in DWORD  *pcbNeeded
    );
