//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.
//
//==========================================================================;

#define KSDEBUG_INIT

#include "BDATuner.h"
#include "wdmdebug.h"
#define FUNCTION_FROM_CTL_CODE(ctlCode) ( ((DWORD)(ctlCode & 0x3FFC)) >> 2)

UNICODE_STRING g_RegistryPath;
typedef long (*DispatchFunctionPtr)(
                                    IN PDEVICE_OBJECT DeviceObject,
                                    IN PIRP Irp);

// variable to to class drivers dispatch function.
DispatchFunctionPtr fpClassDispatchfunction;


extern "C"
NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPathName
    )
/*++

Routine Description:

    Sets up the driver object.

Arguments:

    DriverObject -
        Driver object for this instance.

    RegistryPathName -
        Contains the registry path which was used to load this instance.

Return Values:

    Returns STATUS_SUCCESS if the driver was initialized.

--*/
{
    NTSTATUS    Status = STATUS_SUCCESS;

    _DbgPrintF(DEBUGLVL_VERBOSE,("DriverEntry"));

    // DEBUG_BREAK;

    _DbgPrintF(DEBUGLVL_VERBOSE,("DriverEntry SWTuner"));

    Status = KsInitializeDriver( DriverObject,
                                 RegistryPathName,
                                 &DeviceDescriptor);

    // DEBUG_BREAK;

    fpClassDispatchfunction = DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL];
    //  drivers function is chained into dispatch queue
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = CDevice::DeviceControlDispatch;

    return Status;
}

//  Driver Global Device instance #
//
static ULONG    ulDeviceInstance = 1;

STDMETHODIMP_(NTSTATUS)
CDevice::
Create(
    IN PKSDEVICE Device
    )
{
    NTSTATUS    Status = STATUS_SUCCESS;
    CDevice *   pDevice = NULL;

    // DEBUG_BREAK;

    _DbgPrintF(DEBUGLVL_VERBOSE,("CDevice::Create"));

    ASSERT(Device);
    
    //  Allocate memory for the our device class.
    //
    pDevice = new(NonPagedPool,MS_SAMPLE_TUNER_POOL_TAG) CDevice; // Tags the allocated memory 

    
    
    if (pDevice)
    {

       //
        // Add the item to the object bag if we were successful.
        // Whenever the device goes away, the bag is cleaned up and
        // we will be freed.
        //
        // For backwards compatibility with DirectX 8.0, we must grab
        // the device mutex before doing this.  For Windows XP, this is
        // not required, but it is still safe.
        //
        KsAcquireDevice (Device);
        Status = KsAddItemToObjectBag (
            Device -> Bag,
            reinterpret_cast <PVOID> (pDevice),
	    NULL
            );
        KsReleaseDevice (Device);

        if (!NT_SUCCESS (Status)) {
            delete pDevice;
	    return Status;
        }
        
        //  Point the KSDEVICE at our device class.
        //
        Device->Context = pDevice;
    
        //  Point back to the KSDEVICE.
        //
        pDevice->m_pKSDevice = Device;

        //  Make the resource available for a filter to use.
        //
        pDevice->m_ulcResourceUsers = 0;
        pDevice->m_ulCurResourceID = 0;

        //  Get the instance number of this device.
        //
        pDevice->m_ulDeviceInstance = ulDeviceInstance++;
        pDevice->m_czDeviceInstanceID[0] = '\0';
        pDevice->m_bGetDeviceInstanceID = false;

        //  Set the implementation GUID.  For cases where a single
        //  driver is used for multiple implementations the INF
        //  which installs the device would write the implementation
        //  GUID into the registery.  This code would then
        //  read the Implementation GUID from the registry.
        //
        pDevice->m_guidImplemenation = KSMEDIUMSETID_MyImplementation;
        KeInitializeSpinLock(&pDevice->m_Lock);
        InitializeListHead(&pDevice->m_FilterList);
        pDevice->m_hGuideHandleCache = NULL;
    } else
    {
        Status = STATUS_INSUFFICIENT_RESOURCES;
    }

    return Status;
}


void
CDevice::InsertFilter(
    CFilter* pFilter
    )
{
    KIRQL OldIrql;
    KeAcquireSpinLock(&m_Lock, &OldIrql);
    KeAcquireSpinLockAtDpcLevel(&pFilter->m_Lock);

    InsertHeadList(&m_FilterList, &pFilter->m_FilterListEntry);

    KeReleaseSpinLockFromDpcLevel(&pFilter->m_Lock);
    KeReleaseSpinLock(&m_Lock, OldIrql);    
    
}

void
CDevice::RemoveFilter(
    CFilter* pFilter
    )
{
    KIRQL OldIrql;
    KeAcquireSpinLock(&m_Lock, &OldIrql);
    KeAcquireSpinLockAtDpcLevel(&pFilter->m_Lock);

    RemoveEntryList(&pFilter->m_FilterListEntry);

    KeReleaseSpinLockFromDpcLevel(&pFilter->m_Lock);
    KeReleaseSpinLock(&m_Lock, OldIrql);    
    
}


#ifdef MOVING_START
STDMETHODIMP_(NTSTATUS)
CDevice::
PnPStart(
    IN PKSDEVICE            pKSDevice,
    IN PIRP                 pIrp,
    IN PCM_RESOURCE_LIST    pTranslatedResourceList OPTIONAL,
    IN PCM_RESOURCE_LIST    pUntranslatedResourceList OPTIONAL
    )
{
    NTSTATUS            Status = STATUS_SUCCESS;
    CDevice *           pDevice;
    PKSFILTERFACTORY    pKSFilterFactory = NULL;


    // DEBUG_BREAK;

    _DbgPrintF( DEBUGLVL_VERBOSE, ("CDevice::PnPStart"));
    ASSERT( pKSDevice);

    // DEBUG_BREAK;

    //    pDevice = reinterpret_cast<CDevice *>(pKSDevice->Context);
    //    ASSERT(pDevice);

    // initialize private class variables in pDevice here

    //  Initialize the hardware.
    //
    Status = pDevice->InitializeHW();
    if (Status == STATUS_SUCCESS)
    {
        //  Create the the Filter Factory.  This factory is used to
        //  create instances of the filter.
        //
        Status = BdaCreateFilterFactoryEx( m_pKSDevice,
                                           &InitialFilterDescriptor,
                                           &BdaFilterTemplate,
                                           &pKSFilterFactory
                                       );
    }

    if ((Status == STATUS_SUCCESS) && pKSFilterFactory)
    {
        BdaFilterFactoryUpdateCacheData( 
                        pKSFilterFactory,
                        BdaFilterTemplate.pFilterDescriptor
                        );
    }

    return Status;
}
#endif

NTSTATUS
CDevice::
PnPStart (
    IN PCM_RESOURCE_LIST TranslatedResourceList,
    IN PCM_RESOURCE_LIST UntranslatedResourceList
    )

/*++

Routine Description:

    Called at Pnp start.  We start up our virtual hardware simulation.

Arguments:

    TranslatedResourceList -
        The translated resource list from Pnp

    UntranslatedResourceList -
        The untranslated resource list from Pnp

Return Value:

    Success / Failure

--*/

{

    PAGED_CODE();
    DbgPrint("CDevice::PnPStart 3");
    //
    // Normally, we'd do things here like parsing the resource lists and
    // connecting our interrupt.  Since this is a simulation, there isn't
    // much to parse.  The parsing and connection should be the same as
    // any WDM driver.  The sections that will differ are illustrated below
    // in setting up a simulated DMA.
    //

    UNREFERENCED_PARAMETER(UntranslatedResourceList);
    UNREFERENCED_PARAMETER(TranslatedResourceList);

    NTSTATUS Status = STATUS_SUCCESS;
    PKSFILTERFACTORY    pKSFilterFactory = NULL;

    if (Status == STATUS_SUCCESS)
    {
        //  Create the the Filter Factory.  This factory is used to
        //  create instances of the filter.
        //
        Status = BdaCreateFilterFactoryEx( m_pKSDevice,
                                           &InitialFilterDescriptor,
                                           &BdaFilterTemplate,
                                           &pKSFilterFactory
                                       );
    }

    if ((Status == STATUS_SUCCESS) && pKSFilterFactory)
    {
        BdaFilterFactoryUpdateCacheData( 
                        pKSFilterFactory,
                        BdaFilterTemplate.pFilterDescriptor
                        );
    }

    //
    // By PnP, it's possible to receive multiple starts without an intervening
    // stop (to reevaluate resources, for example).  Thus, we only perform
    // creations of the simulation on the initial start and ignore any 
    // subsequent start.  Hardware drivers with resources should evaluate
    // resources and make changes on 2nd start.
    //
    if (!m_pKSDevice -> Started) {

	m_HardwareSimulation = new (NonPagedPool, MS_SAMPLE_CAPTURE_POOL_TAG) CHardwareSimulation (this);
	if (!m_HardwareSimulation) {
	    //
            // If we couldn't create the hardware simulation, fail.
            //
            Status = STATUS_INSUFFICIENT_RESOURCES;
	    return Status;
	    
	} else {
	    //
	    // Add the item to the object bag if we were successful. 
	    //
	    
	    Status = KsAddItemToObjectBag (
		       m_pKSDevice -> Bag,
		       reinterpret_cast <PVOID> (m_HardwareSimulation),
		       reinterpret_cast <PFNKSFREE> (CHardwareSimulation::CleanupHW)
		       );
	    if (!NT_SUCCESS(Status)) {
		delete m_HardwareSimulation;
		return Status;
	    }
	    
	}	

#if !defined(_BUILD_SW_TUNER_ON_X64)
    // DMA operation is to show typical call sequences for a real hardware driver.
    // Since this fake call below does not succeed on 64-bit, it is disabled. 

	INTERFACE_TYPE InterfaceBuffer;
	ULONG InterfaceLength;
	DEVICE_DESCRIPTION DeviceDescription;
	NTSTATUS IfStatus;
	
	if (NT_SUCCESS (Status)) {
	    //
	    // Set up DMA...
	    //
	    IfStatus = IoGetDeviceProperty (
                    m_pKSDevice -> PhysicalDeviceObject,
		    DevicePropertyLegacyBusType,
		    sizeof (INTERFACE_TYPE),
		    &InterfaceBuffer,
		    &InterfaceLength
		    );

            //
            // Initialize our fake device description.  We claim to be a 
            // bus-mastering 32-bit scatter/gather capable piece of hardware.
            //
            // Ordinarilly, we'd be using InterfaceBuffer or 
            // InterfaceTypeUndefined if !NT_SUCCESS (IfStatus) as the 
            // InterfaceType below; however, for the purposes of this sample, 
            // we lie and say we're on the PCI Bus.  Otherwise, we're using map
            // registers on x86 32 bit physical to 32 bit logical and this isn't
            // what I want to show in this sample.
            //
            RtlZeroMemory(&DeviceDescription, sizeof(DeviceDescription));
            DeviceDescription.Version = DEVICE_DESCRIPTION_VERSION;
            DeviceDescription.DmaChannel = ((ULONG) ~0);
            DeviceDescription.InterfaceType = PCIBus;
            DeviceDescription.DmaWidth = Width32Bits;
            DeviceDescription.DmaSpeed = Compatible;
            DeviceDescription.ScatterGather = TRUE;
            DeviceDescription.Master = TRUE;
            DeviceDescription.Dma32BitAddresses = TRUE;
            DeviceDescription.AutoInitialize = FALSE;
            DeviceDescription.MaximumLength = (ULONG) -1;
    
            //
            // Get a DMA adapter object from the system.
            //
            m_DmaAdapterObject = IoGetDmaAdapter (
                m_pKSDevice -> PhysicalDeviceObject,
                &DeviceDescription,
                &m_NumberOfMapRegisters
                );
    
            if (!m_DmaAdapterObject) {
                Status = STATUS_UNSUCCESSFUL;
            }
    
	}

	if (NT_SUCCESS (Status)) {
        
        //
        // Initialize our DMA adapter object with AVStream.  This is 
        // **ONLY** necessary **IF** you are doing DMA directly into
        // capture buffers as this sample does.  For this,
        // KSPIN_FLAG_GENERATE_MAPPINGS must be specified on a queue.
        //

        //
        // The (1 << 20) below is the maximum size of a single s/g mapping
        // that this hardware can handle.  Note that I have pulled this
        // number out of thin air for the "fake" hardware.
        //
        KsDeviceRegisterAdapterObject (
                                    	m_pKSDevice,
                                        m_DmaAdapterObject,
                                        (1 << 20),
                                        sizeof (KSMAPPING)
                                        );
            
        }

#endif
	
    }
    
    return Status;

}

