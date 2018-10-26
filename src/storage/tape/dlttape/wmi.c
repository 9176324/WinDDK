/*++

Copyright (c) 1999 Microsoft

Module Name:

    wmi.c

Abstract:

    This module contains WMI routines for dlt tape drives.

Environment:

    kernel mode only

Revision History:

--*/

#include "minitape.h"
#include "dlttape.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, TapeWMIControl)
#pragma alloc_text(PAGE, GetNumberOfBytesReturned)
#pragma alloc_text(PAGE, PrepareSrbForTapeAlertInfo)
#pragma alloc_text(PAGE, ProcessReadWriteErrors)
#pragma alloc_text(PAGE, QueryDeviceErrorData)
#pragma alloc_text(PAGE, QueryIoErrorData)
#pragma alloc_text(PAGE, VerifyReadWriteErrors)
#endif

TAPE_STATUS
TapeWMIControl(
    IN OUT  PVOID               MinitapeExtension,
    IN OUT  PVOID               CommandExtension,
    IN OUT  PVOID               CommandParameters,
    IN OUT  PSCSI_REQUEST_BLOCK Srb,
    IN      ULONG               CallNumber,
    IN      TAPE_STATUS         LastError,
    IN OUT  PULONG              RetryFlags
    )
/*+
Routine Description:

   This is the common entry point for all WMI calls from tape class driver.
   
Arguments:

   MinitapeExtension   Pointer to the minidriver's device extension
   CommandExtension    Pointer to the minidriver's command extension
   CommandParameters   Pointer to TAPE_WMI_OPERATIONS struct
   Srb                 SCSI Request block
   CallNumber          Call sequence number
   LastError           Last command error
   RetryFlags          Bit mask for retrying commands
   
Return value:

   TAPE_STATUS
-*/
{
   PTAPE_WMI_OPERATIONS wmiOperations;
   PMINITAPE_EXTENSION miniExtension;

   miniExtension = (PMINITAPE_EXTENSION)MinitapeExtension;
   wmiOperations = (PTAPE_WMI_OPERATIONS)CommandParameters;

   switch (wmiOperations->Method) {
      case TAPE_CHECK_FOR_DRIVE_PROBLEM: {
         if ((miniExtension->DriveID == COMPAQ_8000) ||
             (miniExtension->DriveID == QUANTUM_8000)){
            return QueryDeviceErrorData(MinitapeExtension, 
                                        CommandExtension,
                                        CommandParameters, 
                                        Srb, CallNumber,
                                        LastError, RetryFlags);
         } else {
            return QueryIoErrorData(MinitapeExtension, CommandExtension,
                                    CommandParameters, Srb, CallNumber,
                                    LastError, RetryFlags);
         }
         break;
      }

      case TAPE_QUERY_IO_ERROR_DATA: {
         return QueryIoErrorData(MinitapeExtension, CommandExtension,
                                 CommandParameters, Srb, CallNumber,
                                 LastError, RetryFlags);
         break;
      }

      case TAPE_QUERY_DEVICE_ERROR_DATA: {
         return QueryDeviceErrorData(MinitapeExtension, CommandExtension,
                                     CommandParameters, Srb, CallNumber,
                                     LastError, RetryFlags);
         break;
      }

      default: {
         return TAPE_STATUS_INVALID_DEVICE_REQUEST;
         break;
      }
   } // switch (wmiOperations->Method) 
}

TAPE_STATUS
QueryIoErrorData(
  IN OUT  PVOID               MinitapeExtension,
  IN OUT  PVOID               CommandExtension,
  IN OUT  PVOID               CommandParameters,
  IN OUT  PSCSI_REQUEST_BLOCK Srb,
  IN      ULONG               CallNumber,
  IN      TAPE_STATUS         LastError,
  IN OUT  PULONG              RetryFlags
  )

