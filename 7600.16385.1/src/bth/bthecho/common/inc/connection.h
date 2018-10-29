/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    Connection.c

Abstract:

    Declaration of connection object for L2CA connection

Environment:

    Kernel mode

--*/

typedef struct _BTHECHO_CONNECTION * PBTHECHO_CONNECTION;

#define BTHECHOSAMPLE_NUM_CONTINUOUS_READERS 2

typedef VOID
(*PFN_BTHECHO_CONNECTION_OBJECT_CONTREADER_READ_COMPLETE) (
    __in PBTHECHOSAMPLE_DEVICE_CONTEXT_HEADER DevCtxHdr,
    __in PBTHECHO_CONNECTION Connection,
    __in PVOID Buffer,
    __in size_t BufferSize
    );

typedef VOID
(*PFN_BTHECHO_CONNECTION_OBJECT_CONTREADER_FAILED) (
    __in PBTHECHOSAMPLE_DEVICE_CONTEXT_HEADER DevCtxHdr,
    __in PBTHECHO_CONNECTION Connection
    );


typedef struct _BTHECHO_REPEAT_READER
{
    //
    // BRB used for transfer
    //
    struct _BRB_L2CA_ACL_TRANSFER TransferBrb;    

    //
    // WDF Request used for pending I/O
    //
    WDFREQUEST RequestPendingRead;

    //
    // Data buffer for pending read
    //
    WDFMEMORY MemoryPendingRead;

    //
    // Dpc for resubmitting pending read
    //
    KDPC ResubmitDpc;

    //
    // Whether the continuous reader is transitioning to stopped state
    //

    LONG Stopping;

    //
    // Event used to wait for read completion while stopping the continuos reader
    //
    KEVENT StopEvent;
    
    //
    // Back pointer to connection
    //
    PBTHECHO_CONNECTION Connection;
    
} BTHECHO_REPEAT_READER, *PBTHECHO_REPEAT_READER;

//
// Connection state
//

typedef enum _BTHECHO_CONNECTION_STATE {
    ConnectionStateUnitialized = 0,
    ConnectionStateInitialized,        
    ConnectionStateConnecting,
    ConnectionStateConnected,
    ConnectionStateConnectFailed,
    ConnectionStateDisconnecting,
    ConnectionStateDisconnected
    
} BTHECHOSAMPLE_CONNECTION_STATE, *PBTHECHOSAMPLE_CONNECTION_STATE;

typedef struct _BTHECHO_CONTINUOUS_READER {

    BTHECHO_REPEAT_READER         
        RepeatReaders[BTHECHOSAMPLE_NUM_CONTINUOUS_READERS];

    PFN_BTHECHO_CONNECTION_OBJECT_CONTREADER_READ_COMPLETE    
        BthEchoConnectionObjectContReaderReadCompleteCallback;

    PFN_BTHECHO_CONNECTION_OBJECT_CONTREADER_FAILED           
        BthEchoConnectionObjectContReaderFailedCallback;
        
    DWORD                                   InitializedReadersCount;
    
} BTHECHO_CONTINUOUS_READER, * PBTHECHO_CONTINUOUS_READER;

//
// Connection data structure for L2Ca connection
//

typedef struct _BTHECHO_CONNECTION {

    //
    // List entry for connection list maintained at device level
    //
    LIST_ENTRY                              ConnectionListEntry;

    PBTHECHOSAMPLE_DEVICE_CONTEXT_HEADER    DevCtxHdr;    

    BTHECHOSAMPLE_CONNECTION_STATE          ConnectionState;
    
    //
    // Connection lock, used to synchronize access to _BTHECHO_CONNECTION data structure
    //
    WDFSPINLOCK                             ConnectionLock;

    USHORT                                  OutMTU;
    USHORT                                  InMTU;

    L2CAP_CHANNEL_HANDLE                    ChannelHandle;
    BTH_ADDR                                RemoteAddress;

    //
    // Preallocated Brb, Request used for connect/disconnect
    //
    struct _BRB                             ConnectDisconnectBrb;
    WDFREQUEST                              ConnectDisconnectRequest;

    //
    // Event used to wait for disconnection
    // It is non-signaled when connection is in ConnectionStateDisconnecting
    // transitionary state and signaled otherwise
    //
    KEVENT                                  DisconnectEvent;

    //
    // Continuous readers (used only by server)
    // PLEASE NOTE that KMDF USB Pipe Target uses a single continuous reader
    //
    BTHECHO_CONTINUOUS_READER               ContinuousReader;
} BTHECHO_CONNECTION, *PBTHECHO_CONNECTION;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(BTHECHO_CONNECTION, GetConnectionObjectContext)

__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
BthEchoConnectionObjectCreate(
    __in PBTHECHOSAMPLE_DEVICE_CONTEXT_HEADER DevCtxHdr,
    __in WDFOBJECT ParentObject,
    __out WDFOBJECT*  ConnectionObject
    );

__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
BthEchoConnectionObjectInitializeContinuousReader(
    __in PBTHECHO_CONNECTION Connection,
    __in PFN_BTHECHO_CONNECTION_OBJECT_CONTREADER_READ_COMPLETE 
        BthEchoConnectionObjectContReaderReadCompleteCallback,
    __in PFN_BTHECHO_CONNECTION_OBJECT_CONTREADER_FAILED 
        BthEchoConnectionObjectContReaderFailedCallback,
    __in size_t BufferSize
    );

__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
BthEchoConnectionObjectContinuousReaderSubmitReaders(
    __in PBTHECHO_CONNECTION Connection
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
BthEchoConnectionObjectContinuousReaderCancelReaders(
    __in PBTHECHO_CONNECTION Connection
    );

__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
BthEchoConnectionObjectRemoteDisconnect(
    __in PBTHECHOSAMPLE_DEVICE_CONTEXT_HEADER DevCtxHdr,
    __in PBTHECHO_CONNECTION Connection
    );

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
BthEchoConnectionObjectRemoteDisconnectSynchronously(
    __in PBTHECHOSAMPLE_DEVICE_CONTEXT_HEADER DevCtxHdr,
    __in PBTHECHO_CONNECTION Connection
    );

__drv_maxIRQL(DISPATCH_LEVEL)
__drv_when(return==0, __drv_allocatesMem(Mem))
NTSTATUS
BthEchoConnectionObjectFormatRequestForL2CaTransfer(
    __in PBTHECHO_CONNECTION Connection,
    __in WDFREQUEST Request,
    __inout struct _BRB_L2CA_ACL_TRANSFER ** Brb,
    __in WDFMEMORY Memory,
    __in ULONG TransferFlags //flags include direction of transfer
    );

EVT_WDF_OBJECT_CONTEXT_CLEANUP BthEchoEvtConnectionObjectCleanup;


