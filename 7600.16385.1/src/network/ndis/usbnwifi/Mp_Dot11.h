/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Mp_Dot11.h

Abstract:
    MP layer structures and defines
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
#pragma once

#if USE_SAMPLE_PACKET_BUFFERING_IMPLEMENTATION
#include "Nwf_Mp_PSPacketsManager.h"
#endif  // USE_SAMPLE_PACKET_BUFFERING_IMPLEMENTATION
         
#define MP_MAJOR_NDIS_VERSION       6
#define MP_MINOR_NDIS_VERSION       0

/**
 * This OID specifies the NDIS version in use by the miniport driver. 
 * The high byte is the major version. The low byte is the minor version.
 */
#define MP_NDIS_DRIVER_VERSION            ((MP_MAJOR_NDIS_VERSION << 8) + (MP_MINOR_NDIS_VERSION))

/** NDIS_MEDIUM of the driver */
#define MP_MEDIA_TYPE                NdisMediumNative802_11

/** Physical medium of the driver */
#define MP_PHYSICAL_MEDIA_TYPE      NdisPhysicalMediumNative802_11

/**
 * The name of the company or any appropriate Vendor Description
 * TODO check on this whether it is msft or realtek
 */
#define MP_VENDOR_DESC                          "Realtek"

/**
 * Highest byte is the NIC byte plus three vendor bytes, they are normally
 * obtained from the NIC
 */
#define MP_VENDOR_ID                 0x00FFFFFF

/**
 * This tag identifies the driver uniquely to the OS. Helps track memory owner and
 * leaks when going through the memory pools
 */
#define MP_MEMORY_TAG                             'MltR'

/** Pool tag to identify the NetBufferList Pool for receive indication */
#define MP_RX_NET_BUFFER_LIST_POOL_TAG          'altR'

/** Pool tag to identify the NetBuffer Pool for receive indication */
#define MP_RX_NET_BUFFER_POOL_TAG                'bltR'

/** Size of context for an indicated NBL */
#define MP_RECEIVE_NBL_CONTEXT_SIZE             HW_RECEIVE_NBL_CONTEXT_SIZE

/** The maximum number of packets we will attempt to reassemble in 
  * parallel before dropping the oldest one. Typical number of associations
  * on a typical AP are 25 and 32 will cover such scenarios nicely. If the
  * Reassembly line size needs to be increased, change the value here.
  */
#define MP_MAX_REASSEMBLY_LINE_SIZE              32

/*
 * The number of Reassembly Rx MSDUs that are currently in use before
 * we start expiring old receive Rx MSDUs if they have exceeded the
 * RxLifeTime set by the OS.
 */ 
#define MP_REASSEMBLY_LINE_LOW_RESOURCES_THRESHOLD   30

/*
 * Countdown before which the Reassembly Cleanup algorithm to kick in. The unit
 * is the number of times receive interrupt handling routine has run and indicated
 * something up to the OS.
 */
#define MP_REASSEMBLY_CLEANUP_COUNTDOWN_VALUE         250

/*
 * The Check for Hang interval in seconds. NDIS will call into the driver every time
 * this interval expires. Specifying it as zero makes NDIS default to a two second
 * interval. Think your reasons carefully before specifying any interval except default.
 */
#define MP_CHECK_FOR_HANG_INTERVAL      0

/*
 * The amount of time in units of MP_CHECK_FOR_HANG_INTERVAL for which we will sample
 * the Rx MSDU list to discover if it has been underutilized and needs to be shrunk
 */
#define MP_RX_MSDU_LIST_SAMPLING_PERIOD     5


/*
 * If the under utilization of the Rx MSDU list exceeds this threshold, we will shrink the
 * Rx MSDU list. This threshold is described in percentage of the current List Size. Note that
 * the Rx MSDU List size will not shrink beyond the initial Rx MSDU list size we started with.
 */ 
#define MP_RX_MSDU_LIST_UNDER_UTILIZATION_THRESHOLD     40

/*
 * The number of tick intervals in unit of CheckForHang Interval for which the sends have
 * to be stalled before we will ask Ndis to reset the NIC. Since the algorithm is an approximate
 * one, and not synchronized, the actual period might take an extra tick. This, however, should
 * not cause any issues as long as the value is not too big.
 */
#define MP_SENDS_HAVE_STALLED_PERIOD                  5


/*
 * The maximum number of attempts that will be made to reset a NIC that is not
 * functioning properly. After this threshold of attempts is exceeded, a removal
 * of the miniport is requested from NDIS
 */
#define MP_RESET_ATTEMPTS_THRESHOLD                   5

/** When passed to NdisWaitEvent, will cause an indefinite wait to occur */
#define MP_NEVER_TIMEOUT                  0


/**
 * The current NDIS PnP state of the miniport
 */
