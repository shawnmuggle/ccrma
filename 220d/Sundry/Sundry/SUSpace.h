//
//  SUSpace.h
//  Sundry
//
//  Created by Michael Rotondo on 12/8/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SUPlayer.h"

@interface SUSpace : NSObject

@property (nonatomic, strong) NSMutableSet *worlds;

- (void)drawWithBaseModelViewMatrix:(GLKMatrix4)baseModelViewMatrix 
                   projectionMatrix:(GLKMatrix4)projectionMatrix
                        timeElapsed:(NSTimeInterval)timeElapsed
                          forPlayer:(SUPlayer *)player;
- (void)renderAudioIntoBuffer:(SUAudioBuffer)buffer forPlayer:(SUPlayer *)player;

@end
