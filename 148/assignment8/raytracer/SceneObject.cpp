//
//  SceneObject.cpp
//  raytracer
//
//  Created by Michael Rotondo on 12/6/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#include <iostream>
#include "SceneObject.h"

bool SceneObject::intersectionWithRay(Ray r, Intersection *outIntersection)
{
    Ray transformedRay(transform.Inverse() * r.e, transform.Inverse() * r.d, r.minValidT, r.maxValidT);
    
    bool intersects = shape->intersectionWithRay(transformedRay, outIntersection);
    
    outIntersection->position = transform * outIntersection->position;
    STVector3 normal = transform.Inverse().Transpose() * outIntersection->normal;
    normal.Normalize();
    outIntersection->normal = normal;
    
    return intersects;
}