/*************************************************/


void
CDevice::
PnPStop (
    )

/*++

Routine Description:

    This is the pnp stop dispatch for the capture device.  It releases any
    adapter object previously allocated by IoGetDmaAdapter during Pnp Start.

Arguments:

    None

Return Value:

    None

--*/

{

    if (m_DmaAdapterObject) {
        //
        // Return the DMA adapter back to the system.
        //
        m_DmaAdapterObject -> DmaOperations -> 
            PutDmaAdapter (m_DmaAdapterObject);

        m_DmaAdapterObject = NULL;
    }
    if (m_hGuideHandleCache)
    {
        ZwClose(m_hGuideHandleCache);
        m_hGuideHandleCache = NULL;
    }
}




NTSTATUS
CDevice::
InitializeHW(
    )
{
    NTSTATUS    Status = STATUS_SUCCESS;

    //
    //  Initialize the device hardware here.
    //

    return Status;
}



NTSTATUS 
GetDWORDAttributeFromFrequency(IN DWORD dwFreq, IN WCHAR* pszDevInstaceID, IN __nullterminated LPWSTR AttributeName,  OUT DWORD& AttributeValue)
{
    // 1) Convert dword frequency to string.
    // 2) find matching key in registry
    // 3) read SignalQuality value from registry.
    // 4) pass key back to caller.
    
    NTSTATUS Status = STATUS_SUCCESS;
    UNICODE_STRING KeyName;
    KeyName.Buffer = (LPWSTR)ExAllocatePoolWithTag(NonPagedPool,
                                              MAX_PATH+1, 
                                              MS_SAMPLE_TUNER_POOL_TAG
                                              );
    if (KeyName.Buffer == NULL)
    {
        return STATUS_UNSUCCESSFUL;
    }
    RtlZeroMemory(KeyName.Buffer,  MAX_PATH+1);
    KeyName.Length = MAX_PATH;
    KeyName.MaximumLength = MAX_PATH;

    BOOL fUseSatellite = false;

    if (dwFreq > 2000000000)
    {
        fUseSatellite = true;
    }

    UNICODE_STRING StubName;
    RtlInitUnicodeString(&StubName, L"\\Registry\\Machine\\System\\PSWTuner\\");
    UNICODE_STRING Seperator;
    RtlInitUnicodeString(&Seperator, L"\\");
    UNICODE_STRING Parameters;
    RtlInitUnicodeString(&Parameters, L"Device Parameters\\");

    RtlCopyUnicodeString(&KeyName, &StubName);

    UNICODE_STRING DevInstanceID;
    RtlInitUnicodeString(&DevInstanceID, pszDevInstaceID);

    //DbgPrint("Converted Frequency value %S\n", Freq.Buffer);

    RtlAppendUnicodeStringToString(&KeyName, &DevInstanceID);
    RtlAppendUnicodeStringToString(&KeyName, &Seperator);
    RtlAppendUnicodeStringToString(&KeyName, &Parameters);


    UNICODE_STRING Freq;
    Freq.Buffer = (LPWSTR)ExAllocatePoolWithTag(NonPagedPool,
                                              MAX_PATH+1, 
                                              MS_SAMPLE_TUNER_POOL_TAG
                                              );
    Freq.Length = MAX_PATH;
    Freq.MaximumLength = MAX_PATH;
    
    RtlZeroMemory(Freq.Buffer,  MAX_PATH+1);
    RtlIntegerToUnicodeString(dwFreq, 10, &Freq);

    //DbgPrint("Converted Frequency value %S\n", Freq.Buffer);

    RtlAppendUnicodeStringToString(&KeyName, &Freq);

    //DbgPrint("KeyName %S\n", KeyName.Buffer);


   
    Status = PptRegGetDword(RTL_REGISTRY_ABSOLUTE, 
                        KeyName.Buffer,
                        AttributeName, 
                        &AttributeValue);

    //due to accuracy in SAT systems the target frequency is +/- 1Mhz (+/- 1000khz), 
    //and we have two digits for polarisation and DiseqC so +/- 100000 
    if ((!NT_SUCCESS(Status)) && fUseSatellite) 
    {
        RtlCopyUnicodeString(&KeyName, &StubName);

        RtlAppendUnicodeStringToString(&KeyName, &DevInstanceID);
        RtlAppendUnicodeStringToString(&KeyName, &Seperator);
        RtlAppendUnicodeStringToString(&KeyName, &Parameters);
        RtlIntegerToUnicodeString(dwFreq + 100000, 10, &Freq);  //test +1Mhz

        RtlAppendUnicodeStringToString(&KeyName, &Freq);

        //DbgPrint("KeyName %S\n", KeyName.Buffer);

        Status = PptRegGetDword(RTL_REGISTRY_ABSOLUTE, 
            KeyName.Buffer,
            AttributeName, 
            &AttributeValue);
    }

    if ((!NT_SUCCESS(Status)) && fUseSatellite) 
    {
        RtlCopyUnicodeString(&KeyName, &StubName);

        RtlAppendUnicodeStringToString(&KeyName, &DevInstanceID);
        RtlAppendUnicodeStringToString(&KeyName, &Seperator);
        RtlAppendUnicodeStringToString(&KeyName, &Parameters);
        RtlIntegerToUnicodeString(dwFreq - 100000, 10, &Freq);  //test -1Mhz

        RtlAppendUnicodeStringToString(&KeyName, &Freq);

        //DbgPrint("KeyName %S\n", KeyName.Buffer);

        Status = PptRegGetDword(RTL_REGISTRY_ABSOLUTE, 
            KeyName.Buffer,
            AttributeName, 
            &AttributeValue);
    }

    if (!NT_SUCCESS(Status))
    {
        UNICODE_STRING GenericKeyName;
        GenericKeyName.Buffer = (LPWSTR)ExAllocatePoolWithTag(NonPagedPool,
                                                  MAX_PATH+1, 
                                                  MS_SAMPLE_TUNER_POOL_TAG
                                                  );
        GenericKeyName.Length = MAX_PATH;
        GenericKeyName.MaximumLength = MAX_PATH;

        RtlCopyUnicodeString(&GenericKeyName, &StubName);

        //DbgPrint("Converted Frequency value %S\n", Freq.Buffer);
        RtlAppendUnicodeStringToString(&GenericKeyName, &Parameters);
        RtlAppendUnicodeStringToString(&GenericKeyName, &Freq);

        DbgPrint("GenericKeyName %S\n", GenericKeyName.Buffer);


       
        Status = PptRegGetDword(RTL_REGISTRY_ABSOLUTE, 
                            GenericKeyName.Buffer,
                            AttributeName, 
                            &AttributeValue);
        if (!NT_SUCCESS(Status))
        {
            DbgPrint( "%s  Error reading the registry 0x%08x\n", __FUNCTION__, Status); 
            // There was an error, use the default filename.
            AttributeValue= 0;
        }
        //DbgPrint("%S %ld\n", AttributeName, AttributeValue);
        ExFreePoolWithTag(GenericKeyName.Buffer, MS_SAMPLE_TUNER_POOL_TAG);
        
    }
    ExFreePoolWithTag(Freq.Buffer, MS_SAMPLE_TUNER_POOL_TAG);
    ExFreePoolWithTag(KeyName.Buffer, MS_SAMPLE_TUNER_POOL_TAG);

    return Status;
}


DWORD 
GetSignalQualityFromFrequency(IN DWORD dwFreq,__in_ecount(MAX_PATH) IN WCHAR* pszDevInstaceID, OUT DWORD& SignalQuality)
{
    // 1) Convert dword frequency to string.
    // 2) find matching key in registry
    // 3) read SignalQuality value from registry.
    // 4) pass key back to caller.
    
    NTSTATUS Status = STATUS_SUCCESS;
   
    Status = GetDWORDAttributeFromFrequency(dwFreq, pszDevInstaceID, L"SignalQuality", SignalQuality);
    if ((!NT_SUCCESS(Status)) && (dwFreq < 1000000))
    {
        //backward configuration compatibility for those who created configuration files already in Hz
        Status = GetDWORDAttributeFromFrequency(dwFreq * 1000, pszDevInstaceID, L"SignalQuality", SignalQuality);
    }
    
    if (!NT_SUCCESS(Status))
    {
        DbgPrint( "%s  Error reading the registry 0x%08x\n", __FUNCTION__, Status); 
        // There was an error, use the default filename.
        SignalQuality = 0;
    }
    DbgPrint("SignalQuality %ld\n", SignalQuality);

    return Status;
}

