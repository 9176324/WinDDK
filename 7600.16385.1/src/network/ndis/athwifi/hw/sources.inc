INCLUDES= $(INCLUDES)        \
    ..;                      \
    ..\..\inc;               \
    $(DDK_INC_PATH)

#
# For reset and send tracking
#
# C_DEFINES=$(C_DEFINES) -DMP_RECORD_ENABLED=1

SOURCES=                     \
    ..\hw_utils.c            \
    ..\hw_context.c          \
    ..\hw_main.c             \
    ..\hw_send.c             \
    ..\hw_recv.c             \
    ..\hw_isr.c              \
    ..\hw_oids.c             \
    ..\hw_crypto.c           \
    ..\hw_ccmp.c             \
    ..\hw_phy.c              \
    ..\hw_mac.c              \
    ..\hw_rate.c

