/*  
	Copyright (c) Microsoft Corporation.  All rights reserved.
*/

#ifndef _SDV_WDF_H_
#define _SDV_WDF_H_


#include <sdv_usb_wdf.h>




#ifdef WdfChildListCreate
#undef WdfChildListCreate
#endif
#define WdfChildListCreate sdv_WdfChildListCreate
NTSTATUS  sdv_WdfChildListCreate(  
__in  WDFDEVICE  Device,
__in  PWDF_CHILD_LIST_CONFIG  Config,
__in_opt  PWDF_OBJECT_ATTRIBUTES  ChildListAttributes,
__out  WDFCHILDLIST*  ChildList);


#ifdef WdfChildListGetDevice
#undef WdfChildListGetDevice
#endif
#define WdfChildListGetDevice sdv_WdfChildListGetDevice
WDFDEVICE  sdv_WdfChildListGetDevice(  
__in  WDFCHILDLIST  ChildList);


#ifdef WdfChildListRetrievePdo
#undef WdfChildListRetrievePdo
#endif
#define WdfChildListRetrievePdo sdv_WdfChildListRetrievePdo
WDFDEVICE  sdv_WdfChildListRetrievePdo(  
__in  WDFCHILDLIST  ChildList,
__in  PWDF_CHILD_RETRIEVE_INFO  RetrieveInfo);


#ifdef WdfChildListRetrieveAddressDescription
#undef WdfChildListRetrieveAddressDescription
#endif
#define WdfChildListRetrieveAddressDescription sdv_WdfChildListRetrieveAddressDescription
NTSTATUS  sdv_WdfChildListRetrieveAddressDescription(  
__in  WDFCHILDLIST  ChildList,
__in  PWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER  IdentificationDescription,
__inout  PWDF_CHILD_ADDRESS_DESCRIPTION_HEADER  AddressDescription);


#ifdef WdfChildListBeginScan
#undef WdfChildListBeginScan
#endif
#define WdfChildListBeginScan(arg1) sdv_WdfChildListBeginScan(arg1);
VOID  sdv_WdfChildListBeginScan(  
__in  WDFCHILDLIST  ChildList);


#ifdef WdfChildListEndScan
#undef WdfChildListEndScan
#endif
#define WdfChildListEndScan(arg1) sdv_WdfChildListEndScan(arg1);
VOID  sdv_WdfChildListEndScan(  
__in  WDFCHILDLIST  ChildList);


#ifdef WdfChildListBeginIteration
#undef WdfChildListBeginIteration
#endif
#define WdfChildListBeginIteration(arg1,arg2) sdv_WdfChildListBeginIteration(arg1,arg2);
VOID  sdv_WdfChildListBeginIteration(  
__in  WDFCHILDLIST  ChildList,
__in  PWDF_CHILD_LIST_ITERATOR  Iterator);


#ifdef WdfChildListRetrieveNextDevice
#undef WdfChildListRetrieveNextDevice
#endif
#define WdfChildListRetrieveNextDevice sdv_WdfChildListRetrieveNextDevice
NTSTATUS  sdv_WdfChildListRetrieveNextDevice(  
__in  WDFCHILDLIST  ChildList,
__in  PWDF_CHILD_LIST_ITERATOR  Iterator,
__out  WDFDEVICE*  Device,
__in_opt  PWDF_CHILD_RETRIEVE_INFO  Info);


#ifdef WdfChildListEndIteration
#undef WdfChildListEndIteration
#endif
#define WdfChildListEndIteration(arg1,arg2) sdv_WdfChildListEndIteration(arg1,arg2);
VOID  sdv_WdfChildListEndIteration(  
__in  WDFCHILDLIST  ChildList,
__in  PWDF_CHILD_LIST_ITERATOR  Iterator);


#ifdef WdfChildListAddOrUpdateChildDescriptionAsPresent
#undef WdfChildListAddOrUpdateChildDescriptionAsPresent
#endif
#define WdfChildListAddOrUpdateChildDescriptionAsPresent sdv_WdfChildListAddOrUpdateChildDescriptionAsPresent
NTSTATUS  sdv_WdfChildListAddOrUpdateChildDescriptionAsPresent(  
__in  WDFCHILDLIST  ChildList,
__in  PWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER  IdentificationDescription,
__in_opt  PWDF_CHILD_ADDRESS_DESCRIPTION_HEADER  AddressDescription);


#ifdef WdfChildListUpdateChildDescriptionAsMissing
#undef WdfChildListUpdateChildDescriptionAsMissing
#endif
#define WdfChildListUpdateChildDescriptionAsMissing sdv_WdfChildListUpdateChildDescriptionAsMissing
NTSTATUS  sdv_WdfChildListUpdateChildDescriptionAsMissing(  
__in  WDFCHILDLIST  ChildList,
__in  PWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER  IdentificationDescription);


#ifdef WdfChildListUpdateAllChildDescriptionsAsPresent
#undef WdfChildListUpdateAllChildDescriptionsAsPresent
#endif
#define WdfChildListUpdateAllChildDescriptionsAsPresent(arg1) sdv_WdfChildListUpdateAllChildDescriptionsAsPresent(arg1);
VOID  sdv_WdfChildListUpdateAllChildDescriptionsAsPresent(  
__in  WDFCHILDLIST  ChildList);


#ifdef WdfChildListRequestChildEject
#undef WdfChildListRequestChildEject
#endif
#define WdfChildListRequestChildEject sdv_WdfChildListRequestChildEject
BOOLEAN  sdv_WdfChildListRequestChildEject(  
__in  WDFCHILDLIST  ChildList,
__in  PWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER  IdentificationDescription);


#ifdef WdfCollectionCreate
#undef WdfCollectionCreate
#endif
#define WdfCollectionCreate sdv_WdfCollectionCreate
NTSTATUS  sdv_WdfCollectionCreate(  
__in_opt  PWDF_OBJECT_ATTRIBUTES  CollectionAttributes,
__out  WDFCOLLECTION*  Collection);


#ifdef WdfCollectionGetCount
#undef WdfCollectionGetCount
#endif
#define WdfCollectionGetCount sdv_WdfCollectionGetCount
ULONG  sdv_WdfCollectionGetCount(  
__in  WDFCOLLECTION  Collection);


#ifdef WdfCollectionAdd
#undef WdfCollectionAdd
#endif
#define WdfCollectionAdd sdv_WdfCollectionAdd
NTSTATUS  sdv_WdfCollectionAdd(  
__in  WDFCOLLECTION  Collection,
__in  WDFOBJECT  Object);


#ifdef WdfCollectionRemove
#undef WdfCollectionRemove
#endif
#define WdfCollectionRemove(arg1,arg2) sdv_WdfCollectionRemove(arg1,arg2);
VOID  sdv_WdfCollectionRemove(  
__in  WDFCOLLECTION  Collection,
__in  WDFOBJECT  Item);


#ifdef WdfCollectionRemoveItem
#undef WdfCollectionRemoveItem
#endif
#define WdfCollectionRemoveItem(arg1,arg2) sdv_WdfCollectionRemoveItem(arg1,arg2);
VOID  sdv_WdfCollectionRemoveItem(  
__in  WDFCOLLECTION  Collection,
   ULONG  Index);


#ifdef WdfCollectionGetItem
#undef WdfCollectionGetItem
#endif
#define WdfCollectionGetItem sdv_WdfCollectionGetItem
WDFOBJECT  sdv_WdfCollectionGetItem(  
__in  WDFCOLLECTION  Collection,
   ULONG  Index);


#ifdef WdfCollectionGetFirstItem
#undef WdfCollectionGetFirstItem
#endif
#define WdfCollectionGetFirstItem sdv_WdfCollectionGetFirstItem
WDFOBJECT  sdv_WdfCollectionGetFirstItem(  
__in  WDFCOLLECTION  Collection);


#ifdef WdfCollectionGetLastItem
#undef WdfCollectionGetLastItem
#endif
#define WdfCollectionGetLastItem sdv_WdfCollectionGetLastItem
WDFOBJECT  sdv_WdfCollectionGetLastItem(  
__in  WDFCOLLECTION  Collection);


#ifdef WdfCommonBufferCreate
#undef WdfCommonBufferCreate
#endif
#define WdfCommonBufferCreate sdv_WdfCommonBufferCreate
NTSTATUS  sdv_WdfCommonBufferCreate(  
__in  WDFDMAENABLER  DmaEnabler,
   size_t  Length,
__in_opt  PWDF_OBJECT_ATTRIBUTES  Attributes,
__out  WDFCOMMONBUFFER*  CommonBuffer);


#ifdef WdfCommonBufferCreateWithConfig
#undef WdfCommonBufferCreateWithConfig
#endif
#define WdfCommonBufferCreateWithConfig sdv_WdfCommonBufferCreateWithConfig
NTSTATUS  sdv_WdfCommonBufferCreateWithConfig(  
__in  WDFDMAENABLER  DmaEnabler,
   size_t  Length,
__in  PWDF_COMMON_BUFFER_CONFIG  Config,
__in_opt  PWDF_OBJECT_ATTRIBUTES  Attributes,
__out  WDFCOMMONBUFFER*  CommonBuffer);


#ifdef WdfCommonBufferGetAlignedVirtualAddress
#undef WdfCommonBufferGetAlignedVirtualAddress
#endif
#define WdfCommonBufferGetAlignedVirtualAddress sdv_WdfCommonBufferGetAlignedVirtualAddress
PVOID  sdv_WdfCommonBufferGetAlignedVirtualAddress(  
__in  WDFCOMMONBUFFER  CommonBuffer);


#ifdef WdfCommonBufferGetAlignedLogicalAddress
#undef WdfCommonBufferGetAlignedLogicalAddress
#endif
#define WdfCommonBufferGetAlignedLogicalAddress sdv_WdfCommonBufferGetAlignedLogicalAddress
PHYSICAL_ADDRESS  sdv_WdfCommonBufferGetAlignedLogicalAddress(  
__in  WDFCOMMONBUFFER  CommonBuffer);


#ifdef WdfCommonBufferGetLength
#undef WdfCommonBufferGetLength
#endif
#define WdfCommonBufferGetLength sdv_WdfCommonBufferGetLength
size_t  sdv_WdfCommonBufferGetLength(  
__in  WDFCOMMONBUFFER  CommonBuffer);


#ifdef WdfControlDeviceInitAllocate
#undef WdfControlDeviceInitAllocate
#endif
#define WdfControlDeviceInitAllocate sdv_WdfControlDeviceInitAllocate
PWDFDEVICE_INIT  sdv_WdfControlDeviceInitAllocate(  
  IN  WDFDRIVER  Driver,
  IN CONST UNICODE_STRING*  SDDLString);


#ifdef WdfControlDeviceInitSetShutdownNotification
#undef WdfControlDeviceInitSetShutdownNotification
#endif
#define WdfControlDeviceInitSetShutdownNotification(arg1,arg2,arg3) sdv_WdfControlDeviceInitSetShutdownNotification(arg1,arg2,arg3);
VOID  sdv_WdfControlDeviceInitSetShutdownNotification(  
__in  PWDFDEVICE_INIT  DeviceInit,
__in  PFN_WDF_DEVICE_SHUTDOWN_NOTIFICATION  Notification,
__in  UCHAR  Flags);


#ifdef WdfControlFinishInitializing
#undef WdfControlFinishInitializing
#endif
#define WdfControlFinishInitializing(arg1) sdv_WdfControlFinishInitializing(arg1);
VOID  sdv_WdfControlFinishInitializing(  
__in  WDFDEVICE  Device);


#ifdef WdfDeviceGetDeviceState
#undef WdfDeviceGetDeviceState
#endif
#define WdfDeviceGetDeviceState(arg1,arg2) sdv_WdfDeviceGetDeviceState(arg1,arg2);
VOID  sdv_WdfDeviceGetDeviceState(  
__in  WDFDEVICE  Device,
__out  PWDF_DEVICE_STATE  DeviceState);


#ifdef WdfDeviceSetDeviceState
#undef WdfDeviceSetDeviceState
#endif
#define WdfDeviceSetDeviceState(arg1,arg2) sdv_WdfDeviceSetDeviceState(arg1,arg2);
VOID  sdv_WdfDeviceSetDeviceState(  
__in  WDFDEVICE  Device,
__in  PWDF_DEVICE_STATE  DeviceState);


#ifdef WdfWdmDeviceGetWdfDeviceHandle
#undef WdfWdmDeviceGetWdfDeviceHandle
#endif
#define WdfWdmDeviceGetWdfDeviceHandle sdv_WdfWdmDeviceGetWdfDeviceHandle
WDFDEVICE  sdv_WdfWdmDeviceGetWdfDeviceHandle(  
__in  PDEVICE_OBJECT  DeviceObject);


#ifdef WdfDeviceWdmGetDeviceObject
#undef WdfDeviceWdmGetDeviceObject
#endif
#define WdfDeviceWdmGetDeviceObject sdv_WdfDeviceWdmGetDeviceObject
PDEVICE_OBJECT  sdv_WdfDeviceWdmGetDeviceObject(  
__in  WDFDEVICE  Device);


#ifdef WdfDeviceWdmGetAttachedDevice
#undef WdfDeviceWdmGetAttachedDevice
#endif
#define WdfDeviceWdmGetAttachedDevice sdv_WdfDeviceWdmGetAttachedDevice
PDEVICE_OBJECT  sdv_WdfDeviceWdmGetAttachedDevice(  
__in  WDFDEVICE  Device);


#ifdef WdfDeviceWdmGetPhysicalDevice
#undef WdfDeviceWdmGetPhysicalDevice
#endif
#define WdfDeviceWdmGetPhysicalDevice sdv_WdfDeviceWdmGetPhysicalDevice
PDEVICE_OBJECT  sdv_WdfDeviceWdmGetPhysicalDevice(  
__in  WDFDEVICE  Device);


#ifdef WdfDeviceWdmDispatchPreprocessedIrp
#undef WdfDeviceWdmDispatchPreprocessedIrp
#endif
#define WdfDeviceWdmDispatchPreprocessedIrp sdv_WdfDeviceWdmDispatchPreprocessedIrp
NTSTATUS  sdv_WdfDeviceWdmDispatchPreprocessedIrp(  
__in  WDFDEVICE  Device,
__in  PIRP  Irp);


#ifdef WdfDeviceAddDependentUsageDeviceObject
#undef WdfDeviceAddDependentUsageDeviceObject
#endif
#define WdfDeviceAddDependentUsageDeviceObject sdv_WdfDeviceAddDependentUsageDeviceObject
NTSTATUS  sdv_WdfDeviceAddDependentUsageDeviceObject(  
__in  WDFDEVICE  Device,
__in  PDEVICE_OBJECT  DependentDevice);


#ifdef WdfDeviceRemoveDependentUsageDeviceObject
#undef WdfDeviceRemoveDependentUsageDeviceObject
#endif
#define WdfDeviceRemoveDependentUsageDeviceObject(arg1,arg2) sdv_WdfDeviceRemoveDependentUsageDeviceObject(arg1,arg2);
VOID  sdv_WdfDeviceRemoveDependentUsageDeviceObject(  
__in  WDFDEVICE  Device,
__in  PDEVICE_OBJECT  DependentDevice);


#ifdef WdfDeviceAddRemovalRelationsPhysicalDevice
#undef WdfDeviceAddRemovalRelationsPhysicalDevice
#endif
#define WdfDeviceAddRemovalRelationsPhysicalDevice sdv_WdfDeviceAddRemovalRelationsPhysicalDevice
NTSTATUS  sdv_WdfDeviceAddRemovalRelationsPhysicalDevice(  
__in  WDFDEVICE  Device,
__in  PDEVICE_OBJECT  PhysicalDevice);


#ifdef WdfDeviceRemoveRemovalRelationsPhysicalDevice
#undef WdfDeviceRemoveRemovalRelationsPhysicalDevice
#endif
#define WdfDeviceRemoveRemovalRelationsPhysicalDevice(arg1,arg2) sdv_WdfDeviceRemoveRemovalRelationsPhysicalDevice(arg1,arg2);
VOID  sdv_WdfDeviceRemoveRemovalRelationsPhysicalDevice(  
__in  WDFDEVICE  Device,
__in  PDEVICE_OBJECT  PhysicalDevice);


#ifdef WdfDeviceClearRemovalRelationsDevices
#undef WdfDeviceClearRemovalRelationsDevices
#endif
#define WdfDeviceClearRemovalRelationsDevices(arg1) sdv_WdfDeviceClearRemovalRelationsDevices(arg1);
VOID  sdv_WdfDeviceClearRemovalRelationsDevices(  
__in  WDFDEVICE  Device);


#ifdef WdfDeviceGetDriver
#undef WdfDeviceGetDriver
#endif
#define WdfDeviceGetDriver sdv_WdfDeviceGetDriver
WDFDRIVER  sdv_WdfDeviceGetDriver(  
__in  WDFDEVICE  Device);


#ifdef WdfDeviceRetrieveDeviceName
#undef WdfDeviceRetrieveDeviceName
#endif
#define WdfDeviceRetrieveDeviceName sdv_WdfDeviceRetrieveDeviceName
NTSTATUS  sdv_WdfDeviceRetrieveDeviceName(  
__in  WDFDEVICE  Device,
__in  WDFSTRING  String);


#ifdef WdfDeviceAssignMofResourceName
#undef WdfDeviceAssignMofResourceName
#endif
#define WdfDeviceAssignMofResourceName sdv_WdfDeviceAssignMofResourceName
NTSTATUS  sdv_WdfDeviceAssignMofResourceName(  
__in  WDFDEVICE  Device,
__in  PCUNICODE_STRING  MofResourceName);


#ifdef WdfDeviceGetIoTarget
#undef WdfDeviceGetIoTarget
#endif
#define WdfDeviceGetIoTarget sdv_WdfDeviceGetIoTarget
WDFIOTARGET  sdv_WdfDeviceGetIoTarget(  
__in  WDFDEVICE  Device);


#ifdef WdfDeviceGetDevicePnpState
#undef WdfDeviceGetDevicePnpState
#endif
#define WdfDeviceGetDevicePnpState sdv_WdfDeviceGetDevicePnpState
WDF_DEVICE_PNP_STATE  sdv_WdfDeviceGetDevicePnpState(  
__in  WDFDEVICE  Device);


#ifdef WdfDeviceGetDevicePowerState
#undef WdfDeviceGetDevicePowerState
#endif
#define WdfDeviceGetDevicePowerState sdv_WdfDeviceGetDevicePowerState
WDF_DEVICE_POWER_STATE  sdv_WdfDeviceGetDevicePowerState(  
__in  WDFDEVICE  Device);


#ifdef WdfDeviceGetDevicePowerPolicyState
#undef WdfDeviceGetDevicePowerPolicyState
#endif
#define WdfDeviceGetDevicePowerPolicyState sdv_WdfDeviceGetDevicePowerPolicyState
WDF_DEVICE_POWER_POLICY_STATE  sdv_WdfDeviceGetDevicePowerPolicyState(  
__in  WDFDEVICE  Device);


#ifdef WdfDeviceAssignS0IdleSettings
#undef WdfDeviceAssignS0IdleSettings
#endif
#define WdfDeviceAssignS0IdleSettings sdv_WdfDeviceAssignS0IdleSettings
NTSTATUS  sdv_WdfDeviceAssignS0IdleSettings(  
__in  WDFDEVICE  Device,
__in  PWDF_DEVICE_POWER_POLICY_IDLE_SETTINGS  Settings);


#ifdef WdfDeviceAssignSxWakeSettings
#undef WdfDeviceAssignSxWakeSettings
#endif
#define WdfDeviceAssignSxWakeSettings sdv_WdfDeviceAssignSxWakeSettings
NTSTATUS  sdv_WdfDeviceAssignSxWakeSettings(  
__in  WDFDEVICE  Device,
__in  PWDF_DEVICE_POWER_POLICY_WAKE_SETTINGS  Settings);


#ifdef WdfDeviceOpenRegistryKey
#undef WdfDeviceOpenRegistryKey
#endif
#define WdfDeviceOpenRegistryKey sdv_WdfDeviceOpenRegistryKey
NTSTATUS  sdv_WdfDeviceOpenRegistryKey(  
__in  WDFDEVICE  Device,
   ULONG  DeviceInstanceKeyType,
   ACCESS_MASK  DesiredAccess,
__in_opt  PWDF_OBJECT_ATTRIBUTES  KeyAttributes,
__out  WDFKEY*  Key);


#ifdef WdfDeviceSetSpecialFileSupport
#undef WdfDeviceSetSpecialFileSupport
#endif
#define WdfDeviceSetSpecialFileSupport(arg1,arg2,arg3) sdv_WdfDeviceSetSpecialFileSupport(arg1,arg2,arg3);
VOID  sdv_WdfDeviceSetSpecialFileSupport(  
__in  WDFDEVICE  Device,
   WDF_SPECIAL_FILE_TYPE  FileType,
   BOOLEAN  FileTypeIsSupported);


#ifdef WdfDeviceSetCharacteristics
#undef WdfDeviceSetCharacteristics
#endif
#define WdfDeviceSetCharacteristics(arg1,arg2) sdv_WdfDeviceSetCharacteristics(arg1,arg2);
VOID  sdv_WdfDeviceSetCharacteristics(  
__in  WDFDEVICE  Device,
   ULONG  DeviceCharacteristics);


#ifdef WdfDeviceGetCharacteristics
#undef WdfDeviceGetCharacteristics
#endif
#define WdfDeviceGetCharacteristics sdv_WdfDeviceGetCharacteristics
ULONG  sdv_WdfDeviceGetCharacteristics(  
__in  WDFDEVICE  Device);


#ifdef WdfDeviceGetAlignmentRequirement
#undef WdfDeviceGetAlignmentRequirement
#endif
#define WdfDeviceGetAlignmentRequirement sdv_WdfDeviceGetAlignmentRequirement
ULONG  sdv_WdfDeviceGetAlignmentRequirement(  
__in  WDFDEVICE  Device);


#ifdef WdfDeviceSetAlignmentRequirement
#undef WdfDeviceSetAlignmentRequirement
#endif
#define WdfDeviceSetAlignmentRequirement(arg1,arg2) sdv_WdfDeviceSetAlignmentRequirement(arg1,arg2);
VOID  sdv_WdfDeviceSetAlignmentRequirement(  
__in  WDFDEVICE  Device,
   ULONG  AlignmentRequirement);


#ifdef WdfDeviceInitFree
#undef WdfDeviceInitFree
#endif
#define WdfDeviceInitFree(arg1) sdv_WdfDeviceInitFree(arg1);
VOID  sdv_WdfDeviceInitFree(  
__in  PWDFDEVICE_INIT  DeviceInit);


