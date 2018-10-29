/*++ BUILD Version 0000

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    HIDDevice.cpp

Abstract:

    Implements the interface IHIDDevice and configures the SoftHID 
    device to be a valid USB device.  The device only processes CMDs
    via its control endpoint and sends reports out through its 
    IN endpoint.  It does not process any output or feature reports.

--*/

#include "stdafx.h"
#include <stdlib.h>
#include <time.h>
#include "HIDDevice.h"
#include "SoftHIDReceiver_i.c"


// These are the indices of the string descriptors. They are used both
// as the indices of the strings with SoftUSBDevice.Strings and as the
// string descriptor index property values on the various objects (e.g.
// SoftUSBDevice.Manufacturer = STRING_IDX_MANUFACTURER).

#define STRING_IDX_MANUFACTURER     1
#define STRING_IDX_PRODUCT_DESC     2
#define STRING_IDX_SERIAL_NUM       3
#define STRING_IDX_CONFIG           4
#define STRING_IDX_INTERFACE        5

CHIDDevice::CHIDDevice()
/*++
Routine Description:
    Constructor for CHIDDevice. 

Arguments:
    None

Return value:
    None
--*/
{
    InitMemberVariables();
}

CHIDDevice::~CHIDDevice()
/*++
Routine Description:
    Destructor for CHIDDevice. 

Arguments:
    None

Return value:
    None
--*/
{
    InitMemberVariables();
}

void CHIDDevice::InitMemberVariables()
/*++
Routine Description:
   Initialize member variables for the class CHIDDevice  

Arguments:
    None

Return value:
    None
--*/
{
    //
    // Initialize the generic parameters for this module
    //
    m_piSoftUSBDevice         = NULL;
    m_piINEndpoint            = NULL;
    m_piEndpoint0             = NULL;
    m_piCntrlConnectionPoint  = NULL;
    m_psaHIDDescriptor        = NULL;
    m_psaReportDescriptor     = NULL;
    m_dwCntrlConnectionCookie = 0;
    m_SerialNumber            = 0;
    m_VendorID                = 0x045E;  // MS VendorID
    m_ProductID               = 0x0042;  // A Random ProductID (does not map to any specific device)
    m_DeviceID                = 0x4200;  // A Random DeviceID  (does not map to any specific device)
    m_Logging                 = FALSE;   // Turn off logging by default
    m_ProcessInputReports     = FALSE;   // By default, don't process reports
    m_IdleRate                = 10;      // Set the intial Idle Rate of the Input Thread Proc to 10ms
    m_ThreadHandle            = NULL;
    m_InputReportThreadID     = 0;

}

HRESULT CHIDDevice::FinalConstruct()
/*++
Routine Description:
   Perform tasks which may fail when the class CHIDDevice
   is finally constructed. This involves creating the USB device 
   object and initializing the device so that it is recognized
   as a valid USB device by the controller

Arguments:
    None

Return value:
    From called functions.
--*/
{
    HRESULT hr = S_OK;
    BOOLEAN result = TRUE;

    result = m_InputReportQueue.resize(64);
    if(!result)
    {
        hr = E_FAIL;
        goto Exit;
    }

    hr = CreateUSBDevice();
    IfFailHrGo(hr);
    
Exit:
    return hr;
}

void CHIDDevice::FinalRelease()
/*++
Routine Description:
   This is the last method called before the object is destroyed.
   It is an opportunity to do cleanup.

Arguments:
    None

Return value:
    None
--*/
{
    // FinalRelease is never called because reference is cleaned up in Destroy
}

STDMETHODIMP CHIDDevice::Destroy()
/*++
Routine Description:
   This is the method that is called to cleanup the object and the resources it has acquired.

Arguments:
    None

Return value:
    S_OK
--*/
{
    // This func stops the thread that handles input and frees its resources
    StopProcessing();
    
    //Release the connection point
    ReleaseControlConnectionPoint();
    
    //Release any interface which the device is holding
    RELEASE(m_piINEndpoint);
    RELEASE(m_piEndpoint0);
    
    if (NULL != m_psaHIDDescriptor)
    {
        SafeArrayDestroy(m_psaHIDDescriptor);
        m_psaHIDDescriptor = NULL;
    }
    if (NULL != m_psaReportDescriptor)
    {
        SafeArrayDestroy(m_psaReportDescriptor);
        m_psaReportDescriptor = NULL;
    }
    
    if (NULL != m_piSoftUSBDevice)
    {
        m_piSoftUSBDevice->Destroy();
        RELEASE(m_piSoftUSBDevice);
    }

    // Clean up the queue
    while (!m_InputReportQueue.isEmpty())
    {
        m_InputReportQueue.remove();
    }

    return S_OK;
}

HRESULT CHIDDevice::CreateUSBDevice()
/*++
Routine Description:
   Creates and initializes the device member variables and the device qualifier. 
   The device qualifier is required for USB2.0 devices.

Arguments:
    None

Return value:
    From called functions
--*/
{
    LogMessage( L"[SoftHIDReceiver][Info] CreateUSBDevice Entered...\n" );

    HRESULT hr = S_OK;

    hr = CoCreateInstance(CLSID_SoftUSBDevice, 
                            NULL,
                            CLSCTX_INPROC_SERVER,
                            __uuidof(ISoftUSBDevice),     
                            reinterpret_cast<void**>(&m_piSoftUSBDevice));

    IfFailHrGo(hr);

    //Setup the device 
    IfFailHrGo(m_piSoftUSBDevice->put_USB(0x0110));                           // binary coded decimal USB version 1.1
    IfFailHrGo(m_piSoftUSBDevice->put_DeviceClass(0));                        // 00 = This information is defined in the Interface descriptor
    IfFailHrGo(m_piSoftUSBDevice->put_DeviceSubClass(0));                     // 00 = This information is defined in the Interface descriptor
    IfFailHrGo(m_piSoftUSBDevice->put_DeviceProtocol(0));                     // 00 = This information is defined in the Interface descriptor
    IfFailHrGo(m_piSoftUSBDevice->put_MaxPacketSize0(8));                     // max packet size for the control endpoint (endpoint 0)
    IfFailHrGo(m_piSoftUSBDevice->put_Device(m_DeviceID));                    // Device Revision# - Binary decimal coded
    IfFailHrGo(m_piSoftUSBDevice->put_Vendor(m_VendorID));                    // Vendor ID - VID of the manufacturer of the device
    IfFailHrGo(m_piSoftUSBDevice->put_Product(m_ProductID));                  // product id - PID of the device
    IfFailHrGo(m_piSoftUSBDevice->put_RemoteWakeup(VARIANT_TRUE));            // Device does suppport remote wake up
    IfFailHrGo(m_piSoftUSBDevice->put_Manufacturer(STRING_IDX_MANUFACTURER)); // Index of the manufacturer string
    IfFailHrGo(m_piSoftUSBDevice->put_ProductDesc(STRING_IDX_PRODUCT_DESC));  // Index of the product descripton string
    IfFailHrGo(m_piSoftUSBDevice->put_SerialNumber(STRING_IDX_SERIAL_NUM));   // Index of the serial number string
    IfFailHrGo(m_piSoftUSBDevice->put_HasExternalPower(VARIANT_FALSE));       // Indicate that the device is not self-powered

    //Create the strings associated with the device
    IfFailHrGo(CreateStrings());

Exit:
    LogMessage( L"[SoftHIDReceiver][Info] CreateUSBDevice Exited...\n" );
    return hr;
}

HRESULT CHIDDevice::ConfigureConfig(ISoftUSBConfiguration * piConfig)
/*++
Routine Description:
    Initialize the USB configuration data 

Arguments:
    piConfig - pointer the SoftUSBConfiguration interface to be initialized

Return value:
    from called function.
--*/
{
    HRESULT hr = S_OK;

    IfFailHrGo(piConfig->put_ConfigurationValue(1));                   // config number passed to SetConfig
    IfFailHrGo(piConfig->put_Configuration((BYTE)STRING_IDX_CONFIG)); // Index of string descriptor
    IfFailHrGo(piConfig->put_Attributes(0xA0));                        // Bus powered and supports remote wakeup
    IfFailHrGo(piConfig->put_MaxPower(0x32));                          // max power in 2mA units: 0x32 x 2ma = 100ma

Exit:
    return hr;
}

