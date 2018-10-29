/*
	Copyright (c) Microsoft Corporation.  All rights reserved.
*/

#ifndef _SDV_NTDDK_CONTEXT_

#ifndef _SDV_WDM_H_
#define _SDV_WDM_H_

#define _SDV_NTDDK_H_



#undef _DECL_HAL_KE_IMPORT
#define _DECL_HAL_KE_IMPORT

#undef DECLSPEC_IMPORT
#define DECLSPEC_IMPORT

#undef NTAPI
#define NTAPI __stdcall

#undef NTSYSAPI
#define NTSYSAPI

#undef NTKERNELAPI
#define NTKERNELAPI

#undef NTHALAPI
#define NTHALAPI

/* Turn of warnings that does not make sense for SDV: */

/* Disable warnings about unitialized variables: */
#pragma warning(disable:4700)

/* Disable warnings about inconsistent dll linkage: */
#pragma warning(disable:4273)

/* Disable warnings about unreferenced formal parameters: */
#pragma warning(disable:4100)

/* Disable warnings about formal parameter different from declaration: */
#pragma warning(disable:4028)



#if WINVER <= 0x0500
#endif

#include "csq.h"





BOOLEAN
sdv_NT_ERROR(IN NTSTATUS Status);  

#ifdef NT_ERROR
#undef NT_ERROR
#endif
#define NT_ERROR(arg1) \
( \
sdv_NT_ERROR(arg1) \
)







#ifdef IoAcquireRemoveLock
#undef IoAcquireRemoveLock
#endif
#define IoAcquireRemoveLock sdv_IoAcquireRemoveLock
NTSTATUS
sdv_IoAcquireRemoveLock(
    IN PIO_REMOVE_LOCK RemoveLock,
    IN PVOID Tag
    );

#ifdef IoAdjustPagingPathCount
#undef IoAdjustPagingPathCount
#endif
#define IoAdjustPagingPathCount(arg1, arg2) sdv_IoAdjustPagingPathCount(arg1, arg2);
VOID
sdv_IoAdjustPagingPathCount(
    IN PLONG Count,
    IN BOOLEAN Increment
    );

#ifdef IoAllocateAdapterChannel
#undef IoAllocateAdapterChannel
#endif
#define IoAllocateAdapterChannel sdv_IoAllocateAdapterChannel
NTSTATUS
sdv_IoAllocateAdapterChannel(
    IN PADAPTER_OBJECT AdapterObject,
    IN PDEVICE_OBJECT DeviceObject,
    IN ULONG NumberOfMapRegisters,
    IN PDRIVER_CONTROL ExecutionRoutine,
    IN PVOID Context
    );

#ifdef IoAssignArcName
#undef IoAssignArcName
#endif
#define IoAssignArcName(arg1, arg2) sdv_IoAssignArcName(arg1, arg2);
VOID
sdv_IoAssignArcName(
    IN PUNICODE_STRING ArcName,
    IN PUNICODE_STRING DeviceName
    );

#ifdef IoCallDriver
#undef IoCallDriver
#endif
#define IoCallDriver sdv_IoCallDriver
NTSTATUS
sdv_IoCallDriver(
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PIRP Irp
    );

#ifdef IoCompleteRequest
#undef IoCompleteRequest
#endif
#define IoCompleteRequest(arg1, arg2) sdv_IoCompleteRequest(arg1, arg2);
VOID
sdv_IoCompleteRequest(
    IN PIRP pirp,
    IN CCHAR PriorityBoost
    );

#ifdef IoCopyCurrentIrpStackLocationToNext
#undef IoCopyCurrentIrpStackLocationToNext
#endif
#define IoCopyCurrentIrpStackLocationToNext(arg1) sdv_IoCopyCurrentIrpStackLocationToNext(arg1);
VOID
sdv_IoCopyCurrentIrpStackLocationToNext(
    IN PIRP pirp
    );

#ifdef IoDeassignArcName
#undef IoDeassignArcName
#endif
#define IoDeassignArcName(arg1) sdv_IoDeassignArcName(arg1);
VOID
sdv_IoDeassignArcName(
    IN PUNICODE_STRING ArcName
    );

