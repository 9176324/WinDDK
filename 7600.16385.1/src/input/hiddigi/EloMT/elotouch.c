/*++
    Copyright (c) Microsoft Corporation

    Module Name:
        Elotouch.c

    Abstract:
        Contains Elo specific functions.

    Environment:
        Kernel mode

--*/

#include "pch.h"
#ifdef DEBUG
  #define HID_NAME_DATA
  #define SERIAL_NAME_DATA
#endif

#define MODULE_ID                       0

#define BYTEINDEX_YMAX                  62

UCHAR gReportDescriptor[] = {
   
    0x05, 0x0d,	                        // USAGE_PAGE (Digitizers)          
    0x09, 0x04,	                        // USAGE (Touch Screen)             
    0xa1, 0x01,                         // COLLECTION (Application)         
    0x85, REPORTID_MTOUCH,              //   REPORT_ID (Touch)              
    0x09, 0x22,                         //   USAGE (Finger)                 
    0xa1, 0x02,                         //     COLLECTION (Logical)  
    0x09, 0x42,                         //       USAGE (Tip Switch)           
    0x15, 0x00,                         //       LOGICAL_MINIMUM (0)          
    0x25, 0x01,                         //       LOGICAL_MAXIMUM (1)          
    0x75, 0x01,                         //       REPORT_SIZE (1)              
    0x95, 0x01,                         //       REPORT_COUNT (1)             
    0x81, 0x02,                         //       INPUT (Data,Var,Abs) 
    0x09, 0x32,	                        //       USAGE (In Range)             
    0x81, 0x02,                         //       INPUT (Data,Var,Abs)         
    0x95, 0x06,                         //       REPORT_COUNT (6)  
    0x81, 0x03,                         //       INPUT (Cnst,Ary,Abs)
    0x75, 0x08,                         //       REPORT_SIZE (8)
    0x09, 0x51,                         //       USAGE ( Temp Identifier)
    0x95, 0x01,                         //       REPORT_COUNT (1)             
    0x81, 0x02,                         //       INPUT (Data,Var,Abs) 
    0x05, 0x01,                         //       USAGE_PAGE (Generic Desk..
    0x26, 0xff, 0x0f,                   //     LOGICAL_MAXIMUM (4095)         
    0x75, 0x10,                         //       REPORT_SIZE (16)             
    0x55, 0x0e,                         //       UNIT_EXPONENT (-2)           
    0x65, 0x33,                         //       UNIT (Inch,EngLinear)                  
    0x09, 0x30,                         //       USAGE (X)                    
    0x35, 0x00,                         //       PHYSICAL_MINIMUM (0)         
    0x46, 0xb5, 0x04,                   //       PHYSICAL_MAXIMUM (1205)         
    0x81, 0x02,                         //       INPUT (Data,Var,Abs)         
    0x46, 0x8a, 0x03,                   //       PHYSICAL_MAXIMUM (906)
    0x09, 0x31,                         //       USAGE (Y)                    
    0x81, 0x02,                         //       INPUT (Data,Var,Abs)
    0xc0,                               //    END_COLLECTION
    0xa1, 0x02,                         //    COLLECTION (Logical)  
    0x05, 0x0d,	                        //       USAGE_PAGE (Digitizers)  
    0x09, 0x42,                         //       USAGE (Tip Switch)           
    0x15, 0x00,                         //       LOGICAL_MINIMUM (0)          
    0x25, 0x01,                         //       LOGICAL_MAXIMUM (1)          
    0x75, 0x01,                         //       REPORT_SIZE (1)              
    0x95, 0x01,                         //       REPORT_COUNT (1)             
    0x81, 0x02,                         //       INPUT (Data,Var,Abs) 
    0x09, 0x32,	                        //       USAGE (In Range)             
    0x81, 0x02,                         //       INPUT (Data,Var,Abs)         
    0x95, 0x06,                         //       REPORT_COUNT (6)  
    0x81, 0x03,                         //       INPUT (Cnst,Ary,Abs)
    0x75, 0x08,                         //       REPORT_SIZE (8)
    0x09, 0x51,                         //       USAGE ( Temp Identifier)
    0x95, 0x01,                         //       REPORT_COUNT (1)             
    0x81, 0x02,                         //       INPUT (Data,Var,Abs) 
    0x05, 0x01,                         //       USAGE_PAGE (Generic Desk..
    0x26, 0xff, 0x0f,                   //     LOGICAL_MAXIMUM (4095)       
    0x75, 0x10,                         //       REPORT_SIZE (16)             
    0x55, 0x0e,                         //       UNIT_EXPONENT (-2)           
    0x65, 0x33,                         //       UNIT (Inch,EngLinear)                  
    0x09, 0x30,                         //       USAGE (X)                    
    0x35, 0x00,                         //       PHYSICAL_MINIMUM (0)         
    0x46, 0xb5, 0x04,                   //       PHYSICAL_MAXIMUM (1205)         
    0x81, 0x02,                         //       INPUT (Data,Var,Abs)         
    0x46, 0x8a, 0x03,                   //       PHYSICAL_MAXIMUM (906)
    0x09, 0x31,                         //       USAGE (Y)                    
    0x81, 0x02,                         //       INPUT (Data,Var,Abs)
    0xc0,                               //   END_COLLECTION
    0x05, 0x0d,	                        //   USAGE_PAGE (Digitizers)    
    0x09, 0x54,	                        //   USAGE (Actual count)
    0x95, 0x01,                         //   REPORT_COUNT (1)
    0x75, 0x08,                         //   REPORT_SIZE (8)    
    0x81, 0x02,                         //   INPUT (Data,Var,Abs)
    0x85, REPORTID_MAX_COUNT,           //   REPORT_ID (Feature)              
    0x09, 0x55,                         //   USAGE(Maximum Count)
    0x25, 0x02,                         //   LOGICAL_MAXIMUM (2)
    0xb1, 0x02,                         //   FEATURE (Data,Var,Abs) 
    0xc0,                               // END_COLLECTION
    0x09, 0x0E,                          // USAGE (Configuration)
    0xa1, 0x01,                         // COLLECTION (Application)
    0x85, REPORTID_FEATURE,             //   REPORT_ID (Feature)              6
    0x09, 0x22,                         //   USAGE (Finger)              
    0xa1, 0x00,                         //   COLLECTION (physical)  12   
    0x09, 0x52,                         //    USAGE (Input Mode)         
    0x09, 0x53,                         //    USAGE (Device Index
    0x15, 0x00,                         //    LOGICAL_MINIMUM (0)      
    0x25, 0x0a,                         //    LOGICAL_MAXIMUM (10)
    0x75, 0x08,                         //    REPORT_SIZE (8)         
    0x95, 0x02,                         //    REPORT_COUNT (2)         
    0xb1, 0x02,                         //   FEATURE (Data,Var,Abs    
    0xc0,                               //   END_COLLECTION
    0xc0,                               // END_COLLECTION 
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
    

USHORT g_wYMax = 4095;
USHORT g_wXMax = 4095;
#define MAX_MOUSE_X 32767
#define MAX_MOUSE_Y 32767
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

PWSTR gpwstrManufacturerID = L"Elo";
PWSTR gpwstrProductID = L"IRMTOUCH";
PWSTR gpwstrSerialNumber = L"0";


#ifdef ALLOC_PRAGMA
  #pragma alloc_text(PAGE, NInputDeviceAdd)
  #pragma alloc_text(PAGE, NInputDriverContextCleanup)
  #pragma alloc_text(PAGE, NInputInitDevice)
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
    
    KdPrint(("ELO MT driver entry!\n"));

    WDF_DRIVER_CONFIG_INIT(&config, NInputDeviceAdd);

    //
    // Register a cleanup callback so that we can call WPP_CLEANUP when
    // the framework driver object is deleted during driver unload.
    //
    WDF_OBJECT_ATTRIBUTES_INIT(&attributes);

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
NInputDeviceAdd(
    IN WDFDRIVER       Driver,
    IN PWDFDEVICE_INIT DeviceInit
    )
/*++
Routine Description:

    EloTouchDeviceAdd is called by the framework in response to AddDevice
    call from the PnP manager. We create and initialize a WDF device object to
    represent a new instance of toaster device.

Arguments:

    Driver - Handle to a framework driver object created in DriverEntry

    DeviceInit - Pointer to a framework-allocated WDFDEVICE_INIT structure.

Return Value:

    STATUS_SUCCESS if successful,
    NTSTATUS of failure otherwise.

--*/
{
    NTSTATUS                      status = STATUS_SUCCESS;
    WDF_IO_QUEUE_CONFIG           queueConfig = {0};
    WDF_OBJECT_ATTRIBUTES         attributes = {0};
    WDFDEVICE                     hDevice = {0};
    PDEVICE_EXTENSION             devContext = NULL;
    WDFQUEUE                      queue = {0};
    WDF_PNPPOWER_EVENT_CALLBACKS  pnpPowerCallbacks = {0};
    
    UNREFERENCED_PARAMETER(Driver);

    PAGED_CODE();

    KdPrint(("ELO PMT add device!\n"));
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
        TErr(("WdfDeviceCreate failed status:0x%x",status));
        return status;
    }

    devContext = GetDeviceContext(hDevice);
    devContext->Device = hDevice;
    devContext->InputMode = MODE_MOUSE;
    //initialize spinlock
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
    
    //get the lower device.
    devContext->IoTarget = WdfDeviceGetIoTarget(hDevice);
#ifdef PRIORITY_UPDATE
    devContext->LastState = TOUCH_NONE;
#endif
        
    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&queueConfig, WdfIoQueueDispatchParallel);

    queueConfig.EvtIoInternalDeviceControl = NInputInternalDeviceControl;
    
    queueConfig.PowerManaged = WdfFalse;

    status = WdfIoQueueCreate(hDevice,
                              &queueConfig,
                              WDF_NO_OBJECT_ATTRIBUTES,
                              &queue
                              );
    if (!NT_SUCCESS (status)) {
        TErr(("WdfIoQueueCreate failed status:0x%x",status));
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
        KdPrint(("failed to create queue\n"));       
    }
    TExit(Func, ("=%x", status));
    return status;
}