typedef enum _MP_NDIS_PNP_STATE
{
    MINIPORT_PAUSED = 1,
    MINIPORT_PAUSING,
    MINIPORT_RUNNING
} MP_NDIS_PNP_STATE, *PMP_NDIS_PNP_STATE;


typedef enum _MP_DOT11_STATE {
    INIT_STATE = 1,
    OP_STATE
} MP_DOT11_STATE, *PMP_DOT11_STATE;

typedef enum _MP_DOT11_RUNNING_MODE {
    RUNNING_MODE_UNKNOWN,
    RUNNING_MODE_STA_INFRASTRUCTURE,
    RUNNING_MODE_STA_ADHOC
} MP_DOT11_RUNNING_MODE, *PMP_DOT11_RUNNING_MODE;


/**
 * Appropriate receive handler for indicating packets in safe mode
 */
typedef VOID
(*MP_RECEIVE_HANDLER_FUNCTION)(
	__in PADAPTER         pAdapter,
	PNIC_RX_FRAGMENT    NicFragment,
	size_t              NumBytesRead
	);

/**
 * This is list of all the OIDS (NDIS and DOT11) supported by this driver
 * {VENDOR} Add and remove from Oids from this list based on your drivers capabilities.
 */
EXTERN NDIS_OID MPSupportedOids[];


/**
 * This structure keeps track of various statistics for the NIC.
 * Add any statistics that you are interested in keep here.
 * Please note that not all statistics are being tracked today.
 * This Data structure is easily extended and vendor should track
 * only the statistics it needs.
 * \warning Make sure that adding of new members to this 
 * data structure does not unalign it. This DS is embedded in
 * the ADAPTER and may cause alignment issue there.
 *
 */
typedef struct _MP_STATS
{
    /** The number of packets the miniport has transmitted. Retry not included */
    ULONG64                     TotalTransmits;
    
    /** The number of frames successfully transmitted */
    ULONG64                     TotalGoodTransmits;

    /** The number of frames successfully received */
    ULONG64                     TotalGoodReceives;

    /** The total number of packets dropped because of reassembly failure */
    ULONG64                     ReassemblyFailedReceives;

    /** The total number of data packets that failed transmission */
    ULONG                       TotalBadTransmits;

    /** The total number of packets that failed recevie */
    ULONG                       TotalBadReceives;

} MP_STATS, *PMP_STATS;


/**
 * MP_RX_FRAGMENT represents a single fragment that can be received by the NIC
 */
typedef struct _MP_RX_FRAGMENT
{
    PMDL            Mdl;
    PNET_BUFFER     NetBuffer;

    /** MDL offset before we modified the MDL to remove fragment header */
    ULONG           OrigByteOffset;

    /** Original byte count in the MDL */
    ULONG           OrigByteCount;
} MP_RX_FRAGMENT, *PMP_RX_FRAGMENT;

 #define NIC_ADAPTER_NAME_SIZE          128

typedef enum _NIC_STATE
{
    NicPaused,
    NicPausing,
    NicRunning
} NIC_STATE;


#define NIC_SUPPORTED_FILTERS ( \
                NDIS_PACKET_TYPE_DIRECTED   | \
                NDIS_PACKET_TYPE_MULTICAST  | \
                NDIS_PACKET_TYPE_BROADCAST  | \
                NDIS_PACKET_TYPE_PROMISCUOUS | \
                NDIS_PACKET_TYPE_ALL_MULTICAST)

#define fMP_RESET_IN_PROGRESS               0x00000001
#define fMP_DISCONNECTED                    0x00000002
#define fMP_HALT_IN_PROGRESS                0x00000004
#define fMP_SURPRISE_REMOVED                0x00000008
#define fMP_RECV_LOOKASIDE                  0x00000010
#define fMP_INIT_IN_PROGRESS                0x00000020
#define fMP_SEND_SIDE_RESOURCE_ALLOCATED    0x00000040
#define fMP_RECV_SIDE_RESOURCE_ALLOCATED    0x00000080
#define fMP_POST_READS                      0x00000100
#define fMP_POST_WRITES                     0x00000200


/**
 * \defgroup AdapterStatusFlags These are the various states that the Adapter
 * can be in. These states are kept in the Adapter->Status field.
 */ 
/*@{*/   // AdapterStatusFlags

#define     MP_ADAPTER_RESET_IN_PROGRESS            0x00000001
#define     MP_ADAPTER_HARDWARE_ERROR               0x00000004
#define     MP_ADAPTER_REMOVE_IN_PROGRESS           0x00000008
#define     MP_ADAPTER_HALT_IN_PROGRESS             0x00000010
#define     MP_ADAPTER_NON_RECOVER_ERROR            0x00000020
#define     MP_ADAPTER_DOT11_RESET_IN_PROGRESS      0x00000040
//#define     MP_ADAPTER_DOT11_SCAN_IN_PROGRESS       0x00000080
#define     MP_ADAPTER_SURPRISE_REMOVED             0x00000100
#define     MP_ADAPTER_NDIS_PAUSE_IN_PROGRESS       0x00000200
            