HRESULT CHIDDevice::ConfigureINEndpoint()
/*++
Routine Description:
   Initializes the IN Endpoint interface 

Arguments:
    None 

Return value:
    E_UNEXPECTED if the IN endpoint is NULL otherwise
    From called function.
--*/
{
    HRESULT hr = S_OK;

    if (NULL == m_piINEndpoint)
    {
        IfFailHrGo(E_UNEXPECTED);
    }

    IfFailHrGo(m_piINEndpoint->put_EndpointAddress(0x81)); // Endpoint #1 IN  ( 0x80 = IN, 0x01 = Address )
    IfFailHrGo(m_piINEndpoint->put_Attributes(0x03));      // Mark this endpoint as an Interrupt endpoint
    IfFailHrGo(m_piINEndpoint->put_MaxPacketSize(0x8));    // Transfer 8 bytes at a time
    IfFailHrGo(m_piINEndpoint->put_Interval(0x0A));        // Poll device every 10 USB Frames
    IfFailHrGo(m_piINEndpoint->put_Halted(FALSE));

Exit:    
    return hr;
}

HRESULT CHIDDevice::ConfigureInterface(ISoftUSBInterface * piInterface)
/*++
Routine Description:
    Initialize the devices USB interface  

Arguments:
    piInterface - pointer the device Interface interface
    
Return value:
    From called function.
--*/
{
    HRESULT hr = S_OK;

    IfFailHrGo(piInterface->put_InterfaceNumber(0));      // Zero-based Interface number, this is the first
    IfFailHrGo(piInterface->put_AlternateSetting(0));     // 
    IfFailHrGo(piInterface->put_InterfaceClass(0x03));    // Identifies this Interface as a HID device
    IfFailHrGo(piInterface->put_InterfaceSubClass(0x00)); // Device does not provide a Boot Interface
    IfFailHrGo(piInterface->put_InterfaceProtocol(0x00)); // Device is neither a KB nor a Mouse, so the protocol is 0 (none)

    //Index for string describing the interface
    IfFailHrGo(piInterface->put_Interface((BYTE)STRING_IDX_INTERFACE));
    
    // Attach HID descriptor as device-specific descriptor to the Interface
    IfFailHrGo(piInterface->put_DeviceSpecificDescriptor(m_psaHIDDescriptor));

Exit:
    return hr;
}

STDMETHODIMP CHIDDevice::ConfigureDevice()
/*++
Routine Description:
   Configure the USB device. Create and setup the endpoints, configurations,
   and interfaces used by the device.

Arguments:
    None

Return value:
    From called function.
--*/
{
    LogMessage( L"[SoftHIDReceiver][Info] ConfigureDevice Entered...\n" );

    HRESULT                 hr              = S_OK;
    ISoftUSBConfiguration  *piConfig        = NULL;
    ISoftUSBInterface      *piInterface     = NULL;
    ISoftUSBConfigurations *piConfigList    = NULL;
    ISoftUSBInterfaces     *piInterfaceList = NULL;
    ISoftUSBEndpoints      *piEndpointList  = NULL; 
    VARIANT                 varIndex; 
    
    VariantInit(&varIndex);

    //All members of the collection will be added at the default locations
    //so set up the index appropriately
    varIndex.vt = VT_ERROR;
    varIndex.scode = DISP_E_PARAMNOTFOUND;

    //Create the IN Endpoint
    hr = CoCreateInstance(CLSID_SoftUSBEndpoint, 
                          NULL,
                          CLSCTX_INPROC_SERVER,
                          __uuidof(ISoftUSBEndpoint),     
                          reinterpret_cast<void**>(&m_piINEndpoint));
    IfFailHrGo(hr);

    //Setup the IN Endpoint
    IfFailHrGo(ConfigureINEndpoint());

    //Create the device interface
    hr = CoCreateInstance(CLSID_SoftUSBInterface, 
                          NULL,
                          CLSCTX_INPROC_SERVER,
                          __uuidof(ISoftUSBInterface),     
                          reinterpret_cast<void**>(&piInterface));
    IfFailHrGo(hr);

    // Store an interface on the control endpoint so we can handle
    // class-specific device requests later on
    IfFailHrGo(m_piSoftUSBDevice->get_Endpoint0(&m_piEndpoint0));

    // Set the control endpoint up to handle device specific requests
    hr = m_piEndpoint0->put_HandleStdDeviceRequests(VARIANT_FALSE);
    
    if( FAILED (hr) )
    {
        LogMessage( L"[SoftHIDReceiver][Error] HandleStdDeviceRequests Override Failed...\n" );
    }

    // Set up event sink on the control endpoint
    hr = SetupControlConnectionPoint(m_piEndpoint0, __uuidof(ISoftUSBEndpointEvents));

    if( FAILED (hr) )
    {
        LogMessage( L"[SoftHIDReceiver][Error] Setting Control Connection Point for Endpoint0 Failed...\n" );
    }

    //Setup the device interface
    IfFailHrGo(ConfigureInterface(piInterface));

    //Add the Endpoints to the endpoint list
    IfFailHrGo(piInterface->get_Endpoints(&piEndpointList));
    IfFailHrGo(piEndpointList->Add(reinterpret_cast<SoftUSBEndpoint*>(m_piINEndpoint), varIndex));

    //Create the configuration
    hr = CoCreateInstance(CLSID_SoftUSBConfiguration, 
                          NULL,
                          CLSCTX_INPROC_SERVER,
                          __uuidof(ISoftUSBConfiguration),     
                          reinterpret_cast<void**>(&piConfig));
    IfFailHrGo(hr);

    //Set the configuration data up
    IfFailHrGo(ConfigureConfig(piConfig));

    //Add the interface to the interface collection
    IfFailHrGo(piConfig->get_Interfaces(&piInterfaceList));
    IfFailHrGo(piInterfaceList->Add(reinterpret_cast<SoftUSBInterface*>(piInterface), varIndex));

    //Add the configuration to the configuration collection
    IfFailHrGo(m_piSoftUSBDevice->get_Configurations(&piConfigList));
    IfFailHrGo(piConfigList->Add(reinterpret_cast<SoftUSBConfiguration*>(piConfig), varIndex));

Exit:
    RELEASE(piConfig);
    RELEASE(piInterface);
    RELEASE(piConfigList);
    RELEASE(piInterfaceList);
    RELEASE(piEndpointList);
    LogMessage( L"[SoftHIDReceiver][Info] ConfigureDevice Exited...\n" );
    return hr;
}

