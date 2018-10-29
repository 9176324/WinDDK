/*++ BUILD Version: 0000    

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    SoftHID.h

Abstract:
    Defines the class CSoftHID.
 
Revision History:

--*/
#pragma once
#include "task.h"
#include "MessageTask.h"


class CSoftHID :
    public CTask
{
public:
    CSoftHID(void);
    ~CSoftHID(void);
    
    // Constants
    static const DWORD SOFTHID_TRACE_DATA   = 0x10000000;
    static const DWORD SOFTHID_TRACE_STATUS = 0x20000000;
    static const DWORD SOFTHID_TRACE_CMDS   = 0x40000000;
    static const DWORD SOFTHID_TRACE_INFO   = 0x80000000;
    static const DWORD SOFTHID_TRACE_SIMPLE = 0x01000000;

    virtual HRESULT OnConstruct(CMessageTask* pMsgTask, CMessageTask::TASK_MSG msg);
    
    HRESULT Connect(ISoftHIDProtocolXlator* piSoftProtXlator);
    HRESULT Disconnect(void);
private:
    
    void InitMemberVariables(void);
    virtual DWORD Run(void);
    
    // Soft HID Protocol Translator Interface
    ISoftHIDProtocolXlator *m_piSoftProtXlator;
    // Pointer to task that handles reports
    CMessageTask *m_pMsgTask;
    // OutReportReady message
    CMessageTask::TASK_MSG m_OutReportReadyMsg;
};

