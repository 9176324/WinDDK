/*SDV_HARNESS_METAFILE=SDV_HARNESS_DRIVER_CREATE.h*/
#ifndef SDV_PRE_RUN
#include "..\..\sdv-pre-results.h"
#if (purekmdfdriver_SDV_RESULT == SDV_NA)
   #define SDV_HARNESS SDV_HARNESS_DRIVER_CREATE
   #define init_free 1
#else
  #pragma message("SDV_NA")  
#endif
#else
#pragma message("purekmdfdriver")
#endif
