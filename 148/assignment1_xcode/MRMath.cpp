//
//  MRMath.cpp
//  Assignment1
//
//  Created by Michael Rotondo on 10/10/11.
//  Copyright 2011 Rototyping. All rights reserved.
//

#include <iostream>

#include "MRMath.h"

// The following two functions implement De Casteljau's algorithm for bezier curves, and are taken from
// http://www.cubic.org/docs/bezier.htm

void lerp(Vector3<float> &dest, const Vector3<float> &a, const Vector3<float> &b, const float t)
{
    dest.x = a.x + (b.x-a.x)*t;
    dest.y = a.y + (b.y-a.y)*t;
}

void bezier(Vector3<float> &dest, const Vector3<float> &a, const Vector3<float> &b, const Vector3<float> &c, const Vector3<float> &d, const float t)
{
    Vector3<float> ab,bc,cd,abbc,bccd;
    lerp(ab, a,b,t);           // point between a and b
    lerp(bc, b,c,t);           // point between b and c
    lerp(cd, c,d,t);           // point between c and d
    lerp(abbc, ab,bc,t);       // point between ab and bc
    lerp(bccd, bc,cd,t);       // point between bc and cd
    lerp(dest, abbc,bccd,t);   // point on the bezier-curve
}
