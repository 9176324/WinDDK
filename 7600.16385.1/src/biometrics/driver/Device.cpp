/*++
 
    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
    ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.

    Copyright (c) Microsoft Corporation. All rights reserved

Module Name:

    Device.cpp

Abstract:

    This module contains the implementation of the Biometric
    device driver.

Environment:

   Windows User-Mode Driver Framework (WUDF)

--*/
#include "internal.h"
#include "device.tmh"


DWORD WINAPI
CaptureSleepThread(
    LPVOID lpParam
    ) 
{ 
    CBiometricDevice *device = (CBiometricDevice *) lpParam;
    PCAPTURE_SLEEP_PARAMS sleepParams = device->GetCaptureSleepParams();

    //
    // Make sure it is less than or equal to 1 minute.
    //
    if (sleepParams->SleepValue > 60)
    {
        sleepParams->SleepValue = 60;
    }

    Sleep(sleepParams->SleepValue * 1000);

    device->CompletePendingRequest(sleepParams->Hr, sleepParams->Information);

    return 0;
}


HRESULT
CBiometricDevice::CreateInstanceAndInitialize(
    __in IWDFDriver *FxDriver,
    __in IWDFDeviceInitialize * FxDeviceInit,
    __out CBiometricDevice **Device
    )
/*++
 
  Routine Description:

    This method creates and initializs an instance of the skeleton driver's 
    device callback object.

  Arguments:

    FxDeviceInit - the settings for the device.

    Device - a location to store the referenced pointer to the device object.

  Return Value:

    Status

--*/
{
    //
    // Create a new instance of the device class
    //
    CComObject<CBiometricDevice> *pMyDevice = NULL;
    HRESULT hr = CComObject<CBiometricDevice>::CreateInstance( &pMyDevice );

    if (SUCCEEDED(hr)) {

        //
        // Initialize the instance.  This calls the WUDF framework,
        // which keeps a reference to the device interface for the lifespan
        // of the device.
        //
        hr = pMyDevice->Initialize(FxDriver, FxDeviceInit);

        *Device = pMyDevice;

    }

    return hr;
}

HRESULT
CBiometricDevice::Initialize(
    __in IWDFDriver           * FxDriver,
    __in IWDFDeviceInitialize * FxDeviceInit
    )
/*++
 
  Routine Description:

    This method initializes the device callback object and creates the
    partner device object.

    The method should perform any device-specific configuration that:
        *  could fail (these can't be done in the constructor)
        *  must be done before the partner object is created -or-
        *  can be done after the partner object is created and which aren't 
           influenced by any device-level parameters the parent (the driver
           in this case) might set.

  Arguments:

    FxDeviceInit - the settings for this device.

  Return Value:

    status.

--*/
{
    IWDFDevice *fxDevice = NULL;
    HRESULT hr = S_OK;
    IUnknown *unknown = NULL;

    //
    // Configure things like the locking model before we go to create our 
    // partner device.
    //

    //
    // Set the locking model.
    //

    FxDeviceInit->SetLockingConstraint(WdfDeviceLevel);

    //
    // Any per-device initialization which must be done before 
    // creating the partner object.
    //

    //
    // Create a new FX device object and assign the new callback object to 
    // handle any device level events that occur.
    //

    //
    // We pass an IUnknown reference to CreateDevice, which takes its own
    // reference if everything works.
    //

    if (SUCCEEDED(hr)) 
    {
        hr = this->QueryInterface(__uuidof(IUnknown), (void **)&unknown);

    }

    if (SUCCEEDED(hr)) {

        hr = FxDriver->CreateDevice(FxDeviceInit, unknown, &fxDevice);
        BiometricSafeRelease(unknown);
    }

    //
    // If that succeeded then set our FxDevice member variable.
    //

    if (SUCCEEDED(hr))
    {
        m_FxDevice = fxDevice;

        //
        // Drop the reference we got from CreateDevice.  Since this object
        // is partnered with the framework object they have the same 
        // lifespan - there is no need for an additional reference.
        //

        BiometricSafeRelease(fxDevice);
    }

    return hr;
}

HRESULT
CBiometricDevice::Configure(
    VOID
    )
/*++
 
  Routine Description:

    This method is called after the device callback object has been initialized 
    and returned to the driver.  It would setup the device's queues and their 
    corresponding callback objects.

  Arguments:

    FxDevice - the framework device object for which we're handling events.

  Return Value:

    status

--*/
{
    
    HRESULT hr = S_OK;

    //
    // Create the I/O queue 
    //

    if (SUCCEEDED(hr)) 
    {
        hr = CBiometricIoQueue::CreateInstanceAndInitialize(m_FxDevice, this, &m_IoQueue);

        if (SUCCEEDED(hr)) 
        {
            hr = m_IoQueue->Configure();
        }
    }

    //
    // Create a manual I/O queue to hold requests for notification when
    // the switch state changes.
    //
    if (SUCCEEDED(hr)) 
    {
        hr = m_FxDevice->CreateIoQueue(NULL,
                                       FALSE,
                                       WdfIoQueueDispatchManual,
                                       FALSE,
                                       FALSE,
                                       &m_SwitchChangeQueue);
        
        if (SUCCEEDED(hr)) 
        {
            hr = m_SwitchChangeQueue->Release();
        }

    }

    //
    // Create Device Interface
    //

    if (SUCCEEDED(hr))
    {
        hr = m_FxDevice->CreateDeviceInterface(&GUID_DEVINTERFACE_BIOMETRIC_READER,
                                               NULL);
    }

    if (SUCCEEDED(hr))
    {
        hr = m_FxDevice->AssignDeviceInterfaceState(&GUID_DEVINTERFACE_BIOMETRIC_READER,
                                                    NULL,
                                                    TRUE);
    }

    //
    // TODO - this is where additional interfaces can be exposed.
    //
  
    return hr;
}

HRESULT
CBiometricDevice::OnPrepareHardware(
    __in IWDFDevice * /* FxDevice */
    )
