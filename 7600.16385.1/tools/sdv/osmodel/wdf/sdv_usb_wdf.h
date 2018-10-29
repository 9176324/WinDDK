#include <usbdi.h>
#include <usbdlib.h>
#include <usbdrivr.h>
#include <wdfusb.h>


#define WdfUsbTargetDeviceRetrieveInformation sdv_WdfUsbTargetDeviceRetrieveInformation
NTSTATUS
sdv_WdfUsbTargetDeviceRetrieveInformation(
   WDFUSBDEVICE UsbDevice,
   PWDF_USB_DEVICE_INFORMATION Information
    );
	
#define WdfUsbTargetDeviceGetDeviceDescriptor sdv_WdfUsbTargetDeviceGetDeviceDescriptor
VOID
sdv_WdfUsbTargetDeviceGetDeviceDescriptor(
    WDFUSBDEVICE UsbDevice,
    PUSB_DEVICE_DESCRIPTOR UsbDeviceDescriptor
    );

#define WdfUsbTargetDeviceSelectConfig sdv_WdfUsbTargetDeviceSelectConfig
NTSTATUS
sdv_WdfUsbTargetDeviceSelectConfig(
    WDFUSBDEVICE UsbDevice,
    PWDF_OBJECT_ATTRIBUTES PipeAttributes,
    PWDF_USB_DEVICE_SELECT_CONFIG_PARAMS Params
   );

#define WdfUsbTargetPipeGetIoTarget sdv_WdfUsbTargetPipeGetIoTarget
WDFIOTARGET
sdv_WdfUsbTargetPipeGetIoTarget(
    IN WDFUSBPIPE Pipe
    );

#define WdfUsbTargetPipeGetInformation sdv_WdfUsbTargetPipeGetInformation
VOID
sdv_WdfUsbTargetPipeGetInformation(
    IN WDFUSBPIPE Pipe,
    OUT PUSBD_PIPE_INFORMATION PipeInformation
    );



#ifdef WdfUsbTargetDeviceCreate
#undef WdfUsbTargetDeviceCreate
#endif
#define WdfUsbTargetDeviceCreate sdv_WdfUsbTargetDeviceCreate
NTSTATUS  sdv_WdfUsbTargetDeviceCreate(  
__in  WDFDEVICE  Device,
__in_opt  PWDF_OBJECT_ATTRIBUTES  Attributes,
__out  WDFUSBDEVICE*  UsbDevice);




#ifdef WdfUsbTargetDeviceRetrieveConfigDescriptor
#undef WdfUsbTargetDeviceRetrieveConfigDescriptor
#endif
#define WdfUsbTargetDeviceRetrieveConfigDescriptor sdv_WdfUsbTargetDeviceRetrieveConfigDescriptor
NTSTATUS  sdv_WdfUsbTargetDeviceRetrieveConfigDescriptor(  
__in  WDFUSBDEVICE  UsbDevice,
__out_bcount_full_opt(*ConfigDescriptorLength)  PVOID  ConfigDescriptor,
__inout  PUSHORT  ConfigDescriptorLength);


#ifdef WdfUsbTargetDeviceQueryString
#undef WdfUsbTargetDeviceQueryString
#endif
#define WdfUsbTargetDeviceQueryString sdv_WdfUsbTargetDeviceQueryString
NTSTATUS  sdv_WdfUsbTargetDeviceQueryString(  
__in  WDFUSBDEVICE  UsbDevice,
__in_opt  WDFREQUEST  Request,
__in_opt  PWDF_REQUEST_SEND_OPTIONS  RequestOptions,
__out_ecount_opt(*NumCharacters)  PUSHORT  String,
__inout  PUSHORT  NumCharacters,
   UCHAR  StringIndex,
   USHORT  LangID);


#ifdef WdfUsbTargetDeviceAllocAndQueryString
#undef WdfUsbTargetDeviceAllocAndQueryString
#endif
#define WdfUsbTargetDeviceAllocAndQueryString sdv_WdfUsbTargetDeviceAllocAndQueryString
NTSTATUS  sdv_WdfUsbTargetDeviceAllocAndQueryString(  
__in  WDFUSBDEVICE  UsbDevice,
__in_opt  PWDF_OBJECT_ATTRIBUTES  StringMemoryAttributes,
__out  WDFMEMORY*  StringMemory,
__out_opt  PUSHORT  NumCharacters,
   UCHAR  StringIndex,
   USHORT  LangID);