/*+
Routine Description:

   This routine returns IO Error data such as read\write errors.
   
Arguments:

   MinitapeExtension   Pointer to the minidriver's device extension
   CommandExtension    Pointer to the minidriver's command extension
   CommandParameters   Pointer to TAPE_WMI_OPERATIONS struct
   Srb                 SCSI Request block
   CallNumber          Call sequence number
   LastError           Last command error
   RetryFlags          Bit mask for retrying commands
   
Return value:

   TAPE_STATUS
-*/
{
   PTAPE_WMI_OPERATIONS wmiOperations;
   PWMI_TAPE_PROBLEM_IO_ERROR IoErrorData;
   PWMI_TAPE_PROBLEM_WARNING wmiData;
   TAPE_STATUS  status = TAPE_STATUS_SUCCESS;
   PCDB cdb = (PCDB)Srb->Cdb;

   wmiOperations = (PTAPE_WMI_OPERATIONS)CommandParameters;
   wmiData = (PWMI_TAPE_PROBLEM_WARNING)wmiOperations->DataBuffer;
   IoErrorData = (PWMI_TAPE_PROBLEM_IO_ERROR)&(wmiData->TapeData);

   if (CallNumber == 0) {
      if (!TapeClassAllocateSrbBuffer(Srb, 
                                      sizeof(LOG_SENSE_PARAMETER_FORMAT))) {
         DebugPrint((1, "QueryIoErrorData : No memory for log sense info\n"));
         return TAPE_STATUS_INSUFFICIENT_RESOURCES;
      }

      wmiData->DriveProblemType = TapeDriveProblemNone;
      TapeClassZeroMemory(IoErrorData, 
                          sizeof(WMI_TAPE_PROBLEM_IO_ERROR));
      TapeClassZeroMemory(cdb, MAXIMUM_CDB_SIZE);

      //
      // Prepare SCSI command (CDB)
      //
      Srb->CdbLength = CDB10GENERIC_LENGTH;

      cdb->LOGSENSE.OperationCode = SCSIOP_LOG_SENSE;
      cdb->LOGSENSE.PageCode = LOGSENSEPAGE2;
      cdb->LOGSENSE.PCBit = 1;
      cdb->LOGSENSE.AllocationLength[0] = sizeof(LOG_SENSE_PARAMETER_FORMAT) >> 8;
      cdb->LOGSENSE.AllocationLength[1] = sizeof(LOG_SENSE_PARAMETER_FORMAT);

      Srb->DataTransferLength = sizeof(LOG_SENSE_PARAMETER_FORMAT);
      return  TAPE_STATUS_SEND_SRB_AND_CALLBACK;
   }

   if (CallNumber == 1) {
      ProcessReadWriteErrors(Srb, FALSE, IoErrorData);

      TapeClassZeroMemory(cdb, MAXIMUM_CDB_SIZE);

      //
      // Prepare SCSI command (CDB)
      //
      Srb->CdbLength = CDB10GENERIC_LENGTH;

      cdb->LOGSENSE.OperationCode = SCSIOP_LOG_SENSE;
      cdb->LOGSENSE.PageCode = LOGSENSEPAGE3;
      cdb->LOGSENSE.PCBit = 1;
      cdb->LOGSENSE.AllocationLength[0] = sizeof(LOG_SENSE_PARAMETER_FORMAT) >> 8;
      cdb->LOGSENSE.AllocationLength[1] = sizeof(LOG_SENSE_PARAMETER_FORMAT);

      Srb->DataTransferLength = sizeof(LOG_SENSE_PARAMETER_FORMAT);
      return  TAPE_STATUS_SEND_SRB_AND_CALLBACK;
   }

   if (CallNumber == 2) {
      ProcessReadWriteErrors(Srb, TRUE, IoErrorData);
      
      wmiData->DriveProblemType = VerifyReadWriteErrors(IoErrorData);
      DebugPrint((3, "QueryIoErrorData: DriveProblemType %x\n",
                  wmiData->DriveProblemType));
   }

   return TAPE_STATUS_SUCCESS;
}

