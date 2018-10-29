/*++

Copyright (C) 2004  Microsoft Corporation

Module Name:

    intrface.c

Abstract:

    This driver is the Microsoft Device Specific Module (DSM)
    devices that conform with SPC-3 specs.
    It exports behaviors that mpio.sys will use to determine how to
    multipath these devices.

    This file contains DriverEntry and all the functions that are
    exported to MPIO.

    This DSM is targetted towards Windows 2008 and above.

Environment:

    kernel mode only

--*/

#define DEBUG_MAIN_SOURCE 1

#include "precomp.h"

#ifdef DEBUG_USE_WPP
#include "intrface.tmh"
#endif

#pragma warning (disable:4305)


//
// Flag to indicate whether to ASSERT or ignore a paritcular condition.
//
BOOLEAN DoAssert = TRUE;

//
// OS Version Info
// MSDSM is targetted towards Windows Server 2008 and above.
//
BOOLEAN gServer2008AndAbove = FALSE;

//
// Global to cache MPIO's Control Object.
//
PDEVICE_OBJECT gMPIOControlObject = NULL;

//
// Flag to indicate if the MPIO control object was referenced.
//
BOOLEAN gMPIOControlObjectRefd = FALSE;

//
// Global to cache the Driver Object.
//
PDRIVER_OBJECT gDsmDriverObject = NULL;


#ifdef ALLOC_PRAGMA
    #pragma alloc_text(INIT, DriverEntry)
#endif

//
// The code.
//
NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    )
/*++

Routine Description:

    This routine is called when the driver is loaded.

Arguments:

    DriverObject  - Supplies the driver object.
    RegistryPath  - Supplies the registry path.

Return Value:

    NTSTATUS

--*/
#pragma warning (suppress: 28101) // This warning is flagged for informational purposes only
{
    PDSM_CONTEXT dsmContext = NULL;
    PFILE_OBJECT fileObject;
    WCHAR dosDeviceName[64] = DSM_MPIO_CONTROL_OBJECT_SYMLINK;
    UNICODE_STRING mpUnicodeName;
    NTSTATUS status = STATUS_SUCCESS;
    MPIO_VERSION_INFO versionInfo = {0};
    DSM_TYPE dsmMode = DsmType3;
    DSM_MPIO_CONTEXT mpctlContext;
    IO_STATUS_BLOCK ioStatus;


    //
    // Initialize the tracing subsystem.
    // Any failure is handled by ETW itself.
    //
    WPP_INIT_TRACING(DriverObject, RegistryPath);

    TracePrint((TRACE_LEVEL_VERBOSE,
                TRACE_FLAG_INIT,
                "DriverEntry (DrvObj %p): Entering function.\n",
                DriverObject));

    gDsmDriverObject = DriverObject;

    //
    // Determine the OS version.
    //
    gServer2008AndAbove = RtlIsNtDdiVersionAvailable(NTDDI_VISTA);

    TracePrint((TRACE_LEVEL_INFORMATION,
                TRACE_FLAG_INIT,
                "DriverEntry (DrvObj %p): Server2008AndAbove is %!bool!.\n",
                DriverObject,
                gServer2008AndAbove));

    //
    // MSDSM is supported only on Server 2008 and above.
    //
    if (!gServer2008AndAbove) {

        status = STATUS_NOT_SUPPORTED;
        goto __Exit_DriverEntry;
    }

    //
    // Build the mpio symbolic link name.
    //
    RtlInitUnicodeString(&mpUnicodeName, dosDeviceName);

    //
    // Get a pointer to mpio's deviceObject.
    //
    status = IoGetDeviceObjectPointer(&mpUnicodeName,
                                      FILE_READ_ATTRIBUTES,
                                      &fileObject,
                                      &gMPIOControlObject);

    if (!NT_SUCCESS(status)) {

        TracePrint((TRACE_LEVEL_FATAL,
                    TRACE_FLAG_INIT,
                    "DriverEntry (DrvObj %p): Failed to communicate with MPIO control object. Status %x.\n",
                    DriverObject,
                    status));

        goto __Exit_DriverEntry;
    }

    ObReferenceObject(gMPIOControlObject);
    gMPIOControlObjectRefd = TRUE;
    ObDereferenceObject(fileObject);

    status = DsmGetVersion(&versionInfo, sizeof(MPIO_VERSION_INFO));

    if (!NT_SUCCESS(status)) {

        //
        // If we can't get the version, that means we aren't using a compatible
        // version of MPIO drivers and so should not continue.
        //
        TracePrint((TRACE_LEVEL_FATAL,
                    TRACE_FLAG_INIT,
                    "DriverEntry (DrvObj %p): MPIO version unknown - DSM exiting.\n",
                    DriverObject));

        status = STATUS_UNSUCCESSFUL;
        goto __Exit_DriverEntry;
    }

    TracePrint((TRACE_LEVEL_INFORMATION,
                TRACE_FLAG_INIT,
                "DriverEntry (DrvObj %p): MPIO version %d.%d.%d.%d.\n",
                DriverObject,
                versionInfo.MajorVersion,
                versionInfo.MinorVersion,
                versionInfo.ProductBuild,
                versionInfo.QfeNumber));

    RtlZeroMemory(&gDsmInitData, sizeof(DSM_INIT_DATA));

    //
    // Must be newer than 1.0.7.0 to support DSM type 2 upwards.
    //
    if ((versionInfo.MajorVersion > 1)  ||
        (versionInfo.MinorVersion >= 1) ||
        (versionInfo.ProductBuild > 7)  ||
        (versionInfo.QfeNumber    >= 1)) {

        //
        // Must be newer than 1.18 to support DSM's versioning
        //
        if (versionInfo.MajorVersion > 1 ||
            versionInfo.MinorVersion > 17) {

            dsmMode = DsmType5;

            {
                RTL_OSVERSIONINFOW osVersion = {0};

                osVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFOW);
                RtlGetVersion(&osVersion);

                gDsmInitData.DsmVersion.MajorVersion = osVersion.dwMajorVersion;
                gDsmInitData.DsmVersion.MinorVersion = osVersion.dwMinorVersion;
                gDsmInitData.DsmVersion.ProductBuild = osVersion.dwBuildNumber;
                gDsmInitData.DsmVersion.QfeNumber = 0;
            }
        }
    } else {

        //
        // We cannot use this DSM with older versions of the MPIO drivers.
        //
        TracePrint((TRACE_LEVEL_FATAL,
                    TRACE_FLAG_INIT,
                    "DriverEntry (DrvObj %p): MPIO version not supported - DSM exiting.\n",
                    DriverObject));

        status = STATUS_UNSUCCESSFUL;
        goto __Exit_DriverEntry;
    }

    TracePrint((TRACE_LEVEL_INFORMATION,
                TRACE_FLAG_INIT,
                "DriverEntry (DrvObj %p): Setting DSM type to %d.\n",
                DriverObject,
                dsmMode));

    //
    // Build the init data structure.
    //
    dsmContext = DsmpAllocatePool(NonPagedPool,
                                  sizeof(DSM_CONTEXT),
                                  DSM_TAG_DSM_CONTEXT);
    if (!dsmContext) {

        TracePrint((TRACE_LEVEL_FATAL,
                    TRACE_FLAG_INIT,
                    "DriverEntry (DrvObj %p): Failed to allocate memory for DSM Context.\n",
                    DriverObject));

        status = STATUS_INSUFFICIENT_RESOURCES;
        goto __Exit_DriverEntry;
    }

    //
    // Set-up the init data
    //
    gDsmInitData.DsmContext = (PVOID) dsmContext;
    gDsmInitData.InitDataSize = sizeof(DSM_INIT_DATA);

    gDsmInitData.DsmInquireDriver = DsmInquire;
    gDsmInitData.DsmCompareDevices = DsmCompareDevices;
    gDsmInitData.DsmGetControllerInfo = DsmGetControllerInfo;
    gDsmInitData.DsmSetDeviceInfo = DsmSetDeviceInfo;
    gDsmInitData.DsmIsPathActive = DsmIsPathActive;
    gDsmInitData.DsmPathVerify = DsmPathVerify;
    gDsmInitData.DsmInvalidatePath = DsmInvalidatePath;
    gDsmInitData.DsmMoveDevice = DsmMoveDevice;
    gDsmInitData.DsmRemovePending = DsmRemovePending;
    gDsmInitData.DsmRemoveDevice = DsmRemoveDevice;
    gDsmInitData.DsmRemovePath = DsmRemovePath;
    gDsmInitData.DsmSrbDeviceControl = DsmSrbDeviceControl;
    gDsmInitData.DsmLBGetPath = DsmLBGetPath;
    gDsmInitData.DsmInterpretErrorEx = DsmInterpretError;
    gDsmInitData.DsmUnload = DsmUnload;
    gDsmInitData.DsmSetCompletion = DsmSetCompletion;
    gDsmInitData.DsmCategorizeRequest = DsmCategorizeRequest;
    gDsmInitData.DsmBroadcastSrb = DsmBroadcastRequest;

    //
    // Since MSDSM is for SPC-3 compliant devices, MPIO should be able to build
    // a serial number for the device.
    //
    gDsmInitData.DsmDeviceSerialNumber = NULL;

    //
    // Notifies MPIO of the appropriate Type support
    //
    gDsmInitData.DsmType = dsmMode;

    gDsmInitData.DriverObject = DriverObject;


    //
    // Set-up the WMI Info.
    //
    DsmpWmiInitialize(&gDsmInitData.DsmWmiInfo, RegistryPath);
    DsmpDsmWmiInitialize(&gDsmInitData.DsmWmiGlobalInfo, RegistryPath);

    RtlInitUnicodeString(&gDsmInitData.DisplayName, DSM_FRIENDLY_NAME);

    //
    // Initialize some of the fields in DSM Context structure.
    //
    KeInitializeSpinLock(&dsmContext->SupportedDevicesListLock);
    KeInitializeSpinLock(&dsmContext->DSMContextLock);
    InitializeListHead(&dsmContext->GroupList);
    InitializeListHead(&dsmContext->DeviceList);
    InitializeListHead(&dsmContext->FailGroupList);
    InitializeListHead(&dsmContext->ControllerList);
    InitializeListHead(&dsmContext->StaleFailGroupList);

    //
    // Build the list context structures used for completion processing.
    //
    ExInitializeNPagedLookasideList(&dsmContext->CompletionContextList,
                                    NULL,
                                    NULL,
                                    0,
                                    sizeof(DSM_COMPLETION_CONTEXT),
                                    DSM_TAG_GENERIC,
                                    0);

    RtlZeroMemory(&mpctlContext, sizeof(DSM_MPIO_CONTEXT));

    //
    // Send the IOCTL to mpio.sys to register ourselves.
    //
    DsmSendDeviceIoControlSynchronous(IOCTL_MPDSM_REGISTER,
                                      gMPIOControlObject,
                                      &gDsmInitData,
                                      &mpctlContext,
                                      sizeof(DSM_INIT_DATA),
                                      sizeof(DSM_MPIO_CONTEXT),
                                      TRUE,
                                      &ioStatus);

    status = ioStatus.Status;

    if (NT_SUCCESS(status)) {

        dsmContext->MPIOContext = mpctlContext.MPIOContext;

        TracePrint((TRACE_LEVEL_INFORMATION,
                    TRACE_FLAG_INIT,
                    "DriverEntry (DrvObj %p): Registered with MPIO.\n",
                    DriverObject));

#pragma warning (suppress: 28175) // DSM de-registers itself from MPIO during unload
        DriverObject->DriverUnload = DsmDriverUnload;

        //
        // Query the registry for disabling/enabling statistics gathering
        //
        if (STATUS_OBJECT_NAME_NOT_FOUND == DsmpGetStatsGatheringChoice(dsmContext, (PULONG)&dsmContext->DisableStatsGathering)) {

            //
            // If the value does not exist, write the default to registry.
            //
            DsmpSetStatsGatheringChoice(dsmContext, (ULONG)dsmContext->DisableStatsGathering);
        }
    }

__Exit_DriverEntry:

    if (NT_SUCCESS(status)) {

        TracePrint((TRACE_LEVEL_VERBOSE,
                    TRACE_FLAG_INIT,
                    "DriverEntry (DrvObj %p): Exiting function successfully.\n",
                    DriverObject));
    } else {

        //
        // Since the DSM is going to be unloaded but without DriverUnload being
        // called, we need to perform cleanup here.
        //
        DsmpFreeDSMResources(dsmContext);
        dsmContext = NULL;

        if (gMPIOControlObjectRefd) {

            //
            // Drop the reference on MPIO's control object.
            //
            ObDereferenceObject(gMPIOControlObject);
            gMPIOControlObjectRefd = FALSE;
        }

        TracePrint((TRACE_LEVEL_VERBOSE,
                    TRACE_FLAG_INIT,
                    "DriverEntry (DrvObj %p): Exiting function with status %x.\n",
                    DriverObject,
                    status));

        //
        // Stop the tracing subsystem.
        // NOTE: once we unregister ETW, no more TracePrint can be done, so we
        // must ensure that ETW unregister is the last thing that happens.
        //
        WPP_CLEANUP(gDsmDriverObject);
    }

    return status;
}


VOID
DsmDriverUnload(
    __in IN PDRIVER_OBJECT DriverObject
    )
/*++

Routine Description:

    This routine is called when the driver is unloaded.

Arguments:

    DriverObject  - Supplies the driver object.

Return Value:

    Nothing

--*/
{
    DSM_DEREGISTER_DATA deregisterData;
    IO_STATUS_BLOCK ioStatus;

    deregisterData.DeregisterDataSize = sizeof(DSM_DEREGISTER_DATA);
    deregisterData.DriverObject = DriverObject;
    deregisterData.DsmContext = gDsmInitData.DsmContext;
    deregisterData.MpioContext = ((PDSM_CONTEXT)(gDsmInitData.DsmContext))->MPIOContext;
    //
    // Send the IOCTL to mpio.sys to de-register ourselves.
    //
    DsmSendDeviceIoControlSynchronous(IOCTL_MPDSM_DEREGISTER,
                                      gMPIOControlObject,
                                      &deregisterData,
                                      NULL,
                                      sizeof(DSM_DEREGISTER_DATA),
                                      0,
                                      TRUE,
                                      &ioStatus);

    ASSERT(NT_SUCCESS(ioStatus.Status));

    return;
}


NTSTATUS
DsmInquire(
    __in IN PVOID DsmContext,
    __in IN PDEVICE_OBJECT TargetDevice,
    __in IN PDEVICE_OBJECT PortObject,
    __in IN PSTORAGE_DEVICE_DESCRIPTOR Descriptor,
    __in IN PSTORAGE_DEVICE_ID_DESCRIPTOR DeviceIdList,
    __out OUT PVOID *DsmIdentifier
    )
