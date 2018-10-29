/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    ap_assocmgr.h

Abstract:
    ExtAP association definitions
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    09-10-2007    Created
    
Notes:

--*/
#pragma once
    
#ifndef _AP_ASSOCMGR_H
#define _AP_ASSOCMGR_H

/** Forward declarations */
typedef struct _MP_EXTAP_PORT      MP_EXTAP_PORT, *PMP_EXTAP_PORT;

/** Maximum number of stations we will cache state about */
#define AP_STA_MAX_ENTRIES_DEFAULT              32
#define AP_STA_MAX_ENTRIES_MIN                  16
#define AP_STA_MAX_ENTRIES_MAX                  64

/** Time to wait for association request from station after auth success (in number of milliseconds) */
#define AP_ASSOCIATION_REQUEST_TIMEOUT          250

/** Time in which the station has no activity we assume we have lost connectivity to that station (in number of seconds) */
#define AP_NO_ACTIVITY_TIME                     1800
/** The interval of the station inactive timer, in milliseconds */
#define AP_STA_INACTIVE_TIMER_INTERVAL          1000        

#define AP_MAX_AID                              2007
#define AP_INVALID_AID                          0xFFFF
#define AP_AID_TABLE_UNIT_SIZE                  8          
#define AP_AID_TABLE_UNIT_MASK                  0xFF
#define AP_AID_TABLE_SIZE                       (AP_MAX_AID/AP_AID_TABLE_UNIT_SIZE + 1)
#define AP_AID_HEADER                           0xC000

extern DOT11_MAC_ADDRESS Dot11BroadcastAddress;

/** Association Manager state */
typedef enum _AP_ASSOC_MGR_STATE
{
    AP_ASSOC_MGR_STATE_NOT_INITIALIZED = 0,
    AP_ASSOC_MGR_STATE_STARTING,
    AP_ASSOC_MGR_STATE_STARTED,
    AP_ASSOC_MGR_STATE_STOPPING,
    AP_ASSOC_MGR_STATE_STOPPED
} AP_ASSOC_MGR_STATE, *PAP_ASSOC_MGR_STATE;

/**
 * ExtAP association manager
 */
typedef struct _AP_ASSOC_MGR
{
    /** ExtAP port */
    PMP_EXTAP_PORT          ApPort;
    
    /** state of Association Manager */
    AP_ASSOC_MGR_STATE      State;
    
    /**
     * Hash table for the stations
     */
    MAC_HASH_TABLE          MacHashTable;

    /** 
     * Lock we need before we adding/removing entries from the 
     * hash table. This will be acquired for read by
     * routines that are not modifying the table and acquired 
     * for write by routines that will be removing entries or
     * adding entries to the table.
     */
    MP_READ_WRITE_LOCK      MacHashTableLock;

    /** AID table, a bit for each AID */
    UCHAR                   AidTable[AP_AID_TABLE_SIZE];
    
    /** 
     * Association related configurations
     * A lock is NOT required when updating/querying these configurations
     */

    /** SSID that we advertise (we only support one SSID) */
    DOT11_SSID              Ssid;

    /** AP BSSID */
    DOT11_MAC_ADDRESS       Bssid;

    /** Capability information */
    DOT11_CAPABILITY        Capability;
    
    /** Currently enabled authentication algorithm */
    DOT11_AUTH_ALGORITHM    AuthAlgorithm;  

    /** Currently enabled unicast cipher algorithm */
    DOT11_CIPHER_ALGORITHM  UnicastCipherAlgorithm;  

    /** Currently enabled multicast cipher algorithm */
    DOT11_CIPHER_ALGORITHM  MulticastCipherAlgorithm;  

    /** Use default auth cipher algorithms **/
    BOOLEAN bUseDefaultAlgorithms;

    /** Operational rate set */
    DOT11_RATE_SET          OperationalRateSet;

    /** Current setting related to acceptance of unencrypted data */
    BOOLEAN                 ExcludeUnencrypted;
    PDOT11_PRIVACY_EXEMPTION_LIST   PrivacyExemptionList;

    /** Enable WPS */
    BOOLEAN                 EnableWps;

    /** Beacon Mode */
    BOOLEAN                 BeaconEnabled;

    /** Scan related data */
    /** Scan in process */
    LONG                    ScanInProcess;

    /** Local copy of scan request */
    MP_SCAN_REQUEST         ScanRequest;
    
    /** Scan request ID */
    PVOID                   ScanRequestId;
    
    /** 
     * Station inactive timer 
     * When the timer fires, the inactive time of each
     * station is incremented by 1.
     */
    NDIS_HANDLE              StaInactiveTimer;

    /**
     * This is actually the count of the associated
     * stations because each station is going to 
     * increase the counter by 1 when it is associated
     * and decrement by 1 when it is disassociated.
     */
    LONG                    StaInactiveTimerCounter;

    /** For signalling the completion of a synchronous start request */
    NDIS_STATUS             StartBSSCompletionStatus;
    NDIS_EVENT              StartBSSCompletionEvent;
    NDIS_EVENT              StopBSSCompletionEvent;

    NDIS_STATUS             SetChannelCompletionStatus;
    NDIS_EVENT              SetChannelCompletionEvent;
         
} AP_ASSOC_MGR, *PAP_ASSOC_MGR;

