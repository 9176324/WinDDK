/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    fail_library6.c

Abstract:

    Template library that can be used together with a driver for testing the 
    seamless build feature of SDV via defect injection. This library is not 
    functional and not intended as a sample for real software development 
    projects. It only provides a skeleton for building a defective library.

Environment:

    Kernel mode

--*/
#include "fail_library6.h"

VOID
SDVTest_wdf_BufferAfterReqCompletionRead(
	IN	WDFREQUEST	Request,
	IN	size_t		Length,
	IN	PVOID		Buffer
	)
{
	NTSTATUS	status;

	status = WdfRequestRetrieveInputBuffer(Request, Length, &Buffer, NULL);

	return;
}

VOID
SDVTest_wdf_DeferredRequestCompletedNoAlias(
    IN  WDFREQUEST  Request
    )
{
    UNREFERENCED_PARAMETER(Request);

    //
    // Do not complete the deferred request inside this routine. This will 
    // induce an error when the unload routine is called
    //
    return;
}

VOID
SDVTest_wdf_MemoryAfterReqCompletionRead(
	IN	WDFREQUEST	Request, 
	IN	WDFMEMORY	Memory
	)
{
	NTSTATUS	status;

	status = WdfRequestRetrieveInputMemory(Request, &Memory);

	return;
}

VOID
SDVTest_wdf_MdlAfterReqCompletionRead(
	IN	WDFREQUEST	Request, 
	IN	PMDL		Mdl
	)
{
	NTSTATUS status;

	status = WdfRequestRetrieveOutputWdmMdl(Request, &Mdl);

	return;
}

VOID
SDVTest_wdf_BufferAfterReqCompletionWrite(
	IN	WDFREQUEST	Request,
	IN	size_t		Length,
	IN	PVOID		Buffer
	)
{
	NTSTATUS	status;

	status = WdfRequestRetrieveOutputBuffer(Request, Length, &Buffer, NULL);

	return;
}

VOID
SDVTest_wdf_MdlAfterReqCompletionWrite(
	IN	WDFREQUEST	Request, 
	IN	PMDL		Mdl
	)
{
	NTSTATUS status;

	status = WdfRequestRetrieveInputWdmMdl(Request, &Mdl);

	return;
}

VOID
SDVTest_wdf_MemoryAfterReqCompletionIoctl(
	IN	WDFREQUEST	Request, 
	IN	WDFMEMORY	Memory
	)
{
	NTSTATUS	status;

	status = WdfRequestRetrieveInputMemory(Request, &Memory);

	return;
}
