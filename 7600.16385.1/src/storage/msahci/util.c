/*++

Copyright (C) Microsoft Corporation, 2005

Module Name:

    util.h

Abstract:
    This file contains all the misc auxiliary functionality that the other four files depend on.

Notes:

Revision History:

        Nathan Obr (natobr),  February 2005 - September 2006 rev 1 (NCQ, LPM, Hotplug, persistant state) 
                              December 2006 - August 2007    rev 2 (async)
--*/
#include "geninc.h"

#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning(disable:4054) 
#pragma warning(disable:4055)


VOID
  AhciReportBusChange(
    PAHCI_CHANNEL_EXTENSION ChannelExtension
    )
/*++
    Asks ATAport to request a QDR on behalf of the miniport
It assumes:
    nothing
Called by:
    Indirectly by AhciHwInterrupt

It performs:
    1 Requests QDR
    2 Kicks off the Start Channel state machine
Affected Variables/Registers:
    none
--*/
{
    RecordExecutionHistory(ChannelExtension, 0x00000030);//AhciReportBusChange
  //1 Requests QDR
    AtaPortBusChangeDetected(ChannelExtension);
  //2 Kicks off the Start Channel state machine
    P_Running_StartAttempt(ChannelExtension);
}
VOID
  Set_PxIE(
    PAHCI_CHANNEL_EXTENSION ChannelExtension,
    PAHCI_INTERRUPT_ENABLE IE
    )
/*++
    This is just a space saver function that makes the interrupt configuration more readable

Called by:
    AhciHwControlIdeStart
    AhciHwControlIdePowerUp

--*/
{
    AHCI_INTERRUPT_ENABLE ie;
    AHCI_COMMAND cmd;
    
    ie.AsUlong = AtaPortReadRegisterUlong(&IE->AsUlong);
    ie.DHRE = 1; //Device to Host Register FIS Interrupt (DHRS):  A D2H Register FIS has been received with the ‘I’ bit set, and has been copied into system memory.
    ie.PSE  = 1; //PIO Setup FIS Interrupt (PSS):  A PIO Setup FIS has been received with the ‘I’ bit set, it has been copied into system memory, and the data related to that FIS has been transferred.  This bit shall be set even if the data transfer resulted in an error.
    ie.DSE  = 1; //DMA Setup FIS Interrupt (DSS):  A DMA Setup FIS has been received with the ‘I’ bit set and has been copied into system memory.
    ie.SDBE = 1; //Set Device Bits Interrupt (SDBS):  A Set Device Bits FIS has been received with the ‘I’ bit set and has been copied into system memory.
    
    ie.UFE  = 0; //Unknown FIS Interrupt (UFS): When set to ‘1’, indicates that an unknown FIS was received and has been copied into system memory.  This bit is cleared to ‘0’ by software clearing the PxSERR.DIAG.F bit to ‘0’.  Note that this bit does not directly reflect the PxSERR.DIAG.F bit.  PxSERR.DIAG.F is set immediately when an unknown FIS is detected, whereas this bit is set when that FIS is posted to memory.  Software should wait to act on an unknown FIS until this bit is set to ‘1’ or the two bits may become out of sync.
    ie.DPE  = 0; //Descriptor Processed (DPS):  A PRD with the ‘I’ bit set has transferred all of its data.  Refer to section 5.4.2.
    ie.PCE  = 1; //Port Connect Change Status (PCS): 1=Change in Current Connect Status. 0=No change in Current Connect Status.  This bit reflects the state of PxSERR.DIAG.X.  This bit is only cleared when PxSERR.DIAG.X is cleared.
    if(ChannelExtension->CAP.SMPS) {
        ie.DMPE  = 1; //Device Mechanical Presence Status (DMPS): When set, indicates that a mechanical presence switch attached to this port has been opened or closed, which may lead to a change in the connection state of the device.  This bit is only valid if both CAP.SMPS and P0CMD.MPSP are set to ‘1’.
    } else {
        ie.DMPE  = 0;
    }

    //Reserved :14; 
    ie.PRCE = 1; //PhyRdy Change Status (PRCS): When set to ‘1’ indicates the internal PhyRdy signal changed state.  This bit reflects the state of P0SERR.DIAG.N.  To clear this bit, software must clear P0SERR.DIAG.N to ‘0’.
    ie.IPME = 0; //Incorrect Port Multiplier Status (IPMS):  Indicates that the HBA received a FIS from a device whose Port Multiplier field did not match what was expected.  The IPMS bit may be set during enumeration of devices on a Port Multiplier due to the normal Port Multiplier enumeration process.  It is recommended that IPMS only be used after enumeration is complete on the Port Multiplier.
    
    ie.OFE  = 1; //Overflow Status (OFS):  Indicates that the HBA received more bytes from a device than was specified in the PRD table for the command.
    //Reserved2 :1; 
    ie.INFE = 1; //Interface Non-fatal Error Status (INFS):  Indicates that the HBA encountered an error on the Serial ATA interface but was able to continue operation.  Refer to section 6.1.2.
    ie.IFE  = 1; //Interface Fatal Error Status (IFS):  Indicates that the HBA encountered an error on the Serial ATA interface which caused the transfer to stop.  Refer to section 6.1.2.
     
    ie.HBDE = 1; //Host Bus Data Error Status (HBDS):  Indicates that the HBA encountered a data error (uncorrectable ECC / parity) when reading from or writing to system memory.
    ie.HBFE = 1; //Host Bus Fatal Error Status (HBFS):  Indicates that the HBA encountered a host bus error that it cannot recover from, such as a bad software pointer.  In PCI, such an indication would be a target or master abort.
    ie.TFEE = 1; //Task File Error Status (TFES):  This bit is set whenever the status register is updated by the device and the error bit (bit 0) is set.
    cmd.AsUlong = AtaPortReadRegisterUlong(&ChannelExtension->Px->CMD.AsUlong);
    if(cmd.CPD) {    //check for PxCMD.CPD set to ‘1’ before setting CPDE
        ie.CPDE = 1; //Cold Port Detect Status (CPDS): When set, a device status has changed as detected by the cold presence detect logic.  This bit can either be set due to a non-connected port receiving a device, or a connected port having its device removed.  This bit is only valid if the port supports cold presence detect as indicated by PxCMD.CPD set to ‘1’.
    } else {
        ie.CPDE = 0;
    }
    AtaPortWriteRegisterUlong(&IE->AsUlong, ie.AsUlong);
}

