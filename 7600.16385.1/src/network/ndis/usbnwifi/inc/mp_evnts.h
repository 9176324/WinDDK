/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Mp_Events.h

Abstract:
    Prototypes for MP layer hooking functions
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
#ifndef _MP_CUSTOM_INTERFACES_H_

#define _MP_CUSTOM_INTERFACES_H_


#define OID_RTL_SAMPLE_OID       0xDEEF1201
#define OID_RTL_SAMPLE2_OID      0xDEEF1202


/*
 * Any OIDs that are private to this this miniport should be added to the
 * macro below. At compile time this macro will aggregate these OIDs into
 * the supported OID list and automatically report them to the OS.
 *
 * If there is at least one OID in the list, please remember to include the
 * comma after the last OID. You will see compiler errors otherwise
 */
#define MP_PRIVATE_OID_LIST         \
    OID_RTL_SAMPLE_OID,               \
    OID_RTL_SAMPLE2_OID,
    


NDIS_STATUS
MpEventInitialize(
    __in PADAPTER    pAdapter
    );


VOID
MpEventTerminate(
    __in PADAPTER    pAdapter
    );


NDIS_STATUS
MpEventQueryInformation(
    __in   PADAPTER                  pAdapter,
    __in   NDIS_OID                  Oid,
    __in   PVOID                     InformationBuffer,
    __in   ULONG                     InformationBufferLength,
    __out   PULONG                    BytesWritten,
    __out   PULONG                    BytesNeeded
    );


NDIS_STATUS
MpEventSetInformation(
    __in   PADAPTER                  pAdapter,
    __in   NDIS_OID                  Oid,
    __in   PVOID                     InformationBuffer,
    __in   ULONG                     InformationBufferLength,
    __out   PULONG                    BytesRead,
    __out   PULONG                    BytesNeeded
    );

NDIS_STATUS
MpEventQuerySetInformation(
    __in   PADAPTER                  pAdapter,
    __in   NDIS_OID                  Oid,
    __in   PVOID                     InformationBuffer,
    __in   ULONG                     InputBufferLength,
    __in   ULONG                     OutputBufferLength,
    __in   ULONG                     MethodId,
    __out   PULONG                    BytesWritten,
    __out   PULONG                    BytesRead,
    __out   PULONG                    BytesNeeded
    );

BOOLEAN
MpEventCheckForHang(
    __in PADAPTER    pAdapter
    );


VOID
MpEventDot11Reset(
    __in PADAPTER            pAdapter,
    __in DOT11_RESET_TYPE    ResetType
    );


VOID
MpEventMiniportReset(
    __in PADAPTER    pAdapter
    );


NDIS_STATUS
MpEventSendNetBufferList(
    __in PADAPTER                        pAdapter,
    __in PNET_BUFFER_LIST                NetBufferList,
    __in ULONG                           SendFlags,
    __in PDOT11_EXTSTA_SEND_CONTEXT      pDot11SendContext,
    __in BOOLEAN                         DispatchLevel
    );

BOOL
MpEventShouldBufferPacket(
    __in PADAPTER                        pAdapter,
    __in PDOT11_EXTSTA_SEND_CONTEXT      pDot11SendContext
    );

VOID
MpEventSendComplete(
    __in PADAPTER        pAdapter,
    __in PMP_TX_MSDU     pTxd,
    __in NDIS_STATUS     ndisStatus,
    __in BOOLEAN         DispatchLevel
    );


NDIS_STATUS
MpEventPacketReceived(
    __in PADAPTER                        pAdapter,
    __in PMP_RX_FRAGMENT                 pMpFragment,
    __in PNIC_RX_FRAGMENT                pNicFragment,
    __in PDOT11_MAC_HEADER               pFragmentHdr,
    __in USHORT                          uFragmentSize
    );

VOID
MpEventHandleInterrupt(
    __in PADAPTER        pAdapter
    );

#endif  // _MP_CUSTOM_INTERFACES_H_

