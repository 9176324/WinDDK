/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Sta_aplist.h

Abstract:
    STA layer BSS list functions
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
#ifndef _STATION_AP_LIST_H_
#define _STATION_AP_LIST_H_

#define STA_IS_BASIC_RATE(rate)     ((rate) & 0x80)
#define STA_DATA_RATE(rate)         ((rate) & 0x7f)

/**
 * Verifies that the beacon is acceptable
 * 
 * \param pDot11BeaconFrame Beacon frame buffer
 * \return Appropriate NDIS_STATUS
 */
NDIS_STATUS StaValidateBeacon(
    __in  PDOT11_BEACON_FRAME     pDot11BeaconFrame
    );

/**
 * Initializes the BSS entries list
 * 
 * \param pBSSList  BSS entries list structure
 */
VOID
StaInitializeBSSList(
    __in  PSTA_BSS_LIST           pBSSList
    );

/**
 * Adds an BSSEntry into the pBSSList
 *
 * \note Write lock for the list must be acquired before 
 * making this call
 * 
 * \param pBSSList  BSSList structure to add to
 * \param pBSSEntry The BSSEntry to be added to the list structure
 */
VOID
StaAddBSSEntry(
    __in  PSTA_BSS_LIST           pBSSList,
    __in  PSTA_BSS_ENTRY          pBSSEntry
    );

/**
 * Removes an BSSEntry from the APList
 *
 * \note Write lock for the list must be acquired before 
 * making this call
 * 
 * \param pBSSList  APList structure to remove the entry from
 * \param pBSSEntry The BSSEntry to be removed from the list structure
 */
