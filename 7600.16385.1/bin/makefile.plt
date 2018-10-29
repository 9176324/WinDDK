

!IFNDEF _NTROOT
_NTROOT=\nt
!ENDIF

!IFNDEF BASEDIR
BASEDIR=$(_NTDRIVE)$(_NTROOT)
!ENDIF

# A couple of overrides
!ifndef PUBLIC_ROOT
PUBLIC_ROOT=$(BASEDIR)\public
!endif

__LATEST_NT_TARGET_VERSION=0x601

!ifndef _NT_TARGET_VERSION # Default to current OS version
_NT_TARGET_VERSION = $(__LATEST_NT_TARGET_VERSION)
!endif

!ifndef _NT_TOOLS_VERSION # Default to current VC version
_NT_TOOLS_VERSION = 0x800
!endif

!ifndef SDK_PATH
SDK_PATH = $(PUBLIC_ROOT)\sdk
!endif

!ifndef SDK_INC_PATH
SDK_INC_PATH = $(SDK_PATH)\inc
!endif

!ifndef SDK_REF_PATH
SDK_REF_PATH = $(SDK_PATH)\ref
!endif

!ifndef SDK_INC16_PATH
SDK_INC16_PATH = $(SDK_PATH)\inc16
!endif

!ifndef SDK_LIB_DEST
SDK_LIB_DEST = $(SDK_PATH)\lib
!endif

!ifndef SDK_LIB_PATH
SDK_LIB_PATH = $(SDK_LIB_DEST)\*
!endif

!ifndef SDK_LIB16_PATH
SDK_LIB16_PATH=$(SDK_PATH)\lib16
!endif

!ifndef DDK_PATH
DDK_PATH = $(PUBLIC_ROOT)\ddk
!endif

!ifndef DDK_INC_PATH
DDK_INC_PATH = $(DDK_PATH)\inc
!endif

!ifndef KMDF_INC_PATH
KMDF_INC_PATH = $(DDK_PATH)\inc\wdf\kmdf
!endif

!ifndef KMDF_VER_PATH_1_5
KMDF_VER_PATH_1_5 = 1.5
!endif

!ifndef KMDF_VER_PATH_1_7
KMDF_VER_PATH_1_7 = 1.7
!endif

!ifndef KMDF_VERSION_MINOR_LATEST
KMDF_VERSION_MINOR_LATEST=9
!endif

!ifndef KMDF_VER_PATH_LATEST
KMDF_VER_PATH_LATEST = 1.$(KMDF_VERSION_MINOR_LATEST)
!endif

# default to current KMDF version
!ifndef KMDF_VER_PATH
KMDF_VER_PATH = $(KMDF_VER_PATH_LATEST)
!endif

!ifndef KMDF_LIB_PATH
KMDF_LIB_PATH = $(DDK_LIB_PATH)\wdf\kmdf
!endif

!ifndef UMDF_INC_PATH
UMDF_INC_PATH = $(DDK_PATH)\inc\wdf\umdf
!endif

!ifndef UMDF_VER_PATH_1_5
UMDF_VER_PATH_1_5 = 1.5
!endif

!ifndef UMDF_VER_PATH_1_7
UMDF_VER_PATH_1_7 = 1.7
!endif

!ifndef UMDF_VERSION_MINOR_LATEST
UMDF_VERSION_MINOR_LATEST=9
!endif

!ifndef UMDF_VER_PATH_LATEST
UMDF_VER_PATH_LATEST = 1.$(UMDF_VERSION_MINOR_LATEST)
!endif

# default to current UMDF version
!ifndef UMDF_VER_PATH
UMDF_VER_PATH = $(UMDF_VER_PATH_LATEST)
!endif

!ifndef DDK_LIB_DEST
DDK_LIB_DEST = $(DDK_PATH)\lib
!endif

!ifndef DDK_LIB_PATH
DDK_LIB_PATH = $(DDK_LIB_DEST)\*
!endif

!ifndef UMDF_LIB_PATH
UMDF_LIB_PATH = $(DDK_LIB_DEST)\umdf
!endif

!ifndef IFSKIT_PATH
IFSKIT_PATH = $(PUBLIC_ROOT)\ifskit
!endif