DWORD 
GetSignalStrengthFromFrequency(IN DWORD dwFreq, __in_ecount(MAX_PATH) IN WCHAR* pszDevInstaceID, OUT DWORD& SignalStrength)
{
    // 1) Convert dword frequency to string.
    // 2) find matching key in registry
    // 3) read SignalStrength value from registry.
    // 4) pass key back to caller.
    
    NTSTATUS Status = STATUS_SUCCESS;
    Status = GetDWORDAttributeFromFrequency(dwFreq, pszDevInstaceID, L"SignalStrength", SignalStrength);

    if ((!NT_SUCCESS(Status)) && (dwFreq < 1000000))
    {
        //backward configuration compatibility for those who created configuration files already in Hz
        Status = GetDWORDAttributeFromFrequency(dwFreq * 1000, pszDevInstaceID, L"SignalStrength", SignalStrength);
    }
    
    if (!NT_SUCCESS(Status))
    {
        DbgPrint( "%s  Error reading the registry 0x%08x\n", __FUNCTION__, Status); 
        // There was an error, use the default filename.
        SignalStrength = 0;
    }
    DbgPrint("SignalStrength %ld\n", SignalStrength);

    return Status;
}
DWORD 
GetSignalPresentFromFrequency(IN DWORD dwFreq, __in_ecount(MAX_PATH) IN WCHAR* pszDevInstaceID, OUT BOOLEAN& SignalPresent)
{
    // 1) Convert dword frequency to string.
    // 2) find matching key in registry
    // 3) read SignalPresent value from registry.
    // 4) pass key back to caller.
    SignalPresent = FALSE;
    
    NTSTATUS Status = STATUS_SUCCESS;
    DWORD dwSignalPresent = 0;
    Status = GetDWORDAttributeFromFrequency(dwFreq, pszDevInstaceID, L"SignalPresent", dwSignalPresent);

    if ((!NT_SUCCESS(Status)) && (dwFreq < 1000000))
    {
        //backward configuration compatibility for those who created configuration files already in Hz
        Status = GetDWORDAttributeFromFrequency(dwFreq * 1000, pszDevInstaceID, L"SignalPresent", dwSignalPresent);
    }

    if (!NT_SUCCESS(Status))
    {
        DbgPrint( "%s  Error reading the registry 0x%08x\n", __FUNCTION__, Status); 
    }

    if (dwSignalPresent == 0)
    {
        SignalPresent = FALSE;
    }
    else
    {
        SignalPresent = TRUE;
    }
    
    DbgPrint("SignalPresent %ld\n", SignalPresent);

    return Status;
}

DWORD 
GetSignalLockedFromFrequency(IN DWORD dwFreq, __in_ecount(MAX_PATH) IN WCHAR* pszDevInstaceID, OUT BOOLEAN& SignalLocked)
{
    // 1) Convert dword frequency to string.
    // 2) find matching key in registry
    // 3) read SignalLocked value from registry.
    // 4) pass key back to caller.
    SignalLocked = FALSE;
    
    NTSTATUS Status = STATUS_SUCCESS;
    DWORD dwSignalLocked = 0;
    Status = GetDWORDAttributeFromFrequency(dwFreq, pszDevInstaceID, L"SignalLocked", dwSignalLocked);

    if ((!NT_SUCCESS(Status)) && (dwFreq < 1000000))
    {
        //backward configuration compatibility for those who created configuration files already in Hz
        Status = GetDWORDAttributeFromFrequency(dwFreq * 1000, pszDevInstaceID, L"SignalLocked", dwSignalLocked);
    }

    if (!NT_SUCCESS(Status))
    {
        DbgPrint( "%s  Error reading the registry 0x%08x\n", __FUNCTION__, Status); 
    }
    if (dwSignalLocked == 0)
    {
        SignalLocked = FALSE;
    }
    else
    {
        SignalLocked = TRUE;
    }
    
    DbgPrint("SignalLocked %ld\n", SignalLocked);

    return Status;
}


NTSTATUS
CDevice::ReadGuideServiceListFromFile(BYTE **pBuffer, ULONG *pBufferLength)
{
    // read the file name from registry and return the buffer size of the file 
    // or return the Buffer and update the Bufferlength
    
    NTSTATUS    Status = STATUS_SUCCESS;
    if (!pBufferLength)
    {
        return STATUS_INVALID_PARAMETER;
    }

    InitReadDeviceInstance();

    UNICODE_STRING DefaultStreamKeyName;
    DefaultStreamKeyName.Buffer = (LPWSTR)ExAllocatePoolWithTag(NonPagedPool,
                                              MAX_PATH+1, 
                                              MS_SAMPLE_TUNER_POOL_TAG
                                              );
    DefaultStreamKeyName.Length = MAX_PATH;
    DefaultStreamKeyName.MaximumLength = MAX_PATH;
    if (DefaultStreamKeyName.Buffer == NULL)
    {
        return STATUS_UNSUCCESSFUL;
    }
    DefaultStreamKeyName.Buffer[0]=DefaultStreamKeyName.Buffer[1]=0;

    UNICODE_STRING StubName;
    RtlInitUnicodeString(&StubName, L"\\Registry\\Machine\\System\\PSWTuner\\");
    UNICODE_STRING DevInstanceID;
    RtlInitUnicodeString(&DevInstanceID, m_czDeviceInstanceID);

    RtlCopyUnicodeString(&DefaultStreamKeyName, &StubName);
    RtlAppendUnicodeStringToString(&DefaultStreamKeyName, &DevInstanceID);

    UNICODE_STRING FileLocation;
    RtlInitUnicodeString(&FileLocation, NULL);

    DbgPrint("Guide Services Registry Location %S\n", DefaultStreamKeyName.Buffer);

    Status = PptRegGetSz(RTL_REGISTRY_ABSOLUTE, 
                        DefaultStreamKeyName.Buffer,
                        L"GuideServicesFile", 
                        &FileLocation);

    if (!NT_SUCCESS(Status))
    {
        DbgPrint( "GuideServicesFile not in Registry\n"); 
        *pBufferLength = 0;
        goto errExit;
    } 
    
    if(FileLocation.Length)
    {

        DbgPrint("Guide Services File Location %S\n", FileLocation.Buffer);

        OBJECT_ATTRIBUTES oa;
        IO_STATUS_BLOCK iostatus;
        HANDLE hfile = NULL;
        FILE_STANDARD_INFORMATION sfinfo;

        InitializeObjectAttributes(&oa, &FileLocation, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL); 

        Status = ZwCreateFile(&hfile, GENERIC_READ, &oa, &iostatus,
                                NULL, 0, FILE_SHARE_READ, FILE_OPEN,
                                FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);

        RtlFreeUnicodeString(&FileLocation);

        if (!NT_SUCCESS(Status))
        {
            DbgPrint( "GuideServicesFile not existing!\n"); 
            *pBufferLength = 0;
            goto errExit;
        }
        if (pBuffer)
        {
            if (*pBuffer == NULL)
            {
                *pBuffer = (BYTE *)ExAllocatePoolWithTag(NonPagedPool, *pBufferLength , MS_SAMPLE_TUNER_POOL_TAG);

            }
        }
        if (pBuffer == NULL)
        {
            Status = ZwQueryInformationFile(hfile, &iostatus, &sfinfo, sizeof(sfinfo),FileStandardInformation);
            *pBufferLength = sfinfo.EndOfFile.LowPart;
        } 
        else 
        {
            Status = ZwReadFile(hfile, NULL, NULL, NULL, &iostatus, *pBuffer,
            *pBufferLength, NULL, NULL);
        }

        ZwClose(hfile);

    }
    else
    {
        DbgPrint( "GuideServicesFile empty string in Registry\n"); 
        *pBufferLength = 0;
    }

errExit:
    ExFreePoolWithTag(DefaultStreamKeyName.Buffer, MS_SAMPLE_TUNER_POOL_TAG);
    return Status;
}

NTSTATUS 
CDevice::ReadMCMLFromFile(BYTE *pBuffer, ULONG *pBufferLength, UNICODE_STRING MCMLFileName)
{
    NTSTATUS Status = STATUS_SUCCESS;
    if (!pBufferLength)
    {
        return STATUS_INVALID_PARAMETER;
    }

    Status = InitReadDeviceInstance();

    UNICODE_STRING DefaultMCMLKeyName;
    DefaultMCMLKeyName.Buffer = (LPWSTR)ExAllocatePoolWithTag(NonPagedPool,
                                          MAX_PATH+1, 
                                          MS_SAMPLE_TUNER_POOL_TAG
                                          );
    DefaultMCMLKeyName.Length = MAX_PATH;
    DefaultMCMLKeyName.MaximumLength = MAX_PATH;

    UNICODE_STRING FileName;
    FileName.Buffer = (LPWSTR)ExAllocatePoolWithTag(NonPagedPool,
                                              MAX_PATH*2 +1, 
                                              MS_SAMPLE_TUNER_POOL_TAG
                                              );
    FileName.Length = MAX_PATH*2;
    FileName.MaximumLength = MAX_PATH*2;

    UNICODE_STRING StubName;
    RtlInitUnicodeString(&StubName, L"\\Registry\\Machine\\System\\PSWTuner\\");
    UNICODE_STRING DevInstanceID;
    
    RtlInitUnicodeString(&DevInstanceID, m_czDeviceInstanceID);

    RtlCopyUnicodeString(&DefaultMCMLKeyName, &StubName);
    RtlAppendUnicodeStringToString(&DefaultMCMLKeyName, &DevInstanceID);

    UNICODE_STRING FilePath;
    
    RtlZeroMemory(&FilePath, sizeof(FilePath));

    DbgPrint("MCML Registry Location %S\n", DefaultMCMLKeyName.Buffer);
    
    Status = PptRegGetSz(RTL_REGISTRY_ABSOLUTE, 
                        DefaultMCMLKeyName.Buffer,
                        L"MCMLFilePath", 
                        &FilePath);


    if (!NT_SUCCESS(Status))
    {
        DbgPrint( "MCMLFile not in Registry\n"); 
        *pBufferLength = 0;
        goto errExit;
    }
    
    
    if(MCMLFileName.Length && FilePath.Length)
    {
        RtlCopyUnicodeString(&FileName, &FilePath);
        RtlFreeUnicodeString(&FilePath);
        RtlAppendUnicodeStringToString(&FileName, &MCMLFileName);

        DbgPrint("MCML File Location %S\n", FileName.Buffer);

        OBJECT_ATTRIBUTES oa;
        IO_STATUS_BLOCK iostatus;
        HANDLE hfile = NULL;
        FILE_STANDARD_INFORMATION sfinfo;

        InitializeObjectAttributes(&oa, &FileName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL); 
        Status = ZwCreateFile(&hfile, GENERIC_READ, &oa, &iostatus,
                                NULL, 0, FILE_SHARE_READ, FILE_OPEN,
                                FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);

        if (!NT_SUCCESS(Status))
        {
            DbgPrint( "MCMLFile not existing!\n"); 
            *pBufferLength = 0;
            goto errExit;
        }

        if (pBuffer == NULL)
        {
            Status = ZwQueryInformationFile(hfile, &iostatus, &sfinfo, sizeof(sfinfo),FileStandardInformation);
            *pBufferLength = sfinfo.EndOfFile.LowPart;
        } 
        else 
        {
            Status = ZwReadFile(hfile, NULL, NULL, NULL, &iostatus, pBuffer,
              *pBufferLength, NULL, NULL);
        }

        ZwClose(hfile);

    }
    else
    {
        DbgPrint( "MCMLFile empty string in Registry\n"); 
        *pBufferLength = 0;
    }

errExit:
    ExFreePoolWithTag(DefaultMCMLKeyName.Buffer, MS_SAMPLE_TUNER_POOL_TAG);
    ExFreePoolWithTag(FileName.Buffer, MS_SAMPLE_TUNER_POOL_TAG);

    return Status;
}

 /*
 ** PBDA ReadOEMMCMLFile  method of the CFilter class
 ** DMBTH ADD
 **    Return File buffer or File Len.
 **
 */
 #ifdef DMBTH_RECEIVER
