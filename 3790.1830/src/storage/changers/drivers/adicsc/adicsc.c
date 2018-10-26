/*++

Copyright (C) Microsoft Corporation, 1996 - 1999

Module Name:

    adicsc.c

Abstract:

    This module contains device-specific routines for the ADIC Scalar medium changers:
    ADIC Scalar 218,224,448, 458.
    Also for the Compaq UHDL

Environment:

    kernel mode only

Revision History:


--*/

#include "ntddk.h"
#include "mcd.h"
#include "adicsc.h"

#ifdef  ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)

#pragma alloc_text(PAGE, ChangerExchangeMedium)
#pragma alloc_text(PAGE, ChangerGetElementStatus)
#pragma alloc_text(PAGE, ChangerGetParameters)
#pragma alloc_text(PAGE, ChangerGetProductData)
#pragma alloc_text(PAGE, ChangerGetStatus)
#pragma alloc_text(PAGE, ChangerInitialize)
#pragma alloc_text(PAGE, ChangerInitializeElementStatus)
#pragma alloc_text(PAGE, ChangerMoveMedium)
#pragma alloc_text(PAGE, ChangerPerformDiagnostics)
#pragma alloc_text(PAGE, ChangerQueryVolumeTags)
#pragma alloc_text(PAGE, ChangerReinitializeUnit)
#pragma alloc_text(PAGE, ChangerSetAccess)
#pragma alloc_text(PAGE, ChangerSetPosition)
#pragma alloc_text(PAGE, ElementOutOfRange)
#pragma alloc_text(PAGE, MapExceptionCodes)
#pragma alloc_text(PAGE, AdicBuildAddressMapping)
#endif


ULONG
ChangerAdditionalExtensionSize(
    VOID
    )

/*++

Routine Description:

    This routine returns the additional device extension size
    needed by the ADIC Scalar changers.

Arguments:


Return Value:

    Size, in bytes.

--*/

{

    return sizeof(CHANGER_DATA);
}


NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    )
{
    MCD_INIT_DATA mcdInitData;

    RtlZeroMemory(&mcdInitData, sizeof(MCD_INIT_DATA));

    mcdInitData.InitDataSize = sizeof(MCD_INIT_DATA);

    mcdInitData.ChangerAdditionalExtensionSize = ChangerAdditionalExtensionSize;

    mcdInitData.ChangerError = ChangerError;

    mcdInitData.ChangerInitialize = ChangerInitialize;

    mcdInitData.ChangerPerformDiagnostics = ChangerPerformDiagnostics;

    mcdInitData.ChangerGetParameters = ChangerGetParameters;
    mcdInitData.ChangerGetStatus = ChangerGetStatus;
    mcdInitData.ChangerGetProductData = ChangerGetProductData;
    mcdInitData.ChangerSetAccess = ChangerSetAccess;
    mcdInitData.ChangerGetElementStatus = ChangerGetElementStatus;
    mcdInitData.ChangerInitializeElementStatus = ChangerInitializeElementStatus;
    mcdInitData.ChangerSetPosition = ChangerSetPosition;
    mcdInitData.ChangerExchangeMedium = ChangerExchangeMedium;
    mcdInitData.ChangerMoveMedium = ChangerMoveMedium;
    mcdInitData.ChangerReinitializeUnit = ChangerReinitializeUnit;
    mcdInitData.ChangerQueryVolumeTags = ChangerQueryVolumeTags;

    return ChangerClassInitialize(DriverObject, RegistryPath,
                                  &mcdInitData);
}


