//
//  SAAvatar.mm
//  Soundaround
//
//  Created by Michael Rotondo on 10/21/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#include <Box2D/Box2D.h>

#import "SAAvatar.h"
#import "SAWorld.h"

@interface SAAvatar ()

@property (nonatomic, assign) b2PolygonShape *polygonShape;
@property (nonatomic, assign) CGSize size;

@end

@implementation SAAvatar
@synthesize polygonShape;
@synthesize size;

+ (id)avatarInWorld:(SAWorld *)world
{
    SAAvatar *avatar = [[[SAAvatar alloc] init] autorelease];
    
    b2BodyDef bd;
    float32 x = 5.0f;
    float32 y = 6.0f;
    bd.position.Set(x, y);
    bd.angle = 0.0f;
    bd.type = b2_dynamicBody;
    bd.linearDamping = 0.3f;
    bd.fixedRotation = true; // This is easier than making a weeble-wobble, for now.
//    bd.angularDamping = 0.02f;
//    bd.angularDamping = 5000.02f;
    avatar.physicsBody = world.physicsWorld->CreateBody(&bd);
    
    b2FixtureDef fd;
    avatar.polygonShape = new b2PolygonShape();

    b2Vec2 vertices[4];
    vertices[0] = b2Vec2(-1.0f, -1.0f);
    vertices[1] = b2Vec2(1.0f, -1.0f);
    vertices[2] = b2Vec2(0.5f, 3.0f);
    vertices[3] = b2Vec2(-0.5f, 3.0f);
    avatar.polygonShape->Set(vertices, 4);
    
//    avatar.polygonShape->SetAsBox(0.5f, 2.0f);
    fd.shape = avatar.polygonShape;
    fd.friction = 0.3f;
    fd.density = 20.0f;
    avatar.physicsBody->CreateFixture(&fd);

    [world registerBody:avatar.physicsBody withObject:avatar];
    
    return avatar;
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
    for (int i = 0; i < self.polygonShape->GetVertexCount(); ++i)
    {
        b2Vec2 point = self.polygonShape->GetVertex(i);
        glVertex2f(pos.x + point.x, pos.y + point.y);
    }
    
    glEnd();
    
    glPopMatrix();
}

- (void)processContact:(b2Contact *)contact withOtherObject:(id)otherObject
{
    NSLog(@"AVATAR COLLISION");
}

- (void)audioProducer:(id<SAAudioProducer>)producer fillBuffer:(float *)output withNumFrames:(unsigned int)numBufferFrames
{
    
}

- (CGPoint)position
{
    const b2Vec2 &position = self.physicsBody->GetPosition();
    return CGPointMake(position.x, position.y);
}

- (float)speed
{
    const b2Vec2 &velocity = self.physicsBody->GetLinearVelocity();
    return velocity.Length();
}

#pragma mark Control

- (void)startMoving:(CGPoint)direction
{
    self.physicsBody->ApplyForceToCenter(10000 * b2Vec2(direction.x, direction.y));
}

- (void)stopMoving:(CGPoint)direction
{
    
}


@end
