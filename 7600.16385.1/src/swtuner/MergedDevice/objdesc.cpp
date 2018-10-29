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

#include "BDATuner.h"

#ifdef ALLOC_DATA_PRAGMA
#pragma const_seg("PAGECONST")
#endif // ALLOC_DATA_PRAGMA

#ifdef ALLOC_PRAGMA
#pragma code_seg("PAGE")
#endif // ALLOC_PRAGMA


//
// Before defining structures for nodes, pins, and filters, 
// it may be useful to show the filter's template topology here. 
//
// The sample filter's topology:
//
//                     Node Type 0           Node Type 1
//                          |                     |
//                          v                     v   
//                   ---------------     --------------------
//   Antenna         |             |     |                  |             Transport 
//   In Pin  --------|  Tuner Node |--X--| Demodulator Node |------------   Out Pin
//     ^       ^     |             |  ^  |                  | ^                ^
//     |       |     ---------------  |  -------------------- |                |
//     |       |                      |                       |                |
//     |       -- Connection 0        -- Connection 1         -- Connection 2  |
//     |                                 Topology Joint                        |
//     ---- Pin Type 0                                           Pin Type 1 ----
//
//

//===========================================================================
//
//  Node  definitions
//
//  Nodes are special in that, though they are defined at the filter level,
//  they are actually associated with a pin type.  The filter's node
//  descriptor list is actually a list of node types.
//  
//  BDA allows a node type to appear only once in a template topology.
//  This means that a node in an actual filter topology can be uniquely
//  identified by specifying the node type along with the actual input and
//  output pin IDs of the the path that the node appears in.
//
//  Note that the dispatch routines for nodes actually point to
//  pin-specific methods because the context data associated with
//  a node is stored in the pin context.
//
//  Node property begin with a KSP_NODE structure that contains the node type of the node to which
//  that property applies. This begs the question:
//
//  "How is a node uniquely identified by only the node type?"
//
//  The BDA Support Library uses the concept of a topology joint to solve
//  this problem.  Nodes upstream of a topology joint have their properties
//  dispatched to the input pin of the path.  Properties for nodes
//  downstream of the joint are dispatched to the output pin of the path
//  containing the node.
//
//  Node properties and methods should only be accessed from the 
//  appropriate pin.  The BDA Support Library helps assure this by
//  automatically merging a node type's automation table onto the automation
//  table of the correct pin.  This pin is called the controlling pin
//  for that node type.
//
//===========================================================================

typedef enum {
    BDA_SAMPLE_TUNER_NODE = 0,
    BDA_SAMPLE_DEMODULATOR_NODE,
#ifdef USE_DRM_SERVICE
    BDA_SAMPLE_DRM_NODE,
    BDA_SAMPLE_CORECAS_NODE,
#if defined(ISDBT_RECEIVER) || defined(ISDBS_RECEIVER)
    BDA_SAMPLE_ISDBCAS_NODE,
#endif
#endif
    BDA_MUX_SERVICE_NODE,
    BDA_CA_POD_NODE,
    BDA_PID_FILTER_NODE,
#if defined(ISDBS_RECEIVER) || defined(DVBT_RECEIVER) || defined(DMBTH_RECEIVER)    
    BDA_TS_SELECTOR_NODE,
#endif
    BDA_SAMPLE_CAPTURE_NODE
}BDA_SAMPLE_NODES;


//
// TemplateFilterCategories:
//
// The list of category GUIDs for the capture filter.
//
const
GUID
TemplateFilterCategories [2] = {
    STATICGUIDOF (KSCATEGORY_BDA_NETWORK_TUNER),
    STATICGUIDOF (KSCATEGORY_BDA_RECEIVER_COMPONENT),
};

//===========================================================================
//
//  BDA Sample Tuner Node (Node Type 0) definitions
//
//  Define structures here for the Properties, Methods, and Events
//  available on the BDA Sample Tuner Node.
//
//  This node is associated with an antenna input pin, therefore, the node
//  properties should be set/retrieved using the antenna input pin.  The
//  BDA Support Library will automatically merge the node's automation
//  table into the automation table for the antenna input pin.
//
//  Properties and methods are dispatched to the Antenna class.
//
//===========================================================================


//
//  BDA Sample Tune Frequency Filter
//
//  Define dispatch routines for specific properties.
//
//  One property is used to get and set the tuner's center frequency. 
//  For this property, define dispatch routines to get and set the frequency.
//
//  Other properties can be used to get and set the tuner's frequency range,
//  as well as to report signal strength.
//
//  These properties must be supported by BDA and 
//  defined elsewhere (for example, in Bdamedia.h).
//
DEFINE_KSPROPERTY_TABLE(SampleTunerNodeFrequencyProperties)
{
    DEFINE_KSPROPERTY_ITEM_BDA_RF_TUNER_FREQUENCY(
        CAntennaPin::GetCenterFrequency,
        CAntennaPin::PutCenterFrequency
        ),
    DEFINE_KSPROPERTY_ITEM_BDA_RF_TUNER_FREQUENCY_MULTIPLIER(
        CAntennaPin::GetMultiplier,
        CAntennaPin::PutMultiplier
        ),

#if DVBS_RECEIVER
    DEFINE_KSPROPERTY_ITEM_BDA_RF_TUNER_POLARITY(
        CAntennaPin::GetTunerParamProperty,
        CAntennaPin::PutTunerParamProperty
        ),
    DEFINE_KSPROPERTY_ITEM_BDA_RF_TUNER_RANGE(
        CAntennaPin::GetTunerParamProperty,
        CAntennaPin::PutTunerParamProperty
        ),
#endif // DVBS_TUNER

#ifdef CHANNEL_BASED_TUNER
    DEFINE_KSPROPERTY_ITEM_BDA_RF_TUNER_TRANSPONDER(
        CAntennaPin::GetTunerParamProperty,
        CAntennaPin::PutTunerParamProperty
        ),
#endif // CHANNEL_BASED_TUNER

#ifdef DVBT_TUNER
    DEFINE_KSPROPERTY_ITEM_BDA_RF_TUNER_BANDWIDTH(
        CAntennaPin::GetTunerParamProperty,
        CAntennaPin::PutTunerParamProperty
        ),
#endif // DVBT_TUNER

#ifdef DMBTH_RECEIVER    
    DEFINE_KSPROPERTY_ITEM_BDA_RF_TUNER_BANDWIDTH(
        CAntennaPin::GetTunerParamProperty,
        CAntennaPin::PutTunerParamProperty
        ),
#endif // DMBTH_TUNER

#ifdef ISDBT_TUNER
    DEFINE_KSPROPERTY_ITEM_BDA_RF_TUNER_BANDWIDTH(
        CAntennaPin::GetTunerParamProperty,
        CAntennaPin::PutTunerParamProperty
        ),
#endif // ISDBT_TUNER
};

//
//  BDA Signal Statistics Properties
//
//  Defines the dispatch routines for the Signal Statistics Properties
//  on the RF Tuner, Demodulator, and PID Filter Nodes
//
DEFINE_KSPROPERTY_TABLE(SampleRFSignalStatsProperties)
{
//#ifdef OPTIONAL_SIGNAL_STATISTICS
    DEFINE_KSPROPERTY_ITEM_BDA_SIGNAL_STRENGTH(
        CAntennaPin::GetSignalStatus,
        NULL
        ),
    DEFINE_KSPROPERTY_ITEM_BDA_SIGNAL_QUALITY(
        CAntennaPin::GetSignalStatus,
        NULL
        ),
//#endif // OPTIONAL_SIGNAL_STATISTICS
    
    DEFINE_KSPROPERTY_ITEM_BDA_SIGNAL_PRESENT(
        CAntennaPin::GetSignalStatus,
        NULL
        ),

    DEFINE_KSPROPERTY_ITEM_BDA_SIGNAL_LOCKED(
        CAntennaPin::GetSignalStatus,
        NULL
        ),

    DEFINE_KSPROPERTY_ITEM_BDA_SAMPLE_TIME(
        CAntennaPin::GetSignalStatus,
        NULL
        ),
};

#if DVBS_RECEIVER
DEFINE_KSPROPERTY_TABLE(RF_DiseqCommand)
{
    DEFINE_KSPROPERTY_ITEM_BDA_DISEQC_ENABLE(
        CAntennaPin::GetDiseqCProperty,
        CAntennaPin::PutDiseqCProperty
        ),
    DEFINE_KSPROPERTY_ITEM_BDA_DISEQC_LNB_SOURCE(
        CAntennaPin::GetDiseqCProperty,
        CAntennaPin::PutDiseqCProperty
        ),
    DEFINE_KSPROPERTY_ITEM_BDA_DISEQC_USETONEBURST(
        CAntennaPin::GetDiseqCProperty,
        CAntennaPin::PutDiseqCProperty
        ),
    DEFINE_KSPROPERTY_ITEM_BDA_DISEQC_REPEATS(
        CAntennaPin::GetDiseqCProperty,
        CAntennaPin::PutDiseqCProperty
        ),
    DEFINE_KSPROPERTY_ITEM_BDA_DISEQC_SEND(
        NULL,
        CAntennaPin::PutDiseqCProperty
        ),
    DEFINE_KSPROPERTY_ITEM_BDA_DISEQC_RESPONSE(
        CAntennaPin::GetDiseqCProperty
        )
};
#endif // DVBS_RECEIVER


//
//  Define the Property Sets for the sample tuner node from the 
//  previously defined node properties and from property sets
//  that BDA supports.
//  These supported property sets must be defined elsewhere 
//  (for example, in Bdamedia.h).
//
//  Associate the sample tuner node with the antenna input pin. 
//
DEFINE_KSPROPERTY_SET_TABLE(SampleTunerNodePropertySets)
{
    DEFINE_KSPROPERTY_SET
    (
        &KSPROPSETID_BdaFrequencyFilter,    // Property Set defined elsewhere
        SIZEOF_ARRAY(SampleTunerNodeFrequencyProperties),  // Number of properties in the array
        SampleTunerNodeFrequencyProperties,  // Property set array
        0,      // FastIoCount
        NULL    // FastIoTable
    ),
    DEFINE_KSPROPERTY_SET
    (
        &KSPROPSETID_BdaSignalStats,    // Property Set defined elsewhere
        SIZEOF_ARRAY(SampleRFSignalStatsProperties),  // Number of properties in the array
        SampleRFSignalStatsProperties,  // Property set array
        0,      // FastIoCount
        NULL    // FastIoTable
    ),
#if DVBS_RECEIVER
    DEFINE_KSPROPERTY_SET
    (
        &KSPROPSETID_BdaDiseqCommand,    // Property Set defined elsewhere
        SIZEOF_ARRAY(RF_DiseqCommand),  // Number of properties in the array
        RF_DiseqCommand,  // Property set array
        0,      // FastIoCount
        NULL    // FastIoTable
    )
#endif
};

