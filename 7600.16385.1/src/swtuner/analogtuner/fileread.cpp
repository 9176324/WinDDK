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

#define DEFINE_REG_VAR
#include "common.h"
ULONG g_FileCompleteStatus = 0;
ULONG g_FileChangeIndex = (ULONG)-1;

/**************************************************************************
PAGEABLE CODE
**************************************************************************/

#ifdef ALLOC_PRAGMA
#pragma code_seg()
#endif // ALLOC_PRAGMA

#define VIDEO_READ_BUFFER_SIZE PS_SAMPLE_SIZE*10

extern PCWSTR  MpegFileName;
ULONG ReadBufferSize = VIDEO_READ_BUFFER_SIZE;


PReadVideoStreamFile VidReadStart(IN PDEVICE_OBJECT DeviceObject, CFileReader* cfr);
PReadVideoStreamFile VidReadStop(PReadVideoStreamFile ReadDataBlock);
VOID VidReadWorkItem(IN PDEVICE_OBJECT DeviceObject,IN PVOID Context);
ULONG VidGetCaptureData(__out_ecount(Length) PVOID Buffer,ULONG Length,PReadVideoStreamFile ReadDataBlock);

NTSTATUS
GetSzAttributeFromFrequency(IN DWORD dwFreq, IN WCHAR* pczDeviceInstanceID, IN __nullterminated LPWSTR AttributeName,  IN OUT PUNICODE_STRING AttributeValue);


CFileReader::CFileReader() {
    KeInitializeEvent(&m_FreqChangedEvent, SynchronizationEvent, FALSE);
    m_pczDeviceInstanceID = NULL;
}

CFileReader::CFileReader(PKSDEVICE Device, WCHAR* pczDeviceInstanceID, PUNICODE_STRING RegistryPath) {
    CEncoderDevice* ADevice;
    m_Device = Device;
    ADevice = reinterpret_cast <CEncoderDevice *> (Device -> Context);
    m_RegistryPath = RegistryPath;
    ADevice->GetFrequency(&m_ulFrequency);
    m_ulFrequency /= 1000;
    m_pczDeviceInstanceID = pczDeviceInstanceID;
    KeInitializeEvent(&m_FreqChangedEvent, SynchronizationEvent, FALSE);
}

void CFileReader::Start() {
    m_VideoStream = VidReadStart(m_Device->PhysicalDeviceObject, this);
}

void CFileReader::Stop() {
    m_VideoStream = VidReadStop(m_VideoStream);
}

ULONG CFileReader::Read(OUT PUCHAR buffer, ULONG sampleSize) {
    return VidGetCaptureData(buffer, sampleSize, m_VideoStream);
}

void CFileReader::SetFrequency(ULONG frequency) {
    m_ulFrequency = frequency / 1000;
}

ULONG CFileReader::GetFrequency() {
    return m_ulFrequency;
}

WCHAR* CFileReader::GetDeviceInstanceID() {
    return m_pczDeviceInstanceID;
}

BOOL CFileReader::SingalPresent()
{

    UNICODE_STRING StreamLocation;
    RtlZeroMemory(&StreamLocation, sizeof(StreamLocation));
    NTSTATUS Status = GetSzAttributeFromFrequency(m_ulFrequency, m_pczDeviceInstanceID, L"StreamLocation", &StreamLocation);
    if(NT_SUCCESS(Status))
    {
        DbgPrint("Signal Present(1): frency %d\n",m_ulFrequency);
        RtlFreeUnicodeString(&StreamLocation);
        return TRUE;
    }
    return FALSE;

}

