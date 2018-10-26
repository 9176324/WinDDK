/*++

Copyright (C) Microsoft Corporation, 1999

Module Name:

    exabyte2.h

Abstract:

    This file contains structures and defines that are used
    specifically for the tape drivers.

Revision History:

--*/

#ifndef _EXABYTE2_H
#define _EXABYTE2_H


//
//  Internal (module wide) defines that symbolize
//  density codes returned by/from EXB-8500 drives
//
#define EXB_XX00  0             // undetermined tape recording density
#define EXB_8200  20   // 0x14  // EXB-8200 tape recording density
#define EXB_8500  133  // 0x85  // EXB-8500 tape recording density

//
//  Internal (module wide) defines that symbolize
//  the 8mm drives supported by this module.
//
#define EXABYTE_8500    1  // aka the Maynard 5000
#define EXABYTE_8505    2  // An Exabyte 8500 with data compresion
#define IBM_8505        3  // OEM Exabyte 8500 with data compresion
#define EXABYTE_8500C   4  // 8500 with compression

//
// Define EXABYTE vendor unique mode select/sense information.
//

#define EXABYTE_MODE_LENGTH          0x11
#define EXABYTE_CARTRIDGE            0x80
#define EXABYTE_NO_DATA_DISCONNECT   0x20
#define EXABYTE_NO_BUSY_ENABLE       0x08
#define EXABYTE_EVEN_BYTE_DISCONNECT 0x04
#define EXABYTE_PARITY_ENABLE        0x02
#define EXABYTE_NO_AUTO_LOAD         0X01

#define EXA_SUPPORTED_TYPES 1

//
//  Function prototype(s) for internal function(s)
//
static  ULONG  WhichIsIt(IN PINQUIRYDATA InquiryData);

//
// Minitape extension definition.
//

typedef struct _MINITAPE_EXTENSION {

    ULONG   DriveID;
    ULONG   Capacity;
    ULONG   CurrentPartition;
} MINITAPE_EXTENSION, *PMINITAPE_EXTENSION;

//
// Command extension definition.
//

typedef struct _COMMAND_EXTENSION {

    ULONG   CurrentState;

} COMMAND_EXTENSION, *PCOMMAND_EXTENSION;


//
// Request structure used to determine cleaning needs, and remaining tape
// capacity.
//

typedef struct _EXB_SENSE_DATA {
    UCHAR ErrorCode:7;
    UCHAR Valid:1;
    UCHAR SegmentNumber;
    UCHAR SenseKey:4;
    UCHAR Reserved:1;
    UCHAR IncorrectLength:1;
    UCHAR EndOfMedia:1;
    UCHAR FileMark:1;
    UCHAR Information[4];
    UCHAR AdditionalSenseLength;
    UCHAR CommandSpecificInformation[4];
    UCHAR AdditionalSenseCode;
    UCHAR AdditionalSenseCodeQualifier;
    UCHAR Reserved2[2];
    UCHAR RWDataErrorCounter[3];
    UCHAR UnitSense[3];
    UCHAR Reserved4;
    UCHAR Remaining[3];
    UCHAR RetryCounters[2];
    UCHAR FSC;
} EXB_SENSE_DATA, *PEXB_SENSE_DATA;

//
// Bit definitions for UnitSense

//
// UnitSense[0]
//
#define EXB_MEDIA_ERROR          0x10
#define EXB_TAPE_MOTION_ERROR    0x04

//
// UnitSense[1]
//
#define EXB_WRITE_ERROR          0x04
#define EXB_SERVO_ERROR          0x02
#define EXB_FORMATTER_ERROR      0x01

//
// UnitSense[2]
//
#define EXB_WRITE_SPLICE_ERROR   0x03
#define EXB_DRIVE_NEEDS_CLEANING 0x08
#define EXB_DRIVE_CLEANED        0x10

//
// Error counter upper limits
//
#define TAPE_READ_ERROR_LIMIT        0x8000
#define TAPE_WRITE_ERROR_LIMIT       0x8000

#define TAPE_READ_WARNING_LIMIT      0x4000
#define TAPE_WRITE_WARNING_LIMIT     0x4000

//
// Defines for type of parameter
//
#define TotalCorrectedErrors            0x0003
#define TotalTimesAlgorithmProcessed    0x0004
#define TotalGroupsProcessed            0x0005
#define TotalUncorrectedErrors          0x0006

//
// Defines for Log Sense Pages
//
#define LOGSENSEPAGE0                        0x00
#define LOGSENSEPAGE2                        0x02
#define LOGSENSEPAGE3                        0x03

//
// Defined Log Sense Page Header
//

typedef struct _LOG_SENSE_PAGE_HEADER {

   UCHAR PageCode : 6;
   UCHAR Reserved1 : 2;
   UCHAR Reserved2;
   UCHAR Length[2];           // [0]=MSB ... [1]=LSB

} LOG_SENSE_PAGE_HEADER, *PLOG_SENSE_PAGE_HEADER;


//
// Defined Log Sense Parameter Header
//