NTSTATUS 
CDevice::ReadOEMMCMLFile(BYTE *pBuffer, ULONG *pBufferLength, UNICODE_STRING MCMLFileName)
{
    NTSTATUS Status = STATUS_SUCCESS;
    if (!pBufferLength)
    {
        return STATUS_INVALID_PARAMETER;
    }

    InitReadDeviceInstance();

    UNICODE_STRING DefaultMCMLKeyName;
    DefaultMCMLKeyName.Buffer = (LPWSTR)ExAllocatePoolWithTag(NonPagedPool,
                                          MAX_PATH+1, 
                                          MS_SAMPLE_TUNER_POOL_TAG
                                          );
    DefaultMCMLKeyName.Length = MAX_PATH;
    DefaultMCMLKeyName.MaximumLength = MAX_PATH;

    UNICODE_STRING FileName;
    FileName.Buffer = (LPWSTR)ExAllocatePoolWithTag(NonPagedPool,
                                              MAX_PATH*2 +1, 
                                              MS_SAMPLE_TUNER_POOL_TAG
                                              );
    FileName.Length = MAX_PATH*2;
    FileName.MaximumLength = MAX_PATH*2;

    UNICODE_STRING StubName;
    RtlInitUnicodeString(&StubName, L"\\Registry\\Machine\\System\\PSWTuner\\");
    UNICODE_STRING DevInstanceID;
    
    RtlInitUnicodeString(&DevInstanceID, m_czDeviceInstanceID);

    RtlCopyUnicodeString(&DefaultMCMLKeyName, &StubName);
    RtlAppendUnicodeStringToString(&DefaultMCMLKeyName, &DevInstanceID);

    UNICODE_STRING FilePath;
    
    RtlZeroMemory(&FilePath, sizeof(FilePath));

    DbgPrint("MCML Registry Location %S\n", DefaultMCMLKeyName.Buffer);
    
    Status = PptRegGetSz(RTL_REGISTRY_ABSOLUTE, 
                        DefaultMCMLKeyName.Buffer,
                        L"CNMCMLFile", 
                        &FilePath);


    if (!NT_SUCCESS(Status))
    {
        DbgPrint( "MCMLFile not in Registry\n"); 
        *pBufferLength = 0;
        goto errExit;
    }
    
    
    if(MCMLFileName.Length && FilePath.Length)
    {
        RtlCopyUnicodeString(&FileName, &FilePath);
        RtlFreeUnicodeString(&FilePath);
        RtlAppendUnicodeStringToString(&FileName, &MCMLFileName);

        DbgPrint("MCML File Location %S\n", FileName.Buffer);

        OBJECT_ATTRIBUTES oa;
        IO_STATUS_BLOCK iostatus;
        HANDLE hfile = NULL;
        FILE_STANDARD_INFORMATION sfinfo;

        InitializeObjectAttributes(&oa, &FileName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL); 
        Status = ZwCreateFile(&hfile, GENERIC_READ, &oa, &iostatus,
                                NULL, 0, FILE_SHARE_READ, FILE_OPEN,
                                FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);

        if (!NT_SUCCESS(Status))
        {
            DbgPrint( "MCMLFile not existing!\n"); 
            *pBufferLength = 0;
            goto errExit;
        }

        if (pBuffer == NULL)
        {
            Status = ZwQueryInformationFile(hfile, &iostatus, &sfinfo, sizeof(sfinfo),FileStandardInformation);
            *pBufferLength = sfinfo.EndOfFile.LowPart;
        } 
        else 
        {
            Status = ZwReadFile(hfile, NULL, NULL, NULL, &iostatus, pBuffer,
              *pBufferLength, NULL, NULL);
        }

        ZwClose(hfile);

    }
    else
    {
        DbgPrint( "MCMLFile empty string in Registry\n"); 
        *pBufferLength = 0;
    }

errExit:
    ExFreePoolWithTag(DefaultMCMLKeyName.Buffer, MS_SAMPLE_TUNER_POOL_TAG);
    ExFreePoolWithTag(FileName.Buffer, MS_SAMPLE_TUNER_POOL_TAG);

    return Status;
}
#endif


NTSTATUS
CDevice::GetDeviceParameterString(IN PWSTR pwsParam, OUT PUNICODE_STRING pSzValue)
{
    
    NTSTATUS Status = STATUS_SUCCESS;
    if ((!pSzValue) || (!pwsParam))
    {
        return STATUS_UNSUCCESSFUL;
    }

    InitReadDeviceInstance();

    UNICODE_STRING SzValueKeyName;
    SzValueKeyName.Buffer = (LPWSTR)ExAllocatePoolWithTag(NonPagedPool,
                                              MAX_PATH+1, 
                                              MS_SAMPLE_TUNER_POOL_TAG
                                              );
    SzValueKeyName.Length = MAX_PATH;
    SzValueKeyName.MaximumLength = MAX_PATH;
    if (SzValueKeyName.Buffer == NULL)
    {
        return STATUS_UNSUCCESSFUL;
    }
    SzValueKeyName.Buffer[0]=SzValueKeyName.Buffer[1]=0;

    UNICODE_STRING StubName;
    RtlInitUnicodeString(&StubName, L"\\Registry\\Machine\\System\\PSWTuner\\");
    UNICODE_STRING DevInstanceID;
    RtlInitUnicodeString(&DevInstanceID, m_czDeviceInstanceID);

    RtlCopyUnicodeString(&SzValueKeyName, &StubName);
    RtlAppendUnicodeStringToString(&SzValueKeyName, &DevInstanceID);

    UNICODE_STRING SzValueParam;
    RtlZeroMemory(&SzValueParam, sizeof(SzValueParam));
    
    Status = PptRegGetSz(RTL_REGISTRY_ABSOLUTE, 
                        SzValueKeyName.Buffer,
                        pwsParam, 
                        &SzValueParam);
    if (!NT_SUCCESS(Status))
    {
        RtlInitUnicodeString(&SzValueParam, L"");        
        Status = STATUS_UNSUCCESSFUL;
    } 
    

    if(SzValueParam.Length)
    {
        RtlInitUnicodeString(pSzValue, SzValueParam.Buffer);
    }

    ExFreePoolWithTag(SzValueKeyName.Buffer, MS_SAMPLE_TUNER_POOL_TAG);

    return Status;
}

NTSTATUS
CDevice::InitReadDeviceInstance()
{
    
    NTSTATUS    Status = STATUS_SUCCESS;

    if (!m_bGetDeviceInstanceID)
    {
        UNICODE_STRING DevInstanceID;
        RtlInitUnicodeString(&DevInstanceID, m_czDeviceInstanceID);
        DevInstanceID.MaximumLength = (MAX_PATH + 1) * sizeof(WCHAR);
        //RtlZeroMemory(&DevInstanceID,sizeof(DevInstanceID));

        Status	= PptRegGetDeviceParameterSZ(
            m_pKSDevice -> PhysicalDeviceObject,
            L"DeviceInstanceID",
            &DevInstanceID);

        if (NT_SUCCESS (Status))
        {
            DbgPrint("Get Device Instance ID %S", m_czDeviceInstanceID);
        }

        if(m_czDeviceInstanceID[0])
             m_bGetDeviceInstanceID = true;
    }
    return Status;
}

NTSTATUS
CDevice::
GetStatus(
    PBDATUNER_DEVICE_STATUS     pDeviceStatus
    )
{
    NTSTATUS    Status = STATUS_SUCCESS;
    pDeviceStatus->fCarrierPresent = FALSE;
    pDeviceStatus->fSignalLocked = FALSE;
    pDeviceStatus->dwSignalQuality = 0;
    pDeviceStatus->dwSignalStrength = 0;

    Status = InitReadDeviceInstance();
    // Override the default values with the values stored in the registry.
    ULONG ScaledFrequency;
    //calculated into kHz 
    ULARGE_INTEGER ul64ScaledFrequency;
    ULARGE_INTEGER ul64CarrierFrequency;
    ULARGE_INTEGER ul64Multiplier;
    ul64Multiplier.QuadPart = m_CurResource.ulFrequencyMultiplier;
    ul64CarrierFrequency.LowPart = m_CurResource.ulCarrierFrequency;
    ul64CarrierFrequency.HighPart = 0;
    ul64ScaledFrequency.QuadPart = ul64CarrierFrequency.QuadPart * ul64Multiplier.QuadPart;
    ul64ScaledFrequency.QuadPart /= 1000;
    ScaledFrequency = ul64ScaledFrequency.LowPart;
    pDeviceStatus->fStatusNeedSignaling = m_bFrequencyChanged;
    m_bFrequencyChanged = FALSE;

    if (m_CurResource.fDiseqCEnable == FALSE)
    {
        m_CurResource.ulDiseqC = 0; //if DiseqC is disabled the LNB Source is 0 (vs LNB_A=1)
    }

    if ((m_CurResource.ulPolarity > 0) || (m_CurResource.ulDiseqC > 0)) 
    {
        //for DVB-S we extend the frequency value to Freq[Polarisation][DiseqC]
        ScaledFrequency = ScaledFrequency * 10 + m_CurResource.ulPolarity;
        ScaledFrequency = ScaledFrequency * 10 + m_CurResource.ulDiseqC;
    }
    if ((m_CurResource.ulPLPNumber > 0) && (m_CurResource.ulPLPNumber <= 0xFF))
    {
        ScaledFrequency += (m_CurResource.ulPLPNumber & 0xFF);      
    }
    
    if (pDeviceStatus->fStatusNeedSignaling)
    {
    GetSignalQualityFromFrequency(ScaledFrequency, m_czDeviceInstanceID, pDeviceStatus->dwSignalQuality);
    GetSignalStrengthFromFrequency(ScaledFrequency, m_czDeviceInstanceID, pDeviceStatus->dwSignalStrength);
    GetSignalLockedFromFrequency(ScaledFrequency, m_czDeviceInstanceID, pDeviceStatus->fSignalLocked);
    GetSignalPresentFromFrequency(ScaledFrequency, m_czDeviceInstanceID, pDeviceStatus->fCarrierPresent);
    
        m_dwSignalQuality = pDeviceStatus->dwSignalQuality;
        m_dwSignalStrength = pDeviceStatus->dwSignalStrength;
        m_fSignalLocked = pDeviceStatus->fSignalLocked;
        m_fCarrierPresent = pDeviceStatus->fCarrierPresent;

    } else
    {
        //return the cached value to reduce I/O to the registry
        pDeviceStatus->dwSignalQuality = m_dwSignalQuality;
        pDeviceStatus->dwSignalStrength = m_dwSignalStrength;
        pDeviceStatus->fSignalLocked = m_fSignalLocked;
        pDeviceStatus->fCarrierPresent = m_fCarrierPresent;
    }
    

    return Status;
}

