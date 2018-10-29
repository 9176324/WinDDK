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
#include "BDATuner.h"
ULONG g_FileCompleteStatus = 0;
ULONG g_FileChangeIndex = (ULONG)-1;

/**************************************************************************
    PAGEABLE CODE
**************************************************************************/
    
#ifdef ALLOC_PRAGMA
#pragma code_seg()
#endif // ALLOC_PRAGMA

PCWSTR  MpegFileName193250kHz=L"\\??\\C:\\mpeg\\LipSync4.ts"; //chan10
PCWSTR  MpegFileName199250kHz=L"\\??\\C:\\mpeg\\Ber192-3.trp"; //chan11
PCWSTR  MpegFileName205250kHz=L"\\??\\C:\\mpeg\\205250kHz.ts"; //chan12
PCWSTR  MpegFileName211250kHz=L"\\??\\C:\\mpeg\\211250kHz.ts"; //chan13

PCWSTR  MpegFileName537250kHz=L"\\??\\C:\\mpeg\\Ber192-3.trp"; //chan25
PCWSTR  MpegFileName615250kHz=L"\\??\\C:\\mpeg\\Sample_Type09_25M_pure.ts"; //chan38
PCWSTR  MpegFileName621250kHz=L"\\??\\C:\\mpeg\\Ch77dump.ts"; //chan39
//PCWSTR  MpegFileName943250kHz=L"\\??\\D:\\mpeg\\Ber192-3.trp"; //chan149

PCWSTR FREQUENCY_PIPE_NAME = L"\\??\\C:\\FrequencyToRead.txt";

ULONG ReadBufferSize = VIDEO_READ_BUFFER_SIZE;

PReadVideoStreamFile VidReadStart(IN PDEVICE_OBJECT DeviceObject, PCWSTR FileName,CFileReader* cfr);
PReadVideoStreamFile VidReadStop(PReadVideoStreamFile ReadDataBlock);
VOID VidReadWorkItem(IN PDEVICE_OBJECT DeviceObject,IN PVOID Context);
ULONG VidGetCaptureData(__out_ecount(Length) PVOID Buffer,ULONG Length,PReadVideoStreamFile ReadDataBlock, PBOOL pNewFile);

CFileReader::CFileReader() 
    : m_VideoStream(NULL)
{
    m_pFileName = MpegFileName193250kHz;
    KeInitializeEvent(&m_FreqChangedEvent, SynchronizationEvent, FALSE);
}

CFileReader::CFileReader(PKSDEVICE Device, WCHAR* pczDeviceInstanceID, PUNICODE_STRING RegistryPath) 
    : m_VideoStream(NULL)
{
  CDevice* ADevice;
  m_Device = Device;
  ADevice = reinterpret_cast <CDevice *> (Device -> Context);
  m_RegistryPath = RegistryPath;
  m_pFileName = MpegFileName193250kHz;
  m_ulFrequency = ADevice->GetFrequency();
  m_pczDeviceInstanceID = pczDeviceInstanceID;
    
  DbgPrint("Filename %S", m_pFileName);
  KeInitializeEvent(&m_FreqChangedEvent, SynchronizationEvent, FALSE);
}

void CFileReader::Start() {
  m_VideoStream = VidReadStart(m_Device->PhysicalDeviceObject, m_pFileName, this);
}

void CFileReader::Stop() {
  m_VideoStream = VidReadStop(m_VideoStream);
}

ULONG CFileReader::Read(OUT PUCHAR buffer, ULONG sampleSize, PBOOL pNewFile) {
  if(m_VideoStream)
     return VidGetCaptureData(buffer, sampleSize, m_VideoStream, pNewFile);
  return 0;
}

void CFileReader::SetFrequency(ULONG frequency) {
  DbgPrint("CFileReader::SetFrequency %ld\n", frequency);
  m_ulFrequency = frequency;
}

ULONG CFileReader::GetFrequency() {
  return m_ulFrequency;
}

WCHAR* CFileReader::GetDeviceInstanceID() {
  return m_pczDeviceInstanceID;
}
 
