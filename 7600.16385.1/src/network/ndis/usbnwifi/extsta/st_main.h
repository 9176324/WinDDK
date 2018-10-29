/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Sta_main.h

Abstract:
    STA layer common defines
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
#ifndef _STATION_MAIN_H
#define _STATION_MAIN_H

#define STA11_MEMORY_TAG         'SltR'
#define STA11_MAX_IE_BLOB_SIZE    (DOT11_MAX_PDU_SIZE - sizeof(DOT11_MGMT_HEADER) - \
                                   FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements) - 4)


/** Maximum number of MAC addresses we support in the excluded list */
#define STA_EXCLUDED_MAC_ADDRESS_MAX_COUNT      4
/** Max number of desired BSSIDs we can handle */
#define STA_DESIRED_BSSID_MAX_COUNT             8
/** Max number of desired PHYs we can handle */
#define STA_DESIRED_PHY_MAX_COUNT               5
/** Max number of PMKID we can handle */
#define STA_PMKID_MAX_COUNT                     3
/** Max number of enabled multicast cipher algorithms */
#define STA_MULTICAST_CIPHER_MAX_COUNT          6

/** Maximum number of BSSs we will store in our BSS entries list */
#define STA_BSS_ENTRY_MAX_ENTRIES_DEFAULT       128
#define STA_BSS_ENTRY_MAX_ENTRIES_MIN           16
#define STA_BSS_ENTRY_MAX_ENTRIES_MAX           512

/** Maximum number of Adhoc stations we will cache state about */
#define STA_ADHOC_STA_MAX_ENTRIES_DEFAULT       64
#define STA_ADHOC_STA_MAX_ENTRIES_MIN           16
#define STA_ADHOC_STA_MAX_ENTRIES_MAX           512

/** Maximum number of candidate BSS we will use for association */
#define STA_CANDIDATE_AP_MAX_COUNT              32

/**
 * Time duration after it was created at which a BSS entry is 
 * considered to have expired
 */
#define STA_BSS_ENTRY_EXPIRE_TIME_DEFAULT       750000000       // 75 seconds
#define STA_BSS_ENTRY_EXPIRE_TIME_MIN           150000000       // 15 seconds
#define STA_BSS_ENTRY_EXPIRE_TIME_MAX           2000000000      // 200 seconds

/** Time to wait for infrastructure Join to complete (in number of beacon intervals) */
#define STA_JOIN_FAILURE_TIMEOUT                40 //10 

/** Time to wait for authentication handshakes to complete (in number of milliseconds) */
#define STA_AUTHENTICATE_FAILURE_TIMEOUT        1000 

/** Time to wait for association response (in number of milliseconds) */
#define STA_ASSOCIATE_FAILURE_TIMEOUT           1000 

/** Number to increate the association cost on failed association */
#define STA_ASSOC_COST_FAILED_CONNECT           2

/** Number of increment the association cost on remote disconnection */
#define STA_ASSOC_COST_REMOTE_DISCONNECT        2

/** Number of increae the association cost on disassociation due to roaming */
#define STA_ASSOC_COST_ROAM_DISCONNECT          1

/** Cost at which we will reject an association */
#define STA_ASSOC_COST_MAX_DONT_CONNECT         10

/** 
 * Number of beacon intervals after which if we havent 
 * received a beacon from the AP we assume we have 
 * lost connectivity
 */
#define STA_INFRA_ROAM_NO_BEACON_COUNT_DEFAULT  20
#define STA_INFRA_ROAM_NO_BEACON_COUNT_MIN      5
#define STA_INFRA_ROAM_NO_BEACON_COUNT_MAX      50

/**
 * Minimum time we give before connectivity is partially lost. This
 * is because we may not receive beacon when we are scanning and
 * dont want to penalize the AP for that
 */
#define STA_MIN_INFRA_UNREACHABLE_TIMEOUT       30000000     // In 100s of nanoseconds = 3 second

/**
 * Maximum time we give before assuming connectivity is lost.
 * At this time if we dont find a new AP, we will drop connectivity
 * 
 */
#define STA_MAX_INFRA_UNREACHABLE_TIMEOUT       100000000     // In 100s of nanoseconds = 10 second


/**
 * Link quality value (0-100 which if we go below for a sequence of beacons,
 * would suggest that we need to roam
 */
#define STA_INFRA_LINK_QUALITY_ROAM_THRESHOLD_DEFAULT   15
#define STA_INFRA_LINK_QUALITY_ROAM_THRESHOLD_MIN       5
#define STA_INFRA_LINK_QUALITY_ROAM_THRESHOLD_MAX       80