NTSTATUS
CDevice::GetNextAvailableFrequency(
    IN OUT PULONG pulFrequency
    )
{
    NTSTATUS    Status = STATUS_SUCCESS;

    if (!pulFrequency)
    {
        return STATUS_INVALID_PARAMETER;
    }

    InitReadDeviceInstance();

    UNICODE_STRING KeyName;
    KeyName.Buffer = (LPWSTR)ExAllocatePoolWithTag(NonPagedPool,
                                              MAX_PATH+1, 
                                              MS_SAMPLE_TUNER_POOL_TAG
                                              );
    KeyName.Length = MAX_PATH;
    KeyName.MaximumLength = MAX_PATH;

    UNICODE_STRING StubName;
    RtlInitUnicodeString(&StubName, L"\\Registry\\Machine\\System\\PSWTuner\\");
    UNICODE_STRING Seperator;
    RtlInitUnicodeString(&Seperator, L"\\");
    UNICODE_STRING Parameters;
    RtlInitUnicodeString(&Parameters, L"Device Parameters");

    RtlCopyUnicodeString(&KeyName, &StubName);

    UNICODE_STRING DevInstanceID;
    RtlInitUnicodeString(&DevInstanceID, m_czDeviceInstanceID);

    RtlAppendUnicodeStringToString(&KeyName, &DevInstanceID);
    RtlAppendUnicodeStringToString(&KeyName, &Seperator);
    RtlAppendUnicodeStringToString(&KeyName, &Parameters);

    ULONG NextFrequency = 0;
    UNICODE_STRING NextFreqStr;

    NextFreqStr.Buffer = (LPWSTR)ExAllocatePoolWithTag(NonPagedPool,
                                              MAX_PATH+1, 
                                              MS_SAMPLE_TUNER_POOL_TAG
                                              );
    NextFreqStr.Length = MAX_PATH;
    NextFreqStr.MaximumLength = MAX_PATH;   
    RtlZeroMemory(NextFreqStr.Buffer,  MAX_PATH+1);

    UNICODE_STRING CurrentFrequencyStr;
    
    CurrentFrequencyStr.Buffer = (LPWSTR)ExAllocatePoolWithTag(NonPagedPool,
                                              MAX_PATH+1, 
                                              MS_SAMPLE_TUNER_POOL_TAG
                                              );
    CurrentFrequencyStr.Length = MAX_PATH;
    CurrentFrequencyStr.MaximumLength = MAX_PATH;
    
    RtlZeroMemory(CurrentFrequencyStr.Buffer,  MAX_PATH+1);
    RtlIntegerToUnicodeString(*pulFrequency, 10, &CurrentFrequencyStr);

    Status = PptRegGetNextEntry(&KeyName, &CurrentFrequencyStr, &NextFreqStr, &NextFrequency);
    *pulFrequency = NextFrequency;

    ExFreePoolWithTag(CurrentFrequencyStr.Buffer, MS_SAMPLE_TUNER_POOL_TAG);
    ExFreePoolWithTag(NextFreqStr.Buffer, MS_SAMPLE_TUNER_POOL_TAG);
    ExFreePoolWithTag(KeyName.Buffer, MS_SAMPLE_TUNER_POOL_TAG);

    return Status;
}


NTSTATUS
CDevice::
AcquireResources(
    PBDATUNER_DEVICE_RESOURCE   pNewResource,
    PULONG                      pulAcquiredResourceID
    )
{
    NTSTATUS    Status = STATUS_SUCCESS;

    //
    //  Validate the requested resource here.
    //

    //  Check to see if the resources are being used by another
    //  filter instance.
    //
    if (!m_ulcResourceUsers)
    {
        m_CurResource = *pNewResource;

        //  Generate a new resource ID and hand it back.
        //
        m_ulCurResourceID += 25;
        *pulAcquiredResourceID = m_ulCurResourceID;
        m_ulcResourceUsers += 1;
        m_bFrequencyChanged = FALSE;

        //
        //  Configure the new resource on the hardware here.
        //
    }
//#ifdef RESOURCE_SHARING
    //  For resource sharing the IsEqualResource method should be
    //  implemented
    //
    else if (IsEqualBDADeviceResource( pNewResource, &m_CurResource))
    {
        *pulAcquiredResourceID = m_ulCurResourceID;
        m_ulcResourceUsers += 1;
    }
//#endif // RESOURCE_SHARING
    else
    {
        //  We only allow one active filter at a time in this implementation.
        //
        Status = STATUS_DEVICE_BUSY;
    }

    return Status;
}


NTSTATUS
CDevice::
UpdateResources(
    PBDATUNER_DEVICE_RESOURCE   pNewResource,
    ULONG                       ulResourceID
    )
{
    NTSTATUS    Status = STATUS_SUCCESS;

    //
    //  Validate the requested resource here.
    //

    //  Check to see if the resources are being used by another
    //  filter instance.
    //
    if (   m_ulcResourceUsers
        && (ulResourceID == m_ulCurResourceID)
       )
    {
        m_CurResource = *pNewResource;

        //
        //  Configure the updated resource on the hardware here.
        //
    }
    else
    {
        //  We only allow one active filter at a time in this implementation.
        //
        Status = STATUS_INVALID_DEVICE_REQUEST;
    }

    return Status;
}


NTSTATUS
CDevice::
ReleaseResources(
    ULONG                   ulResourceID
    )
{
    NTSTATUS    Status = STATUS_SUCCESS;

    if (   m_ulcResourceUsers
        && (ulResourceID == m_ulCurResourceID)
       )
    {
        //  Free the resource to be used by another filter.
        //
        m_ulcResourceUsers--;
    }
    else
    {
        Status = STATUS_INVALID_DEVICE_REQUEST;
    }

    return Status;
}

void CDevice::FileReaderFreqChanged()
{ 
    if (m_TsSynth) 
        m_TsSynth->FileReaderFreqChanged();
}

NTSTATUS
CDevice::
AcquireHardwareResources (
    IN ICaptureSink *CaptureSink,
    IN PBDA_TRANSPORT_INFO TransportInfo
    )

/*++

Routine Description:

    Acquire hardware resources for the capture hardware.  If the 
    resources are already acquired, this will return an error.
    The hardware configuration must be passed as a VideoInfoHeader.

Arguments:

    CaptureSink -
        The capture sink attempting to acquire resources.  When scatter /
        gather mappings are completed, the capture sink specified here is
        what is notified of the completions.

    VideoInfoHeader -
        Information about the capture stream.  This **MUST** remain
        stable until the caller releases hardware resources.  Note
        that this could also be guaranteed by bagging it in the device
        object bag as well.

Return Value:

    Success / Failure

--*/

{

    PAGED_CODE();

    NTSTATUS Status = STATUS_SUCCESS;

	// We do not get device instance ID in Create or PnpStart Method
	// because the ID information has not been writen to the registry.
    if (!m_bGetDeviceInstanceID)
    {
        UNICODE_STRING DevInstanceID;
        RtlInitUnicodeString(&DevInstanceID, m_czDeviceInstanceID);
        DevInstanceID.MaximumLength = (MAX_PATH + 1) * sizeof(WCHAR);
        //RtlZeroMemory(&DevInstanceID,sizeof(DevInstanceID));

        Status	= PptRegGetDeviceParameterSZ(
            m_pKSDevice -> PhysicalDeviceObject,
            L"DeviceInstanceID",
            &DevInstanceID);

        if (NT_SUCCESS (Status))
        {
            DbgPrint("Get Device Instance ID %S", m_czDeviceInstanceID);
        }

	if(m_czDeviceInstanceID[0])
            m_bGetDeviceInstanceID = true;
    }

       
    m_bFrequencyChanged = FALSE;

    //
    // If we're the first pin to go into acquire (remember we can have
    // a filter in another graph going simultaneously), grab the resources.
    //
    if (InterlockedCompareExchange (
        &m_PinsWithResources,
        1,
        0) == 0) {

        m_TransportInfo = TransportInfo;

        //
        // If there's an old hardware simulation sitting around for some
        // reason, blow it away.
        //
        if (m_TsSynth) {

	    delete m_TsSynth;
	    m_TsSynth = NULL;

        }

        //
        // Create the necessary type of transport stream synthesizer.
        //
        if (m_TransportInfo)
        {
            m_TsSynth = new (NonPagedPool, MS_SAMPLE_CAPTURE_POOL_TAG) 
                CTsSynthesizer (
                    m_pKSDevice, 
                    m_TransportInfo -> ulcbPhyiscalPacket,
                    m_TransportInfo -> ulcbPhyiscalFrame / m_TransportInfo -> ulcbPhyiscalPacket,
                    m_ulFrequency,
                    m_czDeviceInstanceID
                    );
        }
        else
            //
            // We don't synthesize anything but RGB 24 and UYVY.
            //
            Status = STATUS_INVALID_PARAMETER;
    
        if (NT_SUCCESS (Status) && !m_TsSynth) {
    
            Status = STATUS_INSUFFICIENT_RESOURCES;
    
        } 

	//
	// If everything has succeeded thus far, set the capture sink.
	//
	if (NT_SUCCESS(Status))
	    m_CaptureSink = CaptureSink;
	else {
	    ReleaseHardwareResources();
	}

    } else {
	
        //
        // TODO: Better status code?
        //
        Status = STATUS_SHARING_VIOLATION;
	
    }
    
    return Status;

}

/*************************************************/


void
CDevice::
ReleaseHardwareResources (
    )

/*++

Routine Description:

    Release hardware resources.  This should only be called by
    an object which has acquired them.

Arguments:

    None

Return Value:

    None

--*/

