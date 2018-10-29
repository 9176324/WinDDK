
/*++

Copyright (C) 2004  Microsoft Corporation

Module Name:

    prototypes.h

Abstract:

    Contains function prototypes for all the functions defined
    by Microsoft Device Specific Module (DSM).

Environment:

    kernel mode only

Notes:

--*/

#pragma warning (disable:4214) // bit field usage
#pragma warning (disable:4200) // zero-sized array

#ifndef _PROTOTYPES_H_
#define _PROTOTYPES_H_

#define DSM_VENDOR_ID_LEN       8
#define DSM_PRODUCT_ID_LEN      16
#define DSM_VENDPROD_ID_LEN     24

//
// In accordance with SPC-3 specs
//
#define SPC3_TARGET_PORT_GROUPS_HEADER_SIZE         4

typedef struct _SPC3_CDB_REPORT_TARGET_PORT_GROUPS {
    UCHAR OperationCode;
    UCHAR ServiceAction : 5;
    UCHAR Reserved1 : 3;
    UCHAR Reserved2[4];
    UCHAR AllocationLength[4];
    UCHAR Reserved3;
    UCHAR Control;
} SPC3_CDB_REPORT_TARGET_PORT_GROUPS, *PSPC3_CDB_REPORT_TARGET_PORT_GROUPS;

typedef struct _SPC3_REPORT_TARGET_PORT_GROUP_DESCRIPTOR {
    UCHAR AsymmetricAccessState : 4;
    UCHAR Reserved : 3;
    UCHAR Preferred : 1;
    UCHAR ActiveOptimizedSupported : 1;
    UCHAR ActiveUnoptimizedSupported : 1;
    UCHAR StandbySupported : 1;
    UCHAR UnavailableSupported : 1;
    UCHAR Reserved2 : 3;
    UCHAR TransitioningSupported : 1;
    USHORT TPG_Identifier;
    UCHAR Reserved3;
    UCHAR StatusCode;
    UCHAR VendorUnique;
    UCHAR NumberTargetPorts;
    ULONG TargetPortIds[0];
} SPC3_REPORT_TARGET_PORT_GROUP_DESCRIPTOR, *PSPC3_REPORT_TARGET_PORT_GROUP_DESCRIPTOR;

typedef struct _SPC3_CDB_SET_TARGET_PORT_GROUPS {
    UCHAR OperationCode;
    UCHAR ServiceAction : 5;
    UCHAR Reserved1 : 3;
    UCHAR Reserved2[4];
    UCHAR ParameterListLength[4];
    UCHAR Reserved3;
    UCHAR Control;
} SPC3_CDB_SET_TARGET_PORT_GROUPS, *PSPC3_CDB_SET_TARGET_PORT_GROUPS;

typedef struct _SPC3_SET_TARGET_PORT_GROUP_DESCRIPTOR {
    UCHAR AsymmetricAccessState : 4;
    UCHAR Reserved1 : 4;
    UCHAR Reserved2;
    USHORT TPG_Identifier;
} SPC3_SET_TARGET_PORT_GROUP_DESCRIPTOR, *PSPC3_SET_TARGET_PORT_GROUP_DESCRIPTOR;

typedef struct _SPC3_CONTROL_EXTENSION_MODE_PAGE {
    UCHAR PageCode : 6;
    UCHAR SubpageFormat : 1;
    UCHAR ParametersSavable : 1;
    UCHAR SubpageCode;
    UCHAR PageLength[2];
    UCHAR ImplicitALUAEnable : 1;
    UCHAR ScsiPrecendence : 1;
    UCHAR TimestampChangeable : 1;
    UCHAR Reserved1 : 5;
    UCHAR InitialPriority : 4;
    UCHAR Reserved2 : 4;
    UCHAR Reserved3[26];
} SPC3_CONTROL_EXTENSION_MODE_PAGE, *PSPC3_CONTROL_EXTENSION_MODE_PAGE;

#define SPC3_SCSIOP_REPORT_TARGET_PORT_GROUPS       0xA3
#define SPC3_SCSIOP_SET_TARGET_PORT_GROUPS          0xA4
#define SPC3_SERVICE_ACTION_TARGET_PORT_GROUPS      0xA
#define SPC3_RESERVATION_ACTION_REPORT_CAPABILITIES 0x2

#define SPC3_SCSI_ADSENSE_COMMANDS_CLEARED_BY_ANOTHER_INITIATOR 0x2F
#define SPC3_SCSI_ADSENSE_LOGICAL_UNIT_COMMAND_FAILED       0x67