VOID
ProcessReadWriteErrors(
    IN PSCSI_REQUEST_BLOCK Srb,
    IN BOOLEAN Read,
    IN OUT PWMI_TAPE_PROBLEM_IO_ERROR IoErrorData
)
/*+
Routine Description :

   This routine processes the buffer containing read\write counters,
   and sets the appropriate fields in WMI_TAPE_PROBLEM_IO_ERROR 
   buffer.
   
Arguments :

 Srb            SCSI Request Block
 Read           TRUE if we are to process read counters. FALSE if it is 
                Write counters
 IoErrorData    Buffer in which to return counter values.
 
Return Value :

  None  
-*/
{
   USHORT paramCode;
   UCHAR  paramLen;
   LONG  bytesLeft;
   PLOG_SENSE_PAGE_HEADER logSenseHeader;
   PLOG_SENSE_PARAMETER_HEADER logSenseParamHeader;
   PUCHAR  paramValue = NULL;
   ULONG value;
   ULONG tmpVal;
   ULONG transferLength;

   logSenseHeader = (PLOG_SENSE_PAGE_HEADER)(Srb->DataBuffer);
   
   ASSERT(((logSenseHeader->PageCode) == LOGSENSEPAGE2) ||
          ((logSenseHeader->PageCode) == LOGSENSEPAGE3));
   bytesLeft = logSenseHeader->Length[0];
   bytesLeft <<= 8;
   bytesLeft += logSenseHeader->Length[1];
   transferLength = Srb->DataTransferLength;

   if (bytesLeft > (LONG)(transferLength -
                          sizeof(LOG_SENSE_PAGE_HEADER))) {
       bytesLeft = transferLength - sizeof(LOG_SENSE_PAGE_HEADER);
   }
   DebugPrint((3, "ProcessReadWriteErrors : BytesLeft %x, TransferLength %x\n",
               bytesLeft, transferLength));

   (PUCHAR)logSenseParamHeader = (PUCHAR)logSenseHeader + sizeof(LOG_SENSE_PAGE_HEADER);
   while (bytesLeft >= sizeof(LOG_SENSE_PARAMETER_HEADER)) {
      paramCode = logSenseParamHeader->ParameterCode[0];
      paramCode <<= 8;
      paramCode |= logSenseParamHeader->ParameterCode[1];
      paramLen = logSenseParamHeader->ParameterLength;
      paramValue = (PUCHAR)logSenseParamHeader + sizeof(LOG_SENSE_PARAMETER_HEADER);

      //
      // Make sure we have at least
      // (sizeof(LOG_SENSE_PARAMETER_HEADER) + paramLen) bytes left.
      // Otherwise, we've reached the end of the buffer.
      //
      if (bytesLeft < (LONG)(sizeof(LOG_SENSE_PARAMETER_HEADER) + paramLen)) {
          DebugPrint((1,
                      "dlttape : Reached end of buffer. BytesLeft %x, Expected %x\n",
                      bytesLeft,
                      (sizeof(LOG_SENSE_PARAMETER_HEADER) + paramLen)));
          break;
      }

      DebugPrint((3,
                  "ProcessReadWriteErrors : ParamCode %x, ParamLen %x\n",
                  paramCode, paramLen));
      switch (paramCode) {
         case TotalCorrectedErrors: {   
            value = (UCHAR)*(paramValue);
            value <<= 24;
            tmpVal = (UCHAR)*(paramValue+1);
            tmpVal <<= 16;
            value += tmpVal;
            tmpVal = (UCHAR)*(paramValue+2);
            tmpVal <<= 8;
            value += tmpVal + (UCHAR)*(paramValue+3);
            DebugPrint((3, "TotalCorrectedErrors %x\n", value));
            if (Read) {
               IoErrorData->ReadTotalCorrectedErrors = value;
            } else {
               IoErrorData->WriteTotalCorrectedErrors = value;
            }
            break;
         }

         case TotalUncorrectedErrors: {
            value = (UCHAR)*(paramValue);
            value <<= 24;
            tmpVal = (UCHAR)*(paramValue+1);
            tmpVal <<= 16;
            value += tmpVal;
            tmpVal = (UCHAR)*(paramValue+2);
            tmpVal <<= 8;
            value += tmpVal + (UCHAR)*(paramValue+3);
            DebugPrint((3, "TotalUncorrectedErrors %x\n", value));
            if (Read) {
               IoErrorData->ReadTotalUncorrectedErrors = value;
            } else {
               IoErrorData->WriteTotalUncorrectedErrors = value;
            }

            break;
         }

         case TotalTimesAlgorithmProcessed: {

            value = (UCHAR)*(paramValue);
            value <<= 24;
            tmpVal = (UCHAR)*(paramValue+1);
            tmpVal <<= 16;
            value += tmpVal;
            tmpVal = (UCHAR)*(paramValue+2);
            tmpVal <<= 8;
            value += tmpVal + (UCHAR)*(paramValue+3);
            DebugPrint((3, "TotalTimesAlgorithmProcessed %x\n", value));
            if (Read) {
               IoErrorData->ReadCorrectionAlgorithmProcessed = value;
            } else {
               IoErrorData->WriteCorrectionAlgorithmProcessed = value;
            }
            break;
         }

         default: {
            break;
         }
      } // switch (paramCode) 

      (PUCHAR)logSenseParamHeader = (PUCHAR)logSenseParamHeader + 
                                    sizeof(LOG_SENSE_PARAMETER_HEADER) +
                                    paramLen;

      bytesLeft -= paramLen + sizeof(LOG_SENSE_PARAMETER_HEADER);
   } // while (bytesLeft > 0)

   if (Read) {
      IoErrorData->ReadTotalErrors = IoErrorData->ReadTotalUncorrectedErrors +
                                     IoErrorData->ReadTotalCorrectedErrors;
      DebugPrint((3, "ProcessReadWriteErrors: ReadErrors %x\n",
                  IoErrorData->ReadTotalErrors));
   } else {
      IoErrorData->WriteTotalErrors = IoErrorData->WriteTotalUncorrectedErrors +
                                      IoErrorData->WriteTotalCorrectedErrors;
      DebugPrint((3, "ProcessReadWriteErrors: WriteErrors %x\n",
                  IoErrorData->WriteTotalErrors));
   }
}

