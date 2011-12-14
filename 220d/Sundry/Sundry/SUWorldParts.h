//
//  SUWorldParts.h
//  Sundry
//
//  Created by Michael Rotondo on 12/14/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <GLKit/GLKit.h>

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
+ (void)prepareToDrawSpringThings;
+ (void)finishDrawingSpringThings;
- (void)drawWithRadius:(float)radius 
           timeElapsed:(NSTimeInterval)timeElapsed 
   baseModelViewMatrix:(GLKMatrix4)baseModelViewMatrix 
      projectionMatrix:(GLKMatrix4)projectionMatrix;
- (void)renderAudioIntoBuffer:(SUAudioBuffer)buffer gain:(float)inGain;
- (void)springWithAmplitude:(float)amplitude;
@end