PReadVideoStreamFile VidReadStart(IN PDEVICE_OBJECT DeviceObject, CFileReader* cfr)
{
    KIRQL             SpinIrql;
    PReadVideoStreamFile ReadDataBlock;
    NTSTATUS     WaitStatus;

    DbgPrint("MY-VidReadStart");

    //  First, allocate buffers
    ReadDataBlock = (PReadVideoStreamFile)ExAllocatePoolWithTag(NonPagedPool,
        sizeof(ReadVideoStreamFile), MS_SAMPLE_ANALOG_POOL_TAG);
    if (!ReadDataBlock) {
    }

    RtlZeroMemory(ReadDataBlock,
        sizeof(ReadVideoStreamFile));
    ReadDataBlock->ReadBuffers[0]=(CHAR *)ExAllocatePoolWithTag(NonPagedPool,
        ReadBufferSize, MS_SAMPLE_ANALOG_POOL_TAG);
    if (!ReadDataBlock->ReadBuffers[0]) {
    }

    ReadDataBlock->ReadBuffers[1]=(CHAR *)ExAllocatePoolWithTag(NonPagedPool,
        ReadBufferSize, MS_SAMPLE_ANALOG_POOL_TAG);
    if (!ReadDataBlock->ReadBuffers[1]) {
    }


    //   Now, get events and mutex 
    KeInitializeEvent(&(ReadDataBlock->ReadEvent),
        SynchronizationEvent,
        FALSE);

    KeInitializeEvent(&(ReadDataBlock->InitEvent),
        SynchronizationEvent,
        FALSE);

    ReadDataBlock->FileReader = cfr;

    KeInitializeSpinLock(&(ReadDataBlock->LockAccess));
    KeAcquireSpinLock(&(ReadDataBlock->LockAccess),
        &SpinIrql);
    ReadDataBlock->Tunerfrequency = cfr->GetFrequency();
    ReadDataBlock->ThreadItem=IoAllocateWorkItem(DeviceObject);

    KeReleaseSpinLock(&(ReadDataBlock->LockAccess),
        SpinIrql);

    if (ReadDataBlock->ThreadItem == NULL) {
        return NULL;
    }


    IoQueueWorkItem(ReadDataBlock->ThreadItem,
        VidReadWorkItem,
        CriticalWorkQueue,
        ReadDataBlock);

    WaitStatus = KeWaitForSingleObject(&(ReadDataBlock->InitEvent),
        Executive,
        KernelMode,
        FALSE,
        NULL);

    if (!NT_SUCCESS(WaitStatus)) {
        return NULL;
    }

    return ReadDataBlock;
}


PReadVideoStreamFile VidReadStop(PReadVideoStreamFile ReadDataBlock)
{
    KIRQL             SpinIrql;
    KeAcquireSpinLock(&(ReadDataBlock->LockAccess),
        &SpinIrql);

    BOOL bDoWait = TRUE;
    
    DbgPrint("VidReadStop");

    ReadDataBlock->StopRequest=TRUE;

    DbgPrint( "About to wait ReadDataBlock ThreadItem %p\n",ReadDataBlock->ThreadItem );
    if (ReadDataBlock->ThreadItem == NULL)
    {
        bDoWait = FALSE;
    }    

    KeReleaseSpinLock(&(ReadDataBlock->LockAccess),
        SpinIrql);
    // end

    KeSetEvent(&(ReadDataBlock->ReadEvent),  //Trigger stop
        0,
        FALSE);

    if (bDoWait)
    {
        DbgPrint( "MY-Waiting %p\n",ReadDataBlock->ThreadItem );

        KeWaitForSingleObject(&(ReadDataBlock->InitEvent), //Wait for stop
            Executive,
            KernelMode,
            FALSE,
            NULL);

    }

    ExFreePool(ReadDataBlock->ReadBuffers[0]);
    ExFreePool(ReadDataBlock->ReadBuffers[1]);
    ExFreePool(ReadDataBlock);
    return 0;
}

