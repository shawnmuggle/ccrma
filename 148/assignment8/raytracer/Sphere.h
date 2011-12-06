//
//  Sphere.h
//  raytracer
//
//  Created by Michael Rotondo on 12/5/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#ifndef raytracer_Sphere_h
#define raytracer_Sphere_h

#include "STPoint3.h"
#include "STColor4f.h"
#include "Shape.h"

class Sphere : public Shape {
    STPoint3 position;
    float radius;
    
public:
    Sphere(STPoint3 position, float radius) : position(position), radius(radius) {}
    bool intersectionWithRay(Ray r, Intersection *outIntersection);
    void printDescription();
};

#endif
