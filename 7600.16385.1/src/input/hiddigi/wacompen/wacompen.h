/*++
    Copyright (c) Microsoft Corporation

    Module Name:
        wacompen.h

    Abstract:
        Contains OEM specific definitions.

    Environment:
        Kernel mode

--*/

#ifndef _WACOMPEN_H
#define _WACOMPEN_H

//
// Constants
//
#define OEM_VENDOR_ID           0x5c23      //"WAC"
#define MIN_PRESSURE_THRESHOLD  3
#define DEF_PRESSURE_THRESHOLD  16

#define HPENF_SERIAL_OPENED     0x80000000
#define HPENF_SERIAL_INITED     0x40000000

// OEM serial communication parameters
#define OEM_SERIAL_BAUDRATE     19200
#define OEM_SERIAL_WORDLEN      8
#define OEM_SERIAL_PARITY       NO_PARITY
#define OEM_SERIAL_STOPBITS     STOP_BIT_1

#define NORMALIZE_DATA(w)       ((USHORT)((((w) & 0x007f) << 7) |              \
                                          (((w) & 0x7f00) >> 8)))
#define LOW_X_BITS(w)           ((USHORT)(((w) & 0x6000) >> 13))
#define LOW_Y_BITS(w)           ((USHORT)(((w) & 0x1800) >> 11))
#define NORMALIZE_PRESSURE(w)   ((USHORT)(((w) & 0x007f) |                     \
                                          (((w) & 0x0100) >> 1)))
#define REBASE_PRESSURE(d,p,b)  ((p) = (USHORT)(p) - (USHORT)(b),                                   \
                                (p) = (USHORT)((p)*(d)->OemData.wPressureMax)/        \
                                ((d)->OemData.wPressureMax - (b)))
#define IsValidPacket(p)        (((p)->InputReport.bStatus & INSTATUS_SYNC) && \
                                 !((p)->InputReport.bStatus &                  \
                                   (INSTATUS_CONTROL_DATA | INSTATUS_RESERVED)) &&\
                                 !((p)->InputReport.wXData & INWDATA_SYNC) &&  \
                                 !((p)->InputReport.wYData & INWDATA_SYNC) &&  \
                                 !((p)->InputReport.wPressureData & INWDATA_SYNC) &&\
                                 !((p)->InputReport.bXTiltData & INBDATA_SYNC) &&\
                                 !((p)->InputReport.bYTiltData & INBDATA_SYNC))


#if _MSC_VER >= 1200

#pragma warning(push)

#endif

#pragma warning(disable:4201) // nameless struct/union

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
            USHORT wXData;
            USHORT wYData;
            USHORT wPressureData;
            UCHAR  bXTiltData;
            UCHAR  bYTiltData;
        } InputReport;
        UCHAR RawInput[9];
    };
} OEM_INPUT_REPORT, *POEM_INPUT_REPORT;

typedef struct _QUERY_DATA
{
    union
    {
        struct
        {
            UCHAR  bStatus;
            USHORT wXMax;
            USHORT wYMax;
            USHORT wPressureMax;
            UCHAR  bXTiltMax;
            UCHAR  bYTiltMax;
            USHORT wFirmwareVer;
        } QueryData;
        UCHAR RawData[11];
    };
} QUERYDATA, *PQUERYDATA;

typedef struct _HID_INPUT_REPORT
{
    UCHAR            ReportID;
    OEM_INPUT_REPORT Report;
#ifdef _TIMESTAMP_
    LARGE_INTEGER TimeStamp;
#endif
} HID_INPUT_REPORT, *PHID_INPUT_REPORT;

#if _MSC_VER >= 1200

#pragma warning(pop)

#else
#pragma warning(default:4201)
#endif


// bStatus bit values
#define INSTATUS_PEN_TIP_DOWN           0x01
#define INSTATUS_SIDESW_DOWN            0x02
#define INSTATUS_INVERTED               0x04
#define INSTATUS_ERASER_DOWN            0x08
#define INSTATUS_RESERVED               0x18
#define INSTATUS_IN_RANGE               0x20
#define INSTATUS_CONTROL_DATA           0x40
#define INSTATUS_SYNC                   0x80
#define INWDATA_SYNC                    0x8080
#define INBDATA_SYNC                    0x80
#include <poppack.h>

typedef struct _PRESSURE
{
    USHORT wTip;
    USHORT wEraser;
} PRESSURE, *PPRESSURE;

typedef struct _OEM_DATA
{
    SERIAL_BASIC_SETTINGS PrevSerialSettings;
    OEM_INPUT_REPORT      ResyncData[2];//resync data buffer
    ULONG                 BytesInBuff;  //number of bytes in the resync buffer
    USHORT                wProductID;
    USHORT                wXMax;
    USHORT                wYMax;
    USHORT                wPressureMax;
    UCHAR                 bXTiltMax;
    UCHAR                 bYTiltMax;
    USHORT                wFirmwareVer;
    UCHAR                 bLastStatus;
    PRESSURE              PressureBaseLines;
    PRESSURE              PressureThresholds;
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

//
// Global Data Declarations
//
extern UCHAR gReportDescriptor[];
extern ULONG gdwcbReportDescriptor;
extern HID_DESCRIPTOR gHidDescriptor;
extern PWSTR gpwstrManufacturerID;
extern PWSTR gpwstrProductID;
extern PWSTR gpwstrSerialNumber;

#endif  //ifndef _WACOMPEN_H

