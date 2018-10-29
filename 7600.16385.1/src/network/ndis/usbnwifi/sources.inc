#
# Optimize and Level 4 warnings
#
MSC_OPTIMIZATION=/Oi /Od
MSC_WARNING_LEVEL=/W4


#
# NDIS 6.0 driver
#
C_DEFINES=$(C_DEFINES) -DNDIS60_MINIPORT=1 -DNDIS60=1

#
# Add any PreProcessor defines here
#
C_DEFINES=$(C_DEFINES) -DNDIS_MINIPORT_DRIVER -DBINARY_COMPATIBLE=0


#
# To enable checked build of the driver uncomment the following line
#
#C_DEFINES=$(C_DEFINES) -DDBG

#
# To enable tracing, uncomment the following two lines
#
#C_DEFINES=$(C_DEFINES) -DDOT11_TRACE_ENABLED=1
#RUN_WPP=$(SOURCES) -km -func:MpTrace(COMPID,LEVEL,(MSG,...))

