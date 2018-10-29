/*-----------------------------------------------------------------------------
   Copyright (c) 2000  Microsoft Corporation

Module:
  exts.cpp

  Sampe file showing couple of extension examples

-----------------------------------------------------------------------------*/
#include "dbgexts.h"


/*
   Sample extension to demonstrate executing debugger command

 */
HRESULT CALLBACK
cmdsample(PDEBUG_CLIENT4 Client, PCSTR args)
{
    CHAR Input[256];
    INIT_API();

    UNREFERENCED_PARAMETER(args);

    //
    // Output a 10 frame stack
    //
    g_ExtControl->OutputStackTrace(DEBUG_OUTCTL_ALL_CLIENTS |   // Flags on what to do with output
                                   DEBUG_OUTCTL_OVERRIDE_MASK |
                                   DEBUG_OUTCTL_NOT_LOGGED,
                                   NULL,
                                   10,           // number of frames to display
                                   DEBUG_STACK_FUNCTION_INFO | DEBUG_STACK_COLUMN_NAMES |
                                   DEBUG_STACK_ARGUMENTS | DEBUG_STACK_FRAME_ADDRESSES);
    //
    // Engine interface for print
    //
    g_ExtControl->Output(DEBUG_OUTCTL_ALL_CLIENTS, "\n\nDebugger module list\n");

    //
    // list all the modules by executing lm command
    //
    g_ExtControl->Execute(DEBUG_OUTCTL_ALL_CLIENTS |
                          DEBUG_OUTCTL_OVERRIDE_MASK |
                          DEBUG_OUTCTL_NOT_LOGGED,
                          "lm", // Command to be executed
                          DEBUG_EXECUTE_DEFAULT );

    //
    // Ask for user input
    //
    g_ExtControl->Output(DEBUG_OUTCTL_ALL_CLIENTS, "\n\n***User Input sample\n\nEnter Command to run : ");
    GetInputLine(NULL, &Input[0], sizeof(Input));
    g_ExtControl->Execute(DEBUG_OUTCTL_ALL_CLIENTS |
                          DEBUG_OUTCTL_OVERRIDE_MASK |
                          DEBUG_OUTCTL_NOT_LOGGED,
                          Input, // Command to be executed
                          DEBUG_EXECUTE_DEFAULT );

    EXIT_API();
    return S_OK;
}