PReadVideoStreamFile VidReadStart(IN PDEVICE_OBJECT DeviceObject, PCWSTR FileName, CFileReader* cfr)
{
   KIRQL             SpinIrql;
   DbgPrint( "VidReadStart %S\n", FileName);

   PReadVideoStreamFile ReadDataBlock;
   NTSTATUS     WaitStatus;

    //  First, allocate buffers
    ReadDataBlock = (PReadVideoStreamFile)ExAllocatePoolWithTag(NonPagedPool,
      sizeof(ReadVideoStreamFile), MS_SAMPLE_TUNER_POOL_TAG);
    if (!ReadDataBlock) 
    {
       return NULL;        
    }

    RtlZeroMemory(ReadDataBlock,sizeof(ReadVideoStreamFile));

    ReadDataBlock->ReadBuffers[0]=(CHAR *)ExAllocatePoolWithTag(NonPagedPool,
      ReadBufferSize, MS_SAMPLE_TUNER_POOL_TAG);

    if (!ReadDataBlock->ReadBuffers[0]) 
    {
       ExFreePoolWithTag(ReadDataBlock, MS_SAMPLE_TUNER_POOL_TAG);
       return NULL;
    }

    ReadDataBlock->ReadBuffers[1]=(CHAR *)ExAllocatePoolWithTag(NonPagedPool,
      ReadBufferSize, MS_SAMPLE_TUNER_POOL_TAG);

    if (!ReadDataBlock->ReadBuffers[1]) 
    {
       ExFreePoolWithTag(ReadDataBlock->ReadBuffers[0], MS_SAMPLE_TUNER_POOL_TAG);
       ExFreePoolWithTag(ReadDataBlock, MS_SAMPLE_TUNER_POOL_TAG);
       return NULL;
    }

    //   Now, get events and mutex 
    KeInitializeEvent(&(ReadDataBlock->ReadEvent),
      SynchronizationEvent,FALSE);

    KeInitializeEvent(&(ReadDataBlock->InitEvent),
             SynchronizationEvent,FALSE);

    ReadDataBlock->FileReader = cfr;

    KeInitializeSpinLock(&(ReadDataBlock->LockAccess));
    KeAcquireSpinLock(&(ReadDataBlock->LockAccess), &SpinIrql);

    //     ReadDataBlock->CDevice = reinterpret_cast <CDevice *> (cfr->m_Device -> Context);
    ReadDataBlock->Tunerfrequency = cfr->GetFrequency();
    ReadDataBlock->pczDeviceInstanceID = cfr->GetDeviceInstanceID();

    // worker thread item for reads
    ReadDataBlock->ThreadItem=IoAllocateWorkItem(DeviceObject);

    KeReleaseSpinLock(&(ReadDataBlock->LockAccess), SpinIrql);

    if (ReadDataBlock->ThreadItem == NULL) 
    {
        ExFreePoolWithTag(ReadDataBlock->ReadBuffers[0], MS_SAMPLE_TUNER_POOL_TAG);
        ExFreePoolWithTag(ReadDataBlock->ReadBuffers[1], MS_SAMPLE_TUNER_POOL_TAG);
        ExFreePoolWithTag(ReadDataBlock, MS_SAMPLE_TUNER_POOL_TAG);
        return NULL;
    }

    size_t FileNameLength = 0;
    WaitStatus = RtlStringCbLengthW(FileName, 2147483647 * sizeof(WCHAR), &FileNameLength);
    if (!NT_SUCCESS(WaitStatus)) 
    {
        ExFreePoolWithTag(ReadDataBlock->ReadBuffers[0], MS_SAMPLE_TUNER_POOL_TAG);
        ExFreePoolWithTag(ReadDataBlock->ReadBuffers[1], MS_SAMPLE_TUNER_POOL_TAG);
        ExFreePoolWithTag(ReadDataBlock, MS_SAMPLE_TUNER_POOL_TAG);
        return NULL;
    }
    
    ReadDataBlock->FileName =(WCHAR *)ExAllocatePoolWithTag(NonPagedPool,
       FileNameLength+2, MS_SAMPLE_TUNER_POOL_TAG);

    RtlStringCbCopyW((WCHAR *)ReadDataBlock->FileName, FileNameLength+2, FileName);
    ReadDataBlock->fNewFile = TRUE;

   // worker thread item for reads
   // ReadDataBlock->ThreadItem=IoAllocateWorkItem(DeviceObject);

   IoQueueWorkItem(ReadDataBlock->ThreadItem,
      VidReadWorkItem,
      CriticalWorkQueue,
      ReadDataBlock);

   WaitStatus = KeWaitForSingleObject(&(ReadDataBlock->InitEvent),
      Executive,
      KernelMode,
      FALSE,
      NULL);

   if (!NT_SUCCESS(WaitStatus)) 
   {
        ExFreePoolWithTag(ReadDataBlock->ReadBuffers[0], MS_SAMPLE_TUNER_POOL_TAG);
        ExFreePoolWithTag(ReadDataBlock->ReadBuffers[1], MS_SAMPLE_TUNER_POOL_TAG);
        ExFreePoolWithTag(ReadDataBlock, MS_SAMPLE_TUNER_POOL_TAG);
        return NULL;
   }

   return ReadDataBlock;
}


PReadVideoStreamFile VidReadStop(PReadVideoStreamFile ReadDataBlock)
{
    DbgPrint( "VidReadStop\n");


   KIRQL             SpinIrql;
   NTSTATUS Status;
   BOOL bDoWait = TRUE;

   KeAcquireSpinLock(&(ReadDataBlock->LockAccess),
       &SpinIrql);

   ReadDataBlock->StopRequest=TRUE;

   DbgPrint( "About to wait ReadDataBlock ThreadItem %p\n",ReadDataBlock->ThreadItem );
   if (ReadDataBlock->ThreadItem == NULL)
   {
  
      bDoWait = FALSE;
   }    

   KeReleaseSpinLock(&(ReadDataBlock->LockAccess),
       SpinIrql);

   Status = KeSetEvent(&(ReadDataBlock->ReadEvent),  //Trigger stop
      0,
      FALSE);

   if (bDoWait == TRUE)
   {
       DbgPrint( "Waiting %p\n",ReadDataBlock->ThreadItem );
       Status = KeWaitForSingleObject(&(ReadDataBlock->InitEvent), //Wait for stop
               Executive,
               KernelMode,
               FALSE,
               NULL);
       if (!NT_SUCCESS(Status))
       {
           DbgPrint( "Waiting unsuccessful at VidReadStop" );
       }

   }
   ExFreePoolWithTag(ReadDataBlock->ReadBuffers[0], MS_SAMPLE_TUNER_POOL_TAG);
   ExFreePoolWithTag(ReadDataBlock->ReadBuffers[1], MS_SAMPLE_TUNER_POOL_TAG);
   ExFreePoolWithTag(ReadDataBlock, MS_SAMPLE_TUNER_POOL_TAG);
   return 0;
}

