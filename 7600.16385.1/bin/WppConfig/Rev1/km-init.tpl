`**********************************************************************`
`* This is an include template file for tracewpp preprocessor.        *`
`*                                                                    *`
`*    Copyright (c) Microsoft Corporation. All Rights Reserved.       *`
`**********************************************************************`

// template `TemplateFile`
//
//     Defines a set of functions that simplifies
//     kernel mode registration for tracing
//

#pragma warning(disable: 4201)
#include <ntddk.h>

#if defined(__cplusplus)
extern "C" {
#endif

#if !defined(WppDebug)
#define WppDebug(a,b)
#endif

// Routine to handle the System Control in W2K
__drv_dispatchType(IRP_MJ_SYSTEM_CONTROL)
DRIVER_DISPATCH WPPSystemControlDispatch;

WPPINIT_EXPORT
void WppInitGlobalLogger(
        __in LPCGUID ControlGuid,
        __out PTRACEHANDLE LoggerHandle,
        __out PULONG Flags,
        __out PUCHAR Level 
        );


WPPINIT_EXPORT
VOID 
WppInitKm(
    __in_opt PDEVICE_OBJECT DevObject,
    __in_opt PCUNICODE_STRING RegPath
    );

#if ENABLE_WPP_RECORDER
WPPINIT_EXPORT
VOID 
WppAutoLogStart(
    __in PDRIVER_OBJECT DrvObj,
    __in PCUNICODE_STRING RegPath
    );

VOID 
WppAutoLogStop(
    __in PDRIVER_OBJECT DrvObj
    );    
#endif    

#ifdef ALLOC_PRAGMA
    #pragma alloc_text( PAGE, WPPSystemControlDispatch)
    #pragma alloc_text( PAGE, WppLoadTracingSupport)
    #pragma alloc_text( PAGE, WppInitGlobalLogger)
    #pragma alloc_text( PAGE, WppTraceCallback)
    #pragma alloc_text( PAGE, WppInitKm)
    #pragma alloc_text( PAGE, WppCleanupKm)
#endif // ALLOC_PRAGMA

// define annotation record that will carry control information to pdb (in case somebody needs it)
WPP_FORCEINLINE void WPP_CONTROL_ANNOTATION() {
#if !defined(WPP_NO_ANNOTATIONS)
#if !defined(WPP_ANSI_ANNOTATION) 
#  define WPP_DEFINE_CONTROL_GUID(Name,Guid,Bits) __annotation(L"TMC:", WPP_GUID_WTEXT Guid, _WPPW(WPP_STRINGIZE(Name)) Bits);
#  define WPP_DEFINE_BIT(Name) , _WPPW(#Name)
#else
#  define WPP_DEFINE_CONTROL_GUID(Name,Guid,Bits) __annotation("TMC:", WPP_GUID_TEXT Guid, WPP_STRINGIZE(Name) Bits);
#  define WPP_DEFINE_BIT(Name) , #Name
#endif
    WPP_CONTROL_GUIDS 
#  undef WPP_DEFINE_BIT
#  undef WPP_DEFINE_CONTROL_GUID
#endif
}


#define WPP_NEXT(Name) ((WPP_TRACE_CONTROL_BLOCK*) \
    (WPP_XGLUE(WPP_CTL_, WPP_EVAL(Name)) + 1 == WPP_LAST_CTL ? 0:WPP_MAIN_CB + WPP_XGLUE(WPP_CTL_, WPP_EVAL(Name)) + 1))    


WPP_CB_TYPE WPP_MAIN_CB[WPP_LAST_CTL];

__inline void WPP_INIT_CONTROL_ARRAY(WPP_CB_TYPE* Arr) {
#define WPP_DEFINE_CONTROL_GUID(Name,Guid,Bits)                                         \
   Arr->Control.Callback = NULL;                                                        \
   Arr->Control.ControlGuid = WPP_XGLUE4(&WPP_, ThisDir, _CTLGUID_, WPP_EVAL(Name));    \
   Arr->Control.Next = WPP_NEXT(WPP_EVAL(Name));                                        \
   Arr->Control.RegistryPath= NULL;                                                     \
   Arr->Control.FlagsLen = WPP_FLAG_LEN;                                                \
   Arr->Control.Level = 0;                                                               \
   Arr->Control.Reserved = 0;                                                           \
   Arr->Control.Flags[0] = 0;                                                           \
   ++Arr;
#define WPP_DEFINE_BIT(BitName) L" " L ## #BitName
WPP_CONTROL_GUIDS
#undef WPP_DEFINE_BIT
#undef WPP_DEFINE_CONTROL_GUID
}


#undef WPP_INIT_STATIC_DATA
#define WPP_INIT_STATIC_DATA WPP_INIT_CONTROL_ARRAY(WPP_MAIN_CB)


ULONG_PTR   WPP_Global_NextDeviceOffsetInDeviceExtension =  (ULONG_PTR)-1;

// define WPP_INIT_TRACING.  For performance reasons turn off during
// static analysis compilation with Static Driver Verifier (SDV).
#ifndef SDV
#define WPP_INIT_TRACING(DrvObj, RegPath)                                   \
    {                                                                       \
      WppDebug(0,("WPP_INIT_TRACING: &WPP_CB[0] %p\n", &WPP_MAIN_CB[0]));   \
      WPP_INIT_STATIC_DATA;                                                 \
      WppLoadTracingSupport();                                              \
      ( WPP_CONTROL_ANNOTATION(),                                           \
        WPP_MAIN_CB[0].Control.RegistryPath = NULL,                         \
        WppInitKm( (PDEVICE_OBJECT)DrvObj, RegPath )                        \
      );                                                                    \
    }
