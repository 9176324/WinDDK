//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Copyright 2005 OSR, Open Systems Resources, Inc. All rights Reserved.
// 
// Module Name:
// 
//     wake.c
// 
// Abstract:
//
//     Contains routines and data for wake-up capabilities
// 
// Author:
//
// Revision History:
//      
#include "smscir.h"

#ifdef WPP_TRACING
#include "wake.tmh"
#endif

VOID
DisplayArmingSettings(
    PSMSCIR_DATA DeviceData
    );


SMSCIR_ARM_FOR_WAKE_DATA RC6WakeData[] = {

    SMSCIR_RC6_CARRIER_DIVIDER,         // CarrierDivider
    SMSCIR_RC6_BIT_RATE_DIVIDER,        // BitRateDivider

    SMSCIR_RC6_CUSTOM_CODE,             // CustomCode
    SMSCIR_RC6_CUSTOM_CODE_PRIME,       // CustomCodePrime

    SMSCIR_RC6_DATA_CODE,               // DataCode

    SMSCIR_RC6_PROTOCOL_SELECT,         // ProtocolSelect

    SMSCIR_RC6_HEADER_VALUE_AND_MASK,   // HeaderValueAndMask
    SMSCIR_RC6_DATA_VALUE_1,            // DataValue1
    SMSCIR_RC6_DATA_MASK_1,             // DataMask1
    SMSCIR_RC6_DATA_VALUE_2,            // DataValue2
    SMSCIR_RC6_DATA_MASK_2,             // DataMask2
    SMSCIR_RC6_DATA_VALUE_3,            // DataValue3
    SMSCIR_RC6_DATA_MASK_3,             // DataMask3
    SMSCIR_RC6_DATA_VALUE_4,            // DataValue4
    SMSCIR_RC6_DATA_MASK_4,             // DataMask4
    SMSCIR_RC6_DATA_VALUE_5,            // DataValue5
    SMSCIR_RC6_DATA_MASK_5,             // DataMask5
};


#if 0
SMSCIR_ARM_FOR_WAKE_DATA RC5WakeData[] = {

    SMSCIR_RC5_CARRIER_DIVIDER,         // CarrierDivider
    SMSCIR_RC5_BIT_RATE_DIVIDER,        // BitRateDivider

    SMSCIR_RC5_CUSTOM_CODE,             // CustomCode
    SMSCIR_RC5_CUSTOM_CODE_PRIME,       // CustomCodePrime

    SMSCIR_RC5_DATA_CODE,               // DataCode

    SMSCIR_RC5_PROTOCOL_SELECT,         // ProtocolSelect

    SMSCIR_RC5_HEADER_VALUE_AND_MASK,   // HeaderValueAndMask
    SMSCIR_RC5_DATA_VALUE_1,            // DataValue1
    SMSCIR_RC5_DATA_MASK_1,             // DataMask1
    SMSCIR_RC5_DATA_VALUE_2,            // DataValue2
    SMSCIR_RC5_DATA_MASK_2,             // DataMask2
    SMSCIR_RC5_DATA_VALUE_3,            // DataValue3
    SMSCIR_RC5_DATA_MASK_3,             // DataMask3
    SMSCIR_RC5_DATA_VALUE_4,            // DataValue4
    SMSCIR_RC5_DATA_MASK_4,             // DataMask4
    SMSCIR_RC5_DATA_VALUE_5,            // DataValue5
    SMSCIR_RC5_DATA_MASK_5,             // DataMask5
};


SMSCIR_ARM_FOR_WAKE_DATA NECWakeData[] = {

    SMSCIR_NEC_CARRIER_DIVIDER,         // CarrierDivider
    SMSCIR_NEC_BIT_RATE_DIVIDER,        // BitRateDivider

    SMSCIR_NEC_CUSTOM_CODE,             // CustomCode
    SMSCIR_NEC_CUSTOM_CODE_PRIME,       // CustomCodePrime

    SMSCIR_NEC_DATA_CODE,               // DataCode

    SMSCIR_NEC_PROTOCOL_SELECT,         // ProtocolSelect

    SMSCIR_NEC_HEADER_VALUE_AND_MASK,   // HeaderValueAndMask
    SMSCIR_NEC_DATA_VALUE_1,            // DataValue1
    SMSCIR_NEC_DATA_MASK_1,             // DataMask1
    SMSCIR_NEC_DATA_VALUE_2,            // DataValue2
    SMSCIR_NEC_DATA_MASK_2,             // DataMask2
    SMSCIR_NEC_DATA_VALUE_3,            // DataValue3
    SMSCIR_NEC_DATA_MASK_3,             // DataMask3
    SMSCIR_NEC_DATA_VALUE_4,            // DataValue4
    SMSCIR_NEC_DATA_MASK_4,             // DataMask4
    SMSCIR_NEC_DATA_VALUE_5,            // DataValue5
    SMSCIR_NEC_DATA_MASK_5,             // DataMask5
};
#endif 