{

    PAGED_CODE();

    //
    // Blow away the image synth.
    //
    if (m_TsSynth) {

	delete m_TsSynth;
        m_TsSynth = NULL;

    }

    m_TransportInfo = NULL;
    m_CaptureSink = NULL;
    m_bFrequencyChanged = FALSE;

    //
    // Release our "lock" on hardware resources.  This will allow another
    // pin (perhaps in another graph) to acquire them.
    //
    InterlockedExchange (
        &m_PinsWithResources,
        0
        );

}

NTSTATUS
CDevice::
SetFrequency(
     IN   PULONG pulFrequency
    )
{
    DbgPrint("CDevice::SetFrequency");

    if (m_ulFrequency != *pulFrequency)
    {
        m_ulFrequency = *pulFrequency;
        m_bFrequencyChanged = TRUE;

        if (m_TsSynth) 
                m_TsSynth->SetFrequency(m_ulFrequency);

    }
    return STATUS_SUCCESS;
}

ULONG CDevice::Get_theFrequency()
{
    if (m_ulFrequency > 99000000)
    {
        return (m_ulFrequency/100); //remove the DiseqC and Polarisation value, return only the Centerfrequency
    }
	return m_ulFrequency;
}

ULONG CDevice::GetFrequency()
{
	return m_ulFrequency;
}

NTSTATUS CDevice::GetNextGuideSection(BYTE *pNextGuideSection,ULONG *pulSectionLength, ULONG *pulPercentage)
{
    NTSTATUS Status = STATUS_UNSUCCESSFUL;
    IO_STATUS_BLOCK iostatus;
    
    if (m_TsSynth)
    {
        Status = m_TsSynth->GetNextGuideSection(pNextGuideSection,pulSectionLength, pulPercentage);
    }
    else
    {
        //the graph is not running, so we only can return Guide data from the cached file
        if (!pulSectionLength)
        {
            return STATUS_UNSUCCESSFUL;
        }

        if (m_hGuideHandleCache==NULL)
        {
            Status = OpenEPGStreamCached(&m_hGuideHandleCache);
            m_LastGuideBufferLength = 0;
            m_LastGuidePercentage = 0;
            
            if ((Status != STATUS_SUCCESS) || (m_hGuideHandleCache == NULL))
            {
                return Status;
            }
        }
        BYTE *pBuffer = m_pLastGuideBuffer;
        ULONG *pBufferLength = &m_LastGuideBufferLength;

        if (m_LastGuideBufferLength)
        {
            if ((*pulSectionLength >= m_LastGuideBufferLength) && pNextGuideSection) 
            {
                memcpy(pNextGuideSection,pBuffer,m_LastGuideBufferLength); //return the previous read Buffer
                *pulSectionLength = m_LastGuideBufferLength; //return the previous Length
                m_LastGuideBufferLength = 0; //reset the Buffer
            }
            else
            {
                *pulSectionLength = m_LastGuideBufferLength; //return the previous Length
            }
            if (pulPercentage) *pulPercentage = m_LastGuidePercentage;

            return STATUS_SUCCESS;
        }
        
        if (*pulSectionLength > 0)
        {
            pBuffer = pNextGuideSection; //copy direct into the buffer
            pBufferLength = pulSectionLength;
        }

        if (pulPercentage) *pulPercentage = 0;
        m_LastGuidePercentage = 0;

        //copy to calling buffer or local member
        Status = ZwReadFile(m_hGuideHandleCache,
          NULL,
          NULL,
          NULL,
          &iostatus,
          pBufferLength,
          sizeof(ULONG),
          NULL,
          NULL);

        if (((ULONG)iostatus.Information == 0) || (Status != STATUS_SUCCESS) || (*pBufferLength > 8192) )
        {
            ZwClose(m_hGuideHandleCache);
            m_hGuideHandleCache = NULL;
            *pBufferLength = 0;
            *pulPercentage = 100;
            return STATUS_SUCCESS;
        }

        Status = ZwReadFile(m_hGuideHandleCache,
          NULL,
          NULL,
          NULL,
          &iostatus,
          pBuffer,
          *pBufferLength,
          NULL,
          NULL);

        if (Status == STATUS_SUCCESS)
        {            
            if (((ULONG)iostatus.Information) > (*pBufferLength))
            {
                DbgPrint("Buffer Length error Reading from cached Guide File!!");
                *pBufferLength = 0;
                Status = STATUS_UNSUCCESSFUL;
            }
            *pBufferLength = (ULONG)iostatus.Information;
            *pulSectionLength = *pBufferLength;

            if (m_hGuideHandleCache)
            {
                //calc the percentage based on the file size
                FILE_STANDARD_INFORMATION sfinfo;
                Status = ZwQueryInformationFile(m_hGuideHandleCache, &iostatus, &sfinfo, sizeof(sfinfo),FileStandardInformation);
                if (Status == STATUS_SUCCESS)
                {
                    DWORD dwFileSize = sfinfo.EndOfFile.LowPart;
                    FILE_POSITION_INFORMATION fpinfo;
                    Status = ZwQueryInformationFile(m_hGuideHandleCache, &iostatus, &fpinfo, sizeof(fpinfo),FilePositionInformation);
                    if ((Status == STATUS_SUCCESS) && dwFileSize)
                    {
                        DWORD dwFilePosition = fpinfo.CurrentByteOffset.LowPart;
                        m_LastGuidePercentage = dwFilePosition * 100 / dwFileSize;

                    }
                    else
                    {
                        m_LastGuidePercentage = 100;
                    }
                }
                else
                {
                    m_LastGuidePercentage = 100;
                }
            }

            if (pulPercentage)
            {
                *pulPercentage = m_LastGuidePercentage;
            }
        }
        else
        {
            ZwClose(m_hGuideHandleCache);
            m_hGuideHandleCache = NULL;
            *pBufferLength = 0;
            *pulPercentage = 100;
        }

    }
    return Status;
}

//Open a previously Cached Guide file, which was generated by the GuideCache=1 Option.
NTSTATUS CDevice::OpenEPGStreamCached(HANDLE *pFileHandlePBDASI)
{
    NTSTATUS Status = STATUS_SUCCESS;
    UNICODE_STRING FileName;
    FileName.Buffer = (LPWSTR)ExAllocatePoolWithTag(NonPagedPool,
                                              MAX_PATH+1, 
                                              MS_SAMPLE_TUNER_POOL_TAG
                                              );
    FileName.Length = MAX_PATH;
    FileName.MaximumLength = MAX_PATH;

    UNICODE_STRING FilePathDefault;
    RtlInitUnicodeString(&FilePathDefault, L"\\??\\C:\\swtuner\\");

    UNICODE_STRING GuideFileName;
    RtlInitUnicodeString(&GuideFileName, L"GuideData");//we use a fixed file to load from (instead of the instance)

    UNICODE_STRING FilePath;    
    RtlZeroMemory(&FilePath, sizeof(FilePath));

    Status = GetSwtunerInstallPath(&FilePath);
    if(NT_SUCCESS(Status) && FilePath.Buffer)
    {
        RtlCopyUnicodeString(&FileName, &FilePath);
        RtlFreeUnicodeString(&FilePath);
    }
    else
    {
        RtlCopyUnicodeString(&FileName, &FilePathDefault);
    }

    UNICODE_STRING ExtensionName;
    RtlInitUnicodeString(&ExtensionName, L".pbdasi");

    RtlAppendUnicodeStringToString(&FileName, &GuideFileName);

    RtlAppendUnicodeStringToString(&FileName, &ExtensionName);

    OBJECT_ATTRIBUTES FileObject;
    IO_STATUS_BLOCK IoStatus;  

    InitializeObjectAttributes(&FileObject,
       &FileName,
       OBJ_KERNEL_HANDLE,    
       NULL,
       NULL);

    Status = ZwCreateFile(pFileHandlePBDASI,
       FILE_READ_DATA | SYNCHRONIZE,
       &FileObject,
       &IoStatus,
       NULL,
       FILE_ATTRIBUTE_NORMAL,
       FILE_SHARE_READ,
       FILE_OPEN,
       FILE_SYNCHRONOUS_IO_NONALERT,
       NULL,
       0);
        
    ExFreePoolWithTag(FileName.Buffer, MS_SAMPLE_TUNER_POOL_TAG);

    return Status;

}

//return the swtuner installer directory 
NTSTATUS CDevice::GetSwtunerInstallPath(OUT PUNICODE_STRING FilePath) 
{
    NTSTATUS Status = STATUS_SUCCESS;

    UNICODE_STRING KeyName;
    KeyName.Buffer = (LPWSTR)ExAllocatePoolWithTag(NonPagedPool,
                                              MAX_PATH+1, 
                                              MS_SAMPLE_TUNER_POOL_TAG
                                              );
    KeyName.Length = MAX_PATH;
    KeyName.MaximumLength = MAX_PATH;

    UNICODE_STRING StubName;
    RtlInitUnicodeString(&StubName, L"\\Registry\\Machine\\System\\PSWTuner\\");
    UNICODE_STRING DevInstanceID;
    RtlInitUnicodeString(&DevInstanceID, m_czDeviceInstanceID);


    RtlCopyUnicodeString(&KeyName, &StubName);
    RtlAppendUnicodeStringToString(&KeyName, &DevInstanceID);


    Status = PptRegGetSz(RTL_REGISTRY_ABSOLUTE, 
                        KeyName.Buffer,
                        L"MCMLFilePath", 
                        FilePath);


    ExFreePoolWithTag(KeyName.Buffer , MS_SAMPLE_TUNER_POOL_TAG);
    return Status;
}


/*************************************************/


NTSTATUS
CDevice::
Start (
    )

/*++

Routine Description:

    Start the capture device based on the video info header we were told
    about when resources were acquired.

Arguments:

    None

Return Value:

    Success / Failure

--*/

{

    PAGED_CODE();

    GetDeviceInstanceID();

    m_LastMappingsCompleted = 0;
    m_InterruptTime = 0;
    m_lLatestTSID = -1L;

    m_TsSynth -> Start();

    return
        m_HardwareSimulation -> Start (
            m_TsSynth,
            m_TransportInfo -> AvgTimePerFrame,
            m_TransportInfo -> ulcbPhyiscalPacket,
            m_TransportInfo -> ulcbPhyiscalFrame / m_TransportInfo -> ulcbPhyiscalPacket
            );


}


WCHAR* 
CDevice::
GetDeviceInstanceID()
{
    if (!m_bGetDeviceInstanceID)
    {
        UNICODE_STRING DevInstanceID;
        //RtlZeroMemory(&DevInstanceID,sizeof(DevInstanceID));
        RtlInitUnicodeString(&DevInstanceID, m_czDeviceInstanceID);
        DevInstanceID.MaximumLength = (MAX_PATH + 1) * sizeof(WCHAR);
        NTSTATUS    Status = STATUS_SUCCESS;

        Status	= PptRegGetDeviceParameterSZ(
            m_pKSDevice->PhysicalDeviceObject,
            L"DeviceInstanceID",
            &DevInstanceID);

        if (NT_SUCCESS (Status))
        {
            DbgPrint("Get Device Instance ID2 %S", m_czDeviceInstanceID);
			
	     if(m_czDeviceInstanceID[0])
	     	{
                   m_bGetDeviceInstanceID = true;
		 }
            else
		     DbgPrint("GetDeviceInstanceID is NULL,Registry isn't updated yet!\n");
        }
        else
        {
            m_czDeviceInstanceID[0] = '\0';
        }
    }

    return m_czDeviceInstanceID;
}


