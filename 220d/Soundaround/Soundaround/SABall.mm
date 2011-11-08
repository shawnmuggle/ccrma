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
#import "SASoundPath.h"
#import "SASoundPoint.h"
#import "SABallGuide.h"

@interface SABall ()
{
@private
    SAWorld *world;

    float radius;
    float envelopeAmplitude;
    float frequency;
    float phase;
    
    SASoundPath *currentPath;
}

@end

@implementation SABall
@synthesize held;
@synthesize currentGuide;

- (id)initWithWorld:(SAWorld *)aWorld
{
    self = [super init];
    if (self)
    {
        world = aWorld;
        
        radius = 1.0f;  // in meters
        envelopeAmplitude = 0.0f;
        frequency = 0.0f;
        phase = 0.0f;
        
        b2BodyDef bd;
        float32 x = 15.0f;
        float32 y = 20.0f;
        bd.position.Set(x, y);
        bd.angle = 0.0f;
        bd.type = b2_dynamicBody;
        bd.linearDamping = 0.0f;
        bd.angularDamping = 0.02f;
        self.physicsBody = world.physicsWorld->CreateBody(&bd);
        
        b2FixtureDef fd;
        b2CircleShape circleShape;
        circleShape.m_radius = radius;
        fd.shape = &circleShape;
        fd.friction = 0.3f;
        fd.density = 40.0f;
        fd.restitution = 0.4f;
        self.physicsBody->CreateFixture(&fd);
        
        [world registerBody:self.physicsBody withObject:self];
    }
    return self;
}

- (void)draw
{
    const b2Vec2 &pos = self.physicsBody->GetWorldCenter();
    float angle = self.physicsBody->GetAngle();
    
    if (envelopeAmplitude > 0.001f)
    {
        [currentPath addPoint:[[SASoundPoint alloc] initWithPosition:CGPointMake(pos.x, pos.y) andIntensity:envelopeAmplitude andAngle:angle]];
    }
    
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
        x = pos.x + radius * cos(circleAngle);
        y = pos.y + radius * sin(circleAngle);
        glVertex2f(x, y);
    }
    
    glEnd();
    
    glPopMatrix();
    
    [currentPath draw];
}

- (void)setHeld:(BOOL)inHeld
{
    if (inHeld)
    {
        self.physicsBody->SetLinearDamping(0.3f);
        self.physicsBody->SetAngularDamping(0.3f);
        self.physicsBody->SetGravityScale(0.0f);
        [self.currentGuide deactivate];
    }
    else
    {
        self.physicsBody->SetLinearDamping(0.0f);        
        self.physicsBody->SetAngularDamping(0.02f);
        self.physicsBody->SetGravityScale(1.0f);
    }
    held = inHeld;
}

- (BOOL)shouldDisableContact:(b2Contact *)contact
{
    return self.held;
}


- (void)processContact:(b2Contact *)contact withOtherObject:(SAPhysicsObject *)otherObject
{
    if (self.held || self.currentGuide)
    {
        contact->SetEnabled(false);
        return;
    }

    b2WorldManifold worldManifold;
    contact->GetWorldManifold(&worldManifold);
    b2Vec2 point = worldManifold.points[0];
    const b2Vec2 &vectorA = self.physicsBody->GetLinearVelocityFromWorldPoint(point);
    const b2Vec2 &vectorB = otherObject.physicsBody->GetLinearVelocityFromWorldPoint(point);
    const b2Vec2 &difference = vectorB - vectorA;
    float32 approachVelocity = b2Dot(difference, worldManifold.normal);
    
    float newEnvelopeAmplitude = MAX(0, (-1.0 / powf(approachVelocity - 1, 0.3) + 1));
    if (newEnvelopeAmplitude > 0.001)
    {
        frequency = 50 + 1000 * newEnvelopeAmplitude;
        envelopeAmplitude = newEnvelopeAmplitude;
        if (currentPath)
        {
            [world addSoundPath:currentPath];
        }
        currentPath = [[SASoundPath alloc] init];
    }
}

- (void)audioProducer:(id<SAAudioProducer>)producer fillBuffer:(float *)output withNumFrames:(unsigned int)numBufferFrames
{
    int numChannels = 2;
  
    for (int i = 0; i < numBufferFrames; i++)
    {
        float noiseSample = (rand() / (float)RAND_MAX);
        float sineSample = sin(phase);
        float envelopedSample = (noiseSample + sineSample) * envelopeAmplitude;
        for (int j = 0; j < numChannels; j++)
        {
            output[i * numChannels + j] += envelopedSample;
        }
        envelopeAmplitude *= 0.999;
        phase += (frequency * 2 * M_PI) / 44100;
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
