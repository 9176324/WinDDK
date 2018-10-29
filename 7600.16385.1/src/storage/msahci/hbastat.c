
/*++

Copyright (C) Microsoft Corporation, 2004

Module Name:

    hbastat.c

Abstract:
    This file contains the core logic for managing AHCI controller state transistions.
    Primarily it focuses on starting and stopping the channel and performing resets.

Notes:

Revision History:

        Nathan Obr (natobr),  February 2005 - September 2006 rev 1 (NCQ, LPM, Hotplug, persistant state) 
                              December 2006 - August 2007    rev 2 (async)
--*/

#include "geninc.h"

BOOLEAN
  AhciControllerReset( 
    IN PAHCI_CONTROLLER_EXTENSION ControllerExtension
    )
/*
This function brings the HBA and all ports to the H:Idle and P:Idle states by way of HBA Reset and P:Reset
Called By:
    AhciAdapterControlIdeStart if needed.

It assumes:
    ControllerExtension->ABAR_Address is valid.

It performs:
    //10.4.3    HBA Reset
    //5.3.2.1    P:Reset    
    //5.3.2.2    P:Init

Return value:
    None.  However note the many controller registers that hold reset values.
*/
{
    AHCI_Global_HBA_CONTROL    ghc;
    PAHCI_MEMORY_REGISTERS abar = ControllerExtension->ABAR_Address;
    int i;

//10.4.3    HBA Reset
  //If the HBA becomes unusable for multiple ports, and a software reset or port reset does not correct the problem, software may reset the entire HBA by setting GHC.HR to ‘1’.  When software sets the GHC.HR bit to ‘1’, the HBA shall perform an internal reset action.  
    ghc.HR = 1;
    AtaPortWriteRegisterUlong(&abar->GHC.AsUlong, ghc.AsUlong);

  //The bit shall be cleared to ‘0’ by the HBA when the reset is complete.  A software write of ‘0’ to GHC.HR shall have no effect.  To perform the HBA reset, software sets GHC.HR to ‘1’ and may poll until this bit is read to be ‘0’, at which point software knows that the HBA reset has completed.
    ghc.AsUlong = AtaPortReadRegisterUlong(&abar->GHC.AsUlong);    
    //5.2.2.1    H:Init
    //5.2.2.2    H:WaitForAhciEnable
    for(i=0;(i<50) && (ghc.HR == 1); i++) {
        AtaPortStallExecution(20000);  //20 milliseconds
        ghc.AsUlong = AtaPortReadRegisterUlong(&abar->GHC.AsUlong);
    }

    //If the HBA has not cleared GHC.HR to ‘0’ within 1 second of software setting GHC.HR to ‘1’, the HBA is in a hung or locked state.
    if(i == 50) {
        ControllerExtension->ErrorFlag = (1 << 29);
        return FALSE;
    }
        
    //When GHC.HR is set to ‘1’, GHC.AE, GHC.IE, the IS register, and all port register fields (except PxFB/PxFBU/PxCLB/PxCLBU) that are not HwInit in the HBA’s register memory space are reset.  The HBA’s configuration space and all other global registers/bits are not affected by setting GHC.HR to ‘1’.  Any HwInit bits in the port specific registers are not affected by setting GHC.HR to ‘1’.  The port specific registers PxFB, PxFBU, PxCLB, and PxCLBU are not affected by setting GHC.HR to ‘1’.  If the HBA supports staggered spin-up, the PxCMD.SUD bit will be reset to ‘0’; software is responsible for setting the PxCMD.SUD and PxSCTL.DET fields appropriately such that communication can be established on the Serial ATA link.  If the HBA does not support staggered spin-up, the HBA reset shall cause a COMRESET to be sent on the port

    return TRUE;
}    

BOOLEAN
  AhciPortReset(
    PAHCI_CHANNEL_EXTENSION ChannelExtension
  )
{
//Note: If Error Recovery is envoked during the port driver's initial device discovery (Identify Device), failing the proper IDENTIFY command for that device (PACKET or NON PACKET) will cause the device to not be enumerated as there is no retry logic.
/*10.4.2    Port Reset
    If a port is not functioning properly after a software reset, software may attempt to re-initialize communication with the port via a COMRESET.  It must first clear PxCMD.ST, and wait for PxCMD.CR to clear to ‘0’ before re-initializing communication.  However, if PxCMD.CR does not clear within a reasonable time (500 milliseconds), it may assume the interface is in a hung condition and may continue with issuing the port reset.
    Software causes a port reset (COMRESET) by writing 1h to the PxSCTL.DET field to invoke a COMRESET on the interface and start a re-establishment of Phy layer communications.  Software shall wait at least 1 millisecond before clearing PxSCTL.DET to 0h; this ensures that at least one COMRESET signal is sent over the interface.  After clearing PxSCTL.DET to 0h, software should wait for communication to be re-established as indicated by bit 0 of PxSSTS.DET being set to ‘1’.  Then software should write all 1s 
to the PxSERR register to clear any bits that were set as part of the port reset.
    When PxSCTL.DET is set to 1h, the HBA shall reset PxTFD.STS to 7Fh and shall reset PxSSTS.DET to 0h.  When PxSCTL.DET is set to 0h, upon receiving a COMINIT from the attached device, PxTFD.STS.BSY shall be set to ’1’ by the HBA.
*/
    return AhciHwReset( ChannelExtension );
}