/*++

Routine Description:

    This routine is used to determine if TargetDevice belongs to
    the DSM. If this is a supported device DsmIdentifier will be
    updated with 'deviceInfo'.

Arguments:

    DsmContext   - Context value given to the multipath driver during
                   registration.
    TargetDevice - DeviceObject for the child device.
    PortObject   - The Port driver FDO on which TargetDevice resides.
    Descriptor   - Pointer to the device descriptor corresponding to TargetDevice.
                   Rehash of inquiry data, plus serial number information
                   (if applicable).
    DeviceIdList - VPD Page 0x83 information.
    DsmIdentifier - Pointer to be filled in by the DSM on success.

Return Value:

    STATUS_NOT_SUPPORTED - if not on the SupportList.
    STATUS_INSUFFICIENT_RESOURCES - No mem.
    STATUS_SUCCESS
--*/
{
    PDSM_CONTEXT dsmContext = DsmContext;
    PDSM_DEVICE_INFO deviceInfo = NULL;
    PDSM_GROUP_ENTRY group;
    BOOLEAN newGroup;
    PDSM_TARGET_PORT_GROUP_ENTRY targetPortGroupEntry = NULL;
    PDSM_TARGET_PORT_LIST_ENTRY targetPortEntry = NULL;
    PSTR serialNumber = NULL;
    SIZE_T serialNumberLength = 0;
    NTSTATUS status;
    ULONG allocationLength;
    BOOLEAN serialNumberAllocated = FALSE;
    KIRQL irql = PASSIVE_LEVEL; // Initialize variable to prevent C4701 error
    BOOLEAN supported = FALSE;
    BOOLEAN spinlockHeld = FALSE;
    UCHAR vendorId[9] = {0};
    UCHAR productId[17] = {0};
    INQUIRYDATA inquiryData;
    UCHAR alua = DSM_DEVINFO_ALUA_NOT_SUPPORTED;
    ULONG index;
    PDSM_IDS controllerObjects = NULL;
    PDEVICE_OBJECT controllerDeviceObject;
    PLIST_ENTRY entry = NULL;
    PSTORAGE_DESCRIPTOR_HEADER controllerIdHeader = NULL;
    PULONG relativeTargetPortId = NULL;
    PUSHORT targetPortGroupId = NULL;
    PUCHAR targetPortGroupsInfo = NULL;
    ULONG targetPortGroupsInfoLength = 0;
    PSTR controllerSerialNumber;
    BOOLEAN match = FALSE;
    BOOLEAN doneUpdating = FALSE;
    PDSM_CONTROLLER_LIST_ENTRY controllerEntry = NULL;
    PDSM_TARGET_PORT_DEVICELIST_ENTRY tp_device = NULL;
    PWSTR hardwareId = NULL;
    PWCHAR deviceName = NULL;
    PSCSI_ADDRESS controllerScsiAddress = NULL;
    ULONG tempResult = 0;
    ULONG maxPRRetryTimeDuringStateTransition = DSM_MAX_PR_UNIT_ATTENTION_RETRY_TIME;
    BOOLEAN useCacheForLeastBlocks = FALSE;
    ULONGLONG cacheSizeForLeastBlocks = 0;
    BOOLEAN fakeControllerEntryExists = FALSE;

#if DBG
    BOOLEAN multiport;
#endif

    TracePrint((TRACE_LEVEL_VERBOSE,
                TRACE_FLAG_PNP,
                "DsmInquire (DevObj %p): Entering function.\n",
                TargetDevice));

    //
    //  1. Get standard inquiry for the device. Check if SPC-3 compliant.
    //        If not compliant, check SupportedDeviceList.
    //  2. Create device serial number.
    //  3. Create a partially populated deviceInfo.
    //        DeviceDescriptor.
    //        SCSI address.
    //        Save off serial number.
    //        ALUA, port FDO, etc.
    //  4. Create device name.
    //  5. If ALUA support, send down Report Target Port Groups.
    //  6. Find the group. If none, build one.
    //  7. If new group, build target port groups and target ports info.
    //        Else, update target port groups and target ports info.
    //  8. If both implicit as well as explicit transitions allowed, disable implicit.
    //  9. Get list of controllers objects and get VPD 0x83 for each (only if no
    //     match for existing ones).
    //        Match returned ids of type 0x5 with what was returned in Report Target Port Groups.
    //        If no type 0x5 identifier, use SCSI address.
    //        Create controller list (delete stale entries).
    //

    //
    // Query the registry to find out what devices are being supported
    // on this machine.
    //
    DsmpGetDeviceList(dsmContext);

    status = DsmpGetStandardInquiryData(TargetDevice, &inquiryData);

    ASSERT(NT_SUCCESS(status));

    if (NT_SUCCESS(status)) {

        supported = DsmpCheckScsiCompliance(TargetDevice,
                                            &inquiryData,
                                            Descriptor,
                                            DeviceIdList);

    } else {

        TracePrint((TRACE_LEVEL_ERROR,
                    TRACE_FLAG_PNP,
                    "DsmInquire (DevObj %p): Failed to get inquiry data with status %x.\n",
                    TargetDevice,
                    status));

        status = STATUS_NOT_SUPPORTED;
        goto __Exit_DsmInquire;
    }

    //
    // Since the device isn't SPC-3 compliant, check if the device is on the
    // SupportedDeviceList.
    //
    if (!supported) {


        if (!supported) {

            //
            // Get the inquiry data embedded in the device descriptor.
            //
            RtlStringCchCopyA((LPSTR)vendorId,
                              sizeof(vendorId) / sizeof(vendorId[0]),
                              (LPCSTR)(&inquiryData.VendorId));

            RtlStringCchCopyA((LPSTR)productId,
                              sizeof(productId) / sizeof(productId[0]),
                              (LPCSTR)(&inquiryData.ProductId));

            supported = DsmpDeviceSupported(dsmContext,
                                            (PCSZ)vendorId,
                                            (PCSZ)productId);
        }

        if (!supported) {

            TracePrint((TRACE_LEVEL_ERROR,
                        TRACE_FLAG_PNP,
                        "DsmInquire (DevObj %p): Unsupported Device.\n",
                        TargetDevice));

            status = STATUS_NOT_SUPPORTED;
            goto __Exit_DsmInquire;
        }
    }

    //
    // Find out if device can be accessed via mulitple ports. This info is
    // important since it will determine whether or not to send down a
    // ReportTargetPortGroups command.
    //
#if DBG
    multiport = (inquiryData.MultiPort & 0x10) ? TRUE : FALSE;

    TracePrint((TRACE_LEVEL_INFORMATION,
                TRACE_FLAG_PNP,
                "DsmInquire (DevObj %p): Is %ws multiported.\n",
                TargetDevice,
                multiport ? L"" : L"not"));
#endif

    //
    // Query the assymmetric states transition method
    //
    switch ((inquiryData.Reserved >> 0x4) & 0x3) {
        case 1: alua = DSM_DEVINFO_ALUA_IMPLICIT;
                break;

        case 2: alua = DSM_DEVINFO_ALUA_EXPLICIT;
                break;

        case 3: alua = DSM_DEVINFO_ALUA_IMPLICIT | DSM_DEVINFO_ALUA_EXPLICIT;
                break;

        default: alua = DSM_DEVINFO_ALUA_NOT_SUPPORTED;
                break;
    }

    //
    // Get some information about this device. The preferred info is
    // from the Device ID Page.
    //
    if (DeviceIdList) {

        STORAGE_IDENTIFIER_CODE_SET codeSet;

        //
        // This will parse out the 'best' identifier and return
        // a NULL-terminated ascii string.
        //
        serialNumber = (PSTR)DsmpParseDeviceID(DeviceIdList, DSM_DEVID_SERIAL_NUMBER, NULL, &codeSet);

        if (!serialNumber) {

            TracePrint((TRACE_LEVEL_ERROR,
                        TRACE_FLAG_PNP,
                        "DsmInquire (DevObj %p): NULL serial number.\n",
                        TargetDevice));

            //
            // Either an allocation failed, or the DeviceIdList is malformed.
            //
            status = STATUS_NOT_SUPPORTED;
            goto __Exit_DsmInquire;
        }

        //
        // Indicate that the serialnumber buffer is allocated.
        //
        serialNumberAllocated = TRUE;
        serialNumberLength = strlen((const char*)serialNumber);

    } else {

        //
        // Get the serial number of this device. Use the serial number
        // page (0x80). Ensure that the device's serial number is
        // present. If not, can't claim support for  this drive.
        //
        if ((Descriptor->SerialNumberOffset == MAXULONG) ||
            (Descriptor->SerialNumberOffset == 0)) {

            //
            // The port driver currently doesn't get the VPD page 0x80,
            // if the device doesn't support GET_SUPPORTED_PAGES. Check to
            // see whether there actually is a serial number.
            //
            serialNumber = DsmpGetSerialNumber(TargetDevice);

            if (!serialNumber) {

                TracePrint((TRACE_LEVEL_ERROR,
                            TRACE_FLAG_PNP,
                            "DsmInquire (DevObj %p): serialNumber = NULL.\n",
                            TargetDevice));

                status = STATUS_NOT_SUPPORTED;
                goto __Exit_DsmInquire;

            } else {
                serialNumberAllocated = TRUE;
                serialNumberLength = strlen((const char*)serialNumber);
            }
        } else {

            PSTR serialNumberIndex;

            //
            // Get a pointer to the embedded serial number info.
            //
            serialNumber = (PCHAR)Descriptor + Descriptor->SerialNumberOffset;
            serialNumberIndex = serialNumber;

            while (serialNumberIndex && *serialNumberIndex != '\0') {
                serialNumberLength++;
            }
        }
    }

    //
    // Allocate for the device. This is also used as DsmId.
    //
    allocationLength = sizeof(DSM_DEVICE_INFO);

    //
    // As DSM_DEVICE_INFO has storage for the descriptor, add only
    // the additional stuff that's at the end.
    //
    status = RtlULongSub(Descriptor->Size, sizeof(STORAGE_DEVICE_DESCRIPTOR), &tempResult);
    if (!NT_SUCCESS(status)) {

        TracePrint((TRACE_LEVEL_ERROR,
                    TRACE_FLAG_PNP,
                    "DsmInquire (DevObj %p): Arithmetic underflow - status %x.\n",
                    TargetDevice,
                    status));

        status = STATUS_NOT_SUPPORTED;
        goto __Exit_DsmInquire;
    }

    status = RtlULongAdd(allocationLength, tempResult, &allocationLength);
    if (!NT_SUCCESS(status)) {

        TracePrint((TRACE_LEVEL_ERROR,
                    TRACE_FLAG_PNP,
                    "DsmInquire (DevObj %p): Arithmetic overflow - status %x.\n",
                    TargetDevice,
                    status));

        status = STATUS_NOT_SUPPORTED;
        goto __Exit_DsmInquire;
    }

    deviceInfo = DsmpAllocatePool(NonPagedPool,
                                  allocationLength,
                                  DSM_TAG_DEV_INFO);
    if (!deviceInfo) {

        TracePrint((TRACE_LEVEL_ERROR,
                    TRACE_FLAG_PNP,
                    "DsmInquire (DevObj %p): Failed to allocate Device Info.\n",
                    TargetDevice));

        status = STATUS_NOT_SUPPORTED;
        goto __Exit_DsmInquire;
    }

    deviceInfo->State = deviceInfo->PreviousState = deviceInfo->TempPreviousStateForLB = deviceInfo->ALUAState = deviceInfo->LastKnownGoodState = DSM_DEV_NOT_USED_STATE;
    deviceInfo->DesiredState = DSM_DEV_UNDETERMINED;
    //
    // Copy over the StorageDescriptor.
    //
    RtlCopyMemory(&deviceInfo->Descriptor,
                  Descriptor,
                  Descriptor->Size);

    //
    // Get the scsi address for this device.
    //
    status = DsmGetScsiAddress(TargetDevice,
                               &deviceInfo->ScsiAddress);

    if (!NT_SUCCESS(status)) {

        TracePrint((TRACE_LEVEL_ERROR,
                    TRACE_FLAG_PNP,
                    "DsmInquire (DevObj %p): Error %x while getting scsi address.\n",
                    TargetDevice,
                    status));

        status = STATUS_NOT_SUPPORTED;
        goto __Exit_DsmInquire;
    }

    //
    // Capture the serial number allocated flag.
    //
    deviceInfo->SerialNumberAllocated = serialNumberAllocated;

    //
    // Set the serial number.
    //
    if (!serialNumberAllocated) {

        PSTORAGE_DEVICE_DESCRIPTOR descriptor;
        PSTR serialNumberIndex;

        //
        // serialNumber is not pointing to the buffer passed by MPIO. Update
        // it to point to the Device Descriptor allocated by the DSM.
        //
        descriptor = &(deviceInfo->Descriptor);

        ASSERT(descriptor->SerialNumberOffset != 0 && descriptor->SerialNumberOffset != MAXULONG);

        serialNumber = (PCHAR)descriptor + descriptor->SerialNumberOffset;
        serialNumberIndex = serialNumber;

        while (serialNumberIndex && *serialNumberIndex != '\0') {
            serialNumberLength++;
        }
    }

    if (alua == (DSM_DEVINFO_ALUA_IMPLICIT | DSM_DEVINFO_ALUA_EXPLICIT)) {

        BOOLEAN disableImplicit = FALSE;

        status = DsmpDisableImplicitStateTransition(TargetDevice, &disableImplicit);
        ASSERT(NT_SUCCESS(status));

        if (NT_SUCCESS(status)) {

            if (disableImplicit) {

                alua &= ~DSM_DEVINFO_ALUA_IMPLICIT;
                TracePrint((TRACE_LEVEL_INFORMATION,
                            TRACE_FLAG_PNP,
                            "DsmInquire (DevObj %p): Disabled implicit ALUA state transition.\n",
                            TargetDevice));

                //
                // Record that the storage actually supported implicit also, but we
                // turned it OFF.
                //
                deviceInfo->ImplicitDisabled = TRUE;

            } else {

                TracePrint((TRACE_LEVEL_WARNING,
                            TRACE_FLAG_PNP,
                            "DsmInquire (DevObj %p): Storage support both transitions but does NOT allow disabling Implicit.\n",
                            TargetDevice));
            }
        } else {

            TracePrint((TRACE_LEVEL_WARNING,
                        TRACE_FLAG_PNP,
                        "DsmInquire (DevObj %p): Failed to disable implicit ALUA state transitions - status %x.\n",
                        TargetDevice,
                        status));
        }
    }

    deviceInfo->SerialNumber = serialNumber;

    //
    // Save the Physical Device Object (PDO) of the device.
    // Used to verify that no two devices have the same PDO.
    //
    deviceInfo->PortPdo = TargetDevice;

    //
    // Save the FDO of the adapter. Used for handling reserve\release
    //
    deviceInfo->PortFdo = PortObject;

    //
    // Set the signature.
    //
    deviceInfo->DeviceSig = DSM_DEVICE_SIG;

    deviceInfo->DsmContext = DsmContext;

    deviceInfo->ALUASupport = alua;

    //
    // Build the name (using serialnumber) that will be used as registry key
    // to store Load Balance settings for this device.
    //
#pragma warning (suppress:26035) // serialNumber from STORAGE_DEVICE_DESCRIPTOR is null-terminated.
    deviceName = DsmpBuildDeviceName(deviceInfo, serialNumber, serialNumberLength);

    if (!deviceName) {

        TracePrint((TRACE_LEVEL_ERROR,
                    TRACE_FLAG_PNP,
                    "DsmInquire (DevObj %p): Failed to allocate device name for %p.\n",
                    TargetDevice,
                    deviceInfo));

        status = STATUS_NOT_SUPPORTED;
        goto __Exit_DsmInquire;
    }


    //
    // Send down ReportTargetPortGroups command and keep the info handy.
    //
    if (alua != DSM_DEVINFO_ALUA_NOT_SUPPORTED) {

        status = DsmpReportTargetPortGroups(TargetDevice,
                                            &targetPortGroupsInfo,
                                            &targetPortGroupsInfoLength);

        if (!NT_SUCCESS(status)) {

            TracePrint((TRACE_LEVEL_ERROR,
                        TRACE_FLAG_PNP,
                        "DsmInquire (DevObj %p): Failed to report target port groups for %p. Status %x.\n",
                        TargetDevice,
                        deviceInfo,
                        status));

            status = STATUS_NOT_SUPPORTED;
            goto __Exit_DsmInquire;
        }

        //
        // We've just sent down an RPTG (relatively expensive operation), and it
        // succeeded, so sending down one more as part part of the initialization
        // in PathVerify() since it is going to be called almost immediately.
        //
        deviceInfo->IgnorePathVerify = TRUE;
    }

    //
    // Query the registry for max time to retry failed PR requests
    //
    DsmpGetMaxPRRetryTime(DsmContext, &maxPRRetryTimeDuringStateTransition);

    DsmpQueryCacheInformationFromRegistry(DsmContext,
                                          &useCacheForLeastBlocks,
                                          &cacheSizeForLeastBlocks);

    //
    // Build LUN's hardware id.  Needs to be called at PASSIVE_LEVEL, so
    // do it before grabbing the lock.  The hardware id of the group is
    // later set under the protection of the lock.
    //
    hardwareId = DsmpBuildHardwareId(deviceInfo);

    KeAcquireSpinLock(&dsmContext->DSMContextLock, &irql);
    spinlockHeld = TRUE;

    status = STATUS_SUCCESS;

    //
    // See if there is an existing Multi-path group to which this belongs.
    // (same serial number).
    //
    group = DsmpFindDevice(DsmContext, deviceInfo, FALSE);
    if (!group) {

        TracePrint((TRACE_LEVEL_INFORMATION,
                    TRACE_FLAG_PNP,
                    "DsmInquire (DevObj %p): First device %p in the group.\n",
                    TargetDevice,
                    deviceInfo));

        newGroup = TRUE;

        //
        // This device doesn't belong to any group yet. So Build a multi-path
        // group entry. This'll represents all paths to a particular device.
        //
        group = DsmpBuildGroupEntry(DsmContext, deviceInfo);
        if (group) {

            //
            // Set the registry key name for the new group
            //
            group->RegistryKeyName = deviceName;
            deviceName = NULL;

            //
            // Cache the LUN's hardware id
            //
            if (!hardwareId) {

                TracePrint((TRACE_LEVEL_WARNING,
                            TRACE_FLAG_PNP,
                            "DsmInquire (DevObj %p): Failed to build a hardwareId for %p.\n",
                            TargetDevice,
                            deviceInfo));
            }

            group->HardwareId = hardwareId;
            hardwareId = NULL;

            group->UseCacheForLeastBlocks = useCacheForLeastBlocks;
            group->CacheSizeForLeastBlocks = cacheSizeForLeastBlocks;

        } else {

            TracePrint((TRACE_LEVEL_ERROR,
                        TRACE_FLAG_PNP,
                        "DsmInquire (DevObj %p): Failed to allocate Group Entry for %p.\n",
                        TargetDevice,
                        deviceInfo));

            status = STATUS_NOT_SUPPORTED;
            goto __Exit_DsmInquire;
        }
    } else {

        TracePrint((TRACE_LEVEL_INFORMATION,
                    TRACE_FLAG_PNP,
                    "DsmInquire (DevObj %p): Found group %p for device %p.\n",
                    TargetDevice,
                    group,
                    deviceInfo));

        newGroup = FALSE;

        if (!group->HardwareId) {

            //
            // If we weren't successful in previously building the hardware id for this LUN,
            // retry doing it again now.
            //
            hardwareId = DsmpBuildHardwareId(deviceInfo);
            if (!hardwareId) {

                TracePrint((TRACE_LEVEL_WARNING,
                            TRACE_FLAG_PNP,
                            "DsmInquire (DevObj %p): Failed to build a hardwareId for %p.\n",
                            TargetDevice,
                            deviceInfo));
            }

            group->HardwareId = hardwareId;
            hardwareId = NULL;
        }

        //
        // Sanity check that we haven't been presented with device instances
        // with different ALUA support. So compare with the first device instance.
        //
        for (index = 0; index < DSM_MAX_PATHS; index++) {

            if (group->DeviceList[index]) {

                break;
            }
        }

        if (index < DSM_MAX_PATHS) {

            //
            // Only acceptable conditions are:
            // 1. both have same support,
            // 2. one has explicit, while other has both explicit-and-implicit (this
            //    is a potential valid case because DsmpDisableImplicitStateTransition
            //    may have failed).
            //
            if (!((deviceInfo->ALUASupport == group->DeviceList[index]->ALUASupport) ||
                  ((deviceInfo->ALUASupport == DSM_DEVINFO_ALUA_EXPLICIT && deviceInfo->ImplicitDisabled) &&
                   (group->DeviceList[index]->ALUASupport == (DSM_DEVINFO_ALUA_IMPLICIT | DSM_DEVINFO_ALUA_EXPLICIT))) ||
                  ((group->DeviceList[index]->ALUASupport == DSM_DEVINFO_ALUA_EXPLICIT && group->DeviceList[index]->ImplicitDisabled) &&
                   (deviceInfo->ALUASupport == (DSM_DEVINFO_ALUA_IMPLICIT | DSM_DEVINFO_ALUA_EXPLICIT))))) {

                TracePrint((TRACE_LEVEL_ERROR,
                            TRACE_FLAG_PNP,
                            "DsmInquire (DevObj %p): Mismatch in device instances' ALUA support %d vs %d.\n",
                            TargetDevice,
                            deviceInfo->ALUASupport,
                            group->DeviceList[index]->ALUASupport));

                status = STATUS_NOT_SUPPORTED;
                goto __Exit_DsmInquire;
            }
        }
    }

    if (NT_SUCCESS(status)) {

        ASSERT(group);

        group->MaxPRRetryTimeDuringStateTransition = maxPRRetryTimeDuringStateTransition;

        if (alua == DSM_DEVINFO_ALUA_NOT_SUPPORTED) {

            //
            // Since the device doesn't support ALUA, it is automatically
            // symmetric LU access.
            //
            group->Symmetric = TRUE;

            if (newGroup) {

                //
                // This is the first in the group, so make it the active device.
                // The actual active/passive devices will be set-up when
                // LB policies are set by the user.
                //
                deviceInfo->PreviousState = deviceInfo->State;
                deviceInfo->State = DSM_DEV_ACTIVE_OPTIMIZED;

            } else {

                //
                // Already something active, this will be the fail-over device
                // until the load-balance groups are set-up.
                //
                deviceInfo->PreviousState = deviceInfo->State;
                deviceInfo->State = DSM_DEV_STANDBY;
            }

        } else {

            if (alua == DSM_DEVINFO_ALUA_IMPLICIT) {

                //
                // Assume that the LU access is symmetric. When parsing the TPG
                // info, if we find that not all TPGs are in the same LU access
                // state, then we know that this the access is asymmetric.
                //
                group->Symmetric = TRUE;
            }

            //
            // Build TPG and TP info
            //
            status = DsmpParseTargetPortGroupsInformation(DsmContext,
                                                          group,
                                                          targetPortGroupsInfo,
                                                          targetPortGroupsInfoLength);

            if (!NT_SUCCESS(status)) {

                TracePrint((TRACE_LEVEL_ERROR,
                            TRACE_FLAG_PNP,
                            "DsmInquire (DevObj %p): Failed to build TPG information - status %x.\n",
                            TargetDevice,
                            status));

                status = STATUS_NOT_SUPPORTED;
                goto __Exit_DsmInquire;
            }

            for (index = 0; index < DSM_MAX_PATHS; index++) {

                PDSM_TARGET_PORT_GROUP_ENTRY targetPortGroup;

                targetPortGroup = group->TargetPortGroupList[index];

                if (targetPortGroup) {

                    DsmpUpdateTargetPortGroupDevicesStates(targetPortGroup, targetPortGroup->AsymmetricAccessState);
                }
            }

            //
            // Find the target port through which this devInfo was exposed.
            //
            relativeTargetPortId = (PULONG)DsmpParseDeviceID(DeviceIdList,
                                                             DSM_DEVID_RELATIVE_TARGET_PORT,
                                                             NULL,
                                                             NULL);
            ASSERT(relativeTargetPortId);

            if (!relativeTargetPortId) {

                TracePrint((TRACE_LEVEL_ERROR,
                            TRACE_FLAG_PNP,
                            "DsmInquire (DevObj %p): Couldn't retrieve relative TP id.\n",
                            TargetDevice));

                status = STATUS_NOT_SUPPORTED;
                goto __Exit_DsmInquire;
            }

            //
            // Find the target port group
            //
            targetPortGroupId = (PUSHORT)DsmpParseDeviceID(DeviceIdList,
                                                           DSM_DEVID_TARGET_PORT_GROUP,
                                                           NULL,
                                                           NULL);
            ASSERT(targetPortGroupId);

            if (targetPortGroupId) {

                //
                // Find the target port group entry
                //
                targetPortGroupEntry = DsmpFindTargetPortGroup(DsmContext,
                                                               group,
                                                               targetPortGroupId);

                ASSERT(targetPortGroupEntry);

                if (targetPortGroupEntry) {

                    //
                    // Look through the target port group to find the target port
                    //
                    targetPortEntry = DsmpFindTargetPort(DsmContext,
                                                         targetPortGroupEntry,
                                                         relativeTargetPortId);
                } else {

                    TracePrint((TRACE_LEVEL_ERROR,
                                TRACE_FLAG_PNP,
                                "DsmInquire (DevObj %p): Couldn't find TPG Id %x's entry.\n",
                                TargetDevice,
                                *targetPortGroupId));

                    status = STATUS_NOT_SUPPORTED;
                    goto __Exit_DsmInquire;
                }

                ASSERT(targetPortEntry);

                if (!targetPortEntry) {

                    TracePrint((TRACE_LEVEL_ERROR,
                                TRACE_FLAG_PNP,
                                "DsmInquire (DevObj %p): Couldn't find relative TP %x's entry.\n",
                                TargetDevice,
                                *relativeTargetPortId));

                    status = STATUS_NOT_SUPPORTED;
                    goto __Exit_DsmInquire;
                }

                //
                // Update the devInfo with the target port and target port group
                // info
                //
                deviceInfo->TargetPortGroup = targetPortGroupEntry;
                deviceInfo->TargetPort = targetPortEntry;
                deviceInfo->PreviousState = deviceInfo->State;
                deviceInfo->State = deviceInfo->ALUAState = deviceInfo->TargetPortGroup->AsymmetricAccessState;

                tp_device = DsmpAllocatePool(NonPagedPool,
                                             sizeof(DSM_TARGET_PORT_DEVICELIST_ENTRY),
                                             DSM_TAG_TP_DEVICE_LIST_ENTRY);

                if (!tp_device) {

                    TracePrint((TRACE_LEVEL_ERROR,
                                TRACE_FLAG_PNP,
                                "DsmInquire (DevObj %p): Insufficient resources allocating TP device list entry.\n",
                                TargetDevice));

                    status = STATUS_NOT_SUPPORTED;
                    goto __Exit_DsmInquire;
                }

                //
                // Add the device to the list of devices that are exposed via this target port.
                //
                tp_device->DeviceInfo = deviceInfo;
                InterlockedIncrement((LONG volatile*)&targetPortEntry->Count);
                InsertTailList(&targetPortEntry->TP_DeviceList, &tp_device->ListEntry);

            } else {

                TracePrint((TRACE_LEVEL_ERROR,
                            TRACE_FLAG_PNP,
                            "DsmInquire (DevObj %p): Failed to retrieve TPG Id.\n",
                            TargetDevice));

                status = STATUS_NOT_SUPPORTED;
                goto __Exit_DsmInquire;
            }
        }

        if (NT_SUCCESS(status)) {

            //
            // Add the deviceInfo to the list.
            //
            status = DsmpAddDeviceEntry(DsmContext,
                                        group,
                                        deviceInfo);
            if (NT_SUCCESS(status)) {

                *DsmIdentifier = deviceInfo;

                TracePrint((TRACE_LEVEL_INFORMATION,
                            TRACE_FLAG_PNP,
                            "DsmInquire (DevObj %p): Added device %p to group %p.\n",
                            TargetDevice,
                            *DsmIdentifier,
                            group));
            } else {

                TracePrint((TRACE_LEVEL_ERROR,
                            TRACE_FLAG_PNP,
                            "DsmInquire (DevObj %p): Failed to add device %p to group %p - status %x.\n",
                            TargetDevice,
                            deviceInfo,
                            group,
                            status));

                if (newGroup) {

                    DsmpRemoveGroupEntry(DsmContext, group, FALSE);

                    DsmpFreePool(group);
                    group = NULL;
                }

                status = STATUS_NOT_SUPPORTED;
                goto __Exit_DsmInquire;
            }
        }
    }

    KeReleaseSpinLock(&dsmContext->DSMContextLock, irql);
    spinlockHeld = FALSE;

    TracePrint((TRACE_LEVEL_INFORMATION,
                TRACE_FLAG_PNP,
                "DsmInquire (DevObj %p): Device %p added. State %d, Desired State %d\n",
                TargetDevice,
                deviceInfo,
                deviceInfo->State,
                deviceInfo->DesiredState));

    //
    // Update the global list of controller objects
    //
    controllerObjects = DsmGetAssociatedDevice(dsmContext->MPIOContext,
                                               PortObject,
                                               0x0C);
    if (controllerObjects) {

        //
        // Walk through the list and get VPD 0x83 data and associate the devInfo
        // with the controller object.
        //
        for (index = 0; index < controllerObjects->Count; index++) {

            STORAGE_IDENTIFIER_CODE_SET codeSet = StorageIdCodeSetReserved;

            controllerDeviceObject = (PDEVICE_OBJECT)controllerObjects->IdList[index];
            ASSERT(controllerDeviceObject);

            if (!controllerDeviceObject) {

                TracePrint((TRACE_LEVEL_WARNING,
                            TRACE_FLAG_PNP,
                            "DsmInquire (DevObj %p): Controller list %p's index %x is NULL.\n",
                            TargetDevice,
                            controllerObjects,
                            index));

                continue;
            }

            status = DsmpGetDeviceIdList(controllerDeviceObject, &controllerIdHeader);
            ASSERT(NT_SUCCESS(status));

            if (!NT_SUCCESS(status)) {

                TracePrint((TRACE_LEVEL_WARNING,
                            TRACE_FLAG_PNP,
                            "DsmInquire (DevObj %p): Failed to get DeviceId list for controller %p - status %x.\n",
                            TargetDevice,
                            controllerDeviceObject,
                            status));

                continue;
            }

            controllerSerialNumber = DsmpParseDeviceID((PSTORAGE_DEVICE_ID_DESCRIPTOR)controllerIdHeader,
                                                       DSM_DEVID_SERIAL_NUMBER,
                                                       NULL,
                                                       &codeSet);
            ASSERT(controllerSerialNumber);
            DsmpFreePool(controllerIdHeader);

            if (!controllerSerialNumber) {

                TracePrint((TRACE_LEVEL_WARNING,
                            TRACE_FLAG_PNP,
                            "DsmInquire (DevObj %p): Failed to parse serial number for controller %p.\n",
                            TargetDevice,
                            controllerDeviceObject));

                continue;
            }

            status = DsmGetScsiAddress(controllerDeviceObject, &controllerScsiAddress);
            ASSERT(NT_SUCCESS(status));

            if (!NT_SUCCESS(status)) {

                TracePrint((TRACE_LEVEL_WARNING,
                            TRACE_FLAG_PNP,
                            "DsmInquire (DevObj %p): Failed to get controller %p's scsi address - status %x.\n",
                            TargetDevice,
                            controllerDeviceObject,
                            status));

                if (controllerScsiAddress) {
                    DsmpFreePool(controllerScsiAddress);
                }

                continue;
            }

            controllerEntry = DsmpFindControllerEntry(DsmContext,
                                                      PortObject,
                                                      controllerScsiAddress,
                                                      controllerSerialNumber,
                                                      strlen(controllerSerialNumber),
                                                      codeSet,
                                                      TRUE);

            if (!controllerEntry) {

                controllerEntry = DsmpBuildControllerEntry(DsmContext,
                                                           controllerDeviceObject,
                                                           PortObject,
                                                           controllerScsiAddress,
                                                           controllerSerialNumber,
                                                           codeSet,
                                                           TRUE);

                if (!controllerEntry) {

                    TracePrint((TRACE_LEVEL_WARNING,
                                TRACE_FLAG_PNP,
                                "DsmInquire (DevObj %p): Failed to build an entry for controller %p.\n",
                                TargetDevice,
                                controllerDeviceObject));

                    continue;
                }

                InsertHeadList(&dsmContext->ControllerList, &controllerEntry->ListEntry);
                InterlockedIncrement((LONG volatile*)&dsmContext->NumberControllers);
            }

            ASSERT(controllerEntry);

            controllerEntry->DeviceObject = controllerDeviceObject;

            //
            // Parse the DeviceIdList for all the 0x5 type identifiers
            // and for each, compare the target port groups and target ports to match
            // the device to its controller.
            //
            if (!match) {

                TracePrint((TRACE_LEVEL_WARNING,
                            TRACE_FLAG_PNP,
                            "DsmInquire (DevObj %p): Failed to match devInfo %p with controller %p's Ids.\n",
                            TargetDevice,
                            deviceInfo,
                            controllerDeviceObject));

                match = DsmpIsDeviceBelongsToController(DsmContext,
                                                        deviceInfo,
                                                        controllerEntry);
            }

            if (match && !doneUpdating) {

                InterlockedIncrement((LONG volatile*)&(controllerEntry->RefCount));
                deviceInfo->Controller = controllerEntry;
                doneUpdating = TRUE;
            }
        }
    }

    //
    // If there was no controller to associate this device with, use a fake one.
    // Note that we only really care about matching on the Port and Target
    // portions of the SCSI address.
    //
    if (!deviceInfo->Controller) {

        for (entry = dsmContext->ControllerList.Flink;
             entry != &dsmContext->ControllerList;
             entry = entry->Flink) {

            controllerEntry = CONTAINING_RECORD(entry, DSM_CONTROLLER_LIST_ENTRY, ListEntry);

            if ((controllerEntry->IsFakeController) &&
                (controllerEntry->ScsiAddress->PortNumber == deviceInfo->ScsiAddress->PortNumber) &&
                (controllerEntry->ScsiAddress->TargetId == deviceInfo->ScsiAddress->TargetId)) {

                fakeControllerEntryExists = TRUE;
                break;
            }
        }

        //
        // If no fake one exists as yet for this port FDO, create one now.
        //
        if (!fakeControllerEntryExists) {

            CHAR fakeControllerSerialNumber[] = "FakeController";
            SCSI_ADDRESS fakeControllerScsiAddress = {0};
            fakeControllerScsiAddress.PortNumber = deviceInfo->ScsiAddress->PortNumber;
            fakeControllerScsiAddress.TargetId = deviceInfo->ScsiAddress->TargetId;

            controllerEntry = DsmpBuildControllerEntry(DsmContext,
                                                       NULL,
                                                       PortObject,
                                                       &fakeControllerScsiAddress,
                                                       fakeControllerSerialNumber,
                                                       StorageIdCodeSetBinary,
                                                       TRUE);

            if (controllerEntry) {

                InsertHeadList(&dsmContext->ControllerList, &controllerEntry->ListEntry);
                InterlockedIncrement((LONG volatile*)&dsmContext->NumberControllers);
                controllerEntry->IsFakeController = TRUE;
            }
        }

        InterlockedIncrement((LONG volatile*)&(controllerEntry->RefCount));
        deviceInfo->Controller = controllerEntry;
    }

__Exit_DsmInquire:

    if (spinlockHeld) {
        KeReleaseSpinLock(&dsmContext->DSMContextLock, irql);
    }

    if (NT_SUCCESS(status)) {

        ASSERT(*DsmIdentifier);

    } else {

        //
        // Check to see whether the serial number buffer was allocated, or just
        // an offset into the Descriptor.
        //
        if (serialNumberAllocated) {

            //
            // Need to free this before returning.
            //
            DsmpFreePool(serialNumber);
        }

        if (deviceInfo) {
            DsmpFreePool(deviceInfo);
        }
    }

    //
    // If deviceName is not NULL then it hasn't been assigned to any GROUP.
    // Free the allocated memory.
    //
    if (deviceName) {
        DsmpFreePool(deviceName);
    }

    //
    // If hardwareId is not NULL then it hasn't been assigned to any GROUP.
    // Free the allocated memory.
    //
    if (hardwareId) {
        DsmpFreePool(hardwareId);
    }

    if (targetPortGroupsInfo) {
        DsmpFreePool(targetPortGroupsInfo);
    }

    if (relativeTargetPortId) {
        DsmpFreePool(relativeTargetPortId);
    }

    if (targetPortGroupId) {
        DsmpFreePool(targetPortGroupId);
    }

    if (controllerObjects) {
        DsmpFreePool(controllerObjects);
    }

    TracePrint((TRACE_LEVEL_VERBOSE,
                TRACE_FLAG_PNP,
                "DsmInquire (DevObj %p): Exiting function with status %x.\n",
                TargetDevice,
                status));

    return status;
}