NTSTATUS GetDemodulatorTypeFromRegistry(IN OUT PUNICODE_STRING strDemodulatorType,PUNICODE_STRING DevInstanceID)
{
     NTSTATUS Status = STATUS_SUCCESS;
     WCHAR strKey[MAX_PATH+1]={L"\\Registry\\Machine\\System\\PSWTuner"};
     UNICODE_STRING    strBuffer;
     RtlInitUnicodeString(&strBuffer,strKey);
     strBuffer.MaximumLength = sizeof(WCHAR)*(MAX_PATH+1);
     DbgPrint("GetDemodulatorTypeFromRegistry::dev instance id len %d,buffer is %p\n",DevInstanceID->Length,DevInstanceID->Buffer);

     if(DevInstanceID && DevInstanceID->Buffer && DevInstanceID->Buffer[0])
     {
        DbgPrint("GetDemodulatorTypeFromRegistry::append dev instance id\n");

        UNICODE_STRING strTemp;
        RtlInitUnicodeString(&strTemp,L"\\");

        RtlAppendUnicodeStringToString(&strBuffer,&strTemp);              
        RtlAppendUnicodeStringToString(&strBuffer,DevInstanceID);
     }
     DbgPrint("register location is %S\n",strBuffer.Buffer);
     
     Status = PptRegGetSz(RTL_REGISTRY_ABSOLUTE
                    ,strBuffer.Buffer
                    , L"DevicePnpId"
                    ,strDemodulatorType
                    );
     DbgPrint("device pnp id is %S,len is %d,max len is %d\n",strDemodulatorType->Buffer,(int)strDemodulatorType->Length ,(int)strDemodulatorType->MaximumLength );
     if(!NT_SUCCESS(Status))
     {
          DbgPrint("get device pnp id  failed!\n");
          //UNICODE_STRING strDefault;
          // RtlInitUnicodeString(&strDefault, L"ms_swtatsc");
           //RtlCopyUnicodeString(strDemodulatorType,&strDefault);
           Status = STATUS_SUCCESS;
     }
     return Status;
}

NTSTATUS GetTSParamsFromRegistry(DWORD *pdwEITUsage,DWORD *pPCRDrift,DWORD *pdwTSPacketBlank, DWORD *pdwGuideCacheUsage)
{
    NTSTATUS Status = STATUS_SUCCESS;
    WCHAR strKey[MAX_PATH+1]={L"\\Registry\\Machine\\System\\PSWTuner"};
//    DbgPrint("GetDemodulatorTypeFromRegistry::dev instance id len %d,buffer is %d\n",DevInstanceID->Length,DevInstanceID->Buffer);
    UNICODE_STRING    strBuffer;
    RtlInitUnicodeString(&strBuffer,strKey);
    strBuffer.MaximumLength = sizeof(WCHAR)*(MAX_PATH+1);

    if (pdwEITUsage)
    {
        Status = PptRegGetDword(RTL_REGISTRY_ABSOLUTE, 
            strBuffer.Buffer,
            L"RestampEIT",
            pdwEITUsage);
        if(!NT_SUCCESS(Status))
        {
            DbgPrint("SWTuner use no EIT Restamping\n");
        }
        else
        {
            DbgPrint("SWTuner use EIT Restamping\n");
        }
    }
    if (pPCRDrift)
    {
        Status = PptRegGetDword(RTL_REGISTRY_ABSOLUTE, 
            strBuffer.Buffer,
            L"PCRDriftPPM",
            pPCRDrift);
        if(!NT_SUCCESS(Status))
        {
            DbgPrint("SWTuner use no PCR Drift\n");
            Status = STATUS_SUCCESS;
        }
        else
        {
            DbgPrint("SWTuner use no %d ppm PCR Drift\n",*pPCRDrift);
        }
    }
    if (pdwTSPacketBlank)
    {
        Status = PptRegGetDword(RTL_REGISTRY_ABSOLUTE, 
            strBuffer.Buffer,
            L"TSBlanking",
            pdwTSPacketBlank);
        if(!NT_SUCCESS(Status))
        {
            DbgPrint("SWTuner use no TS blanking at start and end\n");
            Status = STATUS_SUCCESS;
        }
        else
        {
            DbgPrint("SWTuner use TS-Blankingparam %d\n",*pdwTSPacketBlank);
        }
    }
    if (pdwGuideCacheUsage)
    {
        Status = PptRegGetDword(RTL_REGISTRY_ABSOLUTE, 
            strBuffer.Buffer,
            L"GuideCache",
            pdwGuideCacheUsage);
        if(!NT_SUCCESS(Status))
        {
            DbgPrint("SWTuner use default Guidecaching\n");
            Status = STATUS_SUCCESS;
        }
        else
        {
            DbgPrint("SWTuner use Guidecaching %d\n",*pdwGuideCacheUsage);
        }
    }
    return Status;
}


DWORD 
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
                                              MS_SAMPLE_TUNER_POOL_TAG
                                              );
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

    DbgPrint("KeyName %S\n", KeyName.Buffer);


    Status = PptRegGetSz(RTL_REGISTRY_ABSOLUTE, 
                        KeyName.Buffer,
                        AttributeName, 
                        AttributeValue);

    //DbgPrint("AttributeValue");
    //due to accuracy in SAT systems the target frequency is +/- 1Mhz (+/- 1000khz), 
    //and we have two digits for polarisation and DiseqC so +/- 100000 
    if ((!NT_SUCCESS(Status)) && (dwFreq > 20000000)) 
    {
        RtlCopyUnicodeString(&KeyName, &StubName);

        RtlAppendUnicodeStringToString(&KeyName, &DevInstanceID);
        RtlAppendUnicodeStringToString(&KeyName, &Seperator);
        RtlAppendUnicodeStringToString(&KeyName, &Parameters);
        RtlIntegerToUnicodeString(dwFreq + 100000, 10, &Freq);  //test +1Mhz

        RtlAppendUnicodeStringToString(&KeyName, &Freq);

        DbgPrint("KeyName %S\n", KeyName.Buffer);

        Status = PptRegGetSz(RTL_REGISTRY_ABSOLUTE, 
            KeyName.Buffer,
            AttributeName, 
            AttributeValue);
    }

    if ((!NT_SUCCESS(Status)) && (dwFreq > 20000000)) 
    {
        RtlCopyUnicodeString(&KeyName, &StubName);

        RtlAppendUnicodeStringToString(&KeyName, &DevInstanceID);
        RtlAppendUnicodeStringToString(&KeyName, &Seperator);
        RtlAppendUnicodeStringToString(&KeyName, &Parameters);
        RtlIntegerToUnicodeString(dwFreq - 100000, 10, &Freq);  //test -1Mhz

        RtlAppendUnicodeStringToString(&KeyName, &Freq);

        DbgPrint("KeyName %S\n", KeyName.Buffer);

        Status = PptRegGetSz(RTL_REGISTRY_ABSOLUTE, 
            KeyName.Buffer,
            AttributeName, 
            AttributeValue);
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
        //DbgPrint("%S %ld\n", AttributeName, AttributeValue);
        ExFreePoolWithTag(GenericKeyName.Buffer, MS_SAMPLE_TUNER_POOL_TAG);
        
    }
    ExFreePoolWithTag(Freq.Buffer, MS_SAMPLE_TUNER_POOL_TAG);
    ExFreePoolWithTag(KeyName.Buffer, MS_SAMPLE_TUNER_POOL_TAG);
    

    return Status;
}

