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

@interface SUWorld : NSObject
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

@interface SUWorldCube : NSObject
@property (nonatomic) float scale;
- (id)initAtPosition:(GLKVector3)inPosition;
+ (void)prepareToDrawCubes;
- (void)drawWithBaseModelViewMatrix:(GLKMatrix4)baseModelViewMatrix
                   projectionMatrix:(GLKMatrix4)projectionMatrix
                        timeElapsed:(NSTimeInterval)timeElapsed;
- (void)renderAudioIntoBuffer:(SUAudioBuffer)buffer gain:(float)inGain;
@end

@interface SUWorldOrbitingTriangle : NSObject
+ (void)prepareToDrawTrianglesWithModelViewMatrix:(GLKMatrix4)modelViewMatrix projectionMatrix:(GLKMatrix4)projectionMatrix;
+ (void)finishDrawingTriangles;
- (void)drawWithRadius:(float)radius timeElapsed:(NSTimeInterval)timeElapsed;
@end

@interface SUWorldSpringThing : NSObject
+ (void)prepareToDrawSpringThingsWithModelViewMatrix:(GLKMatrix4)modelViewMatrix projectionMatrix:(GLKMatrix4)projectionMatrix;
+ (void)finishDrawingSpringThings;
- (void)drawWithRadius:(float)radius timeElapsed:(NSTimeInterval)timeElapsed;
@end