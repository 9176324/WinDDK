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

`* Dump the definitions specified via -D on the command line to WPP *`

`FORALL def IN MacroDefinitions`
#define `def.Name` `def.Alias`
`ENDFOR`

#define WPP_THIS_FILE `SourceFile.CanonicalName`

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum _WPP_TRACE_API_SUITE {
    WppTraceDisabledSuite,
    WppTraceWin2K,
    WppTraceWinXP,
    WppTraceTraceLH,
    WppTraceServer08,
    WppTraceMaxSuite
} WPP_TRACE_API_SUITE;

__drv_sameIRQL
typedef
VOID
(*PETW_CLASSIC_CALLBACK)(
    __in LPCGUID Guid,
    __in UCHAR ControlCode,
    __in PVOID EnableContext, 
    __in_opt PVOID CallbackContext
    );

__drv_sameIRQL
typedef
NTSTATUS
NTKERNELAPI
(FN_ETWREGISTERCLASSICPROVIDER)(
    __in LPCGUID ProviderGuid,
    __in ULONG Type,
    __in PETW_CLASSIC_CALLBACK EnableCallback,
    __in_opt PVOID CallbackContext,
    __out PREGHANDLE RegHandle
    );

typedef FN_ETWREGISTERCLASSICPROVIDER *PFN_ETWREGISTERCLASSICPROVIDER;

typedef
BOOLEAN
NTKERNELAPI
(FN_WPPGETVERSION)(
    __out_opt PULONG MajorVersion,
    __out_opt PULONG MinorVersion,
    __out_opt PULONG BuildNumber,
    __out_opt PUNICODE_STRING CSDVersion
    );

typedef FN_WPPGETVERSION *PFN_WPPGETVERSION;

typedef
NTSTATUS
NTKERNELAPI
(FN_ETWUNREGISTER)(
    __in REGHANDLE RegHandle
    );

typedef FN_ETWUNREGISTER *PFN_ETWUNREGISTER;


#pragma prefast(suppress:__WARNING_ENCODE_GLOBAL_FUNCTION_POINTER, "this pointer can not be encoded");
__declspec(selectany) PFN_WPPQUERYTRACEINFORMATION   pfnWppQueryTraceInformation = NULL;

#pragma prefast(suppress:__WARNING_ENCODE_GLOBAL_FUNCTION_POINTER, "this pointer can not be encoded");
__declspec(selectany) PFN_WPPTRACEMESSAGE            pfnWppTraceMessage = NULL;

#pragma prefast(suppress:__WARNING_ENCODE_GLOBAL_FUNCTION_POINTER, "this pointer can not be encoded");
__declspec(selectany) PFN_ETWUNREGISTER              pfnEtwUnregister = NULL;

#pragma prefast(suppress:__WARNING_ENCODE_GLOBAL_FUNCTION_POINTER, "this pointer can not be encoded");
__declspec(selectany) PFN_ETWREGISTERCLASSICPROVIDER pfnEtwRegisterClassicProvider = NULL;

#pragma prefast(suppress:__WARNING_ENCODE_GLOBAL_FUNCTION_POINTER, "this pointer can not be encoded");
__declspec(selectany) PFN_WPPGETVERSION              pfnWppGetVersion = NULL;


__declspec(selectany) WPP_TRACE_API_SUITE            WPPTraceSuite = WppTraceDisabledSuite;


#if !defined(_NTRTL_)
#if !defined(_NTHAL_) 
      // fake RTL_TIME_ZONE_INFORMATION //
    typedef int RTL_TIME_ZONE_INFORMATION;
#endif
#   define _WMIKM_  
#endif
#ifndef WPP_TRACE
#define WPP_TRACE pfnWppTraceMessage
#endif

#if ENABLE_WPP_RECORDER

#ifndef WPP_RECORDER
#define WPP_RECORDER WppAutoLogTrace
#endif

#if !defined(WPP_RECORDER_LEVEL_FLAGS_ARGS)
#define WPP_RECORDER_LEVEL_FLAGS_ARGS(lvl, flags) WPP_CONTROL(WPP_BIT_ ## flags).AutoLogContext, lvl, WPP_BIT_ ## flags
#define WPP_RECORDER_LEVEL_FLAGS_FILTER(lvl,flags) (lvl < TRACE_LEVEL_VERBOSE || WPP_CONTROL(WPP_BIT_ ## flags).AutoLogVerboseEnabled)
#endif


