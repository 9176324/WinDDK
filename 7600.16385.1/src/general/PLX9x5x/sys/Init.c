/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    Init.c

Abstract:

    Contains most of initialization functions

Environment:

    Kernel mode

--*/

#include "precomp.h"

#include "Init.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, PLxInitializeDeviceExtension)
#pragma alloc_text (PAGE, PLxPrepareHardware)
#pragma alloc_text (PAGE, PLxInitializeDMA)
#endif

NTSTATUS
PLxInitializeDeviceExtension(
    IN PDEVICE_EXTENSION DevExt
    )
/*++
Routine Description:

    This routine is called by EvtDeviceAdd. Here the device context is
    initialized and all the software resources required by the device is
    allocated.

Arguments:

    DevExt     Pointer to the Device Extension

Return Value:

     NTSTATUS

--*/
{
    NTSTATUS    status;
    ULONG       dteCount;
    WDF_IO_QUEUE_CONFIG  queueConfig;

    PAGED_CODE();

    //
    // Set Maximum Transfer Length (which must be less than the SRAM size).
    //
    DevExt->MaximumTransferLength = PCI9656_MAXIMUM_TRANSFER_LENGTH;
    if(DevExt->MaximumTransferLength > PCI9656_SRAM_SIZE) {
        DevExt->MaximumTransferLength = PCI9656_SRAM_SIZE;
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PNP,
                "MaximumTransferLength %d", DevExt->MaximumTransferLength);

    //
    // Calculate the number of DMA_TRANSFER_ELEMENTS + 1 needed to
    // support the MaximumTransferLength.
    //
    dteCount = BYTES_TO_PAGES((ULONG) ROUND_TO_PAGES(
        DevExt->MaximumTransferLength) + PAGE_SIZE);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PNP, "Number of DTEs %d", dteCount);

    //
    // Set the number of DMA_TRANSFER_ELEMENTs (DTE) to be available.
    //
    DevExt->WriteTransferElements = dteCount;
    DevExt->ReadTransferElements  = dteCount;

    //
    // The PCI9656 has two DMA Channels. This driver will use DMA Channel 0
    // as the "ToDevice" channel (Writes) and DMA Channel 1 as the
    // "From Device" channel (Reads).
    //
    // In order to support "duplex" DMA operation (the ability to have
    // concurrent reads and writes) two Dispatch Queues are created:
    // one for the Write (ToDevice) requests and another for the Read
    // (FromDevice) requests.  While eache Dispatch Queue will operate
    // independently for each other, the requests within a given Dispatch
    // Queue will be serialized. This is hardware can only process one request
    // per DMA Channel at a time.
    //


    //
    // Setup a queue to handle only IRP_MJ_WRITE requests in Sequential
    // dispatch mode. This mode ensures there is only one write request
    // outstanding in the driver at any time. Framework will present the next
    // request only if the current request is completed.
    // Since we have configured the queue to dispatch all the specific requests
    // we care about, we don't need a default queue.  A default queue is
    // used to receive requests that are not preconfigured to goto
    // a specific queue.
    //
    WDF_IO_QUEUE_CONFIG_INIT ( &queueConfig,
                              WdfIoQueueDispatchSequential);

    queueConfig.EvtIoWrite = PLxEvtIoWrite;

    status = WdfIoQueueCreate( DevExt->Device,
                                           &queueConfig,
                                           WDF_NO_OBJECT_ATTRIBUTES,
                                           &DevExt->WriteQueue );

    if(!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_PNP,
                    "WdfIoQueueCreate failed: %!STATUS!", status);
        return status;
    }

    //
    // Set the Write Queue forwarding for IRP_MJ_WRITE requests.
    //
    status = WdfDeviceConfigureRequestDispatching( DevExt->Device,
                                       DevExt->WriteQueue,
                                       WdfRequestTypeWrite);

    if(!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_PNP,
                    "DeviceConfigureRequestDispatching failed: %!STATUS!", status);
        return status;
    }


    //
    // Create a new IO Queue for IRP_MJ_READ requests in sequential mode.
    //
    WDF_IO_QUEUE_CONFIG_INIT( &queueConfig,
                              WdfIoQueueDispatchSequential);

    queueConfig.EvtIoRead = PLxEvtIoRead;

    status = WdfIoQueueCreate( DevExt->Device,
                               &queueConfig,
                               WDF_NO_OBJECT_ATTRIBUTES,
                               &DevExt->ReadQueue );

    if(!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_PNP,
                    "WdfIoQueueCreate failed: %!STATUS!", status);
        return status;
    }

    //
    // Set the Read Queue forwarding for IRP_MJ_READ requests.
    //
    status = WdfDeviceConfigureRequestDispatching( DevExt->Device,
                                       DevExt->ReadQueue,
                                       WdfRequestTypeRead);

    if(!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_PNP,
                    "DeviceConfigureRequestDispatching failed: %!STATUS!", status);
        return status;
    }


    //
    // Create a WDFINTERRUPT object.
    //
    status = PLxInterruptCreate(DevExt);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    status = PLxInitializeDMA( DevExt );

    if (!NT_SUCCESS(status)) {
        return status;
    }

    return status;
}


