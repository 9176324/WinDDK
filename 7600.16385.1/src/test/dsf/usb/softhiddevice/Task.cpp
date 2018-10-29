/*++ BUILD Version: 0000    

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    Task.cpp

Abstract:
    Implementation of CTask class
 
Revision History:

--*/ 
#include "StdAfx.h"
#include ".\task.h"
#include "..\softhiddevice\common.h"

static DWORD WINAPI ThreadProc(
  LPVOID lpParameter
)
/*++
Routine Description:
    Free static function, thread entry point.

Synchronization: None
 
Arguments:
    None

Return Value:
    From called function
    
--*/
{
    return reinterpret_cast<CTask*>(lpParameter)->Run();
}

CTask::CTask(void)
: m_dwThreadId(0)
/*++
Routine Description:
    Constructor for CTask

Synchronization: 
    None
 
Arguments:
    None

Return Value:
    None
    
--*/
{
    InitMemberVariables();

} // CTask::CTask

CTask::~CTask(void)
/*++
Routine Description:
    Destructor for CTask

Synchronization: 
    None
 
Arguments:
    None

Return Value:
    None
    
--*/
{
    InitMemberVariables();

} // CTask::~CTask

HRESULT CTask::Start(DWORD dwDelay)
/*++
Routine Description:
    Starts Win32 thread.

Synchronization: 
    None
 
Arguments:
    dwDelay - timeout to start thread

Return Value:
    S_OK
        Success
    Failure code is returned by HRESULT_FROM_WIN32 macro
    
--*/
{
    HRESULT hr = S_OK;

    UNREFERENCED_PARAMETER(dwDelay);

    // Create stop event
    m_hStopEvt = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (NULL == m_hStopEvt)
    {
        IfFailHrGo(HRESULT_FROM_WIN32(::GetLastError()));
    }

    // Create thread
    m_hThread = CreateThread(NULL, 0, ThreadProc, this, 0, &m_dwThreadId);
    if (NULL == m_hThread)
    {
        IfFailHrGo(HRESULT_FROM_WIN32(::GetLastError()));
    }

Exit:
    return hr;

} // CTask::Start

HRESULT CTask::End(DWORD dwDelay)
/*++
Routine Description:
    Signals Win32 thread to stop.

Synchronization: 
    Signals main thread function Run() to stop by setting Stop event.
    Waits until thread is finished end exits.
 
Arguments:
    dwDelay - timeout to stop thread

Return Value:
    S_OK
        Success
    E_UNEXPECTED
        Thread did not stop as it was expected.

--*/
{
    HRESULT hr = S_OK;

    // If thread is started, signal to stop and wait
    // We do not check return values from Win32 calls since
    // this is done on a best effort and we do not want to fail
    // any subsequent calls.
    if (m_hThread)
    {
        // Signal thread to stop
        (void)SetEvent(m_hStopEvt);
        // Wait for thread to stop
        if (WAIT_OBJECT_0 != WaitForSingleObject(m_hThread, dwDelay))
            hr = E_UNEXPECTED;
        // Close thread handle and reinitialize
        (void)CloseHandle(m_hThread);
        m_hThread = NULL;
        m_dwThreadId = 0;
    }

    // Close stop event handle
    if (m_hStopEvt)
    {
        (void)CloseHandle(m_hStopEvt);
        m_hStopEvt = NULL;
    }

    return hr;

} // CTask::End

void CTask::InitMemberVariables(void)
/*++
Routine Description:
    Initialize member variables of the class CTask

Synchronization: 
    None
 
Arguments:
    None
    
Return Value:
    None
--*/
{
    m_hThread = NULL;
    m_hStopEvt = NULL;
    m_dwThreadId = 0;

} // CTask::InitMemberVariables


