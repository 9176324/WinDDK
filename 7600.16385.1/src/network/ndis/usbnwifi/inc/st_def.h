/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Sta_Defines.h

Abstract:
    Station layer global defines
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
#ifndef _STATION_DEFINES_H_
#define _STATION_DEFINES_H_

#if DBG
#define STA_DECREMENT_REF(x)    MPVERIFY(NdisInterlockedDecrement(&(x)) >= 0)
#define STA_INCREMENT_REF(x)    MPVERIFY(NdisInterlockedIncrement(&(x)) > 0)
#else
#define STA_DECREMENT_REF(x)    NdisInterlockedDecrement(&(x))
#define STA_INCREMENT_REF(x)    NdisInterlockedIncrement(&(x))
#endif

/**
 * Holds information about each BSS we have found.
 * This is updated on a beacon/probe response in the context of our
 * receive handler. A linked list of these entries is maintained
 * by the station to keep track of the discovered BSS
 */
typedef struct _STA_BSS_ENTRY 
{
    /** List entry linkage */
    LIST_ENTRY      Link;

    /** Reference count to keep track of number of users of the AP entry.
     * When the entry is added to the linked list, this starts at 1. Association
     * process would add an extra reference to keep the entry around while
     * it is still associating/associated.
     */
    LONG            RefCount;

    /**
     * Spinlock to protect modification of beacon/information element pointers while
     * other threads may be using it
     */
    NDIS_SPIN_LOCK  Lock;

    DOT11_PHY_TYPE  Dot11PhyType;

    ULONG           PhyId;

    /** RSSI for the beacon/probe */
    LONG            RSSI;

    /** Link quality */
    ULONG           LinkQuality;

    ULONG           ChannelCenterFrequency;
    
    DOT11_BSS_TYPE  Dot11BSSType;

    DOT11_MAC_ADDRESS Dot11BSSID;

    DOT11_MAC_ADDRESS MacAddress;

    USHORT          BeaconInterval;

    ULONGLONG       BeaconTimestamp;

    ULONGLONG       HostTimestamp;

    DOT11_CAPABILITY Dot11Capability;
    
    ULONG           MaxBeaconFrameSize;
    PUCHAR          pDot11BeaconFrame;      // Beacon frame starting after management header
    ULONG           BeaconFrameSize;
    
    PUCHAR          pDot11InfoElemBlob;     // Pointer to the information elements in the beacon frame
    ULONG           InfoElemBlobSize;       // Length of information element blob

    UCHAR           Channel;                // Valid only if it is non-zero

    DOT11_SSID      ProbeSSID;              // The SSID from probe responses. This is stored
                                            // separately since probe buffer may get overwritten
                                            // by beacons and we may lose the SSID from probe

    /** 
     * The OS needs to be given the association request 
     * packet information on a association completion. This 
     * information is cached in this structure
     */
    __field_ecount(AssocRequestLength) 
    PUCHAR          pAssocRequest;
    USHORT          AssocRequestLength;     // Includes header

    /** 
     * The OS needs to be given the association response
     * packet information on a association completion. This 
     * information is cached in this structure
     */
    USHORT          AssocResponseLength;    // Includes header
    __field_ecount(AssocResponseLength) 
    PUCHAR          pAssocResponse;

    /** Association ID */
    USHORT          AssocID;

    /** Association state. Just keeps state and not lock protected */
    DOT11_ASSOCIATION_STATE     AssocState;

    /** Timestamp when we obtained the association */
    LARGE_INTEGER   AssociationUpTime;

    /** 
     * Cost for roaming purpose. A lower number is better. We dont
     * rank of APs based on this, but use higher number this for rejecting 
     * some AP
     */
    LONG            AssocCost;

    /**
     * Time in 100 ns after which we assume connectivity is lost
     * with this AP
     */
    ULONGLONG       NoBeaconRoamTime;

    /**
     * Number of contiguous beacons which had signal strength lower than
     * our roaming threshold
     */
    ULONG           LowQualityCount;

} STA_BSS_ENTRY, * PSTA_BSS_ENTRY;

typedef struct _STA_ADHOC_STA_ENTRY 
{
    /** List entry linkage */
    LIST_ENTRY              Link;

    /** Information obtained from beacon frame */
    ULONG                   PhyId;
    DOT11_MAC_ADDRESS       Dot11BSSID;
    DOT11_MAC_ADDRESS       MacAddress;
    USHORT                  BeaconInterval;
    ULONGLONG               BeaconTimestamp;
    DOT11_CAPABILITY        Dot11Capability;
    ULONG                   InfoElemBlobSize;
    PUCHAR                  InfoElemBlobPtr;

    /** Group cipher the station is running if its auth algo is RSNA-PSK */
    DOT11_CIPHER_ALGORITHM  GroupCipher;

    UCHAR                   Channel;                // Valid only if it is non-zero

    /** Size of currently allocated buffer for IE */ 
    ULONG                   AllocatedIEBlobSize;

    /** Association state of the station */
    DOT11_ASSOCIATION_STATE AssocState;    

    /** Number of ticks lapsed since the station sent a de-auth frame */
    ULONG                   DeauthWaitingTick;

    /** Number of probe requests sent since we detect inactivity */
    ULONG                   ProbeRequestsSent;

    /** Time stamp of the receipt of the lastest beacon/probe response from the station */
    ULONGLONG               HostTimestamp;

} STA_ADHOC_STA_ENTRY, *PSTA_ADHOC_STA_ENTRY;

#endif // _STATION_DEFINES_H_

