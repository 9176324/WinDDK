/*++
    Copyright (c) Microsoft Corporation

    Module Name:
        serial.h

    Abstract:
        Contains definitions of all Serial Port related functions.

    Environment:
        Kernel mode

--*/

#ifndef _SERIAL_H
#define _SERIAL_H

//
// Constants
//

// Serial Port FIFO Control Register bits for Receiver Trigger Level
#define SERIAL_IOC_FCR_FIFO_DISABLE             0
#define SERIAL_IOC_FCR_RCVR_TRIGGER_01_BYTE     0
#define SERIAL_IOC_FCR_RCVR_TRIGGER_04_BYTES    SERIAL_IOC_FCR_RCVR_TRIGGER_LSB
#define SERIAL_IOC_FCR_RCVR_TRIGGER_08_BYTES    SERIAL_IOC_FCR_RCVR_TRIGGER_MSB
#define SERIAL_IOC_FCR_RCVR_TRIGGER_14_BYTES    (SERIAL_IOC_FCR_RCVR_TRIGGER_LSB |\
                                                 SERIAL_IOC_FCR_RCVR_TRIGGER_MSB)


#define SERIAL_TIME_OUT     5
//
// Function prototypes
//

NTSTATUS INTERNAL
SerialSyncSendIoctl(
    __in ULONG          IoctlCode,
    __in WDFIOTARGET    nextLowerDriver,
    __in_bcount_opt(InBufferLen) PVOID      InBuffer,
    __in ULONG          InBufferLen,
    __out_bcount_opt(OutBufferLen) PVOID     OutBuffer,
    __in ULONG          OutBufferLen,
    __in BOOLEAN        fInternal
    );

NTSTATUS INTERNAL
SerialAsyncReadData(
    __in PDEVICE_EXTENSION      DevContext,
    __in ULONG                  BuffLen,
    __in PFN_WDF_REQUEST_COMPLETION_ROUTINE CompletionRoutine,
    __in_opt PVOID                  Context
    );

NTSTATUS INTERNAL
SerialAsyncReadWritePort(
    __in BOOLEAN           fRead,
    __in PDEVICE_EXTENSION DevContext,
    __in PUCHAR            Buffer,
    __in ULONG             BuffLen
    );


EVT_WDF_REQUEST_COMPLETION_ROUTINE AsyncReadWriteCompletion;
/*VOID INTERNAL
AsyncReadWriteCompletion(
    IN WDFREQUEST    Request,
    IN WDFIOTARGET   Target,
    IN PWDF_REQUEST_COMPLETION_PARAMS Params,
    IN WDFCONTEXT    Context
    );*/

NTSTATUS INTERNAL
SerialSyncReadWritePort(
    __in BOOLEAN           fRead,
    __in PDEVICE_EXTENSION DevContext,
    __in_bcount(BuffLen) PUCHAR  Buffer,
    __in ULONG             BuffLen,
    __in_opt PWDF_REQUEST_SEND_OPTIONS   RequestOptions,
    __out_opt PULONG_PTR           BytesAccessed
    );

NTSTATUS INTERNAL
SerialClosePort(
    __in PDEVICE_EXTENSION DevContext
    );

NTSTATUS INTERNAL
SerialSendInitIRP(
    __in PDEVICE_EXTENSION DevContext,
    __in UCHAR           MajorFunction
    );

NTSTATUS INTERNAL
SerialRequestMoreData(
         __in PDEVICE_EXTENSION DevContext          
         );



#endif  //ifndef _SERIAL_H

