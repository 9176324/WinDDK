/*++

Copyright (C) Microsoft Corporation, 2005

Module Name:

    pwrmgmt.c

Abstract:
    This file contains all of the power managment function handlers for both controller and channel interfaces.


Notes:

Revision History:

        Nathan Obr (natobr),  February 2005 - September 2006 rev 1 (NCQ, LPM, Hotplug, persistant state) 
                              December 2006 - August 2007    rev 2 (async)
--*/
#include "geninc.h"

VOID
AhciLpmSettingsModes(
    __in PAHCI_CHANNEL_EXTENSION ChannelExtension,
    __in USER_LPM_POWER_SETTINGS LpmMode
    );


BOOLEAN 
  AhciAdapterControlIdeStart(
    IN PAHCI_CONTROLLER_EXTENSION ControllerExtension, //*ControllerExtension initialized in this function
    IN PIDE_CONTROLLER_CONFIGURATION ControllerConfiguration //*ControllerConfig initialized in this function
    )
{
/*++
    This function is called by the PCIIDEX driver indirectly when handling an IRP_MJ_PnP, IRP_MN_START
    IdeStart indicates that the adapter is being started. The miniport driver should update the member in the IDE_CONTROLLER_CONFIGURATION structure. If required, the miniport could obtain its hardware resources from the above structure.
It assumes:
    Resources for the AHCI controller exist in a single Memory Mapped range.
    No Channels have been initialized or started; no IO is outstanding.
Called by:    
    AhciAdapterControl

It performs:
    (overview)
    1. Start with some defensive structure checking
    2. Initialize ChannelExtension with Identifying memory mapped IO resources, Version and CAP.
    3. Enabling the AHCI interface as per AHCI 1.1 section 10.1.2 (part one)
    4. Initializing the IDE_CONTROLLER_CONFIGURATION structure with data from the AHCI Interface 
    (details)
    1.1 Verify that the Controller Configuration is the version we are expecting
    1.2 Verify that the Controller Extension knows of no ChannelExtensions, but take no action
    1.3 Gather Vendor,Device,Revision IDs from PCI for later
    2.1 Initialize ChannelExtension->ABAR_Address with the base address from the controller resources
    2.2 Save the version and the capabilities registers for feature negotiation once we have the Identify Device data
    3.1 Turn on AE, reset the controller if AE is already set
        AHCI 1.1 Section 10.1.2 - 1.    
        "Indicate that system software is AHCI aware by setting GHC.AE to ‘1’."
    3.2 Determine which ports are implemented by the HBA
        AHCI 1.1 Section 10.1.2 - 2.    
        "Determine which ports are implemented by the HBA, by reading the PI register. This bitmap value will aid software in determining how many ports are available and which port registers need to be initialized."
    [Not done in this function]
        AHCI 1.1 Section 10.1.2 - 3 (see AhciChannelEnabled: called once per channel later)
        "Ensure that the controller is not in the running state by reading and examining each implemented port’s PxCMD register."
        AHCI 1.1 Section 10.1.2 - 4 (see AhciChannelEnabled: called once per channel later)    
        "Determine how many command slots the HBA supports, by reading CAP.NCS."
        NCS is stored in channelExtension->CAP.NCS. NCS is a 0 based number.
        AHCI 1.1 Section 10.1.2 - 5 (see AhciHwControlIdeStart as that is the only time memory can be requested)
        "For each port, allocate memory for CLB and FB"
        AHCI 1.1 Section 10.1.2 - 6 (see AhciChannelEnabled: called once per channel later)
        "clear the PxSERR register"
        AHCI 1.1 Section 10.1.2 - 7 (see AhciHwControlIdeStart when Interrupts are connected)
        "set each implemented port’s PxIE register"
    3.3 Turn on IE after cleaning up Px.IS
        This has to be done after 4.1 because we need to know the number of channels before we check each PxIS.
        Verify that none of the PxIS registers are loaded, but take no action 
        Note: Due to the multi-tiered nature of the AHCI HBA’s interrupt architecture, system software must always ensure that the PxIS (clear this first) and IS.IPS (clear this second) registers are cleared to ‘0’ before programming the PxIE and GHC.IE registers. This will prevent any residual bits set in these registers from causing an interrupt to be asserted.    
        However, the interrupt handler hasn't been hooked up by AtaPort yet, so no interrupts will be handled by software until that happens.
    4.1 Initialize the IDE_CONTROLLER_CONFIGURATION NumberOfChannels
        The CAP.NP field is not used to determine NumberOfChannels, as the number of channels implemented isn't important.  The number of the highest channel implemented is the important piece of info.    
        To find the bit that signifies the highest number channel, start with a MSB bitmask then shift that bit toward the LSB, all the while checking against the PI register to see if that bit is set
        Ataport isn't sensitive to sparse channels.  What we need to find out is what the higest number channel.
        Later, in AhciChannelEnabled, we will actually take a look to see if each channel exists.
    4.2 Initialize the IDE_CONTROLLER_CONFIGURATION structure with data from the AHCI Interface
Affected Variables/Registers:
    ControllerExtension->ABAR_Address
    ControllerExtension->CAP
    ControllerConfiguration->NumberOfChannels
    ControllerConfiguration->NumberOfPhysicalBreaks
    ControllerConfiguration->MaximumTransferLength
    ControllerConfiguration->NativeModeEnabled
    ControllerConfiguration->BusMaster
    ControllerConfiguration->Dma64BitAddress

    GHC.AE, GHC.IE, GHC.HR 
    IS and all fields in the HBA’s register memory space except PxFB/PxFBU/PxCLB/PxCLBU that are not HwInit 
Return Values:
    The miniport driver returns TRUE if it successfully exectute the whole function.
    The port driver assumes a value of FALSE is due to IDE_CONTROLLER_CONFIGURATION structure mis-version.
    This causes the PCIIDEX driver to return from IRP_MJ_PnP, IRP_MN_START with a STATUS_REVISION_MISMATCH
    Any errors causes the driver to return FALSE and prevents the driver from loading.
--*/                                    //Used to Initialize ControllerExtension
    PAHCI_MEMORY_REGISTERS abar;
                                    //Used to find the number of channels implemented
    UCHAR i;
    ULONG pi;                     
    ULONG piMask;
    UCHAR numberOfHighestChannel;
                                    //Used to enable the AHCI interface
    AHCI_Global_HBA_CONTROL    ghc;
    ULONG is;

  //1.1 Verify ControllerConfiguration version
    if (ControllerConfiguration->Version != sizeof(IDE_CONTROLLER_CONFIGURATION) ){
        return FALSE;
    }
  //1.2 Verify no ChannelExtensions
    for (i=0; i<32; i++) {
        if(ControllerExtension->ExtensionOnSlot[i]) {
            ControllerExtension->ErrorFlag |= (1 << 31);
        }
    }
  //1.3 Gather Vendor,Device,Revision IDs from PCI for later
    if (ControllerConfiguration->ControllerMode != IdeModeDump) {
        //PCI config space is not available during hibernate or crash.
        if (! AtaPortGetBusData(ControllerExtension, &ControllerExtension->VendorID, 0, 2) ) {
            return FALSE;
        }
        if (! AtaPortGetBusData(ControllerExtension, &ControllerExtension->DeviceID, 2, 2) ) {
            return FALSE;
        }
        if (! AtaPortGetBusData(ControllerExtension, &ControllerExtension->RevisionID, 8, 1) ) {
            return FALSE;
        }
    }

  //2.1 Initialize ControllerExtension with abar
    abar = GetABARAddress((PVOID)ControllerExtension, ControllerConfiguration->ControllerResources);

  //2.1.1 If abar is still NULL after all of that, malformed resources.  We aren't going to get very far.
    if (! abar) {
        return FALSE;
    } else {
        ControllerExtension->ABAR_Address = abar;
    }
  //2.2 Initialize ControllerExtension with version & cap values
    ControllerExtension->Version.AsUlong = AtaPortReadRegisterUlong(&abar->VS.AsUlong);    
    ControllerExtension->CAP.AsUlong = AtaPortReadRegisterUlong(&abar->CAP.AsUlong);
    ControllerExtension->CAP2.AsUlong = AtaPortReadRegisterUlong(&abar->CAP2.AsUlong);

  //3.1 Turn on AE (AHCI 1.1 Section 10.1.2 - 1)
    ghc.AsUlong = AtaPortReadRegisterUlong(&abar->GHC.AsUlong);
    if (ghc.AE == 1) {
        if( ! AhciControllerReset(ControllerExtension) ) {
            return FALSE;
        }
    } //AE is 0. Either through power up or reset we are now pretty sure the controller is in 5.2.2.1    H:Init
    ghc.AsUlong = 0;
    ghc.AE = 1;
    AtaPortWriteRegisterUlong(&abar->GHC.AsUlong, ghc.AsUlong); 
  //3.2 Determine which ports are implemented by the HBA (AHCI 1.1 Section 10.1.2 - 2)
    pi = AtaPortReadRegisterUlong(&abar->PI);

  //4.1 Initialize the IDE_CONTROLLER_CONFIGURATION NumberOfChannels        
    numberOfHighestChannel = 32;
    //Check from highest bit to lowest bit for the first highest bit set
    for (piMask = (ULONG)(1 << 31); piMask != 0; piMask = (ULONG)(piMask >> 1), numberOfHighestChannel--){
        if ( (pi & piMask) != 0) {
            break;
        }
    } //numberOfHighestChannel now holds the correct value
    ControllerConfiguration->NumberOfChannels = numberOfHighestChannel; 
  //3.3 Turn on IE
    for (i=0; i < numberOfHighestChannel; i++) {
        is = AtaPortReadRegisterUlong(&abar->PortList[i].IS.AsUlong);
        if(is) { 
            //TODO: I would like to call AhciHwInterrupt, but I have no ChannelExtension.  How to clear these?
            ControllerExtension->ErrorFlag |= (ULONG)(1 << 30); 
        }
    }
    ghc.IE = 1;
    AtaPortWriteRegisterUlong(&abar->GHC.AsUlong, ghc.AsUlong);

  //4.2 Initializing the rest of IDE_CONTROLLER_CONFIGURATION
    ControllerConfiguration->NumberOfPhysicalBreaks = 32;
    ControllerConfiguration->MaximumTransferLength = AHCI_MAX_TRANSFER_LENGTH;
    ControllerConfiguration->NativeModeEnabled = TRUE;
    ControllerConfiguration->BusMaster = TRUE;
    ControllerConfiguration->Dma64BitAddress  = ControllerExtension->CAP.S64A ? TRUE : FALSE;
    ControllerConfiguration->AtaBusType = IdeBusSata;

    return TRUE;
}

