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
#define SERIAL_IOC_FCR_RCVR_TRIGGER_01_BYTE     0
#define SERIAL_IOC_FCR_RCVR_TRIGGER_04_BYTES    SERIAL_IOC_FCR_RCVR_TRIGGER_LSB
#define SERIAL_IOC_FCR_RCVR_TRIGGER_08_BYTES    SERIAL_IOC_FCR_RCVR_TRIGGER_MSB
#define SERIAL_IOC_FCR_RCVR_TRIGGER_14_BYTES    (SERIAL_IOC_FCR_RCVR_TRIGGER_LSB |\
                                                 SERIAL_IOC_FCR_RCVR_TRIGGER_MSB)



typedef enum {

   IRPLOCK_CANCELABLE,
   IRPLOCK_CANCEL_STARTED,
   IRPLOCK_CANCEL_COMPLETE,
   IRPLOCK_COMPLETED

} IRPLOCK, *PIRPLOCK;


//
// Function prototypes
//

NTSTATUS INTERNAL
SerialSyncSendIoctl(
    __in ULONG          IoctlCode,
    __in PDEVICE_OBJECT DevObj,
    __in_bcount_opt(InBufferLen) PVOID          InBuffer,
    __in ULONG          InBufferLen,
    __out_bcount_opt(OutBufferLen) PVOID         OutBuffer,
    __in ULONG          OutBufferLen,
    __in BOOLEAN        fInternal,
    __out PIO_STATUS_BLOCK Iosb
    );

NTSTATUS INTERNAL
SerialAsyncReadWriteData(
    __in BOOLEAN                fRead,
    __in PDEVICE_EXTENSION      DevExt,
    __in PIRP                   Irp,
    __in PUCHAR                 Buffer,
    __in ULONG                  BuffLen,
    __in PIO_COMPLETION_ROUTINE CompletionRoutine,
    __in PVOID                  Context
    );

NTSTATUS INTERNAL
SerialAsyncReadWritePort(
    __in BOOLEAN           fRead,
    __in PDEVICE_EXTENSION DevExt,
    __in_bcount_opt(BuffLen) PUCHAR            Buffer,
    __in ULONG             BuffLen
    );


IO_COMPLETION_ROUTINE AsyncReadWriteCompletion;


NTSTATUS INTERNAL
AsyncReadWriteCompletion(
    IN PDEVICE_OBJECT    DevObj,
    IN PIRP              Irp,
    IN PVOID             Context
    );

NTSTATUS INTERNAL
SerialSyncReadWritePort(
    __in BOOLEAN           fRead,
    __in PDEVICE_EXTENSION DevExt,
    __in_bcount_opt(BuffLen) PUCHAR            Buffer,
    __in ULONG             BuffLen,
    __in PIRPLOCK          Lock,
    __in_opt PLARGE_INTEGER Timeout,
    __out_opt PULONG       BytesAccessed
    );

IO_COMPLETION_ROUTINE SyncReadWriteCompletion;

NTSTATUS
SyncReadWriteCompletion(
    IN PDEVICE_OBJECT   DevObj,
    IN PIRP             Irp,
    IN PVOID            Context
    );

#endif  //ifndef _SERIAL_H

