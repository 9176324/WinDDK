/*++

Copyright (c) 1991-1999 Microsoft Corporation

Module Name:

    rpcdcep.h

Abstract:

    This module contains the private RPC runtime APIs for use by the
    stubs and by support libraries.  Applications must not call these
    routines.

--*/

#ifndef __RPCDCEP_H__
#define __RPCDCEP_H__

#if _MSC_VER > 1000
#pragma once
#endif

// Set the packing level for RPC structures for Dos and Windows.

#if defined(__RPC_DOS__) || defined(__RPC_WIN16__)
#pragma pack(2)
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _RPC_VERSION {
    unsigned short MajorVersion;
    unsigned short MinorVersion;
} RPC_VERSION;

typedef struct _RPC_SYNTAX_IDENTIFIER {
    GUID SyntaxGUID;
    RPC_VERSION SyntaxVersion;
} RPC_SYNTAX_IDENTIFIER, __RPC_FAR * PRPC_SYNTAX_IDENTIFIER;

typedef struct _RPC_MESSAGE
{
    RPC_BINDING_HANDLE Handle;
    unsigned long DataRepresentation;
    void __RPC_FAR * Buffer;
    unsigned int BufferLength;
    unsigned int ProcNum;
    PRPC_SYNTAX_IDENTIFIER TransferSyntax;
    void __RPC_FAR * RpcInterfaceInformation;
    void __RPC_FAR * ReservedForRuntime;
    RPC_MGR_EPV __RPC_FAR * ManagerEpv;
    void __RPC_FAR * ImportContext;
    unsigned long RpcFlags;
} RPC_MESSAGE, __RPC_FAR * PRPC_MESSAGE;


typedef RPC_STATUS
RPC_ENTRY RPC_FORWARD_FUNCTION(
                       IN UUID             __RPC_FAR * InterfaceId,
                       IN RPC_VERSION      __RPC_FAR * InterfaceVersion,
                       IN UUID             __RPC_FAR * ObjectId,
                       IN unsigned char         __RPC_FAR * Rpcpro,
                       IN void __RPC_FAR * __RPC_FAR * ppDestEndpoint);

enum RPC_ADDRESS_CHANGE_TYPE
{
    PROTOCOL_NOT_LOADED = 1,
    PROTOCOL_LOADED,
    PROTOCOL_ADDRESS_CHANGE
};

typedef void
RPC_ENTRY RPC_ADDRESS_CHANGE_FN(
                        IN void * arg
                        );

/*
*  New context handle flavors.
*/

#define RPC_CONTEXT_HANDLE_DEFAULT_GUARD    ((void *)0xFFFFF00D)

#define RPC_CONTEXT_HANDLE_DEFAULT_FLAGS    0x00000000UL
#define RPC_CONTEXT_HANDLE_FLAGS            0x30000000UL
#define RPC_CONTEXT_HANDLE_SERIALIZE        0x10000000UL
#define RPC_CONTEXT_HANDLE_DONT_SERIALIZE   0x20000000UL

/*
 * Types of function calls for datagram rpc
 */

#define RPC_NCA_FLAGS_DEFAULT       0x00000000  /* 0b000...000 */
#define RPC_NCA_FLAGS_IDEMPOTENT    0x00000001  /* 0b000...001 */
#define RPC_NCA_FLAGS_BROADCAST     0x00000002  /* 0b000...010 */
#define RPC_NCA_FLAGS_MAYBE         0x00000004  /* 0b000...100 */

#define RPC_BUFFER_COMPLETE         0x00001000 /* used by pipes */
#define RPC_BUFFER_PARTIAL          0x00002000 /* used by pipes */
#define RPC_BUFFER_EXTRA            0x00004000 /* used by pipes */
#define RPC_BUFFER_ASYNC            0x00008000 /* used by async rpc */
#define RPC_BUFFER_NONOTIFY         0x00010000 /* used by async pipes */

