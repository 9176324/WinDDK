/*++

Copyright (c) Microsoft Corporation. All rights reserved

Abstract:

    MSN monitor sample driver notification routines

Environment:

    Kernel mode
    
--*/

#include "ntddk.h"
#include "fwpmk.h"

#pragma warning(push)
#pragma warning(disable:4201)       // unnamed struct/union

#include "fwpsk.h"

#pragma warning(pop)


#include "ioctl.h"

#include "msnmntr.h"
#include "ctl.h"

#include "notify.h"

//
// Software Tracing Definitions 
//
#define WPP_CONTROL_GUIDS \
    WPP_DEFINE_CONTROL_GUID(MsnMntrNotify,(aca2f74a, 7a0d, 4f47, be4b, 66900813b8e5),  \
        WPP_DEFINE_BIT(TRACE_CLIENT_SERVER)                 \
        WPP_DEFINE_BIT(TRACE_PEER_TO_PEER)                  \
        WPP_DEFINE_BIT(TRACE_UNKNOWN)                       \
        WPP_DEFINE_BIT(TRACE_ALL_TRAFFIC) )

#include "notify.tmh" //  This file will be auto generated

typedef enum _COMMAND_TYPE
{
   CMD_TYPE_UNKNOWN = 0,
   CMD_TYPE_CLIENT_SERVER = 1,
   CMD_TYPE_PEER_TO_PEER = 2
} COMMAND_TYPE;

#define TAG_NAME_NOTIFY 'oNnM'

typedef struct _COMMAND
{
   char           cmd[4];
   COMMAND_TYPE   cmdType;
} COMMAND;

const COMMAND commands[] = { 
   {"ACK", CMD_TYPE_CLIENT_SERVER},
   {"ADC", CMD_TYPE_CLIENT_SERVER},
   {"ANS", CMD_TYPE_CLIENT_SERVER},
   {"BLP", CMD_TYPE_CLIENT_SERVER},
   {"BYE", CMD_TYPE_CLIENT_SERVER},
   {"CAL", CMD_TYPE_CLIENT_SERVER},
   {"CHG", CMD_TYPE_CLIENT_SERVER},
   {"FLN", CMD_TYPE_CLIENT_SERVER},
   {"GTC", CMD_TYPE_CLIENT_SERVER},
   {"INF", CMD_TYPE_CLIENT_SERVER},
   {"IRO", CMD_TYPE_CLIENT_SERVER},
   {"JOI", CMD_TYPE_CLIENT_SERVER},
   {"LST", CMD_TYPE_CLIENT_SERVER},
   {"MSG", CMD_TYPE_PEER_TO_PEER},
   {"NAK", CMD_TYPE_CLIENT_SERVER},
   {"NLN", CMD_TYPE_CLIENT_SERVER},
   {"OUT", CMD_TYPE_CLIENT_SERVER},
   {"REM", CMD_TYPE_CLIENT_SERVER},
   {"RNG", CMD_TYPE_CLIENT_SERVER},
   {"SYN", CMD_TYPE_CLIENT_SERVER},
   {"USR", CMD_TYPE_CLIENT_SERVER},
   {"VER", CMD_TYPE_CLIENT_SERVER},
   {"XFR", CMD_TYPE_CLIENT_SERVER} };

#define COMMAND_COUNT sizeof(commands) / sizeof(commands[0])

COMMAND_TYPE
MonitorNfGetCommandType(
   IN BYTE*    stream,
   IN SIZE_T   streamLength)
{
   ULONG i;

   if (streamLength < 3)
   {
      return CMD_TYPE_UNKNOWN;
   }

   for (i = 0; i < COMMAND_COUNT; i++)
   {
      if (strncmp((const char*) stream, commands[i].cmd, 3) == 0)
      {
         return commands[i].cmdType;
      }
   }

   return CMD_TYPE_UNKNOWN;
}

NTSTATUS
MonitorNfInitialize(
   IN PDEVICE_OBJECT deviceObject)
{
   UNREFERENCED_PARAMETER(deviceObject);

   return STATUS_SUCCESS;
}

NTSTATUS
MonitorNfUninitialize()
{
   return STATUS_SUCCESS;
}

