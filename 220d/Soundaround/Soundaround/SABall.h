//
//  SABall.h
//  Soundaround
//
//  Created by Michael Rotondo on 10/21/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SAPhysicsObject.h"
#import "SAAudioProducer.h"
#import "SADrawableObject.h"

@class SAWorld;

@interface SABall : SAPhysicsObject <SAAudioProducer, SADrawableObject>

+ (id)ballInWorld:(SAWorld *)world;
- (void)throwTowards:(CGPoint)point;

@end
