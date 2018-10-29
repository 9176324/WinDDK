#ifndef SDV_PRE_RUN
#include "..\..\sdv-pre-results.h"
#if (isusbdriver_SDV_RESULT == SDV_FAILED)
   #define SDV_HARNESS SDV_USB_HARNESS
#else
  #pragma message("SDV_NA")  
#endif
#else
#pragma message("isusbdriver")
#endif