#else
#define WPP_INIT_TRACING(DrvObj, RegPath)
#endif
    
#define WMIREG_FLAG_CALLBACK  0x80000000 // not exposed in DDK

#ifndef WMIREG_FLAG_TRACE_PROVIDER
#define WMIREG_FLAG_TRACE_PROVIDER          0x00010000
#endif

#ifndef WPP_POOLTAG
#define WPP_POOLTAG     'ECTS'
#endif

#ifndef WPP_POOLTYPE
#define WPP_POOLTYPE PagedPool
#endif

#ifndef WPP_MAX_MOF_FIELDS
#define WPP_MAX_MOF_FIELDS 7
#endif

#ifndef TRACE_MESSAGE_MAXIMUM_SIZE
#define TRACE_MESSAGE_MAXIMUM_SIZE  8*1024
#endif

typedef struct _WPP_TRACE_BUFFER {
    EVENT_TRACE_HEADER Header;
    MOF_FIELD MofField[WPP_MAX_MOF_FIELDS+1];
} WPP_TRACE_BUFFER;


NTSTATUS 
W2kTraceMessage(__in TRACEHANDLE  LoggerHandle, 
                __in ULONG TraceOptions, 
                __in LPCGUID MessageGuid, 
                __in USHORT MessageNumber, 
                     ...
                )
/*++

Routine Description:

    This is a function that simulates tracemessage in W2K
    
Arguments:

    LoggerHandle - handle providers logger handle
    
    TraceOptions - unreferenced
        
    MessageGuid - pointer to message GUID 
    
    MessageNumber - Type of message been logged
    
    ...         - list or arguments
        
Return Value:

    NTSTATUS code indicating success or failure
    
--*/
                
{
    size_t              ByteCount;
    size_t              ArgumentCount;
    va_list             VarArgs;
    PVOID               DataPtr;
    size_t              DataSize = 0;
    size_t              ArgOffset;
    NTSTATUS            Status = STATUS_SUCCESS;
    WPP_TRACE_BUFFER    TraceBuf;
    PVOID               Data=NULL;

    UNREFERENCED_PARAMETER(TraceOptions); // unused

    RtlZeroMemory(&(TraceBuf.Header), sizeof(EVENT_TRACE_HEADER));

    //Fill in header fields
    ((PWNODE_HEADER)&(TraceBuf.Header))->HistoricalContext = LoggerHandle;
    TraceBuf.Header.GuidPtr = (ULONG_PTR)MessageGuid;
    TraceBuf.Header.Flags = WNODE_FLAG_TRACED_GUID |WNODE_FLAG_USE_GUID_PTR|WNODE_FLAG_USE_MOF_PTR;
    //Type is 0xFF to indicate that the first data is the MessageNumber
    TraceBuf.Header.Class.Type = 0xFF;

    //The first Mof data is the Message Number
    TraceBuf.MofField[0].DataPtr = (ULONG_PTR)&MessageNumber;
    TraceBuf.MofField[0].Length = sizeof(USHORT);


    // Determine the number bytes to follow header
    ByteCount = 0;               // For Count of Bytes
    ArgumentCount = 0;           // For Count of Arguments
    va_start(VarArgs, MessageNumber);
    
    while ((DataPtr = va_arg (VarArgs, PVOID)) != NULL)
    {
        DataSize = va_arg (VarArgs, size_t);
        ByteCount += DataSize;
        ArgumentCount++;
        if (ArgumentCount <= WPP_MAX_MOF_FIELDS)
        {
              TraceBuf.MofField[ArgumentCount].DataPtr = (ULONG_PTR)DataPtr;
              TraceBuf.MofField[ArgumentCount].Length = (ULONG)DataSize;
        }
    }

    va_end(VarArgs);

    if (ByteCount > TRACE_MESSAGE_MAXIMUM_SIZE) 
    {
            return STATUS_UNSUCCESSFUL;
    }

    //This occurs infrequently
    if (ArgumentCount > WPP_MAX_MOF_FIELDS)
    {
        //Allocate the blob to hold the data
        Data = ExAllocatePoolWithTag(WPP_POOLTYPE,ByteCount,WPP_POOLTAG);
        if (Data == NULL) 
        {
            return STATUS_NO_MEMORY;
        }

        TraceBuf.MofField[1].DataPtr = (ULONG_PTR)Data;
        TraceBuf.MofField[1].Length = (ULONG)ByteCount;

        ArgOffset = 0;
        DataSize = 0; 
        va_start(VarArgs, MessageNumber);

        while ((DataPtr = va_arg (VarArgs, PVOID)) != NULL)    {
            DataSize = va_arg (VarArgs, size_t);
            RtlCopyMemory((char*)Data + ArgOffset, DataPtr, DataSize);
            ArgOffset += DataSize;
        }
        
        va_end(VarArgs);

        //Fill in the total size (header + 2 mof fields)
        TraceBuf.Header.Size = (USHORT)(sizeof(EVENT_TRACE_HEADER) + 2*sizeof(MOF_FIELD));

    }
    else
    {
        //Fill in the total size (header + mof fields)
        TraceBuf.Header.Size = (USHORT)(sizeof(EVENT_TRACE_HEADER) + (ArgumentCount+1)*sizeof(MOF_FIELD));
    }

    
    Status = IoWMIWriteEvent(&TraceBuf.Header);

    if (Data) ExFreePool(Data);

    return Status;
}

