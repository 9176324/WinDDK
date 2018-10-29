//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Copyright 2005 OSR, Open Systems Resources, Inc. All rights Reserved.
// 
// Module Name:
// 
//     smscir.c
// 
// Abstract:
//
//     Contains the entry and unload routines for the driver
//
// Author:
//
// Revision History:
//
#include "smscir.h"

#ifndef WPP_TRACING
ULONG DefaultDebugFlags = 0;
ULONG DefaultDebugLevel = TRACE_LEVEL_VERBOSE;
#else 
#include "smscir.tmh"
#endif


UNICODE_STRING SmscIrRegistryPath;


NTSTATUS
DriverEntry(
    IN  PDRIVER_OBJECT  DriverObject,
    IN  PUNICODE_STRING RegistryPath
    ) {
/*++
Routine Description:

    Initialize the entry points of the driver.

--*/
    WDF_DRIVER_CONFIG config;
    NTSTATUS          status;
    WDFDRIVER         driver;

#ifdef WPP_TRACING
    //
    // Initialize WPP Tracing
    //
    WPP_INIT_TRACING(DriverObject, RegistryPath);
#endif

    SmscIrTracePrint(TRACE_LEVEL_VERBOSE, 
                     SMSCDBG_INIT_INFO, 
                     ("DriverEntry: Entered"));
    SmscIrTracePrint(TRACE_LEVEL_VERBOSE, 
                     SMSCDBG_INIT_INFO, 
                     ("DriverEntry: RegPath = %wZ", RegistryPath));


    //
    // Initiialize driver config to control the attributes that
    // are global to the driver. Note that framework by default
    // provides a driver unload routine. If you create any resources
    // in the DriverEntry and want to be cleaned in driver unload,
    // you can override that by specifing one in the Config structure.
    //
    WDF_DRIVER_CONFIG_INIT(&config,
                           SmscIrEvtDeviceAdd);

    //
    // Need an unload so that we can free our unicode string buffer.
    //
    config.EvtDriverUnload = SmscIrEvtDriverUnload;

    //
    // Create a framework driver object to represent our driver.
    //
    status = WdfDriverCreate(DriverObject,
                             RegistryPath,
                             WDF_NO_OBJECT_ATTRIBUTES,
                             &config,
                             &driver);

    if (!NT_SUCCESS(status)) {

        SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_INIT_INFO,
            ("DriverEntry: WdfDriverCreate failed - 0x%x", status));

#ifdef WPP_TRACING
        //
        // Cleanup WPP Tracing
        //
        WPP_CLEANUP(DriverObject);
#endif

        return status;

    }
    
    //
    // Save the registry path for later use. We need
    //  to make our own copy of it, because the 
    //  RegistryPath parameter pointer is freed when 
    //  we return from this call.
    //
    SmscIrRegistryPath.MaximumLength 
                    = RegistryPath->Length + sizeof(UNICODE_NULL);

    //
    // Allocate room for the registry path. Since 
    //  we're doing this in DriverEntry, we'll free
    //  the memory when our driver is unloaded in
    //  OsrUnload...
    //
    // PagedPool since we can't use the registry
    //  access functions at anything above 
    //  PASSIVE_LEVEL anyway...
    //
    SmscIrRegistryPath.Buffer = 
            (PWSTR)ExAllocatePoolWithTag(PagedPool,
                                         SmscIrRegistryPath.MaximumLength,
                                        'PRSO');

    if (!SmscIrRegistryPath.Buffer) {

#ifdef WPP_TRACING
        //
        // Cleanup WPP Tracing
        //
        WPP_CLEANUP(DriverObject);
#endif

        SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_INIT_INFO,
            ("DriverEntry: ExAllocatePoolWithTag failed"));
        return STATUS_INSUFFICIENT_RESOURCES;
    }
    
    RtlCopyUnicodeString(&SmscIrRegistryPath, RegistryPath);

    SmscIrTracePrint(TRACE_LEVEL_VERBOSE, 
                     SMSCDBG_INIT_INFO, 
                     ("DriverEntry: Exited"));

    return STATUS_SUCCESS;
}



VOID 
SmscIrEvtDriverUnload(
    WDFDRIVER Driver
    ) {
/*++
Routine Description:

    All devices have gone away, this routine represents the 
    image in memory going away.

--*/
    UNREFERENCED_PARAMETER(Driver);

    SmscIrTracePrint(TRACE_LEVEL_VERBOSE, 
                     SMSCDBG_INIT_INFO, 
                     ("SmscIrEvtDriverUnload: Entered"));

    if (SmscIrRegistryPath.Buffer) {

        ExFreePool(SmscIrRegistryPath.Buffer);

    }

#ifdef WPP_TRACING
    //
    // Cleanup WPP Tracing
    //
    WPP_CLEANUP(WdfDriverWdmGetDriverObject(Driver));
#endif

    SmscIrTracePrint(TRACE_LEVEL_VERBOSE, 
                     SMSCDBG_INIT_INFO, 
                     ("SmscIrEvtDriverUnload: Exited"));

}