TAPE_DRIVE_PROBLEM_TYPE
VerifyReadWriteErrors(
   IN PWMI_TAPE_PROBLEM_IO_ERROR IoErrorData
   )
/*+

Routine Description :

   This routine looks at the read\write error counters.
   If the values are above a certain threshold, it returns
   appropriate error value.
   
Argument :

  IoErrorData  WMI_TAPE_PROBLEM_IO_ERROR struct
  
Return Value :
   
      TapeDriveReadWriteError If there are too many uncorrected 
                              read\write errors
                         
      TapeDriveReadWriteWarning If there are too many corrected
                                read\write errors 
                                
      TapeDriveProblemNone    If the read\write errors are below
                              threshold   
  
-*/
{

   if (((IoErrorData->ReadTotalUncorrectedErrors) >=
         TAPE_READ_ERROR_LIMIT)   ||       
       ((IoErrorData->WriteTotalUncorrectedErrors) >=
         TAPE_WRITE_ERROR_LIMIT)) {
      return TapeDriveReadWriteError;
   }

   if (((IoErrorData->ReadTotalCorrectedErrors) >=
         TAPE_READ_WARNING_LIMIT) ||
       ((IoErrorData->WriteTotalCorrectedErrors) >=
         TAPE_WRITE_WARNING_LIMIT)) {

      return TapeDriveReadWriteWarning;
   }

   return TapeDriveProblemNone;
}

TAPE_STATUS
QueryDeviceErrorData(
  IN OUT  PVOID               MinitapeExtension,
  IN OUT  PVOID               CommandExtension,
  IN OUT  PVOID               CommandParameters,
  IN OUT  PSCSI_REQUEST_BLOCK Srb,
  IN      ULONG               CallNumber,
  IN      TAPE_STATUS         LastError,
  IN OUT  PULONG              RetryFlags
  )

