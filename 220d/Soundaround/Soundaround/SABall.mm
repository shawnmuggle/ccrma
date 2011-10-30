//
//  SABall.mm
//  Soundaround
//
//  Created by Michael Rotondo on 10/21/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#include <Box2D/Box2D.h>
#include "OpenGL/gl.h"
#import "SABall.h"
#import "SAWorld.h"

@interface SABall ()

@property (nonatomic, assign) float radius;
@property (nonatomic, assign) float envelopeAmplitude;
@property (nonatomic, assign) float frequency;
@property (nonatomic, assign) float phase;

@end

@implementation SABall
@synthesize radius;
@synthesize envelopeAmplitude;
@synthesize frequency;
@synthesize phase;

+ (id)ballInWorld:(SAWorld *)world
{
    SABall *ball = [[[SABall alloc] init] autorelease];
    
    ball.radius = 1.0f;  // in meters
    
    b2BodyDef bd;
    float32 x = 15.0f;
    float32 y = 20.0f;
    bd.position.Set(x, y);
    bd.angle = 0.0f;
    bd.type = b2_dynamicBody;
    bd.linearDamping = 0.3f;
    bd.angularDamping = 0.02f;
    ball.physicsBody = world.physicsWorld->CreateBody(&bd);
    
    b2FixtureDef fd;
    b2CircleShape circleShape;
    circleShape.m_radius = ball.radius;
    fd.shape = &circleShape;
    fd.friction = 0.3f;
    fd.density = 20.0f;
    fd.restitution = 0.6f;
    ball.physicsBody->CreateFixture(&fd);
    
    [world registerBody:ball.physicsBody withObject:ball];
    
    return ball;
}

- (void)dealloc
{
    [super dealloc];
}

- (void)draw
{
    const b2Vec2 &pos = self.physicsBody->GetWorldCenter();
    float angle = self.physicsBody->GetAngle();
    
    glPushMatrix();
    glTranslatef(pos.x, pos.y, 0.0f);
    glRotatef(180 * (angle / M_PI), 0.0f, 0.0f, 1.0f);
    glTranslatef(-pos.x, -pos.y, 0.0f);

    glBegin(GL_TRIANGLE_FAN);
    
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f(pos.x, pos.y);

    float circleAngle = 0;
    int numSections = 20;
    float x, y;
    for (int i = 0; i < numSections; ++i)
    {
        circleAngle = 2 * M_PI * (i / (float)(numSections));
        x = pos.x + self.radius * cos(circleAngle);
        y = pos.y + self.radius * sin(circleAngle);
        glVertex2f(x, y);
    }
    
    glEnd();
    
    glPopMatrix();
}

- (void)processContact:(b2Contact *)contact withOtherObject:(SAPhysicsObject *)otherObject
{
    NSLog(@"BALL COLLISION");
    
    b2WorldManifold worldManifold;
    contact->GetWorldManifold(&worldManifold);
    b2Vec2 point = worldManifold.points[0];
    const b2Vec2 &vectorA = self.physicsBody->GetLinearVelocityFromWorldPoint(point);
    const b2Vec2 &vectorB = otherObject.physicsBody->GetLinearVelocityFromWorldPoint(point);
    const b2Vec2 &difference = vectorB - vectorA;
    float32 approachVelocity = b2Dot(difference, worldManifold.normal);
    
    NSLog(@"Velocity: %f", approachVelocity);
    
    float newEnvelopeAmplitude = MAX(0, (-1.0 / powf(approachVelocity - 1, 0.3) + 1));
    if (newEnvelopeAmplitude > 0.001)
    {
        self.frequency = 200 + 600 * newEnvelopeAmplitude;
        self.envelopeAmplitude = newEnvelopeAmplitude;        
    }
}

- (void)audioProducer:(id<SAAudioProducer>)producer fillBuffer:(float *)output withNumFrames:(unsigned int)numBufferFrames
{
    int numChannels = 2;
  
    for (int i = 0; i < numBufferFrames; i++)
    {
        float noiseSample = (rand() / (float)RAND_MAX);
        float sineSample = sin(self.phase);
        float envelopedSample = (noiseSample + sineSample) * self.envelopeAmplitude;
        for (int j = 0; j < numChannels; j++)
        {
            output[i * numChannels + j] += envelopedSample;
        }
        self.envelopeAmplitude *= 0.999;
        self.phase += (self.frequency * 2 * M_PI) / 44100;
    }
}

- (void)throwTowards:(CGPoint)point
{
    const b2Vec2 &pos = self.physicsBody->GetWorldCenter();
    const b2Vec2 vector(point.x - pos.x, point.y - pos.y);
    NSLog(@"applyforce: %f, %f", vector.x, vector.y);
    self.physicsBody->ApplyForceToCenter(1000 * vector);
}

@end
