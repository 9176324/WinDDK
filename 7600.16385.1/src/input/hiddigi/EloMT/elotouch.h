/*++
    Copyright (c) Microsoft Corporation

    Module Name:
        Elotouch.h

    Abstract:
        Contains OEM specific definitions.

    Environment:
        Kernel mode

--*/

#ifndef _ELOTOUCH_H
#define _ELOTOUCH_H

//
// Constants
//
#define OEM_VENDOR_ID           0x04E7      //"ELO"


//
// This must match with hardware, so make sure it is byte aligned.
//
#include <pshpack1.h>
typedef struct _OEM_INPUT_REPORT
{
    union
    {
        struct
        {
            UCHAR  bStatus;
            UCHAR  ContactId;
            USHORT wXData;
            USHORT wYData;            
            UCHAR  bStatus2;
            UCHAR  ContactId2;
            USHORT wXData2;
            USHORT wYData2;
            UCHAR  ActualCount;
        } InputReport;
        UCHAR RawInput[12];
    };
} OEM_INPUT_REPORT, *POEM_INPUT_REPORT;

typedef struct _OEM_RAW_SERIAL_PACKET_1
{
    union
    {
        struct
        {
            UCHAR  Status;
            UCHAR  Contacts;
            USHORT XData;
            USHORT YData;
            UCHAR  ContactId;
        }RawSerialData;
        UCHAR RawInput[7];
    };
}OEM_RAW_SERIAL_PACKET_1, *POEM_RAW_SERIAL_PACKET_1;

typedef struct _OEM_RAW_SERIAL_PACKET_2
{
    union
    {
        struct
        {
            UCHAR  Status;
            UCHAR  Contacts;
            USHORT XData;
            USHORT YData;
            UCHAR  ContactId;
            USHORT XData2;
            USHORT YData2;
            UCHAR  ContactId2;
        }RawSerialData;
        UCHAR RawInput[12];
    };
}OEM_RAW_SERIAL_PACKET_2, *POEM_RAW_SERIAL_PACKET_2;

typedef struct _OEM_MOUSE_REPORT
{
    union
    {
        struct
        {
            UCHAR   bButtons;
            USHORT  wXData;
            USHORT  wYData;
            USHORT  wReserved;
        }InputReport;
        UCHAR RawInput[7];
    };
}OEM_MOUSE_REPORT, *POEM_MOUSE_REPORT;

typedef struct _OEM_INPUT_MODE_REPORT
{
    UCHAR DeviceIndex;
    UCHAR InputMode;
}OEM_INPUT_MODE_REPORT, *POEM_INPUT_MODE_REPORT;

typedef struct _HID_FEATURE_REPORT
{
    UCHAR       ReportID;
    UCHAR       InputMode;
    UCHAR       DeviceIndex;
} HID_FEATURE_REPORT, *PHID_FEATURE_REPORT;

typedef struct _HID_MAX_COUNT_REPORT
{
    UCHAR      ReportID;
    UCHAR      MaxCount;
}HID_MAX_COUNT_REPORT, *PHID_MAX_COUNT_REPORT;

typedef struct _HID_INPUT_REPORT
{
    UCHAR            ReportID;
    union
    {
        OEM_INPUT_REPORT TouchReport;
        OEM_MOUSE_REPORT MouseReport;
    };
#ifdef _TIMESTAMP_
    LARGE_INTEGER TimeStamp;
#endif
} HID_INPUT_REPORT, *PHID_INPUT_REPORT;

// bStatus bit values
#define INSTATUS_INITIAL_TOUCH           0x01
#define INSTATUS_TOUCH_MOVE              0x02
#define INSTATUS_UNTOUCH                 0x04
#define INSTATUS_MULTI_TOUCH             0x08
#define INSTATUS_WARNING_PENDING         0x10
#define INSTATUS_RESERVED                0x20
#define INSTATUS_RANGE_CHECKING          0x40
#define INSTATUS_Z_MEASURE               0x80

#define FINGER_STATUS                   0x01 // finger down (either touch or mouse)
#define RANGE_STATUS                    0x02 // inrange
#define RANGE_FINGER_STATUS             0x03 // finger down (range + finge)
#define MAX_BUFFER_COUNT                sizeof(OEM_RAW_SERIAL_PACKET)*2

#include <poppack.h>

typedef struct _OEM_DATA
{
    SERIAL_BASIC_SETTINGS PrevSerialSettings;
    OEM_RAW_SERIAL_PACKET_2 ResyncData[2];//resync data buffer
    ULONG_PTR             BytesInBuff;  //number of bytes in the resync buffer
    USHORT                wProductID;
    USHORT                wXMax;
    USHORT                wYMax;
    USHORT                wFirmwareVer;
    UCHAR                 bLastStatus;
    BOOLEAN               CycleStarted; //used to let the read handler know the read requests will now be handled fromthe completion routine
#ifdef _TIMESTAMP_
    LARGE_INTEGER         LastCounter;
#endif
#ifdef DEBUG
    OEM_INPUT_REPORT      LastReport;
    ULONGLONG             LastReportTime;
    ULONGLONG             StartTime;
    USHORT                wMaxX;
    USHORT                wMaxY;
    ULONG                 dwcSamples;
    ULONG                 dwcLostBytes;
#endif
} OEM_DATA, *POEM_DATA;
// Constants
//
#define HPEN_POOL_TAG                   'molE'


// dwfHPen flag values
#define HPENF_DEVICE_STARTED            0x00000001
#define HPENF_DIGITIZER_STANDBY         0x00000002

#define ULONG_MAX                       0xffffffffUL

#define OEM_SERIAL_BAUDRATE     115200