#ifdef WdfUsbTargetDeviceFormatRequestForString
#undef WdfUsbTargetDeviceFormatRequestForString
#endif
#define WdfUsbTargetDeviceFormatRequestForString sdv_WdfUsbTargetDeviceFormatRequestForString
NTSTATUS  sdv_WdfUsbTargetDeviceFormatRequestForString(  
__in  WDFUSBDEVICE  UsbDevice,
__in  WDFREQUEST  Request,
__in  WDFMEMORY  Memory,
__in_opt  PWDFMEMORY_OFFSET  Offset,
   UCHAR  StringIndex,
   USHORT  LangID);


#ifdef WdfUsbTargetDeviceGetNumInterfaces
#undef WdfUsbTargetDeviceGetNumInterfaces
#endif
#define WdfUsbTargetDeviceGetNumInterfaces sdv_WdfUsbTargetDeviceGetNumInterfaces
UCHAR  sdv_WdfUsbTargetDeviceGetNumInterfaces(  
__in  WDFUSBDEVICE  UsbDevice);


#ifdef WdfUsbTargetDeviceRetrieveCurrentFrameNumber
#undef WdfUsbTargetDeviceRetrieveCurrentFrameNumber
#endif
#define WdfUsbTargetDeviceRetrieveCurrentFrameNumber sdv_WdfUsbTargetDeviceRetrieveCurrentFrameNumber
NTSTATUS  sdv_WdfUsbTargetDeviceRetrieveCurrentFrameNumber(  
__in  WDFUSBDEVICE  UsbDevice,
__out  PULONG  CurrentFrameNumber);



#ifdef WdfUsbTargetDeviceIsConnectedSynchronous
#undef WdfUsbTargetDeviceIsConnectedSynchronous
#endif
#define WdfUsbTargetDeviceIsConnectedSynchronous sdv_WdfUsbTargetDeviceIsConnectedSynchronous
NTSTATUS  sdv_WdfUsbTargetDeviceIsConnectedSynchronous(  
__in  WDFUSBDEVICE  UsbDevice);


#ifdef WdfUsbTargetDeviceResetPortSynchronously
#undef WdfUsbTargetDeviceResetPortSynchronously
#endif
#define WdfUsbTargetDeviceResetPortSynchronously sdv_WdfUsbTargetDeviceResetPortSynchronously
NTSTATUS  sdv_WdfUsbTargetDeviceResetPortSynchronously(  
__in  WDFUSBDEVICE  UsbDevice);


#ifdef WdfUsbTargetDeviceCyclePortSynchronously
#undef WdfUsbTargetDeviceCyclePortSynchronously
#endif
#define WdfUsbTargetDeviceCyclePortSynchronously sdv_WdfUsbTargetDeviceCyclePortSynchronously
NTSTATUS  sdv_WdfUsbTargetDeviceCyclePortSynchronously(  
__in  WDFUSBDEVICE  UsbDevice);


#ifdef WdfUsbTargetDeviceFormatRequestForCyclePort
#undef WdfUsbTargetDeviceFormatRequestForCyclePort
#endif
#define WdfUsbTargetDeviceFormatRequestForCyclePort sdv_WdfUsbTargetDeviceFormatRequestForCyclePort
NTSTATUS  sdv_WdfUsbTargetDeviceFormatRequestForCyclePort(  
__in  WDFUSBDEVICE  UsbDevice,
__in  WDFREQUEST  Request);



#ifdef WdfUsbTargetDeviceFormatRequestForUrb
#undef WdfUsbTargetDeviceFormatRequestForUrb
#endif
#define WdfUsbTargetDeviceFormatRequestForUrb sdv_WdfUsbTargetDeviceFormatRequestForUrb
NTSTATUS  sdv_WdfUsbTargetDeviceFormatRequestForUrb(  
__in  WDFUSBDEVICE  UsbDevice,
__in  WDFREQUEST  Request,
__in  WDFMEMORY  UrbMemory,
__in_opt  PWDFMEMORY_OFFSET  UrbMemoryOffset);