#define     MP_ADAPTER_NDIS_RESTART_IN_PROGRESS     0x00000400

#define     MP_ADAPTER_UNRESTRICTED_SCAN_RECEIVED   0x00000800
#define     MP_ADAPTER_RESTRICTED_SCAN_RECEIVED     0x00001000

#define     MP_ADAPTER_HW_IS_SCANNING               0x00002000

#define     MP_ADAPTER_CONNECT_IN_PROGRESS          0x00004000
#define     MP_ADAPTER_NETWORK_MONITOR_MODE         0x00008000

#define     MP_ADAPTER_CANNOT_SEND_MASK                 \
    (MP_ADAPTER_RESET_IN_PROGRESS | MP_ADAPTER_HARDWARE_ERROR |    \
     MP_ADAPTER_REMOVE_IN_PROGRESS | MP_ADAPTER_HALT_IN_PROGRESS | MP_ADAPTER_SURPRISE_REMOVED |  \
     MP_ADAPTER_NON_RECOVER_ERROR | MP_ADAPTER_DOT11_RESET_IN_PROGRESS | MP_ADAPTER_NDIS_PAUSE_IN_PROGRESS |  \
     MP_ADAPTER_NETWORK_MONITOR_MODE )

#define     MP_ADAPTER_CANNOT_SCAN_MASK         \
    (MP_ADAPTER_UNRESTRICTED_SCAN_RECEIVED | MP_ADAPTER_RESTRICTED_SCAN_RECEIVED | \
     MP_ADAPTER_HW_IS_SCANNING )

typedef struct _ADAPTER_WORKITEM_CONTEXT {

    PADAPTER        Adapter;
}   ADAPTER_WORKITEM_CONTEXT, *PADAPTER_WORKITEM_CONTEXT;

//
// Specify an accessor method for the WMI_SAMPLE_DEVICE_DATA structure.
//
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(ADAPTER_WORKITEM_CONTEXT, GetAdapterWorkItemContext)

/**
 * This structure describes an Adapter that the miniport driver is working with
 *  Note: this data structure shouldn't have any thing which points to the underlying hardware.
 *  That should be added to the NIC data structure
 */
