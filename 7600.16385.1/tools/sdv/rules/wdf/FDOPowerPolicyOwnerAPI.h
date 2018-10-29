/*SDV_HARNESS_METAFILE=SDV_HARNESS_PNP_IO_REQUESTS.h*/
#ifndef SDV_PRE_RUN
#include "..\..\sdv-pre-results.h"

#if (fdodriver_SDV_RESULT == SDV_PASSED && purekmdfdriver_SDV_RESULT==SDV_NA && nonpnpkmdfdriver_SDV_RESULT==SDV_FAILED)
   #define SDV_HARNESS SDV_HARNESS_PNP_IO_REQUESTS
#else
  #pragma message("SDV_NA")
#endif
#else
#pragma message("fdodriver") 
#pragma message("purekmdfdriver")
#pragma message("nonpnpkmdfdriver") 
#endif
