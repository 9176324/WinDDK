#****************************** File Header ******************************\
# File Name: sources.inc for KBDs
#
# Copyright (c) 1985-2000, Microsoft Corporation
#
# Project specific SOURCES file.
#
# History:
# May-14-1999 HiroYama Created
#***************************************************************************/

INCLUDES=..\..\inc

C_DEFINES=
UMTYPE=nt
UMBASE=0x1000000
TARGETLIBS=
DLLDEF=$(TARGETNAME).def

!if $(IA64)
SECTION_ALIGNMENT=0x4000
!endif
LINKER_FLAGS = -merge:.edata=.data -merge:.rdata=.data -merge:.text=.data -merge:.bss=.data -section:.data,re

LINKER_FLAGS= $(LINKER_FLAGS) /ignore:4254
NO_RDATA_MERGE=1

TARGETTYPE=DYNLINK
USE_NOLIBS=1
BUFFER_OVERFLOW_CHECKS=0

SOURCES=$(TARGETNAME).c $(TARGETNAME).rc