BOOLEAN
  RequiresControllerResources (
    PAHCI_CHANNEL_EXTENSION ChannelExtension,
    PIDE_REQUEST_BLOCK Irb
    )
/*++
    The purpose of this function is to identify if a control command is sent that requires no action to be performed on that SATA bus.
    If so, the command is handled and completed back to ATAport and the calling function is notified
It assumes:
    nothing
Called by:
    AhciHwStartIo

It performs:
    (overview)
    1 IRB_FUNCTION_LUN_RESET => AhciHwReset
    2 IRB_FUNCTION_ADAPTER_FLUSH => Success
    3 IRB_FUNCTION_SHUTDOWN => Success
    4 IDE_FEATURE_DISABLE_WRITE_CACHE => Modify Persistent Settings
    5 IDE_FEATURE_ENABLE_WRITE_CACHE => Modify Persistent Settings


    (details)
Affected Variables/Registers:
    IRB
Return Value:

--*/
{
    UCHAR i;
    PUCHAR returnBuffer;
    PUCHAR mediaStatusBuffer;

    switch (Irb->Function) {
      //1 IRB_FUNCTION_LUN_RESET => AhciHwReset
        case (IRB_FUNCTION_LUN_RESET):
            if (AhciHwReset (ChannelExtension)) {
                Irb->IrbStatus = IRB_STATUS_SUCCESS;
                AhciCompleteRequest(ChannelExtension, Irb);
#ifdef ENABLE_HISTORY_LOG
                RecordExecutionHistory(ChannelExtension, 0x10000044);// Exit RequiresControllerResources, LUN RESET Success
#endif
                return FALSE;
            } else {
                Irb->IrbStatus = IRB_STATUS_DEVICE_ERROR;
                AhciCompleteRequest(ChannelExtension, Irb);
                RecordExecutionHistory(ChannelExtension, 0x20000044);// Exit RequiresControllerResources, LUN RESET Failure
                return FALSE;
            }
            break;
      //2 IRB_FUNCTION_ADAPTER_FLUSH => Success
        case (IRB_FUNCTION_ADAPTER_FLUSH):
            Irb->IrbStatus = IRB_STATUS_SUCCESS;
            AhciCompleteRequest(ChannelExtension, Irb);
#ifdef ENABLE_HISTORY_LOG
            RecordExecutionHistory(ChannelExtension, 0x30000044);// Exit RequiresControllerResources, Adapter Flush
#endif
            return FALSE;
            break;
      //3 IRB_FUNCTION_SHUTDOWN => Success
        case (IRB_FUNCTION_SHUTDOWN):
            Irb->IrbStatus = IRB_STATUS_SUCCESS;
            AhciCompleteRequest(ChannelExtension, Irb);
#ifdef ENABLE_HISTORY_LOG
            RecordExecutionHistory(ChannelExtension, 0x40000044);// Exit RequiresControllerResources, Shutdown
#endif
            return FALSE;
            break;
        case (IRB_FUNCTION_ATA_COMMAND):
            if (Irb->IdeTaskFile.Current.bCommandReg == IDE_COMMAND_SET_FEATURE) {
              //4 IDE_FEATURE_DISABLE_WRITE_CACHE => Modify Persistent Settings
#ifdef ENABLE_HISTORY_LOG
                RecordExecutionHistory(ChannelExtension, 0x50000044);// Exit RequiresControllerResources, SET FEATURES
#endif
                if (Irb->IdeTaskFile.Current.bFeaturesReg == IDE_FEATURE_DISABLE_WRITE_CACHE) {
                    if (! ReplaceSetFeatures(ChannelExtension, IDE_FEATURE_ENABLE_WRITE_CACHE, IDE_FEATURE_DISABLE_WRITE_CACHE, 0, 0) ) {
                        AddSetFeatures(ChannelExtension, IDE_FEATURE_DISABLE_WRITE_CACHE, 0);
                    }
                }
              //5 IDE_FEATURE_ENABLE_WRITE_CACHE => Modify Persistent Settings
                if (Irb->IdeTaskFile.Current.bFeaturesReg == IDE_FEATURE_ENABLE_WRITE_CACHE) {
                    if (! ReplaceSetFeatures(ChannelExtension, IDE_FEATURE_DISABLE_WRITE_CACHE, IDE_FEATURE_ENABLE_WRITE_CACHE, 0, 0) ) {
                        AddSetFeatures(ChannelExtension, IDE_FEATURE_ENABLE_WRITE_CACHE, 0);
                    }
                }
            }
            return TRUE;
            break;
        case (IRB_FUNCTION_ATAPI_COMMAND):
            if ( ChannelExtension->StateFlags.AN_Enabled == TRUE ) {
                if ( !ChannelExtension->SendGesnToDevice ) {
                    if ( IsGESNCommand(Irb) ){
                        returnBuffer = (PUCHAR) Irb->DataBuffer;
                        mediaStatusBuffer = (PUCHAR) ChannelExtension->GesnCachedBuffer;
                        for (i = 0; i < 8; i++) {
                            if (i < 4 ){
                                *returnBuffer = *mediaStatusBuffer;
                            } else {
                                *returnBuffer = 0;
                            }
                            mediaStatusBuffer++;
                            returnBuffer++;
                        }

                        Irb->IrbStatus =  IRB_STATUS_SUCCESS;
                        AtaPortCompleteRequest(ChannelExtension, Irb);
                        return FALSE;
                    }
                }
            }

            return TRUE;
            break;

        default:
            return TRUE;
            break;
    }
}


