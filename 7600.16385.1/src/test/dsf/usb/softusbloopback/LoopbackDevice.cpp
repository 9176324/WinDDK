/*++ BUILD Version 0000

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    LoopbackDevice.cpp

Abstract:

    Implements the interface ILoopbackDevice and
    configures the loopback device to be a valid USB device.
    The device then processes input to its endpoint in one of 
    two ways.

    1. By running in polled mode where the data is simply 
       passed from the OUT Endpoint to the IN Endpoint

       or
    2. In Event mode where the loopback device receives a 
       callback to indicate that data needs to be processed,
       and then processes the data.
--*/

#include "stdafx.h"
#include <stdio.h>
#include <conio.h>
#include <USBProtocolDefs.h>
#include <dsfif.h>
#include <softusbif.h>
#include "LoopbackDevice.h"
#include "SoftUSBLoopback_i.c"

// These are the indexes of the string descriptors. They are used both
// as the indexes of the strings with SoftUSBDevice.Strings and as the
// string descriptor index property values on the various objects (e.g.
// SoftUSBDevice.Manufacturer = STRING_IDX_MANUFACTURER).

#define STRING_IDX_MANUFACTURER     1
#define STRING_IDX_PRODUCT_DESC     2
#define STRING_IDX_CONFIG           3
#define STRING_IDX_INTERFACE        4


CLoopbackDevice::CLoopbackDevice()
/*++
Routine Description:
    Constructor for CLoopbackDevice. 

Arguments:
    None

Return value:
    None
--*/
{
    InitMemberVariables();
}

CLoopbackDevice::~CLoopbackDevice()
/*++
Routine Description:
    Destructor for CLoopbackDevice. 

Arguments:
    None

Return value:
    None
--*/
{
    //Release the conneciton point
    ReleaseConnectionPoint();
    
    //Release any interface which the device is holding
    RELEASE(m_piConnectionPoint);
    RELEASE(m_piINEndpoint);
    RELEASE(m_piOUTEndpoint);

    if (NULL != m_piSoftUSBDevice)
    {
        (void)m_piSoftUSBDevice->Destroy();
        RELEASE(m_piSoftUSBDevice);
    }

    InitMemberVariables();
}

void CLoopbackDevice::InitMemberVariables()
/*++
Routine Description:
   Initialize member variables for the class CLoopbackDevice  

Arguments:
    None

Return value:
    None
--*/
{
    m_piSoftUSBDevice = NULL;
    m_piINEndpoint = NULL;
    m_piOUTEndpoint = NULL;
    m_piConnectionPoint = NULL;
    m_iInterfaceString = 0;
    m_iConfigString = 0;
    m_dwConnectionCookie = 0;
}