#define OEM_STATUS_CHECK       2

#define ELO_SINGLE_TOUCH_COUNT       sizeof(OEM_RAW_SERIAL_PACKET_1)
#define ELO_TWO_TOUCH_COUNT         sizeof(OEM_RAW_SERIAL_PACKET_2)
#define ELO_STATUS_COUNT       2
#define ELO_FINGER_COUNT       1


#define ELO_MT_1_DOWN          0x05   // bit pattern when first finger is down
#define ELO_MT_2_DOWN          0x0a   // bit pattern when second finger is down
#define ELO_MT_BOTH_DOWN       0x0f   //bit pattern when both are down
#define ELO_MT_0_DOWN          0x00   //bit pattern when both finger are off the surface

#define ELO_MT_PACKET_SIZE     0x05   //bytes with actual data 2 for x 2 for y and 1for contact id.

#define ELO_MT_VALID_ID_1      0x00   //first id is always 0
#define ELO_MT_VALID_ID_2      0x01   //second id is always 1
#define ELO_MT_MAX_COUNT       0x02   //maximum number of contacts.
#define ELO_MT_STATUS_COUNT    0x02   //# bytes containing buttons and contact count
#define ELO_MT_CONTACT_1      0x01   //numbfer of contacts
#define ELO_MT_CONTACT_2       0x02   //numbfer of contacts

#define MULTITOUCH
//
// Macros
//
//
// Type Definitions
//

typedef struct _TOUCH_UNIT
{
    UCHAR  bStatus;
    USHORT xData;
    USHORT yData;
    UCHAR  identifier;
}TOUCH_UNIT, *PTOUCH_UNIT;

typedef enum _LAST_TOUCH_STATE
{
    TOUCH_NONE,
    TOUCH_ONE,
    TOUCH_TWO
}LAST_TOUCH_STATE, *PLAST_TOUCH_STATE;

typedef struct _DEVICE_EXTENSION
{
    ULONG                 dwfHPen;      //flags
    WDFDEVICE             Device;       //fdo
    //PDEVICE_OBJECT        pdo;          //pdo of the pen device
    WDFIOTARGET          IoTarget;  //points to the serial device object
    WDFSPINLOCK            SpinLock;     //to protect the resync buffer
    DEVICE_POWER_STATE    PowerState;   //power state of the digitizer
    OEM_DATA              OemData;      //OEM specific data 
    //WDFREQUEST           SerialDataRequest; //used to read data from the serial driver
    WDFQUEUE             PingPongQueue; //queue for storing ping pong IRPs
    UCHAR                InputMode; //device input mode
    BOOLEAN             bIsStateChange;
    TOUCH_UNIT           UnitOne;
#ifdef PRIORITY_UPDATE
    TOUCH_UNIT           UnitTwo;
    LAST_TOUCH_STATE     LastState;
    UCHAR               CurrentId;
    TOUCH_UNIT           LastResetUnit;    
#endif
    PTOUCH_UNIT          pCurrentUnit;
 } DEVICE_EXTENSION, *PDEVICE_EXTENSION;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_EXTENSION, GetDeviceContext)


typedef struct _WORKITEM_CONTEXT
{
    PDEVICE_EXTENSION   devContext;
}WORKITEM_CONTEXT, *PWORKITEM_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(WORKITEM_CONTEXT,
                                                GetWorkItemContext)


#define ISVALIDSTATUS(x)\
    (ELO_MT_1_DOWN == (x) || \
            ELO_MT_BOTH_DOWN == (x) || \
            ELO_MT_0_DOWN == (x) || \
            ELO_MT_2_DOWN == (x))\



NTSTATUS INTERNAL
ProcessInputData(
    __in PDEVICE_EXTENSION DevContext,
    __in PWDF_REQUEST_COMPLETION_PARAMS Params
    );

NTSTATUS INTERNAL
QueryDeviceInfo(
    __in PDEVICE_EXTENSION DevContext
    );

NTSTATUS INTERNAL
ProcessOEMBuffer(
    __in  PDEVICE_EXTENSION DevContext,
    __in  POEM_RAW_SERIAL_PACKET_2 SerialData
    );

NTSTATUS INTERNAL
TranslateRawDataToHID(
    __in  PDEVICE_EXTENSION DevContext,
    __in  POEM_RAW_SERIAL_PACKET_2 RawSerialData,
    __in  WDFREQUEST Request
    );

NTSTATUS
 ValidateOEMBuffer(
 __in_bcount(PacketSize)PUCHAR SerialPacket,
 __in ULONG  PacketSize
 );

NTSTATUS INTERNAL
NInputReadReport(
    __in  PDEVICE_EXTENSION DevContext,
    __in  WDFREQUEST        Request,
    __out BOOLEAN           *fSentDown
    );

NTSTATUS INTERNAL
NInputWriteReport(
    __in PDEVICE_EXTENSION DevContext,
    __in PIRP Irp
    );

NTSTATUS INTERNAL
NInputInitDevice(
     __in PDEVICE_EXTENSION DevContext
    );

NTSTATUS INTERNAL
NInputInitSerialPort(
    __in PDEVICE_EXTENSION DevContext
    );

NTSTATUS INTERNAL
NInputStartDevice(
    __in PDEVICE_EXTENSION DevContext
    );

NTSTATUS INTERNAL
NInputStopDevice(
    __in PDEVICE_EXTENSION DevContext
    );

EVT_WDF_WORKITEM ReadMoreBytes;

NTSTATUS INTERNAL
ValidateOEMHeader(
    __in_bcount(*BytesToValidate) PUCHAR TouchHeader, 
    __inout PULONG BytesToValidate
    );



#endif  //ifndef _WACOMPEN_H

