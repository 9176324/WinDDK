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

#include "BDATuner.h"

#ifdef ALLOC_DATA_PRAGMA
#pragma const_seg("PAGECONST")
#endif // ALLOC_DATA_PRAGMA

#define MS_SAMPLE_TUNER_POOL_TAG 'TadB'

/*
** Constructor() method of the CFilter class
**
**    Creates the filter object,
**    associates it with the device object, and 
**    initializes member variables for it.
**
*/
CFilter::CFilter()
{

    KeInitializeSpinLock(&spinLock);
    pDevObject = NULL;
    pFiObject = NULL;
    m_lEventId = 0;
    m_bPendingEventsEnabled = FALSE;
    m_PendingEventEntry = NULL;
    m_bSignallingPendingEvent = FALSE;
    KeInitializeEvent(&m_evPendingEventsComplete,SynchronizationEvent,FALSE);
    m_ulCurrentDialogRequest = 0;
    m_ulCurrentDialogNumber = 0;
    m_fBroadcastMMIRequested = FALSE;

    WCHAR * cNameStr;
    WCHAR * cValueStr;
    ULONG ulNameLen=0;
    ULONG ulValueLen=0;
    int i;

    for (i=0; i < GPNVS_NUMBER_OF_ROWS; i++)
    {
    
        switch (i)
        {
            case 0:
                ulNameLen = sizeof(L"PBDA:/NV/Variable/Connector Id");
                ulValueLen = 0;
                cNameStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulNameLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                cValueStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulValueLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                RtlStringCbCopy(cNameStr,ulNameLen + sizeof(WCHAR), L"PBDA:/NV/Variable/Connector Id");
                cValueStr[0] = 0;
                break;
            case 1:
                ulNameLen = sizeof(L"PBDA:/NV/Variable/Device Id");
                ulValueLen = sizeof(L"D1AC3899-802C-4b13-9870-56B2F2CCB3A2");//UUID for this device, do not use a static uuid, it will be set at load time
                cNameStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulNameLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                cValueStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulValueLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                RtlStringCbCopy(cNameStr,ulNameLen + sizeof(WCHAR), L"PBDA:/NV/Variable/Device Id");
                RtlStringCbCopy(cValueStr,ulValueLen + sizeof(WCHAR), L"D1AC3899-802C-4b13-9870-56B2F2CCB3A2");
               break;
            case 2:
                ulNameLen = sizeof(L"PBDA:/NV/Variable/Tuner Id");
                ulValueLen = sizeof(L"2734DC82-774D-4daa-A5AD-2838B5A58127");//UUID for this tuner, do not use a static uuid, it will be set at load time
                cNameStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulNameLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                cValueStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulValueLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                RtlStringCbCopy(cNameStr,ulNameLen + sizeof(WCHAR), L"PBDA:/NV/Variable/Tuner Id");
                RtlStringCbCopy(cValueStr,ulValueLen + sizeof(WCHAR), L"2734DC82-774D-4daa-A5AD-2838B5A58127");
               break;
            case 3:
                ulNameLen = sizeof(L"PBDA:/NV/Variable/Supported Languages");
                ulValueLen = sizeof(L"ENG-US,JPN-JP,DEU-DE,FRE-FR");
                cNameStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulNameLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                cValueStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulValueLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                RtlStringCbCopy(cNameStr,ulNameLen + sizeof(WCHAR), L"PBDA:/NV/Variable/Supported Languages");
                RtlStringCbCopy(cValueStr,ulValueLen + sizeof(WCHAR), L"ENG-US,JPN-JP,DEU-DE,FRE-FR");
               break;
            case 4:
                ulNameLen = sizeof(L"PBDA:/NV/Variable/Firmware Version");
                ulValueLen = 0;
                cNameStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulNameLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                cValueStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulValueLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                RtlStringCbCopy(cNameStr,ulNameLen + sizeof(WCHAR), L"PBDA:/NV/Variable/Firmware Version");
                cValueStr[0] = 0;
               break;
            case 5:
                ulNameLen = sizeof(L"PBDA:/NV/Variable/MSD Type");
                ulValueLen = 0;
                cNameStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulNameLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                cValueStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulValueLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                RtlStringCbCopy(cNameStr,ulNameLen + sizeof(WCHAR), L"PBDA:/NV/Variable/MSD Type");
                cValueStr[0] = 0;
               break;
            case 6:
                ulNameLen = sizeof(L"PBDA:/NV/Variable/MSD Version");
                ulValueLen = sizeof(L"0.99.01");
                cNameStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulNameLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                cValueStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulValueLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                RtlStringCbCopy(cNameStr,ulNameLen + sizeof(WCHAR), L"PBDA:/NV/Variable/MSD Version");
                RtlStringCbCopy(cValueStr,ulValueLen + sizeof(WCHAR), L"0.99.01");
               break;
            case 7:
                ulNameLen = sizeof(L"PBDA:/NV/Variable/Manufacturer");
                ulValueLen = 0;
                cNameStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulNameLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                cValueStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulValueLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                RtlStringCbCopy(cNameStr,ulNameLen + sizeof(WCHAR), L"PBDA:/NV/Variable/Manufacturer");
                cValueStr[0] = 0;
               break;
            case 8:
                ulNameLen = sizeof(L"PBDA:/NV/Variable/Model Name");
#ifdef NETOP_RECEIVER
                ulValueLen = sizeof(L"KS SWTUNER Netop");
#elif PBDAT_RECEIVER
                ulValueLen = sizeof(L"KS SWTUNER PBDA-T");
#elif DVBS_RECEIVER
                ulValueLen = sizeof(L"KS SWTUNER DVB-S");
#elif DMBTH_RECEIVER    //DMBTH ADD
                ulValueLen = sizeof(L"KS SWTUNER DMB-TH");
#else
                ulValueLen = 0;
#endif
                cNameStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulNameLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                cValueStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulValueLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                RtlStringCbCopy(cNameStr,ulNameLen + sizeof(WCHAR), L"PBDA:/NV/Variable/Model Name");
                RtlStringCbCopy(cValueStr,ulValueLen + sizeof(WCHAR), L"PBDA KS SWTUNER");
#ifdef NETOP_RECEIVER
                RtlStringCbCopy(cValueStr,ulValueLen + sizeof(WCHAR), L"KS SWTUNER Netop");
#elif PBDAT_RECEIVER
                RtlStringCbCopy(cValueStr,ulValueLen + sizeof(WCHAR), L"KS SWTUNER PBDA-T");
#elif DVBS_RECEIVER
                RtlStringCbCopy(cValueStr,ulValueLen + sizeof(WCHAR), L"KS SWTUNER DVB-S");
#elif DMBTH_RECEIVER    //DMBTH ADD
                RtlStringCbCopy(cValueStr,ulValueLen + sizeof(WCHAR), L"KS SWTUNER DMB-TH");
#else
                cValueStr[0] = 0;
#endif
               break;
            case 9:
                ulNameLen = sizeof(L"PBDA:/NV/Variable/Model Number");
                ulValueLen = 0;
                cNameStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulNameLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                cValueStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulValueLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                RtlStringCbCopy(cNameStr,ulNameLen + sizeof(WCHAR), L"PBDA:/NV/Variable/Model Number");
                cValueStr[0] = 0;
               break;
            case 10:
                ulNameLen = sizeof(L"PBDA:/NV/Variable/Serial Number");
#ifdef NETOP_RECEIVER
                ulValueLen = sizeof(L"1.0.0.1");
#elif PBDAT_RECEIVER
                ulValueLen = sizeof(L"1.0.0.2");
#elif DVBS_RECEIVER
                ulValueLen = sizeof(L"1.0.0.3");
#elif DMBTH_RECEIVER    //DMBTH ADD
                ulValueLen = sizeof(L"1.0.0.4");
#else
                ulValueLen = 0;
#endif
                cNameStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulNameLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                cValueStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulValueLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                RtlStringCbCopy(cNameStr,ulNameLen + sizeof(WCHAR), L"PBDA:/NV/Variable/Serial Number");
#ifdef NETOP_RECEIVER
                RtlStringCbCopy(cValueStr,ulValueLen+ sizeof(WCHAR), L"1.0.0.1");
#elif PBDAT_RECEIVER
                RtlStringCbCopy(cValueStr,ulValueLen+ sizeof(WCHAR), L"1.0.0.2");
#elif DVBS_RECEIVER
                RtlStringCbCopy(cValueStr,ulValueLen+ sizeof(WCHAR), L"1.0.0.3");
#elif DMBTH_RECEIVER    //DMBTH ADD
                RtlStringCbCopy(cValueStr,ulValueLen+ sizeof(WCHAR), L"1.0.0.4");
#else
                cValueStr[0] = 0;
#endif
               break;
            case 11:
                ulNameLen = sizeof(L"PBDA:/NV/Variable/Tuner Types");
                ulValueLen = sizeof(L"95037F6F-3AC7-4452-B6C4-45A9CE9292A2");
                cNameStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulNameLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                cValueStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulValueLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                RtlStringCbCopy(cNameStr,ulNameLen + sizeof(WCHAR), L"PBDA:/NV/Variable/Tuner Types");
                cValueStr[0] = 0;
#ifdef ISDBT_RECEIVER 
                RtlStringCbCopy(cValueStr,ulValueLen + sizeof(WCHAR), L"95037F6F-3AC7-4452-B6C4-45A9CE9292A2");//ISDB-T
#endif
#ifdef ISDBS_RECEIVER 
                RtlStringCbCopy(cValueStr,ulValueLen + sizeof(WCHAR), L"B0A4E6A0-6A1A-4B83-BB5B-903E1D90E6B6");//ISDB-S
#endif
#ifdef NETOP_RECEIVER 
                RtlStringCbCopy(cValueStr,ulValueLen + sizeof(WCHAR), L"216C62DF-6D7F-4E9A-8571-05F14EDB766A");//DVB-T
#endif
#ifdef PBDAT_RECEIVER
                RtlStringCbCopy(cValueStr,ulValueLen + sizeof(WCHAR), L"216C62DF-6D7F-4E9A-8571-05F14EDB766A");//DVB-T
#endif
#ifdef DMBTH_RECEIVER    ///DMBTH ADD
                RtlStringCbCopy(cValueStr,ulValueLen + sizeof(WCHAR), L"216C62DF-6D7F-4E9A-8571-05F14EDB766A");//DVB-T
#endif
               break;
            case 12:
                ulNameLen = sizeof(L"PBDA:/NV/Variable/Configured Tuner Types");
                ulValueLen = 0;
                cNameStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulNameLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                cValueStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulValueLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                RtlStringCbCopy(cNameStr,ulNameLen + sizeof(WCHAR), L"PBDA:/NV/Variable/Configured Tuner Types");
                cValueStr[0] = 0;
               break;
            case 13:
                ulNameLen = sizeof(L"PBDA:/NV/Variable/PBDA Version");
                ulValueLen = 0;
                cNameStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulNameLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                cValueStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulValueLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                RtlStringCbCopy(cNameStr,ulNameLen + sizeof(WCHAR), L"PBDA:/NV/Variable/PBDA Version");
                cValueStr[0] = 0;
               break;
            case 14:
                ulNameLen = sizeof(L"PBDA:/NV/Variable/Postal Code");
                ulValueLen = sizeof(L"47891");
                cNameStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulNameLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                cValueStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulValueLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                RtlStringCbCopy(cNameStr,ulNameLen + sizeof(WCHAR), L"PBDA:/NV/Variable/Postal Code");
                RtlStringCbCopy(cValueStr,ulValueLen + sizeof(WCHAR), L"47891");
               break;
            case 15:
                ulNameLen = sizeof(L"PBDA:/Microsoft.com/NV/Variable/Windows Version");
                ulValueLen = sizeof(L"NOTHING");
                cNameStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulNameLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                cValueStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulValueLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                RtlStringCbCopy(cNameStr,ulNameLen + sizeof(WCHAR), L"PBDA:/Microsoft.com/NV/Variable/Windows Version");
                RtlStringCbCopy(cValueStr,ulValueLen + sizeof(WCHAR), L"NOTHING");
               break;
            case 16:
                ulNameLen = sizeof(L"PBDA:/Microsoft.com/NV/Variable/MCE/MCML Version");
                ulValueLen = sizeof(L"NOTHING");
                cNameStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulNameLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                cValueStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulValueLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                RtlStringCbCopy(cNameStr,ulNameLen + sizeof(WCHAR), L"PBDA:/Microsoft.com/NV/Variable/MCE/MCML Version");
                RtlStringCbCopy(cValueStr,ulValueLen + sizeof(WCHAR), L"NOTHING");
               break;
            case 17:
                ulNameLen = sizeof(L"PBDA:/Microsoft.com/NV/Variable/MCE/TuningSpaces/Assets/BrandingLogo.png");
                ulValueLen = sizeof(L"NOTHING");
                cNameStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulNameLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                cValueStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulValueLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                RtlStringCbCopy(cNameStr,ulNameLen + sizeof(WCHAR), L"PBDA:/Microsoft.com/NV/Variable/MCE/TuningSpaces/Assets/BrandingLogo.png");
                RtlStringCbCopy(cValueStr,ulValueLen + sizeof(WCHAR), L"NOTHING");
               break;
            case 18:
                ulNameLen = sizeof(L"PBDA:/Microsoft.com/NV/Variable/MCE/Extended GuideTypes");
                ulValueLen = sizeof(L"NOTHING");
                cNameStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulNameLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                cValueStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulValueLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                RtlStringCbCopy(cNameStr,ulNameLen + sizeof(WCHAR), L"PBDA:/Microsoft.com/NV/Variable/MCE/Extended GuideTypes");
                RtlStringCbCopy(cValueStr,ulValueLen + sizeof(WCHAR), L"NOTHING");
               break;
            case 19:
                ulNameLen = sizeof(L"PBDA:/Microsoft.com/NV/Variable/MCE/Closed Captioning Format UUID");
                ulValueLen = sizeof(L"NOTHING");
                cNameStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulNameLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                cValueStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulValueLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                RtlStringCbCopy(cNameStr,ulNameLen + sizeof(WCHAR), L"PBDA:/Microsoft.com/NV/Variable/MCE/Closed Captioning Format UUID");
                RtlStringCbCopy(cValueStr,ulValueLen + sizeof(WCHAR), L"NOTHING");
               break;
            case 20:
                ulNameLen = sizeof(L"PBDA:/Microsoft.com/NV/Variable/MCE/Source Types");
                cNameStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulNameLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                RtlStringCbCopy(cNameStr,ulNameLen + sizeof(WCHAR), L"PBDA:/Microsoft.com/NV/Variable/MCE/Source Types");
#if defined(NETOP_RECEIVER) 
                ulValueLen = sizeof(L"<SourceTypes><SourceType TuningSpaceUniqueName=\"DVB-T\"><HeadendType id=\"DVBt\" name=\"Digital Antenna (DVB-T PBDA)\" /><NetworkType id=\"216C62DF-6D7F-4E9A-8571-05F14EDB766A\" name=\"DVB-T\" /><VideoSource id=\"21d902d3-3912-4fe1-a0c9-2fece42efa51\" name=\"PBDA Antenna DVB-T\" /></SourceType></SourceTypes>");
                cValueStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulValueLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                RtlStringCbCopy(cValueStr,ulValueLen + sizeof(WCHAR), L"<SourceTypes><SourceType TuningSpaceUniqueName=\"DVB-T\"><HeadendType id=\"DVBt\" name=\"Digital Antenna (DVB-T PBDA)\" /><NetworkType id=\"216C62DF-6D7F-4E9A-8571-05F14EDB766A\" name=\"DVB-T\" /><VideoSource id=\"21d902d3-3912-4fe1-a0c9-2fece42efa51\" name=\"PBDA Antenna DVB-T\" /></SourceType></SourceTypes>");
#elif defined(PBDAT_RECEIVER)
                ulValueLen = sizeof(L"<SourceTypes><SourceType TuningSpaceUniqueName=\"DVB-T\"><HeadendType id=\"DVBt\" name=\"Digital Antenna (PBDA-T)\" /><NetworkType id=\"216C62DF-6D7F-4E9A-8571-05F14EDB766A\" name=\"DVB-T\" /><VideoSource id=\"21d902d3-3912-4fe1-a0c9-2fece42efa51\" name=\"PBDA-BDA Antenna DVB-T\" /></SourceType></SourceTypes>");
                cValueStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulValueLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                RtlStringCbCopy(cValueStr,ulValueLen + sizeof(WCHAR), L"<SourceTypes><SourceType TuningSpaceUniqueName=\"DVB-T\"><HeadendType id=\"DVBt\" name=\"Digital Antenna (PBDA-T)\" /><NetworkType id=\"216C62DF-6D7F-4E9A-8571-05F14EDB766A\" name=\"DVB-T\" /><VideoSource id=\"21d902d3-3912-4fe1-a0c9-2fece42efa51\" name=\"PBDA-BDA Antenna DVB-T\" /></SourceType></SourceTypes>");
#elif defined(DMBTH_RECEIVER)    ///DMBTH ADD
                ulValueLen = sizeof(L"<SourceTypes><SourceType TuningSpaceUniqueName=\"DVB-T\"><HeadendType id=\"DVBt\" name=\"Digital Antenna (DMB-TH)\" /><NetworkType id=\"216C62DF-6D7F-4E9A-8571-05F14EDB766A\" name=\"DVB-T\" /><VideoSource id=\"21d902d3-3912-4fe1-a0c9-2fece42efa51\" name=\"PBDA-BDA Antenna DVB-T\" /></SourceType></SourceTypes>");
                cValueStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulValueLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                RtlStringCbCopy(cValueStr,ulValueLen + sizeof(WCHAR), L"<SourceTypes><SourceType TuningSpaceUniqueName=\"DVB-T\"><HeadendType id=\"DVBt\" name=\"Digital Antenna (DMB-TH)\" /><NetworkType id=\"216C62DF-6D7F-4E9A-8571-05F14EDB766A\" name=\"DVB-T\" /><VideoSource id=\"21d902d3-3912-4fe1-a0c9-2fece42efa51\" name=\"PBDA-BDA Antenna DVB-T\" /></SourceType></SourceTypes>");
#else
                ulValueLen = 0;
                cValueStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulValueLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                cValueStr[0] = 0;
#endif
                break;
            case 21:
#if defined(DMBTH_RECEIVER)    ///DMBTH ADD
                ulNameLen = sizeof(L"PBDA:/NV/Variable/ISDB no BCAS Id"); // Bug Frun show device page
#else
                ulNameLen = sizeof(L"PBDA:/NV/Variable/ISDB BCAS Id");
#endif
                ulValueLen = sizeof(L"T002,0000-0804-0000-0013-0008");
                cNameStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulNameLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                cValueStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulValueLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
#if defined(DMBTH_RECEIVER)    ///DMBTH ADD 
                RtlStringCbCopy(cNameStr,ulNameLen + sizeof(WCHAR), L"PBDA:/NV/Variable/ISDB no BCAS Id"); 
#else
                RtlStringCbCopy(cNameStr,ulNameLen + sizeof(WCHAR), L"PBDA:/NV/Variable/ISDB BCAS Id");
#endif
                RtlStringCbCopy(cValueStr,ulValueLen + sizeof(WCHAR), L"T002,0000-0804-0000-0013-0008");
               break;
            case 22:
                ulNameLen = sizeof(L"PBDA:/NV/Method/System Setup");
                ulValueLen = 0;
                cNameStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulNameLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                cValueStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulValueLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                RtlStringCbCopy(cNameStr,ulNameLen + sizeof(WCHAR), L"PBDA:/NV/Method/System Setup");
                cValueStr[0] = 0;
               break;
            case 23:
                ulNameLen = sizeof(L"PBDA:/NV/Method/System Diagnostics");
                ulValueLen = 0;
                cNameStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulNameLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                cValueStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulValueLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                RtlStringCbCopy(cNameStr,ulNameLen + sizeof(WCHAR), L"PBDA:/NV/Method/System Diagnostics");
                cValueStr[0] = 0;
               break;
            case 24:
                ulNameLen = sizeof(L"PBDA:/NV/Variable/RESERVED_DYNAMIC");
                ulValueLen = sizeof(L"0");
                cNameStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulNameLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                cValueStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulValueLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                RtlStringCbCopy(cNameStr,ulNameLen + sizeof(WCHAR), L"PBDA:/NV/Variable/RESERVED_DYNAMIC");
                RtlStringCbCopy(cValueStr,ulValueLen + sizeof(WCHAR), L"0");
               break;
            case 25://not visible to Name Index 
                ulNameLen = sizeof(L"PBDA://2734DC82-774D-4daa-A5AD-2838B5A58127/Microsoft.com/Resource/testpage.html");
                ulValueLen = sizeof(L"<html> <body>Default MMI message from the PBDA-KS SWTuner<br> <button defaultFocus=\"true\" href=\"MoreInfo.html\">More Info</button> <button href=\"\">Close</button> </body> </html>");
                cNameStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulNameLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                cValueStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulValueLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                RtlStringCbCopy(cNameStr,ulNameLen + sizeof(WCHAR), L"PBDA://2734DC82-774D-4daa-A5AD-2838B5A58127/Microsoft.com/Resource/testpage.html");
                RtlStringCbCopy(cValueStr,ulValueLen + sizeof(WCHAR), L"<html> <body>Default MMI message from the PBDA-KS SWTuner<br> <button defaultFocus=\"true\" href=\"MoreInfo.html\">More Info</button> <button href=\"\">Close</button> </body> </html>");
               break;
            case 26://not visible to Name Index
                ulNameLen = sizeof(L"PBDA://2734DC82-774D-4daa-A5AD-2838B5A58127/Microsoft.com/Resource/moreinfo.html");
                ulValueLen = sizeof(L"<html> <body>Start Up and Unicode Text button will only work if the actually html files existed inside the PBDDA-KS device.<br> <BUTTON href=\"startup.html\" onclick=\"window.navigate(event.srcElement.href)\">Start Up</BUTTON> <BUTTON href=\"UnicodeText.html\" onclick=\"window.navigate(event.srcElement.href)\">Unicode Text</BUTTON> <BUTTON href=\"\">Close</BUTTON> </body> </html>");
                cNameStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulNameLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                cValueStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulValueLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                RtlStringCbCopy(cNameStr,ulNameLen + sizeof(WCHAR), L"PBDA://2734DC82-774D-4daa-A5AD-2838B5A58127/Microsoft.com/Resource/moreinfo.html");
                RtlStringCbCopy(cValueStr,ulValueLen + sizeof(WCHAR), L"<html> <body>Start Up and Unicode Text button will only work if the actually html files existed inside the PBDDA-KS device.<br> <BUTTON href=\"startup.html\" onclick=\"window.navigate(event.srcElement.href)\">Start Up</BUTTON> <BUTTON href=\"UnicodeText.html\" onclick=\"window.navigate(event.srcElement.href)\">Unicode Text</BUTTON> <BUTTON href=\"\">Close</BUTTON> </body> </html>");
               break;
            case 27://not visible to Name Index 
#if defined(DMBTH_RECEIVER) // Go to Default OEM Page 
				ulNameLen = sizeof(L"PBDA://2734DC82-774D-4daa-A5AD-2838B5A58127/Microsoft.com/Resource/Setup.mcml");
                ulValueLen = sizeof(L"<Mcml xmlns=\"http://schemas.microsoft.com/2008/mcml\"><UI Name=\"ColorFill\"><Rules><Rule><Actions><Navigate Source=\"PBDA://2734dc82-774d-4daa-a5ad-2838b5a58127/Gpnvs Command LFILE:Setup.mcml\" /></Actions></Rule></Rules></UI></Mcml>");
                cNameStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulNameLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                cValueStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulValueLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                RtlStringCbCopy(cNameStr,ulNameLen + sizeof(WCHAR), L"PBDA://2734DC82-774D-4daa-A5AD-2838B5A58127/Microsoft.com/Resource/Setup.mcml");
                RtlStringCbCopy(cValueStr,ulValueLen + sizeof(WCHAR), L"<Mcml xmlns=\"http://schemas.microsoft.com/2008/mcml\"><UI Name=\"ColorFill\"><Rules><Rule><Actions><Navigate Source=\"PBDA://2734dc82-774d-4daa-a5ad-2838b5a58127/Gpnvs Command LFILE:Setup.mcml\" /></Actions></Rule></Rules></UI></Mcml>");
#else
                ulNameLen = sizeof(L"PBDA://2734DC82-774D-4daa-A5AD-2838B5A58127/Microsoft.com/Resource/Setup.mcml");
                ulValueLen = sizeof(L"<Mcml xmlns=\"http://schemas.microsoft.com/2006/mcml\" xmlns:cor=\"assembly://MSCorlib/System\" xmlns:addin=\"assembly://Microsoft.MediaCenter/Microsoft.MediaCenter.Hosting\" xmlns:me=\"Me\"><!-- FirstRun page. Settings is same, except for text. --><UI Name=\"FirstRunPage\"><Properties><Font Name=\"TitleFont\" FontName=\"Segoe Media Center Semibold\" FontSize=\"20\" FontStyle=\"Bold\"/></Properties><Locals><addin:AddInHost Name=\"AddInHost\"/><Command Name=\"Cancel\" Description=\"Cancel\"/><Command Name=\"Finish\" Description=\"Finish\"/><cor:Int32 Name=\"SessionResult\" Int32=\"0\"/></Locals><Rules><Changed Source=\"[Cancel.Invoked]\"><Actions><Set Target=\"[SessionResult]\" Value=\"2\"/><Invoke Target=\"[AddInHost.ApplicationContext.CloseApplication]\"/><!-- Value is 2 for cancel. What else should it do?--></Actions></Changed><Changed Source=\"[Finish.Invoked]\"><Actions><Set Target=\"[SessionResult]\" Value=\"0\"/><Invoke Target=\"[AddInHost.ApplicationContext.CloseApplication]\"/><!-- Value is 0 for finish. What else should it do?--></Actions></Changed><!-- Accessibility defaults --><Default Target=\"[Accessible.Name]\" Value=\"First Run\"/><Default Target=\"[Accessible.Role]\" Value=\"Pane\"/></Rules><Content><Panel Name=\"MainPage\" Margins=\"73, 139, 50, 0\"><Layout><FlowLayout Orientation=\"Vertical\"/></Layout><Children><Text Content=\"Welcome to NetOp for PBDA-KS First Run!!\" Color=\"TVWhite\" Font=\"[TitleFont]\"/><Text Content=\"Note that you will return to bookend page (no close MMI)\" Color=\"TVWhite\" Font=\"[TitleFont]\"/><Panel Name=\"Buttons\" Margins=\"200, 439, 50, 0\"><Layout><FlowLayout Orientation=\"Horizontal\"/></Layout><Children><!-- Finish button. --><me:SimpleButton Command=\"[Finish]\" Margins=\"100, 0, 0, 0\"/></Children></Panel></Children></Panel></Content></UI><!-- Simple button --><UI Name=\"SimpleButton\"><Properties><!-- Command is a required parameter. --><ICommand Name=\"Command\" ICommand=\"$Required\"/><!-- Background color properties. --><Color Name=\"BackgroundColor\" Color=\"DimGray\"/><Color Name=\"BackgroundFocusedColor\" Color=\"DarkGray\"/><Color Name=\"BackgroundHoverColor\" Color=\"PaleGoldenrod\"/><Color Name=\"BackgroundPressedColor\" Color=\"White\"/><!-- Label color properties. --><Color Name=\"LabelColor\" Color=\"White\"/><Color Name=\"LabelFocusedColor\" Color=\"White\"/><Color Name=\"LabelHoverColor\" Color=\"DimGray\"/><Color Name=\"LabelPressedColor\" Color=\"Black\"/><!-- Label font property. --><Font Name=\"LabelFont\" Font=\"Arial,20\"/></Properties><Locals><!-- React to \"click\" input. --><ClickHandler Name=\"Clicker\"/></Locals><Rules><!-- The command description is displayed by the text box. A  --><!-- binding is used in case the description changes at       --><!-- runtime.                                                 --><Binding Source=\"[Command.Description]\" Target=\"[Label.Content]\"/><Binding Source=\"[Command]\" Target=\"[Clicker.Command]\"/><!-- Change color on click. --><Condition Source=\"[Clicker.Clicking]\" SourceValue=\"true\"><Actions><Set Target=\"[Background.Content]\" Value=\"[BackgroundPressedColor]\"/><Set Target=\"[Label.Color]\" Value=\"[LabelPressedColor]\"/></Actions></Condition><!-- Change color on mouse focus. --><Condition Source=\"[Input.MouseFocus]\" SourceValue=\"true\"><Actions><Set Target=\"[Background.Content]\" Value=\"[BackgroundHoverColor]\"/><Set Target=\"[Label.Color]\" Value=\"[LabelHoverColor]\"/></Actions></Condition><!-- Change color on key focus. --><Condition Source=\"[Input.KeyFocus]\" SourceValue=\"true\"><Actions><Set Target=\"[Background.Content]\" Value=\"[BackgroundFocusedColor]\"/><Set Target=\"[Label.Color]\" Value=\"[LabelFocusedColor]\"/></Actions></Condition><!-- Various other state is hooked up as well (all of which are    --><!-- important for accessiblity aides.                             --><Binding Target=\"[Accessible.IsPressed]\"            Source=\"[Clicker.Clicking]\"/><Binding Target=\"[Accessible.IsFocusable]\"          Source=\"[Input.KeyInteractive]\"/><Binding Target=\"[Accessible.IsFocused]\"            Source=\"[Input.KeyFocus]\"/><Binding Target=\"[Accessible.Name]\"                 Source=\"[Command.Description]\"/><Default Target=\"[Accessible.DefaultActionCommand]\" Value=\"[Command]\"/><Default Target=\"[Accessible.DefaultAction]\"        Value=\"Press\"/><Default Target=\"[Accessible.Role]\"                 Value=\"PushButton\"/></Rules><Content><!-- Solid background color. --><ColorFill Name=\"Background\" Content=\"[BackgroundColor]\" Padding=\"5,5,5,5\"><Children><!-- The label to display. --><Text Name=\"Label\" Color=\"[LabelColor]\" Font=\"[LabelFont]\"/></Children></ColorFill></Content></UI></Mcml>");
                cNameStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulNameLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                cValueStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulValueLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                RtlStringCbCopy(cNameStr,ulNameLen + sizeof(WCHAR), L"PBDA://2734DC82-774D-4daa-A5AD-2838B5A58127/Microsoft.com/Resource/Setup.mcml");
                RtlStringCbCopy(cValueStr,ulValueLen + sizeof(WCHAR), L"<Mcml xmlns=\"http://schemas.microsoft.com/2006/mcml\" xmlns:cor=\"assembly://MSCorlib/System\" xmlns:addin=\"assembly://Microsoft.MediaCenter/Microsoft.MediaCenter.Hosting\" xmlns:me=\"Me\"><!-- FirstRun page. Settings is same, except for text. --><UI Name=\"FirstRunPage\"><Properties><Font Name=\"TitleFont\" FontName=\"Segoe Media Center Semibold\" FontSize=\"20\" FontStyle=\"Bold\"/></Properties><Locals><addin:AddInHost Name=\"AddInHost\"/><Command Name=\"Cancel\" Description=\"Cancel\"/><Command Name=\"Finish\" Description=\"Finish\"/><cor:Int32 Name=\"SessionResult\" Int32=\"0\"/></Locals><Rules><Changed Source=\"[Cancel.Invoked]\"><Actions><Set Target=\"[SessionResult]\" Value=\"2\"/><Invoke Target=\"[AddInHost.ApplicationContext.CloseApplication]\"/><!-- Value is 2 for cancel. What else should it do?--></Actions></Changed><Changed Source=\"[Finish.Invoked]\"><Actions><Set Target=\"[SessionResult]\" Value=\"0\"/><Invoke Target=\"[AddInHost.ApplicationContext.CloseApplication]\"/><!-- Value is 0 for finish. What else should it do?--></Actions></Changed><!-- Accessibility defaults --><Default Target=\"[Accessible.Name]\" Value=\"First Run\"/><Default Target=\"[Accessible.Role]\" Value=\"Pane\"/></Rules><Content><Panel Name=\"MainPage\" Margins=\"73, 139, 50, 0\"><Layout><FlowLayout Orientation=\"Vertical\"/></Layout><Children><Text Content=\"Welcome to NetOp for PBDA-KS First Run!!\" Color=\"TVWhite\" Font=\"[TitleFont]\"/><Text Content=\"Note that you will return to bookend page (no close MMI)\" Color=\"TVWhite\" Font=\"[TitleFont]\"/><Panel Name=\"Buttons\" Margins=\"200, 439, 50, 0\"><Layout><FlowLayout Orientation=\"Horizontal\"/></Layout><Children><!-- Finish button. --><me:SimpleButton Command=\"[Finish]\" Margins=\"100, 0, 0, 0\"/></Children></Panel></Children></Panel></Content></UI><!-- Simple button --><UI Name=\"SimpleButton\"><Properties><!-- Command is a required parameter. --><ICommand Name=\"Command\" ICommand=\"$Required\"/><!-- Background color properties. --><Color Name=\"BackgroundColor\" Color=\"DimGray\"/><Color Name=\"BackgroundFocusedColor\" Color=\"DarkGray\"/><Color Name=\"BackgroundHoverColor\" Color=\"PaleGoldenrod\"/><Color Name=\"BackgroundPressedColor\" Color=\"White\"/><!-- Label color properties. --><Color Name=\"LabelColor\" Color=\"White\"/><Color Name=\"LabelFocusedColor\" Color=\"White\"/><Color Name=\"LabelHoverColor\" Color=\"DimGray\"/><Color Name=\"LabelPressedColor\" Color=\"Black\"/><!-- Label font property. --><Font Name=\"LabelFont\" Font=\"Arial,20\"/></Properties><Locals><!-- React to \"click\" input. --><ClickHandler Name=\"Clicker\"/></Locals><Rules><!-- The command description is displayed by the text box. A  --><!-- binding is used in case the description changes at       --><!-- runtime.                                                 --><Binding Source=\"[Command.Description]\" Target=\"[Label.Content]\"/><Binding Source=\"[Command]\" Target=\"[Clicker.Command]\"/><!-- Change color on click. --><Condition Source=\"[Clicker.Clicking]\" SourceValue=\"true\"><Actions><Set Target=\"[Background.Content]\" Value=\"[BackgroundPressedColor]\"/><Set Target=\"[Label.Color]\" Value=\"[LabelPressedColor]\"/></Actions></Condition><!-- Change color on mouse focus. --><Condition Source=\"[Input.MouseFocus]\" SourceValue=\"true\"><Actions><Set Target=\"[Background.Content]\" Value=\"[BackgroundHoverColor]\"/><Set Target=\"[Label.Color]\" Value=\"[LabelHoverColor]\"/></Actions></Condition><!-- Change color on key focus. --><Condition Source=\"[Input.KeyFocus]\" SourceValue=\"true\"><Actions><Set Target=\"[Background.Content]\" Value=\"[BackgroundFocusedColor]\"/><Set Target=\"[Label.Color]\" Value=\"[LabelFocusedColor]\"/></Actions></Condition><!-- Various other state is hooked up as well (all of which are    --><!-- important for accessiblity aides.                             --><Binding Target=\"[Accessible.IsPressed]\"            Source=\"[Clicker.Clicking]\"/><Binding Target=\"[Accessible.IsFocusable]\"          Source=\"[Input.KeyInteractive]\"/><Binding Target=\"[Accessible.IsFocused]\"            Source=\"[Input.KeyFocus]\"/><Binding Target=\"[Accessible.Name]\"                 Source=\"[Command.Description]\"/><Default Target=\"[Accessible.DefaultActionCommand]\" Value=\"[Command]\"/><Default Target=\"[Accessible.DefaultAction]\"        Value=\"Press\"/><Default Target=\"[Accessible.Role]\"                 Value=\"PushButton\"/></Rules><Content><!-- Solid background color. --><ColorFill Name=\"Background\" Content=\"[BackgroundColor]\" Padding=\"5,5,5,5\"><Children><!-- The label to display. --><Text Name=\"Label\" Color=\"[LabelColor]\" Font=\"[LabelFont]\"/></Children></ColorFill></Content></UI></Mcml>");
#endif
               break;
            case 28://not visible to Name Index 
                ulNameLen = sizeof(L"PBDA://2734DC82-774D-4daa-A5AD-2838B5A58127/Microsoft.com/Resource/Diagnostics.mcml");
                ulValueLen = sizeof(L"<Mcml xmlns=\"http://schemas.microsoft.com/2006/mcml\" xmlns:cor=\"assembly://MSCorlib/System\" xmlns:addin=\"assembly://Microsoft.MediaCenter/Microsoft.MediaCenter.Hosting\" xmlns:me=\"Me\"><!-- FirstRun page. Settings is same, except for text. --><UI Name=\"FirstRunPage\"><Properties><Font Name=\"TitleFont\" FontName=\"Segoe Media Center Semibold\" FontSize=\"20\" FontStyle=\"Bold\"/></Properties><Locals><addin:AddInHost Name=\"AddInHost\"/><Command Name=\"Cancel\" Description=\"Cancel\"/><Command Name=\"Finish\" Description=\"Finish\"/><cor:Int32 Name=\"SessionResult\" Int32=\"0\"/></Locals><Rules><Changed Source=\"[Cancel.Invoked]\"><Actions><Set Target=\"[SessionResult]\" Value=\"2\"/><Invoke Target=\"[AddInHost.ApplicationContext.CloseApplication]\"/><!-- Value is 2 for cancel. What else should it do?--></Actions></Changed><Changed Source=\"[Finish.Invoked]\"><Actions><Set Target=\"[SessionResult]\" Value=\"0\"/><Invoke Target=\"[AddInHost.ApplicationContext.CloseApplication]\"/><!-- Value is 0 for finish. What else should it do?--></Actions></Changed><!-- Accessibility defaults --><Default Target=\"[Accessible.Name]\" Value=\"Diagnostics\"/><Default Target=\"[Accessible.Role]\" Value=\"Pane\"/></Rules><Content><Panel Name=\"MainPage\" Margins=\"73, 139, 50, 0\"><Layout><FlowLayout Orientation=\"Vertical\"/></Layout><Children><Text Content=\"Welcome to NetOp for PBDA-KS Diagnostics Page!!\" Color=\"TVWhite\" Font=\"[TitleFont]\"/><Text Content=\"You will go back to the Tv Signal menu (no close MMI)\" Color=\"TVWhite\" Font=\"[TitleFont]\"/><Panel Name=\"Buttons\" Margins=\"200, 439, 50, 0\"><Layout><FlowLayout Orientation=\"Horizontal\"/></Layout><Children><!-- Finish button. --><me:SimpleButton Command=\"[Finish]\" Margins=\"100, 0, 0, 0\"/></Children></Panel></Children></Panel></Content></UI><!-- Simple button --><UI Name=\"SimpleButton\"><Properties><!-- Command is a required parameter. --><ICommand Name=\"Command\" ICommand=\"$Required\"/><!-- Background color properties. --><Color Name=\"BackgroundColor\" Color=\"DimGray\"/><Color Name=\"BackgroundFocusedColor\" Color=\"DarkGray\"/><Color Name=\"BackgroundHoverColor\" Color=\"PaleGoldenrod\"/><Color Name=\"BackgroundPressedColor\" Color=\"White\"/><!-- Label color properties. --><Color Name=\"LabelColor\" Color=\"White\"/><Color Name=\"LabelFocusedColor\" Color=\"White\"/><Color Name=\"LabelHoverColor\" Color=\"DimGray\"/><Color Name=\"LabelPressedColor\" Color=\"Black\"/><!-- Label font property. --><Font Name=\"LabelFont\" Font=\"Arial,20\"/></Properties><Locals><!-- React to \"click\" input. --><ClickHandler Name=\"Clicker\"/></Locals><Rules><!-- The command description is displayed by the text box. A  --><!-- binding is used in case the description changes at       --><!-- runtime.                                                 --><Binding Source=\"[Command.Description]\" Target=\"[Label.Content]\"/><Binding Source=\"[Command]\" Target=\"[Clicker.Command]\"/><!-- Change color on click. --><Condition Source=\"[Clicker.Clicking]\" SourceValue=\"true\"><Actions><Set Target=\"[Background.Content]\" Value=\"[BackgroundPressedColor]\"/><Set Target=\"[Label.Color]\" Value=\"[LabelPressedColor]\"/></Actions></Condition><!-- Change color on mouse focus. --><Condition Source=\"[Input.MouseFocus]\" SourceValue=\"true\"><Actions><Set Target=\"[Background.Content]\" Value=\"[BackgroundHoverColor]\"/><Set Target=\"[Label.Color]\" Value=\"[LabelHoverColor]\"/></Actions></Condition><!-- Change color on key focus. --><Condition Source=\"[Input.KeyFocus]\" SourceValue=\"true\"><Actions><Set Target=\"[Background.Content]\" Value=\"[BackgroundFocusedColor]\"/><Set Target=\"[Label.Color]\" Value=\"[LabelFocusedColor]\"/></Actions></Condition><!-- Various other state is hooked up as well (all of which are    --><!-- important for accessiblity aides.                             --><Binding Target=\"[Accessible.IsPressed]\"            Source=\"[Clicker.Clicking]\"/><Binding Target=\"[Accessible.IsFocusable]\"          Source=\"[Input.KeyInteractive]\"/><Binding Target=\"[Accessible.IsFocused]\"            Source=\"[Input.KeyFocus]\"/><Binding Target=\"[Accessible.Name]\"                 Source=\"[Command.Description]\"/><Default Target=\"[Accessible.DefaultActionCommand]\" Value=\"[Command]\"/><Default Target=\"[Accessible.DefaultAction]\"        Value=\"Press\"/><Default Target=\"[Accessible.Role]\"                 Value=\"PushButton\"/></Rules><Content><!-- Solid background color. --><ColorFill Name=\"Background\" Content=\"[BackgroundColor]\" Padding=\"5,5,5,5\"><Children><!-- The label to display. --><Text Name=\"Label\" Color=\"[LabelColor]\" Font=\"[LabelFont]\"/></Children></ColorFill></Content></UI></Mcml>");
                cNameStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulNameLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                cValueStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulValueLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                RtlStringCbCopy(cNameStr,ulNameLen + sizeof(WCHAR), L"PBDA://2734DC82-774D-4daa-A5AD-2838B5A58127/Microsoft.com/Resource/Diagnostics.mcml");
                RtlStringCbCopy(cValueStr,ulValueLen + sizeof(WCHAR), L"<Mcml xmlns=\"http://schemas.microsoft.com/2006/mcml\" xmlns:cor=\"assembly://MSCorlib/System\" xmlns:addin=\"assembly://Microsoft.MediaCenter/Microsoft.MediaCenter.Hosting\" xmlns:me=\"Me\"><!-- FirstRun page. Settings is same, except for text. --><UI Name=\"FirstRunPage\"><Properties><Font Name=\"TitleFont\" FontName=\"Segoe Media Center Semibold\" FontSize=\"20\" FontStyle=\"Bold\"/></Properties><Locals><addin:AddInHost Name=\"AddInHost\"/><Command Name=\"Cancel\" Description=\"Cancel\"/><Command Name=\"Finish\" Description=\"Finish\"/><cor:Int32 Name=\"SessionResult\" Int32=\"0\"/></Locals><Rules><Changed Source=\"[Cancel.Invoked]\"><Actions><Set Target=\"[SessionResult]\" Value=\"2\"/><Invoke Target=\"[AddInHost.ApplicationContext.CloseApplication]\"/><!-- Value is 2 for cancel. What else should it do?--></Actions></Changed><Changed Source=\"[Finish.Invoked]\"><Actions><Set Target=\"[SessionResult]\" Value=\"0\"/><Invoke Target=\"[AddInHost.ApplicationContext.CloseApplication]\"/><!-- Value is 0 for finish. What else should it do?--></Actions></Changed><!-- Accessibility defaults --><Default Target=\"[Accessible.Name]\" Value=\"Diagnostics\"/><Default Target=\"[Accessible.Role]\" Value=\"Pane\"/></Rules><Content><Panel Name=\"MainPage\" Margins=\"73, 139, 50, 0\"><Layout><FlowLayout Orientation=\"Vertical\"/></Layout><Children><Text Content=\"Welcome to NetOp for PBDA-KS Diagnostics Page!!\" Color=\"TVWhite\" Font=\"[TitleFont]\"/><Text Content=\"You will go back to the Tv Signal menu (no close MMI)\" Color=\"TVWhite\" Font=\"[TitleFont]\"/><Panel Name=\"Buttons\" Margins=\"200, 439, 50, 0\"><Layout><FlowLayout Orientation=\"Horizontal\"/></Layout><Children><!-- Finish button. --><me:SimpleButton Command=\"[Finish]\" Margins=\"100, 0, 0, 0\"/></Children></Panel></Children></Panel></Content></UI><!-- Simple button --><UI Name=\"SimpleButton\"><Properties><!-- Command is a required parameter. --><ICommand Name=\"Command\" ICommand=\"$Required\"/><!-- Background color properties. --><Color Name=\"BackgroundColor\" Color=\"DimGray\"/><Color Name=\"BackgroundFocusedColor\" Color=\"DarkGray\"/><Color Name=\"BackgroundHoverColor\" Color=\"PaleGoldenrod\"/><Color Name=\"BackgroundPressedColor\" Color=\"White\"/><!-- Label color properties. --><Color Name=\"LabelColor\" Color=\"White\"/><Color Name=\"LabelFocusedColor\" Color=\"White\"/><Color Name=\"LabelHoverColor\" Color=\"DimGray\"/><Color Name=\"LabelPressedColor\" Color=\"Black\"/><!-- Label font property. --><Font Name=\"LabelFont\" Font=\"Arial,20\"/></Properties><Locals><!-- React to \"click\" input. --><ClickHandler Name=\"Clicker\"/></Locals><Rules><!-- The command description is displayed by the text box. A  --><!-- binding is used in case the description changes at       --><!-- runtime.                                                 --><Binding Source=\"[Command.Description]\" Target=\"[Label.Content]\"/><Binding Source=\"[Command]\" Target=\"[Clicker.Command]\"/><!-- Change color on click. --><Condition Source=\"[Clicker.Clicking]\" SourceValue=\"true\"><Actions><Set Target=\"[Background.Content]\" Value=\"[BackgroundPressedColor]\"/><Set Target=\"[Label.Color]\" Value=\"[LabelPressedColor]\"/></Actions></Condition><!-- Change color on mouse focus. --><Condition Source=\"[Input.MouseFocus]\" SourceValue=\"true\"><Actions><Set Target=\"[Background.Content]\" Value=\"[BackgroundHoverColor]\"/><Set Target=\"[Label.Color]\" Value=\"[LabelHoverColor]\"/></Actions></Condition><!-- Change color on key focus. --><Condition Source=\"[Input.KeyFocus]\" SourceValue=\"true\"><Actions><Set Target=\"[Background.Content]\" Value=\"[BackgroundFocusedColor]\"/><Set Target=\"[Label.Color]\" Value=\"[LabelFocusedColor]\"/></Actions></Condition><!-- Various other state is hooked up as well (all of which are    --><!-- important for accessiblity aides.                             --><Binding Target=\"[Accessible.IsPressed]\"            Source=\"[Clicker.Clicking]\"/><Binding Target=\"[Accessible.IsFocusable]\"          Source=\"[Input.KeyInteractive]\"/><Binding Target=\"[Accessible.IsFocused]\"            Source=\"[Input.KeyFocus]\"/><Binding Target=\"[Accessible.Name]\"                 Source=\"[Command.Description]\"/><Default Target=\"[Accessible.DefaultActionCommand]\" Value=\"[Command]\"/><Default Target=\"[Accessible.DefaultAction]\"        Value=\"Press\"/><Default Target=\"[Accessible.Role]\"                 Value=\"PushButton\"/></Rules><Content><!-- Solid background color. --><ColorFill Name=\"Background\" Content=\"[BackgroundColor]\" Padding=\"5,5,5,5\"><Children><!-- The label to display. --><Text Name=\"Label\" Color=\"[LabelColor]\" Font=\"[LabelFont]\"/></Children></ColorFill></Content></UI></Mcml>");
               break;
           case 29://not visible to Name Index 
                ulNameLen = sizeof(L"PBDA://2734DC82-774D-4daa-A5AD-2838B5A58127/Microsoft.com/Resource/DiagnosticsReady.mcml");
                ulValueLen = sizeof(L"<Mcml xmlns=\"http://schemas.microsoft.com/2006/mcml\" xmlns:cor=\"assembly://MSCorlib/System\" xmlns:addin=\"assembly://Microsoft.MediaCenter/Microsoft.MediaCenter.Hosting\" xmlns:me=\"Me\"><!-- FirstRun page. Settings is same, except for text. --><UI Name=\"FirstRunPage\"><Properties><Font Name=\"TitleFont\" FontName=\"Segoe Media Center Semibold\" FontSize=\"20\" FontStyle=\"Bold\"/></Properties><Locals><addin:AddInHost Name=\"AddInHost\"/><Command Name=\"Cancel\" Description=\"Cancel\"/><Command Name=\"Finish\" Description=\"Finish\"/><cor:Int32 Name=\"SessionResult\" Int32=\"0\"/></Locals><Rules><Changed Source=\"[Cancel.Invoked]\"><Actions><Set Target=\"[SessionResult]\" Value=\"2\"/><Invoke Target=\"[AddInHost.ApplicationContext.CloseApplication]\"/><!-- Value is 2 for cancel. What else should it do?--></Actions></Changed><Changed Source=\"[Finish.Invoked]\"><Actions><Set Target=\"[SessionResult]\" Value=\"0\"/><Invoke Target=\"[AddInHost.ApplicationContext.CloseApplication]\"/><!-- Value is 0 for finish. What else should it do?--></Actions></Changed><!-- Accessibility defaults --><Default Target=\"[Accessible.Name]\" Value=\"Diagnostics\"/><Default Target=\"[Accessible.Role]\" Value=\"Pane\"/></Rules><Content><Panel Name=\"MainPage\" Margins=\"73, 139, 50, 0\"><Layout><FlowLayout Orientation=\"Vertical\"/></Layout><Children><Text Content=\"Welcome to NetOp for PBDA-KS Diagnostics Page!!\" Color=\"TVWhite\" Font=\"[TitleFont]\"/><Text Content=\"You will go back to the Tv Signal menu (no close MMI)\" Color=\"TVWhite\" Font=\"[TitleFont]\"/><Panel Name=\"Buttons\" Margins=\"200, 439, 50, 0\"><Layout><FlowLayout Orientation=\"Horizontal\"/></Layout><Children><!-- Cancel button. --><me:SimpleButton Command=\"[Cancel]\" Margins=\"0, 0, 0, 0\"/><!-- Finish button. --><me:SimpleButton Command=\"[Finish]\" Margins=\"100, 0, 0, 0\"/></Children></Panel></Children></Panel></Content></UI><!-- Simple button --><UI Name=\"SimpleButton\"><Properties><!-- Command is a required parameter. --><ICommand Name=\"Command\" ICommand=\"$Required\"/><!-- Background color properties. --><Color Name=\"BackgroundColor\" Color=\"DimGray\"/><Color Name=\"BackgroundFocusedColor\" Color=\"DarkGray\"/><Color Name=\"BackgroundHoverColor\" Color=\"PaleGoldenrod\"/><Color Name=\"BackgroundPressedColor\" Color=\"White\"/><!-- Label color properties. --><Color Name=\"LabelColor\" Color=\"White\"/><Color Name=\"LabelFocusedColor\" Color=\"White\"/><Color Name=\"LabelHoverColor\" Color=\"DimGray\"/><Color Name=\"LabelPressedColor\" Color=\"Black\"/><!-- Label font property. --><Font Name=\"LabelFont\" Font=\"Arial,20\"/></Properties><Locals><!-- React to \"click\" input. --><ClickHandler Name=\"Clicker\"/></Locals><Rules><!-- The command description is displayed by the text box. A  --><!-- binding is used in case the description changes at       --><!-- runtime.                                                 --><Binding Source=\"[Command.Description]\" Target=\"[Label.Content]\"/><Binding Source=\"[Command]\" Target=\"[Clicker.Command]\"/><!-- Change color on click. --><Condition Source=\"[Clicker.Clicking]\" SourceValue=\"true\"><Actions><Set Target=\"[Background.Content]\" Value=\"[BackgroundPressedColor]\"/><Set Target=\"[Label.Color]\" Value=\"[LabelPressedColor]\"/></Actions></Condition><!-- Change color on mouse focus. --><Condition Source=\"[Input.MouseFocus]\" SourceValue=\"true\"><Actions><Set Target=\"[Background.Content]\" Value=\"[BackgroundHoverColor]\"/><Set Target=\"[Label.Color]\" Value=\"[LabelHoverColor]\"/></Actions></Condition><!-- Change color on key focus. --><Condition Source=\"[Input.KeyFocus]\" SourceValue=\"true\"><Actions><Set Target=\"[Background.Content]\" Value=\"[BackgroundFocusedColor]\"/><Set Target=\"[Label.Color]\" Value=\"[LabelFocusedColor]\"/></Actions></Condition><!-- Various other state is hooked up as well (all of which are    --><!-- important for accessiblity aides.                             --><Binding Target=\"[Accessible.IsPressed]\"            Source=\"[Clicker.Clicking]\"/><Binding Target=\"[Accessible.IsFocusable]\"          Source=\"[Input.KeyInteractive]\"/><Binding Target=\"[Accessible.IsFocused]\"            Source=\"[Input.KeyFocus]\"/><Binding Target=\"[Accessible.Name]\"                 Source=\"[Command.Description]\"/><Default Target=\"[Accessible.DefaultActionCommand]\" Value=\"[Command]\"/><Default Target=\"[Accessible.DefaultAction]\"        Value=\"Press\"/><Default Target=\"[Accessible.Role]\"                 Value=\"PushButton\"/></Rules><Content><!-- Solid background color. --><ColorFill Name=\"Background\" Content=\"[BackgroundColor]\" Padding=\"5,5,5,5\"><Children><!-- The label to display. --><Text Name=\"Label\" Color=\"[LabelColor]\" Font=\"[LabelFont]\"/></Children></ColorFill></Content></UI></Mcml>");
                cNameStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulNameLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                cValueStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulValueLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                RtlStringCbCopy(cNameStr,ulNameLen + sizeof(WCHAR), L"PBDA://2734DC82-774D-4daa-A5AD-2838B5A58127/Microsoft.com/Resource/DiagnosticsReady.mcml");
                RtlStringCbCopy(cValueStr,ulValueLen + sizeof(WCHAR), L"<Mcml xmlns=\"http://schemas.microsoft.com/2006/mcml\" xmlns:cor=\"assembly://MSCorlib/System\" xmlns:addin=\"assembly://Microsoft.MediaCenter/Microsoft.MediaCenter.Hosting\" xmlns:me=\"Me\"><!-- FirstRun page. Settings is same, except for text. --><UI Name=\"FirstRunPage\"><Properties><Font Name=\"TitleFont\" FontName=\"Segoe Media Center Semibold\" FontSize=\"20\" FontStyle=\"Bold\"/></Properties><Locals><addin:AddInHost Name=\"AddInHost\"/><Command Name=\"Cancel\" Description=\"Cancel\"/><Command Name=\"Finish\" Description=\"Finish\"/><cor:Int32 Name=\"SessionResult\" Int32=\"0\"/></Locals><Rules><Changed Source=\"[Cancel.Invoked]\"><Actions><Set Target=\"[SessionResult]\" Value=\"2\"/><Invoke Target=\"[AddInHost.ApplicationContext.CloseApplication]\"/><!-- Value is 2 for cancel. What else should it do?--></Actions></Changed><Changed Source=\"[Finish.Invoked]\"><Actions><Set Target=\"[SessionResult]\" Value=\"0\"/><Invoke Target=\"[AddInHost.ApplicationContext.CloseApplication]\"/><!-- Value is 0 for finish. What else should it do?--></Actions></Changed><!-- Accessibility defaults --><Default Target=\"[Accessible.Name]\" Value=\"Diagnostics\"/><Default Target=\"[Accessible.Role]\" Value=\"Pane\"/></Rules><Content><Panel Name=\"MainPage\" Margins=\"73, 139, 50, 0\"><Layout><FlowLayout Orientation=\"Vertical\"/></Layout><Children><Text Content=\"Welcome to NetOp for PBDA-KS Diagnostics Page!!\" Color=\"TVWhite\" Font=\"[TitleFont]\"/><Text Content=\"You will go back to the Tv Signal menu (no close MMI)\" Color=\"TVWhite\" Font=\"[TitleFont]\"/><Panel Name=\"Buttons\" Margins=\"200, 439, 50, 0\"><Layout><FlowLayout Orientation=\"Horizontal\"/></Layout><Children><!-- Cancel button. --><me:SimpleButton Command=\"[Cancel]\" Margins=\"0, 0, 0, 0\"/><!-- Finish button. --><me:SimpleButton Command=\"[Finish]\" Margins=\"100, 0, 0, 0\"/></Children></Panel></Children></Panel></Content></UI><!-- Simple button --><UI Name=\"SimpleButton\"><Properties><!-- Command is a required parameter. --><ICommand Name=\"Command\" ICommand=\"$Required\"/><!-- Background color properties. --><Color Name=\"BackgroundColor\" Color=\"DimGray\"/><Color Name=\"BackgroundFocusedColor\" Color=\"DarkGray\"/><Color Name=\"BackgroundHoverColor\" Color=\"PaleGoldenrod\"/><Color Name=\"BackgroundPressedColor\" Color=\"White\"/><!-- Label color properties. --><Color Name=\"LabelColor\" Color=\"White\"/><Color Name=\"LabelFocusedColor\" Color=\"White\"/><Color Name=\"LabelHoverColor\" Color=\"DimGray\"/><Color Name=\"LabelPressedColor\" Color=\"Black\"/><!-- Label font property. --><Font Name=\"LabelFont\" Font=\"Arial,20\"/></Properties><Locals><!-- React to \"click\" input. --><ClickHandler Name=\"Clicker\"/></Locals><Rules><!-- The command description is displayed by the text box. A  --><!-- binding is used in case the description changes at       --><!-- runtime.                                                 --><Binding Source=\"[Command.Description]\" Target=\"[Label.Content]\"/><Binding Source=\"[Command]\" Target=\"[Clicker.Command]\"/><!-- Change color on click. --><Condition Source=\"[Clicker.Clicking]\" SourceValue=\"true\"><Actions><Set Target=\"[Background.Content]\" Value=\"[BackgroundPressedColor]\"/><Set Target=\"[Label.Color]\" Value=\"[LabelPressedColor]\"/></Actions></Condition><!-- Change color on mouse focus. --><Condition Source=\"[Input.MouseFocus]\" SourceValue=\"true\"><Actions><Set Target=\"[Background.Content]\" Value=\"[BackgroundHoverColor]\"/><Set Target=\"[Label.Color]\" Value=\"[LabelHoverColor]\"/></Actions></Condition><!-- Change color on key focus. --><Condition Source=\"[Input.KeyFocus]\" SourceValue=\"true\"><Actions><Set Target=\"[Background.Content]\" Value=\"[BackgroundFocusedColor]\"/><Set Target=\"[Label.Color]\" Value=\"[LabelFocusedColor]\"/></Actions></Condition><!-- Various other state is hooked up as well (all of which are    --><!-- important for accessiblity aides.                             --><Binding Target=\"[Accessible.IsPressed]\"            Source=\"[Clicker.Clicking]\"/><Binding Target=\"[Accessible.IsFocusable]\"          Source=\"[Input.KeyInteractive]\"/><Binding Target=\"[Accessible.IsFocused]\"            Source=\"[Input.KeyFocus]\"/><Binding Target=\"[Accessible.Name]\"                 Source=\"[Command.Description]\"/><Default Target=\"[Accessible.DefaultActionCommand]\" Value=\"[Command]\"/><Default Target=\"[Accessible.DefaultAction]\"        Value=\"Press\"/><Default Target=\"[Accessible.Role]\"                 Value=\"PushButton\"/></Rules><Content><!-- Solid background color. --><ColorFill Name=\"Background\" Content=\"[BackgroundColor]\" Padding=\"5,5,5,5\"><Children><!-- The label to display. --><Text Name=\"Label\" Color=\"[LabelColor]\" Font=\"[LabelFont]\"/></Children></ColorFill></Content></UI></Mcml>");
               break;
           case 30://not visible to Name Index 
                ulNameLen = sizeof(L"PBDA://2734DC82-774D-4daa-A5AD-2838B5A58127/Microsoft.com/Resource/testpage.mcml");
                ulValueLen = sizeof(L"<Mcml xmlns=\"http://schemas.microsoft.com/2006/mcml\" xmlns:cor=\"assembly://MSCorlib/System\" xmlns:addin=\"assembly://Microsoft.MediaCenter/Microsoft.MediaCenter.Hosting\" xmlns:me=\"Me\"><!-- FirstRun page. Settings is same, except for text. --><UI Name=\"FirstRunPage\"><Properties><Font Name=\"TitleFont\" FontName=\"Segoe Media Center Semibold\" FontSize=\"20\" FontStyle=\"Bold\"/></Properties><Locals><addin:AddInHost Name=\"AddInHost\"/><Command Name=\"Cancel\" Description=\"Cancel\"/><Command Name=\"Finish\" Description=\"Finish\"/><cor:Int32 Name=\"SessionResult\" Int32=\"0\"/></Locals><Rules><Changed Source=\"[Cancel.Invoked]\"><Actions><Set Target=\"[SessionResult]\" Value=\"2\"/><Invoke Target=\"[AddInHost.ApplicationContext.CloseApplication]\"/><!-- Value is 2 for cancel. What else should it do?--></Actions></Changed><Changed Source=\"[Finish.Invoked]\"><Actions><Set Target=\"[SessionResult]\" Value=\"0\"/><Invoke Target=\"[AddInHost.ApplicationContext.CloseApplication]\"/><!-- Value is 0 for finish. What else should it do?--></Actions></Changed><!-- Accessibility defaults --><Default Target=\"[Accessible.Name]\" Value=\"Diagnostics\"/><Default Target=\"[Accessible.Role]\" Value=\"Pane\"/></Rules><Content><Panel Name=\"MainPage\" Margins=\"73, 139, 50, 0\"><Layout><FlowLayout Orientation=\"Vertical\"/></Layout><Children><Text Content=\"Welcome to NetOp for PBDA-KS Testpage at Live TV!!\" Color=\"TVWhite\" Font=\"[TitleFont]\"/><Text Content=\"You will go back to the Tv channel(no close MMI)\" Color=\"TVWhite\" Font=\"[TitleFont]\"/><Panel Name=\"Buttons\" Margins=\"200, 439, 50, 0\"><Layout><FlowLayout Orientation=\"Horizontal\"/></Layout><Children><!-- Finish button. --><me:SimpleButton Command=\"[Finish]\" Margins=\"100, 0, 0, 0\"/></Children></Panel></Children></Panel></Content></UI><!-- Simple button --><UI Name=\"SimpleButton\"><Properties><!-- Command is a required parameter. --><ICommand Name=\"Command\" ICommand=\"$Required\"/><!-- Background color properties. --><Color Name=\"BackgroundColor\" Color=\"DimGray\"/><Color Name=\"BackgroundFocusedColor\" Color=\"DarkGray\"/><Color Name=\"BackgroundHoverColor\" Color=\"PaleGoldenrod\"/><Color Name=\"BackgroundPressedColor\" Color=\"White\"/><!-- Label color properties. --><Color Name=\"LabelColor\" Color=\"White\"/><Color Name=\"LabelFocusedColor\" Color=\"White\"/><Color Name=\"LabelHoverColor\" Color=\"DimGray\"/><Color Name=\"LabelPressedColor\" Color=\"Black\"/><!-- Label font property. --><Font Name=\"LabelFont\" Font=\"Arial,20\"/></Properties><Locals><!-- React to \"click\" input. --><ClickHandler Name=\"Clicker\"/></Locals><Rules><!-- The command description is displayed by the text box. A  --><!-- binding is used in case the description changes at       --><!-- runtime.                                                 --><Binding Source=\"[Command.Description]\" Target=\"[Label.Content]\"/><Binding Source=\"[Command]\" Target=\"[Clicker.Command]\"/><!-- Change color on click. --><Condition Source=\"[Clicker.Clicking]\" SourceValue=\"true\"><Actions><Set Target=\"[Background.Content]\" Value=\"[BackgroundPressedColor]\"/><Set Target=\"[Label.Color]\" Value=\"[LabelPressedColor]\"/></Actions></Condition><!-- Change color on mouse focus. --><Condition Source=\"[Input.MouseFocus]\" SourceValue=\"true\"><Actions><Set Target=\"[Background.Content]\" Value=\"[BackgroundHoverColor]\"/><Set Target=\"[Label.Color]\" Value=\"[LabelHoverColor]\"/></Actions></Condition><!-- Change color on key focus. --><Condition Source=\"[Input.KeyFocus]\" SourceValue=\"true\"><Actions><Set Target=\"[Background.Content]\" Value=\"[BackgroundFocusedColor]\"/><Set Target=\"[Label.Color]\" Value=\"[LabelFocusedColor]\"/></Actions></Condition><!-- Various other state is hooked up as well (all of which are    --><!-- important for accessiblity aides.                             --><Binding Target=\"[Accessible.IsPressed]\"            Source=\"[Clicker.Clicking]\"/><Binding Target=\"[Accessible.IsFocusable]\"          Source=\"[Input.KeyInteractive]\"/><Binding Target=\"[Accessible.IsFocused]\"            Source=\"[Input.KeyFocus]\"/><Binding Target=\"[Accessible.Name]\"                 Source=\"[Command.Description]\"/><Default Target=\"[Accessible.DefaultActionCommand]\" Value=\"[Command]\"/><Default Target=\"[Accessible.DefaultAction]\"        Value=\"Press\"/><Default Target=\"[Accessible.Role]\"                 Value=\"PushButton\"/></Rules><Content><!-- Solid background color. --><ColorFill Name=\"Background\" Content=\"[BackgroundColor]\" Padding=\"5,5,5,5\"><Children><!-- The label to display. --><Text Name=\"Label\" Color=\"[LabelColor]\" Font=\"[LabelFont]\"/></Children></ColorFill></Content></UI></Mcml>");
                cNameStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulNameLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                cValueStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulValueLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                RtlStringCbCopy(cNameStr,ulNameLen + sizeof(WCHAR), L"PBDA://2734DC82-774D-4daa-A5AD-2838B5A58127/Microsoft.com/Resource/testpage.mcml");
                RtlStringCbCopy(cValueStr,ulValueLen + sizeof(WCHAR), L"<Mcml xmlns=\"http://schemas.microsoft.com/2006/mcml\" xmlns:cor=\"assembly://MSCorlib/System\" xmlns:addin=\"assembly://Microsoft.MediaCenter/Microsoft.MediaCenter.Hosting\" xmlns:me=\"Me\"><!-- FirstRun page. Settings is same, except for text. --><UI Name=\"FirstRunPage\"><Properties><Font Name=\"TitleFont\" FontName=\"Segoe Media Center Semibold\" FontSize=\"20\" FontStyle=\"Bold\"/></Properties><Locals><addin:AddInHost Name=\"AddInHost\"/><Command Name=\"Cancel\" Description=\"Cancel\"/><Command Name=\"Finish\" Description=\"Finish\"/><cor:Int32 Name=\"SessionResult\" Int32=\"0\"/></Locals><Rules><Changed Source=\"[Cancel.Invoked]\"><Actions><Set Target=\"[SessionResult]\" Value=\"2\"/><Invoke Target=\"[AddInHost.ApplicationContext.CloseApplication]\"/><!-- Value is 2 for cancel. What else should it do?--></Actions></Changed><Changed Source=\"[Finish.Invoked]\"><Actions><Set Target=\"[SessionResult]\" Value=\"0\"/><Invoke Target=\"[AddInHost.ApplicationContext.CloseApplication]\"/><!-- Value is 0 for finish. What else should it do?--></Actions></Changed><!-- Accessibility defaults --><Default Target=\"[Accessible.Name]\" Value=\"Diagnostics\"/><Default Target=\"[Accessible.Role]\" Value=\"Pane\"/></Rules><Content><Panel Name=\"MainPage\" Margins=\"73, 139, 50, 0\"><Layout><FlowLayout Orientation=\"Vertical\"/></Layout><Children><Text Content=\"Welcome to NetOp for PBDA-KS Testpage at Live TV!!\" Color=\"TVWhite\" Font=\"[TitleFont]\"/><Text Content=\"You will go back to the Tv channel(no close MMI)\" Color=\"TVWhite\" Font=\"[TitleFont]\"/><Panel Name=\"Buttons\" Margins=\"200, 439, 50, 0\"><Layout><FlowLayout Orientation=\"Horizontal\"/></Layout><Children><!-- Finish button. --><me:SimpleButton Command=\"[Finish]\" Margins=\"100, 0, 0, 0\"/></Children></Panel></Children></Panel></Content></UI><!-- Simple button --><UI Name=\"SimpleButton\"><Properties><!-- Command is a required parameter. --><ICommand Name=\"Command\" ICommand=\"$Required\"/><!-- Background color properties. --><Color Name=\"BackgroundColor\" Color=\"DimGray\"/><Color Name=\"BackgroundFocusedColor\" Color=\"DarkGray\"/><Color Name=\"BackgroundHoverColor\" Color=\"PaleGoldenrod\"/><Color Name=\"BackgroundPressedColor\" Color=\"White\"/><!-- Label color properties. --><Color Name=\"LabelColor\" Color=\"White\"/><Color Name=\"LabelFocusedColor\" Color=\"White\"/><Color Name=\"LabelHoverColor\" Color=\"DimGray\"/><Color Name=\"LabelPressedColor\" Color=\"Black\"/><!-- Label font property. --><Font Name=\"LabelFont\" Font=\"Arial,20\"/></Properties><Locals><!-- React to \"click\" input. --><ClickHandler Name=\"Clicker\"/></Locals><Rules><!-- The command description is displayed by the text box. A  --><!-- binding is used in case the description changes at       --><!-- runtime.                                                 --><Binding Source=\"[Command.Description]\" Target=\"[Label.Content]\"/><Binding Source=\"[Command]\" Target=\"[Clicker.Command]\"/><!-- Change color on click. --><Condition Source=\"[Clicker.Clicking]\" SourceValue=\"true\"><Actions><Set Target=\"[Background.Content]\" Value=\"[BackgroundPressedColor]\"/><Set Target=\"[Label.Color]\" Value=\"[LabelPressedColor]\"/></Actions></Condition><!-- Change color on mouse focus. --><Condition Source=\"[Input.MouseFocus]\" SourceValue=\"true\"><Actions><Set Target=\"[Background.Content]\" Value=\"[BackgroundHoverColor]\"/><Set Target=\"[Label.Color]\" Value=\"[LabelHoverColor]\"/></Actions></Condition><!-- Change color on key focus. --><Condition Source=\"[Input.KeyFocus]\" SourceValue=\"true\"><Actions><Set Target=\"[Background.Content]\" Value=\"[BackgroundFocusedColor]\"/><Set Target=\"[Label.Color]\" Value=\"[LabelFocusedColor]\"/></Actions></Condition><!-- Various other state is hooked up as well (all of which are    --><!-- important for accessiblity aides.                             --><Binding Target=\"[Accessible.IsPressed]\"            Source=\"[Clicker.Clicking]\"/><Binding Target=\"[Accessible.IsFocusable]\"          Source=\"[Input.KeyInteractive]\"/><Binding Target=\"[Accessible.IsFocused]\"            Source=\"[Input.KeyFocus]\"/><Binding Target=\"[Accessible.Name]\"                 Source=\"[Command.Description]\"/><Default Target=\"[Accessible.DefaultActionCommand]\" Value=\"[Command]\"/><Default Target=\"[Accessible.DefaultAction]\"        Value=\"Press\"/><Default Target=\"[Accessible.Role]\"                 Value=\"PushButton\"/></Rules><Content><!-- Solid background color. --><ColorFill Name=\"Background\" Content=\"[BackgroundColor]\" Padding=\"5,5,5,5\"><Children><!-- The label to display. --><Text Name=\"Label\" Color=\"[LabelColor]\" Font=\"[LabelFont]\"/></Children></ColorFill></Content></UI></Mcml>");
               break;
            default:
                ulNameLen = 0;
                ulValueLen = 0;
                cNameStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulNameLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                cValueStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulValueLen + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                cNameStr[0] = 0;
                cValueStr[0] = 0;
                break;
        }//switch
        m_GpnvsTable[i][0] = cNameStr;
        m_GpnvsTable[i][1] = cValueStr;
    }//end for

    m_AvailableGuideDataPercentage = 100;
    m_CurrentScanningFrequency = 0;
    m_ScanEstimateTime = 120;
    m_pGuideServices = NULL;
 }

CFilter::~CFilter()
{
    PDEVICE_OBJECT   pDeviceObject = NULL;
    PFILE_OBJECT     pFileObject = NULL;
    HANDLE           hFileHandle = 0;
    KIRQL            Irql;

    KeAcquireSpinLock( &this->spinLock, &Irql);

    if (pDevObject)
    {
        pDeviceObject = this->pDevObject;
        this->pDevObject = NULL;
    }
    if(pFiObject)
    {
        pFileObject = this->pFiObject;
        this->pFiObject = NULL;
    }
    if (m_pGuideServices != NULL)
    {
        ExFreePoolWithTag(m_pGuideServices, MS_SAMPLE_TUNER_POOL_TAG);
        m_pGuideServices = NULL;
    }
    KeReleaseSpinLock( &this->spinLock, Irql);

    if (pDeviceObject)
    {
        ObDereferenceObject(pDeviceObject);
        pDeviceObject = NULL;
    }

    if (pFileObject)
    {
        ObDereferenceObject(pFileObject);
        pFileObject = NULL;
    }
    if(hFileHandle)
    {
        ZwClose( hFileHandle);
        hFileHandle = 0;
    }

    int i = 0;
    int j = 0;
    for (i = 0; i < GPNVS_NUMBER_OF_ROWS; i++)
    {
        for (j = 0; j < GPNVS_NUMBER_OF_COLUMNS; j++)
        {
            if (m_GpnvsTable[i][j] != NULL)
            {
                ExFreePoolWithTag(m_GpnvsTable[i][j], MS_SAMPLE_TUNER_POOL_TAG); 
                m_GpnvsTable[i][j] = NULL;
            }
        }
    }
}

/*
** Create() method of the CFilter class
**
**    Creates the filter object,
**    associates it with the device object, and 
**    initializes member variables for it.
**
*/
STDMETHODIMP_(NTSTATUS)
CFilter::Create(
    IN OUT PKSFILTER pKSFilter,
    IN PIRP Irp
    )
{
    NTSTATUS    Status = STATUS_SUCCESS;
    PKSDEVICE   pKSDevice = NULL;
    CDevice *   pDevice = NULL;

    UNREFERENCED_PARAMETER(Irp);
    
    _DbgPrintF(DEBUGLVL_VERBOSE,("FilterCreate"));

    ASSERT(pKSFilter);
    ASSERT(Irp);


    //  Create a filter object for the filter instance.
    //
    CFilter* pFilter = new(NonPagedPool,MS_SAMPLE_TUNER_POOL_TAG) CFilter; // Tags the allocated memory
    if (!pFilter)
    {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto errExit;
    }
    //  Link the filter context to the passed in pointer to the KSFILTER structure.
    //
    pKSFilter->Context = pFilter;

    //  Point to the KS device object for this filter.
    //
    pKSDevice = KsFilterGetDevice( pKSFilter);
    ASSERT( pKSDevice);
    if (!pKSDevice)
    {
        Status = STATUS_DEVICE_NOT_CONNECTED;
        goto errExit;
    }

    //  Get the device object from the retrieved pointer to the KSDevice for this filter.
    //
    pDevice = reinterpret_cast<CDevice *>(pKSDevice->Context);
    ASSERT( pDevice);
    if (!pDevice)
    {
        Status = STATUS_DEVICE_NOT_CONNECTED;
        goto errExit;
    }

    //  Link the filter context to the device context.
    //  That is, set the filter's device pointer data member to the obtained device pointer.
    //
    pFilter->m_pDevice = pDevice;
    KeInitializeSpinLock(&pFilter->m_Lock);
    pFilter->m_FilterEventsManager.Initialize((PVOID)pFilter);
    pFilter->m_FilterDebugEventsManager.Initialize((PVOID)pFilter);
    pFilter->m_FilterGPNVEventsManager.Initialize((PVOID)pFilter);
    pDevice->InsertFilter(pFilter);


    //  Initialize member variables.
    //
    pFilter->m_KsState = KSSTATE_STOP;
    pFilter->m_BdaChangeState = BDA_CHANGES_COMPLETE;
    pFilter->m_ulResourceID = 0;

    pFilter->m_CurResource.ulPolarity = 0;
    pFilter->m_CurResource.ulDiseqC = 0;
    pFilter->m_CurResource.ulPLPNumber = 0;

#if ATSC_RECEIVER
    //  Configure the initial resource for ATSC reception of channel 39.
    pFilter->m_CurResource.ulCarrierFrequency = 621250L;
    pFilter->m_CurResource.ulFrequencyMultiplier = 1000;

#elif QAM_RECEIVER
    //  Configure the initial resource for QAM reception of channel 39.
    pFilter->m_CurResource.ulCarrierFrequency = 621250L;
    pFilter->m_CurResource.ulFrequencyMultiplier = 1000;

#elif DVBT_RECEIVER
    //  Configure the initial resource for DVBT .
    pFilter->m_CurResource.ulCarrierFrequency = 754000L;
    pFilter->m_CurResource.ulFrequencyMultiplier = 1000;

#elif DVBC_RECEIVER
    //  Configure the initial resource for DVBC .
    pFilter->m_CurResource.ulCarrierFrequency = 586000L;
    pFilter->m_CurResource.ulFrequencyMultiplier = 1000;

#elif DVBS_RECEIVER
    //  Configure the initial resource for DVB-S .
    pFilter->m_CurResource.ulCarrierFrequency = 12480000L;
    pFilter->m_CurResource.ulFrequencyMultiplier = 1000;

#elif ISDBT_RECEIVER
    //  Configure the initial resource for ISDB-T .
    pFilter->m_CurResource.ulCarrierFrequency = 171250L;
    pFilter->m_CurResource.ulFrequencyMultiplier = 1000;

#elif ISDBS_RECEIVER
    //  Configure the initial resource for ISDB-S .
    pFilter->m_CurResource.ulCarrierFrequency = 11261000L;
    pFilter->m_CurResource.ulFrequencyMultiplier = 1000;

#elif defined(DMBTH_RECEIVER)    ///DMBTH ADD
    //  Configure the initial resource for DMBTH .
    pFilter->m_CurResource.ulCarrierFrequency = 754000L;
    pFilter->m_CurResource.ulFrequencyMultiplier = 1000;
#elif defined(NETOP_RECEIVER) || defined(PBDAT_RECEIVER)
    //  Configure the initial resource for Netops. Simply choose any frequency
    pFilter->m_CurResource.ulCarrierFrequency = 171250L;
    pFilter->m_CurResource.ulFrequencyMultiplier = 1000;

    WCHAR * cValueStr;
    int i;

        for (i = 0; i < GPNVS_NUMBER_OF_ROWS; i++)
        {
            if (wcscmp( L"PBDA:/NV/Variable/Tuner Types", pFilter->m_GpnvsTable[i][0]) == 0) //find the Tuner Types
            {

                UNICODE_STRING TunerTypeString;
                TunerTypeString.Length = 0;
                TunerTypeString.Buffer = (LPWSTR)ExAllocatePoolWithTag(NonPagedPool,
                                              90, 
                                              MS_SAMPLE_TUNER_POOL_TAG
                                              );
                TunerTypeString.MaximumLength = 90;

                NTSTATUS statusreturn = pDevice->GetDeviceParameterString(L"PbdaTunerType", &TunerTypeString); //get the value from the registry
                if(NT_SUCCESS(statusreturn))
                {
                    cValueStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, TunerTypeString.Length + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                    RtlStringCbCopy(cValueStr,TunerTypeString.Length + sizeof(WCHAR), TunerTypeString.Buffer);
                    if (pFilter->m_GpnvsTable[i][1] != NULL)//delete the old value
                    {
                        ExFreePoolWithTag(pFilter->m_GpnvsTable[i][1], MS_SAMPLE_TUNER_POOL_TAG); 
                    }
                    pFilter->m_GpnvsTable[i][1] = cValueStr; //set the new value
                }
                else
                {
                    DbgPrint("TunerTypeString not in registry we use the default PBDA type");
                }

                if (TunerTypeString.Buffer)
                {
                    ExFreePoolWithTag(TunerTypeString.Buffer, MS_SAMPLE_TUNER_POOL_TAG);
                }
            }
            if (wcscmp( L"PBDA:/Microsoft.com/NV/Variable/MCE/Source Types", pFilter->m_GpnvsTable[i][0]) == 0) //find the Source Types variable
            {
                ULONG ulServiceFileSize = 0;
                ULONG ulSourceTypesBufferSize = 0; 

                char *pSourceTypesStr = NULL;

                pDevice->ReadGuideServiceListFromFile(NULL, &ulServiceFileSize);
                if (ulServiceFileSize != 0)
                {
                    if (pFilter->m_pGuideServices == NULL)
                    {
                        Status = pDevice->ReadGuideServiceListFromFile(&pFilter->m_pGuideServices, &ulServiceFileSize);
                    }
                    //read the Source Types Value, if available and replace GPNV default
                    char *pServicesFileStr = (char *)pFilter->m_pGuideServices;
                    char SourceTypesStr[] = "<SourceTypes";
                    pFilter->FindXMLString(pServicesFileStr, ulServiceFileSize, SourceTypesStr, &pSourceTypesStr, &ulSourceTypesBufferSize);
                    if (ulSourceTypesBufferSize && (ulSourceTypesBufferSize < 0xFFFF))
                    {
                        ulSourceTypesBufferSize++; //add the \0 to the full length
                        if (pFilter->m_GpnvsTable[i][1] != NULL)//delete the old value
                        {
                            ExFreePoolWithTag(pFilter->m_GpnvsTable[i][1], MS_SAMPLE_TUNER_POOL_TAG);
                            pFilter->m_GpnvsTable[i][1] = NULL;
                        }
                        //allocate a new string and copy it into
                        cValueStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulSourceTypesBufferSize * sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
                        if (cValueStr)
                        {
                            USHORT k = 0;
                            for (k=0; k < (ulSourceTypesBufferSize - 1); k++)
                            {
                                cValueStr[k] = pSourceTypesStr[k]; //copy the ANSI chars to WCHAR
                            }
                            cValueStr[ulSourceTypesBufferSize - 1] = 0; //terminate the string with /0
                                
                            DbgPrint("Initalize Source Types to %S",cValueStr);
                            pFilter->m_GpnvsTable[i][1] = cValueStr; //set the new value
                        }
                    }
                    else
                    {
                        DbgPrint("Source Types not found in GuideServiceFile");
                    }
                }
            }
        }//end for

        pFilter->m_guideDataType = PBDA_GUIDE_DATATYPE; //required to load services via the GDDS interface

        UNICODE_STRING GuideDataTypeString;
        GuideDataTypeString.Length = 0;
        GuideDataTypeString.Buffer = (LPWSTR)ExAllocatePoolWithTag(NonPagedPool,
                                      90, 
                                      MS_SAMPLE_TUNER_POOL_TAG
                                      );
        GuideDataTypeString.MaximumLength = 90;
        NTSTATUS statusreturn = pDevice->GetDeviceParameterString(L"GuideDataType", &GuideDataTypeString);
        if(NT_SUCCESS(statusreturn) && GuideDataTypeString.Buffer)
        {
            DbgPrint("GuideDataType = %S",GuideDataTypeString.Buffer);
            statusreturn = RtlGUIDFromString(&GuideDataTypeString, &pFilter->m_guideDataType);
            if (!NT_SUCCESS(statusreturn))
            {
                DbgPrint("CFilter::GuideDataType String cannot be converted to a GUID!!");
            }
        }
        else
        {
            DbgPrint("GuideDataType not in registry we use the default PBDA type");
        }

        if (GuideDataTypeString.Buffer)
        {
            ExFreePoolWithTag(GuideDataTypeString.Buffer, MS_SAMPLE_TUNER_POOL_TAG);
        }

#endif


#if ATSC_RECEIVER
    pFilter->m_CurResource.guidDemodulator = KSNODE_BDA_8VSB_DEMODULATOR;
#elif QAM_RECEIVER
    pFilter->m_CurResource.guidDemodulator = KSNODE_BDA_QAM_DEMODULATOR;
#elif DVBT_RECEIVER
    pFilter->m_CurResource.guidDemodulator = KSNODE_BDA_COFDM_DEMODULATOR;
#elif DVBC_RECEIVER
    pFilter->m_CurResource.guidDemodulator = KSNODE_BDA_QAM_DEMODULATOR;
#elif DVBS_RECEIVER
    pFilter->m_CurResource.guidDemodulator = KSNODE_BDA_QPSK_DEMODULATOR;
#elif ISDBT_RECEIVER
    pFilter->m_CurResource.guidDemodulator = KSNODE_BDA_ISDB_T_DEMODULATOR;
#elif ISDBS_RECEIVER
    pFilter->m_CurResource.guidDemodulator = KSNODE_BDA_ISDB_S_DEMODULATOR;
#elif NETOP_RECEIVER
    pFilter->m_CurResource.guidDemodulator = KSNODE_BDA_PBDA_TUNER;
#elif PBDAT_RECEIVER
    pFilter->m_CurResource.guidDemodulator = KSNODE_BDA_COFDM_DEMODULATOR;
#elif defined(DMBTH_RECEIVER)    //DMBTH ADD
    pFilter->m_CurResource.guidDemodulator = KSNODE_BDA_COFDM_DEMODULATOR;
#endif
    pFilter->m_fResourceAcquired = FALSE;

    //  Call the BDA support library to initialize the
    //  filter instance with the default template topology.
    //
    Status = BdaInitFilter( pKSFilter, &BdaFilterTemplate);
    if (NT_ERROR( Status))
    {
        goto errExit;
    }

#ifdef NO_NETWORK_PROVIDER
    //
    //  This code can be used for initial testing of a filter when
    //  a network provider can't be used to configure the filter.
    //  This situation may arise when the receiver topology includes
    //  filters that have not yet been written or when a new network
    //  type does not yet have a network provider implementation.
    //
    //  This code should NOT be used when a driver is delivered into
    //  a working BDA receiver solution.
    //

    //  Create the transport output pin
    //
    Status = BdaCreatePin( pKSFilter, 
                           PIN_TYPE_TRANSPORT, 
                           &ulPinId
                           );
    if (!NT_SUCCESS(Status))
    {
        goto errExit;
    }
    
    //  Create the topology between the antenna input and
    //  transport output pins.
    //
    //  Note that the antenna input pin was automatically created
    //  because it was included in the pin descriptor list of the
    //  initial filter descriptor passed to BdaCreateFilterFactory.
    //
    Status = BdaCreateTopology( pKSFilter, 
                                INITIAL_ANNTENNA_PIN_ID, 
                                ulPinId
                                );
    if (!NT_SUCCESS(Status))
    {
        goto errExit;
    }

#endif // NO_NETWORK_PROVIDER

 exit:
    return Status;

 errExit:
    if (pFilter)
    {
        delete pFilter;
    }
    pKSFilter->Context = NULL;

    goto exit;
}


/*
** FilterClose() method of the CFilter class
**
**    Deletes the previously created filter object.
**
*/
STDMETHODIMP_(NTSTATUS)
CFilter::FilterClose(
    IN OUT PKSFILTER pKSFilter,
    IN PIRP Irp
    )
{
    _DbgPrintF(DEBUGLVL_VERBOSE,("FilterClose"));
    UNREFERENCED_PARAMETER(Irp);

    ASSERT(pKSFilter);
    ASSERT(Irp);

    CFilter* pFilter = reinterpret_cast<CFilter*>(pKSFilter->Context);
    ASSERT(pFilter);
    pFilter->m_pDevice->RemoveFilter(pFilter);

    delete pFilter;

    return STATUS_SUCCESS;
}

/*
** StartChanges() method of the CFilter class
**
**    Puts the filter into change state.  All changes to BDA topology
**    and properties changed after this will be in effect only after
**    a call to the CFilter::CommitChanges() method.
**
*/
NTSTATUS
CFilter::StartChanges(
                      IN PIRP         pIrp,
                      IN PKSMETHOD    pKSMethod,
                      OPTIONAL PVOID  pvIgnored
                      )
{
    NTSTATUS        Status = STATUS_SUCCESS;
    CFilter *       pFilter;

    UNREFERENCED_PARAMETER(pKSMethod);
    UNREFERENCED_PARAMETER(pvIgnored);

    ASSERT( pIrp);
    ASSERT( pKSMethod);


    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::StartChanges() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    //  Call the BDA support library to 
    //  reset any pending BDA topolgoy changes.
    //
    Status = BdaStartChanges( pIrp);
    if (NT_SUCCESS( Status))
    {
        //  Reset any pending resource changes.
        //
        pFilter->m_NewResource = pFilter->m_CurResource;
        pFilter->m_BdaChangeState = BDA_CHANGES_COMPLETE;
    }

    return Status;
}


/*
** CheckChanges() method of the CFilter class
**
**    Checks the changes to BDA interfaces that have occured since the
**    last call to the CFilter::StartChanges() method.  Returns the identical 
**    result that the CFilter::CommitChanges() method returns.
**
*/
NTSTATUS
CFilter::CheckChanges(
                      IN PIRP         pIrp,
                      IN PKSMETHOD    pKSMethod,
                      OPTIONAL PVOID  pvIgnored
                      )
{
    NTSTATUS            Status = STATUS_SUCCESS;
    CFilter *           pFilter;

    UNREFERENCED_PARAMETER(pKSMethod);
    UNREFERENCED_PARAMETER(pvIgnored);
    
    ASSERT( pIrp);
    ASSERT( pKSMethod);

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::CheckChanges() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    //  Call the BDA support library to 
    //  verify a new set of BDA topology changes.
    //
    Status = BdaCheckChanges( pIrp);
    if (NT_SUCCESS( Status))
    {
        //
        //  Validate the new resource list here.
        //  In this driver the new resource list is always valid.
        //
    }

    return Status;
}
/*
** CommitChanges() method of the CFilter class
**
**    Checks and commits the changes to BDA interfaces that have occured since the
**    last call to the CFilter::StartChanges() method.  
**
*/
NTSTATUS
CFilter::CommitChanges(
                       IN PIRP         pIrp,
                       IN PKSMETHOD    pKSMethod,
                       OPTIONAL PVOID  pvIgnored
                       )
{
    NTSTATUS        Status = STATUS_SUCCESS;
    CFilter *       pFilter;
    PKSFILTER       pksFilter = NULL;

    UNREFERENCED_PARAMETER(pKSMethod);
    UNREFERENCED_PARAMETER(pvIgnored);

    ASSERT( pIrp);
    ASSERT( pKSMethod);

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::CommitChanges() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pksFilter = KsGetFilterFromIrp(pIrp);
    ASSERT( pksFilter);
    pFilter = reinterpret_cast<CFilter *>(pksFilter->Context);
    ASSERT( pFilter);

    //
    //  Validate the new resource list here.
    //  In this driver the new resource list is always valid.
    //

    BOOL fUpdateFrequency = false;
    if (pFilter->m_CurResource.ulCarrierFrequency != pFilter->m_NewResource.ulCarrierFrequency)
        fUpdateFrequency = true;

    if (pFilter->m_CurResource.ulFrequencyMultiplier != pFilter->m_NewResource.ulFrequencyMultiplier)
        fUpdateFrequency = true;

    if (pFilter->m_CurResource.ulPolarity != pFilter->m_NewResource.ulPolarity)
        fUpdateFrequency = true;

    if (pFilter->m_CurResource.ulDiseqC != pFilter->m_NewResource.ulDiseqC)
        fUpdateFrequency = true;

    if (pFilter->m_CurResource.ulPLPNumber != pFilter->m_NewResource.ulPLPNumber)
        fUpdateFrequency = true;

    //  Mark the changes as having been made.
    //
    pFilter->m_CurResource = pFilter->m_NewResource;
    pFilter->m_BdaChangeState = BDA_CHANGES_COMPLETE;

    
    if (fUpdateFrequency)
    {
        ULONG ScaledFrequency;
        //calculated into kHz 
        ULARGE_INTEGER ul64ScaledFrequency;
        ULARGE_INTEGER ul64CarrierFrequency;
        ULARGE_INTEGER ul64Multiplier;
        ul64Multiplier.QuadPart = pFilter->m_NewResource.ulFrequencyMultiplier;
        ul64CarrierFrequency.LowPart = pFilter->m_NewResource.ulCarrierFrequency;
        ul64CarrierFrequency.HighPart = 0;
        ul64ScaledFrequency.QuadPart = ul64CarrierFrequency.QuadPart * ul64Multiplier.QuadPart;
        ul64ScaledFrequency.QuadPart /= 1000;
        ScaledFrequency = ul64ScaledFrequency.LowPart;
        if (pFilter->m_NewResource.fDiseqCEnable == FALSE)
        {
            pFilter->m_NewResource.ulDiseqC = 0; //if DiseqC is disabled the LNB Source is 0 (vs LNB_A=1)
        }

        if ((pFilter->m_NewResource.ulPolarity > 0) || (pFilter->m_NewResource.ulDiseqC > 0)) 
        {
            //for DVB-S we extend the frequency value to Freq[Polarisation][DiseqC]
            ScaledFrequency = ScaledFrequency * 10 + pFilter->m_NewResource.ulPolarity;
            ScaledFrequency = ScaledFrequency * 10 + pFilter->m_NewResource.ulDiseqC;
        }
		
        if ((pFilter->m_NewResource.ulPLPNumber > 0) && (pFilter->m_NewResource.ulPLPNumber <= 0xFF))
        {
            ScaledFrequency += (pFilter->m_NewResource.ulPLPNumber & 0xFF);      
        }

		// talk to the device instance and push the new frequency.
        ASSERT(pFilter->m_pDevice);
        Status = pFilter->m_pDevice->SetFrequency( &ScaledFrequency );

        
        //
        // Push the new tuning information to the capture filter's connection.
        //
        PIKSCONTROL control;
        KsAcquireControl(pksFilter);
        PKSPIN pKSPin = KsFilterGetFirstChildPin(pksFilter, 0);
        ASSERT( pKSPin );
        Status = KsPinGetConnectedFilterInterface(pKSPin,&IID_IKsControl,(PVOID *) &control);
        if (NT_SUCCESS(Status)) 
        {
            ULONG bytesReturned;
            KSPROPERTY Property;

            Property.Set = KSNAME_CaptureNodePropertySet;
            Property.Id = KSPROPERTY_CAPTURENODE_SETFREQ;
            Property.Flags = KSPROPERTY_TYPE_SET;

            Status = 
                control->KsProperty(
                                    &Property,
                                    sizeof(KSPROPERTY),
                                    &ScaledFrequency,
                                    sizeof(ULONG),
                                    &bytesReturned);
            control->Release();
        }
        KsReleaseControl(pksFilter);
    }

    if (pFilter->m_KsState != KSSTATE_STOP)
    {
        //  Commit the resources on the underlying device
        //
        Status = pFilter->AcquireResources( );
        ASSERT( NT_SUCCESS( Status));
    }

    //  Call the BDA support library to 
    //  commit a new set of BDA topology changes.
    //
    Status = BdaCommitChanges( pIrp);
    return Status;
}

/*
** GetChangeState() method of the CFilter class
**
**    Returns the current BDA change state.
**
*/
NTSTATUS
CFilter::GetChangeState(
                        IN PIRP         pIrp,
                        IN PKSMETHOD    pKSMethod,
                        OUT PULONG      pulChangeState
                        )
{
    NTSTATUS            Status = STATUS_SUCCESS;
    CFilter *           pFilter;
    BDA_CHANGE_STATE    topologyChangeState;

    UNREFERENCED_PARAMETER(pKSMethod);

    ASSERT( pIrp);
    ASSERT( pKSMethod);
    // pulChangeState needs to be verified because minData is zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pulChangeState)
    {
        pIrp->IoStatus.Information = sizeof(ULONG);
        return STATUS_MORE_ENTRIES;
    }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::GetChangeState() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);


    //  Call the BDA support library to 
    //  verify for any pending BDA topology changes.
    //
    Status = BdaGetChangeState( pIrp, &topologyChangeState);
    if (NT_SUCCESS( Status))
    {
        //  Figure out if there are changes pending.
        //
        if (   (topologyChangeState == BDA_CHANGES_PENDING)
               || (pFilter->m_BdaChangeState == BDA_CHANGES_PENDING)
               )
        {
            *pulChangeState = BDA_CHANGES_PENDING;
        }
        else
        {
            *pulChangeState = BDA_CHANGES_COMPLETE;
        }
    }


    return Status;
}


/*
** GetMedium() method of the CFilter class
**
** Identifies specific connection on a communication bus 
**
*/
NTSTATUS
CFilter::GetMedium(
                   IN PIRP             pIrp,
                   IN PKSMETHOD        pKSProperty,
                   OUT KSPIN_MEDIUM *  pKSMedium
                   )
{
    NTSTATUS            Status = STATUS_SUCCESS;
    CFilter *           pFilter;

    UNREFERENCED_PARAMETER(pKSProperty);
    ASSERT( pIrp);
    ASSERT( pKSProperty);
    // pulChangeState needs to be verified because minData is zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pKSMedium)
    {
        pIrp->IoStatus.Information = sizeof(KSPIN_MEDIUM);
        return STATUS_MORE_ENTRIES;
    }   


    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::CreateTopology() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    //  Because there is a max of one instance of each pin for a given filter
    //  instance, we can use the same GUID for the medium on each pin.
    //

    //  We use a GUID specific to this implementation of the
    //  device to differentiate from other implemenations of the same
    //  device.
    //
    Status = pFilter->m_pDevice->GetImplementationGUID( &pKSMedium->Set);
    if (!NT_SUCCESS( Status))
    {
        pKSMedium->Set = KSMEDIUMSETID_Standard;
        Status = STATUS_SUCCESS;
    }

    //  Further, we must differentiate this instance of this implementation
    //  from other intances of the same implementation.  We use a device
    //  instance number to do this.
    //
    Status = pFilter->m_pDevice->GetDeviceInstance( &pKSMedium->Id);
    if (!NT_SUCCESS( Status))
    {
        pKSMedium->Id = 0;
        Status = STATUS_SUCCESS;
    }

    //  Across all filters that represent this device there can only be one
    //  input pin instance and one output pin instance with the same
    //  media type so we don't have to distinguish between pin instances.
    //
    pKSMedium->Flags = 0;

    DbgPrint("%s Id %d\n", __FUNCTION__, pKSMedium->Id);

    return Status;
}


/*
** CreateTopology() method of the CFilter class
**
**    Keeps track of the topology association between input and output pins
**
*/
NTSTATUS
CFilter::CreateTopology(
                        IN PIRP         pIrp,
                        IN PKSMETHOD    pKSMethod,
                        PVOID           pvIgnored
                        )
{
    NTSTATUS            Status = STATUS_SUCCESS;
    CFilter *           pFilter;

    ASSERT( pIrp);
    ASSERT( pKSMethod);

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::CreateTopology() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    //
    //  Configure the hardware to complete its internal connection between
    //  the input pin and output pin here.
    //

    //  Call the BDA support library to create the standard topology and 
    //  validate the method, instance count, etc.
    //
    Status = BdaMethodCreateTopology( pIrp, pKSMethod, pvIgnored);


    return Status;
}

/*
** SetDemodulator ()
**
**    Sets the type of the demodulator.
**
** Arguments:
**
**
** Returns:
**
** Side Effects:  none
*/
STDMETHODIMP_(NTSTATUS)
    CFilter::SetDemodulator(
                            IN const GUID *       pguidDemodulator
                            )
{
    NTSTATUS        Status = STATUS_SUCCESS;
    ASSERT (pguidDemodulator);
    if (!pguidDemodulator)
    {
        return STATUS_INVALID_PARAMETER;
    }   

    //  Make sure the demodulator is supported.
    //
    //#if ATSC_RECEIVER
    if (IsEqualGUID( pguidDemodulator, &KSNODE_BDA_8VSB_DEMODULATOR) ||
        //#elif DVBT_RECEIVER
        IsEqualGUID( pguidDemodulator, &KSNODE_BDA_COFDM_DEMODULATOR) ||
        //#elif DVBS_RECEIVER
        IsEqualGUID( pguidDemodulator, &KSNODE_BDA_QPSK_DEMODULATOR) ||
        //#elif DVBC_RECEIVER || QAM_RECEIVER
        IsEqualGUID( pguidDemodulator, &KSNODE_BDA_QAM_DEMODULATOR) ||
        //#elif ISDBT_RECEIVER
        IsEqualGUID( pguidDemodulator, &KSNODE_BDA_ISDB_T_DEMODULATOR) ||
        //#elif ISDBS_RECEIVER
        IsEqualGUID( pguidDemodulator, &KSNODE_BDA_ISDB_S_DEMODULATOR) ||
        //#elif NETOP_RECEIVER
        IsEqualGUID( pguidDemodulator, &KSNODE_BDA_PBDA_TUNER)
        )
        //#endif
    {
        m_NewResource.guidDemodulator = *pguidDemodulator;
        m_BdaChangeState = BDA_CHANGES_PENDING;
    }
    else
    {
        Status = STATUS_NOT_SUPPORTED;
    }

    return Status;
}


/*
** GetStatus() method of the CFilter class
**
**    Gets the current device status for this filter instance.
**
*/
NTSTATUS
CFilter::GetStatus(
    PBDATUNER_DEVICE_STATUS     pDeviceStatus
    )
{
    // return properties about the current "tuned frequency", regardless of device state.
    ASSERT( m_pDevice);

    NTSTATUS Status = m_pDevice->GetStatus( pDeviceStatus);

    if (pDeviceStatus->fStatusNeedSignaling)
    {
        KIRQL               Irql;
        KeAcquireSpinLock( &spinLock, &Irql);
        
        if ((m_PendingEventEntry == NULL)||(m_FilterEventsManager.IsEnabled() == FALSE))
        {
            //DbgPrint("CFilter:: Eventing not enabled");
        }
        else
        {
            PBDATUNER_EVENT pTunerEvent;
            if (pDeviceStatus->fSignalLocked || pDeviceStatus->fCarrierPresent)
            {
                pTunerEvent = AllocateTunerEvent(&EVENTID_BDA_TunerSignalLock, 0,NULL);
            }
            else
            {
                pTunerEvent = AllocateTunerEvent(&EVENTID_BDA_TunerNoSignal, 0,NULL);
                //a real device have to consider the internal timeout time to enter a lock state
            }

            if (pTunerEvent == NULL)
            {
                DbgPrint("CFilter::Signal Status change could not allocate an event");
            }
            else
            {
                m_FilterEventsManager.QueueEvent(pTunerEvent,TRUE);
            }
        }
        KeReleaseSpinLock( &spinLock, Irql);
        //for test purpose only send a MMI on a specific satellite frequency 
        if (m_CurResource.ulCarrierFrequency == 12610000) 
        {
            SendBroadcastMMI(0);
        }
    }

    return Status;

};


/*
** AcquireResources() method of the CFilter class
**
**    Acquires resources for the underlying device.
**
*/
NTSTATUS
CFilter::AcquireResources(
    )
{
    NTSTATUS        Status = STATUS_SUCCESS;

    if (m_fResourceAcquired)
    {
        Status = m_pDevice->UpdateResources(
                                            &m_CurResource,
                                            m_ulResourceID
                                            );
    }
    else
    {
        //  Commit the resources on the underlying device
        //
        Status = m_pDevice->AcquireResources(
                                             &m_CurResource,
                                             &m_ulResourceID
                                             );
        m_fResourceAcquired = NT_SUCCESS( Status);
    }
    
    return Status;
}


/*
** ReleaseResources() method of the CFilter class
**
**    Releases resources from the underlying device.
**
*/
NTSTATUS
CFilter::ReleaseResources(
    )
{
    NTSTATUS        Status = STATUS_SUCCESS;

    //  Release the resources on the underlying device
    //
    if (m_fResourceAcquired)
    {
        Status = m_pDevice->ReleaseResources(
                                             m_ulResourceID
                                             );
        m_ulResourceID = 0;
        m_fResourceAcquired = FALSE;
    }

    return Status;
}

NTSTATUS
CFilter::PutFrequency(
                      IN ULONG        ulBdaParameter
                      )
{
    DbgPrint( "CFilter::PutFrequency %d\n", ulBdaParameter);
    m_NewResource.ulCarrierFrequency = ulBdaParameter;
    m_BdaChangeState = BDA_CHANGES_PENDING;

    return STATUS_SUCCESS;
}

NTSTATUS
CFilter::GetFrequency(
                      IN PULONG        pulBdaParameter
                      )
{
    *pulBdaParameter = m_CurResource.ulCarrierFrequency;
    
    return STATUS_SUCCESS;
}

NTSTATUS
CFilter::PutPolarity (
                      IN   ULONG ulBdaParameter
                      )
{
    DbgPrint( "CFilter::SetPolarity %d\n", ulBdaParameter);

    m_NewResource.ulPolarity = ulBdaParameter;
    m_BdaChangeState = BDA_CHANGES_PENDING;

    return STATUS_SUCCESS;
}


NTSTATUS
CFilter::GetPolarity(
                      IN PULONG        pulBdaParameter
                      )
{
    *pulBdaParameter = m_CurResource.ulPolarity;

    return STATUS_SUCCESS;
}

NTSTATUS
CFilter::PutPLPNumber (
                      IN   ULONG ulBdaParameter
                      )
{
    DbgPrint( "CFilter::SetPLPNumber %d\n", ulBdaParameter);

    m_NewResource.ulPLPNumber = ulBdaParameter;
    m_BdaChangeState = BDA_CHANGES_PENDING;

    return STATUS_SUCCESS;
}


NTSTATUS
CFilter::GetPLPNumber(
                      IN PULONG        pulBdaParameter
                      )
{
    *pulBdaParameter = m_CurResource.ulPLPNumber;

    return STATUS_SUCCESS;
}

NTSTATUS
CFilter::PutLNBSource (
                      IN   ULONG ulBdaParameter
                      )
{
    DbgPrint( "CFilter::PutLNBSource %d\n", ulBdaParameter);

    m_NewResource.ulDiseqC = ulBdaParameter;
    m_BdaChangeState = BDA_CHANGES_PENDING;

    return STATUS_SUCCESS;
}


NTSTATUS
CFilter::GetLNBSource(
                     IN PULONG        pulBdaParameter
                     )
{
    *pulBdaParameter = m_CurResource.ulDiseqC;

    return STATUS_SUCCESS;
}

NTSTATUS
CFilter::PutDiseqCEnable (
                       IN   BOOL fBdaParameter
                       )
{
    DbgPrint( "CFilter::PutDiseqCEnable %d\n", fBdaParameter);

    m_NewResource.fDiseqCEnable = fBdaParameter;
    m_BdaChangeState = BDA_CHANGES_PENDING;

    return STATUS_SUCCESS;
}


NTSTATUS
CFilter::GetDiseqCEnable(
                      IN PBOOL pfBdaParameter
                      )
{
    *pfBdaParameter = m_CurResource.fDiseqCEnable;

    return STATUS_SUCCESS;
}


NTSTATUS
CFilter::PutMultiplier(
                       IN ULONG ulBdaParameter
                       )
{
    DbgPrint( "CFilter::PutMultiplier %d\n", ulBdaParameter);
    m_NewResource.ulFrequencyMultiplier = ulBdaParameter;
    m_BdaChangeState = BDA_CHANGES_PENDING;

    return STATUS_SUCCESS;
}

NTSTATUS
CFilter::GetMultiplier(
                       IN PULONG        pulBdaParameter
                       )
{
    *pulBdaParameter = m_CurResource.ulFrequencyMultiplier;
    return STATUS_SUCCESS;
}

NTSTATUS 
CFilter::ProcessFrequencyFromTuner(
                                   IN PIRP         pIrp,
                                   IN PKSPROPERTY  pKSProperty,
                                   IN ULONG       *pulFreg
                                   )
{
    PAGED_CODE();

    DbgPrint( "CFilter::ProcessFrequencyFromTuner\n");

    // Make sure that we received correct property request
    if( !InlineIsEqualGUID( KSNAME_CaptureNodePropertySet, pKSProperty->Set ) )
        return STATUS_NOT_IMPLEMENTED;

    if( pKSProperty->Id != KSPROPERTY_CAPTURENODE_SETFREQ )
        return STATUS_INVALID_PARAMETER;

    // Get the filter this property corresponds to
    ASSERT(pIrp);
    PKSFILTER pKsFilter = KsGetFilterFromIrp( pIrp );
    if( pKsFilter == NULL )
        return STATUS_INVALID_PARAMETER;

    PKSDEVICE Device = KsFilterGetDevice(pKsFilter);
    CDevice* pDevice = reinterpret_cast<CDevice *>(Device->Context);

    NTSTATUS ntStatus = STATUS_SUCCESS;

    if( pKSProperty->Flags & KSPROPERTY_TYPE_SET )
    {
        DbgPrint( "CFilter::ProcessFrequencyFromTuner %d\n", *pulFreg);
        ntStatus = pDevice->SetFrequency( pulFreg );
    }
    else
    {
        ULONG ulSize = IoGetCurrentIrpStackLocation( pIrp )->Parameters.DeviceIoControl.OutputBufferLength;
        if( ulSize < sizeof( ULONG ) )
            ntStatus = STATUS_BUFFER_TOO_SMALL;
        else
        {
            *pulFreg = pDevice->Get_theFrequency();
            pIrp->IoStatus.Information = sizeof( ULONG );
        }
    }
    return ntStatus;
}


NTSTATUS
CFilter::EnablePendingEvent(
                            IN PIRP pIrp,
                            IN PKSEVENTDATA EventData,
                            IN struct _KSEVENT_ENTRY* pEventEntry
)
{
    NTSTATUS ntStatus = STATUS_NOT_SUPPORTED;
    CFilter *pFilter;
    KIRQL Irql;

    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    
    DbgPrint("EnablePendingEvent\n");

    KeAcquireSpinLock( &pFilter->spinLock, &Irql);
    if (pFilter->m_PendingEventEntry != NULL)
    {
        KeReleaseSpinLock( &pFilter->spinLock, Irql);
        ntStatus = STATUS_UNSUCCESSFUL;
        goto error;
    }

    KeReleaseSpinLock( &pFilter->spinLock, Irql);

    pFilter->m_PendingEventEntry = pEventEntry;
    KeResetEvent(&pFilter->m_evPendingEventsComplete);
    ntStatus = KsDefaultAddEventHandler(pIrp, EventData, pFilter->m_PendingEventEntry);

    KeAcquireSpinLock( &pFilter->spinLock, &Irql);
    
    if (NT_SUCCESS(ntStatus))
    {
        pFilter->m_FilterEventsManager.Enable();
        pFilter->m_FilterDebugEventsManager.Enable();
        pFilter->m_FilterGPNVEventsManager.Enable();
    }
    else
    {
        ASSERT(pFilter->m_FilterEventsManager.IsEnabled() == FALSE);
        ASSERT(pFilter->m_FilterDebugEventsManager.IsEnabled() == FALSE);
        ASSERT(pFilter->m_FilterGPNVEventsManager.IsEnabled() == FALSE);
        pFilter->m_FilterEventsManager.Disable();
        pFilter->m_FilterEventsManager.ReleaseAllEvents();
        pFilter->m_FilterDebugEventsManager.Disable();
        pFilter->m_FilterDebugEventsManager.ReleaseAllEvents();
        pFilter->m_FilterGPNVEventsManager.Disable();
        pFilter->m_FilterGPNVEventsManager.ReleaseAllEvents();

    }
    KeReleaseSpinLock( &pFilter->spinLock, Irql);

error:
    pIrp->IoStatus.Status = ntStatus;
    return ntStatus;
}

VOID
CFilter::DisablePendingEvent(
                            IN PFILE_OBJECT FileObject,
                            IN struct _KSEVENT_ENTRY* EventEntry
    )
{
    CFilter *pFilter;
    KIRQL Irql;
    BOOL bWaitForCompletes = FALSE;

    DbgPrint("Disable Pending Event\n");
    PKSFILTER pKsFilter = KsGetFilterFromFileObject(FileObject);
    pFilter = reinterpret_cast<CFilter *>(pKsFilter->Context);
    
    KeAcquireSpinLock( &pFilter->spinLock, &Irql);
    pFilter->m_FilterEventsManager.Disable();
    pFilter->m_FilterEventsManager.ReleaseAllUnsignalledEvents();

    pFilter->m_FilterDebugEventsManager.Disable();
    pFilter->m_FilterDebugEventsManager.ReleaseAllUnsignalledEvents();

    pFilter->m_FilterGPNVEventsManager.Disable();
    pFilter->m_FilterGPNVEventsManager.ReleaseAllUnsignalledEvents();

    //
    // Are we in the middle of signalling an event. If so then we must wait
    // for it to finish
    // 
    bWaitForCompletes = (pFilter->m_bSignallingPendingEvent == TRUE);
 
    KeReleaseSpinLock( &pFilter->spinLock, Irql);
    if (bWaitForCompletes)
    {
        KeWaitForSingleObject(
            &pFilter->m_evPendingEventsComplete,
            Executive,
            KernelMode,
            FALSE,
            NULL);     // Wait for ever
    }

    KeAcquireSpinLock( &pFilter->spinLock, &Irql);
    pFilter->m_PendingEventEntry = NULL;
    KeReleaseSpinLock( &pFilter->spinLock, Irql);
    
    RemoveEntryList(&EventEntry->ListEntry);    
    
}



NTSTATUS
CFilter::GetEvent(
                  IN PIRP         pIrp,
                  IN PKSMETHOD  pKSMethod,
                  IN PBDA_EVENT_DATA   pData
                  )
{
    DbgPrint("[FILTER] GetEvent called");
    CFilter *pFilter;
    NTSTATUS ntStatus = STATUS_NOT_SUPPORTED;
    PBDATUNER_EVENT pTunerEvent = NULL;
    PIO_STACK_LOCATION pirpSp = IoGetCurrentIrpStackLocation(pIrp);  
    ULONG ulDataSize = pirpSp->Parameters.DeviceIoControl.OutputBufferLength;
    KIRQL Irql;

    UNREFERENCED_PARAMETER(pKSMethod);

    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    KeAcquireSpinLock( &pFilter->spinLock, &Irql);

    pIrp->IoStatus.Information = sizeof(BDA_EVENT_DATA);
    if (ulDataSize < sizeof(BDA_EVENT_DATA))
    {
        ntStatus  = STATUS_INVALID_BUFFER_SIZE;
        pIrp->IoStatus.Status = ntStatus;
        goto exit;
    }
    
    ULONG ulEventDataSize = ulDataSize - sizeof(BDA_EVENT_DATA) + 1;
    ULONG ulSizeNeeded = 0;
    
    ntStatus = pFilter->m_FilterEventsManager.GetCurrentEvent(&pTunerEvent);
    if ((ntStatus == STATUS_SUCCESS))
    {
        DbgPrint("[FILTER] Got Event");
        if (ulEventDataSize < pTunerEvent->ulDataLength)
        {
            pIrp->IoStatus.Information = sizeof(BDA_EVENT_DATA) - 1 + pTunerEvent->ulDataLength;
            ntStatus = STATUS_BUFFER_OVERFLOW;
            goto exit;

        }

        if (pTunerEvent->ulDataLength > 0)
        {
            ulSizeNeeded = pTunerEvent->ulDataLength + sizeof(BDA_EVENT_DATA) -1;
            pIrp->IoStatus.Information = ulSizeNeeded;
            memcpy(pData->argbEventData, pTunerEvent->argbData, pTunerEvent->ulDataLength);
            pData->ulEventDataLength = pTunerEvent->ulDataLength;
        } 

        pData->uuidEventType = pTunerEvent->uuidEvent;
        if (pTunerEvent->ulEventId == INVALID_TUNER_EVENT_ID)
        {
            pData->ulEventID= pTunerEvent->ulEventId = InterlockedIncrement(&pFilter->m_lEventId);
            if (pTunerEvent->ulEventId == INVALID_TUNER_EVENT_ID) 
            {   //wrap around, EventIds should be never 0
                pData->ulEventID= pTunerEvent->ulEventId = InterlockedIncrement(&pFilter->m_lEventId);
            }
        }
        DbgPrint("[FILTER] GetEvent succeeded %d", pTunerEvent->ulEventId);
        pData->lResult = STATUS_SUCCESS;
        goto exit;        
    }
    
    ntStatus = STATUS_SUCCESS;
    pData->lResult = BDA_E_NO_MORE_EVENTS;

  exit:
    KeReleaseSpinLock( &pFilter->spinLock, Irql);
    pIrp->IoStatus.Status = ntStatus;
    return ntStatus;    
}

NTSTATUS
CFilter::GetDebugData(
                      IN PIRP         pIrp,
                      IN PKSMETHOD pKSMethod,
                      IN BDA_DEBUG_DATA   *pData
    )
{
    CFilter *pFilter;
    NTSTATUS ntStatus = STATUS_NOT_SUPPORTED;
    PBDATUNER_EVENT pTunerEvent = NULL;
    PIO_STACK_LOCATION pirpSp = IoGetCurrentIrpStackLocation(pIrp);  
    ULONG ulDataSize = pirpSp->Parameters.DeviceIoControl.OutputBufferLength;
    ULONG ulDebugDataSizeNeeded = 0;
    ULONG ulDebugDataSizeAvailable = 0;
    ULONG ulSizeNeeded = sizeof(BDA_DEBUG_DATA);
    KIRQL Irql;

    UNREFERENCED_PARAMETER(pKSMethod);
    
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);

    KeAcquireSpinLock( &pFilter->spinLock, &Irql);
    
    ntStatus = pFilter->m_FilterDebugEventsManager.ReadCurrentEvent(&pTunerEvent);
    if (ntStatus == STATUS_SUCCESS)
    {
        ulDebugDataSizeNeeded = pTunerEvent->ulDataLength;

        if (ulDataSize < sizeof(BDA_DEBUG_DATA))
        {
            ntStatus = STATUS_BUFFER_OVERFLOW;
            ulSizeNeeded  = ulDebugDataSizeNeeded + sizeof(BDA_DEBUG_DATA);
            goto exit;
            
        }
        
        ulDebugDataSizeAvailable = ulDataSize - sizeof(BDA_DEBUG_DATA);

        if (ulDebugDataSizeNeeded > ulDebugDataSizeAvailable)
        {
            ntStatus = STATUS_BUFFER_OVERFLOW;
            ulSizeNeeded  = ulDebugDataSizeNeeded + sizeof(BDA_DEBUG_DATA);
            goto exit;
            
        }

        //Complete the event in the DebugEventsManager list
        pData->uuidDebugDataType = pTunerEvent->uuidEvent;
        pData->ulDataSize = pTunerEvent->ulDataLength;
        memcpy(pData->argbDebugData, pTunerEvent->argbData, pTunerEvent->ulDataLength);
        ulSizeNeeded  = ulDebugDataSizeNeeded + sizeof(BDA_DEBUG_DATA);
        //
        // The Debug Events can get completed here as the plugin is 
        // not going to complete them.
        //
        pFilter->m_FilterDebugEventsManager.CompleteEvent(pTunerEvent, INVALID_TUNER_EVENT_ID, S_OK);
        goto exit;        
    }

    ntStatus = STATUS_NO_MORE_ENTRIES;

  exit:
    KeReleaseSpinLock( &pFilter->spinLock, Irql);
    pIrp->IoStatus.Information = ulSizeNeeded; 
    DbgPrint("%s: GetDebugData %x %x %x\n",__FUNCTION__, ntStatus, pIrp->IoStatus.Status, INT(pIrp->IoStatus.Information) );
    return ntStatus;    
}


NTSTATUS
CFilter::SetDebugLevel(
                       IN PIRP pIrp,
                       IN PKSMETHOD  pKSMethod,
                       IN PLONG plPbdaResult
                      )
{
    NTSTATUS Status = STATUS_SUCCESS;
    CFilter * pFilter;
    PKSM_BDA_DEBUG_LEVEL pDebugLevel = NULL;
    PIO_STACK_LOCATION pirpSp = IoGetCurrentIrpStackLocation(pIrp);  
    ULONG ulOutputSize = pirpSp->Parameters.DeviceIoControl.OutputBufferLength;
    ULONG ulInputSize = pirpSp->Parameters.DeviceIoControl.InputBufferLength;

    UNREFERENCED_PARAMETER(plPbdaResult);

    ASSERT( pIrp);

    //  Obtain a "this" pointer to the filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    if (ulInputSize < sizeof(KSM_BDA_DEBUG_LEVEL))
    {
        DbgPrint("%s: Not enough input buffer \n",__FUNCTION__);
        Status = STATUS_UNSUCCESSFUL;
        goto exit;
    }

    if (ulOutputSize < sizeof(LONG))
    {
        DbgPrint("%s: Not enough memory\n",__FUNCTION__);
        pIrp->IoStatus.Information = sizeof(LONG);
        Status = STATUS_BUFFER_TOO_SMALL;
        goto exit;
    }
    pIrp->IoStatus.Information = sizeof(LONG);
    Status = STATUS_SUCCESS;
    pDebugLevel = (PKSM_BDA_DEBUG_LEVEL)pKSMethod;
    pFilter->m_FilterDebugEventsManager.SetDebugLevel(pDebugLevel->ucDebugLevel);
        
  exit:
    pIrp->IoStatus.Status = Status;
    return Status;
}


NTSTATUS
CFilter::CompleteEvent(
                      IN PIRP  pIrp,
                      IN PKSMETHOD  pKSMethod,
                      OUT PLONG   plResult
                      )
{
    DbgPrint("[FILTER] Complete event called");
    PIO_STACK_LOCATION pirpSp = IoGetCurrentIrpStackLocation(pIrp);  
    ULONG ulOutputSize = pirpSp->Parameters.DeviceIoControl.OutputBufferLength;
    ULONG ulInputSize = pirpSp->Parameters.DeviceIoControl.InputBufferLength;
    NTSTATUS Status = STATUS_SUCCESS;
    PKSM_BDA_EVENT_COMPLETE pEventComplete = NULL;
    CFilter *pFilter;
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    KIRQL Irql;

    if (ulInputSize < sizeof(KSM_BDA_EVENT_COMPLETE))
    {
        Status = STATUS_UNSUCCESSFUL;
        DbgPrint("%s: Not enough input buffer \n",__FUNCTION__);
        goto exit;
    }

    if ((ulOutputSize < sizeof(LONG)))
    {
        DbgPrint("%s: Not enough memory\n",__FUNCTION__);
        pIrp->IoStatus.Information = sizeof(LONG);
        Status = STATUS_BUFFER_TOO_SMALL;
        goto exit;
    }
    pEventComplete = (PKSM_BDA_EVENT_COMPLETE)pKSMethod;
    KeAcquireSpinLock( &pFilter->spinLock, &Irql);
    DbgPrint("[FILTER] Completing event %d", pEventComplete->ulEventID);
    pFilter->m_FilterEventsManager.CompleteEvent(NULL, pEventComplete->ulEventID, pEventComplete->ulEventResult);
    KeReleaseSpinLock( &pFilter->spinLock, Irql);
    *plResult = S_OK;
    Status = STATUS_SUCCESS;
    pIrp->IoStatus.Information = sizeof(LONG);
exit:
    pIrp->IoStatus.Status = Status;
    return STATUS_SUCCESS;
}

VOID
CFilter::SignalPendingEvent()
{
    BOOL fEventEnabled = m_FilterEventsManager.IsEnabled();
    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);
    
    UNREFERENCED_PARAMETER(fEventEnabled);

    m_bSignallingPendingEvent = TRUE;
    KeReleaseSpinLockFromDpcLevel( &spinLock);

    DbgPrint("Generating Event command\n");
    KsGenerateEvent(m_PendingEventEntry);

    KeAcquireSpinLockAtDpcLevel( &spinLock);
    m_bSignallingPendingEvent = FALSE;
    //
    // Check to see if we have transitioned to a disabled state
    // and if so signal the event that DisableEvent is waiting for
    //
    if(m_FilterEventsManager.IsEnabled() == FALSE)
    {
        ASSERT (fEventEnabled == TRUE);
        DbgPrint("Setting Pending events complete event\n");
        KeSetEvent(&m_evPendingEventsComplete, 0 , FALSE);
    }


}
//=====================================================
// DRM methods
//=====================================================
/*
** PBDA_DRM_SetDRM() method of the CFilter class
**
**    Set the new DRM UUID and return a PBDAResult.
**
*/
NTSTATUS
CFilter::PBDA_DRM_SetDRM(
                         IN PIRP         pIrp,
                         IN PKSMETHOD    pKSMethod,
                         OUT PULONG      pulReturn
                         )
{
    NTSTATUS            Status = STATUS_SUCCESS;
    ULONG               ulInputBufferSize = 0;
    ULONG               ulOutputBufferSize = 0;
    CFilter *           pFilter = NULL;
    PIO_STACK_LOCATION  pIrpStack  = NULL;

    ASSERT( pIrp);
    ASSERT( pKSMethod);
    
    PKSM_BDA_DRM_SETDRM pSetDRMParam = (PKSM_BDA_DRM_SETDRM) pKSMethod;
    // pSetDRMParam needs to be verified because minData is zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pSetDRMParam)
    {
        pIrp->IoStatus.Information = sizeof(KSM_BDA_DRM_SETDRM);
        return STATUS_MORE_ENTRIES;
    }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::GetChangeState() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
        ulInputBufferSize = pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
    }

    if (ulInputBufferSize < sizeof(KSM_BDA_DRM_SETDRM))
    {
        DbgPrint("PBDA_DRM_SetDRM: Input size %d too small",ulInputBufferSize);
        return STATUS_UNSUCCESSFUL;
    }

    if (ulOutputBufferSize < sizeof(ULONG))
    {
        DbgPrint("PBDA_DRM_SetDRM: Output size %d too small",ulOutputBufferSize);
        pIrp->IoStatus.Information = sizeof(ULONG);
        return STATUS_BUFFER_OVERFLOW;
    }


    DbgPrint("PBDA_DRM_SetDRM: NewUUID = %X-%X-%X-%X%X-%X%X%X%X%X%X\n", pSetDRMParam->NewDRMuuid.Data1,
        pSetDRMParam->NewDRMuuid.Data2,
        pSetDRMParam->NewDRMuuid.Data3,
        pSetDRMParam->NewDRMuuid.Data4[0],
        pSetDRMParam->NewDRMuuid.Data4[1],
        pSetDRMParam->NewDRMuuid.Data4[2],pSetDRMParam->NewDRMuuid.Data4[3],pSetDRMParam->NewDRMuuid.Data4[4],pSetDRMParam->NewDRMuuid.Data4[5],pSetDRMParam->NewDRMuuid.Data4[6],pSetDRMParam->NewDRMuuid.Data4[7]);

    *pulReturn = 0;  //PBDAReturn value
    pIrp->IoStatus.Information = sizeof(ULONG);

    return Status;
}

NTSTATUS
CFilter::PBDA_GetDRMStatus(
                           IN  PIRP         pIrp,
                           IN  PKSMETHOD    pKSMethod,
                           OUT PBDA_DRM_DRMSTATUS pDRMStatus
                           )
{
    NTSTATUS        Status = STATUS_SUCCESS;
    CFilter *       pFilter;
    ULONG           ulOutputBufferSize = 0;
    PIO_STACK_LOCATION pIrpStack  = NULL;

    UNREFERENCED_PARAMETER(pKSMethod);

    _DbgPrintF(DEBUGLVL_VERBOSE,("CFilter::PBDA_GetDRMStatus"));

    ASSERT(pIrp);
    ASSERT(pKSMethod);

    // pDRMStatus needs to be verified because minData might be zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pDRMStatus)
    {
        pIrp->IoStatus.Information = sizeof(BDA_DRM_DRMSTATUS);
        return STATUS_MORE_ENTRIES;
    }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::GetChangeState() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
    }
    else
    {
        return STATUS_UNSUCCESSFUL;
    }

    if (ulOutputBufferSize < sizeof(BDA_DRM_DRMSTATUS))
    {
        DbgPrint("CFilter::GetDRMServiceProperty DRM_AVAILABLE Buffer too small");
        pIrp->IoStatus.Information = sizeof(BDA_DRM_DRMSTATUS); 
        Status = STATUS_BUFFER_OVERFLOW;
    }
    else
    {
        pDRMStatus->lResult = BDA_E_NOT_IMPLEMENTED;
        pDRMStatus->DRMuuid = TEST_GUID;

        pDRMStatus->ulDrmUuidListStringSize = sizeof(GUID) * 2; //example size 2 GUIDs
        pDRMStatus->argbDrmUuidListString[0] = TEST_GUID2;
        if ( ulOutputBufferSize >= (sizeof(BDA_DRM_DRMSTATUS) - sizeof(GUID) + pDRMStatus->ulDrmUuidListStringSize) )
        {
            //only if the buffer is big enough we copy further list elements
            pDRMStatus->argbDrmUuidListString[1] = TEST_GUID3;
            //...copy other list UUIDs
            pIrp->IoStatus.Information = sizeof(BDA_DRM_DRMSTATUS) - sizeof(GUID) + pDRMStatus->ulDrmUuidListStringSize;
            //return the correct length
        }
        else
        {
            pIrp->IoStatus.Information = sizeof(BDA_DRM_DRMSTATUS) - sizeof(GUID) + pDRMStatus->ulDrmUuidListStringSize;
            //return the target length (example here 2 GUIDs)
            DbgPrint("CCapturePin::GetDRMServiceProperty DRM_AVAILABLE Buffer Size=%d too small", ulOutputBufferSize);
            Status = STATUS_BUFFER_OVERFLOW;
        }                     
    }

    return Status;
}

//=====================================================
// WMDRM methods
//=====================================================

NTSTATUS
CFilter::PBDA_GetWMDRMStatus(
                             IN  PIRP         pIrp,
                             IN  PKSMETHOD    pKSMethod,
                             OUT PBDA_WMDRM_STATUS pWMDRMStatus
                             )
{
    NTSTATUS        Status = STATUS_SUCCESS;
    CFilter *       pFilter;
    ULONG           ulOutputBufferSize = 0;
    PIO_STACK_LOCATION pIrpStack  = NULL;

    UNREFERENCED_PARAMETER(pKSMethod);

    _DbgPrintF(DEBUGLVL_VERBOSE,("CFilter::GetWMDRMStatus"));

    ASSERT(pIrp);
    ASSERT(pKSMethod);


    // pDRMStatus needs to be verified because minData might be zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pWMDRMStatus)
    {
        pIrp->IoStatus.Information = sizeof(BDA_DRM_DRMSTATUS);
        return STATUS_MORE_ENTRIES;
    }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::GetChangeState() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
    }
    else
    {
        return STATUS_UNSUCCESSFUL;
    }

    if (ulOutputBufferSize < sizeof(BDA_WMDRM_STATUS))
    {
        DbgPrint("CFilter::GetDRMStatus Buffer too small");
        pIrp->IoStatus.Information = sizeof(BDA_WMDRM_STATUS); 
        Status = STATUS_BUFFER_OVERFLOW;
    }
    else
    {
        pWMDRMStatus->lResult = BDA_E_NOT_IMPLEMENTED;//return the correct result
        pWMDRMStatus->ulMaxCaptureTokenSize = 0x11; //some example values
        pWMDRMStatus->ulMaxLicense = 0x12;
        pWMDRMStatus->ulMinSecurityLevel = 0x13;
        pWMDRMStatus->ulRevInfoIssuedTime = 0x14;
        pWMDRMStatus->ulRevInfoSequenceNumber = 0x15;
        pWMDRMStatus->ulRevInfoTTL = 0x16;
        pWMDRMStatus->ulRevListVersion = 0x17;
        pWMDRMStatus->ulState = 0x18;
        pWMDRMStatus->uMaxStreamingPid = 0x19;
        pIrp->IoStatus.Information = sizeof(BDA_WMDRM_STATUS);                                         
        DbgPrint("CCapturePin::Get WMDRMStatus succeded");
    }

    return Status;
}

NTSTATUS
CFilter::PBDA_GetWMDRMKeyInfo(
                              IN  PIRP         pIrp,
                              IN  PKSMETHOD    pKSMethod,
                              OUT PBDA_WMDRM_KEYINFOLIST pWMDRMKeyInfo
                              )
{
    NTSTATUS        Status = STATUS_SUCCESS;
    CFilter *       pFilter;
    ULONG           ulOutputBufferSize = 0;
    ULONG           ulKeyBufferSize = sizeof(GUID) * 2;
    PIO_STACK_LOCATION pIrpStack  = NULL;

    UNREFERENCED_PARAMETER(pKSMethod);

    _DbgPrintF(DEBUGLVL_VERBOSE,("CFilter::PBDA_GetWMDRMKeyInfo"));

    ASSERT(pIrp);
    ASSERT(pKSMethod);


    // pDRMStatus needs to be verified because minData might be zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pWMDRMKeyInfo)
    {
        pIrp->IoStatus.Information = sizeof(BDA_WMDRM_KEYINFOLIST);
        return STATUS_MORE_ENTRIES;
    }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::GetChangeState() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
    }
    else
    {
        return STATUS_UNSUCCESSFUL;
    }
    if (ulOutputBufferSize < (sizeof(BDA_WMDRM_KEYINFOLIST) - sizeof(GUID) + ulKeyBufferSize)) 
    {
        Status = STATUS_BUFFER_OVERFLOW;
        pIrp->IoStatus.Information = sizeof(BDA_WMDRM_KEYINFOLIST) - sizeof(GUID) + ulKeyBufferSize;//default 2 GUIDS example
        DbgPrint("CFilter::Get WMDRM KeyInfo Buffer too small");
    }
    else
    {
        pWMDRMKeyInfo->lResult = 0;
        pWMDRMKeyInfo->argKeyuuidBuffer[0]=TEST_GUID;
        pWMDRMKeyInfo->argKeyuuidBuffer[1]=TEST_GUID3;
        pWMDRMKeyInfo->ulKeyuuidBufferLen = ulKeyBufferSize;//test size
        pIrp->IoStatus.Information = sizeof(BDA_WMDRM_KEYINFOLIST) - sizeof(GUID) + pWMDRMKeyInfo->ulKeyuuidBufferLen; //return the buffer size
        DbgPrint("CFilter::Get WMDRM KeyInfo succeded");
    }

    return Status;
}

/*
** PBDA_WMDRMRevInfo() method of the CFilter class
**
**    Set the Revocation Information and return a PBDAResult.
**
*/
NTSTATUS
CFilter::PBDA_WMDRMRevInfo(
                         IN PIRP         pIrp,
                         IN PKSMETHOD    pKSMethod,
                         OUT PULONG      pulReturn
                         )
{
    NTSTATUS            Status = STATUS_SUCCESS;
    CFilter *           pFilter;
    ULONG               ulInputBufferSize = 0;
    ULONG               ulOutputBufferSize = 0;
    PIO_STACK_LOCATION  pIrpStack  = NULL;

    ASSERT( pIrp);
    ASSERT( pKSMethod);
    PKSM_BDA_BUFFER pRevInfoBuffer = (PKSM_BDA_BUFFER) pKSMethod;
    // pRevInfoBuffer needs to be verified because minData is zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pRevInfoBuffer)
    {
        pIrp->IoStatus.Information = sizeof(KSM_BDA_BUFFER);
        return STATUS_MORE_ENTRIES;
    }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::GetChangeState() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
        ulInputBufferSize = pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
    }

    if (ulInputBufferSize < sizeof(KSM_BDA_BUFFER))
    {
        DbgPrint("PBDA_WMDRMRevInfo: Input size %d too small",ulInputBufferSize);
        return STATUS_UNSUCCESSFUL;
    }

    if (ulOutputBufferSize < sizeof(ULONG))
    {
        DbgPrint("PBDA_WMDRMRevInfo: Output size %d too small",ulOutputBufferSize);
        pIrp->IoStatus.Information = sizeof(ULONG);
        return STATUS_BUFFER_OVERFLOW;
    }
    DbgPrint("PBDA_WMDRMRevInfo: BufferSize = %d\n", pRevInfoBuffer->ulBufferSize);

    //pRevInfoBuffer->argbBuffer contain the data, process it
    *pulReturn = 0;  //return the PBDARESULT
    pIrp->IoStatus.Information = sizeof(PBDARESULT);

    return Status;
}

/*
** PBDA_WMDRMCrl() method of the CFilter class
**
**    Set the Certificate Revocation List and return a PBDAResult.
**
*/
NTSTATUS
CFilter::PBDA_WMDRMCrl(
                           IN PIRP         pIrp,
                           IN PKSMETHOD    pKSMethod,
                           OUT PULONG      pulReturn
                           )
{
    NTSTATUS            Status = STATUS_SUCCESS;
    CFilter *           pFilter;
    ULONG               ulInputBufferSize = 0;
    ULONG               ulOutputBufferSize = 0;
    PIO_STACK_LOCATION  pIrpStack  = NULL;

    ASSERT( pIrp);
    ASSERT( pKSMethod);
    PKSM_BDA_BUFFER pCrlBuffer = (PKSM_BDA_BUFFER) pKSMethod;
    // pulReturn needs to be verified because minData is zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pulReturn)
    {
        pIrp->IoStatus.Information = sizeof(KSM_BDA_BUFFER);
        return STATUS_MORE_ENTRIES;
    }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::GetChangeState() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
        ulInputBufferSize = pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
    }

    if (ulInputBufferSize < sizeof(KSM_BDA_BUFFER))
    {
        DbgPrint("PBDA_WMDRMCrl: Input size %d too small",ulInputBufferSize);
        return STATUS_UNSUCCESSFUL;
    }

    if (ulOutputBufferSize < sizeof(ULONG))
    {
        DbgPrint("PBDA_WMDRMCrl: Output size %d too small",ulOutputBufferSize);
        pIrp->IoStatus.Information = sizeof(ULONG);
        return STATUS_BUFFER_OVERFLOW;
    }

    DbgPrint("PBDA_WMDRMCrl: BufferSize = %d\n", pCrlBuffer->ulBufferSize);

    //pCrlBuffer->argbBuffer contain the data, process it
    *pulReturn = 0;  //return the PBDARESULT
    pIrp->IoStatus.Information = sizeof(PBDARESULT);

    return Status;
}

/*
** PBDA_WMDRMTransactMessage() method of the CFilter class
**
**    Set a Message and return a PBDAResult.
**
*/
NTSTATUS
CFilter::PBDA_WMDRMTransactMessage(
                       IN PIRP         pIrp,
                       IN PKSMETHOD    pKSMethod,
                       OUT PBDA_BUFFER pMessageOutBuffer
                       )
{
    NTSTATUS            Status = STATUS_SUCCESS;
    CFilter *           pFilter;
    ULONG               ulInputBufferSize = 0;
    ULONG               ulOutputBufferSize = 0;
    ULONG               ulTestBufferSize = 10;
    PIO_STACK_LOCATION  pIrpStack  = NULL;
    PKSM_BDA_BUFFER     pMessageInBuffer = (PKSM_BDA_BUFFER)pKSMethod; 

    UNREFERENCED_PARAMETER(pKSMethod);

    ASSERT( pIrp);
    ASSERT( pKSMethod);
    // pMessageBuffer needs to be verified because minData is zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pMessageOutBuffer)
    {
        pIrp->IoStatus.Information = sizeof(BDA_BUFFER);
        return STATUS_BUFFER_OVERFLOW;
    }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::GetChangeState() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulInputBufferSize = pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
    }
    else
    {
        return STATUS_UNSUCCESSFUL;
    }

    DbgPrint("PBDA_WMDRMTransactMessage: BufferSize = %d\n", pMessageInBuffer->ulBufferSize);
    //pMessageInBuffer->argbBuffer contain the send message buffer
    if (ulOutputBufferSize < (sizeof(BDA_BUFFER) + ulTestBufferSize/*use real target length*/) )
    {
        Status = STATUS_BUFFER_OVERFLOW;
        pIrp->IoStatus.Information = sizeof(BDA_BUFFER) -1 + ulTestBufferSize; //means 10Byte Buffer 
        //return the correct size, which is >= sizeof(BDA_BUFFER)
        DbgPrint("CFilter::Get WMDRM Response Buffer %d too small", ulOutputBufferSize);
    }
    else
    {
        pMessageOutBuffer->lResult = 0;
        //copy into pMessageOutBuffer->argbBuffer the response data
        pMessageOutBuffer->argbBuffer[0] = 0x71;
        pMessageOutBuffer->argbBuffer[1] = 0x72;
        //...
        pMessageOutBuffer->argbBuffer[9] = 0x7A;
        pMessageOutBuffer->ulBufferSize = ulTestBufferSize;
        pIrp->IoStatus.Information = sizeof(BDA_BUFFER) -1 + ulTestBufferSize;
        DbgPrint("CFilter::PBDA_WMDRMTransactMessage succeded");
    }

    return Status;
}

/*
** PBDA_WMDRMReissueLicense() method of the CFilter class
**
**    Reissue a License and return a PBDAResult.
**
*/
NTSTATUS
CFilter::PBDA_WMDRMReissueLicense(
                              IN PIRP         pIrp,
                              IN PKSMETHOD    pKSMethod,
                              OUT PULONG      pulReturn 
                              )
{
    NTSTATUS            Status = STATUS_SUCCESS;
    CFilter *           pFilter;
    ULONG               ulBufferLength;
    PKSM_BDA_WMDRM_LICENSE  pReissueLicense = (PKSM_BDA_WMDRM_LICENSE)pKSMethod;

    ASSERT( pIrp);
    ASSERT( pKSMethod);
    // pReissueLicense needs to be verified because minData might be zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pReissueLicense)
        {
        pIrp->IoStatus.Information = sizeof(KSM_BDA_WMDRM_LICENSE);
        return STATUS_MORE_ENTRIES;
        }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::GetChangeState() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    DbgPrint("PBDA_WMDRMReissueLicense: KeyUUID = %X-%X-%X-%X%X-%X%X%X%X%X%X\n", pReissueLicense->uuidKeyID.Data1,
        pReissueLicense->uuidKeyID.Data2,
        pReissueLicense->uuidKeyID.Data3,
        pReissueLicense->uuidKeyID.Data4[0],
        pReissueLicense->uuidKeyID.Data4[1],
        pReissueLicense->uuidKeyID.Data4[2],pReissueLicense->uuidKeyID.Data4[3],pReissueLicense->uuidKeyID.Data4[4],pReissueLicense->uuidKeyID.Data4[5],pReissueLicense->uuidKeyID.Data4[6],pReissueLicense->uuidKeyID.Data4[7]);

    *pulReturn = 0;
    ulBufferLength = sizeof(PBDARESULT);

    pIrp->IoStatus.Information = ulBufferLength;

    return Status;
}

/*
** PBDA_WMDRMRenewLicense() method of the CFilter class
**
**    Renew a License and return a PBDAResult.
**
*/
NTSTATUS
CFilter::PBDA_WMDRMRenewLicense(
                                 IN PIRP         pIrp,
                                 IN PKSMETHOD    pKSMethod,
                                 OUT PBDA_WMDRM_RENEWLICENSE pXMROutBuffer
                                 )
{
    NTSTATUS            Status = STATUS_SUCCESS;
    CFilter *           pFilter;
    PIO_STACK_LOCATION  pIrpStack  = NULL;
    ULONG               ulInBufferLength = 0;
    ULONG               ulOutBufferLength = 0;
    ULONG               ulXmrOutBufferLength = 10; //example size
    BYTE *              pXMRBuffer = NULL;
    BYTE *              pTokenBuffer = NULL;
    PKSM_BDA_WMDRM_RENEWLICENSE pRenewLicenseBuffer = (PKSM_BDA_WMDRM_RENEWLICENSE)pKSMethod;

    ASSERT( pIrp);
    ASSERT( pKSMethod);
    // pXMROutBuffer needs to be verified because minData might be zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pXMROutBuffer)
        {
        pIrp->IoStatus.Information = sizeof(BDA_WMDRM_RENEWLICENSE);
        return STATUS_MORE_ENTRIES;
        }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::GetChangeState() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    DbgPrint("PBDA_WMDRMRenewLicense: XMRLicenseLen = %d ,TokenLen = %d\n", pRenewLicenseBuffer->ulXMRLicenseLength,
        pRenewLicenseBuffer->ulEntitlementTokenLength);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack) 
    {
        ulOutBufferLength = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
        ulInBufferLength = pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
    }
    else
    {
        return STATUS_UNSUCCESSFUL;
    }
    //check the length of the output buffer (pXMROutBuffer->ulXmrLicenseOutputLength)

    if (ulOutBufferLength < (sizeof(BDA_WMDRM_RENEWLICENSE) + ulXmrOutBufferLength -1))
    {
        DbgPrint("PBDA_WMDRMRenewLicense: Buffer Length Error, Output Length = %d\n", ulOutBufferLength);
        pIrp->IoStatus.Information = sizeof(BDA_WMDRM_RENEWLICENSE) + ulXmrOutBufferLength -1;
        return STATUS_BUFFER_OVERFLOW;
    }

    if (ulInBufferLength < sizeof(KSM_BDA_WMDRM_RENEWLICENSE))
    {
        DbgPrint("PBDA_WMDRMRenewLicense: Input size %d too small",ulInBufferLength);
        return STATUS_UNSUCCESSFUL;
    }

    pXMRBuffer = pRenewLicenseBuffer->argbDataBuffer;
    pTokenBuffer = pXMRBuffer + pRenewLicenseBuffer->ulXMRLicenseLength;
    //Process the XMRLicense and Token buffer with there length 
    //pRenewLicenseBuffer->ulXMRLicenseLength and pRenewLicenseBuffer->ulEntitlementTokenLength

    pXMROutBuffer->lResult = 0; //return PBDAResult
    pXMROutBuffer->ulDescrambleStatus = 0xA9A9C5C5;//return the correct descrambling status
    ULONG ulCurrentXMRBufLen = pXMROutBuffer->ulXmrLicenseOutputLength; //use the correct length of the current XMR buffer

    if (ulOutBufferLength < (sizeof(BDA_WMDRM_RENEWLICENSE) + ulCurrentXMRBufLen - 1))
    {
        //the return buffer is not big enough, return the unchanged buffer
        DbgPrint("PBDA_WMDRMRenewLicense: Buffer Length Error, Output XMRLicense Length = %d of Buffer Length =%d\n",ulCurrentXMRBufLen, ulOutBufferLength);       
        pXMROutBuffer->ulXmrLicenseOutputLength = ulXmrOutBufferLength; //return here the correct target length 
        pXMROutBuffer->lResult = BDA_E_BUFFER_TOO_SMALL;
    }
    else 
    {
        //copy the current XMRBuffer into pXMROutBuffer->argbXmrLicenceOutputBuffer
        pXMROutBuffer->ulXmrLicenseOutputLength = ulXmrOutBufferLength;
    }
    pIrp->IoStatus.Information = sizeof(BDA_WMDRM_RENEWLICENSE) + ulXmrOutBufferLength -1;


    return Status;
}

/*
** PBDA_WMDRMGetLicense() method of the CFilter class
**
**    Return a License and return a PBDAResult.
**
*/
NTSTATUS
CFilter::PBDA_WMDRMGetLicense(
                                 IN PIRP         pIrp,
                                 IN PKSMETHOD    pKSMethod,
                                 OUT PBDA_BUFFER  pLicenseBuffer
                                 )
{
    NTSTATUS            Status = STATUS_SUCCESS;
    CFilter *           pFilter;
    ULONG               ulBufferLength;
    ULONG               ulLicenseBufferLength;
    ULONG               ulInBufferLength = 0;
    ULONG               ulOutBufferLength = 0;
    PIO_STACK_LOCATION  pIrpStack  = NULL;
    PKSM_BDA_WMDRM_LICENSE pLicenseInBuffer = (PKSM_BDA_WMDRM_LICENSE)pKSMethod; 

    ASSERT( pIrp);
    ASSERT( pKSMethod);
    // pSetDRMParam needs to be verified because minData is zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pLicenseBuffer)
    {
        pIrp->IoStatus.Information = sizeof(BDA_BUFFER);
        return STATUS_MORE_ENTRIES;
    }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::GetChangeState() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack) 
    {
        ulInBufferLength = pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
        ulOutBufferLength = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
    }
    else
    {
        return STATUS_UNSUCCESSFUL;
    }
    if (ulInBufferLength < sizeof(PKSM_BDA_WMDRM_LICENSE))
    {
        DbgPrint("PBDA_WMDRMRenewLicense: Input size %d too small",ulInBufferLength);
        return STATUS_UNSUCCESSFUL;
    }

    ulLicenseBufferLength = 2; //examble size, use the real size
    if (ulOutBufferLength < (sizeof(BDA_BUFFER) + ulLicenseBufferLength -1))
    {  //check if the buffer fits
        DbgPrint("PBDA_WMDRMGetLicense: Buffer too small, Output Length = %d\n", ulOutBufferLength);
        pIrp->IoStatus.Information = sizeof(BDA_BUFFER) + ulLicenseBufferLength -1;
        return STATUS_BUFFER_OVERFLOW;
    }

    DbgPrint("PBDA_WMDRMGetLicense: KeyUUID = %X-%X-%X-%X%X-%X%X%X%X%X%X Licenselen = %d\n", pLicenseInBuffer->uuidKeyID.Data1,
        pLicenseInBuffer->uuidKeyID.Data2,
        pLicenseInBuffer->uuidKeyID.Data3,
        pLicenseInBuffer->uuidKeyID.Data4[0],
        pLicenseInBuffer->uuidKeyID.Data4[1],
        pLicenseInBuffer->uuidKeyID.Data4[2],pLicenseInBuffer->uuidKeyID.Data4[3],pLicenseInBuffer->uuidKeyID.Data4[4],pLicenseInBuffer->uuidKeyID.Data4[5],pLicenseInBuffer->uuidKeyID.Data4[6],pLicenseInBuffer->uuidKeyID.Data4[7],
        pLicenseBuffer->ulBufferSize);

    if (pLicenseBuffer->ulBufferSize < ulLicenseBufferLength) //compare against real length available, 2 is just a test value
    {
        DbgPrint("PBDA_WMDRMGetLicense: Buffer too small Length = %d\n", pLicenseBuffer->ulBufferSize);
        pLicenseBuffer->ulBufferSize = ulLicenseBufferLength; //return the real package length
        pLicenseBuffer->lResult = BDA_E_BUFFER_TOO_SMALL;
        pIrp->IoStatus.Information = sizeof(BDA_BUFFER) + ulLicenseBufferLength -1;
        return Status;
    }

    pLicenseBuffer->lResult = 0;

    pLicenseBuffer->ulBufferSize = ulLicenseBufferLength; //return the real package length
    pLicenseBuffer->argbBuffer[0] = 0x77; //just a test byte
    pLicenseBuffer->argbBuffer[1] = 0x78; //just a test byte
    //memcpy into pLicenseBuffer->argbBuffer the License package of length pLicenseBuffer->ulBufferSize
    ulBufferLength = sizeof(BDA_BUFFER) + ulLicenseBufferLength -1; //return the correct IO buffer size

    pIrp->IoStatus.Information = ulBufferLength;

    return Status;
}

//=====================================================
// WMDRM TUNER methods
//=====================================================
/*
** PBDA_WMDRMTunerCancelCapToken() method of the CFilter class
**
**    Cancel a Capture Token and return a PBDAResult.
**
*/
NTSTATUS
CFilter::PBDA_WMDRMTunerCancelCapToken(
                           IN PIRP         pIrp,
                           IN PKSMETHOD    pKSMethod,
                           OUT PULONG      pulReturn
                           )
{
    NTSTATUS            Status = STATUS_SUCCESS;
    CFilter *           pFilter;

    ULONG               ulInputBufferSize = 0;
    ULONG               ulOutputBufferSize = 0;
    PIO_STACK_LOCATION  pIrpStack  = NULL;
    PKSM_BDA_BUFFER     pTokenBuffer = (PKSM_BDA_BUFFER)pKSMethod; 

    ASSERT( pIrp);
    ASSERT( pKSMethod);
    // pTokenBuffer needs to be verified because minData is zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pTokenBuffer)
    {
        pIrp->IoStatus.Information = sizeof(BDA_BUFFER);
        return STATUS_MORE_ENTRIES;
    }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);
    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
        ulInputBufferSize = pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
    }

    if (ulInputBufferSize < sizeof(KSM_BDA_BUFFER))
    {
        DbgPrint("PBDA_WMDRMTunerCancelCapToken: Input size %d too small",ulInputBufferSize);
        return STATUS_UNSUCCESSFUL;
    }

    if (ulOutputBufferSize < sizeof(PBDARESULT))
    {
        DbgPrint("PBDA_WMDRMTunerCancelCapToken: Output size %d too small",ulOutputBufferSize);
        pIrp->IoStatus.Information = sizeof(PBDARESULT);
        return STATUS_BUFFER_OVERFLOW;
    }

    DbgPrint("PBDA_WMDRMTunerCancelCapToken: BufferSize = %d\n", pTokenBuffer->ulBufferSize);
    //pTokenBuffer->argbBuffer contain the buffer 

    *pulReturn = 0;
    pIrp->IoStatus.Information = sizeof(PBDARESULT);

    return Status;
}

/*
** PBDA_WMDRMTunerSetPidProtection() method of the CFilter class
**
**    Set PID protection for a KeyUUID and return a PBDAResult.
**
*/
NTSTATUS
CFilter::PBDA_WMDRMTunerSetPidProtection(
                                       IN PIRP         pIrp,
                                       IN PKSMETHOD    pKSMethod,
                                       OUT PULONG      pulReturn
                                       )
{
    NTSTATUS            Status = STATUS_SUCCESS;
    CFilter *           pFilter;
    ULONG               ulInputBufferSize = 0;
    ULONG               ulOutputBufferSize = 0;
    PIO_STACK_LOCATION  pIrpStack  = NULL;
    PKSM_BDA_WMDRMTUNER_SETPIDPROTECTION pPidProtection = (PKSM_BDA_WMDRMTUNER_SETPIDPROTECTION)pKSMethod;

    ASSERT( pIrp);
    ASSERT( pKSMethod);
    // pulReturn needs to be verified because minData is zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pulReturn)
        {
        pIrp->IoStatus.Information = sizeof(PBDARESULT);
        return STATUS_MORE_ENTRIES;
        }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
        ulInputBufferSize = pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
    }

    if (ulInputBufferSize < sizeof(KSM_BDA_WMDRMTUNER_SETPIDPROTECTION))
    {
        DbgPrint("PBDA_WMDRMTunerSetPidProtection: Input size %d too small",ulInputBufferSize);
        return STATUS_UNSUCCESSFUL;
    }

    if (ulOutputBufferSize < sizeof(PBDARESULT))
    {
        DbgPrint("PBDA_WMDRMTunerSetPidProtection: Output size %d too small",ulOutputBufferSize);
        pIrp->IoStatus.Information = sizeof(PBDARESULT);
        return STATUS_BUFFER_OVERFLOW;
    }

    DbgPrint("PBDA_WMDRMTunerSetPidProtection: PID = %d KeyUUID = %X-%X-%X-%X%X-%X%X%X%X%X%X\n", pPidProtection->ulPID, 
                                                                        pPidProtection->uuidKeyID.Data1,
                                                                        pPidProtection->uuidKeyID.Data2,
                                                                        pPidProtection->uuidKeyID.Data3,
                                                                        pPidProtection->uuidKeyID.Data4[0],
                                                                        pPidProtection->uuidKeyID.Data4[1],
                                                                        pPidProtection->uuidKeyID.Data4[2],pPidProtection->uuidKeyID.Data4[3],pPidProtection->uuidKeyID.Data4[4],pPidProtection->uuidKeyID.Data4[5],pPidProtection->uuidKeyID.Data4[6],pPidProtection->uuidKeyID.Data4[7]
                                                                        );

    *pulReturn = 0;
    pIrp->IoStatus.Information = sizeof(PBDARESULT);

    return Status;
}

/*
** PBDA_WMDRMTunerGetPidProtection() method of the CFilter class
**
**    Get KeyUUID for a PID protection and return a PBDAResult.
**
*/
NTSTATUS
CFilter::PBDA_WMDRMTunerGetPidProtection(
    IN PIRP         pIrp,
    IN PKSMETHOD    pKSMethod,
    OUT PBDA_WMDRMTUNER_PIDPROTECTION pUUIDBuffer
    )
{
    NTSTATUS            Status = STATUS_SUCCESS;
    CFilter *           pFilter;
    ULONG               ulBufferLength;

    ULONG               ulInputBufferSize = 0;
    ULONG               ulOutputBufferSize = 0;
    PIO_STACK_LOCATION  pIrpStack  = NULL;
    PKSM_BDA_WMDRMTUNER_GETPIDPROTECTION pPIDBuffer = (PKSM_BDA_WMDRMTUNER_GETPIDPROTECTION) pKSMethod;

    ASSERT( pIrp);
    ASSERT( pKSMethod);
    // pPIDBuffer needs to be verified because minData is zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pUUIDBuffer)
    {
        pIrp->IoStatus.Information = sizeof(BDA_WMDRMTUNER_PIDPROTECTION);
        return STATUS_MORE_ENTRIES;
    }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
        ulInputBufferSize = pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
    }

    if (ulInputBufferSize < sizeof(KSM_BDA_WMDRMTUNER_GETPIDPROTECTION))
    {
        DbgPrint("PBDA_WMDRMTunerGetPidProtection: Input size %d too small",ulInputBufferSize);
        return STATUS_UNSUCCESSFUL;
    }

    if (ulOutputBufferSize < sizeof(BDA_WMDRMTUNER_PIDPROTECTION))
    {
        DbgPrint("PBDA_WMDRMTunerGetPidProtection: Output size %d too small",ulOutputBufferSize);
        pIrp->IoStatus.Information = sizeof(BDA_WMDRMTUNER_PIDPROTECTION);
        return STATUS_BUFFER_OVERFLOW;
    }
    DbgPrint("PBDA_WMDRMTunerGetPidProtection: PID = %d \n", pPIDBuffer->ulPID);

    pUUIDBuffer->lResult = 0;

    pUUIDBuffer->uuidKeyID = TEST_GUID;//return the key UUID

    ulBufferLength = sizeof(BDA_WMDRMTUNER_PIDPROTECTION);
    pIrp->IoStatus.Information = ulBufferLength;

    return Status;
}

/*
** PBDA_WMDRMTunerSetSyncValue() method of the CFilter class
**
**    Set the Sync Value and return a PBDAResult.
**
*/
NTSTATUS
CFilter::PBDA_WMDRMTunerSetSyncValue(
    IN PIRP         pIrp,
    IN PKSMETHOD    pKSMethod,
    OUT PULONG      pulReturn
    )
{
    NTSTATUS            Status = STATUS_SUCCESS;
    CFilter *           pFilter = NULL;

    UNREFERENCED_PARAMETER(pKSMethod);

    ULONG               ulInputBufferSize = 0;
    ULONG               ulOutputBufferSize = 0;
    PIO_STACK_LOCATION  pIrpStack  = NULL;
    PKSM_BDA_WMDRMTUNER_SYNCVALUE pSyncValueBuffer = (PKSM_BDA_WMDRMTUNER_SYNCVALUE) pKSMethod;

    ASSERT( pIrp);
    ASSERT( pKSMethod);
    // pSyncValueBuffer needs to be verified because minData is zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pSyncValueBuffer)
        {
        pIrp->IoStatus.Information = sizeof(KSM_BDA_WMDRMTUNER_SYNCVALUE);
        return STATUS_MORE_ENTRIES;
        }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
        ulInputBufferSize = pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
    }

    if (ulInputBufferSize < sizeof(KSM_BDA_WMDRMTUNER_SYNCVALUE))
    {
        DbgPrint("PBDA_WMDRMTunerSetSyncValue: Input size %d too small",ulInputBufferSize);
        return STATUS_UNSUCCESSFUL;
    }

    if (ulOutputBufferSize < sizeof(PBDARESULT))
    {
        DbgPrint("PBDA_WMDRMTunerSetSyncValue: Output size %d too small",ulOutputBufferSize);
        pIrp->IoStatus.Information = sizeof(PBDARESULT);
        return STATUS_BUFFER_OVERFLOW;
    }
    DbgPrint("PBDA_WMDRMTunerSetSyncValue: SyncValue = 0x%X\n", pSyncValueBuffer->ulSyncValue);

    *pulReturn = 0;
    pIrp->IoStatus.Information = sizeof(PBDARESULT);

    return Status;
}

/*
** PBDA_WMDRMRenewLicense() method of the CFilter class
**
**    Renew a License and return a PBDAResult.
**
*/
NTSTATUS
CFilter::PBDA_WMDRMTunerPurchaseEntitlement(
                                IN PIRP         pIrp,
                                IN PKSMETHOD    pKSMethod,
                                OUT PBDA_WMDRMTUNER_PURCHASEENTITLEMENT pPurchaseEntitlementOutBuffer
                                )
{
    NTSTATUS            Status = STATUS_SUCCESS;
    CFilter *           pFilter = NULL;
    ULONG               ulOutCaptureTokenLen = 10; //use the correct length of the CaptureToken, example 10Bytes

    ULONG               ulInputBufferSize = 0;
    ULONG               ulOutputBufferSize = 0;
    PIO_STACK_LOCATION  pIrpStack  = NULL;
    PKSM_BDA_WMDRMTUNER_PURCHASEENTITLEMENT pPurchaseEntitlementBuffer = (PKSM_BDA_WMDRMTUNER_PURCHASEENTITLEMENT)pKSMethod;

    ASSERT( pIrp);
    ASSERT( pKSMethod);
    // pPurchaseEntitlementOutBuffer needs to be verified because minData might be zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pPurchaseEntitlementOutBuffer)
    {
        pIrp->IoStatus.Information = sizeof(BDA_WMDRMTUNER_PURCHASEENTITLEMENT);
        return STATUS_MORE_ENTRIES;
    }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::GetChangeState() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
        ulInputBufferSize = pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
    }
    else
    {
        return STATUS_UNSUCCESSFUL;
    }
    if (ulInputBufferSize < sizeof(KSM_BDA_WMDRMTUNER_PURCHASEENTITLEMENT))
    {
        DbgPrint("PBDA_WMDRMPurchaseEntitlement: Input size %d too small",ulInputBufferSize);
        return STATUS_UNSUCCESSFUL;
    }

    if (ulOutputBufferSize < (sizeof(BDA_WMDRMTUNER_PURCHASEENTITLEMENT) + ulOutCaptureTokenLen -1))
    {
        DbgPrint("PBDA_WMDRMPurchaseEntitlement: Output size %d too small",ulOutputBufferSize);
        pIrp->IoStatus.Information = sizeof(BDA_WMDRMTUNER_PURCHASEENTITLEMENT) + ulOutCaptureTokenLen -1;
        return STATUS_BUFFER_OVERFLOW;
    }
    DbgPrint("PBDA_WMDRMPurchaseEntitlement: DialogRequest = %X ,Language = %s ,PurchaseTokenLen = %d\n", 
        pPurchaseEntitlementBuffer->ulDialogRequest,
        pPurchaseEntitlementBuffer->cLanguage,
        pPurchaseEntitlementBuffer->ulPurchaseTokenLength);

    //pPurchaseEntitlementBuffer->argbDataBuffer contain the PurchaseToken

    pPurchaseEntitlementOutBuffer->lResult = 0;
    pPurchaseEntitlementOutBuffer->ulDescrambleStatus = 0xA9A9C5C5;//return the correct descrambling status

    if (pPurchaseEntitlementBuffer->ulPurchaseTokenLength < ulOutCaptureTokenLen)
    {
        //the return buffer is not big enough, return the unchanged buffer
        pIrp->IoStatus.Information = sizeof(BDA_WMDRMTUNER_PURCHASEENTITLEMENT) + ulOutCaptureTokenLen -1;
        pPurchaseEntitlementOutBuffer->ulCaptureTokenLength = ulOutCaptureTokenLen; //return here the correct target length 
        pPurchaseEntitlementOutBuffer->lResult = BDA_E_BUFFER_TOO_SMALL;
    }
    else 
    {
        //copy the current CaptureToken into pPurchaseEntitlementOutBuffer->argbCaptureTokenBuffer
        pPurchaseEntitlementOutBuffer->argbCaptureTokenBuffer[0] = 0x10;
        //....
        pPurchaseEntitlementOutBuffer->argbCaptureTokenBuffer[9] = 0x19;
        pPurchaseEntitlementOutBuffer->ulCaptureTokenLength = ulOutCaptureTokenLen;
        pIrp->IoStatus.Information = sizeof(BDA_WMDRMTUNER_PURCHASEENTITLEMENT) + ulOutCaptureTokenLen - 1;
    }


    return Status;
}

NTSTATUS
CFilter::PBDA_GetWMDRMTunerStartCodeProfile(
    IN  PIRP         pIrp,
    IN  PKSMETHOD    pKSMethod,
    OUT PBDA_BUFFER  pProfileBuffer
    )
{
    NTSTATUS        Status = STATUS_SUCCESS;
    CFilter *       pFilter;
    ULONG           ulOutputBufferSize = 0;
    ULONG           ulStartCodeBufferSize = 3;//example size, use real size instead
    PIO_STACK_LOCATION pIrpStack  = NULL;

    UNREFERENCED_PARAMETER(pKSMethod);

    _DbgPrintF(DEBUGLVL_VERBOSE,("CFilter::GetWMDRMStatus"));

    ASSERT(pIrp);
    ASSERT(pKSMethod);


    // pDRMStatus needs to be verified because minData might be zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pProfileBuffer)
    {
        pIrp->IoStatus.Information = (ULONG) sizeof(BDA_BUFFER);
        return STATUS_MORE_ENTRIES;
    }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::GetChangeState() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
    }
    else
    {
        return STATUS_UNSUCCESSFUL;
    }
    if (ulOutputBufferSize < (sizeof(BDA_BUFFER) + ulStartCodeBufferSize -1))
    {
        DbgPrint("CFilter::GetWMDRMTunerStartCodeProfile Buffer too small");
        pIrp->IoStatus.Information = sizeof(BDA_BUFFER); 
        Status = STATUS_BUFFER_OVERFLOW;
    }
    else
    {

        if ( pProfileBuffer->ulBufferSize >= ulStartCodeBufferSize )     //compare against the required buffer
        {
            //only if the buffer is big enough we copy further list elements into pStartCodeProfile->argbBuffer
            pProfileBuffer->lResult = 0;
            pProfileBuffer->argbBuffer[0] = 0x01;
            pProfileBuffer->argbBuffer[1] = 0x02;
            pProfileBuffer->argbBuffer[2] = 0x03;
            pProfileBuffer->ulBufferSize = ulStartCodeBufferSize;
            pIrp->IoStatus.Information = sizeof(BDA_BUFFER) + ulStartCodeBufferSize -1;//add and return the correct length
            DbgPrint("CCapturePin::GetWMDRMTunerStartCodeProfile succeded");
        }
        else 
        {
            pIrp->IoStatus.Information = sizeof(BDA_BUFFER) + ulStartCodeBufferSize -1;
            //the buffer is not big enough, return the target size
            Status = STATUS_BUFFER_OVERFLOW;
            DbgPrint("CFilter::GetWMDRMTunerStartCodeProfile Buffer not big enough");
        }                     
    }

    return Status;
}

/*
** PBDA_MUX_SetPIDList() method of the CFilter class
**
**    Set PID List and return a PBDAResult.
**
*/
NTSTATUS
CFilter::PBDA_MUX_SetPIDList(
                       IN PIRP         pIrp,
                       IN PKSMETHOD    pKSMethod,
                       OUT PULONG      pulReturn
                       )
{
    NTSTATUS            Status = STATUS_SUCCESS;
    CFilter *           pFilter = NULL;
    ULONG               ulInputBufferSize = 0;
    ULONG               ulOutputBufferSize = 0;
    ULONG               ulPIDListSize = 0;
    PIO_STACK_LOCATION  pIrpStack  = NULL;

    ASSERT( pIrp);
    ASSERT( pKSMethod);
    PKSM_BDA_BUFFER pPIDListBuffer = (PKSM_BDA_BUFFER) pKSMethod;
    // pulReturn needs to be verified because minData is zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pulReturn)
    {
        pIrp->IoStatus.Information = sizeof(KSM_BDA_BUFFER);
        return STATUS_MORE_ENTRIES;
    }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::GetChangeState() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
        ulInputBufferSize = pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
    }

    if (ulInputBufferSize < sizeof(KSM_BDA_BUFFER))
    {
        DbgPrint("PBDA_MUX_SetPIDList: Input size %d too small",ulInputBufferSize);
        return STATUS_UNSUCCESSFUL;
    }

    if (ulOutputBufferSize < sizeof(ULONG))
    {
        DbgPrint("PBDA_MUX_SetPIDList: Output size %d too small",ulOutputBufferSize);
        pIrp->IoStatus.Information = sizeof(ULONG);
        return STATUS_BUFFER_OVERFLOW;
    }

    ulPIDListSize = pPIDListBuffer->ulBufferSize / sizeof(BDA_MUX_PIDLISTITEM);
    DbgPrint("PBDA_MUX_SetPIDList: PIDListSize = %d\n", ulPIDListSize);

    BDA_MUX_PIDLISTITEM *pPIDList = (BDA_MUX_PIDLISTITEM *)pPIDListBuffer->argbBuffer;
    //pPIDListBuffer->argbBuffer contain the list of PIDItems, process it
    *pulReturn = 0;  //return the PBDARESULT

    ULONG i;
    char * MessageStr;
    char ValueStr[20];
    size_t MessageStrLen;
    ULONG ulDebugStrSize = ulPIDListSize * 20 + 20;

    MessageStr = (LPSTR)ExAllocatePoolWithTag(NonPagedPool,
        ulDebugStrSize + 1, //string long enough for starttext + PID.Type.ProgramNumber 
        MS_SAMPLE_TUNER_POOL_TAG
        );
        
    if (NULL == MessageStr)
    {
        return STATUS_UNSUCCESSFUL;
    }
    
    Status = RtlStringCbCopyA(MessageStr, ulDebugStrSize, "MUX-PIDList:");
    for (i=0 ; i < ulPIDListSize ; i++)
    {
        if (Status != STATUS_SUCCESS) break;
        Status = RtlStringCbPrintfA(ValueStr, 20, "%d.%d.%d;",pPIDList[i].usPIDNumber,pPIDList[i].ePIDType,pPIDList[i].usProgramNumber);
        if (Status != STATUS_SUCCESS) break;
        Status = RtlStringCbCatNA(MessageStr,ulDebugStrSize,ValueStr,20);
        if (Status != STATUS_SUCCESS) break;
        //test MMI on a specific service
        if ((pPIDList[i].usPIDNumber == 168) && (pPIDList[i].ePIDType == PID_ELEMENTARY_STREAM) && (pPIDList[i].usProgramNumber == 12095))//RTL Shop
        {
            pFilter->SendBroadcastMMI(0);
        }
        if ((pPIDList[i].usPIDNumber == 168) && (pPIDList[i].ePIDType == PID_ELEMENTARY_STREAM) && (pPIDList[i].usProgramNumber == 12080))//RTL Shop
        {
            pFilter->SendBroadcastMMI(0);
        }
        if ((pPIDList[i].usPIDNumber == 1101) && (pPIDList[i].ePIDType == PID_ELEMENTARY_STREAM) && (pPIDList[i].usProgramNumber == 28211))
        {
            pFilter->SendBroadcastMMI(1);
        }
        if ((pPIDList[i].usPIDNumber == 701) && (pPIDList[i].ePIDType == PID_ELEMENTARY_STREAM) && (pPIDList[i].usProgramNumber == 28112))
        {
            pFilter->SendBroadcastMMI(1);
        }
        if (((pPIDList[i].usPIDNumber == 301)||(pPIDList[i].usPIDNumber == 401)) && (pPIDList[i].ePIDType == PID_ELEMENTARY_STREAM) && (pPIDList[i].usProgramNumber == 28229))
        {
            pFilter->SendBroadcastMMI(2);
        }

    }
    Status = RtlStringCbLengthA(MessageStr,ulDebugStrSize,&MessageStrLen);
    
    if ((STATUS_SUCCESS != Status) || (!MessageStrLen))
    {
        ExFreePoolWithTag(MessageStr, MS_SAMPLE_TUNER_POOL_TAG);
        return STATUS_BUFFER_OVERFLOW;
    }
    
    KIRQL OldIrql;
    KeRaiseIrql(DISPATCH_LEVEL, &OldIrql);

    KIRQL               Irql;
    KeAcquireSpinLock( &pFilter->spinLock, &Irql);

    if (pFilter->m_FilterEventsManager.IsEnabled() && pFilter->m_FilterDebugEventsManager.IsEnabled())
    {
        Status = pFilter->m_FilterDebugEventsManager.QueueDebugMessage(0,(ULONG) MessageStrLen,(UCHAR *)MessageStr); 
    }
    else
    {
        DbgPrint("PBDA_MUX_SetPIDList Debug Service not enabled!");
    }

    KeReleaseSpinLock( &pFilter->spinLock, Irql);
    KeLowerIrql(OldIrql);

    ExFreePoolWithTag(MessageStr, MS_SAMPLE_TUNER_POOL_TAG);

    pIrp->IoStatus.Information = (ULONG) sizeof(PBDARESULT);

    return Status;
}

/*
** PBDA_MUX_GetPIDList() method of the CFilter class
**
**    Return the current PIDList.
**
*/
NTSTATUS
CFilter::PBDA_MUX_GetPIDList(
                              IN PIRP         pIrp,
                              IN PKSMETHOD    pKSMethod,
                              OUT PBDA_BUFFER pPIDListBuffer
                              )
{
    NTSTATUS        Status = STATUS_SUCCESS;
    CFilter *       pFilter;
    ULONG           ulOutputBufferSize = 0;
    ULONG           ulPIDListBufferSize = sizeof(BDA_MUX_PIDLISTITEM) * 2;//example size 2 PIDs, use real size instead
    PIO_STACK_LOCATION pIrpStack  = NULL;

    UNREFERENCED_PARAMETER(pKSMethod);

    _DbgPrintF(DEBUGLVL_VERBOSE,("CFilter::PBDA_MUX_GetPIDList"));

    ASSERT(pIrp);
    ASSERT(pKSMethod);


    // pPIDListBuffer needs to be verified because minData might be zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pPIDListBuffer)
    {
        pIrp->IoStatus.Information = sizeof(BDA_BUFFER);
        return STATUS_MORE_ENTRIES;
    }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::GetChangeState() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
    }
    else
    {
        return STATUS_UNSUCCESSFUL;
    }
    if (ulOutputBufferSize < (sizeof(BDA_BUFFER) + ulPIDListBufferSize -1))
    {
        DbgPrint("CFilter::PBDA_MUX_GetPIDList Buffer too small");
        pIrp->IoStatus.Information = sizeof(BDA_BUFFER); 
        Status = STATUS_BUFFER_OVERFLOW;
    }
    else
    {

        if ( pPIDListBuffer->ulBufferSize >= ulPIDListBufferSize )     //compare against the required buffer
        {
            //only if the buffer is big enough we copy further list elements into pPIDListBuffer->argbBuffer
            pPIDListBuffer->lResult = 0;
            BDA_MUX_PIDLISTITEM *pPIDList = (BDA_MUX_PIDLISTITEM *)pPIDListBuffer->argbBuffer;
            pPIDList[0].usPIDNumber = 0;
            pPIDList[0].ePIDType = PID_MPEG2_SECTION_PSI_SI;
            pPIDList[0].usProgramNumber = 0;
            pPIDList[1].usPIDNumber = 0x800;
            pPIDList[1].ePIDType = PID_ELEMENTARY_STREAM;
            pPIDList[1].usProgramNumber = 28201;
            pPIDListBuffer->ulBufferSize = ulPIDListBufferSize;
            pIrp->IoStatus.Information = sizeof(BDA_BUFFER) + ulPIDListBufferSize -1;//add and return the correct length
            DbgPrint("CFilter::PBDA_MUX_GetPIDList succeded");
        }
        else 
        {
            pIrp->IoStatus.Information = sizeof(BDA_BUFFER) + ulPIDListBufferSize -1;
            //the buffer is not big enough, return the target size
            Status = STATUS_BUFFER_OVERFLOW;
            DbgPrint("CFilter::PBDA_MUX_GetPIDList Buffer not big enough");
        }                     
    }

    return Status;
}

#if defined(DMBTH_RECEIVER)

 /*
 **     PBDA wcKeyCharsCmp  method of the CFilter class
 **     Safe Cut WCHAR,StartLen  is The beginning of the location of pSourceStr
 **     Return SourceStr and pKeyStr compare true or false .and return pParmStr and len(pParmStr).
 **    
 */
bool CFilter::GetDMBTHCommandParams(WCHAR *pSourceStr,const WCHAR *pKeyStr,WCHAR *pParmStr)
{
    if ((!pSourceStr) || (!pKeyStr))
    {
        DbgPrint("PBDA:WcKeyCmp Parameters Err  !!\n");
        return false;
    }
	
     DbgPrint("CFilter::COMMAND LEN pSourceStr \"%ws\" \n",pSourceStr);
//PBDA://2734dc82-774d-4daa-a5ad-2838b5a58127/Gpnvs Command LFILE:Diagnostics_Signal.mcml#SearchSignal	 
     WCHAR wctempBuffer[GPNVS_COMMAND_LEN]={0}; 
     int strLength = wcslen(pSourceStr);
     int strKeyLength = wcslen(pKeyStr);
	 
     strLength -=wcslen(GPNVS_CASE_COMMAND); //del gpnvs _case_command Len

      if ( strLength >= strKeyLength + 1)    //Include a Parm  include a space
      {
          strLength -= strKeyLength; //don't include a space.

          RtlStringCchCopyW(wctempBuffer,strKeyLength+1,&pSourceStr[wcslen(GPNVS_CASE_COMMAND)+1]);//6 is Len(" LFILE:")

          DbgPrint("PBDA:CutWcChars OutBuff  \"%ws\" !!\n",wctempBuffer);

         if (_wcsicmp(wctempBuffer,pKeyStr) == 0)
         {

               strLength -= 1; // del a space

               RtlStringCchCopyW(pParmStr,strLength+1,&pSourceStr[wcslen(GPNVS_CASE_COMMAND)+strKeyLength+1]);

               DbgPrint("PBDA:CutWcChars OutBuff  \"%ws\" !!\n",pParmStr);
               return true;
         }
         else
         {

               DbgPrint("PBDA:CutWcChars fail!!\n");
               return false;
         }

      }
      else
      {
         return false;
      }

}

NTSTATUS
CFilter::GetSignalStatus(IN PIRP  pIrp,__in_ecount(ulSourceLen) PWSTR pSourceStr, ULONG ulSourceLen,__in_ecount(*pulValueStrLen) PWSTR pValueStr, ULONG *pulValueStrLen)
{
   CFilter *       pFilter;

   UNREFERENCED_PARAMETER(pIrp);

   if ((!pSourceStr) || (!ulSourceLen) || (!pValueStr) || (!pulValueStrLen))
   {
      return STATUS_UNSUCCESSFUL;
   }

   ASSERT(pIrp);

   ULONG  NewBufferLength=0;


   pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
   ASSERT( pFilter);
// get hw signal status and replace In Out buff
// this is a demo that show Signal status changes from time to time.

   PCSTR    SignalTag1 = "0.101";// 0.101 is demo key. in Mcml file
   char       SignalValueStr1[] = "10000";//SignalQuality=SignalStrength
   PCSTR    SignalTag2 = "0.102";// 0.102 is demo key.in Mcml file
   char       SignalValueStr2[] = "10000";//SignalQuality=SignalStrength
   PCSTR    SignalTag3 = "0.103";// 0.103 is demo key.in Mcml file
   char       SignalValueStr3[] = "10000";//SignalQuality=SignalStrength
   PCSTR    SignalTag4 = "0.104";// 0.104 is demo key.in Mcml file
   char       SignalValueStr4[] = "10000";//SignalQuality=SignalStrength
   PCSTR    SignalTag5 = "0.105";// 0.105 is demo key.in Mcml file
   char       SignalValueStr5[] = "10000";//SignalQuality=SignalStrength
   PCSTR    SignalTag6 = "0.106";// 0.106 is demo key.in Mcml file
   char       SignalValueStr6[] = "10000";//SignalQuality=SignalStrength
// 1.
   RtlStringCbPrintfA(SignalValueStr1,sizeof(SignalValueStr1),"0.%d00",1 + rand() % 10);
   ReplaceDMBTHXML(pSourceStr,ulSourceLen,SignalTag1,(WCHAR*)SignalValueStr1,pValueStr,&NewBufferLength);
// 2.
   RtlStringCbPrintfA(SignalValueStr2,sizeof(SignalValueStr2),"0.%d00",1 + rand() % 10);
   ReplaceDMBTHXML(pSourceStr,ulSourceLen,SignalTag2,(WCHAR*)SignalValueStr2,pValueStr,&NewBufferLength);
// 3.
   RtlStringCbPrintfA(SignalValueStr3,sizeof(SignalValueStr3),"0.%d00",1 + rand() % 10);
   ReplaceDMBTHXML(pSourceStr,ulSourceLen,SignalTag3,(WCHAR*)SignalValueStr3,pValueStr,&NewBufferLength);
// 4.
   RtlStringCbPrintfA(SignalValueStr4,sizeof(SignalValueStr4),"0.%d00",1 + rand() % 10);
   ReplaceDMBTHXML(pSourceStr,ulSourceLen,SignalTag4,(WCHAR*)SignalValueStr4,pValueStr,&NewBufferLength);
// 5.
   RtlStringCbPrintfA(SignalValueStr5,sizeof(SignalValueStr5),"0.%d00",1 + rand() % 10);
   ReplaceDMBTHXML(pSourceStr,ulSourceLen,SignalTag5,(WCHAR*)SignalValueStr5,pValueStr,&NewBufferLength);
// 6.
   RtlStringCbPrintfA(SignalValueStr6,sizeof(SignalValueStr6),"0.%d00",1 + rand() % 10);
   ReplaceDMBTHXML(pSourceStr,ulSourceLen,SignalTag6,(WCHAR*)SignalValueStr6,pValueStr,&NewBufferLength);

  if (NewBufferLength != wcslen(pSourceStr))
  {
    DbgPrint(" CFilter::GetSignalStatus New len %d , Source Len   %d\n",NewBufferLength,wcslen(pSourceStr));
    return STATUS_UNSUCCESSFUL;
  }

//Keep sizeof(pSourceStr) no change.
    *pulValueStrLen = wcslen(pSourceStr);

    return STATUS_SUCCESS;
}


 /*
 ** PBDA ReplaceXMLString  method of the CFilter class
 **
 **    Return Replaced new WCHAR BUFFER.
 **
 */

NTSTATUS
CFilter::ReplaceDMBTHXML(WCHAR *pSourceStr, ULONG ulSourceLen,PCSTR pKeyStr,WCHAR *pTargetStr,WCHAR *pValueStr, ULONG *pulValueStrLen)
{
     if ((!pSourceStr)|| (!ulSourceLen) || (!pKeyStr)  || (!pTargetStr) || (!pValueStr) || (!pulValueStrLen))
     {
          *pulValueStrLen = ulSourceLen;
          return STATUS_UNSUCCESSFUL;
     }

     ULONG strInLength = wcslen(pSourceStr);
     int strTargetLength = wcslen(pTargetStr);
     int strInKeyLength = strlen(pKeyStr);
     ULONG i = 0;
     int j = 0;

     if ( (strInKeyLength < strTargetLength) || (strInKeyLength > GPNVS_COMMAND_LEN))
     {
          DbgPrint("PBDA Targe BUFF To Big !! InKey(%d) TargeLen(%d) \"%ws\"\n",strInKeyLength,strTargetLength,pTargetStr);    
          return STATUS_UNSUCCESSFUL;
     }

     while ((i < strInLength) || (i < 30000)) //< 30000 wchar ,if need replace more char ,please del this or chang it
     {
         if (pSourceStr[i] == pKeyStr[0]) //one key
         {
             for ( j = 0; j <= strTargetLength; j++)
             {
                if (pSourceStr[j+i] != pKeyStr[j])
                {
                     break;
                }
             }

             if ( j == strTargetLength)
             {
                // DbgPrint("PBDA:find Key string\n");

                for ( j = 0; j < strInKeyLength; j++)
                {
                    pSourceStr[j+i]=pTargetStr[j];
                }
                           //break; //Replace once or all
             }
         }
         i++;
     }

    *pulValueStrLen = wcslen(pSourceStr);
          
    return STATUS_SUCCESS;
}
#endif
 /*
 ** PBDA GetNameValue() method of the CFilter class
 **
 **    Return the Value of a Name.
 **
 */
 NTSTATUS
 CFilter::GetNameValue(
                         IN PIRP         pIrp,
                         IN PKSMETHOD    pKSMethod,
                         OUT BDA_STRING  *pData
                         )
{
    NTSTATUS        Status = STATUS_SUCCESS;
    CFilter *       pFilter;
    KSM_BDA_GPNV_GETVALUE *pNameBuffer = (KSM_BDA_GPNV_GETVALUE *)pKSMethod;
    ULONG           ulOutputBufferSize = 0;
    ULONG           ulDataValueBufferSize = 0; //min size of the Value Buffer
    PIO_STACK_LOCATION pIrpStack  = NULL;


    _DbgPrintF(DEBUGLVL_VERBOSE,("CFilter::GetNameValue"));

    ASSERT(pIrp);
    ASSERT(pKSMethod);


    // pData needs to be verified because minData might be zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pData)
    {
        pIrp->IoStatus.Information = sizeof(BDA_STRING);
        return STATUS_MORE_ENTRIES;
    }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::GetChangeState() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
    }
    else
    {
        return STATUS_UNSUCCESSFUL;
    }
    if (ulOutputBufferSize < (sizeof(BDA_STRING) + ulDataValueBufferSize -1))
    {
        DbgPrint("CFilter::GetNameValue Buffer too small");
        pIrp->IoStatus.Information = sizeof(BDA_STRING) - 1 + ulDataValueBufferSize; 
        Status = STATUS_BUFFER_OVERFLOW;
    }
    else
    {

        if ( pData->ulStringSize >= ulDataValueBufferSize )     //compare against the required buffer
        {
            int i = 0;
            int done = 0;
//GPNVS AOMMAND OF DMBTH ADD setp.1
//GPNV values have to be Base64 encoded.
#if defined(USE_GPNV_UNICODE) && defined(DMBTH_RECEIVER)
            BYTE*           pInBuffer[GPNVS_COMMAND_LEN];//Main command
            BYTE*           pInBase64Buffer[GPNVS_COMMAND_LEN];//Base64 command
            WCHAR           wcKeyBuffer[GPNVS_COMMAND_LEN]={0};//Key
            WCHAR           wcParmBuffer[GPNVS_COMMAND_LEN]={0};//Parm
            WCHAR           wcBase64Parm[GPNVS_COMMAND_LEN]={0};//Key Parm
            ULONG           ulOEMMCMLFileSize=0;
            UNICODE_STRING  OEMMCMLFileName;// Mcml file path
            ULONG           ulBase64NameLen;

            ulBase64NameLen = sizeof(L"PBDA://2734dc82-774d-4daa-a5ad-2838b5a58127/Microsoft.com/NV/Variable/MCE/Assets/Base64Binary/");
            RtlStringCbCopy(wcBase64Parm,ulBase64NameLen + sizeof(WCHAR), L"PBDA://2734dc82-774d-4daa-a5ad-2838b5a58127/Microsoft.com/NV/Variable/MCE/Assets/Base64Binary/");

            memset(pInBase64Buffer,0,sizeof(pInBase64Buffer));
            memcpy(pInBase64Buffer,pNameBuffer->argbData,ulBase64NameLen-sizeof(WCHAR));

            memset(pInBuffer,0,sizeof(pInBuffer));
            memcpy(pInBuffer,pNameBuffer->argbData,sizeof(GPNVS_CASE_COMMAND)-sizeof(WCHAR));

#endif
            for (i = 0; done == 0 && i < GPNVS_NUMBER_OF_ROWS; i++)
            {
                pData->argbString[0] = 0;
                
                DbgPrint("CFilter::GetNameValueU comparing \"%ws\" to \"%ws\"", (WCHAR *)pNameBuffer->argbData, pFilter->m_GpnvsTable[i][0]);
                if (_wcsicmp( (WCHAR*)pNameBuffer->argbData, pFilter->m_GpnvsTable[i][0]) == 0)
                {
                    //For Diagnostic ready we try to load the mcml code from file
                    if(i == 29)
                    {
                        PKSFILTER pKsFilter = KsGetFilterFromIrp( pIrp );
                        if( pKsFilter == NULL )
                            return STATUS_INVALID_PARAMETER;

                        PKSDEVICE Device = KsFilterGetDevice(pKsFilter);
                        CDevice* pDevice = reinterpret_cast<CDevice *>(Device->Context);

                        if(pDevice)
                        {
                            ULONG ulMCMLFileSize=0;
                            UNICODE_STRING MCMLFileName;
                            RtlInitUnicodeString(&MCMLFileName,L"DiagnosticsReady.mcml");

                            NTSTATUS status = pDevice->ReadMCMLFromFile(NULL, &ulMCMLFileSize, MCMLFileName);

                            if (ulMCMLFileSize == 0 || !NT_SUCCESS(status))//file does  not exits
                            {
                                DbgPrint("CFilter::GetNameValue No File Use ulMCMLFileSize size=%d",ulMCMLFileSize);
                            }
                            else
                            {
                                DbgPrint("CFilter::SetNameValue File Use ulMCMLFileSize size=%d",ulMCMLFileSize);
                                WCHAR *cValueStrTmp = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulMCMLFileSize+sizeof(WCHAR) , MS_SAMPLE_TUNER_POOL_TAG);
                                cValueStrTmp[ulMCMLFileSize/sizeof(WCHAR)]=0;

                                status = pDevice->ReadMCMLFromFile((BYTE*)cValueStrTmp, &ulMCMLFileSize, MCMLFileName);

                                if(NT_SUCCESS(status))
                                {
                                    pFilter->SetGPNVValue(cValueStrTmp, 29); 
                                }
                                else
                                {
                                    ExFreePoolWithTag(cValueStrTmp, MS_SAMPLE_TUNER_POOL_TAG);
                                }
                            }
                        }
                    }

                    // if we have data in the table then we return, otherwise we return nothing
                    
                    int strLength = wcslen(pFilter->m_GpnvsTable[i][1]);
                    if (strLength != 0)
                    {
                        DbgPrint("Returning value of \"%ws\"", pFilter->m_GpnvsTable[i][1]);
                        ulDataValueBufferSize = (strLength + 1) * sizeof(WCHAR);
                        DbgPrint("ulDataValueBufferSize = %d\n", ulDataValueBufferSize);
                        if ( pData->ulStringSize >= ulDataValueBufferSize )     //compare against the required buffer
                        {
                            RtlStringCbCopy((WCHAR *)pData->argbString,ulDataValueBufferSize, pFilter->m_GpnvsTable[i][1]);
                        }
                        else
                        {
                            //the buffer is not big enough, return the target size
                            Status = STATUS_BUFFER_OVERFLOW;
                            DbgPrint("CFilter::GetNameValue Buffer not big enough");
                        }
                    }
                    done = 1;
                    pData->lResult = 0; //return the right PBDARESULT
                    DbgPrint("CFilter::GetNameValue Language=%s Name=%ws Value=%ws succeded",pNameBuffer->cLanguage,(WCHAR *)pNameBuffer->argbData, (WCHAR *)pData->argbString);
                }
#ifdef DMBTH_RECEIVER    //GPNVS AOMMAND OF DMBTH ADD setp.2
                else if(_wcsicmp( (WCHAR*)pInBuffer,GPNVS_CASE_COMMAND) == 0)
                {
                    DbgPrint("PBDA: \n CFilter::GPNVS COMMAND Url=\"%ws\"  Mask= \"%ws\"  \n",(WCHAR*)pNameBuffer->argbData,(WCHAR*)pInBuffer);

                    RtlStringCbCopy(wcKeyBuffer,sizeof(wcKeyBuffer),(WCHAR *)pNameBuffer->argbData);

                    int strLength = wcslen(wcKeyBuffer);

                    PKSFILTER pKsFilter = KsGetFilterFromIrp( pIrp );

                    if( pKsFilter == NULL )
                    {
                        return STATUS_INVALID_PARAMETER;
                    }

                    PKSDEVICE Device = KsFilterGetDevice(pKsFilter);
                    CDevice* pDevice = reinterpret_cast<CDevice *>(Device->Context);
						
                    RtlZeroMemory(&OEMMCMLFileName, sizeof(OEMMCMLFileName));

                    strLength -=wcslen(GPNVS_CASE_COMMAND);

                    if(GetDMBTHCommandParams(wcKeyBuffer,L"LFILE:",wcParmBuffer))
                    {
                        RtlInitUnicodeString(&OEMMCMLFileName,wcParmBuffer);

                        NTSTATUS status = pDevice->ReadOEMMCMLFile(NULL, &ulOEMMCMLFileSize, OEMMCMLFileName);

                        if (ulOEMMCMLFileSize == 0 || !NT_SUCCESS(status))//file does  not exits
                        {
                            DbgPrint("PBDA: CFilter::GetNameValue No File Use ulMCMLFileSize size=%d \n",ulOEMMCMLFileSize);
                        }
                        else
                        {
                            WCHAR *cValueStrTmp = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulOEMMCMLFileSize+sizeof(WCHAR) , MS_SAMPLE_TUNER_POOL_TAG);
                            cValueStrTmp[ulOEMMCMLFileSize/sizeof(WCHAR)]=0;

                            status = pDevice->ReadOEMMCMLFile((BYTE*)cValueStrTmp, &ulOEMMCMLFileSize, OEMMCMLFileName);

                            if(NT_SUCCESS(status))
                            {// copy to out Buffer
                              // if we have data in the table then we return, otherwise we return nothing
                               int strLength = wcslen(cValueStrTmp);

                               ulDataValueBufferSize = (strLength + 1) * sizeof(WCHAR);

                               if ( pData->ulStringSize >= ulDataValueBufferSize )     //compare against the required buffer
                               {
                                   RtlStringCbCopy((WCHAR *)pData->argbString,ulDataValueBufferSize,(WCHAR *)cValueStrTmp);
                               }
                               else
                               {
                                   //the buffer is not big enough, return the target size
                                   Status = STATUS_BUFFER_OVERFLOW;

                                   DbgPrint("PBDA: CFilter::GetNameValue Buffer not big enough");
                               }

                            }				
                            ExFreePoolWithTag(cValueStrTmp, MS_SAMPLE_TUNER_POOL_TAG);
                        }//*/// Read File
                         done = 1;
                         pData->lResult = 0; //return the right PBDARESULT	

                    }
                    else if(GetDMBTHCommandParams(wcKeyBuffer,L"SCAN:",wcParmBuffer))
                    {
                           RtlInitUnicodeString(&OEMMCMLFileName,wcParmBuffer);

                           NTSTATUS status = pDevice->ReadOEMMCMLFile(NULL, &ulOEMMCMLFileSize, OEMMCMLFileName);

                           if (ulOEMMCMLFileSize == 0 || !NT_SUCCESS(status))//file does  not exits
                           {
                               DbgPrint("CFilter::GetNameValue No File Use ulMCMLFileSize size=%d \n",ulOEMMCMLFileSize);
                           }
                           else
                           {
                               WCHAR *cValueStrTmp = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulOEMMCMLFileSize+sizeof(WCHAR) , MS_SAMPLE_TUNER_POOL_TAG);
                               cValueStrTmp[ulOEMMCMLFileSize/sizeof(WCHAR)]=0;

                               status = pDevice->ReadOEMMCMLFile((BYTE*)cValueStrTmp, &ulOEMMCMLFileSize, OEMMCMLFileName);

                               if(NT_SUCCESS(status))
                               {// copy to out Buffer
                                  // if we have data in the table then we return, otherwise we return nothing
                                   int strLength = wcslen(cValueStrTmp);
                                   ULONG strtempLength = 0;
                                   // Change buff for signal status.
                                   pFilter->GetSignalStatus(pIrp,(WCHAR*)cValueStrTmp,wcslen(cValueStrTmp),(WCHAR*)cValueStrTmp,&strtempLength);
                                   if((int)strtempLength == strLength) 
                                   {
                                       DbgPrint(" Buffer wsLen is right = %d \n",strtempLength);
                                   }

                                   ulDataValueBufferSize = (strLength + 1) * sizeof(WCHAR);

                                   if ( pData->ulStringSize >= ulDataValueBufferSize )     //compare against the required buffer
                                   {
                                       DbgPrint("CFilter::GetNameValue pData->ulStringSize >= ulDataValueBufferSize \n");
                                       RtlStringCbCopy((WCHAR *)pData->argbString,ulDataValueBufferSize,(WCHAR *)cValueStrTmp);
                                   }
                                   else
                                   {
                                       //the buffer is not big enough, return the target size
                                       Status = STATUS_BUFFER_OVERFLOW;

                                       DbgPrint("CFilter::GetNameValue Buffer not big enough");
                                  }

                              }				
                               ExFreePoolWithTag(cValueStrTmp, MS_SAMPLE_TUNER_POOL_TAG);
                           }// Read File
                           done = 1;
                           pData->lResult = 0; //return the right PBDARESULT

                    }
					
                }
                else if(_wcsicmp((WCHAR*)pInBase64Buffer,wcBase64Parm) == 0)    //this is Test Codes,it's Need to modify
                {
                    PKSFILTER pKsFilter = KsGetFilterFromIrp( pIrp );
                    if( pKsFilter == NULL )
                    return STATUS_INVALID_PARAMETER;

                    PKSDEVICE Device = KsFilterGetDevice(pKsFilter);
                    CDevice* pDevice = reinterpret_cast<CDevice *>(Device->Context);
                    WCHAR          wcTempParmBuffer[GPNVS_COMMAND_LEN]={0};//Parm
                    WCHAR          wcBase64ParmBuffer[GPNVS_COMMAND_LEN]={0};//Parm                    
                    ULONG           ulBase64ParmSize=0;

                    RtlStringCbCopy(wcTempParmBuffer,sizeof(wcTempParmBuffer),(WCHAR *)pNameBuffer->argbData);//Key <=6 bit

                    ulBase64ParmSize=(sizeof(wcTempParmBuffer)-ulBase64NameLen)/sizeof(WCHAR);

                    RtlStringCchCopyW(wcBase64ParmBuffer,sizeof(L"Images\\"),L"Images\\");

                    RtlStringCchCatW(wcBase64ParmBuffer,ulBase64ParmSize+1,&wcTempParmBuffer[ulBase64NameLen/sizeof(WCHAR)-1]);

                    DbgPrint("PBDA: \n CFilter::GetNameValue Local 1 Picture name \"%ws\" \n",wcBase64ParmBuffer);

                    RtlInitUnicodeString(&OEMMCMLFileName,wcBase64ParmBuffer);    //L"Logo.png"

                    NTSTATUS status = pDevice->ReadOEMMCMLFile(NULL, &ulOEMMCMLFileSize, OEMMCMLFileName);

                    if (ulOEMMCMLFileSize == 0 || !NT_SUCCESS(status))//file does  not exits
                    {
                        DbgPrint("PBDA: \n CFilter::GetNameValue No File Use ulMCMLFileSize size=%d \n",ulOEMMCMLFileSize);
                    }
                    else
                    {
                        DbgPrint("PBDA: CFilter::GetNameValue File Use ulMCMLFileSize size=%d \n",ulOEMMCMLFileSize);

                        WCHAR *cValueStrTmp = (WCHAR*)ExAllocatePoolWithTag(NonPagedPool, ulOEMMCMLFileSize +sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);

                        cValueStrTmp[ulOEMMCMLFileSize/ sizeof(WCHAR)]=0;

                        status = pDevice->ReadOEMMCMLFile((BYTE*)cValueStrTmp, &ulOEMMCMLFileSize, OEMMCMLFileName);

                        if(NT_SUCCESS(status))
                        { 
                            // copy to out Buffer
                            // if we have data in the table then we return, otherwise we return nothing
                            // the buffer have to be base64.
                            ulDataValueBufferSize = ulOEMMCMLFileSize;

                            if ( pData->ulStringSize >= ulDataValueBufferSize )     //compare against the required buffer
                            {
                                RtlStringCbCopy((WCHAR *)pData->argbString,ulDataValueBufferSize,(WCHAR *)(cValueStrTmp+1));

                                DbgPrint("PBDA: \n WCHAR *cValueStrTmp \"%ws\"\n",(WCHAR *)cValueStrTmp);
                            }
                            else
                            {
                               //the buffer is not big enough, return the target size
                                Status = STATUS_BUFFER_OVERFLOW;

                                DbgPrint("PBDA: \n CFilter::GetNameValue Buffer not big enough");
                            }

                        }                 
                            ExFreePoolWithTag(cValueStrTmp, MS_SAMPLE_TUNER_POOL_TAG);
                    }
                      done = 1;
                      pData->lResult = 0; //return the right PBDARESULT  
                }//*/

#endif // DMBTH
            }           
            
            if (done == 0)
            {
                DbgPrint("BDA_E_NOT_FOUND is %x",BDA_E_NOT_FOUND); 
                DbgPrint("CFilter::GetNameValue Language=%s Name=%ws failed!",pNameBuffer->cLanguage,(WCHAR *)pNameBuffer->argbData);
                pData->lResult = BDA_E_NOT_FOUND; //return the right PBDARESULT
            }
            pData->ulStringSize = ulDataValueBufferSize;
            pIrp->IoStatus.Information = sizeof(BDA_STRING) + ulDataValueBufferSize -1;//add and return the correct length
            
        }
        else 
        {
            pIrp->IoStatus.Information = sizeof(BDA_STRING) + ulDataValueBufferSize -1;
            //the buffer is not big enough, return the target size
            Status = STATUS_BUFFER_OVERFLOW;
            DbgPrint("CFilter::GetNameValue Buffer not big enough");
        }                     
    }

    return Status;
}

/*
** PBDA SetNameValue() method of the CFilter class
**
**    Set the Value of a Name and return a PBDAResult.
**
*/
NTSTATUS
CFilter::SetNameValue(
                      IN PIRP         pIrp,
                      IN PKSMETHOD    pKSMethod,
                      OUT PULONG      pulReturn
                      )
{
    NTSTATUS            Status = STATUS_SUCCESS;
    CFilter *           pFilter;
    ULONG               ulInputBufferSize = 0;
    ULONG               ulOutputBufferSize = 0;
    PIO_STACK_LOCATION  pIrpStack  = NULL;
    PBDARESULT          pbdaResult = 0;

    ASSERT( pIrp);
    ASSERT( pKSMethod);
    KSM_BDA_GPNV_SETVALUE *pNameValueBuffer = (KSM_BDA_GPNV_SETVALUE *) pKSMethod;
    // pulReturn needs to be verified because minData is zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pulReturn)
    {
        pIrp->IoStatus.Information = sizeof(ULONG);
        return STATUS_MORE_ENTRIES;
    }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::GetChangeState() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
        ulInputBufferSize = pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
    }

    if (ulInputBufferSize < sizeof(KSM_BDA_GPNV_SETVALUE))
    {
        DbgPrint("SetNameValue: Input size %d too small",ulInputBufferSize);
        return STATUS_UNSUCCESSFUL;
    }

    if (ulOutputBufferSize < sizeof(ULONG))
    {
        DbgPrint("SetNameValue: Output size %d too small",ulOutputBufferSize);
        pIrp->IoStatus.Information = sizeof(ULONG);
        return STATUS_BUFFER_OVERFLOW;
    }

    PKSFILTER pKsFilter = KsGetFilterFromIrp( pIrp );
    if( pKsFilter == NULL )
        return STATUS_INVALID_PARAMETER;

    WCHAR * cNameStr;
    WCHAR * cValueStr;

    if ((ulInputBufferSize - sizeof(KSM_BDA_GPNV_SETVALUE)+1) >= (pNameValueBuffer->ulValueLength + pNameValueBuffer->ulNameLength) + sizeof(WCHAR))
    {
        cNameStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, pNameValueBuffer->ulNameLength + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
            
        if (NULL == cNameStr)
        {
            return STATUS_UNSUCCESSFUL;
        }
        
        cValueStr = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, pNameValueBuffer->ulValueLength + sizeof(WCHAR), MS_SAMPLE_TUNER_POOL_TAG);
        
        if (NULL == cValueStr)
        {
            ExFreePoolWithTag(cNameStr, MS_SAMPLE_TUNER_POOL_TAG);
            return STATUS_UNSUCCESSFUL;
        }

        DbgPrint("pNameValueBuffer %ws", PWCHAR(pNameValueBuffer->argbName));
        DbgPrint("Name length=%d , Value length=%d", pNameValueBuffer->ulNameLength, pNameValueBuffer->ulValueLength);
        memcpy(cNameStr,pNameValueBuffer->argbName,pNameValueBuffer->ulNameLength);
        
        memcpy(cValueStr,pNameValueBuffer->argbName + pNameValueBuffer->ulNameLength,pNameValueBuffer->ulValueLength);
        cNameStr[pNameValueBuffer->ulNameLength/sizeof(WCHAR)] = 0;
        cValueStr[pNameValueBuffer->ulValueLength/sizeof(WCHAR)] = 0;
        DbgPrint("SetNameValue Name=%ws Value=%ws DialogRequest=%d Language=%s", cNameStr, cValueStr, pNameValueBuffer->ulDialogRequest, pNameValueBuffer->cLanguage);

        int i = 0;
        for (i = 0; i < GPNVS_NUMBER_OF_ROWS; i++)
        {
            if (wcscmp( cNameStr, pFilter->m_GpnvsTable[i][0]) == 0)
            {
                if (wcscmp( cNameStr, L"PBDA:/NV/Method/System Setup") == 0)
                {
                    //Calling System Setup
                    pFilter->m_ulCurrentDialogRequest = pNameValueBuffer->ulDialogRequest;
                    pFilter->SendOpenMMI(pNameValueBuffer->ulDialogRequest,GUID_MMI_MCML_HANDLER,"PBDA://2734DC82-774D-4daa-A5AD-2838B5A58127/Microsoft.com/Resource/Setup.mcml");
                    pbdaResult = MSDRI_S_MMI_PENDING;

                }
                else if (wcscmp( cNameStr, L"PBDA:/NV/Method/System Diagnostics") == 0)
                {
                    //Calling System Diagnostics

                    PKSDEVICE Device = KsFilterGetDevice(pKsFilter);
                    CDevice* pDevice = reinterpret_cast<CDevice *>(Device->Context);

                    if(pDevice)
                    {
                        ULONG ulMCMLFileSize=0;
                        UNICODE_STRING MCMLFileName;
                        RtlInitUnicodeString(&MCMLFileName,L"DiagnosticsPage.mcml");

                        NTSTATUS status = pDevice->ReadMCMLFromFile(NULL, &ulMCMLFileSize, MCMLFileName);

                        if (ulMCMLFileSize == 0 || !NT_SUCCESS(status))//file does  not exits
                        {
                            DbgPrint("CFilter::SetNameValue No File Use ulMCMLFileSize size=%d",ulMCMLFileSize);
                        }
                        else
                        {
                            DbgPrint("CFilter::SetNameValue File Use ulMCMLFileSize size=%d",ulMCMLFileSize);

                            WCHAR *cValueStrTmp = (WCHAR *)ExAllocatePoolWithTag(NonPagedPool, ulMCMLFileSize+sizeof(WCHAR) , MS_SAMPLE_TUNER_POOL_TAG);
                            cValueStrTmp[ulMCMLFileSize/sizeof(WCHAR)]=0;

                            status = pDevice->ReadMCMLFromFile((BYTE*)cValueStrTmp, &ulMCMLFileSize, MCMLFileName);

                            if(NT_SUCCESS(status))
                            {
                                pFilter->SetGPNVValue(cValueStrTmp, 28); 
                            }
                            else
                            {
                                ExFreePoolWithTag(cValueStrTmp, MS_SAMPLE_TUNER_POOL_TAG);
                            }
                        }
                    }

                    pFilter->m_ulCurrentDialogRequest = pNameValueBuffer->ulDialogRequest;
                    pFilter->SendOpenMMI(pNameValueBuffer->ulDialogRequest,GUID_MMI_MCML_HANDLER,"PBDA://2734DC82-774D-4daa-A5AD-2838B5A58127/Microsoft.com/Resource/Diagnostics.mcml");
                    pbdaResult = MSDRI_S_MMI_PENDING;
                }
                else
                {
                    pFilter->SetGPNVValue(cValueStr, i); 
                    cValueStr = NULL;
                }
                break;
            }
        }                

        if (cValueStr) 
        {
            ExFreePoolWithTag(cValueStr, MS_SAMPLE_TUNER_POOL_TAG);
        }

        ExFreePoolWithTag(cNameStr, MS_SAMPLE_TUNER_POOL_TAG);
    }
    else
    {
         DbgPrint("SetNameValue: Name/Value length error\n");
    }

    *pulReturn = (ULONG) pbdaResult;  //return the PBDARESULT

    pIrp->IoStatus.Information = sizeof(PBDARESULT);

    return Status;
}

/*
** PBDA GetNameFromIndex() method of the CFilter class
**
**    Return the Name of a Index.
**
*/
NTSTATUS
CFilter::GetNameFromIndex(
                      IN PIRP         pIrp,
                      IN PKSMETHOD    pKSMethod,
                      OUT BDA_STRING  *pData
                      )
{
    NTSTATUS        Status = STATUS_SUCCESS;
    CFilter *       pFilter;
    KSM_BDA_GPNV_NAMEINDEX *pNameIndexBuffer = (KSM_BDA_GPNV_NAMEINDEX *)pKSMethod;
    ULONG           ulOutputBufferSize = 0;
    ULONG           ulDataValueBufferSize = 1; //min size of the Name Buffer
    PIO_STACK_LOCATION pIrpStack  = NULL;


    _DbgPrintF(DEBUGLVL_VERBOSE,("CFilter::GetNameFromIndex"));

    ASSERT(pIrp);
    ASSERT(pKSMethod);


    // pData needs to be verified because minData might be zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pData)
    {
        pIrp->IoStatus.Information = sizeof(BDA_STRING);
        return STATUS_MORE_ENTRIES;
    }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::GetChangeState() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
    }
    else
    {
        return STATUS_UNSUCCESSFUL;
    }

    ULONG ulStoredValueBufferSize = ulDataValueBufferSize;

    if (pNameIndexBuffer->ulValueNameIndex < GPNVS_NUMBER_OF_ROWS)
    {
        ulStoredValueBufferSize = (wcslen(pFilter->m_GpnvsTable[pNameIndexBuffer->ulValueNameIndex][0]) + 1) *sizeof(WCHAR);
    }

    if (ulStoredValueBufferSize > ulDataValueBufferSize) //internal used Names are bigger than test code length 
    {
       ulDataValueBufferSize = ulStoredValueBufferSize; 
    }

    if (ulOutputBufferSize < (sizeof(BDA_STRING) + ulDataValueBufferSize -1))
    {
        DbgPrint("CFilter::GetNameFromIndex Buffer too small");
        pIrp->IoStatus.Information = sizeof(BDA_STRING) - 1 + ulDataValueBufferSize; 
        Status = STATUS_BUFFER_OVERFLOW;
    }
    else
    {

        if ( pData->ulStringSize >= ulDataValueBufferSize )     //compare against the required buffer
        {
            //only if the buffer is big enough we copy the value of the name
            pData->argbString[0] = 0;
            pData->lResult = 0;
            if (pNameIndexBuffer->ulValueNameIndex < GPNVS_NUMBER_OF_ROWS_VISIBLE)//do not return Resource Names
            {
                RtlStringCbCopy((WCHAR *)pData->argbString,ulStoredValueBufferSize,pFilter->m_GpnvsTable[pNameIndexBuffer->ulValueNameIndex][0]);
            }
            else
            {
                DbgPrint("CFilter::GetNameFromIndex out of range");
                    pData->lResult = BDA_E_OUT_OF_BOUNDS;
            }
            pData->ulStringSize = ulDataValueBufferSize;
            pIrp->IoStatus.Information = sizeof(BDA_STRING) + ulDataValueBufferSize -1;//add and return the correct length
            DbgPrint("CFilter::GetNameFromIndex Index=%d Name=%ws succeded",pNameIndexBuffer->ulValueNameIndex,(WCHAR *)pData->argbString);
        }
        else 
        {
            pIrp->IoStatus.Information = sizeof(BDA_STRING) + ulDataValueBufferSize -1;
            //the buffer is not big enough, return the target size
            Status = STATUS_BUFFER_OVERFLOW;
            DbgPrint("CFilter::GetNameFromIndex Buffer not big enough");
        }                     
    }

    return Status;
}

/*
** PBDA GetValueUpdateName() method of the CFilter class
**
**    Return the Value of a Name.
**
*/
NTSTATUS
CFilter::GetValueUpdateName(
                      IN PIRP         pIrp,
                      IN PKSMETHOD    pKSMethod,
                      OUT BDA_STRING  *pData
                      )
{
    NTSTATUS        Status = STATUS_SUCCESS;
    CFilter *       pFilter;
    PBDATUNER_EVENT pTunerEvent = NULL;
    ULONG           ulOutputBufferSize = 0;
    ULONG           ulDataValueBufferSize = 0; //min size of the NameValue Buffer
    PIO_STACK_LOCATION pIrpStack  = NULL;
    KIRQL Irql;

    UNREFERENCED_PARAMETER(pKSMethod);

    _DbgPrintF(DEBUGLVL_VERBOSE,("CFilter::GetValueUpdateName"));

    ASSERT(pIrp);
    ASSERT(pKSMethod);


    // pData needs to be verified because minData might be zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pData)
    {
        pIrp->IoStatus.Information = sizeof(BDA_STRING);
        return STATUS_MORE_ENTRIES;
    }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::GetChangeState() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
    }
    else
    {
        return STATUS_UNSUCCESSFUL;
    }

    //check if Name/Value Updates are in the EventQueue and complete them
    KeAcquireSpinLock( &pFilter->spinLock, &Irql);

    Status = pFilter->m_FilterGPNVEventsManager.ReadCurrentEvent(&pTunerEvent);
    if (Status == STATUS_SUCCESS)
    {
        ulDataValueBufferSize = pTunerEvent->ulDataLength;
    }
    else
    {
        pIrp->IoStatus.Information = sizeof(BDA_STRING);
        goto exit;
    }


    if (ulOutputBufferSize < (sizeof(BDA_STRING) + ulDataValueBufferSize -1))
    {
        DbgPrint("CFilter::GetValueUpdateName Buffer too small");
        pIrp->IoStatus.Information = sizeof(BDA_STRING) - 1 + ulDataValueBufferSize; 
        Status = STATUS_BUFFER_OVERFLOW;
    }
    else
    {

        if ( pData->ulStringSize >= ulDataValueBufferSize )     //compare against the required buffer
        {
            //only if the buffer is big enough we copy the value of the name
            pData->argbString[0] = 0;

            pData->lResult = 0; //return the right PBDARESULT
            memcpy(pData->argbString, pTunerEvent->argbData, ulDataValueBufferSize);

            pData->ulStringSize = ulDataValueBufferSize;
            pIrp->IoStatus.Information = sizeof(BDA_STRING) + ulDataValueBufferSize -1;//add and return the correct length
            DbgPrint("CFilter::GetValueUpdateName Name=%ws succeded",(WCHAR *)pData->argbString);

            //Complete the Event to remove the Name/Value Entry from the GPNV Event Queue 
            pFilter->m_FilterGPNVEventsManager.CompleteEvent(pTunerEvent, INVALID_TUNER_EVENT_ID, S_OK);
            
        }
        else 
        {
            pIrp->IoStatus.Information = sizeof(BDA_STRING) + ulDataValueBufferSize -1;
            //the buffer is not big enough, return the target size
            Status = STATUS_BUFFER_OVERFLOW;
            DbgPrint("CFilter::GetValueUpdateName Buffer not big enough");
        }                     
    }

exit:
    KeReleaseSpinLock( &pFilter->spinLock, Irql);
    return Status;
}

/*
** PBDA ISDBCAS_SetRequest() method of the CFilter class
**
**    Set the ISDB CAS Request and return a PBDAResult.
**
*/
NTSTATUS
CFilter::PBDA_ISDBCAS_SetRequest(
                      IN PIRP         pIrp,
                      IN PKSMETHOD    pKSMethod,
                      OUT PULONG      pulReturn
                      )
{
    NTSTATUS            Status = STATUS_SUCCESS;
    CFilter *           pFilter = NULL;
    ULONG               ulInputBufferSize = 0;
    ULONG               ulOutputBufferSize = 0;
    PIO_STACK_LOCATION  pIrpStack  = NULL;

    ASSERT( pIrp);
    ASSERT( pKSMethod);
    KSM_BDA_ISDBCAS_REQUEST *pISDBCasBuffer = (KSM_BDA_ISDBCAS_REQUEST *) pKSMethod;
    // pulReturn needs to be verified because minData is zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pulReturn)
    {
        pIrp->IoStatus.Information = sizeof(ULONG);
        return STATUS_MORE_ENTRIES;
    }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::GetChangeState() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
        ulInputBufferSize = pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
    }

    if (ulInputBufferSize < sizeof(KSM_BDA_ISDBCAS_REQUEST))
    {
        DbgPrint("ISDBCAS_SetRequest: Input size %d too small",ulInputBufferSize);
        return STATUS_UNSUCCESSFUL;
    }

    if (ulOutputBufferSize < sizeof(ULONG))
    {
        DbgPrint("ISDBCAS_SetRequest: Output size %d too small",ulOutputBufferSize);
        pIrp->IoStatus.Information = sizeof(ULONG);
        return STATUS_BUFFER_OVERFLOW;
    }

    if ((ulInputBufferSize - sizeof(KSM_BDA_ISDBCAS_REQUEST)+1) >= pISDBCasBuffer->ulIsdbCommandSize)
    {
        
        DbgPrint("ISDBCAS_SetRequest: RequestId=%d CommandLength=%d\n",pISDBCasBuffer->ulRequestID,pISDBCasBuffer->ulIsdbCommandSize);
        //pISDBCasBuffer->argbIsdbCommandSize holds the Command (Instruction(8),Reserved(24),Datalength(32),Data())
    }
    else
    {
        DbgPrint("ISDBCAS_SetRequest: length error\n");
    }

    *pulReturn = 0;  //return the PBDARESULT

    pIrp->IoStatus.Information = sizeof(PBDARESULT);

    return Status;
}

/*
** PBDA ISDBCAS_GetResponseData() method of the CFilter class
**
**    Return the response data based on a previous ISDB CAS Event.
**
*/
NTSTATUS
CFilter::PBDA_ISDBCAS_GetResponseData(
                            IN PIRP         pIrp,
                            IN PKSMETHOD    pKSMethod,
                            OUT BDA_ISDBCAS_RESPONSEDATA *pResponseBuffer
                            )
{
    NTSTATUS        Status = STATUS_SUCCESS;
    CFilter *       pFilter;
    ULONG           ulOutputBufferSize = 0;
    ULONG           ulResponseBufferSize = 10; //min size of the Response Buffer
    PIO_STACK_LOCATION pIrpStack  = NULL;

    UNREFERENCED_PARAMETER(pKSMethod);

    _DbgPrintF(DEBUGLVL_VERBOSE,("CFilter::ISDBCAS_GetResponseData"));

    ASSERT(pIrp);
    ASSERT(pKSMethod);


    // pData needs to be verified because minData might be zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pResponseBuffer)
    {
        pIrp->IoStatus.Information = sizeof(BDA_ISDBCAS_RESPONSEDATA);
        return STATUS_MORE_ENTRIES;
    }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::GetChangeState() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
    }
    else
    {
        return STATUS_UNSUCCESSFUL;
    }
    if (ulOutputBufferSize < (sizeof(BDA_ISDBCAS_RESPONSEDATA) + ulResponseBufferSize -1))
    {
        DbgPrint("CFilter::ISDB CAS Response Buffer too small");
        pIrp->IoStatus.Information = sizeof(BDA_ISDBCAS_RESPONSEDATA) - 1 + ulResponseBufferSize; 
        Status = STATUS_BUFFER_OVERFLOW;
    }
    else
    {

        if ( pResponseBuffer->ulIsdbDataSize >= ulResponseBufferSize )     //compare against the required buffer
        {
            //only if the buffer is big enough we copy the response buffer
            pResponseBuffer->ulRequestID = 0x2;  //set the right RequestID from the Event
            pResponseBuffer->ulIsdbStatus = 0x9000; //set the right Status
            pResponseBuffer->argbIsdbCommandData[0] = 0x99;//fill the command buffer
            pResponseBuffer->argbIsdbCommandData[1] = 0x00;//fill the command buffer
            //.....
            pResponseBuffer->ulIsdbDataSize = ulResponseBufferSize;
            pIrp->IoStatus.Information = sizeof(BDA_ISDBCAS_RESPONSEDATA) + ulResponseBufferSize -1;//add and return the correct length
            DbgPrint("CFilter::ISDBCAS_GetResponseData Length=%d succeded", ulResponseBufferSize);
            pResponseBuffer->lResult = 0;//return the right PBDA Result;
        }
        else 
        {
            pIrp->IoStatus.Information = sizeof(BDA_ISDBCAS_RESPONSEDATA) + ulResponseBufferSize -1;
            //the buffer is not big enough, return the target size
            Status = STATUS_BUFFER_OVERFLOW;
            DbgPrint("CFilter::ISDBCAS_GetResponseData Buffer not big enough");
        }                     
    }

    return Status;
}

/*
** PBDA_CoreCAS_CheckEntitlementToken() method of the CFilter class
**
**    Return the DescrambleStatus.
**
*/
NTSTATUS
CFilter::PBDA_CoreCAS_CheckEntitlementToken(
                      IN PIRP         pIrp,
                      IN PKSMETHOD    pKSMethod,
                      OUT BDA_CAS_CHECK_ENTITLEMENTTOKEN *pDescrambleStatus
                      )
{
    NTSTATUS        Status = STATUS_SUCCESS;
    CFilter *       pFilter;
    
    KSM_BDA_CAS_ENTITLEMENTTOKEN *pCASEntitlementBuffer = (KSM_BDA_CAS_ENTITLEMENTTOKEN *)pKSMethod;
    ULONG               ulInputBufferSize;
    ULONG               ulOutputBufferSize;
    PIO_STACK_LOCATION  pIrpStack  = NULL;

    ASSERT( pIrp);
    ASSERT( pKSMethod);
    // pDescrambleStatus needs to be verified because minData might be zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pDescrambleStatus)
    {
        pIrp->IoStatus.Information = sizeof(BDA_CAS_CHECK_ENTITLEMENTTOKEN);
        return STATUS_MORE_ENTRIES;
    }   

    //  Obtain a "this" pointer to the filter object.
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
        ulInputBufferSize = pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
    }
    else
    {
        return STATUS_UNSUCCESSFUL;
    }


    if (ulInputBufferSize < sizeof(KSM_BDA_CAS_ENTITLEMENTTOKEN))
    {
        DbgPrint("PBDA_CoreCAS_CheckEntitlementToken: Input size %d too small",ulInputBufferSize);
        return STATUS_UNSUCCESSFUL;
    }

    if (ulOutputBufferSize < sizeof(BDA_CAS_CHECK_ENTITLEMENTTOKEN))
    {
        DbgPrint("PBDA_CoreCAS_CheckEntitlementToken: Output size %d too small",ulOutputBufferSize);
        pIrp->IoStatus.Information = sizeof(BDA_CAS_CHECK_ENTITLEMENTTOKEN);
        return STATUS_BUFFER_OVERFLOW;
    }
    DbgPrint("PBDA_CoreCAS_CheckEntitlementToken: DialogRequest = 0x%X ,Language = %s ,RequestType = 0x%X ,EntitlementTokenLen = %d\n", 
        pCASEntitlementBuffer->ulDialogRequest,
        pCASEntitlementBuffer->cLanguage,
        pCASEntitlementBuffer->ulRequestType,
        pCASEntitlementBuffer->ulEntitlementTokenLen);

    if ((pCASEntitlementBuffer->ulEntitlementTokenLen == 8) && (pCASEntitlementBuffer->ulDialogRequest))
    {
        pDescrambleStatus->lResult = MSDRI_S_MMI_PENDING;
        DbgPrint("PBDA_CoreCAS_CheckEntitlementToken: Token = 0x%X,0x%X,0x%X,0x%X,0x%X,0x%X...",pCASEntitlementBuffer->argbEntitlementToken[0],pCASEntitlementBuffer->argbEntitlementToken[1],pCASEntitlementBuffer->argbEntitlementToken[2],pCASEntitlementBuffer->argbEntitlementToken[3],pCASEntitlementBuffer->argbEntitlementToken[4],pCASEntitlementBuffer->argbEntitlementToken[5]);
        if ((pCASEntitlementBuffer->argbEntitlementToken[0] & 0xFF) <= 0x4F)
        {
            //this was a now/next
            pFilter->SendOpenMMI(pCASEntitlementBuffer->ulDialogRequest,GUID_MMI_MCML_HANDLER, "PBDA://2734DC82-774D-4daa-A5AD-2838B5A58127/Microsoft.com/Resource/testpage.mcml");
            
        }
        else
        {
            //this is a schedule
            pFilter->SendOpenMMI(pCASEntitlementBuffer->ulDialogRequest,GUID_MMI_SIMPLE_HTML_PANEL, "PBDA://2734DC82-774D-4daa-A5AD-2838B5A58127/Microsoft.com/Resource/testpage.html");
        }

        pDescrambleStatus->ulDescrambleStatus = 0;
    }
    else
    {
        pDescrambleStatus->ulDescrambleStatus = 0x5959C3C3;//return the correct value
    }


    pIrp->IoStatus.Information = sizeof(BDA_CAS_CHECK_ENTITLEMENTTOKEN);


    return Status;
}

/*
** PBDA_CoreCAS_SetCaptureToken method of the CFilter class
**
**    Set the CAS Capture Token and return a PBDAResult.
**
*/
NTSTATUS
CFilter::PBDA_CoreCAS_SetCaptureToken(
                      IN PIRP         pIrp,
                      IN PKSMETHOD    pKSMethod,
                      OUT PULONG      pulReturn
                      )
{
    NTSTATUS            Status = STATUS_SUCCESS;
    CFilter *           pFilter;
    ULONG               ulInputBufferSize = 0;
    ULONG               ulOutputBufferSize = 0;
    PIO_STACK_LOCATION  pIrpStack  = NULL;

    ASSERT( pIrp);
    ASSERT( pKSMethod);
    KSM_BDA_CAS_CAPTURETOKEN *pCasTokenBuffer = (KSM_BDA_CAS_CAPTURETOKEN *) pKSMethod;
    // pulReturn needs to be verified because minData is zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pulReturn)
    {
        pIrp->IoStatus.Information = sizeof(ULONG);
        return STATUS_MORE_ENTRIES;
    }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::GetChangeState() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
        ulInputBufferSize = pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
    }

    if (ulInputBufferSize < sizeof(KSM_BDA_CAS_CAPTURETOKEN))
    {
        DbgPrint("PBDA_CoreCAS_SetCaptureToken: Input size %d too small",ulInputBufferSize);
        return STATUS_UNSUCCESSFUL;
    }

    if (ulOutputBufferSize < sizeof(PBDARESULT))
    {
        DbgPrint("PBDA_CoreCAS_SetCaptureToken: Output size %d too small",ulOutputBufferSize);
        pIrp->IoStatus.Information = sizeof(PBDARESULT);
        return STATUS_BUFFER_OVERFLOW;
    }

    if ((ulInputBufferSize - sizeof(KSM_BDA_CAS_CAPTURETOKEN)+1) >= pCasTokenBuffer->ulTokenLength)
    {
        
        DbgPrint("PBDA_CoreCAS_SetCaptureToken: Length=%d\n",pCasTokenBuffer->ulTokenLength);
    }
    else
    {
        DbgPrint("PBDA_CoreCAS_SetCaptureToken: length error\n");
    }

    *pulReturn = 0;  //return the PBDARESULT


    pIrp->IoStatus.Information = sizeof(PBDARESULT);

    return Status;
}

/*
** PBDA_CoreCAS_OpenBroadcastMMI() method of the CFilter class
**
**    Open a Broadcast MMI and return a PBDAResult.
**
*/
NTSTATUS
CFilter::PBDA_CoreCAS_OpenBroadcastMMI(
    IN PIRP         pIrp,
    IN PKSMETHOD    pKSMethod,
    OUT PULONG      pulReturn
    )
{
    NTSTATUS            Status = STATUS_SUCCESS;
    CFilter *           pFilter;

    ULONG               ulInputBufferSize = 0;
    ULONG               ulOutputBufferSize = 0;
    PIO_STACK_LOCATION  pIrpStack  = NULL;
    KSM_BDA_CAS_OPENBROADCASTMMI *pMMIBuffer = (KSM_BDA_CAS_OPENBROADCASTMMI *)pKSMethod;

    ASSERT( pIrp);
    ASSERT( pKSMethod);
    // pSyncValueBuffer needs to be verified because minData is zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pulReturn)
    {
        pIrp->IoStatus.Information = sizeof(ULONG);
        return STATUS_MORE_ENTRIES;
    }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
        ulInputBufferSize = pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
    }

    if (ulInputBufferSize < sizeof(KSM_BDA_CAS_OPENBROADCASTMMI))
    {
        DbgPrint("PBDA_CoreCAS_OpenBroadcastMMI: Input size %d too small",ulInputBufferSize);
        return STATUS_UNSUCCESSFUL;
    }

    if (ulOutputBufferSize < sizeof(PBDARESULT))
    {
        DbgPrint("PBDA_CoreCAS_OpenBroadcastMMI: Output size %d too small",ulOutputBufferSize);
        pIrp->IoStatus.Information = sizeof(PBDARESULT);
        return STATUS_BUFFER_OVERFLOW;
    }
    DbgPrint("PBDA_CoreCAS_OpenBroadcastMMI: DialogRequest= 0x%X, EventId = 0x%X, Language = %s\n", pMMIBuffer->ulDialogRequest ,pMMIBuffer->ulEventId, pMMIBuffer->cLanguage);

    if (pFilter->m_fBroadcastMMIRequested)
    {
        pFilter->m_fBroadcastMMIRequested = FALSE;
        if (pFilter->m_uiBroadcastMMIType == 2)
        {
            pFilter->SendOpenMMI(pMMIBuffer->ulDialogRequest,GUID_MMI_MCML_HANDLER, "PBDA://2734DC82-774D-4daa-A5AD-2838B5A58127/Microsoft.com/Resource/testpage.mcml");
        }
        else if (pFilter->m_uiBroadcastMMIType == 1)
        {
            pFilter->SendOpenMMI(pMMIBuffer->ulDialogRequest,GUID_MMI_SIMPLE_HTML_FULLSCREEN, "PBDA://2734DC82-774D-4daa-A5AD-2838B5A58127/Microsoft.com/Resource/testpage.html");
        }
        else
        {
        pFilter->SendOpenMMI(pMMIBuffer->ulDialogRequest,GUID_MMI_SIMPLE_HTML_PANEL, "PBDA://2734DC82-774D-4daa-A5AD-2838B5A58127/Microsoft.com/Resource/testpage.html");
        }
        *pulReturn = MSDRI_S_MMI_PENDING;
    } 
    else
    {
        *pulReturn = 0;
    }


    pIrp->IoStatus.Information = sizeof(PBDARESULT);

    return Status;
}

/*
** PBDA_CoreCAS_CloseMMIDialog() method of the CFilter class
**
**    Return the DescrambleStatus.
**
*/
NTSTATUS
CFilter::PBDA_CoreCAS_CloseMMIDialog(
                      IN PIRP         pIrp,
                      IN PKSMETHOD    pKSMethod,
                      OUT BDA_CAS_CLOSE_MMIDIALOG *pMMIResult
                      )
{
    NTSTATUS        Status = STATUS_SUCCESS;
    CFilter *       pFilter;
    
    KSM_BDA_CAS_CLOSEMMIDIALOG *pCloseMMIDialog = (KSM_BDA_CAS_CLOSEMMIDIALOG *)pKSMethod;
    ULONG               ulInputBufferSize;
    ULONG               ulOutputBufferSize;
    PIO_STACK_LOCATION  pIrpStack  = NULL;

    ULONG               ulSessionResultLen = sizeof(ULONG);

    ASSERT( pIrp);
    ASSERT( pKSMethod);
    // pDescrambleStatus needs to be verified because minData might be zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pMMIResult)
    {
        pIrp->IoStatus.Information = sizeof(BDA_CAS_CLOSE_MMIDIALOG);
        return STATUS_MORE_ENTRIES;
    }   

    //  Obtain a "this" pointer to the filter object.
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
        ulInputBufferSize = pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
    }
    else
    {
        return STATUS_UNSUCCESSFUL;
    }


    if (ulInputBufferSize < sizeof(KSM_BDA_CAS_CLOSEMMIDIALOG))
    {
        DbgPrint("PBDA_CoreCAS_CloseMMIDialog: Input size %d too small",ulInputBufferSize);
        return STATUS_UNSUCCESSFUL;
    }

    if ((ulOutputBufferSize < sizeof(BDA_CAS_CLOSE_MMIDIALOG)) || (ulSessionResultLen > sizeof(ULONG)))
    {
        DbgPrint("PBDA_CoreCAS_CloseMMIDialog: Output size %d too small",ulOutputBufferSize);
        pIrp->IoStatus.Information = sizeof(BDA_CAS_CLOSE_MMIDIALOG);
        return STATUS_BUFFER_OVERFLOW;
    }
    DbgPrint("PBDA_CoreCAS_CloseMMIDialog: DialogRequest = 0x%X ,Language = %s ,DialogNumber = 0x%X ,MMIReason = %d\n", 
        pCloseMMIDialog->ulDialogRequest,
        pCloseMMIDialog->cLanguage,
        pCloseMMIDialog->ulDialogNumber,
        pCloseMMIDialog->ulReason);

    pMMIResult->lResult = 0;
    pMMIResult->SessionResult = 0;//return the correct SessionResult


    pIrp->IoStatus.Information = sizeof(BDA_CAS_CLOSE_MMIDIALOG);

    return Status;
}

 /*
 ** PBDA GuideDataType() method of the CFilter class
 **
 **    Return the Guide Data Type GUID.
 **
 */
 NTSTATUS
 CFilter::GuideDataType(
                         IN PIRP         pIrp,
                         IN PKSMETHOD    pKSMethod,
                         OUT BDA_GDDS_DATATYPE  *pDataType
                         )
{
    NTSTATUS        Status = STATUS_SUCCESS;
    CFilter *       pFilter;
    ULONG           ulOutputBufferSize = 0;
    ULONG           ulDataValueBufferSize = 0; //min size of the Value Buffer
    PIO_STACK_LOCATION pIrpStack  = NULL;

    UNREFERENCED_PARAMETER(pKSMethod);

    _DbgPrintF(DEBUGLVL_VERBOSE,("CFilter::GuideDataType"));

    ASSERT(pIrp);
    ASSERT(pKSMethod);


    // pData needs to be verified because minData might be zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pDataType)
    {
        pIrp->IoStatus.Information = sizeof(BDA_GDDS_DATATYPE);
        return STATUS_MORE_ENTRIES;
    }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::GetChangeState() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
    }
    else
    {
        return STATUS_UNSUCCESSFUL;
    }


    if (ulDataValueBufferSize > sizeof(GUID)) ulDataValueBufferSize = sizeof(GUID); 

    if (ulOutputBufferSize < sizeof(BDA_GDDS_DATATYPE))
    {
        DbgPrint("CFilter::GuideDataType Buffer too small");
        pIrp->IoStatus.Information = sizeof(BDA_GDDS_DATATYPE); 
        Status = STATUS_BUFFER_OVERFLOW;
    }
    else
    {
#if defined(NETOP_RECEIVER) || defined(PBDAT_RECEIVER) || defined(DMBTH_RECEIVER)   
        //pDataType->uuidDataType = DVB_TERRESTRIAL_TV_NETWORK_TYPE;
        pDataType->uuidDataType = pFilter->m_guideDataType;

#else
        pDataType->uuidDataType = TEST_GUID;
#endif
        pDataType->lResult = 0;

        pIrp->IoStatus.Information = sizeof(BDA_GDDS_DATATYPE);
        DbgPrint("CFilter::GuideDataType called");
    }

    return Status;
}

 /*
** PBDA GuideDataValue() method of the CFilter class
**
**    Return the Guide Data and Percentage of all Data.
**
*/
NTSTATUS
CFilter::GuideDataValue(
                            IN PIRP         pIrp,
                            IN PKSMETHOD    pKSMethod,
                            OUT BDA_GDDS_DATA *pGuideDataBuffer
                            )
{
    NTSTATUS        Status = STATUS_SUCCESS;
    CFilter *       pFilter;
    ULONG           ulOutputBufferSize = 0;
    ULONG           ulGuideBufferSize = sizeof(ULONG); //min size of the Guide Buffer and percentage
    PIO_STACK_LOCATION pIrpStack  = NULL;

    UNREFERENCED_PARAMETER(pKSMethod);

    _DbgPrintF(DEBUGLVL_VERBOSE,("CFilter::GuideDataValue"));

    ASSERT(pIrp);
    ASSERT(pKSMethod);


    // pGuideDataBuffer needs to be verified because minData might be zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pGuideDataBuffer)
    {
        pIrp->IoStatus.Information = sizeof(BDA_GDDS_DATA);
        return STATUS_MORE_ENTRIES;
    }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::GetChangeState() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
    }
    else
    {
        return STATUS_UNSUCCESSFUL;
    }

    PKSFILTER pKsFilter = KsGetFilterFromIrp( pIrp );
    if( !pKsFilter)
    {
        return STATUS_UNSUCCESSFUL;
    }

    PKSDEVICE Device = KsFilterGetDevice(pKsFilter);
    CDevice* pDevice = reinterpret_cast<CDevice *>(Device->Context);
    if (!pDevice)
    {
        return STATUS_UNSUCCESSFUL;
    }

    if ((ulGuideBufferSize == sizeof(ULONG)) || (ulGuideBufferSize == 0))//do we use a testbuffer?
    {
        ULONG ulPercentage;
        ulGuideBufferSize = 0;
        Status = pDevice->GetNextGuideSection(NULL,&ulGuideBufferSize, &ulPercentage);//check if guide data is available from the cache
        if (Status == STATUS_SUCCESS)
        {
            ulGuideBufferSize += sizeof(ULONG); //add the percentage
        }
        else
        {
            if ((ulGuideBufferSize == 0) && (ulPercentage == 100))
            {
                Status = BDA_E_NO_MORE_DATA;
            }
            ulGuideBufferSize = sizeof(ULONG); //minimum buffer
        }
    }
    else
    {
        Status = STATUS_BUFFER_OVERFLOW;//we use a testbuffer
    }

    if (ulOutputBufferSize < (sizeof(BDA_GDDS_DATA) + ulGuideBufferSize -1 - sizeof(ULONG)))
    {
        DbgPrint("CFilter::Guide Data Buffer too small");
        pIrp->IoStatus.Information = sizeof(BDA_GDDS_DATA) - 1 - sizeof(ULONG) + ulGuideBufferSize; 
        Status = STATUS_BUFFER_OVERFLOW;
    }
    else
    {
		if (pGuideDataBuffer->ulDataLength > (ulOutputBufferSize - sizeof(BDA_GDDS_DATA) +1))
		{
			DbgPrint("CFilter::Guide Data Buffer length reduced to buffer size");
			pGuideDataBuffer->ulDataLength = ulOutputBufferSize - sizeof(BDA_GDDS_DATA) +1;
		}
        if ( pGuideDataBuffer->ulDataLength >= (ulGuideBufferSize - sizeof(ULONG)))     //compare against the required buffer
        {
            //only if the buffer is big enough we copy the Guide data into the buffer
            if (Status == STATUS_SUCCESS)
            {
                ulGuideBufferSize -= sizeof(ULONG);
                Status = pDevice->GetNextGuideSection(pGuideDataBuffer->argbData,&ulGuideBufferSize, &pGuideDataBuffer->ulPercentageProgress);
                if (Status != STATUS_SUCCESS)
                {
                    ulGuideBufferSize = 1;
                    pGuideDataBuffer->ulPercentageProgress = pFilter->m_AvailableGuideDataPercentage;    
                }
                ulGuideBufferSize += sizeof(ULONG);
                if ((pFilter->m_AvailableGuideDataPercentage >= 100) && (pGuideDataBuffer->ulPercentageProgress >= 100))
                {
                    pGuideDataBuffer->lResult = BDA_E_NO_MORE_DATA;
                }
                else
                {
                    pGuideDataBuffer->lResult = 0;
                }
            }
            else if (Status == BDA_E_NO_MORE_DATA)
            {
                pGuideDataBuffer->ulPercentageProgress = pFilter->m_AvailableGuideDataPercentage = 100;
                pGuideDataBuffer->lResult = BDA_E_NO_MORE_DATA;
            }
            else
            {
                pFilter->m_AvailableGuideDataPercentage = 100; //complete the guide data or keep a percentage if not all data is available
                pGuideDataBuffer->ulPercentageProgress = pFilter->m_AvailableGuideDataPercentage;  //set the percentage
                pGuideDataBuffer->argbData[0] = 0x1;//fill the guide buffer based on PBDA-SI
                //.....


                if (pFilter->m_AvailableGuideDataPercentage == 100)
                {
                    pGuideDataBuffer->lResult = BDA_E_NOT_IMPLEMENTED;//return the right PBDA Result;
                }
                else
                {
                    pGuideDataBuffer->lResult = 0;
                }
            }
            Status = STATUS_SUCCESS;

            pGuideDataBuffer->ulDataLength = ulGuideBufferSize - sizeof(ULONG);
            pFilter->m_AvailableGuideDataPercentage = pGuideDataBuffer->ulPercentageProgress; //if test code use a different value, set it
            pIrp->IoStatus.Information = sizeof(BDA_GDDS_DATA) + ulGuideBufferSize -1 - sizeof(ULONG);//add and return the correct length
            DbgPrint("CFilter::GuideDataValue Length=%d Percentage=%d succeded", ulGuideBufferSize - (ULONG)sizeof(ULONG), pGuideDataBuffer->ulPercentageProgress);
        }
        else 
        {
            pIrp->IoStatus.Information = sizeof(BDA_GDDS_DATA) + ulGuideBufferSize -1 - sizeof(ULONG);
            //the buffer is not big enough, return the target size
            Status = STATUS_BUFFER_OVERFLOW;
            DbgPrint("CFilter::GuideDataValue Buffer not big enough");
        }                     
    }

    return Status;
}

 /*
 ** PBDA GetTuneXMLFromIDX() method of the CFilter class
 **
 **    Return the Value of a Name.
 **
 */
 NTSTATUS
 CFilter::GetTuneXMLFromIDX(
                         IN PIRP         pIrp,
                         IN PKSMETHOD    pKSMethod,
                         OUT BDA_STRING  *pTuneXML
                         )
{
    NTSTATUS        Status = STATUS_SUCCESS;
    CFilter *       pFilter;
    KSM_BDA_GDDS_TUNEXMLFROMIDX *pIDxBuffer = (KSM_BDA_GDDS_TUNEXMLFROMIDX *)pKSMethod;
    ULONG           ulOutputBufferSize = 0;
    ULONG           ulTuneXMLBufferSize = 0; //min size of the Value Buffer
    PIO_STACK_LOCATION pIrpStack  = NULL;

    _DbgPrintF(DEBUGLVL_VERBOSE,("CFilter::GetTuneXMLFromIDX"));

    ASSERT(pIrp);
    ASSERT(pKSMethod);


    // pData needs to be verified because minData might be zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pTuneXML)
    {
        pIrp->IoStatus.Information = sizeof(BDA_STRING);
        return STATUS_MORE_ENTRIES;
    }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::GetChangeState() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
    }
    else
    {
        return STATUS_UNSUCCESSFUL;
    }

    PKSFILTER pKsFilter = KsGetFilterFromIrp( pIrp );
    if( pKsFilter == NULL )
        return STATUS_INVALID_PARAMETER;
    PKSDEVICE Device = KsFilterGetDevice(pKsFilter);
    CDevice* pDevice = reinterpret_cast<CDevice *>(Device->Context);

    ULONG ulServiceFileSize = 0;
    NTSTATUS statusStr = STATUS_SUCCESS;
    char *pServicesStr = (char *)pFilter->m_pGuideServices;
    ULONG ulServicesLen = ulServiceFileSize;
    char IDXStr[] = "Idx value";
    ULONG64 ulTargetIDXValue64;

    char *pTuneStr = NULL;


    pDevice->ReadGuideServiceListFromFile(NULL, &ulServiceFileSize);
    if (ulServiceFileSize == 0) //file does  not exits
    {
        DbgPrint("CFilter::GetTuneXMLFromIDX no file found");
    }
    else
    {
        if (pFilter->m_pGuideServices != NULL)
        {
            ulServicesLen = ulServiceFileSize;
            while (statusStr == STATUS_SUCCESS)
            {
                statusStr = pFilter->FindAndConvertStringValue(pServicesStr, ulServicesLen, IDXStr, NULL, &ulTargetIDXValue64);
                if (statusStr == STATUS_SUCCESS)
                {
                    pServicesStr = (char *)pFilter->m_pGuideServices + (ulServiceFileSize - ulServicesLen);
                    if (ulTargetIDXValue64 == pIDxBuffer->ulIdx)
                    {
                        DbgPrint("CFilter::GetTuneXMLFromIDX Idx = %d found in file", (int) pIDxBuffer->ulIdx);
                        //read/detect the TuneXML Buffer and length from the Guide Services File at tag <tune:xxx ... </tune:xxx> 
                        char TuneStr[] = "<tune:";
                        pFilter->FindXMLString(pServicesStr, ulServicesLen, TuneStr, &pTuneStr, &ulTuneXMLBufferSize);
                        ulTuneXMLBufferSize++; //add the \0 to the full length
                        break;
                    }
                }
            }
        }
    }

    if (ulOutputBufferSize < (sizeof(BDA_STRING) + ulTuneXMLBufferSize -1))
    {
        DbgPrint("CFilter::GetTuneXMLFromIDX Buffer too small");
        pIrp->IoStatus.Information = sizeof(BDA_STRING) - 1 + ulTuneXMLBufferSize; 
        Status = STATUS_BUFFER_OVERFLOW;
    }
    else
    {

        if ( pTuneXML->ulStringSize >= ulTuneXMLBufferSize )     //compare against the required buffer
        {
            pTuneXML->argbString[0] = 0;
            
            DbgPrint("CFilter::GetTuneXMLFromIDX Idx = %d", (int) pIDxBuffer->ulIdx);
            pTuneXML->lResult = 0; //return the right PBDARESULT
            
            if (pIDxBuffer->ulIdx > 0xFFFFFFFFFFFF) //check the Idx value to be valid  
            {
                pTuneXML->lResult = BDA_E_OUT_OF_BOUNDS;
            }
            else
            {
                if (pTuneStr)
                {
                    memcpy(pTuneXML->argbString,pTuneStr,ulTuneXMLBufferSize);
                    pTuneXML->argbString[ulTuneXMLBufferSize-1]=0;
                }
            }

            pTuneXML->ulStringSize = ulTuneXMLBufferSize;
            pIrp->IoStatus.Information = sizeof(BDA_STRING) + ulTuneXMLBufferSize -1;//add and return the correct length
            
        }
        else 
        {
            pIrp->IoStatus.Information = sizeof(BDA_STRING) + ulTuneXMLBufferSize -1;
            //the buffer is not big enough, return the target size
            Status = STATUS_BUFFER_OVERFLOW;
            DbgPrint("CFilter::GetTuneXMLFromIDX Buffer not big enough");
        }                     
    }

    return Status;
}

/*
 ** PBDA GetServices() method of the CFilter class
 **
 **    Return the Value of a Name.
 **
 */
 NTSTATUS
 CFilter::GetServices(
                         IN PIRP         pIrp,
                         IN PKSMETHOD    pKSMethod,
                         OUT BDA_BUFFER  *pServicesData
                         )
{
    NTSTATUS        Status = STATUS_SUCCESS;
    CFilter *       pFilter;
    ULONG           ulOutputBufferSize = 0;
    ULONG           ulServicesBufferSize = 0; //min size of the Service List
    PIO_STACK_LOCATION pIrpStack  = NULL;

    UNREFERENCED_PARAMETER(pKSMethod);

    _DbgPrintF(DEBUGLVL_VERBOSE,("CFilter::GetServices"));

    ASSERT(pIrp);
    ASSERT(pKSMethod);


    // pServicesData needs to be verified because minData might be zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pServicesData)
    {
        pIrp->IoStatus.Information = sizeof(BDA_BUFFER);
        return STATUS_MORE_ENTRIES;
    }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::GetChangeState() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
    }
    else
    {
        return STATUS_UNSUCCESSFUL;
    }

    //read the Guide service list from file located in registry at GuideServicesFile under the PSWTuner\[devid] key
    PKSFILTER pKsFilter = KsGetFilterFromIrp( pIrp );
    if( pKsFilter == NULL )
        return STATUS_INVALID_PARAMETER;
    PKSDEVICE Device = KsFilterGetDevice(pKsFilter);
    CDevice* pDevice = reinterpret_cast<CDevice *>(Device->Context);

    ULONG ulServiceFileSize = 0;
    ULONG ulNrOfIDXValues = 0;
    NTSTATUS statusStr = STATUS_SUCCESS;
    char *pServicesStr = (char *)pFilter->m_pGuideServices;
    ULONG ulServicesLen = ulServiceFileSize;
    char IDXStr[] = "Idx value";
    ULONG64 ulTargetIDXValue64;


    pDevice->ReadGuideServiceListFromFile(NULL, &ulServiceFileSize);
    if (ulServiceFileSize == 0) //file does  not exits
    {
        DbgPrint("CFilter::GetServices No File Use Testbuffer size=%d",ulServicesBufferSize);
    }
    else
    {
        if (pFilter->m_pGuideServices == NULL)
        {
            Status = pDevice->ReadGuideServiceListFromFile(&pFilter->m_pGuideServices, &ulServiceFileSize);
        }
        if (pFilter->m_pGuideServices != NULL)
        {
            pServicesStr = (char *)pFilter->m_pGuideServices;
            ulServicesLen = ulServiceFileSize;
            while (statusStr == STATUS_SUCCESS)
            {
                statusStr = pFilter->FindAndConvertStringValue(pServicesStr, ulServicesLen, IDXStr, NULL, &ulTargetIDXValue64);
                if (statusStr == STATUS_SUCCESS)
                {
                    ulNrOfIDXValues++;
                    pServicesStr = (char *)pFilter->m_pGuideServices + (ulServiceFileSize - ulServicesLen);
                }
            }
            ulServicesBufferSize = ulNrOfIDXValues * sizeof(ULONG64);
            ulServicesBufferSize += sizeof(ULONG); //sizeof ulNrOfIDXValues
        }
    }

    if (ulOutputBufferSize < (sizeof(BDA_BUFFER) + ulServicesBufferSize -1))
    {
        DbgPrint("CFilter::GetServices Buffer too small");
        pIrp->IoStatus.Information = sizeof(BDA_BUFFER) - 1 + ulServicesBufferSize; 
        Status = STATUS_BUFFER_OVERFLOW;
    }
    else
    {

        if ( pServicesData->ulBufferSize >= ulServicesBufferSize )     //compare against the required buffer
        {
            pServicesData->argbBuffer[0] = 0;
            
            DbgPrint("CFilter::GetServices BufferSize = %d", pServicesData->ulBufferSize);
            
            pServicesData->lResult = 0; //return the right PBDARESULT

                statusStr = STATUS_SUCCESS;
                pServicesStr = (char *)pFilter->m_pGuideServices;
                ulServicesLen = ulServiceFileSize;
                ulNrOfIDXValues = 0;
                ULONG64 *pIDXBuffer = (ULONG64 *)(pServicesData->argbBuffer + sizeof(ULONG));
                //fill the buffer with all IDX values starting with ULONG32-NrOfIDXValues, then all ULONG64-IDXValues
                if (pFilter->m_pGuideServices != NULL)
                {
                    while (statusStr == STATUS_SUCCESS)
                    {
                        statusStr = pFilter->FindAndConvertStringValue(pServicesStr, ulServicesLen, IDXStr, NULL, &ulTargetIDXValue64);
                        if (statusStr == STATUS_SUCCESS)
                        {
                            pIDXBuffer[ulNrOfIDXValues] = ulTargetIDXValue64;
                            ulNrOfIDXValues++;
                            pServicesStr = (char *)pFilter->m_pGuideServices + (ulServiceFileSize - ulServicesLen);
                        }
                    }
                    *(ULONG *)pServicesData->argbBuffer = ulNrOfIDXValues;
                }

            pServicesData->ulBufferSize = ulServicesBufferSize;
            pIrp->IoStatus.Information = sizeof(BDA_BUFFER) + ulServicesBufferSize -1;//add and return the correct length
            
        }
        else 
        {
            pIrp->IoStatus.Information = sizeof(BDA_BUFFER) + ulServicesBufferSize -1;
            //the buffer is not big enough, return the target size
            Status = STATUS_BUFFER_OVERFLOW;
            DbgPrint("CFilter::GetServices Buffer not big enough");
        }                     
    }

    return Status;
}

  /*
 ** PBDA GetServiceFromTuneXML() method of the CFilter class
 **
 **    Return the Service for a TuneXML.
 **
 */
 NTSTATUS
 CFilter::GetServiceFromTuneXML(
                         IN PIRP         pIrp,
                         IN PKSMETHOD    pKSMethod,
                         OUT BDA_STRING  *pServiceData
                         )
{
    NTSTATUS        Status = STATUS_SUCCESS;
    CFilter *       pFilter;
    KSM_BDA_GDDS_SERVICEFROMTUNEXML *pTuneXMLBuffer = (KSM_BDA_GDDS_SERVICEFROMTUNEXML *)pKSMethod;
    ULONG           ulOutputBufferSize = 0;
    ULONG           ulServiceBufferSize = 0; //min size of the Service Buffer
    PIO_STACK_LOCATION pIrpStack  = NULL;

    _DbgPrintF(DEBUGLVL_VERBOSE,("CFilter::GetServiceFromTuneXML"));

    ASSERT(pIrp);
    ASSERT(pKSMethod);


    // pServiceData needs to be verified because minData might be zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pServiceData)
    {
        pIrp->IoStatus.Information = sizeof(BDA_STRING);
        return STATUS_MORE_ENTRIES;
    }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::GetChangeState() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
    }
    else
    {
        return STATUS_UNSUCCESSFUL;
    }

    PKSFILTER pKsFilter = KsGetFilterFromIrp( pIrp );
    if( pKsFilter == NULL )
        return STATUS_INVALID_PARAMETER;
    PKSDEVICE Device = KsFilterGetDevice(pKsFilter);
    CDevice* pDevice = reinterpret_cast<CDevice *>(Device->Context);

    ULONG ulServiceFileSize = 0;
    NTSTATUS statusStr = STATUS_SUCCESS;
    char *pServicesStr = (char *)pFilter->m_pGuideServices;
    ULONG ulServicesLen = ulServiceFileSize;

    char *pServiceInfoStr = NULL;

    pDevice->ReadGuideServiceListFromFile(NULL, &ulServiceFileSize);
        if (pFilter->m_pGuideServices != NULL)
        {
            while (statusStr == STATUS_SUCCESS)
            {
                LONG lTargetValue=0;
                ulServicesLen = ulServiceFileSize;
                statusStr = pFilter->FindAndConvertStringValue(pServicesStr, ulServicesLen, (char *)pTuneXMLBuffer->argbTuneXml, &lTargetValue);
                if (statusStr == STATUS_SUCCESS)
                {
                    pServicesStr = (char *)pFilter->m_pGuideServices + (ulServiceFileSize - ulServicesLen);
                    DbgPrint("CFilter::GetServiceTuneXML found TuneXML in file");
                    char ServiceInfoStr[] = "<ps:Service";
                    pFilter->FindXMLString(pServicesStr, ulServicesLen, ServiceInfoStr, &pServiceInfoStr, &ulServiceBufferSize);
                    ulServiceBufferSize++;//add the \0 to the length
                    break;
                }
            }
        }

    if (ulOutputBufferSize < (sizeof(BDA_STRING) + ulServiceBufferSize -1))
    {
        DbgPrint("CFilter::GetServiceFromTuneXML Buffer too small");
        pIrp->IoStatus.Information = sizeof(BDA_STRING) - 1 + ulServiceBufferSize; 
        Status = STATUS_BUFFER_OVERFLOW;
    }
    else
    {

        if ( pServiceData->ulStringSize >= ulServiceBufferSize )     //compare against the required buffer
        {
            pServiceData->argbString[0] = 0;
            
            DbgPrint("CFilter::GetServiceFromTuneXML TuneXML = %s", (char *)pTuneXMLBuffer->argbTuneXml);
            //RtlStringCbCopyA((char *)pServiceData->argbString,ulServiceBufferSize, pFilter->m_ServiceFromTuneXML);
            pServiceData->lResult = 0; //return the right PBDARESULT

                if (pServiceInfoStr)
                {
                    memcpy(pServiceData->argbString,pServiceInfoStr,ulServiceBufferSize);
                    pServiceData->argbString[ulServiceBufferSize-1] = 0;
                }

            pServiceData->ulStringSize = ulServiceBufferSize;
            pIrp->IoStatus.Information = sizeof(BDA_STRING) + ulServiceBufferSize -1;//add and return the correct length
            
        }
        else 
        {
            pIrp->IoStatus.Information = sizeof(BDA_STRING) + ulServiceBufferSize -1;
            //the buffer is not big enough, return the target size
            Status = STATUS_BUFFER_OVERFLOW;
            DbgPrint("CFilter::GetServiceFromTuneXML Buffer not big enough");
        }                     
    }

    return Status;
}

 /*
 ** PBDA GetGuideDataUpdate() method of the CFilter class
 **
 **    Start the Update of the Guide Data.
 **
 */
 NTSTATUS
 CFilter::GetGuideDataUpdate(
                         IN PIRP         pIrp,
                         IN PKSMETHOD    pKSMethod,
                         OUT PULONG      pulReturn
                         )
{
    NTSTATUS        Status = STATUS_SUCCESS;
    CFilter *       pFilter;
    ULONG           ulOutputBufferSize = 0;
    PIO_STACK_LOCATION pIrpStack  = NULL;

    UNREFERENCED_PARAMETER(pKSMethod);

    DbgPrint("CFilter::GetGuideDataUpdate requested");

    ASSERT(pIrp);
    ASSERT(pKSMethod);


    // pulReturn needs to be verified because minData might be zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pulReturn)
    {
        pIrp->IoStatus.Information = sizeof(PBDARESULT);
        return STATUS_MORE_ENTRIES;
    }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::GetChangeState() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
    }
    else
    {
        return STATUS_UNSUCCESSFUL;
    }

    if (ulOutputBufferSize < sizeof(PBDARESULT))
    {
        DbgPrint("CFilter::GetGuideDataUpdate Buffer too small");
        pIrp->IoStatus.Information = sizeof(PBDARESULT); 
        Status = STATUS_BUFFER_OVERFLOW;
    }
    else
    {

        *pulReturn = 0; //return the right PBDARESULT

        //start new internal request of the guide data
        pFilter->m_AvailableGuideDataPercentage = 0;
        //test send an event that new data is available
        if ((pFilter->m_PendingEventEntry == NULL)||(pFilter->m_FilterEventsManager.IsEnabled() == FALSE))
        {
            DbgPrint("CFilter::GetGuideDataUpdate Eventing not enabled");
        }
        else
        {
            KIRQL               Irql;
            KeAcquireSpinLock( &pFilter->spinLock, &Irql);
            PBDATUNER_EVENT pTunerEvent = AllocateTunerEvent(&EVENTID_BDA_GuideServiceInformationUpdated, 0,NULL);
            if (pTunerEvent == NULL)
            {
                DbgPrint("CFilter GetGuideDataUpdate could not allocate an event");
            }
            else
            {
                pFilter->m_FilterEventsManager.QueueEvent(pTunerEvent,TRUE);
                DbgPrint("CFilter create Event EVENTID_BDA_GuideServiceInformationUpdated");
            }

            pTunerEvent = AllocateTunerEvent(&EVENTID_BDA_GuideDataAvailable, 0,NULL);
            if (pTunerEvent == NULL)
            {
                DbgPrint("CFilter::GetGuideDataUpdate could not allocate an event");
            }
            else
            {
                pFilter->m_FilterEventsManager.QueueEvent(pTunerEvent,TRUE);
                DbgPrint("CFilter::GetGuideDataUpdate create Event EVENTID_BDA_GuideDataAvailable");
            }
            KeReleaseSpinLock( &pFilter->spinLock, Irql);
        }
            
        pIrp->IoStatus.Information = sizeof(PBDARESULT);
            
    }

    return Status;
}

/*
 ** PBDA_Tuning_SetTuner() method of the CFilter class
 **
 **    Set the xml tuning information for the PBDA Tuner.
 **
 */
 NTSTATUS
 CFilter::PBDA_Tuning_SetTuner(
    IN PIRP         pIrp,
    IN PKSMETHOD    pKSMethod,
    OUT PULONG      pulReturn
    )
{
    NTSTATUS            Status = STATUS_SUCCESS;
    CFilter *           pFilter;
    ULONG               ulInputBufferSize = 0;
    ULONG               ulOutputBufferSize = 0;
    PIO_STACK_LOCATION  pIrpStack  = NULL;

    ASSERT( pIrp);
    ASSERT( pKSMethod);
    KSM_BDA_TUNER_TUNEREQUEST *pTuneRequestBuffer = (KSM_BDA_TUNER_TUNEREQUEST *) pKSMethod;
    // pulReturn needs to be verified because minData is zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pulReturn)
    {
        pIrp->IoStatus.Information = sizeof(ULONG);
        return STATUS_MORE_ENTRIES;
    }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::GetChangeState() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
        ulInputBufferSize = pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
    }

    if (ulInputBufferSize < sizeof(KSM_BDA_TUNER_TUNEREQUEST))
    {
        DbgPrint("PBDA_Tuning_SetTuner: Input size %d too small",ulInputBufferSize);
        return STATUS_UNSUCCESSFUL;
    }

    if (ulOutputBufferSize < sizeof(ULONG))
    {
        DbgPrint("PBDA_Tuning_SetTuner: Output size %d too small",ulOutputBufferSize);
        pIrp->IoStatus.Information = sizeof(ULONG);
        return STATUS_BUFFER_OVERFLOW;
    }

    if ((ulInputBufferSize - sizeof(KSM_BDA_TUNER_TUNEREQUEST)+1) >= pTuneRequestBuffer->ulTuneLength)
    {
        
        DbgPrint("PBDA_Tuning_SetTuner: TuneLength=%d\n",pTuneRequestBuffer->ulTuneLength);
        //pTuneRequestBuffer->argbTuneData contain the tunerequest
        ULONG ulXMLTuneLen = pTuneRequestBuffer->ulTuneLength;
        char *pXMLTuneStr = (char *)pTuneRequestBuffer->argbTuneData;
        char FreqStr[] = "Frequency";
        LONG lTargetFrequency = 0;
        LONG lPolarization = 0;
        LONG lLNBSource = 0;
        pFilter->FindAndConvertStringValue(pXMLTuneStr,ulXMLTuneLen,FreqStr,&lTargetFrequency);
        if (lTargetFrequency <= 0)
        {
            pXMLTuneStr = (char *)pTuneRequestBuffer->argbTuneData + (pTuneRequestBuffer->ulTuneLength - ulXMLTuneLen);
            pFilter->FindAndConvertStringValue(pXMLTuneStr,ulXMLTuneLen,FreqStr,&lTargetFrequency);
        }
        if (lTargetFrequency <= 0)//do this twice because the xml file may have a default frequency block as -1
        {
            pXMLTuneStr = (char *)pTuneRequestBuffer->argbTuneData + (pTuneRequestBuffer->ulTuneLength - ulXMLTuneLen);
            pFilter->FindAndConvertStringValue(pXMLTuneStr,ulXMLTuneLen,FreqStr,&lTargetFrequency);
        }
        if (lTargetFrequency < 0) lTargetFrequency = 0;
        
        if (lTargetFrequency == 0)
        {
            //check for ChannelId TuneRequests
            pXMLTuneStr = (char *)pTuneRequestBuffer->argbTuneData;
            ulXMLTuneLen = pTuneRequestBuffer->ulTuneLength;
            char ChannelIDStr[] = " ChannelID";
            pFilter->FindAndConvertStringValue(pXMLTuneStr,ulXMLTuneLen,ChannelIDStr,&lTargetFrequency);
        }
        else
        { //check also other parameters, e.g. for DVB-S Polarisation and LNB Source
            pXMLTuneStr = (char *)pTuneRequestBuffer->argbTuneData;
            ulXMLTuneLen = pTuneRequestBuffer->ulTuneLength;
            char PolarisationStr[] = " Polarisation";
            pFilter->FindAndConvertStringValue(pXMLTuneStr,ulXMLTuneLen,PolarisationStr,&lPolarization);

            pXMLTuneStr = (char *)pTuneRequestBuffer->argbTuneData;
            ulXMLTuneLen = pTuneRequestBuffer->ulTuneLength;
            char LNBSourceStr[] = " DiseqLNBSource";
            pFilter->FindAndConvertStringValue(pXMLTuneStr,ulXMLTuneLen,LNBSourceStr,&lLNBSource);
        }
        
        //update the internal Parameters
        Status = BdaStartChanges( pIrp);
        if (NT_SUCCESS( Status))
        {
            DbgPrint("PBDA_Tuning_SetTuner: New Frequency=%d\n",lTargetFrequency);
            pFilter->m_NewResource.ulCarrierFrequency = (ULONG) lTargetFrequency;
            pFilter->m_NewResource.ulFrequencyMultiplier = 1000;
            pFilter->m_NewResource.ulPolarity = lPolarization;
            pFilter->m_NewResource.ulDiseqC = lLNBSource;
            if (lLNBSource)
            {
                pFilter->m_NewResource.fDiseqCEnable = TRUE;
            }
            else
            {
                pFilter->m_NewResource.fDiseqCEnable = FALSE;
            }
            pFilter->m_BdaChangeState = BDA_CHANGES_COMPLETE;
        }
                
        Status = pFilter->CommitChanges(pIrp,pKSMethod,NULL);

        //search the tunerequest for a string frequency and tune to this transport stream file
        //if the frequency exist and lock to the signal send a EVENTID_BDA_TunerSignalLock Event
        //this typically takes time so a real tuner will have a delay and should not block this call until 
        //the locked state entered
    }
    else
    {
        DbgPrint("PBDA_Tuning_SetTuner: length error\n");
    }

    *pulReturn = 0;  //return the PBDARESULT

    pIrp->IoStatus.Information = sizeof(PBDARESULT);

    return Status;

}

/*
 ** PBDA_Tuning_GetTunerState() method of the CFilter class
 **
 **    Get the current xml tuning information from the PBDA Tuner.
 **
 */
 NTSTATUS
 CFilter::PBDA_Tuning_GetTunerState(
    IN PIRP         pIrp,
    IN PKSMETHOD    pKSMethod,
    OUT BDA_TUNER_TUNERSTATE      *pTunerState
    )
 {
    NTSTATUS        Status = STATUS_SUCCESS;
    CFilter *       pFilter;
    ULONG           ulOutputBufferSize = 0;
    PIO_STACK_LOCATION pIrpStack  = NULL;
    ULONG           ulTunerStateBufferSize = 1; 
    char *          pcTuneXMLStr = NULL;

    UNREFERENCED_PARAMETER(pKSMethod);

    _DbgPrintF(DEBUGLVL_VERBOSE,("CFilter::PBDA_Tuning_GetTunerState"));

    ASSERT(pIrp);
    ASSERT(pKSMethod);


    // pServicesData needs to be verified because minData might be zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pTunerState)
    {
        pIrp->IoStatus.Information = sizeof(BDA_TUNER_TUNERSTATE);
        return STATUS_MORE_ENTRIES;
    }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::GetChangeState() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
    }
    else
    {
        return STATUS_UNSUCCESSFUL;
    }


    if (ulTunerStateBufferSize <= 1)
    {
        //determine the length of the TuneXML string
        //big enough for all possible strings
        size_t allocSize = sizeof("<tune:DVB OriginalNetworkID=\"65535\" TransportStreamID=\"65535\" ServiceID=\"65535\"><tune:TuningSpace xsi:type=\"tune:DVBTuningSpaceType\" Name=\"DVB-T\" NetworkType=\"{216C62DF-6D7F-4e9a-8571-05F14EDB766A}\" SystemType=\"Terrestrial\" NetworkID=\"65535\" HighOscillatorFrequency=\"10600000\" LowOscillatorFrequency=\"9750000\" LNBSwitchFrequency=\"11700000\"/><tune:Locator xsi:type=\"tune:DVBTLocatorType\" Frequency=\"12600000\" ModulationType=\"Uninitialized\" InnerFECMethod=\"RS_204_188\" InnerFECRate=\"2_3\" OuterFECMethod=\"Uninitialized\" OuterFECRate=\"Uninitialized\" SymbolRate=\"-1\" Bandwidth=\"-1\" LPInnerFECMethod=\"Uninitialized\" LPInnerFECRate=\"Uninitialized\" HierarchyAlpha=\"Uninitialized\" GuardInterval=\"Uninitialized\" TransmissionMode=\"Uninitialized\" OtherFrequencyInUse=\"false\"/></tune:DVB>");

        pcTuneXMLStr = (char *)ExAllocatePoolWithTag(NonPagedPool, allocSize, MS_SAMPLE_TUNER_POOL_TAG);

        // generate the XML TuneRequest with appropriate tuning parameters
        if (wcscmp( L"FA4B375A-45B4-4D45-8440-263957B11623", pFilter->m_GpnvsTable[11][1]) == 0) //compare the Tuner Types
        {//DVB-S
            char PolarityH[] = "Linear_H";
            char PolarityV[] = "Linear_V";
            char *cpolarity = PolarityH;
            if (pFilter->m_CurResource.ulPolarity == 2)
            {
                cpolarity = PolarityV; 
            }
            RtlStringCbPrintfA(pcTuneXMLStr, allocSize, 
                                "<tune:DVB OriginalNetworkID=\"-1\" TransportStreamID=\"-1\" ServiceID=\"-1\"><tune:TuningSpace xsi:type=\"tune:DVBSTuningSpaceType\" Name=\"DVB-S\" NetworkType=\"{FA4B375A-45B4-4D45-8440-263957B11623}\" SystemType=\"Satellite\" NetworkID=\"-1\" HighOscillatorFrequency=\"10600000\" LowOscillatorFrequency=\"9750000\" LNBSwitchFrequency=\"11700000\"/><tune:Locator xsi:type=\"tune:DVBSLocatorType\" Frequency=\"%d\" ModulationType=\"QPSK\" InnerFECMethod=\"RS_204_188\" InnerFECRate=\"2_3\" OuterFECMethod=\"Uninitialized\" SymbolRate=\"27500000\" Polarisation=\"%s\" DiseqLNBSource=\"SourceA\"/></tune:DVB>",
                                pFilter->m_CurResource.ulCarrierFrequency, cpolarity
                                );

        }
        else if (wcscmp( L"DC0C0FE7-0485-4266-B93F-68FBF80ED834", pFilter->m_GpnvsTable[11][1]) == 0) //compare the Tuner Types
        {//DVB-C or QAM
            RtlStringCbPrintfA(pcTuneXMLStr, allocSize, 
                                "<tune:DVB OriginalNetworkID=\"-1\" TransportStreamID=\"-1\" ServiceID=\"-1\"><tune:TuningSpace xsi:type=\"tune:DVBTuningSpaceType\" Name=\"DVB-C\" NetworkType=\"{DC0C0FE7-0485-4266-B93F-68FBF80ED834}\" SystemType=\"Cable\" NetworkID=\"-1\"/><tune:Locator xsi:type=\"tune:DVBTLocatorType\" Frequency=\"%d\" ModulationType=\"64QAM\" InnerFECMethod=\"RS_204_188\" InnerFECRate=\"2_3\" OuterFECRate=\"Uninitialized\" Bandwidth=\"-1\" LPInnerFECMethod=\"Uninitialized\" LPInnerFECRate=\"Uninitialized\" SymbolRate=\"6900000\"/></tune:DVB>",
                                pFilter->m_CurResource.ulCarrierFrequency
                                );

        }
        else if (wcscmp( L"0DAD2FDD-5FD7-11D3-8F50-00C04F7971E2", pFilter->m_GpnvsTable[11][1]) == 0) //compare the Tuner Types
        {//ATSC
        RtlStringCbPrintfA(pcTuneXMLStr, allocSize, 
                                "<tune:ATSC ><tune:TuningSpace xsi:type=\"tune:ATSCTuningSpaceType\" Name=\"ATSC\" NetworkType=\"{0DAD2FDD-5FD7-11D3-8F50-00C04F7971E2}\" SystemType=\"Terrestrial\" NetworkID=\"-1\"/><tune:Locator xsi:type=\"tune:ATSCLocatorType\" Frequency=\"%d\" /></tune:ATSC>",
                            pFilter->m_CurResource.ulCarrierFrequency
                            );

        }
        else
        {//DVB-T
            RtlStringCbPrintfA(pcTuneXMLStr, allocSize, 
                                "<tune:DVB OriginalNetworkID=\"-1\" TransportStreamID=\"-1\" ServiceID=\"-1\"><tune:TuningSpace xsi:type=\"tune:DVBTuningSpaceType\" Name=\"DVB-T\" NetworkType=\"{216C62DF-6D7F-4e9a-8571-05F14EDB766A}\" SystemType=\"Terrestrial\" NetworkID=\"-1\"/><tune:Locator xsi:type=\"tune:DVBTLocatorType\" Frequency=\"%d\" ModulationType=\"Uninitialized\" InnerFECMethod=\"RS_204_188\" InnerFECRate=\"2_3\" OuterFECMethod=\"Uninitialized\" SymbolRate=\"-1\" Bandwidth=\"-1\" LPInnerFECMethod=\"Uninitialized\" LPInnerFECRate=\"Uninitialized\" HierarchyAlpha=\"Uninitialized\" GuardInterval=\"Uninitialized\" TransmissionMode=\"Uninitialized\" OtherFrequencyInUse=\"false\"/></tune:DVB>",
                                pFilter->m_CurResource.ulCarrierFrequency
                                );
        }

        ulTunerStateBufferSize = strlen(pcTuneXMLStr) + sizeof(char);

    }

    if (ulOutputBufferSize < (sizeof(BDA_TUNER_TUNERSTATE) + ulTunerStateBufferSize -1))
    {
        DbgPrint("CFilter::PBDA_Tuning_GetTunerState Buffer too small");
        pIrp->IoStatus.Information = sizeof(BDA_TUNER_TUNERSTATE) - 1 + ulTunerStateBufferSize; 
        Status = STATUS_BUFFER_OVERFLOW;
    }
    else
    {

        if ( pTunerState->ulTuneLength >= ulTunerStateBufferSize )     //compare against the required buffer
        {
            pTunerState->argbTuneData[0] = 0;
            
            DbgPrint("CFilter::PBDA_Tuning_GetTunerState BufferSize = %d", pTunerState->ulTuneLength);
            RtlStringCbCopyA((char *)pTunerState->argbTuneData,ulTunerStateBufferSize, pcTuneXMLStr);
                         
            pTunerState->lResult = 0; //return the right PBDARESULT


            pTunerState->ulTuneLength = ulTunerStateBufferSize;
            pIrp->IoStatus.Information = sizeof(BDA_TUNER_TUNERSTATE) + ulTunerStateBufferSize -1;//add and return the correct length
            
        }
        else 
        {
            pIrp->IoStatus.Information = sizeof(BDA_TUNER_TUNERSTATE) + ulTunerStateBufferSize -1;
            //the buffer is not big enough, return the target size
            Status = STATUS_BUFFER_OVERFLOW;
            DbgPrint("CFilter::PBDA_Tuning_GetTunerState Buffer not big enough");
        }                     
    }

    if (pcTuneXMLStr)
    {
        ExFreePoolWithTag(pcTuneXMLStr, MS_SAMPLE_TUNER_POOL_TAG);        
    }
    return Status;

 }

/*
 ** PBDA_Tuning_GetSignalNoiseRatio() method of the CFilter class
 **
 **    Get the current signal level (dbmV, %, C/N) from the PBDA Tuner.
 **
 */
 NTSTATUS
 CFilter::PBDA_Tuning_GetSignalNoiseRatio(
    IN PIRP         pIrp,
    IN PKSMETHOD    pKSMethod,
    OUT BDA_TUNER_DIAGNOSTICS      *pTunerDiagnostic
    )
 {
    NTSTATUS        Status = STATUS_SUCCESS;
    CFilter *       pFilter;
    ULONG           ulOutputBufferSize = 0;
    ULONG           ulDataBufferSize = 0; //min size of the Buffer
    PIO_STACK_LOCATION pIrpStack  = NULL;

    UNREFERENCED_PARAMETER(pKSMethod);

    _DbgPrintF(DEBUGLVL_VERBOSE,("CFilter::PBDA_Tuning_GetSignalNoiseRatio"));

    ASSERT(pIrp);
    ASSERT(pKSMethod);


    // pData needs to be verified because minData might be zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pTunerDiagnostic)
    {
        pIrp->IoStatus.Information = sizeof(BDA_TUNER_DIAGNOSTICS);
        return STATUS_MORE_ENTRIES;
    }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::GetChangeState() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
    }
    else
    {
        return STATUS_UNSUCCESSFUL;
    }

	if (ulDataBufferSize > sizeof(BDA_TUNER_DIAGNOSTICS)) ulDataBufferSize = sizeof(BDA_TUNER_DIAGNOSTICS); 

    if (ulOutputBufferSize < sizeof(BDA_TUNER_DIAGNOSTICS))
    {
        DbgPrint("CFilter::PBDA_Tuning_GetSignalNoiseRatio Buffer too small");
        pIrp->IoStatus.Information = sizeof(BDA_TUNER_DIAGNOSTICS); 
        Status = STATUS_BUFFER_OVERFLOW;
    }
    else
    {
        pTunerDiagnostic->ulSignalLevel = 0xFF00;
        pTunerDiagnostic->ulSignalLevelQuality = 95;
        pTunerDiagnostic->ulSignalNoiseRatio = 60;
        pTunerDiagnostic->lResult = 0;

        pIrp->IoStatus.Information = sizeof(BDA_TUNER_DIAGNOSTICS);
        DbgPrint("CFilter::PBDA_Tuning_GetSignalNoiseRatio call succeeded with Signal=0x%X, Quality=%d, C/N=%d", 
            pTunerDiagnostic->ulSignalLevel, pTunerDiagnostic->ulSignalLevelQuality, pTunerDiagnostic->ulSignalNoiseRatio);
    }

    return Status;

 }

/*
 ** PBDA_Scanning_GetCapabilities() method of the CFilter class
 **
 **    Get the scanning capabilities from the PBDA Tuner.
 **
 */
 NTSTATUS
 CFilter::PBDA_Scanning_GetCapabilities(
    IN PIRP         pIrp,
    IN PKSMETHOD    pKSMethod,
    OUT BDA_SCAN_CAPABILTIES      *pScanCapabilities
    )
 {
    NTSTATUS        Status = STATUS_SUCCESS;
    CFilter *       pFilter;
    ULONG           ulOutputBufferSize = 0;
    ULONG           ulDataBufferSize = 0; //min size of the Buffer
    PIO_STACK_LOCATION pIrpStack  = NULL;

    UNREFERENCED_PARAMETER(pKSMethod);

    _DbgPrintF(DEBUGLVL_VERBOSE,("CFilter::PBDA_Scanning_GetCapabilities"));

    ASSERT(pIrp);
    ASSERT(pKSMethod);


    // pData needs to be verified because minData might be zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pScanCapabilities)
    {
        pIrp->IoStatus.Information = sizeof(BDA_SCAN_CAPABILTIES);
        return STATUS_MORE_ENTRIES;
    }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::GetChangeState() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
    }
    else
    {
        return STATUS_UNSUCCESSFUL;
    }

    if (ulDataBufferSize > sizeof(UINT64)) ulDataBufferSize = sizeof(UINT64); 

    if (ulOutputBufferSize < sizeof(BDA_SCAN_CAPABILTIES))
    {
        DbgPrint("CFilter::PBDA_Scanning_GetCapabilities Buffer too small");
        pIrp->IoStatus.Information = sizeof(BDA_SCAN_CAPABILTIES); 
        Status = STATUS_BUFFER_OVERFLOW;
    }
    else
    {
        pScanCapabilities->lResult = 0;

        pIrp->IoStatus.Information = sizeof(BDA_SCAN_CAPABILTIES);
        DbgPrint("CFilter::PBDA_Scanning_GetCapabilities call succeeded with AnalogStandardsSupported=0x%X (ulong)", 
            (ULONG)pScanCapabilities->ul64AnalogStandardsSupported);
    }

    return Status;
 }

/*
 ** PBDA_Scanning_GetScanningState() method of the CFilter class
 **
 **    Get the scanning state (lock and frequency) from the PBDA Tuner.
 **
 */
 NTSTATUS
 CFilter::PBDA_Scanning_GetScanningState(
    IN PIRP         pIrp,
    IN PKSMETHOD    pKSMethod,
    OUT BDA_SCAN_STATE      *pScanState
    )
 {
    NTSTATUS        Status = STATUS_SUCCESS;
    CFilter *       pFilter;
    ULONG           ulOutputBufferSize = 0;
    ULONG           ulDataBufferSize = 0; //min size of the Buffer
    PIO_STACK_LOCATION pIrpStack  = NULL;

    UNREFERENCED_PARAMETER(pKSMethod);

    _DbgPrintF(DEBUGLVL_VERBOSE,("CFilter::PBDA_Tuning_GetSignalNoiseRatio"));

    ASSERT(pIrp);
    ASSERT(pKSMethod);


    // pData needs to be verified because minData might be zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pScanState)
    {
        pIrp->IoStatus.Information = sizeof(BDA_SCAN_STATE);
        return STATUS_MORE_ENTRIES;
    }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::GetChangeState() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
    }
    else
    {
        return STATUS_UNSUCCESSFUL;
    }

	if (ulDataBufferSize > sizeof(BDA_SCAN_STATE)) ulDataBufferSize = sizeof(BDA_SCAN_STATE); 

    if (ulOutputBufferSize < sizeof(BDA_SCAN_STATE))
    {
        DbgPrint("CFilter::PBDA_Scanning_GetScanningState Buffer too small");
        pIrp->IoStatus.Information = sizeof(BDA_SCAN_STATE); 
        Status = STATUS_BUFFER_OVERFLOW;
    }
    else
    {
        BDATUNER_DEVICE_STATUS      DeviceStatus;
        Status = pFilter->GetStatus( &DeviceStatus);

        pScanState->ulSignalLock = DeviceStatus.fSignalLocked ? 0xFF : 0;//return the current lock state 
        pScanState->ulSecondsLeft = pFilter->m_ScanEstimateTime;//estimated time to the end of scanning 
        pScanState->ulCurrentFrequency = pFilter->m_CurResource.ulCarrierFrequency;//use the current scan frequency
        pScanState->lResult = 0;
        if (pFilter->m_guideDataType == PBDA_GUIDE_DATATYPE)
        {
            pScanState->lResult = BDA_E_NOT_IMPLEMENTED; //if GDDS is in use disable any scanning capability
        }

        pIrp->IoStatus.Information = sizeof(BDA_SCAN_STATE);
        DbgPrint("CFilter::PBDA_Scanning_GetScanningState call succeeded with SignalLock=0x%X, SecondsLeft=%d, CurrentFreq=%d", 
            pScanState->ulSignalLock, pScanState->ulSecondsLeft, pScanState->ulCurrentFrequency);
#if defined(NETOP_RECEIVER) || defined(PBDAT_RECEIVER)
        //send ServiceInformationUpdate so that the list of services are read via GetServices
        
        if ((pFilter->m_PendingEventEntry == NULL)||(pFilter->m_FilterEventsManager.IsEnabled() == FALSE))
        {
            DbgPrint("CFilter Eventing not enabled");
        }
        else
        {
            KIRQL               Irql;
            if ((pScanState->ulSignalLock) && (pFilter->m_ScanEstimateTime > 2) && (pFilter->m_guideDataType != PBDA_GUIDE_DATATYPE) )
            {
                KeAcquireSpinLock( &pFilter->spinLock, &Irql);
                PBDATUNER_EVENT pTunerEvent = AllocateTunerEvent(&EVENTID_BDA_GuideServiceInformationUpdated, 0,NULL);
                if (pTunerEvent == NULL)
                {
                    DbgPrint("CFilter GetScanState could not allocate an event");
                }
                else
                {
                    pFilter->m_FilterEventsManager.QueueEvent(pTunerEvent,TRUE);
                    DbgPrint("CFilter create Event EVENTID_BDA_GuideServiceInformationUpdated");
                }
                KeReleaseSpinLock( &pFilter->spinLock, Irql);
            }
            else
            {
                if (pFilter->m_ScanEstimateTime > 0)
                {
                        pFilter->m_ScanEstimateTime--;
                }
            }
        }

#endif
    }

    return Status;

 }

/*
 ** PBDA_Scanning_SetScanFilter() method of the CFilter class
 **
 **    Get the scanning filter for the PBDA Tuner.
 **
 */
 NTSTATUS
 CFilter::PBDA_Scanning_SetScanFilter(
    IN PIRP         pIrp,
    IN PKSMETHOD    pKSMethod,
    OUT PULONG      pulReturn
    )
 {
    NTSTATUS            Status = STATUS_SUCCESS;
    CFilter *           pFilter;
    ULONG               ulInputBufferSize = 0;
    ULONG               ulOutputBufferSize = 0;
    PIO_STACK_LOCATION  pIrpStack  = NULL;

    ASSERT( pIrp);
    ASSERT( pKSMethod);
    KSM_BDA_SCAN_FILTER *pScanFilterBuffer = (KSM_BDA_SCAN_FILTER *) pKSMethod;
    // pulReturn needs to be verified because minData is zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pulReturn)
    {
        pIrp->IoStatus.Information = sizeof(ULONG);
        return STATUS_MORE_ENTRIES;
    }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::GetChangeState() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
        ulInputBufferSize = pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
    }

    if (ulInputBufferSize < sizeof(KSM_BDA_SCAN_FILTER))
    {
        DbgPrint("PBDA_Scanning_SetScanFilter: Input size %d too small",ulInputBufferSize);
        return STATUS_UNSUCCESSFUL;
    }

    if (ulOutputBufferSize < sizeof(ULONG))
    {
        DbgPrint("PBDA_Scanning_SetScanFilter: Output size %d too small",ulOutputBufferSize);
        pIrp->IoStatus.Information = sizeof(ULONG);
        return STATUS_BUFFER_OVERFLOW;
    }

    if ((ulInputBufferSize - sizeof(KSM_BDA_SCAN_FILTER)+1) >= pScanFilterBuffer->ulScanModulationTypeSize)
    {
        
        DbgPrint("PBDA_Scanning_SetScanFilter: ScanModulationTypes Length=%d AnalogVideoStandards=0x%X\n",pScanFilterBuffer->ulScanModulationTypeSize, (ULONG)pScanFilterBuffer->AnalogVideoStandards);
       //pScanFilterBuffer->argbScanModulationTypes contain the scanmodulation types
       //a real tuner should limit scanning based on the transfered modultions (transfered as string, e.g.: "64-QAM,256-QAM")
    }
    else
    {
        DbgPrint("PBDA_Scanning_SetScanFilter: length error\n");
    }

    *pulReturn = 0;  //return the PBDARESULT

    pIrp->IoStatus.Information = sizeof(PBDARESULT);

    return Status;
 }

/*
 ** PBDA_Scanning_ScanStart() method of the CFilter class
 **
 **    Start the scanning for a frequency range at the PBDA Tuner.
 **
 */
 NTSTATUS
 CFilter::PBDA_Scanning_ScanStart(
    IN PIRP         pIrp,
    IN PKSMETHOD    pKSMethod,
    OUT PULONG      pulReturn
    )
 {
    NTSTATUS            Status = STATUS_SUCCESS;
    CFilter *           pFilter;
    ULONG               ulInputBufferSize = 0;
    ULONG               ulOutputBufferSize = 0;
    PIO_STACK_LOCATION  pIrpStack  = NULL;

    ASSERT( pIrp);
    ASSERT( pKSMethod);
    KSM_BDA_SCAN_START *pScanStartBuffer = (KSM_BDA_SCAN_START *) pKSMethod;
    // pulReturn needs to be verified because minData is zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pulReturn)
    {
        pIrp->IoStatus.Information = sizeof(ULONG);
        return STATUS_MORE_ENTRIES;
    }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::GetChangeState() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
        ulInputBufferSize = pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
    }

    if (ulInputBufferSize < sizeof(KSM_BDA_SCAN_START))
    {
        DbgPrint("PBDA_Scanning_ScanStart: Input size %d too small",ulInputBufferSize);
        return STATUS_UNSUCCESSFUL;
    }

    if (ulOutputBufferSize < sizeof(ULONG))
    {
        DbgPrint("PBDA_Scanning_ScanStart: Output size %d too small",ulOutputBufferSize);
        pIrp->IoStatus.Information = sizeof(ULONG);
        return STATUS_BUFFER_OVERFLOW;
    }

    DbgPrint("PBDA_Scanning_ScanStart: Scan Lowerfrequency=%d Higherfrequency=%d\n",pScanStartBuffer->LowerFrequency, pScanStartBuffer->HigherFrequency);
   //we start scanning from the LowerFrequency and scan to the next available and send a ScanUpdateEvent
    PKSFILTER pKsFilter = KsGetFilterFromIrp( pIrp );
    if( pKsFilter == NULL )
        return STATUS_INVALID_PARAMETER;

    PKSDEVICE Device = KsFilterGetDevice(pKsFilter);
    CDevice* pDevice = reinterpret_cast<CDevice *>(Device->Context);

    pFilter->m_CurrentScanningFrequency = pScanStartBuffer->LowerFrequency;

    Status = pDevice->GetNextAvailableFrequency(&pFilter->m_CurrentScanningFrequency);//STATUS_UNSUCCESSFUL if not found , STATUS_SUCCESS if found

    if (Status == STATUS_UNSUCCESSFUL)
    {
        pFilter->m_ScanEstimateTime = 120;//test-start from the beginning
    }
    else
    {
        pFilter->m_ScanEstimateTime = 100;//test-start from somewhere in the middle
    }
    
    //update the internal Parameters
    Status = BdaStartChanges( pIrp);
    if (NT_SUCCESS( Status))
    {
        DbgPrint("PBDA_Scanning_ScanStart: Next Available Frequency=%d\n",pFilter->m_CurrentScanningFrequency);
        pFilter->m_NewResource.ulCarrierFrequency = pFilter->m_CurrentScanningFrequency;
        pFilter->m_NewResource.ulFrequencyMultiplier = 1000;
        pFilter->m_BdaChangeState = BDA_CHANGES_COMPLETE;
    }            
    Status = pFilter->CommitChanges(pIrp,pKSMethod,NULL);

    KIRQL               Irql;
    KeAcquireSpinLock( &pFilter->spinLock, &Irql);
    
    if ((pFilter->m_PendingEventEntry == NULL)||(pFilter->m_FilterEventsManager.IsEnabled() == FALSE))
    {
        DbgPrint("CFilter::Scanning ScanStart Eventing not enabled");
    }
    else
    {
        PBDATUNER_EVENT pTunerEvent = AllocateTunerEvent(&EVENTID_BDA_UpdateScanState, 0,NULL);
        if (pTunerEvent == NULL)
        {
            DbgPrint("CFilter::Scanning ScanStart could not allocate an event");
        }
        else
        {
            pFilter->m_FilterEventsManager.QueueEvent(pTunerEvent,TRUE);
            DbgPrint("CFilter::Scanning ScanStart create Event EVENTID_BDA_UpdateScanState");
        }
    }
    KeReleaseSpinLock( &pFilter->spinLock, Irql);

    *pulReturn = MSDRI_S_PENDING;  //return MSDRI_S_PENDING as PBDARESULT if successful

    pIrp->IoStatus.Information = sizeof(PBDARESULT);

    return Status;
 }

/*
 ** PBDA_Scanning_ScanResume() method of the CFilter class
 **
 **    Resume the scanning that was previously locked by the PBDA Tuner.
 **
 */
 NTSTATUS
 CFilter::PBDA_Scanning_ScanResume(
    IN PIRP         pIrp,
    IN PKSMETHOD    pKSMethod,
    OUT PULONG      pulReturn
    )
 {
    NTSTATUS            Status = STATUS_SUCCESS;
    CFilter *           pFilter;
    ULONG               ulInputBufferSize = 0;
    ULONG               ulOutputBufferSize = 0;
    PIO_STACK_LOCATION  pIrpStack  = NULL;
    UNREFERENCED_PARAMETER(pKSMethod);

    ASSERT( pIrp);
    ASSERT( pKSMethod);
    // pulReturn needs to be verified because minData is zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pulReturn)
    {
        pIrp->IoStatus.Information = sizeof(ULONG);
        return STATUS_MORE_ENTRIES;
    }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::GetChangeState() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
        ulInputBufferSize = pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
    }

    if (ulOutputBufferSize < sizeof(ULONG))
    {
        DbgPrint("PBDA_Scanning_ScanResume: Output size %d too small",ulOutputBufferSize);
        pIrp->IoStatus.Information = sizeof(ULONG);
        return STATUS_BUFFER_OVERFLOW;
    }

    PKSFILTER pKsFilter = KsGetFilterFromIrp( pIrp );
    if( pKsFilter == NULL )
        return STATUS_INVALID_PARAMETER;

    PKSDEVICE Device = KsFilterGetDevice(pKsFilter);
    CDevice* pDevice = reinterpret_cast<CDevice *>(Device->Context);
    ULONG ulCurrentFrequency = pFilter->m_CurrentScanningFrequency;
    Status = pDevice->GetNextAvailableFrequency(&ulCurrentFrequency);//STATUS_UNSUCCESSFUL if not found , STATUS_SUCCESS if found
    
    if (Status != STATUS_SUCCESS) //end of the table reached
    {
        DbgPrint("PBDA_Scanning_ScanResume: End of scan\n");
        if (pFilter->m_ScanEstimateTime > 2)
        {
            pFilter->m_ScanEstimateTime = 2;// we delay by 2 cycles
        }
        *pulReturn = 0;  //return 0 to signal the end of the scan
    }
    else
    {
        pFilter->m_CurrentScanningFrequency = ulCurrentFrequency;
        DbgPrint("PBDA_Scanning_ScanResume: scan moved to %d\n", pFilter->m_CurrentScanningFrequency);
        if (pFilter->m_ScanEstimateTime > 3)
        {
            pFilter->m_ScanEstimateTime--;
        }
        *pulReturn = MSDRI_S_PENDING;  //return MSDRI_S_PENDING as PBDARESULT if successful
    }

    //update the internal Parameters
    Status = BdaStartChanges( pIrp);
    if (NT_SUCCESS( Status))
    {
        pFilter->m_NewResource.ulCarrierFrequency = pFilter->m_CurrentScanningFrequency;
        pFilter->m_NewResource.ulFrequencyMultiplier = 1000;
        pFilter->m_BdaChangeState = BDA_CHANGES_COMPLETE;
    }            
    Status = pFilter->CommitChanges(pIrp,pKSMethod,NULL);
   //we resume scanning from the current Frequency and scan to the next available and send a ScanUpdateEvent
    KIRQL               Irql;
    KeAcquireSpinLock( &pFilter->spinLock, &Irql);
    
    if ((pFilter->m_PendingEventEntry == NULL)||(pFilter->m_FilterEventsManager.IsEnabled() == FALSE))
    {
        DbgPrint("CFilter::Scanning ScanStart Eventing not enabled");
    }
    else
    {
        if (pFilter->m_ScanEstimateTime > 2)
        {
        PBDATUNER_EVENT pTunerEvent = AllocateTunerEvent(&EVENTID_BDA_UpdateScanState, 0,NULL);
        if (pTunerEvent == NULL)
        {
                DbgPrint("CFilter::Scanning ScanResume could not allocate an event");
        }
        else
        {
            pFilter->m_FilterEventsManager.QueueEvent(pTunerEvent,TRUE);
                DbgPrint("CFilter::Scanning ScanResume create Event EVENTID_BDA_UpdateScanState");
            }
        }
    }
    KeReleaseSpinLock( &pFilter->spinLock, Irql);

    pIrp->IoStatus.Information = sizeof(PBDARESULT);

    return Status;

 }

/*
 ** PBDA_Scanning_ScanStop() method of the CFilter class
 **
 **    Stop the scanning of the PBDA Tuner.
 **
 */
 NTSTATUS
 CFilter::PBDA_Scanning_ScanStop(
    IN PIRP         pIrp,
    IN PKSMETHOD    pKSMethod,
    OUT PULONG      pulReturn
    )
 {
    NTSTATUS            Status = STATUS_SUCCESS;
    CFilter *           pFilter;
    ULONG               ulInputBufferSize = 0;
    ULONG               ulOutputBufferSize = 0;
    PIO_STACK_LOCATION  pIrpStack  = NULL;
    UNREFERENCED_PARAMETER(pKSMethod);

    ASSERT( pIrp);
    ASSERT( pKSMethod);
    // pulReturn needs to be verified because minData is zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pulReturn)
    {
        pIrp->IoStatus.Information = sizeof(ULONG);
        return STATUS_MORE_ENTRIES;
    }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::GetChangeState() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
        ulInputBufferSize = pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
    }

    if (ulOutputBufferSize < sizeof(ULONG))
    {
        DbgPrint("PBDA_Scanning_ScanStop: Output size %d too small",ulOutputBufferSize);
        pIrp->IoStatus.Information = sizeof(ULONG);
        return STATUS_BUFFER_OVERFLOW;
    }

    pFilter->m_ScanEstimateTime = 0;
    DbgPrint("PBDA_Scanning_ScanStop: \n");
   //we resume scanning from the current Frequency and scan to the next available and send a ScanUpdateEvent

    *pulReturn = 0;  //return the PBDARESULT

    pIrp->IoStatus.Information = sizeof(PBDARESULT);

    return Status;

 }

#if defined(ISDBS_RECEIVER) || defined(DVBT_RECEIVER) || defined(DMBTH_RECEIVER)    
/*
 ** BDA_TsSelectorSetTSID() method of the CFilter class
 **
 **    Set a TSID value to be tuned of the PBDA Tuner.
 **
 */
NTSTATUS
CFilter::BDA_TsSelectorSetTSID(
    IN PIRP         pIrp,
    IN PKSMETHOD    pKSMethod,
    OUT PULONG      pulReturn
    )
{
    NTSTATUS            Status = STATUS_SUCCESS;
    ULONG               ulInputBufferSize = 0UL;
    ULONG               ulOutputBufferSize = 0UL;
    PIO_STACK_LOCATION  pIrpStack  = NULL;
    KSM_BDA_TS_SELECTOR_SETTSID * pTSInfo = (KSM_BDA_TS_SELECTOR_SETTSID *)pKSMethod;

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
        ulInputBufferSize = pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
    }

    if (ulInputBufferSize < sizeof(KSM_BDA_TS_SELECTOR_SETTSID))
    {
        DbgPrint("BDA_TsSelectorSetTSID: Input buffer size %d too small", ulInputBufferSize);
        return STATUS_UNSUCCESSFUL;
    }

    if (ulOutputBufferSize < sizeof(ULONG))
    {
        DbgPrint("BDA_TsSelectorSetTSID: Output buffer size %d too small", ulOutputBufferSize);
        pIrp->IoStatus.Information = sizeof(ULONG);
        return STATUS_BUFFER_OVERFLOW;
    }

    DbgPrint("BDA_TsSelectorSetTSID with TSID(0x%X). \n", pTSInfo->usTSID);

    *pulReturn = 0;  // result

    pIrp->IoStatus.Information = sizeof(ULONG);

    return Status;
}

/*
 ** BDA_TsSelectorGetTSInformation() method of the CFilter class
 **
 **    Get TS info of the current tuned stream of the PBDA Tuner.
 **
 */
NTSTATUS
CFilter::BDA_TsSelectorGetTSInformation(
    IN PIRP                     pIrp,
    IN PKSMETHOD                pKSMethod,
    OUT PBDA_TS_SELECTORINFO    pReturn
    )
{
    NTSTATUS            Status = STATUS_SUCCESS;
    ULONG               ulOutputBufferSize = 0UL;
    PIO_STACK_LOCATION  pIrpStack  = NULL;
    UNREFERENCED_PARAMETER(pKSMethod);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
    }

#ifdef ISDBS_RECEIVER 
    const USHORT        usTSIDs[] = {0x1234};
    ULONG               ulSizeOfTSIdBuffer = sizeof(usTSIDs);
    ULONG               ulMinRequiredBuffer = sizeof(BDA_TS_SELECTORINFO) - sizeof(USHORT) + ulSizeOfTSIdBuffer;

    if (ulOutputBufferSize < ulMinRequiredBuffer)
    {
        pIrp->IoStatus.Information = ulMinRequiredBuffer;
        return STATUS_SUCCESS;
    }

    pReturn->bTSInfolength = sizeof(BDA_TS_SELECTORINFO);
    pReturn->bReserved[0] = 0xFF;
    pReturn->bReserved[1] = 0xFF;
    pReturn->guidNetworkType = ISDB_SATELLITE_TV_NETWORK_TYPE;
    pReturn->bTSIDCount = ARRAYSIZE(usTSIDs);
    for (int i = 0; i < pReturn->bTSIDCount; i++)
    {
        pReturn->usTSID[0] = usTSIDs[i];
    }

    pIrp->IoStatus.Information = sizeof(BDA_TS_SELECTORINFO);
#elif defined(DVBT_RECEIVER) || defined(DMBTH_RECEIVER)     ///DMBTH ADD

    CFilter * pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);

    //todo: based on a selected frequency/transport return the L1 buffer or PLP Loop
    BDA_DVBT2_L1_SIGNALLING_DATA *pL2Buffer = (BDA_DVBT2_L1_SIGNALLING_DATA *)pReturn;
    ULONG ulMinRequiredBuffer = sizeof(BDA_DVBT2_L1_SIGNALLING_DATA);

    DbgPrint("BDA_TsSelectorGetTSInformation: Frequency %d",pFilter->m_CurResource.ulCarrierFrequency);

    ULONG ulL1PostSigBufferLength = 0;

    if ( pFilter->m_CurResource.ulCarrierFrequency == 729833)
    {
        ulL1PostSigBufferLength = 24;
        ulMinRequiredBuffer = ulMinRequiredBuffer - 1 + ulL1PostSigBufferLength;

    if (ulOutputBufferSize < ulMinRequiredBuffer)
    {
        pIrp->IoStatus.Information = ulMinRequiredBuffer;
            return STATUS_MORE_ENTRIES;
        }
        
        //25 Byte L1 Pre signalling
        pL2Buffer->L1Pre_TYPE = 0x00;
        pL2Buffer->L1Pre_BWT_S1_S2 = 0x86;
        pL2Buffer->L1Pre_REPETITION_GUARD_PAPR = 0x40;
        pL2Buffer->L1Pre_MOD_COD_FEC = 0x30;
        pL2Buffer->L1Pre_POSTSIZE_INFO_PILOT[0] = 0x00; pL2Buffer->L1Pre_POSTSIZE_INFO_PILOT[1] = 0x3E; pL2Buffer->L1Pre_POSTSIZE_INFO_PILOT[2] = 0x80;
        pL2Buffer->L1Pre_POSTSIZE_INFO_PILOT[3] = 0x13; pL2Buffer->L1Pre_POSTSIZE_INFO_PILOT[4] = 0xE6;
        pL2Buffer->L1Pre_TX_ID_AVAIL = 0x00;
        pL2Buffer->L1Pre_CELL_ID[0] = 0x00; pL2Buffer->L1Pre_CELL_ID[1] = 0x00;
        pL2Buffer->L1Pre_NETWORK_ID[0] = 0x30; pL2Buffer->L1Pre_NETWORK_ID[1] = 0x85;
        pL2Buffer->L1Pre_T2SYSTEM_ID[0] = 0x80; pL2Buffer->L1Pre_T2SYSTEM_ID[1] = 0x01;
        pL2Buffer->L1Pre_NUM_T2_FRAMES = 0x02;
        pL2Buffer->L1Pre_NUM_DATA_REGENFLAG_L1POSTEXT[0] = 0x03; pL2Buffer->L1Pre_NUM_DATA_REGENFLAG_L1POSTEXT[1] = 0xB0;
        pL2Buffer->L1Pre_NUMRF_CURRENTRF_RESERVED[0] = 0x20; pL2Buffer->L1Pre_NUMRF_CURRENTRF_RESERVED[1] = 0x00;
        pL2Buffer->L1Pre_CRC32[0] = 0x49; pL2Buffer->L1Pre_CRC32[1] = 0xFF; pL2Buffer->L1Pre_CRC32[2] = 0xBA; pL2Buffer->L1Pre_CRC32[3] = 0xFA;

        pL2Buffer->L1PostData[0] = 0x00;
        pL2Buffer->L1PostData[1] = 0x02;
        pL2Buffer->L1PostData[2] = 0x02;
        pL2Buffer->L1PostData[3] = 0x00;
        pL2Buffer->L1PostData[4] = 0x00;
        pL2Buffer->L1PostData[5] = 0xAE;
        pL2Buffer->L1PostData[6] = 0x01;
        pL2Buffer->L1PostData[7] = 0x7D;
        pL2Buffer->L1PostData[8] = 0xD4;
        pL2Buffer->L1PostData[9] = 0x00;//PLP - 2bit
        pL2Buffer->L1PostData[10] = 0x8C;
        pL2Buffer->L1PostData[11] = 0x00;
        pL2Buffer->L1PostData[12] = 0x00;
        pL2Buffer->L1PostData[13] = 0x4B;
        pL2Buffer->L1PostData[14] = 0xA6;
        pL2Buffer->L1PostData[15] = 0x50;
        pL2Buffer->L1PostData[16] = 0x08;
        pL2Buffer->L1PostData[17] = 0x18;
        pL2Buffer->L1PostData[18] = 0x00;
        pL2Buffer->L1PostData[19] = 0x00;
        pL2Buffer->L1PostData[20] = 0x00;
        pL2Buffer->L1PostData[21] = 0x00;
        pL2Buffer->L1PostData[22] = 0x00;
        pL2Buffer->L1PostData[23] = 0x00;//end at reserved2 left 1 bit unused
                
        pIrp->IoStatus.Information = ulMinRequiredBuffer;

    }
    else if ( pFilter->m_CurResource.ulCarrierFrequency == 721833)
    {
        ulL1PostSigBufferLength = 35;
        ulMinRequiredBuffer = ulMinRequiredBuffer - 1 + ulL1PostSigBufferLength;

        if (ulOutputBufferSize < ulMinRequiredBuffer)
        {
            pIrp->IoStatus.Information = ulMinRequiredBuffer;
            return STATUS_MORE_ENTRIES;
        }
        
        //25 Byte L1 Pre signalling
        pL2Buffer->L1Pre_TYPE = 0x00;
        pL2Buffer->L1Pre_BWT_S1_S2 = 0x86;
        pL2Buffer->L1Pre_REPETITION_GUARD_PAPR = 0x40;
        pL2Buffer->L1Pre_MOD_COD_FEC = 0x30;
        pL2Buffer->L1Pre_POSTSIZE_INFO_PILOT[0] = 0x00; pL2Buffer->L1Pre_POSTSIZE_INFO_PILOT[1] = 0x3E; pL2Buffer->L1Pre_POSTSIZE_INFO_PILOT[2] = 0x80;
          pL2Buffer->L1Pre_POSTSIZE_INFO_PILOT[3] = 0x13; pL2Buffer->L1Pre_POSTSIZE_INFO_PILOT[4] = 0xE6;
        pL2Buffer->L1Pre_TX_ID_AVAIL = 0x00;
        pL2Buffer->L1Pre_CELL_ID[0] = 0x00; pL2Buffer->L1Pre_CELL_ID[1] = 0x00;
        pL2Buffer->L1Pre_NETWORK_ID[0] = 0x30; pL2Buffer->L1Pre_NETWORK_ID[1] = 0x85;
        pL2Buffer->L1Pre_T2SYSTEM_ID[0] = 0x80; pL2Buffer->L1Pre_T2SYSTEM_ID[1] = 0x01;
        pL2Buffer->L1Pre_NUM_T2_FRAMES = 0x02;
        pL2Buffer->L1Pre_NUM_DATA_REGENFLAG_L1POSTEXT[0] = 0x03; pL2Buffer->L1Pre_NUM_DATA_REGENFLAG_L1POSTEXT[1] = 0xB0;
        pL2Buffer->L1Pre_NUMRF_CURRENTRF_RESERVED[0] = 0x20; pL2Buffer->L1Pre_NUMRF_CURRENTRF_RESERVED[1] = 0x00;
        pL2Buffer->L1Pre_CRC32[0] = 0x49; pL2Buffer->L1Pre_CRC32[1] = 0xFF; pL2Buffer->L1Pre_CRC32[2] = 0xBA; pL2Buffer->L1Pre_CRC32[3] = 0xFA;

        pL2Buffer->L1PostData[0] = 0x00;
        pL2Buffer->L1PostData[1] = 0x02;
        pL2Buffer->L1PostData[2] = 0x04; //2PLPs
        pL2Buffer->L1PostData[3] = 0x00;
        pL2Buffer->L1PostData[4] = 0x00;
        pL2Buffer->L1PostData[5] = 0xAC;
        pL2Buffer->L1PostData[6] = 0x19;
        pL2Buffer->L1PostData[7] = 0x35;
        pL2Buffer->L1PostData[8] = 0xD4;
        pL2Buffer->L1PostData[9] = 0x00;//PLP - 2bit
        pL2Buffer->L1PostData[10] = 0x8C;
        pL2Buffer->L1PostData[11] = 0x00;
        pL2Buffer->L1PostData[12] = 0x00;
        pL2Buffer->L1PostData[13] = 0x4B;
        pL2Buffer->L1PostData[14] = 0xA6;
        pL2Buffer->L1PostData[15] = 0x50;
        pL2Buffer->L1PostData[16] = 0x08;
        pL2Buffer->L1PostData[17] = 0x18;
        pL2Buffer->L1PostData[18] = 0x00;
        pL2Buffer->L1PostData[19] = 0x00;

        pL2Buffer->L1PostData[20] = 0x02;//PLP - 1bit
        pL2Buffer->L1PostData[21] = 0x46;
        pL2Buffer->L1PostData[22] = 0x00;
        pL2Buffer->L1PostData[23] = 0x00;
        pL2Buffer->L1PostData[24] = 0x25;
        pL2Buffer->L1PostData[25] = 0xD3;
        pL2Buffer->L1PostData[26] = 0x28;
        pL2Buffer->L1PostData[27] = 0x04;
        pL2Buffer->L1PostData[28] = 0x0C;
        pL2Buffer->L1PostData[29] = 0x00;
        pL2Buffer->L1PostData[30] = 0x00;

        pL2Buffer->L1PostData[31] = 0x00;
        pL2Buffer->L1PostData[32] = 0x00;
        pL2Buffer->L1PostData[33] = 0x00;
        pL2Buffer->L1PostData[34] = 0x00;//end at reserved2 
                
        pIrp->IoStatus.Information = ulMinRequiredBuffer;

    }
    else
    {
    pIrp->IoStatus.Information = 0;//no data
    }

#endif
    return Status;
}

#endif // ISDBS_RECEIVER



 // USER ACTIVITY SERVICE Handlers

/*
 ** SetUserActivityUseReason() method of the CFilter class
 **
 **    Set the use reason for the PBDA Tuner.
 **
 */
 NTSTATUS
 CFilter::SetUserActivityUseReason(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT PLONG       pPbdaResult
        )
{
    NTSTATUS            Status = STATUS_SUCCESS;
    CFilter *           pFilter;
    ULONG               ulOutputBufferSize = 0;
    PIO_STACK_LOCATION  pIrpStack  = NULL;
    KSM_BDA_USERACTIVITY_USEREASON *pUserActivityUseReason = (KSM_BDA_USERACTIVITY_USEREASON *)pKSMethod;

    ASSERT( pIrp);
    ASSERT( pKSMethod);
    // pulReturn needs to be verified because minData is zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pPbdaResult)
    {
        pIrp->IoStatus.Information = sizeof(PBDARESULT);
        return STATUS_MORE_ENTRIES;
    }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::GetChangeState() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
    }

    if (ulOutputBufferSize < sizeof(PBDARESULT))
    {
        DbgPrint("SetUserActivityUseReason: Output size %d too small",ulOutputBufferSize);
        pIrp->IoStatus.Information = sizeof(PBDARESULT);
        return STATUS_BUFFER_OVERFLOW;
    }

    DbgPrint("SetUserActivityUseReason: UseReason = %d \n", pUserActivityUseReason->ulUseReason);

    *pPbdaResult = 0;  //return the PBDARESULT

    pIrp->IoStatus.Information = sizeof(PBDARESULT);

    return Status;

}

/*
 ** GetUserActivityIntervall() method of the CFilter class
 **
 **    Get the user activity interval for the PBDA Tuner.
 **
 */
 NTSTATUS
 CFilter::GetUserActivityInterval(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT BDA_USERACTIVITY_INTERVAL  *pActivityInterval
        )
{
    NTSTATUS            Status = STATUS_SUCCESS;
    CFilter *           pFilter;
    ULONG               ulOutputBufferSize = 0;
    PIO_STACK_LOCATION  pIrpStack  = NULL;
    UNREFERENCED_PARAMETER(pKSMethod);

    ASSERT( pIrp);
    ASSERT( pKSMethod);
    // pulReturn needs to be verified because minData is zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pActivityInterval)
    {
        pIrp->IoStatus.Information = sizeof(BDA_USERACTIVITY_INTERVAL);
        return STATUS_MORE_ENTRIES;
    }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::GetChangeState() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
    }

    if (ulOutputBufferSize < sizeof(BDA_USERACTIVITY_INTERVAL))
    {
        DbgPrint("GetUserActivityInterval: Output size %d too small",ulOutputBufferSize);
        pIrp->IoStatus.Information = sizeof(PBDARESULT);
        return STATUS_BUFFER_OVERFLOW;
    }

    pActivityInterval->ulActivityInterval = 0;
    pActivityInterval->lResult = 0;  //return the PBDARESULT
    DbgPrint("GetUserActivityInterval: return Interval = %d \n", pActivityInterval->ulActivityInterval);

    pIrp->IoStatus.Information = sizeof(BDA_USERACTIVITY_INTERVAL);

    return Status;

}

/*
 ** UserActivityDetected() method of the CFilter class
 **
 **    Acknowledge the PBDA Tuner for a detected user activity.
 **
 */
 NTSTATUS
 CFilter::UserActivityDetected(
        IN PIRP         pIrp,
        IN PKSMETHOD    pKSMethod,
        OUT PLONG       pPbdaResult
        )
{
    NTSTATUS            Status = STATUS_SUCCESS;
    CFilter *           pFilter;
    ULONG               ulOutputBufferSize = 0;
    PIO_STACK_LOCATION  pIrpStack  = NULL;
    UNREFERENCED_PARAMETER(pKSMethod);

    ASSERT( pIrp);
    ASSERT( pKSMethod);
    // pulReturn needs to be verified because minData is zero
    // in the KSMETHOD_ITEM definition in bdamedia.h
    if (!pPbdaResult)
    {
        pIrp->IoStatus.Information = sizeof(PBDARESULT);
        return STATUS_MORE_ENTRIES;
    }   

    //  Obtain a "this" pointer to the filter object.
    //
    //  Because the property dispatch table calls the CFilter::GetChangeState() method 
    //  directly, the method must retrieve a pointer to the underlying filter object.
    //
    pFilter = reinterpret_cast<CFilter *>(KsGetFilterFromIrp(pIrp)->Context);
    ASSERT( pFilter);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    if(pIrpStack)
    {
        ulOutputBufferSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
    }

    if (ulOutputBufferSize < sizeof(PBDARESULT))
    {
        DbgPrint("UserActivityDetected: Output size %d too small",ulOutputBufferSize);
        pIrp->IoStatus.Information = sizeof(PBDARESULT);
        return STATUS_BUFFER_OVERFLOW;
    }

    DbgPrint("UserActivityDetected called\n");

    *pPbdaResult = 0;  //return the PBDARESULT

    pIrp->IoStatus.Information = sizeof(PBDARESULT);

    return Status;

}

//utility function for string to ULONG64 conversion
ULONG64 atoul64(IN __nullterminated char *pStr)
{
    ULONG64 ulValue64=0;
    ULONG64 ullMultiplier=1;
    BYTE bCount=0;
    if (pStr)
    {
        char *pStrPos = pStr;
        while(*pStrPos && bCount < 21)
        {
            pStrPos++;
            bCount++;
        }
        if (bCount == 21) return 0;//too big to convert
        pStrPos--;
        while (bCount)
        {
            if ((*pStrPos < 0x30) || (*pStrPos > 0x39))
            {
                return 0;//not a numeric string
            }
            ulValue64 += (*pStrPos - 0x30)*ullMultiplier;
            ullMultiplier = ullMultiplier * 10;
            pStrPos--;
            bCount--;
        }

    }
    return ulValue64;
}

//Utility function for XML text search
//Find a target numeric value from a source buffer  
//The value is retrieved from a tag e.g. Frequency="506000"
//Break search for the value is < appear
 NTSTATUS
 CFilter::FindAndConvertStringValue(__in_ecount(ulSourceLen) char *pSourceStr, ULONG &ulSourceLen,__in_ecount(ulSourceLen+1) PCSTR pTargetStr, LONG *plValue, ULONG64 *pulValue64)
 {
     if ((!pSourceStr) || (!pTargetStr) )
     {
         return STATUS_UNSUCCESSFUL;
     }

     int strTargetLength = 0;
     strTargetLength = (int) strlen(pTargetStr);

     ULONG i = 0;
     int ulDetected = 0;
     char ValueStr[21];//maximum numeric value for ULONGLONG 

     ValueStr[0] = 0;

     while (i < ulSourceLen)
     {
        if (pSourceStr[i] == pTargetStr[ulDetected])
        {
            ulDetected++;
            if (ulDetected == strTargetLength)
            {
                i++;
                break;
            }
        }
        else
        {
            ulDetected = 0;
        }
        i++;
     }

     //search the string inbetween " as start after the Name of the Value
     int k=0;
     while (i < ulSourceLen)
     {
        if (pSourceStr[i] == '"')
        {
            if ((k != 0) || (k > 19))    
            {
                i++;
                ValueStr[k] = 0;//terminating 0
                break;
            }
            i++;
            if (i < ulSourceLen)
            {
            ValueStr[k] = pSourceStr[i];
            }
            k++;
        }
        else if (k > 0)
        {
            ValueStr[k] = pSourceStr[i];
            k++;
            if (k > 19)    
            {
                ValueStr[k] = 0;//terminating 0
                k++; i++;
                break;
            }
        }
        else if (pSourceStr[i] == '<')
        {
            //we stop at a delimiter
            if (plValue)*plValue = 0;
            if (pulValue64)*pulValue64 = 0;
            ulSourceLen -= i;
            return STATUS_SUCCESS; 
        }
        i++;
     }

     if ( k != 0 )
     {
       
        if (pulValue64) *pulValue64 = atoul64( ValueStr );

        if (plValue) 
        {
       *plValue = atol( ValueStr );
            if (*plValue == 0)
            {// we might want to return converted text values
                if (k >=8)
                {
                    if (RtlCompareMemory(ValueStr,"Linear_H",8) == 8)
                    {
                        *plValue = 1;
                    }
                    else if (RtlCompareMemory(ValueStr,"Linear_V",8) == 8)
                    {
                        *plValue = 2;
                    }
                }
                if (k >=7)
                {
                    if (RtlCompareMemory(ValueStr,"SourceA",7) == 7)
                    {
                        *plValue = 1;
                    }
                    else if (RtlCompareMemory(ValueStr,"SourceB",7) == 7)
                    {
                        *plValue = 2;
                    }
                    else if (RtlCompareMemory(ValueStr,"SourceC",7) == 7)
                    {
                        *plValue = 3;
                    }
                    else if (RtlCompareMemory(ValueStr,"SourceD",7) == 7)
                    {
                        *plValue = 4;
                    }
                }
            }
            DbgPrint("FindAndConvertStringValue: found %s return %d\n", ValueStr, *plValue);
        }
     }
     else
     {
        return STATUS_UNSUCCESSFUL;
     }

     if (ulSourceLen >= i)
     {
     ulSourceLen -= i; //correct the length
     }
     else
     {
         ulSourceLen = 0;
     }

     return STATUS_SUCCESS;
 }


//find the XML Tag from the start of the buffer
//TargetStr is the tag that we search for
NTSTATUS
CFilter::FindXMLString(__in_ecount(ulSourceLen) char *pSourceStr, ULONG &ulSourceLen,__in_ecount(ulSourceLen+1) PCSTR pTargetStr,__in_ecount(*pulValueLen) char **pValue, ULONG *pulValueLen)
{
     if ((!pSourceStr) || (!pTargetStr) || (!pValue) || (!pulValueLen))
     {
         return STATUS_UNSUCCESSFUL;
     }

     int strTargetLength = (int) strlen(pTargetStr);
     ULONG i = 0;
     int ulDetected = 0;
     if (pTargetStr[0] == '<')
     { //remove the start char if available
        pTargetStr++;
        strTargetLength--;
     }

     while (i < ulSourceLen)
     {
        if (pSourceStr[i] == pTargetStr[ulDetected])
        {
            ulDetected++;
            if (ulDetected == strTargetLength)
            {
                i++;
                break;
            }
        }
        else
        {
            ulDetected = 0;
        }
        i++;
     }

     if (ulDetected != strTargetLength)
     {
         *pulValueLen=0;
         return STATUS_UNSUCCESSFUL;
     }
     *pValue = pSourceStr + i - strTargetLength -1; //start of the string including the < char at the beginning

     //search the string inbetween pTargetStr as start until / and the string and > appear as end
     while (i < ulSourceLen)
     {
        if (pSourceStr[i] == '/')
        {
             i++; ulDetected=0;
             while (i < ulSourceLen)
             {
                if (pSourceStr[i] == pTargetStr[ulDetected])
                {
                    ulDetected++;
                    if (ulDetected == strTargetLength)
                    {
                        while (i < ulSourceLen)
                        {
                            if (pSourceStr[i] == '>')
                            {
                                break;
                            }
                            i++; //potentially another char
                        }
                        if (i >= ulSourceLen)
                        {
                            break;
                        }
                        if (pSourceStr[i] != '>')
                        {
                            break; //wrong format
                        }
                        i++; //skip >
                        i++; //set end
                        *pulValueLen = (UINT32)((pSourceStr + i) - (*pValue));
                        return STATUS_SUCCESS;
                    }
                }
                else
                {
                    break; //not the target end
                }
                i++;
             }
        }
        i++;
     }

     return STATUS_UNSUCCESSFUL;
}

//Helper function for sending a Brodacast MMI
HRESULT
CFilter::SendBroadcastMMI(UINT uiType)
{

    KIRQL               Irql;
    NTSTATUS            Status = STATUS_SUCCESS;
    KeAcquireSpinLock( &spinLock, &Irql);
    
    if ((m_PendingEventEntry == NULL)||(m_FilterEventsManager.IsEnabled() == FALSE))
    {
        DbgPrint("CFilter:: Eventing not enabled while sending a BroadcastMMI");
        Status = STATUS_UNSUCCESSFUL;
    }
    else
    {
        PBDATUNER_EVENT pTunerEvent;
        pTunerEvent = AllocateTunerEvent(&EVENTID_BDA_CASBroadcastMMI, 0,NULL);
        //a real device have to consider the internal timeout time to enter a lock state

        if (pTunerEvent == NULL)
        {
            DbgPrint("CFilter::BroadcastMMI could not allocate an event");
            Status = STATUS_UNSUCCESSFUL;
        }
        else
        {
            m_fBroadcastMMIRequested = TRUE;
            m_uiBroadcastMMIType = uiType;
            m_FilterEventsManager.QueueEvent(pTunerEvent,TRUE);
        }
    }
    KeReleaseSpinLock( &spinLock, Irql);

    return Status;
}

HRESULT 
CFilter::SendOpenMMI(ULONG ulDialogRequest, GUID uuidDialogType, PCSTR pPageData)
{
    KIRQL               Irql;
    NTSTATUS            Status = STATUS_SUCCESS;
    KeAcquireSpinLock( &spinLock, &Irql);
    
    if ((m_PendingEventEntry == NULL)||(m_FilterEventsManager.IsEnabled() == FALSE))
    {
        DbgPrint("CFilter:: Eventing not enabled while sending a OpenMMI");
        Status = STATUS_UNSUCCESSFUL;
    }
    else
    {
        PBDATUNER_EVENT pTunerEvent;

        ULONG ulOpenMMIDatalength = sizeof(BDA_CAS_OPENMMIDATA) + strlen(pPageData)+ 1  - MIN_DIMENSION;
        BDA_CAS_OPENMMIDATA *pOpenMMIEvent = (BDA_CAS_OPENMMIDATA *) ExAllocatePoolWithTag(NonPagedPool, ulOpenMMIDatalength, MS_SAMPLE_TUNER_POOL_TAG);

        if (pOpenMMIEvent == NULL)
        {
            KeReleaseSpinLock( &spinLock, Irql);
            return STATUS_UNSUCCESSFUL;         
        }
        RtlZeroMemory(pOpenMMIEvent, sizeof(BDA_CAS_OPENMMIDATA));

        m_ulCurrentDialogNumber++;
        if (m_ulCurrentDialogNumber == 0) m_ulCurrentDialogNumber++;

        pOpenMMIEvent->ulDialogNumber = m_ulCurrentDialogNumber;
        pOpenMMIEvent->ulDialogRequest = ulDialogRequest;
        pOpenMMIEvent->usDialogDataLength = (USHORT)strlen(pPageData);
        pOpenMMIEvent->uuidDialogType = uuidDialogType;
        memcpy(pOpenMMIEvent->argbDialogData, pPageData, pOpenMMIEvent->usDialogDataLength+1);

        pTunerEvent = AllocateTunerEvent(&EVENTID_BDA_CASOpenMMI, ulOpenMMIDatalength,(UCHAR*)pOpenMMIEvent);
        //a real device have to consider the internal timeout time to enter a lock state

        if (pTunerEvent == NULL)
        {
            DbgPrint("CFilter::BroadcastMMI could not allocate an event");
            Status = STATUS_UNSUCCESSFUL;
        }
        else
        {
            m_FilterEventsManager.QueueEvent(pTunerEvent,TRUE);
        }
        ExFreePoolWithTag(pOpenMMIEvent, MS_SAMPLE_TUNER_POOL_TAG);
    }
    KeReleaseSpinLock( &spinLock, Irql);

    return Status;
}

void CFilter::SetGPNVValue(__in __drv_in(__drv_aliasesMem) WCHAR *pValue, int RowIndex)
{
    if (m_GpnvsTable[RowIndex][1] != NULL)
    {
        ExFreePoolWithTag(m_GpnvsTable[RowIndex][1], MS_SAMPLE_TUNER_POOL_TAG); 
    }
    m_GpnvsTable[RowIndex][1] = pValue;        
}

