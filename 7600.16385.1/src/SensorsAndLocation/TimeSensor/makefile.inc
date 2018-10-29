.SUFFIXES: .inx

STAMP=stampinf

.inx{$(OBJ_PATH)\$(O)}.inf:
    copy $(@B).inx $@
    $(STAMP) -f $@ -a $(_BUILDARCH) -u $(UMDF_VERSION_MAJOR).$(UMDF_VERSION_MINOR).0