/*++

Routine Description:

    This routine is invoked to ready the driver
    to talk to hardware. It opens the handle to the 
    device and talks to it using the WINUSB interface.
    It invokes WINUSB to discver the interfaces and stores
    the information related to bulk endpoints.

Arguments:

    FxDevice  : Pointer to the WDF device interface

Return Value:

    HRESULT 

--*/
{
    PWSTR deviceName = NULL;
    DWORD deviceNameCch = 0;

    HRESULT hr;

    //
    // Get the device name.
    // Get the length to allocate first
    //

    hr = m_FxDevice->RetrieveDeviceName(NULL, &deviceNameCch);

    if (FAILED(hr))
    {
        TraceEvents(TRACE_LEVEL_ERROR, 
                    BIOMETRIC_TRACE_DEVICE, 
                    "%!FUNC! Cannot get device name %!hresult!",
                    hr
                    );
    }

    //
    // Allocate the buffer
    //

    if (SUCCEEDED(hr))
    {
        deviceName = (PWSTR) malloc(deviceNameCch * sizeof (WCHAR));

        if (deviceName == NULL)
        {
            hr = E_OUTOFMEMORY;
        }
    }

    //
    // Get the actual name
    //

    if (SUCCEEDED(hr))
    {
        hr = m_FxDevice->RetrieveDeviceName(deviceName, &deviceNameCch);

        if (FAILED(hr))
        {
            TraceEvents(TRACE_LEVEL_ERROR, 
                        BIOMETRIC_TRACE_DEVICE, 
                        "%!FUNC! Cannot get device name %!hresult!",
                        hr
                        );
        }
    }

    if (SUCCEEDED(hr))
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, 
                    BIOMETRIC_TRACE_DEVICE, 
                    "%!FUNC! Device name %S",
                    deviceName
                    );
    }

    //
    // Create USB I/O Targets and configure them
    //

    if (SUCCEEDED(hr))
    {
        hr = CreateUsbIoTargets();
    }

    if (SUCCEEDED(hr))
    {
        ULONG length = sizeof(m_Speed);

        hr = m_pIUsbTargetDevice->RetrieveDeviceInformation(DEVICE_SPEED, 
                                                            &length,
                                                            &m_Speed);
        if (FAILED(hr)) 
        {
            TraceEvents(TRACE_LEVEL_ERROR, 
                        BIOMETRIC_TRACE_DEVICE, 
                        "%!FUNC! Cannot get usb device speed information %!HRESULT!",
                        hr
                        );
        }
    }

    if (SUCCEEDED(hr)) 
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, 
                    BIOMETRIC_TRACE_DEVICE, 
                    "%!FUNC! Speed - %x\n",
                    m_Speed
                    );
    }

    if (SUCCEEDED(hr))
    {
        hr = ConfigureUsbPipes();
    }

    //
    // Setup power-management settings on the device.
    //

    if (SUCCEEDED(hr))
    {
        hr = SetPowerManagement();
    }

    //
    // Clear the seven segement display to indicate that we're done with 
    // prepare hardware.
    //

    if (SUCCEEDED(hr))
    {
        hr = IndicateDeviceReady();
    }

    //
    // We have non-power managed queues so we Stop them in OnReleaseHardware
    // and start them in OnPrepareHardware
    //

    if (SUCCEEDED(hr))
    {
        m_IoQueue->Start();
    }

    if (SUCCEEDED(hr))
    {
        //
        // If the device stack allows read to remain pending across power-down
        // and up, it can be initiated during OnPrepareHardware
        //
        // If the device stack doesn't allow the read to remain pending (i.e. it
        // cancels the pending read during power transition) driver will have to
        // stop sending pending read during D0Exit and re-initiate it during
        // D0Entry
        //
        // USB core actually doesn't allow read to remain pending across power
        // transition but WinUSB does. Since we are layered above WinUSB we don't 
        // need to manage pending read across power transitions.
        //
        
        hr = InitiatePendingRead();
    }

    if (deviceName)
    {
        free(deviceName);
        deviceName = NULL;
    }

    return hr;
}

HRESULT
CBiometricDevice::OnReleaseHardware(
    __in IWDFDevice * /* FxDevice */
    )
/*++

Routine Description:

    This routine is invoked when the device is being removed or stopped
    It releases all resources allocated for this device.

Arguments:

    FxDevice - Pointer to the Device object.

Return Value:

    HRESULT - Always succeeds.

--*/
{
    //
    // Cancel the pending data collection I/O, if one exists.
    //
    CompletePendingRequest(HRESULT_FROM_WIN32(ERROR_CANCELLED), 0);

    //
    // Since we have non-power managed queues, we need to Stop them
    // explicitly
    //
    // We need to stop them before deleting I/O targets otherwise we
    // will continue to get I/O and our I/O processing will try to access
    // freed I/O targets
    //
    // We initialize queues in CMyDevice::Initialize so we can't get
    // here with queues being NULL and don't need to guard against that
    //

    m_IoQueue->StopSynchronously();

    //
    // Delete USB Target Device WDF Object, this will in turn
    // delete all the children - interface and the pipe objects
    //
    // This makes sure that 
    //    1. We drain the I/O before releasing the targets
    //        a. We always need to do that for the pending read which does 
    //           not come from an I/O queue
    //        b. We need to do this even for I/O coming from I/O queues because
    //           we set them to non-power managed queues (to leverage wait/wake
    //           from WinUsb.sys)
    //    2. We remove USB target objects from object tree (and thereby free them)
    //       before any potential subsequent OnPrepareHardware creates new ones
    //
    // m_pIUsbTargetDevice could be NULL if OnPrepareHardware failed so we need
    // to guard against that
    //

    if (m_pIUsbTargetDevice)
    {
        m_pIUsbTargetDevice->DeleteWdfObject();
    }

    //
    // This sample has a thread that will sleep for 5 seconds before
    // completing a capture request.
    //
    if (m_SleepThread != INVALID_HANDLE_VALUE)
    {
        WaitForSingleObject(m_SleepThread, INFINITE);
        CloseHandle(m_SleepThread);
        m_SleepThread = INVALID_HANDLE_VALUE;
    }
    
    return S_OK;
}

HRESULT
CBiometricDevice::CreateUsbIoTargets(
    )
