!IF 0

Copyright (c) Microsoft Corporation

Module Name:

    amd64mk.inc

Abstract:

        This module contains the AMD64 specific build controls.  It is included
        by makefile.def.

Revision History:

!ENDIF

#
# AMD64 option control
#
UMBASE=$(UMBASE:*=amd64)
UMLIBS=$(UMLIBS:*=amd64)
NTRES=$(NTRES:*=amd64)
UMRES=$(UMRES:*=amd64)
UMOBJS=$(UMOBJS:*=amd64)
CRTLIBS=$(CRTLIBS:*=amd64)
LINKLIBS=$(LINKLIBS:*=amd64)
GDI_DRIVER_LIBS=$(GDI_DRIVER_LIBS:*=amd64)
DLLBASE=$(DLLBASE:*=amd64)
DLLDEF=$(DLLDEF:*=amd64)
MACHINE_TARGETLIBS=$(MACHINE_TARGETLIBS:*=amd64)
!ifdef USE_MSHTML_PDB_RULES
MACHINE_TARGETLIBS=$(MACHINE_TARGETLIBS:????????=*)
!endif
TARGET=$(TARGET:*=amd64)
DYNLINK_LIB=$(DYNLINK_LIB:*=amd64)
TARGETEXEFILES=$(TARGETEXEFILES:*=amd64)
TARGETLIBFILES=$(TARGETLIBFILES:*=amd64)
TARGETOBJFILES=$(TARGETOBJFILES:*=amd64)
UMOBJFILES=$(UMOBJFILES:*=amd64)
UMEXEFILES=$(UMEXEFILES:*=amd64)
HEADERFILE=$(HEADERFILE:*=amd64)
HEADEROBJNAME=$(HEADEROBJNAME:*=amd64)
HEADEROBJ=$(HEADEROBJ:*=amd64)
PRECOMPILED=$(PRECOMPILED:*=amd64)
PRECOMPILED_CXX=$(PRECOMPILED_CXX:*=amd64)
PRECOMPILED_TARGET=$(PRECOMPILED_TARGET:*=amd64)
MFC_STATIC_LIB=$(MFC_STATIC_LIB:*=amd64)
CRT_LIB_PATH=$(CRT_LIB_PATH:*=amd64)
SDK_LIB_PATH=$(SDK_LIB_PATH:*=amd64)
DDK_LIB_PATH=$(DDK_LIB_PATH:*=amd64)
IFSKIT_LIB_PATH=$(IFSKIT_LIB_PATH:*=amd64)
HALKIT_LIB_PATH=$(HALKIT_LIB_PATH:*=amd64)
ORDER=$(ORDER:*=amd64)
OBJLIBFILES=$(OBJLIBFILES:*=amd64)
MISCFILES=$(MISCFILES) $(AMD64_MISCFILES)
!if defined(AMD64_DRIVER_INFS)
DRIVER_INFS=$(DRIVER_INFS) $(AMD64_DRIVER_INFS)
!endif
!if defined(AMD64_MANIFESTED_INFS)
MANIFESTED_INFS=$(MANIFESTED_INFS) $(AMD64_MANIFESTED_INFS)
!endif
!if defined(AMD64_PRINTER_INFS)
PRINTER_INFS=$(PRINTER_INFS) $(AMD64_PRINTER_INFS)
!endif
!if defined(AMD64_MANIFESTED_PRINTER_INFS)
MANIFESTED_PRINTER_INFS=$(MANIFESTED_PRINTER_INFS) $(AMD64_MANIFESTED_PRINTER_INFS)
!endif
!if defined(AMD64_NON_PNP_INFS)
NON_PNP_INFS=$(NON_PNP_INFS) $(AMD64_NON_PNP_INFS)
!endif
!if defined(AMD64_NON_PNP_SIGN_INFS)
NON_PNP_SIGN_INFS=$(NON_PNP_SIGN_INFS) $(AMD64_NON_PNP_SIGN_INFS)
!endif
!if defined(AMD64_LOC_DRIVER_INFS)
LOC_DRIVER_INFS=$(LOC_DRIVER_INFS) $(AMD64_LOC_DRIVER_INFS)
!endif
!if defined(AMD64_LOC_MANIFESTED_INFS)
LOC_MANIFESTED_INFS=$(LOC_MANIFESTED_INFS) $(AMD64_LOC_MANIFESTED_INFS)
!endif
!if defined(AMD64_LOC_PRINTER_INFS)
LOC_PRINTER_INFS=$(LOC_PRINTER_INFS) $(AMD64_LOC_PRINTER_INFS)
!endif
!if defined(AMD64_LOC_MANIFESTED_PRINTER_INFS)
LOC_MANIFESTED_PRINTER_INFS=$(LOC_MANIFESTED_PRINTER_INFS) $(AMD64_LOC_MANIFESTED_PRINTER_INFS)
!endif
!if defined(AMD64_LOC_NON_PNP_INFS)
LOC_NON_PNP_INFS=$(LOC_NON_PNP_INFS) $(AMD64_LOC_NON_PNP_INFS)
!endif
!if defined(AMD64_LOC_NON_PNP_SIGN_INFS)
LOC_NON_PNP_SIGN_INFS=$(LOC_NON_PNP_SIGN_INFS) $(AMD64_LOC_NON_PNP_SIGN_INFS)
!endif
SOURCES=$(SOURCES) $(AMD64_SOURCES)

