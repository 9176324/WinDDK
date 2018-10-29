/************************************************************************
*                                                                       *
*   winbase.h -- This module defines the 32-Bit Windows Base APIs       *
*                                                                       *
*   Copyright (c) Microsoft Corp. All rights reserved.                  *
*                                                                       *
************************************************************************/
#ifndef _WINBASE_
#define _WINBASE_


#if _MSC_VER > 1000
#pragma once
#endif

#ifdef _MAC
#include <macwin32.h>
#endif //_MAC

//
// Define API decoration for direct importing of DLL references.
//

#if !defined(_ADVAPI32_)
#define WINADVAPI DECLSPEC_IMPORT
#else
#define WINADVAPI
#endif

#if !defined(_KERNEL32_)
#define WINBASEAPI DECLSPEC_IMPORT
#else
#define WINBASEAPI
#endif

#if !defined(_ZAWPROXY_)
#define ZAWPROXYAPI DECLSPEC_IMPORT
#else
#define ZAWPROXYAPI
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Compatibility macros
 */

#define DefineHandleTable(w)            ((w),TRUE)
#define LimitEmsPages(dw)
#define SetSwapAreaSize(w)              (w)
#define LockSegment(w)                  GlobalFix((HANDLE)(w))
#define UnlockSegment(w)                GlobalUnfix((HANDLE)(w))
#define GetCurrentTime()                GetTickCount()

#define Yield()

#define INVALID_HANDLE_VALUE ((HANDLE)(LONG_PTR)-1)
#define INVALID_FILE_SIZE ((DWORD)0xFFFFFFFF)
#define INVALID_SET_FILE_POINTER ((DWORD)-1)
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)

#define FILE_BEGIN           0
#define FILE_CURRENT         1
#define FILE_END             2

#define TIME_ZONE_ID_INVALID ((DWORD)0xFFFFFFFF)

#define WAIT_FAILED ((DWORD)0xFFFFFFFF)
#define WAIT_OBJECT_0       ((STATUS_WAIT_0 ) + 0 )

#define WAIT_ABANDONED         ((STATUS_ABANDONED_WAIT_0 ) + 0 )
#define WAIT_ABANDONED_0       ((STATUS_ABANDONED_WAIT_0 ) + 0 )

#define WAIT_IO_COMPLETION                  STATUS_USER_APC
#define STILL_ACTIVE                        STATUS_PENDING
#define EXCEPTION_ACCESS_VIOLATION          STATUS_ACCESS_VIOLATION
#define EXCEPTION_DATATYPE_MISALIGNMENT     STATUS_DATATYPE_MISALIGNMENT
#define EXCEPTION_BREAKPOINT                STATUS_BREAKPOINT
#define EXCEPTION_SINGLE_STEP               STATUS_SINGLE_STEP
#define EXCEPTION_ARRAY_BOUNDS_EXCEEDED     STATUS_ARRAY_BOUNDS_EXCEEDED
#define EXCEPTION_FLT_DENORMAL_OPERAND      STATUS_FLOAT_DENORMAL_OPERAND
#define EXCEPTION_FLT_DIVIDE_BY_ZERO        STATUS_FLOAT_DIVIDE_BY_ZERO
#define EXCEPTION_FLT_INEXACT_RESULT        STATUS_FLOAT_INEXACT_RESULT
#define EXCEPTION_FLT_INVALID_OPERATION     STATUS_FLOAT_INVALID_OPERATION
#define EXCEPTION_FLT_OVERFLOW              STATUS_FLOAT_OVERFLOW
#define EXCEPTION_FLT_STACK_CHECK           STATUS_FLOAT_STACK_CHECK
#define EXCEPTION_FLT_UNDERFLOW             STATUS_FLOAT_UNDERFLOW
#define EXCEPTION_INT_DIVIDE_BY_ZERO        STATUS_INTEGER_DIVIDE_BY_ZERO
#define EXCEPTION_INT_OVERFLOW              STATUS_INTEGER_OVERFLOW
#define EXCEPTION_PRIV_INSTRUCTION          STATUS_PRIVILEGED_INSTRUCTION
#define EXCEPTION_IN_PAGE_ERROR             STATUS_IN_PAGE_ERROR
#define EXCEPTION_ILLEGAL_INSTRUCTION       STATUS_ILLEGAL_INSTRUCTION
#define EXCEPTION_NONCONTINUABLE_EXCEPTION  STATUS_NONCONTINUABLE_EXCEPTION
#define EXCEPTION_STACK_OVERFLOW            STATUS_STACK_OVERFLOW
#define EXCEPTION_INVALID_DISPOSITION       STATUS_INVALID_DISPOSITION
#define EXCEPTION_GUARD_PAGE                STATUS_GUARD_PAGE_VIOLATION
#define EXCEPTION_INVALID_HANDLE            STATUS_INVALID_HANDLE
#define EXCEPTION_POSSIBLE_DEADLOCK         STATUS_POSSIBLE_DEADLOCK
#define CONTROL_C_EXIT                      STATUS_CONTROL_C_EXIT

#define MoveMemory RtlMoveMemory
#define CopyMemory RtlCopyMemory
#define FillMemory RtlFillMemory
#define ZeroMemory RtlZeroMemory
#define SecureZeroMemory RtlSecureZeroMemory
#define CaptureStackBackTrace RtlCaptureStackBackTrace

//
// File creation flags must start at the high end since they
// are combined with the attributes
//

#define FILE_FLAG_WRITE_THROUGH         0x80000000
#define FILE_FLAG_OVERLAPPED            0x40000000
#define FILE_FLAG_NO_BUFFERING          0x20000000
#define FILE_FLAG_RANDOM_ACCESS         0x10000000
#define FILE_FLAG_SEQUENTIAL_SCAN       0x08000000
#define FILE_FLAG_DELETE_ON_CLOSE       0x04000000
#define FILE_FLAG_BACKUP_SEMANTICS      0x02000000
#define FILE_FLAG_POSIX_SEMANTICS       0x01000000
#define FILE_FLAG_OPEN_REPARSE_POINT    0x00200000
#define FILE_FLAG_OPEN_NO_RECALL        0x00100000
#define FILE_FLAG_FIRST_PIPE_INSTANCE   0x00080000

#define CREATE_NEW          1
#define CREATE_ALWAYS       2
#define OPEN_EXISTING       3
#define OPEN_ALWAYS         4
#define TRUNCATE_EXISTING   5

#if(_WIN32_WINNT >= 0x0400)
//
// Define possible return codes from the CopyFileEx callback routine
//

#define PROGRESS_CONTINUE   0
#define PROGRESS_CANCEL     1
#define PROGRESS_STOP       2
#define PROGRESS_QUIET      3

//
// Define CopyFileEx callback routine state change values
//

#define CALLBACK_CHUNK_FINISHED         0x00000000
#define CALLBACK_STREAM_SWITCH          0x00000001

//
// Define CopyFileEx option flags
//

#define COPY_FILE_FAIL_IF_EXISTS              0x00000001
#define COPY_FILE_RESTARTABLE                 0x00000002
#define COPY_FILE_OPEN_SOURCE_FOR_WRITE       0x00000004
#define COPY_FILE_ALLOW_DECRYPTED_DESTINATION 0x00000008

//
//  Gap for private copyfile flags
//

#if (_WIN32_WINNT >= 0x0600)
#define COPY_FILE_COPY_SYMLINK                0x00000800
#define COPY_FILE_NO_BUFFERING                0x00001000
#endif
#endif /* _WIN32_WINNT >= 0x0400 */

#if (_WIN32_WINNT >= 0x0500)
//
// Define ReplaceFile option flags
//

#define REPLACEFILE_WRITE_THROUGH       0x00000001
#define REPLACEFILE_IGNORE_MERGE_ERRORS 0x00000002

#if (_WIN32_WINNT >= 0x0600)
#define REPLACEFILE_IGNORE_ACL_ERRORS   0x00000004
#endif

#endif // #if (_WIN32_WINNT >= 0x0500)

//
// Define the NamedPipe definitions
//


//
// Define the dwOpenMode values for CreateNamedPipe
//

#define PIPE_ACCESS_INBOUND         0x00000001
#define PIPE_ACCESS_OUTBOUND        0x00000002
#define PIPE_ACCESS_DUPLEX          0x00000003

//
// Define the Named Pipe End flags for GetNamedPipeInfo
//

#define PIPE_CLIENT_END             0x00000000
#define PIPE_SERVER_END             0x00000001

//
// Define the dwPipeMode values for CreateNamedPipe
//

#define PIPE_WAIT                   0x00000000
#define PIPE_NOWAIT                 0x00000001
#define PIPE_READMODE_BYTE          0x00000000
#define PIPE_READMODE_MESSAGE       0x00000002
#define PIPE_TYPE_BYTE              0x00000000
#define PIPE_TYPE_MESSAGE           0x00000004
#define PIPE_ACCEPT_REMOTE_CLIENTS  0x00000000
#define PIPE_REJECT_REMOTE_CLIENTS  0x00000008

//
// Define the well known values for CreateNamedPipe nMaxInstances
//

#define PIPE_UNLIMITED_INSTANCES    255

//
// Define the Security Quality of Service bits to be passed
// into CreateFile
//

#define SECURITY_ANONYMOUS          ( SecurityAnonymous      << 16 )
#define SECURITY_IDENTIFICATION     ( SecurityIdentification << 16 )
#define SECURITY_IMPERSONATION      ( SecurityImpersonation  << 16 )
#define SECURITY_DELEGATION         ( SecurityDelegation     << 16 )

#define SECURITY_CONTEXT_TRACKING  0x00040000
#define SECURITY_EFFECTIVE_ONLY    0x00080000

#define SECURITY_SQOS_PRESENT      0x00100000
#define SECURITY_VALID_SQOS_FLAGS  0x001F0000

//
//  File structures
//

typedef struct _OVERLAPPED {
    ULONG_PTR Internal;
    ULONG_PTR InternalHigh;
    union {
        struct {
            DWORD Offset;
            DWORD OffsetHigh;
        } DUMMYSTRUCTNAME;
        PVOID Pointer;
    } DUMMYUNIONNAME;

    HANDLE  hEvent;
} OVERLAPPED, *LPOVERLAPPED;

typedef struct _OVERLAPPED_ENTRY {
    ULONG_PTR lpCompletionKey;
    LPOVERLAPPED lpOverlapped;
    ULONG_PTR Internal;
    DWORD dwNumberOfBytesTransferred;
} OVERLAPPED_ENTRY, *LPOVERLAPPED_ENTRY;

typedef struct _SECURITY_ATTRIBUTES {
    DWORD nLength;
    LPVOID lpSecurityDescriptor;
    BOOL bInheritHandle;
} SECURITY_ATTRIBUTES, *PSECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;

typedef struct _PROCESS_INFORMATION {
    HANDLE hProcess;
    HANDLE hThread;
    DWORD dwProcessId;
    DWORD dwThreadId;
} PROCESS_INFORMATION, *PPROCESS_INFORMATION, *LPPROCESS_INFORMATION;

//
//  File System time stamps are represented with the following structure:
//


#ifndef _FILETIME_
#define _FILETIME_
typedef struct _FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME, *PFILETIME, *LPFILETIME;
#endif

//
// System time is represented with the following structure:
//


typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;


typedef DWORD (WINAPI *PTHREAD_START_ROUTINE)(
    LPVOID lpThreadParameter
    );
typedef PTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE;

#if(_WIN32_WINNT >= 0x0400)
typedef VOID (WINAPI *PFIBER_START_ROUTINE)(
    LPVOID lpFiberParameter
    );
typedef PFIBER_START_ROUTINE LPFIBER_START_ROUTINE;
#endif /* _WIN32_WINNT >= 0x0400 */

typedef RTL_CRITICAL_SECTION CRITICAL_SECTION;
typedef PRTL_CRITICAL_SECTION PCRITICAL_SECTION;
typedef PRTL_CRITICAL_SECTION LPCRITICAL_SECTION;

typedef RTL_CRITICAL_SECTION_DEBUG CRITICAL_SECTION_DEBUG;
typedef PRTL_CRITICAL_SECTION_DEBUG PCRITICAL_SECTION_DEBUG;
typedef PRTL_CRITICAL_SECTION_DEBUG LPCRITICAL_SECTION_DEBUG;

#if (_WIN32_WINNT >= 0x0600)

//
// Define one-time initialization primitive
//

typedef RTL_RUN_ONCE INIT_ONCE;
typedef PRTL_RUN_ONCE PINIT_ONCE;
typedef PRTL_RUN_ONCE LPINIT_ONCE;

#define INIT_ONCE_STATIC_INIT   RTL_RUN_ONCE_INIT

//
// Run once flags
//

#define INIT_ONCE_CHECK_ONLY        RTL_RUN_ONCE_CHECK_ONLY
#define INIT_ONCE_ASYNC             RTL_RUN_ONCE_ASYNC
#define INIT_ONCE_INIT_FAILED       RTL_RUN_ONCE_INIT_FAILED

//
// The context stored in the run once structure must leave the following number
// of low order bits unused.
//

#define INIT_ONCE_CTX_RESERVED_BITS RTL_RUN_ONCE_CTX_RESERVED_BITS

//
// FailFast Exception Flags
//

#define FAIL_FAST_GENERATE_EXCEPTION_ADDRESS    0x1
#define FAIL_FAST_NO_HARD_ERROR_DLG             0x2

typedef
BOOL
(WINAPI *PINIT_ONCE_FN) (
    __inout PINIT_ONCE InitOnce,
    __inout_opt PVOID Parameter,
    __deref_opt_out_opt PVOID *Context
    );

WINBASEAPI
VOID
WINAPI
InitOnceInitialize (
    __out PINIT_ONCE InitOnce
    );

WINBASEAPI
BOOL
WINAPI
InitOnceExecuteOnce (
    __inout PINIT_ONCE InitOnce,
    __in __callback PINIT_ONCE_FN InitFn,
    __inout_opt PVOID Parameter,
    __deref_opt_out_opt LPVOID *Context
    );

WINBASEAPI
BOOL
WINAPI
InitOnceBeginInitialize (
    __inout LPINIT_ONCE lpInitOnce,
    __in DWORD dwFlags,
    __out PBOOL fPending,
    __deref_opt_out_opt LPVOID *lpContext
    );

WINBASEAPI
BOOL
WINAPI
InitOnceComplete (
    __inout LPINIT_ONCE lpInitOnce,
    __in DWORD dwFlags,
    __in_opt LPVOID lpContext
    );

#endif

//
// Define the slim r/w lock
//

typedef RTL_SRWLOCK SRWLOCK, *PSRWLOCK;

#define SRWLOCK_INIT RTL_SRWLOCK_INIT

WINBASEAPI
VOID
WINAPI
InitializeSRWLock (
     __out PSRWLOCK SRWLock
     );

WINBASEAPI
VOID
WINAPI
ReleaseSRWLockExclusive (
     __inout PSRWLOCK SRWLock
     );

WINBASEAPI
VOID
WINAPI
ReleaseSRWLockShared (
     __inout PSRWLOCK SRWLock
     );

WINBASEAPI
VOID
WINAPI
AcquireSRWLockExclusive (
     __inout PSRWLOCK SRWLock
     );

WINBASEAPI
VOID
WINAPI
AcquireSRWLockShared (
     __inout PSRWLOCK SRWLock
     );

WINBASEAPI
BOOLEAN
WINAPI
TryAcquireSRWLockExclusive (
    __inout PSRWLOCK SRWLock
    );

WINBASEAPI
BOOLEAN
WINAPI
TryAcquireSRWLockShared (
    __inout PSRWLOCK SRWLock
    );

//
// Define condition variable
//

typedef RTL_CONDITION_VARIABLE CONDITION_VARIABLE, *PCONDITION_VARIABLE;

WINBASEAPI
VOID
WINAPI
InitializeConditionVariable (
    __out PCONDITION_VARIABLE ConditionVariable
    );

WINBASEAPI
VOID
WINAPI
WakeConditionVariable (
    __inout PCONDITION_VARIABLE ConditionVariable
    );

WINBASEAPI
VOID
WINAPI
WakeAllConditionVariable (
    __inout PCONDITION_VARIABLE ConditionVariable
    );

WINBASEAPI
BOOL
WINAPI
SleepConditionVariableCS (
    __inout PCONDITION_VARIABLE ConditionVariable,
    __inout PCRITICAL_SECTION CriticalSection,
    __in DWORD dwMilliseconds
    );

WINBASEAPI
BOOL
WINAPI
SleepConditionVariableSRW (
    __inout PCONDITION_VARIABLE ConditionVariable,
    __inout PSRWLOCK SRWLock,
    __in DWORD dwMilliseconds,
    __in ULONG Flags
    );

//
// Static initializer for the condition variable
//

#define CONDITION_VARIABLE_INIT RTL_CONDITION_VARIABLE_INIT

//
// Flags for condition variables
//
#define CONDITION_VARIABLE_LOCKMODE_SHARED RTL_CONDITION_VARIABLE_LOCKMODE_SHARED


WINBASEAPI
__out_opt
PVOID
WINAPI
EncodePointer (
    __in_opt PVOID Ptr
    );

WINBASEAPI
__out_opt
PVOID
WINAPI
DecodePointer (
    __in_opt PVOID Ptr
    );

WINBASEAPI
__out_opt
PVOID
WINAPI
EncodeSystemPointer (
    __in_opt PVOID Ptr
    );

WINBASEAPI
__out_opt
PVOID
WINAPI
DecodeSystemPointer (
    __in_opt PVOID Ptr
    );

#if defined(_X86_)
typedef PLDT_ENTRY LPLDT_ENTRY;
#else
typedef LPVOID LPLDT_ENTRY;
#endif

#define MUTEX_MODIFY_STATE MUTANT_QUERY_STATE
#define MUTEX_ALL_ACCESS MUTANT_ALL_ACCESS

//
// Serial provider type.
//

#define SP_SERIALCOMM    ((DWORD)0x00000001)

//
// Provider SubTypes
//

#define PST_UNSPECIFIED      ((DWORD)0x00000000)
#define PST_RS232            ((DWORD)0x00000001)
#define PST_PARALLELPORT     ((DWORD)0x00000002)
#define PST_RS422            ((DWORD)0x00000003)
#define PST_RS423            ((DWORD)0x00000004)
#define PST_RS449            ((DWORD)0x00000005)
#define PST_MODEM            ((DWORD)0x00000006)
#define PST_FAX              ((DWORD)0x00000021)
#define PST_SCANNER          ((DWORD)0x00000022)
#define PST_NETWORK_BRIDGE   ((DWORD)0x00000100)
#define PST_LAT              ((DWORD)0x00000101)
#define PST_TCPIP_TELNET     ((DWORD)0x00000102)
#define PST_X25              ((DWORD)0x00000103)


//
// Provider capabilities flags.
//

#define PCF_DTRDSR        ((DWORD)0x0001)
#define PCF_RTSCTS        ((DWORD)0x0002)
#define PCF_RLSD          ((DWORD)0x0004)
#define PCF_PARITY_CHECK  ((DWORD)0x0008)
#define PCF_XONXOFF       ((DWORD)0x0010)
#define PCF_SETXCHAR      ((DWORD)0x0020)
#define PCF_TOTALTIMEOUTS ((DWORD)0x0040)
#define PCF_INTTIMEOUTS   ((DWORD)0x0080)
#define PCF_SPECIALCHARS  ((DWORD)0x0100)
#define PCF_16BITMODE     ((DWORD)0x0200)

//
// Comm provider settable parameters.
//

#define SP_PARITY         ((DWORD)0x0001)
#define SP_BAUD           ((DWORD)0x0002)
#define SP_DATABITS       ((DWORD)0x0004)
#define SP_STOPBITS       ((DWORD)0x0008)
#define SP_HANDSHAKING    ((DWORD)0x0010)
#define SP_PARITY_CHECK   ((DWORD)0x0020)
#define SP_RLSD           ((DWORD)0x0040)

//
// Settable baud rates in the provider.
//

#define BAUD_075          ((DWORD)0x00000001)
#define BAUD_110          ((DWORD)0x00000002)
#define BAUD_134_5        ((DWORD)0x00000004)
#define BAUD_150          ((DWORD)0x00000008)
#define BAUD_300          ((DWORD)0x00000010)
#define BAUD_600          ((DWORD)0x00000020)
#define BAUD_1200         ((DWORD)0x00000040)
#define BAUD_1800         ((DWORD)0x00000080)
#define BAUD_2400         ((DWORD)0x00000100)
#define BAUD_4800         ((DWORD)0x00000200)
#define BAUD_7200         ((DWORD)0x00000400)
#define BAUD_9600         ((DWORD)0x00000800)
#define BAUD_14400        ((DWORD)0x00001000)
#define BAUD_19200        ((DWORD)0x00002000)
#define BAUD_38400        ((DWORD)0x00004000)
#define BAUD_56K          ((DWORD)0x00008000)
#define BAUD_128K         ((DWORD)0x00010000)
#define BAUD_115200       ((DWORD)0x00020000)
#define BAUD_57600        ((DWORD)0x00040000)
#define BAUD_USER         ((DWORD)0x10000000)

//
// Settable Data Bits
//

#define DATABITS_5        ((WORD)0x0001)
#define DATABITS_6        ((WORD)0x0002)
#define DATABITS_7        ((WORD)0x0004)
#define DATABITS_8        ((WORD)0x0008)
#define DATABITS_16       ((WORD)0x0010)
#define DATABITS_16X      ((WORD)0x0020)

//
// Settable Stop and Parity bits.
//

#define STOPBITS_10       ((WORD)0x0001)
#define STOPBITS_15       ((WORD)0x0002)
#define STOPBITS_20       ((WORD)0x0004)
#define PARITY_NONE       ((WORD)0x0100)
#define PARITY_ODD        ((WORD)0x0200)
#define PARITY_EVEN       ((WORD)0x0400)
#define PARITY_MARK       ((WORD)0x0800)
#define PARITY_SPACE      ((WORD)0x1000)

typedef struct _COMMPROP {
    WORD wPacketLength;
    WORD wPacketVersion;
    DWORD dwServiceMask;
    DWORD dwReserved1;
    DWORD dwMaxTxQueue;
    DWORD dwMaxRxQueue;
    DWORD dwMaxBaud;
    DWORD dwProvSubType;
    DWORD dwProvCapabilities;
    DWORD dwSettableParams;
    DWORD dwSettableBaud;
    WORD wSettableData;
    WORD wSettableStopParity;
    DWORD dwCurrentTxQueue;
    DWORD dwCurrentRxQueue;
    DWORD dwProvSpec1;
    DWORD dwProvSpec2;
    WCHAR wcProvChar[1];
} COMMPROP,*LPCOMMPROP;

//
// Set dwProvSpec1 to COMMPROP_INITIALIZED to indicate that wPacketLength
// is valid before a call to GetCommProperties().
//
#define COMMPROP_INITIALIZED ((DWORD)0xE73CF52E)

typedef struct _COMSTAT {
    DWORD fCtsHold : 1;
    DWORD fDsrHold : 1;
    DWORD fRlsdHold : 1;
    DWORD fXoffHold : 1;
    DWORD fXoffSent : 1;
    DWORD fEof : 1;
    DWORD fTxim : 1;
    DWORD fReserved : 25;
    DWORD cbInQue;
    DWORD cbOutQue;
} COMSTAT, *LPCOMSTAT;

//
// DTR Control Flow Values.
//
#define DTR_CONTROL_DISABLE    0x00
#define DTR_CONTROL_ENABLE     0x01
#define DTR_CONTROL_HANDSHAKE  0x02

//
// RTS Control Flow Values
//
#define RTS_CONTROL_DISABLE    0x00
#define RTS_CONTROL_ENABLE     0x01
#define RTS_CONTROL_HANDSHAKE  0x02
#define RTS_CONTROL_TOGGLE     0x03

typedef struct _DCB {
    DWORD DCBlength;      /* sizeof(DCB)                     */
    DWORD BaudRate;       /* Baudrate at which running       */
    DWORD fBinary: 1;     /* Binary Mode (skip EOF check)    */
    DWORD fParity: 1;     /* Enable parity checking          */
    DWORD fOutxCtsFlow:1; /* CTS handshaking on output       */
    DWORD fOutxDsrFlow:1; /* DSR handshaking on output       */
    DWORD fDtrControl:2;  /* DTR Flow control                */
    DWORD fDsrSensitivity:1; /* DSR Sensitivity              */
    DWORD fTXContinueOnXoff: 1; /* Continue TX when Xoff sent */
    DWORD fOutX: 1;       /* Enable output X-ON/X-OFF        */
    DWORD fInX: 1;        /* Enable input X-ON/X-OFF         */
    DWORD fErrorChar: 1;  /* Enable Err Replacement          */
    DWORD fNull: 1;       /* Enable Null stripping           */
    DWORD fRtsControl:2;  /* Rts Flow control                */
    DWORD fAbortOnError:1; /* Abort all reads and writes on Error */
    DWORD fDummy2:17;     /* Reserved                        */
    WORD wReserved;       /* Not currently used              */
    WORD XonLim;          /* Transmit X-ON threshold         */
    WORD XoffLim;         /* Transmit X-OFF threshold        */
    BYTE ByteSize;        /* Number of bits/byte, 4-8        */
    BYTE Parity;          /* 0-4=None,Odd,Even,Mark,Space    */
    BYTE StopBits;        /* 0,1,2 = 1, 1.5, 2               */
    char XonChar;         /* Tx and Rx X-ON character        */
    char XoffChar;        /* Tx and Rx X-OFF character       */
    char ErrorChar;       /* Error replacement char          */
    char EofChar;         /* End of Input character          */
    char EvtChar;         /* Received Event character        */
    WORD wReserved1;      /* Fill for now.                   */
} DCB, *LPDCB;

typedef struct _COMMTIMEOUTS {
    DWORD ReadIntervalTimeout;          /* Maximum time between read chars. */
    DWORD ReadTotalTimeoutMultiplier;   /* Multiplier of characters.        */
    DWORD ReadTotalTimeoutConstant;     /* Constant in milliseconds.        */
    DWORD WriteTotalTimeoutMultiplier;  /* Multiplier of characters.        */
    DWORD WriteTotalTimeoutConstant;    /* Constant in milliseconds.        */
} COMMTIMEOUTS,*LPCOMMTIMEOUTS;

typedef struct _COMMCONFIG {
    DWORD dwSize;               /* Size of the entire struct */
    WORD wVersion;              /* version of the structure */
    WORD wReserved;             /* alignment */
    DCB dcb;                    /* device control block */
    DWORD dwProviderSubType;    /* ordinal value for identifying
                                   provider-defined data structure format*/
    DWORD dwProviderOffset;     /* Specifies the offset of provider specific
                                   data field in bytes from the start */
    DWORD dwProviderSize;       /* size of the provider-specific data field */
    WCHAR wcProviderData[1];    /* provider-specific data */
} COMMCONFIG,*LPCOMMCONFIG;

typedef struct _SYSTEM_INFO {
    union {
        DWORD dwOemId;          // Obsolete field...do not use
        struct {
            WORD wProcessorArchitecture;
            WORD wReserved;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
    DWORD dwPageSize;
    LPVOID lpMinimumApplicationAddress;
    LPVOID lpMaximumApplicationAddress;
    DWORD_PTR dwActiveProcessorMask;
    DWORD dwNumberOfProcessors;
    DWORD dwProcessorType;
    DWORD dwAllocationGranularity;
    WORD wProcessorLevel;
    WORD wProcessorRevision;
} SYSTEM_INFO, *LPSYSTEM_INFO;

//
//


#define FreeModule(hLibModule) FreeLibrary((hLibModule))
#define MakeProcInstance(lpProc,hInstance) (lpProc)
#define FreeProcInstance(lpProc) (lpProc)

/* Global Memory Flags */
#define GMEM_FIXED          0x0000
#define GMEM_MOVEABLE       0x0002
#define GMEM_NOCOMPACT      0x0010
#define GMEM_NODISCARD      0x0020
#define GMEM_ZEROINIT       0x0040
#define GMEM_MODIFY         0x0080
#define GMEM_DISCARDABLE    0x0100
#define GMEM_NOT_BANKED     0x1000
#define GMEM_SHARE          0x2000
#define GMEM_DDESHARE       0x2000
#define GMEM_NOTIFY         0x4000
#define GMEM_LOWER          GMEM_NOT_BANKED
#define GMEM_VALID_FLAGS    0x7F72
#define GMEM_INVALID_HANDLE 0x8000

#define GHND                (GMEM_MOVEABLE | GMEM_ZEROINIT)
#define GPTR                (GMEM_FIXED | GMEM_ZEROINIT)

#define GlobalLRUNewest( h )    ((HANDLE)(h))
#define GlobalLRUOldest( h )    ((HANDLE)(h))
#define GlobalDiscard( h )      GlobalReAlloc( (h), 0, GMEM_MOVEABLE )

/* Flags returned by GlobalFlags (in addition to GMEM_DISCARDABLE) */
#define GMEM_DISCARDED      0x4000
#define GMEM_LOCKCOUNT      0x00FF

typedef struct _MEMORYSTATUS {
    DWORD dwLength;
    DWORD dwMemoryLoad;
    SIZE_T dwTotalPhys;
    SIZE_T dwAvailPhys;
    SIZE_T dwTotalPageFile;
    SIZE_T dwAvailPageFile;
    SIZE_T dwTotalVirtual;
    SIZE_T dwAvailVirtual;
} MEMORYSTATUS, *LPMEMORYSTATUS;

/* Local Memory Flags */
#define LMEM_FIXED          0x0000
#define LMEM_MOVEABLE       0x0002
#define LMEM_NOCOMPACT      0x0010
#define LMEM_NODISCARD      0x0020
#define LMEM_ZEROINIT       0x0040
#define LMEM_MODIFY         0x0080
#define LMEM_DISCARDABLE    0x0F00
#define LMEM_VALID_FLAGS    0x0F72
#define LMEM_INVALID_HANDLE 0x8000

#define LHND                (LMEM_MOVEABLE | LMEM_ZEROINIT)
#define LPTR                (LMEM_FIXED | LMEM_ZEROINIT)

#define NONZEROLHND         (LMEM_MOVEABLE)
#define NONZEROLPTR         (LMEM_FIXED)

#define LocalDiscard( h )   LocalReAlloc( (h), 0, LMEM_MOVEABLE )

/* Flags returned by LocalFlags (in addition to LMEM_DISCARDABLE) */
#define LMEM_DISCARDED      0x4000
#define LMEM_LOCKCOUNT      0x00FF

//
// NUMA values
//
#define NUMA_NO_PREFERRED_NODE ((DWORD) -1)

//
// Process dwCreationFlag values
//

#define DEBUG_PROCESS                     0x00000001
#define DEBUG_ONLY_THIS_PROCESS           0x00000002
#define CREATE_SUSPENDED                  0x00000004
#define DETACHED_PROCESS                  0x00000008

#define CREATE_NEW_CONSOLE                0x00000010
#define NORMAL_PRIORITY_CLASS             0x00000020
#define IDLE_PRIORITY_CLASS               0x00000040
#define HIGH_PRIORITY_CLASS               0x00000080

#define REALTIME_PRIORITY_CLASS           0x00000100
#define CREATE_NEW_PROCESS_GROUP          0x00000200
#define CREATE_UNICODE_ENVIRONMENT        0x00000400
#define CREATE_SEPARATE_WOW_VDM           0x00000800

#define CREATE_SHARED_WOW_VDM             0x00001000
#define CREATE_FORCEDOS                   0x00002000
#define BELOW_NORMAL_PRIORITY_CLASS       0x00004000
#define ABOVE_NORMAL_PRIORITY_CLASS       0x00008000

#define INHERIT_PARENT_AFFINITY           0x00010000
#define INHERIT_CALLER_PRIORITY           0x00020000    // Deprecated
#define CREATE_PROTECTED_PROCESS          0x00040000
#define EXTENDED_STARTUPINFO_PRESENT      0x00080000

#define PROCESS_MODE_BACKGROUND_BEGIN     0x00100000
#define PROCESS_MODE_BACKGROUND_END       0x00200000

#define CREATE_BREAKAWAY_FROM_JOB         0x01000000
#define CREATE_PRESERVE_CODE_AUTHZ_LEVEL  0x02000000
#define CREATE_DEFAULT_ERROR_MODE         0x04000000
#define CREATE_NO_WINDOW                  0x08000000

#define PROFILE_USER                      0x10000000
#define PROFILE_KERNEL                    0x20000000
#define PROFILE_SERVER                    0x40000000
#define CREATE_IGNORE_SYSTEM_DEFAULT      0x80000000

//
// Thread dwCreationFlag values
//

//#define CREATE_SUSPENDED                  0x00000004

#define STACK_SIZE_PARAM_IS_A_RESERVATION   0x00010000    // Threads only

//
// Priority flags
//

#define THREAD_PRIORITY_LOWEST          THREAD_BASE_PRIORITY_MIN
#define THREAD_PRIORITY_BELOW_NORMAL    (THREAD_PRIORITY_LOWEST+1)
#define THREAD_PRIORITY_NORMAL          0
#define THREAD_PRIORITY_HIGHEST         THREAD_BASE_PRIORITY_MAX
#define THREAD_PRIORITY_ABOVE_NORMAL    (THREAD_PRIORITY_HIGHEST-1)
#define THREAD_PRIORITY_ERROR_RETURN    (MAXLONG)

#define THREAD_PRIORITY_TIME_CRITICAL   THREAD_BASE_PRIORITY_LOWRT
#define THREAD_PRIORITY_IDLE            THREAD_BASE_PRIORITY_IDLE

#define THREAD_MODE_BACKGROUND_BEGIN    0x00010000
#define THREAD_MODE_BACKGROUND_END      0x00020000

//
// GetFinalPathNameByHandle
//

#define VOLUME_NAME_DOS  0x0      //default
#define VOLUME_NAME_GUID 0x1
#define VOLUME_NAME_NT   0x2
#define VOLUME_NAME_NONE 0x4

#define FILE_NAME_NORMALIZED 0x0  //default
#define FILE_NAME_OPENED     0x8

//
// Debug APIs
//
#define EXCEPTION_DEBUG_EVENT       1
#define CREATE_THREAD_DEBUG_EVENT   2
#define CREATE_PROCESS_DEBUG_EVENT  3
#define EXIT_THREAD_DEBUG_EVENT     4
#define EXIT_PROCESS_DEBUG_EVENT    5
#define LOAD_DLL_DEBUG_EVENT        6
#define UNLOAD_DLL_DEBUG_EVENT      7
#define OUTPUT_DEBUG_STRING_EVENT   8
#define RIP_EVENT                   9

typedef struct _EXCEPTION_DEBUG_INFO {
    EXCEPTION_RECORD ExceptionRecord;
    DWORD dwFirstChance;
} EXCEPTION_DEBUG_INFO, *LPEXCEPTION_DEBUG_INFO;

typedef struct _CREATE_THREAD_DEBUG_INFO {
    HANDLE hThread;
    LPVOID lpThreadLocalBase;
    LPTHREAD_START_ROUTINE lpStartAddress;
} CREATE_THREAD_DEBUG_INFO, *LPCREATE_THREAD_DEBUG_INFO;

typedef struct _CREATE_PROCESS_DEBUG_INFO {
    HANDLE hFile;
    HANDLE hProcess;
    HANDLE hThread;
    LPVOID lpBaseOfImage;
    DWORD dwDebugInfoFileOffset;
    DWORD nDebugInfoSize;
    LPVOID lpThreadLocalBase;
    LPTHREAD_START_ROUTINE lpStartAddress;
    LPVOID lpImageName;
    WORD fUnicode;
} CREATE_PROCESS_DEBUG_INFO, *LPCREATE_PROCESS_DEBUG_INFO;

typedef struct _EXIT_THREAD_DEBUG_INFO {
    DWORD dwExitCode;
} EXIT_THREAD_DEBUG_INFO, *LPEXIT_THREAD_DEBUG_INFO;

typedef struct _EXIT_PROCESS_DEBUG_INFO {
    DWORD dwExitCode;
} EXIT_PROCESS_DEBUG_INFO, *LPEXIT_PROCESS_DEBUG_INFO;

typedef struct _LOAD_DLL_DEBUG_INFO {
    HANDLE hFile;
    LPVOID lpBaseOfDll;
    DWORD dwDebugInfoFileOffset;
    DWORD nDebugInfoSize;
    LPVOID lpImageName;
    WORD fUnicode;
} LOAD_DLL_DEBUG_INFO, *LPLOAD_DLL_DEBUG_INFO;

typedef struct _UNLOAD_DLL_DEBUG_INFO {
    LPVOID lpBaseOfDll;
} UNLOAD_DLL_DEBUG_INFO, *LPUNLOAD_DLL_DEBUG_INFO;

typedef struct _OUTPUT_DEBUG_STRING_INFO {
    LPSTR lpDebugStringData;
    WORD fUnicode;
    WORD nDebugStringLength;
} OUTPUT_DEBUG_STRING_INFO, *LPOUTPUT_DEBUG_STRING_INFO;

typedef struct _RIP_INFO {
    DWORD dwError;
    DWORD dwType;
} RIP_INFO, *LPRIP_INFO;


typedef struct _DEBUG_EVENT {
    DWORD dwDebugEventCode;
    DWORD dwProcessId;
    DWORD dwThreadId;
    union {
        EXCEPTION_DEBUG_INFO Exception;
        CREATE_THREAD_DEBUG_INFO CreateThread;
        CREATE_PROCESS_DEBUG_INFO CreateProcessInfo;
        EXIT_THREAD_DEBUG_INFO ExitThread;
        EXIT_PROCESS_DEBUG_INFO ExitProcess;
        LOAD_DLL_DEBUG_INFO LoadDll;
        UNLOAD_DLL_DEBUG_INFO UnloadDll;
        OUTPUT_DEBUG_STRING_INFO DebugString;
        RIP_INFO RipInfo;
    } u;
} DEBUG_EVENT, *LPDEBUG_EVENT;

//
// JIT Debugging Info. This structure is defined to have constant size in
// both the emulated and native environment.
//

typedef struct _JIT_DEBUG_INFO {
    DWORD dwSize;
    DWORD dwProcessorArchitecture;
    DWORD dwThreadID;
    DWORD dwReserved0;
    ULONG64 lpExceptionAddress;
    ULONG64 lpExceptionRecord;
    ULONG64 lpContextRecord;
} JIT_DEBUG_INFO, *LPJIT_DEBUG_INFO;

typedef JIT_DEBUG_INFO JIT_DEBUG_INFO32, *LPJIT_DEBUG_INFO32;
typedef JIT_DEBUG_INFO JIT_DEBUG_INFO64, *LPJIT_DEBUG_INFO64;

#if !defined(MIDL_PASS)
typedef PCONTEXT LPCONTEXT;
typedef PEXCEPTION_RECORD LPEXCEPTION_RECORD;
typedef PEXCEPTION_POINTERS LPEXCEPTION_POINTERS;
#endif

#define DRIVE_UNKNOWN     0
#define DRIVE_NO_ROOT_DIR 1
#define DRIVE_REMOVABLE   2
#define DRIVE_FIXED       3
#define DRIVE_REMOTE      4
#define DRIVE_CDROM       5
#define DRIVE_RAMDISK     6


#ifndef _MAC
#define GetFreeSpace(w)                 (0x100000L)
#else
WINBASEAPI DWORD WINAPI GetFreeSpace(__in UINT);
#endif


#define FILE_TYPE_UNKNOWN   0x0000
#define FILE_TYPE_DISK      0x0001
#define FILE_TYPE_CHAR      0x0002
#define FILE_TYPE_PIPE      0x0003
#define FILE_TYPE_REMOTE    0x8000


#define STD_INPUT_HANDLE    ((DWORD)-10)
#define STD_OUTPUT_HANDLE   ((DWORD)-11)
#define STD_ERROR_HANDLE    ((DWORD)-12)

#define NOPARITY            0
#define ODDPARITY           1
#define EVENPARITY          2
#define MARKPARITY          3
#define SPACEPARITY         4

#define ONESTOPBIT          0
#define ONE5STOPBITS        1
#define TWOSTOPBITS         2

#define IGNORE              0       // Ignore signal
#define INFINITE            0xFFFFFFFF  // Infinite timeout

//
// Baud rates at which the communication device operates
//

#define CBR_110             110
#define CBR_300             300
#define CBR_600             600
#define CBR_1200            1200
#define CBR_2400            2400
#define CBR_4800            4800
#define CBR_9600            9600
#define CBR_14400           14400
#define CBR_19200           19200
#define CBR_38400           38400
#define CBR_56000           56000
#define CBR_57600           57600
#define CBR_115200          115200
#define CBR_128000          128000
#define CBR_256000          256000

//
// Error Flags
//

#define CE_RXOVER           0x0001  // Receive Queue overflow
#define CE_OVERRUN          0x0002  // Receive Overrun Error
#define CE_RXPARITY         0x0004  // Receive Parity Error
#define CE_FRAME            0x0008  // Receive Framing error
#define CE_BREAK            0x0010  // Break Detected
#define CE_TXFULL           0x0100  // TX Queue is full
#define CE_PTO              0x0200  // LPTx Timeout
#define CE_IOE              0x0400  // LPTx I/O Error
#define CE_DNS              0x0800  // LPTx Device not selected
#define CE_OOP              0x1000  // LPTx Out-Of-Paper
#define CE_MODE             0x8000  // Requested mode unsupported

#define IE_BADID            (-1)    // Invalid or unsupported id
#define IE_OPEN             (-2)    // Device Already Open
#define IE_NOPEN            (-3)    // Device Not Open
#define IE_MEMORY           (-4)    // Unable to allocate queues
#define IE_DEFAULT          (-5)    // Error in default parameters
#define IE_HARDWARE         (-10)   // Hardware Not Present
#define IE_BYTESIZE         (-11)   // Illegal Byte Size
#define IE_BAUDRATE         (-12)   // Unsupported BaudRate

//
// Events
//

#define EV_RXCHAR           0x0001  // Any Character received
#define EV_RXFLAG           0x0002  // Received certain character
#define EV_TXEMPTY          0x0004  // Transmitt Queue Empty
#define EV_CTS              0x0008  // CTS changed state
#define EV_DSR              0x0010  // DSR changed state
#define EV_RLSD             0x0020  // RLSD changed state
#define EV_BREAK            0x0040  // BREAK received
#define EV_ERR              0x0080  // Line status error occurred
#define EV_RING             0x0100  // Ring signal detected
#define EV_PERR             0x0200  // Printer error occured
#define EV_RX80FULL         0x0400  // Receive buffer is 80 percent full
#define EV_EVENT1           0x0800  // Provider specific event 1
#define EV_EVENT2           0x1000  // Provider specific event 2

//
// Escape Functions
//

#define SETXOFF             1       // Simulate XOFF received
#define SETXON              2       // Simulate XON received
#define SETRTS              3       // Set RTS high
#define CLRRTS              4       // Set RTS low
#define SETDTR              5       // Set DTR high
#define CLRDTR              6       // Set DTR low
#define RESETDEV            7       // Reset device if possible
#define SETBREAK            8       // Set the device break line.
#define CLRBREAK            9       // Clear the device break line.

//
// PURGE function flags.
//
#define PURGE_TXABORT       0x0001  // Kill the pending/current writes to the comm port.
#define PURGE_RXABORT       0x0002  // Kill the pending/current reads to the comm port.
#define PURGE_TXCLEAR       0x0004  // Kill the transmit queue if there.
#define PURGE_RXCLEAR       0x0008  // Kill the typeahead buffer if there.

#define LPTx                0x80    // Set if ID is for LPT device

//
// Modem Status Flags
//
#define MS_CTS_ON           ((DWORD)0x0010)
#define MS_DSR_ON           ((DWORD)0x0020)
#define MS_RING_ON          ((DWORD)0x0040)
#define MS_RLSD_ON          ((DWORD)0x0080)

//
// WaitSoundState() Constants
//

#define S_QUEUEEMPTY        0
#define S_THRESHOLD         1
#define S_ALLTHRESHOLD      2

//
// Accent Modes
//

#define S_NORMAL      0
#define S_LEGATO      1
#define S_STACCATO    2

//
// SetSoundNoise() Sources
//

#define S_PERIOD512   0     // Freq = N/512 high pitch, less coarse hiss
#define S_PERIOD1024  1     // Freq = N/1024
#define S_PERIOD2048  2     // Freq = N/2048 low pitch, more coarse hiss
#define S_PERIODVOICE 3     // Source is frequency from voice channel (3)
#define S_WHITE512    4     // Freq = N/512 high pitch, less coarse hiss
#define S_WHITE1024   5     // Freq = N/1024
#define S_WHITE2048   6     // Freq = N/2048 low pitch, more coarse hiss
#define S_WHITEVOICE  7     // Source is frequency from voice channel (3)

#define S_SERDVNA     (-1)  // Device not available
#define S_SEROFM      (-2)  // Out of memory
#define S_SERMACT     (-3)  // Music active
#define S_SERQFUL     (-4)  // Queue full
#define S_SERBDNT     (-5)  // Invalid note
#define S_SERDLN      (-6)  // Invalid note length
#define S_SERDCC      (-7)  // Invalid note count
#define S_SERDTP      (-8)  // Invalid tempo
#define S_SERDVL      (-9)  // Invalid volume
#define S_SERDMD      (-10) // Invalid mode
#define S_SERDSH      (-11) // Invalid shape
#define S_SERDPT      (-12) // Invalid pitch
#define S_SERDFQ      (-13) // Invalid frequency
#define S_SERDDR      (-14) // Invalid duration
#define S_SERDSR      (-15) // Invalid source
#define S_SERDST      (-16) // Invalid state

#define NMPWAIT_WAIT_FOREVER            0xffffffff
#define NMPWAIT_NOWAIT                  0x00000001
#define NMPWAIT_USE_DEFAULT_WAIT        0x00000000

#define FS_CASE_IS_PRESERVED            FILE_CASE_PRESERVED_NAMES
#define FS_CASE_SENSITIVE               FILE_CASE_SENSITIVE_SEARCH
#define FS_UNICODE_STORED_ON_DISK       FILE_UNICODE_ON_DISK
#define FS_PERSISTENT_ACLS              FILE_PERSISTENT_ACLS
#define FS_VOL_IS_COMPRESSED            FILE_VOLUME_IS_COMPRESSED
#define FS_FILE_COMPRESSION             FILE_FILE_COMPRESSION
#define FS_FILE_ENCRYPTION              FILE_SUPPORTS_ENCRYPTION

#define FILE_MAP_COPY       SECTION_QUERY
#define FILE_MAP_WRITE      SECTION_MAP_WRITE
#define FILE_MAP_READ       SECTION_MAP_READ
#define FILE_MAP_ALL_ACCESS SECTION_ALL_ACCESS
#define FILE_MAP_EXECUTE    SECTION_MAP_EXECUTE_EXPLICIT    // not included in FILE_MAP_ALL_ACCESS

#define OF_READ             0x00000000
#define OF_WRITE            0x00000001
#define OF_READWRITE        0x00000002
#define OF_SHARE_COMPAT     0x00000000
#define OF_SHARE_EXCLUSIVE  0x00000010
#define OF_SHARE_DENY_WRITE 0x00000020
#define OF_SHARE_DENY_READ  0x00000030
#define OF_SHARE_DENY_NONE  0x00000040
#define OF_PARSE            0x00000100
#define OF_DELETE           0x00000200
#define OF_VERIFY           0x00000400
#define OF_CANCEL           0x00000800
#define OF_CREATE           0x00001000
#define OF_PROMPT           0x00002000
#define OF_EXIST            0x00004000
#define OF_REOPEN           0x00008000

#define OFS_MAXPATHNAME 128
typedef struct _OFSTRUCT {
    BYTE cBytes;
    BYTE fFixedDisk;
    WORD nErrCode;
    WORD Reserved1;
    WORD Reserved2;
    CHAR szPathName[OFS_MAXPATHNAME];
} OFSTRUCT, *LPOFSTRUCT, *POFSTRUCT;

#ifndef NOWINBASEINTERLOCK

#ifndef _NTOS_

#if defined(_M_IA64) && !defined(RC_INVOKED)

#define InterlockedIncrement _InterlockedIncrement
#define InterlockedIncrementAcquire _InterlockedIncrement_acq
#define InterlockedIncrementRelease _InterlockedIncrement_rel
#define InterlockedDecrement _InterlockedDecrement
#define InterlockedDecrementAcquire _InterlockedDecrement_acq
#define InterlockedDecrementRelease _InterlockedDecrement_rel
#define InterlockedExchange _InterlockedExchange
#define InterlockedExchangeAdd _InterlockedExchangeAdd
#define InterlockedCompareExchange _InterlockedCompareExchange
#define InterlockedCompareExchangeAcquire _InterlockedCompareExchange_acq
#define InterlockedCompareExchangeRelease _InterlockedCompareExchange_rel
#define InterlockedExchangePointer _InterlockedExchangePointer
#define InterlockedCompareExchangePointer _InterlockedCompareExchangePointer
#define InterlockedCompareExchangePointerRelease _InterlockedCompareExchangePointer_rel
#define InterlockedCompareExchangePointerAcquire _InterlockedCompareExchangePointer_acq

#define InterlockedIncrement64 _InterlockedIncrement64
#define InterlockedDecrement64 _InterlockedDecrement64
#define InterlockedExchange64 _InterlockedExchange64
#define InterlockedExchangeAcquire64 _InterlockedExchange64_acq
#define InterlockedExchangeAdd64 _InterlockedExchangeAdd64
#define InterlockedCompareExchange64 _InterlockedCompareExchange64
#define InterlockedCompareExchangeAcquire64 _InterlockedCompareExchange64_acq
#define InterlockedCompareExchangeRelease64 _InterlockedCompareExchange64_rel
#define InterlockedCompare64Exchange128     _InterlockedCompare64Exchange128
#define InterlockedCompare64ExchangeAcquire128  _InterlockedCompare64Exchange128_acq
#define InterlockedCompare64ExchangeRelease128  _InterlockedCompare64Exchange128_rel

#define InterlockedOr _InterlockedOr
#define InterlockedOrAcquire _InterlockedOr_acq
#define InterlockedOrRelease _InterlockedOr_rel
#define InterlockedOr8 _InterlockedOr8
#define InterlockedOr8Acquire _InterlockedOr8_acq
#define InterlockedOr8Release _InterlockedOr8_rel
#define InterlockedOr16 _InterlockedOr16
#define InterlockedOr16Acquire _InterlockedOr16_acq
#define InterlockedOr16Release _InterlockedOr16_rel
#define InterlockedOr64 _InterlockedOr64
#define InterlockedOr64Acquire _InterlockedOr64_acq
#define InterlockedOr64Release _InterlockedOr64_rel
#define InterlockedXor _InterlockedXor
#define InterlockedXorAcquire _InterlockedXor_acq
#define InterlockedXorRelease _InterlockedXor_rel
#define InterlockedXor8 _InterlockedXor8
#define InterlockedXor8Acquire _InterlockedXor8_acq
#define InterlockedXor8Release _InterlockedXor8_rel
#define InterlockedXor16 _InterlockedXor16
#define InterlockedXor16Acquire _InterlockedXor16_acq
#define InterlockedXor16Release _InterlockedXor16_rel
#define InterlockedXor64 _InterlockedXor64
#define InterlockedXor64Acquire _InterlockedXor64_acq
#define InterlockedXor64Release _InterlockedXor64_rel
#define InterlockedAnd _InterlockedAnd
#define InterlockedAndAcquire _InterlockedAnd_acq
#define InterlockedAndRelease _InterlockedAnd_rel
#define InterlockedAnd8 _InterlockedAnd8
#define InterlockedAnd8Acquire _InterlockedAnd8_acq
#define InterlockedAnd8Release _InterlockedAnd8_rel
#define InterlockedAnd16 _InterlockedAnd16
#define InterlockedAnd16Acquire _InterlockedAnd16_acq
#define InterlockedAnd16Release _InterlockedAnd16_rel
#define InterlockedAnd64 _InterlockedAnd64
#define InterlockedAnd64Acquire _InterlockedAnd64_acq
#define InterlockedAnd64Release _InterlockedAnd64_rel

LONG
__cdecl
InterlockedOr (
    __inout LONG volatile *Destination,
    __in    LONG Value
    );

LONG
__cdecl
InterlockedOrAcquire (
    __inout LONG volatile *Destination,
    __in    LONG Value
    );

LONG
__cdecl
InterlockedOrRelease (
    __inout LONG volatile *Destination,
    __in    LONG Value
    );

char
__cdecl
InterlockedOr8 (
    __inout char volatile *Destination,
    __in    char Value
    );

char
__cdecl
InterlockedOr8Acquire (
    __inout char volatile *Destination,
    __in    char Value
    );

char
__cdecl
InterlockedOr8Release (
    __inout char volatile *Destination,
    __in    char Value
    );

SHORT
__cdecl
InterlockedOr16(
    __inout SHORT volatile *Destination,
    __in    SHORT Value
    );

SHORT
__cdecl
InterlockedOr16Acquire (
    __inout SHORT volatile *Destination,
    __in    SHORT Value
    );

SHORT
__cdecl
InterlockedOr16Release (
    __inout SHORT volatile *Destination,
    __in    SHORT Value
    );

LONGLONG
__cdecl
InterlockedOr64 (
    __inout LONGLONG volatile *Destination,
    __in    LONGLONG Value
    );

LONGLONG
__cdecl
InterlockedOr64Acquire (
    __inout LONGLONG volatile *Destination,
    __in    LONGLONG Value
    );

LONGLONG
__cdecl
InterlockedOr64Release (
    __inout LONGLONG volatile *Destination,
    __in    LONGLONG Value
    );

LONG
__cdecl
InterlockedXor (
    __inout LONG volatile *Destination,
    __in    LONG Value
    );

LONG
__cdecl
InterlockedXorAcquire (
    __inout LONG volatile *Destination,
    __in    LONG Value
    );

LONG
__cdecl
InterlockedXorRelease (
    __inout LONG volatile *Destination,
    __in    LONG Value
    );

char
__cdecl
InterlockedXor8 (
    __inout char volatile *Destination,
    __in    char Value
    );

char
__cdecl
InterlockedXor8Acquire (
    __inout char volatile *Destination,
    __in    char Value
    );

char
__cdecl
InterlockedXor8Release (
    __inout char volatile *Destination,
    __in    char Value
    );

SHORT
__cdecl
InterlockedXor16(
    __inout SHORT volatile *Destination,
    __in    SHORT Value
    );

SHORT
__cdecl
InterlockedXor16Acquire (
    __inout SHORT volatile *Destination,
    __in    SHORT Value
    );

SHORT
__cdecl
InterlockedXor16Release (
    __inout SHORT volatile *Destination,
    __in    SHORT Value
    );

LONGLONG
__cdecl
InterlockedXor64 (
    __inout LONGLONG volatile *Destination,
    __in    LONGLONG Value
    );

LONGLONG
__cdecl
InterlockedXor64Acquire (
    __inout LONGLONG volatile *Destination,
    __in    LONGLONG Value
    );

LONGLONG
__cdecl
InterlockedXor64Release (
    __inout LONGLONG volatile *Destination,
    __in    LONGLONG Value
    );

LONG
__cdecl
InterlockedAnd (
    __inout LONG volatile *Destination,
    __in    LONG Value
    );

LONG
__cdecl
InterlockedAndAcquire (
    __inout LONG volatile *Destination,
    __in    LONG Value
    );

LONG
__cdecl
InterlockedAndRelease (
    __inout LONG volatile *Destination,
    __in    LONG Value
    );

char
__cdecl
InterlockedAnd8 (
    __inout char volatile *Destination,
    __in    char Value
    );

char
__cdecl
InterlockedAnd8Acquire (
    __inout char volatile *Destination,
    __in    char Value
    );

char
__cdecl
InterlockedAnd8Release (
    __inout char volatile *Destination,
    __in    char Value
    );

SHORT
__cdecl
InterlockedAnd16(
    __inout SHORT volatile *Destination,
    __in    SHORT Value
    );

SHORT
__cdecl
InterlockedAnd16Acquire (
    __inout SHORT volatile *Destination,
    __in    SHORT Value
    );

SHORT
__cdecl
InterlockedAnd16Release (
    __inout SHORT volatile *Destination,
    __in    SHORT Value
    );

LONGLONG
__cdecl
InterlockedAnd64 (
    __inout LONGLONG volatile *Destination,
    __in    LONGLONG Value
    );

LONGLONG
__cdecl
InterlockedAnd64Acquire (
    __inout LONGLONG volatile *Destination,
    __in    LONGLONG Value
    );

LONGLONG
__cdecl
InterlockedAnd64Release (
    __inout LONGLONG volatile *Destination,
    __in    LONGLONG Value
    );

LONGLONG
__cdecl
InterlockedIncrement64 (
    __inout LONGLONG volatile *Addend
    );

LONGLONG
__cdecl
InterlockedDecrement64 (
    __inout LONGLONG volatile *Addend
    );

LONG
__cdecl
InterlockedIncrementAcquire (
    __inout LONG volatile *Addend
    );

LONG
__cdecl
InterlockedDecrementAcquire (
    __inout LONG volatile *Addend
    );

LONG
__cdecl
InterlockedIncrementRelease (
    __inout LONG volatile *Addend
    );

LONG
__cdecl
InterlockedDecrementRelease (
    __inout LONG volatile *Addend
    );

LONGLONG
__cdecl
InterlockedExchange64 (
    __inout LONGLONG volatile *Target,
    __in    LONGLONG Value
    );

LONGLONG
__cdecl
InterlockedExchangeAcquire64 (
    __inout LONGLONG volatile *Target,
    __in    LONGLONG Value
    );

LONGLONG
__cdecl
InterlockedExchangeAdd64 (
    __inout LONGLONG volatile *Addend,
    __in    LONGLONG Value
    );

LONGLONG
__cdecl
InterlockedCompareExchange64 (
    __inout LONGLONG volatile *Destination,
    __in    LONGLONG ExChange,
    __in    LONGLONG Comperand
    );

LONGLONG
__cdecl
InterlockedCompareExchangeAcquire64 (
    __inout LONGLONG volatile *Destination,
    __in    LONGLONG ExChange,
    __in    LONGLONG Comperand
    );

LONGLONG
__cdecl
InterlockedCompareExchangeRelease64 (
    __inout LONGLONG volatile *Destination,
    __in    LONGLONG ExChange,
    __in    LONGLONG Comperand
    );

LONG64
__cdecl
InterlockedCompare64Exchange128(
    __inout LONG64 volatile * Destination,
    __in LONG64 ExchangeHigh,
    __in LONG64 ExchangeLow,
    __in LONG64 Comperand
    );

LONG64
__cdecl
InterlockedCompare64ExchangeAcquire128(
    __inout LONG64 volatile * Destination,
    __in LONG64 ExchangeHigh,
    __in LONG64 ExchangeLow,
    __in LONG64 Comperand
    );

LONG64
__cdecl
InterlockedCompare64ExchangeRelease128(
    __inout LONG64 volatile * Destination,
    __in LONG64 ExchangeHigh,
    __in LONG64 ExchangeLow,
    __in LONG64 Comperand
    );

LONG
__cdecl
InterlockedIncrement (
    __inout LONG volatile *lpAddend
    );

LONG
__cdecl
InterlockedDecrement (
    __inout LONG volatile *lpAddend
    );

LONG
__cdecl
InterlockedExchange (
    __inout LONG volatile *Target,
    __in    LONG Value
    );

LONG
__cdecl
InterlockedExchangeAdd (
    __inout LONG volatile *Addend,
    __in    LONG Value
    );

LONG
__cdecl
InterlockedCompareExchange (
    __inout LONG volatile *Destination,
    __in    LONG ExChange,
    __in    LONG Comperand
    );

LONG
__cdecl
InterlockedCompareExchangeRelease (
    __inout LONG volatile *Destination,
    __in    LONG ExChange,
    __in    LONG Comperand
    );

LONG
__cdecl
InterlockedCompareExchangeAcquire (
    __inout LONG volatile *Destination,
    __in    LONG ExChange,
    __in    LONG Comperand
    );

PVOID
__cdecl
InterlockedExchangePointer (
    __inout  PVOID volatile *Target,
    __in_opt PVOID Value
    );

PVOID
__cdecl
InterlockedCompareExchangePointer (
    __inout  PVOID volatile *Destination,
    __in_opt PVOID ExChange,
    __in_opt PVOID Comperand
    );

PVOID
__cdecl
InterlockedCompareExchangePointerAcquire (
    __inout  PVOID volatile *Destination,
    __in_opt PVOID Exchange,
    __in_opt PVOID Comperand
    );

PVOID
__cdecl
InterlockedCompareExchangePointerRelease (
    __inout  PVOID volatile *Destination,
    __in_opt PVOID Exchange,
    __in_opt PVOID Comperand
    );


#if !defined(MIDL_PASS)

#if !defined (InterlockedAnd)

#define InterlockedAnd InterlockedAnd_Inline

FORCEINLINE
LONG
InterlockedAnd_Inline (
    __inout LONG volatile *Target,
    __in    LONG Set
    )
{
    LONG i;
    LONG j;

    j = *Target;
    do {
        i = j;
        j = InterlockedCompareExchange(Target,
                                       i & Set,
                                       i);

    } while (i != j);

    return j;
}

#endif

#if !defined (InterlockedOr)

#define InterlockedOr InterlockedOr_Inline

FORCEINLINE
LONG
InterlockedOr_Inline (
    __inout LONG volatile *Target,
    __in    LONG Set
    )
{
    LONG i;
    LONG j;

    j = *Target;
    do {
        i = j;
        j = InterlockedCompareExchange(Target,
                                       i | Set,
                                       i);

    } while (i != j);

    return j;
}

#endif

#if !defined (InterlockedXor)

#define InterlockedXor InterlockedXor_Inline

FORCEINLINE
LONG
InterlockedXor_Inline (
    __inout LONG volatile *Target,
    __in    LONG Set
    )
{
    LONG i;
    LONG j;

    j = *Target;
    do {
        i = j;
        j = InterlockedCompareExchange(Target,
                                       i ^ Set,
                                       i);

    } while (i != j);

    return j;
}

#endif

#if !defined (InterlockedAnd64)

#define InterlockedAnd64 InterlockedAnd64_Inline

FORCEINLINE
LONGLONG
InterlockedAnd64_Inline (
    __inout LONGLONG volatile *Destination,
    __in    LONGLONG Value
    )
{
    LONGLONG Old;

    do {
        Old = *Destination;
    } while (InterlockedCompareExchange64(Destination,
                                          Old & Value,
                                          Old) != Old);

    return Old;
}

#endif

#if !defined (InterlockedOr64)

#define InterlockedOr64 InterlockedOr64_Inline

FORCEINLINE
LONGLONG
InterlockedOr64_Inline (
    __inout LONGLONG volatile *Destination,
    __in    LONGLONG Value
    )
{
    LONGLONG Old;

    do {
        Old = *Destination;
    } while (InterlockedCompareExchange64(Destination,
                                          Old | Value,
                                          Old) != Old);

    return Old;
}

#endif

#if !defined (InterlockedXor64)

#define InterlockedXor64 InterlockedXor64_Inline

FORCEINLINE
LONGLONG
InterlockedXor64_Inline (
    __inout LONGLONG volatile *Destination,
    __in    LONGLONG Value
    )
{
    LONGLONG Old;

    do {
        Old = *Destination;
    } while (InterlockedCompareExchange64(Destination,
                                          Old ^ Value,
                                          Old) != Old);

    return Old;
}

#endif

#if !defined (InterlockedBitTestAndSet)

#define InterlockedBitTestAndSet InterlockedBitTestAndSet_Inline

FORCEINLINE
BOOLEAN
InterlockedBitTestAndSet_Inline (
    __inout LONG volatile *Base,
    __in LONG Bit
    )
{
    LONG tBit;

    tBit = 1<<(Bit & (sizeof (*Base)*8-1));
    return (BOOLEAN)((InterlockedOr(&Base[Bit/(sizeof(*Base)*8)], tBit)&tBit) != 0);
}

#endif

#if !defined (InterlockedBitTestAndReset)

#define InterlockedBitTestAndReset InterlockedBitTestAndReset_Inline

FORCEINLINE
BOOLEAN
InterlockedBitTestAndReset_Inline (
    __inout LONG volatile *Base,
    __in LONG Bit
    )
{
    LONG tBit;

    tBit = 1<<(Bit & (sizeof (*Base)*8-1));
    return (BOOLEAN)((InterlockedAnd(&Base[Bit/(sizeof(*Base)*8)], ~tBit)&tBit) != 0);
}

#endif

#if !defined (InterlockedBitTestAndComplement)

#define InterlockedBitTestAndComplement InterlockedBitTestAndComplement_Inline

FORCEINLINE
BOOLEAN
InterlockedBitTestAndComplement_Inline (
    __inout LONG volatile *Base,
    __in LONG Bit
    )
{
    LONG tBit;

    tBit = 1<<(Bit & (sizeof (*Base)*8-1));
    return (BOOLEAN)((InterlockedXor(&Base[Bit/(sizeof(*Base)*8)], tBit)&tBit) != 0);
}

#endif
#endif

#pragma intrinsic(_InterlockedIncrement)
#pragma intrinsic(_InterlockedIncrement_acq)
#pragma intrinsic(_InterlockedIncrement_rel)
#pragma intrinsic(_InterlockedDecrement)
#pragma intrinsic(_InterlockedDecrement_acq)
#pragma intrinsic(_InterlockedDecrement_rel)
#pragma intrinsic(_InterlockedExchange)
#pragma intrinsic(_InterlockedExchangeAdd)
#pragma intrinsic(_InterlockedCompareExchange)
#pragma intrinsic(_InterlockedCompareExchange_acq)
#pragma intrinsic(_InterlockedCompareExchange_rel)
#pragma intrinsic(_InterlockedExchangePointer)
#pragma intrinsic(_InterlockedCompareExchangePointer)
#pragma intrinsic(_InterlockedCompareExchangePointer_acq)
#pragma intrinsic(_InterlockedCompareExchangePointer_rel)
#pragma intrinsic(_InterlockedIncrement64)
#pragma intrinsic(_InterlockedDecrement64)
#pragma intrinsic(_InterlockedExchange64)
#pragma intrinsic(_InterlockedExchange64_acq)
#pragma intrinsic(_InterlockedCompareExchange64)
#pragma intrinsic(_InterlockedCompareExchange64_acq)
#pragma intrinsic(_InterlockedCompareExchange64_rel)
#pragma intrinsic(_InterlockedExchangeAdd64)
#pragma intrinsic (_InterlockedOr)
#pragma intrinsic (_InterlockedOr_acq)
#pragma intrinsic (_InterlockedOr_rel)
#pragma intrinsic (_InterlockedOr8)
#pragma intrinsic (_InterlockedOr8_acq)
#pragma intrinsic (_InterlockedOr8_rel)
#pragma intrinsic (_InterlockedOr16)
#pragma intrinsic (_InterlockedOr16_acq)
#pragma intrinsic (_InterlockedOr16_rel)
#pragma intrinsic (_InterlockedOr64)
#pragma intrinsic (_InterlockedOr64_acq)
#pragma intrinsic (_InterlockedOr64_rel)
#pragma intrinsic (_InterlockedXor)
#pragma intrinsic (_InterlockedXor_acq)
#pragma intrinsic (_InterlockedXor_rel)
#pragma intrinsic (_InterlockedXor8)
#pragma intrinsic (_InterlockedXor8_acq)
#pragma intrinsic (_InterlockedXor8_rel)
#pragma intrinsic (_InterlockedXor16)
#pragma intrinsic (_InterlockedXor16_acq)
#pragma intrinsic (_InterlockedXor16_rel)
#pragma intrinsic (_InterlockedXor64)
#pragma intrinsic (_InterlockedXor64_acq)
#pragma intrinsic (_InterlockedXor64_rel)
#pragma intrinsic (_InterlockedAnd)
#pragma intrinsic (_InterlockedAnd_acq)
#pragma intrinsic (_InterlockedAnd_rel)
#pragma intrinsic (_InterlockedAnd8)
#pragma intrinsic (_InterlockedAnd8_acq)
#pragma intrinsic (_InterlockedAnd8_rel)
#pragma intrinsic (_InterlockedAnd16)
#pragma intrinsic (_InterlockedAnd16_acq)
#pragma intrinsic (_InterlockedAnd16_rel)
#pragma intrinsic (_InterlockedAnd64)
#pragma intrinsic (_InterlockedAnd64_acq)
#pragma intrinsic (_InterlockedAnd64_rel)

#elif defined(_M_AMD64) && !defined(RC_INVOKED)

#define InterlockedAnd _InterlockedAnd
#define InterlockedOr _InterlockedOr
#define InterlockedXor _InterlockedXor
#define InterlockedIncrement _InterlockedIncrement
#define InterlockedIncrementAcquire InterlockedIncrement
#define InterlockedIncrementRelease InterlockedIncrement
#define InterlockedDecrement _InterlockedDecrement
#define InterlockedDecrementAcquire InterlockedDecrement
#define InterlockedDecrementRelease InterlockedDecrement
#define InterlockedExchange _InterlockedExchange
#define InterlockedExchangeAdd _InterlockedExchangeAdd
#define InterlockedCompareExchange _InterlockedCompareExchange
#define InterlockedCompareExchangeAcquire InterlockedCompareExchange
#define InterlockedCompareExchangeRelease InterlockedCompareExchange
#define InterlockedExchangePointer _InterlockedExchangePointer
#define InterlockedCompareExchangePointer _InterlockedCompareExchangePointer
#define InterlockedCompareExchangePointerAcquire _InterlockedCompareExchangePointer
#define InterlockedCompareExchangePointerRelease _InterlockedCompareExchangePointer

#define InterlockedAnd64 _InterlockedAnd64
#define InterlockedOr64 _InterlockedOr64
#define InterlockedXor64 _InterlockedXor64
#define InterlockedIncrement64 _InterlockedIncrement64
#define InterlockedDecrement64 _InterlockedDecrement64
#define InterlockedExchange64 _InterlockedExchange64
#define InterlockedExchangeAdd64 _InterlockedExchangeAdd64
#define InterlockedCompareExchange64 _InterlockedCompareExchange64
#define InterlockedCompareExchangeAcquire64 InterlockedCompareExchange64
#define InterlockedCompareExchangeRelease64 InterlockedCompareExchange64


LONG
InterlockedAnd (
    __inout LONG volatile *Destination,
    __in LONG Value
    );

LONG
InterlockedOr (
    __inout LONG volatile *Destination,
    __in LONG Value
    );

LONG
InterlockedXor (
    __inout LONG volatile *Destination,
    __in LONG Value
    );

LONG
InterlockedIncrement (
    __inout LONG volatile *Addend
    );

LONG
InterlockedDecrement (
    __inout LONG volatile *Addend
    );

LONG
InterlockedExchange (
    __inout LONG volatile *Target,
    __in LONG Value
    );

LONG
InterlockedExchangeAdd (
    __inout LONG volatile *Addend,
    __in LONG Value
    );

LONG
InterlockedCompareExchange (
    __inout LONG volatile *Destination,
    __in LONG ExChange,
    __in LONG Comperand
    );

PVOID
InterlockedCompareExchangePointer (
    __inout  PVOID volatile *Destination,
    __in_opt PVOID Exchange,
    __in_opt PVOID Comperand
    );

PVOID
InterlockedExchangePointer (
    __inout  PVOID volatile *Target,
    __in_opt PVOID Value
    );

LONG64
InterlockedAnd64 (
    __inout LONG64 volatile *Destination,
    __in LONG64 Value
    );

LONG64
InterlockedOr64 (
    __inout LONG64 volatile *Destination,
    __in LONG64 Value
    );

LONG64
InterlockedXor64 (
    __inout LONG64 volatile *Destination,
    __in LONG64 Value
    );

LONG64
InterlockedIncrement64 (
    __inout LONG64 volatile *Addend
    );

LONG64
InterlockedDecrement64 (
    __inout LONG64 volatile *Addend
    );

LONG64
InterlockedExchange64 (
    __inout LONG64 volatile *Target,
    __in LONG64 Value
    );

LONG64
InterlockedExchangeAdd64 (
    __inout LONG64 volatile *Addend,
    __in LONG64 Value
    );

LONG64
InterlockedCompareExchange64 (
    __inout LONG64 volatile *Destination,
    __in LONG64 ExChange,
    __in LONG64 Comperand
    );

#pragma intrinsic(_InterlockedAnd)
#pragma intrinsic(_InterlockedOr)
#pragma intrinsic(_InterlockedXor)
#pragma intrinsic(_InterlockedIncrement)
#pragma intrinsic(_InterlockedDecrement)
#pragma intrinsic(_InterlockedExchange)
#pragma intrinsic(_InterlockedExchangeAdd)
#pragma intrinsic(_InterlockedCompareExchange)
#pragma intrinsic(_InterlockedExchangePointer)
#pragma intrinsic(_InterlockedCompareExchangePointer)
#pragma intrinsic(_InterlockedAnd64)
#pragma intrinsic(_InterlockedOr64)
#pragma intrinsic(_InterlockedXor64)
#pragma intrinsic(_InterlockedIncrement64)
#pragma intrinsic(_InterlockedDecrement64)
#pragma intrinsic(_InterlockedExchange64)
#pragma intrinsic(_InterlockedExchangeAdd64)
#pragma intrinsic(_InterlockedCompareExchange64)

#if _MSC_FULL_VER >= 140041204

#define InterlockedAnd8 _InterlockedAnd8
#define InterlockedOr8 _InterlockedOr8
#define InterlockedXor8 _InterlockedXor8
#define InterlockedAnd16 _InterlockedAnd16
#define InterlockedOr16 _InterlockedOr16
#define InterlockedXor16 _InterlockedXor16

char
InterlockedAnd8 (
    __inout char volatile *Destination,
    __in char Value
    );

char
InterlockedOr8 (
    __inout char volatile *Destination,
    __in char Value
    );

char
InterlockedXor8 (
    __inout char volatile *Destination,
    __in char Value
    );

SHORT
InterlockedAnd16(
    __inout SHORT volatile *Destination,
    __in SHORT Value
    );

SHORT
InterlockedOr16(
    __inout SHORT volatile *Destination,
    __in SHORT Value
    );

SHORT
InterlockedXor16(
    __inout SHORT volatile *Destination,
    __in SHORT Value
    );

#pragma intrinsic (_InterlockedAnd8)
#pragma intrinsic (_InterlockedOr8)
#pragma intrinsic (_InterlockedXor8)
#pragma intrinsic (_InterlockedAnd16)
#pragma intrinsic (_InterlockedOr16)
#pragma intrinsic (_InterlockedXor16)

#endif

#else           // X86 interlocked definitions

WINBASEAPI
LONG
WINAPI
InterlockedIncrement (
    __inout LONG volatile *lpAddend
    );

WINBASEAPI
LONG
WINAPI
InterlockedDecrement (
    __inout LONG volatile *lpAddend
    );

WINBASEAPI
LONG
WINAPI
InterlockedExchange (
    __inout LONG volatile *Target,
    __in    LONG Value
    );

#define InterlockedExchangePointer(Target, Value) \
    (PVOID)InterlockedExchange((PLONG)(Target), (LONG)(Value))

WINBASEAPI
LONG
WINAPI
InterlockedExchangeAdd (
    __inout LONG volatile *Addend,
    __in    LONG Value
    );

WINBASEAPI
LONG
WINAPI
InterlockedCompareExchange (
    __inout LONG volatile *Destination,
    __in    LONG Exchange,
    __in    LONG Comperand
    );

#if (_WIN32_WINNT >= 0x0502)

WINBASEAPI
LONGLONG
WINAPI
InterlockedCompareExchange64 (
    __inout LONGLONG volatile *Destination,
    __in    LONGLONG Exchange,
    __in    LONGLONG Comperand
    );

#endif

#if !defined(MIDL_PASS)

#if (_WIN32_WINNT >= 0x0502)

FORCEINLINE
LONGLONG
InterlockedAnd64 (
    __inout LONGLONG volatile *Destination,
    __in    LONGLONG Value
    )
{
    LONGLONG Old;

    do {
        Old = *Destination;
    } while (InterlockedCompareExchange64(Destination,
                                          Old & Value,
                                          Old) != Old);

    return Old;
}

FORCEINLINE
LONGLONG
InterlockedOr64 (
    __inout LONGLONG volatile *Destination,
    __in    LONGLONG Value
    )
{
    LONGLONG Old;

    do {
        Old = *Destination;
    } while (InterlockedCompareExchange64(Destination,
                                          Old | Value,
                                          Old) != Old);

    return Old;
}

FORCEINLINE
LONGLONG
InterlockedXor64 (
    __inout LONGLONG volatile *Destination,
    __in    LONGLONG Value
    )
{
    LONGLONG Old;

    do {
        Old = *Destination;
    } while (InterlockedCompareExchange64(Destination,
                                          Old ^ Value,
                                          Old) != Old);

    return Old;
}

FORCEINLINE
LONGLONG
InterlockedIncrement64 (
    __inout LONGLONG volatile *Addend
    )
{
    LONGLONG Old;

    do {
        Old = *Addend;
    } while (InterlockedCompareExchange64(Addend,
                                          Old + 1,
                                          Old) != Old);

    return Old + 1;
}

FORCEINLINE
LONGLONG
InterlockedDecrement64 (
    __inout LONGLONG volatile *Addend
    )
{
    LONGLONG Old;

    do {
        Old = *Addend;
    } while (InterlockedCompareExchange64(Addend,
                                          Old - 1,
                                          Old) != Old);

    return Old - 1;
}

FORCEINLINE
LONGLONG
InterlockedExchange64 (
    __inout LONGLONG volatile *Target,
    __in    LONGLONG Value
    )
{
    LONGLONG Old;

    do {
        Old = *Target;
    } while (InterlockedCompareExchange64(Target,
                                          Value,
                                          Old) != Old);

    return Old;
}

FORCEINLINE
LONGLONG
InterlockedExchangeAdd64(
    __inout LONGLONG volatile *Addend,
    __in    LONGLONG Value
    )
{
    LONGLONG Old;

    do {
        Old = *Addend;
    } while (InterlockedCompareExchange64(Addend,
                                          Old + Value,
                                          Old) != Old);

    return Old;
}

#endif


#endif

//
// Use a function for C++ so X86 will generate the same errors as RISC.
//

#ifdef __cplusplus

FORCEINLINE
PVOID
#if !defined(_M_CEE_PURE)
__cdecl
#endif
__InlineInterlockedCompareExchangePointer (
    __inout  PVOID volatile *Destination,
    __in_opt PVOID ExChange,
    __in_opt PVOID Comperand
    )
{
    return((PVOID)(LONG_PTR)InterlockedCompareExchange((LONG volatile *)Destination, (LONG)(LONG_PTR)ExChange, (LONG)(LONG_PTR)Comperand));
}

#define InterlockedCompareExchangePointer __InlineInterlockedCompareExchangePointer

#else

#define InterlockedCompareExchangePointer(Destination, ExChange, Comperand) \
    (PVOID)(LONG_PTR)InterlockedCompareExchange((LONG volatile *)(Destination), (LONG)(LONG_PTR)(ExChange), (LONG)(LONG_PTR)(Comperand))

#endif /* __cplusplus */

#define InterlockedIncrementAcquire InterlockedIncrement
#define InterlockedIncrementRelease InterlockedIncrement
#define InterlockedDecrementAcquire InterlockedDecrement
#define InterlockedDecrementRelease InterlockedDecrement
#define InterlockedIncrementAcquire InterlockedIncrement
#define InterlockedIncrementRelease InterlockedIncrement
#define InterlockedCompareExchangeAcquire InterlockedCompareExchange
#define InterlockedCompareExchangeRelease InterlockedCompareExchange
#define InterlockedCompareExchangeAcquire64 InterlockedCompareExchange64
#define InterlockedCompareExchangeRelease64 InterlockedCompareExchange64
#define InterlockedCompareExchangePointerAcquire InterlockedCompareExchangePointer
#define InterlockedCompareExchangePointerRelease InterlockedCompareExchangePointer

#endif /* X86 | IA64 */

#if defined(_SLIST_HEADER_) && !defined(_NTOSP_)

WINBASEAPI
VOID
WINAPI
InitializeSListHead (
    __inout PSLIST_HEADER ListHead
    );

WINBASEAPI
PSLIST_ENTRY
WINAPI
InterlockedPopEntrySList (
    __inout PSLIST_HEADER ListHead
    );

WINBASEAPI
PSLIST_ENTRY
WINAPI
InterlockedPushEntrySList (
    __inout PSLIST_HEADER ListHead,
    __inout PSLIST_ENTRY ListEntry
    );

WINBASEAPI
PSLIST_ENTRY
WINAPI
InterlockedFlushSList (
    __inout PSLIST_HEADER ListHead
    );

WINBASEAPI
USHORT
WINAPI
QueryDepthSList (
    __in PSLIST_HEADER ListHead
    );

#endif /* _SLIST_HEADER_ */

#endif /* _NTOS_ */

#endif /* NOWINBASEINTERLOCK */

WINBASEAPI
BOOL
WINAPI
FreeResource(
    __in HGLOBAL hResData
    );

WINBASEAPI
LPVOID
WINAPI
LockResource(
    __in HGLOBAL hResData
    );

#define UnlockResource(hResData) ((hResData), 0)
#define MAXINTATOM 0xC000
#define MAKEINTATOM(i)  (LPTSTR)((ULONG_PTR)((WORD)(i)))
#define INVALID_ATOM ((ATOM)0)

int
#if !defined(_MAC)
#if defined(_M_CEE_PURE)
__clrcall
#else
WINAPI
#endif
#else
CALLBACK
#endif
WinMain (
    __in HINSTANCE hInstance,
    __in_opt HINSTANCE hPrevInstance,
    __in LPSTR lpCmdLine,
    __in int nShowCmd
    );

int
#if defined(_M_CEE_PURE)
__clrcall
#else
WINAPI
#endif
wWinMain(
    __in HINSTANCE hInstance,
    __in_opt HINSTANCE hPrevInstance,
    __in LPWSTR lpCmdLine,
    __in int nShowCmd
    );

WINBASEAPI
BOOL
WINAPI
FreeLibrary (
    __in HMODULE hLibModule
    );


WINBASEAPI
DECLSPEC_NORETURN
VOID
WINAPI
FreeLibraryAndExitThread (
    __in HMODULE hLibModule,
    __in DWORD dwExitCode
    );

WINBASEAPI
BOOL
WINAPI
DisableThreadLibraryCalls (
    __in HMODULE hLibModule
    );

WINBASEAPI
FARPROC
WINAPI
GetProcAddress (
    __in HMODULE hModule,
    __in LPCSTR lpProcName
    );

WINBASEAPI
DWORD
WINAPI
GetVersion (
    VOID
    );

WINBASEAPI
__out_opt
HGLOBAL
WINAPI
GlobalAlloc (
    __in UINT uFlags,
    __in SIZE_T dwBytes
    );

WINBASEAPI
__out_opt
HGLOBAL
WINAPI
GlobalReAlloc (
    __in HGLOBAL hMem,
    __in SIZE_T dwBytes,
    __in UINT uFlags
    );

WINBASEAPI
SIZE_T
WINAPI
GlobalSize (
    __in HGLOBAL hMem
    );

WINBASEAPI
UINT
WINAPI
GlobalFlags (
    __in HGLOBAL hMem
    );

WINBASEAPI
__out_opt
LPVOID
WINAPI
GlobalLock (
    __in HGLOBAL hMem
    );

WINBASEAPI
__out_opt
HGLOBAL
WINAPI
GlobalHandle (
    __in LPCVOID pMem
    );

WINBASEAPI
BOOL
WINAPI
GlobalUnlock(
    __in HGLOBAL hMem
    );

WINBASEAPI
__out_opt
HGLOBAL
WINAPI
GlobalFree(
    __deref HGLOBAL hMem
    );

WINBASEAPI
SIZE_T
WINAPI
GlobalCompact(
    __in DWORD dwMinFree
    );

WINBASEAPI
VOID
WINAPI
GlobalFix(
    __in HGLOBAL hMem
    );

WINBASEAPI
VOID
WINAPI
GlobalUnfix(
    __in HGLOBAL hMem
    );

WINBASEAPI
__out
LPVOID
WINAPI
GlobalWire(
    __in HGLOBAL hMem
    );

WINBASEAPI
BOOL
WINAPI
GlobalUnWire(
    __in HGLOBAL hMem
    );

WINBASEAPI
VOID
WINAPI
GlobalMemoryStatus(
    __out LPMEMORYSTATUS lpBuffer
    );

typedef struct _MEMORYSTATUSEX {
    DWORD dwLength;
    DWORD dwMemoryLoad;
    DWORDLONG ullTotalPhys;
    DWORDLONG ullAvailPhys;
    DWORDLONG ullTotalPageFile;
    DWORDLONG ullAvailPageFile;
    DWORDLONG ullTotalVirtual;
    DWORDLONG ullAvailVirtual;
    DWORDLONG ullAvailExtendedVirtual;
} MEMORYSTATUSEX, *LPMEMORYSTATUSEX;

WINBASEAPI
BOOL
WINAPI
GlobalMemoryStatusEx(
    __out LPMEMORYSTATUSEX lpBuffer
    );

WINBASEAPI
__out_bcount_opt( (uFlags&LMEM_FIXED) ? uBytes : 0 )
HLOCAL
WINAPI
LocalAlloc(
    __in UINT uFlags,
    __in SIZE_T uBytes
    );

WINBASEAPI
__out_opt
HLOCAL
WINAPI
LocalReAlloc(
    __in HLOCAL hMem,
    __in SIZE_T uBytes,
    __in UINT uFlags
    );

WINBASEAPI
__out_opt
LPVOID
WINAPI
LocalLock(
    __in HLOCAL hMem
    );

WINBASEAPI
__out_opt
HLOCAL
WINAPI
LocalHandle(
    __in LPCVOID pMem
    );

WINBASEAPI
BOOL
WINAPI
LocalUnlock(
    __in HLOCAL hMem
    );

WINBASEAPI
SIZE_T
WINAPI
LocalSize(
    __in HLOCAL hMem
    );

WINBASEAPI
UINT
WINAPI
LocalFlags(
    __in HLOCAL hMem
    );

WINBASEAPI
HLOCAL
WINAPI
LocalFree(
    __deref HLOCAL hMem
    );

WINBASEAPI
SIZE_T
WINAPI
LocalShrink(
    __in HLOCAL hMem,
    __in UINT cbNewSize
    );

WINBASEAPI
SIZE_T
WINAPI
LocalCompact(
    __in UINT uMinFree
    );

WINBASEAPI
BOOL
WINAPI
FlushInstructionCache(
    __in HANDLE hProcess,
    __in_bcount_opt(dwSize) LPCVOID lpBaseAddress,
    __in SIZE_T dwSize
    );

#if (_WIN32_WINNT >= 0x0600)

WINBASEAPI
VOID
WINAPI
FlushProcessWriteBuffers(
    VOID
    );

WINBASEAPI
BOOL
WINAPI
QueryThreadCycleTime (
    __in HANDLE ThreadHandle,
    __out PULONG64 CycleTime
    );

WINBASEAPI
BOOL
WINAPI
QueryProcessCycleTime (
    __in HANDLE ProcessHandle,
    __out PULONG64 CycleTime
    );

WINBASEAPI
BOOL
WINAPI
QueryIdleProcessorCycleTime (
    __inout PULONG BufferLength,
    __out_bcount_opt(*BufferLength) PULONG64 ProcessorIdleCycleTime
    );

#endif

#if (_WIN32_WINNT >= 0x0601)

WINBASEAPI
BOOL
WINAPI
QueryIdleProcessorCycleTimeEx (
    __in USHORT Group,
    __inout PULONG BufferLength,
    __out_bcount_opt(*BufferLength) PULONG64 ProcessorIdleCycleTime
    );

WINBASEAPI
BOOL
WINAPI
QueryUnbiasedInterruptTime (
    __out PULONGLONG UnbiasedTime
    );

WINBASEAPI
BOOL
WINAPI
GetProcessorSystemCycleTime (
    __in USHORT Group,
    __out_bcount_part_opt(*ReturnedLength, *ReturnedLength) PSYSTEM_PROCESSOR_CYCLE_TIME_INFORMATION Buffer,
    __inout PDWORD ReturnedLength
    );

#endif // (_WIN32_WINNT >= 0x0601)

WINBASEAPI
__bcount_opt(dwSize)
LPVOID
WINAPI
VirtualAlloc(
    __in_opt LPVOID lpAddress,
    __in     SIZE_T dwSize,
    __in     DWORD flAllocationType,
    __in     DWORD flProtect
    );

__drv_when(((dwFreeType&(MEM_RELEASE|MEM_DECOMMIT)))==(MEM_RELEASE|MEM_DECOMMIT),
    __drv_reportError("Passing both MEM_RELEASE and MEM_DECOMMIT to VirtualFree is not allowed. This results in the failure of this call"))

__drv_when(dwFreeType==0,
    __drv_reportError("Passing zero as the dwFreeType parameter to VirtualFree is not allowed. This results in the failure of this call"))

__drv_when(((dwFreeType&MEM_RELEASE))!=0 && dwSize!=0,
    __drv_reportError("Passing MEM_RELEASE and a non-zero dwSize parameter to VirtualFree is not allowed. This results in the failure of this call"))
WINBASEAPI
BOOL
WINAPI
VirtualFree(
    __in LPVOID lpAddress,
    __in SIZE_T dwSize,
    __in DWORD dwFreeType
    );

WINBASEAPI
BOOL
WINAPI
VirtualProtect(
    __in  LPVOID lpAddress,
    __in  SIZE_T dwSize,
    __in  DWORD flNewProtect,
    __out PDWORD lpflOldProtect
    );

WINBASEAPI
SIZE_T
WINAPI
VirtualQuery(
    __in_opt LPCVOID lpAddress,
    __out_bcount_part(dwLength, return) PMEMORY_BASIC_INFORMATION lpBuffer,
    __in     SIZE_T dwLength
    );

WINBASEAPI
__bcount_opt(dwSize)
LPVOID
WINAPI
VirtualAllocEx(
    __in     HANDLE hProcess,
    __in_opt LPVOID lpAddress,
    __in     SIZE_T dwSize,
    __in     DWORD flAllocationType,
    __in     DWORD flProtect
    );

#if _WIN32_WINNT >= 0x0600

WINBASEAPI
__bcount(dwSize)
LPVOID
WINAPI
VirtualAllocExNuma(
    __in     HANDLE hProcess,
    __in_opt LPVOID lpAddress,
    __in     SIZE_T dwSize,
    __in     DWORD  flAllocationType,
    __in     DWORD  flProtect,
    __in     DWORD  nndPreferred
    );

#endif // _WIN32_WINNT >= 0x0600

WINBASEAPI
UINT
WINAPI
GetWriteWatch(
    __in DWORD dwFlags,
    __in PVOID lpBaseAddress,
    __in SIZE_T dwRegionSize,
    __out_ecount_part(*lpdwCount, *lpdwCount) PVOID *lpAddresses,
    __inout ULONG_PTR *lpdwCount,
    __out PULONG lpdwGranularity
    );

WINBASEAPI
UINT
WINAPI
ResetWriteWatch(
    __in LPVOID lpBaseAddress,
    __in SIZE_T dwRegionSize
    );

WINBASEAPI
SIZE_T
WINAPI
GetLargePageMinimum(
    VOID
    );

WINBASEAPI
UINT
WINAPI
EnumSystemFirmwareTables(
    __in DWORD FirmwareTableProviderSignature,
    __out_bcount_part_opt(BufferSize, return) PVOID pFirmwareTableEnumBuffer,
    __in DWORD BufferSize
    );

WINBASEAPI
UINT
WINAPI
GetSystemFirmwareTable(
    __in DWORD FirmwareTableProviderSignature,
    __in DWORD FirmwareTableID,
    __out_bcount_part_opt(BufferSize, return) PVOID pFirmwareTableBuffer,
    __in DWORD BufferSize
    );

WINBASEAPI
BOOL
WINAPI
GetPhysicallyInstalledSystemMemory(
    __out PULONGLONG TotalMemoryInKilobytes
    );

__drv_when(((dwFreeType&(MEM_RELEASE|MEM_DECOMMIT)))==(MEM_RELEASE|MEM_DECOMMIT),
    __drv_reportError("Passing both MEM_RELEASE and MEM_DECOMMIT to VirtualFree is not allowed. This results in the failure of this call"))

__drv_when(dwFreeType==0,
    __drv_reportError("Passing zero as the dwFreeType parameter to VirtualFree is not allowed. This results in the failure of this call"))

__drv_when(((dwFreeType&MEM_RELEASE))!=0 && dwSize!=0,
    __drv_reportError("Passing MEM_RELEASE and a non-zero dwSize parameter to VirtualFree is not allowed. This results in the failure of this call"))

__drv_when(((dwFreeType&MEM_DECOMMIT))!=0,
    __drv_reportError("Calling VirtualFreeEx without the MEM_RELEASE flag frees memory but not address descriptors (VADs); results in address space leaks"))
WINBASEAPI
BOOL
WINAPI
VirtualFreeEx(
    __in HANDLE hProcess,
    __in LPVOID lpAddress,
    __in SIZE_T dwSize,
    __in DWORD  dwFreeType
    );

WINBASEAPI
BOOL
WINAPI
VirtualProtectEx(
    __in  HANDLE hProcess,
    __in  LPVOID lpAddress,
    __in  SIZE_T dwSize,
    __in  DWORD flNewProtect,
    __out PDWORD lpflOldProtect
    );

WINBASEAPI
SIZE_T
WINAPI
VirtualQueryEx(
    __in     HANDLE hProcess,
    __in_opt LPCVOID lpAddress,
    __out_bcount_part(dwLength, return) PMEMORY_BASIC_INFORMATION lpBuffer,
    __in     SIZE_T dwLength
    );

WINBASEAPI
__out_opt
HANDLE
WINAPI
HeapCreate(
    __in DWORD flOptions,
    __in SIZE_T dwInitialSize,
    __in SIZE_T dwMaximumSize
    );

WINBASEAPI
BOOL
WINAPI
HeapDestroy(
    __in HANDLE hHeap
    );

WINBASEAPI
__bcount(dwBytes)
LPVOID
WINAPI
HeapAlloc(
    __in HANDLE hHeap,
    __in DWORD dwFlags,
    __in SIZE_T dwBytes
    );

WINBASEAPI
__bcount(dwBytes)
LPVOID
WINAPI
HeapReAlloc(
    __inout HANDLE hHeap,
    __in    DWORD dwFlags,
    __deref LPVOID lpMem,
    __in    SIZE_T dwBytes
    );

WINBASEAPI
BOOL
WINAPI
HeapFree(
    __inout HANDLE hHeap,
    __in    DWORD dwFlags,
    __drv_freesMem(Mem) __post __notvalid __deref LPVOID lpMem
    );

WINBASEAPI
SIZE_T
WINAPI
HeapSize(
    __in HANDLE hHeap,
    __in DWORD dwFlags,
    __in LPCVOID lpMem
    );

WINBASEAPI
BOOL
WINAPI
HeapValidate(
    __in     HANDLE hHeap,
    __in     DWORD dwFlags,
    __in_opt LPCVOID lpMem
    );

WINBASEAPI
SIZE_T
WINAPI
HeapCompact(
    __in HANDLE hHeap,
    __in DWORD dwFlags
    );

WINBASEAPI
__out
HANDLE
WINAPI
GetProcessHeap( VOID );

WINBASEAPI
DWORD
WINAPI
GetProcessHeaps(
    __in DWORD NumberOfHeaps,
    __out_ecount_part(NumberOfHeaps, return) PHANDLE ProcessHeaps
    );

typedef struct _PROCESS_HEAP_ENTRY {
    PVOID lpData;
    DWORD cbData;
    BYTE cbOverhead;
    BYTE iRegionIndex;
    WORD wFlags;
    union {
        struct {
            HANDLE hMem;
            DWORD dwReserved[ 3 ];
        } Block;
        struct {
            DWORD dwCommittedSize;
            DWORD dwUnCommittedSize;
            LPVOID lpFirstBlock;
            LPVOID lpLastBlock;
        } Region;
    } DUMMYUNIONNAME;
} PROCESS_HEAP_ENTRY, *LPPROCESS_HEAP_ENTRY, *PPROCESS_HEAP_ENTRY;

#define PROCESS_HEAP_REGION             0x0001
#define PROCESS_HEAP_UNCOMMITTED_RANGE  0x0002
#define PROCESS_HEAP_ENTRY_BUSY         0x0004
#define PROCESS_HEAP_ENTRY_MOVEABLE     0x0010
#define PROCESS_HEAP_ENTRY_DDESHARE     0x0020

WINBASEAPI
BOOL
WINAPI
HeapLock(
    __in HANDLE hHeap
    );

WINBASEAPI
BOOL
WINAPI
HeapUnlock(
    __in HANDLE hHeap
    );


WINBASEAPI
BOOL
WINAPI
HeapWalk(
    __in    HANDLE hHeap,
    __inout LPPROCESS_HEAP_ENTRY lpEntry
    );

WINBASEAPI
BOOL
WINAPI
HeapSetInformation (
    __in_opt HANDLE HeapHandle,
    __in HEAP_INFORMATION_CLASS HeapInformationClass,
    __in_bcount_opt(HeapInformationLength) PVOID HeapInformation,
    __in SIZE_T HeapInformationLength
    );

WINBASEAPI
BOOL
WINAPI
HeapQueryInformation (
    __in_opt HANDLE HeapHandle,
    __in HEAP_INFORMATION_CLASS HeapInformationClass,
    __out_bcount_part_opt(HeapInformationLength, *ReturnLength) PVOID HeapInformation,
    __in SIZE_T HeapInformationLength,
    __out_opt PSIZE_T ReturnLength
    );

// GetBinaryType return values.

#define SCS_32BIT_BINARY    0
#define SCS_DOS_BINARY      1
#define SCS_WOW_BINARY      2
#define SCS_PIF_BINARY      3
#define SCS_POSIX_BINARY    4
#define SCS_OS216_BINARY    5
#define SCS_64BIT_BINARY    6

#if defined(_WIN64)
# define SCS_THIS_PLATFORM_BINARY SCS_64BIT_BINARY
#else
# define SCS_THIS_PLATFORM_BINARY SCS_32BIT_BINARY
#endif

WINBASEAPI
BOOL
WINAPI
GetBinaryTypeA(
    __in  LPCSTR lpApplicationName,
    __out LPDWORD  lpBinaryType
    );
WINBASEAPI
BOOL
WINAPI
GetBinaryTypeW(
    __in  LPCWSTR lpApplicationName,
    __out LPDWORD  lpBinaryType
    );
#ifdef UNICODE
#define GetBinaryType  GetBinaryTypeW
#else
#define GetBinaryType  GetBinaryTypeA
#endif // !UNICODE

WINBASEAPI
DWORD
WINAPI
GetShortPathNameA(
    __in LPCSTR lpszLongPath,
    __out_ecount_part_opt(cchBuffer, return + 1) LPSTR  lpszShortPath,
    __in DWORD cchBuffer
    );
WINBASEAPI
DWORD
WINAPI
GetShortPathNameW(
    __in LPCWSTR lpszLongPath,
    __out_ecount_part_opt(cchBuffer, return + 1) LPWSTR  lpszShortPath,
    __in DWORD cchBuffer
    );
#ifdef UNICODE
#define GetShortPathName  GetShortPathNameW
#else
#define GetShortPathName  GetShortPathNameA
#endif // !UNICODE

WINBASEAPI
DWORD
WINAPI
GetLongPathNameA(
    __in LPCSTR lpszShortPath,
    __out_ecount_part_opt(cchBuffer, return + 1) LPSTR  lpszLongPath,
    __in DWORD cchBuffer
    );
WINBASEAPI
DWORD
WINAPI
GetLongPathNameW(
    __in LPCWSTR lpszShortPath,
    __out_ecount_part_opt(cchBuffer, return + 1) LPWSTR  lpszLongPath,
    __in DWORD cchBuffer
    );
#ifdef UNICODE
#define GetLongPathName  GetLongPathNameW
#else
#define GetLongPathName  GetLongPathNameA
#endif // !UNICODE

#if _WIN32_WINNT >= 0x0600

WINBASEAPI
DWORD
WINAPI
GetLongPathNameTransactedA(
    __in     LPCSTR lpszShortPath,
    __out_ecount_part_opt(cchBuffer, return + 1) LPSTR  lpszLongPath,
    __in     DWORD cchBuffer,
    __in     HANDLE hTransaction
    );
WINBASEAPI
DWORD
WINAPI
GetLongPathNameTransactedW(
    __in     LPCWSTR lpszShortPath,
    __out_ecount_part_opt(cchBuffer, return + 1) LPWSTR  lpszLongPath,
    __in     DWORD cchBuffer,
    __in     HANDLE hTransaction
    );
#ifdef UNICODE
#define GetLongPathNameTransacted  GetLongPathNameTransactedW
#else
#define GetLongPathNameTransacted  GetLongPathNameTransactedA
#endif // !UNICODE

#endif // _WIN32_WINNT >= 0x0600

WINBASEAPI
BOOL
WINAPI
GetProcessAffinityMask(
    __in  HANDLE hProcess,
    __out PDWORD_PTR lpProcessAffinityMask,
    __out PDWORD_PTR lpSystemAffinityMask
    );

WINBASEAPI
BOOL
WINAPI
SetProcessAffinityMask(
    __in HANDLE hProcess,
    __in DWORD_PTR dwProcessAffinityMask
    );

#if _WIN32_WINNT >= 0x0601

BOOL
WINAPI
GetProcessGroupAffinity(
    __in HANDLE hProcess,
    __inout PUSHORT GroupCount,
    __out_ecount(*GroupCount) PUSHORT GroupArray
    );

#endif // _WIN32_WINNT >= 0x0601

#if _WIN32_WINNT >= 0x0501

WINBASEAPI
BOOL
WINAPI
GetProcessHandleCount(
    __in  HANDLE hProcess,
    __out PDWORD pdwHandleCount
    );

#endif // (_WIN32_WINNT >= 0x0501)

WINBASEAPI
BOOL
WINAPI
GetProcessTimes(
    __in  HANDLE hProcess,
    __out LPFILETIME lpCreationTime,
    __out LPFILETIME lpExitTime,
    __out LPFILETIME lpKernelTime,
    __out LPFILETIME lpUserTime
    );

WINBASEAPI
BOOL
WINAPI
GetProcessIoCounters(
    __in  HANDLE hProcess,
    __out PIO_COUNTERS lpIoCounters
    );

WINBASEAPI
BOOL
WINAPI
GetProcessWorkingSetSize(
    __in  HANDLE hProcess,
    __out PSIZE_T lpMinimumWorkingSetSize,
    __out PSIZE_T lpMaximumWorkingSetSize
    );

WINBASEAPI
BOOL
WINAPI
GetProcessWorkingSetSizeEx(
    __in  HANDLE hProcess,
    __out PSIZE_T lpMinimumWorkingSetSize,
    __out PSIZE_T lpMaximumWorkingSetSize,
    __out PDWORD Flags
    );

WINBASEAPI
BOOL
WINAPI
SetProcessWorkingSetSize(
    __in HANDLE hProcess,
    __in SIZE_T dwMinimumWorkingSetSize,
    __in SIZE_T dwMaximumWorkingSetSize
    );

WINBASEAPI
BOOL
WINAPI
SetProcessWorkingSetSizeEx(
    __in HANDLE hProcess,
    __in SIZE_T dwMinimumWorkingSetSize,
    __in SIZE_T dwMaximumWorkingSetSize,
    __in DWORD Flags
    );

#if (_WIN32_WINNT >= 0x0600)

#define PROCESS_AFFINITY_ENABLE_AUTO_UPDATE 0x00000001UL

WINBASEAPI
BOOL
WINAPI
SetProcessAffinityUpdateMode(
    __in HANDLE hProcess,
    __in DWORD dwFlags
    );

WINBASEAPI
BOOL
WINAPI
QueryProcessAffinityUpdateMode(
    __in HANDLE hProcess,
    __out_opt LPDWORD lpdwFlags
    );

#endif // _WIN32_WINNT >= 0x0600

WINBASEAPI
HANDLE
WINAPI
OpenProcess(
    __in DWORD dwDesiredAccess,
    __in BOOL bInheritHandle,
    __in DWORD dwProcessId
    );

WINBASEAPI
__out
HANDLE
WINAPI
GetCurrentProcess(
    VOID
    );

WINBASEAPI
DWORD
WINAPI
GetCurrentProcessId(
    VOID
    );

WINBASEAPI
DECLSPEC_NORETURN
VOID
WINAPI
ExitProcess(
    __in UINT uExitCode
    );

WINBASEAPI
BOOL
WINAPI
TerminateProcess(
    __in HANDLE hProcess,
    __in UINT uExitCode
    );

WINBASEAPI
BOOL
WINAPI
GetExitCodeProcess(
    __in  HANDLE hProcess,
    __out LPDWORD lpExitCode
    );

WINBASEAPI
VOID
WINAPI
FatalExit(
    __in int ExitCode
    );

WINBASEAPI
__out
__nullnullterminated
LPCH
WINAPI
GetEnvironmentStrings(
    VOID
    );

WINBASEAPI
__out
__nullnullterminated
LPWCH
WINAPI
GetEnvironmentStringsW(
    VOID
    );

#ifdef UNICODE
#define GetEnvironmentStrings  GetEnvironmentStringsW
#else
#define GetEnvironmentStringsA  GetEnvironmentStrings
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
SetEnvironmentStringsA(
    __in __nullnullterminated LPCH NewEnvironment
    );
WINBASEAPI
BOOL
WINAPI
SetEnvironmentStringsW(
    __in __nullnullterminated LPWCH NewEnvironment
    );
#ifdef UNICODE
#define SetEnvironmentStrings  SetEnvironmentStringsW
#else
#define SetEnvironmentStrings  SetEnvironmentStringsA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
FreeEnvironmentStringsA(
    __in __nullnullterminated LPCH
    );
WINBASEAPI
BOOL
WINAPI
FreeEnvironmentStringsW(
    __in __nullnullterminated LPWCH
    );
#ifdef UNICODE
#define FreeEnvironmentStrings  FreeEnvironmentStringsW
#else
#define FreeEnvironmentStrings  FreeEnvironmentStringsA
#endif // !UNICODE

WINBASEAPI
VOID
WINAPI
RaiseException(
    __in DWORD dwExceptionCode,
    __in DWORD dwExceptionFlags,
    __in DWORD nNumberOfArguments,
    __in_ecount_opt(nNumberOfArguments) CONST ULONG_PTR *lpArguments
    );

WINBASEAPI
VOID
WINAPI
RaiseFailFastException(
    __in_opt PEXCEPTION_RECORD pExceptionRecord,
    __in PCONTEXT pContextRecord,
    __in DWORD dwFlags
    );

__callback
WINBASEAPI
LONG
WINAPI
UnhandledExceptionFilter(
    __in struct _EXCEPTION_POINTERS *ExceptionInfo
    );

typedef LONG (WINAPI *PTOP_LEVEL_EXCEPTION_FILTER)(
    __in struct _EXCEPTION_POINTERS *ExceptionInfo
    );
typedef PTOP_LEVEL_EXCEPTION_FILTER LPTOP_LEVEL_EXCEPTION_FILTER;

WINBASEAPI
LPTOP_LEVEL_EXCEPTION_FILTER
WINAPI
SetUnhandledExceptionFilter(
    __in_opt LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter
    );

#if(_WIN32_WINNT >= 0x0400)

//
// Fiber creation flags
//

#define FIBER_FLAG_FLOAT_SWITCH 0x1     // context switch floating point

WINBASEAPI
__out_opt
LPVOID
WINAPI
CreateFiber(
    __in     SIZE_T dwStackSize,
    __in     LPFIBER_START_ROUTINE lpStartAddress,
    __in_opt LPVOID lpParameter
    );

WINBASEAPI
__out_opt
LPVOID
WINAPI
CreateFiberEx(
    __in     SIZE_T dwStackCommitSize,
    __in     SIZE_T dwStackReserveSize,
    __in     DWORD dwFlags,
    __in     LPFIBER_START_ROUTINE lpStartAddress,
    __in_opt LPVOID lpParameter
    );

WINBASEAPI
VOID
WINAPI
DeleteFiber(
    __in LPVOID lpFiber
    );

WINBASEAPI
__out_opt
LPVOID
WINAPI
ConvertThreadToFiber(
    __in_opt LPVOID lpParameter
    );

WINBASEAPI
__out_opt
LPVOID
WINAPI
ConvertThreadToFiberEx(
    __in_opt LPVOID lpParameter,
    __in     DWORD dwFlags
    );

#if (_WIN32_WINNT >= 0x0501)

WINBASEAPI
BOOL
WINAPI
ConvertFiberToThread(
    VOID
    );

#endif

#if (_WIN32_WINNT >= 0x0600)

WINBASEAPI
BOOL
WINAPI
IsThreadAFiber(
    VOID
    );

#endif

WINBASEAPI
VOID
WINAPI
SwitchToFiber(
    __in LPVOID lpFiber
    );

WINBASEAPI
BOOL
WINAPI
SwitchToThread(
    VOID
    );

//
// UMS begin
//

#if (_WIN32_WINNT >= 0x0601) && !defined(MIDL_PASS)

#define UMS_VERSION RTL_UMS_VERSION

typedef void *PUMS_CONTEXT;

typedef void *PUMS_COMPLETION_LIST;

typedef enum _RTL_UMS_THREAD_INFO_CLASS UMS_THREAD_INFO_CLASS, *PUMS_THREAD_INFO_CLASS;

typedef enum _RTL_UMS_SCHEDULER_REASON UMS_SCHEDULER_REASON;

typedef PRTL_UMS_SCHEDULER_ENTRY_POINT PUMS_SCHEDULER_ENTRY_POINT;

typedef struct _UMS_SCHEDULER_STARTUP_INFO {

    //
    // UMS Version the application was built to. Should be set to UMS_VERSION
    //
    ULONG UmsVersion;

    //
    // Completion List to associate the new User Scheduler to.
    //
    PUMS_COMPLETION_LIST CompletionList;

    //
    // A pointer to the application-defined function that represents the starting
    // address of the Sheduler.
    //
    PUMS_SCHEDULER_ENTRY_POINT SchedulerProc;

    //
    // pointer to a variable to be passed to the scheduler uppon first activation.
    //
    PVOID SchedulerParam;

} UMS_SCHEDULER_STARTUP_INFO, *PUMS_SCHEDULER_STARTUP_INFO;

__checkReturn
WINBASEAPI
BOOL
WINAPI
CreateUmsCompletionList(
    __deref_out PUMS_COMPLETION_LIST* UmsCompletionList
    );

WINBASEAPI
BOOL
WINAPI
DequeueUmsCompletionListItems(
    __in PUMS_COMPLETION_LIST UmsCompletionList,
    __in DWORD WaitTimeOut,
    __out PUMS_CONTEXT* UmsThreadList
    );

WINBASEAPI
BOOL
WINAPI
GetUmsCompletionListEvent(
    __in PUMS_COMPLETION_LIST UmsCompletionList,
    __inout PHANDLE UmsCompletionEvent
    );

WINBASEAPI
BOOL
WINAPI
ExecuteUmsThread(
    __inout PUMS_CONTEXT UmsThread
    );

WINBASEAPI
BOOL
WINAPI
UmsThreadYield(
    __in PVOID SchedulerParam
    );

WINBASEAPI
BOOL
WINAPI
DeleteUmsCompletionList(
    __in PUMS_COMPLETION_LIST UmsCompletionList
    );

WINBASEAPI
PUMS_CONTEXT
WINAPI
GetCurrentUmsThread(
    VOID
    );

WINBASEAPI
PUMS_CONTEXT
WINAPI
GetNextUmsListItem(
    __inout PUMS_CONTEXT UmsContext
    );

WINBASEAPI
BOOL
WINAPI
QueryUmsThreadInformation(
    __in PUMS_CONTEXT UmsThread,
    __in UMS_THREAD_INFO_CLASS UmsThreadInfoClass,
    __out_bcount_part(UmsThreadInformationLength, *ReturnLength) PVOID UmsThreadInformation,
    __in ULONG UmsThreadInformationLength,
    __out_opt PULONG ReturnLength
    );

WINBASEAPI
BOOL
WINAPI
SetUmsThreadInformation(
    __in PUMS_CONTEXT UmsThread,
    __in UMS_THREAD_INFO_CLASS UmsThreadInfoClass,
    __in PVOID UmsThreadInformation,
    __in ULONG UmsThreadInformationLength
    );

WINBASEAPI
BOOL
WINAPI
DeleteUmsThreadContext(
    __in PUMS_CONTEXT UmsThread
    );

WINBASEAPI
BOOL
WINAPI
CreateUmsThreadContext(
    __deref_out PUMS_CONTEXT *lpUmsThread
    );

WINBASEAPI
BOOL
WINAPI
EnterUmsSchedulingMode(
    __in PUMS_SCHEDULER_STARTUP_INFO SchedulerStartupInfo
    );

#endif // (_WIN32_WINNT >= 0x0601) && !defined(MIDL_PASS)

//
// UMS end
//

#endif /* _WIN32_WINNT >= 0x0400 */

typedef struct _PROC_THREAD_ATTRIBUTE_LIST *PPROC_THREAD_ATTRIBUTE_LIST, *LPPROC_THREAD_ATTRIBUTE_LIST;

WINBASEAPI
__out_opt
HANDLE
WINAPI
CreateThread(
    __in_opt  LPSECURITY_ATTRIBUTES lpThreadAttributes,
    __in      SIZE_T dwStackSize,
    __in      LPTHREAD_START_ROUTINE lpStartAddress,
    __in_opt __deref __drv_aliasesMem LPVOID lpParameter,
    __in      DWORD dwCreationFlags,
    __out_opt LPDWORD lpThreadId
    );

WINBASEAPI
__out_opt
HANDLE
WINAPI
CreateRemoteThread(
    __in      HANDLE hProcess,
    __in_opt  LPSECURITY_ATTRIBUTES lpThreadAttributes,
    __in      SIZE_T dwStackSize,
    __in      LPTHREAD_START_ROUTINE lpStartAddress,
    __in_opt  LPVOID lpParameter,
    __in      DWORD dwCreationFlags,
    __out_opt LPDWORD lpThreadId
    );

WINBASEAPI
__out_opt
HANDLE
WINAPI
CreateRemoteThreadEx(
    __in      HANDLE hProcess,
    __in_opt  LPSECURITY_ATTRIBUTES lpThreadAttributes,
    __in      SIZE_T dwStackSize,
    __in      LPTHREAD_START_ROUTINE lpStartAddress,
    __in_opt  LPVOID lpParameter,
    __in      DWORD dwCreationFlags,
    __in_opt  LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList,
    __out_opt LPDWORD lpThreadId
    );

WINBASEAPI
__out
HANDLE
WINAPI
GetCurrentThread(
    VOID
    );

WINBASEAPI
DWORD
WINAPI
GetCurrentThreadId(
    VOID
    );

WINBASEAPI
BOOL
WINAPI
SetThreadStackGuarantee (
    __inout PULONG StackSizeInBytes
    );

WINBASEAPI
DWORD
WINAPI
GetProcessIdOfThread(
    __in HANDLE Thread
    );

#if (_WIN32_WINNT >= 0x0502)

WINBASEAPI
DWORD
WINAPI
GetThreadId(
    __in HANDLE Thread
    );

#endif // _WIN32_WINNT >= 0x0502

WINBASEAPI
DWORD
WINAPI
GetProcessId(
    __in HANDLE Process
    );

WINBASEAPI
DWORD
WINAPI
GetCurrentProcessorNumber(
    VOID
    );

#if (_WIN32_WINNT >= 0x0601)
WINBASEAPI
VOID
WINAPI
GetCurrentProcessorNumberEx(
    __out PPROCESSOR_NUMBER ProcNumber
    );
#endif

#if (_WIN32_WINNT >= 0x0601)
WINBASEAPI
BOOL
WINAPI
GetThreadGroupAffinity(
    __in HANDLE hThread,
    __out PGROUP_AFFINITY GroupAffinity
    );
#endif // _WIN32_WINNT >= 0x0601

WINBASEAPI
DWORD_PTR
WINAPI
SetThreadAffinityMask(
    __in HANDLE hThread,
    __in DWORD_PTR dwThreadAffinityMask
    );

#if (_WIN32_WINNT >= 0x0601)
WINBASEAPI
BOOL
WINAPI
SetThreadGroupAffinity(
    __in HANDLE hThread,
    __in CONST GROUP_AFFINITY *GroupAffinity,
    __out_opt PGROUP_AFFINITY PreviousGroupAffinity
    );

WINBASEAPI
BOOL
WINAPI
SetThreadIdealProcessorEx (
    __in HANDLE hThread,
    __in PPROCESSOR_NUMBER lpIdealProcessor,
    __out_opt PPROCESSOR_NUMBER lpPreviousIdealProcessor
    );

WINBASEAPI
BOOL
WINAPI
GetThreadIdealProcessorEx (
    __in HANDLE hThread,
    __out PPROCESSOR_NUMBER lpIdealProcessor
    );

#endif // _WIN32_WINNT >= 0x0601

#if(_WIN32_WINNT >= 0x0400)
WINBASEAPI
DWORD
WINAPI
SetThreadIdealProcessor(
    __in HANDLE hThread,
    __in DWORD dwIdealProcessor
    );
#endif /* _WIN32_WINNT >= 0x0400 */

#if (_WIN32_WINNT >= 0x0600)

#define PROCESS_DEP_ENABLE                          0x00000001
#define PROCESS_DEP_DISABLE_ATL_THUNK_EMULATION     0x00000002

WINBASEAPI
BOOL
WINAPI
SetProcessDEPPolicy(
    __in DWORD dwFlags
    );

WINBASEAPI
BOOL
WINAPI
GetProcessDEPPolicy(
    __in HANDLE hProcess,
    __out LPDWORD lpFlags,
    __out PBOOL lpPermanent
    );

#endif // _WIN32_WINNT >= 0x0600

WINBASEAPI
BOOL
WINAPI
SetProcessPriorityBoost(
    __in HANDLE hProcess,
    __in BOOL bDisablePriorityBoost
    );

WINBASEAPI
BOOL
WINAPI
GetProcessPriorityBoost(
    __in  HANDLE hProcess,
    __out PBOOL  pDisablePriorityBoost
    );

WINBASEAPI
BOOL
WINAPI
RequestWakeupLatency(
    __in LATENCY_TIME latency
    );

WINBASEAPI
BOOL
WINAPI
IsSystemResumeAutomatic(
    VOID
    );

WINBASEAPI
__out_opt
HANDLE
WINAPI
OpenThread(
    __in DWORD dwDesiredAccess,
    __in BOOL bInheritHandle,
    __in DWORD dwThreadId
    );

WINBASEAPI
BOOL
WINAPI
SetThreadPriority(
    __in HANDLE hThread,
    __in int nPriority
    );

WINBASEAPI
BOOL
WINAPI
SetThreadPriorityBoost(
    __in HANDLE hThread,
    __in BOOL bDisablePriorityBoost
    );

WINBASEAPI
BOOL
WINAPI
GetThreadPriorityBoost(
    __in  HANDLE hThread,
    __out PBOOL pDisablePriorityBoost
    );

WINBASEAPI
int
WINAPI
GetThreadPriority(
    __in HANDLE hThread
    );

WINBASEAPI
BOOL
WINAPI
GetThreadTimes(
    __in  HANDLE hThread,
    __out LPFILETIME lpCreationTime,
    __out LPFILETIME lpExitTime,
    __out LPFILETIME lpKernelTime,
    __out LPFILETIME lpUserTime
    );

#if _WIN32_WINNT >= 0x0501

WINBASEAPI
BOOL
WINAPI
GetThreadIOPendingFlag(
    __in  HANDLE hThread,
    __out PBOOL  lpIOIsPending
    );

#endif // (_WIN32_WINNT >= 0x0501)

WINBASEAPI
DECLSPEC_NORETURN
VOID
WINAPI
ExitThread(
    __in DWORD dwExitCode
    );

WINBASEAPI
BOOL
WINAPI
TerminateThread(
    __in HANDLE hThread,
    __in DWORD dwExitCode
    );

WINBASEAPI
__success(return != 0)
BOOL
WINAPI
GetExitCodeThread(
    __in  HANDLE hThread,
    __out LPDWORD lpExitCode
    );

WINBASEAPI
BOOL
WINAPI
GetThreadSelectorEntry(
    __in  HANDLE hThread,
    __in  DWORD dwSelector,
    __out LPLDT_ENTRY lpSelectorEntry
    );

WINBASEAPI
EXECUTION_STATE
WINAPI
SetThreadExecutionState(
    __in EXECUTION_STATE esFlags
    );

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN7)

typedef struct _REASON_CONTEXT {
    ULONG Version;
    DWORD Flags;
    union {
        struct {
            HMODULE LocalizedReasonModule;
            ULONG LocalizedReasonId;
            ULONG ReasonStringCount;
            LPWSTR *ReasonStrings;

        } Detailed;

        LPWSTR SimpleReasonString;
    } Reason;
} REASON_CONTEXT, *PREASON_CONTEXT;

//
// Power Request APIs
//

typedef REASON_CONTEXT POWER_REQUEST_CONTEXT, *PPOWER_REQUEST_CONTEXT, *LPPOWER_REQUEST_CONTEXT;

WINBASEAPI
HANDLE
WINAPI
PowerCreateRequest (
    __in PREASON_CONTEXT Context
    );

WINBASEAPI
BOOL
WINAPI
PowerSetRequest (
    __in HANDLE PowerRequest,
    __in POWER_REQUEST_TYPE RequestType
    );

WINBASEAPI
BOOL
WINAPI
PowerClearRequest (
    __in HANDLE PowerRequest,
    __in POWER_REQUEST_TYPE RequestType
    );

#endif // (_WIN32_WINNT >= _WIN32_WINNT_WIN7)

#ifdef _M_CEE_PURE
#define GetLastError System::Runtime::InteropServices::Marshal::GetLastWin32Error
#else
WINBASEAPI
__checkReturn
DWORD
WINAPI
GetLastError(
    VOID
    );
#endif

WINBASEAPI
VOID
WINAPI
SetLastError(
    __in DWORD dwErrCode
    );

#if !defined(RC_INVOKED) // RC warns because "WINBASE_DECLARE_RESTORE_LAST_ERROR" is a bit long.
//#if _WIN32_WINNT >= 0x0501 || defined(WINBASE_DECLARE_RESTORE_LAST_ERROR)
#if defined(WINBASE_DECLARE_RESTORE_LAST_ERROR)

WINBASEAPI
VOID
WINAPI
RestoreLastError(
    __in DWORD dwErrCode
    );

typedef VOID (WINAPI* PRESTORE_LAST_ERROR)(DWORD);
#define RESTORE_LAST_ERROR_NAME_A      "RestoreLastError"
#define RESTORE_LAST_ERROR_NAME_W     L"RestoreLastError"
#define RESTORE_LAST_ERROR_NAME   TEXT("RestoreLastError")

#endif
#endif

#define HasOverlappedIoCompleted(lpOverlapped) (((DWORD)(lpOverlapped)->Internal) != STATUS_PENDING)

WINBASEAPI
BOOL
WINAPI
GetOverlappedResult(
    __in  HANDLE hFile,
    __in  LPOVERLAPPED lpOverlapped,
    __out LPDWORD lpNumberOfBytesTransferred,
    __in  BOOL bWait
    );

WINBASEAPI
__out_opt
HANDLE
WINAPI
CreateIoCompletionPort(
    __in     HANDLE FileHandle,
    __in_opt HANDLE ExistingCompletionPort,
    __in     ULONG_PTR CompletionKey,
    __in     DWORD NumberOfConcurrentThreads
    );

WINBASEAPI
BOOL
WINAPI
GetQueuedCompletionStatus(
    __in  HANDLE CompletionPort,
    __out LPDWORD lpNumberOfBytesTransferred,
    __out PULONG_PTR lpCompletionKey,
    __out LPOVERLAPPED *lpOverlapped,
    __in  DWORD dwMilliseconds
    );

#if (_WIN32_WINNT >= 0x0600)

WINBASEAPI
BOOL
WINAPI
GetQueuedCompletionStatusEx(
    __in  HANDLE CompletionPort,
    __out_ecount_part(ulCount, *ulNumEntriesRemoved) LPOVERLAPPED_ENTRY lpCompletionPortEntries,
    __in  ULONG ulCount,
    __out PULONG ulNumEntriesRemoved,
    __in  DWORD dwMilliseconds,
    __in  BOOL fAlertable
    );

#endif // _WIN32_WINNT >= 0x0600

WINBASEAPI
BOOL
WINAPI
PostQueuedCompletionStatus(
    __in     HANDLE CompletionPort,
    __in     DWORD dwNumberOfBytesTransferred,
    __in     ULONG_PTR dwCompletionKey,
    __in_opt LPOVERLAPPED lpOverlapped
    );

#if (_WIN32_WINNT >= 0x0600)

//
// The following flags allows an application to change
// the semantics of IO completion notification.
//

//
// Don't queue an entry to an associated completion port if returning success
// synchronously.
//
#define FILE_SKIP_COMPLETION_PORT_ON_SUCCESS    0x1

//
// Don't set the file handle event on IO completion.
//
#define FILE_SKIP_SET_EVENT_ON_HANDLE           0x2

WINBASEAPI
BOOL
WINAPI
SetFileCompletionNotificationModes(
    __in HANDLE FileHandle,
    __in UCHAR Flags
    );

WINBASEAPI
BOOL
WINAPI
SetFileIoOverlappedRange(
    __in HANDLE FileHandle,
    __in PUCHAR OverlappedRangeStart,
    __in ULONG Length
    );

#endif // _WIN32_WINNT >= 0x0600

#define SEM_FAILCRITICALERRORS      0x0001
#define SEM_NOGPFAULTERRORBOX       0x0002
#define SEM_NOALIGNMENTFAULTEXCEPT  0x0004
#define SEM_NOOPENFILEERRORBOX      0x8000

WINBASEAPI
UINT
WINAPI
GetErrorMode(
    VOID
    );

WINBASEAPI
UINT
WINAPI
SetErrorMode(
    __in UINT uMode
    );

//
// Thread error mode support
//

WINBASEAPI
DWORD
WINAPI
GetThreadErrorMode(
    VOID
    );

WINBASEAPI
BOOL
WINAPI
SetThreadErrorMode(
    __in DWORD dwNewMode,
    __in_opt LPDWORD lpOldMode
    );

WINBASEAPI
BOOL
WINAPI
ReadProcessMemory(
    __in      HANDLE hProcess,
    __in      LPCVOID lpBaseAddress,
    __out_bcount_part(nSize, *lpNumberOfBytesRead) LPVOID lpBuffer,
    __in      SIZE_T nSize,
    __out_opt SIZE_T * lpNumberOfBytesRead
    );

WINBASEAPI
BOOL
WINAPI
WriteProcessMemory(
    __in      HANDLE hProcess,
    __in      LPVOID lpBaseAddress,
    __in_bcount(nSize) LPCVOID lpBuffer,
    __in      SIZE_T nSize,
    __out_opt SIZE_T * lpNumberOfBytesWritten
    );

#if !defined(MIDL_PASS)
WINBASEAPI
BOOL
WINAPI
GetThreadContext(
    __in    HANDLE hThread,
    __inout LPCONTEXT lpContext
    );

WINBASEAPI
BOOL
WINAPI
SetThreadContext(
    __in HANDLE hThread,
    __in CONST CONTEXT *lpContext
    );

WINBASEAPI
BOOL
WINAPI
Wow64GetThreadContext(
    __in    HANDLE hThread,
    __inout PWOW64_CONTEXT lpContext
    );

#if(_WIN32_WINNT >= 0x0601)

WINBASEAPI
BOOL
WINAPI
Wow64GetThreadSelectorEntry(
    __in HANDLE hThread,
    __in DWORD dwSelector,
    __out PWOW64_LDT_ENTRY lpSelectorEntry
    );

#endif /* _WIN32_WINNT >= 0x0601 */

WINBASEAPI
BOOL
WINAPI
Wow64SetThreadContext(
    __in HANDLE hThread,
    __in CONST WOW64_CONTEXT *lpContext
    );

#endif

WINBASEAPI
DWORD
WINAPI
SuspendThread(
    __in HANDLE hThread
    );

WINBASEAPI
DWORD
WINAPI
Wow64SuspendThread(
    __in HANDLE hThread
    );

WINBASEAPI
DWORD
WINAPI
ResumeThread(
    __in HANDLE hThread
    );


#if(_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)

WINBASEAPI
DWORD
WINAPI
QueueUserAPC(
    __in PAPCFUNC pfnAPC,
    __in HANDLE hThread,
    __in ULONG_PTR dwData
    );

#endif /* _WIN32_WINNT >= 0x0400 || _WIN32_WINDOWS > 0x0400 */

#if (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)
WINBASEAPI
BOOL
WINAPI
IsDebuggerPresent(
    VOID
    );
#endif

#if _WIN32_WINNT >= 0x0501

WINBASEAPI
BOOL
WINAPI
CheckRemoteDebuggerPresent(
    __in  HANDLE hProcess,
    __out PBOOL pbDebuggerPresent
    );

#endif // (_WIN32_WINNT >= 0x0501)

WINBASEAPI
VOID
WINAPI
DebugBreak(
    VOID
    );

WINBASEAPI
BOOL
WINAPI
WaitForDebugEvent(
    __in LPDEBUG_EVENT lpDebugEvent,
    __in DWORD dwMilliseconds
    );

WINBASEAPI
BOOL
WINAPI
ContinueDebugEvent(
    __in DWORD dwProcessId,
    __in DWORD dwThreadId,
    __in DWORD dwContinueStatus
    );

WINBASEAPI
BOOL
WINAPI
DebugActiveProcess(
    __in DWORD dwProcessId
    );

WINBASEAPI
BOOL
WINAPI
DebugActiveProcessStop(
    __in DWORD dwProcessId
    );

WINBASEAPI
BOOL
WINAPI
DebugSetProcessKillOnExit(
    __in BOOL KillOnExit
    );

WINBASEAPI
BOOL
WINAPI
DebugBreakProcess (
    __in HANDLE Process
    );

#if (_WIN32_WINNT < _WIN32_WINNT_LONGHORN)
__drv_inTry
#endif
WINBASEAPI
VOID
WINAPI
InitializeCriticalSection(
    __out LPCRITICAL_SECTION lpCriticalSection
    );

WINBASEAPI
VOID
WINAPI
EnterCriticalSection(
    __inout LPCRITICAL_SECTION lpCriticalSection
    );

WINBASEAPI
VOID
WINAPI
LeaveCriticalSection(
    __inout LPCRITICAL_SECTION lpCriticalSection
    );

#if (_WIN32_WINNT >= 0x0403)
#define CRITICAL_SECTION_NO_DEBUG_INFO  RTL_CRITICAL_SECTION_FLAG_NO_DEBUG_INFO

WINBASEAPI
BOOL
WINAPI
InitializeCriticalSectionAndSpinCount(
    __out LPCRITICAL_SECTION lpCriticalSection,
    __in  DWORD dwSpinCount
    );

WINBASEAPI
BOOL
WINAPI
InitializeCriticalSectionEx(
    __out LPCRITICAL_SECTION lpCriticalSection,
    __in  DWORD dwSpinCount,
    __in  DWORD Flags
    );

WINBASEAPI
DWORD
WINAPI
SetCriticalSectionSpinCount(
    __inout LPCRITICAL_SECTION lpCriticalSection,
    __in    DWORD dwSpinCount
    );
#endif

#if(_WIN32_WINNT >= 0x0400)
WINBASEAPI
BOOL
WINAPI
TryEnterCriticalSection(
    __inout LPCRITICAL_SECTION lpCriticalSection
    );
#endif /* _WIN32_WINNT >= 0x0400 */

WINBASEAPI
VOID
WINAPI
DeleteCriticalSection(
    __inout LPCRITICAL_SECTION lpCriticalSection
    );

WINBASEAPI
BOOL
WINAPI
SetEvent(
    __in HANDLE hEvent
    );

WINBASEAPI
BOOL
WINAPI
ResetEvent(
    __in HANDLE hEvent
    );

WINBASEAPI
BOOL
WINAPI
PulseEvent(
    __in HANDLE hEvent
    );

WINBASEAPI
BOOL
WINAPI
ReleaseSemaphore(
    __in      HANDLE hSemaphore,
    __in      LONG lReleaseCount,
    __out_opt LPLONG lpPreviousCount
    );

WINBASEAPI
BOOL
WINAPI
ReleaseMutex(
    __in HANDLE hMutex
    );

WINBASEAPI
DWORD
WINAPI
WaitForSingleObject(
    __in HANDLE hHandle,
    __in DWORD dwMilliseconds
    );

WINBASEAPI
DWORD
WINAPI
WaitForMultipleObjects(
    __in DWORD nCount,
    __in_ecount(nCount) CONST HANDLE *lpHandles,
    __in BOOL bWaitAll,
    __in DWORD dwMilliseconds
    );

WINBASEAPI
VOID
WINAPI
Sleep(
    __in DWORD dwMilliseconds
    );

WINBASEAPI
__out_opt
HGLOBAL
WINAPI
LoadResource(
    __in_opt HMODULE hModule,
    __in HRSRC hResInfo
    );

WINBASEAPI
DWORD
WINAPI
SizeofResource(
    __in_opt HMODULE hModule,
    __in HRSRC hResInfo
    );


WINBASEAPI
ATOM
WINAPI
GlobalDeleteAtom(
    __in ATOM nAtom
    );

WINBASEAPI
BOOL
WINAPI
InitAtomTable(
    __in DWORD nSize
    );

WINBASEAPI
ATOM
WINAPI
DeleteAtom(
    __in ATOM nAtom
    );

WINBASEAPI
UINT
WINAPI
SetHandleCount(
    __in UINT uNumber
    );

WINBASEAPI
DWORD
WINAPI
GetLogicalDrives(
    VOID
    );

WINBASEAPI
BOOL
WINAPI
LockFile(
    __in HANDLE hFile,
    __in DWORD dwFileOffsetLow,
    __in DWORD dwFileOffsetHigh,
    __in DWORD nNumberOfBytesToLockLow,
    __in DWORD nNumberOfBytesToLockHigh
    );

WINBASEAPI
BOOL
WINAPI
UnlockFile(
    __in HANDLE hFile,
    __in DWORD dwFileOffsetLow,
    __in DWORD dwFileOffsetHigh,
    __in DWORD nNumberOfBytesToUnlockLow,
    __in DWORD nNumberOfBytesToUnlockHigh
    );

WINBASEAPI
BOOL
WINAPI
LockFileEx(
    __in       HANDLE hFile,
    __in       DWORD dwFlags,
    __reserved DWORD dwReserved,
    __in       DWORD nNumberOfBytesToLockLow,
    __in       DWORD nNumberOfBytesToLockHigh,
    __inout    LPOVERLAPPED lpOverlapped
    );

#define LOCKFILE_FAIL_IMMEDIATELY   0x00000001
#define LOCKFILE_EXCLUSIVE_LOCK     0x00000002

WINBASEAPI
BOOL
WINAPI
UnlockFileEx(
    __in       HANDLE hFile,
    __reserved DWORD dwReserved,
    __in       DWORD nNumberOfBytesToUnlockLow,
    __in       DWORD nNumberOfBytesToUnlockHigh,
    __inout    LPOVERLAPPED lpOverlapped
    );

typedef struct _BY_HANDLE_FILE_INFORMATION {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD dwVolumeSerialNumber;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD nNumberOfLinks;
    DWORD nFileIndexHigh;
    DWORD nFileIndexLow;
} BY_HANDLE_FILE_INFORMATION, *PBY_HANDLE_FILE_INFORMATION, *LPBY_HANDLE_FILE_INFORMATION;

WINBASEAPI
BOOL
WINAPI
GetFileInformationByHandle(
    __in  HANDLE hFile,
    __out LPBY_HANDLE_FILE_INFORMATION lpFileInformation
    );

WINBASEAPI
DWORD
WINAPI
GetFileType(
    __in HANDLE hFile
    );

WINBASEAPI
DWORD
WINAPI
GetFileSize(
    __in      HANDLE hFile,
    __out_opt LPDWORD lpFileSizeHigh
    );

WINBASEAPI
BOOL
WINAPI
GetFileSizeEx(
    __in  HANDLE hFile,
    __out PLARGE_INTEGER lpFileSize
    );


WINBASEAPI
HANDLE
WINAPI
GetStdHandle(
    __in DWORD nStdHandle
    );

WINBASEAPI
BOOL
WINAPI
SetStdHandle(
    __in DWORD nStdHandle,
    __in HANDLE hHandle
    );

#if (_WIN32_WINNT >= 0x0600)

WINBASEAPI
BOOL
WINAPI
SetStdHandleEx(
    __in      DWORD nStdHandle,
    __in      HANDLE hHandle,
    __out_opt PHANDLE phPrevValue
    );

#endif // _WIN32_WINNT >= 0x0600

WINBASEAPI
BOOL
WINAPI
WriteFile(
    __in        HANDLE hFile,
    __in_bcount_opt(nNumberOfBytesToWrite) LPCVOID lpBuffer,
    __in        DWORD nNumberOfBytesToWrite,
    __out_opt   LPDWORD lpNumberOfBytesWritten,
    __inout_opt LPOVERLAPPED lpOverlapped
    );

WINBASEAPI
BOOL
WINAPI
ReadFile(
    __in        HANDLE hFile,
    __out_bcount_part_opt(nNumberOfBytesToRead, *lpNumberOfBytesRead) __out_data_source(FILE) LPVOID lpBuffer,
    __in        DWORD nNumberOfBytesToRead,
    __out_opt   LPDWORD lpNumberOfBytesRead,
    __inout_opt LPOVERLAPPED lpOverlapped
    );

WINBASEAPI
BOOL
WINAPI
FlushFileBuffers(
    __in HANDLE hFile
    );

WINBASEAPI
BOOL
WINAPI
DeviceIoControl(
    __in        HANDLE hDevice,
    __in        DWORD dwIoControlCode,
    __in_bcount_opt(nInBufferSize) LPVOID lpInBuffer,
    __in        DWORD nInBufferSize,
    __out_bcount_part_opt(nOutBufferSize, *lpBytesReturned) LPVOID lpOutBuffer,
    __in        DWORD nOutBufferSize,
    __out_opt   LPDWORD lpBytesReturned,
    __inout_opt LPOVERLAPPED lpOverlapped
    );

WINBASEAPI
BOOL
WINAPI
RequestDeviceWakeup(
    __in HANDLE hDevice
    );

WINBASEAPI
BOOL
WINAPI
CancelDeviceWakeupRequest(
    __in HANDLE hDevice
    );

WINBASEAPI
BOOL
WINAPI
GetDevicePowerState(
    __in  HANDLE hDevice,
    __out BOOL *pfOn
    );

WINBASEAPI
BOOL
WINAPI
SetMessageWaitingIndicator(
    __in HANDLE hMsgIndicator,
    __in ULONG ulMsgCount
    );

WINBASEAPI
BOOL
WINAPI
SetEndOfFile(
    __in HANDLE hFile
    );

WINBASEAPI
DWORD
WINAPI
SetFilePointer(
    __in        HANDLE hFile,
    __in        LONG lDistanceToMove,
    __inout_opt PLONG lpDistanceToMoveHigh,
    __in        DWORD dwMoveMethod
    );

WINBASEAPI
BOOL
WINAPI
SetFilePointerEx(
    __in      HANDLE hFile,
    __in      LARGE_INTEGER liDistanceToMove,
    __out_opt PLARGE_INTEGER lpNewFilePointer,
    __in      DWORD dwMoveMethod
    );

WINBASEAPI
BOOL
WINAPI
FindClose(
    __inout HANDLE hFindFile
    );

WINBASEAPI
BOOL
WINAPI
GetFileTime(
    __in      HANDLE hFile,
    __out_opt LPFILETIME lpCreationTime,
    __out_opt LPFILETIME lpLastAccessTime,
    __out_opt LPFILETIME lpLastWriteTime
    );

WINBASEAPI
BOOL
WINAPI
SetFileTime(
    __in     HANDLE hFile,
    __in_opt CONST FILETIME *lpCreationTime,
    __in_opt CONST FILETIME *lpLastAccessTime,
    __in_opt CONST FILETIME *lpLastWriteTime
    );


#if _WIN32_WINNT >= 0x0501

WINBASEAPI
BOOL
WINAPI
SetFileValidData(
    __in HANDLE hFile,
    __in LONGLONG ValidDataLength
    );

#endif // (_WIN32_WINNT >= 0x0501)


WINBASEAPI
BOOL
WINAPI
SetFileShortNameA(
    __in HANDLE hFile,
    __in LPCSTR lpShortName
    );
WINBASEAPI
BOOL
WINAPI
SetFileShortNameW(
    __in HANDLE hFile,
    __in LPCWSTR lpShortName
    );
#ifdef UNICODE
#define SetFileShortName  SetFileShortNameW
#else
#define SetFileShortName  SetFileShortNameA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
CloseHandle(
    __in HANDLE hObject
    );

WINBASEAPI
BOOL
WINAPI
DuplicateHandle(
    __in        HANDLE hSourceProcessHandle,
    __in        HANDLE hSourceHandle,
    __in        HANDLE hTargetProcessHandle,
    __deref_out LPHANDLE lpTargetHandle,
    __in        DWORD dwDesiredAccess,
    __in        BOOL bInheritHandle,
    __in        DWORD dwOptions
    );

WINBASEAPI
BOOL
WINAPI
GetHandleInformation(
    __in  HANDLE hObject,
    __out LPDWORD lpdwFlags
    );

WINBASEAPI
BOOL
WINAPI
SetHandleInformation(
    __in HANDLE hObject,
    __in DWORD dwMask,
    __in DWORD dwFlags
    );

#define HANDLE_FLAG_INHERIT             0x00000001
#define HANDLE_FLAG_PROTECT_FROM_CLOSE  0x00000002

#define HINSTANCE_ERROR 32

WINBASEAPI
DWORD
WINAPI
LoadModule(
    __in LPCSTR lpModuleName,
    __in LPVOID lpParameterBlock
    );


__drv_preferredFunction("CreateProcess","Deprecated. See MSDN for details")
WINBASEAPI
UINT
WINAPI
WinExec(
    __in LPCSTR lpCmdLine,
    __in UINT uCmdShow
    );

WINBASEAPI
BOOL
WINAPI
ClearCommBreak(
    __in HANDLE hFile
    );

WINBASEAPI
BOOL
WINAPI
ClearCommError(
    __in      HANDLE hFile,
    __out_opt LPDWORD lpErrors,
    __out_opt LPCOMSTAT lpStat
    );

WINBASEAPI
BOOL
WINAPI
SetupComm(
    __in HANDLE hFile,
    __in DWORD dwInQueue,
    __in DWORD dwOutQueue
    );

WINBASEAPI
BOOL
WINAPI
EscapeCommFunction(
    __in HANDLE hFile,
    __in DWORD dwFunc
    );

WINBASEAPI
__success(return == TRUE)
BOOL
WINAPI
GetCommConfig(
    __in      HANDLE hCommDev,
    __out_bcount_opt(*lpdwSize) LPCOMMCONFIG lpCC,
    __inout   LPDWORD lpdwSize
    );

WINBASEAPI
BOOL
WINAPI
GetCommMask(
    __in  HANDLE hFile,
    __out LPDWORD lpEvtMask
    );

WINBASEAPI
BOOL
WINAPI
GetCommProperties(
    __in    HANDLE hFile,
    __inout LPCOMMPROP lpCommProp
    );

WINBASEAPI
BOOL
WINAPI
GetCommModemStatus(
    __in  HANDLE hFile,
    __out LPDWORD lpModemStat
    );

WINBASEAPI
BOOL
WINAPI
GetCommState(
    __in  HANDLE hFile,
    __out LPDCB lpDCB
    );

WINBASEAPI
BOOL
WINAPI
GetCommTimeouts(
    __in  HANDLE hFile,
    __out LPCOMMTIMEOUTS lpCommTimeouts
    );

WINBASEAPI
BOOL
WINAPI
PurgeComm(
    __in HANDLE hFile,
    __in DWORD dwFlags
    );

WINBASEAPI
BOOL
WINAPI
SetCommBreak(
    __in HANDLE hFile
    );

WINBASEAPI
BOOL
WINAPI
SetCommConfig(
    __in HANDLE hCommDev,
    __in_bcount(dwSize) LPCOMMCONFIG lpCC,
    __in DWORD dwSize
    );

WINBASEAPI
BOOL
WINAPI
SetCommMask(
    __in HANDLE hFile,
    __in DWORD dwEvtMask
    );

WINBASEAPI
BOOL
WINAPI
SetCommState(
    __in HANDLE hFile,
    __in LPDCB lpDCB
    );

WINBASEAPI
BOOL
WINAPI
SetCommTimeouts(
    __in HANDLE hFile,
    __in LPCOMMTIMEOUTS lpCommTimeouts
    );

WINBASEAPI
BOOL
WINAPI
TransmitCommChar(
    __in HANDLE hFile,
    __in char cChar
    );

WINBASEAPI
BOOL
WINAPI
WaitCommEvent(
    __in        HANDLE hFile,
    __inout     LPDWORD lpEvtMask,
    __inout_opt LPOVERLAPPED lpOverlapped
    );


WINBASEAPI
DWORD
WINAPI
SetTapePosition(
    __in HANDLE hDevice,
    __in DWORD dwPositionMethod,
    __in DWORD dwPartition,
    __in DWORD dwOffsetLow,
    __in DWORD dwOffsetHigh,
    __in BOOL bImmediate
    );

WINBASEAPI
DWORD
WINAPI
GetTapePosition(
    __in  HANDLE hDevice,
    __in  DWORD dwPositionType,
    __out LPDWORD lpdwPartition,
    __out LPDWORD lpdwOffsetLow,
    __out LPDWORD lpdwOffsetHigh
    );

WINBASEAPI
DWORD
WINAPI
PrepareTape(
    __in HANDLE hDevice,
    __in DWORD dwOperation,
    __in BOOL bImmediate
    );

WINBASEAPI
DWORD
WINAPI
EraseTape(
    __in HANDLE hDevice,
    __in DWORD dwEraseType,
    __in BOOL bImmediate
    );

WINBASEAPI
DWORD
WINAPI
CreateTapePartition(
    __in HANDLE hDevice,
    __in DWORD dwPartitionMethod,
    __in DWORD dwCount,
    __in DWORD dwSize
    );

WINBASEAPI
DWORD
WINAPI
WriteTapemark(
    __in HANDLE hDevice,
    __in DWORD dwTapemarkType,
    __in DWORD dwTapemarkCount,
    __in BOOL bImmediate
    );

WINBASEAPI
DWORD
WINAPI
GetTapeStatus(
    __in HANDLE hDevice
    );

WINBASEAPI
DWORD
WINAPI
GetTapeParameters(
    __in    HANDLE hDevice,
    __in    DWORD dwOperation,
    __inout LPDWORD lpdwSize,
    __out_bcount(*lpdwSize) LPVOID lpTapeInformation
    );

#define GET_TAPE_MEDIA_INFORMATION 0
#define GET_TAPE_DRIVE_INFORMATION 1

WINBASEAPI
DWORD
WINAPI
SetTapeParameters(
    __in HANDLE hDevice,
    __in DWORD dwOperation,
    __in LPVOID lpTapeInformation
    );

#define SET_TAPE_MEDIA_INFORMATION 0
#define SET_TAPE_DRIVE_INFORMATION 1

WINBASEAPI
BOOL
WINAPI
Beep(
    __in DWORD dwFreq,
    __in DWORD dwDuration
    );

WINBASEAPI
int
WINAPI
MulDiv(
    __in int nNumber,
    __in int nNumerator,
    __in int nDenominator
    );

WINBASEAPI
VOID
WINAPI
GetSystemTime(
    __out LPSYSTEMTIME lpSystemTime
    );

WINBASEAPI
VOID
WINAPI
GetSystemTimeAsFileTime(
    __out LPFILETIME lpSystemTimeAsFileTime
    );

WINBASEAPI
BOOL
WINAPI
SetSystemTime(
    __in CONST SYSTEMTIME *lpSystemTime
    );

WINBASEAPI
VOID
WINAPI
GetLocalTime(
    __out LPSYSTEMTIME lpSystemTime
    );

WINBASEAPI
BOOL
WINAPI
SetLocalTime(
    __in CONST SYSTEMTIME *lpSystemTime
    );

WINBASEAPI
VOID
WINAPI
GetSystemInfo(
    __out LPSYSTEM_INFO lpSystemInfo
    );

typedef enum _DEP_SYSTEM_POLICY_TYPE {
    DEPPolicyAlwaysOff = 0,
    DEPPolicyAlwaysOn,
    DEPPolicyOptIn,
    DEPPolicyOptOut,
    DEPTotalPolicyCount
} DEP_SYSTEM_POLICY_TYPE;

WINBASEAPI
DEP_SYSTEM_POLICY_TYPE
WINAPI
GetSystemDEPPolicy(
    VOID
    );

#if _WIN32_WINNT >= 0x0502

#define FILE_CACHE_FLAGS_DEFINED

#define FILE_CACHE_MAX_HARD_ENABLE      0x00000001
#define FILE_CACHE_MAX_HARD_DISABLE     0x00000002
#define FILE_CACHE_MIN_HARD_ENABLE      0x00000004
#define FILE_CACHE_MIN_HARD_DISABLE     0x00000008

WINBASEAPI
BOOL
WINAPI
SetSystemFileCacheSize (
    __in SIZE_T MinimumFileCacheSize,
    __in SIZE_T MaximumFileCacheSize,
    __in DWORD Flags
    );

WINBASEAPI
BOOL
WINAPI
GetSystemFileCacheSize (
    __out PSIZE_T lpMinimumFileCacheSize,
    __out PSIZE_T lpMaximumFileCacheSize,
    __out PDWORD lpFlags
    );

#endif // (_WIN32_WINNT >= 0x0502)

#if _WIN32_WINNT >= 0x0501

WINBASEAPI
BOOL
WINAPI
GetSystemRegistryQuota(
    __out_opt PDWORD pdwQuotaAllowed,
    __out_opt PDWORD pdwQuotaUsed
    );

BOOL
WINAPI
GetSystemTimes(
    __out_opt LPFILETIME lpIdleTime,
    __out_opt LPFILETIME lpKernelTime,
    __out_opt LPFILETIME lpUserTime
    );

#endif // (_WIN32_WINNT >= 0x0501)

#if _WIN32_WINNT >= 0x0501
WINBASEAPI
VOID
WINAPI
GetNativeSystemInfo(
    __out LPSYSTEM_INFO lpSystemInfo
    );
#endif

WINBASEAPI
BOOL
WINAPI
IsProcessorFeaturePresent(
    __in DWORD ProcessorFeature
    );

typedef struct _TIME_ZONE_INFORMATION {
    LONG Bias;
    WCHAR StandardName[ 32 ];
    SYSTEMTIME StandardDate;
    LONG StandardBias;
    WCHAR DaylightName[ 32 ];
    SYSTEMTIME DaylightDate;
    LONG DaylightBias;
} TIME_ZONE_INFORMATION, *PTIME_ZONE_INFORMATION, *LPTIME_ZONE_INFORMATION;

typedef struct _TIME_DYNAMIC_ZONE_INFORMATION {
    LONG Bias;
    WCHAR StandardName[ 32 ];
    SYSTEMTIME StandardDate;
    LONG StandardBias;
    WCHAR DaylightName[ 32 ];
    SYSTEMTIME DaylightDate;
    LONG DaylightBias;
    WCHAR TimeZoneKeyName[ 128 ];
    BOOLEAN DynamicDaylightTimeDisabled;
} DYNAMIC_TIME_ZONE_INFORMATION, *PDYNAMIC_TIME_ZONE_INFORMATION;


WINBASEAPI
BOOL
WINAPI
SystemTimeToTzSpecificLocalTime(
    __in_opt CONST TIME_ZONE_INFORMATION *lpTimeZoneInformation,
    __in     CONST SYSTEMTIME *lpUniversalTime,
    __out    LPSYSTEMTIME lpLocalTime
    );

WINBASEAPI
BOOL
WINAPI
TzSpecificLocalTimeToSystemTime(
    __in_opt CONST TIME_ZONE_INFORMATION *lpTimeZoneInformation,
    __in     CONST SYSTEMTIME *lpLocalTime,
    __out    LPSYSTEMTIME lpUniversalTime
    );

BOOL
WINAPI
GetTimeZoneInformationForYear(
    __in USHORT wYear,
    __in_opt PDYNAMIC_TIME_ZONE_INFORMATION pdtzi,
    __out LPTIME_ZONE_INFORMATION ptzi
    );

WINBASEAPI
DWORD
WINAPI
GetTimeZoneInformation(
    __out LPTIME_ZONE_INFORMATION lpTimeZoneInformation
    );

WINBASEAPI
BOOL
WINAPI
SetTimeZoneInformation(
    __in CONST TIME_ZONE_INFORMATION *lpTimeZoneInformation
    );

WINBASEAPI
DWORD
WINAPI
GetDynamicTimeZoneInformation(
    __out PDYNAMIC_TIME_ZONE_INFORMATION pTimeZoneInformation
    );

WINBASEAPI
BOOL
WINAPI
SetDynamicTimeZoneInformation(
    __in CONST DYNAMIC_TIME_ZONE_INFORMATION *lpTimeZoneInformation
    );



//
// Routines to convert back and forth between system time and file time
//

WINBASEAPI
BOOL
WINAPI
SystemTimeToFileTime(
    __in  CONST SYSTEMTIME *lpSystemTime,
    __out LPFILETIME lpFileTime
    );

WINBASEAPI
BOOL
WINAPI
FileTimeToLocalFileTime(
    __in  CONST FILETIME *lpFileTime,
    __out LPFILETIME lpLocalFileTime
    );

WINBASEAPI
BOOL
WINAPI
LocalFileTimeToFileTime(
    __in  CONST FILETIME *lpLocalFileTime,
    __out LPFILETIME lpFileTime
    );

WINBASEAPI
BOOL
WINAPI
FileTimeToSystemTime(
    __in  CONST FILETIME *lpFileTime,
    __out LPSYSTEMTIME lpSystemTime
    );

WINBASEAPI
LONG
WINAPI
CompareFileTime(
    __in CONST FILETIME *lpFileTime1,
    __in CONST FILETIME *lpFileTime2
    );

WINBASEAPI
BOOL
WINAPI
FileTimeToDosDateTime(
    __in  CONST FILETIME *lpFileTime,
    __out LPWORD lpFatDate,
    __out LPWORD lpFatTime
    );

WINBASEAPI
BOOL
WINAPI
DosDateTimeToFileTime(
    __in  WORD wFatDate,
    __in  WORD wFatTime,
    __out LPFILETIME lpFileTime
    );

__drv_preferredFunction("GetTickCount64", "GetTickCount overflows roughly every 49 days.  Code that does not take that into account can loop indefinitely.  GetTickCount64 operates on 64 bit values and does not have that problem")
WINBASEAPI
DWORD
WINAPI
GetTickCount(
    VOID
    );

#if (_WIN32_WINNT >= 0x0600)

WINBASEAPI
ULONGLONG
WINAPI
GetTickCount64(
    VOID
    );

#endif

WINBASEAPI
BOOL
WINAPI
SetSystemTimeAdjustment(
    __in DWORD dwTimeAdjustment,
    __in BOOL  bTimeAdjustmentDisabled
    );

WINBASEAPI
BOOL
WINAPI
GetSystemTimeAdjustment(
    __out PDWORD lpTimeAdjustment,
    __out PDWORD lpTimeIncrement,
    __out PBOOL  lpTimeAdjustmentDisabled
    );

#if !defined(MIDL_PASS)
WINBASEAPI
DWORD
WINAPI
FormatMessageA(
    __in     DWORD dwFlags,
    __in_opt LPCVOID lpSource,
    __in     DWORD dwMessageId,
    __in     DWORD dwLanguageId,
    __out    LPSTR lpBuffer,
    __in     DWORD nSize,
    __in_opt va_list *Arguments
    );
WINBASEAPI
DWORD
WINAPI
FormatMessageW(
    __in     DWORD dwFlags,
    __in_opt LPCVOID lpSource,
    __in     DWORD dwMessageId,
    __in     DWORD dwLanguageId,
    __out    LPWSTR lpBuffer,
    __in     DWORD nSize,
    __in_opt va_list *Arguments
    );
#ifdef UNICODE
#define FormatMessage  FormatMessageW
#else
#define FormatMessage  FormatMessageA
#endif // !UNICODE

#if defined(_M_CEE)
#undef FormatMessage
__inline
DWORD
FormatMessage(
    DWORD dwFlags,
    LPCVOID lpSource,
    DWORD dwMessageId,
    DWORD dwLanguageId,
    LPTSTR lpBuffer,
    DWORD nSize,
    va_list *Arguments
    )
{
#ifdef UNICODE
    return FormatMessageW(
#else
    return FormatMessageA(
#endif
        dwFlags,
        lpSource,
        dwMessageId,
        dwLanguageId,
        lpBuffer,
        nSize,
        Arguments
        );
}
#endif  /* _M_CEE */
#endif  /* MIDL_PASS */

#define FORMAT_MESSAGE_ALLOCATE_BUFFER 0x00000100
#define FORMAT_MESSAGE_IGNORE_INSERTS  0x00000200
#define FORMAT_MESSAGE_FROM_STRING     0x00000400
#define FORMAT_MESSAGE_FROM_HMODULE    0x00000800
#define FORMAT_MESSAGE_FROM_SYSTEM     0x00001000
#define FORMAT_MESSAGE_ARGUMENT_ARRAY  0x00002000
#define FORMAT_MESSAGE_MAX_WIDTH_MASK  0x000000FF



WINBASEAPI
BOOL
WINAPI
CreatePipe(
    __out_ecount_full(1) PHANDLE hReadPipe,
    __out_ecount_full(1) PHANDLE hWritePipe,
    __in_opt LPSECURITY_ATTRIBUTES lpPipeAttributes,
    __in     DWORD nSize
    );

WINBASEAPI
BOOL
WINAPI
ConnectNamedPipe(
    __in        HANDLE hNamedPipe,
    __inout_opt LPOVERLAPPED lpOverlapped
    );

WINBASEAPI
BOOL
WINAPI
DisconnectNamedPipe(
    __in HANDLE hNamedPipe
    );

WINBASEAPI
BOOL
WINAPI
SetNamedPipeHandleState(
    __in     HANDLE hNamedPipe,
    __in_opt LPDWORD lpMode,
    __in_opt LPDWORD lpMaxCollectionCount,
    __in_opt LPDWORD lpCollectDataTimeout
    );

WINBASEAPI
BOOL
WINAPI
GetNamedPipeInfo(
    __in      HANDLE hNamedPipe,
    __out_opt LPDWORD lpFlags,
    __out_opt LPDWORD lpOutBufferSize,
    __out_opt LPDWORD lpInBufferSize,
    __out_opt LPDWORD lpMaxInstances
    );

WINBASEAPI
BOOL
WINAPI
PeekNamedPipe(
    __in      HANDLE hNamedPipe,
    __out_bcount_part_opt(nBufferSize, *lpBytesRead) LPVOID lpBuffer,
    __in      DWORD nBufferSize,
    __out_opt LPDWORD lpBytesRead,
    __out_opt LPDWORD lpTotalBytesAvail,
    __out_opt LPDWORD lpBytesLeftThisMessage
    );

WINBASEAPI
BOOL
WINAPI
TransactNamedPipe(
    __in        HANDLE hNamedPipe,
    __in_bcount_opt(nInBufferSize) LPVOID lpInBuffer,
    __in        DWORD nInBufferSize,
    __out_bcount_part_opt(nOutBufferSize, *lpBytesRead) LPVOID lpOutBuffer,
    __in        DWORD nOutBufferSize,
    __out       LPDWORD lpBytesRead,
    __inout_opt LPOVERLAPPED lpOverlapped
    );

WINBASEAPI
__out
HANDLE
WINAPI
CreateMailslotA(
    __in     LPCSTR lpName,
    __in     DWORD nMaxMessageSize,
    __in     DWORD lReadTimeout,
    __in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes
    );
WINBASEAPI
__out
HANDLE
WINAPI
CreateMailslotW(
    __in     LPCWSTR lpName,
    __in     DWORD nMaxMessageSize,
    __in     DWORD lReadTimeout,
    __in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes
    );
#ifdef UNICODE
#define CreateMailslot  CreateMailslotW
#else
#define CreateMailslot  CreateMailslotA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
GetMailslotInfo(
    __in      HANDLE hMailslot,
    __out_opt LPDWORD lpMaxMessageSize,
    __out_opt LPDWORD lpNextSize,
    __out_opt LPDWORD lpMessageCount,
    __out_opt LPDWORD lpReadTimeout
    );

WINBASEAPI
BOOL
WINAPI
SetMailslotInfo(
    __in HANDLE hMailslot,
    __in DWORD lReadTimeout
    );

WINBASEAPI
__out_opt __out_data_source(FILE)
LPVOID
WINAPI
MapViewOfFile(
    __in HANDLE hFileMappingObject,
    __in DWORD dwDesiredAccess,
    __in DWORD dwFileOffsetHigh,
    __in DWORD dwFileOffsetLow,
    __in SIZE_T dwNumberOfBytesToMap
    );

WINBASEAPI
BOOL
WINAPI
FlushViewOfFile(
    __in LPCVOID lpBaseAddress,
    __in SIZE_T dwNumberOfBytesToFlush
    );

WINBASEAPI
BOOL
WINAPI
UnmapViewOfFile(
    __in LPCVOID lpBaseAddress
    );

//
// File Encryption API
//

WINADVAPI
BOOL
WINAPI
EncryptFileA(
    __in LPCSTR lpFileName
    );
WINADVAPI
BOOL
WINAPI
EncryptFileW(
    __in LPCWSTR lpFileName
    );
#ifdef UNICODE
#define EncryptFile  EncryptFileW
#else
#define EncryptFile  EncryptFileA
#endif // !UNICODE

WINADVAPI
BOOL
WINAPI
DecryptFileA(
    __in       LPCSTR lpFileName,
    __reserved DWORD dwReserved
    );
WINADVAPI
BOOL
WINAPI
DecryptFileW(
    __in       LPCWSTR lpFileName,
    __reserved DWORD dwReserved
    );
#ifdef UNICODE
#define DecryptFile  DecryptFileW
#else
#define DecryptFile  DecryptFileA
#endif // !UNICODE

//
//  Encryption Status Value
//

#define FILE_ENCRYPTABLE                0
#define FILE_IS_ENCRYPTED               1
#define FILE_SYSTEM_ATTR                2
#define FILE_ROOT_DIR                   3
#define FILE_SYSTEM_DIR                 4
#define FILE_UNKNOWN                    5
#define FILE_SYSTEM_NOT_SUPPORT         6
#define FILE_USER_DISALLOWED            7
#define FILE_READ_ONLY                  8
#define FILE_DIR_DISALLOWED             9

WINADVAPI
BOOL
WINAPI
FileEncryptionStatusA(
    __in  LPCSTR lpFileName,
    __out LPDWORD  lpStatus
    );
WINADVAPI
BOOL
WINAPI
FileEncryptionStatusW(
    __in  LPCWSTR lpFileName,
    __out LPDWORD  lpStatus
    );
#ifdef UNICODE
#define FileEncryptionStatus  FileEncryptionStatusW
#else
#define FileEncryptionStatus  FileEncryptionStatusA
#endif // !UNICODE

//
// Currently defined recovery flags
//

#define EFS_USE_RECOVERY_KEYS  (0x1)

typedef
DWORD
(WINAPI *PFE_EXPORT_FUNC)(
    __in_bcount(ulLength) PBYTE pbData,
    __in_opt PVOID pvCallbackContext,
    __in     ULONG ulLength
    );

typedef
DWORD
(WINAPI *PFE_IMPORT_FUNC)(
    __out_bcount_part(*ulLength, *ulLength) PBYTE pbData,
    __in_opt PVOID pvCallbackContext,
    __inout  PULONG ulLength
    );


//
//  OpenRaw flag values
//

#define CREATE_FOR_IMPORT  (1)
#define CREATE_FOR_DIR     (2)
#define OVERWRITE_HIDDEN   (4)
#define EFSRPC_SECURE_ONLY (8)


WINADVAPI
DWORD
WINAPI
OpenEncryptedFileRawA(
    __in        LPCSTR lpFileName,
    __in        ULONG    ulFlags,
    __deref_out PVOID   *pvContext
    );
WINADVAPI
DWORD
WINAPI
OpenEncryptedFileRawW(
    __in        LPCWSTR lpFileName,
    __in        ULONG    ulFlags,
    __deref_out PVOID   *pvContext
    );
#ifdef UNICODE
#define OpenEncryptedFileRaw  OpenEncryptedFileRawW
#else
#define OpenEncryptedFileRaw  OpenEncryptedFileRawA
#endif // !UNICODE

WINADVAPI
DWORD
WINAPI
ReadEncryptedFileRaw(
    __in     PFE_EXPORT_FUNC pfExportCallback,
    __in_opt PVOID           pvCallbackContext,
    __in     PVOID           pvContext
    );

WINADVAPI
DWORD
WINAPI
WriteEncryptedFileRaw(
    __in     PFE_IMPORT_FUNC pfImportCallback,
    __in_opt PVOID           pvCallbackContext,
    __in     PVOID           pvContext
    );

WINADVAPI
VOID
WINAPI
CloseEncryptedFileRaw(
    __in PVOID           pvContext
    );

//
// _l Compat Functions
//

WINBASEAPI
int
WINAPI
lstrcmpA(
    __in LPCSTR lpString1,
    __in LPCSTR lpString2
    );
WINBASEAPI
int
WINAPI
lstrcmpW(
    __in LPCWSTR lpString1,
    __in LPCWSTR lpString2
    );
#ifdef UNICODE
#define lstrcmp  lstrcmpW
#else
#define lstrcmp  lstrcmpA
#endif // !UNICODE

WINBASEAPI
int
WINAPI
lstrcmpiA(
    __in LPCSTR lpString1,
    __in LPCSTR lpString2
    );
WINBASEAPI
int
WINAPI
lstrcmpiW(
    __in LPCWSTR lpString1,
    __in LPCWSTR lpString2
    );
#ifdef UNICODE
#define lstrcmpi  lstrcmpiW
#else
#define lstrcmpi  lstrcmpiA
#endif // !UNICODE

WINBASEAPI
__out
LPSTR
WINAPI
lstrcpynA(
    __out_ecount(iMaxLength) LPSTR lpString1,
    __in LPCSTR lpString2,
    __in int iMaxLength
    );
WINBASEAPI
__out
LPWSTR
WINAPI
lstrcpynW(
    __out_ecount(iMaxLength) LPWSTR lpString1,
    __in LPCWSTR lpString2,
    __in int iMaxLength
    );
#ifdef UNICODE
#define lstrcpyn  lstrcpynW
#else
#define lstrcpyn  lstrcpynA
#endif // !UNICODE

#if defined(DEPRECATE_SUPPORTED)
#pragma warning(push)
#pragma warning(disable:4995)
#endif

WINBASEAPI
__out
LPSTR
WINAPI
lstrcpyA(
    __out_z LPSTR lpString1, // deprecated: annotation is as good as it gets
    __in  LPCSTR lpString2
    );
WINBASEAPI
__out
LPWSTR
WINAPI
lstrcpyW(
    __out_z LPWSTR lpString1, // deprecated: annotation is as good as it gets
    __in  LPCWSTR lpString2
    );
#ifdef UNICODE
#define lstrcpy  lstrcpyW
#else
#define lstrcpy  lstrcpyA
#endif // !UNICODE

WINBASEAPI
__out
LPSTR
WINAPI
lstrcatA(
    __inout LPSTR lpString1,
    __in    LPCSTR lpString2
    );
WINBASEAPI
__out
LPWSTR
WINAPI
lstrcatW(
    __inout LPWSTR lpString1,
    __in    LPCWSTR lpString2
    );
#ifdef UNICODE
#define lstrcat  lstrcatW
#else
#define lstrcat  lstrcatA
#endif // !UNICODE

#if defined(DEPRECATE_SUPPORTED)
#pragma warning(pop)
#endif

WINBASEAPI
int
WINAPI
lstrlenA(
    __in LPCSTR lpString
    );
WINBASEAPI
int
WINAPI
lstrlenW(
    __in LPCWSTR lpString
    );
#ifdef UNICODE
#define lstrlen  lstrlenW
#else
#define lstrlen  lstrlenA
#endif // !UNICODE

WINBASEAPI
HFILE
WINAPI
OpenFile(
    __in    LPCSTR lpFileName,
    __inout LPOFSTRUCT lpReOpenBuff,
    __in    UINT uStyle
    );

WINBASEAPI
HFILE
WINAPI
_lopen(
    __in LPCSTR lpPathName,
    __in int iReadWrite
    );

WINBASEAPI
HFILE
WINAPI
_lcreat(
    __in LPCSTR lpPathName,
    __in int  iAttribute
    );

WINBASEAPI
UINT
WINAPI
_lread(
    __in HFILE hFile,
    __out_bcount_part(uBytes, return) LPVOID lpBuffer,
    __in UINT uBytes
    );

WINBASEAPI
UINT
WINAPI
_lwrite(
    __in HFILE hFile,
    __in_bcount(uBytes) LPCCH lpBuffer,
    __in UINT uBytes
    );

WINBASEAPI
long
WINAPI
_hread(
    __in HFILE hFile,
    __out_bcount_part(lBytes, return) LPVOID lpBuffer,
    __in long lBytes
    );

WINBASEAPI
long
WINAPI
_hwrite(
    __in HFILE hFile,
    __in_bcount(lBytes) LPCCH lpBuffer,
    __in long lBytes
    );

WINBASEAPI
HFILE
WINAPI
_lclose(
    __in HFILE hFile
    );

WINBASEAPI
LONG
WINAPI
_llseek(
    __in HFILE hFile,
    __in LONG lOffset,
    __in int iOrigin
    );

WINADVAPI
BOOL
WINAPI
IsTextUnicode(
    __in_bcount(iSize) CONST VOID* lpv,
    __in        int iSize,
    __inout_opt LPINT lpiResult
    );

#define FLS_OUT_OF_INDEXES ((DWORD)0xFFFFFFFF)

WINBASEAPI
DWORD
WINAPI
FlsAlloc(
    __in_opt PFLS_CALLBACK_FUNCTION lpCallback
    );

WINBASEAPI
PVOID
WINAPI
FlsGetValue(
    __in DWORD dwFlsIndex
    );

WINBASEAPI
BOOL
WINAPI
FlsSetValue(
    __in     DWORD dwFlsIndex,
    __in_opt PVOID lpFlsData
    );

WINBASEAPI
BOOL
WINAPI
FlsFree(
    __in DWORD dwFlsIndex
    );

#define TLS_OUT_OF_INDEXES ((DWORD)0xFFFFFFFF)

WINBASEAPI
DWORD
WINAPI
TlsAlloc(
    VOID
    );

WINBASEAPI
LPVOID
WINAPI
TlsGetValue(
    __in DWORD dwTlsIndex
    );

WINBASEAPI
BOOL
WINAPI
TlsSetValue(
    __in     DWORD dwTlsIndex,
    __in_opt LPVOID lpTlsValue
    );

WINBASEAPI
BOOL
WINAPI
TlsFree(
    __in DWORD dwTlsIndex
    );

typedef
VOID
(WINAPI *LPOVERLAPPED_COMPLETION_ROUTINE)(
    __in    DWORD dwErrorCode,
    __in    DWORD dwNumberOfBytesTransfered,
    __inout LPOVERLAPPED lpOverlapped
    );

WINBASEAPI
DWORD
WINAPI
SleepEx(
    __in DWORD dwMilliseconds,
    __in BOOL bAlertable
    );

WINBASEAPI
DWORD
WINAPI
WaitForSingleObjectEx(
    __in HANDLE hHandle,
    __in DWORD dwMilliseconds,
    __in BOOL bAlertable
    );

WINBASEAPI
DWORD
WINAPI
WaitForMultipleObjectsEx(
    __in DWORD nCount,
    __in_ecount(nCount) CONST HANDLE *lpHandles,
    __in BOOL bWaitAll,
    __in DWORD dwMilliseconds,
    __in BOOL bAlertable
    );

#if(_WIN32_WINNT >= 0x0400)
WINBASEAPI
DWORD
WINAPI
SignalObjectAndWait(
    __in HANDLE hObjectToSignal,
    __in HANDLE hObjectToWaitOn,
    __in DWORD dwMilliseconds,
    __in BOOL bAlertable
    );
#endif /* _WIN32_WINNT >= 0x0400 */

WINBASEAPI
BOOL
WINAPI
ReadFileEx(
    __in     HANDLE hFile,
    __out_bcount_opt(nNumberOfBytesToRead) __out_data_source(FILE) LPVOID lpBuffer,
    __in     DWORD nNumberOfBytesToRead,
    __inout  LPOVERLAPPED lpOverlapped,
    __in_opt LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
    );

WINBASEAPI
BOOL
WINAPI
WriteFileEx(
    __in     HANDLE hFile,
    __in_bcount_opt(nNumberOfBytesToWrite) LPCVOID lpBuffer,
    __in     DWORD nNumberOfBytesToWrite,
    __inout  LPOVERLAPPED lpOverlapped,
    __in_opt LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
    );

WINBASEAPI
BOOL
WINAPI
BackupRead(
    __in    HANDLE hFile,
    __out_bcount_part(nNumberOfBytesToRead, *lpNumberOfBytesRead) LPBYTE lpBuffer,
    __in    DWORD nNumberOfBytesToRead,
    __out   LPDWORD lpNumberOfBytesRead,
    __in    BOOL bAbort,
    __in    BOOL bProcessSecurity,
    __inout LPVOID *lpContext
    );

WINBASEAPI
BOOL
WINAPI
BackupSeek(
    __in    HANDLE hFile,
    __in    DWORD  dwLowBytesToSeek,
    __in    DWORD  dwHighBytesToSeek,
    __out   LPDWORD lpdwLowByteSeeked,
    __out   LPDWORD lpdwHighByteSeeked,
    __inout LPVOID *lpContext
    );

WINBASEAPI
BOOL
WINAPI
BackupWrite(
    __in    HANDLE hFile,
    __in_bcount(nNumberOfBytesToWrite) LPBYTE lpBuffer,
    __in    DWORD nNumberOfBytesToWrite,
    __out   LPDWORD lpNumberOfBytesWritten,
    __in    BOOL bAbort,
    __in    BOOL bProcessSecurity,
    __inout LPVOID *lpContext
    );

//
//  Stream id structure
//
typedef struct _WIN32_STREAM_ID {
        DWORD          dwStreamId ;
        DWORD          dwStreamAttributes ;
        LARGE_INTEGER  Size ;
        DWORD          dwStreamNameSize ;
        WCHAR          cStreamName[ ANYSIZE_ARRAY ] ;
} WIN32_STREAM_ID, *LPWIN32_STREAM_ID ;

//
//  Stream Ids
//

#define BACKUP_INVALID          0x00000000
#define BACKUP_DATA             0x00000001
#define BACKUP_EA_DATA          0x00000002
#define BACKUP_SECURITY_DATA    0x00000003
#define BACKUP_ALTERNATE_DATA   0x00000004
#define BACKUP_LINK             0x00000005
#define BACKUP_PROPERTY_DATA    0x00000006
#define BACKUP_OBJECT_ID        0x00000007
#define BACKUP_REPARSE_DATA     0x00000008
#define BACKUP_SPARSE_BLOCK     0x00000009
#define BACKUP_TXFS_DATA        0x0000000a


//
//  Stream Attributes
//

#define STREAM_NORMAL_ATTRIBUTE         0x00000000
#define STREAM_MODIFIED_WHEN_READ       0x00000001
#define STREAM_CONTAINS_SECURITY        0x00000002
#define STREAM_CONTAINS_PROPERTIES      0x00000004
#define STREAM_SPARSE_ATTRIBUTE         0x00000008

WINBASEAPI
BOOL
WINAPI
ReadFileScatter(
    __in       HANDLE hFile,
    __in       FILE_SEGMENT_ELEMENT aSegmentArray[],
    __in       DWORD nNumberOfBytesToRead,
    __reserved LPDWORD lpReserved,
    __inout    LPOVERLAPPED lpOverlapped
    );

WINBASEAPI
BOOL
WINAPI
WriteFileGather(
    __in       HANDLE hFile,
    __in       FILE_SEGMENT_ELEMENT aSegmentArray[],
    __in       DWORD nNumberOfBytesToWrite,
    __reserved LPDWORD lpReserved,
    __inout    LPOVERLAPPED lpOverlapped
    );

//
// Dual Mode API below this line. Dual Mode Structures also included.
//

#define STARTF_USESHOWWINDOW    0x00000001
#define STARTF_USESIZE          0x00000002
#define STARTF_USEPOSITION      0x00000004
#define STARTF_USECOUNTCHARS    0x00000008
#define STARTF_USEFILLATTRIBUTE 0x00000010
#define STARTF_RUNFULLSCREEN    0x00000020  // ignored for non-x86 platforms
#define STARTF_FORCEONFEEDBACK  0x00000040
#define STARTF_FORCEOFFFEEDBACK 0x00000080
#define STARTF_USESTDHANDLES    0x00000100

#if(WINVER >= 0x0400)

#define STARTF_USEHOTKEY        0x00000200
#define STARTF_TITLEISLINKNAME  0x00000800
#define STARTF_TITLEISAPPID     0x00001000
#define STARTF_PREVENTPINNING   0x00002000
#endif /* WINVER >= 0x0400 */

typedef struct _STARTUPINFOA {
    DWORD   cb;
    LPSTR   lpReserved;
    LPSTR   lpDesktop;
    LPSTR   lpTitle;
    DWORD   dwX;
    DWORD   dwY;
    DWORD   dwXSize;
    DWORD   dwYSize;
    DWORD   dwXCountChars;
    DWORD   dwYCountChars;
    DWORD   dwFillAttribute;
    DWORD   dwFlags;
    WORD    wShowWindow;
    WORD    cbReserved2;
    LPBYTE  lpReserved2;
    HANDLE  hStdInput;
    HANDLE  hStdOutput;
    HANDLE  hStdError;
} STARTUPINFOA, *LPSTARTUPINFOA;
typedef struct _STARTUPINFOW {
    DWORD   cb;
    LPWSTR  lpReserved;
    LPWSTR  lpDesktop;
    LPWSTR  lpTitle;
    DWORD   dwX;
    DWORD   dwY;
    DWORD   dwXSize;
    DWORD   dwYSize;
    DWORD   dwXCountChars;
    DWORD   dwYCountChars;
    DWORD   dwFillAttribute;
    DWORD   dwFlags;
    WORD    wShowWindow;
    WORD    cbReserved2;
    LPBYTE  lpReserved2;
    HANDLE  hStdInput;
    HANDLE  hStdOutput;
    HANDLE  hStdError;
} STARTUPINFOW, *LPSTARTUPINFOW;
#ifdef UNICODE
typedef STARTUPINFOW STARTUPINFO;
typedef LPSTARTUPINFOW LPSTARTUPINFO;
#else
typedef STARTUPINFOA STARTUPINFO;
typedef LPSTARTUPINFOA LPSTARTUPINFO;
#endif // UNICODE

#if (_WIN32_WINNT >= 0x0600)

typedef struct _STARTUPINFOEXA {
    STARTUPINFOA StartupInfo;
    LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList;
} STARTUPINFOEXA, *LPSTARTUPINFOEXA;
typedef struct _STARTUPINFOEXW {
    STARTUPINFOW StartupInfo;
    LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList;
} STARTUPINFOEXW, *LPSTARTUPINFOEXW;
#ifdef UNICODE
typedef STARTUPINFOEXW STARTUPINFOEX;
typedef LPSTARTUPINFOEXW LPSTARTUPINFOEX;
#else
typedef STARTUPINFOEXA STARTUPINFOEX;
typedef LPSTARTUPINFOEXA LPSTARTUPINFOEX;
#endif // UNICODE

#endif // (_WIN32_WINNT >= 0x0600)

#define SHUTDOWN_NORETRY                0x00000001

typedef struct _WIN32_FIND_DATAA {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD dwReserved0;
    DWORD dwReserved1;
    CHAR   cFileName[ MAX_PATH ];
    CHAR   cAlternateFileName[ 14 ];
#ifdef _MAC
    DWORD dwFileType;
    DWORD dwCreatorType;
    WORD  wFinderFlags;
#endif
} WIN32_FIND_DATAA, *PWIN32_FIND_DATAA, *LPWIN32_FIND_DATAA;
typedef struct _WIN32_FIND_DATAW {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD dwReserved0;
    DWORD dwReserved1;
    WCHAR  cFileName[ MAX_PATH ];
    WCHAR  cAlternateFileName[ 14 ];
#ifdef _MAC
    DWORD dwFileType;
    DWORD dwCreatorType;
    WORD  wFinderFlags;
#endif
} WIN32_FIND_DATAW, *PWIN32_FIND_DATAW, *LPWIN32_FIND_DATAW;
#ifdef UNICODE
typedef WIN32_FIND_DATAW WIN32_FIND_DATA;
typedef PWIN32_FIND_DATAW PWIN32_FIND_DATA;
typedef LPWIN32_FIND_DATAW LPWIN32_FIND_DATA;
#else
typedef WIN32_FIND_DATAA WIN32_FIND_DATA;
typedef PWIN32_FIND_DATAA PWIN32_FIND_DATA;
typedef LPWIN32_FIND_DATAA LPWIN32_FIND_DATA;
#endif // UNICODE

typedef struct _WIN32_FILE_ATTRIBUTE_DATA {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
} WIN32_FILE_ATTRIBUTE_DATA, *LPWIN32_FILE_ATTRIBUTE_DATA;

//
// Synchronization APIs
//

WINBASEAPI
__out_opt
HANDLE
WINAPI
CreateMutexA(
    __in_opt LPSECURITY_ATTRIBUTES lpMutexAttributes,
    __in     BOOL bInitialOwner,
    __in_opt LPCSTR lpName
    );
WINBASEAPI
__out_opt
HANDLE
WINAPI
CreateMutexW(
    __in_opt LPSECURITY_ATTRIBUTES lpMutexAttributes,
    __in     BOOL bInitialOwner,
    __in_opt LPCWSTR lpName
    );
#ifdef UNICODE
#define CreateMutex  CreateMutexW
#else
#define CreateMutex  CreateMutexA
#endif // !UNICODE

WINBASEAPI
__out_opt
HANDLE
WINAPI
OpenMutexA(
    __in DWORD dwDesiredAccess,
    __in BOOL bInheritHandle,
    __in LPCSTR lpName
    );
WINBASEAPI
__out_opt
HANDLE
WINAPI
OpenMutexW(
    __in DWORD dwDesiredAccess,
    __in BOOL bInheritHandle,
    __in LPCWSTR lpName
    );
#ifdef UNICODE
#define OpenMutex  OpenMutexW
#else
#define OpenMutex  OpenMutexA
#endif // !UNICODE

WINBASEAPI
__out_opt
HANDLE
WINAPI
CreateEventA(
    __in_opt LPSECURITY_ATTRIBUTES lpEventAttributes,
    __in     BOOL bManualReset,
    __in     BOOL bInitialState,
    __in_opt LPCSTR lpName
    );
WINBASEAPI
__out_opt
HANDLE
WINAPI
CreateEventW(
    __in_opt LPSECURITY_ATTRIBUTES lpEventAttributes,
    __in     BOOL bManualReset,
    __in     BOOL bInitialState,
    __in_opt LPCWSTR lpName
    );
#ifdef UNICODE
#define CreateEvent  CreateEventW
#else
#define CreateEvent  CreateEventA
#endif // !UNICODE

WINBASEAPI
__out_opt
HANDLE
WINAPI
OpenEventA(
    __in DWORD dwDesiredAccess,
    __in BOOL bInheritHandle,
    __in LPCSTR lpName
    );
WINBASEAPI
__out_opt
HANDLE
WINAPI
OpenEventW(
    __in DWORD dwDesiredAccess,
    __in BOOL bInheritHandle,
    __in LPCWSTR lpName
    );
#ifdef UNICODE
#define OpenEvent  OpenEventW
#else
#define OpenEvent  OpenEventA
#endif // !UNICODE

WINBASEAPI
__out_opt
HANDLE
WINAPI
CreateSemaphoreA(
    __in_opt LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
    __in     LONG lInitialCount,
    __in     LONG lMaximumCount,
    __in_opt LPCSTR lpName
    );
WINBASEAPI
__out_opt
HANDLE
WINAPI
CreateSemaphoreW(
    __in_opt LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
    __in     LONG lInitialCount,
    __in     LONG lMaximumCount,
    __in_opt LPCWSTR lpName
    );
#ifdef UNICODE
#define CreateSemaphore  CreateSemaphoreW
#else
#define CreateSemaphore  CreateSemaphoreA
#endif // !UNICODE

WINBASEAPI
__out_opt
HANDLE
WINAPI
OpenSemaphoreA(
    __in DWORD dwDesiredAccess,
    __in BOOL bInheritHandle,
    __in LPCSTR lpName
    );
WINBASEAPI
__out_opt
HANDLE
WINAPI
OpenSemaphoreW(
    __in DWORD dwDesiredAccess,
    __in BOOL bInheritHandle,
    __in LPCWSTR lpName
    );
#ifdef UNICODE
#define OpenSemaphore  OpenSemaphoreW
#else
#define OpenSemaphore  OpenSemaphoreA
#endif // !UNICODE

#if (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)
typedef
VOID
(APIENTRY *PTIMERAPCROUTINE)(
    __in_opt LPVOID lpArgToCompletionRoutine,
    __in     DWORD dwTimerLowValue,
    __in     DWORD dwTimerHighValue
    );

WINBASEAPI
__out_opt
HANDLE
WINAPI
CreateWaitableTimerA(
    __in_opt LPSECURITY_ATTRIBUTES lpTimerAttributes,
    __in     BOOL bManualReset,
    __in_opt LPCSTR lpTimerName
    );
WINBASEAPI
__out_opt
HANDLE
WINAPI
CreateWaitableTimerW(
    __in_opt LPSECURITY_ATTRIBUTES lpTimerAttributes,
    __in     BOOL bManualReset,
    __in_opt LPCWSTR lpTimerName
    );
#ifdef UNICODE
#define CreateWaitableTimer  CreateWaitableTimerW
#else
#define CreateWaitableTimer  CreateWaitableTimerA
#endif // !UNICODE

WINBASEAPI
__out_opt
HANDLE
WINAPI
OpenWaitableTimerA(
    __in DWORD dwDesiredAccess,
    __in BOOL bInheritHandle,
    __in LPCSTR lpTimerName
    );
WINBASEAPI
__out_opt
HANDLE
WINAPI
OpenWaitableTimerW(
    __in DWORD dwDesiredAccess,
    __in BOOL bInheritHandle,
    __in LPCWSTR lpTimerName
    );
#ifdef UNICODE
#define OpenWaitableTimer  OpenWaitableTimerW
#else
#define OpenWaitableTimer  OpenWaitableTimerA
#endif // !UNICODE

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN7)

BOOL
WINAPI
SetWaitableTimerEx(
    __in     HANDLE hTimer,
    __in     const LARGE_INTEGER *lpDueTime,
    __in     LONG lPeriod,
    __in_opt PTIMERAPCROUTINE pfnCompletionRoutine,
    __in_opt LPVOID lpArgToCompletionRoutine,
    __in_opt PREASON_CONTEXT WakeContext,
    __in     ULONG TolerableDelay
    );

#endif // (_WIN32_WINNT >= _WIN32_WINNT_WIN7)

WINBASEAPI
BOOL
WINAPI
SetWaitableTimer(
    __in     HANDLE hTimer,
    __in     const LARGE_INTEGER *lpDueTime,
    __in     LONG lPeriod,
    __in_opt PTIMERAPCROUTINE pfnCompletionRoutine,
    __in_opt LPVOID lpArgToCompletionRoutine,
    __in     BOOL fResume
    );

WINBASEAPI
BOOL
WINAPI
CancelWaitableTimer(
    __in HANDLE hTimer
    );

#if (_WIN32_WINNT >= 0x0600)

#define CREATE_MUTEX_INITIAL_OWNER  0x00000001

WINBASEAPI
__out_opt
HANDLE
WINAPI
CreateMutexExA(
    __in_opt LPSECURITY_ATTRIBUTES lpMutexAttributes,
    __in_opt LPCSTR lpName,
    __in     DWORD dwFlags,
    __in     DWORD dwDesiredAccess
    );
WINBASEAPI
__out_opt
HANDLE
WINAPI
CreateMutexExW(
    __in_opt LPSECURITY_ATTRIBUTES lpMutexAttributes,
    __in_opt LPCWSTR lpName,
    __in     DWORD dwFlags,
    __in     DWORD dwDesiredAccess
    );
#ifdef UNICODE
#define CreateMutexEx  CreateMutexExW
#else
#define CreateMutexEx  CreateMutexExA
#endif // !UNICODE

#define CREATE_EVENT_MANUAL_RESET   0x00000001
#define CREATE_EVENT_INITIAL_SET    0x00000002

WINBASEAPI
__out_opt
HANDLE
WINAPI
CreateEventExA(
    __in_opt LPSECURITY_ATTRIBUTES lpEventAttributes,
    __in_opt LPCSTR lpName,
    __in     DWORD dwFlags,
    __in     DWORD dwDesiredAccess
    );
WINBASEAPI
__out_opt
HANDLE
WINAPI
CreateEventExW(
    __in_opt LPSECURITY_ATTRIBUTES lpEventAttributes,
    __in_opt LPCWSTR lpName,
    __in     DWORD dwFlags,
    __in     DWORD dwDesiredAccess
    );
#ifdef UNICODE
#define CreateEventEx  CreateEventExW
#else
#define CreateEventEx  CreateEventExA
#endif // !UNICODE

WINBASEAPI
__out_opt
HANDLE
WINAPI
CreateSemaphoreExA(
    __in_opt    LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
    __in        LONG lInitialCount,
    __in        LONG lMaximumCount,
    __in_opt    LPCSTR lpName,
    __reserved  DWORD dwFlags,
    __in        DWORD dwDesiredAccess
    );
WINBASEAPI
__out_opt
HANDLE
WINAPI
CreateSemaphoreExW(
    __in_opt    LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
    __in        LONG lInitialCount,
    __in        LONG lMaximumCount,
    __in_opt    LPCWSTR lpName,
    __reserved  DWORD dwFlags,
    __in        DWORD dwDesiredAccess
    );
#ifdef UNICODE
#define CreateSemaphoreEx  CreateSemaphoreExW
#else
#define CreateSemaphoreEx  CreateSemaphoreExA
#endif // !UNICODE

#define CREATE_WAITABLE_TIMER_MANUAL_RESET  0x00000001

WINBASEAPI
__out_opt
HANDLE
WINAPI
CreateWaitableTimerExA(
    __in_opt LPSECURITY_ATTRIBUTES lpTimerAttributes,
    __in_opt LPCSTR lpTimerName,
    __in     DWORD dwFlags,
    __in     DWORD dwDesiredAccess
    );
WINBASEAPI
__out_opt
HANDLE
WINAPI
CreateWaitableTimerExW(
    __in_opt LPSECURITY_ATTRIBUTES lpTimerAttributes,
    __in_opt LPCWSTR lpTimerName,
    __in     DWORD dwFlags,
    __in     DWORD dwDesiredAccess
    );
#ifdef UNICODE
#define CreateWaitableTimerEx  CreateWaitableTimerExW
#else
#define CreateWaitableTimerEx  CreateWaitableTimerExA
#endif // !UNICODE

#endif /* (_WIN32_WINNT >= 0x0600) */

#endif /* (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400) */

WINBASEAPI
__out_opt
HANDLE
WINAPI
CreateFileMappingA(
    __in     HANDLE hFile,
    __in_opt LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
    __in     DWORD flProtect,
    __in     DWORD dwMaximumSizeHigh,
    __in     DWORD dwMaximumSizeLow,
    __in_opt LPCSTR lpName
    );
WINBASEAPI
__out_opt
HANDLE
WINAPI
CreateFileMappingW(
    __in     HANDLE hFile,
    __in_opt LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
    __in     DWORD flProtect,
    __in     DWORD dwMaximumSizeHigh,
    __in     DWORD dwMaximumSizeLow,
    __in_opt LPCWSTR lpName
    );
#ifdef UNICODE
#define CreateFileMapping  CreateFileMappingW
#else
#define CreateFileMapping  CreateFileMappingA
#endif // !UNICODE

#if _WIN32_WINNT >= 0x0600

WINBASEAPI
__out_opt
HANDLE
WINAPI
CreateFileMappingNumaA(
    __in     HANDLE hFile,
    __in_opt LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
    __in     DWORD flProtect,
    __in     DWORD dwMaximumSizeHigh,
    __in     DWORD dwMaximumSizeLow,
    __in_opt LPCSTR lpName,
    __in     DWORD nndPreferred
    );
WINBASEAPI
__out_opt
HANDLE
WINAPI
CreateFileMappingNumaW(
    __in     HANDLE hFile,
    __in_opt LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
    __in     DWORD flProtect,
    __in     DWORD dwMaximumSizeHigh,
    __in     DWORD dwMaximumSizeLow,
    __in_opt LPCWSTR lpName,
    __in     DWORD nndPreferred
    );
#ifdef UNICODE
#define CreateFileMappingNuma  CreateFileMappingNumaW
#else
#define CreateFileMappingNuma  CreateFileMappingNumaA
#endif // !UNICODE

#endif // _WIN32_WINNT >= 0x0600

WINBASEAPI
__out
HANDLE
WINAPI
OpenFileMappingA(
    __in DWORD dwDesiredAccess,
    __in BOOL bInheritHandle,
    __in LPCSTR lpName
    );
WINBASEAPI
__out
HANDLE
WINAPI
OpenFileMappingW(
    __in DWORD dwDesiredAccess,
    __in BOOL bInheritHandle,
    __in LPCWSTR lpName
    );
#ifdef UNICODE
#define OpenFileMapping  OpenFileMappingW
#else
#define OpenFileMapping  OpenFileMappingA
#endif // !UNICODE

WINBASEAPI
DWORD
WINAPI
GetLogicalDriveStringsA(
    __in DWORD nBufferLength,
    __out_ecount_part_opt(nBufferLength, return + 1) LPSTR lpBuffer
    );
WINBASEAPI
DWORD
WINAPI
GetLogicalDriveStringsW(
    __in DWORD nBufferLength,
    __out_ecount_part_opt(nBufferLength, return + 1) LPWSTR lpBuffer
    );
#ifdef UNICODE
#define GetLogicalDriveStrings  GetLogicalDriveStringsW
#else
#define GetLogicalDriveStrings  GetLogicalDriveStringsA
#endif // !UNICODE

#if _WIN32_WINNT >= 0x0501

typedef enum _MEMORY_RESOURCE_NOTIFICATION_TYPE {
    LowMemoryResourceNotification,
    HighMemoryResourceNotification
} MEMORY_RESOURCE_NOTIFICATION_TYPE;

WINBASEAPI
__out_opt
HANDLE
WINAPI
CreateMemoryResourceNotification(
    __in MEMORY_RESOURCE_NOTIFICATION_TYPE NotificationType
    );

WINBASEAPI
BOOL
WINAPI
QueryMemoryResourceNotification(
    __in  HANDLE ResourceNotificationHandle,
    __out PBOOL  ResourceState
    );

#endif // _WIN32_WINNT >= 0x0501


WINBASEAPI
__out_opt
HMODULE
WINAPI
LoadLibraryA(
    __in LPCSTR lpLibFileName
    );
WINBASEAPI
__out_opt
HMODULE
WINAPI
LoadLibraryW(
    __in LPCWSTR lpLibFileName
    );
#ifdef UNICODE
#define LoadLibrary  LoadLibraryW
#else
#define LoadLibrary  LoadLibraryA
#endif // !UNICODE

WINBASEAPI
__out_opt
HMODULE
WINAPI
LoadLibraryExA(
    __in       LPCSTR lpLibFileName,
    __reserved HANDLE hFile,
    __in       DWORD dwFlags
    );
WINBASEAPI
__out_opt
HMODULE
WINAPI
LoadLibraryExW(
    __in       LPCWSTR lpLibFileName,
    __reserved HANDLE hFile,
    __in       DWORD dwFlags
    );
#ifdef UNICODE
#define LoadLibraryEx  LoadLibraryExW
#else
#define LoadLibraryEx  LoadLibraryExA
#endif // !UNICODE

#define DONT_RESOLVE_DLL_REFERENCES         0x00000001
#define LOAD_LIBRARY_AS_DATAFILE            0x00000002
#define LOAD_WITH_ALTERED_SEARCH_PATH       0x00000008
#define LOAD_IGNORE_CODE_AUTHZ_LEVEL        0x00000010
#define LOAD_LIBRARY_AS_IMAGE_RESOURCE      0x00000020
#define LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE  0x00000040
#define LOAD_LIBRARY_REQUIRE_SIGNED_TARGET  0x00000080

WINBASEAPI
DWORD
WINAPI
GetModuleFileNameA(
    __in_opt HMODULE hModule,
    __out_ecount_part(nSize, return + 1) LPSTR lpFilename,
    __in     DWORD nSize
    );
WINBASEAPI
DWORD
WINAPI
GetModuleFileNameW(
    __in_opt HMODULE hModule,
    __out_ecount_part(nSize, return + 1) LPWSTR lpFilename,
    __in     DWORD nSize
    );
#ifdef UNICODE
#define GetModuleFileName  GetModuleFileNameW
#else
#define GetModuleFileName  GetModuleFileNameA
#endif // !UNICODE

WINBASEAPI
__out_opt
HMODULE
WINAPI
GetModuleHandleA(
    __in_opt LPCSTR lpModuleName
    );
WINBASEAPI
__out_opt
HMODULE
WINAPI
GetModuleHandleW(
    __in_opt LPCWSTR lpModuleName
    );
#ifdef UNICODE
#define GetModuleHandle  GetModuleHandleW
#else
#define GetModuleHandle  GetModuleHandleA
#endif // !UNICODE

#if !defined(RC_INVOKED)
#if _WIN32_WINNT > 0x0500 || defined(WINBASE_DECLARE_GET_MODULE_HANDLE_EX) || ISOLATION_AWARE_ENABLED

#define GET_MODULE_HANDLE_EX_FLAG_PIN                 (0x00000001)
#define GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT  (0x00000002)
#define GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS        (0x00000004)

typedef
BOOL
(WINAPI*
PGET_MODULE_HANDLE_EXA)(
    __in        DWORD        dwFlags,
    __in_opt    LPCSTR     lpModuleName,
    __deref_out HMODULE*    phModule
    );
typedef
BOOL
(WINAPI*
PGET_MODULE_HANDLE_EXW)(
    __in        DWORD        dwFlags,
    __in_opt    LPCWSTR     lpModuleName,
    __deref_out HMODULE*    phModule
    );
#ifdef UNICODE
#define PGET_MODULE_HANDLE_EX  PGET_MODULE_HANDLE_EXW
#else
#define PGET_MODULE_HANDLE_EX  PGET_MODULE_HANDLE_EXA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
GetModuleHandleExA(
    __in        DWORD    dwFlags,
    __in_opt    LPCSTR lpModuleName,
    __out HMODULE* phModule
    );
WINBASEAPI
BOOL
WINAPI
GetModuleHandleExW(
    __in        DWORD    dwFlags,
    __in_opt    LPCWSTR lpModuleName,
    __out HMODULE* phModule
    );
#ifdef UNICODE
#define GetModuleHandleEx  GetModuleHandleExW
#else
#define GetModuleHandleEx  GetModuleHandleExA
#endif // !UNICODE

#endif
#endif

#if _WIN32_WINNT >= 0x0502

WINBASEAPI
BOOL
WINAPI
NeedCurrentDirectoryForExePathA(
    __in LPCSTR ExeName
    );
WINBASEAPI
BOOL
WINAPI
NeedCurrentDirectoryForExePathW(
    __in LPCWSTR ExeName
    );
#ifdef UNICODE
#define NeedCurrentDirectoryForExePath  NeedCurrentDirectoryForExePathW
#else
#define NeedCurrentDirectoryForExePath  NeedCurrentDirectoryForExePathA
#endif // !UNICODE

#endif

#if _WIN32_WINNT >= 0x0600

#define PROCESS_NAME_NATIVE     0x00000001

WINBASEAPI
BOOL
WINAPI
QueryFullProcessImageNameA(
    __in HANDLE hProcess,
    __in DWORD dwFlags,
    __out_ecount_part(*lpdwSize, *lpdwSize) LPSTR lpExeName,
    __inout PDWORD lpdwSize
    );
WINBASEAPI
BOOL
WINAPI
QueryFullProcessImageNameW(
    __in HANDLE hProcess,
    __in DWORD dwFlags,
    __out_ecount_part(*lpdwSize, *lpdwSize) LPWSTR lpExeName,
    __inout PDWORD lpdwSize
    );
#ifdef UNICODE
#define QueryFullProcessImageName  QueryFullProcessImageNameW
#else
#define QueryFullProcessImageName  QueryFullProcessImageNameA
#endif // !UNICODE

#endif

#if (_WIN32_WINNT >= 0x0600)

//
// Extended process and thread attribute support
//

#define PROC_THREAD_ATTRIBUTE_NUMBER    0x0000FFFF
#define PROC_THREAD_ATTRIBUTE_THREAD    0x00010000  // Attribute may be used with thread creation
#define PROC_THREAD_ATTRIBUTE_INPUT     0x00020000  // Attribute is input only
#define PROC_THREAD_ATTRIBUTE_ADDITIVE  0x00040000  // Attribute may be "accumulated," e.g. bitmasks, counters, etc.

typedef enum _PROC_THREAD_ATTRIBUTE_NUM {
    ProcThreadAttributeParentProcess = 0,
    ProcThreadAttributeExtendedFlags,
    ProcThreadAttributeHandleList,
    ProcThreadAttributeGroupAffinity,
    ProcThreadAttributePreferredNode,
    ProcThreadAttributeIdealProcessor,
    ProcThreadAttributeUmsThread,
    ProcThreadAttributeMitigationPolicy,
    ProcThreadAttributeMax
} PROC_THREAD_ATTRIBUTE_NUM;

#define ProcThreadAttributeValue(Number, Thread, Input, Additive) \
    (((Number) & PROC_THREAD_ATTRIBUTE_NUMBER) | \
     ((Thread != FALSE) ? PROC_THREAD_ATTRIBUTE_THREAD : 0) | \
     ((Input != FALSE) ? PROC_THREAD_ATTRIBUTE_INPUT : 0) | \
     ((Additive != FALSE) ? PROC_THREAD_ATTRIBUTE_ADDITIVE : 0))

#define PROC_THREAD_ATTRIBUTE_PARENT_PROCESS \
    ProcThreadAttributeValue (ProcThreadAttributeParentProcess, FALSE, TRUE, FALSE)
#define PROC_THREAD_ATTRIBUTE_EXTENDED_FLAGS \
    ProcThreadAttributeValue (ProcThreadAttributeExtendedFlags, FALSE, TRUE, TRUE)
#define PROC_THREAD_ATTRIBUTE_HANDLE_LIST \
    ProcThreadAttributeValue (ProcThreadAttributeHandleList, FALSE, TRUE, FALSE)
#define PROC_THREAD_ATTRIBUTE_GROUP_AFFINITY \
    ProcThreadAttributeValue (ProcThreadAttributeGroupAffinity, TRUE, TRUE, FALSE)
#define PROC_THREAD_ATTRIBUTE_PREFERRED_NODE \
    ProcThreadAttributeValue (ProcThreadAttributePreferredNode, FALSE, TRUE, FALSE)
#define PROC_THREAD_ATTRIBUTE_IDEAL_PROCESSOR \
    ProcThreadAttributeValue (ProcThreadAttributeIdealProcessor, TRUE, TRUE, FALSE)
#define PROC_THREAD_ATTRIBUTE_UMS_THREAD \
    ProcThreadAttributeValue (ProcThreadAttributeUmsThread, TRUE, TRUE, FALSE)
#define PROC_THREAD_ATTRIBUTE_MITIGATION_POLICY \
    ProcThreadAttributeValue (ProcThreadAttributeMitigationPolicy, FALSE, TRUE, FALSE)


#define PROCESS_CREATION_MITIGATION_POLICY_DEP_ENABLE            0x01
#define PROCESS_CREATION_MITIGATION_POLICY_DEP_ATL_THUNK_ENABLE  0x02
#define PROCESS_CREATION_MITIGATION_POLICY_SEHOP_ENABLE          0x04

WINBASEAPI
BOOL
WINAPI
InitializeProcThreadAttributeList(
    __out_xcount_opt(*lpSize) LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList,
    __in DWORD dwAttributeCount,
    __reserved DWORD dwFlags,
    __inout PSIZE_T lpSize
    );

WINBASEAPI
VOID
WINAPI
DeleteProcThreadAttributeList(
    __inout LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList
    );

#define PROC_THREAD_ATTRIBUTE_REPLACE_VALUE     0x00000001

WINBASEAPI
BOOL
WINAPI
UpdateProcThreadAttribute(
    __inout LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList,
    __in DWORD dwFlags,
    __in DWORD_PTR Attribute,
    __in_bcount_opt(cbSize) PVOID lpValue,
    __in SIZE_T cbSize,
    __out_bcount_opt(cbSize) PVOID lpPreviousValue,
    __in_opt PSIZE_T lpReturnSize
    );

#endif // (_WIN32_WINNT >= 0x0600)


WINBASEAPI
BOOL
WINAPI
CreateProcessA(
    __in_opt    LPCSTR lpApplicationName,
    __inout_opt LPSTR lpCommandLine,
    __in_opt    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    __in_opt    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    __in        BOOL bInheritHandles,
    __in        DWORD dwCreationFlags,
    __in_opt    LPVOID lpEnvironment,
    __in_opt    LPCSTR lpCurrentDirectory,
    __in        LPSTARTUPINFOA lpStartupInfo,
    __out       LPPROCESS_INFORMATION lpProcessInformation
    );
WINBASEAPI
BOOL
WINAPI
CreateProcessW(
    __in_opt    LPCWSTR lpApplicationName,
    __inout_opt LPWSTR lpCommandLine,
    __in_opt    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    __in_opt    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    __in        BOOL bInheritHandles,
    __in        DWORD dwCreationFlags,
    __in_opt    LPVOID lpEnvironment,
    __in_opt    LPCWSTR lpCurrentDirectory,
    __in        LPSTARTUPINFOW lpStartupInfo,
    __out       LPPROCESS_INFORMATION lpProcessInformation
    );
#ifdef UNICODE
#define CreateProcess  CreateProcessW
#else
#define CreateProcess  CreateProcessA
#endif // !UNICODE



WINBASEAPI
BOOL
WINAPI
SetProcessShutdownParameters(
    __in DWORD dwLevel,
    __in DWORD dwFlags
    );

WINBASEAPI
BOOL
WINAPI
GetProcessShutdownParameters(
    __out LPDWORD lpdwLevel,
    __out LPDWORD lpdwFlags
    );

WINBASEAPI
DWORD
WINAPI
GetProcessVersion(
    __in DWORD ProcessId
    );

WINBASEAPI
VOID
WINAPI
FatalAppExitA(
    __in UINT uAction,
    __in LPCSTR lpMessageText
    );
WINBASEAPI
VOID
WINAPI
FatalAppExitW(
    __in UINT uAction,
    __in LPCWSTR lpMessageText
    );
#ifdef UNICODE
#define FatalAppExit  FatalAppExitW
#else
#define FatalAppExit  FatalAppExitA
#endif // !UNICODE

WINBASEAPI
VOID
WINAPI
GetStartupInfoA(
    __out LPSTARTUPINFOA lpStartupInfo
    );
WINBASEAPI
VOID
WINAPI
GetStartupInfoW(
    __out LPSTARTUPINFOW lpStartupInfo
    );
#ifdef UNICODE
#define GetStartupInfo  GetStartupInfoW
#else
#define GetStartupInfo  GetStartupInfoA
#endif // !UNICODE

WINBASEAPI
__out
LPSTR
WINAPI
GetCommandLineA(
    VOID
    );
WINBASEAPI
__out
LPWSTR
WINAPI
GetCommandLineW(
    VOID
    );
#ifdef UNICODE
#define GetCommandLine  GetCommandLineW
#else
#define GetCommandLine  GetCommandLineA
#endif // !UNICODE

WINBASEAPI
__success(return < nSize)
__success(return != 0)
DWORD
WINAPI
GetEnvironmentVariableA(
    __in_opt LPCSTR lpName,
    __out_ecount_part_opt(nSize, return + 1) LPSTR lpBuffer,
    __in DWORD nSize
    );
WINBASEAPI
__success(return < nSize)
__success(return != 0)
DWORD
WINAPI
GetEnvironmentVariableW(
    __in_opt LPCWSTR lpName,
    __out_ecount_part_opt(nSize, return + 1) LPWSTR lpBuffer,
    __in DWORD nSize
    );
#ifdef UNICODE
#define GetEnvironmentVariable  GetEnvironmentVariableW
#else
#define GetEnvironmentVariable  GetEnvironmentVariableA
#endif // !UNICODE

#if defined(_M_CEE)
#undef GetEnvironmentVariable
__inline
DWORD
GetEnvironmentVariable(
    __in_opt LPCTSTR lpName,
    __out_ecount_part_opt(nSize, return + 1) LPTSTR lpBuffer,
    __in DWORD nSize
    )
{
#ifdef UNICODE
    return GetEnvironmentVariableW(
#else
    return GetEnvironmentVariableA(
#endif
        lpName,
        lpBuffer,
        nSize
        );
}
#endif  /* _M_CEE */

WINBASEAPI
BOOL
WINAPI
SetEnvironmentVariableA(
    __in     LPCSTR lpName,
    __in_opt LPCSTR lpValue
    );
WINBASEAPI
BOOL
WINAPI
SetEnvironmentVariableW(
    __in     LPCWSTR lpName,
    __in_opt LPCWSTR lpValue
    );
#ifdef UNICODE
#define SetEnvironmentVariable  SetEnvironmentVariableW
#else
#define SetEnvironmentVariable  SetEnvironmentVariableA
#endif // !UNICODE

#if defined(_M_CEE)
#undef SetEnvironmentVariable
__inline
BOOL
SetEnvironmentVariable(
    LPCTSTR lpName,
    LPCTSTR lpValue
    )
{
#ifdef UNICODE
    return SetEnvironmentVariableW(
#else
    return SetEnvironmentVariableA(
#endif
        lpName,
        lpValue
        );
}
#endif  /* _M_CEE */

WINBASEAPI
__success(return <= nSize)
__success(return != 0)
DWORD
WINAPI
ExpandEnvironmentStringsA(
    __in LPCSTR lpSrc,
    __out_ecount_part_opt(nSize, return) LPSTR lpDst,
    __in DWORD nSize
    );
WINBASEAPI
__success(return <= nSize)
__success(return != 0)
DWORD
WINAPI
ExpandEnvironmentStringsW(
    __in LPCWSTR lpSrc,
    __out_ecount_part_opt(nSize, return) LPWSTR lpDst,
    __in DWORD nSize
    );
#ifdef UNICODE
#define ExpandEnvironmentStrings  ExpandEnvironmentStringsW
#else
#define ExpandEnvironmentStrings  ExpandEnvironmentStringsA
#endif // !UNICODE

WINBASEAPI
DWORD
WINAPI
GetFirmwareEnvironmentVariableA(
    __in LPCSTR lpName,
    __in LPCSTR lpGuid,
    __out_bcount_part_opt(nSize, return) PVOID pBuffer,
    __in DWORD    nSize
    );
WINBASEAPI
DWORD
WINAPI
GetFirmwareEnvironmentVariableW(
    __in LPCWSTR lpName,
    __in LPCWSTR lpGuid,
    __out_bcount_part_opt(nSize, return) PVOID pBuffer,
    __in DWORD    nSize
    );
#ifdef UNICODE
#define GetFirmwareEnvironmentVariable  GetFirmwareEnvironmentVariableW
#else
#define GetFirmwareEnvironmentVariable  GetFirmwareEnvironmentVariableA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
SetFirmwareEnvironmentVariableA(
    __in LPCSTR lpName,
    __in LPCSTR lpGuid,
    __in_bcount_opt(nSize) PVOID pValue,
    __in DWORD    nSize
    );
WINBASEAPI
BOOL
WINAPI
SetFirmwareEnvironmentVariableW(
    __in LPCWSTR lpName,
    __in LPCWSTR lpGuid,
    __in_bcount_opt(nSize) PVOID pValue,
    __in DWORD    nSize
    );
#ifdef UNICODE
#define SetFirmwareEnvironmentVariable  SetFirmwareEnvironmentVariableW
#else
#define SetFirmwareEnvironmentVariable  SetFirmwareEnvironmentVariableA
#endif // !UNICODE


WINBASEAPI
VOID
WINAPI
OutputDebugStringA(
    __in_opt LPCSTR lpOutputString
    );
WINBASEAPI
VOID
WINAPI
OutputDebugStringW(
    __in_opt LPCWSTR lpOutputString
    );
#ifdef UNICODE
#define OutputDebugString  OutputDebugStringW
#else
#define OutputDebugString  OutputDebugStringA
#endif // !UNICODE

WINBASEAPI
__out_opt
HRSRC
WINAPI
FindResourceA(
    __in_opt HMODULE hModule,
    __in     LPCSTR lpName,
    __in     LPCSTR lpType
    );
WINBASEAPI
__out_opt
HRSRC
WINAPI
FindResourceW(
    __in_opt HMODULE hModule,
    __in     LPCWSTR lpName,
    __in     LPCWSTR lpType
    );
#ifdef UNICODE
#define FindResource  FindResourceW
#else
#define FindResource  FindResourceA
#endif // !UNICODE

WINBASEAPI
__out_opt
HRSRC
WINAPI
FindResourceExA(
    __in_opt HMODULE hModule,
    __in     LPCSTR lpType,
    __in     LPCSTR lpName,
    __in     WORD    wLanguage
    );
WINBASEAPI
__out_opt
HRSRC
WINAPI
FindResourceExW(
    __in_opt HMODULE hModule,
    __in     LPCWSTR lpType,
    __in     LPCWSTR lpName,
    __in     WORD    wLanguage
    );
#ifdef UNICODE
#define FindResourceEx  FindResourceExW
#else
#define FindResourceEx  FindResourceExA
#endif // !UNICODE

#ifdef STRICT
typedef BOOL (CALLBACK* ENUMRESTYPEPROCA)(__in_opt HMODULE hModule, __in LPSTR lpType,
__in LONG_PTR lParam);
typedef BOOL (CALLBACK* ENUMRESTYPEPROCW)(__in_opt HMODULE hModule, __in LPWSTR lpType,
__in LONG_PTR lParam);
#ifdef UNICODE
#define ENUMRESTYPEPROC  ENUMRESTYPEPROCW
#else
#define ENUMRESTYPEPROC  ENUMRESTYPEPROCA
#endif // !UNICODE
typedef BOOL (CALLBACK* ENUMRESNAMEPROCA)(__in_opt HMODULE hModule, __in LPCSTR lpType,
__in LPSTR lpName, __in LONG_PTR lParam);
typedef BOOL (CALLBACK* ENUMRESNAMEPROCW)(__in_opt HMODULE hModule, __in LPCWSTR lpType,
__in LPWSTR lpName, __in LONG_PTR lParam);
#ifdef UNICODE
#define ENUMRESNAMEPROC  ENUMRESNAMEPROCW
#else
#define ENUMRESNAMEPROC  ENUMRESNAMEPROCA
#endif // !UNICODE
typedef BOOL (CALLBACK* ENUMRESLANGPROCA)(__in_opt HMODULE hModule, __in LPCSTR lpType,
__in LPCSTR lpName, __in WORD  wLanguage, __in LONG_PTR lParam);
typedef BOOL (CALLBACK* ENUMRESLANGPROCW)(__in_opt HMODULE hModule, __in LPCWSTR lpType,
__in LPCWSTR lpName, __in WORD  wLanguage, __in LONG_PTR lParam);
#ifdef UNICODE
#define ENUMRESLANGPROC  ENUMRESLANGPROCW
#else
#define ENUMRESLANGPROC  ENUMRESLANGPROCA
#endif // !UNICODE
#else
typedef FARPROC ENUMRESTYPEPROCA;
typedef FARPROC ENUMRESTYPEPROCW;
#ifdef UNICODE
typedef ENUMRESTYPEPROCW ENUMRESTYPEPROC;
#else
typedef ENUMRESTYPEPROCA ENUMRESTYPEPROC;
#endif // UNICODE
typedef FARPROC ENUMRESNAMEPROCA;
typedef FARPROC ENUMRESNAMEPROCW;
#ifdef UNICODE
typedef ENUMRESNAMEPROCW ENUMRESNAMEPROC;
#else
typedef ENUMRESNAMEPROCA ENUMRESNAMEPROC;
#endif // UNICODE
typedef FARPROC ENUMRESLANGPROCA;
typedef FARPROC ENUMRESLANGPROCW;
#ifdef UNICODE
typedef ENUMRESLANGPROCW ENUMRESLANGPROC;
#else
typedef ENUMRESLANGPROCA ENUMRESLANGPROC;
#endif // UNICODE
#endif

WINBASEAPI
BOOL
WINAPI
EnumResourceTypesA(
    __in_opt HMODULE hModule,
    __in     ENUMRESTYPEPROCA lpEnumFunc,
    __in     LONG_PTR lParam
    );
WINBASEAPI
BOOL
WINAPI
EnumResourceTypesW(
    __in_opt HMODULE hModule,
    __in     ENUMRESTYPEPROCW lpEnumFunc,
    __in     LONG_PTR lParam
    );
#ifdef UNICODE
#define EnumResourceTypes  EnumResourceTypesW
#else
#define EnumResourceTypes  EnumResourceTypesA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
EnumResourceNamesA(
    __in_opt HMODULE hModule,
    __in     LPCSTR lpType,
    __in     ENUMRESNAMEPROCA lpEnumFunc,
    __in     LONG_PTR lParam
    );
WINBASEAPI
BOOL
WINAPI
EnumResourceNamesW(
    __in_opt HMODULE hModule,
    __in     LPCWSTR lpType,
    __in     ENUMRESNAMEPROCW lpEnumFunc,
    __in     LONG_PTR lParam
    );
#ifdef UNICODE
#define EnumResourceNames  EnumResourceNamesW
#else
#define EnumResourceNames  EnumResourceNamesA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
EnumResourceLanguagesA(
    __in_opt HMODULE hModule,
    __in     LPCSTR lpType,
    __in     LPCSTR lpName,
    __in     ENUMRESLANGPROCA lpEnumFunc,
    __in     LONG_PTR lParam
    );
WINBASEAPI
BOOL
WINAPI
EnumResourceLanguagesW(
    __in_opt HMODULE hModule,
    __in     LPCWSTR lpType,
    __in     LPCWSTR lpName,
    __in     ENUMRESLANGPROCW lpEnumFunc,
    __in     LONG_PTR lParam
    );
#ifdef UNICODE
#define EnumResourceLanguages  EnumResourceLanguagesW
#else
#define EnumResourceLanguages  EnumResourceLanguagesA
#endif // !UNICODE

#define  RESOURCE_ENUM_LN               (0x0001)
#define  RESOURCE_ENUM_MUI              (0x0002)
#define  RESOURCE_ENUM_MUI_SYSTEM       (0x0004)
#define  RESOURCE_ENUM_VALIDATE         (0x0008)
#define  RESOURCE_ENUM_MODULE_EXACT     (0x0010)

WINBASEAPI
BOOL
WINAPI
EnumResourceTypesExA(
    __in_opt HMODULE hModule,
    __in ENUMRESTYPEPROCA lpEnumFunc,
    __in LONG_PTR lParam,
    DWORD dwFlags,
    LANGID LangId
    );
WINBASEAPI
BOOL
WINAPI
EnumResourceTypesExW(
    __in_opt HMODULE hModule,
    __in ENUMRESTYPEPROCW lpEnumFunc,
    __in LONG_PTR lParam,
    DWORD dwFlags,
    LANGID LangId
    );
#ifdef UNICODE
#define EnumResourceTypesEx  EnumResourceTypesExW
#else
#define EnumResourceTypesEx  EnumResourceTypesExA
#endif // !UNICODE


WINBASEAPI
BOOL
WINAPI
EnumResourceNamesExA(
    __in_opt HMODULE hModule,
    __in LPCSTR lpType,
    __in ENUMRESNAMEPROCA lpEnumFunc,
    __in LONG_PTR lParam,
    DWORD dwFlags,
    LANGID LangId
    );
WINBASEAPI
BOOL
WINAPI
EnumResourceNamesExW(
    __in_opt HMODULE hModule,
    __in LPCWSTR lpType,
    __in ENUMRESNAMEPROCW lpEnumFunc,
    __in LONG_PTR lParam,
    DWORD dwFlags,
    LANGID LangId
    );
#ifdef UNICODE
#define EnumResourceNamesEx  EnumResourceNamesExW
#else
#define EnumResourceNamesEx  EnumResourceNamesExA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
EnumResourceLanguagesExA(
    __in_opt HMODULE hModule,
    __in LPCSTR lpType,
    __in LPCSTR lpName,
    __in ENUMRESLANGPROCA lpEnumFunc,
    __in LONG_PTR lParam,
    DWORD dwFlags,
    LANGID LangId
    );
WINBASEAPI
BOOL
WINAPI
EnumResourceLanguagesExW(
    __in_opt HMODULE hModule,
    __in LPCWSTR lpType,
    __in LPCWSTR lpName,
    __in ENUMRESLANGPROCW lpEnumFunc,
    __in LONG_PTR lParam,
    DWORD dwFlags,
    LANGID LangId
    );
#ifdef UNICODE
#define EnumResourceLanguagesEx  EnumResourceLanguagesExW
#else
#define EnumResourceLanguagesEx  EnumResourceLanguagesExA
#endif // !UNICODE

WINBASEAPI
HANDLE
WINAPI
BeginUpdateResourceA(
    __in LPCSTR pFileName,
    __in BOOL bDeleteExistingResources
    );
WINBASEAPI
HANDLE
WINAPI
BeginUpdateResourceW(
    __in LPCWSTR pFileName,
    __in BOOL bDeleteExistingResources
    );
#ifdef UNICODE
#define BeginUpdateResource  BeginUpdateResourceW
#else
#define BeginUpdateResource  BeginUpdateResourceA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
UpdateResourceA(
    __in HANDLE hUpdate,
    __in LPCSTR lpType,
    __in LPCSTR lpName,
    __in WORD wLanguage,
    __in_bcount_opt(cb) LPVOID lpData,
    __in DWORD cb
    );
WINBASEAPI
BOOL
WINAPI
UpdateResourceW(
    __in HANDLE hUpdate,
    __in LPCWSTR lpType,
    __in LPCWSTR lpName,
    __in WORD wLanguage,
    __in_bcount_opt(cb) LPVOID lpData,
    __in DWORD cb
    );
#ifdef UNICODE
#define UpdateResource  UpdateResourceW
#else
#define UpdateResource  UpdateResourceA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
EndUpdateResourceA(
    __in HANDLE hUpdate,
    __in BOOL   fDiscard
    );
WINBASEAPI
BOOL
WINAPI
EndUpdateResourceW(
    __in HANDLE hUpdate,
    __in BOOL   fDiscard
    );
#ifdef UNICODE
#define EndUpdateResource  EndUpdateResourceW
#else
#define EndUpdateResource  EndUpdateResourceA
#endif // !UNICODE


WINBASEAPI
ATOM
WINAPI
GlobalAddAtomA(
    __in_opt LPCSTR lpString
    );
WINBASEAPI
ATOM
WINAPI
GlobalAddAtomW(
    __in_opt LPCWSTR lpString
    );
#ifdef UNICODE
#define GlobalAddAtom  GlobalAddAtomW
#else
#define GlobalAddAtom  GlobalAddAtomA
#endif // !UNICODE

WINBASEAPI
ATOM
WINAPI
GlobalFindAtomA(
    __in_opt LPCSTR lpString
    );
WINBASEAPI
ATOM
WINAPI
GlobalFindAtomW(
    __in_opt LPCWSTR lpString
    );
#ifdef UNICODE
#define GlobalFindAtom  GlobalFindAtomW
#else
#define GlobalFindAtom  GlobalFindAtomA
#endif // !UNICODE

WINBASEAPI
UINT
WINAPI
GlobalGetAtomNameA(
    __in ATOM nAtom,
    __out_ecount_part(nSize, return + 1) LPSTR lpBuffer,
    __in int nSize
    );
WINBASEAPI
UINT
WINAPI
GlobalGetAtomNameW(
    __in ATOM nAtom,
    __out_ecount_part(nSize, return + 1) LPWSTR lpBuffer,
    __in int nSize
    );
#ifdef UNICODE
#define GlobalGetAtomName  GlobalGetAtomNameW
#else
#define GlobalGetAtomName  GlobalGetAtomNameA
#endif // !UNICODE

WINBASEAPI
ATOM
WINAPI
AddAtomA(
    __in_opt LPCSTR lpString
    );
WINBASEAPI
ATOM
WINAPI
AddAtomW(
    __in_opt LPCWSTR lpString
    );
#ifdef UNICODE
#define AddAtom  AddAtomW
#else
#define AddAtom  AddAtomA
#endif // !UNICODE

WINBASEAPI
ATOM
WINAPI
FindAtomA(
    __in_opt LPCSTR lpString
    );
WINBASEAPI
ATOM
WINAPI
FindAtomW(
    __in_opt LPCWSTR lpString
    );
#ifdef UNICODE
#define FindAtom  FindAtomW
#else
#define FindAtom  FindAtomA
#endif // !UNICODE

WINBASEAPI
UINT
WINAPI
GetAtomNameA(
    __in ATOM nAtom,
    __out_ecount_part(nSize, return + 1) LPSTR lpBuffer,
    __in int nSize
    );
WINBASEAPI
UINT
WINAPI
GetAtomNameW(
    __in ATOM nAtom,
    __out_ecount_part(nSize, return + 1) LPWSTR lpBuffer,
    __in int nSize
    );
#ifdef UNICODE
#define GetAtomName  GetAtomNameW
#else
#define GetAtomName  GetAtomNameA
#endif // !UNICODE

WINBASEAPI
UINT
WINAPI
GetProfileIntA(
    __in LPCSTR lpAppName,
    __in LPCSTR lpKeyName,
    __in INT nDefault
    );
WINBASEAPI
UINT
WINAPI
GetProfileIntW(
    __in LPCWSTR lpAppName,
    __in LPCWSTR lpKeyName,
    __in INT nDefault
    );
#ifdef UNICODE
#define GetProfileInt  GetProfileIntW
#else
#define GetProfileInt  GetProfileIntA
#endif // !UNICODE

WINBASEAPI
DWORD
WINAPI
GetProfileStringA(
    __in_opt LPCSTR lpAppName,
    __in_opt LPCSTR lpKeyName,
    __in_opt LPCSTR lpDefault,
    __out_ecount_part_opt(nSize, return + 1) LPSTR lpReturnedString,
    __in     DWORD nSize
    );
WINBASEAPI
DWORD
WINAPI
GetProfileStringW(
    __in_opt LPCWSTR lpAppName,
    __in_opt LPCWSTR lpKeyName,
    __in_opt LPCWSTR lpDefault,
    __out_ecount_part_opt(nSize, return + 1) LPWSTR lpReturnedString,
    __in     DWORD nSize
    );
#ifdef UNICODE
#define GetProfileString  GetProfileStringW
#else
#define GetProfileString  GetProfileStringA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
WriteProfileStringA(
    __in_opt LPCSTR lpAppName,
    __in_opt LPCSTR lpKeyName,
    __in_opt LPCSTR lpString
    );
WINBASEAPI
BOOL
WINAPI
WriteProfileStringW(
    __in_opt LPCWSTR lpAppName,
    __in_opt LPCWSTR lpKeyName,
    __in_opt LPCWSTR lpString
    );
#ifdef UNICODE
#define WriteProfileString  WriteProfileStringW
#else
#define WriteProfileString  WriteProfileStringA
#endif // !UNICODE

WINBASEAPI
DWORD
WINAPI
GetProfileSectionA(
    __in LPCSTR lpAppName,
    __out_ecount_part_opt(nSize, return + 1) LPSTR lpReturnedString,
    __in DWORD nSize
    );
WINBASEAPI
DWORD
WINAPI
GetProfileSectionW(
    __in LPCWSTR lpAppName,
    __out_ecount_part_opt(nSize, return + 1) LPWSTR lpReturnedString,
    __in DWORD nSize
    );
#ifdef UNICODE
#define GetProfileSection  GetProfileSectionW
#else
#define GetProfileSection  GetProfileSectionA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
WriteProfileSectionA(
    __in LPCSTR lpAppName,
    __in LPCSTR lpString
    );
WINBASEAPI
BOOL
WINAPI
WriteProfileSectionW(
    __in LPCWSTR lpAppName,
    __in LPCWSTR lpString
    );
#ifdef UNICODE
#define WriteProfileSection  WriteProfileSectionW
#else
#define WriteProfileSection  WriteProfileSectionA
#endif // !UNICODE

WINBASEAPI
UINT
WINAPI
GetPrivateProfileIntA(
    __in     LPCSTR lpAppName,
    __in     LPCSTR lpKeyName,
    __in     INT nDefault,
    __in_opt LPCSTR lpFileName
    );
WINBASEAPI
UINT
WINAPI
GetPrivateProfileIntW(
    __in     LPCWSTR lpAppName,
    __in     LPCWSTR lpKeyName,
    __in     INT nDefault,
    __in_opt LPCWSTR lpFileName
    );
#ifdef UNICODE
#define GetPrivateProfileInt  GetPrivateProfileIntW
#else
#define GetPrivateProfileInt  GetPrivateProfileIntA
#endif // !UNICODE

#if defined(_M_CEE)
#undef GetPrivateProfileInt
__inline
UINT
GetPrivateProfileInt(
    LPCTSTR lpAppName,
    LPCTSTR lpKeyName,
    INT nDefault,
    LPCTSTR lpFileName
    )
{
#ifdef UNICODE
    return GetPrivateProfileIntW(
#else
    return GetPrivateProfileIntA(
#endif
        lpAppName,
        lpKeyName,
        nDefault,
        lpFileName
        );
}
#endif  /* _M_CEE */

WINBASEAPI
DWORD
WINAPI
GetPrivateProfileStringA(
    __in_opt LPCSTR lpAppName,
    __in_opt LPCSTR lpKeyName,
    __in_opt LPCSTR lpDefault,
    __out_ecount_part_opt(nSize, return + 1) LPSTR lpReturnedString,
    __in     DWORD nSize,
    __in_opt LPCSTR lpFileName
    );
WINBASEAPI
DWORD
WINAPI
GetPrivateProfileStringW(
    __in_opt LPCWSTR lpAppName,
    __in_opt LPCWSTR lpKeyName,
    __in_opt LPCWSTR lpDefault,
    __out_ecount_part_opt(nSize, return + 1) LPWSTR lpReturnedString,
    __in     DWORD nSize,
    __in_opt LPCWSTR lpFileName
    );
#ifdef UNICODE
#define GetPrivateProfileString  GetPrivateProfileStringW
#else
#define GetPrivateProfileString  GetPrivateProfileStringA
#endif // !UNICODE

#if defined(_M_CEE)
#undef GetPrivateProfileString
__inline
DWORD
GetPrivateProfileString(
    LPCTSTR lpAppName,
    LPCTSTR lpKeyName,
    LPCTSTR lpDefault,
    LPTSTR lpReturnedString,
    DWORD nSize,
    LPCTSTR lpFileName
    )
{
#ifdef UNICODE
    return GetPrivateProfileStringW(
#else
    return GetPrivateProfileStringA(
#endif
        lpAppName,
        lpKeyName,
        lpDefault,
        lpReturnedString,
        nSize,
        lpFileName
        );
}
#endif  /* _M_CEE */

WINBASEAPI
BOOL
WINAPI
WritePrivateProfileStringA(
    __in_opt LPCSTR lpAppName,
    __in_opt LPCSTR lpKeyName,
    __in_opt LPCSTR lpString,
    __in_opt LPCSTR lpFileName
    );
WINBASEAPI
BOOL
WINAPI
WritePrivateProfileStringW(
    __in_opt LPCWSTR lpAppName,
    __in_opt LPCWSTR lpKeyName,
    __in_opt LPCWSTR lpString,
    __in_opt LPCWSTR lpFileName
    );
#ifdef UNICODE
#define WritePrivateProfileString  WritePrivateProfileStringW
#else
#define WritePrivateProfileString  WritePrivateProfileStringA
#endif // !UNICODE

WINBASEAPI
DWORD
WINAPI
GetPrivateProfileSectionA(
    __in     LPCSTR lpAppName,
    __out_ecount_part_opt(nSize, return + 1) LPSTR lpReturnedString,
    __in     DWORD nSize,
    __in_opt LPCSTR lpFileName
    );
WINBASEAPI
DWORD
WINAPI
GetPrivateProfileSectionW(
    __in     LPCWSTR lpAppName,
    __out_ecount_part_opt(nSize, return + 1) LPWSTR lpReturnedString,
    __in     DWORD nSize,
    __in_opt LPCWSTR lpFileName
    );
#ifdef UNICODE
#define GetPrivateProfileSection  GetPrivateProfileSectionW
#else
#define GetPrivateProfileSection  GetPrivateProfileSectionA
#endif // !UNICODE

#if defined(_M_CEE)
#undef GetPrivateProfileSection
__inline
DWORD
GetPrivateProfileSection(
    LPCTSTR lpAppName,
    LPTSTR lpReturnedString,
    DWORD nSize,
    LPCTSTR lpFileName
    )
{
#ifdef UNICODE
    return GetPrivateProfileSectionW(
#else
    return GetPrivateProfileSectionA(
#endif
        lpAppName,
        lpReturnedString,
        nSize,
        lpFileName
        );
}
#endif  /* _M_CEE */

WINBASEAPI
BOOL
WINAPI
WritePrivateProfileSectionA(
    __in_opt LPCSTR lpAppName,
    __in_opt LPCSTR lpString,
    __in_opt LPCSTR lpFileName
    );
WINBASEAPI
BOOL
WINAPI
WritePrivateProfileSectionW(
    __in_opt LPCWSTR lpAppName,
    __in_opt LPCWSTR lpString,
    __in_opt LPCWSTR lpFileName
    );
#ifdef UNICODE
#define WritePrivateProfileSection  WritePrivateProfileSectionW
#else
#define WritePrivateProfileSection  WritePrivateProfileSectionA
#endif // !UNICODE


WINBASEAPI
DWORD
WINAPI
GetPrivateProfileSectionNamesA(
    __out_ecount_part_opt(nSize, return + 1) LPSTR lpszReturnBuffer,
    __in     DWORD nSize,
    __in_opt LPCSTR lpFileName
    );
WINBASEAPI
DWORD
WINAPI
GetPrivateProfileSectionNamesW(
    __out_ecount_part_opt(nSize, return + 1) LPWSTR lpszReturnBuffer,
    __in     DWORD nSize,
    __in_opt LPCWSTR lpFileName
    );
#ifdef UNICODE
#define GetPrivateProfileSectionNames  GetPrivateProfileSectionNamesW
#else
#define GetPrivateProfileSectionNames  GetPrivateProfileSectionNamesA
#endif // !UNICODE

#if defined(_M_CEE)
#undef GetPrivateProfileSectionNames
__inline
DWORD
GetPrivateProfileSectionNames(
    LPTSTR lpszReturnBuffer,
    DWORD nSize,
    LPCTSTR lpFileName
    )
{
#ifdef UNICODE
    return GetPrivateProfileSectionNamesW(
#else
    return GetPrivateProfileSectionNamesA(
#endif
        lpszReturnBuffer,
        nSize,
        lpFileName
        );
}
#endif  /* _M_CEE */

WINBASEAPI
BOOL
WINAPI
GetPrivateProfileStructA(
    __in     LPCSTR lpszSection,
    __in     LPCSTR lpszKey,
    __out_bcount_opt(uSizeStruct) LPVOID   lpStruct,
    __in     UINT     uSizeStruct,
    __in_opt LPCSTR szFile
    );
WINBASEAPI
BOOL
WINAPI
GetPrivateProfileStructW(
    __in     LPCWSTR lpszSection,
    __in     LPCWSTR lpszKey,
    __out_bcount_opt(uSizeStruct) LPVOID   lpStruct,
    __in     UINT     uSizeStruct,
    __in_opt LPCWSTR szFile
    );
#ifdef UNICODE
#define GetPrivateProfileStruct  GetPrivateProfileStructW
#else
#define GetPrivateProfileStruct  GetPrivateProfileStructA
#endif // !UNICODE

#if defined(_M_CEE)
#undef GetPrivateProfileStruct
__inline
BOOL
GetPrivateProfileStruct(
    LPCTSTR lpszSection,
    LPCTSTR lpszKey,
    LPVOID   lpStruct,
    UINT     uSizeStruct,
    LPCTSTR szFile
    )
{
#ifdef UNICODE
    return GetPrivateProfileStructW(
#else
    return GetPrivateProfileStructA(
#endif
        lpszSection,
        lpszKey,
        lpStruct,
        uSizeStruct,
        szFile
        );
}
#endif  /* _M_CEE */

WINBASEAPI
BOOL
WINAPI
WritePrivateProfileStructA(
    __in     LPCSTR lpszSection,
    __in     LPCSTR lpszKey,
    __in_bcount_opt(uSizeStruct) LPVOID lpStruct,
    __in     UINT     uSizeStruct,
    __in_opt LPCSTR szFile
    );
WINBASEAPI
BOOL
WINAPI
WritePrivateProfileStructW(
    __in     LPCWSTR lpszSection,
    __in     LPCWSTR lpszKey,
    __in_bcount_opt(uSizeStruct) LPVOID lpStruct,
    __in     UINT     uSizeStruct,
    __in_opt LPCWSTR szFile
    );
#ifdef UNICODE
#define WritePrivateProfileStruct  WritePrivateProfileStructW
#else
#define WritePrivateProfileStruct  WritePrivateProfileStructA
#endif // !UNICODE


WINBASEAPI
UINT
WINAPI
GetDriveTypeA(
    __in_opt LPCSTR lpRootPathName
    );
WINBASEAPI
UINT
WINAPI
GetDriveTypeW(
    __in_opt LPCWSTR lpRootPathName
    );
#ifdef UNICODE
#define GetDriveType  GetDriveTypeW
#else
#define GetDriveType  GetDriveTypeA
#endif // !UNICODE

WINBASEAPI
UINT
WINAPI
GetSystemDirectoryA(
    __out_ecount_part_opt(uSize, return + 1) LPSTR lpBuffer,
    __in UINT uSize
    );
WINBASEAPI
UINT
WINAPI
GetSystemDirectoryW(
    __out_ecount_part_opt(uSize, return + 1) LPWSTR lpBuffer,
    __in UINT uSize
    );
#ifdef UNICODE
#define GetSystemDirectory  GetSystemDirectoryW
#else
#define GetSystemDirectory  GetSystemDirectoryA
#endif // !UNICODE

WINBASEAPI
DWORD
WINAPI
GetTempPathA(
    __in DWORD nBufferLength,
    __out_ecount_part_opt(nBufferLength, return + 1) LPSTR lpBuffer
    );
WINBASEAPI
DWORD
WINAPI
GetTempPathW(
    __in DWORD nBufferLength,
    __out_ecount_part_opt(nBufferLength, return + 1) LPWSTR lpBuffer
    );
#ifdef UNICODE
#define GetTempPath  GetTempPathW
#else
#define GetTempPath  GetTempPathA
#endif // !UNICODE

WINBASEAPI
UINT
WINAPI
GetTempFileNameA(
    __in LPCSTR lpPathName,
    __in LPCSTR lpPrefixString,
    __in UINT uUnique,
    __out_ecount(MAX_PATH) LPSTR lpTempFileName
    );
WINBASEAPI
UINT
WINAPI
GetTempFileNameW(
    __in LPCWSTR lpPathName,
    __in LPCWSTR lpPrefixString,
    __in UINT uUnique,
    __out_ecount(MAX_PATH) LPWSTR lpTempFileName
    );
#ifdef UNICODE
#define GetTempFileName  GetTempFileNameW
#else
#define GetTempFileName  GetTempFileNameA
#endif // !UNICODE

#if defined(_M_CEE)
#undef GetTempFileName
__inline
UINT
GetTempFileName(
    LPCTSTR lpPathName,
    LPCTSTR lpPrefixString,
    UINT uUnique,
    LPTSTR lpTempFileName
    )
{
#ifdef UNICODE
    return GetTempFileNameW(
#else
    return GetTempFileNameA(
#endif
        lpPathName,
        lpPrefixString,
        uUnique,
        lpTempFileName
        );
}
#endif  /* _M_CEE */

WINBASEAPI
UINT
WINAPI
GetWindowsDirectoryA(
    __out_ecount_part_opt(uSize, return + 1) LPSTR lpBuffer,
    __in UINT uSize
    );
WINBASEAPI
UINT
WINAPI
GetWindowsDirectoryW(
    __out_ecount_part_opt(uSize, return + 1) LPWSTR lpBuffer,
    __in UINT uSize
    );
#ifdef UNICODE
#define GetWindowsDirectory  GetWindowsDirectoryW
#else
#define GetWindowsDirectory  GetWindowsDirectoryA
#endif // !UNICODE

WINBASEAPI
UINT
WINAPI
GetSystemWindowsDirectoryA(
    __out_ecount_part_opt(uSize, return + 1) LPSTR lpBuffer,
    __in UINT uSize
    );
WINBASEAPI
UINT
WINAPI
GetSystemWindowsDirectoryW(
    __out_ecount_part_opt(uSize, return + 1) LPWSTR lpBuffer,
    __in UINT uSize
    );
#ifdef UNICODE
#define GetSystemWindowsDirectory  GetSystemWindowsDirectoryW
#else
#define GetSystemWindowsDirectory  GetSystemWindowsDirectoryA
#endif // !UNICODE

#if !defined(RC_INVOKED) // RC warns because "WINBASE_DECLARE_GET_SYSTEM_WOW64_DIRECTORY" is a bit long.
#if _WIN32_WINNT >= 0x0501 || defined(WINBASE_DECLARE_GET_SYSTEM_WOW64_DIRECTORY)

WINBASEAPI
UINT
WINAPI
GetSystemWow64DirectoryA(
    __out_ecount_part_opt(uSize, return + 1) LPSTR lpBuffer,
    __in UINT uSize
    );
WINBASEAPI
UINT
WINAPI
GetSystemWow64DirectoryW(
    __out_ecount_part_opt(uSize, return + 1) LPWSTR lpBuffer,
    __in UINT uSize
    );
#ifdef UNICODE
#define GetSystemWow64Directory  GetSystemWow64DirectoryW
#else
#define GetSystemWow64Directory  GetSystemWow64DirectoryA
#endif // !UNICODE

WINBASEAPI
BOOLEAN
WINAPI
Wow64EnableWow64FsRedirection (
    __in BOOLEAN Wow64FsEnableRedirection
    );

WINBASEAPI
BOOL
WINAPI
Wow64DisableWow64FsRedirection (
    __out PVOID *OldValue
    );

WINBASEAPI
BOOL
WINAPI
Wow64RevertWow64FsRedirection (
    __in PVOID OlValue
    );


//
// for GetProcAddress
//
typedef UINT (WINAPI* PGET_SYSTEM_WOW64_DIRECTORY_A)(__out_ecount_part_opt(uSize, return + 1) LPSTR lpBuffer, __in UINT uSize);
typedef UINT (WINAPI* PGET_SYSTEM_WOW64_DIRECTORY_W)(__out_ecount_part_opt(uSize, return + 1) LPWSTR lpBuffer, __in UINT uSize);

//
// GetProcAddress only accepts GET_SYSTEM_WOW64_DIRECTORY_NAME_A_A,
// GET_SYSTEM_WOW64_DIRECTORY_NAME_W_A, GET_SYSTEM_WOW64_DIRECTORY_NAME_T_A.
// The others are if you want to use the strings in some other way.
//
#define GET_SYSTEM_WOW64_DIRECTORY_NAME_A_A      "GetSystemWow64DirectoryA"
#define GET_SYSTEM_WOW64_DIRECTORY_NAME_A_W     L"GetSystemWow64DirectoryA"
#define GET_SYSTEM_WOW64_DIRECTORY_NAME_A_T TEXT("GetSystemWow64DirectoryA")
#define GET_SYSTEM_WOW64_DIRECTORY_NAME_W_A      "GetSystemWow64DirectoryW"
#define GET_SYSTEM_WOW64_DIRECTORY_NAME_W_W     L"GetSystemWow64DirectoryW"
#define GET_SYSTEM_WOW64_DIRECTORY_NAME_W_T TEXT("GetSystemWow64DirectoryW")

#ifdef UNICODE
#define GET_SYSTEM_WOW64_DIRECTORY_NAME_T_A GET_SYSTEM_WOW64_DIRECTORY_NAME_W_A
#define GET_SYSTEM_WOW64_DIRECTORY_NAME_T_W GET_SYSTEM_WOW64_DIRECTORY_NAME_W_W
#define GET_SYSTEM_WOW64_DIRECTORY_NAME_T_T GET_SYSTEM_WOW64_DIRECTORY_NAME_W_T
#else
#define GET_SYSTEM_WOW64_DIRECTORY_NAME_T_A GET_SYSTEM_WOW64_DIRECTORY_NAME_A_A
#define GET_SYSTEM_WOW64_DIRECTORY_NAME_T_W GET_SYSTEM_WOW64_DIRECTORY_NAME_A_W
#define GET_SYSTEM_WOW64_DIRECTORY_NAME_T_T GET_SYSTEM_WOW64_DIRECTORY_NAME_A_T
#endif

#endif // _WIN32_WINNT >= 0x0501
#endif

WINBASEAPI
BOOL
WINAPI
SetCurrentDirectoryA(
    __in LPCSTR lpPathName
    );
WINBASEAPI
BOOL
WINAPI
SetCurrentDirectoryW(
    __in LPCWSTR lpPathName
    );
#ifdef UNICODE
#define SetCurrentDirectory  SetCurrentDirectoryW
#else
#define SetCurrentDirectory  SetCurrentDirectoryA
#endif // !UNICODE

#if defined(_M_CEE)
#undef SetCurrentDirectory
__inline
BOOL
SetCurrentDirectory(
    LPCTSTR lpPathName
    )
{
#ifdef UNICODE
    return SetCurrentDirectoryW(
#else
    return SetCurrentDirectoryA(
#endif
        lpPathName
        );
}
#endif  /* _M_CEE */

WINBASEAPI
DWORD
WINAPI
GetCurrentDirectoryA(
    __in DWORD nBufferLength,
    __out_ecount_part_opt(nBufferLength, return + 1) LPSTR lpBuffer
    );
WINBASEAPI
DWORD
WINAPI
GetCurrentDirectoryW(
    __in DWORD nBufferLength,
    __out_ecount_part_opt(nBufferLength, return + 1) LPWSTR lpBuffer
    );
#ifdef UNICODE
#define GetCurrentDirectory  GetCurrentDirectoryW
#else
#define GetCurrentDirectory  GetCurrentDirectoryA
#endif // !UNICODE

#if defined(_M_CEE)
#undef GetCurrentDirectory
__inline
DWORD
GetCurrentDirectory(
    DWORD nBufferLength,
    LPTSTR lpBuffer
    )
{
#ifdef UNICODE
    return GetCurrentDirectoryW(
#else
    return GetCurrentDirectoryA(
#endif
        nBufferLength,
        lpBuffer
        );
}
#endif  /* _M_CEE */

#if _WIN32_WINNT >= 0x0502

WINBASEAPI
BOOL
WINAPI
SetDllDirectoryA(
    __in_opt LPCSTR lpPathName
    );
WINBASEAPI
BOOL
WINAPI
SetDllDirectoryW(
    __in_opt LPCWSTR lpPathName
    );
#ifdef UNICODE
#define SetDllDirectory  SetDllDirectoryW
#else
#define SetDllDirectory  SetDllDirectoryA
#endif // !UNICODE

WINBASEAPI
DWORD
WINAPI
GetDllDirectoryA(
    __in DWORD nBufferLength,
    __out_ecount_part_opt(nBufferLength, return + 1) LPSTR lpBuffer
    );
WINBASEAPI
DWORD
WINAPI
GetDllDirectoryW(
    __in DWORD nBufferLength,
    __out_ecount_part_opt(nBufferLength, return + 1) LPWSTR lpBuffer
    );
#ifdef UNICODE
#define GetDllDirectory  GetDllDirectoryW
#else
#define GetDllDirectory  GetDllDirectoryA
#endif // !UNICODE

#endif // _WIN32_WINNT >= 0x0502

#define BASE_SEARCH_PATH_ENABLE_SAFE_SEARCHMODE 0x1
#define BASE_SEARCH_PATH_DISABLE_SAFE_SEARCHMODE 0x10000
#define BASE_SEARCH_PATH_PERMANENT 0x8000
#define BASE_SEARCH_PATH_INVALID_FLAGS ~0x18001

WINBASEAPI
BOOL
WINAPI
SetSearchPathMode (
    __in DWORD Flags
    );

WINBASEAPI
BOOL
WINAPI
GetDiskFreeSpaceA(
    __in_opt  LPCSTR lpRootPathName,
    __out_opt LPDWORD lpSectorsPerCluster,
    __out_opt LPDWORD lpBytesPerSector,
    __out_opt LPDWORD lpNumberOfFreeClusters,
    __out_opt LPDWORD lpTotalNumberOfClusters
    );
WINBASEAPI
BOOL
WINAPI
GetDiskFreeSpaceW(
    __in_opt  LPCWSTR lpRootPathName,
    __out_opt LPDWORD lpSectorsPerCluster,
    __out_opt LPDWORD lpBytesPerSector,
    __out_opt LPDWORD lpNumberOfFreeClusters,
    __out_opt LPDWORD lpTotalNumberOfClusters
    );
#ifdef UNICODE
#define GetDiskFreeSpace  GetDiskFreeSpaceW
#else
#define GetDiskFreeSpace  GetDiskFreeSpaceA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
GetDiskFreeSpaceExA(
    __in_opt  LPCSTR lpDirectoryName,
    __out_opt PULARGE_INTEGER lpFreeBytesAvailableToCaller,
    __out_opt PULARGE_INTEGER lpTotalNumberOfBytes,
    __out_opt PULARGE_INTEGER lpTotalNumberOfFreeBytes
    );
WINBASEAPI
BOOL
WINAPI
GetDiskFreeSpaceExW(
    __in_opt  LPCWSTR lpDirectoryName,
    __out_opt PULARGE_INTEGER lpFreeBytesAvailableToCaller,
    __out_opt PULARGE_INTEGER lpTotalNumberOfBytes,
    __out_opt PULARGE_INTEGER lpTotalNumberOfFreeBytes
    );
#ifdef UNICODE
#define GetDiskFreeSpaceEx  GetDiskFreeSpaceExW
#else
#define GetDiskFreeSpaceEx  GetDiskFreeSpaceExA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
CreateDirectoryA(
    __in     LPCSTR lpPathName,
    __in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes
    );
WINBASEAPI
BOOL
WINAPI
CreateDirectoryW(
    __in     LPCWSTR lpPathName,
    __in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes
    );
#ifdef UNICODE
#define CreateDirectory  CreateDirectoryW
#else
#define CreateDirectory  CreateDirectoryA
#endif // !UNICODE

#if defined(_M_CEE)
#undef CreateDirectory
__inline
BOOL
CreateDirectory(
    LPCTSTR lpPathName,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes
    )
{
#ifdef UNICODE
    return CreateDirectoryW(
#else
    return CreateDirectoryA(
#endif
        lpPathName,
        lpSecurityAttributes
        );
}
#endif  /* _M_CEE */

WINBASEAPI
BOOL
WINAPI
CreateDirectoryExA(
    __in     LPCSTR lpTemplateDirectory,
    __in     LPCSTR lpNewDirectory,
    __in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes
    );
WINBASEAPI
BOOL
WINAPI
CreateDirectoryExW(
    __in     LPCWSTR lpTemplateDirectory,
    __in     LPCWSTR lpNewDirectory,
    __in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes
    );
#ifdef UNICODE
#define CreateDirectoryEx  CreateDirectoryExW
#else
#define CreateDirectoryEx  CreateDirectoryExA
#endif // !UNICODE

#if _WIN32_WINNT >= 0x0600

WINBASEAPI
BOOL
WINAPI
CreateDirectoryTransactedA(
    __in_opt LPCSTR lpTemplateDirectory,
    __in     LPCSTR lpNewDirectory,
    __in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    __in     HANDLE hTransaction
    );
WINBASEAPI
BOOL
WINAPI
CreateDirectoryTransactedW(
    __in_opt LPCWSTR lpTemplateDirectory,
    __in     LPCWSTR lpNewDirectory,
    __in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    __in     HANDLE hTransaction
    );
#ifdef UNICODE
#define CreateDirectoryTransacted  CreateDirectoryTransactedW
#else
#define CreateDirectoryTransacted  CreateDirectoryTransactedA
#endif // !UNICODE

#endif // _WIN32_WINNT >= 0x0600

WINBASEAPI
BOOL
WINAPI
RemoveDirectoryA(
    __in LPCSTR lpPathName
    );
WINBASEAPI
BOOL
WINAPI
RemoveDirectoryW(
    __in LPCWSTR lpPathName
    );
#ifdef UNICODE
#define RemoveDirectory  RemoveDirectoryW
#else
#define RemoveDirectory  RemoveDirectoryA
#endif // !UNICODE

#if _WIN32_WINNT >= 0x0600

WINBASEAPI
BOOL
WINAPI
RemoveDirectoryTransactedA(
    __in LPCSTR lpPathName,
    __in     HANDLE hTransaction
    );
WINBASEAPI
BOOL
WINAPI
RemoveDirectoryTransactedW(
    __in LPCWSTR lpPathName,
    __in     HANDLE hTransaction
    );
#ifdef UNICODE
#define RemoveDirectoryTransacted  RemoveDirectoryTransactedW
#else
#define RemoveDirectoryTransacted  RemoveDirectoryTransactedA
#endif // !UNICODE

#endif // _WIN32_WINNT >= 0x0600

WINBASEAPI
DWORD
WINAPI
GetFullPathNameA(
    __in            LPCSTR lpFileName,
    __in            DWORD nBufferLength,
    __out_ecount_part_opt(nBufferLength, return + 1) LPSTR lpBuffer,
    __deref_opt_out LPSTR *lpFilePart
    );
WINBASEAPI
DWORD
WINAPI
GetFullPathNameW(
    __in            LPCWSTR lpFileName,
    __in            DWORD nBufferLength,
    __out_ecount_part_opt(nBufferLength, return + 1) LPWSTR lpBuffer,
    __deref_opt_out LPWSTR *lpFilePart
    );
#ifdef UNICODE
#define GetFullPathName  GetFullPathNameW
#else
#define GetFullPathName  GetFullPathNameA
#endif // !UNICODE
#if _WIN32_WINNT >= 0x0600

WINBASEAPI
DWORD
WINAPI
GetFullPathNameTransactedA(
    __in            LPCSTR lpFileName,
    __in            DWORD nBufferLength,
    __out_ecount_part_opt(nBufferLength, return + 1) LPSTR lpBuffer,
    __deref_opt_out LPSTR *lpFilePart,
    __in            HANDLE hTransaction
    );
WINBASEAPI
DWORD
WINAPI
GetFullPathNameTransactedW(
    __in            LPCWSTR lpFileName,
    __in            DWORD nBufferLength,
    __out_ecount_part_opt(nBufferLength, return + 1) LPWSTR lpBuffer,
    __deref_opt_out LPWSTR *lpFilePart,
    __in            HANDLE hTransaction
    );
#ifdef UNICODE
#define GetFullPathNameTransacted  GetFullPathNameTransactedW
#else
#define GetFullPathNameTransacted  GetFullPathNameTransactedA
#endif // !UNICODE

#endif // _WIN32_WINNT >= 0x0600

#define DDD_RAW_TARGET_PATH         0x00000001
#define DDD_REMOVE_DEFINITION       0x00000002
#define DDD_EXACT_MATCH_ON_REMOVE   0x00000004
#define DDD_NO_BROADCAST_SYSTEM     0x00000008
#define DDD_LUID_BROADCAST_DRIVE    0x00000010

WINBASEAPI
BOOL
WINAPI
DefineDosDeviceA(
    __in     DWORD dwFlags,
    __in     LPCSTR lpDeviceName,
    __in_opt LPCSTR lpTargetPath
    );
WINBASEAPI
BOOL
WINAPI
DefineDosDeviceW(
    __in     DWORD dwFlags,
    __in     LPCWSTR lpDeviceName,
    __in_opt LPCWSTR lpTargetPath
    );
#ifdef UNICODE
#define DefineDosDevice  DefineDosDeviceW
#else
#define DefineDosDevice  DefineDosDeviceA
#endif // !UNICODE

WINBASEAPI
DWORD
WINAPI
QueryDosDeviceA(
    __in_opt LPCSTR lpDeviceName,
    __out_ecount_part_opt(ucchMax, return) LPSTR lpTargetPath,
    __in     DWORD ucchMax
    );
WINBASEAPI
DWORD
WINAPI
QueryDosDeviceW(
    __in_opt LPCWSTR lpDeviceName,
    __out_ecount_part_opt(ucchMax, return) LPWSTR lpTargetPath,
    __in     DWORD ucchMax
    );
#ifdef UNICODE
#define QueryDosDevice  QueryDosDeviceW
#else
#define QueryDosDevice  QueryDosDeviceA
#endif // !UNICODE

#define EXPAND_LOCAL_DRIVES

WINBASEAPI
__out
HANDLE
WINAPI
CreateFileA(
    __in     LPCSTR lpFileName,
    __in     DWORD dwDesiredAccess,
    __in     DWORD dwShareMode,
    __in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    __in     DWORD dwCreationDisposition,
    __in     DWORD dwFlagsAndAttributes,
    __in_opt HANDLE hTemplateFile
    );
WINBASEAPI
__out
HANDLE
WINAPI
CreateFileW(
    __in     LPCWSTR lpFileName,
    __in     DWORD dwDesiredAccess,
    __in     DWORD dwShareMode,
    __in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    __in     DWORD dwCreationDisposition,
    __in     DWORD dwFlagsAndAttributes,
    __in_opt HANDLE hTemplateFile
    );
#ifdef UNICODE
#define CreateFile  CreateFileW
#else
#define CreateFile  CreateFileA
#endif // !UNICODE

#if _WIN32_WINNT >= 0x0600

WINBASEAPI
__out
HANDLE
WINAPI
CreateFileTransactedA(
    __in       LPCSTR lpFileName,
    __in       DWORD dwDesiredAccess,
    __in       DWORD dwShareMode,
    __in_opt   LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    __in       DWORD dwCreationDisposition,
    __in       DWORD dwFlagsAndAttributes,
    __in_opt   HANDLE hTemplateFile,
    __in       HANDLE hTransaction,
    __in_opt   PUSHORT pusMiniVersion,
    __reserved PVOID  lpExtendedParameter
    );
WINBASEAPI
__out
HANDLE
WINAPI
CreateFileTransactedW(
    __in       LPCWSTR lpFileName,
    __in       DWORD dwDesiredAccess,
    __in       DWORD dwShareMode,
    __in_opt   LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    __in       DWORD dwCreationDisposition,
    __in       DWORD dwFlagsAndAttributes,
    __in_opt   HANDLE hTemplateFile,
    __in       HANDLE hTransaction,
    __in_opt   PUSHORT pusMiniVersion,
    __reserved PVOID  lpExtendedParameter
    );
#ifdef UNICODE
#define CreateFileTransacted  CreateFileTransactedW
#else
#define CreateFileTransacted  CreateFileTransactedA
#endif // !UNICODE

#endif // _WIN32_WINNT >= 0x0600


#if _WIN32_WINNT >= 0x0502

WINBASEAPI
__out
HANDLE
WINAPI
ReOpenFile(
    __in HANDLE  hOriginalFile,
    __in DWORD   dwDesiredAccess,
    __in DWORD   dwShareMode,
    __in DWORD   dwFlagsAndAttributes
    );

#endif // _WIN32_WINNT >= 0x0502

WINBASEAPI
BOOL
WINAPI
SetFileAttributesA(
    __in LPCSTR lpFileName,
    __in DWORD dwFileAttributes
    );
WINBASEAPI
BOOL
WINAPI
SetFileAttributesW(
    __in LPCWSTR lpFileName,
    __in DWORD dwFileAttributes
    );
#ifdef UNICODE
#define SetFileAttributes  SetFileAttributesW
#else
#define SetFileAttributes  SetFileAttributesA
#endif // !UNICODE

WINBASEAPI
DWORD
WINAPI
GetFileAttributesA(
    __in LPCSTR lpFileName
    );
WINBASEAPI
DWORD
WINAPI
GetFileAttributesW(
    __in LPCWSTR lpFileName
    );
#ifdef UNICODE
#define GetFileAttributes  GetFileAttributesW
#else
#define GetFileAttributes  GetFileAttributesA
#endif // !UNICODE

#if _WIN32_WINNT >= 0x0600

WINBASEAPI
BOOL
WINAPI
SetFileAttributesTransactedA(
    __in     LPCSTR lpFileName,
    __in     DWORD dwFileAttributes,
    __in     HANDLE hTransaction
    );
WINBASEAPI
BOOL
WINAPI
SetFileAttributesTransactedW(
    __in     LPCWSTR lpFileName,
    __in     DWORD dwFileAttributes,
    __in     HANDLE hTransaction
    );
#ifdef UNICODE
#define SetFileAttributesTransacted  SetFileAttributesTransactedW
#else
#define SetFileAttributesTransacted  SetFileAttributesTransactedA
#endif // !UNICODE

#endif // _WIN32_WINNT >= 0x0600

typedef enum _GET_FILEEX_INFO_LEVELS {
    GetFileExInfoStandard,
    GetFileExMaxInfoLevel
} GET_FILEEX_INFO_LEVELS;

#if _WIN32_WINNT >= 0x0600

WINBASEAPI
BOOL
WINAPI
GetFileAttributesTransactedA(
    __in  LPCSTR lpFileName,
    __in  GET_FILEEX_INFO_LEVELS fInfoLevelId,
    __out LPVOID lpFileInformation,
    __in     HANDLE hTransaction
    );
WINBASEAPI
BOOL
WINAPI
GetFileAttributesTransactedW(
    __in  LPCWSTR lpFileName,
    __in  GET_FILEEX_INFO_LEVELS fInfoLevelId,
    __out LPVOID lpFileInformation,
    __in     HANDLE hTransaction
    );
#ifdef UNICODE
#define GetFileAttributesTransacted  GetFileAttributesTransactedW
#else
#define GetFileAttributesTransacted  GetFileAttributesTransactedA
#endif // !UNICODE

#endif // _WIN32_WINNT >= 0x0600

WINBASEAPI
BOOL
WINAPI
GetFileAttributesExA(
    __in  LPCSTR lpFileName,
    __in  GET_FILEEX_INFO_LEVELS fInfoLevelId,
    __out LPVOID lpFileInformation
    );
WINBASEAPI
BOOL
WINAPI
GetFileAttributesExW(
    __in  LPCWSTR lpFileName,
    __in  GET_FILEEX_INFO_LEVELS fInfoLevelId,
    __out LPVOID lpFileInformation
    );
#ifdef UNICODE
#define GetFileAttributesEx  GetFileAttributesExW
#else
#define GetFileAttributesEx  GetFileAttributesExA
#endif // !UNICODE

WINBASEAPI
DWORD
WINAPI
GetCompressedFileSizeA(
    __in  LPCSTR lpFileName,
    __out_opt LPDWORD  lpFileSizeHigh
    );
WINBASEAPI
DWORD
WINAPI
GetCompressedFileSizeW(
    __in  LPCWSTR lpFileName,
    __out_opt LPDWORD  lpFileSizeHigh
    );
#ifdef UNICODE
#define GetCompressedFileSize  GetCompressedFileSizeW
#else
#define GetCompressedFileSize  GetCompressedFileSizeA
#endif // !UNICODE

#if _WIN32_WINNT >= 0x0600

WINBASEAPI
DWORD
WINAPI
GetCompressedFileSizeTransactedA(
    __in      LPCSTR lpFileName,
    __out_opt LPDWORD  lpFileSizeHigh,
    __in      HANDLE hTransaction
    );
WINBASEAPI
DWORD
WINAPI
GetCompressedFileSizeTransactedW(
    __in      LPCWSTR lpFileName,
    __out_opt LPDWORD  lpFileSizeHigh,
    __in      HANDLE hTransaction
    );
#ifdef UNICODE
#define GetCompressedFileSizeTransacted  GetCompressedFileSizeTransactedW
#else
#define GetCompressedFileSizeTransacted  GetCompressedFileSizeTransactedA
#endif // !UNICODE

#endif // _WIN32_WINNT >= 0x0600

WINBASEAPI
BOOL
WINAPI
DeleteFileA(
    __in LPCSTR lpFileName
    );
WINBASEAPI
BOOL
WINAPI
DeleteFileW(
    __in LPCWSTR lpFileName
    );
#ifdef UNICODE
#define DeleteFile  DeleteFileW
#else
#define DeleteFile  DeleteFileA
#endif // !UNICODE

#if _WIN32_WINNT >= 0x0600

WINBASEAPI
BOOL
WINAPI
DeleteFileTransactedA(
    __in     LPCSTR lpFileName,
    __in     HANDLE hTransaction
    );
WINBASEAPI
BOOL
WINAPI
DeleteFileTransactedW(
    __in     LPCWSTR lpFileName,
    __in     HANDLE hTransaction
    );
#ifdef UNICODE
#define DeleteFileTransacted  DeleteFileTransactedW
#else
#define DeleteFileTransacted  DeleteFileTransactedA
#endif // !UNICODE

#endif // _WIN32_WINNT >= 0x0600

#if defined(_M_CEE)
#undef DeleteFile
__inline
BOOL
DeleteFile(
    LPCTSTR lpFileName
    )
{
#ifdef UNICODE
    return DeleteFileW(
#else
    return DeleteFileA(
#endif
        lpFileName
        );
}
#endif  /* _M_CEE */


#if _WIN32_WINNT >= 0x0501

WINBASEAPI
BOOL
WINAPI
CheckNameLegalDOS8Dot3A(
    __in      LPCSTR lpName,
    __out_ecount_opt(OemNameSize) LPSTR lpOemName,
    __in      DWORD OemNameSize,
    __out_opt PBOOL pbNameContainsSpaces OPTIONAL,
    __out     PBOOL pbNameLegal
    );
WINBASEAPI
BOOL
WINAPI
CheckNameLegalDOS8Dot3W(
    __in      LPCWSTR lpName,
    __out_ecount_opt(OemNameSize) LPSTR lpOemName,
    __in      DWORD OemNameSize,
    __out_opt PBOOL pbNameContainsSpaces OPTIONAL,
    __out     PBOOL pbNameLegal
    );
#ifdef UNICODE
#define CheckNameLegalDOS8Dot3  CheckNameLegalDOS8Dot3W
#else
#define CheckNameLegalDOS8Dot3  CheckNameLegalDOS8Dot3A
#endif // !UNICODE

#endif // (_WIN32_WINNT >= 0x0501)

#if(_WIN32_WINNT >= 0x0400)
typedef enum _FINDEX_INFO_LEVELS {
    FindExInfoStandard,
    FindExInfoBasic,
    FindExInfoMaxInfoLevel
} FINDEX_INFO_LEVELS;

typedef enum _FINDEX_SEARCH_OPS {
    FindExSearchNameMatch,
    FindExSearchLimitToDirectories,
    FindExSearchLimitToDevices,
    FindExSearchMaxSearchOp
} FINDEX_SEARCH_OPS;

#define FIND_FIRST_EX_CASE_SENSITIVE   0x00000001
#define FIND_FIRST_EX_LARGE_FETCH      0x00000002

WINBASEAPI
__out
HANDLE
WINAPI
FindFirstFileExA(
    __in       LPCSTR lpFileName,
    __in       FINDEX_INFO_LEVELS fInfoLevelId,
    __out      LPVOID lpFindFileData,
    __in       FINDEX_SEARCH_OPS fSearchOp,
    __reserved LPVOID lpSearchFilter,
    __in       DWORD dwAdditionalFlags
    );
WINBASEAPI
__out
HANDLE
WINAPI
FindFirstFileExW(
    __in       LPCWSTR lpFileName,
    __in       FINDEX_INFO_LEVELS fInfoLevelId,
    __out      LPVOID lpFindFileData,
    __in       FINDEX_SEARCH_OPS fSearchOp,
    __reserved LPVOID lpSearchFilter,
    __in       DWORD dwAdditionalFlags
    );
#ifdef UNICODE
#define FindFirstFileEx  FindFirstFileExW
#else
#define FindFirstFileEx  FindFirstFileExA
#endif // !UNICODE

#if _WIN32_WINNT >= 0x0600

WINBASEAPI
__out
HANDLE
WINAPI
FindFirstFileTransactedA(
    __in       LPCSTR lpFileName,
    __in       FINDEX_INFO_LEVELS fInfoLevelId,
    __out      LPVOID lpFindFileData,
    __in       FINDEX_SEARCH_OPS fSearchOp,
    __reserved LPVOID lpSearchFilter,
    __in       DWORD dwAdditionalFlags,
    __in       HANDLE hTransaction
    );
WINBASEAPI
__out
HANDLE
WINAPI
FindFirstFileTransactedW(
    __in       LPCWSTR lpFileName,
    __in       FINDEX_INFO_LEVELS fInfoLevelId,
    __out      LPVOID lpFindFileData,
    __in       FINDEX_SEARCH_OPS fSearchOp,
    __reserved LPVOID lpSearchFilter,
    __in       DWORD dwAdditionalFlags,
    __in       HANDLE hTransaction
    );
#ifdef UNICODE
#define FindFirstFileTransacted  FindFirstFileTransactedW
#else
#define FindFirstFileTransacted  FindFirstFileTransactedA
#endif // !UNICODE

#endif

#endif /* _WIN32_WINNT >= 0x0400 */

WINBASEAPI
__out
HANDLE
WINAPI
FindFirstFileA(
    __in  LPCSTR lpFileName,
    __out LPWIN32_FIND_DATAA lpFindFileData
    );
WINBASEAPI
__out
HANDLE
WINAPI
FindFirstFileW(
    __in  LPCWSTR lpFileName,
    __out LPWIN32_FIND_DATAW lpFindFileData
    );
#ifdef UNICODE
#define FindFirstFile  FindFirstFileW
#else
#define FindFirstFile  FindFirstFileA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
FindNextFileA(
    __in  HANDLE hFindFile,
    __out LPWIN32_FIND_DATAA lpFindFileData
    );
WINBASEAPI
BOOL
WINAPI
FindNextFileW(
    __in  HANDLE hFindFile,
    __out LPWIN32_FIND_DATAW lpFindFileData
    );
#ifdef UNICODE
#define FindNextFile  FindNextFileW
#else
#define FindNextFile  FindNextFileA
#endif // !UNICODE


WINBASEAPI
DWORD
WINAPI
SearchPathA(
    __in_opt  LPCSTR lpPath,
    __in      LPCSTR lpFileName,
    __in_opt  LPCSTR lpExtension,
    __in      DWORD nBufferLength,
    __out_ecount_part_opt(nBufferLength, return + 1) LPSTR lpBuffer,
    __out_opt LPSTR *lpFilePart
    );
WINBASEAPI
DWORD
WINAPI
SearchPathW(
    __in_opt  LPCWSTR lpPath,
    __in      LPCWSTR lpFileName,
    __in_opt  LPCWSTR lpExtension,
    __in      DWORD nBufferLength,
    __out_ecount_part_opt(nBufferLength, return + 1) LPWSTR lpBuffer,
    __out_opt LPWSTR *lpFilePart
    );
#ifdef UNICODE
#define SearchPath  SearchPathW
#else
#define SearchPath  SearchPathA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
CopyFileA(
    __in LPCSTR lpExistingFileName,
    __in LPCSTR lpNewFileName,
    __in BOOL bFailIfExists
    );
WINBASEAPI
BOOL
WINAPI
CopyFileW(
    __in LPCWSTR lpExistingFileName,
    __in LPCWSTR lpNewFileName,
    __in BOOL bFailIfExists
    );
#ifdef UNICODE
#define CopyFile  CopyFileW
#else
#define CopyFile  CopyFileA
#endif // !UNICODE

#if defined(_M_CEE)
#undef CopyFile
__inline
BOOL
CopyFile(
    LPCTSTR lpExistingFileName,
    LPCTSTR lpNewFileName,
    BOOL bFailIfExists
    )
{
#ifdef UNICODE
    return CopyFileW(
#else
    return CopyFileA(
#endif
        lpExistingFileName,
        lpNewFileName,
        bFailIfExists
        );
}
#endif  /* _M_CEE */

#if(_WIN32_WINNT >= 0x0400)
typedef
DWORD
(WINAPI *LPPROGRESS_ROUTINE)(
    __in     LARGE_INTEGER TotalFileSize,
    __in     LARGE_INTEGER TotalBytesTransferred,
    __in     LARGE_INTEGER StreamSize,
    __in     LARGE_INTEGER StreamBytesTransferred,
    __in     DWORD dwStreamNumber,
    __in     DWORD dwCallbackReason,
    __in     HANDLE hSourceFile,
    __in     HANDLE hDestinationFile,
    __in_opt LPVOID lpData
    );

WINBASEAPI
BOOL
WINAPI
CopyFileExA(
    __in     LPCSTR lpExistingFileName,
    __in     LPCSTR lpNewFileName,
    __in_opt LPPROGRESS_ROUTINE lpProgressRoutine,
    __in_opt LPVOID lpData,
    __in_opt LPBOOL pbCancel,
    __in     DWORD dwCopyFlags
    );
WINBASEAPI
BOOL
WINAPI
CopyFileExW(
    __in     LPCWSTR lpExistingFileName,
    __in     LPCWSTR lpNewFileName,
    __in_opt LPPROGRESS_ROUTINE lpProgressRoutine,
    __in_opt LPVOID lpData,
    __in_opt LPBOOL pbCancel,
    __in     DWORD dwCopyFlags
    );
#ifdef UNICODE
#define CopyFileEx  CopyFileExW
#else
#define CopyFileEx  CopyFileExA
#endif // !UNICODE

#if _WIN32_WINNT >= 0x0600

WINBASEAPI
BOOL
WINAPI
CopyFileTransactedA(
    __in     LPCSTR lpExistingFileName,
    __in     LPCSTR lpNewFileName,
    __in_opt LPPROGRESS_ROUTINE lpProgressRoutine,
    __in_opt LPVOID lpData,
    __in_opt LPBOOL pbCancel,
    __in     DWORD dwCopyFlags,
    __in     HANDLE hTransaction
    );
WINBASEAPI
BOOL
WINAPI
CopyFileTransactedW(
    __in     LPCWSTR lpExistingFileName,
    __in     LPCWSTR lpNewFileName,
    __in_opt LPPROGRESS_ROUTINE lpProgressRoutine,
    __in_opt LPVOID lpData,
    __in_opt LPBOOL pbCancel,
    __in     DWORD dwCopyFlags,
    __in     HANDLE hTransaction
    );
#ifdef UNICODE
#define CopyFileTransacted  CopyFileTransactedW
#else
#define CopyFileTransacted  CopyFileTransactedA
#endif // !UNICODE

#endif // _WIN32_WINNT >= 0x0600
#endif /* _WIN32_WINNT >= 0x0400 */

WINBASEAPI
BOOL
WINAPI
MoveFileA(
    __in LPCSTR lpExistingFileName,
    __in LPCSTR lpNewFileName
    );
WINBASEAPI
BOOL
WINAPI
MoveFileW(
    __in LPCWSTR lpExistingFileName,
    __in LPCWSTR lpNewFileName
    );
#ifdef UNICODE
#define MoveFile  MoveFileW
#else
#define MoveFile  MoveFileA
#endif // !UNICODE

#if defined(_M_CEE)
#undef MoveFile
__inline
BOOL
MoveFile(
    LPCTSTR lpExistingFileName,
    LPCTSTR lpNewFileName
    )
{
#ifdef UNICODE
    return MoveFileW(
#else
    return MoveFileA(
#endif
        lpExistingFileName,
        lpNewFileName
        );
}
#endif  /* _M_CEE */

WINBASEAPI
BOOL
WINAPI
MoveFileExA(
    __in     LPCSTR lpExistingFileName,
    __in_opt LPCSTR lpNewFileName,
    __in     DWORD    dwFlags
    );
WINBASEAPI
BOOL
WINAPI
MoveFileExW(
    __in     LPCWSTR lpExistingFileName,
    __in_opt LPCWSTR lpNewFileName,
    __in     DWORD    dwFlags
    );
#ifdef UNICODE
#define MoveFileEx  MoveFileExW
#else
#define MoveFileEx  MoveFileExA
#endif // !UNICODE

#if (_WIN32_WINNT >= 0x0500)
WINBASEAPI
BOOL
WINAPI
MoveFileWithProgressA(
    __in     LPCSTR lpExistingFileName,
    __in_opt LPCSTR lpNewFileName,
    __in_opt LPPROGRESS_ROUTINE lpProgressRoutine,
    __in_opt LPVOID lpData,
    __in     DWORD dwFlags
    );
WINBASEAPI
BOOL
WINAPI
MoveFileWithProgressW(
    __in     LPCWSTR lpExistingFileName,
    __in_opt LPCWSTR lpNewFileName,
    __in_opt LPPROGRESS_ROUTINE lpProgressRoutine,
    __in_opt LPVOID lpData,
    __in     DWORD dwFlags
    );
#ifdef UNICODE
#define MoveFileWithProgress  MoveFileWithProgressW
#else
#define MoveFileWithProgress  MoveFileWithProgressA
#endif // !UNICODE
#endif // (_WIN32_WINNT >= 0x0500)

#if (_WIN32_WINNT >= 0x0600)
WINBASEAPI
BOOL
WINAPI
MoveFileTransactedA(
    __in     LPCSTR lpExistingFileName,
    __in_opt LPCSTR lpNewFileName,
    __in_opt LPPROGRESS_ROUTINE lpProgressRoutine,
    __in_opt LPVOID lpData,
    __in     DWORD dwFlags,
    __in     HANDLE hTransaction
    );
WINBASEAPI
BOOL
WINAPI
MoveFileTransactedW(
    __in     LPCWSTR lpExistingFileName,
    __in_opt LPCWSTR lpNewFileName,
    __in_opt LPPROGRESS_ROUTINE lpProgressRoutine,
    __in_opt LPVOID lpData,
    __in     DWORD dwFlags,
    __in     HANDLE hTransaction
    );
#ifdef UNICODE
#define MoveFileTransacted  MoveFileTransactedW
#else
#define MoveFileTransacted  MoveFileTransactedA
#endif // !UNICODE
#endif // (_WIN32_WINNT >= 0x0600)

#define MOVEFILE_REPLACE_EXISTING       0x00000001
#define MOVEFILE_COPY_ALLOWED           0x00000002
#define MOVEFILE_DELAY_UNTIL_REBOOT     0x00000004
#define MOVEFILE_WRITE_THROUGH          0x00000008
#if (_WIN32_WINNT >= 0x0500)
#define MOVEFILE_CREATE_HARDLINK        0x00000010
#define MOVEFILE_FAIL_IF_NOT_TRACKABLE  0x00000020
#endif // (_WIN32_WINNT >= 0x0500)



#if (_WIN32_WINNT >= 0x0500)

WINBASEAPI
BOOL
WINAPI
ReplaceFileA(
    __in       LPCSTR lpReplacedFileName,
    __in       LPCSTR lpReplacementFileName,
    __in_opt   LPCSTR lpBackupFileName,
    __in       DWORD    dwReplaceFlags,
    __reserved LPVOID   lpExclude,
    __reserved LPVOID  lpReserved
    );
WINBASEAPI
BOOL
WINAPI
ReplaceFileW(
    __in       LPCWSTR lpReplacedFileName,
    __in       LPCWSTR lpReplacementFileName,
    __in_opt   LPCWSTR lpBackupFileName,
    __in       DWORD    dwReplaceFlags,
    __reserved LPVOID   lpExclude,
    __reserved LPVOID  lpReserved
    );
#ifdef UNICODE
#define ReplaceFile  ReplaceFileW
#else
#define ReplaceFile  ReplaceFileA
#endif // !UNICODE
#endif // (_WIN32_WINNT >= 0x0500)


#if (_WIN32_WINNT >= 0x0500)
//
// API call to create hard links.
//

WINBASEAPI
BOOL
WINAPI
CreateHardLinkA(
    __in       LPCSTR lpFileName,
    __in       LPCSTR lpExistingFileName,
    __reserved LPSECURITY_ATTRIBUTES lpSecurityAttributes
    );
WINBASEAPI
BOOL
WINAPI
CreateHardLinkW(
    __in       LPCWSTR lpFileName,
    __in       LPCWSTR lpExistingFileName,
    __reserved LPSECURITY_ATTRIBUTES lpSecurityAttributes
    );
#ifdef UNICODE
#define CreateHardLink  CreateHardLinkW
#else
#define CreateHardLink  CreateHardLinkA
#endif // !UNICODE

#endif // (_WIN32_WINNT >= 0x0500)

#if (_WIN32_WINNT >= 0x0600)
//
// API call to create hard links.
//

WINBASEAPI
BOOL
WINAPI
CreateHardLinkTransactedA(
    __in       LPCSTR lpFileName,
    __in       LPCSTR lpExistingFileName,
    __reserved LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    __in       HANDLE hTransaction
    );
WINBASEAPI
BOOL
WINAPI
CreateHardLinkTransactedW(
    __in       LPCWSTR lpFileName,
    __in       LPCWSTR lpExistingFileName,
    __reserved LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    __in       HANDLE hTransaction
    );
#ifdef UNICODE
#define CreateHardLinkTransacted  CreateHardLinkTransactedW
#else
#define CreateHardLinkTransacted  CreateHardLinkTransactedA
#endif // !UNICODE

#endif // (_WIN32_WINNT >= 0x0600)


#if (_WIN32_WINNT >= 0x0501)

//
// API call to enumerate for streams within a file
//

typedef enum _STREAM_INFO_LEVELS {

    FindStreamInfoStandard,
    FindStreamInfoMaxInfoLevel

} STREAM_INFO_LEVELS;

typedef struct _WIN32_FIND_STREAM_DATA {

    LARGE_INTEGER StreamSize;
    WCHAR cStreamName[ MAX_PATH + 36 ];

} WIN32_FIND_STREAM_DATA, *PWIN32_FIND_STREAM_DATA;

WINBASEAPI
__out
HANDLE
WINAPI
FindFirstStreamW(
    __in       LPCWSTR lpFileName,
    __in       STREAM_INFO_LEVELS InfoLevel,
    __out      LPVOID lpFindStreamData,
    __reserved DWORD dwFlags
    );

#if _WIN32_WINNT >= 0x0600
WINBASEAPI
HANDLE
WINAPI
FindFirstStreamTransactedW (
    __in       LPCWSTR lpFileName,
    __in       STREAM_INFO_LEVELS InfoLevel,
    __out      LPVOID lpFindStreamData,
    __reserved DWORD dwFlags,
    __in       HANDLE hTransaction
    );
#endif // _WIN32_WINNT >= 0x600

WINBASEAPI
BOOL
APIENTRY
FindNextStreamW(
    __in  HANDLE hFindStream,
    __out LPVOID lpFindStreamData
    );
#endif // (_WIN32_WINNT >= 0x0501)

#if _WIN32_WINNT >= 0x0600

WINBASEAPI
HANDLE
WINAPI
FindFirstFileNameW (
    __in    LPCWSTR lpFileName,
    __in    DWORD dwFlags,
    __inout LPDWORD StringLength,
    __inout_ecount(*StringLength) PWCHAR LinkName
    );

WINBASEAPI
BOOL
APIENTRY
FindNextFileNameW (
    __in    HANDLE hFindStream,
    __inout LPDWORD StringLength,
    __inout_ecount(*StringLength) PWCHAR LinkName
    );

WINBASEAPI
HANDLE
WINAPI
FindFirstFileNameTransactedW (
    __in     LPCWSTR lpFileName,
    __in     DWORD dwFlags,
    __inout  LPDWORD StringLength,
    __inout_ecount(*StringLength) PWCHAR LinkName,
    __in_opt HANDLE hTransaction
    );

#endif


WINBASEAPI
__out
HANDLE
WINAPI
CreateNamedPipeA(
    __in     LPCSTR lpName,
    __in     DWORD dwOpenMode,
    __in     DWORD dwPipeMode,
    __in     DWORD nMaxInstances,
    __in     DWORD nOutBufferSize,
    __in     DWORD nInBufferSize,
    __in     DWORD nDefaultTimeOut,
    __in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes
    );
WINBASEAPI
__out
HANDLE
WINAPI
CreateNamedPipeW(
    __in     LPCWSTR lpName,
    __in     DWORD dwOpenMode,
    __in     DWORD dwPipeMode,
    __in     DWORD nMaxInstances,
    __in     DWORD nOutBufferSize,
    __in     DWORD nInBufferSize,
    __in     DWORD nDefaultTimeOut,
    __in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes
    );
#ifdef UNICODE
#define CreateNamedPipe  CreateNamedPipeW
#else
#define CreateNamedPipe  CreateNamedPipeA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
GetNamedPipeHandleStateA(
    __in      HANDLE hNamedPipe,
    __out_opt LPDWORD lpState,
    __out_opt LPDWORD lpCurInstances,
    __out_opt LPDWORD lpMaxCollectionCount,
    __out_opt LPDWORD lpCollectDataTimeout,
    __out_ecount_opt(nMaxUserNameSize) LPSTR lpUserName,
    __in      DWORD nMaxUserNameSize
    );
WINBASEAPI
BOOL
WINAPI
GetNamedPipeHandleStateW(
    __in      HANDLE hNamedPipe,
    __out_opt LPDWORD lpState,
    __out_opt LPDWORD lpCurInstances,
    __out_opt LPDWORD lpMaxCollectionCount,
    __out_opt LPDWORD lpCollectDataTimeout,
    __out_ecount_opt(nMaxUserNameSize) LPWSTR lpUserName,
    __in      DWORD nMaxUserNameSize
    );
#ifdef UNICODE
#define GetNamedPipeHandleState  GetNamedPipeHandleStateW
#else
#define GetNamedPipeHandleState  GetNamedPipeHandleStateA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
CallNamedPipeA(
    __in  LPCSTR lpNamedPipeName,
    __in_bcount_opt(nInBufferSize) LPVOID lpInBuffer,
    __in  DWORD nInBufferSize,
    __out_bcount_part_opt(nOutBufferSize, *lpBytesRead) LPVOID lpOutBuffer,
    __in  DWORD nOutBufferSize,
    __out LPDWORD lpBytesRead,
    __in  DWORD nTimeOut
    );
WINBASEAPI
BOOL
WINAPI
CallNamedPipeW(
    __in  LPCWSTR lpNamedPipeName,
    __in_bcount_opt(nInBufferSize) LPVOID lpInBuffer,
    __in  DWORD nInBufferSize,
    __out_bcount_part_opt(nOutBufferSize, *lpBytesRead) LPVOID lpOutBuffer,
    __in  DWORD nOutBufferSize,
    __out LPDWORD lpBytesRead,
    __in  DWORD nTimeOut
    );
#ifdef UNICODE
#define CallNamedPipe  CallNamedPipeW
#else
#define CallNamedPipe  CallNamedPipeA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
WaitNamedPipeA(
    __in LPCSTR lpNamedPipeName,
    __in DWORD nTimeOut
    );
WINBASEAPI
BOOL
WINAPI
WaitNamedPipeW(
    __in LPCWSTR lpNamedPipeName,
    __in DWORD nTimeOut
    );
#ifdef UNICODE
#define WaitNamedPipe  WaitNamedPipeW
#else
#define WaitNamedPipe  WaitNamedPipeA
#endif // !UNICODE

typedef enum {
    PipeAttribute,
    PipeConnectionAttribute,
    PipeHandleAttribute
} PIPE_ATTRIBUTE_TYPE;

WINBASEAPI
BOOL
WINAPI
GetNamedPipeAttribute(
    __in HANDLE Pipe,
    __in PIPE_ATTRIBUTE_TYPE AttributeType,
    __in PSTR AttributeName,
    __out_bcount(*AttributeValueLength) PVOID AttributeValue,
    __inout PSIZE_T AttributeValueLength
    );

WINBASEAPI
BOOL
WINAPI
SetNamedPipeAttribute(
    __in HANDLE Pipe,
    __in PIPE_ATTRIBUTE_TYPE AttributeType,
    __in PSTR AttributeName,
    __in PVOID AttributeValue,
    __in SIZE_T AttributeValueLength
    );

WINBASEAPI
BOOL
WINAPI
GetNamedPipeClientComputerNameA(
    __in HANDLE Pipe,
    __out_bcount(ClientComputerNameLength)  LPSTR ClientComputerName,
    __in ULONG ClientComputerNameLength
    );
WINBASEAPI
BOOL
WINAPI
GetNamedPipeClientComputerNameW(
    __in HANDLE Pipe,
    __out_bcount(ClientComputerNameLength)  LPWSTR ClientComputerName,
    __in ULONG ClientComputerNameLength
    );
#ifdef UNICODE
#define GetNamedPipeClientComputerName  GetNamedPipeClientComputerNameW
#else
#define GetNamedPipeClientComputerName  GetNamedPipeClientComputerNameA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
GetNamedPipeClientProcessId(
    __in HANDLE Pipe,
    __out PULONG ClientProcessId
    );

WINBASEAPI
BOOL
WINAPI
GetNamedPipeClientSessionId(
    __in HANDLE Pipe,
    __out PULONG ClientSessionId
    );

WINBASEAPI
BOOL
WINAPI
GetNamedPipeServerProcessId(
    __in HANDLE Pipe,
    __out PULONG ServerProcessId
    );

WINBASEAPI
BOOL
WINAPI
GetNamedPipeServerSessionId(
    __in HANDLE Pipe,
    __out PULONG ServerSessionId
    );

WINBASEAPI
BOOL
WINAPI
SetVolumeLabelA(
    __in_opt LPCSTR lpRootPathName,
    __in_opt LPCSTR lpVolumeName
    );
WINBASEAPI
BOOL
WINAPI
SetVolumeLabelW(
    __in_opt LPCWSTR lpRootPathName,
    __in_opt LPCWSTR lpVolumeName
    );
#ifdef UNICODE
#define SetVolumeLabel  SetVolumeLabelW
#else
#define SetVolumeLabel  SetVolumeLabelA
#endif // !UNICODE

WINBASEAPI
VOID
WINAPI
SetFileApisToOEM( VOID );

WINBASEAPI
VOID
WINAPI
SetFileApisToANSI( VOID );

WINBASEAPI
BOOL
WINAPI
AreFileApisANSI( VOID );

WINBASEAPI
BOOL
WINAPI
GetVolumeInformationA(
    __in_opt  LPCSTR lpRootPathName,
    __out_ecount_opt(nVolumeNameSize) LPSTR lpVolumeNameBuffer,
    __in      DWORD nVolumeNameSize,
    __out_opt LPDWORD lpVolumeSerialNumber,
    __out_opt LPDWORD lpMaximumComponentLength,
    __out_opt LPDWORD lpFileSystemFlags,
    __out_ecount_opt(nFileSystemNameSize) LPSTR lpFileSystemNameBuffer,
    __in      DWORD nFileSystemNameSize
    );
WINBASEAPI
BOOL
WINAPI
GetVolumeInformationW(
    __in_opt  LPCWSTR lpRootPathName,
    __out_ecount_opt(nVolumeNameSize) LPWSTR lpVolumeNameBuffer,
    __in      DWORD nVolumeNameSize,
    __out_opt LPDWORD lpVolumeSerialNumber,
    __out_opt LPDWORD lpMaximumComponentLength,
    __out_opt LPDWORD lpFileSystemFlags,
    __out_ecount_opt(nFileSystemNameSize) LPWSTR lpFileSystemNameBuffer,
    __in      DWORD nFileSystemNameSize
    );
#ifdef UNICODE
#define GetVolumeInformation  GetVolumeInformationW
#else
#define GetVolumeInformation  GetVolumeInformationA
#endif // !UNICODE

#if(_WIN32_WINNT >= 0x0600)
WINBASEAPI
BOOL
WINAPI
GetVolumeInformationByHandleW(
    __in      HANDLE hFile,
    __out_ecount_opt(nVolumeNameSize) LPWSTR lpVolumeNameBuffer,
    __in      DWORD nVolumeNameSize,
    __out_opt LPDWORD lpVolumeSerialNumber,
    __out_opt LPDWORD lpMaximumComponentLength,
    __out_opt LPDWORD lpFileSystemFlags,
    __out_ecount_opt(nFileSystemNameSize) LPWSTR lpFileSystemNameBuffer,
    __in      DWORD nFileSystemNameSize
    );
#endif /* _WIN32_WINNT >=  0x0600 */

WINBASEAPI
BOOL
WINAPI
CancelSynchronousIo(
    __in HANDLE hThread
    );

WINBASEAPI
BOOL
WINAPI
CancelIoEx(
    __in HANDLE hFile,
    __in_opt LPOVERLAPPED lpOverlapped
    );

WINBASEAPI
BOOL
WINAPI
CancelIo(
    __in HANDLE hFile
    );

WINBASEAPI
BOOL
WINAPI
SetFileBandwidthReservation(
    __in  HANDLE  hFile,
    __in  DWORD   nPeriodMilliseconds,
    __in  DWORD   nBytesPerPeriod,
    __in  BOOL    bDiscardable,
    __out LPDWORD lpTransferSize,
    __out LPDWORD lpNumOutstandingRequests
    );

WINBASEAPI
BOOL
WINAPI
GetFileBandwidthReservation(
    __in  HANDLE  hFile,
    __out LPDWORD lpPeriodMilliseconds,
    __out LPDWORD lpBytesPerPeriod,
    __out LPBOOL  pDiscardable,
    __out LPDWORD lpTransferSize,
    __out LPDWORD lpNumOutstandingRequests
    );

//
// Event logging APIs
//

WINADVAPI
BOOL
WINAPI
ClearEventLogA (
    __in     HANDLE hEventLog,
    __in_opt LPCSTR lpBackupFileName
    );
WINADVAPI
BOOL
WINAPI
ClearEventLogW (
    __in     HANDLE hEventLog,
    __in_opt LPCWSTR lpBackupFileName
    );
#ifdef UNICODE
#define ClearEventLog  ClearEventLogW
#else
#define ClearEventLog  ClearEventLogA
#endif // !UNICODE

WINADVAPI
BOOL
WINAPI
BackupEventLogA (
    __in HANDLE hEventLog,
    __in LPCSTR lpBackupFileName
    );
WINADVAPI
BOOL
WINAPI
BackupEventLogW (
    __in HANDLE hEventLog,
    __in LPCWSTR lpBackupFileName
    );
#ifdef UNICODE
#define BackupEventLog  BackupEventLogW
#else
#define BackupEventLog  BackupEventLogA
#endif // !UNICODE

WINADVAPI
BOOL
WINAPI
CloseEventLog (
    __in HANDLE hEventLog
    );

WINADVAPI
BOOL
WINAPI
DeregisterEventSource (
    __in HANDLE hEventLog
    );

WINADVAPI
BOOL
WINAPI
NotifyChangeEventLog(
    __in HANDLE  hEventLog,
    __in HANDLE  hEvent
    );

WINADVAPI
BOOL
WINAPI
GetNumberOfEventLogRecords (
    __in  HANDLE hEventLog,
    __out PDWORD NumberOfRecords
    );

WINADVAPI
BOOL
WINAPI
GetOldestEventLogRecord (
    __in  HANDLE hEventLog,
    __out PDWORD OldestRecord
    );

WINADVAPI
__out
HANDLE
WINAPI
OpenEventLogA (
    __in_opt LPCSTR lpUNCServerName,
    __in     LPCSTR lpSourceName
    );
WINADVAPI
__out
HANDLE
WINAPI
OpenEventLogW (
    __in_opt LPCWSTR lpUNCServerName,
    __in     LPCWSTR lpSourceName
    );
#ifdef UNICODE
#define OpenEventLog  OpenEventLogW
#else
#define OpenEventLog  OpenEventLogA
#endif // !UNICODE

WINADVAPI
__out
HANDLE
WINAPI
RegisterEventSourceA (
    __in_opt LPCSTR lpUNCServerName,
    __in     LPCSTR lpSourceName
    );
WINADVAPI
__out
HANDLE
WINAPI
RegisterEventSourceW (
    __in_opt LPCWSTR lpUNCServerName,
    __in     LPCWSTR lpSourceName
    );
#ifdef UNICODE
#define RegisterEventSource  RegisterEventSourceW
#else
#define RegisterEventSource  RegisterEventSourceA
#endif // !UNICODE

WINADVAPI
__out
HANDLE
WINAPI
OpenBackupEventLogA (
    __in_opt LPCSTR lpUNCServerName,
    __in     LPCSTR lpFileName
    );
WINADVAPI
__out
HANDLE
WINAPI
OpenBackupEventLogW (
    __in_opt LPCWSTR lpUNCServerName,
    __in     LPCWSTR lpFileName
    );
#ifdef UNICODE
#define OpenBackupEventLog  OpenBackupEventLogW
#else
#define OpenBackupEventLog  OpenBackupEventLogA
#endif // !UNICODE

WINADVAPI
BOOL
WINAPI
ReadEventLogA (
    __in  HANDLE     hEventLog,
    __in  DWORD      dwReadFlags,
    __in  DWORD      dwRecordOffset,
    __out_bcount_part(nNumberOfBytesToRead, *pnBytesRead) LPVOID     lpBuffer,
    __in  DWORD      nNumberOfBytesToRead,
    __out DWORD      *pnBytesRead,
    __out DWORD      *pnMinNumberOfBytesNeeded
    );
WINADVAPI
BOOL
WINAPI
ReadEventLogW (
    __in  HANDLE     hEventLog,
    __in  DWORD      dwReadFlags,
    __in  DWORD      dwRecordOffset,
    __out_bcount_part(nNumberOfBytesToRead, *pnBytesRead) LPVOID     lpBuffer,
    __in  DWORD      nNumberOfBytesToRead,
    __out DWORD      *pnBytesRead,
    __out DWORD      *pnMinNumberOfBytesNeeded
    );
#ifdef UNICODE
#define ReadEventLog  ReadEventLogW
#else
#define ReadEventLog  ReadEventLogA
#endif // !UNICODE

WINADVAPI
BOOL
WINAPI
ReportEventA (
    __in     HANDLE     hEventLog,
    __in     WORD       wType,
    __in     WORD       wCategory,
    __in     DWORD      dwEventID,
    __in_opt PSID       lpUserSid,
    __in     WORD       wNumStrings,
    __in     DWORD      dwDataSize,
    __in_ecount_opt(wNumStrings) LPCSTR *lpStrings,
    __in_bcount_opt(dwDataSize) LPVOID lpRawData
    );
WINADVAPI
BOOL
WINAPI
ReportEventW (
    __in     HANDLE     hEventLog,
    __in     WORD       wType,
    __in     WORD       wCategory,
    __in     DWORD      dwEventID,
    __in_opt PSID       lpUserSid,
    __in     WORD       wNumStrings,
    __in     DWORD      dwDataSize,
    __in_ecount_opt(wNumStrings) LPCWSTR *lpStrings,
    __in_bcount_opt(dwDataSize) LPVOID lpRawData
    );
#ifdef UNICODE
#define ReportEvent  ReportEventW
#else
#define ReportEvent  ReportEventA
#endif // !UNICODE


#define EVENTLOG_FULL_INFO      0

typedef struct _EVENTLOG_FULL_INFORMATION
{
    DWORD    dwFull;
}
EVENTLOG_FULL_INFORMATION, *LPEVENTLOG_FULL_INFORMATION;

WINADVAPI
BOOL
WINAPI
GetEventLogInformation (
    __in  HANDLE     hEventLog,
    __in  DWORD      dwInfoLevel,
    __out_bcount_part(cbBufSize, *pcbBytesNeeded) LPVOID lpBuffer,
    __in  DWORD      cbBufSize,
    __out LPDWORD    pcbBytesNeeded
    );

//
//
// Security APIs
//


WINADVAPI
BOOL
WINAPI
DuplicateToken(
    __in        HANDLE ExistingTokenHandle,
    __in        SECURITY_IMPERSONATION_LEVEL ImpersonationLevel,
    __deref_out PHANDLE DuplicateTokenHandle
    );

WINADVAPI
BOOL
WINAPI
GetKernelObjectSecurity (
    __in  HANDLE Handle,
    __in  SECURITY_INFORMATION RequestedInformation,
    __out_bcount_opt(nLength) PSECURITY_DESCRIPTOR pSecurityDescriptor,
    __in  DWORD nLength,
    __out LPDWORD lpnLengthNeeded
    );

WINADVAPI
BOOL
WINAPI
ImpersonateNamedPipeClient(
    __in HANDLE hNamedPipe
    );

WINADVAPI
BOOL
WINAPI
ImpersonateSelf(
    __in SECURITY_IMPERSONATION_LEVEL ImpersonationLevel
    );


WINADVAPI
BOOL
WINAPI
RevertToSelf (
    VOID
    );

WINADVAPI
BOOL
APIENTRY
SetThreadToken (
    __in_opt PHANDLE Thread,
    __in_opt HANDLE Token
    );

WINADVAPI
BOOL
WINAPI
AccessCheck (
    __in    PSECURITY_DESCRIPTOR pSecurityDescriptor,
    __in    HANDLE ClientToken,
    __in    DWORD DesiredAccess,
    __in    PGENERIC_MAPPING GenericMapping,
    __out_bcount_part_opt(*PrivilegeSetLength, *PrivilegeSetLength) PPRIVILEGE_SET PrivilegeSet,
    __inout LPDWORD PrivilegeSetLength,
    __out   LPDWORD GrantedAccess,
    __out   LPBOOL AccessStatus
    );

#if(_WIN32_WINNT >= 0x0500)
WINADVAPI
BOOL
WINAPI
AccessCheckByType (
    __in     PSECURITY_DESCRIPTOR pSecurityDescriptor,
    __in_opt PSID PrincipalSelfSid,
    __in     HANDLE ClientToken,
    __in     DWORD DesiredAccess,
    __inout_ecount_opt(ObjectTypeListLength) POBJECT_TYPE_LIST ObjectTypeList,
    __in     DWORD ObjectTypeListLength,
    __in     PGENERIC_MAPPING GenericMapping,
    __out_bcount_part_opt(*PrivilegeSetLength, *PrivilegeSetLength) PPRIVILEGE_SET PrivilegeSet,
    __inout  LPDWORD PrivilegeSetLength,
    __out    LPDWORD GrantedAccess,
    __out    LPBOOL AccessStatus
    );

WINADVAPI
BOOL
WINAPI
AccessCheckByTypeResultList (
    __in     PSECURITY_DESCRIPTOR pSecurityDescriptor,
    __in_opt PSID PrincipalSelfSid,
    __in     HANDLE ClientToken,
    __in     DWORD DesiredAccess,
    __inout_ecount_opt(ObjectTypeListLength) POBJECT_TYPE_LIST ObjectTypeList,
    __in     DWORD ObjectTypeListLength,
    __in     PGENERIC_MAPPING GenericMapping,
    __out_bcount_part_opt(*PrivilegeSetLength, *PrivilegeSetLength) PPRIVILEGE_SET PrivilegeSet,
    __inout  LPDWORD PrivilegeSetLength,
    __out    LPDWORD GrantedAccessList,
    __out    LPDWORD AccessStatusList
    );
#endif /* _WIN32_WINNT >=  0x0500 */


WINADVAPI
BOOL
WINAPI
OpenProcessToken (
    __in        HANDLE ProcessHandle,
    __in        DWORD DesiredAccess,
    __deref_out PHANDLE TokenHandle
    );


WINADVAPI
BOOL
WINAPI
OpenThreadToken (
    __in        HANDLE ThreadHandle,
    __in        DWORD DesiredAccess,
    __in        BOOL OpenAsSelf,
    __deref_out PHANDLE TokenHandle
    );


WINADVAPI
BOOL
WINAPI
GetTokenInformation (
    __in      HANDLE TokenHandle,
    __in      TOKEN_INFORMATION_CLASS TokenInformationClass,
    __out_bcount_part_opt(TokenInformationLength, *ReturnLength) LPVOID TokenInformation,
    __in      DWORD TokenInformationLength,
    __out     PDWORD ReturnLength
    );


WINADVAPI
BOOL
WINAPI
SetTokenInformation (
    __in HANDLE TokenHandle,
    __in TOKEN_INFORMATION_CLASS TokenInformationClass,
    __in_bcount(TokenInformationLength) LPVOID TokenInformation,
    __in DWORD TokenInformationLength
    );


WINADVAPI
BOOL
WINAPI
AdjustTokenPrivileges (
    __in      HANDLE TokenHandle,
    __in      BOOL DisableAllPrivileges,
    __in_opt  PTOKEN_PRIVILEGES NewState,
    __in      DWORD BufferLength,
    __out_bcount_part_opt(BufferLength, *ReturnLength) PTOKEN_PRIVILEGES PreviousState,
    __out_opt PDWORD ReturnLength
    );


WINADVAPI
BOOL
WINAPI
AdjustTokenGroups (
    __in      HANDLE TokenHandle,
    __in      BOOL ResetToDefault,
    __in_opt  PTOKEN_GROUPS NewState,
    __in      DWORD BufferLength,
    __out_bcount_part_opt(BufferLength, *ReturnLength) PTOKEN_GROUPS PreviousState,
    __out_opt PDWORD ReturnLength
    );


WINADVAPI
BOOL
WINAPI
PrivilegeCheck (
    __in    HANDLE ClientToken,
    __inout PPRIVILEGE_SET RequiredPrivileges,
    __out   LPBOOL pfResult
    );


WINADVAPI
BOOL
WINAPI
AccessCheckAndAuditAlarmA (
    __in     LPCSTR SubsystemName,
    __in_opt LPVOID HandleId,
    __in     LPSTR ObjectTypeName,
    __in_opt LPSTR ObjectName,
    __in     PSECURITY_DESCRIPTOR SecurityDescriptor,
    __in     DWORD DesiredAccess,
    __in     PGENERIC_MAPPING GenericMapping,
    __in     BOOL ObjectCreation,
    __out    LPDWORD GrantedAccess,
    __out    LPBOOL AccessStatus,
    __out    LPBOOL pfGenerateOnClose
    );
WINADVAPI
BOOL
WINAPI
AccessCheckAndAuditAlarmW (
    __in     LPCWSTR SubsystemName,
    __in_opt LPVOID HandleId,
    __in     LPWSTR ObjectTypeName,
    __in_opt LPWSTR ObjectName,
    __in     PSECURITY_DESCRIPTOR SecurityDescriptor,
    __in     DWORD DesiredAccess,
    __in     PGENERIC_MAPPING GenericMapping,
    __in     BOOL ObjectCreation,
    __out    LPDWORD GrantedAccess,
    __out    LPBOOL AccessStatus,
    __out    LPBOOL pfGenerateOnClose
    );
#ifdef UNICODE
#define AccessCheckAndAuditAlarm  AccessCheckAndAuditAlarmW
#else
#define AccessCheckAndAuditAlarm  AccessCheckAndAuditAlarmA
#endif // !UNICODE

#if(_WIN32_WINNT >= 0x0500)

WINADVAPI
BOOL
WINAPI
AccessCheckByTypeAndAuditAlarmA (
    __in     LPCSTR SubsystemName,
    __in     LPVOID HandleId,
    __in     LPCSTR ObjectTypeName,
    __in_opt LPCSTR ObjectName,
    __in     PSECURITY_DESCRIPTOR SecurityDescriptor,
    __in_opt PSID PrincipalSelfSid,
    __in     DWORD DesiredAccess,
    __in     AUDIT_EVENT_TYPE AuditType,
    __in     DWORD Flags,
    __inout_ecount_opt(ObjectTypeListLength) POBJECT_TYPE_LIST ObjectTypeList,
    __in     DWORD ObjectTypeListLength,
    __in     PGENERIC_MAPPING GenericMapping,
    __in     BOOL ObjectCreation,
    __out    LPDWORD GrantedAccess,
    __out    LPBOOL AccessStatus,
    __out    LPBOOL pfGenerateOnClose
    );
WINADVAPI
BOOL
WINAPI
AccessCheckByTypeAndAuditAlarmW (
    __in     LPCWSTR SubsystemName,
    __in     LPVOID HandleId,
    __in     LPCWSTR ObjectTypeName,
    __in_opt LPCWSTR ObjectName,
    __in     PSECURITY_DESCRIPTOR SecurityDescriptor,
    __in_opt PSID PrincipalSelfSid,
    __in     DWORD DesiredAccess,
    __in     AUDIT_EVENT_TYPE AuditType,
    __in     DWORD Flags,
    __inout_ecount_opt(ObjectTypeListLength) POBJECT_TYPE_LIST ObjectTypeList,
    __in     DWORD ObjectTypeListLength,
    __in     PGENERIC_MAPPING GenericMapping,
    __in     BOOL ObjectCreation,
    __out    LPDWORD GrantedAccess,
    __out    LPBOOL AccessStatus,
    __out    LPBOOL pfGenerateOnClose
    );
#ifdef UNICODE
#define AccessCheckByTypeAndAuditAlarm  AccessCheckByTypeAndAuditAlarmW
#else
#define AccessCheckByTypeAndAuditAlarm  AccessCheckByTypeAndAuditAlarmA
#endif // !UNICODE

WINADVAPI
BOOL
WINAPI
AccessCheckByTypeResultListAndAuditAlarmA (
    __in     LPCSTR SubsystemName,
    __in     LPVOID HandleId,
    __in     LPCSTR ObjectTypeName,
    __in_opt LPCSTR ObjectName,
    __in     PSECURITY_DESCRIPTOR SecurityDescriptor,
    __in_opt PSID PrincipalSelfSid,
    __in     DWORD DesiredAccess,
    __in     AUDIT_EVENT_TYPE AuditType,
    __in     DWORD Flags,
    __inout_ecount_opt(ObjectTypeListLength) POBJECT_TYPE_LIST ObjectTypeList,
    __in     DWORD ObjectTypeListLength,
    __in     PGENERIC_MAPPING GenericMapping,
    __in     BOOL ObjectCreation,
    __out    LPDWORD GrantedAccess,
    __out    LPDWORD AccessStatusList,
    __out    LPBOOL pfGenerateOnClose
    );
WINADVAPI
BOOL
WINAPI
AccessCheckByTypeResultListAndAuditAlarmW (
    __in     LPCWSTR SubsystemName,
    __in     LPVOID HandleId,
    __in     LPCWSTR ObjectTypeName,
    __in_opt LPCWSTR ObjectName,
    __in     PSECURITY_DESCRIPTOR SecurityDescriptor,
    __in_opt PSID PrincipalSelfSid,
    __in     DWORD DesiredAccess,
    __in     AUDIT_EVENT_TYPE AuditType,
    __in     DWORD Flags,
    __inout_ecount_opt(ObjectTypeListLength) POBJECT_TYPE_LIST ObjectTypeList,
    __in     DWORD ObjectTypeListLength,
    __in     PGENERIC_MAPPING GenericMapping,
    __in     BOOL ObjectCreation,
    __out    LPDWORD GrantedAccess,
    __out    LPDWORD AccessStatusList,
    __out    LPBOOL pfGenerateOnClose
    );
#ifdef UNICODE
#define AccessCheckByTypeResultListAndAuditAlarm  AccessCheckByTypeResultListAndAuditAlarmW
#else
#define AccessCheckByTypeResultListAndAuditAlarm  AccessCheckByTypeResultListAndAuditAlarmA
#endif // !UNICODE

WINADVAPI
BOOL
WINAPI
AccessCheckByTypeResultListAndAuditAlarmByHandleA (
    __in     LPCSTR SubsystemName,
    __in     LPVOID HandleId,
    __in     HANDLE ClientToken,
    __in     LPCSTR ObjectTypeName,
    __in_opt LPCSTR ObjectName,
    __in     PSECURITY_DESCRIPTOR SecurityDescriptor,
    __in_opt PSID PrincipalSelfSid,
    __in     DWORD DesiredAccess,
    __in     AUDIT_EVENT_TYPE AuditType,
    __in     DWORD Flags,
    __inout_ecount_opt(ObjectTypeListLength) POBJECT_TYPE_LIST ObjectTypeList,
    __in     DWORD ObjectTypeListLength,
    __in     PGENERIC_MAPPING GenericMapping,
    __in     BOOL ObjectCreation,
    __out    LPDWORD GrantedAccess,
    __out    LPDWORD AccessStatusList,
    __out    LPBOOL pfGenerateOnClose
    );
WINADVAPI
BOOL
WINAPI
AccessCheckByTypeResultListAndAuditAlarmByHandleW (
    __in     LPCWSTR SubsystemName,
    __in     LPVOID HandleId,
    __in     HANDLE ClientToken,
    __in     LPCWSTR ObjectTypeName,
    __in_opt LPCWSTR ObjectName,
    __in     PSECURITY_DESCRIPTOR SecurityDescriptor,
    __in_opt PSID PrincipalSelfSid,
    __in     DWORD DesiredAccess,
    __in     AUDIT_EVENT_TYPE AuditType,
    __in     DWORD Flags,
    __inout_ecount_opt(ObjectTypeListLength) POBJECT_TYPE_LIST ObjectTypeList,
    __in     DWORD ObjectTypeListLength,
    __in     PGENERIC_MAPPING GenericMapping,
    __in     BOOL ObjectCreation,
    __out    LPDWORD GrantedAccess,
    __out    LPDWORD AccessStatusList,
    __out    LPBOOL pfGenerateOnClose
    );
#ifdef UNICODE
#define AccessCheckByTypeResultListAndAuditAlarmByHandle  AccessCheckByTypeResultListAndAuditAlarmByHandleW
#else
#define AccessCheckByTypeResultListAndAuditAlarmByHandle  AccessCheckByTypeResultListAndAuditAlarmByHandleA
#endif // !UNICODE

#endif //(_WIN32_WINNT >= 0x0500)

WINADVAPI
BOOL
WINAPI
ObjectOpenAuditAlarmA (
    __in     LPCSTR SubsystemName,
    __in     LPVOID HandleId,
    __in     LPSTR ObjectTypeName,
    __in_opt LPSTR ObjectName,
    __in     PSECURITY_DESCRIPTOR pSecurityDescriptor,
    __in     HANDLE ClientToken,
    __in     DWORD DesiredAccess,
    __in     DWORD GrantedAccess,
    __in_opt PPRIVILEGE_SET Privileges,
    __in     BOOL ObjectCreation,
    __in     BOOL AccessGranted,
    __out    LPBOOL GenerateOnClose
    );
WINADVAPI
BOOL
WINAPI
ObjectOpenAuditAlarmW (
    __in     LPCWSTR SubsystemName,
    __in     LPVOID HandleId,
    __in     LPWSTR ObjectTypeName,
    __in_opt LPWSTR ObjectName,
    __in     PSECURITY_DESCRIPTOR pSecurityDescriptor,
    __in     HANDLE ClientToken,
    __in     DWORD DesiredAccess,
    __in     DWORD GrantedAccess,
    __in_opt PPRIVILEGE_SET Privileges,
    __in     BOOL ObjectCreation,
    __in     BOOL AccessGranted,
    __out    LPBOOL GenerateOnClose
    );
#ifdef UNICODE
#define ObjectOpenAuditAlarm  ObjectOpenAuditAlarmW
#else
#define ObjectOpenAuditAlarm  ObjectOpenAuditAlarmA
#endif // !UNICODE


WINADVAPI
BOOL
WINAPI
ObjectPrivilegeAuditAlarmA (
    __in LPCSTR SubsystemName,
    __in LPVOID HandleId,
    __in HANDLE ClientToken,
    __in DWORD DesiredAccess,
    __in PPRIVILEGE_SET Privileges,
    __in BOOL AccessGranted
    );
WINADVAPI
BOOL
WINAPI
ObjectPrivilegeAuditAlarmW (
    __in LPCWSTR SubsystemName,
    __in LPVOID HandleId,
    __in HANDLE ClientToken,
    __in DWORD DesiredAccess,
    __in PPRIVILEGE_SET Privileges,
    __in BOOL AccessGranted
    );
#ifdef UNICODE
#define ObjectPrivilegeAuditAlarm  ObjectPrivilegeAuditAlarmW
#else
#define ObjectPrivilegeAuditAlarm  ObjectPrivilegeAuditAlarmA
#endif // !UNICODE


WINADVAPI
BOOL
WINAPI
ObjectCloseAuditAlarmA (
    __in LPCSTR SubsystemName,
    __in LPVOID HandleId,
    __in BOOL GenerateOnClose
    );
WINADVAPI
BOOL
WINAPI
ObjectCloseAuditAlarmW (
    __in LPCWSTR SubsystemName,
    __in LPVOID HandleId,
    __in BOOL GenerateOnClose
    );
#ifdef UNICODE
#define ObjectCloseAuditAlarm  ObjectCloseAuditAlarmW
#else
#define ObjectCloseAuditAlarm  ObjectCloseAuditAlarmA
#endif // !UNICODE


WINADVAPI
BOOL
WINAPI
ObjectDeleteAuditAlarmA (
    __in LPCSTR SubsystemName,
    __in LPVOID HandleId,
    __in BOOL GenerateOnClose
    );
WINADVAPI
BOOL
WINAPI
ObjectDeleteAuditAlarmW (
    __in LPCWSTR SubsystemName,
    __in LPVOID HandleId,
    __in BOOL GenerateOnClose
    );
#ifdef UNICODE
#define ObjectDeleteAuditAlarm  ObjectDeleteAuditAlarmW
#else
#define ObjectDeleteAuditAlarm  ObjectDeleteAuditAlarmA
#endif // !UNICODE


WINADVAPI
BOOL
WINAPI
PrivilegedServiceAuditAlarmA (
    __in LPCSTR SubsystemName,
    __in LPCSTR ServiceName,
    __in HANDLE ClientToken,
    __in PPRIVILEGE_SET Privileges,
    __in BOOL AccessGranted
    );
WINADVAPI
BOOL
WINAPI
PrivilegedServiceAuditAlarmW (
    __in LPCWSTR SubsystemName,
    __in LPCWSTR ServiceName,
    __in HANDLE ClientToken,
    __in PPRIVILEGE_SET Privileges,
    __in BOOL AccessGranted
    );
#ifdef UNICODE
#define PrivilegedServiceAuditAlarm  PrivilegedServiceAuditAlarmW
#else
#define PrivilegedServiceAuditAlarm  PrivilegedServiceAuditAlarmA
#endif // !UNICODE



#if(_WIN32_WINNT >= 0x0501)


WINADVAPI
BOOL
WINAPI
IsWellKnownSid (
    __in PSID pSid,
    __in WELL_KNOWN_SID_TYPE WellKnownSidType
    );

WINADVAPI
__success(return != FALSE) BOOL
WINAPI
CreateWellKnownSid(
    __in     WELL_KNOWN_SID_TYPE WellKnownSidType,
    __in_opt PSID DomainSid,
    __out_bcount_part_opt(*cbSid, *cbSid) PSID pSid,
    __inout  DWORD *cbSid
    );

WINADVAPI
__success(return != FALSE) BOOL
WINAPI
EqualDomainSid(
    __in  PSID pSid1,
    __in  PSID pSid2,
    __out BOOL *pfEqual
    );

WINADVAPI
__success(return != FALSE) BOOL
WINAPI
GetWindowsAccountDomainSid(
    __in    PSID pSid,
    __out_bcount_part_opt(*cbDomainSid, *cbDomainSid) PSID pDomainSid,
    __inout DWORD* cbDomainSid
    );

#endif //(_WIN32_WINNT >= 0x0501)

WINADVAPI
BOOL
WINAPI
IsValidSid (
    __in PSID pSid
    );


WINADVAPI
BOOL
WINAPI
EqualSid (
    __in PSID pSid1,
    __in PSID pSid2
    );


WINADVAPI
BOOL
WINAPI
EqualPrefixSid (
    __in PSID pSid1,
    __in PSID pSid2
    );


WINADVAPI
DWORD
WINAPI
GetSidLengthRequired (
    __in UCHAR nSubAuthorityCount
    );


WINADVAPI
BOOL
WINAPI
AllocateAndInitializeSid (
    __in        PSID_IDENTIFIER_AUTHORITY pIdentifierAuthority,
    __in        BYTE nSubAuthorityCount,
    __in        DWORD nSubAuthority0,
    __in        DWORD nSubAuthority1,
    __in        DWORD nSubAuthority2,
    __in        DWORD nSubAuthority3,
    __in        DWORD nSubAuthority4,
    __in        DWORD nSubAuthority5,
    __in        DWORD nSubAuthority6,
    __in        DWORD nSubAuthority7,
    __deref_out PSID *pSid
    );

WINADVAPI
PVOID
WINAPI
FreeSid(
    __in PSID pSid
    );

WINADVAPI
BOOL
WINAPI
InitializeSid (
    __out PSID Sid,
    __in  PSID_IDENTIFIER_AUTHORITY pIdentifierAuthority,
    __in  BYTE nSubAuthorityCount
    );


WINADVAPI
__out
PSID_IDENTIFIER_AUTHORITY
WINAPI
GetSidIdentifierAuthority (
    __in PSID pSid
    );


WINADVAPI
__out
PDWORD
WINAPI
GetSidSubAuthority (
    __in PSID pSid,
    __in DWORD nSubAuthority
    );


WINADVAPI
__out
PUCHAR
WINAPI
GetSidSubAuthorityCount (
    __in PSID pSid
    );


WINADVAPI
DWORD
WINAPI
GetLengthSid (
    __in PSID pSid
    );


WINADVAPI
BOOL
WINAPI
CopySid (
    __in DWORD nDestinationSidLength,
    __out_bcount(nDestinationSidLength) PSID pDestinationSid,
    __in PSID pSourceSid
    );


WINADVAPI
BOOL
WINAPI
AreAllAccessesGranted (
    __in DWORD GrantedAccess,
    __in DWORD DesiredAccess
    );


WINADVAPI
BOOL
WINAPI
AreAnyAccessesGranted (
    __in DWORD GrantedAccess,
    __in DWORD DesiredAccess
    );


WINADVAPI
VOID
WINAPI
MapGenericMask (
    __inout PDWORD AccessMask,
    __in    PGENERIC_MAPPING GenericMapping
    );


WINADVAPI
BOOL
WINAPI
IsValidAcl (
    __in PACL pAcl
    );


WINADVAPI
BOOL
WINAPI
InitializeAcl (
    __out_bcount(nAclLength) PACL pAcl,
    __in DWORD nAclLength,
    __in DWORD dwAclRevision
    );


WINADVAPI
BOOL
WINAPI
GetAclInformation (
    __in PACL pAcl,
    __out_bcount(nAclInformationLength) LPVOID pAclInformation,
    __in DWORD nAclInformationLength,
    __in ACL_INFORMATION_CLASS dwAclInformationClass
    );


WINADVAPI
BOOL
WINAPI
SetAclInformation (
    __inout PACL pAcl,
    __in_bcount(nAclInformationLength) LPVOID pAclInformation,
    __in    DWORD nAclInformationLength,
    __in    ACL_INFORMATION_CLASS dwAclInformationClass
    );


WINADVAPI
BOOL
WINAPI
AddAce (
    __inout PACL pAcl,
    __in    DWORD dwAceRevision,
    __in    DWORD dwStartingAceIndex,
    __in_bcount(nAceListLength) LPVOID pAceList,
    __in    DWORD nAceListLength
    );


WINADVAPI
BOOL
WINAPI
DeleteAce (
    __inout PACL pAcl,
    __in    DWORD dwAceIndex
    );


WINADVAPI
BOOL
WINAPI
GetAce (
    __in        PACL pAcl,
    __in        DWORD dwAceIndex,
    __deref_out LPVOID *pAce
    );


WINADVAPI
BOOL
WINAPI
AddAccessAllowedAce (
    __inout PACL pAcl,
    __in    DWORD dwAceRevision,
    __in    DWORD AccessMask,
    __in    PSID pSid
    );

#if(_WIN32_WINNT >= 0x0500)
WINADVAPI
BOOL
WINAPI
AddAccessAllowedAceEx (
    __inout PACL pAcl,
    __in    DWORD dwAceRevision,
    __in    DWORD AceFlags,
    __in    DWORD AccessMask,
    __in    PSID pSid
    );
#endif /* _WIN32_WINNT >=  0x0500 */

#if(_WIN32_WINNT >= 0x0600)
WINADVAPI
BOOL
WINAPI
AddMandatoryAce (
    __inout PACL pAcl,
    __in    DWORD dwAceRevision,
    __in    DWORD AceFlags,
    __in    DWORD MandatoryPolicy,
    __in    PSID pLabelSid
    );
#endif /* _WIN32_WINNT >=  0x0600 */

#if(_WIN32_WINNT >= 0x0601)
WINADVAPI
BOOL
WINAPI
AddConditionalAce (
    __inout PACL pAcl,
    __in    DWORD dwAceRevision,
    __in    DWORD AceFlags,
    __in    UCHAR AceType,
    __in    DWORD AccessMask,
    __in    PSID pSid,
    __in    PWCHAR ConditionStr,
    __out DWORD *ReturnLength
    );
#endif /* _WIN32_WINNT >=  0x0601 */

WINADVAPI
BOOL
WINAPI
AddAccessDeniedAce (
    __inout PACL pAcl,
    __in    DWORD dwAceRevision,
    __in    DWORD AccessMask,
    __in    PSID pSid
    );

#if(_WIN32_WINNT >= 0x0500)
WINADVAPI
BOOL
WINAPI
AddAccessDeniedAceEx (
    __inout PACL pAcl,
    __in    DWORD dwAceRevision,
    __in    DWORD AceFlags,
    __in    DWORD AccessMask,
    __in    PSID pSid
    );
#endif /* _WIN32_WINNT >=  0x0500 */

WINADVAPI
BOOL
WINAPI
AddAuditAccessAce(
    __inout PACL pAcl,
    __in    DWORD dwAceRevision,
    __in    DWORD dwAccessMask,
    __in    PSID pSid,
    __in    BOOL bAuditSuccess,
    __in    BOOL bAuditFailure
    );

#if(_WIN32_WINNT >= 0x0500)
WINADVAPI
BOOL
WINAPI
AddAuditAccessAceEx(
    __inout PACL pAcl,
    __in    DWORD dwAceRevision,
    __in    DWORD AceFlags,
    __in    DWORD dwAccessMask,
    __in    PSID pSid,
    __in    BOOL bAuditSuccess,
    __in    BOOL bAuditFailure
    );

WINADVAPI
BOOL
WINAPI
AddAccessAllowedObjectAce (
    __inout  PACL pAcl,
    __in     DWORD dwAceRevision,
    __in     DWORD AceFlags,
    __in     DWORD AccessMask,
    __in_opt GUID *ObjectTypeGuid,
    __in_opt GUID *InheritedObjectTypeGuid,
    __in     PSID pSid
    );

WINADVAPI
BOOL
WINAPI
AddAccessDeniedObjectAce (
    __inout  PACL pAcl,
    __in     DWORD dwAceRevision,
    __in     DWORD AceFlags,
    __in     DWORD AccessMask,
    __in_opt GUID *ObjectTypeGuid,
    __in_opt GUID *InheritedObjectTypeGuid,
    __in     PSID pSid
    );

WINADVAPI
BOOL
WINAPI
AddAuditAccessObjectAce (
    __inout  PACL pAcl,
    __in     DWORD dwAceRevision,
    __in     DWORD AceFlags,
    __in     DWORD AccessMask,
    __in_opt GUID *ObjectTypeGuid,
    __in_opt GUID *InheritedObjectTypeGuid,
    __in     PSID pSid,
    __in     BOOL bAuditSuccess,
    __in     BOOL bAuditFailure
    );
#endif /* _WIN32_WINNT >=  0x0500 */

WINADVAPI
BOOL
WINAPI
FindFirstFreeAce (
    __in        PACL pAcl,
    __deref_out LPVOID *pAce
    );


WINADVAPI
BOOL
WINAPI
InitializeSecurityDescriptor (
    __out PSECURITY_DESCRIPTOR pSecurityDescriptor,
    __in  DWORD dwRevision
    );


WINADVAPI
BOOL
WINAPI
IsValidSecurityDescriptor (
    __in PSECURITY_DESCRIPTOR pSecurityDescriptor
    );

WINADVAPI
BOOL
WINAPI
IsValidRelativeSecurityDescriptor (
    __in PSECURITY_DESCRIPTOR pSecurityDescriptor,
    __in ULONG SecurityDescriptorLength,
    __in SECURITY_INFORMATION RequiredInformation
    );

WINADVAPI
DWORD
WINAPI
GetSecurityDescriptorLength (
    __in PSECURITY_DESCRIPTOR pSecurityDescriptor
    );


WINADVAPI
BOOL
WINAPI
GetSecurityDescriptorControl (
    __in  PSECURITY_DESCRIPTOR pSecurityDescriptor,
    __out PSECURITY_DESCRIPTOR_CONTROL pControl,
    __out LPDWORD lpdwRevision
    );

#if(_WIN32_WINNT >= 0x0500)
WINADVAPI
BOOL
WINAPI
SetSecurityDescriptorControl (
    __in PSECURITY_DESCRIPTOR pSecurityDescriptor,
    __in SECURITY_DESCRIPTOR_CONTROL ControlBitsOfInterest,
    __in SECURITY_DESCRIPTOR_CONTROL ControlBitsToSet
    );
#endif /* _WIN32_WINNT >=  0x0500 */

WINADVAPI
BOOL
WINAPI
SetSecurityDescriptorDacl (
    __inout  PSECURITY_DESCRIPTOR pSecurityDescriptor,
    __in     BOOL bDaclPresent,
    __in_opt PACL pDacl,
    __in     BOOL bDaclDefaulted
    );


WINADVAPI
BOOL
WINAPI
GetSecurityDescriptorDacl (
    __in        PSECURITY_DESCRIPTOR pSecurityDescriptor,
    __out       LPBOOL lpbDaclPresent,
    __deref_out PACL *pDacl,
    __out       LPBOOL lpbDaclDefaulted
    );


WINADVAPI
BOOL
WINAPI
SetSecurityDescriptorSacl (
    __inout  PSECURITY_DESCRIPTOR pSecurityDescriptor,
    __in     BOOL bSaclPresent,
    __in_opt PACL pSacl,
    __in     BOOL bSaclDefaulted
    );


WINADVAPI
BOOL
WINAPI
GetSecurityDescriptorSacl (
    __in        PSECURITY_DESCRIPTOR pSecurityDescriptor,
    __out       LPBOOL lpbSaclPresent,
    __deref_out PACL *pSacl,
    __out       LPBOOL lpbSaclDefaulted
    );


WINADVAPI
BOOL
WINAPI
SetSecurityDescriptorOwner (
    __inout  PSECURITY_DESCRIPTOR pSecurityDescriptor,
    __in_opt PSID pOwner,
    __in     BOOL bOwnerDefaulted
    );


WINADVAPI
BOOL
WINAPI
GetSecurityDescriptorOwner (
    __in        PSECURITY_DESCRIPTOR pSecurityDescriptor,
    __deref_out PSID *pOwner,
    __out       LPBOOL lpbOwnerDefaulted
    );


WINADVAPI
BOOL
WINAPI
SetSecurityDescriptorGroup (
    __inout  PSECURITY_DESCRIPTOR pSecurityDescriptor,
    __in_opt PSID pGroup,
    __in     BOOL bGroupDefaulted
    );


WINADVAPI
BOOL
WINAPI
GetSecurityDescriptorGroup (
    __in        PSECURITY_DESCRIPTOR pSecurityDescriptor,
    __deref_out PSID *pGroup,
    __out       LPBOOL lpbGroupDefaulted
    );


WINADVAPI
DWORD
WINAPI
SetSecurityDescriptorRMControl(
    __inout  PSECURITY_DESCRIPTOR SecurityDescriptor,
    __in_opt PUCHAR RMControl
    );

WINADVAPI
DWORD
WINAPI
GetSecurityDescriptorRMControl(
    __in  PSECURITY_DESCRIPTOR SecurityDescriptor,
    __out PUCHAR RMControl
    );

WINADVAPI
BOOL
WINAPI
CreatePrivateObjectSecurity (
    __in_opt    PSECURITY_DESCRIPTOR ParentDescriptor,
    __in_opt    PSECURITY_DESCRIPTOR CreatorDescriptor,
    __deref_out PSECURITY_DESCRIPTOR * NewDescriptor,
    __in        BOOL IsDirectoryObject,
    __in_opt    HANDLE Token,
    __in        PGENERIC_MAPPING GenericMapping
    );

#if(_WIN32_WINNT >= 0x0500)
WINADVAPI
BOOL
WINAPI
ConvertToAutoInheritPrivateObjectSecurity(
    __in_opt    PSECURITY_DESCRIPTOR ParentDescriptor,
    __in        PSECURITY_DESCRIPTOR CurrentSecurityDescriptor,
    __deref_out PSECURITY_DESCRIPTOR *NewSecurityDescriptor,
    __in_opt    GUID *ObjectType,
    __in        BOOLEAN IsDirectoryObject,
    __in        PGENERIC_MAPPING GenericMapping
    );

WINADVAPI
BOOL
WINAPI
CreatePrivateObjectSecurityEx (
    __in_opt    PSECURITY_DESCRIPTOR ParentDescriptor,
    __in_opt    PSECURITY_DESCRIPTOR CreatorDescriptor,
    __deref_out PSECURITY_DESCRIPTOR * NewDescriptor,
    __in_opt    GUID *ObjectType,
    __in        BOOL IsContainerObject,
    __in        ULONG AutoInheritFlags,
    __in_opt    HANDLE Token,
    __in        PGENERIC_MAPPING GenericMapping
    );

WINADVAPI
BOOL
WINAPI
CreatePrivateObjectSecurityWithMultipleInheritance (
    __in_opt    PSECURITY_DESCRIPTOR ParentDescriptor,
    __in_opt    PSECURITY_DESCRIPTOR CreatorDescriptor,
    __deref_out PSECURITY_DESCRIPTOR * NewDescriptor,
    __in_ecount_opt(GuidCount) GUID **ObjectTypes,
    __in        ULONG GuidCount,
    __in        BOOL IsContainerObject,
    __in        ULONG AutoInheritFlags,
    __in_opt    HANDLE Token,
    __in        PGENERIC_MAPPING GenericMapping
    );
#endif /* _WIN32_WINNT >=  0x0500 */

WINADVAPI
BOOL
WINAPI
SetPrivateObjectSecurity (
    __in          SECURITY_INFORMATION SecurityInformation,
    __in          PSECURITY_DESCRIPTOR ModificationDescriptor,
    __deref_inout PSECURITY_DESCRIPTOR *ObjectsSecurityDescriptor,
    __in          PGENERIC_MAPPING GenericMapping,
    __in_opt      HANDLE Token
    );

#if(_WIN32_WINNT >= 0x0500)
WINADVAPI
BOOL
WINAPI
SetPrivateObjectSecurityEx (
    __in          SECURITY_INFORMATION SecurityInformation,
    __in          PSECURITY_DESCRIPTOR ModificationDescriptor,
    __deref_inout PSECURITY_DESCRIPTOR *ObjectsSecurityDescriptor,
    __in          ULONG AutoInheritFlags,
    __in          PGENERIC_MAPPING GenericMapping,
    __in_opt      HANDLE Token
    );
#endif /* _WIN32_WINNT >=  0x0500 */

WINADVAPI
__success(return != FALSE) BOOL
WINAPI
GetPrivateObjectSecurity (
    __in  PSECURITY_DESCRIPTOR ObjectDescriptor,
    __in  SECURITY_INFORMATION SecurityInformation,
    __out_bcount_part_opt(DescriptorLength, *ReturnLength) PSECURITY_DESCRIPTOR ResultantDescriptor,
    __in  DWORD DescriptorLength,
    __out PDWORD ReturnLength
    );


WINADVAPI
BOOL
WINAPI
DestroyPrivateObjectSecurity (
    __deref PSECURITY_DESCRIPTOR * ObjectDescriptor
    );


WINADVAPI
__success(return != FALSE) BOOL
WINAPI
MakeSelfRelativeSD (
    __in    PSECURITY_DESCRIPTOR pAbsoluteSecurityDescriptor,
    __out_bcount_part_opt(*lpdwBufferLength, *lpdwBufferLength) PSECURITY_DESCRIPTOR pSelfRelativeSecurityDescriptor,
    __inout LPDWORD lpdwBufferLength
    );


WINADVAPI
__success(return != FALSE) BOOL
WINAPI
MakeAbsoluteSD (
    __in    PSECURITY_DESCRIPTOR pSelfRelativeSecurityDescriptor,
    __out_bcount_part_opt(*lpdwAbsoluteSecurityDescriptorSize, *lpdwAbsoluteSecurityDescriptorSize) PSECURITY_DESCRIPTOR pAbsoluteSecurityDescriptor,
    __inout LPDWORD lpdwAbsoluteSecurityDescriptorSize,
    __out_bcount_part_opt(*lpdwDaclSize, *lpdwDaclSize) PACL pDacl,
    __inout LPDWORD lpdwDaclSize,
    __out_bcount_part_opt(*lpdwSaclSize, *lpdwSaclSize) PACL pSacl,
    __inout LPDWORD lpdwSaclSize,
    __out_bcount_part_opt(*lpdwOwnerSize, *lpdwOwnerSize) PSID pOwner,
    __inout LPDWORD lpdwOwnerSize,
    __out_bcount_part_opt(*lpdwPrimaryGroupSize, *lpdwPrimaryGroupSize) PSID pPrimaryGroup,
    __inout LPDWORD lpdwPrimaryGroupSize
    );


WINADVAPI
__success(return != FALSE) BOOL
WINAPI
MakeAbsoluteSD2 (
    __inout_bcount_part(*lpdwBufferSize, *lpdwBufferSize) PSECURITY_DESCRIPTOR pSelfRelativeSecurityDescriptor,
    __inout LPDWORD lpdwBufferSize
    );

WINADVAPI
VOID
WINAPI
QuerySecurityAccessMask(
    __in SECURITY_INFORMATION SecurityInformation,
    __out LPDWORD DesiredAccess
    );

WINADVAPI
VOID
WINAPI
SetSecurityAccessMask(
    __in SECURITY_INFORMATION SecurityInformation,
    __out LPDWORD DesiredAccess
    );

WINADVAPI
BOOL
WINAPI
SetFileSecurityA (
    __in LPCSTR lpFileName,
    __in SECURITY_INFORMATION SecurityInformation,
    __in PSECURITY_DESCRIPTOR pSecurityDescriptor
    );
WINADVAPI
BOOL
WINAPI
SetFileSecurityW (
    __in LPCWSTR lpFileName,
    __in SECURITY_INFORMATION SecurityInformation,
    __in PSECURITY_DESCRIPTOR pSecurityDescriptor
    );
#ifdef UNICODE
#define SetFileSecurity  SetFileSecurityW
#else
#define SetFileSecurity  SetFileSecurityA
#endif // !UNICODE


WINADVAPI
BOOL
WINAPI
GetFileSecurityA (
    __in  LPCSTR lpFileName,
    __in  SECURITY_INFORMATION RequestedInformation,
    __out_bcount_part_opt(nLength, *lpnLengthNeeded) PSECURITY_DESCRIPTOR pSecurityDescriptor,
    __in  DWORD nLength,
    __out LPDWORD lpnLengthNeeded
    );
WINADVAPI
BOOL
WINAPI
GetFileSecurityW (
    __in  LPCWSTR lpFileName,
    __in  SECURITY_INFORMATION RequestedInformation,
    __out_bcount_part_opt(nLength, *lpnLengthNeeded) PSECURITY_DESCRIPTOR pSecurityDescriptor,
    __in  DWORD nLength,
    __out LPDWORD lpnLengthNeeded
    );
#ifdef UNICODE
#define GetFileSecurity  GetFileSecurityW
#else
#define GetFileSecurity  GetFileSecurityA
#endif // !UNICODE


WINADVAPI
BOOL
WINAPI
SetKernelObjectSecurity (
    __in HANDLE Handle,
    __in SECURITY_INFORMATION SecurityInformation,
    __in PSECURITY_DESCRIPTOR SecurityDescriptor
    );

WINBASEAPI
__out
HANDLE
WINAPI
FindFirstChangeNotificationA(
    __in LPCSTR lpPathName,
    __in BOOL bWatchSubtree,
    __in DWORD dwNotifyFilter
    );
WINBASEAPI
__out
HANDLE
WINAPI
FindFirstChangeNotificationW(
    __in LPCWSTR lpPathName,
    __in BOOL bWatchSubtree,
    __in DWORD dwNotifyFilter
    );
#ifdef UNICODE
#define FindFirstChangeNotification  FindFirstChangeNotificationW
#else
#define FindFirstChangeNotification  FindFirstChangeNotificationA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
FindNextChangeNotification(
    __in HANDLE hChangeHandle
    );

WINBASEAPI
BOOL
WINAPI
FindCloseChangeNotification(
    __in HANDLE hChangeHandle
    );

#if(_WIN32_WINNT >= 0x0400)
WINBASEAPI
BOOL
WINAPI
ReadDirectoryChangesW(
    __in        HANDLE hDirectory,
    __out_bcount_part(nBufferLength, *lpBytesReturned) LPVOID lpBuffer,
    __in        DWORD nBufferLength,
    __in        BOOL bWatchSubtree,
    __in        DWORD dwNotifyFilter,
    __out_opt   LPDWORD lpBytesReturned,
    __inout_opt LPOVERLAPPED lpOverlapped,
    __in_opt    LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
    );
#endif /* _WIN32_WINNT >= 0x0400 */

WINBASEAPI
BOOL
WINAPI
VirtualLock(
    __in LPVOID lpAddress,
    __in SIZE_T dwSize
    );

WINBASEAPI
BOOL
WINAPI
VirtualUnlock(
    __in LPVOID lpAddress,
    __in SIZE_T dwSize
    );

WINBASEAPI
__out_opt __out_data_source(FILE)
LPVOID
WINAPI
MapViewOfFileEx(
    __in     HANDLE hFileMappingObject,
    __in     DWORD dwDesiredAccess,
    __in     DWORD dwFileOffsetHigh,
    __in     DWORD dwFileOffsetLow,
    __in     SIZE_T dwNumberOfBytesToMap,
    __in_opt LPVOID lpBaseAddress
    );

#if _WIN32_WINNT >= 0x0600

WINBASEAPI
__out __out_data_source(FILE)
LPVOID
WINAPI
MapViewOfFileExNuma(
    __in     HANDLE hFileMappingObject,
    __in     DWORD dwDesiredAccess,
    __in     DWORD dwFileOffsetHigh,
    __in     DWORD dwFileOffsetLow,
    __in     SIZE_T dwNumberOfBytesToMap,
    __in_opt LPVOID lpBaseAddress,
    __in     DWORD nndPreferred
    );

#endif // _WIN32_WINNT >= 0x0600

WINBASEAPI
BOOL
WINAPI
SetPriorityClass(
    __in HANDLE hProcess,
    __in DWORD dwPriorityClass
    );

WINBASEAPI
DWORD
WINAPI
GetPriorityClass(
    __in HANDLE hProcess
    );

WINBASEAPI
BOOL
WINAPI
IsBadReadPtr(
    __in_opt CONST VOID *lp,
    __in     UINT_PTR ucb
    );

WINBASEAPI
BOOL
WINAPI
IsBadWritePtr(
    __in_opt LPVOID lp,
    __in     UINT_PTR ucb
    );

WINBASEAPI
BOOL
WINAPI
IsBadHugeReadPtr(
    __in_opt CONST VOID *lp,
    __in     UINT_PTR ucb
    );

WINBASEAPI
BOOL
WINAPI
IsBadHugeWritePtr(
    __in_opt LPVOID lp,
    __in     UINT_PTR ucb
    );

WINBASEAPI
BOOL
WINAPI
IsBadCodePtr(
    __in_opt FARPROC lpfn
    );

WINBASEAPI
BOOL
WINAPI
IsBadStringPtrA(
    __in_opt LPCSTR lpsz,
    __in     UINT_PTR ucchMax
    );
WINBASEAPI
BOOL
WINAPI
IsBadStringPtrW(
    __in_opt LPCWSTR lpsz,
    __in     UINT_PTR ucchMax
    );
#ifdef UNICODE
#define IsBadStringPtr  IsBadStringPtrW
#else
#define IsBadStringPtr  IsBadStringPtrA
#endif // !UNICODE

WINADVAPI
__success(return != FALSE) BOOL
WINAPI
LookupAccountSidA(
    __in_opt LPCSTR lpSystemName,
    __in PSID Sid,
    __out_ecount_part_opt(*cchName, *cchName + 1) LPSTR Name,
    __inout  LPDWORD cchName,
    __out_ecount_part_opt(*cchReferencedDomainName, *cchReferencedDomainName + 1) LPSTR ReferencedDomainName,
    __inout LPDWORD cchReferencedDomainName,
    __out PSID_NAME_USE peUse
    );
WINADVAPI
__success(return != FALSE) BOOL
WINAPI
LookupAccountSidW(
    __in_opt LPCWSTR lpSystemName,
    __in PSID Sid,
    __out_ecount_part_opt(*cchName, *cchName + 1) LPWSTR Name,
    __inout  LPDWORD cchName,
    __out_ecount_part_opt(*cchReferencedDomainName, *cchReferencedDomainName + 1) LPWSTR ReferencedDomainName,
    __inout LPDWORD cchReferencedDomainName,
    __out PSID_NAME_USE peUse
    );
#ifdef UNICODE
#define LookupAccountSid  LookupAccountSidW
#else
#define LookupAccountSid  LookupAccountSidA
#endif // !UNICODE

WINADVAPI
__success(return != FALSE) BOOL
WINAPI
LookupAccountSidLocalA(
    __in PSID Sid,
    __out_ecount_part_opt(*cchName, *cchName + 1) LPSTR Name,
    __inout  LPDWORD cchName,
    __out_ecount_part_opt(*cchReferencedDomainName, *cchReferencedDomainName + 1) LPSTR ReferencedDomainName,
    __inout LPDWORD cchReferencedDomainName,
    __out PSID_NAME_USE peUse
    );
WINADVAPI
__success(return != FALSE) BOOL
WINAPI
LookupAccountSidLocalW(
    __in PSID Sid,
    __out_ecount_part_opt(*cchName, *cchName + 1) LPWSTR Name,
    __inout  LPDWORD cchName,
    __out_ecount_part_opt(*cchReferencedDomainName, *cchReferencedDomainName + 1) LPWSTR ReferencedDomainName,
    __inout LPDWORD cchReferencedDomainName,
    __out PSID_NAME_USE peUse
    );
#ifdef UNICODE
#define LookupAccountSidLocal  LookupAccountSidLocalW
#else
#define LookupAccountSidLocal  LookupAccountSidLocalA
#endif // !UNICODE

WINADVAPI
__success(return != FALSE) BOOL
WINAPI
LookupAccountNameA(
    __in_opt LPCSTR lpSystemName,
    __in     LPCSTR lpAccountName,
    __out_bcount_part_opt(*cbSid, *cbSid) PSID Sid,
    __inout  LPDWORD cbSid,
    __out_ecount_part_opt(*cchReferencedDomainName, *cchReferencedDomainName + 1) LPSTR ReferencedDomainName,
    __inout  LPDWORD cchReferencedDomainName,
    __out    PSID_NAME_USE peUse
    );
WINADVAPI
__success(return != FALSE) BOOL
WINAPI
LookupAccountNameW(
    __in_opt LPCWSTR lpSystemName,
    __in     LPCWSTR lpAccountName,
    __out_bcount_part_opt(*cbSid, *cbSid) PSID Sid,
    __inout  LPDWORD cbSid,
    __out_ecount_part_opt(*cchReferencedDomainName, *cchReferencedDomainName + 1) LPWSTR ReferencedDomainName,
    __inout  LPDWORD cchReferencedDomainName,
    __out    PSID_NAME_USE peUse
    );
#ifdef UNICODE
#define LookupAccountName  LookupAccountNameW
#else
#define LookupAccountName  LookupAccountNameA
#endif // !UNICODE

WINADVAPI
__success(return != FALSE) BOOL
WINAPI
LookupAccountNameLocalA(
    __in     LPCSTR lpAccountName,
    __out_bcount_part_opt(*cbSid, *cbSid) PSID Sid,
    __inout  LPDWORD cbSid,
    __out_ecount_part_opt(*cchReferencedDomainName, *cchReferencedDomainName + 1) LPSTR ReferencedDomainName,
    __inout  LPDWORD cchReferencedDomainName,
    __out    PSID_NAME_USE peUse
    );
WINADVAPI
__success(return != FALSE) BOOL
WINAPI
LookupAccountNameLocalW(
    __in     LPCWSTR lpAccountName,
    __out_bcount_part_opt(*cbSid, *cbSid) PSID Sid,
    __inout  LPDWORD cbSid,
    __out_ecount_part_opt(*cchReferencedDomainName, *cchReferencedDomainName + 1) LPWSTR ReferencedDomainName,
    __inout  LPDWORD cchReferencedDomainName,
    __out    PSID_NAME_USE peUse
    );
#ifdef UNICODE
#define LookupAccountNameLocal  LookupAccountNameLocalW
#else
#define LookupAccountNameLocal  LookupAccountNameLocalA
#endif // !UNICODE

WINADVAPI
BOOL
WINAPI
LookupPrivilegeValueA(
    __in_opt LPCSTR lpSystemName,
    __in     LPCSTR lpName,
    __out    PLUID   lpLuid
    );
WINADVAPI
BOOL
WINAPI
LookupPrivilegeValueW(
    __in_opt LPCWSTR lpSystemName,
    __in     LPCWSTR lpName,
    __out    PLUID   lpLuid
    );
#ifdef UNICODE
#define LookupPrivilegeValue  LookupPrivilegeValueW
#else
#define LookupPrivilegeValue  LookupPrivilegeValueA
#endif // !UNICODE

WINADVAPI
__success(return != FALSE) BOOL
WINAPI
LookupPrivilegeNameA(
    __in_opt LPCSTR lpSystemName,
    __in     PLUID   lpLuid,
    __out_ecount_part_opt(*cchName, *cchName + 1) LPSTR lpName,
    __inout  LPDWORD cchName
    );
WINADVAPI
__success(return != FALSE) BOOL
WINAPI
LookupPrivilegeNameW(
    __in_opt LPCWSTR lpSystemName,
    __in     PLUID   lpLuid,
    __out_ecount_part_opt(*cchName, *cchName + 1) LPWSTR lpName,
    __inout  LPDWORD cchName
    );
#ifdef UNICODE
#define LookupPrivilegeName  LookupPrivilegeNameW
#else
#define LookupPrivilegeName  LookupPrivilegeNameA
#endif // !UNICODE

WINADVAPI
__success(return != FALSE) BOOL
WINAPI
LookupPrivilegeDisplayNameA(
    __in_opt LPCSTR lpSystemName,
    __in     LPCSTR lpName,
    __out_ecount_part_opt(*cchDisplayName, *cchDisplayName + 1) LPSTR lpDisplayName,
    __inout  LPDWORD cchDisplayName,
    __out    LPDWORD lpLanguageId
    );
WINADVAPI
__success(return != FALSE) BOOL
WINAPI
LookupPrivilegeDisplayNameW(
    __in_opt LPCWSTR lpSystemName,
    __in     LPCWSTR lpName,
    __out_ecount_part_opt(*cchDisplayName, *cchDisplayName + 1) LPWSTR lpDisplayName,
    __inout  LPDWORD cchDisplayName,
    __out    LPDWORD lpLanguageId
    );
#ifdef UNICODE
#define LookupPrivilegeDisplayName  LookupPrivilegeDisplayNameW
#else
#define LookupPrivilegeDisplayName  LookupPrivilegeDisplayNameA
#endif // !UNICODE

WINADVAPI
BOOL
WINAPI
AllocateLocallyUniqueId(
    __out PLUID Luid
    );

WINBASEAPI
BOOL
WINAPI
BuildCommDCBA(
    __in  LPCSTR lpDef,
    __out LPDCB lpDCB
    );
WINBASEAPI
BOOL
WINAPI
BuildCommDCBW(
    __in  LPCWSTR lpDef,
    __out LPDCB lpDCB
    );
#ifdef UNICODE
#define BuildCommDCB  BuildCommDCBW
#else
#define BuildCommDCB  BuildCommDCBA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
BuildCommDCBAndTimeoutsA(
    __in  LPCSTR lpDef,
    __out LPDCB lpDCB,
    __out LPCOMMTIMEOUTS lpCommTimeouts
    );
WINBASEAPI
BOOL
WINAPI
BuildCommDCBAndTimeoutsW(
    __in  LPCWSTR lpDef,
    __out LPDCB lpDCB,
    __out LPCOMMTIMEOUTS lpCommTimeouts
    );
#ifdef UNICODE
#define BuildCommDCBAndTimeouts  BuildCommDCBAndTimeoutsW
#else
#define BuildCommDCBAndTimeouts  BuildCommDCBAndTimeoutsA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
CommConfigDialogA(
    __in     LPCSTR lpszName,
    __in_opt HWND hWnd,
    __inout  LPCOMMCONFIG lpCC
    );
WINBASEAPI
BOOL
WINAPI
CommConfigDialogW(
    __in     LPCWSTR lpszName,
    __in_opt HWND hWnd,
    __inout  LPCOMMCONFIG lpCC
    );
#ifdef UNICODE
#define CommConfigDialog  CommConfigDialogW
#else
#define CommConfigDialog  CommConfigDialogA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
GetDefaultCommConfigA(
    __in    LPCSTR lpszName,
    __out_bcount_part(*lpdwSize, *lpdwSize) LPCOMMCONFIG lpCC,
    __inout LPDWORD lpdwSize
    );
WINBASEAPI
BOOL
WINAPI
GetDefaultCommConfigW(
    __in    LPCWSTR lpszName,
    __out_bcount_part(*lpdwSize, *lpdwSize) LPCOMMCONFIG lpCC,
    __inout LPDWORD lpdwSize
    );
#ifdef UNICODE
#define GetDefaultCommConfig  GetDefaultCommConfigW
#else
#define GetDefaultCommConfig  GetDefaultCommConfigA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
SetDefaultCommConfigA(
    __in LPCSTR lpszName,
    __in_bcount(dwSize) LPCOMMCONFIG lpCC,
    __in DWORD dwSize
    );
WINBASEAPI
BOOL
WINAPI
SetDefaultCommConfigW(
    __in LPCWSTR lpszName,
    __in_bcount(dwSize) LPCOMMCONFIG lpCC,
    __in DWORD dwSize
    );
#ifdef UNICODE
#define SetDefaultCommConfig  SetDefaultCommConfigW
#else
#define SetDefaultCommConfig  SetDefaultCommConfigA
#endif // !UNICODE

#ifndef _MAC
#define MAX_COMPUTERNAME_LENGTH 15
#else
#define MAX_COMPUTERNAME_LENGTH 31
#endif

WINBASEAPI
__success(return != 0)
BOOL
WINAPI
GetComputerNameA (
    __out_ecount_part_opt(*nSize, *nSize + 1) LPSTR lpBuffer,
    __inout LPDWORD nSize
    );
WINBASEAPI
__success(return != 0)
BOOL
WINAPI
GetComputerNameW (
    __out_ecount_part_opt(*nSize, *nSize + 1) LPWSTR lpBuffer,
    __inout LPDWORD nSize
    );
#ifdef UNICODE
#define GetComputerName  GetComputerNameW
#else
#define GetComputerName  GetComputerNameA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
SetComputerNameA (
    __in LPCSTR lpComputerName
    );
WINBASEAPI
BOOL
WINAPI
SetComputerNameW (
    __in LPCWSTR lpComputerName
    );
#ifdef UNICODE
#define SetComputerName  SetComputerNameW
#else
#define SetComputerName  SetComputerNameA
#endif // !UNICODE


#if (_WIN32_WINNT >= 0x0500)

typedef enum _COMPUTER_NAME_FORMAT {
    ComputerNameNetBIOS,
    ComputerNameDnsHostname,
    ComputerNameDnsDomain,
    ComputerNameDnsFullyQualified,
    ComputerNamePhysicalNetBIOS,
    ComputerNamePhysicalDnsHostname,
    ComputerNamePhysicalDnsDomain,
    ComputerNamePhysicalDnsFullyQualified,
    ComputerNameMax
} COMPUTER_NAME_FORMAT ;

WINBASEAPI
__success(return != 0)
BOOL
WINAPI
GetComputerNameExA (
    __in    COMPUTER_NAME_FORMAT NameType,
    __out_ecount_part_opt(*nSize, *nSize + 1) LPSTR lpBuffer,
    __inout LPDWORD nSize
    );
WINBASEAPI
__success(return != 0)
BOOL
WINAPI
GetComputerNameExW (
    __in    COMPUTER_NAME_FORMAT NameType,
    __out_ecount_part_opt(*nSize, *nSize + 1) LPWSTR lpBuffer,
    __inout LPDWORD nSize
    );
#ifdef UNICODE
#define GetComputerNameEx  GetComputerNameExW
#else
#define GetComputerNameEx  GetComputerNameExA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
SetComputerNameExA (
    __in COMPUTER_NAME_FORMAT NameType,
    __in LPCSTR lpBuffer
    );
WINBASEAPI
BOOL
WINAPI
SetComputerNameExW (
    __in COMPUTER_NAME_FORMAT NameType,
    __in LPCWSTR lpBuffer
    );
#ifdef UNICODE
#define SetComputerNameEx  SetComputerNameExW
#else
#define SetComputerNameEx  SetComputerNameExA
#endif // !UNICODE


WINBASEAPI
__success(return == TRUE)
BOOL
WINAPI
DnsHostnameToComputerNameA (
    __in    LPCSTR Hostname,
    __out_ecount_part_opt(*nSize, *nSize + 1) LPSTR ComputerName,
    __inout LPDWORD nSize
    );
WINBASEAPI
__success(return == TRUE)
BOOL
WINAPI
DnsHostnameToComputerNameW (
    __in    LPCWSTR Hostname,
    __out_ecount_part_opt(*nSize, *nSize + 1) LPWSTR ComputerName,
    __inout LPDWORD nSize
    );
#ifdef UNICODE
#define DnsHostnameToComputerName  DnsHostnameToComputerNameW
#else
#define DnsHostnameToComputerName  DnsHostnameToComputerNameA
#endif // !UNICODE

#endif // _WIN32_WINNT

WINADVAPI
BOOL
WINAPI
GetUserNameA (
    __out_ecount_part_opt(*pcbBuffer, *pcbBuffer) LPSTR lpBuffer,
    __inout LPDWORD pcbBuffer
    );
WINADVAPI
BOOL
WINAPI
GetUserNameW (
    __out_ecount_part_opt(*pcbBuffer, *pcbBuffer) LPWSTR lpBuffer,
    __inout LPDWORD pcbBuffer
    );
#ifdef UNICODE
#define GetUserName  GetUserNameW
#else
#define GetUserName  GetUserNameA
#endif // !UNICODE

//
// Logon Support APIs
//

#define LOGON32_LOGON_INTERACTIVE       2
#define LOGON32_LOGON_NETWORK           3
#define LOGON32_LOGON_BATCH             4
#define LOGON32_LOGON_SERVICE           5
#define LOGON32_LOGON_UNLOCK            7
#if(_WIN32_WINNT >= 0x0500)
#define LOGON32_LOGON_NETWORK_CLEARTEXT 8
#define LOGON32_LOGON_NEW_CREDENTIALS   9
#endif // (_WIN32_WINNT >= 0x0500)

#define LOGON32_PROVIDER_DEFAULT    0
#define LOGON32_PROVIDER_WINNT35    1
#if(_WIN32_WINNT >= 0x0400)
#define LOGON32_PROVIDER_WINNT40    2
#endif /* _WIN32_WINNT >= 0x0400 */
#if(_WIN32_WINNT >= 0x0500)
#define LOGON32_PROVIDER_WINNT50    3
#endif // (_WIN32_WINNT >= 0x0500)
#if(_WIN32_WINNT >= 0x0600)
#define LOGON32_PROVIDER_VIRTUAL    4
#endif // (_WIN32_WINNT >= 0x0600)



WINADVAPI
BOOL
WINAPI
LogonUserA (
    __in        LPCSTR lpszUsername,
    __in_opt    LPCSTR lpszDomain,
    __in        LPCSTR lpszPassword,
    __in        DWORD dwLogonType,
    __in        DWORD dwLogonProvider,
    __deref_out PHANDLE phToken
    );
WINADVAPI
BOOL
WINAPI
LogonUserW (
    __in        LPCWSTR lpszUsername,
    __in_opt    LPCWSTR lpszDomain,
    __in        LPCWSTR lpszPassword,
    __in        DWORD dwLogonType,
    __in        DWORD dwLogonProvider,
    __deref_out PHANDLE phToken
    );
#ifdef UNICODE
#define LogonUser  LogonUserW
#else
#define LogonUser  LogonUserA
#endif // !UNICODE

WINADVAPI
BOOL
WINAPI
LogonUserExA (
    __in            LPCSTR lpszUsername,
    __in_opt        LPCSTR lpszDomain,
    __in            LPCSTR lpszPassword,
    __in            DWORD dwLogonType,
    __in            DWORD dwLogonProvider,
    __deref_opt_out PHANDLE phToken,
    __deref_opt_out PSID  *ppLogonSid,
    __deref_opt_out_bcount_full(*pdwProfileLength) PVOID *ppProfileBuffer,
    __out_opt       LPDWORD pdwProfileLength,
    __out_opt       PQUOTA_LIMITS pQuotaLimits
    );
WINADVAPI
BOOL
WINAPI
LogonUserExW (
    __in            LPCWSTR lpszUsername,
    __in_opt        LPCWSTR lpszDomain,
    __in            LPCWSTR lpszPassword,
    __in            DWORD dwLogonType,
    __in            DWORD dwLogonProvider,
    __deref_opt_out PHANDLE phToken,
    __deref_opt_out PSID  *ppLogonSid,
    __deref_opt_out_bcount_full(*pdwProfileLength) PVOID *ppProfileBuffer,
    __out_opt       LPDWORD pdwProfileLength,
    __out_opt       PQUOTA_LIMITS pQuotaLimits
    );
#ifdef UNICODE
#define LogonUserEx  LogonUserExW
#else
#define LogonUserEx  LogonUserExA
#endif // !UNICODE


#if(_WIN32_WINNT >= 0x0600)


#endif // (_WIN32_WINNT >= 0x0600)

WINADVAPI
BOOL
WINAPI
ImpersonateLoggedOnUser(
    __in HANDLE  hToken
    );

WINADVAPI
BOOL
WINAPI
CreateProcessAsUserA (
    __in_opt    HANDLE hToken,
    __in_opt    LPCSTR lpApplicationName,
    __inout_opt LPSTR lpCommandLine,
    __in_opt    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    __in_opt    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    __in        BOOL bInheritHandles,
    __in        DWORD dwCreationFlags,
    __in_opt    LPVOID lpEnvironment,
    __in_opt    LPCSTR lpCurrentDirectory,
    __in        LPSTARTUPINFOA lpStartupInfo,
    __out       LPPROCESS_INFORMATION lpProcessInformation
    );
WINADVAPI
BOOL
WINAPI
CreateProcessAsUserW (
    __in_opt    HANDLE hToken,
    __in_opt    LPCWSTR lpApplicationName,
    __inout_opt LPWSTR lpCommandLine,
    __in_opt    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    __in_opt    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    __in        BOOL bInheritHandles,
    __in        DWORD dwCreationFlags,
    __in_opt    LPVOID lpEnvironment,
    __in_opt    LPCWSTR lpCurrentDirectory,
    __in        LPSTARTUPINFOW lpStartupInfo,
    __out       LPPROCESS_INFORMATION lpProcessInformation
    );
#ifdef UNICODE
#define CreateProcessAsUser  CreateProcessAsUserW
#else
#define CreateProcessAsUser  CreateProcessAsUserA
#endif // !UNICODE


#if(_WIN32_WINNT >= 0x0500)

//
// LogonFlags
//
#define LOGON_WITH_PROFILE              0x00000001
#define LOGON_NETCREDENTIALS_ONLY       0x00000002
#define LOGON_ZERO_PASSWORD_BUFFER      0x80000000

WINADVAPI
__checkReturn BOOL
WINAPI
CreateProcessWithLogonW(
    __in        LPCWSTR lpUsername,
    __in_opt    LPCWSTR lpDomain,
    __in        LPCWSTR lpPassword,
    __in        DWORD dwLogonFlags,
    __in_opt    LPCWSTR lpApplicationName,
    __inout_opt LPWSTR lpCommandLine,
    __in        DWORD dwCreationFlags,
    __in_opt    LPVOID lpEnvironment,
    __in_opt    LPCWSTR lpCurrentDirectory,
    __in        LPSTARTUPINFOW lpStartupInfo,
    __out       LPPROCESS_INFORMATION lpProcessInformation
      );

WINADVAPI
__checkReturn BOOL
WINAPI
CreateProcessWithTokenW(
    __in        HANDLE hToken,
    __in        DWORD dwLogonFlags,
    __in_opt    LPCWSTR lpApplicationName,
    __inout_opt LPWSTR lpCommandLine,
    __in        DWORD dwCreationFlags,
    __in_opt    LPVOID lpEnvironment,
    __in_opt    LPCWSTR lpCurrentDirectory,
    __in        LPSTARTUPINFOW lpStartupInfo,
    __out       LPPROCESS_INFORMATION lpProcessInformation
      );

#endif // (_WIN32_WINNT >= 0x0500)

WINADVAPI
BOOL
APIENTRY
ImpersonateAnonymousToken(
    __in HANDLE ThreadHandle
    );

WINADVAPI
BOOL
WINAPI
DuplicateTokenEx(
    __in        HANDLE hExistingToken,
    __in        DWORD dwDesiredAccess,
    __in_opt    LPSECURITY_ATTRIBUTES lpTokenAttributes,
    __in        SECURITY_IMPERSONATION_LEVEL ImpersonationLevel,
    __in        TOKEN_TYPE TokenType,
    __deref_out PHANDLE phNewToken);

WINADVAPI
BOOL
APIENTRY
CreateRestrictedToken(
    __in        HANDLE ExistingTokenHandle,
    __in        DWORD Flags,
    __in        DWORD DisableSidCount,
    __in_ecount_opt(DisableSidCount) PSID_AND_ATTRIBUTES SidsToDisable,
    __in        DWORD DeletePrivilegeCount,
    __in_ecount_opt(DeletePrivilegeCount) PLUID_AND_ATTRIBUTES PrivilegesToDelete,
    __in        DWORD RestrictedSidCount,
    __in_ecount_opt(RestrictedSidCount) PSID_AND_ATTRIBUTES SidsToRestrict,
    __deref_out PHANDLE NewTokenHandle
    );


WINADVAPI
BOOL
WINAPI
IsTokenRestricted(
    __in HANDLE TokenHandle
    );

WINADVAPI
BOOL
WINAPI
IsTokenUntrusted(
    __in HANDLE TokenHandle
    );

WINADVAPI
BOOL
APIENTRY
CheckTokenMembership(
    __in_opt HANDLE TokenHandle,
    __in     PSID SidToCheck,
    __out    PBOOL IsMember
    );

//
// Thread pool API's
//

#if (_WIN32_WINNT >= 0x0500)

typedef WAITORTIMERCALLBACKFUNC WAITORTIMERCALLBACK ;

WINBASEAPI
BOOL
WINAPI
RegisterWaitForSingleObject(
    __deref_out PHANDLE phNewWaitObject,
    __in        HANDLE hObject,
    __in        WAITORTIMERCALLBACK Callback,
    __in_opt    PVOID Context,
    __in        ULONG dwMilliseconds,
    __in        ULONG dwFlags
    );

WINBASEAPI
HANDLE
WINAPI
RegisterWaitForSingleObjectEx(
    __in     HANDLE hObject,
    __in     WAITORTIMERCALLBACK Callback,
    __in_opt PVOID Context,
    __in     ULONG dwMilliseconds,
    __in     ULONG dwFlags
    );

WINBASEAPI
__checkReturn
BOOL
WINAPI
UnregisterWait(
    __in HANDLE WaitHandle
    );

WINBASEAPI
__checkReturn
BOOL
WINAPI
UnregisterWaitEx(
    __in     HANDLE WaitHandle,
    __in_opt HANDLE CompletionEvent
    );

WINBASEAPI
BOOL
WINAPI
QueueUserWorkItem(
    __in     LPTHREAD_START_ROUTINE Function,
    __in_opt PVOID Context,
    __in     ULONG Flags
    );

WINBASEAPI
BOOL
WINAPI
BindIoCompletionCallback (
    __in HANDLE FileHandle,
    __in LPOVERLAPPED_COMPLETION_ROUTINE Function,
    __in ULONG Flags
    );

WINBASEAPI
__out_opt
HANDLE
WINAPI
CreateTimerQueue(
    VOID
    );

WINBASEAPI
BOOL
WINAPI
CreateTimerQueueTimer(
    __deref_out PHANDLE phNewTimer,
    __in_opt    HANDLE TimerQueue,
    __in        WAITORTIMERCALLBACK Callback,
    __in_opt    PVOID Parameter,
    __in        DWORD DueTime,
    __in        DWORD Period,
    __in        ULONG Flags
    ) ;

WINBASEAPI
__checkReturn
BOOL
WINAPI
ChangeTimerQueueTimer(
    __in_opt HANDLE TimerQueue,
    __inout  HANDLE Timer,
    __in     ULONG DueTime,
    __in     ULONG Period
    );

WINBASEAPI
__checkReturn
BOOL
WINAPI
DeleteTimerQueueTimer(
    __in_opt HANDLE TimerQueue,
    __in     HANDLE Timer,
    __in_opt HANDLE CompletionEvent
    );

WINBASEAPI
__checkReturn
BOOL
WINAPI
DeleteTimerQueueEx(
    __in     HANDLE TimerQueue,
    __in_opt HANDLE CompletionEvent
    );

WINBASEAPI
HANDLE
WINAPI
SetTimerQueueTimer(
    __in_opt HANDLE TimerQueue,
    __in     WAITORTIMERCALLBACK Callback,
    __in_opt PVOID Parameter,
    __in     DWORD DueTime,
    __in     DWORD Period,
    __in     BOOL PreferIo
    );

WINBASEAPI
__checkReturn
BOOL
WINAPI
CancelTimerQueueTimer(
    __in_opt HANDLE TimerQueue,
    __in     HANDLE Timer
    );

WINBASEAPI
__checkReturn
BOOL
WINAPI
DeleteTimerQueue(
    __in HANDLE TimerQueue
    );

#if (_WIN32_WINNT >= 0x0600)

typedef VOID (WINAPI *PTP_WIN32_IO_CALLBACK)(
    __inout     PTP_CALLBACK_INSTANCE Instance,
    __inout_opt PVOID                 Context,
    __inout_opt PVOID                 Overlapped,
    __in        ULONG                 IoResult,
    __in        ULONG_PTR             NumberOfBytesTransferred,
    __inout     PTP_IO                Io
    );

WINBASEAPI
__checkReturn
__out
PTP_POOL
WINAPI
CreateThreadpool(
    __reserved PVOID reserved
    );

WINBASEAPI
VOID
WINAPI
SetThreadpoolThreadMaximum(
    __inout PTP_POOL ptpp,
    __in    DWORD    cthrdMost
    );

WINBASEAPI
BOOL
WINAPI
SetThreadpoolThreadMinimum(
    __inout PTP_POOL ptpp,
    __in    DWORD    cthrdMic
    );

WINBASEAPI
BOOL
WINAPI
SetThreadpoolStackInformation(
    __inout PTP_POOL           ptpp,
    __in    PTP_POOL_STACK_INFORMATION ptpsi
    );

WINBASEAPI
BOOL
WINAPI
QueryThreadpoolStackInformation(
    __in    PTP_POOL           ptpp,
    __out   PTP_POOL_STACK_INFORMATION ptpsi
    );

WINBASEAPI
VOID
WINAPI
CloseThreadpool(
    __inout PTP_POOL ptpp
    );

WINBASEAPI
__checkReturn
__out
PTP_CLEANUP_GROUP
WINAPI
CreateThreadpoolCleanupGroup(
    VOID
    );

WINBASEAPI
VOID
WINAPI
CloseThreadpoolCleanupGroupMembers(
    __inout     PTP_CLEANUP_GROUP ptpcg,
    __in        BOOL              fCancelPendingCallbacks,
    __inout_opt PVOID             pvCleanupContext
    );

WINBASEAPI
VOID
WINAPI
CloseThreadpoolCleanupGroup(
    __inout PTP_CLEANUP_GROUP ptpcg
    );

#if !defined(MIDL_PASS)

FORCEINLINE
VOID
InitializeThreadpoolEnvironment(
    __out PTP_CALLBACK_ENVIRON pcbe
    )
{
    TpInitializeCallbackEnviron(pcbe);
}

FORCEINLINE
VOID
SetThreadpoolCallbackPool(
    __inout PTP_CALLBACK_ENVIRON pcbe,
    __in    PTP_POOL             ptpp
    )
{
    TpSetCallbackThreadpool(pcbe, ptpp);
}

FORCEINLINE
VOID
SetThreadpoolCallbackCleanupGroup(
    __inout  PTP_CALLBACK_ENVIRON              pcbe,
    __in     PTP_CLEANUP_GROUP                 ptpcg,
    __in_opt PTP_CLEANUP_GROUP_CANCEL_CALLBACK pfng
    )
{
    TpSetCallbackCleanupGroup(pcbe, ptpcg, pfng);
}

FORCEINLINE
VOID
SetThreadpoolCallbackRunsLong(
    __inout PTP_CALLBACK_ENVIRON pcbe
    )
{
    TpSetCallbackLongFunction(pcbe);
}

FORCEINLINE
VOID
SetThreadpoolCallbackLibrary(
    __inout PTP_CALLBACK_ENVIRON pcbe,
    __in    PVOID                mod
    )
{
    TpSetCallbackRaceWithDll(pcbe, mod);
}

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN7)

FORCEINLINE
VOID
SetThreadpoolCallbackPriority(
    __inout PTP_CALLBACK_ENVIRON pcbe,
    __in    TP_CALLBACK_PRIORITY Priority
    )
{
    TpSetCallbackPriority(pcbe, Priority);
}

#endif

FORCEINLINE
VOID
SetThreadpoolCallbackPersistent(
    __inout PTP_CALLBACK_ENVIRON pcbe
    )
{
    TpSetCallbackPersistent(pcbe);
}

FORCEINLINE
VOID
DestroyThreadpoolEnvironment(
    __inout PTP_CALLBACK_ENVIRON pcbe
    )
{
    TpDestroyCallbackEnviron(pcbe);
}

#endif // !defined(MIDL_PASS)

WINBASEAPI
VOID
WINAPI
SetEventWhenCallbackReturns(
    __inout PTP_CALLBACK_INSTANCE pci,
    __in    HANDLE                evt
    );

WINBASEAPI
VOID
WINAPI
ReleaseSemaphoreWhenCallbackReturns(
    __inout PTP_CALLBACK_INSTANCE pci,
    __in    HANDLE                sem,
    __in    DWORD                 crel
    );

WINBASEAPI
VOID
WINAPI
ReleaseMutexWhenCallbackReturns(
    __inout PTP_CALLBACK_INSTANCE pci,
    __in    HANDLE                mut
    );

WINBASEAPI
VOID
WINAPI
LeaveCriticalSectionWhenCallbackReturns(
    __inout PTP_CALLBACK_INSTANCE pci,
    __inout PCRITICAL_SECTION     pcs
    );

WINBASEAPI
VOID
WINAPI
FreeLibraryWhenCallbackReturns(
    __inout PTP_CALLBACK_INSTANCE pci,
    __in    HMODULE               mod
    );

WINBASEAPI
BOOL
WINAPI
CallbackMayRunLong(
    __inout PTP_CALLBACK_INSTANCE pci
    );

WINBASEAPI
VOID
WINAPI
DisassociateCurrentThreadFromCallback(
    __inout PTP_CALLBACK_INSTANCE pci
    );

WINBASEAPI
__checkReturn
BOOL
WINAPI
TrySubmitThreadpoolCallback(
    __in        PTP_SIMPLE_CALLBACK  pfns,
    __inout_opt PVOID                pv,
    __in_opt    PTP_CALLBACK_ENVIRON pcbe
    );

WINBASEAPI
__checkReturn
__out
PTP_WORK
WINAPI
CreateThreadpoolWork(
    __in        PTP_WORK_CALLBACK    pfnwk,
    __inout_opt PVOID                pv,
    __in_opt    PTP_CALLBACK_ENVIRON pcbe
    );

WINBASEAPI
VOID
WINAPI
SubmitThreadpoolWork(
    __inout PTP_WORK pwk
    );

WINBASEAPI
VOID
WINAPI
WaitForThreadpoolWorkCallbacks(
    __inout PTP_WORK pwk,
    __in    BOOL     fCancelPendingCallbacks
    );

WINBASEAPI
VOID
WINAPI
CloseThreadpoolWork(
    __inout PTP_WORK pwk
    );

WINBASEAPI
__checkReturn
__out
PTP_TIMER
WINAPI
CreateThreadpoolTimer(
    __in        PTP_TIMER_CALLBACK   pfnti,
    __inout_opt PVOID                pv,
    __in_opt    PTP_CALLBACK_ENVIRON pcbe
    );

WINBASEAPI
VOID
WINAPI
SetThreadpoolTimer(
    __inout  PTP_TIMER pti,
    __in_opt PFILETIME pftDueTime,
    __in     DWORD     msPeriod,
    __in_opt DWORD     msWindowLength
    );

WINBASEAPI
BOOL
WINAPI
IsThreadpoolTimerSet(
    __inout PTP_TIMER pti
    );

WINBASEAPI
VOID
WINAPI
WaitForThreadpoolTimerCallbacks(
    __inout PTP_TIMER pti,
    __in    BOOL      fCancelPendingCallbacks
    );

WINBASEAPI
VOID
WINAPI
CloseThreadpoolTimer(
    __inout PTP_TIMER pti
    );

WINBASEAPI
__checkReturn
__out
PTP_WAIT
WINAPI
CreateThreadpoolWait(
    __in        PTP_WAIT_CALLBACK    pfnwa,
    __inout_opt PVOID                pv,
    __in_opt    PTP_CALLBACK_ENVIRON pcbe
    );

WINBASEAPI
VOID
WINAPI
SetThreadpoolWait(
    __inout  PTP_WAIT  pwa,
    __in_opt HANDLE    h,
    __in_opt PFILETIME pftTimeout
    );

WINBASEAPI
VOID
WINAPI
WaitForThreadpoolWaitCallbacks(
    __inout PTP_WAIT pwa,
    __in    BOOL     fCancelPendingCallbacks
    );

WINBASEAPI
VOID
WINAPI
CloseThreadpoolWait(
    __inout PTP_WAIT pwa
    );

WINBASEAPI
__checkReturn
__out
PTP_IO
WINAPI
CreateThreadpoolIo(
    __in        HANDLE                fl,
    __in        PTP_WIN32_IO_CALLBACK pfnio,
    __inout_opt PVOID                 pv,
    __in_opt    PTP_CALLBACK_ENVIRON  pcbe
    );

WINBASEAPI
VOID
WINAPI
StartThreadpoolIo(
    __inout PTP_IO pio
    );

WINBASEAPI
VOID
WINAPI
CancelThreadpoolIo(
    __inout PTP_IO pio
    );

WINBASEAPI
VOID
WINAPI
WaitForThreadpoolIoCallbacks(
    __inout PTP_IO pio,
    __in    BOOL   fCancelPendingCallbacks
    );

WINBASEAPI
VOID
WINAPI
CloseThreadpoolIo(
    __inout PTP_IO pio
    );

//
//  Private Namespaces support
//

WINBASEAPI
__out_opt
HANDLE
WINAPI
CreatePrivateNamespaceA(
    __in_opt LPSECURITY_ATTRIBUTES lpPrivateNamespaceAttributes,
    __in     LPVOID lpBoundaryDescriptor,
    __in     LPCSTR lpAliasPrefix
    );
WINBASEAPI
__out_opt
HANDLE
WINAPI
CreatePrivateNamespaceW(
    __in_opt LPSECURITY_ATTRIBUTES lpPrivateNamespaceAttributes,
    __in     LPVOID lpBoundaryDescriptor,
    __in     LPCWSTR lpAliasPrefix
    );
#ifdef UNICODE
#define CreatePrivateNamespace  CreatePrivateNamespaceW
#else
#define CreatePrivateNamespace  CreatePrivateNamespaceA
#endif // !UNICODE

WINBASEAPI
__out_opt
HANDLE
WINAPI
OpenPrivateNamespaceA(
    __in     LPVOID lpBoundaryDescriptor,
    __in     LPCSTR lpAliasPrefix
    );
WINBASEAPI
__out_opt
HANDLE
WINAPI
OpenPrivateNamespaceW(
    __in     LPVOID lpBoundaryDescriptor,
    __in     LPCWSTR lpAliasPrefix
    );
#ifdef UNICODE
#define OpenPrivateNamespace  OpenPrivateNamespaceW
#else
#define OpenPrivateNamespace  OpenPrivateNamespaceA
#endif // !UNICODE


#define PRIVATE_NAMESPACE_FLAG_DESTROY      0x00000001

WINBASEAPI
BOOLEAN
WINAPI
ClosePrivateNamespace(
    __in HANDLE Handle,
    __in ULONG Flags
    );


//
//  Boundary descriptors support
//

WINBASEAPI
__out_opt
HANDLE
APIENTRY
CreateBoundaryDescriptorA(
    __in LPCSTR Name,
    __in ULONG Flags
    );
WINBASEAPI
__out_opt
HANDLE
APIENTRY
CreateBoundaryDescriptorW(
    __in LPCWSTR Name,
    __in ULONG Flags
    );
#ifdef UNICODE
#define CreateBoundaryDescriptor  CreateBoundaryDescriptorW
#else
#define CreateBoundaryDescriptor  CreateBoundaryDescriptorA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
AddSIDToBoundaryDescriptor(
    __inout HANDLE * BoundaryDescriptor,
    __in PSID RequiredSid
    );

WINBASEAPI
BOOL
WINAPI
AddIntegrityLabelToBoundaryDescriptor(
    __inout HANDLE * BoundaryDescriptor,
    __in PSID IntegrityLabel
    );


WINBASEAPI
VOID
WINAPI
DeleteBoundaryDescriptor(
    __in HANDLE BoundaryDescriptor
    );


#endif // _WIN32_WINNT >= 0x0600

#endif // _WIN32_WINNT >= 0x0500


#if(_WIN32_WINNT >= 0x0400)
//
// Plug-and-Play API's
//

#define HW_PROFILE_GUIDLEN         39      // 36-characters plus NULL terminator
#define MAX_PROFILE_LEN            80

#define DOCKINFO_UNDOCKED          (0x1)
#define DOCKINFO_DOCKED            (0x2)
#define DOCKINFO_USER_SUPPLIED     (0x4)
#define DOCKINFO_USER_UNDOCKED     (DOCKINFO_USER_SUPPLIED | DOCKINFO_UNDOCKED)
#define DOCKINFO_USER_DOCKED       (DOCKINFO_USER_SUPPLIED | DOCKINFO_DOCKED)

typedef struct tagHW_PROFILE_INFOA {
    DWORD  dwDockInfo;
    CHAR   szHwProfileGuid[HW_PROFILE_GUIDLEN];
    CHAR   szHwProfileName[MAX_PROFILE_LEN];
} HW_PROFILE_INFOA, *LPHW_PROFILE_INFOA;
typedef struct tagHW_PROFILE_INFOW {
    DWORD  dwDockInfo;
    WCHAR  szHwProfileGuid[HW_PROFILE_GUIDLEN];
    WCHAR  szHwProfileName[MAX_PROFILE_LEN];
} HW_PROFILE_INFOW, *LPHW_PROFILE_INFOW;
#ifdef UNICODE
typedef HW_PROFILE_INFOW HW_PROFILE_INFO;
typedef LPHW_PROFILE_INFOW LPHW_PROFILE_INFO;
#else
typedef HW_PROFILE_INFOA HW_PROFILE_INFO;
typedef LPHW_PROFILE_INFOA LPHW_PROFILE_INFO;
#endif // UNICODE


WINADVAPI
BOOL
WINAPI
GetCurrentHwProfileA (
    __out LPHW_PROFILE_INFOA  lpHwProfileInfo
    );
WINADVAPI
BOOL
WINAPI
GetCurrentHwProfileW (
    __out LPHW_PROFILE_INFOW  lpHwProfileInfo
    );
#ifdef UNICODE
#define GetCurrentHwProfile  GetCurrentHwProfileW
#else
#define GetCurrentHwProfile  GetCurrentHwProfileA
#endif // !UNICODE
#endif /* _WIN32_WINNT >= 0x0400 */

//
// Performance counter API's
//

WINBASEAPI
BOOL
WINAPI
QueryPerformanceCounter(
    __out LARGE_INTEGER *lpPerformanceCount
    );

WINBASEAPI
BOOL
WINAPI
QueryPerformanceFrequency(
    __out LARGE_INTEGER *lpFrequency
    );



WINBASEAPI
BOOL
WINAPI
GetVersionExA(
    __inout LPOSVERSIONINFOA lpVersionInformation
    );
WINBASEAPI
BOOL
WINAPI
GetVersionExW(
    __inout LPOSVERSIONINFOW lpVersionInformation
    );
#ifdef UNICODE
#define GetVersionEx  GetVersionExW
#else
#define GetVersionEx  GetVersionExA
#endif // !UNICODE



WINBASEAPI
BOOL
WINAPI
VerifyVersionInfoA(
    __inout LPOSVERSIONINFOEXA lpVersionInformation,
    __in    DWORD dwTypeMask,
    __in    DWORDLONG dwlConditionMask
    );
WINBASEAPI
BOOL
WINAPI
VerifyVersionInfoW(
    __inout LPOSVERSIONINFOEXW lpVersionInformation,
    __in    DWORD dwTypeMask,
    __in    DWORDLONG dwlConditionMask
    );
#ifdef UNICODE
#define VerifyVersionInfo  VerifyVersionInfoW
#else
#define VerifyVersionInfo  VerifyVersionInfoA
#endif // !UNICODE

#if (_WIN32_WINNT >= 0x0600)

WINBASEAPI
BOOL
WINAPI
GetProductInfo(
    __in  DWORD  dwOSMajorVersion,
    __in  DWORD  dwOSMinorVersion,
    __in  DWORD  dwSpMajorVersion,
    __in  DWORD  dwSpMinorVersion,
    __out PDWORD pdwReturnedProductType
    );

#endif

// DOS and OS/2 Compatible Error Code definitions returned by the Win32 Base
// API functions.
//

#include <winerror.h>

/* Abnormal termination codes */

#define TC_NORMAL       0
#define TC_HARDERR      1
#define TC_GP_TRAP      2
#define TC_SIGNAL       3

#if(WINVER >= 0x0400)
//
// Power Management APIs
//

#define AC_LINE_OFFLINE                 0x00
#define AC_LINE_ONLINE                  0x01
#define AC_LINE_BACKUP_POWER            0x02
#define AC_LINE_UNKNOWN                 0xFF

#define BATTERY_FLAG_HIGH               0x01
#define BATTERY_FLAG_LOW                0x02
#define BATTERY_FLAG_CRITICAL           0x04
#define BATTERY_FLAG_CHARGING           0x08
#define BATTERY_FLAG_NO_BATTERY         0x80
#define BATTERY_FLAG_UNKNOWN            0xFF

#define BATTERY_PERCENTAGE_UNKNOWN      0xFF

#define BATTERY_LIFE_UNKNOWN        0xFFFFFFFF

typedef struct _SYSTEM_POWER_STATUS {
    BYTE ACLineStatus;
    BYTE BatteryFlag;
    BYTE BatteryLifePercent;
    BYTE Reserved1;
    DWORD BatteryLifeTime;
    DWORD BatteryFullLifeTime;
}   SYSTEM_POWER_STATUS, *LPSYSTEM_POWER_STATUS;

WINBASEAPI
BOOL
WINAPI
GetSystemPowerStatus(
    __out LPSYSTEM_POWER_STATUS lpSystemPowerStatus
    );

WINBASEAPI
BOOL
WINAPI
SetSystemPowerState(
    __in BOOL fSuspend,
    __in BOOL fForce
    );

#endif /* WINVER >= 0x0400 */

#if (_WIN32_WINNT >= 0x0500)
//
// Very Large Memory API Subset
//

WINBASEAPI
BOOL
WINAPI
AllocateUserPhysicalPages(
    __in    HANDLE hProcess,
    __inout PULONG_PTR NumberOfPages,
    __out_ecount_part(*NumberOfPages, *NumberOfPages) PULONG_PTR PageArray
    );

#if _WIN32_WINNT >= 0x0600

WINBASEAPI
BOOL
WINAPI
AllocateUserPhysicalPagesNuma(
    __in    HANDLE hProcess,
    __inout PULONG_PTR NumberOfPages,
    __out_ecount_part(*NumberOfPages, *NumberOfPages) PULONG_PTR PageArray,
    __in    DWORD nndPreferred
    );

#endif // _WIN32_WINNT >= 0x0600

WINBASEAPI
BOOL
WINAPI
FreeUserPhysicalPages(
    __in    HANDLE hProcess,
    __inout PULONG_PTR NumberOfPages,
    __in_ecount(*NumberOfPages) PULONG_PTR PageArray
    );

WINBASEAPI
BOOL
WINAPI
MapUserPhysicalPages(
    __in PVOID VirtualAddress,
    __in ULONG_PTR NumberOfPages,
    __in_ecount_opt(NumberOfPages) PULONG_PTR PageArray
    );

WINBASEAPI
BOOL
WINAPI
MapUserPhysicalPagesScatter(
    __in_ecount(NumberOfPages) PVOID *VirtualAddresses,
    __in ULONG_PTR NumberOfPages,
    __in_ecount_opt(NumberOfPages) PULONG_PTR PageArray
    );

WINBASEAPI
__out_opt
HANDLE
WINAPI
CreateJobObjectA(
    __in_opt LPSECURITY_ATTRIBUTES lpJobAttributes,
    __in_opt LPCSTR lpName
    );
WINBASEAPI
__out_opt
HANDLE
WINAPI
CreateJobObjectW(
    __in_opt LPSECURITY_ATTRIBUTES lpJobAttributes,
    __in_opt LPCWSTR lpName
    );
#ifdef UNICODE
#define CreateJobObject  CreateJobObjectW
#else
#define CreateJobObject  CreateJobObjectA
#endif // !UNICODE

WINBASEAPI
__out_opt
HANDLE
WINAPI
OpenJobObjectA(
    __in DWORD dwDesiredAccess,
    __in BOOL bInheritHandle,
    __in LPCSTR lpName
    );
WINBASEAPI
__out_opt
HANDLE
WINAPI
OpenJobObjectW(
    __in DWORD dwDesiredAccess,
    __in BOOL bInheritHandle,
    __in LPCWSTR lpName
    );
#ifdef UNICODE
#define OpenJobObject  OpenJobObjectW
#else
#define OpenJobObject  OpenJobObjectA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
AssignProcessToJobObject(
    __in HANDLE hJob,
    __in HANDLE hProcess
    );

WINBASEAPI
BOOL
WINAPI
TerminateJobObject(
    __in HANDLE hJob,
    __in UINT uExitCode
    );

WINBASEAPI
BOOL
WINAPI
QueryInformationJobObject(
    __in_opt  HANDLE hJob,
    __in      JOBOBJECTINFOCLASS JobObjectInformationClass,
    __out_bcount_part(cbJobObjectInformationLength, *lpReturnLength) LPVOID lpJobObjectInformation,
    __in      DWORD cbJobObjectInformationLength,
    __out_opt LPDWORD lpReturnLength
    );

WINBASEAPI
BOOL
WINAPI
SetInformationJobObject(
    __in HANDLE hJob,
    __in JOBOBJECTINFOCLASS JobObjectInformationClass,
    __in_bcount(cbJobObjectInformationLength) LPVOID lpJobObjectInformation,
    __in DWORD cbJobObjectInformationLength
    );

#if (_WIN32_WINNT >= 0x0501)

WINBASEAPI
BOOL
WINAPI
IsProcessInJob (
    __in     HANDLE ProcessHandle,
    __in_opt HANDLE JobHandle,
    __out    PBOOL Result
    );

#endif

WINBASEAPI
BOOL
WINAPI
CreateJobSet (
    __in ULONG NumJob,
    __in_ecount(NumJob) PJOB_SET_ARRAY UserJobSet,
    __in ULONG Flags);

WINBASEAPI
__out_opt
PVOID
WINAPI
AddVectoredExceptionHandler (
    __in ULONG First,
    __in PVECTORED_EXCEPTION_HANDLER Handler
    );

WINBASEAPI
ULONG
WINAPI
RemoveVectoredExceptionHandler (
    __in PVOID Handle
    );

WINBASEAPI
__out_opt
PVOID
WINAPI
AddVectoredContinueHandler (
    __in ULONG First,
    __in PVECTORED_EXCEPTION_HANDLER Handler
    );

WINBASEAPI
ULONG
WINAPI
RemoveVectoredContinueHandler (
    __in PVOID Handle
    );

//
// New Volume Mount Point API.
//

WINBASEAPI
__out
HANDLE
WINAPI
FindFirstVolumeA(
    __out_ecount(cchBufferLength) LPSTR lpszVolumeName,
    __in DWORD cchBufferLength
    );
WINBASEAPI
__out
HANDLE
WINAPI
FindFirstVolumeW(
    __out_ecount(cchBufferLength) LPWSTR lpszVolumeName,
    __in DWORD cchBufferLength
    );
#ifdef UNICODE
#define FindFirstVolume FindFirstVolumeW
#else
#define FindFirstVolume FindFirstVolumeA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
FindNextVolumeA(
    __inout HANDLE hFindVolume,
    __out_ecount(cchBufferLength) LPSTR lpszVolumeName,
    __in    DWORD cchBufferLength
    );
WINBASEAPI
BOOL
WINAPI
FindNextVolumeW(
    __inout HANDLE hFindVolume,
    __out_ecount(cchBufferLength) LPWSTR lpszVolumeName,
    __in    DWORD cchBufferLength
    );
#ifdef UNICODE
#define FindNextVolume FindNextVolumeW
#else
#define FindNextVolume FindNextVolumeA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
FindVolumeClose(
    __in HANDLE hFindVolume
    );

WINBASEAPI
__out
HANDLE
WINAPI
FindFirstVolumeMountPointA(
    __in LPCSTR lpszRootPathName,
    __out_ecount(cchBufferLength) LPSTR lpszVolumeMountPoint,
    __in DWORD cchBufferLength
    );
WINBASEAPI
__out
HANDLE
WINAPI
FindFirstVolumeMountPointW(
    __in LPCWSTR lpszRootPathName,
    __out_ecount(cchBufferLength) LPWSTR lpszVolumeMountPoint,
    __in DWORD cchBufferLength
    );
#ifdef UNICODE
#define FindFirstVolumeMountPoint FindFirstVolumeMountPointW
#else
#define FindFirstVolumeMountPoint FindFirstVolumeMountPointA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
FindNextVolumeMountPointA(
    __in HANDLE hFindVolumeMountPoint,
    __out_ecount(cchBufferLength) LPSTR lpszVolumeMountPoint,
    __in DWORD cchBufferLength
    );
WINBASEAPI
BOOL
WINAPI
FindNextVolumeMountPointW(
    __in HANDLE hFindVolumeMountPoint,
    __out_ecount(cchBufferLength) LPWSTR lpszVolumeMountPoint,
    __in DWORD cchBufferLength
    );
#ifdef UNICODE
#define FindNextVolumeMountPoint FindNextVolumeMountPointW
#else
#define FindNextVolumeMountPoint FindNextVolumeMountPointA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
FindVolumeMountPointClose(
    __in HANDLE hFindVolumeMountPoint
    );

WINBASEAPI
BOOL
WINAPI
SetVolumeMountPointA(
    __in LPCSTR lpszVolumeMountPoint,
    __in LPCSTR lpszVolumeName
    );
WINBASEAPI
BOOL
WINAPI
SetVolumeMountPointW(
    __in LPCWSTR lpszVolumeMountPoint,
    __in LPCWSTR lpszVolumeName
    );
#ifdef UNICODE
#define SetVolumeMountPoint  SetVolumeMountPointW
#else
#define SetVolumeMountPoint  SetVolumeMountPointA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
DeleteVolumeMountPointA(
    __in LPCSTR lpszVolumeMountPoint
    );
WINBASEAPI
BOOL
WINAPI
DeleteVolumeMountPointW(
    __in LPCWSTR lpszVolumeMountPoint
    );
#ifdef UNICODE
#define DeleteVolumeMountPoint  DeleteVolumeMountPointW
#else
#define DeleteVolumeMountPoint  DeleteVolumeMountPointA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
GetVolumeNameForVolumeMountPointA(
    __in LPCSTR lpszVolumeMountPoint,
    __out_ecount(cchBufferLength) LPSTR lpszVolumeName,
    __in DWORD cchBufferLength
    );
WINBASEAPI
BOOL
WINAPI
GetVolumeNameForVolumeMountPointW(
    __in LPCWSTR lpszVolumeMountPoint,
    __out_ecount(cchBufferLength) LPWSTR lpszVolumeName,
    __in DWORD cchBufferLength
    );
#ifdef UNICODE
#define GetVolumeNameForVolumeMountPoint  GetVolumeNameForVolumeMountPointW
#else
#define GetVolumeNameForVolumeMountPoint  GetVolumeNameForVolumeMountPointA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
GetVolumePathNameA(
    __in LPCSTR lpszFileName,
    __out_ecount(cchBufferLength) LPSTR lpszVolumePathName,
    __in DWORD cchBufferLength
    );
WINBASEAPI
BOOL
WINAPI
GetVolumePathNameW(
    __in LPCWSTR lpszFileName,
    __out_ecount(cchBufferLength) LPWSTR lpszVolumePathName,
    __in DWORD cchBufferLength
    );
#ifdef UNICODE
#define GetVolumePathName  GetVolumePathNameW
#else
#define GetVolumePathName  GetVolumePathNameA
#endif // !UNICODE

#endif

#if(_WIN32_WINNT >= 0x0501)

WINBASEAPI
BOOL
WINAPI
GetVolumePathNamesForVolumeNameA(
    __in  LPCSTR lpszVolumeName,
    __out_ecount_part_opt(cchBufferLength, *lpcchReturnLength) __nullnullterminated LPCH lpszVolumePathNames,
    __in  DWORD cchBufferLength,
    __out PDWORD lpcchReturnLength
    );
WINBASEAPI
BOOL
WINAPI
GetVolumePathNamesForVolumeNameW(
    __in  LPCWSTR lpszVolumeName,
    __out_ecount_part_opt(cchBufferLength, *lpcchReturnLength) __nullnullterminated LPWCH lpszVolumePathNames,
    __in  DWORD cchBufferLength,
    __out PDWORD lpcchReturnLength
    );
#ifdef UNICODE
#define GetVolumePathNamesForVolumeName  GetVolumePathNamesForVolumeNameW
#else
#define GetVolumePathNamesForVolumeName  GetVolumePathNamesForVolumeNameA
#endif // !UNICODE

#endif // (_WIN32_WINNT >= 0x0501)

#if (_WIN32_WINNT >= 0x0500) || (_WIN32_FUSION >= 0x0100) || ISOLATION_AWARE_ENABLED

#define ACTCTX_FLAG_PROCESSOR_ARCHITECTURE_VALID    (0x00000001)
#define ACTCTX_FLAG_LANGID_VALID                    (0x00000002)
#define ACTCTX_FLAG_ASSEMBLY_DIRECTORY_VALID        (0x00000004)
#define ACTCTX_FLAG_RESOURCE_NAME_VALID             (0x00000008)
#define ACTCTX_FLAG_SET_PROCESS_DEFAULT             (0x00000010)
#define ACTCTX_FLAG_APPLICATION_NAME_VALID          (0x00000020)
#define ACTCTX_FLAG_SOURCE_IS_ASSEMBLYREF           (0x00000040)
#define ACTCTX_FLAG_HMODULE_VALID                   (0x00000080)

typedef struct tagACTCTXA {
    ULONG       cbSize;
    DWORD       dwFlags;
    LPCSTR      lpSource;
    USHORT      wProcessorArchitecture;
    LANGID      wLangId;
    LPCSTR      lpAssemblyDirectory;
    LPCSTR      lpResourceName;
    LPCSTR      lpApplicationName;
    HMODULE     hModule;
} ACTCTXA, *PACTCTXA;
typedef struct tagACTCTXW {
    ULONG       cbSize;
    DWORD       dwFlags;
    LPCWSTR     lpSource;
    USHORT      wProcessorArchitecture;
    LANGID      wLangId;
    LPCWSTR     lpAssemblyDirectory;
    LPCWSTR     lpResourceName;
    LPCWSTR     lpApplicationName;
    HMODULE     hModule;
} ACTCTXW, *PACTCTXW;
#ifdef UNICODE
typedef ACTCTXW ACTCTX;
typedef PACTCTXW PACTCTX;
#else
typedef ACTCTXA ACTCTX;
typedef PACTCTXA PACTCTX;
#endif // UNICODE

typedef const ACTCTXA *PCACTCTXA;
typedef const ACTCTXW *PCACTCTXW;
#ifdef UNICODE
typedef PCACTCTXW PCACTCTX;
#else
typedef PCACTCTXA PCACTCTX;
#endif // UNICODE



WINBASEAPI
__out
HANDLE
WINAPI
CreateActCtxA(
    __in PCACTCTXA pActCtx
    );
WINBASEAPI
__out
HANDLE
WINAPI
CreateActCtxW(
    __in PCACTCTXW pActCtx
    );
#ifdef UNICODE
#define CreateActCtx  CreateActCtxW
#else
#define CreateActCtx  CreateActCtxA
#endif // !UNICODE

WINBASEAPI
VOID
WINAPI
AddRefActCtx(
    __inout HANDLE hActCtx
    );


WINBASEAPI
VOID
WINAPI
ReleaseActCtx(
    __inout HANDLE hActCtx
    );

WINBASEAPI
BOOL
WINAPI
ZombifyActCtx(
    __inout HANDLE hActCtx
    );


WINBASEAPI
BOOL
WINAPI
ActivateActCtx(
    __inout_opt HANDLE hActCtx,
    __out   ULONG_PTR *lpCookie
    );


#define DEACTIVATE_ACTCTX_FLAG_FORCE_EARLY_DEACTIVATION (0x00000001)

WINBASEAPI
BOOL
WINAPI
DeactivateActCtx(
    __in DWORD dwFlags,
    __in ULONG_PTR ulCookie
    );

WINBASEAPI
BOOL
WINAPI
GetCurrentActCtx(
    __deref_out HANDLE *lphActCtx);


typedef struct tagACTCTX_SECTION_KEYED_DATA_2600 {
    ULONG cbSize;
    ULONG ulDataFormatVersion;
    PVOID lpData;
    ULONG ulLength;
    PVOID lpSectionGlobalData;
    ULONG ulSectionGlobalDataLength;
    PVOID lpSectionBase;
    ULONG ulSectionTotalLength;
    HANDLE hActCtx;
    ULONG ulAssemblyRosterIndex;
} ACTCTX_SECTION_KEYED_DATA_2600, *PACTCTX_SECTION_KEYED_DATA_2600;
typedef const ACTCTX_SECTION_KEYED_DATA_2600 * PCACTCTX_SECTION_KEYED_DATA_2600;

typedef struct tagACTCTX_SECTION_KEYED_DATA_ASSEMBLY_METADATA {
    PVOID lpInformation;
    PVOID lpSectionBase;
    ULONG ulSectionLength;
    PVOID lpSectionGlobalDataBase;
    ULONG ulSectionGlobalDataLength;
} ACTCTX_SECTION_KEYED_DATA_ASSEMBLY_METADATA, *PACTCTX_SECTION_KEYED_DATA_ASSEMBLY_METADATA;
typedef const ACTCTX_SECTION_KEYED_DATA_ASSEMBLY_METADATA *PCACTCTX_SECTION_KEYED_DATA_ASSEMBLY_METADATA;

typedef struct tagACTCTX_SECTION_KEYED_DATA {
    ULONG cbSize;
    ULONG ulDataFormatVersion;
    PVOID lpData;
    ULONG ulLength;
    PVOID lpSectionGlobalData;
    ULONG ulSectionGlobalDataLength;
    PVOID lpSectionBase;
    ULONG ulSectionTotalLength;
    HANDLE hActCtx;
    ULONG ulAssemblyRosterIndex;
// 2600 stops here
    ULONG ulFlags;
    ACTCTX_SECTION_KEYED_DATA_ASSEMBLY_METADATA AssemblyMetadata;
} ACTCTX_SECTION_KEYED_DATA, *PACTCTX_SECTION_KEYED_DATA;
typedef const ACTCTX_SECTION_KEYED_DATA * PCACTCTX_SECTION_KEYED_DATA;

#define FIND_ACTCTX_SECTION_KEY_RETURN_HACTCTX (0x00000001)
#define FIND_ACTCTX_SECTION_KEY_RETURN_FLAGS   (0x00000002)
#define FIND_ACTCTX_SECTION_KEY_RETURN_ASSEMBLY_METADATA (0x00000004)



WINBASEAPI
BOOL
WINAPI
FindActCtxSectionStringA(
    __in       DWORD dwFlags,
    __reserved const GUID *lpExtensionGuid,
    __in       ULONG ulSectionId,
    __in       LPCSTR lpStringToFind,
    __out      PACTCTX_SECTION_KEYED_DATA ReturnedData
    );
WINBASEAPI
BOOL
WINAPI
FindActCtxSectionStringW(
    __in       DWORD dwFlags,
    __reserved const GUID *lpExtensionGuid,
    __in       ULONG ulSectionId,
    __in       LPCWSTR lpStringToFind,
    __out      PACTCTX_SECTION_KEYED_DATA ReturnedData
    );
#ifdef UNICODE
#define FindActCtxSectionString  FindActCtxSectionStringW
#else
#define FindActCtxSectionString  FindActCtxSectionStringA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
FindActCtxSectionGuid(
    __in       DWORD dwFlags,
    __reserved const GUID *lpExtensionGuid,
    __in       ULONG ulSectionId,
    __in_opt   const GUID *lpGuidToFind,
    __out      PACTCTX_SECTION_KEYED_DATA ReturnedData
    );


#if !defined(RC_INVOKED) /* RC complains about long symbols in #ifs */
#if !defined(ACTIVATION_CONTEXT_BASIC_INFORMATION_DEFINED)

typedef struct _ACTIVATION_CONTEXT_BASIC_INFORMATION {
    HANDLE  hActCtx;
    DWORD   dwFlags;
} ACTIVATION_CONTEXT_BASIC_INFORMATION, *PACTIVATION_CONTEXT_BASIC_INFORMATION;

typedef const struct _ACTIVATION_CONTEXT_BASIC_INFORMATION *PCACTIVATION_CONTEXT_BASIC_INFORMATION;

#define ACTIVATION_CONTEXT_BASIC_INFORMATION_DEFINED 1

#endif // !defined(ACTIVATION_CONTEXT_BASIC_INFORMATION_DEFINED)
#endif

#define QUERY_ACTCTX_FLAG_USE_ACTIVE_ACTCTX (0x00000004)
#define QUERY_ACTCTX_FLAG_ACTCTX_IS_HMODULE (0x00000008)
#define QUERY_ACTCTX_FLAG_ACTCTX_IS_ADDRESS (0x00000010)
#define QUERY_ACTCTX_FLAG_NO_ADDREF         (0x80000000)



//
// switch (ulInfoClass)
//
//  case ActivationContextBasicInformation:
//    pvSubInstance == NULL
//    pvBuffer is of type PACTIVATION_CONTEXT_BASIC_INFORMATION
//
//  case ActivationContextDetailedInformation:
//    pvSubInstance == NULL
//    pvBuffer is of type PACTIVATION_CONTEXT_DETAILED_INFORMATION
//
//  case AssemblyDetailedInformationInActivationContext:
//    pvSubInstance is of type PULONG
//      *pvSubInstance < ACTIVATION_CONTEXT_DETAILED_INFORMATION::ulAssemblyCount
//    pvBuffer is of type PACTIVATION_CONTEXT_ASSEMBLY_DETAILED_INFORMATION
//
//  case FileInformationInAssemblyOfAssemblyInActivationContext:
//    pvSubInstance is of type PACTIVATION_CONTEXT_QUERY_INDEX
//      pvSubInstance->ulAssemblyIndex < ACTIVATION_CONTEXT_DETAILED_INFORMATION::ulAssemblyCount
//      pvSubInstance->ulFileIndexInAssembly < ACTIVATION_CONTEXT_ASSEMBLY_DETAILED_INFORMATION::ulFileCount
//    pvBuffer is of type PASSEMBLY_FILE_DETAILED_INFORMATION
//
//  case RunlevelInformationInActivationContext :
//    pvSubInstance == NULL
//    pvBuffer is of type PACTIVATION_CONTEXT_RUN_LEVEL_INFORMATION
//
// String are placed after the structs.
//
WINBASEAPI
BOOL
WINAPI
QueryActCtxW(
    __in      DWORD dwFlags,
    __in      HANDLE hActCtx,
    __in_opt  PVOID pvSubInstance,
    __in      ULONG ulInfoClass,
    __out_bcount_part_opt(cbBuffer, *pcbWrittenOrRequired) PVOID pvBuffer,
    __in      SIZE_T cbBuffer,
    __out_opt SIZE_T *pcbWrittenOrRequired
    );

typedef BOOL (WINAPI * PQUERYACTCTXW_FUNC)(
    __in      DWORD dwFlags,
    __in      HANDLE hActCtx,
    __in_opt  PVOID pvSubInstance,
    __in      ULONG ulInfoClass,
    __out_bcount_part_opt(cbBuffer, *pcbWrittenOrRequired) PVOID pvBuffer,
    __in      SIZE_T cbBuffer,
    __out_opt SIZE_T *pcbWrittenOrRequired
    );

#endif // (_WIN32_WINNT > 0x0500) || (_WIN32_FUSION >= 0x0100) || ISOLATION_AWARE_ENABLED


WINBASEAPI
BOOL
WINAPI
ProcessIdToSessionId(
    __in  DWORD dwProcessId,
    __out DWORD *pSessionId
    );

#if _WIN32_WINNT >= 0x0501

WINBASEAPI
DWORD
WINAPI
WTSGetActiveConsoleSessionId(
    VOID
    );

WINBASEAPI
BOOL
WINAPI
IsWow64Process(
    __in  HANDLE hProcess,
    __out PBOOL Wow64Process
    );

#endif // (_WIN32_WINNT >= 0x0501)

WINBASEAPI
BOOL
WINAPI
GetLogicalProcessorInformation(
    __out_bcount_part_opt(*ReturnedLength, *ReturnedLength) PSYSTEM_LOGICAL_PROCESSOR_INFORMATION Buffer,
    __inout PDWORD ReturnedLength
    );

#if _WIN32_WINNT >= 0x0601

WINBASEAPI
BOOL
WINAPI
GetLogicalProcessorInformationEx(
    __in LOGICAL_PROCESSOR_RELATIONSHIP RelationshipType,
    __out_bcount_part_opt(*ReturnedLength, *ReturnedLength) PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX Buffer,
    __inout PDWORD ReturnedLength
    );

WINBASEAPI
WORD
WINAPI
GetActiveProcessorGroupCount(
    VOID
    );

WINBASEAPI
WORD
WINAPI
GetMaximumProcessorGroupCount(
    VOID
    );

WINBASEAPI
DWORD
WINAPI
GetActiveProcessorCount(
    __in WORD GroupNumber
    );

WINBASEAPI
DWORD
WINAPI
GetMaximumProcessorCount(
    __in WORD GroupNumber
    );

#endif // (_WIN32_WINNT >=0x0601)

//
// NUMA Information routines.
//

WINBASEAPI
BOOL
WINAPI
GetNumaHighestNodeNumber(
    __out PULONG HighestNodeNumber
    );

WINBASEAPI
BOOL
WINAPI
GetNumaProcessorNode(
    __in  UCHAR Processor,
    __out PUCHAR NodeNumber
    );

#if _WIN32_WINNT >= 0x0601

WINBASEAPI
BOOL
WINAPI
GetNumaNodeNumberFromHandle(
    __in  HANDLE hFile,
    __out PUSHORT NodeNumber
    );

#endif // (_WIN32_WINNT >=0x0601)

#if _WIN32_WINNT >= 0x0601

WINBASEAPI
BOOL
WINAPI
GetNumaProcessorNodeEx(
    __in  PPROCESSOR_NUMBER Processor,
    __out PUSHORT NodeNumber
    );

#endif // (_WIN32_WINNT >=0x0601)

WINBASEAPI
BOOL
WINAPI
GetNumaNodeProcessorMask(
    __in  UCHAR Node,
    __out PULONGLONG ProcessorMask
    );

#if _WIN32_WINNT >= 0x0601

WINBASEAPI
BOOL
WINAPI
GetNumaNodeProcessorMaskEx(
    __in  USHORT Node,
    __out PGROUP_AFFINITY ProcessorMask
    );

#endif // (_WIN32_WINNT >=0x0601)

WINBASEAPI
BOOL
WINAPI
GetNumaAvailableMemoryNode(
    __in  UCHAR Node,
    __out PULONGLONG AvailableBytes
    );

#if _WIN32_WINNT >= 0x0601

WINBASEAPI
BOOL
WINAPI
GetNumaAvailableMemoryNodeEx(
    __in  USHORT Node,
    __out PULONGLONG AvailableBytes
    );

#endif // (_WIN32_WINNT >=0x0601)

#if (_WIN32_WINNT >= 0x0600)

WINBASEAPI
BOOL
WINAPI
GetNumaProximityNode(
    __in  ULONG ProximityId,
    __out PUCHAR NodeNumber
    );

#endif

#if _WIN32_WINNT >= 0x0601

WINBASEAPI
BOOL
WINAPI
GetNumaProximityNodeEx(
    __in  ULONG ProximityId,
    __out PUSHORT NodeNumber
    );

#endif // (_WIN32_WINNT >=0x0601)

//
// Application restart and data recovery callback
//
typedef DWORD (WINAPI *APPLICATION_RECOVERY_CALLBACK)(PVOID pvParameter);

//
// Max length of commandline in characters (including the NULL character that can be registered for restart)
//
#define RESTART_MAX_CMD_LINE    1024

//
// Do not restart the process for termination due to application crashes
//
#define RESTART_NO_CRASH        1

//
// Do not restart the process for termination due to application hangs
//
#define RESTART_NO_HANG         2

//
// Do not restart the process for termination due to patch installations
//
#define RESTART_NO_PATCH        4

//
// Do not restart the process when the system is rebooted due to patch installations
//
#define RESTART_NO_REBOOT        8

WINBASEAPI
HRESULT
WINAPI
RegisterApplicationRecoveryCallback(
    __in  APPLICATION_RECOVERY_CALLBACK pRecoveyCallback,
    __in_opt  PVOID pvParameter,
    __in DWORD dwPingInterval,
    __in DWORD dwFlags
    );

WINBASEAPI
HRESULT
WINAPI
UnregisterApplicationRecoveryCallback(void);

WINBASEAPI
HRESULT
WINAPI
RegisterApplicationRestart(
    __in_opt PCWSTR pwzCommandline,
    __in DWORD dwFlags
    );

WINBASEAPI
HRESULT
WINAPI
UnregisterApplicationRestart(void);

#define RECOVERY_DEFAULT_PING_INTERVAL  5000
#define RECOVERY_MAX_PING_INTERVAL      (5 * 60 * 1000)

WINBASEAPI
HRESULT
WINAPI
GetApplicationRecoveryCallback(
    __in  HANDLE hProcess,
    __out APPLICATION_RECOVERY_CALLBACK* pRecoveryCallback,
    __deref_opt_out_opt PVOID* ppvParameter,
    __out_opt PDWORD pdwPingInterval,
    __out_opt PDWORD pdwFlags
    );

WINBASEAPI
HRESULT
WINAPI
GetApplicationRestartSettings(
    __in HANDLE hProcess,
    __out_ecount_opt(*pcchSize) PWSTR pwzCommandline,
    __inout PDWORD pcchSize,
    __out_opt PDWORD pdwFlags
    );

WINBASEAPI
HRESULT
WINAPI
ApplicationRecoveryInProgress(
    __out PBOOL pbCancelled
    );

WINBASEAPI
VOID
WINAPI
ApplicationRecoveryFinished(
    __in BOOL bSuccess
    );

#if (_WIN32_WINNT >= 0x0600)
typedef enum _FILE_INFO_BY_HANDLE_CLASS {
    FileBasicInfo,
    FileStandardInfo,
    FileNameInfo,
    FileRenameInfo,
    FileDispositionInfo,
    FileAllocationInfo,
    FileEndOfFileInfo,
    FileStreamInfo,
    FileCompressionInfo,
    FileAttributeTagInfo,
    FileIdBothDirectoryInfo,
    FileIdBothDirectoryRestartInfo,
    FileIoPriorityHintInfo,
    FileRemoteProtocolInfo, 
    MaximumFileInfoByHandleClass
} FILE_INFO_BY_HANDLE_CLASS, *PFILE_INFO_BY_HANDLE_CLASS;

typedef struct _FILE_BASIC_INFO {
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    DWORD FileAttributes;
} FILE_BASIC_INFO, *PFILE_BASIC_INFO;

typedef struct _FILE_STANDARD_INFO {
    LARGE_INTEGER AllocationSize;
    LARGE_INTEGER EndOfFile;
    DWORD NumberOfLinks;
    BOOLEAN DeletePending;
    BOOLEAN Directory;
} FILE_STANDARD_INFO, *PFILE_STANDARD_INFO;

typedef struct _FILE_NAME_INFO {
    DWORD FileNameLength;
    WCHAR FileName[1];
} FILE_NAME_INFO, *PFILE_NAME_INFO;

typedef struct _FILE_RENAME_INFO {
    BOOLEAN ReplaceIfExists;
    HANDLE RootDirectory;
    DWORD FileNameLength;
    WCHAR FileName[1];
} FILE_RENAME_INFO, *PFILE_RENAME_INFO;

typedef struct _FILE_ALLOCATION_INFO {
    LARGE_INTEGER AllocationSize;
} FILE_ALLOCATION_INFO, *PFILE_ALLOCATION_INFO;

typedef struct _FILE_END_OF_FILE_INFO {
    LARGE_INTEGER EndOfFile;
} FILE_END_OF_FILE_INFO, *PFILE_END_OF_FILE_INFO;

typedef struct _FILE_STREAM_INFO {
    DWORD NextEntryOffset;
    DWORD StreamNameLength;
    LARGE_INTEGER StreamSize;
    LARGE_INTEGER StreamAllocationSize;
    WCHAR StreamName[1];
} FILE_STREAM_INFO, *PFILE_STREAM_INFO;

typedef struct _FILE_COMPRESSION_INFO {
    LARGE_INTEGER CompressedFileSize;
    WORD CompressionFormat;
    UCHAR CompressionUnitShift;
    UCHAR ChunkShift;
    UCHAR ClusterShift;
    UCHAR Reserved[3];
} FILE_COMPRESSION_INFO, *PFILE_COMPRESSION_INFO;

typedef struct _FILE_ATTRIBUTE_TAG_INFO {
    DWORD FileAttributes;
    DWORD ReparseTag;
} FILE_ATTRIBUTE_TAG_INFO, *PFILE_ATTRIBUTE_TAG_INFO;

typedef struct _FILE_DISPOSITION_INFO {
    BOOLEAN DeleteFile;
} FILE_DISPOSITION_INFO, *PFILE_DISPOSITION_INFO;

typedef struct _FILE_ID_BOTH_DIR_INFO {
    DWORD NextEntryOffset;
    DWORD FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    DWORD FileAttributes;
    DWORD FileNameLength;
    DWORD EaSize;
    CCHAR ShortNameLength;
    WCHAR ShortName[12];
    LARGE_INTEGER FileId;
    WCHAR FileName[1];
} FILE_ID_BOTH_DIR_INFO, *PFILE_ID_BOTH_DIR_INFO;

typedef enum _PRIORITY_HINT {
      IoPriorityHintVeryLow = 0,
      IoPriorityHintLow,
      IoPriorityHintNormal,
      MaximumIoPriorityHintType
} PRIORITY_HINT;

typedef struct _FILE_IO_PRIORITY_HINT_INFO {
    PRIORITY_HINT PriorityHint;
} FILE_IO_PRIORITY_HINT_INFO, *PFILE_IO_PRIORITY_HINT_INFO;

// Structure and constants must match those in ntioapi_x.w

#define REMOTE_PROTOCOL_INFO_FLAG_LOOPBACK       0x00000001
#define REMOTE_PROTOCOL_INFO_FLAG_OFFLINE        0x00000002

typedef struct _FILE_REMOTE_PROTOCOL_INFO
{
    // Structure Version
    USHORT StructureVersion;     // 1
    USHORT StructureSize;        // sizeof(FILE_REMOTE_PROTOCOL_INFO)
    
    DWORD  Protocol;             // Protocol (WNNC_NET_*) defined in wnnc.h or ntifs.h.
    
    // Protocol Version & Type
    USHORT ProtocolMajorVersion;
    USHORT ProtocolMinorVersion;
    USHORT ProtocolRevision;
    
    USHORT Reserved;
    
    // Protocol-Generic Information
    DWORD  Flags;
    
    struct {
        DWORD Reserved[8];
    } GenericReserved;

    // Protocol specific information
    
    struct {
        DWORD Reserved[16];
    } ProtocolSpecificReserved;
    
} FILE_REMOTE_PROTOCOL_INFO, *PFILE_REMOTE_PROTOCOL_INFO;


WINBASEAPI
BOOL
WINAPI
SetFileInformationByHandle(
    __in  HANDLE hFile,
    __in  FILE_INFO_BY_HANDLE_CLASS FileInformationClass,
    __in_bcount(dwBufferSize)  LPVOID lpFileInformation,
    __in  DWORD dwBufferSize
);

WINBASEAPI
BOOL
WINAPI
GetFileInformationByHandleEx(
    __in  HANDLE hFile,
    __in  FILE_INFO_BY_HANDLE_CLASS FileInformationClass,
    __out_bcount(dwBufferSize) LPVOID lpFileInformation,
    __in  DWORD dwBufferSize
);

typedef enum _FILE_ID_TYPE {
      FileIdType,
      ObjectIdType,
      MaximumFileIdType
} FILE_ID_TYPE, *PFILE_ID_TYPE;

typedef struct FILE_ID_DESCRIPTOR {
    DWORD dwSize;  // Size of the struct
    FILE_ID_TYPE Type; // Describes the type of identifier passed in.
    union {
        LARGE_INTEGER FileId;
        GUID ObjectId;
    } DUMMYUNIONNAME;
} FILE_ID_DESCRIPTOR, *LPFILE_ID_DESCRIPTOR;

WINBASEAPI
__out
HANDLE
WINAPI
OpenFileById (
    __in     HANDLE hVolumeHint,
    __in     LPFILE_ID_DESCRIPTOR lpFileId,
    __in     DWORD dwDesiredAccess,
    __in     DWORD dwShareMode,
    __in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    __in     DWORD dwFlagsAndAttributes
    );
#endif

#if (_WIN32_WINNT >= 0x0600)

//
//  Flags to be passed into CREATE_SYMBOLIC_LINK
//

#define SYMBOLIC_LINK_FLAG_DIRECTORY            (0x1)

#define VALID_SYMBOLIC_LINK_FLAGS  SYMBOLIC_LINK_FLAG_DIRECTORY // & whatever other flags we think of!

WINBASEAPI
BOOLEAN
APIENTRY
CreateSymbolicLinkA (
    __in LPCSTR lpSymlinkFileName,
    __in LPCSTR lpTargetFileName,
    __in DWORD dwFlags
    );
WINBASEAPI
BOOLEAN
APIENTRY
CreateSymbolicLinkW (
    __in LPCWSTR lpSymlinkFileName,
    __in LPCWSTR lpTargetFileName,
    __in DWORD dwFlags
    );
#ifdef UNICODE
#define CreateSymbolicLink  CreateSymbolicLinkW
#else
#define CreateSymbolicLink  CreateSymbolicLinkA
#endif // !UNICODE

WINBASEAPI
BOOLEAN
APIENTRY
CreateSymbolicLinkTransactedA (
    __in     LPCSTR lpSymlinkFileName,
    __in     LPCSTR lpTargetFileName,
    __in     DWORD dwFlags,
    __in     HANDLE hTransaction
    );
WINBASEAPI
BOOLEAN
APIENTRY
CreateSymbolicLinkTransactedW (
    __in     LPCWSTR lpSymlinkFileName,
    __in     LPCWSTR lpTargetFileName,
    __in     DWORD dwFlags,
    __in     HANDLE hTransaction
    );
#ifdef UNICODE
#define CreateSymbolicLinkTransacted  CreateSymbolicLinkTransactedW
#else
#define CreateSymbolicLinkTransacted  CreateSymbolicLinkTransactedA
#endif // !UNICODE

WINBASEAPI
DWORD
WINAPI
GetFinalPathNameByHandleA (
    __in HANDLE hFile,
    __out_ecount(cchFilePath) LPSTR lpszFilePath,
    __in DWORD cchFilePath,
    __in DWORD dwFlags
);
WINBASEAPI
DWORD
WINAPI
GetFinalPathNameByHandleW (
    __in HANDLE hFile,
    __out_ecount(cchFilePath) LPWSTR lpszFilePath,
    __in DWORD cchFilePath,
    __in DWORD dwFlags
);
#ifdef UNICODE
#define GetFinalPathNameByHandle  GetFinalPathNameByHandleW
#else
#define GetFinalPathNameByHandle  GetFinalPathNameByHandleA
#endif // !UNICODE

#endif // (_WIN32_WINNT >= 0x0600)


#if (_WIN32_WINNT >= 0x0600)

WINBASEAPI
BOOL
WINAPI
QueryActCtxSettingsW(
    __in_opt      DWORD dwFlags,
    __in_opt      HANDLE hActCtx,
    __in_opt      PCWSTR settingsNameSpace,
    __in          PCWSTR settingName,
    __out_bcount_part_opt(dwBuffer, *pdwWrittenOrRequired) PWSTR pvBuffer,
    __in      SIZE_T dwBuffer,
    __out_opt SIZE_T *pdwWrittenOrRequired
    );

#endif

#if (_WIN32_WINNT >= 0x0600)

WINBASEAPI
BOOL
WINAPI
ReplacePartitionUnit (
    __in PWSTR TargetPartition,
    __in PWSTR SparePartition,
    __in ULONG Flags
    );

#endif


#if (_WIN32_WINNT >= 0x0600)

WINBASEAPI
BOOL
WINAPI
AddSecureMemoryCacheCallback(
    __in __callback PSECURE_MEMORY_CACHE_CALLBACK pfnCallBack
    );

WINBASEAPI
BOOL
WINAPI
RemoveSecureMemoryCacheCallback(
    __in __callback PSECURE_MEMORY_CACHE_CALLBACK pfnCallBack
    );

#endif

#if (_WIN32_WINNT >= 0x0601)

__checkReturn
WINBASEAPI
BOOL
WINAPI
CopyExtendedContext(
    __out PCONTEXT_EX Destination,
    __in DWORD ContextFlags,
    __in PCONTEXT_EX Source
    );

__checkReturn
WINBASEAPI
BOOL
WINAPI
InitializeExtendedContext(
    __out PVOID Context,
    __in DWORD ContextFlags,
    __out PCONTEXT_EX* ContextEx
    );

WINBASEAPI
DWORD64
WINAPI
GetEnabledExtendedFeatures(
    __in DWORD64 FeatureMask
    );


WINBASEAPI
BOOL
WINAPI
GetExtendedContextLength(
    __in DWORD ContextFlags,
    __out PDWORD ContextLength
    );

WINBASEAPI
DWORD64
WINAPI
GetExtendedFeaturesMask(
    __in PCONTEXT_EX ContextEx
    );

WINBASEAPI
PVOID
WINAPI
LocateExtendedFeature(
    __in PCONTEXT_EX ContextEx,
    __in DWORD FeatureId,
    __out_opt PDWORD Length
    );

WINBASEAPI
PCONTEXT
WINAPI
LocateLegacyContext(
    __in PCONTEXT_EX ContextEx,
    __out_opt PDWORD Length
    );

WINBASEAPI
VOID
WINAPI
SetExtendedFeaturesMask(
    __out PCONTEXT_EX ContextEx,
    __in DWORD64 FeatureMask
    );

WINBASEAPI
DWORD
APIENTRY
EnableThreadProfiling(
    __in HANDLE ThreadHandle,
    __in DWORD Flags,
    __in DWORD64 HardwareCounters,
    __out HANDLE *PerformanceDataHandle
    );

WINBASEAPI
DWORD
APIENTRY
DisableThreadProfiling(
    __in HANDLE PerformanceDataHandle
    );

WINBASEAPI
DWORD
APIENTRY
QueryThreadProfiling(
    __in HANDLE ThreadHandle,
    __out PBOOLEAN Enabled
    );

WINBASEAPI
DWORD
APIENTRY
ReadThreadProfilingData(
    __in HANDLE PerformanceDataHandle,
    __in DWORD Flags,
    __out PPERFORMANCE_DATA PerformanceData
    );

#endif



#if !defined(RC_INVOKED) /* RC complains about long symbols in #ifs */
#if defined(ISOLATION_AWARE_ENABLED) && (ISOLATION_AWARE_ENABLED != 0)
#include "winbase.inl"
#endif /* ISOLATION_AWARE_ENABLED */
#endif /* RC */

#ifdef __cplusplus
}
#endif



#endif // _WINBASE_

#if !defined(RC_INVOKED)
#if !defined(NOWINBASEINTERLOCK)
#if !defined(_NTOS_)
/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    winbase_interlockedcplusplus.h

Abstract:

    C++ function overloads in place of "manual name mangling".
    This file is meant to be #included by winbase.h or any other file declaring the signed interlocked functions.

Author:

    Jay Krell (JayKrell) April 2002

--*/

#if !defined(RC_INVOKED) /* { */

#if !defined(MICROSOFT_WINDOWS_WINBASE_INTERLOCKED_CPLUSPLUS_H_INCLUDED) /* { */
#define MICROSOFT_WINDOWS_WINBASE_INTERLOCKED_CPLUSPLUS_H_INCLUDED
#if _MSC_VER > 1000
#pragma once
#endif

#if !defined(MIDL_PASS) /* { */

/*
To turn off/hide the contents of this file:
 #define MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS 0
*/

#if !defined(MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS)
#define MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS (_WIN32_WINNT >= 0x0502 || !defined(_WINBASE_))
#endif

#if MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS  /* { */

#if defined(__cplusplus) /* { */

extern "C++" {

FORCEINLINE
unsigned
InterlockedIncrement(
    __inout __drv_interlocked unsigned volatile *Addend
    )
{
    return (unsigned) InterlockedIncrement((volatile long*) Addend);
}

FORCEINLINE
unsigned long
InterlockedIncrement(
    __inout __drv_interlocked unsigned long volatile *Addend
    )
{
    return (unsigned long) InterlockedIncrement((volatile long*) Addend);
}

#if defined(_WIN64) || ((_WIN32_WINNT >= 0x0502) && defined(_WINBASE_))

FORCEINLINE
unsigned __int64
InterlockedIncrement(
    __inout __drv_interlocked unsigned __int64 volatile *Addend
    )
{
    return (unsigned __int64) InterlockedIncrement64((volatile __int64*) Addend);
}

#endif

FORCEINLINE
unsigned
InterlockedDecrement(
    __inout __drv_interlocked unsigned volatile *Addend
    )
{
    return (unsigned long) InterlockedDecrement((volatile long*) Addend);
}

FORCEINLINE
unsigned long
InterlockedDecrement(
    __inout __drv_interlocked unsigned long volatile *Addend
    )
{
    return (unsigned long) InterlockedDecrement((volatile long*) Addend);
}

#if defined(_WIN64) || ((_WIN32_WINNT >= 0x0502) && defined(_WINBASE_))

FORCEINLINE
unsigned __int64
InterlockedDecrement(
    __inout __drv_interlocked unsigned __int64 volatile *Addend
    )
{
    return (unsigned __int64) InterlockedDecrement64((volatile __int64*) Addend);
}

#endif

FORCEINLINE
unsigned
InterlockedExchange(
    __inout __drv_interlocked unsigned volatile *Target,
    __in unsigned Value
    )
{
    return (unsigned) InterlockedExchange((volatile long*) Target, (long) Value);
}

FORCEINLINE
unsigned long
InterlockedExchange(
    __inout __drv_interlocked unsigned long volatile *Target,
    __in unsigned long Value
    )
{
    return (unsigned long) InterlockedExchange((volatile long*) Target, (long) Value);
}

#if defined(_WIN64) || ((_WIN32_WINNT >= 0x0502) && defined(_WINBASE_))

FORCEINLINE
unsigned __int64
InterlockedExchange(
    __inout __drv_interlocked unsigned __int64 volatile *Target,
    __in unsigned __int64 Value
    )
{
    return (unsigned __int64) InterlockedExchange64((volatile __int64*) Target, (__int64) Value);
}

#endif

FORCEINLINE
unsigned
InterlockedExchangeAdd(
    __inout __drv_interlocked unsigned volatile *Addend,
    __in unsigned Value
    )
{
    return (unsigned) InterlockedExchangeAdd((volatile long*) Addend, (long) Value);
}

FORCEINLINE
unsigned
InterlockedExchangeSubtract(
    __inout __drv_interlocked unsigned volatile *Addend,
    __in unsigned Value
    )
{
    return (unsigned) InterlockedExchangeAdd((volatile long*) Addend,  - (long) Value);
}

FORCEINLINE
unsigned long
InterlockedExchangeAdd(
    __inout __drv_interlocked unsigned long volatile *Addend,
    __in unsigned long Value
    )
{
    return (unsigned long) InterlockedExchangeAdd((volatile long*) Addend, (long) Value);
}

FORCEINLINE
unsigned long
InterlockedExchangeSubtract(
    __inout __drv_interlocked unsigned long volatile *Addend,
    __in unsigned long Value
    )
{
    return (unsigned long) InterlockedExchangeAdd((volatile long*) Addend,  - (long) Value);
}

#if defined(_WIN64) || ((_WIN32_WINNT >= 0x0502) && defined(_WINBASE_))

FORCEINLINE
unsigned __int64
InterlockedExchangeAdd(
    __inout __drv_interlocked unsigned __int64 volatile *Addend,
    __in unsigned __int64 Value
    )
{
    return (unsigned __int64) InterlockedExchangeAdd64((volatile __int64*) Addend,  (__int64) Value);
}

FORCEINLINE
unsigned __int64
InterlockedExchangeSubtract(
    __inout __drv_interlocked unsigned __int64 volatile *Addend,
    __in unsigned __int64 Value
    )
{
    return (unsigned __int64) InterlockedExchangeAdd64((volatile __int64*) Addend,  - (__int64) Value);
}

#endif

FORCEINLINE
unsigned
InterlockedCompareExchange(
    __inout __drv_interlocked unsigned volatile *Destination,
    __in unsigned Exchange,
    __in unsigned Comperand
    )
{
    return (unsigned) InterlockedCompareExchange((volatile long*) Destination, (long) Exchange, (long) Comperand);
}

FORCEINLINE
unsigned long
InterlockedCompareExchange(
    __inout __drv_interlocked unsigned long volatile *Destination,
    __in unsigned long Exchange,
    __in unsigned long Comperand
    )
{
    return (unsigned long) InterlockedCompareExchange((volatile long*) Destination, (long) Exchange, (long) Comperand);
}

#if defined(_WIN64) || ((_WIN32_WINNT >= 0x0502) && defined(_WINBASE_))

FORCEINLINE
unsigned __int64
InterlockedCompareExchange(
    __inout __drv_interlocked unsigned __int64 volatile *Destination,
    __in unsigned __int64 Exchange,
    __in unsigned __int64 Comperand
    )
{
    return (unsigned __int64) InterlockedCompareExchange64((volatile __int64*) Destination, (__int64) Exchange, (__int64) Comperand);
}

FORCEINLINE
unsigned __int64
InterlockedAnd(
    __inout __drv_interlocked unsigned __int64 volatile *Destination,
    __in unsigned __int64 Value
    )
{
    return (unsigned __int64) InterlockedAnd64((volatile __int64*) Destination, (__int64) Value);
}

FORCEINLINE
unsigned __int64
InterlockedOr(
    __inout __drv_interlocked unsigned __int64 volatile *Destination,
    __in unsigned __int64 Value
    )
{
    return (unsigned __int64) InterlockedOr64((volatile __int64*) Destination, (__int64) Value);
}

FORCEINLINE
unsigned __int64
InterlockedXor(
    __inout __drv_interlocked unsigned __int64 volatile *Destination,
    __in unsigned __int64 Value
    )
{
    return (unsigned __int64) InterlockedXor64((volatile __int64*) Destination, (__int64) Value);
}

#endif

} /* extern "C++" */
#endif /* } __cplusplus */

#endif /* } MICROSOFT_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS */

#undef MICROSOFT_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS
#define MICROSOFT_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS 0

#endif /* } MIDL_PASS */
#endif /* } MICROSOFT_WINDOWS_WINBASE_INTERLOCKED_CPLUSPLUS_H_INCLUDED */
#endif /* } RC_INVOKED */
#endif /* _NTOS_ */
#endif /* NOWINBASEINTERLOCK */
#endif /* RC_INVOKED */