//
// Public routines to break down the Loggerhandle
//

#if !defined(KERNEL_LOGGER_ID)
#define KERNEL_LOGGER_ID                      0xFFFF    // USHORT only 
#endif

typedef struct _WPP_TRACE_ENABLE_CONTEXT {
    USHORT  LoggerId;           // Actual Id of the logger
    UCHAR   Level;              // Enable level passed by control caller
    UCHAR   InternalFlag;       // Reserved
    ULONG   EnableFlags;        // Enable flags passed by control caller
} WPP_TRACE_ENABLE_CONTEXT, *PWPP_TRACE_ENABLE_CONTEXT;

#if !defined(WmiGetLoggerId)
#define WmiGetLoggerId(LoggerContext) \
    (((PWPP_TRACE_ENABLE_CONTEXT) (&LoggerContext))->LoggerId == \
        (USHORT)KERNEL_LOGGER_ID) ? \
        KERNEL_LOGGER_ID : \
        ((PWPP_TRACE_ENABLE_CONTEXT) (&LoggerContext))->LoggerId

#define WmiGetLoggerEnableFlags(LoggerContext) \
   ((PWPP_TRACE_ENABLE_CONTEXT) (&LoggerContext))->EnableFlags
#define WmiGetLoggerEnableLevel(LoggerContext) \
    ((PWPP_TRACE_ENABLE_CONTEXT) (&LoggerContext))->Level
#endif

#ifndef WPPINIT_EXPORT
#define WPPINIT_EXPORT
#endif

#define WppIsEqualGuid(G1, G2)(RtlCompareMemory(G1, G2, sizeof(GUID)) == sizeof(GUID))


VOID
WppLoadTracingSupport(
    VOID
    )
/*++

Routine Description:

    This function assigns at runtime the ETW API set to be use for tracing.
    
Arguments:
    
Remarks:

    At runtime determine assing the funtions pointers for the trace APIs to be use. 
    XP and above will use TraceMessage, and Win2K will use our private W2kTraceMessage
    which uses TraceEvent

--*/
{
    ULONG MajorVersion = 0;
    UNICODE_STRING name;

    PAGED_CODE();

    RtlInitUnicodeString(&name, L"PsGetVersion");       
    pfnWppGetVersion = (PFN_WPPGETVERSION) (INT_PTR) 
        MmGetSystemRoutineAddress(&name);

    RtlInitUnicodeString(&name, L"WmiTraceMessage");
    pfnWppTraceMessage = (PFN_WPPTRACEMESSAGE) (INT_PTR) 
        MmGetSystemRoutineAddress(&name);

    //
    // Win2k
    //
    if (pfnWppTraceMessage == NULL) {
        //
        // Subsitute the W2K verion of WmiTraceMessage
        //
        pfnWppTraceMessage = W2kTraceMessage;
        WPPTraceSuite = WppTraceWin2K;

        WppDebug(0,("WppLoadTracingSupport:subsitute pWmiTraceMessage %p\n",
                    pfnWppTraceMessage));

        return;
    }

    //
    // WinXp
    //
    RtlInitUnicodeString(&name, L"WmiQueryTraceInformation");
    pfnWppQueryTraceInformation = (PFN_WPPQUERYTRACEINFORMATION) (INT_PTR) 
        MmGetSystemRoutineAddress(&name);
    WPPTraceSuite = WppTraceWinXP;

    //
    // Server08
    //

    if (pfnWppGetVersion != NULL) {
        pfnWppGetVersion(&MajorVersion,
                         NULL,
                         NULL,
                         NULL);
    }

    if (MajorVersion >= 6) {

        RtlInitUnicodeString(&name, L"EtwRegisterClassicProvider");
        pfnEtwRegisterClassicProvider = (PFN_ETWREGISTERCLASSICPROVIDER) (INT_PTR) 
            MmGetSystemRoutineAddress(&name);

        if (pfnEtwRegisterClassicProvider != NULL) {
            //
            // For Vista SP1 and later
            //
            RtlInitUnicodeString(&name, L"EtwUnregister");
            pfnEtwUnregister = (PFN_ETWUNREGISTER) (INT_PTR) 
                MmGetSystemRoutineAddress(&name);
        
            WPPTraceSuite = WppTraceServer08;
        }
    }
}


#ifdef WPP_GLOBALLOGGER
#define DEFAULT_GLOBAL_LOGGER_KEY       L"WMI\\GlobalLogger\\"
#define WPP_TEXTGUID_LEN 38
#define GREGVALUENAMELENGTH (18 + WPP_TEXTGUID_LEN) // wslen(L"WMI\\GlobalLogger\\") + GUIDLENGTH