/**
 * Station port state
 */
typedef enum _STA_PORT_STATE
{
    STA_PORT_STATE_INVALID = 0,
    STA_PORT_STATE_OPEN,
    STA_PORT_STATE_CLOSED
} STA_PORT_STATE, *PSTA_PORT_STATE;

/**
 * Tracks the state of a station
 */
typedef struct _AP_STA_ENTRY
{
    /** 
     * MAC hash entry.
     * This is used for hash table operations.
     */
    MAC_HASH_ENTRY          MacHashEntry;

    /** 
     * Pointer to the association manager
     * where the station is managed.
     */
    PAP_ASSOC_MGR           AssocMgr;
    
    /** Capability information */
    DOT11_CAPABILITY        CapabilityInformation;

    /** Listen interval */
    USHORT                  ListenInterval;

    /** Supported rates */
    DOT11_RATE_SET          SupportedRateSet;
    
    /** Current association state of the station */
    DOT11_ASSOCIATION_STATE AssocState;

    /** Current association ID */
    USHORT                  Aid;

    /** Power mode */
    DOT11_POWER_MODE        PowerMode;
    
    /** Auth algorithm */
    DOT11_AUTH_ALGORITHM    AuthAlgo;

    /** Unicast cipher algorithm */
    DOT11_CIPHER_ALGORITHM  UnicastCipher;

    /** Multicast cipher algorithm */
    DOT11_CIPHER_ALGORITHM  MulticastCipher;

    /** WPS enabled */
    BOOLEAN                 WpsEnabled;
    
    /** Association timer */
    NDIS_HANDLE             AssocTimer;

    /** Waiting for association request */
    LONG                    WaitingForAssocReq;

    /** 
     * Association up time, i.e. timestamp at which association is completed with success.
     * Timestamp value is returned by NdisGetCurrentSystemTime
     */
    LARGE_INTEGER           AssocUpTime;

    /** Statistics */
    DOT11_PEER_STATISTICS   Statistics;

    /** 
     * Station reference count. 
     * Indicate the number of external functions 
     * that are accessing the station entry.
     * The reference count is 1 when an entry is created.
     * It is deleted when the reference count reaches zero.
     */
    LONG                    RefCount;

#if 0
    /** Buffer for association complete indication */
    PDOT11_INCOMING_ASSOC_COMPLETION_PARAMETERS AssocCompletePara;
    ULONG                   AssocCompleteParaSize;

    /** Received association request */
    PDOT11_MGMT_HEADER      AssocReqFrame;
    USHORT                  AssocReqFrameSize;
#endif
    /** Association decision */
    BOOLEAN                 AcceptAssoc;
    USHORT                  Reason;

    /** 
     * Inactive time. 
     * Indicates how long the station has been inactive, in seconds.
     */
    LONG                    InactiveTime;
    
    /** 
     * Port state
     * This is used to decide whether a non-forced scan
     * shall be allowed or not.
     */
    STA_PORT_STATE          PortState; 
} AP_STA_ENTRY, *PAP_STA_ENTRY;

/** Association Manager Functions invoked by other components */

/**
 * The following functions are defined for upper layer components.
 * Caller must handle synchronization.
 */
 
/** Initialize Association Manager */
NDIS_STATUS
ApInitializeAssocMgr(
    __in PAP_ASSOC_MGR AssocMgr,
    __in PMP_EXTAP_PORT ApPort
    );

/** Deinitialize Association Manager */
VOID
ApDeinitializeAssocMgr(
    __in PAP_ASSOC_MGR AssocMgr
    );