VOID
  AhciNonQueuedErrorRecovery(
    PAHCI_CHANNEL_EXTENSION ChannelExtension
  )
{
/*
AHCI 1.1 Section 6.2.2.1
"The flow for system software to recover from an error when non-queued commands are issued is as follows:
    Reads PxCI to see which commands are still outstanding
    Reads PxCMD.CCS to determine the slot that the HBA was processing when the error occurred
    Clears PxCMD.ST to ‘0’ to reset the PxCI register, waits for PxCMD.CR to clear to ‘0’
    Clears any error bits in PxSERR to enable capturing new errors. 
    Clears status bits in PxIS as appropriate
    If PxTFD.STS.BSY or PxTFD.STS.DRQ is set to ‘1’, issue a COMRESET to the device to put it in an idle state
    Sets PxCMD.ST to ‘1’ to enable issuing new commands    

It assumes:
    Called asynchronously

Called by:
    WorkerDispatch

It performs:
    (overview)
    1 Recover from an error
    2 Complete the Failed Command
    3 Complete Succeeded Commands

    (details)
    1.1 Initialize
    1.2 Reads PxCI to see which commands are still outstanding
    1.3 Reads PxCMD.CCS to determine the slot that the HBA was processing when the error occurred
    1.4.1 Clears PxCMD.ST to ‘0’ to reset the PxCI register, waits for PxCMD.CR to clear to ‘0’
     1.5 Clears any error bits in PxSERR to enable capturing new errors.
     1.6 Clears status bits in PxIS as appropriate
    1.4.2 If PxTFD.STS.BSY or PxTFD.STS.DRQ is set to ‘1’, issue a COMRESET to the device to put it in an idle state
    1.4.3 If a COMRESET was issued, restore Preserved Settings
    1.4.4 Start the channel
    
    2.1 Complete the command being issued if there was one
    2.2 Restore the unsent programmed commands for careful reprocessing
    2.3 If a request sense IRB was created due to this error

    3.1 If there were commands that are ready to complete, complete them now

Affected Variables/Registers:
    CI, CMD, TFD
    Channel Extension
*/
    BOOLEAN performedCOMRESET;
    ULONG ci;
    ULONG localCommandsIssued;
    UCHAR numberCommandsOutstanding;
    ULONG failingCommand;
    AHCI_COMMAND cmd;
    AHCI_TASK_FILE_DATA tfd;
    PIDE_REQUEST_BLOCK senseIrb;
    PAHCI_CHANNEL_EXTENSION channelExtension = (PAHCI_CHANNEL_EXTENSION) ChannelExtension;
    senseIrb = NULL;
    
  //1.1 Initialize variables
#ifdef ENABLE_HISTORY_LOG
    RecordExecutionHistory(channelExtension, 0x00000013);//AhciNonQueuedErrorRecovery
#endif
    performedCOMRESET = FALSE;

  //1.2 Reads PxCI to see which commands are still outstanding
    ci = AtaPortReadRegisterUlong(&channelExtension->Px->CI);

  //1.3 Reads PxCMD.CCS to determine the slot that the HBA was processing when the error occurred
    cmd.AsUlong = AtaPortReadRegisterUlong(&channelExtension->Px->CMD.AsUlong);

  //1.4 Clears PxCMD.ST to ‘0’ to reset the PxCI register, waits for PxCMD.CR to clear to ‘0’
    if (! P_NotRunning(channelExtension, channelExtension->Px, FALSE) ){ //This clears PxCI
        RecordExecutionHistory(channelExtension, 0x10160013);//Stop Failed
        channelExtension->StateFlags.CallAhciReset = TRUE;
        if(!AtaPortRequestSynchronizedRoutine(channelExtension, WorkerDispatch)){
            WorkerDispatch(channelExtension);
        }
        return;
    }
    
  //2.1 Complete the command being issued if there was one
        
    //Determine how many commands are outstanding
    localCommandsIssued = channelExtension->SlotManager.CommandsIssued;
    numberCommandsOutstanding = 0;
    failingCommand = 0;
    while (localCommandsIssued) {
        if (localCommandsIssued & 1) {
            numberCommandsOutstanding++;
        }
        //To keep from having to walk through the localCommandsIssued mask again, 
        //if there is only 1 command, failing commands will be 1 greater than that slot after this while loop.
        failingCommand++;
        localCommandsIssued >>= 1;
    }
    
    //If there is only one command outstanding, then we know which one to complete.
    if ( numberCommandsOutstanding == 1) {
        failingCommand--;
        //As long as there is an IRB in that command slot.
        if (! channelExtension->Slot[failingCommand].Irb ) {
            AhciHwReset(channelExtension);
            return;
        }
    //If NonQueuedErrorRecovery should not be done for more than 1 command stop here.
    } else if ( (cmd.ST == 1) && ( channelExtension->StateFlags.NCQ_NeverNonQueuedErrorRecovery ) ) {
        AhciHwReset(channelExtension);
        return;
    } else {
        //otherwise the failing command is in CCS, as long as there is an IRB in that command slot.
        if( ((channelExtension->SlotManager.CommandsIssued & (1 << cmd.CCS)) > 0) && 
            (channelExtension->Slot[cmd.CCS].Irb) ) {
            failingCommand = cmd.CCS;
        } else {
            AhciHwReset(channelExtension);
            return;
        }
    }
    
    // Keep in mind although Px.CMD.ST was just shut off, we are working with the previous snapshot of ST.
    if (cmd.ST == 1) {
        // Remove the errant command from the Commands Issued.
        ci &= ~(1 << failingCommand);
        // Move the errant command from Issued list to the 'to complete' list
        channelExtension->SlotManager.CommandsIssued &= ~(1 << failingCommand); 
        channelExtension->SlotManager.HighPriorityAttribute &= ~(1 << failingCommand);
        channelExtension->SlotManager.CommandsToComplete |= (1 << failingCommand);
        // Fill in the status of the command
        channelExtension->Slot[failingCommand].Irb->AtaStatus = AtaPortReadRegisterUchar(&channelExtension->Px->TFD.STS.AsUchar);    
        channelExtension->Slot[failingCommand].Irb->AtaError  = AtaPortReadRegisterUchar(&channelExtension->Px->TFD.ERR); 
        channelExtension->Slot[failingCommand].Irb->IrbStatus = IRB_STATUS_DEVICE_ERROR;
        // Handle Request Sense if needed
        if( NeedRequestSense(channelExtension->Slot[failingCommand].Irb) ) {
            senseIrb = AtaPortBuildRequestSenseIrb (channelExtension, channelExtension->Slot[failingCommand].Irb);
            if (senseIrb) {           
                senseIrb->NextIrb = channelExtension->Slot[failingCommand].Irb;
                //Make the slot available again now, its slot content will not be cleaned up later.
                channelExtension->Slot[failingCommand].CmdHeader = NULL;
                channelExtension->Slot[failingCommand].CommandHistoryIndex = 0;
                channelExtension->Slot[failingCommand].CompletionRoutine = NULL;
                channelExtension->Slot[failingCommand].Irb = NULL;
                //Do not complete this command normally, it will be completed when it's request sense is completed
                channelExtension->SlotManager.CommandsToComplete &= ~(1 << failingCommand);
                RecordExecutionHistory(channelExtension, 0x10730013);//NonNCQ Error Received RequestSense
            } else {
                // If the senseIrb could not be acquired, complete the command manually
                channelExtension->Slot[failingCommand].Irb->IrbStatus &= ~IRB_STATUS_AUTOSENSE_VALID;
                ReleaseSlottedCommand(channelExtension, (UCHAR) failingCommand);//&channelExtension->Slot[cmd.CCS]); //failed IRB
                RecordExecutionHistory(channelExtension, 0x10830013);//NonNCQ Error Failed to get RequestSense
            }
        }else{
            // Fill in the status of the command
            channelExtension->Slot[failingCommand].Irb->AtaStatus = 0x51;    
            channelExtension->Slot[failingCommand].Irb->AtaError  = 0x04; 
            channelExtension->Slot[failingCommand].Irb->IrbStatus = IRB_STATUS_DEVICE_ERROR;
            // Complete the command manually    
            ReleaseSlottedCommand(channelExtension, (UCHAR)failingCommand); //&channelExtension->Slot[cmd.CCS] );
            ActivateQueue(ChannelExtension);
            RecordExecutionHistory(channelExtension, 0x10630013);//NonNCQ Error Completed failed command
        }
    }

    //1.5 Clears any error bits in PxSERR to enable capturing new errors. 
        //Handled in the Interrupt routine
    //1.6 Clears status bits in PxIS as appropriate
        //Handled in the Interrupt routine

    //1.4.2 If PxTFD.STS.BSY or PxTFD.STS.DRQ is set to ‘1’, issue a COMRESET to the device to put it in an idle state
    tfd.AsUlong = AtaPortReadRegisterUlong(&channelExtension->Px->TFD.AsUlong);
    if(tfd.STS.BSY || tfd.STS.DRQ) {
        AhciCOMRESET(channelExtension, channelExtension->Px);
        performedCOMRESET = TRUE;
    }

    //2.2 Restore the unsent programmed commands for careful reprocessing
    channelExtension->SlotManager.NormalQueueSlice |= ci;    //put the commands that didn't get a chance to finish back into the normal queue
    channelExtension->SlotManager.CommandsIssued &= ~ci;//Remove the unfinished commands from the 'issued' list

    //2.3 If a request sense IRB was created due to this error
    if (senseIrb){
        ChannelExtension->StateFlags.QueuePaused = FALSE;
        AhciHwStartIo(channelExtension, senseIrb);
    }
    //3.1 If there were commands that are ready to complete, complete them
    if (channelExtension->SlotManager.CommandsToComplete) {
        AhciCompleteIssuedIRBs(channelExtension, IRB_STATUS_SUCCESS); //complete the successful commands to start the RS IRB.
    }

    //1.4.3 If a COMRESET was issued, restore Preserved Settings
    if (performedCOMRESET == TRUE) {
        RestorePreservedSettings(ChannelExtension);
    }
    //1.4.4 Start the channel 
    P_Running_StartAttempt(ChannelExtension);

    RecordExecutionHistory(channelExtension, 0x10000013);//Exit AhciNonQueuedErrorRecovery
}