!ifndef IFSKIT_INC_PATH
IFSKIT_INC_PATH = $(IFSKIT_PATH)\inc
!endif

!ifndef IFSKIT_LIB_DEST
IFSKIT_LIB_DEST = $(IFSKIT_PATH)\lib
!endif

!ifndef IFSKIT_LIB_PATH
IFSKIT_LIB_PATH = $(IFSKIT_LIB_DEST)\*
!endif

!ifndef HALKIT_PATH
HALKIT_PATH = $(PUBLIC_ROOT)\halkit
!endif

!ifndef HALKIT_INC_PATH
HALKIT_INC_PATH = $(HALKIT_PATH)\inc
!endif

!ifndef HALKIT_LIB_DEST
HALKIT_LIB_DEST = $(HALKIT_PATH)\lib
!endif

!ifndef HALKIT_LIB_PATH
HALKIT_LIB_PATH = $(HALKIT_LIB_DEST)\*
!endif

!ifndef WDM_INC_PATH
WDM_INC_PATH = $(DDK_INC_PATH)\wdm
!endif

!ifndef CRT_INC_PATH
CRT_INC_PATH = $(SDK_INC_PATH)\crt
!endif

!ifndef IOSTREAMS_INC_PATH
IOSTREAMS_INC_PATH = $(SDK_INC_PATH)\crt\iostreams
!endif

!ifndef STL6_INC_PATH
STL6_INC_PATH = $(SDK_INC_PATH)\crt\stl60
!endif

!ifndef STL7_INC_PATH
STL7_INC_PATH = $(SDK_INC_PATH)\crt\stl70
!endif

!ifndef CRT_LIB_PATH
CRT_LIB_PATH = $(SDK_LIB_PATH)
!endif

!ifndef OAK_INC_PATH
OAK_INC_PATH = $(PUBLIC_ROOT)\oak\inc
!endif

!ifndef OAK_BIN_PATH
OAK_BIN_PATH = $(PUBLIC_ROOT)\oak\binr
!endif

!ifndef OAK_INC_PATH
OAK_INC_PATH = $(PUBLIC_ROOT)\oak\inc
!endif

!ifndef ATL_LIB_PATH
ATL_LIB_PATH=$(SDK_LIB_PATH)
!endif

!ifndef MFC_LIB_PATH
MFC_LIB_PATH=$(SDK_LIB_PATH)
!endif

# "mfc$(MFC_VER)" will be appended to this in makefile.def to
# create MFC_INC_PATH
!ifndef MFC_INC_ROOT
MFC_INC_ROOT=$(SDK_INC_PATH)
!endif

# "atl$(ATL_VER)" will be appended to this in makefile.def to
# create ATL_INC_PATH
!ifndef ATL_INC_ROOT
ATL_INC_ROOT=$(SDK_INC_PATH)
!endif

!ifndef WPP_CONFIG_PATH
# If this ever changes you must change the DDK's setenv.bat
WPP_CONFIG_PATH = $(RAZZLETOOLPATH)\WppConfig
!endif

!ifndef PUBLIC_INTERNAL_PATH
PUBLIC_INTERNAL_PATH = $(PUBLIC_ROOT)\internal
!endif

!if defined(USE_OBJECT_ROOT) && "$(USE_OBJECT_ROOT)" != "0" && "$(USE_OBJECT_ROOT)" != "1"
!  error USE_OBJECT_ROOT must be 0 or 1 or not defined.
!endif

!if !defined(USE_OBJECT_ROOT)
!  if "$(BUILD_PRODUCT)" == "NT"
USE_OBJECT_ROOT=1
!  else
USE_OBJECT_ROOT=0
!  endif
!endif

COPYRIGHT_STRING = Copyright (c) Microsoft Corporation. All rights reserved.

# perl.exe disabled for WDK only
_DEL=del $@ $@.temp 2>nul
_DELFILE=del
_MOVE=move $@.temp $@ >nul
_PERL=echo perl.exe
_TYPE=type
_COPYFILE=copy
_IF=if

