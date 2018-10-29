/*++ BUILD Version: 0000    

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    MessageTask.cpp

Abstract:
    Implements the class CMessageTask
 
Revision History:

--*/
#include "StdAfx.h"
#include ".\messagetask.h"
#include "..\softhiddevice\common.h"

CMessageTask::CMessageTask(void)
/*++
Routine Description:
    Constructor for CMessageTask

Synchronization: 
    None
 
Arguments:
    None

Return Value:
    None
    
--*/
{
    InitMemberVariables();

} // CMessageTask::CMessageTask

CMessageTask::~CMessageTask(void)
/*++
Routine Description:
    Destructor for CMessageTask

Synchronization: 
    None
 
Arguments:
    None

Return Value:
    None
    
--*/
{
    InitMemberVariables();

} // CMessageTask::~CMessageTask

HRESULT CMessageTask::Start(DWORD dwDelay)
/*++
Routine Description:
    Starts Win32 thread and creates message queue.

Synchronization: 
    Waits on Start event, which is fired from main thread routine Run()
    by successful creation of thread message queue.
 
Arguments:
    dwDelay - timeout to start thread

Return Value:
    S_OK
        Success
    Failure code is returned by HRESULT_FROM_WIN32 macro
    
--*/
{
    HRESULT hr = S_OK;

    // Create start event
    m_hStartEvt = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (NULL == m_hStartEvt)
    {
        IfFailHrGo(HRESULT_FROM_WIN32(::GetLastError()));
    }

    // Register quit message
    IfFailHrGo(RegisterMessage(MSG_QUIT, &m_uiQuitMsg));

    // Call Start method from the base class
    IfFailHrGo(CTask::Start(dwDelay));

    // Wait until thread creates message queue
    IfFalseHrGo(WAIT_OBJECT_0 == WaitForSingleObject(m_hStartEvt, dwDelay), E_UNEXPECTED);

Exit:
    return hr;

} // CMessageTask::Start


HRESULT CMessageTask::End(DWORD dwDelay)
/*++
Routine Description:
    Posts QUIT message and signals Win32 thread to stop.

Synchronization: 
    Is implemented in CTask::End() mewthod.
 
Arguments:
    dwDelay - timeout to start thread

Return Value:
    S_OK
        Success
    Failure code is returned by HRESULT_FROM_WIN32 macro
    
--*/
{
    HRESULT hr = S_OK;

    // Custom QUIT message is sent instead of WM_QUIT message  
    // because quit message should have same priority as other 
    // user-defined messages.
    if (!PostThreadMessage(m_dwThreadId, m_uiQuitMsg, 0, 0))
    {
        IfFailHrGo(HRESULT_FROM_WIN32(::GetLastError()));
    }

    // Call End method from the base class
    IfFailHrGo(CTask::End(dwDelay));

Exit:
    return hr;

} // CMessageTask::End

HRESULT CMessageTask::RegisterMessage(LPCTSTR lpString, TASK_MSG* pMsg)
/*++
Routine Description:
    Registers new message ID that is guaranteed to be 
    unique in the system.

Synchronization: 
    None
 
Arguments:
    lpString - Pointer to a null-terminated string (usually GUID).
    pMsg - message ID in the range 0xC000 through 0xFFFF
    
Return Value:
    S_OK
        Success
    Failure code is returned by HRESULT_FROM_WIN32 macro

--*/
{
    HRESULT hr = S_OK;

    *pMsg = (TASK_MSG)RegisterWindowMessage(lpString);
    if (0 == *pMsg)
    {
        IfFailHrGo(HRESULT_FROM_WIN32(::GetLastError()));
    }

Exit:
    return hr;

} // CMessageTask::RegisterMessage

HRESULT CMessageTask::PostMessage2Task(TASK_MSG msg, WPARAM wParam, LPARAM lParam)
/*++
Routine Description:
    Places a message in the message queue of the task

Synchronization: 
    None
 
Arguments:
    msg - message ID
    wParam - 1-st message parameter
    lParam - 2-nd message parameter
    
Return Value:
    S_OK
        Success
    Failure code is returned by HRESULT_FROM_WIN32 macro

--*/
{
    HRESULT hr = S_OK;

    if (FALSE == ::PostThreadMessage(m_dwThreadId, msg, wParam, lParam))
    {
        IfFailHrGo(HRESULT_FROM_WIN32(::GetLastError()));
    }

Exit:
    return hr;

} // CMessageTask::PostMessage2Task

void CMessageTask::InitMemberVariables(void)
/*++
Routine Description:
    Initialize member variables of the class CMessageTask

Synchronization: 
    None
 
Arguments:
    None
    
Return Value:
    None
--*/
{
    m_hStartEvt = NULL;
    m_uiQuitMsg = 0;

} // CMessageTask::InitMemberVariables

DWORD CMessageTask::Run(void)
/*++
Routine Description:
    Creates thread message queue and distributes messages
    to OnMsgReceived() method.

Synchronization: 	
    Fires Start event to release Start() method.
 
Arguments:
    None

Return Value:
    Thread exit code 0.
    
--*/
{
    MSG msg;
    // Call PeekMessage to create message queue
    (void)PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
    // Signal Start() method that thread is created and initialized
    (void)SetEvent(m_hStartEvt);

    // Message loop
    while(GetMessage(&msg, NULL, 0, 0))
    {
        OnMsgReceived(msg.message, msg.wParam, msg.lParam);		

        // Simulate WM_QUIT
        if (m_uiQuitMsg == msg.message)
            break;
    }

    return 0;

} // CMessageTask::Run

void CMessageTask::OnMsgReceived(TASK_MSG msg, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(msg);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
}

HRESULT CMessageTask::SetTimer(TASK_TIMER& rTimer, UINT uiElapse)
/*++
Routine Description:
    Initialize timer, which generates WM_TIMER message.

Synchronization: 
    None
 
Arguments:
    rTimer - timer ID
    uElapse - timeout in milliseconds
    
Return Value:
    S_OK
        Success
    Failure code is returned by HRESULT_FROM_WIN32 macro

--*/
{
    HRESULT hr = S_OK;

    // We assume that if uElapse is set to or greater than 0x7fffffff
    // then this is an infinite timeout, so there is no need to set 
    // timer at all. Exit the function without a failure.
    if (USER_TIMER_MAXIMUM <= uiElapse)
        goto Exit;

    rTimer = ::SetTimer(NULL, NULL, uiElapse, NULL);
    if (0 == rTimer)
    {
        IfFailHrGo(HRESULT_FROM_WIN32(::GetLastError()));
    }

Exit:
    return hr;

} // CMessageTask::SetTimer

HRESULT CMessageTask::KillTimer(TASK_TIMER& rTimer)
/*++
Routine Description:
    Destroys timer.

Synchronization: 
    None
 
Arguments:
    rTimer - timer ID
    
Return Value:
    S_OK
        Success
    Failure code is returned by HRESULT_FROM_WIN32 macro

--*/
{
    HRESULT hr = S_OK;

    if (rTimer)
    {
        if (FALSE == ::KillTimer(NULL, rTimer))
        {
            hr = HRESULT_FROM_WIN32(::GetLastError());
        }
        rTimer = 0;
    }

    return hr;

} // CMessageTask::KillTimer

