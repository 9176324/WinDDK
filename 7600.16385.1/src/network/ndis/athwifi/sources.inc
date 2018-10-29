#
# Optimize and Level 4 warnings
#
MSC_OPTIMIZATION=/Oi /Od
MSC_WARNING_LEVEL=/W4

#
# This is a NDIS 6.20 driver that also supports loading on NDIS 6.1. So we 
# define both NDIS620_MINIPORT & NDIS61_MINIPORT
#
C_DEFINES=$(C_DEFINES) -DNDIS620_MINIPORT=1 -DNDIS61_MINIPORT=1

#
# For Vista compatible driver
#
#C_DEFINES=$(C_DEFINES) -DNO_READ_WRITE_LOCK=1

#
# Add any PreProcessor defines here
#
C_DEFINES=$(C_DEFINES) -DNDIS_MINIPORT_DRIVER -DBINARY_COMPATIBLE=0

#
# To enable tracing, uncomment the following two lines
#
C_DEFINES=$(C_DEFINES) -DDOT11_TRACE_ENABLED=1
RUN_WPP=$(SOURCES) -km -func:MpTrace(COMPID,LEVEL,(MSG,...))

#
# To enable dbg print, uncomment the following line
#
#C_DEFINES=$(C_DEFINES) -DDBG