typedef struct _ADAPTER
{
    /** Link multiple adapters using this Entry */
    LIST_ENTRY                          Link;

    /** 
     * The handle by which NDIS recognizes this adapter
     * This handle needs to be passed in for many of the calls made to NDIS
     */
    NDIS_HANDLE                         MiniportAdapterHandle;

    /**
      * Keep track of various device objects.
      */ 
    PDEVICE_OBJECT          Pdo;
    PDEVICE_OBJECT          Fdo;
    PDEVICE_OBJECT          NextDeviceObject;
    WDFDEVICE               WdfDevice;

    /* COntrol device Information*/
    NDIS_HANDLE        NdisControlDeviceHandle; // From NdisMRegisterDevice
    PDEVICE_OBJECT     ControlDeviceObject;  // Device for IOCTLs

    
    /** 
     * This is the hardware specific data structure.
     * Vendors should implement this to work with their proprietary hardware
     */
    PNIC                                Nic;

    /**
     * This maintains 802.11 station specific state
     */
    PSTATION                            Station;

    /**
     * Flags that contain the current status of the adapter 
     */
    ULONG                               Status;

    /** The Current NDIS PnP State of the miniport */
    MP_NDIS_PNP_STATE                   NdisPnPState;

    /** Lock to synchronize Reset, Pause and Restart events */
    NDIS_MUTEX                          ResetPnPMutex;

    /** Number of times Pause/Reset/Dot11Resets we are processing/have started */
    ULONG                               ResetPauseCount;

    /** Reference count of operations currently executing on the miniport */
    ULONG                               RefCount;
    
    /** This event is signalled when the MiniportReference hits zero */
    NDIS_EVENT                          MiniportRefCountZeroEvent;

    /** This event is set when miniport halt can take place */
    NDIS_EVENT                          HaltWaitEvent;
    
   
    /** The current Operational mode of the miniport */
    ULONG                               OperationMode;
    
    /** The current state of the miniport (INIT or OPERATIONAL) */
    MP_DOT11_STATE                      State;

    /** The 802.11 mode the miniport is operating under (EXT STA INFRA, EXT STA ADHOC, AP, STA INFRA, STA ADHOC, ) */
    MP_DOT11_RUNNING_MODE               Dot11RunningMode;

    /** Contains various statistics for the adapter */
    MP_STATS;

    /** This value is set when StartRequest is called */
    BOOLEAN                                StartRequestCalled;
    
    /** The Power State the device is currently in */
    NDIS_DEVICE_POWER_STATE             DevicePowerState;  

    /** The power state we would be going into next. This is used by SynchronizeWithInterrupt */
    NDIS_DEVICE_POWER_STATE             NextDevicePowerState;

    /** The current look ahead for the adapter */
    ULONG                               LookAhead;

    /** The current packet filter set on the miniport */
    ULONG                               PacketFilter;

    /** The Permanent MAC address of the adapter */
    UCHAR                               PermanentAddress[ETH_LENGTH_OF_ADDRESS];

    /** The current MAC address of the adapter */
    UCHAR                               CurrentAddress[ETH_LENGTH_OF_ADDRESS];

    /** Autoconfig enabled */
    ULONG                               AutoConfigEnabled;

    /** Spin lock to synchronise access to the TX_MSDUs */
    //NDIS_SPIN_LOCK                      SendLock;
    WDFSPINLOCK                         SendLock;
    PVOID                               OwningThreadCaller ;
    

    /** Total number of TX_MSDU that have been allocated */
    ULONG                               TotalTxMSDUs;

    /** The number of TX_MSDU currently available for sending */
    ULONG                               NumAvailableTxMSDU;

    /** The list of TX_MSDU preallocated for this adapter */
    PMP_TX_MSDU                         TxMSDUList;

    /** Pointer to a free TX_MSDU that is free and can be used for a send */
    PMP_TX_MSDU                         FreeTxMSDU;

    /** Pointer to the next TX_MSDU to be sent to the hardware. Needed to maintain Tx order */
    PMP_TX_MSDU                         NextTxMSDUToSend;

    /** Pointer to the next TX_MSDU we expect to be SendCompleted. It is protected by the SendLock */
    PMP_TX_MSDU                         NextTxMSDUToComplete;

    /*
     * Data structure to queue NBL to be transmitted in. We will queue NBL
     * when we are running low on TX_MSDUs 
     */
    struct _MP_NBL_QUEUE               TxQueue;

    /** Number of thread currently active in the send path */
    ULONG                               NumActiveSenders;

    /** Event to signal when ActiveSenders count gets to Zero */
    NDIS_EVENT                          ActiveSendersDoneEvent;

#ifdef  USE_SAMPLE_PACKET_BUFFERING_IMPLEMENTATION
    /** The number of ticks that occurs for this machine before a TU expires. */
    ULONG                               NumTicksPer100TU;

    /** Queues to hold packets for associated stations in PS mode */
    struct _MP_PS_PACKETS_MANAGER       PSPacketManager;

    /*
     * Hold the TIM element information. TIM element contained in beacon
     * is extracted from this data structure
     */
    MP_TIM_ELEMENT                      ElementTIM;

    /** Set if we have to handle the DTIM packet delivery in the InterruptHandle routine */
    BOOLEAN                                HandleDTIMDeferred;

    /** The highest Association ID for which a bit is set in the virtual bitmap */
    USHORT                              HighestElementTIMAssocationID;

    /** The lowest Association ID for which a bit is set in the virtual bitmap */
    USHORT                              LowestElementTIMAssocationID;
    
#endif  // USE_SAMPLE_PACKET_BUFFERING_IMPLEMENTATION
    /*
     * The total number of Rx MSDU that should be allocated by this miniport.
     * This is the value obtained from Registry
     */
    ULONG                               TotalRxMSDU;

    /** The maximum number of Rx MSDU we should allow the pools to grow to */
    ULONG                               MaxRxMSDU;

    /** The minimum number of Rx MSDU we should allow the pools to grow to */
    ULONG                               MinRxMSDU;    

    /** The correct function to handle receives at this time */
    MP_RECEIVE_HANDLER_FUNCTION         ReceiveHandlerFunction;

    /**
     * The count of Rx MSDUs that have actually been allocated by the miniport.
     * When the Rx MSDU pool grows, this number will reflect that. However, this
     * number will never exceed MaxRxMSDU 
     */
    ULONG                               TotalRxMSDUAllocated;

    /** The Number of Rx MSDU currently available for indicating packets */
    ULONG                               NumAvailableRxMSDU;

    /** The list in which free Rx MSDUs are maintained */
    SLIST_HEADER                        RxMSDUFreeList;
    
    /** The lookaside list from which Receive Rx MSDU will be allocated. Allocation
    can be made on the fly from this list if load is heavy */
    PNPAGED_LOOKASIDE_LIST              RxMSDULookasideList;

    /** The Pool for allocating NET_BUFFER_LIST from for receive indication */
    NDIS_HANDLE                         NetBufferListPool;

    /** The Pool for allocating NET_BUFFERs from for receive indication */
    NDIS_HANDLE                         NetBufferPool;
    
    /** The total number of Rx MSDUs in the Reassembly line */
    ULONG                               TotalRxMSDUInReassembly;

    /** An array to hold Rx MSDUs that are currently in Reassembly */
    PMP_RX_MSDU                         ReassemblyLine[MP_MAX_REASSEMBLY_LINE_SIZE];

    /** This is the Reassembly Rx MSDU for which we most recently received a fragment */
    PMP_RX_MSDU                         MRUReassemblyRxMSDU;
    
    /*
     * Count down to when we will run through the ReassemblyLine and clear
     * out any expired Rx MSDUs. This is a periodic cleanup mechanism. The
     * unit is the number of interrupts in which we have indicated up
     * frame(s) to the OS
     */
    ULONG                               ReassemblyLineCleanupCountdown;
    
    /** The max amount of time in TUs after which reassembly is abandoned */
    ULONG                               MaxRxLifeTime;

    /*
     * Number of thread currently active in the receive path. There is only one
     * DPC in which receives can be occuring but we need to track this value in
     * order to correctly be able to Pause and Reset.
     */
    ULONG                               NumActiveReceivers;
    
    /** This event is set when all packets received and indicated have returned */
    NDIS_EVENT                          AllReceivesHaveReturnedEvent;
    
    /** The number of 2 second intervals that have passed since sampling of Rx MSDU list started */
    ULONG                               RxMSDUListSampleTicks;

    /** The sum of number of Rx MSDU that have been found free in each sampling */
    ULONG                               NumRxMSDUNotUtilized;

    /** The number of packets submitted to hardware but not yet completed. */
    ULONG                               PendingSends;

    /** The number of packets recevied that have not yet been indicated to NDIS */
    ULONG                               PendingReceives;

    /** The number of Good Transmits that Check For Hang uses to determine if send has stalled */
    ULONG64                             TotalTransmitsSnapShot;

    /** The number of Check For Hang intervals that have passed with send stalled */
    ULONG                               NumStalledSendTicks;

    /** When a reset operation pends, this work item will be used to complete it asynchrously */
    WDFWORKITEM                         NdisResetWorkItem;

    /** Dot11 Reset can get issued at Dispatch level */
    WDFWORKITEM                         Dot11ResetWorkItem;

    /** The number of attempts that have been made to reset this NIC */
    ULONG                               NumResetAttempts;

    /** Work item to asynchronously complete NDIS Pause in */
    //NDIS_WORK_ITEM                      PauseWorkItem;

    /** If the miniport pends a request, the copy is kept here */
    PNDIS_OID_REQUEST                   PendedRequest;

    /** The Max MPDU length cached for quick access */
    ULONG                               MPDUMaxLength;
    
    WCHAR                   AdapterDesc[NIC_ADAPTER_NAME_SIZE];

    union {
        ULONG                   Flags;
        //
        // Each field in this struct correspond to ascending #define fMP_XXX
        // values. Don't touch these fields directly. It's there for
        // debugging purposes.
        //
        struct {
            ULONG ResetInProgess : 1;
            ULONG Disconnected : 1;
            ULONG HaltInProgress : 1;
            ULONG SurpriseRemoved: 1;
            ULONG RecvLookAside : 1;
            ULONG InitInProgess : 1;
            ULONG SendResourcesAllocated : 1;
            ULONG RecvResourcesAllocated : 1;
            ULONG PostReads : 1;
            ULONG PostWrites : 1;
            ULONG Unused : 23;
        } FlagsByName;
    };
    NIC_STATE               AdapterState;

} ADAPTER, *PADAPTER;

