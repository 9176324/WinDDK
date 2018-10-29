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
    ULONG        Tunerfrequency;
    CFileReader   *FileReader;
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
	

public:
	CFileReader();
	CFileReader(PKSDEVICE Device, WCHAR* pczDeviceInstanceID, PUNICODE_STRING RegistryPath);
	void SetFrequency(ULONG frequency);
	ULONG GetFrequency();
    WCHAR* GetDeviceInstanceID();
	ULONG Read(OUT PUCHAR buffer, ULONG sampleSize);
	void Start();
	void Stop();

	PKSDEVICE GetDevice() { return m_Device;}
	PKEVENT GetFreqChangedEvent(void) { return &m_FreqChangedEvent;}
	void SetReadStarted(BOOL fReadStarted) { m_fReadStarted = fReadStarted;}
	BOOL IsReadStarted(void) { return m_fReadStarted;}
	BOOL SingalPresent();
};





