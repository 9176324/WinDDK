#pragma warning(disable:4214)   // bit field types other than int

#pragma warning(disable:4201)   // nameless struct/union
#pragma warning(disable:4115)   // named type definition in parentheses
#pragma warning(disable:4127)   // conditional expression is constant
#pragma warning(disable:4054)   // cast of function pointer to PVOID
#pragma warning(disable:4244)   // conversion from 'int' to 'BOOLEAN', possible loss of data
#pragma warning(disable:4206)   // nonstandard extension used : translation unit is empty

#include "ndis.h"
#include "ntddk.h"
#include "debug.h"
#include "ndisprot.h"
#include "macros.h"
#include "nuiouser.h"
#include <wdmsec.h>
