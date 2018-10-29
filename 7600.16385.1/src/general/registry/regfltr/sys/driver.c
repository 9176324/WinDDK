/*++
Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    regfltr.c

Abstract: 

    Sample driver used to run the kernel mode registry callback samples.

Environment:

    Kernel mode only

--*/

#include "regfltr.h"


DRIVER_INITIALIZE DriverEntry;
DRIVER_UNLOAD     DeviceUnload;

__drv_dispatchType(IRP_MJ_CREATE)         DRIVER_DISPATCH DeviceCreate;
__drv_dispatchType(IRP_MJ_CLOSE)          DRIVER_DISPATCH DeviceClose;
__drv_dispatchType(IRP_MJ_CLEANUP)        DRIVER_DISPATCH DeviceCleanup;
__drv_dispatchType(IRP_MJ_DEVICE_CONTROL) DRIVER_DISPATCH DeviceControl;

//
// Pointer to the device object used to register registry callbacks
//
PDEVICE_OBJECT g_DeviceObj;

//
// Registry callback version
//
ULONG g_MajorVersion;
ULONG g_MinorVersion;

//
// Set to TRUE if TM and RM were successfully created and the transaction
// callback was successfully enabled. 
//
BOOLEAN g_RMCreated;


NTSTATUS
DriverEntry (
    __in PDRIVER_OBJECT  DriverObject,
    __in PUNICODE_STRING RegistryPath
    )
/*++

Routine Description:

    This routine is called by the operating system to initialize the driver. 
    It allocates a device object, initializes the supported Io callbacks, and
    creates a symlink to make the device accessible to Win32.

    It gets the registry callback version and stores it in the global
    variables g_MajorVersion and g_MinorVersion. It also calls
    CreateKTMResourceManager to create a resource manager that is used in 
    the transaction samples.

Arguments:
    
    DriverObject - Supplies the system control object for this test driver.

    RegistryPath - The string location of the driver's corresponding services 
                   key in the registry.

Return value:

    Success or appropriate failure code.

--*/
{
    NTSTATUS Status;
    UNICODE_STRING NtDeviceName;
    UNICODE_STRING DosDevicesLinkName;

    UNREFERENCED_PARAMETER(RegistryPath);

    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 
               DPFLTR_ERROR_LEVEL,
               "RegFltr: DriverEntry()\n");

    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 
               DPFLTR_ERROR_LEVEL,
               "RegFltr: Use ed nt!Kd_IHVDRIVER_Mask 8 to enable more detailed printouts\n");

    //
    // Create our device object.
    //

    RtlInitUnicodeString(&NtDeviceName, NT_DEVICE_NAME);

    Status = IoCreateDevice(DriverObject,                 // pointer to driver object
                            0,                            // device extension size
                            &NtDeviceName,                // device name
                            FILE_DEVICE_UNKNOWN,          // device type
                            0,                            // device characteristics
                            FALSE,                        // not exclusive
                            &g_DeviceObj);                     // returned device object pointer

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Set dispatch routines.
    //

    DriverObject->MajorFunction[IRP_MJ_CREATE]         = DeviceCreate;
    DriverObject->MajorFunction[IRP_MJ_CLOSE]          = DeviceClose;
    DriverObject->MajorFunction[IRP_MJ_CLEANUP]        = DeviceCleanup;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DeviceControl;
    DriverObject->DriverUnload                         = DeviceUnload;

    //
    // Create a link in the Win32 namespace.
    //
    
    RtlInitUnicodeString(&DosDevicesLinkName, DOS_DEVICES_LINK_NAME);

    Status = IoCreateSymbolicLink(&DosDevicesLinkName, &NtDeviceName);

    if (!NT_SUCCESS(Status)) {
        IoDeleteDevice(DriverObject->DeviceObject);
        return Status;
    }

    //
    // Get callback version.
    //

    CmGetCallbackVersion(&g_MajorVersion, &g_MinorVersion);
    InfoPrint("Callback version %u.%u", g_MajorVersion, g_MinorVersion);

    //
    // Set up KTM resource manager and pass in RMCallback as our
    // callback routine.
    //

    Status = CreateKTMResourceManager(RMCallback, NULL);

    if (NT_SUCCESS(Status)) {
        g_RMCreated = TRUE;
    }

    return STATUS_SUCCESS;
    
}