# debugging support
!ifdef BUILD_DEBUG
TYPE=rem
NOECHO=
BUILD_NOKEEP=KEEP
!else
TYPE=@$(_TYPE)
NOECHO=@
BUILD_NOKEEP=NOKEEP
!endif

BINPLACE_FLAGS=$(BINPLACE_FLAGS) -:LOGPDB

!ifndef PUBLISH_CMD
PUBLISH_CMD=$(_PERL) $(NTMAKEENV)\publish.pl /Pass:$(BUILD_PASS)
!endif

!ifndef BUILD_PASS
# Old build.exe.  Guess the correct pass.
!ifdef PASS0ONLY
BUILD_PASS=PASS0
!elseif defined(LINKONLY)
BUILD_PASS=PASS2
!elseif defined(NOLINK) && !defined(PASS0ONLY)
BUILD_PASS=PASS1
!else
# catch all - someone used build /z or build /2 or nmake
BUILD_PASS=PASSALL
!endif
!endif

#
# Set the flag which indicates whether we should be publishing binaries
# to 0 by default.  the project.mk file is responsible for parsing
# BINARY_PUBLISH_PROJECTS to determine if its value should be changed.
#

BINPUBLISH=0

#
# Define global project paths.
#

!include $(NTMAKEENV)\projects.inc

#
# Do this before project.mk so project.mk can use it.
#
!if ("$(NTDEBUG)" == "") || ("$(NTDEBUG)" == "ntsdnodbg")
FREEBUILD=1
!else
FREEBUILD=0
!endif

#
# Find and include the project configuration file.
#

!if exists(.\project.mk)
_PROJECT_MK_PATH=.
!elseif exists(..\project.mk)
_PROJECT_MK_PATH=..
!elseif exists(..\..\project.mk)
_PROJECT_MK_PATH=..\..
!elseif exists(..\..\..\project.mk)
_PROJECT_MK_PATH=..\..\..
!elseif exists(..\..\..\..\project.mk)
_PROJECT_MK_PATH=..\..\..\..
!elseif exists(..\..\..\..\..\project.mk)
_PROJECT_MK_PATH=..\..\..\..\..
!elseif exists(..\..\..\..\..\..\project.mk)
_PROJECT_MK_PATH=..\..\..\..\..\..
!elseif exists(..\..\..\..\..\..\..\project.mk)
_PROJECT_MK_PATH=..\..\..\..\..\..\..
!elseif exists(..\..\..\..\..\..\..\..\project.mk)
_PROJECT_MK_PATH=..\..\..\..\..\..\..\..
!elseif exists(..\..\..\..\..\..\..\..\..\project.mk)
_PROJECT_MK_PATH=..\..\..\..\..\..\..\..\..
!elseif exists(..\..\..\..\..\..\..\..\..\..\project.mk)
_PROJECT_MK_PATH=..\..\..\..\..\..\..\..\..\..
!elseif exists(..\..\..\..\..\..\..\..\..\..\..\project.mk)
_PROJECT_MK_PATH=..\..\..\..\..\..\..\..\..\..\..
!elseif exists(..\..\..\..\..\..\..\..\..\..\..\..\project.mk)
_PROJECT_MK_PATH=..\..\..\..\..\..\..\..\..\..\..\..
!elseif exists(..\..\..\..\..\..\..\..\..\..\..\..\..\project.mk)
_PROJECT_MK_PATH=..\..\..\..\..\..\..\..\..\..\..\..\..
!elseif exists(..\..\..\..\..\..\..\..\..\..\..\..\..\..\project.mk)
_PROJECT_MK_PATH=..\..\..\..\..\..\..\..\..\..\..\..\..\..
!elseif exists(..\..\..\..\..\..\..\..\..\..\..\..\..\..\..\project.mk)
_PROJECT_MK_PATH=..\..\..\..\..\..\..\..\..\..\..\..\..\..\..
!endif

!if "$(_PROJECT_MK_PATH)" != ""
#!message "Including $(_PROJECT_MK_PATH)\project.mk"
!include $(_PROJECT_MK_PATH)\project.mk
!if exists($(_PROJECT_MK_PATH)\myproject.mk)
!include $(_PROJECT_MK_PATH)\myproject.mk
!endif
!else
#!message "Unable to find project.mk. Update makefile.plt or create project.mk."
!endif

