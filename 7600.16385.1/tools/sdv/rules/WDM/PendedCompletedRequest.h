/*SDV_HARNESS_METAFILE=sdv_flat_dispatch_harness.h*/
#ifndef SDV_PRE_RUN
#include "..\..\sdv-pre-results.h"
#if (dispatchroutine_SDV_RESULT == SDV_FAILED)
   #define SDV_FLAT_HARNESS_MODIFIER_NO_DRIVER_CANCEL
   #define SDV_HARNESS SDV_FLAT_DISPATCH_HARNESS
#else
  #pragma message("SDV_NA")  
#endif
#else
#pragma message("dispatchroutine")
#endif