BOOLEAN 
  AhciAdapterControlIdeStop(
    IN PAHCI_CONTROLLER_EXTENSION ControllerExtension
    )
{
/*++
    The miniport driver should stop using any resources allocated for this controller. 
It assumes:
    The port driver will ensure that all the channels exposed by the adapter are stopped before calling this function
Called by:    
    AhciAdapterControl

It performs:
    Undefines the ControllerExtension->ABAR_Address
Affected Variables/Registers:
    ControllerExtension->ABAR_Address
Return Values:
    TRUE always.
--*/
    ControllerExtension->ABAR_Address = 0;
    return TRUE;
}

BOOLEAN 
  AhciAdapterControlIdePowerUp(
    IN PAHCI_CONTROLLER_EXTENSION ControllerExtension
    )
{
/*++
    Indicates that the adapter is being powered up.
    Anything that doesn't persist across a power cycle needs to be done here.  
It assumes:
    PCI Ensures the HBA is in D0 (Offset PMCAP + 4h: PMCS[0,1])
Called by:    
    AhciAdapterControl

It performs:
    Enables the AHCI Interface and global Interrupts
Affected Variables/Registers:
    GHC.AE, GHC.IE  
Return Values:
    TRUE always.
--*/                                    //Used to enable the AHCI interface
    AHCI_Global_HBA_CONTROL    ghc;
    PAHCI_MEMORY_REGISTERS abar = (PAHCI_MEMORY_REGISTERS) ControllerExtension->ABAR_Address;

    ghc.AsUlong = AtaPortReadRegisterUlong(&abar->GHC.AsUlong);
    if (ghc.AE == 0) {
        ghc.AsUlong = 0;
        ghc.AE = 1;
        AtaPortWriteRegisterUlong(&abar->GHC.AsUlong, ghc.AsUlong);
    }
    if (ghc.IE == 0) {
        ghc.IE = 1;
        AtaPortWriteRegisterUlong(&abar->GHC.AsUlong, ghc.AsUlong);
    }

    return TRUE;
}

BOOLEAN 
  AhciAdapterControlIdePowerDown(
    IN PAHCI_CONTROLLER_EXTENSION ControllerExtension
    )
{
/*++
    Indicates that the adapter is being powered down.
It assumes:
    All Channels are already stopped PxCMD.ST = 0 (ATAport has called AhciHwControlIdePowerDown per channel)
    PCI powers down the HBA after this function returns: D3 Offset PMCAP + 4h: PMCS[0,1]  
Called by:    
    AhciAdapterControl

It performs:
    1. Clear GHC.IE
    AHCI 1.1 Section 8.3.3 
    "Software must disable interrupts (GHC.IE must be cleared to ‘0’) prior to requesting a transition of the HBA to the D3 state.  This precaution by software avoids an interrupt storm if an interrupt occurs during the transition to the D3 state."
Affected Variables/Registers:
    GHC.IE
Return Values:
    TRUE always.
--*/
    AHCI_Global_HBA_CONTROL    ghc;
    PAHCI_MEMORY_REGISTERS abar = (PAHCI_MEMORY_REGISTERS) ControllerExtension->ABAR_Address;

    ghc.AsUlong = AtaPortReadRegisterUlong(&abar->GHC.AsUlong);
    ghc.IE = 0;
    AtaPortWriteRegisterUlong(&abar->GHC.AsUlong, ghc.AsUlong);

    return TRUE;
}