BOOLEAN
  IsFPDMACommand (
    PIDE_REQUEST_BLOCK Irb
    )
/*++
    This could be a macro

Return Value:
    TRUE if CommandRegister is 0x60 or 0x61
--*/
{
    if ( (Irb->IdeTaskFile.Current.bCommandReg == 0x60) || 
        (Irb->IdeTaskFile.Current.bCommandReg == 0x61) ) {
        return TRUE;
    }

    return FALSE;
}

BOOLEAN
  IsGESNCommand (
    PIDE_REQUEST_BLOCK Irb
    )
/*
    This function recognizes the polling GESN command pattern sent by cdrom.sys.

Return Value:
    TRUE if command meets the Pattern. Otherwise, FALSE.
*/
{
    if ( (Irb->Function == IRB_FUNCTION_ATAPI_COMMAND) &&
         (Irb->Cdb[0] == 0x4a ) &&
         (Irb->Cdb[1] == 0x01 ) &&
         (Irb->Cdb[2] == 0x00 ) &&
         (Irb->Cdb[3] == 0x00 ) &&
         //(Irb->Cdb[4] == 0x5a ) &&
         (Irb->Cdb[5] == 0x00 ) &&
         (Irb->Cdb[6] == 0x00 ) &&
         (Irb->Cdb[7] == 0x00 ) &&
         (Irb->Cdb[8] == 0x08 ) &&
         (Irb->Cdb[9] == 0x00 ) ){
             return TRUE;
    }

    return FALSE;
}

BOOLEAN
  IsNormalCommand (
    PIDE_REQUEST_BLOCK Irb
    )
{
/*++
    This could be a macro

Return Value:
    TRUE if any Command Register is any non NCQ IO command 
--*/
    if ((Irb->IdeTaskFile.Current.bCommandReg == IDE_COMMAND_READ) || 
        (Irb->IdeTaskFile.Current.bCommandReg == IDE_COMMAND_WRITE) ||
        (Irb->IdeTaskFile.Current.bCommandReg == IDE_COMMAND_READ_EXT) || 
        (Irb->IdeTaskFile.Current.bCommandReg == IDE_COMMAND_WRITE_EXT) ||
        (Irb->IdeTaskFile.Current.bCommandReg == IDE_COMMAND_READ_DMA) || 
        (Irb->IdeTaskFile.Current.bCommandReg == IDE_COMMAND_WRITE_DMA) || 
        (Irb->IdeTaskFile.Current.bCommandReg == IDE_COMMAND_READ_DMA_EXT) || 
        (Irb->IdeTaskFile.Current.bCommandReg == IDE_COMMAND_WRITE_DMA_EXT) ||
        (Irb->IdeTaskFile.Current.bCommandReg == IDE_COMMAND_WRITE_DMA_FUA_EXT) || 
        (Irb->IdeTaskFile.Current.bCommandReg == IDE_COMMAND_READ_DMA_QUEUED_EXT) || 
        (Irb->IdeTaskFile.Current.bCommandReg == IDE_COMMAND_WRITE_DMA_QUEUED_EXT) ||
        (Irb->IdeTaskFile.Current.bCommandReg == IDE_COMMAND_WRITE_DMA_QUEUED_FUA_EXT) ||
        (Irb->IdeTaskFile.Current.bCommandReg == IDE_COMMAND_READ_MULTIPLE) || 
        (Irb->IdeTaskFile.Current.bCommandReg == IDE_COMMAND_WRITE_MULTIPLE) ||
        (Irb->IdeTaskFile.Current.bCommandReg == IDE_COMMAND_READ_MULTIPLE_EXT) || 
        (Irb->IdeTaskFile.Current.bCommandReg == IDE_COMMAND_WRITE_MULTIPLE_EXT) ||  
        (Irb->IdeTaskFile.Current.bCommandReg == IDE_COMMAND_WRITE_MULTIPLE_FUA_EXT) ||
        (Irb->IdeTaskFile.Current.bCommandReg == IDE_COMMAND_WRITE_DMA_QUEUED) ||
        (Irb->Function == IRB_FUNCTION_ATAPI_COMMAND) ) {
        return TRUE;
    }

    return FALSE;
}

VOID
  ReleaseSlottedCommand(
    PAHCI_CHANNEL_EXTENSION ChannelExtension,
    UCHAR SlotNumber
    )
