/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    fail_library7.h

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
SDVTest_wdf_BufferAfterReqCompletionReadAdd(
    IN  PVOID   Buffer, 
    IN  size_t  Length
    );

VOID
SDVTest_wdf_MdlAfterReqCompletionReadAdd(
    IN  PMDL    Mdl
    );

VOID
SDVTest_wdf_MemoryAfterReqCompletionWriteAdd(
    IN  WDFMEMORY   Memory,
    IN  size_t      Length
    );

VOID
SDVTest_wdf_MdlAfterReqCompletionWriteAdd(
    IN  PMDL    Mdl
    );

VOID
SDVTest_wdf_BufferAfterReqCompletionIoctl(
	IN	WDFREQUEST	Request,
	IN	size_t		Length,
	IN	PVOID		Buffer
	);

VOID
SDVTest_wdf_BufferAfterReqCompletionIntIoctlAdd(
	IN	size_t		Length,
	IN	PVOID		Buffer
	);