VOID
SmscIrSetWakeKeyInfo(
    IN PSMSCIR_DATA DeviceData
    )
/*++

Routine Description:

    SmscIrSetWakeKeyInfo sets up the ArmForWakeData portion 
    of the SMSCIR_DATA structure. This information will 
    then be used to arm the device for wake if necessary (and thus
    chooses what button on what remote can wake the system

Arguments:

    DeviceData - Our per device context

Return Value:

    None

--*/
{
    UNICODE_STRING powerKeySubKey;
    UNICODE_STRING powerKeyValue;
    NTSTATUS       status;
    WDFKEY         serviceKey;
    WDFKEY         powerSubKey;
    ULONG          valueLength;
    ULONG          valueType;

    SmscIrTracePrint(
        TRACE_LEVEL_VERBOSE,
        SMSCDBG_WAKE_INFO,
        ("SmscIrSetWakeKeyInfo: Entered."));

    //
    // Try to read the PowerKey value out of 
    //  HKLM\CCS\Services\smscir\PowerKey\PowerKey.
    //  
    //  If it's there, then it contains a 
    //  SMSCIR_ARM_FOR_WAKE_DATA structure with our
    //  information for wake. If not, we'll always
    //  just arm for RC6 wake.
    //
    RtlInitUnicodeString(&powerKeySubKey,
                         SMSCIR_POWER_KEY_SUBKEY);

    RtlInitUnicodeString(&powerKeyValue,
                         SMSCIR_POWER_KEY_VALUE);


    status = WdfRegistryOpenKey(NULL,
                                &SmscIrRegistryPath,
                                KEY_READ,
                                WDF_NO_OBJECT_ATTRIBUTES,
                                &serviceKey);

    if (NT_SUCCESS(status)) {


        SmscIrTracePrint(
            TRACE_LEVEL_VERBOSE,
            SMSCDBG_WAKE_INFO,
             ("SmscIrSetWakeKeyInfo: Opened services key"));

        status = WdfRegistryOpenKey(serviceKey,
                                    &powerKeySubKey,
                                    KEY_READ,
                                    WDF_NO_OBJECT_ATTRIBUTES,
                                    &powerSubKey);

        if (NT_SUCCESS(status)) {

            SmscIrTracePrint(
                TRACE_LEVEL_VERBOSE,
                SMSCDBG_WAKE_INFO,
                 ("SmscIrSetWakeKeyInfo: Opened power subkey"));


            status = WdfRegistryQueryValue(powerSubKey,
                                           &powerKeyValue,
                                           sizeof(SMSCIR_ARM_FOR_WAKE_DATA),
                                           &DeviceData->ArmForWakeData,
                                           &valueLength,
                                           &valueType);

            if (NT_SUCCESS(status)) {

                ASSERT(valueLength == sizeof(SMSCIR_ARM_FOR_WAKE_DATA));
                ASSERT(valueType == REG_BINARY);
                
                SmscIrTracePrint(
                    TRACE_LEVEL_VERBOSE,
                    SMSCDBG_WAKE_INFO,
                     ("SmscIrSetWakeKeyInfo: Succesfully read power key"));

                DisplayArmingSettings(DeviceData);

            } else {

                SmscIrTracePrint(
                    TRACE_LEVEL_ERROR,
                    SMSCDBG_WAKE_INFO,
                    ("SmscIrSetWakeKeyInfo: Reading power key failed - 0x%x",
                        status));

            }

        } else {

            SmscIrTracePrint(
                TRACE_LEVEL_ERROR,
                SMSCDBG_WAKE_INFO,
                ("SmscIrSetWakeKeyInfo: Couldn't open power subkey - 0x%x",
                        status));

        }

    } else {
       
        SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_WAKE_INFO,
            ("SmscIrSetWakeKeyInfo: Couldn't open services key - 0x%x",
                status));

    }

    if (!NT_SUCCESS(status)) {

        SmscIrTracePrint(
            TRACE_LEVEL_ERROR,
            SMSCDBG_WAKE_INFO,
              ("SmscIrSetWakeKeyInfo: Defaulting to wake by RC6"));

        //
        // Default to the RC6 key
        //
        RtlCopyMemory(&DeviceData->ArmForWakeData,
                      RC6WakeData,
                      sizeof(SMSCIR_ARM_FOR_WAKE_DATA));

        DisplayArmingSettings(DeviceData);

    }

    return;

}