VOID
StaRemoveBSSEntry(
    __in  PSTA_BSS_LIST           pBSSList,
    __in  PSTA_BSS_ENTRY          pBSSEntry
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
PSTA_BSS_ENTRY
StaFindBSSEntry(
    __in  PSTA_BSS_LIST           pBSSList,
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
PSTA_BSS_ENTRY
StaExpireBSSEntry(
    __in  PSTA_BSS_LIST           pBSSList,
    __in  ULONGLONG               ullMaxActiveTime,
    __in  ULONGLONG               ullExpireTimeStamp
    );

/**
 * Adds/Updates information about an access point to the APList. If the
 * access point had been previously discovered, the AP's information is
 * updated, else its added.
 * 
 * \param pStation          Station structure
 * \param pNicFragment      NIC Fragment structure containing the received
 *                          beacon/probe response
 * \param pDot11BeaconFrame Start of beacon frame
 * \param BeaconDataLength  Length of actual data in the beacon. This includes
 *                          beacon frame header and total length of the
 *                          information elements in the beacon
 * \return NDIS_STATUS depending on whether or not the save succeeded
 * \sa StaInsertBSSEntry, StaUpdateBSSEntry
 */
NDIS_STATUS 
StaSaveBSSInformation(
    __in  PSTATION        pStation,
    __in  PNIC_RX_FRAGMENT    pNicFragment,
    __in  PDOT11_BEACON_FRAME pDot11BeaconFrame,
    __in  ULONG           BeaconDataLength
    );


/**
 * Inserts information about the access point to the AP list
 * 
 * \param pStation          Station structure
 * \param pNicFragment      NIC Fragment structure containing the received
 *                          beacon/probe response
 * \param pDot11BeaconFrame Start of beacon frame
 * \param BeaconDataLength  Length of actual data in the beacon. This includes
 *                          beacon frame header and total length of the
 *                          information elements in the beacon
 * \return NDIS_STATUS depending on whether or not the save succeeded
 * \sa StaSaveApInformation
 */
NDIS_STATUS 
StaInsertBSSEntry(
    __in  PSTATION        pStation,
    __in  PNIC_RX_FRAGMENT    pNicFragment,
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
 * \param pNicFragment      NIC Fragment structure containing the received
 *                          beacon/probe response
 * \param pDot11BeaconFrame Start of beacon frame
 * \param BeaconDataLength  Length of actual data in the beacon. This includes
 *                          beacon frame header and total length of the
 *                          information elements in the beacon
 * \return NDIS_STATUS depending on whether or not the save succeeded
 * \sa StaSaveApInformation
 */
NDIS_STATUS 
StaUpdateBSSEntry(
    __in  PSTATION        pStation,
    __in  PSTA_BSS_ENTRY  pBSSEntry,
    __in  PNIC_RX_FRAGMENT    pNicFragment,
    __in  PDOT11_BEACON_FRAME pDot11BeaconFrame,
    __in  ULONG           BeaconDataLength
    );

/**
 * Determines the list of access points that can be used for association.
 * This uses the list of discovered access points along with parameters set
 * be the operating system to determine list of candidate access point. The
 * function populates the station's connection context structure. The
 * access points are sorted by priority with the first access point in 
 * the list as best association candidate
 * 
 * \param pStation  Station structure
 * \param bStrictFiltering     True if we want to be stricter filter in discarding APs. This
 *                  is FALSE when we are not connected and want to connect and is TRUE
 *                  if we are roaming when connected
 * \return NDIS_STATUS depending on whether or not it was able to obtain
 * list of candidate access points
 * \sa StaRankCandidateAPList
 */
NDIS_STATUS
StaGetCandidateAPList(
    __in  PSTATION                        pStation,
    __in  BOOLEAN                         bStrictFiltering
    );


/**
 * Ranks the candidate access points list in the connection context.
 * It will rank and order the access points in the list such that the first
 * entry in the list is the most preferred access point for association. 
 *
 * \note APs which are not picked for use must have their refcounts decremented
 * 
 * \param pStation  Station structure contain the connection context
 * \return NDIS_STATUS
 * \sa StaGetCandidateAPLists
 */
NDIS_STATUS
StaRankCandidateAPList(
    __in  PSTATION                        pStation
    );

/**
 * Determines if the specified AP can be used for association based on
 * the current configuration set by the operating system.
 *
 * \note The BSSEntry must be locked
 * 
 * \param pBSSEntry  BSSEntry to be used for matching
 * \param pStation  Station structure
 * \return TRUE if the AP can be used for association. FALSE otherwise
 */
BOOLEAN
StaMatchBSSEntry(
    __in  PSTA_BSS_ENTRY                  pBSSEntry,
    __in  PSTATION                        pStation
    );

/**
 * Determines if the specified PhyId is in our desired PHY list.
 * 
 * \param pStation  Station structure
 * \param PhyId     the PhyId to check.
 * \return TRUE if the PhyId is in our desired PHY list. FALSE otherwise
 */
BOOLEAN
StaMatchPhyId(
    __in  PSTATION    pStation,
    __in  ULONG       PhyId
    );


/**
 * The timer routine that performs periodic scans
 */
EVT_WDF_TIMER StaPeriodicScanCallback;

/**
 * Returns the amount of space required for the IE's to be copied
 * as part of EnumBSS buffer
 *
 * \param pStaBSSEntry      The BSS entry to copy
 * \return Number of bytes required to copy the IE blob 
 * \sa StaCopyEnummBSSIEBuffer
 */
ULONG
StaGetEnumBSSIELength(
    __in PSTA_BSS_ENTRY       pStaBSSEntry
    );

/**
 * Copies the IE blob from the BSS_ENTRY into the OS buffer for
 * EnumBSS
 * 
 * \param pStaBSSEntry      The BSS entry to copy
 * \param pDestBuffer       Buffer to copy the IE information into
 * \param DestinationLength Available length in buffer
 * \param BytesWritten      Number of bytes copied
 * \return NDIS_STATUS from the operation
 * \sa StaGetEnummBSSIELength
 */
NDIS_STATUS
StaCopyEnumBSSIEBuffer(
    __in PSTA_BSS_ENTRY       pStaBSSEntry,
    __in PUCHAR               pDestBuffer,
    __in ULONG                DestinationLength,
    __out PULONG               BytesWritten    
    );

    
#endif