HRESULT CHIDDevice::CreateStrings()
/*++
Routine Description:
   Creates all the strings used by the device. These strings are
   added to the strings collection which is maintained by the 
   USB device.

Arguments:
    None

Return value:
    From called function.
--*/
{
    LogMessage( L"[SoftHIDReceiver][Info] CreateStrings Entered...\n" );

    HRESULT             hr                   = S_OK;
    ISoftUSBStrings    *piStringList         = NULL;
    ISoftUSBString     *piStringManufacturer = NULL;
    ISoftUSBString     *piStringProductDesc  = NULL;
    ISoftUSBString     *piStringSerialNo     = NULL;
    ISoftUSBString     *piStringConfig       = NULL;
    ISoftUSBString     *piStringEndpoint     = NULL;
    BSTR                bstrManufacturer     = SysAllocString(L"Microsoft Corporation");
    BSTR                bstrProductDesc      = SysAllocString(L"Simulated USB HID Receiver device");
    BSTR                bstrSerialNo         = SysAllocString(L"000000");
    BSTR                bstrConfig           = SysAllocString(L"Configuration with a single interface");
    BSTR                bstrEndpoint         = SysAllocString(L"Interface with interrupt IN endpoint");
    VARIANT             varIndex; 
    time_t              currTime;    

    VariantInit(&varIndex);

    // Check that all BSTR allocations succeeded
    IfFalseHrGo(0 != SysStringLen(bstrManufacturer), E_OUTOFMEMORY);
    IfFalseHrGo(0 != SysStringLen(bstrProductDesc), E_OUTOFMEMORY);
    IfFalseHrGo(0 != SysStringLen(bstrSerialNo), E_OUTOFMEMORY);
    IfFalseHrGo(0 != SysStringLen(bstrConfig), E_OUTOFMEMORY);
    IfFalseHrGo(0 != SysStringLen(bstrEndpoint), E_OUTOFMEMORY);

    //
    // We want to ensure that each device that is instantiated has a 
    // serial number.  We've allocated spaces for 6 values in the initial string. 
    //
    time( &currTime );
    srand( (UINT)currTime );
    
    // Now get a random value
    m_SerialNumber = (UINT)( rand() % 999999 );

    //  Convert the int to a string and assign it to the BSTR
    _itow_s( m_SerialNumber, &bstrSerialNo[0], 6, 10 );


    //Create and initialize the string descriptors. Also create a string 
    //descriptor index for each. This index is used both to set the string's
    //descriptors position in the m_piSoftUSBDevice.Strings and is the index value 
    //the GetDescriptors request from the host. Note that we don't use 
    //string descriptor index zero because that is a reserved value for a 
    //device's language ID descriptor.

    //Get the string list from the device
    hr = m_piSoftUSBDevice->get_Strings(&piStringList);
    IfFailHrGo(hr);

    //Set up the varaint used as the index
    varIndex.vt = VT_I4;
    varIndex.lVal = STRING_IDX_MANUFACTURER;
    hr = AddString( piStringList,
               piStringManufacturer,
               bstrManufacturer,
               &varIndex );
    IfFailHrGo(hr);
    
    varIndex.lVal = STRING_IDX_PRODUCT_DESC;
    hr = AddString( piStringList,
               piStringProductDesc,
               bstrProductDesc,
               &varIndex );
    IfFailHrGo(hr);

    varIndex.lVal = STRING_IDX_SERIAL_NUM;
    hr = AddString( piStringList,
               piStringSerialNo,
               bstrSerialNo,
               &varIndex );
    IfFailHrGo(hr);

    varIndex.lVal = STRING_IDX_CONFIG;
    hr = AddString( piStringList,
               piStringConfig,
               bstrConfig,
               &varIndex );
    IfFailHrGo(hr);

    varIndex.lVal = STRING_IDX_INTERFACE;
    hr = AddString( piStringList,
               piStringEndpoint,
               bstrEndpoint,
               &varIndex );
    IfFailHrGo(hr);

Exit:
    RELEASE(piStringList);
    RELEASE(piStringManufacturer);
    RELEASE(piStringProductDesc);
    RELEASE(piStringSerialNo);
    RELEASE(piStringConfig);
    RELEASE(piStringEndpoint);
    
    SysFreeString(bstrManufacturer);
    SysFreeString(bstrProductDesc);
    SysFreeString(bstrSerialNo);
    SysFreeString(bstrConfig);
    SysFreeString(bstrEndpoint);
    
    LogMessage( L"[SoftHIDReceiver][Info] CreateStrings Exited...\n" );
    return hr;
}

HRESULT CHIDDevice::AddString( ISoftUSBStrings *piStringList,
                                  ISoftUSBString  *piString,
                                  BSTR            bstrVal,
                                  VARIANT         *vIndex )
/*++
Routine Description:
   Create the SoftUSBString used by the device, set the value, and
   add it to the collection of strings.

Arguments:
    piStringList - collection of strings
    piString     - SoftUSBString to populate
    bstrVal      - value of string
    vIndex       - index of string

Return value:
    From called function.
--*/
{
    HRESULT hr = S_OK;
    
    hr = CoCreateInstance(CLSID_SoftUSBString,
                          NULL,
                          CLSCTX_INPROC_SERVER,
                          __uuidof(ISoftUSBString),     
                          reinterpret_cast<void**>(&piString));
    IfFailHrGo(hr);

    IfFailHrGo(piString->put_Value(bstrVal));
    IfFailHrGo(piStringList->Add(reinterpret_cast<SoftUSBString*>(piString), *vIndex));

Exit:    
    return hr;
}

HRESULT CHIDDevice::ReleaseControlConnectionPoint()
/*++
Routine Description:
   Release the connection point to the Control Endpoint
   if one has been established.

Arguments:
    None

Return value:
    S_OK
--*/
{
    HRESULT hr = S_OK;
    
    LogMessage( L"[SoftHIDReceiver][Info] ReleaseControlConnectionPoint Entered...\n" );

    if ( (NULL != m_piCntrlConnectionPoint) && (0 != m_dwCntrlConnectionCookie) )
    {
        m_piCntrlConnectionPoint->Unadvise(m_dwCntrlConnectionCookie);
        m_dwCntrlConnectionCookie = 0;
    }

    RELEASE(m_piCntrlConnectionPoint);

    LogMessage( L"[SoftHIDReceiver][Info] ReleaseControlConnectionPoint Exited...\n" );

    return hr;
}

HRESULT CHIDDevice::SetupControlConnectionPoint(IUnknown *punkObject,
                                                REFIID    iidConnectionPoint)
/*++
Routine Description:
   Setup the connection point to the Control Endpoint.
   It validates that the punkObject supports IConnectionPointContainer
   then finds the correct connection point and attaches this object
   as the event sink.

Arguments:
    punkObject - Object which implement IConnectionPointContainer

    iidConnectionPoint - IID of the connection point

Return value:
    S_OK if the connection point is correctly set up
    E_UNEXPECTED if the IUnknown of this can not be obtained
    Otherwise from called functions.
--*/
{
    LogMessage( L"[SoftHIDReceiver][Info] SetupControlConnectionPoint Entered...\n" );

    HRESULT                     hr                          = S_OK;
    IConnectionPointContainer   *piConnectionPointContainer = NULL;
    IUnknown                    *punkSink                   = NULL;

    //If there is already connection point enabled, disable it
    if(NULL != m_piCntrlConnectionPoint)
    {
        IfFailHrGo(ReleaseControlConnectionPoint());
    }
        
    IfFailHrGo(punkObject->QueryInterface(IID_IConnectionPointContainer,
                                          reinterpret_cast<void **>(&piConnectionPointContainer)));

    IfFailHrGo(piConnectionPointContainer->FindConnectionPoint(iidConnectionPoint,
                                                               &m_piCntrlConnectionPoint));

    //Get the IUnknown of this interface as this is the event sink
    punkSink = (this)->GetUnknown(); 

    if(NULL == punkSink)
    {
        IfFailHrGo(E_UNEXPECTED);
    }

    IfFailHrGo(m_piCntrlConnectionPoint->Advise(punkSink, &m_dwCntrlConnectionCookie));

Exit:
    if( FAILED(hr) )
    {        
        ReleaseControlConnectionPoint();
    }

    LogMessage( L"[SoftHIDReceiver][Info] SetupControlConnectionPoint Exited...\n" );
    return hr;
}

STDMETHODIMP CHIDDevice::SetEndpointDiagnostics( BYTE endpointNum, ULONG diagnosticType )
/*++
Routine Description:
   Sets the diagnostic level on the endpoints for the device

Arguments:
    endpointNum    - The endpoint to apply the diagnostic level to
    diagnosticType - Flag used to set the level of logging on the endpoint

Return value:
    S_OK
    Error = From called functions
    E_UNEXPECTED = Endpoint does not exist
--*/
{
    HRESULT hr = S_OK;
    
    switch( endpointNum )
    {
        case 0:
            hr = EndpointDiagnosticsHelper( m_piEndpoint0, diagnosticType );
            break;
        case 1:
            hr = EndpointDiagnosticsHelper( m_piINEndpoint, diagnosticType );
            break;
        default:
            hr = E_UNEXPECTED;
            break;
    }

    return hr;
}

HRESULT CHIDDevice::EndpointDiagnosticsHelper(ISoftUSBEndpoint *piSoftUSBEndpoint, ULONG diagnosticType)
/*++
Routine Description:
   Sets the diagnostic level on the endpoints for the device

Arguments:
    piSoftUSBEndpoint - The endpoint to apply the diagnostic level to
    diagnosticType - Flag used to set the level of logging on the endpoint

Return value:
    S_OK = OK
    Other = Error
--*/
{
    IDSFDebug *piDSFDebug = NULL;

    HRESULT hr = piSoftUSBEndpoint->QueryInterface(__uuidof(IDSFDebug),
                                                   reinterpret_cast<void **>(&piDSFDebug));
    if (SUCCEEDED(hr))
    {
        hr = piDSFDebug->SetObjectFlags((DSFFlagType)SOFTUSBENDPOINT_OBJECTFLAGS, diagnosticType);
        piDSFDebug->Release();
    }
    return hr;
}