/*************************************************/


NTSTATUS
CDevice::
Pause (
    IN BOOLEAN Pausing
    )

/*++

Routine Description:

    Pause or unpause the hardware simulation.  This is an effective start
    or stop without resetting counters and formats.  Note that this can
    only be called to transition from started -> paused -> started.  Calling
    this without starting the hardware with Start() does nothing.

Arguments:

    Pausing -
        An indicatation of whether we are pausing or unpausing

        TRUE -
            Pause the hardware simulation

        FALSE -
            Unpause the hardware simulation

Return Value:

    Success / Failure

--*/

{

    PAGED_CODE();

    return
        m_HardwareSimulation -> Pause (
            Pausing
            );

}

/*************************************************/


NTSTATUS
CDevice::
Stop (
    )

/*++

Routine Description:

    Stop the capture device.

Arguments:

    None

Return Value:

    Success / Failure

--*/

{

    PAGED_CODE();
    NTSTATUS Status;

    Status = m_HardwareSimulation -> Stop ();

    m_TsSynth -> Stop();

    return Status;

}

/*************************************************/


ULONG
CDevice::
ProgramScatterGatherMappings (
    IN PUCHAR *Buffer,
    IN PKSMAPPING Mappings,
    IN ULONG MappingsCount
    )

/*++

Routine Description:

    Program the scatter / gather mappings for the "fake" hardware.

Arguments:

    Buffer -
        Points to a pointer to the virtual address of the topmost
        scatter / gather chunk.  The pointer will be updated as the
        device "programs" mappings.  Reason for this is that we get
        the physical addresses and sizes, but must calculate the virtual
        addresses...  This is used as scratch space for that.

    Mappings -
        An array of mappings to program

    MappingsCount -
        The count of mappings in the array

Return Value:

    The number of mappings successfully programmed

--*/

{

    PAGED_CODE();

    return 
        m_HardwareSimulation -> ProgramScatterGatherMappings (
            Buffer,
            Mappings,
            MappingsCount,
            sizeof (KSMAPPING)
            );

}


/*************************************************/


void
CDevice::
Interrupt (
    )

/*++

Routine Description:

    This is the "faked" interrupt service routine for this device.  It
    is called at dispatch level by the hardware simulation.

Arguments:

    None

Return Value:

    None

--*/

{

    m_InterruptTime++;

    //
    // Realistically, we'd do some hardware manipulation here and then queue
    // a DPC.  Since this is fake hardware, we do what's necessary here.  This
    // is pretty much what the DPC would look like short of the access
    // of hardware registers (ReadNumberOfMappingsCompleted) which would likely
    // be done in the ISR.
    //
    ULONG NumMappingsCompleted = 
        m_HardwareSimulation -> ReadNumberOfMappingsCompleted ();

    //
    // Inform the capture sink that a given number of scatter / gather
    // mappings have completed.
    //
    m_CaptureSink -> CompleteMappings (
        NumMappingsCompleted - m_LastMappingsCompleted
        );

    m_LastMappingsCompleted = NumMappingsCompleted;

#ifdef ISDBS_RECEIVER

    NTSTATUS ntStatus;
    CONST LONG lTSID = m_HardwareSimulation->GetCurrentTSID();
    while (0 <= lTSID && lTSID <= 0xFFFF && lTSID != m_lLatestTSID)
    {
        CFilter *       pFilter;
        LIST_ENTRY* pNext;
        KIRQL OldIrql;
        KeAcquireSpinLock(&m_Lock, &OldIrql);
        DbgPrint("[DEVICE] Acquiring Device Lock");
        pNext = m_FilterList.Flink;
        if( pNext == &m_FilterList)
        {
            DbgPrint("[DEVICE] No Filter  found");
//                         KeReleaseSpinLock(&pDevice->m_Lock, OldIrql);
            ntStatus = STATUS_UNSUCCESSFUL;
            break;
        }
        while (pNext != &m_FilterList)
        {    
            pFilter = CONTAINING_RECORD( pNext,CFilter, m_FilterListEntry);
            KIRQL               Irql;
            KeAcquireSpinLock( &pFilter->spinLock, &Irql);
//                         KeAcquireSpinLockAtDpcLevel(&pFilter->m_Lock);
        
            DbgPrint("[DEVICE] Throwing a new event on Filter %p", pFilter);
                            
            ASSERT( pFilter);
            DbgPrint("[DEVICE] Filter %p", pFilter);

            DbgPrint("[DEVICE] checking m_PendingEventEntry");
            if (pFilter->m_PendingEventEntry != NULL)
            {
                DbgPrint("[DEVICE] checking m_FilterEventsManager.IsEnabled()");
                if (pFilter->m_FilterEventsManager.IsEnabled() != FALSE)
                {                                
                    GUID usedEventGuid = EVENTID_BDA_TransprtStreamSelectorInfo;
                    BDA_TS_SELECTORINFO TSInfo;
                    TSInfo.bTSInfolength    = sizeof(TSInfo);
                    TSInfo.guidNetworkType  = ISDB_SATELLITE_TV_NETWORK_TYPE;
                    TSInfo.bTSIDCount       = 1;
                    TSInfo.usTSID[0]        = (WORD)lTSID;

                    PBDATUNER_EVENT pTunerEvent = AllocateTunerEvent(&usedEventGuid, TSInfo.bTSInfolength, (UCHAR*)&TSInfo);
        
                    if (pTunerEvent == NULL)
                    {
                        DbgPrint("[DEVICE] no event created");
                        ntStatus = STATUS_UNSUCCESSFUL;
                    }
                    else
                    {
                        DbgPrint("[DEVICE] created event %X-%X-%X-%X%X-%X%X%X%X%X%X len=%d", usedEventGuid.Data1, usedEventGuid.Data2, usedEventGuid.Data3, 
                                    usedEventGuid.Data4[0],usedEventGuid.Data4[1],usedEventGuid.Data4[2],usedEventGuid.Data4[3],usedEventGuid.Data4[4],usedEventGuid.Data4[5],usedEventGuid.Data4[6],usedEventGuid.Data4[7],
                                    TSInfo.bTSInfolength);
                        ntStatus = pFilter->m_FilterEventsManager.QueueEvent(pTunerEvent,TRUE);
                    }
                }
        
            } // if
            
//                         KeReleaseSpinLockFromDpcLevel(&pFilter->m_Lock);
            pNext = pNext->Flink;
            KeReleaseSpinLock( &pFilter->spinLock, Irql);
        }
        KeReleaseSpinLock(&m_Lock, OldIrql);
        ntStatus = STATUS_SUCCESS;
        m_lLatestTSID = lTSID;
        DbgPrint("[DEVICE] done");
        break; // exit while
    }
#endif

#ifdef NETOP_RECEIVER
    //check current Guide availability and if guide data arrive 
    //send events to the system. Only if GDDS is enabled send events to each filter instance
    BOOL bIsGuideAvailable = FALSE;
    if (m_TsSynth && (m_InterruptTime % 11 == 10)) //about every 10 sample (about 2 seconds) we check guide data
    {
        if (m_TsSynth->IsGuideDataAvailable() == TRUE)
        {
            bIsGuideAvailable = TRUE;
        }
    }
    
    if (bIsGuideAvailable == TRUE)
    {
        CFilter *       pFilter;
        LIST_ENTRY* pNext;
        KIRQL OldIrql;
        KeAcquireSpinLock(&m_Lock, &OldIrql);
        pNext = m_FilterList.Flink;
        while (pNext != NULL)
        {    
            pFilter = CONTAINING_RECORD( pNext,CFilter, m_FilterListEntry);
            KIRQL               Irql;
            KeAcquireSpinLock( &pFilter->spinLock, &Irql);        
                            
            if ( (pFilter->m_PendingEventEntry != NULL) && (pFilter->m_guideDataType == PBDA_GUIDE_DATATYPE) )
            {
                if (pFilter->m_FilterEventsManager.IsEnabled() != FALSE)
                {                                
                    DbgPrint("[DEVICE] Sending GuideDataAvailable Event On Filter %p", pFilter);
                    PBDATUNER_EVENT pTunerEvent = AllocateTunerEvent(&EVENTID_BDA_GuideDataAvailable, 0,NULL);
        
                    if (pTunerEvent)
                    {
                        pFilter->m_FilterEventsManager.QueueEvent(pTunerEvent,TRUE);
                    }
                }
        
            } 
            
            pNext = pNext->Flink;
            if (pNext == &m_FilterList) 
            {
                pNext = NULL;
            }
            KeReleaseSpinLock( &pFilter->spinLock, Irql);
        }// end while
        KeReleaseSpinLock(&m_Lock, OldIrql);
    }

#endif
}