NTSTATUS
PLxPrepareHardware(
    IN PDEVICE_EXTENSION DevExt,
    IN WDFCMRESLIST     ResourcesTranslated
    )
/*++
Routine Description:

    Gets the HW resources assigned by the bus driver from the start-irp
    and maps it to system address space.

Arguments:

    DevExt      Pointer to our DEVICE_EXTENSION

Return Value:

     None

--*/
{
    ULONG               i;
    NTSTATUS            status = STATUS_SUCCESS;
    CHAR              * bar;

    BOOLEAN             foundRegs      = FALSE;
    PHYSICAL_ADDRESS    regsBasePA     = {0};
    ULONG               regsLength     = 0;

    BOOLEAN             foundSRAM      = FALSE;
    PHYSICAL_ADDRESS    SRAMBasePA     = {0};
    ULONG               SRAMLength     = 0;

    BOOLEAN             foundSRAM2     = FALSE;
    PHYSICAL_ADDRESS    SRAM2BasePA    = {0};
    ULONG               SRAM2Length    = 0;

    BOOLEAN             foundPort      = FALSE;
    PHYSICAL_ADDRESS    PortBasePA     = {0};
    ULONG               PortLength     = 0;

    PCM_PARTIAL_RESOURCE_DESCRIPTOR  desc;

    PAGED_CODE();

    //
    // Parse the resource list and save the resource information.
    //
    for (i=0; i < WdfCmResourceListGetCount(ResourcesTranslated); i++) {

        desc = WdfCmResourceListGetDescriptor( ResourcesTranslated, i );

        if(!desc) {
            TraceEvents(TRACE_LEVEL_ERROR, DBG_PNP,
                        "WdfResourceCmGetDescriptor failed");
            return STATUS_DEVICE_CONFIGURATION_ERROR;
        }

        switch (desc->Type) {

            case CmResourceTypeMemory:

                bar = NULL;

                if (foundSRAM && !foundSRAM2 &&
                    desc->u.Memory.Length == PCI9656_SRAM_SIZE) {

                    SRAM2BasePA = desc->u.Memory.Start;
                    SRAM2Length = desc->u.Memory.Length;
                    foundSRAM2 = TRUE;
                    bar = "BAR3";
                }

                if (foundRegs && !foundSRAM &&
                    desc->u.Memory.Length == PCI9656_SRAM_SIZE) {

                    SRAMBasePA = desc->u.Memory.Start;
                    SRAMLength = desc->u.Memory.Length;
                    foundSRAM = TRUE;
                    bar = "BAR2";
                }

                if (!foundRegs &&
                    desc->u.Memory.Length == 0x200) {

                    regsBasePA = desc->u.Memory.Start;
                    regsLength = desc->u.Memory.Length;
                    foundRegs = TRUE;
                    bar = "BAR0";
                }

                TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PNP,
                            " - Memory Resource [%I64X-%I64X] %s",
                            desc->u.Memory.Start.QuadPart,
                            desc->u.Memory.Start.QuadPart +
                            desc->u.Memory.Length,
                            (bar) ? bar : "<unrecognized>" );
                break;

            case CmResourceTypePort:

                bar = NULL;

                if (!foundPort &&
                    desc->u.Port.Length >= 0x100) {

                    PortBasePA = desc->u.Port.Start;
                    PortLength = desc->u.Port.Length;
                    foundPort = TRUE;
                    bar = "BAR1";
                }

                TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PNP,
                            " - Port   Resource [%08I64X-%08I64X] %s",
                            desc->u.Port.Start.QuadPart,
                            desc->u.Port.Start.QuadPart +
                            desc->u.Port.Length,
                            (bar) ? bar : "<unrecognized>" );
                break;

            default:
                //
                // Ignore all other descriptors
                //
                break;
        }
    }

    if (!(foundRegs && foundSRAM)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_PNP,
                    "PLxMapResources: Missing resources");
        return STATUS_DEVICE_CONFIGURATION_ERROR;
    }

    //
    // Map in the Registers Memory resource: BAR0
    //
    DevExt->RegsBase = (PUCHAR) MmMapIoSpace( regsBasePA,
                                              regsLength,
                                              MmNonCached );

    if (!DevExt->RegsBase) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_PNP,
                    " - Unable to map Registers memory %08I64X, length %d",
                    regsBasePA.QuadPart, regsLength);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    DevExt->RegsLength = regsLength;

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PNP,
                " - Registers %p, length %d",
                DevExt->RegsBase, DevExt->RegsLength );

    //
    // Set seperated pointer to PCI9656_REGS structure.
    //
    DevExt->Regs = (PPCI9656_REGS) DevExt->RegsBase;

    //
    // Map in the SRAM Memory Space resource: BAR2
    //
    DevExt->SRAMBase = (PUCHAR) MmMapIoSpace( SRAMBasePA,
                                              SRAMLength,
                                              MmNonCached );

    if (!DevExt->SRAMBase) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_PNP,
                    " - Unable to map SRAM memory %08I64X, length %d",
                    SRAMBasePA.QuadPart,  SRAMLength);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    DevExt->SRAMLength = SRAMLength;

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PNP,
                " - SRAM      %p, length %d",
                DevExt->SRAMBase, DevExt->SRAMLength );

    return status;
}

