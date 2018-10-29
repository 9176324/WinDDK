/*++
    Copyright (c) Microsoft Corporation

    Module Name:
        hidpen.c

    Abstract:
        Serial Pen Tablet HID Driver.

    Environment:
        Kernel mode

--*/

#include "pch.h"

#define MODULE_ID                       0

#ifdef ALLOC_PRAGMA
  #pragma alloc_text(INIT, DriverEntry)
  #pragma alloc_text(PAGE, HpenCreateClose)
  #pragma alloc_text(PAGE, HpenSystemControl)
  #pragma alloc_text(PAGE, HpenAddDevice)
  #pragma alloc_text(PAGE, HpenUnload)
  #pragma alloc_text(PAGE, RegQueryValue)
  #pragma alloc_text(PAGE, RegSetValue)
#endif  //ifdef ALLOC_PRAGMA

/*++
    @doc    EXTERNAL

    @func   NTSTATUS | DriverEntry |
            Installable driver initialization entry point.
            <nl>This entry point is called directly by the I/O system.

    @parm   IN PDRIVER_OBJECT | DrvObj | Points to the driver object.
    @parm   IN PUNICODE_STRINT | RegPath | Points to the registry path.

    @rvalue SUCCESS | returns STATUS_SUCCESS.
    @rvalue FAILURE | returns NT status code.
--*/

NTSTATUS EXTERNAL
DriverEntry(
    __in PDRIVER_OBJECT  DrvObj,
    __in PUNICODE_STRING RegPath
    )
{
    NTSTATUS status = STATUS_SUCCESS;
    HID_MINIDRIVER_REGISTRATION hidMinidriverRegistration;

    TEnter(API, ("(DrvObj=%p,RegPath=%p)", DrvObj, RegPath));

    gDriverObj = DrvObj;
    DrvObj->MajorFunction[IRP_MJ_CREATE] =
    DrvObj->MajorFunction[IRP_MJ_CLOSE] = HpenCreateClose;
    DrvObj->MajorFunction[IRP_MJ_SYSTEM_CONTROL] = HpenSystemControl;

    DrvObj->MajorFunction[IRP_MJ_INTERNAL_DEVICE_CONTROL] = HpenInternalIoctl;

    DrvObj->MajorFunction[IRP_MJ_PNP]   = HpenPnp;
    DrvObj->MajorFunction[IRP_MJ_POWER] = HpenPower;
    DrvObj->DriverUnload                = HpenUnload;
    DrvObj->DriverExtension->AddDevice  = HpenAddDevice;

    //
    // Register with HIDCLASS.SYS module
    //
    RtlZeroMemory(&hidMinidriverRegistration,
                  sizeof(hidMinidriverRegistration));

    hidMinidriverRegistration.Revision            = HID_REVISION;
    hidMinidriverRegistration.DriverObject        = DrvObj;
    hidMinidriverRegistration.RegistryPath        = RegPath;
    hidMinidriverRegistration.DeviceExtensionSize = sizeof(DEVICE_EXTENSION);
    hidMinidriverRegistration.DevicesArePolled    = FALSE;

    status = HidRegisterMinidriver(&hidMinidriverRegistration);

    if (!NT_SUCCESS(status))
    {
        LogError(ERRLOG_MINIDRV_REG_FAILED,
                 status,
                 UNIQUE_ERRID(0x10),
                 NULL,
                 NULL);
        TErr(("Failed to register mini driver (status=%x).", status));
    }

    TExit(API, ("=%x", status));
    return status;
}       //DriverEntry

/*++
    @doc    EXTERNAL

    @func   NTSTATUS | HpenCreateClose |
            Process the create and close IRPs sent to this device.

    @parm   IN PDEVICE_OBJECT | DevObj | Points to the device object.
    @parm   IN PIRP | Irp | Points to an I/O Request Packet.

    @rvalue SUCCESS | returns STATUS_SUCCESS.
    @rvalue FAILURE | returns NT status code.
--*/

NTSTATUS EXTERNAL
HpenCreateClose(
    IN PDEVICE_OBJECT DevObj,
    IN PIRP Irp
    )
{
    NTSTATUS status = STATUS_SUCCESS;

    PAGED_CODE ();
    UNREFERENCED_PARAMETER(DevObj);
    TEnter(API, ("(DevObj=%p,Irp=%p)", DevObj, Irp));

    Irp->IoStatus.Information = 0;
    Irp->IoStatus.Status = status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    TExit(API, ("=%x", status));
    return status;
}       //HpenCreateClose

/*++
    @doc    EXTERNAL

    @func   NTSTATUS | HpenSystemControl |
            Process the System Control IRPs sent to this device.

    @parm   IN PDEVICE_OBJECT | DevObj | Points to the device object.
    @parm   IN PIRP | Irp | Points to an I/O Request Packet.

    @rvalue SUCCESS | returns STATUS_SUCCESS.
    @rvalue FAILURE | returns NT status code.
--*/

