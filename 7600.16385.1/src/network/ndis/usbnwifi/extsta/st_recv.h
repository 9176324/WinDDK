/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Sta_Recv.h

Abstract:
    STA layer receive processing functions
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
#ifndef _STATION_RECEIVE_H_
#define _STATION_RECEIVE_H_

#include <packon.h>
typedef struct {
    UCHAR sh_dsap;
    UCHAR sh_ssap;
    UCHAR sh_ctl;
    UCHAR sh_protid[3];
    unsigned short  sh_etype;
} IEEE_8022_LLC_SNAP, *PIEEE_8022_LLC_SNAP;
#include <packoff.h>

/**
 * 
 * 
 * \param pStation
 * \param pMpFragment
 * \param pNicFragment
 * \param pFragmentHdr
 * \param FragmentSize
 * \return 
 * \sa 
 */
NDIS_STATUS
StaReceiveMgmtPacket(
    __in  PSTATION                        pStation,
    __in  PMP_RX_FRAGMENT                 pMpFragment,
    __in  PNIC_RX_FRAGMENT                pNicFragment,
    __in  PDOT11_MAC_HEADER               pFragmentHdr,
    __in  USHORT                          FragmentSize
    );

/**
 * 
 * 
 * \param pStation
 * \param pNicFragment
 * \param TotalLength
 * \return 
 * \sa 
 */
VOID 
StaReceiveBeacon(
    __in  PSTATION                        pStation,
    __in  PNIC_RX_FRAGMENT                pNicFragment,
    __in  ULONG                           TotalLength
    );

/**
 * 
 * 
 * \param pStation
 * \param pNicFragment
 * \param TotalLength
 * \sa 
 */
VOID 
StaReceiveAuthentication(
    __in  PSTATION                        pStation,
    __in  PNIC_RX_FRAGMENT                pNicFragment,
    __in  ULONG                           TotalLength
    );

/**
 * 
 * 
 * \param pStation
 * \param pNicFragment
 * \param TotalLength
 * \sa 
 */
VOID 
StaReceiveDeauthentication(
    __in  PSTATION                        pStation,
    __in  PNIC_RX_FRAGMENT                pNicFragment,
    __in  ULONG                           TotalLength
    );

/**
 * 
 * 
 * \param pStation
 * \param pNicFragment
 * \param TotalLength
 * \return 
 * \sa 
 */
VOID 
StaReceiveAssociationResponse(
    __in  PSTATION                        pStation,
    __in  PNIC_RX_FRAGMENT                pNicFragment,
    __in  ULONG                           TotalLength
    );


/**
 * 
 * 
 * \param pStation
 * \param pNicFragment
 * \param TotalLength
 * \return 
 * \sa 
 */
VOID 
StaReceiveDisassociation(
    __in  PSTATION                        pStation,
    __in  PNIC_RX_FRAGMENT                pNicFragment,
    __in  ULONG                           TotalLength
    );

#endif // _STATION_RECEIVE_H_

