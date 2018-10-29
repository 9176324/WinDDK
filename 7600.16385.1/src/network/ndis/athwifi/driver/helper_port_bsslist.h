/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    helper_port_bsslist.h

Abstract:
    Contains helper port bsslist specific defines
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    09-04-2007    Created

Notes:

--*/

#pragma once

/**
 * Initializes the BSS entries list
 * 
 * \param pBSSList	BSS entries list structure
 */
NDIS_STATUS
HelperPortInitializeBSSList(
    __in PMP_HELPER_PORT          HelperPort
    );

/**
 * Frees the BSS entries list
 * 
 * \param pBSSList	BSS entries list structure
 */
VOID
HelperPortTerminateBSSList(
    __in PMP_HELPER_PORT          HelperPort
    );

#if DBG
NDIS_STATUS
HelperPortCheckForExtraRef(
    __in PMP_HELPER_PORT          HelperPort
    );
#endif


NDIS_STATUS
HelperPortReceiveMgmtPacket(
    __in  PMP_HELPER_PORT         HelperPort,
    __in  PMP_RX_MPDU             pNicFragment,
    __in  USHORT                  FragmentSize
    );

VOID 
HelperPortReceiveBeacon(
    __in  PMP_HELPER_PORT         HelperPort,
    __in  PMP_RX_MPDU             pFragment,
    __in  ULONG                   TotalLength
    );

/**
 * Verifies that the beacon is acceptable
 * 
 * \param pDot11BeaconFrame Beacon frame buffer
 * \return Appropriate NDIS_STATUS
 */
NDIS_STATUS HelperPortValidateBeacon(
    __in  PDOT11_BEACON_FRAME     pDot11BeaconFrame
    );



/**
 * Adds an BSSEntry into the pBSSList
 *
 * \note Write lock for the list must be acquired before 
 * making this call
 * 
 * \param pBSSList  BSSList structure to add to
 * \param pBSSEntry	The BSSEntry to be added to the list structure
 */
VOID
HelperPortAddBSSEntry(
    __in  PMP_BSS_LIST           pBSSList,
    __in  PMP_BSS_ENTRY          pBSSEntry
    );

/**
 * Removes an BSSEntry from the APList
 *
 * \note Write lock for the list must be acquired before 
 * making this call
 * 
 * \param pBSSList	APList structure to remove the entry from
 * \param pBSSEntry	The BSSEntry to be removed from the list structure
 */
VOID
HelperPortRemoveBSSEntry(
    __in  PMP_BSS_LIST           pBSSList,
    __in  PMP_BSS_ENTRY          pBSSEntry
    );

/**
 * Searches for the BSSEntry for the access point using AP's MAC address
 *
 * \note APList must be locked
 *
 * \param pBSSList      APList structure to search inside
 * \param srcAddress    The MAC address to search for
 * \return  If found, BSSEntry corresponding to the specified MAC address. Else
 *          NULL.
 */
PMP_BSS_ENTRY
HelperPortFindBSSEntry(
    __in  PMP_BSS_LIST           pBSSList,
    __in  const unsigned char *   srcAddress
    );


/**
 * Searches for an old BSSEntry in the APList that can be expired.
 * Goes through APList and finds the first BSSEntry for which we havent 
 * received a beacon/probe response since ullExpireTimeStamp. This entry
 * is removed from the list and then entry returned. The memory is not
 * freed
 *
 * \note Write lock for the list must be acquired before 
 * making this call
 *
 * \param pBSSList      The APList structure to expire and entry from
 * \param ullMaxActiveTime      Expire entries that are older than this period (in 100 ns)
 * \param ullExpireTimeStamp    Time duration in 100ns. This is used to
 *                      determine which entries are old enough to be expired.
 * \return Returns an BSSEntry that has been expired. The entry is removed
 *          from the list  but not free. If no entries can be
 *          expired, this function returns NULL
 */
PMP_BSS_ENTRY
HelperPortExpireBSSEntry(
    __in  PMP_BSS_LIST           pBSSList,
    __in  ULONGLONG               ullMaxActiveTime,
    __in  ULONGLONG               ullExpireTimeStamp
    );

/**
 * Adds/Updates information about an access point to the APList. If the
 * access point had been previously discovered, the AP's information is
 * updated, else its added.
 * 
 * \param pStation          Station structure
 * \param pFragment      NIC Fragment structure containing the received
 *                          beacon/probe response
 * \param pDot11BeaconFrame Start of beacon frame
 * \param BeaconDataLength  Length of actual data in the beacon. This includes
 *                          beacon frame header and total length of the
 *                          information elements in the beacon
 * \return NDIS_STATUS depending on whether or not the save succeeded
 * \sa StaInsertBSSEntry, StaUpdateBSSEntry
 */
NDIS_STATUS 
HelperPortSaveBSSInformation(
    __in  PMP_HELPER_PORT        pStation,
    __in  PMP_RX_MPDU        pFragment,
    __in  PDOT11_BEACON_FRAME pDot11BeaconFrame,
    __in  ULONG           BeaconDataLength
    );


/**
 * Inserts information about the access point to the AP list
 * 
 * \param pStation          Station structure
 * \param pFragment      NIC Fragment structure containing the received
 *                          beacon/probe response
 * \param pDot11BeaconFrame Start of beacon frame
 * \param BeaconDataLength  Length of actual data in the beacon. This includes
 *                          beacon frame header and total length of the
 *                          information elements in the beacon
 * \return NDIS_STATUS depending on whether or not the save succeeded
 * \sa StaSaveApInformation
 */
NDIS_STATUS 
HelperPortInsertBSSEntry(
    __in  PMP_HELPER_PORT        pStation,
    __in  PMP_RX_MPDU        pFragment,
    __in  PDOT11_BEACON_FRAME pDot11BeaconFrame,
    __in  ULONG           BeaconDataLength
    );

/**
 * Updated specified AP entry with updated information from the new
 * beacon/probe response
 * 
 * \note The APList lock must be acquired. Also this function must be
 * called at IRQL_DISPATCH
 *
 * \param pStation          Station structure
 * \param pBSSEntry          BSSEntry to be updated
 * \param pFragment      NIC Fragment structure containing the received
 *                          beacon/probe response
 * \param pDot11BeaconFrame Start of beacon frame
 * \param BeaconDataLength  Length of actual data in the beacon. This includes
 *                          beacon frame header and total length of the
 *                          information elements in the beacon
 * \return NDIS_STATUS depending on whether or not the save succeeded
 * \sa StaSaveApInformation
 */
NDIS_STATUS 
HelperPortUpdateBSSEntry(
    __in  PMP_HELPER_PORT        pStation,
    __in  PMP_BSS_ENTRY  pBSSEntry,
    __in  PMP_RX_MPDU        pFragment,
    __in  PDOT11_BEACON_FRAME pDot11BeaconFrame,
    __in  ULONG           BeaconDataLength
    );