BOOLEAN 
  AhciHwControlIdeStart(
    IN PAHCI_CHANNEL_EXTENSION ChannelExtension, 
    IN PIDE_CHANNEL_CONFIGURATION ChannelConfiguration
    )
{
/*++
    This function is used by the AtaPort driver while handling an IRP_MJ_PnP, IRP_MN_START
It assumes:
    Immediately before Ataport called this function, it connected the interrupts.
    Since interrupts were enabled back in AhciChannelEnabled it is possible that AhciHwInterrupt will run at any time now.
Called By:
    AhciHwControl

Affected Variables/Registers:
    ChannelConfiguration->NumberOfOverlappedRequests
    ChannelConfiguration->MaxTargetId
    ChannelConfiguration->SyncWithIsr
    ChannelConfiguration->SupportsWmi
    
    ChannelExtension->StateFlags.StartCapable
    ChannelExtension->CommandList
    ChannelExtension->ReceivedFIS

    PX.CLB,PX.CLBU
    PX.FB,PX.FBU
    CMD.ST
It performs:
    (overview)
    1. Start with some defensive structure checking and variable initialization
    2. Initialize the ChannelConfiguration structure (final steps)
    3. Enable the AHCI interface as per AHCI 1.1 section 10.1.2 (final steps)
    4. Allocate memory for the CommandList, the Receive FIS buffer, and IRB Extension.
    5. Start the channel processing commands.
    (details)
    1.1 Initialize variables
    1.2 Verify the Channel Configuration
    2.1 Initialize the ChannelConfiguration structure
    2.2 Initialize the Channel's base address and the controller's Interrupt Status register address
    3.1 Enable the AHCI interface
        AHCI 1.1 Section 10.1.2 - 5.    
        "For each implemented port, system software shall allocate memory for and program:
            •    PxCLB and PxCLBU (if CAP.S64A is set to ‘1’)
            •    PxFB and PxFBU (if CAP.S64A is set to ‘1’)
            It is good practice for system software to ‘zero-out’ the memory allocated and referenced by PxCLB and PxFB.  After setting PxFB and PxFBU to the physical address of the FIS receive area, system software shall set PxCMD.FRE to ‘1’."
    3.2 Enable Interrupts on the Channel
        AHCI 1.1 Section 10.1.2 - 7.    
        "Determine which events should cause an interrupt, and set each implemented port’s PxIE register with the appropriate enables."
        Note: Due to the multi-tiered nature of the AHCI HBA’s interrupt architecture, system software must always ensure that the PxIS (clear this first) and IS.IPS (clear this second) registers are cleared to ‘0’ before programming the PxIE and GHC.IE registers. This will prevent any residual bits set in these registers from causing an interrupt to be asserted.
  
    4.1 Allocate memory for the CommandList, the Receive FIS buffer and IRB Extension
        Now is the time to allocate memory that will be used for controller and per request structures.
        In AHCI, the controller structures are both the command header list and the received FIS buffer.
        The per request structure will be recieved through the IRB and will be used to make a Command Table
        The mechanism for requesting all of this memory is AtaPortGetUnCachedExtension.
        NOTE! AtaPortGetUnCachedExtension can only be called while processing a HwControl IdeStart.
        Also NOTE! In order to perform crashdump/hibernate the UncachedExtensionSize cannot be larger than 30K.

        The call to AtaPortGetUnCachedExtension is complicated by alignment restrictions that an AHCI controller has so here are the rules:
        - Command List Base Addresses must be 1K aligned, and the Command list is (sizeof (AHCI_COMMAND_HEADER) * cap.NCS), which is some multiple of 32 bytes in length.
        - The FIS Base Address must be 256 aligned, and the FIS Receive buffer is sizeof (AHCI_RECEIVED_FIS), 256 bytes in length.
        - The Command Table must be 128 aligned, and is sizeof(AHCI_COMMAND_TABLE), 1280 bytes in length thanks to some padding in the AHCI_COMMAND_TABLE structure.
        
        The alignment of the addresses (virtual and physical) returned by the function follow these rules
        - the address returned by AtaPortGetUnCachedExtension will have both its virtual and physical addresses page aligned
        - the memory received through the IRB will either be physically and virtually 4K aligned or IrbExtensionSize aligned. The first allocation will be on a 4K boundry the address of the second will be IrbExtensionSize larger than the first, the third will be IrbExtensionSize larger than the second, etc.
    
        Since the Command Header must be 1K aligned and the uncached extension starts 4K aligned, this works.
        However, the Command Header is variable and must be padded so the Received FIS is on a 256 boundry.
        Therefore the number of Command Headers must be 256/32 = 8. Round cap.NCS to the next multiple of 8
    4.2 Setup the CommandList 
        Although the pointer returned from AtaPortGetUnCachedExtension is useful to this driver, it does the controller no good and can't be used in CLB. The VIRTUAL address must be translated into the PHYSICAL address before being written to the CLB register as the controller doesn't have the CPU's virtual address translation tables.  AtaPortGetPhysicalAddress Returns the physical address for the given Va. The va has to be an offset into any one of the following buffers.
            - Irb's data buffer 
            - Irb's IrbExtension 
            - Miniport's uncached extension
    4.3 Setup the Receive FIS buffer 
        Handle the Receive FIS buffer the same as 4.2 Command List
    4.4 Setup the Local IRB Extension
    5.1 Enable Command Processing 
    5.2 Initialize the ChannelConfiguration structure
        ChannelConfiguration->ChannelNumber and    ChannelConfiguration->ChannelResources are kept default values. 
        If it is found that CI and/or SACT can be changed from a 1 to 0, Number of overlapped requests becomes 1.
        Number of overlapped requests is a 1 based number (1=1, 2=2, etc.), CAP.NCS is a 0 based number.   
    5.3 START COMMAND PROCESSING
    5.4 Check registry settings
Return Values:
    The miniport driver returns TRUE if it successfully exectute the whole function.
    Any errors causes the function to return FALSE and prevents the channel from loading. This ultimately causes a yellow '!' to show up on the channel in device manager.
--*/                                           
    PAHCI_CONTROLLER_EXTENSION controllerExtension;
    PIDE_ADVANCED_CHANNEL_CONFIGURATION advancedConfigInfo;

                                    //holders of the physical addresses that will be programmed into the port mapped IO space
    PAHCI_MEMORY_REGISTERS abar;
    IDE_PHYSICAL_ADDRESS commandListPhysicalAddress;
    IDE_PHYSICAL_ADDRESS receivedFisPhysicalAddress;
                                    //these are throw away variables
    ULONG i, paddedNCS;
    PUCHAR uncachedExtensionInitializer;
    BOOLEAN registry_read_succeeded;


                                    //msahci registry keys
    CHAR  ncqKey[] = "NcqDisabled";
    CHAR  regKeyInternalPort[] = "TreatAsInternalPort";

#ifdef ENABLE_HISTORY_LOG
    RecordExecutionHistory(ChannelExtension, 0x00000024);//AhciHwControlIdeStart
#endif

  //1.1 Initialize variables
    controllerExtension = (PAHCI_CONTROLLER_EXTENSION) ChannelExtension->ControllerExtension;
    abar = (PAHCI_MEMORY_REGISTERS) controllerExtension->ABAR_Address;
    ChannelExtension->Px = &abar->PortList[ChannelExtension->ChannelNumber];
    ChannelExtension->ControllerIS = &abar->IS;

  //1.2 Verify the Channel Configuration
    if (ChannelConfiguration->Version != sizeof(IDE_CHANNEL_CONFIGURATION) ){
        RecordExecutionHistory(ChannelExtension, 0xff00);//Improper Channel Configuration
        return FALSE;
    }

  //2.1 Initialize the ChannelConfiguration structure    
    ChannelConfiguration->NumberOfOverlappedRequests = 32;  //This must be set to the max possible value here before AtaPortGetUnCachedExtension is called
    ChannelConfiguration->MaxTargetId = 0;                   // Typically (MaxNumDevices - 1)
    ChannelConfiguration->SyncWithIsr = TRUE;               
    ChannelConfiguration->SupportsWmi = TRUE;              

    if (ChannelConfiguration->SupportedAdvances.AdvancedChannelConfigurationSupported == TRUE) {
        advancedConfigInfo = ChannelConfiguration->AdvancedChannelConfiguration;
        
        if (advancedConfigInfo->Length >= sizeof(IDE_ADVANCED_CHANNEL_CONFIGURATION) ){
            if (advancedConfigInfo->Present.VendorDefinedPower == TRUE) {
                advancedConfigInfo->VendorDefinedPower.ValidGuids = 2;
                /* 0b2d69d7-a2a1-449c-9680-f91c70521c60 -DIPM/HIPM */
                advancedConfigInfo->VendorDefinedPower.Guid[0].Data1 = 0x0b2d69d7; 
                advancedConfigInfo->VendorDefinedPower.Guid[0].Data2 = 0xa2a1; 
                advancedConfigInfo->VendorDefinedPower.Guid[0].Data3 = 0x449c;
                advancedConfigInfo->VendorDefinedPower.Guid[0].Data4[0] = 0x96; 
                advancedConfigInfo->VendorDefinedPower.Guid[0].Data4[1] = 0x80;
                advancedConfigInfo->VendorDefinedPower.Guid[0].Data4[2] = 0xf9; 
                advancedConfigInfo->VendorDefinedPower.Guid[0].Data4[3] = 0x1c;
                advancedConfigInfo->VendorDefinedPower.Guid[0].Data4[4] = 0x70;
                advancedConfigInfo->VendorDefinedPower.Guid[0].Data4[5] = 0x52;
                advancedConfigInfo->VendorDefinedPower.Guid[0].Data4[6] = 0x1c;
                advancedConfigInfo->VendorDefinedPower.Guid[0].Data4[7] = 0x60;

                /* DAB60367-53FE-4fbc-825E-521D069D2456 -Adaptive */
                advancedConfigInfo->VendorDefinedPower.Guid[1].Data1 = 0xDAB60367; 
                advancedConfigInfo->VendorDefinedPower.Guid[1].Data2 = 0x53FE; 
                advancedConfigInfo->VendorDefinedPower.Guid[1].Data3 = 0x4fbc;
                advancedConfigInfo->VendorDefinedPower.Guid[1].Data4[0] = 0x82; 
                advancedConfigInfo->VendorDefinedPower.Guid[1].Data4[1] = 0x5E;
                advancedConfigInfo->VendorDefinedPower.Guid[1].Data4[2] = 0x52; 
                advancedConfigInfo->VendorDefinedPower.Guid[1].Data4[3] = 0x1D;
                advancedConfigInfo->VendorDefinedPower.Guid[1].Data4[4] = 0x06;
                advancedConfigInfo->VendorDefinedPower.Guid[1].Data4[5] = 0x9D;
                advancedConfigInfo->VendorDefinedPower.Guid[1].Data4[6] = 0x24;
                advancedConfigInfo->VendorDefinedPower.Guid[1].Data4[7] = 0x56; 
            }
        }
    }
    
    
  //3.1 Enable the AHCI interface
  //4.1 Allocate CommandList, Receive FIS, IRB Extension
                // paddedNCS is (CAP.NCS+1) rounds up to the next multiple of 8.  Trust me :)
    paddedNCS = (ChannelExtension->CAP.NCS + 1)  +  ((8 - ((ChannelExtension->CAP.NCS + 1)%8) )%8);

    uncachedExtensionInitializer = (PUCHAR) AtaPortGetUnCachedExtension (
        ChannelExtension,                                                                            
        ( (sizeof(AHCI_COMMAND_HEADER) * paddedNCS) + sizeof(AHCI_RECEIVED_FIS) + sizeof(AHCI_COMMAND_TABLE) + sizeof(AHCI_READ_LOG_EXT_DATA) + 8 ), 
        sizeof(AHCI_COMMAND_TABLE));//<-IrbExtensionSize        // the line above is added together to calculate the UncachedExtensionSize; 8 is for GesnCachedBuffer.
    if (uncachedExtensionInitializer == NULL) {
        RecordExecutionHistory(ChannelExtension, 0xff01);//Uncached Extension Failed
        return FALSE;
    }

  //4.2 Setup the CommandList
    ChannelExtension->CommandList = (PAHCI_COMMAND_HEADER) uncachedExtensionInitializer; 
    commandListPhysicalAddress = AtaPortGetPhysicalAddress(ChannelExtension, NULL, uncachedExtensionInitializer, NULL);
    ChannelExtension->CommandListPhysicalAddress = commandListPhysicalAddress;
    if ( commandListPhysicalAddress.QuadPart == 0) {
        RecordExecutionHistory(ChannelExtension, 0xff02);//Command List Failed
        return FALSE;
    }
    for (i= 0; i < (sizeof(AHCI_COMMAND_HEADER) * paddedNCS); i++) {
        (*(uncachedExtensionInitializer))=0;
        uncachedExtensionInitializer++; 
    } //uncachedExtensionInitializer now holds the VIRTUAL address of the area to be used as the Received FIS.

  //3.1.1 PxCLB and PxCLBU (AHCI 1.1 Section 10.1.2 - 5)
    if ( (commandListPhysicalAddress.LowPart % 1024) == 0) {
        AtaPortWriteRegisterUlong(&ChannelExtension->Px->CLB.AsUlong, commandListPhysicalAddress.LowPart);
    }else{ 
        RecordExecutionHistory(ChannelExtension, 0xff03);//Command List alignment failed
        return FALSE;
       }
    if (ChannelExtension->CAP.S64A) { //If the controller supports 64 bits, write the high part too
        AtaPortWriteRegisterUlong(&ChannelExtension->Px->CLBU, commandListPhysicalAddress.HighPart);
    }

  //4.3 Setup the Receive FIS buffer 
    ChannelExtension->ReceivedFIS = (PAHCI_RECEIVED_FIS) uncachedExtensionInitializer;
    receivedFisPhysicalAddress = AtaPortGetPhysicalAddress(ChannelExtension, NULL, uncachedExtensionInitializer, NULL);
    ChannelExtension->ReceivedFisPhysicalAddress = receivedFisPhysicalAddress;
    if ( receivedFisPhysicalAddress.QuadPart == 0) {
        RecordExecutionHistory(ChannelExtension, 0xff04);//Receive FIS failed
        return FALSE;
    }

  //3.1.2 PxFB and PxFBU (AHCI 1.1 Section 10.1.2 - 5)
    if ((receivedFisPhysicalAddress.LowPart % 256) == 0) {
            AtaPortWriteRegisterUlong(&ChannelExtension->Px->FB.AsUlong, receivedFisPhysicalAddress.LowPart);
    } else {
        RecordExecutionHistory(ChannelExtension, 0xff05);//Receive FIS alignment failed
        return FALSE;
    }
    if (ChannelExtension->CAP.S64A) { //If the controller supports 64 bits, write the high part too
        AtaPortWriteRegisterUlong(&ChannelExtension->Px->FBU, receivedFisPhysicalAddress.HighPart);
    }
    // initialize the Received FIS buffer to 0s
    for (i= 0; i < sizeof(AHCI_RECEIVED_FIS); i++) {
        (*(uncachedExtensionInitializer))=0;
        uncachedExtensionInitializer++; 
    } //uncachedExtensionInitializer now holds the VIRTUAL address of the area to be used as the Local IRB Extension.

  //4.4 Setup the Local IRB Extension
    ChannelExtension->Local.IrbExtension = (PAHCI_COMMAND_TABLE) uncachedExtensionInitializer;
    ChannelExtension->Local.IrbExtensionPhysicalAddress = AtaPortGetPhysicalAddress(ChannelExtension, NULL, uncachedExtensionInitializer, NULL);
    for (i= 0; i < sizeof(AHCI_COMMAND_TABLE); i++) {
        (*(uncachedExtensionInitializer))=0;
        uncachedExtensionInitializer++; 
    }
    ChannelExtension->Local.IrbDataBuffer = (PAHCI_READ_LOG_EXT_DATA) uncachedExtensionInitializer;
    ChannelExtension->Local.IrbDataBufferPhysicalAddress = AtaPortGetPhysicalAddress(ChannelExtension, NULL, uncachedExtensionInitializer, NULL);
    for (i= 0; i < sizeof(AHCI_READ_LOG_EXT_DATA); i++) {
        (*(uncachedExtensionInitializer))=0;
        uncachedExtensionInitializer++; 
    }    
    
  //4.5 Setup the TUR and GESN receive buffers.
    for (i= 0; i < 8; i++) {
        (*(uncachedExtensionInitializer))=0;
        uncachedExtensionInitializer++; 
    }
    ChannelExtension->GesnCachedBuffer = (PUCHAR) uncachedExtensionInitializer;
    ChannelExtension->GesnCachedBufferPhysicalAddress = AtaPortGetPhysicalAddress(ChannelExtension, NULL, uncachedExtensionInitializer, NULL);

    //uncachedExtensionInitializer points to the first byte after the Local IrbDataBuffer. It can no longer be used.

  //3.2 Enable Interrupts on the Channel (AHCI 1.1 Section 10.1.2 - 7)
    AtaPortWriteRegisterUlong(&ChannelExtension->Px->IS.AsUlong, (ULONG)~0);
    AtaPortWriteRegisterUlong(ChannelExtension->ControllerIS, (1 << ChannelExtension->ChannelNumber));
    Set_PxIE(ChannelExtension, &ChannelExtension->Px->IE);

  //5.1 Enable Command Processing 
    ChannelExtension->StateFlags.StartCapable = TRUE;
    if (ChannelConfiguration->ChannelMode == IdeModeDump) {
        ChannelExtension->StateFlags.Crashing = TRUE;
    }
  //5.2 Initialize the ChannelConfiguration structure    
    if (ChannelExtension->CAP.NCS > 0) { //this leaves one emergency slot free if possible, as CAP.NCS is 0-based.
        ChannelConfiguration->NumberOfOverlappedRequests = (UCHAR) ChannelExtension->CAP.NCS;   
    } else {
        ChannelConfiguration->NumberOfOverlappedRequests = 1;
    }

    if ( (ChannelExtension->StateFlags.SingleIo == TRUE) || (ChannelExtension->StateFlags.Crashing) ) {
        ChannelConfiguration->NumberOfOverlappedRequests = 1;
    }
    ChannelExtension->NumberOfOverlappedRequests = ChannelConfiguration->NumberOfOverlappedRequests;

    if ( ChannelExtension->StateFlags.Crashing == FALSE) {
        if (ChannelExtension->StateFlags.ResetInInit == TRUE) {
            P_NotRunning(ChannelExtension, ChannelExtension->Px, TRUE);
            AhciCOMRESET(ChannelExtension, ChannelExtension->Px);
        }
    }
  //5.3 START COMMAND PROCESSING
    P_Running_StartAttempt(ChannelExtension);
  //5.4 Check registry settings
    //Don't touch the registry during a crashdump
    if (ChannelExtension->StateFlags.Crashing == FALSE) {  

        // prepare to retrieve a DWORD value, set the length to 4.
        ULONG registryBufferLength = sizeof(ULONG);
        
        ChannelExtension->RegistryBuffer = (PUCHAR) AtaPortRegistryAllocateBuffer(ChannelExtension, registryBufferLength);

        if( ChannelExtension->RegistryBuffer != NULL){
            
          // 5.4.1 Get NCQ allowance information.
            if (ChannelExtension->StateFlags.NCQ_Disallowed == FALSE) {
                
                // prepare to retrieve a BINARY value, set the length to 1.        
                *ChannelExtension->RegistryBuffer = 0;
                registryBufferLength = sizeof(UCHAR);

                registry_read_succeeded = AtaPortRegistryControllerKeyRead(ChannelExtension, 
                                                 0, 
                                                 (PCHAR)ncqKey, 
                                                 IDE_REG_BINARY, 
                                                 ChannelExtension->RegistryBuffer, 
                                                 &registryBufferLength
                                                 );             
                if ( ( *ChannelExtension->RegistryBuffer != 0 ) && ( registry_read_succeeded != FALSE) ) {
                    ChannelExtension->StateFlags.NCQ_Disallowed = TRUE;
                }
            }

           // 5.4.2 Get eSATA registry informaiton.

            // prepare to retrieve a DWORD value, set the length to 4.        
            *((PULONG)ChannelExtension->RegistryBuffer) = 0;
            registryBufferLength = sizeof(ULONG);

            registry_read_succeeded = AtaPortRegistryChannelSubkeyRead(ChannelExtension, 
                                                 0, 
                                                 (PCHAR)regKeyInternalPort, 
                                                 IDE_REG_DWORD, 
                                                 ChannelExtension->RegistryBuffer, 
                                                 &registryBufferLength
                                                 );             
            if ( ( *((PULONG)ChannelExtension->RegistryBuffer) != 0 ) && ( registry_read_succeeded != FALSE) ) {
                ChannelExtension->StateFlags.TreatAsInternalPort = TRUE;
            } else {
                ChannelExtension->StateFlags.TreatAsInternalPort = FALSE;
            }


            AtaPortRegistryFreeBuffer(ChannelExtension, ChannelExtension->RegistryBuffer);
            ChannelExtension->RegistryBuffer = NULL;
        }
    }

    RecordExecutionHistory(ChannelExtension, 0x10000024);//Exit AhciHwControlIdeStart
    return TRUE;
}

