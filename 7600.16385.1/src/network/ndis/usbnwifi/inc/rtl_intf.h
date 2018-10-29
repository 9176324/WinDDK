/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    RtlHw_Interface.h

Abstract:
    Interface functions into Hw layer
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created
    09-27-2005    Renamed

Notes:

--*/
#ifndef _HARDWARE_STATION_INTERFACE_H_
#define _HARDWARE_STATION_INTERFACE_H_

/**
 * This file contains all the functions that are shared between the hardware/station
 * code. This is required for the station functionality as implemented in the sample
 * driver. IHV specific station implementation may not use these functions and hence
 * they dont need to implement these (unlike the Sta11 & Hw11 functions which every
 * implementation would need to implement)
 */

//
// Forward declaration
//
typedef struct _ADAPTER         ADAPTER, *PADAPTER;
typedef struct _MP_TX_MSDU      MP_TX_MSDU, *PMP_TX_MSDU;
typedef struct _MP_RX_MSDU      MP_RX_MSDU, *PMP_RX_MSDU;
typedef struct _MP_RX_FRAGMENT  MP_RX_FRAGMENT, *PMP_RX_FRAGMENT ;


typedef struct _NIC_TX_MSDU     NIC_TX_MSDU, *PNIC_TX_MSDU;
typedef struct _NIC_RX_MSDU     NIC_RX_MSDU, *PNIC_RX_MSDU;
typedef struct _NIC_RX_FRAGMENT NIC_RX_FRAGMENT, *PNIC_RX_FRAGMENT;
typedef struct _NIC             NIC, *PNIC;


typedef struct _STA_BSS_ENTRY   STA_BSS_ENTRY, *PSTA_BSS_ENTRY;
typedef struct _STATION         STATION, *PSTATION;


/**
 * This function is invoked to ask the hardware functions to send a management
 * packet. The packet buffer is freed as soon as the function returns. This means 
 * the the function must copy the send buffer if it cannot send inline.
 *
 * \param pNic          NIC structure
 * \param pBSSentry     BSS Entry that this packet is being sent to. If the packet
 *                      is not BSS specific, this pointer could be NULL.
 * \param pMgmtPacket   Complete Management packet. This buffer would be freed
 *                      when the function returns.
 * \param MgmtPacketLength  Length of management packet
 * \return NDIS_STATUS from the function. This function cannot return 
 *         NDIS_STATUS_PENDING since the packet buffer would be freed 
 *         when the function returns.
 */
NDIS_STATUS
Hw11SendMgmtPacket(
    __in  PNIC            pNic,
    __in_opt  PSTA_BSS_ENTRY  pBSSEntry,
    __in  PUCHAR          pMgmtPacket,
    __in  USHORT          MgmtPacketLength
    );


/**
 * Called at the end of receive interrupt processing after we have
 * indicated packets. It is used to handle the situation where 
 * the hardware has run out of available descriptors and 
 * hence it wont interrupt for receive. In this scenarios, 
 * this routine would temporarily enable the descriptor overflow 
 * interrupt to raise an extra interrupt
 * 
 * \param pNic          NIC structure
 * \sa 
 */
VOID
Hw11ReceiveInterruptComplete(
    __in  PNIC            pNic
    );

#endif // _HARDWARE_STATION_INTERFACE_H_