NTSTATUS
DeviceCreate (
    __in PDEVICE_OBJECT DeviceObject,
    __in PIRP Irp
    )
/*++

Routine Description:

    Dispatches file create requests.  
    
Arguments:

    DeviceObject - The device object receiving the request.

    Irp - The request packet.

Return Value:

    STATUS_NOT_IMPLEMENTED

--*/
{
    UNREFERENCED_PARAMETER(DeviceObject);

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}



NTSTATUS
DeviceClose (
    __in PDEVICE_OBJECT DeviceObject,
    __in PIRP Irp
    )
/*++

Routine Description:

    Dispatches close requests.

Arguments:

    DeviceObject - The device object receiving the request.

    Irp - The request packet.

Return Value:

    STATUS_SUCCESS

--*/
{
    UNREFERENCED_PARAMETER(DeviceObject);

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}



NTSTATUS
DeviceCleanup (
    __in PDEVICE_OBJECT DeviceObject,
    __in PIRP Irp
    )
/*++

Routine Description:

    Dispatches cleanup requests.  Does nothing right now.

Arguments:

    DeviceObject - The device object receiving the request.

    Irp - The request packet.

Return Value:

    STATUS_SUCCESS

--*/
{
    UNREFERENCED_PARAMETER(DeviceObject);

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}



NTSTATUS
DeviceControl (
    __in PDEVICE_OBJECT DeviceObject,
    __in PIRP Irp
    )
/*++

Routine Description:

    Dispatches ioctl requests. 

Arguments:

    DeviceObject - The device object receiving the request.

    Irp - The request packet.

Return Value:

    Status returned from the method called.

--*/
{
    PIO_STACK_LOCATION IrpStack;
    ULONG Ioctl;
    NTSTATUS Status;

    UNREFERENCED_PARAMETER(DeviceObject);

    Status = STATUS_SUCCESS;

    IrpStack = IoGetCurrentIrpStackLocation(Irp);
    Ioctl = IrpStack->Parameters.DeviceIoControl.IoControlCode;

    switch (Ioctl)
    {

    case IOCTL_DO_KERNELMODE_SAMPLES:
        Status = DoCallbackSamples(DeviceObject, Irp);
        break;

    case IOCTL_REGISTER_CALLBACK:
        Status = RegisterCallback(DeviceObject, Irp);
        break;

    case IOCTL_UNREGISTER_CALLBACK:
        Status = UnRegisterCallback(DeviceObject, Irp);
        break;

    case IOCTL_GET_CALLBACK_VERSION:
        Status = GetCallbackVersion(DeviceObject, Irp);
        break;

    default:
        ErrorPrint("Unrecognized ioctl code 0x%x", Ioctl);
    }

    //
    // Complete the irp and return.
    //

    Irp->IoStatus.Status = Status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return Status;
    
}


VOID
DeviceUnload (
    __in PDRIVER_OBJECT DriverObject
    )
/*++

Routine Description:

    Cleans up any driver-level allocations and prepares for unload. All 
    this driver needs to do is to delete the device object and the 
    symbolic link between our device name and the Win32 visible name.

Arguments:

    DeviceObject - The device object receiving the request.

    Irp - The request packet.

Return Value:

    STATUS_NOT_IMPLEMENTED

--*/
{
    UNICODE_STRING  DosDevicesLinkName;

    //
    // Clean up the KTM data structures
    //

    DeleteKTMResourceManager();
    
    //
    // Delete the link from our device name to a name in the Win32 namespace.
    //

    RtlInitUnicodeString(&DosDevicesLinkName, DOS_DEVICES_LINK_NAME);
    IoDeleteSymbolicLink(&DosDevicesLinkName);

    //
    // Finally delete our device object
    //

    IoDeleteDevice(DriverObject->DeviceObject);

    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 
               DPFLTR_ERROR_LEVEL,
               "RegFltr: DeviceUnload\n");
}