!ifndef OBJECT_ROOT
OBJECT_ROOT=$(BASEDIR)
!endif

!IFDEF _PROJECT_

PROJECT_OBJ_ROOT=$(OBJECT_ROOT)\$(_PROJECT_)
PROJECT_ROOT=$(BASEDIR)\$(_PROJECT_)
PROJECT_PUBLIC_PATH=$(PUBLIC_INTERNAL_PATH)\$(_PROJECT_)
PROJECT_INC_PATH=$(PROJECT_PUBLIC_PATH)\inc
PRIVATE_INC_PATH=$(PROJECT_INC_PATH)
PROJECT_LIB_DEST=$(PROJECT_PUBLIC_PATH)\lib
PROJECT_LIB_PATH=$(PROJECT_LIB_DEST)\$(TARGET_DIRECTORY)
PROJECT_INF_PATH=$(PROJECT_PUBLIC_PATH)\inf
PROJECT_REF_PATH=$(PROJECT_PUBLIC_PATH)\ref

!ELSE
#!message "ERROR: _PROJECT_ is not defined. Should be defined in project.mk."
!ENDIF

#
# If not defined, define the build message banner.
#

!IFNDEF BUILDMSG
BUILDMSG=
!ENDIF

#
# Do this again in case project.mk changed it.
#
!if ("$(NTDEBUG)" == "") || ("$(NTDEBUG)" == "ntsdnodbg")
FREEBUILD=1
! ifndef BUILD_TYPE
BUILD_TYPE=fre
! endif
!else
FREEBUILD=0
! ifndef BUILD_TYPE
BUILD_TYPE=chk
! endif
!endif


# Allow alternate object directories.

!if "$(BUILD_PRODUCT)" != "NT" && !defined(BUILD_ALT_DIR) && defined(CHECKED_ALT_DIR) && !$(FREEBUILD)
BUILD_ALT_DIR=d
!endif

_OBJ_DIR = obj$(BUILD_ALT_DIR)

# define a simple macro that can be used for the object subdir in makefile.inc/sources files

O = $(_OBJ_DIR)\$(TARGET_DIRECTORY)

#
# Determine which target is being built (i386 or ia64) and define
# the appropriate target variables.
#

!IFNDEF 386
386=0
!ENDIF

!IFNDEF AMD64
AMD64=0
!ENDIF

!IFNDEF IA64
IA64=0
!ENDIF

!IFNDEF ARM
ARM=0
!ENDIF

# Disable.
PPC=0
MPPC=0
MIPS=0
AXP64=0
ALPHA=0

#
# Default to building for the i386 target, if no target is specified.
#

!IF !$(386)
! IF !$(AMD64)
!  IF !$(IA64)
!   IF !$(ARM)
386=1
!   ENDIF
!  ENDIF
! ENDIF
!ENDIF

PLATFORM_MFC_VER=0x0600
TLB_SWITCHES=/tlb

#
# Define the target platform specific information.
#

!if $(386)

ASM_SUFFIX=asm
ASM_INCLUDE_SUFFIX=inc

TARGET_BRACES=

!ifdef SUBSTITUTE_386_CC
TARGET_CPP=$(SUBSTITUTE_386_CC)
!else
TARGET_CPP=cl.exe
!endif

MIDL_PLATFORM_FLAG=/win32

TARGET_DEFINES=/Di386 /D_X86_
TARGET_DIRECTORY=i386
!ifndef _NTTREE
! ifdef _NTX86TREE
_NTTREE=$(_NTX86TREE)
! elseif defined(_NT386TREE)
_NTTREE=$(_NT386TREE)
! endif
!endif

VCCOM_SUPPORTED=1
SCP_SUPPORTED=1
WIN64=0
MACHINE_TYPE=ix86
TLBIMP_MACHINE=/machine:x86
TLBEXP_MACHINE=/Win32
ILASM_MACHINE_SWITCH=
ANSI_ANNOTATION=0
DEFAULT_MSC_OPT=/Oxs
TARGET_CSC=csc

!elseif $(AMD64)

ASM_SUFFIX=asm
ASM_INCLUDE_SUFFIX=inc

