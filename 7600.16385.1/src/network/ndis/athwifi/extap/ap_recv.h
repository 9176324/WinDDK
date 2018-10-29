/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    ap_recv.h

Abstract:
    AP layer receive processing functions
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    10-08-2007    Created

Notes:

--*/
#ifndef _AP_RECV_H_
#define _AP_RECV_H_

#include <packon.h>
#include <packoff.h>

/**
 * Processing management packet
 */
NDIS_STATUS
ApReceiveMgmtPacket(
    __in PMP_EXTAP_PORT ApPort,
    __in PMP_RX_MPDU     Fragment,
    __in USHORT FragmentSize
    );

/**
 * Processing authentication
 */
VOID 
ApReceiveAuthentication(
    __in PMP_EXTAP_PORT ApPort,
    __in PDOT11_MGMT_HEADER MgmtPacket,
    __in USHORT PacketSize
    );

/**
 * Processing deauthentication
 */
VOID 
ApReceiveDeauthentication(
    __in PMP_EXTAP_PORT ApPort,
    __in PDOT11_MGMT_HEADER MgmtPacket,
    __in USHORT PacketSize
    );

/**
 * Processing association request
 */
VOID 
ApReceiveAssociationRequest(
    __in PMP_EXTAP_PORT ApPort,
    __in PDOT11_MGMT_HEADER MgmtPacket,
    __in USHORT PacketSize
    );

/**
 * Processing reassociation request
 */
VOID 
ApReceiveReassociationRequest(
    __in PMP_EXTAP_PORT ApPort,
    __in PDOT11_MGMT_HEADER MgmtPacket,
    __in USHORT PacketSize
    );

/**
 * Processing disassociation
 */
VOID 
ApReceiveDisassociation(
    __in PMP_EXTAP_PORT ApPort,
    __in PDOT11_MGMT_HEADER MgmtPacket,
    __in USHORT PacketSize
    );

#endif // _AP_RECV_H_

