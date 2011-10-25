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

@interface SAAvatar : SAPhysicsObject <SAAudioProducer, SADrawableObject>

@property (nonatomic, readonly) CGPoint position;
@property (nonatomic, readonly) float speed;

+ (id)avatarInWorld:(SAWorld *)world;

- (CGPoint)position;

- (void)startMoving:(CGPoint)direction;
- (void)stopMoving:(CGPoint)direction;


@end
