/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    mp_pnp.h

Abstract:
    Contains defines relevant for MP layer initialize/halt
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    09-04-2007    Created

Notes:

--*/

#pragma once

NDIS_STATUS
MpAllocateAdapter(
    __in  NDIS_HANDLE             MiniportAdapterHandle,
    __deref_out_opt PADAPTER*     Adapter
    );

VOID
MpFreeAdapter(
    __in  PADAPTER                Adapter
    );


NDIS_STATUS
MpReadRegistryConfiguration(
    __in  PADAPTER                Adapter
    );

NDIS_STATUS
MpSetRegistrationAttributes(
    __in  PADAPTER                Adapter
    );
    
NDIS_STATUS
MpSetMiniportAttributes(
    __in  PADAPTER                Adapter
    );

NDIS_STATUS
MpInitializeAdapter(
    __in  PADAPTER                Adapter,
    __out NDIS_ERROR_CODE*        ErrorCode,
    __out PULONG                  ErrorValue
    );

VOID
MpTerminateAdapter(
    __in  PADAPTER                Adapter
    );

NDIS_STATUS
MpStart(
    __in  PADAPTER                Adapter
    );

VOID
MpStop(
    __in  PADAPTER                Adapter,
    __in  NDIS_HALT_ACTION        HaltAction
    );

BOOLEAN
MpRemoveAdapter(
    __in  PADAPTER                Adapter
    );


