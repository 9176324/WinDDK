/*++

Copyright (C) Microsoft Corporation, 2005

Module Name:

    io.c

Abstract:
    This file contains the core logic for programming and completing IO commands into an AHCI controller

Notes:

Revision History:

        Nathan Obr (natobr),  February 2005 - September 2006 rev 1 (NCQ, LPM, Hotplug, persistant state) 
                              December 2006 - August 2007    rev 2 (async)
--*/
#include "geninc.h"

VOID
  IRBtoATA_CFIS(
    PAHCI_CHANNEL_EXTENSION ChannelExtension,
    PSLOT_CONTENT SlotContent
  )
/*++
    Populates CFIS structure with an ATA command in the given slot
It assumes:
    The slot is empty and not being used
Called by:
    AhciHwStartIo

It performs:
    (overview)
    1 Fills in the CFIS structure
    (details)
    1.1 Map IRB fields to CFIS fields
    1.2 Specail case mapping of NCQ

Affected Variables/Registers:
    Command Table
--*/
{
    PAHCI_COMMAND_TABLE cmdTable = (PAHCI_COMMAND_TABLE) SlotContent->Irb->IrbExtension;
    PIDE_REQUEST_BLOCK irb = SlotContent->Irb;

    UNREFERENCED_PARAMETER(ChannelExtension);

  //1.1 Map IRB fields to CFIS fields
    cmdTable->CFIS.FisType = 0x27;
    cmdTable->CFIS.Reserved1 = 0;
    cmdTable->CFIS.C = 1;
    cmdTable->CFIS.Command = irb->IdeTaskFile.Current.bCommandReg;
  //1.2 Specail case mapping of NCQ
    if( IsFPDMACommand(irb) ){
        cmdTable->CFIS.Features = irb->IdeTaskFile.Current.bSectorCountReg;
        cmdTable->CFIS.Features_Exp = irb->IdeTaskFile.Previous.bSectorCountReg;
        cmdTable->CFIS.SectorCount = (irb->QueueTag << 3);
        cmdTable->CFIS.Dev_Head = 0xF & irb->IdeTaskFile.Current.bDriveHeadReg;
        cmdTable->CFIS.Dev_Head |= (1 << 6);

        if( SlotContent->StateFlags.FUA ){
            cmdTable->CFIS.Dev_Head |= (1 << 7);
        } else {
            cmdTable->CFIS.Dev_Head &= ~(1 << 7);
        }

    } else {
        cmdTable->CFIS.Features = irb->IdeTaskFile.Current.bFeaturesReg;
        cmdTable->CFIS.Features_Exp = irb->IdeTaskFile.Previous.bFeaturesReg;
        cmdTable->CFIS.SectorCount = irb->IdeTaskFile.Current.bSectorCountReg;
        cmdTable->CFIS.SectorCount_Exp = irb->IdeTaskFile.Previous.bSectorCountReg;
        cmdTable->CFIS.Dev_Head = irb->IdeTaskFile.Current.bDriveHeadReg;
    }

  ////1.1 Map IRB fields to CFIS fields
    cmdTable->CFIS.SectorNumber = irb->IdeTaskFile.Current.bSectorNumberReg;
    cmdTable->CFIS.SecNum_Exp = irb->IdeTaskFile.Previous.bSectorNumberReg;

    cmdTable->CFIS.CylLow = irb->IdeTaskFile.Current.bCylLowReg;
    cmdTable->CFIS.CylLow_Exp = irb->IdeTaskFile.Previous.bCylLowReg;

    cmdTable->CFIS.CylHigh = irb->IdeTaskFile.Current.bCylHighReg;
    cmdTable->CFIS.CylHigh_Exp = irb->IdeTaskFile.Previous.bCylHighReg;

    cmdTable->CFIS.Reserved2 = 0;
    cmdTable->CFIS.Control = 0; // Device control consists of the 48bit HighOrderByte, SRST and nIEN.  None apply here.
}

VOID
  NonDataCMDtoATA_CFIS(
    PAHCI_CHANNEL_EXTENSION ChannelExtension,
    PSLOT_CONTENT SlotContent,
    UCHAR Cmd
    )
