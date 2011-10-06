//
//  Art2.h
//  Assignment1
//
//  Created by Michael Rotondo on 10/6/11.
//  Copyright 2011 Rototyping. All rights reserved.
//

#ifndef Assignment1_Art2_h
#define Assignment1_Art2_h

#include <vector>

#include "MRMath.h"
#include "MRPhysics.h"
#include "MRGraphics.h"

class ConnectedVertex : public PhysicsEntity, public DrawableEntity
{
public:
    ConnectedVertex(Vector3<float> aPosition, ConnectedVertex *connection);
    ConnectedVertex(Vector3<float> aPosition);
    virtual void draw();
    virtual void update();
private:
    std::vector<ConnectedVertex *> connections;
};

class Art2
{
public:
    Art2(int aWidth, int aHeight);
    ~Art2();
    void update();
    void draw();
    void setTargetPoint(float x, float y);
private:
    std::vector<ConnectedVertex *> vertices;
    std::vector<ConnectedVertex *> prevVertices;
    Vector3<float> targetPoint;
    Vector3<float> prevTargetPoint;
    int width;
    int height;
};

#endif
