/*++

Copyright (C) Microsoft Corporation, 2004

Module Name:

    pwrmgmt.h

Abstract:



Authors:

    Nathan Obr (natobr),  February 2005

--*/

#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning(disable:4214) // bit field types other than int
#pragma warning(disable:4201) // nameless struct/union

typedef union _USER_LPM_POWER_SETTINGS {
    struct {
    //LSB
    ULONG HipmEnabled: 1;
    ULONG DipmEnabled: 1;
    ULONG Reserved: 30;
    //MSB
    };

    ULONG AsUlong;

} USER_LPM_POWER_SETTINGS, *PUSER_LPM_POWER_SETTINGS;

BOOLEAN 
  AhciAdapterControlIdeStart(
    IN PAHCI_CONTROLLER_EXTENSION ControllerExtension, 
	IN PIDE_CONTROLLER_CONFIGURATION ControllerConfiguration
	);
BOOLEAN 
  AhciAdapterControlIdeStop(
	IN PAHCI_CONTROLLER_EXTENSION ControllerExtension
	);
BOOLEAN 
  AhciAdapterControlIdePowerUp(
	IN PAHCI_CONTROLLER_EXTENSION ControllerExtension
	);
BOOLEAN 
  AhciAdapterControlIdePowerDown(
	IN PAHCI_CONTROLLER_EXTENSION ControllerExtension
	);
BOOLEAN 
  AhciHwControlIdeStart(
	IN PAHCI_CHANNEL_EXTENSION ChannelExtension, 
	IN PIDE_CHANNEL_CONFIGURATION ChannelConfiguration
	);
BOOLEAN 
  AhciHwControlIdeStop(
	IN PAHCI_CHANNEL_EXTENSION ChannelExtension
	);
BOOLEAN 
  AhciHwControlIdePowerUp(
	IN PAHCI_CHANNEL_EXTENSION ChannelExtension
	);
BOOLEAN 
  AhciHwControlIdePowerDown(
	IN PAHCI_CHANNEL_EXTENSION ChannelExtension
	);

BOOLEAN
  AhciHwControlVendorDefined(
    IN PAHCI_CHANNEL_EXTENSION ChannelExtension,
    IN PIDE_VENDOR_DEFINED_POWER_INFO PowerInfo
    );

VOID
  AhciAutoPartialToSlumber(
	PAHCI_CHANNEL_EXTENSION ChannelExtension
  );


#if _MSC_VER >= 1200
#pragma warning(pop)
#else
#pragma warning(default:4214)
#pragma warning(default:4201)
#endif