#ifdef WdfDeviceInitSetPnpPowerEventCallbacks
#undef WdfDeviceInitSetPnpPowerEventCallbacks
#endif
#define WdfDeviceInitSetPnpPowerEventCallbacks(arg1,arg2) sdv_WdfDeviceInitSetPnpPowerEventCallbacks(arg1,arg2);
VOID  sdv_WdfDeviceInitSetPnpPowerEventCallbacks(  
__in  PWDFDEVICE_INIT  DeviceInit,
__in  PWDF_PNPPOWER_EVENT_CALLBACKS  PnpPowerEventCallbacks);


#ifdef WdfDeviceInitSetPowerPolicyEventCallbacks
#undef WdfDeviceInitSetPowerPolicyEventCallbacks
#endif
#define WdfDeviceInitSetPowerPolicyEventCallbacks(arg1,arg2) sdv_WdfDeviceInitSetPowerPolicyEventCallbacks(arg1,arg2);
VOID  sdv_WdfDeviceInitSetPowerPolicyEventCallbacks(  
__in  PWDFDEVICE_INIT  DeviceInit,
__in  PWDF_POWER_POLICY_EVENT_CALLBACKS  PowerPolicyEventCallbacks);


#ifdef WdfDeviceInitSetPowerPolicyOwnership
#undef WdfDeviceInitSetPowerPolicyOwnership
#endif
#define WdfDeviceInitSetPowerPolicyOwnership(arg1,arg2) sdv_WdfDeviceInitSetPowerPolicyOwnership(arg1,arg2);
VOID  sdv_WdfDeviceInitSetPowerPolicyOwnership(  
__in  PWDFDEVICE_INIT  DeviceInit,
   BOOLEAN  IsPowerPolicyOwner);


#ifdef WdfDeviceInitRegisterPnpStateChangeCallback
#undef WdfDeviceInitRegisterPnpStateChangeCallback
#endif
#define WdfDeviceInitRegisterPnpStateChangeCallback sdv_WdfDeviceInitRegisterPnpStateChangeCallback
NTSTATUS  sdv_WdfDeviceInitRegisterPnpStateChangeCallback(  
__in  PWDFDEVICE_INIT  DeviceInit,
   WDF_DEVICE_PNP_STATE  PnpState,
__in  PFN_WDF_DEVICE_PNP_STATE_CHANGE_NOTIFICATION  EvtDevicePnpStateChange,
   ULONG  CallbackTypes);


#ifdef WdfDeviceInitRegisterPowerStateChangeCallback
#undef WdfDeviceInitRegisterPowerStateChangeCallback
#endif
#define WdfDeviceInitRegisterPowerStateChangeCallback sdv_WdfDeviceInitRegisterPowerStateChangeCallback
NTSTATUS  sdv_WdfDeviceInitRegisterPowerStateChangeCallback(  
__in  PWDFDEVICE_INIT  DeviceInit,
   WDF_DEVICE_POWER_STATE  PowerState,
__in  PFN_WDF_DEVICE_POWER_STATE_CHANGE_NOTIFICATION  EvtDevicePowerStateChange,
   ULONG  CallbackTypes);


#ifdef WdfDeviceInitRegisterPowerPolicyStateChangeCallback
#undef WdfDeviceInitRegisterPowerPolicyStateChangeCallback
#endif
#define WdfDeviceInitRegisterPowerPolicyStateChangeCallback sdv_WdfDeviceInitRegisterPowerPolicyStateChangeCallback
NTSTATUS  sdv_WdfDeviceInitRegisterPowerPolicyStateChangeCallback(  
__in  PWDFDEVICE_INIT  DeviceInit,
   WDF_DEVICE_POWER_POLICY_STATE  PowerPolicyState,
__in  PFN_WDF_DEVICE_POWER_POLICY_STATE_CHANGE_NOTIFICATION  EvtDevicePowerPolicyStateChange,
   ULONG  CallbackTypes);


#ifdef WdfDeviceInitSetIoType
#undef WdfDeviceInitSetIoType
#endif
#define WdfDeviceInitSetIoType(arg1,arg2) sdv_WdfDeviceInitSetIoType(arg1,arg2);
VOID  sdv_WdfDeviceInitSetIoType(  
__in  PWDFDEVICE_INIT  DeviceInit,
   WDF_DEVICE_IO_TYPE  IoType);


#ifdef WdfDeviceInitSetExclusive
#undef WdfDeviceInitSetExclusive
#endif
#define WdfDeviceInitSetExclusive(arg1,arg2) sdv_WdfDeviceInitSetExclusive(arg1,arg2);
VOID  sdv_WdfDeviceInitSetExclusive(  
__in  PWDFDEVICE_INIT  DeviceInit,
   BOOLEAN  IsExclusive);


#ifdef WdfDeviceInitSetPowerNotPageable
#undef WdfDeviceInitSetPowerNotPageable
#endif
#define WdfDeviceInitSetPowerNotPageable(arg1) sdv_WdfDeviceInitSetPowerNotPageable(arg1);
VOID  sdv_WdfDeviceInitSetPowerNotPageable(  
__in  PWDFDEVICE_INIT  DeviceInit);


#ifdef WdfDeviceInitSetPowerPageable
#undef WdfDeviceInitSetPowerPageable
#endif
#define WdfDeviceInitSetPowerPageable(arg1) sdv_WdfDeviceInitSetPowerPageable(arg1);
VOID  sdv_WdfDeviceInitSetPowerPageable(  
__in  PWDFDEVICE_INIT  DeviceInit);


#ifdef WdfDeviceInitSetPowerInrush
#undef WdfDeviceInitSetPowerInrush
#endif
#define WdfDeviceInitSetPowerInrush(arg1) sdv_WdfDeviceInitSetPowerInrush(arg1);
VOID  sdv_WdfDeviceInitSetPowerInrush(  
__in  PWDFDEVICE_INIT  DeviceInit);


#ifdef WdfDeviceInitSetDeviceType
#undef WdfDeviceInitSetDeviceType
#endif
#define WdfDeviceInitSetDeviceType(arg1,arg2) sdv_WdfDeviceInitSetDeviceType(arg1,arg2);
VOID  sdv_WdfDeviceInitSetDeviceType(  
__in  PWDFDEVICE_INIT  DeviceInit,
   DEVICE_TYPE  DeviceType);


#ifdef WdfDeviceInitAssignName
#undef WdfDeviceInitAssignName
#endif
#define WdfDeviceInitAssignName sdv_WdfDeviceInitAssignName
NTSTATUS  sdv_WdfDeviceInitAssignName(  
__in  PWDFDEVICE_INIT  DeviceInit,
__in_opt  PCUNICODE_STRING  DeviceName);


#ifdef WdfDeviceInitAssignSDDLString
#undef WdfDeviceInitAssignSDDLString
#endif
#define WdfDeviceInitAssignSDDLString sdv_WdfDeviceInitAssignSDDLString
NTSTATUS  sdv_WdfDeviceInitAssignSDDLString(  
__in  PWDFDEVICE_INIT  DeviceInit,
__in  PCUNICODE_STRING  SDDLString);


#ifdef WdfDeviceInitSetDeviceClass
#undef WdfDeviceInitSetDeviceClass
#endif
#define WdfDeviceInitSetDeviceClass(arg1,arg2) sdv_WdfDeviceInitSetDeviceClass(arg1,arg2);
VOID  sdv_WdfDeviceInitSetDeviceClass(  
__in  PWDFDEVICE_INIT  DeviceInit,
__in  GUID*  DeviceClassGuid);


#ifdef WdfDeviceInitSetCharacteristics
#undef WdfDeviceInitSetCharacteristics
#endif
#define WdfDeviceInitSetCharacteristics(arg1,arg2,arg3) sdv_WdfDeviceInitSetCharacteristics(arg1,arg2,arg3);
VOID  sdv_WdfDeviceInitSetCharacteristics(  
__in  PWDFDEVICE_INIT  DeviceInit,
   ULONG  DeviceCharacteristics,
   BOOLEAN  OrInValues);


#ifdef WdfDeviceInitSetFileObjectConfig
#undef WdfDeviceInitSetFileObjectConfig
#endif
#define WdfDeviceInitSetFileObjectConfig(arg1,arg2,arg3) sdv_WdfDeviceInitSetFileObjectConfig(arg1,arg2,arg3);
VOID  sdv_WdfDeviceInitSetFileObjectConfig(  
__in  PWDFDEVICE_INIT  DeviceInit,
__in  PWDF_FILEOBJECT_CONFIG  FileObjectConfig,
__in_opt  PWDF_OBJECT_ATTRIBUTES  FileObjectAttributes);


#ifdef WdfDeviceInitSetRequestAttributes
#undef WdfDeviceInitSetRequestAttributes
#endif
#define WdfDeviceInitSetRequestAttributes(arg1,arg2) sdv_WdfDeviceInitSetRequestAttributes(arg1,arg2);
VOID  sdv_WdfDeviceInitSetRequestAttributes(  
__in  PWDFDEVICE_INIT  DeviceInit,
__in  PWDF_OBJECT_ATTRIBUTES  RequestAttributes);


#ifdef WdfDeviceInitAssignWdmIrpPreprocessCallback
#undef WdfDeviceInitAssignWdmIrpPreprocessCallback
#endif
#define WdfDeviceInitAssignWdmIrpPreprocessCallback sdv_WdfDeviceInitAssignWdmIrpPreprocessCallback
NTSTATUS  sdv_WdfDeviceInitAssignWdmIrpPreprocessCallback(  
__in  PWDFDEVICE_INIT  DeviceInit,
__in  PFN_WDFDEVICE_WDM_IRP_PREPROCESS  EvtDevicePreprocessWdmIrp,
   UCHAR  MajorFunction,
   PUCHAR  MinorFunctions,
   ULONG  NumMinorFunctions);


#ifdef WdfDeviceInitSetIoInCallerContextCallback
#undef WdfDeviceInitSetIoInCallerContextCallback
#endif
#define WdfDeviceInitSetIoInCallerContextCallback(arg1,arg2) sdv_WdfDeviceInitSetIoInCallerContextCallback(arg1,arg2);
VOID  sdv_WdfDeviceInitSetIoInCallerContextCallback(  
__in  PWDFDEVICE_INIT  DeviceInit,
__in  PFN_WDF_IO_IN_CALLER_CONTEXT  EvtIoInCallerContext);


#ifdef WdfDeviceCreate
#undef WdfDeviceCreate
#endif
#define WdfDeviceCreate sdv_WdfDeviceCreate
NTSTATUS  sdv_WdfDeviceCreate(  
__inout  PWDFDEVICE_INIT*  DeviceInit,
__in_opt  PWDF_OBJECT_ATTRIBUTES  DeviceAttributes,
__out  WDFDEVICE*  Device);


#ifdef WdfDeviceSetStaticStopRemove
#undef WdfDeviceSetStaticStopRemove
#endif
#define WdfDeviceSetStaticStopRemove(arg1,arg2) sdv_WdfDeviceSetStaticStopRemove(arg1,arg2);
VOID  sdv_WdfDeviceSetStaticStopRemove(  
__in  WDFDEVICE  Device,
   BOOLEAN  Stoppable);


#ifdef WdfDeviceCreateDeviceInterface
#undef WdfDeviceCreateDeviceInterface
#endif
#define WdfDeviceCreateDeviceInterface sdv_WdfDeviceCreateDeviceInterface
NTSTATUS  sdv_WdfDeviceCreateDeviceInterface(  
IN WDFDEVICE  Device,
IN CONST GUID*  InterfaceClassGUID,
IN OPTIONAL PCUNICODE_STRING  ReferenceString);


#ifdef WdfDeviceSetDeviceInterfaceState
#undef WdfDeviceSetDeviceInterfaceState
#endif
#define WdfDeviceSetDeviceInterfaceState(arg1,arg2,arg3,arg4) sdv_WdfDeviceSetDeviceInterfaceState(arg1,arg2,arg3,arg4);
VOID  sdv_WdfDeviceSetDeviceInterfaceState(  
__in  WDFDEVICE  Device,
__in  GUID*  InterfaceClassGUID,
__in_opt  PCUNICODE_STRING  ReferenceString,
   BOOLEAN  IsInterfaceEnabled);


#ifdef WdfDeviceRetrieveDeviceInterfaceString
#undef WdfDeviceRetrieveDeviceInterfaceString
#endif
#define WdfDeviceRetrieveDeviceInterfaceString sdv_WdfDeviceRetrieveDeviceInterfaceString
NTSTATUS  sdv_WdfDeviceRetrieveDeviceInterfaceString(  
__in  WDFDEVICE  Device,
__in  GUID*  InterfaceClassGUID,
__in_opt  PCUNICODE_STRING  ReferenceString,
__in  WDFSTRING  String);


#ifdef WdfDeviceCreateSymbolicLink
#undef WdfDeviceCreateSymbolicLink
#endif
#define WdfDeviceCreateSymbolicLink sdv_WdfDeviceCreateSymbolicLink
NTSTATUS  sdv_WdfDeviceCreateSymbolicLink(  
__in  WDFDEVICE  Device,
__in  PCUNICODE_STRING  SymbolicLinkName);


#ifdef WdfDeviceQueryProperty
#undef WdfDeviceQueryProperty
#endif
#define WdfDeviceQueryProperty sdv_WdfDeviceQueryProperty
NTSTATUS  sdv_WdfDeviceQueryProperty(  
__in  WDFDEVICE  Device,
   DEVICE_REGISTRY_PROPERTY  DeviceProperty,
   ULONG  BufferLength,
__out_bcount_full(BufferLength)  PVOID  PropertyBuffer,
__out  PULONG  ResultLength);


#ifdef WdfDeviceAllocAndQueryProperty
#undef WdfDeviceAllocAndQueryProperty
#endif
#define WdfDeviceAllocAndQueryProperty sdv_WdfDeviceAllocAndQueryProperty
NTSTATUS  sdv_WdfDeviceAllocAndQueryProperty(  
__in  WDFDEVICE  Device,
   DEVICE_REGISTRY_PROPERTY  DeviceProperty,
   POOL_TYPE  PoolType,
__in_opt  PWDF_OBJECT_ATTRIBUTES  PropertyMemoryAttributes,
__out  WDFMEMORY*  PropertyMemory);


#ifdef WdfDeviceSetPnpCapabilities
#undef WdfDeviceSetPnpCapabilities
#endif
#define WdfDeviceSetPnpCapabilities(arg1,arg2) sdv_WdfDeviceSetPnpCapabilities(arg1,arg2);
VOID  sdv_WdfDeviceSetPnpCapabilities(  
__in  WDFDEVICE  Device,
__in  PWDF_DEVICE_PNP_CAPABILITIES  PnpCapabilities);


#ifdef WdfDeviceSetPowerCapabilities
#undef WdfDeviceSetPowerCapabilities
#endif
#define WdfDeviceSetPowerCapabilities(arg1,arg2) sdv_WdfDeviceSetPowerCapabilities(arg1,arg2);
VOID  sdv_WdfDeviceSetPowerCapabilities(  
__in  WDFDEVICE  Device,
__in  PWDF_DEVICE_POWER_CAPABILITIES  PowerCapabilities);


#ifdef WdfDeviceSetBusInformationForChildren
#undef WdfDeviceSetBusInformationForChildren
#endif
#define WdfDeviceSetBusInformationForChildren(arg1,arg2) sdv_WdfDeviceSetBusInformationForChildren(arg1,arg2);
VOID  sdv_WdfDeviceSetBusInformationForChildren(  
__in  WDFDEVICE  Device,
__in  PPNP_BUS_INFORMATION  BusInformation);


#ifdef WdfDeviceIndicateWakeStatus
#undef WdfDeviceIndicateWakeStatus
#endif
#define WdfDeviceIndicateWakeStatus sdv_WdfDeviceIndicateWakeStatus
NTSTATUS  sdv_WdfDeviceIndicateWakeStatus(  
__in  WDFDEVICE  Device,
   NTSTATUS  WaitWakeStatus);


#ifdef WdfDeviceSetFailed
#undef WdfDeviceSetFailed
#endif
#define WdfDeviceSetFailed(arg1,arg2) sdv_WdfDeviceSetFailed(arg1,arg2);
VOID  sdv_WdfDeviceSetFailed(  
__in  WDFDEVICE  Device,
   WDF_DEVICE_FAILED_ACTION  FailedAction);


#ifdef WdfDeviceStopIdle
#undef WdfDeviceStopIdle
#endif
#define WdfDeviceStopIdle sdv_WdfDeviceStopIdle
NTSTATUS  sdv_WdfDeviceStopIdle(  
__in  WDFDEVICE  Device,
   BOOLEAN  WaitForD0);


#ifdef WdfDeviceResumeIdle
#undef WdfDeviceResumeIdle
#endif
#define WdfDeviceResumeIdle(arg1) sdv_WdfDeviceResumeIdle(arg1);
VOID  sdv_WdfDeviceResumeIdle(  
__in  WDFDEVICE  Device);


#ifdef WdfDeviceGetFileObject
#undef WdfDeviceGetFileObject
#endif
#define WdfDeviceGetFileObject sdv_WdfDeviceGetFileObject
WDFFILEOBJECT  sdv_WdfDeviceGetFileObject(  
__in  WDFDEVICE  Device,
__in  PFILE_OBJECT  FileObject);


#ifdef WdfDeviceEnqueueRequest
#undef WdfDeviceEnqueueRequest
#endif
#define WdfDeviceEnqueueRequest sdv_WdfDeviceEnqueueRequest
NTSTATUS  sdv_WdfDeviceEnqueueRequest(  
__in  WDFDEVICE  Device,
__in  WDFREQUEST  Request);


#ifdef WdfDeviceGetDefaultQueue
#undef WdfDeviceGetDefaultQueue
#endif
#define WdfDeviceGetDefaultQueue sdv_WdfDeviceGetDefaultQueue
WDFQUEUE  sdv_WdfDeviceGetDefaultQueue(  
__in  WDFDEVICE  Device);


#ifdef WdfDeviceConfigureRequestDispatching
#undef WdfDeviceConfigureRequestDispatching
#endif
#define WdfDeviceConfigureRequestDispatching sdv_WdfDeviceConfigureRequestDispatching
NTSTATUS  sdv_WdfDeviceConfigureRequestDispatching(  
__in  WDFDEVICE  Device,
__in  WDFQUEUE  Queue,
   WDF_REQUEST_TYPE  RequestType);


#ifdef WdfDeviceGetSystemPowerAction
#undef WdfDeviceGetSystemPowerAction
#endif
#define WdfDeviceGetSystemPowerAction sdv_WdfDeviceGetSystemPowerAction
POWER_ACTION  sdv_WdfDeviceGetSystemPowerAction(  
__in  WDFDEVICE  Device);


#ifdef WdfDmaEnablerCreate
#undef WdfDmaEnablerCreate
#endif
#define WdfDmaEnablerCreate sdv_WdfDmaEnablerCreate
NTSTATUS  sdv_WdfDmaEnablerCreate(  
__in  WDFDEVICE  Device,
__in  PWDF_DMA_ENABLER_CONFIG  Config,
__in_opt  PWDF_OBJECT_ATTRIBUTES  Attributes,
__out  WDFDMAENABLER*  DmaEnablerHandle);


#ifdef WdfDmaEnablerGetMaximumLength
#undef WdfDmaEnablerGetMaximumLength
#endif
#define WdfDmaEnablerGetMaximumLength sdv_WdfDmaEnablerGetMaximumLength
size_t  sdv_WdfDmaEnablerGetMaximumLength(  
__in  WDFDMAENABLER  DmaEnabler);


#ifdef WdfDmaEnablerGetMaximumScatterGatherElements
#undef WdfDmaEnablerGetMaximumScatterGatherElements
#endif
#define WdfDmaEnablerGetMaximumScatterGatherElements sdv_WdfDmaEnablerGetMaximumScatterGatherElements
size_t  sdv_WdfDmaEnablerGetMaximumScatterGatherElements(  
__in  WDFDMAENABLER  DmaEnabler);


#ifdef WdfDmaEnablerSetMaximumScatterGatherElements
#undef WdfDmaEnablerSetMaximumScatterGatherElements
#endif
#define WdfDmaEnablerSetMaximumScatterGatherElements(arg1,arg2) sdv_WdfDmaEnablerSetMaximumScatterGatherElements(arg1,arg2);
VOID  sdv_WdfDmaEnablerSetMaximumScatterGatherElements(  
__in  WDFDMAENABLER  DmaEnabler,
   size_t  MaximumFragments);


#ifdef WdfDmaEnablerGetFragmentLength
#undef WdfDmaEnablerGetFragmentLength
#endif
#define WdfDmaEnablerGetFragmentLength sdv_WdfDmaEnablerGetFragmentLength
size_t  sdv_WdfDmaEnablerGetFragmentLength(  
__in  WDFDMAENABLER  DmaEnabler,
   WDF_DMA_DIRECTION  DmaDirection);


#ifdef WdfDmaEnablerWdmGetDmaAdapter
#undef WdfDmaEnablerWdmGetDmaAdapter
#endif
#define WdfDmaEnablerWdmGetDmaAdapter sdv_WdfDmaEnablerWdmGetDmaAdapter
PDMA_ADAPTER  sdv_WdfDmaEnablerWdmGetDmaAdapter(  
__in  WDFDMAENABLER  DmaEnabler,
   WDF_DMA_DIRECTION  DmaDirection);


#ifdef WdfDmaTransactionCreate
#undef WdfDmaTransactionCreate
#endif
#define WdfDmaTransactionCreate sdv_WdfDmaTransactionCreate
NTSTATUS  sdv_WdfDmaTransactionCreate(  
__in  WDFDMAENABLER  DmaEnabler,
__in_opt  PWDF_OBJECT_ATTRIBUTES  Attributes,
__out  WDFDMATRANSACTION*  DmaTransaction);


#ifdef WdfDmaTransactionInitialize
#undef WdfDmaTransactionInitialize
#endif
#define WdfDmaTransactionInitialize sdv_WdfDmaTransactionInitialize
NTSTATUS  sdv_WdfDmaTransactionInitialize(  
__in  WDFDMATRANSACTION  DmaTransaction,
__in  PFN_WDF_PROGRAM_DMA  EvtProgramDmaFunction,
   WDF_DMA_DIRECTION  DmaDirection,
__in  PMDL  Mdl,
__in  PVOID  VirtualAddress,
   size_t  Length);


#ifdef WdfDmaTransactionInitializeUsingRequest
#undef WdfDmaTransactionInitializeUsingRequest
#endif
#define WdfDmaTransactionInitializeUsingRequest sdv_WdfDmaTransactionInitializeUsingRequest
NTSTATUS  sdv_WdfDmaTransactionInitializeUsingRequest(  
__in  WDFDMATRANSACTION  DmaTransaction,
__in  WDFREQUEST  Request,
__in  PFN_WDF_PROGRAM_DMA  EvtProgramDmaFunction,
   WDF_DMA_DIRECTION  DmaDirection);