ATL_STATIC_LIBS = $(ATL_STATIC_LIBS) $(ATL_LIB_PATH)\atlthunk.lib

!ifdef NTTARGETFILES
NTTARGETFILES=$(NTTARGETFILES:*=amd64)
!endif
!ifdef NTTARGETFILE0
NTTARGETFILE0=$(NTTARGETFILE0:*=amd64)
!endif
!ifdef NTTARGETFILE1
NTTARGETFILE1=$(NTTARGETFILE1:*=amd64)
!endif

!ifdef PROJECT_LIB_PATH
PROJECT_LIB_PATH=$(PROJECT_LIB_PATH:*=amd64)
!endif

ENTRY_SUFFIX=
GDI_ENTRY_SUFFIX=

DEFAULT_STACKRESERVE=0x80000
DEFAULT_STACKCOMMIT=0x2000

ADDITIONAL_ATL_LIBS = $(ADDITIONAL_ATL_LIBS)

!IFDEF AMD64_WARNING_LEVEL
MSC_WARNING_LEVEL=$(AMD64_WARNING_LEVEL)
!ENDIF

MSC_WARNING_LEVEL=$(MSC_WARNING_LEVEL) $(COMPILER_WX_SWITCH)

!ifdef AMD64_PERFFLAGS
PERFFLAGS = $(AMD64_PERFFLAGS)
!endif

# Set MSC_OPTIMIZATION.
# Order of precedence:
#   Platform specific override
#   Environment variable
#   System Default

!ifdef AMD64_OPTIMIZATION
MSC_OPTIMIZATION=$(AMD64_OPTIMIZATION)
!elseif !defined(MSC_OPTIMIZATION)
MSC_OPTIMIZATION=$(DEFAULT_MSC_OPT)
!endif

LINK_TIME_CODE_GENERATION_MSC_FLAG=
!if defined(LINK_TIME_CODE_GENERATION) && !defined(FORCENATIVEOBJECT)
# Some objects must be built native, so turn off GL for those
LINK_TIME_CODE_GENERATION_MSC_FLAG=/GL
!endif

DBGFLAGS=$(DBGFLAGS) $(MSC_OPTIMIZATION) $(LINK_TIME_CODE_GENERATION_MSC_FLAG)

!IFDEF AMD64_CPPFLAGS
MSC_CPPFLAGS=$(AMD64_CPPFLAGS)
!ENDIF