/*++
    Populates CFIS structure with a given non data ATA command in the given slot
It assumes:
    The slot is empty and not being used
Called by:
    AhciHwStartIo

Affected Variables/Registers:
    Command Table

--*/
{
    PAHCI_COMMAND_TABLE cmdTable = (PAHCI_COMMAND_TABLE) SlotContent->Irb->IrbExtension;

    UNREFERENCED_PARAMETER(ChannelExtension);

    cmdTable->CFIS.FisType = 0x27;
    cmdTable->CFIS.Reserved1 = 0;
    cmdTable->CFIS.C = 1;
    cmdTable->CFIS.Command = Cmd;
    cmdTable->CFIS.Features = 0; 

    cmdTable->CFIS.SectorNumber = 0;
    cmdTable->CFIS.CylLow = 0;
    cmdTable->CFIS.CylHigh = 0;
    cmdTable->CFIS.Dev_Head = 0; 

    cmdTable->CFIS.SecNum_Exp = 0;
    cmdTable->CFIS.CylLow_Exp = 0;
    cmdTable->CFIS.CylHigh_Exp = 0;
    cmdTable->CFIS.Features_Exp = 0;

    cmdTable->CFIS.SectorCount = 0;
    cmdTable->CFIS.SectorCount_Exp = 0;
    cmdTable->CFIS.Reserved2 = 0;
    cmdTable->CFIS.Control = 0; 
}

VOID
  IRBtoATAPI_CFIS(
    PAHCI_CHANNEL_EXTENSION ChannelExtension,
    PSLOT_CONTENT SlotContent
  )
/*++
    Populates CFIS structure with an ATAPI command in the given slot
It assumes:
    The slot is empty and not being used
Called by:
    AhciHwStartIo

It performs:
    (overview)
    1 Popluate the ACMD
    2 Popluate the PACKET command
    (details)
    1.1 Memcopy CDB into ACMD
    2.1 Put the PACKET Command in the CFIS
    2.2 Populate DMA bit properly

Affected Variables/Registers:
    Command Table
--*/
{
                        //These are used for copying a CDB to ACMD on ATAPI commands.
    ULONG dataLength;
    UCHAR i;

  //1.1 Memcopy CDB into ACMD
    PAHCI_COMMAND_TABLE cmdTable = (PAHCI_COMMAND_TABLE) SlotContent->Irb->IrbExtension;

    UNREFERENCED_PARAMETER(ChannelExtension);

    dataLength = SlotContent->Irb->DataTransferLength;

    for(i=0; i<16; i++) {//TODO: make 16 a sizeof(ACMD)
        cmdTable->ACMD[i] = SlotContent->Irb->Cdb[i];
    }

  //2.1 Put the PACKET Command in the CFIS
    cmdTable->CFIS.FisType = 0x27;
    cmdTable->CFIS.Reserved1 = 0;
    cmdTable->CFIS.C = 1;
    cmdTable->CFIS.Command = 0xA0;   //A0 is the PACKET command
  //2.2 Populate DMA bit properly
    if (IRB_USES_DMA(SlotContent->Irb)) {
        cmdTable->CFIS.Features = 0x1;
        //DMA DIR support removed because it breaks some SATA bridges.  Ironic.
        //cmdTable->CFIS.Features |= (irb->IrbFlags & IRB_FLAGS_DATA_IN) ? 0 : 4; //bit 0 is DMA, bit 3 is DMADIR
    } else {
        cmdTable->CFIS.Features = 0x0;
    }

  //2.1 Put the PACKET Command in the CFIS
    cmdTable->CFIS.SectorNumber = 0;
    cmdTable->CFIS.CylLow = (UCHAR) dataLength; //put the low byte of the in CylLow
    dataLength >>= 8;
    cmdTable->CFIS.CylHigh = (UCHAR) dataLength; //put the high byte in CylHigh
    cmdTable->CFIS.Dev_Head = 0; 

    cmdTable->CFIS.SecNum_Exp = 0;
    cmdTable->CFIS.CylLow_Exp = 0;
    cmdTable->CFIS.CylHigh_Exp = 0;
    cmdTable->CFIS.Features_Exp = 0;

    cmdTable->CFIS.SectorCount = 0;
    cmdTable->CFIS.SectorCount_Exp = 0;
    cmdTable->CFIS.Reserved2 = 0;
    cmdTable->CFIS.Control = 0; // Device control consists of the 48bit HighOrderByte, SRST and nIEN.  None apply here.
}