DWORD 
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
                                              MS_SAMPLE_TUNER_POOL_TAG
                                              );
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
    if (!NT_SUCCESS(Status))
    {
        DbgPrint( "%s  Error reading the registry 0x%08x\n", __FUNCTION__, Status); 
        // There was an error, use the default filename.
        RtlInitUnicodeString(&StreamLocation, MpegFileName193250kHz);        
        Status = STATUS_SUCCESS;
    } 
    
    DbgPrint("Stream Location %S\n", StreamLocation.Buffer);

    if(StreamLocation.Length)
      RtlInitUnicodeString(FileName, StreamLocation.Buffer);
    else
#ifdef ATSC_RECEIVER
      RtlInitUnicodeString(FileName, MpegFileName193250kHz);
#else
      RtlInitUnicodeString(FileName, MpegFileName199250kHz);
#endif

    ExFreePoolWithTag(DefaultStreamKeyName.Buffer, MS_SAMPLE_TUNER_POOL_TAG);

    return Status;
}
DWORD 
GetStreamLocationFromFrequency(IN PDEVICE_OBJECT DeviceObject, IN DWORD dwFreq, IN WCHAR* pczDeviceInstanceID, OUT PUNICODE_STRING FileName, BOOLEAN *pfEncryptStream = NULL)
{
    // 1) Convert dword frequency to string.
    // 2) find matching key in registry
    // 3) read StreamLocation value from registry.
    // 4) pass key back to caller.
    
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

    UNICODE_STRING Parameters;
    RtlInitUnicodeString(&Parameters, L"\\Device Parameters\\");

    UNICODE_STRING DevInstanceID;
    RtlInitUnicodeString(&DevInstanceID, pczDeviceInstanceID);

    RtlCopyUnicodeString(&KeyName, &StubName);
    RtlAppendUnicodeStringToString(&KeyName, &DevInstanceID);
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

    DbgPrint("Converted Frequency value %S\n", Freq.Buffer);

    RtlAppendUnicodeStringToString(&KeyName, &Freq);

    DbgPrint("KeyName %S\n", KeyName.Buffer);


    UNICODE_STRING StreamLocation;
    RtlZeroMemory(&StreamLocation, sizeof(StreamLocation));
    
//    Status = PptRegGetSz(RTL_REGISTRY_ABSOLUTE, 
//                        KeyName.Buffer,
//                        L"StreamLocation", 
//                        &StreamLocation);

    Status = GetSzAttributeFromFrequency(dwFreq, pczDeviceInstanceID, L"StreamLocation", &StreamLocation);

    
    if (!NT_SUCCESS(Status))
    {
        DbgPrint( "%s  Error reading the registry 0x%08x\n", __FUNCTION__, Status); 
        // There was an error, use the default stream filename.
        Status = GetDefaultStreamLocation(DeviceObject, pczDeviceInstanceID, &StreamLocation, pfEncryptStream);
    } 

    DbgPrint("Stream Location %S\n", StreamLocation.Buffer);

    RtlInitUnicodeString(FileName, StreamLocation.Buffer);
    ExFreePoolWithTag(Freq.Buffer, MS_SAMPLE_TUNER_POOL_TAG);
    ExFreePoolWithTag(KeyName.Buffer, MS_SAMPLE_TUNER_POOL_TAG);

    return Status;
}
VOID VidReadWorkItem(IN PDEVICE_OBJECT DeviceObject,IN PVOID Context)
{
    DbgPrint( "VidReadWorkItem \n");

    CTsTimestamp stt;
    OBJECT_ATTRIBUTES FileObject;
    UNICODE_STRING    FileName; 
    NTSTATUS          Status;
    BOOL              FirstPass=TRUE;
    KIRQL             SpinIrql;
    HANDLE            hFileHandleEPG = NULL;

    PReadVideoStreamFile ReadDataBlock= static_cast<PReadVideoStreamFile>( Context);

    DbgPrint("Filename %S", ReadDataBlock->FileName);


    DbgPrint( "VidReadWorkItem  Check for change in named pipe and/or config file. \n");

    DWORD dwInitialFreq = ReadDataBlock->FileReader->GetFrequency();
    WCHAR* pczDevInstanceID = ReadDataBlock->FileReader->GetDeviceInstanceID();
    
    DWORD dwEITUsage = 0;//default no EIT restamp, 1= EIT restamping from the first detected TDT/TOT table, 2= every time the frequency change, 8= no restamp but EIT processing
    DWORD dwPCRDrift = 0;//default no PCR drift
    DWORD dwTSPacketBlank = 3;//default blank end=1 and start=2
    ReadDataBlock->FileReader->m_GuideCacheUsage = 0;
    GetTSParamsFromRegistry(&dwEITUsage,&dwPCRDrift,&dwTSPacketBlank,&ReadDataBlock->FileReader->m_GuideCacheUsage);

    if (ReadDataBlock->FileReader->OpenEPGStream(&hFileHandleEPG, dwInitialFreq, FALSE) != STATUS_SUCCESS)
    {
        hFileHandleEPG = NULL;
    }

    if(dwInitialFreq)
    {
       Status = GetStreamLocationFromFrequency(DeviceObject, dwInitialFreq, pczDevInstanceID, &FileName);
       if (!NT_SUCCESS(Status))
       {
           DbgPrint( "VidReadWorkItem InitialFreq unsuccessful" );
       }
    }
    else
    {
        GetDefaultStreamLocation(DeviceObject, pczDevInstanceID, &FileName);
    }

    InitializeObjectAttributes(&FileObject,
       &FileName,
       OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,    
       NULL,
       NULL);

    ReadDataBlock->VideoFile = NULL;

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

    stt.InitParams(dwEITUsage,dwPCRDrift,dwTSPacketBlank);

    stt.TsStreamProcess((BYTE *)ReadDataBlock->ReadBuffers[0],ReadDataBlock->ReadBufferFill[0]);

    ReadDataBlock->FileReader->FlushPBDASIBuffers(&stt, hFileHandleEPG);

    do {
        //DbgPrint( "VidReadWorkItem:: about to acquire spinlock2\n");
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
            if (!NT_SUCCESS(Status)) {
                DbgPrint("KeSetEvent ReadDataBlock->InitEvent failed\n");
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
            if (hFileHandleEPG)
            {
                Status = ZwClose(hFileHandleEPG);
                hFileHandleEPG = NULL;
            }
            if (ReadDataBlock->VideoFile)
            {
                Status = ZwClose(ReadDataBlock->VideoFile); // Close File
                ReadDataBlock->VideoFile = NULL;
            }
            if (!NT_SUCCESS(Status)) 
            {
                goto Exit;
            }

            goto Exit; //  bye bye thread
        }

        // Check the current frequency value and change filenames if needed.
        // if the frequency is different, close the old file, and open the new file.
        DWORD dwCurFrequency = ReadDataBlock->FileReader->GetFrequency();
        WCHAR* pczDeviceInstanceID = ReadDataBlock->FileReader->GetDeviceInstanceID();
        if (dwInitialFreq != dwCurFrequency)
        {
            DbgPrint("freqency changed o %d, new is %d\n",dwInitialFreq,dwCurFrequency);
            if (hFileHandleEPG && (!ReadDataBlock->FileReader->m_GuideCacheUsage))
            {
                Status = ZwClose(hFileHandleEPG);
                hFileHandleEPG = NULL;
            }
            if (ReadDataBlock->VideoFile)
            {
            Status = ZwClose(ReadDataBlock->VideoFile); // Close File
                ReadDataBlock->VideoFile = NULL;
            }
            if (!NT_SUCCESS(Status)) 
            {
                DbgPrint("Couldn't close the old file 0x%08x", Status);
            }
            // Ensure the setting is updated.
            dwInitialFreq = dwCurFrequency;

            if (!ReadDataBlock->FileReader->m_GuideCacheUsage)
            {
                //open the new EPG file, once the frequency change
                if (ReadDataBlock->FileReader->OpenEPGStream(&hFileHandleEPG, dwInitialFreq, FALSE) != STATUS_SUCCESS)
                {
                    hFileHandleEPG = NULL;
                }
            }

            // now, open the new file.
            UNICODE_STRING _pFileName;
            Status = GetStreamLocationFromFrequency(DeviceObject, dwCurFrequency, pczDeviceInstanceID, &_pFileName);
            if (!NT_SUCCESS(Status))
            {
                DbgPrint( "VidReadWorkItem  Error Getting StreamLocation 0x%08x\n", Status); 
            }

            DbgPrint("New Filename %S", _pFileName.Buffer);

            InitializeObjectAttributes(&FileObject,
                       &_pFileName,
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
                DbgPrint("Couldn't open the new file 0x%08x", Status);
                goto Exit;
            }

            // Clear the internal buffer, because all the previous data is invalid now.
            KeAcquireSpinLock(&(ReadDataBlock->LockAccess), &SpinIrql);
            ReadDataBlock->ReadBufferFill[0] = 0;
            ReadDataBlock->ReadBufferFill[1] = 0;
            ReadDataBlock->ReadBufferGet[0] = 0;
            ReadDataBlock->ReadBufferGet[1] = 0;
            ReadDataBlock->fNewFile = TRUE;
            KeReleaseSpinLock(&(ReadDataBlock->LockAccess), SpinIrql);
            
            // Signal the SetFrequency thread that the switch is done
            CDevice *pDevice = reinterpret_cast <CDevice *> (ReadDataBlock->FileReader->GetDevice() -> Context);
            pDevice->FileReaderFreqChanged();
            PKEVENT pEvent = ReadDataBlock->FileReader->GetFreqChangedEvent();
            if (pEvent != NULL)
                KeSetEvent(pEvent, 0, FALSE);
        
            stt.Reset();
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
        stt.TsStreamProcess((BYTE *)ReadDataBlock->ReadBuffers[ReadDataBlock->ReadIndex],ReadDataBlock->IoStatus.Information);

        ReadDataBlock->FileReader->FlushPBDASIBuffers(&stt, hFileHandleEPG);

        //Dump the first ten bytes of the packet to verify data is coming from the driver.
        //DbgPrint( "VidReadWorkItem::First 10 bytes %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",
        //            ReadDataBlock->ReadBuffers[ReadDataBlock->ReadIndex][0],
        //            ReadDataBlock->ReadBuffers[ReadDataBlock->ReadIndex][1],
        //            ReadDataBlock->ReadBuffers[ReadDataBlock->ReadIndex][2],
        //            ReadDataBlock->ReadBuffers[ReadDataBlock->ReadIndex][3],
        //            ReadDataBlock->ReadBuffers[ReadDataBlock->ReadIndex][4],
        //            ReadDataBlock->ReadBuffers[ReadDataBlock->ReadIndex][5],
        //            ReadDataBlock->ReadBuffers[ReadDataBlock->ReadIndex][6],
        //            ReadDataBlock->ReadBuffers[ReadDataBlock->ReadIndex][7],
        //            ReadDataBlock->ReadBuffers[ReadDataBlock->ReadIndex][8],
        //            ReadDataBlock->ReadBuffers[ReadDataBlock->ReadIndex][9]
        //            );  

        // Set size from read
        //DbgPrint( "VidReadWorkItem::About to acquire spinlock 597\n");
        KeAcquireSpinLock(&(ReadDataBlock->LockAccess),
           &SpinIrql);
        ReadDataBlock->ReadCount++;
        
        if (ReadDataBlock->IoStatus.Information<ReadBufferSize) 
        {
            if (g_FileChangeIndex == -1) g_FileChangeIndex = ReadDataBlock->ReadIndex;

            ULONG RewindReadSize=(ULONG)(ReadBufferSize-ReadDataBlock->IoStatus.Information);
            PUCHAR RewindBuffer=(PUCHAR) ReadDataBlock->ReadBuffers[ReadDataBlock->ReadIndex];
            IO_STATUS_BLOCK RewindCompletion;
            NTSTATUS RewindStatus;
            FILE_POSITION_INFORMATION Rewind;

            RtlZeroMemory(RewindBuffer, RewindReadSize);
            ReadDataBlock->IoStatus.Information=ReadBufferSize;
            KeReleaseSpinLock(&(ReadDataBlock->LockAccess),
              SpinIrql);

            //DbgPrint( "VidReadWorkItem::released spinlock 619 \n");
            
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
            stt.UpdateCurrentTimestampBase();
            stt.DbgCurrentTimestamp();
            DbgPrint( "VidReadWorkItem::rewind \n");
        }

        ReadDataBlock->ReadBufferFill[ReadDataBlock->ReadIndex] = 
           (ULONG)ReadDataBlock->IoStatus.Information;

        KeReleaseSpinLock(&(ReadDataBlock->LockAccess),
          SpinIrql);  
        //DbgPrint( "VidReadWorkItem::read count is  %d\n", ReadDataBlock->ReadCount);
#pragma warning(disable:4127) // error C4127: conditional expression is constant
    } while ( TRUE );

    Exit:
    DbgPrint("88 Read thread!\n");
    if (hFileHandleEPG)
    {
        Status = ZwClose(hFileHandleEPG);
        hFileHandleEPG = NULL;
    }
    if (ReadDataBlock->VideoFile)
    {
        Status = ZwClose(ReadDataBlock->VideoFile); // Close File
        ReadDataBlock->VideoFile = NULL;
    }
    KeAcquireSpinLock(&(ReadDataBlock->LockAccess),
       &SpinIrql);
    IoFreeWorkItem(ReadDataBlock->ThreadItem); // Release Work Item
    ReadDataBlock->ThreadItem = NULL;
    KeReleaseSpinLock(&(ReadDataBlock->LockAccess),
      SpinIrql);  
    Status = KeSetEvent(&(ReadDataBlock->InitEvent), 0,FALSE);
    return;
}


typedef struct _NAMED_PIPE_CREATE_PARAMETERS {
    ULONG NamedPipeType;
    ULONG ReadMode;
    ULONG CompletionMode;
    ULONG MaximumInstances;
    ULONG InboundQuota;
    ULONG OutboundQuota;
    LARGE_INTEGER DefaultTimeout;
    BOOLEAN TimeoutSpecified;
} NAMED_PIPE_CREATE_PARAMETERS, *PNAMED_PIPE_CREATE_PARAMETERS;

NTSTATUS  NtCreateNamedPipeFile (
            OUT PHANDLE FileHandle,
            IN ULONG DesiredAccess,
            IN POBJECT_ATTRIBUTES ObjectAttributes,
            OUT PIO_STATUS_BLOCK IoStatusBlock,
            IN ULONG ShareAccess,
            IN ULONG CreateDisposition,
            IN ULONG CreateOptions,
            IN ULONG NamedPipeType,
            IN ULONG ReadMode,
            IN ULONG CompletionMode,
            IN ULONG MaximumInstances,
            IN ULONG InboundQuota,
            IN ULONG OutboundQuota,
            IN PLARGE_INTEGER DefaultTimeout OPTIONAL 
            )  
{   
    NAMED_PIPE_CREATE_PARAMETERS NamedPipeParms;
    NTSTATUS Status;
    __try   
    {   
        if ( DefaultTimeout )
        {
            NamedPipeParms.TimeoutSpecified = TRUE;
            NamedPipeParms.DefaultTimeout.QuadPart = DefaultTimeout->QuadPart;
        }
        else
        {   
            NamedPipeParms.TimeoutSpecified = FALSE;
        } 
          
        NamedPipeParms.NamedPipeType = NamedPipeType;
        NamedPipeParms.ReadMode = ReadMode;
        NamedPipeParms.CompletionMode = CompletionMode;
        NamedPipeParms.MaximumInstances = MaximumInstances;
        NamedPipeParms.InboundQuota = InboundQuota;
        NamedPipeParms.OutboundQuota = OutboundQuota;
        Status = IoCreateFile (
                    FileHandle,
                    DesiredAccess,
                    ObjectAttributes,
                    IoStatusBlock,
                    NULL,
                    0,
                    ShareAccess,
                    CreateDisposition,
                    CreateOptions,
                    NULL,
                    0,
                    CreateFileTypeNamedPipe,
                    &NamedPipeParms,
                    0   
                    );

        return Status;
    }   
    __except (EXCEPTION_EXECUTE_HANDLER)   
    {
       KdPrint (("NtCreateNamedPipeFile: Exception occured.\n"));
       return STATUS_UNSUCCESSFUL;   
    }
}    


ULONG VidGetCaptureData(__out_ecount(Length) PVOID Buffer,ULONG Length,PReadVideoStreamFile ReadDataBlock, PBOOL pNewFile)
{

   ULONG ThisIndex;
   ULONG NextIndex;
   CHAR * GetBuffer;
   CHAR * PutBuffer;
   ULONG Length1;
   ULONG Length2;
   NTSTATUS Status;


   KeAcquireSpinLockAtDpcLevel(&(ReadDataBlock->LockAccess));
   //DbgPrint("VidGetCaptureData\n");

   // New file is started.
   if (pNewFile)
       *pNewFile = ReadDataBlock->fNewFile;
   ReadDataBlock->fNewFile = FALSE;

   ThisIndex = ReadDataBlock->CaptureIndex;
  
   if(ThisIndex == 0)
      NextIndex=1;
   else
      NextIndex=0;

   if (ThisIndex == g_FileChangeIndex) { 
       g_FileCompleteStatus = 1;
       g_FileChangeIndex = (ULONG)-1;
   }

   Length1 = ReadDataBlock->ReadBufferFill[ThisIndex]-
      ReadDataBlock->ReadBufferGet[ThisIndex];

   //   Get not spanned across buffers
   if(Length1>=Length) {

        //  first get from buffer, start read to next
        if(!ReadDataBlock->ReadBufferGet[ThisIndex]) 
        {           
             Status = KeSetEvent(&(ReadDataBlock->ReadEvent),
                0,
                FALSE);
            // DbgPrint("about toKeSetEvent ReadDataBlock->ReadEvent\n");
             
            if (Status) {
                DbgPrint("KeSetEvent ReadDataBlock->ReadEvent failed!\n");    
                return 0;
            }
        }
        //DbgPrint("VidGetCaptureData begin to copy data!\n");

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
    DbgPrint("about toKeSetEvent ReadDataBlock->ReadEvent 2\n");

    if (Status) {
        DbgPrint("about toKeSetEvent ReadDataBlock->ReadEvent 2 failed!\n");
        // Do something
    }

    ReadDataBlock->GetSuccess++;
    KeReleaseSpinLockFromDpcLevel(&(ReadDataBlock->LockAccess));
    return Length;
}

void CFileReader::FlushPBDASIBuffers(CTsTimestamp *pctts, HANDLE hFileHandlePBDASI)
{
    //read PBDA SI converted data that is current available and copy it into a binary Guide file named "Freq[DevInstance].pbdasi" next to the transport stream file
    //Or copy it into a local memory within ReadDataBlock
    //We need also the statistic values to determine roughly the amount of data we have for all the epg data within this transport stream 
    NTSTATUS Status = STATUS_SUCCESS;
    IO_STATUS_BLOCK IoStatus;
    PBDA_SI_BUFFER *pPBDABuffer=NULL;

    if ((!pctts) || (hFileHandlePBDASI == NULL) )
    {
        return;
    }

    ULONG ulSavedSections = 0;//count the stored sections

    while ((pctts->GetPBDASIGuideData(&pPBDABuffer) == true) && (Status == STATUS_SUCCESS))
    {
        if (pPBDABuffer == NULL) break;
        //DbgPrint("Write %d PBDA-SI Data into file\n",pPBDABuffer->ulDataLength);
        
        ulSavedSections++;
        //append the length
        Status = ZwWriteFile(hFileHandlePBDASI,
                  NULL,
                  NULL,
                  NULL,
                  &IoStatus,
                  &pPBDABuffer->ulDataLength,
                  sizeof(ULONG),
                  NULL,
                  NULL);

        //append the PBDA-SI Section
        if (Status == STATUS_SUCCESS)
        {
            Status = ZwWriteFile(hFileHandlePBDASI,
                  NULL,
                  NULL,
                  NULL,
                  &IoStatus,
                  pPBDABuffer->argbData,
                  pPBDABuffer->ulDataLength,
                  NULL,
                  NULL);

        }
        pctts->ReleasePBDASIData(pPBDABuffer);
    }

    ULONG ulNrOfCachedSections = 0; 
    ULONG ulTotalSections = 0;

    pctts->GetNrOfTables(&ulNrOfCachedSections, &ulTotalSections);

    ULONG ulPreviousSavedSections;
    ULONG ulPreviousTotalSections;
    GetCurrentEPGStatistic(&ulPreviousSavedSections, &ulPreviousTotalSections);
    SetCurrentEPGStatistic(ulSavedSections + ulPreviousSavedSections, ulTotalSections);
    if (ulSavedSections)
    {
        DbgPrint("Write %d of %d PBDA-SI Sections into file\n",ulSavedSections + ulPreviousSavedSections,ulTotalSections);
    }
}

NTSTATUS CFileReader::OpenEPGStream(HANDLE *pFileHandlePBDASI, DWORD dwInitialFreq, BOOL bReadAccess)
{
    NTSTATUS Status = STATUS_SUCCESS;
    UNICODE_STRING FileName;
    FileName.Buffer = (LPWSTR)ExAllocatePoolWithTag(NonPagedPool,
                                              MAX_PATH+1, 
                                              MS_SAMPLE_TUNER_POOL_TAG
                                              );
    FileName.Length = MAX_PATH;
    FileName.MaximumLength = MAX_PATH;

    UNICODE_STRING DefaultFilePath;
    RtlInitUnicodeString(&DefaultFilePath, L"\\??\\C:\\swtuner\\");

    UNICODE_STRING DefaultGuideFolder;
    RtlInitUnicodeString(&DefaultGuideFolder, L"GuideCache\\");

    UNICODE_STRING ExtensionName;
    RtlInitUnicodeString(&ExtensionName, L".pbdasi");


    UNICODE_STRING DevInstanceID;
    RtlInitUnicodeString(&DevInstanceID, m_pczDeviceInstanceID);

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
        RtlCopyUnicodeString(&FileName, &DefaultFilePath);
    }

    RtlAppendUnicodeStringToString(&FileName, &DefaultGuideFolder);
    RtlAppendUnicodeStringToString(&FileName, &DevInstanceID);

    UNICODE_STRING Freq;
    Freq.Buffer = (LPWSTR)ExAllocatePoolWithTag(NonPagedPool,
                                              MAX_PATH+1, 
                                              MS_SAMPLE_TUNER_POOL_TAG
                                              );
    Freq.Length = MAX_PATH;
    Freq.MaximumLength = MAX_PATH;
    
    RtlZeroMemory(Freq.Buffer,  MAX_PATH+1);
    RtlIntegerToUnicodeString(dwInitialFreq, 10, &Freq);

    if (!m_GuideCacheUsage)
    {
        RtlAppendUnicodeStringToString(&FileName, &Freq);
    }

    ExFreePoolWithTag(Freq.Buffer, MS_SAMPLE_TUNER_POOL_TAG);

    RtlAppendUnicodeStringToString(&FileName, &ExtensionName);

    OBJECT_ATTRIBUTES FileObject;
    IO_STATUS_BLOCK IoStatus;  

    InitializeObjectAttributes(&FileObject,
       &FileName,
       OBJ_KERNEL_HANDLE,    
       NULL,
       NULL);

    if (bReadAccess == TRUE)
    {
        Status = ZwCreateFile(pFileHandlePBDASI,
           FILE_READ_DATA | SYNCHRONIZE,
           &FileObject,
           &IoStatus,
           NULL,
           FILE_ATTRIBUTE_NORMAL,
           FILE_SHARE_WRITE | FILE_SHARE_READ,
           FILE_OPEN,
           FILE_SYNCHRONOUS_IO_NONALERT,
           NULL,
           0);
    }
    else //WriteAccess
    {
        ULONG ulShareAccess = FILE_OVERWRITE_IF;//default to overwrite every time we change the channel
        if (m_GuideCacheUsage)
        {
            ulShareAccess = FILE_OPEN_IF;//just open or create the file 
        }

        Status = ZwCreateFile(pFileHandlePBDASI,
           FILE_WRITE_DATA | SYNCHRONIZE | FILE_APPEND_DATA,
           &FileObject,
           &IoStatus,
           NULL,
           FILE_ATTRIBUTE_NORMAL,
           FILE_SHARE_WRITE | FILE_SHARE_READ,
           ulShareAccess,//access depend on the mode
           FILE_SYNCHRONOUS_IO_NONALERT,
           NULL,
           0);

        SetCurrentEPGStatistic(0,0);
    }
        
    ExFreePoolWithTag(FileName.Buffer, MS_SAMPLE_TUNER_POOL_TAG);

    return Status;

}

//return the swtuner installer directory 
NTSTATUS CFileReader::GetSwtunerInstallPath(OUT PUNICODE_STRING FilePath) 
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
    RtlInitUnicodeString(&DevInstanceID, m_pczDeviceInstanceID);


    RtlCopyUnicodeString(&KeyName, &StubName);
    RtlAppendUnicodeStringToString(&KeyName, &DevInstanceID);


    Status = PptRegGetSz(RTL_REGISTRY_ABSOLUTE, 
                        KeyName.Buffer,
                        L"MCMLFilePath", 
                        FilePath);


    ExFreePoolWithTag(KeyName.Buffer , MS_SAMPLE_TUNER_POOL_TAG);
    return Status;
}

