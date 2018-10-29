#include "hw_pcomp.h"
#include <stdio.h>

#pragma warning(disable:4200)  // nameless struct/union
#pragma warning(disable:4201)  // nameless struct/union
#pragma warning(disable:4214)  // bit field types other than int

#include "hw_main.h"
#include "hw_nic.h"

#pragma warning(default:4200)
#pragma warning(default:4201)
#pragma warning(default:4214)
#pragma warning(disable:4748)

#include "usb_main.h"
#include "usb_reg.h"

#pragma NDIS_PAGEABLE_FUNCTION(HwReadMacSieRegister)
#pragma NDIS_PAGEABLE_FUNCTION(HwWriteMacSieRegister)


UCHAR
HwPlatformIORead1Byte(
    PNIC Nic,
    ULONG      offset
    )
{
    UCHAR              data = 0xff;
    NTSTATUS           status;
    
    if (KeGetCurrentIrql() > PASSIVE_LEVEL) {
        //
        // <NOTE> Asyn IO Read is not allowed, one should use Sync IO instead. 
        //
        return 0xff;
    }
    
    status =  HwReadMacSieRegister(
                                 Nic,
                                 0x05,  //STATNDARD_REQUEST
                                 (USHORT)offset,
                                 sizeof(UCHAR),
                                 &data
                                 );
    
    if (!NT_SUCCESS(status)) {
         data = 0xff;
    }
    
    return data;    
}

USHORT
HwPlatformIORead2Byte(
    PNIC Nic,
    ULONG      offset
    )
{
    USHORT              data = 0xffff;
    NTSTATUS            status;

    if (KeGetCurrentIrql() > PASSIVE_LEVEL) {
        //
        // <NOTE> Asyn IO Read is not allowed, one should use Sync IO instead. 
        //
        return 0xffff;
    }

    status =  HwReadMacSieRegister(
                                 Nic,
                                 0x05,  //STATNDARD_REQUEST
                                 (USHORT)offset,
                                 sizeof(USHORT),
                                 &data
                                 );
    
    if (!NT_SUCCESS(status)) {
         data = 0xffff;
    }

    return data;
}

ULONG
HwPlatformIORead4Byte(
    PNIC Nic,
    ULONG      offset
    )
{
    ULONG              data = 0xffffffff;
    NTSTATUS            status;

    if (KeGetCurrentIrql() > PASSIVE_LEVEL) {
        //
        // <NOTE> Asyn IO Read is not allowed, one should use Sync IO instead. 
        //
        return 0xffffffff;
    }

    status =  HwReadMacSieRegister(
                                 Nic,
                                 0x05,  //STATNDARD_REQUEST
                                 (USHORT)offset,
                                 sizeof(ULONG),
                                 &data
                                 );
    
    if (!NT_SUCCESS(status)) {
         data = 0xffffffff;
    }

    return data;   
}



NTSTATUS 
HwReadMacSieRegister(
    __in PNIC Nic,
    __in BYTE Request, 
    __in USHORT Value,
    __in ULONG Length,
    __out PVOID Buffer
    )
/*++

Routine Description

    This routine reads a register 

Arguments:

    Nic - One of our device extensions
    
Return Value:

    NT status value

--*/
{
    NTSTATUS status;
    WDF_USB_CONTROL_SETUP_PACKET    controlSetupPacket;
    WDF_MEMORY_DESCRIPTOR memDesc;
    ULONG    bytesTransferred;
    
    PAGED_CODE();
    //MpTrace (COMP_OID, DBG_TRACE, ( "--> SendVendorSpecificCommand\n"));
    WDF_USB_CONTROL_SETUP_PACKET_INIT_VENDOR(&controlSetupPacket,
                                           BmRequestDeviceToHost,
                                           BmRequestToDevice,
                                           Request, // Request
                                           Value, // Value
                                           0); // Index                                                        
    WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&memDesc, 
                                      Buffer, 
                                      Length);
    status = WdfUsbTargetDeviceSendControlTransferSynchronously(
                                        Nic->UsbDevice, 
                                        WDF_NO_HANDLE, // Optional WDFREQUEST
                                        NULL, // PWDF_REQUEST_SEND_OPTIONS
                                        &controlSetupPacket,
                                        &memDesc,
                                        &bytesTransferred);

    if(!NT_SUCCESS(status)) {

        MpTrace (COMP_OID, DBG_SERIOUS, (
                "WdfUsbTargetDeviceSendControlTransferSynchronously Failed - 0x%x \n", status));

    } 
        
    //MpTrace (COMP_OID, DBG_TRACE, ( "<-- SendVendorSpecificCommand\n"));
    return status;

}

 