typedef struct _WDF_DEVICE_INFO{

    PADAPTER Adapter;

} WDF_DEVICE_INFO, *PWDF_DEVICE_INFO;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(WDF_DEVICE_INFO, GetWdfDeviceInfo)

#define MP_OFFSET(field)   ((UINT)FIELD_OFFSET(ADAPTER,field))
#define MP_SIZE(field)     sizeof(((PADAPTER)0)->field)

typedef struct _MP_REG_ENTRY
{
    NDIS_STRING RegName;                // variable name text
    UINT        FieldOffset;            // offset to MP_ADAPTER field
    UINT        FieldSize;              // size (in bytes) of the field
    UINT        Default;                // default value to use
    UINT        Min;                    // minimum value allowed
    UINT        Max;                    // maximum value allowed
} MP_REG_ENTRY, *PMP_REG_ENTRY;


ULONG
FASTCALL
MpInterlockedSetClearBits (
    __inout PULONG Flags,
    __in ULONG sFlag,
    __in ULONG cFlag
    );


#define MP_RUNNING_IN_STA_INFRA_MODE(_Adapter)   \
    (_Adapter->Dot11RunningMode == RUNNING_MODE_STA_INFRASTRUCTURE)

#define MP_RUNNING_IN_STA_ADHOC_MODE(_Adapter)   \
    (_Adapter->Dot11RunningMode == RUNNING_MODE_STA_ADHOC)