#ifdef WdfDmaTransactionExecute
#undef WdfDmaTransactionExecute
#endif
#define WdfDmaTransactionExecute sdv_WdfDmaTransactionExecute
NTSTATUS  sdv_WdfDmaTransactionExecute(  
__in  WDFDMATRANSACTION  DmaTransaction,
__in_opt  WDFCONTEXT  Context);


#ifdef WdfDmaTransactionRelease
#undef WdfDmaTransactionRelease
#endif
#define WdfDmaTransactionRelease sdv_WdfDmaTransactionRelease
NTSTATUS  sdv_WdfDmaTransactionRelease(  
__in  WDFDMATRANSACTION  DmaTransaction);


#ifdef WdfDmaTransactionDmaCompleted
#undef WdfDmaTransactionDmaCompleted
#endif
#define WdfDmaTransactionDmaCompleted sdv_WdfDmaTransactionDmaCompleted
BOOLEAN  sdv_WdfDmaTransactionDmaCompleted(  
__in  WDFDMATRANSACTION  DmaTransaction,
__out  NTSTATUS*  Status);


#ifdef WdfDmaTransactionDmaCompletedWithLength
#undef WdfDmaTransactionDmaCompletedWithLength
#endif
#define WdfDmaTransactionDmaCompletedWithLength sdv_WdfDmaTransactionDmaCompletedWithLength
BOOLEAN  sdv_WdfDmaTransactionDmaCompletedWithLength(  
__in  WDFDMATRANSACTION  DmaTransaction,
   size_t  TransferredLength,
__out  NTSTATUS*  Status);


#ifdef WdfDmaTransactionDmaCompletedFinal
#undef WdfDmaTransactionDmaCompletedFinal
#endif
#define WdfDmaTransactionDmaCompletedFinal sdv_WdfDmaTransactionDmaCompletedFinal
BOOLEAN  sdv_WdfDmaTransactionDmaCompletedFinal(  
__in  WDFDMATRANSACTION  DmaTransaction,
   size_t  FinalTransferredLength,
__out  NTSTATUS*  Status);


#ifdef WdfDmaTransactionGetBytesTransferred
#undef WdfDmaTransactionGetBytesTransferred
#endif
#define WdfDmaTransactionGetBytesTransferred sdv_WdfDmaTransactionGetBytesTransferred
size_t  sdv_WdfDmaTransactionGetBytesTransferred(  
__in  WDFDMATRANSACTION  DmaTransaction);


#ifdef WdfDmaTransactionSetMaximumLength
#undef WdfDmaTransactionSetMaximumLength
#endif
#define WdfDmaTransactionSetMaximumLength(arg1,arg2) sdv_WdfDmaTransactionSetMaximumLength(arg1,arg2);
VOID  sdv_WdfDmaTransactionSetMaximumLength(  
__in  WDFDMATRANSACTION  DmaTransaction,
   size_t  MaximumLength);


#ifdef WdfDmaTransactionGetRequest
#undef WdfDmaTransactionGetRequest
#endif
#define WdfDmaTransactionGetRequest sdv_WdfDmaTransactionGetRequest
WDFREQUEST  sdv_WdfDmaTransactionGetRequest(  
__in  WDFDMATRANSACTION  DmaTransaction);


#ifdef WdfDmaTransactionGetCurrentDmaTransferLength
#undef WdfDmaTransactionGetCurrentDmaTransferLength
#endif
#define WdfDmaTransactionGetCurrentDmaTransferLength sdv_WdfDmaTransactionGetCurrentDmaTransferLength
size_t  sdv_WdfDmaTransactionGetCurrentDmaTransferLength(  
__in  WDFDMATRANSACTION  DmaTransaction);


#ifdef WdfDmaTransactionGetDevice
#undef WdfDmaTransactionGetDevice
#endif
#define WdfDmaTransactionGetDevice sdv_WdfDmaTransactionGetDevice
WDFDEVICE  sdv_WdfDmaTransactionGetDevice(  
__in  WDFDMATRANSACTION  DmaTransaction);


#ifdef WdfDpcCreate
#undef WdfDpcCreate
#endif
#define WdfDpcCreate sdv_WdfDpcCreate
NTSTATUS  sdv_WdfDpcCreate(  
__in  PWDF_DPC_CONFIG  Config,
__in  PWDF_OBJECT_ATTRIBUTES  Attributes,
__out  WDFDPC*  Dpc);


#ifdef WdfDpcEnqueue
#undef WdfDpcEnqueue
#endif
#define WdfDpcEnqueue sdv_WdfDpcEnqueue
BOOLEAN  sdv_WdfDpcEnqueue(  
__in  WDFDPC  Dpc);


#ifdef WdfDpcCancel
#undef WdfDpcCancel
#endif
#define WdfDpcCancel sdv_WdfDpcCancel
BOOLEAN  sdv_WdfDpcCancel(  
__in  WDFDPC  Dpc,
   BOOLEAN  Wait);


#ifdef WdfDpcGetParentObject
#undef WdfDpcGetParentObject
#endif
#define WdfDpcGetParentObject sdv_WdfDpcGetParentObject
WDFOBJECT  sdv_WdfDpcGetParentObject(  
__in  WDFDPC  Dpc);


#ifdef WdfDpcWdmGetDpc
#undef WdfDpcWdmGetDpc
#endif
#define WdfDpcWdmGetDpc sdv_WdfDpcWdmGetDpc
PKDPC  sdv_WdfDpcWdmGetDpc(  
__in  WDFDPC  Dpc);


#ifdef WdfDriverCreate
#undef WdfDriverCreate
#endif
#define WdfDriverCreate sdv_WdfDriverCreate
NTSTATUS  sdv_WdfDriverCreate(  
__in  PDRIVER_OBJECT  DriverObject,
__in  PCUNICODE_STRING  RegistryPath,
__in_opt  PWDF_OBJECT_ATTRIBUTES  DriverAttributes,
__in  PWDF_DRIVER_CONFIG  DriverConfig,
__out_opt  WDFDRIVER*  Driver);


#ifdef WdfDriverGetRegistryPath
#undef WdfDriverGetRegistryPath
#endif
#define WdfDriverGetRegistryPath sdv_WdfDriverGetRegistryPath
PWSTR  sdv_WdfDriverGetRegistryPath(  
__in  WDFDRIVER  Driver);


#ifdef WdfDriverWdmGetDriverObject
#undef WdfDriverWdmGetDriverObject
#endif
#define WdfDriverWdmGetDriverObject sdv_WdfDriverWdmGetDriverObject
PDRIVER_OBJECT  sdv_WdfDriverWdmGetDriverObject(  
__in  WDFDRIVER  Driver);


#ifdef WdfDriverOpenParametersRegistryKey
#undef WdfDriverOpenParametersRegistryKey
#endif
#define WdfDriverOpenParametersRegistryKey sdv_WdfDriverOpenParametersRegistryKey
NTSTATUS  sdv_WdfDriverOpenParametersRegistryKey(  
__in  WDFDRIVER  Driver,
   ACCESS_MASK  DesiredAccess,
__in_opt  PWDF_OBJECT_ATTRIBUTES  KeyAttributes,
__out  WDFKEY*  Key);


#ifdef WdfWdmDriverGetWdfDriverHandle
#undef WdfWdmDriverGetWdfDriverHandle
#endif
#define WdfWdmDriverGetWdfDriverHandle sdv_WdfWdmDriverGetWdfDriverHandle
WDFDRIVER  sdv_WdfWdmDriverGetWdfDriverHandle(  
__in  PDRIVER_OBJECT  DriverObject);


#ifdef WdfDriverRegisterTraceInfo
#undef WdfDriverRegisterTraceInfo
#endif
#define WdfDriverRegisterTraceInfo sdv_WdfDriverRegisterTraceInfo
NTSTATUS  sdv_WdfDriverRegisterTraceInfo(  
__in  PDRIVER_OBJECT  DriverObject,
__in  PFN_WDF_TRACE_CALLBACK  EvtTraceCallback,
__in  PVOID  ControlBlock);


#ifdef WdfDriverRetrieveVersionString
#undef WdfDriverRetrieveVersionString
#endif
#define WdfDriverRetrieveVersionString sdv_WdfDriverRetrieveVersionString
NTSTATUS  sdv_WdfDriverRetrieveVersionString(  
__in  WDFDRIVER  Driver,
__in  WDFSTRING  String);


#ifdef WdfDriverIsVersionAvailable
#undef WdfDriverIsVersionAvailable
#endif
#define WdfDriverIsVersionAvailable sdv_WdfDriverIsVersionAvailable
BOOLEAN  sdv_WdfDriverIsVersionAvailable(  
__in  WDFDRIVER  Driver,
__in  PWDF_DRIVER_VERSION_AVAILABLE_PARAMS  VersionAvailableParams);


#ifdef WdfFdoInitWdmGetPhysicalDevice
#undef WdfFdoInitWdmGetPhysicalDevice
#endif
#define WdfFdoInitWdmGetPhysicalDevice sdv_WdfFdoInitWdmGetPhysicalDevice
PDEVICE_OBJECT  sdv_WdfFdoInitWdmGetPhysicalDevice(  
__in  PWDFDEVICE_INIT  DeviceInit);


#ifdef WdfFdoInitOpenRegistryKey
#undef WdfFdoInitOpenRegistryKey
#endif
#define WdfFdoInitOpenRegistryKey sdv_WdfFdoInitOpenRegistryKey
NTSTATUS  sdv_WdfFdoInitOpenRegistryKey(  
__in  PWDFDEVICE_INIT  DeviceInit,
   ULONG  DeviceInstanceKeyType,
   ACCESS_MASK  DesiredAccess,
__in_opt  PWDF_OBJECT_ATTRIBUTES  KeyAttributes,
__out  WDFKEY*  Key);


#ifdef WdfFdoInitQueryProperty
#undef WdfFdoInitQueryProperty
#endif
#define WdfFdoInitQueryProperty sdv_WdfFdoInitQueryProperty
NTSTATUS  sdv_WdfFdoInitQueryProperty(  
__in  PWDFDEVICE_INIT  DeviceInit,
   DEVICE_REGISTRY_PROPERTY  DeviceProperty,
   ULONG  BufferLength,
__out_bcount_full(BufferLength)  PVOID  PropertyBuffer,
__out  PULONG  ResultLength);


#ifdef WdfFdoInitAllocAndQueryProperty
#undef WdfFdoInitAllocAndQueryProperty
#endif
#define WdfFdoInitAllocAndQueryProperty sdv_WdfFdoInitAllocAndQueryProperty
NTSTATUS  sdv_WdfFdoInitAllocAndQueryProperty(  
__in  PWDFDEVICE_INIT  DeviceInit,
   DEVICE_REGISTRY_PROPERTY  DeviceProperty,
   POOL_TYPE  PoolType,
__in_opt  PWDF_OBJECT_ATTRIBUTES  PropertyMemoryAttributes,
__out  WDFMEMORY*  PropertyMemory);


#ifdef WdfFdoInitSetEventCallbacks
#undef WdfFdoInitSetEventCallbacks
#endif
#define WdfFdoInitSetEventCallbacks(arg1,arg2) sdv_WdfFdoInitSetEventCallbacks(arg1,arg2);
VOID  sdv_WdfFdoInitSetEventCallbacks(  
__in  PWDFDEVICE_INIT  DeviceInit,
__in  PWDF_FDO_EVENT_CALLBACKS  FdoEventCallbacks);


#ifdef WdfFdoInitSetFilter
#undef WdfFdoInitSetFilter
#endif
#define WdfFdoInitSetFilter(arg1) sdv_WdfFdoInitSetFilter(arg1);
VOID  sdv_WdfFdoInitSetFilter(  
__in  PWDFDEVICE_INIT  DeviceInit);


#ifdef WdfFdoInitSetDefaultChildListConfig
#undef WdfFdoInitSetDefaultChildListConfig
#endif
#define WdfFdoInitSetDefaultChildListConfig(arg1,arg2,arg3) sdv_WdfFdoInitSetDefaultChildListConfig(arg1,arg2,arg3);
VOID  sdv_WdfFdoInitSetDefaultChildListConfig(  
__inout  PWDFDEVICE_INIT  DeviceInit,
__in  PWDF_CHILD_LIST_CONFIG  Config,
__in_opt  PWDF_OBJECT_ATTRIBUTES  DefaultChildListAttributes);


#ifdef WdfFdoQueryForInterface
#undef WdfFdoQueryForInterface
#endif
#define WdfFdoQueryForInterface sdv_WdfFdoQueryForInterface
NTSTATUS  sdv_WdfFdoQueryForInterface(  
__in  WDFDEVICE  Fdo,
__in  LPCGUID  InterfaceType,
__out  PINTERFACE  Interface,
   USHORT  Size,
   USHORT  Version,
__in_opt  PVOID  InterfaceSpecificData);


#ifdef WdfFdoGetDefaultChildList
#undef WdfFdoGetDefaultChildList
#endif
#define WdfFdoGetDefaultChildList sdv_WdfFdoGetDefaultChildList
WDFCHILDLIST  sdv_WdfFdoGetDefaultChildList(  
__in  WDFDEVICE  Fdo);


#ifdef WdfFdoAddStaticChild
#undef WdfFdoAddStaticChild
#endif
#define WdfFdoAddStaticChild sdv_WdfFdoAddStaticChild
NTSTATUS  sdv_WdfFdoAddStaticChild(  
__in  WDFDEVICE  Fdo,
__in  WDFDEVICE  Child);


#ifdef WdfFdoLockStaticChildListForIteration
#undef WdfFdoLockStaticChildListForIteration
#endif
#define WdfFdoLockStaticChildListForIteration(arg1) sdv_WdfFdoLockStaticChildListForIteration(arg1);
VOID  sdv_WdfFdoLockStaticChildListForIteration(  
__in  WDFDEVICE  Fdo);


#ifdef WdfFdoRetrieveNextStaticChild
#undef WdfFdoRetrieveNextStaticChild
#endif
#define WdfFdoRetrieveNextStaticChild sdv_WdfFdoRetrieveNextStaticChild
WDFDEVICE  sdv_WdfFdoRetrieveNextStaticChild(  
__in  WDFDEVICE  Fdo,
__in_opt  WDFDEVICE  PreviousChild,
   ULONG  Flags);


#ifdef WdfFdoUnlockStaticChildListFromIteration
#undef WdfFdoUnlockStaticChildListFromIteration
#endif
#define WdfFdoUnlockStaticChildListFromIteration(arg1) sdv_WdfFdoUnlockStaticChildListFromIteration(arg1);
VOID  sdv_WdfFdoUnlockStaticChildListFromIteration(  
__in  WDFDEVICE  Fdo);


#ifdef WdfFileObjectGetFileName
#undef WdfFileObjectGetFileName
#endif
#define WdfFileObjectGetFileName sdv_WdfFileObjectGetFileName
PUNICODE_STRING  sdv_WdfFileObjectGetFileName(  
__in  WDFFILEOBJECT  FileObject);


#ifdef WdfFileObjectGetFlags
#undef WdfFileObjectGetFlags
#endif
#define WdfFileObjectGetFlags sdv_WdfFileObjectGetFlags
ULONG  sdv_WdfFileObjectGetFlags(  
__in  WDFFILEOBJECT  FileObject);


#ifdef WdfFileObjectGetDevice
#undef WdfFileObjectGetDevice
#endif
#define WdfFileObjectGetDevice sdv_WdfFileObjectGetDevice
WDFDEVICE  sdv_WdfFileObjectGetDevice(  
__in  WDFFILEOBJECT  FileObject);


#ifdef WdfFileObjectWdmGetFileObject
#undef WdfFileObjectWdmGetFileObject
#endif
#define WdfFileObjectWdmGetFileObject sdv_WdfFileObjectWdmGetFileObject
PFILE_OBJECT  sdv_WdfFileObjectWdmGetFileObject(  
__in  WDFFILEOBJECT  FileObject);


#ifdef WdfInterruptCreate
#undef WdfInterruptCreate
#endif
#define WdfInterruptCreate sdv_WdfInterruptCreate
NTSTATUS  sdv_WdfInterruptCreate(  
__in  WDFDEVICE  Device,
__in  PWDF_INTERRUPT_CONFIG  Configuration,
__in_opt  PWDF_OBJECT_ATTRIBUTES  Attributes,
__out  WDFINTERRUPT*  Interrupt);


#ifdef WdfInterruptQueueDpcForIsr
#undef WdfInterruptQueueDpcForIsr
#endif
#define WdfInterruptQueueDpcForIsr sdv_WdfInterruptQueueDpcForIsr
BOOLEAN  sdv_WdfInterruptQueueDpcForIsr(  
__in  WDFINTERRUPT  Interrupt);


#ifdef WdfInterruptSynchronize
#undef WdfInterruptSynchronize
#endif
#define WdfInterruptSynchronize sdv_WdfInterruptSynchronize
BOOLEAN  sdv_WdfInterruptSynchronize(  
__in  WDFINTERRUPT  Interrupt,
__in  PFN_WDF_INTERRUPT_SYNCHRONIZE  Callback,
__in  WDFCONTEXT  Context);


#ifdef WdfInterruptAcquireLock
#undef WdfInterruptAcquireLock
#endif
#define WdfInterruptAcquireLock(arg1) sdv_WdfInterruptAcquireLock(arg1);
VOID  sdv_WdfInterruptAcquireLock(  
__in  WDFINTERRUPT  Interrupt);


#ifdef WdfInterruptReleaseLock
#undef WdfInterruptReleaseLock
#endif
#define WdfInterruptReleaseLock(arg1) sdv_WdfInterruptReleaseLock(arg1);
VOID  sdv_WdfInterruptReleaseLock(  
__in  WDFINTERRUPT  Interrupt);


#ifdef WdfInterruptEnable
#undef WdfInterruptEnable
#endif
#define WdfInterruptEnable(arg1) sdv_WdfInterruptEnable(arg1);
VOID  sdv_WdfInterruptEnable(  
__in  WDFINTERRUPT  Interrupt);


#ifdef WdfInterruptDisable
#undef WdfInterruptDisable
#endif
#define WdfInterruptDisable(arg1) sdv_WdfInterruptDisable(arg1);
VOID  sdv_WdfInterruptDisable(  
__in  WDFINTERRUPT  Interrupt);


#ifdef WdfInterruptWdmGetInterrupt
#undef WdfInterruptWdmGetInterrupt
#endif
#define WdfInterruptWdmGetInterrupt sdv_WdfInterruptWdmGetInterrupt
PKINTERRUPT  sdv_WdfInterruptWdmGetInterrupt(  
__in  WDFINTERRUPT  Interrupt);


#ifdef WdfInterruptGetInfo
#undef WdfInterruptGetInfo
#endif
#define WdfInterruptGetInfo(arg1,arg2) sdv_WdfInterruptGetInfo(arg1,arg2);
VOID  sdv_WdfInterruptGetInfo(  
__in  WDFINTERRUPT  Interrupt,
__out  PWDF_INTERRUPT_INFO  Info);


#ifdef WdfInterruptSetPolicy
#undef WdfInterruptSetPolicy
#endif
#define WdfInterruptSetPolicy(arg1,arg2,arg3,arg4) sdv_WdfInterruptSetPolicy(arg1,arg2,arg3,arg4);
VOID  sdv_WdfInterruptSetPolicy(  
__in  WDFINTERRUPT  Interrupt,
   WDF_INTERRUPT_POLICY  Policy,
   WDF_INTERRUPT_PRIORITY  Priority,
   KAFFINITY  TargetProcessorSet);


#ifdef WdfInterruptGetDevice
#undef WdfInterruptGetDevice
#endif
#define WdfInterruptGetDevice sdv_WdfInterruptGetDevice
WDFDEVICE  sdv_WdfInterruptGetDevice(  
__in  WDFINTERRUPT  Interrupt);


#ifdef WdfIoQueueCreate
#undef WdfIoQueueCreate
#endif
#define WdfIoQueueCreate sdv_WdfIoQueueCreate
NTSTATUS  sdv_WdfIoQueueCreate(  
__in  WDFDEVICE  Device,
__in  PWDF_IO_QUEUE_CONFIG  Config,
__in_opt  PWDF_OBJECT_ATTRIBUTES  QueueAttributes,
__out_opt  WDFQUEUE*  Queue);


#ifdef WdfIoQueueGetState
#undef WdfIoQueueGetState
#endif
#define WdfIoQueueGetState sdv_WdfIoQueueGetState
WDF_IO_QUEUE_STATE  sdv_WdfIoQueueGetState(  
__in  WDFQUEUE  Queue,
__out_opt  PULONG  QueueRequests,
__out_opt  PULONG  DriverRequests);


#ifdef WdfIoQueueStart
#undef WdfIoQueueStart
#endif
#define WdfIoQueueStart(arg1) sdv_WdfIoQueueStart(arg1);
VOID  sdv_WdfIoQueueStart(  
__in  WDFQUEUE  Queue);


#ifdef WdfIoQueueStop
#undef WdfIoQueueStop
#endif
#define WdfIoQueueStop(arg1,arg2,arg3) sdv_WdfIoQueueStop(arg1,arg2,arg3);
VOID  sdv_WdfIoQueueStop(  
__in  WDFQUEUE  Queue,
   PFN_WDF_IO_QUEUE_STATE  StopComplete,
   WDFCONTEXT  Context);


#ifdef WdfIoQueueStopSynchronously
#undef WdfIoQueueStopSynchronously
#endif
#define WdfIoQueueStopSynchronously(arg1) sdv_WdfIoQueueStopSynchronously(arg1);
VOID  sdv_WdfIoQueueStopSynchronously(  
__in  WDFQUEUE  Queue);


#ifdef WdfIoQueueGetDevice
#undef WdfIoQueueGetDevice
#endif
#define WdfIoQueueGetDevice sdv_WdfIoQueueGetDevice
WDFDEVICE  sdv_WdfIoQueueGetDevice(  
__in  WDFQUEUE  Queue);


#ifdef WdfIoQueueRetrieveNextRequest
#undef WdfIoQueueRetrieveNextRequest
#endif
#define WdfIoQueueRetrieveNextRequest sdv_WdfIoQueueRetrieveNextRequest
NTSTATUS  sdv_WdfIoQueueRetrieveNextRequest(  
__in  WDFQUEUE  Queue,
__out  WDFREQUEST*  OutRequest);


#ifdef WdfIoQueueRetrieveRequestByFileObject
#undef WdfIoQueueRetrieveRequestByFileObject
#endif
#define WdfIoQueueRetrieveRequestByFileObject sdv_WdfIoQueueRetrieveRequestByFileObject
NTSTATUS  sdv_WdfIoQueueRetrieveRequestByFileObject(  
__in  WDFQUEUE  Queue,
__in  WDFFILEOBJECT  FileObject,
__out  WDFREQUEST*  OutRequest);


#ifdef WdfIoQueueFindRequest
#undef WdfIoQueueFindRequest
#endif
#define WdfIoQueueFindRequest sdv_WdfIoQueueFindRequest
NTSTATUS  sdv_WdfIoQueueFindRequest(  
__in  WDFQUEUE  Queue,
__in_opt  WDFREQUEST  FoundRequest,
__in_opt  WDFFILEOBJECT  FileObject,
__inout_opt  PWDF_REQUEST_PARAMETERS  Parameters,
__out  WDFREQUEST*  OutRequest);