// Define the Method Sets and Method handlers for the MUX Node
DEFINE_KSMETHOD_TABLE(SampleMUXNodeServiceMethods)
{
    DEFINE_KSMETHOD_ITEM_BDA_MUX_SETPIDLIST(
        CFilter::PBDA_MUX_SetPIDList,
        NULL
        ),
    DEFINE_KSMETHOD_ITEM_BDA_MUX_GETPIDLIST(
        CFilter::PBDA_MUX_GetPIDList,
        NULL
        )
};

DEFINE_KSMETHOD_SET_TABLE(SampleBDA_MUXNodeMethodSets)
{
    DEFINE_KSMETHOD_SET
        (
        &KSMETHODSETID_BdaMux,    // Method Set defined elsewhere
        SIZEOF_ARRAY(SampleMUXNodeServiceMethods),  // Number of methods in the array
        SampleMUXNodeServiceMethods,  // Method set array
        0,      // FastIoCount
        NULL    // FastIoTable
        )
};



DEFINE_KSAUTOMATION_TABLE(SampleMUXNodeAutomation) {
    DEFINE_KSAUTOMATION_PROPERTIES_NULL,
    DEFINE_KSAUTOMATION_METHODS(SampleBDA_MUXNodeMethodSets),
    DEFINE_KSAUTOMATION_EVENTS_NULL
};

//  Define the Property Sets and Property handlers for the sample CA node
DEFINE_KSPROPERTY_TABLE(RFTableSectionNodeProperties)
{
    DEFINE_KSPROPERTY_ITEM_BDA_TABLE_SECTION(
        CCapturePin::PropGetTableSection,
        CCapturePin::PropSetTableSection
        )    
};

DEFINE_KSPROPERTY_TABLE(RF_BdaCAProperties)
{
    DEFINE_KSPROPERTY_ITEM_BDA_ECM_MAP_STATUS(
        CCapturePin::PropGetECM_MapStatus,
        CCapturePin::PropSetECM_MapStatus
        )
};

DEFINE_KSPROPERTY_SET_TABLE(CommonCANodePropertySets)
{
    DEFINE_KSPROPERTY_SET
        (
        &KSPROPSETID_BdaTableSection,             // Set
        SIZEOF_ARRAY(RFTableSectionNodeProperties),// PropertiesCount
        RFTableSectionNodeProperties,              // PropertyItems
        0,                                          // FastIoCount
        NULL                                        // FastIoTable
        ),
        DEFINE_KSPROPERTY_SET
        (
        &KSPROPSETID_BdaCA,             // Set
        SIZEOF_ARRAY(RF_BdaCAProperties),// PropertiesCount
        RF_BdaCAProperties,              // PropertyItems
        0,                                          // FastIoCount
        NULL                                        // FastIoTable
        )

};

DEFINE_KSAUTOMATION_TABLE(CommonCANodeAutomation) {
    DEFINE_KSAUTOMATION_PROPERTIES(CommonCANodePropertySets),
        DEFINE_KSAUTOMATION_METHODS_NULL,
        DEFINE_KSAUTOMATION_EVENTS_NULL
};

//
//  Define the Automation Table for the BDA sample PID Filter node.
//
DEFINE_KSPROPERTY_TABLE(BdaPIDFilterProperties)
{
    DEFINE_KSPROPERTY_ITEM_BDA_PIDFILTER_MAP_PIDS(
        CCapturePin::PropGetMapPIDS,
        CCapturePin::PropSetMapPIDS
        ),
    DEFINE_KSPROPERTY_ITEM_BDA_PIDFILTER_UNMAP_PIDS(
        CCapturePin::PropGetUnmapPIDS,
        CCapturePin::PropSetUnmapPIDS
        )
};

DEFINE_KSPROPERTY_SET_TABLE(SamplePIDFilterNodePropertySets)
{
    DEFINE_KSPROPERTY_SET
    (
        &KSPROPSETID_BdaPIDFilter,             // Set
        SIZEOF_ARRAY(BdaPIDFilterProperties),// PropertiesCount
        BdaPIDFilterProperties,              // PropertyItems
        0,                                          // FastIoCount
        NULL                                        // FastIoTable
    )

};

DEFINE_KSAUTOMATION_TABLE(SamplePIDFilterNodeAutomation) {
    DEFINE_KSAUTOMATION_PROPERTIES(SamplePIDFilterNodePropertySets),
    DEFINE_KSAUTOMATION_METHODS_NULL,
    DEFINE_KSAUTOMATION_EVENTS_NULL
};


//
//  Define the Automation Table for the BDA sample tuner node.
//
DEFINE_KSAUTOMATION_TABLE(SampleTunerNodeAutomation) {
    DEFINE_KSAUTOMATION_PROPERTIES(SampleTunerNodePropertySets),
    DEFINE_KSAUTOMATION_METHODS_NULL,
    DEFINE_KSAUTOMATION_EVENTS_NULL
};

//
//  Define the Automation Table for the BDA DRM node (ISDBT Receiver only)
//
#ifdef USE_DRM_SERVICE
         
//=========================
// DRM Service Methods
//=========================
DEFINE_KSMETHOD_TABLE(SampleDRMNodeServiceMethods)
{
    DEFINE_KSMETHOD_BDA_DRM_SETDRM(
        CFilter::PBDA_DRM_SetDRM,
        NULL
        ),
    DEFINE_KSMETHOD_BDA_DRM_DRMSTATUS(
        CFilter::PBDA_GetDRMStatus,
        NULL
        )
};

//=========================
//WMDRM Session Methods
//=========================
DEFINE_KSMETHOD_TABLE(SampleWMDRMSessionMethods)
{
    DEFINE_KSMETHOD_BDA_WMDRM_STATUS(
        CFilter::PBDA_GetWMDRMStatus,
        NULL
        ),
    DEFINE_KSMETHOD_BDA_WMDRM_REVINFO(
        CFilter::PBDA_WMDRMRevInfo,
        NULL
        ),
    DEFINE_KSMETHOD_BDA_WMDRM_CRL(
        CFilter::PBDA_WMDRMCrl,
        NULL
        ),
    DEFINE_KSMETHOD_BDA_WMDRM_TRANSACTMESSAGE(
        CFilter::PBDA_WMDRMTransactMessage,
        NULL
        ),
    DEFINE_KSMETHOD_BDA_WMDRM_REISSUELICENSE(
        CFilter::PBDA_WMDRMReissueLicense,
        NULL
        ),
    DEFINE_KSMETHOD_BDA_WMDRM_RENEWLICENSE(
        CFilter::PBDA_WMDRMRenewLicense,
        NULL
        ),
    DEFINE_KSMETHOD_BDA_WMDRM_GETLICENSE(
        CFilter::PBDA_WMDRMGetLicense,
        NULL
        ),
    DEFINE_KSMETHOD_BDA_WMDRM_KEYINFO(
        CFilter::PBDA_GetWMDRMKeyInfo,
        NULL
        )
};

//=========================
//WMDRM Tuner Methods
//=========================
DEFINE_KSMETHOD_TABLE(SampleWMDRMTunerMethods)
{
    DEFINE_KSMETHOD_BDA_WMDRMTUNER_CANCELCAPTURETOKEN(
        CFilter::PBDA_WMDRMTunerCancelCapToken,
        NULL
        ),
    DEFINE_KSMETHOD_BDA_WMDRMTUNER_SETPIDPROTECTION(
        CFilter::PBDA_WMDRMTunerSetPidProtection,
        NULL
        ),
    DEFINE_KSMETHOD_BDA_WMDRMTUNER_GETPIDPROTECTION(
        CFilter::PBDA_WMDRMTunerGetPidProtection,
        NULL
        ),
    DEFINE_KSMETHOD_BDA_WMDRMTUNER_SETSYNCVALUE(
        CFilter::PBDA_WMDRMTunerSetSyncValue,
        NULL
        ),
    DEFINE_KSMETHOD_BDA_WMDRMTUNER_PURCHASE_ENTITLEMENT(
        CFilter::PBDA_WMDRMTunerPurchaseEntitlement,
        NULL
        ),
    DEFINE_KSMETHOD_BDA_WMDRMTUNER_STARTCODEPROFILE(
        CFilter::PBDA_GetWMDRMTunerStartCodeProfile,
        NULL
        )
};


//=========================
//DRM Method Sets Array
//=========================
DEFINE_KSMETHOD_SET_TABLE(SampleBDA_DRMNodeMethodSets)
{
    DEFINE_KSMETHOD_SET
    (
        &KSMETHODSETID_BdaDrmService,    // Method Set defined elsewhere
        SIZEOF_ARRAY(SampleDRMNodeServiceMethods),  // Number of methods in the array
        SampleDRMNodeServiceMethods,  // Method set array
        0,      // FastIoCount
        NULL    // FastIoTable
    ),
    DEFINE_KSMETHOD_SET
    (
        &KSMETHODSETID_BdaWmdrmSession,    // Method Set defined elsewhere
        SIZEOF_ARRAY(SampleWMDRMSessionMethods),  // Number of methods in the array
        SampleWMDRMSessionMethods,  // Method set array
        0,      // FastIoCount
        NULL    // FastIoTable
    ),
    DEFINE_KSMETHOD_SET
    (
        &KSMETHODSETID_BdaWmdrmTuner,    // Method Set defined elsewhere
        SIZEOF_ARRAY(SampleWMDRMTunerMethods),  // Number of methods in the array
        SampleWMDRMTunerMethods,  // Method set array
        0,      // FastIoCount
        NULL    // FastIoTable
    )
};

DEFINE_KSAUTOMATION_TABLE(SampleBDA_DRMNodeAutomation) {
        DEFINE_KSAUTOMATION_PROPERTIES_NULL,
        DEFINE_KSAUTOMATION_METHODS(SampleBDA_DRMNodeMethodSets),
        DEFINE_KSAUTOMATION_EVENTS_NULL
};

//=========================
// ISDB CAS Service Methods
//=========================
DEFINE_KSMETHOD_TABLE(SampleISDBCASNodeServiceMethods)
{
    DEFINE_KSMETHOD_ITEM_BDA_ISDBCAS_SETREQUEST(
        CFilter::PBDA_ISDBCAS_SetRequest,
        NULL
        ),
    DEFINE_KSMETHOD_ITEM_BDA_ISDBCAS_RESPONSEDATA(
        CFilter::PBDA_ISDBCAS_GetResponseData,
        NULL
        )
};

//===========================
//ISDB CAS Method Sets Array
//===========================
DEFINE_KSMETHOD_SET_TABLE(SampleBDA_ISDBCASNodeMethodSets)
{
    DEFINE_KSMETHOD_SET
        (
        &KSMETHODSETID_BdaIsdbConditionalAccess,    // Method Set defined elsewhere
        SIZEOF_ARRAY(SampleISDBCASNodeServiceMethods),  // Number of methods in the array
        SampleISDBCASNodeServiceMethods,  // Method set array
        0,      // FastIoCount
        NULL    // FastIoTable
        )
};

DEFINE_KSAUTOMATION_TABLE(SampleBDA_ISDBCASNodeAutomation) {
    DEFINE_KSAUTOMATION_PROPERTIES_NULL,
    DEFINE_KSAUTOMATION_METHODS(SampleBDA_ISDBCASNodeMethodSets),
    DEFINE_KSAUTOMATION_EVENTS_NULL
};


