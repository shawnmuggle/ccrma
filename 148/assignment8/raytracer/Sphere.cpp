//
//  Sphere.cpp
//  raytracer
//
//  Created by Michael Rotondo on 12/5/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#include <iostream>
#include "Sphere.h"

bool Sphere::intersectionWithRay(Ray r, Intersection *outIntersection)
{
    float discriminant = (powf(STVector3::Dot(r.d, (r.e - position)), 2.0f) - 
                          STVector3::Dot(r.d, r.d) * (STVector3::Dot(r.e - position, r.e - position) -
                          powf(radius, 2.0f)));
    
    if (discriminant < 0.0f)
    {
        return false;
    }

    // Subtract the discriminant instead of adding it because we want the minimum t value
    float t = (STVector3::Dot(-r.d, r.e - position) - sqrtf(discriminant)) / STVector3::Dot(r.d, r.d);

    if (!r.isValidT(t)) {
        return false;
    }
    
    STPoint3 p = r.pointOnRayAtT(t);
    outIntersection->t = t;
    outIntersection->position = p;
    outIntersection->normal = (p - position) / radius; // Normalized
    
    return true;
}

void Sphere::printDescription()
{
    printf("HI I AM A SPHERE");
}