HRESULT WINAPI CHIDDevice::InputReportThreadProcWrapper( void* pHidObj )
/*++
Routine Description:
   This function takes a VOID* to a member function on an Object and 
   re-interprets it as a HIDDevice Member Func*. This is used to 
   work around the limitation of Win32 threads requiring a static 
   function which can't access the locals of an object.

Arguments:
    pHidObj - member function on a Hid object
    
Return value:
    S_OK 
--*/
{
    CHIDDevice* pHidDevice = (CHIDDevice*)pHidObj;
    pHidDevice->InputReportThreadProc();

    return S_OK;
}

HRESULT WINAPI CHIDDevice::InputReportThreadProc() 
/*++
Routine Description:
    This function handles Device CMDs (which include Tx data)

Arguments:
    none
    
Return value:
    S_OK 
--*/
{
    UINT sleepTime = 0;
    // Loop until the main thread decides to exit
    while( m_ProcessInputReports )
    {
        // If any reports are queued, send the report
        if( m_InputReportQueue.size() > 0 )
        {
            SendInputReport(&sleepTime);
        }
        
        if( sleepTime > 0 )
        {
            Sleep( sleepTime );
        }
        else
        {
            // If no default Sleep value was set for the HID keypress, wait the default amount of time
            Sleep( m_IdleRate );
        }
        
        sleepTime = 0; // Reset the sleep value
    }
  
    return S_OK; 
} 

STDMETHODIMP CHIDDevice::OnSetupTransfer( BYTE DataToggle,
                                               BYTE *pbDataBuffer,
                                               ULONG cbDataBuffer, 
                                               BYTE *pbStatus)
/*++
Routine Description:
   Implements the OnSetupTransfer for the interface ISoftUSBEndpointEvent

Arguments:
    DataToggle - DataToggle value
    pbDataBuffer - pointer to the buffer which contains the data
    cbDataBuffer - size of the data buffer
    pbStatus - pointer to transfer status value

Return value:
    E_NOTIMPL as this event is not handled by the device
--*/
{
    UNREFERENCED_PARAMETER(DataToggle);
    UNREFERENCED_PARAMETER(pbDataBuffer);
    UNREFERENCED_PARAMETER(cbDataBuffer);
    UNREFERENCED_PARAMETER(pbStatus);

    return E_NOTIMPL;
}

STDMETHODIMP CHIDDevice::OnWriteTransfer( BYTE DataToggle, 
                                               BYTE *pbDataBuffer,
                                               ULONG cbDataBuffer, 
                                               BYTE *pbStatus)
/*++
Routine Description:
   Implements the OnWriteTransfer for the interface ISoftUSBEndpointEvent

Arguments:
    DataToggle - DataToggle value
    pbDataBuffer - pointer to the buffer which contains the data
    cbDataBuffer - size of the data buffer
    pbStatus - pointer to transfer status value

Return value:
    E_UNEXPECTED if the IN endpoint is not valid or if any of the parameters are invalid
    Otherwise from call function
--*/
{
    UNREFERENCED_PARAMETER(DataToggle);
    UNREFERENCED_PARAMETER(pbDataBuffer);
    UNREFERENCED_PARAMETER(cbDataBuffer);
    UNREFERENCED_PARAMETER(pbStatus);

    return E_NOTIMPL;
}

STDMETHODIMP CHIDDevice::OnReadTransfer( BYTE DataToggle, 
                                              BYTE  *pbDataBuffer,
                                              ULONG   cbDataBuffer,
                                              ULONG *cbDataWritten,
                                              BYTE *pbStatus)       
/*++
Routine Description:
   Implements the OnReadTransfer for the interface ISoftUSBEndpointEvent

Arguments:
    DataToggle - DataToggle value
    pbDataBuffer - pointer to the data buffer
    cbDataBuffer - size of the data buffer
    cbDataWritten - amount of data actually written to the buffer
    pbStatus - pointer to transfer status value

Return value:
    E_NOTIMPL as this event is not implemented
--*/
{
    UNREFERENCED_PARAMETER(DataToggle);
    UNREFERENCED_PARAMETER(pbDataBuffer);
    UNREFERENCED_PARAMETER(cbDataBuffer);
    UNREFERENCED_PARAMETER(cbDataWritten);
    UNREFERENCED_PARAMETER(pbStatus);
    
    return E_NOTIMPL;
}

STDMETHODIMP CHIDDevice::OnDeviceRequest( USBSETUPREQUEST *pSetupRequest,
                                          ULONG           *RequestHandle, 
                                          BYTE            *pbHostData,
                                          ULONG            cbHostData, 
                                          BYTE           **ppbResponseData,
                                          ULONG           *pcbResponseData,
                                          BYTE            *pbSetupStatus)
/*++
Routine Description:
   Implements the OnDeviceRequest for the interface ISoftUSBEndpointEvent

Arguments:
    pSetupRequest   - pointer to the setup request begin handled
    RequestHandle   - Handle to the request
    pbHostData      - pointer to a buffer which contain data sent from the
                      host to the device for an OUT setup transfer
    cbHostData      - amount of data in the HostData buffer
    ppbReponseData  - pointer to data buffer to hold the response data
    pcbResponseData - pointer to the size of the data buffer
    pbSetupStatus   - pointer to the setup status

Return value:
    S_OK = The request was acknowledged and handled by the OnDeviceRequest (or called) function
    E_NOTIMPL = This request was not handled by OnDeviceRequest and should be passed on to the core DSF code to handle
    Other Error Values = These indicate something very wrong happened and will STALL the Control endpoint
--*/
{
    HRESULT hr = S_OK;
       
    UNREFERENCED_PARAMETER(RequestHandle);
    UNREFERENCED_PARAMETER(pbHostData);
    UNREFERENCED_PARAMETER(cbHostData);


    // Handle requests
    switch(pSetupRequest->bmRequestType.Byte)
    {
        // Device-to-Host Standard requests
        case USB_MAKE_REQUEST_TYPE(DIR_DEVICE_TO_HOST, TYPE_STANDARD, RCPT_DEVICE):
        case USB_MAKE_REQUEST_TYPE(DIR_DEVICE_TO_HOST, TYPE_STANDARD, RCPT_INTERFACE):
        {
            if (GET_DESCRIPTOR == pSetupRequest->bRequest)
            {
                IfFailHrGo(Get_Descriptor(pSetupRequest,
                                          ppbResponseData,
                                          pcbResponseData ));
            }
            else
            {
                hr = E_NOTIMPL;
            }
            break;
        }
        // Device-to-Host Class-specific requests
        case USB_MAKE_REQUEST_TYPE(DIR_DEVICE_TO_HOST, TYPE_CLASS, RCPT_INTERFACE):
        {
            switch (pSetupRequest->bRequest)
            {
                case GET_REPORT:
                    IfFailHrGo(Get_Report(pSetupRequest,
                                          ppbResponseData,
                                          pcbResponseData, 
                                          pSetupRequest->sSetupLength ));
                    break;
                case GET_IDLE:
                    // Not implemented for this device
                    hr = E_NOTIMPL;
                    break;
                case GET_PROTOCOL:
                    // This is not supported because this device does not implement a Boot protocol
                    hr = E_NOTIMPL;
                    break;
                // Let standard implementation handle everything else
                default:
                    hr = E_NOTIMPL;
                    break;
            }
            break;
        }
        // Host-to-Device Standard requests
        case USB_MAKE_REQUEST_TYPE(DIR_HOST_TO_DEVICE, TYPE_STANDARD, RCPT_DEVICE):
        case USB_MAKE_REQUEST_TYPE(DIR_HOST_TO_DEVICE, TYPE_STANDARD, RCPT_INTERFACE):
        {
                // This request is not supported
                hr = E_NOTIMPL;
            break;
        }
        // Host-to-Device Class-specific requests
        case USB_MAKE_REQUEST_TYPE(DIR_HOST_TO_DEVICE, TYPE_CLASS, RCPT_INTERFACE):
        {
            switch (pSetupRequest->bRequest)
            {
                case SET_REPORT:
                    // This is not supported since this device does not accept output reports
                    hr = E_NOTIMPL;
                    break;
                case SET_IDLE:
                    // This is not supported yet
                    hr = S_OK;
                    break;
                case SET_PROTOCOL:
                    // This is not supported since this device does not implement a boot protocol
                    hr = E_NOTIMPL;
                    break;
                // Let standard implementation handle everything else
                default:
                    hr = E_NOTIMPL;
                    break;
            }
            break;
        }
        // Let standard implementation handle everything else
        default:
        {
            hr = E_NOTIMPL;
            break;
        }
    }

Exit:

    if (FAILED(hr))
    {
        // Deallocate response memory
        if ((NULL != *ppbResponseData) || (NULL != *pcbResponseData))
        {
            CoTaskMemFree(*ppbResponseData);
            *ppbResponseData = NULL;
            *pcbResponseData = 0;
        }
        
        // We want to deallocate the memory, but not stall the endpoint if we fail because we 
        // see a request that we have not implemented a handler for
        if( hr != E_NOTIMPL )
            *pbSetupStatus = USB_STALL;
    }
    else
    {
        *pbSetupStatus = USB_ACK;
    }
    
    return hr;
}