!ifdef NTCPPFLAGS
!error NTCPPFLAGS is deprecated.
!endif

!ifdef NO_STRING_POOLING
STRING_POOLING =
!else
!ifdef NO_READONLY_STRINGS
STRING_POOLING = /Gf
!else
STRING_POOLING = /GF
!endif
!endif

!ifdef USE_NATIVE_EH
!if "$(USE_NATIVE_EH)" == "ASYNC"
EH_FLAGS=/EHa
!else
!if "$(USE_NATIVE_EH)" == "CTHROW"
EH_FLAGS=/EHsc-
!else
EH_FLAGS=/EHsc
!endif
!endif
!else
EH_FLAGS=/EHs-c-
!endif

!if defined(USE_RTTI) || defined(MANAGED_CXX)
RTTI_FLAGS=/GR
! else
RTTI_FLAGS=/GR-
! endif

# On by default
!if "$(BUFFER_OVERFLOW_CHECKS)" == ""
BUFFER_OVERFLOW_CHECKS=1
!endif

# Turn off for the select few
!if ("$(DRIVERTYPE)" == "VXD") || \
    (("$(TARGETTYPE)" == "MINIPORT") && ($(_NT_TARGET_VERSION) < 0x502)) || \
    (("$(TARGETTYPE)" == "DYNLINK") && defined(RESOURCE_ONLY_DLL)) || \
    "$(BUFFER_OVERFLOW_CHECKS)" == "0"
!undef BUFFER_OVERFLOW_CHECKS
!endif

!if defined(BUFFER_OVERFLOW_CHECKS)
# verify it's one of the chosen few - 1, NTDLL or compile only - all others are wrong.
! if "$(BUFFER_OVERFLOW_CHECKS)" != "1" && \
     "$(BUFFER_OVERFLOW_CHECKS)" != "NTDLL" && \
     "$(BUFFER_OVERFLOW_CHECKS)" != "COMPILE_ONLY" && \
     "$(BUFFER_OVERFLOW_CHECKS)" != "LINK_ONLY"
!  error INVALID BUFFER_OVERFLOW_CHECKS value "$(BUFFER_OVERFLOW_CHECKS)" in $(MAKEDIR)
! endif
! if "$(BUFFER_OVERFLOW_CHECKS)" == "LINK_ONLY"
BO_FLAGS=/GS-
BUFFER_OVERFLOW_CHECKS=1
! else
BO_FLAGS=/GS
! endif
! if "$(BUFFER_OVERFLOW_CHECKS)" == "1"
!  if "$(TARGETTYPE)" == "DRIVER" || \
      "$(TARGETTYPE)" == "EXPORT_DRIVER" || \
      "$(TARGETTYPE)" == "DRIVER_LIBRARY" || \
      "$(TARGETTYPE)" == "MINIPORT" 