/*+
Routine Description:

   This routine returns device Error data such as "drive calibration"
   error, etc.
   
Arguments:

   MinitapeExtension   Pointer to the minidriver's device extension
   CommandExtension    Pointer to the minidriver's command extension
   CommandParameters   Pointer to TAPE_WMI_OPERATIONS struct
   Srb                 SCSI Request block
   CallNumber          Call sequence number
   LastError           Last command error
   RetryFlags          Bit mask for retrying commands
   
Return value:

   TAPE_STATUS
-*/
{
   PTAPE_WMI_OPERATIONS wmiOperations;
   PMINITAPE_EXTENSION miniExtension;
   PWMI_TAPE_PROBLEM_DEVICE_ERROR DeviceErrorData;
   PWMI_TAPE_PROBLEM_WARNING wmiData;
   TAPE_STATUS  status = TAPE_STATUS_SUCCESS;
   PCDB cdb = (PCDB)Srb->Cdb;
   
   miniExtension = (PMINITAPE_EXTENSION)MinitapeExtension;
   wmiOperations = (PTAPE_WMI_OPERATIONS)CommandParameters;
   wmiData = (PWMI_TAPE_PROBLEM_WARNING)wmiOperations->DataBuffer;
   DeviceErrorData = (PWMI_TAPE_PROBLEM_DEVICE_ERROR)wmiData->TapeData;

   if (CallNumber == 0) {
      ULONG allocLength;

      if ((miniExtension->DriveID != COMPAQ_8000) &&
          (miniExtension->DriveID != QUANTUM_8000)) {
         DebugPrint((1, 
                     "QueryDeviceErrorData : TapeAlertPage not supported\n"));
         return TAPE_STATUS_NOT_IMPLEMENTED;
      }

      wmiData->DriveProblemType = TapeDriveProblemNone;
      TapeClassZeroMemory(DeviceErrorData, 
                          sizeof(WMI_TAPE_PROBLEM_DEVICE_ERROR));

      return PrepareSrbForTapeAlertInfo(Srb);
   }

   if (CallNumber == 1) {
       PTAPE_ALERT_INFO  tapeAlertInfo;
       PLOG_SENSE_PAGE_HEADER logSenseHeader;
       LONG bytesLeft;
       UCHAR paramCode;
       UCHAR flagValue;

       bytesLeft = GetNumberOfBytesReturned(Srb);

       logSenseHeader = (PLOG_SENSE_PAGE_HEADER)(Srb->DataBuffer);
       tapeAlertInfo= (PTAPE_ALERT_INFO)((PUCHAR)logSenseHeader +
                                         sizeof(LOG_SENSE_PAGE_HEADER));
       while (bytesLeft >= sizeof(TAPE_ALERT_INFO)) {
            //
            // ParamCode is 2 bytes long. Upper Byte is 0.
            // Lower Byte goes from 0x01 to 0x40. So, we just
            // pick up the lower byte
            //
            paramCode = tapeAlertInfo->ParamCodeLB;
            flagValue = tapeAlertInfo->Flag;
            ASSERT((tapeAlertInfo->ParamLen) == 1);
            DebugPrint((3, "QDED: ParamCode %x, FlagValue %x, ParamLen %x\n",
                        paramCode, flagValue, tapeAlertInfo->ParamLen));
            switch (paramCode) {
               case READ_WARNING: {
                  if (flagValue) {
                     DebugPrint((3, "QueryDeviceErrorData : READ_WARNING\n"));
                     DeviceErrorData->ReadWarning = TRUE;
                     wmiData->DriveProblemType = TapeDriveReadWarning;
                  }
   
                  break;
               }
   
               case WRITE_WARNING: {
                  if (flagValue) {
                     DebugPrint((3, "QueryDeviceErrorData : WRITE_WARNING\n"));
                     DeviceErrorData->WriteWarning = TRUE;
                     wmiData->DriveProblemType = TapeDriveWriteWarning;
                  }
   
                  break;
               }
   
               case HARD_ERROR: {
                  if (flagValue) {
                     DebugPrint((3, "QueryDeviceErrorData : HARD_ERROR\n"));
                     DeviceErrorData->HardError = TRUE;
                     wmiData->DriveProblemType = TapeDriveHardwareError;
                  }
                  break;
               }
   
               case READ_FAILURE: {
                  if (flagValue) {
                     DebugPrint((3, "QueryDeviceErrorData : READ_FAILURE\n"));
                     DeviceErrorData->ReadFailure = TRUE;
                     wmiData->DriveProblemType = TapeDriveReadError;
                  }
                  break;
               }
   
               case WRITE_FAILURE: {
                  if (flagValue) {
                     DebugPrint((3, "QueryDeviceErrorData : WRITE_FAILURE\n"));
                     DeviceErrorData->WriteFailure = TRUE;
                     wmiData->DriveProblemType = TapeDriveWriteError;
                  }
                  break;
               }
   
               case CLEAN_NOW: {
                  if (flagValue) {
                     DebugPrint((3, "QueryDeviceErrorData : CLEAN_NOW\n"));
                     DeviceErrorData->DriveRequiresCleaning = TRUE;
                     wmiData->DriveProblemType = TapeDriveCleanDriveNow;
                  }
                  break;
               }
   
               case HARDWARE_B: {
                  if (flagValue) {
                     DebugPrint((3, "QueryDeviceErrorData : HARDWARE_B\n"));
                     DeviceErrorData->DriveHardwareError = TRUE;
                     wmiData->DriveProblemType = TapeDriveHardwareError;
                  }
                  break;
               }
   
               case INTERFACE_ERROR: {
                  if (flagValue) {
                     DebugPrint((3, "QueryDeviceErrorData : INTERFACE_ERROR\n"));
                     DeviceErrorData->ScsiInterfaceError = TRUE;
                     wmiData->DriveProblemType = TapeDriveScsiConnectionError;
                  }
                  break;
               }
   
               default:
                  break;
            } // switch (paramCode) {

            tapeAlertInfo++;
            bytesLeft -= sizeof(TAPE_ALERT_INFO);   
         }
   }

   return TAPE_STATUS_SUCCESS;
}