STDMETHODIMP CHIDDevice::OnDeviceRequestComplete( ULONG RequestHandle,
                                                           BYTE *pbFinalRequestStatus)
/*++
Routine Description:
   Implements the OnDeviceRequestComplete for the interface ISoftUSBEndpointEvent

Arguments:
    RequestHandle - Handle to the request which has just completed
    pbFinalStatus - Pointer to the final status that is to be returned.
    
Return value:
    E_NOTIMPL as this event is not handled by the device
--*/
{
    UNREFERENCED_PARAMETER(RequestHandle);
    UNREFERENCED_PARAMETER(pbFinalRequestStatus);
    
    return E_NOTIMPL;
}

STDMETHODIMP CHIDDevice::get_DSFDevice(DSFDevice** ppDSFDevice)
/*++
Routine Description:
   Implement the get property DSFDevice for the interface
   IHIDDevice.

Arguments:
    ppDSFDevice - address of a pointer to receive the DSFDevice.

Return value:
    E_UNEXPECTED if the USB device does not exisit
    E_POINTER if ppDSFDevice is not a valid pointer
    Otherwise from called function
--*/
{
    HRESULT    hr = S_OK;
    DSFDevice *pDSFDevice = NULL;

    LogMessage( L"[SoftHIDReceiver][Info] get_DSFDevice Entered...\n" );

    // Validate that the USB device exists 
    if (NULL == m_piSoftUSBDevice)
    {
        IfFailHrGo(E_UNEXPECTED);
    }    

    if (NULL == ppDSFDevice)
    {
        IfFailHrGo(E_POINTER);
    }

    IfFailHrGo(m_piSoftUSBDevice->get_DSFDevice(&pDSFDevice));
    IfFailHrGo(reinterpret_cast<IDSFDevice *>(pDSFDevice)->QueryInterface(__uuidof(IDispatch), reinterpret_cast<void **>(ppDSFDevice)));

Exit:
    if (NULL != pDSFDevice)
    {
        reinterpret_cast<IDSFDevice *>(pDSFDevice)->Release();
    }
    LogMessage( L"[SoftHIDReceiver][Info] get_DSFDevice Exited...\n" );
    return hr;
}

STDMETHODIMP CHIDDevice::get_Logging( VARIANT_BOOL* logLevel )
/*++
Routine Description:
   Gets the current log level of the device

Arguments:
    logLevel - used to return the value of the log level

Return value:
    S_OK
--*/
{
    HRESULT hr = S_OK;
    
    *logLevel = m_Logging;
   
    return hr;
}

STDMETHODIMP CHIDDevice::put_Logging( VARIANT_BOOL logLevel )
/*++
Routine Description:
   Sets the current log level of the device

Arguments:
    logLevel - used to set the value of the log level

Return value:
    S_OK
--*/
{  
    HRESULT hr = S_OK;
    
    m_Logging = (BOOLEAN)logLevel;
   
    return hr;
}

STDMETHODIMP CHIDDevice::get_IdleTimeout( UINT* IdleTimeout )
/*++
Routine Description:
   Gets the current Idle Timeout of the device

Arguments:
    IdleTimeout - used to return the value of the log level

Return value:
    S_OK
--*/
{
    *IdleTimeout = m_IdleRate;
    
    return S_OK;
}

STDMETHODIMP CHIDDevice::put_IdleTimeout( UINT IdleTimeout )
/*++
Routine Description:
   Sets the current Idle Timeout of the device

Arguments:
    IdleTimeout - used to set the value of the log level

Return value:
    S_OK
--*/
{
    InterlockedExchange(&m_IdleRate, IdleTimeout);
    return S_OK;
}

STDMETHODIMP CHIDDevice::QueueInputReport( SAFEARRAY* psaInputReport, UINT timeoutDuration )
/*++
Routine Description:
   Queues additional input reports

Arguments:
    IdleTimeout - used to set the value of the log level

Return value:
    S_OK
--*/
{
    VARIANT HUGEP*      pArrayData  = NULL;
    ULONG               cbData      = 0;
    LONG                lLBound     = 0;
    LONG                lUBound     = 0;
    HRESULT             hr          = S_OK;
    HID_INPUT_REPORT    inputReport;
    BOOLEAN             result      = TRUE;

    // Initialize Structure
    inputReport.timeout = timeoutDuration;
    inputReport.cbInputReport = 0;
    inputReport.pbInputReport = NULL;

    // Get SAFEARRAY size
    IfFailHrGo(SafeArrayGetLBound(psaInputReport, 1, &lLBound));
    IfFailHrGo(SafeArrayGetUBound(psaInputReport, 1, &lUBound));
    IfFalseHrGo(lUBound > lLBound, E_UNEXPECTED);
    cbData = lUBound - lLBound + 1;

    inputReport.pbInputReport = (BYTE*)CoTaskMemAlloc( cbData * sizeof(BYTE) );
    
    // If the memory Allocation fails, then fail and exit
    if( inputReport.pbInputReport == NULL )
    {
        hr = E_UNEXPECTED;
        goto Exit;
    }
    
    IfFailHrGo( SafeArrayAccessData( psaInputReport, 
                                     (void HUGEP**)&pArrayData ) );

    // Step through the SAFEARRAY and populating only BYTE input report
    // and bail out if the type is not correct
    for( LONG i = lLBound; i <= lUBound; ++i )
    {
        if (pArrayData[i].vt == VT_UI1)
        {
            inputReport.pbInputReport[i-lLBound] = pArrayData[i].bVal;
        }
        else
        {
            hr = E_FAIL;
            goto Exit;
        }
    }
    
    inputReport.cbInputReport = cbData;
    
    // Add the report to the input queue (does a shallow copy so no need to free the array data)
    result = m_InputReportQueue.insert( inputReport );
    if (result == FALSE)
    {
        hr = E_FAIL;
    }

Exit:
    SafeArrayUnaccessData(psaInputReport);
    if( FAILED(hr) )
    {
        CoTaskMemFree(inputReport.pbInputReport);
    }
    return hr;
}

STDMETHODIMP CHIDDevice::StartProcessing()
/*++
Routine Description:
    Begin processing input reports

Arguments:
    none

Return value:
    S_OK 
    E_UNEXPECTED
--*/
{
    HRESULT hr = S_OK;

    // This is the sentinel that is watched so the Input Report thread knows
    //  if it should begin/continue processing input
    m_ProcessInputReports = TRUE;

    //
    // Begin Processing input
    //
    m_ThreadHandle = CreateThread( NULL,                    //LPSECURITY_ATTRIBUTES lpsa
                                   0,                       // DWORD dwStackSize
                                   (LPTHREAD_START_ROUTINE)InputReportThreadProcWrapper, // LPTHREAD_START_ROUTINE pfnThreadProc
                                   this,                    // void* pvParam
                                   0,                       // DWORD dwCreationFlags
                                   &m_InputReportThreadID   // DWORD* pdwThreadId 
                                  );
                                  
    if( m_ThreadHandle == NULL )
    {
        LogMessage( L"[SoftHIDReceiver][Error] Input Thread Creation for the InputReport Queue Failed..." );
        hr = E_UNEXPECTED;
        goto Exit;
    }

Exit:
    return hr;
}

STDMETHODIMP CHIDDevice::StopProcessing()
/*++
Routine Description:
    Stop processing input reports

Arguments:
    none

Return value:
    S_OK
    E_UNEXPECTED
--*/
{
    // This is the value the thread looks at 
    //  to know if it should exit
    m_ProcessInputReports = FALSE;

    if( m_ThreadHandle != NULL )
    {
        // Wait for the thread to exit
        WaitForSingleObject( m_ThreadHandle, INFINITE);
    
        CloseHandle( m_ThreadHandle );
        m_ThreadHandle = NULL;
        m_InputReportThreadID = 0;
    }
    
    return S_OK;
}