BOOLEAN 
  AhciHwControlIdeStop(
    IN PAHCI_CHANNEL_EXTENSION ChannelExtension
    )
{
/*++

    The miniport driver should stop using the resources allocated for this channel. It should relinquish the uncached extension allocated for this channel. 
It assumes:
    The uncached extension is automatically freed by the port driver.
    AhciHwControlIdeStop is called per channel by ATAport.
    AhciAdapterControlIdeStop is called after all the channels are stopped.
Called by:    
    AhciHwControl

It performs:
    (overview)
    1. Stop the channel
    2. Undefine all references to anything within the Uncached Extension
Affected Variables/Registers:
    CMD.ST, CMD.CR, CMD.FRE, CMD.FR

    ChannelExtension->CommandList
    ChannelExtension->CommandListPhysicalAddress
    ChannelExtension->ReceivedFIS
    ChannelExtension->ReceivedFisPhysicalAddress
    ChannelExtension->StateFlags.StartCapable
Return Values:
    TRUE if the function excecuted completely.
    FALSE if the channel could not be stopped.  
--*/

#ifdef ENABLE_HISTORY_LOG
    RecordExecutionHistory(ChannelExtension, 0x00000025);//AhciHwControlIdeStop
#endif
  //1. Stop the channel
    if (! P_NotRunning(ChannelExtension, ChannelExtension->Px, FALSE) ) {
        RecordExecutionHistory(ChannelExtension, 0xff08);//Stop Failed
        return FALSE;    //I can't stop this train
    }

  //2. Undefine the Uncached Extension

    AtaPortWriteRegisterUlong(&ChannelExtension->Px->IE.AsUlong, 0);    //disabling interrupts
    AtaPortWriteRegisterUlong(&ChannelExtension->Px->IS.AsUlong, (ULONG)~0);  
    AtaPortWriteRegisterUlong(ChannelExtension->ControllerIS, (1 << ChannelExtension->ChannelNumber));
  
    ChannelExtension->CommandList = 0;                                  //undoing AhciHwControlIdeStart section 4.2
    ChannelExtension->CommandListPhysicalAddress.QuadPart = 0;

    AtaPortWriteRegisterUlong(&ChannelExtension->Px->CLB.AsUlong, 0);   //undoing AhciHwControlIdeStart section 3.1.1
    AtaPortWriteRegisterUlong(&ChannelExtension->Px->CLBU, (ULONG)0);

    ChannelExtension->ReceivedFIS = 0;                                  //undoing AhciHwControlIdeStart section 4.3
    ChannelExtension->ReceivedFisPhysicalAddress.QuadPart = 0;

    AtaPortWriteRegisterUlong(&ChannelExtension->Px->FB.AsUlong, 0);    //undoing AhciHwControlIdeStart section 3.1.2
    AtaPortWriteRegisterUlong(&ChannelExtension->Px->FBU, 0);
  
    ChannelExtension->Local.IrbExtension = NULL;                        //undoing AhciHwControlIdeStart section 4.4
    ChannelExtension->Local.IrbExtensionPhysicalAddress.QuadPart = 0;
    ChannelExtension->Local.IrbDataBuffer = NULL;
    ChannelExtension->Local.IrbDataBufferPhysicalAddress.QuadPart = 0;

    ChannelExtension->GesnCachedBuffer = NULL;                          //undoing AhciHwControlIdeStart section 4.5
    ChannelExtension->GesnCachedBufferPhysicalAddress.QuadPart = 0;

    ChannelExtension->Px = 0;                                           //undoing AhciHwControlIdeStart section 1.1
    ChannelExtension->ControllerIS = 0;

    ChannelExtension->StateFlags.StartCapable = FALSE;
    RecordExecutionHistory(ChannelExtension, 0x10000025);//Exit AhciHwControlIdeStop

    return TRUE;
}