NTSTATUS 
HwWriteMacSieRegister(
    __in PNIC Nic,
    __in BYTE Request, 
    __in USHORT Value,
    __in ULONG Length,
    __in PVOID Buffer,
    __in USHORT Index    
    )
/*++

Routine Description

    
Arguments:

    Nic - One of our device extensions
    
Return Value:

    NT status value

--*/
{
    NTSTATUS status;
    WDF_USB_CONTROL_SETUP_PACKET    controlSetupPacket;
    WDF_MEMORY_DESCRIPTOR memDesc;
    ULONG    bytesTransferred;
    
    PAGED_CODE();    

    if (Value == 0xff7c) {
        UCHAR   cmdByte;
        ULONG   dataBytes;

        NdisMoveMemory(&dataBytes, Buffer, 4);

        cmdByte = (UCHAR)(dataBytes & 0x000000ff);
        dataBytes = dataBytes>>8;

        WDF_USB_CONTROL_SETUP_PACKET_INIT_VENDOR(&controlSetupPacket,
                                            BmRequestHostToDevice,
                                            BmRequestToDevice,
                                            Request, // Request
                                            Value+1, // Value  -- write this to ff7d
                                            Index); // Index                                                        
    
        WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&memDesc, 
                                          (PVOID)&dataBytes, 
                                          3);
        
        status = WdfUsbTargetDeviceSendControlTransferSynchronously(
                                            Nic->UsbDevice, 
                                            NULL, // Optional WDFREQUEST
                                            NULL, // PWDF_REQUEST_SEND_OPTIONS
                                            &controlSetupPacket,
                                            &memDesc,
                                            &bytesTransferred);
    
        if(!NT_SUCCESS(status)) {
            
            MpTrace (COMP_OID, DBG_SERIOUS, (
                     "WriteRegister WdfUsbTargetDeviceSendControlTransferSynchronously: Failed - 0x%x \n", status));
            return status;
        } 

        NdisStallExecution(1000);       
        WDF_USB_CONTROL_SETUP_PACKET_INIT_VENDOR(&controlSetupPacket,
                                            BmRequestHostToDevice,
                                            BmRequestToDevice,
                                            Request, // Request
                                            Value, // Value
                                            Index); // Index                                                        
        WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&memDesc, 
                                          (PVOID)&cmdByte, 
                                          1);
        status = WdfUsbTargetDeviceSendControlTransferSynchronously(
                                            Nic->UsbDevice, 
                                            NULL, // Optional WDFREQUEST
                                            NULL, // PWDF_REQUEST_SEND_OPTIONS
                                            &controlSetupPacket,
                                            &memDesc,
                                            &bytesTransferred);
    
        if(!NT_SUCCESS(status)) {           
            MpTrace (COMP_OID, DBG_SERIOUS, (
                     "WriteRegister WdfUsbTargetDeviceSendControlTransferSynchronously: Failed - 0x%x \n", status));   
        } 

    }
    else {

        WDF_USB_CONTROL_SETUP_PACKET_INIT_VENDOR(&controlSetupPacket,
                                            BmRequestHostToDevice,
                                            BmRequestToDevice,
                                            Request, // Request
                                            Value, // Value
                                            Index); // Index                                                        
        WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&memDesc, 
                                          Buffer, 
                                          Length);        
        status = WdfUsbTargetDeviceSendControlTransferSynchronously(
                                            Nic->UsbDevice, 
                                            NULL, // Optional WDFREQUEST
                                            NULL, // PWDF_REQUEST_SEND_OPTIONS
                                            &controlSetupPacket,
                                            &memDesc,
                                            &bytesTransferred);
    
        if(!NT_SUCCESS(status)) {
            
            MpTrace (COMP_OID, DBG_SERIOUS, (
                     "WriteRegister WdfUsbTargetDeviceSendControlTransferSynchronously: Failed - 0x%x \n", status));
    
        } 
    }
    
    return status;
}


