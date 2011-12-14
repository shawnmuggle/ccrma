//
//  SUSpace.m
//  Sundry
//
//  Created by Michael Rotondo on 12/8/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import "SUSpace.h"
#import "SUWorld.h"
#import "SULandscape.h"

@implementation SUSpace
@synthesize worlds;
@synthesize worldSeeds;
@synthesize landscape;

- (id)init {
    self = [super init];
    if (self) {
        int numWorlds = 20;
        self.worlds = [NSMutableSet setWithCapacity:numWorlds];
        for (int i = 0; i < numWorlds; i++)
        {
            SUWorld *world = [[SUWorld alloc] init];
            [world autoPopulate];
            [self.worlds addObject:world];
        }
        
        int numWorldSeeds = 10;
        self.worldSeeds = [NSMutableSet setWithCapacity:numWorldSeeds];
        for (int i = 0; i < numWorldSeeds; i++)
        {
            [self.worldSeeds addObject:[[SUWorldSeed alloc] init]];
        }
        
//        self.landscape = [[SULandscape alloc] init];

    }
    return self;
}

- (void)drawWithBaseModelViewMatrix:(GLKMatrix4)baseModelViewMatrix 
                   projectionMatrix:(GLKMatrix4)projectionMatrix
                        timeElapsed:(NSTimeInterval)timeElapsed
                          forPlayer:(SUPlayer *)player
{
//    [self.landscape drawWithBaseModelViewMatrix:baseModelViewMatrix projectionMatrix:projectionMatrix];
    
    for (SUWorld *world in self.worlds)
    {
        [world drawWithBaseModelViewMatrix:baseModelViewMatrix
                          projectionMatrix:projectionMatrix
                               timeElapsed:timeElapsed
                                 forPlayer:player];
    }
    
    for (SUWorldSeed *worldSeed in self.worldSeeds)
    {
        [worldSeed drawWithBaseModelViewMatrix:baseModelViewMatrix
                              projectionMatrix:projectionMatrix
                                   timeElapsed:timeElapsed
                                     forPlayer:player];
    }
    
    for (SUWorld *world in self.worlds)
    {    
        [world drawTransparentCrapWithBaseModelViewMatrix:baseModelViewMatrix
                                         projectionMatrix:projectionMatrix
                                              timeElapsed:timeElapsed
                                                forPlayer:player];

    }
}

- (SUWorldSeed *)checkWorldSeedsForCollisionWithPlayer:(SUPlayer *)player
{
    for (SUWorldSeed *worldSeed in self.worldSeeds)
    {
        if ([worldSeed checkForCollisionWithPlayer:player])
        {
            return worldSeed;
        }
    }
    return nil;
}

- (void)renderAudioIntoBuffer:(SUAudioBuffer)buffer forPlayer:(SUPlayer *)player
{
    for (SUWorld *world in self.worlds)
    {
        [world renderAudioIntoBuffer:buffer forPlayer:player];
    }
    
    [player renderAudioIntoBuffer:buffer gain:1.0f];
}

- (void)addWorld:(SUWorld *)world
{
    [worlds addObject:world];
}

- (void)removeWorldSeed:(SUWorldSeed *)worldSeed
{
    [worldSeeds removeObject:worldSeed];
}

@end
