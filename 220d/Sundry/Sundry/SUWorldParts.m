//
//  SUWorldParts.m
//  Sundry
//
//  Created by Michael Rotondo on 12/14/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import "SUWorldParts.h"
#import "ROGLProgram.h"
#import "ROGLShapes.h"
#import "SUPlayer.h"
#import "SUSynths.h"
#import "fast_funcs.h"
#import "SUTimeline.h"

static float TWO_PI = 2 * M_PI;

@interface SUWorldCube ()
{
    float angle;

    SUFMSynth *synth;    
}
@end

@implementation SUWorldCube
@synthesize scale, minScale, maxScale;
@synthesize angleIncrement, minAngleIncrement, maxAngleIncrement;
@synthesize axis;
@synthesize relativePosition, minRelativePosition, maxRelativePosition;
@synthesize color;

- (id)init
{
    self = [super init];
    if (self) {
        synth = [[SUFMSynth alloc] init];
        
        self.minRelativePosition = -5.0f;
        self.maxRelativePosition = 5.0f;
        self.relativePosition = GLKVector3Make(self.minRelativePosition + (self.maxRelativePosition - self.minRelativePosition) * 
                                               (arc4random() / (float)0x100000000), 
                                               self.minRelativePosition + (self.maxRelativePosition - self.minRelativePosition) * 
                                               (arc4random() / (float)0x100000000), 
                                               self.minRelativePosition + (self.maxRelativePosition - self.minRelativePosition) *
                                               (arc4random() / (float)0x100000000));
        
        angle = 0.0f;
        self.minAngleIncrement = 0.0f;
        self.maxAngleIncrement = 4.0f;
        self.angleIncrement = self.minAngleIncrement + (self.maxAngleIncrement - self.minAngleIncrement) * (arc4random() / (float)0x100000000);  // radians per second
        
        self.axis = GLKVector3Make((arc4random() / (float)0x100000000), 
                                   (arc4random() / (float)0x100000000), 
                                   (arc4random() / (float)0x100000000));
        
        self.minScale = 20.0f;
        self.maxScale = 50.0f;
        self.scale = self.minScale + (self.maxScale - self.minScale) * (arc4random() / (float)0x100000000);
        
        self.color = GLKVector4Make((arc4random() / (float)0x100000000),
                                    (arc4random() / (float)0x100000000),
                                    (arc4random() / (float)0x100000000),
                                    1.0f);
    }
    return self;
}

- (void)setScale:(float)inScale
{
    scale = inScale;
    synth.carrierFreq = 100.0f + 600.0f * (1.0f - (self.scale / self.maxScale));
}

- (void)setAngleIncrement:(float)inAngleIncrement
{
    angleIncrement = inAngleIncrement;
    synth.lfoFreq = angleIncrement / TWO_PI;
}

- (void)setAxis:(GLKVector3)inAxis
{
    axis = inAxis;
    synth.modFreq = 200 * GLKVector3Length(self.axis);
}

- (void)setColor:(GLKVector4)inColor
{
    color = inColor;
    synth.modDepth = synth.carrierFreq * (GLKVector3Length(GLKVector3Make(self.color.x, self.color.y, self.color.z)) / GLKVector3Length(GLKVector3Make(1.0f, 1.0f, 1.0f)));
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
    GLKMatrix4 modelViewMatrix = GLKMatrix4MakeTranslation(-self.relativePosition.x, 
                                                           -self.relativePosition.y, 
                                                           -self.relativePosition.z);
    modelViewMatrix = GLKMatrix4Scale(modelViewMatrix, self.scale, self.scale, self.scale);
    modelViewMatrix = GLKMatrix4Rotate(modelViewMatrix, angle, self.axis.x, self.axis.y, self.axis.z);
    modelViewMatrix = GLKMatrix4Multiply(baseModelViewMatrix, modelViewMatrix);
    
    float actualAngleIncrement = self.angleIncrement * timeElapsed;    
    angle = fmodf(angle + actualAngleIncrement, TWO_PI);
    
    GLKMatrix4 modelViewProjectionMatrix = GLKMatrix4Multiply(projectionMatrix, modelViewMatrix);
    GLKMatrix3 normalMatrix = GLKMatrix3InvertAndTranspose(GLKMatrix4GetMatrix3(modelViewMatrix), NULL);
    
#warning CHECK HERE to see if the cube is within the projection frustrum, and if not don't draw it!
    
    [[ROGLGouraudProgram sharedInstance] setModelViewProjectionMatrix:modelViewProjectionMatrix normalMatrix:normalMatrix];
    [[ROGLGouraudProgram sharedInstance] setColor:self.color];
    
    [[ROGLShapes sharedInstance] drawCube];
}