#endif  //USE_DRM_SERVICE

#ifndef DISABLE_CORE_CAS
//=========================
// CORE CAS Service Methods
//=========================
DEFINE_KSMETHOD_TABLE(SampleCoreCASNodeServiceMethods)
{
    DEFINE_KSMETHOD_ITEM_BDA_CAS_CHECKENTITLEMENTTOKEN(
        CFilter::PBDA_CoreCAS_CheckEntitlementToken,
        NULL
        ),
    DEFINE_KSMETHOD_ITEM_BDA_CAS_SETCAPTURETOKEN(
        CFilter::PBDA_CoreCAS_SetCaptureToken,
        NULL
        ),
    DEFINE_KSMETHOD_ITEM_BDA_CAS_OPENBROADCASTMMI(
        CFilter::PBDA_CoreCAS_OpenBroadcastMMI,
        NULL
        ),
    DEFINE_KSMETHOD_ITEM_BDA_CAS_CLOSEMMIDIALOG(
        CFilter::PBDA_CoreCAS_CloseMMIDialog,
        NULL
        )
};

//===========================
//CORE CAS Method Sets Array
//===========================
DEFINE_KSMETHOD_SET_TABLE(SampleBDA_CoreCASNodeMethodSets)
{
    DEFINE_KSMETHOD_SET
        (
        &KSMETHODSETID_BdaConditionalAccessService,    // Method Set defined elsewhere
        SIZEOF_ARRAY(SampleCoreCASNodeServiceMethods),  // Number of methods in the array
        SampleCoreCASNodeServiceMethods,  // Method set array
        0,      // FastIoCount
        NULL    // FastIoTable
        )
};

DEFINE_KSAUTOMATION_TABLE(SampleBDA_CoreCASNodeAutomation) {
    DEFINE_KSAUTOMATION_PROPERTIES_NULL,
    DEFINE_KSAUTOMATION_METHODS(SampleBDA_CoreCASNodeMethodSets),
    DEFINE_KSAUTOMATION_EVENTS_NULL
};

#if defined(ISDBS_RECEIVER) || defined(DVBT_RECEIVER) || defined(DMBTH_RECEIVER)    
//=========================
// TS Selector Service Methods
//=========================
DEFINE_KSMETHOD_TABLE(SampleTsSelectorNodeServiceMethods)
{
    DEFINE_KSMETHOD_ITEM_BDA_TS_SELECTOR_SETTSID(
        CFilter::BDA_TsSelectorSetTSID,
        NULL
        ),

    DEFINE_KSMETHOD_ITEM_BDA_TS_SELECTOR_GETTSINFORMATION(
        CFilter::BDA_TsSelectorGetTSInformation,
        NULL
        ),
};

//===========================
// TS Selector Method Sets Array
//===========================
DEFINE_KSMETHOD_SET_TABLE(SampleBDA_TsSelectorNodeMethodSets)
{
    DEFINE_KSMETHOD_SET
        (
        &KSMETHODSETID_BdaTSSelector,    // Method Set defined elsewhere
        SIZEOF_ARRAY(SampleTsSelectorNodeServiceMethods),  // Number of methods in the array
        SampleTsSelectorNodeServiceMethods,  // Method set array
        0,      // FastIoCount
        NULL    // FastIoTable
        )
};

DEFINE_KSAUTOMATION_TABLE(SampleBdaTsSelectorAutomation) {
    DEFINE_KSAUTOMATION_PROPERTIES_NULL,
    DEFINE_KSAUTOMATION_METHODS(SampleBDA_TsSelectorNodeMethodSets),
    DEFINE_KSAUTOMATION_EVENTS_NULL
};
#endif // ISDBS_RECEIVER

#endif


//===========================================================================
//
//  Sample Demodulator Node definitions
//
//  This structure defines the Properties, Methods, and Events
//  available on the BDA Demodulator Node.
//
//  This node is associated with a transport output pin and thus the node
//  properties should be set/put using the transport output pin.
//
//===========================================================================


//
//  BDA Signal Statistics Properties for Demodulator Node
//
//  Defines the dispatch routines for the Signal Statistics Properties
//  on the Demodulator Node.
//
DEFINE_KSPROPERTY_TABLE(SampleDemodSignalStatsProperties)
{
//#ifdef OPTIONAL_SIGNAL_STATISTICS
    DEFINE_KSPROPERTY_ITEM_BDA_SIGNAL_QUALITY(
        CCapturePin::GetSignalStatus,
        NULL
        ),
//#endif // OPTIONAL_SIGNAL_STATISTICS

    DEFINE_KSPROPERTY_ITEM_BDA_SIGNAL_LOCKED(
        CCapturePin::GetSignalStatus,
        NULL
        ),

    DEFINE_KSPROPERTY_ITEM_BDA_SIGNAL_PRESENT(
        CCapturePin::GetSignalStatus,
        NULL
        ),

    DEFINE_KSPROPERTY_ITEM_BDA_SIGNAL_STRENGTH(
        CCapturePin::GetSignalStatus,
        NULL
        ),

    DEFINE_KSPROPERTY_ITEM_BDA_SAMPLE_TIME(
        CCapturePin::GetSignalStatus,
        NULL
        ),
};


DEFINE_KSPROPERTY_TABLE(SampleAutoDemodProperties)
{
    DEFINE_KSPROPERTY_ITEM_BDA_AUTODEMODULATE_START(
        NULL,
        CCapturePin::PutAutoDemodProperty
        ),
    DEFINE_KSPROPERTY_ITEM_BDA_AUTODEMODULATE_STOP(
        NULL,
        CCapturePin::PutAutoDemodProperty
        )
};

//
//  BDA Digital Demodulator Property Set for Demodulator Node
//
//  Defines the dispatch routines for the Digital Demodulator Properties
//  on the Demodulator Node.
//
DEFINE_KSPROPERTY_TABLE(SampleDigitalDemodProperties)
{
    DEFINE_KSPROPERTY_ITEM_BDA_MODULATION_TYPE(
        CCapturePin::GetDigitalDemodProperty,
        CCapturePin::PutDigitalDemodProperty
        ),
    DEFINE_KSPROPERTY_ITEM_BDA_INNER_FEC_TYPE(
        CCapturePin::GetDigitalDemodProperty,
        CCapturePin::PutDigitalDemodProperty
        ),
    DEFINE_KSPROPERTY_ITEM_BDA_INNER_FEC_RATE(
        CCapturePin::GetDigitalDemodProperty,
        CCapturePin::PutDigitalDemodProperty
        ),
    DEFINE_KSPROPERTY_ITEM_BDA_OUTER_FEC_TYPE(
        CCapturePin::GetDigitalDemodProperty,
        CCapturePin::PutDigitalDemodProperty
        ),
    DEFINE_KSPROPERTY_ITEM_BDA_OUTER_FEC_RATE(
        CCapturePin::GetDigitalDemodProperty,
        CCapturePin::PutDigitalDemodProperty
        ),
    DEFINE_KSPROPERTY_ITEM_BDA_SYMBOL_RATE(
        CCapturePin::GetDigitalDemodProperty,
        CCapturePin::PutDigitalDemodProperty
        ),
    DEFINE_KSPROPERTY_ITEM_BDA_SPECTRAL_INVERSION(
        CCapturePin::GetDigitalDemodProperty,
        CCapturePin::PutDigitalDemodProperty
        ),

#if DVBS_RECEIVER
    DEFINE_KSPROPERTY_ITEM_BDA_ROLL_OFF( //DVB-S2 property
        CCapturePin::GetDigitalDemodProperty,
        CCapturePin::PutDigitalDemodProperty
        ),
    DEFINE_KSPROPERTY_ITEM_BDA_PILOT( //DVB-S2 property
        CCapturePin::GetDigitalDemodProperty,
        CCapturePin::PutDigitalDemodProperty
        ),
#endif // DVBS_RECEIVER

#if DVBT_RECEIVER
    DEFINE_KSPROPERTY_ITEM_BDA_GUARD_INTERVAL(
        CCapturePin::GetDigitalDemodProperty,
        CCapturePin::PutDigitalDemodProperty
        ),
    DEFINE_KSPROPERTY_ITEM_BDA_TRANSMISSION_MODE(
        CCapturePin::GetDigitalDemodProperty,
        CCapturePin::PutDigitalDemodProperty
        ),
        //DVB-T2 defines the transport selection based on the PLP number (if multiple PLPs in use)
    DEFINE_KSPROPERTY_ITEM_BDA_PLP_NUMBER(
        CCapturePin::GetDigitalDemodProperty,
        CCapturePin::PutDigitalDemodProperty
        )
#endif // DVBT_RECEIVER

#ifdef DMBTH_RECEIVER    
    DEFINE_KSPROPERTY_ITEM_BDA_GUARD_INTERVAL(
        CCapturePin::GetDigitalDemodProperty,
        CCapturePin::PutDigitalDemodProperty
        ),
    DEFINE_KSPROPERTY_ITEM_BDA_TRANSMISSION_MODE(
        CCapturePin::GetDigitalDemodProperty,
        CCapturePin::PutDigitalDemodProperty
        ),
        //DVB-T2 defines the transport selection based on the PLP number (if multiple PLPs in use)
    DEFINE_KSPROPERTY_ITEM_BDA_PLP_NUMBER(
        CCapturePin::GetDigitalDemodProperty,
        CCapturePin::PutDigitalDemodProperty
        )
#endif // DMBTH_RECEIVER
};

//#endif // !ATSC_RECEIVER

//
//  Sample Demodulator Node Extension Properties
//
//  Define dispatch routines for a set of driver specific
//  demodulator node properties.  This is how venders add support
//  for properties that are specific to their hardware.  They can
//  access these properties through a KSProxy plugin.
//
DEFINE_KSPROPERTY_TABLE(BdaSampleDemodExtensionProperties)
{
    DEFINE_KSPROPERTY_ITEM_BDA_SAMPLE_DEMOD_EXTENSION_PROPERTY1(  // A read and write property
        CCapturePin::GetExtensionProperties, // or NULL if not method to get the property
        CCapturePin::PutExtensionProperties // or NULL if not method to put the property
        ),
    DEFINE_KSPROPERTY_ITEM_BDA_SAMPLE_DEMOD_EXTENSION_PROPERTY2(  //A read only property
        CCapturePin::GetExtensionProperties, // or NULL if not method to get the property
        NULL // or NULL if not method to put the property
        ),
    DEFINE_KSPROPERTY_ITEM_BDA_SAMPLE_DEMOD_EXTENSION_PROPERTY3(  //A write only property
        NULL, // or NULL if not method to get the property
        CCapturePin::PutExtensionProperties // or NULL if not method to put the property
        ),
};