#ifdef IoFlushAdapterBuffers
#undef IoFlushAdapterBuffers
#endif
#define IoFlushAdapterBuffers sdv_IoFlushAdapterBuffers
BOOLEAN
sdv_IoFlushAdapterBuffers(
    IN PADAPTER_OBJECT AdapterObject,
    IN PMDL Mdl,
    IN PVOID MapRegisterBase,
    IN PVOID CurrentVa,
    IN ULONG Length,
    IN BOOLEAN WriteToDevice
    );

#ifdef IoFreeAdapterChannel
#undef IoFreeAdapterChannel
#endif
#define IoFreeAdapterChannel(arg1) sdv_IoFreeAdapterChannel(arg1);
VOID
sdv_IoFreeAdapterChannel(
    IN PADAPTER_OBJECT AdapterObject
    );

#ifdef IoFreeMapRegisters
#undef IoFreeMapRegisters
#endif
#define IoFreeMapRegisters(arg1, arg2, arg3) sdv_IoFreeMapRegisters(arg1, arg2, arg3);
VOID
sdv_IoFreeMapRegisters(
    IN PADAPTER_OBJECT AdapterObject,
    IN PVOID MapRegisterBase,
    IN ULONG NumberOfMapRegisters
    );

#ifdef IoGetCurrentIrpStackLocation
#undef IoGetCurrentIrpStackLocation
#endif
#define IoGetCurrentIrpStackLocation sdv_IoGetCurrentIrpStackLocation
PIO_STACK_LOCATION
sdv_IoGetCurrentIrpStackLocation(
    IN PIRP pirp
    );

#ifdef IoGetFunctionCodeFromCtlCode
#undef IoGetFunctionCodeFromCtlCode
#endif
#define IoGetFunctionCodeFromCtlCode sdv_IoGetFunctionCodeFromCtlCode
ULONG
sdv_IoGetFunctionCodeFromCtlCode(
    IN ULONG ControlCode
    );

#ifdef IoGetNextIrpStackLocation
#undef IoGetNextIrpStackLocation
#endif
#define IoGetNextIrpStackLocation sdv_IoGetNextIrpStackLocation
PIO_STACK_LOCATION
sdv_IoGetNextIrpStackLocation(
    IN PIRP pirp
    );

#ifdef IoGetRemainingStackSize
#undef IoGetRemainingStackSize
#endif
#define IoGetRemainingStackSize sdv_IoGetRemainingStackSize
ULONG_PTR
sdv_IoGetRemainingStackSize(
    VOID
    );

#ifdef IoInitializeDpcRequest
#undef IoInitializeDpcRequest
#endif
#define IoInitializeDpcRequest(arg1, arg2) sdv_IoInitializeDpcRequest(arg1, arg2);
VOID
sdv_IoInitializeDpcRequest(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIO_DPC_ROUTINE DpcRoutine
    );

#ifdef IoInitializeRemoveLock
#undef IoInitializeRemoveLock
#endif
#define IoInitializeRemoveLock(arg1, arg2, arg3, arg4) sdv_IoInitializeRemoveLock(arg1, arg2, arg3, arg4);
VOID
sdv_IoInitializeRemoveLock(
    IN PIO_REMOVE_LOCK Lock,
    IN ULONG AllocateTag,
    IN ULONG MaxLockedMinutes,
    IN ULONG HighWatermark
    );

#ifdef IoIsErrorUserInduced
#undef IoIsErrorUserInduced
#endif
#define IoIsErrorUserInduced sdv_IoIsErrorUserInduced
BOOLEAN
sdv_IoIsErrorUserInduced(
    IN NTSTATUS Status
    );

#ifdef IoMapTransfer
#undef IoMapTransfer
#endif
#define IoMapTransfer sdv_IoMapTransfer
PHYSICAL_ADDRESS
sdv_IoMapTransfer(
    IN PADAPTER_OBJECT AdapterObject,
    IN PMDL Mdl,
    IN PVOID MapRegisterBase,
    IN PVOID CurrentVa,
    IN OUT PULONG Length,
    IN BOOLEAN WriteToDevice
    );

