//
//  Physics.cpp
//  Assignment1
//
//  Created by Michael Rotondo on 10/2/11.
//  Copyright 2011 Stanford. All rights reserved.
//

#include <iostream>

#include "MRPhysics.h"

using namespace std;

void PhysicsEntity::addForce(Vector3<float> force)
{
    velocity += force;
//    printf("Force: %f, %f, %f\n", force.x, force.y, force.z);
//    printf("Velocity: %f, %f, %f\n", velocity.x, velocity.y, velocity.z);
//    printf("---\n");
}

void PhysicsEntity::update()
{
    applyVelocityToPosition();
    velocity *= 0.9;
}

void PhysicsEntity::applyVelocityToPosition()
{
    position += velocity;
}

void PhysicsEngine::update()
{
    for (vector<PhysicsEntity *>::iterator i = entities.begin(); i != entities.end(); i++)
    {
        PhysicsEntity * entity = *i;
        entity->update();
    }
}

void PhysicsEngine::addEntity(PhysicsEntity *entity)
{
    entities.push_back(entity);
}