/*++
    Performs Completion Data marshalling back into the IRB
It assumes:
    IRB STATUS has already been filled out
    ATA status from the AHCI registers is valid for this command
    SlotNumber points to a fully filled out Slot entry
Called by:
    AhciHwStartIo
    AhciNonQueuedErrorRecovery
    AhciCompleteIssuedIRBs

It performs:
    (overview)
    1 Intialize
    2 Marshal completion data
    3 Complete the Command
    (details)
    1.1 Keep track of the completion for our debugging records
    2.1 Handle Request Sense marshalling
    2.2.1 Handle everything else's marshalling
    2.2.2 If this was marked as needing return data, fill in the return Task File
    3.1 Make the slot available again
    3.2 Perform the Slot's last request, if there was one
    3.3  Complete the command
Affected Variables/Registers:
    IRB
--*/
{
    PIDE_REQUEST_BLOCK irbToRelease;
    PIDE_TASK_FILE returnTaskFile;
    PSLOT_CONTENT slotContent;
    PSLOT_COMPLETION_ROUTINE completionRoutine;

    slotContent = &ChannelExtension->Slot[SlotNumber];

#ifdef ENABLE_HISTORY_LOG
    RecordExecutionHistory(ChannelExtension, 0x10000053);//ReleaseSlottedCommand

  // 2/2 Log command completion part for our debugging records
    {
        PCOMMAND_HISTORY cmdHistory;
        cmdHistory = &ChannelExtension->CommandHistory[slotContent->CommandHistoryIndex];
        AtaPortCopyMemory(&cmdHistory->CompletionFIS, &ChannelExtension->ReceivedFIS->D2hRegisterFis, sizeof(AHCI_D2H_REGISTER_FIS));
        AtaPortCopyMemory(&cmdHistory->CompletionPx, ChannelExtension->Px, 0x40);
        cmdHistory->IrbStatus = slotContent->Irb->IrbStatus;
    }
#endif

  //Then complete the command
  //2.1 Handle Request Sense marshalling
    if ( IsRequestSenseIrb(slotContent->Irb) ) {
        //the IRB to complete normally is the original IRB, not the REQUEST SENSE IRB which was passed in
        irbToRelease = slotContent->Irb->NextIrb;
        //that IRB must have IRB_STATUS_AUTOSENSE_VALID set appropriately
        if (slotContent->Irb->IrbStatus == IRB_STATUS_SUCCESS) { 
            irbToRelease->IrbStatus |= IRB_STATUS_AUTOSENSE_VALID;

        } else { 
            irbToRelease->IrbStatus &= ~IRB_STATUS_AUTOSENSE_VALID;
        }
        //and release the Request Sense IRB so that ATAport merges the Request Sense IRB data into the original IRB
        AtaPortReleaseRequestSenseIrb(ChannelExtension, slotContent->Irb);

  //2.2.1 Handle everything else's marshalling
    } else {
        //the IRB to complete normally is the IRB passed in
        irbToRelease = slotContent->Irb;
        //Record error and status
        slotContent->Irb->AtaStatus = AtaPortReadRegisterUchar(&ChannelExtension->Px->TFD.STS.AsUchar);    
        slotContent->Irb->AtaError  = AtaPortReadRegisterUchar(&ChannelExtension->Px->TFD.ERR);
     
      //2.2.2 If this was marked as needing return data, fill in the return Task File.
        if( IsReturnResults(slotContent->Irb)) {
            returnTaskFile = (PIDE_TASK_FILE) slotContent->Irb->SenseInfoBuffer;
            returnTaskFile->Current.bCommandReg = ChannelExtension->ReceivedFIS->D2hRegisterFis.Status;
            returnTaskFile->Current.bFeaturesReg = ChannelExtension->ReceivedFIS->D2hRegisterFis.Error;
            returnTaskFile->Current.bCylHighReg = ChannelExtension->ReceivedFIS->D2hRegisterFis.CylHigh;
            returnTaskFile->Current.bCylLowReg = ChannelExtension->ReceivedFIS->D2hRegisterFis.CylLow;
            returnTaskFile->Current.bDriveHeadReg = ChannelExtension->ReceivedFIS->D2hRegisterFis.Dev_Head;
            returnTaskFile->Current.bSectorCountReg = ChannelExtension->ReceivedFIS->D2hRegisterFis.SectorCount;
            returnTaskFile->Current.bSectorNumberReg = ChannelExtension->ReceivedFIS->D2hRegisterFis.SectorNumber;
            //if 48bit, get all of it
            if( Is48BitIrb(slotContent->Irb)) {
                slotContent->Irb->SenseInfoBufferType = SENSE_INFO_BUFFER_RETURN_TYPE_48BIT_TASKFILE; 

                returnTaskFile->Previous.bCommandReg = ChannelExtension->ReceivedFIS->D2hRegisterFis.Status;
                returnTaskFile->Previous.bFeaturesReg = ChannelExtension->ReceivedFIS->D2hRegisterFis.Error;
                returnTaskFile->Previous.bCylHighReg = ChannelExtension->ReceivedFIS->D2hRegisterFis.CylHigh_Exp;
                returnTaskFile->Previous.bCylLowReg = ChannelExtension->ReceivedFIS->D2hRegisterFis.CylLow_Exp;
                returnTaskFile->Previous.bDriveHeadReg = ChannelExtension->ReceivedFIS->D2hRegisterFis.Dev_Head;
                returnTaskFile->Previous.bSectorCountReg = ChannelExtension->ReceivedFIS->D2hRegisterFis.SectorCount_Exp;
                returnTaskFile->Previous.bSectorNumberReg = ChannelExtension->ReceivedFIS->D2hRegisterFis.SectorNum_Exp;
            } else {
            // and mark the return data type accordingly
                slotContent->Irb->SenseInfoBufferType = SENSE_INFO_BUFFER_RETURN_TYPE_28BIT_TASKFILE; 
            }
            slotContent->Irb->IrbStatus |= IRB_STATUS_RETURN_TASKFILE_VALID;
        }
    }

  //3.1 Make the slot available again
    completionRoutine = slotContent->CompletionRoutine;
    
    slotContent->CmdHeader = NULL;
    slotContent->CommandHistoryIndex = 0;
    slotContent->CompletionRoutine = NULL;
    slotContent->Irb = NULL;
    slotContent->StateFlags.FUA = FALSE;
    //Clear the CommandsToComplete bit
    ChannelExtension->SlotManager.CommandsToComplete &= ~(1 << SlotNumber);
    ChannelExtension->SlotManager.HighPriorityAttribute &= ~(1 << SlotNumber);

  //3.2 Perform the Slot's last request, if there was one
    if (completionRoutine) {
        completionRoutine(ChannelExtension);
    }

  //3.3 Complete the command
    AhciCompleteRequest(ChannelExtension, irbToRelease);
}