/*++

Routine Description:

    This routine creates Usb device, interface and pipe objects

Arguments:

    None

Return Value:

    HRESULT
--*/
{
    HRESULT                 hr;
    UCHAR                   NumEndPoints = 0;
    IWDFUsbTargetFactory *  pIUsbTargetFactory = NULL;
    IWDFUsbTargetDevice *   pIUsbTargetDevice = NULL;
    IWDFUsbInterface *      pIUsbInterface = NULL;
    IWDFUsbTargetPipe *     pIUsbPipe = NULL;
    
    hr = m_FxDevice->QueryInterface(IID_PPV_ARGS(&pIUsbTargetFactory));

    if (FAILED(hr))
    {
        TraceEvents(TRACE_LEVEL_ERROR, 
                    BIOMETRIC_TRACE_DEVICE, 
                    "%!FUNC! Cannot get usb target factory %!HRESULT!",
                    hr
                    );        
    }

    if (SUCCEEDED(hr)) 
    {
        hr = pIUsbTargetFactory->CreateUsbTargetDevice(
                                                  &pIUsbTargetDevice);
        if (FAILED(hr))
        {
            TraceEvents(TRACE_LEVEL_ERROR, 
                        BIOMETRIC_TRACE_DEVICE, 
                        "%!FUNC! Unable to create USB Device I/O Target %!HRESULT!",
                        hr
                        );        
        }
        else
        {
            m_pIUsbTargetDevice = pIUsbTargetDevice;

            //
            // Release the creation reference as object tree will maintain a reference
            //
            
            pIUsbTargetDevice->Release();            
        }
    }

    if (SUCCEEDED(hr)) 
    {
        UCHAR NumInterfaces = pIUsbTargetDevice->GetNumInterfaces();

        WUDF_TEST_DRIVER_ASSERT(1 == NumInterfaces);
        
        hr = pIUsbTargetDevice->RetrieveUsbInterface(0, &pIUsbInterface);
        if (FAILED(hr))
        {
            TraceEvents(TRACE_LEVEL_ERROR, 
                        BIOMETRIC_TRACE_DEVICE, 
                        "%!FUNC! Unable to retrieve USB interface from USB Device I/O Target %!HRESULT!",
                        hr
                        );        
        }
        else
        {
            m_pIUsbInterface = pIUsbInterface;

            pIUsbInterface->Release(); //release creation reference                        
        }
    }

    if (SUCCEEDED(hr)) 
    {
        NumEndPoints = pIUsbInterface->GetNumEndPoints();

        if (NumEndPoints != NUM_OSRUSB_ENDPOINTS) {
            hr = E_UNEXPECTED;
            TraceEvents(TRACE_LEVEL_ERROR, 
                        BIOMETRIC_TRACE_DEVICE, 
                        "%!FUNC! Has %d endpoints, expected %d, returning %!HRESULT! ", 
                        NumEndPoints,
                        NUM_OSRUSB_ENDPOINTS,
                        hr
                        );
        }
    }

    if (SUCCEEDED(hr)) 
    {
        for (UCHAR PipeIndex = 0; PipeIndex < NumEndPoints; PipeIndex++)
        {
            hr = pIUsbInterface->RetrieveUsbPipeObject(PipeIndex, 
                                                  &pIUsbPipe);

            if (FAILED(hr))
            {
                TraceEvents(TRACE_LEVEL_ERROR, 
                            BIOMETRIC_TRACE_DEVICE, 
                            "%!FUNC! Unable to retrieve USB Pipe for PipeIndex %d, %!HRESULT!",
                            PipeIndex,
                            hr
                            );        
            }
            else
            {
                if ( pIUsbPipe->IsInEndPoint() )
                {
                    if ( UsbdPipeTypeInterrupt == pIUsbPipe->GetType() )
                    {
                        m_pIUsbInterruptPipe = pIUsbPipe;
                    }
                    else if ( UsbdPipeTypeBulk == pIUsbPipe->GetType() )
                    {
                        m_pIUsbInputPipe = pIUsbPipe;
                    }
                    else
                    {
                        pIUsbPipe->DeleteWdfObject();
                    }                      
                }
                else if ( pIUsbPipe->IsOutEndPoint() && (UsbdPipeTypeBulk == pIUsbPipe->GetType()) )
                {
                    m_pIUsbOutputPipe = pIUsbPipe;
                }
                else
                {
                    pIUsbPipe->DeleteWdfObject();
                }
    
                BiometricSafeRelease(pIUsbPipe); //release creation reference
            }
        }

        if (NULL == m_pIUsbInputPipe || NULL == m_pIUsbOutputPipe)
        {
            hr = E_UNEXPECTED;
            TraceEvents(TRACE_LEVEL_ERROR, 
                        BIOMETRIC_TRACE_DEVICE, 
                        "%!FUNC! Input or output pipe not found, returning %!HRESULT!",
                        hr
                        );        
        }
    }

    BiometricSafeRelease(pIUsbTargetFactory);

    return hr;
}

HRESULT
CBiometricDevice::ConfigureUsbPipes(
    )
/*++

Routine Description:

    This routine retrieves the IDs for the bulk end points of the USB device.

Arguments:

    None

Return Value:

    HRESULT
--*/
{
    HRESULT                 hr = S_OK;
    LONG                    timeout;

    //
    // Set timeout policies for input/output pipes
    //

    if (SUCCEEDED(hr)) 
    {
        timeout = ENDPOINT_TIMEOUT;

        hr = m_pIUsbInputPipe->SetPipePolicy(PIPE_TRANSFER_TIMEOUT,
                                             sizeof(timeout),
                                             &timeout);
        if (FAILED(hr))
        {
            TraceEvents(TRACE_LEVEL_ERROR, 
                        BIOMETRIC_TRACE_DEVICE, 
                        "%!FUNC! Unable to set timeout policy for input pipe %!HRESULT!",
                        hr
                        );
        }
    }
        
    if (SUCCEEDED(hr)) 
    {
        timeout = ENDPOINT_TIMEOUT;

        hr = m_pIUsbOutputPipe->SetPipePolicy(PIPE_TRANSFER_TIMEOUT,
                                             sizeof(timeout),
                                             &timeout);
        if (FAILED(hr)) 
        {
            TraceEvents(TRACE_LEVEL_ERROR, 
                        BIOMETRIC_TRACE_DEVICE, 
                        "%!FUNC! Unable to set timeout policy for output pipe %!HRESULT!",
                        hr
                        );
        }
    }

    return hr;
}


HRESULT
CBiometricDevice::SetPowerManagement(
    VOID
    )
/*++

  Routine Description:

    This method enables the WinUSB driver to power the device down when it is
    idle.

  Arguments:

    None

  Return Value:
    
    Status

--*/
{
    ULONG value;

    HRESULT hr;

    //
    // First set the suspend delay to 10 seconds.
    //

    value = 10 * 1000;

    hr = m_pIUsbTargetDevice->SetPowerPolicy( SUSPEND_DELAY,
                                              sizeof(ULONG),
                                              (PVOID) &value );                                         

    if (FAILED(hr))
    {
        TraceEvents(TRACE_LEVEL_ERROR, 
                    BIOMETRIC_TRACE_DEVICE, 
                    "%!FUNC! Unable to set power policy (SUSPEND_DELAY) for the device %!HRESULT!",
                    hr
                    );
    }


    //
    // Finally enable auto-suspend.
    //

    if (SUCCEEDED(hr))
    {
        BOOL AutoSuspsend = TRUE;
    
        hr = m_pIUsbTargetDevice->SetPowerPolicy( AUTO_SUSPEND,
                                                  sizeof(BOOL),
                                                  (PVOID) &AutoSuspsend );     
    }

    if (FAILED(hr))
    {
        TraceEvents(TRACE_LEVEL_ERROR, 
                    BIOMETRIC_TRACE_DEVICE, 
                    "%!FUNC! Unable to set power policy (AUTO_SUSPEND) for the device %!HRESULT!",
                    hr
                    );
    }

    return hr;
}

//
// FX2 specific routines
//

HRESULT
CBiometricDevice::IndicateDeviceReady(
    VOID
    )
/*++
 
  Routine Description:

    This method lights the period on the device's seven-segment display to 
    indicate that the driver's PrepareHardware method has completed.

  Arguments:

    None

  Return Value:

    Status

--*/
{
    SEVEN_SEGMENT display = {0};

    HRESULT hr;

    //
    // First read the contents of the seven segment display.
    //

    hr = GetSevenSegmentDisplay(&display);

    if (SUCCEEDED(hr)) 
    {
        display.Segments |= 0x08;

        hr = SetSevenSegmentDisplay(&display);
    }

    return hr;
}

HRESULT
CBiometricDevice::GetSevenSegmentDisplay(
    __in PSEVEN_SEGMENT SevenSegment
    )
