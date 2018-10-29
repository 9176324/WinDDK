/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    hw_context.h

Abstract:
    Contains the defines for MAC context management for the HW layer
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    09-04-2007    Created

Notes:

--*/
#pragma once


#define HW_WAIT_FOR_BSS_JOIN(_MacContext)     (_MacContext->JoinWaitForBeacon = 1)
#define HW_STOP_WAITING_FOR_JOIN(_MacContext)      \
    (InterlockedCompareExchange((PLONG)&_MacContext->JoinWaitForBeacon, 0, 1) == 1)

/** 
 * Duration of the per-context periodic timer (this is in milliseconds)
 */
#define HW_CONTEXT_PERIODIC_TIMER_DURATION      2000

NDIS_STATUS
HwInitializeMACContext(
    __in  PHW                     Hw,
    __in  PHW_MAC_CONTEXT         MacContext,
    __in  PVNIC                   VNic,
    __in  ULONG                   ContextId
    );

VOID
HwTerminateMACContext(
    __in  PHW                     Hw,
    __in  PHW_MAC_CONTEXT         MacContext
    );

NDIS_STATUS
HwResetMACContext(
    __in  PHW_MAC_CONTEXT         MacContext,
    __in  PDOT11_RESET_REQUEST    Dot11ResetRequest    
    );

NDIS_STATUS
HwSetMACContextOnNic(
    __in  PHW                     Hw,
    __in  PHW_MAC_CONTEXT         MacContext
    );

NDIS_STATUS
HwClearMACContextFromNic(
    __in  PHW                     Hw,
    __in  PHW_MAC_CONTEXT         MacContext
    );

VOID
HwDot11ResetStep1(
    __in  PHW_MAC_CONTEXT         MacContext,
    __in  PDOT11_RESET_REQUEST    Dot11ResetRequest    
    );

NDIS_STATUS
HwDot11ResetStep2(
    __in  PHW_MAC_CONTEXT         MacContext,
    __in  PDOT11_RESET_REQUEST    Dot11ResetRequest    
    );

NDIS_STATUS
HwUpdateBSSDescription(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  PMP_BSS_DESCRIPTION     BSSDescription,
    __in  BOOLEAN                 BSSStart
    );

NDIS_STATUS 
HwJoinChannelSwitchComplete(
    __in  PHW                     Hw,
    __in  PHW_MAC_CONTEXT         MacContext,
    __in_opt  PVOID                   Data
    );

NDIS_TIMER_FUNCTION HwJoinTimeoutTimer;

VOID
HwJoinBSSComplete(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in_opt  PHW_RX_MPDU             Mpdu,
    __in  NDIS_STATUS             Status
    );

VOID
HwScanComplete(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  NDIS_STATUS             Status
    );

NDIS_TIMER_FUNCTION HwMACContextPeriodicTimer;


