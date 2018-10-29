/*SDV_HARNESS_METAFILE=SDV_PNP_HARNESS_CHECKFORHANG.h*/
#ifndef SDV_PRE_RUN
#include "..\..\sdv-pre-results.h"
#if (pre_miniportcheckforhang_return_SDV_RESULT == SDV_FAILED)
	#pragma message("SDV_PASSED")  
#else
	#define SDV_HARNESS SDV_PNP_HARNESS_CHECKFORHANG
#endif
#else
#pragma message("pre_miniportcheckforhang_return")
#endif