BOOLEAN
DsmCompareDevices(
    __in IN PVOID DsmContext,
    __in IN PVOID DsmId1,
    __in IN PVOID DsmId2
    )
/*++

Routine Description:

    This routine is called to determine if the device ids represent
    the same underlying physical device.

Arguments:

    DsmContext - Context value given to the multipath driver during
                 registration.
    DsmId1/2 - Identifers returned from DMS_INQUIRE_DRIVER.

Return Value:

    TRUE if DsmIds correspond to the same underlying device.

--*/
{
    PDSM_DEVICE_INFO deviceInfo0 = DsmId1;
    PDSM_DEVICE_INFO deviceInfo1 = DsmId2;
    PSTR serialNumber0;
    PSTR serialNumber1;
    SIZE_T length;
    BOOLEAN match = FALSE;

    UNREFERENCED_PARAMETER(DsmContext);

    TracePrint((TRACE_LEVEL_VERBOSE,
                TRACE_FLAG_PNP,
                "DsmCompareDevices (DevInfo %p): Entering function - comparing with %p.\n",
                deviceInfo0,
                deviceInfo1));

    //
    // Get the two serial numbers. They were either embedded in
    // the STORAGE_DEVICE_DESCRIPTOR or built by directly issuing
    // the VPD request.
    //
    serialNumber0 = deviceInfo0->SerialNumber;
    serialNumber1 = deviceInfo1->SerialNumber;

    if (serialNumber0 && serialNumber1) {

        //
        // Get the length of the base-device Serial Number.
        //
        length = strlen((const char*)serialNumber0);

        //
        // If the lengths match, compare the contents.
        //
        if (length == strlen((const char*)serialNumber1)) {

            if (RtlEqualMemory(serialNumber0, serialNumber1, length)) {
                match = TRUE;
            }
        }

    } else {

        TracePrint((TRACE_LEVEL_ERROR,
                    TRACE_FLAG_PNP,
                    "DsmCompareDevices (DevInfo %p): Serialnumber not assigned for %p and\\or %p.\n",
                    DsmId1,
                    deviceInfo0,
                    deviceInfo1));
    }


    TracePrint((TRACE_LEVEL_VERBOSE,
                TRACE_FLAG_PNP,
                "DsmCompareDevices (DevInfo %p): Exiting function with match = %!bool!.\n",
                DsmId1,
                match));

    return match;
}


NTSTATUS
DsmGetControllerInfo(
    __in IN PVOID DsmContext,
    __in IN PVOID DsmId,
    __in IN ULONG Flags,
    __inout IN OUT PCONTROLLER_INFO *ControllerInfo
    )
/*++

Routine Description:

    This routine is used to get information about the controller that
    the device corresponding to DsmId in on. Currently this DSM controls
    hardware that doesn't expose controllers directly. Therefore State
    is always NO_CNTRL. This information is used mainly by whatever
    WMI admin utilities want it.

Arguments:

    DsmContext - Context value given to the multipath driver during
                 registration.

    DsmId - Value returned from DMSInquireDriver.

    Flags - Bitfield of modifiers. If ALLOCATE is not set, ControllerInfo
            will have a valid buffer for the DSM to operate on.

    ControllerInfo - Pointer  for  the DSM to place the allocated controller
                     info pertaining to DsmId

Return Value:

    STATUS_INSUFFICIENT_RESOURCES if memory allocation fails.

    STATUS_SUCCESS on success

--*/
{
    PDSM_DEVICE_INFO deviceInfo = DsmId;
    PDSM_CONTROLLER_LIST_ENTRY controllerEntry = deviceInfo->Controller;
    PCONTROLLER_INFO controllerInfo = NULL;
    LARGE_INTEGER time;
    ULONG controllerId = 0;
    NTSTATUS status = STATUS_SUCCESS;
    UNREFERENCED_PARAMETER(DsmContext);

    TracePrint((TRACE_LEVEL_VERBOSE,
                TRACE_FLAG_PNP,
                "DsmGetControllerInfo (DevInfo %p): Entering function.\n",
                DsmId));

    //
    // Check to see whether a controller id has already been made-up.
    //
    if (!controllerEntry) {

        //
        // Since this device is in an enclosure that doesn't have controllers,
        // e.g. JBOD, make one up.
        //
        KeQuerySystemTime(&time);

        //
        // Use only the lower 32-bits.
        //
        controllerId = time.LowPart;
    }

    //
    // Check the Flags
    //
    if (Flags & DSM_CNTRL_FLAGS_ALLOCATE) {

        //
        // This is the first call. Need to allocate the controller structure.
        //
        controllerInfo = DsmpAllocatePool(NonPagedPool,
                                          sizeof(CONTROLLER_INFO),
                                          DSM_TAG_CTRL_INFO);
        if (!controllerInfo) {

            TracePrint((TRACE_LEVEL_ERROR,
                        TRACE_FLAG_PNP,
                        "DsmGetControllerInfo (DevInfo %p): Failed to allocate memory for Controller Info\n",
                        DsmId));

            status = STATUS_INSUFFICIENT_RESOURCES;
            goto __Exit_DsmGetControllerInfo;
        }

        if (!controllerEntry) {

            //
            // Indicate that there are no specific controllers.
            //
            controllerInfo->State = DSM_CONTROLLER_NO_CNTRL;

            //
            // Set the identifier to the value generated earlier.
            // Indicate that it's Binary, not ASCII.
            //
            controllerInfo->Identifier.Type = StorageIdCodeSetBinary;
            controllerInfo->Identifier.Length = 8;

            RtlCopyMemory(controllerInfo->Identifier.SerialNumber,
                          &controllerId,
                          sizeof(controllerId));

        } else {

            //
            // If either implicit or explicit ALUA state transition is supported,
            // every controller is active. Else, if the devInfo's is in Active
            // state, the controller is obviously in the active state.
            //
            if ((deviceInfo->ALUASupport != DSM_DEVINFO_ALUA_NOT_SUPPORTED) ||
                (DsmpIsDeviceStateActive(deviceInfo->State))) {

                controllerInfo->State = DSM_CONTROLLER_ACTIVE;

            } else {

                controllerInfo->State = DSM_CONTROLLER_STANDBY;
            }

            controllerInfo->Identifier.Type = controllerEntry->IdCodeSet;
            controllerInfo->Identifier.Length = controllerEntry->IdLength;

            if (controllerInfo->Identifier.Length > 32) {

                controllerInfo->Identifier.Length = 32;
            }

            RtlCopyMemory(controllerInfo->Identifier.SerialNumber,
                          controllerEntry->Identifier,
                          controllerInfo->Identifier.Length);

            controllerInfo->DeviceObject = controllerEntry->DeviceObject;
        }

        *ControllerInfo = controllerInfo;

    } else if (Flags & DSM_CNTRL_FLAGS_CHECK_STATE) {

        //
        // Get the passed in struct.
        //
        controllerInfo = *ControllerInfo;

        //
        // If the enclosures supported by this DSM actually had controllers,
        // there would be a list of them and a search based on
        // ControllerIdentifier would be made.
        //
        controllerEntry = deviceInfo->Controller;

        if (!controllerEntry) {

            controllerInfo->State = DSM_CONTROLLER_NO_CNTRL;

        } else {

            //
            // If either implicit or explicit ALUA state transition is supported,
            // every controller is active. Else, if the devInfo's is in Active
            // state, the controller is obviously in the active state.
            //
            if ((deviceInfo->ALUASupport != DSM_DEVINFO_ALUA_NOT_SUPPORTED) ||
                (DsmpIsDeviceStateActive(deviceInfo->State))) {

                controllerInfo->State = DSM_CONTROLLER_ACTIVE;

            } else {

                controllerInfo->State = DSM_CONTROLLER_STANDBY;
            }
        }
    }

__Exit_DsmGetControllerInfo:

    TracePrint((TRACE_LEVEL_VERBOSE,
                TRACE_FLAG_PNP,
                "DsmGetControllerInfo (DevInfo %p): Exiting function with status %x.\n",
                DsmId,
                status));

    return status;
}