/**
 * Number of beacon for which we have continuously received
 * beacons with RSSI below above threshold, we will roam
 */
#define STA_INFRA_RSSI_ROAM_BEACON_COUNT_DEFAULT        15
#define STA_INFRA_RSSI_ROAM_BEACON_COUNT_MIN            5
#define STA_INFRA_RSSI_ROAM_BEACON_COUNT_MAX            20

/**
 * The interval at which the periodic scan timer 
 * runs. It will not scan 
 * 
 */ 
#define STA_DEFAULT_SCAN_TIMER_INTERVAL         1000

/**
 * Periodic scan will happen every so many intervals
 * if everything is okay. This is related to the
 * above time
 */
#define STA_DEFAULT_SCAN_TICK_COUNT             60

/**
 * Periodic scan will happen not be initiated if
 * we had performed a scan these many SECONDs ago
 * and we dont need to do a scan
 */
#define STA_PERIODIC_SCAN_NORMAL_GAP            70

/**
 * Periodic scan will happen not be initiated if
 * we had performed a scan these many SECONDs ago
 * and we need to do a scan for connecting or something.
 * This gets exponentially increased
 */
#define STA_PERIODIC_SCAN_SHORT_GAP             1

/**
 * Number of milliseconds to wait for queuing the timer
 * when we are not connected and want to 
 */
#define STA_FORCED_PERIODIC_SCAN_INTERVAL       10

/**
 * Time interval in seconds between two consecutive check
 * of new PKMID candidates.
 */
#define STA_PMKID_CHECK_INTERVAL                120

/** 
 * Listen interval in default power save mode
 */
#define STA_LISTEN_INTERVAL_DEFAULT             3

/** 
 * Listen interval in low power save mode
 */
#define STA_LISTEN_INTERVAL_LOW_PS_MODE         1


/**
 * Holds information about AdHoc stations we have discovered.
 */
#define STA_ADHOC_DISCONNECTED                  0x00000001
#define STA_ADHOC_CONNECTION_IN_PROGRESS        0x00000002
#define STA_ADHOC_CONNECTED                     0x00000004
#define STA_ADHOC_DISCONNECTION_IN_PROGRESS     0x00000008
#define STA_ADHOC_RESET_PENDING                 0x00000010

typedef struct _STATION_TIMER_SCAN_CONTEXT {

    PSTATION        Station;
}   STATION_TIMER_SCAN_CONTEXT, *PSTATION_TIMER_SCAN_CONTEXT;

//
// Specify an accessor method for the WMI_SAMPLE_DEVICE_DATA structure.
//
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(STATION_TIMER_SCAN_CONTEXT, GetStationTimerContext)


typedef struct _STATION_WORKITEM_CONTEXT {

    PSTATION        Station;
}   STATION_WORKITEM_CONTEXT, *PSTATION_WORKITEM_CONTEXT;

//
// Specify an accessor method for the WMI_SAMPLE_DEVICE_DATA structure.
//
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(STATION_WORKITEM_CONTEXT, GetStationWorkItemContext)
    
/** 
 * Keeps tracks of the current state of association attempt by the driver. Note
 * the some states are set just before we start an action to avoid timing windows.
 */