NTSTATUS INTERNAL
NInputInitDevice(
     __in PDEVICE_EXTENSION DevContext
    )
    /*++
Routine Description:

    This function writes/read serial data for initialization purposes. 
    (This is just a placeholder because the device this driver is for doesn't require initialization)

Arguments:

    DevContext - Device context

Return Value:

    STATUS_SUCCESS if successful,
    NTSTATUS of failure otherwise.

--*/
{
    NTSTATUS status = STATUS_SUCCESS;
    
    PAGED_CODE();
    UNREFERENCED_PARAMETER(DevContext);
    return status;

} 

NTSTATUS INTERNAL
ProcessOEMBuffer(
    __in  PDEVICE_EXTENSION DevContext,
    __in  POEM_RAW_SERIAL_PACKET_2 SerialData
    )
    /*++

Routine Description:
       checks to see if data in the resync buffer is valid

Arguments:

  DevContext - Device context

  SerialData - raw packet data from device

Return Value:

    STATUS_SUCCESS if successful,
    NTSTATUS of failure otherwise.


  --*/
{
    NTSTATUS status = STATUS_DEVICE_DATA_ERROR;    
    PUCHAR   CurrentSerialContact = (PUCHAR)SerialData;
    PUCHAR   ResyncData = (PUCHAR)&DevContext->OemData.ResyncData;
    ULONG    MovedBytes = 0;
    ULONG    BytesToValidate  = ELO_MT_STATUS_COUNT;
    
    TEnter(Func, ("(DevContext=%p)", DevContext));
    KdPrint(("elo touch ProcessOEMBuffer!\n"));
    if(DevContext->OemData.BytesInBuff >= sizeof(OEM_RAW_SERIAL_PACKET_1) )
    {
        //resync buffer has enough data to meet pingpong IRP so
        //copy resync data into raw serial data
        
        status = ValidateOEMHeader(ResyncData, &BytesToValidate);
        if(	NT_SUCCESS(status))            
        {
            status = ValidateOEMBuffer(&ResyncData[2], ELO_MT_PACKET_SIZE);
            if(NT_SUCCESS(status))
            {
                RtlCopyMemory(CurrentSerialContact, 
                    ResyncData, 
                    sizeof(OEM_RAW_SERIAL_PACKET_1));

                DevContext->OemData.BytesInBuff -= sizeof(OEM_RAW_SERIAL_PACKET_1);
                CurrentSerialContact += sizeof(OEM_RAW_SERIAL_PACKET_1);
                ResyncData += sizeof(OEM_RAW_SERIAL_PACKET_1);
                MovedBytes = sizeof(OEM_RAW_SERIAL_PACKET_1);
                if(DevContext->OemData.BytesInBuff >= ELO_MT_PACKET_SIZE)
                {
                    status = ValidateOEMBuffer(ResyncData, ELO_MT_PACKET_SIZE);
                    if(NT_SUCCESS(status))
                    {
                        //copy the second contact data after it has been validated.
                        RtlCopyMemory(CurrentSerialContact, 
                            ResyncData, 
                            ELO_TWO_TOUCH_COUNT - ELO_SINGLE_TOUCH_COUNT);

                        DevContext->OemData.BytesInBuff -=  ELO_MT_PACKET_SIZE;
                        MovedBytes += ELO_TWO_TOUCH_COUNT - ELO_SINGLE_TOUCH_COUNT;
                    }                    
                }
            }
            if(!NT_SUCCESS(status))
            {
               //validation failed even though status and contact count are valid. shift data down one step
                DevContext->OemData.BytesInBuff--;
                status = STATUS_DEVICE_DATA_ERROR;
                if(MovedBytes)
                {
                    MovedBytes+=1;
                }
            }            
        }
        else
        {
            //at least 1 byte is bad. drop buffer count
            KdPrint(("BytesToValidate:%d\n", BytesToValidate));
            MovedBytes+= ELO_MT_STATUS_COUNT - BytesToValidate;
            KdPrint(("MovedBytes:%d\n", MovedBytes));
            DevContext->OemData.BytesInBuff -= MovedBytes;
            KdPrint(("DevContext->OemData.BytesInBuff:%d\n", DevContext->OemData.BytesInBuff));
        }
    }
    //either we found valid data in resync buffer or we didn't. If we did we have already
    //copied the valid data into the new buffer. If we didn't we need to overwrite the bad data
    //by moving the rest of the data to the front.
    if(DevContext->OemData.BytesInBuff)
    {
        //we still have data left in the resync buffer. We should move it down to the beginning 
        //of the buffer in preparation for the walk through we do below.
        KdPrint(("moved bytes:%d, data left:%d\n", MovedBytes, DevContext->OemData.BytesInBuff));
        if(MovedBytes)
        {
            RtlMoveMemory((PUCHAR)&DevContext->OemData.ResyncData[0], 
                ((PUCHAR)&DevContext->OemData.ResyncData[0])+MovedBytes,
                DevContext->OemData.BytesInBuff);
        }
        ResyncData = (PUCHAR)&DevContext->OemData.ResyncData[0];
        BytesToValidate = 1;
        while(
            !NT_SUCCESS(ValidateOEMHeader(ResyncData, &BytesToValidate))
            && (DevContext->OemData.BytesInBuff > 0)
            )
        {
            KdPrint(("moving data left in resync buffer to beginning\n buffer count:%d\n", DevContext->OemData.BytesInBuff));
            DevContext->OemData.BytesInBuff--;
            KdPrint(("BYTE 0 before move:%d\n", DevContext->OemData.ResyncData[0].RawInput[0]));
            RtlMoveMemory((PUCHAR)&DevContext->OemData.ResyncData[0], 
                    ((PUCHAR)&DevContext->OemData.ResyncData[0])+1,
                    DevContext->OemData.BytesInBuff);
            KdPrint(("BYTE 0 after move:%d\n", DevContext->OemData.ResyncData[0].RawInput[0]));              
        };
    }
    if(2 <= DevContext->OemData.BytesInBuff)
    {
        KdPrint(("Status:%d, Contacts:%d\n", *ResyncData, *(ResyncData+1)));
    }
    KdPrint(("current bytes in re-sync buffer:%d\n", DevContext->OemData.BytesInBuff));        
        
    TExit(Func, ("=%x", status));
    return status;
}


