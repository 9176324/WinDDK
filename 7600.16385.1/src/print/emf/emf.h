//
//  Windows Server (Printing) Driver Development Kit Samples.
//
//  SSample Enabling Print Queues for Enhanced Meta-File (EMF) Despooling.
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

int
__cdecl
_tmain(
    __in                int     argc,
    __in_ecount(argc)   TCHAR * argv[]
    );

void
usage(
    void
    );

void
SetPrintQueueEmfDespooling(
    __in    WCHAR *   pszPrinterName
    );

