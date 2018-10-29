/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    netioddk.h

Abstract:

    This module contains the DDK definitions and structures for the
    network I/O subsystem.

Environment:

    kernel mode only

--*/

#ifndef _NETIODDK_
#define _NETIODDK_
#pragma once

#include <netiodef.h>

#ifdef __cplusplus
extern "C" {
#endif

#pragma warning(push)
#pragma warning(disable:4200) // zero-sized array in struct/union


#if (NTDDI_VERSION >= NTDDI_VISTA)

//
// These types define the pointers to constant data supplied by a client
// or provider during its registration.  The data pointed to must be
// kept valid for the life of the registration.
//

typedef CONST struct _NPI_CLIENT_CHARACTERISTICS * PNPI_CLIENT_CHARACTERISTICS;
typedef CONST struct _NPI_PROVIDER_CHARACTERISTICS * PNPI_PROVIDER_CHARACTERISTICS;
typedef CONST struct _NPI_REGISTRATION_INSTANCE * PNPI_REGISTRATION_INSTANCE;

//
// Network Programming Interface Registration Instance: represents a
// registration of an implementation of a specific network programming
// interface by a specific network module.  The information in this structure
// is specified by the module implementing an instance of a network
// programming interface.
//

typedef struct _NPI_REGISTRATION_INSTANCE {
    USHORT Version;
    USHORT Size;
    PNPIID NpiId;
    PNPI_MODULEID ModuleId;
    ULONG Number;
    CONST VOID  *NpiSpecificCharacteristics OPTIONAL;
} NPI_REGISTRATION_INSTANCE;


//
// A Network Programming Interface (NPI) is a well-defined table of function
// pointers and a caller-specific handle representing the caller of the
// functions.  The handle is usually passed as a parameter to the functions
// as defined by the specific network programming interface definition.
// This structure is a place holder to opaquely describe network programming
// interfaces.  It is usually cast to a specific NPI structure for use.
//
typedef struct _NPI {
    HANDLE Handle;
    CONST VOID* Dispatch;
} NPI;


typedef
NTSTATUS
(NTAPI NPI_CLIENT_ATTACH_PROVIDER_FN)(
    __in HANDLE  NmrBindingHandle,
    __in PVOID  ClientContext,
    __in PNPI_REGISTRATION_INSTANCE  ProviderRegistrationInstance
    );
typedef NPI_CLIENT_ATTACH_PROVIDER_FN *PNPI_CLIENT_ATTACH_PROVIDER_FN;


typedef
NTSTATUS
(NTAPI NPI_CLIENT_DETACH_PROVIDER_FN  )(
    __in PVOID  ClientBindingContext
    );
typedef NPI_CLIENT_DETACH_PROVIDER_FN *PNPI_CLIENT_DETACH_PROVIDER_FN;


typedef
VOID
(NTAPI NPI_CLIENT_CLEANUP_BINDING_CONTEXT_FN)(
    __in PVOID  ClientBindingContext
    );
typedef NPI_CLIENT_CLEANUP_BINDING_CONTEXT_FN *PNPI_CLIENT_CLEANUP_BINDING_CONTEXT_FN;


typedef struct _NPI_CLIENT_CHARACTERISTICS {
    USHORT Version;
    USHORT Length;
    PNPI_CLIENT_ATTACH_PROVIDER_FN  ClientAttachProvider;
    PNPI_CLIENT_DETACH_PROVIDER_FN  ClientDetachProvider;
    PNPI_CLIENT_CLEANUP_BINDING_CONTEXT_FN  ClientCleanupBindingContext;
    NPI_REGISTRATION_INSTANCE  ClientRegistrationInstance;
} NPI_CLIENT_CHARACTERISTICS;


//
// Definition for provider side of structures and prototypes.
//
typedef
NTSTATUS
(NTAPI NPI_PROVIDER_ATTACH_CLIENT_FN)(
    __in HANDLE  NmrBindingHandle,
    __in PVOID  ProviderContext,
    __in PNPI_REGISTRATION_INSTANCE  ClientRegistrationInstance,
    __in PVOID  ClientBindingContext,
    __in CONST VOID *ClientDispatch,
    __out PVOID  *ProviderBindingContext,
    __out CONST VOID*  *ProviderDispatch
    );
typedef NPI_PROVIDER_ATTACH_CLIENT_FN *PNPI_PROVIDER_ATTACH_CLIENT_FN;


typedef
NTSTATUS
(NTAPI NPI_PROVIDER_DETACH_CLIENT_FN)(
    __in PVOID  ProviderBindingContext
    );
typedef NPI_PROVIDER_DETACH_CLIENT_FN *PNPI_PROVIDER_DETACH_CLIENT_FN;


typedef
VOID
(NTAPI NPI_PROVIDER_CLEANUP_BINDING_CONTEXT_FN)(
    __in PVOID  ProviderBindingContext
    );
typedef NPI_PROVIDER_CLEANUP_BINDING_CONTEXT_FN *PNPI_PROVIDER_CLEANUP_BINDING_CONTEXT_FN;


typedef struct _NPI_PROVIDER_CHARACTERISTICS {
    USHORT Version;
    USHORT Length;
    PNPI_PROVIDER_ATTACH_CLIENT_FN ProviderAttachClient;
    PNPI_PROVIDER_DETACH_CLIENT_FN ProviderDetachClient;
    PNPI_PROVIDER_CLEANUP_BINDING_CONTEXT_FN ProviderCleanupBindingContext;
    NPI_REGISTRATION_INSTANCE ProviderRegistrationInstance;
} NPI_PROVIDER_CHARACTERISTICS;


//
// Called by a module to register an instance of an implementation of the
// client side of a particular network programming interface.
//
NTSTATUS
NmrRegisterClient(
    __in PNPI_CLIENT_CHARACTERISTICS  ClientCharacteristics,
    __in PVOID  ClientContext,
    __out PHANDLE  NmrClientHandle
    );

//
// Called by a module to un-register an instance of an implementation of the
// client side of a particular network programming interface.
//
NTSTATUS
NmrDeregisterClient(
    __in HANDLE  NmrClientHandle
    );

//
// Called by a client to close a registration.
//
NTSTATUS
NmrWaitForClientDeregisterComplete(
    __in HANDLE  NmrClientHandle
    );

//
// Called by a client module to attach itself to a provider.
//
NTSTATUS
NmrClientAttachProvider(
    __in HANDLE  NmrBindingHandle,
    __in PVOID  ClientBindingContext,
    __in CONST VOID  *ClientDispatch,
    __out PVOID  *ProviderBindingContext,
    __out CONST VOID*  *ProviderDispatch
    );

//
// Called by a client module to indicate detach to a provider is
// complete.
//
VOID
NmrClientDetachProviderComplete(
    __in HANDLE  NmrBindingHandle
    );


//
// Called by a module to register an instance of an implementation of the
// provider side of a particular network programming interface.
//
NTSTATUS
NmrRegisterProvider(
    __in PNPI_PROVIDER_CHARACTERISTICS  ProviderCharacteristics,
    __in PVOID  ProviderContext,
    __out PHANDLE  NmrProviderHandle
    );

//
// Called by a module to un-register an instance of an implementation of the
// provider side of a particular network programming interface.
//
NTSTATUS
NmrDeregisterProvider(
    __in HANDLE  NmrProviderHandle
    );

//
// Called by a provider to close a registration.
//
NTSTATUS
NmrWaitForProviderDeregisterComplete(
    __in HANDLE  NmrProviderHandle
    );

//
// Called by a provider module to notify NMR that a detach from client
// is complete.
//
VOID
NmrProviderDetachClientComplete(
    __in HANDLE  NmrBindingHandle
    );

#endif // if (NTDDI_VERSION >= NTDDI_VISTA)


#pragma warning(pop)

#ifdef __cplusplus
}
#endif

#endif // _NETIODDK_