HRESULT CLoopbackDevice::FinalConstruct()
/*++
Routine Description:
   Perform tasks which may fail when the class CLoopbackDevice
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

    hr = CreateUSBDevice();
    IfFailHrGo(hr);

    hr = ConfigureDevice();
    IfFailHrGo(hr);

Exit:
    return hr;
}

void CLoopbackDevice::FinalRelease()
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
}

HRESULT CLoopbackDevice::CreateUSBDevice()
/*++
Routine Description:
   Creates the USB device and initializes the device
   member variables and creates and initializes the 
   device qualifier. The device qualifier is required
   for USB2.0 devices.

Arguments:
    None

Return value:
    From called functions
--*/
{
    HRESULT hr = S_OK;
    ISoftUSBDeviceQualifier *piDeviceQual = NULL;
    USHORT usProductId = 0x930A; //product id for BULK USB

    hr = ::CoCreateInstance(CLSID_SoftUSBDevice, 
                            NULL,
                            CLSCTX_INPROC_SERVER,
                            __uuidof(ISoftUSBDevice),     
                            reinterpret_cast<void**>(&m_piSoftUSBDevice));

    IfFailHrGo(hr);

    //Create the device qualifer
    hr = ::CoCreateInstance(CLSID_SoftUSBDeviceQualifier, 
                            NULL,
                            CLSCTX_INPROC_SERVER,
                            __uuidof(ISoftUSBDeviceQualifier),     
                            reinterpret_cast<void**>(&piDeviceQual));

    IfFailHrGo(hr);

    //Setup the device qualifier
    IfFailHrGo(piDeviceQual->put_USB(0x0200)); //binary coded decimal USB version 2.0
    IfFailHrGo(piDeviceQual->put_DeviceClass(0xff)); //FF=Vendor specfic device class
    IfFailHrGo(piDeviceQual->put_DeviceSubClass(0xff)); //FF = Vendor specific device sub-class
    IfFailHrGo(piDeviceQual->put_DeviceProtocol(0xff)); //FF = Vendor specific device protocol
    IfFailHrGo(piDeviceQual->put_MaxPacketSize0(64)); //max packet size endpoint 0
    IfFailHrGo(piDeviceQual->put_NumConfigurations(1)); //Number of configurations


    //Setup the device 
    IfFailHrGo(m_piSoftUSBDevice->put_USB(0x0200)); //binary coded decimal USB version 2.0
    IfFailHrGo(m_piSoftUSBDevice->put_DeviceClass(0xff)); //FF=Vendor specfic device class
    IfFailHrGo(m_piSoftUSBDevice->put_DeviceSubClass(0xff)); //FF = Vendor specific device sub-class
    IfFailHrGo(m_piSoftUSBDevice->put_DeviceProtocol(0xff)); //FF = Vendor specific device protocol
    IfFailHrGo(m_piSoftUSBDevice->put_MaxPacketSize0(64)); //max packet size endpoint 0
    IfFailHrGo(m_piSoftUSBDevice->put_Vendor(0x045E)); //Vendor ID - Microsoft
    IfFailHrGo(m_piSoftUSBDevice->put_Product((SHORT)usProductId)); //product id - BulkUSB
    IfFailHrGo(m_piSoftUSBDevice->put_Device(0x0100));//Binary decimal coded version 1.0
    IfFailHrGo(m_piSoftUSBDevice->put_RemoteWakeup(VARIANT_FALSE)); //Device does not suppport remote wake up
    IfFailHrGo(m_piSoftUSBDevice->put_Manufacturer(STRING_IDX_MANUFACTURER)); //Index of the manufacturer string
    IfFailHrGo(m_piSoftUSBDevice->put_ProductDesc(STRING_IDX_PRODUCT_DESC)); //Index of the product descripton string
    IfFailHrGo(m_piSoftUSBDevice->put_HasExternalPower(VARIANT_TRUE)); // Indicate that the device has power


    //Create the strings associated with the device
    IfFailHrGo(CreateStrings());

    //Add the device qualifier
    IfFailHrGo(m_piSoftUSBDevice->put_DeviceQualifier(piDeviceQual));

Exit:
    RELEASE(piDeviceQual);
    return hr;
}


HRESULT CLoopbackDevice::ConfigureConfig(ISoftUSBConfiguration * piConfig)
/*++
Routine Description:
    Initialize the USB configuration data 

Arguments:
    piConfig - pointer the SoftUSBCondiguration interface to be initialized

Return value:
    from called function.
--*/
{
    HRESULT          hr = S_OK;
    USBCONFIGATTRIBS ConfigAttribs; ConfigAttribs.Byte = 0;

    IfFailHrGo(piConfig->put_ConfigurationValue(1));                // Config number passed by host in SET_CONFIGURATION request
    IfFailHrGo(piConfig->put_Configuration((BYTE)m_iConfigString)); // Index of string descriptor
    IfFailHrGo(piConfig->put_MaxPower(0));                          // Max bus power consumption in 2mA units

    ConfigAttribs.Bits.bRemoteWakeup = 0;                           // Device does not do remote wakeup
    ConfigAttribs.Bits.bSelfPowered = 1;                            // Device is self-powered
    IfFailHrGo(piConfig->put_Attributes(ConfigAttribs.Byte));

Exit:
    return hr;
}

HRESULT CLoopbackDevice::ConfigureINEndpoint()
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

    IfFailHrGo(m_piINEndpoint->put_EndpointAddress(0x81)); // Endpoint #1 IN 
    IfFailHrGo(m_piINEndpoint->put_Attributes(0x02)); //Bulk data endpoint
    IfFailHrGo(m_piINEndpoint->put_MaxPacketSize(1024));
    IfFailHrGo(m_piINEndpoint->put_Interval(0));
    IfFailHrGo(m_piINEndpoint->put_Halted(FALSE));
    
Exit:    
    return hr;
}

