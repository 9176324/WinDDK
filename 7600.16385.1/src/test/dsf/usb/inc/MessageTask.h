/*++ BUILD Version: 0000    

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    MessageTask.h

Abstract:
    Defines the class CMessageTask
 
Revision History:

--*/

#pragma once
#include "task.h"

// Define unique name for QUIT message
#define MSG_QUIT    TEXT("77DAC497-2FEE-415F-A6A2-7BCD87A78C39")
#define USER_TIMER_MAXIMUM  0x7FFFFFFF

class CMessageTask :
    public CTask
{
public:
    typedef UINT_PTR TASK_TIMER;
    typedef UINT     TASK_MSG;

    CMessageTask(void);
    virtual ~CMessageTask(void);
    
    virtual HRESULT Start(DWORD dwDelay);
    virtual HRESULT End(DWORD dwDelay = DEFAULT_SYNC_DELAY);
    
    HRESULT RegisterMessage(LPCTSTR lpString, TASK_MSG* pMsg);
    HRESULT PostMessage2Task(TASK_MSG msg, WPARAM wParam, LPARAM lParam);

    BOOLEAN IsQuitMessage(TASK_MSG msg)
    {
        return (msg == m_uiQuitMsg) ? TRUE : FALSE;
    }
private:
    void InitMemberVariables(void);
    virtual DWORD Run(void);

protected:
    //Methods
    virtual void OnMsgReceived(TASK_MSG msg, WPARAM wParam, LPARAM lParam);
    HRESULT SetTimer(TASK_TIMER& rTimer, UINT uiElapse);
    HRESULT KillTimer(TASK_TIMER& rTimer);

    //Internal variables
    HANDLE m_hStartEvt;
    TASK_MSG m_uiQuitMsg;
};

