/*++
    Copyright (c) Microsoft Corporation

    Module Name:
        ioctl.c

    Abstract:
        Contains routines to support HIDCLASS internal ioctl queries
        for the pen tablet devices.

    Environment:
        Kernel mode

--*/

#include "pch.h"

#define MODULE_ID                       2

#ifdef ALLOC_PRAGMA
  #pragma alloc_text(PAGE, GetDeviceDescriptor)
  #pragma alloc_text(PAGE, GetReportDescriptor)
  #pragma alloc_text(PAGE, GetString)
  #pragma alloc_text(PAGE, GetAttributes)
#endif

/*++
    @doc    EXTERNAL

    @func   NTSTATUS | HpenInternalIoctl | Process the Control IRPs sent to
            this device.  This function cannot be pageable because reads/writes
            can be made at dispatch-level

    @parm   IN PDEVICE_OBJECT | DevObj | Points to the driver object.
    @parm   IN PIRP | Irp | Points to an I/O Request Packet.

    @rvalue SUCCESS | returns STATUS_SUCCESS.
    @rvalue FAILURE | returns NT status code.
--*/

NTSTATUS EXTERNAL
HpenInternalIoctl(
    IN PDEVICE_OBJECT DevObj,
    IN PIRP           Irp
    )
{
    NTSTATUS status;
    PDEVICE_EXTENSION devext = GET_MINIDRIVER_DEVICE_EXTENSION(DevObj);
    PIO_STACK_LOCATION irpsp = IoGetCurrentIrpStackLocation(Irp);

    TEnter(API, ("(DevObj=%p,Irp=%p,IrpSp=%p,Ioctl=%s)",
                 DevObj, Irp, irpsp,
                 LookupName(irpsp->Parameters.DeviceIoControl.IoControlCode,
                            HidIoctlNames)));

    Irp->IoStatus.Information = 0;
    status = IoAcquireRemoveLock(&devext->RemoveLock, Irp);
    if (!NT_SUCCESS(status))
    {
        LogError(ERRLOG_DEVICE_REMOVED,
                 status,
                 UNIQUE_ERRID(0x10),
                 NULL,
                 NULL);
        TWarn(("Received IRP after device was removed (status=%x).", status));
        Irp->IoStatus.Status = status;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
    }
    else if (!(devext->dwfHPen & HPENF_DEVICE_STARTED))
    {
        IoReleaseRemoveLock(&devext->RemoveLock, Irp);
        status = STATUS_DEVICE_NOT_READY;
        TWarn(("Digitizer is not started."));
        Irp->IoStatus.Status = status;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
    }
    else
    {
        BOOLEAN fSkipCompletion = FALSE;        

        switch(irpsp->Parameters.DeviceIoControl.IoControlCode)
        {
            case IOCTL_HID_GET_DEVICE_DESCRIPTOR:
                status = GetDeviceDescriptor(devext, Irp);
                break;

            case IOCTL_HID_GET_REPORT_DESCRIPTOR:
                status = GetReportDescriptor(devext, Irp);
                break;

            case IOCTL_HID_READ_REPORT:
                status = ReadReport(devext, Irp, &fSkipCompletion);
                break;

            case IOCTL_HID_WRITE_REPORT:
                status = OemWriteReport(devext, Irp);
                break;

            case IOCTL_HID_GET_STRING:
                status = GetString(devext, Irp);
                break;

            case IOCTL_HID_GET_DEVICE_ATTRIBUTES:
                status = GetAttributes(devext, Irp);
                break;

            case IOCTL_HID_ACTIVATE_DEVICE:
            case IOCTL_HID_DEACTIVATE_DEVICE:
                status = STATUS_SUCCESS;
                break;

            default:
                status = STATUS_NOT_SUPPORTED;
                LogError(ERRLOG_NOT_SUPPORTED,
                         status,
                         UNIQUE_ERRID(0x30),
                         NULL,
                         NULL);
                TWarn(("Unsupported (IOCTL=%x).",
                       irpsp->Parameters.DeviceIoControl.IoControlCode));
                break;
        }
        if (!fSkipCompletion)
        {
            IoReleaseRemoveLock(&devext->RemoveLock, Irp);
            Irp->IoStatus.Status = status;
            IoCompleteRequest(Irp, IO_NO_INCREMENT);
        }
       
    }

    TExit(API, ("=%x", status));
    return status;
}       //HpenInternalIoctl

/*++
    @doc    INTERNAL

    @func   NTSTATUS | GetDeviceDescriptor | Respond to
            HIDCLASS IOCTL_HID_GET_DEVICE_DESCRIPTOR by returning a device
            descriptor.

    @parm   IN PDEVICE_EXTENSION | DevExt | Points to the device extension.
    @parm   IN PIRP | Irp | Points to an I/O Request Packet.

    @rvalue SUCCESS | returns STATUS_SUCCESS.
    @rvalue FAILURE | returns NT status code.
--*/