//
//  Demodulator Node Property Sets supported
//
//  This table defines all property sets supported by the
//  Demodulator Node associated with the transport output pin.
//
DEFINE_KSPROPERTY_SET_TABLE(SampleDemodNodePropertySets)
{
    DEFINE_KSPROPERTY_SET
    (
        &KSPROPSETID_BdaAutodemodulate,                // Set
        SIZEOF_ARRAY(SampleAutoDemodProperties),   // PropertiesCount
        SampleAutoDemodProperties,                 // PropertyItems
        0,                                          // FastIoCount
        NULL                                        // FastIoTable
    ),


    DEFINE_KSPROPERTY_SET
    (
        &KSPROPSETID_BdaDigitalDemodulator,                // Set
        SIZEOF_ARRAY(SampleDigitalDemodProperties),   // PropertiesCount
        SampleDigitalDemodProperties,                 // PropertyItems
        0,                                          // FastIoCount
        NULL                                        // FastIoTable
    ),


    DEFINE_KSPROPERTY_SET
    (
        &KSPROPSETID_BdaSignalStats,                // Set
        SIZEOF_ARRAY(SampleDemodSignalStatsProperties),   // PropertiesCount
        SampleDemodSignalStatsProperties,                 // PropertyItems
        0,                                          // FastIoCount
        NULL                                        // FastIoTable
    ),
    DEFINE_KSPROPERTY_SET
    (
        &KSPROPSETID_BdaSampleDemodExtensionProperties, // Set
        SIZEOF_ARRAY(BdaSampleDemodExtensionProperties), // Number of properties in the array
        BdaSampleDemodExtensionProperties, // Property set array
        0,                                                                              // FastIoCount
        NULL                                                                      // FastIoTable
    ),
    //
    //  Additional property sets for the node can be added here.
    //
};


//
//  Demodulator Node Automation Table
//
//  This structure defines the Properties, Methods, and Events
//  available on the BDA Demodulator Node.
//  These are used to set the symbol rate, and Viterbi rate,
//  as well as to report signal lock and signal quality.
//
DEFINE_KSAUTOMATION_TABLE(SampleDemodulatorNodeAutomation) {
    DEFINE_KSAUTOMATION_PROPERTIES( SampleDemodNodePropertySets),
    DEFINE_KSAUTOMATION_METHODS_NULL,
    DEFINE_KSAUTOMATION_EVENTS_NULL
};

#ifndef DISABLE_PBDA_TUNER_NODE
//the PBDA Tuner can expose an own demodulator node and properties, 
//however tuning and scanning is accessed at the filter level to the 
//scanning and tuning service. Failing calls to tuning 
//will use the properties defined in the tuner and demodulator node 
DEFINE_KSAUTOMATION_TABLE(SampleBDA_PBDATUNERNodeAutomation) {
    DEFINE_KSAUTOMATION_PROPERTIES( SampleDemodNodePropertySets),
    DEFINE_KSAUTOMATION_METHODS_NULL,
    DEFINE_KSAUTOMATION_EVENTS_NULL
};

#endif


//===========================================================================
//
//  Antenna Pin Definitions
//
//===========================================================================

//
//  The BDA Support Library automatically merges the RF tuner node properties,
//  methods, and events onto the antenna pin's automation table. It also
//  merges properties, methods, and events the that are require and
//  supplied by the BDA Support Library.
//

//  
//  The hardware vendor may want to provide driver specific properties,
//  methods, or events on the antenna pin or override those provided by
//  the BDA Support Library.  Such roperties, methods, and events will
//  be defined here.
//

//
//  Define the Automation Table for the antenna pin.
//
//
DEFINE_KSAUTOMATION_TABLE(AntennaAutomation) {
    DEFINE_KSAUTOMATION_PROPERTIES_NULL,
    DEFINE_KSAUTOMATION_METHODS_NULL,
    DEFINE_KSAUTOMATION_EVENTS_NULL
};


//
//  Dispatch Table for the antenna pin.
//
const
KSPIN_DISPATCH
AntennaPinDispatch =
{
    CAntennaPin::PinCreate,  // Create
    CAntennaPin::PinClose,  // Close
    NULL,  // Process siganl data
    NULL,  // Reset
    NULL,  // SetDataFormat
    CAntennaPin::PinSetDeviceState,  // SetDeviceState
    NULL,  // Connect
    NULL,  // Disconnect
    NULL,  // Clock
    NULL   // Allocator
};


//
//  Format the input antenna stream connection.
//
//  Used to connect the input antenna pin to a specific upstream filter,
//  such as the network provider.
//
const KS_DATARANGE_BDA_ANTENNA AntennaPinRange =
{
   // insert the KSDATARANGE and KSDATAFORMAT here
    {
        sizeof( KS_DATARANGE_BDA_ANTENNA), // FormatSize
        0,                                 // Flags - (N/A)
        0,                                 // SampleSize - (N/A)
        0,                                 // Reserved
        { STATIC_KSDATAFORMAT_TYPE_BDA_ANTENNA },  // MajorFormat
        { STATIC_KSDATAFORMAT_SUBTYPE_NONE },  // SubFormat
        { STATIC_KSDATAFORMAT_SPECIFIER_NONE } // Specifier
    }
};

//  Format Ranges of Antenna Input Pin.
//
static PKSDATAFORMAT AntennaPinRanges[] =
{
    (PKSDATAFORMAT) &AntennaPinRange,

    // Add more formats here if additional antenna signal formats are supported.
    //
};


//===========================================================================
//
//  Transport Output Pin Definitions
//
//===========================================================================

//
//  The BDA Support Library automatically merges the RF tuner node properties,
//  methods, and events onto the antenna pin's automation table. It also
//  merges properties, methods, and events the that are require and
//  supplied by the BDA Support Library.
//

//  
//  The hardware vendor may want to provide driver specific properties,
//  methods, or events on the antenna pin or override those provided by
//  the BDA Support Library.  Such roperties, methods, and events will
//  be defined here.
//

//
//  Define the Automation Table for the transport pin.
//
//
DEFINE_KSAUTOMATION_TABLE(TransportAutomation) {
    DEFINE_KSAUTOMATION_PROPERTIES_NULL,
    DEFINE_KSAUTOMATION_METHODS_NULL,
    DEFINE_KSAUTOMATION_EVENTS_NULL
};

//
//  Dispatch Table for the transport Output pin.
//
//  Since data on the transport is actually delivered to the
//  PCI bridge in hardware, this pin does not process data.
//
//  Connection of, and state transitions on, this pin help the
//  driver to determine when to allocate hardware resources for
//  each node.
//
const
KSPIN_DISPATCH
TransportPinDispatch =
{
    CCapturePin::PinCreate,     // Create
    CCapturePin::PinClose,      // Close
    NULL,                               // Process
    NULL,                               // Reset
    NULL,                               // SetDataFormat
    NULL,                               //SetDeviceState
    NULL,                               // Connect
    NULL,                               // Disconnect
    NULL,                               // Clock
    NULL                                // Allocator
};


//
//  Format the output Transport Stream Connection
//
//  Used to connect the output pin to a specific downstream filter
//
const KS_DATARANGE_BDA_TRANSPORT TransportPinRange =
{
   // insert the KSDATARANGE and KSDATAFORMAT here
    {
        sizeof( KS_DATARANGE_BDA_TRANSPORT),               // FormatSize
        0,                                                 // Flags - (N/A)
        0,                                                 // SampleSize - (N/A)
        0,                                                 // Reserved
        { STATIC_KSDATAFORMAT_TYPE_STREAM },               // MajorFormat
        { STATIC_KSDATAFORMAT_TYPE_MPEG2_TRANSPORT },      // SubFormat
        { STATIC_KSDATAFORMAT_SPECIFIER_BDA_TRANSPORT }    // Specifier
    },
    // insert the BDA_TRANSPORT_INFO here
    {
        188,        //  ulcbPhyiscalPacket
        312 * 188,  //  ulcbPhyiscalFrame
        0,          //  ulcbPhyiscalFrameAlignment (no requirement)
        0           //  AvgTimePerFrame (not known)
    }
};

//  Format Ranges of Transport Output Pin.
//
static PKSDATAFORMAT TransportPinRanges[] =
{
    (PKSDATAFORMAT) &TransportPinRange,

    // Add more formats here if additional transport formats are supported.
    //
};


//  Medium GUIDs for the Transport Output Pin.
//
//  This insures contection to the correct Capture Filter pin.
//
// {60FAB82F-D605-4A33-A099-D809A08B7F71}
#define GUID_BdaSWRcv   0x60FAB82F,  0xD605, 0x4A33, 0xA0, 0x99, 0xD8, 0x09, 0xA0, 0x8B, 0x7F, 0x71
const KSPIN_MEDIUM TransportPinMedium =
{
    GUID_BdaSWRcv, 0, 0
};


//===========================================================================
//
//  Filter  definitions
//
//  Define arrays here that contain the types of nodes and pins that are possible for the filter 
//  Define structures here that describe Properties, Methods, and Events available on the filter
//
//  Properties can be used to set and retrieve information for the filter.
//  Methods can be used to perform operations on the filter. 
//
//===========================================================================