NTSTATUS
SmscIrEvtDeviceWakeArm(
    IN WDFDEVICE Device
    )
/*++

Routine Description:

    EvtDeviceWakeArm is called when the Framework arms the device for
    wake. 

    Our device supports waking via an RC6, RC5, NEC or ProtoX remote,
    which one we wake from here will be dictated by policy setup in the
    registry

Arguments:

    Device - Handle to a Framework device object.

Return Value:

    NTSTATUS - Failure will result in the device remaining in the D0 state.

--*/
{

    PSMSCIR_DATA         deviceData;
    MASTER_BLOCK_CONTROL mbc;
    LINE_CONTROL_B       lcb;
    CONSUMER_IR_CTRL     irCtrl;

    deviceData = (PSMSCIR_DATA)GetIrData(Device);

    SmscIrTracePrint(
        TRACE_LEVEL_VERBOSE,
        SMSCDBG_WAKE_INFO,
        ("SmscIrEvtDeviceWakeArm: Entered."));

    WdfInterruptAcquireLock(deviceData->Interrupt);

    //
    // Master reset the device
    //
    mbc.AsUChar     = 0;
    mbc.ErrorReset  = TRUE;
    mbc.MasterReset = TRUE;
    WRITE_HARDWARE_UCHAR(deviceData,
                         CIRCC2_MASTER_BLOCK_CONTROL,
                         mbc.AsUChar);

    //
    // Clear the reset
    //
    mbc.AsUChar = 0;
    WRITE_HARDWARE_UCHAR(deviceData,
                         CIRCC2_MASTER_BLOCK_CONTROL,
                         mbc.AsUChar);


    //
    // Setup the standard stuff.
    //
    mbc.AsUChar = 0;
    mbc.RegisterBlockSelect = 2;
    WRITE_HARDWARE_UCHAR(deviceData,
                         CIRCC2_MASTER_BLOCK_CONTROL,
                         mbc.AsUChar);

    irCtrl.AsUChar     = 0;
    irCtrl.CarrierOff  = TRUE;
    irCtrl.ReceiveSync = TRUE;
    irCtrl.PMEWake     = TRUE;
    irCtrl.Frame       = TRUE;
    WRITE_HARDWARE_UCHAR(deviceData,
                         CIRCC2_CONSUMER_IR_CTRL,
                         irCtrl.AsUChar);

    //
    // Display the settings that we're going to arm with
    //
    DisplayArmingSettings(deviceData);


    //
    // And now setup the protocol specific registers
    //
    WRITE_HARDWARE_UCHAR(deviceData,
                         CIRCC2_CONSUMER_IR_CARRIER,
                         deviceData->ArmForWakeData.CarrierDivider);

    WRITE_HARDWARE_UCHAR(deviceData,
                         CIRCC2_CONSUMER_IR_BIT_RATE,
                         deviceData->ArmForWakeData.BitRateDivider);

    WRITE_HARDWARE_UCHAR(deviceData,
                         CIRCC2_CUSTOM_CODE,
                         deviceData->ArmForWakeData.CustomCode);

    WRITE_HARDWARE_UCHAR(deviceData,
                         CIRCC2_CUSTOM_CODE_PRIME,
                         deviceData->ArmForWakeData.CustomCodePrime);

    WRITE_HARDWARE_UCHAR(deviceData,
                         CIRCC2_DATA_CODE,
                         deviceData->ArmForWakeData.DataCode);

    WRITE_HARDWARE_UCHAR(deviceData,
                         CIRCC2_CONSUMER_PROTOCOL_SELECT,
                         deviceData->ArmForWakeData.ProtocolSelect);

    mbc.AsUChar = 0;
    mbc.RegisterBlockSelect = 6;
    WRITE_HARDWARE_UCHAR(deviceData,
                         CIRCC2_MASTER_BLOCK_CONTROL,
                         mbc.AsUChar);

    WRITE_HARDWARE_UCHAR(deviceData,
                         CIRCC2_HEADER_VALUE_AND_MASK,
                         deviceData->ArmForWakeData.HeaderValueAndMask); 
    
    WRITE_HARDWARE_UCHAR(deviceData,
                         CIRCC2_DATA_VALUE_1,
                         deviceData->ArmForWakeData.DataValue1);
    WRITE_HARDWARE_UCHAR(deviceData,
                         CIRCC2_DATA_MASK_1,
                         deviceData->ArmForWakeData.DataMask1);


    WRITE_HARDWARE_UCHAR(deviceData,
                         CIRCC2_DATA_VALUE_2,
                         deviceData->ArmForWakeData.DataValue2);
    WRITE_HARDWARE_UCHAR(deviceData,
                         CIRCC2_DATA_MASK_2,
                         deviceData->ArmForWakeData.DataMask2);


    WRITE_HARDWARE_UCHAR(deviceData,
                         CIRCC2_DATA_VALUE_3,
                         deviceData->ArmForWakeData.DataValue3);
    WRITE_HARDWARE_UCHAR(deviceData,
                         CIRCC2_DATA_MASK_3,
                         deviceData->ArmForWakeData.DataMask3);

    mbc.AsUChar = 0;
    mbc.RegisterBlockSelect = 7;
    WRITE_HARDWARE_UCHAR(deviceData,
                         CIRCC2_MASTER_BLOCK_CONTROL,
                         mbc.AsUChar);

    WRITE_HARDWARE_UCHAR(deviceData,
                         CIRCC2_DATA_VALUE_4,
                         deviceData->ArmForWakeData.DataValue4);
    WRITE_HARDWARE_UCHAR(deviceData,
                         CIRCC2_DATA_MASK_4,
                         deviceData->ArmForWakeData.DataMask4);

    WRITE_HARDWARE_UCHAR(deviceData,
                         CIRCC2_DATA_VALUE_5,
                         deviceData->ArmForWakeData.DataValue5);
    WRITE_HARDWARE_UCHAR(deviceData,
                         CIRCC2_DATA_MASK_5,
                         deviceData->ArmForWakeData.DataMask5);

    mbc.AsUChar = 0;
    mbc.RegisterBlockSelect = 0;
    WRITE_HARDWARE_UCHAR(deviceData,
                         CIRCC2_MASTER_BLOCK_CONTROL,
                         mbc.AsUChar);

    //
    // Enable the device for receive and get out!
    //
    lcb.AsUChar = 0;
    lcb.SCEModeBits = SCE_MODE_RECEIVE;
    WRITE_HARDWARE_UCHAR(deviceData,
                         CIRCC2_LINE_CONTROL_B,
                         lcb.AsUChar);

    WdfInterruptReleaseLock(deviceData->Interrupt);



    SmscIrTracePrint(
        TRACE_LEVEL_VERBOSE,
        SMSCDBG_WAKE_INFO,
        ("SmscIrEvtDeviceWakeArm: Exit."));

    return STATUS_SUCCESS;

}