#ifdef WdfUsbTargetPipeIsInEndpoint
#undef WdfUsbTargetPipeIsInEndpoint
#endif
#define WdfUsbTargetPipeIsInEndpoint sdv_WdfUsbTargetPipeIsInEndpoint
BOOLEAN  sdv_WdfUsbTargetPipeIsInEndpoint(  
__in  WDFUSBPIPE  Pipe);


#ifdef WdfUsbTargetPipeIsOutEndpoint
#undef WdfUsbTargetPipeIsOutEndpoint
#endif
#define WdfUsbTargetPipeIsOutEndpoint sdv_WdfUsbTargetPipeIsOutEndpoint
BOOLEAN  sdv_WdfUsbTargetPipeIsOutEndpoint(  
__in  WDFUSBPIPE  Pipe);



#ifdef WdfUsbTargetPipeSetNoMaximumPacketSizeCheck
#undef WdfUsbTargetPipeSetNoMaximumPacketSizeCheck
#endif
#define WdfUsbTargetPipeSetNoMaximumPacketSizeCheck(arg1) sdv_WdfUsbTargetPipeSetNoMaximumPacketSizeCheck(arg1);
VOID  sdv_WdfUsbTargetPipeSetNoMaximumPacketSizeCheck(  
__in  WDFUSBPIPE  Pipe);


#ifdef WdfUsbTargetPipeWriteSynchronously
#undef WdfUsbTargetPipeWriteSynchronously
#endif
#define WdfUsbTargetPipeWriteSynchronously sdv_WdfUsbTargetPipeWriteSynchronously
NTSTATUS  sdv_WdfUsbTargetPipeWriteSynchronously(  
__in  WDFUSBPIPE  Pipe,
__in_opt  WDFREQUEST  Request,
__in_opt  PWDF_REQUEST_SEND_OPTIONS  RequestOptions,
__in_opt  PWDF_MEMORY_DESCRIPTOR  MemoryDescriptor,
__out_opt  PULONG  BytesWritten);


#ifdef WdfUsbTargetPipeFormatRequestForWrite
#undef WdfUsbTargetPipeFormatRequestForWrite
#endif
#define WdfUsbTargetPipeFormatRequestForWrite sdv_WdfUsbTargetPipeFormatRequestForWrite
NTSTATUS  sdv_WdfUsbTargetPipeFormatRequestForWrite(  
__in  WDFUSBPIPE  Pipe,
__in  WDFREQUEST  Request,
__in_opt  WDFMEMORY  WriteMemory,
__in_opt  PWDFMEMORY_OFFSET  WriteOffset);


#ifdef WdfUsbTargetPipeReadSynchronously
#undef WdfUsbTargetPipeReadSynchronously
#endif
#define WdfUsbTargetPipeReadSynchronously sdv_WdfUsbTargetPipeReadSynchronously
NTSTATUS  sdv_WdfUsbTargetPipeReadSynchronously(  
__in  WDFUSBPIPE  Pipe,
__in_opt  WDFREQUEST  Request,
__in_opt  PWDF_REQUEST_SEND_OPTIONS  RequestOptions,
__in_opt  PWDF_MEMORY_DESCRIPTOR  MemoryDescriptor,
__out_opt  PULONG  BytesRead);


#ifdef WdfUsbTargetPipeFormatRequestForRead
#undef WdfUsbTargetPipeFormatRequestForRead
#endif
#define WdfUsbTargetPipeFormatRequestForRead sdv_WdfUsbTargetPipeFormatRequestForRead
NTSTATUS  sdv_WdfUsbTargetPipeFormatRequestForRead(  
__in  WDFUSBPIPE  Pipe,
__in  WDFREQUEST  Request,
__in_opt  WDFMEMORY  ReadMemory,
__in_opt  PWDFMEMORY_OFFSET  ReadOffset);



#ifdef WdfUsbTargetPipeAbortSynchronously
#undef WdfUsbTargetPipeAbortSynchronously
#endif
#define WdfUsbTargetPipeAbortSynchronously sdv_WdfUsbTargetPipeAbortSynchronously
NTSTATUS  sdv_WdfUsbTargetPipeAbortSynchronously(  
__in  WDFUSBPIPE  Pipe,
__in_opt  WDFREQUEST  Request,
__in_opt  PWDF_REQUEST_SEND_OPTIONS  RequestOptions);


