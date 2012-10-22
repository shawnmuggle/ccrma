/*
 *  time.c
 *  GLProfiler
 *
 *  Created by handley on 7/27/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "time.h"
#include <sys/time.h>

uint64_t GetHighResTime()
{
	struct timeval tv;
	gettimeofday(&tv,0);
	uint64_t result = tv.tv_sec;
	result *= 1000000;
	result += tv.tv_usec;
	return result;
}