//
//  Template Node Descriptors
//
//  Define an array that contains all the node types that are available in the template
//  topology of the filter.
//  These node types must be supported by BDA and 
//  defined elsewhere (for example, in Bdamedia.h).
//
const
KSNODE_DESCRIPTOR
NodeDescriptors[] =
{
    {
        &SampleTunerNodeAutomation, // Point to KSAUTOMATION_TABLE structure for the node's automation table
        &KSNODE_BDA_RF_TUNER, // Point to the guid that defines function of the node
        NULL // Point to the guid that represents the name of the topology node
    },
#if ATSC_RECEIVER
    {
        &SampleDemodulatorNodeAutomation, // Point to KSAUTOMATION_TABLE 
        &KSNODE_BDA_8VSB_DEMODULATOR, // Point to the guid that defines the topology node
        NULL // Point to the guid that represents the name of the topology node
    },
#elif DVBS_RECEIVER
    {
        &SampleDemodulatorNodeAutomation, // Point to KSAUTOMATION_TABLE 
        &KSNODE_BDA_QPSK_DEMODULATOR, // Point to the guid that defines the topology node
        NULL // Point to the guid that represents the name of the topology node
    },
#ifdef USE_DRM_SERVICE
    {
        &SampleBDA_DRMNodeAutomation, // Point to KSAUTOMATION_TABLE structure for the node's automation table
        &KSNODE_BDA_PBDA_DRM, // Point to the guid that defines function of the node
        NULL // Point to the guid that represents the name of the topology node
    },
    {
        &SampleBDA_CoreCASNodeAutomation, // Point to KSAUTOMATION_TABLE structure for the node's automation table
        &KSNODE_BDA_PBDA_CAS, // Point to the guid that defines function of the node
        NULL // Point to the guid that represents the name of the topology node
    },    
#endif
#elif DVBT_RECEIVER
    {
        &SampleDemodulatorNodeAutomation, // Point to KSAUTOMATION_TABLE 
        &KSNODE_BDA_COFDM_DEMODULATOR, // Point to the guid that defines the topology node
        NULL // Point to the guid that represents the name of the topology node
    },

#elif QAM_RECEIVER
    {
        &SampleDemodulatorNodeAutomation, // Point to KSAUTOMATION_TABLE 
        &KSNODE_BDA_QAM_DEMODULATOR, // Point to the guid that defines the topology node
        NULL // Point to the guid that represents the name of the topology node
    },
#elif DVBC_RECEIVER
    {
        &SampleDemodulatorNodeAutomation, // Point to KSAUTOMATION_TABLE 
        &KSNODE_BDA_QAM_DEMODULATOR, // Point to the guid that defines the topology node
        NULL // Point to the guid that represents the name of the topology node
    },
#elif defined(ISDBT_RECEIVER) || defined(ISDBS_RECEIVER)
#ifdef ISDBT_RECEIVER
    {
        &SampleDemodulatorNodeAutomation, // Point to KSAUTOMATION_TABLE 
        &KSNODE_BDA_ISDB_T_DEMODULATOR, // Point to the guid that defines the topology node
        NULL // Point to the guid that represents the name of the topology node
    },
#elif defined(ISDBS_RECEIVER)
    {
        &SampleDemodulatorNodeAutomation, // Point to KSAUTOMATION_TABLE 
        &KSNODE_BDA_ISDB_S_DEMODULATOR, // Point to the guid that defines the topology node
        NULL // Point to the guid that represents the name of the topology node
    },
#endif
#ifdef USE_DRM_SERVICE
    {
        &SampleBDA_DRMNodeAutomation, // Point to KSAUTOMATION_TABLE structure for the node's automation table
        &KSNODE_BDA_PBDA_DRM, // Point to the guid that defines function of the node
        NULL // Point to the guid that represents the name of the topology node
    },
    {
        &SampleBDA_CoreCASNodeAutomation, // Point to KSAUTOMATION_TABLE structure for the node's automation table
        &KSNODE_BDA_PBDA_CAS, // Point to the guid that defines function of the node
        NULL // Point to the guid that represents the name of the topology node
    },    
    {
        &SampleBDA_ISDBCASNodeAutomation, // Point to KSAUTOMATION_TABLE structure for the node's automation table
        &KSNODE_BDA_PBDA_ISDBCAS, // Point to the guid that defines function of the node
        NULL // Point to the guid that represents the name of the topology node
    },    
#endif
#elif NETOP_RECEIVER
    {
        &SampleBDA_PBDATUNERNodeAutomation, // Point to KSAUTOMATION_TABLE 
        &KSNODE_BDA_PBDA_TUNER, // Point to the guid that defines the topology node
        NULL // Point to the guid that represents the name of the topology node
    },
#ifdef USE_DRM_SERVICE
    {
        &SampleBDA_DRMNodeAutomation, // Point to KSAUTOMATION_TABLE structure for the node's automation table
        &KSNODE_BDA_PBDA_DRM, // Point to the guid that defines function of the node
        NULL // Point to the guid that represents the name of the topology node
    },
    {
        &SampleBDA_CoreCASNodeAutomation, // Point to KSAUTOMATION_TABLE structure for the node's automation table
        &KSNODE_BDA_PBDA_CAS, // Point to the guid that defines function of the node
        NULL // Point to the guid that represents the name of the topology node
    },    
#endif
#elif PBDAT_RECEIVER
    {
        &SampleDemodulatorNodeAutomation, // Point to KSAUTOMATION_TABLE 
        &KSNODE_BDA_COFDM_DEMODULATOR, // Point to the guid that defines the topology node
        NULL // Point to the guid that represents the name of the topology node
    },
#ifdef USE_DRM_SERVICE
    {
        &SampleBDA_DRMNodeAutomation, // Point to KSAUTOMATION_TABLE structure for the node's automation table
        &KSNODE_BDA_PBDA_DRM, // Point to the guid that defines function of the node
        NULL // Point to the guid that represents the name of the topology node
    },
    {
        &SampleBDA_CoreCASNodeAutomation, // Point to KSAUTOMATION_TABLE structure for the node's automation table
        &KSNODE_BDA_PBDA_CAS, // Point to the guid that defines function of the node
        NULL // Point to the guid that represents the name of the topology node
    },
#endif
#elif defined(DMBTH_RECEIVER)    
    {
        &SampleDemodulatorNodeAutomation, // Point to KSAUTOMATION_TABLE 
        &KSNODE_BDA_COFDM_DEMODULATOR, // Point to the guid that defines the topology node
        NULL // Point to the guid that represents the name of the topology node
    },
    {
        &SampleBdaTsSelectorAutomation, // Point to KSAUTOMATION_TABLE 
        &KSNODE_BDA_TS_SELECTOR, // Point to the guid that defines the topology node
        NULL // Point to the guid that represents the name of the topology node
    },
#ifdef USE_DRM_SERVICE
    {
        &SampleBDA_DRMNodeAutomation, // Point to KSAUTOMATION_TABLE structure for the node's automation table
        &KSNODE_BDA_PBDA_DRM, // Point to the guid that defines function of the node
        NULL // Point to the guid that represents the name of the topology node
    },
    {
        &SampleBDA_CoreCASNodeAutomation, // Point to KSAUTOMATION_TABLE structure for the node's automation table
        &KSNODE_BDA_PBDA_CAS, // Point to the guid that defines function of the node
        NULL // Point to the guid that represents the name of the topology node
    },    
#endif
#endif
    {
        &SampleMUXNodeAutomation,  // Point to KSAUTOMATION_TABLE
        &KSNODE_BDA_PBDA_MUX, // Point to the guid that defines the topology node
        NULL // Point to the guid that represents the name of the topology node
    },
    {
        &CommonCANodeAutomation,  // Point to KSAUTOMATION_TABLE
#if QAM_RECEIVER
        &KSNODE_BDA_OPENCABLE_POD,
#else
        &KSNODE_BDA_COMMON_CA_POD, // Point to the guid that defines the topology node
#endif
        NULL // Point to the guid that represents the name of the topology node
    },
    {
        &SamplePIDFilterNodeAutomation,  // Point to KSAUTOMATION_TABLE
        &KSNODE_BDA_PID_FILTER, // Point to the guid that defines the topology node
        NULL // Point to the guid that represents the name of the topology node
    },
#if defined(ISDBS_RECEIVER) || defined(DVBT_RECEIVER)
    {
        &SampleBdaTsSelectorAutomation, // Point to KSAUTOMATION_TABLE 
        &KSNODE_BDA_TS_SELECTOR, // Point to the guid that defines the topology node
        NULL // Point to the guid that represents the name of the topology node
    },
#endif
};


//
//  Initial Pin Descriptors
//
//  This data structure defines the pins that will appear on the filer
//  when it is first created.
//
//  All BDA filters should expose at lease one input pin to insure that
//  the filter can be properly inserted in a BDA receiver graph.  The
//  initial pins can be created in a number of ways.
//
//  The initial filters descriptor passed in to BdaInitFilter can include
//  a list of n pin descriptors that correspond to the first n of m pin
//  descriptors in the template filter descriptor.  This list of pin
//  descriptors will usually only include those input pins that are
//  ALWAYS exposed by the filter in question.
//
//  Alternatively, the driver may call BdaCreatePin from its filter Create
//  dispatch function for each pin that it ALWAYS wants exposed.
//
//  This filter uses an initial filter descriptor to force the Antenna
//  input pin to always be exposed.
//
const
KSPIN_DESCRIPTOR_EX
InitialPinDescriptors[] =
{
    //  Antenna Input Pin
    //
    {
        &AntennaPinDispatch,   // Point to the dispatch table for the input pin
        &AntennaAutomation,   // Point to the automation table for the input pin
        {  // Specify members of a KSPIN_DESCRIPTOR structure for the input pin
            0,  // Interfaces
            NULL,
            0,  // Mediums
            NULL,
            SIZEOF_ARRAY(AntennaPinRanges),
            AntennaPinRanges,  
            KSPIN_DATAFLOW_IN,  // specifies that data flow is into the pin
            KSPIN_COMMUNICATION_BOTH, // specifies that the pin factory instantiates pins 
                                                                               // that are both IRP sinks and IRP sources
            NULL,   //  Category GUID
            NULL,   // GUID of the localized Unicode string name for the pin type
            0
        },  // Specify flags
        KSPIN_FLAG_DO_NOT_USE_STANDARD_TRANSPORT | 
            KSPIN_FLAG_FRAMES_NOT_REQUIRED_FOR_PROCESSING | 
            KSPIN_FLAG_FIXED_FORMAT,
        1,  // Specify the maximum number of possible instances of the input pin
        1,      // Specify the number of instances of this pin type that are necessary for proper functioning of this filter
        NULL,   // Point to KSALLOCATOR_FRAMING_EX structure for allocator framing
        CAntennaPin::IntersectDataFormat    // Point to the data intersection handler function
    }
};


//
//  Template Pin Descriptors
//
//  This data structure defines the pin types available in the filters
//  template topology.  These structures will be used to create a
//  KDPinFactory for a pin type when BdaCreatePin or BdaMethodCreatePin
//  are called.
//
//  This structure defines ALL pins the filter is capable of supporting,
//  including those pins which may only be created dynamically by a ring
//  3 component such as a Network Provider.
//
//
const
KSPIN_DESCRIPTOR_EX
TemplatePinDescriptors[] =
{
    //  Antenna Input Pin
    //
    {
        &AntennaPinDispatch,   // Point to the dispatch table for the input pin
        &AntennaAutomation,   // Point to the automation table for the input pin
        {  // Specify members of a KSPIN_DESCRIPTOR structure for the input pin
            0,  // Interfaces
            NULL,
            0,  // Mediums
            NULL,
            SIZEOF_ARRAY(AntennaPinRanges),
            AntennaPinRanges,  
            KSPIN_DATAFLOW_IN,  // specifies that data flow is into the pin
            KSPIN_COMMUNICATION_BOTH, // specifies that the pin factory instantiates pins 
                                                                               // that are both IRP sinks and IRP sources
            NULL,   //  Category GUID
            NULL,   // GUID of the localized Unicode string name for the pin type
            0
        },  // Specify flags
        KSPIN_FLAG_DO_NOT_USE_STANDARD_TRANSPORT | 
            KSPIN_FLAG_FRAMES_NOT_REQUIRED_FOR_PROCESSING | 
            KSPIN_FLAG_FIXED_FORMAT,
        1,  // Specify the maximum number of possible instances of the input pin
        1,  // Specify the number of instances of this pin type that are necessary for proper functioning of this filter
        NULL,   // Point to KSALLOCATOR_FRAMING_EX structure for allocator framing
        CAntennaPin::IntersectDataFormat    // Point to the data intersection handler function
    },

    //
    // Capture Output Pin
    //
    {
        &CapturePinDispatch,
        NULL,             
        {
            NULL,                           // Interfaces (NULL, 0 == default)
            0,
            NULL,                           // Mediums (NULL, 0 == default)
            0,
            SIZEOF_ARRAY (CaptureOutPinDataRanges), // Range Count
            CaptureOutPinDataRanges,           // Ranges
            KSPIN_DATAFLOW_OUT,             // Dataflow
            KSPIN_COMMUNICATION_BOTH,       // Communication
            NULL,         // Category
            NULL,         // Name
            0                               // Reserved
        },
        
#if !defined(_BUILD_SW_TUNER_ON_X64)
        KSPIN_FLAG_GENERATE_MAPPINGS |      // Pin Flags
#endif
        KSPIN_FLAG_PROCESS_IN_RUN_STATE_ONLY,
        1,                                  // Instances Possible
        0,                                  // Instances Necessary
        &CapturePinAllocatorFraming,        // Allocator Framing
        CCapturePin::IntersectDataFormat   // Format Intersect Handler
    }

#if 0
    //  Tranport Output Pin
    //
    {
        &TransportPinDispatch,   // Point to the dispatch table for the output pin
        &TransportAutomation,   // Point to the automation table for the output pin
        {  // Specify members of a KSPIN_DESCRIPTOR structure for the output pin
            0,  // Interfaces
            NULL,
            1,  // Mediums
            &TransportPinMedium,
            SIZEOF_ARRAY(TransportPinRanges),
            TransportPinRanges,
//            0,
//            NULL,
            KSPIN_DATAFLOW_OUT, // specifies that data flow is out of the pin
            KSPIN_COMMUNICATION_BOTH, // specifies that the pin factory instantiates pins 
                                                                               // that are both IRP sinks and IRP sources
//            NULL,//Name
//            NULL,//Category
            (GUID *) &PINNAME_BDA_TRANSPORT,   //  Category GUID
            (GUID *) &PINNAME_BDA_TRANSPORT,   // GUID of the localized Unicode string 
                                                                                              // name for the pin type
            0
        },  // Specify flags
        KSPIN_FLAG_DO_NOT_USE_STANDARD_TRANSPORT | 
            KSPIN_FLAG_FRAMES_NOT_REQUIRED_FOR_PROCESSING | 
            KSPIN_FLAG_FIXED_FORMAT,
        1,  // Specify the maximum number of possible instances of the output pin
        0,  // Specify the number of instances of this pin type that are necessary for proper functioning of this filter
        NULL,   // Allocator Framing
        CTransportPin::IntersectDataFormat    // Point to the data intersection handler function
    }

#endif
};


