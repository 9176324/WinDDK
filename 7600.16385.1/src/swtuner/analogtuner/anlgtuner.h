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

#ifndef __ANLGTUNER_H__
#define __ANLGTUNER_H__

/**************************************************************************
    File    : anlgtuner.h
    Abstract: Contains pin and filter descriptors of the tuner filter
**************************************************************************/

#include "common.h"

NTSTATUS AnlgTunerFilterCreate(IN PKSFILTER Filter, IN PIRP Irp);
NTSTATUS AnlgTunerFilterClose(IN PKSFILTER Filter, IN PIRP Irp);

const KSFILTER_DISPATCH AnlgTunerFilterDispatch = 
{
    AnlgTunerFilterCreate, // Filter Create
    AnlgTunerFilterClose,  // Filter Close
    NULL,                  // Filter Process
    NULL                   // Filter Reset
};


NTSTATUS AnlgTunerFrequencyGetHandler
(
    IN PIRP Irp,
    IN PKSPROPERTY_TUNER_FREQUENCY_S pRequest,
    IN OUT PKSPROPERTY_TUNER_FREQUENCY_S pData
);

NTSTATUS AnlgTunerFrequencySetHandler
(
    IN PIRP Irp,
    IN PKSPROPERTY_TUNER_FREQUENCY_S pRequest,
    IN OUT PKSPROPERTY_TUNER_FREQUENCY_S pData
);

NTSTATUS AnlgTunerInputGetHandler
(
    IN PIRP Irp,
    IN PKSPROPERTY_TUNER_INPUT_S pRequest,
    IN OUT PKSPROPERTY_TUNER_INPUT_S pData
);

NTSTATUS AnlgTunerModeGetHandler
(
    IN PIRP Irp,
    IN PKSPROPERTY_TUNER_MODE_S pRequest,
    IN OUT PKSPROPERTY_TUNER_MODE_S pData

); 

NTSTATUS AnlgTunerCapsGetHandler
(
    IN PIRP Irp,
    IN PKSPROPERTY_TUNER_CAPS_S pRequest,
    IN OUT PKSPROPERTY_TUNER_CAPS_S pData
); 
NTSTATUS AnlgTunerModeCapsGetHandler
(
    IN PIRP Irp,
    IN PKSPROPERTY_TUNER_MODE_CAPS_S pRequest,
    IN OUT PKSPROPERTY_TUNER_MODE_CAPS_S pData
); 
NTSTATUS AnlgTunerStatusGetHandler
(
    IN PIRP Irp,
    IN PKSPROPERTY_TUNER_STATUS_S pRequest,
    IN OUT PKSPROPERTY_TUNER_STATUS_S pData
); 

NTSTATUS AnlgTunerStandardGetHandler
(
    IN PIRP Irp,
    IN PKSPROPERTY_TUNER_STANDARD_S pRequest,
    IN OUT PKSPROPERTY_TUNER_STANDARD_S pData
); 

NTSTATUS AnlgTunerGenericHandler
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
/*
NTSTATUS AnlgTunerModeCaps2GetHandler
(
    IN PIRP Irp,
    IN PKSPROPERTY_TUNER_MODE_CAPS2_S pRequest,
    IN OUT PKSPROPERTY_TUNER_MODE_CAPS2_S pData
); 
*/
NTSTATUS AnlgTunerScanStatusGetHandler
(
    IN PIRP Irp,
    IN PKSPROPERTY_TUNER_SCAN_STATUS_S pRequest,
    IN OUT PKSPROPERTY_TUNER_SCAN_STATUS_S pData
); 

NTSTATUS AnlgTunerStandardModeGetHandler
(
    IN PIRP Irp,
    IN PKSPROPERTY_TUNER_STANDARD_MODE_S pRequest,
    IN OUT PKSPROPERTY_TUNER_STANDARD_MODE_S pData
); 

NTSTATUS AnlgTunerStandardModeSetHandler
(
    IN PIRP Irp,
    IN PKSPROPERTY_TUNER_STANDARD_MODE_S pRequest,
    IN OUT PKSPROPERTY_TUNER_STANDARD_MODE_S pData
); 
NTSTATUS AnlgTunerScanCapsGetHandler
(
    IN PIRP Irp,
    IN PKSPROPERTY_TUNER_SCAN_CAPS_S pRequest,
    IN OUT PKSPROPERTY_TUNER_SCAN_CAPS_S pData
); 
NTSTATUS AnlgTunerAnalogCapsGetHandler
(
    IN PIRP Irp,
    IN PKSPROPERTY_TUNER_NETWORKTYPE_SCAN_CAPS_S pRequest,
    IN OUT PKSPROPERTY_TUNER_NETWORKTYPE_SCAN_CAPS_S pData
); 