#ifdef IoMarkIrpPending
#undef IoMarkIrpPending
#endif
#define IoMarkIrpPending(arg1) sdv_IoMarkIrpPending(arg1);
VOID
sdv_IoMarkIrpPending(
    IN OUT PIRP pirp
    );

#ifdef IoReleaseRemoveLock
#undef IoReleaseRemoveLock
#endif
#define IoReleaseRemoveLock(arg1, arg2) sdv_IoReleaseRemoveLock(arg1, arg2);
VOID
sdv_IoReleaseRemoveLock(
    IN PIO_REMOVE_LOCK RemoveLock,
    IN PVOID Tag
    );

#ifdef IoReleaseRemoveLockAndWait
#undef IoReleaseRemoveLockAndWait
#endif
#define IoReleaseRemoveLockAndWait(arg1, arg2) sdv_IoReleaseRemoveLockAndWait(arg1, arg2);
VOID
sdv_IoReleaseRemoveLockAndWait(
    IN PIO_REMOVE_LOCK RemoveLock,
    IN PVOID Tag
    );

#ifdef IoRequestDpc
#undef IoRequestDpc
#endif
#define IoRequestDpc(arg1, arg2, arg3) sdv_IoRequestDpc(arg1, arg2, arg3);
VOID
sdv_IoRequestDpc(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    );

#ifdef IoSetCancelRoutine
#undef IoSetCancelRoutine
#endif
#define IoSetCancelRoutine sdv_IoSetCancelRoutine
PDRIVER_CANCEL
sdv_IoSetCancelRoutine(
    IN PIRP pirp,
    IN PDRIVER_CANCEL CancelRoutine
    );

#ifdef IoSetCompletionRoutine
#undef IoSetCompletionRoutine
#endif
#define IoSetCompletionRoutine(arg1, arg2, arg3, arg4, arg5, arg6) sdv_IoSetCompletionRoutine(arg1, arg2, arg3, arg4, arg5, arg6);
VOID
sdv_IoSetCompletionRoutine(
    IN PIRP pirp,
    IN PIO_COMPLETION_ROUTINE CompletionRoutine,
    IN PVOID Context,
    IN BOOLEAN InvokeOnSuccess,
    IN BOOLEAN InvokeOnError,
    IN BOOLEAN InvokeOnCancel
    );

#ifdef IoSetCompletionRoutineEx
#undef IoSetCompletionRoutineEx
#endif
#define IoSetCompletionRoutineEx sdv_IoSetCompletionRoutineEx
NTSTATUS
sdv_IoSetCompletionRoutineEx(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PIO_COMPLETION_ROUTINE CompletionRoutine,
    IN PVOID Context,
    IN BOOLEAN InvokeOnSuccess,
    IN BOOLEAN InvokeOnError,
    IN BOOLEAN InvokeOnCancel
    );

#ifdef IoSetNextIrpStackLocation
#undef IoSetNextIrpStackLocation
#endif
#define IoSetNextIrpStackLocation(arg1) sdv_IoSetNextIrpStackLocation(arg1);
VOID
sdv_IoSetNextIrpStackLocation(
    IN OUT PIRP Irp
    );

#ifdef IoSizeOfIrp
#undef IoSizeOfIrp
#endif
#define IoSizeOfIrp sdv_IoSizeOfIrp
USHORT
sdv_IoSizeOfIrp(
    IN CCHAR StackSize
    );

#ifdef IoSizeofWorkItem
#undef IoSizeofWorkItem
#endif
#define IoSizeofWorkItem sdv_IoSizeofWorkItem
ULONG
sdv_IoSizeofWorkItem(
    VOID
    );

#ifdef IoSkipCurrentIrpStackLocation
#undef IoSkipCurrentIrpStackLocation
#endif
#define IoSkipCurrentIrpStackLocation(arg1) sdv_IoSkipCurrentIrpStackLocation(arg1);
VOID
sdv_IoSkipCurrentIrpStackLocation(
    IN PIRP pirp
    );