//
//  BDA Device Topology Property Set
//
//  The BDA Support Library supplies a default implementation of the
//  BDA Device Topology Property Set.  If the driver needs to override
//  this default implemenation, the definitions for the override properties
//  will be defined here.
//


//
//  BDA Change Sync Method Set
//
//  The Change Sync Method Set is required on BDA filters.  Setting a
//  node property should not become effective on the underlying device
//  until CommitChanges is called.
//
//  The BDA Support Library provides routines that handle committing
//  changes to topology.  The BDA Support Library routines should be
//  called from the driver's implementation before the driver implementation
//  returns.
//
DEFINE_KSMETHOD_TABLE(BdaChangeSyncMethods)
{
    DEFINE_KSMETHOD_ITEM_BDA_START_CHANGES(
        CFilter::StartChanges, // Calls BdaStartChanges
        NULL
        ),
    DEFINE_KSMETHOD_ITEM_BDA_CHECK_CHANGES(
        CFilter::CheckChanges, // Calls BdaCheckChanges
        NULL
        ),
    DEFINE_KSMETHOD_ITEM_BDA_COMMIT_CHANGES(
        CFilter::CommitChanges, // Calls BdaCommitChanges
        NULL
        ),
    DEFINE_KSMETHOD_ITEM_BDA_GET_CHANGE_STATE(
        CFilter::GetChangeState, // Calls BdaGetChangeState
        NULL
        )
};

//  Override the standard pin medium property set so that we can provide
//  device specific medium information.
//
//  Because the property is on a Pin Factory and not on a pin instance,
//  this is a filter property.
//
DEFINE_KSPROPERTY_TABLE( SampleFilterPropertyOverrides)
{

    DEFINE_KSPROPERTY_ITEM_PIN_MEDIUMS(
        CFilter::GetMedium
        )
};

DEFINE_KSPROPERTY_SET_TABLE(SampleFilterPropertySets)
{
    DEFINE_KSPROPERTY_SET
    (
        &KSPROPSETID_Pin,                            // Property Set GUID
        SIZEOF_ARRAY(SampleFilterPropertyOverrides), // Number of Properties
        SampleFilterPropertyOverrides,               // Array of KSPROPERTY_ITEM structures 
        0,                                           // FastIoCount
        NULL                                         // FastIoTable
    )

    //
    //  Additional property sets for the filter can be added here.
    //
};


//
//  BDA Device Configuration Method Set
//
//  The BDA Support Library provides a default implementation of
//  the BDA Device Configuration Method Set.  In this example, the
//  driver overrides the CreateTopology method.  Note that the
//  support libraries CreateTopology method is called before the
//  driver's implementation returns.
//
DEFINE_KSMETHOD_TABLE(BdaDeviceConfigurationMethods)
{
    DEFINE_KSMETHOD_ITEM_BDA_CREATE_TOPOLOGY(
        CFilter::CreateTopology, // Calls BdaMethodCreateTopology
        NULL
        )
};


DEFINE_KSMETHOD_TABLE( SampleFilterEventingMethods)
{
    DEFINE_KSMETHOD_ITEM_BDA_EVENT_DATA(CFilter::GetEvent, NULL),
    DEFINE_KSMETHOD_ITEM_BDA_EVENT_COMPLETE(CFilter::CompleteEvent, NULL)
};

DEFINE_KSMETHOD_TABLE( SampleFilterDebugMethods)
{
    DEFINE_KSMETHOD_ITEM_BDA_DEBUG_DATA(CFilter::GetDebugData, NULL),
    DEFINE_KSMETHOD_ITEM_BDA_DEBUG_LEVEL(CFilter::SetDebugLevel, NULL)

};

DEFINE_KSMETHOD_TABLE( SampleFilterNameValueMethods)
{
    DEFINE_KSMETHOD_ITEM_BDA_GPNV_GETVALUE(CFilter::GetNameValue, NULL),
    DEFINE_KSMETHOD_ITEM_BDA_GPNV_SETVALUE(CFilter::SetNameValue, NULL),
    DEFINE_KSMETHOD_ITEM_BDA_GPNV_NAMEFROMINDEX(CFilter::GetNameFromIndex, NULL),
    DEFINE_KSMETHOD_ITEM_BDA_GPNV_GETVALUEUPDATENAME(CFilter::GetValueUpdateName, NULL)
};

DEFINE_KSMETHOD_TABLE( SampleFilterGuideDataMethods)
{
    DEFINE_KSMETHOD_ITEM_BDA_GDDS_DATATYPE(CFilter::GuideDataType, NULL),
    DEFINE_KSMETHOD_ITEM_BDA_GDDS_DATA(CFilter::GuideDataValue, NULL),
    DEFINE_KSMETHOD_ITEM_BDA_GDDS_TUNEXMLFROMIDX(CFilter::GetTuneXMLFromIDX, NULL),
    DEFINE_KSMETHOD_ITEM_BDA_GDDS_GETSERVICES(CFilter::GetServices, NULL),
    DEFINE_KSMETHOD_ITEM_BDA_GDDS_SERVICEFROMTUNEXML(CFilter::GetServiceFromTuneXML, NULL),
    DEFINE_KSMETHOD_ITEM_BDA_GDDS_DATAUPDATE(CFilter::GetGuideDataUpdate, NULL)
};

DEFINE_KSMETHOD_TABLE( SampleFilterUserActivityMethods)
{
    DEFINE_KSMETHOD_ITEM_BDA_USERACTIVITY_USEREASON(CFilter::SetUserActivityUseReason, NULL),
    DEFINE_KSMETHOD_ITEM_BDA_USERACTIVITY_INTERVAL(CFilter::GetUserActivityInterval, NULL),
    DEFINE_KSMETHOD_ITEM_BDA_USERACTIVITY_DETECTED(CFilter::UserActivityDetected, NULL),
};

//==================================================
// PBDA Scanning and Tuning Service Methods
//==================================================
DEFINE_KSMETHOD_TABLE(SamplePBDATuningServiceMethods)
{
    DEFINE_KSMETHOD_ITEM_BDA_TUNER_SETTUNER(CFilter::PBDA_Tuning_SetTuner,NULL),
    DEFINE_KSMETHOD_ITEM_BDA_TUNER_GETTUNERSTATE(CFilter::PBDA_Tuning_GetTunerState,NULL),
    DEFINE_KSMETHOD_ITEM_BDA_TUNER_SIGNALNOISERATIO(CFilter::PBDA_Tuning_GetSignalNoiseRatio,NULL)
};

DEFINE_KSMETHOD_TABLE(SamplePBDAScanningServiceMethods)
{
    DEFINE_KSMETHOD_ITEM_BDA_SCAN_CAPABILTIES(CFilter::PBDA_Scanning_GetCapabilities,NULL),
    DEFINE_KSMETHOD_ITEM_BDA_SCANNING_STATE(CFilter::PBDA_Scanning_GetScanningState,NULL),
    DEFINE_KSMETHOD_ITEM_BDA_SCAN_FILTER(CFilter::PBDA_Scanning_SetScanFilter,NULL),
    DEFINE_KSMETHOD_ITEM_BDA_SCAN_START(CFilter::PBDA_Scanning_ScanStart,NULL),
    DEFINE_KSMETHOD_ITEM_BDA_SCAN_RESUME(CFilter::PBDA_Scanning_ScanResume,NULL),
    DEFINE_KSMETHOD_ITEM_BDA_SCAN_STOP(CFilter::PBDA_Scanning_ScanStop,NULL)
};

