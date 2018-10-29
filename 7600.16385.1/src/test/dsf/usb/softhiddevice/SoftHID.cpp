/*++ BUILD Version: 0000    

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    SoftHID.cpp

Abstract:
    Implements the class CSoftHID.
 
Revision History:

--*/
#include "StdAfx.h"
#include <dsfif.h>
#include <softhidusbif.h>
#include <InputKbd.h>
#include "SoftHidDevice.h"
#include ".\softhid.h"
#include "Common.h"

// Set 500 ms polling rate at which we make requests to kernel mode
const DWORD KERNEL_MODE_POLLING_RATE = 500;

CSoftHID::CSoftHID(void)
: m_pMsgTask(NULL)
/*++
Routine Description:
    Constructor for CSoftHIDDevice

Synchronization: None
 
Arguments:
    None

Return Value:
    None
    
--*/
{
    InitMemberVariables();
}


CSoftHID::~CSoftHID(void)
/*++
Routine Description:
    Destructor for CSoftHIDDevice

Synchronization: None
 
Arguments:
    None

Return Value:
    None
    
--*/
{
    // Disconnect from SoftUsbHidDevice object.
    Disconnect();

    // Initialize the member variables 
    InitMemberVariables();
}

HRESULT CSoftHID::OnConstruct(CMessageTask* pMsgTask, CMessageTask::TASK_MSG msg)
/*++
Routine Description:
    Should be called when object is constructed. Creates the default
    properties.

Synchronization: 
    None
 
Arguments:
    None
    
Return Value:
    From called function
--*/
{
    HRESULT hr = S_OK;

    IfFalseHrGo(NULL != pMsgTask, E_FAIL);
    
    m_pMsgTask = pMsgTask;
    m_OutReportReadyMsg = msg;

Exit:
    return hr;

}

HRESULT CSoftHID::Connect(ISoftHIDProtocolXlator* piSoftProtXlator)
/*++
Routine Description:
    Performs actions to connect to SoftUsbHidDevice object. Starts
    calling thread in the context of which ReadOutputReport() is called.

Synchronization: 
    None
 
Arguments:
    None
    
Return Value:
    S_OK
        Success
    E_FAIL
--*/    
{
    HRESULT hr = S_OK;

    IfFalseHrGo(NULL != piSoftProtXlator, E_FAIL);

    // Set reference to ISoftHIDProtocolXlator interface
    RELEASE(m_piSoftProtXlator);
    m_piSoftProtXlator = piSoftProtXlator;
    ADDREF(m_piSoftProtXlator);
    // Start calling thread
    IfFailHrGo(Start());

Exit:
    return hr;
}

HRESULT CSoftHID::Disconnect(void)
/*++
Routine Description:
    Performs actions to disconnect from SoftUsbHidDevice object. 
    Cancels blocking call ReadOutputReport() and stops calling therad.

Synchronization: 
    None
 
Arguments:
    None
    
Return Value:
    S_OK
        Success
    E_FAIL
        Could not start calling thread.
--*/
{
    HRESULT hr = S_OK;

    // Stop calling thread
    IfFailHrGo(End());

Exit:
    RELEASE(m_piSoftProtXlator);
    return hr;

}


void CSoftHID::InitMemberVariables(void)
/*++
Routine Description:
    Initialize member variables of the class CSoftHIDDevice 

Synchronization: 
    None
 
Arguments:
    None
Return Value:
    None
--*/
{
    m_pMsgTask = NULL;
    m_OutReportReadyMsg = 0;
    m_piSoftProtXlator = NULL;
}

DWORD CSoftHID::Run(void)
/*++
Routine Description:
    This is a main routine for a calling thread, which implements 
    a blocking call of ISoftHIDProtocolXlator::ReadOutputReport().

Synchronization:
    None
 
Arguments:
    None

Return Value:
    Thread exit code 0.
    
--*/
{
    SAFEARRAY* psa = NULL;
    HRESULT    hr = S_OK;

    while(!NeedToStop(KERNEL_MODE_POLLING_RATE))
    {
        // Make blocking call to kernel mode
        hr = m_piSoftProtXlator->ReadOutputReport(&psa);
        if ( hr == S_OK)
        {
            // Send Output Report to processing thread
            hr = m_pMsgTask->PostMessage2Task(
                m_OutReportReadyMsg, (WPARAM)psa, (LPARAM)0);
            // Note: psa will be deallocated in
            // CSoftHIDInputKbdMapper::ProcessKbdInput
        }
    }

    return 0;
}