//
// These macros verifies the operational mode and current state of the miniport.
//
#define MP_VERIFY_MODE_AND_STATE(_Adapter, _Mode, _State)     \
    ((_Adapter->OperationMode == _Mode) && (_Adapter->State == _State))

#define MP_VERIFY_MODE(_Adapter, _Mode)         \
    ((_Adapter->OperationMode == _Mode))

#define MP_VERIFY_STATE(_Adapter, _State)       \
    ((_Adapter->State == _State))

/*
 * When the number of Rx MSDU available to the miniport falls below this
 * value, the miniport starts indicating low resources with packets indicated
 * to NDIS. At this point, the upper layer driver will copy the NdisPacket
 * indicated and return it immediately so that the next received packets can
 * be indicated using this Rx MSDU. This helps avoid packet loss due to lack of
 * resources to indicate packets up in.
 */
#define     MP_RX_MSDU_LOW_RESOURCE_THRESHOLD        4

#define     MP_RX_MSDU_GROW_POOL_SIZE                1

/*
 * The maximum number of Rx MSDU we will indicate to NDIS while handling
 * an interrupt. This prevents us from staying within a DPC for too
 * long and also helps avoid the need for dynamically allocating an array 
 */
#define     MP_MAX_RX_MSDU_TO_INDICATE_DURING_INTERRUPT_HANDLING     5

/** Macro to test if the miniport is in low power state */
#define MP_IS_LOW_POWER_STATE(_PwrState)                       \
            ((_PwrState) > NdisDeviceStateD0)

/** Access the timestamp field of the packet in the MiniportReservedEx */
#define MP_TIMESTAMP_FIELD(_NBL)             \
    ((PLARGE_INTEGER) &(NET_BUFFER_LIST_MINIPORT_RESERVED(_NBL)))

/** Placed the time when packet will expire in the MiniportReservedEx field */
#define MP_TIMESTAMP_NBL(_NBL, _LifeTime)         \
{                                                                           \
    KeQueryTickCount(MP_TIMESTAMP_FIELD(_NBL));     \
    MP_TIMESTAMP_FIELD(_NBL)->QuadPart += _LifeTime;  \
}



    
/*@}*/   // AdapterStatusFlags

#define     MP_SET_STATUS_FLAG(_M, _F)                  RtlInterlockedSetBits(&((_M)->Status), (_F))
#define     MP_CLEAR_STATUS_FLAG(_M, _F)                RtlInterlockedClearBits(&((_M)->Status), (_F))
#define     MP_SET_CLEAR_STATUS_FLAG(_M, _SF, _CF)     MpInterlockedSetClearBits(&((_M)->Status), (_SF), (_CF))
#define     MP_TEST_STATUS_FLAG(_M, _F)                 (((_M)->Status & (_F)) != 0)
#define     MP_ADAPTER_CANNOT_SEND_PACKETS(_Adapter)  (_Adapter->Status & MP_ADAPTER_CANNOT_SEND_MASK)
#define     MP_ADAPTER_CAN_SEND_PACKETS(_Adapter)      !(MP_ADAPTER_CANNOT_SEND_PACKETS(_Adapter))

#define     MP_GET_NDIS_PNP_STATE(_Adapter)                   (_Adapter->NdisPnPState)
#define     MP_SET_NDIS_PNP_STATE(_Adapter, _NdisPnPState)  (_Adapter->NdisPnPState = _NdisPnPState)

//
// Some helper MACROS for the Adapter
//
#define     MP_INCREMENT_REF(_pAdapter)                     NdisInterlockedIncrement((PLONG)&_pAdapter->RefCount)
#define     MP_DECREMENT_REF(_pAdapter)     \
{                                               \
    if (NdisInterlockedDecrement((PLONG)&_pAdapter->RefCount) == 0)       \
        NdisSetEvent(&_pAdapter->HaltWaitEvent);                            \
}    

#define     MP_INCREMENT_ACTIVE_COUNT(_pAdapter)    NdisInterlockedIncrement((PLONG)&_pAdapter->ActiveCount)
#define     MP_DECREMENT_ACTIVE_COUNT(_pAdapter)    NdisInterlockedDecrement((PLONG)&_pAdapter->ActiveCount)

#define     MP_INCREMENT_RESET_PAUSE_COUNT(_pAdapter)       NdisInterlockedIncrement((PLONG)&_pAdapter->ResetPauseCount)
#define     MP_DECREMENT_RESET_PAUSE_COUNT(_pAdapter)       NdisInterlockedDecrement((PLONG)&_pAdapter->ResetPauseCount)

