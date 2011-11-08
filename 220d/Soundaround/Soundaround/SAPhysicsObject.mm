//
//  SAPhysicsObject.mm
//  Soundaround
//
//  Created by Michael Rotondo on 10/23/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#include <Box2D/Box2D.h>

#import "SAPhysicsObject.h"

@implementation SAPhysicsObject
@synthesize physicsBody;

- (CGPoint)position
{
    const b2Vec2 &position = self.physicsBody->GetPosition();
    return CGPointMake(position.x, position.y);
}

- (float)speed
{
    const b2Vec2 &velocity = self.physicsBody->GetLinearVelocity();
    return velocity.Length();
}

- (BOOL)shouldDisableContact:(b2Contact *)contact
{
//    NSLog(@"I, %@, am being asked if I want to disable %p!", self, contact);
    return NO;
}

- (void)processContact:(b2Contact *)contact withOtherObject:(id)otherObject
{
//    NSLog(@"I, %@, collided with %@!", self, otherObject);
}

- (void)update
{
//    NSLog(@"I, %@, have been told to update myself!", self);
}

@end