#ifdef WdfUsbTargetPipeFormatRequestForAbort
#undef WdfUsbTargetPipeFormatRequestForAbort
#endif
#define WdfUsbTargetPipeFormatRequestForAbort sdv_WdfUsbTargetPipeFormatRequestForAbort
NTSTATUS  sdv_WdfUsbTargetPipeFormatRequestForAbort(  
__in  WDFUSBPIPE  Pipe,
__in  WDFREQUEST  Request);


#ifdef WdfUsbTargetPipeResetSynchronously
#undef WdfUsbTargetPipeResetSynchronously
#endif
#define WdfUsbTargetPipeResetSynchronously sdv_WdfUsbTargetPipeResetSynchronously
NTSTATUS  sdv_WdfUsbTargetPipeResetSynchronously(  
__in  WDFUSBPIPE  Pipe,
__in_opt  WDFREQUEST  Request,
__in_opt  PWDF_REQUEST_SEND_OPTIONS  RequestOptions);


#ifdef WdfUsbTargetPipeFormatRequestForReset
#undef WdfUsbTargetPipeFormatRequestForReset
#endif
#define WdfUsbTargetPipeFormatRequestForReset sdv_WdfUsbTargetPipeFormatRequestForReset
NTSTATUS  sdv_WdfUsbTargetPipeFormatRequestForReset(  
__in  WDFUSBPIPE  Pipe,
__in  WDFREQUEST  Request);



#ifdef WdfUsbTargetPipeFormatRequestForUrb
#undef WdfUsbTargetPipeFormatRequestForUrb
#endif
#define WdfUsbTargetPipeFormatRequestForUrb sdv_WdfUsbTargetPipeFormatRequestForUrb
NTSTATUS  sdv_WdfUsbTargetPipeFormatRequestForUrb(  
__in  WDFUSBPIPE  PIPE,
__in  WDFREQUEST  Request,
__in  WDFMEMORY  UrbMemory,
__in_opt  PWDFMEMORY_OFFSET  UrbMemoryOffset);


#ifdef WdfUsbInterfaceGetInterfaceNumber
#undef WdfUsbInterfaceGetInterfaceNumber
#endif
#define WdfUsbInterfaceGetInterfaceNumber sdv_WdfUsbInterfaceGetInterfaceNumber
BYTE  sdv_WdfUsbInterfaceGetInterfaceNumber(  
__in  WDFUSBINTERFACE  UsbInterface);


#ifdef WdfUsbInterfaceGetNumEndpoints
#undef WdfUsbInterfaceGetNumEndpoints
#endif
#define WdfUsbInterfaceGetNumEndpoints sdv_WdfUsbInterfaceGetNumEndpoints
BYTE  sdv_WdfUsbInterfaceGetNumEndpoints(  
__in  WDFUSBINTERFACE  UsbInterface,
   UCHAR  SettingIndex);



#ifdef WdfUsbInterfaceGetNumSettings
#undef WdfUsbInterfaceGetNumSettings
#endif
#define WdfUsbInterfaceGetNumSettings sdv_WdfUsbInterfaceGetNumSettings
BYTE  sdv_WdfUsbInterfaceGetNumSettings(  
__in  WDFUSBINTERFACE  UsbInterface);



#ifdef WdfUsbTargetDeviceGetInterface
#undef WdfUsbTargetDeviceGetInterface
#endif
#define WdfUsbTargetDeviceGetInterface sdv_WdfUsbTargetDeviceGetInterface
WDFUSBINTERFACE  sdv_WdfUsbTargetDeviceGetInterface(  
__in  WDFUSBDEVICE  UsbDevice,
   UCHAR  InterfaceIndex);


#ifdef WdfUsbInterfaceGetConfiguredSettingIndex
#undef WdfUsbInterfaceGetConfiguredSettingIndex
#endif
#define WdfUsbInterfaceGetConfiguredSettingIndex sdv_WdfUsbInterfaceGetConfiguredSettingIndex
BYTE  sdv_WdfUsbInterfaceGetConfiguredSettingIndex(  
__in  WDFUSBINTERFACE  Interface);