ULONG
  IRBtoPRDT(
    IN PAHCI_CHANNEL_EXTENSION ChannelExtension,
    PSLOT_CONTENT SlotContent
  )
/*++

It assumes:
    MDLs and ScatterGatherList entries will not violate PRDT rules
Called by:
    AhciHwStartIo
It performs:
    (overview)
    1 Get the DataBuffer's address
    2 Map SGL entries into PRDT entries
    (details)
    1.1 Get the ScatterGatherList
    1.2 Verify that the DataBuffer is properly aligned
    2.1 Map SGL entries into PRDT entries
    2.2 Break up all 128K single entry IO into 2 64K IO entries
    2.3 Verify that the DataLength is even

Affected Variables/Registers:

Return Values:
    The number of entries generated to fill the PRDT 
    If the value returned is -1 the PRDT could not be built.
--*/
{
    PIDE_SCATTER_GATHER_LIST sgl;
    IDE_PHYSICAL_ADDRESS dataBufferPhysicalAddress;
    ULONG i;
    PAHCI_COMMAND_TABLE cmdTable = (PAHCI_COMMAND_TABLE) SlotContent->Irb->IrbExtension;
    PIDE_REQUEST_BLOCK irb = SlotContent->Irb;

    //It is possible that the IRB's ScatterGatherList is NULL.  
    //When the Ataport driver creates a RequestSense IRB it doesn't create a new ScatterGatherList for it.  For RequestSense the sense IRB's DataBuffer shall be used to hold the data sent by the ATAPI device.
    if ( (IsRequestSenseIrb (irb)) || (&ChannelExtension->Local.Irb == irb) || ( (ChannelExtension->StateFlags.AN_Enabled == TRUE) && IsGESNCommand(irb)) ){
      //1.1 Get the ScatterGatherList (in this case there is none so use the Data Buffer as the only address is a single entry scenario
        if (&ChannelExtension->Local.Irb == irb) {
            dataBufferPhysicalAddress = ChannelExtension->Local.IrbDataBufferPhysicalAddress;
        } else if ( (ChannelExtension->StateFlags.AN_Enabled == TRUE) && (IsGESNCommand(irb)) ) {
            dataBufferPhysicalAddress = ChannelExtension->GesnCachedBufferPhysicalAddress;
        } else {
            dataBufferPhysicalAddress = AtaPortGetPhysicalAddress(ChannelExtension, irb, irb->DataBuffer, NULL);
        }
      //1.2 Verify that the DataBuffer is word aligned
        if ( (dataBufferPhysicalAddress.LowPart & 0x1) == 0) {
          //2.1 Map SGL entries into PRDT entries
            cmdTable->PRDT[0].DBA.AsUlong = dataBufferPhysicalAddress.LowPart;
        } else {
            return (ULONG)-1;
        }
        if( ChannelExtension->CAP.S64A) {//If the controller supports 64 bits, write the high part too
            cmdTable->PRDT[0].DBAU = dataBufferPhysicalAddress.HighPart;
        }
      //2.3 Verify that the DataLength is even
        // all SATA transfers must be even
        // DBC is a 0 based number (i.e. 0 is 1, 1 is 2, etc.
        // sgl->Elements.Length is not (i.e. 0 is 0, 1 is 1, etc.
        if( (irb->DataTransferLength & 1) == 0 ) {                   //therefore length must be even here
            cmdTable->PRDT[0].DI.DBC = irb->DataTransferLength - 1;  //but it must be odd here
        } else {
            return (ULONG)-1;
        }
        cmdTable->PRDT[0].DI.I = 0;
        return 1; //one element, not the value 'TRUE'

    //Otherwise it has a sgl and should be processed normally.
    } else {
      //1.1 Get the ScatterGatherList
        sgl = AtaPortGetScatterGatherList(ChannelExtension, irb);

        if (sgl == NULL) {
           //return as invalid request in case of cannot get scatter gather list.
            return (ULONG)-1;
        }
      //2.2 Break up all 128K single entry IO into 2 64K IO entries
        //Some controllers cannot handle a DBC > 128K.
        if ( (sgl->NumberOfElements == 1) && (irb->DataTransferLength == 0x20000) ){      

          //1.2 Verify that the DataBuffer is properly aligned      
            if ( (sgl->Elements[0].Address.LowPart & 0x1) != 0) {
                 return (ULONG)-1;
            }

          //2.1 Map SGL entries into PRDT entries
            // Create the first entry
            cmdTable->PRDT[0].DBA.AsUlong = sgl->Elements[0].Address.LowPart;
            if( ChannelExtension->CAP.S64A) {//If the controller supports 64 bits, write the high part too
                cmdTable->PRDT[0].DBAU = sgl->Elements[0].Address.HighPart;
            }
            cmdTable->PRDT[0].DI.DBC = (0x10000 - 1);  //but it must be odd here
          //2.1 Map SGL entries into PRDT entries          
            //And do the second entry
            cmdTable->PRDT[1].DBA.AsUlong = (sgl->Elements[0].Address.LowPart + 0x10000);
            if( ChannelExtension->CAP.S64A) {//If the controller supports 64 bits, write the high part too
                if ( (sgl->Elements[0].Address.LowPart + 0x10000) < sgl->Elements[0].Address.LowPart) {
                    cmdTable->PRDT[1].DBAU = (sgl->Elements[0].Address.HighPart + 1);
                } else {
                    cmdTable->PRDT[1].DBAU = sgl->Elements[0].Address.HighPart;
                }
            }
            cmdTable->PRDT[1].DI.DBC = (0x10000 - 1);  //but it must be odd here
            return 2; //two elements

        } else {
            for (i=0; i < sgl->NumberOfElements; i++) { 
            //1.2 Verify that the DataBuffer is properly aligned
                if ( (sgl->Elements[i].Address.LowPart & 0x1) == 0) {
                  //2.1 Map SGL entries into PRDT entries
                    cmdTable->PRDT[i].DBA.AsUlong = sgl->Elements[i].Address.LowPart;
                } else {
                    return (ULONG)-1;
                }
                if( ChannelExtension->CAP.S64A) {//If the controller supports 64 bits, write the high part too
                    cmdTable->PRDT[i].DBAU = sgl->Elements[i].Address.HighPart;
                }
              //2.3 Verify that the DataLength is even
                // all SATA transfers must be even
                // DBC is a 0 based number (i.e. 0 is 1, 1 is 2, etc.
                // sgl->Elements.Length is not (i.e. 0 is 0, 1 is 1, etc.
                if( (sgl->Elements[i].Length & 1) == 0 ) {                  //therefore length must be even here
                    cmdTable->PRDT[i].DI.DBC = sgl->Elements[i].Length - 1;  //but it must be odd here
                } else {
                    return (ULONG)-1;
                }
            }
            return sgl->NumberOfElements;
        }
    }
}

