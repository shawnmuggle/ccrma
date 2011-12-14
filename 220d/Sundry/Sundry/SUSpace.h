//
//  SUSpace.h
//  Sundry
//
//  Created by Michael Rotondo on 12/8/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SUPlayer.h"

@class SUWorldSeed;
@class SUWorld;
@class SULandscape;

@interface SUSpace : NSObject

@property (nonatomic, strong) NSMutableSet *worlds;
@property (nonatomic, strong) NSMutableSet *worldSeeds;
@property (nonatomic, strong) SULandscape *landscape;

- (void)drawWithBaseModelViewMatrix:(GLKMatrix4)baseModelViewMatrix 
                   projectionMatrix:(GLKMatrix4)projectionMatrix
                        timeElapsed:(NSTimeInterval)timeElapsed
                          forPlayer:(SUPlayer *)player;
- (void)renderAudioIntoBuffer:(SUAudioBuffer)buffer forPlayer:(SUPlayer *)player;
- (SUWorldSeed *)checkWorldSeedsForCollisionWithPlayer:(SUPlayer *)player;
- (void)addWorld:(SUWorld *)world;
- (void)removeWorldSeed:(SUWorldSeed *)worldSeed;
@end
