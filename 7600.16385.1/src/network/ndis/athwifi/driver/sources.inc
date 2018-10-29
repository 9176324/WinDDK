INCLUDES= $(INCLUDES)        \
    ..;                      \
    ..\..\inc;               \
    $(DDK_INC_PATH)

SOURCES=                     \
    ..\glb_utils.c           \
    ..\mp_dbg.c              \
    ..\mp_record.c           \
    ..\mp_rwlock.c           \
    ..\mp_main.c             \
    ..\mp_pnp.c              \
    ..\mp_oids.c             \
    ..\port_main.c           \
    ..\port_oids.c           \
    ..\base_port_main.c      \
    ..\base_port_oids.c      \
    ..\base_port_send.c      \
    ..\base_port_recv.c      \
    ..\helper_port_main.c    \
    ..\helper_port_scan.c    \
    ..\helper_port_bsslist.c

TARGETLIBS=\
    $(DDK_LIB_PATH)\ndis.lib             \
    $(OBJ_PATH)\..\..\extsta\$O\extsta.lib  \
    $(OBJ_PATH)\..\..\extap\$O\extap.lib  \
    $(OBJ_PATH)\..\..\hvl\$O\hvl.lib