NTSTATUS EXTERNAL
HpenSystemControl(
    IN PDEVICE_OBJECT DevObj,
    IN PIRP Irp
    )
{
    NTSTATUS status;
    PDEVICE_EXTENSION devext = GET_MINIDRIVER_DEVICE_EXTENSION(DevObj);

    PAGED_CODE ();
    TEnter(API, ("(DevObj=%p,Irp=%p)", DevObj, Irp));

    status = IoAcquireRemoveLock(&devext->RemoveLock, Irp);
    if (!NT_SUCCESS(status))
    {
        //
        // Someone sent us another plug and play IRP after removed
        //
        LogError(ERRLOG_DEVICE_REMOVED,
                 status,
                 UNIQUE_ERRID(0x20),
                 NULL,
                 NULL);
        TWarn(("Received IRP after device was removed (status=%x).", status));
        Irp->IoStatus.Information = 0;
        Irp->IoStatus.Status = status;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
    }
    else
    {
        IoSkipCurrentIrpStackLocation(Irp);
        TEnter(Func, (".IoCallDriver(DevObj=%p,Irp=%p)",
                      devext->LowerDevObj, Irp));
        status = IoCallDriver(devext->LowerDevObj, Irp);
        TExit(Func, (".IoCallDriver=%x", status));
        IoReleaseRemoveLock(&devext->RemoveLock, Irp);
    }

    TExit(API, ("=%x", status));
    return status;
}       //HpenSystemControl

/*++
    @doc    EXTERNAL

    @func   NTSTATUS | HpenAddDevice |
            Called by hidclass, allows us to initialize our device extensions.

    @parm   IN PDRIVER_OBJECT | DrvObj | Points to the driver object.
    @parm   IN PDEVICE_OBJECT | DevObj |
            Points to a functional device object created by hidclass.

    @rvalue SUCCESS | Returns STATUS_SUCCESS.
    @rvalue FAILURE | Returns NT status code.
--*/

NTSTATUS EXTERNAL
HpenAddDevice(
    __in PDRIVER_OBJECT DrvObj,
    __in PDEVICE_OBJECT DevObj
    )
{
    NTSTATUS status;
    PDEVICE_EXTENSION devext = GET_MINIDRIVER_DEVICE_EXTENSION(DevObj);

    PAGED_CODE ();

    TEnter(API, ("(DrvObj=%p,DevObj=%p)", DrvObj, DevObj));

    TAssert(DevObj != NULL);
    UNREFERENCED_PARAMETER(DrvObj);

    RtlZeroMemory(devext, sizeof(*devext));
    devext->DevObj = DevObj;
    devext->pdo = GET_PDO(DevObj);
    devext->LowerDevObj = GET_NEXT_DEVICE_OBJECT(DevObj);
    IoInitializeRemoveLock(&devext->RemoveLock, HPEN_POOL_TAG, 0, 10);
    KeInitializeSpinLock(&devext->SpinLock);
    devext->PowerState = PowerDeviceD0;
    status = OemAddDevice(devext);
    if (NT_SUCCESS(status))
    {
        DevObj->Flags &= ~DO_DEVICE_INITIALIZING;
        DevObj->Flags |= DO_POWER_PAGABLE;
    }

    TExit(API, ("=%x", status));
    return status;
}       //HpenAddDevice

/*++
    @doc    EXTERNAL

    @func   void | HpenUnload | Free all the allocated resources, etc.

    @parm   IN PDRIVER_OBJECT | DrvObj | Points to the driver object.

    @rvalue None.
--*/

VOID EXTERNAL
HpenUnload(
    __in PDRIVER_OBJECT DrvObj
    )
{
    PAGED_CODE();

    TEnter(API, ("(DrvObj=%p)", DrvObj));

    TAssert(DrvObj->DeviceObject == NULL);
    UNREFERENCED_PARAMETER(DrvObj);

    TExit(API, ("!"));
    return;
}       //HpenUnload

/*++
    @doc    INTERNAL

    @func   NTSTATUS | RegQueryValue | Read registry data.

    @parm   IN HANDLE | hkey | Registry handle.
    @parm   IN LPCWSTR | pszValueName | Points to the value name string.
    @parm   IN ULONG | dwType | Registry data type.
    @parm   OUT PUCHAR | lpbData | To hold the value data.
    @parm   IN ULONG | dwcbData | Specifies the buffer size.
    @parm   OUT PULONG | pdwcbLen | To hold the require data length (optional).

    @rvalue SUCCESS | Returns STATUS_SUCCESS.
    @rvalue FAILURE | Returns NT status code.
--*/

