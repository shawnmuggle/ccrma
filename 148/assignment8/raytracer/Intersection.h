//
//  Intersection.h
//  raytracer
//
//  Created by Michael Rotondo on 12/5/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#ifndef raytracer_Intersection_h
#define raytracer_Intersection_h

#include "stForward.h"

class Intersection {
    
public:
    float t;
    STPoint3 position;
    STVector3 normal;
};

#endif