NTSTATUS
ChangerInitialize(
    IN PDEVICE_OBJECT DeviceObject
    )
{
    PFUNCTIONAL_DEVICE_EXTENSION fdoExtension = DeviceObject->DeviceExtension;
    PCHANGER_DATA  changerData = (PCHANGER_DATA)(fdoExtension->CommonExtension.DriverData);
    NTSTATUS       status;
    PINQUIRYDATA   dataBuffer;
    PCDB           cdb;
    ULONG          length;
    SCSI_REQUEST_BLOCK srb;

    changerData->Size = sizeof(CHANGER_DATA);

    //
    // Build address mapping.
    //

    status = AdicBuildAddressMapping(DeviceObject);
    if (!NT_SUCCESS(status)) {
        DebugPrint((1,
                    "BuildAddressMapping failed. %x\n", status));

        //
        // Indicate that the device needs to re-init.
        //

        changerData->AddressMapping.Initialized = FALSE;
    }

    //
    // Get inquiry data.
    //

    dataBuffer = ChangerClassAllocatePool(NonPagedPoolCacheAligned, sizeof(INQUIRYDATA));
    if (!dataBuffer) {
        DebugPrint((1,
                    "Adicsc.ChangerInitialize: Error allocating dataBuffer. %x\n", status));
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // Now get the full inquiry information for the device.
    //

    RtlZeroMemory(&srb, SCSI_REQUEST_BLOCK_SIZE);

    //
    // Set timeout value.
    //

    srb.TimeOutValue = 10;

    srb.CdbLength = 6;

    cdb = (PCDB)srb.Cdb;

    //
    // Set CDB operation code.
    //

    cdb->CDB6INQUIRY.OperationCode = SCSIOP_INQUIRY;

    //
    // Set allocation length to inquiry data buffer size.
    //

    cdb->CDB6INQUIRY.AllocationLength = sizeof(INQUIRYDATA);

    status = ChangerClassSendSrbSynchronous(DeviceObject,
                                         &srb,
                                         dataBuffer,
                                         sizeof(INQUIRYDATA),
                                         FALSE);

    if (SRB_STATUS(srb.SrbStatus) == SRB_STATUS_SUCCESS ||
        SRB_STATUS(srb.SrbStatus) == SRB_STATUS_DATA_OVERRUN) {

        //
        // Updated the length actually transfered.
        //

        length = dataBuffer->AdditionalLength + FIELD_OFFSET(INQUIRYDATA, Reserved);

        if (length > srb.DataTransferLength) {
            length = srb.DataTransferLength;
        }


        RtlMoveMemory(&changerData->InquiryData, dataBuffer, length);

        //
        // Determine drive id.
        //

        if (RtlCompareMemory(dataBuffer->ProductId,"Scalar DLT 448", 14) == 14) {
            changerData->DriveID = ADIC_SCALAR_448;
        }

        if ((RtlCompareMemory(dataBuffer->ProductId,"Scalar 100", 10) == 10) ||
            (RtlCompareMemory(dataBuffer->ProductId,"PV-136T", 7) == 7)) {
            changerData->DriveID = ADIC_SCALAR;
        }

        if ((RtlCompareMemory(dataBuffer->ProductId,"FastStor DLT", 12) == 12) ||
            (RtlCompareMemory(dataBuffer->ProductId,"PV-120T DLT", 11) == 11)) {
            changerData->DriveID = ADIC_FASTSTOR;
        }

        if (RtlCompareMemory(dataBuffer->ProductId, "UHDL", 4) == 4)
        {
            changerData->DriveID = UHDL;
        }
    }

    ChangerClassFreePool(dataBuffer);

    //
    // We'll try to get device identifier info. If it fails
    // this flag will be set to FALSE so that subsequently
    // we won't try to get device identifier info
    //
    changerData->ObtainDeviceIdentifier = TRUE;

    return STATUS_SUCCESS;
}


VOID
ChangerError(
    PDEVICE_OBJECT DeviceObject,
    PSCSI_REQUEST_BLOCK Srb,
    NTSTATUS *Status,
    BOOLEAN *Retry
    )

/*++

Routine Description:

    This routine executes any device-specific error handling needed.

Arguments:

    DeviceObject
    Irp

Return Value:

    NTSTATUS

--*/
{

    PFUNCTIONAL_DEVICE_EXTENSION fdoExtension = DeviceObject->DeviceExtension;
    PCHANGER_DATA     changerData = (PCHANGER_DATA)(fdoExtension->CommonExtension.DriverData);
    PSENSE_DATA senseBuffer = Srb->SenseInfoBuffer;

    if (Srb->SrbStatus & SRB_STATUS_AUTOSENSE_VALID) {

        switch (senseBuffer->SenseKey & 0xf) {

        case SCSI_SENSE_NOT_READY:

            if (senseBuffer->AdditionalSenseCode == 0x04) {
                switch (senseBuffer->AdditionalSenseCodeQualifier)
                {
                    //
                    // The Compaq StorageWorks UHDL returns 0x02/04/03 when a
                    // magazine is ejected out and a TUR is sent down.
                    //
                    case 0x03:
                        if (changerData->DriveID == UHDL)
                        {
                            *Retry = FALSE;
                            *Status = STATUS_DEVICE_DOOR_OPEN;
                        }
                        break;

                    case 0x82:
                        *Retry = FALSE;
                        *Status = STATUS_DEVICE_DOOR_OPEN;
                        break;
                }
            }

            break;

        case SCSI_SENSE_UNIT_ATTENTION:
            //
            // The Compaq StorageWorks UHDL returns 0x06/3B/12 when a magazine
            // is ejected out.
            //
            if (senseBuffer->AdditionalSenseCode == 0x3B &&
                senseBuffer->AdditionalSenseCodeQualifier == 0x12 &&
                changerData->DriveID == UHDL)
            {
                *Retry = FALSE;
                *Status = STATUS_DEVICE_DOOR_OPEN;
            }
            break;

        case SCSI_SENSE_ILLEGAL_REQUEST:
            break;

        case SCSI_SENSE_HARDWARE_ERROR:

            //
            // The ADIC returns 4/40/01 for some illegal commands.
            //

            if (senseBuffer->AdditionalSenseCode == 0x40) {
                if (senseBuffer->AdditionalSenseCodeQualifier == 0x01) {
                    *Retry = FALSE;
                    *Status = STATUS_INVALID_DEVICE_REQUEST;
                }
            } else {
                changerData->DeviceStatus = ADICSC_HW_ERROR;
            }
            break;

        default:
            break;
        }
    }


    return;
}

NTSTATUS
ChangerGetParameters(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine determines and returns the "drive parameters" of the
    ADIC Scalar changers.

Arguments:

    DeviceObject
    Irp

Return Value:

    NTSTATUS

--*/

{
    PFUNCTIONAL_DEVICE_EXTENSION          fdoExtension = DeviceObject->DeviceExtension;
    PCHANGER_DATA              changerData = (PCHANGER_DATA)(fdoExtension->CommonExtension.DriverData);
    PCHANGER_ADDRESS_MAPPING   addressMapping = &(changerData->AddressMapping);
    PSCSI_REQUEST_BLOCK        srb;
    PGET_CHANGER_PARAMETERS    changerParameters;
    PMODE_ELEMENT_ADDRESS_PAGE elementAddressPage;
    PMODE_TRANSPORT_GEOMETRY_PAGE transportGeometryPage;
    PMODE_DEVICE_CAPABILITIES_PAGE capabilitiesPage;
    NTSTATUS status;
    ULONG    length;
    PVOID    modeBuffer;
    PCDB     cdb;

    srb = ChangerClassAllocatePool(NonPagedPool, SCSI_REQUEST_BLOCK_SIZE);

    if (srb == NULL) {

        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory(srb, SCSI_REQUEST_BLOCK_SIZE);
    cdb = (PCDB)srb->Cdb;

    //
    // Build a mode sense - Element address assignment page.
    //

    modeBuffer = ChangerClassAllocatePool(NonPagedPoolCacheAligned, sizeof(MODE_PARAMETER_HEADER)
                                + sizeof(MODE_ELEMENT_ADDRESS_PAGE));
    if (!modeBuffer) {
        ChangerClassFreePool(srb);
        return STATUS_INSUFFICIENT_RESOURCES;
    }


    RtlZeroMemory(modeBuffer, sizeof(MODE_PARAMETER_HEADER) + sizeof(MODE_ELEMENT_ADDRESS_PAGE));
    srb->CdbLength = CDB6GENERIC_LENGTH;
    srb->TimeOutValue = 20;
    srb->DataTransferLength = sizeof(MODE_PARAMETER_HEADER) + sizeof(MODE_ELEMENT_ADDRESS_PAGE);
    srb->DataBuffer = modeBuffer;

    cdb->MODE_SENSE.OperationCode = SCSIOP_MODE_SENSE;
    cdb->MODE_SENSE.PageCode = MODE_PAGE_ELEMENT_ADDRESS;
    cdb->MODE_SENSE.Dbd = 1;
    cdb->MODE_SENSE.AllocationLength = (UCHAR)srb->DataTransferLength;

    //
    // Send the request.
    //

    status = ChangerClassSendSrbSynchronous(DeviceObject,
                                         srb,
                                         srb->DataBuffer,
                                         srb->DataTransferLength,
                                         FALSE);

    if (!NT_SUCCESS(status)) {
        ChangerClassFreePool(srb);
        ChangerClassFreePool(modeBuffer);
        return status;
    }

    //
    // Fill in values.
    //

    changerParameters = Irp->AssociatedIrp.SystemBuffer;
    RtlZeroMemory(changerParameters, sizeof(GET_CHANGER_PARAMETERS));

    elementAddressPage = modeBuffer;
    (PCHAR)elementAddressPage += sizeof(MODE_PARAMETER_HEADER);

    changerParameters->Size = sizeof(GET_CHANGER_PARAMETERS);
    changerParameters->NumberTransportElements = elementAddressPage->NumberTransportElements[1];
    changerParameters->NumberTransportElements |= (elementAddressPage->NumberTransportElements[0] << 8);

    changerParameters->NumberStorageElements = elementAddressPage->NumberStorageElements[1];
    changerParameters->NumberStorageElements |= (elementAddressPage->NumberStorageElements[0] << 8);

    changerParameters->NumberIEElements = elementAddressPage->NumberIEPortElements[1];
    changerParameters->NumberIEElements |= (elementAddressPage->NumberIEPortElements[0] << 8);

    changerParameters->NumberDataTransferElements = elementAddressPage->NumberDataXFerElements[1];
    changerParameters->NumberDataTransferElements |= (elementAddressPage->NumberDataXFerElements[0] << 8);
    changerParameters->NumberOfDoors = 1;
    changerParameters->NumberCleanerSlots = 1;

    changerParameters->FirstSlotNumber = 0;
    changerParameters->FirstDriveNumber =  0;
    changerParameters->FirstTransportNumber = 0;
    changerParameters->FirstIEPortNumber = 0;
    changerParameters->FirstCleanerSlotAddress = 0;

    if (changerData->DriveID == UHDL)
    {
        changerParameters->MagazineSize = 8;
    }
    else
    {
        changerParameters->MagazineSize = 0;
    }

    changerParameters->DriveCleanTimeout = 300;

    if (!addressMapping->Initialized) {
        ULONG i;

        //
        // Build address mapping.
        //

        addressMapping->FirstElement[ChangerTransport] = (elementAddressPage->MediumTransportElementAddress[0] << 8) |
                                                          elementAddressPage->MediumTransportElementAddress[1];
        addressMapping->FirstElement[ChangerDrive] = (elementAddressPage->FirstDataXFerElementAddress[0] << 8) |
                                                      elementAddressPage->FirstDataXFerElementAddress[1];
        addressMapping->FirstElement[ChangerIEPort] = (elementAddressPage->FirstIEPortElementAddress[0] << 8) |
                                                       elementAddressPage->FirstIEPortElementAddress[1];
        addressMapping->FirstElement[ChangerSlot] = (elementAddressPage->FirstStorageElementAddress[0] << 8) |
                                                     elementAddressPage->FirstStorageElementAddress[1];
        addressMapping->FirstElement[ChangerDoor] = 0;

        addressMapping->FirstElement[ChangerKeypad] = 0;

        addressMapping->NumberOfElements[ChangerTransport] = elementAddressPage->NumberTransportElements[1];
        addressMapping->NumberOfElements[ChangerTransport] |= (elementAddressPage->NumberTransportElements[0] << 8);

        addressMapping->NumberOfElements[ChangerDrive] = elementAddressPage->NumberDataXFerElements[1];
        addressMapping->NumberOfElements[ChangerDrive] |= (elementAddressPage->NumberDataXFerElements[0] << 8);

        addressMapping->NumberOfElements[ChangerIEPort] = elementAddressPage->NumberIEPortElements[1];
        addressMapping->NumberOfElements[ChangerIEPort] |= (elementAddressPage->NumberIEPortElements[0] << 8);

        addressMapping->NumberOfElements[ChangerSlot] = elementAddressPage->NumberStorageElements[1];
        addressMapping->NumberOfElements[ChangerSlot] |= (elementAddressPage->NumberStorageElements[0] << 8);

        addressMapping->NumberOfElements[ChangerDoor] = 1;
        addressMapping->NumberOfElements[ChangerKeypad] = 1;

        addressMapping->Initialized = TRUE;

        //
        // Determine the lowest element address for use with AllElements.
        //

        for (i = 0; i < ChangerDrive; i++) {
            if (addressMapping->FirstElement[i] < addressMapping->FirstElement[AllElements]) {
                addressMapping->FirstElement[AllElements] = addressMapping->FirstElement[i];
            }
        }
    }

    //
    // Free buffer.
    //

    ChangerClassFreePool(modeBuffer);

    //
    // build transport geometry mode sense.
    //

    RtlZeroMemory(srb, SCSI_REQUEST_BLOCK_SIZE);
    cdb = (PCDB)srb->Cdb;

    modeBuffer = ChangerClassAllocatePool(NonPagedPoolCacheAligned, sizeof(MODE_PARAMETER_HEADER)
                                + sizeof(MODE_TRANSPORT_GEOMETRY_PAGE));
    if (!modeBuffer) {
        ChangerClassFreePool(srb);
        return STATUS_INSUFFICIENT_RESOURCES;
    }


    RtlZeroMemory(modeBuffer, sizeof(MODE_PARAMETER_HEADER) + sizeof(MODE_TRANSPORT_GEOMETRY_PAGE));
    srb->CdbLength = CDB6GENERIC_LENGTH;
    srb->TimeOutValue = 20;
    srb->DataTransferLength = sizeof(MODE_PARAMETER_HEADER) + sizeof(MODE_TRANSPORT_GEOMETRY_PAGE);
    srb->DataBuffer = modeBuffer;

    cdb->MODE_SENSE.OperationCode = SCSIOP_MODE_SENSE;
    cdb->MODE_SENSE.PageCode = MODE_PAGE_TRANSPORT_GEOMETRY;
    cdb->MODE_SENSE.Dbd = 1;
    cdb->MODE_SENSE.AllocationLength = (UCHAR)srb->DataTransferLength;

    //
    // Send the request.
    //

    status = ChangerClassSendSrbSynchronous(DeviceObject,
                                         srb,
                                         srb->DataBuffer,
                                         srb->DataTransferLength,
                                         FALSE);

    if (!NT_SUCCESS(status)) {
        ChangerClassFreePool(srb);
        ChangerClassFreePool(modeBuffer);
        return status;
    }

    changerParameters = Irp->AssociatedIrp.SystemBuffer;
    transportGeometryPage = modeBuffer;
    (PCHAR)transportGeometryPage += sizeof(MODE_PARAMETER_HEADER);

    //
    // Determine if mc has 2-sided media.
    //
    changerParameters->Features0 = transportGeometryPage->Flip ? CHANGER_MEDIUM_FLIP : 0;

    //
    // Set IEPort flags if the changer has at least one IEPort
    //
    if (changerParameters->NumberIEElements != 0) {
       changerParameters->Features1 = CHANGER_IEPORT_USER_CONTROL_OPEN |
                                      CHANGER_IEPORT_USER_CONTROL_CLOSE;
    }

    if (changerData->DriveID == UHDL)
    {
        changerParameters->Features0 |= CHANGER_BAR_CODE_SCANNER_INSTALLED;
    }
    else
    {
        //
        // The ADICs indicates whether a bar-code scanner is
        // attached by setting bit-0 in this byte.
        //
        changerParameters->Features0 |= ((changerData->InquiryData.VendorSpecific[19] & 0x1)) ?
                                             CHANGER_BAR_CODE_SCANNER_INSTALLED : 0;
    }

    //
    // Features based on manual, nothing programatic.
    //

    changerParameters->Features0 |= CHANGER_INIT_ELEM_STAT_WITH_RANGE     |
                                    CHANGER_POSITION_TO_ELEMENT           |
                                    CHANGER_PREDISMOUNT_EJECT_REQUIRED    |
                                    CHANGER_DRIVE_CLEANING_REQUIRED       |
                                    CHANGER_DRIVE_EMPTY_ON_DOOR_ACCESS    |
                                    CHANGER_CLEANER_ACCESS_NOT_VALID;

    if (changerParameters->NumberIEElements == 0) {

        //
        // No real IE Elements - nothing can be programatically locked.
        //

        changerParameters->PositionCapabilities = (CHANGER_TO_SLOT | CHANGER_TO_DRIVE);

    } else {
        changerParameters->PositionCapabilities = (CHANGER_TO_SLOT | CHANGER_TO_DRIVE | CHANGER_TO_IEPORT);
    }

    changerParameters->LockUnlockCapabilities = 0;
    if ((changerData->DriveID) == ADIC_SCALAR) {
        changerParameters->LockUnlockCapabilities = LOCK_UNLOCK_IEPORT;
        changerParameters->Features0 |= CHANGER_LOCK_UNLOCK;
    }

    if (changerData->DriveID == UHDL)
    {
        changerParameters->LockUnlockCapabilities = LOCK_UNLOCK_DOOR;
        changerParameters->Features0 |= (CHANGER_LOCK_UNLOCK | CHANGER_CARTRIDGE_MAGAZINE);
        changerParameters->Features0 &= ~(CHANGER_INIT_ELEM_STAT_WITH_RANGE | CHANGER_PREDISMOUNT_EJECT_REQUIRED);
    }

    changerParameters->NumberCleanerSlots = 0;
    changerParameters->FirstSlotNumber = 1;

    if (changerParameters->Features0 & CHANGER_BAR_CODE_SCANNER_INSTALLED) {

        if ((changerParameters->NumberStorageElements == 17) ||
            (changerParameters->NumberStorageElements == 6)) {

            //
            // This is the 218 or FastStor
            // Barcode scanner steals the lowest slot.
            //

            changerParameters->FirstSlotNumber = 2;

        }
    }

    //
    // Free buffer.
    //

    ChangerClassFreePool(modeBuffer);

    //
    // build device capabilities mode sense.
    //


    RtlZeroMemory(srb, SCSI_REQUEST_BLOCK_SIZE);
    cdb = (PCDB)srb->Cdb;

    length =  sizeof(MODE_PARAMETER_HEADER) + sizeof(MODE_DEVICE_CAPABILITIES_PAGE);
    modeBuffer = ChangerClassAllocatePool(NonPagedPoolCacheAligned, length);

    if (!modeBuffer) {
        ChangerClassFreePool(srb);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory(modeBuffer, length);
    srb->CdbLength = CDB6GENERIC_LENGTH;
    srb->TimeOutValue = 20;
    srb->DataTransferLength = length;
    srb->DataBuffer = modeBuffer;

    cdb->MODE_SENSE.OperationCode = SCSIOP_MODE_SENSE;
    cdb->MODE_SENSE.PageCode = MODE_PAGE_DEVICE_CAPABILITIES;
    cdb->MODE_SENSE.Dbd = 1;
    cdb->MODE_SENSE.AllocationLength = (UCHAR)srb->DataTransferLength;

    //
    // Send the request.
    //

    status = ChangerClassSendSrbSynchronous(DeviceObject,
                                         srb,
                                         srb->DataBuffer,
                                         srb->DataTransferLength,
                                         FALSE);

    if (!NT_SUCCESS(status)) {
        ChangerClassFreePool(srb);
        ChangerClassFreePool(modeBuffer);
        return status;
    }

    //
    // Get the systembuffer and by-pass the mode header for the mode sense data.
    //

    changerParameters = Irp->AssociatedIrp.SystemBuffer;
    capabilitiesPage = modeBuffer;
    (PCHAR)capabilitiesPage += sizeof(MODE_PARAMETER_HEADER);

    //
    // Fill in values in Features that are contained in this page.
    //

    changerParameters->Features0 |= capabilitiesPage->MediumTransport ? CHANGER_STORAGE_DRIVE : 0;
    changerParameters->Features0 |= capabilitiesPage->StorageLocation ? CHANGER_STORAGE_SLOT : 0;
    changerParameters->Features0 |= capabilitiesPage->IEPort ? CHANGER_STORAGE_IEPORT : 0;
    changerParameters->Features0 |= capabilitiesPage->DataXFer ? CHANGER_STORAGE_DRIVE : 0;

    //
    // Fix up the 218's storage capabilities.
    //

    if (changerParameters->NumberIEElements == 0) {
        changerParameters->Features0 &= ~CHANGER_STORAGE_IEPORT;
    }

    //
    // Determine all the move from and exchange from capabilities of this device.
    //

    changerParameters->MoveFromTransport = capabilitiesPage->MTtoMT ? CHANGER_TO_TRANSPORT : 0;
    changerParameters->MoveFromTransport |= capabilitiesPage->MTtoST ? CHANGER_TO_SLOT : 0;
    changerParameters->MoveFromTransport |= capabilitiesPage->MTtoIE ? CHANGER_TO_IEPORT : 0;
    changerParameters->MoveFromTransport |= capabilitiesPage->MTtoDT ? CHANGER_TO_DRIVE : 0;

    changerParameters->MoveFromSlot = capabilitiesPage->STtoMT ? CHANGER_TO_TRANSPORT : 0;
    changerParameters->MoveFromSlot |= capabilitiesPage->STtoST ? CHANGER_TO_SLOT : 0;
    changerParameters->MoveFromSlot |= capabilitiesPage->STtoIE ? CHANGER_TO_IEPORT : 0;
    changerParameters->MoveFromSlot |= capabilitiesPage->STtoDT ? CHANGER_TO_DRIVE : 0;

    changerParameters->MoveFromIePort = capabilitiesPage->IEtoMT ? CHANGER_TO_TRANSPORT : 0;
    changerParameters->MoveFromIePort |= capabilitiesPage->IEtoST ? CHANGER_TO_SLOT : 0;
    changerParameters->MoveFromIePort |= capabilitiesPage->IEtoIE ? CHANGER_TO_IEPORT : 0;
    changerParameters->MoveFromIePort |= capabilitiesPage->IEtoDT ? CHANGER_TO_DRIVE : 0;

    changerParameters->MoveFromDrive = capabilitiesPage->DTtoMT ? CHANGER_TO_TRANSPORT : 0;
    changerParameters->MoveFromDrive |= capabilitiesPage->DTtoST ? CHANGER_TO_SLOT : 0;
    changerParameters->MoveFromDrive |= capabilitiesPage->DTtoIE ? CHANGER_TO_IEPORT : 0;
    changerParameters->MoveFromDrive |= capabilitiesPage->DTtoDT ? CHANGER_TO_DRIVE : 0;

    changerParameters->ExchangeFromTransport = capabilitiesPage->XMTtoMT ? CHANGER_TO_TRANSPORT : 0;
    changerParameters->ExchangeFromTransport |= capabilitiesPage->XMTtoST ? CHANGER_TO_SLOT : 0;
    changerParameters->ExchangeFromTransport |= capabilitiesPage->XMTtoIE ? CHANGER_TO_IEPORT : 0;
    changerParameters->ExchangeFromTransport |= capabilitiesPage->XMTtoDT ? CHANGER_TO_DRIVE : 0;

    changerParameters->ExchangeFromSlot = capabilitiesPage->XSTtoMT ? CHANGER_TO_TRANSPORT : 0;
    changerParameters->ExchangeFromSlot |= capabilitiesPage->XSTtoST ? CHANGER_TO_SLOT : 0;
    changerParameters->ExchangeFromSlot |= capabilitiesPage->XSTtoIE ? CHANGER_TO_IEPORT : 0;
    changerParameters->ExchangeFromSlot |= capabilitiesPage->XSTtoDT ? CHANGER_TO_DRIVE : 0;

    changerParameters->ExchangeFromIePort = capabilitiesPage->XIEtoMT ? CHANGER_TO_TRANSPORT : 0;
    changerParameters->ExchangeFromIePort |= capabilitiesPage->XIEtoST ? CHANGER_TO_SLOT : 0;
    changerParameters->ExchangeFromIePort |= capabilitiesPage->XIEtoIE ? CHANGER_TO_IEPORT : 0;
    changerParameters->ExchangeFromIePort |= capabilitiesPage->XIEtoDT ? CHANGER_TO_DRIVE : 0;

    changerParameters->ExchangeFromDrive = capabilitiesPage->XDTtoMT ? CHANGER_TO_TRANSPORT : 0;
    changerParameters->ExchangeFromDrive |= capabilitiesPage->XDTtoST ? CHANGER_TO_SLOT : 0;
    changerParameters->ExchangeFromDrive |= capabilitiesPage->XDTtoIE ? CHANGER_TO_IEPORT : 0;
    changerParameters->ExchangeFromDrive |= capabilitiesPage->XDTtoDT ? CHANGER_TO_DRIVE : 0;

    //
    // The FastStor's door really isn't a door in
    // the same sense as other changers, neither is its ieport
    //
    if(changerData->DriveID == ADIC_FASTSTOR) {
          changerParameters->NumberIEElements = 0;
          changerParameters->Features0 &= ~(CHANGER_STORAGE_IEPORT |
                                            CHANGER_DRIVE_EMPTY_ON_DOOR_ACCESS);
          changerParameters->Features1 &= ~(CHANGER_IEPORT_USER_CONTROL_OPEN |
                                      CHANGER_IEPORT_USER_CONTROL_CLOSE);
    }

    ChangerClassFreePool(srb);
    ChangerClassFreePool(modeBuffer);

    Irp->IoStatus.Information = sizeof(GET_CHANGER_PARAMETERS);

    return STATUS_SUCCESS;
}


NTSTATUS
ChangerGetStatus(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine returns the status of the medium changer as determined through a TUR.

Arguments:

    DeviceObject
    Irp

Return Value:

    NTSTATUS

--*/

{
    PFUNCTIONAL_DEVICE_EXTENSION   fdoExtension = DeviceObject->DeviceExtension;
    PSCSI_REQUEST_BLOCK srb;
    PCDB     cdb;
    NTSTATUS status;

    srb = ChangerClassAllocatePool(NonPagedPool, SCSI_REQUEST_BLOCK_SIZE);

    if (!srb) {

        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory(srb, SCSI_REQUEST_BLOCK_SIZE);
    cdb = (PCDB)srb->Cdb;

    //
    // Build TUR.
    //

    RtlZeroMemory(srb, SCSI_REQUEST_BLOCK_SIZE);
    cdb = (PCDB)srb->Cdb;

    srb->CdbLength = CDB6GENERIC_LENGTH;
    cdb->CDB6GENERIC.OperationCode = SCSIOP_TEST_UNIT_READY;
    srb->TimeOutValue = 20;

    //
    // Send SCSI command (CDB) to device
    //

    status = ChangerClassSendSrbSynchronous(DeviceObject,
                                         srb,
                                         NULL,
                                         0,
                                         FALSE);

    ChangerClassFreePool(srb);
    return status;
}


NTSTATUS
ChangerGetProductData(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine returns fields from the inquiry data useful for
    identifying the particular device.

Arguments:

    DeviceObject
    Irp

Return Value:

    NTSTATUS

--*/

{

    PFUNCTIONAL_DEVICE_EXTENSION fdoExtension = DeviceObject->DeviceExtension;
    PCHANGER_DATA changerData = (PCHANGER_DATA)(fdoExtension->CommonExtension.DriverData);
    PCHANGER_PRODUCT_DATA productData = Irp->AssociatedIrp.SystemBuffer;

    RtlZeroMemory(productData, sizeof(CHANGER_PRODUCT_DATA));

    //
    // Copy cached inquiry data fields into the system buffer.
    //

    RtlMoveMemory(productData->VendorId, changerData->InquiryData.VendorId, VENDOR_ID_LENGTH);
    RtlMoveMemory(productData->ProductId, changerData->InquiryData.ProductId, PRODUCT_ID_LENGTH);
    RtlMoveMemory(productData->Revision, changerData->InquiryData.ProductRevisionLevel, REVISION_LENGTH);

    //
    // Indicate that this is a tape changer and that media isn't two-sided.
    //

    productData->DeviceType = MEDIUM_CHANGER;

    Irp->IoStatus.Information = sizeof(CHANGER_PRODUCT_DATA);
    return STATUS_SUCCESS;
}



NTSTATUS
ChangerSetAccess(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine sets the state of the door or IEPort. Value can be one of the
    following:


Arguments:

    DeviceObject
    Irp

Return Value:

    NTSTATUS

--*/

{
    PFUNCTIONAL_DEVICE_EXTENSION   fdoExtension =
        DeviceObject->DeviceExtension;

    PCHANGER_DATA       changerData =
        (PCHANGER_DATA)(fdoExtension->CommonExtension.DriverData);

    PCHANGER_ADDRESS_MAPPING addressMapping = &(changerData->AddressMapping);

    PCHANGER_SET_ACCESS setAccess = Irp->AssociatedIrp.SystemBuffer;
    ULONG               controlOperation = setAccess->Control;
    NTSTATUS            status = STATUS_SUCCESS;
    PSCSI_REQUEST_BLOCK srb;
    PCDB                cdb;

    //
    // IEPort solenoid burns out if left activated constantly, on 448 unit.
    // So IEPort locking is only done around MoveMedium calls that involve
    // the IEPORT on these units.
    //
    // For other Scalar units, we'll keep the IEPort locked except when
    // the user is to inject or eject media via the IEPort.
    //
    if ((changerData->DriveID) == ADIC_SCALAR_448) {
        return STATUS_INVALID_DEVICE_REQUEST;
    }

    //
    // Only IEPort lock\unlock function is supported for ADICs and Door
    // lock/unlock is supported for Compaq UHDL
    //

    if (setAccess->Element.ElementType != ChangerIEPort &&
        !(setAccess->Element.ElementType == ChangerDoor && changerData->DriveID == UHDL)) {
        return STATUS_INVALID_DEVICE_REQUEST;
    }

    if ((controlOperation != LOCK_ELEMENT) &&
        (controlOperation != UNLOCK_ELEMENT)) {
        return STATUS_INVALID_DEVICE_REQUEST;
    }

    if (ElementOutOfRange(addressMapping,
                          (USHORT)setAccess->Element.ElementAddress,
                          setAccess->Element.ElementType)) {

        DebugPrint((1, "ChangerSetAccess: Element out of range.\n"));

        return STATUS_ILLEGAL_ELEMENT_ADDRESS;
    }

    srb = ChangerClassAllocatePool(NonPagedPool, SCSI_REQUEST_BLOCK_SIZE);

    if (srb == NULL) {

        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory(srb, SCSI_REQUEST_BLOCK_SIZE);

    cdb = (PCDB)srb->Cdb;

    srb->CdbLength = CDB6GENERIC_LENGTH;

    srb->DataBuffer = NULL;
    srb->DataTransferLength = 0;

    srb->TimeOutValue = 10;

    cdb->MEDIA_REMOVAL.OperationCode = SCSIOP_MEDIUM_REMOVAL;

    if (controlOperation == LOCK_ELEMENT) {
        cdb->MEDIA_REMOVAL.Prevent = 1;
    } else {
        cdb->MEDIA_REMOVAL.Prevent = 0;
    }

    status = ChangerClassSendSrbSynchronous(DeviceObject,
                                            srb,
                                            srb->DataBuffer,
                                            srb->DataTransferLength,
                                            FALSE);
    if (!NT_SUCCESS(status)) {
       DebugPrint((1, "ChangerSetAccess failed. Status - %x",
                   status));
    }

    ChangerClassFreePool(srb);

    return status;
}


NTSTATUS
ChangerGetElementStatus(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine builds and issues a read element status command for either all elements or the
    specified element type. The buffer returned is used to build the user buffer.

Arguments:

    DeviceObject
    Irp

Return Value:

    NTSTATUS

--*/

{

    PFUNCTIONAL_DEVICE_EXTENSION fdoExtension = DeviceObject->DeviceExtension;
    PCHANGER_DATA     changerData = (PCHANGER_DATA)(fdoExtension->CommonExtension.DriverData);
    PCHANGER_ADDRESS_MAPPING     addressMapping = &(changerData->AddressMapping);
    PCHANGER_READ_ELEMENT_STATUS readElementStatus = Irp->AssociatedIrp.SystemBuffer;
    PCHANGER_ELEMENT_STATUS      elementStatus;
    PCHANGER_ELEMENT    element;
    ELEMENT_TYPE        elementType;
    PSCSI_REQUEST_BLOCK srb;
    PCDB     cdb;
    ULONG    length;
    NTSTATUS status;
    PVOID    statusBuffer;
    PCHANGER_ELEMENT_STATUS_EX elementStatusEx;
    ULONG    totalElements = readElementStatus->ElementList.NumberOfElements;
    PIO_STACK_LOCATION     irpStack = IoGetCurrentIrpStackLocation(Irp);
    ULONG    outputBuffLen = irpStack->Parameters.DeviceIoControl.OutputBufferLength;

    //
    // Determine the element type.
    //

    elementType = readElementStatus->ElementList.Element.ElementType;
    element = &readElementStatus->ElementList.Element;

    //
    // length will be based on whether vol. tags are returned and element type(s).
    //

    if (elementType != AllElements)
    {
        if (ElementOutOfRange(addressMapping, (USHORT)element->ElementAddress, elementType))
        {
            DebugPrint((1, "ChangerGetElementStatus: Element out of range.\n"));

            return STATUS_ILLEGAL_ELEMENT_ADDRESS;
        }
    }

    //
    // Allocate first only for the header
    //
    length = sizeof(ELEMENT_STATUS_HEADER);
    statusBuffer = ChangerClassAllocatePool(NonPagedPoolCacheAligned, length);

    if (!statusBuffer)
    {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    srb = ChangerClassAllocatePool(NonPagedPool, SCSI_REQUEST_BLOCK_SIZE);

    if (!srb)
    {
        ChangerClassFreePool(statusBuffer);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

RetrySRB:
    RtlZeroMemory(statusBuffer, length);
    RtlZeroMemory(srb, SCSI_REQUEST_BLOCK_SIZE);

    cdb = (PCDB)srb->Cdb;

    srb->CdbLength = CDB12GENERIC_LENGTH;
    srb->DataBuffer = statusBuffer;
    srb->DataTransferLength = length;
    srb->TimeOutValue = 200;

    cdb->READ_ELEMENT_STATUS.OperationCode = SCSIOP_READ_ELEMENT_STATUS;

    cdb->READ_ELEMENT_STATUS.ElementType = (UCHAR)elementType;
    cdb->READ_ELEMENT_STATUS.VolTag = readElementStatus->VolumeTagInfo;

    //
    // Fill in element addressing info based on the mapping values.
    //

    cdb->READ_ELEMENT_STATUS.StartingElementAddress[0] =
        (UCHAR)((element->ElementAddress + addressMapping->FirstElement[element->ElementType]) >> 8);

    cdb->READ_ELEMENT_STATUS.StartingElementAddress[1] =
        (UCHAR)((element->ElementAddress + addressMapping->FirstElement[element->ElementType]) & 0xFF);

    cdb->READ_ELEMENT_STATUS.NumberOfElements[0] = (UCHAR)(readElementStatus->ElementList.NumberOfElements >> 8);
    cdb->READ_ELEMENT_STATUS.NumberOfElements[1] = (UCHAR)(readElementStatus->ElementList.NumberOfElements & 0xFF);

    cdb->READ_ELEMENT_STATUS.AllocationLength[0] = (UCHAR)(length >> 16);
    cdb->READ_ELEMENT_STATUS.AllocationLength[1] = (UCHAR)(length >> 8);
    cdb->READ_ELEMENT_STATUS.AllocationLength[2] = (UCHAR)(length & 0xFF);

    //
    // ISSUE - 2001/04/11 - nramas : Should change Reserved1 field in CDB
    // to meaningful name.
    //
    if ((elementType == ChangerDrive) && (changerData->ObtainDeviceIdentifier == TRUE) && (changerData->DriveID != UHDL))
    {
        //
        // Set this bit to retrieve device identifier information
        //
        cdb->READ_ELEMENT_STATUS.Reserved1 = 0x01;

        //
        // Since serial number info follows volume tag field,
        // we need to set VolTag bit also in the CDB
        //
        cdb->READ_ELEMENT_STATUS.VolTag = 0x01;
    }
    else
    {
        cdb->READ_ELEMENT_STATUS.Reserved1 = 0x00;
    }

    //
    // Send SCSI command (CDB) to device
    //
    status = ChangerClassSendSrbSynchronous(DeviceObject,
                                         srb,
                                         srb->DataBuffer,
                                         srb->DataTransferLength,
                                         FALSE);

    if (NT_SUCCESS(status) || STATUS_DATA_OVERRUN == status)
    {
        PELEMENT_STATUS_HEADER statusHeader = statusBuffer;;

        if (STATUS_DATA_OVERRUN == status)
        {
            if ((srb->DataTransferLength) <= length)
            {
                status = STATUS_SUCCESS;
            }
            else
            {
                DebugPrint((1, "Adicsc:ReadElementStatus - Dataoverrun.\n"));
                ChangerClassFreePool(srb);
                ChangerClassFreePool(statusBuffer);

                return status;
            }
        }

        //
        // Get the actual needed length
        //
        length =  (statusHeader->ReportByteCount[2]);
        length |= (statusHeader->ReportByteCount[1] << 8);
        length |= (statusHeader->ReportByteCount[0] << 16);

        //
        // Account for the size of the status header
        //
        length += sizeof(ELEMENT_STATUS_HEADER);

        ChangerClassFreePool(statusBuffer);
        ChangerClassFreePool(srb);
    }
    else if (status == STATUS_INVALID_DEVICE_REQUEST)
    {
        //
        // Probably the device doesn't support DVCID bit
        // which retrieves Device Identifier info such as
        // serial number for drives. Try RES once more with
        // DVCID bit turned off.
        //
        if (changerData->ObtainDeviceIdentifier == TRUE)
        {
            changerData->ObtainDeviceIdentifier = FALSE;
            goto RetrySRB;
        }
    }
    else
    {
        ChangerClassFreePool(statusBuffer);
        ChangerClassFreePool(srb);
        return status;
    }

    DebugPrint((3,
               "ChangerGetElementStatus: Allocation Length %x, for %x elements of type %x\n",
               length,
               totalElements,
               elementType));

    statusBuffer = ChangerClassAllocatePool(NonPagedPoolCacheAligned, length);

    if (!statusBuffer) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // Build srb and cdb.
    //

    srb = ChangerClassAllocatePool(NonPagedPool, SCSI_REQUEST_BLOCK_SIZE);

    if (!srb) {
        ChangerClassFreePool(statusBuffer);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

RetryRequest:

    RtlZeroMemory(statusBuffer, length);

    RtlZeroMemory(srb, SCSI_REQUEST_BLOCK_SIZE);
    cdb = (PCDB)srb->Cdb;

    srb->CdbLength = CDB12GENERIC_LENGTH;
    srb->DataBuffer = statusBuffer;
    srb->DataTransferLength = length;
    srb->TimeOutValue = 200;

    cdb->READ_ELEMENT_STATUS.OperationCode = SCSIOP_READ_ELEMENT_STATUS;

    cdb->READ_ELEMENT_STATUS.ElementType = (UCHAR)elementType;
    cdb->READ_ELEMENT_STATUS.VolTag = readElementStatus->VolumeTagInfo;

    //
    // Fill in element addressing info based on the mapping values.
    //

    cdb->READ_ELEMENT_STATUS.StartingElementAddress[0] =
        (UCHAR)((element->ElementAddress + addressMapping->FirstElement[element->ElementType]) >> 8);

    cdb->READ_ELEMENT_STATUS.StartingElementAddress[1] =
        (UCHAR)((element->ElementAddress + addressMapping->FirstElement[element->ElementType]) & 0xFF);

    cdb->READ_ELEMENT_STATUS.NumberOfElements[0] = (UCHAR)(readElementStatus->ElementList.NumberOfElements >> 8);
    cdb->READ_ELEMENT_STATUS.NumberOfElements[1] = (UCHAR)(readElementStatus->ElementList.NumberOfElements & 0xFF);

    cdb->READ_ELEMENT_STATUS.AllocationLength[0] = (UCHAR)(length >> 16);
    cdb->READ_ELEMENT_STATUS.AllocationLength[1] = (UCHAR)(length >> 8);
    cdb->READ_ELEMENT_STATUS.AllocationLength[2] = (UCHAR)(length & 0xFF);

    //
    // ISSUE - 2001/04/11 - nramas : Should change Reserved1 field in CDB
    // to meaningful name.
    //
    if ((elementType == ChangerDrive) && (changerData->ObtainDeviceIdentifier == TRUE) && (changerData->DriveID != UHDL))
    {
        //
        // Set this bit to retrieve device identifier information
        //
        cdb->READ_ELEMENT_STATUS.Reserved1 = 0x01;

        //
        // Since serial number info follows volume tag field,
        // we need to set VolTag bit also in the CDB
        //
        cdb->READ_ELEMENT_STATUS.VolTag = 0x01;
    } else {
        cdb->READ_ELEMENT_STATUS.Reserved1 = 0x00;
    }

    //
    // Send SCSI command (CDB) to device
    //

    status = ChangerClassSendSrbSynchronous(DeviceObject,
                                         srb,
                                         srb->DataBuffer,
                                         srb->DataTransferLength,
                                         FALSE);

    if (NT_SUCCESS(status) || (status == STATUS_DATA_OVERRUN)) {

        PELEMENT_STATUS_HEADER statusHeader = statusBuffer;
        PELEMENT_STATUS_PAGE statusPage;
        PADICS_ELEMENT_DESCRIPTOR elementDescriptor;
        ULONG numberElements = readElementStatus->ElementList.NumberOfElements;
        LONG remainingElements;
        LONG typeCount;
        BOOLEAN tagInfo = readElementStatus->VolumeTagInfo;
        LONG i;
        ULONG descriptorLength;


        //
        // Check if the error was STATUS_DATA_OVERRUN
        //
        if (status == STATUS_DATA_OVERRUN) {
           //
           // Check if there was a DATA_OVERRUN, or was it just
           // DATA_UNDERRUN reported as DATA_OVERRRUN.
           //
           if (srb->DataTransferLength < length) {
              DebugPrint((1,
                          "DATA_UNDERRUN reported as DATA_OVERRUN."));
              DebugPrint((1,
                          "Expected: %d, Transferred: %d.\n",
                          length, srb->DataTransferLength));
              status = STATUS_SUCCESS;
           } else {
              //
              // It was really DATA_OVERRUN error. Report accordingly.
              //
              ChangerClassFreePool(srb);
              ChangerClassFreePool(statusBuffer);

              return status;
           }
        }

        //
        // Determine total number elements returned.
        //

        remainingElements = statusHeader->NumberOfElements[1];
        remainingElements |= (statusHeader->NumberOfElements[0] << 8);

        //
        // The buffer is composed of a header, status page, and element descriptors.
        // Point each element to it's respective place in the buffer.
        //

        (PCHAR)statusPage = (PCHAR)statusHeader;
        (PCHAR)statusPage += sizeof(ELEMENT_STATUS_HEADER);

        elementType = statusPage->ElementType;

        (PCHAR)elementDescriptor = (PCHAR)statusPage;
        (PCHAR)elementDescriptor += sizeof(ELEMENT_STATUS_PAGE);

        descriptorLength = statusPage->ElementDescriptorLength[1];
        descriptorLength |= (statusPage->ElementDescriptorLength[0] << 8);

        //
        // Determine the number of elements of this type reported.
        //

        typeCount =  statusPage->DescriptorByteCount[2];
        typeCount |=  (statusPage->DescriptorByteCount[1] << 8);
        typeCount |=  (statusPage->DescriptorByteCount[0] << 16);

        if (descriptorLength > 0) {
            typeCount /= descriptorLength;
        } else {
            typeCount = 0;
        }

        if ((typeCount == 0) &&
            (remainingElements > 0)) {
            --remainingElements;
        }

        //
        // Fill in user buffer.
        //

        elementStatus = Irp->AssociatedIrp.SystemBuffer;
        RtlZeroMemory(elementStatus, outputBuffLen);

        do {

            for (i = 0; i < typeCount; i++, remainingElements--) {

                //
                // Get the address for this element.
                //

                elementStatus->Element.ElementAddress =
                    elementDescriptor->ElementAddress[1];
                elementStatus->Element.ElementAddress |=
                    (elementDescriptor->ElementAddress[0] << 8);

                //
                // Account for address mapping.
                //

                elementStatus->Element.ElementAddress -= addressMapping->FirstElement[elementType];

                //
                // Set the element type.
                //

                elementStatus->Element.ElementType = elementType;


                if (elementDescriptor->SValid) {

                    ULONG  j;
                    USHORT tmpAddress;


                    //
                    // Source address is valid. Determine the device specific address.
                    //

                    tmpAddress = elementDescriptor->SourceStorageElementAddress[1];
                    tmpAddress |= (elementDescriptor->SourceStorageElementAddress[0] << 8);

                    //
                    // Now convert to 0-based values.
                    //

                    for (j = 1; j <= ChangerDrive; j++) {
                        if (addressMapping->FirstElement[j] <= tmpAddress) {
                            if (tmpAddress < (addressMapping->NumberOfElements[j] + addressMapping->FirstElement[j])) {
                                elementStatus->SrcElementAddress.ElementType = j;
                                break;
                            }
                        }
                    }

                    elementStatus->SrcElementAddress.ElementAddress = tmpAddress - addressMapping->FirstElement[j];

                }

                //
                // Build Flags field.
                //

                elementStatus->Flags = elementDescriptor->Full;
                elementStatus->Flags |= (elementDescriptor->Exception << 2);
                elementStatus->Flags |= (elementDescriptor->Accessible << 3);

                elementStatus->Flags |= (elementDescriptor->LunValid << 12);
                elementStatus->Flags |= (elementDescriptor->IdValid << 13);
                elementStatus->Flags |= (elementDescriptor->NotThisBus << 15);

                elementStatus->Flags |= (elementDescriptor->Invert << 22);
                elementStatus->Flags |= (elementDescriptor->SValid << 23);


                if (elementStatus->Flags & ELEMENT_STATUS_EXCEPT) {
                    elementStatus->ExceptionCode = MapExceptionCodes(elementDescriptor, changerData->DriveID);

                    //
                    // Since exception code 0 indicates no error, clear the
                    // exception bit
                    //
                    if (!elementStatus->ExceptionCode) {
                        elementStatus->Flags &= ~ELEMENT_STATUS_EXCEPT;
                    }
                }

                if (elementDescriptor->IdValid) {
                    elementStatus->TargetId = elementDescriptor->BusAddress;
                }
                if (elementDescriptor->LunValid) {
                    elementStatus->Lun = elementDescriptor->Lun;
                }

                //
                // Ensure that media is actually present. If so, get the tag info.
                //

                if (elementDescriptor->Full) {
                    if (tagInfo) {
                        if (statusPage->PVolTag) {
                            ULONG tagIndex;

                            //
                            // Verify validity of volume tag information.
                            //

                            for (tagIndex = 0; tagIndex < 14; tagIndex++) {
                                if (((PADICS_ELEMENT_DESCRIPTOR_PLUS)elementDescriptor)->VolumeTagDeviceID.VolumeTagInformation[tagIndex] != 0) {
                                    break;
                                }
                            }
                            if (tagIndex == 14) {

                                elementStatus->ExceptionCode = ERROR_LABEL_UNREADABLE;
                                elementStatus->Flags |= ELEMENT_STATUS_EXCEPT;

                            } else {
                                RtlMoveMemory(elementStatus->PrimaryVolumeID,
                                              ((PADICS_ELEMENT_DESCRIPTOR_PLUS)elementDescriptor)->VolumeTagDeviceID.VolumeTagInformation,
                                              MAX_VOLUME_ID_SIZE);

                                elementStatus->Flags |= ELEMENT_STATUS_PVOLTAG;
                            }

                        } else {
                            DebugPrint((1,
                                       "ChangerGetElementStatus: tagInfo requested but PVoltag not set\n"));
                        }
                    }
                }

                if (elementType == ChangerDrive) {
                    if (outputBuffLen >=
                        (totalElements * sizeof(CHANGER_ELEMENT_STATUS_EX))) {

                        PADICS_ELEMENT_DESCRIPTOR_PLUS elementDescPlus =
                            (PADICS_ELEMENT_DESCRIPTOR_PLUS) elementDescriptor;
                        PUCHAR idField = NULL;
                        ULONG idLength = 0;

                        elementStatusEx = (PCHANGER_ELEMENT_STATUS_EX)elementStatus;

                        if (statusPage->PVolTag) {
                            idField =  elementDescPlus->VolumeTagDeviceID.Identifier;
                            idLength = elementDescPlus->VolumeTagDeviceID.IdLength;
                        } else {
                            idField = elementDescPlus->DeviceID.Identifier;
                            idLength = elementDescPlus->DeviceID.IdLength;
                        }

                        if (idLength != 0) {

                            if (idLength > SERIAL_NUMBER_LENGTH) {
                                idLength = SERIAL_NUMBER_LENGTH;
                            }

                            RtlMoveMemory(elementStatusEx->SerialNumber,
                                          idField,
                                          idLength);

                            DebugPrint((3, "Serial number : %s\n",
                                        elementStatusEx->SerialNumber));

                            elementStatusEx->Flags |= ELEMENT_STATUS_PRODUCT_DATA;
                        }
                    }
                }

                //
                // Get next descriptor.
                //

                (PCHAR)elementDescriptor += descriptorLength;

                //
                // Advance to the next entry in the user buffer and element descriptor array.
                //
                if (outputBuffLen >=
                    (totalElements * sizeof(CHANGER_ELEMENT_STATUS_EX))) {
                    DebugPrint((3,
                                "Incrementing by sizeof(CHANGER_ELEMENT_STATUS_EX\n"));
                    (PUCHAR)elementStatus += sizeof(CHANGER_ELEMENT_STATUS_EX);
                } else {
                    elementStatus += 1;
                }

            }

            if (remainingElements > 0) {

                //
                // Get next status page.
                //

                (PCHAR)statusPage = (PCHAR)elementDescriptor;

                elementType = statusPage->ElementType;

                //
                // Point to decriptors.
                //

                (PCHAR)elementDescriptor = (PCHAR)statusPage;
                (PCHAR)elementDescriptor += sizeof(ELEMENT_STATUS_PAGE);

                descriptorLength = statusPage->ElementDescriptorLength[1];
                descriptorLength |= (statusPage->ElementDescriptorLength[0] << 8);

                //
                // Determine the number of this element type reported.
                //

                typeCount =  statusPage->DescriptorByteCount[2];
                typeCount |=  (statusPage->DescriptorByteCount[1] << 8);
                typeCount |=  (statusPage->DescriptorByteCount[0] << 16);

                if (descriptorLength > 0) {
                    typeCount /= descriptorLength;
                } else {
                    typeCount = 0;
                }

                if ((typeCount == 0) &&
                    (remainingElements > 0)) {
                    --remainingElements;
                }
            }

        } while (remainingElements);

        if (outputBuffLen >= (totalElements * sizeof(CHANGER_ELEMENT_STATUS_EX))) {
            Irp->IoStatus.Information = numberElements * sizeof(CHANGER_ELEMENT_STATUS_EX);
        } else {
            Irp->IoStatus.Information = numberElements * sizeof(CHANGER_ELEMENT_STATUS);
        }
    } else if (status == STATUS_INVALID_DEVICE_REQUEST) {
        //
        // Probably the device doesn't support DVCID bit
        // which retrieves Device Identifier info such as
        // serial number for drives. Try RES once more with
        // DVCID bit turned off.
        //
        if (changerData->ObtainDeviceIdentifier == TRUE) {
            changerData->ObtainDeviceIdentifier = FALSE;
            goto RetryRequest;
        }
    }

    ChangerClassFreePool(srb);
    ChangerClassFreePool(statusBuffer);

    return status;
}


NTSTATUS
ChangerInitializeElementStatus(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine issues the necessary command to either initialize all elements
    or the specified range of elements using the normal SCSI-2 command, or a vendor-unique
    range command.

Arguments:

    DeviceObject
    Irp

Return Value:

    NTSTATUS

--*/

{

    PFUNCTIONAL_DEVICE_EXTENSION   fdoExtension = DeviceObject->DeviceExtension;
    PCHANGER_DATA       changerData = (PCHANGER_DATA)(fdoExtension->CommonExtension.DriverData);
    PCHANGER_ADDRESS_MAPPING addressMapping = &(changerData->AddressMapping);
    PCHANGER_INITIALIZE_ELEMENT_STATUS initElementStatus = Irp->AssociatedIrp.SystemBuffer;
    PSCSI_REQUEST_BLOCK srb;
    PCDB                cdb;
    NTSTATUS            status;

    //
    // Build srb and cdb.
    //

    srb = ChangerClassAllocatePool(NonPagedPool, SCSI_REQUEST_BLOCK_SIZE);

    if (!srb) {

        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory(srb, SCSI_REQUEST_BLOCK_SIZE);
    cdb = (PCDB)srb->Cdb;

    if (initElementStatus->ElementList.Element.ElementType == AllElements) {

        //
        // Build the normal SCSI-2 command for all elements.
        //

        srb->CdbLength = CDB6GENERIC_LENGTH;
        cdb->INIT_ELEMENT_STATUS.OperationCode = SCSIOP_INIT_ELEMENT_STATUS;
        cdb->INIT_ELEMENT_STATUS.NoBarCode = initElementStatus->BarCodeScan ? 0 : 1;

        srb->TimeOutValue = fdoExtension->TimeOutValue;
        srb->DataTransferLength = 0;

    } else {

        PCHANGER_ELEMENT_LIST elementList;
        PCHANGER_ELEMENT element;

        if (changerData->DriveID == UHDL)
        {
            ChangerClassFreePool(srb);
            return STATUS_INVALID_PARAMETER;
        }

        elementList = &initElementStatus->ElementList;
        element = &elementList->Element;

        //
        // Use the ADIC vendor-unique initialize with range command
        //

        srb->CdbLength = CDB10GENERIC_LENGTH;
        cdb->INITIALIZE_ELEMENT_RANGE.OperationCode = SCSIOP_INIT_ELEMENT_RANGE;
        cdb->INITIALIZE_ELEMENT_RANGE.Range = 1;

        //
        // Addresses of elements need to be mapped from 0-based to device-specific.
        //

        cdb->INITIALIZE_ELEMENT_RANGE.FirstElementAddress[0] =
            (UCHAR)((element->ElementAddress + addressMapping->FirstElement[element->ElementType]) >> 8);
        cdb->INITIALIZE_ELEMENT_RANGE.FirstElementAddress[1] =
            (UCHAR)((element->ElementAddress + addressMapping->FirstElement[element->ElementType]) & 0xFF);

        cdb->INITIALIZE_ELEMENT_RANGE.NumberOfElements[0] = (UCHAR)(elementList->NumberOfElements >> 8);
        cdb->INITIALIZE_ELEMENT_RANGE.NumberOfElements[1] = (UCHAR)(elementList->NumberOfElements & 0xFF);

        //
        // Indicate whether to use bar code scanning.
        //

        cdb->INITIALIZE_ELEMENT_RANGE.NoBarCode = initElementStatus->BarCodeScan ? 0 : 1;

        srb->TimeOutValue = fdoExtension->TimeOutValue;
        srb->DataTransferLength = 0;
    }

    //
    // Send SCSI command (CDB) to device
    //

    status = ChangerClassSendSrbSynchronous(DeviceObject,
                                         srb,
                                         NULL,
                                         0,
                                         FALSE);

    if (NT_SUCCESS(status)) {
        Irp->IoStatus.Information = sizeof(CHANGER_INITIALIZE_ELEMENT_STATUS);
    }

    ChangerClassFreePool(srb);
    return status;
}


NTSTATUS
ChangerSetPosition(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine issues the appropriate command to set the robotic mechanism to the specified
    element address. Normally used to optimize moves or exchanges by pre-positioning the picker.

Arguments:

    DeviceObject
    Irp

Return Value:

    NTSTATUS

--*/

{
    PFUNCTIONAL_DEVICE_EXTENSION   fdoExtension = DeviceObject->DeviceExtension;
    PCHANGER_DATA       changerData = (PCHANGER_DATA)(fdoExtension->CommonExtension.DriverData);
    PCHANGER_ADDRESS_MAPPING addressMapping = &(changerData->AddressMapping);
    PCHANGER_SET_POSITION setPosition = Irp->AssociatedIrp.SystemBuffer;
    USHORT              transport;
    USHORT              destination;
    PSCSI_REQUEST_BLOCK srb;
    PCDB                cdb;
    NTSTATUS            status;


    //
    // Verify transport, source, and dest. are within range.
    // Convert from 0-based to device-specific addressing.
    //

    transport = (USHORT)(setPosition->Transport.ElementAddress);

    if (ElementOutOfRange(addressMapping, transport, ChangerTransport)) {

        DebugPrint((1,
                   "ChangerSetPosition: Transport element out of range.\n"));

        return STATUS_ILLEGAL_ELEMENT_ADDRESS;
    }

    destination = (USHORT)(setPosition->Destination.ElementAddress);

    if (ElementOutOfRange(addressMapping, destination, setPosition->Destination.ElementType)) {
        DebugPrint((1,
                   "ChangerSetPosition: Destination element out of range.\n"));

        return STATUS_ILLEGAL_ELEMENT_ADDRESS;
    }

    //
    // Convert to device addresses.
    //

    transport += addressMapping->FirstElement[ChangerTransport];
    destination += addressMapping->FirstElement[setPosition->Destination.ElementType];

    //
    // The Scalars don't support 2-sided media.
    //

    if (setPosition->Flip) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Build srb and cdb.
    //

    srb = ChangerClassAllocatePool(NonPagedPool, SCSI_REQUEST_BLOCK_SIZE);

    if (!srb) {

        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory(srb, SCSI_REQUEST_BLOCK_SIZE);
    cdb = (PCDB)srb->Cdb;

    srb->CdbLength = CDB10GENERIC_LENGTH;
    cdb->POSITION_TO_ELEMENT.OperationCode = SCSIOP_POSITION_TO_ELEMENT;

    //
    // Build device-specific addressing.
    //

    cdb->POSITION_TO_ELEMENT.TransportElementAddress[0] = (UCHAR)(transport >> 8);
    cdb->POSITION_TO_ELEMENT.TransportElementAddress[1] = (UCHAR)(transport & 0xFF);

    cdb->POSITION_TO_ELEMENT.DestinationElementAddress[0] = (UCHAR)(destination >> 8);
    cdb->POSITION_TO_ELEMENT.DestinationElementAddress[1] = (UCHAR)(destination & 0xFF);

    //
    // Doesn't support two-sided media, but as a ref. source base, it should be noted.
    //

    cdb->POSITION_TO_ELEMENT.Flip = setPosition->Flip;


    srb->DataTransferLength = 0;
    srb->TimeOutValue = 200;

    //
    // Send SCSI command (CDB) to device
    //

    status = ChangerClassSendSrbSynchronous(DeviceObject,
                                         srb,
                                         NULL,
                                         0,
                                         TRUE);

    if (NT_SUCCESS(status)) {
        Irp->IoStatus.Information = sizeof(CHANGER_SET_POSITION);
    }

    ChangerClassFreePool(srb);
    return status;
}


NTSTATUS
ChangerExchangeMedium(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    None of the Scalars units support exchange medium.

Arguments:

    DeviceObject
    Irp

Return Value:

    STATUS_INVALID_DEVICE_REQUEST

--*/

{
    return STATUS_INVALID_DEVICE_REQUEST;
}


NTSTATUS
ChangerMoveMedium(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:


Arguments:

    DeviceObject
    Irp

Return Value:

    NTSTATUS

--*/


{
    PFUNCTIONAL_DEVICE_EXTENSION   fdoExtension = DeviceObject->DeviceExtension;
    PCHANGER_DATA       changerData = (PCHANGER_DATA)(fdoExtension->CommonExtension.DriverData);
    PCHANGER_ADDRESS_MAPPING addressMapping = &(changerData->AddressMapping);
    PCHANGER_MOVE_MEDIUM moveMedium = Irp->AssociatedIrp.SystemBuffer;
    USHORT              transport;
    USHORT              source;
    USHORT              destination;
    PSCSI_REQUEST_BLOCK srb;
    PCDB                cdb;
    NTSTATUS            status, moveStatus;

    //
    // Verify transport, source, and dest. are within range.
    // Convert from 0-based to device-specific addressing.
    //

    transport = (USHORT)(moveMedium->Transport.ElementAddress);

    if (ElementOutOfRange(addressMapping, transport, ChangerTransport)) {

        DebugPrint((1,
                   "ChangerMoveMedium: Transport element out of range.\n"));

        return STATUS_ILLEGAL_ELEMENT_ADDRESS;
    }

    source = (USHORT)(moveMedium->Source.ElementAddress);

    if (ElementOutOfRange(addressMapping, source, moveMedium->Source.ElementType)) {

        DebugPrint((1,
                   "ChangerMoveMedium: Source element out of range.\n"));

        return STATUS_ILLEGAL_ELEMENT_ADDRESS;
    }

    destination = (USHORT)(moveMedium->Destination.ElementAddress);

    if (ElementOutOfRange(addressMapping, destination, moveMedium->Destination.ElementType)) {
        DebugPrint((1,
                   "ChangerMoveMedium: Destination element out of range.\n"));

        return STATUS_ILLEGAL_ELEMENT_ADDRESS;
    }

    //
    // Convert to device addresses.
    //

    transport += addressMapping->FirstElement[ChangerTransport];
    source += addressMapping->FirstElement[moveMedium->Source.ElementType];
    destination += addressMapping->FirstElement[moveMedium->Destination.ElementType];

    //
    // ADICs don't support 2-sided media.
    //

    if (moveMedium->Flip) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Build srb and cdb.
    //

    srb = ChangerClassAllocatePool(NonPagedPool, SCSI_REQUEST_BLOCK_SIZE);

    if (!srb) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory(srb, SCSI_REQUEST_BLOCK_SIZE);
    cdb = (PCDB)srb->Cdb;


    if ((changerData->DriveID) == ADIC_SCALAR_448) {

        // Pre-MoveMedium Locking ? =========================================
        // if source or destination involved IEPORT, lock the IEPORT first
        if ((moveMedium->Source.ElementType == ChangerIEPort) ||
            (moveMedium->Destination.ElementType == ChangerIEPort)) {

            srb->CdbLength = CDB6GENERIC_LENGTH;
            cdb->MEDIA_REMOVAL.OperationCode = SCSIOP_MEDIUM_REMOVAL;
            srb->DataBuffer = NULL;
            srb->DataTransferLength = 0;
            srb->TimeOutValue = 10;

            // lock the IEPORT !
            cdb->MEDIA_REMOVAL.Prevent = 1;
            status = ChangerClassSendSrbSynchronous(DeviceObject,
                                                 srb,
                                                 srb->DataBuffer,
                                                 srb->DataTransferLength,
                                                 FALSE);
            if (!NT_SUCCESS(status)) {
               DebugPrint((1,
                           "Pre-MoveMedium Locking failed : ",
                           status));
            }
        }
    }


    // MoveMedium  =========================================

    RtlZeroMemory(srb, SCSI_REQUEST_BLOCK_SIZE);
    cdb = (PCDB)srb->Cdb;
    srb->CdbLength = CDB12GENERIC_LENGTH;
    srb->TimeOutValue = fdoExtension->TimeOutValue;

    cdb->MOVE_MEDIUM.OperationCode = SCSIOP_MOVE_MEDIUM;

    cdb->MOVE_MEDIUM.TransportElementAddress[0] = (UCHAR)(transport >> 8);
    cdb->MOVE_MEDIUM.TransportElementAddress[1] = (UCHAR)(transport & 0xFF);

    cdb->MOVE_MEDIUM.SourceElementAddress[0] = (UCHAR)(source >> 8);
    cdb->MOVE_MEDIUM.SourceElementAddress[1] = (UCHAR)(source & 0xFF);

    cdb->MOVE_MEDIUM.DestinationElementAddress[0] = (UCHAR)(destination >> 8);
    cdb->MOVE_MEDIUM.DestinationElementAddress[1] = (UCHAR)(destination & 0xFF);

    cdb->MOVE_MEDIUM.Flip = moveMedium->Flip;
    srb->DataTransferLength = 0;
    moveStatus = ChangerClassSendSrbSynchronous(DeviceObject,
                                         srb,
                                         NULL,
                                         0,
                                         FALSE);

    if (NT_SUCCESS(moveStatus)) {
        Irp->IoStatus.Information = sizeof(CHANGER_MOVE_MEDIUM);
    }

    if ((changerData->DriveID) == ADIC_SCALAR_448) {

        // Post-MoveMedium UnLocking ? =========================================
        // if source or destination involved IEPORT, unlock the IEPORT
        if ((moveMedium->Source.ElementType == ChangerIEPort) ||
            (moveMedium->Destination.ElementType == ChangerIEPort)) {

            RtlZeroMemory(srb, SCSI_REQUEST_BLOCK_SIZE);
            srb->CdbLength = CDB6GENERIC_LENGTH;
            cdb->MEDIA_REMOVAL.OperationCode = SCSIOP_MEDIUM_REMOVAL;
            srb->DataBuffer = NULL;
            srb->DataTransferLength = 0;
            srb->TimeOutValue = 10;

            // unlock the IEPORT !
            cdb->MEDIA_REMOVAL.Prevent = 0;
            status = ChangerClassSendSrbSynchronous(DeviceObject,
                                                 srb,
                                                 srb->DataBuffer,
                                                 srb->DataTransferLength,
                                                 FALSE);
            if (!NT_SUCCESS(status)) {
               DebugPrint((1,
                           "Post-MoveMedium UnLocking failed : ",
                           status));
            }
        }
    }

    ChangerClassFreePool(srb);
    return moveStatus;
}


NTSTATUS
ChangerReinitializeUnit(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:


Arguments:

    DeviceObject
    Irp

Return Value:

    NTSTATUS

--*/

{
    return STATUS_INVALID_DEVICE_REQUEST;
}


NTSTATUS
ChangerQueryVolumeTags(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:


Arguments:

    DeviceObject
    Irp

Return Value:

    NTSTATUS

--*/

{

    return STATUS_INVALID_DEVICE_REQUEST;
}


NTSTATUS
AdicBuildAddressMapping(
    IN PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    This routine issues the appropriate mode sense commands and builds an
    array of element addresses. These are used to translate between the device-specific
    addresses and the zero-based addresses of the API.

Arguments:

    DeviceObject

Return Value:

    NTSTATUS

--*/
{

    PFUNCTIONAL_DEVICE_EXTENSION      fdoExtension = DeviceObject->DeviceExtension;
    PCHANGER_DATA          changerData = (PCHANGER_DATA)(fdoExtension->CommonExtension.DriverData);
    PCHANGER_ADDRESS_MAPPING addressMapping = &changerData->AddressMapping;
    PSCSI_REQUEST_BLOCK    srb;
    PCDB                   cdb;
    NTSTATUS               status;
    PMODE_ELEMENT_ADDRESS_PAGE elementAddressPage;
    PVOID modeBuffer;
    ULONG i;

    srb = ChangerClassAllocatePool(NonPagedPool, SCSI_REQUEST_BLOCK_SIZE);
    if (!srb) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // Set all FirstElements to NO_ELEMENT.
    //

    for (i = 0; i < ChangerMaxElement; i++) {
        addressMapping->FirstElement[i] = ADIC_NO_ELEMENT;
    }

    RtlZeroMemory(srb, SCSI_REQUEST_BLOCK_SIZE);

    cdb = (PCDB)srb->Cdb;

    //
    // Build a mode sense - Element address assignment page.
    //

    modeBuffer = ChangerClassAllocatePool(NonPagedPoolCacheAligned, sizeof(MODE_PARAMETER_HEADER)
                                + sizeof(MODE_ELEMENT_ADDRESS_PAGE));
    if (!modeBuffer) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }


    RtlZeroMemory(modeBuffer, sizeof(MODE_PARAMETER_HEADER) + sizeof(MODE_ELEMENT_ADDRESS_PAGE));
    srb->CdbLength = CDB6GENERIC_LENGTH;
    srb->TimeOutValue = 20;
    srb->DataTransferLength = sizeof(MODE_PARAMETER_HEADER) + sizeof(MODE_ELEMENT_ADDRESS_PAGE);
    srb->DataBuffer = modeBuffer;

    cdb->MODE_SENSE.OperationCode = SCSIOP_MODE_SENSE;
    cdb->MODE_SENSE.PageCode = MODE_PAGE_ELEMENT_ADDRESS;
    cdb->MODE_SENSE.Dbd = 1;
    cdb->MODE_SENSE.AllocationLength = (UCHAR)srb->DataTransferLength;

    //
    // Send the request.
    //

    status = ChangerClassSendSrbSynchronous(DeviceObject,
                                         srb,
                                         srb->DataBuffer,
                                         srb->DataTransferLength,
                                         FALSE);


    elementAddressPage = modeBuffer;
    (PCHAR)elementAddressPage += sizeof(MODE_PARAMETER_HEADER);

    if (NT_SUCCESS(status)) {

        //
        // Build address mapping.
        //

        addressMapping->FirstElement[ChangerTransport] = (elementAddressPage->MediumTransportElementAddress[0] << 8) |
                                                          elementAddressPage->MediumTransportElementAddress[1];
        addressMapping->FirstElement[ChangerDrive] = (elementAddressPage->FirstDataXFerElementAddress[0] << 8) |
                                                      elementAddressPage->FirstDataXFerElementAddress[1];
        addressMapping->FirstElement[ChangerIEPort] = (elementAddressPage->FirstIEPortElementAddress[0] << 8) |
                                                       elementAddressPage->FirstIEPortElementAddress[1];
        addressMapping->FirstElement[ChangerSlot] = (elementAddressPage->FirstStorageElementAddress[0] << 8) |
                                                     elementAddressPage->FirstStorageElementAddress[1];
        addressMapping->FirstElement[ChangerDoor] = 0;

        addressMapping->FirstElement[ChangerKeypad] = 0;

        addressMapping->NumberOfElements[ChangerTransport] = elementAddressPage->NumberTransportElements[1];
        addressMapping->NumberOfElements[ChangerTransport] |= (elementAddressPage->NumberTransportElements[0] << 8);

        addressMapping->NumberOfElements[ChangerDrive] = elementAddressPage->NumberDataXFerElements[1];
        addressMapping->NumberOfElements[ChangerDrive] |= (elementAddressPage->NumberDataXFerElements[0] << 8);

        addressMapping->NumberOfElements[ChangerIEPort] = elementAddressPage->NumberIEPortElements[1];
        addressMapping->NumberOfElements[ChangerIEPort] |= (elementAddressPage->NumberIEPortElements[0] << 8);

        addressMapping->NumberOfElements[ChangerSlot] = elementAddressPage->NumberStorageElements[1];
        addressMapping->NumberOfElements[ChangerSlot] |= (elementAddressPage->NumberStorageElements[0] << 8);

        addressMapping->NumberOfElements[ChangerDoor] = 1;
        addressMapping->NumberOfElements[ChangerKeypad] = 1;

        addressMapping->Initialized = TRUE;

        //
        // Determine the lowest element address for use with AllElements.
        //

        for (i = 0; i < ChangerDrive; i++) {
            if (addressMapping->FirstElement[i] < addressMapping->FirstElement[AllElements]) {

                DebugPrint((1,
                           "BuildAddressMapping: New lowest address %x\n",
                           addressMapping->FirstElement[i]));
                addressMapping->FirstElement[AllElements] = addressMapping->FirstElement[i];
            }
        }
    }



    //
    // Free buffer.
    //

    ChangerClassFreePool(modeBuffer);
    ChangerClassFreePool(srb);

    return status;
}


ULONG
MapExceptionCodes(
    IN PADICS_ELEMENT_DESCRIPTOR ElementDescriptor,
    ULONG DriveID
    )

/*++

Routine Description:

    This routine takes the sense data from the elementDescriptor and creates
    the appropriate bitmap of values.

Arguments:

   ElementDescriptor - pointer to the descriptor page.

Return Value:

    Bit-map of exception codes.

--*/

{
    UCHAR asq = ElementDescriptor->AddSenseCodeQualifier;
    UCHAR asc = ElementDescriptor->AdditionalSenseCode;
    ULONG exceptionCode = 0;

    switch (asc) {
        case 0x00:

            //
            // No error.
            //

            exceptionCode = 0;
            break;

        case 0x83:
            if ((asq == 0x03) ||
                (asq == 0x00 && DriveID == ADIC_SCALAR)) {

                exceptionCode = ERROR_LABEL_QUESTIONABLE;

            } else if (asq == 0x0A && DriveID == ADIC_SCALAR) {
                exceptionCode = ERROR_UNHANDLED_ERROR;
            }
            break;

        default:
            exceptionCode = ERROR_UNHANDLED_ERROR;
            break;
    }

    DebugPrint((1,
               "adicsmc: MapExceptionCode - ASC %x, ASCQ %x ExceptionCode %x\n",
               asc,
               asq,
               exceptionCode));

    return exceptionCode;

}


BOOLEAN
ElementOutOfRange(
    IN PCHANGER_ADDRESS_MAPPING AddressMap,
    IN USHORT ElementOrdinal,
    IN ELEMENT_TYPE ElementType
    )
/*++

Routine Description:

    This routine determines whether the element address passed in is within legal range for
    the device.

Arguments:

    AddressMap - The dds' address map array
    ElementOrdinal - Zero-based address of the element to check.
    ElementType

Return Value:

    TRUE if out of range

--*/
{

    if (ElementOrdinal >= AddressMap->NumberOfElements[ElementType]) {

        DebugPrint((1,
                   "ElementOutOfRange: Type %x, Ordinal %x, Max %x\n",
                   ElementType,
                   ElementOrdinal,
                   AddressMap->NumberOfElements[ElementType]));
        return TRUE;
    } else if (AddressMap->FirstElement[ElementType] == ADIC_NO_ELEMENT) {

        DebugPrint((1,
                   "ElementOutOfRange: No Type %x present\n",
                   ElementType));

        return TRUE;
    }

    return FALSE;
}


NTSTATUS
ChangerPerformDiagnostics(
    IN PDEVICE_OBJECT DeviceObject,
    OUT PWMI_CHANGER_PROBLEM_DEVICE_ERROR changerDeviceError
    )
/*+++

Routine Description :

   This routine performs diagnostics tests on the changer
   to determine if the device is working fine or not. If
   it detects any problem the fields in the output buffer
   are set appropriately.

Arguments :

   DeviceObject         -   Changer device object
   changerDeviceError   -   Buffer in which the diagnostic information
                            is returned.
Return Value :

   NTStatus
--*/
{
   PSCSI_REQUEST_BLOCK srb;
   PCDB                cdb;
   NTSTATUS            status;
   PCHANGER_DATA       changerData;
   PFUNCTIONAL_DEVICE_EXTENSION fdoExtension;
   CHANGER_DEVICE_PROBLEM_TYPE changerProblemType;
   ULONG changerId;
   PUCHAR  resultBuffer;
   ULONG length;

   fdoExtension = DeviceObject->DeviceExtension;
   changerData = (PCHANGER_DATA)(fdoExtension->CommonExtension.DriverData);

   //
   // Initialize the devicestatus in the device extension to
   // ADICSC_DEVICE_PROBLEM_NONE. If the changer returns sense code
   // SCSI_SENSE_HARDWARE_ERROR on SelfTest, we'll set an appropriate
   // devicestatus.
   //
   changerData->DeviceStatus = ADICSC_DEVICE_PROBLEM_NONE;

   changerDeviceError->ChangerProblemType = DeviceProblemNone;

   srb = ChangerClassAllocatePool(NonPagedPool, SCSI_REQUEST_BLOCK_SIZE);

   if (srb == NULL) {
      DebugPrint((1, "ADICSC\\ChangerPerformDiagnostics : No memory\n"));
      return STATUS_INSUFFICIENT_RESOURCES;
   }

   RtlZeroMemory(srb, SCSI_REQUEST_BLOCK_SIZE);
   cdb = (PCDB)srb->Cdb;

   //
   // Set the SRB for Send Diagnostic command
   //
   srb->CdbLength = CDB6GENERIC_LENGTH;
   srb->TimeOutValue = 600;

   cdb->CDB6GENERIC.OperationCode = SCSIOP_SEND_DIAGNOSTIC;

   //
   // Set only SelfTest bit
   //
   cdb->CDB6GENERIC.CommandUniqueBits = 0x2;

   status =  ChangerClassSendSrbSynchronous(DeviceObject,
                                     srb,
                                     srb->DataBuffer,
                                     srb->DataTransferLength,
                                     FALSE);
   if (NT_SUCCESS(status)) {
      changerDeviceError->ChangerProblemType = DeviceProblemNone;
   } else if ((changerData->DeviceStatus) != ADICSC_DEVICE_PROBLEM_NONE) {
       changerDeviceError->ChangerProblemType = DeviceProblemHardware;
   }

   ChangerClassFreePool(srb);
   return status;
}