#ifdef WdfIoQueueRetrieveFoundRequest
#undef WdfIoQueueRetrieveFoundRequest
#endif
#define WdfIoQueueRetrieveFoundRequest sdv_WdfIoQueueRetrieveFoundRequest
NTSTATUS  sdv_WdfIoQueueRetrieveFoundRequest(  
__in  WDFQUEUE  Queue,
__in  WDFREQUEST  FoundRequest,
__out  WDFREQUEST*  OutRequest);


#ifdef WdfIoQueueDrainSynchronously
#undef WdfIoQueueDrainSynchronously
#endif
#define WdfIoQueueDrainSynchronously(arg1) sdv_WdfIoQueueDrainSynchronously(arg1);
VOID  sdv_WdfIoQueueDrainSynchronously(  
__in  WDFQUEUE  Queue);


#ifdef WdfIoQueueDrain
#undef WdfIoQueueDrain
#endif
#define WdfIoQueueDrain(arg1,arg2,arg3) sdv_WdfIoQueueDrain(arg1,arg2,arg3);
VOID  sdv_WdfIoQueueDrain(  
__in  WDFQUEUE  Queue,
   PFN_WDF_IO_QUEUE_STATE  DrainComplete,
   WDFCONTEXT  Context);


#ifdef WdfIoQueuePurgeSynchronously
#undef WdfIoQueuePurgeSynchronously
#endif
#define WdfIoQueuePurgeSynchronously(arg1) sdv_WdfIoQueuePurgeSynchronously(arg1);
VOID  sdv_WdfIoQueuePurgeSynchronously(  
__in  WDFQUEUE  Queue);


#ifdef WdfIoQueuePurge
#undef WdfIoQueuePurge
#endif
#define WdfIoQueuePurge(arg1,arg2,arg3) sdv_WdfIoQueuePurge(arg1,arg2,arg3);
VOID  sdv_WdfIoQueuePurge(  
__in  WDFQUEUE  Queue,
   PFN_WDF_IO_QUEUE_STATE  PurgeComplete,
   WDFCONTEXT  Context);


#ifdef WdfIoQueueReadyNotify
#undef WdfIoQueueReadyNotify
#endif
#define WdfIoQueueReadyNotify sdv_WdfIoQueueReadyNotify
NTSTATUS  sdv_WdfIoQueueReadyNotify(  
__in  WDFQUEUE  Queue,
__in_opt  PFN_WDF_IO_QUEUE_STATE  QueueReady,
__in_opt  WDFCONTEXT  Context);


#ifdef WdfIoTargetCreate
#undef WdfIoTargetCreate
#endif
#define WdfIoTargetCreate sdv_WdfIoTargetCreate
NTSTATUS  sdv_WdfIoTargetCreate(  
__in  WDFDEVICE  Device,
__in_opt  PWDF_OBJECT_ATTRIBUTES  IoTargetAttributes,
__out  WDFIOTARGET*  IoTarget);


#ifdef WdfIoTargetOpen
#undef WdfIoTargetOpen
#endif
#define WdfIoTargetOpen sdv_WdfIoTargetOpen
NTSTATUS  sdv_WdfIoTargetOpen(  
__in  WDFIOTARGET  IoTarget,
__in  PWDF_IO_TARGET_OPEN_PARAMS  OpenParams);


#ifdef WdfIoTargetCloseForQueryRemove
#undef WdfIoTargetCloseForQueryRemove
#endif
#define WdfIoTargetCloseForQueryRemove(arg1) sdv_WdfIoTargetCloseForQueryRemove(arg1);
VOID  sdv_WdfIoTargetCloseForQueryRemove(  
__in  WDFIOTARGET  IoTarget);


#ifdef WdfIoTargetClose
#undef WdfIoTargetClose
#endif
#define WdfIoTargetClose(arg1) sdv_WdfIoTargetClose(arg1);
VOID  sdv_WdfIoTargetClose(  
__in  WDFIOTARGET  IoTarget);


#ifdef WdfIoTargetStart
#undef WdfIoTargetStart
#endif
#define WdfIoTargetStart sdv_WdfIoTargetStart
NTSTATUS  sdv_WdfIoTargetStart(  
__in  WDFIOTARGET  IoTarget);


#ifdef WdfIoTargetStop
#undef WdfIoTargetStop
#endif
#define WdfIoTargetStop(arg1,arg2) sdv_WdfIoTargetStop(arg1,arg2);
VOID  sdv_WdfIoTargetStop(  
__in  WDFIOTARGET  IoTarget,
   WDF_IO_TARGET_SENT_IO_ACTION  Action);


#ifdef WdfIoTargetGetState
#undef WdfIoTargetGetState
#endif
#define WdfIoTargetGetState sdv_WdfIoTargetGetState
WDF_IO_TARGET_STATE  sdv_WdfIoTargetGetState(  
__in  WDFIOTARGET  IoTarget);


#ifdef WdfIoTargetGetDevice
#undef WdfIoTargetGetDevice
#endif
#define WdfIoTargetGetDevice sdv_WdfIoTargetGetDevice
WDFDEVICE  sdv_WdfIoTargetGetDevice(  
__in  WDFIOTARGET  IoTarget);


#ifdef WdfIoTargetQueryTargetProperty
#undef WdfIoTargetQueryTargetProperty
#endif
#define WdfIoTargetQueryTargetProperty sdv_WdfIoTargetQueryTargetProperty
NTSTATUS  sdv_WdfIoTargetQueryTargetProperty(  
__in  WDFIOTARGET  IoTarget,
   DEVICE_REGISTRY_PROPERTY  DeviceProperty,
   ULONG  BufferLength,
__out_bcount_full(BufferLength)  PVOID  PropertyBuffer,
__out  PULONG  ResultLength);


#ifdef WdfIoTargetAllocAndQueryTargetProperty
#undef WdfIoTargetAllocAndQueryTargetProperty
#endif
#define WdfIoTargetAllocAndQueryTargetProperty sdv_WdfIoTargetAllocAndQueryTargetProperty
NTSTATUS  sdv_WdfIoTargetAllocAndQueryTargetProperty(  
__in  WDFIOTARGET  IoTarget,
   DEVICE_REGISTRY_PROPERTY  DeviceProperty,
   POOL_TYPE  PoolType,
__in_opt  PWDF_OBJECT_ATTRIBUTES  PropertyMemoryAttributes,
__out  WDFMEMORY*  PropertyMemory);


#ifdef WdfIoTargetQueryForInterface
#undef WdfIoTargetQueryForInterface
#endif
#define WdfIoTargetQueryForInterface sdv_WdfIoTargetQueryForInterface
NTSTATUS  sdv_WdfIoTargetQueryForInterface(  
__in  WDFIOTARGET  IoTarget,
__in  LPCGUID  InterfaceType,
__out  PINTERFACE  Interface,
   USHORT  Size,
   USHORT  Version,
__in_opt  PVOID  InterfaceSpecificData);


#ifdef WdfIoTargetWdmGetTargetDeviceObject
#undef WdfIoTargetWdmGetTargetDeviceObject
#endif
#define WdfIoTargetWdmGetTargetDeviceObject sdv_WdfIoTargetWdmGetTargetDeviceObject
PDEVICE_OBJECT  sdv_WdfIoTargetWdmGetTargetDeviceObject(  
__in  WDFIOTARGET  IoTarget);


#ifdef WdfIoTargetWdmGetTargetPhysicalDevice
#undef WdfIoTargetWdmGetTargetPhysicalDevice
#endif
#define WdfIoTargetWdmGetTargetPhysicalDevice sdv_WdfIoTargetWdmGetTargetPhysicalDevice
PDEVICE_OBJECT  sdv_WdfIoTargetWdmGetTargetPhysicalDevice(  
__in  WDFIOTARGET  IoTarget);


#ifdef WdfIoTargetWdmGetTargetFileObject
#undef WdfIoTargetWdmGetTargetFileObject
#endif
#define WdfIoTargetWdmGetTargetFileObject sdv_WdfIoTargetWdmGetTargetFileObject
PFILE_OBJECT  sdv_WdfIoTargetWdmGetTargetFileObject(  
__in  WDFIOTARGET  IoTarget);


#ifdef WdfIoTargetWdmGetTargetFileHandle
#undef WdfIoTargetWdmGetTargetFileHandle
#endif
#define WdfIoTargetWdmGetTargetFileHandle sdv_WdfIoTargetWdmGetTargetFileHandle
HANDLE  sdv_WdfIoTargetWdmGetTargetFileHandle(  
__in  WDFIOTARGET  IoTarget);


#ifdef WdfIoTargetSendReadSynchronously
#undef WdfIoTargetSendReadSynchronously
#endif
#define WdfIoTargetSendReadSynchronously sdv_WdfIoTargetSendReadSynchronously
NTSTATUS  sdv_WdfIoTargetSendReadSynchronously(  
__in  WDFIOTARGET  IoTarget,
__in_opt  WDFREQUEST  Request,
__in_opt  PWDF_MEMORY_DESCRIPTOR  OutputBuffer,
__in_opt  PLONGLONG  DeviceOffset,
__in_opt  PWDF_REQUEST_SEND_OPTIONS  RequestOptions,
__out_opt  PULONG_PTR  BytesRead);


#ifdef WdfIoTargetFormatRequestForRead
#undef WdfIoTargetFormatRequestForRead
#endif
#define WdfIoTargetFormatRequestForRead sdv_WdfIoTargetFormatRequestForRead
NTSTATUS  sdv_WdfIoTargetFormatRequestForRead(  
__in  WDFIOTARGET  IoTarget,
__in  WDFREQUEST  Request,
__in_opt  WDFMEMORY  OutputBuffer,
__in_opt  PWDFMEMORY_OFFSET  OutputBufferOffset,
__in_opt  PLONGLONG  DeviceOffset);


#ifdef WdfIoTargetSendWriteSynchronously
#undef WdfIoTargetSendWriteSynchronously
#endif
#define WdfIoTargetSendWriteSynchronously sdv_WdfIoTargetSendWriteSynchronously
NTSTATUS  sdv_WdfIoTargetSendWriteSynchronously(  
__in  WDFIOTARGET  IoTarget,
__in_opt  WDFREQUEST  Request,
__in_opt  PWDF_MEMORY_DESCRIPTOR  InputBuffer,
__in_opt  PLONGLONG  DeviceOffset,
__in_opt  PWDF_REQUEST_SEND_OPTIONS  RequestOptions,
__out_opt  PULONG_PTR  BytesWritten);


#ifdef WdfIoTargetFormatRequestForWrite
#undef WdfIoTargetFormatRequestForWrite
#endif
#define WdfIoTargetFormatRequestForWrite sdv_WdfIoTargetFormatRequestForWrite
NTSTATUS  sdv_WdfIoTargetFormatRequestForWrite(  
__in  WDFIOTARGET  IoTarget,
__in  WDFREQUEST  Request,
__in_opt  WDFMEMORY  InputBuffer,
__in_opt  PWDFMEMORY_OFFSET  InputBufferOffset,
__in_opt  PLONGLONG  DeviceOffset);


#ifdef WdfIoTargetSendIoctlSynchronously
#undef WdfIoTargetSendIoctlSynchronously
#endif
#define WdfIoTargetSendIoctlSynchronously sdv_WdfIoTargetSendIoctlSynchronously
NTSTATUS  sdv_WdfIoTargetSendIoctlSynchronously(  
__in  WDFIOTARGET  IoTarget,
__in_opt  WDFREQUEST  Request,
   ULONG  IoctlCode,
__in_opt  PWDF_MEMORY_DESCRIPTOR  InputBuffer,
__in_opt  PWDF_MEMORY_DESCRIPTOR  OutputBuffer,
__in_opt  PWDF_REQUEST_SEND_OPTIONS  RequestOptions,
__out_opt  PULONG_PTR  BytesReturned);


#ifdef WdfIoTargetFormatRequestForIoctl
#undef WdfIoTargetFormatRequestForIoctl
#endif
#define WdfIoTargetFormatRequestForIoctl sdv_WdfIoTargetFormatRequestForIoctl
NTSTATUS  sdv_WdfIoTargetFormatRequestForIoctl(  
__in  WDFIOTARGET  IoTarget,
__in  WDFREQUEST  Request,
   ULONG  IoctlCode,
__in_opt  WDFMEMORY  InputBuffer,
__in_opt  PWDFMEMORY_OFFSET  InputBufferOffset,
__in_opt  WDFMEMORY  OutputBuffer,
__in_opt  PWDFMEMORY_OFFSET  OutputBufferOffset);


#ifdef WdfIoTargetSendInternalIoctlSynchronously
#undef WdfIoTargetSendInternalIoctlSynchronously
#endif
#define WdfIoTargetSendInternalIoctlSynchronously sdv_WdfIoTargetSendInternalIoctlSynchronously
NTSTATUS  sdv_WdfIoTargetSendInternalIoctlSynchronously(  
__in  WDFIOTARGET  IoTarget,
__in_opt  WDFREQUEST  Request,
   ULONG  IoctlCode,
__in_opt  PWDF_MEMORY_DESCRIPTOR  InputBuffer,
__in_opt  PWDF_MEMORY_DESCRIPTOR  OutputBuffer,
__in_opt  PWDF_REQUEST_SEND_OPTIONS  RequestOptions,
__out_opt  PULONG_PTR  BytesReturned);


#ifdef WdfIoTargetFormatRequestForInternalIoctl
#undef WdfIoTargetFormatRequestForInternalIoctl
#endif
#define WdfIoTargetFormatRequestForInternalIoctl sdv_WdfIoTargetFormatRequestForInternalIoctl
NTSTATUS  sdv_WdfIoTargetFormatRequestForInternalIoctl(  
__in  WDFIOTARGET  IoTarget,
__in  WDFREQUEST  Request,
   ULONG  IoctlCode,
__in_opt  WDFMEMORY  InputBuffer,
__in_opt  PWDFMEMORY_OFFSET  InputBufferOffset,
__in_opt  WDFMEMORY  OutputBuffer,
__in_opt  PWDFMEMORY_OFFSET  OutputBufferOffset);


#ifdef WdfIoTargetSendInternalIoctlOthersSynchronously
#undef WdfIoTargetSendInternalIoctlOthersSynchronously
#endif
#define WdfIoTargetSendInternalIoctlOthersSynchronously sdv_WdfIoTargetSendInternalIoctlOthersSynchronously
NTSTATUS  sdv_WdfIoTargetSendInternalIoctlOthersSynchronously(  
  IN WDFIOTARGET  IoTarget,
  IN OPTIONAL  WDFREQUEST  Request,
  IN ULONG  IoctlCode,
  IN OPTIONAL  PWDF_MEMORY_DESCRIPTOR  OtherArg1,
  IN OPTIONAL  PWDF_MEMORY_DESCRIPTOR  OtherArg2,
  IN OPTIONAL  PWDF_MEMORY_DESCRIPTOR  OtherArg4,
  IN OPTIONAL  PWDF_REQUEST_SEND_OPTIONS  RequestOptions,
  OUT OPTIONAL  PULONG_PTR  BytesReturned);


#ifdef WdfIoTargetFormatRequestForInternalIoctlOthers
#undef WdfIoTargetFormatRequestForInternalIoctlOthers
#endif
#define WdfIoTargetFormatRequestForInternalIoctlOthers sdv_WdfIoTargetFormatRequestForInternalIoctlOthers
NTSTATUS  sdv_WdfIoTargetFormatRequestForInternalIoctlOthers(  
__in  WDFIOTARGET  IoTarget,
__in  WDFREQUEST  Request,
   ULONG  IoctlCode,
__in_opt  WDFMEMORY  OtherArg1,
__in_opt  PWDFMEMORY_OFFSET  OtherArg1Offset,
__in_opt  WDFMEMORY  OtherArg2,
__in_opt  PWDFMEMORY_OFFSET  OtherArg2Offset,
__in_opt  WDFMEMORY  OtherArg4,
__in_opt  PWDFMEMORY_OFFSET  OtherArg4Offset);


#ifdef WdfMemoryCreate
#undef WdfMemoryCreate
#endif
#define WdfMemoryCreate sdv_WdfMemoryCreate
NTSTATUS  sdv_WdfMemoryCreate(  
__in_opt  PWDF_OBJECT_ATTRIBUTES  Attributes,
   POOL_TYPE  PoolType,
   ULONG  PoolTag,
   size_t  BufferSize,
__out  WDFMEMORY*  Memory,
__out_opt  PVOID*  Buffer);


#ifdef WdfMemoryCreatePreallocated
#undef WdfMemoryCreatePreallocated
#endif
#define WdfMemoryCreatePreallocated sdv_WdfMemoryCreatePreallocated
NTSTATUS  sdv_WdfMemoryCreatePreallocated(  
__in_opt  PWDF_OBJECT_ATTRIBUTES  Attributes,
__in  PVOID  Buffer,
   size_t  BufferSize,
__out  WDFMEMORY*  Memory);


#ifdef WdfMemoryGetBuffer
#undef WdfMemoryGetBuffer
#endif
#define WdfMemoryGetBuffer sdv_WdfMemoryGetBuffer
PVOID  sdv_WdfMemoryGetBuffer(  
__in  WDFMEMORY  Memory,
__out_opt  size_t*  BufferSize);


#ifdef WdfMemoryAssignBuffer
#undef WdfMemoryAssignBuffer
#endif
#define WdfMemoryAssignBuffer sdv_WdfMemoryAssignBuffer
NTSTATUS  sdv_WdfMemoryAssignBuffer(  
__in  WDFMEMORY  Memory,
__in  PVOID  Buffer,
   size_t  BufferSize);


#ifdef WdfMemoryCopyToBuffer
#undef WdfMemoryCopyToBuffer
#endif
#define WdfMemoryCopyToBuffer sdv_WdfMemoryCopyToBuffer
NTSTATUS  sdv_WdfMemoryCopyToBuffer(  
__in  WDFMEMORY  SourceMemory,
   size_t  SourceOffset,
__out_bcount( NumBytesToCopyTo )  PVOID  Buffer,
   size_t  NumBytesToCopyTo);


#ifdef WdfMemoryCopyFromBuffer
#undef WdfMemoryCopyFromBuffer
#endif
#define WdfMemoryCopyFromBuffer sdv_WdfMemoryCopyFromBuffer
NTSTATUS  sdv_WdfMemoryCopyFromBuffer(  
__in  WDFMEMORY  DestinationMemory,
   size_t  DestinationOffset,
__in  PVOID  Buffer,
   size_t  NumBytesToCopyFrom);


#ifdef WdfLookasideListCreate
#undef WdfLookasideListCreate
#endif
#define WdfLookasideListCreate sdv_WdfLookasideListCreate
NTSTATUS  sdv_WdfLookasideListCreate(  
__in_opt  PWDF_OBJECT_ATTRIBUTES  LookasideAttributes,
   size_t  BufferSize,
   POOL_TYPE  PoolType,
__in_opt  PWDF_OBJECT_ATTRIBUTES  MemoryAttributes,
   ULONG  PoolTag,
__out  WDFLOOKASIDE*  Lookaside);


#ifdef WdfMemoryCreateFromLookaside
#undef WdfMemoryCreateFromLookaside
#endif
#define WdfMemoryCreateFromLookaside sdv_WdfMemoryCreateFromLookaside
NTSTATUS  sdv_WdfMemoryCreateFromLookaside(  
__in  WDFLOOKASIDE  Lookaside,
__out  WDFMEMORY*  Memory);


#ifdef WdfDeviceMiniportCreate
#undef WdfDeviceMiniportCreate
#endif
#define WdfDeviceMiniportCreate sdv_WdfDeviceMiniportCreate
NTSTATUS  sdv_WdfDeviceMiniportCreate(  
__in  WDFDRIVER  Driver,
__in_opt  PWDF_OBJECT_ATTRIBUTES  Attributes,
__in  PDEVICE_OBJECT  DeviceObject,
__in_opt  PDEVICE_OBJECT  AttachedDeviceObject,
__in_opt  PDEVICE_OBJECT  Pdo,
__out  WDFDEVICE*  Device);


#ifdef WdfDriverMiniportUnload
#undef WdfDriverMiniportUnload
#endif
#define WdfDriverMiniportUnload(arg1) sdv_WdfDriverMiniportUnload(arg1);
VOID  sdv_WdfDriverMiniportUnload(  
__in  WDFDRIVER  Driver);


#ifdef WdfObjectGetTypedContextWorker
#undef WdfObjectGetTypedContextWorker
#endif
#define WdfObjectGetTypedContextWorker sdv_WdfObjectGetTypedContextWorker
PVOID  sdv_WdfObjectGetTypedContextWorker(  
__in  WDFOBJECT  Handle,
__in  PCWDF_OBJECT_CONTEXT_TYPE_INFO  TypeInfo);


#ifdef WdfObjectAllocateContext
#undef WdfObjectAllocateContext
#endif
#define WdfObjectAllocateContext sdv_WdfObjectAllocateContext
NTSTATUS  sdv_WdfObjectAllocateContext(  
__in  WDFOBJECT  Handle,
__in  PWDF_OBJECT_ATTRIBUTES  ContextAttributes,
__deref_opt_out  PVOID*  Context);


#ifdef WdfObjectContextGetObject
#undef WdfObjectContextGetObject
#endif
#define WdfObjectContextGetObject sdv_WdfObjectContextGetObject
WDFOBJECT  sdv_WdfObjectContextGetObject(  
__in  PVOID  ContextPointer);


#ifdef WdfObjectReferenceActual
#undef WdfObjectReferenceActual
#endif
#define WdfObjectReferenceActual(arg1,arg2,arg3,arg4) sdv_WdfObjectReferenceActual(arg1,arg2,arg3,arg4);
VOID  sdv_WdfObjectReferenceActual(  
__in  WDFOBJECT  Handle,
__in_opt  PVOID  Tag,
   LONG  Line,
__in __nullterminated  PCHAR  File);


#ifdef WdfObjectDereferenceActual
#undef WdfObjectDereferenceActual
#endif
#define WdfObjectDereferenceActual(arg1,arg2,arg3,arg4) sdv_WdfObjectDereferenceActual(arg1,arg2,arg3,arg4);
VOID  sdv_WdfObjectDereferenceActual(  
__in  WDFOBJECT  Handle,
__in_opt  PVOID  Tag,
   LONG  Line,
__in __nullterminated  PCHAR  File);


#ifdef WdfObjectCreate
#undef WdfObjectCreate
#endif
#define WdfObjectCreate sdv_WdfObjectCreate
NTSTATUS  sdv_WdfObjectCreate(  
__in_opt  PWDF_OBJECT_ATTRIBUTES  Attributes,
__out  WDFOBJECT*  Object);


