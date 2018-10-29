/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Sta_Send.c

Abstract:
    STA layer send processing functions
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
#include "precomp.h"
#include "st_scan.h"

#if DOT11_TRACE_ENABLED
#include "Sta_Send.tmh"
#endif

BOOLEAN
Sta11CanTransmit(
    __in  PSTATION        pStation
    )
{
    //
    // If we are in the middle of a scan, this send must be queued
    //
    if (STA_TEST_SCAN_FLAG(pStation, (STA_COMPLETE_PERIODIC_SCAN | STA_COMPLETE_EXTERNAL_SCAN)))
        return FALSE;

    return TRUE;
}

NDIS_STATUS
Sta11SendNetBufferList(
    __in  PSTATION        pStation,
    __in  PNET_BUFFER_LIST    pNetBufferList,
    __in  ULONG           SendFlags,
    __in  PDOT11_EXTSTA_SEND_CONTEXT  pSendContext
    )
{
    UNREFERENCED_PARAMETER(pStation);
    UNREFERENCED_PARAMETER(pNetBufferList);
    UNREFERENCED_PARAMETER(SendFlags);
    UNREFERENCED_PARAMETER(pSendContext);
    
    return NDIS_STATUS_NOT_RECOGNIZED;
}

VOID
Sta11ProcessSend(
    __in  PSTATION        pStation,
    __in  PMP_TX_MSDU     pMpTxd
    )
{
   UNREFERENCED_PARAMETER(pStation);
    UNREFERENCED_PARAMETER(pMpTxd);
}

VOID 
Sta11ProcessSendComplete(
    __in  PSTATION        pStation,
    __in  PMP_TX_MSDU     pMpTxd,
    __in  NDIS_STATUS     ndisStatus
    )
{
    UNREFERENCED_PARAMETER(pStation);
    UNREFERENCED_PARAMETER(pMpTxd);
    UNREFERENCED_PARAMETER(ndisStatus);
}

