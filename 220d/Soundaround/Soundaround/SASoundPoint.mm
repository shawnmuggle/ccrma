//
//  SASoundPoint.m
//  Soundaround
//
//  Created by Michael Rotondo on 10/31/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import "SASoundPoint.h"

@implementation SASoundPoint
@synthesize position;
@synthesize intensity;
@synthesize angle;

- (id)initWithPosition:(CGPoint)inPosition andIntensity:(float)inIntensity andAngle:(float)inAngle
{
    self = [super init];
    if (self)
    {
        position = inPosition;
        intensity = inIntensity;
        angle = inAngle;
    }
    return self;
}

@end