VOID
  IRBtoCmdHeader(
    IN PAHCI_CHANNEL_EXTENSION ChannelExtension,
    PSLOT_CONTENT SlotContent,
    ULONG Length,
    BOOLEAN Reset
    )
/*++
It assumes:
    The chosen slot is available and not in use
Called by:
    AhciHwStartIo
It performs:
    Steps defined in AHCI 1.2 section 5.5.1 step #3
Affected Variables/Registers:
    Command Header
--*/
{
    PAHCI_COMMAND_HEADER cmdHeader = SlotContent->CmdHeader;
    PIDE_REQUEST_BLOCK irb = SlotContent->Irb;

    UNREFERENCED_PARAMETER(ChannelExtension);

//  a.  PRDTL containing the number of entries in the PRD table
    cmdHeader->DI.PRDTL = Length; 
//  b.  CFL set to the length of the command in the CFIS area
    cmdHeader->DI.CFL = 5; 
//  c.  A bit set if it is an ATAPI command
    cmdHeader->DI.A = (irb->Function & IRB_FUNCTION_ATAPI_COMMAND) ? 1 : 0;
//  d.  W (Write) bit set if data is going to the device
    cmdHeader->DI.W = (irb->IrbFlags & IRB_FLAGS_DATA_OUT) ? 1 : 0;
//  e.  P (Prefetch) bit optionally set (see rules in section 5.5.2)
    //Some controllers have problems if P is set.
    cmdHeader->DI.P = 0;
//  f.  If a Port Multiplier is attached, the PMP field set to the correct Port Multiplier port.
    cmdHeader->DI.PMP = 0;
    
    //Reset
    cmdHeader->DI.R = Reset;
    cmdHeader->DI.B = 0;
    cmdHeader->DI.C = Reset;     

    //initialize the PRD byte count
    cmdHeader->PRDBC = 0; 
    
    cmdHeader->Reserved[0] = 0;
    cmdHeader->Reserved[1] = 0;
    cmdHeader->Reserved[2] = 0;
    cmdHeader->Reserved[3] = 0;
}

