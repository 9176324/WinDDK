/*++ BUILD Version: 0000    

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    Task.h

Abstract:
    Defines the class CTask
 
Revision History:

--*/
#pragma once

// Default value of synchronization delay is one minute
const DWORD DEFAULT_SYNC_DELAY = 60000;

class CTask
{
public:
    CTask(void);
    virtual ~CTask(void);
protected:
    HANDLE m_hThread;
    HANDLE m_hStopEvt;
    DWORD m_dwThreadId;
public:
    virtual HRESULT Start(DWORD dwDelay = DEFAULT_SYNC_DELAY);
    HRESULT End(DWORD dwDelay = DEFAULT_SYNC_DELAY);
    virtual DWORD Run(void) = 0;
protected:

    BOOLEAN NeedToStop(DWORD dwDelay)
    {
        return (WaitForSingleObject(m_hStopEvt, dwDelay) == WAIT_TIMEOUT) ? FALSE : TRUE; 
    }

private:
    void InitMemberVariables(void);
};