HRESULT CLoopbackDevice::ConfigureOUTEndpoint()
/*++
Routine Description:
    Initializes the OUT Endpoint interface  

Arguments:
    None

Return value:
    E_UNEXPETED if the OUT endpoint is NULL, otherwise
    From called function.
--*/
{
    HRESULT hr = S_OK;

    if (NULL == m_piOUTEndpoint)
    {
        IfFailHrGo(E_UNEXPECTED);
    }

    IfFailHrGo(m_piOUTEndpoint->put_EndpointAddress(0x02)); // Endpoint #2 OUT
    IfFailHrGo(m_piOUTEndpoint->put_Attributes(0x02)); //Bulk data endpoint
    IfFailHrGo(m_piOUTEndpoint->put_MaxPacketSize(1024));
    IfFailHrGo(m_piOUTEndpoint->put_Interval(0));
    IfFailHrGo(m_piOUTEndpoint->put_Halted(FALSE));
    
Exit:    
    return hr;
}

HRESULT CLoopbackDevice::ConfigureInterface(ISoftUSBInterface * piInterface)
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

    IfFailHrGo(piInterface->put_InterfaceNumber(0));
    IfFailHrGo(piInterface->put_AlternateSetting(0));
    IfFailHrGo(piInterface->put_InterfaceClass(0xFF)); //Vendor specific class code
    IfFailHrGo(piInterface->put_InterfaceSubClass(0xFF)); //Vendor specific sub class code
    IfFailHrGo(piInterface->put_InterfaceProtocol(0xFF)); //Vendor specific protcol
    //Index for string describing the interface
    IfFailHrGo(piInterface->put_Interface((BYTE)m_iInterfaceString));

Exit:
    return hr;
}

HRESULT CLoopbackDevice::ConfigureDevice()
/*++
Routine Description:
   Configure the USB device. Create the objects used by the 
   device and finally connect the OUT Endpoint to the IN Endpoint

Arguments:
    None

Return value:
    From called function.
--*/
{
    HRESULT                 hr               = S_OK;
    ISoftUSBConfiguration  *piConfig         = NULL;
    ISoftUSBInterface      *piInterface      = NULL;
    ISoftUSBConfigurations *piConfigurations = NULL;
    ISoftUSBInterfaces     *piInterfaces     = NULL;
    ISoftUSBEndpoints      *piEndpoints      = NULL; 
    VARIANT                 varIndex; VariantInit(&varIndex);

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

    //Create the OUT Endpoint
    hr = CoCreateInstance(CLSID_SoftUSBEndpoint, 
                          NULL,
                          CLSCTX_INPROC_SERVER,
                          __uuidof(ISoftUSBEndpoint),     
                          reinterpret_cast<void**>(&m_piOUTEndpoint));
    IfFailHrGo(hr);

    //Setup the OUT Endpoint
    IfFailHrGo(ConfigureOUTEndpoint());

    //Create the device interface
    hr = CoCreateInstance(CLSID_SoftUSBInterface, 
                          NULL,
                          CLSCTX_INPROC_SERVER,
                          __uuidof(ISoftUSBInterface),     
                          reinterpret_cast<void**>(&piInterface));
    IfFailHrGo(hr);

    //Setup the device interface
    IfFailHrGo(ConfigureInterface(piInterface));

    //Add the Endpoints to the endpoint collection
    IfFailHrGo(piInterface->get_Endpoints(&piEndpoints));
    IfFailHrGo(piEndpoints->Add(reinterpret_cast<SoftUSBEndpoint*>(m_piINEndpoint), varIndex));
    IfFailHrGo(piEndpoints->Add(reinterpret_cast<SoftUSBEndpoint*>(m_piOUTEndpoint), varIndex));

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
    IfFailHrGo(piConfig->get_Interfaces(&piInterfaces));
    IfFailHrGo(piInterfaces->Add(reinterpret_cast<SoftUSBInterface*>(piInterface), varIndex));

    //Add the configuration to the configuration collection
    IfFailHrGo(m_piSoftUSBDevice->get_Configurations(&piConfigurations));
    IfFailHrGo(piConfigurations->Add(reinterpret_cast<SoftUSBConfiguration*>(piConfig), varIndex));

Exit:
    RELEASE(piConfig);
    RELEASE(piInterface);
    RELEASE(piConfigurations);
    RELEASE(piInterfaces);
    RELEASE(piEndpoints);
    return hr;
}