NTSTATUS INTERNAL
TranslateRawDataToHID(
    __in  PDEVICE_EXTENSION DevContext,
    __in  POEM_RAW_SERIAL_PACKET_2 RawSerialPacket,
    __in  WDFREQUEST Request
    )
/*++

  Routine Description:
       Translates serial data packet to hid data packet

  Arguments:

      DevContext - Device Context

      RawSerialData - serial data packet

      Request - request from HIDCLASS

  Return Value:

    STATUS_SUCCESS if successful,
    NTSTATUS of failure otherwise.
--*/
{
       
            
    NTSTATUS            status = STATUS_SUCCESS;
    size_t              bufferLength = 0;
    PHID_INPUT_REPORT   HidReport = NULL;
    POEM_INPUT_REPORT   HidData = NULL;
    POEM_MOUSE_REPORT   HidMouse = NULL;
    size_t              RequestInformation = 0;
    USHORT              XData = 0;
    USHORT              YData = 0;
    
    TEnter(Func, ("(DevContext=%p)", DevContext));
    
    KdPrint(("elo touch TranslateRawDataToHID!\n"));
    status = WdfRequestRetrieveOutputBuffer(Request, 
                                    sizeof(HID_INPUT_REPORT), 
                                    &HidReport, 
                                    &bufferLength);

    if(!NT_SUCCESS(status))
    {
        KdPrint(("unable to retrieve ping pong IRP buffer!\n")); 
        //this should never happen since it means we got a bad buffer from HIDCLASS
    }
    else
    {
        RequestInformation = sizeof(HID_INPUT_REPORT);
        //x and y stay the same regardless of which mode we're in
        XData = RawSerialPacket->RawSerialData.XData;
        YData = g_wYMax - RawSerialPacket->RawSerialData.YData;
        //copy the data into the ping pong buffer
        RtlZeroMemory(HidReport, sizeof(HID_INPUT_REPORT));
        
        if(MODE_MOUSE == DevContext->InputMode)
        {
            HidReport->ReportID = REPORTID_MOUSE;
            HidMouse = &HidReport->MouseReport;
            HidMouse->InputReport.wXData = (XData * MAX_MOUSE_X)/g_wXMax;
            HidMouse->InputReport.wYData = (YData * MAX_MOUSE_Y)/g_wYMax;
            
            if(RawSerialPacket->RawSerialData.Status & ELO_MT_1_DOWN)                
            {
                //if we are not untouch we set the button down bit
                HidMouse->InputReport.bButtons |= FINGER_STATUS;
            }
        }        
        else if (MODE_MULTI_TOUCH == DevContext->InputMode)
        {
            
            HidReport->ReportID = REPORTID_MTOUCH;
            HidData = &HidReport->TouchReport;            
            HidData->InputReport.wXData = XData;
            HidData->InputReport.wYData = YData;
            HidData->InputReport.ContactId = RawSerialPacket->RawSerialData.ContactId;
            HidData->InputReport.ActualCount = 0x01;
            if(RawSerialPacket->RawSerialData.Status & ELO_MT_1_DOWN)
            {
                HidData->InputReport.bStatus |=RANGE_FINGER_STATUS;
            }
            if(0x02 == RawSerialPacket->RawSerialData.Contacts)
            {
                if((ELO_MT_BOTH_DOWN == RawSerialPacket->RawSerialData.Status) ||
                    (ELO_MT_2_DOWN == RawSerialPacket->RawSerialData.Status)
                    )
                {
                    HidData->InputReport.bStatus2 |=RANGE_FINGER_STATUS;
                }
                HidData->InputReport.wXData2 = RawSerialPacket->RawSerialData.XData2;
                HidData->InputReport.wYData2 = g_wYMax - RawSerialPacket->RawSerialData.YData2;
                HidData->InputReport.ContactId2 = RawSerialPacket->RawSerialData.ContactId2;
                HidData->InputReport.ActualCount = 0x02;
            }			
        }
        else
        {
            //this driver only supports mouse and multi-touch mode
            status = STATUS_INVALID_DEVICE_REQUEST;
            RequestInformation = 0;
            KdPrint(("Unsupported mode: %d\n", DevContext->InputMode));
        }
        TInfo(("finger 1 (status=%x,xData=%d,yData=%d, Identifier=%d)\n",
                         HidReport->TouchReport.InputReport.bStatus,
                         HidReport->TouchReport.InputReport.wXData,
                         HidReport->TouchReport.InputReport.wYData, 
                         HidReport->TouchReport.InputReport.ContactId));
        if(RawSerialPacket->RawSerialData.Contacts == 0x02)
        {
             TInfo(("finger 2device (status=%x,xData=%d,yData=%d, Identifier=%d)\n",
                     HidReport->TouchReport.InputReport.bStatus2,
                     HidReport->TouchReport.InputReport.wXData2,
                     HidReport->TouchReport.InputReport.wYData2, 
                     HidReport->TouchReport.InputReport.ContactId2));
        }
        
         
    }
    //complete ping pong IRP.
    
    WdfRequestCompleteWithInformation(Request,
                                        status,
                                        RequestInformation);
    TExit(Func, ("=%x", status));
    return status;
 }

 NTSTATUS
 ValidateOEMBuffer(
 __in_bcount(PacketSize)PUCHAR SerialPacket,
 __in ULONG  PacketSize
 )
 /*++

 Routine Description:
   Validate raw serial data packet
Arguments:
  SerialPacket - Serial data packet

Return Value:

    STATUS_SUCCESS if successful,
    NTSTATUS of failure otherwise.


 --*/
 {
     NTSTATUS status = STATUS_SUCCESS;
     USHORT xData = *((PUSHORT)&SerialPacket[0]);
     USHORT yData = *((PUSHORT)&SerialPacket[2]);
     
     TEnter(Func, ("(SerialPacket=%p)", SerialPacket));

     if(ELO_MT_PACKET_SIZE != PacketSize)
     {
        status = STATUS_INVALID_BUFFER_SIZE;
     }
     else
     {
         //if everything but x or y is invalid notify caller. there's no reason to put this packet in the resync buffer
         if((xData > g_wXMax) ||
                (yData > g_wYMax) || (ELO_MT_VALID_ID_1 != SerialPacket[4] && ELO_MT_VALID_ID_2 != SerialPacket[4])
            )
         {
             KdPrint(("out of range coodinate: %d, %d, Contact id:%d\n", xData, yData, SerialPacket[4]));
             status = STATUS_DEVICE_DATA_ERROR;
         }
     }
     TExit(Func, ("=%x", status));
     return status;
 }


