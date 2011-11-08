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
@class SASoundPath;
@class SABallGuide;

@interface SAWorld : SAPhysicsObject <SAAudioProducer, SADrawableObject>

@property (nonatomic, strong) SABall *ball;
@property (nonatomic, strong) SAAvatar *avatar;
@property (nonatomic, assign) b2World *physicsWorld;
@property BOOL editMode;

- (void)registerBody:(b2Body *)body withObject:(SAPhysicsObject *)object;
- (void)update:(NSTimer *)timer;

- (void)addSoundPath:(SASoundPath *)inSoundPath;

- (void)toggleEditMode;

- (void)startBallGuide;
- (void)addBallGuidePoint:(CGPoint)point;
- (void)finishBallGuide;


@end