HRESULT CLoopbackDevice::CreateStrings()
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
    HRESULT             hr = S_OK;
    ISoftUSBStrings    *piStrings = NULL;
    ISoftUSBString     *piStringManufacturer = NULL;
    ISoftUSBString     *piStringProductDesc = NULL;
    ISoftUSBString     *piStringConfig = NULL;
    ISoftUSBString     *piStringEndpoint = NULL;
    BSTR                bstrManufacturer = ::SysAllocString(L"Microsoft Corporation");
    BSTR                bstrProductDesc = ::SysAllocString(L"Simulated Generic USB device");
    BSTR                bstrConfig = ::SysAllocString(L"Configuration with a single interface");
    BSTR                bstrEndpoint = ::SysAllocString(L"Interface with bulk IN endpoint and bulk OUT endpoint");
    VARIANT             varIndex; VariantInit(&varIndex);

    // Check that all BSTR allocations succeeded
    IfFalseHrGo(0 != ::SysStringLen(bstrManufacturer), E_OUTOFMEMORY);
    IfFalseHrGo(0 != ::SysStringLen(bstrProductDesc), E_OUTOFMEMORY);
    IfFalseHrGo(0 != ::SysStringLen(bstrConfig), E_OUTOFMEMORY);
    IfFalseHrGo(0 != ::SysStringLen(bstrEndpoint), E_OUTOFMEMORY);

    //Set up the varaint used as the index
    varIndex.vt = VT_I4;
    varIndex.lVal = STRING_IDX_MANUFACTURER;

    //Create and initialize the string descriptors. Also create a string 
    //descriptor index for each. This index is used both to set the string's
    //descriptors position in the m_piSoftUSBDevice.Strings and is the index value 
    //the GetDescriptors request from the host. Note that we don't use 
    //string descriptor index zero because that is a reserved value for a 
    //device's language ID descriptor.

    //Get the string collection from the device
    hr = m_piSoftUSBDevice->get_Strings(&piStrings);
    IfFailHrGo(hr);

    hr = CoCreateInstance(CLSID_SoftUSBString,
                          NULL,
                          CLSCTX_INPROC_SERVER,
                          __uuidof(ISoftUSBString),     
                          reinterpret_cast<void**>(&piStringManufacturer));
    IfFailHrGo(hr);

    IfFailHrGo(piStringManufacturer->put_Value(bstrManufacturer));
    IfFailHrGo(piStrings->Add(reinterpret_cast<SoftUSBString*>(piStringManufacturer), varIndex));
    
    hr = CoCreateInstance(CLSID_SoftUSBString,
                          NULL,
                          CLSCTX_INPROC_SERVER,
                          __uuidof(ISoftUSBString),     
                          reinterpret_cast<void**>(&piStringProductDesc));

    IfFailHrGo(hr);
    IfFailHrGo(piStringProductDesc->put_Value(bstrProductDesc));
    varIndex.lVal = STRING_IDX_PRODUCT_DESC;
    IfFailHrGo(piStrings->Add(reinterpret_cast<SoftUSBString*>(piStringProductDesc), varIndex));

    hr = CoCreateInstance(CLSID_SoftUSBString,
                          NULL,
                          CLSCTX_INPROC_SERVER,
                          __uuidof(ISoftUSBString),     
                          reinterpret_cast<void**>(&piStringConfig));
    IfFailHrGo(hr);
    IfFailHrGo(piStringConfig->put_Value(bstrConfig));
    varIndex.lVal = STRING_IDX_CONFIG;
    m_iConfigString = varIndex.lVal;
    IfFailHrGo(piStrings->Add(reinterpret_cast<SoftUSBString*>(piStringConfig), varIndex));

    hr = CoCreateInstance(CLSID_SoftUSBString,
                          NULL,
                          CLSCTX_INPROC_SERVER,
                          __uuidof(ISoftUSBString),     
                          reinterpret_cast<void**>(&piStringEndpoint));
    IfFailHrGo(hr);
    IfFailHrGo(piStringEndpoint->put_Value(bstrEndpoint));
    varIndex.lVal = STRING_IDX_INTERFACE;
    m_iInterfaceString = varIndex.lVal;
    IfFailHrGo(piStrings->Add(reinterpret_cast<SoftUSBString*>(piStringEndpoint), varIndex));

Exit:
    RELEASE(piStrings);
    RELEASE(piStringManufacturer);
    RELEASE(piStringProductDesc);
    RELEASE(piStringConfig);
    RELEASE(piStringEndpoint);
    ::SysFreeString(bstrManufacturer);
    ::SysFreeString(bstrProductDesc);
    ::SysFreeString(bstrConfig);
    ::SysFreeString(bstrEndpoint);

    return hr;
}

HRESULT CLoopbackDevice::ReleaseConnectionPoint()
/*++
Routine Description:
   Release the connection point to the OUT Endpoint
   if one has been established.

Arguments:
    None

Return value:
    S_OK
--*/
{
    HRESULT hr = S_OK;
    
    if (NULL != m_piConnectionPoint)
    {
        m_piConnectionPoint->Unadvise(m_dwConnectionCookie);
        m_dwConnectionCookie = 0;
    }

    RELEASE(m_piConnectionPoint);

    return hr;
}


