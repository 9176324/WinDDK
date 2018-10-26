/*++

Copyright (C) Microsoft Corporation, 1991 - 1999

Module Name:

    disk.c

Abstract:

    SCSI disk class driver

Environment:

    kernel mode only

Notes:

Revision History:

--*/

#include "disk.h"

//
// Now instantiate the GUIDs
//

#include "initguid.h"
#include "ntddstor.h"
#include "ntddvol.h"
#include "ioevent.h"

NTSTATUS
DiskDetermineMediaTypes(
    IN PDEVICE_OBJECT Fdo,
    IN PIRP     Irp,
    IN UCHAR    MediumType,
    IN UCHAR    DensityCode,
    IN BOOLEAN  MediaPresent,
    IN BOOLEAN  IsWritable
    );

PPARTITION_INFORMATION_EX
DiskPdoFindPartitionEntry(
    IN PPHYSICAL_DEVICE_EXTENSION Pdo,
    IN PDRIVE_LAYOUT_INFORMATION_EX LayoutInfo
    );

PPARTITION_INFORMATION_EX
DiskFindAdjacentPartition(
    IN PDRIVE_LAYOUT_INFORMATION_EX LayoutInfo,
    IN PPARTITION_INFORMATION_EX BasePartition
    );

PPARTITION_INFORMATION_EX
DiskFindContainingPartition(
    IN PDRIVE_LAYOUT_INFORMATION_EX LayoutInfo,
    IN PPARTITION_INFORMATION_EX BasePartition,
    IN BOOLEAN SearchTopToBottom
    );