/*++
 
  Routine Description:

    This method synchronously retrieves the bar graph display information 
    from the OSR USB-FX2 device.  It uses the buffers in the FxRequest
    to hold the data it retrieves.

  Arguments:

    FxRequest - the request for the bar-graph info.

  Return Value:

    Status

--*/
{
    WINUSB_CONTROL_SETUP_PACKET setupPacket;

    ULONG bytesReturned;

    HRESULT hr = S_OK;

    //
    // Zero the output buffer - the device will or in the bits for 
    // the lights that are set.
    //

    SevenSegment->Segments = 0;

    //
    // Setup the control packet.
    //

    WINUSB_CONTROL_SETUP_PACKET_INIT( &setupPacket,
                                      BmRequestDeviceToHost,
                                      BmRequestToDevice,
                                      USBFX2LK_READ_7SEGMENT_DISPLAY,
                                      0,
                                      0 );

    //
    // Issue the request to WinUsb.
    //

    hr = SendControlTransferSynchronously(
                &(setupPacket.WinUsb),
                (PUCHAR) SevenSegment,
                sizeof(SEVEN_SEGMENT),
                &bytesReturned
                );

    return hr;
}

HRESULT
CBiometricDevice::SetSevenSegmentDisplay(
    __in PSEVEN_SEGMENT SevenSegment
    )
/*++
 
  Routine Description:

    This method synchronously sets the bar graph display on the OSR USB-FX2 
    device using the buffers in the FxRequest as input.

  Arguments:

    FxRequest - the request to set the bar-graph info.

  Return Value:

    Status

--*/
{
    WINUSB_CONTROL_SETUP_PACKET setupPacket;

    ULONG bytesTransferred;

    HRESULT hr = S_OK;

    //
    // Setup the control packet.
    //

    WINUSB_CONTROL_SETUP_PACKET_INIT( &setupPacket,
                                      BmRequestHostToDevice,
                                      BmRequestToDevice,
                                      USBFX2LK_SET_7SEGMENT_DISPLAY,
                                      0,
                                      0 );

    //
    // Issue the request to WinUsb.
    //

    hr = SendControlTransferSynchronously(
                &(setupPacket.WinUsb),
                (PUCHAR) SevenSegment,
                sizeof(SEVEN_SEGMENT),
                &bytesTransferred
                );

    return hr;
}

HRESULT
CBiometricDevice::SendControlTransferSynchronously(
    __in PWINUSB_SETUP_PACKET SetupPacket,
    __inout_ecount(BufferLength) PBYTE Buffer,
    __in ULONG BufferLength,
    __out PULONG LengthTransferred
    )
/*++
 
  Routine Description:

    This method synchronously sends a control transfer request to
    the USB I/O target.

  Arguments:

    SetupPacket - The command parameter structure

    Buffer - The data to transfer

    BufferLength - The size of the data buffer to transfer

    LengthTransferred - Contains the actual number of bytes transferred.

  Return Value:

    HRESULT

--*/
{
    HRESULT hr = S_OK;
    IWDFIoRequest *pWdfRequest = NULL;
    IWDFDriver * FxDriver = NULL;
    IWDFMemory * FxMemory = NULL; 
    IWDFRequestCompletionParams * FxComplParams = NULL;
    IWDFUsbRequestCompletionParams * FxUsbComplParams = NULL;

    *LengthTransferred = 0;
    
    hr = m_FxDevice->CreateRequest( NULL, //pCallbackInterface
                                    NULL, //pParentObject
                                    &pWdfRequest);

    if (SUCCEEDED(hr))
    {
        m_FxDevice->GetDriver(&FxDriver);

        hr = FxDriver->CreatePreallocatedWdfMemory( Buffer,
                                                    BufferLength,
                                                    NULL, //pCallbackInterface
                                                    pWdfRequest, //pParetObject
                                                    &FxMemory );
    }

    if (SUCCEEDED(hr))
    {
        hr = m_pIUsbTargetDevice->FormatRequestForControlTransfer( pWdfRequest,
                                                                   SetupPacket,
                                                                   FxMemory,
                                                                   NULL); //TransferOffset
    }                                                          
                        
    if (SUCCEEDED(hr))
    {
        hr = pWdfRequest->Send( m_pIUsbTargetDevice,
                                WDF_REQUEST_SEND_OPTION_SYNCHRONOUS,
                                0); //Timeout
    }

    if (SUCCEEDED(hr))
    {
        pWdfRequest->GetCompletionParams(&FxComplParams);

        hr = FxComplParams->GetCompletionStatus();
    }

    if (SUCCEEDED(hr))
    {
        HRESULT hrQI = FxComplParams->QueryInterface(IID_PPV_ARGS(&FxUsbComplParams));
        WUDF_TEST_DRIVER_ASSERT(SUCCEEDED(hrQI));

        WUDF_TEST_DRIVER_ASSERT( WdfUsbRequestTypeDeviceControlTransfer == 
                            FxUsbComplParams->GetCompletedUsbRequestType() );

        FxUsbComplParams->GetDeviceControlTransferParameters( NULL,
                                                             LengthTransferred,
                                                             NULL,
                                                             NULL );
    }

    BiometricSafeRelease(FxUsbComplParams);
    BiometricSafeRelease(FxComplParams);
    BiometricSafeRelease(FxMemory);

    pWdfRequest->DeleteWdfObject();        
    BiometricSafeRelease(pWdfRequest);

    BiometricSafeRelease(FxDriver);

    return hr;
}

WDF_IO_TARGET_STATE
CBiometricDevice::GetTargetState(
    IWDFIoTarget * pTarget
    )
/*++
 
  Routine Description:

    This method gets the state of the I/O target

  Arguments:

    pTarget - A pointer to the I/O target

  Return Value:

    WDF_IO_TARGET_STATE

--*/
{
    IWDFIoTargetStateManagement * pStateMgmt = NULL;
    WDF_IO_TARGET_STATE state;

    HRESULT hrQI = pTarget->QueryInterface(IID_PPV_ARGS(&pStateMgmt));
    WUDF_TEST_DRIVER_ASSERT((SUCCEEDED(hrQI) && pStateMgmt));
    
    state = pStateMgmt->GetState();

    BiometricSafeRelease(pStateMgmt);
    
    return state;
}

HRESULT
CBiometricDevice::InitiatePendingRead(
    VOID
    )
