//
// PREfast Examples
// Copyright c Microsoft Corporation.  All rights reserved.
//

void drivers_test1()
{
	// should not be able to use floating point without protecting it.
	float a;
	a = 6.0;
}

// Declare ProbeForRead simply so we don't have to include the large header
// for this test case.
void
  ProbeForRead(
    const void *Address,
    size_t Length,
    unsigned long Alignment);

void drivers_test2()
{
    ProbeForRead(0, 100, 4);
}