- (void)renderAudioIntoBuffer:(SUAudioBuffer)buffer gain:(float)inGain
{    
    float masterGain = 0.08f;
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
            v[i] = GLKVector2Make(fmodf(1.0 * (arc4random() / (float)0x100000000) - 0.5, TWO_PI), 
                                  fmodf(1.0 * (arc4random() / (float)0x100000000) - 0.5, TWO_PI));
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
    
    NSTimeInterval timeSinceSpringing;
    float maxSpringAmplitude;
    float amplitudeFollower;
    float amplitudeFollowingAmount;
    
    SUPercSynth *synth;
    SUTimeline *timeline;
    NSTimeInterval totalTimeElapsed;
    
    // For emphasis!
    GLKVector3 scale;
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
        
        synth = [[SUPercSynth alloc] init];
        synth.startFreq = inclination * 300.0f;
        synth.endFreq = azimuth * 300.0f;
        synth.length = inclinationIncrement;
        
        amplitudeFollowingAmount = 0.8f;
        
        scale = GLKVector3Make(1.0f + (arc4random() / (float)0x100000000), 
                               1.0f + (arc4random() / (float)0x100000000), 
                               1.0f + (arc4random() / (float)0x100000000));
        
        float timeLineLength = 2.0f;
        timeline = [[SUTimeline alloc] initWithLength:timeLineLength];
        for (int i = 0; i < 3 + (int)(3 * (arc4random() / (float)0x100000000)); i++)
        {
            [timeline addEvent:(arc4random() / (float)0x100000000) atTime:timeLineLength * (arc4random() / (float)0x100000000)];
        }
        
        float inclinationOffset = 0.0f;
        float azimuthOffset = 0.0f;
        while (radius < 1.0f)
        {
            GLKVector3 vertex = GLKVector3Make(radius, inclinationOffset, azimuthOffset);
            NSValue *vertexValue = [NSValue valueWithBytes:&vertex objCType:@encode(GLKVector3)];
            [vertices addObject:vertexValue];
            radius += 0.1 * (arc4random() / (float)0x100000000);
            inclinationOffset = fmodf(inclinationOffset + 0.2 * (arc4random() / (float)0x100000000) - 0.1, TWO_PI);
            azimuthOffset = fmodf(azimuthOffset + 0.2 * (arc4random() / (float)0x100000000) - 0.1, TWO_PI);
        }
        while (radius > 0.0f)
        {
            GLKVector3 vertex = GLKVector3Make(radius, inclinationOffset, azimuthOffset);
            NSValue *vertexValue = [NSValue valueWithBytes:&vertex objCType:@encode(GLKVector3)];
            [vertices addObject:vertexValue];
            radius -= 0.1 * (arc4random() / (float)0x100000000);
            inclinationOffset = fmodf(inclinationOffset + 0.2 * (arc4random() / (float)0x100000000) - 0.1, TWO_PI);
            azimuthOffset = fmodf(azimuthOffset + 0.2 * (arc4random() / (float)0x100000000) - 0.1, TWO_PI);            
        }
        
        color = GLKVector4Make(0.25 + 0.5 * (arc4random() / (float)0x100000000),
                               0.25 + 0.5 * (arc4random() / (float)0x100000000),
                               0.25 + 0.5 * (arc4random() / (float)0x100000000),
                               1.0f);
    }
    return self;
}

+ (void)prepareToDrawSpringThings
{
    [[ROGLGouraudProgram sharedInstance] use];
}

+ (void)finishDrawingSpringThings
{
    
}

- (void)drawWithRadius:(float)radius 
           timeElapsed:(NSTimeInterval)timeElapsed 
   baseModelViewMatrix:(GLKMatrix4)baseModelViewMatrix 
      projectionMatrix:(GLKMatrix4)projectionMatrix;
{
    SUTimeEvent event = [timeline lastValueBetweenStartTime:totalTimeElapsed endTime:totalTimeElapsed + timeElapsed];
    if (event.time > 0)
    {
        [self springWithAmplitude:event.value];
    }
    totalTimeElapsed += timeElapsed;
    
    timeSinceSpringing += timeElapsed;
    float amplitude = 0.0f;
    if (timeSinceSpringing <= synth.length)
    {
        amplitude = maxSpringAmplitude * expf(-timeSinceSpringing / (0.2f * synth.length));
    }
    amplitudeFollower = amplitudeFollower + amplitudeFollowingAmount * (amplitude - amplitudeFollower);
    
    GLKVector3 currentScale = GLKVector3MultiplyScalar(scale, amplitudeFollower / maxSpringAmplitude);
    GLKMatrix4 modelViewMatrix = GLKMatrix4Scale(baseModelViewMatrix, currentScale.x, currentScale.y, currentScale.z);
    GLKMatrix4 modelViewProjectionMatrix = GLKMatrix4Multiply(projectionMatrix, modelViewMatrix);
    GLKMatrix3 normalMatrix = GLKMatrix3InvertAndTranspose(GLKMatrix4GetMatrix3(modelViewMatrix), NULL);
    [[ROGLGouraudProgram sharedInstance] setModelViewProjectionMatrix:modelViewProjectionMatrix normalMatrix:normalMatrix];
    
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
        r = amplitudeFollower * 2.0f * radius * vertex.x;
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
    
    GLKVector4 brightener = GLKVector4MultiplyScalar(GLKVector4Make(1.0f - color.x, 1.0f - color.y, 1.0f - color.z, 0.0), amplitudeFollower / maxSpringAmplitude);
    [[ROGLGouraudProgram sharedInstance] setColor:GLKVector4Add(color, brightener)];
    
    glEnableVertexAttribArray(GLKVertexAttribPosition);
    glVertexAttribPointer(GLKVertexAttribPosition, 3, GL_FLOAT, GL_FALSE, numVertexAttribs * sizeof(float), glVertices);
    //    glEnableVertexAttribArray(GLKVertexAttribNormal);
    //    glVertexAttribPointer(GLKVertexAttribNormal, 3, GL_FLOAT, GL_FALSE, numVertexAttribs * sizeof(float), vertices + 3);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, [vertices count]);
}

- (void)renderAudioIntoBuffer:(SUAudioBuffer)buffer gain:(float)inGain
{    
    float masterGain = 0.1f;
    float gain = inGain * masterGain;
    [synth renderAudioIntoBuffer:buffer gain:gain];
}

- (void)springWithAmplitude:(float)amplitude
{
    maxSpringAmplitude = amplitude;
    timeSinceSpringing = 0.0f;
    [synth play:amplitude];
}

@end
