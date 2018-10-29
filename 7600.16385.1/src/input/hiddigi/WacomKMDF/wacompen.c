/*++
    Copyright (c) Microsoft Corporation

    Module Name:
        wacompen.c

    Abstract:
        Contains wacom specific functions.

    Environment:
        Kernel mode

--*/

#include "pch.h"
#ifdef DEBUG
  #define HID_NAME_DATA
  #define SERIAL_NAME_DATA
#endif

#define MODULE_ID                      0

#define BYTEINDEX_XMAX                  61
#define BYTEINDEX_YMAX                  71
#define BYTEINDEX_PRESSUREMAX           81
#define BYTEINDEX_XTILTMIN              90
#define BYTEINDEX_XTILTMAX              92
#define BYTEINDEX_YTILTMIN              98
#define BYTEINDEX_YTILTMAX              100
#define BYTEINDEX_PHYSICAL_XMAX         58
#define BYTEINDEX_PHYSICAL_YMAX         68
#define MAX_USHORT                      65535
#define RETRY_LIMIT                     2
#define POLL_LIMIT                      60

UCHAR gReportDescriptor[] = {
    0x05, 0x0d,                         // USAGE_PAGE (Digitizers)          0
    0x09, 0x02,                         // USAGE (Pen)                      2
    0xa1, 0x01,                         // COLLECTION (Application)         4
    0x85, REPORTID_PEN,                 //   REPORT_ID (Pen)                6
    0x09, 0x20,                         //   USAGE (Stylus)                 8
    0xa1, 0x00,                         //   COLLECTION (Physical)          10
    0x09, 0x42,                         //     USAGE (Tip Switch)           12
    0x09, 0x44,                         //     USAGE (Barrel Switch)        14
    0x09, 0x3c,                         //     USAGE (Invert)               16
    0x09, 0x45,                         //     USAGE (Eraser Switch)        18
    0x15, 0x00,                         //     LOGICAL_MINIMUM (0)          20
    0x25, 0x01,                         //     LOGICAL_MAXIMUM (1)          22
    0x75, 0x01,                         //     REPORT_SIZE (1)              24
    0x95, 0x04,                         //     REPORT_COUNT (4)             26
    0x81, 0x02,                         //     INPUT (Data,Var,Abs)         28
    0x95, 0x01,                         //     REPORT_COUNT (1)             30
    0x81, 0x03,                         //     INPUT (Cnst,Var,Abs)         32
    0x09, 0x32,                         //     USAGE (In Range)             34
    0x81, 0x02,                         //     INPUT (Data,Var,Abs)         36
    0x95, 0x02,                         //     REPORT_COUNT (2)             38
    0x81, 0x03,                         //     INPUT (Cnst,Var,Abs)         40
    0x05, 0x01,                         //     USAGE_PAGE (Generic Desktop) 42
    0x09, 0x30,                         //     USAGE (X)                    44
    0x75, 0x10,                         //     REPORT_SIZE (16)             46
    0x95, 0x01,                         //     REPORT_COUNT (1)             48
    0xa4,                               //     PUSH                         50
    0x55, 0x0d,                         //     UNIT_EXPONENT (-3)           51
    0x65, 0x33,                         //     UNIT (Inch,EngLinear)        53
    0x35, 0x00,                         //     PHYSICAL_MINIMUM (0)         55
    0x46, 0x3a, 0x20,                   //     PHYSICAL_MAXIMUM (8250)      57
    0x26, 0xf8, 0x52,                   //     LOGICAL_MAXIMUM (21240)      60
    0x81, 0x02,                         //     INPUT (Data,Var,Abs)         63
    0x09, 0x31,                         //     USAGE (Y)                    65
    0x46, 0x2c, 0x18,                   //     PHYSICAL_MAXIMUM (6188)      67
    0x26, 0x6c, 0x3e,                   //     LOGICAL_MAXIMUM (15980)      70
    0x81, 0x02,                         //     INPUT (Data,Var,Abs)         73
    0xb4,                               //     POP                          75
    0x05, 0x0d,                         //     USAGE_PAGE (Digitizers)      76
    0x09, 0x30,                         //     USAGE (Tip Pressure)         78
    0x26, 0xff, 0x00,                   //     LOGICAL_MAXIMUM (255)        80
    0x81, 0x02,                         //     INPUT (Data,Var,Abs)         83
    0x75, 0x08,                         //     REPORT_SIZE (8)              85
    0x09, 0x3d,                         //     USAGE (X Tilt)               87
    0x15, 0x81,                         //     LOGICAL_MINIMUM (-127)       89
    0x25, 0x7f,                         //     LOGICAL_MAXIMUM (127)        91
    0x81, 0x02,                         //     INPUT (Data,Var,Abs)         93
    0x09, 0x3e,                         //     USAGE (Y Tilt)               95
    0x15, 0x81,                         //     LOGICAL_MINIMUM (-127)       97
    0x25, 0x7f,                         //     LOGICAL_MAXIMUM (127)        99
    0x81, 0x02,                         //     INPUT (Data,Var,Abs)         101/103
#ifdef _TIMESTAMP_
    0x06, 0x00, 0xff,                   //     USAGE_PAGE (Vendor Defined)  0
    0x09, 0x80,                         //     USAGE (Vendor Usage 0x80)    3
    0x09, 0x81,                         //     USAGE (Vendor Usage 0x81)    5
    0x15, 0x00,                         //     LOGICAL_MINIMUM (0)          7
    0x27, 0xff, 0xff, 0xff, 0xff,       //     LOGICAL_MAXIMUM (0xffffffff) 9
    0x75, 0x20,                         //     REPORT_SIZE (32)             14
    0x95, 0x02,                         //     REPORT_COUNT (2)             16
    0x81, 0x02,                         //     INPUT (Data,Var,Abs)         18/20
#endif
    0xc0,                               //   END_COLLECTION                 0
    0xc0,                               // END_COLLECTION                   1/2
    //
    // Dummy mouse collection starts here
    //
    0x05, 0x01,                         // USAGE_PAGE (Generic Desktop)     0
    0x09, 0x02,                         // USAGE (Mouse)                    2
    0xa1, 0x01,                         // COLLECTION (Application)         4
    0x85, REPORTID_MOUSE,               //   REPORT_ID (Mouse)              6
    0x09, 0x01,                         //   USAGE (Pointer)                8
    0xa1, 0x00,                         //   COLLECTION (Physical)          10
    0x05, 0x09,                         //     USAGE_PAGE (Button)          12
    0x19, 0x01,                         //     USAGE_MINIMUM (Button 1)     14
    0x29, 0x02,                         //     USAGE_MAXIMUM (Button 2)     16
    0x15, 0x00,                         //     LOGICAL_MINIMUM (0)          18
    0x25, 0x01,                         //     LOGICAL_MAXIMUM (1)          20
    0x75, 0x01,                         //     REPORT_SIZE (1)              22
    0x95, 0x02,                         //     REPORT_COUNT (2)             24
    0x81, 0x02,                         //     INPUT (Data,Var,Abs)         26
    0x95, 0x06,                         //     REPORT_COUNT (6)             28
    0x81, 0x03,                         //     INPUT (Cnst,Var,Abs)         30
    0x05, 0x01,                         //     USAGE_PAGE (Generic Desktop) 32
    0x09, 0x30,                         //     USAGE (X)                    34
    0x09, 0x31,                         //     USAGE (Y)                    36
    0x75, 0x10,                         //     REPORT_SIZE (16)             38
    0x95, 0x02,                         //     REPORT_COUNT (2)             40
    0x15, 0x00,                         //     LOGICAL_MINIMUM (0)          42
    0x26, 0xff, 0x7f,                   //     LOGICAL_MAXIMUM (32767)      44
    0x81, 0x02,                         //     INPUT (Data,Var,Abs)         47
    0xc0,                               //   END_COLLECTION                 49
    0xc0                                // END_COLLECTION                   50/51
};
ULONG gdwcbReportDescriptor = sizeof(gReportDescriptor);