VOID
  AhciCOMRESET(
    PAHCI_CHANNEL_EXTENSION ChannelExtension,
    PAHCI_PORT Px
    )
/*
PHY Reset:COMRESET
    SCTL.DET Controls the HBA’s device detection and interface initialization.
    DET=1 Performs interface communication initialization sequence to establish communication. This is functionally equivalent to a hard reset and results in the interface being reset and communications reinitialized.  While this field is 1h, COMRESET is transmitted on the interface.  
    Software should leave the DET field set to 1h for a minimum of 1 millisecond to ensure that a COMRESET is sent on the interface.
//TODO: Ask Amber, since we are in 5.3.2.3    P:NotRunning and PxCMD.SUD = ‘0’ does this still take us to P:StartComm?
Called By:
    AhciHwReset,AhciNonQueuedErrorRecovery,P_Running_WaitWhileDET1,AhciHwControlIdeStart
It assumes:
    nothing
It performs:
    5.3.2.11    P:StartComm
    (overview)
    1 Prepare for COMRSET
    2 Perform COMRESET
    3 Clean up after COMRESET
    (details)
    1.1 make sure ST is 0.  DET cannot be altered while ST == 1 as per AHCI 1.1 section 5.3.2.3.
    1.2 Don't allow a comm init to trigger a hotplug
    1.3 Ignore Hotplug events until the channel is started again
    2.1 Perform COMRESET
    3.1 Clear SERR

Affected Variables,Registers:
    CMD.ST, SCTL.DET, CI, SACT, SERR

Return Values:
    none
*/
{
    AHCI_SERIAL_ATA_CONTROL sctl;
    AHCI_COMMAND cmd;
    AHCI_INTERRUPT_ENABLE ieOrig;
    AHCI_INTERRUPT_ENABLE ieTemp;

#ifdef ENABLE_HISTORY_LOG
    if (ChannelExtension) {
        RecordExecutionHistory(ChannelExtension, 0x00000010);//AhciCOMRESET
    }
#endif
    
  //1.1 make sure ST is 0.  DET cannot be altered while ST == 1 as per AHCI 1.1 section 5.3.2.3.
    cmd.AsUlong = AtaPortReadRegisterUlong(&Px->CMD.AsUlong);
    if(cmd.ST == 1) {
        if (ChannelExtension) {
            RecordExecutionHistory(ChannelExtension, 0x10fa0010);//ST is 1. Abort
        }
        return;
    }

  //1.2 Don't allow a comm init to trigger a hotplug
    ieTemp.AsUlong = ieOrig.AsUlong = AtaPortReadRegisterUlong(&Px->IE.AsUlong);
    ieTemp.PRCE = 0;
    ieTemp.PCE = 0;
    AtaPortWriteRegisterUlong(&Px->IE.AsUlong, ieTemp.AsUlong);

  //1.3 Ignore Hotplug events until the channel is started again
    ChannelExtension->StateFlags.IgnoreHotplugDueToResetInProgress = TRUE;

  //2.1 Perform COMRESET
    sctl.AsUlong = AtaPortReadRegisterUlong(&Px->SCTL.AsUlong);
    sctl.DET = 1;
    AtaPortWriteRegisterUlong(&Px->SCTL.AsUlong, sctl.AsUlong);
    //DET=1 Performs interface communication initialization sequence to establish communication. This is functionally equivalent to a hard reset and results in the interface being reset and communications reinitialized.  While this field is 1h, COMRESET is transmitted on the interface.  
    //Software should leave the DET field set to 1h for a minimum of 1 millisecond to ensure that a COMRESET is sent on the interface.
    AtaPortStallExecution(1000);  

    sctl.AsUlong = AtaPortReadRegisterUlong(&Px->SCTL.AsUlong);
    sctl.DET = 0;
    AtaPortWriteRegisterUlong(&Px->SCTL.AsUlong, sctl.AsUlong);

  //1.2 Don't allow a comm init to trigger a hotplug
    AtaPortStallExecution(50);
    //Enable Hotplug again if it was enabled before
    AtaPortWriteRegisterUlong(&Px->IE.AsUlong, ieOrig.AsUlong);

  //3.1 Clear SERR
    AtaPortWriteRegisterUlong(&Px->SERR.AsUlong, (ULONG)~0);

    RecordExecutionHistory(ChannelExtension, 0x10000010);//Exit AhciCOMRESET
}


BOOLEAN
  P_NotRunning(
    PAHCI_CHANNEL_EXTENSION ChannelExtension,
    PAHCI_PORT Px,
    BOOLEAN InReset
    )
