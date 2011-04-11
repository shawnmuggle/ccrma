/*
 *  at_mglUtils.h
 *  AngryTruck
 *
 *  Created by njb on 2/28/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __MGLUTILS_H__
#define __MGLUTILS_H__

#include "at_mglTypes.h"
#include <stdio.h>
#include <string>



// Computes the transpose of a matrix
void tMatrix(MGLfloat * matrix);


/**
 * A utility function for error reporting.
 */
void MGL_ERROR(const char* description);
#endif