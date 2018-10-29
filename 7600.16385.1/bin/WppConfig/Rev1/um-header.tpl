`**********************************************************************`
`* This is an include template file for tracewpp preprocessor.        *`
`*                                                                    *`
`*    Copyright (c) Microsoft Corporation. All Rights Reserved.       *`
`**********************************************************************`

// template `TemplateFile`
#ifdef  WPP_THIS_FILE
// included twice
#       define  WPP_ALREADY_INCLUDED
#       undef   WPP_THIS_FILE
#endif  // #ifdef WPP_THIS_FILE

#define WPP_THIS_FILE `SourceFile.CanonicalName`

#ifndef WPP_ALREADY_INCLUDED

`* Dump defintions specified via -D on the command line to WPP *`

`FORALL def IN MacroDefinitions`
#define `def.Name` `def.Alias`
`ENDFOR`

#ifdef BUILD_MINWIN
#include <windowscore.h>
#else
#include <windows.h>
#endif

#include <wmistr.h>
#include <evntrace.h>
#ifndef WPP_TRACE_W2K_COMPATABILITY
#include <sddl.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif

// LEGACY: This structure was used by Win2k RpcRt4 and cluster tracing

typedef struct _WPP_WIN2K_CONTROL_BLOCK {
    TRACEHANDLE Logger;
    ULONG Flags;
    ULONG Level;
} WPP_WIN2K_CONTROL_BLOCK, *PWPP_WIN2K_CONTROL_BLOCK;

#pragma warning(push)
#pragma warning(disable:4201) // nonstandard extension used nameless struct/union

typedef struct _WPP_TRACE_CONTROL_BLOCK
{
    struct _WPP_TRACE_CONTROL_BLOCK *Next;
    TRACEHANDLE     UmRegistrationHandle;
    union {
        TRACEHANDLE              Logger;
        PWPP_WIN2K_CONTROL_BLOCK Win2kCb;
        PVOID                    Ptr;
        struct _WPP_TRACE_CONTROL_BLOCK *Cb;
    };

    UCHAR           FlagsLen; 
    UCHAR           Level; 
    USHORT          Options;
    ULONG           Flags[1];
} WPP_TRACE_CONTROL_BLOCK, *PWPP_TRACE_CONTROL_BLOCK;
#pragma warning(pop)

#ifdef WPP_USE_NTDLL_FUNCTIONS

ULONG
__stdcall
EtwTraceMessage(
    __in TRACEHANDLE LoggerHandle,
    __in ULONG MessageFlags,
    __in LPCGUID MessageGuid,
    __in USHORT      MessageNumber,
    ...
    );

TRACEHANDLE
__stdcall
EtwGetTraceLoggerHandle(
    __in PVOID Buffer
    );

UCHAR
__stdcall
EtwGetTraceEnableLevel(
    __in TRACEHANDLE LoggerHandle
    );

ULONG
__stdcall
EtwGetTraceEnableFlags(
    __in TRACEHANDLE LoggerHandle
    );

ULONG 
__stdcall
EtwRegisterTraceGuidsW(
    __in WMIDPREQUEST RequestAddress,
    __in PVOID RequestContext,
    __in LPCGUID ControlGuid,
    __in ULONG GuidCount,
    __in PTRACE_GUID_REGISTRATION GuidReg,
    __in_opt LPCWSTR MofImagePath,
    __in_opt LPCWSTR MofResourceName,
    __out PTRACEHANDLE RegistrationHandle
    );

ULONG
__stdcall
EtwUnregisterTraceGuids(
    __in TRACEHANDLE RegistrationHandle
    );

#define WPP_TRACE                   EtwTraceMessage
#define WPP_GET_TRACE_LOGGER_HANDLE EtwGetTraceLoggerHandle
#define WPP_GET_TRACE_ENABLE_LEVEL  EtwGetTraceEnableLevel
#define WPP_GET_TRACE_ENABLE_FLAGS  EtwGetTraceEnableFlags
#define WPP_REGISTER_TRACE_GUIDS    EtwRegisterTraceGuidsW
#define WPP_UNREGISTER_TRACE_GUIDS  EtwUnregisterTraceGuids

#endif

#define WPP_IsValidSid IsValidSid
#define WPP_GetLengthSid GetLengthSid

#ifndef WPP_TRACE
#define WPP_TRACE TraceMessage
#endif

#ifndef WPP_UNREGISTER_TRACE_GUIDS
#define WPP_UNREGISTER_TRACE_GUIDS  UnregisterTraceGuids 
#endif


#ifndef WPP_REGISTER_TRACE_GUIDS 
#define WPP_REGISTER_TRACE_GUIDS  RegisterTraceGuids
#endif


#ifndef WPP_GET_TRACE_LOGGER_HANDLE
#define WPP_GET_TRACE_LOGGER_HANDLE  GetTraceLoggerHandle
#endif

#ifndef WPP_GET_TRACE_ENABLE_LEVEL 
#define WPP_GET_TRACE_ENABLE_LEVEL  GetTraceEnableLevel
#endif

#ifndef WPP_GET_TRACE_ENABLE_FLAGS 
#define WPP_GET_TRACE_ENABLE_FLAGS  GetTraceEnableFlags
#endif

enum {
    WPP_VER_WIN2K_CB_FORWARD_PTR    = 0x01,
    WPP_VER_WHISTLER_CB_FORWARD_PTR = 0x02,
    WPP_VER_LH_CB_FORWARD_PTR = 0x03
};

VOID WppCleanupUm( VOID );
#define WPP_CLEANUP() WppCleanupUm()

#define WppLoadTracingSupport 

#if defined(__cplusplus)
};
#endif

#endif  // #ifndef WPP_ALREADY_INCLUDED



