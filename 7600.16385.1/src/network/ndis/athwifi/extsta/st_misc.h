/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    st_misc.h

Abstract:
    STA layer utility functions
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created
    01-15-2006    Renamed sta_misc.h to st_misc.h

Notes:

--*/
#ifndef _STATION_MISC_H
#define _STATION_MISC_H

#define STA_INVALID_PHY_ID          0x80000000U


ULONG
StaGetPhyIdByType(
    __in PMP_EXTSTA_PORT pStation,
    __in DOT11_PHY_TYPE PhyType
    );

DOT11_PHY_TYPE
StaGetPhyTypeById(
    __in PMP_EXTSTA_PORT pStation,
    __in ULONG PhyId
    );

NDIS_STATUS
StaFilterUnsupportedRates(
    __in PMP_BSS_ENTRY pAPEntry, 
    __in PDOT11_RATE_SET rateSet
    );


NDIS_STATUS
StaAttachInfraRSNIE(
    __in PMP_EXTSTA_PORT pStation, 
    __in PMP_BSS_ENTRY pAPEntry, 
    __inout PUCHAR *ppCurrentIE,
    __inout PUSHORT pIESize
    );

NDIS_STATUS
StaAttachAdHocRSNIE(
    __in PMP_EXTSTA_PORT pStation, 
    __inout PUCHAR *ppCurrentIE,
    __inout PUSHORT pIESize
    );

BOOLEAN
StaMatchRSNInfo(
    __in PMP_EXTSTA_PORT        pStation,
    __in PRSN_IE_INFO    RSNIEInfo
    );

VOID 
StaIndicateDot11Status(
    __in PMP_EXTSTA_PORT        pStation,
    __in  NDIS_STATUS     StatusCode,
    __in_opt  PVOID           RequestID,
    __in  PVOID           pStatusBuffer,
    __in  ULONG           StatusBufferSize
    );

VOID
Sta11Notify(
    __in  PMP_PORT        Port,
    PVOID               Notif
);

#endif // _STATION_MAIN_H