#ifdef WdfUsbInterfaceGetNumConfiguredPipes
#undef WdfUsbInterfaceGetNumConfiguredPipes
#endif
#define WdfUsbInterfaceGetNumConfiguredPipes sdv_WdfUsbInterfaceGetNumConfiguredPipes
BYTE  sdv_WdfUsbInterfaceGetNumConfiguredPipes(  
__in  WDFUSBINTERFACE  UsbInterface);


#ifdef	WDF_REQUEST_SEND_OPTIONS_SET_TIMEOUT
#undef	WDF_REQUEST_SEND_OPTIONS_SET_TIMEOUT
#endif
#define	WDF_REQUEST_SEND_OPTIONS_SET_TIMEOUT(arg1,arg2)	sdv_WDF_REQUEST_SEND_OPTIONS_SET_TIMEOUT(arg1,arg2)
VOID  
sdv_WDF_REQUEST_SEND_OPTIONS_SET_TIMEOUT(    
	OUT PWDF_REQUEST_SEND_OPTIONS  Options,  
	IN LONGLONG  Timeout    );

#ifdef WDF_REQUEST_SEND_OPTIONS_INIT
#undef WDF_REQUEST_SEND_OPTIONS_INIT
#endif
#define WDF_REQUEST_SEND_OPTIONS_INIT sdv_WDF_REQUEST_SEND_OPTIONS_INIT
VOID  WDF_REQUEST_SEND_OPTIONS_INIT(  
									__out PWDF_REQUEST_SEND_OPTIONS  Options,
									__in ULONG  Flags    
									);
#ifdef WdfDeviceGetIoTarget
#undef WdfDeviceGetIoTarget
#endif
#define WdfDeviceGetIoTarget sdv_WdfDeviceGetIoTarget
WDFIOTARGET  WdfDeviceGetIoTarget(    __in WDFDEVICE  Device    );

#ifdef WdfUsbTargetDeviceWdmGetConfigurationHandle
#undef WdfUsbTargetDeviceWdmGetConfigurationHandle
#endif
#define WdfUsbTargetDeviceWdmGetConfigurationHandle sdv_WdfUsbTargetDeviceWdmGetConfigurationHandle
USBD_CONFIGURATION_HANDLE  sdv_WdfUsbTargetDeviceWdmGetConfigurationHandle(    
	__in WDFUSBDEVICE  UsbDevice    );

#ifdef WdfUsbTargetDeviceSendControlTransferSynchronously
#undef WdfUsbTargetDeviceSendControlTransferSynchronously
#endif
#define WdfUsbTargetDeviceSendControlTransferSynchronously sdv_WdfUsbTargetDeviceSendControlTransferSynchronously
NTSTATUS  sdv_WdfUsbTargetDeviceSendControlTransferSynchronously(    
	__in WDFUSBDEVICE  UsbDevice,    
	__in_opt WDFREQUEST  Request,    
	__in_opt PWDF_REQUEST_SEND_OPTIONS  RequestOptions,    
	__in PWDF_USB_CONTROL_SETUP_PACKET  SetupPacket,    
	__in_opt PWDF_MEMORY_DESCRIPTOR  MemoryDescriptor,    
	__out_opt PULONG  BytesTransferred    );

#ifdef WdfUsbTargetDeviceFormatRequestForControlTransfer
#undef WdfUsbTargetDeviceFormatRequestForControlTransfer
#endif
#define WdfUsbTargetDeviceFormatRequestForControlTransfer sdv_WdfUsbTargetDeviceFormatRequestForControlTransfer
NTSTATUS  sdv_WdfUsbTargetDeviceFormatRequestForControlTransfer(   
	__in WDFUSBDEVICE  UsbDevice,    
	__in WDFREQUEST  Request,  
	__in PWDF_USB_CONTROL_SETUP_PACKET  SetupPacket,   
	__in_opt WDFMEMORY  TransferMemory,    
	__in_opt PWDFMEMORY_OFFSET  TransferOffset    );

#ifdef WdfUsbTargetDeviceSendUrbSynchronously
#undef WdfUsbTargetDeviceSendUrbSynchronously
#endif
#define WdfUsbTargetDeviceSendUrbSynchronously sdv_WdfUsbTargetDeviceSendUrbSynchronously
NTSTATUS  sdv_WdfUsbTargetDeviceSendUrbSynchronously(    
	__in WDFUSBDEVICE  UsbDevice,   
	__in_opt WDFREQUEST  Request,  
	__in_opt PWDF_REQUEST_SEND_OPTIONS  RequestOptions,   
	__in PURB  Urb    );