HRESULT CLoopbackDevice::SetupConnectionPoint(IUnknown *punkObject,
                                              REFIID    iidConnectionPoint)
/*++
Routine Description:
   Setup the connection point to the OUT Endpoint.
   It validates that the punkObject supports IConnectionPointContainer
   then finds the correct connection point and attaches this object
   as the event sink.

Arguments:
    punkObject - Object which implement IConnectionPointContainer

    iidConnectionPoint - IID of the connection point

Return value:
    S_OK if the connection point is correctly set up
    E_UNEXPECTED if the IUknown of this can not be obtained
    Otherwise from called functions.
--*/
{
    HRESULT hr = S_OK;
    IConnectionPointContainer *piConnectionPointContainer = NULL;
    IUnknown                  *punkSink = NULL;

    //If there is already connection point enabled, disable it
    if(NULL != m_piConnectionPoint)
    {
        IfFailHrGo(ReleaseConnectionPoint());
    }
        
    IfFailHrGo(punkObject->QueryInterface(IID_IConnectionPointContainer,
                                          reinterpret_cast<void **>(&piConnectionPointContainer)));

    IfFailHrGo(piConnectionPointContainer->FindConnectionPoint(iidConnectionPoint,
                                                               &m_piConnectionPoint));

    //Get the IUknown of this interface as this is the event sink
    punkSink = (this)->GetUnknown(); 

    if(NULL == punkSink)
    {
        IfFailHrGo(E_UNEXPECTED);
    }

    IfFailHrGo(m_piConnectionPoint->Advise(punkSink, &m_dwConnectionCookie));

Exit:
    RELEASE(piConnectionPointContainer);

    return hr;
}

STDMETHODIMP CLoopbackDevice::get_DSFDevice(DSFDevice** ppDSFDevice)
/*++
Routine Description:
   Implement the get property DSFDevice for the interface
   ILoopbackDevice.

Arguments:
    ppDSFDevice - address of a pointer to receive the DSFDevice.

Return value:
    E_FAIL if the USB device does not exist
    E_POINTER if ppDSFDevice is not a valid pointer
    Otherwise from called function
--*/
{
    HRESULT     hr = S_OK;


    IfFalseHrGo(NULL != ppDSFDevice, E_POINTER);
    *ppDSFDevice = NULL;

    // Validate the the USB device exists else this is an internal error
    IfFalseHrGo(NULL != m_piSoftUSBDevice, E_FAIL);

    IfFailHrGo(m_piSoftUSBDevice->get_DSFDevice(ppDSFDevice));

Exit:

    return hr;
}