typedef enum _STA_ASSOC_STATE
{
    /** 
     * Start state when we are not associated and have not started association process 
     */
    ASSOC_STATE_NOT_ASSOCIATED = 0,
    
    /** 
     * When set we are ready to start the association process and are either
     * about to start the association or have failed a previous association and 
     * would be restarting the association. Roaming and connect request code
     * are synchronized after this state.
     */
    ASSOC_STATE_READY_TO_ASSOCIATE,

    /** 
     * Have started the association process. This is set just after we have selected
     * an access point to start the association attempt. This would only be set for
     * the duration of the association process
     */
    ASSOC_STATE_STARTED_ASSOCIATION,
    
    /** 
     * Have asked the hardware functions to synchronize with the access point 
     * and are waiting for hardware functions to return
     */ 
    ASSOC_STATE_WAITING_FOR_JOIN,
    
    /**
     * Hardware functions have returned after successful synchronization
     * with the access point
     */
    ASSOC_STATE_JOINED,
    
    /** 
     * Special state set when we get deauthenticate packet from 
     * the access point while we still havent completed association. This is
     * not set on normal code path. It is used to ensure that we dont 
     * complete the association successfully even if we received a
     * deauth packet from the access point
     */
    ASSOC_STATE_REMOTELY_DEAUTHENTICATED,
    
    /** 
     * We are waiting for an authentication packet from the accesspoint.
     * Currently driver only uses open authentication or shared key authentication
     * so this is set when we are waiting for packet with sequence number 2 or 4
     */
    ASSOC_STATE_WAITING_FOR_AUTHENTICATE,

    /** 
     * Received successful authentication response from the access point
     */
    ASSOC_STATE_RECEIVED_AUTHENTICATE,
    
    /** 
     * Special state set when we get disassociate request from 
     * the access point while we still havent completed association. This
     * is again not set on normal code paths. It is used to ensure that
     * we dont complete the association successfully even if we received
     * a disassociate packet from the access point
     */
    ASSOC_STATE_REMOTELY_DISASSOCIATED,
    
    /** 
     * Waiting for Association response from the access point
     */
    ASSOC_STATE_WAITING_FOR_ASSOCIATE,
    
    /** 
     * Received successful association response from the access point 
     */
    ASSOC_STATE_RECEIVED_ASSOCIATE,
    
    /** 
     * Associated process completed successfully & status indicated. After this
     * is set, any disassociate/deauthenticate packet from the accesspoint
     * would case disassociation status indication
     */
    ASSOC_STATE_ASSOCIATED
    
} STA_ASSOC_STATE, *PSTA_ASSOC_STATE  ;

/** 
 * State of the connection attempt as per the operating system. This is 
 * used to keep track of what the operating system is expecting the driver
 * to do with regards to connection to an 802.11 network
 */
typedef enum _STA_CONNECT_STATE{
    /** 
     * Disconnected: Driver should not attempt to associate and if associated
     * should disconnect cleanly. This is set on receiving DISCONNECT OID and the
     * driver terminates any existing connection.
     */
    CONN_STATE_DISCONNECTED,

    /** 
     * In reset. Operating system is reseting the adapter (either by OID or 
     * NdisReset) or halting the adapter, etc. The driver waits
     * for on going connection to complete and then resets back to 
     * disconnected state
     */
    CONN_STATE_IN_RESET,

    /** 
     * Okay to connect. The operating system expects the driver to start connecting. The
     * driver would attempt to find and associate with candidate access point. This
     * is set on receiving CONNECT OID and is maintained until we have made a connection
     * attempt.
     */
    CONN_STATE_READY_TO_CONNECT,

    /** 
     * Okay to roam. The operating system expects the driver to stay connected a
     * and if connection is lost for some reason, to roam to a new access point.
     * In this state, any connection attempt is because we want to roam
     */
    CONN_STATE_READY_TO_ROAM
    
} STA_CONNECT_STATE, *PSTA_CONNECT_STATE;


// Forward declaration
typedef struct _STA_BSS_ENTRY STA_BSS_ENTRY, *PSTA_BSS_ENTRY;

/**
 * Holds information about BSS we have discovered.
 */
typedef struct _STA_BSS_LIST 
{
    /** 
     * Linked list of discovered BSS
     * This list must not be modified/read without acquiring the 
     * ListLock
     */
    LIST_ENTRY              List;

    /** 
     * Lock we need before we adding/removing entries from the 
     * discovered BSS list. This will be acquired for read by
     * routines that are not modifying the chain and acquired 
     * for write by routines that will be removing entries or
     * adding entries to the chain
     */
    NDIS_RW_LOCK            ListLock;

    /** Maximum number of BSS we will keep track of */
    ULONG                   MaxNumOfBSSEntries;

    /** Number of BSSs we have discovered */
    ULONG                   NumOfBSSEntries;

}STA_BSS_LIST, *PSTA_BSS_LIST;

typedef struct _STA_ADHOC_STA_INFO
{
    /** 
     * Linked list of discovered access points
     * This list must not be modified/read without acquiring the 
     * ListLock
     */
    LIST_ENTRY              StaList;
    ULONG                   StaCount;
    ULONG                   DeauthStaCount;

    /** 
     * Lock we need before we adding/removing entries from the 
     * discovered AP list. This will be acquired for read by
     * routines that are not modifying the chain and acquired 
     * for write by routines that will be removing entries or
     * adding entries to the chain
     */
    NDIS_RW_LOCK            StaListLock; 

    /** 
     * Lock for changing state, protecting connection variables, etc. 
     *
     * When acquiring both this lock and StaListLock, StaListLock 
     * must be acquired FIRST to ensure consistency throughout the code
     * and avoid deadlock
     */
    NDIS_SPIN_LOCK          StaInfoLock;

    ULONG                   AdHocState;
    
    /** Watchdog timer used in AdHoc mode */
    //NDIS_MINIPORT_TIMER     WatchdogTimer;
    WDFTIMER                WatchdogTimer;
    LONG                    TimerCounter;

    /** Connect work item */     
    WDFWORKITEM             ConnectWorkItem;
    //NDIS_HANDLE             ConnectWorkItem;
    
    /** For signalling the completion of a synchroneous join request */
    NDIS_EVENT              JoinCompletionEvent;

    BOOLEAN                 JoinAdHocAfterPowerResume;
} STA_ADHOC_STA_INFO, *PSTA_ADHOC_STA_INFO;