VOID
  AhciCompleteRequest(
    PAHCI_CHANNEL_EXTENSION ChannelExtension,
    PIDE_REQUEST_BLOCK Irb
    )
/*++
    Wrapper for AtaPortComleteRequest to protect against completing the localIRB back to ATAport who doesn't know anything about localIRB
It assumes:
    IRB is completely ready to be completed back to the IRB generator
It performs:
    (overview)
    1 Clean Up
    2 Finalize Completion
    (details)
    1.1 IO is completing, that IO may have paused the queue so unpause the queue
    1.2 Perform the IRB's last request, if there was one
    2.1 Complete the command back to the owner
Called by:
    AhciHwStartIo
    RequiresControllerResources
    ReleaseSlottedCommand
--*/
{
    PSLOT_COMPLETION_ROUTINE completionRoutine;
    UCHAR i;
    PUCHAR returnBuffer;
    PUCHAR mediaStatusBuffer;

  //1.1 IO is completing, that IO may have paused the queue so unpause the queue
    ChannelExtension->StateFlags.QueuePaused = FALSE;

  //1.2 Perform the IRB's last request, if there was one
    if (Irb->Reserved) {
        completionRoutine = (PSLOT_COMPLETION_ROUTINE)Irb->Reserved;
        completionRoutine(ChannelExtension);
        Irb->Reserved = NULL;
    }

    if (Irb->Function == IRB_FUNCTION_ATAPI_COMMAND){
        if ( ChannelExtension->StateFlags.AN_Enabled == TRUE ) {
            if ( IsGESNCommand(Irb) ){
                returnBuffer = (PUCHAR) Irb->DataBuffer;
                mediaStatusBuffer = (PUCHAR) ChannelExtension->GesnCachedBuffer;
                for (i = 0; i < 8; i++) {
                    *returnBuffer = *mediaStatusBuffer;
                    mediaStatusBuffer++;
                    returnBuffer++;
                }

                if ((ChannelExtension->GesnCachedBuffer[4] & 0x0f) == 0) {
                    // No Event anymore, return cached data for following GESN commands util an AN occurs.
                    ChannelExtension->SendGesnToDevice = FALSE;
                }
            }
        }
    }

  //2.1 Complete the command back to the owner
    if (Irb == &ChannelExtension->Local.Irb ) {
        RecordExecutionHistory(ChannelExtension, 0x20000045); //Exit AhciCompleteRequest, local IRB
        return;
    } else {
        AtaPortCompleteRequest(ChannelExtension, Irb);
    }
#ifdef ENABLE_HISTORY_LOG
    RecordExecutionHistory(ChannelExtension, 0x10000045); //Exit AhciCompleteRequest, ATAport IRB
#endif
}

VOID
  GetTag(
    PAHCI_CHANNEL_EXTENSION ChannelExtension,
    PIDE_REQUEST_BLOCK Irb
    )
