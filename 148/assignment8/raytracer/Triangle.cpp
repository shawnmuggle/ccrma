//
//  Triangle.cpp
//  raytracer
//
//  Created by Michael Rotondo on 12/6/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#include <iostream>
#include "Triangle.h"

Triangle::Triangle(STPoint3 const& v1, STPoint3 const& v2, STPoint3 const& v3)
{
    v[0] = v1;
    v[1] = v2;
    v[2] = v3;
}

bool Triangle::intersectionWithRay(Ray r, Intersection *outIntersection)
{
    float a = v[0].x - v[1].x;
    float b = v[0].y - v[1].y;
    float c = v[0].z - v[1].z;
    float d = v[0].x - v[2].x;
    float e = v[0].y - v[2].y;
    float f = v[0].z - v[2].z;
    float g = r.d.x;
    float h = r.d.y;
    float i = r.d.z;
    float j = v[0].x - r.e.x;
    float k = v[0].y - r.e.y;
    float l = v[0].z - r.e.z;
    
    float ak_jb = a * k - j * b;
    float jc_al = j * c - a * l;
    float bl_kc = b * l - k * c;
    float ei_hf = e * i - h * f;
    float gf_di = g * f - d * i;
    float dh_eg = d * h - e * g;
    
    float M = a * ei_hf + b * gf_di + c * dh_eg;
    
    float t = -(f * ak_jb + e * jc_al + d * bl_kc) / M;
    if (!r.isValidT(t))
        return false;
    
    float gamma = (i * ak_jb + h * jc_al + g * bl_kc) / M;
    if (gamma < 0 || gamma > 1)
        return false;
    
    float beta = (j * ei_hf + k * gf_di + l * dh_eg) / M;
    if (beta < 0 || beta > 1 - gamma)
        return false;
    
    outIntersection->t = t;
    outIntersection->position = STPoint3((1.0f - beta - gamma) * STVector3(v[0]) + beta * STVector3(v[1]) + gamma * STVector3(v[2]));
    outIntersection->normal = STVector3::Cross(v[2] - v[0], v[1] - v[0]);
    outIntersection->normal.Normalize();
    
    return true;
}

void Triangle::printDescription()
{
    printf("HI I AM TRIANGLE\n");
}