/**
 * Holds current configuration of the miniport. This information
 * is updated on OIDs request from the OS. One place this
 * is used is for selecting the access points for making a conneciton
 */
typedef struct _STA_CURRENT_CONFIG 
{
    /** BSS type configured by the OS */
    DOT11_BSS_TYPE          BSSType;

    /** SSID that we can associate with (we only support one SSID)
     */
    DOT11_SSID              SSID;

    /** List of MAC addresses we should not try to associate with */
    DOT11_MAC_ADDRESS       ExcludedMACAddressList[STA_EXCLUDED_MAC_ADDRESS_MAX_COUNT];
    ULONG                   ExcludedMACAddressCount;
    BOOLEAN                 IgnoreAllMACAddresses;

    /** Desired BSSID we should attempt to associate with */
    DOT11_MAC_ADDRESS       DesiredBSSIDList[STA_DESIRED_BSSID_MAX_COUNT];
    ULONG                   DesiredBSSIDCount;
    BOOLEAN                 AcceptAnyBSSID;

    /** PHY list */
    ULONG                   DesiredPhyList[STA_DESIRED_PHY_MAX_COUNT];
    ULONG                   DesiredPhyCount;
    ULONG                   ActivePhyId;

    /** PMKID list */
    DOT11_PMKID_ENTRY       PMKIDList[STA_PMKID_MAX_COUNT];
    ULONG                   PMKIDCount;

    /** Currently enabled authentication algorithm */
    DOT11_AUTH_ALGORITHM    AuthAlgorithm;  

    /** Currently enabled unicast cipher algorithm */
    DOT11_CIPHER_ALGORITHM  UnicastCipherAlgorithm;  

    /** Currently enabled multicast cipher algorithm */
    DOT11_CIPHER_ALGORITHM  MulticastCipherAlgorithmList[STA_MULTICAST_CIPHER_MAX_COUNT];
    ULONG                   MulticastCipherAlgorithmCount;
    DOT11_CIPHER_ALGORITHM  MulticastCipherAlgorithm;  

    /** Current setting of unreachable detection threshold */
    ULONG                   UnreachableDetectionThreshold;

    /** Current setting related to acceptance of unencrypted data */
    BOOLEAN                 ExcludeUnencrypted;
    PDOT11_PRIVACY_EXEMPTION_LIST   PrivacyExemptionList;

    /** Current association ID */
    BOOLEAN                 ValidAID;
    USHORT                  AID;

    /** Current Listen interval */
    USHORT                  ListenInterval;

    /** Current power saving level */
    ULONG                   PowerSavingLevel;

    /** Current IBSS parameters */
    BOOLEAN                 IBSSJoinOnly;
    PVOID                   AdditionalIEData;
    ULONG                   AdditionalIESize;

    /** Media streaming enabled or not */
    BOOLEAN                 MediaStreamingEnabled;

    /** Determines if we associate with an BSS which does not support any pairwise cipher */
    BOOLEAN                 UnicastUseGroupEnabled;

    /** Current hidden OID setting */
    BOOLEAN                 HiddenNetworkEnabled;

    /** check for use protection bit in beacon ERP IE after a successful connection or scan */
    BOOLEAN                 CheckForProtectionInERP;
}STA_CURRENT_CONFIG, *PSTA_CURRENT_CONFIG;

/**
 * Maintains station side statistics
 */
typedef struct _STA_STATS
{
    ULONGLONG               ullUcastWEPExcludedCount;
    ULONGLONG               ullMcastWEPExcludedCount;
    
}STA_STATS, *PSTA_STATS;

/**
 * Connection context structure used during infrastructure connection attempt
 */
