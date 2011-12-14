//
//  SUWorld.h
//  Sundry
//
//  Created by Michael Rotondo on 12/8/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <GLKit/GLKit.h>

@class SUPlayer;
@class SUWorldCube;
@class SUWorldSpringThing;

@interface SUWorld : NSObject

- (id)initWithPosition:(GLKVector3)inPosition;
- (void)autoPopulate;
- (void)addCube:(SUWorldCube *)cube;
- (void)addSpringThing:(SUWorldSpringThing *)springThing;
- (void)makeTriangles;

- (void)drawWithBaseModelViewMatrix:(GLKMatrix4)baseModelViewMatrix
                   projectionMatrix:(GLKMatrix4)projectionMatrix
                        timeElapsed:(NSTimeInterval)timeElapsed
                          forPlayer:(SUPlayer *)player;
- (void)drawTransparentCrapWithBaseModelViewMatrix:(GLKMatrix4)baseModelViewMatrix
                                  projectionMatrix:(GLKMatrix4)projectionMatrix
                                       timeElapsed:(NSTimeInterval)timeElapsed
                                         forPlayer:(SUPlayer *)player;
- (void)renderAudioIntoBuffer:(SUAudioBuffer)buffer forPlayer:(SUPlayer *)player;
@end

@interface SUWorldSeed : NSObject

@property (nonatomic) GLKVector3 position;

- (void)drawWithBaseModelViewMatrix:(GLKMatrix4)baseModelViewMatrix
                   projectionMatrix:(GLKMatrix4)projectionMatrix
                        timeElapsed:(NSTimeInterval)timeElapsed
                          forPlayer:(SUPlayer *)player;
- (BOOL)checkForCollisionWithPlayer:(SUPlayer *)player;
@end