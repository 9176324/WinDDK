!IF 0

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    makefile.

!ENDIF

# This sample is not supported on IA64
DDK_BLOCK_ON_IA64=1

!IF DEFINED(_NT_TARGET_VERSION)
!	IF $(_NT_TARGET_VERSION)>=0x600
!		INCLUDE $(NTMAKEENV)\makefile.def
!	ELSE
#               Only warn once per directory
!               INCLUDE $(NTMAKEENV)\makefile.plt
!               IF "$(BUILD_PASS)"=="PASS1"
!		    message BUILDMSG: Warning : The sample "$(MAKEDIR)" is not valid for the current OS target.
!               ENDIF
!	ENDIF
!ELSE
!	INCLUDE $(NTMAKEENV)\makefile.def
!ENDIF