WPPINIT_EXPORT
void WppInitGlobalLogger(
        __in LPCGUID ControlGuid,
        __out PTRACEHANDLE LoggerHandle,
        __out PULONG Flags,
        __out PUCHAR Level 
        )
{
WCHAR                      GRegValueName[GREGVALUENAMELENGTH]; 
RTL_QUERY_REGISTRY_TABLE   Parms[3];
ULONG                      CurrentFlags = 0;
ULONG                      CurrentLevel = 0;
ULONG                      Start = 0;
NTSTATUS                   Status;
ULONG                      Zero = 0;
UNICODE_STRING             GuidString;  


   PAGED_CODE();
    
   WppDebug(0,("WPP checking Global Logger\n"));
   

   //
   // Fill in the query table to find out if the Global Logger is Started
   //
   // Trace Flags
      Parms[0].QueryRoutine  = NULL;
      Parms[0].Flags         = RTL_QUERY_REGISTRY_DIRECT;
      Parms[0].Name          = L"Start";
      Parms[0].EntryContext  = &Start;
      Parms[0].DefaultType   = REG_DWORD;
      Parms[0].DefaultData   = &Zero;
      Parms[0].DefaultLength = sizeof(ULONG);
      // Termination
      Parms[1].QueryRoutine  = NULL;
      Parms[1].Flags         = 0;
   //
   // Perform the query
   //

   Status = RtlQueryRegistryValues(RTL_REGISTRY_CONTROL | RTL_REGISTRY_OPTIONAL,
                                   DEFAULT_GLOBAL_LOGGER_KEY,
                                   Parms,
                                   NULL,
                                   NULL);
    if (!NT_SUCCESS(Status) || Start == 0 ) {  
        return;
    }

    // Fill in the query table to find out if we should use the Global logger
    //
    // Trace Flags
      Parms[0].QueryRoutine  = NULL;
      Parms[0].Flags         = RTL_QUERY_REGISTRY_DIRECT;
      Parms[0].Name          = L"Flags";
      Parms[0].EntryContext  = &CurrentFlags;
      Parms[0].DefaultType   = REG_DWORD;
      Parms[0].DefaultData   = &Zero;
      Parms[0].DefaultLength = sizeof(ULONG);
      // Trace level
      Parms[1].QueryRoutine  = NULL;
      Parms[1].Flags         = RTL_QUERY_REGISTRY_DIRECT;
      Parms[1].Name          = L"Level";
      Parms[1].EntryContext  = &CurrentLevel;
      Parms[1].DefaultType   = REG_DWORD;
      Parms[1].DefaultData   = &Zero;
      Parms[1].DefaultLength = sizeof(UCHAR);
      // Termination
      Parms[2].QueryRoutine  = NULL;
      Parms[2].Flags         = 0;


      RtlCopyMemory(GRegValueName, DEFAULT_GLOBAL_LOGGER_KEY,  (wcslen(DEFAULT_GLOBAL_LOGGER_KEY)+1) *sizeof(WCHAR));
 
      Status = RtlStringFromGUID(ControlGuid, &GuidString);
      if( Status != STATUS_SUCCESS ) {
        WppDebug(0,("WPP GlobalLogger failed RtlStringFromGUID \n"));
        return;
      }

      if (GuidString.Length > (WPP_TEXTGUID_LEN * sizeof(WCHAR))){
        WppDebug(0,("WPP GlobalLogger RtlStringFromGUID  too large\n"));
        RtlFreeUnicodeString(&GuidString);
        return;
      }
        
      // got the GUID in form "{xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}"   
      // need GUID in form "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
      // copy the translated GUID string 
      
      RtlCopyMemory(&GRegValueName[(ULONG)wcslen(GRegValueName)], &GuidString.Buffer[1], GuidString.Length);
      GRegValueName[(ULONG)wcslen(GRegValueName) - 1] = L'\0';  
      RtlFreeUnicodeString(&GuidString);

   //
   // Perform the query
   //

   Status = RtlQueryRegistryValues(RTL_REGISTRY_CONTROL | RTL_REGISTRY_OPTIONAL,
                                   GRegValueName,
                                   Parms,
                                   NULL,
                                   NULL);
   if (NT_SUCCESS(Status)) {
        if (Start==1) {
           *LoggerHandle= WMI_GLOBAL_LOGGER_ID;
           *Flags = CurrentFlags & 0x7FFFFFFF;
           *Level = (UCHAR)(CurrentLevel & 0xFF);
           WppDebug(0,("WPP Enabled via Global Logger Flags=0x%08X Level=0x%02X\n",CurrentFlags,CurrentLevel));

        }
   } else {
        WppDebug(0,("WPP GlobalLogger has No Flags/Levels Status=%08X\n",Status));
   }                                    
}
#endif  //#ifdef WPP_GLOBALLOGGER

#define WPP_MAX_COUNT_REGISTRATION_GUID 63

WPPINIT_EXPORT
NTSTATUS
WppTraceCallback(
    __in UCHAR MinorFunction,
    __in_opt PVOID DataPath,
    __in ULONG BufferLength,
    __inout_bcount(BufferLength) PVOID Buffer,
    __in PVOID Context,
    __out PULONG Size
    )
/*++

Routine Description:

    This function is the callback WMI calls when we register and when our
    events are enabled or disabled.

Arguments:

    MinorFunction - specifies the type of callback (register, event enable/disable)
    
    DataPath - varies depending on the ActionCode
    
    BufferLength - size of the Buffer parameter
    
    Buffer - in/out buffer where we read from or write to depending on the type
        of callback
        
    Context - the pointer private struct WPP_TRACE_CONTROL_BLOCK
        
    Size - output parameter to receive the amount of data written into Buffer

Return Value:

    NTSTATUS code indicating success/failure

Comments:

    if return value is STATUS_BUFFER_TOO_SMALL and BufferLength >= 4,
    then first ulong of buffer contains required size


--*/