NTSTATUS
DsmSetDeviceInfo(
    __in IN PVOID DsmContext,
    __in IN PDEVICE_OBJECT TargetObject,
    __in IN PVOID DsmId,
    __inout IN OUT PVOID *PathId
    )
/*++

Routine Description:

    This routine associates the DsmId to the controlling MPDisk PDO,
    the targetObject for DSM-initiated requests, and to a Path
    (given by PathId).
    This routine will update the PathId in a way that better explains
    the topology to MPIO.
    Additionally, if we are in failover LB policy, failback if this
    path is preferred path.
    Also, if PR is being used, send registration down this path.

Arguments:

    DsmContext - Context value given to the multipath driver during
                 registration.
    TargetObject - The D.O. to which DSM-initiated requests should be sent.
    DsmId - Value returned from DMSInquireDriver.
    PathId - Id that represents the path. The value passed in may be used
             as is, or the DSM optionally can update it if it requires
             additional state info to be kept.

Return Value:

    INSUFFICENT_RESOURCES for no-mem conditions.
    STATUS_SUCCESS

--*/
{
    PDSM_DEVICE_INFO deviceInfo = DsmId;
    PDSM_GROUP_ENTRY group = deviceInfo->Group;
    PDSM_FAILOVER_GROUP failGroup;
    PDSM_CONTEXT dsmContext;
    PSCSI_ADDRESS scsiAddress;
    ULONG primaryPath = 0;
    ULONG optimizedPath = 0;
    ULONG pathWeight = 0;
    ULONG pathId;
    NTSTATUS status = STATUS_SUCCESS;
    WCHAR registryKeyName[256] = {0};
    BOOLEAN newFOGroup = FALSE;
    BOOLEAN registryKeyExists = FALSE;
    KIRQL irql;
    PVOID tempPathId = *PathId;
    DSM_LOAD_BALANCE_TYPE loadBalanceType;
    ULONGLONG preferredPath = (ULONGLONG)((ULONG_PTR)MAXULONG);
    UCHAR explicitlySet = FALSE;
    BOOLEAN vidpidPolicySet = FALSE;
    BOOLEAN overallPolicySet = FALSE;

    TracePrint((TRACE_LEVEL_VERBOSE,
                TRACE_FLAG_PNP,
                "DsmSetDeviceInfo (DevInfo %p): Entering function.\n",
                DsmId));

    //
    //  1. Set default LB policy.
    //  2. Query LB policy from registry and update if necessary.
    //  3. Set default value for primaryPath and optimizedPath based on device's
    //     access state
    //  4. Map deviceInfo to real LUN by saving off the target for I/O
    //  5. Build pathId from SCSI address
    //  6. Find FOG for device. If none found, build one.
    //       Add deviceInfo to FOG.
    //  7. Query registry for pathWeight, primaryPath and optimizedPath
    //       Update deviceInfo with results of query.
    //  8. Compare deviceInfo access state with persistent value (based on
    //     primaryPath and optimizedPath) and update its DesiredState.
    //

    if (!TargetObject) {

        TracePrint((TRACE_LEVEL_ERROR,
                    TRACE_FLAG_PNP,
                    "DsmSetDeviceInfo (DevInfo %p): No target object.\n",
                    deviceInfo));

        //
        // This deviceInfo will have no path or targetObject associated with it.
        // Mark it in a failed state so it won't be used to handle any requests.
        //
        deviceInfo->PreviousState = deviceInfo->State;
        deviceInfo->State = DSM_DEV_UNDETERMINED;

        goto __Exit_DsmSetDeviceInfo;
    }

    //
    // Default LB type is Round Robin.
    //
    loadBalanceType = DSM_LB_ROUND_ROBIN;

    //
    // Override the default with whatever is the overall policy that needs to be
    // applied for all LUNs controlled by MSDSM.
    //
    // Override that policy if one has been set for this device's VID/PID.
    //
    // Override that policy with whatever has been explicitly set for this particular
    // device.
    //
    // In order to perform the above, first query the policy for this particular device.
    // If it has not been explicity set, use MSDSM's overall policy or VID/PID policy.
    //
    status = DsmpQueryDeviceLBPolicyFromRegistry(deviceInfo,
                                                 group->RegistryKeyName,
                                                 &loadBalanceType,
                                                 &preferredPath,
                                                 &explicitlySet);
    if (!NT_SUCCESS(status)) {

        TracePrint((TRACE_LEVEL_ERROR,
                    TRACE_FLAG_PNP,
                    "DsmSetDeviceInfo (DevInfo %p): Failed to query LB policy from registry. Status %x.\n",
                    deviceInfo,
                    status));

        ASSERT(NT_SUCCESS(status));

        //
        // This deviceInfo will have no path or targetObject associated with it.
        // Mark it in a failed state so it won't be used to handle any requests.
        //
        deviceInfo->PreviousState = deviceInfo->State;
        deviceInfo->State = DSM_DEV_UNDETERMINED;

        goto __Exit_DsmSetDeviceInfo;
    }

    //
    // If this device's policy was not explicitly set, check to see if a policy
    // was set for this device's VID/PID and use that.
    // If VID/PID policy is not set, query the overall default policy
    // that needs to be applied to all devices controlled by this DSM.
    // If this setting hasn't been set, we'll fall back to using the default that was
    // determined based on the storage's ALUA capabilities.
    //
    if (!explicitlySet) {

        status = DsmpQueryTargetLBPolicyFromRegistry(deviceInfo,
                                                     &loadBalanceType,
                                                     &preferredPath);

        if (NT_SUCCESS(status)) {

            group->LBPolicySelection = DSM_DEFAULT_LB_POLICY_VID_PID;
            vidpidPolicySet = TRUE;

        } else if (status == STATUS_OBJECT_NAME_NOT_FOUND) {

            //
            // Since the policy hasn't been set for this VID/PID, check if
            // overall MSDSM-wide policy has been set.
            //
            status = DsmpQueryDsmLBPolicyFromRegistry(&loadBalanceType,
                                                      &preferredPath);
            if (NT_SUCCESS(status)) {

                group->LBPolicySelection = DSM_DEFAULT_LB_POLICY_DSM_WIDE;
                overallPolicySet = TRUE;

            } else {

                TracePrint((TRACE_LEVEL_ERROR,
                            TRACE_FLAG_PNP,
                            "DsmSetDeviceInfo (DevInfo %p): Failed to query Dsm overall LB policy from registry. Status %x.\n",
                            deviceInfo,
                            status));

                ASSERT(status == STATUS_OBJECT_NAME_NOT_FOUND);
                status = STATUS_SUCCESS;
            }
        } else {

            TracePrint((TRACE_LEVEL_ERROR,
                        TRACE_FLAG_PNP,
                        "DsmSetDeviceInfo (DevInfo %p): Failed to query VID/PID LB policy from registry. Status %x.\n",
                        deviceInfo,
                        status));

            ASSERT(status == STATUS_OBJECT_NAME_NOT_FOUND);
            status = STATUS_SUCCESS;
        }
    } else {

        group->LBPolicySelection = DSM_DEFAULT_LB_POLICY_LUN_EXPLICIT;
    }

    if (!explicitlySet && !vidpidPolicySet && !overallPolicySet) {

        group->LBPolicySelection = DSM_DEFAULT_LB_POLICY_ALUA_CAPABILITY;
    }

    //
    // If ALUA is enabled and the load balance policy is set to Round Robin,
    // we need to set it to Round Robin with Subset instead.
    //
    if (!DsmpIsSymmetricAccess(deviceInfo) && loadBalanceType == DSM_LB_ROUND_ROBIN) {

        loadBalanceType = DSM_LB_ROUND_ROBIN_WITH_SUBSET;
    }

    group->LoadBalanceType = loadBalanceType;
    group->PreferredPath = preferredPath;
    dsmContext = (PDSM_CONTEXT) DsmContext;

    KeAcquireSpinLock(&dsmContext->DSMContextLock, &irql);

    //
    // Save the registry key name under which Load balance policies
    // are stored. This will be used to query the LB policy later.
    //
    if (group->RegistryKeyName) {

        registryKeyExists = TRUE;

        if (!NT_SUCCESS(RtlStringCchCopyNW(registryKeyName,
                                           sizeof(registryKeyName) / sizeof(registryKeyName[0]),
                                           group->RegistryKeyName,
                                           ((sizeof(registryKeyName) / sizeof(registryKeyName[0])) - sizeof(WCHAR))))) {

            registryKeyName[(sizeof(registryKeyName) / sizeof(registryKeyName[0])) - 1] = L'\0';
        }
    }

    //
    // TargetObject is the destination for any requests created by this driver.
    // Save this for future reference.
    //
    deviceInfo->TargetObject = TargetObject;

    //
    // Set the PathId - All devices on the same PathId will
    // failover together. Currently the pathId is constructed
    // from Port Number, Bus Number, and Target Id of the device.
    //
    scsiAddress = deviceInfo->ScsiAddress;
    ASSERT(scsiAddress);

    pathId = 0x77;
    pathId <<= 8;
    pathId |= scsiAddress->PortNumber;
    pathId <<= 8;
    pathId |= scsiAddress->PathId;
    pathId <<= 8;
    pathId |= scsiAddress->TargetId;

    *PathId = ((PVOID)((ULONG_PTR)(pathId)));

    //
    // PathId indicates the path on which this device resides. Meaning
    // that when a Fail-Over occurs all device's on the same path fail
    // together. Search for a matching F.O. Group
    //
    failGroup = DsmpFindFOGroup(DsmContext, *PathId);

    //
    // If not found, create a new failover group
    //
    if (!failGroup) {

        failGroup = DsmpBuildFOGroup(DsmContext, deviceInfo, PathId);

        if (failGroup) {

            newFOGroup = TRUE;
            failGroup->MPIOPath = tempPathId;

        } else {

            TracePrint((TRACE_LEVEL_ERROR,
                        TRACE_FLAG_PNP,
                        "DsmSetDeviceInfo (DevInfo %p): Failed to build FO Group.\n",
                        DsmId));

            status = STATUS_INSUFFICIENT_RESOURCES;
        }
    }

    if (NT_SUCCESS(status)) {

        //
        // If this path is in the midst of failover processing, mark it as "good"
        // again.
        //
        failGroup->State = DSM_FG_NORMAL;

        //
        // add this deviceInfo to the f.o. group.
        //
        status = DsmpUpdateFOGroup(DsmContext, failGroup, deviceInfo);
        ASSERT(NT_SUCCESS(status));
    }

    KeReleaseSpinLock(&dsmContext->DSMContextLock, irql);

    if (NT_SUCCESS(status)) {

        if (registryKeyExists) {

            NTSTATUS queryStatus = STATUS_INVALID_PARAMETER;
            ULONGLONG pathId64;

            //
            // If the overall default policy or a target-level policy has been set and
            // this device's policy has not been explicitly set, there's no use querying
            // its individual path (desired) states.
            //
            if ((!overallPolicySet && !vidpidPolicySet) || (explicitlySet)) {

                //
                // Created a new failover group. Query the LB policy
                // for this device from registry.
                //
                pathId64 = (ULONGLONG)((ULONG_PTR)*PathId);

                queryStatus = DsmpQueryLBPolicyForDevice(registryKeyName,
                                                         pathId64,
                                                         &primaryPath,
                                                         &optimizedPath,
                                                         &pathWeight);
            }

            KeAcquireSpinLock(&dsmContext->DSMContextLock, &irql);

            if (NT_SUCCESS(queryStatus)) {

                deviceInfo->PathWeight = pathWeight;

                //
                // If device doesn't support ALUA, update the device state
                // based on the primary path info in the registry.
                //
                if (DsmpIsSymmetricAccess(deviceInfo)) {

                    if (primaryPath) {

                        deviceInfo->DesiredState = DSM_DEV_ACTIVE_OPTIMIZED;

                    } else {

                        deviceInfo->DesiredState = DSM_DEV_STANDBY;
                    }

                } else {

                    DSM_DEVICE_STATE devState;

                    if (primaryPath) {

                        devState = optimizedPath ? DSM_DEV_ACTIVE_OPTIMIZED : DSM_DEV_ACTIVE_UNOPTIMIZED;

                    } else {

                        devState = optimizedPath ? DSM_DEV_STANDBY : DSM_DEV_UNAVAILABLE;
                    }

                    //
                    // For ALUA, desired state makes sense for FOO.
                    // For RRWS, we assume desired state was explicitly selected
                    // by Admin if the ALUA state is different from the path
                    // state. Only under such cases would the path state have
                    // been saved in registry.
                    // In all other policies, state must just match the TPG state.
                    //
                    if (group->LoadBalanceType == DSM_LB_FAILOVER ||
                        group->LoadBalanceType == DSM_LB_ROUND_ROBIN_WITH_SUBSET) {

                        deviceInfo->DesiredState = devState;

                    } else {

                        deviceInfo->DesiredState = DSM_DEV_UNDETERMINED;
                    }
                }
            } else if (queryStatus == STATUS_OBJECT_NAME_NOT_FOUND) {

                deviceInfo->PathWeight = pathWeight;
                deviceInfo->DesiredState = DSM_DEV_UNDETERMINED;

            } else {

                deviceInfo->PathWeight = 0;
                deviceInfo->DesiredState = DSM_DEV_UNDETERMINED;
            }

            KeReleaseSpinLock(&dsmContext->DSMContextLock, irql);
        }
    }

    TracePrint((TRACE_LEVEL_INFORMATION,
                TRACE_FLAG_PNP,
                "DsmSetDeviceInfo (DevInfo %p): PathWeight %x, DesiredState %x, State %x, PrevState %x.\n",
                deviceInfo,
                deviceInfo->PathWeight,
                deviceInfo->DesiredState,
                deviceInfo->State,
                deviceInfo->PreviousState));

    if (NT_SUCCESS(status)) {

        deviceInfo->Initialized = TRUE;

    } else if (!NT_SUCCESS(status) && newFOGroup) {

        //
        // This deviceInfo will have no path associated with it.
        // Mark it in a failed state so it won't be used to handle any requests.
        //
        deviceInfo->PreviousState = deviceInfo->State;
        deviceInfo->State = DSM_DEV_UNDETERMINED;

        TracePrint((TRACE_LEVEL_ERROR,
                    TRACE_FLAG_PNP,
                    "DsmSetDeviceInfo (DevInfo %p): No path associated with instance. Changing state from %u to %u.\n",
                    deviceInfo,
                    deviceInfo->PreviousState,
                    deviceInfo->State));

        DsmpRemoveDeviceFailGroup(DsmContext, failGroup, deviceInfo, FALSE);

        if (failGroup->Count == 0) {

            //
            // Yank it from the list.
            //
            RemoveEntryList(&failGroup->ListEntry);
            InterlockedDecrement((LONG volatile*)&dsmContext->NumberFOGroups);

            TracePrint((TRACE_LEVEL_INFORMATION,
                        TRACE_FLAG_PNP,
                        "DsmSetDeviceInfo (DevInfo %p): Removing FOGroup %p with path %p. Count of FOGroups %d.\n",
                        DsmId,
                        failGroup,
                        failGroup->PathId,
                        dsmContext->NumberFOGroups));

            //
            // Free the allocation.
            //
            DsmpFreePool(failGroup);
        }
    }

__Exit_DsmSetDeviceInfo:

    TracePrint((TRACE_LEVEL_VERBOSE,
                TRACE_FLAG_PNP,
                "DsmSetDeviceInfo (DevInfo %p): Exiting function with status %x.\n",
                DsmId,
                status));

    return status;
}


BOOLEAN
DsmIsPathActive(
    __in IN PVOID DsmContext,
    __in IN PVOID PathId,
    __in IN PVOID DsmId
    )