BO_LIB=$(SDK_LIB_PATH)\BufferOverflowK.lib
DRIVER_ENTRY=GsDriverEntry
!  elseif "$(TARGETTYPE)" == "GDI_DRIVER"
BO_LIB=$(SDK_LIB_PATH)\BufferOverflowGDI.lib
GDI_DRIVER_ENTRY=GsDrvEnableDriver
!  elseif "$(TARGETTYPE)" == "DYNLINK"
!   if !(("$(UMTYPE)"== "nt") || ("$(UMTYPE)"=="ntss"))
!    if ("$(DLLENTRY)" == "/noentry" || "$(DLLENTRY)" == "-noentry")
!     if defined(USE_MSVCRT) || defined(USE_LIBCMT)
DLLENTRY=-entry:_DllMainCRTStartup
!     else
DLLENTRY=-entry:_DllMainCRTStartupForGS
!     endif
!    endif
!    if !defined(USE_MSVCRT) && !defined(USE_LIBCMT)
BO_LIB=$(SDK_LIB_PATH)\BufferOverflowU.lib
!    endif
!   endif
!  else	# TARGETTYPE == PROGRAM/LIBRARY/NOTARGET with special build rules
!   if ("$(UMTYPE)"== "nt") || ("$(UMTYPE)"=="ntss")
!    if defined(NTKERNEL)
BO_LIB=$(SDK_LIB_PATH)\BufferOverflowK.lib
!    else
BO_LIB=$(SDK_LIB_PATH)\BufferOverflow.lib
!    endif
!   else
!    if !defined(USE_MSVCRT) && !defined(USE_LIBCMT)
BO_LIB=$(SDK_LIB_PATH)\BufferOverflowU.lib
!    endif
!   endif
!  endif
! elseif "$(BUFFER_OVERFLOW_CHECKS)" == "NTDLL"
BO_LIB=$(SDK_LIB_PATH)\BufferOverflow.lib
!  if ("$(TARGETTYPE)" == "DYNLINK") && \
      ("$(DLLENTRY)" == "/noentry" || "$(DLLENTRY)" == "-noentry") && \
      ("$(TARGETNAME)" != "ntdll")
DLLENTRY=-entry:_DllMainCRTStartupForGS
!  endif
! endif
!endif  # Overflow checks

!ifndef AMD64_CPU_OPTIMIZATION
AMD64_CPU_OPTIMIZATION=
!endif

!if defined(AMD64_STRUCT_PACKING)
MSC_STRUCT_PACKING=$(AMD64_STRUCT_PACKING)
!elseif !defined(MSC_STRUCT_PACKING)
MSC_STRUCT_PACKING=/Zp8
!endif

#
# Allow the default behavior of WCHAR to be overridden.
#

!ifndef NO_WCHAR_T
COMPILER_WCHAR_T=/Zc:wchar_t-
!else
COMPILER_WCHAR_T=
!endif

#
# Turn off the default behavior of WCHAR if managed code version 1.1 is used.
#

!if "$(URT_VER)" == "1.1" && defined(MANAGED_CXX)
COMPILER_WCHAR_T=
!endif


STDFLAGS= /c $(COMPILER_WCHAR_T) /Zl $(MSC_STRUCT_PACKING) /Gy $(CBSTRING) $(MSC_WARNING_LEVEL) $(MSC_CALL_TYPE) \
          $(ERATTA_FLAGS) $(AMD64_CPU_OPTIMIZATION) $(EH_FLAGS) \
          $(RTTI_FLAGS) $(STRING_POOLING) $(BO_FLAGS)

!if $(FREEBUILD)
ASM_DBG_DEFINES=/DDBG=0
!else
AMD64_ADBGFLAGS=$(AMD64_ADBGFLAGS) /Zi
!endif

DBGFLAGS=$(DBGFLAGS) /Zi
AMD64_ADBGFLAGS=$(AMD64_ADBGFLAGS) /Zi

!if !defined(USE_PDB_TO_COMPILE) || "$(USE_PDB_TO_COMPILE)" == "0"
DBGFLAGS=$(DBGFLAGS:/Zi=/Z7)
DBGFLAGS=$(DBGFLAGS:-Zi=/Z7)
!else
USER_C_FLAGS=$(USER_C_FLAGS:/Z7=/Zi)
USER_C_FLAGS=$(USER_C_FLAGS:-Z7=/Zi)
! ifndef USE_MSHTML_PDB_RULES
!  if "$(TARGETTYPE)" == "LIBRARY"
DBGFLAGS=$(DBGFLAGS) /Fd$(TARGETPATH)\$(TARGET_DIRECTORY)\$(TARGETNAME).pdb
!  else
DBGFLAGS=$(DBGFLAGS) /Fd$(OBJ_PATH)\$(_OBJ_DIR)\amd64^\
!  endif
! else
!  ifndef TARGETPDB
TARGETPDB=$(TARGETNAME)
!  endif
DBGFLAGS=$(DBGFLAGS) /Fd$(OBJ_PATH)\$(_OBJ_DIR)\amd64\$(TARGETPDB).pdb
! endif
!endif

