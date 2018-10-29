/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    helper_port_scan.h

Abstract:
    Contains helper port scan specific defines
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    09-04-2007    Created

Notes:

--*/

#pragma once

NDIS_STATUS
HelperPortInitializeScanContext(
    __in  PMP_HELPER_PORT         HelperPort
    );

VOID
HelperPortTerminateScanContext(
    __in  PMP_HELPER_PORT         HelperPort
    );

NDIS_STATUS
HelperPortCreateScanChannelList(
    __in  PMP_HELPER_PORT         HelperPort
    );

VOID
HelperPortScanParametersReleaseRef(
    __in  PMP_HELPER_PORT         HelperPort,
    __in  PMP_SCAN_PARAMETERS     ScanParameters
    );

VOID
HelperPortProcessPendingScans(
    __in  PMP_HELPER_PORT         HelperPort
    );

VOID
HelperPortIndicateScanCompletion(
    __in  PMP_HELPER_PORT         HelperPort,
    __in  PMP_SCAN_PARAMETERS     ScanParameters,
    __in  PNDIS_STATUS            CompletionStatus
    );

BOOLEAN
HelperPortShouldPerformScan(
    __in  PMP_HELPER_PORT         HelperPort,
    __in  PMP_SCAN_PARAMETERS     ScanParameters
    );
    
NDIS_STATUS
HelperPortScanExAccessCallback(
    __in  PMP_PORT                Port, 
    __in  PVOID                   Ctx
    );

NDIS_STATUS 
HelperPortScanCompleteCallback(
    __in  PMP_PORT                Port,
    __in  PVOID                   Data
    );

VOID
HelperPortScanTimerCallback(
    __in  PMP_HELPER_PORT         HelperPort
    );

NDIS_TIMER_FUNCTION HelperPortScanTimer;

VOID
HelperPortStartScanProcess(
    __in  PMP_HELPER_PORT         HelperPort,
    __in  PMP_SCAN_PARAMETERS     ScanParameters
    );


VOID
HelperPortCompleteScanProcess(
    __in  PMP_HELPER_PORT         HelperPort,
    __in  PMP_SCAN_PARAMETERS     ScanParameters,
    __in  PNDIS_STATUS            CompletionStatus
    );

VOID
HelperPortStartPartialScan(
    __in  PMP_HELPER_PORT         HelperPort,
    __in  PMP_SCAN_PARAMETERS     ScanParameters
    );
    
VOID
HelperPortCompletePartialScan(
    __in  PMP_HELPER_PORT         HelperPort,
    __in  PMP_SCAN_PARAMETERS     ScanParameters,
    __in  PNDIS_STATUS            CompletionStatus
    );


