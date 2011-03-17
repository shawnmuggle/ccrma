//
//  WOSolarSystem.m
//  Wold 2
//
//  Created by Michael Rotondo on 3/15/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "WOSolarSystem.h"


@implementation WOSolarSystem
@synthesize star, planets, sky, zoom;

- (id) init
{
    self = [super init];
    if (self) {
        self.sky = [CALayer layer];
        self.zoom = [CALayer layer];
        [self.sky addSublayer:self.zoom];
        
        CGPoint starLoc = CGPointMake(0, 0);
        self.star = [[[WOStar alloc] initAtPoint:starLoc] autorelease];
        [self.zoom addSublayer:self.star.translation];
        
        
        int numPlanets = 6;
        
        self.planets = [NSMutableArray arrayWithCapacity:numPlanets];
        float dist, angle;
        
        // TODO: randomize distances, angles, hill functions
        // TODO: draw orbit lines
        
        for (int i = 0; i < numPlanets; i++) {
            dist = 4000 + i * 2200.0; // HACK: magic numbers
            angle = -M_PI * 0.2 + i * M_PI * 0.05; // HACK: magic numbers
            CGPoint planetLoc = CGPointMake(dist * cos(angle), dist * sin(angle));
            WOPlanet* planet = [[[WOPlanet alloc] initAtPoint:planetLoc] autorelease];
            [self.zoom addSublayer:planet.translation];
            [self.planets addObject:planet];
        }
    }
    return self;
}

@end
