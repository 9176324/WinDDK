//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Assumptions for the type definitions:
//    ULONGLONG = 64bit unsigned integer
//    ULONG     = 32bit unsigned integer
//    USHORT    = 16bit unsigned integer
//    UCHAR     = 8bit unsigned integer
//
#ifndef _HDAUDIO_H_
#define _HDAUDIO_H_

#pragma warning(disable:4201) // nameless struct/union
#pragma warning(disable:4214) // bit field types other than int

//
// The HDAUDIO_BUS_INTERFACE interface GUID
//
// {D2EAF88B-AB18-41a8-B664-8D592167671B}
DEFINE_GUID (GUID_HDAUDIO_BUS_INTERFACE, 
0xd2eaf88b, 0xab18, 0x41a8, 0xb6, 0x64, 0x8d, 0x59, 0x21, 0x67, 0x67, 0x1b);

//
// The HDAUDIO_BUS_INTERFACE_BDL interface GUID
//
// {B4D65397-5634-40b0-B068-F5B9F8B967A5}
DEFINE_GUID(GUID_HDAUDIO_BUS_INTERFACE_BDL, 
0xb4d65397, 0x5634, 0x40b0, 0xb0, 0x68, 0xf5, 0xb9, 0xf8, 0xb9, 0x67, 0xa5);

//
// The HDAUDIO_BUS_INTERFACE_V2 interface GUID
//
// {B52AF5FB-424B-4BB9-A160-5B38BE94E568}
DEFINE_GUID (GUID_HDAUDIO_BUS_INTERFACE_V2,
0xb52af5fb, 0x424b, 0x4bb9, 0xa1, 0x60, 0x5b, 0x38, 0xbe, 0x94, 0xe5, 0x68);

//
// The HDAudio bus class GUID
//
// {BBD1A745-ADD6-4575-9C2E-9B428D1C3266}
DEFINE_GUID (GUID_HDAUDIO_BUS_CLASS,
  0xbbd1a745, 0xadd6, 0x4575, 0x9c, 0x2e, 0x9b, 0x42, 0x8d, 0x1c, 0x32, 0x66);

#ifndef _HDAUDIO_CODEC_TRANSFER_
#define _HDAUDIO_CODEC_TRANSFER_
//
// Structure for a codec command.
//
typedef struct _HDAUDIO_CODEC_COMMAND
{
  union
  {
    struct
    {
      ULONG Data    : 8;
      ULONG VerbId  : 12;
      ULONG Node    : 8;
      ULONG CodecAddress : 4;
    } Verb8;
    struct
    {
      ULONG Data    : 16;
      ULONG VerbId  : 4;
      ULONG Node    : 8;
      ULONG CodecAddress : 4;
    } Verb16;
    ULONG Command;
  };
} HDAUDIO_CODEC_COMMAND, *PHDAUDIO_CODEC_COMMAND;

//
// Structure to access a codec response.
//
typedef struct _HDAUDIO_CODEC_RESPONSE
{
  union
  {
    struct
    {
      union
      {
        struct
        {
          ULONG Response:21;
          ULONG SubTag  : 5;
          ULONG Tag     : 6;
        } Unsolicited;
        ULONG Response;
      };
      ULONG SDataIn : 4;
      ULONG IsUnsolicitedResponse : 1;
      ULONG :25;
      ULONG HasFifoOverrun : 1;
      ULONG IsValid : 1;
    };
    ULONGLONG CompleteResponse;   // Mostly used for debug print messages.
  };
} HDAUDIO_CODEC_RESPONSE, *PHDAUDIO_CODEC_RESPONSE;

//
// The structure passed in for sending CODEC verbs.
//
typedef struct _HDAUDIO_CODEC_TRANSFER
{
  HDAUDIO_CODEC_COMMAND  Output;
  HDAUDIO_CODEC_RESPONSE Input;
} HDAUDIO_CODEC_TRANSFER, *PHDAUDIO_CODEC_TRANSFER;
#endif

