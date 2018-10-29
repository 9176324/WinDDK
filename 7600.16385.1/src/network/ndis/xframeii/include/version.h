/*
 *  FileName :    version.h
 *
 *  Description:  versioning file
 */

#ifndef VERSION_H
#define VERSION_H

#include "build-version.h"

#define XGE_HAL_VERSION_MAJOR   "2"
#define XGE_HAL_VERSION_MINOR   "5"
#define XGE_HAL_VERSION_FIX "0"
#define XGE_HAL_VERSION_BUILD   GENERATED_BUILD_VERSION
#define XGE_HAL_VERSION XGE_HAL_VERSION_MAJOR"."XGE_HAL_VERSION_MINOR"."\
	        XGE_HAL_VERSION_FIX"."XGE_HAL_VERSION_BUILD
#define XGE_HAL_DESC    XGE_DRIVER_NAME" v."XGE_HAL_VERSION

/* Link Layer versioning */
#include "xgell-version.h"

#endif /* VERSION_H */

