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
@class SABallGuide;

@interface SABall : SAPhysicsObject <SAAudioProducer, SADrawableObject>

@property (nonatomic) BOOL held;
@property (nonatomic, weak) SABallGuide *currentGuide;

- (id)initWithWorld:(SAWorld *)aWorld;
- (void)throwTowards:(CGPoint)point;

@end
