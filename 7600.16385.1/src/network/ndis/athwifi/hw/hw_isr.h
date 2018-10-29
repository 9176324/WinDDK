/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    hw_isr.h

Abstract:
    Contains defines for interrupt processing in the HW layer
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    09-04-2007    Created

Notes:

--*/
#pragma once

NDIS_STATUS
HwRegisterInterrupt(
    __in  PHW                     Hw,
    __out NDIS_ERROR_CODE*        ErrorCode,
    __out PULONG                  ErrorValue
    );

VOID
HwDeregisterInterrupt(
    __in  PHW                     Hw
    );

MINIPORT_ISR HWInterrupt;

// The pre 6.20 Interrupt DPC that does not support receive side throttling
MINIPORT_INTERRUPT_DPC HWInterruptDPCNoRST;

// Receive side throttling compatible interrupt DPC
MINIPORT_INTERRUPT_DPC HWInterruptDPC;

MINIPORT_ENABLE_INTERRUPT HWEnableInterrupt;

MINIPORT_DISABLE_INTERRUPT HWDisableInterrupt;

VOID
HwEnableInterrupt(
    __in  PHW                     Hw,
    __in  HW_ISR_TRACKING_REASON  Reason
    );

VOID
HwEnableInterruptWithSync(
    __in  PHW                     Hw,
    __in  HW_ISR_TRACKING_REASON  Reason
    );

VOID
HwDisableInterrupt(
    __in  PHW                     Hw,
    __in  HW_ISR_TRACKING_REASON  Reason
    );

VOID
HwDisableInterruptWithSync(
    __in  PHW                     Hw,
    __in  HW_ISR_TRACKING_REASON  Reason
    );

VOID
HwProcessBeaconInterrupt(
    __in  PHW                     Hw,
    __in  ULONG                   Isr
    );

VOID
HwProcessATIMWindowInterrupt(
    __in  PHW                     Hw
    );

VOID
HwSetBeaconIEWithSync(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  PVOID                   pBeaconIEBlob,
    __in  ULONG                   uBeaconIEBlobSize
    );