TAPE_STATUS
PrepareSrbForTapeAlertInfo(
    PSCSI_REQUEST_BLOCK Srb
    )
/*+++

Routine Description:

        This routine sets the SCSI_REQUEST_BLOCK to retrieve
        Tape Alert information from the drive

Arguements:

        Srb - Pointer to the SCSI_REQUEST_BLOCK

Return Value:

        TAPE_STATUS_SEND_SRB_AND_CALLBACK if Srb fields were successfully set.

        TAPE_STATUS_SUCCESS if there was not enough memory for Srb Databuffer.
                            case is not treated as an error.
--*/
{
    PCDB cdb = (PCDB)Srb->Cdb;
    ULONG allocLength;

    allocLength = sizeof(LOG_SENSE_PAGE_HEADER) +
                   (sizeof(TAPE_ALERT_INFO) * 0x40);
    if (!TapeClassAllocateSrbBuffer(Srb, allocLength)) {
       DebugPrint((1, 
                   "PrepareSrbForTapeAlertInfo : No mem for logsense info\n"));
       return TAPE_STATUS_INSUFFICIENT_RESOURCES;
    }

    TapeClassZeroMemory(cdb, MAXIMUM_CDB_SIZE);

    //
    // Prepare SCSI command (CDB)
    //
    Srb->CdbLength = CDB10GENERIC_LENGTH;

    cdb->LOGSENSE.OperationCode = SCSIOP_LOG_SENSE;
    cdb->LOGSENSE.PageCode = TapeAlertLogPage;
    cdb->LOGSENSE.PCBit = 1;
    cdb->LOGSENSE.AllocationLength[0] = (UCHAR)((allocLength & 0xFF00) >> 8);
    cdb->LOGSENSE.AllocationLength[1] = (UCHAR)(allocLength & 0xFF);

    Srb->DataTransferLength = allocLength;
    return  TAPE_STATUS_SEND_SRB_AND_CALLBACK;
}

LONG 
GetNumberOfBytesReturned(
    PSCSI_REQUEST_BLOCK Srb
    )
{
    PLOG_SENSE_PAGE_HEADER logSenseHeader;
    ULONG transferLength;
    LONG  bytesLeft;

    logSenseHeader = (PLOG_SENSE_PAGE_HEADER)(Srb->DataBuffer);

    ASSERT((logSenseHeader->PageCode) == TapeAlertLogPage);
    bytesLeft = logSenseHeader->Length[0];
    bytesLeft <<= 8;
    bytesLeft += logSenseHeader->Length[1];

    transferLength = Srb->DataTransferLength;

    if (bytesLeft > (LONG)(transferLength -
                           sizeof(LOG_SENSE_PAGE_HEADER))) {
        bytesLeft = transferLength - sizeof(LOG_SENSE_PAGE_HEADER);
    }

    DebugPrint((3, "GetNumberOfBytesReturned : BytesLeft %x, TransferLength %x\n",
                bytesLeft, transferLength));

    return bytesLeft;
}

