/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    hvl_intf.h

Abstract:
    Contains interfaces into the HVL
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    09-04-2007    Created

Notes:

--*/
#pragma once

//
// Forward declaration
//
typedef struct _ADAPTER         ADAPTER, *PADAPTER;
typedef struct _HVL             HVL, *PHVL;
typedef struct _HW              HW, *PHW;
typedef struct _MP_PORT         MP_PORT, *PMP_PORT;
typedef struct _VNIC            VNIC, *PVNIC;
typedef struct _HW_MAC_CONTEXT  HW_MAC_CONTEXT, *PHW_MAC_CONTEXT;

NDIS_STATUS
Hvl11Allocate(
    __in  NDIS_HANDLE             MiniportAdapterHandle,
    __deref_out_opt PHVL*         ppHvl,
    __in  PADAPTER                Adapter
    );


VOID
Hvl11Free(
    __in  PHVL                    Hvl
    );

NDIS_STATUS
Hvl11Initialize(
    __in  PHVL                    Hvl,
    __in  PHW                     Hw
    );

VOID
Hvl11Terminate(
    __in  PHVL                    Hvl
    );

NDIS_STATUS
Hvl11Fill80211Attributes(
    __in  PHVL                    Hvl,
    __out PNDIS_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES Attr
    );

VOID
Hvl11Cleanup80211Attributes(
    __in  PHVL                    Hvl,
    __in  PNDIS_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES Attr
    );

VOID
Hvl11EnableContextSwitches(
    __in  PHVL                    Hvl
    );

VOID
Hvl11ActivatePort(
    __in  PHVL                    Hvl,
    __in  PVNIC                   VNic
    );

NDIS_STATUS
Hvl11AllocateMACContext(
    __in  PHVL                    Hvl,
    __out PHW_MAC_CONTEXT*        MacContext,
    __in  PVNIC                   VNic
    );

VOID
Hvl11FreeMACContext(
    __in  PHVL                    Hvl,
    __in  PHW_MAC_CONTEXT         MacContext
    );


VOID 
Hvl11SendCompletePackets(
    __in  PVNIC                   VNic,
    __in  PMP_TX_MSDU             PacketList,
    __in  ULONG                   NumPkts,
    __in  ULONG                   SendCompleteFlags
    );

VOID
Hvl11IndicateReceivePackets(
    __in  PVNIC                   VNic,
    __in  PMP_RX_MSDU             PacketList,
    __in  ULONG                   ReceiveFlags
    );

NDIS_STATUS
Hvl11RegisterVNic(
    __in  PHVL    Hvl,
    __in  PVNIC   pVNic               
    );

NDIS_STATUS
Hvl11DeregisterVNic(
    __in  PHVL    Hvl,
    __in  PVNIC   pVNic               
    );
NDIS_STATUS
Hvl11RegisterHelperPort(
    __in  PHVL    pHvl,
    __in  PVNIC   pVNic               
    );

NDIS_STATUS
Hvl11DeregisterHelperPort(
    __in  PHVL    pHvl,
    __in  PVNIC   pVNic               
    );

NDIS_STATUS
Hvl11RequestExAccess(
    PHVL Hvl, 
    PVNIC VNic,
    BOOLEAN fPnPOperation
    );

NDIS_STATUS
Hvl11ReleaseExAccess(
    PHVL Hvl, 
    PVNIC VNic
    );

VOID
Hvl11BlockTimedCtxS(PHVL Hvl);

VOID
Hvl11UnblockTimedCtxS(PHVL Hvl);

VOID
Hvl11IndicateStatus(
    __in  PVNIC                   VNic,
    __in  NDIS_STATUS             StatusCode,
    __in  PVOID                   StatusBuffer,
    __in  ULONG                   StatusBufferSize
    );

VOID
Hvl11Notify(
    PHVL                    Hvl,
    PVOID                   pvNotif
    );


