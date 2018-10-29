/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Mp_Interface.h

Abstract:
    Interface functions into Mp layer
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
#ifndef _MP_INTERFACE_H

#define _MP_INTERFACE_H

//
// Forward declaration
//
typedef struct _ADAPTER         ADAPTER, *PADAPTER;
typedef struct _MP_TX_MSDU      MP_TX_MSDU, *PMP_TX_MSDU;
typedef struct _MP_RX_MSDU      MP_RX_MSDU, *PMP_RX_MSDU;
typedef struct _MP_RX_FRAGMENT  MP_RX_FRAGMENT, *PMP_RX_FRAGMENT;

typedef struct _NIC_RX_FRAGMENT NIC_RX_FRAGMENT, *PNIC_RX_FRAGMENT;
typedef struct _NIC             NIC, *PNIC;

//
// This is the Interface Miniport must implement that NIC will call into.
// These implementations for these functions should be placed in files
// whose name start with MP_ and should be located in the MP folder
//



/**
 * Hw should call this function when it is called on Hw11InitializeReceive.
 * This allows the Mp portion to allocate and initialize its own data structures.
 * 
 * \param pAdaper           The adapter context for this miniport
 * \param uNumFragments     The number of packets that the hardware can receive before
 *                          a packet gets dropped.
 * \return NDIS_STATUS_SUCCESS on success else failure.
 */
NDIS_STATUS
Mp11InitializeReceiveEngine(
    __in PADAPTER         pAdapter,
    __in ULONG            uNumFragments
    );

/**
 * This function releases all the resources that were allocated for the
 * purpose of Receiving packets. This function is called from Hw11TerminateReceive
 * which is usually called when Miniport is halting.
 *
 * \param pAdapter      The Adapter context for this hardware
 * \sa Mp11InitializeReceiveEngine, Hw11TerminateReceive, MpTerminateSendEngine
 */
VOID
Mp11TerminateReceiveEngine(
    __in PADAPTER pAdapter
    );


/**
 * This function is called to Associate a fragment that the Hw portion can receive
 * with its corresponding Mp portion. This binding will remain static through the
 * lifetime of the miniport.
 * 
 * \param pAdapter      
 * \param Fragment      The NIC_RX_FRAGMENT that we have to associate with
 * \param ppMpFragment  The MP_RX_FRAGMENT this is associated with NIC_RX_FRAGMENT
 * \return NDIS_STATUS_SUCCESS on a successful association, else failure
 * \sa Mp11DisassocaiteFragment
 */
NDIS_STATUS
Mp11AssociateRxFragment(
    __in  PADAPTER         pAdapter,
    __in  PNIC_RX_FRAGMENT Fragment,
    __deref_out_opt PMP_RX_FRAGMENT* ppMpFragment
    );

VOID
Mp11FreeRxNetBuffer(
    __in  PNIC_RX_FRAGMENT    pNicFragment
    );

/**
 * This function is called by Hw to break an association made at Initialize time
 * between a NIC_RX_FRAGMENT and MP_RX_FRAGMENT. This function will typically be called
 * during MiniportHalt. At this time, the MP_RX_FRAGMENT previously associated will
 * be freed back to the OS.
 * 
 * \param pAdapter
 * \param pNicFragment      The NIC_RX_FRAGMENT to disassociate from
 * \sa Mp11AssociateRxFragment
 */
VOID
Mp11DisassociateRxFragment(
    __in PADAPTER         pAdapter,
    __in PNIC_RX_FRAGMENT pNicFragment
    );

/**
 * This function is called to allocated shared memory while the miniport is
 * running. The need for this might occur when the driver starts running low
 * on NIC_RX_FRAGMENTs and wants to allocate more to avoid low resource issues.
 *
 * This function will typically pend on success and the allocated memory will
 * be provided to the Hw on a callback function.
 * 
 * \param pAdapter
 * \param Length        The amount of memory to be allocated
 * \param Cached        Set if this is to be Cached memory
 * \param Context       Any context that Hw needs when the allocated completes
 * \return NDIS_STATUS value returned by NdisMAllocateSharedMemoryAsync
 * \sa Hw11SharedMemoryAllocateComplete, NdisMAllocateSharedMemoryAsync from DDK
 */
NDIS_STATUS
Mp11AllocateSharedMemoryAsync(
    __in PADAPTER         pAdapter,
    __in ULONG            Length,
    __in BOOLEAN          Cached,
    __in PVOID            Context
    );


/**
 * This function should be called to complete any Oid that was pended by the
 * Hw functions. Do not use the Ndis API directly. Since there can be only
 * one OID pended on the miniport, therefore, state information about the Oid
 * is already with the Mp functions.
 *
 * \param   pAdapter        The adapter context for this miniport
 * \param   ndisStatus      The status with which this Oid should complete
 */
VOID
Mp11CompletePendedRequest(
    __in  PADAPTER        pAdapter,
    __in  NDIS_STATUS     ndisStatus
    );