NTSTATUS 
GetSzAttributeFromFrequency(IN DWORD dwFreq, IN WCHAR* pczDeviceInstanceID, IN __nullterminated LPWSTR AttributeName,  IN OUT PUNICODE_STRING AttributeValue)
{
    // 1) Convert dword frequency to string.
    // 2) find matching key in registry
    // 3) read attribute value from registry.
    // 4) pass value back to caller.
    
    NTSTATUS Status = STATUS_SUCCESS;
    UNICODE_STRING KeyName;
    KeyName.Buffer = (LPWSTR)ExAllocatePoolWithTag(NonPagedPool,
                                              MAX_PATH+1, 
                                              MS_SAMPLE_ANALOG_POOL_TAG
                                              );
	if (NULL == KeyName.Buffer)
	{
        return STATUS_UNSUCCESSFUL;
	}

    KeyName.Length = MAX_PATH;
    KeyName.MaximumLength = MAX_PATH;

    UNICODE_STRING StubName;
    RtlInitUnicodeString(&StubName, L"\\Registry\\Machine\\System\\PSWTuner\\");
    UNICODE_STRING Seperator;
    RtlInitUnicodeString(&Seperator, L"\\");
    UNICODE_STRING Parameters;
    RtlInitUnicodeString(&Parameters, L"Device Parameters\\");
    UNICODE_STRING DevInstanceID;
    RtlInitUnicodeString(&DevInstanceID, pczDeviceInstanceID);

    //DbgPrint("Converted Frequency value %S\n", Freq.Buffer);

    RtlCopyUnicodeString(&KeyName, &StubName);
    
    if (!StubName.Length)
    {
        ExFreePoolWithTag(KeyName.Buffer, MS_SAMPLE_ANALOG_POOL_TAG);
        return STATUS_UNSUCCESSFUL;
    }
    
    if (STATUS_SUCCESS != RtlAppendUnicodeStringToString(&KeyName, &DevInstanceID))
    {
        ExFreePoolWithTag(KeyName.Buffer, MS_SAMPLE_ANALOG_POOL_TAG);
        return STATUS_UNSUCCESSFUL;
    }
    
    if (STATUS_SUCCESS != RtlAppendUnicodeStringToString(&KeyName, &Seperator))
    {
        ExFreePoolWithTag(KeyName.Buffer, MS_SAMPLE_ANALOG_POOL_TAG);
        return STATUS_UNSUCCESSFUL;
    }
    
    if (STATUS_SUCCESS != RtlAppendUnicodeStringToString(&KeyName, &Parameters))
    {
        ExFreePoolWithTag(KeyName.Buffer, MS_SAMPLE_ANALOG_POOL_TAG);
        return STATUS_UNSUCCESSFUL;
    }

    UNICODE_STRING Freq;
    Freq.Buffer = (LPWSTR)ExAllocatePoolWithTag(NonPagedPool,
                                              MAX_PATH+1, 
                                              MS_SAMPLE_ANALOG_POOL_TAG
                                              );
	if (NULL == Freq.Buffer)
	{
        ExFreePoolWithTag(KeyName.Buffer, MS_SAMPLE_ANALOG_POOL_TAG);
        return STATUS_UNSUCCESSFUL;
	}

    Freq.Length = MAX_PATH;
    Freq.MaximumLength = MAX_PATH;
    
    RtlZeroMemory(Freq.Buffer,  MAX_PATH+1);
    RtlIntegerToUnicodeString(dwFreq, 10, &Freq);

    //DbgPrint("Converted Frequency value %S\n", Freq.Buffer);

    if (STATUS_SUCCESS != RtlAppendUnicodeStringToString(&KeyName, &Freq))
    {
        ExFreePoolWithTag(Freq.Buffer, MS_SAMPLE_ANALOG_POOL_TAG);
        ExFreePoolWithTag(KeyName.Buffer, MS_SAMPLE_ANALOG_POOL_TAG);
        return STATUS_UNSUCCESSFUL;
    }

    DbgPrint("2 KeyName %S\n", KeyName.Buffer);
    DbgPrint("2 AttributeName %S\n",AttributeName);
   
    Status = PptRegGetSz(RTL_REGISTRY_ABSOLUTE, 
                        KeyName.Buffer,
                        AttributeName, 
                        AttributeValue);

    if (!NT_SUCCESS(Status))
    {
        UNICODE_STRING GenericKeyName;
        GenericKeyName.Buffer = (LPWSTR)ExAllocatePoolWithTag(NonPagedPool,
                                                  MAX_PATH+1, 
                                                  MS_SAMPLE_ANALOG_POOL_TAG
                                                  );
        if (NULL == GenericKeyName.Buffer)
        {
            if (KeyName.Buffer)
            {
                ExFreePoolWithTag(KeyName.Buffer, MS_SAMPLE_ANALOG_POOL_TAG);
            }
        
            if (Freq.Buffer)
            {
                ExFreePoolWithTag(Freq.Buffer, MS_SAMPLE_ANALOG_POOL_TAG);
            }
        
            return STATUS_UNSUCCESSFUL;
        }

        GenericKeyName.Length = MAX_PATH;
        GenericKeyName.MaximumLength = MAX_PATH;

        RtlCopyUnicodeString(&GenericKeyName, &StubName);
        
        if (!StubName.Length)
        {
            ExFreePoolWithTag(GenericKeyName.Buffer, MS_SAMPLE_ANALOG_POOL_TAG);
            ExFreePoolWithTag(Freq.Buffer, MS_SAMPLE_ANALOG_POOL_TAG);
            ExFreePoolWithTag(KeyName.Buffer, MS_SAMPLE_ANALOG_POOL_TAG);
            return STATUS_UNSUCCESSFUL;
        }

        //DbgPrint("Converted Frequency value %S\n", Freq.Buffer);
        RtlAppendUnicodeStringToString(&GenericKeyName, &Parameters);
        RtlAppendUnicodeStringToString(&GenericKeyName, &Freq);

        DbgPrint("GenericKeyName %S\n", GenericKeyName.Buffer);


       
        Status = PptRegGetSz(RTL_REGISTRY_ABSOLUTE, 
                            GenericKeyName.Buffer,
                            AttributeName, 
                            AttributeValue);
        if (!NT_SUCCESS(Status))
        {
            DbgPrint( "%s  Error reading the registry 0x%08x\n", __FUNCTION__, Status); 
            // There was an error, use the default filename.
            AttributeValue= 0;
        }
        
        ExFreePoolWithTag(GenericKeyName.Buffer, MS_SAMPLE_ANALOG_POOL_TAG);
        
        //DbgPrint("%S %ld\n", AttributeName, AttributeValue);
    }

    if (KeyName.Buffer)
    {
        ExFreePoolWithTag(KeyName.Buffer, MS_SAMPLE_ANALOG_POOL_TAG);
    }

    if (Freq.Buffer)
    {
        ExFreePoolWithTag(Freq.Buffer, MS_SAMPLE_ANALOG_POOL_TAG);
    }

    return Status;
}

