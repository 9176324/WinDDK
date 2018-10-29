/*SDV_HARNESS_METAFILE=SDV_HARNESS SDV_WDF_FLAT_SIMPLE_IO_HARNESS.h*/
#ifndef SDV_PRE_RUN
#include "..\..\sdv-pre-results.h"
#if (manualqueuedispatching_SDV_RESULT != SDV_FAILED)
    #define SDV_HARNESS SDV_WDF_FLAT_SIMPLE_IO_HARNESS
#else
  #pragma message("SDV_NA")
#endif
#else
#pragma message("manualqueuedispatching")
#endif
