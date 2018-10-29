/*++

Copyright (C) Microsoft Corporation, 2004

Module Name:

    hbastat.h

Abstract:



Notes:

Revision History:

		Nathan Obr (natobr),  February 2005
--*/
#pragma once

BOOLEAN
  AhciControllerReset( 
	IN PAHCI_CONTROLLER_EXTENSION ControllerExtension
	);
BOOLEAN
  AhciPortReset(
	IN PAHCI_CHANNEL_EXTENSION ChannelExtension
  );

VOID
  AhciNonQueuedErrorRecovery(
	PAHCI_CHANNEL_EXTENSION ChannelExtension
  );

VOID
  AhciNativeCommandQueueingErrorRecovery(
	PAHCI_CHANNEL_EXTENSION ChannelExtension
  );
VOID
  WorkerDispatch(
	PAHCI_CHANNEL_EXTENSION ChannelExtension
  );

VOID
  AhciCOMRESET(
	PAHCI_CHANNEL_EXTENSION ChannelExtension,
	PAHCI_PORT Px
	);
BOOLEAN
  P_NotRunning(
    PAHCI_CHANNEL_EXTENSION ChannelExtension,
	PAHCI_PORT Px,
    BOOLEAN InReset
	);
BOOLEAN
  P_Running(
	PAHCI_CHANNEL_EXTENSION ChannelExtension
	);
VOID
  P_Running_WaitOnDET(
	PAHCI_CHANNEL_EXTENSION ChannelExtension
	);
VOID
  P_Running_WaitWhileDET1(
	PAHCI_CHANNEL_EXTENSION ChannelExtension
	);
VOID
  P_Running_WaitOnDET3(
	PAHCI_CHANNEL_EXTENSION ChannelExtension
	);
VOID
  P_Running_WaitOnFRE(
	PAHCI_CHANNEL_EXTENSION ChannelExtension
	);
VOID
  P_Running_StartFailed(
    PAHCI_CHANNEL_EXTENSION ChannelExtension
    );
VOID
  P_Running_StartAttempt(
    PAHCI_CHANNEL_EXTENSION ChannelExtension
    );
VOID
  P_Running_WaitOnBSYDRQ(
	PAHCI_CHANNEL_EXTENSION ChannelExtension
	);