NTSTATUS
GetDefaultStreamLocation(IN PDEVICE_OBJECT DeviceObject, IN WCHAR* pczDeviceInstanceID, OUT PUNICODE_STRING FileName, BOOLEAN *pfEncryptStream = NULL)
{
    // 1) Convert dword frequency to string.
    // 2) find matching key in registry
    // 3) read StreamLocation value from registry.
    // 4) pass key back to caller.
    
    NTSTATUS Status = STATUS_SUCCESS;

    UNREFERENCED_PARAMETER(pfEncryptStream);
    UNREFERENCED_PARAMETER(DeviceObject);

    UNICODE_STRING DefaultStreamKeyName;
    DefaultStreamKeyName.Buffer = (LPWSTR)ExAllocatePoolWithTag(NonPagedPool,
                                              MAX_PATH+1, 
                                              MS_SAMPLE_ANALOG_POOL_TAG
                                              );
	if (NULL == DefaultStreamKeyName.Buffer)
	{
        return STATUS_UNSUCCESSFUL;
	}

    DefaultStreamKeyName.Length = MAX_PATH;
    DefaultStreamKeyName.MaximumLength = MAX_PATH;

    UNICODE_STRING StubName;
    RtlInitUnicodeString(&StubName, L"\\Registry\\Machine\\System\\PSWTuner\\");
    UNICODE_STRING Parameters;
    RtlInitUnicodeString(&Parameters, L"\\Device Parameters");
    UNICODE_STRING DevInstanceID;
    RtlInitUnicodeString(&DevInstanceID, pczDeviceInstanceID);

    RtlCopyUnicodeString(&DefaultStreamKeyName, &StubName);
    RtlAppendUnicodeStringToString(&DefaultStreamKeyName, &DevInstanceID);
    RtlAppendUnicodeStringToString(&DefaultStreamKeyName, &Parameters);

    UNICODE_STRING StreamLocation;
    RtlZeroMemory(&StreamLocation, sizeof(StreamLocation));
    
    Status = PptRegGetSz(RTL_REGISTRY_ABSOLUTE, 
                        DefaultStreamKeyName.Buffer,
                        L"DefaultStreamLocation", 
                        &StreamLocation);
    if (!NT_SUCCESS(Status) || StreamLocation.Length == 0)
    {
        RtlInitUnicodeString(&StreamLocation, MpegFileName);
        RtlCopyUnicodeString(FileName, &StreamLocation);

        Status = STATUS_SUCCESS;
    }
    else
    {
        RtlCopyUnicodeString(FileName, &StreamLocation);
        RtlFreeUnicodeString(&StreamLocation);
    }
    
    DbgPrint("Stream Location %S\n", FileName->Buffer);

    ExFreePoolWithTag(DefaultStreamKeyName.Buffer, MS_SAMPLE_ANALOG_POOL_TAG);

    return Status;
}

bool g_bFrequencySignal = 0;