STDMETHODIMP CLoopbackDevice::DoPolledLoopback(long lTimeInterval)
/*++
Routine Description:
   Demonstrates how to use the drain OUT queue and queue IN data
   methods to communicate with the host controller. 

   The code checks to see if there is any data in the OUT, if no 
   data is present an event is fired to indicate if the function 
   should exit. If the function should not exit then the function 
   sleeps for the time interval before re-checking the queue.

   If there is data then the function reads the data and passes the
   data to the IN queue. This simply provides a loopback mechanism
   to the host controller.

Arguments:
    lTimeInterval - time to sleep between checking if the out
                    queue has data if none is present. This time
                    is indicated in milliseconds.

Return value:
    S_OK if the data is read/written correctly
    E_UNEXPECTED if the IN or OUT endpoints are NULL.
    otherwise from called function
--*/
{
    HRESULT               hr                = S_OK;
    BOOL                  fKeepLooping      = TRUE;
    ULONG                 ulNoOfQueuedItems = 0; //Number of items currently in the queue
    ULONG                 ulTransfers       = 1; //Only going to read one transfer at a time
    SOFTUSB_OUT_TRANSFER *pOUTTransfer      = NULL;
    BYTE                  bStatus           = 0; //Copied the message status
    BYTE                 *pDataBuffer       = NULL; //Copied the message data
    ULONG                 cbDataBuffer      = 0; //Holds the size of the data buffer
    VARIANT_BOOL          fvarContinue      = VARIANT_TRUE;

    if (NULL == m_piINEndpoint || NULL == m_piOUTEndpoint)
    {
        IfFailHrGo(E_UNEXPECTED);
    }

    while (fKeepLooping)
    {
        //Reset the number of queued items
        ulNoOfQueuedItems = 0;
        
        //Check to see if there is any data in the out queue
        IfFailHrGo(m_piOUTEndpoint->DrainOUTQueue(0, &ulNoOfQueuedItems, NULL));

        if (0 == ulNoOfQueuedItems)
        {
            //There is no data in the list so we need to check
            //If we should continue to loop
            //Fire Event to check if more processing is required
            IfFailHrGo(Fire_ContinueToPoll(&fvarContinue));

            //Check to see if the return value is VARIANT_FALSE
            if (VARIANT_FALSE == fvarContinue)
            {
                fKeepLooping = FALSE;
            }
            
            if (fKeepLooping)
            {
                //
                ::Sleep(lTimeInterval);
            }        
        }
        else
        {
            //There is data to read, loop until we have moved all 
            //the data from the OUT queue to the IN queue moving
            //one data item at a time
            do
            {
                //Get the OUT data
                IfFailHrGo(m_piOUTEndpoint->DrainOUTQueue(ulTransfers, 
                                                          &ulNoOfQueuedItems, 
                                                          &pOUTTransfer));

                //Setup the IN data
                bStatus= pOUTTransfer->bStatus;
                cbDataBuffer = pOUTTransfer->cbData;
                pDataBuffer =&pOUTTransfer->Data[0];
                
                //Send the data to the out queue
                IfFailHrGo(m_piINEndpoint->QueueINData(pDataBuffer,
                                                       cbDataBuffer,
                                                       bStatus,
                                                       SOFTUSB_FOREVER));

                //Free the memory used by pOUTTransfer   
                m_piOUTEndpoint->FreeOUTQueue(pOUTTransfer);
                pOUTTransfer = NULL;

                //Force a context switch 
                ::Sleep(1);
            }while (0 != ulNoOfQueuedItems);
        }

    }

Exit:
    //If one of the calls failed pOUTTransfer will be NON-NULL 
    //And needs to be freed
    if (NULL != pOUTTransfer)
    {
        //Free the memory used by pOUTTransfer   
        m_piOUTEndpoint->FreeOUTQueue(pOUTTransfer);
        pOUTTransfer = NULL;
    }

    return hr;
}

STDMETHODIMP CLoopbackDevice::StartEventProcessing()
/*++
Routine Description:
   Demonstrates how to setup event sinks so that the 
   event mechanism can be used to control data flow to and
   from the USB controller. In this example an event sink
   is installed on the OUT USB endpoint, when the controller
   has data to send to the device the OnWriteTransfer event
   will fire, this will occur on an arbitrary thread. The 
   device then simply copies this data and passes it the
   IN queue of the IN Endpoint.

Arguments:
    None

Return value:
    From called functions
    
--*/
{
    HRESULT               hr                = S_OK;
    BOOL                  fKeepLooping      = TRUE;
    VARIANT_BOOL          fvarContinue      = VARIANT_TRUE;

    //Set up event sink on the OUT endpoint
    IfFailHrGo(SetupConnectionPoint(m_piOUTEndpoint, __uuidof(ISoftUSBEndpointEvents)));

    //Loop waiting for Events to be fired
    while (TRUE == fKeepLooping)
    {
        //Context switch to allow other threads to process
        ::Sleep(1);

        //Fire Event to check if the caller want to continue processing
        IfFailHrGo(Fire_ContinueEventProcessing(&fvarContinue));

        //Check to see if the return value is VARIANT_FALSE
        if (VARIANT_FALSE == fvarContinue)
        {
            fKeepLooping = FALSE;
        }
    }

    //Remove the event sink from the OUT endpoint
    IfFailHrGo(ReleaseConnectionPoint());
    
Exit:
    return hr;
}

STDMETHODIMP CLoopbackDevice::StartAsyncEventProcessing()
/*++
Routine Description:
   Demonstrates how to setup event sinks so that the event mechanism can
   be used to control data flow to and from the USB controller. In this
   example an event sink is installed on the OUT USB endpoint, when the
   controller has data to send to the device the OnWriteTransfer event
   will fire, this will occur on an arbitrary thread. The device then
   simply copies this data and passes it the IN queue of the IN
   Endpoint. Control returns to the caller and event processing
   continues in an arbitrary thread. To terminate event processing call
   StopAsyncEventProcessing.

Arguments:
    None

Return value:
    From called functions

--*/
{
    HRESULT hr = S_OK;

    // Set up event sink on the OUT endpoint
    IfFailHrGo(SetupConnectionPoint(m_piOUTEndpoint, __uuidof(ISoftUSBEndpointEvents)));

Exit:
    return hr;
}

