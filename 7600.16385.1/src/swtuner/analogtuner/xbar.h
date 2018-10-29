//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.
//
//==========================================================================;
/**************************************************************************
    File    : xbar.h
    Abstract: Contains pin and filter descriptors for the Crossbar Filter
**************************************************************************/
#ifndef __XBAR_H__
#define __XBAR_H__

#include "common.h"

NTSTATUS XBarFilterCreate(IN PKSFILTER Filter, IN PIRP Irp);
NTSTATUS XBarFilterClose(IN PKSFILTER Filter, IN PIRP Irp);

static NTSTATUS IntersectDataFormat(
        IN PVOID pContext,
        IN PIRP pIrp,
        IN PKSP_PIN Pin,
        IN PKSDATARANGE DataRange,
        IN PKSDATARANGE MatchingDataRange,
        IN ULONG DataBufferSize,
        OUT PVOID Data OPTIONAL,
        OUT PULONG DataSize
        );

static NTSTATUS VideoOutIntersectDataFormat(
        IN PVOID pContext,
        IN PIRP pIrp,
        IN PKSP_PIN Pin,
        IN PKSDATARANGE DataRange,
        IN PKSDATARANGE MatchingDataRange,
        IN ULONG DataBufferSize,
        OUT PVOID Data OPTIONAL,
        OUT PULONG DataSize
        );

static NTSTATUS AudioOutIntersectDataFormat(
        IN PVOID pContext,
        IN PIRP pIrp,
        IN PKSP_PIN Pin,
        IN PKSDATARANGE DataRange,
        IN PKSDATARANGE MatchingDataRange,
        IN ULONG DataBufferSize,
        OUT PVOID Data OPTIONAL,
        OUT PULONG DataSize
        );

const KSFILTER_DISPATCH XBarFilterDispatch = 
{
    XBarFilterCreate,     // Filter Create
    XBarFilterClose,      // Filter Close
    NULL,                 // Filter Process
    NULL                  // Filter Reset
};


NTSTATUS XBarGenericHandler
(
    IN PIRP Irp,
    IN PKSPROPERTY pRequest,
    IN OUT PVOID pVoid
) 
{
	UNREFERENCED_PARAMETER(Irp);
	UNREFERENCED_PARAMETER(pRequest);
	UNREFERENCED_PARAMETER(pVoid);
	return STATUS_SUCCESS;
}

NTSTATUS XBarPinInfoGetHandler
(
    IN PIRP Irp,
    IN PKSPROPERTY_CROSSBAR_PININFO_S pRequest,
    IN OUT PKSPROPERTY_CROSSBAR_PININFO_S pData
);

NTSTATUS XBarCapsGetHandler
(
    IN PIRP Irp,
    IN PKSPROPERTY_CROSSBAR_CAPS_S pRequest,
    IN OUT PKSPROPERTY_CROSSBAR_CAPS_S pData
);

NTSTATUS XBarRouteCanGetHandler
(
    IN PIRP Irp,
    IN PKSPROPERTY_CROSSBAR_ROUTE_S pRequest,
    IN OUT PKSPROPERTY_CROSSBAR_ROUTE_S pData
);

NTSTATUS XBarRouteHandler
(
    IN PIRP pIrp,
    IN PKSPROPERTY_CROSSBAR_ROUTE_S pRequest,
    IN OUT PKSPROPERTY_CROSSBAR_ROUTE_S pData
);


