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
    File    : tvaudio.h
    Abstract: Contains pin and filter descriptors of the TV Audio filter
**************************************************************************/

#include "common.h"

NTSTATUS TVAudioFilterCreate(IN PKSFILTER Filter, IN PIRP Irp);
NTSTATUS TVAudioFilterClose(IN PKSFILTER Filter, IN PIRP Irp);

const KSFILTER_DISPATCH TVAudioFilterDispatch = 
{
    TVAudioFilterCreate, // Filter Create
    TVAudioFilterClose,  // Filter Close
    NULL,                // Filter Process
    NULL                 // Filter Reset
};


NTSTATUS TVAudioGenericHandler
(
    IN PIRP Irp,
    IN PKSPROPERTY pRequest,
    IN OUT PVOID pData
)
{ 
	UNREFERENCED_PARAMETER(Irp);
	UNREFERENCED_PARAMETER(pRequest);
	UNREFERENCED_PARAMETER(pData);

	return STATUS_SUCCESS;
}

NTSTATUS TVAudioCapsGetHandler
(
    IN PIRP Irp,
    IN PKSPROPERTY_TVAUDIO_CAPS_S pRequest,
    IN OUT PKSPROPERTY_TVAUDIO_CAPS_S pData
);

NTSTATUS TVAudioModeGetHandler
(
    IN PIRP Irp,
    IN PKSPROPERTY_TVAUDIO_S pRequest,
    IN OUT PKSPROPERTY_TVAUDIO_S pData
);

DEFINE_KSPROPERTY_TABLE(TVAudioFilterPropertyTable)
{
	DEFINE_KSPROPERTY_ITEM								
	(
		KSPROPERTY_TVAUDIO_CAPS,						// Property
		TVAudioCapsGetHandler,							// Get Handler
		sizeof(KSPROPERTY_TVAUDIO_CAPS_S),				// Min Property
		sizeof(KSPROPERTY_TVAUDIO_CAPS_S),				// Min Data
		TVAudioCapsGetHandler,							// Set Handler
		NULL,											// Values
		0,												// Relation count
		NULL,											// Relations
		NULL,											// Support Handler
		0												// Serialized Size
	),
	DEFINE_KSPROPERTY_ITEM								
	(
		KSPROPERTY_TVAUDIO_CURRENTLY_AVAILABLE_MODES,	// Property
		TVAudioGenericHandler,							// Get Handler
		sizeof(KSPROPERTY_TVAUDIO_S),					// Min Property
		sizeof(KSPROPERTY_TVAUDIO_S),					// Min Data
		TVAudioGenericHandler,							// Set Handler
		NULL,											// Values
		0,												// Relation count
		NULL,											// Relations
		NULL,											// Support Handler
		0												// Serialized Size
	),
	DEFINE_KSPROPERTY_ITEM								
	(
		KSPROPERTY_TVAUDIO_MODE,						// Property
		TVAudioModeGetHandler,							// Get Handler
		sizeof(KSPROPERTY_TVAUDIO_S),					// Min Property
		sizeof(KSPROPERTY_TVAUDIO_S),					// Min Data
		TVAudioGenericHandler,							// Set Handler
		NULL,											// Values
		0,												// Relations Count
		NULL,											// Relations
		NULL,											// Support Handler
		0												// Serialized Size
	)
};

DEFINE_KSPROPERTY_SET_TABLE(TVAudioFilterPropertySetTable)
{
    DEFINE_KSPROPERTY_SET
    (
        &PROPSETID_VIDCAP_TVAUDIO,                    
        SIZEOF_ARRAY(TVAudioFilterPropertyTable), 
        TVAudioFilterPropertyTable,               
        0,                                          
        NULL                                        
    )
};


DEFINE_KSAUTOMATION_TABLE(TVAudioFilterAutomation) 
{
    DEFINE_KSAUTOMATION_PROPERTIES(TVAudioFilterPropertySetTable),
    DEFINE_KSAUTOMATION_METHODS_NULL,
    DEFINE_KSAUTOMATION_EVENTS_NULL
};

const KSPIN_MEDIUM PinMediumAudioIn =
{
    GUID_AnlgTunerPinMediumAudioOut, 0, 0
};

const KSPIN_MEDIUM PinMediumAudioOut =
{
    GUID_TVAudioPinMediumAudioOut, 0, 0
};



//
const KSPIN_DESCRIPTOR_EX TVAudioPinDescriptors[] =
{ 
    {
	NULL,												// Dispatch
	NULL,												// Automation
	{													// Pin Descriptor
	    0,												// Interfaces Count
	    NULL,											// Interfaces
	    1,												// Mediums Count
	    &PinMediumAudioIn,								// Mediums
	    0,												// size of data range
	    NULL,											// Data Range
	    KSPIN_DATAFLOW_IN,								// Data flow
	    KSPIN_COMMUNICATION_BOTH,						// Communication
	    NULL,											// Category
       (GUID *) &PINNAME_TVAUDIOIN,						// Name
	    0												// Constrained Data Range
	},
	KSPIN_FLAG_DO_NOT_USE_STANDARD_TRANSPORT | 
	KSPIN_FLAG_FRAMES_NOT_REQUIRED_FOR_PROCESSING | 
	KSPIN_FLAG_FIXED_FORMAT,							// Flags
	1,													// Instances possible
	0,													// Instances necessary
	NULL,												// Allocator Framing
	NULL												// Intersect Data Format
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
	    0,												// Data Range
	    KSPIN_DATAFLOW_OUT,								// Data Flow
	    KSPIN_COMMUNICATION_BOTH,						// Communication
	    NULL,											// Category
       (GUID *) &PINNAME_TVAUDIOOUT,					// Name
	    0												// Constrained Data Range
	},
	KSPIN_FLAG_DO_NOT_USE_STANDARD_TRANSPORT | 
	KSPIN_FLAG_FRAMES_NOT_REQUIRED_FOR_PROCESSING | 
	KSPIN_FLAG_FIXED_FORMAT,							// Flags
	1,													// Instances possible
	0,													// Instances necessary
	NULL,												// Allocator Framing
	NULL												// Intersect Data Format
    }
};

const
KSTOPOLOGY_CONNECTION FilterConnections[] =
{   
    { KSFILTER_NODE, 0,                 KSFILTER_NODE, 1},
};

extern
DEFINE_KSFILTER_DESCRIPTOR(BdaTVAudioFilterDescriptor)
{
    &TVAudioFilterDispatch,                                 // Dispatch
    &TVAudioFilterAutomation,                               // AutomationTable
    KSFILTER_DESCRIPTOR_VERSION,                            // Version
    0,                                                      // Flags
    &KSNAME_BdaTVAudioFilter,                               // ReferenceGuid
    DEFINE_KSFILTER_PIN_DESCRIPTORS(TVAudioPinDescriptors), // PinDescriptorsCount
    DEFINE_KSFILTER_CATEGORY(KSCATEGORY_TVAUDIO),           // CategoriesCount
    DEFINE_KSFILTER_NODE_DESCRIPTORS_NULL,					// Node Descriptors
    DEFINE_KSFILTER_CONNECTIONS(FilterConnections),			// Topology
    NULL                                                    // ComponentId
};