BOOLEAN 
  AhciHwControlIdePowerUp(
    IN PAHCI_CHANNEL_EXTENSION ChannelExtension
    )
{
/*++
    Indicates that the channel is being powered up.
It assumes:
    Ataport calls AhciAdapterControlIdePowerUp before it calls AhciHwControlIdePowerUp
    ATAport calls AhciHwControlIdePowerUp once per channel
    ATAport starts up the device by sending an IRB_FUNCTION_POWER_CHANGE before starting IO.
Called by:    
    AhciHwControl

It performs:
    (overview)
    1. Start the Port. 
    2. If APM is supported, make sure the Link is Active as defined in AHCI1.0 8.3.1.2.   
    3. If Port Multiplier is supported, powered it up.
    (details)
    1.1 Reload the CLB,CBU,FLB,FBU stored in the channel extension
    1.2 Reload PxIE
    1.3 Reinitialize the StateFlags
    1.4 Start the channel
Affected Variables/Registers:
    PxCMD.ST, PxCMD.ICC
    PxCLB,PxCLBU,PxFB,PxFBU
    PxIE
Return Values:
    TRUE if the function excecuted completely.
    FALSE if the channel could not be started for Power Up.  
    Neither return value is used by ATAport.
--*/

    USER_LPM_POWER_SETTINGS userLpmSettings;

#ifdef ENABLE_HISTORY_LOG
    RecordExecutionHistory(ChannelExtension, 0x00000026);//AhciHwControlIdePowerUp
#endif

  //1.1 Reload the CLB,CBU,FLB,FBU
    AtaPortWriteRegisterUlong(&ChannelExtension->Px->CLB.AsUlong, ChannelExtension->CommandListPhysicalAddress.LowPart);
    if (ChannelExtension->CAP.S64A) { //If the controller supports 64 bits, write high part
        AtaPortWriteRegisterUlong(&ChannelExtension->Px->CLBU, ChannelExtension->CommandListPhysicalAddress.HighPart);
    }
    AtaPortWriteRegisterUlong(&ChannelExtension->Px->FB.AsUlong, ChannelExtension->ReceivedFisPhysicalAddress.LowPart);
    if (ChannelExtension->CAP.S64A) { //If the controller supports 64 bits, write high part
        AtaPortWriteRegisterUlong(&ChannelExtension->Px->FBU, ChannelExtension->ReceivedFisPhysicalAddress.HighPart);
    }
  //1.2 Reload PxIE
    Set_PxIE(ChannelExtension, &ChannelExtension->Px->IE);
  //1.3 Reinitialize the StateFlags
    ChannelExtension->StateFlags.StartCapable = TRUE;

  //1.4.1 Restore Perserved Settings
    if (ChannelExtension->StateFlags.NeedToSetTransferMode == TRUE) {
        RestorePreservedSettings(ChannelExtension);
    }

  //1.4.2 Restore LPM settings in case of hardware loose it during power down.
    userLpmSettings.AsUlong = (ULONG)ChannelExtension->LastUserLpmPowerSetting;
    AhciLpmSettingsModes(ChannelExtension, userLpmSettings);

  //1.5 Start the channel
    P_Running_StartAttempt(ChannelExtension);

    RecordExecutionHistory(ChannelExtension, 0x10000026);//Exit AhciHwControlIdePowerUp
    return TRUE;
}

