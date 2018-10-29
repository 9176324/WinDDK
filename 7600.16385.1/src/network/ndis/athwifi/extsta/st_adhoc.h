/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    st_adhoc.h

Abstract:
    STA layer adhoc functions
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created
    01-15-2006    Renamed sta_adhoc.h to st_adhoc.h

Notes:

--*/
#ifndef _STATION_ADHOC_H
#define _STATION_ADHOC_H

#define STA11_ADHOC_JOIN_TIMEOUT                20              /* 20 beacons */
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
    __in  PMP_EXTSTA_PORT        pStation
    );

/**
 * Free the AdHoc station info
 * 
 * \param pStation  station pointer
 */
VOID
StaFreeAdHocStaInfo(
    __in  PMP_EXTSTA_PORT        pStation
    );

NTSTATUS
StaConnectAdHoc(
    __in  PMP_EXTSTA_PORT pStation
    );

NTSTATUS
StaDisconnectAdHoc(
    __in  PMP_EXTSTA_PORT pStation
    );
    
NDIS_STATUS 
StaSaveAdHocStaInfo(
    __in  PMP_EXTSTA_PORT        pStation,
    __in  PMP_RX_MPDU        pFragment,
    __in  PDOT11_BEACON_FRAME pDot11BeaconFrame,
    __in  ULONG           InfoElemBlobSize
    );

NDIS_STATUS
StaResetAdHocStaInfo(
    __in  PMP_EXTSTA_PORT       pStation,
    __in  BOOLEAN        flushStaList
    );

void
StaClearStaListAssocState(
    __in  PMP_EXTSTA_PORT    pStation,
    __in  BOOLEAN     SendDeauth
    );

NDIS_IO_WORKITEM_FUNCTION StaConnectAdHocWorkItem;

void 
StaAdHocReceiveDirectData(
    __in  PMP_EXTSTA_PORT                    pStation,
    __in  PDOT11_DATA_SHORT_HEADER    pDataHdr
    );

VOID
StaAdhocIndicateConnectionStart(
    __in  PMP_EXTSTA_PORT         pStation,
    __in_bcount_opt(DOT11_ADDRESS_SIZE)  DOT11_MAC_ADDRESS       BSSID,
    __in  BOOLEAN                 Internal
    );

VOID
StaAdhocIndicateConnectionCompletion(
    __in  PMP_EXTSTA_PORT         pStation,
    __in  ULONG                   CompletionStatus,
    __in  BOOLEAN                 Internal
    );

void 
StaAdHocIndicateAssociation(
    __in  PMP_EXTSTA_PORT pStation,
    __in  PSTA_ADHOC_STA_ENTRY StaEntry
    );

NDIS_STATUS
StaFlushAdHocStaList(
    __in  PMP_EXTSTA_PORT         pStation
    );

NDIS_TIMER_FUNCTION StaAdHocWatchdogTimerRoutine;

VOID
StaAdhocProcessMgmtPacket(
    __in  PMP_EXTSTA_PORT                        pStation,
    __in  PDOT11_MGMT_HEADER              MgmtPacket,
    __in  ULONG                           PacketLength
    );

VOID
StaStopAdHocBeaconing(
    __in  PMP_EXTSTA_PORT                        pStation
    );

NDIS_STATUS
StaEnumerateAssociationInfoAdHoc(
    __in  PMP_EXTSTA_PORT        pStation,
    __out PDOT11_ASSOCIATION_INFO_LIST   pAssocInfoList,
    __in  ULONG           TotalLength
    );

NDIS_STATUS
StaStartAdHocBeaconing(
    __in  PMP_EXTSTA_PORT pStation
    );

VOID
StaStopAdHocBeaconing(
    __in  PMP_EXTSTA_PORT pStation
    );


#endif