DEFINE_KSPROPERTY_TABLE(XBarFilterPropertyTable)
{
	DEFINE_KSPROPERTY_ITEM								
	(
		KSPROPERTY_CROSSBAR_CAN_ROUTE,					// Property
		XBarRouteCanGetHandler,							// Get Handler
		sizeof(KSPROPERTY_CROSSBAR_ROUTE_S),		    // MinProperty
		sizeof(KSPROPERTY_CROSSBAR_ROUTE_S),			// Min Data
	        XBarGenericHandler,							// Set Handler
		NULL,											// Values
		0,												// RelationsCount
		NULL,											// Relations
		NULL,											// SupportHandler
		0												// SerializedSize
	),
	DEFINE_KSPROPERTY_ITEM								
	(
		KSPROPERTY_CROSSBAR_CAPS,						// Property
		XBarCapsGetHandler,								// Get Handler
		sizeof(KSPROPERTY_CROSSBAR_CAPS_S),				// Min Property
		sizeof(KSPROPERTY_CROSSBAR_CAPS_S),				// Min Data
	        XBarCapsGetHandler,							// Set Handler
		NULL,											// Values
		0,												// RelationsCount
		NULL,											// Relations
		NULL,											// SupportHandler
		0												// SerializedSize
	),
	DEFINE_KSPROPERTY_ITEM								
	(
		KSPROPERTY_CROSSBAR_PININFO,					// Property
		XBarPinInfoGetHandler,							// Get Handler
		sizeof(KSPROPERTY_CROSSBAR_PININFO_S),			// MinProperty
		sizeof(KSPROPERTY_CROSSBAR_PININFO_S),			// Min Data
	        XBarPinInfoGetHandler,						// Set Handler
		NULL,											// Values
		0,												// RelationsCount
		NULL,											// Relations
		NULL,											// SupportHandler
		0												// SerializedSize
	),
	DEFINE_KSPROPERTY_ITEM								
	(
		KSPROPERTY_CROSSBAR_ROUTE,						// Property
		XBarRouteHandler,								// Get Handler
		sizeof(KSPROPERTY_CROSSBAR_ROUTE_S),			// MinProperty
		sizeof(KSPROPERTY_CROSSBAR_ROUTE_S),			// Min Data
	        XBarRouteHandler,							// Set Handler
		NULL,											// Values
		0,												// RelationsCount
		NULL,											// Relations
		NULL,											// SupportHandler
		0												// SerializedSize
	)
};

DEFINE_KSPROPERTY_SET_TABLE(XBarFilterPropertySetTable)
{
    DEFINE_KSPROPERTY_SET
    (
        &PROPSETID_VIDCAP_CROSSBAR,                    // Set
        SIZEOF_ARRAY(XBarFilterPropertyTable),         // PropertiesCount
        XBarFilterPropertyTable,                       // PropertyItems
        0,                                             // FastIoCount
        NULL                                           // FastIoTable
    )
};


DEFINE_KSAUTOMATION_TABLE(XBarFilterAutomation) 
{
    DEFINE_KSAUTOMATION_PROPERTIES(XBarFilterPropertySetTable),
    DEFINE_KSAUTOMATION_METHODS_NULL,
    DEFINE_KSAUTOMATION_EVENTS_NULL
};

const KSPIN_MEDIUM PinMediumAudioIn =
{
    GUID_TVAudioPinMediumAudioOut, 0, 0
};

const KSPIN_MEDIUM PinMediumVideoIn =
{
    GUID_AnlgTunerPinMediumVideoOut, 0, 0
};

const KSPIN_MEDIUM PinMediumAudioAUXIn =
{
    GUID_XBarPinMediumAudioAUX, 0, 0
};

const KSPIN_MEDIUM PinMediumVideoAUXIn =
{
    GUID_XBarPinMediumVideoAUX, 0, 0
};

const KSPIN_MEDIUM PinMediumAudioOut =
{
    GUID_XBarPinMediumAudioOut, 0, 0
};

const KSPIN_MEDIUM PinMediumVideoOut =
{
    GUID_XBarPinMediumVideoOut, 0, 0
};

const KSPIN_MEDIUM PinMediumVBIOut = 
{
    GUID_XBarPinMediumVBIOut, 0, 0
};