/*++
    Returns the next available slot that can be used for an ATA command in the QueueTag field of the IRB
It assumes:
    
Called by:

It performs:
    (overview)
    1 Initialize
    2 Chose the slot
    3 Update CCS
    (details)
    1.1 Initialize variables
    2.1 Special case the slot for the local IRB
    2.2 Chose the slot circularly starting with CCS
    3.1 Update CCS

Affected Variables/Registers:

Return Value:
    Next available slot number.  If IRB is localIRB then slot is 0.
    If no slot is avalable tag returned is 0xFF.
--*/
{
    ULONG allocated;
    UCHAR limit;
    UCHAR i;
    AHCI_COMMAND cmd;

  //1.1 Initialize variables
    Irb->QueueTag = 0xFF;
    limit = ChannelExtension->CCS;
    cmd.AsUlong = AtaPortReadRegisterUlong(&ChannelExtension->Px->CMD.AsUlong);
    allocated = ChannelExtension->SlotManager.CommandsIssued | 
                ChannelExtension->SlotManager.NCQueueSlice | 
                ChannelExtension->SlotManager.NormalQueueSlice | 
                ChannelExtension->SlotManager.SingleIoSlice |
                ChannelExtension->SlotManager.CommandsToComplete;
    
  //2.1 Use slot 0 for internal commands, don't increment CCS
    if (Irb == &ChannelExtension->Local.Irb ) {
        if ((allocated & (1 << 0)) > 0) {
            Irb->QueueTag = 0xFF;
        } else {
            Irb->QueueTag = 0;
        }
        return;
    }

  //2.2 Chose the slot circularly starting with CCS
    for (i = limit; i <= ChannelExtension->CAP.NCS; i++) {
        if ( (allocated & (1 << i)) == 0){
            Irb->QueueTag = i;
            goto getout;
        }
    }
    
    for (i = 1; i < limit; i++) {
        if ( (allocated & (1 << i)) == 0){
            Irb->QueueTag = i;
            goto getout;
        }
    }

getout:
  //3.1 Update CCS
    if (IsRequestSenseIrb(Irb)) {
      //If this IRB is for Request Sense, make sure it is given the next chance to run during ActivateQueue by not incrementing CCS.
        return;      
    }

    ChannelExtension->CCS++;
    if (ChannelExtension->CCS == (ChannelExtension->CAP.NCS + 1)) {
        ChannelExtension->CCS = 1;
    }
    if (cmd.ST == 0) {
        ChannelExtension->CCS = 1;
    }

    return;
}

UCHAR
  GetSingleIo(
    PAHCI_CHANNEL_EXTENSION ChannelExtension
    )
/*++
    Selects an IO circularly from the programmed Single IO slice starting with the slot after the most recently programmed slot
    This ensures the issuing of the SingleIO is FIFO

It assumes:
    Single IO are never high priority

Called by:
    Activate Queue

It performs:
    (overview)
    1 Initialize
    2 Chose the slot
    (details)
    1.1 Initialize variables
    2.1 Chose the slot circularly starting with CCS

Affected Variables/Registers:
    none

Return Value:
    Slot number of the oldest programmed Single IO Slice slot
    If no slots are programmed the tag returned is 0xFF
--*/
{
    UCHAR limit;
    UCHAR i;
  //1.1 Initialize variables
    limit = ChannelExtension->CCS;

  //2.1 Chose the slot circularly starting with CCS
    for (i = limit; i <= ChannelExtension->CAP.NCS; i++) {
        if ( (ChannelExtension->SlotManager.SingleIoSlice & (1 << i)) > 0){
            return i;
        }
    }
    
    for (i = 0; i < limit; i++) {
        if ( (ChannelExtension->SlotManager.SingleIoSlice & (1 << i)) > 0){
            return i;
        }
    }

    return 0xff;
}

VOID
  RecordExecutionHistory(
    PAHCI_CHANNEL_EXTENSION ChannelExtension, 
    ULONG Function
  )
/*++
    Takes a snapshot of the AHCI registers for debugging
It assumes:
    nothing
Called by:
    Everything

It performs:
    (overview)
    1 Select the next available index
    2 Take the snapshot
    (details)
    1.1 Select the next available index
    2.1 Copy over the Channel Extension header
    2.2 Copy over the AHCI registers
Affected Variables/Registers:
    none
Return Value:
    none
--*/
{
    int i;

  //1.1 Select the next available index
    if ( ChannelExtension->ExecutionHistoryNextAvailableIndex >= 99) {
        ChannelExtension->ExecutionHistoryNextAvailableIndex = 0;
    }else{
        ChannelExtension->ExecutionHistoryNextAvailableIndex++;
    }
    
  //2.1 Copy over the Channel Extension header
    ChannelExtension->ExecutionHistory[ChannelExtension->ExecutionHistoryNextAvailableIndex].Function = Function;
    if(ChannelExtension->ControllerIS) {
        AtaPortCopyMemory(&ChannelExtension->ExecutionHistory[ChannelExtension->ExecutionHistoryNextAvailableIndex].IS, &ChannelExtension->StateFlags, sizeof(ULONG));
    } else {
        ChannelExtension->ExecutionHistory[ChannelExtension->ExecutionHistoryNextAvailableIndex].IS = (ULONG)~0;
    }
    AtaPortCopyMemory(&ChannelExtension->ExecutionHistory[ChannelExtension->ExecutionHistoryNextAvailableIndex].ChannelExtension, ChannelExtension, 0x18);
  //2.2 Copy over the AHCI registers
    if(ChannelExtension->Px) {
        AtaPortCopyMemory(&ChannelExtension->ExecutionHistory[ChannelExtension->ExecutionHistoryNextAvailableIndex].Px, ChannelExtension->Px, 0x40);
    } else {
        for(i=0; i<0x10; i++) {
            ChannelExtension->ExecutionHistory[ChannelExtension->ExecutionHistoryNextAvailableIndex].Px[i] = (ULONG)~0;
        }
    }
}

VOID
  AddSetFeatures(
    PAHCI_CHANNEL_EXTENSION ChannelExtension,
    UCHAR Features,
    UCHAR SectorCount
  )