VOID
HwPlatformIOWrite1Byte(
    __in PNIC     Nic,
    ULONG       Offset,
    UCHAR       ParamData
    )
{
    UCHAR              data = 0xff;
    USHORT             index;

    index =0;
    data = ParamData;
    
    if (KeGetCurrentIrql() > PASSIVE_LEVEL) {
        //
        //Asyn I/O
        //
        switch (Offset)
        {
            case BQREQ:
            case MSR:
            case RR:
            case 0xFF80:
            case 0xFF84:
            case 0xFF9D:
            case 0xFF9E:
            case 0xFF9F:                    //realtek code review
            case CR9346:
            case EIFS:      // 0xFF35       
            case DIFS:      // 0xFFB5       
            case SLOT_TIME: // 0xFFB6       
            case 0xFFB0:        // SECR byte 0
            case 0xFFB1:        // SECR byte 1
            case AESMSK1:
            case AESMSK2:
            case CW_VALUE:  // 0xFFBD       
            case 0xFF91:
            case 0xFF85:
            case 0xFF90:
            case Config4:
                HwUsbSendVendorControlPacketAsync(Nic, 0x05, (USHORT)Offset, sizeof(UCHAR), &data, index);
                break;

            default:
                MpTrace(COMP_EVENTS, DBG_NORMAL, ("Async. Control Write Not supported !! Size 1 byte: %x data %x\n", Offset, ParamData));
                break;          
        }
    }
    else {
        HwWriteMacSieRegister(Nic, 0x05, (USHORT)Offset, sizeof(UCHAR), &data, index);    
    }

}


VOID
HwPlatformIOWrite2Byte(
    __in PNIC     Nic,
    ULONG       Offset,
    USHORT      ParamData
    )
{

    ULONG              data = 0xffff;
    USHORT             index;

    index =0;
    data = ParamData;
    if (KeGetCurrentIrql() > PASSIVE_LEVEL) {
        //
        //Asyn I/O
        //
        switch (Offset)
        {
            case BRSR:          // 0xFF2C
            case 0xFF2E:        // 0xFF2E
            case BcnIntv:       // 0xFF70
            case AtimWnd:       // 0xFF72
            case BcnIntTime:    // 0xFF74
            case 0xFF76:        // 0xFF76
                HwUsbSendVendorControlPacketAsync(Nic, 0x05, (USHORT)Offset, sizeof(USHORT), &data, index);
                break;

            default:
                MpTrace(COMP_EVENTS, DBG_NORMAL, ("Async. Control Write Not supported !! Size 2 bytes : %x data %x \n", Offset, ParamData));
                break;
        }
    }
    else {
        HwWriteMacSieRegister(Nic, 0x05, (USHORT)Offset, sizeof(USHORT), &data, index);

    }
    
}

VOID
HwPlatformIOWrite4Byte(
    __in PNIC     Nic,
    ULONG       Offset,
    ULONG       ParamData
    )
{

    ULONG              data = 0xffffffff;
    USHORT             index;

    index =0;
    data = ParamData;                       
        
    if (KeGetCurrentIrql() > PASSIVE_LEVEL) {
        //
        // Async. I/O
        //
        switch (Offset)
        {
            case 0xFF30:
            case TCR:
            case RCR:
            case 0xFFA0:
            case 0xFFA4:
               HwUsbSendVendorControlPacketAsync(Nic, 0x05, (USHORT)Offset, sizeof(ULONG), &data, index);
               break;

            default:
                MpTrace(COMP_EVENTS, DBG_NORMAL, ("Async. Control Write Not supported. Size 4 bytes: %x data %x\n", Offset, ParamData));
                break;
        }
    }
    else {
         HwWriteMacSieRegister(Nic, 0x05, (USHORT)Offset, sizeof(ULONG), &data, index) ;
    }
}