TARGET_BRACES=
!ifdef SUBSTITUTE_AMD64_CC
TARGET_CPP=$(SUBSTITUTE_AMD64_CC)
!else
TARGET_CPP=cl.exe
!endif
TARGET_DEFINES=/D_AMD64_ /D_WIN64
MIDL_PLATFORM_FLAG=/amd64
TARGET_DIRECTORY=amd64

!ifndef _NTTREE
! ifdef _NTAMD64TREE
_NTTREE=$(_NTAMD64TREE)
! endif
!endif

VCCOM_SUPPORTED=1
SCP_SUPPORTED=0
WIN64=1
MACHINE_TYPE=amd64
TLBIMP_MACHINE=/machine:x64
TLBEXP_MACHINE=/Win64
ILASM_MACHINE_SWITCH=/x64
ANSI_ANNOTATION=0
DEFAULT_MSC_OPT=/Oxs 
!elseif $(IA64)

ASM_SUFFIX=s
ASM_INCLUDE_SUFFIX=h

TARGET_BRACES=-B
TARGET_CPP=cl.exe
TARGET_DEFINES=/DIA64 /D_IA64_
TARGET_DIRECTORY=ia64
MIDL_PLATFORM_FLAG=/ia64

!ifndef _NTTREE
! ifdef _NTIA64TREE
_NTTREE=$(_NTIA64TREE)
! endif
!endif

WIN64=1
SCP_SUPPORTED=0
MACHINE_TYPE=ia64
TLBIMP_MACHINE=/machine:Itanium
TLBEXP_MACHINE=/Win64
ILASM_MACHINE_SWITCH=/Itanium

ANSI_ANNOTATION=0
DEFAULT_MSC_OPT=/Oxs

!elseif $(ARM)

ASM_SUFFIX=s
ASM_INCLUDE_SUFFIX=inc

TARGET_BRACES=
TARGET_CPP=clarm.exe
TARGET_DEFINES=/D_ARM_
TARGET_DIRECTORY=arm

!ifndef _NTTREE
! ifdef _NTARMTREE
_NTTREE=$(_NTARMTREE)
! endif
!endif

VCCOM_SUPPORTED=0
SCP_SUPPORTED=0
WIN64=0
PLATFORM_MFC_VER=0x0600
MACHINE_TYPE=arm
ANSI_ANNOTATION=0
DEFAULT_MSC_OPT=/Oxs

!else
!error Must define the target as 386, ia64, amd64, or arm.
!endif

#
#  These flags don't depend on i386 etc. however have to be in this file.
#
#  MIDL_OPTIMIZATION is the optimization flag set for the current NT.
#  MIDL_OPTIMIZATION_NO_ROBUST is the current optimization without robust.
#
!ifdef MIDL_PROTOCOL
MIDL_PROTOCOL_DEFAULT=/protocol $(MIDL_PROTOCOL)
!else
MIDL_PROTOCOL_DEFAULT=/protocol all
!endif

!if $(WIN64)
MIDL_OPTIMIZATION_NO_ROBUST=/Oicf /error all $(MIDL_PLATFORM_FLAG)
MIDL_OPTIMIZATION_NT4=/Oicf /error all $(MIDL_PLATFORM_FLAG) 
!else
MIDL_OPTIMIZATION_NO_ROBUST=/Oicf /error all /no_robust $(MIDL_PLATFORM_FLAG) 
MIDL_OPTIMIZATION_NT4=/Oicf /error all /no_robust $(MIDL_PLATFORM_FLAG) 
!endif
MIDL_OPTIMIZATION_NT5=/Oicf /robust /error all $(MIDL_PROTOCOL_DEFAULT) $(MIDL_PLATFORM_FLAG) 
!ifdef SUBSTITUTE_MIDL_CC
MIDL_CPP=$(SUBSTITUTE_MIDL_CC)
!else
MIDL_CPP=$(TARGET_CPP)
!endif
MIDL_FLAGS=$(TARGET_DEFINES) /D_WCHAR_T_DEFINED

! if $(WIN64)
MIDL_ALWAYS_GENERATE_STUBS=1
! else
MIDL_ALWAYS_GENERATE_STUBS=0
! endif