STDMETHODIMP CHIDDevice::BuildHIDDescriptors()
/*++
Routine Description:
   Constructs the Report and HID descriptors in order

Arguments:
    none

Return value:
    S_OK
    Otherwise from called function
--*/
{
    HRESULT hr = S_OK;
    
    // Build up the HID and Report Descriptors
    // We want to build the Report Descriptor first because the HID descriptor needs
    //  to know how large the Report Descriptor is
    IfFailHrGo(CreateReportDescriptor());
    
    IfFailHrGo(CreateHIDDescriptor());

Exit:
    return hr;
}

HRESULT CHIDDevice::CreateHIDDescriptor()
/*++
Routine Description:
   Constructs the default HID descriptors

Arguments:
    none

Return value:
    S_OK
    Otherwise from called function
--*/
{
    HRESULT hr                      = S_OK;
    VARIANT HUGEP* pDescriptorData  = NULL;
    LONG    lLBound                 = 0;
    LONG    lUBound                 = 0;
    SHORT   sTotalSize              = 0;

    // If for some reason this is not NULL, free the memory so we can build it up again
    if( m_psaHIDDescriptor != NULL )
    {
        SafeArrayDestroy(m_psaHIDDescriptor);
        m_psaHIDDescriptor = NULL;
    }

    if( m_psaReportDescriptor == NULL )
    {
        // If the Report Descriptor is not built yet, we can't create the HID descriptor  
        hr = E_UNEXPECTED;
        goto Exit;
    }

    // Now find out how large the Report Descriptor is to allows us to 
    // update the Report Descriptor in one place and have the code update automatically
    IfFailHrGo(SafeArrayGetLBound(m_psaReportDescriptor, 1, &lLBound));
    IfFailHrGo(SafeArrayGetUBound(m_psaReportDescriptor, 1, &lUBound));
    IfFalseHrGo(lUBound > lLBound, E_UNEXPECTED);
    
    // This size must fit into a Short as only two BYTEs are allocated in the HID Descriptor for the Report Size
    sTotalSize = (SHORT)(lUBound - lLBound + 1);  

    BYTE  bHIDDescriptorData[] = { 
                                    0x09,                         // bLength - Total size of the HID Descriptor
                                    0x21,                         // bDescriptorType - HID descriptor
                                    0x01, 0x01,                   // bcdHID - HID Class Specification release
                                    0x00,                         // bCountryCode - Haredware is not localized for any specific locale
                                    0x01,                         // bNumDescriptors - We only have one report descriptor
                                    0x22,                         // bDescriptorType - First Descriptor Type (0x22 == Report Descriptor)
                                    LOBYTE(sTotalSize), HIBYTE(sTotalSize)  // wDescriptorLength - First Descriptor Length ( LSB first )
                                    // Optional - bDescriptorType - Repeat for each additional Descriptor
                                    // Optional - wDescriptorLength - Repeat for each additional Descriptor
                                  };
    
    // Create a boundary struct to pass in when we create the SafeArray
    SAFEARRAYBOUND rgsabound[1];
    ZeroMemory(rgsabound, sizeof(rgsabound));
    rgsabound[0].lLbound = 0;
    rgsabound[0].cElements = sizeof(bHIDDescriptorData);
    
    // Create as an Array of Variants becuase that is what the DSF 
    // Interface->put_DeviceSpecificDescriptor is expecting an array of Variants
    m_psaHIDDescriptor = SafeArrayCreate( VT_VARIANT, 1, rgsabound );
    
    if( m_psaHIDDescriptor == NULL )
    {
        // if the create failed, then goto to exit
        hr = E_UNEXPECTED;
        goto Exit;
    }
    
    IfFailHrGo( SafeArrayAccessData( m_psaHIDDescriptor, 
                                     (void HUGEP**)&pDescriptorData) );

    // Step through the SAFEARRAY and populate each variant with a Byte value
    INT descSize = sizeof(bHIDDescriptorData);
    for( INT i = 0; i < descSize; ++i )
    {
        pDescriptorData[i].vt = VT_UI1;
        pDescriptorData[i].bVal = bHIDDescriptorData[i];
    }

Exit:
    SafeArrayUnaccessData(m_psaHIDDescriptor);
    return hr;
}

STDMETHODIMP CHIDDevice::CreateCustomHIDDescriptor(SAFEARRAY* psaHIDDescriptor)
/*++
Routine Description:
   Constructs the HID descriptors based on input parameter.  
   Call ConfigureDevice from the client to load the descriptor.

Arguments:
    SAFEARRAY - variant subtype is enforced by the interface and is the vbscript array type

Return value:
    S_OK
    E_UNEXPECTED
    Otherwise from called function
--*/
{
    HRESULT     hr          = S_OK;
    LONG        lLBound     = 0;
    LONG        lUBound     = 0;
    VARIANT     *pVariant   = NULL;
    VARTYPE     vt          = VT_ERROR;
    
    if( m_psaReportDescriptor == NULL )
    {
        // If the Report Descriptor is not built yet, we can't create the HID descriptor.  
        hr = E_UNEXPECTED;
        goto Exit;
    }

    if( psaHIDDescriptor == NULL )
    {
        // If passed in SAFEARRAY is null fail out
        hr = E_UNEXPECTED;
        goto Exit;
    }

    // Check that the SAFEARRAY parameter is the typed as VARIANT
    IfFailHrGo( SafeArrayGetVartype(psaHIDDescriptor, &vt) );

    if ( vt != VT_VARIANT )
    {
        hr = E_UNEXPECTED;
        goto Exit;
    }

    IfFailHrGo( SafeArrayAccessData( psaHIDDescriptor,
                                     (void **)&pVariant ) );

    // Now find out how large the Report Descriptor is to allows us to 
    // update the Report Descriptor in one place and have the code update automatically
    IfFailHrGo(SafeArrayGetLBound(psaHIDDescriptor, 1, &lLBound));
    IfFailHrGo(SafeArrayGetUBound(psaHIDDescriptor, 1, &lUBound));
    IfFalseHrGo(lUBound > lLBound, E_UNEXPECTED);

    // Walk through the VARIANT array and check that each
    // element contains required byte type.  If an 
    // unexpected variant type is found, bail out gently to 
    // the default report and hid descriptors
    for(INT i = lLBound; i <= lUBound; i++)
    {
        switch(pVariant->vt){
            case VT_I1:
            case VT_UI1:
                break;
            default:
                // Unexpected variant type or empty
                goto Exit;
                break;
        }
        pVariant++;
    }

    if( m_psaHIDDescriptor != NULL )
    {
        SafeArrayDestroy(m_psaHIDDescriptor);
        m_psaHIDDescriptor = NULL;
    }
    
    // Create as an Array of Variants because that is what the DSF 
    // Interface->put_DeviceSpecificDescriptor is expecting an array of Variants
    m_psaHIDDescriptor = SafeArrayCreate( VT_VARIANT, 1, psaHIDDescriptor->rgsabound);
    
    if( m_psaHIDDescriptor == NULL )
    {
        // if the create failed, then goto to exit
        hr = E_UNEXPECTED;
        goto Exit;
    }
    
    IfFailHrGo( SafeArrayAccessData( psaHIDDescriptor, 
                                     &(m_psaHIDDescriptor->pvData) ) );

Exit:
    SafeArrayUnaccessData(m_psaHIDDescriptor);
    SafeArrayUnaccessData(psaHIDDescriptor);
    return hr;
}