/*++
    Adds a Set Features Command to the PerservedSettings list
It assumes:
    32 command max
Called by:
    AhciHwInitialize
    RequiresControllerResources

It performs:
    1.1 Initialize
    2.1 check to see if the command has already been added
    3.1 Add New parameters to the end of the list
Affected Variables/Registers:
    PersistenSettings
--*/
{
  //1.1 Initialize
    ULONG slotsAvailable = ChannelExtension->PersistentSettings.Slots;
    UCHAR i = 0;
    
  //2.1 check to see if the command has already been added
    while (slotsAvailable) {
        if ( (ChannelExtension->PersistentSettings.CommandParams[i].Features == Features) &&
             (ChannelExtension->PersistentSettings.CommandParams[i].SectorCount == SectorCount) ) {
            
            //Parameters already exist.  Do nothing.
#ifdef ENABLE_HISTORY_LOG
            RecordExecutionHistory(ChannelExtension, 0x20000043); //Exit AddSetFeatures, already present
#endif
            return;
        }
        slotsAvailable >>= 1;
        i++;
    }

  //3.1 Add New parameters to the end of the list
    if (i < MAX_SETTINGS_PRESERVED) {   //protect against overflow which should never happen
        ChannelExtension->PersistentSettings.CommandParams[i].Features = Features;
        ChannelExtension->PersistentSettings.CommandParams[i].SectorCount = SectorCount;
        //mark the Persistent settings slot as programmed
        ChannelExtension->PersistentSettings.Slots |= (1 << i);
    }
#ifdef ENABLE_HISTORY_LOG
    RecordExecutionHistory(ChannelExtension, 0x10000043); //Exit AddSetFeatures, Added new Feature
#endif

    return;
}

BOOLEAN
  ReplaceSetFeatures(
    PAHCI_CHANNEL_EXTENSION ChannelExtension,
    UCHAR OldFeatures,
    UCHAR NewFeatures,
    UCHAR OldSectorCount,
    UCHAR NewSectorCount
  )
/*++
    Edit a PersistentSetting command
It assumes:
    nothing
Called by:
    RequiresControllerResources

It performs:
    1.1 Find the location of the given command
    2.1 Replace it with the new command
Affected Variables/Registers:
    PersistentSettings
Return Value:
    TRUE if the edit succeeded
    FALSE if the command did not already exist
--*/
{
    UCHAR i = 0;
    
    for (i = 0; i < MAX_SETTINGS_PRESERVED; i++) {
      //1.1 Find the location of the given command
        if ( (ChannelExtension->PersistentSettings.CommandParams[i].Features == OldFeatures) &&
             (ChannelExtension->PersistentSettings.CommandParams[i].SectorCount == OldSectorCount) ) {
          //2.1 Replace it with the new command
            ChannelExtension->PersistentSettings.CommandParams[i].Features = NewFeatures;
            ChannelExtension->PersistentSettings.CommandParams[i].SectorCount = NewSectorCount;
#ifdef ENABLE_HISTORY_LOG
            RecordExecutionHistory(ChannelExtension, 0x10000042); //Exit ReplaceSetFeatures Succeeded
#endif
            return TRUE;
        }
    }

#ifdef ENABLE_HISTORY_LOG
    RecordExecutionHistory(ChannelExtension, 0x10000042); //Exit ReplaceSetFeatures Failed
#endif
    return FALSE;
}

VOID
  RestorePreservedSettings(
    PAHCI_CHANNEL_EXTENSION ChannelExtension
  )
/*++
    Send each of the commands in the PersistentSettings structure one at a time using the local IRB
It assumes:
    The local IRB is not currently in use
Called by:
    AhciHwReset
    AhciHwInitialize
    AhciHwNonQueuedErrorRecovery

It performs:
    1 Reinitialize all the commands to send
    2 Start the first command

Affected Variables/Registers:
--*/
{
#ifdef ENABLE_HISTORY_LOG
    RecordExecutionHistory(ChannelExtension, 0x00000041); //RestorePreservedSettings
#endif
  //1 Reinitialize all the commands to send
    ChannelExtension->PersistentSettings.SlotsToSend = ChannelExtension->PersistentSettings.Slots;
  //2 Start the first command
    IssueSetFeatures(ChannelExtension);
    return;
}


VOID
  IssueSetFeatures(
    PVOID ChannelExtension
  )
/*++
    Uses the local IRB to send down the next Preserved Setting
It assumes:
    Local IRB is only used for restoring preseved settings
Called by:
    RestorePreservedSettings,
    Itself indirectly through local IRB callback

It performs:
    1 Verify local IRB is not in use
    2 Find the next Preserved Setting
    3 Send it

Affected Variables/Registers:
    none
--*/
{
    UCHAR i;
    PAHCI_CHANNEL_EXTENSION channelExtension = ChannelExtension;
    ULONG allocated;

#ifdef ENABLE_HISTORY_LOG
    RecordExecutionHistory(channelExtension, 0x00000040); //IssueSetFeatures
#endif
  //1 Verify local IRB is not in use
    allocated = channelExtension->SlotManager.CommandsIssued | 
                channelExtension->SlotManager.NCQueueSlice | 
                channelExtension->SlotManager.NormalQueueSlice | 
                channelExtension->SlotManager.SingleIoSlice |
                channelExtension->SlotManager.CommandsToComplete;
    if ((allocated & (1 << 0)) > 0) {
        // Already restoring preserved Settings
#ifdef ENABLE_HISTORY_LOG
        RecordExecutionHistory(channelExtension, 0x30000040); //Exit IssueSetFeatures, local IRB in use
#endif
        return;
    }

  //2 find the next command to send
    for (i = 0; i < MAX_SETTINGS_PRESERVED; i++) {
        if( ( channelExtension->PersistentSettings.SlotsToSend & (1 << i) ) > 0) {
            channelExtension->PersistentSettings.SlotsToSend &= ~(1 << i);
            break;
        }
    }

    //perhaps there is none.  Done.
    if ( i >= MAX_SETTINGS_PRESERVED) {
#ifdef ENABLE_HISTORY_LOG
        RecordExecutionHistory(channelExtension, 0x20000040); //Exit IssueSetFeatures, no more commands
#endif
        return;
    }
 
  //3 Otherwise use the LocalIRB to send the command. When it is done, call this routine again
    IssueSetFeaturesIrb(channelExtension, 
        channelExtension->PersistentSettings.CommandParams[i].Features, 
        channelExtension->PersistentSettings.CommandParams[i].SectorCount);
#ifdef ENABLE_HISTORY_LOG
    RecordExecutionHistory(channelExtension, 0x10000040); //Exit IssueSetFeatures, IssueSetFeatures
#endif
    return;
}