STDMETHODIMP CLoopbackDevice::StopAsyncEventProcessing()
/*++
Routine Description:
   This method terminates the event processing started by an earlier
   call to StartAsyncEventProcessing.

Arguments:
    None

Return value:
    From called functions

--*/
{
    HRESULT hr = S_OK;

    // Remove the event sink on the OUT endpoint
    IfFailHrGo(ReleaseConnectionPoint());

Exit:
    return hr;
}



STDMETHODIMP CLoopbackDevice::AreKeystrokesWaiting(VARIANT_BOOL *pfvarKeyWaiting)
/*++
Routine Description:
   Implements ISoftUSBLoopback::AreKeystrokesWaiting. It calls the low level 
   IO function _kbhit to see if the keyboard has been struck. If the Keyboard
   has been hit the function return VARIANT_TRUE otherwise it returns VARIANT_FALSE
   
Arguments:
    pfvarKetyWaiting - variant bool to hold the return result

Return value:
    S_OK if the result is returned successfully
    E_POINTER if pfvarKeyWaiting is not valid
    
--*/
{
    HRESULT hr = S_OK;
    int iKeyHit = 0;

    if (NULL == pfvarKeyWaiting)
    {
        IfFailHrGo(E_POINTER);
    }
    
    *pfvarKeyWaiting = VARIANT_FALSE;

    iKeyHit = _kbhit();

    if (0 != iKeyHit)
    {
        *pfvarKeyWaiting = VARIANT_TRUE;
    }

Exit:
    return hr;
}

STDMETHODIMP CLoopbackDevice::put_SerialNumber
(
    BSTR bstrSerialNumber
)
/*++
Routine Description:
   Implement the put property SerialNumber for the interface
   ILoopbackDevice.

Arguments:
   bstrSerialNumber - New value for the serial number property

Return value:
    E_FAIL if the underlying USB device does not exist
    Otherwise from called function
--*/
{
    HRESULT          hr                   = S_OK;
    BYTE             bSerialNumber        = 0;
    ISoftUSBStrings *piStrings            = NULL;
    ISoftUSBString  *piSerialNumberString = NULL;
    LONG             lCount               = 0;
    VARIANT          varIndex; ::VariantInit(&varIndex);

    // Validate the the USB device exists else this is an internal error
    IfFalseHrGo(NULL != m_piSoftUSBDevice, E_FAIL);

    //Get the current serial number of the device
    IfFailHrGo(m_piSoftUSBDevice->get_SerialNumber(&bSerialNumber));

    //Get the string collection from the device
    IfFailHrGo(m_piSoftUSBDevice->get_Strings(&piStrings));

    //Check if there is a currently allocated serial number
    if (0 == bSerialNumber)
    {
        //String does not exist need to create one
        hr = CoCreateInstance(CLSID_SoftUSBString,
                          NULL,
                          CLSCTX_INPROC_SERVER,
                          __uuidof(ISoftUSBString),     
                          reinterpret_cast<void**>(&piSerialNumberString));
        IfFailHrGo(hr);

        //Get the current number of items in the collection
        IfFailHrGo(piStrings->get_Count(&lCount));

        //Add the serial number string as the next item in the colletion
        lCount++;
        varIndex.vt = VT_I4;
        varIndex.lVal = lCount;

        IfFailHrGo(piStrings->Add(reinterpret_cast<SoftUSBString*>(piSerialNumberString), 
                                  varIndex));

        //Write the serial number location back to the device
        bSerialNumber = (BYTE)lCount;
        IfFailHrGo(m_piSoftUSBDevice->put_SerialNumber(bSerialNumber));
    }
    else
    {
        varIndex.vt = VT_I4;
        varIndex.lVal = bSerialNumber;
        
        //Get the string from the collection
        IfFailHrGo(piStrings->get_Item(varIndex,
                      reinterpret_cast<SoftUSBString**>(&piSerialNumberString)));
    }

    //Write the value to the serial number string
    IfFailHrGo(piSerialNumberString->put_Value(bstrSerialNumber));

Exit:
    RELEASE(piStrings);
    RELEASE(piSerialNumberString);

    return hr;
}


//ISoftUSBEndpointEvents

STDMETHODIMP CLoopbackDevice::OnSetupTransfer(BYTE DataToggle,BYTE *pbDataBuffer,
                               ULONG cbDataBuffer, BYTE *pbStatus)
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
    HRESULT hr = E_NOTIMPL;

    UNREFERENCED_PARAMETER(DataToggle);
    UNREFERENCED_PARAMETER(pbDataBuffer);
    UNREFERENCED_PARAMETER(cbDataBuffer);
    UNREFERENCED_PARAMETER(pbStatus);

    return hr;
}