BOOLEAN 
  AhciHwControlIdePowerDown(
    IN PAHCI_CHANNEL_EXTENSION ChannelExtension
    )
{
/*++
Indicates that the channel is being powered down.

It assumes:
    ATAport has already powered down the device first through ATA commands
    All outstanding IO will be complete before the first power request is sent to the miniport
Called by:    
    AhciHwControl

It performs:
    Then each port must be stopped. PxCMD.ST
    If APM is supported, the Link need to be put into Slumber as defined in AHCI 1.1 Section 8.3.1.2
    If Port Multiplier is support, it would need to be powered down next.
Affected Variables/Registers:
    none
Return Values:
    TRUE if the function excecuted completely.
    FALSE if the channel could not be stopped for Power Down.  
    Neither return value is used by ATAport.
--*/

#ifdef ENABLE_HISTORY_LOG
    RecordExecutionHistory(ChannelExtension, 0x00000027);//AhciHwControlIdePowerDown
#endif

    ChannelExtension->StateFlags.StartCapable = FALSE;
    RecordExecutionHistory(ChannelExtension, 0x10000027);//Exit AhciHwControlIdePowerDown
    return TRUE;
}

__inline 
BOOLEAN
IsDeviceSupportsHIPM(
    __in PIDENTIFY_DEVICE_DATA IdentifyData
    )
{
    return (IdentifyData->ReservedWords76[0] & (1 << 9)) > 0;
}

