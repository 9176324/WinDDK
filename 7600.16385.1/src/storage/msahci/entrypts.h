/*++

Copyright (C) Microsoft Corporation, 2004

Module Name:

    entrypts.h

Abstract:

    This header file contains function definitions of entry points to the AHCI ATA miniport.  
	This includes WDM entry points and ATA miniport interface functions.

Authors:

    Nathan Obr (natobr),  February 2005

--*/

__inline 
BOOLEAN
IsExternalPort(
    __in PAHCI_CHANNEL_EXTENSION ChannelExtension
    )
{
    if (!ChannelExtension->StateFlags.TreatAsInternalPort) {
        if ((ChannelExtension->Px->CMD.HPCP) || 
            ((ChannelExtension->CAP.SXS) && (ChannelExtension->Px->CMD.ESP))) {
            //Px->CMD.HPCP indicates that the port is hot-pluggable. (both signal and power cable)
            //CAP.SXS && Px->CMD.ESP indicates that it's an ESATA port. (only signal cable)
            return TRUE;
        }
    } 
    
    return FALSE;
}

//NTSTATUS 
//  DriverEntry ( 
//    IN PDRIVER_OBJECT  DriverObject, 
//    IN PUNICODE_STRING  RegistryPath 
//    ); 
BOOLEAN
  AhciChannelInitRoutine(
	IN PVOID ChannelExtension,
	OUT PIDE_CHANNEL_INTERFACE ChannelConfig,
	IN PVOID InitContext
	);
ATA_CHANNEL_STATE
  AhciChannelEnabled(
	IN PVOID ControllerExtension,
    IN ULONG Channel
    );
/*BOOLEAN  
  AhciChannelTransferModeSelect (
	IN PVOID ControllerExtension,
    IN PIDE_TRANSFER_MODE_PARAMETERS TransferModeParams
	);*/
BOOLEAN  
  AhciAdapterControl(
    PVOID ControllerExtension,
    IN IDE_CONTROL_ACTION ControlAction,
    IN PVOID Parameters
    );
BOOLEAN
  AhciHwInitialize (
    IN PVOID ChannelExtension,
    IN OUT PIDE_DEVICE_PARAMETERS DeviceParameters,
    IN PIDENTIFY_DEVICE_DATA IdentifyData
    );
BOOLEAN
  AhciHwBuildIo (
    IN PVOID ChannelExtension,
    IN PIDE_REQUEST_BLOCK Irb
    );
BOOLEAN
  AhciHwStartIo (
    IN PVOID ChannelExtension,
    IN PIDE_REQUEST_BLOCK Irb
    );
BOOLEAN
  ActivateQueue(
	PAHCI_CHANNEL_EXTENSION ChannelExtension
	);
BOOLEAN
  AhciHwInterrupt (
    IN PVOID ChannelExtension
    );
BOOLEAN
  AhciHwReset (
    IN PVOID ChannelExtension
    );
BOOLEAN
  AhciHwControl (
    IN PVOID ChannelExtension,
	IN IDE_CONTROL_ACTION ControlAction,
    IN PVOID Parameters
    );