//
// Replacement for WAVEFORMATEXTENSIBLE which has fields that are not used.
//
typedef struct _HDAUDIO_STREAM_FORMAT
{
  ULONG   SampleRate;
  USHORT  ValidBitsPerSample;
  USHORT  ContainerSize;
  USHORT  NumberOfChannels;
} HDAUDIO_STREAM_FORMAT, *PHDAUDIO_STREAM_FORMAT;

//
// The stream descriptor format used to program the input/output converters.
//
typedef struct _HDAUDIO_CONVERTER_FORMAT
{
  union
  {
    struct
    {
      USHORT NumberOfChannels : 4;
      USHORT BitsPerSample : 3;
      USHORT : 1;
      USHORT SampleRate : 7;
      USHORT StreamType : 1;        // Is always set to 0 by bus driver DDI
    };
    USHORT  ConverterFormat;
  };
} HDAUDIO_CONVERTER_FORMAT, *PHDAUDIO_CONVERTER_FORMAT;

//
// The different stream states supported by HDAudio -> STOP (reset), PAUSE or RUN
//
typedef enum _HDAUDIO_STREAM_STATE
{
  ResetState = 0,
  StopState  = 1,
  PauseState = 1,
  RunState   = 2
} HDAUDIO_STREAM_STATE, *PHDAUDIO_STREAM_STATE;

//
// The different power states that HD Audio codecs can support.  All states
// are from DEVICE_POWER_STATE except PowerCodecD3Cold.
//
typedef enum _HDAUDIO_CODEC_POWER_STATE {
    PowerCodecUnspecified = 0,
    PowerCodecD0,
    PowerCodecD1,
    PowerCodecD2,
    PowerCodecD3,
    PowerCodecD3Cold,
    PowerCodecMaximum
} HDAUDIO_CODEC_POWER_STATE, *PHDAUDIO_CODEC_POWER_STATE;

//
// HDAudio codec transfer complete callback function
//
typedef VOID (*PHDAUDIO_TRANSFER_COMPLETE_CALLBACK)(HDAUDIO_CODEC_TRANSFER *, PVOID);

//
// HDAudio unsolicited response callback function
//
typedef VOID (*PHDAUDIO_UNSOLICITED_RESPONSE_CALLBACK)(HDAUDIO_CODEC_RESPONSE, PVOID);

//
// HDAudio device information structure
//
typedef struct _HDAUDIO_DEVICE_INFORMATION
{
  USHORT  Size;                 // size of this structure
  USHORT  DeviceVersion;        // maj.min (maj is high byte, min is low byte)
  USHORT  DriverVersion;        // maj.min (maj is high byte, min is low byte)
  USHORT  CodecsDetected;       // mask of codecs present. Bit number == SDI line number
  BOOLEAN IsStripingSupported;  // TRUE if striping (2 SDO lines) is supported
} HDAUDIO_DEVICE_INFORMATION, *PHDAUDIO_DEVICE_INFORMATION;

//
// HDAudio Buffer Descriptor list entry
//
typedef struct _HDAUDIO_BUFFER_DESCRIPTOR
{
  PHYSICAL_ADDRESS  Address;
  ULONG             Length;
  ULONG             InterruptOnCompletion;
} HDAUDIO_BUFFER_DESCRIPTOR, *PHDAUDIO_BUFFER_DESCRIPTOR;



