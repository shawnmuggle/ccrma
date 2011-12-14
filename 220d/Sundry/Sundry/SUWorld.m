//
//  SUWorld.m
//  Sundry
//
//  Created by Michael Rotondo on 12/8/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import "SUWorld.h"
#import "SUWorldParts.h"
#import "ROGLShapes.h"
#import "SUPlayer.h"

@interface SUWorld ()
{
    GLKVector3 position;
    float range;
    NSMutableSet *cubes;
    NSMutableSet *orbitingTriangles;
    NSMutableSet *springThings;
}
@end

@implementation SUWorld

- (id)init {
    self = [super init];
    if (self) {
        position = GLKVector3Make(2000 * (arc4random() / (float)0x100000000) - 1000, 
                                  2000 * (arc4random() / (float)0x100000000) - 1000, 
                                  2000 * (arc4random() / (float)0x100000000) - 1000);
        
        cubes = [NSMutableSet set];
        int numCubes = 7;
        for (int i = 0; i < numCubes; i++)
        {
            [cubes addObject:[[SUWorldCube alloc] initAtPosition:position]];
            
        }
        float maxCubeScale = 0.0f;
        for(SUWorldCube *cube in cubes)
        {
            if (cube.scale > maxCubeScale)
                maxCubeScale = cube.scale;
        }
        range = 12.0f * maxCubeScale;
        
        orbitingTriangles = [NSMutableSet set];
        int numTriangles = 50;
        for (int i = 0; i < numTriangles; i++)
        {
            [orbitingTriangles addObject:[[SUWorldOrbitingTriangle alloc] init]];
        }
        
        springThings = [NSMutableSet set];
        int numSpringThings = 10;
        for (int i = 0; i < numSpringThings; i++)
        {
            [springThings addObject:[[SUWorldSpringThing alloc] init]];
        }
    }
    return self;
}

- (void)drawWithBaseModelViewMatrix:(GLKMatrix4)baseModelViewMatrix 
                   projectionMatrix:(GLKMatrix4)projectionMatrix
                        timeElapsed:(NSTimeInterval)timeElapsed
                          forPlayer:(SUPlayer *)player
{
    GLKMatrix4 modelViewMatrix = GLKMatrix4Translate(baseModelViewMatrix, -position.x, -position.y, -position.z);

    [[ROGLShapes sharedInstance] prepareToDrawCubes];
    [SUWorldCube prepareToDrawCubes];
    for (SUWorldCube *cube in cubes)
    {
        [cube drawWithBaseModelViewMatrix:modelViewMatrix
                         projectionMatrix:projectionMatrix
                              timeElapsed:timeElapsed];
    }
    
    float maxDistance = range;
    float maxScale = 0.2 * range;
    float distanceToPlayer = GLKVector3Length(GLKVector3Subtract(player.position, position));
    
    if (distanceToPlayer > maxDistance)
    {
        // Too far away, no more cool graphics!!
        return;
    }
    
    float attenuationGain = -powf(distanceToPlayer / maxDistance, 8) + 1;
    
    [[ROGLShapes sharedInstance] prepareToDrawSpringThings];
    [SUWorldSpringThing prepareToDrawSpringThings];
    for (SUWorldSpringThing *springThing in springThings)
    {
        [springThing drawWithRadius:maxScale * attenuationGain timeElapsed:timeElapsed baseModelViewMatrix:modelViewMatrix projectionMatrix:projectionMatrix];
    }
    [SUWorldSpringThing finishDrawingSpringThings];
}

- (void)drawTransparentCrapWithBaseModelViewMatrix:(GLKMatrix4)baseModelViewMatrix 
                                  projectionMatrix:(GLKMatrix4)projectionMatrix
                                       timeElapsed:(NSTimeInterval)timeElapsed
                                         forPlayer:(SUPlayer *)player
{
    float maxDistance = range;
    float distanceToPlayer = GLKVector3Length(GLKVector3Subtract(player.position, position));
    
    if (distanceToPlayer > maxDistance)
    {
        // Too far away, no more cool graphics!!
        return;
    }

    float attenuationGain = -powf(distanceToPlayer / maxDistance, 8) + 1;
    GLKMatrix4 modelViewMatrix = GLKMatrix4Translate(baseModelViewMatrix, -position.x, -position.y, -position.z);
    float maxScale = 0.2 * range;
    [[ROGLShapes sharedInstance] prepareToDrawTriangles];
    [SUWorldOrbitingTriangle prepareToDrawTrianglesWithModelViewMatrix:modelViewMatrix projectionMatrix:projectionMatrix];
    for (SUWorldOrbitingTriangle *triangle in orbitingTriangles)
    {
        [triangle drawWithRadius:maxScale * attenuationGain timeElapsed:timeElapsed];
    }
    [SUWorldOrbitingTriangle finishDrawingTriangles];
}

- (void)renderAudioIntoBuffer:(SUAudioBuffer)buffer forPlayer:(SUPlayer *)player
{
    float maxDistance = range;
    float distanceToPlayer = GLKVector3Length(GLKVector3Subtract(player.position, position));
    float attenuationGain = -powf((distanceToPlayer / maxDistance) - 1, 3);
    
    if (distanceToPlayer > maxDistance)
    {
        // Too far away, no sound!
        return;
    }
    
#warning maybe collapse all the synthesis down into this one method
    for (SUWorldCube *cube in cubes)
    {
        [cube renderAudioIntoBuffer:buffer gain:attenuationGain];
    }
    
    for (SUWorldSpringThing *springThing in springThings)
    {
        [springThing renderAudioIntoBuffer:buffer gain:attenuationGain];
    }
}

@end


@implementation SUWorldSeed

- (void)drawWithBaseModelViewMatrix:(GLKMatrix4)baseModelViewMatrix
                   projectionMatrix:(GLKMatrix4)projectionMatrix
                        timeElapsed:(NSTimeInterval)timeElapsed
                          forPlayer:(SUPlayer *)player
{
    
}

@end