HID_DESCRIPTOR gHidDescriptor =
{
    sizeof(HID_DESCRIPTOR),             //bLength
    HID_HID_DESCRIPTOR_TYPE,            //bDescriptorType
    HID_REVISION,                       //bcdHID
    0,                                  //bCountry - not localized
    1,                                  //bNumDescriptors
    {                                   //DescriptorList[0]
        HID_REPORT_DESCRIPTOR_TYPE,     //bReportType
        sizeof(gReportDescriptor)       //wReportLength
    }
};

PWSTR gpwstrManufacturerID = L"Wacom";
PWSTR gpwstrProductID = L"ISD V4";
PWSTR gpwstrSerialNumber = L"0";


#ifdef ALLOC_PRAGMA
  #pragma alloc_text(PAGE, WacompenDeviceAdd)
  #pragma alloc_text(PAGE, NInputDriverContextCleanup)
  #pragma alloc_text(PAGE, NInputInitDevice)
  #pragma alloc_text(PAGE, QueryDeviceInfo)
#endif


NTSTATUS
DriverEntry (
    __in PDRIVER_OBJECT  DriverObject,
    __in PUNICODE_STRING RegistryPath
    )
/*++

Routine Description:

    Installable driver initialization entry point.
    This entry point is called directly by the I/O system.

Arguments:

    DriverObject - pointer to the driver object

    RegistryPath - pointer to a unicode string representing the path,
                   to driver-specific key in the registry.

Return Value:

    STATUS_SUCCESS if successful,
    STATUS_UNSUCCESSFUL otherwise.

--*/
{
    NTSTATUS               status = STATUS_SUCCESS;
    WDF_DRIVER_CONFIG      config = {0};
    WDF_OBJECT_ATTRIBUTES  attributes = {0};

    
    WDF_DRIVER_CONFIG_INIT(&config, WacompenDeviceAdd);

    //
    // Register a cleanup callback so that we can call WPP_CLEANUP when
    // the framework driver object is deleted during driver unload.
    //
    WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
    attributes.EvtCleanupCallback = NInputDriverContextCleanup;

    //
    // Create a framework driver object to represent our driver.
    //
    status = WdfDriverCreate(DriverObject,
                             RegistryPath,
                             &attributes, // Driver Attributes
                             &config,          // Driver Config Info
                             WDF_NO_HANDLE
                             );

    if (NT_SUCCESS(status)) {
    }
    else{
        KdPrint(("failed to create driver object status=%x", status));
    }

    return status;
}

NTSTATUS
WacompenDeviceAdd(
    IN WDFDRIVER       Driver,
    IN PWDFDEVICE_INIT DeviceInit
    )