/*++
 
  Routine Description:

    This routine starts up a cycling read on the interrupt pipe.  As each 
    read completes it will start up the next one.

  Arguments:
    
    None

  Return Value:

    Status

--*/
{
    HRESULT hr = S_OK;
    IWDFIoRequest * FxRequest = NULL;
    IWDFMemory * FxMemory = NULL;
    IWDFDriver * FxDriver = NULL;
    IRequestCallbackRequestCompletion * FxComplCallback = NULL;

    hr = m_FxDevice->CreateRequest(NULL, NULL, &FxRequest);

    if (SUCCEEDED(hr))
    {
        m_FxDevice->GetDriver(&FxDriver);
        
        hr = FxDriver->CreatePreallocatedWdfMemory( (PBYTE) &m_SwitchStateBuffer,
                                                    sizeof(m_SwitchStateBuffer),
                                                    NULL, //pCallbackInterface
                                                    FxRequest, //pParetObject
                                                    &FxMemory );        
    }

    if (SUCCEEDED(hr))
    {
        hr = m_pIUsbInterruptPipe->FormatRequestForRead(FxRequest,
                                                        NULL, //pFile - IoTarget would apply its file
                                                        FxMemory, 
                                                        NULL, //Memory offset
                                                        NULL);  //Device offset                                                                                   
    }

    if (SUCCEEDED(hr))
    {
        HRESULT hrQI = this->QueryInterface(IID_PPV_ARGS(&FxComplCallback));
        WUDF_TEST_DRIVER_ASSERT((SUCCEEDED(hrQI) && FxComplCallback));
        
        FxRequest->SetCompletionCallback(FxComplCallback, NULL);

        hr = FxRequest->Send(m_pIUsbInterruptPipe, 0, 0);
    }

    if (FAILED(hr) && FxRequest)
    {
        m_InterruptReadProblem = hr;

        //
        // We won't get a completion, so satisfy app request with failure now
        //
        
        ServiceSwitchChangeQueue(m_SwitchState, 
                                 hr,
                                 NULL);
        
        FxRequest->DeleteWdfObject();
    }


    BiometricSafeRelease(FxRequest);
    BiometricSafeRelease(FxMemory);
    BiometricSafeRelease(FxDriver);
    BiometricSafeRelease(FxComplCallback);

    return hr;
}

VOID
CBiometricDevice::OnCompletion(
    IWDFIoRequest*                 FxRequest,
    IWDFIoTarget*                  pIoTarget,
    IWDFRequestCompletionParams*   pParams,
    PVOID                          pContext
    )
/*++
 
  Routine Description:

    This method is called when the asynchronous pending
    read on the interrupt pipe completes.

  Arguments:

    FxRequest - The request object

    pIoTarget - The I/O target for the request

    pParams - The completion parameters

    pContext - Optional context

  Return Value:

    None

--*/
{
    UNREFERENCED_PARAMETER(pIoTarget);
    UNREFERENCED_PARAMETER(pContext);

    IWDFUsbRequestCompletionParams * pUsbComplParams = NULL;
    IWDFMemory * FxMemory = NULL;
    SIZE_T bytesRead = 0;
    HRESULT hrCompletion = pParams->GetCompletionStatus();

    TraceEvents(TRACE_LEVEL_INFORMATION, 
                BIOMETRIC_TRACE_DEVICE, 
                "%!FUNC! Pending read completed with %!hresult!",
                hrCompletion
                );

    if (FAILED(hrCompletion))
    {
        m_InterruptReadProblem = hrCompletion;
    }
    else
    {
        //
        // Get the switch state
        //
        
        HRESULT hrQI = pParams->QueryInterface(IID_PPV_ARGS(&pUsbComplParams));
        WUDF_TEST_DRIVER_ASSERT( (SUCCEEDED(hrQI) && pUsbComplParams) );

        pUsbComplParams->GetPipeReadParameters(&FxMemory, &bytesRead, NULL);
        WUDF_TEST_DRIVER_ASSERT(bytesRead == sizeof(m_SwitchState));

        PVOID pBuff = FxMemory->GetDataBuffer(NULL);

        CopyMemory(&m_SwitchState, pBuff, sizeof(m_SwitchState));
    }
        
    FxRequest->DeleteWdfObject();

    //
    // Satisfy application request for switch change notification
    //
    
    ServiceSwitchChangeQueue(m_SwitchState, 
                             hrCompletion,
                             NULL);

    //
    // Re-initiate pending read if I/O Target is not stopped/removed
    //
    
    if (WdfIoTargetStarted == GetTargetState(m_pIUsbInterruptPipe))
    {
        InitiatePendingRead();
    }

    BiometricSafeRelease(pUsbComplParams);
    BiometricSafeRelease(FxMemory);
}

VOID
CBiometricDevice::ServiceSwitchChangeQueue(
    __in SWITCH_STATE NewState,
    __in HRESULT CompletionStatus,
    __in_opt IWDFFile *SpecificFile
    )
/*++
 
  Routine Description:

    This method processes switch-state change notification requests as 
    part of reading the OSR device's interrupt pipe.  As each read completes
    this pulls all pending I/O off the switch change queue and completes
    each request with the current switch state.

  Arguments:

    NewState - the state of the switches

    CompletionStatus - all pending operations are completed with this status.

    SpecificFile - if provided only requests for this file object will get
                   completed.

  Return Value:

    None

--*/
{
    IWDFIoRequest *fxRequest;

    HRESULT enumHr = S_OK;

    do 
    {
        HRESULT hr;

        //
        // Get the next request.
        //

        if (NULL != SpecificFile)
        {
            enumHr = m_SwitchChangeQueue->RetrieveNextRequestByFileObject(
                                            SpecificFile,
                                            &fxRequest
                                            );
        }
        else
        {
            enumHr = m_SwitchChangeQueue->RetrieveNextRequest(&fxRequest);
        }

        //
        // if we got one then complete it.
        //

        if (SUCCEEDED(enumHr)) 
        {
            if (SUCCEEDED(CompletionStatus)) 
            {
                IWDFMemory *fxMemory;

                //
                // First copy the result to the request buffer.
                //

                fxRequest->GetOutputMemory(&fxMemory );

                hr = fxMemory->CopyFromBuffer(0, 
                                              &NewState, 
                                              sizeof(SWITCH_STATE));
                fxMemory->Release();
            }
            else 
            {
                hr = CompletionStatus;
            }

            //
            // Complete the request with the status of the copy (or the completion
            // status if that was an error).
            //

            if (SUCCEEDED(hr)) 
            {
                fxRequest->CompleteWithInformation(hr, sizeof(SWITCH_STATE));
            }
            else
            {
                fxRequest->Complete(hr);
            }

            fxRequest->Release();            
        }
    }
    while (SUCCEEDED(enumHr));
}

//
// I/O handlers
//

void 
CBiometricDevice::GetIoRequestParams(
    __in IWDFIoRequest *FxRequest,
    __out ULONG *MajorControlCode,
    __deref_out_bcount(*InputBufferSizeInBytes) PUCHAR *InputBuffer,
    __out SIZE_T *InputBufferSizeInBytes,
    __deref_out_bcount(*OutputBufferSizeInBytes) PUCHAR *OutputBuffer,
    __out SIZE_T *OutputBufferSizeInBytes
    )
/*++
 
  Routine Description:

    This method retrieves the input and output buffers associated with the request.

  Arguments:

    FxRequest - The WDF request oject
    
    MajorControlCode - Contains the control code for the I/O request

    InputBuffer - Contains the input buffer pointer

    InputBufferSizeInBytes - Contains the size of the input buffer

    OutputBuffer - Contains the output buffer pointer

    OutputBufferSizeInBytes - Contains the size of the output buffer

  Return Value:

    None

--*/
{
    //
    // Get main parameters
    //
    FxRequest->GetDeviceIoControlParameters(MajorControlCode,
                                            InputBufferSizeInBytes,
                                            OutputBufferSizeInBytes);

    // Get pointer to input buffer
    IWDFMemory *fxMemory = NULL;
    FxRequest->GetInputMemory(&fxMemory);
    if (fxMemory) {
        *InputBuffer = (PUCHAR) fxMemory->GetDataBuffer(InputBufferSizeInBytes);
        BiometricSafeRelease(fxMemory);
    }

    // Save pointer to reply buffer
    fxMemory = NULL;
    FxRequest->GetOutputMemory(&fxMemory);
    if (fxMemory) {
        *OutputBuffer = (PUCHAR) fxMemory->GetDataBuffer(OutputBufferSizeInBytes);
        BiometricSafeRelease(fxMemory);
    }
}

