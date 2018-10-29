/*SDV_HARNESS_METAFILE=sdv_flat_harness.h*/
#ifndef SDV_PRE_RUN
#include "..\..\sdv-pre-results.h"
#if (startioroutine_SDV_RESULT == SDV_FAILED || \
     dispatchroutine_SDV_RESULT == SDV_FAILED || \
     isrroutine_SDV_RESULT == SDV_FAILED || \
     kedpcroutine_SDV_RESULT == SDV_FAILED || \
     cancelroutine_SDV_RESULT == SDV_FAILED || \
     iodpcroutine_SDV_RESULT  == SDV_FAILED || \
     iocompletion_SDV_RESULT == SDV_FAILED || \
     workerthread_SDV_RESULT == SDV_FAILED|| \
     checkadddevice_SDV_RESULT == SDV_FAILED|| \
     checkdriverunload_SDV_RESULT == SDV_FAILED|| \
     checkirpmjpnp_SDV_RESULT == SDV_FAILED)
  #define SDV_HARNESS SDV_FLAT_HARNESS
#else
  #pragma message("SDV_NA")  
#endif
#else
#pragma message("startioroutine")
#pragma message("dispatchroutine")
#pragma message("isrroutine")
#pragma message("kedpcroutine")
#pragma message("cancelroutine")
#pragma message("iodpcroutine") 
#pragma message("iocompletion")
#pragma message("workerthread")
#pragma message("checkadddevice")
#pragma message("checkdriverunload")
#pragma message("checkirpmjpnp")
#endif