DWORD 
GetStreamLocationFromFrequency(IN PDEVICE_OBJECT DeviceObject, IN DWORD dwFreq, IN WCHAR* pczDeviceInstanceID, OUT PUNICODE_STRING FileName, BOOLEAN *pfEncryptStream = NULL)
{
    // 1) Convert dword frequency to string.
    // 2) find matching key in registry
    // 3) read StreamLocation value from registry.
    // 4) pass key back to caller.
    
    NTSTATUS Status = STATUS_SUCCESS;
    UNICODE_STRING StreamLocation;
    RtlZeroMemory(&StreamLocation, sizeof(StreamLocation));
    
    Status = GetSzAttributeFromFrequency(dwFreq, pczDeviceInstanceID, L"StreamLocation", &StreamLocation);
    if (!NT_SUCCESS(Status))
    {
        DbgPrint( "%s  Error reading the registry 0x%08x\n", __FUNCTION__, Status); 
        // There was an error, use the default stream filename.
        Status = GetDefaultStreamLocation(DeviceObject, pczDeviceInstanceID, FileName, pfEncryptStream);
        g_bFrequencySignal = false;
   } 
    else
    {
        g_bFrequencySignal = true;
        RtlCopyUnicodeString(FileName, &StreamLocation);
        RtlFreeUnicodeString(&StreamLocation);
    }


    return Status;
}