#if !defined(WPP_RECORDER_LEVEL_ARGS)
#define WPP_RECORDER_LEVEL_ARGS(lvl) WPP_CONTROL(WPP_BIT_ ## lvl).AutoLogContext, 0, WPP_BIT_ ## lvl
#define WPP_RECORDER_LEVEL_FILTER(lvl) (WPP_CONTROL(WPP_BIT_ ## lvl).AutoLogVerboseEnabled)
#endif

NTSTATUS
WppAutoLogTrace(
    IN PVOID              AutoLogContext,
    IN UCHAR              MessageLevel,
    IN ULONG              MessageFlags,
    IN LPGUID             MessageGuid,
    IN USHORT             MessageNumber,
    IN ...
    );

#endif

VOID
WppLoadTracingSupport(
    VOID
    );

NTSTATUS
WppTraceCallback(
    __in UCHAR MinorFunction,
    __in_opt PVOID DataPath,
    __in ULONG BufferLength,
    __inout_bcount(BufferLength) PVOID Buffer,
    __in PVOID Context,
    __out PULONG Size
    );
#if !defined(WPP_TRACE_CONTROL_NULL_GUID)
DEFINE_GUID(WPP_TRACE_CONTROL_NULL_GUID, 0x00000000L, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
#endif
#define WPP_TRACE_CONTROL(Function,Buffer,BufferSize,ReturnSize) WppTraceCallback(Function,NULL,BufferSize,Buffer,&WPP_CB[0],&ReturnSize);
    

__inline ULONG64 WppQueryLogger(__in_opt PCWSTR LoggerName)
{

    if (WppTraceWinXP == WPPTraceSuite) {

        ULONG ReturnLength ;
        LONG Status ;
        ULONG64 TraceHandle ;
        UNICODE_STRING  Buffer  ;
           
        RtlInitUnicodeString(&Buffer, LoggerName ? LoggerName : L"stdout");

        Status = pfnWppQueryTraceInformation(
                                            TraceHandleByNameClass,
                                            (PVOID)&TraceHandle,
                                            sizeof(TraceHandle),
                                            &ReturnLength,
                                            (PVOID)&Buffer
                                            );
        if (Status != STATUS_SUCCESS) {
           return (ULONG64)0 ;
        }
        
        return TraceHandle ;
    } else {
        return (ULONG64) 0 ;
    }
}

typedef LONG (*WMIENTRY_NEW)(
    __in UCHAR MinorFunction,
    __in_opt PVOID DataPath,
    __in ULONG BufferLength,
    __inout_bcount(BufferLength) PVOID Buffer,
    __in PVOID Context,
    __out PULONG Size
    );

typedef struct _WPP_TRACE_CONTROL_BLOCK
{
    WMIENTRY_NEW                        Callback;
    LPCGUID                             ControlGuid;
    struct _WPP_TRACE_CONTROL_BLOCK    *Next;
    __int64                             Logger;
    PUNICODE_STRING                     RegistryPath;
    UCHAR                               FlagsLen; 
    UCHAR                               Level; 
    USHORT                              Reserved;
    ULONG                               Flags[1];
    ULONG                               ReservedFlags;
    REGHANDLE                           RegHandle;
#if ENABLE_WPP_RECORDER    
    PVOID                               AutoLogContext;
    USHORT                              AutoLogVerboseEnabled;
    USHORT                              AutoLogAttachToMiniDump;
#endif    
} WPP_TRACE_CONTROL_BLOCK, *PWPP_TRACE_CONTROL_BLOCK;

VOID WppCleanupKm(__in_opt PDEVICE_OBJECT pDeviceObject);

#define WPP_CLEANUP(DrvObj) WppCleanupKm((PDEVICE_OBJECT) DrvObj)

#define WPP_IsValidSid RtlValidSid
#define WPP_GetLengthSid RtlLengthSid

//
// Callback routine to be defined by the driver, which will be called from WPP callback
// WPP will pass current valued of : GUID, Logger, Enable, Flags, and Level
// 
// To activate driver must define WPP_PRIVATE_ENABLE_CALLBACK in their code, sample below 
// #define WPP_PRIVATE_ENABLE_CALLBACK MyPrivateCallback;
//
typedef
VOID
(*PFN_WPP_PRIVATE_ENABLE_CALLBACK)(
    __in LPCGUID Guid,   
    __in __int64 Logger, 
    __in BOOLEAN Enable, 
    __in ULONG Flags,    
    __in UCHAR Level);   

#if defined(__cplusplus)
};
#endif

#endif  // #ifndef WPP_ALREADY_INCLUDED