VOID
  IssueSetFeaturesIrb(
    PAHCI_CHANNEL_EXTENSION ChannelExtension,
    UCHAR Features,
    UCHAR SectorCount
    )
/*++
    This could be a macro.  broken out here to make the logic easier to read
It assumes:
    nothing
Called by:
    IssueSetFeatures

It performs:
    1 Fills in the local IRB with the SetFeatures command
    2 Starts processing the command
Affected Variables/Registers:
    none

--*/
{
    PIDE_REQUEST_BLOCK irb;
    
  //1 Fills in the local IRB with the SetFeatures command
    irb = &ChannelExtension->Local.Irb;
    irb->Function = IRB_FUNCTION_ATA_COMMAND;
    
    irb->IrbStatus = 0;
    irb->AtaStatus = 0;
    irb->AtaError = 0;
    irb->Channel = (UCHAR) ChannelExtension->ChannelNumber;
    irb->TargetId = 0;
    irb->Lun = 0;
    irb->CdbLength = 0;   
    irb->SenseInfoBufferLength = 0;    
    irb->SenseInfoBufferType = 0;       
    irb->QueueTag = 0;
    irb->ReservedAsUlong = 0;
    irb->IrbFlags = 0;                 
    irb->TimeOutValue = 1;             
    irb->DataTransferLength = 0;       
    irb->IrbExtension = (PVOID)ChannelExtension->Local.IrbExtension;
    irb->DataBuffer = NULL;               
    irb->SenseInfoBuffer = NULL;          
    irb->NextIrb = NULL;
    irb->Reserved = (PVOID)IssueSetFeatures;
    irb->IdeTaskFile.Current.bFeaturesReg = Features;
    irb->IdeTaskFile.Current.bSectorCountReg = SectorCount;
    irb->IdeTaskFile.Current.bSectorNumberReg = 0;
    irb->IdeTaskFile.Current.bCylLowReg = 0;
    irb->IdeTaskFile.Current.bCylHighReg = 0;
    irb->IdeTaskFile.Current.bDriveHeadReg = 0;
    irb->IdeTaskFile.Current.bCommandReg = IDE_COMMAND_SET_FEATURE;
    irb->IdeTaskFile.Current.bReserved = 0;
    irb->IdeTaskFile.Previous.bFeaturesReg = 0;
    irb->IdeTaskFile.Previous.bSectorCountReg = 0;
    irb->IdeTaskFile.Previous.bSectorNumberReg = 0;
    irb->IdeTaskFile.Previous.bCylLowReg = 0;
    irb->IdeTaskFile.Previous.bCylHighReg = 0;
    irb->IdeTaskFile.Previous.bDriveHeadReg = 0;
    irb->IdeTaskFile.Previous.bCommandReg = 0;
    irb->IdeTaskFile.Previous.bReserved = 0;

    MARK_IRB_FOR_PIO(irb);
  //2 Starts processing the command
    AhciHwStartIo(ChannelExtension, irb);

    return;
}

PAHCI_MEMORY_REGISTERS
  GetABARAddress(
    PVOID DeviceExtension,
    PIDE_MINIPORT_RESOURCES MiniportResources
    )
/*++
    Search a set of resources (I/O and memory) for an entry corresponding to
    an AHCI ABAR address.
    NOTE: DeviceExtension is either of type PAHCI_CONTROLLER_EXTENSION
    or PAHCI_CHANNEL_EXTENSION, so typed as expected by AtaPortGetDeviceBase
It assumes:
    nothing
Called by:
    AhciAdapterControlIdeStart, AhciHwControlIdeStart
It performs:
    1. If any resources are defined, loop over all resource entries, looking for 
       the AHCI ABAR address. If found, return it.
    2. If this point is reached, ABAR was not found. Return NULL.
Affected Variables/Registers:
    none
ReturnValue:
    Address of AHCI controller memory registers if found, NULL otherwise

--*/
{
    ULONG i;
    
  //1. If any resources are defined, loop over all resource entries
    if (MiniportResources->NumberOfAccessRanges) {
        for (i=0; i<MiniportResources->NumberOfAccessRanges; i++) {
            if ((MiniportResources->IdeAccessRange[i].InMemory) &&
                (MiniportResources->IdeAccessRange[i].Bar == 5) ) {
                    //AHCI is BAR 5
                    return AtaPortGetDeviceBase(DeviceExtension, MiniportResources->IdeAccessRange[i].RangeStart, 0);
            }
        }
    }
  //2. Failed to find ABAR, return NULL
    return NULL;
}


#if _MSC_VER >= 1200
#pragma warning(pop)
#else
#pragma warning(default:4054)
#pragma warning(default:4055)
#endif