/*
Called By:
        AhciNonQueuedErrorRecovery
        AhciHwReset
        P_Running
        AhciHwControlIdeStart
        AhciHwControlIdeStop
        AhciHwControlIdePowerDown

It assumes:
    nothing

It performs:
    5.3.2.3    P:NotRunning
    (overview)
    1 Clear CMD.ST
    2 Clear CMD.FRE
    3 Update the Channel Start State

    (details)
    1.1 Clear CMD.ST
    1.2 Verify CR cleared
    1.3 Verify CI cleared
    2.1 Clear CMD.FRE
    2.2 Verify FR cleared
    3.1 Update the Channel Start State

Affected Variables,Registers:
    cmd.ST cmd.CR cmd.FRE cmd.FR are 0 and the controller is in P:NotRunning

Return value:
    FALSE if the ST,CR,FRE, and FRE could not be cleared.  TRUE if success.    
*/
{
    AHCI_COMMAND cmd; 
    ULONG ci;
    UCHAR i;
    AHCI_TASK_FILE_DATA tfd;
    BOOLEAN enableCLO = FALSE;

    // record this function is called.
    if (ChannelExtension) {
        RecordExecutionHistory(ChannelExtension, 0x00000011);//P_NotRunning
        
        if ( (InReset == TRUE) &&
             (ChannelExtension->CAP.SCLO == TRUE) &&
             (ChannelExtension->StateFlags.CLOReset_Enable) ) {
            //3.1.1
            //make sure HBA supports Command List Override.
            enableCLO = TRUE;
            RecordExecutionHistory(ChannelExtension, (0x10810011) );
        }
    }

  //1.1 Clear CMD.ST
    cmd.AsUlong = AtaPortReadRegisterUlong(&Px->CMD.AsUlong);
    //10.3.2 on FRE it says:
    //Software shall not clear this [FRE] bit while PxCMD.ST remains set to ‘1’."
    //System software places a port into the idle state by clearing PxCMD.ST and waiting for PxCMD.CR to return ‘0’ when read.  
    cmd.ST = 0;
    AtaPortWriteRegisterUlong(&Px->CMD.AsUlong, cmd.AsUlong);

    if ( enableCLO ) {

        //3.3.7
        //make sure PxCMD.ST is 0.
        cmd.AsUlong = AtaPortReadRegisterUlong(&Px->CMD.AsUlong);
        for (i=1; i < 101; i++) {
            cmd.AsUlong = AtaPortReadRegisterUlong(&Px->CMD.AsUlong);
            if (cmd.ST == 0) {
                break;
            }
            AtaPortStallExecution(5000);  //5 milliseconds
        }
        if (i == 101) {
            RecordExecutionHistory(ChannelExtension, (0x10820011 | (i << 8)) );
        }
        tfd.AsUlong = AtaPortReadRegisterUlong(&ChannelExtension->Px->TFD.AsUlong);
        if (tfd.STS.BSY) {
            cmd.CLO = 1;
            AtaPortWriteRegisterUlong(&Px->CMD.AsUlong, cmd.AsUlong);
        }
    }

  //Software should wait at least 500 milliseconds for this to occur.    
    for (i=1; i < 101; i++) {
        cmd.AsUlong = AtaPortReadRegisterUlong(&Px->CMD.AsUlong);
        if ( (cmd.CR == 0) && (cmd.ST == 0) ) {
            break;
        }
        AtaPortStallExecution(5000);  //5 milliseconds
    }

#ifdef ENABLE_HISTORY_LOG
    if (ChannelExtension) {
        RecordExecutionHistory(ChannelExtension, (0x10340011 | (i << 8)) );
    }
#endif

    //If PxCMD.CR or PxCMD.FR do not clear to ‘0’ correctly, then software may attempt a port reset or a full HBA reset to recover.
    if (i == 101) {
        return FALSE;
    }

  //1.2 Verify CR cleared
    //This must have the effect of clearling CI as per section 3.3.14
    for (i=1; i < 101; i++) {
        ci = AtaPortReadRegisterUlong(&Px->CI);
        if ( ci == 0 ) {
            break;
        } else {
            cmd.ST = 0;
            AtaPortWriteRegisterUlong(&Px->CMD.AsUlong, cmd.AsUlong);
        }
        AtaPortStallExecution(50);  //50 microseconds
    }

#ifdef ENABLE_HISTORY_LOG
    if (ChannelExtension) {
        RecordExecutionHistory(ChannelExtension, (0x10840011 | (i << 8)) );
    }
#endif

    //If CI does not clear to ‘0’ correctly abort the stop
    if (i == 101) {
        return FALSE;
    }

  //2.1 Clear CMD.FRE
    cmd.AsUlong = AtaPortReadRegisterUlong(&Px->CMD.AsUlong);
    if( (cmd.FRE || cmd.FR) != 0){
        //If PxCMD.FRE is set to ‘1’, software should clear it to ‘0’ and wait at least 500 milliseconds for PxCMD.FR to return ‘0’ when read.  
        //10.3.2 Software shall not clear this bit while PxCMD.ST or PxCMD.CR is set to ‘1’.

        cmd.FRE = 0;
        AtaPortWriteRegisterUlong(&Px->CMD.AsUlong, cmd.AsUlong);

        if ( enableCLO ) {
            tfd.AsUlong = AtaPortReadRegisterUlong(&ChannelExtension->Px->TFD.AsUlong);

            if (tfd.STS.BSY) {
                cmd.CLO = 1;
                AtaPortWriteRegisterUlong(&Px->CMD.AsUlong, cmd.AsUlong);
            }
        }

        //Software should wait at least 500 milliseconds for this to occur.  
        for (i=1; i<101; i++) {
            cmd.AsUlong = AtaPortReadRegisterUlong(&Px->CMD.AsUlong);
            if( (cmd.CR == 0) && (cmd.FR == 0) && (cmd.ST == 0) && (cmd.FRE == 0) ) {
                break;
            }
            AtaPortStallExecution(5000);    //  5 milliseconds
        }

#ifdef ENABLE_HISTORY_LOG
        if (ChannelExtension) {
            RecordExecutionHistory(ChannelExtension, (0x10330011 | (i << 8)) );
        }
#endif

        if (i == 101) {  //If PxCMD.CR or PxCMD.FR do not clear to ‘0’ correctly, then software may attempt a port reset or a full HBA reset to recover.
            if ( enableCLO ) {
                //if we return caller will start COMRESET.
                return TRUE;
            }
            return FALSE;
        }
    }

  //3.1 Update the Channel Start State
    ChannelExtension->StartState.ChannelNextStartState = WaitOnDET;
    ChannelExtension->StartState.ChannelStateDETCount = 0;
    ChannelExtension->StartState.ChannelStateDET1Count = 0;
    ChannelExtension->StartState.ChannelStateDET3Count = 0;
    ChannelExtension->StartState.ChannelStateFRECount = 0;
    ChannelExtension->StartState.ChannelStateBSYDRQCount = 0;

#ifdef ENABLE_HISTORY_LOG
    if (ChannelExtension) {
        RecordExecutionHistory(ChannelExtension, 0x10000011);//Exit P_NotRunning
    }
#endif

    return TRUE;
}


BOOLEAN
  P_Running(
    PAHCI_CHANNEL_EXTENSION ChannelExtension
    )