DEFINE_KSPROPERTY_TABLE(AnlgTunerFilterPropertyTable)
{
    DEFINE_KSPROPERTY_ITEM                              
    (

        KSPROPERTY_TUNER_FREQUENCY,                     // Property
        AnlgTunerFrequencyGetHandler,                   // Get Handler
        sizeof(KSPROPERTY_TUNER_FREQUENCY_S),           // MinProperty
        sizeof(KSPROPERTY_TUNER_FREQUENCY_S),           // MinData
        AnlgTunerFrequencySetHandler,                   // Set Handler
        NULL,                                           // Values
        0,                                              // RelationsCount
        NULL,                                           // Relations
        NULL,                                           // SupportHandler
        0                                               // SerializedSize
        ),
    DEFINE_KSPROPERTY_ITEM                              
    (
        KSPROPERTY_TUNER_CAPS,                          // Property
        AnlgTunerCapsGetHandler,                        // Get Handler
        sizeof(KSPROPERTY_TUNER_CAPS_S),                // MinProperty
        sizeof(KSPROPERTY_TUNER_CAPS_S),                // MinData
        AnlgTunerCapsGetHandler,                        // Set Handler
        NULL,                                           // Values
        0,                                              // RelationsCount
        NULL,                                           // Relations
        NULL,                                           // SupportHandler
        0                                               // SerializedSize
        ),
    DEFINE_KSPROPERTY_ITEM                              
    (
        KSPROPERTY_TUNER_IF_MEDIUM,                     // Property
        AnlgTunerGenericHandler,                        // Get Handler
        sizeof(KSPROPERTY),                             // MinProperty
        0,                                              // MinData
        AnlgTunerGenericHandler,                        // Set Handler
        NULL,                                           // Values
        0,                                              // RelationsCount
        NULL,                                           // Relations
        NULL,                                           // SupportHandler
        0                                               // SerializedSize
        ),
    DEFINE_KSPROPERTY_ITEM                              
    (
        KSPROPERTY_TUNER_INPUT,                         // Property
        AnlgTunerInputGetHandler,                       // Get Handler
        sizeof(KSPROPERTY_TUNER_INPUT_S),               // MinProperty
        sizeof(KSPROPERTY_TUNER_INPUT_S),               // MinData
        AnlgTunerInputGetHandler,                       // Set Handler
        NULL,                                           // Values
        0,                                              // RelationsCount
        NULL,                                           // Relations
        NULL,                                           // SupportHandler
        0                                               // SerializedSize
        ),
    DEFINE_KSPROPERTY_ITEM                              
    (
        KSPROPERTY_TUNER_MODE,                          // Property
        AnlgTunerModeGetHandler,                        // Get Handler
        sizeof(KSPROPERTY_TUNER_MODE_S),                // MinProperty
        sizeof(KSPROPERTY_TUNER_MODE_S),                // MinData
        AnlgTunerModeGetHandler,                        // Set Handler
        NULL,                                           // Values
        0,                                              // RelationsCount
        NULL,                                           // Relations
        NULL,                                           // SupportHandler
        0                                               // SerializedSize
        ),
    DEFINE_KSPROPERTY_ITEM                              
    (
        KSPROPERTY_TUNER_MODE_CAPS,                     // Property
        AnlgTunerModeCapsGetHandler,                    // Get Handler
        sizeof(KSPROPERTY_TUNER_MODE_CAPS_S),           // MinProperty
        sizeof(KSPROPERTY_TUNER_MODE_CAPS_S),           // MinData
        AnlgTunerModeCapsGetHandler,                    // Set Handler
        NULL,                                           // Values
        0,                                              // RelationsCount
        NULL,                                           // Relations
        NULL,                                           // SupportHandler
        0                                               // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM                              
    (
        KSPROPERTY_TUNER_STANDARD,                      // Property
        AnlgTunerStandardGetHandler,                    // Get Handler
        sizeof(KSPROPERTY_TUNER_STANDARD_S),            // MinProperty
        sizeof(KSPROPERTY_TUNER_STANDARD_S),            // MinData
        AnlgTunerStandardGetHandler,                    // Set Handler
        NULL,                                           // Values
        0,                                              // RelationsCount
        NULL,                                           // Relations
        NULL,                                           // SupportHandler
        0                                               // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM                              
    (
        KSPROPERTY_TUNER_STATUS,                        // Property
        AnlgTunerStatusGetHandler,                      // Get Handler
        sizeof(KSPROPERTY_TUNER_STATUS_S),              // MinProperty
        sizeof(KSPROPERTY_TUNER_STATUS_S),              // MinData
        AnlgTunerStatusGetHandler,                      // Set Handler
        NULL,                                           // Values
        0,                                              // RelationsCount
        NULL,                                           // Relations
        NULL,                                           // SupportHandler
        0                                               // SerializedSize
     ),
/*  DEFINE_KSPROPERTY_ITEM                              
    (
        KSPROPERTY_TUNER_SCAN_CAPS,                         // Property
        AnlgTunerModeCaps2GetHandler,                       // Get Handler
        sizeof(KSPROPERTY_TUNER_SCAN_CAPS_S),               // MinProperty
        sizeof(KSPROPERTY_TUNER_SCAN_CAPS_S),               // MinData
        AnlgTunerModeCaps2GetHandler,                       // Set Handler
        NULL,                                           // Values
        0,                                              // RelationsCount
        NULL,                                           // Relations
        NULL,                                           // SupportHandler
        0                                               // SerializedSize
        ),*/
    DEFINE_KSPROPERTY_ITEM                              
    (
        KSPROPERTY_TUNER_SCAN_CAPS,                         // Property
        AnlgTunerScanCapsGetHandler,                        // Get Handler
        sizeof(KSPROPERTY_TUNER_SCAN_CAPS_S),               // MinProperty
        sizeof(KSPROPERTY_TUNER_SCAN_CAPS_S),               // MinData
        AnlgTunerScanCapsGetHandler,                        // Set Handler
        NULL,                                           // Values
        0,                                              // RelationsCount
        NULL,                                           // Relations
        NULL,                                           // SupportHandler
        0                                               // SerializedSize
        ),
    DEFINE_KSPROPERTY_ITEM                              
    (
        KSPROPERTY_TUNER_SCAN_STATUS,                           // Property
        AnlgTunerScanStatusGetHandler,                      // Get Handler
        sizeof(KSPROPERTY_TUNER_SCAN_STATUS_S),             // MinProperty
        sizeof(KSPROPERTY_TUNER_SCAN_STATUS_S),             // MinData
        AnlgTunerScanStatusGetHandler,                      // Set Handler
        NULL,                                           // Values
        0,                                              // RelationsCount
        NULL,                                           // Relations
        NULL,                                           // SupportHandler
        0                                               // SerializedSize
        ),
    DEFINE_KSPROPERTY_ITEM                              
    (
        KSPROPERTY_TUNER_STANDARD_MODE,                     // Property
        AnlgTunerStandardModeGetHandler,                    // Get Handler
        sizeof(KSPROPERTY_TUNER_STANDARD_MODE_S),           // MinProperty
        sizeof(KSPROPERTY_TUNER_STANDARD_MODE_S),           // MinData
        AnlgTunerStandardModeSetHandler,                    // Set Handler
        NULL,                                           // Values
        0,                                              // RelationsCount
        NULL,                                           // Relations
        NULL,                                           // SupportHandler
        0                                               // SerializedSize
    ),
    DEFINE_KSPROPERTY_ITEM                              
    (
        KSPROPERTY_TUNER_NETWORKTYPE_SCAN_CAPS,             // Property
        AnlgTunerAnalogCapsGetHandler,                  // Get Handler
        sizeof(KSPROPERTY_TUNER_NETWORKTYPE_SCAN_CAPS_S),   // MinProperty
        sizeof(KSPROPERTY_TUNER_NETWORKTYPE_SCAN_CAPS_S),   // MinData
        AnlgTunerAnalogCapsGetHandler,                  // Set Handler
        NULL,                                           // Values
        0,                                              // RelationsCount
        NULL,                                           // Relations
        NULL,                                           // SupportHandler
        0                                               // SerializedSize
    )

};

DEFINE_KSEVENT_TABLE(AnlgTunerEventTable)
{
    DEFINE_KSEVENT_ITEM
    (
        KSEVENT_TUNER_INITIATE_SCAN,
        sizeof(KSEVENT_TUNER_INITIATE_SCAN_S),
        0,
        AnlgTunerScanAddEvent,
        AnlgTunerScanRemoveEvent,
        NULL
    )
};


DEFINE_KSPROPERTY_SET_TABLE(AnlgTunerFilterPropertySetTable)
{
    DEFINE_KSPROPERTY_SET
    (
        &PROPSETID_TUNER,                           // Set
        SIZEOF_ARRAY(AnlgTunerFilterPropertyTable), // PropertiesCount
        AnlgTunerFilterPropertyTable,               // PropertyItems
        0,                                          // FastIoCount
        NULL                                        // FastIoTable
    )
};

// Event sets supported by the filter
DEFINE_KSEVENT_SET_TABLE(AnlgTunerFilterEventSetTable)
{
    DEFINE_KSEVENT_SET
    (
        &EVENTSETID_TUNER,
        SIZEOF_ARRAY(AnlgTunerEventTable),
        AnlgTunerEventTable
    )
};

DEFINE_KSAUTOMATION_TABLE(AnlgTunerFilterAutomation) 
{
    DEFINE_KSAUTOMATION_PROPERTIES(AnlgTunerFilterPropertySetTable),
    DEFINE_KSAUTOMATION_METHODS_NULL,
    DEFINE_KSAUTOMATION_EVENTS(AnlgTunerFilterEventSetTable)
};

const KSPIN_MEDIUM PinMediumAudioOut =
{
    GUID_AnlgTunerPinMediumAudioOut, 0, 0
};

const KSPIN_MEDIUM PinMediumVideoOut =
{
    GUID_AnlgTunerPinMediumVideoOut, 0, 0
};


const KSPIN_DESCRIPTOR_EX AnlgTunerFilterPinDescriptors[] =
{ 
    {
    NULL,                                           // Dispatch
    NULL,                                           // Automation
    {                                               // Pin Descriptor
        0,                                          // Interfaces Count
        NULL,                                       // Interfaces
        1,                                          // Mediums Count
        &PinMediumVideoOut,                         // Mediums
        0,                                          // Size of data range
        NULL,                                       // Data Range
        KSPIN_DATAFLOW_OUT,                         // Data Flow
        KSPIN_COMMUNICATION_BOTH,                   // Communication
        (GUID *) &KSCATEGORY_VIDEO,                 // Category GUID
        (GUID *) &PINNAME_ANALOGVIDEOOUT,           // Pin Name
        0                                           // Constrained Data Ranges
    },
    KSPIN_FLAG_DO_NOT_USE_STANDARD_TRANSPORT | 
    KSPIN_FLAG_FRAMES_NOT_REQUIRED_FOR_PROCESSING | 
    KSPIN_FLAG_FIXED_FORMAT,                        // Flags
    1,                                              // Instances possible
    1,                                              // Instances necessary
    NULL,                                           // Allocator Framing
    NULL                                            // Intersect Handler
    },
    {
    NULL,                               // Dispatch
    NULL,                                           // Automation
    {                                               // Pin Descriptor
        0,                                          // Interfaces Count
        NULL,                                       // Interfaces
        1,                                          // Mediums Count
        &PinMediumAudioOut,                         // Mediums
        0,                                          // Size of data range
        NULL,                                       // Data Range
        KSPIN_DATAFLOW_OUT,                         // Data Flow
        KSPIN_COMMUNICATION_BOTH,                   // Communication
            (GUID *) &KSCATEGORY_AUDIO,             // Category GUID
            (GUID *) &PINNAME_ANALOGAUDIOOUT,       // GUID of the localized Unicode string 
        0                                           // Constrained Data Ranges
    },
    KSPIN_FLAG_DO_NOT_USE_STANDARD_TRANSPORT | 
    KSPIN_FLAG_FRAMES_NOT_REQUIRED_FOR_PROCESSING | 
    KSPIN_FLAG_FIXED_FORMAT,                        // Flags
    1,                                              // Instances possible
    0,                                              // Instances necessary
    NULL,                                           // Allocator Framing
    NULL                                            // Intersect Handler
    }
};

// {CB63007C-9ACB-43f7-8020-1EF8FC1EFA41}
#define STATIC_KSCATEGORY_MCEANALOG_TUNER\
    0xCB63007CL, 0x9ACB, 0x43f7, 0x80, 0x20, 0x1E, 0xF8, 0xFC, 0x1E, 0xFA, 0x41
DEFINE_GUIDSTRUCT("CB63007C-9ACB-43f7-8020-1EF8FC1EFA41", KSCATEGORY_MCEANALOG_TUNER);
#define KSCATEGORY_MCEANALOG_TUNER DEFINE_GUIDNAMED(KSCATEGORY_MCEANALOG_TUNER)


extern
DEFINE_KSFILTER_DESCRIPTOR(BdaAnlgTunerFilterDescriptor)
{
    &AnlgTunerFilterDispatch,                                       // Dispatch
    &AnlgTunerFilterAutomation,                                     // AutomationTable
    KSFILTER_DESCRIPTOR_VERSION,                                    // Version
    0,                                                              // Flags
    &KSNAME_BdaSWAnlgTunerFilter,                                   // ReferenceGuid
    DEFINE_KSFILTER_PIN_DESCRIPTORS(AnlgTunerFilterPinDescriptors), // PinDescriptorsCount
    DEFINE_KSFILTER_CATEGORY(KSCATEGORY_TVTUNER),           // CategoriesCount
    DEFINE_KSFILTER_NODE_DESCRIPTORS_NULL,                          // Node Descriptors
    DEFINE_KSFILTER_DEFAULT_CONNECTIONS,                            // Topology
    NULL                                                            // ComponentId
};

#endif