void
CBiometricDevice::OnGetAttributes(
    __inout IWDFIoRequest *FxRequest
    )
/*++
 
  Routine Description:

    This method is invoked when the IOCTL_BIOMETRIC_GET_ATTRIBUTES command is called.

  Arguments:

    FxRequest - The output for this request is a PWINBIO_SENSOR_ATTRIBUTES.

  Return Value:

    None

--*/
{
    CRequestHelper MyRequest(FxRequest);  // RAII helper class
    ULONG controlCode = 0;
    PUCHAR inputBuffer= NULL;
    SIZE_T inputBufferSize = 0;
    PWINBIO_SENSOR_ATTRIBUTES sensorAttributes = NULL;
    SIZE_T outputBufferSize;

    //
    // Get the request parameters
    //
    GetIoRequestParams(FxRequest,
                      &controlCode,
                      &inputBuffer,
                      &inputBufferSize,
                      (PUCHAR *)&sensorAttributes,
                      &outputBufferSize);

    //
    // Make sure we have an output buffer big enough
    //
    if (sensorAttributes == NULL || outputBufferSize < sizeof(DWORD)) {
        // We cannot return size information.
        TraceEvents(TRACE_LEVEL_ERROR, 
                   BIOMETRIC_TRACE_DEVICE, 
                   "%!FUNC!Output buffer NULL or too small to return size information.");
        MyRequest.SetCompletionHr(E_INVALIDARG);
        return;
    }

    // We only have one supported format, so sizeof (WINBIO_SENSOR_ATTRIBUTES) is sufficient.
    if (outputBufferSize < sizeof(WINBIO_SENSOR_ATTRIBUTES)) {
        // Buffer too small.
        TraceEvents(TRACE_LEVEL_ERROR, 
                   BIOMETRIC_TRACE_DEVICE, 
                   "%!FUNC!Buffer too small - return size necessary in PayloadSize - 0x%x.", sizeof(WINBIO_SENSOR_ATTRIBUTES));
        sensorAttributes->PayloadSize = (DWORD) sizeof(WINBIO_SENSOR_ATTRIBUTES);
        MyRequest.SetInformation(sizeof(DWORD));
        MyRequest.SetCompletionHr(S_OK);
        return;
    }

    //
    // Fill in the attribute payload structure
    //
    RtlZeroMemory(sensorAttributes, outputBufferSize);
    sensorAttributes->PayloadSize = (DWORD) sizeof(WINBIO_SENSOR_ATTRIBUTES);
    sensorAttributes->WinBioHresult = S_OK;
    sensorAttributes->WinBioVersion.MajorVersion = WINBIO_WBDI_MAJOR_VERSION;
    sensorAttributes->WinBioVersion.MinorVersion = WINBIO_WBDI_MINOR_VERSION;
    sensorAttributes->SensorType = WINBIO_TYPE_FINGERPRINT;
    sensorAttributes->SensorSubType = WINBIO_FP_SENSOR_SUBTYPE_SWIPE;
    sensorAttributes->Capabilities = WINBIO_CAPABILITY_SENSOR;
    sensorAttributes->SupportedFormatEntries = 1;
    sensorAttributes->SupportedFormat[0].Owner = WINBIO_ANSI_381_FORMAT_OWNER;
    sensorAttributes->SupportedFormat[0].Type= WINBIO_ANSI_381_FORMAT_TYPE;
    RtlCopyMemory(sensorAttributes->ManufacturerName, SAMPLE_MANUFACTURER_NAME, (wcslen(SAMPLE_MANUFACTURER_NAME)+1)*sizeof(WCHAR));
    RtlCopyMemory(sensorAttributes->ModelName, SAMPLE_MODEL_NAME, (wcslen(SAMPLE_MODEL_NAME)+1)*sizeof(WCHAR));
    RtlCopyMemory(sensorAttributes->SerialNumber, SAMPLE_SERIAL_NUMBER, (wcslen(SAMPLE_SERIAL_NUMBER)+1)*sizeof(WCHAR));
    sensorAttributes->FirmwareVersion.MajorVersion = 1;
    sensorAttributes->FirmwareVersion.MinorVersion = 0;

    MyRequest.SetInformation(sensorAttributes->PayloadSize);
    MyRequest.SetCompletionHr(S_OK);
}


void
CBiometricDevice::OnReset(
    __inout IWDFIoRequest *FxRequest
    )
/*++
 
  Routine Description:

    This method is invoked when the IOCTL_BIOMETRIC_RESET command is called.

  Arguments:

    FxRequest - 

  Return Value:

    None

--*/
{
    CRequestHelper MyRequest(FxRequest);  // RAII helper class
    ULONG controlCode = 0;
    PUCHAR inputBuffer= NULL;
    SIZE_T inputBufferSize = 0;
    PWINBIO_BLANK_PAYLOAD blankPayload = NULL;
    SIZE_T outputBufferSize;

    //
    // Get the request parameters
    //
    GetIoRequestParams(FxRequest,
                      &controlCode,
                      &inputBuffer,
                      &inputBufferSize,
                      (PUCHAR *)&blankPayload,
                      &outputBufferSize);

    //
    // Make sure we have an output buffer big enough
    //
    if (blankPayload== NULL || outputBufferSize < sizeof(DWORD)) {
        // We cannot return size information.
        TraceEvents(TRACE_LEVEL_ERROR, 
                   BIOMETRIC_TRACE_DEVICE, 
                   "%!FUNC!Output buffer NULL or too small to return size information.");
        MyRequest.SetInformation(sizeof(DWORD));
        MyRequest.SetCompletionHr(S_OK);
        MyRequest.SetCompletionHr(E_INVALIDARG);
        return;
    }

    if (outputBufferSize < sizeof(WINBIO_BLANK_PAYLOAD)) {
        // Buffer too small.
        TraceEvents(TRACE_LEVEL_ERROR, 
                   BIOMETRIC_TRACE_DEVICE, 
                   "%!FUNC!Buffer too small - return size necessary in PayloadSize - 0x%x.", sizeof(WINBIO_DIAGNOSTICS));
        MyRequest.SetInformation(sizeof(DWORD));
        MyRequest.SetCompletionHr(S_OK);
        return;
    }

    //
    // This is a simulated device.  Nothing to do here except cancel the pending data
    // collection I/O, if one exists.
    //
    CompletePendingRequest(HRESULT_FROM_WIN32(ERROR_CANCELLED), 0);

    //
    // Fill in the OUT payload structure
    //
    RtlZeroMemory(blankPayload, outputBufferSize);
    blankPayload->PayloadSize = (DWORD) sizeof(WINBIO_BLANK_PAYLOAD);
    blankPayload->WinBioHresult = S_OK;

    FxRequest->SetInformation(blankPayload->PayloadSize);
    MyRequest.SetCompletionHr(S_OK);

}

