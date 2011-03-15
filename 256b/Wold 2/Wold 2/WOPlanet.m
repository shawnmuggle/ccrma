//
//  WOPlanet.m
//  Wold 2
//
//  Created by Michael Rotondo on 3/13/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "WOPlanet.h"
#import "WOTree.h"

@implementation WOPlanet
@synthesize trees, layer;

- (id) init
{
    self = [super init];
    if (self) {
        self.trees = [NSMutableSet setWithCapacity:10];

        baseRadius = 900;
        maxHeight = 300;
        
        self.layer = [CAShapeLayer layer];
        path = CGPathCreateMutable();
        [self generatePath];
        self.layer.path = path;
        self.layer.strokeColor = [UIColor colorWithRed:0.2 green:0.8 blue:0.3 alpha:1.0].CGColor;
        self.layer.fillColor = [UIColor whiteColor].CGColor;
        self.layer.lineWidth = 2.0;
    }
    return self;
}

// TODO: Figure out if we should generate multiple paths for different zoom levels, or just scale one path
- (void) generatePath
{
    int numPlanetSegments = 500;
    
    float planetSurface = [self surfaceFunction:0];
    float x = (baseRadius + maxHeight * planetSurface) * cos(0);
    float y = (baseRadius + maxHeight * planetSurface) * sin(0);
    CGPathMoveToPoint(path, NULL, x, y);
    
    float angle;    
    for (int i = 1; i <= numPlanetSegments; i++) {
        angle = (i / (float)numPlanetSegments) * 2 * M_PI;
        planetSurface = [self surfaceFunction:-angle];
        x = (baseRadius + maxHeight * planetSurface) * cos(-angle);
        y = (baseRadius + maxHeight * planetSurface) * sin(-angle);
        CGPathAddLineToPoint(path, NULL, x, y);
    }
}

- (void) addTreeAtAngle:(float)angle
{
    float planetSurface = [self surfaceFunction:angle];
    float treeX = (baseRadius + maxHeight * planetSurface) * cos(-angle);
    float treeY = (baseRadius + maxHeight * planetSurface) * sin(-angle);

    WOTree* tree = [[[WOTree alloc] initWithAngle:-angle andOrigin:CGPointMake(treeX, treeY)] autorelease];
    [self.trees addObject:tree];
    [self.layer addSublayer:[tree layer]];
}

- (float) surfaceFunction:(float)x
{
    return cos(8 * x) * 0.03 + cos(15 * x) * 0.05 + sin(x * 17) * 0.1;
}

- (void) tick
{
    for (WOTree* tree in self.trees) {
        [tree tick];
    }
}

@end