typedef struct _STA_INFRA_CONNECT_CONTEXT
{
    /** 
     * Used to keep track of number of asynchronous functions pending to
     * complete the connection attempt. This is used in CONN_STATE_IN_RESET 
     * to wait for the connection process to complete before reseting the
     * adapter. Reset waits for this to go to zero. This structure is 
     * modified using interlocked operations.
     */
    LONG                    AsyncFuncCount;

    /**
     * Different events can cause the driver to reset its connection attempt.
     * This mutex is used to serialize multiple simultaneous reset/cleanup
     * routines
     */
    NDIS_MUTEX              DisconnectGate;
    
    /** 
     * Lock for changing state, protecting connection variables, etc. 
     * This is used for connecting and during scanning
     *
     * When acquiring both this lock and lock on the AP entry, the AP entry 
     * lock must be acquired second to ensure consistency throughout the code
     * and avoid deadlock
     */
    NDIS_SPIN_LOCK          Lock;

    /** 
     * Current association state of the station. This variable is 
     * normally modified by the various association routines. The Reset/Disconnect
     * routines will modify this once they are sure no association routine is using
     * this. The Connect routine would initialize this. This is modified
     * with the Lock held
     */
    STA_ASSOC_STATE         AssociateState;

#ifndef USE_NWIFI_SPEC_1_66
    /**
     * DeAuthReason value is valid only when AssociateState == ASSOC_STATE_REMOTELY_DEAUTHENTICATED
     */
    USHORT                  DeAuthReason;
#endif

    /** 
     * This field is only meaningful when AssociateState is 
     * ASSOC_STATE_WAITING_FOR_AUTHENTICATE. It specifies the sequence 
     * number of the authenticate response we are expecting.
     */
    USHORT                  ExpectedAuthSeqNumber;

    /** 
     * Tracks whether or not we are allowed to connect. This is only 
     * modified by the Connect/Reset/Disconnect routines. The various
     * functions that handle associate would read this to check if they 
     * should continue with the association. This is modified
     * with the Lock held
     */
    STA_CONNECT_STATE       ConnectState;

    /** 
     * BSSEntry that we are currently using for association, are associated with. 
     * This is modified with the Lock held
     */
    PSTA_BSS_ENTRY          ActiveAP;
    
    /**
     * The candidate access points we will attempt to associate with.
     */
    PSTA_BSS_ENTRY          CandidateAPList[STA_CANDIDATE_AP_MAX_COUNT];
    ULONG                   CandidateAPCount;

    /** Index of the candidate AP we are currently associating with (starts with 0) */
    ULONG                   CurrentCandidateAPIndex;

    /**
     * Buffer to use for association completion. We will preallocate this
     * buffer to ensure that we are able to indicate completion even in case
     * of low resources. We only use this for signalling association completion
     * in case we run low on resources
     */
    PUCHAR                  pAssocFailBuffer;

    /** 
     * Timer object used when waiting for authentication process to complete.
     * The timer will be set when the authentication process begins
     */
    //NDIS_MINIPORT_TIMER     Timer_AuthenticateTimeout;
    WDFTIMER                Timer_AuthenticateTimeout;
    /** 
     * Timer object used when waiting for association process to complete.
     * The timer will be set when the association process begins
     */
    //NDIS_MINIPORT_TIMER     Timer_AssociateTimeout;
    WDFTIMER                Timer_AssociateTimeout;
    
    BOOLEAN                 RoamForSendFailures;

    /** indicate link quality to the OS */
    BOOLEAN                 UpdateLinkQuality;
}STA_INFRA_CONNECT_CONTEXT, *PSTA_INFRA_CONNECT_CONTEXT;



/**
 * State we maintain for holding 
 */
typedef struct _STA_SCAN_CONTEXT
{
    /** Scan related data. (May be grouped together later) */
    PVOID                   ScanRequestID;

    /** Scan request buffer for OID request from OS */
    PDOT11_SCAN_REQUEST_V2  ExternalScanRequestBuffer;

    /** Timer we use for periodic scanning */
    //NDIS_MINIPORT_TIMER     Timer_PeriodicScan;
    WDFTIMER                Timer_PeriodicScan;
    
    /** Scan request buffer we use for internal scan request */
    PDOT11_SCAN_REQUEST_V2  InternalScanRequestBuffer;

    /** Length of above buffer */
    ULONG                   InternalScanBufferLength;

    /** Flags that keep track of scan state */
    ULONG                   Flags;

    /**
     * Number of times that the periodic scan timer has run, but
     * did has not done any scanning
     */
    ULONG                   PeriodicScanCounter;

    /**
     * Number of ticks since are have performed a periodic scan
     * for roaming/connectiong purposes
     */
    ULONG                   RoamingScanGap;

    /** The time at which we started the last scan (periodic or OS scan) */
    ULONGLONG               LastScanTime;

    /**
     * Set to true to indicate that we are scanning for roaming
     */
    BOOLEAN                 ScanForRoam;

    /**
     * Set to true when we need to roam now or we have a problem
     */
    BOOLEAN                 MustRoam;

    /** 
     * This is generally true and we would be adding the SSID of the network in
     * our probe requests. When we go to sleep, this becomes false, causing us to 
     * stop putting the SSID in our probe requests until we wake up
     */
    BOOLEAN                 SSIDInProbeRequest;


}STA_SCAN_CONTEXT, *PSTA_SCAN_CONTEXT;

