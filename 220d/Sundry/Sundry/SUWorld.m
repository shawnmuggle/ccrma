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
#import "SUSynths.h"
#import "sin_lookup.h"

static float TWO_PI = 2 * M_PI;

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
        int numTriangles = 100;
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
    [SUWorldSpringThing prepareToDrawSpringThingsWithModelViewMatrix:modelViewMatrix projectionMatrix:projectionMatrix];
    for (SUWorldSpringThing *springThing in springThings)
    {
        [springThing drawWithRadius:maxScale * attenuationGain timeElapsed:timeElapsed];
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
}

@end


@interface SUWorldCube ()
{
    GLKVector3 relativePosition;
    
    float angle;
    float angleIncrement;
    GLKVector3 axis;
    
    GLKVector4 color;
    
    float scale;
    
    SUFMSynth *synth;    
}
@end

@implementation SUWorldCube
@synthesize scale;

- (id)initAtPosition:(GLKVector3)inPosition
{
    self = [super init];
    if (self) {
        relativePosition = GLKVector3Make(10 * (arc4random() / (float)0x100000000) - 5, 
                                          10 * (arc4random() / (float)0x100000000) - 5, 
                                          10 * (arc4random() / (float)0x100000000) - 5);
        
        angle = 0.0f;
        angleIncrement = 4.0f * (arc4random() / (float)0x100000000);  // radians per second
        
        axis = GLKVector3Make((arc4random() / (float)0x100000000), 
                              (arc4random() / (float)0x100000000), 
                              (arc4random() / (float)0x100000000));

        float maxScale = 50.0f;
        float minScale = 20.0f;
        self.scale = minScale + (maxScale - minScale) * (arc4random() / (float)0x100000000);
        
        color = GLKVector4Make((arc4random() / (float)0x100000000),
                               (arc4random() / (float)0x100000000),
                               (arc4random() / (float)0x100000000),
                               1.0f);
        
        synth = [[SUFMSynth alloc] init];
        synth.carrierFreq = 100.0f + 300.0f * (1.0f - (self.scale / maxScale)); // Smaller world, higher pitch
        synth.modDepth = synth.carrierFreq * 0.8 * (arc4random() / (float)0x100000000);
        synth.modFreq = 2000 * angleIncrement / TWO_PI;
        synth.lfoFreq = angleIncrement / TWO_PI;
    }
    return self;
}

// MUST BE CALLED before drawWith... is called!
+ (void)prepareToDrawCubes
{
    [[ROGLGouraudProgram sharedInstance] use];
}

- (void)drawWithBaseModelViewMatrix:(GLKMatrix4)baseModelViewMatrix 
                   projectionMatrix:(GLKMatrix4)projectionMatrix
                        timeElapsed:(NSTimeInterval)timeElapsed
{    
    GLKMatrix4 modelViewMatrix = GLKMatrix4MakeTranslation(-relativePosition.x, -relativePosition.y, -relativePosition.z);
    modelViewMatrix = GLKMatrix4Scale(modelViewMatrix, self.scale, self.scale, self.scale);
    modelViewMatrix = GLKMatrix4Rotate(modelViewMatrix, angle, axis.x, axis.y, axis.z);
    modelViewMatrix = GLKMatrix4Multiply(baseModelViewMatrix, modelViewMatrix);
    
    float actualAngleIncrement = angleIncrement * timeElapsed;    
    angle = fmodf(angle + actualAngleIncrement, TWO_PI);
    
    GLKMatrix4 modelViewProjectionMatrix = GLKMatrix4Multiply(projectionMatrix, modelViewMatrix);
    GLKMatrix3 normalMatrix = GLKMatrix3InvertAndTranspose(GLKMatrix4GetMatrix3(modelViewMatrix), NULL);
    
#warning CHECK HERE to see if the cube is within the projection frustrum, and if not don't draw it!
    
    [[ROGLGouraudProgram sharedInstance] setModelViewProjectionMatrix:modelViewProjectionMatrix normalMatrix:normalMatrix];
    [[ROGLGouraudProgram sharedInstance] setColor:color];
    
    [[ROGLShapes sharedInstance] drawCube];
}

- (void)renderAudioIntoBuffer:(SUAudioBuffer)buffer gain:(float)inGain
{    
    float masterGain = 0.07f;
    float gain = inGain * masterGain;
    [synth renderAudioIntoBuffer:buffer gain:gain];
}

@end


@interface SUWorldOrbitingTriangle ()
{
    ROGLGouraudProgram *program;
    
    GLKVector4 color;
    float inclination;
    float azimuth;
    float inclinationIncrement;
    float azimuthIncrement;
    GLKVector2 v[3]; // Represented as offsets of the base radial coordinates of this triangle
    float radiusPercentage;
}

@end

@implementation SUWorldOrbitingTriangle

