//
// PREfast Examples
// Copyright © Microsoft Corporation.  All rights reserved.
//

#pragma warning(disable:4700) // so we can see PREfast's equivalent warning

int test2()
{
	int i = 1;
	int j, k = 3;

	if (k)
	{
		i = j;
		goto CleanUp;
	}
	else
	{
		j = 2;
	}

	i = j + 2;

CleanUp:

    return j;
}
