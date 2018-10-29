/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    st_recv.h

Abstract:
    STA layer receive processing functions
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created
    01-15-2006    Renamed sta_recv.h to st_recv.h

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
 * \param pFragment
 * \param pFragmentHdr
 * \param FragmentSize
 * \return 
 * \sa 
 */
NDIS_STATUS
StaReceiveMgmtPacket(
    __in  PMP_EXTSTA_PORT                        pStation,
    __in  PMP_RX_MPDU                    pFragment,
    __in  USHORT                          FragmentSize
    );

/**
 * 
 * 
 * \param pStation
 * \param pFragment
 * \param TotalLength
 * \return 
 * \sa 
 */
VOID 
StaReceiveBeacon(
    __in  PMP_EXTSTA_PORT                        pStation,
    __in  PMP_RX_MPDU                    pFragment,
    __in  ULONG                           TotalLength
    );

/**
 * 
 * 
 * \param pStation
 * \param pFragment
 * \param TotalLength
 * \sa 
 */
VOID 
StaReceiveAuthentication(
    __in  PMP_EXTSTA_PORT                        pStation,
    __in  PMP_RX_MPDU                    pFragment,
    __in  ULONG                           TotalLength
    );

/**
 * 
 * 
 * \param pStation
 * \param pFragment
 * \param TotalLength
 * \sa 
 */
VOID 
StaReceiveDeauthentication(
    __in  PMP_EXTSTA_PORT                        pStation,
    __in  PMP_RX_MPDU                    pFragment,
    __in  ULONG                           TotalLength
    );

/**
 * 
 * 
 * \param pStation
 * \param pFragment
 * \param TotalLength
 * \return 
 * \sa 
 */
VOID 
StaReceiveAssociationResponse(
    __in  PMP_EXTSTA_PORT                        pStation,
    __in  PMP_RX_MPDU                    pFragment,
    __in  ULONG                           TotalLength
    );


/**
 * 
 * 
 * \param pStation
 * \param pFragment
 * \param TotalLength
 * \return 
 * \sa 
 */
VOID 
StaReceiveDisassociation(
    __in  PMP_EXTSTA_PORT                        pStation,
    __in  PMP_RX_MPDU                    pFragment,
    __in  ULONG                           TotalLength
    );

#endif // _STATION_RECEIVE_H_