#define RPCFLG_MESSAGE              0x01000000UL
#define RPCFLG_AUTO_COMPLETE        0x08000000UL
#define RPCFLG_LOCAL_CALL           0x10000000UL
#define RPCFLG_INPUT_SYNCHRONOUS    0x20000000UL
#define RPCFLG_ASYNCHRONOUS         0x40000000UL
#define RPCFLG_NON_NDR              0x80000000UL

#if defined(__RPC_WIN32__) || defined(__RPC_MAC__) || defined(__RPC_WIN64__)
#define RPC_FLAGS_VALID_BIT 0x00008000
#elif defined(__RPC_DOS__) || defined(__RPC_WIN16__)
#define RPC_FLAGS_VALID_BIT 0x8000
#endif

typedef
void
(__RPC_STUB __RPC_FAR * RPC_DISPATCH_FUNCTION) (
    IN OUT PRPC_MESSAGE Message
    );

typedef struct {
    unsigned int DispatchTableCount;
    RPC_DISPATCH_FUNCTION __RPC_FAR * DispatchTable;
    LONG_PTR                          Reserved;
} RPC_DISPATCH_TABLE, __RPC_FAR * PRPC_DISPATCH_TABLE;

typedef struct _RPC_PROTSEQ_ENDPOINT
{
    unsigned char __RPC_FAR * RpcProtocolSequence;
    unsigned char __RPC_FAR * Endpoint;
} RPC_PROTSEQ_ENDPOINT, __RPC_FAR * PRPC_PROTSEQ_ENDPOINT;

/*
Both of these types MUST start with the InterfaceId and TransferSyntax.
Look at RpcIfInqId and I_RpcIfInqTransferSyntaxes to see why.
*/
#define NT351_INTERFACE_SIZE 0x40
#define RPC_INTERFACE_HAS_PIPES           0x0001

typedef struct _RPC_SERVER_INTERFACE
{
    unsigned int Length;
    RPC_SYNTAX_IDENTIFIER InterfaceId;
    RPC_SYNTAX_IDENTIFIER TransferSyntax;
    PRPC_DISPATCH_TABLE DispatchTable;
    unsigned int RpcProtseqEndpointCount;
    PRPC_PROTSEQ_ENDPOINT RpcProtseqEndpoint;
    RPC_MGR_EPV __RPC_FAR *DefaultManagerEpv;
    void const __RPC_FAR *InterpreterInfo;
    unsigned int Flags ;
} RPC_SERVER_INTERFACE, __RPC_FAR * PRPC_SERVER_INTERFACE;