!ifndef NO_BROWSER_INFO
! ifdef BROWSER_INFO
DBGFLAGS=$(DBGFLAGS) /FR$(OBJ_PATH)\$(_OBJ_DIR)\amd64^\
! endif
!endif

#
# amd64 option control
#

!IF "$(HALTYPE)" == ""
HALDEF=
!ELSE
HALDEF=/D$(HALTYPE)=1
!ENDIF

STD_DEFINES=/D_WIN64 /D_AMD64_ /DAMD64 $(HALDEF) $(MSC_CALL_DEFINE) $(STD_DEFINES)

CDEFINES=$(STD_DEFINES) $(TARGET_DBG_DEFINES) \
              $(LIBC_DEFINES) $(C_DEFINES) $(NET_C_DEFINES) $(MFC_DEFINES)
CFLAGS=$(AMD64_FLAGS) $(NTAMD64FLAGS) $(STDFLAGS) $(DBGFLAGS) $(PERFFLAGS) $(USER_C_FLAGS) /typedil- /wd4603 /wd4627
AMD64_ASMFLAGS=$(AMD64_ADBGFLAGS) $(STD_DEFINES) $(ASM_DBG_DEFINES)\
              $(TARGET_DBG_DEFINES) $(ASM_DEFINES)

AMD64_CDEFINES=$(CDEFINES)
AMD64_CFLAGS=$(CFLAGS)

!if defined(SUBSTITUTE_AMD64_CC_MANAGED) && defined(MANAGED_CXX)
CC_NAME=$(SUBSTITUTE_AMD64_CC_MANAGED) 
!elseif defined(SUBSTITUTE_AMD64_CC) && !defined(MANAGED_CXX)
CC_NAME=$(SUBSTITUTE_AMD64_CC) 
!elseif !defined(CC_NAME)
CC_NAME = cl 
!endif

C_COMPILER_NAME     = $(CC_NAME)
CXX_COMPILER_NAME   = $(CC_NAME)
C_PREPROCESSOR_NAME = $(CC_NAME)
!ifndef AMD64_ASSEMBLER_NAME
AMD64_ASSEMBLER_NAME  = ml64
!endif

GLOBAL_C_FLAGS = /Iamd64\ /I. $(INCPATH1) $(CDEFINES) $(CFLAGS)

!if !$(FREEBUILD) || defined(BUILD_NO_OPT)
GLOBAL_C_FLAGS = $(GLOBAL_C_FLAGS) /homeparams
!endif

GLOBAL_C_FLAGS=$(CRT_BUILD_FLAGS) $(GLOBAL_C_FLAGS)

NP_COMPILER_FLAGS = $(GLOBAL_C_FLAGS) $(COMPILER_WARNINGS)

CXX_COMPILER_FLAGS_NOPCH = $(NP_COMPILER_FLAGS) $(MSC_CPPFLAGS) $(NTCPPFLAGS) $(MANAGED_CXX_FLAGS)
C_COMPILER_FLAGS_NOPCH = $(NP_COMPILER_FLAGS)
C_COMPILER_FLAGS = $(C_COMPILER_FLAGS_NOPCH) $(PRECOMPILED)
CXX_COMPILER_FLAGS = $(CXX_COMPILER_FLAGS_NOPCH) $(PRECOMPILED_CXX)
C_PREPROCESSOR_FLAGS = $(GLOBAL_C_FLAGS) /EP /Tc

C_PREPROCESSOR = $(C_PREPROCESSOR_NAME) $(C_PREPROCESSOR_FLAGS)
C_ASM_PREPROCESSOR = $(C_PREPROCESSOR_NAME) $(ASM_DEFINES) $(C_PREPROCESSOR_FLAGS)
C_COMPILER     = $(C_COMPILER_NAME) $(C_COMPILER_FLAGS)
NP_C_COMPILER  = $(C_COMPILER_NAME) $(NP_COMPILER_FLAGS)
CXX_COMPILER   = $(CXX_COMPILER_NAME) $(CXX_COMPILER_FLAGS)