NTSTATUS
DiskIoctlCreateDisk(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
DiskIoctlGetDriveLayout(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
DiskIoctlGetDriveLayoutEx(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
DiskIoctlSetDriveLayout(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
DiskIoctlSetDriveLayoutEx(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
DiskIoctlGetPartitionInfo(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
DiskIoctlGetPartitionInfoEx(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
DiskIoctlGetLengthInfo(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
DiskIoctlSetPartitionInfo(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
DiskIoctlSetPartitionInfoEx(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
DiskIoctlSetPartitionInfoEx(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
DiskIoctlGetDriveGeometryEx(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

#ifdef ALLOC_PRAGMA

#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, DiskUnload)
#pragma alloc_text(PAGE, DiskCreateFdo)
#pragma alloc_text(PAGE, DiskDetermineMediaTypes)
#pragma alloc_text(PAGE, DiskShutdownFlush)
#pragma alloc_text(PAGE, DiskFlushDispatch)
#pragma alloc_text(PAGE, DiskModeSelect)
#pragma alloc_text(PAGE, DisableWriteCache)
#pragma alloc_text(PAGE, DiskIoctlVerify)
#pragma alloc_text(PAGE, DiskSetSpecialHacks)
#pragma alloc_text(PAGE, DiskQueryPnpCapabilities)
#pragma alloc_text(PAGE, DiskGetCacheInformation)
#pragma alloc_text(PAGE, DiskSetCacheInformation)
#pragma alloc_text(PAGE, DiskLogCacheInformation)
#pragma alloc_text(PAGE, DiskSetInfoExceptionInformation)
#pragma alloc_text(PAGE, DiskGetInfoExceptionInformation)

#pragma alloc_text(PAGE, DiskPdoFindPartitionEntry)
#pragma alloc_text(PAGE, DiskFindAdjacentPartition)
#pragma alloc_text(PAGE, DiskFindContainingPartition)

#pragma alloc_text(PAGE, DiskIoctlCreateDisk)
#pragma alloc_text(PAGE, DiskIoctlGetDriveLayout)
#pragma alloc_text(PAGE, DiskIoctlGetDriveLayoutEx)
#pragma alloc_text(PAGE, DiskIoctlSetDriveLayout)
#pragma alloc_text(PAGE, DiskIoctlSetDriveLayoutEx)
#pragma alloc_text(PAGE, DiskIoctlGetPartitionInfo)
#pragma alloc_text(PAGE, DiskIoctlGetPartitionInfoEx)
#pragma alloc_text(PAGE, DiskIoctlGetLengthInfo)
#pragma alloc_text(PAGE, DiskIoctlSetPartitionInfo)
#pragma alloc_text(PAGE, DiskIoctlSetPartitionInfoEx)
#pragma alloc_text(PAGE, DiskIoctlGetDriveGeometryEx)
#endif

extern ULONG DiskDisableGpt;

const GUID GUID_NULL = { 0 };
#define DiskCompareGuid(_First,_Second) \
    (memcmp ((_First),(_Second), sizeof (GUID)))

//
// This macro is used to work around a bug in the definition of
// DISK_CACHE_RETENTION_PRIORITY.  The value KeepReadData should be
// assigned 0xf rather than 0x2.  Since the interface was already published
// when this was discovered the disk driver has been modified to translate
// between the interface value and the correct scsi value.
//
// 0x2 is turned into 0xf
// 0xf is turned into 0x2 - this ensures that future SCSI defintions can be
//                          accomodated.
//

#define TRANSLATE_RETENTION_PRIORITY(_x)\
        ((_x) == 0xf ?  0x2 :           \
            ((_x) == 0x2 ? 0xf : _x)    \
        )

#define IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS_ADMIN  CTL_CODE(IOCTL_VOLUME_BASE, 0, METHOD_BUFFERED, FILE_READ_ACCESS)


NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    )

/*++

Routine Description:

    This routine initializes the SCSI hard disk class driver.

Arguments:

    DriverObject - Pointer to driver object created by system.

    RegistryPath - Pointer to the name of the services node for this driver.

Return Value:

    The function value is the final status from the initialization operation.

--*/

{
    CLASS_INIT_DATA InitializationData = { 0 };
    CLASS_QUERY_WMI_REGINFO_EX_LIST classQueryWmiRegInfoExList = { 0 };
    GUID guidQueryRegInfoEx = GUID_CLASSPNP_QUERY_REGINFOEX;

    NTSTATUS status;

#if defined(_X86_) || defined(_AMD64_)

    //
    // Read the information NtDetect squirreled away about the disks in this
    // system.
    //

    DiskSaveDetectInfo(DriverObject);

#endif

    InitializationData.InitializationDataSize = sizeof(CLASS_INIT_DATA);

    //
    // Setup sizes and entry points for functional device objects
    //

    InitializationData.FdoData.DeviceExtensionSize   = FUNCTIONAL_EXTENSION_SIZE;
    InitializationData.FdoData.DeviceType            = FILE_DEVICE_DISK;
    InitializationData.FdoData.DeviceCharacteristics = FILE_DEVICE_SECURE_OPEN;

    InitializationData.FdoData.ClassInitDevice    = DiskInitFdo;
    InitializationData.FdoData.ClassStartDevice   = DiskStartFdo;
    InitializationData.FdoData.ClassStopDevice    = DiskStopDevice;
    InitializationData.FdoData.ClassRemoveDevice  = DiskRemoveDevice;
    InitializationData.FdoData.ClassPowerDevice   = ClassSpinDownPowerHandler;

    InitializationData.FdoData.ClassError         = DiskFdoProcessError;
    InitializationData.FdoData.ClassReadWriteVerification = DiskReadWriteVerification;
    InitializationData.FdoData.ClassDeviceControl = DiskDeviceControl;
    InitializationData.FdoData.ClassShutdownFlush = DiskShutdownFlush;
    InitializationData.FdoData.ClassCreateClose   = NULL;

    //
    // Setup sizes and entry points for physical device objects
    //

    InitializationData.PdoData.DeviceExtensionSize   = PHYSICAL_EXTENSION_SIZE;
    InitializationData.PdoData.DeviceType            = FILE_DEVICE_DISK;
    InitializationData.PdoData.DeviceCharacteristics = FILE_DEVICE_SECURE_OPEN;

    InitializationData.PdoData.ClassInitDevice    = DiskInitPdo;
    InitializationData.PdoData.ClassStartDevice   = DiskStartPdo;
    InitializationData.PdoData.ClassStopDevice    = DiskStopDevice;
    InitializationData.PdoData.ClassRemoveDevice  = DiskRemoveDevice;

    //
    // Use default power routine for PDOs
    //

    InitializationData.PdoData.ClassPowerDevice   = NULL;

    InitializationData.PdoData.ClassError         = NULL;
    InitializationData.PdoData.ClassReadWriteVerification = DiskReadWriteVerification;
    InitializationData.PdoData.ClassDeviceControl = DiskDeviceControl;
    InitializationData.PdoData.ClassShutdownFlush = DiskShutdownFlush;
    InitializationData.PdoData.ClassCreateClose   = NULL;
    InitializationData.PdoData.ClassQueryPnpCapabilities  = DiskQueryPnpCapabilities;

    InitializationData.ClassAddDevice       = DiskAddDevice;
    InitializationData.ClassEnumerateDevice = DiskEnumerateDevice;
    InitializationData.ClassQueryId         = DiskQueryId;

    InitializationData.FdoData.ClassWmiInfo.GuidCount               = 7;
    InitializationData.FdoData.ClassWmiInfo.GuidRegInfo             = DiskWmiFdoGuidList;
    InitializationData.FdoData.ClassWmiInfo.ClassQueryWmiRegInfo    = DiskFdoQueryWmiRegInfo;
    InitializationData.FdoData.ClassWmiInfo.ClassQueryWmiDataBlock  = DiskFdoQueryWmiDataBlock;
    InitializationData.FdoData.ClassWmiInfo.ClassSetWmiDataBlock    = DiskFdoSetWmiDataBlock;
    InitializationData.FdoData.ClassWmiInfo.ClassSetWmiDataItem     = DiskFdoSetWmiDataItem;
    InitializationData.FdoData.ClassWmiInfo.ClassExecuteWmiMethod   = DiskFdoExecuteWmiMethod;
    InitializationData.FdoData.ClassWmiInfo.ClassWmiFunctionControl = DiskWmiFunctionControl;


#if 0
    //
    // Enable this to add WMI support for PDOs
    InitializationData.PdoData.ClassWmiInfo.GuidCount               = 1;
    InitializationData.PdoData.ClassWmiInfo.GuidRegInfo             = DiskWmiPdoGuidList;
    InitializationData.PdoData.ClassWmiInfo.ClassQueryWmiRegInfo    = DiskPdoQueryWmiRegInfo;
    InitializationData.PdoData.ClassWmiInfo.ClassQueryWmiDataBlock  = DiskPdoQueryWmiDataBlock;
    InitializationData.PdoData.ClassWmiInfo.ClassSetWmiDataBlock    = DiskPdoSetWmiDataBlock;
    InitializationData.PdoData.ClassWmiInfo.ClassSetWmiDataItem     = DiskPdoSetWmiDataItem;
    InitializationData.PdoData.ClassWmiInfo.ClassExecuteWmiMethod   = DiskPdoExecuteWmiMethod;
    InitializationData.PdoData.ClassWmiInfo.ClassWmiFunctionControl = DiskWmiFunctionControl;
#endif

    InitializationData.ClassUnload = DiskUnload;

    //
    // Initialize regregistration data structures
    //

    DiskInitializeReregistration();

    //
    // Call the class init routine
    //

    status = ClassInitialize(DriverObject, RegistryPath, &InitializationData);

#if defined(_X86_) || defined(_AMD64_)

    if(NT_SUCCESS(status)) {

        IoRegisterBootDriverReinitialization(DriverObject,
                                             DiskDriverReinitialization,
                                             NULL);
    }

#endif

    //
    // Call class init Ex routine to register a
    // PCLASS_QUERY_WMI_REGINFO_EX routine
    //

    classQueryWmiRegInfoExList.Size = sizeof(CLASS_QUERY_WMI_REGINFO_EX_LIST);
    classQueryWmiRegInfoExList.ClassFdoQueryWmiRegInfoEx = DiskFdoQueryWmiRegInfoEx;

    ClassInitializeEx(DriverObject,
                      &guidQueryRegInfoEx,
                      &classQueryWmiRegInfoExList);

    return status;

} // end DriverEntry()


VOID
DiskUnload(
    IN PDRIVER_OBJECT DriverObject
    )
{
    PAGED_CODE();

#if defined(_X86_) || defined(_AMD64_)
    DiskCleanupDetectInfo(DriverObject);
#endif

    return;
}


NTSTATUS
DiskCreateFdo(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT PhysicalDeviceObject,
    IN PULONG DeviceCount,
    IN BOOLEAN DasdAccessOnly
    )

/*++

Routine Description:

    This routine creates an object for the functional device

Arguments:

    DriverObject - Pointer to driver object created by system.

    PhysicalDeviceObject - Lower level driver we should attach to

    DeviceCount  - Number of previously installed devices.

    DasdAccessOnly - indicates whether or not a file system is allowed to mount
                     on this device object.  Used to avoid double-mounting of
                     file systems on super-floppies (which can unfortunately be
                     fixed disks).  If set the i/o system will only allow rawfs
                     to be mounted.

Return Value:

    NTSTATUS

--*/

{
    PUCHAR deviceName = NULL;
    HANDLE handle = NULL;
    PDEVICE_OBJECT lowerDevice  = NULL;
    PDEVICE_OBJECT deviceObject = NULL;
    PFUNCTIONAL_DEVICE_EXTENSION fdoExtension;
    NTSTATUS status;

    PAGED_CODE();

    *DeviceCount = 0;

    //
    // Set up an object directory to contain the objects for this
    // device and all its partitions.
    //

    do {

        WCHAR dirBuffer[64] = { 0 };
        UNICODE_STRING dirName;
        OBJECT_ATTRIBUTES objectAttribs;

        _snwprintf(dirBuffer, sizeof(dirBuffer) / sizeof(dirBuffer[0]) - 1, L"\\Device\\Harddisk%d", *DeviceCount);

        RtlInitUnicodeString(&dirName, dirBuffer);

        InitializeObjectAttributes(&objectAttribs,
                                   &dirName,
                                   OBJ_CASE_INSENSITIVE | OBJ_PERMANENT,
                                   NULL,
                                   NULL);

        status = ZwCreateDirectoryObject(&handle,
                                         DIRECTORY_ALL_ACCESS,
                                         &objectAttribs);

        (*DeviceCount)++;

    } while((status == STATUS_OBJECT_NAME_COLLISION) ||
            (status == STATUS_OBJECT_NAME_EXISTS));

    if (!NT_SUCCESS(status)) {

        DebugPrint((1, "DiskCreateFdo: Could not create directory - %lx\n",
                    status));

        return(status);
    }

    //
    // When this loop exits the count is inflated by one - fix that.
    //

    (*DeviceCount)--;

    //
    // Claim the device.
    //

    lowerDevice = IoGetAttachedDeviceReference(PhysicalDeviceObject);

    status = ClassClaimDevice(lowerDevice, FALSE);

    if (!NT_SUCCESS(status)) {
        ZwMakeTemporaryObject(handle);
        ZwClose(handle);
        ObDereferenceObject(lowerDevice);
        return status;
    }

    //
    // Create a device object for this device. Each physical disk will
    // have at least one device object. The required device object
    // describes the entire device. Its directory path is
    // \Device\HarddiskN\Partition0, where N = device number.
    //

    status = DiskGenerateDeviceName(TRUE,
                                    *DeviceCount,
                                    0,
                                    NULL,
                                    NULL,
                                    &deviceName);

    if(!NT_SUCCESS(status)) {
        DebugPrint((1, "DiskCreateFdo - couldn't create name %lx\n",
                       status));

        goto DiskCreateFdoExit;

    }

    status = ClassCreateDeviceObject(DriverObject,
                                     deviceName,
                                     PhysicalDeviceObject,
                                     TRUE,
                                     &deviceObject);

    if (!NT_SUCCESS(status)) {

        DebugPrint((1,
                    "DiskCreateFdo: Can not create device object %s\n",
                    deviceName));

        goto DiskCreateFdoExit;
    }

    ExFreePool(deviceName);
    deviceName = NULL;

    //
    // Indicate that IRPs should include MDLs for data transfers.
    //

    SET_FLAG(deviceObject->Flags, DO_DIRECT_IO);

    fdoExtension = deviceObject->DeviceExtension;

    if(DasdAccessOnly) {

        //
        // Inidicate that only RAW should be allowed to mount on the root
        // partition object.  This ensures that a file system can't doubly
        // mount on a super-floppy by mounting once on P0 and once on P1.
        //

        SET_FLAG(deviceObject->Vpb->Flags, VPB_RAW_MOUNT);
    }

    //
    // Initialize lock count to zero. The lock count is used to
    // disable the ejection mechanism on devices that support
    // removable media. Only the lock count in the physical
    // device extension is used.
    //

    fdoExtension->LockCount = 0;

    //
    // Save system disk number.
    //

    fdoExtension->DeviceNumber = *DeviceCount;

    //
    // Set the alignment requirements for the device based on the
    // host adapter requirements
    //

    if (lowerDevice->AlignmentRequirement > deviceObject->AlignmentRequirement) {
        deviceObject->AlignmentRequirement = lowerDevice->AlignmentRequirement;
    }

    //
    // Finally, attach to the pdo
    //

    fdoExtension->LowerPdo = PhysicalDeviceObject;

    fdoExtension->CommonExtension.LowerDeviceObject =
        IoAttachDeviceToDeviceStack(
            deviceObject,
            PhysicalDeviceObject);

    if(fdoExtension->CommonExtension.LowerDeviceObject == NULL) {

        //
        // Uh - oh, we couldn't attach
        // cleanup and return
        //

        status = STATUS_UNSUCCESSFUL;
        goto DiskCreateFdoExit;
    }

    {
        PDISK_DATA diskData = fdoExtension->CommonExtension.DriverData;

        //
        // Initialize the partitioning lock as it may be used in the remove
        // code.
        //

        KeInitializeEvent(&(diskData->PartitioningEvent),
                          SynchronizationEvent,
                          TRUE);
    }


    //
    // Clear the init flag.
    //

    CLEAR_FLAG(deviceObject->Flags, DO_DEVICE_INITIALIZING);

    //
    // Store a handle to the device object directory for this disk
    //

    fdoExtension->DeviceDirectory = handle;

    ObDereferenceObject(lowerDevice);

    return STATUS_SUCCESS;

DiskCreateFdoExit:

    if (deviceObject != NULL)
    {
        IoDeleteDevice(deviceObject);
    }

    if (deviceName != NULL)
    {
        ExFreePool(deviceName);
    }

    ObDereferenceObject(lowerDevice);

    ZwMakeTemporaryObject(handle);
    ZwClose(handle);

    return status;
}


NTSTATUS
DiskReadWriteVerification(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    I/O System entry for read and write requests to SCSI disks.

Arguments:

    DeviceObject - Pointer to driver object created by system.
    Irp - IRP involved.

Return Value:

    NT Status

--*/

{
    PCOMMON_DEVICE_EXTENSION commonExtension = DeviceObject->DeviceExtension;
    PIO_STACK_LOCATION irpSp = IoGetCurrentIrpStackLocation(Irp);
    ULONG residualBytes;
    NTSTATUS status = STATUS_SUCCESS;

    //
    // Make sure that the request is within the bounds of the  partition and
    // that the number of bytes to transfer is a multiple of the sector size
    //

    residualBytes = irpSp->Parameters.Read.Length & (commonExtension->PartitionZeroExtension->DiskGeometry.BytesPerSector - 1);

    if ((irpSp->Parameters.Read.ByteOffset.QuadPart > commonExtension->PartitionLength.QuadPart) ||
        (irpSp->Parameters.Read.ByteOffset.QuadPart < 0) ||
        (residualBytes != 0))
    {
        status = STATUS_INVALID_PARAMETER;
    }
    else
    {
        ULONGLONG bytesRemaining = commonExtension->PartitionLength.QuadPart - irpSp->Parameters.Read.ByteOffset.QuadPart;

        if ((ULONGLONG)irpSp->Parameters.Read.Length > bytesRemaining)
        {
            status = STATUS_INVALID_PARAMETER;
        }
    }

    if (!NT_SUCCESS(status))
    {
        //
        // This error may be caused by the fact that the drive is not ready.
        //

        status = ((PDISK_DATA) commonExtension->DriverData)->ReadyStatus;

        if (!NT_SUCCESS(status)) {

            //
            // Flag this as a user errror so that a popup is generated.
            //

            DebugPrint((1, "DiskReadWriteVerification: ReadyStatus is %lx\n",
                        status));

            IoSetHardErrorOrVerifyDevice(Irp, DeviceObject);

            //
            // status will keep the current error
            //

            ASSERT( status != STATUS_INSUFFICIENT_RESOURCES );

        } else if((commonExtension->IsFdo == TRUE) && (residualBytes == 0)) {

            //
            // This failed because we think the physical disk is too small.
            // Send it down to the drive and let the hardware decide for
            // itself.
            //

            status = STATUS_SUCCESS;

        } else {

            //
            // Note fastfat depends on this parameter to determine when to
            // remount due to a sector size change.
            //

            status = STATUS_INVALID_PARAMETER;
        }
    }

    Irp->IoStatus.Status = status;

    return status;

} // end DiskReadWrite()


NTSTATUS
DiskDetermineMediaTypes(
    IN PDEVICE_OBJECT Fdo,
    IN PIRP     Irp,
    IN UCHAR    MediumType,
    IN UCHAR    DensityCode,
    IN BOOLEAN  MediaPresent,
    IN BOOLEAN  IsWritable
    )

/*++

Routine Description:

    Determines number of types based on the physical device, validates the user buffer
    and builds the MEDIA_TYPE information.

Arguments:

    DeviceObject - Pointer to functional device object created by system.
    Irp - IOCTL_STORAGE_GET_MEDIA_TYPES_EX Irp.
    MediumType - byte returned in mode data header.
    DensityCode - byte returned in mode data block descriptor.
    NumberOfTypes - pointer to be updated based on actual device.

Return Value:

    Status is returned.

--*/

{
    PFUNCTIONAL_DEVICE_EXTENSION fdoExtension = Fdo->DeviceExtension;
    PPHYSICAL_DEVICE_EXTENSION pdoExtension  = Fdo->DeviceExtension;
    PCOMMON_DEVICE_EXTENSION commonExtension = Fdo->DeviceExtension;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);

    PGET_MEDIA_TYPES  mediaTypes = Irp->AssociatedIrp.SystemBuffer;
    PDEVICE_MEDIA_INFO mediaInfo = &mediaTypes->MediaInfo[0];
    BOOLEAN deviceMatched = FALSE;

    PAGED_CODE();

    //
    // this should be checked prior to calling into this routine
    // as we use the buffer as mediaTypes
    //

    ASSERT(irpStack->Parameters.DeviceIoControl.OutputBufferLength >=
           sizeof(GET_MEDIA_TYPES));

    //
    // Determine if this device is removable or fixed.
    //

    if (!TEST_FLAG(Fdo->Characteristics, FILE_REMOVABLE_MEDIA)) {

        //
        // Fixed disk.
        //

        mediaTypes->DeviceType = FILE_DEVICE_DISK;
        mediaTypes->MediaInfoCount = 1;

        mediaInfo->DeviceSpecific.DiskInfo.Cylinders.QuadPart   = fdoExtension->DiskGeometry.Cylinders.QuadPart;
        mediaInfo->DeviceSpecific.DiskInfo.MediaType            = FixedMedia;
        mediaInfo->DeviceSpecific.DiskInfo.TracksPerCylinder    = fdoExtension->DiskGeometry.TracksPerCylinder;
        mediaInfo->DeviceSpecific.DiskInfo.SectorsPerTrack      = fdoExtension->DiskGeometry.SectorsPerTrack;
        mediaInfo->DeviceSpecific.DiskInfo.BytesPerSector       = fdoExtension->DiskGeometry.BytesPerSector;
        mediaInfo->DeviceSpecific.DiskInfo.NumberMediaSides     = 1;
        mediaInfo->DeviceSpecific.DiskInfo.MediaCharacteristics = (MEDIA_CURRENTLY_MOUNTED | MEDIA_READ_WRITE);

        if (!IsWritable) {

            SET_FLAG(mediaInfo->DeviceSpecific.DiskInfo.MediaCharacteristics,
                     MEDIA_WRITE_PROTECTED);
        }

    } else {

        PUCHAR vendorId = (PUCHAR) fdoExtension->DeviceDescriptor + fdoExtension->DeviceDescriptor->VendorIdOffset;
        PUCHAR productId = (PUCHAR) fdoExtension->DeviceDescriptor + fdoExtension->DeviceDescriptor->ProductIdOffset;
        PUCHAR productRevision = (PUCHAR) fdoExtension->DeviceDescriptor + fdoExtension->DeviceDescriptor->ProductRevisionOffset;
        DISK_MEDIA_TYPES_LIST const *mediaListEntry;
        ULONG  currentMedia;
        ULONG  i;
        ULONG  j;
        ULONG  sizeNeeded;

        DebugPrint((1,
                   "DiskDetermineMediaTypes: Vendor %s, Product %s\n",
                   vendorId,
                   productId));

        //
        // Run through the list until we find the entry with a NULL Vendor Id.
        //

        for (i = 0; DiskMediaTypes[i].VendorId != NULL; i++) {

            mediaListEntry = &DiskMediaTypes[i];

            if (strncmp(mediaListEntry->VendorId,vendorId,strlen(mediaListEntry->VendorId))) {
                continue;
            }

            if ((mediaListEntry->ProductId != NULL) &&
                 strncmp(mediaListEntry->ProductId, productId, strlen(mediaListEntry->ProductId))) {
                continue;
            }

            if ((mediaListEntry->Revision != NULL) &&
                 strncmp(mediaListEntry->Revision, productRevision, strlen(mediaListEntry->Revision))) {
                continue;
            }

            deviceMatched = TRUE;

            mediaTypes->DeviceType = FILE_DEVICE_DISK;
            mediaTypes->MediaInfoCount = mediaListEntry->NumberOfTypes;

            //
            // Ensure that buffer is large enough.
            //

            sizeNeeded = FIELD_OFFSET(GET_MEDIA_TYPES, MediaInfo[0]) +
                         (mediaListEntry->NumberOfTypes *
                          sizeof(DEVICE_MEDIA_INFO)
                          );

            if (irpStack->Parameters.DeviceIoControl.OutputBufferLength <
                sizeNeeded) {

                //
                // Buffer too small
                //

                Irp->IoStatus.Status = STATUS_BUFFER_TOO_SMALL;
                return STATUS_BUFFER_TOO_SMALL;
            }

            for (j = 0; j < mediaListEntry->NumberOfTypes; j++) {

                mediaInfo->DeviceSpecific.RemovableDiskInfo.Cylinders.QuadPart = fdoExtension->DiskGeometry.Cylinders.QuadPart;
                mediaInfo->DeviceSpecific.RemovableDiskInfo.TracksPerCylinder = fdoExtension->DiskGeometry.TracksPerCylinder;
                mediaInfo->DeviceSpecific.RemovableDiskInfo.SectorsPerTrack = fdoExtension->DiskGeometry.SectorsPerTrack;
                mediaInfo->DeviceSpecific.RemovableDiskInfo.BytesPerSector = fdoExtension->DiskGeometry.BytesPerSector;
                mediaInfo->DeviceSpecific.RemovableDiskInfo.NumberMediaSides = mediaListEntry->NumberOfSides;

                //
                // Set the type.
                //

                mediaInfo->DeviceSpecific.RemovableDiskInfo.MediaType = mediaListEntry->MediaTypes[j];

                if (mediaInfo->DeviceSpecific.RemovableDiskInfo.MediaType == MO_5_WO) {
                    mediaInfo->DeviceSpecific.RemovableDiskInfo.MediaCharacteristics = MEDIA_WRITE_ONCE;
                } else {
                    mediaInfo->DeviceSpecific.RemovableDiskInfo.MediaCharacteristics = MEDIA_READ_WRITE;
                }

                //
                // Status will either be success, if media is present, or no media.
                // It would be optimal to base from density code and medium type, but not all devices
                // have values for these fields.
                //

                if (MediaPresent) {

                    //
                    // The usage of MediumType and DensityCode is device specific, so this may need
                    // to be extended to further key off of product/vendor ids.
                    // Currently, the MO units are the only devices that return this information.
                    //

                    if (MediumType == 2) {
                        currentMedia = MO_5_WO;
                    } else if (MediumType == 3) {
                        currentMedia = MO_5_RW;

                        if (DensityCode == 0x87) {

                            //
                            // Indicate that the pinnacle 4.6 G media
                            // is present. Other density codes will default to normal
                            // RW MO media.
                            //

                            currentMedia = PINNACLE_APEX_5_RW;
                        }
                    } else {
                        currentMedia = 0;
                    }

                    if (currentMedia) {
                        if (mediaInfo->DeviceSpecific.RemovableDiskInfo.MediaType == (STORAGE_MEDIA_TYPE)currentMedia) {
                            SET_FLAG(mediaInfo->DeviceSpecific.RemovableDiskInfo.MediaCharacteristics, MEDIA_CURRENTLY_MOUNTED);
                        }

                    } else {
                        SET_FLAG(mediaInfo->DeviceSpecific.RemovableDiskInfo.MediaCharacteristics, MEDIA_CURRENTLY_MOUNTED);
                    }
                }

                if (!IsWritable) {
                    SET_FLAG(mediaInfo->DeviceSpecific.RemovableDiskInfo.MediaCharacteristics, MEDIA_WRITE_PROTECTED);
                }

                //
                // Advance to next entry.
                //

                mediaInfo++;
            }
        }

        if (!deviceMatched) {

            DebugPrint((1,
                       "DiskDetermineMediaTypes: Unknown device. Vendor: %s Product: %s Revision: %s\n",
                                   vendorId,
                                   productId,
                                   productRevision));
            //
            // Build an entry for unknown.
            //

            mediaTypes->DeviceType = FILE_DEVICE_DISK;
            mediaTypes->MediaInfoCount = 1;

            mediaInfo->DeviceSpecific.RemovableDiskInfo.Cylinders.QuadPart   = fdoExtension->DiskGeometry.Cylinders.QuadPart;
            mediaInfo->DeviceSpecific.RemovableDiskInfo.MediaType            = RemovableMedia;
            mediaInfo->DeviceSpecific.RemovableDiskInfo.TracksPerCylinder    = fdoExtension->DiskGeometry.TracksPerCylinder;
            mediaInfo->DeviceSpecific.RemovableDiskInfo.SectorsPerTrack      = fdoExtension->DiskGeometry.SectorsPerTrack;
            mediaInfo->DeviceSpecific.RemovableDiskInfo.BytesPerSector       = fdoExtension->DiskGeometry.BytesPerSector;
            mediaInfo->DeviceSpecific.RemovableDiskInfo.NumberMediaSides     = 1;
            mediaInfo->DeviceSpecific.RemovableDiskInfo.MediaCharacteristics = MEDIA_READ_WRITE;

            if (MediaPresent) {

                SET_FLAG(mediaInfo->DeviceSpecific.RemovableDiskInfo.MediaCharacteristics, MEDIA_CURRENTLY_MOUNTED);
            }

            if (!IsWritable) {

                SET_FLAG(mediaInfo->DeviceSpecific.RemovableDiskInfo.MediaCharacteristics, MEDIA_WRITE_PROTECTED);
            }
        }
    }

    Irp->IoStatus.Information =
        FIELD_OFFSET(GET_MEDIA_TYPES, MediaInfo[0]) +
        (mediaTypes->MediaInfoCount * sizeof(DEVICE_MEDIA_INFO));

    return STATUS_SUCCESS;
}


NTSTATUS
DiskDeviceControl(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp
    )

/*++

Routine Description:

    I/O system entry for device controls to SCSI disks.

Arguments:

    Fdo - Pointer to functional device object created by system.
    Irp - IRP involved.

Return Value:

    Status is returned.

--*/

#define SendToFdo(Dev, Irp, Rval)   {                       \
    PCOMMON_DEVICE_EXTENSION ce = Dev->DeviceExtension;     \
    ASSERT_PDO(Dev);                                        \
    IoCopyCurrentIrpStackLocationToNext(Irp);               \
    Rval = IoCallDriver(ce->LowerDeviceObject, Irp);        \
    }

{
    PFUNCTIONAL_DEVICE_EXTENSION fdoExtension = DeviceObject->DeviceExtension;
    PPHYSICAL_DEVICE_EXTENSION pdoExtension   = DeviceObject->DeviceExtension;
    PCOMMON_DEVICE_EXTENSION commonExtension  = DeviceObject->DeviceExtension;

    PIO_STACK_LOCATION     irpStack = IoGetCurrentIrpStackLocation(Irp);
    PDISK_DATA             diskData = (PDISK_DATA)(commonExtension->DriverData);
    PSCSI_REQUEST_BLOCK    srb;
    PCDB                   cdb;
    PMODE_PARAMETER_HEADER modeData;
    PIRP                   irp2;
    ULONG                  length;
    NTSTATUS               status = STATUS_SUCCESS;
    KEVENT                 event;
    IO_STATUS_BLOCK        ioStatus = { 0 };

    BOOLEAN                b = FALSE;

    srb = ExAllocatePoolWithTag(NonPagedPool,
                                SCSI_REQUEST_BLOCK_SIZE,
                                DISK_TAG_SRB);
    Irp->IoStatus.Information = 0;

    if (srb == NULL) {

        Irp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
        ClassReleaseRemoveLock(DeviceObject, Irp);
        ClassCompleteRequest(DeviceObject, Irp, IO_NO_INCREMENT);
        return(STATUS_INSUFFICIENT_RESOURCES);
    }

    RtlZeroMemory(srb, SCSI_REQUEST_BLOCK_SIZE);

    cdb = (PCDB)srb->Cdb;

    switch (irpStack->Parameters.DeviceIoControl.IoControlCode) {

    case IOCTL_DISK_GET_CACHE_INFORMATION:
        b = TRUE;
    case IOCTL_DISK_SET_CACHE_INFORMATION: {

        BOOLEAN getCaching = b;
        PDISK_CACHE_INFORMATION cacheInfo = Irp->AssociatedIrp.SystemBuffer;

        if(!commonExtension->IsFdo) {

            ClassReleaseRemoveLock(DeviceObject, Irp);
            ExFreePool(srb);
            SendToFdo(DeviceObject, Irp, status);
            return status;
        }

        //
        // Validate the request.
        //

        if((getCaching) &&
           (irpStack->Parameters.DeviceIoControl.OutputBufferLength <
            sizeof(DISK_CACHE_INFORMATION))
           ) {
            status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

        if ((!getCaching) &&
            (irpStack->Parameters.DeviceIoControl.InputBufferLength <
             sizeof(DISK_CACHE_INFORMATION))
           ) {

            status = STATUS_INFO_LENGTH_MISMATCH;
            break;
        }

        ASSERT(Irp->AssociatedIrp.SystemBuffer != NULL);

        if (getCaching) {

            status = DiskGetCacheInformation(fdoExtension, cacheInfo);

            if (NT_SUCCESS(status)) {
                Irp->IoStatus.Information = sizeof(DISK_CACHE_INFORMATION);
            }

        } else {

            status = DiskSetCacheInformation(fdoExtension, cacheInfo);

            //
            // Save away the user-defined override in our extension and the registry
            //
            if (cacheInfo->WriteCacheEnabled)
            {
                diskData->WriteCacheOverride = DiskWriteCacheEnable;
            }
            else
            {
                diskData->WriteCacheOverride = DiskWriteCacheDisable;
            }

            ClassSetDeviceParameter(fdoExtension, DiskDeviceParameterSubkey, DiskDeviceUserWriteCacheSetting, diskData->WriteCacheOverride);

            DiskLogCacheInformation(fdoExtension, cacheInfo, status);
        }

        break;
    }

    case IOCTL_DISK_GET_CACHE_SETTING: {

        if (!commonExtension->IsFdo) {

            ClassReleaseRemoveLock(DeviceObject, Irp);
            ExFreePool(srb);
            SendToFdo(DeviceObject, Irp, status);
            return status;
        }

        status = DiskIoctlGetCacheSetting(fdoExtension, Irp);
        break;
    }

    case IOCTL_DISK_SET_CACHE_SETTING: {

        if (!commonExtension->IsFdo)
        {
            ClassReleaseRemoveLock(DeviceObject, Irp);
            ExFreePool(srb);
            SendToFdo(DeviceObject, Irp, status);
            return status;
        }

        status = DiskIoctlSetCacheSetting(fdoExtension, Irp);
        break;
    }

    case SMART_GET_VERSION: {

        PUCHAR buffer;
        PSRB_IO_CONTROL  srbControl;
        PGETVERSIONINPARAMS versionParams;

        if(!commonExtension->IsFdo) {
            ClassReleaseRemoveLock(DeviceObject, Irp);
            ExFreePool(srb);
            SendToFdo(DeviceObject, Irp, status);
            return status;
        }

        if (irpStack->Parameters.DeviceIoControl.OutputBufferLength <
            sizeof(GETVERSIONINPARAMS)) {
                status = STATUS_BUFFER_TOO_SMALL;
                break;
        }

        srbControl = ExAllocatePoolWithTag(NonPagedPool,
                                           sizeof(SRB_IO_CONTROL) +
                                           sizeof(GETVERSIONINPARAMS),
                                           DISK_TAG_SMART);

        if (!srbControl) {
            status =  STATUS_INSUFFICIENT_RESOURCES;
            break;
        }

        RtlZeroMemory(srbControl,
                      sizeof(SRB_IO_CONTROL) + sizeof(GETVERSIONINPARAMS)
                      );

        //
        // fill in srbControl fields
        //

        srbControl->HeaderLength = sizeof(SRB_IO_CONTROL);
        RtlMoveMemory (srbControl->Signature, "SCSIDISK", 8);
        srbControl->Timeout = fdoExtension->TimeOutValue;
        srbControl->Length = sizeof(GETVERSIONINPARAMS);
        srbControl->ControlCode = IOCTL_SCSI_MINIPORT_SMART_VERSION;

        //
        // Point to the 'buffer' portion of the SRB_CONTROL
        //

        buffer = (PUCHAR)srbControl;
        (ULONG_PTR)buffer += srbControl->HeaderLength;

        //
        // Ensure correct target is set in the cmd parameters.
        //

        versionParams = (PGETVERSIONINPARAMS)buffer;
        versionParams->bIDEDeviceMap = diskData->ScsiAddress.TargetId;

        ClassSendDeviceIoControlSynchronous(
            IOCTL_SCSI_MINIPORT,
            commonExtension->LowerDeviceObject,
            srbControl,
            sizeof(SRB_IO_CONTROL) + sizeof(GETVERSIONINPARAMS),
            sizeof(SRB_IO_CONTROL) + sizeof(GETVERSIONINPARAMS),
            FALSE,
            &ioStatus);

        status = ioStatus.Status;

        //
        // If successful, copy the data received into the output buffer.
        // This should only fail in the event that the IDE driver is older
        // than this driver.
        //

        if (NT_SUCCESS(status)) {

            buffer = (PUCHAR)srbControl;
            (ULONG_PTR)buffer += srbControl->HeaderLength;

            RtlMoveMemory (Irp->AssociatedIrp.SystemBuffer, buffer,
                           sizeof(GETVERSIONINPARAMS));
            Irp->IoStatus.Information = sizeof(GETVERSIONINPARAMS);
        }

        ExFreePool(srbControl);
        break;
    }

    case SMART_RCV_DRIVE_DATA: {

        PSENDCMDINPARAMS cmdInParameters = ((PSENDCMDINPARAMS)Irp->AssociatedIrp.SystemBuffer);
        ULONG            controlCode = 0;
        PSRB_IO_CONTROL  srbControl;
        PUCHAR           buffer;

        if(!commonExtension->IsFdo) {
            ClassReleaseRemoveLock(DeviceObject, Irp);
            ExFreePool(srb);
            SendToFdo(DeviceObject, Irp, status);
            return status;
        }

        if (irpStack->Parameters.DeviceIoControl.InputBufferLength <
            (sizeof(SENDCMDINPARAMS) - 1)) {
                status = STATUS_INVALID_PARAMETER;
                break;

        } else if (irpStack->Parameters.DeviceIoControl.OutputBufferLength <
            (sizeof(SENDCMDOUTPARAMS) + 512 - 1)) {
                status = STATUS_BUFFER_TOO_SMALL;
                break;
        }

        //
        // Create notification event object to be used to signal the
        // request completion.
        //

        KeInitializeEvent(&event, NotificationEvent, FALSE);

        //
        // use controlCode as a sort of 'STATUS_SUCCESS' to see if it's
        // a valid request type
        //

        status = STATUS_INVALID_PARAMETER;

        if (cmdInParameters->irDriveRegs.bCommandReg == ID_CMD) {

            length = IDENTIFY_BUFFER_SIZE + sizeof(SENDCMDOUTPARAMS);
            controlCode = IOCTL_SCSI_MINIPORT_IDENTIFY;

        } else if (cmdInParameters->irDriveRegs.bCommandReg == SMART_CMD) {

            switch (cmdInParameters->irDriveRegs.bFeaturesReg) {

                case READ_ATTRIBUTES:
                    controlCode = IOCTL_SCSI_MINIPORT_READ_SMART_ATTRIBS;
                    length = READ_ATTRIBUTE_BUFFER_SIZE + sizeof(SENDCMDOUTPARAMS);
                    break;

                case READ_THRESHOLDS:
                    controlCode = IOCTL_SCSI_MINIPORT_READ_SMART_THRESHOLDS;
                    length = READ_THRESHOLD_BUFFER_SIZE + sizeof(SENDCMDOUTPARAMS);
                    break;

                case SMART_READ_LOG:
                {
                    if (diskData->FailurePredictionCapability != FailurePredictionSmart)
                    {
                        status = STATUS_INVALID_DEVICE_REQUEST;
                        break;
                    }

                    //
                    // Calculate additional length based on number of sectors to be read.
                    // Then verify the output buffer is large enough.
                    //

                    length = cmdInParameters->irDriveRegs.bSectorCountReg * SMART_LOG_SECTOR_SIZE;
                    length += max(sizeof(SENDCMDOUTPARAMS), sizeof(SENDCMDINPARAMS));

                    if (irpStack->Parameters.DeviceIoControl.OutputBufferLength < length - 1)
                    {
                        status = STATUS_BUFFER_TOO_SMALL;
                        break;
                    }

                    controlCode = IOCTL_SCSI_MINIPORT_READ_SMART_LOG;
                    break;
                }

            }
        }

        if (controlCode == 0) {

            break;
        }

        srbControl = ExAllocatePoolWithTag(NonPagedPool,
                                           sizeof(SRB_IO_CONTROL) + length,
                                           DISK_TAG_SMART);

        if (!srbControl) {
            status =  STATUS_INSUFFICIENT_RESOURCES;
            break;
        }

        //
        // fill in srbControl fields
        //

        srbControl->HeaderLength = sizeof(SRB_IO_CONTROL);
        RtlMoveMemory (srbControl->Signature, "SCSIDISK", 8);
        srbControl->Timeout = fdoExtension->TimeOutValue;
        srbControl->Length = length;
        srbControl->ControlCode = controlCode;

        //
        // Point to the 'buffer' portion of the SRB_CONTROL
        //

        buffer = (PUCHAR)srbControl;
        (ULONG_PTR)buffer += srbControl->HeaderLength;

        //
        // Ensure correct target is set in the cmd parameters.
        //

        cmdInParameters->bDriveNumber = diskData->ScsiAddress.TargetId;

        //
        // Copy the IOCTL parameters to the srb control buffer area.
        //

        RtlMoveMemory(buffer,
                      Irp->AssociatedIrp.SystemBuffer,
                      sizeof(SENDCMDINPARAMS) - 1);

        irp2 = IoBuildDeviceIoControlRequest(IOCTL_SCSI_MINIPORT,
                                            commonExtension->LowerDeviceObject,
                                            srbControl,
                                            sizeof(SRB_IO_CONTROL) + sizeof(SENDCMDINPARAMS) - 1,
                                            srbControl,
                                            sizeof(SRB_IO_CONTROL) + length,
                                            FALSE,
                                            &event,
                                            &ioStatus);

        if (irp2 == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            ExFreePool(srbControl);
            break;
        }

        //
        // Call the port driver with the request and wait for it to complete.
        //

        status = IoCallDriver(commonExtension->LowerDeviceObject, irp2);

        if (status == STATUS_PENDING) {
            KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, NULL);
            status = ioStatus.Status;
        }

        //
        // Copy the data received into the output buffer. Since the status buffer
        // contains error information also, always perform this copy. IO will will
        // either pass this back to the app, or zero it, in case of error.
        //

        buffer = (PUCHAR)srbControl;
        (ULONG_PTR)buffer += srbControl->HeaderLength;

        if (NT_SUCCESS(status)) {

            RtlMoveMemory ( Irp->AssociatedIrp.SystemBuffer, buffer, length - 1);
            Irp->IoStatus.Information = length - 1;

        } else {

            RtlMoveMemory ( Irp->AssociatedIrp.SystemBuffer, buffer, (sizeof(SENDCMDOUTPARAMS) - 1));
            Irp->IoStatus.Information = sizeof(SENDCMDOUTPARAMS) - 1;

        }

        ExFreePool(srbControl);
        break;

    }

    case SMART_SEND_DRIVE_COMMAND: {

        PSENDCMDINPARAMS cmdInParameters = ((PSENDCMDINPARAMS)Irp->AssociatedIrp.SystemBuffer);
        PSRB_IO_CONTROL  srbControl;
        ULONG            controlCode = 0;
        PUCHAR           buffer;

        if(!commonExtension->IsFdo) {
            ClassReleaseRemoveLock(DeviceObject, Irp);
            ExFreePool(srb);
            SendToFdo(DeviceObject, Irp, status);
            return status;
        }

        if (irpStack->Parameters.DeviceIoControl.InputBufferLength <
               (sizeof(SENDCMDINPARAMS) - 1)) {
                status = STATUS_INVALID_PARAMETER;
                break;

        } else if (irpStack->Parameters.DeviceIoControl.OutputBufferLength <
                      (sizeof(SENDCMDOUTPARAMS) - 1)) {
                status = STATUS_BUFFER_TOO_SMALL;
                break;
        }

        //
        // Create notification event object to be used to signal the
        // request completion.
        //

        KeInitializeEvent(&event, NotificationEvent, FALSE);

        length = 0;

        if (cmdInParameters->irDriveRegs.bCommandReg == SMART_CMD) {

            switch (cmdInParameters->irDriveRegs.bFeaturesReg) {

                case SMART_WRITE_LOG:
                {
                    if (diskData->FailurePredictionCapability != FailurePredictionSmart)
                    {
                        status = STATUS_INVALID_DEVICE_REQUEST;
                        break;
                    }

                    //
                    // Calculate additional length based on number of sectors to be written.
                    // Then verify the input buffer is large enough.
                    //

                    length = cmdInParameters->irDriveRegs.bSectorCountReg * SMART_LOG_SECTOR_SIZE;

                    if (irpStack->Parameters.DeviceIoControl.InputBufferLength <
                            (sizeof(SENDCMDINPARAMS) - 1) + length)
                    {
                        status = STATUS_BUFFER_TOO_SMALL;
                        break;
                    }

                    controlCode = IOCTL_SCSI_MINIPORT_WRITE_SMART_LOG;
                    break;
                }

                case ENABLE_SMART:
                    controlCode = IOCTL_SCSI_MINIPORT_ENABLE_SMART;
                    break;

                case DISABLE_SMART:
                    controlCode = IOCTL_SCSI_MINIPORT_DISABLE_SMART;
                    break;

                case  RETURN_SMART_STATUS:

                    //
                    // Ensure bBuffer is at least 2 bytes (to hold the values of
                    // cylinderLow and cylinderHigh).
                    //

                    if (irpStack->Parameters.DeviceIoControl.OutputBufferLength <
                        (sizeof(SENDCMDOUTPARAMS) - 1 + sizeof(IDEREGS))) {

                        /*
                         *  Don't set controlCode; that'll cause us to break out below.
                         */
                        status = STATUS_BUFFER_TOO_SMALL;
                        break;
                    }

                    controlCode = IOCTL_SCSI_MINIPORT_RETURN_STATUS;
                    length = sizeof(IDEREGS);
                    break;

                case ENABLE_DISABLE_AUTOSAVE:
                    controlCode = IOCTL_SCSI_MINIPORT_ENABLE_DISABLE_AUTOSAVE;
                    break;

                case SAVE_ATTRIBUTE_VALUES:
                    controlCode = IOCTL_SCSI_MINIPORT_SAVE_ATTRIBUTE_VALUES;
                    break;

                case EXECUTE_OFFLINE_DIAGS:
                    //
                    // Validate that this is an ok self test command
                    //
                    if (DiskIsValidSmartSelfTest(cmdInParameters->irDriveRegs.bSectorNumberReg))
                    {
                        controlCode = IOCTL_SCSI_MINIPORT_EXECUTE_OFFLINE_DIAGS;
                    }
                    break;

                case ENABLE_DISABLE_AUTO_OFFLINE:
                    controlCode = IOCTL_SCSI_MINIPORT_ENABLE_DISABLE_AUTO_OFFLINE;
                    break;

                default:
                    status = STATUS_INVALID_PARAMETER;
                    break;
            }
        }
        else {
            status = STATUS_INVALID_PARAMETER;
        }

        if (controlCode == 0) {
            break;
        }

        length += max(sizeof(SENDCMDOUTPARAMS), sizeof(SENDCMDINPARAMS));
        srbControl = ExAllocatePoolWithTag(NonPagedPool,
                                           sizeof(SRB_IO_CONTROL) + length,
                                           DISK_TAG_SMART);

        if (!srbControl) {
            status =  STATUS_INSUFFICIENT_RESOURCES;
            break;
        }

        //
        // fill in srbControl fields
        //

        srbControl->HeaderLength = sizeof(SRB_IO_CONTROL);
        RtlMoveMemory (srbControl->Signature, "SCSIDISK", 8);
        srbControl->Timeout = fdoExtension->TimeOutValue;
        srbControl->Length = length;
        srbControl->ControlCode = controlCode;

        //
        // Point to the 'buffer' portion of the SRB_CONTROL
        //

        buffer = (PUCHAR)srbControl;
        (ULONG_PTR)buffer += srbControl->HeaderLength;

        //
        // Ensure correct target is set in the cmd parameters.
        //

        cmdInParameters->bDriveNumber = diskData->ScsiAddress.TargetId;

        //
        // Copy the IOCTL parameters to the srb control buffer area.
        //

        RtlMoveMemory(buffer, Irp->AssociatedIrp.SystemBuffer, sizeof(SENDCMDINPARAMS) - 1);

        irp2 = IoBuildDeviceIoControlRequest(IOCTL_SCSI_MINIPORT,
                                            commonExtension->LowerDeviceObject,
                                            srbControl,
                                            sizeof(SRB_IO_CONTROL) + sizeof(SENDCMDINPARAMS) - 1,
                                            srbControl,
                                            sizeof(SRB_IO_CONTROL) + length,
                                            FALSE,
                                            &event,
                                            &ioStatus);

        if (irp2 == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            ExFreePool(srbControl);
            break;
        }

        //
        // Call the port driver with the request and wait for it to complete.
        //

        status = IoCallDriver(commonExtension->LowerDeviceObject, irp2);

        if (status == STATUS_PENDING) {
            KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, NULL);
            status = ioStatus.Status;
        }

        //
        // Copy the data received into the output buffer. Since the status buffer
        // contains error information also, always perform this copy. IO will will
        // either pass this back to the app, or zero it, in case of error.
        //

        buffer = (PUCHAR)srbControl;
        (ULONG_PTR)buffer += srbControl->HeaderLength;

        //
        // Update the return buffer size based on the sub-command.
        //

        if (cmdInParameters->irDriveRegs.bFeaturesReg == RETURN_SMART_STATUS) {
            length = sizeof(SENDCMDOUTPARAMS) - 1 + sizeof(IDEREGS);
        } else {
            length = sizeof(SENDCMDOUTPARAMS) - 1;
        }

        RtlMoveMemory ( Irp->AssociatedIrp.SystemBuffer, buffer, length);
        Irp->IoStatus.Information = length;

        ExFreePool(srbControl);
        break;

    }

    case IOCTL_STORAGE_GET_MEDIA_TYPES_EX: {

        PMODE_PARAMETER_BLOCK blockDescriptor;
        ULONG modeLength;
        ULONG retries = 4;
        UCHAR densityCode = 0;
        BOOLEAN writable = TRUE;
        BOOLEAN mediaPresent = FALSE;

        DebugPrint((3,
                   "Disk.DiskDeviceControl: GetMediaTypes\n"));

        if (irpStack->Parameters.DeviceIoControl.OutputBufferLength <
            sizeof(GET_MEDIA_TYPES)) {
            status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

        if(!commonExtension->IsFdo) {
            ClassReleaseRemoveLock(DeviceObject, Irp);
            ExFreePool(srb);
            SendToFdo(DeviceObject, Irp, status);
            return status;
        }

        //
        // Send a TUR to determine if media is present.
        //

        srb->CdbLength = 6;
        cdb = (PCDB)srb->Cdb;
        cdb->CDB6GENERIC.OperationCode = SCSIOP_TEST_UNIT_READY;

        //
        // Set timeout value.
        //

        srb->TimeOutValue = fdoExtension->TimeOutValue;

        status = ClassSendSrbSynchronous(DeviceObject,
                                         srb,
                                         NULL,
                                         0,
                                         FALSE);


        if (NT_SUCCESS(status)) {
            mediaPresent = TRUE;
        }

        RtlZeroMemory(srb, SCSI_REQUEST_BLOCK_SIZE);

        modeLength = MODE_DATA_SIZE;
        modeData = ExAllocatePoolWithTag(NonPagedPoolCacheAligned,
                                         modeLength,
                                         DISK_TAG_MODE_DATA);

        if (modeData == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }

        RtlZeroMemory(modeData, modeLength);

        //
        // Build the MODE SENSE CDB.
        //

        srb->CdbLength = 6;
        cdb = (PCDB)srb->Cdb;

        //
        // Set timeout value from device extension.
        //

        srb->TimeOutValue = fdoExtension->TimeOutValue;

        //
        // Page code of 0x3F will return all pages.
        //

        cdb->MODE_SENSE.OperationCode = SCSIOP_MODE_SENSE;
        cdb->MODE_SENSE.PageCode         = MODE_SENSE_RETURN_ALL;
        cdb->MODE_SENSE.AllocationLength = (UCHAR)modeLength;

Retry:
        status = ClassSendSrbSynchronous(DeviceObject,
                                         srb,
                                         modeData,
                                         modeLength,
                                         FALSE);


        if (status == STATUS_VERIFY_REQUIRED) {

            if (retries--) {

                //
                // Retry request.
                //

                goto Retry;
            }
        } else if (SRB_STATUS(srb->SrbStatus) == SRB_STATUS_DATA_OVERRUN) {
            status = STATUS_SUCCESS;
        }

        if (NT_SUCCESS(status) || (status == STATUS_NO_MEDIA_IN_DEVICE)) {

            //
            // Get the block descriptor.
            //

            if (modeData->BlockDescriptorLength != 0)
            {
                blockDescriptor = (PMODE_PARAMETER_BLOCK)modeData;
                (ULONG_PTR)blockDescriptor += sizeof(MODE_PARAMETER_HEADER);

                densityCode = blockDescriptor->DensityCode;
            }

            if (TEST_FLAG(modeData->DeviceSpecificParameter,
                          MODE_DSP_WRITE_PROTECT)) {

                writable = FALSE;
            }

            status = DiskDetermineMediaTypes(DeviceObject,
                                             Irp,
                                             modeData->MediumType,
                                             densityCode,
                                             mediaPresent,
                                             writable);

            //
            // If the buffer was too small, DetermineMediaTypes updated the status and information and the request will fail.
            //

        } else {
            DebugPrint((1,
                       "DiskDeviceControl: Mode sense for header/bd failed. %lx\n",
                       status));
        }

        ExFreePool(modeData);
        break;
    }

    case IOCTL_DISK_GET_DRIVE_GEOMETRY: {

        DebugPrint((2, "IOCTL_DISK_GET_DRIVE_GEOMETRY to device %p through irp %p\n",
                    DeviceObject, Irp));
        DebugPrint((2, "Device is a%s.\n",
                    commonExtension->IsFdo ? "n fdo" : " pdo"));

        if (irpStack->Parameters.DeviceIoControl.OutputBufferLength <
            sizeof(DISK_GEOMETRY)) {

            status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

        if(!commonExtension->IsFdo) {

            //
            // Pdo should issue this request to the lower device object
            //

            ClassReleaseRemoveLock(DeviceObject, Irp);
            ExFreePool(srb);
            SendToFdo(DeviceObject, Irp, status);
            return status;
        }

        if (TEST_FLAG(DeviceObject->Characteristics, FILE_REMOVABLE_MEDIA)) {

            //
            // Issue ReadCapacity to update device extension
            // with information for current media.
            //

            status = DiskReadDriveCapacity(
                        commonExtension->PartitionZeroExtension->DeviceObject);

            //
            // Note whether the drive is ready.
            //

            diskData->ReadyStatus = status;

            if (!NT_SUCCESS(status)) {
                break;
            }
        }

        //
        // Copy drive geometry information from device extension.
        //

        RtlMoveMemory(Irp->AssociatedIrp.SystemBuffer,
                      &(fdoExtension->DiskGeometry),
                      sizeof(DISK_GEOMETRY));

        status = STATUS_SUCCESS;
        Irp->IoStatus.Information = sizeof(DISK_GEOMETRY);
        break;
    }

    case IOCTL_DISK_GET_DRIVE_GEOMETRY_EX: {
        DebugPrint((1, "IOCTL_DISK_GET_DRIVE_GEOMETRY_EX to device %p through irp %p\n",
                DeviceObject, Irp));
        DebugPrint((1, "Device Is a%s.\n",
                commonExtension->IsFdo ? "n fdo" : " pdo"));


        if (!commonExtension->IsFdo) {

            //
            // Pdo should issue this request to the lower device object
            //

            ClassReleaseRemoveLock (DeviceObject, Irp);
            ExFreePool (srb);
            SendToFdo (DeviceObject, Irp, status);
            return status;

        } else {

            status = DiskIoctlGetDriveGeometryEx( DeviceObject, Irp );
        }

        break;
    }

    case IOCTL_STORAGE_PREDICT_FAILURE : {

        PSTORAGE_PREDICT_FAILURE checkFailure;
        STORAGE_FAILURE_PREDICT_STATUS diskSmartStatus;

        DebugPrint((2, "IOCTL_STORAGE_PREDICT_FAILURE to device %p through irp %p\n",
                    DeviceObject, Irp));
        DebugPrint((2, "Device is a%s.\n",
                    commonExtension->IsFdo ? "n fdo" : " pdo"));

        checkFailure = (PSTORAGE_PREDICT_FAILURE)Irp->AssociatedIrp.SystemBuffer;

        if (irpStack->Parameters.DeviceIoControl.OutputBufferLength <
            sizeof(STORAGE_PREDICT_FAILURE)) {

            status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

        if(!commonExtension->IsFdo) {

            //
            // Pdo should issue this request to the lower device object
            //

            ClassReleaseRemoveLock(DeviceObject, Irp);
            ExFreePool(srb);
            SendToFdo(DeviceObject, Irp, status);
            return status;
        }

        //
        // See if the disk is predicting failure
        //

        if (diskData->FailurePredictionCapability == FailurePredictionSense) {
            ULONG readBufferSize;
            PUCHAR readBuffer;
            PIRP readIrp;
            PDEVICE_OBJECT topOfStack;

            checkFailure->PredictFailure = 0;

            KeInitializeEvent(&event, SynchronizationEvent, FALSE);

            topOfStack = IoGetAttachedDeviceReference(DeviceObject);

            //
            // SCSI disks need to have a read sent down to provoke any
            // failures to be reported.
            //
            // Issue a normal read operation.  The error-handling code in
            // classpnp will take care of a failure prediction by logging the
            // correct event.
            //

            readBufferSize = fdoExtension->DiskGeometry.BytesPerSector;
            readBuffer = ExAllocatePoolWithTag(NonPagedPool,
                                               readBufferSize,
                                               DISK_TAG_SMART);

            if (readBuffer != NULL) {
                LARGE_INTEGER offset;

                offset.QuadPart = 0;
                readIrp = IoBuildSynchronousFsdRequest(
                        IRP_MJ_READ,
                        topOfStack,
                        readBuffer,
                        readBufferSize,
                        &offset,
                        &event,
                        &ioStatus);


                if (readIrp != NULL) {
                    status = IoCallDriver(topOfStack, readIrp);
                    if (status == STATUS_PENDING) {
                        KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, NULL);
                        status = ioStatus.Status;
                    }
                }
                else
                {
                    status = STATUS_INSUFFICIENT_RESOURCES;
                }

                ExFreePool(readBuffer);
            }
            else
            {
                status = STATUS_INSUFFICIENT_RESOURCES;
            }

            ObDereferenceObject(topOfStack);
        }

        if ((diskData->FailurePredictionCapability == FailurePredictionSmart) ||
            (diskData->FailurePredictionCapability == FailurePredictionSense))
        {
            status = DiskReadFailurePredictStatus(fdoExtension,
                                                  &diskSmartStatus);

            if (NT_SUCCESS(status))
            {
                status = DiskReadFailurePredictData(fdoExtension,
                                           Irp->AssociatedIrp.SystemBuffer);

                if (diskSmartStatus.PredictFailure)
                {
                    checkFailure->PredictFailure = 1;
                } else {
                    checkFailure->PredictFailure = 0;
                }

                Irp->IoStatus.Information = sizeof(STORAGE_PREDICT_FAILURE);
            }
        } else {
            status = STATUS_INVALID_DEVICE_REQUEST;
        }

        break;
    }

    case IOCTL_DISK_VERIFY: {

        PVERIFY_INFORMATION verifyInfo = Irp->AssociatedIrp.SystemBuffer;
        LARGE_INTEGER byteOffset;

        DebugPrint((2, "IOCTL_DISK_VERIFY to device %p through irp %p\n",
                    DeviceObject, Irp));
        DebugPrint((2, "Device is a%s.\n",
                    commonExtension->IsFdo ? "n fdo" : " pdo"));

        //
        // Validate buffer length.
        //

        if (irpStack->Parameters.DeviceIoControl.InputBufferLength <
            sizeof(VERIFY_INFORMATION)) {

            status = STATUS_INFO_LENGTH_MISMATCH;
            break;
        }

        //
        // Add disk offset to starting sector.
        //

        byteOffset.QuadPart = commonExtension->StartingOffset.QuadPart +
                              verifyInfo->StartingOffset.QuadPart;

        if(!commonExtension->IsFdo) {

            //
            // Adjust the request and forward it down
            //

            verifyInfo->StartingOffset.QuadPart = byteOffset.QuadPart;

            ClassReleaseRemoveLock(DeviceObject, Irp);
            SendToFdo(DeviceObject, Irp, status);
            ExFreePool(srb);
            return status;
        }

        //
        // Perform a bounds check on the sector range
        //

        if ((verifyInfo->StartingOffset.QuadPart > commonExtension->PartitionLength.QuadPart) ||
            (verifyInfo->StartingOffset.QuadPart < 0))
        {
            status = STATUS_NONEXISTENT_SECTOR;
            break;
        }
        else
        {
            ULONGLONG bytesRemaining = commonExtension->PartitionLength.QuadPart - verifyInfo->StartingOffset.QuadPart;

            if ((ULONGLONG)verifyInfo->Length > bytesRemaining)
            {
                status = STATUS_NONEXISTENT_SECTOR;
                break;
            }
        }

        {
            PDISK_VERIFY_WORKITEM_CONTEXT Context = NULL;

            Context = ExAllocatePoolWithTag(NonPagedPool,
                                            sizeof(DISK_VERIFY_WORKITEM_CONTEXT),
                                            DISK_TAG_WI_CONTEXT);

            if (Context)
            {
                Context->Irp = Irp;
                Context->Srb = srb;
                Context->WorkItem = IoAllocateWorkItem(DeviceObject);

                if (Context->WorkItem)
                {
                    IoMarkIrpPending(Irp);

                    IoQueueWorkItem(Context->WorkItem,
                                    DiskIoctlVerify,
                                    DelayedWorkQueue,
                                    Context);

                    return STATUS_PENDING;
                }

                ExFreePool(Context);
            }

            status = STATUS_INSUFFICIENT_RESOURCES;
        }

        break;
    }

    case IOCTL_DISK_CREATE_DISK: {

        if (!commonExtension->IsFdo) {
            ClassReleaseRemoveLock(DeviceObject, Irp);
            ExFreePool(srb);
            SendToFdo(DeviceObject, Irp, status);
            return status;
        }

        status = DiskIoctlCreateDisk (
                        DeviceObject,
                        Irp
                        );

        //
        // Notify everyone that the disk layout has changed
        //
        if (NT_SUCCESS(status))
        {
            TARGET_DEVICE_CUSTOM_NOTIFICATION Notification = {0};

            Notification.Event   = GUID_IO_DISK_LAYOUT_CHANGE;
            Notification.Version = 1;
            Notification.Size    = (USHORT)FIELD_OFFSET(TARGET_DEVICE_CUSTOM_NOTIFICATION, CustomDataBuffer);
            Notification.FileObject = NULL;
            Notification.NameBufferOffset = -1;

            IoReportTargetDeviceChangeAsynchronous(fdoExtension->LowerPdo,
                                                   &Notification,
                                                   NULL,
                                                   NULL);
        }

        break;
    }

    case IOCTL_DISK_GET_DRIVE_LAYOUT: {

        DebugPrint((1, "IOCTL_DISK_GET_DRIVE_LAYOUT to device %p through irp %p\n",
                    DeviceObject, Irp));
        DebugPrint((1, "Device is a%s.\n",
                    commonExtension->IsFdo ? "n fdo" : " pdo"));

        if (!commonExtension->IsFdo) {
            ClassReleaseRemoveLock(DeviceObject, Irp);
            ExFreePool(srb);
            SendToFdo(DeviceObject, Irp, status);
            return status;
        }

        status = DiskIoctlGetDriveLayout(
                        DeviceObject,
                        Irp);
        break;
    }

    case IOCTL_DISK_GET_DRIVE_LAYOUT_EX: {

        DebugPrint((1, "IOCTL_DISK_GET_DRIVE_LAYOUT_EX to device %p through irp %p\n",
                    DeviceObject, Irp));
        DebugPrint((1, "Device is a%s.\n",
                    commonExtension->IsFdo ? "n fdo" : " pdo"));

        if (!commonExtension->IsFdo) {
            ClassReleaseRemoveLock(DeviceObject, Irp);
            ExFreePool(srb);
            SendToFdo(DeviceObject, Irp, status);
            return status;
        }

        status = DiskIoctlGetDriveLayoutEx(
                        DeviceObject,
                        Irp);
        break;

    }

    case IOCTL_DISK_SET_DRIVE_LAYOUT: {

        DebugPrint((1, "IOCTL_DISK_SET_DRIVE_LAYOUT to device %p through irp %p\n",
                    DeviceObject, Irp));
        DebugPrint((1, "Device is a%s.\n",
                    commonExtension->IsFdo ? "n fdo" : " pdo"));

        if(!commonExtension->IsFdo) {
            ClassReleaseRemoveLock(DeviceObject, Irp);
            ExFreePool(srb);
            SendToFdo(DeviceObject, Irp, status);
            return status;
        }

        status = DiskIoctlSetDriveLayout(DeviceObject, Irp);

        //
        // Notify everyone that the disk layout has changed
        //
        if (NT_SUCCESS(status))
        {
            TARGET_DEVICE_CUSTOM_NOTIFICATION Notification = {0};

            Notification.Event   = GUID_IO_DISK_LAYOUT_CHANGE;
            Notification.Version = 1;
            Notification.Size    = (USHORT)FIELD_OFFSET(TARGET_DEVICE_CUSTOM_NOTIFICATION, CustomDataBuffer);
            Notification.FileObject = NULL;
            Notification.NameBufferOffset = -1;

            IoReportTargetDeviceChangeAsynchronous(fdoExtension->LowerPdo,
                                                   &Notification,
                                                   NULL,
                                                   NULL);
        }

        break;
    }

    case IOCTL_DISK_SET_DRIVE_LAYOUT_EX: {

        DebugPrint((1, "IOCTL_DISK_SET_DRIVE_LAYOUT_EX to device %p through irp %p\n",
                    DeviceObject, Irp));
        DebugPrint((1, "Device is a%s.\n",
                    commonExtension->IsFdo ? "n fdo" : " pdo"));

        if (!commonExtension->IsFdo) {
            ClassReleaseRemoveLock(DeviceObject, Irp);
            ExFreePool(srb);
            SendToFdo(DeviceObject, Irp, status);

            return status;
        }

        status = DiskIoctlSetDriveLayoutEx(
                        DeviceObject,
                        Irp);

        //
        // Notify everyone that the disk layout has changed
        //
        if (NT_SUCCESS(status))
        {
            TARGET_DEVICE_CUSTOM_NOTIFICATION Notification = {0};

            Notification.Event   = GUID_IO_DISK_LAYOUT_CHANGE;
            Notification.Version = 1;
            Notification.Size    = (USHORT)FIELD_OFFSET(TARGET_DEVICE_CUSTOM_NOTIFICATION, CustomDataBuffer);
            Notification.FileObject = NULL;
            Notification.NameBufferOffset = -1;

            IoReportTargetDeviceChangeAsynchronous(fdoExtension->LowerPdo,
                                                   &Notification,
                                                   NULL,
                                                   NULL);
        }

        break;
    }

    case IOCTL_DISK_GET_PARTITION_INFO: {

        DebugPrint((1, "IOCTL_DISK_GET_PARTITION_INFO to device %p through irp %p\n",
                DeviceObject, Irp));
        DebugPrint((1, "Device is a%s.\n",
                commonExtension->IsFdo ? "n fdo" : " pdo"));

        status = DiskIoctlGetPartitionInfo(
                        DeviceObject,
                        Irp);
        break;
    }

    case IOCTL_DISK_GET_PARTITION_INFO_EX: {

        DebugPrint((1, "IOCTL_DISK_GET_PARTITION_INFO to device %p through irp %p\n",
                DeviceObject, Irp));
        DebugPrint((1, "Device is a%s.\n",
                commonExtension->IsFdo ? "n fdo" : " pdo"));

        status = DiskIoctlGetPartitionInfoEx(
                        DeviceObject,
                        Irp);
        break;
    }

    case IOCTL_DISK_GET_LENGTH_INFO: {
        DebugPrint((1, "IOCTL_DISK_GET_LENGTH_INFO to device %p through irp %p\n",
                DeviceObject, Irp));
        DebugPrint((1, "Device is a%s.\n",
                commonExtension->IsFdo ? "n fdo" : " pdo"));

        status = DiskIoctlGetLengthInfo(
                        DeviceObject,
                        Irp);
        break;
    }

    case IOCTL_DISK_SET_PARTITION_INFO: {

        DebugPrint((1, "IOCTL_DISK_SET_PARTITION_INFO to device %p through irp %p\n",
                    DeviceObject, Irp));
        DebugPrint((1, "Device is a%s.\n",
                    commonExtension->IsFdo ? "n fdo" : " pdo"));

        status = DiskIoctlSetPartitionInfo (
                        DeviceObject,
                        Irp);

        //
        // Notify everyone that the disk layout has changed
        //
        if (NT_SUCCESS(status))
        {
            TARGET_DEVICE_CUSTOM_NOTIFICATION Notification = {0};

            Notification.Event   = GUID_IO_DISK_LAYOUT_CHANGE;
            Notification.Version = 1;
            Notification.Size    = (USHORT)FIELD_OFFSET(TARGET_DEVICE_CUSTOM_NOTIFICATION, CustomDataBuffer);
            Notification.FileObject = NULL;
            Notification.NameBufferOffset = -1;

            //
            // We can safely assume that we are in the context of a PDO
            //
            IoReportTargetDeviceChangeAsynchronous((commonExtension->PartitionZeroExtension)->LowerPdo,
                                                   &Notification,
                                                   NULL,
                                                   NULL);
        }

        break;
    }


    case IOCTL_DISK_SET_PARTITION_INFO_EX: {

        DebugPrint((1, "IOCTL_DISK_SET_PARTITION_INFO_EX to device %p through irp %p\n",
                    DeviceObject, Irp));
        DebugPrint((1, "Device is a%s.\n",
                    commonExtension->IsFdo ? "n fdo" : " pdo"));

        status = DiskIoctlSetPartitionInfoEx(
                        DeviceObject,
                        Irp);

        //
        // Notify everyone that the disk layout has changed
        //
        if (NT_SUCCESS(status))
        {
            TARGET_DEVICE_CUSTOM_NOTIFICATION Notification = {0};

            Notification.Event   = GUID_IO_DISK_LAYOUT_CHANGE;
            Notification.Version = 1;
            Notification.Size    = (USHORT)FIELD_OFFSET(TARGET_DEVICE_CUSTOM_NOTIFICATION, CustomDataBuffer);
            Notification.FileObject = NULL;
            Notification.NameBufferOffset = -1;

            //
            // We can safely assume that we are in the context of a PDO
            //
            IoReportTargetDeviceChangeAsynchronous((commonExtension->PartitionZeroExtension)->LowerPdo,
                                                   &Notification,
                                                   NULL,
                                                   NULL);
        }

        break;
    }

    case IOCTL_DISK_DELETE_DRIVE_LAYOUT: {

        CREATE_DISK CreateDiskInfo = { 0 };

        //
        // Update the disk with new partition information.
        //

        DebugPrint((1, "IOCTL_DISK_DELETE_DRIVE_LAYOUT to device %p through irp %p\n",
                    DeviceObject, Irp));
        DebugPrint((1, "Device is a%s.\n",
                    commonExtension->IsFdo ? "n fdo" : " pdo"));

        if(!commonExtension->IsFdo) {

            ClassReleaseRemoveLock(DeviceObject, Irp);
            ExFreePool(srb);
            SendToFdo(DeviceObject, Irp, status);
            return status;
        }

        DiskAcquirePartitioningLock(fdoExtension);

        DiskInvalidatePartitionTable(fdoExtension, TRUE);

        //
        // IoCreateDisk called with a partition style of raw
        // will remove any partition tables from the disk.
        //

        CreateDiskInfo.PartitionStyle = PARTITION_STYLE_RAW;

        status = IoCreateDisk(
                    DeviceObject,
                    &CreateDiskInfo);

        //
        // Notify everyone that the disk layout has changed
        //
        if (NT_SUCCESS(status))
        {
            TARGET_DEVICE_CUSTOM_NOTIFICATION Notification = {0};

            Notification.Event   = GUID_IO_DISK_LAYOUT_CHANGE;
            Notification.Version = 1;
            Notification.Size    = (USHORT)FIELD_OFFSET(TARGET_DEVICE_CUSTOM_NOTIFICATION, CustomDataBuffer);
            Notification.FileObject = NULL;
            Notification.NameBufferOffset = -1;

            IoReportTargetDeviceChangeAsynchronous(fdoExtension->LowerPdo,
                                                   &Notification,
                                                   NULL,
                                                   NULL);
        }

        DiskReleasePartitioningLock(fdoExtension);

        ClassInvalidateBusRelations(DeviceObject);

        Irp->IoStatus.Status = status;

        break;
    }

    case IOCTL_DISK_REASSIGN_BLOCKS: {

        //
        // Map defective blocks to new location on disk.
        //

        PREASSIGN_BLOCKS badBlocks = Irp->AssociatedIrp.SystemBuffer;
        ULONG bufferSize;
        ULONG blockNumber;
        ULONG blockCount;

        DebugPrint((2, "IOCTL_DISK_REASSIGN_BLOCKS to device %p through irp %p\n",
                    DeviceObject, Irp));
        DebugPrint((2, "Device is a%s.\n",
                    commonExtension->IsFdo ? "n fdo" : " pdo"));

        //
        // Validate buffer length.
        //

        if (irpStack->Parameters.DeviceIoControl.InputBufferLength <
            sizeof(REASSIGN_BLOCKS)) {

            status = STATUS_INFO_LENGTH_MISMATCH;
            break;
        }

        bufferSize = sizeof(REASSIGN_BLOCKS) +
            ((badBlocks->Count - 1) * sizeof(ULONG));

        if (irpStack->Parameters.DeviceIoControl.InputBufferLength <
            bufferSize) {

            status = STATUS_INFO_LENGTH_MISMATCH;
            break;
        }

        //
        // Send to FDO
        //

        if(!commonExtension->IsFdo) {

            ClassReleaseRemoveLock(DeviceObject, Irp);
            ExFreePool(srb);
            SendToFdo(DeviceObject, Irp, status);
            return status;
        }

        //
        // Build the data buffer to be transferred in the input buffer.
        // The format of the data to the device is:
        //
        //      2 bytes Reserved
        //      2 bytes Length
        //      x * 4 btyes Block Address
        //
        // All values are big endian.
        //

        badBlocks->Reserved = 0;
        blockCount = badBlocks->Count;

        //
        // Convert # of entries to # of bytes.
        //

        blockCount *= 4;
        badBlocks->Count = (USHORT) ((blockCount >> 8) & 0XFF);
        badBlocks->Count |= (USHORT) ((blockCount << 8) & 0XFF00);

        //
        // Convert back to number of entries.
        //

        blockCount /= 4;

        for (; blockCount > 0; blockCount--) {

            blockNumber = badBlocks->BlockNumber[blockCount-1];

            REVERSE_BYTES((PFOUR_BYTE) &badBlocks->BlockNumber[blockCount-1],
                          (PFOUR_BYTE) &blockNumber);
        }

        srb->CdbLength = 6;

        cdb->CDB6GENERIC.OperationCode = SCSIOP_REASSIGN_BLOCKS;

        //
        // Set timeout value.
        //

        srb->TimeOutValue = fdoExtension->TimeOutValue;

        status = ClassSendSrbSynchronous(DeviceObject,
                                         srb,
                                         badBlocks,
                                         bufferSize,
                                         TRUE);
        break;
    }

    case IOCTL_DISK_REASSIGN_BLOCKS_EX: {

        //
        // Map defective blocks to new location on disk.
        //

        PREASSIGN_BLOCKS_EX badBlocks = Irp->AssociatedIrp.SystemBuffer;
        LARGE_INTEGER blockNumber;
        ULONG bufferSize;
        ULONG blockCount;

        DebugPrint((2, "IOCTL_DISK_REASSIGN_BLOCKS_EX to device %p through irp %p\n",
                    DeviceObject, Irp));
        DebugPrint((2, "Device is a%s.\n",
                    commonExtension->IsFdo ? "n fdo" : " pdo"));

        //
        // Validate buffer length.
        //

        if (irpStack->Parameters.DeviceIoControl.InputBufferLength <
            sizeof(REASSIGN_BLOCKS_EX)) {

            status = STATUS_INFO_LENGTH_MISMATCH;
            break;
        }

        bufferSize = sizeof(REASSIGN_BLOCKS_EX) +
            ((badBlocks->Count - 1) * sizeof(LARGE_INTEGER));

        if (irpStack->Parameters.DeviceIoControl.InputBufferLength <
            bufferSize) {

            status = STATUS_INFO_LENGTH_MISMATCH;
            break;
        }

        //
        // Send to FDO
        //

        if(!commonExtension->IsFdo) {

            ClassReleaseRemoveLock(DeviceObject, Irp);
            ExFreePool(srb);
            SendToFdo(DeviceObject, Irp, status);
            return status;
        }

        //
        // Build the data buffer to be transferred in the input buffer.
        // The format of the data to the device is:
        //
        //      2 bytes Reserved
        //      2 bytes Length
        //      x * 8 btyes Block Address
        //
        // All values are big endian.
        //

        badBlocks->Reserved = 0;
        blockCount = badBlocks->Count;

        //
        // Convert # of entries to # of bytes.
        //

        blockCount *= 8;
        badBlocks->Count = (USHORT) ((blockCount >> 8) & 0XFF);
        badBlocks->Count |= (USHORT) ((blockCount << 8) & 0XFF00);

        //
        // Convert back to number of entries.
        //

        blockCount /= 8;

        for (; blockCount > 0; blockCount--) {

            blockNumber = badBlocks->BlockNumber[blockCount-1];
            REVERSE_BYTES_QUAD(&badBlocks->BlockNumber[blockCount-1], &blockNumber);
        }

        srb->CdbLength = 6;

        cdb->CDB6GENERIC.OperationCode = SCSIOP_REASSIGN_BLOCKS;
        cdb->CDB6GENERIC.CommandUniqueBits =  1; // LONGLBA

        //
        // Set timeout value.
        //

        srb->TimeOutValue = fdoExtension->TimeOutValue;

        status = ClassSendSrbSynchronous(DeviceObject,
                                         srb,
                                         badBlocks,
                                         bufferSize,
                                         TRUE);
        break;
    }


    case IOCTL_DISK_IS_WRITABLE: {

        //
        // This routine mimics IOCTL_STORAGE_GET_MEDIA_TYPES_EX
        //

        ULONG modeLength;
        ULONG retries = 4;

        DebugPrint((3, "Disk.DiskDeviceControl: IOCTL_DISK_IS_WRITABLE\n"));

        if (!commonExtension->IsFdo)
        {
            ClassReleaseRemoveLock(DeviceObject, Irp);
            ExFreePool(srb);
            SendToFdo(DeviceObject, Irp, status);
            return status;
        }

        RtlZeroMemory(srb, SCSI_REQUEST_BLOCK_SIZE);

        //
        // Allocate memory for a mode header and then some
        // for port drivers that need to convert to MODE10
        // or always return the MODE_PARAMETER_BLOCK (even
        // when memory was not allocated for this purpose)
        //

        modeLength = sizeof(MODE_PARAMETER_HEADER) + sizeof(MODE_PARAMETER_BLOCK);
        modeData = ExAllocatePoolWithTag(NonPagedPoolCacheAligned,
                                         modeLength,
                                         DISK_TAG_MODE_DATA);

        if (modeData == NULL)
        {
            status = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }

        RtlZeroMemory(modeData, modeLength);

        //
        // Build the MODE SENSE CDB
        //

        srb->CdbLength = 6;
        cdb = (PCDB)srb->Cdb;

        //
        // Set the timeout value from the device extension
        //

        srb->TimeOutValue = fdoExtension->TimeOutValue;

        cdb->MODE_SENSE.OperationCode    = SCSIOP_MODE_SENSE;
        cdb->MODE_SENSE.PageCode         = MODE_SENSE_RETURN_ALL;
        cdb->MODE_SENSE.AllocationLength = (UCHAR)modeLength;

        while (retries != 0)
        {
            status = ClassSendSrbSynchronous(DeviceObject,
                                             srb,
                                             modeData,
                                             modeLength,
                                             FALSE);

            if (status != STATUS_VERIFY_REQUIRED)
            {
                if (SRB_STATUS(srb->SrbStatus) == SRB_STATUS_DATA_OVERRUN)
                {
                    status = STATUS_SUCCESS;
                }

                break;
            }

            retries--;
        }

        if (NT_SUCCESS(status))
        {
            if (TEST_FLAG(modeData->DeviceSpecificParameter, MODE_DSP_WRITE_PROTECT))
            {
                status = STATUS_MEDIA_WRITE_PROTECTED;
            }
        }

        ExFreePool(modeData);
        break;
    }

    case IOCTL_DISK_INTERNAL_SET_VERIFY: {

        //
        // If the caller is kernel mode, set the verify bit.
        //

        if (Irp->RequestorMode == KernelMode) {

            SET_FLAG(DeviceObject->Flags, DO_VERIFY_VOLUME);
        }

        DiskInvalidatePartitionTable(fdoExtension, FALSE);

        status = STATUS_SUCCESS;
        break;
    }

    case IOCTL_DISK_INTERNAL_CLEAR_VERIFY: {

        //
        // If the caller is kernel mode, clear the verify bit.
        //

        if (Irp->RequestorMode == KernelMode) {
            CLEAR_FLAG(DeviceObject->Flags, DO_VERIFY_VOLUME);
        }
        status = STATUS_SUCCESS;
        break;
    }

    case IOCTL_DISK_UPDATE_DRIVE_SIZE: {

        DebugPrint((2, "IOCTL_DISK_UPDATE_DRIVE_SIZE to device %p "
                       "through irp %p\n",
                    DeviceObject, Irp));

        DebugPrint((2, "Device is a%s.\n",
                    commonExtension->IsFdo ? "n fdo" : " pdo"));

        if (irpStack->Parameters.DeviceIoControl.OutputBufferLength <
            sizeof(DISK_GEOMETRY)) {

            status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

        if(!commonExtension->IsFdo) {

            //
            // Pdo should issue this request to the lower device object.
            //

            ClassReleaseRemoveLock(DeviceObject, Irp);
            ExFreePool(srb);
            SendToFdo(DeviceObject, Irp, status);
            return status;
        }

        DiskAcquirePartitioningLock(fdoExtension);

        //
        // Invalidate the cached partition table.
        //

        DiskInvalidatePartitionTable(fdoExtension, TRUE);

        //
        // At this point, commonExtension *is* the FDO extension.  This
        // should be the same as PartitionZeroExtension.
        //

        ASSERT(commonExtension ==
               &(commonExtension->PartitionZeroExtension->CommonExtension));

        //
        // Issue ReadCapacity to update device extension with information
        // for current media.
        //

        status = DiskReadDriveCapacity(DeviceObject);

        //
        // Note whether the drive is ready.
        //

        diskData->ReadyStatus = status;

        //
        // The disk's partition tables may be invalid after the drive geometry
        // has been updated. The call to IoValidatePartitionTable (below) will
        // fix it if this is the case.
        //

        if (NT_SUCCESS(status)) {

            status = DiskVerifyPartitionTable (fdoExtension, TRUE);
        }


        if (NT_SUCCESS(status)) {

            //
            // Copy drive geometry information from the device extension.
            //

            RtlMoveMemory(Irp->AssociatedIrp.SystemBuffer,
                          &(fdoExtension->DiskGeometry),
                          sizeof(DISK_GEOMETRY));

            Irp->IoStatus.Information = sizeof(DISK_GEOMETRY);
            status = STATUS_SUCCESS;

            //
            // Notify everyone that the disk layout may have changed
            //
            {
                TARGET_DEVICE_CUSTOM_NOTIFICATION Notification = {0};

                Notification.Event   = GUID_IO_DISK_LAYOUT_CHANGE;
                Notification.Version = 1;
                Notification.Size    = (USHORT)FIELD_OFFSET(TARGET_DEVICE_CUSTOM_NOTIFICATION, CustomDataBuffer);
                Notification.FileObject = NULL;
                Notification.NameBufferOffset = -1;

                IoReportTargetDeviceChangeAsynchronous(fdoExtension->LowerPdo,
                                                       &Notification,
                                                       NULL,
                                                       NULL);
            }
        }

        DiskReleasePartitioningLock(fdoExtension);

        break;
    }

    case IOCTL_DISK_GROW_PARTITION: {

        PDISK_GROW_PARTITION inputBuffer;
        PCOMMON_DEVICE_EXTENSION pdoCommonExtension = &commonExtension->ChildList->CommonExtension;
        LARGE_INTEGER bytesPerCylinder;
        LARGE_INTEGER newStoppingOffset;
        LARGE_INTEGER newPartitionLength;

        PPHYSICAL_DEVICE_EXTENSION sibling;

        PDRIVE_LAYOUT_INFORMATION_EX layoutInfo;
        PPARTITION_INFORMATION_EX pdoPartition;
        PPARTITION_INFORMATION_EX containerPartition;
        ULONG partitionIndex;

        DebugPrint((2, "IOCTL_DISK_GROW_PARTITION to device %p through "
                       "irp %p\n",
                    DeviceObject, Irp));

        DebugPrint((2, "Device is a%s.\n",
                    commonExtension->IsFdo ? "n fdo" : " pdo"));

        if (irpStack->Parameters.DeviceIoControl.InputBufferLength <
            sizeof(DISK_GROW_PARTITION)) {

            status = STATUS_INFO_LENGTH_MISMATCH;
            break;
        }

        if(!commonExtension->IsFdo) {

            //
            // Pdo should issue this request to the lower device object
            //

            ClassReleaseRemoveLock(DeviceObject, Irp);
            ExFreePool(srb);
            SendToFdo(DeviceObject, Irp, status);
            return status;
        }

        DiskAcquirePartitioningLock(fdoExtension);

        ClassAcquireChildLock(fdoExtension);

        //
        // At this point, commonExtension *is* the FDO extension.  This should
        // be the same as PartitionZeroExtension.
        //

        ASSERT(commonExtension ==
               &(commonExtension->PartitionZeroExtension->CommonExtension));

        //
        // Get the input parameters
        //

        inputBuffer = (PDISK_GROW_PARTITION) Irp->AssociatedIrp.SystemBuffer;

        ASSERT(inputBuffer);

        //
        // Make sure that we are actually being asked to grow the partition.
        //

        if(inputBuffer->BytesToGrow.QuadPart <= 0) {

            status = STATUS_INVALID_PARAMETER;
            ClassReleaseChildLock(fdoExtension);
            DiskReleasePartitioningLock(fdoExtension);
            break;
        }

        //
        // Find the partition that matches the supplied number
        //


        while (pdoCommonExtension){

            //
            // Is this the partition we are searching for?
            //

            if(inputBuffer->PartitionNumber == pdoCommonExtension->PartitionNumber) {
                break;
            }

            pdoCommonExtension = &pdoCommonExtension->ChildList->CommonExtension;
        }

        // Did we find the partition?

        if (pdoCommonExtension == NULL){
            status = STATUS_INVALID_PARAMETER;
            ClassReleaseChildLock(fdoExtension);
            DiskReleasePartitioningLock(fdoExtension);
            break;
        }

        //
        // Compute the new values for the partition to grow.
        //

        newPartitionLength.QuadPart =
            (pdoCommonExtension->PartitionLength.QuadPart +
             inputBuffer->BytesToGrow.QuadPart);

        newStoppingOffset.QuadPart =
            (pdoCommonExtension->StartingOffset.QuadPart +
             newPartitionLength.QuadPart - 1);

        //
        // The below alignment requirement only applies to the MBR layout
        //

        if (diskData->PartitionStyle == PARTITION_STYLE_MBR) {

            //
            // Test the partition alignment before getting to involved.
            //
            // NOTE:
            //     All partition stopping offsets should be one byte less
            //     than a cylinder boundary offset. Also, all first partitions
            //     (within partition0 and within an extended partition) start
            //     on the second track while all other partitions start on a
            //     cylinder boundary.
            //
            bytesPerCylinder.QuadPart =
                ((LONGLONG) fdoExtension->DiskGeometry.TracksPerCylinder *
                 (LONGLONG) fdoExtension->DiskGeometry.SectorsPerTrack *
                 (LONGLONG) fdoExtension->DiskGeometry.BytesPerSector);

            // Temporarily adjust up to cylinder boundary.
            newStoppingOffset.QuadPart += 1;

            if(newStoppingOffset.QuadPart % bytesPerCylinder.QuadPart) {

                // Adjust the length first...
                newPartitionLength.QuadPart -=
                    (newStoppingOffset.QuadPart % bytesPerCylinder.QuadPart);

                // ...and then the stopping offset.
                newStoppingOffset.QuadPart -=
                    (newStoppingOffset.QuadPart % bytesPerCylinder.QuadPart);

                DebugPrint((2, "IOCTL_DISK_GROW_PARTITION: "
                               "Adjusted the requested partition size to cylinder boundary"));
            }

            // Restore to one byte less than a cylinder boundary.
            newStoppingOffset.QuadPart -= 1;
        }

        //
        // Will the new partition fit within Partition0?
        // Remember: commonExtension == &PartitionZeroExtension->CommonExtension
        //

        if(newStoppingOffset.QuadPart >
            (commonExtension->StartingOffset.QuadPart +
             commonExtension->PartitionLength.QuadPart - 1)) {

            //
            // The new partition falls outside Partition0
            //

            status = STATUS_UNSUCCESSFUL;
            ClassReleaseChildLock(fdoExtension);
            DiskReleasePartitioningLock(fdoExtension);
            break;
        }

        //
        // Search for any partition that will conflict with the new partition.
        // This is done before testing for any containing partitions to
        // simplify the container handling.
        //

        sibling = commonExtension->ChildList;

        while(sibling != NULL) {
            LARGE_INTEGER sibStoppingOffset;
            PCOMMON_DEVICE_EXTENSION siblingExtension;

            siblingExtension = &(sibling->CommonExtension);

            ASSERT( siblingExtension );

            sibStoppingOffset.QuadPart =
                (siblingExtension->StartingOffset.QuadPart +
                 siblingExtension->PartitionLength.QuadPart - 1);

            //
            // Only check the siblings that start beyond the new partition
            // starting offset.  Also, assume that since the starting offset
            // has not changed, it will not be in conflict with any other
            // partitions; only the new stopping offset needs to be tested.
            //

            if((inputBuffer->PartitionNumber !=
                siblingExtension->PartitionNumber) &&

               (siblingExtension->StartingOffset.QuadPart >
                pdoCommonExtension->StartingOffset.QuadPart) &&

               (newStoppingOffset.QuadPart >=
                siblingExtension->StartingOffset.QuadPart)) {

                //
                // We have a conflict; bail out leaving pdoSibling set.
                //

                break;
            }
            sibling = siblingExtension->ChildList;
        }


        //
        // If there is a sibling that conflicts, it will be in pdoSibling; there
        // could be more than one, but this is the first one detected.
        //

        if(sibling != NULL) {
            //
            // Report the conflict and abort the grow request.
            //

            status = STATUS_UNSUCCESSFUL;
            ClassReleaseChildLock(fdoExtension);
            DiskReleasePartitioningLock(fdoExtension);
            break;
        }

        //
        // Read the partition table.  Since we're planning on modifying it
        // we should bypass the cache.
        //

        status = DiskReadPartitionTableEx(fdoExtension, TRUE, &layoutInfo );

        if( !NT_SUCCESS(status) ) {
            ClassReleaseChildLock(fdoExtension);
            DiskReleasePartitioningLock(fdoExtension);
            break;
        }

        ASSERT( layoutInfo );

        //
        // The  allowed  partition growth is a bit  smaller on
        // GPT due to the presence of a backup partition table
        //

        if ((layoutInfo->PartitionStyle == PARTITION_STYLE_GPT) &&
            (newStoppingOffset.QuadPart >= layoutInfo->Gpt.StartingUsableOffset.QuadPart +
                                           layoutInfo->Gpt.UsableLength.QuadPart)) {

            status = STATUS_UNSUCCESSFUL;
            ClassReleaseChildLock(fdoExtension);
            DiskReleasePartitioningLock(fdoExtension);
            break;
        }

        //
        // Search the layout for the partition that matches the
        // PDO in hand.
        //

        pdoPartition =
            DiskPdoFindPartitionEntry(
                (PPHYSICAL_DEVICE_EXTENSION)pdoCommonExtension,
                layoutInfo);

        if(pdoPartition == NULL) {
            // Looks like something is wrong interally-- error ok?
            status = STATUS_DRIVER_INTERNAL_ERROR;
            layoutInfo = NULL;
            ClassReleaseChildLock(fdoExtension);
            DiskReleasePartitioningLock(fdoExtension);
            break;
        }

        //
        // Search the on-disk partition information to find the root containing
        // partition (top-to-bottom).
        //
        // Remember: commonExtension == &PartitionZeroExtension->CommonExtension
        //

        //
        // All affected containers will have a new stopping offset
        // that is equal to the new partition (logical drive)
        // stopping offset.  Walk the layout information from
        // bottom-to-top searching for logical drive containers and
        // propagating the change.
        //

        containerPartition =
            DiskFindContainingPartition(
                layoutInfo,
                pdoPartition,
                FALSE);

        //
        // This loop should only execute at most 2 times; once for
        // the logical drive container, and once for the root
        // extended partition container.  If the growing partition
        // is not contained, the loop does not run.
        //

        while(containerPartition != NULL) {
            LARGE_INTEGER containerStoppingOffset;
            PPARTITION_INFORMATION_EX nextContainerPartition;

            //
            // Plan ahead and get the container's container before
            // modifing the current size.
            //

            nextContainerPartition =
                DiskFindContainingPartition(
                    layoutInfo,
                    containerPartition,
                    FALSE);

            //
            // Figure out where the current container ends and test
            // to see if it already encompasses the containee.
            //

            containerStoppingOffset.QuadPart =
                (containerPartition->StartingOffset.QuadPart +
                 containerPartition->PartitionLength.QuadPart - 1);

            if(newStoppingOffset.QuadPart <=
               containerStoppingOffset.QuadPart) {

                //
                // No need to continue since this container fits
                //
                break;
            }

            //
            // Adjust the container to have a stopping offset that
            // matches the grown partition stopping offset.
            //

            containerPartition->PartitionLength.QuadPart =
                newStoppingOffset.QuadPart + 1 -
                containerPartition->StartingOffset.QuadPart;

            containerPartition->RewritePartition = TRUE;

            // Continue with the next container
            containerPartition = nextContainerPartition;
        }

        //
        // Wait until after searching the containers to update the
        // partition size.
        //

        pdoPartition->PartitionLength.QuadPart =
            newPartitionLength.QuadPart;

        pdoPartition->RewritePartition = TRUE;

        //
        // Commit the changes to disk
        //

        status = DiskWritePartitionTableEx(fdoExtension, layoutInfo );

        if( NT_SUCCESS(status) ) {

            //
            // Everything looks good so commit the new length to the
            // PDO.  This has to be done carefully.  We may potentially
            // grow the partition in three steps:
            //  * increase the high-word of the partition length
            //    to be just below the new size - the high word should
            //    be greater than or equal to the current length.
            //
            //  * change the low-word of the partition length to the
            //    new value - this value may potentially be lower than
            //    the current value (if the high part was changed which
            //    is why we changed that first)
            //
            //  * change the high part to the correct value.
            //

            if(newPartitionLength.HighPart >
               pdoCommonExtension->PartitionLength.HighPart) {

                //
                // Swap in one less than the high word.
                //

                InterlockedExchange(
                    &pdoCommonExtension->PartitionLength.HighPart,
                    (newPartitionLength.HighPart - 1));
            }

            //
            // Swap in the low part.
            //

            InterlockedExchange(
                &(pdoCommonExtension->PartitionLength.LowPart),
                newPartitionLength.LowPart);

            if(newPartitionLength.HighPart !=
               pdoCommonExtension->PartitionLength.HighPart) {

                //
                // Swap in one less than the high word.
                //

                InterlockedExchange(
                    &(pdoCommonExtension->PartitionLength.HighPart),
                    newPartitionLength.HighPart);
            }

            //
            // Notify everyone that the disk layout has changed
            //
            {
                TARGET_DEVICE_CUSTOM_NOTIFICATION Notification = {0};

                Notification.Event   = GUID_IO_DISK_LAYOUT_CHANGE;
                Notification.Version = 1;
                Notification.Size    = (USHORT)FIELD_OFFSET(TARGET_DEVICE_CUSTOM_NOTIFICATION, CustomDataBuffer);
                Notification.FileObject = NULL;
                Notification.NameBufferOffset = -1;

                IoReportTargetDeviceChangeAsynchronous(fdoExtension->LowerPdo,
                                                       &Notification,
                                                       NULL,
                                                       NULL);
            }
        }

        //
        // Invalidate and free the cached partition table.
        //

        DiskInvalidatePartitionTable(fdoExtension, TRUE);

        //
        // Free the partition buffer regardless of the status
        //

        ClassReleaseChildLock(fdoExtension);
        DiskReleasePartitioningLock(fdoExtension);

        break;
    }

    case IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS:
    case IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS_ADMIN:
    {
        if (TEST_FLAG(DeviceObject->Characteristics, FILE_REMOVABLE_MEDIA))
        {
            if (irpStack->Parameters.DeviceIoControl.OutputBufferLength < sizeof(VOLUME_DISK_EXTENTS))
            {
                status = STATUS_BUFFER_TOO_SMALL;
                break;
            }

            status = DiskReadDriveCapacity(commonExtension->PartitionZeroExtension->DeviceObject);

            //
            // Note whether the drive is ready.
            //

            diskData->ReadyStatus = status;

            if (NT_SUCCESS(status))
            {
                PVOLUME_DISK_EXTENTS pVolExt = (PVOLUME_DISK_EXTENTS)Irp->AssociatedIrp.SystemBuffer;

                pVolExt->NumberOfDiskExtents = 1;
                pVolExt->Extents[0].DiskNumber     = commonExtension->PartitionZeroExtension->DeviceNumber;
                pVolExt->Extents[0].StartingOffset = commonExtension->StartingOffset;
                pVolExt->Extents[0].ExtentLength   = commonExtension->PartitionLength;

                Irp->IoStatus.Information = sizeof(VOLUME_DISK_EXTENTS);
            }
        }
        else
        {
            status = STATUS_INVALID_DEVICE_REQUEST;
        }

        break;
    }

    case IOCTL_DISK_UPDATE_PROPERTIES: {

        if (!commonExtension->IsFdo)
        {
            ClassReleaseRemoveLock(DeviceObject, Irp);
            ExFreePool(srb);
            SendToFdo(DeviceObject,Irp,status);
            return status;
        }

        //
        // Invalidate the partition table and re-enumerate the device.
        //

        if (DiskInvalidatePartitionTable(fdoExtension, FALSE))
        {
            IoInvalidateDeviceRelations(fdoExtension->LowerPdo, BusRelations);

            //
            // Notify everyone that the disk layout may have changed
            //
            {
                TARGET_DEVICE_CUSTOM_NOTIFICATION Notification = {0};

                Notification.Event   = GUID_IO_DISK_LAYOUT_CHANGE;
                Notification.Version = 1;
                Notification.Size    = (USHORT)FIELD_OFFSET(TARGET_DEVICE_CUSTOM_NOTIFICATION, CustomDataBuffer);
                Notification.FileObject = NULL;
                Notification.NameBufferOffset = -1;

                IoReportTargetDeviceChangeAsynchronous(fdoExtension->LowerPdo,
                                                       &Notification,
                                                       NULL,
                                                       NULL);
            }
        }

        status = STATUS_SUCCESS;

        break;
    }

    default: {

        //
        // Free the Srb, since it is not needed.
        //

        ExFreePool(srb);

        //
        // Pass the request to the common device control routine.
        //

        return(ClassDeviceControl(DeviceObject, Irp));

        break;
    }

    } // end switch

    Irp->IoStatus.Status = status;

    if (!NT_SUCCESS(status) && IoIsErrorUserInduced(status)) {

        IoSetHardErrorOrVerifyDevice(Irp, DeviceObject);
    }

    ClassReleaseRemoveLock(DeviceObject, Irp);
    ClassCompleteRequest(DeviceObject, Irp, IO_NO_INCREMENT);
    ExFreePool(srb);
    return(status);
} // end DiskDeviceControl()


NTSTATUS
DiskShutdownFlush(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine is the handler for shutdown and flush requests. It sends
    down a synch cache command to the device if its cache is enabled.  If
    the request is a  shutdown and the media is removable,  it sends down
    an unlock request

    Finally,  an SRB_FUNCTION_SHUTDOWN or SRB_FUNCTION_FLUSH is sent down
    the stack

Arguments:

    DeviceObject - The device object processing the request
    Irp - The shutdown | flush request being serviced

Return Value:

    STATUS_PENDING if successful, an error code otherwise

--*/

{
    PCOMMON_DEVICE_EXTENSION commonExtension = DeviceObject->DeviceExtension;
    PFUNCTIONAL_DEVICE_EXTENSION fdoExtension = commonExtension->PartitionZeroExtension;
    PDISK_DATA diskData = (PDISK_DATA) commonExtension->DriverData;
    PIO_STACK_LOCATION irpStack;
    NTSTATUS status = STATUS_SUCCESS;

    PAGED_CODE();

    //
    // Send partition flush requests to the FDO
    //

    if(!commonExtension->IsFdo) {

        PDEVICE_OBJECT lowerDevice = commonExtension->LowerDeviceObject;

        ClassReleaseRemoveLock(DeviceObject, Irp);
        IoMarkIrpPending(Irp);
        IoCopyCurrentIrpStackLocationToNext(Irp);
        IoCallDriver(lowerDevice, Irp);
        return STATUS_PENDING;
    }

    //
    // Flush requests are combined and need to be handled in a special manner
    //

    irpStack = IoGetCurrentIrpStackLocation(Irp);

    if (irpStack->MajorFunction == IRP_MJ_FLUSH_BUFFERS) {

        if (TEST_FLAG(fdoExtension->DeviceFlags, DEV_POWER_PROTECTED)) {

            //
            // We've been assured that both the disk
            // and adapter caches are battery-backed
            //

            Irp->IoStatus.Status = STATUS_SUCCESS;
            ClassReleaseRemoveLock(DeviceObject, Irp);
            ClassCompleteRequest(DeviceObject, Irp, IO_NO_INCREMENT);
            return STATUS_SUCCESS;
        }

        KeWaitForMutexObject(&diskData->FlushContext.Mutex, Executive, KernelMode, FALSE, NULL);

        //
        // This request will most likely be completed asynchronously
        //
        IoMarkIrpPending(Irp);

        //
        // Look to see if a flush is in progress
        //

        if (diskData->FlushContext.CurrIrp != NULL) {

            //
            // There is an outstanding flush. Queue this
            // request to the group that is next in line
            //

            if (diskData->FlushContext.NextIrp != NULL) {

                #if DBG
                    diskData->FlushContext.DbgTagCount++;
                #endif

                InsertTailList(&diskData->FlushContext.NextList, &Irp->Tail.Overlay.ListEntry);

                KeReleaseMutex(&diskData->FlushContext.Mutex, FALSE);

                //
                // This request will be completed by its representative
                //

            } else {

                #if DBG
                    if (diskData->FlushContext.DbgTagCount < 64) {

                        diskData->FlushContext.DbgRefCount[diskData->FlushContext.DbgTagCount]++;
                    }

                    diskData->FlushContext.DbgSavCount += diskData->FlushContext.DbgTagCount;
                    diskData->FlushContext.DbgTagCount  = 0;
                #endif

                diskData->FlushContext.NextIrp = Irp;
                ASSERT(IsListEmpty(&diskData->FlushContext.NextList));

                KeReleaseMutex(&diskData->FlushContext.Mutex, FALSE);

                //
                // Wait for the outstanding flush to complete
                //
                KeWaitForSingleObject(&diskData->FlushContext.Event, Executive, KernelMode, FALSE, NULL);

                //
                // Make this group the outstanding one and free up the next slot
                //

                KeWaitForMutexObject(&diskData->FlushContext.Mutex, Executive, KernelMode, FALSE, NULL);

                ASSERT(IsListEmpty(&diskData->FlushContext.CurrList));

                while (!IsListEmpty(&diskData->FlushContext.NextList)) {

                    PLIST_ENTRY listEntry = RemoveHeadList(&diskData->FlushContext.NextList);
                    InsertTailList(&diskData->FlushContext.CurrList, listEntry);
                }

                diskData->FlushContext.CurrIrp = diskData->FlushContext.NextIrp;
                diskData->FlushContext.NextIrp = NULL;

                KeReleaseMutex(&diskData->FlushContext.Mutex, FALSE);

                //
                // Send this request down to the device
                //
                DiskFlushDispatch(DeviceObject, &diskData->FlushContext);
            }

        } else {

            diskData->FlushContext.CurrIrp = Irp;
            ASSERT(IsListEmpty(&diskData->FlushContext.CurrList));

            ASSERT(diskData->FlushContext.NextIrp == NULL);
            ASSERT(IsListEmpty(&diskData->FlushContext.NextList));

            KeReleaseMutex(&diskData->FlushContext.Mutex, FALSE);

            //
            // Send this request down to the device
            //
            DiskFlushDispatch(DeviceObject, &diskData->FlushContext);
        }

    } else {

        //
        // Allocate SCSI request block.
        //

        PSCSI_REQUEST_BLOCK srb = ExAllocatePoolWithTag(NonPagedPool,
                                                        sizeof(SCSI_REQUEST_BLOCK),
                                                        DISK_TAG_SRB);
        if (srb == NULL) {

            //
            // Set the status and complete the request.
            //

            Irp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
            ClassReleaseRemoveLock(DeviceObject, Irp);
            ClassCompleteRequest(DeviceObject, Irp, IO_NO_INCREMENT);
            return(STATUS_INSUFFICIENT_RESOURCES);
        }

        RtlZeroMemory(srb, SCSI_REQUEST_BLOCK_SIZE);

        //
        // Write length to SRB.
        //

        srb->Length = SCSI_REQUEST_BLOCK_SIZE;

        //
        // Set timeout value and mark the request as not being a tagged request.
        //

        srb->TimeOutValue = fdoExtension->TimeOutValue * 4;
        srb->QueueTag = SP_UNTAGGED;
        srb->QueueAction = SRB_SIMPLE_TAG_REQUEST;
        srb->SrbFlags = fdoExtension->SrbFlags;

        //
        // If the write cache is enabled then send a synchronize cache request.
        //

        if (TEST_FLAG(fdoExtension->DeviceFlags, DEV_WRITE_CACHE)) {

            srb->Function = SRB_FUNCTION_EXECUTE_SCSI;
            srb->CdbLength = 10;

            srb->Cdb[0] = SCSIOP_SYNCHRONIZE_CACHE;

            status = ClassSendSrbSynchronous(DeviceObject,
                                             srb,
                                             NULL,
                                             0,
                                             TRUE);

            DebugPrint((1, "DiskShutdownFlush: Synchonize cache sent. Status = %lx\n", status ));
        }

        //
        // Unlock the device if it contains removable media
        //

        if (TEST_FLAG(DeviceObject->Characteristics, FILE_REMOVABLE_MEDIA))
        {
            PCDB cdb;

            srb->CdbLength = 6;
            cdb = (PVOID) srb->Cdb;
            cdb->MEDIA_REMOVAL.OperationCode = SCSIOP_MEDIUM_REMOVAL;
            cdb->MEDIA_REMOVAL.Prevent = FALSE;

            //
            // Set timeout value.
            //

            srb->TimeOutValue = fdoExtension->TimeOutValue;
            status = ClassSendSrbSynchronous(DeviceObject,
                                             srb,
                                             NULL,
                                             0,
                                             TRUE);

            DebugPrint((1, "DiskShutdownFlush: Unlock device request sent. Status = %lx\n", status ));
        }

        srb->CdbLength = 0;

        //
        // Save a few parameters in the current stack location.
        //

        srb->Function = SRB_FUNCTION_SHUTDOWN;

        //
        // Set the retry count to zero.
        //

        irpStack->Parameters.Others.Argument4 = (PVOID) 0;

        //
        // Set up IoCompletion routine address.
        //

        IoSetCompletionRoutine(Irp, ClassIoComplete, srb, TRUE, TRUE, TRUE);

        //
        // Get next stack location and
        // set major function code.
        //

        irpStack = IoGetNextIrpStackLocation(Irp);

        irpStack->MajorFunction = IRP_MJ_SCSI;

        //
        // Set up SRB for execute scsi request.
        // Save SRB address in next stack for port driver.
        //

        irpStack->Parameters.Scsi.Srb = srb;

        //
        // Set up Irp Address.
        //

        srb->OriginalRequest = Irp;

        //
        // Call the port driver to process the request.
        //

        IoMarkIrpPending(Irp);
        IoCallDriver(commonExtension->LowerDeviceObject, Irp);
    }

    return STATUS_PENDING;
}


VOID
DiskFlushDispatch(
    IN PDEVICE_OBJECT Fdo,
    IN PDISK_GROUP_CONTEXT FlushContext
    )

/*++

Routine Description:

    This routine is the handler for flush requests. It sends down a synch
    cache command to the device if its cache is enabled. This is followed
    by an SRB_FUNCTION_FLUSH

Arguments:

    Fdo - The device object processing the flush request
    FlushContext - The flush group context

Return Value:

    None

--*/

{
    PFUNCTIONAL_DEVICE_EXTENSION fdoExt = Fdo->DeviceExtension;
    PSCSI_REQUEST_BLOCK srb = &FlushContext->Srb;
    PIO_STACK_LOCATION  irpSp = NULL;

    PAGED_CODE();

    ASSERT_FDO(Fdo);

    //
    // Fill in the srb fields appropriately
    //
    RtlZeroMemory(srb, SCSI_REQUEST_BLOCK_SIZE);

    srb->Length       = SCSI_REQUEST_BLOCK_SIZE;
    srb->TimeOutValue = fdoExt->TimeOutValue * 4;
    srb->QueueTag     = SP_UNTAGGED;
    srb->QueueAction  = SRB_SIMPLE_TAG_REQUEST;
    srb->SrbFlags     = fdoExt->SrbFlags;

    //
    // If write caching is enabled then send down a synchronize cache request
    //
    if (TEST_FLAG(fdoExt->DeviceFlags, DEV_WRITE_CACHE))
    {
        srb->Function  = SRB_FUNCTION_EXECUTE_SCSI;
        srb->CdbLength = 10;
        srb->Cdb[0]    = SCSIOP_SYNCHRONIZE_CACHE;

        ClassSendSrbSynchronous(Fdo, srb, NULL, 0, TRUE);
    }

    srb->Function  = SRB_FUNCTION_FLUSH;
    srb->CdbLength = 0;
    srb->OriginalRequest = FlushContext->CurrIrp;

    //
    // Make sure that this srb does not get freed
    //
    SET_FLAG(srb->SrbFlags, SRB_CLASS_FLAGS_PERSISTANT);

    //
    // Make sure that this request does not get retried
    //
    irpSp = IoGetCurrentIrpStackLocation(FlushContext->CurrIrp);

    irpSp->Parameters.Others.Argument4 = (PVOID) 0;

    //
    // Fill in the irp fields appropriately
    //
    irpSp = IoGetNextIrpStackLocation(FlushContext->CurrIrp);

    irpSp->MajorFunction       = IRP_MJ_SCSI;
    irpSp->Parameters.Scsi.Srb = srb;

    IoSetCompletionRoutine(FlushContext->CurrIrp, DiskFlushComplete, FlushContext, TRUE, TRUE, TRUE);

    //
    // Send down the flush request
    //
    IoCallDriver(((PCOMMON_DEVICE_EXTENSION)fdoExt)->LowerDeviceObject, FlushContext->CurrIrp);
}


NTSTATUS
DiskFlushComplete(
    IN PDEVICE_OBJECT Fdo,
    IN PIRP Irp,
    IN PVOID Context
    )

/*++

Routine Description:

    This completion routine is a wrapper around ClassIoComplete. It
    will complete all the flush requests that are tagged to it, set
    an event to signal the next group to proceed and return

Arguments:

    Fdo - The device object which requested the completion routine
    Irp - The irp that is being completed
    Context - The flush group context

Return Value:

    STATUS_SUCCESS if successful, an error code otherwise

--*/

{
    PDISK_GROUP_CONTEXT FlushContext = Context;
    NTSTATUS status;

    DebugPrint((ClassDebugInfo, "DiskFlushComplete: %p %p %p\n", Fdo, Irp, FlushContext));

    //
    // Make sure everything is in order
    //
    ASSERT(Irp == FlushContext->CurrIrp);

    status = ClassIoComplete(Fdo, Irp, &FlushContext->Srb);

    //
    // Make sure that ClassIoComplete did not decide to retry this request
    //
    ASSERT(status != STATUS_MORE_PROCESSING_REQUIRED);

    //
    // Complete the flush requests tagged to this one
    //

    while (!IsListEmpty(&FlushContext->CurrList)) {

        PLIST_ENTRY listEntry = RemoveHeadList(&FlushContext->CurrList);
        PIRP tempIrp = CONTAINING_RECORD(listEntry, IRP, Tail.Overlay.ListEntry);

        InitializeListHead(&tempIrp->Tail.Overlay.ListEntry);
        tempIrp->IoStatus = Irp->IoStatus;

        ClassReleaseRemoveLock(Fdo, tempIrp);
        ClassCompleteRequest(Fdo, tempIrp, IO_NO_INCREMENT);
    }

    //
    // Notify the next group's representative that it may go ahead now
    //
    KeSetEvent(&FlushContext->Event, IO_NO_INCREMENT, FALSE);

    return status;
}


NTSTATUS
DiskModeSelect(
    IN PDEVICE_OBJECT Fdo,
    IN PCHAR ModeSelectBuffer,
    IN ULONG Length,
    IN BOOLEAN SavePage
    )

/*++

Routine Description:

    This routine sends a mode select command.

Arguments:

    DeviceObject - Supplies the device object associated with this request.

    ModeSelectBuffer - Supplies a buffer containing the page data.

    Length - Supplies the length in bytes of the mode select buffer.

    SavePage - Indicates that parameters should be written to disk.

Return Value:

    Length of the transferred data is returned.

--*/

{
    PFUNCTIONAL_DEVICE_EXTENSION fdoExtension = Fdo->DeviceExtension;
    PCDB cdb;
    SCSI_REQUEST_BLOCK srb = {0};
    ULONG retries = 1;
    ULONG length2;
    NTSTATUS status;
    PULONG buffer;
    PMODE_PARAMETER_BLOCK blockDescriptor;

    PAGED_CODE();

    ASSERT_FDO(Fdo);

    length2 = Length + sizeof(MODE_PARAMETER_HEADER) + sizeof(MODE_PARAMETER_BLOCK);

    //
    // Allocate buffer for mode select header, block descriptor, and mode page.
    //

    buffer = ExAllocatePoolWithTag(NonPagedPoolCacheAligned,
                                   length2,
                                   DISK_TAG_MODE_DATA);

    if(buffer == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory(buffer, length2);

    //
    // Set length in header to size of mode page.
    //

    ((PMODE_PARAMETER_HEADER)buffer)->BlockDescriptorLength = sizeof(MODE_PARAMETER_BLOCK);

    (PULONG)blockDescriptor = (buffer + 1);

    //
    // Set size
    //

    blockDescriptor->BlockLength[1]=0x02;

    //
    // Copy mode page to buffer.
    //

    RtlCopyMemory(buffer + 3, ModeSelectBuffer, Length);

    //
    // Build the MODE SELECT CDB.
    //

    srb.CdbLength = 6;
    cdb = (PCDB)srb.Cdb;

    //
    // Set timeout value from device extension.
    //

    srb.TimeOutValue = fdoExtension->TimeOutValue * 2;

    cdb->MODE_SELECT.OperationCode = SCSIOP_MODE_SELECT;
    cdb->MODE_SELECT.SPBit = SavePage;
    cdb->MODE_SELECT.PFBit = 1;
    cdb->MODE_SELECT.ParameterListLength = (UCHAR)(length2);

Retry:

    status = ClassSendSrbSynchronous(Fdo,
                                     &srb,
                                     buffer,
                                     length2,
                                     TRUE);

    if (status == STATUS_VERIFY_REQUIRED) {

        //
        // Routine ClassSendSrbSynchronous does not retry requests returned with
        // this status.
        //

        if (retries--) {

            //
            // Retry request.
            //

            goto Retry;
        }

    } else if (SRB_STATUS(srb.SrbStatus) == SRB_STATUS_DATA_OVERRUN) {
        status = STATUS_SUCCESS;
    }

    ExFreePool(buffer);

    return status;
} // end DiskModeSelect()


//
// This routine is structured as a work-item routine
//
VOID
DisableWriteCache(
    IN PDEVICE_OBJECT Fdo,
    IN PIO_WORKITEM WorkItem
    )

{
    PFUNCTIONAL_DEVICE_EXTENSION fdoExtension = Fdo->DeviceExtension;
    DISK_CACHE_INFORMATION cacheInfo = { 0 };
    NTSTATUS status;

    PAGED_CODE();

    ASSERT_FDO(Fdo);

    status = DiskGetCacheInformation(fdoExtension, &cacheInfo);

    if (NT_SUCCESS(status) && (cacheInfo.WriteCacheEnabled == TRUE)) {

        cacheInfo.WriteCacheEnabled = FALSE;

        DiskSetCacheInformation(fdoExtension, &cacheInfo);
    }

    IoFreeWorkItem(WorkItem);
}


//
// This routine is structured as a work-item routine
//
VOID
DiskIoctlVerify(
    IN PDEVICE_OBJECT Fdo,
    IN PDISK_VERIFY_WORKITEM_CONTEXT Context
    )

{
    PIRP Irp = Context->Irp;
    PFUNCTIONAL_DEVICE_EXTENSION FdoExtension = Fdo->DeviceExtension;
    PDISK_DATA DiskData = (PDISK_DATA)FdoExtension->CommonExtension.DriverData;
    PVERIFY_INFORMATION verifyInfo = Irp->AssociatedIrp.SystemBuffer;
    PSCSI_REQUEST_BLOCK Srb = Context->Srb;
    PCDB Cdb = (PCDB)Srb->Cdb;
    LARGE_INTEGER byteOffset;
    LARGE_INTEGER sectorOffset;
    ULONG sectorCount;
    NTSTATUS status = STATUS_SUCCESS;

    PAGED_CODE();

    ASSERT(FdoExtension->CommonExtension.IsFdo);

    //
    // We don't need to hold on to this memory as
    // the following operation may take some time
    //

    IoFreeWorkItem(Context->WorkItem);

    DebugPrint((1, "Disk.DiskIoctlVerify: Spliting up the request\n"));

    //
    // Add disk offset to starting the sector
    //

    byteOffset.QuadPart = FdoExtension->CommonExtension.StartingOffset.QuadPart +
                          verifyInfo->StartingOffset.QuadPart;

    //
    // Convert byte offset to the sector offset
    //

    sectorOffset.QuadPart = byteOffset.QuadPart >> FdoExtension->SectorShift;

    //
    // Convert byte count to sector count.
    //

    sectorCount = verifyInfo->Length >> FdoExtension->SectorShift;

    //
    // Make sure  that all previous verify requests have indeed completed
    // This greatly reduces the possibility of a Denial-of-Service attack
    //

    KeWaitForMutexObject(&DiskData->VerifyMutex,
                         Executive,
                         KernelMode,
                         FALSE,
                         NULL);

    while (NT_SUCCESS(status) && (sectorCount != 0))
    {
        USHORT numSectors = (USHORT) min(sectorCount, MAX_SECTORS_PER_VERIFY);

        RtlZeroMemory(Srb, SCSI_REQUEST_BLOCK_SIZE);

        if (TEST_FLAG(FdoExtension->DeviceFlags, DEV_USE_16BYTE_CDB)) {

            Srb->CdbLength = 16;
            Cdb->CDB10.OperationCode = SCSIOP_VERIFY16;

            REVERSE_BYTES_QUAD(&Cdb->CDB16.LogicalBlock, &sectorOffset);
            REVERSE_BYTES_SHORT(&Cdb->CDB16.TransferLength[2], &numSectors);
        } else {

            Srb->CdbLength = 10;

            Cdb->CDB10.OperationCode = SCSIOP_VERIFY;

            //
            // Move little endian values into CDB in big endian format
            //

            Cdb->CDB10.LogicalBlockByte0 = ((PFOUR_BYTE)&sectorOffset)->Byte3;
            Cdb->CDB10.LogicalBlockByte1 = ((PFOUR_BYTE)&sectorOffset)->Byte2;
            Cdb->CDB10.LogicalBlockByte2 = ((PFOUR_BYTE)&sectorOffset)->Byte1;
            Cdb->CDB10.LogicalBlockByte3 = ((PFOUR_BYTE)&sectorOffset)->Byte0;

            Cdb->CDB10.TransferBlocksMsb = ((PFOUR_BYTE)&numSectors)->Byte1;
            Cdb->CDB10.TransferBlocksLsb = ((PFOUR_BYTE)&numSectors)->Byte0;
        }

        //
        // Calculate the request timeout value based
        // on  the number of sectors  being verified
        //

        Srb->TimeOutValue = ((numSectors + 0x7F) >> 7) * FdoExtension->TimeOutValue;

        status = ClassSendSrbSynchronous(Fdo,
                                         Srb,
                                         NULL,
                                         0,
                                         FALSE);

        ASSERT(status != STATUS_NONEXISTENT_SECTOR);

        sectorCount  -= numSectors;
        sectorOffset.QuadPart += numSectors;
    }

    KeReleaseMutex(&DiskData->VerifyMutex, FALSE);

    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = 0;

    ClassReleaseRemoveLock(Fdo, Irp);
    ClassCompleteRequest(Fdo, Irp, IO_NO_INCREMENT);

    ExFreePool(Srb);
    ExFreePool(Context);
}


VOID
DiskFdoProcessError(
    PDEVICE_OBJECT Fdo,
    PSCSI_REQUEST_BLOCK Srb,
    NTSTATUS *Status,
    BOOLEAN *Retry
    )

/*++

Routine Description:

   This routine checks the type of error.  If the error indicates an underrun
   then indicate the request should be retried.

Arguments:

    Fdo - Supplies a pointer to the functional device object.

    Srb - Supplies a pointer to the failing Srb.

    Status - Status with which the IRP will be completed.

    Retry - Indication of whether the request will be retried.

Return Value:

    None.

--*/

{
    PFUNCTIONAL_DEVICE_EXTENSION fdoExtension = Fdo->DeviceExtension;
    PCDB cdb = (PCDB)(Srb->Cdb);

    ASSERT(fdoExtension->CommonExtension.IsFdo);

    if (*Status == STATUS_DATA_OVERRUN &&
        (IS_SCSIOP_READWRITE(cdb->CDB10.OperationCode))) {

            *Retry = TRUE;

            //
            // Update the error count for the device.
            //

            fdoExtension->ErrorCount++;

    } else if (SRB_STATUS(Srb->SrbStatus) == SRB_STATUS_ERROR &&
               Srb->ScsiStatus == SCSISTAT_BUSY) {

        //
        // a disk drive should never be busy this long. Reset the scsi bus
        // maybe this will clear the condition.
        //

        ResetBus(Fdo);

        //
        // Update the error count for the device.
        //

        fdoExtension->ErrorCount++;

    } else {

        BOOLEAN invalidatePartitionTable = FALSE;
        BOOLEAN bForceBusRelations = FALSE;

        //
        // See if this might indicate that something on the drive has changed.
        //

        if ((Srb->SrbStatus & SRB_STATUS_AUTOSENSE_VALID) &&
            (Srb->SenseInfoBufferLength >=
                FIELD_OFFSET(SENSE_DATA, CommandSpecificInformation))) {

            PSENSE_DATA senseBuffer = Srb->SenseInfoBuffer;
            ULONG senseKey = senseBuffer->SenseKey & 0xf;
            ULONG asc = senseBuffer->AdditionalSenseCode;
            ULONG ascq = senseBuffer->AdditionalSenseCodeQualifier;

            switch (senseKey) {

            case SCSI_SENSE_ILLEGAL_REQUEST: {

                switch (asc) {

                    case SCSI_ADSENSE_ILLEGAL_BLOCK: {
                        if (IS_SCSIOP_READWRITE(cdb->CDB10.OperationCode)) {

                            // should only retry these if the request was
                            // truly within our expected size.  however,
                            // this check was already done in the
                            // ReadWriteVerification routine.

                            /*
                             *  Fujitsu IDE drives have been observed to
                             *  return this error transiently for a legal LBA;
                             *  manual retry in the debugger then works, so
                             *  there is a good chance that a programmed retry
                             *  will also work.
                             */
                            *Retry = TRUE;

                        }
                        break;
                    }

                    case SCSI_ADSENSE_INVALID_CDB:
                    {
                        //
                        // Look to see if this is an Io request with the ForceUnitAccess flag set
                        //
                        if (((cdb->CDB10.OperationCode == SCSIOP_WRITE)   ||
                            (cdb->CDB10.OperationCode == SCSIOP_WRITE16)) &&
                            (cdb->CDB10.ForceUnitAccess))
                        {
                            PDISK_DATA diskData = (PDISK_DATA)fdoExtension->CommonExtension.DriverData;

                            if (diskData->WriteCacheOverride == DiskWriteCacheEnable)
                            {
                                //
                                // The user has explicitly requested that write caching be turned on
                                //
                            }
                            else
                            {
                                //
                                // Turn off write caching on this device. This is so that future
                                // critical requests need not be sent down with  ForceUnitAccess
                                //
                                PIO_WORKITEM workItem = IoAllocateWorkItem(Fdo);

                                if (workItem)
                                {
                                    IoQueueWorkItem(workItem, DisableWriteCache, CriticalWorkQueue, workItem);
                                }
                            }

                            SET_FLAG(fdoExtension->ScanForSpecialFlags, CLASS_SPECIAL_FUA_NOT_SUPPORTED);

                            cdb->CDB10.ForceUnitAccess = FALSE;
                            *Retry = TRUE;
                        }

                        break;
                    }
                } // end switch(asc)
                break;
            }

            case SCSI_SENSE_NOT_READY: {

                switch (asc) {
                case SCSI_ADSENSE_LUN_NOT_READY: {
                    switch (ascq) {
                    case SCSI_SENSEQ_BECOMING_READY:
                    case SCSI_SENSEQ_MANUAL_INTERVENTION_REQUIRED:
                    case SCSI_SENSEQ_CAUSE_NOT_REPORTABLE: {
                        invalidatePartitionTable = TRUE;
                        break;
                    }
                    } // end switch(ascq)
                    break;
                }

                case SCSI_ADSENSE_NO_MEDIA_IN_DEVICE: {
                    invalidatePartitionTable = TRUE;
                    break;
                }
                } // end switch(asc)
                break;
            }

            case SCSI_SENSE_MEDIUM_ERROR: {
                invalidatePartitionTable = TRUE;
                break;
            }

            case SCSI_SENSE_HARDWARE_ERROR: {
                invalidatePartitionTable = TRUE;
                break;
            }

            case SCSI_SENSE_UNIT_ATTENTION:
            {
                invalidatePartitionTable = TRUE;

                switch (senseBuffer->AdditionalSenseCode)
                {
                    case SCSI_ADSENSE_MEDIUM_CHANGED:
                    {
                        bForceBusRelations = TRUE;
                        break;
                    }
                }
                break;
            }

            case SCSI_SENSE_RECOVERED_ERROR: {
                invalidatePartitionTable = TRUE;
                break;
            }

            } // end switch(senseKey)
        } else {

            //
            // On any exceptional scsi condition which might indicate that the
            // device was changed we will flush out the state of the partition
            // table.
            //

            switch (SRB_STATUS(Srb->SrbStatus)) {
                case SRB_STATUS_INVALID_LUN:
                case SRB_STATUS_INVALID_TARGET_ID:
                case SRB_STATUS_NO_DEVICE:
                case SRB_STATUS_NO_HBA:
                case SRB_STATUS_INVALID_PATH_ID:
                case SRB_STATUS_COMMAND_TIMEOUT:
                case SRB_STATUS_TIMEOUT:
                case SRB_STATUS_SELECTION_TIMEOUT:
                case SRB_STATUS_REQUEST_FLUSHED:
                case SRB_STATUS_UNEXPECTED_BUS_FREE:
                case SRB_STATUS_PARITY_ERROR:
                {
                    invalidatePartitionTable = TRUE;
                    break;
                }

                case SRB_STATUS_ERROR:
                {
                    if (Srb->ScsiStatus == SCSISTAT_RESERVATION_CONFLICT)
                    {
                        invalidatePartitionTable = TRUE;
                    }

                    break;
                }
            } // end switch(Srb->SrbStatus)
        }

        if (invalidatePartitionTable)
        {
            if (TEST_FLAG(Fdo->Characteristics, FILE_REMOVABLE_MEDIA) && (fdoExtension->CommonExtension.ChildList))
            {
                //
                // Locate the PDO associated with this removable device
                //

                PDEVICE_OBJECT Pdo = (fdoExtension->CommonExtension.ChildList)->CommonExtension.DeviceObject;

                if ((ClassGetVpb(Pdo) != NULL) && (ClassGetVpb(Pdo)->Flags & VPB_MOUNTED))
                {
                    //
                    // Set a flag to inform the filesystem
                    // that this volume needs verification
                    //

                    SET_FLAG(Pdo->Flags, DO_VERIFY_VOLUME);
                }
            }

            if (DiskInvalidatePartitionTable(fdoExtension, FALSE) || bForceBusRelations)
            {
                IoInvalidateDeviceRelations(fdoExtension->LowerPdo, BusRelations);
            }
        }
    }

    return;
}


VOID
DiskSetSpecialHacks(
    IN PFUNCTIONAL_DEVICE_EXTENSION FdoExtension,
    IN ULONG_PTR Data
    )

/*++

Routine Description:

    This function checks to see if an SCSI logical unit requires speical
    flags to be set.

Arguments:

    Fdo - Supplies the device object to be tested.

    InquiryData - Supplies the inquiry data returned by the device of interest.

    AdapterDescriptor - Supplies the capabilities of the device object.

Return Value:

    None.

--*/

{
    PDEVICE_OBJECT fdo = FdoExtension->DeviceObject;

    PAGED_CODE();

    DebugPrint((1, "Disk SetSpecialHacks, Setting Hacks %p\n", Data));

    //
    // Found a listed controller.  Determine what must be done.
    //

    if (TEST_FLAG(Data, HackDisableTaggedQueuing)) {

        //
        // Disable tagged queuing.
        //

        CLEAR_FLAG(FdoExtension->SrbFlags, SRB_FLAGS_QUEUE_ACTION_ENABLE);
    }

    if (TEST_FLAG(Data, HackDisableSynchronousTransfers)) {

        //
        // Disable synchronous data transfers.
        //

        SET_FLAG(FdoExtension->SrbFlags, SRB_FLAGS_DISABLE_SYNCH_TRANSFER);

    }

    if (TEST_FLAG(Data, HackDisableSpinDown)) {

        //
        // Disable spinning down of drives.
        //

        SET_FLAG(FdoExtension->ScanForSpecialFlags,
                 CLASS_SPECIAL_DISABLE_SPIN_DOWN);

    }

    if (TEST_FLAG(Data, HackDisableWriteCache)) {

        //
        // Disable the drive's write cache
        //

        SET_FLAG(FdoExtension->ScanForSpecialFlags,
                 CLASS_SPECIAL_DISABLE_WRITE_CACHE);

    }

    if (TEST_FLAG(Data, HackCauseNotReportableHack)) {

        SET_FLAG(FdoExtension->ScanForSpecialFlags,
                 CLASS_SPECIAL_CAUSE_NOT_REPORTABLE_HACK);
    }

    if (TEST_FLAG(fdo->Characteristics, FILE_REMOVABLE_MEDIA) &&
        TEST_FLAG(Data, HackRequiresStartUnitCommand)
        ) {

        //
        // this is a list of vendors who require the START_UNIT command
        //

        DebugPrint((1, "DiskScanForSpecial (%p) => This unit requires "
                    " START_UNITS\n", fdo));
        SET_FLAG(FdoExtension->DeviceFlags, DEV_SAFE_START_UNIT);

    }

    return;
}


VOID
ResetBus(
    IN PDEVICE_OBJECT Fdo
    )

/*++

Routine Description:

    This command sends a reset bus command to the SCSI port driver.

Arguments:

    Fdo - The functional device object for the logical unit with hardware problem.

Return Value:

    None.

--*/

{
    PIO_STACK_LOCATION irpStack;
    PIRP irp;

    PFUNCTIONAL_DEVICE_EXTENSION fdoExtension = Fdo->DeviceExtension;
    PSCSI_REQUEST_BLOCK srb;
    PCOMPLETION_CONTEXT context;

    DebugPrint((1, "Disk ResetBus: Sending reset bus request to port driver.\n"));

    //
    // Allocate Srb from nonpaged pool.
    //

    context = ExAllocatePoolWithTag(NonPagedPool,
                                    sizeof(COMPLETION_CONTEXT),
                                    DISK_TAG_CCONTEXT);

    if(context == NULL) {
        return;
    }

    //
    // Save the device object in the context for use by the completion
    // routine.
    //

    context->DeviceObject = Fdo;
    srb = &context->Srb;

    //
    // Zero out srb.
    //

    RtlZeroMemory(srb, SCSI_REQUEST_BLOCK_SIZE);

    //
    // Write length to SRB.
    //

    srb->Length = SCSI_REQUEST_BLOCK_SIZE;

    srb->Function = SRB_FUNCTION_RESET_BUS;

    //
    // Build the asynchronous request to be sent to the port driver.
    // Since this routine is called from a DPC the IRP should always be
    // available.
    //

    irp = IoAllocateIrp(Fdo->StackSize, FALSE);

    if(irp == NULL) {
        ExFreePool(context);
        return;
    }

    ClassAcquireRemoveLock(Fdo, irp);

    IoSetCompletionRoutine(irp,
                           (PIO_COMPLETION_ROUTINE)ClassAsynchronousCompletion,
                           context,
                           TRUE,
                           TRUE,
                           TRUE);

    irpStack = IoGetNextIrpStackLocation(irp);

    irpStack->MajorFunction = IRP_MJ_SCSI;

    srb->OriginalRequest = irp;

    //
    // Store the SRB address in next stack for port driver.
    //

    irpStack->Parameters.Scsi.Srb = srb;

    //
    // Call the port driver with the IRP.
    //

    IoCallDriver(fdoExtension->CommonExtension.LowerDeviceObject, irp);

    return;

} // end ResetBus()


NTSTATUS
DiskQueryPnpCapabilities(
    IN PDEVICE_OBJECT DeviceObject,
    IN PDEVICE_CAPABILITIES Capabilities
    )

{
    PCOMMON_DEVICE_EXTENSION commonExtension = DeviceObject->DeviceExtension;
    PDISK_DATA diskData = commonExtension->DriverData;

    PAGED_CODE();

    ASSERT(DeviceObject);
    ASSERT(Capabilities);

    if(commonExtension->IsFdo) {
        return STATUS_NOT_IMPLEMENTED;
    } else {

        PPHYSICAL_DEVICE_EXTENSION physicalExtension =
            DeviceObject->DeviceExtension;

        Capabilities->SilentInstall = 1;
        Capabilities->RawDeviceOK = 1;
        Capabilities->Address = commonExtension->PartitionNumber;

        if(!TEST_FLAG(DeviceObject->Characteristics, FILE_REMOVABLE_MEDIA)) {

            //
            // Media's not removable, deviceId/DeviceInstance should be
            // globally unique.
            //

            Capabilities->UniqueID = 1;
        } else {
            Capabilities->UniqueID = 0;
        }
    }

    return STATUS_SUCCESS;
}


NTSTATUS
DiskGetCacheInformation(
    IN PFUNCTIONAL_DEVICE_EXTENSION FdoExtension,
    IN PDISK_CACHE_INFORMATION CacheInfo
    )

{
    PMODE_PARAMETER_HEADER modeData;
    PMODE_CACHING_PAGE pageData;

    ULONG length;

    NTSTATUS status;

    PAGED_CODE();

    modeData = ExAllocatePoolWithTag(NonPagedPoolCacheAligned,
                                     MODE_DATA_SIZE,
                                     DISK_TAG_DISABLE_CACHE);

    if (modeData == NULL) {

        DebugPrint((1, "DiskGetSetCacheInformation: Unable to allocate mode "
                       "data buffer\n"));
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory(modeData, MODE_DATA_SIZE);

    length = ClassModeSense(FdoExtension->DeviceObject,
                            (PUCHAR) modeData,
                            MODE_DATA_SIZE,
                            MODE_SENSE_RETURN_ALL);

    if (length < sizeof(MODE_PARAMETER_HEADER)) {

        //
        // Retry the request in case of a check condition.
        //

        length = ClassModeSense(FdoExtension->DeviceObject,
                                (PUCHAR) modeData,
                                MODE_DATA_SIZE,
                                MODE_SENSE_RETURN_ALL);

        if (length < sizeof(MODE_PARAMETER_HEADER)) {


            DebugPrint((1, "Disk.DisableWriteCache: Mode Sense failed\n"));

            ExFreePool(modeData);
            return STATUS_IO_DEVICE_ERROR;
        }
    }

    //
    // If the length is greater than length indicated by the mode data reset
    // the data to the mode data.
    //

    if (length > (ULONG) (modeData->ModeDataLength + 1)) {
        length = modeData->ModeDataLength + 1;
    }

    //
    // Check to see if the write cache is enabled.
    //

    pageData = ClassFindModePage((PUCHAR) modeData,
                                 length,
                                 MODE_PAGE_CACHING,
                                 TRUE);

    //
    // Check if valid caching page exists.
    //

    if (pageData == NULL) {
        ExFreePool(modeData);
        return STATUS_NOT_SUPPORTED;
    }

    //
    // Copy the parameters over.
    //

    RtlZeroMemory(CacheInfo, sizeof(DISK_CACHE_INFORMATION));

    CacheInfo->ParametersSavable = pageData->PageSavable;

    CacheInfo->ReadCacheEnabled = !(pageData->ReadDisableCache);
    CacheInfo->WriteCacheEnabled = pageData->WriteCacheEnable;


    //
    // Translate the values in the mode page into the ones defined in
    // ntdddisk.h.
    //

    CacheInfo->ReadRetentionPriority =
        TRANSLATE_RETENTION_PRIORITY(pageData->ReadRetensionPriority);
    CacheInfo->WriteRetentionPriority =
        TRANSLATE_RETENTION_PRIORITY(pageData->WriteRetensionPriority);

    CacheInfo->DisablePrefetchTransferLength =
        ((pageData->DisablePrefetchTransfer[0] << 8) +
         pageData->DisablePrefetchTransfer[1]);

    CacheInfo->ScalarPrefetch.Minimum =
        ((pageData->MinimumPrefetch[0] << 8) + pageData->MinimumPrefetch[1]);

    CacheInfo->ScalarPrefetch.Maximum =
        ((pageData->MaximumPrefetch[0] << 8) + pageData->MaximumPrefetch[1]);

    if(pageData->MultiplicationFactor) {
        CacheInfo->PrefetchScalar = TRUE;
        CacheInfo->ScalarPrefetch.MaximumBlocks =
            ((pageData->MaximumPrefetchCeiling[0] << 8) +
             pageData->MaximumPrefetchCeiling[1]);
    }

    ExFreePool(modeData);
    return STATUS_SUCCESS;
}


NTSTATUS
DiskSetCacheInformation(
    IN PFUNCTIONAL_DEVICE_EXTENSION FdoExtension,
    IN PDISK_CACHE_INFORMATION CacheInfo
    )
{
    PMODE_PARAMETER_HEADER modeData;
    ULONG length;
    PMODE_CACHING_PAGE pageData;
    ULONG i;
    NTSTATUS status;

    PAGED_CODE();

    modeData = ExAllocatePoolWithTag(NonPagedPoolCacheAligned,
                                     MODE_DATA_SIZE,
                                     DISK_TAG_DISABLE_CACHE);

    if (modeData == NULL) {

        DebugPrint((1, "DiskSetCacheInformation: Unable to allocate mode "
                       "data buffer\n"));
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory(modeData, MODE_DATA_SIZE);

    length = ClassModeSense(FdoExtension->DeviceObject,
                            (PUCHAR) modeData,
                            MODE_DATA_SIZE,
                            MODE_PAGE_CACHING);

    if (length < sizeof(MODE_PARAMETER_HEADER)) {

        //
        // Retry the request in case of a check condition.
        //

        length = ClassModeSense(FdoExtension->DeviceObject,
                                (PUCHAR) modeData,
                                MODE_DATA_SIZE,
                                MODE_PAGE_CACHING);

        if (length < sizeof(MODE_PARAMETER_HEADER)) {


            DebugPrint((1, "Disk.DisableWriteCache: Mode Sense failed\n"));

            ExFreePool(modeData);
            return STATUS_IO_DEVICE_ERROR;
        }
    }

    //
    // If the length is greater than length indicated by the mode data reset
    // the data to the mode data.
    //

    if (length > (ULONG) (modeData->ModeDataLength + 1)) {
        length = modeData->ModeDataLength + 1;
    }

    //
    // Check to see if the write cache is enabled.
    //

    pageData = ClassFindModePage((PUCHAR) modeData,
                                 length,
                                 MODE_PAGE_CACHING,
                                 TRUE);

    //
    // Check if valid caching page exists.
    //

    if (pageData == NULL) {
        ExFreePool(modeData);
        return STATUS_NOT_SUPPORTED;
    }

    //
    // Don't touch any of the normal parameters - not all drives actually
    // use the correct size of caching mode page.  Just change the things
    // which the user could have modified.
    //

    pageData->PageSavable = FALSE;

    pageData->ReadDisableCache = !(CacheInfo->ReadCacheEnabled);
    pageData->MultiplicationFactor = CacheInfo->PrefetchScalar;
    pageData->WriteCacheEnable = CacheInfo->WriteCacheEnabled;

    pageData->WriteRetensionPriority = (UCHAR)
        TRANSLATE_RETENTION_PRIORITY(CacheInfo->WriteRetentionPriority);
    pageData->ReadRetensionPriority = (UCHAR)
        TRANSLATE_RETENTION_PRIORITY(CacheInfo->ReadRetentionPriority);

    pageData->DisablePrefetchTransfer[0] =
        (UCHAR) (CacheInfo->DisablePrefetchTransferLength >> 8);
    pageData->DisablePrefetchTransfer[1] =
        (UCHAR) (CacheInfo->DisablePrefetchTransferLength & 0x00ff);

    pageData->MinimumPrefetch[0] =
        (UCHAR) (CacheInfo->ScalarPrefetch.Minimum >> 8);
    pageData->MinimumPrefetch[1] =
        (UCHAR) (CacheInfo->ScalarPrefetch.Minimum & 0x00ff);

    pageData->MaximumPrefetch[0] =
        (UCHAR) (CacheInfo->ScalarPrefetch.Maximum >> 8);
    pageData->MaximumPrefetch[1] =
        (UCHAR) (CacheInfo->ScalarPrefetch.Maximum & 0x00ff);

    if(pageData->MultiplicationFactor) {

        pageData->MaximumPrefetchCeiling[0] =
            (UCHAR) (CacheInfo->ScalarPrefetch.MaximumBlocks >> 8);
        pageData->MaximumPrefetchCeiling[1] =
            (UCHAR) (CacheInfo->ScalarPrefetch.MaximumBlocks & 0x00ff);
    }

    //
    // We will attempt (twice) to issue the mode select with the page.
    //

    for (i = 0; i < 2; i++) {

        status = DiskModeSelect(FdoExtension->DeviceObject,
                                (PUCHAR) pageData,
                                (pageData->PageLength + 2),
                                CacheInfo->ParametersSavable);

        if (NT_SUCCESS(status)) {

            if (CacheInfo->WriteCacheEnabled)
            {
                SET_FLAG(FdoExtension->DeviceFlags, DEV_WRITE_CACHE);
            }
            else
            {
                CLEAR_FLAG(FdoExtension->DeviceFlags, DEV_WRITE_CACHE);
            }

            break;
        }
    }

    if (!NT_SUCCESS(status))
    {
        //
        // We were unable to modify the disk write cache setting
        //

        SET_FLAG(FdoExtension->ScanForSpecialFlags, CLASS_SPECIAL_MODIFY_CACHE_UNSUCCESSFUL);
    }

    ExFreePool(modeData);
    return status;
}


VOID
DiskLogCacheInformation(
    IN PFUNCTIONAL_DEVICE_EXTENSION FdoExtension,
    IN PDISK_CACHE_INFORMATION CacheInfo,
    IN NTSTATUS Status
    )
{
    PIO_ERROR_LOG_PACKET logEntry = NULL;

    PAGED_CODE();

    logEntry = IoAllocateErrorLogEntry(FdoExtension->DeviceObject, sizeof(IO_ERROR_LOG_PACKET) + (4 * sizeof(ULONG)));

    if (logEntry != NULL)
    {
        PDISK_DATA diskData = FdoExtension->CommonExtension.DriverData;
        BOOLEAN bIsEnabled  = TEST_FLAG(FdoExtension->DeviceFlags, DEV_WRITE_CACHE);

        logEntry->FinalStatus       = Status;
        logEntry->ErrorCode         = (bIsEnabled) ? IO_WRITE_CACHE_ENABLED : IO_WRITE_CACHE_DISABLED;
        logEntry->SequenceNumber    = 0;
        logEntry->MajorFunctionCode = IRP_MJ_SCSI;
        logEntry->IoControlCode     = 0;
        logEntry->RetryCount        = 0;
        logEntry->UniqueErrorValue  = 0x1;
        logEntry->DumpDataSize      = 4;

        logEntry->DumpData[0] = diskData->ScsiAddress.PathId;
        logEntry->DumpData[1] = diskData->ScsiAddress.TargetId;
        logEntry->DumpData[2] = diskData->ScsiAddress.Lun;
        logEntry->DumpData[3] = CacheInfo->WriteCacheEnabled;

        //
        // Write the error log packet.
        //

        IoWriteErrorLogEntry(logEntry);
    }
}


NTSTATUS
DiskIoctlGetCacheSetting(
    IN PFUNCTIONAL_DEVICE_EXTENSION FdoExtension,
    IN PIRP Irp
    )

/*++

Routine description:

    This routine services IOCTL_DISK_GET_CACHE_SETTING. It looks to
    see if there are any issues with the disk cache and whether the
    user had previously indicated that the cache is power-protected

Arguments:

    Fdo - The functional device object processing the request
    Irp - The ioctl to be processed

Return Value:

    STATUS_SUCCESS if successful, an error code otherwise

--*/

{
    PIO_STACK_LOCATION irpSp = IoGetCurrentIrpStackLocation(Irp);
    NTSTATUS status = STATUS_SUCCESS;

    PAGED_CODE();

    if (irpSp->Parameters.DeviceIoControl.OutputBufferLength < sizeof(DISK_CACHE_SETTING))
    {
        status = STATUS_BUFFER_TOO_SMALL;
    }
    else
    {
        PDISK_CACHE_SETTING cacheSetting = (PDISK_CACHE_SETTING)Irp->AssociatedIrp.SystemBuffer;

        cacheSetting->Version = sizeof(DISK_CACHE_SETTING);
        cacheSetting->State   = DiskCacheNormal;

        //
        // Determine whether it is safe to turn on the cache
        //
        if (TEST_FLAG(FdoExtension->ScanForSpecialFlags, CLASS_SPECIAL_FUA_NOT_SUPPORTED))
        {
            cacheSetting->State = DiskCacheWriteThroughNotSupported;
        }

        //
        // Determine whether it is possible to modify the cache setting
        //
        if (TEST_FLAG(FdoExtension->ScanForSpecialFlags, CLASS_SPECIAL_MODIFY_CACHE_UNSUCCESSFUL))
        {
            cacheSetting->State = DiskCacheModifyUnsuccessful;
        }

        cacheSetting->IsPowerProtected = TEST_FLAG(FdoExtension->DeviceFlags, DEV_POWER_PROTECTED);

        Irp->IoStatus.Information = sizeof(DISK_CACHE_SETTING);
    }

    return status;
}


NTSTATUS
DiskIoctlSetCacheSetting(
    IN PFUNCTIONAL_DEVICE_EXTENSION FdoExtension,
    IN PIRP Irp
    )

/*++

Routine description:

    This routine services IOCTL_DISK_SET_CACHE_SETTING. It allows
    the user to specify whether the disk cache is power-protected
    or not

Arguments:

    Fdo - The functional device object processing the request
    Irp - The ioctl to be processed

Return Value:

    STATUS_SUCCESS if successful, an error code otherwise

--*/

{
    PIO_STACK_LOCATION irpSp = IoGetCurrentIrpStackLocation(Irp);
    NTSTATUS status = STATUS_SUCCESS;

    PAGED_CODE();

    if (irpSp->Parameters.DeviceIoControl.InputBufferLength < sizeof(DISK_CACHE_SETTING))
    {
        status = STATUS_INFO_LENGTH_MISMATCH;
    }
    else
    {
        PDISK_CACHE_SETTING cacheSetting = (PDISK_CACHE_SETTING)Irp->AssociatedIrp.SystemBuffer;

        if (cacheSetting->Version == sizeof(DISK_CACHE_SETTING))
        {
            ULONG isPowerProtected;

            //
            // Save away the user-defined override in our extension and the registry
            //
            if (cacheSetting->IsPowerProtected)
            {
                SET_FLAG(FdoExtension->DeviceFlags, DEV_POWER_PROTECTED);
                isPowerProtected = 1;
            }
            else
            {
                CLEAR_FLAG(FdoExtension->DeviceFlags, DEV_POWER_PROTECTED);
                isPowerProtected = 0;
            }

            ClassSetDeviceParameter(FdoExtension, DiskDeviceParameterSubkey, DiskDeviceCacheIsPowerProtected, isPowerProtected);
        }
        else
        {
            status = STATUS_INVALID_PARAMETER;
        }
    }

    return status;
}


PPARTITION_INFORMATION_EX
DiskPdoFindPartitionEntry(
    IN PPHYSICAL_DEVICE_EXTENSION Pdo,
    IN PDRIVE_LAYOUT_INFORMATION_EX LayoutInfo
    )

{
    PCOMMON_DEVICE_EXTENSION commonExtension= &(Pdo->CommonExtension);
    ULONG partitionIndex;

    PAGED_CODE();

    DebugPrint((1, "DiskPdoFindPartitionEntry: Searching layout for "
                   "matching partition.\n"));

    for(partitionIndex = 0;
        partitionIndex < LayoutInfo->PartitionCount;
        partitionIndex++) {

        PPARTITION_INFORMATION_EX partitionInfo;

        //
        // Get the partition entry
        //

        partitionInfo = &LayoutInfo->PartitionEntry[partitionIndex];

        //
        // See if it is the one we are looking for...
        //

        if( LayoutInfo->PartitionStyle == PARTITION_STYLE_MBR &&
            (partitionInfo->Mbr.PartitionType == PARTITION_ENTRY_UNUSED ||
             IsContainerPartition(partitionInfo->Mbr.PartitionType)) ) {

            continue;
        }

        if( LayoutInfo->PartitionStyle == PARTITION_STYLE_GPT &&
            DiskCompareGuid (&partitionInfo->Gpt.PartitionType, &GUID_NULL) == 00) {

            continue;
        }

        if( (commonExtension->StartingOffset.QuadPart ==
             partitionInfo->StartingOffset.QuadPart) &&
            (commonExtension->PartitionLength.QuadPart ==
             partitionInfo->PartitionLength.QuadPart)) {

            //
            // Found it!
            //

            DebugPrint((1, "DiskPdoFindPartitionEntry: Found matching "
                           "partition.\n"));
            return partitionInfo;
        }
    }

    return NULL;
}


PPARTITION_INFORMATION_EX
DiskFindAdjacentPartition(
    IN PDRIVE_LAYOUT_INFORMATION_EX LayoutInfo,
    IN PPARTITION_INFORMATION_EX BasePartition
    )
{
    ULONG partitionIndex;
    LONGLONG baseStoppingOffset;
    LONGLONG adjacentStartingOffset;
    PPARTITION_INFORMATION_EX adjacentPartition = 0;

    ASSERT(LayoutInfo && BasePartition);

    PAGED_CODE();

    DebugPrint((1, "DiskPdoFindAdjacentPartition: Searching layout for adjacent partition.\n"));

    //
    // Construct the base stopping offset for comparison
    //

    baseStoppingOffset = (BasePartition->StartingOffset.QuadPart +
                          BasePartition->PartitionLength.QuadPart -
                          1);

    adjacentStartingOffset = MAXLONGLONG;

    for(partitionIndex = 0;
        partitionIndex < LayoutInfo->PartitionCount;
        partitionIndex++) {

        PPARTITION_INFORMATION_EX partitionInfo;

        //
        // Get the partition entry
        //

        partitionInfo = &LayoutInfo->PartitionEntry[partitionIndex];

        //
        // See if it is the one we are looking for...
        //

        if( LayoutInfo->PartitionStyle == PARTITION_STYLE_MBR &&
            partitionInfo->Mbr.PartitionType == PARTITION_ENTRY_UNUSED ) {

            continue;
        }

        if( LayoutInfo->PartitionStyle == PARTITION_STYLE_GPT &&
            DiskCompareGuid (&partitionInfo->Gpt.PartitionType, &GUID_NULL) == 00 ) {

            continue;
        }


        if((partitionInfo->StartingOffset.QuadPart > baseStoppingOffset) &&
           (partitionInfo->StartingOffset.QuadPart < adjacentStartingOffset)) {

            // Found a closer neighbor...update and remember.
            adjacentPartition = partitionInfo;

            adjacentStartingOffset = adjacentPartition->StartingOffset.QuadPart;

            DebugPrint((1, "DiskPdoFindAdjacentPartition: Found adjacent "
                           "partition.\n"));
        }
    }
    return adjacentPartition;
}


PPARTITION_INFORMATION_EX
DiskFindContainingPartition(
    IN PDRIVE_LAYOUT_INFORMATION_EX LayoutInfo,
    IN PPARTITION_INFORMATION_EX BasePartition,
    IN BOOLEAN SearchTopToBottom
    )

{

    LONG partitionIndex;
    LONG startIndex;
    LONG stopIndex;
    LONG stepIndex;

    LONGLONG baseStoppingOffset;
    LONGLONG containerStoppingOffset;

    PPARTITION_INFORMATION_EX partitionInfo = 0;
    PPARTITION_INFORMATION_EX containerPartition = 0;

    PAGED_CODE();

    ASSERT( LayoutInfo && BasePartition);

    DebugPrint((1, "DiskFindContainingPartition: Searching for extended partition.\n"));

    if( LayoutInfo->PartitionCount != 0) {

        baseStoppingOffset = (BasePartition->StartingOffset.QuadPart +
                              BasePartition->PartitionLength.QuadPart - 1);

        //
        // Determine the search direction and setup the loop
        //
        if(SearchTopToBottom == TRUE) {

            startIndex = 0;
            stopIndex = LayoutInfo->PartitionCount;
            stepIndex = +1;
        } else {
            startIndex = LayoutInfo->PartitionCount - 1;
            stopIndex = -1;
            stepIndex = -1;
        }

        //
        // Using the loop parameters, walk the layout information and
        // return the first containing partition.
        //

        for(partitionIndex = startIndex;
            partitionIndex != stopIndex;
            partitionIndex += stepIndex) {

            //
            // Get the next partition entry
            //

            partitionInfo = &LayoutInfo->PartitionEntry[partitionIndex];

            containerStoppingOffset = (partitionInfo->StartingOffset.QuadPart +
                                       partitionInfo->PartitionLength.QuadPart -
                                       1);

            //
            // Search for a containing partition without detecting the
            // same partition as a container of itself.  The starting
            // offset of a partition and its container should never be
            // the same; however, the stopping offset can be the same.
            //

            //
            // NOTE: Container partitions are MBR only.
            //

            if((LayoutInfo->PartitionStyle == PARTITION_STYLE_MBR) &&
                (IsContainerPartition(partitionInfo->Mbr.PartitionType)) &&
               (BasePartition->StartingOffset.QuadPart >
                partitionInfo->StartingOffset.QuadPart) &&
               (baseStoppingOffset <= containerStoppingOffset)) {

                containerPartition = partitionInfo;

                DebugPrint((1, "DiskFindContainingPartition: Found a "
                               "containing extended partition.\n"));

                break;
            }
        }
    }

    return containerPartition;
}


NTSTATUS
DiskGetInfoExceptionInformation(
    IN PFUNCTIONAL_DEVICE_EXTENSION FdoExtension,
    IN PMODE_INFO_EXCEPTIONS ReturnPageData
    )
{
    PMODE_PARAMETER_HEADER modeData;
    PMODE_INFO_EXCEPTIONS pageData;
    ULONG length;

    NTSTATUS status;

    PAGED_CODE();

    //
    // ReturnPageData is allocated by the caller
    //

    modeData = ExAllocatePoolWithTag(NonPagedPoolCacheAligned,
                                     MODE_DATA_SIZE,
                                     DISK_TAG_INFO_EXCEPTION);

    if (modeData == NULL) {

        DebugPrint((1, "DiskGetInfoExceptionInformation: Unable to allocate mode "
                       "data buffer\n"));
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory(modeData, MODE_DATA_SIZE);

    length = ClassModeSense(FdoExtension->DeviceObject,
                            (PUCHAR) modeData,
                            MODE_DATA_SIZE,
                            MODE_PAGE_FAULT_REPORTING);

    if (length < sizeof(MODE_PARAMETER_HEADER)) {

        //
        // Retry the request in case of a check condition.
        //

        length = ClassModeSense(FdoExtension->DeviceObject,
                                (PUCHAR) modeData,
                                MODE_DATA_SIZE,
                                MODE_PAGE_FAULT_REPORTING);

        if (length < sizeof(MODE_PARAMETER_HEADER)) {


            DebugPrint((1, "Disk.DisableWriteCache: Mode Sense failed\n"));

            ExFreePool(modeData);
            return STATUS_IO_DEVICE_ERROR;
        }
    }

    //
    // If the length is greater than length indicated by the mode data reset
    // the data to the mode data.
    //

    if (length > (ULONG) (modeData->ModeDataLength + 1)) {
        length = modeData->ModeDataLength + 1;
    }

    //
    // Find the mode page for info exceptions
    //

    pageData = ClassFindModePage((PUCHAR) modeData,
                                 length,
                                 MODE_PAGE_FAULT_REPORTING,
                                 TRUE);

    if (pageData != NULL) {
        RtlCopyMemory(ReturnPageData, pageData, sizeof(MODE_INFO_EXCEPTIONS));
        status =  STATUS_SUCCESS;
    } else {
        status = STATUS_NOT_SUPPORTED;
    }

    DebugPrint((3, "DiskGetInfoExceptionInformation: %s support SMART for device %x\n",
                  NT_SUCCESS(status) ? "does" : "does not",
                  FdoExtension->DeviceObject));


    ExFreePool(modeData);
    return(status);
}


NTSTATUS
DiskSetInfoExceptionInformation(
    IN PFUNCTIONAL_DEVICE_EXTENSION FdoExtension,
    IN PMODE_INFO_EXCEPTIONS PageData
    )

{
    ULONG i;
    NTSTATUS status;

    PAGED_CODE();

    //
    // We will attempt (twice) to issue the mode select with the page.
    // Make the setting persistant so that we don't have to turn it back
    // on after a bus reset.
    //

    for (i = 0; i < 2; i++)
    {
        status = DiskModeSelect(FdoExtension->DeviceObject,
                                (PUCHAR) PageData,
                                sizeof(MODE_INFO_EXCEPTIONS),
                                TRUE);

    }

    DebugPrint((3, "DiskSetInfoExceptionInformation: %s for device %p\n",
                        NT_SUCCESS(status) ? "succeeded" : "failed",
                        FdoExtension->DeviceObject));

    return status;
}


NTSTATUS
DiskIoctlCreateDisk(
    IN OUT PDEVICE_OBJECT DeviceObject,
    IN OUT PIRP Irp
    )

/*++

Routine Description:

    Handler for IOCTL_DISK_CREATE_DISK ioctl.

Arguments:

    DeviceObject - Device object representing a disk that will be created or
            erased.

    Irp - The IRP for this request.

Return Values:

    NTSTATUS code.

--*/

{
    NTSTATUS status;
    PCOMMON_DEVICE_EXTENSION commonExtension;
    PFUNCTIONAL_DEVICE_EXTENSION fdoExtension;
    PIO_STACK_LOCATION irpStack;
    PDISK_DATA diskData;
    PCREATE_DISK createDiskInfo;


    PAGED_CODE ();

    ASSERT ( DeviceObject != NULL );
    ASSERT ( Irp != NULL );

    //
    // Initialization
    //

    commonExtension = DeviceObject->DeviceExtension;
    fdoExtension = DeviceObject->DeviceExtension;

    irpStack = IoGetCurrentIrpStackLocation(Irp);
    diskData = (PDISK_DATA)(commonExtension->DriverData);


    ASSERT (commonExtension->IsFdo);

    //
    // Check the input buffer size.
    //

    if (irpStack->Parameters.DeviceIoControl.InputBufferLength <
        sizeof (CREATE_DISK) ) {

        return STATUS_INFO_LENGTH_MISMATCH;
    }

    //
    // If we are being asked to create a GPT disk on a system that doesn't
    // support GPT, fail.
    //

    createDiskInfo = (PCREATE_DISK)Irp->AssociatedIrp.SystemBuffer;

    if (DiskDisableGpt &&
        createDiskInfo->PartitionStyle == PARTITION_STYLE_GPT) {

        return STATUS_INVALID_PARAMETER;
    }

    //
    // Call the lower level Io routine to do the dirty work of writing a
    // new partition table.
    //

    DiskAcquirePartitioningLock(fdoExtension);

    DiskInvalidatePartitionTable(fdoExtension, TRUE);

    status = IoCreateDisk (
                    commonExtension->PartitionZeroExtension->CommonExtension.DeviceObject,
                    Irp->AssociatedIrp.SystemBuffer
                    );
    DiskReleasePartitioningLock(fdoExtension);
    ClassInvalidateBusRelations(DeviceObject);

    Irp->IoStatus.Status = status;

    return status;
}


NTSTATUS
DiskIoctlGetDriveLayout(
    IN OUT PDEVICE_OBJECT DeviceObject,
    IN OUT PIRP Irp
    )

/*++

Routine Description:

    Handler for IOCTL_DISK_GET_DRIVE_LAYOUT ioctl.

    This ioctl has been replace by IOCTL_DISK_GET_DRIVE_LAYOUT_EX.

Arguments:

    DeviceObject - Device object representing a disk the layout information
            will be obtained for.

    Irp - The IRP for this request.


Return Values:

    NTSTATUS code.

--*/

{
    NTSTATUS status;
    ULONG size;
    PDRIVE_LAYOUT_INFORMATION partitionList;
    PDRIVE_LAYOUT_INFORMATION_EX partitionListEx;
    PFUNCTIONAL_DEVICE_EXTENSION fdoExtension;
    PPHYSICAL_DEVICE_EXTENSION pdoExtension;
    PCOMMON_DEVICE_EXTENSION commonExtension;
    PIO_STACK_LOCATION irpStack;
    PDISK_DATA diskData;
    BOOLEAN bUseCache = TRUE;

    PAGED_CODE ();

    ASSERT ( DeviceObject );
    ASSERT ( Irp );

    //
    // Initialization
    //

    partitionListEx = NULL;
    partitionList = NULL;
    fdoExtension = DeviceObject->DeviceExtension;
    commonExtension = DeviceObject->DeviceExtension;

    irpStack = IoGetCurrentIrpStackLocation(Irp);
    diskData = (PDISK_DATA)(commonExtension->DriverData);

    //
    // If our cached partition table is valid we do not need to touch the disk
    //

    if (diskData->CachedPartitionTableValid == FALSE)
    {
        //
        // Issue a read capacity to update the apparent size of the disk.
        //

        DiskReadDriveCapacity(fdoExtension->DeviceObject);

        bUseCache = FALSE;
    }

    DiskAcquirePartitioningLock(fdoExtension);

    status = DiskReadPartitionTableEx(fdoExtension, FALSE, &partitionListEx);

    if (!NT_SUCCESS(status)) {
        DiskReleasePartitioningLock(fdoExtension);
        return status;
    }

    //
    // This ioctl is only supported on MBR partitioned disks. Fail the
    // call otherwise.
    //

    if (partitionListEx->PartitionStyle != PARTITION_STYLE_MBR) {
        DiskReleasePartitioningLock(fdoExtension);
        return STATUS_INVALID_DEVICE_REQUEST;
    }

    //
    // The disk layout has been returned in the partitionListEx
    // buffer.  Determine its size and, if the data will fit
    // into the intermediate buffer, return it.
    //

    size = FIELD_OFFSET(DRIVE_LAYOUT_INFORMATION, PartitionEntry[0]);
    size += partitionListEx->PartitionCount * sizeof(PARTITION_INFORMATION);

    if (irpStack->Parameters.DeviceIoControl.OutputBufferLength <
        size) {

        Irp->IoStatus.Status = STATUS_BUFFER_TOO_SMALL;

        DiskReleasePartitioningLock(fdoExtension);
        return STATUS_BUFFER_TOO_SMALL;
    }

    //
    // Update the partition device objects and set valid partition numbers
    //

    ASSERT(diskData->UpdatePartitionRoutine != NULL);
    diskData->UpdatePartitionRoutine(DeviceObject, partitionListEx);

    //
    // Convert the extended drive layout structure to a regular drive layout
    // structure to return. DiskConvertExtendedToLayout() allocates pool
    // that we must free.
    //

    partitionList = DiskConvertExtendedToLayout(partitionListEx);

    if (partitionList == NULL) {
        Irp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
        DiskReleasePartitioningLock (fdoExtension);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // We're done with the extended partition list now.
    //

    partitionListEx = NULL;

    //
    // Copy partition information to system buffer.
    //

    RtlMoveMemory(Irp->AssociatedIrp.SystemBuffer,
                  partitionList,
                  size);

    Irp->IoStatus.Information = size;
    Irp->IoStatus.Status = status;

    //
    // Finally, free the buffer allocated by reading the
    // partition table.
    //

    ExFreePool(partitionList);
    DiskReleasePartitioningLock(fdoExtension);

    if (bUseCache == FALSE)
    {
        ClassInvalidateBusRelations(DeviceObject);
    }

    return status;
}


NTSTATUS
DiskIoctlGetDriveLayoutEx(
    IN OUT PDEVICE_OBJECT DeviceObject,
    IN OUT PIRP Irp
    )

/*++

Routine Description:

    Handler for IOCTL_DISK_GET_DRIVE_LAYOUT_EX ioctl.

    This ioctl replaces IOCTL_DISK_GET_DRIVE_LAYOUT.

Arguments:

    DeviceObject - Device object representing a disk the layout information
            will be obtained for.

    Irp - The IRP for this request.


Return Values:

    NTSTATUS code.

--*/

{
    NTSTATUS status;
    ULONG size;
    PDRIVE_LAYOUT_INFORMATION_EX partitionList;
    PFUNCTIONAL_DEVICE_EXTENSION fdoExtension;
    PPHYSICAL_DEVICE_EXTENSION pdoExtension;
    PCOMMON_DEVICE_EXTENSION commonExtension;
    PIO_STACK_LOCATION irpStack;
    PDISK_DATA diskData;
    BOOLEAN bUseCache = TRUE;


    PAGED_CODE ();

    ASSERT ( DeviceObject );
    ASSERT ( Irp );

    //
    // Initialization
    //

    fdoExtension = DeviceObject->DeviceExtension;
    pdoExtension = DeviceObject->DeviceExtension;
    commonExtension = DeviceObject->DeviceExtension;

    irpStack = IoGetCurrentIrpStackLocation(Irp);
    diskData = (PDISK_DATA)(commonExtension->DriverData);

    //
    // If our cached partition table is valid we do not need to touch the disk
    //

    if (diskData->CachedPartitionTableValid == FALSE)
    {
        //
        // Issue a read capacity to update the apparent size of the disk.
        //

        DiskReadDriveCapacity(fdoExtension->DeviceObject);

        bUseCache = FALSE;
    }

    DiskAcquirePartitioningLock (fdoExtension);

    status = DiskReadPartitionTableEx (fdoExtension, FALSE, &partitionList);

    if ( !NT_SUCCESS (status) ) {
        DiskReleasePartitioningLock (fdoExtension);
        return status;
    }

    size = FIELD_OFFSET (DRIVE_LAYOUT_INFORMATION_EX, PartitionEntry[0]) +
           partitionList->PartitionCount * sizeof (PARTITION_INFORMATION_EX);

    //
    // If the output buffer is large enough, copy data to the output buffer,
    // otherwise, fail.
    //

    if (irpStack->Parameters.DeviceIoControl.OutputBufferLength <
        size) {

        Irp->IoStatus.Status = STATUS_BUFFER_TOO_SMALL;

        DiskReleasePartitioningLock(fdoExtension);
        return STATUS_BUFFER_TOO_SMALL;
    }

    //
    // Update the partition device objects and set valid partition numbers
    //

    ASSERT(diskData->UpdatePartitionRoutine != NULL);
    diskData->UpdatePartitionRoutine(DeviceObject, partitionList);

    RtlCopyMemory (Irp->AssociatedIrp.SystemBuffer,
                   partitionList,
                   size
                   );

    Irp->IoStatus.Information = size;
    Irp->IoStatus.Status = status;

    DiskReleasePartitioningLock(fdoExtension);

    if (bUseCache == FALSE)
    {
        ClassInvalidateBusRelations(DeviceObject);
    }

    return status;
}


NTSTATUS
DiskIoctlSetDriveLayout(
    IN OUT PDEVICE_OBJECT DeviceObject,
    IN OUT PIRP Irp
    )

/*++

Routine Description:

    Handler for IOCTL_DISK_SET_DRIVE_LAYOUT ioctl.

    This ioctl has been replaced by IOCTL_DISK_SET_DRIVE_LAYOUT_EX.

Arguments:

    DeviceObject - Device object for which partition table should be written.

    Irp - IRP involved.

Return Values:

    NTSTATUS code.

--*/

{
    NTSTATUS status;
    PDRIVE_LAYOUT_INFORMATION partitionList;
    PDRIVE_LAYOUT_INFORMATION_EX partitionListEx;
    PFUNCTIONAL_DEVICE_EXTENSION fdoExtension;
    PPHYSICAL_DEVICE_EXTENSION pdoExtension;
    PCOMMON_DEVICE_EXTENSION commonExtension;
    PIO_STACK_LOCATION irpStack;
    PDISK_DATA diskData;
    BOOLEAN invalidateBusRelations;
    SIZE_T listSize;
    SIZE_T inputBufferLength;
    SIZE_T outputBufferLength;

    PAGED_CODE ();

    ASSERT ( DeviceObject );
    ASSERT ( Irp );

    //
    // Initialization
    //

    partitionListEx = NULL;
    partitionList = NULL;
    fdoExtension = DeviceObject->DeviceExtension;
    pdoExtension = DeviceObject->DeviceExtension;
    commonExtension = DeviceObject->DeviceExtension;

    irpStack = IoGetCurrentIrpStackLocation(Irp);
    diskData = (PDISK_DATA)(commonExtension->DriverData);
    partitionList = Irp->AssociatedIrp.SystemBuffer;

    inputBufferLength = irpStack->Parameters.DeviceIoControl.InputBufferLength;
    outputBufferLength = irpStack->Parameters.DeviceIoControl.OutputBufferLength;

    //
    // Update the partition table.
    //

    if (inputBufferLength < sizeof (DRIVE_LAYOUT_INFORMATION)) {

        status = STATUS_INFO_LENGTH_MISMATCH;
        return status;
    }

    DiskAcquirePartitioningLock(fdoExtension);

    listSize = (partitionList->PartitionCount - 1);
    listSize *= sizeof(PARTITION_INFORMATION);
    listSize += sizeof(DRIVE_LAYOUT_INFORMATION);

    if (inputBufferLength < listSize) {

        //
        // The remaning size of the input buffer not big enough to
        // hold the additional partition entries
        //

        status = STATUS_INFO_LENGTH_MISMATCH;
        DiskReleasePartitioningLock(fdoExtension);
        return status;
    }

    //
    // Convert the parititon information structure into an extended
    // structure.
    //

    partitionListEx = DiskConvertLayoutToExtended (partitionList);

    if ( partitionListEx == NULL ) {

        status = STATUS_INSUFFICIENT_RESOURCES;
        Irp->IoStatus.Status = status;
        DiskReleasePartitioningLock(fdoExtension);
        return status;
    }

    //
    // Redo all the partition numbers in the partition information
    //

    ASSERT(diskData->UpdatePartitionRoutine != NULL);
    diskData->UpdatePartitionRoutine(DeviceObject, partitionListEx);

    //
    // Write changes to disk.
    //

    status = DiskWritePartitionTableEx(fdoExtension, partitionListEx);

    //
    // Update IRP with bytes returned.  Make sure we don't claim to be
    // returning more bytes than the caller is expecting to get back.
    //

    if (NT_SUCCESS (status)) {
        if (outputBufferLength < listSize) {
            Irp->IoStatus.Information = outputBufferLength;
        } else {
            ULONG i;

            Irp->IoStatus.Information = listSize;

            //
            // Also update the partition numbers.
            //

            for (i = 0; i < partitionList->PartitionCount; i++) {

                PPARTITION_INFORMATION partition;
                PPARTITION_INFORMATION_EX partitionEx;

                partition = &partitionList->PartitionEntry[i];
                partitionEx = &partitionListEx->PartitionEntry[i];
                partition->PartitionNumber = partitionEx->PartitionNumber;

            }
        }
    }

    ExFreePool (partitionListEx);
    DiskReleasePartitioningLock(fdoExtension);
    ClassInvalidateBusRelations(DeviceObject);

    Irp->IoStatus.Status = status;
    return status;
}


NTSTATUS
DiskIoctlSetDriveLayoutEx(
    IN OUT PDEVICE_OBJECT DeviceObject,
    IN OUT PIRP Irp
    )

/*++

Routine Description:

    Handler for IOCTL_DISK_SET_DRIVE_LAYOUT_EX ioctl.

    This ioctl replaces IOCTL_DISK_SET_DRIVE_LAYOUT.

Arguments:

    DeviceObject - Device object for which partition table should be written.

    Irp - IRP involved.

Return Values:

    NTSTATUS code.

--*/

{

    NTSTATUS status;
    PDRIVE_LAYOUT_INFORMATION_EX partitionListEx;
    PFUNCTIONAL_DEVICE_EXTENSION fdoExtension;
    PCOMMON_DEVICE_EXTENSION commonExtension;
    PARTITION_INFORMATION_EX partitionInfo;

    PIO_STACK_LOCATION irpStack;
    PDISK_DATA diskData;
    BOOLEAN invalidateBusRelations;
    SIZE_T listSize;
    SIZE_T inputBufferLength;
    SIZE_T outputBufferLength;
    ULONG  part;
    LARGE_INTEGER   offset;

    PAGED_CODE ();

    ASSERT ( DeviceObject );
    ASSERT ( Irp );

    //
    // Initialization
    //

    partitionListEx = NULL;
    fdoExtension = DeviceObject->DeviceExtension;
    commonExtension = DeviceObject->DeviceExtension;

    irpStack = IoGetCurrentIrpStackLocation(Irp);
    diskData = (PDISK_DATA)(commonExtension->DriverData);
    partitionListEx = Irp->AssociatedIrp.SystemBuffer;

    inputBufferLength = irpStack->Parameters.DeviceIoControl.InputBufferLength;
    outputBufferLength = irpStack->Parameters.DeviceIoControl.OutputBufferLength;

    //
    // Update the partition table.
    //

    if (inputBufferLength <
        FIELD_OFFSET (DRIVE_LAYOUT_INFORMATION_EX, PartitionEntry)) {
        status = STATUS_INFO_LENGTH_MISMATCH;
        return status;
    }

    DiskAcquirePartitioningLock(fdoExtension);

    listSize = partitionListEx->PartitionCount;
    listSize *= sizeof(PARTITION_INFORMATION_EX);
    listSize += FIELD_OFFSET (DRIVE_LAYOUT_INFORMATION_EX, PartitionEntry);

    if (inputBufferLength < listSize) {

        //
        // The remaning size of the input buffer not big enough to
        // hold the additional partition entries
        //

        status = STATUS_INFO_LENGTH_MISMATCH;
        DiskReleasePartitioningLock(fdoExtension);
        return status;
    }

    //
    // If the disk type is MBR make sure the partitioning does not
    // go beyond 2TB boundary.
    //

    if (partitionListEx->PartitionStyle == PARTITION_STYLE_MBR) {

        for (part = 0; part < partitionListEx->PartitionCount; part++) {

            partitionInfo = partitionListEx->PartitionEntry[part];
            offset.QuadPart = partitionInfo.StartingOffset.QuadPart + partitionInfo.PartitionLength.QuadPart;
            if (offset.QuadPart >= 0x20000000000) {
                DebugPrint((1, "Partition crosses 2TB boundary! Failing request.\n"));

                status = STATUS_INVALID_DEVICE_REQUEST;
                DiskReleasePartitioningLock(fdoExtension);
                return status;
            }
        }
    }   // if PartitionStyle == PARTITION_STYLE_MBR

    //
    // If the partition count is zero, this is a request to clear
    // the partition table.
    //

    if (partitionListEx->PartitionCount == 0) {

        CREATE_DISK CreateDiskInfo = { 0 };

        CreateDiskInfo.PartitionStyle = diskData->PartitionStyle;

        if (diskData->PartitionStyle == PARTITION_STYLE_MBR) {
            CreateDiskInfo.Mbr.Signature = partitionListEx->Mbr.Signature;
        } else {
            ASSERT (diskData->PartitionStyle == PARTITION_STYLE_GPT);
            CreateDiskInfo.Gpt.DiskId = partitionListEx->Gpt.DiskId;
            //
            // NB: Setting MaxPartitionCount to zero will
            // force the GPT partition table writing code
            // to use the default minimum for this value.
            //
            CreateDiskInfo.Gpt.MaxPartitionCount = 0;
        }
        DiskInvalidatePartitionTable(fdoExtension, TRUE);

        status = IoCreateDisk(DeviceObject, &CreateDiskInfo);

    } else {

        //
        // Redo all the partition numbers in the partition information
        //

        ASSERT(diskData->UpdatePartitionRoutine != NULL);
        diskData->UpdatePartitionRoutine(DeviceObject, partitionListEx);

        //
        // Write changes to disk.
        //

        status = DiskWritePartitionTableEx(fdoExtension, partitionListEx);
    }

    //
    // Update IRP with bytes returned.  Make sure we don't claim to be
    // returning more bytes than the caller is expecting to get back.
    //

    if (NT_SUCCESS(status)) {
        if (outputBufferLength < listSize) {
            Irp->IoStatus.Information = outputBufferLength;
        } else {
            Irp->IoStatus.Information = listSize;
        }
    }

    DiskReleasePartitioningLock(fdoExtension);
    ClassInvalidateBusRelations(DeviceObject);

    Irp->IoStatus.Status = status;
    return status;
}


NTSTATUS
DiskIoctlGetPartitionInfo(
    IN OUT PDEVICE_OBJECT DeviceObject,
    IN OUT PIRP Irp
    )

/*++

Routine Description:

    Handle the IOCTL_DISK_GET_PARTITION_INFO ioctl. Return the information
    about the partition specified by the device object.  Note that no
    information is ever returned about the size or partition type of the
    physical disk, as this doesn't make any sense.

    This ioctl has been replaced by IOCTL_DISK_GET_PARTITION_INFO_EX.

Arguments:

    DeviceObject -

    Irp -

Return Values:

    NTSTATUS code.

--*/

{
    NTSTATUS status;
    PIO_STACK_LOCATION irpStack;
    PDISK_DATA diskData;
    PPARTITION_INFORMATION partitionInfo;
    PFUNCTIONAL_DEVICE_EXTENSION p0Extension;
    PCOMMON_DEVICE_EXTENSION commonExtension;
    PDISK_DATA partitionZeroData;
    NTSTATUS oldReadyStatus;


    PAGED_CODE ();

    ASSERT ( DeviceObject );
    ASSERT ( Irp );


    //
    // Initialization
    //

    commonExtension = DeviceObject->DeviceExtension;
    irpStack = IoGetCurrentIrpStackLocation(Irp);
    diskData = (PDISK_DATA)(commonExtension->DriverData);
    p0Extension = commonExtension->PartitionZeroExtension;
    partitionZeroData = ((PDISK_DATA) p0Extension->CommonExtension.DriverData);


    //
    // Check that the buffer is large enough.
    //

    if (irpStack->Parameters.DeviceIoControl.OutputBufferLength <
        sizeof(PARTITION_INFORMATION)) {

        status = STATUS_BUFFER_TOO_SMALL;
        return status;
    }

    //
    // Update the geometry in case it has changed
    //

    status = DiskReadDriveCapacity(p0Extension->DeviceObject);

    //
    // Note whether the drive is ready.  If the status has changed then
    // notify pnp.
    //

    oldReadyStatus = InterlockedExchange(
                        &(partitionZeroData->ReadyStatus),
                        status);

    if(partitionZeroData->ReadyStatus != oldReadyStatus) {
        IoInvalidateDeviceRelations(p0Extension->LowerPdo,
                                    BusRelations);
    }

    if(!NT_SUCCESS(status)) {
        return status;
    }


    //
    // Partition zero, the partition representing the entire disk, is
    // special cased. The logic below allows for sending this ioctl to
    // a GPT disk only for partition zero. This allows us to obtain
    // the size of a GPT disk using Win2k compatible IOCTLs.
    //

    if (commonExtension->PartitionNumber == 0) {

        partitionInfo = (PPARTITION_INFORMATION) Irp->AssociatedIrp.SystemBuffer;

        partitionInfo->PartitionType = PARTITION_ENTRY_UNUSED;
        partitionInfo->StartingOffset = commonExtension->StartingOffset;
        partitionInfo->PartitionLength = commonExtension->PartitionLength;
        partitionInfo->HiddenSectors = 0;
        partitionInfo->PartitionNumber = commonExtension->PartitionNumber;
        partitionInfo->BootIndicator = FALSE;
        partitionInfo->RewritePartition = FALSE;
        partitionInfo->RecognizedPartition = FALSE;

    } else {

        //
        // We do not support this IOCTL on an EFI partitioned disk
        // for any partition other than partition zero.
        //

        if (diskData->PartitionStyle != PARTITION_STYLE_MBR) {
            status = STATUS_INVALID_DEVICE_REQUEST;
            Irp->IoStatus.Status = status;
            return status;
        }

        DiskEnumerateDevice(p0Extension->DeviceObject);

        DiskAcquirePartitioningLock(p0Extension);


        partitionInfo = (PPARTITION_INFORMATION) Irp->AssociatedIrp.SystemBuffer;

        partitionInfo->PartitionType = diskData->Mbr.PartitionType;
        partitionInfo->StartingOffset = commonExtension->StartingOffset;
        partitionInfo->PartitionLength = commonExtension->PartitionLength;
        partitionInfo->HiddenSectors = diskData->Mbr.HiddenSectors;
        partitionInfo->PartitionNumber = commonExtension->PartitionNumber;
        partitionInfo->BootIndicator = diskData->Mbr.BootIndicator;
        partitionInfo->RewritePartition = FALSE;
        partitionInfo->RecognizedPartition =
                IsRecognizedPartition(diskData->Mbr.PartitionType);

        DiskReleasePartitioningLock(p0Extension);

    }

    status = STATUS_SUCCESS;
    Irp->IoStatus.Information = sizeof(PARTITION_INFORMATION);

    return status;
}


NTSTATUS
DiskIoctlGetPartitionInfoEx(
    IN OUT PDEVICE_OBJECT DeviceObject,
    IN OUT PIRP Irp
    )
{
    NTSTATUS status;
    PIO_STACK_LOCATION irpStack;
    PDISK_DATA diskData;
    PPARTITION_INFORMATION_EX partitionInfo;
    PFUNCTIONAL_DEVICE_EXTENSION p0Extension;
    PCOMMON_DEVICE_EXTENSION commonExtension;
    PDISK_DATA partitionZeroData;
    NTSTATUS oldReadyStatus;


    PAGED_CODE ();

    ASSERT ( DeviceObject );
    ASSERT ( Irp );


    //
    // Initialization
    //

    commonExtension = DeviceObject->DeviceExtension;
    irpStack = IoGetCurrentIrpStackLocation(Irp);
    diskData = (PDISK_DATA)(commonExtension->DriverData);
    p0Extension = commonExtension->PartitionZeroExtension;
    partitionZeroData = ((PDISK_DATA) p0Extension->CommonExtension.DriverData);


    //
    // Check that the buffer is large enough.
    //

    if (irpStack->Parameters.DeviceIoControl.OutputBufferLength <
        sizeof(PARTITION_INFORMATION_EX)) {

        status = STATUS_BUFFER_TOO_SMALL;
        Irp->IoStatus.Status = status;
        return status;
    }

    //
    // Update the geometry in case it has changed
    //

    status = DiskReadDriveCapacity(p0Extension->DeviceObject);

    //
    // Note whether the drive is ready.  If the status has changed then
    // notify pnp.
    //

    oldReadyStatus = InterlockedExchange(
                        &(partitionZeroData->ReadyStatus),
                        status);

    if(partitionZeroData->ReadyStatus != oldReadyStatus) {
        IoInvalidateDeviceRelations(p0Extension->LowerPdo,
                                    BusRelations);
    }

    if(!NT_SUCCESS(status)) {
        return status;
    }

    //
    // If this is something other than partition 0 then do a
    // re-enumeration to make sure we've got up-to-date information.
    //
    if(commonExtension->PartitionNumber != 0) {
        DiskEnumerateDevice(p0Extension->DeviceObject);
        DiskAcquirePartitioningLock(p0Extension);
    }

    partitionInfo = (PPARTITION_INFORMATION_EX) Irp->AssociatedIrp.SystemBuffer;

    partitionInfo->StartingOffset = commonExtension->StartingOffset;
    partitionInfo->PartitionLength = commonExtension->PartitionLength;
    partitionInfo->RewritePartition = FALSE;
    partitionInfo->PartitionNumber = commonExtension->PartitionNumber;
    partitionInfo->PartitionStyle = diskData->PartitionStyle;

    if ( diskData->PartitionStyle == PARTITION_STYLE_MBR ) {

        partitionInfo->Mbr.PartitionType = diskData->Mbr.PartitionType;
        partitionInfo->Mbr.HiddenSectors = diskData->Mbr.HiddenSectors;
        partitionInfo->Mbr.BootIndicator = diskData->Mbr.BootIndicator;
        partitionInfo->Mbr.RecognizedPartition =
                IsRecognizedPartition(diskData->Mbr.PartitionType);

    } else {

        //
        // ISSUE - 2000/02/09 - math: Review for Partition0.
        // Is this correct for Partition0?
        //

        partitionInfo->Gpt.PartitionType = diskData->Efi.PartitionType;
        partitionInfo->Gpt.PartitionId = diskData->Efi.PartitionId;
        partitionInfo->Gpt.Attributes = diskData->Efi.Attributes;
        RtlCopyMemory (
                partitionInfo->Gpt.Name,
                diskData->Efi.PartitionName,
                sizeof (partitionInfo->Gpt.Name)
                );
    }

    status = STATUS_SUCCESS;
    Irp->IoStatus.Information = sizeof(PARTITION_INFORMATION_EX);

    if(commonExtension->PartitionNumber != 0) {
        DiskReleasePartitioningLock(p0Extension);
    }

    return status;
}


NTSTATUS
DiskIoctlGetLengthInfo(
    IN OUT PDEVICE_OBJECT DeviceObject,
    IN OUT PIRP Irp
    )
{
    NTSTATUS status;
    PIO_STACK_LOCATION irpStack;
    PDISK_DATA diskData;
    PGET_LENGTH_INFORMATION lengthInfo;
    PFUNCTIONAL_DEVICE_EXTENSION p0Extension;
    PCOMMON_DEVICE_EXTENSION commonExtension;
    PDISK_DATA partitionZeroData;
    NTSTATUS oldReadyStatus;


    PAGED_CODE ();

    ASSERT ( DeviceObject );
    ASSERT ( Irp );


    //
    // Initialization
    //

    commonExtension = DeviceObject->DeviceExtension;
    irpStack = IoGetCurrentIrpStackLocation(Irp);
    diskData = (PDISK_DATA)(commonExtension->DriverData);
    p0Extension = commonExtension->PartitionZeroExtension;
    partitionZeroData = ((PDISK_DATA) p0Extension->CommonExtension.DriverData);


    //
    // Check that the buffer is large enough.
    //

    if (irpStack->Parameters.DeviceIoControl.OutputBufferLength <
        sizeof(GET_LENGTH_INFORMATION)) {

        Irp->IoStatus.Status = STATUS_BUFFER_TOO_SMALL;
        return STATUS_BUFFER_TOO_SMALL;
    }

    //
    // Update the geometry in case it has changed
    //

    status = DiskReadDriveCapacity(p0Extension->DeviceObject);

    //
    // Note whether the drive is ready.  If the status has changed then
    // notify pnp.
    //

    oldReadyStatus = InterlockedExchange(
                        &(partitionZeroData->ReadyStatus),
                        status);

    if(partitionZeroData->ReadyStatus != oldReadyStatus) {
        IoInvalidateDeviceRelations(p0Extension->LowerPdo,
                                    BusRelations);
    }

    if(!NT_SUCCESS(status)) {
        return status;
    }

    //
    // If this is something other than partition 0 then do a
    // re-enumeration to make sure we've got up-to-date information.
    //
    if(commonExtension->PartitionNumber != 0) {
        DiskEnumerateDevice(p0Extension->DeviceObject);
        DiskAcquirePartitioningLock(p0Extension);
    }

    lengthInfo = (PGET_LENGTH_INFORMATION) Irp->AssociatedIrp.SystemBuffer;

    lengthInfo->Length = commonExtension->PartitionLength;

    status = STATUS_SUCCESS;
    Irp->IoStatus.Information = sizeof(GET_LENGTH_INFORMATION);

    if(commonExtension->PartitionNumber != 0) {
        DiskReleasePartitioningLock(p0Extension);
    }

    return status;
}


NTSTATUS
DiskIoctlSetPartitionInfo(
    IN OUT PDEVICE_OBJECT DeviceObject,
    IN OUT PIRP Irp
    )
{
    NTSTATUS status;
    PSET_PARTITION_INFORMATION inputBuffer;
    PDISK_DATA diskData;
    PIO_STACK_LOCATION irpStack;
    PCOMMON_DEVICE_EXTENSION commonExtension;


    PAGED_CODE ();

    ASSERT ( DeviceObject != NULL );
    ASSERT ( Irp != NULL );


    //
    // Initialization
    //

    commonExtension = DeviceObject->DeviceExtension;
    irpStack = IoGetCurrentIrpStackLocation(Irp);
    diskData = (PDISK_DATA)(commonExtension->DriverData);
    inputBuffer = (PSET_PARTITION_INFORMATION)Irp->AssociatedIrp.SystemBuffer;

    if(commonExtension->IsFdo) {

        return STATUS_UNSUCCESSFUL;
    }


    if (diskData->PartitionStyle != PARTITION_STYLE_MBR) {
        return STATUS_INVALID_DEVICE_REQUEST;
    }

    //
    // Validate buffer length
    //

    if(irpStack->Parameters.DeviceIoControl.InputBufferLength <
       sizeof(SET_PARTITION_INFORMATION)) {

        return STATUS_INFO_LENGTH_MISMATCH;
    }

    DiskAcquirePartitioningLock(commonExtension->PartitionZeroExtension);

    //
    // The HAL routines IoGet- and IoSetPartitionInformation were
    // developed before support of dynamic partitioning and therefore
    // don't distinguish between partition ordinal (that is the order
    // of a paritition on a disk) and the partition number.  (The
    // partition number is assigned to a partition to identify it to
    // the system.) Use partition ordinals for these legacy calls.
    //

    status = DiskSetPartitionInformation(
                commonExtension->PartitionZeroExtension,
                commonExtension->PartitionZeroExtension->DiskGeometry.BytesPerSector,
                diskData->PartitionOrdinal,
                inputBuffer->PartitionType);

    if(NT_SUCCESS(status)) {

        diskData->Mbr.PartitionType = inputBuffer->PartitionType;
    }

    DiskReleasePartitioningLock(commonExtension->PartitionZeroExtension);

    return status;
}


NTSTATUS
DiskIoctlSetPartitionInfoEx(
    IN OUT PDEVICE_OBJECT DeviceObject,
    IN OUT PIRP Irp
    )
{
    NTSTATUS status;
    PSET_PARTITION_INFORMATION_EX inputBuffer;
    PDISK_DATA diskData;
    PIO_STACK_LOCATION irpStack;
    PCOMMON_DEVICE_EXTENSION commonExtension;


    PAGED_CODE ();

    ASSERT ( DeviceObject != NULL );
    ASSERT ( Irp != NULL );


    //
    // Initialization
    //

    commonExtension = DeviceObject->DeviceExtension;
    irpStack = IoGetCurrentIrpStackLocation(Irp);
    diskData = (PDISK_DATA)(commonExtension->DriverData);
    inputBuffer = (PSET_PARTITION_INFORMATION_EX)Irp->AssociatedIrp.SystemBuffer;

    if(commonExtension->IsFdo) {

        return STATUS_UNSUCCESSFUL;
    }

    //
    // Validate buffer length
    //

    if(irpStack->Parameters.DeviceIoControl.InputBufferLength <
       sizeof(SET_PARTITION_INFORMATION_EX)) {

        return STATUS_INFO_LENGTH_MISMATCH;
    }

    DiskAcquirePartitioningLock(commonExtension->PartitionZeroExtension);

    //
    // The HAL routines IoGet- and IoSetPartitionInformation were
    // developed before support of dynamic partitioning and therefore
    // don't distinguish between partition ordinal (that is the order
    // of a paritition on a disk) and the partition number.  (The
    // partition number is assigned to a partition to identify it to
    // the system.) Use partition ordinals for these legacy calls.
    //

    status = DiskSetPartitionInformationEx(
                commonExtension->PartitionZeroExtension,
                diskData->PartitionOrdinal,
                inputBuffer
                );

    if(NT_SUCCESS(status)) {

        if (diskData->PartitionStyle == PARTITION_STYLE_MBR) {

            diskData->Mbr.PartitionType = inputBuffer->Mbr.PartitionType;

        } else {

            diskData->Efi.PartitionType = inputBuffer->Gpt.PartitionType;
            diskData->Efi.PartitionId = inputBuffer->Gpt.PartitionId;
            diskData->Efi.Attributes = inputBuffer->Gpt.Attributes;

            RtlCopyMemory (
                    diskData->Efi.PartitionName,
                    inputBuffer->Gpt.Name,
                    sizeof (diskData->Efi.PartitionName)
                    );
        }
    }

    DiskReleasePartitioningLock(commonExtension->PartitionZeroExtension);

    return status;
}

typedef struct _DISK_GEOMETRY_EX_INTERNAL {
    DISK_GEOMETRY Geometry;
    LARGE_INTEGER DiskSize;
    DISK_PARTITION_INFO Partition;
    DISK_DETECTION_INFO Detection;
} DISK_GEOMETRY_EX_INTERNAL, *PDISK_GEOMETRY_EX_INTERNAL;


NTSTATUS
DiskIoctlGetDriveGeometryEx(
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PIRP Irp
    )

/*++

Routine Description:

    Obtain the extended geometry information for the drive.

Arguments:

    DeviceObject - The device object to obtain the geometry for.

    Irp - IRP with a return buffer large enough to receive the
            extended geometry information.

Return Value:

    NTSTATUS code

--*/

{
    NTSTATUS status;
    PIO_STACK_LOCATION irpStack;
    PCOMMON_DEVICE_EXTENSION commonExtension;
    PFUNCTIONAL_DEVICE_EXTENSION fdoExtension;
    PDISK_DATA diskData;
    PDISK_GEOMETRY_EX_INTERNAL geometryEx;
    ULONG OutputBufferLength;

    //
    // Verification
    //

    PAGED_CODE ();

    ASSERT ( DeviceObject != NULL );
    ASSERT ( Irp != NULL );

    //
    // Setup parameters
    //

    commonExtension = DeviceObject->DeviceExtension;
    fdoExtension = DeviceObject->DeviceExtension;
    diskData = (PDISK_DATA)(commonExtension->DriverData);
    irpStack = IoGetCurrentIrpStackLocation ( Irp );
    geometryEx = NULL;
    OutputBufferLength = irpStack->Parameters.DeviceIoControl.OutputBufferLength;

    //
    // This is only valid for the FDO.
    //

    ASSERT ( commonExtension->IsFdo );

    //
    // Check that the buffer is large enough. It must be large enough
    // to hold at lest the Geometry and DiskSize fields of of the
    // DISK_GEOMETRY_EX structure.
    //

    if ( OutputBufferLength < FIELD_OFFSET (DISK_GEOMETRY_EX, Data) ) {

        //
        // Buffer too small. Bail out, telling the caller the required
        // size.
        //

        status = STATUS_BUFFER_TOO_SMALL;
        return status;
    }

    if (TEST_FLAG (DeviceObject->Characteristics, FILE_REMOVABLE_MEDIA)) {

        //
        // Issue a ReadCapacity to update device extension
        // with information for the current media.
        //

        status = DiskReadDriveCapacity (
                    commonExtension->PartitionZeroExtension->DeviceObject);

        diskData->ReadyStatus = status;

        if (!NT_SUCCESS (status)) {
            return status;
        }
    }

    //
    // Copy drive geometry.
    //

    geometryEx = (PDISK_GEOMETRY_EX_INTERNAL)Irp->AssociatedIrp.SystemBuffer;
    geometryEx->Geometry = fdoExtension->DiskGeometry;
    geometryEx->DiskSize = commonExtension->PartitionZeroExtension->CommonExtension.PartitionLength;

    //
    // If the user buffer is large enough to hold the partition information
    // then add that as well.
    //

    if (OutputBufferLength >=  FIELD_OFFSET (DISK_GEOMETRY_EX_INTERNAL, Detection)) {

        geometryEx->Partition.SizeOfPartitionInfo = sizeof (geometryEx->Partition);
        geometryEx->Partition.PartitionStyle = diskData->PartitionStyle;

        switch ( diskData->PartitionStyle ) {

            case PARTITION_STYLE_GPT:

                //
                // Copy GPT signature.
                //

                geometryEx->Partition.Gpt.DiskId = diskData->Efi.DiskId;
                break;

            case PARTITION_STYLE_MBR:

                //
                // Copy MBR signature and checksum.
                //

                geometryEx->Partition.Mbr.Signature = diskData->Mbr.Signature;
                geometryEx->Partition.Mbr.CheckSum = diskData->Mbr.MbrCheckSum;
                break;

            default:

                //
                // This is a raw disk. Zero out the signature area so
                // nobody gets confused.
                //

                RtlZeroMemory (
                    &geometryEx->Partition,
                    sizeof (geometryEx->Partition));
        }
    }

    //
    // If the buffer is large enough to hold the detection information,
    // then also add that.
    //

    if (OutputBufferLength >= sizeof (DISK_GEOMETRY_EX_INTERNAL)) {

        geometryEx->Detection.SizeOfDetectInfo =
            sizeof (geometryEx->Detection);

        status = DiskGetDetectInfo (
                    fdoExtension,
                    &geometryEx->Detection);

        //
        // Failed to obtain detection information, set to none.
        //

        if (!NT_SUCCESS (status)) {
            geometryEx->Detection.DetectionType = DetectNone;
        }
    }


    status = STATUS_SUCCESS;
    Irp->IoStatus.Information = min (OutputBufferLength,
                                     sizeof (DISK_GEOMETRY_EX_INTERNAL));

    return status;
}