- (id)init
{
    self = [super init];
    if (self) {
        radiusPercentage = 1.2f + 0.5 * (arc4random() / (float)0x100000000);
        
        inclination = TWO_PI * (arc4random() / (float)0x100000000) ;  // theta
        azimuth = TWO_PI * (arc4random() / (float)0x100000000);  // phi
        
        inclinationIncrement = 3.0f * (arc4random() / (float)0x100000000);  // radians per second
        azimuthIncrement = 3.0f * (arc4random() / (float)0x100000000);  // radians per second
        
        for (int i = 0; i < 3; i++)
        {
            v[i] = GLKVector2Make(fmodf(1.4 * (arc4random() / (float)0x100000000) - 0.7, TWO_PI), 
                                  fmodf(1.4 * (arc4random() / (float)0x100000000) - 0.7, TWO_PI));
        }
        
        color = GLKVector4Make(0.25 + 0.5 * (arc4random() / (float)0x100000000),
                               0.25 + 0.5 * (arc4random() / (float)0x100000000),
                               0.25 + 0.5 * (arc4random() / (float)0x100000000),
                               0.01f);
    }
    return self;
}

+ (void)prepareToDrawTrianglesWithModelViewMatrix:(GLKMatrix4)modelViewMatrix projectionMatrix:(GLKMatrix4)projectionMatrix
{
    [[ROGLGouraudProgram sharedInstance] use];
    GLKMatrix4 modelViewProjectionMatrix = GLKMatrix4Multiply(projectionMatrix, modelViewMatrix);
    GLKMatrix3 normalMatrix = GLKMatrix3InvertAndTranspose(GLKMatrix4GetMatrix3(modelViewMatrix), NULL);
    [[ROGLGouraudProgram sharedInstance] setModelViewProjectionMatrix:modelViewProjectionMatrix normalMatrix:normalMatrix];
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_ALPHA);
}

+ (void)finishDrawingTriangles
{
    glDisable(GL_BLEND);
}

- (void)drawWithRadius:(float)radius timeElapsed:(NSTimeInterval)timeElapsed
{
    inclination = fmodf(inclination + inclinationIncrement * timeElapsed, TWO_PI);
    azimuth = fmodf(azimuth + azimuthIncrement * timeElapsed, TWO_PI);

    GLKVector2 absoluteVertex;
    GLKVector3 vertexPositions[3];
    GLKVector3 vertexNormals[3];
    for (int i = 0; i < 3; i++)
    {
        absoluteVertex = GLKVector2Make(fmodf(inclination + v[i].x, TWO_PI), 
                                             fmodf(azimuth + v[i].y, TWO_PI));
        if (absoluteVertex.x < 0)
            absoluteVertex.x += TWO_PI;
        if (absoluteVertex.y < 0)
            absoluteVertex.y += TWO_PI;
        
        vertexPositions[i].x = radius * radiusPercentage * sin_lookup(absoluteVertex.y + M_PI_2) * sin_lookup(absoluteVertex.x);
        vertexPositions[i].y = radius * radiusPercentage * sin_lookup(absoluteVertex.y) * sin_lookup(absoluteVertex.x);
        vertexPositions[i].z = radius * radiusPercentage * sin_lookup(absoluteVertex.x + M_PI_2);
        vertexNormals[i] = GLKVector3Normalize(vertexPositions[i]);
    }
        
    float vertices[36] = 
    {
        vertexPositions[0].x, vertexPositions[0].y, vertexPositions[0].z, 
        vertexNormals[0].x, vertexNormals[0].y, vertexNormals[0].z,
        vertexPositions[1].x, vertexPositions[1].y, vertexPositions[1].z, 
        vertexNormals[1].x, vertexNormals[1].y, vertexNormals[1].z,
        vertexPositions[2].x, vertexPositions[2].y, vertexPositions[2].z, 
        vertexNormals[2].x, vertexNormals[2].y, vertexNormals[2].z,
        vertexPositions[0].x*0.999f, vertexPositions[0].y*0.999f, vertexPositions[0].z*0.999f, 
        -vertexNormals[0].x, -vertexNormals[0].y, -vertexNormals[0].z,
        vertexPositions[1].x*0.999f, vertexPositions[1].y*0.999f, vertexPositions[1].z*0.999f, 
        -vertexNormals[1].x, -vertexNormals[1].y, -vertexNormals[1].z,
        vertexPositions[2].x*0.999f, vertexPositions[2].y*0.999f, vertexPositions[2].z*0.999f, 
        -vertexNormals[2].x, -vertexNormals[2].y, -vertexNormals[2].z
    };

    [[ROGLGouraudProgram sharedInstance] setColor:color];
    
    glEnableVertexAttribArray(GLKVertexAttribPosition);
    glVertexAttribPointer(GLKVertexAttribPosition, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), vertices);
    glEnableVertexAttribArray(GLKVertexAttribNormal);
    glVertexAttribPointer(GLKVertexAttribNormal, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), vertices + 3);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

@end

@interface SUWorldSpringThing ()
{
    NSMutableArray *vertices; // holds spherical coordinate points (r, Θ, φ)
    GLKVector4 color;
    float inclination, azimuth;
    float inclinationIncrement, azimuthIncrement;
}
@end

@implementation SUWorldSpringThing