/*
    The purpose of this function is to verify and drive the Start Channel state machine

Called By:
    P_Running_StartAttempt

    Through AtaPortRequestTimer which is set up by each of the Start Channel state functions:
            P_Running_WaitOnDET
            P_Running_WaitWhileDET1
            P_Running_WaitOnDET3
            P_Running_WaitOnFRE
            P_Running_WaitOnBSYDRQ
It assumes:
    Nothing

It performs:
    (overview)
    1 Verify Start can/needs to be done
    2 Run the Start Channel state machine

    (details)
    1.1 First, is the channel initialized.  Would turning start on blow up the machine?
    1.2 Next, is the port somehow already running?
    1.3 Make sure the device knows it is supposed to be spun up
    1.4 Check to make sure that FR and CR are both 0.  Attempt to bring the controller into a consistent state by stopping the controller. 
    1.5 CMD.ST has to be set, when that happens check to see that PxSSTS.DET is not ‘4h’

    2.1 Dispatch to the current state (states are responsible for selecting the next state)

Affected Variables/Registers:
    CMD 

Return Values:
    TRUE if the state machine can be run
    FALSE if the state machine can't
*/
{

    AHCI_COMMAND cmd;
    AHCI_SERIAL_ATA_STATUS ssts;
    PAHCI_PORT px = ChannelExtension->Px;

#ifdef ENABLE_HISTORY_LOG
    RecordExecutionHistory(ChannelExtension, 0x00000015);//P_Running
#endif

  //1.1 First, is the channel initialized.  Would turning start on blow up the machine?
    if( ChannelExtension->StateFlags.StartCapable == FALSE ) {
        RecordExecutionHistory(ChannelExtension, 0x10120015);//No Channel Resources
        return FALSE;
    }

  //1.2 Next, is the port somehow already running?
    cmd.AsUlong = AtaPortReadRegisterUlong(&px->CMD.AsUlong);
    if( (cmd.ST == 1) && (cmd.CR == 1) && (cmd.FRE == 1) && (cmd.FR == 1) ) {
        ChannelExtension->StartState.ChannelNextStartState = StartComplete;
#ifdef ENABLE_HISTORY_LOG
        RecordExecutionHistory(ChannelExtension, 0x30000015);//Channel Already Running
#endif
        return TRUE;
    }


  //1.3 Make sure the device knows it is supposed to be spun up
    cmd.SUD = 1;
    AtaPortWriteRegisterUlong(&px->CMD.AsUlong, cmd.AsUlong);

  //1.4 Check to make sure that FR and CR are both 0.  If not then ST and/or FRE are 0 which a bad scenario.  
    if ( ( (cmd.FR == 1) && (cmd.FRE == 0) ) || 
         ( (cmd.CR == 1) && (cmd.ST  == 0) ) ){
        //Attempt to bring the controller into a consistent state by stopping the controller. 
        if ( !P_NotRunning(ChannelExtension, ChannelExtension->Px, FALSE) ) {
            RecordExecutionHistory(ChannelExtension, 0x10880015);//CR or FR 1 when ST or FRE 0
            //return FALSE;
        }
    }

  //1.5 CMD.ST has to be set, when that happens check to see that PxSSTS.DET is not ‘4h’
    ssts.AsUlong = AtaPortReadRegisterUlong(&px->SSTS.AsUlong);
    if( ssts.DET == 0x4){
        P_Running_StartFailed(ChannelExtension);
        RecordExecutionHistory(ChannelExtension, 0x10150015);//Channel Disabled
        return FALSE;
    }

  //2.1 Ok, run the Start Channel State Machine
    switch(ChannelExtension->StartState.ChannelNextStartState) {

        case WaitOnDET:
            P_Running_WaitOnDET(ChannelExtension);
            break;
        case WaitWhileDET1:
            P_Running_WaitWhileDET1(ChannelExtension);
            break;
        case WaitOnDET3:
            P_Running_WaitOnDET3(ChannelExtension);
            break;
        case WaitOnFRE:
            P_Running_WaitOnFRE(ChannelExtension);
            break;
        case WaitOnBSYDRQ:
            P_Running_WaitOnBSYDRQ(ChannelExtension);
            break;
        default:
            break;
    }

    // record this function is called.
    RecordExecutionHistory(ChannelExtension, 0x10000015);//Exit P_Running
    return TRUE;
}

VOID
  P_Running_WaitOnDET(
    PAHCI_CHANNEL_EXTENSION ChannelExtension
    )
/*   
    Search for Device Activity phase.  Use DET and IPM for any signs of life as defined in AHCI 1.2 section 10.1.2 and 10.3.1.
    Polled 100 times in 1 second

Called By:
    P_Running
It assumes:
    Controller is start capable 
It performs:
    (overview)
    1 Look for signs of life while DET == 0
    2 Look for signs of life while DET == 1
    3 Look for signs of life which is DET == 3
    (details)
    1.1 When looking for device presence, seeing link level power managment shall count as device present as per 10.3.1
    1.2 Wait for 1 second for DET to show signs of life
    1.3 After 1 second give up
    2.1 When looking for device presence, seeing link level power managment shall count as device present as per 10.3.1
    2.2 Otherwise continue on to WaitOnDET1
    3.1 If DET==3 we are ready for WaitOnFRE already.

Affected Variables/Registers:
    none
*/
{
    AHCI_SERIAL_ATA_STATUS ssts;

WaitOnDET_Start:
#ifdef ENABLE_HISTORY_LOG
    RecordExecutionHistory(ChannelExtension, 0x00000016);//P_Running_WaitOnDET
#endif

  //1 Look for signs of life while DET == 0
    ssts.AsUlong = AtaPortReadRegisterUlong(&ChannelExtension->Px->SSTS.AsUlong);
    if (ssts.DET == 0)  { //When a COMRESET is sent to the device the PxSSTS.DET field shall be cleared to 0h.
      
      //1.1 When looking for device presence, seeing link level power managment shall count as device present as per 10.3.1
        if ((ssts.IPM == 0x2) || (ssts.IPM == 0x6)) {
            ChannelExtension->StartState.ChannelNextStartState = WaitOnFRE;
            RecordExecutionHistory(ChannelExtension, 0x10800016);//P_Running_WaitOnDET is 0 w/ LPM activity, goto FRE
            P_Running_WaitOnFRE(ChannelExtension);
            return;
      //1.3 After 1 second give up
        } else if (ChannelExtension->StartState.ChannelStateDETCount > 100) {
            P_Running_StartFailed(ChannelExtension);
            RecordExecutionHistory(ChannelExtension, 0x10ff0016);//P_Running_WaitOnDET Timed out, No Device
            return;
      //1.2 Wait for 1 second for DET to show signs of life
        } else {
            ChannelExtension->StartState.ChannelStateDETCount++;
            RecordExecutionHistory(ChannelExtension, 0x10810016);//P_Running_WaitOnDET is 0, still waiting
            
            if (ChannelExtension->StateFlags.Crashing) {
                AtaPortStallExecution(10000); //10 milliseconds
                goto WaitOnDET_Start;
            } else {
                if(! AtaPortRequestTimer(ChannelExtension, P_Running, 10000)) { //10 milliseconds
                    AtaPortStallExecution(10000); //10 milliseconds
                    goto WaitOnDET_Start;
                }
            }
            return;
        }

    } else {
      //2 Look for signs of life while DET == 1
        if (ssts.DET == 1)  {
          //2.1 When looking for device presence, seeing link level power managment shall count as device present as per 10.3.1
            if ((ssts.IPM == 0x2) || (ssts.IPM == 0x6)) {
                ChannelExtension->StartState.ChannelNextStartState = WaitOnFRE;
                RecordExecutionHistory(ChannelExtension, 0x10a00016);//P_Running_WaitOnDET is 1 w/ LPM activity, goto FRE
                P_Running_WaitOnFRE(ChannelExtension);
                return;
          //2.2 Otherwise continue on to WaitOnDET1
            } else {
                ChannelExtension->StartState.ChannelNextStartState = WaitWhileDET1;
                RecordExecutionHistory(ChannelExtension, 0x10a10016);//P_Running_WaitOnDET is 1
                P_Running_WaitWhileDET1(ChannelExtension);
                return;
            }
      //3.1 If DET==3 we are ready for WaitOnFRE already.
        } else if (ssts.DET == 3)  {
            ChannelExtension->StartState.ChannelNextStartState = WaitOnFRE; 
            RecordExecutionHistory(ChannelExtension, 0x10a20016);//P_Running_WaitOnDET is 3
            P_Running_WaitOnFRE(ChannelExtension);
            return;

        } else {
            P_Running_StartFailed(ChannelExtension);
            RecordExecutionHistory(ChannelExtension, 0x10a30016);//P_Running_WaitOnDET is Bogus, aborting
            return;
        }
    }
}