__forceinline
BOOLEAN
MonitorNfpIsPrintableCharacter(
   IN BYTE ch)
{
   if (ch == '\r' || ch == '\n' || (ch >= 0x20 && ch <= 0x7e))
   {
      return TRUE;
   }
   return FALSE;
}

__forceinline
PVOID
MonitorNfpFindCharacters(
   IN const char* stream,
   IN SIZE_T streamLength,
   IN const char* subStream,
   IN ULONG subStreamLength,
   OUT SIZE_T* bytesLeft)
{
   ULONG currentOffset = 0;
   PVOID subStreamPtr = NULL;
   
   *bytesLeft = streamLength;

   if (subStreamLength > streamLength)
   {
      return NULL;
   }

   while (currentOffset+subStreamLength <= streamLength)
   {
      if (0 == memcmp((void*)(stream+currentOffset), subStream, subStreamLength))
      {
         subStreamPtr = (void*)(char*)(stream+currentOffset);
         *bytesLeft = streamLength;
         *bytesLeft -= currentOffset;
         *bytesLeft -= subStreamLength;
         break;
      }
      currentOffset += subStreamLength;
   }
   
   return subStreamPtr;
}

__forceinline
BOOLEAN
MonitorNfContainsNonPrintableCharacters(
   IN BYTE*    buffer,
   IN SIZE_T   bufferLength)
{
   SIZE_T i;

   for (i = 0; i < bufferLength; i++)
   {
      if (!MonitorNfpIsPrintableCharacter(buffer[i]))
      {
         return TRUE;
      }
   }
   return FALSE;
}

NTSTATUS
MonitorNfParseMessageInbound(
   IN BYTE*       stream,
   IN SIZE_T      streamLength,
   IN USHORT      localPort,
   IN USHORT      remotePort)
{
   NTSTATUS status = STATUS_SUCCESS;
   
   if (streamLength != 0)
   {
      COMMAND_TYPE cmdType;
      BYTE stackStream[1024];
      BYTE* tmpStream = NULL;
      BOOLEAN allocatedStream = FALSE;
      
      if (streamLength >= sizeof(stackStream))
      {
         tmpStream = ExAllocatePoolWithTag(NonPagedPool, streamLength + 1, 'yftN');
         if (tmpStream == NULL)
           return STATUS_INSUFFICIENT_RESOURCES;
         else
           allocatedStream = TRUE; 
      }
      else
      {
         tmpStream = (BYTE*) stackStream;
      }

      RtlZeroMemory(tmpStream, streamLength+1);

      cmdType = MonitorNfGetCommandType(stream, streamLength);

      switch (cmdType)
      {
      case CMD_TYPE_CLIENT_SERVER:
         {
            RtlCopyMemory(tmpStream, stream, streamLength);
            DoTraceMessage(TRACE_CLIENT_SERVER,
                           "Client-Server Message Received. Local Port: %d Remote Port: %d. Message: %s",
                           localPort,
                           remotePort,
                           (const char*) tmpStream);

            break;
         }
      case CMD_TYPE_PEER_TO_PEER:
         {
            RtlCopyMemory(tmpStream, stream, streamLength);
            DoTraceMessage(TRACE_PEER_TO_PEER,
                           "Client-Client Message Received. Local Port: %d Remote Port: %d. Message: %s",
                           localPort,
                           remotePort,
                           (const char*) tmpStream);
            break;
         }
      case CMD_TYPE_UNKNOWN:
         {
            RtlCopyMemory(tmpStream, stream, streamLength);
            DoTraceMessage(TRACE_UNKNOWN,
                           "Unknown or Invalid Command Message Received. Local Port: %d Remote Port: %d. Message: %s",
                           localPort,
                           remotePort,
                           (const char*) tmpStream);
            break;
         }
      default:
         ASSERT(0);
      }

      if (allocatedStream && tmpStream)
      {
         ExFreePoolWithTag(tmpStream, 'yftN');
      }
      return status;
   }
   else
   {
      status = STATUS_BUFFER_TOO_SMALL;
      return status;
   }
}