{
    PWPP_TRACE_CONTROL_BLOCK    cntl;
    NTSTATUS                    Status = STATUS_SUCCESS;

    UNREFERENCED_PARAMETER(DataPath);


    PAGED_CODE();

    
    WppDebug(0,("WppTraceCallBack 0x%08X %p\n", MinorFunction, Context));
    
    *Size = 0;

    switch(MinorFunction)
    {
        case IRP_MN_REGINFO:
        {
            PWMIREGINFOW     WmiRegInfo;
            PCUNICODE_STRING RegPath;
            PWCHAR           StringPtr;
            ULONG            RegistryPathOffset;
            ULONG            BufferNeeded;
            ULONG            GuidCount = 0;

            //
            // Initialize locals 
            //

            cntl = (PWPP_TRACE_CONTROL_BLOCK)Context;
            WmiRegInfo = (PWMIREGINFO)Buffer;
            
            if (WppTraceWin2K == WPPTraceSuite && WmiRegInfo->GuidCount > 1) {
                
                //
                // Calculate buffer size need to hold all info.
                //
                BufferNeeded = FIELD_OFFSET(WMIREGINFOW, WmiRegGuid) + 
                               WmiRegInfo->GuidCount * sizeof(WMIREGGUIDW);
                
                if ( BufferNeeded > BufferLength) {
                    Status = STATUS_BUFFER_TOO_SMALL;
                    if (BufferLength >= sizeof(ULONG)) {
                        *((PULONG)Buffer) = BufferNeeded;
                        *Size = sizeof(ULONG);
                    }
                    break;
                }
                
                //
                // Deal with the WMI + WPP registration case can only handle 
                // One Tracing CONTROL GUID, at the last poscition
                //

                WmiRegInfo->WmiRegGuid[WmiRegInfo->GuidCount - 1].Guid  = *cntl->ControlGuid;  
                WmiRegInfo->WmiRegGuid[WmiRegInfo->GuidCount - 1].Flags = 
                WMIREG_FLAG_TRACE_CONTROL_GUID | WMIREG_FLAG_TRACED_GUID;
                cntl->Level = 0;
                cntl->Flags[0] = 0;
#ifdef WPP_GLOBALLOGGER                
                WppInitGlobalLogger( cntl->ControlGuid, 
                    (PTRACEHANDLE)&cntl->Logger, 
                    &cntl->Flags[0], 
                    &cntl->Level
                    );
#endif
                break;
            }
            
            RegPath = cntl->RegistryPath;

            //
            // Count the number of guid to be identified.
            //
            while(cntl) { GuidCount++; cntl = cntl->Next; }
            
            if (GuidCount > WPP_MAX_COUNT_REGISTRATION_GUID){
                Status = STATUS_INVALID_PARAMETER;
                break;            
            }
            
            WppDebug(0,("WppTraceCallBack: GUID count %d\n", GuidCount)); 

            //
            // Calculate buffer size need to hold all info.
            // Calculate offset to where RegistryPath parm will be copied.
            //
            
            if (RegPath == NULL)
            {

                RegistryPathOffset = 0;

                BufferNeeded = FIELD_OFFSET(WMIREGINFOW, WmiRegGuid) + 
                               GuidCount * sizeof(WMIREGGUIDW);
                
            } else {

                RegistryPathOffset = FIELD_OFFSET(WMIREGINFOW, WmiRegGuid) + 
                                     GuidCount * sizeof(WMIREGGUIDW);

                BufferNeeded = RegistryPathOffset +
                               RegPath->Length + sizeof(USHORT);
            }            

            //
            // If the provided buffer is large enough, then fill with info.
            //
            
            if (BufferNeeded <= BufferLength)
            {
                ULONG  i;

                RtlZeroMemory(Buffer, BufferLength);

                //
                // Fill in the WMIREGINFO
                //
                
                WmiRegInfo->BufferSize   = BufferNeeded;
                WmiRegInfo->RegistryPath = RegistryPathOffset;
                WmiRegInfo->GuidCount    = GuidCount;

                if (RegPath != NULL) {
                    StringPtr    = (PWCHAR)((PUCHAR)Buffer + RegistryPathOffset);
                    *StringPtr++ = RegPath->Length;
                    
                    RtlCopyMemory(StringPtr, RegPath->Buffer, RegPath->Length);
                }

                //
                // Fill in the WMIREGGUID
                //

                cntl = (PWPP_TRACE_CONTROL_BLOCK) Context;

                for (i=0; i<GuidCount; i++) {

                    WmiRegInfo->WmiRegGuid[i].Guid  = *cntl->ControlGuid;  
                    WmiRegInfo->WmiRegGuid[i].Flags = WMIREG_FLAG_TRACE_CONTROL_GUID | 
                                                      WMIREG_FLAG_TRACED_GUID;
                    cntl->Level = 0;
                    cntl->Flags[0] = 0;
                    WppDebug(0,("Control GUID::%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x\n",
                                cntl->ControlGuid->Data1,
                                cntl->ControlGuid->Data2,
                                cntl->ControlGuid->Data3,
                                cntl->ControlGuid->Data4[0],
                                cntl->ControlGuid->Data4[1],
                                cntl->ControlGuid->Data4[2],
                                cntl->ControlGuid->Data4[3],
                                cntl->ControlGuid->Data4[4],
                                cntl->ControlGuid->Data4[5],
                                cntl->ControlGuid->Data4[6],
                                cntl->ControlGuid->Data4[7]
                        )); 
                    
                    cntl = cntl->Next;
                }

                Status = STATUS_SUCCESS;
                *Size  = BufferNeeded;

            } else {
                Status = STATUS_BUFFER_TOO_SMALL;

                if (BufferLength >= sizeof(ULONG)) {
                    *((PULONG)Buffer) = BufferNeeded;
                    *Size = sizeof(ULONG);
                }
            }

#ifdef WPP_GLOBALLOGGER
            // Check if Global logger is active            
            
            cntl = (PWPP_TRACE_CONTROL_BLOCK) Context;
            while(cntl) {
                WppInitGlobalLogger(
                                    cntl->ControlGuid,
                                    (PTRACEHANDLE)&cntl->Logger,
                                    &cntl->Flags[0],
                                    &cntl->Level);
                cntl = cntl->Next;                    
            }
#endif  //#ifdef WPP_GLOBALLOGGER
            
            break;
        }

        case IRP_MN_ENABLE_EVENTS:
        case IRP_MN_DISABLE_EVENTS:
        {
            PWNODE_HEADER             Wnode;
            ULONG                     Level;
            ULONG                     ReturnLength;
            ULONG                     index;

            if (Context == NULL ) {
                Status = STATUS_WMI_GUID_NOT_FOUND;
                break;
            }

            if (BufferLength < sizeof(WNODE_HEADER)) {
                Status = STATUS_INVALID_PARAMETER;
                break;
            }

            //
            // Initialize locals 
            //
            Wnode = (PWNODE_HEADER)Buffer;
            
            //
            // Traverse this ProjectControlBlock's ControlBlock list and 
            // find the "cntl" ControlBlock which matches the Wnode GUID.
            //
            cntl  = (PWPP_TRACE_CONTROL_BLOCK) Context;
            index = 0;
            while(cntl) { 
                if (WppIsEqualGuid(cntl->ControlGuid, &Wnode->Guid )) {
                    break;
                }
                index++;
                cntl = cntl->Next; 
            }

            if (cntl == NULL) {
                Status = STATUS_WMI_GUID_NOT_FOUND;
                break;
            }

            //
            // Do the requested event action
            //
            Status = STATUS_SUCCESS;

            if (MinorFunction == IRP_MN_DISABLE_EVENTS) {

                WppDebug(0,("WppTraceCallBack: DISABLE_EVENTS\n")); 

                cntl->Level    = 0;
                cntl->Flags[0] = 0;
                cntl->Logger   = 0;

            } else {

                TRACEHANDLE  lh;

                lh = (TRACEHANDLE)( Wnode->HistoricalContext );
                cntl->Logger = lh;

                if (WppTraceWinXP == WPPTraceSuite) {

                    Status = pfnWppQueryTraceInformation( TraceEnableLevelClass,
                                                          &Level,
                                                          sizeof(Level),
                                                          &ReturnLength,
                                                          (PVOID) Wnode );

                    if (Status == STATUS_SUCCESS) {
                        cntl->Level = (UCHAR)Level;
                    }

                    Status = pfnWppQueryTraceInformation( TraceEnableFlagsClass,
                                                          &cntl->Flags[0],
                                                          sizeof(cntl->Flags[0]),
                                                          &ReturnLength,
                                                          (PVOID) Wnode );

                } else {
                    cntl->Flags[0] = ((PWPP_TRACE_ENABLE_CONTEXT) &lh)->EnableFlags;
                    cntl->Level = (UCHAR) ((PWPP_TRACE_ENABLE_CONTEXT) &lh)->Level;
                }

                WppDebug(0,("WppTraceCallBack: ENABLE_EVENTS "
                            "LoggerId %d, Flags 0x%08X, Level 0x%02X\n",
                            (USHORT) cntl->Logger,
                            cntl->Flags[0],
                            cntl->Level));

            }

#ifdef WPP_PRIVATE_ENABLE_CALLBACK
            //
            // Notify changes to flags, level for GUID
            //
                WPP_PRIVATE_ENABLE_CALLBACK( cntl->ControlGuid, 
                                             cntl->Logger, 
                                             (MinorFunction != IRP_MN_DISABLE_EVENTS) ? TRUE:FALSE,
                                             cntl->Flags[0],
                                             cntl->Level );
#endif 

            break;
        }
        
        case IRP_MN_ENABLE_COLLECTION:
        case IRP_MN_DISABLE_COLLECTION:
        {
            Status = STATUS_SUCCESS;
            break;
        }

        case IRP_MN_QUERY_ALL_DATA:
        case IRP_MN_QUERY_SINGLE_INSTANCE:
        case IRP_MN_CHANGE_SINGLE_INSTANCE:
        case IRP_MN_CHANGE_SINGLE_ITEM:
        case IRP_MN_EXECUTE_METHOD:
        {
            Status = STATUS_INVALID_DEVICE_REQUEST;
            break;
        }

        default:
        {
            Status = STATUS_INVALID_DEVICE_REQUEST;
            break;
        }

    }
    return(Status);
}