typedef __checkReturn NTSTATUS (*PTRANSFER_CODEC_VERBS) (__in PVOID _context, __in ULONG Count, __inout_ecount(Count) PHDAUDIO_CODEC_TRANSFER CodecTransfer, __in PHDAUDIO_TRANSFER_COMPLETE_CALLBACK Callback, __in PVOID Context);
typedef __checkReturn NTSTATUS (*PALLOCATE_CAPTURE_DMA_ENGINE) (__in PVOID _context, __in UCHAR CodecAddress, __in PHDAUDIO_STREAM_FORMAT StreamFormat, OUT PHANDLE Handle, __out PHDAUDIO_CONVERTER_FORMAT ConverterFormat);
typedef __checkReturn NTSTATUS (*PALLOCATE_RENDER_DMA_ENGINE) (__in PVOID _context, __in PHDAUDIO_STREAM_FORMAT StreamFormat, __in BOOLEAN Stripe, __out PHANDLE Handle, __out PHDAUDIO_CONVERTER_FORMAT ConverterFormat);
typedef __checkReturn NTSTATUS (*PCHANGE_BANDWIDTH_ALLOCATION) (__in PVOID _context, __in HANDLE Handle, __in PHDAUDIO_STREAM_FORMAT StreamFormat, __out PHDAUDIO_CONVERTER_FORMAT ConverterFormat);
typedef __checkReturn NTSTATUS (*PALLOCATE_DMA_BUFFER) (__in PVOID _context, __in HANDLE Handle, __in SIZE_T RequestedBufferSize, __out PMDL *BufferMdl, __out PSIZE_T AllocatedBufferSize, OUT PUCHAR StreamId, __out PULONG FifoSize);
typedef __checkReturn NTSTATUS (*PFREE_DMA_BUFFER) (__in PVOID _context, __in HANDLE Handle);
typedef __checkReturn NTSTATUS (*PFREE_DMA_ENGINE) (__in PVOID _context, __in HANDLE Handle);
typedef __checkReturn NTSTATUS (*PSET_DMA_ENGINE_STATE) (__in PVOID _context, __in HDAUDIO_STREAM_STATE StreamState, __in ULONG NumberOfHandles, __in PHANDLE Handles);
typedef VOID     (*PGET_WALL_CLOCK_REGISTER) (__in PVOID _context, __out PULONG *Wallclock);
typedef __checkReturn NTSTATUS (*PGET_LINK_POSITION_REGISTER) (__in PVOID _context, __in HANDLE Handle, __out PULONG *Position);
typedef __checkReturn NTSTATUS (*PREGISTER_EVENT_CALLBACK) (__in PVOID _context, __in PHDAUDIO_UNSOLICITED_RESPONSE_CALLBACK Routine, __in PVOID Context, __out PUCHAR Tag);
typedef __checkReturn NTSTATUS (*PUNREGISTER_EVENT_CALLBACK) (__in PVOID _context, __in UCHAR Tag);
typedef __checkReturn NTSTATUS (*PGET_DEVICE_INFORMATION) (__in PVOID _context, __in __out PHDAUDIO_DEVICE_INFORMATION DeviceInformation);
typedef VOID     (*PGET_RESOURCE_INFORMATION) (__in PVOID _context, __out PUCHAR CodecAddress, __out PUCHAR FunctionGroupStartNode);

typedef struct _HDAUDIO_BUS_INTERFACE
{
  //
  // First we define the standard INTERFACE structure ...
  //
  USHORT                    Size;
  USHORT                    Version;
  PVOID                     Context;
  PINTERFACE_REFERENCE      InterfaceReference;
  PINTERFACE_DEREFERENCE    InterfaceDereference;

  //
  // Then we expand the structure with our interface specific data
  //
  PTRANSFER_CODEC_VERBS           TransferCodecVerbs;
  PALLOCATE_CAPTURE_DMA_ENGINE    AllocateCaptureDmaEngine;
  PALLOCATE_RENDER_DMA_ENGINE     AllocateRenderDmaEngine;
  PCHANGE_BANDWIDTH_ALLOCATION    ChangeBandwidthAllocation;
  PALLOCATE_DMA_BUFFER            AllocateDmaBuffer;
  PFREE_DMA_BUFFER                FreeDmaBuffer;
  PFREE_DMA_ENGINE                FreeDmaEngine;
  PSET_DMA_ENGINE_STATE           SetDmaEngineState;
  PGET_WALL_CLOCK_REGISTER        GetWallClockRegister;
  PGET_LINK_POSITION_REGISTER     GetLinkPositionRegister;
  PREGISTER_EVENT_CALLBACK        RegisterEventCallback;
  PUNREGISTER_EVENT_CALLBACK      UnregisterEventCallback;
  PGET_DEVICE_INFORMATION         GetDeviceInformation;
  PGET_RESOURCE_INFORMATION       GetResourceInformation;
} HDAUDIO_BUS_INTERFACE, *PHDAUDIO_BUS_INTERFACE;

