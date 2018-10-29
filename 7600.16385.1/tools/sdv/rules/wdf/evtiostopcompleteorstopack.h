#ifndef SDV_PRE_RUN
#include "..\..\sdv-pre-results.h"
#if (purekmdfdriver_SDV_RESULT == SDV_NA)
  #define SDV_HARNESS SDV_HARNESS_PNP_DEFERRED_IO_REQUESTS
  #define EvtIoStopCompleteOrStopAck_Context
#else
  #pragma message("SDV_NA")  
#endif
#else
#pragma message("purekmdfdriver")
#endif