VOID 
NTAPI
WppClassicProviderCallback(
    __in LPCGUID Guid,
    __in UCHAR ControlCode,
    __in PVOID EnableContext,
    __in_opt PVOID CallbackContext
    )

/*++

Routine Description:

    Enable callback function when EtwRegisterClassicProvider was used.
    It happens in Windows Vista SP1 and newer.

Arguments:

    Guid - provider guid.

    ControlCode -  code indicating operations request.

    EnableContext - context from the ETW infrastructure.

    CallbackContext - context from the user.

Return Value:

    None.

--*/

{
    PWPP_TRACE_CONTROL_BLOCK TraceCb = (PWPP_TRACE_CONTROL_BLOCK)CallbackContext;
    PWPP_TRACE_ENABLE_CONTEXT TraceContext = (PWPP_TRACE_ENABLE_CONTEXT)EnableContext;
    
    UNREFERENCED_PARAMETER (Guid);

    WppDebug(0,("WppClassicProviderCallback %d\n", (int)ControlCode));

    //
    // Only handle enable and disable operations.
    //

    if ((ControlCode != EVENT_CONTROL_CODE_ENABLE_PROVIDER) &&
        (ControlCode != EVENT_CONTROL_CODE_DISABLE_PROVIDER)) {
        
        return;
    }

    if (ControlCode != EVENT_CONTROL_CODE_DISABLE_PROVIDER) {
        TraceCb->Flags[0] = TraceContext->EnableFlags;
        TraceCb->Level = (UCHAR)TraceContext->Level;
        TraceCb->Logger = *((TRACEHANDLE*)TraceContext);

        WppDebug(0,("ENABLE: LoggerId=%d Flags=%08x Level=%02d\n", (int)TraceContext->LoggerId, TraceCb->Flags[0], TraceCb->Level));
    } else {
        TraceCb->Level = 0;
        TraceCb->Flags[0] = 0;
        TraceCb->Logger = 0;
    }

#ifdef WPP_PRIVATE_ENABLE_CALLBACK
    //
    // Notify changes to flags, level for GUID
    //
    WppDebug(0,("WppClassicProviderCallback: calling private callback.\n"));

    WPP_PRIVATE_ENABLE_CALLBACK(TraceCb->ControlGuid, 
                                TraceCb->Logger, 
                                ControlCode,
                                TraceCb->Flags[0],
                                TraceCb->Level);
#endif

}
    

