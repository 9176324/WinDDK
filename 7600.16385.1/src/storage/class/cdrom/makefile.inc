$(OBJ_PATH)\$(O)\$(INF_NAME).inf: $(INF_NAME).inf
    copy $? $@
    stampinf -f $@ -a $(_BUILDARCH) -k $(KMDF_VERSION_MAJOR).$(KMDF_VERSION_MINOR)