NTSTATUS INTERNAL
RegQueryValue(
    __in  HANDLE  hkey,
    __in  LPCWSTR pszValueName,
    __in  ULONG   dwType,
    __out_bcount(dwcbData) PUCHAR  lpbData,
    __in  ULONG   dwcbData,
    __out_opt PULONG  pdwcbLen 
    )
{
    NTSTATUS status;
    UNICODE_STRING ucstrName;
    PKEY_VALUE_PARTIAL_INFORMATION RegInfo;
    ULONG dwLen;

    PAGED_CODE ();
    TEnter(Func, ("(hkey=%p,ValueName=%S,Type=%x,lpbData=%p,Len=%d,pdwLen=%p)",
                  hkey, pszValueName, dwType, lpbData, dwcbData,
                  pdwcbLen));

    TAssert(lpbData != NULL);
    RtlInitUnicodeString(&ucstrName, pszValueName);
    dwLen = FIELD_OFFSET(KEY_VALUE_PARTIAL_INFORMATION, Data) + dwcbData;
    RegInfo = ExAllocatePoolWithTag(PagedPool, dwLen, HPEN_POOL_TAG);
    if (RegInfo != NULL)
    {
        status = ZwQueryValueKey(hkey,
                                 &ucstrName,
                                 KeyValuePartialInformation,
                                 RegInfo,
                                 dwLen,
                                 &dwLen);
        if (NT_SUCCESS(status))
        {
            if ((dwType != REG_NONE) && (dwType != RegInfo->Type))
            {
                status = STATUS_OBJECT_TYPE_MISMATCH;
                LogError(ERRLOG_REGTYPE_MISMATCH,
                         status,
                         UNIQUE_ERRID(0x30),
                         NULL,
                         NULL);
                TErr(("Registry data type mismatch (Type=%x,Expected=%x).",
                      RegInfo->Type, dwType));
            }
            else
            {
                TAssert(RegInfo->DataLength <= dwcbData);
                if(RegInfo->DataLength <= dwcbData)
                {
                    RtlCopyMemory(lpbData, RegInfo->Data, RegInfo->DataLength);
                }
                else
                {
                    TErr(("Insufficient buffer size! buffer size:%d, Data size:%d", dwcbData, RegInfo->DataLength));
                    status = STATUS_BUFFER_TOO_SMALL;
                }
            }
        }

        if ((pdwcbLen != NULL) &&
            ((status == STATUS_SUCCESS) ||
             (status == STATUS_BUFFER_TOO_SMALL)))
        {
            *pdwcbLen = RegInfo->DataLength;
        }
        ExFreePool(RegInfo);
    }
    else
    {
        status = STATUS_INSUFFICIENT_RESOURCES;
        LogError(ERRLOG_INSUFFICIENT_RESOURCES,
                 status,
                 UNIQUE_ERRID(0x40),
                 NULL,
                 NULL);
        TErr(("Failed to allocate registry info (len=%d).", dwcbData));
    }

    TExit(Func, ("=%x", status));
    return status;
}       //RegQueryValue
/*++
    @doc    INTERNAL

    @func   NTSTATUS | RegSetValue | Write registry data.

    @parm   IN HANDLE | hkey | Registry handle.
    @parm   IN LPCWSTR | pszValueName | Points to the value name string.
    @parm   IN ULONG | dwType | Registry data type.
    @parm   IN PUCHAR | lpbData | Points to the data to be written to the
            registry.
    @parm   IN ULONG | dwcbData | Specifies the buffer size.

    @rvalue SUCCESS | Returns STATUS_SUCCESS.
    @rvalue FAILURE | Returns NT status code.
--*/

NTSTATUS INTERNAL
RegSetValue(
    __in HANDLE  hkey,
    __in LPCWSTR pszValueName,
    __in ULONG   dwType,
    __in_bcount_opt(dwcbData) PUCHAR  lpbData,
    __in ULONG   dwcbData
    )
{
    NTSTATUS status;
    UNICODE_STRING ucstrName;


    PAGED_CODE ();
    TEnter(Func, ("(hkey=%p,ValueName=%S,Type=%x,lpbData=%p,Len=%d)",
                  hkey, pszValueName, dwType, lpbData, dwcbData));

    TAssert(lpbData != NULL);
    RtlInitUnicodeString(&ucstrName, pszValueName);
    status = ZwSetValueKey(hkey,
                           &ucstrName,
                           0,
                           dwType,
                           lpbData,
                           dwcbData);
    if (!NT_SUCCESS(status))
    {
        LogError(ERRLOG_WRITEREG_FAILED,
                 status,
                 UNIQUE_ERRID(0x50),
                 NULL,
                 NULL);
        TErr(("Failed to write registry value %S (status=%x).",
              pszValueName, status));
    }

    TExit(Func, ("=%x", status));
    return status;
} //RegSetValue