#ifdef IoWMIDeviceObjectToProviderId
#undef IoWMIDeviceObjectToProviderId
#endif
#define IoWMIDeviceObjectToProviderId sdv_IoWMIDeviceObjectToProviderId
ULONG
sdv_IoWMIDeviceObjectToProviderId(
    IN PDEVICE_OBJECT DeviceObject
    );



#ifdef KeEnterCriticalRegion
#undef KeEnterCriticalRegion
#endif
#define KeEnterCriticalRegion() sdv_KeEnterCriticalRegion();
NTKERNELAPI
VOID
sdv_KeEnterCriticalRegion(
    VOID
    );

#ifdef KeFlushIoBuffers
#undef KeFlushIoBuffers
#endif
#define KeFlushIoBuffers(arg1, arg2, arg3) sdv_KeFlushIoBuffers(arg1, arg2, arg3);
NTKERNELAPI
VOID
sdv_KeFlushIoBuffers(
    IN PMDL Mdl,
    IN BOOLEAN ReadOperation,
    IN BOOLEAN DmaOperation
    );

#ifdef KeLeaveCriticalRegion
#undef KeLeaveCriticalRegion
#endif
#define KeLeaveCriticalRegion() sdv_KeLeaveCriticalRegion();
NTKERNELAPI
VOID
sdv_KeLeaveCriticalRegion(
    VOID
    );

#ifdef KeRaiseIrql
#undef KeRaiseIrql
#endif
#define KeRaiseIrql(arg1, arg2) sdv_KeRaiseIrql(arg1, arg2);
VOID
sdv_KeRaiseIrql(
    IN KIRQL NewIrql,
    OUT PKIRQL OldIrql
    );
    
#ifdef KfRaiseIrql
#undef KfRaiseIrql
#endif
#define KfRaiseIrql(arg1, arg2) sdv_KeRaiseIrql(arg1, arg2);


#ifdef KeLowerIrql
#undef KeLowerIrql
#endif
#define KeLowerIrql(arg1) sdv_KeLowerIrql(arg1);
VOID
sdv_KeLowerIrql(
    IN KIRQL NewIrql
    );

#ifdef KfLowerIrql
#undef KfLowerIrql
#endif
#define KfLowerIrql(arg1) sdv_KeLowerIrql(arg1);

#ifdef KeGetCurrentIrql
#undef KeGetCurrentIrql
#endif
#define KeGetCurrentIrql sdv_KeGetCurrentIrql
NTHALAPI
KIRQL
NTAPI
sdv_KeGetCurrentIrql(
    VOID
    );

#ifdef KeWaitForMultipleObjects
#undef KeWaitForMultipleObjects
#endif
#define KeWaitForMultipleObjects sdv_KeWaitForMultipleObjects
NTSTATUS
sdv_KeWaitForMultipleObjects(
    IN ULONG Count,
    IN PVOID Object[],
    IN WAIT_TYPE WaitType,
    IN KWAIT_REASON WaitReason,
    IN KPROCESSOR_MODE WaitMode,
    IN BOOLEAN Alertable,
    IN PLARGE_INTEGER Timeout OPTIONAL,
    IN PKWAIT_BLOCK WaitBlockArray OPTIONAL
    );

#ifdef KeWaitForMutexObject
#undef KeWaitForMutexObject
#endif
#define KeWaitForMutexObject sdv_KeWaitForMutexObject
NTSTATUS
sdv_KeWaitForMutexObject(
    IN PRKMUTEX Mutex,
    IN KWAIT_REASON WaitReason,
    IN KPROCESSOR_MODE WaitMode,
    IN BOOLEAN Alertable,
    IN PLARGE_INTEGER Timeout OPTIONAL
    );

#ifdef KeWaitForSingleObject
#undef KeWaitForSingleObject
#endif
#define KeWaitForSingleObject sdv_KeWaitForSingleObject
NTSTATUS
sdv_KeWaitForSingleObject(
    IN PVOID Object,
    IN KWAIT_REASON WaitReason,
    IN KPROCESSOR_MODE WaitMode,
    IN BOOLEAN Alertable,
    IN PLARGE_INTEGER Timeout OPTIONAL
    );

