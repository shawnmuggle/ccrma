//
//  Triangle.h
//  raytracer
//
//  Created by Michael Rotondo on 12/6/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#ifndef raytracer_Triangle_h
#define raytracer_Triangle_h

#include "Shape.h"
#include "STPoint3.h"

class Triangle : public Shape {
    STPoint3 v[3];
    
public:
    Triangle(STPoint3 const& v1, STPoint3 const& v2, STPoint3 const& v3);
    bool intersectionWithRay(Ray r, Intersection *outIntersection);
    void printDescription();

};

#endif