#define     MP_INCREMENT_AVAILABLE_TX_MSDU(_pAdapter)       NdisInterlockedIncrement((PLONG)&_pAdapter->NumAvailableTxMSDU)
#define     MP_DECREMENT_AVAILABLE_TX_MSDU(_pAdapter)       NdisInterlockedDecrement((PLONG)&_pAdapter->NumAvailableTxMSDU)
#define     MP_ADD_AVAILABLE_TX_MSDU(_pAdapter, _Addend)    InterlockedExchangeAdd((PLONG)&_pAdapter->NumAvailableTxMSDU, _Addend)
#define     MP_INCREMENT_ACTIVE_SENDERS(_Adapter)           NdisInterlockedIncrement((PLONG)&_Adapter->NumActiveSenders)
#define     MP_DECREMENT_ACTIVE_SENDERS(_Adapter)       \
{                                                       \
    if(NdisInterlockedDecrement((PLONG)&_Adapter->NumActiveSenders) == 0)       \
    {                                                                           \
        NdisSetEvent(&_Adapter->ActiveSendersDoneEvent);                        \
    }                                                                           \
}

//
// Some global macros to work with NetBuffers, NetBufferLists and MDLs
//
#define     MP_GET_SEND_CONTEXT(_NBL)   ((PDOT11_EXTSTA_SEND_CONTEXT) NET_BUFFER_LIST_INFO(_NBL, MediaSpecificInformation))

#define     MP_SEND_COMPLETE_NBLS(_Adapter, _NBL, _Dispatch)        \
{ \
    Hw11CleanupNetBufferList((_Adapter)->Nic, _NBL); \
    NdisMSendNetBufferListsComplete((_Adapter)->MiniportAdapterHandle, _NBL, _Dispatch); \
}

#define     MP_SEND_COMPLETE_ONE_NBL(_Adapter, _NBL, _ndisStatus, _Dispatch)   \
{                                                                   \
    MPVERIFY(_NBL->Next == NULL);                   \
    MP_SET_NBL_STATUS(_NBL, _ndisStatus);         \
    Hw11CleanupNetBufferList((_Adapter)->Nic, _NBL); \
    NdisMSendNetBufferListsComplete((_Adapter)->MiniportAdapterHandle, _NBL, _Dispatch);                  \
}
#define     MP_SET_NBL_STATUS(_NBL, _ndisStatus)    (NET_BUFFER_LIST_STATUS(_NBL) = _ndisStatus)
#define     MP_GET_NBL_MP_RESERVED(_NBL)         (&(_NBL)->MiniportReserved[0])
#define     MP_IS_LAST_NB_IN_NBL(_NB)            (NET_BUFFER_NEXT_NB(_NB) == NULL)

//
// Macros for handling  Rx MSDUs 
//
#define     MP_INCREMENT_AVAILABLE_RX_MSDU(_pAdapter)          NdisInterlockedIncrement((PLONG)&_pAdapter->NumAvailableRxMSDU)
#define     MP_DECREMENT_AVAILABLE_RX_MSDU(_pAdapter)          NdisInterlockedDecrement((PLONG)&_pAdapter->NumAvailableRxMSDU)
#define     MP_ADD_TO_AVAILABLE_RX_MSDU(_pAdapter, NumToAdd)   InterlockedExchangeAdd((PLONG)&_pAdapter->NumAvailableRxMSDU, NumToAdd);
#define     MP_INCREMENT_TOTAL_RX_MSDU_ALLOCATED(_pAdapter)    NdisInterlockedIncrement((PLONG)&_pAdapter->TotalRxMSDUAllocated)
#define     MP_DECREMENT_TOTAL_RX_MSDU_ALLOCATED(_pAdapter)    NdisInterlockedDecrement((PLONG)&_pAdapter->TotalRxMSDUAllocated)
#define     MP_ADD_TO_TOTAL_RX_MSDU_ALLOCATED(_pAdapter, NumToAdd)      InterlockedExchangeAdd((PLONG)&_pAdapter->TotalRxMSDUAllocated, NumToAdd);

#define     MP_INCREMENT_PENDING_SENDS(_pAdapter)          NdisInterlockedIncrement((PLONG)&_pAdapter->PendingSends)
#define     MP_DECREMENT_PENDING_SENDS(_pAdapter)          NdisInterlockedDecrement((PLONG)&_pAdapter->PendingSends)

#define     MP_INCREMENT_ACTIVE_RECEIVERS(_Adapter)        NdisInterlockedIncrement((PLONG)&_Adapter->NumActiveReceivers)
#define     MP_DECREMENT_ACTIVE_RECEIVERS(_Adapter)        NdisInterlockedDecrement((PLONG)&_Adapter->NumActiveReceivers)