__inline 
BOOLEAN
IsDeviceSupportsDIPM(
    __in PIDENTIFY_DEVICE_DATA IdentifyData
    )
{
    return (IdentifyData->ReservedWords76[2] & (1 << 3)) > 0;
}


__inline 
BOOLEAN
IsLpmModeSetting(
    __in PIDE_VENDOR_DEFINED_POWER_INFO PowerInfo
    )
{
    if (PowerInfo->SettingGuid.Data1 == 0x0b2d69d7) {
        if (PowerInfo->SettingGuid.Data2 == 0xa2a1){
            if (PowerInfo->SettingGuid.Data3 == 0x449c){
                if (PowerInfo->SettingGuid.Data4[0] == 0x96){
                    if (PowerInfo->SettingGuid.Data4[1] == 0x80){
                        if (PowerInfo->SettingGuid.Data4[2] == 0xf9){
                            if (PowerInfo->SettingGuid.Data4[3] == 0x1c){
                                if (PowerInfo->SettingGuid.Data4[4] == 0x70){
                                    if (PowerInfo->SettingGuid.Data4[5] == 0x52){
                                        if (PowerInfo->SettingGuid.Data4[6] == 0x1c){
                                            if (PowerInfo->SettingGuid.Data4[7] == 0x60){
                                                return TRUE;
                                            }  }  }  }  }  }  } }  }  }  }

    return FALSE;
}

__inline 
BOOLEAN
IsLpmAdaptiveSetting(
    __in PIDE_VENDOR_DEFINED_POWER_INFO PowerInfo
    )
{
    if (PowerInfo->SettingGuid.Data1 == 0xDAB60367) {
        if (PowerInfo->SettingGuid.Data2 == 0x53FE){
            if (PowerInfo->SettingGuid.Data3 == 0x4fbc){
                if (PowerInfo->SettingGuid.Data4[0] == 0x82){
                    if (PowerInfo->SettingGuid.Data4[1] == 0x5E){
                        if (PowerInfo->SettingGuid.Data4[2] == 0x52){
                            if (PowerInfo->SettingGuid.Data4[3] == 0x1D){
                                if (PowerInfo->SettingGuid.Data4[4] == 0x06){
                                    if (PowerInfo->SettingGuid.Data4[5] == 0x9D){
                                        if (PowerInfo->SettingGuid.Data4[6] == 0x24){
                                            if (PowerInfo->SettingGuid.Data4[7] == 0x56){
                                                return TRUE;
                                            }  }  }  }  }  }  } }  }  }  }

    return FALSE;
}

UCHAR
SetAllowedLpmStates(
    __in PAHCI_CHANNEL_EXTENSION ChannelExtension
    )
/*
    Return Value: disabled modes; 
*/
{
    UCHAR lpm;
    AHCI_SERIAL_ATA_CONTROL sctl;

    // 0h  No interface restrictions 
    // 1h  Transitions to the Partial state disabled 
    // 2h  Transitions to the Slumber state disabled 
    // 3h  Transitions to both Partial and Slumber states disabled 
    // disable LPM for eSATA port as hot-plug cannot be detected in partial or slumber state.
    if ((ChannelExtension->LastUserLpmPowerSetting == 0) ||
        IsExternalPort(ChannelExtension)) {
        lpm = 0x03; // slumber and partial disallowed
    } else {
        AHCI_COMMAND cmd;
        cmd.AsUlong = AtaPortReadRegisterUlong(&ChannelExtension->Px->CMD.AsUlong);

        if ((ChannelExtension->AutoPartialToSlumberInterval == 0) &&    // No software auto partial to slumber
            ( (ChannelExtension->CAP2.APST == 0) ||                     // device auto Partial to Slumber is not supported.
              (cmd.APSTE == 0) )) {                                     // device auto Partial to Slumber is not enabled.
            lpm = 0x02; // partial allowed; slumber disallowed
        } else {
            lpm = 0x00; // partial allowed; slumber allowed
        }

    }

    if (ChannelExtension->CAP.SSC == 0) {
        // disable Slumber if controller does not support it.
        lpm |= 0x02;
    }

    if (ChannelExtension->CAP.PSC == 0) {
        // msachi LPM is to put device into partial, then transit into slumber according to defined interval value.
        // do not enable LPM if partial is not supported. 
        // the case of device supporting slumber but not partial is very rare.
        lpm = 0x03;
    }

    //Set PxSCTL.IPM to 3h to restrict slumber and partial interface power management state transitions.
    sctl.AsUlong = AtaPortReadRegisterUlong(&ChannelExtension->Px->SCTL.AsUlong);
    sctl.IPM = lpm; 
    AtaPortWriteRegisterUlong(&ChannelExtension->Px->SCTL.AsUlong, sctl.AsUlong);

    return lpm; 
}