CLEANSE_PUBLISHED_HDR=$(_COPYFILE)

PATH_TOOLS16=$(RAZZLETOOLPATH)\tools16

#
# If a build path is defined, use it.
#
!ifdef BUILD_PATH
PATH=$(BUILD_PATH)
!endif

BINPLACE_PLACEFILE_DIR=$(NTMAKEENV)

!if exist(makefil0) && "$(SDK_INC_PATH)" != "" && exist($(SDK_INC_PATH)\ntverpmk.inc)
!include $(SDK_INC_PATH)\ntverpmk.inc
!endif

!if exist ($(OBJECT_ROOT)\$O\__blddate__)
!include $(OBJECT_ROOT)\$O\__blddate__
!endif

!if exist ($(OBJECT_ROOT)\$O\__bldnum__)
!include $(OBJECT_ROOT)\$O\__bldnum__
!endif

#
# Make $O available as an environment variable.
# This does not cost a CreateProcess, it is emulated by nmake.
#
# TBD -- get rid of this.
#
!if [set O=$(O)]
!endif

#
# This does not cost a CreateProcess, it is emulated by nmake.
#
# TBD -- get rid of this.
#
!if defined(URT_VER) && [set URT_VER=$(URT_VER)]
!endif

# This does not cost a CreateProcess, it is emulated by nmake.
!if [set BINPLACE_PDB_DLL=mspdb80.dll]
!endif

!if defined(MAKEDIR_RELATIVE_TO_BASEDIR) # { {
#
# OBJ_PATH is the path under OBJECT_ROOT associated with your source.
#
# To pick up your neighor's .lib, you say
#   TARGETLIBS=$(OBJ_PATH)\..\foo\$O\foo.lib.
#
# To pick up your neighbor's midl outputs, you say:
#   INCLUDES=$(OBJ_PATH)\..\foo\$O
#
# To pick up your midl outputs, you say:
#   INCLUDES=$(OBJ_PATH)\$O
#
# To build midl outputs, you say:
#   SOURCES=$(OBJ_PATH)\$O\foo_i.c
#
# Build.exe passes MAKEDIR_RELATIVE_TO_BASEDIR on the command line.
# Build.exe also lowercases _NTDRIVE, _NTROOT, BASEDIR, OBJECT_ROOT.
# If MAKEDIR_RELATIVE_TO_BASEDIR is not on the command line, then nmake must compute it itself.
#
!if "$(MAKEDIR_RELATIVE_TO_BASEDIR)" == ""
!undef MAKEDIR_RELATIVE_TO_BASEDIR
!endif

!if defined(OBJECT_ROOT) && $(USE_OBJECT_ROOT)
!if defined(MAKEDIR_RELATIVE_TO_BASEDIR)

OBJ_PATH=$(OBJECT_ROOT)\$(MAKEDIR_RELATIVE_TO_BASEDIR)

!elseif defined(WDK_ALTERNATE_MAKEDIR)

OBJ_PATH=$(OBJECT_ROOT)\$(WDK_ALTERNATE_MAKEDIR)

!else

OBJ_PATH=$(MAKEDIR)

!endif
!endif

!else # } {