VOID
  P_Running_WaitWhileDET1(
    PAHCI_CHANNEL_EXTENSION ChannelExtension
    )
/*       
    Waiting on establishment of link level communications phase.
    A '1' should become a '3'.  If it doesn't, help it along.
    NOTE:   When a COMINIT is received, the PxSSTS.DET field shall be set to 1h.
            That means a device is detected, but communications has not finished
    Polled 100 times in 1 second

Called By:
    P_Running
It assumes:
    Channel is start capable
    DET was previously the value of 1
It performs:
    (overview)
    1 Look for signs of life which is DET == 3
    2 Look for signs of live while DET == 1
    (details)
    1.1 If DET moves from 1 to 3, go to WaitOnFRE
    2.1 Wait for 1 second for DET to become 3
    2.2 After 1 second of waiting, force the controller to 150MB/s speeds and go to WaitOnDET3
Affected Variables/Registers:
    SCTL, SERR
*/
{
    AHCI_SERIAL_ATA_STATUS ssts;
    AHCI_SERIAL_ATA_CONTROL sctl;
    AHCI_SERIAL_ATA_ERROR serr;

WaitWhileDET1_Start:
#ifdef ENABLE_HISTORY_LOG
    RecordExecutionHistory(ChannelExtension, 0x00000017);//P_Running_WaitWhileDET1
#endif
  
  //1.1 If DET moves from 1 to 3, go to WaitOnFRE
    ssts.AsUlong = AtaPortReadRegisterUlong(&ChannelExtension->Px->SSTS.AsUlong);
    if (ssts.DET == 3) {  
        ChannelExtension->StartState.ChannelNextStartState = WaitOnFRE; 
        RecordExecutionHistory(ChannelExtension, 0x10a20017);//P_Running_WaitWhileDET1 done
        P_Running_WaitOnFRE(ChannelExtension);
        return;
 
    } else {
      //2.2 After 1 second of waiting, force the controller to 150MB/s speeds and go to WaitOnDET3
        if (ChannelExtension->StartState.ChannelStateDET1Count > 100) {
            //A very wise woman once taught me this trick
            //it is possible that the device is not handling speed negotation very well
            //help it out by allowing only 150MB/s
            sctl.AsUlong = AtaPortReadRegisterUlong(&ChannelExtension->Px->SCTL.AsUlong);
            sctl.SPD = 1;
            AtaPortWriteRegisterUlong(&ChannelExtension->Px->SCTL.AsUlong, sctl.AsUlong);
            //Reset the port
            AhciCOMRESET(ChannelExtension, ChannelExtension->Px);
            //and clear out SERR
            serr.AsUlong = (ULONG)~0;
            AtaPortWriteRegisterUlong(&ChannelExtension->Px->SERR.AsUlong, serr.AsUlong);

            ChannelExtension->StartState.ChannelNextStartState = WaitOnDET3;
            RecordExecutionHistory(ChannelExtension, 0x100a0017);//P_Running_WaitWhileDET1 timed out, speed stepping down
            if (ChannelExtension->StateFlags.Crashing) {
                AtaPortStallExecution(10000); //10 milliseconds
                P_Running_WaitOnDET3(ChannelExtension);
            } else {
                if(! AtaPortRequestTimer(ChannelExtension, P_Running, 10000)){ //10 milliseconds
                    AtaPortStallExecution(10000); //10 milliseconds
                    P_Running_WaitOnDET3(ChannelExtension);
                }
            }
            return;
      //2.1 Wait for DET to become 3 for 1 second
        } else {
            ChannelExtension->StartState.ChannelStateDET1Count++;
            RecordExecutionHistory(ChannelExtension, 0x10600017);//P_Running_WaitWhileDET1 still waiting
            if (ChannelExtension->StateFlags.Crashing) {
                AtaPortStallExecution(10000); //10 milliseconds 
                goto WaitWhileDET1_Start;
            } else {
                if(! AtaPortRequestTimer(ChannelExtension, P_Running, 10000)){ //10 milliseconds
                    AtaPortStallExecution(10000); //10 milliseconds 
                    goto WaitWhileDET1_Start;
                }
            }
            return;
        }
    }
}

VOID
  P_Running_WaitOnDET3(
    PAHCI_CHANNEL_EXTENSION ChannelExtension
    )
/*    
    Done monkeying phase.  
    From here on out only a DET of 3 will do

Called By:
    P_Running

It assumes:
    Channel is start capable

It performs:
    (overview)
    1 Look for signs of life which are DET == 3
    (details)
    1.1 Wait for 1 second until DET becomes 3
    1.2 After waiting for 1 second, give up on starting the channel
    1.3 If DET==3 we are ready for WaitOnFRE

Affected Variables/Registers:
    none
*/
{
    AHCI_SERIAL_ATA_STATUS ssts;

WaitOnDET3_Start:
#ifdef ENABLE_HISTORY_LOG
    RecordExecutionHistory(ChannelExtension, 0x00000018);//P_Running_WaitOnDET3
#endif

  //1.2 After waiting for 1 second, give up on starting the channel
    ssts.AsUlong = AtaPortReadRegisterUlong(&ChannelExtension->Px->SSTS.AsUlong);
    if (ssts.DET != 3) { 

        if (ChannelExtension->StartState.ChannelStateDET3Count > 100) {
            P_Running_StartFailed(ChannelExtension);
            RecordExecutionHistory(ChannelExtension, 0x10ff0018);//P_Running_WaitOnDET3 timed out
            return;
      //1.1 Wait for 1 second until DET becomes 3
        } else {
            ChannelExtension->StartState.ChannelStateDET3Count++;
            RecordExecutionHistory(ChannelExtension, 0x10080018);//P_Running_WaitOnDET3 still waiting
            if (ChannelExtension->StateFlags.Crashing) {
                AtaPortStallExecution(10000); //10 milliseconds 
                goto WaitOnDET3_Start;
            } else {
                if(!AtaPortRequestTimer(ChannelExtension, P_Running, 10000)){ //10 milliseconds
                    AtaPortStallExecution(10000); //10 milliseconds 
                    goto WaitOnDET3_Start;
                }
            }
            return;
        }
  //1.3 If DET==3 we are ready for WaitOnFRE
    } else {
        ChannelExtension->StartState.ChannelNextStartState = WaitOnFRE;
        RecordExecutionHistory(ChannelExtension, 0x100a0018);//P_Running_WaitOnDET3 Success
        P_Running_WaitOnFRE(ChannelExtension);
        return;
    }
}