#ifdef KeAcquireSpinLockAtDpcLevel
#undef KeAcquireSpinLockAtDpcLevel
#endif
#define KeAcquireSpinLockAtDpcLevel(arg1) sdv_KeAcquireSpinLockAtDpcLevel(arg1);
VOID
sdv_KeAcquireSpinLockAtDpcLevel(
    IN PKSPIN_LOCK SpinLock
    );


#ifdef KeReleaseSpinLockFromDpcLevel
#undef KeReleaseSpinLockFromDpcLevel
#endif
#define KeReleaseSpinLockFromDpcLevel(arg1) sdv_KeReleaseSpinLockFromDpcLevel(arg1);
VOID
sdv_KeReleaseSpinLockFromDpcLevel(
    IN PKSPIN_LOCK SpinLock
    );

#ifdef KeAcquireSpinLock
#undef KeAcquireSpinLock
#endif
#define KeAcquireSpinLock(arg1, arg2) sdv_KeAcquireSpinLock(arg1, arg2);
VOID
sdv_KeAcquireSpinLock(
    IN PKSPIN_LOCK SpinLock,
    OUT PKIRQL OldIrql
    );

#ifdef KeReleaseSpinLock
#undef KeReleaseSpinLock
#endif
#define KeReleaseSpinLock(arg1, arg2) sdv_KeReleaseSpinLock(arg1, arg2);
VOID
sdv_KeReleaseSpinLock(
    IN PKSPIN_LOCK SpinLock,
    IN KIRQL NewIrql
    );



#ifdef ExInitializeFastMutex
#undef ExInitializeFastMutex
#endif
#define ExInitializeFastMutex(arg1) sdv_ExInitializeFastMutex(arg1);
VOID
sdv_ExInitializeFastMutex(
    IN PFAST_MUTEX FastMutex
    );


#ifdef ExAcquireFastMutex
#undef ExAcquireFastMutex
#endif
#define ExAcquireFastMutex(arg1) sdv_ExAcquireFastMutex(arg1);
NTHALAPI
VOID
FASTCALL
sdv_ExAcquireFastMutex(
    IN PFAST_MUTEX FastMutex
    );

#ifdef ExReleaseFastMutex
#undef ExReleaseFastMutex
#endif
#define ExReleaseFastMutex(arg1) sdv_ExReleaseFastMutex(arg1);
NTHALAPI
VOID
FASTCALL
sdv_ExReleaseFastMutex(
    IN PFAST_MUTEX FastMutex
    );

#ifdef ExTryToAcquireFastMutex
#undef ExTryToAcquireFastMutex
#endif
#define ExTryToAcquireFastMutex sdv_ExTryToAcquireFastMutex
NTHALAPI
BOOLEAN
FASTCALL
sdv_ExTryToAcquireFastMutex(
    IN PFAST_MUTEX FastMutex
    );

#ifdef ExAllocatePool
#undef ExAllocatePool
#endif
#define ExAllocatePool sdv_ExAllocatePool
NTKERNELAPI
PVOID
sdv_ExAllocatePool(
    IN POOL_TYPE PoolType,
    IN SIZE_T NumberOfBytes
    );

#ifdef ExAllocatePoolWithTag
#undef ExAllocatePoolWithTag
#endif
#define ExAllocatePoolWithTag sdv_ExAllocatePoolWithTag
NTKERNELAPI
PVOID
sdv_ExAllocatePoolWithTag(
    IN POOL_TYPE PoolType,
    IN SIZE_T NumberOfBytes,
    IN ULONG Tag
    );

#ifdef ExFreePool
#undef ExFreePool
#endif
#define ExFreePool(arg1) sdv_ExFreePool(arg1);
NTKERNELAPI
VOID
NTAPI
sdv_ExFreePool(
    IN PVOID P
    );


#ifdef ExAllocatePoolWithQuotaTag
#undef ExAllocatePoolWithQuotaTag
#endif
#define ExAllocatePoolWithQuotaTag sdv_ExAllocatePoolWithQuotaTag
NTKERNELAPI
PVOID
sdv_ExAllocatePoolWithQuotaTag(
    IN POOL_TYPE PoolType,
    IN SIZE_T NumberOfBytes,
    IN ULONG Tag
    );

