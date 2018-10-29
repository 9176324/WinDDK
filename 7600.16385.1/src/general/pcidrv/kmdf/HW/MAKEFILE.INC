_LNG=$(LANGUAGE)
_INX=..
STAMP=stampinf -f $@ -a $(_BUILDARCH) -k $(KMDF_VERSION_MAJOR).$(KMDF_VERSION_MINOR)

$(OBJ_PATH)\$(O)\$(INF_NAME1).inf: $(_INX)\$(INF_NAME1).inx 
    copy $(_INX)\$(@B).inx $@
    $(STAMP)

$(OBJ_PATH)\$(O)\$(INF_NAME2).inf: $(_INX)\$(INF_NAME2).inx 
    copy $(_INX)\$(@B).inx $@
    $(STAMP)

$(OBJ_PATH)\$(O)\pcidrv.bmf : ..\pcidrv.mof
    mofcomp -B:$@ ..\pcidrv.mof
    wmimofck $@   