VOID
  P_Running_WaitOnFRE(
    PAHCI_CHANNEL_EXTENSION ChannelExtension
    )
/*    
    Start the Receive buffer phase
    Wait for confirmation is a 'nice to have' but isn't necessary.

Called By:
    P_Running

It assumes:
    DET == 3 or IPM == 02 or 06

It performs:
    (overview)
    1 Set FRE
    2 Wait for 50ms for FR to reflect running status
    3 Move to WaitOnBSYDRQ
    (details)
    1.1 Set FRE
    2.1 Wait for 50ms for FR to reflect running status
    3.1 Move to WaitOnBSYDRQ
Affected Variables/Registers:
    CMD
*/
{
    AHCI_COMMAND cmd;
 
WaitOnFRE_Start:
#ifdef ENABLE_HISTORY_LOG
    RecordExecutionHistory(ChannelExtension, 0x00000019);//P_Running_FRE
#endif

  //1.1 Set FRE 
    cmd.AsUlong = AtaPortReadRegisterUlong(&ChannelExtension->Px->CMD.AsUlong);
    cmd.FRE = 1;
    AtaPortWriteRegisterUlong(&ChannelExtension->Px->CMD.AsUlong, cmd.AsUlong);
  
    cmd.AsUlong = AtaPortReadRegisterUlong(&ChannelExtension->Px->CMD.AsUlong);
    if (cmd.FR == 1) {
      //3.1 Move to WaitOnBSYDRQ  
        ChannelExtension->StartState.ChannelNextStartState = WaitOnBSYDRQ;
        RecordExecutionHistory(ChannelExtension, 0x000a0019);//P_Running_WaitOnFRE Success
        P_Running_WaitOnBSYDRQ(ChannelExtension);
        return;

    } else {
      //3.1 Move to WaitOnBSYDRQ
        if (ChannelExtension->StartState.ChannelStateFRECount > 5) {
            ChannelExtension->StartState.ChannelNextStartState = WaitOnBSYDRQ;
            RecordExecutionHistory(ChannelExtension, 0x00ff0019);//P_Running_WaitOnFRE timed out
            P_Running_WaitOnBSYDRQ(ChannelExtension);
            return;
      //2.1 Wait for 50ms for FR to reflect running status
        } else {
            ChannelExtension->StartState.ChannelStateFRECount++;
            RecordExecutionHistory(ChannelExtension, 0x00080019);//P_Running_WaitOnFRE still waiting
            if (ChannelExtension->StateFlags.Crashing) {
                AtaPortStallExecution(10000); //10 milliseconds 
                goto WaitOnFRE_Start;
            } else {
                if(! AtaPortRequestTimer(ChannelExtension, P_Running, 10000)) { //10 milliseconds
                    AtaPortStallExecution(10000); //10 milliseconds 
                    goto WaitOnFRE_Start;
                }
            }
            return; 
        }
    }   
}

VOID
  P_Running_WaitOnBSYDRQ(
    PAHCI_CHANNEL_EXTENSION ChannelExtension
    )
/*    
    Home stretch

Called By:
    P_Running
It assumes:
    DET == 3 or IPM == 02 or 06
It performs:
    (overview)
    1 Enable BSY and DRQ to go to 0
    2 Wait for BSY and DRQ to go to 0
    3 Set ST to 1
    (details)
    1.1 Clear serr.DIAG.X
    2.1 Wait for the rest of the 10 seconds for BSY and DRQ to clear
    2.2 After a total amount of 3 seconds working on the start COMRESET
    2.3 After waiting for the rest of the 4 seconds for BSY and DRQ to clear, give up
    3.1 Set ST to 1
Affected Variables/Registers:

*/
{
    AHCI_COMMAND cmd;
    AHCI_TASK_FILE_DATA tfd;
    AHCI_SERIAL_ATA_ERROR serr;
    USHORT totalstarttime;

WaitOnBSYDRQ_Start:
    RecordExecutionHistory(ChannelExtension, 0x0000001a);//P_Running_WaitOnBSYDRQ
    tfd.AsUlong = AtaPortReadRegisterUlong(&ChannelExtension->Px->TFD.AsUlong);

  //1.1 Enable BSY and DRQ to go to 0
    if ( (tfd.STS.BSY) || (tfd.STS.DRQ) ) {
        //When [serr.DIAG.X is] set to one this bit indicates a COMINIT signal was received.  This bit is reflected in the P0IS.PCS bit.
        //to allow the TFD to be updated serr.DIAG.X must be cleared.
        serr.AsUlong = 0;
        serr.DIAG.X = 1;
        AtaPortWriteRegisterUlong(&ChannelExtension->Px->SERR.AsUlong, serr.AsUlong);
    }
  //3.1 Set ST to 1
    if ( ( tfd.STS.BSY == 0) && ( tfd.STS.DRQ == 0) ) {
      //We made it!  Set ST and start the IO we have collected!
        cmd.AsUlong = AtaPortReadRegisterUlong(&ChannelExtension->Px->CMD.AsUlong);
        cmd.ST = 1;
        AtaPortWriteRegisterUlong(&ChannelExtension->Px->CMD.AsUlong, cmd.AsUlong);

        ChannelExtension->StartState.ChannelNextStartState = StartComplete;
        ChannelExtension->StateFlags.IgnoreHotplugDueToResetInProgress = FALSE;

        
        ActivateQueue(ChannelExtension);
        RecordExecutionHistory(ChannelExtension, 0x100a001a);//Exit P_Running_WaitOnBSYDRQ Succeeded
        return;

    } else {
      //2.3 After waiting for the remainder of the 10 second maximum Channel Start time for BSY and DRQ to clear, give up
        totalstarttime = ChannelExtension->StartState.ChannelStateDETCount  +
                         ChannelExtension->StartState.ChannelStateDET1Count +
                         ChannelExtension->StartState.ChannelStateDET3Count +
                         ChannelExtension->StartState.ChannelStateFRECount  +
                        (ChannelExtension->StartState.ChannelStateBSYDRQCount * 2);
        if ( totalstarttime > 1000) {
            P_Running_StartFailed(ChannelExtension);
            RecordExecutionHistory(ChannelExtension, 0x00ff001a);//P_Running_WaitOnBSYDRQ timed out
           return;
      //2.2 After a total amount of 3 seconds working on the start COMRESET
        } else if ( ChannelExtension->StartState.ChannelStateBSYDRQCount == 50 ){
            //Stop FRE,FR in preperation for RESET
            if (! P_NotRunning(ChannelExtension, ChannelExtension->Px, TRUE) ){
                //It takes 1/2 second for stop to fail.  
                //This is taking way too long and the controller is not responding properly. Abort the start.
                P_Running_StartFailed(ChannelExtension);
                RecordExecutionHistory(ChannelExtension, 0x00fc001a);//P_Running_WaitOnBSYDRQ Stop Failed on COMRESET
                return;
            }

            //All set, bring down the hammer
            AhciCOMRESET(ChannelExtension, ChannelExtension->Px);

            //Set the timers for time remaining.  This is a best case scenario and the best that can be offered.
            ChannelExtension->StartState.ChannelStateDETCount = 0;
            ChannelExtension->StartState.ChannelStateDET1Count = 0;
            ChannelExtension->StartState.ChannelStateDET3Count = 0; // won't exceed 100 * 10ms for 100 ms
            ChannelExtension->StartState.ChannelStateFRECount = 0;  // won't exceed 5 * 10ms for 50 ms
            ChannelExtension->StartState.ChannelStateBSYDRQCount = 51; //won't exceed 500 * 20ms for 10 seconds minus what DET3 and FRE use.
            //Go back to WaitOnDet3
            ChannelExtension->StartState.ChannelNextStartState = WaitOnDET3;
            RecordExecutionHistory(ChannelExtension, 0x00fd001a);//P_Running_WaitOnBSYDRQ crossing 1 second.  COMRESET done, going to WaitOnDET3
            P_Running_WaitOnDET3(ChannelExtension);
            return;
      //2.1 Wait for the rest of the 4 seconds for BSY and DRQ to clear      
        } else {
            ChannelExtension->StartState.ChannelStateBSYDRQCount++;
            RecordExecutionHistory(ChannelExtension, 0x0008001a);//P_Running_WaitOnBSYDRQ still waiting
            if (ChannelExtension->StateFlags.Crashing) {
                AtaPortStallExecution(20000); //20 milliseconds 
                goto WaitOnBSYDRQ_Start;
            } else {
                if(! AtaPortRequestTimer(ChannelExtension, P_Running, 20000)){ //20 milliseconds
                    AtaPortStallExecution(20000); //20 milliseconds 
                    goto WaitOnBSYDRQ_Start;
                }
            }
            return;
        }
    }
}

