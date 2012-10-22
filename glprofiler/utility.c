/*
 *  utility.c
 *  GLProfiler
 *
 *  Created by handley on 7/26/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "utility.h"
#include <string.h>
#include <stdlib.h>

void *maligned(size_t size, size_t alignment)
{
	void *ret = 0;
	int r = posix_memalign(&ret, size, alignment);
	if (r != 0)
	{
		*(int *)(uintptr_t)0 = 1;
	}
	
	return ret;
}

uint32_t ConvertFloatColorTo32Bits(float red,float green,float blue,float alpha)
{
	return ((uint32_t)(red * 255.0f) << 24) + ((uint32_t)(green * 255.0f) << 16) + ((uint32_t)(blue * 255.0f) << 8) + ((uint32_t)(alpha * 255.0f));
}
