#ifndef SDV_PRE_RUN
#include "..\..\sdv-pre-results.h"

#if (aliaswithintimerdpc_SDV_RESULT == SDV_FAILED && aliaswithindispatch_SDV_RESULT == SDV_FAILED && purekmdfdriver_SDV_RESULT == SDV_NA&&manualqueuedispatching_SDV_RESULT != SDV_FAILED)
   #define SDV_HARNESS SDV_HARNESS_PNP_DEFERRED_IO_REQUESTS
#else
  #pragma message("SDV_NA")
#endif
#else
#pragma message("aliaswithintimerdpc")
#pragma message("aliaswithindispatch")
#pragma message("purekmdfdriver")
#pragma message("manualqueuedispatching")
#endif



