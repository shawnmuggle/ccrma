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

- (id) initWithAngle:(float)newAngle andOrigin:(CGPoint)origin andType:(int)type
{
    self = [super init];
    if (self) {
        self.angle = newAngle;
        
        switch (type) {

                float range, branchAngle;
                int numBranches;
                WONode* n;
                
            case 1:
                // Tree 1 (normal looking, with leaves)
                
                self.lSystem = [[WOLSystem alloc] initWithMaxGeneration:5 andAngle:newAngle andOrigin:origin];
                self.lSystem.ticksPerGeneration = 30;
                self.lSystem.layer.strokeColor = [UIColor colorWithRed:45.0 / 255 green:115.0 / 255 blue:57.0 / 255 alpha:1.0].CGColor;
                self.lSystem.layer.fillColor = nil;
                self.lSystem.layer.lineWidth = 2.0;
                n = [[[WOLittleFNode alloc] initWithBaseLength:40 andMaxOffset:30] autorelease];
                [self.lSystem.nodes addObject:n];
                n = [[[WOANode alloc] init] autorelease];
                [self.lSystem.nodes addObject:n];
                
                break;

            case 2:
                // Tree 2 (curly bush)
                range = 60;
                numBranches = 10;
                self.lSystem = [[WOLSystem alloc] initWithMaxGeneration:10 andAngle:newAngle andOrigin:origin];
                self.lSystem.ticksPerGeneration = 10;
                self.lSystem.layer.strokeColor = [UIColor colorWithRed:0.5 green:0.2 blue:0.1 alpha:1.0].CGColor;
                self.lSystem.layer.fillColor = nil;
                self.lSystem.layer.lineWidth = 1.0;
                for (int i = 0; i < numBranches; i++) {
                    branchAngle = range * ((2 * i / (float)(numBranches - 1)) - 1.0);
                    
                    n = [[[WOLeftBracketNode alloc] init] autorelease];
                    [self.lSystem.nodes addObject:n];
                    // TODO: Make angle's random offset vary around 0 instead of strictly positive
                    n = [[[WOAngleNode alloc] initWithBaseAngle:branchAngle andMaxOffset:10] autorelease];
                    [self.lSystem.nodes addObject:n];
                    n = [[[WOWiggleNode alloc] init] autorelease];
                    [self.lSystem.nodes addObject:n];
                    n = [[[WORightBracketNode alloc] init] autorelease];
                    [self.lSystem.nodes addObject:n];
                }
                
                break;
            default:
                break;
        }
        
    }
    return self;
}

- (void) tick
{
    [self.lSystem tick];
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