#define SPC3_SCSI_SENSEQ_MODE_PARAMETERS_CHANGED            0x1
#define SPC3_SCSI_SENSEQ_RESERVATIONS_PREEMPTED             0x3
#define SPC3_SCSI_SENSEQ_RESERVATIONS_RELEASED              0x4
#define SPC3_SCSI_SENSEQ_REGISTRATIONS_PREEMPTED            0x5
#define SPC3_SCSI_SENSEQ_ASYMMETRIC_ACCESS_STATE_CHANGED    0x6
#define SPC3_SCSI_SENSEQ_IMPLICIT_ASYMMETRIC_ACCESS_STATE_TRANSITION_FAILED 0x7
#define SPC3_SCSI_SENSEQ_CAPACITY_DATA_HAS_CHANGED          0x9
#define SPC3_SCSI_SENSEQ_ASYMMETRIC_ACCESS_STATE_TRANSITION 0xA
#define SPC3_SCSI_SENSEQ_TARGET_PORT_IN_STANDBY_STATE       0xB
#define SPC3_SCSI_SENSEQ_TARGET_PORT_IN_UNAVAILABLE_STATE   0xC

#define SPC3_SCSI_SENSEQ_SET_TARGET_PORT_GROUPS_FAILED      0xA

#define SPC3_SET_TARGET_PORT_GROUPS_TIMEOUT     4
#define SPC3_REPORT_TARGET_PORT_GROUPS_TIMEOUT  4


//
// Function prototypes for functions intrface.c
//

DRIVER_INITIALIZE DriverEntry;
DRIVER_UNLOAD DsmDriverUnload;

NTSTATUS
DsmInquire (
    __in IN PVOID DsmContext,
    __in IN PDEVICE_OBJECT TargetDevice,
    __in IN PDEVICE_OBJECT PortObject,
    __in IN PSTORAGE_DEVICE_DESCRIPTOR Descriptor,
    __in IN PSTORAGE_DEVICE_ID_DESCRIPTOR DeviceIdList,
    __out OUT PVOID *DsmIdentifier
    );

BOOLEAN
DsmCompareDevices(
    __in IN PVOID DsmContext,
    __in IN PVOID DsmId1,
    __in IN PVOID DsmId2
    );

NTSTATUS
DsmGetControllerInfo(
    __in IN PVOID DsmContext,
    __in IN PVOID DsmId,
    __in IN ULONG Flags,
    __inout IN OUT PCONTROLLER_INFO *ControllerInfo
    );

NTSTATUS
DsmSetDeviceInfo(
    __in IN PVOID DsmContext,
    __in IN PDEVICE_OBJECT TargetObject,
    __in IN PVOID DsmId,
    __inout IN OUT PVOID *PathId
    );

BOOLEAN
DsmIsPathActive(
    __in IN PVOID DsmContext,
    __in IN PVOID PathId,
    __in IN PVOID DsmId
    );

NTSTATUS
DsmPathVerify(
    __in IN PVOID DsmContext,
    __in IN PVOID DsmId,
    __in IN PVOID PathId
    );

NTSTATUS
DsmInvalidatePath(
    __in IN PVOID DsmContext,
    __in IN ULONG ErrorMask,
    __in IN PVOID PathId,
    __inout IN OUT PVOID *NewPathId
    );

NTSTATUS
DsmMoveDevice(
    __in IN PVOID DsmContext,
    __in IN PDSM_IDS DsmIds,
    __in IN PVOID MPIOPath,
    __in IN PVOID SuggestedPath,
    __in IN ULONG Flags
    );

NTSTATUS
DsmRemovePending(
    __in IN PVOID DsmContext,
    __in IN PVOID DsmId
    );

NTSTATUS
DsmRemoveDevice(
    __in IN PVOID DsmContext,
    __in IN PVOID DsmId,
    __in IN PVOID PathId
    );

NTSTATUS
DsmRemovePath(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PVOID PathId
    );

NTSTATUS
DsmSrbDeviceControl(
    __in IN PVOID DsmContext,
    __in IN PDSM_IDS DsmIds,
    __in IN PIRP Irp,
    __in IN PSCSI_REQUEST_BLOCK Srb,
    __in IN PKEVENT Event
    );

PVOID
DsmLBGetPath(
    __in IN PVOID DsmContext,
    __in IN PSCSI_REQUEST_BLOCK Srb,
    __in IN PDSM_IDS DsmList,
    __in IN PVOID CurrentPath,
    __out OUT NTSTATUS *Status
    );

ULONG
DsmInterpretError(
    __in IN PVOID DsmContext,
    __in IN PVOID DsmId,
    __in IN PSCSI_REQUEST_BLOCK Srb,
    __inout IN OUT NTSTATUS *Status,
    __out OUT PBOOLEAN Retry,
    __out OUT PLONG RetryInterval,
    ...
    );

NTSTATUS
DsmUnload(
    __in IN PVOID DsmContext
    );

VOID
DsmSetCompletion(
    __in IN PVOID DsmContext,
    __in IN PVOID DsmId,
    __in IN PIRP Irp,
    __in IN PSCSI_REQUEST_BLOCK Srb,
    __inout IN OUT PDSM_COMPLETION_INFO DsmCompletion
    );

ULONG
DsmCategorizeRequest(
    __in IN PVOID DsmContext,
    __in IN PDSM_IDS DsmIds,
    __in IN PIRP Irp,
    __in IN PSCSI_REQUEST_BLOCK Srb,
    __in IN PVOID CurrentPath,
    __deref_out_opt OUT PVOID *PathId,
    __out OUT NTSTATUS *Status
    );

