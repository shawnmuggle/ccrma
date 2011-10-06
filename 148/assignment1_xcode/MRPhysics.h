//
//  Physics.h
//  Assignment1
//
//  Created by Michael Rotondo on 10/1/11.
//  Copyright 2011 Stanford. All rights reserved.
//

#ifndef Assignment1_Entity_h
#define Assignment1_Entity_h

#include <vector>

#include "MRMath.h"

class PhysicsEntity
{
public:
    virtual void update();
    void addForce(Vector3<float> force);

    // These should maybe be protected via getters/setters?
    Vector3<float> position;
    Vector3<float> velocity;
protected:
    void applyVelocityToPosition();
};

class PhysicsEngine
{
public:
    void update();
    void addEntity(PhysicsEntity *entity);
private:
    std::vector<PhysicsEntity *> entities;
};


#endif