/*
  Sample extension to read and dump a struct on target

  This reads the struct _EXCEPTION_RECORD which is defined as:

  typedef struct _EXCEPTION_RECORD {
    NTSTATUS ExceptionCode;
    ULONG ExceptionFlags;
    struct _EXCEPTION_RECORD *ExceptionRecord;
    PVOID ExceptionAddress;
    ULONG NumberParameters;
    ULONG_PTR ExceptionInformation[EXCEPTION_MAXIMUM_PARAMETERS];
    } EXCEPTION_RECORD;
*/
HRESULT CALLBACK
structsample(PDEBUG_CLIENT4 Client, PCSTR args)
{
    ULONG64 Address;
    INIT_API();

    Address = GetExpression(args);

    DWORD Buffer[4], cb;

    // Read and display first 4 dwords at Address
    if (ReadMemory(Address, &Buffer, sizeof(Buffer), &cb) && cb == sizeof(Buffer)) {
        dprintf("%p: %08lx %08lx %08lx %08lx\n\n", Address,
                Buffer[0], Buffer[1], Buffer[2], Buffer[3]);
    }

    //
    // Method 1 to dump a struct
    //
    dprintf("Method 1:\n");
    // Inititalze type read from the Address
    if (InitTypeRead(Address, _EXCEPTION_RECORD) != 0) {
        dprintf("Error in reading _EXCEPTION_RECORD at %p", // Use %p to print pointer values
                Address);
    } else {
        // read and dump the fields
        dprintf("_EXCEPTION_RECORD @ %p\n", Address);
        dprintf("\tExceptionCode           : %lx\n", (ULONG) ReadField(ExceptionCode));
        dprintf("\tExceptionAddress        : %p\n", ReadField(ExceptionAddress));
        dprintf("\tExceptionInformation[1] : %I64lx\n", ReadField(ExceptionInformation[1]));
        // And so on...
    }

    //
    // Method 2 to read a struct
    //
    ULONG64 ExceptionInformation_1, ExceptionAddress, ExceptionCode;
    dprintf("\n\nMethod 2:\n");
    // Read and dump the fields by specifying type and address individually
    if (GetFieldValue(Address, "_EXCEPTION_RECORD", "ExceptionCode", ExceptionCode)) {
        dprintf("Error in reading _EXCEPTION_RECORD at %p\n",
                Address);
    } else {
        // Pointers are read as ULONG64 values
        GetFieldValue(Address, "_EXCEPTION_RECORD", "ExceptionAddress", ExceptionAddress);
        GetFieldValue(Address, "_EXCEPTION_RECORD", "ExceptionInformation[1]", ExceptionInformation_1);
        // And so on..

        dprintf("_EXCEPTION_RECORD @ %p\n", Address);
        dprintf("\tExceptionCode           : %lx\n", ExceptionCode);
        dprintf("\tExceptionAddress        : %p\n", ExceptionAddress);
        dprintf("\tExceptionInformation[1] : %I64lx\n", ExceptionInformation_1);
    }

    ULONG64 Module;
    ULONG   i, TypeId;
    CHAR Name[MAX_PATH];
    //
    // To get/list field names
    //
    g_ExtSymbols->GetSymbolTypeId("_EXCEPTION_RECORD", &TypeId, &Module);
    dprintf("Fields of _EXCEPTION_RECORD\n");
    for (i=0; ;i++) {
        HRESULT Hr;
        ULONG Offset=0;

        Hr = g_ExtSymbols->GetFieldName(Module, TypeId, i, Name, MAX_PATH, NULL);
        if (Hr == S_OK) {
            g_ExtSymbols->GetFieldOffset(Module, TypeId, Name, &Offset);
            dprintf("%lx (+%03lx) %s\n", i, Offset, Name);
        } else if (Hr == E_INVALIDARG) {
            // All Fields done
            break;
        } else {
            dprintf("GetFieldName Failed %lx\n", Hr);
            break;
        }
    }

    //
    // Get name for an enumerate
    //
    //     typedef enum {
    //        Enum1,
    //        Enum2,
    //        Enum3,
    //     } TEST_ENUM;
    //
    ULONG   ValueOfEnum = 0;
    g_ExtSymbols->GetSymbolTypeId("TEST_ENUM", &TypeId, &Module);
    g_ExtSymbols->GetConstantName(Module, TypeId, ValueOfEnum, Name, MAX_PATH, NULL);
    dprintf("Testenum %I64lx == %s\n", ExceptionCode, Name);
    // This prints out, Testenum 0 == Enum1

    //
    // Read an array
    //
    //    typedef struct FOO_TYPE {
    //      ULONG Bar;
    //      ULONG Bar2;
    //    } FOO_TYPE;
    //
    //    FOO_TYPE sampleArray[20];
    ULONG Bar, Bar2;
    CHAR TypeName[100];
    for (i=0; i<20; i++) {
        sprintf_s(TypeName, sizeof(TypeName), "sampleArray[%lx]", i);
        if (GetFieldValue(0, TypeName, "Bar", Bar))
            break;
        GetFieldValue(0, TypeName, "Bar2", Bar2);
        dprintf("%16s -  Bar %2ld  Bar2 %ld\n", TypeName, Bar, Bar2);
    }

    EXIT_API();
    return S_OK;
}

