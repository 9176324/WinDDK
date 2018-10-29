/*++
    Copyright (c) Microsoft Corporation

    Module Name:
        naturalInput.h

    Abstract:
        Contains OEM specific definitions.

    Environment:
        Kernel mode

--*/

#ifndef _NATURALINPUT_H
#define _NATURALINPUT_H

//
// Constants
//

#define HPENF_SERIAL_OPENED     0x80000000
#define HPENF_SERIAL_INITED     0x40000000

// OEM serial communication parameters
#define OEM_SERIAL_WORDLEN      8
#define OEM_SERIAL_PARITY       NO_PARITY
#define OEM_SERIAL_STOPBITS     STOP_BIT_1

#define OEM_PACKET_SIZE         10
//#define MAX_Y                   4000


//
// Global Data Declarations
//
extern UCHAR gReportDescriptor[];
extern ULONG gdwcbReportDescriptor;
extern HID_DESCRIPTOR gHidDescriptor;
extern PWSTR gpwstrManufacturerID;
extern PWSTR gpwstrProductID;
extern PWSTR gpwstrSerialNumber;

#define MODE_MOUSE                      0x00
#define MODE_SINGLE_TOUCH               0x01
#define MODE_MULTI_TOUCH                0x02



//
// Input Report IDs
//

#define REPORTID_FEATURE                7
#define REPORTID_MOUSE                  3
#define REPORTID_MTOUCH                 1
#define REPORTID_MAX_COUNT              8

// dwfHPen flag values
#define HPENF_DEVICE_STARTED            0x00000001
#define HPENF_DIGITIZER_STANDBY         0x00000002

#define ULONG_MAX                       0xffffffffUL
//
// Function prototypes
//
EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL NInputInternalDeviceControl;


NTSTATUS INTERNAL
NInputGetString(
    __in WDFDEVICE Device,
    __in WDFREQUEST Request
    );

NTSTATUS
NInputGetHidDescriptor(
    __in WDFDEVICE Device,
    __in WDFREQUEST Request
    );

NTSTATUS
NInputGetReportDescriptor(
    __in WDFDEVICE Device,
    __in WDFREQUEST Request
    );


NTSTATUS
NInputGetDeviceAttributes(
    __in WDFREQUEST Request
    );


NTSTATUS INTERNAL
NInputRegQueryValue(
    __in  WDFKEY  hkey,
    __in  LPCWSTR pszValueName,
    __in  ULONG   dwType,
    __out_ecount(dwcbData) PUCHAR  lpbData,
    __in  ULONG   dwcbData,
    __out_opt PULONG  pdwcbLen
    );

NTSTATUS INTERNAL
NInputRegSetValue(
    __in WDFKEY  hkey,
    __in LPCWSTR pszValueName,
    __in ULONG   dwType,
    __in PUCHAR  lpbData,
    __in ULONG   dwcbData
    );

EVT_WDF_DEVICE_D0_ENTRY NInputEvtDeviceD0Entry;


EVT_WDF_DEVICE_D0_EXIT NInputEvtDeviceD0Exit;


EVT_WDF_DEVICE_SELF_MANAGED_IO_INIT NInputEvtDeviceSelfManagedIoInit;

EVT_WDF_DEVICE_SELF_MANAGED_IO_SUSPEND NInputEvtDeviceSelfManagedIoSuspend;

EVT_WDF_DEVICE_SELF_MANAGED_IO_RESTART NInputEvtDeviceSelfManagedRestart;

EVT_WDF_DEVICE_QUERY_STOP  NInputEvtDeviceStop;

EVT_WDF_DEVICE_QUERY_REMOVE NInputEvtDeviceQueryRemove;
    
DRIVER_INITIALIZE DriverEntry;

EVT_WDF_DRIVER_DEVICE_ADD NInputDeviceAdd;

EVT_WDF_DEVICE_CONTEXT_CLEANUP NInputDriverContextCleanup;

EVT_WDF_REQUEST_COMPLETION_ROUTINE NInputReadReportCompletion;
/*
VOID
NInputReadReportCompletion(
    __in WDFREQUEST    Request,
    __in WDFIOTARGET   Target,
    __in PWDF_REQUEST_COMPLETION_PARAMS Params,
    __in WDFCONTEXT    Context
    );*/


NTSTATUS INTERNAL
SendSyncIrp(
    __in PDEVICE_OBJECT DevObj,
    __in PIRP           Irp,
    __in BOOLEAN        fCopyToNext
    );

NTSTATUS
NInputPnpIrpPreprocessCallback (
    __in WDFDEVICE Device,
    __in PIRP Irp
    );

NTSTATUS INTERNAL
NInputSetFeatureReport(
    __in WDFDEVICE Device,
    __in WDFREQUEST Request
    );

NTSTATUS INTERNAL
NInputGetFeatureReport(
    __in WDFDEVICE Device,
    __in WDFREQUEST Request
    );

NTSTATUS INTERNAL
NInputWakeDevice(
   __in WDFDEVICE Device
   );

NTSTATUS INTERNAL
NInputStandbyDevice(
   __in WDFDEVICE Device
   );


#endif  //ifndef 

