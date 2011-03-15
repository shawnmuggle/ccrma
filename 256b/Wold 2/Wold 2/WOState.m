//
//  WOState.m
//  Wold 2
//
//  Created by Michael Rotondo on 3/13/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "WOState.h"

@implementation WOState
@synthesize planets, currentPlanet, zoomLevel, rotationAngle, zoomedPlanetYOffset;

- (id) init
{
    self = [super init];
    if (self) {
        self.planets = [NSMutableSet setWithCapacity:1];
        [self.planets addObject:[[[WOPlanet alloc] init] autorelease]];
        
        self.currentPlanet = [self.planets anyObject];
        self.zoomLevel = 3;
        
        self.rotationAngle = 0;
        rotationAngleIncrement = 0;
        rotationDamping = 0.92;
        
        self.zoomedPlanetYOffset = 1000;
        
        srand((unsigned)time(0));
    }
    return self;
}

- (void) tick:(id)sender
{
    self.rotationAngle += rotationAngleIncrement;
    rotationAngleIncrement *= rotationDamping;
    
    for (WOPlanet* planet in self.planets) {
        [planet tick];
    }
}

- (void) handlePan:(id)sender
{
    UIPanGestureRecognizer* pan = (UIPanGestureRecognizer*)sender;
    rotationAngleIncrement += [pan velocityInView:pan.view].x * 0.000005;
}

- (void) handleLongPress:(id)sender
{
    UILongPressGestureRecognizer* press = (UILongPressGestureRecognizer*)sender;
    if (press.state == UIGestureRecognizerStateBegan) {
        rotationAngleIncrement = 0;
        
        CGPoint loc = [press locationInView:press.view];
        
        float xDiff = loc.x - 512; // hacked magic number, we know the screen is sideways & 1024 px wide
        float yDiff = (768 - loc.y) + (self.zoomedPlanetYOffset - 384); // more hackass magic numbers

        [self.currentPlanet addTreeAtAngle:atan2f(yDiff, xDiff) + self.rotationAngle];
    }
}

- (void) handlePinch:(id)sender
{
    UIPinchGestureRecognizer* pinch = (UIPinchGestureRecognizer*)sender;
    if (pinch.state == UIGestureRecognizerStateBegan) {
        if (pinch.scale > 1) { 
            self.zoomLevel += 1;
        } else if (pinch.scale < 1) {
            self.zoomLevel -= 1;
        }
        if (self.zoomLevel < 1) self.zoomLevel = 1;
        if (self.zoomLevel > 3) self.zoomLevel = 3; // TODO: Eventually use 4 as the max here for individual-tree style zoom
    }
}

@end