/*++
Routine Description:

    NInputDeviceAdd is called by the framework in response to AddDevice
    call from the PnP manager. We create and initialize a WDF device object to
    represent a new instance of toaster device.

Arguments:

    Driver - Handle to a framework driver object created in DriverEntry

    DeviceInit - Pointer to a framework-allocated WDFDEVICE_INIT structure.

Return Value:

    NTSTATUS

--*/
{
    NTSTATUS                      status = STATUS_SUCCESS;
    WDF_IO_QUEUE_CONFIG           queueConfig = {0};
    WDF_OBJECT_ATTRIBUTES         attributes = {0};
    WDFDEVICE                     hDevice = {0};
    PDEVICE_EXTENSION             devContext = NULL;
    WDFQUEUE                      queue = {0};
    WDF_PNPPOWER_EVENT_CALLBACKS  pnpPowerCallbacks;
    
    UNREFERENCED_PARAMETER(Driver);

    PAGED_CODE();

   
    //
    // Relinquish power policy ownership because HIDCLASS acts a power
    // policy owner for ther HID stack.
    //
    WdfDeviceInitSetPowerPolicyOwnership(DeviceInit, FALSE);

    

    //
    // Initialize pnp-power callbacks, attributes and a context area for the device object.
    //
    //
    WDF_PNPPOWER_EVENT_CALLBACKS_INIT(&pnpPowerCallbacks);

       //
    // These two callbacks start and stop the wdfusb pipe continuous reader
    // as we go in and out of the D0-working state.
    //
    pnpPowerCallbacks.EvtDeviceD0Entry = NInputEvtDeviceD0Entry;
    pnpPowerCallbacks.EvtDeviceD0Exit  = NInputEvtDeviceD0Exit;
    pnpPowerCallbacks.EvtDeviceQueryStop = NInputEvtDeviceStop;
    pnpPowerCallbacks.EvtDeviceQueryRemove = NInputEvtDeviceQueryRemove;
    
    WdfDeviceInitSetPnpPowerEventCallbacks(DeviceInit, &pnpPowerCallbacks);
    
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes, DEVICE_EXTENSION);

    //
    // Create a framework device object.This call will in turn create
    // a WDM device object, attach to the lower stack, and set the
    // appropriate flags and attributes.
    //
    status = WdfDeviceCreate(&DeviceInit, &attributes, &hDevice);
    if (!NT_SUCCESS(status)) {
        /*TraceEvents(TRACE_LEVEL_ERROR, DBG_PNP,
            "WdfDeviceCreate failed with status code 0x%x\n", status);*/
        return status;
    }

    devContext = GetDeviceContext(hDevice);
    devContext->Device = hDevice;
    //get the lower device.
    devContext->IoTarget = WdfDeviceGetIoTarget(hDevice);
    
      
    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&queueConfig, WdfIoQueueDispatchParallel);

    queueConfig.EvtIoInternalDeviceControl = NInputInternalDeviceControl;

    queueConfig.PowerManaged = WdfFalse;

    status = WdfIoQueueCreate(hDevice,
                              &queueConfig,
                              WDF_NO_OBJECT_ATTRIBUTES,
                              &queue
                              );
    if (!NT_SUCCESS (status)) {
       
        return status;
    }

    //
    // Register a manual I/O queue for Read Requests.
    // This queue will be used for storing Requests that need to wait for data 
    //  from the serial driver before they can be completed.
    //
    WDF_IO_QUEUE_CONFIG_INIT(&queueConfig, WdfIoQueueDispatchManual);

    //
    // This queue is used for requests that dont directly access the device. The
    // requests in this queue are serviced only when the device is in a fully
    // powered state and sends an interrupt. So we can use a non-power managed
    // queue to park the requests since we dont care whether the device is idle
    // or fully powered up.
    //
     queueConfig.PowerManaged = WdfFalse;

    status = WdfIoQueueCreate(hDevice,
                              &queueConfig,
                              WDF_NO_OBJECT_ATTRIBUTES,
                              &devContext->PingPongQueue
                              );

    if (!NT_SUCCESS(status)) {
        
        return status;
    }
    WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
    attributes.ParentObject = hDevice;
    status = WdfSpinLockCreate(
                           &attributes,
                           &devContext->SpinLock
                           );
    if(!NT_SUCCESS(status))
    {
        KdPrint(("Spink lock creation failed\n"));
        return status;
    }
    return status;
}


VOID
NInputDriverContextCleanup(
    IN WDFDRIVER Driver
    )
/*++
Routine Description:

    Free resources allocated in DriverEntry that are not automatically
    cleaned up framework.

Arguments:

    Driver - handle to a WDF Driver object.

Return Value:

    VOID.

--*/
{
    PAGED_CODE ();

    UNREFERENCED_PARAMETER(Driver);


}

NTSTATUS INTERNAL
NInputInitDevice(
     __in PDEVICE_EXTENSION DevContext
    )
