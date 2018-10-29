//----------------------------------------------------------------------------
//
// Example of how to connect to a debugger server and execute
// a command when the server is broken in.
//
// Copyright (C) Microsoft Corporation, 2002-2006.
//
//----------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <windows.h>
#include <dbgeng.h>

#include "extr.hpp"
#include "out.hpp"

BOOL g_ForceBreak;
PSTR g_Connect;
PSTR g_Command = ".echo Broken in";
PSTR g_ExtDll;
PSTR g_ExtArgs = "";

IDebugClient* g_Client;
IDebugClient* g_ExitDispatchClient;
IDebugControl* g_Control;
ULONG g_ExecStatus;
Extension g_Ext;

//----------------------------------------------------------------------------
//
// Event callbacks.
//
//----------------------------------------------------------------------------

class EventCallbacks : public DebugBaseEventCallbacks
{
public:
    // IUnknown.
    STDMETHOD_(ULONG, AddRef)(
        THIS
        );
    STDMETHOD_(ULONG, Release)(
        THIS
        );

    // IDebugEventCallbacks.
    STDMETHOD(GetInterestMask)(
        THIS_
        OUT PULONG Mask
        );
    
    STDMETHOD(ChangeEngineState)(
        THIS_
        IN ULONG Flags,
        IN ULONG64 Argument
        );
};

STDMETHODIMP_(ULONG)
EventCallbacks::AddRef(
    THIS
    )
{
    // This class is designed to be static so
    // there's no true refcount.
    return 1;
}

STDMETHODIMP_(ULONG)
EventCallbacks::Release(
    THIS
    )
{
    // This class is designed to be static so
    // there's no true refcount.
    return 0;
}

STDMETHODIMP
EventCallbacks::GetInterestMask(
    THIS_
    OUT PULONG Mask
    )
{
    *Mask = DEBUG_EVENT_CHANGE_ENGINE_STATE;
    return S_OK;
}

STDMETHODIMP
EventCallbacks::ChangeEngineState(
    THIS_
    IN ULONG Flags,
    IN ULONG64 Argument
    )
{
    if (Flags & DEBUG_CES_EXECUTION_STATUS)
    {
        g_ExecStatus = (ULONG)Argument;

        // If this notification came from a wait completing
        // we want to wake up the input thread so that new
        // commands can be processed.  If it came from inside
        // a wait we don't want to ask for input as the engine
        // may go back to running at any time.
        if (!(Argument & DEBUG_STATUS_INSIDE_WAIT))
        {
            // Wake up the wait loop.
            g_ExitDispatchClient->ExitDispatch(g_Client);
        }
    }
    
    return S_OK;
}

EventCallbacks g_EventCb;

//----------------------------------------------------------------------------
//
// Functions.
//
//----------------------------------------------------------------------------

void
Exit(__in int Code,
     __in PCSTR Format,
     ...)
{
    // Clean up any resources.
    if (g_Client != NULL)
    {
        g_Client->EndSession(DEBUG_END_DISCONNECT);
        g_Client->Release();
        g_Client = NULL;
    }

    if (g_Control != NULL)
    {
        g_Control->Release();
        g_Control = NULL;
    }
    if (g_ExitDispatchClient != NULL)
    {
        g_ExitDispatchClient->Release();
        g_ExitDispatchClient = NULL;
    }

    // Output an error message if given.
    if (Format != NULL)
    {
        va_list Args;

        va_start(Args, Format);
        vfprintf(stderr, Format, Args);
        va_end(Args);
    }
    
    exit(Code);
}

void
CreateInterfaces(void)
{
    HRESULT Status;

    // Start things off by getting an initial interface from
    // the engine.  This can be any engine interface but is
    // generally IDebugClient as the client interface is
    // where sessions are started.
    if ((Status = DebugConnect(g_Connect,
                               __uuidof(IDebugClient),
                              (void**)&g_Client)) != S_OK)
    {
        Exit(1, "DebugConnect(%s) failed, 0x%X\n",
             g_Connect, Status);
    }

    // Query for some other interfaces that we'll need.
    if ((Status = g_Client->QueryInterface(__uuidof(IDebugControl),
                                           (void**)&g_Control)) != S_OK)
    {
        Exit(1, "QueryInterface failed, 0x%X\n", Status);
    }

    if ((Status = g_Client->SetEventCallbacks(&g_EventCb)) != S_OK)
    {
        Exit(1, "SetEventCallbacks failed, 0x%X\n", Status);
    }

    //
    // This app may wait inside of a DispatchCallbacks
    // while it's waiting for the server to break in.
    // We'll need to be able to exit that dispatch so
    // we need another connection to the server to
    // send the exit request.
    //
    // This could all be avoided by simply doing a polling
    // loop when waiting, but the intent of this sample
    // is to show some of the more advanced callback-driven
    // techniques.
    //

    if ((Status = DebugConnect(g_Connect,
                               __uuidof(IDebugClient),
                              (void**)&g_ExitDispatchClient)) != S_OK)
    {
        Exit(1, "DebugConnect(%s) failed, 0x%X\n",
             g_Connect, Status);
    }
}