NTSTATUS
MonitorNfParseMessageInboundHttpHeader(
   IN BYTE*       stream,
   IN SIZE_T      streamLength,
   IN USHORT      localPort,
   IN USHORT      remotePort)
{
   BYTE* msgStart = NULL;
   SIZE_T bytesLeft;
   NTSTATUS status = STATUS_INVALID_PARAMETER;
   
   // Walk past the HTTP header.
   msgStart = (BYTE*) MonitorNfpFindCharacters((char*)stream, 
                                               streamLength,
                                               "\r\n\r\n",
                                               (ULONG)strlen("\r\n\r\n"),
                                               &bytesLeft);
   if (msgStart && (bytesLeft > 0))
   {
      SIZE_T msgLength;

      msgStart += 4; // step past \r\n\r\n.

      msgLength = streamLength - (ULONG)(ULONG_PTR)(msgStart - stream);
      
      // Do the final inbound message processing.
      status = MonitorNfParseMessageInbound(msgStart,
                                            msgLength,
                                            localPort,
                                            remotePort);
   }

   return status;
}

NTSTATUS
MonitorNfParseMessageOutbound(
   IN BYTE*       stream,
   IN SIZE_T      streamLength,
   IN USHORT      localPort,
   IN USHORT      remotePort)
{
   NTSTATUS status = STATUS_SUCCESS;
   
   if (streamLength != 0)
   {
      COMMAND_TYPE cmdType;
      BYTE stackStream[1024];
      BYTE* tmpStream = NULL;
      BOOLEAN allocatedStream = FALSE;

      if (streamLength >= sizeof(stackStream))
      {
         tmpStream = ExAllocatePoolWithTag(NonPagedPool, streamLength + 1, 'yftN');
         if (tmpStream == NULL)
           return STATUS_INSUFFICIENT_RESOURCES;
         else
           allocatedStream = TRUE;
      }
      else
      {
         tmpStream = (BYTE*) stackStream;
      }

      RtlZeroMemory(tmpStream, streamLength+1);

      cmdType = MonitorNfGetCommandType(stream, streamLength);

      switch (cmdType)
      {
      case CMD_TYPE_CLIENT_SERVER:
         {
            RtlCopyMemory(tmpStream, stream, streamLength);
            DoTraceMessage(TRACE_CLIENT_SERVER,
                           "Client-Server Message Sent. Local Port: %d Remote Port: %d. Message: %s\r\n",
                           localPort,
                           remotePort,
                           (const char*) tmpStream);

            break;
         }
      case CMD_TYPE_PEER_TO_PEER:
         {
            RtlCopyMemory(tmpStream, stream, streamLength);
            DoTraceMessage(TRACE_PEER_TO_PEER,
                           "Client-Client Message Sent. Local Port: %d Remote Port: %d. Message: %s\r\n",
                           localPort,
                           remotePort,
                           (const char*) tmpStream);
            break;
         }
      case CMD_TYPE_UNKNOWN:
         {
            RtlCopyMemory(tmpStream, stream, streamLength);
            DoTraceMessage(TRACE_UNKNOWN,
                           "Unknown or Invalid Command Message Sent. Local Port: %d Remote Port: %d. Message: %s\r\n",
                           localPort,
                           remotePort,
                           (const char*) tmpStream);
            break;
         }
      default:
         ASSERT(0);
      }

      if (allocatedStream && tmpStream)
      {
         ExFreePoolWithTag(tmpStream, 'yftN');
      }
      return status;
   }
   else
   {
      status = STATUS_BUFFER_TOO_SMALL;
      return status;
   }
}

NTSTATUS
MonitorNfParseMessageOutboundHttpHeader(
   IN BYTE*       stream,
   IN SIZE_T      streamLength,
   IN USHORT      localPort,
   IN USHORT      remotePort)
{
   BYTE* msgStart = NULL;
   SIZE_T bytesLeft;
   NTSTATUS status = STATUS_SUCCESS;
   
   // Walk past the HTTP header.
   msgStart = (BYTE*) MonitorNfpFindCharacters((char*)stream, 
                                               streamLength,
                                               "\r\n\r\n",
                                               (ULONG)strlen("\r\n\r\n"),
                                               &bytesLeft);
   if (msgStart && (bytesLeft > 0))
   {
      SIZE_T msgLength;

      msgStart += 4; // step past \r\n\r\n.

      msgLength = streamLength - (ULONG)(ULONG_PTR)(msgStart - stream);
      status = MonitorNfParseMessageOutbound(msgStart,
                                             msgLength,
                                             localPort,
                                             remotePort);
   }
   
   return status;
}