/*++
Routine Description:

    This function writes/read serial data for initialization purposes.

Arguments:

    DevContext - Device context

Return Value:

    STATUS_SUCCESS if successful,
    NTSTATUS of failure otherwise.

--*/
{
    NTSTATUS status = STATUS_SUCCESS;
    WDFKEY hkey = {0};
    
    PAGED_CODE();
    TEnter(Func, ("(DevContext=%p)", DevContext));

    status = WdfDeviceOpenRegistryKey(DevContext->Device,
                                     PLUGPLAY_REGKEY_DEVICE,
                                     STANDARD_RIGHTS_READ | STANDARD_RIGHTS_WRITE,
                                     WDF_NO_OBJECT_ATTRIBUTES,
                                     &hkey);
    if (NT_SUCCESS(status))
    {
        status = NInputRegQueryValue(hkey,
                               L"DeviceInfo",
                               REG_BINARY,
                               (PUCHAR)&DevContext->OemData,
                               sizeof(DevContext->OemData),
                               NULL);
        if (!NT_SUCCESS(status)|| (DevContext->OemData.wXMax < 1) ||
                                (DevContext->OemData.wYMax < 1) ||
                                (DevContext->OemData.wPressureMax < 1))
        {
            //
            // Can't get the device info from the registry, probably
            // first time.  Let's try querying the hardware instead.
            //
            status = QueryDeviceInfo(DevContext);
            if (NT_SUCCESS(status) && (hkey != NULL))
            {
                status = NInputRegSetValue(hkey,
                                     L"DeviceInfo",
                                     REG_BINARY,
                                     (PUCHAR)&DevContext->OemData,
                                     sizeof(DevContext->OemData));
            }
        }
        else
        {
            //fill data in report descriptor
            status = SetDeviceReportDescriptor(DevContext);
        }
        if (NT_SUCCESS(status))
        {
            PRESSURE PressureThresholds = {0};

            status = NInputRegQueryValue(hkey,
                                   L"PressureThresholds",
                                   REG_BINARY,
                                   (PUCHAR)&PressureThresholds,
                                   sizeof(PressureThresholds),
                                   NULL);
            if (NT_SUCCESS(status))
            {
                if ((PressureThresholds.wTip <=
                     DevContext->OemData.wPressureMax) &&
                    (PressureThresholds.wEraser <=
                     DevContext->OemData.wPressureMax))
                {
                    TInfo(("RegData: TipThreshold=%d, EraserThreshold=%d",
                           PressureThresholds.wTip,
                           PressureThresholds.wEraser));
                    DevContext->OemData.PressureThresholds = PressureThresholds;
                }
                else
                {
                    TWarn(("Invalid pressure thresholds (Tip=%d,Eraser=%d)",
                           PressureThresholds.wTip,
                           PressureThresholds.wEraser));
                }

                if (DevContext->OemData.PressureThresholds.wTip <
                    MIN_PRESSURE_THRESHOLD)
                {
                    DevContext->OemData.PressureThresholds.wTip =
                        MIN_PRESSURE_THRESHOLD;
                }

                if (DevContext->OemData.PressureThresholds.wEraser <
                    MIN_PRESSURE_THRESHOLD)
                {
                    DevContext->OemData.PressureThresholds.wEraser =
                        MIN_PRESSURE_THRESHOLD;
                }
            }
            else
            {
                //
                // Pressure threshold is optional, it's OK if we don't find
                // it.  We will use default.
                //
                TWarn(("Failed to read pressure thresholds (status=%x).",
                       status));
            }

            status = SerialSyncReadWritePort(FALSE,
                                             DevContext,
                                             (PUCHAR)"1",
                                             1,
                                             NULL,
                                             NULL);
            if (!NT_SUCCESS(status))
            {
                LogError(ERRLOG_INIT_DIGITIZER_FAILED,
                         status,
                         UNIQUE_ERRID(0x10),
                         NULL,
                         NULL);
                TErr(("Failed to initialize digitizer (status=%x).", status));
            }
        }
        if(hkey != NULL)
        {
            ZwClose(hkey);
        }
    }
    else
    {
        LogError(ERRLOG_OPENREG_FAILED,
                 status,
                 UNIQUE_ERRID(0x20),
                 NULL,
                 NULL);
        TErr(("Failed to open device registry (status=%x).", status));
    }

    TExit(Func, ("=%x", status));
    return status;

}       