typedef struct _RPC_CLIENT_INTERFACE
{
    unsigned int Length;
    RPC_SYNTAX_IDENTIFIER   InterfaceId;
    RPC_SYNTAX_IDENTIFIER   TransferSyntax;
    PRPC_DISPATCH_TABLE     DispatchTable;
    unsigned int            RpcProtseqEndpointCount;
    PRPC_PROTSEQ_ENDPOINT   RpcProtseqEndpoint;
    ULONG_PTR               Reserved;
    void const __RPC_FAR *  InterpreterInfo;
    unsigned int Flags ;
} RPC_CLIENT_INTERFACE, __RPC_FAR * PRPC_CLIENT_INTERFACE;

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcGetBuffer (
    IN OUT RPC_MESSAGE __RPC_FAR * Message
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcGetBufferWithObject (
    IN OUT RPC_MESSAGE __RPC_FAR * Message,
    IN UUID * ObjectUuid
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcSendReceive (
    IN OUT RPC_MESSAGE __RPC_FAR * Message
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcFreeBuffer (
    IN OUT RPC_MESSAGE __RPC_FAR * Message
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcSend (
    IN OUT PRPC_MESSAGE Message
    ) ;

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcReceive (
    IN OUT PRPC_MESSAGE Message,
    IN unsigned int Size
    ) ;

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcFreePipeBuffer (
    IN OUT RPC_MESSAGE __RPC_FAR * Message
    ) ;

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcReallocPipeBuffer (
    IN PRPC_MESSAGE Message,
    IN unsigned int NewSize
    ) ;

typedef void * I_RPC_MUTEX;

RPCRTAPI
void
RPC_ENTRY
I_RpcRequestMutex (
    IN OUT I_RPC_MUTEX * Mutex
    );

RPCRTAPI
void
RPC_ENTRY
I_RpcClearMutex (
    IN I_RPC_MUTEX Mutex
    );

RPCRTAPI
void
RPC_ENTRY
I_RpcDeleteMutex (
    IN I_RPC_MUTEX Mutex
    );

RPCRTAPI
void __RPC_FAR *
RPC_ENTRY
I_RpcAllocate (
    IN unsigned int Size
    );

RPCRTAPI
void
RPC_ENTRY
I_RpcFree (
    IN void __RPC_FAR * Object
    );

RPCRTAPI
void
RPC_ENTRY
I_RpcPauseExecution (
    IN unsigned long Milliseconds
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcGetExtendedError (
    );

typedef
void
(__RPC_USER __RPC_FAR * PRPC_RUNDOWN) (
    void __RPC_FAR * AssociationContext
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcMonitorAssociation (
    IN RPC_BINDING_HANDLE Handle,
    IN PRPC_RUNDOWN RundownRoutine,
    IN void * Context
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcStopMonitorAssociation (
    IN RPC_BINDING_HANDLE Handle
    );

RPCRTAPI
RPC_BINDING_HANDLE
RPC_ENTRY
I_RpcGetCurrentCallHandle(
    void
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcGetAssociationContext (
    IN RPC_BINDING_HANDLE BindingHandle,
    OUT void __RPC_FAR * __RPC_FAR * AssociationContext
    );

RPCRTAPI
void *
RPC_ENTRY
I_RpcGetServerContextList (
    IN RPC_BINDING_HANDLE BindingHandle
    );

RPCRTAPI
void
RPC_ENTRY
I_RpcSetServerContextList (
    IN RPC_BINDING_HANDLE BindingHandle,
    OUT void * ServerContextList
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcNsInterfaceExported (
    IN unsigned long EntryNameSyntax,
    IN unsigned short *EntryName,
    IN RPC_SERVER_INTERFACE * RpcInterfaceInformation
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcNsInterfaceUnexported (
    IN unsigned long EntryNameSyntax,
    IN unsigned short *EntryName,
    IN RPC_SERVER_INTERFACE * RpcInterfaceInformation
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcBindingToStaticStringBindingW (
    IN RPC_BINDING_HANDLE Binding,
    OUT unsigned short **StringBinding
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcBindingInqSecurityContext (
    IN RPC_BINDING_HANDLE Binding,
    OUT void **SecurityContextHandle
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcBindingInqWireIdForSnego (
    IN RPC_BINDING_HANDLE Binding,
    OUT unsigned char *WireId
    );

#ifdef RPC_UNICODE_SUPPORTED

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcNsBindingSetEntryNameW (
    IN RPC_BINDING_HANDLE Binding,
    IN unsigned long EntryNameSyntax,
    IN unsigned short __RPC_FAR * EntryName
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcNsBindingSetEntryNameA (
    IN RPC_BINDING_HANDLE Binding,
    IN unsigned long EntryNameSyntax,
    IN unsigned char __RPC_FAR * EntryName
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcServerUseProtseqEp2A (
    IN unsigned char __RPC_FAR * NetworkAddress,
    IN unsigned char __RPC_FAR * Protseq,
    IN unsigned int MaxCalls,
    IN unsigned char __RPC_FAR * Endpoint,
    IN void __RPC_FAR * SecurityDescriptor,
    IN void *Policy
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcServerUseProtseqEp2W (
    IN unsigned short __RPC_FAR * NetworkAddress,
    IN unsigned short __RPC_FAR * Protseq,
    IN unsigned int MaxCalls,
    IN unsigned short __RPC_FAR * Endpoint,
    IN void __RPC_FAR * SecurityDescriptor,
    IN void * Policy
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcServerUseProtseq2W (
    IN unsigned short __RPC_FAR * NetworkAddress,
    IN unsigned short __RPC_FAR * Protseq,
    IN unsigned int MaxCalls,
    IN void __RPC_FAR * SecurityDescriptor,
    IN void * Policy
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcServerUseProtseq2A (
    IN unsigned char __RPC_FAR * NetworkAddress,
    IN unsigned char __RPC_FAR * Protseq,
    IN unsigned int MaxCalls,
    IN void __RPC_FAR * SecurityDescriptor,
    IN void * Policy
    );


#ifdef UNICODE
#define I_RpcNsBindingSetEntryName I_RpcNsBindingSetEntryNameW
#define I_RpcServerUseProtseqEp2 I_RpcServerUseProtseqEp2W
#define I_RpcServerUseProtseq2 I_RpcServerUseProtseq2W
#else
#define I_RpcNsBindingSetEntryName I_RpcNsBindingSetEntryNameA
#define I_RpcServerUseProtseqEp2 I_RpcServerUseProtseqEp2A
#define I_RpcServerUseProtseq2 I_RpcServerUseProtseq2A
#endif

#else

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcNsBindingSetEntryName (
    IN RPC_BINDING_HANDLE Binding,
    IN unsigned long EntryNameSyntax,
    IN unsigned char __RPC_FAR * EntryName
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcServerUseProtseq2 (
    IN unsigned char __RPC_FAR * NetworkAddress,
    IN unsigned char __RPC_FAR * Protseq,
    IN unsigned int MaxCalls,
    IN void __RPC_FAR * SecurityDescriptor,
    IN void * Policy
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcServerUseProtseqEp2 (
    IN unsigned char __RPC_FAR * NetworkAddress,
    IN unsigned char __RPC_FAR * Protseq,
    IN unsigned int MaxCalls,
    IN unsigned char __RPC_FAR * Endpoint,
    IN void __RPC_FAR * SecurityDescriptor,
    IN void * Policy
    );
#endif

#ifdef RPC_UNICODE_SUPPORTED

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcBindingInqDynamicEndpointW (
    IN RPC_BINDING_HANDLE Binding,
    OUT unsigned short __RPC_FAR * __RPC_FAR * DynamicEndpoint
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcBindingInqDynamicEndpointA (
    IN RPC_BINDING_HANDLE Binding,
    OUT unsigned char __RPC_FAR * __RPC_FAR * DynamicEndpoint
    );

#ifdef UNICODE
#define I_RpcBindingInqDynamicEndpoint I_RpcBindingInqDynamicEndpointW
#else
#define I_RpcBindingInqDynamicEndpoint I_RpcBindingInqDynamicEndpointA
#endif

#else

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcBindingInqDynamicEndpoint (
    IN RPC_BINDING_HANDLE Binding,
    OUT unsigned char __RPC_FAR * __RPC_FAR * DynamicEndpoint
    );

#endif

#define TRANSPORT_TYPE_CN        0x01
#define TRANSPORT_TYPE_DG        0x02
#define TRANSPORT_TYPE_LPC       0x04
#define TRANSPORT_TYPE_WMSG      0x08

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcBindingInqTransportType (
    IN RPC_BINDING_HANDLE Binding,
    OUT unsigned int __RPC_FAR * Type
    );

typedef struct _RPC_TRANSFER_SYNTAX
{
    UUID Uuid;
    unsigned short VersMajor;
    unsigned short VersMinor;
} RPC_TRANSFER_SYNTAX;

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcIfInqTransferSyntaxes (
    IN RPC_IF_HANDLE RpcIfHandle,
    OUT RPC_TRANSFER_SYNTAX __RPC_FAR * TransferSyntaxes,
    IN unsigned int TransferSyntaxSize,
    OUT unsigned int __RPC_FAR * TransferSyntaxCount
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_UuidCreate (
    OUT UUID __RPC_FAR * Uuid
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcBindingCopy (
    IN RPC_BINDING_HANDLE SourceBinding,
    OUT RPC_BINDING_HANDLE __RPC_FAR * DestinationBinding
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcBindingIsClientLocal (
    IN RPC_BINDING_HANDLE BindingHandle OPTIONAL,
    OUT unsigned int __RPC_FAR * ClientLocalFlag
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcBindingInqConnId (
    IN RPC_BINDING_HANDLE Binding,
    OUT void **ConnId,
    OUT int *pfFirstCall
    );

RPCRTAPI
void
RPC_ENTRY
I_RpcSsDontSerializeContext (
    void
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcLaunchDatagramReceiveThread(
    void __RPC_FAR * pAddress
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcServerRegisterForwardFunction (
    IN RPC_FORWARD_FUNCTION  * pForwardFunction
    );

RPC_ADDRESS_CHANGE_FN * RPC_ENTRY
I_RpcServerInqAddressChangeFn();

RPC_STATUS RPC_ENTRY
I_RpcServerSetAddressChangeFn(
    IN RPC_ADDRESS_CHANGE_FN * pAddressChangeFn
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcConnectionInqSockBuffSize(
  OUT unsigned long __RPC_FAR * RecvBuffSize,
  OUT unsigned long __RPC_FAR * SendBuffSize
  );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcConnectionSetSockBuffSize(
   IN unsigned long RecvBuffSize,
   IN unsigned long SendBuffSize
   );

typedef
void
(*RPCLT_PDU_FILTER_FUNC) (
    IN void *Buffer,
    IN unsigned int BufferLength,
    IN int fDatagram
    );

typedef
void
(__cdecl *RPC_SETFILTER_FUNC) (
    IN RPCLT_PDU_FILTER_FUNC pfnFilter
    );

#ifndef WINNT
RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcServerStartListening(
    void * hWnd
    ) ;

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcServerStopListening(
    ) ;

typedef RPC_STATUS (*RPC_BLOCKING_FN) (
    IN void * hWnd,
    IN void * Context,
    IN OPTIONAL void * hSyncEvent
    ) ;

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcBindingSetAsync(
    IN RPC_BINDING_HANDLE Binding,
    IN RPC_BLOCKING_FN BlockingFn,
    IN unsigned long ServerTid
    ) ;

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcSetThreadParams(
    IN int fClientFree,
    IN OPTIONAL void *Context,
    IN OPTIONAL void * hWndClient
    );

RPCRTAPI
unsigned int
RPC_ENTRY
I_RpcWindowProc(
    IN void * hWnd,
    IN unsigned int Message,
    IN unsigned int wParam,
    IN unsigned long lParam
    ) ;

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcServerUnregisterEndpointA (
    IN unsigned char * Protseq,
    IN unsigned char * Endpoint
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcServerUnregisterEndpointW (
    IN unsigned short * Protseq,
    IN unsigned short * Endpoint
    );

#ifdef UNICODE
#define I_RpcServerUnregisterEndpoint I_RpcServerUnregisterEndpointW
#else
#define I_RpcServerUnregisterEndpoint I_RpcServerUnregisterEndpointA
#endif
#endif // WINNT

RPCRTAPI
RPC_STATUS
RPC_ENTRY
I_RpcServerInqTransportType(
    OUT unsigned int __RPC_FAR * Type
    ) ;

RPCRTAPI
long
RPC_ENTRY
I_RpcMapWin32Status (
    IN RPC_STATUS Status
    );


#ifdef __cplusplus
}
#endif

// Reset the packing level for Dos and Windows.

#if defined(__RPC_DOS__) || defined(__RPC_WIN16__)
#pragma pack()
#endif

#endif /* __RPCDCEP_H__ */