MAKEDIR_LOWERCASE=$(MAKEDIR)
MAKEDIR_LOWERCASE=$(MAKEDIR_LOWERCASE:A=a)
MAKEDIR_LOWERCASE=$(MAKEDIR_LOWERCASE:B=b)
MAKEDIR_LOWERCASE=$(MAKEDIR_LOWERCASE:C=c)
MAKEDIR_LOWERCASE=$(MAKEDIR_LOWERCASE:D=d)
MAKEDIR_LOWERCASE=$(MAKEDIR_LOWERCASE:E=e)
MAKEDIR_LOWERCASE=$(MAKEDIR_LOWERCASE:F=f)
MAKEDIR_LOWERCASE=$(MAKEDIR_LOWERCASE:G=g)
MAKEDIR_LOWERCASE=$(MAKEDIR_LOWERCASE:H=h)
MAKEDIR_LOWERCASE=$(MAKEDIR_LOWERCASE:I=i)
MAKEDIR_LOWERCASE=$(MAKEDIR_LOWERCASE:J=j)
MAKEDIR_LOWERCASE=$(MAKEDIR_LOWERCASE:K=k)
MAKEDIR_LOWERCASE=$(MAKEDIR_LOWERCASE:L=l)
MAKEDIR_LOWERCASE=$(MAKEDIR_LOWERCASE:M=m)
MAKEDIR_LOWERCASE=$(MAKEDIR_LOWERCASE:N=n)
MAKEDIR_LOWERCASE=$(MAKEDIR_LOWERCASE:O=o)
MAKEDIR_LOWERCASE=$(MAKEDIR_LOWERCASE:P=p)
MAKEDIR_LOWERCASE=$(MAKEDIR_LOWERCASE:Q=q)
MAKEDIR_LOWERCASE=$(MAKEDIR_LOWERCASE:R=r)
MAKEDIR_LOWERCASE=$(MAKEDIR_LOWERCASE:S=s)
MAKEDIR_LOWERCASE=$(MAKEDIR_LOWERCASE:T=t)
MAKEDIR_LOWERCASE=$(MAKEDIR_LOWERCASE:U=u)
MAKEDIR_LOWERCASE=$(MAKEDIR_LOWERCASE:V=v)
MAKEDIR_LOWERCASE=$(MAKEDIR_LOWERCASE:W=w)
MAKEDIR_LOWERCASE=$(MAKEDIR_LOWERCASE:X=x)
MAKEDIR_LOWERCASE=$(MAKEDIR_LOWERCASE:Y=y)
MAKEDIR_LOWERCASE=$(MAKEDIR_LOWERCASE:Z=z)

BASEDIR=$(BASEDIR:A=a)
BASEDIR=$(BASEDIR:B=b)
BASEDIR=$(BASEDIR:C=c)
BASEDIR=$(BASEDIR:D=d)
BASEDIR=$(BASEDIR:E=e)
BASEDIR=$(BASEDIR:F=f)
BASEDIR=$(BASEDIR:G=g)
BASEDIR=$(BASEDIR:H=h)
BASEDIR=$(BASEDIR:I=i)
BASEDIR=$(BASEDIR:J=j)
BASEDIR=$(BASEDIR:K=k)
BASEDIR=$(BASEDIR:L=l)
BASEDIR=$(BASEDIR:M=m)
BASEDIR=$(BASEDIR:N=n)
BASEDIR=$(BASEDIR:O=o)
BASEDIR=$(BASEDIR:P=p)
BASEDIR=$(BASEDIR:Q=q)
BASEDIR=$(BASEDIR:R=r)
BASEDIR=$(BASEDIR:S=s)
BASEDIR=$(BASEDIR:T=t)
BASEDIR=$(BASEDIR:U=u)
BASEDIR=$(BASEDIR:V=v)
BASEDIR=$(BASEDIR:W=w)
BASEDIR=$(BASEDIR:X=x)
BASEDIR=$(BASEDIR:Y=y)
BASEDIR=$(BASEDIR:Z=z)

OBJECT_ROOT=$(OBJECT_ROOT:A=a)
OBJECT_ROOT=$(OBJECT_ROOT:B=b)
OBJECT_ROOT=$(OBJECT_ROOT:C=c)
OBJECT_ROOT=$(OBJECT_ROOT:D=d)
OBJECT_ROOT=$(OBJECT_ROOT:E=e)
OBJECT_ROOT=$(OBJECT_ROOT:F=f)
OBJECT_ROOT=$(OBJECT_ROOT:G=g)
OBJECT_ROOT=$(OBJECT_ROOT:H=h)
OBJECT_ROOT=$(OBJECT_ROOT:I=i)
OBJECT_ROOT=$(OBJECT_ROOT:J=j)
OBJECT_ROOT=$(OBJECT_ROOT:K=k)
OBJECT_ROOT=$(OBJECT_ROOT:L=l)
OBJECT_ROOT=$(OBJECT_ROOT:M=m)
OBJECT_ROOT=$(OBJECT_ROOT:N=n)
OBJECT_ROOT=$(OBJECT_ROOT:O=o)
OBJECT_ROOT=$(OBJECT_ROOT:P=p)
OBJECT_ROOT=$(OBJECT_ROOT:Q=q)
OBJECT_ROOT=$(OBJECT_ROOT:R=r)
OBJECT_ROOT=$(OBJECT_ROOT:S=s)
OBJECT_ROOT=$(OBJECT_ROOT:T=t)
OBJECT_ROOT=$(OBJECT_ROOT:U=u)
OBJECT_ROOT=$(OBJECT_ROOT:V=v)
OBJECT_ROOT=$(OBJECT_ROOT:W=w)
OBJECT_ROOT=$(OBJECT_ROOT:X=x)
OBJECT_ROOT=$(OBJECT_ROOT:Y=y)
OBJECT_ROOT=$(OBJECT_ROOT:Z=z)