NTSTATUS INTERNAL
QueryDeviceInfo(
    __in PDEVICE_EXTENSION DevContext
    )
 /*++

  Routine Description:
       Reads/writes configuration data to /from the serial device

  Arguments:
     DevContext - Device context

  Return Value:

    STATUS_SUCCESS if successful,
    NTSTATUS of failure otherwise.
     
  --*/
{
    NTSTATUS status = STATUS_SUCCESS;
    
    PAGED_CODE();
    TEnter(Func, ("(DevContext=%p)", DevContext));

    status = SerialSyncReadWritePort(FALSE, DevContext, (PUCHAR)"0", 1, NULL, NULL);
    if (!NT_SUCCESS(status))
    {
        LogError(ERRLOG_QUERY_DEVINFO_FAILED,
                 status,
                 UNIQUE_ERRID(0xc0),
                 NULL,
                 NULL);
        TWarn(("Failed to stop the digitizer (status=%x).", status));
    }
    else
    {
        LARGE_INTEGER Timeout;
        ULONG cRetries;
        ULONG cPolls;

        Timeout.QuadPart = Int32x32To64(1000, -10000);
        KeDelayExecutionThread(KernelMode, FALSE, (LARGE_INTEGER *)&Timeout);

        for (cRetries = RETRY_LIMIT; cRetries > 0; --cRetries)
        {
            status = SerialSyncReadWritePort(FALSE, DevContext, (PUCHAR)"*", 1, NULL, NULL);
            if (!NT_SUCCESS(status))
            {
                LogError(ERRLOG_QUERY_DEVINFO_FAILED,
                         status,
                         UNIQUE_ERRID(0xd0),
                         NULL,
                         NULL);
                TWarn(("Failed to send the Query command (status=%x).",
                       status));
            }
            else
            {
                QUERYDATA Data = {0};
                ULONG_PTR BytesRead;
                WDF_REQUEST_SEND_OPTIONS   Options;

                #define INSTATUS_CTRLDATA (INSTATUS_SYNC |          \
                                           INSTATUS_CONTROL_DATA)

                
                WDF_REQUEST_SEND_OPTIONS_INIT(
                                &Options,
                                WDF_REQUEST_SEND_OPTION_SYNCHRONOUS 
                                );
                // Spec. says 160ms max., set timeout to 1 sec just
                // to be safe.
                WDF_REQUEST_SEND_OPTIONS_SET_TIMEOUT(
                                     &Options,
                                     WDF_REL_TIMEOUT_IN_SEC(1)
                                     );
                
                for(cPolls = POLL_LIMIT; cPolls > 0; cPolls--)
                {
                    status = SerialSyncReadWritePort(TRUE,
                                                         DevContext,
                                                         (PUCHAR)&Data,
                                                         1,
                                                         &Options,
                                                         &BytesRead);
                    //if the read succeeds and we get the expected value we break
                    //if the read fails, we also break.
                    if((STATUS_SUCCESS != status)  ||
                        (Data.QueryData.bStatus == INSTATUS_CTRLDATA))
                    {
                        break;
                    }
                }
                if (status == STATUS_SUCCESS &&
                    (Data.QueryData.bStatus == INSTATUS_CTRLDATA))
                {
                    status = SerialSyncReadWritePort(TRUE,
                                                     DevContext,
                                                     ((PUCHAR)&Data) + 1,
                                                     sizeof(Data) - 1,
                                                     &Options,
                                                     &BytesRead);
                    if (status == STATUS_SUCCESS)
                    {
                        if (BytesRead == sizeof(Data) - 1)
                        {
                            DevContext->OemData.wXMax =
                                (NORMALIZE_DATA(Data.QueryData.wXMax) << 2) |
                                LOW_X_BITS(Data.QueryData.wPressureMax);                            

                            DevContext->OemData.wYMax =
                                (NORMALIZE_DATA(Data.QueryData.wYMax) << 2) |
                                LOW_Y_BITS(Data.QueryData.wPressureMax);

                            DevContext->OemData.wPressureMax =
                                NORMALIZE_PRESSURE(Data.QueryData.wPressureMax);

                            DevContext->OemData.bXTiltMax =
                                Data.QueryData.bXTiltMax & ~INBDATA_SYNC;

                            DevContext->OemData.bYTiltMax =
                                Data.QueryData.bYTiltMax & ~INBDATA_SYNC;

                            DevContext->OemData.wFirmwareVer =
                                NORMALIZE_DATA(Data.QueryData.wFirmwareVer);                            
                            
                            status = SetDeviceReportDescriptor(DevContext);

                            TInfo(("XMax=%x(%d),YMax=%x(%d),PressureMax=%x(%d),XTiltMax=%x(%d),YTiltMax=%x(%d),FirmwareVer=%x(%d),TipPressThreshold=%d,EraserPressThreshold=%d",
                                   DevContext->OemData.wXMax,
                                   DevContext->OemData.wXMax,
                                   DevContext->OemData.wYMax,
                                   DevContext->OemData.wYMax,
                                   DevContext->OemData.wPressureMax,
                                   DevContext->OemData.wPressureMax,
                                   DevContext->OemData.bXTiltMax,
                                   DevContext->OemData.bXTiltMax,
                                   DevContext->OemData.bYTiltMax,
                                   DevContext->OemData.bYTiltMax,
                                   DevContext->OemData.wFirmwareVer,
                                   DevContext->OemData.wFirmwareVer,
                                   DevContext->OemData.PressureThresholds.wTip,
                                   DevContext->OemData.PressureThresholds.wEraser));
                            if ((DevContext->OemData.wXMax > 0) &&
                                (DevContext->OemData.wYMax > 0) &&
                                (DevContext->OemData.wPressureMax > 0))
                            {
                                //
                                // We have good data, no need to retry.
                                // Let's also store it back in the
                                // registry.
                                //
                                break;
                            }
                            else
                            {
                                status = STATUS_DEVICE_DATA_ERROR;
                                LogError(ERRLOG_QUERY_DEVINFO_FAILED,
                                         status,
                                         UNIQUE_ERRID(0xd8),
                                         NULL,
                                         NULL);
                                TWarn(("Invalid status response (XMax=%d,YMax=%d,PressureMax=%d,FirmwareVer=%x).",
                                       DevContext->OemData.wXMax,
                                       DevContext->OemData.wYMax,
                                       DevContext->OemData.wPressureMax,
                                       DevContext->OemData.wFirmwareVer));
                            }
                        }
                        else
                        {
                            status = STATUS_DEVICE_DATA_ERROR;
                            LogError(ERRLOG_QUERY_DEVINFO_FAILED,
                                     status,
                                     UNIQUE_ERRID(0xe0),
                                     NULL,
                                     NULL);
                            TWarn(("Invalid status response (size=%d,pData=%p).",
                                   (ULONG)BytesRead, &Data));
                        }
                    }
                    else
                    {
                        status = STATUS_DEVICE_DATA_ERROR;
                        LogError(ERRLOG_QUERY_DEVINFO_FAILED,
                                 status,
                                 UNIQUE_ERRID(0xf0),
                                 NULL,
                                 NULL);
                        TWarn(("Failed to read response packet (status=%x).",
                               status));
                    }
                }
                else
                {
                    status = STATUS_DEVICE_DATA_ERROR;
                    LogError(ERRLOG_QUERY_DEVINFO_FAILED,
                             status,
                             UNIQUE_ERRID(0x110),
                             NULL,
                             NULL);
                    TWarn(("Failed to read 1st response byte (status=%x).",
                           status));
                }
            }
        }
    }

    TExit(Func, ("=%x", status));
    return status;
}       //QueryDeviceInfo