/*++

Routine Description:

    This routine is used to determine whether the path to DsmId is usable
    (ie. able to handle requests without a failover).

    Also, after a failover, the path validity will be queried.
    If the path error was transitory and the DSM feels that the path is good,
    then this request will be re-issued to determine whether it is usable.

Arguments:

    DsmContext - Context value given to the multipath driver during
                 registration.
    PathId - Value set in SetPathId.
    DsmId - DSM Id returned during DsmInquire.

Return Value:

    TRUE if the path is active. FALSE otherwise.
--*/
{
    PDSM_FAILOVER_GROUP foGroup;
    PDSM_DEVICE_INFO deviceInfo = DsmId;
    PDSM_GROUP_ENTRY group = deviceInfo->Group;
    PDSM_CONTEXT dsmContext = (PDSM_CONTEXT) DsmContext;
    KIRQL irql;
    BOOLEAN retVal;

    //
    //  1. If PR and reserved by this node, register the PR keys.
    //  2. Find the FOG for the passed in PathId
    //  3. Depending on the LB policy, set the appropriate devInfo states
    //        If FailOver, and DesiredState is AO, change the active
    //        devInfos to non-active state and make this one AO.
    //           If ALUA supported, send down SetTPG to make this change,
    //               else directly make the change.
    //        If RR/LWP/LQD, make this DevInfo ActiveOptimized.
    //        If RRS, and DesiredState is AO, change the active devInfos to
    //        their desired states and then make this one AO.
    //           If DesiredState is not AO, find a devInfo in AO state. If
    //           one is found, make this devInfo's state its desired state,
    //           else if one isn't found, make this one AO.
    //  3. If this is preferredPath, and LB policy is failover-only, change the
    //     access state of deviceInfo to AO.
    //       If there is another devInfo currently in AO, change its state too.
    //       If ALUA supported, send down SetTPG to make these changes.
    //  4. Get the appropriate AO DeviceInfo and mark the group's PTBU to its
    //     pathId.
    //

    TracePrint((TRACE_LEVEL_VERBOSE,
                TRACE_FLAG_PNP,
                "DsmIsPathActive (DevInfo %p): Entering function.\n",
                DsmId));

    //
    // Initialize this instance to be usable so that during the possible processing
    // of PR register, this device can be a candidate for certain kind of requests.
    //
    deviceInfo->Usable = TRUE;

    //
    // New path arriving. If this Node owns the reservation register this path.
    //
    if (group->PRKeyValid) {

        NTSTATUS prRegStatus;
        ULONG i;
        PDSM_DEVICE_INFO devInfo;
        ULONG ordinal;

        prRegStatus = DsmpRegisterPersistentReservationKeys(deviceInfo, TRUE);
        deviceInfo->IsReserved = TRUE;

        if (NT_SUCCESS(prRegStatus)) {

            deviceInfo->PRKeyRegistered = TRUE;

        } else {

            TracePrint((TRACE_LEVEL_WARNING,
                        TRACE_FLAG_PNP,
                        "DsmIsPathActive (DevInfo %p): Failed (status %x) to register PR key\n",
                        deviceInfo,
                        prRegStatus));
        }

        for (i = 0; i < group->NumberDevices; i++) {

            devInfo = group->DeviceList[i];
            if (devInfo && devInfo == deviceInfo) {

                ordinal = (1 << i);
                group->ReservationList |= ordinal;
                break;
            }
        }
    }

    KeAcquireSpinLock(&dsmContext->DSMContextLock, &irql);

    //
    // Get the F.O. Group information.
    //
    foGroup = DsmpFindFOGroup(DsmContext, PathId);

    //
    // If there are any devices on this path, and it's not in a failed state
    // it's capable of handling requests. So it's active.
    //
    if ((foGroup) &&
        (foGroup->Count) &&
        (foGroup->State == DSM_FG_NORMAL)) {

        TracePrint((TRACE_LEVEL_INFORMATION,
                    TRACE_FLAG_PNP,
                    "DsmIsPathActive (DevInfo %p): Path %p is usable.\n",
                    DsmId,
                    PathId));

        retVal = TRUE;

        //
        // Update the next path to be used for the group if it not set already.
        //
        deviceInfo = (PDSM_DEVICE_INFO)DsmId;

        group = deviceInfo->Group;
        DSM_ASSERT(group != NULL);
        DSM_ASSERT(group->GroupSig == DSM_GROUP_SIG);

        //
        // If an invalidated path came back online before PnP removes came in,
        // then MPIO's path recovery thread would have sent down a PathVerify
        // just moments before by which we changed the state of the FOG to
        // normal. Now it is time to change the deviceInfo's state to a "good"
        // state.
        //
        if (deviceInfo->State >= DSM_DEV_FAILED) {

            DSM_ASSERT(deviceInfo->State == DSM_DEV_INVALIDATED);

            if (DsmpIsSymmetricAccess(deviceInfo)) {

                //
                // Mark it as AO. The SetLBForPathArrival will update the state
                // appropriately.
                //
                deviceInfo->State = DSM_DEV_ACTIVE_OPTIMIZED;

            } else {

                //
                // Set it to the state that was reported during the last RTPG
                // call that was made.
                //
                deviceInfo->State = deviceInfo->ALUAState;
            }
        }

        if (DsmpIsSymmetricAccess(deviceInfo)) {

            DsmpSetLBForPathArrival(DsmContext, deviceInfo);

        } else {

            KeReleaseSpinLock(&dsmContext->DSMContextLock, irql);
            DsmpSetLBForPathArrivalALUA(DsmContext, deviceInfo);
            KeAcquireSpinLock(&dsmContext->DSMContextLock, &irql);
        }

        TracePrint((TRACE_LEVEL_INFORMATION,
                    TRACE_FLAG_PNP,
                    "DsmIsPathActive (DevInfo %p): State set to %d\n",
                    deviceInfo,
                    deviceInfo->State));

        ASSERT(group->PathToBeUsed);

        if (group->PathToBeUsed == NULL) {

            TracePrint((TRACE_LEVEL_INFORMATION,
                        TRACE_FLAG_PNP,
                        "DsmIsPathActive (DevInfo %p): Will set PathToBeUsed for %p\n",
                        deviceInfo,
                        group));

            deviceInfo = DsmpGetActivePathToBeUsed(group);
            if (deviceInfo != NULL) {

                TracePrint((TRACE_LEVEL_INFORMATION,
                            TRACE_FLAG_PNP,
                            "DsmIsPathActive (DevInfo %p): FOG %p set for PathToBeUsed for %p\n",
                            deviceInfo,
                            deviceInfo->FailGroup,
                            group));

                group->PathToBeUsed = deviceInfo->FailGroup;

            } else {

                //
                // In the case of implicit transitions, it is possible that a TPG hasn't yet
                // been made A/O. So instead of not sending down the command, fall back to using
                // some other path.
                // The argument holds true for storages that support implict as well as explicit
                // transitions, where an implicit transition by the storage may have overridden
                // the TPG state that we had explicitly made A/O, and it is possible that we aren't
                // left with any path in A/O state.
                //
                if (!DsmpIsSymmetricAccess((PDSM_DEVICE_INFO)DsmId) &&
                    ((PDSM_DEVICE_INFO)DsmId)->ALUASupport != DSM_DEVINFO_ALUA_EXPLICIT) {

                    BOOLEAN sendTPG = FALSE;

                    deviceInfo = DsmpFindStandbyPathToActivateALUA(group, &sendTPG);

                    if (!deviceInfo) {

                        TracePrint((TRACE_LEVEL_WARNING,
                                    TRACE_FLAG_PNP,
                                    "DsmIsPathActive (DevInfo %p): No FOG available for group %p\n",
                                    DsmId,
                                    group));

                        group->PathToBeUsed = NULL;

                    } else {

                        TracePrint((TRACE_LEVEL_WARNING,
                                    TRACE_FLAG_PNP,
                                    "DsmpIsPathActive (DevInfo %p): Using best alternative candidate %p.\n",
                                    DsmId,
                                    deviceInfo));
                    }
                } else {

                    ASSERT(deviceInfo);

                    TracePrint((TRACE_LEVEL_WARNING,
                                TRACE_FLAG_PNP,
                                "DsmIsPathActive (DevInfo %p): No active/alternative path available for group %p\n",
                                DsmId,
                                group));

                    group->PathToBeUsed = NULL;
                }
            }
        }
    } else {

        TracePrint((TRACE_LEVEL_ERROR,
                    TRACE_FLAG_PNP,
                    "DsmIsPathActive (DevInfo %p): Path %p is NOT usable.\n",
                    DsmId,
                    PathId));

        retVal = FALSE;
    }

    ((PDSM_DEVICE_INFO)DsmId)->Usable = retVal;

    KeReleaseSpinLock(&dsmContext->DSMContextLock, irql);

    TracePrint((TRACE_LEVEL_VERBOSE,
                TRACE_FLAG_PNP,
                "DsmIsPathActive (DevInfo %p): Exiting function with retVal = %!bool!.\n",
                DsmId,
                retVal));

    return retVal;
}


NTSTATUS
DsmPathVerify(
    __in IN PVOID DsmContext,
    __in IN PVOID DsmId,
    __in IN PVOID PathId
    )
/*++

Routine Description:

    This routine ensures that the path to the device indicated by DsmId
    is healthy. It's called periodically by the bus driver, and also
    after a fail-over condition has been dealt with to ensure that
    the path is able to handle requests.

Arguments:

    DsmContext - Context value given to the multipath driver during
                 registration.
    DsmId - Value returned from DMSInquire.
    PathId - Value set in SetPathId.

Return Value:

    NTSTATUS
--*/

{
    PDSM_CONTEXT dsmCtxt = (PDSM_CONTEXT) DsmContext;
    PDSM_DEVICE_INFO deviceInfo = DsmId;
    PDSM_FAILOVER_GROUP foGroup;
    NTSTATUS status = STATUS_UNSUCCESSFUL;
    BOOLEAN found = FALSE;
    KIRQL irql;
    PLIST_ENTRY entry;
    PDSM_FOG_DEVICELIST_ENTRY fogDeviceListEntry = NULL;
    PDSM_GROUP_ENTRY group = deviceInfo->Group;

    TracePrint((TRACE_LEVEL_VERBOSE,
                TRACE_FLAG_PNP,
                "DsmPathVerify (DevInfo %p): Entering function.\n",
                DsmId));

    if (DsmpIsDeviceInitialized(deviceInfo)) {

        KeAcquireSpinLock(&dsmCtxt->DSMContextLock, &irql);

        //
        // Get the failover group
        //
        foGroup = DsmpFindFOGroup(DsmContext, PathId);

        if (foGroup) {

            //
            // Find the device.
            //
            for (entry = foGroup->FOG_DeviceList.Flink;
                 entry != &foGroup->FOG_DeviceList;
                 entry = entry->Flink) {

                fogDeviceListEntry = CONTAINING_RECORD(entry, DSM_FOG_DEVICELIST_ENTRY, ListEntry);

                if (fogDeviceListEntry && fogDeviceListEntry->DeviceInfo == deviceInfo) {

                    status = STATUS_SUCCESS;
                    found = TRUE;

                    break;
                }
            }
        } else {

            //
            // This is not a good thing. It indicates that either we
            // returned a bogus path to the bus-driver on a fail-over,
            // or that the path evaporated between polls and PnP hasn't
            // torn stuff down.
            //
            TracePrint((TRACE_LEVEL_ERROR,
                        TRACE_FLAG_PNP,
                        "DsmPathVerify (DevInfo %p): Failed to find failover group for path %p.\n",
                        DsmId,
                        PathId));

            status = STATUS_DEVICE_NOT_CONNECTED;
        }

        KeReleaseSpinLock(&dsmCtxt->DSMContextLock, irql);

        if (NT_SUCCESS(status)) {

            if (found) {

                //
                // Send down TUR if ALUA is not supported.
                // Else, send down ReportTargetPortGroups (sending TUR down non-A/O path will
                // always result in a check condition).
                //
                if (deviceInfo->ALUASupport == DSM_DEVINFO_ALUA_NOT_SUPPORTED) {

                    TracePrint((TRACE_LEVEL_INFORMATION,
                                TRACE_FLAG_PNP,
                                "DsmPathVerify (DevInfo %p): Sending TUR using %p to verify path %p.\n",
                                DsmId,
                                deviceInfo,
                                deviceInfo->FailGroup->PathId));

                    status = DsmSendTUR(deviceInfo->TargetObject);

                } else {

                    //
                    // Check for whether we should ignore sending down an RTPG:
                    // Flag set indicates that this PathVerify() is happening in response to device
                    // arrival and can be skipped since Inquire() has just already sent down an RTPG.
                    // All that needs to be done is to clear the flag so that subsequent PathVerify()
                    // sent in response to InitiateFO will send RTPG as a ping.
                    // This is an optimization with the idea of helping speed up boot time, which is
                    // is adversely impacted, especially if there are many LUNs, each with many paths.
                    //
                    if (deviceInfo->IgnorePathVerify) {

                        deviceInfo->IgnorePathVerify = FALSE;

                        TracePrint((TRACE_LEVEL_INFORMATION,
                                    TRACE_FLAG_PNP,
                                    "DsmPathVerify (DevInfo %p): Returning success immediately since RPTG was already just sent.\n",
                                    DsmId));

                        status = STATUS_SUCCESS;

                    } else {

                        TracePrint((TRACE_LEVEL_INFORMATION,
                                    TRACE_FLAG_PNP,
                                    "DsmPathVerify (DevInfo %p): Sending RTPG using %p to verify path %p.\n",
                                    DsmId,
                                    deviceInfo,
                                    deviceInfo->FailGroup->PathId));

                        status = DsmpGetDeviceALUAState(dsmCtxt, deviceInfo, NULL);

                        //
                        // Since this RTPG may have resulted in us losing a UA, adjust
                        // the states if needed.
                        //
                        if (NT_SUCCESS(status)) {

                            DsmpAdjustDeviceStatesALUA(group, NULL);
                        }
                    }
                }
            }

            if (NT_SUCCESS(status)) {

                if (deviceInfo->State >= DSM_DEV_FAILED) {

                    foGroup->State = DSM_FG_NORMAL;
                    deviceInfo->State = deviceInfo->LastKnownGoodState;
                }
            }
        }
    }

    TracePrint((TRACE_LEVEL_VERBOSE,
                TRACE_FLAG_PNP,
                "DsmPathVerify (DevInfo %p): Exiting function with status %x.\n",
                DsmId,
                status));

    return status;
}


NTSTATUS
DsmInvalidatePath(
    __in IN PVOID DsmContext,
    __in IN ULONG ErrorMask,
    __in IN PVOID PathId,
    __inout IN OUT PVOID *NewPathId
    )
/*++

Routine Description:

    This routine will mark up devices as failed on PathId, and find
    an appropriate path to return to MPIO.

Arguments:

    DsmContext - Context value given to the multipath driver during
                 registration.
    ErrorMask - Value returned from InterpretError.
    PathId - The failing path.
    NewPathId - Pointer to the new path.

Return Value:

    NTSTATUS of the operation.

--*/
{
    PDSM_CONTEXT context = DsmContext;
    PDSM_FAILOVER_GROUP failGroup;
    PDSM_FAILOVER_GROUP newPath = NULL;
    PDSM_FAILOVER_GROUP pathId;
    PDSM_DEVICE_INFO deviceInfo;
    LIST_ENTRY reservedDeviceList;
    NTSTATUS status = STATUS_SUCCESS;
    KIRQL irql;
    PLIST_ENTRY entry;
    PDSM_FOG_DEVICELIST_ENTRY fogDeviceListEntry = NULL;
    BOOLEAN lockHeld = FALSE;

    UNREFERENCED_PARAMETER(ErrorMask);

    TracePrint((TRACE_LEVEL_VERBOSE,
                TRACE_FLAG_RW,
                "DsmInvalidatePath (PathId %p): Entering function.\n",
                PathId));

    DSM_ASSERT(ErrorMask & DSM_FATAL_ERROR);

    *NewPathId = NULL;

    InitializeListHead(&reservedDeviceList);

    KeAcquireSpinLock(&context->DSMContextLock, &irql);
    lockHeld = TRUE;

    //
    // Get the fail-over group corresponding to the PathId.
    //
    failGroup = DsmpFindFOGroup(DsmContext, PathId);

    if (!failGroup || failGroup->State == DSM_FG_FAILED) {

        TracePrint((TRACE_LEVEL_ERROR,
                    TRACE_FLAG_RW,
                    "DsmInvalidatePath (PathId %p): Failed to find FailOver group.\n",
                    PathId));

        status = STATUS_NO_SUCH_DEVICE;
        goto __Exit_DsmInvalidatePath;
    }

    TracePrint((TRACE_LEVEL_INFORMATION,
                TRACE_FLAG_RW,
                "DsmInvalidatePath (PathId %p): Context %p, FOG %p failing.\n",
                PathId,
                DsmContext,
                failGroup));

    //
    // Mark the path as failed.
    //
    failGroup->State = DSM_FG_FAILED;

    //
    // Check to see whether the port driver and PnP removed the devices
    // BEFORE the fail-over indication actually occurred. Work-around
    // of several Fibre miniports.
    //
    if (failGroup->Count == 0) {

        //
        // This indicates that all of the devices have already been removed.
        // If there were reservations outstanding, the RemoveDevice code
        // should have updated them.
        //
        TracePrint((TRACE_LEVEL_ERROR,
                    TRACE_FLAG_RW,
                    "DsmInvalidatePath (PathId %p): Failed to find FailOver group.\n",
                    PathId));

        status = STATUS_NO_SUCH_DEVICE;
        goto __Exit_DsmInvalidatePath;
    }

    //
    // TODO: jaivira: Ideally speaking we should update (ie. decrement) the old (in InterpretError)
    // and new failgroup's inflight IO stats (ie. increment here in InvalidatePath). Also, loop
    // through for each devInfo on the failed path, using that devInfo's group and the new path,
    // find the new devInfo and update both devInfos' inflight IO stats. However, this will impact
    // perf, so revisit to determine if there is a better way to do this. The only adverse effect
    // of not doing this is that for a couple of IO, it is possible that the path chosen, in LQD
    // policy, is not the one with the least number of outstanding IO.
    //

    //
    // Process each device in the fail-over group
    //
    for (entry = failGroup->FOG_DeviceList.Flink;
         entry != &failGroup->FOG_DeviceList;
         entry = entry->Flink) {

        fogDeviceListEntry = CONTAINING_RECORD(entry, DSM_FOG_DEVICELIST_ENTRY, ListEntry);

        if (!fogDeviceListEntry) {
            continue;
        }

        //
        // Get the deviceInfo.
        //
        deviceInfo = fogDeviceListEntry->DeviceInfo;

        if (!(DsmpIsDeviceFailedState(deviceInfo->State))) {

            deviceInfo->LastKnownGoodState = deviceInfo->State;
        }

        //
        // Set the state of the Failing Device
        //
        deviceInfo->PreviousState = deviceInfo->State;
        deviceInfo->State = DSM_DEV_INVALIDATED;

        InterlockedIncrement(&deviceInfo->BlockRemove);

        KeReleaseSpinLock(&context->DSMContextLock, irql);
        lockHeld = FALSE;

        pathId = DsmpSetNewPath(DsmContext, deviceInfo);

        if (!newPath) {
            newPath = pathId; // Save off first good alternative path that we find
        }

        if (!lockHeld) {
            KeAcquireSpinLock(&context->DSMContextLock, &irql);
            lockHeld = TRUE;
        }

        InterlockedDecrement(&deviceInfo->BlockRemove);
    }

    if (!newPath) {

        //
        // This indicates that no acceptable paths
        // were found. Return the error to mpctl.
        //

        TracePrint((TRACE_LEVEL_ERROR,
                    TRACE_FLAG_RW,
                    "DsmInvalidatePath (PathId %p): No valid path found.\n",
                    PathId));

        status = STATUS_NO_SUCH_DEVICE;

    } else {

        //
        // return the new path.
        //
        *NewPathId = newPath->PathId;

        TracePrint((TRACE_LEVEL_INFORMATION,
                    TRACE_FLAG_RW,
                    "DsmInvalidatePath (PathId %p): Returning %p as newPath.\n",
                    PathId,
                    newPath->PathId));
    }

__Exit_DsmInvalidatePath:

    if (lockHeld) {
        KeReleaseSpinLock(&context->DSMContextLock, irql);
    }

    TracePrint((TRACE_LEVEL_VERBOSE,
                TRACE_FLAG_RW,
                "DsmInvalidatePath (PathId %p): Exiting function with status %x.\n",
                PathId,
                status));

    return status;
}


