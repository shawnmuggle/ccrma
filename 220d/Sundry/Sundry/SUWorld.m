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
#import "ROGLProgram.h"
#import "SUPlayer.h"

@interface SUWorld ()
{
    GLKVector3 position;
    float range;
    NSMutableSet *cubes;
    NSMutableSet *orbitingTriangles;
    NSMutableSet *springThings;
}

- (void)setup;

@end

@implementation SUWorld

- (id)init 
{
    self = [super init];
    if (self) 
    {        
        position = GLKVector3Make(2000 * (arc4random() / (float)0x100000000) - 1000, 
                                  2000 * (arc4random() / (float)0x100000000) - 1000, 
                                  2000 * (arc4random() / (float)0x100000000) - 1000);
        [self setup];
    }
    return self;
}

- (id)initWithPosition:(GLKVector3)inPosition
{
    self = [super init];
    if (self) 
    {
        position = inPosition;
        [self setup];
    }
    return self;
}

- (void)setup
{
    cubes = [NSMutableSet set];
    orbitingTriangles = [NSMutableSet set];
    springThings = [NSMutableSet set];
}

- (void)autoPopulate
{
    int numCubes = 7;
    for (int i = 0; i < numCubes; i++)
    {
        [self addCube:[[SUWorldCube alloc] init]];
        
    }    

    [self makeTriangles];

    int numSpringThings = 10;
    for (int i = 0; i < numSpringThings; i++)
    {
        [self addSpringThing:[[SUWorldSpringThing alloc] init]];
    }
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
    
    range = 10.0f * 50.0f;  // I secretly know that this is the maximum possible cube scale! [SECRETZ]
    
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

- (void)addCube:(SUWorldCube *)cube
{
    [cubes addObject:cube];
//    float maxCubeScale = 0.0f;
//    for(SUWorldCube *cube in cubes)
//    {
//        if (cube.scale > maxCubeScale)
//            maxCubeScale = cube.scale;
//    }
//    range = 12.0f * maxCubeScale;
}

- (void)addSpringThing:(SUWorldSpringThing *)springThing
{
    [springThings addObject:springThing];
}

- (void)makeTriangles
{
    int numTriangles = 50;
    for (int i = 0; i < numTriangles; i++)
    {
        [orbitingTriangles addObject:[[SUWorldOrbitingTriangle alloc] init]];
    }
}

@end

@interface SUWorldSeed ()
{
    float scale;
}
@end

@implementation SUWorldSeed
@synthesize position;

- (id)init {
    self = [super init];
    if (self) {
        self.position = GLKVector3Make(2000 * (arc4random() / (float)0x100000000) - 1000, 
                                       2000 * (arc4random() / (float)0x100000000) - 1000, 
                                       2000 * (arc4random() / (float)0x100000000) - 1000);

        scale = 30.0f;
    }
    return self;
}

- (void)drawWithBaseModelViewMatrix:(GLKMatrix4)baseModelViewMatrix
                   projectionMatrix:(GLKMatrix4)projectionMatrix
                        timeElapsed:(NSTimeInterval)timeElapsed
                          forPlayer:(SUPlayer *)player
{
    baseModelViewMatrix = GLKMatrix4Translate(baseModelViewMatrix, -self.position.x, -self.position.y, -self.position.z);
    GLKMatrix4 modelViewMatrix = GLKMatrix4MakeScale(scale, scale, scale);
    modelViewMatrix = GLKMatrix4Multiply(baseModelViewMatrix, modelViewMatrix);
        
    GLKMatrix4 modelViewProjectionMatrix = GLKMatrix4Multiply(projectionMatrix, modelViewMatrix);
    GLKMatrix3 normalMatrix = GLKMatrix3InvertAndTranspose(GLKMatrix4GetMatrix3(modelViewMatrix), NULL);
    
#warning CHECK HERE to see if the cube is within the projection frustrum, and if not don't draw it!
    
    GLKVector4 color = GLKVector4Make(1.0f, 1.0f, 1.0f, 0.6f);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    
    [[ROGLShapes sharedInstance] prepareToDrawCubes];
    [SUWorldCube prepareToDrawCubes];

    [[ROGLGouraudProgram sharedInstance] setModelViewProjectionMatrix:modelViewProjectionMatrix normalMatrix:normalMatrix];
    [[ROGLGouraudProgram sharedInstance] setColor:color];
    
    [[ROGLShapes sharedInstance] drawCube];
    
    glDisable(GL_BLEND);
}

- (BOOL)checkForCollisionWithPlayer:(SUPlayer *)player
{
    float dist = GLKVector3Length(GLKVector3Subtract(player.position, self.position));
    return dist < scale;
}

@end