NTSTATUS
DsmBroadcastRequest(
    __in IN PVOID DsmContext,
    __in IN PDSM_IDS DsmIds,
    __in IN PIRP Irp,
    __in IN PSCSI_REQUEST_BLOCK Srb,
    __in IN PKEVENT Event
    );


//
// Function prototypes for functions in dsmmain.c
//

VOID
DsmpFreeDSMResources(
    __in IN PDSM_CONTEXT DsmContext
    );

PDSM_GROUP_ENTRY
DsmpFindDevice(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PDSM_DEVICE_INFO DeviceInfo,
    __in IN BOOLEAN AcquireDSMLock
    );

PDSM_GROUP_ENTRY
DsmpBuildGroupEntry(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PDSM_DEVICE_INFO DeviceInfo
    );

NTSTATUS
DsmpParseTargetPortGroupsInformation(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PDSM_GROUP_ENTRY Group,
    __in_bcount(TargetPortGroupsInfoLength) IN PUCHAR TargetPortGroupsInfo,
    __in IN ULONG TargetPortGroupsInfoLength
    );

PDSM_TARGET_PORT_GROUP_ENTRY
DsmpFindTargetPortGroupEntry(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PDSM_GROUP_ENTRY Group,
    __in_bcount(TPGs_BufferLength) IN PUCHAR TargetPortGroupsDescriptor,
    __in IN ULONG TPGs_BufferLength
    );

PDSM_TARGET_PORT_GROUP_ENTRY
DsmpUpdateTargetPortGroupEntry(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PDSM_TARGET_PORT_GROUP_ENTRY TargetPortGroup,
    __in_bcount(TPGs_BufferLength) IN PUCHAR TargetPortGroupsDescriptor,
    __in IN ULONG TPGs_BufferLength,
    __out OUT PULONG DescriptorSize
    );

PDSM_TARGET_PORT_GROUP_ENTRY
DsmpBuildTargetPortGroupEntry(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PDSM_GROUP_ENTRY Group,
    __in_bcount(TPGs_BufferLength) IN PUCHAR TargetPortGroupsDescriptor,
    __in IN ULONG TPGs_BufferLength,
    __out OUT PULONG DescriptorSize
    );

PDSM_TARGET_PORT_LIST_ENTRY
DsmpFindTargetPortListEntry(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PDSM_TARGET_PORT_GROUP_ENTRY TargetPortGroup,
    __in IN ULONG RelativeTargetPortId
    );

PDSM_TARGET_PORT_LIST_ENTRY
DsmpBuildTargetPortListEntry(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PDSM_TARGET_PORT_GROUP_ENTRY TargetPortGroup,
    __in IN ULONG RelativeTargetPortId
    );

PDSM_TARGET_PORT_GROUP_ENTRY
DsmpFindTargetPortGroup(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PDSM_GROUP_ENTRY Group,
    __in IN PUSHORT TargetPortGroupId
    );

PDSM_TARGET_PORT_LIST_ENTRY
DsmpFindTargetPort(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PDSM_TARGET_PORT_GROUP_ENTRY TargetPortGroup,
    __in IN PULONG TargetPortGroupId
    );

NTSTATUS
DsmpAddDeviceEntry(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PDSM_GROUP_ENTRY Group,
    __in IN PDSM_DEVICE_INFO DeviceInfo
    );

PDSM_CONTROLLER_LIST_ENTRY
DsmpFindControllerEntry(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PDEVICE_OBJECT PortObject,
    __in IN PSCSI_ADDRESS ScsiAddress,
    __in_ecount(ControllerSerialNumberLength) IN PSTR ControllerSerialNumber,
    __in IN SIZE_T ControllerSerialNumberLength,
    __in IN STORAGE_IDENTIFIER_CODE_SET CodeSet,
    __in IN BOOLEAN AcquireLock
    );

__maybenull __checkReturn
__drv_when(return!=0, __drv_allocatesMem(Mem))
PDSM_CONTROLLER_LIST_ENTRY
DsmpBuildControllerEntry(
    __in IN PDSM_CONTEXT DsmContext,
    __in_opt IN PDEVICE_OBJECT DeviceObject,
    __in IN PDEVICE_OBJECT PortObject,
    __in IN PSCSI_ADDRESS ScsiAddress,
    __in IN PSTR ControllerSerialNumber,
    __in IN STORAGE_IDENTIFIER_CODE_SET CodeSet,
    __in IN BOOLEAN AcquireLock
    );

VOID
DsmpFreeControllerEntry(
    __in IN PDSM_CONTEXT DsmContext,
    __in __drv_freesMem(Mem) IN PDSM_CONTROLLER_LIST_ENTRY ControllerEntry
    );

BOOLEAN
DsmpIsDeviceBelongsToController(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PDSM_DEVICE_INFO DeviceInfo,
    __in IN PDSM_CONTROLLER_LIST_ENTRY ControllerEntry
    );

PDSM_DEVICE_INFO
DsmpFindDevInfoFromGroupAndFOGroup(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PDSM_GROUP_ENTRY Group,
    __in IN PDSM_FAILOVER_GROUP FOGroup
    );