typedef struct _STA_PMKID_CACHE {
    
    /** The time at which we checked for PMKID candidate */
    ULONGLONG               CheckingTime;

    /** Number of PMKID candidates indicated last time */
    ULONG                   Count;

    /** PMKID candidates indicated last time */
    DOT11_BSSID_CANDIDATE   Candidate[STA_PMKID_MAX_COUNT];

} STA_PMKID_CACHE, PSTA_PMKID_CACHE;

/** State of the station */
#define STA_RESET_IN_PROGRESS       0x00000001

/**
 * State read from the registry
 */
typedef struct _STA_REG_INFO 
{ 
    /** Number of low quality beacons before we roam */
    ULONG                   RSSIRoamBeaconCount;

    /** Link quality for low threshold */
    ULONG                   RSSILinkQualityThreshold;

    /** Number of missed beacons before we roam */
    ULONG                   LostAPRoamBeaconCount;

    /** Time in 100ns, to hold a BSS entry even after beacons are missed */
    ULONG                   BSSEntryExpireTime;

    /** Max number of BSS entries to cache */
    ULONG                   BSSEntryMaxCount;
    
    /** Max number of adhoc station entries to maintain */
    ULONG                   AdhocStationMaxCount;
    
    /** Data transmission failure in percentage for reducing Tx rate */
    ULONG                   FailureThresholdForTxRateFallback;

    /** Data transmission failure in percentage for increasing Tx rate */
    ULONG                   FailureThresholdForTxRateIncrease;

    /** Data transmission failure in percentage for device to roam */
    ULONG                   FailureThresholdForRoam;
} STA_REG_INFO, *PSTA_REG_INFO;

#define STA_OFFSET(field)   ((UINT)FIELD_OFFSET(STA_REG_INFO,field))
#define STA_SIZE(field)     sizeof(((PSTA_REG_INFO)0)->field)


/**
 * Maintains all our 802.11 station specific state such as discovered APs,
 * current AP, etc
 */
typedef struct _STATION
{
    /** Pointer to the associated Adapter instance */
    PADAPTER                pAdapter;

    /** The handle to be passed to NDIS when calling functions */
    NDIS_HANDLE             MiniportAdapterHandle;  

    /** Pointer to the associated Nic instance */
    PNIC                    pNic;

    /** Flag to check if we are in reset */
    ULONG                   Flags;
    
    /** Information about BSS */
    STA_BSS_LIST            BSSList;

    /** State from the registry */
    STA_REG_INFO            RegInfo;

    /** Information about AdHoc stations */
    STA_ADHOC_STA_INFO      AdHocStaInfo;

    /** Current configuration of extensible station */
    STA_CURRENT_CONFIG      Config;

    /** Connection request context information */
    STA_INFRA_CONNECT_CONTEXT     ConnectContext;

    /** State we maintaining scan state */
    STA_SCAN_CONTEXT        ScanContext;

    /** Current PMKID cache the station maintains */
    STA_PMKID_CACHE         PMKIDCache;

    /** Statistics information */
    STA_STATS               Statistics;

} STATION, *PSTATION;

VOID 
StaIndicateDot11Status(
    __in  PSTATION        pStation,
    __in  NDIS_STATUS     StatusCode,
    __in_opt  PVOID           RequestID,
    __in  PVOID           pStatusBuffer,
    __in  ULONG           StatusBufferSize
    );

VOID
StaInitializeStationConfig(
    __in PSTATION         pStation
    );


VOID
StaResetStationConfig(
    __in PSTATION         pStation
    );

VOID
StaSetDefaultAuthAlgo(
    __in PSTATION pStation
    );

VOID
StaSetDefaultCipher(
    __in PSTATION pStation
    );

#endif // _STATION_MAIN_H

