_LNG=$(LANGUAGE)
_INX=.
STAMP=stampinf -f $@ -a $(_BUILDARCH) -k $(KMDF_VERSION_MAJOR).$(KMDF_VERSION_MINOR)

$(OBJ_PATH)\$(O)\$(INF_NAME).inf: $(_INX)\$(INF_NAME).inx 
    copy $(_INX)\$(@B).inx $@
    $(STAMP)

$(OBJ_PATH)\$(O)\firefly.bmf : firefly.mof
    mofcomp -B:$@ firefly.mof
    wmimofck $@   
    wmimofck -h$(OBJ_PATH)\$(O)\fireflymof.h $@
