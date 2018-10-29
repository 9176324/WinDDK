/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Sta_Oids.h

Abstract:
    STA layer OID handling functions
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
#ifndef _STATION_OIDS_H_
#define _STATION_OIDS_H_

typedef NDIS_STATUS (*STA_QUERY_ALGO_PAIR_FUNC)(
    IN  PSTATION                        pStation,
    __out PDOT11_AUTH_CIPHER_PAIR_LIST    pAuthCipherList,
    IN  ULONG                           TotalLength
    );

NDIS_STATUS
StaSetPowerSavingLevel(
    __in   PSTATION pStation,
    __in  ULONG PowerSavingLevel
    );

#endif // _STATION_OIDS_H_

