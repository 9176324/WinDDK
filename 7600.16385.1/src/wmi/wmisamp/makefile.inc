#
# generate auxilary files from the binary mof (bmf) file
#
# wmidata.h has definitions for the guids, data structures and definitions
#            of the method ids.
#
# wmisamp.vbs is a ready to run vbscript applet that will query all classes
#            in the mof. Once you install your driver, run the script
#            at the command line by typing: wmisamp.vbs.

LNG=$(LANGUAGE)
_INX=.
STAMP=stampinf -f $@ -a $(_BUILDARCH) -k $(KMDF_VERSION_MAJOR).$(KMDF_VERSION_MINOR)



$(OBJ_PATH)\$(O)\$(INF_NAME).inf: $(_INX)\$(INF_NAME).inx
        copy $(_INX)\$(@B).inx $@
        $(STAMP)

$(OBJ_PATH)\$O\wmisamp.bmf : $(OBJ_PATH)\$O\wmisamp.mof
        del /q wmidata.h
        del /q wmisamp.vbs
        del /q wmisamp.x
        mofcomp -WMI -B:$(OBJ_PATH)\$O\wmisamp.bmf wmisamp.mof
        wmimofck -m -h$(OBJ_PATH)\$(O)\wmidata.h -t$(OBJ_PATH)\$(O)\wmisamp.vbs -x$(OBJ_PATH)\$(O)\wmisamp.x -w$(OBJ_PATH)\$O\htm $(OBJ_PATH)\$O\wmisamp.bmf

$(OBJ_PATH)\$O\wmisamp.vbs $(OBJ_PATH)\$O\wmidata.h $(OBJ_PATH)\$O\wmisamp.x: $(OBJ_PATH)\$O\wmisamp.bmf

