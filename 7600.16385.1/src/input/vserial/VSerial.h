/*++
    Copyright (c) 2006 Microsoft Corporation

    Module Name:
        VSerial.h

    Abstract:
        Contains definitions of all constants and data types for the
        serial pen hid driver.

    Environment:
        Kernel mode
--*/

#ifndef VSERIAL_H
#define VSERIAL_H

#include <ntddk.h>
#include <wdf.h>

#define HPEN_HARDWARE_IDS    L"ACPI\\MSVS010\0\0"
#define HPEN_HARDWARE_IDS_LENGTH sizeof (HPEN_HARDWARE_IDS)

typedef struct tagDEVICE_EXTENSION {
    KEVENT                           TerminateWorkerThread;
    ULONG                            WriteCount;
    ULONG                            ReadCount;
    PKTHREAD                         Thread;
    WDFKEY							 SimDevKey;
    CHAR                             DeviceData[500];
    ULONG                            ByteCount;
    HANDLE                           hDataFile;
    BOOLEAN                          IsDataFileActive;
    ULONG_PTR                        CurrentPos;
    ULONG_PTR                        LastPos;
    KSPIN_LOCK                       BufferLock;
    WDFQUEUE                         ReadQueue;
} SERIAL_DEVICE_EXTENSION, *PSERIAL_DEVICE_EXTENSION;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(SERIAL_DEVICE_EXTENSION, GetDeviceContext)

#define VSERIAL_TAG 'RLSV'

#define MAX_FILE_PATH               255
#define DEFAULT_FILE        L"\\??\\C:\\TempDir\\SimPenData.log"
#define CUSTOM_FILE_NAME    L"FileName"
#define SIMULATED_DEVICE_KEY_NAME L"\\Registry\\Machine\\Software\\Microsoft\\SimDevice"

#define READ_RESPONSE	    L"ReadResponse"
#define WRITE_RESPONSE	    L"WriteResponse"
#define CONTROL_RESPONSE    L"ControlResponse"
#define PNP_QUERY_RESPONSE  L"PnpQueryResponse"
#define CLOSE_RESPONSE      L"CloseResponse"
#define CREATE_RESPONSE     L"CreateResponse"
#define CLEANUP_RESPONSE    L"CleanupResponse"

#define IO_TIMEOUT          -40000000

///////////////////////////////////////////////////////////////////////////////
// Global functions

DRIVER_INITIALIZE DriverEntry;

//thread function
VOID ReadIrpThreadCompletionRoutine(PVOID pContext);
// I/O request handlers
KSTART_ROUTINE ReadIrpCompletionRoutine;

VOID
ReadIrpCompletionRoutine(
    __in PVOID pContext
    );
NTSTATUS
QueryDesiredReadWriteResponse(
    __in UCHAR IrpMajorFunc,
    __in PSERIAL_DEVICE_EXTENSION DevExt
);

NTSTATUS
QueryDesiredControlResponse(
    __in ULONG_PTR controlCode,
    __in PSERIAL_DEVICE_EXTENSION DevExt
);

NTSTATUS
ReadDeviceBuffer(
    __out_bcount_full(readLength) CHAR* dataBuffer,
    __in size_t readLength,
    __in PSERIAL_DEVICE_EXTENSION    DevExt
    );

NTSTATUS
FillDeviceBuffer(
    __in PSERIAL_DEVICE_EXTENSION    DevExt
    );
NTSTATUS
OpenDeviceDataFile(
    __in PSERIAL_DEVICE_EXTENSION    DevExt
    );

NTSTATUS
DeleteDeviceDataFile(
    __in PSERIAL_DEVICE_EXTENSION    DevExt
    );

EVT_WDF_DRIVER_DEVICE_ADD VSerialAddDevice;

EVT_WDF_DEVICE_SELF_MANAGED_IO_INIT VSerialEvtDeviceSelfManagedIoInit;

EVT_WDF_DEVICE_CONTEXT_CLEANUP VSerialDeviceContextCleanup;

EVT_WDF_IO_QUEUE_IO_WRITE VSerialEvtIoWrite;

EVT_WDF_IO_QUEUE_IO_READ VSerialEvtIoRead;

EVT_WDFDEVICE_WDM_IRP_PREPROCESS VSerialWdmFileCreate;

EVT_WDFDEVICE_WDM_IRP_PREPROCESS VSerialWdmFileCleanup;

EVT_WDFDEVICE_WDM_IRP_PREPROCESS VSerialWdmFileClose;

EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL VSerialEvtIoDeviceControl;

EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL VSerialEvtIoInternalDeviceControl;

EVT_WDFDEVICE_WDM_IRP_PREPROCESS VSerialPnpIrpPreprocessCallback;

NTSTATUS
RegQueryValue(
    __in  WDFKEY  hkey,
    __in  LPCWSTR pszValueName,
    __in  ULONG   dwType,
    __out_bcount_opt(dwcbData) PVOID   lpbData,
    __in  ULONG   dwcbData,
    __out_opt PULONG  pdwcbLen
    );

NTSTATUS
RegSetValue(
    __in WDFKEY  hkey,
    __in LPCWSTR pszValueName,
    __in ULONG   dwType,
    __in PUCHAR  lpbData,
    __in ULONG   dwcbData
    );

NTSTATUS
RegOpenDeviceKey(
	__in PSERIAL_DEVICE_EXTENSION DevExt
	);

#endif // VSERIAL_H