VOID
AhciLpmSettingsModes(
    __in PAHCI_CHANNEL_EXTENSION ChannelExtension,
    __in USER_LPM_POWER_SETTINGS LpmMode
    )
{
    AHCI_COMMAND cmd;

    ChannelExtension->LastUserLpmPowerSetting = (UCHAR)LpmMode.AsUlong;
    
    if (LpmMode.AsUlong == 0) {
        // Active Mode.
        //Turn LPM off is Active is chosen
        cmd.AsUlong = AtaPortReadRegisterUlong(&ChannelExtension->Px->CMD.AsUlong);
        cmd.ALPE = 0;
        cmd.ASP = 0;
        AtaPortWriteRegisterUlong(&ChannelExtension->Px->CMD.AsUlong, cmd.AsUlong);

        //Set PxSCTL.IPM to 3h to restrict slumber and partial interface power management state transitions.
        SetAllowedLpmStates(ChannelExtension);

        // Disable DIPM
        if (IsDeviceSupportsDIPM(&ChannelExtension->DeviceExtension[0].IdentifyData)) {
            //The enable/disable state for device initiated power management shall persist across software reset.
            //The enable/disable state shall be reset to its default disabled state upon COMRESET.
            ReplaceSetFeatures(ChannelExtension,
                               IDE_FEATURE_ENABLE_SATA_FEATURE,
                               IDE_FEATURE_DISABLE_SATA_FEATURE,
                               IDE_SATA_FEATURE_DEVICE_INITIATED_POWER_MANAGEMENT,
                               IDE_SATA_FEATURE_DEVICE_INITIATED_POWER_MANAGEMENT);

            AddSetFeatures(ChannelExtension,
                           IDE_FEATURE_DISABLE_SATA_FEATURE,
                           IDE_SATA_FEATURE_DEVICE_INITIATED_POWER_MANAGEMENT);

            //Configure device with persistent configuration commands
            RestorePreservedSettings(ChannelExtension);
        }

    } else {
        // link power management is allowed.

        //Set PxSCTL.IPM for LPM allowed states.
        UCHAR sctlIpm = SetAllowedLpmStates(ChannelExtension);

        // Setting HIPM if it's enabled.
        if (LpmMode.HipmEnabled > 0) {
            // If Partial is capable and device supports HIPM.
            if ((sctlIpm != 0x03) &&
                IsDeviceSupportsHIPM(&ChannelExtension->DeviceExtension[0].IdentifyData)) {

                // Turn on LPM and set it for Partial
                cmd.AsUlong = AtaPortReadRegisterUlong(&ChannelExtension->Px->CMD.AsUlong);
                cmd.ALPE = 1;
                cmd.ASP = 0; //0 = partial, 1 = slumber
                AtaPortWriteRegisterUlong(&ChannelExtension->Px->CMD.AsUlong, cmd.AsUlong);
            }
        }

        // Setting DIPM if it's enabled.
        if ((LpmMode.DipmEnabled > 0) &&
            (sctlIpm != 0x03)) {
            // Enable DIPM feature.
            if (IsDeviceSupportsDIPM(&ChannelExtension->DeviceExtension[0].IdentifyData)) {

                //The enable/disable state for device initiated power management shall persist across software reset.
                //The enable/disable state shall be reset to its default disabled state upon COMRESET.
                ReplaceSetFeatures(ChannelExtension,
                                   IDE_FEATURE_DISABLE_SATA_FEATURE,
                                   IDE_FEATURE_ENABLE_SATA_FEATURE,
                                   IDE_SATA_FEATURE_DEVICE_INITIATED_POWER_MANAGEMENT,
                                   IDE_SATA_FEATURE_DEVICE_INITIATED_POWER_MANAGEMENT);

                AddSetFeatures(ChannelExtension,
                               IDE_FEATURE_ENABLE_SATA_FEATURE,
                               IDE_SATA_FEATURE_DEVICE_INITIATED_POWER_MANAGEMENT);

                //Configure device with persistent configuration commands
                RestorePreservedSettings(ChannelExtension);

            }

        } else {
            // Disable DIPM feature.
            if (IsDeviceSupportsDIPM(&ChannelExtension->DeviceExtension[0].IdentifyData)) {

                //The enable/disable state for device initiated power management shall persist across software reset.
                //The enable/disable state shall be reset to its default disabled state upon COMRESET.
                ReplaceSetFeatures(ChannelExtension,
                                   IDE_FEATURE_ENABLE_SATA_FEATURE,
                                   IDE_FEATURE_DISABLE_SATA_FEATURE,
                                   IDE_SATA_FEATURE_DEVICE_INITIATED_POWER_MANAGEMENT,
                                   IDE_SATA_FEATURE_DEVICE_INITIATED_POWER_MANAGEMENT);

                AddSetFeatures(ChannelExtension,
                               IDE_FEATURE_DISABLE_SATA_FEATURE,
                               IDE_SATA_FEATURE_DEVICE_INITIATED_POWER_MANAGEMENT);

                //Configure device with persistent configuration commands
                RestorePreservedSettings(ChannelExtension);

            }
        }

    }

    return;
}


BOOLEAN
  AhciHwControlVendorDefined(
    IN PAHCI_CHANNEL_EXTENSION ChannelExtension,
    IN PIDE_VENDOR_DEFINED_POWER_INFO PowerInfo
    )
{
    //Make sure the Controller supports LPM, otherwise, don't touch anything.
    if (ChannelExtension->CAP.SALP == 0) {
        return FALSE;
    }

    // Validate input LPM data from the Power Manager
    if (PowerInfo->ValueLength != 4) {
        return FALSE;
    }

    if (!IsLpmModeSetting(PowerInfo) && 
        !IsLpmAdaptiveSetting(PowerInfo)) {
        // invalid power policy.
        return FALSE;
    }

    if (IsLpmAdaptiveSetting(PowerInfo)) {
        // max allowed value: 5 minutes (in ms)
        ULONG interval = (ULONG)*((PULONG)PowerInfo->Value);

        if (interval <= 300000) {
            ChannelExtension->AutoPartialToSlumberInterval = interval;

            //Set PxSCTL.IPM for LPM allowed states.
            SetAllowedLpmStates(ChannelExtension);
        }

    } else if (IsLpmModeSetting(PowerInfo)) {
        USER_LPM_POWER_SETTINGS userLpmPowerSettings;
        userLpmPowerSettings.AsUlong = (ULONG)*((PULONG)PowerInfo->Value);

        AhciLpmSettingsModes(ChannelExtension, userLpmPowerSettings);

    }

    return TRUE;
}

VOID
AhciAutoPartialToSlumber(
    PAHCI_CHANNEL_EXTENSION ChannelExtension
)
{
    AHCI_SERIAL_ATA_STATUS ssts;
    AHCI_COMMAND cmd;

    cmd.AsUlong = AtaPortReadRegisterUlong(&ChannelExtension->Px->CMD.AsUlong);

    // 1.1 check the Link Power State should be enabled.
    if ((ChannelExtension->CAP.SALP == 0) ||    // not support Aggressive LPM
        (cmd.ALPE == 0)) {                      // Aggressive LPM is disabled
        return;
    }

    // 1.2 check if the device is able to automatically go to Slumber from Partial.
    if ((ChannelExtension->CAP2.APST == 1) &&   // device auto Partial to Slumber is supported.
        (cmd.APSTE == 1)) {                     // device auto Partial to Slumber is enabled.
        return;
    }

    ssts.AsUlong = AtaPortReadRegisterUlong(&ChannelExtension->Px->SSTS.AsUlong);

    // 1.3 check the Link Power State, should be Partial (value 2).
    if (ssts.IPM != 2) {
        return;
    }

    // 2. Change LPM State.
    // link should be in idle state.
    if (cmd.ICC == 0) {
        // 2.1 set into Active state.
        cmd.ICC = 1;
        AtaPortWriteRegisterUlong(&ChannelExtension->Px->CMD.AsUlong, cmd.AsUlong);

        // 2.2 set into Slumber state.
        cmd.AsUlong = AtaPortReadRegisterUlong(&ChannelExtension->Px->CMD.AsUlong);
        if (cmd.ICC != 0) {
            //by spec, partial to active transition should be completed in 10us. Reading registry already takes sometime.
            AtaPortStallExecution(10);  //10 microseconds
        }

        //try to bring it into slumber state.
        cmd.ICC = 6;
        AtaPortWriteRegisterUlong(&ChannelExtension->Px->CMD.AsUlong, cmd.AsUlong);


    }

    return;
}