#ifdef WdfUsbTargetPipeGetType
#undef WdfUsbTargetPipeGetType
#endif
#define WdfUsbTargetPipeGetType sdv_WdfUsbTargetPipeGetType
WDF_USB_PIPE_TYPE  sdv_WdfUsbTargetPipeGetType(    __in WDFUSBPIPE  Pipe    );

#ifdef WdfUsbTargetPipeConfigContinuousReader
#undef WdfUsbTargetPipeConfigContinuousReader
#endif
#define WdfUsbTargetPipeConfigContinuousReader sdv_WdfUsbTargetPipeConfigContinuousReader
NTSTATUS  sdv_WdfUsbTargetPipeConfigContinuousReader(   
	__in WDFUSBPIPE  Pipe,    
	__in PWDF_USB_CONTINUOUS_READER_CONFIG  Config  
	);

#ifdef WdfUsbTargetPipeSendUrbSynchronously
#undef WdfUsbTargetPipeSendUrbSynchronously
#endif
#define WdfUsbTargetPipeSendUrbSynchronously sdv_WdfUsbTargetPipeSendUrbSynchronously
NTSTATUS  sdv_WdfUsbTargetPipeSendUrbSynchronously(    
	__in WDFUSBPIPE  Pipe,   
	__in_opt WDFREQUEST  Request,  
	__in_opt PWDF_REQUEST_SEND_OPTIONS  RequestOptions,   
	__in PURB  Urb    );

#ifdef WdfUsbInterfaceGetDescriptor
#undef WdfUsbInterfaceGetDescriptor
#endif
#define WdfUsbInterfaceGetDescriptor sdv_WdfUsbInterfaceGetDescriptor
VOID  sdv_WdfUsbInterfaceGetDescriptor(   
								   __in WDFUSBINTERFACE  UsbInterface,  
								   UCHAR  SettingIndex,   
								   __out PUSB_INTERFACE_DESCRIPTOR  InterfaceDescriptor  
								   );
#ifdef WdfUsbInterfaceSelectSetting
#undef WdfUsbInterfaceSelectSetting
#endif
#define WdfUsbInterfaceSelectSetting sdv_WdfUsbInterfaceSelectSetting
NTSTATUS  sdv_WdfUsbInterfaceSelectSetting(  
									   __in WDFUSBINTERFACE  UsbInterface, 
									   __in_opt PWDF_OBJECT_ATTRIBUTES  PipesAttributes, 
									   __in PWDF_USB_INTERFACE_SELECT_SETTING_PARAMS  Params    );

#ifdef WdfUsbInterfaceGetEndpointInformation
#undef WdfUsbInterfaceGetEndpointInformation
#endif
#define WdfUsbInterfaceGetEndpointInformation sdv_WdfUsbInterfaceGetEndpointInformation
VOID  sdv_WdfUsbInterfaceGetEndpointInformation(    
	__in WDFUSBINTERFACE  UsbInterface, 
	UCHAR  SettingIndex,    
	UCHAR  EndpointIndex,
	__in PWDF_USB_PIPE_INFORMATION  EndpointInfo    );

#ifdef WdfUsbInterfaceGetConfiguredPipe
#undef WdfUsbInterfaceGetConfiguredPipe
#endif
#define WdfUsbInterfaceGetConfiguredPipe sdv_WdfUsbInterfaceGetConfiguredPipe
WDFUSBPIPE  sdv_WdfUsbInterfaceGetConfiguredPipe(   
	__in WDFUSBINTERFACE  UsbInterface, 
	__in UCHAR  PipeIndex,   
	__out_opt PWDF_USB_PIPE_INFORMATION  PipeInfo    );

#ifdef WdfUsbTargetPipeWdmGetPipeHandle
#undef WdfUsbTargetPipeWdmGetPipeHandle
#endif
#define WdfUsbTargetPipeWdmGetPipeHandle sdv_WdfUsbTargetPipeWdmGetPipeHandle
USBD_PIPE_HANDLE  sdv_WdfUsbTargetPipeWdmGetPipeHandle(    
	__in WDFUSBPIPE  UsbPipe    );




