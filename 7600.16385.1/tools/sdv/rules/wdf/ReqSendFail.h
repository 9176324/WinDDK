/*SDV_HARNESS_METAFILE=SDV_WDF_FLAT_SIMPLE_HARNESS.h*/
#ifndef SDV_PRE_RUN
#include "..\..\sdv-pre-results.h"

#if (purekmdfdriver_SDV_RESULT == SDV_NA&&manualqueuedispatching_SDV_RESULT != SDV_FAILED)
    #define SDV_HARNESS SDV_WDF_FLAT_SIMPLE_HARNESS
    #define SDV_COMPLEX_REQUEST_OBJ
#else
  #pragma message("SDV_NA")
#endif
#else
#pragma message("purekmdfdriver")
#pragma message("manualqueuedispatching")
#endif