/*
   Extension to look at locals through IDebugSymbolGroup interface

        Usage: !symgrptest [args]

        To demontrate local/args lookup, this will go through the stack
        and set scope with the instruction of the stack frame and
        enumerate all the locals/arguments for the frame function
*/
HRESULT CALLBACK
symgrptest(PDEBUG_CLIENT4 Client, PCSTR args)
{
    HRESULT hRes;
    IDebugSymbolGroup *pDbgSymGroup;
    ULONG ulCount, nFrames;
    DEBUG_STACK_FRAME Stack[50];
    ULONG SymGroupType;

    INIT_API();

    if (!_stricmp(args, "args"))
    {
        // Disply only the arguments
        SymGroupType = DEBUG_SCOPE_GROUP_ARGUMENTS;
    } else
    {
        // Display all the locals
        SymGroupType = DEBUG_SCOPE_GROUP_LOCALS;
    }

    //
    // Get the current stack
    //
    if ((hRes = g_ExtControl->GetStackTrace(0, 0, 0, &Stack[0], 50, &nFrames)) != S_OK) {
        dprintf("Stacktrace failed - %lx\n", hRes);
        nFrames = 0;
    }

    // Create a local symbol group client
    if ((hRes = g_ExtSymbols->
         GetScopeSymbolGroup(SymGroupType,
                             NULL, &pDbgSymGroup)) == E_NOTIMPL)
    {
        EXIT_API();
        return S_OK;
    }

    while (nFrames) {
        //
        // Enumerate locals for this frame
        //
        --nFrames;

        // Set scope at this frame
        g_ExtSymbols->SetScope(0, &Stack[nFrames], NULL, 0);

        // refresh the symbol group with current scope
        if ((hRes = g_ExtSymbols->
             GetScopeSymbolGroup(DEBUG_SCOPE_GROUP_LOCALS,
                                 pDbgSymGroup, &pDbgSymGroup)) == E_NOTIMPL)
        {
            break;
        }
        hRes =
            pDbgSymGroup->GetNumberSymbols ( &ulCount);

        dprintf("\n\n>Scope Frame %lx: %lx Symbols\n",nFrames,ulCount);

        PDEBUG_SYMBOL_PARAMETERS pSymParams =
                 new DEBUG_SYMBOL_PARAMETERS[ ulCount ] ;
        if (ulCount)
        {
            // Get all symbols for the frame
            hRes =
                pDbgSymGroup->GetSymbolParameters (0,
                                               ulCount    ,
                                               pSymParams);
        }
        if ( S_OK == hRes )
        {
            for ( ULONG i = 0 ; i < ulCount ; i++ )
                {
                TCHAR szName[ MAX_PATH ], *pName;
                ULONG ulSize;
                DEBUG_VALUE Value;

                // Lookup symbol name and print
                pName = &szName[1];
                hRes = pDbgSymGroup->GetSymbolName ( i,
                                                     pName,
                                                     MAX_PATH - 1,
                                                     &ulSize ) ;

                // Prefix ! so this is evaluated as symbol
                szName[0] = '!';
                hRes = g_ExtControl->Evaluate(szName,
                                              DEBUG_VALUE_INVALID,
                                              &Value, NULL);

                dprintf("%lx: %32s = 0x%p\n",
                        i, pName, Value.I64);
            }
        }
        delete pSymParams;
    }
    pDbgSymGroup->Release();
    EXIT_API();
    return S_OK;
}

/*
  This gets called (by DebugExtensionNotify whentarget is halted and is accessible
*/
HRESULT
NotifyOnTargetAccessible(PDEBUG_CONTROL Control)
{
    dprintf("Extension dll detected a break");
    if (Connected) {
        dprintf(" connected to ");
        switch (TargetMachine) {
        case IMAGE_FILE_MACHINE_I386:
            dprintf("X86");
            break;
        case IMAGE_FILE_MACHINE_IA64:
            dprintf("IA64");
            break;
        default:
            dprintf("Other");
            break;
        }
    }
    dprintf("\n");

    //
    // show the top frame and execute dv to dump the locals here and return
    //
    Control->Execute(DEBUG_OUTCTL_ALL_CLIENTS |
                     DEBUG_OUTCTL_OVERRIDE_MASK |
                     DEBUG_OUTCTL_NOT_LOGGED,
                     ".frame", // Command to be executed
                     DEBUG_EXECUTE_DEFAULT );
    Control->Execute(DEBUG_OUTCTL_ALL_CLIENTS |
                     DEBUG_OUTCTL_OVERRIDE_MASK |
                     DEBUG_OUTCTL_NOT_LOGGED,
                     "dv", // Command to be executed
                     DEBUG_EXECUTE_DEFAULT );
    return S_OK;
}

/*
  A built-in help for the extension dll
*/
HRESULT CALLBACK
help(PDEBUG_CLIENT4 Client, PCSTR args)
{
    INIT_API();

    UNREFERENCED_PARAMETER(args);

    dprintf("Help for dbgexts.dll\n"
            "  cmdsample           - This does stacktrace and lists\n"
            "  help                = Shows this help\n"
            "  structsample <addr> - This dumps a struct at given address\n"
            );
    EXIT_API();

    return S_OK;
}
