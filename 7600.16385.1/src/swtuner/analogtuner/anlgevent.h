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
/**************************************************************************
    File    : anlgtuner.h
    Abstract: Contains pevent handling function and data structure.
**************************************************************************/

typedef struct _EventHandlerData {
   BOOLEAN          bStopScan;
   KSPIN_LOCK       LockAccess;
   ULONG            Status;
   KEVENT           InitEvent;
   KEVENT           TuneEvent;
   KEVENT           ThreadEvent;
   PIO_WORKITEM     ThreadItem;
   PKSEVENT_ENTRY   EventEntry;
   LARGE_INTEGER    IdleTimeout;     
   PKTHREAD         thread;
   KTIMER           timer;
   KDPC             DPCObject;
   ULONG            CurrentFrequency;
   TunerLockType    StatusCode;
   KSEVENT_TUNER_INITIATE_SCAN_S ScanData;
   CEncoderDevice*   pDevice;
} EventHandlerData, *PEventHandlerData;

extern NTSTATUS AnlgTunerScanAddEvent(PIRP pIrp,PKSEVENTDATA EventData,PKSEVENT_ENTRY pEventEntry);
extern VOID AnlgTunerScanRemoveEvent(PFILE_OBJECT  FileObject, struct _KSEVENT_ENTRY*  EventEntry);