#ifdef ExAllocatePoolWithQuota
#undef ExAllocatePoolWithQuota
#endif
#define ExAllocatePoolWithQuota sdv_ExAllocatePoolWithQuota
NTKERNELAPI
PVOID
sdv_ExAllocatePoolWithQuota(
    IN POOL_TYPE PoolType,
    IN SIZE_T NumberOfBytes
    );



#ifdef ExInterlockedInsertHeadList
#undef ExInterlockedInsertHeadList
#endif
#define ExInterlockedInsertHeadList sdv_ExInterlockedInsertHeadList
NTKERNELAPI
PLIST_ENTRY
FASTCALL
sdv_ExInterlockedInsertHeadList(
    IN PLIST_ENTRY ListHead,
    IN PLIST_ENTRY ListEntry,
    IN PKSPIN_LOCK Lock
    );

#ifdef ExInterlockedInsertTailList
#undef ExInterlockedInsertTailList
#endif
#define ExInterlockedInsertTailList sdv_ExInterlockedInsertTailList
NTKERNELAPI
PLIST_ENTRY
FASTCALL
sdv_ExInterlockedInsertTailList(
    IN PLIST_ENTRY ListHead,
    IN PLIST_ENTRY ListEntry,
    IN PKSPIN_LOCK Lock
    );

#ifdef ExInterlockedPushEntryList
#undef ExInterlockedPushEntryList
#endif
#define ExInterlockedPushEntryList sdv_ExInterlockedPushEntryList
NTKERNELAPI
PSINGLE_LIST_ENTRY
FASTCALL
sdv_ExInterlockedPushEntryList(
    IN PSINGLE_LIST_ENTRY ListHead,
    IN PSINGLE_LIST_ENTRY ListEntry,
    IN PKSPIN_LOCK Lock
    );


#ifdef RtlMoveMemory
#undef RtlMoveMemory
#endif
#define RtlMoveMemory(arg1, arg2, arg3) sdv_RtlMoveMemory(arg1, arg2, arg3);
NTSYSAPI
VOID
NTAPI
sdv_RtlMoveMemory(
    VOID UNALIGNED *Destination,
    CONST VOID UNALIGNED *Source,
    SIZE_T Length
    );

#ifdef RtlZeroMemory
#undef RtlZeroMemory
#endif
#define RtlZeroMemory(arg1, arg2) sdv_RtlZeroMemory(arg1, arg2);
NTSYSAPI
VOID
NTAPI
sdv_RtlZeroMemory(
    VOID UNALIGNED *Destination,
    SIZE_T Length
    );



#ifdef MmGetMdlByteCount
#undef MmGetMdlByteCount
#endif
#define MmGetMdlByteCount sdv_MmGetMdlByteCount
ULONG
sdv_MmGetMdlByteCount(
    IN PMDL Mdl
    );

#ifdef MmGetMdlByteOffset
#undef MmGetMdlByteOffset
#endif
#define MmGetMdlByteOffset sdv_MmGetMdlByteOffset
ULONG
sdv_MmGetMdlByteOffset(
    IN PMDL Mdl
    );

#ifdef MmGetMdlPfnArray
#undef MmGetMdlPfnArray
#endif
#define MmGetMdlPfnArray sdv_MmGetMdlPfnArray
PPFN_NUMBER
sdv_MmGetMdlPfnArray(
    IN PMDL Mdl
    );

#ifdef MmGetMdlVirtualAddress
#undef MmGetMdlVirtualAddress
#endif
#define MmGetMdlVirtualAddress sdv_MmGetMdlVirtualAddress
PVOID
sdv_MmGetMdlVirtualAddress(
    IN PMDL Mdl
    );

#ifdef MmGetSystemAddressForMdlSafe
#undef MmGetSystemAddressForMdlSafe
#endif
#define MmGetSystemAddressForMdlSafe sdv_MmGetSystemAddressForMdlSafe
PVOID
sdv_MmGetSystemAddressForMdlSafe(
    IN PMDL MDL,
    IN MM_PAGE_PRIORITY PRIORITY
    );