STDMETHODIMP_(NTSTATUS)
CDevice::DeviceControlDispatch(
                        IN PDEVICE_OBJECT DeviceObject, //pointer to the device object
                        IN PIRP Irp //@parm currently processed IRP
                        )
{
    NTSTATUS                ntStatus = STATUS_SUCCESS;
    PIO_STACK_LOCATION      irpStack;
    ULONG                   ioControlCode;
    ULONG					ulInBufferlength;
    ULONG					ulOutBufferlength;
    PKSDEVICE				pKSDevice;
    CDevice *			    pDevice;
    BYTE *                  pDataBuffer;

    pKSDevice = KsGetDeviceForDeviceObject(DeviceObject);

    ASSERT( pKSDevice );
    pDevice = reinterpret_cast<CDevice *>(pKSDevice->Context);
    ASSERT( pDevice );

    // Get a pointer to the current location in the Irp. This is where
    // the function codes and parameters are located.

    irpStack = IoGetCurrentIrpStackLocation(Irp);


    ulInBufferlength = irpStack->Parameters.DeviceIoControl.InputBufferLength;
    ulOutBufferlength = irpStack->Parameters.DeviceIoControl.OutputBufferLength;



    switch(irpStack->MajorFunction)
    {
        case IRP_MJ_DEVICE_CONTROL:

            ioControlCode = FUNCTION_FROM_CTL_CODE(irpStack->Parameters.DeviceIoControl.IoControlCode);
            switch(ioControlCode)
            {
            case 801:  //SetData
                pDataBuffer = (BYTE *) Irp->AssociatedIrp.SystemBuffer;
                if (ulInBufferlength < 1)
                {					// not enough input data
                    ntStatus = STATUS_INVALID_BUFFER_SIZE;
                    break;
                }					
                if (ulOutBufferlength < 1)
                {					// not enough output data
                    ntStatus = STATUS_INVALID_BUFFER_SIZE;
                    break;
                }					
                Irp->IoStatus.Information = 0; //return number of bytes we want to return 
                ntStatus = STATUS_SUCCESS;
                break;
            case 802:  //GetData
                pDataBuffer = (BYTE *) Irp->AssociatedIrp.SystemBuffer;
                if (ulInBufferlength < 1)
                {					// not enough input data
                    ntStatus = STATUS_INVALID_BUFFER_SIZE;
                    break;
                }					
                if (ulOutBufferlength < 1)
                {					// not enough output data
                    ntStatus = STATUS_INVALID_BUFFER_SIZE;
                    break;
                }					
                Irp->IoStatus.Information = 0; //return number of bytes we return 
                ntStatus = STATUS_SUCCESS;
                break;
            case 803:
                {
//                     CFilter *       pFilter;
//                     PKSFILTER pKSFilter;
                    
//                     DbgPrint("[DEVICE] Throwing GPNVS Value Update event");
                    
//                     if (pDevice->m_pFilter)
//                     {
//                         pFilter = pDevice->m_pFilter;
//                     }
//                     else
//                     {
//                         ntStatus = STATUS_UNSUCCESSFUL;
//                         break;
//                     }
                    
//                     ASSERT( pFilter);
//                     DbgPrint("[DEVICE] Filter %p", pFilter);
                    
//                     KIRQL               Irql;
//                     KeAcquireSpinLock( &pFilter->spinLock, &Irql);

//                     DbgPrint("[DEVICE] checking m_PendingEventEntry");
//                     if (pFilter->m_PendingEventEntry == NULL)
//                     {
//                         ntStatus = STATUS_UNSUCCESSFUL;
//                         break;
//                     }
//                     DbgPrint("[DEVICE] m_PendingEventEntry is not NULL");
                    
//                     DbgPrint("[DEVICE] checking m_FilterEventsManager.IsEnabled()");
//                     if (pFilter->m_FilterEventsManager.IsEnabled() == FALSE)
//                     {
//                         ntStatus = STATUS_UNSUCCESSFUL;
//                         break;
//                     }
//                     DbgPrint("[DEVICE] m_FilterEventsManager.IsEnabled() is TRUE");

//                     DbgPrint("[DEVICE] ulInBufferlength %d", ulInBufferlength);
                    
//                     pDataBuffer = (BYTE *) Irp->AssociatedIrp.SystemBuffer;
//                     DbgPrint("[DEVICE] input buffer %s", (UCHAR*)pDataBuffer);
                    
// //                     PBDATUNER_EVENT pTunerEvent = AllocateTunerEvent(&BDAEVENT_FakeEvent, 0, NULL);
//                     PBDATUNER_EVENT pTunerEvent = AllocateTunerEvent(&EVENTID_BDA_GPNVValueUpdate, ulInBufferlength, (UCHAR*)pDataBuffer);
                    
//                     if (pTunerEvent == NULL)
//                     {
//                         DbgPrint("[DEVICE] no event created");

//                         ntStatus = STATUS_UNSUCCESSFUL;
//                         break;
//                     }
//                     DbgPrint("[DEVICE] created event");
                
//                     ntStatus = pFilter->m_FilterGPNVEventsManager.QueueEvent(pTunerEvent,TRUE);
// //                     ntStatus = pFilter->m_FilterEventsManager.QueueEvent(pTunerEvent,TRUE);

                    ntStatus = STATUS_SUCCESS;
                    DbgPrint("[DEVICE] done");
                    
//                     KeReleaseSpinLock( &pFilter->spinLock, Irql);

                    break;
                }
            case 804:
                {
                    DbgPrint("[DEVICE] Throwing GPNVS Value Update event");
                    CFilter *       pFilter;
                    LIST_ENTRY* pNext;
                    KIRQL OldIrql;
                    KeAcquireSpinLock(&pDevice->m_Lock, &OldIrql);
                    DbgPrint("[DEVICE] Acquiring Device Lock");
                    pNext = pDevice->m_FilterList.Flink;
                    if( pNext == &pDevice->m_FilterList)
                    {
                        DbgPrint("[DEVICE] No Filter  found");
//                         KeReleaseSpinLock(&pDevice->m_Lock, OldIrql);
                        ntStatus = STATUS_UNSUCCESSFUL;
                        break;
                    }
                    while (pNext != &pDevice->m_FilterList)
                    {    
                        pFilter = CONTAINING_RECORD( pNext,CFilter, m_FilterListEntry);
                        KIRQL               Irql;
                        KeAcquireSpinLock( &pFilter->spinLock, &Irql);
//                         KeAcquireSpinLockAtDpcLevel(&pFilter->m_Lock);
                    
                        DbgPrint("[DEVICE] Throwing ISDB CAS Response event on Filter %p", pFilter);
                                        
                        ASSERT( pFilter);
                        DbgPrint("[DEVICE] Filter %p", pFilter);
                    
                        DbgPrint("[DEVICE] checking m_PendingEventEntry");
                        if (pFilter->m_PendingEventEntry != NULL)
                        {
                            DbgPrint("[DEVICE] checking m_FilterEventsManager.IsEnabled()");
                            if (pFilter->m_FilterEventsManager.IsEnabled() != FALSE)
                            {
                                DbgPrint("[DEVICE] m_FilterEventsManager.IsEnabled() is TRUE");

                                DbgPrint("[DEVICE] ulInBufferlength %d", ulInBufferlength);
                    
                                pDataBuffer = (BYTE *) Irp->AssociatedIrp.SystemBuffer;
                                DbgPrint("[DEVICE] input buffer %s", (UCHAR*)pDataBuffer);
                    
                                //                     PBDATUNER_EVENT pTunerEvent = AllocateTunerEvent(&BDAEVENT_FakeEvent, 0, NULL);
                                PBDATUNER_EVENT pTunerEvent = AllocateTunerEvent(&EVENTID_BDA_GPNVValueUpdate, ulInBufferlength, (UCHAR*)pDataBuffer);
                    
                                if (pTunerEvent == NULL)
                                {
                                    DbgPrint("[DEVICE] no event created");
                                    
                                    ntStatus = STATUS_UNSUCCESSFUL;
                                }
                                else
                                {
                                    DbgPrint("[DEVICE] created event");
                                    
                                    ntStatus = pFilter->m_FilterGPNVEventsManager.QueueEvent(pTunerEvent,TRUE);
                                    //                     ntStatus = pFilter->m_FilterEventsManager.QueueEvent(pTunerEvent,TRUE);
                                }
                            } // if
                        } // if
                        
                        pNext = pNext->Flink;
                        KeReleaseSpinLock( &pFilter->spinLock, Irql);
                    }
                    KeReleaseSpinLock(&pDevice->m_Lock, OldIrql);
                    ntStatus = STATUS_SUCCESS;
                    DbgPrint("[DEVICE] done");
                    
                    break;
                }
            case 805:
                {
                    CFilter *       pFilter;
                    LIST_ENTRY* pNext;
                    KIRQL OldIrql;
                    KeAcquireSpinLock(&pDevice->m_Lock, &OldIrql);
                    DbgPrint("[DEVICE] Acquiring Device Lock");
                    pNext = pDevice->m_FilterList.Flink;
                    if( pNext == &pDevice->m_FilterList)
                    {
                        DbgPrint("[DEVICE] No Filter  found");
//                         KeReleaseSpinLock(&pDevice->m_Lock, OldIrql);
                        ntStatus = STATUS_UNSUCCESSFUL;
                        break;
                    }
                    while (pNext != &pDevice->m_FilterList)
                    {    
                        pFilter = CONTAINING_RECORD( pNext,CFilter, m_FilterListEntry);
                        KIRQL               Irql;
                        KeAcquireSpinLock( &pFilter->spinLock, &Irql);
//                         KeAcquireSpinLockAtDpcLevel(&pFilter->m_Lock);
                    
                        DbgPrint("[DEVICE] Throwing ISDB CAS Response event on Filter %p", pFilter);
                                        
                        ASSERT( pFilter);
                        DbgPrint("[DEVICE] Filter %p", pFilter);

                        DbgPrint("[DEVICE] checking m_PendingEventEntry");
                        if (pFilter->m_PendingEventEntry != NULL)
                        {
                            DbgPrint("[DEVICE] checking m_FilterEventsManager.IsEnabled()");
                            if (pFilter->m_FilterEventsManager.IsEnabled() != FALSE)
                            {
                                DbgPrint("[DEVICE] m_FilterEventsManager.IsEnabled() is TRUE");

                                DbgPrint("[DEVICE] ulInBufferlength %d", ulInBufferlength);
                                
                                BDA_ISDBCAS_RESPONSEDATA* pblah;
                                pblah = (BDA_ISDBCAS_RESPONSEDATA*)(BYTE*) Irp->AssociatedIrp.SystemBuffer;
                                
                                PBDATUNER_EVENT pTunerEvent = AllocateTunerEvent(&EVENTID_BDA_IsdbCASResponse, ulInBufferlength, (UCHAR*)pblah);
                    
                                if (pTunerEvent == NULL)
                                {
                                    DbgPrint("[DEVICE] no event created");
                                    ntStatus = STATUS_UNSUCCESSFUL;
                                }
                                else
                                {
                                    DbgPrint("[DEVICE] created event");
                                    ntStatus = pFilter->m_FilterEventsManager.QueueEvent(pTunerEvent,TRUE);
                                }
                            } // if
                    
                        } // if
                        
//                         KeReleaseSpinLockFromDpcLevel(&pFilter->m_Lock);
                        pNext = pNext->Flink;
                        KeReleaseSpinLock( &pFilter->spinLock, Irql);
                    }
                    KeReleaseSpinLock(&pDevice->m_Lock, OldIrql);
                    ntStatus = STATUS_SUCCESS;
                    DbgPrint("[DEVICE] done");

                    break;
                }
            default: 
                ntStatus = STATUS_NOT_SUPPORTED;
                break;
            }
            break;
        default:
            ntStatus = STATUS_NOT_SUPPORTED;
            break;
    }


    // Complete Irp
    if( ntStatus == STATUS_SUCCESS )
    {
        // Real return status set in Irp->IoStatus.Status
        Irp->IoStatus.Status = ntStatus;
        IoCompleteRequest( Irp, IO_NO_INCREMENT );
        ntStatus = STATUS_SUCCESS;
    }
    else
    {
        ntStatus = fpClassDispatchfunction( DeviceObject, Irp );
    }

    return ntStatus;
}



