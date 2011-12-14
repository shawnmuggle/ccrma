//
//  SUSpace.m
//  Sundry
//
//  Created by Michael Rotondo on 12/8/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import "SUSpace.h"
#import "SUWorld.h"

@implementation SUSpace
@synthesize worlds;

- (id)init {
    self = [super init];
    if (self) {
        int numWorlds = 20;
        worlds = [NSMutableSet setWithCapacity:numWorlds];
        for (int i = 0; i < numWorlds; i++)
        {
            [worlds addObject:[[SUWorld alloc] init]];
        }

    }
    return self;
}

- (void)drawWithBaseModelViewMatrix:(GLKMatrix4)baseModelViewMatrix 
                   projectionMatrix:(GLKMatrix4)projectionMatrix
                        timeElapsed:(NSTimeInterval)timeElapsed
                          forPlayer:(SUPlayer *)player
{
    for (SUWorld *world in self.worlds)
    {
        [world drawWithBaseModelViewMatrix:baseModelViewMatrix
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

- (void)renderAudioIntoBuffer:(SUAudioBuffer)buffer forPlayer:(SUPlayer *)player
{
    for (SUWorld *world in self.worlds)
    {
        [world renderAudioIntoBuffer:buffer forPlayer:player];
    }
}

@end
