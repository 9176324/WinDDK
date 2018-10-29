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

/***************************************************************************
    File    : anlgdevice.h
    Abstract: Contains descriptor for the AVStream device
****************************************************************************/
extern "C" {
#include <wdm.h>
#include "wdmdebug.h"
}

#include <winerror.h>
#include <unknown.h>
#include <ks.h>
#include <ksmedia.h>
#include <bdatypes.h>
#include <bdamedia.h>
#include <atsmedia.h>
#include <bdasup.h>

#include <kcom.h>
#include <ksdebug.h>

#undef INTERFACE

#define STR_MODULENAME "BDA Analog Tuner"

extern const KSFILTER_DESCRIPTOR BdaAnlgTunerFilterDescriptor;
extern const KSFILTER_DESCRIPTOR BdaXBarFilterDescriptor;
extern const KSFILTER_DESCRIPTOR BdaEncoderFilterDescriptor;
extern const KSFILTER_DESCRIPTOR BdaTVAudioFilterDescriptor;
extern const KSFILTER_DESCRIPTOR BdaAnlgCaptureFilterDescriptor;

STDMETHODIMP_(NTSTATUS)
Create(
    IN PKSDEVICE Device
    );
STDMETHODIMP_(NTSTATUS)
Start(
    IN PKSDEVICE            pKSDevice,
    IN PIRP                 pIrp,
    IN PCM_RESOURCE_LIST    pTranslatedResourceList OPTIONAL,
    IN PCM_RESOURCE_LIST    pUntranslatedResourceList OPTIONAL
    );



const
KSDEVICE_DISPATCH
DeviceDispatch =
{
    Create,             // Add
    Start,              // Start
    NULL,               // PostStart
    NULL,               // QueryStop
    NULL,               // CancelStop
    NULL,               // Stop
    NULL,               // QueryRemove
    NULL,               // CancelRemove
    NULL,               // Remove
    NULL,               // QueryCapabilities
    NULL,               // SurpriseRemoval
    NULL,               // QueryPower
    NULL                // SetPower
};

DEFINE_KSFILTER_DESCRIPTOR_TABLE(FilterDescriptor)
{
    &BdaAnlgTunerFilterDescriptor,
    &BdaXBarFilterDescriptor,
    &BdaEncoderFilterDescriptor,
    &BdaTVAudioFilterDescriptor,
    &BdaAnlgCaptureFilterDescriptor
};


const
KSDEVICE_DESCRIPTOR
DeviceDescriptor =
{
    &DeviceDispatch,                   // Dispatch
   SIZEOF_ARRAY( FilterDescriptor),    // FilterDescriptorsCount
   FilterDescriptor                    // Filter Descriptor Table
};


