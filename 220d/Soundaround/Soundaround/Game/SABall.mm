//
//  SABall.m
//  Soundaround
//
//  Created by Michael Rotondo on 10/21/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#import "SABall.h"

#include <Box2D/Box2D.h>

#import "SAWorld.h"

@interface SABall ()

@property (nonatomic, assign) b2Body *physicsBall;
@property (nonatomic, assign) float radius;

@end

@implementation SABall
@synthesize physicsBall;
@synthesize radius;

+ (id)ballInWorld:(SAWorld *)world
{
    SABall *ball = [[[SABall alloc] init] autorelease];
    
    ball.radius = 1.0f;  // in meters
    
    b2BodyDef bd;
    float32 x = 15.0f;
    float32 y = 6.0f;
    bd.position.Set(x, y);
    bd.angle = 0.0f;
    bd.type = b2_dynamicBody;
    bd.angularDamping = 0.02f;
    ball.physicsBall = world.physicsWorld->CreateBody(&bd);
    
    b2FixtureDef fd;
    b2CircleShape circleShape;
    circleShape.m_radius = ball.radius;
    fd.shape = &circleShape;
    fd.friction = 0.3f;
    fd.density = 20.0f;
    ball.physicsBall->CreateFixture(&fd);
    
    return ball;
}

- (void)dealloc
{
    [super dealloc];
}

- (void)draw
{
    const b2Vec2 &pos = self.physicsBall->GetWorldCenter();

    glBegin(GL_TRIANGLE_FAN);
    glColor3f(1.0, 1.0, 1.0);
    glVertex2f(pos.x, pos.y);
    float angle = 0;
    int numSections = 20;
    float x, y;
    for (int i = 0; i < numSections; ++i)
    {
        angle = 2 * M_PI * (i / (float)(numSections - 1));
        x = pos.x + self.radius * cos(angle);
        y = pos.y + self.radius * sin(angle);
        glVertex2f(x, y);
    }
    
    glEnd();
}

- (void)throwTowards:(CGPoint)point
{
    const b2Vec2 &pos = self.physicsBall->GetWorldCenter();
    const b2Vec2 vector(point.x - pos.x, point.y - pos.y);
    NSLog(@"applyforce: %f, %f", vector.x, vector.y);
    self.physicsBall->ApplyForceToCenter(1000 * vector);
}

@end
