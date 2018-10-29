/*SDV_HARNESS_METAFILE=sdv_flat_simple_harness.h*/
#ifndef SDV_PRE_RUN
#include "..\..\sdv-pre-results.h"
#if (startioroutine_SDV_RESULT == SDV_FAILED || dispatchroutine_SDV_RESULT == SDV_FAILED || isrroutine_SDV_RESULT == SDV_FAILED || kedpcroutine_SDV_RESULT == SDV_FAILED || iodpcroutine_SDV_RESULT  == SDV_FAILED || iocompletion_SDV_RESULT == SDV_FAILED || workerthread_SDV_RESULT == SDV_FAILED)
  #define SDV_FLAT_HARNESS_MODIFIER_NO_DRIVER_CANCEL
  #define SDV_HARNESS SDV_FLAT_SIMPLE_HARNESS
  #define SDV_FLAT_HARNESS_COMPLETION_ROUTINE
#else
  #pragma message("SDV_NA")  
#endif
#else
#pragma message("startioroutine")
#pragma message("dispatchroutine")
#pragma message("isrroutine")
#pragma message("kedpcroutine")
#pragma message("iodpcroutine") 
#pragma message("iocompletion")
#pragma message("workerthread")
#endif
