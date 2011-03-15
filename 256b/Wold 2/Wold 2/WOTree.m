//
//  WOTree.m
//  Wold 2
//
//  Created by Michael Rotondo on 3/14/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "WOTree.h"

@implementation WOTree
@synthesize angle, lSystem;

- (id) initWithAngle:(float)newAngle andOrigin:(CGPoint)origin
{
    self = [super init];
    if (self) {
        self.angle = newAngle;
        self.lSystem = [[WOLSystem alloc] initWithMaxGeneration:5 andAngle:newAngle andOrigin:origin];
        WOANode* node = [[WOANode alloc] init];
        [self.lSystem.nodes addObject:node];
    }
    return self;
}

- (void) tick
{
    [self.lSystem tick];
}

- (void) renderAtPoint:(CGPoint)pos withAngle:(float)renderAngle;
{
    [self.lSystem renderAtPoint:pos withAngle:renderAngle];
}

- (CALayer*) layer
{
    return self.lSystem.layer;
}

- (void) dealloc
{
    [lSystem release];
    
    [super dealloc];
}

@end
