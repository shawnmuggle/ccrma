/*
 *  at_mglUtils.cpp
 *  AngryTruck
 *
 *  Created by njb on 2/28/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "at_mglUtils.h"

void tMatrix(MGLfloat * matrix)
{
    MGLfloat m[16];
    memcpy(m, matrix, sizeof(MGLfloat)*16);
    
    matrix[1] = m[4];
    matrix[2] = m[8];
    matrix[3] = m[12];
    
    matrix[4] = m[1];
    matrix[6] = m[9];
    matrix[7] = m[13];
    
    matrix[8] = m[2];
    matrix[9] = m[6];
    matrix[11] = m[14];
    
    matrix[12] = m[3];
    matrix[13] = m[7];
    matrix[14] = m[11];
}


/**
 * A utility function for error reporting.
 */
void MGL_ERROR(const char* description) {
    printf("ERROR: %s\n", description);
    exit(1);
}