NTSTATUS
PLxInitializeDMA(
    IN PDEVICE_EXTENSION DevExt
    )
/*++
Routine Description:

    Initializes the DMA adapter.

Arguments:

    DevExt      Pointer to our DEVICE_EXTENSION

Return Value:

     None

--*/
{
    NTSTATUS    status;
    WDF_OBJECT_ATTRIBUTES attributes;

    PAGED_CODE();

    //
    // PLx PCI9656 DMA_TRANSFER_ELEMENTS must be 16-byte aligned
    //
    WdfDeviceSetAlignmentRequirement( DevExt->Device,
                                      PCI9656_DTE_ALIGNMENT_16 );

    //
    // Create a new DMA Enabler instance.
    // Use Scatter/Gather, 64-bit Addresses, Duplex-type profile.
    //
    {
        WDF_DMA_ENABLER_CONFIG   dmaConfig;

        WDF_DMA_ENABLER_CONFIG_INIT( &dmaConfig,
                                     WdfDmaProfileScatterGather64Duplex,
                                     DevExt->MaximumTransferLength );

        TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PNP,
                    " - The DMA Profile is WdfDmaProfileScatterGather64Duplex");

        status = WdfDmaEnablerCreate( DevExt->Device,
                                      &dmaConfig,
                                      WDF_NO_OBJECT_ATTRIBUTES,
                                      &DevExt->DmaEnabler );

        if (!NT_SUCCESS (status)) {

            TraceEvents(TRACE_LEVEL_ERROR, DBG_PNP,
                        "WdfDmaEnablerCreate failed: %!STATUS!", status);
            return status;
        }
    }

    //
    // Allocate common buffer for building writes
    //
    // NOTE: This common buffer will not be cached.
    //       Perhaps in some future revision, cached option could
    //       be used. This would have faster access, but requires
    //       flushing before starting the DMA in PLxStartWriteDma.
    //
    DevExt->WriteCommonBufferSize =
        sizeof(DMA_TRANSFER_ELEMENT) * DevExt->WriteTransferElements;

    status = WdfCommonBufferCreate( DevExt->DmaEnabler,
                                    DevExt->WriteCommonBufferSize,
                                    WDF_NO_OBJECT_ATTRIBUTES,
                                    &DevExt->WriteCommonBuffer );

    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_PNP,
                    "WdfCommonBufferCreate (write) failed: %!STATUS!", status);
        return status;
    }


    DevExt->WriteCommonBufferBase =
        WdfCommonBufferGetAlignedVirtualAddress(DevExt->WriteCommonBuffer);

    DevExt->WriteCommonBufferBaseLA =
        WdfCommonBufferGetAlignedLogicalAddress(DevExt->WriteCommonBuffer);

    RtlZeroMemory( DevExt->WriteCommonBufferBase,
                   DevExt->WriteCommonBufferSize);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PNP,
                "WriteCommonBuffer 0x%p  (#0x%I64X), length %I64d",
                DevExt->WriteCommonBufferBase,
                DevExt->WriteCommonBufferBaseLA.QuadPart,
                WdfCommonBufferGetLength(DevExt->WriteCommonBuffer) );

    //
    // Allocate common buffer for building reads
    //
    // NOTE: This common buffer will not be cached.
    //       Perhaps in some future revision, cached option could
    //       be used. This would have faster access, but requires
    //       flushing before starting the DMA in PLxStartReadDma.
    //
    DevExt->ReadCommonBufferSize =
        sizeof(DMA_TRANSFER_ELEMENT) * DevExt->ReadTransferElements;

    status = WdfCommonBufferCreate( DevExt->DmaEnabler,
                                    DevExt->ReadCommonBufferSize,
                                    WDF_NO_OBJECT_ATTRIBUTES,
                                    &DevExt->ReadCommonBuffer );

    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_PNP,
                    "WdfCommonBufferCreate (read) failed %!STATUS!", status);
        return status;
    }

    DevExt->ReadCommonBufferBase =
        WdfCommonBufferGetAlignedVirtualAddress(DevExt->ReadCommonBuffer);

    DevExt->ReadCommonBufferBaseLA =
        WdfCommonBufferGetAlignedLogicalAddress(DevExt->ReadCommonBuffer);

    RtlZeroMemory( DevExt->ReadCommonBufferBase,
                   DevExt->ReadCommonBufferSize);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PNP,
                "ReadCommonBuffer  0x%p  (#0x%I64X), length %I64d",
                DevExt->ReadCommonBufferBase,
                DevExt->ReadCommonBufferBaseLA.QuadPart,
                WdfCommonBufferGetLength(DevExt->ReadCommonBuffer) );

    //
    // Since we are using sequential queue and processing one request
    // at a time, we will create transaction objects upfront and reuse
    // them to do DMA transfer. Transactions objects are parented to
    // DMA enabler object by default. They will be deleted along with
    // along with the DMA enabler object. So need to delete them
    // explicitly.
    //
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes, TRANSACTION_CONTEXT);
    
    status = WdfDmaTransactionCreate( DevExt->DmaEnabler,
                                      &attributes,
                                      &DevExt->ReadDmaTransaction);

    if(!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_WRITE,
                    "WdfDmaTransactionCreate(read) failed: %!STATUS!", status);
        return status;
    }

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes, TRANSACTION_CONTEXT);
    //
    // Create a new DmaTransaction.
    //
    status = WdfDmaTransactionCreate( DevExt->DmaEnabler,
                                      &attributes,
                                      &DevExt->WriteDmaTransaction );

    if(!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_WRITE,
                    "WdfDmaTransactionCreate(write) failed: %!STATUS!", status);
        return status;
    }

    return status;
}