NTSTATUS CFileReader::ReadNextPBDASIBufferFromFile(HANDLE hFileHandlePBDASI, ULONG *pulReadLength, BYTE *pPBDASection)
{
    IO_STATUS_BLOCK IoStatus;  
    NTSTATUS Status = STATUS_SUCCESS;
    ULONG ulLength=0; 

    if ((!pulReadLength) || (!pPBDASection))
    {
        return STATUS_UNSUCCESSFUL;
    }

    Status = ZwReadFile(hFileHandlePBDASI,
      NULL,
      NULL,
      NULL,
      &IoStatus,
      &ulLength,
      sizeof(ULONG),
      NULL,
      NULL);

    if ((Status == STATUS_SUCCESS) && (ulLength <= 8192))//maximum allowed PBDA Section length 
    {

        Status = ZwReadFile(hFileHandlePBDASI,
          NULL,
          NULL,
          NULL,
          &IoStatus,
          pPBDASection,
          ulLength,
          NULL,
          NULL);

        *pulReadLength = (ULONG)IoStatus.Information;
        if ((*pulReadLength) > ulLength)
        {
            *pulReadLength = 0;
            Status = STATUS_UNSUCCESSFUL;
        }

    }
    else
    {
        *pulReadLength = 0;
    }
    return Status;
}

void CFileReader::SetCurrentEPGStatistic(ULONG ulSavedSections, ULONG ulTotalSections)
{
    m_ulSavedSections = ulSavedSections;
    m_ulTotalSections = ulTotalSections;
    if (m_ulSavedSections > m_ulTotalSections)
    {
        //the estimation to total sections can be wrong over time because incremental updates 
        //increase the incoming data while the total amount remain or is replaced by outdated data
        m_ulTotalSections = m_ulSavedSections + 10;// so we keep next to the estimated limit with 10 packets 
    }
}

void CFileReader::GetCurrentEPGStatistic(ULONG *ulSavedSections, ULONG *ulTotalSections)
{
    if (ulSavedSections)
    {
        *ulSavedSections = m_ulSavedSections;
    }
    if (ulTotalSections)
    {
        *ulTotalSections = m_ulTotalSections;
    }
}

