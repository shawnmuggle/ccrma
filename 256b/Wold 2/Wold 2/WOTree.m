//
//  WOTree.m
//  Wold 2
//
//  Created by Michael Rotondo on 3/14/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "WOTree.h"

@implementation WOTree
@synthesize angle;

- (id) initWithAngle:(float)newAngle
{
    self = [super init];
    if (self) {
        self.angle = newAngle;
        lSystem = [[WOLSystem alloc] initWithMaxGeneration:5];
        WOANode* node = [[WOANode alloc] init];
        [lSystem.nodes addObject:node];
    }
    return self;
}

- (void) tick
{
    [lSystem tick];
}

- (void) renderAtPoint:(CGPoint)pos withAngle:(float)renderAngle;
{
    [lSystem renderAtPoint:pos withAngle:renderAngle];
}

- (void) dealloc
{
    [lSystem release];
    
    [super dealloc];
}

@end