PDSM_FAILOVER_GROUP
DsmpFindFOGroup(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PVOID PathId
    );

PDSM_FAILOVER_GROUP
DsmpBuildFOGroup(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PDSM_DEVICE_INFO DeviceInfo,
    __in IN PVOID        *PathId
    );

NTSTATUS
DsmpUpdateFOGroup(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PDSM_FAILOVER_GROUP FailGroup,
    __in IN PDSM_DEVICE_INFO DeviceInfo
    );

VOID
DsmpRemoveDeviceFailGroup(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PDSM_FAILOVER_GROUP FailGroup,
    __in IN PDSM_DEVICE_INFO DeviceInfo,
    __in IN BOOLEAN AcquireDSMLock
    );

ULONG
DsmpRemoveDeviceEntry(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PDSM_GROUP_ENTRY Group,
    __in IN PDSM_DEVICE_INFO DeviceInfo
    );

PDSM_FAILOVER_GROUP
DsmpSetNewPath(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PDSM_DEVICE_INFO FailingDevice
    );

NTSTATUS
DsmpUpdateTargetPortGroupDevicesStates(
    __in IN PDSM_TARGET_PORT_GROUP_ENTRY TargetPortGroup,
    __in IN DSM_DEVICE_STATE NewState
    );

PDSM_FAILOVER_GROUP
DsmpGetPath(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PDSM_IDS DsmList,
    __in IN PSCSI_REQUEST_BLOCK Srb
    );

PVOID
DsmpGetPathIdFromPassThroughPath(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PDSM_IDS DsmList,
    __in IN PIRP Irp,
    __inout IN OUT NTSTATUS *Status
    );

VOID
DsmpRemoveGroupEntry(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PDSM_GROUP_ENTRY GroupEntry,
    __in IN BOOLEAN AcquireDSMLock
    );

BOOLEAN
DsmpMpioPassThroughPathCommand(
    __in IN PIRP Irp
    );

BOOLEAN
DsmpReservationCommand(
    __in IN PIRP Irp,
    __in IN PSCSI_REQUEST_BLOCK Srb
    );

VOID
DsmpRequestComplete(
    __in IN PVOID DsmId,
    __in IN PIRP Irp,
    __in IN PSCSI_REQUEST_BLOCK Srb,
    __in IN PVOID DsmContext
    );

NTSTATUS
DsmpRegisterPersistentReservationKeys(
    __in IN PDSM_DEVICE_INFO DeviceInfo,
    __in IN BOOLEAN      Register
    );

BOOLEAN
DsmpShouldRetryPassThroughRequest(
    __in IN PSENSE_DATA SenseData,
    __in IN ULONG   SenseDataSize
    );

BOOLEAN
DsmpShouldRetryPersistentReserveCommand(
    __in IN PSENSE_DATA SenseData,
    __in IN ULONG   SenseDataSize
    );

BOOLEAN
DsmpShouldRetryTPGRequest(
    __in IN PSENSE_DATA SenseData,
    __in IN ULONG   SenseDataSize
    );

PDSM_DEVICE_INFO
DsmpGetActivePathToBeUsed(
    __in PDSM_GROUP_ENTRY Group
    );

PDSM_DEVICE_INFO
DsmpGetAnyActivePath(
    __in PDSM_GROUP_ENTRY Group,
    __in BOOLEAN Exception,
    __in_opt PDSM_DEVICE_INFO DeviceInfo
    );

PDSM_DEVICE_INFO
DsmpFindStandbyPathToActivate(
    __in IN PDSM_GROUP_ENTRY Group
    );

PDSM_DEVICE_INFO
DsmpFindStandbyPathToActivateALUA(
    __in IN PDSM_GROUP_ENTRY Group,
    __in IN PBOOLEAN SendTPG
    );

PDSM_DEVICE_INFO
DsmpFindStandbyPathInAlternateTpgALUA(
    __in IN PDSM_GROUP_ENTRY Group,
    __in IN PDSM_DEVICE_INFO DeviceInfo
    );

NTSTATUS
DsmpSetLBForDsmPolicyAdjustment(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN DSM_LOAD_BALANCE_TYPE LoadBalanceType,
    __in IN ULONGLONG PreferredPath
    );

NTSTATUS
DsmpSetLBForVidPidPolicyAdjustment(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PWSTR TargetHardwareId,
    __in IN DSM_LOAD_BALANCE_TYPE LoadBalanceType,
    __in IN ULONGLONG PreferredPath
    );

NTSTATUS
DsmpSetNewDefaultLBPolicy(
    __in IN PDSM_CONTEXT DsmContext,
    __in_opt IN PDSM_DEVICE_INFO NewDeviceInfo,
    __in IN DSM_LOAD_BALANCE_TYPE LoadBalanceType
    );

NTSTATUS
DsmpSetLBForPathArrival(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PDSM_DEVICE_INFO NewDeviceInfo
    );

NTSTATUS
DsmpSetLBForPathArrivalALUA(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PDSM_DEVICE_INFO NewDeviceInfo
    );

