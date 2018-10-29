/*++

Copyright (C) Microsoft Corporation, 2005

Module Name:

    util.h

Abstract:



Notes:

Revision History:

        Nathan Obr (natobr),  February 2005
--*/

VOID
  AhciReportBusChange(
    PAHCI_CHANNEL_EXTENSION ChannelExtension
    );

VOID
  GetTag(
    PAHCI_CHANNEL_EXTENSION ChannelExtension,
    PIDE_REQUEST_BLOCK Irb
    );

UCHAR
  GetSingleIo(
    PAHCI_CHANNEL_EXTENSION ChannelExtension
    );

VOID
  Set_PxIE(
    PAHCI_CHANNEL_EXTENSION ChannelExtension,
    PAHCI_INTERRUPT_ENABLE IE
    );

BOOLEAN
  IsFPDMACommand (
    PIDE_REQUEST_BLOCK Irb
    );

BOOLEAN
  IsGESNCommand (
    PIDE_REQUEST_BLOCK Irb
    );

BOOLEAN
  IsNormalCommand (
    PIDE_REQUEST_BLOCK Irb
    );

BOOLEAN
  RequiresControllerResources (
    PAHCI_CHANNEL_EXTENSION ChannelExtension,
    PIDE_REQUEST_BLOCK Irb
    );

VOID
  ReleaseSlottedCommand(
    PAHCI_CHANNEL_EXTENSION ChannelExtension,
    UCHAR SlotNumber
    );

VOID
  RecordExecutionHistory(
    PAHCI_CHANNEL_EXTENSION ChannelExtension,
    ULONG Function
  );


VOID
  AhciCompleteRequest(
    PAHCI_CHANNEL_EXTENSION ChannelExtension,
    PIDE_REQUEST_BLOCK Irb
    );

VOID
  AddSetFeatures(
    PAHCI_CHANNEL_EXTENSION ChannelExtension,
    UCHAR Features,
    UCHAR SectorCount
  );

BOOLEAN
  ReplaceSetFeatures(
    PAHCI_CHANNEL_EXTENSION ChannelExtension,
    UCHAR OldFeatures,
    UCHAR NewFeatures,
    UCHAR OldSectorCount,
    UCHAR NewSectorCount
  );

VOID
  RestorePreservedSettings(
    PAHCI_CHANNEL_EXTENSION ChannelExtension
  );

VOID
  IssueSetFeatures(
    PVOID ChannelExtension
  );

VOID
  IssueSetFeaturesIrb(
    PAHCI_CHANNEL_EXTENSION ChannelExtension,
    UCHAR Features,
    UCHAR SectorCount
    );

VOID
  EnableHotPlug(
    PAHCI_CHANNEL_EXTENSION ChannelExtension
    );

PAHCI_MEMORY_REGISTERS
  GetABARAddress(
    PVOID DeviceExtension,
    PIDE_MINIPORT_RESOURCES MiniportResources
    );

BOOLEAN
  NeedToSetTransferMode(
    PAHCI_CHANNEL_EXTENSION ChannelExtension
    ); 