void
CBiometricDevice::OnCalibrate(
    __inout IWDFIoRequest *FxRequest
    )
/*++
 
  Routine Description:

    This method is invoked when the IOCTL_BIOMETRIC_CALIBRATE command is called.

  Arguments:

    FxRequest -
        IN - blank payload
        OUT - PWINBIO_CALIBRATION_INFO

  Return Value:

    None

--*/
{
    CRequestHelper MyRequest(FxRequest);  // RAII helper class
    ULONG controlCode = 0;
    PUCHAR inputBuffer= NULL;
    SIZE_T inputBufferSize = 0;
    PWINBIO_CALIBRATION_INFO calibrationInfo = NULL;
    SIZE_T outputBufferSize;

    //
    // Get the request parameters
    //
    GetIoRequestParams(FxRequest,
                      &controlCode,
                      &inputBuffer,
                      &inputBufferSize,
                      (PUCHAR *)&calibrationInfo,
                      &outputBufferSize);

    //
    // Make sure we have an output buffer big enough
    //
    if (calibrationInfo == NULL || outputBufferSize < sizeof(DWORD)) {
        // We cannot return size information.
        TraceEvents(TRACE_LEVEL_ERROR, 
                   BIOMETRIC_TRACE_DEVICE, 
                   "%!FUNC!Output buffer NULL or too small to return size information.");
        MyRequest.SetCompletionHr(E_INVALIDARG);
        return;
    }

    if (outputBufferSize < sizeof(WINBIO_CALIBRATION_INFO)) {
        // Buffer too small.
        TraceEvents(TRACE_LEVEL_ERROR, 
                   BIOMETRIC_TRACE_DEVICE, 
                   "%!FUNC!Buffer too small - return size necessary in PayloadSize - 0x%x.", sizeof(WINBIO_DIAGNOSTICS));
        calibrationInfo->PayloadSize = (DWORD) sizeof(WINBIO_CALIBRATION_INFO);
        MyRequest.SetInformation(sizeof(DWORD));
        MyRequest.SetCompletionHr(S_OK);
        return;
    }

    //
    // This is where code to calibrate the device goes.
    //

    //
    // Fill in the OUT payload structure
    //
    RtlZeroMemory(calibrationInfo, outputBufferSize);
    calibrationInfo->PayloadSize = (DWORD) sizeof(WINBIO_CALIBRATION_INFO);
    calibrationInfo->WinBioHresult = S_OK;

    MyRequest.SetInformation(calibrationInfo->PayloadSize);
    MyRequest.SetCompletionHr(S_OK);
}


void
CBiometricDevice::OnGetSensorStatus(
    __inout IWDFIoRequest *FxRequest
    )
/*++
 
  Routine Description:

    This method is invoked when the IOCTL_BIOMETRIC_GET_SENSOR_STATUS command is called.

  Arguments:

    FxRequest -
        IN payload: none
        OUT payload: PWINBIO_DIAGNOSTICS

  Return Value:

    None

--*/
{
    CRequestHelper MyRequest(FxRequest);  // RAII helper class
    ULONG controlCode = 0;
    PUCHAR inputBuffer= NULL;
    SIZE_T inputBufferSize = 0;
    PWINBIO_DIAGNOSTICS diagnostics = NULL;
    SIZE_T outputBufferSize;

    //
    // Get the request parameters
    //
    GetIoRequestParams(FxRequest,
                      &controlCode,
                      &inputBuffer,
                      &inputBufferSize,
                      (PUCHAR *)&diagnostics,
                      &outputBufferSize);

    //
    // Make sure we have an output buffer big enough
    //
    if (diagnostics == NULL || outputBufferSize < sizeof(DWORD)) {
        // We cannot return size information.
        TraceEvents(TRACE_LEVEL_ERROR, 
                   BIOMETRIC_TRACE_DEVICE, 
                   "%!FUNC!Output buffer NULL or too small to return size information.");
        MyRequest.SetCompletionHr(E_INVALIDARG);
        return;
    }

    if (outputBufferSize < sizeof(WINBIO_DIAGNOSTICS)) {
        // Buffer too small.
        TraceEvents(TRACE_LEVEL_ERROR, 
                   BIOMETRIC_TRACE_DEVICE, 
                   "%!FUNC!Buffer too small - return size necessary in PayloadSize - 0x%x.", sizeof(WINBIO_DIAGNOSTICS));
        diagnostics->PayloadSize = (DWORD) sizeof(WINBIO_DIAGNOSTICS);
        MyRequest.SetInformation(sizeof(DWORD));
        MyRequest.SetCompletionHr(S_OK);
        return;
    }

    //
    // Fill in the OUT payload structure
    //
    RtlZeroMemory(diagnostics, outputBufferSize);
    diagnostics->PayloadSize = (DWORD) sizeof(WINBIO_DIAGNOSTICS);
    diagnostics->WinBioHresult = S_OK;
    diagnostics->SensorStatus = WINBIO_SENSOR_READY;

    MyRequest.SetInformation(diagnostics->PayloadSize);
    MyRequest.SetCompletionHr(S_OK);
}


void
CBiometricDevice::OnCaptureData(
    __inout IWDFIoRequest *FxRequest
    )
