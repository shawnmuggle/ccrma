//
//  SUWorld.m
//  Sundry
//
//  Created by Michael Rotondo on 12/8/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import "SUWorld.h"
#import "ROGLProgram.h"
#import "ROGLShapes.h"
#import "SUPlayer.h"
#import "SUSynth.h"

@interface SUWorldCube ()
{
    ROGLGouraudProgram *program;
    
    GLKVector3 position;
    
    float angle;
    float angleIncrement;
    GLKVector3 axis;
    
    GLKVector3 color;
    
    float scale;
    
    SUSynth *synth;    
}
@end

@implementation SUWorldCube

- (id)initAtPosition:(GLKVector3)inPosition
{
    self = [super init];
    if (self) {
        program = [ROGLGouraudProgram sharedInstance];
        
        position = GLKVector3Add(inPosition, GLKVector3Make(10 * (arc4random() / (float)0x100000000) - 5, 
                                                            10 * (arc4random() / (float)0x100000000) - 5, 
                                                            10 * (arc4random() / (float)0x100000000) - 5));
        
        angle = 0.0f;
        angleIncrement = 3.0f * (arc4random() / (float)0x100000000);  // radians per second
        
        axis = GLKVector3Make((arc4random() / (float)0x100000000), 
                              (arc4random() / (float)0x100000000), 
                              (arc4random() / (float)0x100000000));
        scale = 10.0f + 20.0f * (arc4random() / (float)0x100000000);
        
        color = GLKVector3Make((arc4random() / (float)0x100000000),
                               (arc4random() / (float)0x100000000),
                               (arc4random() / (float)0x100000000));
        
        synth = [[SUSynth alloc] init];
        synth.centerFreq = 100.0f + 800.0f * (1.0f - (scale / 30.0f)); // Smaller world, higher pitch
        synth.lfoDepth = synth.centerFreq * 0.3;
        synth.lfoFreq = angleIncrement / (2 * M_PI);
        synth.waveAmounts = color;
    }
    return self;
}

- (void)drawWithBaseModelViewMatrix:(GLKMatrix4)baseModelViewMatrix 
                   projectionMatrix:(GLKMatrix4)projectionMatrix
                        timeElapsed:(NSTimeInterval)timeElapsed
{    
    [program use];
    
    GLKMatrix4 modelViewMatrix = GLKMatrix4MakeTranslation(-position.x, -position.y, -position.z);
    modelViewMatrix = GLKMatrix4Scale(modelViewMatrix, scale, scale, scale);
    modelViewMatrix = GLKMatrix4Rotate(modelViewMatrix, angle, axis.x, axis.y, axis.z);
    modelViewMatrix = GLKMatrix4Multiply(baseModelViewMatrix, modelViewMatrix);
    
    float actualAngleIncrement = angleIncrement * timeElapsed;    
    angle = fmodf(angle + actualAngleIncrement, 2 * M_PI);
    
    GLKMatrix4 modelViewProjectionMatrix = GLKMatrix4Multiply(projectionMatrix, modelViewMatrix);
    GLKMatrix3 normalMatrix = GLKMatrix3InvertAndTranspose(GLKMatrix4GetMatrix3(modelViewMatrix), NULL);
    
#warning CHECK HERE to see if the cube is within the projection frustrum, and if not don't draw it!
    
    [program setModelViewProjectionMatrix:modelViewProjectionMatrix normalMatrix:normalMatrix];
    [program setColor:color];
    
    [[ROGLShapes sharedInstance] drawCube];
}

- (void)renderAudioIntoBuffer:(SUAudioBuffer)buffer forPlayer:(SUPlayer *)player
{
    float maxDistance = scale * 5.0f;
    float distanceToPlayer = GLKVector3Length(GLKVector3Subtract(player.position, position));
    
    if (distanceToPlayer > maxDistance)
    {
        // Too far away, no sound!
        return;
    }
    
    float masterGain = 0.1f;
    float gain = -powf((distanceToPlayer / maxDistance) - 1, 3) * masterGain;
    [synth renderAudioIntoBuffer:buffer gain:gain];
}

@end

@interface SUWorld ()
{
    NSMutableSet *cubes;
}
@end

@implementation SUWorld

- (id)init {
    self = [super init];
    if (self) {
        GLKVector3 position = GLKVector3Make(1500 * (arc4random() / (float)0x100000000) - 750, 
                                             1500 * (arc4random() / (float)0x100000000) - 750, 
                                             1500 * (arc4random() / (float)0x100000000) - 750);
        
        cubes = [NSMutableSet set];
        int numCubes = 5;
        for (int i = 0; i < numCubes; i++)
        {
            [cubes addObject:[[SUWorldCube alloc] initAtPosition:position]];
        }
    }
    return self;
}

- (void)drawWithBaseModelViewMatrix:(GLKMatrix4)baseModelViewMatrix 
                   projectionMatrix:(GLKMatrix4)projectionMatrix
                        timeElapsed:(NSTimeInterval)timeElapsed
{
    for (SUWorldCube *cube in cubes)
    {
        [cube drawWithBaseModelViewMatrix:baseModelViewMatrix projectionMatrix:projectionMatrix timeElapsed:timeElapsed];
    }
}

- (void)renderAudioIntoBuffer:(SUAudioBuffer)buffer forPlayer:(SUPlayer *)player
{
    for (SUWorldCube *cube in cubes)
    {
        [cube renderAudioIntoBuffer:buffer forPlayer:player];
    }
}

@end
