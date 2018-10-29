## Sign driver cat file after PASS2 (link)

!if "$(BUILD_PASS)" == "PASS2"

$(OBJ_PATH)\$(O)\avshws.cat:
        copy /y avshws.inf $(OBJ_PATH)\$(O)\avshws.inf
	deltacat.cmd $(OBJ_PATH)\$(O)
	move /y $(OBJ_PATH)\$(O)\delta.cat $(OBJ_PATH)\$(O)\avshws.cat
        move /y $(OBJ_PATH)\$(O)\delta.cdf $(OBJ_PATH)\$(O)\avshws.cdf
        $(SIGNCODE_CMD) 
!else

$(OBJ_PATH)\$(O)\avshws.cat:

!endif
