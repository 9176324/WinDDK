/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    hw_phy.h

Abstract:
    Contains defines used for phy functionality 
    in the HW layer
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    09-04-2007    Created

Notes:

--*/
#pragma once

BOOLEAN
HwQueryShortSlotTimeOptionImplemented(
    __in  PHW                     Hw,
    __in  ULONG                   PhyId
    );


BOOLEAN
HwQueryDsssOfdmOptionImplemented(
    __in  PHW                     Hw,
    __in  ULONG                   PhyId
    );

BOOLEAN
HwQueryShortPreambleOptionImplemented(
    __in  PHW                     Hw,
    __in  ULONG                   PhyId
    );


BOOLEAN
HwQueryPbccOptionImplemented(
    __in  PHW                     Hw,
    __in  ULONG                   PhyId
    );

BOOLEAN
HwQueryChannelAgilityPresent(
    __in  PHW                     Hw,
    __in  ULONG                   PhyId
    );

BOOLEAN
HwQueryNicPowerState(
    __in  PHW                     Hw,
    __in  ULONG                   PhyId
    );

BOOLEAN
HwQueryHardwarePhyState(
    __in  PHW                     Hw,
    __in  ULONG                   PhyId
    );

BOOLEAN
HwQuerySoftwarePhyState(
    __in  PHW                     Hw,
    __in  ULONG                   PhyId
    );


NDIS_STATUS
HwQueryDiversitySelectionRX(
    __in  PHW                     Hw,
    __in  ULONG                   PhyId,
    __in  ULONG                   MaxEntries,
    __out PDOT11_DIVERSITY_SELECTION_RX_LIST Dot11DiversitySelectionRXList
    );

NDIS_STATUS
HwQueryRegDomainsSupportValue(
    __in  PHW                     Hw,
    __in  ULONG                   PhyId,
    __in  ULONG                   MaxEntries,
    __out PDOT11_REG_DOMAINS_SUPPORT_VALUE    Dot11RegDomainsSupportValue
    );

LONG
HwQueryMinRSSI(
    __in  PHW                     Hw,
    __in  ULONG                   DataRate
    );

LONG
HwQueryMaxRSSI(
    __in  PHW                     Hw,
    __in  ULONG                   DataRate
    );

NDIS_STATUS
HwQuerySupportedDataRatesValue(
    __in  PHW                     Hw,
    __in  ULONG                   PhyId,
    __out PDOT11_SUPPORTED_DATA_RATES_VALUE_V2    Dot11SupportedDataRatesValue
    );

ULONG
HwQueryCCAModeSupported(
    __in  PHW                     Hw,
    __in  ULONG                   PhyId
    );


ULONG
HwQueryCurrentTXPowerLevel(
    __in  PHW                     Hw,
    __in  ULONG                   PhyId
    );


DOT11_DIVERSITY_SUPPORT
HwQueryDiversitySupport(
    __in  PHW                     Hw,
    __in  ULONG                   PhyId
    );


ULONG
HwQueryEDThreshold(
    __in  PHW                     Hw,
    __in  ULONG                   PhyId
    );


ULONG
HwQueryFrequencyBandsSupported(
    __in  PHW                     Hw,
    __in  ULONG                   PhyId
    );


BOOLEAN
HwQueryMultiDomainCapabilityImplemented(
    __in  PHW                     Hw
    );

DOT11_TEMP_TYPE
HwQueryTempType(
    __in  PHW                     Hw,
    __in  ULONG                   PhyId
    );

NDIS_STATUS
HwPersistRadioPowerState(
    __in  PHW                     Hw,
    __in  BOOLEAN                 RadioOff
    );

NDIS_STATUS
HwSetNicPowerState(
    __in  PHW                     Hw,
    __in  ULONG                   PhyId,
    __in  BOOLEAN                 PowerOn
    );

BOOLEAN
HwAwake(
    __in  PHW                     Hw,
	__in  BOOLEAN                 DeviceIRQL
	);

NDIS_TIMER_FUNCTION HwAwakeTimer;

NDIS_TIMER_FUNCTION HwDozeTimer;

BOOLEAN
HwSetRFState(
    __in  PHW                     Hw,
    __in  UCHAR	                NewRFState
    );

BOOLEAN
HwSetRFOn(
    __in  PHW                     Hw,
	__in  UCHAR                   MaxRetries
	);

NDIS_STATUS
HwSetOperationalRateSet(
    __in  PHW                     Hw,
    __in  ULONG                   PhyId,
    __in  PDOT11_RATE_SET         Dot11RateSet
    );

NDIS_STATUS
HwValidateChannel(
    __in  PHW                     Hw,
    __in  ULONG                   PhyId,
    __in  UCHAR                   Channel
    );

NDIS_STATUS
HwSetChannel(
    __in  PHW                     Hw,
    __in  ULONG                   PhyId,
    __in  UCHAR                   Channel
    );

NDIS_STATUS
HwSetOperatingPhyId(
    __in  PHW                     Hw,
    __in  ULONG                   PhyId
    );

NDIS_STATUS
HwSetPhyContext(
    __in  PHW                     Hw,
    __in  ULONG                   PhyId,
    __in  PHW_PHY_CONTEXT         PhyContext
    );

NDIS_IO_WORKITEM_FUNCTION HwPhyProgramWorkItem;

NDIS_STATUS
HwProgramPhy(
    __in  PHW                     Hw,
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  ULONG                   PhyId,
    __in  PHW_PHY_CONTEXT         PhyContext,
    __in_opt  HW_GENERIC_CALLBACK_FUNC    CompletionCallback
    );