NTSTATUS
PLxInitWrite(
    IN PDEVICE_EXTENSION DevExt
    )
/*++
Routine Description:

    Initialize write data structures

Arguments:

    DevExt     Pointer to Device Extension

Return Value:

    None

--*/
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PNP, "--> PLxInitWrite");

    //
    // Make sure the Dma0 DAC (Dual Address Cycle) register is set to 0.
    //
    WRITE_REGISTER_ULONG( (PULONG) &DevExt->Regs->Dma0_PCI_DAC, 0 );

    //
    // Clear the saved copy of the DMA Channel 0's CSR
    //
    DevExt->Dma0Csr.uchar = 0;


    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PNP, "<-- PLxInitWrite");

    return STATUS_SUCCESS;
}


NTSTATUS
PLxInitRead(
    IN PDEVICE_EXTENSION DevExt
    )
/*++
Routine Description:

    Initialize read data structures

Arguments:

    DevExt     Pointer to Device Extension

Return Value:

--*/
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PNP, "--> PLxInitRead");

    //
    // Make sure the DMA Chan 1 DAC (Dual Address Cycle) is set to 0.
    //
    WRITE_REGISTER_ULONG( (PULONG) &DevExt->Regs->Dma1_PCI_DAC, 0 );

    //
    // Clear the saved copy of the DMA Channel 1's CSR
    //
    DevExt->Dma1Csr.uchar = 0;


    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PNP, "<-- PLxInitRead");

    return STATUS_SUCCESS;
}

