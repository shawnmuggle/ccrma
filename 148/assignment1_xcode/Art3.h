//
//  Art3.h
//  Assignment1
//
//  Created by Michael Rotondo on 10/6/11.
//  Copyright 2011 Rototyping. All rights reserved.
//

#ifndef Assignment1_Art3_h
#define Assignment1_Art3_h

#include <vector>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "MRMath.h"
#include "MRPhysics.h"
#include "MRGraphics.h"

class Blob2D : public PhysicsEntity, public DrawableEntity
{
public:
    Blob2D(Vector3<float> aPosition);
    void generatePoints();
    virtual void draw();
    virtual void update();
private:
    std::vector< Vector3<float> > points;
    std::vector< Vector3<float> > initialPoints;
    float size;
};

class Art3
{
public:
    Art3(int aWidth, int aHeight);
    ~Art3();
    void update();
    void draw();
    void setTargetPoint(float x, float y);
private:
    Vector3<float> targetPoint;
    Vector3<float> prevTargetPoint;
    int width;
    int height;
    
    std::vector<Blob2D *> blobs;
};


#endif