//
// To support the Bdl interface...
//
//
// ISR Callback definition for Bdl interface
//
typedef void (*PHDAUDIO_BDL_ISR) (__in VOID *Context, __in ULONG InterruptBitMask); 

//
// Additional BDL interface functions.
//
typedef __checkReturn NTSTATUS (*PALLOCATE_CONTIGUOUS_DMA_BUFFER) (__in PVOID _context, __in HANDLE Handle,
  ULONG RequestedBufferSize, __out PVOID *DataBuffer, __out PHDAUDIO_BUFFER_DESCRIPTOR *BdlBuffer);
typedef __checkReturn NTSTATUS (*PFREE_CONTIGUOUS_DMA_BUFFER) (__in PVOID _context, __in HANDLE Handle);
typedef __checkReturn NTSTATUS (*PSETUP_DMA_ENGINE_WITH_BDL) (__in_bcount(BufferLength) PVOID _context, __in HANDLE Handle, __in ULONG BufferLength,
  __in ULONG Lvi, __in PHDAUDIO_BDL_ISR Isr, __in PVOID Context, __out PUCHAR StreamId, __out PULONG FifoSize);

typedef struct _HDAUDIO_BUS_INTERFACE_BDL
{
  //
  // First we define the standard INTERFACE structure ...
  //
  USHORT                    Size;
  USHORT                    Version;
  PVOID                     Context;
  PINTERFACE_REFERENCE      InterfaceReference;
  PINTERFACE_DEREFERENCE    InterfaceDereference;

  //
  // Then we expand the structure with the HDAUDIO_BUS_INTERFACE_BDL stuff.
  // Many functions are identical (and derived) from the HDAUDIO_BUS_INTERFACE
  // interface. PrepareDmaEngineWithBdl was added instead of PrepareDmaEngine
  // and GetDeviceInformationBdl instead of GetDeviceInformation.
  //
  PTRANSFER_CODEC_VERBS           TransferCodecVerbs;
  PALLOCATE_CAPTURE_DMA_ENGINE    AllocateCaptureDmaEngine;
  PALLOCATE_RENDER_DMA_ENGINE     AllocateRenderDmaEngine;
  PCHANGE_BANDWIDTH_ALLOCATION    ChangeBandwidthAllocation;
  PALLOCATE_CONTIGUOUS_DMA_BUFFER AllocateContiguousDmaBuffer;
  PSETUP_DMA_ENGINE_WITH_BDL      SetupDmaEngineWithBdl;
  PFREE_CONTIGUOUS_DMA_BUFFER     FreeContiguousDmaBuffer;
  PFREE_DMA_ENGINE                FreeDmaEngine;
  PSET_DMA_ENGINE_STATE           SetDmaEngineState;
  PGET_WALL_CLOCK_REGISTER        GetWallClockRegister;
  PGET_LINK_POSITION_REGISTER     GetLinkPositionRegister;
  PREGISTER_EVENT_CALLBACK        RegisterEventCallback;
  PUNREGISTER_EVENT_CALLBACK      UnregisterEventCallback;
  PGET_DEVICE_INFORMATION         GetDeviceInformation;
  PGET_RESOURCE_INFORMATION       GetResourceInformation;
} HDAUDIO_BUS_INTERFACE_BDL, *PHDAUDIO_BUS_INTERFACE_BDL;

