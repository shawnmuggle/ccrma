//
//  SAUtils.h
//  Soundaround
//
//  Created by Michael Rotondo on 11/8/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface SAUtils : NSObject

@end

float lerp(float a, float b, float t);

float CGPointMagnitudeSquared(const CGPoint &p);
float CGPointMagnitude(const CGPoint &p);
CGPoint CGPointSubtract(const CGPoint &p1, const CGPoint &p2);
CGPoint CGPointAdd(const CGPoint &p1, const CGPoint &p2);
CGPoint CGPointDivide(const CGPoint &p, const float scalar);
CGPoint CGPointLerp(const CGPoint &p1, const CGPoint &p2, const float t);
CGPoint CGPointBezier(const CGPoint &a, const CGPoint &b, const CGPoint &c, const CGPoint &d, const float t);

CGSize CGSizeLerp(const CGSize &a, const CGSize &b, float t);