NTSTATUS
DsmMoveDevice(
    __in IN PVOID DsmContext,
    __in IN PDSM_IDS DsmIds,
    __in IN PVOID MPIOPath,
    __in IN PVOID SuggestedPath,
    __in IN ULONG Flags
    )
/*++

Routine Description:

    This routine is invoked in response to an administrative request.
    The device that's associated with SuggestedPath will be made active, and the
    current active device, moved to stand-by.

Arguments:

    DsmContext - Context value given to the multipath driver during registration.
    DsmIds - The collection of DSM IDs that pertain to the MPDisk.
    MPIOPath - The original path value passed to SetDeviceInfo.
    SuggestedPath - The path which should become the active path.
    Flags - Bitmask indicating the intent of the move.

Return Value:

    NTSTATUS - STATUS_SUCCESS, unless SuggestedPath is somehow invalid.
               STATUS_INVALID_PARAMETER is ADMIN is set and the path is invalid.

--*/
{
    PDSM_CONTEXT context = DsmContext;
    PDSM_DEVICE_INFO deviceInfo;
    PDSM_FAILOVER_GROUP failGroup;
    ULONG i;
    NTSTATUS status;
    KIRQL irql;
    BOOLEAN adminRequest = FALSE;
    PDSM_GROUP_ENTRY group = NULL;

    TracePrint((TRACE_LEVEL_VERBOSE,
                TRACE_FLAG_WMI,
                "DsmMoveDevice (DsmIds %p): Entering function - DsmContext %p MPIOPath (%p) SuggestedPath %p.\n",
                DsmIds,
                DsmContext,
                MPIOPath,
                SuggestedPath));

    //
    // Capture the value of the ADMIN flag bit.
    // Currently, permanent assignment of the device to "preferred path" isn't supported.
    // This driver doesn't care about the pending remove flag (currently).
    //
    adminRequest = (BOOLEAN)(Flags & DSM_MOVE_ADMIN_REQUEST);

    KeAcquireSpinLock(&context->DSMContextLock, &irql);

    group = ((PDSM_DEVICE_INFO)(DsmIds->IdList[0]))->Group;

    //
    // Find the first active device.
    //
    deviceInfo = DsmpGetActivePathToBeUsed(group);

    if (!deviceInfo) {

        //
        // Didn't find an active device. Should LOG.
        // Use the first one to piggy-back the request.
        //
        deviceInfo = DsmIds->IdList[0];
    }

    //
    // Get the fail-over group associated with the Path.
    //
    failGroup = DsmpFindFOGroup(DsmContext,
                                SuggestedPath);

    if (!failGroup) {

        //
        // The caller has made a terrible mistake.
        // If it's an ADMIN request, blow it off.
        //
        if (adminRequest) {
            status = STATUS_INVALID_PARAMETER;
        } else {

            //
            // Try to set another path.
            //
            // Note that failGroup will be NULL going into
            // SetNewPath. This is OK.
            //
            status = STATUS_SUCCESS;
        }
    } else {
        status = STATUS_SUCCESS;
    }

    if (status == STATUS_SUCCESS) {

        //
        // Set the new path, using SuggestedPath.
        //
        InterlockedIncrement(&deviceInfo->BlockRemove);
        KeReleaseSpinLock(&context->DSMContextLock, irql);
        failGroup = DsmpSetNewPath(context,
                                   deviceInfo);
        KeAcquireSpinLock(&context->DSMContextLock, &irql);
        InterlockedDecrement(&deviceInfo->BlockRemove);

        //
        // If we were able to make the suggested path active, that should be used.
        //
        for (i = 0, status = STATUS_UNSUCCESSFUL; i < DsmIds->Count && !NT_SUCCESS(status); i++) {

            deviceInfo = DsmIds->IdList[i];

            if (deviceInfo->FailGroup == failGroup) {

                if (deviceInfo->State == DSM_DEV_ACTIVE_OPTIMIZED) {

                    group->PathToBeUsed = failGroup;
                    status = STATUS_SUCCESS;
                }
            }
        }
    }

    KeReleaseSpinLock(&context->DSMContextLock, irql);

    TracePrint((TRACE_LEVEL_VERBOSE,
                TRACE_FLAG_WMI,
                "DsmMoveDevice (DsmIds %p): Exiting function with status %x.\n",
                DsmIds,
                status));

    return status;
}


NTSTATUS
DsmRemovePending(
    __in IN PVOID DsmContext,
    __in IN PVOID DsmId
    )
/*++

Routine Description:

    This routine indicates that the device represented by DsmId will be
    removed, so the deviceInfo is marked up to indicate the pending removal,
    so that it won't be used.

Arguments:

    DsmContext - Context value given to the multipath driver
                 during registration.
    DsmId - Value referring to the failed device.

Return Value:

    STATUS_SUCCESS

--*/

{
    PDSM_CONTEXT dsmContext = DsmContext;
    PDSM_DEVICE_INFO deviceInfo = DsmId;
    KIRQL irql;

    TracePrint((TRACE_LEVEL_VERBOSE,
                TRACE_FLAG_PNP,
                "DsmRemovePending (DevInfo %p): Entering function.\n",
                DsmId));

    //
    // DsmpSetNewPath then finds the next available device. This is basically a
    // fail-over for just this device.
    //
    InterlockedIncrement(&deviceInfo->BlockRemove);
    DsmpSetNewPath(DsmContext, deviceInfo);
    KeAcquireSpinLock(&dsmContext->DSMContextLock, &irql);
    InterlockedDecrement(&deviceInfo->BlockRemove);

    if (!(DsmpIsDeviceFailedState(deviceInfo->State))) {

        deviceInfo->LastKnownGoodState = deviceInfo->State;
    }

    //
    // Mark the device as being unavailable since remove will be sent shortly.
    //
    deviceInfo->PreviousState = deviceInfo->State;
    deviceInfo->State = DSM_DEV_REMOVE_PENDING;

    KeReleaseSpinLock(&dsmContext->DSMContextLock, irql);

    TracePrint((TRACE_LEVEL_VERBOSE,
                TRACE_FLAG_PNP,
                "DsmRemovePending (DevInfo %p): Exiting function.\n",
                DsmId));

    return STATUS_SUCCESS;
}

NTSTATUS
DsmRemoveDevice(
    __in IN PVOID DsmContext,
    __in IN PVOID DsmId,
    __in IN PVOID PathId
    )
/*++

Routine Description:

    The device is gone and the port pdo has been removed. This routine will
    update the internal structures and free any allocations.

Arguments:

    DsmContext - Context value given to the multipath driver during
                 registration.
    DsmId - Value referring to the failed device.
    PathId - The path on which the Device lives.

Return Value:

    STATUS_SUCCESS

--*/

{
    PDSM_CONTEXT dsmContext = DsmContext;
    PDSM_DEVICE_INFO deviceInfo = DsmId;
    KIRQL irql;
    PDSM_FAILOVER_GROUP failGroup = deviceInfo->FailGroup;
    PDSM_GROUP_ENTRY group = deviceInfo->Group;
    LONG block;

    UNREFERENCED_PARAMETER(PathId);

    TracePrint((TRACE_LEVEL_VERBOSE,
                TRACE_FLAG_PNP,
                "DsmRemoveDevice (DevInfo %p): Entering function.\n",
                DsmId));

    do {

        KeAcquireSpinLock(&dsmContext->DSMContextLock, &irql);
        block = deviceInfo->BlockRemove;
        ASSERT(block >= 0);

        if (block) {

            KeReleaseSpinLock(&dsmContext->DSMContextLock, irql);
            KeStallExecutionProcessor(10000);
        }

    } while (block);

    if (!(DsmpIsDeviceFailedState(deviceInfo->State))) {

        deviceInfo->LastKnownGoodState = deviceInfo->State;
    }

    deviceInfo->PreviousState = deviceInfo->State;
    deviceInfo->State = DSM_DEV_REMOVED;

    //
    // Decrement the reference count for this device's controller entry and
    // delete the entry if its reference count is now zero.
    //
    if (deviceInfo->Controller) {

        if (InterlockedDecrement((LONG volatile*)&(deviceInfo->Controller->RefCount)) == 0) {

            RemoveEntryList(&(deviceInfo->Controller->ListEntry));
            DsmpFreeControllerEntry(dsmContext, deviceInfo->Controller);
            deviceInfo->Controller = NULL;
            InterlockedDecrement((LONG volatile*)&(dsmContext->NumberControllers));
        }
    }

    //
    // Ensure that the device has been fully initialized before trying to
    // remove it from the FOG. If SetDeviceInfo has yet to be invoked, there
    // will yet to be an association set.
    //
    if (failGroup) {

        //
        // Remove its entry from the Fail-Over Group.
        //
        DsmpRemoveDeviceFailGroup(DsmContext, failGroup, deviceInfo, FALSE);
    }

    KeReleaseSpinLock(&dsmContext->DSMContextLock, irql);

    //
    // Remove it from it's multi-path group. This has the side-effect
    // of cleaning up the Group if the number of devices goes to zero.
    //
    DsmpRemoveDeviceEntry(DsmContext, group, deviceInfo);

    TracePrint((TRACE_LEVEL_VERBOSE,
                TRACE_FLAG_PNP,
                "DsmRemoveDevice (DevInfo %p): Exiting function.\n",
                DsmId));

    return STATUS_SUCCESS;
}


NTSTATUS
DsmRemovePath(
    __in IN PDSM_CONTEXT DsmContext,
    __in IN PVOID PathId
    )
/*++

Routine Description:

    This routine indicates that the path is no longer valid, and that it should
    be removed. Internal counts will be updated and any allocations associated
    with this path freed.

Arguments:

    DsmContext - Context value given to the multipath driver during registration.
    PathId - The path to remove.

Return Value:

    NTSTATUS of the operation.

--*/

{
    PDSM_FAILOVER_GROUP failGroup;
    KIRQL irql;

    TracePrint((TRACE_LEVEL_VERBOSE,
                TRACE_FLAG_PNP,
                "DsmRemovePath (PathId %p): Entering function.\n",
                PathId));

    KeAcquireSpinLock(&DsmContext->DSMContextLock, &irql);

    failGroup = DsmpFindFOGroup(DsmContext, PathId);

    if (failGroup) {

        //
        // The claim is that a path won't be removed, until all
        // the devices on it are.
        //
        if (failGroup->Count == 0) {

            //
            // Yank it from the list.
            //
            RemoveEntryList(&failGroup->ListEntry);
            InterlockedDecrement((LONG volatile*)&DsmContext->NumberFOGroups);

            //
            // Move this over to the stale FOG list if there are inflight requests.
            // Otherwise free the allocation.
            //
            if (InterlockedCompareExchange((LONG volatile*)&failGroup->NumberOfRequestsInFlight, 0, 0) > 0) {

                failGroup->State = DSM_FG_PENDING_REMOVE;
                InsertTailList(&DsmContext->StaleFailGroupList, &failGroup->ListEntry);
                InterlockedIncrement((LONG volatile*)&DsmContext->NumberStaleFOGroups);

                TracePrint((TRACE_LEVEL_INFORMATION,
                            TRACE_FLAG_PNP,
                            "DsmRemovePath (PathId %p): Outstanding requests %d. Moving FOGroup %p with path %p to stale path list.\n",
                            PathId,
                            failGroup->NumberOfRequestsInFlight,
                            failGroup,
                            failGroup->PathId));
            } else {

                TracePrint((TRACE_LEVEL_INFORMATION,
                            TRACE_FLAG_PNP,
                            "DsmRemovePath (PathId %p): Removing FOGroup %p with path %p. Count of FOGroups %d.\n",
                            PathId,
                            failGroup,
                            failGroup->PathId,
                            DsmContext->NumberFOGroups));

                //
                // Free the allocation.
                //
                DsmpFreePool(failGroup);
            }
        } else {

            TracePrint((TRACE_LEVEL_ERROR,
                        TRACE_FLAG_PNP,
                        "DsmRemovePath (PathId %p): Count %d. Not removing FOGroup %p.\n",
                        PathId,
                        failGroup->Count,
                        failGroup));

            //
            // Should never be here.
            //
            ASSERT(failGroup->Count == 0);
        }
    } else {

        //
        // It's already been removed.
        //
        TracePrint((TRACE_LEVEL_ERROR,
                    TRACE_FLAG_PNP,
                    "DsmRemovePath (PathId %p): Did not find the FO group.\n",
                    PathId));

        ASSERT(failGroup);
    }

    KeReleaseSpinLock(&DsmContext->DSMContextLock, irql);

    TracePrint((TRACE_LEVEL_VERBOSE,
                TRACE_FLAG_PNP,
                "DsmRemovePath (PathId %p): Exiting function.\n",
                PathId));

    return STATUS_SUCCESS;
}


PVOID
DsmLBGetPath(
    __in IN PVOID DsmContext,
    __in IN PSCSI_REQUEST_BLOCK Srb,
    __in IN PDSM_IDS DsmList,
    __in IN PVOID CurrentPath,
    __out OUT NTSTATUS *Status
    )
/*++

Routine Description:

    This routine is used by mpio to handle load-balancing.

Arguments:

    DsmContext - Context value given to the multipath driver during
                 registration.
    Srb - The current read/write Srb.
    DsmList - List of our DSM IDs.
    CurrentPath - The last path that was returned for this multi-path group.
    Status - Storage to place NTSTATUS of the call.

Return Value:

    The path ID to which the request should be sent.

--*/

{
    PDSM_CONTEXT dsmContext = DsmContext;
    PDSM_DEVICE_INFO deviceInfo;
    PDSM_GROUP_ENTRY group;
    PDSM_FAILOVER_GROUP failGroup = NULL;
    PVOID newPath = NULL;
    KIRQL irql;
    PDSM_FAILOVER_GROUP oldFailGroup = NULL;
    PDSM_FAIL_PATH_PROCESSING_LIST_ENTRY failPathDevInfoEntry = NULL;

    TracePrint((TRACE_LEVEL_VERBOSE,
                TRACE_FLAG_RW,
                "DsmLBGetPath (DsmIds %p): Entering function.\n",
                DsmList));

    //
    // Up-front checking to minimally validate the list of
    // DsmId's being passed in.
    //
    ASSERT(DsmList->Count && DsmList->IdList[0]);
    if (!(DsmList->Count && DsmList->IdList[0])) {

        *Status = STATUS_NO_SUCH_DEVICE;
        goto __Exit_DsmLBGetPath;
    }

    KeAcquireSpinLock(&dsmContext->DSMContextLock, &irql);

    deviceInfo = DsmList->IdList[0];
    group = deviceInfo->Group;

    failGroup = DsmpGetPath(dsmContext, DsmList, Srb);

    //
    // If there wasn't a single active/optimized path found, check to see if
    // there is an STPG in progress that may be making a path A/O.
    //
    if (!failGroup) {

        //
        // Take the last path used.
        //
        oldFailGroup = DsmpFindFOGroup(dsmContext, CurrentPath);

        //
        // Find the devInfo corresponding to this path.
        //
        deviceInfo = DsmpFindDevInfoFromGroupAndFOGroup(dsmContext,
                                                        group,
                                                        oldFailGroup);

        if (deviceInfo) {

            //
            // Check if there is an alternate devInfo to be used temporarily
            // for this deviceInfo
            //
            failPathDevInfoEntry = DsmpFindFailPathDevInfoEntry(dsmContext,
                                                                group,
                                                                deviceInfo);

            if (failPathDevInfoEntry) {

                //
                // Use the alternate devInfo for now temporarily while the STPG
                // that was previously sent (asynchronously) works on making the
                // appropriate path active/optimized.
                //
                failGroup = (failPathDevInfoEntry->TempDeviceInfo)->FailGroup;
            }

            TracePrint((TRACE_LEVEL_WARNING,
                        TRACE_FLAG_RW,
                        "DsmLBGetPath (DsmIds %p): Couldn't find FOG but FO in progress, so returning devInfo %p (FOG %p path %p).\n",
                        DsmList,
                        deviceInfo,
                        deviceInfo->FailGroup,
                        deviceInfo->FailGroup->PathId));
        } else {

            //
            // Check if there is an RTPG in progress, if yes, return some path
            // for the IO to be sent down.
            //
            if (InterlockedCompareExchange((LONG volatile*)&group->InFlightRTPG, 0, 0)) {

                BOOLEAN sendTPG = FALSE;
                deviceInfo = DsmpFindStandbyPathToActivateALUA(group, &sendTPG);

                if (deviceInfo) {

                    failGroup = deviceInfo->FailGroup;

                    TracePrint((TRACE_LEVEL_WARNING,
                                TRACE_FLAG_RW,
                                "DsmLBGetPath (DsmIds %p): Couldn't find FOG but RPTG inflight, so returning devInfo %p (FOG %p path %p).\n",
                                DsmList,
                                deviceInfo,
                                deviceInfo->FailGroup,
                                deviceInfo->FailGroup->PathId));
                } else {

                    ASSERT(deviceInfo);

                    TracePrint((TRACE_LEVEL_WARNING,
                                TRACE_FLAG_RW,
                                "DsmLBGetPath (DsmIds %p): Couldn't find FOG but RTPG inflight, even then couldn't find alternative devInfo.\n",
                                DsmList));
                }
            }
        }
    }

    if (failGroup) {

        newPath = failGroup->PathId;
        *Status = STATUS_SUCCESS;

        //
        // If this is a retried request, our SetCompletion would have been bypassed,
        // and our completion routine won't yet get called, so update the old and
        // the new paths' stats.
        //
        if (Srb && DsmIsReadWrite(Srb->Cdb[0])) {

            PDSM_COMPLETION_CONTEXT completionContext;
            PDSM_FAILOVER_GROUP oldPath;
            PIRP irp;
            PIO_STACK_LOCATION irpStack;

            completionContext = DsmGetContextFromSrb(Srb);

            if (completionContext && completionContext->DeviceInfo) {

                //
                // This indicates that the request is being retried. So we need to:
                // 1. Update old path's and new path's request count
                // 2. If the old path was supposed to be removed, check if there are
                //    no more requests are outstanding, and if yes, remove the path
                // 3. Update the old and new devInfos' stats (TODO: jaivira: Revisit.
                //    The old devInfo may be already removed, so may be unsafe to touch)
                //

                irp = (PIRP)completionContext->RequestUnique1;
                irpStack = IoGetCurrentIrpStackLocation(irp);
                oldPath = irpStack->Parameters.Others.Argument3;

                if (oldPath) {

                    if (InterlockedCompareExchange((LONG volatile*)&oldPath->NumberOfRequestsInFlight, 0, 0) > 0) {

                        InterlockedDecrement(&oldPath->NumberOfRequestsInFlight);
                        InterlockedExchangeAdd64((LONGLONG volatile*)&(oldPath->OutstandingBytesOfIO), -(LONGLONG)(Srb->DataTransferLength));
                    }

                    if (InterlockedCompareExchange((LONG volatile*)&oldPath->NumberOfRequestsInFlight, 0, 0) == 0) {

                        //
                        // If there are no requests on a path that is supposed to be removed,
                        // remove it now.
                        //
                        if (oldPath->State == DSM_FG_PENDING_REMOVE) {

                            ASSERT(oldPath->Count == 0);

                            RemoveEntryList(&oldPath->ListEntry);
                            InterlockedDecrement((LONG volatile*)&dsmContext->NumberStaleFOGroups);

                            TracePrint((TRACE_LEVEL_INFORMATION,
                                        TRACE_FLAG_PNP,
                                        "DsmLBGetPath (DsmIds %p): Removing FOGroup %p with path %p.\n",
                                        DsmList,
                                        oldPath,
                                        oldPath->PathId));

                            DsmpFreePool(oldPath);
                        }
                    }

                    InterlockedIncrement(&failGroup->NumberOfRequestsInFlight);
                    InterlockedExchangeAdd64((LONGLONG volatile*)&(failGroup->OutstandingBytesOfIO), Srb->DataTransferLength);

                    completionContext->DeviceInfo = DsmpFindDevInfoFromGroupAndFOGroup(DsmContext, group, failGroup);
                    irpStack->Parameters.Others.Argument3 = failGroup;
                }
            }
        }

    } else {

        TracePrint((TRACE_LEVEL_ERROR,
                    TRACE_FLAG_RW,
                    "DsmLBGetPath (DsmIds %p): Failed to get FO group in LBGetPath.\n",
                    DsmList));

        *Status = STATUS_NO_SUCH_DEVICE;
    }

    KeReleaseSpinLock(&dsmContext->DSMContextLock, irql);

__Exit_DsmLBGetPath:

    TracePrint((TRACE_LEVEL_VERBOSE,
                TRACE_FLAG_RW,
                "DsmLBGetPath (DsmIds %p): Exiting function returning path %p for request %p.\n",
                DsmList,
                newPath,
                Srb));

    return newPath;
}