/** Start Association Manager */
NDIS_STATUS
ApStartAssocMgr(
    __in PAP_ASSOC_MGR AssocMgr
    );

/**
 * Stop Association Manager
 * 1. Stop accepting new association requests
 * 2. Disassociate all stations and send corresponding indications
 * 3. Doesn't have to wait for the pending association request decision from IM
 * 4. Cancel on-going scan and wait for it to complete
 */
VOID
ApStopAssocMgr(
    __in PAP_ASSOC_MGR AssocMgr
    );

/** Restart Association Manager */
NDIS_STATUS
ApRestartAssocMgr(
    __in PAP_ASSOC_MGR AssocMgr
    );

/**
 * Pause Association Manager
 * 1. Cancel on-going scan and wait for it to complete
 */
NDIS_STATUS
ApPauseAssocMgr(
    __in PAP_ASSOC_MGR AssocMgr
    );

/** Internal functions called by other association manager functions */

/**
 * Station entry related functions
 */

/**
 * Allocate a station entry
 */
NDIS_STATUS
AmAllocateStaEntry(
    __in const PAP_ASSOC_MGR AssocMgr,
    __in const DOT11_MAC_ADDRESS * StaMacAddr,
    __deref_out PAP_STA_ENTRY * StaEntry
    );

/**
 * Free a station entry
 */
VOID
FORCEINLINE
AmFreeStaEntry(
    __in PAP_STA_ENTRY StaEntry
    )
{
    if (StaEntry->AssocTimer)
    {
        NdisFreeTimerObject(StaEntry->AssocTimer);
        StaEntry->AssocTimer = NULL;
    }
    
    MP_FREE_MEMORY(StaEntry);
}

/** 
 * Reference a station entry 
 * Must be called when the station is still in the MAC table.
 * The caller must synchronize the access.
 */
LONG
FORCEINLINE
ApRefSta(
    __in PAP_STA_ENTRY StaEntry
    )
{
    return InterlockedIncrement(&StaEntry->RefCount);
}

/** 
 * Dereference a station entry 
 * Can be called anywhere. The caller must ensure the reference count is 
 * greater than 0.
 */
LONG
FORCEINLINE
ApDerefSta(
    __in PAP_STA_ENTRY StaEntry
    )
{
    LONG refCount = InterlockedDecrement(&StaEntry->RefCount);

    if (0 == refCount)
    {
        AmFreeStaEntry(StaEntry);
    }

    return refCount;
}

/** 
 * Get scan state.
 */
LONG
FORCEINLINE
ApGetScanState(
    __in PAP_ASSOC_MGR AssocMgr
    )
{
    return InterlockedExchangeAdd(
                &AssocMgr->ScanInProcess,
                0
                );
}

/** 
 * Set scan state.
 */
LONG
FORCEINLINE
ApSetScanState(
    __in PAP_ASSOC_MGR AssocMgr,
    __in LONG NewScanState
    )
{
    return InterlockedExchange(
                &AssocMgr->ScanInProcess,
                NewScanState
                );
}

/** 
 * Get the association state of a station.
 */
DOT11_ASSOCIATION_STATE
FORCEINLINE
ApGetStaAssocState(
    __in PAP_STA_ENTRY StaEntry
    )
{
    return (DOT11_ASSOCIATION_STATE)InterlockedExchangeAdd(
                (LONG *)&StaEntry->AssocState,
                0
                );
}

/** 
 * Set the association state of a station.
 */
DOT11_ASSOCIATION_STATE
FORCEINLINE
ApSetStaAssocState(
    __in PAP_STA_ENTRY StaEntry,
    __in DOT11_ASSOCIATION_STATE NewAssocState
    )
{
    return (DOT11_ASSOCIATION_STATE)InterlockedExchange(
                (LONG *)&StaEntry->AssocState,
                (LONG)NewAssocState
                );
}

/** 
 * Get the port state of a station.
 */
STA_PORT_STATE
FORCEINLINE
ApGetStaPortState(
    __in PAP_STA_ENTRY StaEntry
    )
{
    return (STA_PORT_STATE)InterlockedExchangeAdd(
                (LONG *)&StaEntry->PortState,
                0
                );
}

/** 
 * Set the port state of a station.
 */
STA_PORT_STATE
FORCEINLINE
ApSetStaPortState(
    __in PAP_STA_ENTRY StaEntry,
    __in STA_PORT_STATE NewPortState
    )
{
    return (STA_PORT_STATE)InterlockedExchange(
                (LONG *)&StaEntry->PortState,
                (LONG)NewPortState
                );
}

