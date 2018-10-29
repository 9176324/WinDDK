/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    fail_library7.c

Abstract:

    Template library that can be used together with a driver for testing the 
    seamless build feature of SDV via defect injection. This library is not 
    functional and not intended as a sample for real software development 
    projects. It only provides a skeleton for building a defective library.

Environment:

    Kernel mode

--*/
#include "fail_library7.h"

VOID
SDVTest_wdf_BufferAfterReqCompletionReadAdd(
    IN  PVOID   Buffer, 
    IN  size_t  Length
    )
{
    RtlZeroMemory(Buffer, Length);

    return;
}

VOID
SDVTest_wdf_MdlAfterReqCompletionReadAdd(
    IN  PMDL    Mdl
    )
{
    PVOID   pVirtualAddress;
    
    pVirtualAddress = MmGetMdlVirtualAddress(Mdl);

    return;
}

VOID
SDVTest_wdf_MemoryAfterReqCompletionWriteAdd(
    IN  WDFMEMORY   Memory,
    IN  size_t      Length
    )
{
    NTSTATUS    status;
    PVOID       pBuffer = NULL;
    
    status = WdfMemoryCopyToBuffer(Memory, 0, pBuffer, Length);

    return;
}

VOID
SDVTest_wdf_MdlAfterReqCompletionWriteAdd(
    IN  PMDL    Mdl
    )
{
    MmPrepareMdlForReuse(Mdl);
}

VOID
SDVTest_wdf_BufferAfterReqCompletionIoctl(
	IN	WDFREQUEST	Request,
	IN	size_t		Length,
	IN	PVOID		Buffer
	)
{
    NTSTATUS    status;

    status = WdfRequestRetrieveInputBuffer(Request, Length, &Buffer, NULL);

    return;
}

VOID
SDVTest_wdf_BufferAfterReqCompletionIntIoctlAdd(
	IN	size_t		Length,
	IN	PVOID		Buffer
	)
{
    ULONG   destBuffer;

    RtlMoveMemory(&destBuffer, Buffer, Length);
}
