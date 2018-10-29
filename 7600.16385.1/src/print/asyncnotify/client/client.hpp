//
//  Windows Server (Printing) Driver Development Kit Samples.
//
//  Sample Print Provider template.
//
//  Copyright (c) 1990 - 2005 Microsoft Corporation.
//  All Rights Reserved.
//
//
//  This source code is intended only as a supplement to Microsoft
//  Development Tools and/or on-line documentation.  See these other
//  materials for detailed information regarding Microsoft code samples.
// 
//  THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//  PARTICULAR PURPOSE.
//

#ifndef _CLIENT_HPP
#define _CLIENT_HPP


struct CmdLineArgs
{
    PCWSTR pszName;
};

EXTERN_C
HRESULT _cdecl
_tmain(
    __in              INT         argc,
    __in_ecount(argc) TCHAR**     argv
    );

HRESULT
ValidateArgs(
    __in CmdLineArgs*    pArgs
    );

HRESULT
ExecuteSample(
    __in CmdLineArgs*    pArgs
    );

HRESULT
ExecuteSample2(
    __in CmdLineArgs*    pArgs
    );


HRESULT
ParseCommandLine(
    __in               UINT            argc,
    __in_ecount(argc)  TCHAR**         argv,
    __in               CmdLineArgs*    pArgs    
    );

VOID
Usage(
    VOID
    );

#endif