typedef struct _LOG_SENSE_PARAMETER_HEADER {

   UCHAR ParameterCode[2];    // [0]=MSB ... [1]=LSB
   UCHAR LPBit     : 1;
   UCHAR Reserved1 : 1;
   UCHAR TMCBit    : 2;
   UCHAR ETCBit    : 1;
   UCHAR TSDBit    : 1;
   UCHAR DSBit     : 1;
   UCHAR DUBit     : 1;
   UCHAR ParameterLength;

} LOG_SENSE_PARAMETER_HEADER, *PLOG_SENSE_PARAMETER_HEADER;

//
// Defined Log Page Information - statistical values, accounts
// for maximum parameter values that is returned for each page
//

typedef struct _LOG_SENSE_PAGE_INFORMATION {

   union {

       struct {
          UCHAR Page0;
          UCHAR Page2;
          UCHAR Page3;
       } PageData ;

       struct {
          LOG_SENSE_PARAMETER_HEADER Parm1;
          UCHAR TotalNumberOfErrors[3];
          LOG_SENSE_PARAMETER_HEADER Parm2;
          UCHAR TotalErrorsCorrected[3];
          LOG_SENSE_PARAMETER_HEADER Parm3;
          UCHAR TotalTimesAlgoProcessed[3];
          LOG_SENSE_PARAMETER_HEADER Parm4;
          UCHAR TotalGroupsWritten[5];
          LOG_SENSE_PARAMETER_HEADER Parm5;
          UCHAR TotalErrorsUncorrected[2];
       } Page2 ;

       struct {
           LOG_SENSE_PARAMETER_HEADER Parm1;
           UCHAR TotalNumberOfErrors[3];
           LOG_SENSE_PARAMETER_HEADER Parm2;
           UCHAR TotalErrorsCorrected[3];
           LOG_SENSE_PARAMETER_HEADER Parm3;
           UCHAR TotalTimesAlgoProcessed[3];
           LOG_SENSE_PARAMETER_HEADER Parm4;
           UCHAR TotalGroupsWritten[5];
           LOG_SENSE_PARAMETER_HEADER Parm5;
           UCHAR TotalErrorsUncorrected[2];
       } Page3 ;
   } LogSensePage;

} LOG_SENSE_PAGE_INFORMATION, *PLOG_SENSE_PAGE_INFORMATION;

//
// Defined Log Sense Parameter Format - statistical values, accounts
// for maximum parameter values that is returned
//

typedef struct _LOG_SENSE_PARAMETER_FORMAT {

   LOG_SENSE_PAGE_HEADER       LogSenseHeader;
   LOG_SENSE_PAGE_INFORMATION  LogSensePageInfo;

} LOG_SENSE_PARAMETER_FORMAT, *PLOG_SENSE_PARAMETER_FORMAT;


BOOLEAN
VerifyInquiry(
    IN  PINQUIRYDATA            InquiryData,
    IN  PMODE_CAPABILITIES_PAGE ModeCapabilitiesPage
    );

VOID
ExtensionInit(
    OUT PVOID                   MinitapeExtension,
    IN  PINQUIRYDATA            InquiryData,
    IN  PMODE_CAPABILITIES_PAGE ModeCapabilitiesPage
    );

TAPE_STATUS
CreatePartition(
    IN OUT  PVOID               MinitapeExtension,
    IN OUT  PVOID               CommandExtension,
    IN OUT  PVOID               CommandParameters,
    IN OUT  PSCSI_REQUEST_BLOCK Srb,
    IN      ULONG               CallNumber,
    IN      TAPE_STATUS         LastError,
    IN OUT  PULONG              RetryFlags
    );

TAPE_STATUS
Erase(
    IN OUT  PVOID               MinitapeExtension,
    IN OUT  PVOID               CommandExtension,
    IN OUT  PVOID               CommandParameters,
    IN OUT  PSCSI_REQUEST_BLOCK Srb,
    IN      ULONG               CallNumber,
    IN      TAPE_STATUS         LastError,
    IN OUT  PULONG              RetryFlags
    );

TAPE_STATUS
GetDriveParameters(
    IN OUT  PVOID               MinitapeExtension,
    IN OUT  PVOID               CommandExtension,
    IN OUT  PVOID               CommandParameters,
    IN OUT  PSCSI_REQUEST_BLOCK Srb,
    IN      ULONG               CallNumber,
    IN      TAPE_STATUS         LastError,
    IN OUT  PULONG              RetryFlags
    );

TAPE_STATUS
GetMediaParameters(
    IN OUT  PVOID               MinitapeExtension,
    IN OUT  PVOID               CommandExtension,
    IN OUT  PVOID               CommandParameters,
    IN OUT  PSCSI_REQUEST_BLOCK Srb,
    IN      ULONG               CallNumber,
    IN      TAPE_STATUS         LastError,
    IN OUT  PULONG              RetryFlags
    );

TAPE_STATUS
GetPosition(
    IN OUT  PVOID               MinitapeExtension,
    IN OUT  PVOID               CommandExtension,
    IN OUT  PVOID               CommandParameters,
    IN OUT  PSCSI_REQUEST_BLOCK Srb,
    IN      ULONG               CallNumber,
    IN      TAPE_STATUS         LastError,
    IN OUT  PULONG              RetryFlags
    );