NTSTATUS INTERNAL
GetDeviceDescriptor(
    __in PDEVICE_EXTENSION DevExt,
    __in PIRP              Irp
    )
{
    NTSTATUS status;
    PIO_STACK_LOCATION irpsp = IoGetCurrentIrpStackLocation(Irp);
#ifdef _DBG
    TEnter(Func, ("(DevExt=%p,Irp=%p,IrpSp=%p)", DevExt, Irp, irpsp));
#else
    UNREFERENCED_PARAMETER(DevExt);
#endif

    PAGED_CODE ();

    if (irpsp->Parameters.DeviceIoControl.OutputBufferLength <
        sizeof(gHidDescriptor))
    {
        status = STATUS_BUFFER_TOO_SMALL;
        LogError(ERRLOG_BUFFER_TOO_SMALL,
                 status,
                 UNIQUE_ERRID(0x40),
                 NULL,
                 NULL);
        TWarn(("Output buffer too small (len=%d).",
               irpsp->Parameters.DeviceIoControl.OutputBufferLength));
    }
    else
    {
        RtlCopyMemory(Irp->UserBuffer,
                      &gHidDescriptor,
                      sizeof(gHidDescriptor));

        Irp->IoStatus.Information = sizeof(gHidDescriptor);
        status = STATUS_SUCCESS;
    }
#ifdef _DBG
    TExit(Func, ("=%x", status));
#endif
    return status;
}       //GetDeviceDescriptor

/*++
    @doc    INTERNAL

    @func   NTSTATUS | GetReportDescriptor | Respond to
            HIDCLASS IOCTL_HID_GET_REPORT_DESCRIPTOR by returning
            appropriate the report descriptor.

    @parm   IN PDEVICE_EXTENSION | DevExt | Points to the device extension.
    @parm   IN PIRP | Irp | Points to an I/O Request Packet.

    @rvalue SUCCESS | returns STATUS_SUCCESS.
    @rvalue FAILURE | returns NT status code.
--*/

NTSTATUS INTERNAL
GetReportDescriptor(
    __in PDEVICE_EXTENSION DevExt,
    __in PIRP              Irp
    )
{
    NTSTATUS status;
    PIO_STACK_LOCATION irpsp = IoGetCurrentIrpStackLocation(Irp);

    
#ifdef _DBG
    TEnter(Func, ("(DevExt=%p,Irp=%p,IrpSp=%p)", DevExt, Irp, irpsp));
#else
    UNREFERENCED_PARAMETER(DevExt);
#endif

    PAGED_CODE ();

    if (irpsp->Parameters.DeviceIoControl.OutputBufferLength <
        gdwcbReportDescriptor)
    {
        status = STATUS_BUFFER_TOO_SMALL;
        LogError(ERRLOG_BUFFER_TOO_SMALL,
                 status,
                 UNIQUE_ERRID(0x50),
                 NULL,
                 NULL);
        TWarn(("Output buffer too small (len=%d).",
               irpsp->Parameters.DeviceIoControl.OutputBufferLength));
    }
    else
    {
        RtlCopyMemory(Irp->UserBuffer,
                      gReportDescriptor,
                      gdwcbReportDescriptor);

        Irp->IoStatus.Information = gdwcbReportDescriptor;
        status = STATUS_SUCCESS;
    }

#ifdef _DBG
    TExit(Func, ("=%x", status));
#endif
    return status;
}       //GetReportDescriptor

/*++
    @doc    INTERNAL

    @func   NTSTATUS | ReadReport | Read input report.

    @parm   IN PDEVICE_EXTENSION | DevExt | Points to the device extension.
    @parm   IN PIRP | Irp | Points to an I/O Request Packet.
    @parm   IN PIRP | fSentDown | Points to BOOLEAN for checking if the IRP was sent down the stack

    @rvalue SUCCESS | returns STATUS_SUCCESS.
    @rvalue FAILURE | returns NT status code.

    @note   This could be called at DISPATCH_LEVEL.
--*/

NTSTATUS INTERNAL
ReadReport(
    __in  PDEVICE_EXTENSION DevExt,
    __in  PIRP              Irp,
    __out BOOLEAN           *fSentDown
    )
{
    NTSTATUS status;
    PIO_STACK_LOCATION irpsp = IoGetCurrentIrpStackLocation(Irp);
    ULONG DataLen = irpsp->Parameters.DeviceIoControl.OutputBufferLength;

    TEnter(Func, ("(DevExt=%p,Irp=%p,IrpSp=%p)", DevExt, Irp, irpsp));
    TAssert(Irp->UserBuffer != NULL);
    if (DataLen != sizeof(HID_INPUT_REPORT))
    {
        status = STATUS_INVALID_BUFFER_SIZE;
        LogError(ERRLOG_INVALID_BUFFER_SIZE,
                 status,
                 UNIQUE_ERRID(0x60),
                 NULL,
                 NULL);
        TWarn(("Invalid input report size (len=%d,Expected=%d).",
               DataLen, sizeof(HID_INPUT_REPORT)));
    }
    else
    {
        status = OemReadReport(DevExt, Irp, fSentDown);
    }

    TExit(Func, ("=%x", status));
    return status;
}       //ReadReport

