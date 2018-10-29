/*
	Copyright (c) Microsoft Corporation.  All rights reserved.
*/

#ifndef _SDV_NTIFS_CONTEXT_

#ifndef _SDV_NTDDK_H_
#define _SDV_NTDDK_H_

#include <sdv_wdm.h>




#ifdef HalQuerySystemInformation
#undef HalQuerySystemInformation
#endif
#define HalQuerySystemInformation sdv_HalQuerySystemInformation
NTSTATUS
sdv_HalQuerySystemInformation(
    IN HAL_QUERY_INFORMATION_CLASS  InformationClass,
    IN ULONG  BufferSize,
    OUT PVOID  Buffer,
    OUT PULONG  ReturnedLength
    );


#ifdef KeRaiseIrqlToSynchLevel
#undef KeRaiseIrqlToSynchLevel
#endif
#define KeRaiseIrqlToSynchLevel sdv_KeRaiseIrqlToSynchLevel
KIRQL
sdv_KeRaiseIrqlToSynchLevel(
    VOID
    );

#ifdef KeRaiseIrqlToDpcLevel
#undef KeRaiseIrqlToDpcLevel
#endif
#define KeRaiseIrqlToDpcLevel sdv_KeRaiseIrqlToDpcLevel
KIRQL
sdv_KeRaiseIrqlToDpcLevel(
    VOID
    );

#endif

#endif
