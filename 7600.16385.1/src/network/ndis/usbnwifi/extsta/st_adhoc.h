/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Sta_Adhoc.h

Abstract:
    STA layer adhoc functions
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
#ifndef _STATION_ADHOC_H
#define _STATION_ADHOC_H

#define STA11_ADHOC_JOIN_TIMEOUT                2000            /* 2 seconds */
#define STA_ADHOC_MIN_UNREACHABLE_THRESHOLD     5000            /* 5 seconds */
#define STA_ADHOC_MAX_UNREACHABLE_THRESHOLD     30000           /* 30 seconds */
#define STA_DEAUTH_WAITING_THRESHOLD            3               /* 3 ticks, or 6 seconds */
#define STA_PROBE_REQUEST_LIMIT                 2               /* 2 probe requests */

/**
 * Information Element to be put in beacon for FHSS phy
 */
typedef struct STA_FHSS_IE {
    USHORT dot11DwellTime;
    UCHAR dot11HopSet;
    UCHAR dot11HopPattern;
    UCHAR dot11HopIndex;
} STA_FHSS_IE, *PSTA_FHSS_IE;

/**
 * Initializes the AdHoc station info
 * 
 * \param pStation  station pointer
 */
NDIS_STATUS
StaInitializeAdHocStaInfo(
    __in  PSTATION        pStation
    );

/**
 * Free the AdHoc station info
 * 
 * \param pStation  station pointer
 */
VOID
StaFreeAdHocStaInfo(
    __in  PSTATION        pStation
    );

NDIS_STATUS 
StaSaveAdHocStaInfo(
    __in  PSTATION        pStation,
    __in  PNIC_RX_FRAGMENT    pNicFragment,
    __in  PDOT11_BEACON_FRAME pDot11BeaconFrame,
    __in  ULONG           InfoElemBlobSize
    );

NDIS_STATUS
StaResetAdHocStaInfo(
    __in  PSTATION       pStation,
    __in  BOOLEAN        flushStaList
    );

void
StaClearStaListAssocState(
    __in  PSTATION    pStation,
    __in  BOOLEAN     SendDeauth
    );

EVT_WDF_WORKITEM StaConnectAdHoc;

void 
StaAdHocReceiveDirectData(
    __in  PSTATION                    pStation,
    __in  PDOT11_DATA_SHORT_HEADER    pDataHdr
    );

void 
StaAdHocIndicateAssociation(
    __in  PSTATION pStation,
    __in  PSTA_ADHOC_STA_ENTRY StaEntry
    );

NDIS_STATUS
StaFlushAdHocStaList(
    __in  PSTATION         pStation
    );

EVT_WDF_TIMER StaAdHocWatchdogTimerRoutine;

VOID
StaAdhocProcessMgmtPacket(
    __in  PSTATION                        pStation,
    __in  PDOT11_MGMT_HEADER              MgmtPacket,
    __in  ULONG                           PacketLength
    );

VOID
StaResumeAdHoc(
    __in  PSTATION                        pStation
    );

#endif