- (id)init {
    self = [super init];
    if (self) {
        vertices = [NSMutableArray arrayWithCapacity:40];
        float radius = 0.0f;
        inclination = TWO_PI * (arc4random() / (float)0x100000000);
        azimuth = TWO_PI * (arc4random() / (float)0x100000000);
        
        inclinationIncrement = 3.0f * (arc4random() / (float)0x100000000);  // radians per second
        azimuthIncrement = 3.0f * (arc4random() / (float)0x100000000);  // radians per second
        
        float inclinationOffset = 0.0f;
        float azimuthOffset = 0.0f;
        while (radius < 1.0f)
        {
            GLKVector3 vertex = GLKVector3Make(radius, inclinationOffset, azimuthOffset);
            NSValue *vertexValue = [NSValue valueWithBytes:&vertex objCType:@encode(GLKVector3)];
            [vertices addObject:vertexValue];
            radius += 0.1 * (arc4random() / (float)0x100000000);
            inclinationOffset = fmodf(inclinationOffset + 0.1 * (arc4random() / (float)0x100000000) - 0.05, TWO_PI);
            azimuthOffset = fmodf(azimuthOffset + 0.1 * (arc4random() / (float)0x100000000) - 0.05, TWO_PI);
        }
        while (radius > 0.0f)
        {
            GLKVector3 vertex = GLKVector3Make(radius, inclinationOffset, azimuthOffset);
            NSValue *vertexValue = [NSValue valueWithBytes:&vertex objCType:@encode(GLKVector3)];
            [vertices addObject:vertexValue];
            radius -= 0.1 * (arc4random() / (float)0x100000000);
            inclinationOffset = fmodf(inclinationOffset + 0.1 * (arc4random() / (float)0x100000000) - 0.05, TWO_PI);
            azimuthOffset = fmodf(azimuthOffset + 0.1 * (arc4random() / (float)0x100000000) - 0.05, TWO_PI);            
        }
        
        color = GLKVector4Make(0.25 + 0.5 * (arc4random() / (float)0x100000000),
                               0.25 + 0.5 * (arc4random() / (float)0x100000000),
                               0.25 + 0.5 * (arc4random() / (float)0x100000000),
                               1.0f);
    }
    return self;
}

+ (void)prepareToDrawSpringThingsWithModelViewMatrix:(GLKMatrix4)modelViewMatrix projectionMatrix:(GLKMatrix4)projectionMatrix
{
    [[ROGLGouraudProgram sharedInstance] use];
    GLKMatrix4 modelViewProjectionMatrix = GLKMatrix4Multiply(projectionMatrix, modelViewMatrix);
    GLKMatrix3 normalMatrix = GLKMatrix3InvertAndTranspose(GLKMatrix4GetMatrix3(modelViewMatrix), NULL);
    [[ROGLGouraudProgram sharedInstance] setModelViewProjectionMatrix:modelViewProjectionMatrix normalMatrix:normalMatrix];
}

+ (void)finishDrawingSpringThings
{
    
}

- (void)drawWithRadius:(float)radius timeElapsed:(NSTimeInterval)timeElapsed
{    
    inclination = fmodf(inclination + inclinationIncrement * timeElapsed, TWO_PI);
    azimuth = fmodf(azimuth + azimuthIncrement * timeElapsed, TWO_PI);
    if (inclination < 0)
        inclination += TWO_PI;
    if (azimuth < 0)
        azimuth += TWO_PI;
    
    int numVertexAttribs = 3;  // 6 when I figure out normals for these things
    float glVertices[[vertices count] * numVertexAttribs];
    int i = 0;
    float r, inc, azi;
    for (NSValue *vertexValue in vertices)
    {
        GLKVector3 vertex;
        [vertexValue getValue:&vertex];
        r = 2.0f * radius * vertex.x;
        inc = fmodf(inclination + vertex.y, TWO_PI);
        if (inc < 0)
            inc += TWO_PI;
        azi = fmodf(azimuth + vertex.z, TWO_PI);
        if (azi < 0)
            azi += TWO_PI;
        GLKVector3 vertexPosition = GLKVector3Make(r * sin_lookup(azi + M_PI_2) * sin_lookup(inc),
                                                   r * sin_lookup(azi) * sin_lookup(inc),
                                                   r * sin_lookup(inc + M_PI_2));
        glVertices[i * numVertexAttribs + 0] = vertexPosition.x;
        glVertices[i * numVertexAttribs + 1] = vertexPosition.y;
        glVertices[i * numVertexAttribs + 2] = vertexPosition.z;
        i++;
    }
    
    [[ROGLGouraudProgram sharedInstance] setColor:color];
    
    glEnableVertexAttribArray(GLKVertexAttribPosition);
    glVertexAttribPointer(GLKVertexAttribPosition, 3, GL_FLOAT, GL_FALSE, numVertexAttribs * sizeof(float), glVertices);
//    glEnableVertexAttribArray(GLKVertexAttribNormal);
//    glVertexAttribPointer(GLKVertexAttribNormal, 3, GL_FLOAT, GL_FALSE, numVertexAttribs * sizeof(float), vertices + 3);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, [vertices count]);
}

@end