#pragma warning(push)
#pragma warning(disable:4068)
WPPINIT_EXPORT
VOID 
WppInitKm(
    __in_opt PDEVICE_OBJECT DevObject,
    __in_opt PCUNICODE_STRING RegPath
    )

/*++

Routine Description:

    This function registers a driver with ETW as a provider of trace
    events from the defined GUIDs.
    
Arguments:

    DevObject - Optional pointer to a device object object, neede only for W2K.

    RegPath - Optional pointer to registry path, needed for wpp recorder.
    
Remarks:

   This function is called by the WPP_INIT_TRACING(DrvObj, RegPath) macro, for XP and 
   above the DrvObj can be NULL.
    
--*/
    
{
    C_ASSERT(WPP_MAX_FLAG_LEN_CHECK); 

    NTSTATUS Status;
    PWPP_TRACE_CONTROL_BLOCK WppReg = NULL;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(RegPath);                                      \
    
    if (WPP_CB != WPP_MAIN_CB) {

        WPP_CB = WPP_MAIN_CB;

    } else {
      //
      // WPP_INIT_TRACING allready called
      //
      WppDebug(0,("Warning : WPP_INIT_TRACING already called, ignoring this one"));
      return;
    }

    WppReg = &WPP_CB[0].Control;

    WppDebug(0,("WPP Init.\n"));

    if (WppTraceServer08 == WPPTraceSuite) {

        //
        // Windows version >= Vista SP1
        //      
        while (WppReg) {

            WppReg->RegHandle = 0;
            Status = pfnEtwRegisterClassicProvider(
                WppReg->ControlGuid, 
                0, 
                WppClassicProviderCallback, 
                (PVOID)WppReg, 
                &WppReg->RegHandle);

            if (!NT_SUCCESS(Status)) {
                WppDebug(0,("EtwRegisterClassicProvider Status = %d, ControlBlock = %p.\n", Status, WppReg));
            }
            
            WppReg = WppReg->Next;
        }        

    } else if (WppTraceWinXP == WPPTraceSuite) {

        
        WppReg -> Callback = WppTraceCallback;

#pragma prefast(suppress:__WARNING_BANNED_API_ARGUMENT_USAGE, "WPP generated, requires legacy providers");
        Status = IoWMIRegistrationControl(
                                    (PDEVICE_OBJECT)WppReg,
                                    WMIREG_ACTION_REGISTER  | 
                                    WMIREG_FLAG_CALLBACK    |
                                    WMIREG_FLAG_TRACE_PROVIDER
                                    );        

        if (!NT_SUCCESS(Status)) {
            WppDebug(0,("IoWMIRegistrationControl Status = %08X\n",Status));
        }
        
    } else {

#ifdef WDF_WPP_KMDF_DRIVER
#pragma prefast(suppress:__WARNING_CLASS_MISMATCH_NONE, "Function uses a KMDF specific roletype");
        Status = WdfDriverRegisterTraceInfo((PDRIVER_OBJECT)DevObject, 
                                            WppTraceCallback, 
                                            WppReg);
        if (!NT_SUCCESS(Status)) {
            WppDebug(0,("WdfDriverRegisterTraceInfo failed Status = %08X\n",Status));
        }
#else
        WppReg -> Callback = NULL;

        Status = IoWMIRegistrationControl(DevObject,
                                          WMIREG_ACTION_REGISTER
                                         );
        if (!NT_SUCCESS(Status)) {
            WppDebug(0,("IoWMIRegistrationControl Status = %08X\n",Status));
        }
#endif
    }                         

#if ENABLE_WPP_RECORDER
    WppAutoLogStart((PDRIVER_OBJECT)DevObject, RegPath);
#endif

}

