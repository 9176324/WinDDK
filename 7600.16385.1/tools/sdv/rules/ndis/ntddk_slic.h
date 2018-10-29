/*
	Copyright (c) Microsoft Corporation.  All rights reserved.
*/

#include "sdv_prefixer.h"


#define NT_SUCCESS(Status) (Status >= 0)
#define NULL 0
#define TRUE 	1
#define FALSE	0

#define STATUS_PENDING 259
#define STATUS_SUCCESS 0
#define STATUS_UNSUCCESSFUL (0xC0000001L)

#define NDIS_STATUS_SUCCESS     0
#define NDIS_STATUS_PENDING     259
#define NDIS_STATUS_FAILURE     (-1073741823)

#define NDIS_SUCCESS(Status)    (Status == NDIS_STATUS_SUCCESS)
#define NDIS_PENDING(Status)    (Status == NDIS_STATUS_PENDING)
#define NDIS_FAILURE(Status)    (Status == NDIS_STATUS_FAILURE)
#define NDIS_OK(Status)         (Status >= 0)

#define NdisShutdownPowerOff 	0
#define NdisShutdownBugCheck	1

#define NDIS_STATUS_INVALID_OID                 -1073676265
#define NDIS_STATUS_NOT_SUPPORTED               -1073741637


/*
  IRQL

  The SDV Rules specifies IRQL requirements for various DDIs.

  The IRQLs are positive ordered numbers starting from 0.  The actual
  numbers are processer specific, on X86 ranging from 0 to 31 and on
  AMD64 and IA64 ranging from 0 to 15.  For all three platforms
  PASSIVE_LEVEL, APC_LEVEL and DISPATCH_LEVEL are respectively 0, 1
  and 2.

  DIRQ_LEVEL
  SDV rules only distinguish between the concrete values
  PASSIVE_LEVEL, APC_LEVEL, DISPATCH_LEVEL and the predicate
  SDV_IRQL_ELEVATED_LEVEL for 'everything else' (3-31 on X86, 3-15 on
  AMD64 or IA64).  The SDV rules are unable to distinguish between
  DIRQL, CMC_LEVEL, HIGH_LEVEL, etc.  The OS header files provide the
  equivalent macro definitions except for the SDV_IRQL_ELEVATED_LEVEL
  predicate.

  The SDV OS Model declares the SDV_DIRQL macro for elevating to an
  arbitrary Device IRQL.  The definition of SDV_DIRQL and the
  definition of SDV_IRQL_ELEVATED_LEVEL implies that
  SDV_IRQL_ELEVATED_LEVEL(SDV_DIRQL)==TRUE.

  See the article "Scheduling, Thread Context, and IRQL" for more
  details.
*/

#define PASSIVE_LEVEL 0 
#define APC_LEVEL 1 
#define DISPATCH_LEVEL 2 

#define SDV_IRQL_ELEVATED_LEVEL(irql) 	((irql)!=PASSIVE_LEVEL && (irql)!=APC_LEVEL && (irql)!=DISPATCH_LEVEL)

#define LTDIRQL(irql) 					(!SDV_IRQL_ELEVATED_LEVEL(irql))
#define LTDISPATCH(irql)				((irql)==PASSIVE_LEVEL || (irql)==APC_LEVEL)


#define STACKL Tail.Overlay.CurrentStackLocation