NTSTATUS
MonitorNfParseStreamAndTraceMessage(
   IN BYTE*       stream,
   IN SIZE_T      streamLength,
   IN BOOLEAN     inbound,
   IN USHORT      localPort,
   IN USHORT      remotePort)
{
   NTSTATUS status;
   ULONG bytesWritten;
   SIZE_T bytesLeft;
   ULONG bytesSkipped;

   bytesLeft = streamLength;
   bytesWritten = 0;
   bytesSkipped = 0;

   if (MonitorNfContainsNonPrintableCharacters(stream,
                                               streamLength))
   {
      // We don't want to trace non printable characters.
      return STATUS_SUCCESS;
   }

   {
      BYTE stackStream[1024];
      BYTE* tmpStream = NULL;
      BOOLEAN allocatedStream = FALSE;

      if (streamLength >= sizeof(stackStream))
      {
         tmpStream = ExAllocatePoolWithTag(NonPagedPool, streamLength + 1, 'yftN');
         if (tmpStream == NULL)
           return STATUS_INSUFFICIENT_RESOURCES;
         else
           allocatedStream = TRUE;
      }
      else
      {
         tmpStream = (BYTE*) stackStream;
      }

      RtlZeroMemory(tmpStream, streamLength+1);

      RtlCopyMemory(tmpStream, stream, streamLength);
      DoTraceMessage(TRACE_ALL_TRAFFIC,
                  "Message Received. Local Port: %d Remote Port: %d. Message: %s",
                  localPort,
                  remotePort,
                  (const char*) tmpStream);

      if (allocatedStream && tmpStream)
      {
         ExFreePoolWithTag(tmpStream, 'yftN');
      }
   }

   if (!inbound)
   {
      if ((_strnicmp((const char*)stream, "POST", 4) == 0)
          || (_strnicmp((const char*)stream, "GET", 3) == 0))
      {
         if ((MonitorNfParseMessageOutboundHttpHeader(stream,
                                                      streamLength,
                                                      localPort,
                                                      remotePort)) != STATUS_SUCCESS)
           return STATUS_INSUFFICIENT_RESOURCES; 
      }
      else
      {
         if ((MonitorNfParseMessageOutbound(stream,
                                            streamLength,
                                            localPort,
                                            remotePort)!= STATUS_SUCCESS))
           return STATUS_INSUFFICIENT_RESOURCES;
      }
   }
   else
   {
      if (_strnicmp((const char*)stream, "HTTP", 4) == 0)
      {
         if ((MonitorNfParseMessageInboundHttpHeader(stream,
                                                     streamLength,
                                                     localPort,
                                                     remotePort)) != STATUS_SUCCESS)
            return STATUS_INSUFFICIENT_RESOURCES;
      }
      else
      {
         if ((MonitorNfParseMessageInbound(stream,
                                           streamLength,
                                           localPort,
                                           remotePort)) != STATUS_SUCCESS)
            return STATUS_INSUFFICIENT_RESOURCES;
      }
   }

   {
      status = STATUS_SUCCESS;
   }

   return status;
}

NTSTATUS MonitorNfNotifyMessage(
   IN FWPS_STREAM_DATA*       streamBuffer,
   IN BOOLEAN                    inbound,
   IN USHORT                     localPort,
   IN USHORT                     remotePort
)
{
   NTSTATUS status = STATUS_SUCCESS;
   BYTE* stream = NULL;
   SIZE_T streamLength = streamBuffer->dataLength;
   SIZE_T bytesCopied = 0;

   if(streamLength == 0)
      return status;

   stream =  ExAllocatePoolWithTag(NonPagedPool,
                                   streamLength,
                                   TAG_NAME_NOTIFY);
   if (!stream)
      return STATUS_INSUFFICIENT_RESOURCES;

   RtlZeroMemory(stream,streamLength);

   FwpsCopyStreamDataToBuffer0(
      streamBuffer,
      stream,
      streamLength,
      &bytesCopied);

   ASSERT(bytesCopied == streamLength);

   status = MonitorNfParseStreamAndTraceMessage(stream, streamLength, inbound, localPort, remotePort);

   ExFreePoolWithTag(stream, TAG_NAME_NOTIFY);

   return status;
}