VOID
PLxShutdown(
    IN PDEVICE_EXTENSION DevExt
    )
/*++

Routine Description:

    Reset the device to put the device in a known initial state.
    This is called from D0Exit when the device is torn down or
    when the system is shutdown. Note that Wdf has already
    called out EvtDisable callback to disable the interrupt.

Arguments:

    DevExt -  Pointer to our adapter

Return Value:

    None

--*/
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PNP, "---> PLxShutdown");

    //
    // WdfInterrupt is already disabled so issue a full reset
    //
    if (DevExt->Regs) {

        PLxHardwareReset(DevExt);
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PNP, "<--- PLxShutdown");
}

VOID
PLxHardwareReset(
    IN PDEVICE_EXTENSION DevExt
    )
/*++
Routine Description:

    Called by D0Exit when the device is being disabled or when the system is shutdown to
    put the device in a known initial state.

Arguments:

    DevExt     Pointer to Device Extension

Return Value:

--*/
{
    LARGE_INTEGER delay;

    union {
        EEPROM_CSR  bits;
        ULONG       ulong;
    } eeCSR;

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PNP, "--> PLxIssueFullReset");

    //
    // Drive the 9656 into soft reset.
    //
    eeCSR.ulong =
        READ_REGISTER_ULONG( (PULONG) &DevExt->Regs->EEPROM_Ctrl_Stat );

    eeCSR.bits.SoftwareReset = TRUE;

    WRITE_REGISTER_ULONG( (PULONG) &DevExt->Regs->EEPROM_Ctrl_Stat,
                          eeCSR.ulong );

    //
    // Wait 100 msec.
    //
    delay.QuadPart =  WDF_REL_TIMEOUT_IN_MS(100);

    KeDelayExecutionThread( KernelMode, TRUE, &delay );

    //
    // Finally pull the 9656 out of reset.
    //
    eeCSR.bits.SoftwareReset = FALSE;

    WRITE_REGISTER_ULONG( (PULONG) &DevExt->Regs->EEPROM_Ctrl_Stat,
                          eeCSR.ulong );

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PNP, "<-- PLxIssueFullReset");
}