HRESULT CHIDDevice::CreateReportDescriptor()
/*++
Routine Description:
   Constructs the default report descriptors

Arguments:
    none

Return value:
    S_OK
    Otherwise from called function
--*/
{
    HRESULT hr                     = S_OK;
    VARIANT HUGEP* pDescriptorData = NULL;
    
    if( m_psaReportDescriptor != NULL )
    {
        SafeArrayDestroy(m_psaReportDescriptor);
        m_psaReportDescriptor = NULL;
    }

    BYTE           bReportDescriptorData[ ] = { 
                                                // Consumer Controls
                                                    0x05,   0x0c,       // Usage Page (Consumer Controls),
                                                    0x09,   0x01,       // Usage (Consumer Control),
                                                    0xA1,   0x01,       // Collection (Application),
                                                    0x85,   0x01,       //  Report Id (1)
                                                    0x19,   0x00,       //  Usage Minimum (0),
                                                    0x2a,   0x3c, 0x02, //  Usage Maximum (23c)  
                                                    0x15,   0x00,       //  Logical Minimum (0),
                                                    0x26,   0x3c, 0x02, //  Logical Maximum (23c)  
                                                    0x95,   0x01,       //  Report Count (1),
                                                    0x75,   0x10,       //  Report Size (16),
                                                    0x81,   0x00,       //  Input (Data, Array), 
                                                    0xC0,               // End Collection 
                                                // MS Vendor controls
                                                    0x06,   0xbc, 0xff, // Usage Page (Vendor 0xffbc),
                                                    0x09,   0x88,       // Usage (88),
                                                    0xa1,   0x01,       // Collection (Application),
                                                    0x85,   0x02,       //  Report Id (2)
                                                    0x19,   0x01,       //  Usage Minimum (0x01),
                                                    0x29,   0xff,       //  Usage Maximum (0xff),
                                                    0x15,   0x00,       //  Logical Minimum (0),
                                                    0x25,   0x01,       //  Logical Maximum(1),
                                                    0x95,   0x01,       //  Report Count (1),
                                                    0x75,   0x08,       //  Report Size (8),
                                                    0x81,   0x00,       //  Input (Data, Array), 
                                                    0xc0,               // End Collection
                                                // Standby button
                                                    0x05,   0x01,       // Usage Page (Generic Desktop),
                                                    0x09,   0x80,       // Usage (System Control),
                                                    0xa1,   0x01,       // Collection (Application),
                                                    0x85,   0x03,       //  Report Id (3)
                                                    0x19,   0x81,       //  Usage Minimum (0x81),
                                                    0x29,   0x83,       //  Usage Maximum (0x83),
                                                    0x25,   0x01,       //  Logical Maximum(1),
                                                    0x75,   0x01,       //  Report Size (1),
                                                    0x95,   0x03,       //  Report Count (3),
                                                    0x81,   0x02,       //  Input
                                                    0x75,   0x01,       //  Report Size (1),   
                                                    0x95,   0x05,       //  Report Count (5),
                                                    0x81,   0x01,       //  Input (Constant),
                                                    0xC0,               // End Collection
                                                // Keyboard
                                                    0x05,   0x01,       // Usage Page (Generic Desktop),
                                                    0x09,   0x06,       // Usage (Keyboard),
                                                    0xA1,   0x01,       // Collection (Application),
                                                    0x85,   0x04,       //  Report Id (4)
                                                    0x05,   0x07,       //  Usage Page (Key Cdes)
                                                    0x19,   0xe0,       //  Usage Minimum left control
                                                    0x29,   0xe8,       //  Usage Maximum keyboard right gui
                                                    0x75,   0x01,       //  Report Size (1)
                                                    0x95,   0x08,       //  Report Count (8)
                                                    0x81,   0x02,       //  Input (Variable)
                                                    0x19,   0x00,       //  Usage Minimum (0)
                                                    0x29,   0x90,       //  Usage Maximum (90)
                                                    0x26,   0xff, 0x00, //  Logical Maximum (0xff)
                                                    0x75,   0x08,       //  Report Size (8)
                                                    0x95,   0x01,       //  Report Count (1)
                                                    0x81,   0x00,       //  Input (Data, Array),
                                                    0xC0                // End Collection
                                            };
    
    // Build a SafeArray Struct that will hold the size and dimension requirements that 
    // will be used during the Safe Array creation
    SAFEARRAYBOUND rgsabound[1];
    ZeroMemory(rgsabound, sizeof(rgsabound));
    rgsabound[0].lLbound = 0;
    rgsabound[0].cElements = sizeof(bReportDescriptorData);
    
    m_psaReportDescriptor = SafeArrayCreate(VT_VARIANT, 1, rgsabound);
    
    if( m_psaReportDescriptor == NULL )
    {
        hr = E_UNEXPECTED;
        goto Exit;
    }
        
    IfFailHrGo( SafeArrayAccessData( m_psaReportDescriptor, 
                                     (void HUGEP**)&pDescriptorData));

    INT descSize = sizeof(bReportDescriptorData);
    for( INT i = 0; i < descSize; ++i )
    {
        pDescriptorData[i].vt = VT_UI1;
        pDescriptorData[i].bVal = bReportDescriptorData[i];
    }

Exit:
    SafeArrayUnaccessData(m_psaReportDescriptor);
    return hr;
}

STDMETHODIMP CHIDDevice::CreateCustomReportDescriptor(SAFEARRAY* psaReportDescriptor)
/*++
Routine Description:
   Constructs the Report descriptors based on the input parameter.
   Call ConfigureDevice from the client to load the descriptor.

Arguments:
    none

Return value:
    S_OK
    Otherwise from called function
--*/
{
    HRESULT     hr          = S_OK;
    LONG        lLBound     = 0;
    LONG        lUBound     = 0;
    VARTYPE     vt          = VT_ERROR;
    VARIANT     *pVariant   = NULL;
    
    if ( psaReportDescriptor == NULL )
    {
        hr = E_UNEXPECTED;
        goto Exit;
    }

    // Check that the SAFEARRAY parameter is the typed as VARIANT
    IfFailHrGo( SafeArrayGetVartype(psaReportDescriptor, &vt) );

    if ( vt != VT_VARIANT )
    {
        hr = E_UNEXPECTED;
        goto Exit;
    }

    hr = ( SafeArrayAccessData( psaReportDescriptor,
                                     (void **)&pVariant ) );
    if( !FAILED(hr) )
    {
        IfFailHrGo( SafeArrayGetLBound(psaReportDescriptor, 1, &lLBound) );
        IfFailHrGo( SafeArrayGetUBound(psaReportDescriptor, 1, &lUBound) );
        SafeArrayUnaccessData(psaReportDescriptor);
    }
    
    // Walk through the VARIANT array and check that each
    // element contains supported data type.  If an 
    // unexpected variant type is found, bail out gently to 
    // the default report and hid descriptors
    for(INT i = lLBound; i <= lUBound; i++)
    {
        switch(pVariant->vt){
            case VT_I1:
            case VT_UI1:
                break;
            default:
                // Unexpected variant type or empty
                goto Exit;
                break;
        }
        pVariant++;
    }

    // Clear out default report descriptors
    if( m_psaReportDescriptor != NULL )
    {
        SafeArrayDestroy(m_psaReportDescriptor);
        m_psaReportDescriptor = NULL;
    }

    // Initialize the report descriptor to the correct size
    m_psaReportDescriptor = SafeArrayCreate(VT_VARIANT, 1, psaReportDescriptor->rgsabound);
    
    if( m_psaReportDescriptor == NULL )
        goto Exit;

    IfFailHrGo( SafeArrayAccessData( psaReportDescriptor,
                                     &(m_psaReportDescriptor->pvData) ) );

Exit:
    SafeArrayUnaccessData(psaReportDescriptor);   
    return hr;
}

HRESULT CHIDDevice::Get_Descriptor( USBSETUPREQUEST  *pSetupRequest,
                                    BYTE            **ppbDataBuffer,
                                    ULONG            *pcbDataWritten )
/*++
Routine Description:
    Implements the Get_Descriptor request. This is a USB HID class request.
    Return the request descriptor from the device.
 
Arguments:
    pSetupRequest - Pointer to setup request structure
    pbDataBuffer - Pointer to buffer data to be written to
    cbDataBuffer - Size of buffer in bytes
    pcbDataWritten - Pointer to number of bytes that were actually written

Return Value:
    E_POINTER
        Invalid pointer for output parameter
    E_INVALIDARG
        At least one argument is invalid.
    E_FAIL
        Could not complete the request.
    S_OK
        Success
--*/
{
    HRESULT hr = S_OK;

    // Check to ensure we don't have a pre-allocated buffer and 
    // have a place to store the count of bytes written
    IfFalseHrGo(NULL != ppbDataBuffer, E_POINTER);
    IfFalseHrGo(NULL != pcbDataWritten, E_POINTER);
    IfFalseHrGo(NULL == *ppbDataBuffer, E_UNEXPECTED);
    *pcbDataWritten = 0;

    // Arguments sanity check
    IfFalseHrGo(GET_DESCRIPTOR     == pSetupRequest->bRequest,                     E_INVALIDARG);
    IfFalseHrGo(DIR_DEVICE_TO_HOST == pSetupRequest->bmRequestType.Bits.Direction, E_INVALIDARG);
    IfFalseHrGo(TYPE_STANDARD      == pSetupRequest->bmRequestType.Bits.Type,      E_INVALIDARG);
    IfFalseHrGo(RCPT_INTERFACE     == pSetupRequest->bmRequestType.Bits.Recipient, E_INVALIDARG);

    // This is USB HID class descriptor request
    if (REPORT_DESCRIPTOR == HIBYTE(pSetupRequest->sSetupValue))
    {
        IfFailHrGo( GetReportDescriptor(ppbDataBuffer, pcbDataWritten, pSetupRequest->sSetupValue) );
    }
    else if (PHYSICAL_DESCRIPTOR == HIBYTE(pSetupRequest->sSetupValue))
    {
        // We don't support Physical Descriptors on this device
        hr = E_NOTIMPL;
    }
    else
    {
        // This catches any other unknown descriptor requests
        hr = E_NOTIMPL;
    }

Exit:
    return hr;

}