STDMETHODIMP CLoopbackDevice::OnWriteTransfer(BYTE DataToggle, BYTE *pbDataBuffer,
                               ULONG cbDataBuffer, BYTE *pbStatus)
/*++
Routine Description:
   Implements the OnWriteTransfer for the interface ISoftUSBEndpointEvent

Arguments:
    DataToggle - DataToggle value
    pbDataBuffer - pointer to the buffer which contains the data
    cbDataBuffer - size of the data buffer
    pbStatus - pointer to transfer status value

Return value:
    E_UNEXPECTED if the IN endpoint is not valit
    Otherwise form call function
--*/
{
    
    HRESULT hr        = S_OK;
    BYTE    bINStatus = USB_ACK;

    UNREFERENCED_PARAMETER(DataToggle);

    // Check that the IN endpoint is valid
    if (NULL == m_piINEndpoint)
    {
        IfFailHrGo(E_UNEXPECTED);
    }

    // Send the data to the IN Endpoint
    IfFailHrGo(m_piINEndpoint->QueueINData(pbDataBuffer,
                                           cbDataBuffer,
                                           bINStatus,
                                           SOFTUSB_FOREVER));

    //ACK the status as the data was successfully sent to the IN endpoint
    *pbStatus = USB_ACK;

Exit:
    if (FAILED(hr))
    {
        *pbStatus = USB_STALL;
    }
    return hr;
}

STDMETHODIMP CLoopbackDevice::OnReadTransfer(BYTE DataToggle, BYTE  *pbDataBuffer,
                              ULONG   cbDataBuffer,ULONG *cbDataWritten,
                              BYTE *pbStatus)       
/*++
Routine Description:
   Implements the OnReadTransfer for the interface ISoftUSBEndpointEvent

Arguments:
    DataToggle - DataToggle value
    pbDataBuffer - pointer to the data beffer
    cbDataBuffer - size of the data buffer
    cbDataWritten - amount of data actually written to the buffer
    pbStatus - pointer to transfer status value

Return value:
    E_NOTIMPL as this event is not handled by the device
--*/
{
    HRESULT hr = E_NOTIMPL;

    UNREFERENCED_PARAMETER(DataToggle);
    UNREFERENCED_PARAMETER(pbDataBuffer);
    UNREFERENCED_PARAMETER(cbDataBuffer);
    UNREFERENCED_PARAMETER(cbDataWritten);
    UNREFERENCED_PARAMETER(pbStatus);

    return hr;
}

STDMETHODIMP CLoopbackDevice::OnDeviceRequest(USBSETUPREQUEST *pSetupRequest,
                               ULONG *RequestHandle, BYTE *pbHostData,
                               ULONG  cbHostData, BYTE **ppbResponseData,
                               ULONG *pcbResponseData,BYTE  *pbSetupStatus)
/*++
Routine Description:
   Implements the OnDeviceRequest for the interface ISoftUSBEndpointEvent

Arguments:
    pSetupRequest - pointer to the setup request begin handled
    RequestHandle - Handle fro the request
    pbHostData - pointer to a buffer which contain data sebt from the
                 host to the device for an OUT setup transfer
    cbHostData - amount of data in the HostData buffer
    ppbReponseData - pointer to data buffer to hold the response data
    pcbResponseData - pointer to the size of the data buffer
    pbSetpStatus - pointer to the setup status

Return value:
    E_NOTIMPL as this event is not handled by the device
--*/
{
    HRESULT hr = E_NOTIMPL;

    UNREFERENCED_PARAMETER(pSetupRequest);
    UNREFERENCED_PARAMETER(RequestHandle);
    UNREFERENCED_PARAMETER(pbHostData);
    UNREFERENCED_PARAMETER(cbHostData);
    UNREFERENCED_PARAMETER(ppbResponseData);
    UNREFERENCED_PARAMETER(pcbResponseData);
    UNREFERENCED_PARAMETER(pbSetupStatus);

    return hr;
}

STDMETHODIMP CLoopbackDevice::OnDeviceRequestComplete(ULONG RequestHandle,
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
    HRESULT hr = E_NOTIMPL;

    UNREFERENCED_PARAMETER(RequestHandle);
    UNREFERENCED_PARAMETER(pbFinalRequestStatus);

    return hr;
}

