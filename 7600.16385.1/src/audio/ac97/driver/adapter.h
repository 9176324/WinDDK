/********************************************************************************
**    Copyright (c) 1998-1999 Microsoft Corporation. All Rights Reserved.
**
**       Portions Copyright (c) 1998-1999 Intel Corporation
**
********************************************************************************/

#ifndef _ADAPTER_H_
#define _ADAPTER_H_

#include "shared.h"

/*****************************************************************************
 * Defines
 *****************************************************************************
 */
const ULONG MAX_MINIPORTS = 2;

/*****************************************************************************
 * Externals
 *****************************************************************************
 */
extern NTSTATUS CreateAC97MiniportWaveRT
(
    OUT     PUNKNOWN *  Unknown,
    IN      REFCLSID,
    IN      PUNKNOWN    UnknownOuter    OPTIONAL,
    IN      POOL_TYPE   PoolType
);

extern NTSTATUS CreateAC97MiniportWavePCI
(
    OUT     PUNKNOWN *  Unknown,
    IN      REFCLSID,
    IN      PUNKNOWN    UnknownOuter    OPTIONAL,
    IN      POOL_TYPE   PoolType
);

extern NTSTATUS CreateAC97MiniportTopology
(
    OUT     PUNKNOWN *  Unknown,
    IN      REFCLSID,
    IN      PUNKNOWN    UnknownOuter    OPTIONAL,
    IN      POOL_TYPE   PoolType
);

#endif  //_ADAPTER_H_