NTSTATUS
DsmpSetLBForPathRemoval(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PDSM_DEVICE_INFO RemovedDeviceInfo
    );

NTSTATUS
DsmpSetLBForPathRemovalALUA(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PDSM_DEVICE_INFO RemovedDeviceInfo
    );

NTSTATUS
DsmpSetLBForPathFailing(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PDSM_DEVICE_INFO FailingDeviceInfo
    );

NTSTATUS
DsmpSetLBForPathFailingALUA(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PDSM_DEVICE_INFO FailingDeviceInfo
    );

NTSTATUS
DsmpSetPathForIoRetryALUA(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PDSM_DEVICE_INFO FailingDeviceInfo,
    __in IN BOOLEAN TPGException,
    __in IN BOOLEAN DeviceInfoException
    );

PDSM_FAIL_PATH_PROCESSING_LIST_ENTRY
DsmpFindFailPathDevInfoEntry(
    __in IN PDSM_CONTEXT Context,
    __in IN PDSM_GROUP_ENTRY Group,
    __in IN PDSM_DEVICE_INFO FailingDevInfo
    );

PDSM_FAIL_PATH_PROCESSING_LIST_ENTRY
DsmpBuildFailPathDevInfoEntry(
    __in IN PDSM_CONTEXT Context,
    __in IN PDSM_GROUP_ENTRY Group,
    __in IN PDSM_DEVICE_INFO FailingDevInfo,
    __in IN PDSM_DEVICE_INFO AlternateDevInfo
    );

IO_COMPLETION_ROUTINE DsmpPhase1ProcessPathFailingALUA;

NTSTATUS
DsmpRemoveFailPathDevInfoEntry(
    __in IN PDSM_CONTEXT Context,
    __in IN PDSM_GROUP_ENTRY Group,
    __in IN PDSM_FAIL_PATH_PROCESSING_LIST_ENTRY FailPathDevInfoEntry
    );

IO_COMPLETION_ROUTINE DsmpPhase2ProcessPathFailingALUA;

NTSTATUS
DsmpPersistentReserveOut(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PDSM_IDS DsmIds,
    __in IN PIRP Irp,
    __in IN PSCSI_REQUEST_BLOCK Srb,
    __in IN PKEVENT Event
    );

NTSTATUS
DsmpPersistentReserveIn(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PDSM_IDS DsmIds,
    __in IN PIRP Irp,
    __in IN PSCSI_REQUEST_BLOCK Srb,
    __in IN PKEVENT Event
    );

IO_COMPLETION_ROUTINE DsmpPersistentReserveCompletion;


//
// Function prototypes for functions in utils.c
//

__drv_allocatesMem(Mem)
__drv_when(((PoolType&0x1))!=0, __drv_maxIRQL(APC_LEVEL))
__drv_when(((PoolType&0x1))==0, __drv_maxIRQL(DISPATCH_LEVEL))
__drv_when(((PoolType&0x2))!=0,
    __drv_reportError("Must succeed pool allocations are forbidden. "
    "Allocation failures cause a system crash"))
__drv_when(((PoolType&(0x2|POOL_RAISE_IF_ALLOCATION_FAILURE)))==0,
    __post __maybenull __checkReturn)
__drv_when(((PoolType&(0x2|POOL_RAISE_IF_ALLOCATION_FAILURE)))!=0,
    __post __notnull)
__bcount(NumberOfBytes)
PVOID
DsmpAllocatePool(
    __in __drv_strictTypeMatch(__drv_typeExpr) IN POOL_TYPE PoolType,
    __in IN SIZE_T NumberOfBytes,
    __in IN ULONG Tag
    );