MAKEDIR_RELATIVE_TO_BASEDIR=$(MAKEDIR_LOWERCASE)
!if !defined(_IN_MAKEFILE_PLT) && !exist($(OBJECT_ROOT)\$O\makedir_relative_to_basedirmk.inc) && exist($(BASEDIR)\makefil0)
!if  [nmake.exe /nologo /f $(BASEDIR)\makefil0 _IN_MAKEFILE_PLT=1 $(OBJECT_ROOT)\$O\makedir_relative_to_basedirmk.inc]
!endif
!endif
!if exist($(OBJECT_ROOT)\$O\makedir_relative_to_basedirmk.inc)
!include $(OBJECT_ROOT)\$O\makedir_relative_to_basedirmk.inc
!else
!endif

!if "$(MAKEDIR_LOWERCASE)" == "$(BASEDIR)"
MAKEDIR_RELATIVE_TO_BASEDIR=.
!endif

#
# Is MAKEDIR not under MAKEDIR_RELATIVE_TO_BASEDIR?
# Or are we early in the build such that $(OBJECT_ROOT)\$O\makedir_relative_to_basedirmk.inc doesn't yet exist?
#
!if "$(MAKEDIR_RELATIVE_TO_BASEDIR)" == "$(MAKEDIR_LOWERCASE)"
!undef MAKEDIR_RELATIVE_TO_BASEDIR
!endif

#
# First set OBJ_PATH independent of USE_OBJECT_ROOT. Makefile.def changes OBJ_PATH
# back to MAKEDIR if USE_OBJECT_ROOT is 0. This way you can set it
# in sources files, though it is usually set in project.mk or the environment
# or defaulted based on BUILD_PRODUCT. Setting it in any of those is prefered
# over setting it in sources files in order to make it visible to makefil0.
#
!if defined(OBJECT_ROOT) && defined(MAKEDIR_RELATIVE_TO_BASEDIR)
OBJ_PATH=$(OBJECT_ROOT)\$(MAKEDIR_RELATIVE_TO_BASEDIR)
!else
OBJ_PATH=$(MAKEDIR)
!endif

!endif # } }

!if defined(_PROJECT_MK_PATH)
_PROJECT_MK_OBJ_PATH=$(OBJ_PATH)\$(_PROJECT_MK_PATH)
!endif

#
# Put TEMP in $O unless it has spaces.
# This ensures no files are left in TEMP -- via scorch/build /c -- all of which
# are in the business of cleaning up $O whereas nothing in the build cleans up $TEMP.
#
# This gives TEMP other characteristics of OBJECT_ROOT/BASEDIR, such as realtime antivirus scanner disabling.
#
# It can be spindle-unfriendly, but if TEMP files are deleted and never leave the file system cache,
# it is spindle-ok.
#
!if defined(MAKEDIR_RELATIVE_TO_BASEDIR)
!if "$(OBJ_PATH: =)" == "$(OBJ_PATH)"
!if [set TEMP=$(OBJ_PATH)\$O]
!endif
!if [set TMP=$(OBJ_PATH)\$O]
!endif
!if !exist($(OBJ_PATH)\$O)
!if [mkdir $(OBJ_PATH)\$O]
!endif
!endif
!endif
!endif

!if ("$(MUI)" == "0")
!  ifndef MUI_COMMENT
MUI_COMMENT = ENVIRONMENT_DISABLED
!  endif
!endif