VOID VidReadWorkItem(IN PDEVICE_OBJECT DeviceObject,IN PVOID Context)
{

    OBJECT_ATTRIBUTES FileObject;
    UNICODE_STRING    FileName; 
    NTSTATUS          Status;
    BOOL              FirstPass=TRUE;
    KIRQL             SpinIrql;

    DbgPrint("VidReadWorkItem ");

    PReadVideoStreamFile ReadDataBlock= static_cast<PReadVideoStreamFile>( Context);

    ReadDataBlock->Tunerfrequency = ReadDataBlock->FileReader->GetFrequency();
    DWORD dwInitialFreq = ReadDataBlock->FileReader->GetFrequency();
    WCHAR* pczDevInstanceID = ReadDataBlock->FileReader->GetDeviceInstanceID();
  
    FileName.Buffer = (LPWSTR)ExAllocatePoolWithTag(NonPagedPool,
                                              MAX_PATH+1, 
                                              MS_SAMPLE_ANALOG_POOL_TAG
                                              );
                                              
    if (NULL == FileName.Buffer)
    {
        DbgPrint("ExAllocatePoolWithTag allocation failed\n");
        goto Exit;    
    }

    FileName.Length = MAX_PATH;
    FileName.MaximumLength = MAX_PATH;
    RtlZeroMemory(FileName.Buffer,  MAX_PATH+1);

    DbgPrint("dwInitialFreq %u", dwInitialFreq);

    if(dwInitialFreq)
    {
        GetStreamLocationFromFrequency(DeviceObject, dwInitialFreq, pczDevInstanceID, &FileName);
    }
    else
    {
        GetDefaultStreamLocation(DeviceObject, pczDevInstanceID, &FileName);
    }

    InitializeObjectAttributes(&FileObject,
        &FileName,
        OBJ_KERNEL_HANDLE,
        NULL,
        NULL);

    Status = ZwCreateFile(&(ReadDataBlock->VideoFile),
        FILE_READ_DATA | SYNCHRONIZE,
        &FileObject,
        &(ReadDataBlock->IoStatus),
        NULL,
        FILE_ATTRIBUTE_NORMAL,
        FILE_SHARE_READ,
        FILE_OPEN,
        FILE_SYNCHRONOUS_IO_NONALERT,
        NULL,
        0);

    if (!NT_SUCCESS(Status)) {
        DbgPrint( "VidReadWorkItem::ZwCreateFile Failed %08x\n", Status);
        goto Exit;    
    }

    //  Signal Event to fill both buffers
    Status = ZwReadFile(ReadDataBlock->VideoFile,
        NULL,
        NULL,
        NULL,
        &(ReadDataBlock->IoStatus),
        ReadDataBlock->ReadBuffers[0],
        VIDEO_READ_BUFFER_SIZE,
        NULL,
        NULL);

    if (!NT_SUCCESS(Status)) {
        DbgPrint( "VidReadWorkItem::ZwReadFile Failed\n");
        goto Exit;
    }

    // Set size from read
    KeAcquireSpinLock(&(ReadDataBlock->LockAccess),
        &SpinIrql);
    ReadDataBlock->ReadBufferFill[0]=(ULONG)ReadDataBlock->IoStatus.Information;
    ReadDataBlock->ReadCount++;
    KeReleaseSpinLock(&(ReadDataBlock->LockAccess),
        SpinIrql);

    ReadDataBlock->FileReader->SetReadStarted(TRUE);

    do {
        KeAcquireSpinLock(&(ReadDataBlock->LockAccess),
            &SpinIrql);
        ReadDataBlock->ReadIndex++;  // Switch Buffers

        if(ReadDataBlock->ReadIndex>1)
            ReadDataBlock->ReadIndex=0;

        // Lock buffer so only we can get to it
        if(FirstPass) 
        {
            // On initilization, let start() continue
            FirstPass=FALSE;
            Status = KeSetEvent(&(ReadDataBlock->InitEvent),
                0,
                FALSE);
            if (Status) {
            }
        }
        KeReleaseSpinLock(&(ReadDataBlock->LockAccess),
            SpinIrql);

        // Wait for buffer to be needed
        Status = KeWaitForSingleObject(&(ReadDataBlock->ReadEvent),
            Executive,
            KernelMode,
            FALSE,
            NULL);

        if (!NT_SUCCESS(Status)) {
            ReadDataBlock->FileReader->SetReadStarted(FALSE);
            DbgPrint( "VidReadWorkItem::KeWaitForSingleObject Failed\n");
            goto Exit;
        }

        if(ReadDataBlock->StopRequest)
        {  // Stop Code Here
            ReadDataBlock->FileReader->SetReadStarted(FALSE);
           Status = ZwClose(ReadDataBlock->VideoFile); // Close File
            if (!NT_SUCCESS(Status)) 
            {
                goto Exit;
            }

            goto Exit; //  bye bye thread
        }

        ReadDataBlock->Tunerfrequency = ReadDataBlock->FileReader->GetFrequency();
        ULONG dwCurrentFreq = ReadDataBlock->Tunerfrequency;

        if(!dwCurrentFreq)
            dwCurrentFreq = dwInitialFreq;

        // if the frequency is different, close the old file, and open the new file.
        if (dwInitialFreq != dwCurrentFreq)
        {
            Status = ZwClose(ReadDataBlock->VideoFile); // Close File
            if (!NT_SUCCESS(Status)) 
            {
                DbgPrint("MY-Couldn't close the old file 0x%08x", Status);
            }
            // Ensure the setting is updated.
            dwInitialFreq = dwCurrentFreq;

            DbgPrint("new Frequency %u", dwCurrentFreq);

            // now, open the new file.
            FileName.Length = MAX_PATH;
            FileName.MaximumLength = MAX_PATH;
            RtlZeroMemory(FileName.Buffer,  MAX_PATH+1);

            Status = GetStreamLocationFromFrequency(DeviceObject, dwCurrentFreq, pczDevInstanceID, &FileName);

            if (!NT_SUCCESS(Status))
            {
                DbgPrint( "VidReadWorkItem  Error Getting StreamLocation 0x%08x\n", Status); 
            }
            DbgPrint("New Filename %S", FileName.Buffer);

            InitializeObjectAttributes(&FileObject,
                &FileName,
                OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
                NULL,
                NULL);

            Status = ZwCreateFile(&(ReadDataBlock->VideoFile),
                FILE_READ_DATA | SYNCHRONIZE,
                &FileObject,
                &(ReadDataBlock->IoStatus),
                NULL,
                FILE_ATTRIBUTE_NORMAL,
                FILE_SHARE_READ,
                FILE_OPEN,
                FILE_SYNCHRONOUS_IO_NONALERT,
                NULL,
                0);

            if (!NT_SUCCESS(Status)) 
            {
                    DbgPrint("MY-Couldn't open the new file 0x%08x", Status);
	             // Signal the SetFrequency thread that the switch is done
	            CEncoderDevice *pDevice = reinterpret_cast <CEncoderDevice *> (ReadDataBlock->FileReader->GetDevice() -> Context);
	            pDevice->FileReaderFreqChanged();
	            PKEVENT pEvent = ReadDataBlock->FileReader->GetFreqChangedEvent();
	            if (pEvent != NULL)
	                KeSetEvent(pEvent, 0, FALSE);
		    goto Exit;
            }

            // Clear the internal buffer, because all the previous data is invalid now.
            KeAcquireSpinLock(&(ReadDataBlock->LockAccess), &SpinIrql);
            ReadDataBlock->ReadBufferFill[0] = 0;
            ReadDataBlock->ReadBufferFill[1] = 0;
            ReadDataBlock->ReadBufferGet[0] = 0;
            ReadDataBlock->ReadBufferGet[1] = 0;         
            KeReleaseSpinLock(&(ReadDataBlock->LockAccess), SpinIrql);

            // Signal the SetFrequency thread that the switch is done
            CEncoderDevice *pDevice = reinterpret_cast <CEncoderDevice *> (ReadDataBlock->FileReader->GetDevice() -> Context);
            pDevice->FileReaderFreqChanged();
            PKEVENT pEvent = ReadDataBlock->FileReader->GetFreqChangedEvent();
            if (pEvent != NULL)
                KeSetEvent(pEvent, 0, FALSE);
        }
        Status = ZwReadFile(ReadDataBlock->VideoFile,
            NULL,
            NULL,
            NULL,
            &(ReadDataBlock->IoStatus),
            ReadDataBlock->ReadBuffers[ReadDataBlock->ReadIndex],
            VIDEO_READ_BUFFER_SIZE,
            NULL,
            NULL);

        if (!NT_SUCCESS(Status)) 
        {
            //get file size
	     FILE_STANDARD_INFORMATION fsi;
            Status = ZwQueryInformationFile(ReadDataBlock->VideoFile, &(ReadDataBlock->IoStatus),&fsi, sizeof(FILE_STANDARD_INFORMATION), FileStandardInformation);
	     if(!NT_SUCCESS(Status))
            {
                     DbgPrint( "ZwQueryInformationFile query standard information Failed\n");
			goto Exit;        
            }
		 
            //FILE_POSITION_INFORMATION fpi;
	     FILE_POSITION_INFORMATION psi;
            Status = ZwQueryInformationFile(ReadDataBlock->VideoFile, &(ReadDataBlock->IoStatus),&psi, sizeof(FILE_POSITION_INFORMATION), FilePositionInformation);
	     if(!NT_SUCCESS(Status))
            {
                     DbgPrint( "ZwQueryInformationFile query file position Failed\n");
			goto Exit;        
            }

	     if(fsi.EndOfFile.QuadPart  != psi.CurrentByteOffset.QuadPart )
	     {
                 DbgPrint( "VidReadWorkItem::ZwReadFile line 646 Failed\n");
                 goto Exit;
	     }
	}

        // Set size from read
        KeAcquireSpinLock(&(ReadDataBlock->LockAccess),
            &SpinIrql);
        ReadDataBlock->ReadCount++;

        //DbgPrint("MY-VidReadWorkItem readcount: %u", ReadDataBlock->ReadCount);

        if (ReadDataBlock->IoStatus.Information<ReadBufferSize) 
        {
            if (g_FileChangeIndex == -1) g_FileChangeIndex = ReadDataBlock->ReadIndex;


            ULONG RewindReadSize=(ULONG)(ReadBufferSize-ReadDataBlock->IoStatus.Information);

            PUCHAR RewindBuffer=(PUCHAR) ReadDataBlock->ReadBuffers[ReadDataBlock->ReadIndex];
            IO_STATUS_BLOCK RewindCompletion;
            NTSTATUS RewindStatus;
            FILE_POSITION_INFORMATION Rewind;

            RtlZeroMemory(RewindBuffer,
                RewindReadSize);
            ReadDataBlock->IoStatus.Information=ReadBufferSize;
            KeReleaseSpinLock(&(ReadDataBlock->LockAccess),
                SpinIrql);

            RtlZeroMemory(&Rewind,
                sizeof(FILE_POSITION_INFORMATION));

            RewindStatus = ZwSetInformationFile(ReadDataBlock->VideoFile,
                &RewindCompletion,
                &Rewind,
                sizeof(FILE_POSITION_INFORMATION),
                FilePositionInformation);

            if (!NT_SUCCESS(RewindStatus)) 
            {
                DbgPrint( "VidReadWorkItem::ZwSetInformationFile Failed\n");
                goto Exit;
            }

            KeAcquireSpinLock(&(ReadDataBlock->LockAccess),
                &SpinIrql);
            ReadDataBlock->RewindCount++;
        }

        ReadDataBlock->ReadBufferFill[ReadDataBlock->ReadIndex] = 
            (ULONG)ReadDataBlock->IoStatus.Information;

        KeReleaseSpinLock(&(ReadDataBlock->LockAccess),
            SpinIrql);  

#pragma warning(push)
#pragma warning(disable:4127)
    } while ( TRUE );
#pragma warning (pop)
Exit:
    KeAcquireSpinLock(&(ReadDataBlock->LockAccess),
        &SpinIrql);
    IoFreeWorkItem(ReadDataBlock->ThreadItem); // Release Work Item
    ReadDataBlock->ThreadItem = NULL;
    KeReleaseSpinLock(&(ReadDataBlock->LockAccess),
        SpinIrql);  
    KeSetEvent(&(ReadDataBlock->InitEvent), 0,FALSE);

    if (FileName.Buffer)
    {
        ExFreePoolWithTag(FileName.Buffer, MS_SAMPLE_ANALOG_POOL_TAG);
    }

    g_bFrequencySignal = false;
    DbgPrint("Exit read work item");

    return;
}