/*++
 
  Routine Description:

    This method is invoked when the IOCTL_BIOMETRIC_CAPTURE_DATA command is called.

  Arguments:

    FxRequest -
        IN payload: PWINBIO_CAPTURE_PARAMETERS
        OUT payload: PWINBIO_CAPTURE_DATA

  Return Value:

    None

--*/
{
    ULONG controlCode = 0;
    PWINBIO_CAPTURE_PARAMETERS captureParams = NULL;
    SIZE_T inputBufferSize = 0;
    PWINBIO_CAPTURE_DATA captureData = NULL;
    SIZE_T outputBufferSize = 0;

    //
    // We can only have one outstanding data capture request at a time.
    // Check to see if we have a request pending.
    //

    if (m_PendingRequest == NULL) {

        //
        // See if we have an active sleep thread
        // Wait for it to exit.
        // NOTE - real driver would signal thread to exit.
        //
        if (m_SleepThread != INVALID_HANDLE_VALUE)
        {
            WaitForSingleObject(m_SleepThread, INFINITE);
            CloseHandle(m_SleepThread);
            m_SleepThread = INVALID_HANDLE_VALUE;
        }

        // Mark the request as cancellable.
        FxRequest->MarkCancelable(this);

        // Save the request.
        (void) InterlockedExchangePointer((PVOID *)&m_PendingRequest, (PVOID)FxRequest);

    } else {

        // Complete the request to tell the app that there is already
        // a pending data collection request.
        FxRequest->Complete(WINBIO_E_DATA_COLLECTION_IN_PROGRESS);
        return;

    }

    //
    // Get the request parameters
    //
    GetIoRequestParams(FxRequest,
                       &controlCode,
                       (PUCHAR *)&captureParams,
                       &inputBufferSize,
                       (PUCHAR *)&captureData,
                       &outputBufferSize);

    //
    // Check input parameters.
    //
     if (inputBufferSize < sizeof (WINBIO_CAPTURE_PARAMETERS)) {
        // Invalid arguments
        TraceEvents(TRACE_LEVEL_ERROR, 
                   BIOMETRIC_TRACE_DEVICE, 
                   "%!FUNC!Invalid argument(s).");
        CompletePendingRequest(E_INVALIDARG, 0);
        return;
    }

    //
    // Make sure we have an output buffer big enough
    //
    if (outputBufferSize < sizeof(DWORD)) {
        // We cannot return size information.
        TraceEvents(TRACE_LEVEL_ERROR, 
                   BIOMETRIC_TRACE_DEVICE, 
                   "%!FUNC!Output buffer NULL or too small to return size information.");
        CompletePendingRequest(E_INVALIDARG, 0);
        return;
    }

    //
    // Check output buffer size.
    //
    if (outputBufferSize < sizeof (WINBIO_CAPTURE_DATA)) {
        // Buffer too small.
        TraceEvents(TRACE_LEVEL_ERROR, 
                   BIOMETRIC_TRACE_DEVICE, 
                   "%!FUNC!Buffer too small - must be at least 0x%x.", sizeof (WINBIO_CAPTURE_DATA));
        //
        // NOTE:  The output buffer size necessary for this sample is sizeof(WINBIO_CAPTURE_DATA).
        // Real devices will need additional space to handle a typical capture.
        // The value that should be returned here is sizeof(WINBIO_CAPTURE_DATA) + CaptureBufferSize.
        //
        captureData->PayloadSize = (DWORD) sizeof(WINBIO_CAPTURE_DATA);
        CompletePendingRequest(S_OK, sizeof(DWORD));
        return;
    }

    //
    // NOTE:  This call always fails in this sample since it is not
    // written for a real device.
    //

    //
    // Set default values in output buffer.
    //
    captureData->PayloadSize = (DWORD) sizeof (WINBIO_CAPTURE_DATA);
    captureData->WinBioHresult = WINBIO_E_NO_CAPTURE_DATA;
    captureData->SensorStatus = WINBIO_SENSOR_FAILURE;
    captureData->RejectDetail= 0;
    captureData->CaptureData.Size = 0;

    //
    // Check purpose, format and type.
    //
    if (captureParams->Purpose == WINBIO_NO_PURPOSE_AVAILABLE)
    {
        captureData->WinBioHresult = WINBIO_E_UNSUPPORTED_PURPOSE;
    }
    else if ((captureParams->Format.Type != WINBIO_ANSI_381_FORMAT_TYPE) ||
             (captureParams->Format.Owner != WINBIO_ANSI_381_FORMAT_OWNER))
    {
        captureData->WinBioHresult = WINBIO_E_UNSUPPORTED_DATA_FORMAT;
    }
    else if (captureParams->Flags != WINBIO_DATA_FLAG_RAW)
    {
        captureData->WinBioHresult = WINBIO_E_UNSUPPORTED_DATA_TYPE;
    }

    //
    // NOTE:  This sample completes the request after
    // sleeping for 5 seconds.  A real driver would 
    // program the device fo capture mode, and then
    // return from this callback.  The request would
    // remain pending until cancelled, or until the
    // driver detects a capture is complete.
    //
    // The construct of m_PendingRequest will allow
    // a driver to have only one pending request at any
    // time, which can be cancelled in a Reset IOCTL, or
    // by calling CancelIoEx.
    //

    //
    // Create thread to sleep 5 seconds before completing the request.
    //
    m_SleepParams.SleepValue = 5;
    m_SleepParams.Hr = S_OK;
    m_SleepParams.Information = captureData->PayloadSize;
    m_SleepThread = CreateThread(NULL,                   // default security attributes
                                 0,                      // use default stack size  
                                 CaptureSleepThread,     // thread function name
                                 this,                   // argument to thread function 
                                 0,                      // use default creation flags 
                                 NULL);                  // returns the thread identifier 
}


void
CBiometricDevice::OnUpdateFirmware(
    __inout IWDFIoRequest *FxRequest
    )
/*++
 
  Routine Description:

    This method is invoked when the IOCTL_BIOMETRIC_UPDATE_FIRMWARE command is called.

  Arguments:

    FxRequest - 

  Return Value:

    None

--*/
{
    FxRequest->Complete(E_NOTIMPL);
}

void
CBiometricDevice::OnGetSupportedAlgorithms(
    __inout IWDFIoRequest *FxRequest
    )
/*++
 
  Routine Description:

    This method is invoked when the IOCTL_BIOMETRIC_GET_SUPPORTED_ALGORITHMS command is called.

  Arguments:

    FxRequest - 

  Return Value:

    None

--*/
{
    FxRequest->Complete(E_NOTIMPL);
}

void
CBiometricDevice::OnGetIndicator(
    __inout IWDFIoRequest *FxRequest
    )
/*++
 
  Routine Description:

    This method is invoked when the IOCTL_BIOMETRIC_GET_INDICATOR command is called.

  Arguments:

    FxRequest - 

  Return Value:

    None

--*/
{
    FxRequest->Complete(E_NOTIMPL);
}


void
CBiometricDevice::OnSetIndicator(
    __inout IWDFIoRequest *FxRequest
    )
/*++
 
  Routine Description:

    This method is invoked when the IOCTL_BIOMETRIC_SET_INDICATOR command is called.

  Arguments:

    FxRequest - 

  Return Value:

    None

--*/
{
    FxRequest->Complete(E_NOTIMPL);
}

void
CBiometricDevice::OnControlUnit(
    __inout IWDFIoRequest *FxRequest
    )
/*++
 
  Routine Description:

    This method is invoked when the IOCTL_BIOMETRIC_CONTROL_UNIT command is called.

  Arguments:

    FxRequest - 

  Return Value:

    None

--*/
{
    FxRequest->Complete(E_NOTIMPL);
}


VOID
CBiometricDevice::CompletePendingRequest(
    HRESULT hr,
    DWORD   information
    )
{
    IWDFIoRequest *FxRequest = (IWDFIoRequest *)InterlockedExchangePointer((PVOID *)&m_PendingRequest, NULL);

    if (FxRequest) {
        FxRequest->UnmarkCancelable();
        FxRequest->SetInformation(information);
        FxRequest->Complete(hr);
    }
}

VOID
STDMETHODCALLTYPE
CBiometricDevice::OnCancel(
    __in IWDFIoRequest *pWdfRequest
    )
{
    IWDFIoRequest *FxRequest = (IWDFIoRequest *)InterlockedExchangePointer((PVOID *)&m_PendingRequest, NULL);

    if (FxRequest != pWdfRequest) {
        TraceEvents(TRACE_LEVEL_ERROR, 
                    BIOMETRIC_TRACE_DEVICE, 
                    "%!FUNC! Cancelled request does not match pending request.");
    }

    pWdfRequest->Complete(HRESULT_FROM_WIN32(ERROR_OPERATION_ABORTED));

}

