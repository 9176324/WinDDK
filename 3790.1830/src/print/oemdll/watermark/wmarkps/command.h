//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//  ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//  PARTICULAR PURPOSE.
//
//  Copyright  1997 - 2003  Microsoft Corporation.  All Rights Reserved.
//
//  FILE:  Command.h
//    
//
//  PURPOSE:	Define common data types, and external function prototypes
//				for OEM Command function(s).
//
//  PLATFORMS:
//
//    Windows 2000, Windows XP, Windows Server 2003
//
//
#ifndef _COMMAND_H
#define _COMMAND_H




/////////////////////////////////////////////////////////
//		ProtoTypes
/////////////////////////////////////////////////////////

HRESULT PSCommand(PDEVOBJ, DWORD, PVOID, DWORD, IPrintOemDriverPS*, PDWORD pdwReturn);


#endif