#ifdef WdfObjectDelete
#undef WdfObjectDelete
#endif
#define WdfObjectDelete(arg1) sdv_WdfObjectDelete(arg1);
VOID  sdv_WdfObjectDelete(  
__in  WDFOBJECT  Object);


#ifdef WdfObjectQuery
#undef WdfObjectQuery
#endif
#define WdfObjectQuery sdv_WdfObjectQuery
NTSTATUS  sdv_WdfObjectQuery(  
__in  WDFOBJECT  Object,
__in  GUID*  Guid,
   ULONG  QueryBufferLength,
__out_bcount( QueryBufferLength)  PVOID  QueryBuffer);


#ifdef WdfPdoInitAllocate
#undef WdfPdoInitAllocate
#endif
#define WdfPdoInitAllocate sdv_WdfPdoInitAllocate
PWDFDEVICE_INIT  sdv_WdfPdoInitAllocate(  
__in  WDFDEVICE  ParentDevice);


#ifdef WdfPdoInitSetEventCallbacks
#undef WdfPdoInitSetEventCallbacks
#endif
#define WdfPdoInitSetEventCallbacks(arg1,arg2) sdv_WdfPdoInitSetEventCallbacks(arg1,arg2);
VOID  sdv_WdfPdoInitSetEventCallbacks(  
__in  PWDFDEVICE_INIT  DeviceInit,
__in  PWDF_PDO_EVENT_CALLBACKS  DispatchTable);


#ifdef WdfPdoInitAssignDeviceID
#undef WdfPdoInitAssignDeviceID
#endif
#define WdfPdoInitAssignDeviceID sdv_WdfPdoInitAssignDeviceID
NTSTATUS  sdv_WdfPdoInitAssignDeviceID(  
__in  PWDFDEVICE_INIT  DeviceInit,
__in  PCUNICODE_STRING  DeviceID);


#ifdef WdfPdoInitAssignInstanceID
#undef WdfPdoInitAssignInstanceID
#endif
#define WdfPdoInitAssignInstanceID sdv_WdfPdoInitAssignInstanceID
NTSTATUS  sdv_WdfPdoInitAssignInstanceID(  
__in  PWDFDEVICE_INIT  DeviceInit,
__in  PCUNICODE_STRING  InstanceID);


#ifdef WdfPdoInitAddHardwareID
#undef WdfPdoInitAddHardwareID
#endif
#define WdfPdoInitAddHardwareID sdv_WdfPdoInitAddHardwareID
NTSTATUS  sdv_WdfPdoInitAddHardwareID(  
__in  PWDFDEVICE_INIT  DeviceInit,
__in  PCUNICODE_STRING  HardwareID);


#ifdef WdfPdoInitAddCompatibleID
#undef WdfPdoInitAddCompatibleID
#endif
#define WdfPdoInitAddCompatibleID sdv_WdfPdoInitAddCompatibleID
NTSTATUS  sdv_WdfPdoInitAddCompatibleID(  
__in  PWDFDEVICE_INIT  DeviceInit,
__in  PCUNICODE_STRING  CompatibleID);


#ifdef WdfPdoInitAddDeviceText
#undef WdfPdoInitAddDeviceText
#endif
#define WdfPdoInitAddDeviceText sdv_WdfPdoInitAddDeviceText
NTSTATUS  sdv_WdfPdoInitAddDeviceText(  
__in  PWDFDEVICE_INIT  DeviceInit,
__in  PCUNICODE_STRING  DeviceDescription,
__in  PCUNICODE_STRING  DeviceLocation,
   LCID  LocaleId);


#ifdef WdfPdoInitSetDefaultLocale
#undef WdfPdoInitSetDefaultLocale
#endif
#define WdfPdoInitSetDefaultLocale(arg1,arg2) sdv_WdfPdoInitSetDefaultLocale(arg1,arg2);
VOID  sdv_WdfPdoInitSetDefaultLocale(  
__in  PWDFDEVICE_INIT  DeviceInit,
   LCID  LocaleId);


#ifdef WdfPdoInitAssignRawDevice
#undef WdfPdoInitAssignRawDevice
#endif
#define WdfPdoInitAssignRawDevice sdv_WdfPdoInitAssignRawDevice
NTSTATUS  sdv_WdfPdoInitAssignRawDevice(  
  IN  PWDFDEVICE_INIT  DeviceInit,
  IN CONST GUID*  DeviceClassGuid);


#ifdef WdfPdoMarkMissing
#undef WdfPdoMarkMissing
#endif
#define WdfPdoMarkMissing sdv_WdfPdoMarkMissing
NTSTATUS  sdv_WdfPdoMarkMissing(  
__in  WDFDEVICE  Device);


#ifdef WdfPdoRequestEject
#undef WdfPdoRequestEject
#endif
#define WdfPdoRequestEject(arg1) sdv_WdfPdoRequestEject(arg1);
VOID  sdv_WdfPdoRequestEject(  
__in  WDFDEVICE  Device);


#ifdef WdfPdoGetParent
#undef WdfPdoGetParent
#endif
#define WdfPdoGetParent sdv_WdfPdoGetParent
WDFDEVICE  sdv_WdfPdoGetParent(  
__in  WDFDEVICE  Device);


#ifdef WdfPdoRetrieveIdentificationDescription
#undef WdfPdoRetrieveIdentificationDescription
#endif
#define WdfPdoRetrieveIdentificationDescription sdv_WdfPdoRetrieveIdentificationDescription
NTSTATUS  sdv_WdfPdoRetrieveIdentificationDescription(  
__in  WDFDEVICE  Device,
__inout  PWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER  IdentificationDescription);


#ifdef WdfPdoRetrieveAddressDescription
#undef WdfPdoRetrieveAddressDescription
#endif
#define WdfPdoRetrieveAddressDescription sdv_WdfPdoRetrieveAddressDescription
NTSTATUS  sdv_WdfPdoRetrieveAddressDescription(  
__in  WDFDEVICE  Device,
__inout  PWDF_CHILD_ADDRESS_DESCRIPTION_HEADER  AddressDescription);


#ifdef WdfPdoUpdateAddressDescription
#undef WdfPdoUpdateAddressDescription
#endif
#define WdfPdoUpdateAddressDescription sdv_WdfPdoUpdateAddressDescription
NTSTATUS  sdv_WdfPdoUpdateAddressDescription(  
__in  WDFDEVICE  Device,
__inout  PWDF_CHILD_ADDRESS_DESCRIPTION_HEADER  AddressDescription);


#ifdef WdfPdoAddEjectionRelationsPhysicalDevice
#undef WdfPdoAddEjectionRelationsPhysicalDevice
#endif
#define WdfPdoAddEjectionRelationsPhysicalDevice sdv_WdfPdoAddEjectionRelationsPhysicalDevice
NTSTATUS  sdv_WdfPdoAddEjectionRelationsPhysicalDevice(  
__in  WDFDEVICE  Device,
__in  PDEVICE_OBJECT  PhysicalDevice);


#ifdef WdfPdoRemoveEjectionRelationsPhysicalDevice
#undef WdfPdoRemoveEjectionRelationsPhysicalDevice
#endif
#define WdfPdoRemoveEjectionRelationsPhysicalDevice(arg1,arg2) sdv_WdfPdoRemoveEjectionRelationsPhysicalDevice(arg1,arg2);
VOID  sdv_WdfPdoRemoveEjectionRelationsPhysicalDevice(  
__in  WDFDEVICE  Device,
__in  PDEVICE_OBJECT  PhysicalDevice);


#ifdef WdfPdoClearEjectionRelationsDevices
#undef WdfPdoClearEjectionRelationsDevices
#endif
#define WdfPdoClearEjectionRelationsDevices(arg1) sdv_WdfPdoClearEjectionRelationsDevices(arg1);
VOID  sdv_WdfPdoClearEjectionRelationsDevices(  
__in  WDFDEVICE  Device);


#ifdef WdfDeviceAddQueryInterface
#undef WdfDeviceAddQueryInterface
#endif
#define WdfDeviceAddQueryInterface sdv_WdfDeviceAddQueryInterface
NTSTATUS  sdv_WdfDeviceAddQueryInterface(  
__in  WDFDEVICE  Device,
__in  PWDF_QUERY_INTERFACE_CONFIG  InterfaceConfig);


#ifdef WdfRegistryOpenKey
#undef WdfRegistryOpenKey
#endif
#define WdfRegistryOpenKey sdv_WdfRegistryOpenKey
NTSTATUS  sdv_WdfRegistryOpenKey(  
__in_opt  WDFKEY  ParentKey,
__in  PCUNICODE_STRING  KeyName,
   ACCESS_MASK  DesiredAccess,
__in_opt  PWDF_OBJECT_ATTRIBUTES  KeyAttributes,
__out  WDFKEY*  Key);


#ifdef WdfRegistryCreateKey
#undef WdfRegistryCreateKey
#endif
#define WdfRegistryCreateKey sdv_WdfRegistryCreateKey
NTSTATUS  sdv_WdfRegistryCreateKey(  
__in_opt  WDFKEY  ParentKey,
__in  PCUNICODE_STRING  KeyName,
   ACCESS_MASK  DesiredAccess,
   ULONG  CreateOptions,
__out_opt  PULONG  CreateDisposition,
__in_opt  PWDF_OBJECT_ATTRIBUTES  KeyAttributes,
__out  WDFKEY*  Key);


#ifdef WdfRegistryClose
#undef WdfRegistryClose
#endif
#define WdfRegistryClose(arg1) sdv_WdfRegistryClose(arg1);
VOID  sdv_WdfRegistryClose(  
__in  WDFKEY  Key);


#ifdef WdfRegistryWdmGetHandle
#undef WdfRegistryWdmGetHandle
#endif
#define WdfRegistryWdmGetHandle sdv_WdfRegistryWdmGetHandle
HANDLE  sdv_WdfRegistryWdmGetHandle(  
__in  WDFKEY  Key);


#ifdef WdfRegistryRemoveKey
#undef WdfRegistryRemoveKey
#endif
#define WdfRegistryRemoveKey sdv_WdfRegistryRemoveKey
NTSTATUS  sdv_WdfRegistryRemoveKey(  
__in  WDFKEY  Key);


#ifdef WdfRegistryRemoveValue
#undef WdfRegistryRemoveValue
#endif
#define WdfRegistryRemoveValue sdv_WdfRegistryRemoveValue
NTSTATUS  sdv_WdfRegistryRemoveValue(  
__in  WDFKEY  Key,
__in  PCUNICODE_STRING  ValueName);


#ifdef WdfRegistryQueryValue
#undef WdfRegistryQueryValue
#endif
#define WdfRegistryQueryValue sdv_WdfRegistryQueryValue
NTSTATUS  sdv_WdfRegistryQueryValue(  
__in  WDFKEY  Key,
__in  PCUNICODE_STRING  ValueName,
   ULONG  ValueLength,
__out_bcount_opt( ValueLength)  PVOID  Value,
__out_opt  PULONG  ValueLengthQueried,
__out_opt  PULONG  ValueType);


#ifdef WdfRegistryQueryMemory
#undef WdfRegistryQueryMemory
#endif
#define WdfRegistryQueryMemory sdv_WdfRegistryQueryMemory
NTSTATUS  sdv_WdfRegistryQueryMemory(  
__in  WDFKEY  Key,
__in  PCUNICODE_STRING  ValueName,
   POOL_TYPE  PoolType,
__in_opt  PWDF_OBJECT_ATTRIBUTES  MemoryAttributes,
__out  WDFMEMORY*  Memory,
__out_opt  PULONG  ValueType);


#ifdef WdfRegistryQueryMultiString
#undef WdfRegistryQueryMultiString
#endif
#define WdfRegistryQueryMultiString sdv_WdfRegistryQueryMultiString
NTSTATUS  sdv_WdfRegistryQueryMultiString(  
__in  WDFKEY  Key,
__in  PCUNICODE_STRING  ValueName,
__in_opt  PWDF_OBJECT_ATTRIBUTES  StringsAttributes,
__in  WDFCOLLECTION  Collection);


#ifdef WdfRegistryQueryUnicodeString
#undef WdfRegistryQueryUnicodeString
#endif
#define WdfRegistryQueryUnicodeString sdv_WdfRegistryQueryUnicodeString
NTSTATUS  sdv_WdfRegistryQueryUnicodeString(  
__in  WDFKEY  Key,
__in  PCUNICODE_STRING  ValueName,
__out_opt  PUSHORT  ValueByteLength,
__inout_opt  PUNICODE_STRING  Value);


#ifdef WdfRegistryQueryString
#undef WdfRegistryQueryString
#endif
#define WdfRegistryQueryString sdv_WdfRegistryQueryString
NTSTATUS  sdv_WdfRegistryQueryString(  
  WDFKEY  Key,
  PCUNICODE_STRING  ValueName,
   WDFSTRING  String);


#ifdef WdfRegistryQueryULong
#undef WdfRegistryQueryULong
#endif
#define WdfRegistryQueryULong sdv_WdfRegistryQueryULong
NTSTATUS  sdv_WdfRegistryQueryULong(  
__in  WDFKEY  Key,
__in  PCUNICODE_STRING  ValueName,
__out  PULONG  Value);


#ifdef WdfRegistryAssignValue
#undef WdfRegistryAssignValue
#endif
#define WdfRegistryAssignValue sdv_WdfRegistryAssignValue
NTSTATUS  sdv_WdfRegistryAssignValue(  
__in  WDFKEY  Key,
__in  PCUNICODE_STRING  ValueName,
   ULONG  ValueType,
   ULONG  ValueLength,
__in_ecount( ValueLength)  PVOID  Value);


#ifdef WdfRegistryAssignMemory
#undef WdfRegistryAssignMemory
#endif
#define WdfRegistryAssignMemory sdv_WdfRegistryAssignMemory
NTSTATUS  sdv_WdfRegistryAssignMemory(  
__in  WDFKEY  Key,
__in  PCUNICODE_STRING  ValueName,
   ULONG  ValueType,
   WDFMEMORY  Memory,
__in_opt  PWDFMEMORY_OFFSET  MemoryOffsets);


#ifdef WdfRegistryAssignMultiString
#undef WdfRegistryAssignMultiString
#endif
#define WdfRegistryAssignMultiString sdv_WdfRegistryAssignMultiString
NTSTATUS  sdv_WdfRegistryAssignMultiString(  
__in  WDFKEY  Key,
__in  PCUNICODE_STRING  ValueName,
   WDFCOLLECTION  StringsCollection);


#ifdef WdfRegistryAssignUnicodeString
#undef WdfRegistryAssignUnicodeString
#endif
#define WdfRegistryAssignUnicodeString sdv_WdfRegistryAssignUnicodeString
NTSTATUS  sdv_WdfRegistryAssignUnicodeString(  
__in  WDFKEY  Key,
__in  PCUNICODE_STRING  ValueName,
__in  PCUNICODE_STRING  Value);


#ifdef WdfRegistryAssignString
#undef WdfRegistryAssignString
#endif
#define WdfRegistryAssignString sdv_WdfRegistryAssignString
NTSTATUS  sdv_WdfRegistryAssignString(  
__in  WDFKEY  Key,
__in  PCUNICODE_STRING  ValueName,
   WDFSTRING  String);


#ifdef WdfRegistryAssignULong
#undef WdfRegistryAssignULong
#endif
#define WdfRegistryAssignULong sdv_WdfRegistryAssignULong
NTSTATUS  sdv_WdfRegistryAssignULong(  
__in  WDFKEY  Key,
__in  PCUNICODE_STRING  ValueName,
   ULONG  Value);


#ifdef WdfRequestCreate
#undef WdfRequestCreate
#endif
#define WdfRequestCreate sdv_WdfRequestCreate
NTSTATUS  sdv_WdfRequestCreate(  
__in_opt  PWDF_OBJECT_ATTRIBUTES  RequestAttributes,
__in_opt  WDFIOTARGET  IoTarget,
__out  WDFREQUEST*  Request);


#ifdef WdfRequestCreateFromIrp
#undef WdfRequestCreateFromIrp
#endif
#define WdfRequestCreateFromIrp sdv_WdfRequestCreateFromIrp
NTSTATUS  sdv_WdfRequestCreateFromIrp(  
__in_opt  PWDF_OBJECT_ATTRIBUTES  RequestAttributes,
__in  PIRP  Irp,
   BOOLEAN  RequestFreesIrp,
__out  WDFREQUEST*  Request);


#ifdef WdfRequestReuse
#undef WdfRequestReuse
#endif
#define WdfRequestReuse sdv_WdfRequestReuse
NTSTATUS  sdv_WdfRequestReuse(  
__in  WDFREQUEST  Request,
__in  PWDF_REQUEST_REUSE_PARAMS  ReuseParams);


#ifdef WdfRequestChangeTarget
#undef WdfRequestChangeTarget
#endif
#define WdfRequestChangeTarget sdv_WdfRequestChangeTarget
NTSTATUS  sdv_WdfRequestChangeTarget(  
__in  WDFREQUEST  Request,
__in  WDFIOTARGET  IoTarget);


#ifdef WdfRequestFormatRequestUsingCurrentType
#undef WdfRequestFormatRequestUsingCurrentType
#endif
#define WdfRequestFormatRequestUsingCurrentType(arg1) sdv_WdfRequestFormatRequestUsingCurrentType(arg1);
VOID  sdv_WdfRequestFormatRequestUsingCurrentType(  
__in  WDFREQUEST  Request);


#ifdef WdfRequestWdmFormatUsingStackLocation
#undef WdfRequestWdmFormatUsingStackLocation
#endif
#define WdfRequestWdmFormatUsingStackLocation(arg1,arg2) sdv_WdfRequestWdmFormatUsingStackLocation(arg1,arg2);
VOID  sdv_WdfRequestWdmFormatUsingStackLocation(  
__in  WDFREQUEST  Request,
__in  PIO_STACK_LOCATION  Stack);


#ifdef WdfRequestSend
#undef WdfRequestSend
#endif
#define WdfRequestSend sdv_WdfRequestSend
BOOLEAN  sdv_WdfRequestSend(  
__in  WDFREQUEST  Request,
__in  WDFIOTARGET  Target,
__in_opt  PWDF_REQUEST_SEND_OPTIONS  Options);


#ifdef WdfRequestGetStatus
#undef WdfRequestGetStatus
#endif
#define WdfRequestGetStatus sdv_WdfRequestGetStatus
NTSTATUS  sdv_WdfRequestGetStatus(  
__in  WDFREQUEST  Request);


#ifdef WdfRequestMarkCancelable
#undef WdfRequestMarkCancelable
#endif
#define WdfRequestMarkCancelable(arg1,arg2) sdv_WdfRequestMarkCancelable(arg1,arg2);
VOID  sdv_WdfRequestMarkCancelable(  
__in  WDFREQUEST  Request,
__in  PFN_WDF_REQUEST_CANCEL  EvtRequestCancel);


#ifdef WdfRequestUnmarkCancelable
#undef WdfRequestUnmarkCancelable
#endif
#define WdfRequestUnmarkCancelable sdv_WdfRequestUnmarkCancelable
NTSTATUS  sdv_WdfRequestUnmarkCancelable(  
__in  WDFREQUEST  Request);


#ifdef WdfRequestIsCanceled
#undef WdfRequestIsCanceled
#endif
#define WdfRequestIsCanceled sdv_WdfRequestIsCanceled
BOOLEAN  sdv_WdfRequestIsCanceled(  
__in  WDFREQUEST  Request);


#ifdef WdfRequestCancelSentRequest
#undef WdfRequestCancelSentRequest
#endif
#define WdfRequestCancelSentRequest sdv_WdfRequestCancelSentRequest
BOOLEAN  sdv_WdfRequestCancelSentRequest(  
__in  WDFREQUEST  Request);


#ifdef WdfRequestIsFrom32BitProcess
#undef WdfRequestIsFrom32BitProcess
#endif
#define WdfRequestIsFrom32BitProcess sdv_WdfRequestIsFrom32BitProcess
BOOLEAN  sdv_WdfRequestIsFrom32BitProcess(  
__in  WDFREQUEST  Request);


#ifdef WdfRequestSetCompletionRoutine
#undef WdfRequestSetCompletionRoutine
#endif
#define WdfRequestSetCompletionRoutine(arg1,arg2,arg3) sdv_WdfRequestSetCompletionRoutine(arg1,arg2,arg3);
VOID  sdv_WdfRequestSetCompletionRoutine(  
__in  WDFREQUEST  Request,
__in_opt  PFN_WDF_REQUEST_COMPLETION_ROUTINE  CompletionRoutine,
__in_opt  WDFCONTEXT  CompletionContext);


#ifdef WdfRequestGetCompletionParams
#undef WdfRequestGetCompletionParams
#endif
#define WdfRequestGetCompletionParams(arg1,arg2) sdv_WdfRequestGetCompletionParams(arg1,arg2);
VOID  sdv_WdfRequestGetCompletionParams(  
__in  WDFREQUEST  Request,
__inout  PWDF_REQUEST_COMPLETION_PARAMS  Params);


#ifdef WdfRequestAllocateTimer
#undef WdfRequestAllocateTimer
#endif
#define WdfRequestAllocateTimer sdv_WdfRequestAllocateTimer
NTSTATUS  sdv_WdfRequestAllocateTimer(  
__in  WDFREQUEST  Request);


#ifdef WdfRequestComplete
#undef WdfRequestComplete
#endif
#define WdfRequestComplete(arg1,arg2) sdv_WdfRequestComplete(arg1,arg2);
VOID  sdv_WdfRequestComplete(  
__in  WDFREQUEST  Request,
   NTSTATUS  Status);


#ifdef WdfRequestCompleteWithPriorityBoost
#undef WdfRequestCompleteWithPriorityBoost
#endif
#define WdfRequestCompleteWithPriorityBoost(arg1,arg2,arg3) sdv_WdfRequestCompleteWithPriorityBoost(arg1,arg2,arg3);
VOID  sdv_WdfRequestCompleteWithPriorityBoost(  
__in  WDFREQUEST  Request,
   NTSTATUS  Status,
   CCHAR  PriorityBoost);


#ifdef WdfRequestCompleteWithInformation
#undef WdfRequestCompleteWithInformation
#endif
#define WdfRequestCompleteWithInformation(arg1,arg2,arg3) sdv_WdfRequestCompleteWithInformation(arg1,arg2,arg3);
VOID  sdv_WdfRequestCompleteWithInformation(  
__in  WDFREQUEST  Request,
   NTSTATUS  Status,
   ULONG_PTR  Information);


#ifdef WdfRequestGetParameters
#undef WdfRequestGetParameters
#endif
#define WdfRequestGetParameters(arg1,arg2) sdv_WdfRequestGetParameters(arg1,arg2);
VOID  sdv_WdfRequestGetParameters(  
__in  WDFREQUEST  Request,
__inout  PWDF_REQUEST_PARAMETERS  Parameters);


