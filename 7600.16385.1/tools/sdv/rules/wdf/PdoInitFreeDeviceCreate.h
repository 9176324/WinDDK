#ifndef SDV_PRE_RUN
#include "..\..\sdv-pre-results.h"
#if (purekmdfdriver_SDV_RESULT == SDV_NA && nonpnpkmdfdriver_SDV_RESULT==SDV_FAILED)
   #define SDV_HARNESS SDV_HARNESS_PNP_IO_REQUEST
   #define SDV_DEVICE_INIT_NULL
   #define init_free 1
#else
  #pragma message("SDV_NA")  
#endif
#else
#pragma message("purekmdfdriver")
#pragma message("nonpnpkmdfdriver")
#endif