VOID
  AhciCompleteIssuedIRBs(
    PAHCI_CHANNEL_EXTENSION ChannelExtension,
    UCHAR IrbStatus
  )
/*++
This function completes a command that has been programmed into a Slot back to ATAport

It assumes:
    The command in not running on the AHCI controller
    ATAPI commands will be sent 1 at a time

Called by:
    AhciPortReset
    AhciHwInterrupt
    AhciNonQueuedErrorRecovery
    P_Running_Start_Failed

It performs:
    (overview)
    1 Initialize
    2 Process all commands in CommandsToComplete
    3 Start the next batch of commands
    (details)
    1.1 Initialize variables
    2.1 For every command marked as completed
    2.2 Set the status 
    2.3 Monitor to see that any NCQ commands are completing
    2.4 Give the slot back
    3.1 Start the next IO(s) if any

Affected Variables/Registers:
    ChannelExtension->Slot[Irb->QueueTag].Irb
    ChannelExtension->SlotManager.CommandsToComplete

--*/
{                       //Only used if there is a need to issue a RequestSense
    PIDE_REQUEST_BLOCK senseIrb;

    PSLOT_CONTENT slotContent;
    ULONG ci;
    ULONG sact;
    UCHAR i;
    
#ifdef ENABLE_HISTORY_LOG
    RecordExecutionHistory(ChannelExtension, 0x00000046);//AhciCompleteIRB
#endif
  //1.1 Initialize variables
    senseIrb = NULL;
    ci = AtaPortReadRegisterUlong( &ChannelExtension->Px->CI);
    sact = AtaPortReadRegisterUlong( &ChannelExtension->Px->SACT);

  //2.1 For every command marked as completed
    for (i=0; i <= (ChannelExtension->CAP.NCS); i++) {
        if( ( ChannelExtension->SlotManager.CommandsToComplete & (1 << i) ) > 0) {
            
            if (( ( ci & (1 << i) ) > 0) ||
                ( ( sact & (1 << i) ) > 0) ) { 
                    //This shall never happen.  This command can not be completed back to ATAport, it is still running
                    continue;
                }

            slotContent = &ChannelExtension->Slot[i];

          //2.2 Set the status
            if( ( IrbStatus == IRB_STATUS_SUCCESS) &&
                ( ! IsRequestSenseIrb( slotContent->Irb) ) && 
                ( slotContent->Irb->Function != IRB_FUNCTION_ATA_SMART) &&
                ( ! IsFPDMACommand( slotContent->Irb) ) && 
                ( slotContent->CmdHeader->PRDBC != slotContent->Irb->DataTransferLength ) 
                ){
                slotContent->Irb->DataTransferLength = slotContent->CmdHeader->PRDBC;
                slotContent->Irb->IrbStatus = IRB_STATUS_DATALENGTH_MISMATCH;
            } else {
                //If anything has set a STATUS on this IRB, honor that over the one passed in
                if ( slotContent->Irb->IrbStatus == IRB_STATUS_PENDING ) {
                    slotContent->Irb->IrbStatus = IrbStatus;
                }
            }
            
          //2.3 Monitor to see that any NCQ commands are completing
            if ( (slotContent->Irb->IrbStatus == IRB_STATUS_SUCCESS) && 
                 (IsFPDMACommand(slotContent->Irb)) ) {
                ChannelExtension->StateFlags.NCQ_Succeeded = TRUE;
            }

          //2.4 Give the slot back
            ReleaseSlottedCommand(ChannelExtension, i); //slotContent); // Request sense is handled here.
#ifdef ENABLE_HISTORY_LOG
            RecordExecutionHistory(ChannelExtension, 0x10000046);//Completed IRB
#endif
        }
    }

  //3.1 Start the next IO(s) if any
    if (senseIrb){
        AhciHwStartIo(ChannelExtension, senseIrb);
    } else {
        ActivateQueue(ChannelExtension);
    }
    return;
}