void
ParseCommandLine(__in int Argc,
                 __in_ecount(Argc) char** Argv)
{
    while (--Argc > 0)
    {
        Argv++;

        if (!strcmp(*Argv, "-b"))
        {
            g_ForceBreak = TRUE;
        }
        else if (!strcmp(*Argv, "-cmd"))
        {
            if (Argc < 2)
            {
                Exit(1, "-cmd missing argument\n");
            }

            Argv++;
            Argc--;

            g_Command = *Argv;
        }
        else if (!strcmp(*Argv, "-e"))
        {
            if (Argc < 2)
            {
                Exit(1, "-e missing argument\n");
            }

            Argv++;
            Argc--;

            g_ExtDll = *Argv;
        }
        else if (!strcmp(*Argv, "-eargs"))
        {
            if (Argc < 2)
            {
                Exit(1, "-eargs missing argument\n");
            }

            Argv++;
            Argc--;

            g_ExtArgs = *Argv;
        }
        else if (!strcmp(*Argv, "-remote"))
        {
            if (Argc < 2)
            {
                Exit(1, "-remote missing argument\n");
            }

            Argv++;
            Argc--;

            g_Connect = *Argv;
        }
        else
        {
            Exit(1, "Unknown command line argument '%s'\n", *Argv);
        }
    }

    if (!g_Connect)
    {
        Exit(1, "No connection string specified, use -remote <options>\n");
    }
}

void
WaitForBreakIn(void)
{
    HRESULT Status;

    // If we're forcing a break in request one.
    if (g_ForceBreak)
    {
        if ((Status = g_Control->SetInterrupt(DEBUG_INTERRUPT_ACTIVE)) != S_OK)
        {
            Exit(1, "SetInterrupt failed, 0x%X\n", Status);
        }
    }

    // Check on the current state as the server may be broken in.
    if ((Status = g_Control->GetExecutionStatus(&g_ExecStatus)) != S_OK)
    {
        Exit(1, "GetExecutionStatus failed, 0x%X\n", Status);
    }

    printf("Waiting for break-in...\n");
    
    while (g_ExecStatus != DEBUG_STATUS_BREAK)
    {
        // Wait for the server to enter the break-in state.
        // When this happens our event callbacks will get called
        // to update g_ExecStatus and wake up this wait.
        if ((Status = g_Client->DispatchCallbacks(INFINITE)) != S_OK)
        {
            Exit(1, "DispatchCallbacks failed, 0x%X\n", Status);
        }
    }

    // The server is broken in.  Another user can immediately resume
    // it but we'll assume that we're not competing with
    // other server users.
}

void
ExecuteExtensionCommand(void)
{
    HRESULT Status;

    Status = g_Ext.Load(g_ExtDll);
    if (Status != S_OK)
    {
        Exit(1, "Unable to load extension DLL '%s', 0x%X\n",
             g_ExtDll, Status);
    }

    // Some extensions monitor session notifications
    // in order to do cache management or to know when
    // certain operations are possible or not.  We know
    // the debuggee is broken in at this point so notify
    // the extension that the session is active and
    // accessible.
    g_Ext.NotifyAccessible();
    
    // Install our output callbacks so that
    // we can display any output the extension
    // produces.
    Status = g_Client->SetOutputCallbacks(&g_OutputCb);
    if (Status != S_OK)
    {
        Exit(1, "Unable to register output callbacks, 0x%X\n",
             Status);
    }

    printf("Executing extension '%s'\n", g_Command);

    HRESULT ExtStatus;
    
    Status = g_Ext.Call(g_Client, g_Command, g_ExtArgs, &ExtStatus);
    if (Status != S_OK)
    {
        Exit(1, "Unable to call extension %s, 0x%X\n",
             g_Command, Status);
    }

    // The extension return value is usually
    // uninteresting so don't bother displaying it.
    
    g_Ext.Unload();
}

void
ExecuteDirectCommand(void)
{
    printf("Executing '%s' on server\n", g_Command);
    g_Control->Execute(DEBUG_OUTCTL_ALL_CLIENTS,
                       g_Command, DEBUG_EXECUTE_DEFAULT);
}

void __cdecl
main(__in int Argc,
     __in_ecount(Argc) char** Argv)
{
    ParseCommandLine(Argc, Argv);

    CreateInterfaces();
    
    WaitForBreakIn();

    if (g_ExtDll)
    {
        ExecuteExtensionCommand();
    }
    else
    {
        ExecuteDirectCommand();
    }

    exit(0);
}