const KSPIN_DESCRIPTOR_EX XBarPinDescriptors[] =
{ 
    {
	NULL,												// Dispatch
	NULL,												// Automation
	{													// Pin Descriptor
	    0,												// Interfaces Count
	    NULL,											// Interfaces
	    1,												// Mediums Count
	    &PinMediumVideoIn,								// Mediums
	    0,												// Data Ranges Count
	    NULL,											// Data Ranges
	    KSPIN_DATAFLOW_IN,								// Data Flow
	    KSPIN_COMMUNICATION_BOTH,						// Communication
        NULL,											// Category
        (GUID *) &PINNAME_VIDEOTUNERIN,					// Name
	    0												// Constrained Data Range
	},
	KSPIN_FLAG_DO_NOT_USE_STANDARD_TRANSPORT | 
	KSPIN_FLAG_FRAMES_NOT_REQUIRED_FOR_PROCESSING | 
	KSPIN_FLAG_FIXED_FORMAT,							// Flags
	1,													// Instances possible
	1,													// Instances necessary
	NULL,												// Allocator Framing
	NULL												// Intersect Handler
    },
    {
	NULL,												// Dispatch
	NULL,												// Automation
	{													// Pin Descriptor
	    0,												// Interfaces Count
	    NULL,											// Interfaces
	    1,												// Mediums Count
	    &PinMediumAudioIn,								// Mediums
	    0,												// Data Range Count
	    NULL,											// Data Ranges
	    KSPIN_DATAFLOW_IN,								// Data Flow
	    KSPIN_COMMUNICATION_BOTH,						// Communication
        NULL,											// Category GUID
        (GUID *) &PINNAME_AUDIOTUNERIN,					// Name
	    0												// Constrained Data Range
	},
	KSPIN_FLAG_DO_NOT_USE_STANDARD_TRANSPORT | 
	KSPIN_FLAG_FRAMES_NOT_REQUIRED_FOR_PROCESSING | 
	KSPIN_FLAG_FIXED_FORMAT,							// Flags
	1,													// Instances possible
	0,													// Instances necessary
	NULL,												// Allocator Framing
	NULL												// Intersect Handler
    },
    {
	NULL,												// Dispatch
	NULL,												// Automation
	{													// Pin Descriptor
	    0,												// Interfaces Count
	    NULL,											// Interfaces
	    1,												// Mediums Count
	    &PinMediumVideoAUXIn,								// Mediums
	    0,												// Data Ranges Count
	    NULL,											// Data Ranges
	    KSPIN_DATAFLOW_IN,								// Data Flow
	    KSPIN_COMMUNICATION_BOTH,						// Communication
        NULL,											// Category
        (GUID *) &PINNAME_ANALOGVIDEOIN,					// Name
	    0												// Constrained Data Range
	},
	KSPIN_FLAG_DO_NOT_USE_STANDARD_TRANSPORT | 
	KSPIN_FLAG_FRAMES_NOT_REQUIRED_FOR_PROCESSING | 
	KSPIN_FLAG_FIXED_FORMAT,							// Flags
	1,													// Instances possible
	1,													// Instances necessary
	NULL,												// Allocator Framing
	NULL												// Intersect Handler
    },
    {
	NULL,												// Dispatch
	NULL,												// Automation
	{													// Pin Descriptor
	    0,												// Interfaces Count
	    NULL,											// Interfaces
	    1,												// Mediums Count
	    &PinMediumAudioAUXIn,								// Mediums
	    0,												// Data Range Count
	    NULL,											// Data Ranges
	    KSPIN_DATAFLOW_IN,								// Data Flow
	    KSPIN_COMMUNICATION_BOTH,						// Communication
        NULL,											// Category GUID
        (GUID *) &PINNAME_ANALOGAUDIOIN,					// Name
	    0												// Constrained Data Range
	},
	KSPIN_FLAG_DO_NOT_USE_STANDARD_TRANSPORT | 
	KSPIN_FLAG_FRAMES_NOT_REQUIRED_FOR_PROCESSING | 
	KSPIN_FLAG_FIXED_FORMAT,							// Flags
	1,													// Instances possible
	0,													// Instances necessary
	NULL,												// Allocator Framing
	NULL												// Intersect Handler
    },
    {
	NULL,												// Dispatch
	NULL,												// Automation
	{													// Pin Descriptor
	    0,												// Interfaces Count
	    NULL,											// Interfaces
	    1,												// Mediums Count
	    &PinMediumVideoAUXIn,								// Mediums
	    0,												// Data Ranges Count
	    NULL,											// Data Ranges
	    KSPIN_DATAFLOW_IN,								// Data Flow
	    KSPIN_COMMUNICATION_BOTH,						// Communication
        NULL,											// Category
        (GUID *) &PINNAME_ANALOGVIDEOIN,					// Name
	    0												// Constrained Data Range
	},
	KSPIN_FLAG_DO_NOT_USE_STANDARD_TRANSPORT | 
	KSPIN_FLAG_FRAMES_NOT_REQUIRED_FOR_PROCESSING | 
	KSPIN_FLAG_FIXED_FORMAT,							// Flags
	1,													// Instances possible
	1,													// Instances necessary
	NULL,												// Allocator Framing
	NULL												// Intersect Handler
    },
    {
	NULL,												// Dispatch
	NULL,												// Automation
	{													// Pin Descriptor
	    0,												// Interfaces Count
	    NULL,											// Interfaces
	    1,												// Mediums Count
	    &PinMediumAudioAUXIn,								// Mediums
	    0,												// Data Range Count
	    NULL,											// Data Ranges
	    KSPIN_DATAFLOW_IN,								// Data Flow
	    KSPIN_COMMUNICATION_BOTH,						// Communication
        NULL,											// Category GUID
        (GUID *) &PINNAME_ANALOGAUDIOIN,					// Name
	    0												// Constrained Data Range
	},
	KSPIN_FLAG_DO_NOT_USE_STANDARD_TRANSPORT | 
	KSPIN_FLAG_FRAMES_NOT_REQUIRED_FOR_PROCESSING | 
	KSPIN_FLAG_FIXED_FORMAT,							// Flags
	1,													// Instances possible
	0,													// Instances necessary
	NULL,												// Allocator Framing
	NULL												// Intersect Handler
    },
    {
	NULL,												// Dispatch
	NULL,												// Automation
	{													// Pin Descriptor
	    0,												// Interfaces Count
	    NULL,											// Interfaces
	    1,												// Mediums Count
	    &PinMediumVideoOut,								// Mediums
	    0,												// Data Range Count
	    NULL,											// Data Ranges
	    KSPIN_DATAFLOW_OUT,								// Data Flow
	    KSPIN_COMMUNICATION_BOTH,						// Communication
            NULL,										// Category
            (GUID *) &PINNAME_VIDEODECOUT,				// Name
	    0												// Constrained Data Range
	},
	KSPIN_FLAG_DO_NOT_USE_STANDARD_TRANSPORT | 
	KSPIN_FLAG_FRAMES_NOT_REQUIRED_FOR_PROCESSING | 
	KSPIN_FLAG_FIXED_FORMAT,							// Flags
	1,													// Instances possible
	1,													// Instances necessary
	NULL,												// Allocator Framing
	NULL												// Intersect Handler
    },
    {
	NULL,												// Dispatch
	NULL,												// Automation
	{													// Pin Descriptor
	    0,												// Interfaces Count
	    NULL,											// Interfaces
	    1,												// Mediums Count
	    &PinMediumAudioOut,								// Mediums
	    0,												// Data Range Count
	    NULL,											// Data Ranges
	    KSPIN_DATAFLOW_OUT,								// Data Flow
	    KSPIN_COMMUNICATION_BOTH,						// Communication
            NULL,										// Category
            (GUID *) &PINNAME_AUDIODECOUT,				// Name
	    0												// Constrained Data Range
	},
	KSPIN_FLAG_DO_NOT_USE_STANDARD_TRANSPORT | 
	KSPIN_FLAG_FRAMES_NOT_REQUIRED_FOR_PROCESSING | 
	KSPIN_FLAG_FIXED_FORMAT,							// Flags
	1,													// Instances possible
	0,													// Instances necessary
	NULL,												// Allocator Framing
	NULL												// Intersect Handler
    }
};

const
KSTOPOLOGY_CONNECTION FilterConnections[] =
{
    { KSFILTER_NODE, 0,                 KSFILTER_NODE, 2 },
    { KSFILTER_NODE, 1,                 KSFILTER_NODE, 3 }
};

extern
DEFINE_KSFILTER_DESCRIPTOR(BdaXBarFilterDescriptor)
{
    &XBarFilterDispatch,								// Dispatch
    &XBarFilterAutomation,								// AutomationTable
    KSFILTER_DESCRIPTOR_VERSION,						// Version
    0,													// Flags
    &KSNAME_BdaXBarFilter,								// ReferenceGuid
    DEFINE_KSFILTER_PIN_DESCRIPTORS(XBarPinDescriptors),// PinDescriptorsCount
    DEFINE_KSFILTER_CATEGORY(KSCATEGORY_CROSSBAR),		// CategoriesCount
    DEFINE_KSFILTER_NODE_DESCRIPTORS_NULL,				// Node Descriptors
    DEFINE_KSFILTER_CONNECTIONS(FilterConnections),		// Topology
    NULL												// ComponentId
};

#endif