TAPE_STATUS
GetStatus(
    IN OUT  PVOID               MinitapeExtension,
    IN OUT  PVOID               CommandExtension,
    IN OUT  PVOID               CommandParameters,
    IN OUT  PSCSI_REQUEST_BLOCK Srb,
    IN      ULONG               CallNumber,
    IN      TAPE_STATUS         LastError,
    IN OUT  PULONG              RetryFlags
    );

TAPE_STATUS
Prepare(
    IN OUT  PVOID               MinitapeExtension,
    IN OUT  PVOID               CommandExtension,
    IN OUT  PVOID               CommandParameters,
    IN OUT  PSCSI_REQUEST_BLOCK Srb,
    IN      ULONG               CallNumber,
    IN      TAPE_STATUS         LastError,
    IN OUT  PULONG              RetryFlags
    );

TAPE_STATUS
SetDriveParameters(
    IN OUT  PVOID               MinitapeExtension,
    IN OUT  PVOID               CommandExtension,
    IN OUT  PVOID               CommandParameters,
    IN OUT  PSCSI_REQUEST_BLOCK Srb,
    IN      ULONG               CallNumber,
    IN      TAPE_STATUS         LastError,
    IN OUT  PULONG              RetryFlags
    );

TAPE_STATUS
SetMediaParameters(
    IN OUT  PVOID               MinitapeExtension,
    IN OUT  PVOID               CommandExtension,
    IN OUT  PVOID               CommandParameters,
    IN OUT  PSCSI_REQUEST_BLOCK Srb,
    IN      ULONG               CallNumber,
    IN      TAPE_STATUS         LastError,
    IN OUT  PULONG              RetryFlags
    );

TAPE_STATUS
SetPosition(
    IN OUT  PVOID               MinitapeExtension,
    IN OUT  PVOID               CommandExtension,
    IN OUT  PVOID               CommandParameters,
    IN OUT  PSCSI_REQUEST_BLOCK Srb,
    IN      ULONG               CallNumber,
    IN      TAPE_STATUS         LastError,
    IN OUT  PULONG              RetryFlags
    );

TAPE_STATUS
WriteMarks(
    IN OUT  PVOID               MinitapeExtension,
    IN OUT  PVOID               CommandExtension,
    IN OUT  PVOID               CommandParameters,
    IN OUT  PSCSI_REQUEST_BLOCK Srb,
    IN      ULONG               CallNumber,
    IN      TAPE_STATUS         LastError,
    IN OUT  PULONG              RetryFlags
    );


TAPE_STATUS
GetMediaTypes(
    IN OUT  PVOID               MinitapeExtension,
    IN OUT  PVOID               CommandExtension,
    IN OUT  PVOID               CommandParameters,
    IN OUT  PSCSI_REQUEST_BLOCK Srb,
    IN      ULONG               CallNumber,
    IN      TAPE_STATUS         LastError,
    IN OUT  PULONG              RetryFlags
    );

VOID
TapeError(
    IN OUT  PVOID               MinitapeExtension,
    IN OUT  PSCSI_REQUEST_BLOCK Srb,
    IN OUT  TAPE_STATUS         *LastError
    );

TAPE_STATUS
TapeWMIControl(
    IN OUT  PVOID               MinitapeExtension,
    IN OUT  PVOID               CommandExtension,
    IN OUT  PVOID               CommandParameters,
    IN OUT  PSCSI_REQUEST_BLOCK Srb,
    IN      ULONG               CallNumber,
    IN      TAPE_STATUS         LastError,
    IN OUT  PULONG              RetryFlags
    );

//
// Internal routines wmi
//

TAPE_STATUS
QueryIoErrorData(
  IN OUT  PVOID               MinitapeExtension,
  IN OUT  PVOID               CommandExtension,
  IN OUT  PVOID               CommandParameters,
  IN OUT  PSCSI_REQUEST_BLOCK Srb,
  IN      ULONG               CallNumber,
  IN      TAPE_STATUS         LastError,
  IN OUT  PULONG              RetryFlags
  );

TAPE_STATUS
QueryDeviceErrorData(
  IN OUT  PVOID               MinitapeExtension,
  IN OUT  PVOID               CommandExtension,
  IN OUT  PVOID               CommandParameters,
  IN OUT  PSCSI_REQUEST_BLOCK Srb,
  IN      ULONG               CallNumber,
  IN      TAPE_STATUS         LastError,
  IN OUT  PULONG              RetryFlags
  );

VOID
ProcessReadWriteErrors(
    IN PSCSI_REQUEST_BLOCK Srb,
    IN BOOLEAN Read,
    IN OUT PWMI_TAPE_PROBLEM_IO_ERROR IoErrorData
  );

TAPE_DRIVE_PROBLEM_TYPE
VerifyReadWriteErrors(
   IN PWMI_TAPE_PROBLEM_IO_ERROR IoErrorData
   );

#endif // _EXABYTE2_H