#ifdef WdfRequestRetrieveInputMemory
#undef WdfRequestRetrieveInputMemory
#endif
#define WdfRequestRetrieveInputMemory sdv_WdfRequestRetrieveInputMemory
NTSTATUS  sdv_WdfRequestRetrieveInputMemory(  
__in  WDFREQUEST  Request,
__out  WDFMEMORY*  Memory);


#ifdef WdfRequestRetrieveOutputMemory
#undef WdfRequestRetrieveOutputMemory
#endif
#define WdfRequestRetrieveOutputMemory sdv_WdfRequestRetrieveOutputMemory
NTSTATUS  sdv_WdfRequestRetrieveOutputMemory(  
__in  WDFREQUEST  Request,
__out  WDFMEMORY*  Memory);


#ifdef WdfRequestRetrieveInputBuffer
#undef WdfRequestRetrieveInputBuffer
#endif
#define WdfRequestRetrieveInputBuffer sdv_WdfRequestRetrieveInputBuffer
NTSTATUS  sdv_WdfRequestRetrieveInputBuffer(  
__in  WDFREQUEST  Request,
   size_t  MinimumRequiredLength,
__deref_out_bcount(*Length)  PVOID*  Buffer,
__out_opt  size_t*  Length);


#ifdef WdfRequestRetrieveOutputBuffer
#undef WdfRequestRetrieveOutputBuffer
#endif
#define WdfRequestRetrieveOutputBuffer sdv_WdfRequestRetrieveOutputBuffer
NTSTATUS  sdv_WdfRequestRetrieveOutputBuffer(  
__in  WDFREQUEST  Request,
   size_t  MinimumRequiredSize,
__deref_out_bcount(*Length)  PVOID*  Buffer,
__out_opt  size_t*  Length);


#ifdef WdfRequestRetrieveInputWdmMdl
#undef WdfRequestRetrieveInputWdmMdl
#endif
#define WdfRequestRetrieveInputWdmMdl sdv_WdfRequestRetrieveInputWdmMdl
NTSTATUS  sdv_WdfRequestRetrieveInputWdmMdl(  
__in  WDFREQUEST  Request,
__deref_out  PMDL*  Mdl);


#ifdef WdfRequestRetrieveOutputWdmMdl
#undef WdfRequestRetrieveOutputWdmMdl
#endif
#define WdfRequestRetrieveOutputWdmMdl sdv_WdfRequestRetrieveOutputWdmMdl
NTSTATUS  sdv_WdfRequestRetrieveOutputWdmMdl(  
__in  WDFREQUEST  Request,
__deref_out  PMDL*  Mdl);


#ifdef WdfRequestRetrieveUnsafeUserInputBuffer
#undef WdfRequestRetrieveUnsafeUserInputBuffer
#endif
#define WdfRequestRetrieveUnsafeUserInputBuffer sdv_WdfRequestRetrieveUnsafeUserInputBuffer
NTSTATUS  sdv_WdfRequestRetrieveUnsafeUserInputBuffer(  
__in  WDFREQUEST  Request,
   size_t  MinimumRequiredLength,
__deref_out_bcount_opt(*Length)  PVOID*  InputBuffer,
__out_opt  size_t*  Length);


#ifdef WdfRequestRetrieveUnsafeUserOutputBuffer
#undef WdfRequestRetrieveUnsafeUserOutputBuffer
#endif
#define WdfRequestRetrieveUnsafeUserOutputBuffer sdv_WdfRequestRetrieveUnsafeUserOutputBuffer
NTSTATUS  sdv_WdfRequestRetrieveUnsafeUserOutputBuffer(  
__in  WDFREQUEST  Request,
   size_t  MinimumRequiredLength,
__deref_out_bcount_opt(*Length)  PVOID*  OutputBuffer,
__out_opt  size_t*  Length);


#ifdef WdfRequestSetInformation
#undef WdfRequestSetInformation
#endif
#define WdfRequestSetInformation(arg1,arg2) sdv_WdfRequestSetInformation(arg1,arg2);
VOID  sdv_WdfRequestSetInformation(  
__in  WDFREQUEST  Request,
   ULONG_PTR  Information);


#ifdef WdfRequestGetInformation
#undef WdfRequestGetInformation
#endif
#define WdfRequestGetInformation sdv_WdfRequestGetInformation
ULONG_PTR  sdv_WdfRequestGetInformation(  
__in  WDFREQUEST  Request);


#ifdef WdfRequestGetFileObject
#undef WdfRequestGetFileObject
#endif
#define WdfRequestGetFileObject sdv_WdfRequestGetFileObject
WDFFILEOBJECT  sdv_WdfRequestGetFileObject(  
__in  WDFREQUEST  Request);


#ifdef WdfRequestProbeAndLockUserBufferForRead
#undef WdfRequestProbeAndLockUserBufferForRead
#endif
#define WdfRequestProbeAndLockUserBufferForRead sdv_WdfRequestProbeAndLockUserBufferForRead
NTSTATUS  sdv_WdfRequestProbeAndLockUserBufferForRead(  
__in  WDFREQUEST  Request,
__in  PVOID  Buffer,
   size_t  Length,
__out  WDFMEMORY*  MemoryObject);


#ifdef WdfRequestProbeAndLockUserBufferForWrite
#undef WdfRequestProbeAndLockUserBufferForWrite
#endif
#define WdfRequestProbeAndLockUserBufferForWrite sdv_WdfRequestProbeAndLockUserBufferForWrite
NTSTATUS  sdv_WdfRequestProbeAndLockUserBufferForWrite(  
__in  WDFREQUEST  Request,
__in  PVOID  Buffer,
   size_t  Length,
__out  WDFMEMORY*  MemoryObject);


#ifdef WdfRequestGetRequestorMode
#undef WdfRequestGetRequestorMode
#endif
#define WdfRequestGetRequestorMode sdv_WdfRequestGetRequestorMode
KPROCESSOR_MODE  sdv_WdfRequestGetRequestorMode(  
__in  WDFREQUEST  Request);


#ifdef WdfRequestForwardToIoQueue
#undef WdfRequestForwardToIoQueue
#endif
#define WdfRequestForwardToIoQueue sdv_WdfRequestForwardToIoQueue
NTSTATUS  sdv_WdfRequestForwardToIoQueue(  
__in  WDFREQUEST  Request,
__in  WDFQUEUE  DestinationQueue);


#ifdef WdfRequestGetIoQueue
#undef WdfRequestGetIoQueue
#endif
#define WdfRequestGetIoQueue sdv_WdfRequestGetIoQueue
WDFQUEUE  sdv_WdfRequestGetIoQueue(  
__in  WDFREQUEST  Request);


#ifdef WdfRequestRequeue
#undef WdfRequestRequeue
#endif
#define WdfRequestRequeue sdv_WdfRequestRequeue
NTSTATUS  sdv_WdfRequestRequeue(  
__in  WDFREQUEST  Request);


#ifdef WdfRequestWdmGetIrp
#undef WdfRequestWdmGetIrp
#endif
#define WdfRequestWdmGetIrp sdv_WdfRequestWdmGetIrp
PIRP  sdv_WdfRequestWdmGetIrp(  
__in  WDFREQUEST  Request);


#ifdef WdfIoResourceRequirementsListSetSlotNumber
#undef WdfIoResourceRequirementsListSetSlotNumber
#endif
#define WdfIoResourceRequirementsListSetSlotNumber(arg1,arg2) sdv_WdfIoResourceRequirementsListSetSlotNumber(arg1,arg2);
VOID  sdv_WdfIoResourceRequirementsListSetSlotNumber(  
__in  WDFIORESREQLIST  RequirementsList,
   ULONG  SlotNumber);


#ifdef WdfIoResourceRequirementsListSetInterfaceType
#undef WdfIoResourceRequirementsListSetInterfaceType
#endif
#define WdfIoResourceRequirementsListSetInterfaceType(arg1,arg2) sdv_WdfIoResourceRequirementsListSetInterfaceType(arg1,arg2);
VOID  sdv_WdfIoResourceRequirementsListSetInterfaceType(  
__in  WDFIORESREQLIST  RequirementsList,
   INTERFACE_TYPE  InterfaceType);


#ifdef WdfIoResourceRequirementsListAppendIoResList
#undef WdfIoResourceRequirementsListAppendIoResList
#endif
#define WdfIoResourceRequirementsListAppendIoResList sdv_WdfIoResourceRequirementsListAppendIoResList
NTSTATUS  sdv_WdfIoResourceRequirementsListAppendIoResList(  
__in  WDFIORESREQLIST  RequirementsList,
__in  WDFIORESLIST  IoResList);


#ifdef WdfIoResourceRequirementsListInsertIoResList
#undef WdfIoResourceRequirementsListInsertIoResList
#endif
#define WdfIoResourceRequirementsListInsertIoResList sdv_WdfIoResourceRequirementsListInsertIoResList
NTSTATUS  sdv_WdfIoResourceRequirementsListInsertIoResList(  
__in  WDFIORESREQLIST  RequirementsList,
__in  WDFIORESLIST  IoResList,
   ULONG  Index);


#ifdef WdfIoResourceRequirementsListGetCount
#undef WdfIoResourceRequirementsListGetCount
#endif
#define WdfIoResourceRequirementsListGetCount sdv_WdfIoResourceRequirementsListGetCount
ULONG  sdv_WdfIoResourceRequirementsListGetCount(  
__in  WDFIORESREQLIST  RequirementsList);


#ifdef WdfIoResourceRequirementsListGetIoResList
#undef WdfIoResourceRequirementsListGetIoResList
#endif
#define WdfIoResourceRequirementsListGetIoResList sdv_WdfIoResourceRequirementsListGetIoResList
WDFIORESLIST  sdv_WdfIoResourceRequirementsListGetIoResList(  
__in  WDFIORESREQLIST  RequirementsList,
   ULONG  Index);


#ifdef WdfIoResourceRequirementsListRemove
#undef WdfIoResourceRequirementsListRemove
#endif
#define WdfIoResourceRequirementsListRemove(arg1,arg2) sdv_WdfIoResourceRequirementsListRemove(arg1,arg2);
VOID  sdv_WdfIoResourceRequirementsListRemove(  
__in  WDFIORESREQLIST  RequirementsList,
   ULONG  Index);


#ifdef WdfIoResourceRequirementsListRemoveByIoResList
#undef WdfIoResourceRequirementsListRemoveByIoResList
#endif
#define WdfIoResourceRequirementsListRemoveByIoResList(arg1,arg2) sdv_WdfIoResourceRequirementsListRemoveByIoResList(arg1,arg2);
VOID  sdv_WdfIoResourceRequirementsListRemoveByIoResList(  
__in  WDFIORESREQLIST  RequirementsList,
__in  WDFIORESLIST  IoResList);


#ifdef WdfIoResourceListCreate
#undef WdfIoResourceListCreate
#endif
#define WdfIoResourceListCreate sdv_WdfIoResourceListCreate
NTSTATUS  sdv_WdfIoResourceListCreate(  
__in  WDFIORESREQLIST  RequirementsList,
__in_opt  PWDF_OBJECT_ATTRIBUTES  Attributes,
__out  WDFIORESLIST*  ResourceList);


#ifdef WdfIoResourceListAppendDescriptor
#undef WdfIoResourceListAppendDescriptor
#endif
#define WdfIoResourceListAppendDescriptor sdv_WdfIoResourceListAppendDescriptor
NTSTATUS  sdv_WdfIoResourceListAppendDescriptor(  
__in  WDFIORESLIST  ResourceList,
__in  PIO_RESOURCE_DESCRIPTOR  Descriptor);


#ifdef WdfIoResourceListInsertDescriptor
#undef WdfIoResourceListInsertDescriptor
#endif
#define WdfIoResourceListInsertDescriptor sdv_WdfIoResourceListInsertDescriptor
NTSTATUS  sdv_WdfIoResourceListInsertDescriptor(  
__in  WDFIORESLIST  ResourceList,
__in  PIO_RESOURCE_DESCRIPTOR  Descriptor,
   ULONG  Index);


#ifdef WdfIoResourceListUpdateDescriptor
#undef WdfIoResourceListUpdateDescriptor
#endif
#define WdfIoResourceListUpdateDescriptor(arg1,arg2,arg3) sdv_WdfIoResourceListUpdateDescriptor(arg1,arg2,arg3);
VOID  sdv_WdfIoResourceListUpdateDescriptor(  
__in  WDFIORESLIST  ResourceList,
__in  PIO_RESOURCE_DESCRIPTOR  Descriptor,
   ULONG  Index);


#ifdef WdfIoResourceListGetCount
#undef WdfIoResourceListGetCount
#endif
#define WdfIoResourceListGetCount sdv_WdfIoResourceListGetCount
ULONG  sdv_WdfIoResourceListGetCount(  
__in  WDFIORESLIST  ResourceList);


#ifdef WdfIoResourceListGetDescriptor
#undef WdfIoResourceListGetDescriptor
#endif
#define WdfIoResourceListGetDescriptor sdv_WdfIoResourceListGetDescriptor
PIO_RESOURCE_DESCRIPTOR  sdv_WdfIoResourceListGetDescriptor(  
__in  WDFIORESLIST  ResourceList,
   ULONG  Index);


#ifdef WdfIoResourceListRemove
#undef WdfIoResourceListRemove
#endif
#define WdfIoResourceListRemove(arg1,arg2) sdv_WdfIoResourceListRemove(arg1,arg2);
VOID  sdv_WdfIoResourceListRemove(  
__in  WDFIORESLIST  ResourceList,
   ULONG  Index);


#ifdef WdfIoResourceListRemoveByDescriptor
#undef WdfIoResourceListRemoveByDescriptor
#endif
#define WdfIoResourceListRemoveByDescriptor(arg1,arg2) sdv_WdfIoResourceListRemoveByDescriptor(arg1,arg2);
VOID  sdv_WdfIoResourceListRemoveByDescriptor(  
__in  WDFIORESLIST  ResourceList,
__in  PIO_RESOURCE_DESCRIPTOR  Descriptor);


#ifdef WdfCmResourceListAppendDescriptor
#undef WdfCmResourceListAppendDescriptor
#endif
#define WdfCmResourceListAppendDescriptor sdv_WdfCmResourceListAppendDescriptor
NTSTATUS  sdv_WdfCmResourceListAppendDescriptor(  
__in  WDFCMRESLIST  List,
__in  PCM_PARTIAL_RESOURCE_DESCRIPTOR  Descriptor);


#ifdef WdfCmResourceListInsertDescriptor
#undef WdfCmResourceListInsertDescriptor
#endif
#define WdfCmResourceListInsertDescriptor sdv_WdfCmResourceListInsertDescriptor
NTSTATUS  sdv_WdfCmResourceListInsertDescriptor(  
__in  WDFCMRESLIST  List,
__in  PCM_PARTIAL_RESOURCE_DESCRIPTOR  Descriptor,
   ULONG  Index);


#ifdef WdfCmResourceListGetCount
#undef WdfCmResourceListGetCount
#endif
#define WdfCmResourceListGetCount sdv_WdfCmResourceListGetCount
ULONG  sdv_WdfCmResourceListGetCount(  
__in  WDFCMRESLIST  List);


#ifdef WdfCmResourceListGetDescriptor
#undef WdfCmResourceListGetDescriptor
#endif
#define WdfCmResourceListGetDescriptor sdv_WdfCmResourceListGetDescriptor
PCM_PARTIAL_RESOURCE_DESCRIPTOR  sdv_WdfCmResourceListGetDescriptor(  
__in  WDFCMRESLIST  List,
   ULONG  Index);


#ifdef WdfCmResourceListRemove
#undef WdfCmResourceListRemove
#endif
#define WdfCmResourceListRemove(arg1,arg2) sdv_WdfCmResourceListRemove(arg1,arg2);
VOID  sdv_WdfCmResourceListRemove(  
__in  WDFCMRESLIST  List,
   ULONG  Index);


#ifdef WdfCmResourceListRemoveByDescriptor
#undef WdfCmResourceListRemoveByDescriptor
#endif
#define WdfCmResourceListRemoveByDescriptor(arg1,arg2) sdv_WdfCmResourceListRemoveByDescriptor(arg1,arg2);
VOID  sdv_WdfCmResourceListRemoveByDescriptor(  
__in  WDFCMRESLIST  List,
__in  PCM_PARTIAL_RESOURCE_DESCRIPTOR  Descriptor);


#ifdef WdfStringCreate
#undef WdfStringCreate
#endif
#define WdfStringCreate sdv_WdfStringCreate
NTSTATUS  sdv_WdfStringCreate(  
__in_opt  PCUNICODE_STRING  UnicodeString,
__in_opt  PWDF_OBJECT_ATTRIBUTES  StringAttributes,
__out  WDFSTRING*  String);


#ifdef WdfStringGetUnicodeString
#undef WdfStringGetUnicodeString
#endif
#define WdfStringGetUnicodeString(arg1,arg2) sdv_WdfStringGetUnicodeString(arg1,arg2);
VOID  sdv_WdfStringGetUnicodeString(  
__in  WDFSTRING  String,
   PUNICODE_STRING  UnicodeString);


#ifdef WdfObjectAcquireLock
#undef WdfObjectAcquireLock
#endif
#define WdfObjectAcquireLock(arg1) sdv_WdfObjectAcquireLock(arg1);
VOID  sdv_WdfObjectAcquireLock(  
__in  WDFOBJECT  Object);


#ifdef WdfObjectReleaseLock
#undef WdfObjectReleaseLock
#endif
#define WdfObjectReleaseLock(arg1) sdv_WdfObjectReleaseLock(arg1);
VOID  sdv_WdfObjectReleaseLock(  
__in  WDFOBJECT  Object);


#ifdef WdfWaitLockCreate
#undef WdfWaitLockCreate
#endif
#define WdfWaitLockCreate sdv_WdfWaitLockCreate
NTSTATUS  sdv_WdfWaitLockCreate(  
__in_opt  PWDF_OBJECT_ATTRIBUTES  LockAttributes,
__out  WDFWAITLOCK*  Lock);


#ifdef WdfWaitLockAcquire
#undef WdfWaitLockAcquire
#endif
#define WdfWaitLockAcquire sdv_WdfWaitLockAcquire
NTSTATUS  sdv_WdfWaitLockAcquire(  
__in  WDFWAITLOCK  Lock,
__in_opt  PLONGLONG  Timeout);


#ifdef WdfWaitLockRelease
#undef WdfWaitLockRelease
#endif
#define WdfWaitLockRelease(arg1) sdv_WdfWaitLockRelease(arg1);
VOID  sdv_WdfWaitLockRelease(  
__in  WDFWAITLOCK  Lock);


#ifdef WdfSpinLockCreate
#undef WdfSpinLockCreate
#endif
#define WdfSpinLockCreate sdv_WdfSpinLockCreate
NTSTATUS  sdv_WdfSpinLockCreate(  
__in_opt  PWDF_OBJECT_ATTRIBUTES  SpinLockAttributes,
__out  WDFSPINLOCK*  SpinLock);


#ifdef WdfSpinLockAcquire
#undef WdfSpinLockAcquire
#endif
#define WdfSpinLockAcquire(arg1) sdv_WdfSpinLockAcquire(arg1);
VOID  sdv_WdfSpinLockAcquire(  
__in  WDFSPINLOCK  SpinLock);


#ifdef WdfSpinLockRelease
#undef WdfSpinLockRelease
#endif
#define WdfSpinLockRelease(arg1) sdv_WdfSpinLockRelease(arg1);
VOID  sdv_WdfSpinLockRelease(  
__in  WDFSPINLOCK  SpinLock);


#ifdef WdfTimerCreate
#undef WdfTimerCreate
#endif
#define WdfTimerCreate sdv_WdfTimerCreate
NTSTATUS  sdv_WdfTimerCreate(  
__in  PWDF_TIMER_CONFIG  Config,
__in  PWDF_OBJECT_ATTRIBUTES  Attributes,
__out  WDFTIMER*  Timer);


#ifdef WdfTimerStart
#undef WdfTimerStart
#endif
#define WdfTimerStart sdv_WdfTimerStart
BOOLEAN  sdv_WdfTimerStart(  
__in  WDFTIMER  Timer,
   LONGLONG  DueTime);


#ifdef WdfTimerStop
#undef WdfTimerStop
#endif
#define WdfTimerStop sdv_WdfTimerStop
BOOLEAN  sdv_WdfTimerStop(  
__in  WDFTIMER  Timer,
__in  BOOLEAN  Wait);


#ifdef WdfTimerGetParentObject
#undef WdfTimerGetParentObject
#endif
#define WdfTimerGetParentObject sdv_WdfTimerGetParentObject
WDFOBJECT  sdv_WdfTimerGetParentObject(  
__in  WDFTIMER  Timer);



#ifdef WdfVerifierKeBugCheck
#undef WdfVerifierKeBugCheck
#endif
#define WdfVerifierKeBugCheck(arg1,arg2,arg3,arg4,arg5) sdv_WdfVerifierKeBugCheck(arg1,arg2,arg3,arg4,arg5);
VOID  sdv_WdfVerifierKeBugCheck(  
   ULONG  BugCheckCode,
   ULONG_PTR  BugCheckParameter1,
   ULONG_PTR  BugCheckParameter2,
   ULONG_PTR  BugCheckParameter3,
   ULONG_PTR  BugCheckParameter4);


#ifdef WdfWmiProviderCreate
#undef WdfWmiProviderCreate
#endif
#define WdfWmiProviderCreate sdv_WdfWmiProviderCreate
NTSTATUS  sdv_WdfWmiProviderCreate(  
__in  WDFDEVICE  Device,
__in  PWDF_WMI_PROVIDER_CONFIG  WmiProviderConfig,
__in_opt  PWDF_OBJECT_ATTRIBUTES  ProviderAttributes,
__out  WDFWMIPROVIDER*  WmiProvider);


#ifdef WdfWmiProviderGetDevice
#undef WdfWmiProviderGetDevice
#endif
#define WdfWmiProviderGetDevice sdv_WdfWmiProviderGetDevice
WDFDEVICE  sdv_WdfWmiProviderGetDevice(  
__in  WDFWMIPROVIDER  WmiProvider);


#ifdef WdfWmiProviderIsEnabled
#undef WdfWmiProviderIsEnabled
#endif
#define WdfWmiProviderIsEnabled sdv_WdfWmiProviderIsEnabled
BOOLEAN  sdv_WdfWmiProviderIsEnabled(  
__in  WDFWMIPROVIDER  WmiProvider,
   WDF_WMI_PROVIDER_CONTROL  ProviderControl);


#ifdef WdfWmiProviderGetTracingHandle
#undef WdfWmiProviderGetTracingHandle
#endif
#define WdfWmiProviderGetTracingHandle sdv_WdfWmiProviderGetTracingHandle
ULONGLONG  sdv_WdfWmiProviderGetTracingHandle(  
__in  WDFWMIPROVIDER  WmiProvider);