//
//  Define an array of method sets that the filter supports
//
DEFINE_KSMETHOD_SET_TABLE(FilterMethodSets)
{
    DEFINE_KSMETHOD_SET
    (
        &KSMETHODSETID_BdaChangeSync,       // Method set GUID
        SIZEOF_ARRAY(BdaChangeSyncMethods), // Number of methods
        BdaChangeSyncMethods,               // Array of KSMETHOD_ITEM structures 
        0,                                  // FastIoCount
        NULL                                // FastIoTable
    ),

    DEFINE_KSMETHOD_SET
    (
        &KSMETHODSETID_BdaDeviceConfiguration,       // Method set GUID
        SIZEOF_ARRAY(BdaDeviceConfigurationMethods), // Number of methods
        BdaDeviceConfigurationMethods,  // Array of KSMETHOD_ITEM structures 
        0,                                           // FastIoCount
        NULL                                         // FastIoTable
    )
#if ISDBT_RECEIVER || ISDBS_RECEIVER  
    ,
    DEFINE_KSMETHOD_SET
    (
        &KSMETHODSETID_BdaEventing,                   // Method Set GUID
        SIZEOF_ARRAY(SampleFilterEventingMethods), // Number of Methods
        SampleFilterEventingMethods,               // Array of KSMETHOD_ITEM structures
        0,                                           // FastIoCount
        NULL                                         // FastIoTable
    ),
    DEFINE_KSMETHOD_SET
    (
        &KSMETHODSETID_BdaDebug,                          // Method Set GUID
        SIZEOF_ARRAY(SampleFilterDebugMethods), // Number of Methods
        SampleFilterDebugMethods,               // Array of KSMETHOD_ITEM structures
        0,                                           // FastIoCount
        NULL                                         // FastIoTable
    ),
    DEFINE_KSMETHOD_SET
    (
        &KSMETHODSETID_BdaNameValue,                // Method Set GUID
        SIZEOF_ARRAY(SampleFilterNameValueMethods), // Number of Methods
        SampleFilterNameValueMethods,               // Array of KSMETHOD_ITEM structures
        0,                                          // FastIoCount
        NULL                                        // FastIoTable
    )
#elif defined(NETOP_RECEIVER) || defined(PBDAT_RECEIVER)    
    ,
    DEFINE_KSMETHOD_SET
    (
        &KSMETHODSETID_BdaEventing,                   // Method Set GUID
        SIZEOF_ARRAY(SampleFilterEventingMethods), // Number of Methods
        SampleFilterEventingMethods,               // Array of KSMETHOD_ITEM structures
        0,                                           // FastIoCount
        NULL                                         // FastIoTable
    ),
    DEFINE_KSMETHOD_SET
    (
        &KSMETHODSETID_BdaDebug,                          // Method Set GUID
        SIZEOF_ARRAY(SampleFilterDebugMethods), // Number of Methods
        SampleFilterDebugMethods,               // Array of KSMETHOD_ITEM structures
        0,                                           // FastIoCount
        NULL                                         // FastIoTable
    ),
    DEFINE_KSMETHOD_SET
    (
        &KSMETHODSETID_BdaNameValue,                // Method Set GUID
        SIZEOF_ARRAY(SampleFilterNameValueMethods), // Number of Methods
        SampleFilterNameValueMethods,               // Array of KSMETHOD_ITEM structures
        0,                                          // FastIoCount
        NULL                                        // FastIoTable
    ),
    DEFINE_KSMETHOD_SET
    (
        &KSMETHODSETID_BdaGuideDataDeliveryService, // Method Set GUID
        SIZEOF_ARRAY(SampleFilterGuideDataMethods), // Number of Methods
        SampleFilterGuideDataMethods,               // Array of KSMETHOD_ITEM structures
        0,                                          // FastIoCount
        NULL                                        // FastIoTable
    ),
#ifdef NETOP_RECEIVER
    DEFINE_KSMETHOD_SET
    (
        &KSMETHODSETID_BdaTuner,    // Method Set defined elsewhere
        SIZEOF_ARRAY(SamplePBDATuningServiceMethods),  // Number of methods in the array
        SamplePBDATuningServiceMethods,  // Method set array
        0,      // FastIoCount
        NULL    // FastIoTable
    ),
    DEFINE_KSMETHOD_SET
    (
        &KSMETHODSETID_BdaScanning,    // Method Set defined elsewhere
        SIZEOF_ARRAY(SamplePBDAScanningServiceMethods),  // Number of methods in the array
        SamplePBDAScanningServiceMethods,  // Method set array
        0,      // FastIoCount
        NULL    // FastIoTable
    ),
#endif
    DEFINE_KSMETHOD_SET
    (
        &KSMETHODSETID_BdaUserActivity,                // Method Set GUID
        SIZEOF_ARRAY(SampleFilterUserActivityMethods), // Number of Methods
        SampleFilterUserActivityMethods,               // Array of KSMETHOD_ITEM structures
        0,                                          // FastIoCount
        NULL                                        // FastIoTable
    ),
   
#elif defined(DVBS_RECEIVER) || defined(DMBTH_RECEIVER)    
    #ifdef USE_DRM_SERVICE
    ,
    DEFINE_KSMETHOD_SET
    (
        &KSMETHODSETID_BdaEventing,                   // Method Set GUID
        SIZEOF_ARRAY(SampleFilterEventingMethods), // Number of Methods
        SampleFilterEventingMethods,               // Array of KSMETHOD_ITEM structures
        0,                                           // FastIoCount
        NULL                                         // FastIoTable
    ),
    DEFINE_KSMETHOD_SET
    (
        &KSMETHODSETID_BdaDebug,                          // Method Set GUID
        SIZEOF_ARRAY(SampleFilterDebugMethods), // Number of Methods
        SampleFilterDebugMethods,               // Array of KSMETHOD_ITEM structures
        0,                                           // FastIoCount
        NULL                                         // FastIoTable
    ),
    DEFINE_KSMETHOD_SET
    (
        &KSMETHODSETID_BdaNameValue,                // Method Set GUID
        SIZEOF_ARRAY(SampleFilterNameValueMethods), // Number of Methods
        SampleFilterNameValueMethods,               // Array of KSMETHOD_ITEM structures
        0,                                          // FastIoCount
        NULL                                        // FastIoTable
    ),
    DEFINE_KSMETHOD_SET
    (
        &KSMETHODSETID_BdaGuideDataDeliveryService, // Method Set GUID
        SIZEOF_ARRAY(SampleFilterGuideDataMethods), // Number of Methods
        SampleFilterGuideDataMethods,               // Array of KSMETHOD_ITEM structures
        0,                                          // FastIoCount
        NULL                                        // FastIoTable
    ),   
    DEFINE_KSMETHOD_SET
    (
        &KSMETHODSETID_BdaUserActivity,                // Method Set GUID
        SIZEOF_ARRAY(SampleFilterUserActivityMethods), // Number of Methods
        SampleFilterUserActivityMethods,               // Array of KSMETHOD_ITEM structures
        0,                                          // FastIoCount
        NULL                                        // FastIoTable
    )
    #endif
#endif    
};
DEFINE_KSEVENT_TABLE(FilterPendingEventItem)
{
    DEFINE_KSEVENT_ITEM_BDA_PENDINGEVENT(
        CFilter::EnablePendingEvent,
        CFilter::DisablePendingEvent,
        NULL)
};

DEFINE_KSEVENT_SET_TABLE(SampleFilterEventSets)
{
    DEFINE_KSEVENT_SET(
        &KSEVENTSETID_BdaEvent,
        SIZEOF_ARRAY(FilterPendingEventItem),
        FilterPendingEventItem
        )
};

#define KS_PROPS DEFINE_KSAUTOMATION_PROPERTIES_NULL

//
//  Filter Automation Table
//
//  Lists all Property, Method, and Event Set tables for the filter
//
DEFINE_KSAUTOMATION_TABLE(FilterAutomation) {
    KS_PROPS,
//$$$    DEFINE_KSAUTOMATION_PROPERTIES(SampleFilterTestPropertySets),
    DEFINE_KSAUTOMATION_METHODS(FilterMethodSets),
#if ISDBT_RECEIVER || ISDBS_RECEIVER    
    DEFINE_KSAUTOMATION_EVENTS(SampleFilterEventSets)
#elif defined(NETOP_RECEIVER) || defined(PBDAT_RECEIVER)  || defined(DMBTH_RECEIVER)    
    DEFINE_KSAUTOMATION_EVENTS(SampleFilterEventSets)
#elif defined(DVBS_RECEIVER) && defined(USE_DRM_SERVICE)
    DEFINE_KSAUTOMATION_EVENTS(SampleFilterEventSets)
#else
    DEFINE_KSAUTOMATION_EVENTS_NULL
#endif
};

//
//  Filter Dispatch Table
//
//  Lists the dispatch routines for major events at the filter
//  level.
//
const
KSFILTER_DISPATCH
FilterDispatch =
{
    CFilter::Create,        // Create
    CFilter::FilterClose,   // Close
    NULL,                   // Process
    NULL                    // Reset
};


//
//  Define the Filter Factory Descriptor for the filter
//
//  This structure brings together all of the structures that define
//  the tuner filter as it appears when it is first instantiated.
//  Note that not all of the template pin and node types may be exposed as
//  pin and node factories when the filter is first instanciated.
//
//  If a driver exposes more than one filter, each filter must have a
//  unique ReferenceGuid.
//
DEFINE_KSFILTER_DESCRIPTOR(InitialFilterDescriptor)
{
    &FilterDispatch,        // Dispatch
    &FilterAutomation,  // AutomationTable
    KSFILTER_DESCRIPTOR_VERSION,  // Version
    0,                                 // Flags
    &KSNAME_BdaSWTunerFilter,  // ReferenceGuid
    DEFINE_KSFILTER_PIN_DESCRIPTORS(InitialPinDescriptors), 
                                       // PinDescriptorsCount; must expose at least one pin
                                       // PinDescriptorSize; size of each item
                                       // PinDescriptors; table of pin descriptors
      //    DEFINE_KSFILTER_CATEGORY(KSCATEGORY_BDA_NETWORK_TUNER),
    DEFINE_KSFILTER_CATEGORIES(TemplateFilterCategories),
                                       // CategoriesCount; number of categories in the table
                                       // Categories; table of categories
    DEFINE_KSFILTER_NODE_DESCRIPTORS_NULL,              
                                       // NodeDescriptorsCount; in this case, 0
                                       // NodeDescriptorSize; in this case, 0
                                       // NodeDescriptors; in this case, NULL
    DEFINE_KSFILTER_DEFAULT_CONNECTIONS,
    // Automatically fills in the connections table for a filter which defines no explicit connections
                                       // ConnectionsCount; number of connections in the table
                                       // Connections; table of connections
    NULL                        // ComponentId; in this case, no ID is provided
};


//===========================================================================
//
//  Define Filter Template Topology 
//
//===========================================================================

