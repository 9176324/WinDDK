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

#pragma warning (disable : 28718 28931 28930 28170 28155 26035 6014 28172 28101 28168 6031 28922 6273 28251 36510)

class CFileReader;

typedef struct tag_ReadVideoStreamFile {
   KSPIN_LOCK  LockAccess;
   ULONG       Status;
   IO_STATUS_BLOCK IoStatus;
   HANDLE      VideoFile;
   CHAR *      ReadBuffers[2];
   ULONG       ReadBufferFill[2];
   ULONG       ReadBufferGet[2];
   ULONG       ReadIndex;
   PVOID       CaptureBuffer;
   ULONG       CaptureRemaining;
   ULONG       CaptureIndex;
   ULONG       ReadCount;
   ULONG       RewindCount;
   ULONG       GetSuccess;
   ULONG       GetFail;
   ULONG       GetFailRun;
   ULONG       GetFailRunMax;
   BOOL        StopRequest;
   KEVENT      ReadEvent;
   KEVENT      InitEvent;
   PIO_WORKITEM  ThreadItem;
   PWSTR        FileName;
   ULONG        Tunerfrequency;
   WCHAR*		pczDeviceInstanceID;
  CDevice* CDevice;
  CFileReader   *FileReader;
  BOOL          fNewFile;
} ReadVideoStreamFile,*PReadVideoStreamFile;

class CFileReader {

 protected:
  PKSDEVICE m_Device;
  PUNICODE_STRING m_RegistryPath;
  ULONG m_ulFrequency;
  PCWSTR m_pFileName;
  KEVENT m_FreqChangedEvent;
  BOOL m_fReadStarted;
  WCHAR* m_pczDeviceInstanceID;
  PReadVideoStreamFile m_VideoStream;
  ULONG m_ulSavedSections;
  ULONG m_ulTotalSections;

 public:
  CFileReader();
  CFileReader(PKSDEVICE Device, IN __nullterminated WCHAR* pczDeviceInstanceID, IN __nullterminated PUNICODE_STRING RegistryPath);
  void SetFrequency(ULONG frequency);
  ULONG GetFrequency();
  WCHAR* GetDeviceInstanceID();
  ULONG Read(OUT PUCHAR buffer, ULONG sampleSize, PBOOL pNewFile);
  void Start();
  void Stop();
  PKSDEVICE GetDevice() { return m_Device;}
  PKEVENT GetFreqChangedEvent(void) { return &m_FreqChangedEvent;}
  void SetReadStarted(BOOL fReadStarted) { m_fReadStarted = fReadStarted;}
  BOOL IsReadStarted(void) { return m_fReadStarted;}
  void SetCurrentEPGStatistic(ULONG ulSavedSections, ULONG ulTotalSections);
  void GetCurrentEPGStatistic(ULONG *ulSavedSections, ULONG *ulTotalSections);
  NTSTATUS OpenEPGStream(HANDLE *pFileHandlePBDASI, DWORD dwInitialFreq, BOOL bReadAccess);
  NTSTATUS GetSwtunerInstallPath(OUT PUNICODE_STRING FilePath);
  void FlushPBDASIBuffers(CTsTimestamp *pctts, HANDLE hFileHandlePBDASI);
  NTSTATUS ReadNextPBDASIBufferFromFile(HANDLE hFileHandlePBDASI, ULONG *pulReadLength, BYTE *pPBDASection);
  DWORD m_GuideCacheUsage; //GuideCache usage 0=default to create and erase every time the channel change, 1=create and append over time

};





