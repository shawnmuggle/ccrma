//
//  SAAvatar.h
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
@class SABall;

@interface SAAvatar : SAPhysicsObject <SAAudioProducer, SADrawableObject>

@property (nonatomic, weak) SABall *ball;

- (id)initWithWorld:(SAWorld *)inWorld andBall:(SABall *)inBall;

- (void)startMoving:(CGPoint)direction;
- (void)stopMoving:(CGPoint)direction;

- (void)grabBall;
- (void)throwBallTowardsPoint:(CGPoint)point;

@end