//
//  Define BDA Template Topology Connections
//
//  Lists the Connections that are possible between pin types and
//  node types.  This, together with the Template Filter Descriptor, and
//  the Pin Pairings, describe how topologies can be created in the filter.
//
//                 ===========         ============
//  AntennaPin ----| RF Node |--Joint--|Demod Node|----TransportPin
//                 ===========         ============
//
//  The RF Node of this filter is controlled by the Antenna input pin.
//  RF properties will be set as NODE properties (with NodeType == 0)
//  on the filter's Antenna Pin
//
//  The Demodulator Node of this filter is controlled by the Transport output pin.
//  Demod properties will be set as NODE properties (with NodeType == 1 ) 
//  on the filter's Transport Pin
//
const
KSTOPOLOGY_CONNECTION TemplateFilterConnections[] =
{   // KSFILTER_NODE  is defined as ((ULONG)-1) in ks.h 
     // Indicate pin types by the item number is the TemplatePinDescriptors array.
     // Indicate node types by either the item number in the NodeDescriptors array
     // or the element in the BDA_SAMPLE_NODE enumeration.
#ifdef USE_DRM_SERVICE
    { KSFILTER_NODE, 0,                 BDA_SAMPLE_TUNER_NODE, 0},
    { BDA_SAMPLE_TUNER_NODE, 0,         BDA_SAMPLE_DEMODULATOR_NODE, 0},
    { BDA_SAMPLE_DEMODULATOR_NODE, 0,   BDA_SAMPLE_DRM_NODE, 1},
    { BDA_SAMPLE_DRM_NODE, 1,           BDA_SAMPLE_CORECAS_NODE, 1},
#if defined(ISDBT_RECEIVER) || defined(ISDBS_RECEIVER)
    { BDA_SAMPLE_CORECAS_NODE, 1,       BDA_SAMPLE_ISDBCAS_NODE, 1},
    { BDA_SAMPLE_ISDBCAS_NODE, 1,       BDA_MUX_SERVICE_NODE, 1},
#else
    { BDA_SAMPLE_CORECAS_NODE, 1,       BDA_MUX_SERVICE_NODE, 1},
#endif
    { BDA_MUX_SERVICE_NODE, 1,          BDA_CA_POD_NODE, 1},
    { BDA_CA_POD_NODE, 1,               BDA_PID_FILTER_NODE, 1},
#if defined(ISDBS_RECEIVER) || defined(DVBT_RECEIVER) || defined(DMBTH_RECEIVER)
    { BDA_PID_FILTER_NODE, 1,           BDA_TS_SELECTOR_NODE, 1},       
    { BDA_TS_SELECTOR_NODE, 1,          KSFILTER_NODE, 1}                 
#else
    { BDA_PID_FILTER_NODE, 1,           KSFILTER_NODE, 1}                 
#endif

#else 
    { KSFILTER_NODE, 0,                 BDA_SAMPLE_TUNER_NODE, 0},
    { BDA_SAMPLE_TUNER_NODE, 0,         BDA_SAMPLE_DEMODULATOR_NODE, 0},
    { BDA_SAMPLE_DEMODULATOR_NODE, 0,   BDA_MUX_SERVICE_NODE, 0},
    { BDA_MUX_SERVICE_NODE, 0,          BDA_CA_POD_NODE, 0},
    { BDA_CA_POD_NODE, 0,               BDA_PID_FILTER_NODE, 0},
#if defined(DVBT_RECEIVER) || defined(DMBTH_RECEIVER)
    { BDA_PID_FILTER_NODE, 0,           BDA_TS_SELECTOR_NODE, 0},       
    { BDA_TS_SELECTOR_NODE, 0,          KSFILTER_NODE, 1}                 
#else
    { BDA_PID_FILTER_NODE, 0,           KSFILTER_NODE, 1}  
#endif
#endif
};

//
//  Template Joints between the Antenna and Transport Pin Types.
//
//  Lists the template joints between the Antenna Input Pin Type and
//  the Transport Output Pin Type.
//
//  In this case the RF Node is considered to belong to the antennea input
//  pin and the 8VSB Demodulator Node is considered to belong to the
//  tranport stream output pin.
//
const
ULONG   AntennaTransportJoints[] =
{
    1 // joint occurs between the two node types (second element in array)
       // indicates that 1st node is controlled by input pin and 2nd node by output pin
};
//
//  Define Template Pin Parings.
//
//  Array of BDA_PIN_PAIRING structures that are used to determine 
//  which nodes get duplicated when more than one output pin type is 
//  connected to a single input pin type or when more that one input pin 
//  type is connected to a single output pin type.
//  
const
BDA_PIN_PAIRING TemplatePinPairings[] =
{
    //  Input pin to Output pin Topology Joints
    //
    {
        0,  // ulInputPin; 0 element in the TemplatePinDescriptors array.
        1,  // ulOutputPin; 1 element in the TemplatePinDescriptors array.
        1,  // ulcMaxInputsPerOutput
        1,  // ulcMinInputsPerOutput
        1,  // ulcMaxOutputsPerInput
        1,  // ulcMinOutputsPerInput
        SIZEOF_ARRAY(AntennaTransportJoints),   // ulcTopologyJoints
        AntennaTransportJoints   // pTopologyJoints; array of joints
    }
    //  If applicable, list topology of joints between other pins.
    //
};





#if 0
//
// CaptureFilterDispatch:
//
// This is the dispatch table for the capture filter.  It provides notification
// of creation, closure, processing (for filter-centrics, not for the capture
// filter), and resets (for filter-centrics, not for the capture filter).
//
const 
KSFILTER_DISPATCH
CaptureFilterDispatch = {
    CCaptureFilter::DispatchCreate,         // Filter Create
    NULL,                                   // Filter Close
    NULL,                                   // Filter Process
    NULL                                    // Filter Reset
};
#endif
DEFINE_KSPROPERTY_TABLE(SampleCaptureNodeSetProperties)
{
    DEFINE_KSPROPERTY_ITEM                                
    (

        KSPROPERTY_CAPTURENODE_SETFREQ,                        // Property
        CFilter::ProcessFrequencyFromTuner,            // Get Handler
        sizeof(KSPROPERTY),                                    // MinProperty
        sizeof(ULONG),                                        // MinData
        CFilter::ProcessFrequencyFromTuner,            // Set Handler
        NULL,                                            // Values
        0,                                                // RelationsCount
        NULL,                                            // Relations
        NULL,                                            // SupportHandler
        0                                                // SerializedSize
    )
};

DEFINE_KSPROPERTY_SET_TABLE(SampleCaptureNodePropertySets)
{
    DEFINE_KSPROPERTY_SET
    (
        &KSNAME_CaptureNodePropertySet,    // Property Set defined elsewhere
        SIZEOF_ARRAY(SampleCaptureNodeSetProperties),  // Number of properties in the array
        SampleCaptureNodeSetProperties,  // Property set array
        0,      // FastIoCount
        NULL    // FastIoTable
    ),
};

DEFINE_KSAUTOMATION_TABLE(SampleCaptureNodeAutomation) {
    DEFINE_KSAUTOMATION_PROPERTIES(SampleCaptureNodePropertySets),
    DEFINE_KSAUTOMATION_METHODS(FilterMethodSets),
    DEFINE_KSAUTOMATION_EVENTS_NULL
};

//
//  Define the Filter Factory Descriptor that the BDA support library uses 
//  to create template topology for the filter.
//
//  This KSFILTER_DESCRIPTOR structure combines the structures that 
//  define the topologies that the filter can assume as a result of
//  pin factory and topology creation methods.
//  Note that not all of the template pin and node types may be exposed as
//  pin and node factories when the filter is first instanciated.
//
DEFINE_KSFILTER_DESCRIPTOR(TemplateFilterDescriptor)
{
    &FilterDispatch,  // Dispatch
    &SampleCaptureNodeAutomation,  // AutomationTable
    KSFILTER_DESCRIPTOR_VERSION,  // Version
    0,  // Flags
    &KSNAME_BdaSWTunerFilter,  // ReferenceGuid
    DEFINE_KSFILTER_PIN_DESCRIPTORS(TemplatePinDescriptors),
                                       // PinDescriptorsCount; exposes all template pins 
                                       // PinDescriptorSize; size of each item
                                       // PinDescriptors; table of pin descriptors
    DEFINE_KSFILTER_CATEGORIES(TemplateFilterCategories),
                                       // CategoriesCount; number of categories in the table
                                       // Categories; table of categories
    DEFINE_KSFILTER_NODE_DESCRIPTORS(NodeDescriptors),  
                                      // NodeDescriptorsCount; exposes all template nodes
                                      // NodeDescriptorSize; size of each item
                                      // NodeDescriptors; table of node descriptors
    DEFINE_KSFILTER_CONNECTIONS(TemplateFilterConnections), 
                                      // ConnectionsCount; number of connections in the table
                                       // Connections; table of connections
    NULL                        // ComponentId; in this case, no ID is provided
};


DEFINE_KSFILTER_DESCRIPTOR_TABLE(FilterDescriptors)
{
    &TemplateFilterDescriptor//, 
//    &CaptureFilterDescriptor
};

//
//  Define BDA Template Topology Descriptor for the filter.
//
//  This structure combines the filter descriptor and pin pairings that
//  the BDA support library uses to create an instance of the filter.
//
const
BDA_FILTER_TEMPLATE
BdaFilterTemplate =
{
    &TemplateFilterDescriptor,
    SIZEOF_ARRAY(TemplatePinPairings),
    TemplatePinPairings
};



//
//  Define Template Pin Parings.
//
//  Array of BDA_PIN_PAIRING structures that are used to determine 
//  which nodes get duplicated when more than one output pin type is 
//  connected to a single input pin type or when more that one input pin 
//  type is connected to a single output pin type.
//  
const
BDA_PIN_PAIRING CapturePinPairings[] =
{
    //  Input pin to Output pin Topology Joints
    //
    {
        0,  // ulInputPin; 0 element in the TemplatePinDescriptors array.
        1,  // ulOutputPin; 1 element in the TemplatePinDescriptors array.
        1,  // ulcMaxInputsPerOutput
        1,  // ulcMinInputsPerOutput
        1,  // ulcMaxOutputsPerInput
        1,  // ulcMinOutputsPerInput
        SIZEOF_ARRAY(AntennaTransportJoints),   // ulcTopologyJoints
        AntennaTransportJoints   // pTopologyJoints; array of joints
    }
    //  If applicable, list topology of joints between other pins.
    //
};


//
//  Define BDA Template Topology Descriptor for the filter.
//
//  This structure combines the filter descriptor and pin pairings that
//  the BDA support library uses to create an instance of the filter.
//
//const
//BDA_FILTER_TEMPLATE
//CaptureFilterTemplate =
//{
//    &CaptureFilterDescriptor,
//    SIZEOF_ARRAY(CapturePinPairings),
//    CapturePinPairings
//};


//===========================================================================
//
//  Define the Device 
//
//===========================================================================


//
//  Define Device Dispatch Table
//
//  List the dispatch routines for the major events that occur 
//  during the existence of the underlying device.
//
extern
const
KSDEVICE_DISPATCH
DeviceDispatch =
{
    CDevice::Create,    // Add
    CDevice::DispatchPnPStart,     // Start
    NULL,               // PostStart
    NULL,               // QueryStop
    NULL,               // CancelStop
    CDevice::DispatchPnPStop,               // Stop
    NULL,               // QueryRemove
    NULL,               // CancelRemove
    NULL,               // Remove
    NULL,               // QueryCapabilities
    NULL,               // SurpriseRemoval
    NULL,               // QueryPower
    NULL                // SetPower
};


//
//  Define Device Descriptor
//
//  Combines structures that define the device and any non-BDA
//  intial filter factories that can be created on it.
//  Note that this structure does not include the template topology
//  structures as they are specific to BDA.
//
extern
const
KSDEVICE_DESCRIPTOR
DeviceDescriptor =
{
    &DeviceDispatch,    // Dispatch
    0,   // FilterDescriptorsCount
    NULL                  // FilterDescriptors
};

//extern
//const
//KSDEVICE_DESCRIPTOR
//DeviceDescriptor =
//{
//    &DeviceDispatch,    // Dispatch
//    SIZEOF_ARRAY( FilterDescriptors),   // FilterDescriptorsCount
//    FilterDescriptors,                   // FilterDescriptors
//    KSDEVICE_DESCRIPTOR_VERSION
//};



