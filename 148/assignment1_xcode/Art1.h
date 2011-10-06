//
//  Art1.h
//  Assignment1
//
//  Created by Michael Rotondo on 10/2/11.
//  Copyright 2011 Stanford. All rights reserved.
//

#ifndef Assignment1_Art1_h
#define Assignment1_Art1_h

#include <vector>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "MRPhysics.h"
#include "MRGraphics.h"

class TriangleThing : public PhysicsEntity, public DrawableEntity
{
public:
    TriangleThing(float x, float y, float z);
    virtual void update();
    virtual void draw();
//    void drawVBO(GLvoid *data);
    
    Vector3<float> home;
};

class Art1
{
public:
    Art1(int aWidth, int aHeight);
    ~Art1();
    void update();
    void draw();
    void setTargetPoint(float x, float y);
private:
    PhysicsEngine *physicsEngine;
    std::vector<TriangleThing *> triangleThings;
    Vector3<float> targetPoint;
    int width;
    int height;
    int numTriangles;
//    GLuint trianglesVBO;
};

#endif
