/*SDV_HARNESS_METAFILE=sdv_wdf_semi_flat_harness.h*/
#ifndef SDV_PRE_RUN
#include "..\..\sdv-pre-results.h"
#if (manualqueuedispatching_SDV_RESULT != SDV_FAILED)
    #define SDV_HARNESS SDV_WDF_IRQL_SEMI_FLAT_HARNESS
    #define SDV_WDF_IRQL_SEMI_FLAT_HARNESS_MODIFIER_NO_MALLOC_sdv_WDF_IO_QUEUE_CONFIG_INIT
    #define SDV_WDF_IRQL_SEMI_FLAT_HARNESS_MODIFIER_NO_MALLOC_sdv_WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE
    #define NO_fun_WDF_OBJECT_CONTEXT_CLEANUP
#else
  #pragma message("SDV_NA")
#endif
#else
#pragma message("manualqueuedispatching")
#endif