BOOLEAN
SmscIrDeviceWokeSystem(
    IN PSMSCIR_DATA DeviceData
    ) {

/*++

Routine Description:

   This routine should be called every time the device powers up.
   It will return TRUE if the remote control was used to wake
   the system, FALSE otherwise.

Arguments:

    DeviceData - Our per device context


Return Value:

    TRUE  - Device woke the system

    FALSE - Device did NOT wake the system

--*/

    MASTER_BLOCK_CONTROL mbc;
    BOOLEAN              remoteWokeSystem = FALSE;

    //
    // The "was wake" bit is in register block 3. Note
    //  that we're not racing against the ISR or the 
    //  code that enables interrupts on our device. This
    //  code is called before the InterrupEnable code.
    //
    mbc.AsUChar = 0;
    mbc.RegisterBlockSelect = 3;
    WRITE_HARDWARE_UCHAR(DeviceData,
                         CIRCC2_MASTER_BLOCK_CONTROL, 
                         mbc.AsUChar);

    remoteWokeSystem = READ_HARDWARE_UCHAR(DeviceData,
                                           CIRCC2_WAS_REMOTE_WAKE);
    
    //
    // Return to block 0.
    //
    mbc.AsUChar = 0;
    WRITE_HARDWARE_UCHAR(DeviceData,
                         CIRCC2_MASTER_BLOCK_CONTROL, 
                         mbc.AsUChar);
    return remoteWokeSystem;

}


