//
//  Shape.h
//  raytracer
//
//  Created by Michael Rotondo on 12/5/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#ifndef raytracer_Shape_h
#define raytracer_Shape_h

#include "Ray.h"
#include "Intersection.h"

class Shape {
    
public:
    virtual bool intersectionWithRay(Ray r, Intersection *outIntersection) = 0;
    virtual void printDescription() = 0;
};

#endif