HRESULT CHIDDevice::Get_Report( USBSETUPREQUEST   *pSetupRequest,
                                BYTE             **ppbDataBuffer,
                                ULONG             *pcbDataWritten,
                                SHORT              cbMaxDataSize  )
/*++
Routine Description:
    Implements the Get_Report method.
    This allows the host to receive a report
 
Arguments:
    pSetupRequest - Pointer to setup request structure
    pbDataBuffer - Pointer to buffer data to be written to
    pcbDataWritten - Pointer to number of bytes that were actually written

Return Value:
    E_POINTER
        Invalid pointer for output parameter
    S_FALSE
        Could not complete request
    S_OK
        Success
--*/
{
    HRESULT hr = S_OK;
    HID_INPUT_REPORT inputReport;    
    BOOLEAN result = TRUE;
    
    // Check to ensure we don't have a pre-allocated buffer and have a place 
    // to store the count of bytes written
    IfFalseHrGo(NULL != ppbDataBuffer, E_POINTER);
    IfFalseHrGo(NULL != pcbDataWritten, E_POINTER);
    IfFalseHrGo(NULL == *ppbDataBuffer, E_UNEXPECTED);
    *pcbDataWritten = 0;
    
    switch(HIBYTE(pSetupRequest->sSetupValue))
    {
        case HID_REPORT_TYPE_INPUT:
        {
            if ( m_InputReportQueue.size() > 0 )
            {
                // Return Input Report if it exists, otherwise return zero buffer
                inputReport = m_InputReportQueue.remove();                
                if (result == FALSE)
                {
                    hr = E_FAIL;
                    goto Exit;
                }
                
                // Copy Input Report data buffer. We can't return more than the size 
                // sent in the USB request (cbMaxDataSize) but we can return less.
                // Choose the smaller of the two values to fill
                *pcbDataWritten = min( inputReport.cbInputReport, (USHORT)cbMaxDataSize );
                *ppbDataBuffer = (BYTE*)CoTaskMemAlloc(*pcbDataWritten);
                
                if( NULL == *ppbDataBuffer )
                {
                    // We failed to allocate memory, unlock the buffer and bail out...
                    *pcbDataWritten = 0;
                    hr = E_OUTOFMEMORY;
                    goto Exit;
                }
                
                RtlCopyMemory(*ppbDataBuffer, inputReport.pbInputReport, *pcbDataWritten);
                
                // If we send a report in this manner, ignore the timeoutDuration in the HID REPORT
                if( inputReport.pbInputReport != NULL )
                {
                    CoTaskMemFree(inputReport.pbInputReport);
                }
            }
            else
            {
                // Ensure the byte count is zero
                *pcbDataWritten = 0;
            }
            
            hr = S_OK;
            break;
        }
        case HID_REPORT_TYPE_OUTPUT:
            // This device does not support queueing Output Reports
            hr = E_NOTIMPL;
            break;
        case HID_REPORT_TYPE_FEATURE:
            // This device does not support quering Feature Reports
            hr = E_NOTIMPL;
            break;
        default:
            hr = E_NOTIMPL;
    }
    
Exit:
    return hr;

}

HRESULT CHIDDevice::SendInputReport(UINT *pTimeout)
/*++
Routine Description:
    Sends the input report by queueing it on the IN endpoint.

Arguments:
    none
    
Return Value:
     UINT for report timeout
--*/
{
    HID_INPUT_REPORT    inputReport;
    HRESULT             hr              = S_OK;
    inputReport.cbInputReport           = 0;
    inputReport.pbInputReport           = NULL;
    inputReport.timeout                 = 0;

    inputReport = m_InputReportQueue.remove();
    if (NULL == inputReport.pbInputReport)
    {
        hr = E_FAIL;
        goto Exit;
    }
    *pTimeout = inputReport.timeout;
    
    hr = m_piINEndpoint->QueueINData(inputReport.pbInputReport,
                                inputReport.cbInputReport,
                                USB_ACK,
                                SOFTUSB_FOREVER);

    IfFalseHrGo(hr, S_OK);
    
    if( inputReport.pbInputReport != NULL )
    {
        CoTaskMemFree(inputReport.pbInputReport);
    }

Exit:    
    return hr;
}

HRESULT CHIDDevice::GetReportDescriptor( BYTE   **ppbDataBuffer,
                                         ULONG   *pcbDataWritten,
                                         SHORT   cbMaxBufferLen )
/*++
Routine Description:
    Returns Report Descriptor in supplied data buffer
 
Arguments:
    pbDataBuffer - Pointer to buffer data to be written to
    cbDataBuffer - Size of buffer in bytes
    pcbDataWritten - Pointer to number of bytes that were actually written

Return Value:
    E_UNEXPECTED
        Safearray with Report Descriptor has unexpected boundaries
    S_OK
        Success

--*/
{
    VARIANT HUGEP* pArrayData = NULL;
    LONG           lLBound = 0;
    LONG           lUBound = 0;
    HRESULT        hr = S_OK;

    // Check to ensure we don't have a pre-allocated buffer and 
    // have a place to store the count of bytes written
    IfFalseHrGo(NULL != ppbDataBuffer, E_POINTER);
    IfFalseHrGo(NULL != pcbDataWritten, E_POINTER);
    IfFalseHrGo(NULL == *ppbDataBuffer, E_UNEXPECTED);

    // Ensure we have a Report Descriptor to access
    IfFalseHrGo(NULL != m_psaReportDescriptor , E_POINTER);

    // Get SAFEARRAY size
    IfFailHrGo(SafeArrayGetLBound(m_psaReportDescriptor, 1, &lLBound));
    IfFailHrGo(SafeArrayGetUBound(m_psaReportDescriptor, 1, &lUBound));
    IfFalseHrGo(lUBound > lLBound, E_UNEXPECTED);
    
    // We can't return more than the size sent to us via the USB Request, so figure out
    // which buffer size is the smallest and constrain to that
    *pcbDataWritten = min( lUBound - lLBound + 1, cbMaxBufferLen );

    IfFailHrGo( SafeArrayAccessData( m_psaReportDescriptor, 
                                     (void HUGEP**)&pArrayData ) );

    *ppbDataBuffer = (BYTE*)CoTaskMemAlloc(*pcbDataWritten);
    IfFalseHrGo(NULL != ppbDataBuffer, E_OUTOFMEMORY);
    
    for( UINT i = 0; i < *pcbDataWritten; ++i )
    {
        (*ppbDataBuffer)[i] = pArrayData[i].bVal;
    }
    
    IfFailHrGo( SafeArrayUnaccessData(m_psaReportDescriptor) );

Exit:

    if( FAILED(hr) )
    {
        *pcbDataWritten = 0; 
        
        // Free the buffer Memory
        if( *ppbDataBuffer != NULL )
        {
            CoTaskMemFree( *ppbDataBuffer );
            *ppbDataBuffer = NULL;
        }    
    }

    return hr;

}

void CHIDDevice::LogMessage( __in WCHAR* messageToLog )
/*++
Routine Description:
    This is a wrapper function for logging as we currently use a generic "OutputDebugString" call
    which is useful for basic debugging. Some developers may wish to use a different logging method 
    and this function makes it easy to replace OutputDebugString with their favorite function
    
Arguments:
    messageToLog - message to write to log
    
Return Value:
    none

--*/
{
    if( m_Logging == TRUE )
    {
        OutputDebugString( messageToLog );
    }
}

