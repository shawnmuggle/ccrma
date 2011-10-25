//
//  SAWorld.h
//  Soundaround
//
//  Created by Michael Rotondo on 10/20/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SAPhysicsObject.h"
#import "SAAudioProducer.h"
#import "SADrawableObject.h"

class b2World;
class b2Body;
@class SABall;
@class SAAvatar;

@interface SAWorld : SAPhysicsObject <SAAudioProducer, SADrawableObject>

@property (nonatomic, retain) SABall *ball;
@property (nonatomic, retain) SAAvatar *avatar;
@property (nonatomic, assign) b2World *physicsWorld;

+ (id)world;
- (void)registerBody:(b2Body *)body withObject:(SAPhysicsObject *)object;
- (void)update:(NSTimer *)timer;

// Control
- (void)setTargetPoint:(CGPoint)targetPoint;

@end