/**
 * Reset station inactive time.
 * Return the original inactive time.
 */
LONG
FORCEINLINE
ApResetStaInactiveTime(
    __in PAP_STA_ENTRY StaEntry
    )
{
    return InterlockedExchange(
            &StaEntry->InactiveTime,
            0
            );
}

/**
 * Increment station inactive time by 1.
 * Return the original inactive time.
 */
LONG
FORCEINLINE
ApIncrementStaInactiveTime(
    __in PAP_STA_ENTRY StaEntry
    )
{
    return InterlockedIncrement(
            &StaEntry->InactiveTime
            );
}

/** 
 * Check and set the association state of a station.
 * The new state is set only if the old state matches
 * the given state.
 */
DOT11_ASSOCIATION_STATE
FORCEINLINE
ApCheckAndSetStaAssocState(
    __in PAP_STA_ENTRY StaEntry,
    __in DOT11_ASSOCIATION_STATE NewAssocState,
    __in DOT11_ASSOCIATION_STATE OldAssocState
    )
{
    return (DOT11_ASSOCIATION_STATE)InterlockedCompareExchange(
                (LONG *)&StaEntry->AssocState,
                (LONG)NewAssocState,
                (LONG)OldAssocState
                );
}

/**
 * Association manager related functions
 */

/** 
 * Set association manager to its default based on the hardware capability 
 * and registry settings
 */
VOID
AmSetDefault(
    __in PAP_ASSOC_MGR AssocMgr
    );

/**
 * Set default cipher based on the auth algorithm and hardware capability
 */
VOID
AmSetDefaultCipher(
    __in PAP_ASSOC_MGR AssocMgr
    );

/**
 * Set default auth and cipher algorithms in VNIC
 */
VOID
AmSetDefaultAuthAndCipherAlgorithms
(
    __in PAP_ASSOC_MGR AssocMgr
    );


/** 
 * Clean up Association Manager Privacy Exemption List
 */
VOID
AmCleanupPrivacyExemptionList(
    __in PAP_ASSOC_MGR AssocMgr
    );


/** 
 * Clean up Association Manager Desired SSID List
 */
VOID
AmCleanupDesiredSsidList(
    __in PAP_ASSOC_MGR AssocMgr
    );


/** 
 * Clean up Association Manager 
 * Shall be called after ApStopAssocMgr()
 * All clients must have been disassociated at this point
 */
VOID
AmCleanup(
    __in PAP_ASSOC_MGR AssocMgr
    );

/**
 * Internal functions for OIDs
 */
