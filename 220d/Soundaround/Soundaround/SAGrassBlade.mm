//
//  SAGrassBlade.mm
//  Soundaround
//
//  Created by Michael Rotondo on 10/24/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#include <Box2D/Box2D.h>
#include "OpenGL/gl.h"
#import "SAGrassBlade.h"
#import "SAWorld.h"

@interface SAGrassBlade ()

@property (nonatomic, assign) b2Shape *shape;
@property (nonatomic, assign) b2RevoluteJoint *joint;

@end

@implementation SAGrassBlade
@synthesize shape;
@synthesize joint;

+ (id)grassbladeWithWorld:(SAWorld *)world atPosition:(CGPoint)position
{
    SAGrassBlade *grassblade = [[SAGrassBlade alloc] init];
    
    b2BodyDef bd;
    bd.position.Set(position.x, position.y + 1.0f);
    bd.angularDamping = 0.02f;
    bd.type = b2_dynamicBody;
    grassblade.physicsBody = world.physicsWorld->CreateBody(&bd);
    
        b2CircleShape shape;
    shape.m_radius = 0.01f;

    b2FixtureDef fd;
//    b2Vec2 v1(0.0f, 0.0f);
//    b2Vec2 v2(0.2f, 0.3f + 0.2f * (rand() / (float)RAND_MAX) );
//    grassblade.shape = new b2EdgeShape();
//    grassblade.shape->Set(v1, v2);
//    fd.shape = grassblade.shape;
    grassblade.physicsBody->CreateFixture(&shape, 5.0f);
    
    b2RevoluteJointDef jointDef;
    jointDef.Initialize(grassblade.physicsBody, world.physicsBody, b2Vec2(position.x, position.y));
    jointDef.lowerAngle = -0.25f * b2_pi; // -90 degrees
    jointDef.upperAngle = 0.25f * b2_pi; // 45 degrees
    jointDef.enableLimit = true;
    jointDef.maxMotorTorque = 1.0f;
    jointDef.motorSpeed = 0.0f;
    jointDef.enableMotor = true;
    grassblade.joint = (b2RevoluteJoint*)world.physicsWorld->CreateJoint(&jointDef);
    
//    grassblade.physicsBody->ApplyAngularImpulse(500.0f);
    
    [world registerBody:grassblade.physicsBody withObject:grassblade];
    
    return grassblade;
}

- (void)draw
{
    float32 angleError = self.joint->GetJointAngle();
    float32 gain = 0.001f;
    self.joint->SetMotorSpeed(-gain * angleError);
    
//    self.joint->SetMotorSpeed(cosf(100.0f * CFAbsoluteTimeGetCurrent()));
    
    const b2Vec2 &anchor = self.joint->GetAnchorA();    
    const b2Vec2 &pos = self.physicsBody->GetWorldCenter();
    float angle = self.physicsBody->GetAngle();
    
    glPushMatrix();
    glTranslatef(anchor.x, anchor.y, 0.0f);
    glRotatef(180 * (angle / M_PI), 0.0f, 0.0f, 1.0f);
    glTranslatef(-anchor.x, -anchor.y, 0.0f);
    
    glBegin(GL_LINES);
    
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex2f(anchor.x, anchor.y);
    glVertex2f(pos.x, pos.y);
//    const b2Vec2 &v1 = self.edgeShape->m_vertex1;
//    const b2Vec2 &v2 = self.edgeShape->m_vertex2;
//    glVertex2f(pos.x + v1.x, pos.y + v1.y);
//    glVertex2f(pos.x + v2.x, pos.y + v2.y);
    
    glEnd();
    
    glPopMatrix();
}

@end