BOOLEAN INTERNAL
OemIsResyncDataValid(
    __in PDEVICE_EXTENSION DevContext
    )
    /*++

  Routine Description:
       Validates resync buffer

  Arguments:
     DevContext - Device context

  Return Value:

    TRUE if successful,
    FALSE otherwise.
     
  --*/
{
    BOOLEAN rc = FALSE;

    TEnter(Func, ("(DevContext=%p)", DevContext));

    rc = IsValidPacket(&DevContext->OemData.ResyncData[0]);
    if ((rc == FALSE) ||
        (DevContext->OemData.BytesInBuff > sizeof(OEM_INPUT_REPORT)))
    {
        PUCHAR pb = (PUCHAR)&DevContext->OemData.ResyncData[0] +
                    DevContext->OemData.BytesInBuff - 1;
        PUCHAR pbEnd = rc? (PUCHAR)&DevContext->OemData.ResyncData[1]:
                           (PUCHAR)&DevContext->OemData.ResyncData[0];

        //
        // Even if we seem to have a valid packet in the resync buffer, we
        // still need to scan the next packet if any.  If the next packet
        // has a sync bit out of place, the first packet could still be
        // invalid and we better throw it away.
        //
        while (pb > pbEnd)
        {
            if (*pb & INSTATUS_SYNC)
            {
                TInfo(("Invalid buffer (len=%d,status0=%x,x0=%x,y0=%x,Pressure0=%x,XTilt0=%x,YTilt0=%x,status1=%x,x1=%x,y1=%x,Pressure1=%x,XTilt1=%x,YTilt1=%x)",
                       DevContext->OemData.BytesInBuff,
                       DevContext->OemData.ResyncData[0].InputReport.bStatus,
                       DevContext->OemData.ResyncData[0].InputReport.wXData,
                       DevContext->OemData.ResyncData[0].InputReport.wYData,
                       DevContext->OemData.ResyncData[0].InputReport.wPressureData,
                       DevContext->OemData.ResyncData[0].InputReport.bXTiltData,
                       DevContext->OemData.ResyncData[0].InputReport.bYTiltData,
                       DevContext->OemData.ResyncData[1].InputReport.bStatus,
                       DevContext->OemData.ResyncData[1].InputReport.wXData,
                       DevContext->OemData.ResyncData[1].InputReport.wYData,
                       DevContext->OemData.ResyncData[1].InputReport.wPressureData,
                       DevContext->OemData.ResyncData[1].InputReport.bXTiltData,
                       DevContext->OemData.ResyncData[1].InputReport.bYTiltData));
                DevContext->OemData.BytesInBuff = (ULONG)
                        ((PUCHAR)&DevContext->OemData.ResyncData[0] +
                         DevContext->OemData.BytesInBuff - pb);
                RtlMoveMemory(&DevContext->OemData.ResyncData[0],
                              pb,
                              DevContext->OemData.BytesInBuff);
                TInfo(("Resync'd buffer (len=%d,status=%x,x=%x,y=%x,Pressure=%x,XTilt=%x,YTilt=%x)",
                       DevContext->OemData.BytesInBuff,
                       DevContext->OemData.ResyncData[0].InputReport.bStatus,
                       DevContext->OemData.ResyncData[0].InputReport.wXData,
                       DevContext->OemData.ResyncData[0].InputReport.wYData,
                       DevContext->OemData.ResyncData[0].InputReport.wPressureData,
                       DevContext->OemData.ResyncData[0].InputReport.bXTiltData,
                       DevContext->OemData.ResyncData[0].InputReport.bYTiltData));
              #ifdef DEBUG
                {
                    ULONG dwcDeletedBytes = (ULONG)
                                (pb - (PUCHAR)&DevContext->OemData.ResyncData[0]);

                    DevContext->OemData.dwcLostBytes +=
                        (dwcDeletedBytes > sizeof(OEM_INPUT_REPORT))?
                                sizeof(OEM_INPUT_REPORT)*2 - dwcDeletedBytes:
                                sizeof(OEM_INPUT_REPORT) - dwcDeletedBytes;
                }
              #endif
                rc = FALSE;
                break;
            }
            --pb;
        }

        if ((rc == FALSE) && (pb <= pbEnd))
        {
            //
            // We didn't have a valid packet and we couldn't find the sync
            // bit of the next packet, so the whole resync buffer is invalid.
            //
            DevContext->OemData.BytesInBuff = 0;
        }
    }

    TExit(Func, ("=%x", rc));
    return rc;
}       //OemIsResyncDataValid



