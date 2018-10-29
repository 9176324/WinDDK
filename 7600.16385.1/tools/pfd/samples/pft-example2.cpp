//
// PREfast Examples
// Copyright © Microsoft Corporation.  All rights reserved.
//

#pragma warning(disable:4700) // so we can see PREfast's equivalent warning

#include <stdlib.h>
struct S {
	int a;
};

void test()
{
	int *p, a;
	S *ps, c;

	if (a)
	{
		p = &a;
	}
	else
	{
		ps = (struct S*)malloc(sizeof(struct S));
	}

	if (p)
	{
		ps = &c;
	}

	*p;
	a = (((ps)))->a;

	return;
}
