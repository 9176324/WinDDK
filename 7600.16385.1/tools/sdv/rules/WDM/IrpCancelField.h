/*SDV_HARNESS_METAFILE=SDV_FLAT_DISPATCH_STARTIO_HARNESS.h*/
#ifndef SDV_PRE_RUN
#include "..\..\sdv-pre-results.h"
#if (startioroutine_SDV_RESULT == SDV_FAILED || dispatchroutine_SDV_RESULT == SDV_FAILED)
   #define SDV_HARNESS SDV_FLAT_DISPATCH_STARTIO_HARNESS
#else
  #pragma message("SDV_NA")  
#endif
#else
#pragma message("startioroutine")
#pragma message("dispatchroutine")
#endif
