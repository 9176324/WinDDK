/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    fail_library6.h

Environment:

    Kernel mode

--*/


#ifdef __cplusplus
extern "C" {
#endif
#include <NTDDK.h>  
#include <wdf.h>
#ifdef __cplusplus
}
#endif

VOID
SDVTest_wdf_BufferAfterReqCompletionRead(
	IN	WDFREQUEST	Request,
	IN	size_t		Length,
	IN	PVOID		Buffer
	);

VOID
SDVTest_wdf_DeferredRequestCompletedNoAlias(
    IN  WDFREQUEST  Request
    );

VOID
SDVTest_wdf_MemoryAfterReqCompletionRead(
	IN	WDFREQUEST	Request, 
	IN	WDFMEMORY	Memory
	);

VOID
SDVTest_wdf_MdlAfterReqCompletionRead(
	IN	WDFREQUEST	Request, 
	IN	PMDL		Mdl
	);

VOID
SDVTest_wdf_BufferAfterReqCompletionWrite(
	IN	WDFREQUEST	Request,
	IN	size_t		Length,
	IN	PVOID		Buffer
	);

VOID
SDVTest_wdf_MdlAfterReqCompletionWrite(
	IN	WDFREQUEST	Request, 
	IN	PMDL		Mdl
	);

VOID
SDVTest_wdf_MemoryAfterReqCompletionIoctl(
	IN	WDFREQUEST	Request, 
	IN	WDFMEMORY	Memory
	);