NDIS_STATUS
AmQuerySsid(
    __in PAP_ASSOC_MGR AssocMgr,
    __out PDOT11_SSID_LIST SsidList,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

NDIS_STATUS
AmSetSsid(
    __in PAP_ASSOC_MGR AssocMgr,
    __in PDOT11_SSID_LIST SsidList
    );

VOID
FORCEINLINE    
AmQueryBssid(
    __in PAP_ASSOC_MGR AssocMgr,
    __out DOT11_MAC_ADDRESS * Bssid
    )
{
    MPASSERT(AssocMgr->State != AP_ASSOC_MGR_STATE_NOT_INITIALIZED);
    
    RtlCopyMemory(
            *Bssid,
            AssocMgr->Bssid,
            sizeof(DOT11_MAC_ADDRESS)
            );
}

NDIS_STATUS
AmSetBssid(
    __in PAP_ASSOC_MGR AssocMgr,
    __in DOT11_MAC_ADDRESS * Bssid
    );

NDIS_STATUS
AmQueryAuthAlgorithm(
    __in PAP_ASSOC_MGR AssocMgr,
    __out PDOT11_AUTH_ALGORITHM_LIST EnabledAuthenticationAlgorithm,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

NDIS_STATUS
AmSetAuthAlgorithm(
    __in PAP_ASSOC_MGR AssocMgr,
    __in PDOT11_AUTH_ALGORITHM_LIST EnabledAuthenticationAlgorithm
    );

NDIS_STATUS
AmQueryUnicastCipherAlgorithm(
    __in PAP_ASSOC_MGR AssocMgr,
    __out PDOT11_CIPHER_ALGORITHM_LIST EnabledUnicastCipherAlgorithm,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

NDIS_STATUS
AmSetUnicastCipherAlgorithm(
    __in PAP_ASSOC_MGR AssocMgr,
    __in PDOT11_CIPHER_ALGORITHM_LIST EnabledUnicastCipherAlgorithm
    );

NDIS_STATUS
AmQueryMulticastCipherAlgorithm(
    __in PAP_ASSOC_MGR AssocMgr,
    __out PDOT11_CIPHER_ALGORITHM_LIST EnabledMulticastCipherAlgorithm,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

NDIS_STATUS
AmSetMulticastCipherAlgorithm(
    __in PAP_ASSOC_MGR AssocMgr,
    __in PDOT11_CIPHER_ALGORITHM_LIST EnabledMulticastCipherAlgorithm
    );

VOID
FORCEINLINE
AmQueryOperationalRateSet(
    __in PAP_ASSOC_MGR AssocMgr,
    __out PDOT11_RATE_SET OperationalRateSet
    )
{
    MPASSERT(AssocMgr->State != AP_ASSOC_MGR_STATE_NOT_INITIALIZED);
    
    RtlCopyMemory(
            OperationalRateSet,
            &AssocMgr->OperationalRateSet,
            sizeof(DOT11_RATE_SET)
            );
}

NDIS_STATUS
AmSetOperationalRateSet(
    __in PAP_ASSOC_MGR AssocMgr,
    __in PDOT11_RATE_SET OperationalRateSet
    );

VOID
FORCEINLINE
AmQueryExcludeUnencrypted(
    __in PAP_ASSOC_MGR AssocMgr,
    __out BOOLEAN * ExcludeUnencrypted
    )
{
    *ExcludeUnencrypted = AssocMgr->ExcludeUnencrypted;
}

NDIS_STATUS
AmSetExcludeUnencrypted(
    __in PAP_ASSOC_MGR AssocMgr,
    __in BOOLEAN ExcludeUnencrypted
    );

NDIS_STATUS
AmQueryPrivacyExemptionList(
    __in PAP_ASSOC_MGR AssocMgr,
    __out PDOT11_PRIVACY_EXEMPTION_LIST PrivacyExemptionList,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

NDIS_STATUS
AmSetPrivacyExemptionList(
    __in PAP_ASSOC_MGR AssocMgr,
    __in PDOT11_PRIVACY_EXEMPTION_LIST PrivacyExemptionList
    );

VOID
FORCEINLINE    
AmQueryWpsEnabled(
    __in PAP_ASSOC_MGR AssocMgr,
    __out BOOLEAN * WpsEnabled
    )
{
    MPASSERT(AssocMgr->State != AP_ASSOC_MGR_STATE_NOT_INITIALIZED);
    
    *WpsEnabled = AssocMgr->EnableWps;
}

NDIS_STATUS
AmSetWpsEnabled(
    __in PAP_ASSOC_MGR AssocMgr,
    __in BOOLEAN WpsEnabled
    );


VOID
FORCEINLINE    
AmQueryApBeaconMode(
    __in PAP_ASSOC_MGR AssocMgr,
    __out BOOLEAN * BeaconEnabled
    )
{
    MPASSERT(AssocMgr->State != AP_ASSOC_MGR_STATE_NOT_INITIALIZED);
    
    *BeaconEnabled = AssocMgr->BeaconEnabled;
}

NDIS_STATUS
AmSetApBeaconMode(
    __in PAP_ASSOC_MGR AssocMgr,
    __in BOOLEAN BeaconEnabled
    );


NDIS_STATUS
AmEnumPeerInfo(
    __in PAP_ASSOC_MGR AssocMgr,
    __out PDOT11_PEER_INFO_LIST PeerInfo,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

NDIS_STATUS
AmDisassociatePeerRequest(
    __in PAP_ASSOC_MGR AssocMgr,
    __in PDOT11_MAC_ADDRESS PeerMacAddr,
    __in USHORT Reason
    );

NDIS_STATUS
AmSetStaPortState(
    __in PAP_ASSOC_MGR AssocMgr,
    __in PDOT11_MAC_ADDRESS PeerMacAddr,
    __in BOOLEAN PortOpen
    );

NDIS_STATUS
AmScanRequest(
    __in PAP_ASSOC_MGR AssocMgr,
    __in PVOID ScanRequestId,
    __in PDOT11_SCAN_REQUEST_V2 ScanRequest,
    __in ULONG ScanRequestBufferLength
    );

/*
 * This function is called inside a direct call.
 * The caller must ensure the AP port stays valid.
 * The function must handle potential concurrent accesses
 * to shared resources.
 */
NDIS_STATUS
AmAssociationDecision(
    __in PAP_ASSOC_MGR AssocMgr,
    __in PDOT11_INCOMING_ASSOC_DECISION AssocDecision
    );

/*
 * Process the association decision made by OS.
 */
NDIS_STATUS
AmProcessAssociationDecision(
    __in PAP_ASSOC_MGR AssocMgr,
    __in BOOLEAN reAssociation,
    __in_bcount(AssocReqFrameSize) PDOT11_MGMT_HEADER AssocReqFrame,
    __in USHORT AssocReqFrameSize,
    __in PDOT11_INCOMING_ASSOC_COMPLETION_PARAMETERS AssocCompletePara,
    __in ULONG AssocCompleteParaSize
    );

/**
 * Internal functions for station management
 */

/**
 * Processing station authentication frame
 */
VOID 
AmProcessStaAuthentication(
    __in PAP_ASSOC_MGR AssocMgr,
    __in_bcount(PacketSize) PDOT11_MGMT_HEADER MgmtPacket,
    __in USHORT PacketSize
    );

/**
 * Processing station deauthentication frame
 */
VOID 
AmProcessStaDeauthentication(
    __in PAP_ASSOC_MGR AssocMgr,
    __in_bcount(PacketSize) PDOT11_MGMT_HEADER MgmtPacket,
    __in __range(DOT11_MGMT_HEADER_SIZE + sizeof(DOT11_DEAUTH_FRAME), USHORT_MAX) USHORT PacketSize
    );

/**
 * Processing station association request frame
 */
VOID 
AmProcessStaAssociation(
    __in PAP_ASSOC_MGR AssocMgr,
    __in BOOLEAN reAssociation,
    __in_bcount(PacketSize) PDOT11_MGMT_HEADER MgmtPacket,
    __in __range(DOT11_MGMT_HEADER_SIZE + sizeof(DOT11_REASSOC_REQUEST_FRAME), USHORT_MAX)USHORT PacketSize
    );


/**
 * Processing station disassociation frame
 */
VOID 
AmProcessStaDisassociation(
    __in PAP_ASSOC_MGR AssocMgr,
    __in_bcount(PacketSize) PDOT11_MGMT_HEADER MgmtPacket,
    __in __range(DOT11_MGMT_HEADER_SIZE + sizeof(DOT11_DISASSOC_FRAME), USHORT_MAX) USHORT PacketSize
    );

/**
 * Association related management frames
 */

/** Create authentication response frame */
NDIS_STATUS
AmCreateAuthRespFrame(
    __in PAP_ASSOC_MGR AssocMgr,
    __in_opt PAP_STA_ENTRY StaEntry,
    __in PDOT11_MGMT_HEADER ReceivedMgmtPacket,
    __in USHORT ReceivedPacketSize,
    __in USHORT StatusCode,
    __deref_out_bcount(*AuthRespFrameSize) PUCHAR * AuthRespFrame,
    __out PUSHORT AuthRespFrameSize
    );
    
/** Create association/reassociation response frame */
NDIS_STATUS
AmCreateAssocRespFrame(
    __in PAP_ASSOC_MGR AssocMgr,
    __in PAP_STA_ENTRY StaEntry,
    __in BOOLEAN reAssociation,
    __in PDOT11_MGMT_HEADER ReceivedMgmtPacket,
    __in USHORT ReceivedPacketSize,
    __in USHORT StatusCode,
    __deref_out_bcount(*AssocRespFrameSize) PUCHAR * AssocRespFrame,
    __out PUSHORT AssocRespFrameSize
    );

/** Create disassociation frame */
NDIS_STATUS
AmCreateDisassocFrame(
    __in PAP_ASSOC_MGR AssocMgr,
    __in_bcount(DOT11_ADDRESS_SIZE) PDOT11_MAC_ADDRESS DestAddr,
    __in USHORT ReasonCode,
    __deref_out_bcount(*DisassocFrameSize) PUCHAR * DisassocFrame,
    __out PUSHORT DisassocFrameSize
    );

/** Create deauthentication frame */
NDIS_STATUS
AmCreateDeauthFrame(
    __in PAP_ASSOC_MGR AssocMgr,
    __in_bcount(DOT11_ADDRESS_SIZE) PDOT11_MAC_ADDRESS DestAddr,
    __in USHORT ReasonCode,
    __deref_out_bcount(*DeauthFrameSize) PUCHAR * DeauthFrame,
    __out PUSHORT DeauthFrameSize
    );

/** 
 * Send disassociation frame. 
 * Shall not invoke it in a lock.
 */
NDIS_STATUS
AmSendDisassociationFrame(
    __in PAP_ASSOC_MGR AssocMgr,
    __in_bcount(DOT11_ADDRESS_SIZE) PDOT11_MAC_ADDRESS DestAddr,
    __in USHORT ReasonCode
    );

/** 
 * Send deauthentication frame. 
 * Shall not invoke it in a lock.
 */
NDIS_STATUS
AmSendDeauthenticationFrame(
    __in PAP_ASSOC_MGR AssocMgr,
    __in_bcount(DOT11_ADDRESS_SIZE) PDOT11_MAC_ADDRESS DestAddr,
    __in USHORT ReasonCode
    );

/** 
 * Disassociate a station. 
 * The caller must ensure the station entry is valid during this call.
 * The caller should not hold any lock.
 */
VOID
AmDisassociateSta(
    __in PAP_ASSOC_MGR AssocMgr,
    __in PAP_STA_ENTRY StaEntry,
    __in ULONG Reason
    );

/** 
 * Deauthenicate a station. 
 * This shall be called only after a station 
 * is authenticated.
 * The station entry must be removed from the
 * Mac table already.
 * So this function will be invoked only once
 * for each station entry.
 */
VOID
AmDeauthenticateSta(
    __in PAP_ASSOC_MGR AssocMgr,
    __in PAP_STA_ENTRY StaEntry,
    __in ULONG Reason,
    __in BOOLEAN SendDeauthFrame,
    __in USHORT Dot11Reason 
    );

/** Supporting functions for NDIS indications */

/** Allocate association completion parameters */
NDIS_STATUS
AmAllocAssocCompletePara(
    __in PAP_ASSOC_MGR AssocMgr,
    __in USHORT AssocReqFrameSize,
    __in USHORT AssocRespFrameSize,
    __deref_out PDOT11_INCOMING_ASSOC_COMPLETION_PARAMETERS * AssocCompletePara,
    __out PULONG AssocCompleteParaSize
    );

/** Allocate  association request received parameters */
NDIS_STATUS
AmAllocAssocReqPara(
    __in PAP_ASSOC_MGR AssocMgr,
    __in USHORT AssocReqFrameSize,
    __deref_out PDOT11_INCOMING_ASSOC_REQUEST_RECEIVED_PARAMETERS * AssocReqPara,
    __out PULONG AssocReqParaSize
    );

/**
 * Prepare information for association start indication.
 * The buffer is allocated by the caller.
 */
VOID
AmPrepareAssocStartPara(
    __in PDOT11_MAC_ADDRESS StaMacAddr,
    __out PDOT11_INCOMING_ASSOC_STARTED_PARAMETERS AssocStartPara
    );

/**
 * Prepare information for association request received indication.
 * The buffer is allocated by the caller.
 * The caller must make sure the buffer is big enough to hold all data.
 */
VOID
AmPrepareAssocReqPara(
    __in PAP_ASSOC_MGR AssocMgr,
    __in_bcount(DOT11_ADDRESS_SIZE) PDOT11_MAC_ADDRESS StaMacAddr,
    __in BOOLEAN Reassociation,
    __in_bcount(RequestFrameSize) PUCHAR RequestFrame,
    __in USHORT RequestFrameSize,
    __out PDOT11_INCOMING_ASSOC_REQUEST_RECEIVED_PARAMETERS AssocReqPara,
    __inout PULONG AssocReqParaSize
    );

/**
 * Prepare information for association complete indication.
 * The buffer is allocated by the caller.
 * The caller must make sure the buffer is big enough to hold all data.
 */
VOID
AmPrepareAssocCompletePara(
    __in PAP_ASSOC_MGR AssocMgr,
    __in_opt PAP_STA_ENTRY StaEntry,
    __in_bcount(DOT11_ADDRESS_SIZE) PDOT11_MAC_ADDRESS StaMacAddr,
    __in ULONG Status,
    __in UCHAR ErrorSource,
    __in BOOLEAN Reassociation,
    __in_bcount_opt(RequestFrameSize) PUCHAR RequestFrame,
    __in USHORT RequestFrameSize,
    __in_bcount_opt(ResponseFrameSize) PUCHAR ResponseFrame,
    __in USHORT ResponseFrameSize,
    __out PDOT11_INCOMING_ASSOC_COMPLETION_PARAMETERS AssocCompletePara,
    __inout PULONG AssocCompleteParaSize
    );

/**
 * Prepare information for disassociation indication.
 * The buffer is allocated by the caller.
 */
VOID
AmPrepareDisassocPara(
    __in PDOT11_MAC_ADDRESS StaMacAddr,
    __in ULONG Reason,
    __out PDOT11_DISASSOCIATION_PARAMETERS DisassocPara
    );

/**
 * Timer related functions
 */

/**
 * Start station inactive timer
 * This function first increments the counter.
 * If the counter is 1, it sets the timer.
 */
VOID
FORCEINLINE
AmStartStaInactiveTimer(
    __in PAP_ASSOC_MGR AssocMgr
)
{
    LARGE_INTEGER               fireTime;
    if (InterlockedIncrement(&AssocMgr->StaInactiveTimerCounter) == 1)
    {
        //
        // This is the first one to start the timer.
        // Start the periodic timer
        //
        fireTime.QuadPart = Int32x32To64((LONG)AP_STA_INACTIVE_TIMER_INTERVAL, -10000);
        NdisSetTimerObject(
            AssocMgr->StaInactiveTimer,
            fireTime,
            AP_STA_INACTIVE_TIMER_INTERVAL, 
            NULL
            );
    }
}

/**
 * Stop station inactive timer
 * This function first decrements the counter.
 * If the counter is zero, it cancels the timer.
 */
VOID
FORCEINLINE
AmStopStaInactiveTimer(
    __in PAP_ASSOC_MGR AssocMgr
)
{
    LARGE_INTEGER  delayedFireTime;
    
    if (InterlockedDecrement(&AssocMgr->StaInactiveTimerCounter) == 0)
    {
        //
        // This is the last one to stop the timer.
        // Reschedule the periodic timer to not run for a long time while
        //
        delayedFireTime.QuadPart = Int32x32To64((LONG)MAXLONG, -10000);
        NdisSetTimerObject(
            AssocMgr->StaInactiveTimer,
            delayedFireTime,
            0, 
            NULL
            );        
    }
}

/**
 * Timeout callback for the station inactive timer.
 *
 * \param param PAP_ASSOC_MGR
 */
NDIS_TIMER_FUNCTION AmStaInactiveTimeoutCallback;

#define STA_WAITING_FOR_ASSOC_REQ       1
#define STA_NOT_WAITING_FOR_ASSOC_REQ   0

/**
 * Start the association timer for a station.
 */
VOID
FORCEINLINE
AmStartStaAssocTimer(
    PAP_STA_ENTRY StaEntry
    )
{
    LARGE_INTEGER fireTime;

    //
    // Reference the station first
    //
    ApRefSta(StaEntry);

    //
    // Set the waiting for association request flag
    //
    InterlockedExchange(
        &StaEntry->WaitingForAssocReq,
        STA_WAITING_FOR_ASSOC_REQ
        );
    
    //
    // Start timer
    //
    fireTime.QuadPart = Int32x32To64((LONG)AP_ASSOCIATION_REQUEST_TIMEOUT, -10000);
    NdisSetTimerObject(StaEntry->AssocTimer, 
        fireTime, 
        0, 
        NULL
        );
}

/**
 * Stop the association timer for a station.
 * It is a no-op if the timer is not started.
 */
VOID
FORCEINLINE
AmStopStaAssocTimer(
    PAP_STA_ENTRY StaEntry
    )
{
    BOOLEAN timerCancelled = FALSE;
    
    //
    // Clear the waiting for association request flag
    //
    if (InterlockedExchange(
            &StaEntry->WaitingForAssocReq,
            STA_NOT_WAITING_FOR_ASSOC_REQ
            ) == STA_WAITING_FOR_ASSOC_REQ)
    {
        //
        // Stop periodic timer
        //
        timerCancelled = NdisCancelTimerObject(StaEntry->AssocTimer);
        if (timerCancelled)
        {
            //
            // Need to deref the station
            //
            ApDerefSta(StaEntry);
        }
    }
}

/**
 * Timeout callback for the association timer.
 * The ref count of the station must be incremented
 * when the timer is scheduled.
 *
 * \param param PAP_STA_ENTRY
 */
NDIS_TIMER_FUNCTION AmStaAssocTimeoutCallback;

#endif // _AP_ASSOCMGR_H