#ifdef MmLockPagableCodeSection
#undef MmLockPagableCodeSection
#endif
#define MmLockPagableCodeSection sdv_MmLockPagableCodeSection
PVOID
sdv_MmLockPagableCodeSection(
    IN PVOID AddressWithinSection
    );

#ifdef MmPrepareMdlForReuse
#undef MmPrepareMdlForReuse
#endif
#define MmPrepareMdlForReuse(arg1) sdv_MmPrepareMdlForReuse(arg1)
VOID
sdv_MmPrepareMdlForReuse(
    IN PMDL Mdl
    );


#ifdef ObReferenceObject
#undef ObReferenceObject
#endif
#define ObReferenceObject sdv_ObReferenceObject
LONG_PTR
sdv_ObReferenceObject(
    IN PVOID Object
    );

#ifdef ObDereferenceObject
#undef ObDereferenceObject
#endif
#define ObDereferenceObject sdv_ObDereferenceObject
LONG_PTR
sdv_ObDereferenceObject(
    IN PVOID Object
    );

#ifdef InitializeObjectAttributes
#undef InitializeObjectAttributes
#endif
#define InitializeObjectAttributes(arg1, arg2, arg3, arg4, arg5) sdv_InitializeObjectAttributes(arg1, arg2, arg3, arg4, arg5);
VOID
sdv_InitializeObjectAttributes(
    OUT POBJECT_ATTRIBUTES p,
    IN PUNICODE_STRING n,
    IN ULONG a,
    IN HANDLE r,
    IN PSECURITY_DESCRIPTOR s
    );


#ifdef PoCallDriver
#undef PoCallDriver
#endif
#define PoCallDriver sdv_PoCallDriver
NTSTATUS
sdv_PoCallDriver(
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PIRP Irp
    );




void sdv_do_paged_code_check();

#undef PAGED_CODE
#define PAGED_CODE() sdv_do_paged_code_check();


void sdv_do_assert_check(int i);

#undef ASSERT
#if DBG
#define ASSERT(e) sdv_do_assert_check(e);
#else
#define ASSERT(e) ((void) 0)
#endif

/* 
Turn the generic __analysis_assume into a call to SdvAssumeSoft.

For more information about __analysis_assume see: MSDN Library >
Development Tools and Languages > Visual Studio Team System > Team
Edition for Developers > Writing Quality Code > Detecting and
Correcting C/C++ Code Defects > How to: Specify Additional Code
Information.

*/
VOID SdvAssumeSoft(int e);
#ifdef __analysis_assume
#undef __analysis_assume
#endif
#define __analysis_assume(e) SdvAssumeSoft(e);

#define SDV_MACRO_PASS

#undef CONTAINING_RECORD
#define CONTAINING_RECORD(x,y,z) ((y*)sdv_containing_record((PCHAR)x))

PCHAR sdv_containing_record(
    PCHAR Address
    );

#ifdef InterlockedIncrement
#undef InterlockedIncrement
#endif
#define InterlockedIncrement sdv_InterlockedIncrement
NTKERNELAPI
LONG
FASTCALL
sdv_InterlockedIncrement(
    IN LONG *Addend
    );

#ifdef InterlockedDecrement
#undef InterlockedDecrement
#endif
#define InterlockedDecrement sdv_InterlockedDecrement
NTKERNELAPI
LONG
FASTCALL
sdv_InterlockedDecrement(
    IN LONG *Addend
    );


#ifdef KeTryToAcquireSpinLockAtDpcLevel
#undef KeTryToAcquireSpinLockAtDpcLevel
#endif
#define KeTryToAcquireSpinLockAtDpcLevel(arg1) sdv_KeTryToAcquireSpinLockAtDpcLevel(arg1)
BOOLEAN  
sdv_KeTryToAcquireSpinLockAtDpcLevel(
    IN PKSPIN_LOCK  SpinLock    
    );
    
#ifdef ExReleaseResourceLite
#undef ExReleaseResourceLite
#endif
#define ExReleaseResourceLite(arg1) sdv_ExReleaseResourceLite(arg1)
NTKERNELAPI
VOID
FASTCALL
ExReleaseResourceLite(
    IN PERESOURCE Resource    
    );

#endif

#endif
