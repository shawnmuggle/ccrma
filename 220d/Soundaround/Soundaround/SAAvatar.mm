//
//  SAAvatar.mm
//  Soundaround
//
//  Created by Michael Rotondo on 10/21/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#include <Box2D/Box2D.h>
#include "OpenGL/gl.h"
#import "SAAvatar.h"
#import "SAWorld.h"
#import "SABall.h"

@interface SAAvatar ()
{
    b2PolygonShape *polygonShape;
    CGSize size;
    BOOL grabbing;
    CGPoint movementVector;
    CGPoint facingVector;
}

@end

@implementation SAAvatar
@synthesize ball;

- (id)initWithWorld:(SAWorld *)inWorld andBall:(SABall *)inBall
{
    self = [super init];
    if (self)
    {
        b2BodyDef bd;
        float32 x = 5.0f;
        float32 y = 15.0f;
        bd.position.Set(x, y);
        bd.angle = 0.0f;
        bd.type = b2_dynamicBody;
        bd.linearDamping = 0.3f;
        bd.fixedRotation = true; // This is easier than making a weeble-wobble, for now.
        //    bd.angularDamping = 0.02f;
        //    bd.angularDamping = 5000.02f;
        self.physicsBody = inWorld.physicsWorld->CreateBody(&bd);
        
        b2FixtureDef fd;
        polygonShape = new b2PolygonShape();
        
        b2Vec2 vertices[6];
        vertices[0] = b2Vec2(-1.0f, -0.8f);
        vertices[1] = b2Vec2(-0.8f, -1.0f);
        vertices[2] = b2Vec2(0.8f, -1.0f);
        vertices[3] = b2Vec2(1.0f, -0.8f);
        vertices[4] = b2Vec2(0.5f, 3.0f);
        vertices[5] = b2Vec2(-0.5f, 3.0f);
        polygonShape->Set(vertices, 6);
        
        fd.shape = polygonShape;
        fd.friction = 0.3f;
        fd.density = 20.0f;
        self.physicsBody->CreateFixture(&fd);
        
        self.ball = inBall;
        
        [inWorld registerBody:self.physicsBody withObject:self];
    }
    return self;
}

- (void)draw
{
    
    const b2Vec2 &pos = self.physicsBody->GetWorldCenter();
    const b2Vec2 &localPos = self.physicsBody->GetLocalCenter();    
    
    float angle = self.physicsBody->GetAngle();

    glPushMatrix();
    glTranslatef(pos.x, pos.y, 0.0f);
    glRotatef(180 * (angle / M_PI), 0.0f, 0.0f, 1.0f);
    glTranslatef(-pos.x, -pos.y, 0.0f);
    
    glTranslatef(-localPos.x, -localPos.y, 0.0f);
    
    glBegin(GL_POLYGON);
    glColor3f(1.0, 1.0, 1.0);
    for (int i = 0; i < polygonShape->GetVertexCount(); ++i)
    {
        b2Vec2 point = polygonShape->GetVertex(i);
        glVertex2f(pos.x + point.x, pos.y + point.y);
    }
    
    glEnd();
    
    glPopMatrix();
    
    if (grabbing)
    {
        glColor3f(0.8f, 0.1f, 0.06f);
        
        const b2Vec2 &ballPos = self.ball.physicsBody->GetWorldCenter();
        const b2Vec2 &vectorToBall = ballPos - (pos + localPos);
        
        float lineSegmentLength = 0.3;
        int numLineSegments = vectorToBall.Length() / lineSegmentLength;
        int numLines = 2;
        for (int i = 0; i < numLines; i++)
        {
            glBegin(GL_LINE_STRIP);
            
            for (int j = 0; j < numLineSegments; j++)
            {
                float percent = (float)j / (numLineSegments - 1);
                b2Vec2 interpolatedPoint(pos + localPos + percent * vectorToBall);
                b2Vec2 randomVector(rand() - RAND_MAX / 2, rand() - RAND_MAX / 2);
                randomVector.Normalize();
                const b2Vec2 &randomizedInterpolatedPoint = interpolatedPoint + 0.1 * randomVector;
                glVertex2f(randomizedInterpolatedPoint.x, randomizedInterpolatedPoint.y);
            }
            
            glEnd();
        }
    }
}

- (void)update
{
    self.physicsBody->ApplyForceToCenter(5000 * b2Vec2(movementVector.x, movementVector.y));

    if (grabbing)
    {
        b2Vec2 holdPoint = self.physicsBody->GetWorldCenter() + b2Vec2(0.0f, 4.0f);
        b2Vec2 vectorTowardsHoldPoint = holdPoint - self.ball.physicsBody->GetWorldCenter();
        self.ball.physicsBody->SetLinearVelocity(100.0f * vectorTowardsHoldPoint);
    }
}

- (BOOL)shouldDisableContact:(b2Contact *)contact
{
    return NO;
}

- (void)processContact:(b2Contact *)contact withOtherObject:(id)otherObject
{
//    NSLog(@"AVATAR COLLISION");
}

- (void)audioProducer:(id<SAAudioProducer>)producer fillBuffer:(float *)output withNumFrames:(unsigned int)numBufferFrames
{
    
}

#pragma mark Control

- (void)startMoving:(CGPoint)direction
{
    movementVector = direction;
    facingVector = direction;
}

- (void)stopMoving:(CGPoint)direction
{
    if (CGPointEqualToPoint(direction, movementVector))
        movementVector = CGPointMake(0.0f, 0.0f);
}

- (void)grabBall
{
    grabbing = YES;
    self.ball.held = YES;

    self.ball.physicsBody->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
    self.ball.physicsBody->SetGravityScale(0.0f);
}

- (void)throwBallTowardsPoint:(CGPoint)point
{
    if (grabbing)
    {
        grabbing = NO;
        self.ball.held = NO;
        b2Vec2 vectorTowardsThrowTarget = b2Vec2(point.x, point.y) - self.ball.physicsBody->GetWorldCenter();
        vectorTowardsThrowTarget.Normalize();
        self.ball.physicsBody->ApplyForceToCenter(250000.0f * vectorTowardsThrowTarget);
        self.ball.physicsBody->SetGravityScale(1.0f);
    }
}

@end
