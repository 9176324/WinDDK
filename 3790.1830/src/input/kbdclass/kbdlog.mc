;/*++ BUILD Version: 0001    // Increment this if a change has global effects
;
;Copyright (c) 1992, 1993  Microsoft Corporation
;
;Module Name:
;
;    kbdlog.mc
;
;Abstract:
;
;    Constant definitions for the I/O error code log values.
;
;Revision History:
;
;--*/
;
;#ifndef _KBDLOG_
;#define _KBDLOG_
;
;//
;//  Status values are 32 bit values layed out as follows:
;//
;//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
;//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
;//  +---+-+-------------------------+-------------------------------+
;//  |Sev|C|       Facility          |               Code            |
;//  +---+-+-------------------------+-------------------------------+
;//
;//  where
;//
;//      Sev - is the severity code
;//
;//          00 - Success
;//          01 - Informational
;//          10 - Warning
;//          11 - Error
;//
;//      C - is the Customer code flag
;//
;//      Facility - is the facility code
;//
;//      Code - is the facility's status code
;//
;
MessageIdTypedef=NTSTATUS

SeverityNames=(Success=0x0:STATUS_SEVERITY_SUCCESS
               Informational=0x1:STATUS_SEVERITY_INFORMATIONAL
               Warning=0x2:STATUS_SEVERITY_WARNING
               Error=0x3:STATUS_SEVERITY_ERROR
              )

FacilityNames=(System=0x0
               RpcRuntime=0x2:FACILITY_RPC_RUNTIME
               RpcStubs=0x3:FACILITY_RPC_STUBS
               Io=0x4:FACILITY_IO_ERROR_CODE
               Kbdclass=0x5:FACILITY_KBDCLASS_ERROR_CODE
              )



MessageId=0x0001 Facility=Kbdclass Severity=Error SymbolicName=KBDCLASS_INSUFFICIENT_RESOURCES
Language=English
Not enough memory was available to allocate internal storage needed for the device %1.
.

MessageId=0x0002 Facility=Kbdclass Severity=Error SymbolicName=KBDCLASS_NO_BUFFER_ALLOCATED
Language=English
Not enough memory was available to allocate the ring buffer that holds incoming data for %1.
.

MessageId=0x0003 Facility=Kbdclass Severity=Informational SymbolicName=KBDCLASS_NOT_ENOUGH_CONFIG_INFO
Language=English
Some firmware configuration information was incomplete, so defaults were used.
.

MessageId=0x0004 Facility=Kbdclass Severity=Informational SymbolicName=KBDCLASS_USER_OVERRIDE
Language=English
User configuration data is overriding firmware configuration data.
.

MessageId=0x0005 Facility=Kbdclass Severity=Warning SymbolicName=KBDCLASS_NO_DEVICEMAP_CREATED
Language=English
Unable to create the device map entry for %1.
.

MessageId=0x0006 Facility=Kbdclass Severity=Warning SymbolicName=KBDCLASS_NO_DEVICEMAP_DELETED
Language=English
Unable to delete the device map entry for %1.
.

MessageId=0x0007 Facility=Kbdclass Severity=Error SymbolicName=KBDCLASS_NO_PORT_DEVICE_OBJECT
Language=English
Could not locate the device object for one or more keyboard port devices.
.

MessageId=0x0008 Facility=Kbdclass Severity=Error SymbolicName=KBDCLASS_NO_PORT_CONNECT
Language=English
Could not connect to port device %1.
.

MessageId=0x0009 Facility=Kbdclass Severity=Error SymbolicName=KBDCLASS_PORT_INTERRUPTS_NOT_ENABLED
Language=English
Could not enable interrupts on connected port device %1.
.

MessageId=0x000A Facility=Kbdclass Severity=Error SymbolicName=KBDCLASS_PORT_INTERRUPTS_NOT_DISABLED
Language=English
Could not disable interrupts on connected port device %1.
.

MessageId=0x000B Facility=Kbdclass Severity=Informational SymbolicName=KBDCLASS_KBD_BUFFER_OVERFLOW
Language=English
The ring buffer that stores incoming keyboard data has overflowed (buffer size is configurable via the registry).
.

MessageId=0x000C Facility=Kbdclass Severity=Error SymbolicName=KBDCLASS_INTERNAL_ERROR
Language=English
The driver for device %1 encountered an internal driver error.
.

MessageId=0x000D Facility=Kbdclass Severity=Error SymbolicName=KBDCLASS_COULD_NOT_CREATE_DEVICE
Language=English
The driver could not create the required device object.
.

MessageId=0x000E Facility=Kbdclass Severity=Warning SymbolicName=KBDCLASS_NO_RESOURCES_FOR_WAITWAKE
Language=English
The driver could not obtain resources required to create a propper WaitWake IRP.
.

MessageId=0x000F Facility=Kbdclass Severity=Error SymbolicName=KBDCLASS_ATTACH_DEVICE_FAILED
Language=English
Could not attach to the PnP device stack.
.
;#endif /* _KBDLOG_ */