//
// Additional interface functions for DMA notification support
//
typedef __checkReturn NTSTATUS (*PALLOCATE_DMA_BUFFER_WITH_NOTIFICATION) (__in PVOID _context,
                                                                          __in HANDLE Handle, 
                                                                          __in ULONG NotificationCount,
                                                                          __in SIZE_T RequestedBufferSize,
                                                                          __out PMDL *BufferMdl,
                                                                          __out PSIZE_T AllocatedBufferSize,
                                                                          __out PSIZE_T OffsetFromFirstPage,
                                                                          __out PUCHAR StreamId,
                                                                          __out PULONG FifoSize);

typedef __checkReturn NTSTATUS (*PFREE_DMA_BUFFER_WITH_NOTIFICATION) (  __in PVOID _context,
                                                                        __in HANDLE Handle,
                                                                        __in PMDL BufferMdl,
                                                                        __in SIZE_T BufferSize);

typedef __checkReturn NTSTATUS (*PREGISTER_NOTIFICATION_EVENT) (__in PVOID _context,
                                                                __in HANDLE Handle,
                                                                __in PKEVENT NotificationEvent);

typedef __checkReturn NTSTATUS (*PUNREGISTER_NOTIFICATION_EVENT) (__in PVOID _context,
                                                                  __in HANDLE Handle,
                                                                  __in PKEVENT NotificationEvent);

typedef struct _HDAUDIO_BUS_INTERFACE_V2
{
    //
    // First we define the standard INTERFACE structure ...
    //
    USHORT                    Size;
    USHORT                    Version;
    PVOID                     Context;
    PINTERFACE_REFERENCE      InterfaceReference;
    PINTERFACE_DEREFERENCE    InterfaceDereference;

    //
    // Then we expand the structure with the HDAUDIO_BUS_INTERFACE_PING_PONG stuff.
    // Many functions are identical (and derived) from the HDAUDIO_BUS_INTERFACE
    // interface. 

    PTRANSFER_CODEC_VERBS           TransferCodecVerbs;
    PALLOCATE_CAPTURE_DMA_ENGINE    AllocateCaptureDmaEngine;
    PALLOCATE_RENDER_DMA_ENGINE     AllocateRenderDmaEngine;
    PCHANGE_BANDWIDTH_ALLOCATION    ChangeBandwidthAllocation;
    PALLOCATE_DMA_BUFFER            AllocateDmaBuffer;
    PFREE_DMA_BUFFER                FreeDmaBuffer;
    PFREE_DMA_ENGINE                FreeDmaEngine;
    PSET_DMA_ENGINE_STATE           SetDmaEngineState;
    PGET_WALL_CLOCK_REGISTER        GetWallClockRegister;
    PGET_LINK_POSITION_REGISTER     GetLinkPositionRegister;
    PREGISTER_EVENT_CALLBACK        RegisterEventCallback;
    PUNREGISTER_EVENT_CALLBACK      UnregisterEventCallback;
    PGET_DEVICE_INFORMATION         GetDeviceInformation;
    PGET_RESOURCE_INFORMATION       GetResourceInformation;
    PALLOCATE_DMA_BUFFER_WITH_NOTIFICATION AllocateDmaBufferWithNotification;
    PFREE_DMA_BUFFER_WITH_NOTIFICATION FreeDmaBufferWithNotification;
    PREGISTER_NOTIFICATION_EVENT    RegisterNotificationEvent;
    PUNREGISTER_NOTIFICATION_EVENT  UnregisterNotificationEvent;
} HDAUDIO_BUS_INTERFACE_V2, *PHDAUDIO_BUS_INTERFACE_V2;

#pragma warning(default:4201)
#pragma warning(default:4214)

#endif  // _HDAUDIO_H_