ULONG
DsmCategorizeRequest(
    __in IN PVOID DsmContext,
    __in IN PDSM_IDS DsmIds,
    __in IN PIRP Irp,
    __in IN PSCSI_REQUEST_BLOCK Srb,
    __in IN PVOID CurrentPath,
    __deref_out_opt OUT PVOID *PathId,
    __out OUT NTSTATUS *Status
    )
/*++

Routine Description:

    This routine is called when a request is received other than a read/write.
    It will determine the best path to which the request is to be sent.

    In order to support clusters, reserve and release need to be handled
    via SrbControl.

Arguments:

    DsmContext - Context value given to the multipath driver during
                 registration.
    DsmIds - List of our DSM IDs.
    Irp - The Irp containing Srb.
    Srb - The current non-read/write Srb.
    CurrentPath - The last path that was returned for this multi-path group.
    PathId - Placeholder for the PathID
    Status - Storage to place NTSTATUS of the call.

Return Value:

    DSM_PATH_SET - Indicates PathID is valid.
    DSM_ERROR - Couldn't get a path.

--*/
{
    ULONG dsmStatus;
    NTSTATUS status = STATUS_UNSUCCESSFUL;

    TracePrint((TRACE_LEVEL_VERBOSE,
                TRACE_FLAG_IOCTL,
                "DsmCategorizeRequest (DsmIds %p): Entering function.\n",
                DsmIds));

    //
    // Determine whether this is a special-case request.
    //
    if (DsmpReservationCommand(Irp, Srb)) {

        dsmStatus = DSM_WILL_HANDLE;
        goto __Exit_DsmCategorizeRequest;
    }

    //
    // If this is a mpio pass through or a mpio pass through direct request,
    // pick the path that corresponds to the pathId specified.
    //
    if (DsmpMpioPassThroughPathCommand(Irp)) {

        *PathId = DsmpGetPathIdFromPassThroughPath(DsmContext,
                                                   DsmIds,
                                                   Irp,
                                                   &status);
    } else {

        //
        // For requests other than reservation-handling and pass through, punt
        // it back to the bus-driver. Need to get a path for the request first,
        // so call the Load-Balance function.
        //
        *PathId = DsmLBGetPath(DsmContext,
                               Srb,
                               DsmIds,
                               CurrentPath,
                               &status);
    }

    if (NT_SUCCESS(status)) {

        DSM_ASSERT(*PathId);

        //
        // Indicate that the path is updated, and mpctl should handle the request.
        //
        dsmStatus = DSM_PATH_SET;

    } else {

        //
        // Indicate the error back to mpctl.
        //
        dsmStatus = DSM_ERROR;

        //
        // Mark-up the Srb to show that a failure has occurred.
        // This value is really only for this DSM to know what to do
        // in the InterpretError routine - Fatal Error.
        // It could be something more meaningful.
        //
        if (Srb) {
            Srb->SrbStatus = SRB_STATUS_NO_DEVICE;
        }

        *PathId = NULL;
    }

    //
    // Pass back status info to mpctl.
    //
    *Status = status;

__Exit_DsmCategorizeRequest:

    TracePrint((TRACE_LEVEL_VERBOSE,
                TRACE_FLAG_IOCTL,
                "DsmCategorizeRequest (DsmIds %p): Exiting function with categorization %x.\n",
                DsmIds,
                dsmStatus));

    return dsmStatus;
}


NTSTATUS
DsmBroadcastRequest(
    __in IN PVOID DsmContext,
    __in IN PDSM_IDS DsmIds,
    __in IN PIRP Irp,
    __in IN PSCSI_REQUEST_BLOCK Srb,
    __in IN PKEVENT Event
    )
/*++

Routine Description:

    This routine is called when the DSM has indicated that Srb should be
    sent to the device down all paths. The DSM will update IoStatus
    information and status, but not complete the request.

    Currently MSDSM doesn't have a need for this.

Arguments:

    DsmIds - The collection of DSM IDs that pertain to the MPDisk.
    Irp - Irp containing SRB.
    Srb - Scsi request block
    Event - DSM sets this once all sub-requests have completed and
            the original request's IoStatus has been setup.

Return Value:

    NTSTATUS of the operation.

--*/
{
    NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;

    UNREFERENCED_PARAMETER(DsmContext);
    UNREFERENCED_PARAMETER(Srb);
    UNREFERENCED_PARAMETER(Irp);

    TracePrint((TRACE_LEVEL_VERBOSE,
                TRACE_FLAG_IOCTL,
                "DsmBroadcastRequest (DsmIds %p): Entering function.\n",
                DsmIds));

    //
    // Currently nothing is handled via Broadcast. Just set the event to
    // free up the request handling in the bus-driver.
    //
    ASSERT(NT_SUCCESS(status));
    KeSetEvent(Event, 0, FALSE);

    TracePrint((TRACE_LEVEL_VERBOSE,
                TRACE_FLAG_IOCTL,
                "DsmBroadcastReqeust (DsmIds %p): Exiting function with status %x.\n",
                DsmIds,
                status));

    return status;
}


NTSTATUS
DsmSrbDeviceControl(
    __in IN PVOID DsmContext,
    __in IN PDSM_IDS DsmIds,
    __in IN PIRP Irp,
    __in IN PSCSI_REQUEST_BLOCK Srb,
    __in IN PKEVENT Event
    )
/*++

Routine Description:

    This routine is called when the DSM has indicated that it wants to handle
    it internally (via returning DSM_WILL_HANDLE in CategorizeRequest).

    It should set IoStatus (Status and Information) and the Event, but not
    complete the request.

Arguments:

    DsmContext - The DSM's context
    DsmIds - The collection of DSM IDs that pertain to the MPDISK.
    Irp - Irp containing SRB.
    Srb - Scsi request block
    Event - Event to be set when the DSM is finished if DsmHandled is TRUE

Return Value:

    NTSTATUS of the request.

--*/
{
    PDSM_CONTEXT dsmContext = DsmContext;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    NTSTATUS status;
    UCHAR opCode;

    TracePrint((TRACE_LEVEL_VERBOSE,
                TRACE_FLAG_IOCTL,
                "DsmSrbDeviceControl (DsmIds %p): Entering function.\n",
                DsmIds));

    if (!DsmIds || !DsmIds->Count) {

        TracePrint((TRACE_LEVEL_ERROR,
                    TRACE_FLAG_IOCTL,
                    "DsmSrbDeviceControl (DsmIds %p): No DsmIds passed in.\n",
                    DsmIds));

        status = STATUS_NO_SUCH_DEVICE;
        goto __Exit_DsmSrbDeviceControl;
    }

    if (irpStack->MajorFunction == IRP_MJ_SCSI) {

        //
        // Determine the operation.
        //
        opCode = Srb->Cdb[0];

        if (opCode == SCSIOP_PERSISTENT_RESERVE_OUT) {

            status = DsmpPersistentReserveOut(dsmContext,
                                              DsmIds,
                                              Irp,
                                              Srb,
                                              Event);

        } else if (opCode == SCSIOP_PERSISTENT_RESERVE_IN) {

            status = DsmpPersistentReserveIn(dsmContext,
                                             DsmIds,
                                             Irp,
                                             Srb,
                                             Event);

        } else {

            //
            // Should never be here.
            //
            DSM_ASSERT(FALSE);
            status = STATUS_INVALID_DEVICE_REQUEST;
        }
    } else {

        //
        // Should never be here.
        //
        DSM_ASSERT(irpStack->MajorFunction == IRP_MJ_SCSI);
        status = STATUS_INVALID_DEVICE_REQUEST;
    }

__Exit_DsmSrbDeviceControl:
    if (status != STATUS_PENDING) {

        //
        // Set-up the Irp status for mpio's completion of the request.
        // If it was IRP_MJ_SCSI, one of the helper routines set Srb->SrbStatus
        // already.
        //
        if (Srb && Srb->SrbStatus == SRB_STATUS_PENDING) {
            Srb->SrbStatus = SRB_STATUS_ERROR;
        }

        Irp->IoStatus.Status = status;

        //
        // Set the event to free up the request handling in the bus-driver.
        //
        KeSetEvent(Event, 0, FALSE);
    }

    TracePrint((TRACE_LEVEL_VERBOSE,
                TRACE_FLAG_IOCTL,
                "DsmSrbDeviceControl (DsmIds %p): Exiting function with status %x.\n",
                DsmIds,
                status));

    return status;
}


VOID
DsmSetCompletion(
    __in IN PVOID DsmContext,
    __in IN PVOID DsmId,
    __in IN PIRP Irp,
    __in IN PSCSI_REQUEST_BLOCK Srb,
    __inout IN OUT PDSM_COMPLETION_INFO DsmCompletion
    )
/*++

Routine Description:

    This routine is called before the actual submission of a request,
    but after the categorisation of the I/O. This will be called only
    for those requests not handled by the DSM directly:
         Read/Write
         Other requests not handled by SrbControl or Broadcast

Arguments:

    DsmContext - The DSM's context.
    DsmId - Identifer that was indicated when the request was
            categorized (or be LBGetPath)
    Irp - Irp containing Srb.
    Srb - The request
    DsmCompletion - Completion info structure to be filled out by DSM.

Return Value:

    None

--*/
{
    PDSM_COMPLETION_CONTEXT completionContext;
    PDSM_CONTEXT dsmContext = DsmContext;
    PDSM_DEVICE_INFO deviceInfo = DsmId;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    PDSM_FAILOVER_GROUP failGroup = deviceInfo->FailGroup;

    TracePrint((TRACE_LEVEL_VERBOSE,
                TRACE_FLAG_RW,
                "DsmSetCompletion (DevInfo %p): Entering function.\n",
                DsmId));

    //
    // Save the DeviceInfo as being the target for this request.
    // Get a timestamp
    // TODO Determine other data.
    //
    completionContext = ExAllocateFromNPagedLookasideList(&dsmContext->CompletionContextList);
    if (!completionContext) {

        TracePrint((TRACE_LEVEL_ERROR,
                    TRACE_FLAG_RW,
                    "DsmSetCompletion (DevInfo %p): Failed to alloc completion context for request %p.\n",
                    DsmId,
                    Irp));

        DsmCompletion->DsmCompletionRoutine = NULL;
        DsmCompletion->DsmContext = NULL;

        goto __Exit_DsmSetCompletion;
    }

#if DBG
    //
    // Time stamp this.
    //
    KeQueryTickCount(&completionContext->TickCount);
#endif

    //
    // Indicate the target for this request.
    // This can change if the I/O is retried, so don't depend on this.
    //
    completionContext->DeviceInfo = deviceInfo;
    completionContext->DsmContext = DsmContext;
    completionContext->RequestUnique1 = (PVOID)Irp;

    //
    // Save off the path that was selected to service this request in Argument3.
    //
    irpStack->Parameters.Others.Argument3 = failGroup;

    //
    // Indicate an inflight request on this path. LB may use this.
    //
    InterlockedIncrement(&failGroup->NumberOfRequestsInFlight);

    //
    // If this is a read/write request, reflect the increase in cumulative
    // outstanding IO down the selected path.
    //
    if (Srb && DsmIsReadWrite(Srb->Cdb[0])) {

        InterlockedExchangeAdd64((LONGLONG volatile*)&(failGroup->OutstandingBytesOfIO), Srb->DataTransferLength);
    }

    if (!dsmContext->DisableStatsGathering) {

        //
        // Indicate one more request on this device down this path.
        //
        InterlockedIncrement(&deviceInfo->NumberOfRequestsInProgress);
    }

    //
    // Update the passed-in struct with our routine and context values.
    //
    DsmCompletion->DsmCompletionRoutine = DsmpRequestComplete;
    DsmCompletion->DsmContext = completionContext;

__Exit_DsmSetCompletion:

    TracePrint((TRACE_LEVEL_VERBOSE,
                TRACE_FLAG_RW,
                "DsmSetCompletion (DevInfo %p): Exiting function.\n",
                DsmId));

    return;
}


ULONG
DsmInterpretError(
    __in IN PVOID DsmContext,
    __in IN PVOID DsmId,
    __in IN PSCSI_REQUEST_BLOCK Srb,
    __inout IN OUT NTSTATUS *Status,
    __out OUT PBOOLEAN Retry,
    __out OUT PLONG RetryInterval,
    ...
    )
