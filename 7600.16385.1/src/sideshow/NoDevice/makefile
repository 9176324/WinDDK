#
# Build using BUILD.EXE (Do not edit this section of this file, edit SOURCES)
#
# Ensure that build environment is at least Windows Vista
# For Reference
#     0x500 == Windows 2000
#     0x501 == Windows XP
#     0x502 == Windows Server 2003
#     0x600 == Windows Vista
#     0x601 == Windows 7
#
!IF DEFINED(_NT_TARGET_VERSION)
!   IF $(_NT_TARGET_VERSION)>=0x600
!      INCLUDE $(NTMAKEENV)\makefile.def
!   ELSE
!      INCLUDE $(NTMAKEENV)\makefile.plt
!         IF "$(BUILD_PASS)"=="PASS1"
!            message BUILDMSG: Warning : The sample "$(MAKEDIR)" is not valid for the current OS target.
!         ENDIF
!      ENDIF
!ELSE
!   INCLUDE $(NTMAKEENV)\makefile.def
!ENDIF