AMD64_ASSEMBLER=$(AMD64_ASSEMBLER_NAME) /c /Cx /nologo /Iamd64\ /I. $(INCPATH1) $(AMD64_ASMFLAGS)

ECHO_MSG=$(C_COMPILER) $<
ECHO_MSG_P=$(NP_C_COMPILER) $**
ECHO_CXX_MSG=$(CXX_COMPILER) $<

ECHO_PRECOMPILED_MSG1=$(C_COMPILER_NAME) $(C_COMPILER_FLAGS) /Yl$(TARGETNAME) /Yc$(?F) $(HEADERFILE) \
               $(HEADEROBJ) $(PRECOMPILED_FLAG) $(PRECOMPILED_SOURCEFILE) 

ECHO_PRECOMPILED_MSG2=$(C_COMPILER_NAME) $(C_COMPILER_FLAGS) /Yl$(TARGETNAME) /Yc$(?F) $(HEADERFILE) \
               $(HEADEROBJ) $(PRECOMPILED_FLAG)

{}.c{}.asm:
    $(C_COMPILER_NAME) @<< /Fa $<
$(C_COMPILER_FLAGS: =
)
$(C_COMPILER_FLAGS:##= )
<<NOKEEP

#
# Here is a workaround for a masm code generation bug having to do with
# broken interaction between the preprocessor and the code generator.
#
# The workaround is to generate a preprocessed file first, then assemble
# that.  When the assembler is fixed, delete the following !if-related
# statements and the stuff in the !else block.
#

{..\amd64}.asm{}.pp:
    $(AMD64_ASSEMBLER) /Zs /EP $< > $@

{amd64}.asm{}.pp:
    $(AMD64_ASSEMBLER) /Zs /EP $< > $@

!if 1

{..\amd64}.asm{}.lst:
    $(AMD64_ASSEMBLER) /Fl$@ /Fo$@.obj $<

{amd64}.asm{}.lst:
    $(AMD64_ASSEMBLER) /Fl$@ /Fo$@.obj $<

{..\amd64}.asm{$(_OBJ_DIR)\amd64}.obj:
    $(AMD64_ASSEMBLER) /Fo$@ $<

{amd64}.asm{$(_OBJ_DIR)\amd64}.obj:
    $(AMD64_ASSEMBLER) /Fo$@ $<

{$(_OBJ_DIR)\amd64}.asm{$(_OBJ_DIR)\amd64}.obj:
    $(AMD64_ASSEMBLER) /Fo$@ $<

!else

{..\amd64}.asm{}.lst:
    $(AMD64_ASSEMBLER) /Zs /EP $< > $@.pp
    $(AMD64_ASSEMBLER) /Fl$@ /Fo$@.obj $@.pp
    del $@.pp

{amd64}.asm{}.lst:
    $(AMD64_ASSEMBLER) /Zs /EP $< > $@.pp
    $(AMD64_ASSEMBLER) /Fl$@ /Fo$@.obj $@.pp
    del $@.pp

{..\amd64}.asm{$(_OBJ_DIR)\amd64}.obj:
    $(AMD64_ASSEMBLER) /Zs /EP $< > $@.pp
    $(AMD64_ASSEMBLER) /Fo$@ $@.pp
    del $@.pp

{amd64}.asm{$(_OBJ_DIR)\amd64}.obj:
    $(AMD64_ASSEMBLER) /Zs /EP $< > $@.pp
    $(AMD64_ASSEMBLER) /Fo$@ $@.pp
    del $@.pp

{$(_OBJ_DIR)\amd64}.asm{$(_OBJ_DIR)\amd64}.obj:
    $(AMD64_ASSEMBLER) /Zs /EP $< > $@.pp
    $(AMD64_ASSEMBLER) /Fo$@ $@.pp
    del $@.pp

!endif

!if 1

{..\amd64}.asm{}.lst:
    $(AMD64_ASSEMBLER) /Fl$@ /Fo$@.obj $<

{amd64}.asm{}.lst:
    $(AMD64_ASSEMBLER) /Fl$@ /Fo$@.obj $<

{..\amd64}.asm{$(OBJ_PATH)\$(_OBJ_DIR)\amd64}.obj:
    $(AMD64_ASSEMBLER) /Fo$@ $<

{amd64}.asm{$(OBJ_PATH)\$(_OBJ_DIR)\amd64}.obj:
    $(AMD64_ASSEMBLER) /Fo$@ $<

{$(OBJ_PATH)\$(_OBJ_DIR)\amd64}.asm{$(OBJ_PATH)\$(_OBJ_DIR)\amd64}.obj:
    $(AMD64_ASSEMBLER) /Fo$@ $<

!else

{..\amd64}.asm{}.lst:
    $(AMD64_ASSEMBLER) /Zs /EP $< > $@.pp
    $(AMD64_ASSEMBLER) /Fl$@ $@.pp
    del $@.pp

{amd64}.asm{}.lst:
    $(AMD64_ASSEMBLER) /Zs /EP $< > $@.pp
    $(AMD64_ASSEMBLER) /Fl$@ $@.pp
    del $@.pp

{..\amd64}.asm{$(OBJ_PATH)\$(_OBJ_DIR)\amd64}.obj:
    $(AMD64_ASSEMBLER) /Zs /EP $< > $@.pp
    $(AMD64_ASSEMBLER) /Fo$@ $@.pp
    del $@.pp

{amd64}.asm{$(OBJ_PATH)\$(_OBJ_DIR)\amd64}.obj:
    $(AMD64_ASSEMBLER) /Zs /EP $< > $@.pp
    $(AMD64_ASSEMBLER) /Fo$@ $@.pp
    del $@.pp

{$(OBJ_PATH)\$(_OBJ_DIR)\amd64}.asm{$(OBJ_PATH)\$(_OBJ_DIR)\amd64}.obj:
    $(AMD64_ASSEMBLER) /Zs /EP $< > $@.pp
    $(AMD64_ASSEMBLER) /Fo$@ $@.pp
    del $@.pp

!endif

!if !defined(NTKERNEL_ENTRY)
NTKERNEL_ENTRY=KiSystemStartup
!endif

!IF "$(NTDEBUGTYPE)" == "windbg" || "$(NTDEBUGTYPE)" == "both"
NTKERNEL_LINK_OPTIONS=/entry:$(NTKERNEL_ENTRY)
!ELSE
NTKERNEL_LINK_OPTIONS=/base:0x80100000 /entry:$(NTKERNEL_ENTRY)
!ENDIF

LINKER_FLAGS=$(LINKER_FLAGS)

!if !defined(USE_LOCAL_HOTPATCHBUFFER) && !defined(PURE_FORWARDER_DLL) && !defined(RESOURCE_ONLY_DLL)
LINKER_FLAGS=$(LINKER_FLAGS) $(SDK_LIB_PATH)\hotpatch.obj
!endif

!ifndef USE_LOCAL_FUNCTIONPAD
LINKER_FLAGS=$(LINKER_FLAGS) /functionpadmin:6
!endif

DRIVER_ALIGNMENT=0x0080
KERNEL_ALIGNMENT=0x1000

# SEH support for libs built with an older compiler
!ifdef _NT_TARGET_VERSION
!    if $(_NT_TARGET_VERSION) < 0x600 && $(_NT_TOOLS_VERSION) >= 0x700
TARGETLIBS=$(TARGETLIBS) $(DDK_LIB_PATH)\sehupd.lib
!    endif
!endif
