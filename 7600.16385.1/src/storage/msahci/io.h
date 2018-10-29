/*++

Copyright (C) Microsoft Corporation, 2005

Module Name:

    io.h

Abstract:



Notes:

Revision History:

		Nathan Obr (natobr),  February 2005
--*/
VOID
  IRBtoATA_CFIS(
	PAHCI_CHANNEL_EXTENSION ChannelExtension,
	PSLOT_CONTENT SlotContent
  );

VOID
  IRBtoATAPI_CFIS(
	PAHCI_CHANNEL_EXTENSION ChannelExtension,
	PSLOT_CONTENT SlotContent
  );

VOID
  NonDataCMDtoATA_CFIS(
	PAHCI_CHANNEL_EXTENSION ChannelExtension,
	PSLOT_CONTENT SlotContent,
  	UCHAR Cmd
  );

ULONG
  IRBtoPRDT(
	IN PVOID ChannelExtension,
	PSLOT_CONTENT SlotContent
  );

VOID
  IRBtoCmdHeader(
	IN PVOID ChannelExtension,
	PSLOT_CONTENT SlotContent,
	ULONG Length,
	BOOLEAN Reset
  );
VOID
  AhciCompleteIssuedIRBs(
	PAHCI_CHANNEL_EXTENSION ChannelExtension,
	UCHAR IrbStatus
  );