NTSTATUS INTERNAL
OemNormalizeInputData(
    __in     PDEVICE_EXTENSION DevContext,
    __inout POEM_INPUT_REPORT InData
    )
    /*++

    Routine Description:
      Normalizes data from device format.

    Arguments:
      DevContext - Device Context

      InData - Data from device
   
   Return Value:

    STATUS_SUCCESS if successful,
    NTSTATUS of failure otherwise.     

    --*/
{
    NTSTATUS    status = STATUS_SUCCESS;
    UCHAR       bCurStatus = 0;
    BOOLEAN     fInRange = FALSE;
    USHORT      wPressureThreshold = 0;
    PUSHORT     pwPressureBaseLine = NULL;
#ifdef DEBUG
    ULONGLONG CurrentTime;
#endif

    TEnter(Func, ("(DevContext=%p,InData=%p,Status=%x,XData=%x,YData=%x)",
                  DevContext, InData, InData->InputReport.bStatus,
                InData->InputReport.wXData, InData->InputReport.wYData));

    bCurStatus = InData->InputReport.bStatus;
    fInRange = (bCurStatus & INSTATUS_IN_RANGE) != 0;
    wPressureThreshold = (bCurStatus & INSTATUS_INVERTED)?
                                DevContext->OemData.PressureThresholds.wEraser:
                                DevContext->OemData.PressureThresholds.wTip;
    pwPressureBaseLine = (bCurStatus & INSTATUS_INVERTED)?
                                &DevContext->OemData.PressureBaseLines.wEraser:
                                &DevContext->OemData.PressureBaseLines.wTip;

    InData->InputReport.wXData =
        (NORMALIZE_DATA(InData->InputReport.wXData) << 2) |
        LOW_X_BITS(InData->InputReport.wPressureData);
    InData->InputReport.wYData =
        (NORMALIZE_DATA(InData->InputReport.wYData) << 2) |
        LOW_Y_BITS(InData->InputReport.wPressureData);
    InData->InputReport.wPressureData = fInRange?
                                        NORMALIZE_PRESSURE(
                                            InData->InputReport.wPressureData):
                                        0;
    InData->InputReport.bXTiltData &= ~INBDATA_SYNC;
    InData->InputReport.bYTiltData &= ~INBDATA_SYNC;

    if (!(DevContext->OemData.bLastStatus & INSTATUS_IN_RANGE) && fInRange)
    {
        //
        // We just came into proximity, let's determine our baseline pressure
        // again. This could drift upward from the last baseline pressure
        // because the "pressure spring" may not recover back to its original
        // shape.
        //
        if (InData->InputReport.wPressureData < DEF_PRESSURE_THRESHOLD)
        {
            *pwPressureBaseLine = InData->InputReport.wPressureData;
        }
    }
    else if (InData->InputReport.wPressureData < *pwPressureBaseLine)
    {
        //
        // This allows the baseline pressure to drift downward as the
        // "pressure spring" recovers.
        //
        *pwPressureBaseLine = InData->InputReport.wPressureData;
    }
    //
    // Zero adjust and re-scale the pressure according to baseline.
    //
    REBASE_PRESSURE(DevContext,
                    InData->InputReport.wPressureData,
                    *pwPressureBaseLine);

    //
    // Ignore the PEN_TIP_DOWN bit from the firmware and generate our
    // own using our pressure threshold.
    //
    InData->InputReport.bStatus &= ~INSTATUS_PEN_TIP_DOWN;
    if (InData->InputReport.wPressureData >= wPressureThreshold)
    {
        InData->InputReport.bStatus |= INSTATUS_PEN_TIP_DOWN;
        REBASE_PRESSURE(DevContext,
                        InData->InputReport.wPressureData,
                        wPressureThreshold);
    }
    else
    {
        InData->InputReport.wPressureData = 0;
    }
#ifdef DEBUG
    if ((bCurStatus^InData->InputReport.bStatus) & INSTATUS_PEN_TIP_DOWN)
    {
        TInfo(("Pressure Thresholds different: Status=%x, Pressure=%d, Baseline=%d, Threshold=%d",
               bCurStatus, InData->InputReport.wPressureData,
               *pwPressureBaseLine, wPressureThreshold));
    }
#endif

    if ((InData->InputReport.bStatus & INSTATUS_INVERTED) &&
        (InData->InputReport.bStatus & INSTATUS_PEN_TIP_DOWN))
    {
        //
        // The eraser is down.
        //
        InData->InputReport.bStatus &= ~INSTATUS_PEN_TIP_DOWN;
        InData->InputReport.bStatus |= INSTATUS_ERASER_DOWN;
    }
    else
    {
        //
        // ERASER_DOWN is a generated bit not from the hardware.  We clear it
        // so that it wouldn't contain any garbage.
        //
        InData->InputReport.bStatus &= ~INSTATUS_ERASER_DOWN;
    }

    if (!fInRange)
    {
        //
        // We are going out of proximity, make sure pen and eraser are not
        // down. There is a problem with the tip pressure support that the
        // tip pressure may still be above the "pen down" threshold when it
        // is out-of-proxmity.  So the next time the pen comes back into
        // proximity, it then realizes there was a pen up and generates a
        // pen up at the re-entry location which creates a hyper jump from
        // the exit proximity point to the re-entry proximity point.  By
        // clearing the bits here, we are preventing this problem by
        // simulating a pen/erase up event on the last point before we exit
        // proximity.
        //
        InData->InputReport.bStatus &= ~(INSTATUS_PEN_TIP_DOWN |
                                         INSTATUS_ERASER_DOWN);
    }

    if (InData->InputReport.wXData > DevContext->OemData.wXMax)
    {
      #ifdef DEBUG
        if (InData->InputReport.wXData > DevContext->OemData.wMaxX)
        {
            DevContext->OemData.wMaxX = InData->InputReport.wXData;
        }
      #endif
        InData->InputReport.wXData = DevContext->OemData.wXMax;
    }

    if (InData->InputReport.wYData > DevContext->OemData.wYMax)
    {
      #ifdef DEBUG
        if (InData->InputReport.wYData > DevContext->OemData.wMaxY)
        {
            DevContext->OemData.wMaxY = InData->InputReport.wYData;
        }
      #endif
        InData->InputReport.wYData = DevContext->OemData.wYMax;
    }
    DevContext->OemData.bLastStatus = bCurStatus;

#ifdef _TIMESTAMP_
    DevContext->OemData.LastCounter = KeQueryPerformanceCounter(NULL);
#endif
#ifdef DEBUG
    CurrentTime = KeQueryInterruptTime();
    if ((DevContext->OemData.LastReport.InputReport.bStatus ^
         InData->InputReport.bStatus) & INSTATUS_PEN_TIP_DOWN)
    {
        //
        // The tip switch changes state
        //
        if (InData->InputReport.bStatus & INSTATUS_PEN_TIP_DOWN)
        {
            DevContext->OemData.dwcSamples = 0;
            DevContext->OemData.dwcLostBytes = 0;
            DevContext->OemData.StartTime = CurrentTime;
        }
        else
        {
            CurrentTime -= DevContext->OemData.StartTime;
            CurrentTime /= 10000;
            TInfo(("Samples=%d,Elapsed=%d,Rate=%I64d,BytesLost=%d",
                   DevContext->OemData.dwcSamples,
                   (ULONG)CurrentTime,
                   CurrentTime? DevContext->OemData.dwcSamples*1000/CurrentTime: 0,
                   DevContext->OemData.dwcLostBytes));
        }
    }

    DevContext->OemData.dwcSamples++;

    DevContext->OemData.LastReportTime = CurrentTime;
    RtlCopyMemory(&DevContext->OemData.LastReport,
                  InData,
                  sizeof(DevContext->OemData.LastReport));
#endif
    PoSetSystemState(ES_USER_PRESENT);

    TInfo(("Status=%x,x=%x(%d),y=%x(%d),Pressure=%x(%d),XTilt=%x(%d),YTilt=%x(%d)",
           InData->InputReport.bStatus, InData->InputReport.wXData,
           InData->InputReport.wXData, InData->InputReport.wYData,
           InData->InputReport.wYData, InData->InputReport.wPressureData,
           InData->InputReport.wPressureData, InData->InputReport.bXTiltData,
           InData->InputReport.bXTiltData, InData->InputReport.bYTiltData,
           InData->InputReport.bYTiltData));

    TExit(Func, ("=%x (Status=%x,XData=%x(%d),YData=%x(%d),Pressure=%x(%d),XTilt=%x(%d),YTilt=%x(%d)",
                 status, InData->InputReport.bStatus,
                 InData->InputReport.wXData,
                 InData->InputReport.wXData,
                 InData->InputReport.wYData,
                 InData->InputReport.wYData,
                 InData->InputReport.wPressureData,
                 InData->InputReport.wPressureData,
                 InData->InputReport.bXTiltData,
                 InData->InputReport.bXTiltData,
                 InData->InputReport.bYTiltData,
                 InData->InputReport.bYTiltData));
    return status;
}       //OemNormalizeInputData