ULONG VidGetCaptureData(__out_ecount(Length) PVOID Buffer,ULONG Length,PReadVideoStreamFile ReadDataBlock)
{

    ULONG ThisIndex;
    ULONG NextIndex;
    CHAR * GetBuffer;
    CHAR * PutBuffer;
    ULONG Length1;
    ULONG Length2;
    NTSTATUS Status;

    KeAcquireSpinLockAtDpcLevel(&(ReadDataBlock->LockAccess));

    ThisIndex = ReadDataBlock->CaptureIndex;

    if(ThisIndex == 0)
        NextIndex=1;
    else
        NextIndex=0;

    if (ThisIndex == g_FileChangeIndex) { 
        g_FileCompleteStatus = 1;
        g_FileChangeIndex = (UINT)-1;
    }

    Length1 = ReadDataBlock->ReadBufferFill[ThisIndex]-
        ReadDataBlock->ReadBufferGet[ThisIndex];

    //   Get not spanned across buffers
    if(Length1>=Length) {

        //  first get from buffer, start read to next
        if(!ReadDataBlock->ReadBufferGet[ThisIndex]) {  

            Status = KeSetEvent(&(ReadDataBlock->ReadEvent),
                0,
                FALSE);
            if (Status) {
                return 0;
            }
        }

        // Copy Buffer
        GetBuffer = ReadDataBlock->ReadBuffers[ThisIndex];
        GetBuffer=&GetBuffer[ReadDataBlock->ReadBufferGet[ThisIndex]];

        RtlCopyMemory(Buffer,
            GetBuffer,
            Length);

        //  Increment Get Offset
        ReadDataBlock->ReadBufferGet[ThisIndex]+=Length;

        //  Buffer to very end
        if(Length1==Length)  {

            ReadDataBlock->ReadBufferGet[ThisIndex]=0;  // Reset Get Offset
            ReadDataBlock->ReadBufferFill[ThisIndex]=0;  //  Reset Fill Bytes
            ReadDataBlock->CaptureIndex=NextIndex;      // Swap Buffers

        }

        ReadDataBlock->GetSuccess++;
        KeReleaseSpinLockFromDpcLevel(&(ReadDataBlock->LockAccess));

        return Length;
    }

    // Handle Spanned Gets

    //  Get bytes available in next buffer
    Length2=ReadDataBlock->ReadBufferFill[NextIndex]-
        ReadDataBlock->ReadBufferGet[NextIndex];

    //   Return zero if can't fully satisfy request
    if((Length1+Length2) < Length) {
        ReadDataBlock->GetFail++;
        KeReleaseSpinLockFromDpcLevel(&(ReadDataBlock->LockAccess));
        return 0;
    }

    GetBuffer = ReadDataBlock->ReadBuffers[ThisIndex];
    GetBuffer=&GetBuffer[ReadDataBlock->ReadBufferGet[ThisIndex]];
    PutBuffer=(CHAR *)Buffer;

    if(Length1) { // Should always be >0

        RtlCopyMemory(Buffer,
            GetBuffer,
            Length1);

        PutBuffer = &PutBuffer[Length1];  //  Bump Put Buffer 
        Length -= Length1;
    } 
      
    GetBuffer=ReadDataBlock->ReadBuffers[NextIndex];

    RtlCopyMemory(PutBuffer,
       GetBuffer,
       Length);

    ReadDataBlock->ReadBufferGet[ThisIndex]=0;  // Reset Get Offset on First Buffer    
    ReadDataBlock->ReadBufferFill[ThisIndex]=0;  //  Reset Fill Bytes 
    ReadDataBlock->CaptureIndex=NextIndex;      // Swap Buffers
    ReadDataBlock->ReadBufferGet[NextIndex]=Length;  //  Set Next Offset
    ReadDataBlock->CaptureIndex=NextIndex;      // Swap Buffers

    // Trigger Read Event
    Status = KeSetEvent(&(ReadDataBlock->ReadEvent),
        0,
        FALSE);
    if (Status) {
        // Do something
    }

    ReadDataBlock->GetSuccess++;
    KeReleaseSpinLockFromDpcLevel(&(ReadDataBlock->LockAccess));
    return Length;
}

