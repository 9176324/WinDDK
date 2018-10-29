/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    ap_config.h

Abstract:
    ExtAP configuration definitions
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    09-10-2007    Created
    
Notes:

--*/
#pragma once
    
#ifndef _AP_CONFIG_H
#define _AP_CONFIG_H

/** Forward declarations */
typedef struct _MP_EXTAP_PORT      MP_EXTAP_PORT, *PMP_EXTAP_PORT;

/**
 * Holds current ExtAP configuration of the miniport that
 * are not managed by association manager. 
 * These configurations can be updated and/or queried via OIDs request from the OS. 
 * A lock is NOT needed when updating/querying these configurations.
 * This data is stateless so we don't need a flag to indicate whether it is initialized or not.
 */

#define AP_DESIRED_PHY_MAX_COUNT    1

typedef struct _AP_CONFIG 
{
    /** ExtAP port */
    PMP_EXTAP_PORT          ApPort;

    /** The types of auto configuration for 802.11 parameters that are enabled */
    ULONG                   AutoConfigEnabled;
    
    /** Beacon period, in TUs */
    ULONG                   BeaconPeriod;

    /** DTIM period, in beacon intervals */
    ULONG                   DTimPeriod;
#if 0       // remove it after confirmed
    /** RTS threshold */
    ULONG                   RtsThreshold;

    /** Short retry limit */
    ULONG                   ShortRetryLimit;

    /** Long retry limit */
    ULONG                   LongRetryLimit;

    /** Fragmentation threshold */
    ULONG                   FragmentationThreshold;

    /** Current operating frequency channel list for the DSSS/HRDSSS/ERP PHY */
    ULONG                   CurrentChannel;
    
    /** Current operating frequency channel list for the OFDM PHY */
    ULONG                   CurrentFrequency;
    
    /** Current PHY ID */
    ULONG                   CurrentPhyId;
    
#endif
    
    /** Default key ID */
    ULONG                   CipherDefaultKeyId;
    
    /** Desired PHY ID list */
    ULONG                   DesiredPhyList[AP_DESIRED_PHY_MAX_COUNT];
    ULONG                   DesiredPhyCount;

    /** Additonal IEs for beacon */
    ULONG                   AdditionalBeaconIESize;
    PVOID                   AdditionalBeaconIEData;

    /** Additonal IEs for probe response */
    ULONG                   AdditionalResponseIESize;
    PVOID                   AdditionalResponseIEData;

    /** Saved copy of beacon IEs for association completion **/
    ULONG                   ApBeaconIESize;
    PVOID                   ApBeaconIEData;

}AP_CONFIG, *PAP_CONFIG;

/** AP configurations */

/** Initialize AP configurations */
NDIS_STATUS
ApInitializeConfig(
    __in PAP_CONFIG ApConfig,
    __in PMP_EXTAP_PORT ApPort
    );

/** Deinitialize AP configurations */
VOID
ApDeinitializeConfig(
    __in PAP_CONFIG ApConfig
    );

/** Internal functions that are invoked by other configuration functions */

/** 
 * Set AP configuration to its default based on the hardware capability 
 * and registry settings
 */
VOID
CfgSetDefault(
    __in PAP_CONFIG ApConfig
    );

/** Clean up AP configuration */
VOID
CfgCleanup(
    __in PAP_CONFIG ApConfig
    );

/**
 * Internal functions for OIDs
 */
VOID
CfgQueryAutoConfigEnabled(
    __in PAP_CONFIG Config,
    __out PULONG AutoConfigEnabled
    );

#define ALLOWED_AUTO_CONFIG_FLAGS (DOT11_PHY_AUTO_CONFIG_ENABLED_FLAG | DOT11_MAC_AUTO_CONFIG_ENABLED_FLAG)

NDIS_STATUS
CfgSetAutoConfigEnabled(
    __in PAP_CONFIG Config,
    __in ULONG AutoConfigEnabled
    );
  
VOID
FORCEINLINE
CfgQueryBeaconPeriod(
    __in PAP_CONFIG Config,
    __out PULONG BeaconPeriod
    )
{
    *BeaconPeriod = Config->BeaconPeriod;
}

NDIS_STATUS
CfgSetBeaconPeriod(
    __in PAP_CONFIG Config,
    __in_range(1, 65535) ULONG BeaconPeriod
    );
    
