/*
 *  Utilities.cpp
 *  GLProfiler
 *
 *  Created by handley on 9/28/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "Utilities.h"

void GetMakeProportionalRectSize(float maxW, float maxH, float w, float h, float *rw, float *rh)
{
	if (h * maxW > maxH * w)
	{
		//we're more widescreen than tallscreen, relative to our viewport. That means the viewport's height will be the rect's height... and the width will be scaled
		*rw = (maxH / h * w);
		*rh = maxH;
	}
	else
	{
		*rw = maxW;
		*rh = (maxW / w * h);
	}
}