/**
 * This function starts the scan process. The pDot11ScanRequest
 * buffer contains the settings for scanning. This is called
 * both in case of OS initiated scan and internal periodic scans
 * by the station portion. The adapter would forward the scan request
 * to the hardware
 *
 * \param   pAdapter        The adapter context for this miniport
 * \param pDot11ScanRequest     The DOT11_SCAN_REQUEST_V2 block setting
 *                              scan parameters
 * \param ScanRequestBufferLength   Lenght of scan request block
 * \return NDIS_STATUS
 */
NDIS_STATUS
Mp11StartScan(
    __in  PADAPTER        pAdapter,
    __in  PDOT11_SCAN_REQUEST_V2 pDot11ScanRequest,
    __out ULONG           ScanRequestBufferLength
    );


/**
 * This function should be called by Hw when an outstanding scan operation completes.
 * Scan operations always pends and have to be completed asynchronously.
 * 
 * \param pAdapter          The adapter context for this miniport
 * \param ndisStatus        The success status of the scan request
 * \sa Hw11StartScan
 */
VOID
Mp11ScanComplete(
    __in PADAPTER         pAdapter,
    __in NDIS_STATUS      ndisStatus
    );

/**
 * Causes the miniport to perofrm a reset. This is used mainly for 
 * power management operations where we want to reset the miniport state
 * without requiring a NDIS initiated reset
 * 
 * \param pAdapter          The adapter context for this miniport
 * \param ResetType         Type of reset to perform
 */
VOID
Mp11InternalReset(
    __in  PADAPTER        pAdapter,
    __in  MP_RESET_TYPE   ResetType
    );

/**
 * Used to synchronize execution of a particular miniport routine with
 * the miniport interrupt handler.
 * 
 * \param pAdapter          The adapter context for this miniport
 * \param SynchronizeFunction   Function to invoke
 * \param SynchronizeContext    Context for the function
 */
VOID
Mp11SynchronizeWithInterrupt(
    __in  PADAPTER        pAdapter,
    __in  PVOID           SynchronizeFunction,
    __in  PVOID           SynchronizeContext
    );

/**
 * Indicates a status change to the NDIS. This is used for indication
 * connection status indication by the station portion and for link state
 * change status indications by the hardware portion. These are just
 * forwarded to NDIS
 * 
 * \param pAdapter          The adapter context for this miniport
 * \param pStatusIndication     Status indication buffer
 */
VOID
Mp11IndicateStatusIndication(
    __in  PADAPTER        pAdapter,
    __in  PNDIS_STATUS_INDICATION pStatusIndication
    );


/**
 * This function is called by the Hw when it has made any and all changes that 
 * were needed to the 802.11 frame to be transmitted. After the scatter gather 
 * operation has been performed, the NIC must NOT make any changes to the 
 * packet as they may not get reflected in the SG Elements.
 *
 * Please note that the entire NetBufferList will be scatter gathered by the Mp
 * function. The Hw will be called back on Hw11ScatterGatherComplete routine to
 * notify about the status of the SG operation. A return value of NDIS_STATUS_SUCCESS
 * on this routine does NOT imply that SG has been completed.
 * 
 * \param pAdapter      The adapter context for this miniport
 * \param pMpTxd        The MP_TX_MSDU to be scatter gathered
 * \param DispatchLevel Set if current IRQL is dispatch
 * \return NDIS_STATUS_SUCCESS if all SG has been requested, else failure code.
 * \sa Hw11ScatterGatherComplete
 */
NDIS_STATUS
Mp11PerformScatterGather(
    __in  PADAPTER    pAdapter,
    __in  PMP_TX_MSDU pMpTxd,
    __in BOOLEAN      DispatchLevel
    );


/**
 * Hw portion can call this function if it wants Mp functions to try and send any
 * packets that are ready and available for sending. If sends have been pended by
 * the Hw for resource reasons, this function can be called when resources become
 * available to immediately resume sends. Alternatively, we will have to wait till
 * the next packet comes along from the miniport and/or a send complete occurs before
 * we transmit again.
 * 
 * \param pAdapter          The adapter context for this miniport
 * \param DispatchLevel     Set to true if current IRQL is known to be DISPATCH
 * \sa Hw11TransmitTxMSDU
 */
VOID
Mp11PollForTransmission(
    __in  PADAPTER    pAdapter,
    __in  BOOLEAN     DispatchLevel
    );

WDFDEVICE
Mp11GetWdfDevice(
    __in  PADAPTER        pAdapter
    );

VOID
Mp11SendCompleteNBL(
    PADAPTER            pAdapter
    );

VOID
Mp11HandleSendCompleteInterrupt(
    PADAPTER            pAdapter
    );

VOID
Mp11ReadCompletionCallback(
    PADAPTER            Adapter,
    PNIC_RX_FRAGMENT NicFragment,
    size_t NumBytesRead
    );

#endif //_MP_INTERFACE_H

