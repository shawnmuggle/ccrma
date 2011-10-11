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


class SplinePoint : public PhysicsEntity, public DrawableEntity
{
public:
    SplinePoint(Vector3<float> position);
    virtual void draw();
    virtual void update();
    void setTangentWithControlPoint(Vector3<float> controlPoint);
    float slope();
    void drawHermiteCurveToNextPoint(int numInterpolatedPoints);
    void drawBezierCurveToNextPoint(int numInterpolatedPoints);
    
    SplinePoint *nextPoint;
    SplinePoint *prevPoint;
private:
    Vector3<float> position;
    Vector3<float> tangentVector;
};

class Spline : public DrawableEntity
{
public:
    Spline();
    ~Spline();
    void addPoint(Vector3<float> position);
    virtual void draw();
//private:
    SplinePoint *firstPoint;
    SplinePoint *lastPoint;
};

class Blob2D : public PhysicsEntity, public DrawableEntity
{
public:
    Blob2D(Vector3<float> aPosition);
    void generatePoints();
    virtual void draw();
    virtual void update();
    void addPoint(Vector3<float> point);
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
    void mouseButton(int button, int state, int x, int y);
    void setTargetPoint(float x, float y);
private:
    int width;
    int height;
    bool mouseDown;
    Vector3<float> targetPoint;
    
    std::vector<Blob2D *> blobs;
    
    Spline spline;
};


#endif