/*++
    @doc    INTERNAL

    @func   NTSTATUS | GetString | Respond to IOCTL_HID_GET_STRING.

    @parm   IN PDEVICE_EXTENSION | DevExt | Points to the device extension.
    @parm   IN PIRP | Irp | Points to an I/O Request Packet.

    @rvalue SUCCESS | returns STATUS_SUCCESS.
    @rvalue FAILURE | returns NT status code.
--*/

NTSTATUS INTERNAL
GetString(
    __in PDEVICE_EXTENSION DevExt,
    __in PIRP              Irp
    )
{
    NTSTATUS status;
    PIO_STACK_LOCATION irpsp = IoGetCurrentIrpStackLocation(Irp);
    PWSTR pwstrID;
    ULONG_PTR lenID;

    PAGED_CODE();
#ifdef _DBG
    TEnter(Func, ("(DevExt=%p,Irp=%p,IrpSp=%p,StringID=%p)",
                  DevExt, Irp, irpsp,
                  irpsp->Parameters.DeviceIoControl.Type3InputBuffer));
#else
    UNREFERENCED_PARAMETER(DevExt);
#endif

    switch ((ULONG_PTR)irpsp->Parameters.DeviceIoControl.Type3InputBuffer &
            0xffff)
    {
        case HID_STRING_ID_IMANUFACTURER:
            pwstrID = gpwstrManufacturerID;
            break;

        case HID_STRING_ID_IPRODUCT:
            pwstrID = gpwstrProductID;
            break;

        case HID_STRING_ID_ISERIALNUMBER:
            pwstrID = gpwstrSerialNumber;
            break;

        default:
            pwstrID = NULL;
            break;
    }

    lenID = pwstrID? wcslen(pwstrID)*sizeof(WCHAR) + sizeof(UNICODE_NULL): 0;
    if (pwstrID == NULL)
    {
        status = STATUS_INVALID_PARAMETER;
        LogError(ERRLOG_INVALID_PARAMETER,
                 status,
                 UNIQUE_ERRID(0x70),
                 NULL,
                 NULL);
        TWarn(("Invalid string ID (ID=%p).",
               irpsp->Parameters.DeviceIoControl.Type3InputBuffer));
    }
    else if (irpsp->Parameters.DeviceIoControl.OutputBufferLength < lenID)
    {
        status = STATUS_BUFFER_TOO_SMALL;
        LogError(ERRLOG_BUFFER_TOO_SMALL,
                 status,
                 UNIQUE_ERRID(0x80),
                 NULL,
                 NULL);
        TWarn(("Output buffer too small (len=%d,need=%d).",
               irpsp->Parameters.DeviceIoControl.OutputBufferLength,
               (int)lenID));
    }
    else
    {
        RtlCopyMemory(Irp->UserBuffer, pwstrID, lenID);

        Irp->IoStatus.Information = lenID;
        status = STATUS_SUCCESS;
    }

#ifdef _DBG
    TExit(Func, ("=%x (string=%S)", status, pwstrID? pwstrID: L"Null"));
#endif
    return status;
}       //GetString

/*++
    @doc    INTERNAL

    @func   NTSTATUS | GetAttributes | Respond to IOCTL_HID_GET_ATTRIBUTES,
            by filling the HID_DEVICE_ATTRIBUTES struct.

    @parm   IN PDEVICE_EXTENSION | DevExt | Points to the device extension.
    @parm   IN PIRP | Irp | Points to an I/O Request Packet.

    @rvalue SUCCESS | returns STATUS_SUCCESS.
    @rvalue FAILURE | returns NT status code.
--*/

NTSTATUS INTERNAL
GetAttributes(
    __in PDEVICE_EXTENSION DevExt,
    __in PIRP              Irp
    )
{
    NTSTATUS status;
    PIO_STACK_LOCATION irpsp = IoGetCurrentIrpStackLocation(Irp);;

    PAGED_CODE();
    TEnter(Func, ("(DevExt=%p,Irp=%p,IrpSp=%p)", DevExt, Irp, irpsp));

    if (irpsp->Parameters.DeviceIoControl.OutputBufferLength <
        sizeof(HID_DEVICE_ATTRIBUTES))
    {
        status = STATUS_BUFFER_TOO_SMALL;
        LogError(ERRLOG_BUFFER_TOO_SMALL,
                 status,
                 UNIQUE_ERRID(0x90),
                 NULL,
                 NULL);
        TWarn(("Output buffer too small (len=%d).",
               irpsp->Parameters.DeviceIoControl.OutputBufferLength));
    }
    else
    {
        PHID_DEVICE_ATTRIBUTES DevAttrib;

        DevAttrib = (PHID_DEVICE_ATTRIBUTES)Irp->UserBuffer;

        DevAttrib->Size = sizeof(HID_DEVICE_ATTRIBUTES);
        DevAttrib->VendorID = OEM_VENDOR_ID;
        DevAttrib->ProductID = DevExt->OemData.wProductID;
        DevAttrib->VersionNumber = DevExt->OemData.wFirmwareVer;

        Irp->IoStatus.Information = sizeof(HID_DEVICE_ATTRIBUTES);
        status = STATUS_SUCCESS;
    }

    TExit(Func, ("=%x", status));
    return status;
}       //GetAttributes