VOID
  P_Running_StartFailed(
    PAHCI_CHANNEL_EXTENSION ChannelExtension
    )
/*    

Called By:
    P_Running_WaitOnDET
    P_Running_WaitOnDET3
    P_Running_WaitOnBSYDRQ

It assumes:
    nothing

It performs:
    (overview)
    1 clean up internal state structures
    2 complete all the commands that showed up while the start channel failed
    (details)
    1.1 update state machines
    1.2 clear out the programmed slots
    2.1 complete all outstanding commands

Affected Variables/Registers:
    none
*/
{
  //1.1 update state machines
    ChannelExtension->StartState.ChannelNextStartState = StartFailed;
    ChannelExtension->StateFlags.IgnoreHotplugDueToResetInProgress = FALSE;
  //1.2 clear out the programmed slots
    ChannelExtension->SlotManager.CommandsToComplete |= 
        (ChannelExtension->SlotManager.NCQueueSlice | 
        ChannelExtension->SlotManager.NormalQueueSlice | 
        ChannelExtension->SlotManager.SingleIoSlice |
        ChannelExtension->SlotManager.CommandsIssued);
    ChannelExtension->SlotManager.CommandsIssued = 0;
    ChannelExtension->SlotManager.NCQueueSlice = 0;
    ChannelExtension->SlotManager.NormalQueueSlice = 0;
    ChannelExtension->SlotManager.SingleIoSlice = 0;
    ChannelExtension->SlotManager.HighPriorityAttribute = 0;

    
  //2.1 complete all outstanding commands
    AhciCompleteIssuedIRBs(ChannelExtension, IRB_STATUS_BUS_RESET);
    
    return;
}

VOID
  P_Running_StartAttempt(
    PAHCI_CHANNEL_EXTENSION ChannelExtension
    )
/*    

Called By:
    AhciHwReset
    AhciNonQueuedErrorRecovery
    AhciHwControlIdeStart
    AhciHwControlIdePowerUp
    AhciReportBusChange
It assumes:
    Channel Start may already be in progress (not function reentrant)
It performs:
    1 Initializes the Channel Start state machine
    2 Starts the Channel Start state machine

Affected Variables/Registers:
    none
*/
{
  //1 Initializes the Channel Start state machine
    ChannelExtension->StartState.ChannelNextStartState = WaitOnDET;
    ChannelExtension->StartState.ChannelStateDETCount = 0;
    ChannelExtension->StartState.ChannelStateDET1Count = 0;
    ChannelExtension->StartState.ChannelStateDET3Count = 0;
    ChannelExtension->StartState.ChannelStateFRECount = 0;
    ChannelExtension->StartState.ChannelStateBSYDRQCount = 0;
  //2 Starts the Channel Start state machine
    P_Running(ChannelExtension);
    return;
}

VOID
  WorkerDispatch(
    PAHCI_CHANNEL_EXTENSION ChannelExtension
  )
/*
    Dispatch routine for asynchronous handler functions
    This is done because AtaPortRequestSynchronizedRoutine only keeps track of the last function requested.
    Consequently the ATAminiport must keep track if it wants to call multiple functions as work items.
*/
{
    if(ChannelExtension->StateFlags.CallAhciReportBusChange){
        ChannelExtension->StateFlags.CallAhciReportBusChange = FALSE;
        AhciReportBusChange(ChannelExtension);
    }

    if(ChannelExtension->StateFlags.CallAhciReset){
        ChannelExtension->StateFlags.CallAhciNonQueuedErrorRecovery = FALSE;
        ChannelExtension->StateFlags.CallAhciReset = FALSE;
        AhciHwReset(ChannelExtension);
    }

    if(ChannelExtension->StateFlags.CallAhciNonQueuedErrorRecovery){
        ChannelExtension->StateFlags.CallAhciNonQueuedErrorRecovery = FALSE;
        AhciNonQueuedErrorRecovery(ChannelExtension);
    } 

    if(ChannelExtension->StateFlags.CallAhciAutoPartialToSlumber){
        ChannelExtension->StateFlags.CallAhciAutoPartialToSlumber = FALSE;
        AhciAutoPartialToSlumber(ChannelExtension);
    } 

    return;
}