NTSTATUS SetDeviceReportDescriptor(
     __in PDEVICE_EXTENSION DevContext
     )
 /*++

 Routine Description:
   Sets logical values in report descriptor
 
 Arguments:
   DevContext - Device context

Return Value:
   On Success - STATUS_SUCCESS
   On Failure - STATUS_INVALID_PARAMETER

 --*/
{
    NTSTATUS status = STATUS_INVALID_PARAMETER;
    ULONG phyX = 0;
    ULONG phyY = 0;

    if(NULL != DevContext)
    {
        //set logical maximums
        *((PUSHORT)&gReportDescriptor[BYTEINDEX_XMAX]) =
                                DevContext->OemData.wXMax;
        *((PUSHORT)&gReportDescriptor[BYTEINDEX_YMAX]) =
            DevContext->OemData.wYMax;
        
        //set physical maximums
        phyX = DevContext->OemData.wXMax	* 1000 / 2540;
        phyY = DevContext->OemData.wYMax	* 1000 / 2540;
        
        //make sure we didn't overflow.
        if(phyX <= MAX_USHORT &&
            phyY <= MAX_USHORT)
        {
            
            *((PUSHORT)&gReportDescriptor[BYTEINDEX_PHYSICAL_XMAX]) =
                                (USHORT)phyX;
            *((PUSHORT)&gReportDescriptor[BYTEINDEX_PHYSICAL_YMAX]) =
                                (USHORT)phyY;
        }
        
        if (DevContext->OemData.wPressureMax != 0)
        {
            *((PUSHORT)&gReportDescriptor[BYTEINDEX_PRESSUREMAX]) =
                DevContext->OemData.wPressureMax;
        }
        if (DevContext->OemData.bXTiltMax != 0)
        {
            gReportDescriptor[BYTEINDEX_XTILTMIN] =
                -DevContext->OemData.bXTiltMax;
            gReportDescriptor[BYTEINDEX_XTILTMAX] =
                DevContext->OemData.bXTiltMax;
        }
        if (DevContext->OemData.bYTiltMax != 0)
        {
            gReportDescriptor[BYTEINDEX_YTILTMIN] =
                -DevContext->OemData.bYTiltMax;
            gReportDescriptor[BYTEINDEX_YTILTMAX] =
                DevContext->OemData.bYTiltMax;
        }
        //
        // Initialize the default pressure thresholds and
        // max baseline pressures.
        //
        DevContext->OemData.PressureBaseLines.wTip =
        DevContext->OemData.PressureBaseLines.wEraser =
            DEF_PRESSURE_THRESHOLD;
        DevContext->OemData.PressureThresholds.wTip =
        DevContext->OemData.PressureThresholds.wEraser =
            MIN_PRESSURE_THRESHOLD;
        status = STATUS_SUCCESS;
    }
    return status;
}//SetDeviceReportDescriptor