/*++

Routine Description:

    This routine is invoked by MPIO if Status is other than SUCCESS.
    A few NTSTATUS and SRB_STATUS values indicate a fatal error.
    Also checked are unit attentions, for which a retry is requested.

Arguments:

    DsmContext - The DSM's context.
    DsmId - Identifers returned from DMS_INQUIRE_DRIVER.
    Srb - The Srb with an error.
    Status - NTSTATUS of the operation. Can be updated.
    Retry - Allows the DSM to indicate whether to retry the IO.
    RetryInterval - Lets DSM specify (in seconds) when this specific I/O
                    should be retried. Use MAXLONG to use the default
                    retry interval. Use zero to retry immediately.

Return Value:

    DSM_FATAL_ERROR indicates a fatal error.

--*/
{
    //
    // The requests that will be encountered can be divided into four categories:
    // 1. The request that has failed.
    // 2. Subsequent requests that were sent down the failing path that will
    //    complete with failure.
    // 3. Requests that were already submitted to LBGetPath() just before InterpretError()
    //    was called for the failed request (but have yet to have the LB policy
    //    algo run).
    // 4. Requests that come into the Dispatch() routine after the failed request
    //    has been processed by InterpretError().
    //
    // For the failed request:
    // =======================
    // 1. Find a standby path to make active/optimized.
    // 2. Send STPG asynchronously as a scsi pass through via IRP_MJ_SCSI (this
    //    way it can be sent at DISPATCH_IRQL) after setting a completion routine.
    // 3. Save the devInfo corresponding to the standby path for the failing devInfo.
    // 4. Return FATAL to MPIO so that new IO are queued.
    // 5. In the completion routine, update the new states for the devInfos. Then
    //    clear the saved (previously) standby devInfo for the failing devInfo.
    //
    // For the subsequent request that will fail (since it was sent on the failing path):
    // ==================================================================================
    // 1. If a standby devInfo has been saved off, it indicates that an STPG was
    //    already sent, so no need to send another one.
    // 2. Return FATAL to MPIO so that this request gets queued.
    //
    // For the requests that were already submitted to LBGetPath() during this time:
    // =============================================================================
    // 1. If there is no active path, check if a standby devInfo has been saved
    //    away. If it has, return this path. Such requests will fail with check
    //    condition saying path used is in standby.
    // 2. In InterpretError() retry (since the error indicates that request
    //    completed before STPG completed) without decrementing the remaining
    //    retries count.
    //
    // For new requests that come into Dispatch() after above processing:
    // ==================================================================
    // We don't need to worry about such requests, since MPIO will queue them
    // automatically.
    //

    PDSM_DEVICE_INFO deviceInfo = DsmId;
    ULONG errorMask = 0;
    PSENSE_DATA senseData = Srb->SenseInfoBuffer;
    BOOLEAN failover = FALSE;
    BOOLEAN retry = FALSE;
    BOOLEAN handled = FALSE;
    BOOLEAN sendTPG = FALSE;
    BOOLEAN tpgException = FALSE;
    BOOLEAN devInfoException = FALSE;

    TracePrint((TRACE_LEVEL_VERBOSE,
                TRACE_FLAG_RW,
                "DsmInterpretError (DevInfo %p): Entering function.\n",
                DsmId));

    *RetryInterval = MAXLONG;

    if ((Srb->ScsiStatus == SCSISTAT_RESERVATION_CONFLICT) ||
        (*Status == STATUS_DEVICE_BUSY)) {

        TracePrint((TRACE_LEVEL_WARNING,
                    TRACE_FLAG_RW,
                    "DsmInterpretError (DevInfo %p): Either busy or res. conflict (%x %x).\n",
                    DsmId,
                    Srb->ScsiStatus,
                    *Status));
    }

    //
    // Check the NT Status first.
    // Several are clearly failover conditions.
    //
    switch (*Status) {
        case STATUS_DEVICE_NOT_CONNECTED:
        case STATUS_DEVICE_DOES_NOT_EXIST:
        case STATUS_NO_SUCH_DEVICE: {

            //
            // The port pdo has either been removed or is
            // very broken. A fail-over is necessary.
            //
            TracePrint((TRACE_LEVEL_ERROR,
                        TRACE_FLAG_RW,
                        "DsmInterpretError (DevInfo %p): Will initiate fail over. Status %x. Opcode %x.\n",
                        DsmId,
                        *Status,
                        Srb->Cdb[0]));

            handled = TRUE;
            failover = TRUE;
            break;
        }

        case STATUS_IO_DEVICE_ERROR: {

            if (Srb->SrbStatus & SRB_STATUS_AUTOSENSE_VALID) {

                //
                // See if it's a unit attention.
                //
                if ((senseData->SenseKey & 0xF) == SCSI_SENSE_UNIT_ATTENTION) {

                    switch (senseData->AdditionalSenseCode) {

                        case SCSI_ADSENSE_PARAMETERS_CHANGED: {

                            switch (senseData->AdditionalSenseCodeQualifier) {

                                case SPC3_SCSI_SENSEQ_ASYMMETRIC_ACCESS_STATE_CHANGED:
                                case SPC3_SCSI_SENSEQ_IMPLICIT_ASYMMETRIC_ACCESS_STATE_TRANSITION_FAILED: {

                                    TracePrint((TRACE_LEVEL_ERROR,
                                                TRACE_FLAG_RW,
                                                "DsmInterpretError (DevInfo %p): TPG states have changed. Requesting retry on Srb %p. Will send asyn RPTG.\n",
                                                DsmId,
                                                Srb));

                                    //
                                    // Retry but after sending RTPG, which will update the path states.
                                    //
                                    sendTPG = TRUE;
                                    retry = TRUE;
                                    handled = TRUE;
                                    errorMask = DSM_RETRY_DONT_DECREMENT;

                                    if (senseData->AdditionalSenseCodeQualifier == SPC3_SCSI_SENSEQ_ASYMMETRIC_ACCESS_STATE_CHANGED) {

                                        //
                                        // Worth retrying on the same path.
                                        //
                                        devInfoException = TRUE;
                                        ASSERT(!tpgException);
                                    }

                                    break;
                                }

                                case SPC3_SCSI_SENSEQ_MODE_PARAMETERS_CHANGED:
                                case SPC3_SCSI_SENSEQ_RESERVATIONS_PREEMPTED:
                                case SPC3_SCSI_SENSEQ_RESERVATIONS_RELEASED:
                                case SPC3_SCSI_SENSEQ_REGISTRATIONS_PREEMPTED:
                                case SPC3_SCSI_SENSEQ_CAPACITY_DATA_HAS_CHANGED: {

                                    TracePrint((TRACE_LEVEL_ERROR,
                                                TRACE_FLAG_RW,
                                                "DsmInterpretError (DevInfo %p): Failing request. STATUS_IO_DEVICE_ERROR (params changed). SrbStatus (%x) Scsi (%x) AddQual (%u).\n",
                                                DsmId,
                                                Srb->SrbStatus,
                                                Srb->ScsiStatus,
                                                senseData->AdditionalSenseCodeQualifier));

                                    //
                                    // Just fail these back.
                                    //
                                    handled = TRUE;
                                    break;
                                }

                                default: {

                                    TracePrint((TRACE_LEVEL_ERROR,
                                                TRACE_FLAG_RW,
                                                "DsmInterpretError (DevInfo %p): UNIT_ATTENTION for params changed. ASCQ %x. Asking for retry on Srb %p.\n",
                                                DsmId,
                                                senseData->AdditionalSenseCodeQualifier,
                                                Srb));

                                    //
                                    // Indicate that a retry is necessary.
                                    //
                                    retry = TRUE;
                                    handled = TRUE;

                                    break;
                                }
                            }

                            break;
                        }

                        case SPC3_SCSI_ADSENSE_COMMANDS_CLEARED_BY_ANOTHER_INITIATOR: {

                            if (senseData->AdditionalSenseCodeQualifier == 0x00) {

                                TracePrint((TRACE_LEVEL_ERROR,
                                            TRACE_FLAG_RW,
                                            "DsmInterpretError (DevInfo %p): UNIT_ATTENTION (commands cleared by another initiator). Fail back to upper level. Srb %p.\n",
                                            DsmId,
                                            Srb));

                                //
                                // Commands cleared by another Initiator
                                //
                                handled = TRUE;

                            } else {

                                TracePrint((TRACE_LEVEL_ERROR,
                                            TRACE_FLAG_RW,
                                            "DsmInterpretError (DevInfo %p): UNIT_ATTENTION (commands cleared by another initiator). ASCQ %x. Asking for retry on Srb %p.\n",
                                            DsmId,
                                            senseData->AdditionalSenseCodeQualifier,
                                            Srb));

                                //
                                // Indicate that a retry is necessary.
                                //
                                retry = TRUE;
                                handled = TRUE;
                            }


                            break;
                        }

                        case SCSI_ADSENSE_OPERATING_CONDITIONS_CHANGED: {

                            if (senseData->AdditionalSenseCodeQualifier == SCSI_SENSEQ_VOLUME_SET_MODIFIED ||
                                senseData->AdditionalSenseCodeQualifier == SCSI_SENSEQ_REPORTED_LUNS_DATA_CHANGED) {

                                TracePrint((TRACE_LEVEL_ERROR,
                                            TRACE_FLAG_RW,
                                            "DsmInterpretError (DevInfo %p): VolumeSet/LunsData changed. Fail Srb %p.\n",
                                            DsmId,
                                            Srb));

                                //
                                // Fail back to upper layers.
                                //
                                handled = TRUE;

                                break;

                            } else {

                                //
                                // Fall through to default case (ie. retry the request)
                                //
                            }
                        }

                        default: {

                            TracePrint((TRACE_LEVEL_ERROR,
                                        TRACE_FLAG_RW,
                                        "DsmInterpretError (DevInfo %p): UNIT_ATTENTION. ASC %x, ASCQ %x. Asking for retry on Srb %p.\n",
                                        DsmId,
                                        senseData->AdditionalSenseCode,
                                        senseData->AdditionalSenseCodeQualifier,
                                        Srb));

                            //
                            // Indicate that a retry is necessary.
                            //
                            retry = TRUE;
                            handled = TRUE;

                            break;
                        }
                    }
                } else if ((senseData->SenseKey & 0xF) == SCSI_SENSE_NOT_READY) {

                    //
                    // See if failure is due to device's current TPG state.
                    //
                    if (senseData->AdditionalSenseCode == SCSI_ADSENSE_LUN_NOT_READY) {

                        if (senseData->AdditionalSenseCodeQualifier == SPC3_SCSI_SENSEQ_ASYMMETRIC_ACCESS_STATE_TRANSITION ||
                            senseData->AdditionalSenseCodeQualifier == SPC3_SCSI_SENSEQ_TARGET_PORT_IN_STANDBY_STATE ||
                            senseData->AdditionalSenseCodeQualifier == SPC3_SCSI_SENSEQ_TARGET_PORT_IN_UNAVAILABLE_STATE) {

                            if (Srb->ScsiStatus == SCSISTAT_CHECK_CONDITION) {

                                TracePrint((TRACE_LEVEL_ERROR,
                                            TRACE_FLAG_RW,
                                            "DsmInterpretError (DevInfo %p): TPG-transition/TPG-SB/TPG-UA. ASCQ %x. Will send down async RTPG. Asking for retry on Srb %p.\n",
                                            DsmId,
                                            senseData->AdditionalSenseCodeQualifier,
                                            Srb));

                                //
                                // Indicate that a retry is necessary but without decrementing the remaining
                                // retries count. However, we may need to send down an STPG/RTPG also.
                                // And we must set PTBU to a path that is in a different TPG.
                                //
                                sendTPG = TRUE;
                                tpgException = TRUE;
                                ASSERT(!devInfoException);
                                retry = TRUE;
                                handled = TRUE;
                                errorMask = DSM_RETRY_DONT_DECREMENT;
                            }
                        }
                    }
                }

            } else if (Srb->SrbStatus == SRB_STATUS_BUS_RESET) {

                TracePrint((TRACE_LEVEL_ERROR,
                            TRACE_FLAG_RW,
                            "DsmInterpretError (DevInfo %p): BUS_RESET. Asking for retry on Srb %p.\n",
                            DsmId,
                            Srb));

                //
                // Bus reset indicates many things. Among them is a link
                // transition. Retry these as it might be a real reset.
                //
                retry = TRUE;
                handled = TRUE;

            } else {

                TracePrint((TRACE_LEVEL_ERROR,
                            TRACE_FLAG_RW,
                            "DsmInterpretError (DevInfo %p): Failing request. STATUS_IO_DEVICE_ERROR. SrbStatus (%x) ScsiStatus (%x).\n",
                            DsmId,
                            Srb->SrbStatus,
                            Srb->ScsiStatus));
            }

            break;
        }

        case STATUS_BUFFER_OVERFLOW: {

            if (DsmIsReadWrite(Srb->Cdb[0])) {

                TracePrint((TRACE_LEVEL_ERROR,
                            TRACE_FLAG_RW,
                            "DsmInterpretError (DevInfo %p): BUFFER_OVERFLOW: Retry.\n",
                            DsmId));

                //
                // Retry these, as this condition might indicate a torn write.
                //
                retry = TRUE;
                handled = TRUE;
            }

            break;
        }

        case STATUS_DEVICE_BUSY: {

            //
            // See if it's a check condition for TPG states in transition.
            //
            if (Srb->SrbStatus & SRB_STATUS_AUTOSENSE_VALID &&
                Srb->ScsiStatus == SCSISTAT_CHECK_CONDITION &&
                (senseData->SenseKey & 0xF) == SCSI_SENSE_NOT_READY &&
                senseData->AdditionalSenseCode == SCSI_ADSENSE_LUN_NOT_READY &&
                senseData->AdditionalSenseCodeQualifier == SPC3_SCSI_SENSEQ_ASYMMETRIC_ACCESS_STATE_TRANSITION) {

                TracePrint((TRACE_LEVEL_ERROR,
                            TRACE_FLAG_RW,
                            "DsmInterpretError (DevInfo %p): TPG transition. Will send down async RTPG. Asking for retry on Srb %p.\n",
                            DsmId,
                            Srb));

                //
                // Indicate that a retry is necessary but without decrementing the remaining
                // retries count. However, we may need to send down an STPG/RTPG also.
                // And we must set PTBU to a path that is in a different TPG.
                //
                sendTPG = TRUE;
                tpgException = TRUE;
                ASSERT(!devInfoException);
                retry = TRUE;
                handled = TRUE;
                errorMask = DSM_RETRY_DONT_DECREMENT;
            }

            break;
        }

        case STATUS_DEVICE_NOT_READY: {

            if (Srb->SrbStatus & SRB_STATUS_AUTOSENSE_VALID &&
                Srb->ScsiStatus == SCSISTAT_CHECK_CONDITION &&
                (senseData->SenseKey & 0xF) == SCSI_SENSE_NOT_READY &&
                senseData->AdditionalSenseCode == SCSI_ADSENSE_LUN_NOT_READY) {

                switch (senseData->AdditionalSenseCodeQualifier) {

                    case SCSI_SENSEQ_MANUAL_INTERVENTION_REQUIRED: {

                        TracePrint((TRACE_LEVEL_ERROR,
                                    TRACE_FLAG_RW,
                                    "DsmInterpretError (DevInfo %p): Manual intervention required. Asking for retry on Srb %p.\n",
                                    DsmId,
                                    Srb));

                        //
                        // This may be caused by NDU of controller firmware. It does not
                        // necessarily indicate that the device won't be ready via other path(s).
                        // Worth retrying instead of immediately failing back.
                        //
                        retry = TRUE;
                        handled = TRUE;

                        break;
                    }

                    default: {

                        TracePrint((TRACE_LEVEL_ERROR,
                                    TRACE_FLAG_RW,
                                    "DsmInterpretError (DevInfo %p): Unhandled AddQual %x.\n",
                                    DsmId,
                                    senseData->AdditionalSenseCodeQualifier));

                        break;
                    }
                }
            }
        }

        default: {

            TracePrint((TRACE_LEVEL_WARNING,
                        TRACE_FLAG_RW,
                        "DsmInterpretError (DevInfo %p): Unhandled status code %x.\n",
                        DsmId,
                        *Status));

            break;
        }
    }

    if (!handled) {

        //
        // The NTSTATUS didn't indicate a fail-over condition, but
        // check various srb status for failover-class error.
        //
        switch (Srb->SrbStatus) {
            case SRB_STATUS_SELECTION_TIMEOUT:
            case SRB_STATUS_INVALID_LUN:
            case SRB_STATUS_INVALID_TARGET_ID:
            case SRB_STATUS_NO_DEVICE:
            case SRB_STATUS_NO_HBA:
            case SRB_STATUS_INVALID_PATH_ID: {

                //
                // All of these are fatal.
                //
                TracePrint((TRACE_LEVEL_ERROR,
                            TRACE_FLAG_RW,
                            "DsmInterpretError (DevInfo %p): SrbStatus 0x%x. Will initiate fail over.\n",
                            DsmId,
                            Srb->SrbStatus));

                failover = TRUE;
                break;
            }

            default: {

                if ((Srb->ScsiStatus == SCSISTAT_CHECK_CONDITION) &&
                    (Srb->SrbStatus & SRB_STATUS_AUTOSENSE_VALID)) {

                    switch (senseData->SenseKey & 0xF) {

                        case SCSI_SENSE_NO_SENSE: {

                            if (senseData->AdditionalSenseCode == SCSI_ADSENSE_NO_SENSE &&
                                senseData->AdditionalSenseCodeQualifier == SCSI_SENSEQ_CAUSE_NOT_REPORTABLE) {

                                TracePrint((TRACE_LEVEL_ERROR,
                                            TRACE_FLAG_RW,
                                            "DsmInterpretError (DevInfo %p): CheckCondition with no sense info. Will initiate fail over.\n",
                                            DsmId));

                                //
                                // This could be a transient error generated
                                // in response to potentially a hardware fault.
                                // Worth trying another path.
                                //
                                failover = TRUE;
                                handled = TRUE;
                            }

                            break;
                        }

                        case SCSI_SENSE_ILLEGAL_REQUEST: {

                            if (senseData->AdditionalSenseCode == SCSI_ADSENSE_INVALID_LUN) {

                                if (senseData->AdditionalSenseCodeQualifier == 0x00) {

                                    TracePrint((TRACE_LEVEL_ERROR,
                                                TRACE_FLAG_RW,
                                                "DsmInterpretError (DevInfo %p): Invalid LUN. Will initiate fail over.\n",
                                                DsmId));

                                    //
                                    // LUN may still exist on other path(s).
                                    // Worth a failover.
                                    //
                                    failover = TRUE;
                                    handled = TRUE;
                                }
                            }

                            break;
                        }

                        case SCSI_SENSE_HARDWARE_ERROR: {

                            if (senseData->AdditionalSenseCode == SPC3_SCSI_ADSENSE_LOGICAL_UNIT_COMMAND_FAILED) {

                                if (senseData->AdditionalSenseCodeQualifier == SPC3_SCSI_SENSEQ_SET_TARGET_PORT_GROUPS_FAILED) {

                                    TracePrint((TRACE_LEVEL_ERROR,
                                                TRACE_FLAG_RW,
                                                "DsmInterpretError (DevInfo %p): STPG failed. Will initiate fail over.\n",
                                                DsmId));

                                    //
                                    // If an STPG failed, treat as FATAL and get another
                                    // path set to A/O via another STPG.
                                    //
                                    failover = TRUE;
                                    handled = TRUE;
                                }
                            }

                            break;
                        }

                        default: {

                            break;
                        }
                    }
                }

                if (!handled) {

                    TracePrint((TRACE_LEVEL_WARNING,
                                TRACE_FLAG_RW,
                                "DsmInterpretError (DevInfo %p): Unhandled SRB Status 0x%x. Sense data %x|%x|%x.\n",
                                DsmId,
                                Srb->SrbStatus,
                                senseData ? senseData->SenseKey & 0xF : 0xFF,
                                senseData ? senseData->AdditionalSenseCode : 0xFF,
                                senseData ? senseData->AdditionalSenseCodeQualifier : 0xFF));
                }

                break;
            }
        }
    }

    if (failover) {

        //
        // If ALUA is supported, then it is possible that we may need to send
        // down an STPG so build an IRP and fill in the SRB for STPG and send it down.
        //
        if (!DsmpIsSymmetricAccess(deviceInfo)) {

            DsmpSetLBForPathFailingALUA(DsmContext, deviceInfo);

        } else {

            //
            // If device doesn't support ALUA, we just need to update
            // states without sending down any commands (STPG)
            //
            DsmpSetLBForPathFailing(DsmContext, deviceInfo);
        }

        errorMask = DSM_FATAL_ERROR;
        TracePrint((TRACE_LEVEL_INFORMATION,
                    TRACE_FLAG_RW,
                    "DsmInterpretError(DevInfo %p): Device changed to state %d\n",
                    deviceInfo,
                    deviceInfo->State));

#if DBG
        {
            ULONG inx;
            PDSM_GROUP_ENTRY group = deviceInfo->Group;
            PDSM_DEVICE_INFO tempDevInfo;

            TracePrint((TRACE_LEVEL_ERROR,
                        TRACE_FLAG_RW,
                        "DsmInterpretError (DevInfo %p): Device %p in group %p being marked as failed. NTStatus 0x%x.\n",
                        DsmId,
                        deviceInfo,
                        group,
                        *Status));

            for (inx = 0; inx < group->NumberDevices; inx++) {

                tempDevInfo = group->DeviceList[inx];

                TracePrint((TRACE_LEVEL_ERROR,
                            TRACE_FLAG_RW,
                            "DsmInterpretError (DevInfo %p): Device %p at %d. State %d.\n",
                            DsmId,
                            tempDevInfo,
                            inx,
                            tempDevInfo->State));
            }
        }
#endif // DBG
    }

    if (retry) {

        if (sendTPG) {

            //
            // If ALUA is supported, send down STPG/RTPG as appropriate.
            //
            if (!DsmpIsSymmetricAccess(deviceInfo)) {

                DsmpSetPathForIoRetryALUA(DsmContext, deviceInfo, tpgException, devInfoException);

                TracePrint((TRACE_LEVEL_INFORMATION,
                            TRACE_FLAG_RW,
                            "DsmInterpretError(DevInfo %p): SRB request %p will be retried. PTBU set to %p.\n",
                            deviceInfo,
                            Srb,
                            deviceInfo->Group->PathToBeUsed));
            }
        }
    }

    *Retry = retry;

    TracePrint((TRACE_LEVEL_VERBOSE,
                TRACE_FLAG_RW,
                "DsmInterpretError (DevInfo %p): Exiting function returning errorMask %x.\n",
                DsmId,
                errorMask));

    return errorMask;
}


NTSTATUS
DsmUnload(
    __in IN PVOID DsmContext
    )
/*++

Routine Description:

    This routine is called when the main module requires the DSM to be unloaded
    (ie. prior to the main module unload).

Arguments:

    DsmContext - Context value passed to DsmInitialize()

Return Value:

    STATUS_SUCCESS;

--*/

{
    PVOID tempAddress = DsmContext;

    TracePrint((TRACE_LEVEL_VERBOSE,
                TRACE_FLAG_INIT,
                "DsmUnload (DsmCtxt %p): Entering function.\n",
                DsmContext));

    DsmpFreeDSMResources((PDSM_CONTEXT) DsmContext);

    if (gMPIOControlObjectRefd) {

        ObDereferenceObject(gMPIOControlObject);
        gMPIOControlObjectRefd = FALSE;
    }

    TracePrint((TRACE_LEVEL_VERBOSE,
                TRACE_FLAG_INIT,
                "DsmUnload (DsmCtxt %p): Exiting function.\n",
                tempAddress));

    //
    // Stop the tracing subsystem.
    //
    WPP_CLEANUP(gDsmDriverObject);

    return STATUS_SUCCESS;
}