#define     MP_INCREMENT_PENDING_RECEIVES(_pAdapter)       NdisInterlockedIncrement((PLONG)&_pAdapter->PendingReceives)
#define     MP_DECREMENT_PENDING_RECEIVES(_pAdapter)       NdisInterlockedDecrement((PLONG)&_pAdapter->PendingReceives)
#define     MP_ADD_TO_PENDING_RECEIVES(_pAdapter, _Val)    InterlockedExchangeAdd((PLONG)&_pAdapter->PendingReceives, _Val)
#define     MP_SUBTRACT_FROM_PENDING_RECEIVES(_pAdapter, _Val)    InterlockedExchangeAdd((PLONG)&_pAdapter->PendingReceives, (_Val * -1))

#define     MP_INCREMENT_TOTAL_TRANSMITS(_Adapter)                _Adapter->TotalTransmits++
#define     MP_INCREMENT_TOTAL_GOOD_TRANSMITS(_Adapter)          _Adapter->TotalGoodTransmits++
#define     MP_INCREMENT_TOTAL_GOOD_RECEIVES(_Adapter)           _Adapter->TotalGoodReceives++
#define     MP_INCREMENT_REASSEMBLY_FAILED_COUNT(_Adapter)      _Adapter->ReassemblyFailedReceives++
#define     MP_INCREMENT_EXPIRED_PS_LIFETIME_COUNT(_Adapter)    _Adapter->ExpiredPSLifeTimeCount++
#define     MP_INCREMENT_TOTAL_BAD_TRANSMITS(_Adapter)           _Adapter->TotalBadTransmits++
#define     MP_INCREMENT_TOTAL_BAD_RECEIVES(_Adapter)            _Adapter->TotalBadReceives++
/*
#define     MP_INCREMENT_REASSEMBLY_FAILED_RECEIVES(_Adapter)   _Adapter->ReassemblyFailedReceives++;
*/

#define     MP_INTERLOCKED_INCREMENT_STATISTICS(_pUlongVal)      NdisInterlockedIncrement((PLONG)_pUlongVal)
#define     MP_INTERLOCKED_ADD_TO_STATISTICS(_pUlongVal, _Val)   InterlockedExchangeAdd((PLONG)_pUlongVal, _Val)
#define     MP_INTERLOCKED_INCREMENT_STATISTICS64(_pUlong64Val)     \
{                                                               \
    PLARGE_INTEGER      _pLargeInt = (PLARGE_INTEGER)_pUlong64Val; \
    if (NdisInterlockedIncrement(&_pLargeInt->LowPart) == 0)    \
    {                                                           \
        NdisInterlockedIncrement(&_pLargeInt->HighPart);        \
    }                                                           \
}


#define MP_ACQUIRE_SEND_LOCK(_Adapter, _DispatchLevel)      WdfSpinLockAcquire(_Adapter->SendLock); \
                                                            _Adapter->OwningThreadCaller = _ReturnAddress();//MP_ACQUIRE_SPIN_LOCK(&(_Adapter->SendLock), _DispatchLevel)
#define MP_RELEASE_SEND_LOCK(_Adapter, _DispatchLevel)      _Adapter->OwningThreadCaller = NULL; \
                                                            WdfSpinLockRelease(_Adapter->SendLock); //MP_RELEASE_SPIN_LOCK(&(_Adapter->SendLock), _DispatchLevel)

//#define MP_ACQUIRE_RESET_PNP_LOCK(_Adapter)     MP_ACQUIRE_SPIN_LOCK(&_Adapter->ResetPnPLock, FALSE)
//#define MP_RELEASE_RESET_PNP_LOCK(_Adapter)     MP_RELEASE_SPIN_LOCK(&_Adapter->ResetPnPLock, FALSE)
#define MP_ACQUIRE_RESET_PNP_LOCK(_Adapter)   NDIS_WAIT_FOR_MUTEX(&_Adapter->ResetPnPMutex);
#define MP_RELEASE_RESET_PNP_LOCK(_Adapter)   NDIS_RELEASE_MUTEX(&_Adapter->ResetPnPMutex);


#define MP_NIC_POWER_STATE_IS_OFF(_Adapter)  (!Hw11QueryNicPowerState(_Adapter->Nic, FALSE))
#define MP_DOT11_SCAN_IN_PROGRESS(_Adapter) (MP_TEST_STATUS_FLAG(_Adapter, MP_ADAPTER_CANNOT_SCAN_MASK))
#define MP_NDIS_PAUSE_IN_PROGRESS(_Adapter) (MP_TEST_STATUS_FLAG(_Adapter, MP_ADAPTER_NDIS_PAUSE_IN_PROGRESS))
#define MP_DOT11_CONNECT_IN_PROGRESS(_Adapter) (MP_TEST_STATUS_FLAG(_Adapter, MP_ADAPTER_CONNECT_IN_PROGRESS))


