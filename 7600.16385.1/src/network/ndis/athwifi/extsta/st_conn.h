/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    st_conn.h

Abstract:
    STA layer infrastructure connection/roaming functions
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created
    01-15-2006    Renamed sta_conn.h to st_conn.h

Notes:

--*/
#ifndef _STATION_CONNECT_H_
#define _STATION_CONNECT_H_

NDIS_STATUS 
StaInitializeConnectionContext(
    __in  PMP_EXTSTA_PORT        pStation
    );

VOID
StaFreeConnectionContext(
    __in  PMP_EXTSTA_PORT        pStation
    );

/**
 * Resets the connection. This function should disconnect
 * 
 * \param pStation          STATION structure
 * \param bDisconnect       TRUE if the function should perform
 *                          a clean disconnect, FALSE if this is 
 *                          called in context of halt to not perform
 *                          cleanup
 */
VOID
StaResetConnection(
    __in  PMP_EXTSTA_PORT        pStation,
    __in  BOOLEAN         bDisconnect
    );


NDIS_STATUS
StaConnectInfra(
    __in  PMP_EXTSTA_PORT        pStation
    );

NDIS_STATUS
StaDisconnectInfra(
    __in  PMP_EXTSTA_PORT        pStation
    );

VOID
StaIndicateDisassociation(
    __in  PMP_EXTSTA_PORT        pStation,
    __in  PMP_BSS_ENTRY  pAPEntry,
    __in  ULONG           Reason
    );

/**
 * Performs connection completion operations. This function would indicate
 * connection completion.
 * 
 * \param pStation          STATION structure
 * \param CompletionStatus  Status code to use for indicating connection completion. 
 *              If a function knows why the conneciton is failing, it will set 
 *              this status, else everyone will just forward the status from 
 *              previous function
 */
VOID
StaConnectComplete(
    __in  PMP_EXTSTA_PORT        pStation,
    __in  ULONG           CompletionStatus
    );


/**
 * Start the infrastructure connection process. This is called first time when the
 * OS sets the connect OID and may be called a second time if during the first
 * call we were not able to find the AP and had to do a scan to find the AP
 * 
 * \param pStation          STATION structure
 * \param FirstAttempt      TRUE if called directly in response to 
 *                          OID_DOT11_CONNECT_REQUEST and false if called
 *                          from a scan complete callback.
 *
 * \return NDIS_STATUS
 */
NDIS_STATUS
StaConnectStart(
    __in  PMP_EXTSTA_PORT        pStation,
    __in  BOOLEAN         FirstAttempt
    );

/**
 * Initiates the association process with a particular access point. If the
 * function succeeds, the StaCompleteAssociationProcess is called.
 * 
 * \param pStation          STATION structure
 * \param pAPEntry          The APEntry structure to initiate the association
 *                          process
 * \return NDIS_STATUS  
 * \sa StaCompleteAssociationProcess
 */
NDIS_STATUS
StaStartAssociationProcess(
    __in  PMP_EXTSTA_PORT        pStation,
    __in  PMP_BSS_ENTRY  pAPEntry
    );

/**
 * Completes a previous association process. If the association process was
 * unsuccessful, this function would restart association with any other
 * access points
 *
 * \param pStation          STATION structure
 * \param Status            Status of the association attempt
 * \sa StaStartAssociationProcess
 */
VOID
StaCompleteAssociationProcess(
    __in  PMP_EXTSTA_PORT        pStation,
    __in  ULONG           Status
    );

/**
 * Disconnects from a specified access point
 * 
 * \param pStation          STATION structure
 * \param pAPEntry          The APEntry structure to disconnect from
 * \param CompletionStatus  Original association state when the disconnect
 *              was initiated. This is used to determine what steps to take
 *              to perform the disconnect
 *
 * \return NDIS_STATUS from the disconnect operation
 */

NDIS_STATUS
StaDisconnect(
    __in  PMP_EXTSTA_PORT        pStation,
    __in  PMP_BSS_ENTRY  pAPEntry,
    __in  STA_ASSOC_STATE OriginalState
    );


/**
 * Initiates a join operation with an access point. If the join attempt
 * fails, this function must return to the caller without invoking
 * association completion
 * 
 * \param pStation          STATION structure
 * \param pAPEntry          The APEntry to connect to
 * \param JoinFailureTimeout    Time to wait for join in beacon intervals
 * \return NDIS_STATUS from the join attempt
 */
NDIS_STATUS
StaJoin(
    __in  PMP_EXTSTA_PORT        pStation,
    __in  PMP_BSS_ENTRY  pAPEntry,
    __in  ULONG           JoinFailureTimeout
    );

/**
 * Invoked to attempt roaming. This is called from within the context
 * of the periodic scan completion. The routine tries to determine if 
 * we have a better access point to associate with and if yes, performs
 * the roam
 * 
 * \note The connect lock must be held when calling this routine
 *
 * \param pStation          STATION structure
 */
#ifndef HIDE_ANNOTATION
__drv_mustHold(KeSpinLockType)
__drv_requiresIRQL(DISPATCH_LEVEL)
#endif
VOID
StaRoamStart(
    __in  PMP_EXTSTA_PORT        pStation
    );

/**
 * Invoked to reset all the roam state. To roam, we collect and store
 * some state information and this routine goes and clears it all up.
 * This is generally called on a reset request
 *
 * \param pStation          STATION structure
 */
VOID 
StaResetRoamState(
    __in  PMP_EXTSTA_PORT        pStation
    );


#endif // _STATION_CONNECT_H_

