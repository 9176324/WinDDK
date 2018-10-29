/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    ap_action.h

Abstract:
    ExtAP actions such as start, stop
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    10-01-2007    Created
    
Notes:

--*/
#pragma once
    
#ifndef _AP_ACTION_H
#define _AP_ACTION_H

/** Start AP */
NDIS_STATUS
StartExtAp(
    __in PMP_EXTAP_PORT ApPort
    );

/** VNic Start BSS completion handler */
NDIS_STATUS
Ap11StartBSSCompleteCallback(
    __in PMP_PORT Port,
    __in PVOID Data
    );

/** Stop AP */
NDIS_STATUS
StopExtAp(
    __in PMP_EXTAP_PORT ApPort
    );

/** Stop Ext AP BSS */
NDIS_STATUS
StopExtApBss(
    __in PMP_EXTAP_PORT ApPort
    );


/** VNic stop BSS completion handler */
NDIS_STATUS
Ap11StopBSSCompleteCallback(
    __in PMP_PORT Port,
    __in PVOID Data
    );

/** Restart AP */
NDIS_STATUS
RestartExtAp(
    __in PMP_EXTAP_PORT ApPort
    );

/** Pause AP */
NDIS_STATUS
PauseExtAp(
    __in PMP_EXTAP_PORT ApPort
    );

NDIS_STATUS
Ap11SetChannelCompleteCallback(
    __in PMP_PORT Port,
    __in PVOID Data
    );

NDIS_STATUS
ConstructAPIEBlob(
    __in PMP_EXTAP_PORT ApPort,
    PUCHAR ieBlobPtr,
    BOOLEAN bResponseIE,
    USHORT *pIEBlobSize,
    USHORT *pbytesWritten
    );

#endif // _AP_ACTION_H