#ifdef WdfWmiInstanceCreate
#undef WdfWmiInstanceCreate
#endif
#define WdfWmiInstanceCreate sdv_WdfWmiInstanceCreate
NTSTATUS  sdv_WdfWmiInstanceCreate(  
__in  WDFDEVICE  Device,
__in  PWDF_WMI_INSTANCE_CONFIG  InstanceConfig,
__in_opt  PWDF_OBJECT_ATTRIBUTES  InstanceAttributes,
__out_opt  WDFWMIINSTANCE*  Instance);


#ifdef WdfWmiInstanceRegister
#undef WdfWmiInstanceRegister
#endif
#define WdfWmiInstanceRegister sdv_WdfWmiInstanceRegister
NTSTATUS  sdv_WdfWmiInstanceRegister(  
__in  WDFWMIINSTANCE  WmiInstance);


#ifdef WdfWmiInstanceDeregister
#undef WdfWmiInstanceDeregister
#endif
#define WdfWmiInstanceDeregister(arg1) sdv_WdfWmiInstanceDeregister(arg1);
VOID  sdv_WdfWmiInstanceDeregister(  
__in  WDFWMIINSTANCE  WmiInstance);


#ifdef WdfWmiInstanceGetDevice
#undef WdfWmiInstanceGetDevice
#endif
#define WdfWmiInstanceGetDevice sdv_WdfWmiInstanceGetDevice
WDFDEVICE  sdv_WdfWmiInstanceGetDevice(  
__in  WDFWMIINSTANCE  WmiInstance);


#ifdef WdfWmiInstanceGetProvider
#undef WdfWmiInstanceGetProvider
#endif
#define WdfWmiInstanceGetProvider sdv_WdfWmiInstanceGetProvider
WDFWMIPROVIDER  sdv_WdfWmiInstanceGetProvider(  
__in  WDFWMIINSTANCE  WmiInstance);


#ifdef WdfWmiInstanceFireEvent
#undef WdfWmiInstanceFireEvent
#endif
#define WdfWmiInstanceFireEvent sdv_WdfWmiInstanceFireEvent
NTSTATUS  sdv_WdfWmiInstanceFireEvent(  
__in  WDFWMIINSTANCE  WmiInstance,
   ULONG  EventDataSize,
__in_bcount_opt(EventDataSize)  PVOID  EventData);


#ifdef WdfWorkItemCreate
#undef WdfWorkItemCreate
#endif
#define WdfWorkItemCreate sdv_WdfWorkItemCreate
NTSTATUS  sdv_WdfWorkItemCreate(  
__in  PWDF_WORKITEM_CONFIG  Config,
__in  PWDF_OBJECT_ATTRIBUTES  Attributes,
__out  WDFWORKITEM*  WorkItem);


#ifdef WdfWorkItemEnqueue
#undef WdfWorkItemEnqueue
#endif
#define WdfWorkItemEnqueue(arg1) sdv_WdfWorkItemEnqueue(arg1);
VOID  sdv_WdfWorkItemEnqueue(  
__in  WDFWORKITEM  WorkItem);


#ifdef WdfWorkItemGetParentObject
#undef WdfWorkItemGetParentObject
#endif
#define WdfWorkItemGetParentObject sdv_WdfWorkItemGetParentObject
WDFOBJECT  sdv_WdfWorkItemGetParentObject(  
__in  WDFWORKITEM  WorkItem);


#ifdef WdfWorkItemFlush
#undef WdfWorkItemFlush
#endif
#define WdfWorkItemFlush(arg1) sdv_WdfWorkItemFlush(arg1);
VOID  sdv_WdfWorkItemFlush(  
__in  WDFWORKITEM  WorkItem);




#define WdfObjectGetTypedContextWorker sdv_WdfObjectGetTypedContextWorker
PVOID
sdv_WdfObjectGetTypedContextWorker(
      IN WDFOBJECT Handle,
      IN PCWDF_OBJECT_CONTEXT_TYPE_INFO TypeInfo
   );

#define WDF_DRIVER_CONFIG_INIT sdv_WDF_DRIVER_CONFIG_INIT
VOID
sdv_WDF_DRIVER_CONFIG_INIT(
    OUT PWDF_DRIVER_CONFIG Config,
    IN PFN_WDF_DRIVER_DEVICE_ADD EvtDriverDeviceAdd
    );

#define WdfDriverOpenParametersRegistryKey sdv_WdfDriverOpenParametersRegistryKey
NTSTATUS
sdv_WdfDriverOpenParametersRegistryKey(
    IN WDFDRIVER Driver,
    IN ACCESS_MASK DesiredAccess,
    IN OPTIONAL PWDF_OBJECT_ATTRIBUTES KeyAttributes,
    OUT WDFKEY* Key
    );

#define WdfDriverWdmGetDriverObject sdv_WdfDriverWdmGetDriverObject
PDRIVER_OBJECT
sdv_WdfDriverWdmGetDriverObject(
    IN WDFDRIVER Driver
    );

#define WdfDeviceCreate sdv_WdfDeviceCreate
WDFAPI
NTSTATUS
sdv_WdfDeviceCreate(
    IN OUT PWDFDEVICE_INIT* DeviceInit,
    IN OPTIONAL PWDF_OBJECT_ATTRIBUTES DeviceAttributes,
    OUT WDFDEVICE* Device
    );
#define WdfDeviceCreateSymbolicLink sdv_WdfDeviceCreateSymbolicLink
WDFAPI NTSTATUS
sdv_WdfDeviceCreateSymbolicLink(
    IN WDFDEVICE  Device,
    IN PCUNICODE_STRING  SymbolicLinkName
    );

#define WdfDeviceGetDriver sdv_WdfDeviceGetDriver
WDFAPI WDFDRIVER
sdv_WdfDeviceGetDriver(
   IN WDFDEVICE  Device
   );
#define WdfDeviceGetDefaultQueue sdv_WdfDeviceGetDefaultQueue
WDFQUEUE
sdv_WdfDeviceGetDefaultQueue(
    IN WDFDEVICE Device
    );
#define WdfDeviceOpenRegistryKey sdv_WdfDeviceOpenRegistryKey
NTSTATUS
sdv_WdfDeviceOpenRegistryKey(
    IN WDFDEVICE Device,
    IN ULONG DeviceInstanceKeyType,
    IN ACCESS_MASK DesiredAccess,
    IN OPTIONAL PWDF_OBJECT_ATTRIBUTES KeyAttributes,
    OUT WDFKEY* Key
    );
    
#ifdef WDF_IO_QUEUE_CONFIG_INIT
#undef WDF_IO_QUEUE_CONFIG_INIT
#endif
#define WDF_IO_QUEUE_CONFIG_INIT sdv_WDF_IO_QUEUE_CONFIG_INIT
VOID
sdv_WDF_IO_QUEUE_CONFIG_INIT(
    OUT PWDF_IO_QUEUE_CONFIG      Config,
    IN WDF_IO_QUEUE_DISPATCH_TYPE DispatchType
    );
#ifdef WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE
#undef WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE
#endif
#define WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE sdv_WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE
VOID
sdv_WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(
    OUT PWDF_IO_QUEUE_CONFIG      Config,
    IN WDF_IO_QUEUE_DISPATCH_TYPE DispatchType
    );
#define WdfIoQueueCreate sdv_WdfIoQueueCreate
WDFAPI
NTSTATUS
sdv_WdfIoQueueCreate(
    IN WDFDEVICE Device,
    IN PWDF_IO_QUEUE_CONFIG Config,
    IN OPTIONAL PWDF_OBJECT_ATTRIBUTES QueueAttributes,
    OUT WDFQUEUE* Queue
    );
#define WdfIoQueueGetDevice sdv_WdfIoQueueGetDevice
WDFDEVICE
sdv_WdfIoQueueGetDevice(
    IN WDFQUEUE Queue
    );
#define WDF_TIMER_CONFIG_INIT_PERIODIC sdv_WDF_TIMER_CONFIG_INIT_PERIODIC
VOID
sdv_WDF_TIMER_CONFIG_INIT_PERIODIC(
    IN PWDF_TIMER_CONFIG Config,
    IN PFN_WDF_TIMER     EvtTimerFunc,
    IN ULONG             Period
    );
#define WdfTimerCreate sdv_WdfTimerCreate
WDFAPI
NTSTATUS
sdv_WdfTimerCreate(
    IN  PWDF_TIMER_CONFIG            Config,
    IN  PWDF_OBJECT_ATTRIBUTES Attributes,
    OUT WDFTIMER*              Timer
    );
#define WdfTimerStart sdv_WdfTimerStart
BOOLEAN
sdv_WdfTimerStart(
    IN WDFTIMER Timer,
    IN LONGLONG DueTime
    );
#define WdfTimerStop sdv_WdfTimerStop
BOOLEAN
sdv_WdfTimerStop(
    IN WDFTIMER Timer,
    IN BOOLEAN Wait
    );
#define WdfTimerGetParentObject sdv_WdfTimerGetParentObject
WDFOBJECT
FORCEINLINE
sdv_WdfTimerGetParentObject(
    IN WDFTIMER Timer
    );
#define WDF_WORKITEM_CONFIG_INIT sdv_WDF_WORKITEM_CONFIG_INIT
VOID
sdv_WDF_WORKITEM_CONFIG_INIT(
    OUT PWDF_WORKITEM_CONFIG Config,
    IN PFN_WDF_WORKITEM     EvtWorkItemFunc
    );

#define WdfWorkItemCreate sdv_WdfWorkItemCreate
NTSTATUS
sdv_WdfWorkItemCreate(
    IN PWDF_WORKITEM_CONFIG Config,
	IN OPTIONAL PWDF_OBJECT_ATTRIBUTES Attributes,
    OUT WDFWORKITEM* WorkItem
    );
#define WDF_DPC_CONFIG_INIT sdv_WDF_DPC_CONFIG_INIT
VOID
sdv_WDF_DPC_CONFIG_INIT(
    IN PWDF_DPC_CONFIG Config,
    IN PFN_WDF_DPC     EvtDpcFunc
    );
#define WdfDpcCreate sdv_WdfDpcCreate
NTSTATUS
sdv_WdfDpcCreate(
    IN PWDF_DPC_CONFIG Config,
    IN PWDF_OBJECT_ATTRIBUTES Attributes,
    OUT WDFDPC* Dpc);
#define WdfDpcEnqueue sdv_WdfDpcEnqueue
BOOLEAN
sdv_WdfDpcEnqueue(
    IN WDFDPC Dpc
    );
#define WdfDpcCancel sdv_WdfDpcCancel
BOOLEAN
sdv_WdfDpcCancel(
    IN WDFDPC Dpc,
    IN BOOLEAN Wait
    );
#define WdfCommonBufferGetLength sdv_WdfCommonBufferGetLength
size_t 
sdv_WdfCommonBufferGetLength(
    IN WDFCOMMONBUFFER CommonBuffer
    );

#define WdfCommonBufferGetAlignedLogicalAddress sdv_WdfCommonBufferGetAlignedLogicalAddress
PHYSICAL_ADDRESS
sdv_WdfCommonBufferGetAlignedLogicalAddress(
    IN WDFCOMMONBUFFER CommonBuffer
    );

#define WdfCommonBufferGetAlignedVirtualAddress sdv_WdfCommonBufferGetAlignedVirtualAddress
PVOID
sdv_WdfCommonBufferGetAlignedVirtualAddress(
    IN WDFCOMMONBUFFER CommonBuffer
    );

#define WdfCommonBufferCreate sdv_WdfCommonBufferCreate
NTSTATUS
sdv_WdfCommonBufferCreate(
    IN WDFDMAENABLER DmaEnabler,
    IN size_t Length,
	IN OPTIONAL PWDF_OBJECT_ATTRIBUTES Attributes,
    OUT WDFCOMMONBUFFER* CommonBufferHandle
    );

#define WdfRequestCreate sdv_WdfRequestCreate
NTSTATUS 
sdv_WdfRequestCreate(
    IN OPTIONAL PWDF_OBJECT_ATTRIBUTES RequestAttributes,
    IN OPTIONAL WDFIOTARGET IoTarget,
    OUT WDFREQUEST* Request
    );

#define WdfRequestGetFileObject sdv_WdfRequestGetFileObject
WDFFILEOBJECT
sdv_WdfRequestGetFileObject(
    IN WDFREQUEST Request
    );
#define WdfRequestRetrieveOutputBuffer sdv_WdfRequestRetrieveOutputBuffer
NTSTATUS
sdv_WdfRequestRetrieveOutputBuffer(
	IN WDFREQUEST Request,
	IN size_t  MinimumRequiredSize,
	OUT PVOID* Buffer,
	OUT size_t* Length
	);

#define WdfRequestRetrieveInputBuffer sdv_WdfRequestRetrieveInputBuffer
NTSTATUS
sdv_WdfRequestRetrieveInputBuffer(
    IN WDFREQUEST Request,
    IN size_t  MinimumRequiredSize,
    OUT PVOID* Buffer,
    OUT size_t* Length
    );

#define WdfRequestRetrieveUnsafeUserInputBuffer sdv_WdfRequestRetrieveUnsafeUserInputBuffer
NTSTATUS
sdv_WdfRequestRetrieveUnsafeUserInputBuffer(
    IN WDFREQUEST Request,
    IN size_t MinimumRequiredLength,
    OUT PVOID* InputBuffer,
    OUT OPTIONAL size_t* Length
    );

#define WdfRequestRetrieveUnsafeUserOutputBuffer sdv_WdfRequestRetrieveUnsafeUserOutputBuffer
NTSTATUS
sdv_WdfRequestRetrieveUnsafeUserOutputBuffer(
    IN WDFREQUEST Request,
    IN size_t MinimumRequiredLength,
    OUT PVOID* OutputBuffer,
    OUT OPTIONAL size_t* Length
    );

#define WdfRequestRetrieveOutputMemory sdv_WdfRequestRetrieveOutputMemory
NTSTATUS
sdv_WdfRequestRetrieveOutputMemory(
	IN WDFREQUEST Request,
	OUT WDFMEMORY* Memory
	);

#define WdfRequestRetrieveInputMemory sdv_WdfRequestRetrieveInputMemory
NTSTATUS
sdv_WdfRequestRetrieveInputMemory(
	IN WDFREQUEST Request,
	OUT WDFMEMORY* Memory
	);

#define WdfRequestRetrieveOutputWdmMdl sdv_WdfRequestRetrieveOutputWdmMdl
NTSTATUS
sdv_WdfRequestRetrieveOutputWdmMdl(
	IN WDFREQUEST Request,
	OUT PMDL* Mdl
	);

#define WdfRequestRetrieveInputWdmMdl sdv_WdfRequestRetrieveInputWdmMdl
NTSTATUS
sdv_WdfRequestRetrieveInputWdmMdl(
	IN WDFREQUEST Request,
	OUT PMDL* Mdl
	);

#define WdfMemoryGetBuffer sdv_WdfMemoryGetBuffer
PVOID
sdv_WdfMemoryGetBuffer(
    IN WDFMEMORY Memory,
    OUT OPTIONAL size_t* BufferSize
    );

#define WDF_MEMORY_DESCRIPTOR_INIT_HANDLE sdv_WDF_MEMORY_DESCRIPTOR_INIT_HANDLE
VOID
sdv_WDF_MEMORY_DESCRIPTOR_INIT_HANDLE(
    OUT PWDF_MEMORY_DESCRIPTOR Descriptor,
    IN WDFMEMORY Memory,
    IN OPTIONAL PWDFMEMORY_OFFSET Offsets
    );

#define WDF_MEMORY_DESCRIPTOR_INIT_BUFFER sdv_WDF_MEMORY_DESCRIPTOR_INIT_BUFFER
VOID
sdv_WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(
    OUT PWDF_MEMORY_DESCRIPTOR Descriptor,
    IN PVOID Buffer,
    IN ULONG BufferLength
    );

#define WdfMemoryCreate sdv_WdfMemoryCreate
NTSTATUS
sdv_WdfMemoryCreate(
    PWDF_OBJECT_ATTRIBUTES Attributes,
    IN POOL_TYPE PoolType,
    IN OPTIONAL ULONG PoolTag,
    IN size_t BufferSize,
    OUT WDFMEMORY* Memory,
    OUT OPTIONAL PVOID* Buffer
    );

#define WdfMemoryCreatePreallocated sdv_WdfMemoryCreatePreallocated
NTSTATUS
sdv_WdfMemoryCreatePreallocated(
    IN OPTIONAL PWDF_OBJECT_ATTRIBUTES Attributes,
    IN PVOID Buffer,
    IN size_t BufferSize,
    OUT WDFMEMORY* Memory
    );

#define WdfMemoryAssignBuffer sdv_WdfMemoryAssignBuffer
NTSTATUS
sdv_WdfMemoryAssignBuffer(
    IN WDFMEMORY Memory,
    IN PVOID Buffer,
    IN size_t BufferSize
    );

#define WdfMemoryCopyFromBuffer sdv_WdfMemoryCopyFromBuffer
NTSTATUS
sdv_WdfMemoryCopyFromBuffer(
    IN WDFMEMORY DestinationMemory,
    IN size_t DestinationOffset,
    IN PVOID Buffer,
    IN size_t NumBytesToCopyFrom
    );

#define WdfMemoryCopyToBuffer sdv_WdfMemoryCopyToBuffer
NTSTATUS
sdv_WdfMemoryCopyToBuffer(
    IN WDFMEMORY SourceMemory,
    IN size_t SourceOffset,
    IN OUT PVOID Buffer,
    IN size_t NumBytesToCopyTo
    );

#define WDF_MEMORY_DESCRIPTOR_INIT_MDL sdv_WDF_MEMORY_DESCRIPTOR_INIT_MDL
VOID
sdv_WDF_MEMORY_DESCRIPTOR_INIT_MDL(
    OUT PWDF_MEMORY_DESCRIPTOR Descriptor,
    IN PMDL Mdl,
    IN ULONG BufferLength
    );

#define WdfDmaTransactionInitialize sdv_WdfDmaTransactionInitialize
NTSTATUS
sdv_WdfDmaTransactionInitialize(
    IN WDFDMATRANSACTION DmaTransaction,
    IN PFN_WDF_PROGRAM_DMA EvtProgramDmaFunction,
    IN WDF_DMA_DIRECTION DmaDirection,
    IN PMDL Mdl,
    IN PVOID VirtualAddress,
    IN size_t  Length
    );

#define WdfDmaTransactionCreate sdv_WdfDmaTransactionCreate
NTSTATUS
sdv_WdfDmaTransactionCreate(
    IN WDFDMAENABLER DmaEnabler,
    IN OPTIONAL PWDF_OBJECT_ATTRIBUTES Attributes,
    OUT WDFDMATRANSACTION* DmaTransaction
    );

#define WdfDmaTransactionGetDevice sdv_WdfDmaTransactionGetDevice
WDFDEVICE
sdv_WdfDmaTransactionGetDevice(
    IN WDFDMATRANSACTION DmaTransaction
    );

#define WdfDmaTransactionGetCurrentDmaTransferLength sdv_WdfDmaTransactionGetCurrentDmaTransferLength
size_t
sdv_WdfDmaTransactionGetCurrentDmaTransferLength(
    IN WDFDMATRANSACTION DmaTransaction
    );

#define WdfDmaTransactionGetRequest sdv_WdfDmaTransactionGetRequest
WDFREQUEST
sdv_WdfDmaTransactionGetRequest(
    IN WDFDMATRANSACTION DmaTransaction
    );


#define WdfDmaTransactionGetBytesTransferred sdv_WdfDmaTransactionGetBytesTransferred
size_t
sdv_WdfDmaTransactionGetBytesTransferred(
    IN WDFDMATRANSACTION DmaTransaction
    );

#define WdfDmaTransactionDmaCompletedFinal sdv_WdfDmaTransactionDmaCompletedFinal
BOOLEAN
sdv_WdfDmaTransactionDmaCompletedFinal(
    IN WDFDMATRANSACTION DmaTransaction,
    IN size_t FinalTransferredLength,
    OUT NTSTATUS* Status
    );

#define WdfDmaTransactionDmaCompletedWithLength sdv_WdfDmaTransactionDmaCompletedWithLength
BOOLEAN
sdv_WdfDmaTransactionDmaCompletedWithLength(
    IN WDFDMATRANSACTION DmaTransaction,
    IN size_t TransferredLength,
    OUT NTSTATUS* Status
    );

#define WdfDmaTransactionDmaCompleted sdv_WdfDmaTransactionDmaCompleted
BOOLEAN
sdv_WdfDmaTransactionDmaCompleted(
    IN WDFDMATRANSACTION DmaTransaction,
    OUT NTSTATUS* Status
    );

#define WdfDmaTransactionRelease sdv_WdfDmaTransactionRelease
NTSTATUS
sdv_WdfDmaTransactionRelease(
    IN WDFDMATRANSACTION DmaTransaction
    );

#define WdfDmaTransactionExecute sdv_WdfDmaTransactionExecute
NTSTATUS
sdv_WdfDmaTransactionExecute(
    IN WDFDMATRANSACTION DmaTransaction,
    IN OPTIONAL PVOID Context
    );

#define WdfDmaTransactionInitializeUsingRequest sdv_WdfDmaTransactionInitializeUsingRequest
NTSTATUS
sdv_WdfDmaTransactionInitializeUsingRequest(
    IN WDFDMATRANSACTION DmaTransaction,
    IN WDFREQUEST Request,
    IN PFN_WDF_PROGRAM_DMA EvtProgramDmaFunction,
    IN WDF_DMA_DIRECTION DmaDirection
    );
#define WdfDmaEnablerGetMaximumScatterGatherElements sdv_WdfDmaEnablerGetMaximumScatterGatherElements
size_t
sdv_WdfDmaEnablerGetMaximumScatterGatherElements(
    IN WDFDMAENABLER DmaEnabler
    );

#define WdfDmaEnablerGetMaximumLength sdv_WdfDmaEnablerGetMaximumLength
size_t
sdv_WdfDmaEnablerGetMaximumLength(
    IN WDFDMAENABLER DmaEnabler
    );

#define WdfDmaEnablerCreate sdv_WdfDmaEnablerCreate
NTSTATUS
sdv_WdfDmaEnablerCreate(
    IN WDFDEVICE Device,
    IN PWDF_DMA_ENABLER_CONFIG Config,
    IN OPTIONAL PWDF_OBJECT_ATTRIBUTES Attributes,
    OUT WDFDMAENABLER* DmaEnablerHandle
    );

#define WDF_DMA_ENABLER_CONFIG_INIT sdv_WDF_DMA_ENABLER_CONFIG_INIT
VOID
sdv_WDF_DMA_ENABLER_CONFIG_INIT(
    IN  PWDF_DMA_ENABLER_CONFIG  Config,
    IN  WDF_DMA_PROFILE          Profile,
    IN  size_t                   MaximumLength
    );
#define WdfRegistryAssignULong sdv_WdfRegistryAssignULong
NTSTATUS
sdv_WdfRegistryAssignULong(
    IN WDFKEY Key,
    IN PCUNICODE_STRING ValueName,
    IN ULONG Value
    );