VOID
DisplayArmingSettings(
    PSMSCIR_DATA DeviceData
    ) {
    
    SmscIrTracePrint(
        TRACE_LEVEL_VERBOSE,
        SMSCDBG_WAKE_INFO,
        ("DisplayArmingSettings: Will arm the device with the following:"));

    SmscIrTracePrint(
        TRACE_LEVEL_VERBOSE,
        SMSCDBG_WAKE_INFO,
        ("\tCarrierDivider == 0x%x", 
            DeviceData->ArmForWakeData.CarrierDivider));
    SmscIrTracePrint(
        TRACE_LEVEL_VERBOSE,
        SMSCDBG_WAKE_INFO,
        ("\tBitRateDivider == 0x%x", 
            DeviceData->ArmForWakeData.BitRateDivider));
    SmscIrTracePrint(
        TRACE_LEVEL_VERBOSE,
        SMSCDBG_WAKE_INFO,
        ("\tCustomCode == 0x%x", 
            DeviceData->ArmForWakeData.CustomCode));
    SmscIrTracePrint(
        TRACE_LEVEL_VERBOSE,
        SMSCDBG_WAKE_INFO,
        ("\tCustomCodePrime == 0x%x", 
            DeviceData->ArmForWakeData.CustomCodePrime));
    SmscIrTracePrint(
        TRACE_LEVEL_VERBOSE,
        SMSCDBG_WAKE_INFO,
        ("\tDataCode == 0x%x", 
            DeviceData->ArmForWakeData.DataCode));
    SmscIrTracePrint(
        TRACE_LEVEL_VERBOSE,
        SMSCDBG_WAKE_INFO,
        ("\tProtocolSelect  == 0x%x", 
            DeviceData->ArmForWakeData.ProtocolSelect));
    SmscIrTracePrint(
        TRACE_LEVEL_VERBOSE,
        SMSCDBG_WAKE_INFO,
        ("\tHeaderValueAndMask == 0x%x", 
            DeviceData->ArmForWakeData.HeaderValueAndMask));
    SmscIrTracePrint(
        TRACE_LEVEL_VERBOSE,
        SMSCDBG_WAKE_INFO,
        ("\tDataValue1 == 0x%x", 
            DeviceData->ArmForWakeData.DataValue1));
    SmscIrTracePrint(
        TRACE_LEVEL_VERBOSE,
        SMSCDBG_WAKE_INFO,
        ("\tDataMask1 == 0x%x", 
            DeviceData->ArmForWakeData.DataMask1));
    SmscIrTracePrint(
        TRACE_LEVEL_VERBOSE,
        SMSCDBG_WAKE_INFO,
        ("\tDataValue2 == 0x%x", 
            DeviceData->ArmForWakeData.DataValue2));
    SmscIrTracePrint(
        TRACE_LEVEL_VERBOSE,
        SMSCDBG_WAKE_INFO,
        ("\tDataMask2 == 0x%x", 
            DeviceData->ArmForWakeData.DataMask2));
    SmscIrTracePrint(
        TRACE_LEVEL_VERBOSE,
        SMSCDBG_WAKE_INFO,
        ("\tDataValue3 == 0x%x", 
            DeviceData->ArmForWakeData.DataValue3));
    SmscIrTracePrint(
        TRACE_LEVEL_VERBOSE,
        SMSCDBG_WAKE_INFO,
        ("\tDataMask3 == 0x%x", 
            DeviceData->ArmForWakeData.DataMask3));
    SmscIrTracePrint(
        TRACE_LEVEL_VERBOSE,
        SMSCDBG_WAKE_INFO,
        ("\tDataValue4 == 0x%x", 
            DeviceData->ArmForWakeData.DataValue4));
    SmscIrTracePrint(
        TRACE_LEVEL_VERBOSE,
        SMSCDBG_WAKE_INFO,
        ("\tDataMask4 == 0x%x", 
            DeviceData->ArmForWakeData.DataMask4));
    SmscIrTracePrint(
        TRACE_LEVEL_VERBOSE,
        SMSCDBG_WAKE_INFO,
        ("\tDataValue5 == 0x%x", 
            DeviceData->ArmForWakeData.DataValue5));
    SmscIrTracePrint(
        TRACE_LEVEL_VERBOSE,
        SMSCDBG_WAKE_INFO,
        ("\tDataMask5 == 0x%x", 
            DeviceData->ArmForWakeData.DataMask5));

    return;

}