VOID
FORCEINLINE
CfgQueryDTimPeriod(
    __in PAP_CONFIG Config,
    __out PULONG DTimPeriod
    )
{
    *DTimPeriod = Config->DTimPeriod;
}

NDIS_STATUS
CfgSetDTimPeriod(
    __in PAP_CONFIG Config,
    __in_range(1, 255) ULONG DTimPeriod
    );

#if 0
VOID
FORCEINLINE
CfgQueryRtsThreshold(
    __in PAP_CONFIG Config,
    __out PULONG RtsThreshold
    )
{
    *RtsThreshold = Config->RtsThreshold;
}

NDIS_STATUS
CfgSetRtsThreshold(
    __in PAP_CONFIG Config,
    __in_range(0, 2347) ULONG RtsThreshold
    );

VOID
FORCEINLINE
CfgQueryShortRetryLimit(
    __in PAP_CONFIG Config,
    __out PULONG ShortRetryLimit
    )
{
    *ShortRetryLimit = Config->ShortRetryLimit;
}

NDIS_STATUS
CfgSetShortRetryLimit(
    __in PAP_CONFIG Config,
    __in_range(1, 255) ULONG ShortRetryLimit
    );

VOID
FORCEINLINE
CfgQueryLongRetryLimit(
    __in PAP_CONFIG Config,
    __out PULONG LongRetryLimit
    )
{
    *LongRetryLimit = Config->LongRetryLimit;
}

NDIS_STATUS
CfgSetLongRetryLimit(
    __in PAP_CONFIG Config,
    __in_range(1, 255) ULONG LongRetryLimit
    );

VOID
FORCEINLINE
CfgQueryFragmentationThreshold(
    __in PAP_CONFIG Config,
    __out PULONG FragmentationThreshold
    )
{
    *FragmentationThreshold = Config->FragmentationThreshold;
}

NDIS_STATUS
CfgSetFragmentationThreshold(
    __in PAP_CONFIG Config,
    __in_range(256, 2346) ULONG FragmentationThreshold
    );
#endif

VOID
CfgQueryCurrentChannel(
    __in PAP_CONFIG Config,
    __out PULONG CurrentChannel
    );

NDIS_STATUS
CfgSetCurrentChannel(
    __in PAP_CONFIG Config,
    __in_range(1, 14) ULONG CurrentChannel
    );

VOID
CfgQueryCurrentFrequency(
    __in PAP_CONFIG Config,
    __out PULONG CurrentFrequency
    );

NDIS_STATUS
CfgSetCurrentFrequency(
    __in PAP_CONFIG Config,
    __in_range(0, 200) ULONG CurrentFrequency
    );

VOID
FORCEINLINE
CfgQueryCipherDefaultKeyId(
    __in PAP_CONFIG Config,
    __out PULONG CipherDefaultKeyId
    )
{
    *CipherDefaultKeyId = Config->CipherDefaultKeyId;
}

NDIS_STATUS
CfgSetCipherDefaultKeyId(
    __in PAP_CONFIG Config,
    __in ULONG CipherDefaultKeyId
    );

NDIS_STATUS
CfgQueryDesiredPhyList(
    __in PAP_CONFIG Config,
    __out PDOT11_PHY_ID_LIST DesiredPhyList,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

NDIS_STATUS
CfgSetDesiredPhyList(
    __in PAP_CONFIG Config,
    __in PDOT11_PHY_ID_LIST DesiredPhyList
    );

VOID
CfgQueryCurrentPhyId(
    __in PAP_CONFIG Config,
    __out PULONG CurrentPhyId
    );

NDIS_STATUS
CfgSetCurrentPhyId(
    __in PAP_CONFIG Config,
    __in ULONG CurrentPhyId
    );

NDIS_STATUS
CfgQueryAdditionalIe(
    __in PAP_CONFIG Config,
    __out PDOT11_ADDITIONAL_IE AdditionalIe,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

NDIS_STATUS
CfgSetAdditionalIe(
    __in PAP_CONFIG Config,
    __in PDOT11_ADDITIONAL_IE AdditionalIe
    );

VOID
CfgResetAdditionalIe(
    __in PAP_CONFIG Config
    );

VOID
CfgSaveBeaconIe(
    __in PAP_CONFIG Config,
    __in PVOID ApBeaconIEData,
    __in ULONG ApBeaconIESize
    );

#endif // _AP_CONFIG_H