#define WdfRegistryAssignString sdv_WdfRegistryAssignString
NTSTATUS
sdv_WdfRegistryAssignString(
    IN WDFKEY Key,
    IN PCUNICODE_STRING ValueName,
    IN WDFSTRING String
    );

#define WdfRegistryAssignMultiString sdv_WdfRegistryAssignMultiString
NTSTATUS
sdv_WdfRegistryAssignMultiString(
    IN WDFKEY Key,
    IN PCUNICODE_STRING ValueName,
    IN WDFCOLLECTION StringsCollection
    );

#define WdfRegistryAssignMemory sdv_WdfRegistryAssignMemory
NTSTATUS
sdv_WdfRegistryAssignMemory(
    IN WDFKEY Key,
    IN PCUNICODE_STRING ValueName,
    IN ULONG ValueType,
    IN WDFMEMORY Memory,
    IN OPTIONAL PWDFMEMORY_OFFSET MemoryOffsets
    );

#define WdfRegistryAssignValue sdv_WdfRegistryAssignValue
NTSTATUS
sdv_WdfRegistryAssignValue(
    IN WDFKEY Key,
    IN PCUNICODE_STRING ValueName,
    IN ULONG ValueType,
    IN ULONG ValueLength,
    IN PVOID Value
    );

#define WdfRegistryQueryULong sdv_WdfRegistryQueryULong
NTSTATUS
sdv_WdfRegistryQueryULong(
    IN WDFKEY Key,
    IN PCUNICODE_STRING ValueName,
    OUT PULONG Value
    );

#define WDF_OBJECT_ATTRIBUTES_INIT sdv_WDF_OBJECT_ATTRIBUTES_INIT
VOID
sdv_WDF_OBJECT_ATTRIBUTES_INIT(
    PWDF_OBJECT_ATTRIBUTES Attributes
    );
    
#pragma warning(disable:4005)
#define WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(_attributes,  _contexttype) \
    sdv_WDF_OBJECT_ATTRIBUTES_INIT(_attributes);
#pragma warning(default:4005)


#ifdef WdfObjectReference
#undef WdfObjectReference
#endif
#define WdfObjectReference sdv_WdfObjectReference
VOID
sdv_WdfObjectReference(
    IN WDFOBJECT Handle);


#ifdef WdfObjectDereference
#undef WdfObjectDereference
#endif
#define WdfObjectDereference sdv_WdfObjectDereference
VOID
sdv_WdfObjectDereference(
    IN WDFOBJECT Handle);


#define WDF_REQUEST_PARAMETERS_INIT sdv_WDF_REQUEST_PARAMETERS_INIT
VOID
sdv_WDF_REQUEST_PARAMETERS_INIT(
    OUT PWDF_REQUEST_PARAMETERS Parameters
    );

#define WdfRequestUnmarkCancelable sdv_WdfRequestUnmarkCancelable
NTSTATUS
sdv_WdfRequestUnmarkCancelable(
    IN WDFREQUEST Request
    );

#define WdfRequestIsCanceled sdv_WdfRequestIsCanceled
BOOLEAN
sdv_WdfRequestIsCanceled(
    IN WDFREQUEST Request
    );

#define WdfRequestSend sdv_WdfRequestSend
WDFAPI
BOOLEAN
sdv_WdfRequestSend(
    IN WDFREQUEST Request,
    IN WDFIOTARGET Target,
    IN OPTIONAL PWDF_REQUEST_SEND_OPTIONS RequestOptions
    );
#define WdfRequestGetStatus sdv_WdfRequestGetStatus
NTSTATUS
sdv_WdfRequestGetStatus(
    IN WDFREQUEST Request
    );
#define WdfRequestGetIoQueue sdv_WdfRequestGetIoQueue
WDFQUEUE
sdv_WdfRequestGetIoQueue(
    IN WDFREQUEST Request
    );
#define WdfIoTargetSendReadSynchronously  sdv_WdfIoTargetSendReadSynchronously 
WDFAPI
NTSTATUS
sdv_WdfIoTargetSendReadSynchronously (
    IN WDFIOTARGET IoTarget,
    IN OPTIONAL WDFREQUEST Request,
    IN OPTIONAL PWDF_MEMORY_DESCRIPTOR OutputBuffer,
    IN OPTIONAL PLONGLONG  DeviceOffset,
    IN OPTIONAL PWDF_REQUEST_SEND_OPTIONS RequestOptions,
    OUT OPTIONAL PULONG_PTR BytesRead
    );
#define WdfIoTargetSendWriteSynchronously  sdv_WdfIoTargetSendWriteSynchronously 
WDFAPI
NTSTATUS
sdv_WdfIoTargetSendWriteSynchronously (
    IN WDFIOTARGET IoTarget,
    IN OPTIONAL WDFREQUEST Request,
    IN OPTIONAL PWDF_MEMORY_DESCRIPTOR InputBuffer,
    IN OPTIONAL PLONGLONG DeviceOffset,
    IN OPTIONAL PWDF_REQUEST_SEND_OPTIONS RequestOptions,
    OUT OPTIONAL PULONG_PTR BytesWritten
    );
#define WdfIoTargetSendIoctlSynchronously  sdv_WdfIoTargetSendIoctlSynchronously 
WDFAPI
NTSTATUS
sdv_WdfIoTargetSendIoctlSynchronously (
    IN WDFIOTARGET IoTarget,
    IN OPTIONAL WDFREQUEST Request,
    IN ULONG IoctlCode,
    IN OPTIONAL PWDF_MEMORY_DESCRIPTOR InputBuffer,
    IN OPTIONAL PWDF_MEMORY_DESCRIPTOR OutputBuffer,
    IN OPTIONAL PWDF_REQUEST_SEND_OPTIONS RequestOptions,
    OUT OPTIONAL PULONG_PTR BytesReturned
    );
#define WdfIoTargetSendInternalIoctlSynchronously  sdv_WdfIoTargetSendInternalIoctlSynchronously 
WDFAPI
NTSTATUS
sdv_WdfIoTargetSendInternalIoctlSynchronously (
    IN WDFIOTARGET IoTarget,
    IN OPTIONAL WDFREQUEST Request,
    IN ULONG IoctlCode,
    IN OPTIONAL PWDF_MEMORY_DESCRIPTOR InputBuffer,
    IN OPTIONAL PWDF_MEMORY_DESCRIPTOR OutputBuffer,
    IN OPTIONAL PWDF_REQUEST_SEND_OPTIONS RequestOptions,
    OUT OPTIONAL PULONG_PTR BytesReturned
    );

#define WdfDeviceEnqueueRequest sdv_WdfDeviceEnqueueRequest
WDFAPI
NTSTATUS
sdv_WdfDeviceEnqueueRequest(
    IN WDFDEVICE Device,
    IN WDFREQUEST Request
    );
#define WdfRequestForwardToIoQueue sdv_WdfRequestForwardToIoQueue
NTSTATUS
sdv_WdfRequestForwardToIoQueue(
    IN WDFREQUEST Request,
    IN WDFQUEUE DestinationQueue
    );
#define WdfRequestStopAcknowledge sdv_WdfRequestStopAcknowledge
VOID
sdv_WdfRequestStopAcknowledge(
    IN WDFREQUEST Request,
    IN BOOLEAN Requeue
    );
#define WdfIoQueueRetrieveNextRequest sdv_WdfIoQueueRetrieveNextRequest
NTSTATUS
sdv_WdfIoQueueRetrieveNextRequest(
    IN WDFQUEUE Queue,
    OUT WDFREQUEST* OutRequest
    );
#define WdfIoQueueRetrieveRequestByFileObject sdv_WdfIoQueueRetrieveRequestByFileObject
NTSTATUS
sdv_WdfIoQueueRetrieveRequestByFileObject(
    IN WDFQUEUE Queue,
    IN WDFFILEOBJECT FileObject,
    OUT WDFREQUEST* OutRequest
    );
#define WdfIoQueueFindRequest sdv_WdfIoQueueFindRequest
NTSTATUS
sdv_WdfIoQueueFindRequest(
    IN WDFQUEUE Queue,
    IN WDFREQUEST FoundRequest,
    IN WDFFILEOBJECT FileObject,
    OUT PWDF_REQUEST_PARAMETERS Parameters,
    OUT WDFREQUEST* OutRequest
    );
#define WdfIoQueueRetrieveFoundRequest sdv_WdfIoQueueRetrieveFoundRequest
NTSTATUS
sdv_WdfIoQueueRetrieveFoundRequest(
    IN WDFQUEUE Queue,
    IN WDFREQUEST FoundRequest,
    OUT WDFREQUEST* OutRequest
    );
#define WdfIoQueueReadyNotify sdv_WdfIoQueueReadyNotify
NTSTATUS
sdv_WdfIoQueueReadyNotify(
    IN WDFQUEUE Queue,
    IN OPTIONAL PFN_WDF_IO_QUEUE_STATE QueueReady,
    IN OPTIONAL WDFCONTEXT Context
    );
#define WdfIoQueueGetState sdv_WdfIoQueueGetState
WDF_IO_QUEUE_STATE
sdv_WdfIoQueueGetState(
    IN WDFQUEUE Queue,
    OUT OPTIONAL PULONG QueueRequests,
    OUT OPTIONAL PULONG DriverRequests
    );
#ifdef WDF_IO_QUEUE_IDLE
#undef WDF_IO_QUEUE_IDLE
#endif
#define WDF_IO_QUEUE_IDLE sdv_WDF_IO_QUEUE_IDLE
BOOLEAN
sdv_WDF_IO_QUEUE_IDLE(
  IN WDF_IO_QUEUE_STATE Status);

#ifdef WdfIoQueueNoRequests
#undef WdfIoQueueNoRequests
#endif
#define WdfIoQueueNoRequests sdv_WdfIoQueueNoRequests

#ifdef WdfIoQueueDriverNoRequests
#undef WdfIoQueueDriverNoRequests
#endif
#define WdfIoQueueDriverNoRequests sdv_WdfIoQueueDriverNoRequests

#define WDF_PNPPOWER_EVENT_CALLBACKS_INIT sdv_WDF_PNPPOWER_EVENT_CALLBACKS_INIT
VOID
sdv_WDF_PNPPOWER_EVENT_CALLBACKS_INIT(
    OUT PWDF_PNPPOWER_EVENT_CALLBACKS Callbacks
    );

#define WdfDeviceInitAssignWdmIrpPreprocessCallback sdv_WdfDeviceInitAssignWdmIrpPreprocessCallback
NTSTATUS
sdv_WdfDeviceInitAssignWdmIrpPreprocessCallback(
    IN PWDFDEVICE_INIT DeviceInit,
    IN PFN_WDFDEVICE_WDM_IRP_PREPROCESS EvtDevicePreprocessWdmIrp,
    UCHAR MajorFunction,
    IN OPTIONAL PUCHAR MinorFunctions,
    ULONG NumMinorFunctions
    );
#define WdfFileObjectGetFileName sdv_WdfFileObjectGetFileName
PUNICODE_STRING
sdv_WdfFileObjectGetFileName(
    IN WDFFILEOBJECT FileObject
    );
#define WdfDeviceInitAssignName sdv_WdfDeviceInitAssignName
NTSTATUS
sdv_WdfDeviceInitAssignName(
	IN PWDFDEVICE_INIT  DeviceInit,
    IN PCUNICODE_STRING  DeviceName
    );

#define WdfDeviceInitRegisterPnpStateChangeCallback sdv_WdfDeviceInitRegisterPnpStateChangeCallback
NTSTATUS
sdv_WdfDeviceInitRegisterPnpStateChangeCallback(
    PWDFDEVICE_INIT DeviceInit,
    WDF_DEVICE_PNP_STATE PnpState,
    PFN_WDF_DEVICE_PNP_STATE_CHANGE_NOTIFICATION  EvtDevicePnpStateChange,
    ULONG CallbackTypes
    );
#define WdfDeviceInitRegisterPowerStateChangeCallback sdv_WdfDeviceInitRegisterPowerStateChangeCallback
NTSTATUS
sdv_WdfDeviceInitRegisterPowerStateChangeCallback(
    PWDFDEVICE_INIT DeviceInit,
    WDF_DEVICE_POWER_STATE PowerState,
    PFN_WDF_DEVICE_POWER_STATE_CHANGE_NOTIFICATION  EvtDevicePowerStateChange,
    ULONG CallbackTypes
    );
#define WdfDeviceInitRegisterPowerPolicyStateChangeCallback sdv_WdfDeviceInitRegisterPowerPolicyStateChangeCallback
NTSTATUS
sdv_WdfDeviceInitRegisterPowerPolicyStateChangeCallback(
    PWDFDEVICE_INIT DeviceInit,
    WDF_DEVICE_POWER_POLICY_STATE PowerPolicyState,
    PFN_WDF_DEVICE_POWER_POLICY_STATE_CHANGE_NOTIFICATION  EvtDevicePowerPolicyStateChange,
    ULONG CallbackTypes
    );
#define WdfDeviceInitAssignSDDLString sdv_WdfDeviceInitAssignSDDLString
NTSTATUS
sdv_WdfDeviceInitAssignSDDLString(
    IN PWDFDEVICE_INIT DeviceInit,
    IN PCUNICODE_STRING SDDLString
    );
#define WdfFdoInitOpenRegistryKey sdv_WdfFdoInitOpenRegistryKey
NTSTATUS
sdv_WdfFdoInitOpenRegistryKey(
    IN PWDFDEVICE_INIT DeviceInit,
    IN ULONG DeviceInstanceKeyType,
    IN ACCESS_MASK DesiredAccess,
    IN OPTIONAL PWDF_OBJECT_ATTRIBUTES KeyAttributes,
    OUT WDFKEY* Key
    );
#define WdfFdoInitQueryProperty sdv_WdfFdoInitQueryProperty
NTSTATUS
sdv_WdfFdoInitQueryProperty(
    IN PWDFDEVICE_INIT DeviceInit,
    IN DEVICE_REGISTRY_PROPERTY DeviceProperty,
    IN ULONG BufferLength,
    OUT PVOID PropertyBuffer,
    OUT PULONG ResultLength
    );
#define WdfFdoInitAllocAndQueryProperty sdv_WdfFdoInitAllocAndQueryProperty
NTSTATUS
sdv_WdfFdoInitAllocAndQueryProperty(
    IN PWDFDEVICE_INIT DeviceInit,
    IN DEVICE_REGISTRY_PROPERTY DeviceProperty,
    IN POOL_TYPE PoolType,
    IN OPTIONAL PWDF_OBJECT_ATTRIBUTES PropertyMemoryAttributes,
    OUT WDFMEMORY* PropertyMemory
    );
#define WdfFdoInitWdmGetPhysicalDevice sdv_WdfFdoInitWdmGetPhysicalDevice
PDEVICE_OBJECT
sdv_WdfFdoInitWdmGetPhysicalDevice(
    IN PWDFDEVICE_INIT  DeviceInit
	);
#define WdfPdoInitAssignDeviceID sdv_WdfPdoInitAssignDeviceID 
NTSTATUS
sdv_WdfPdoInitAssignDeviceID(
    IN PWDFDEVICE_INIT DeviceInit,
    IN PCUNICODE_STRING DeviceID
    );
#define WdfPdoInitAssignInstanceID sdv_WdfPdoInitAssignInstanceID
NTSTATUS
sdv_WdfPdoInitAssignInstanceID(
    IN PWDFDEVICE_INIT DeviceInit,
    IN PCUNICODE_STRING InstanceID
    );
#define WdfPdoInitAddHardwareID sdv_WdfPdoInitAddHardwareID
NTSTATUS
sdv_WdfPdoInitAddHardwareID(
    IN PWDFDEVICE_INIT DeviceInit,
    IN PCUNICODE_STRING HardwareID
    );
#define WdfPdoInitAddCompatibleID sdv_WdfPdoInitAddCompatibleID
NTSTATUS
sdv_WdfPdoInitAddCompatibleID(
    IN PWDFDEVICE_INIT DeviceInit,
    IN PCUNICODE_STRING CompatibleID
    );
#define WdfPdoInitAddDeviceText sdv_WdfPdoInitAddDeviceText
NTSTATUS
sdv_WdfPdoInitAddDeviceText(
    IN PWDFDEVICE_INIT DeviceInit,
    IN PCUNICODE_STRING DeviceDescription,
    IN PCUNICODE_STRING DeviceLocation,
    IN LCID LocaleId
    );
#define WdfPdoInitAllocate sdv_WdfPdoInitAllocate
PWDFDEVICE_INIT
sdv_WdfPdoInitAllocate(
    IN WDFDEVICE ParentDevice
    );
#define WDF_DEVICE_POWER_POLICY_IDLE_SETTINGS_INIT sdv_WDF_DEVICE_POWER_POLICY_IDLE_SETTINGS_INIT
VOID
sdv_WDF_DEVICE_POWER_POLICY_IDLE_SETTINGS_INIT(
    PWDF_DEVICE_POWER_POLICY_IDLE_SETTINGS Settings,
    WDF_POWER_POLICY_S0_IDLE_CAPABILITIES IdleCaps
    );
#define WdfDeviceAssignS0IdleSettings sdv_WdfDeviceAssignS0IdleSettings
NTSTATUS
sdv_WdfDeviceAssignS0IdleSettings(
    WDFDEVICE Device,
    PWDF_DEVICE_POWER_POLICY_IDLE_SETTINGS Settings
    );
#define WDF_DEVICE_POWER_POLICY_WAKE_SETTINGS_INIT sdv_WDF_DEVICE_POWER_POLICY_WAKE_SETTINGS_INIT
VOID
sdv_WDF_DEVICE_POWER_POLICY_WAKE_SETTINGS_INIT(
    PWDF_DEVICE_POWER_POLICY_WAKE_SETTINGS Settings
    );
#define WdfDeviceAssignSxWakeSettings sdv_WdfDeviceAssignSxWakeSettings
NTSTATUS
sdv_WdfDeviceAssignSxWakeSettings(
    WDFDEVICE Device,
    PWDF_DEVICE_POWER_POLICY_WAKE_SETTINGS Settings
    );
#define WDF_POWER_POLICY_EVENT_CALLBACKS_INIT sdv_WDF_POWER_POLICY_EVENT_CALLBACKS_INIT
VOID
sdv_WDF_POWER_POLICY_EVENT_CALLBACKS_INIT(
    PWDF_POWER_POLICY_EVENT_CALLBACKS Callbacks
    );
#define WdfCollectionCreate sdv_WdfCollectionCreate
NTSTATUS
sdv_WdfCollectionCreate(
	IN OPTIONAL PWDF_OBJECT_ATTRIBUTES CollectionAttributes,
    OUT WDFCOLLECTION *Collection
	);
#define WdfCollectionGetFirstItem sdv_WdfCollectionGetFirstItem
WDFOBJECT
sdv_WdfCollectionGetFirstItem(
    WDFCOLLECTION Collection
    );
#define WdfCollectionGetItem sdv_WdfCollectionGetItem
WDFOBJECT 
sdv_WdfCollectionGetItem(
    IN WDFCOLLECTION Collection,
    IN ULONG Index
    );
#define WdfCollectionAdd sdv_WdfCollectionAdd
WDFAPI NTSTATUS
sdv_WdfCollectionAdd(
    IN WDFCOLLECTION Collection,
    IN WDFOBJECT Object
    );
#define WdfCollectionGetCount sdv_WdfCollectionGetCount
WDFAPI ULONG
sdv_WdfCollectionGetCount(
    IN WDFCOLLECTION Collection
    );
#define WdfFdoQueryForInterface sdv_WdfFdoQueryForInterface
NTSTATUS 
sdv_WdfFdoQueryForInterface(
    IN WDFDEVICE Device,
    IN LPCGUID InterfaceType,
    IN OUT PINTERFACE Interface,
    IN USHORT Size,
    IN USHORT Version,
    IN OUT PVOID InterfaceSpecificData
    );
#define WDF_DEVICE_STATE_INIT sdv_WDF_DEVICE_STATE_INIT
VOID
sdv_WDF_DEVICE_STATE_INIT(
    OUT PWDF_DEVICE_STATE PnpDeviceState
    );
#define WdfDeviceAssignMofResourceName sdv_WdfDeviceAssignMofResourceName
NTSTATUS 
sdv_WdfDeviceAssignMofResourceName(
    IN WDFDEVICE Device,
    IN PCUNICODE_STRING MofResourceName
    );
#define WDF_INTERRUPT_CONFIG_INIT sdv_WDF_INTERRUPT_CONFIG_INIT
VOID
sdv_WDF_INTERRUPT_CONFIG_INIT(
    OUT PWDF_INTERRUPT_CONFIG Configuration,
    IN  PFN_WDF_INTERRUPT_ISR EvtInterruptIsr,
    IN  PFN_WDF_INTERRUPT_DPC EvtInterruptDpc
    );

#define WdfInterruptCreate sdv_WdfInterruptCreate
NTSTATUS
sdv_WdfInterruptCreate(
    IN WDFDEVICE Device,
    IN PWDF_INTERRUPT_CONFIG Configuration,
    IN OPTIONAL PWDF_OBJECT_ATTRIBUTES Attributes,
    OUT WDFINTERRUPT* Interrupt
    );

#define WdfInterruptSynchronize sdv_WdfInterruptSynchronize
BOOLEAN
sdv_WdfInterruptSynchronize(
    IN WDFINTERRUPT Interrupt,
    IN PFN_WDF_INTERRUPT_SYNCHRONIZE Callback,
    IN WDFCONTEXT Context
    );
#define WdfSpinLockCreate sdv_WdfSpinLockCreate
NTSTATUS
sdv_WdfSpinLockCreate(
    IN OPTIONAL PWDF_OBJECT_ATTRIBUTES SpinLockAttributes,
    OUT WDFSPINLOCK* SpinLock
    );
#define	WdfDeviceGetDevicePowerState	sdv_WdfDeviceGetDevicePowerState
WDF_DEVICE_POWER_STATE
sdv_WdfDeviceGetDevicePowerState(
    __in WDFDEVICE Device
    );
#define	WdfDeviceGetDevicePowerPolicyState	sdv_WdfDeviceGetDevicePowerPolicyState
WDF_DEVICE_POWER_POLICY_STATE
sdv_WdfDeviceGetDevicePowerPolicyState(
    __in WDFDEVICE Device
    );
#define	__sdv_save_request sdv_save_request
void sdv_save_request(void *r);
#define	__sdv_retrieve_request sdv_retrieve_request
void sdv_retrieve_request(void *r);

void sdv_io_read_begin();
void sdv_io_read_end();
void sdv_io_write_begin();
void sdv_io_write_end();
void sdv_io_default_begin();
void sdv_io_default_end();
void sdv_device_control_begin();
void sdv_device_control_end();
void sdv_internal_device_control_begin();
void sdv_internal_device_control_end();

#endif
