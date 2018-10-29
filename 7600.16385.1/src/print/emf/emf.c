//
//  Windows (Printing) Driver Development Kit Samples.
//
//  Sample Enabling Print Queues for Enhanced Meta-File (EMF) Despooling.
//
//  emf.cxx - Sample code to enable Print Queue so that it forces all
//            clients, that make a connection to it, to use EMF
//            Despooling or Server-side Rendering instead of using
//            Client-side Rendering (default).
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



#include <windows.h>
#include <winspool.h>

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

#include "emf.h"

int
__cdecl
_tmain(
    __in                int     argc,
    __in_ecount(argc)   TCHAR * argv[]
    )
{
    //
    //  Check the arguments and display usage if they are incorrect.
    //
    if (argc == 2 && argv[1] && *argv[1] && argv[1][0] != TEXT('\\') && argv[1][0] != TEXT('/') && argv[1][0] != TEXT('-'))
    {

        wprintf(L"Trying to Set Emf Despooling on Print Queue: %s\n\n", argv[1]);

        //
        // Call the function that enables EMF despooling.
        //
        SetPrintQueueEmfDespooling(argv[1]);
    }
    else
    {
        usage();
    }

    return 1;
}

//
// Helper function to display usage of this sample Application.
//
void
usage(
    void
    )
{
    wprintf(L"Syntax:   EmfDespooling <Local Printer Name>\n\n"
            L"Sets a Local Print Queue so that it forces all clients, that make a\n"
            L"connection to it, to use EMF Despooling or Server-side Rendering\n"
            L"instead of using Client-side Rendering (default).\n");

    exit(0);
}



//
// This is the actual code that sets the value on the PrintQueue
// to enable EMF despooling.
//
void
SetPrintQueueEmfDespooling(
    __in    WCHAR *   pszPrinterName
)
{
    HANDLE              hPrinter    = 0;
    PRINTER_DEFAULTS    Defaults;
    DWORD               dwData      = 0;
    DWORD               dwError     = ERROR_SUCCESS;

    Defaults.pDatatype     = NULL;
    Defaults.pDevMode      = NULL;
    Defaults.DesiredAccess = PRINTER_ACCESS_ADMINISTER;

    //
    // Open a handle to the printer with administrator access.
    //
    if (!OpenPrinter(pszPrinterName, &hPrinter, &Defaults))
    {
        wprintf(L"OpenPrinter on %s failed with error %d.\n\n",
                pszPrinterName, GetLastError());
    }
    else
    {
        wprintf(L"OpenPrinter on %s succeeded.\n\n",
                pszPrinterName);

        dwData = 1;

        //
        // Set the EmfDespoolingSetting value to 1 for this printer.
        //
        dwError = SetPrinterData(hPrinter,
                                 L"EmfDespoolingSetting",
                                 REG_DWORD,
                                 (LPBYTE)&dwData,
                                 sizeof(DWORD));

        if(dwError != ERROR_SUCCESS)
        {
            wprintf(L"SetPrinterData on %s failed with error %d.\n\n",
                    pszPrinterName, dwError);
        }
        else
        {
            wprintf(L"SetPrinterData on %s succeeded.\n\n",
                    pszPrinterName);
        }

        //
        // Close the opened printer handle.
        //
        ClosePrinter(hPrinter);
    }
}

