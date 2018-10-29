/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    hw_main.h

Abstract:
    Contains defines for initialization/PNP routines in the HW layer
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    09-04-2007    Created

Notes:

--*/
#pragma once


VOID
HwResetSoftwareMacState(
    __in  PHW                     Hw
    );

VOID
HwResetSoftwarePhyState(
    __in  PHW                     Hw
    );

NDIS_STATUS
HwSetNicState(
    __in  PHW                     Hw
    );

NDIS_STATUS
HwClearNicState(
    __in  PHW                     Hw
    );

#ifndef HIDE_ANNOTATION
__drv_requiresIRQL(PASSIVE_LEVEL)
#endif
NDIS_STATUS
HwResetHAL(
    __in  PHW                     Hw,
    __in  PHW_HAL_RESET_PARAMETERS ResetParams,
    __in  BOOLEAN                 DispatchLevel
    );

/**
 * This function is called by NDIS when a NetBuffer, that had previously been
 * submitted to NDIS, is successfully scatter gathered.
 * 
 * \sa NdisMAllocateNetBufferSGList from the DDK
 */
MINIPORT_PROCESS_SG_LIST HWProcessSGList;

MINIPORT_ALLOCATE_SHARED_MEM_COMPLETE HWAllocateComplete;

/**
 * Helper routine to initialize Scatter Gather DMA engine during
 * Initialize.
 */
NDIS_STATUS
HwInitializeScatterGatherDma(
    __in  PHW                     Hw,
    __out NDIS_ERROR_CODE*        ErrorCode,
    __out PULONG                  ErrorValue
    );