__maybenull __checkReturn
__drv_when(return!=0, __drv_allocatesMem(Mem))
__drv_when(((PoolType&0x1))!=0, __drv_maxIRQL(APC_LEVEL))
__drv_when(((PoolType&0x1))==0, __drv_maxIRQL(DISPATCH_LEVEL))
__drv_when(return!=0, __bcount(*BytesAllocated))
PVOID
DsmpAllocateAlignedPool(
    __in IN POOL_TYPE PoolType,
    __in IN SIZE_T NumberOfBytes,
    __in IN ULONG AlignmentMask,
    __in IN ULONG Tag,
    __out OUT SIZE_T *BytesAllocated
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
DsmpFreePool(
    __in_opt __drv_freesMem(Mem) IN PVOID Block
    );

NTSTATUS
DsmpGetStatsGatheringChoice(
    __in IN PDSM_CONTEXT Context,
    __out OUT PULONG StatsGatherChoice
    );

NTSTATUS
DsmpSetStatsGatheringChoice(
    __in IN PDSM_CONTEXT Context,
    __in IN ULONG StatsGatherChoice
    );

NTSTATUS
DsmpGetDeviceList(
    __in IN PDSM_CONTEXT Context
    );

NTSTATUS
DsmpGetStandardInquiryData(
    __in IN PDEVICE_OBJECT DeviceObject,
    __out OUT PINQUIRYDATA InquiryData
    );

BOOLEAN
DsmpCheckScsiCompliance(
    __in IN PDEVICE_OBJECT DeviceObject,
    __in IN PINQUIRYDATA InquiryData,
    __in IN PSTORAGE_DEVICE_DESCRIPTOR Descriptor,
    __in IN PSTORAGE_DEVICE_ID_DESCRIPTOR DeviceIdList
    );

BOOLEAN
DsmpDeviceSupported(
    __in IN PDSM_CONTEXT Context,
    __in IN PCSTR VendorId,
    __in IN PCSTR ProductId
    );

BOOLEAN
DsmpFindSupportedDevice(
    __in IN PUNICODE_STRING DeviceName,
    __in IN PUNICODE_STRING SupportedDevices
    );

PVOID
DsmpParseDeviceID (
    __in IN PSTORAGE_DEVICE_ID_DESCRIPTOR DeviceID,
    __in IN DSM_DEVID_TYPE DeviceIdType,
    __in_opt IN PULONG IdNumber,
    __out_opt OUT PSTORAGE_IDENTIFIER_CODE_SET CodeSet
    );

PUCHAR
DsmpBinaryToAscii(
    __in_ecount(Length) IN PUCHAR HexBuffer,
    __in IN ULONG Length,
    __inout IN OUT PULONG UpdateLength
    );

PSTR
DsmpGetSerialNumber(
    __in IN PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
DsmpDisableImplicitStateTransition(
    __in IN PDEVICE_OBJECT DeviceObject,
    __out OUT PBOOLEAN DisableImplicit
    );

PWSTR
DsmpBuildHardwareId(
    __in IN PDSM_DEVICE_INFO DeviceInfo
    );

PWSTR
DsmpBuildDeviceNameLegacy(
    __in IN PDSM_DEVICE_INFO DeviceInfo
    );

PWSTR
DsmpBuildDeviceName(
    __in IN PDSM_DEVICE_INFO DeviceInfo,
    __in_ecount(SerialNumberLength) IN PSTR SerialNumber,
    __in IN SIZE_T SerialNumberLength
    );

NTSTATUS
DsmpApplyDeviceNameCorrection(
    __in IN PDSM_DEVICE_INFO DeviceInfo,
    __in_ecount(DeviceNameLegacyLen) PWSTR DeviceNameLegacy,
    __in IN SIZE_T DeviceNameLegacyLen,
    __in_ecount(DeviceNameLen) PWSTR DeviceName,
    __in IN SIZE_T DeviceNameLen
    );

NTSTATUS
DsmpQueryDeviceLBPolicyFromRegistry(
    __in IN PDSM_DEVICE_INFO DeviceInfo,
    __in IN PWSTR RegistryKeyName,
    __out OUT PDSM_LOAD_BALANCE_TYPE LoadBalanceType,
    __out OUT PULONGLONG PreferredPath,
    __out OUT PUCHAR ExplicitlySet
    );

NTSTATUS
DsmpQueryTargetLBPolicyFromRegistry(
    __in IN PDSM_DEVICE_INFO DeviceInfo,
    __out OUT PDSM_LOAD_BALANCE_TYPE LoadBalanceType,
    __out OUT PULONGLONG PreferredPath
    );

NTSTATUS
DsmpQueryDsmLBPolicyFromRegistry(
    __out OUT PDSM_LOAD_BALANCE_TYPE LoadBalanceType,
    __out OUT PULONGLONG PreferredPath
    );

NTSTATUS
DsmpSetDsmLBPolicyInRegistry(
    __in IN DSM_LOAD_BALANCE_TYPE LoadBalanceType,
    __in IN ULONGLONG PreferredPath
    );

NTSTATUS
DsmpSetVidPidLBPolicyInRegistry(
    __in IN PWSTR TargetHardwareId,
    __in IN DSM_LOAD_BALANCE_TYPE LoadBalanceType,
    __in IN ULONGLONG PreferredPath
    );

NTSTATUS
DsmpOpenLoadBalanceSettingsKey(
    __in IN ACCESS_MASK AccessMask,
    __out OUT PHANDLE LoadBalanceSettingsKey
    );

NTSTATUS
DsmpOpenTargetsLoadBalanceSettingKey(
    __in IN ACCESS_MASK AccessMask,
    __out OUT PHANDLE TargetsLoadBalanceSettingKey
    );

NTSTATUS
DsmpOpenDsmServicesParametersKey(
    __in IN ACCESS_MASK AccessMask,
    __out OUT PHANDLE ParametersSettingsKey
    );

NTSTATUS
DsmpReportTargetPortGroups(
    __in IN PDEVICE_OBJECT DeviceObject,
    __out OUT PUCHAR *TargetPortGroupsInfo,
    __out OUT PULONG TargetPortGroupsInfoLength
    );

NTSTATUS
DsmpReportTargetPortGroupsAsync(
    __in IN PDSM_DEVICE_INFO DeviceInfo,
    __in IN PIO_COMPLETION_ROUTINE CompletionRoutine,
    __inout __drv_aliasesMem IN PDSM_TPG_COMPLETION_CONTEXT CompletionContext,
    __in IN ULONG TargetPortGroupsInfoLength,
    __inout __drv_aliasesMem IN OUT PUCHAR TargetPortGroupsInfo
    );

NTSTATUS
DsmpQueryLBPolicyForDevice(
    __in IN PWSTR RegistryKeyName,
    __in IN  ULONGLONG PathId,
    __out OUT PULONG PrimaryPath,
    __out OUT PULONG OptimizedPath,
    __out OUT PULONG PathWeight
    );

VOID
DsmpGetDSMPathKeyName(
    __in IN   ULONGLONG DSMPathId,
    __out_ecount(DsmPathKeyNameSize) OUT PWCHAR DsmPathKeyName,
    __in IN   ULONG  DsmPathKeyNameSize
    );

UCHAR
DsmpGetAsciiForBinary(
    __in UCHAR BinaryChar
    );

NTSTATUS
DsmpGetDeviceIdList (
    __in IN PDEVICE_OBJECT DeviceObject,
    __out OUT PSTORAGE_DESCRIPTOR_HEADER *Descriptor
    );

NTSTATUS
DsmpSetTargetPortGroups(
    __in IN PDEVICE_OBJECT DeviceObject,
    __in_bcount(TargetPortGroupsInfoLength) IN PUCHAR TargetPortGroupsInfo,
    __in IN ULONG TargetPortGroupsInfoLength
    );

NTSTATUS
DsmpSetTargetPortGroupsAsync(
    __in IN PDSM_DEVICE_INFO DeviceInfo,
    __in IN PIO_COMPLETION_ROUTINE CompletionRoutine,
    __in IN PDSM_TPG_COMPLETION_CONTEXT CompletionContext,
    __in IN ULONG TargetPortGroupsInfoLength,
    __in IN PUCHAR TargetPortGroupsInfo
    );

PDSM_LOAD_BALANCE_POLICY_SETTINGS
DsmpCopyLoadBalancePolicies(
    __in IN PDSM_GROUP_ENTRY GroupEntry,
    __in IN ULONG DsmWmiVersion,
    __in IN PVOID SupportedLBPolicies
    );

NTSTATUS
DsmpPersistLBSettings(
    __in IN PDSM_LOAD_BALANCE_POLICY_SETTINGS LoadBalanceSettings
    );

NTSTATUS
DsmpSetDeviceALUAState(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PDSM_DEVICE_INFO DeviceInfo,
    __in IN DSM_DEVICE_STATE DevState
    );

NTSTATUS
DsmpGetDeviceALUAState(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PDSM_DEVICE_INFO DeviceInfo,
    __in_opt IN PDSM_DEVICE_STATE DevState
    );

NTSTATUS
DsmpAdjustDeviceStatesALUA(
    __in IN PDSM_GROUP_ENTRY Group,
    __in_opt IN PDSM_DEVICE_INFO PreferredActiveDeviceInfo
    );

PDSM_WORKITEM
DsmpAllocateWorkItem(
    __in IN PDEVICE_OBJECT DeviceObject,
    __in IN PVOID Context
    );

VOID
DsmpFreeWorkItem(
    __in IN PDSM_WORKITEM DsmWorkItem
    );

NTSTATUS
DsmpRegCopyTree(
    __in IN HANDLE SourceKey,
    __in IN HANDLE DestKey
    );

NTSTATUS
DsmpRegDeleteTree(
    __in IN HANDLE KeyRoot
    );

#if defined (_WIN64)
VOID
DsmpPassThroughPathTranslate32To64(
    __in IN PMPIO_PASS_THROUGH_PATH32 MpioPassThroughPath32,
    __inout IN OUT PMPIO_PASS_THROUGH_PATH MpioPassThroughPath64
    );

VOID
DsmpPassThroughPathTranslate64To32(
    __in IN PMPIO_PASS_THROUGH_PATH MpioPassThroughPath64,
    __inout IN OUT PMPIO_PASS_THROUGH_PATH32 MpioPassThroughPath32
    );
#endif

NTSTATUS
DsmpGetMaxPRRetryTime(
    __in IN PDSM_CONTEXT Context,
    __out OUT PULONG RetryTime
    );

NTSTATUS
DsmpQueryCacheInformationFromRegistry(
    __in IN PDSM_CONTEXT DsmContext,
    __out OUT PBOOLEAN UseCacheForLeastBlocks,
    __out OUT PULONGLONG CacheSizeForLeastBlocks
    );


//
// Function prototypes for functions in wmi.c
//

VOID
DsmpDsmWmiInitialize(
    __in IN PDSM_WMILIB_CONTEXT WmiGlobalInfo,
    __in IN PUNICODE_STRING RegistryPath
    );

NTSTATUS
DsmGlobalQueryData(
    __in IN PVOID DsmContext,
    __in IN PDSM_IDS DsmIds,
    __in IN PIRP Irp,
    __in IN ULONG GuidIndex,
    __in IN ULONG InstanceIndex,
    __in IN ULONG InstanceCount,
    __inout IN OUT PULONG InstanceLengthArray,
    __in IN ULONG BufferAvail,
    __out_bcount(BufferAvail) OUT PUCHAR Buffer,
    __out OUT PULONG DataLength,
    ...
    );

NTSTATUS
DsmGlobalSetData(
    __in IN PVOID DsmContext,
    __in IN PDSM_IDS DsmIds,
    __in IN PIRP Irp,
    __in IN ULONG GuidIndex,
    __in IN ULONG InstanceIndex,
    __in IN ULONG BufferAvail,
    __in_bcount(BufferAvail) IN PUCHAR Buffer,
    ...
    );

VOID
DsmpWmiInitialize(
    __in IN PDSM_WMILIB_CONTEXT WmiInfo,
    __in IN PUNICODE_STRING RegistryPath
    );

NTSTATUS
DsmQueryData(
    __in IN PVOID DsmContext,
    __in IN PDSM_IDS DsmIds,
    __in IN PIRP Irp,
    __in IN ULONG GuidIndex,
    __in IN ULONG InstanceIndex,
    __in IN ULONG InstanceCount,
    __inout IN OUT PULONG InstanceLengthArray,
    __in IN ULONG BufferAvail,
    __out_bcount(BufferAvail) OUT PUCHAR Buffer,
    __out OUT PULONG DataLength,
    ...
    );

NTSTATUS
DsmpQueryLoadBalancePolicy(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PDSM_IDS     DsmIds,
    __in IN ULONG        DsmWmiVersion,
    __in IN ULONG        InBufferSize,
    __in IN PULONG       OutBufferSize,
    __out OUT PVOID      Buffer
    );

NTSTATUS
DsmpQuerySupportedLBPolicies(
    __in IN   PDSM_CONTEXT DsmContext,
    __in IN   PDSM_IDS DsmIds,
    __in IN   ULONG BufferAvail,
    __in IN   ULONG DsmWmiVersion,
    __out OUT PULONG OutBufferSize,
    __out_bcount(BufferAvail) OUT PUCHAR Buffer
   );

NTSTATUS
DsmExecuteMethod(
    __in IN PVOID DsmContext,
    __in IN PDSM_IDS DsmIds,
    __in IN PIRP  Irp,
    __in IN ULONG GuidIndex,
    __in IN ULONG InstanceIndex,
    __in IN ULONG MethodId,
    __in IN ULONG InBufferSize,
    __in IN PULONG OutBufferSize,
    __inout IN OUT PUCHAR Buffer,
    ...
    );

NTSTATUS
DsmpClearLoadBalancePolicy(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PDSM_IDS     DsmIds
    );

NTSTATUS
DsmpSetLoadBalancePolicy(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PDSM_IDS     DsmIds,
    __in IN ULONG        DsmWmiVersion,
    __in IN ULONG        InBufferSize,
    __in IN PULONG       OutBufferSize,
    __in IN PVOID        Buffer
    );

NTSTATUS
DsmpValidateSetLBPolicyInput(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PDSM_IDS     DsmIds,
    __in IN ULONG        DsmWmiVersion,
    __in IN PVOID        SetLoadBalancePolicyIN,
    __in IN ULONG        InBufferSize
    );

VOID
DsmpSaveDeviceState(
    __in IN PVOID SupportedLBPolicies,
    __in IN ULONG DsmWmiVersion
    );

VOID
DsmpRestorePreviousDeviceState(
    __in IN PVOID SupportedLBPolicies,
    __in IN ULONG DsmWmiVersion
    );

VOID
DsmpUpdateDesiredStateAndWeight(
    __in IN PDSM_GROUP_ENTRY Group,
    __in IN ULONG DsmWmiVersion,
    __in IN PVOID SupportedLBPolicies
    );

NTSTATUS
DsmpQueryDevicePerf(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PDSM_IDS DsmIds,
    __in IN ULONG InBufferSize,
    __out OUT PULONG OutBufferSize,
    __out_bcount(*OutBufferSize) OUT PUCHAR Buffer
    );

NTSTATUS
DsmpClearPerfCounters(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PDSM_IDS DsmIds
    );

NTSTATUS
DsmpQuerySupportedDevicesList(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN ULONG InBufferSize,
    __out OUT PULONG OutBufferSize,
    __out_bcount(*OutBufferSize) OUT PUCHAR Buffer
    );

NTSTATUS
DsmpQueryTargetsDefaultPolicy(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN ULONG InBufferSize,
    __out OUT PULONG OutBufferSize,
    __out_bcount(*OutBufferSize) OUT PUCHAR Buffer
    );

NTSTATUS
DsmpQueryDsmDefaultPolicy(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN ULONG InBufferSize,
    __out OUT PULONG OutBufferSize,
    __out_bcount(*OutBufferSize) OUT PUCHAR Buffer
    );


//
// Function prototypes for functions in debug.c
//

VOID
DsmpDebugPrint(
    __in ULONG DebugPrintLevel,
    __in PCCHAR DebugMessage,
    ...
    );

#endif // _PROTOTYPES_H_

