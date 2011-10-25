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

- (void)processContact:(b2Contact *)contact withOtherObject:(id)otherObject
{
    NSLog(@"I, %@, collided with %@!", self, otherObject);
}

@end
