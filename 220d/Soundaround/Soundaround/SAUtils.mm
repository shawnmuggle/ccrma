//
//  SAUtils.mm
//  Soundaround
//
//  Created by Michael Rotondo on 11/8/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import "SAUtils.h"

float lerp(float a, float b, float t)
{
    return a + t * (b - a);
}

float CGPointMagnitudeSquared(const CGPoint &p)
{
    return powf(p.x, 2) + powf(p.y, 2);
}

float CGPointMagnitude(const CGPoint &p)
{
    return sqrtf(CGPointMagnitudeSquared(p));
}

CGPoint CGPointSubtract(const CGPoint &p1, const CGPoint &p2)
{
    return CGPointMake(p1.x - p2.x, p1.y - p2.y);
}

CGPoint CGPointAdd(const CGPoint &p1, const CGPoint &p2)
{
    return CGPointMake(p1.x + p2.x, p1.y + p2.y);
}

CGPoint CGPointDivide(const CGPoint &p, float scalar)
{
    return CGPointMake(p.x / scalar, p.y / scalar);
}

CGPoint CGPointLerp(const CGPoint &p1, const CGPoint &p2, const float t)
{
    return CGPointMake(lerp(p1.x, p2.x, t), lerp(p1.y, p2.y, t));
}

CGPoint CGPointBezier(const CGPoint &a, const CGPoint &b, const CGPoint &c, const CGPoint &d, const float t)
{
    CGPoint ab,bc,cd,abbc,bccd;
    ab = CGPointLerp(a, b, t);           // point between a and b
    bc = CGPointLerp(b, c, t);           // point between b and c
    cd = CGPointLerp(c, d, t);           // point between c and d
    abbc = CGPointLerp(ab, bc, t);       // point between ab and bc
    bccd = CGPointLerp(bc, cd, t);       // point between bc and cd
    return CGPointLerp(abbc,bccd,t);     // point on the bezier-curve
}

CGSize CGSizeLerp(const CGSize &a, const CGSize &b, float t)
{
    CGSize s = CGSizeMake(lerp(a.width, b.width, t), lerp(a.height, b.height, t));
    return s;
}



@implementation SAUtils

@end
