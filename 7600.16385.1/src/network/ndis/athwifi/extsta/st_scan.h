/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    st_scan.h

Abstract:
    STA layer OS/internal scan functions
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created
    01-15-2006    Renamed sta_scan.h to st_scan.h

Notes:

--*/
#ifndef _STATION_SCAN_H_
#define _STATION_SCAN_H_


/** OS initiated scan is in progress (running) */
#define STA_EXTERNAL_SCAN_IN_PROGRESS           0x00000001

/** We are completing an external scan */
#define STA_COMPLETE_EXTERNAL_SCAN              0x00000002

/** Internal perdiodic scan is in progress (running) */
#define STA_PERIODIC_SCAN_IN_PROGRESS           0x00000010

/** We are completing a periodic scan */
#define STA_COMPLETE_PERIODIC_SCAN              0x00000020

/** Internal perdiodic scan has been queued to run (queued but not yet run) */
#define STA_PERIODIC_SCAN_QUEUED                0x00000040

/** Stop periodic scanning (dont requeue) */
#define STA_STOP_PERIODIC_SCAN                  0x00000100

/* Macros to change scanning flags */
#define STA_SET_SCAN_FLAG(_S, _F)       MP_SET_FLAG((_S)->ScanContext.Flags, (_F))
#define STA_TEST_SCAN_FLAG(_S, _F)      MP_TEST_FLAG((_S)->ScanContext.Flags, (_F))
#define STA_CLEAR_SCAN_FLAG(_S, _F)     MP_CLEAR_FLAG((_S)->ScanContext.Flags, (_F))

NDIS_STATUS
StaInitializeScanContext(
    __in  PMP_EXTSTA_PORT            pStation
    );

VOID
StaFreeScanContext(
    __in  PMP_EXTSTA_PORT            pStation
    );

VOID
StaStopScan(
    __in  PMP_EXTSTA_PORT            pStation
    );

VOID
StaStopPeriodicScan(
    __in  PMP_EXTSTA_PORT            pStation
    );

VOID
StaStartPeriodicScan(
    __in  PMP_EXTSTA_PORT            pStation
    );

VOID
StaWaitForPeriodicScan(
    __in  PMP_EXTSTA_PORT            pStation
    );

VOID
StaForceInternalScan(
    __in  PMP_EXTSTA_PORT            pStation,
    __in  BOOLEAN             bScanToRoam
    );

#ifndef HIDE_ANNOTATION
__drv_mustHold(KeSpinLockType)
__drv_requiresIRQL(DISPATCH_LEVEL)
#endif
VOID
StaPeriodicScanComplete(
    __in  PMP_EXTSTA_PORT            pStation,
    __in  NDIS_STATUS         ndisStatus
    );

/**
 * The timer routine that performs periodic scans
 */
NDIS_TIMER_FUNCTION StaPeriodicScanCallback;


NDIS_STATUS
StaStartScan(
    __in  PMP_EXTSTA_PORT            pStation,
    __in  PVOID               ScanRequestID,
    __in  PDOT11_SCAN_REQUEST_V2 pDot11ScanRequest,
    __out ULONG               ScanRequestBufferLength
    );

#endif