WPPINIT_EXPORT
VOID 
WppCleanupKm(
    __in_opt PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    This function deregisters a driver from ETW as provider of trace
    events.
    
Arguments:

    DevObject - Optional pointer to a device object object, neede only for W2K. 

Remarks:

    This is the defined WPP_CLEANUP(DrvObj) macro, and the macro can
    take NULL as an argument for XP and above.
        
--*/

{
    PAGED_CODE();
    
    if (WPP_CB == (WPP_CB_TYPE*)&WPP_CB){
        //
        // WPP_INIT_TRACING macro has not been called
        //
        WppDebug(0,("Warning : WPP_CLEANUP already called, or called with out WPP_INIT_TRACING first"));
        return;
    }

    if (WppTraceServer08 == WPPTraceSuite) {

        PWPP_TRACE_CONTROL_BLOCK WppReg = &WPP_CB[0].Control;

        while (WppReg) {
            if (WppReg->RegHandle) {
                pfnEtwUnregister(WppReg->RegHandle);    
                WppDebug(0,("EtwUnregister RegHandle = %lld.\n",WppReg->RegHandle));
            } else {
                WppDebug(0,("WppCleanupKm: invalid RegHandle.\n"));
            }
            WppReg = WppReg->Next;
        }

    } else if (WppTraceWinXP == WPPTraceSuite) {
        PWPP_TRACE_CONTROL_BLOCK WppReg = &WPP_CB[0].Control;

        IoWMIRegistrationControl(   (PDEVICE_OBJECT)WppReg, 
                                    WMIREG_ACTION_DEREGISTER | 
                                    WMIREG_FLAG_CALLBACK );
                                    
    } else {
#ifndef WDF_WPP_KMDF_DRIVER  
        IoWMIRegistrationControl(DeviceObject, WMIREG_ACTION_DEREGISTER );
#else
        UNREFERENCED_PARAMETER(DeviceObject);
        //
        // WDF does the job of unregistering the device and deleting it
        // when the driver is unloaded. So we don't do anything in the
        // WPP_CLEANUP call.
        //
#endif    
    }
    
#if ENABLE_WPP_RECORDER    
        WppAutoLogStop((PDRIVER_OBJECT)DeviceObject);
#endif    

    WPP_CB = (WPP_CB_TYPE*)&WPP_CB;
}

#pragma warning(pop)

#define WPP_SYSTEMCONTROL(PDO)                                                      \
        if(WPPTraceSuite == WppTraceDisabledSuite){                                 \
            WppLoadTracingSupport();                                                \
        }                                                                           \
        if(WPPTraceSuite == WppTraceWin2K){                                         \
            PDO->MajorFunction[ IRP_MJ_SYSTEM_CONTROL ] = WPPSystemControlDispatch; \
        }
        
#define WPP_SYSTEMCONTROL2(PDO, offset)                                             \
        if(WPPTraceSuite == WppTraceDisabledSuite){                                 \
            WppLoadTracingSupport();                                                \
        }                                                                           \
        if(WPPTraceSuite == WppTraceWin2K){ \
            WPP_SYSTEMCONTROL(PDO); WPP_Global_NextDeviceOffsetInDeviceExtension = (ULONG_PTR)offset; \
        }



// Routine to handle the System Control in W2K
NTSTATUS
WPPSystemControlDispatch(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp
    )
{

    PIO_STACK_LOCATION irpSp = IoGetCurrentIrpStackLocation(Irp);
    ULONG BufferSize = irpSp->Parameters.WMI.BufferSize;
    PVOID Buffer = irpSp->Parameters.WMI.Buffer;
    ULONG ReturnSize = 0;
    NTSTATUS Status = STATUS_SUCCESS;

    PAGED_CODE();
    
    WppDebug(0,("WPPSYSTEMCONTROL\n"));

    if (DeviceObject == (PDEVICE_OBJECT)irpSp->Parameters.WMI.ProviderId) {
        if (irpSp->MinorFunction == IRP_MN_REGINFO) {
            RtlZeroMemory(Buffer, BufferSize);
        }
        Status = WppTraceCallback((UCHAR)(irpSp->MinorFunction),
                                     NULL,
                                     BufferSize,
                                     Buffer,
                                     &WPP_CB[0],
                                     &ReturnSize
                                    );

        WppDebug(0,("WPPSYSTEMCONTROL Status 0x%08X\n",Status));

        Irp->IoStatus.Status = Status;
        Irp->IoStatus.Information = ReturnSize;
        IoCompleteRequest( Irp, IO_NO_INCREMENT );
        return Status;
    } else if (WPP_Global_NextDeviceOffsetInDeviceExtension != -1) {

        ULONG_PTR t;

        WppDebug(0,("WPPSYSTEMCONTROL - not for us\n"));

        //
        // Set current stack back one.
        //
        IoSkipCurrentIrpStackLocation( Irp );
        //
        // Pass the call to the next driver.
        //
        t = (ULONG_PTR)DeviceObject->DeviceExtension;
        t += WPP_Global_NextDeviceOffsetInDeviceExtension;
        return IoCallDriver(*(PDEVICE_OBJECT*)t,Irp);

    } else {
        //
        // Unable to pass down 
        //
        Status = Irp->IoStatus.Status;

        WppDebug(0,("WPPSYSTEMCONTROL: fail to pass down Irp Status 0x%08X\n",Status));

        IoCompleteRequest( Irp, IO_NO_INCREMENT );
        return Status; 
    }
}

#if defined(__cplusplus)
};
#endif

