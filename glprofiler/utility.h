/*
 *  utility.h
 *  GLProfiler
 *
 *  Created by handley on 7/26/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef UTILITY_H
#define UTILITY_H

#include <sys/types.h>

//mem utils
void *maligned(size_t size, size_t alignment);

//gl utils
uint32_t ConvertFloatColorTo32Bits(float red,float green,float blue,float alpha);

//matrix funcs
void Identity(float *mat);
void CopyMat(float *dest, float *src);
void MultMat(float *dest, float *a, float *b);

#endif
